/* Include file for C Alice */

#define MAX_FILENAME_SIZE 256   /* max size a filename and path can be */
#define MAX_VARNAME_SIZE  40    /* max size the name of a var can be   */
#define MAX_VARVAL_SIZE   512   /* max size the value of a var can be  */
#define MAX_LINE_SIZE     16386 /* max size a line of text can be      */
#define	MIN_LINE_SIZE	  1024  /* min size for a algetline	       */
#define MAX_RANDLIST_SIZE 200   /* max # of <li></li> in a random.     */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "core.h"
#include "respond.h"
#include "preproc.h"
#include "gadgets.h"
#include "stuff.h"
/*
typedef struct {
	char *start;
	int len;
} string;
*/
typedef struct _var_array { 
	int protect;
	int def;
	char *varname;
	char *value;
	struct _var_array *next;
} var_array;

extern char *star, *that, *pbuffer;

/* best holds the pattern matched */
extern char *best;

/* char's for our init file. */
extern FILE *aimlfile, *personfile, *person2file;
extern FILE *substitutefile, *templatefile, *patternfile;
extern FILE *logfile, *gossipfile, *personffile;
extern char *datafile, *unsortedfile;

extern fpos_t chunk[28];

extern var_array *root_var;

