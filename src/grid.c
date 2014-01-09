#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "error.h"
#include "grid.h"
#include "utilities.h"
#include "interface.h"

/*Internal functions prototypes*/

static int grid_uncover_zeros(Grid grid, int row, int col, int row_dim, int col_dim);
static int grid_uncover_surrounding(Grid grid, int row, int col, int row_dim, int col_dim);
/*
static int grid_chain_uncover(Grid grid, int row, int col, int row_dim, int col_dim);
*/
static int grid_get_surr_flags(int *flags, Grid grid, int row, int col, int row_dim, int col_dim);
static int random_number(int min, int max);
static int grid_get_surr_bombs(int *bombs, Grid grid, int row, int col, int row_dim, int col_dim);
static int grid_get_surr_things(int *num, int thing, Grid grid, int row, int col, int row_dim, int col_dim);
static int grid_flag_bombs(Grid grid, int row_dim, int col_dim);
/*
static int grid_debug_print_matrix(FILE *output, Grid grid, int row_dim, int col_dim);
*/

/*Global functions definitions*/

int grid_create(Grid *grid, int row_dim, int col_dim)
{
  int i, j;

  *grid = malloc(sizeof(int *) * row_dim);
  for (i = 0; i < row_dim; i++)
  {
    (*grid)[i] = malloc(sizeof(int) * col_dim);
    for (j = 0; j < col_dim; j++)
      (*grid)[i][j] = GRID_NUM(0);
  }
  return OK;
}
  
int grid_destroy(Grid *grid, int row_dim, int col_dim)
{
  int i;

  for (i = 0; i < row_dim; i++)
    free((*grid)[i]);

  free(*grid);

  *grid = NULL;
  
  return OK;
}

int grid_populate_random(Grid grid, int row_avoid, int col_avoid, int row_dim, int col_dim, int bombs)
{
  int curr_row, curr_col;
/*
  int i,j;
*/

  while (bombs)
  {
    curr_row = random_number(0, row_dim - 1);
    curr_col = random_number(0, col_dim - 1);
    if (!is_attr_on(grid_get_cell(grid, curr_row, curr_col), BOMB) &&
          !(curr_row >= row_avoid - 1 && curr_row <= row_avoid + 1 &&
            curr_col >= col_avoid - 1 && curr_col <= col_avoid + 1))
    {
      int cell;
      cell = grid_get_cell(grid, curr_row, curr_col);
      cell = turn_attr_on(cell, BOMB);
      grid_set_cell(grid, curr_row, curr_col, cell);
      bombs--;
    }
  }

/*
  grid_debug_print_matrix(stderr, grid, row_dim, col_dim);
*/

  for (curr_row = 0; curr_row < row_dim; curr_row++)
    for (curr_col = 0; curr_col < col_dim; curr_col++)
      if (!is_attr_on(grid_get_cell(grid, curr_row, curr_col), BOMB))
      {
        int cell;
        int num;
        grid_get_surr_bombs(&num, grid, curr_row, curr_col, row_dim, col_dim);
        cell = turn_attr_on(0, GRID_NUM(num));
        grid_set_cell(grid, curr_row, curr_col, cell);
      }

/*
  grid_debug_print_matrix(stderr, grid, row_dim, col_dim);
*/

/*
  curr_row = 3;
  curr_col = 3;

  grid[curr_row][curr_col] = BOMB;

  for (i = curr_row - 1; i <= curr_row + 1; i++)
    for (j = curr_col - 1; j <= curr_col + 1; j++)
      if (i != curr_row || j != curr_col)
        grid[i][j] = GRID_NUM(1);

  curr_row = 6;
  curr_col = 3;

  grid[curr_row][curr_col] = BOMB;

  for (i = curr_row - 1; i <= curr_row + 1; i++)
    for (j = curr_col - 1; j <= curr_col + 1; j++)
      if (i != curr_row || j != curr_col)
        grid[i][j] = GRID_NUM(1);

*/


/*
  grid[0][0] = GRID_NUM(0) | UNCOVERED;
  grid[0][1] = GRID_NUM(0);

  grid[1][0] = GRID_NUM(5) | UNCOVERED;
  grid[1][1] = GRID_NUM(5);

  grid[2][0] = FLAG;
  grid[2][1] = BOMB;
  grid[2][2] = BOMB;
*/
/*
  grid[2][2] = BOMB | UNCOVERED;
*/

  return 0;
}

/*
int grid_debug_example_place(Grid grid, int row_dim, int col_dim)
{
  grid[0][0] = GRID_NUM(0) | UNCOVERED;
  grid[0][1] = GRID_NUM(0);

  grid[1][0] = GRID_NUM(5) | UNCOVERED;
  grid[1][1] = GRID_NUM(5);

  grid[2][0] = FLAG;
  grid[2][1] = BOMB;
  grid[2][2] = BOMB;

  return 0;
}
*/

