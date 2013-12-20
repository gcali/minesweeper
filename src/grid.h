#ifndef GRID_H
#define GRID_H

#define GRID_NUM(n) ((n)+1)
#define BOMB 15
#define FLAG 32
#define UNCOVERED 64

#define MOVE_FLAG 1
#define MOVE_REVEAL 2
#define MOVE_REVEAL_EXTENDED 3

typedef int **Grid;

#define grid_get_cell(grid, row, col) ((grid)[(row)][(col)])
#define grid_set_cell(grid, row, col, value) (((grid)[(row)][(col)]) = (value))

int grid_create(Grid *grid, int row_dim, int col_dim);
int grid_populate_random(Grid grid, int row_avoid, int col_avoid, int row_dim, int col_dim, int bombs);
int grid_destroy(Grid *grid, int row_dim, int col_dim);
int grid_create_result(Grid *result_grid, Grid original_grid, int row_dim, int col_dim);
#define GRIDS_NOTHING 0
#define  GRIDS_EXPLODED 1
#define GRIDS_WON 2
int grid_status(int *status, Grid grid, int row_dim, int col_dim);
int grid_get_num(int *num, int cell);
int grid_get_bombs_from_dimension(int *bombs, int row_dim, int col_dim);
int grid_get_remaining_bombs(int *bombs, Grid grid, int row_dim, int col_dim);
int grid_elaborate_move(Grid grid, int curr_row, int curr_col,
                                   int row_dim, int col_dim,
                                   int choice);

/*
int grid_debug_example_place(Grid grid, int row_dim, int col_dim);
*/

#endif
