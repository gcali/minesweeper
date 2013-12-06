/******************************************************************;
 * Funzioni di utilita' varia; riunite insieme perche' poche, se	*		
 * aumenteranno di numero verrano spezzettate in piu' file				*
 ******************************************************************/

#ifndef OD_UTILITIES
#define OD_UTILITIES
#include <stdio.h>

#define max(a,b) ( (a) > (b) ? (a) : (b) )
/*returns the max between a and b*/

int is_digitable(int c);
/*returns 1 if the character c is alphanumeric or a symbol usally used
  (see the table in the function's definition), 0 otherwise*/

#define is_alphanum(x) ((is_a_number(x)) || (is_a_letter(x)))

#define is_a_letter(x) (((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))

#define is_a_number(x) ((x) >= '0' && (x) <= '9')
/* Macro; prende un carattere x e restituisce 1 se e' un numero,	*
 * 0 altrimenti.																									*/

/*#define is_attr_on(var, attr) (((var) & (attr)) == (attr))*/
int is_attr_on(int var, int attr);
int toggle_attr(int var, int attr);
int turn_attr_off(int var, int attr);
int turn_attr_on(int var, int attr);

#define string_destroy(string) (free(string))

#define sign(x) ((x > 0) - (x < 0))

int cat_string(char **dest_addr, int number_of_strings, ...);
/*concatenates the number_of_strings given strings in *dest_addr;
  any content eventually stored in *dest_addr is lost. Behaviour
  undefined if number_of_strings and the number of given strings is
  different, or if something not a string is passed.
  Returns 1 if everything went fine, 0 and sets *dest_addr to NULL
  if an error was encountered*/

int fget_string_term(FILE *stream, char **string, int term);
/* Funzione che dato uno stream di input stream, l'indirizzo di		*
 * una stringa string e un carattere di terminazione term continua*
 * a prendere caratteri finche' non incontra EOF o il carattere		*
 * term. Se term e' il carattere ' ', smette di prendere caratteri*
 * anche se incontra un '\n'.	L'indirizzo cui punta string viene	*
 * perso, quindi usare solo indirizzi conservati anche da altri		*
 * puntatori, non inizializzati, o NULL se non lo si vuole perdere
 * 
 * Nota: il carattere di terminazione viene rimosso dal buffer		*/

#define get_string_term(string, term) (fget_string_term(stdin, string, term))
/* Ridefinita su fget_string_term, prende l'indirizzo di una			*
 * stringa string e il carattere di terminazione term. Lo stream	*
 * e' impostato automaticamente come stdin												*/

#define fget_string_line(stream, string) (fget_string_term(stream, string, '\n'))
/* Ridefinita su fget_string_term, prende uno stream di input			*
 * stream, l'indirizzo di una stringa string e smette di prendere	*
 * caratteri quando comincia una nuova riga.											*/

#define get_string_line(string) (fget_string_line(stdin, string))
/* Ridefinita su fget_string_line, lo stream e' impostato a stdin	*/

#define fget_string(stream, string) (get_string_term(stream, string, ' '))
/* Ridefinita su fget_string_term, prende uno stream di input			*
 * stream, un indirizzo di stringa string e continua a prendere		*
 * caratteri fino a che non incontra uno spazio o una nuova riga	*/

#define get_string(string) (get_string_term(stdin, string))
/* Ridefinita su fget_string, lo stream e' impostato a stdin 			*/

int is_string_a_number(char *string);
/* Restituisce uno se la stringa e' numerica, 0 altrimenti        */

int string_to_num(int *num, const char *string);
/* Prende una stringa string e l'indirizzo di un numero num e			*
 * la stringa in un numero che conserva nell'indirizzo puntato da *
 * num. Se la stringa contiene caratteri non numerici restituisce	*
 * 1, e imposta *num come se la stringa fosse troncata all'ultimo *
 * carattere numerico, altrimenti restituisce 0. Lavora anche con *
 * i negativi.	          																				*/


int pow_int(int base, int exp);
/* Prende una base intera ed un esponente positivo e restituisce  *
 * base ** esponente.                                             *
 * Attenzione: NON ci sono controlli di sorta sul range o altro;  *
 * l'unica verifica e' nel caso in cui l'esponente sia negativo,  *
 * il risultato in quel caso sarà 0.                              */

int last_digit(int num);
/* Prende un intero num e restituisce l'ultima cifra partendo da  *
 * destra.                                                        */

int number_of_digits(int num);
/* Prende un intero e restituisce il numero di cifre da cui e'    *
 * formato in base 10                                             */

int string_length(char *string);
/* Restituisce la lunghezza della stringa string ("abc" -> 3)     */

int string_append(char **string, int *dim, char c);
/* Aggiunge il carattere c alla stringa, allocando spazio se      *
 * necessario.                                                    */

int digit(int digit_number, int num); 
/* Restituisce la cifra digit_number-esima, partendo da sinistra, *
 * di num. Esempio: digit(0, 12) == 1, digit(2, 115) == 5         */

int file_exists(char *file);
/* Restituisce 1 se il file esiste, 0 se non esiste               */

int chomp(char *string);
/* Taglia l'ultimo carattere della stringa */


#endif