int grid_get_num(int *num, int cell)
{
  int i;
  static int num_flags = 0;

  if (num_flags == 0)
    for (i = 0; i <= 9; i++)
      num_flags |= GRID_NUM(i);

  cell &= num_flags;

  for (i = 0; i <= 9; i++)
    if (cell == GRID_NUM(i))
    {
      *num = i;
      return OK;
    }

/*
  for (i = 0; i <= 9; i++)
    if (is_attr_on(cell, GRID_NUM(i)))
    {
      *num = i;
      return OK;
    }
*/

  *num = 0;

  return OK;
}

int grid_status(int *status, Grid grid, int row_dim, int col_dim)
{
  int i, j;
  int bomb_covered_found = 0;
  int non_bomb_flagged_found = 0;
  int covered_cell_not_bomb_found = 0;
  int cell;

  *status = GRIDS_NOTHING;

  i = 0;

  while (i < row_dim && *status == GRIDS_NOTHING)
  {
    j = 0;
    while (j < col_dim && *status == GRIDS_NOTHING)
    {
      cell = grid_get_cell(grid, i, j);
      if (is_attr_on(cell, BOMB))
      {
        if (is_attr_on(cell, UNCOVERED))
          *status = GRIDS_EXPLODED;
        else if (!is_attr_on(cell, FLAG))
          bomb_covered_found = 1;
      }
      else if (is_attr_on(cell, FLAG))
        non_bomb_flagged_found = 1;
      else if (!is_attr_on(cell, UNCOVERED))
        covered_cell_not_bomb_found = 1;
      j++;
    }
    i++;
  }

  /*
  if ((!covered_cell_not_bomb_found || !bomb_covered_found) &&
      !non_bomb_flagged_found &&
      *status == GRIDS_NOTHING)
  */
    /*
  if (!bomb_covered_found && !non_bomb_flagged_found &&
      *status == GRIDS_NOTHING)
    */
  /*
  if (!covered_cell_not_bomb_found && !non_bomb_flagged_found
      && *status == GRIDS_NOTHING)
  */
  if (!bomb_covered_found && !covered_cell_not_bomb_found &&
      !non_bomb_flagged_found && *status == GRIDS_NOTHING)
  {
    *status = GRIDS_WON;
    grid_flag_bombs(grid, row_dim, col_dim);
  }

  return OK;
}

int grid_elaborate_move(Grid grid, int curr_row, int curr_col,
                                   int row_dim, int col_dim, int choice)
{

  int cell;

  cell = grid_get_cell(grid, curr_row, curr_col);

  if (choice == MOVE_REVEAL && !is_attr_on(cell, FLAG))
  {
    if (!is_attr_on(cell, UNCOVERED))
    {
      int num;
      cell = turn_attr_on(cell, UNCOVERED);
      grid_set_cell(grid, curr_row, curr_col, cell);
      grid_get_num(&num, cell);
      /*
      {
        char string[20];
        sprintf(string, "num: %d", num);
        interface_print_debug(string);
      }
      */
      if (num == 0)
        grid_uncover_zeros(grid, curr_row, curr_col, row_dim, col_dim);
    }
    else
    {
      int num, flags;

      grid_get_num(&num, cell);
      grid_get_surr_flags(&flags, grid, curr_row, curr_col, row_dim, col_dim);
      if (num == flags)
        grid_uncover_surrounding(grid, curr_row, curr_col, row_dim, col_dim);
    }
  }
  else if (choice == MOVE_FLAG && !is_attr_on(cell, UNCOVERED))
  {
    cell =  toggle_attr(cell, FLAG);
    grid_set_cell(grid, curr_row, curr_col, cell);
  }

  return OK;
}

int grid_create_result(Grid *result_grid, Grid original_grid, int row_dim, int col_dim)
{
  int i, j;
  int cell;

  grid_create(result_grid, row_dim, col_dim);

  for (i = 0; i < row_dim; i++)
    for (j = 0; j < col_dim; j++)
    {
      cell = grid_get_cell(original_grid, i, j);
      cell = turn_attr_on(cell, UNCOVERED);
      grid_set_cell(*result_grid, i, j, cell);
    }

  return OK;
}

int grid_get_bombs_from_dimension(int *bombs, int row_dim, int col_dim)
{
  if (row_dim == 8)
    *bombs = 10;
  else if (row_dim == 12)
    *bombs = 26;
  else if (row_dim == 16)

/*
    *bombs = 78;
*/
#if 0
    *bombs = 52;
#endif
    *bombs = 60;
  else if (row_dim == 2)
    *bombs = 0;

  return 0;
}

