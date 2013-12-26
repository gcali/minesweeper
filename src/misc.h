#ifndef MISC_H
#define MISC_H

#include <stdio.h>

int get_up_until(char **dest, int *sep_found,
                 char *string, int *offset,
                 char separator[], int dim);
int get_num(int *num, FILE *stream);
#endif
