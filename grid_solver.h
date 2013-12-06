#ifndef GRID_SOLVER_H
#define GRID_SOLVER_H

int grid_is_solvable(int *result, Grid grid, int dim_row, int dim_col);
int grid_find_next_move(int *dest_row, int *dest_col, Grid grid, int dim_row, int dim_col);

#endif
