#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "error.h"

int main(int argc, char *argv[])
{
  int retval;
  int result;
  char *choices[] = { "Nuovo Gioco", "Aiuto", "Esci" };
  int lines;

  lines = sizeof(choices) / sizeof(*choices);

  retval = interface_init();
  return_if_error(retval);
  retval = interface_main_screen(&result);
  interface_close_down();

  printf("%d - %d\n", retval, result);
  getchar();

  retval = interface_init();
  return_if_error(retval);
  retval = interface_screen_choices(&result, choices, lines, "Titolo");
  if (result == 0)
  {
    char *choices[] = { "Singolo Giocatore", "Multigiocatore" };
    int lines = sizeof(choices) / sizeof(*choices);
    retval = interface_screen_choices(&result, choices, lines, "Numero giocatori");
  }
  interface_close_down();

  printf("%d - %d\n", retval, result);

  return 0;
}
  
