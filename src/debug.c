#include <stdio.h>
#include <stdlib.h>
#include "highscore.h"

int main(int argc, char *argv[])
{
  Score array[10];
  unsigned int dim = sizeof(array) / sizeof(*array);
  unsigned int i;

  highscore_init();

  highscore_print_debug();
  highscore_get_highscores(array, &dim);
  for (i = 0; i < dim; i++)
  {
    printf("%d | %s\n", array[i].time, array[i].name);
    free(array[i].name);
  }

  highscore_save();

  highscore_close_down();

  return 0;
}
  
