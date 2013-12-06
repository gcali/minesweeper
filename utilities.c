#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#define BLOCK_SIZE_MIN 10
#define BLOCK_SIZE_MAX 400

#define STRING_CHUNK 50

int is_attr_on(int var, int attr)
{
  if (attr == 0)
  {
    return var == attr;
  }

  return (((var) & (attr)) == (attr));
}

int toggle_attr(int var, int attr)
{
  return var ^ attr;
}

int turn_attr_off(int var, int attr)
{
  return var & (~attr);
}

int turn_attr_on(int var, int attr)
{
  return var | attr;
}

int is_digitable(int c) {
  
  int dim = 32;
  int digitable_chars[] = {'\'', ' ', ',', '.', '-',
                           '_' , '*', '?', '!', '"',
                           '+' , '=', '@', '#', '$',
                           '%' , '^', '&', '(', ')',
                           '[' , ']', ';', ':', '{',
                           '\\', '}', '|', '/', '<',
                           '>' , '~'};
  /*please keep the table with 5 columns*/

  if (is_alphanum(c))
    return 1;

  {
    int i;
    for (i = 0; i < dim; i++) {
      if (c == digitable_chars[i])
        return 1;
    }

  }

  return 0;

}

int string_append(char **string, int *dim, char c) {

  if (*string == NULL) {
    *string = malloc(sizeof(char) * STRING_CHUNK);
    *dim = STRING_CHUNK;
    (*string)[0] = c;
    (*string)[1] = '\0';
    return 1;
  }

  if (strlen(*string) + 2 > *dim) {
   char *temp_string;
   temp_string = realloc(*string, (*dim) + STRING_CHUNK);
   *string = temp_string;
   *dim = *dim + STRING_CHUNK;
  }

  {
    int i;
    for (i = 0; (*string)[i] != '\0'; i++)
      ;
    (*string)[i] = c;
    (*string)[i+1] = '\0';
  }

  return 1;

}

int string_to_num(int *num, const char *string) {
	
	int i;
	int fact;

	*num = 0;
  fact = 1;

	for (i = 0; string[i] != '\0'; i++) {
		if (i == 0 && string[i] == '-')
			fact *= -1;
		else if (!is_a_number(string[i])) {
			return 1;
		}
		else {
			*num *= 10;
			*num += string[i] - '0';
		}
	}

  *num *= fact;

	return 0;

}

int fget_string_term(FILE *stream, char **string, int term) {

	int i;
	int c;
	int block_lim;

	block_lim = BLOCK_SIZE_MIN;
	i = 0;
	*string = malloc(sizeof(char) * (block_lim + 1));

	while ((c = getc(stream)) != term &&
				 (term != ' ' || c != '\n') &&
				 c != EOF) {
		if (i < block_lim)
			(*string)[i] = c;
		else {
			block_lim *= 2;
			if (block_lim > BLOCK_SIZE_MAX)
				return 1;
			(*string) = realloc((*string), sizeof(char) * (block_lim + 1));
			if (*string == NULL)
				return -1;
			(*string)[i] = c;
		}

		i++;
	}

	(*string)[i] = '\0';

	return 0;

}

int pow_int(int base, int exp) {
  
  int res;

  res = 1;

  if (exp < 0)
    return 0;

  while (exp >= 1) {
    
    res *= base;
    exp--;

  }

  return res;

}

int last_digit(int num) {
  
  while (num / 10 != 0)
    num /= 10;

  return num;

}

int number_of_digits(int num) {
  
  int digits;

  for (digits = 0; num != 0; digits++) {
    num /= 10;
  }

  return digits;

}

int string_length(char *string) {
  
  int i;

  for (i = 0; string[i] != '\0'; i++)
    ;

  return i;

}

int digit(int digit_number, int num) {
  
  num /= pow_int(10, digit_number);
  num %= pow_int(10, 2);

  return num;

}

int file_exists(char *file) {

  FILE *stream;

  stream = fopen(file, "r");

  return (stream == NULL ? 0 : 1);

}

int chomp (char *string) {

  string[strlen(string) - 1] = '\0';

  return 0;

}

int is_string_a_number(char *string) {
  
  int i;

  for (i = 0; string[i] != '\0'; i++) {
    if (!is_a_number(string[i]))
      if (i != 0 || string[i] != '-')
        return 0;
  }

  return 1;

}

int cat_string(char **dest_addr, int number_of_strings, ...)
{
  char *dest_string;
  char *curr_string;
  int dim;
  va_list argv;

  va_start(argv, number_of_strings);

  dim = STRING_CHUNK;

  dest_string = malloc(sizeof(char) * (dim + 1));
  if (dest_string == NULL)
    return 1;

  while (number_of_strings-- > 0)
  {
    curr_string = va_arg(argv, char *);
    while (strlen(dest_string) + strlen(curr_string) > dim)
    {
      char *temp_realloc;

      dim += STRING_CHUNK;
      temp_realloc = realloc(dest_string, sizeof(char) * (dim + 1));
      if (temp_realloc == NULL)
      {
        free(dest_string);
        return 1;
      }
    }
    strcat(dest_string, curr_string);
  }

  va_end(argv);

  *dest_addr = dest_string;

  return 0;
}
      

