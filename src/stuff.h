/* c_stuff.h
   This is the header file for extra functions that make life
   a little easier.  */

#ifndef __STUFF__
#define	__STUFF__

/* gets a random number from 0 to max */
int alirandom( int max );
/* delays the appropiate # of miliseconds. */
void delay(int msecs);

/* makes the strings uppercase & lowercase. */
void uppercase(char *text);
void lowercase(char *text);

/* replaces string1 with string2 in line */
int replace(char *line, char *string1, char *string2);
/* removes all the text from first to last */
void strremove(char* text, char* first, char *last);
/* cleans up text. */
void cleaner(char* text);				
/* removes all spaces included '\t' '\v' ... */
void spacetrim(char *line);
/* finds string from end */
char *strlstr(char *line, char *token);

#endif
// __STUFF__
