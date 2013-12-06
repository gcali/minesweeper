#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include "error.h"
#include "interface.h"
#include "utilities.h"
#include "grid.h"

/*Internal variables*/
static int interface_left_margin = 2;
static int interface_up_margin = 1;
static int row_debug;
static int col_debug;

static WINDOW *debug;

#define COL_DEF 1
#define COL_FLAG 2
#define COL_BOMB 3
#define COL_NUM 4
#define COL_COVER 5
#define COL_SELECT 6
#define COL_RES_FLAG 7

#define DEBUG 0

#define interface_cell_get_number(cell) ((cell) % 16)

/*
#define ALL_LINES
*/

/*Internal functions prototypes*/

/*
static int interface_w_main_screen_print_title(WINDOW **wtitle, int wtitle_start);
static int interface_w_main_screen_print_list(WINDOW **wlist, int *lines, int wlist_start);
*/
static int interface_w_create_title(WINDOW **wtitle, int wtitle_start, char *title);
static int interface_w_create_list(WINDOW **wlist, int wlist_start,
                                   char *(wlist_entries[]), int number_of_entries);

#define INVERT_DEFAULT 12
static int interface_w_invert_line_min(WINDOW *window, int not_until);
#define interface_w_invert_line(window)\
       (interface_w_invert_line_min((window), INVERT_DEFAULT))

#define interface_window_row_from_grid_row(row_dim) ((row_dim) * 2 + 1)
#define interface_window_col_from_grid_col(col_dim) ((col_dim) * 4 + 1)
static int interface_w_set_grid(WINDOW **wgrid, int row_dim, int col_dim);
static int interface_w_print_grid(WINDOW *wgrid, Grid grid, int row_dim, int col_dim);

/*
static int interface_get_char_attribute_from_cell(int *c, int *attribute, Grid grid, int row, int col);
*/

static int interface_w_print_separator_line(WINDOW *wgrid, int col_dim);
static int interface_w_print_grid_line(WINDOW *wgrid, int col_dim, Grid grid, int grid_row);
static int interface_w_print_cursor(WINDOW *wgrid, Grid grid, int row, int col);
static int interface_w_set_print_contextual_help(WINDOW **wcont_help, int col_dim, int bombs);
static int interface_print_cheat(WINDOW *wgrid, Grid grid, int row_dim, int col_dim);

static void interface_sigwinch_adjust(int sig);

/*Global functions definitions*/
int interface_init(void)
{
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  cbreak();
  curs_set(0);
  start_color();
  init_pair(COL_DEF, COLOR_WHITE, COLOR_BLACK);
  bkgd(' ' | COLOR_PAIR(COL_DEF));
  init_pair(COL_FLAG, COLOR_RED, COLOR_WHITE);
  init_pair(COL_BOMB, COLOR_BLACK, COLOR_RED);
  init_pair(COL_NUM, COLOR_WHITE, COLOR_BLACK);
  init_pair(COL_COVER, COLOR_BLACK, COLOR_WHITE);
  init_pair(COL_SELECT, COLOR_WHITE, COLOR_GREEN);
  init_pair(COL_RES_FLAG, COLOR_BLACK, COLOR_BLUE);
  erase();
  refresh();

  signal(SIGWINCH, interface_sigwinch_adjust);

  if (DEBUG)
  {
    debug = newwin(2, COLS - interface_left_margin, LINES - 2, interface_left_margin);
    wrefresh(debug);
    row_debug = 0;
    col_debug = 0;
    wmove(debug, row_debug, col_debug);
  }

  return OK;

}

int interface_close_down(void)
{
  refresh();
  endwin();
  return OK;
}

int interface_reset_screen(void)
{
  erase();
  touchwin(stdscr);
  refresh();
  return OK;
}

int interface_print_debug(char *string)
{
  if (DEBUG) 
  {
    werase(debug);
    wmove(debug, row_debug, col_debug);
    wprintw(debug, string);
    wrefresh(debug);
    getch();
  }
  return OK;
}

