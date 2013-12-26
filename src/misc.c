#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

#ifdef is_digit
#undef is_digit
#endif
#define is_digit(c) ((c) >= '0' && (c) <= '9')

int get_up_until(char **dest, int *sep_found,
                 char *string, int *offset,
                 char separator[], int dim)
{
  int i;
  int done;

  *dest = malloc(sizeof(char) * (strlen(string) + 1));

  string += *offset;

  i = 0;
  #if 0
  printf("string: %s\n", string);
  printf("offset: %d\n", i);
  #endif
  done = 0;
  while (string[i] != '\0' && !done)
  {
    int j = -1;
    #if 0
    printf("char %d: %c\n", i, string[i]);
    #endif
    while (++j < dim && !done)
      if (string[i] == separator[j])
      {
        done = 1;
        *sep_found = separator[j];
      }
    if (!done)
    {
      (*dest)[i] = string[i];
      i++;
    }
  }

  if (!done)
    *sep_found = '\0';

  (*dest)[i] = '\0';
  *offset += i+1;

  *dest = realloc(*dest, sizeof(char) * (strlen(*dest) + 1));

  return OK;
}


int get_num(int *num, FILE *stream)
{
  int c;
  int work_num;

  work_num = 0;

  while ((c = getc(stream)) != EOF &&
         !is_digit(c))
    ;

  if (c == EOF)
  {
    *num = 0;
    set_error(WRONG_INPUT);
    return ERROR;
  }

  work_num = 0;

  do 
  {
    work_num *= 10;
    work_num += c - '0';
    c = getc(stream);
  }
  while (c != EOF && is_digit(c));

  *num = work_num;

  return OK;
}