int grid_get_data_from_id(int *row_dim, int *col_dim, int *bombs, int id)
{
  if (id == 0)
  {
    *row_dim =
    *col_dim = 8;
    *bombs = 10;
  }
  else if (id == 1)
  {
    *row_dim =
    *col_dim = 12;
    *bombs = 26;
  }
  else if (id == 2)
  {
    *row_dim = 16;
    *col_dim = 24;
    *bombs = 60;
  }
  else
  {
    set_error(WRONG_INPUT);
    return ERROR;
  }

  return OK;
}

int grid_get_remaining_bombs(int *bombs, Grid grid, int row_dim, int col_dim)
{
  int bomb_total;
  int curr_row, curr_col;

  grid_get_bombs_from_dimension(&bomb_total, row_dim, col_dim);

  for (curr_row = 0; curr_row < row_dim; curr_row++)
    for (curr_col = 0; curr_col < col_dim; curr_col++)
      if (is_attr_on(grid_get_cell(grid, curr_row, curr_col), FLAG))
        bomb_total--;

  *bombs = bomb_total;

  return OK;
}


/*Internal functions definitions*/

static int grid_uncover_zeros(Grid grid, int row, int col, int row_dim, int col_dim)
{
  int i, j;
  int cell;

  for (i = row - 1; i <= row + 1; i++)
    for (j = col - 1; j <= col + 1; j++)
      if (i < row_dim && j < col_dim && i >= 0 && j >= 0)
      {
        int num;
        cell = grid_get_cell(grid, i, j);
        grid_get_num(&num, cell);

        if ((cell == GRID_NUM(0)) &&
            !is_attr_on(cell, UNCOVERED))
        {
          cell = turn_attr_on(cell, UNCOVERED);
          grid_set_cell(grid, i, j, cell);
          grid_uncover_zeros(grid, i, j, row_dim, col_dim);
        }
        cell = turn_attr_on(cell, UNCOVERED);
        grid_set_cell(grid, i, j, cell);
      }
  
  return OK;
}

static int grid_get_surr_flags(int *flags, Grid grid, int row, int col, int row_dim, int col_dim)
{
  return grid_get_surr_things(flags, FLAG, grid, row, col, row_dim, col_dim);
}

static int grid_get_surr_bombs(int *bombs, Grid grid, int row, int col, int row_dim, int col_dim)
{
  return grid_get_surr_things(bombs, BOMB, grid, row, col, row_dim, col_dim);
}

static int grid_get_surr_things(int *num, int thing, Grid grid, int row, int col, int row_dim, int col_dim)
{
  int i, j;
  int count;

  count = 0;
  for (i = row - 1; i <= row + 1; i++)
    for (j = col - 1; j <= col + 1; j++)
      if (i >= 0 && j >= 0 && i < row_dim && j < col_dim)
        if (i != row || j != col)
          if (is_attr_on(grid_get_cell(grid, i, j), thing))
            count++;

  *num = count;

  return OK;
}

static int grid_uncover_surrounding(Grid grid, int row, int col, int row_dim, int col_dim)
{
  int i, j;

  for (i = row - 1; i <= row + 1; i++)
    for (j = col - 1; j <= col + 1; j++)
      if (i >= 0 && j >= 0 && i < row_dim && j < col_dim && (i != row || j != col))
      {
        int cell;
        cell = grid_get_cell(grid, i, j);
        if (!is_attr_on(cell, FLAG) && !is_attr_on(cell, UNCOVERED))
          grid_elaborate_move(grid, i, j, row_dim, col_dim, MOVE_REVEAL);
      }
  
  return OK;
}

static int random_number(int min, int max)
{
	static int initialized = 0;
  int divisor = RAND_MAX/(max - min +1);
  int retval;

	if (!initialized)
  {
		srand(time(NULL));
    initialized = 1;
  }


  do { 
    retval = rand() / divisor;
  } while (retval > max - min);

  return retval + min;
	
}
/*
static int grid_debug_print_matrix(FILE *output, Grid grid, int row_dim, int col_dim)
{
  int i, j;

  for (i = 0; i < row_dim; i++, putc('\n', output))
    for (j = 0; j < col_dim; j++)
      fprintf(output, "%02d ", grid_get_cell(grid, i, j));

  putc('\n', output);

  return 0;
}


*/

static int grid_flag_bombs(Grid grid, int row_dim, int col_dim)
{
  int curr_row, curr_col;

  for (curr_row = 0; curr_row < row_dim; curr_row++)
    for (curr_col = 0; curr_col < col_dim; curr_col++)
    {
      int cell;
      cell = grid_get_cell(grid, curr_row, curr_col);
      if (is_attr_on(cell, BOMB))
      {
        cell = turn_attr_on(cell, FLAG);
        grid_set_cell(grid, curr_row, curr_col, cell);
      }
    }

  return OK;
}
