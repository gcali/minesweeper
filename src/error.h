#ifndef ERROR_H
#define ERROR_H

#ifndef OK
  #define OK (0)
#endif
#ifndef ERROR
  #define ERROR (!OK)
#endif
#define OOM (2)
#define NOT_EXISTS (4)
#define INVALID_INPUT (8)
#define UNK (16)
#define TIMEOUT (32)
#define WRONG_INPUT INVALID_INPUT
#define OOR (64)
#define COLLISION (128)
#define END_OF_FILE (256)

/*pay *a lot* of attention when using this macro; it's just to save a
few ketstrokes*/
#define return_if_error(retval) if (((retval) == ERROR)) return ERROR
int set_error(int value);/*sets error to value*/
int get_error();/*returns error value*/

#endif
