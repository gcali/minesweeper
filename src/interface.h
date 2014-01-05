#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include <time.h>
#include "grid.h"
#include "highscore.h"

#define INTERFACE_EXIT -1

int interface_init(void);
int interface_close_down(void);
int interface_reset_screen(void);
int interface_screen_choices(int *result,
                             char *(choices_list[]), int number_of_choices,
                             char *title);
int interface_grid_next_move(int *row, int *col, int *flag_or_reveal,
                             Grid grid, int row_dim, int col_dim);
int interface_result_screen(Grid grid, int row_dim, int col_dim, int result, time_t time);
int interface_print_help(char *help_string);
int interface_print_scores(Score array[], unsigned int dim);

int interface_print_debug(char *string);

#endif
