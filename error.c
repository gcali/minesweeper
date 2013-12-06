#include "error.h"

static int error_global = OK;

int set_error(int value) {

  error_global = value;

  return OK;

}

int get_error() {
  
  int error;

  error = error_global;

  error_global = OK;
  
  return error;

}
