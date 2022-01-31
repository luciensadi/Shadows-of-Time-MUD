/* c_preproc.h
   This is the header file for the preproccesor. */

/* checks to see if a file is around. */
int checkfile(char *filepath);
/* checks a line of AIML for syntax errors. */
int checkline(char *line);
/* splits the aiml files into patterns.txt and templates.txt */
int prepare();

