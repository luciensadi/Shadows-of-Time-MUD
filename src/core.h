/* alice_core.cpp */

typedef struct {
	char *varname;
	char *value;
	bool protect;
	bool def;
} var_def;

char *getvar(char *varname, char *vardefault);
int setvar(char *varname, char *value);
char *get_pattern_matched();
/* Breaks lines, cleans up text and logs, then sends to respond2 */
char* respond( char* text );

/* saves internal variables to file */
void savevars( char* varfile ); 
/* saves default vars to a new file. Also loads defaults into memory. */
void loadvars( char* varfile ); 
/* initialize's the program. */
int initialize(char *init_file); 
/* de-initialize's the program */
extern "C"
{
void deinitialize();
}
