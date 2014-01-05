#include "highscore.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "misc.h"
#include "utilities.h"

#define HIGHSCORE_FILE "/.minesweeper_scores"

/*internal typedefs and global variables*/
static FILE *highscore_stream = NULL;
#if 0
typedef struct ScoreStruct
{
  int time;
  char *name;
} Score;
#endif

static char *scores_file = NULL;

static char separator_arr[] =
  { '|', '\n' };
static int separator_dim =
  sizeof(separator_arr) / sizeof(*separator_arr);


typedef struct NodeStruct
{
  Score score;
  struct NodeStruct *next;
} Node;

typedef Node *List;

static List score_list = NULL;
#if 0
static int (*precedence)(int a, int b) = NULL;
#endif


/*internal function prototypes*/
static int highscore_init_scores_file_string();
static int highscore_import_file(FILE *file);
static int highscore_remove_first();
static int highscore_reverse_list();
static int highscore_standard_precedence(int a, int b);
static int highscore_import_precedence(int a, int b);


/*global function definitions*/
int highscore_init()
{

  if (scores_file == NULL)
    highscore_init_scores_file_string();

#if 0
  printf("scores_file: %s\n", scores_file);
#endif

  highscore_stream = fopen(scores_file, "r");
  if (highscore_stream == NULL)
    highscore_stream = fopen(scores_file, "w");
  else
    highscore_import_file(highscore_stream); 

  fclose(highscore_stream);

#if 0
  if (precedence == NULL)
    precedence = highscore_standard_precedence;
#endif

  return 0;
}

int highscore_save()
{
  FILE *file;
  List work_list;

  if (scores_file == NULL)
    highscore_init_scores_file_string();

#if 0
  highscore_reverse_list();
  precedence = highscore_import_precedence;
#endif
/*FIXME*/

  file = fopen(scores_file, "w");
  work_list = score_list;
  while (work_list != NULL)
  {
    fprintf(file, "%d|%d|%s\n", work_list->score.id, work_list->score.time, work_list->score.name);
    work_list = work_list->next;
  }

#if 0
  precedence = highscore_standard_precedence;
  highscore_reverse_list();
#endif

  fclose(file);

  return OK;
}
  
  
int highscore_close_down()
{
  while (score_list != NULL)
    highscore_remove_first();

  if (scores_file != NULL)
  {
    free(scores_file);
    scores_file = NULL;
  }

  return OK;
}

int highscore_destroy(Score array[], unsigned int nmemb)
{
  int i;

  for (i = 0; i < nmemb; i++)
    if (array[i].name != NULL)
    {
      free(array[i].name);
      array[i].name = NULL;
    }

  return OK;
}


int highscore_get_highscores(Score *array, unsigned int *nmemb, int id)
{
  unsigned int i;
  List work_list;

  i = 0;
  work_list = score_list;

  while (i < *nmemb && work_list != NULL)
  {
    if (work_list->score.id == id)
    {
      array[i].time = work_list->score.time;
      array[i].id = work_list->score.id;
      array[i].name = malloc(sizeof(char) * (strlen(work_list->score.name) + 1));
      strcpy(array[i].name, work_list->score.name);
      i++;
    }
    work_list = work_list->next;
  }

  if (i < *nmemb);
    *nmemb = i;

  return OK;
}

int highscore_add_score(int id, int time, char *name)
{
  Score score;
  #if 0
  fprintf(stderr, "%d - %d\n", id, time);
  #endif
  score.id = id;
  score.time = time;
  score.name = name;
  return highscore_add_score_struct(score);
}

int highscore_add_score_struct(Score score)
{
  List *work_list;
  List aux_list;

  work_list = &score_list;
  #if 0
  while (*work_list != NULL && (*precedence)((*work_list)->score.time, score.time))
  #endif
  /*FIXME*/
  while (*work_list != NULL && (*work_list)->score.id <= score.id && (*work_list)->score.time <= score.time)
    work_list = &((*work_list)->next);
  aux_list = malloc(sizeof(*aux_list));
  aux_list->score.id = score.id;
  aux_list->score.time = score.time;
  aux_list->score.name = malloc(sizeof(char) * (strlen(score.name) + 1));
  strcpy(aux_list->score.name, score.name);
  aux_list->next = *work_list;
  *work_list = aux_list;

  return OK;
}

int highscore_print_debug()
{
  List work_list;
  int i;
  
  work_list = score_list;
  i = 0;
  while (work_list != NULL)
  {
    printf("%02d) %d | %s\n", ++i, work_list->score.time, work_list->score.name);
    work_list = work_list->next;
  }

  printf("---\n");

  return OK;
}

/*internal function definitions*/
static int highscore_import_file(FILE *file)
{
  int time;
  int id;
  int found;
  int offset;
  char *string;
  char *line = NULL;

  while (1)
  { 
    fget_string_line(file, &line);
    if (feof(file))
    {
      if (line != NULL)
        free(line);
      break;
    }
#if 0
    printf("line: %s\n", line);
#endif

    offset = 0; 
    get_up_until(&string, &found, line, &offset, separator_arr, separator_dim);
    sscanf(string, "%d", &id);
    free(string);

    get_up_until(&string, &found, line, &offset, separator_arr, separator_dim);
    #if 0
    printf("time string: %s\n", string);
    #endif
    sscanf(string, "%d", &time);
    #if 0
    printf("time: %d\n", time);
    #endif
    free(string);

    get_up_until(&string, &found, line, &offset, separator_arr, separator_dim); 
    #if 0
    printf("name string: %s\n", string);
    #endif
    highscore_add_score(id, time, string);
    free(string);

    free(line);
    line = NULL;
  }

  return OK;
}

static int highscore_remove_first()
{
  if (score_list != NULL)
  { 
    List temp_list;
    temp_list = score_list->next;
    free(score_list->score.name);
    free(score_list);
    score_list = temp_list;
  }

  return OK;
}

static int highscore_init_scores_file_string()
{
  char *home;
  char *path;
  home = getenv("HOME"); 
  path = malloc(sizeof(char) * (strlen(home) +
                                strlen(HIGHSCORE_FILE) +
                                1));

  sprintf(path, "%s%s", home, HIGHSCORE_FILE);

  #if 0
  printf("path: %s\n", path);
  #endif
  scores_file = path;

  return OK;
}

static int highscore_reverse_list()
{
  List aux_list;

  if (score_list == NULL || score_list->next == NULL)
    return OK;

  aux_list = score_list;
  score_list = score_list->next;
  highscore_reverse_list();
  aux_list->next->next = aux_list;
  aux_list->next = NULL;

  return OK;
}

static int highscore_standard_precedence(int a, int b)
{
  return (a < b || a == b);
}

static int highscore_import_precedence(int a, int b)
{
  return a < b;
}
