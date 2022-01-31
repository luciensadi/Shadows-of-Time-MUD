/*****************************
** Asmo's Protection Scheme **
******************************/

#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"

void system_check args (( void ));

char *checklog(long size)
{
  struct dirent **namelist;
  bool problem = FALSE;
  char buf[MSL];
  BUFFER *buffer;
  int n, x;
  char *txt;

  
  buffer = new_buf();

  add_buf(buffer, "Check Log Results:\n\r");
  n = scandir("/usr/local/www/shadowsoftime_html/secure/log", &namelist, 
0, 
alphasort);
  if (n < 0)
  {
    add_buf(buffer, "Scandir: no files.\n\r");
  }
  else
  {
    struct stat filestat;
    while(n--) 
    {
      sprintf(buf, "%s%s", "/usr/local/www/shadowsoftime_html/secure/log", 
namelist[n]->d_name);
      x = stat (buf, &filestat);
      if (x >= 0)
      {
        if (filestat.st_size > size)
        {
          problem = TRUE;
          sprintf(buf, "Warning %s is %ld bytes big.\n\r", 
	    namelist[n]->d_name, (long)filestat.st_size);
          add_buf(buffer, buf);
        }
      }
      free(namelist[n]);
    }
    free(namelist);
    
    if (!problem)
    {
//      logf("Problem = FALSE");
      sprintf(buf, "No files exceeding %ld bytes found.\n\r", size);
      add_buf(buffer, buf);
    }
  }
  txt = (char *) alloc_mem(strlen(buf_string(buffer)) + 1);
  strcpy (txt, buf_string(buffer));
  free_buf(buffer);
  return txt;  
}

void do_checklog(CHAR_DATA *ch, char *argument)
{
  char *txt;
  long size;

  if (!is_number(argument))
    size = 1000000;
  else
    size = atoi(argument);
  txt = checklog(size);
  page_to_char(txt, ch);
  free_string(txt);
}

void system_check(void)
{
  char *txt;
  txt = checklog(2500000);
  if (strstr(txt, "Warning"))
    system_note("System Note",NOTE_NOTE,"IMP", "Log Results", txt);
}
