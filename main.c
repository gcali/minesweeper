#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "error.h"
#include "grid.h"

static char *help_screen_string =
               "Usare i tasti freccia o 'h','j','k','l'\n"
               "per muoversi, 'f' per inserire una bandiera,\n"
               "Invio o 'rr' per rivelare la cella selezionata oppure\n"
               "'q' o 'x' per tornare alla schermata precedente o\n"
               "chiudere il programma.\n"
               "\n"
               "Nota: nel caso in cui venga scelta una dimensione della\n"
               "griglia eccessiva rispetto alle dimensioni del terminale\n"
               "l'interfaccia potrebbe non funzionare correttamente";

int new_game(int *result, int row_dim, int col_dim, int bombs);
int choose_dimension_bombs(int *row_dim, int *col_dim, int *bombs);

int main(int argc, char *argv[])
{
  int retval;
  int result;

  retval = interface_init();
  return_if_error(retval);

  while (1)
  {
    interface_reset_screen();
    {
      char *(choices[]) = {"Nuovo Gioco", "Aiuto", "Esci"};
      int number_of_choices = sizeof(choices) / sizeof(*choices);
      char *title = "Campo Minato";

      retval = interface_screen_choices(&result, choices, number_of_choices, title);
    }

    if (result == 0)
    {
      /*New game*/
      int row_dim, col_dim;
      int bombs;
      retval = choose_dimension_bombs(&row_dim, &col_dim, &bombs);
      if (retval != INTERFACE_EXIT)
      {
        retval = new_game(&result, row_dim, col_dim, bombs);
      /*
        if (retval == INTERFACE_EXIT)
          break;
      */
      }
    /*
      {
        char *result_string;

        if (retval == INTERFACE_EXIT)
          result_string = "Gioco Abbandonato";
        else if (result == GRIDS_EXPLODED)
          result_string = "Sconfitta";
        else
          result_string = "Vittoria";

        interface_print_result(result_string);
      }
    */
    }
    else if (result == 1)
    {
      /*Help*/
      interface_print_help(help_screen_string);
    /*
      interface_print_help("Schermata\n"
                           "di aiuto");
    */
    }
    else
      break;
  }

  interface_close_down();

  return 0;
}


int choose_dimension_bombs(int *row_dim, int *col_dim, int *bombs)
{
  int result;
  int retval;
  char *(choices[]) = { "Piccolo\t (08x08)", "Medio\t (12x12)", "Grande\t (16x24)" };
  int lines = sizeof(choices) / sizeof(*choices);

  retval = interface_screen_choices(&result, choices, lines, "Scegli la dimensione");
  return_if_error(retval);

/*
  if (result == 0)
  {
    *dimension = 8;
    *bombs = 10;
  }
  else if (result == 1)
  {
    *dimension = 12;
    *bombs = 26;
  }
  else if (result == 2)
  {
    *dimension = 16;
    *bombs = 52;
  }
  else
    return INTERFACE_EXIT;
*/

  if (result == 0)
  {
    *row_dim = 8;
    *col_dim = 8;
  }
  else if (result == 1)
  {
    *row_dim = 12;
    *col_dim = 12;
  }
  else if (result == 2)
  {
    *row_dim = 16;
    *col_dim = 24;
  }
  else
    return INTERFACE_EXIT;

  grid_get_bombs_from_dimension(bombs, *row_dim, *col_dim);

  return OK;
}

int new_game(int *result, int row_dim, int col_dim, int bombs)
{
  int curr_row, curr_col;
  Grid grid;
  int choice;
  int game_ended = 0;
  int grid_initialized = 0;

  grid_create(&grid, row_dim, col_dim);
/*
  grid_debug_example_place(grid, row_dim, col_dim);
*/
/*
  grid_populate_random(grid, row_dim/2, col_dim/2, row_dim, col_dim, bombs);
*/

  curr_row = row_dim / 2;
  curr_col = col_dim / 2;

  do
  {
    int status;
    int retval;
    retval = interface_grid_next_move(&curr_row, &curr_col, &choice, grid, row_dim, col_dim);
    if (retval == INTERFACE_EXIT)
    {
      grid_destroy(&grid, row_dim, col_dim);
      return INTERFACE_EXIT;
    }
    else
    {
      if (!grid_initialized)
      {
        grid_populate_random(grid, curr_row, curr_col, row_dim, col_dim, bombs);
        grid_initialized = 1;
      }
      grid_elaborate_move(grid, curr_row, curr_col, row_dim, col_dim, choice);
    }
    grid_status(&status, grid, row_dim, col_dim);
    if (status == GRIDS_EXPLODED)
    {
      *result = GRIDS_EXPLODED;
      game_ended = 1;
    }
    else if (status == GRIDS_WON)
    {
      *result = GRIDS_WON;
      game_ended = 1;
    }
  } while (!game_ended);

  interface_reset_screen();

  interface_result_screen(grid, row_dim, col_dim, *result);

  grid_destroy(&grid, row_dim, col_dim);

  return OK;
}