int interface_screen_choices(int *result,
                             char *(choices_list[]), int number_of_choices,
                             char *title)
{
  WINDOW *wtitle, *wlist;

  int wtitle_start, wlist_start;

  int row;
  int retval;
  int c;
  int wlist_lines = number_of_choices;

  wtitle_start = interface_up_margin;
  interface_w_create_title(&wtitle, wtitle_start, title);
  wrefresh(wtitle);

  wlist_start = wtitle_start + 2;
  retval = interface_w_create_list(&wlist, wlist_start, choices_list, number_of_choices);
  return_if_error(retval);

  row = 0;
  wmove(wlist, row, 0);
  interface_w_invert_line(wlist);
  wrefresh(wlist);

  while ((c = getch()) != '\n' && c != '\r' && c != KEY_ENTER &&
          c != ' ' && c != 'q' && c != 'x')
  {
    switch (c)
    {
      
      case KEY_UP:
      case 'k':
        if (row > 0 && row < wlist_lines)
        {
          interface_w_invert_line(wlist);
          row--;
          wmove(wlist, row, 0);
          interface_w_invert_line(wlist);
          wrefresh(wlist);
        }
        break;

      case KEY_DOWN:
      case 'j':
        if (row >= 0 && row < wlist_lines - 1)
        {
          interface_w_invert_line(wlist);
          row++;
          wmove(wlist, row, 0);
          interface_w_invert_line(wlist);
          wrefresh(wlist);
        }
        break;

      default:
        break;
    }
    move(0,0);
  }

  werase(wtitle);
  wrefresh(wtitle);
  delwin(wtitle);

  werase(wlist);
  wrefresh(wlist);
  delwin(wlist);

  /*
  touchwin(stdscr);
  */
  refresh();

  if (c == 'q' || c == 'x')
  {
    *result = INTERFACE_EXIT;
    return OK;
  }
  else
  {
    *result = row;
    return OK;
  }
}

int interface_grid_next_move(int *row, int *col, int *flag_or_reveal,
                             Grid grid, int row_dim, int col_dim)
{
  int c;
  int bombs;

  WINDOW *wgrid;
  WINDOW *wcont_help;

  interface_w_set_grid(&wgrid, row_dim, col_dim);
  interface_w_print_grid(wgrid, grid, row_dim, col_dim);
  interface_w_print_cursor(wgrid, grid, *row, *col);
  wrefresh(wgrid);
  grid_get_remaining_bombs(&bombs, grid, row_dim, col_dim);
  interface_w_set_print_contextual_help(&wcont_help, col_dim, bombs);
  wrefresh(wcont_help);

  /*
  while ((c = getch()) != '\n' && c != '\r' && c != KEY_ENTER &&
          c != ' ' && c != 'r' && c != 'f' &&
          c != 'd' && c != 'q' && c != 'x')
  */
  do
  {
    c = getch();
    switch (c)
    {
      case 'y':
        if (*col > 0)
          (*col)--;
      case KEY_UP:
      case 'k':
        if (*row > 0)
          (*row)--;
        break;
      
      case 'n':
        if (*col < col_dim - 1)
          (*col)++;
      case KEY_DOWN:
      case 'j':
        if (*row < row_dim - 1)
          (*row)++;
        break;

      case 'b':
        if (*row < row_dim - 1)
          (*row)++;
      case KEY_LEFT:
      case 'h':
        if (*col > 0)
          (*col)--;
        break;

      case 'u':
        if (*row > 0)
          (*row)--;
      case KEY_RIGHT:
      case 'l':
        if (*col < col_dim - 1)
          (*col)++;
        break;

      case 'r':
        c = getch();
        break;

      case 'i':
        if ((c = getch()) == 'd' &&
            (c = getch()) == 'd' &&
            (c = getch()) == 'q' &&
            (c = getch()) == 'd')
          interface_print_cheat(wgrid, grid, row_dim, col_dim);

      default:
        break;
    }
    interface_w_print_grid(wgrid, grid, row_dim, col_dim);
    interface_w_print_cursor(wgrid, grid, *row, *col);
    wrefresh(wgrid);
    if (DEBUG)
    {
      werase(debug);
      wprintw(debug, "%d,%d: %d", *row, *col, grid_get_cell(grid, *row, *col));
      wrefresh(debug);
    }
  } while (c != '\n' && c != '\r' && c != KEY_ENTER &&
  
          c != ' ' && c != 'r' && c != 'f' &&
          c != 'd' && c != 'q' && c != 'x');
    

  werase(wgrid);
  /*
  wrefresh(wgrid);
  */
  werase(wcont_help);
/*
  wrefresh(wcont_help);
*/

  delwin(wgrid);
  delwin(wcont_help);

  /*
  touchwin(stdscr);
  */
  refresh();

  if (c == 'q' || c == 'x')
    return INTERFACE_EXIT;

  if (c == 'f')
    *flag_or_reveal = MOVE_FLAG;
  else
    *flag_or_reveal = MOVE_REVEAL;

  return OK;
}

