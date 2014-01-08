#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "interface.h"
#include "error.h"
#include "grid.h"
#include "timer.h"


#if 0
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
#endif
static char *help_screen_string =
               "Use the arrow keys or 'h', 'j', 'k', 'l' to move,\n"
               "'f' to flag the selected cell, Enter or 'rr' to uncover\n"
               "the selected cell and 'q' or 'x' to go back to the previous\n"
               "screen or quit the program.\n"
               "\n"
               "Important: if the selected board size is too large\n"
               "for the screen, the interface will probably not work\n"
               "correctly. The only solution is to close and reopen\n"
               "the program.\n";

#define SCORES_DIM 10

int new_game(int *result, int row_dim, int col_dim, int bombs);
int choose_dimension_bombs(int *row_dim, int *col_dim, int *bombs);

int main(int argc, char *argv[])
{
  int retval;
  int result;

  highscore_init();

  retval = interface_init();
  return_if_error(retval);

  while (1)
  {
    interface_reset_screen();
    {
      #if 0
      char *(choices[]) = {"Nuovo Gioco", "Aiuto", "Esci"};
      #endif
      char *(choices[]) = {"New Game", "Help", "Exit"};
      int number_of_choices = sizeof(choices) / sizeof(*choices);
      char *title = "MineSweeper";

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
    }
    else if (result == 1)
    {
      /*Help*/
      interface_print_help(help_screen_string);
    }
    else
      break;
  }

  interface_close_down();

#if 0
  /*uncomment only if highscore starts to use dynamic memory allocation
    for the name part*/
  highscore_destroy(scores, scores_dim);
#endif
  highscore_save();
  highscore_close_down();

  return 0;
}


int choose_dimension_bombs(int *row_dim, int *col_dim, int *bombs)
{
  int result;
  int retval;
  char *(choices[]) = { "Small\t (08x08)", "Medium\t (12x12)", "Large\t (16x24)" };
  int lines = sizeof(choices) / sizeof(*choices);

  retval = interface_screen_choices(&result, choices, lines, "Choose the size");
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
  #if 1
    *row_dim = 8;
    *col_dim = 8;
  #else
    *row_dim =
    *col_dim = 2;
  #endif
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
  time_t start_time;
  time_t end_time;
  Time time;
  int score_id;
  Score scores[SCORES_DIM];
  unsigned int scores_dim = SCORES_DIM;

  score_id = row_dim * 100 + col_dim;
#if 0
  fprintf(stderr, "score_id: %d\n", score_id);
#endif

  grid_create(&grid, row_dim, col_dim);
/*
  grid_debug_example_place(grid, row_dim, col_dim);
*/
/*
  grid_populate_random(grid, row_dim/2, col_dim/2, row_dim, col_dim, bombs);
*/

  curr_row = row_dim / 2;
  curr_col = col_dim / 2;
  timer_reset_time();
#if 0
  time(&start_time);
#endif

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

  #if 0
  time(&end_time);
  #endif
  timer_get_elapsed_time(&(time.sec), &(time.msec));

  interface_reset_screen();

  interface_result_screen(grid, row_dim, col_dim, *result, time);

  if (*result == GRIDS_WON)
  {
    highscore_add_score(score_id, time, "\0");
    scores_dim = SCORES_DIM;
    highscore_get_highscores(scores, &scores_dim, score_id);
    interface_print_scores(scores, scores_dim);
    highscore_destroy(scores, scores_dim);
  }


  grid_destroy(&grid, row_dim, col_dim);

  return OK;
}
