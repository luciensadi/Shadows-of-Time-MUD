/*  c_respond.h
    This file holds the functions for the responder. */

/* makes alice's answer pretty. (cleans up text) */
void print( char *line ); 
/* makes substitutions on text. */
void substitute(FILE *subst, char *text);
/* gets a response. */
char* respond2( char *text);