int interface_result_screen(Grid grid, int row_dim, int col_dim, int result)
{
  /*result can be GRID_EXPLODED or GRIDS_WON*/
  Grid result_grid;
  WINDOW *wresult;

  grid_create_result(&result_grid, grid, row_dim, col_dim);

  interface_w_set_grid(&wresult, row_dim, col_dim);
  interface_w_print_grid(wresult, result_grid, row_dim, col_dim);
  wrefresh(wresult);

  getch();

  grid_destroy(&result_grid, row_dim, col_dim);


  werase(wresult);
  wrefresh(wresult);
  delwin(wresult);

  return OK;
}

int interface_print_help(char *help_string)
{
  WINDOW *whelp;

  whelp = newwin(LINES - interface_up_margin, COLS - interface_left_margin, interface_up_margin, interface_left_margin);

  wprintw(whelp, "Aiuto\n\n");

  wprintw(whelp, "%s", help_string);
  wrefresh(whelp);
  getch();
  
  delwin(whelp);

  return OK;
}

/*Internal functions definitions*/

static int interface_w_create_list(WINDOW **wlist, int wlist_start,
                                   char *(wlist_entries[]), int wlist_lines)
{
  int row;

  if (wlist_entries == NULL)
  {
    set_error(OOM);
    return ERROR;
  }

  *wlist = newwin(wlist_lines, COLS - interface_left_margin, wlist_start, interface_left_margin);
  wcolor_set(*wlist, COLOR_PAIR(COL_DEF), NULL);
  wbkgd(*wlist, COLOR_PAIR(COL_DEF) | ' ');

  wmove(*wlist, 0, 0);
  for (row = 0; row < wlist_lines; row++)
  {
    wmove(*wlist, row, 0);
    wprintw(*wlist, "%s", wlist_entries[row]);
  }

  return OK;

}

static int interface_w_invert_line_min(WINDOW *window, int not_until)
{
  
  int c;
  int col;
  int i;
  int row;
  int max_row, max_col;
  int inverse;
  int up_to;

  getyx(window, row, col);
  getmaxyx(window, max_row, max_col);

  for (i = max_col - 1; i >= 0 && i >= not_until; i--) {
    wmove(window, row, i);
    c = (winch(window));
    inverse = c & A_STANDOUT;
    c = c & A_CHARTEXT;
    if (inverse || (is_digitable(c) && c != ' '))
      break;
  }

  i++;

  wmove(window, row, 0);
  up_to = max(i, 4);

  for (i = 0, c = winch(window);
       i < up_to;
       i++, c = winch(window)) {
    waddch(window, (c ^ A_STANDOUT));
  }

  wmove(window, row, col);

  return OK;

}

static int interface_w_create_title(WINDOW **wtitle, int wtitle_start, char *title)
{
  *wtitle = newwin(2, COLS - interface_left_margin, wtitle_start, interface_left_margin);
  wcolor_set(*wtitle, COLOR_PAIR(COL_DEF), NULL);
  wbkgd(*wtitle, COLOR_PAIR(COL_DEF) | ' ');
  
  wmove(*wtitle, 0, 0);
  wprintw(*wtitle, title);
/*
  wrefresh(*wtitle);
*/

  return OK;
}

static int interface_w_set_grid(WINDOW **wgrid, int row_dim, int col_dim)
{
  int window_row_dim, window_col_dim;

  window_row_dim = interface_window_row_from_grid_row(row_dim);
  window_col_dim = interface_window_col_from_grid_col(col_dim);

/*
  window_row_dim = row_dim * 2 + 1;
  window_col_dim = col_dim * 4 + 1;
*/

  *wgrid = newwin(window_row_dim, window_col_dim, interface_up_margin, interface_left_margin);

  return OK;
}

static int interface_w_set_print_contextual_help(WINDOW **wcont_help, int col_dim, int bombs)
{
  int left_margin, up_margin;
/*
  char *text = "Usare i tasti freccia\n"
               "o 'h','j','k','l'\n"
               "per muoversi,\n"
               "'f' per segnalare\n"
               "Invio o 'rr' per rivelare\n"
               "'q' o 'x' per chiudere\n";
*/
/*
  char *text = "Use 'f' to flag a cell\n"
               "Use 'enter' to uncover a cell\n"
               "Use 'q' or 'x' to quit";
*/

  left_margin = interface_left_margin + interface_window_col_from_grid_col(col_dim) + 1;
  up_margin = interface_up_margin + 1;

  *wcont_help = newwin(10, 40, up_margin, left_margin);

  /*
  wprintw(*wcont_help, text);
  wprintw(*wcont_help, "\n\n\nBombe: %02d\n", bombs);
  */
  wprintw(*wcont_help, "Bombe: %02d\n", bombs);

  return OK;
}

