#include "timer.h"

#include <stdio.h>
#include <sys/time.h>

#include "error.h"

#define MSEC_MOD 1000000

struct timeval set_time;

static int timer_distance(struct timeval start_time, struct timeval end_time,
              long int *res_s, long int *res_ms);

int timer_reset_time(void)
{
  gettimeofday(&set_time, NULL);
  return OK;
}

int timer_get_elapsed_time(long int *seconds, long int *milliseconds)
{
  struct timeval curr_time;
  gettimeofday(&curr_time, NULL);
#if 0
  printf("set_time: %ld %ld\n"
         "curr_time: %ld %ld\n", set_time.tv_sec, set_time.tv_usec,
                                 curr_time.tv_sec, curr_time.tv_usec);
#endif

  timer_distance(set_time, curr_time, seconds, milliseconds);

  return OK;
}

static int timer_distance(struct timeval start_time, struct timeval end_time,
              long int *res_s, long int *res_ms)
{
  
  *res_ms = end_time.tv_usec - start_time.tv_usec;
  if (*res_ms < 0)
  {
    end_time.tv_sec--;
    *res_ms += MSEC_MOD;
  }
  *res_ms /= 1000;
  *res_s = 0;
  *res_s = end_time.tv_sec - start_time.tv_sec;

  return OK;
}
