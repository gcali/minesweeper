#include <stdio.h>
#include <stdlib.h>
#include "highscore.h"
#include "interface.h"
#include "timer.h"

#define DEBUG_SCORE
#if 0
#define DEBUG_TIMER
#endif

int main(int argc, char *argv[])
{
#ifdef DEBUG_TIMER
  long int seconds;
  long int mseconds;
  int i;
#endif
#ifdef DEBUG_SCORE 
  Score array[10];
  unsigned int dim = sizeof(array) / sizeof(*array);
  unsigned int i;

  highscore_init();

  highscore_print_debug();
  highscore_get_highscores(array, &dim, 808);
  for (i = 0; i < dim; i++)
  {
    printf("%ld.%ld | %s\n", array[i].time.sec, array[i].time.msec, array[i].name);
  }

  highscore_save();

  interface_init();
  interface_print_scores(array, dim);
  highscore_destroy(array, dim);

  interface_close_down();

  highscore_close_down();

#endif

#ifdef DEBUG_TIMER
  timer_reset_time();
  for (i = 0; i < 10000; i++)
    printf(".");
  putchar('\n');
  timer_get_elapsed_time(&seconds, &mseconds);
  printf("sec: %ld msec: %ld\n", seconds, mseconds);
#endif
  
  return 0;
}
  