static int interface_w_print_grid(WINDOW *wgrid, Grid grid, int row_dim, int col_dim)
{
  int window_row;
  int window_row_dim, window_col_dim;

  window_row_dim = row_dim * 2 + 1;
  window_col_dim = col_dim * 4 + 1;

  for (window_row = 1; window_row < window_row_dim - 1; window_row++)
  {
    if (window_row % 2 == 0)
    {
      wmove(wgrid, window_row, 0);
      interface_w_print_separator_line(wgrid, col_dim);
    }
    else
    {
      wmove(wgrid, window_row, 0);
      interface_w_print_grid_line(wgrid, col_dim, grid, window_row / 2);
    }
  }

/*
  wrefresh(wgrid);
*/

  return OK;
}

static int interface_w_print_separator_line(WINDOW *wgrid, int col_dim)
{
  int col;

  for (col = 0; col < col_dim; col++)
  {
#ifdef ALL_LINES
    if (col == 0)
      waddch(wgrid, ACS_LTEE);
    else
      waddch(wgrid, ACS_PLUS);
    waddch(wgrid, ACS_HLINE);
    waddch(wgrid, ACS_HLINE);
    waddch(wgrid, ACS_HLINE);
#else
    if (col == 0)
      waddch(wgrid, ' ');
    else
      waddch(wgrid, ' ');
    /*
      waddch(wgrid, '+');
    */
    waddch(wgrid, ' ');
    waddch(wgrid, ' ');
    waddch(wgrid, ' ');
#endif
  }
#ifdef ALL_LINES
  waddch(wgrid, ACS_RTEE);
#endif
/*
  waddch(wgrid, '+');
*/

  return OK;
}

static int interface_w_print_grid_line(WINDOW *wgrid, int col_dim, Grid grid, int grid_row)
{
  int col;

  for (col = 0; col < col_dim; col++)
  {
    int c;
    int attribute;
    int cell;

    cell = grid_get_cell(grid, grid_row, col);
    
    if (is_attr_on(cell, FLAG))
    {
      c = '!';
      if (is_attr_on(cell, BOMB) && is_attr_on(cell, UNCOVERED))
        attribute = COLOR_PAIR(COL_RES_FLAG);
      else if (is_attr_on(cell, UNCOVERED))
        attribute = COLOR_PAIR(COL_BOMB);
      else
        attribute = COLOR_PAIR(COL_FLAG);
    }
    else if (!is_attr_on(cell, UNCOVERED))
    {
      c = ' ';
      attribute = COLOR_PAIR(COL_COVER);
    }
    else if (is_attr_on(cell, BOMB))
    {
      c = ACS_DIAMOND;
      attribute = COLOR_PAIR(COL_BOMB);
    }
    else
    {
      int num;
      grid_get_num(&num, cell);
      /*
      fprintf(stderr, "%d\n", num);
      */
      if (num != 0)
        c = '0' + num;
      else
        c = ' ';
      attribute = COLOR_PAIR(COL_NUM);
    /*
      if (DEBUG)
      {
        wmove(debug, row_debug, col_debug);
        wprintw(debug, "%d,%d: %d", grid_row, col, cell);
        wrefresh(debug);
        getch();
      }
    */
    }


#ifdef ALL_LINES
    waddch(wgrid, ACS_VLINE);
#else
    waddch(wgrid, ' ');
#endif
    waddch(wgrid, ' ' | attribute);
    waddch(wgrid, c | attribute);
    waddch(wgrid, ' ' | attribute);
  }
  
#ifdef ALL_LINES
  waddch(wgrid, ACS_VLINE);
#endif

  return OK;
}

static int interface_w_print_cursor(WINDOW *wgrid, Grid grid, int row, int col)
{
  int win_row, win_col;
  int c = ':', attribute = COLOR_PAIR(COL_SELECT);

/*
  interface_get_char_attribute_from_cell(&c, &attribute, grid, row, col);
*/

  win_row = 1 + row * 2;
  win_col = 2 + col * 4;

  wmove(wgrid, win_row, win_col - 1);
  waddch(wgrid, c | attribute);

  wmove(wgrid, win_row, win_col + 1);
  waddch(wgrid, c | attribute);

  return OK;
}

static void interface_sigwinch_adjust(int sig)
{
  resize_term(LINES, COLS);
  wrefresh(curscr);
  signal(SIGWINCH, interface_sigwinch_adjust);
}


static int interface_print_cheat(WINDOW *wgrid, Grid grid, int row_dim, int col_dim)
{
  interface_result_screen(grid, row_dim, col_dim, GRIDS_EXPLODED);
  return 0;
}
  
