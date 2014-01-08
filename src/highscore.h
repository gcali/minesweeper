#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#define NMAX_HIGHSCORES 10
#define HIGHSCORE_MAX_STRING 20
#define HIGHSCORE_MAX_DIGITS 4

typedef struct TimeStruct
{
  long int sec;
  long int msec;
} Time;

typedef struct ScoreStruct
{
  int id;
  Time time;
  char *name;
} Score;

int highscore_init();
int highscore_save();
int highscore_close_down();
int highscore_destroy(Score array[], unsigned int nmemb);

int highscore_get_highscores(Score *array, unsigned int *nmemb, int id);
int highscore_add_score(int id, Time time, char *name);
int highscore_add_score_struct(Score score);

int highscore_print_debug();

#endif
