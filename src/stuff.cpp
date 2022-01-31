/* Stuff.cpp */

#include "alice.h"
#include <ctype.h>

static char *cleanproc(char *line, int step)
{
	char symb;

	while (1) {
		symb = *line;
		if (isspace(symb)) {
			line += step;
			continue;
		}
		break;
	}

	return line;
}

void spacetrim(char *line)
{
	char *scrap;
	int len;

	len = strlen(line);
	if (!len) return;
	
	scrap = cleanproc(line + len - 1, -1);
	scrap[1] = 0;

	scrap = cleanproc(line, 1);
	if (scrap == line) return;
	memmove(line, scrap, strlen(scrap) + 1);
}

char *strlstr(char * line, char *token)
{
	char *pos = NULL;
	
	while ((line = strstr(line, token))) {
		pos = line;
		line++;
	}
	
	return(pos);
}

/* returns a random number from 0 to max. excludes MAX! */
/* this might cause a problem in randomize if we happened to have a list
that was the same size as RAND_MAX have to check into that */

int alirandom( int max ){
  return (int)((float)rand() * max / RAND_MAX);
}

void delay( int msecs )
{
	clock_t goal = clock() + msecs;
	while (goal > clock());
}

void strremove( char* text, char* first, char* last )
{
	char *pos1, *pos2;

	if (!first || !last || !text) return;

	if (!(pos1 = strstr( text, first ))) return;
	if (!(pos2 = strstr( pos1, last ))) return;
	if (pos1 >= pos2) return;
	
	pos2 += strlen(last);
 	memmove(pos1, pos2, strlen(pos2) + 1);
}


void uppercase( char* text ){
  char* temp = text - 1;
  while (*(++temp)) 
    if ((*temp >= 'a') && (*temp <= 'z')) 
      *temp += 'A' - 'a';
}

void lowercase( char* text ) {
  char* temp = text - 1;
  while (*(++temp)) 
    if ((*temp >= 'A') && (*temp <= 'Z')) 
      *temp -= 'A' - 'a';
}

int replace( char *string, char *strfrom, char *strto ) 
{
	char *pos, *tmp;
	int len, len2;
	
	if (!string || !strfrom || !strto) return(0);
	pos = strstr(string, strfrom);
	if (!pos) return(0);

	len = strlen(strfrom);
	len2 = strlen(strto);
	
	if (len > len2) {
		tmp = pos + len - len2;
		memmove(pos, tmp, strlen(tmp) + 1);
	}
	if (len < len2) {
		char *buf = new char[MAX_LINE_SIZE];
		int buflen;

		tmp = pos + len;
		buflen = strlen(tmp) + 1;
		memcpy(buf, tmp, buflen);
		memcpy(pos + len2, buf, buflen);

		delete [] buf;
	}

	strncpy(pos, strto, len2);
	return 1;
}

void cleaner(char *text) {
	char symb;
	char *oldtext = text;
	
	while ((symb = *text)) {
		if (isalnum(symb) || symb == ' ' || symb == '-')
			text++;
		else
			memmove(text, text + 1, strlen(text) + 1);
 	}

	spacetrim(oldtext);
}

