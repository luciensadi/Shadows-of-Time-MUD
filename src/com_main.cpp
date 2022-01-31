#include <ctype.h>
#include <sys/time.h>
#include "alice.h"


#define LOWER(c)        ((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)        ((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))

void logf (const char *format, ...);

extern "C"
{
  char *com_main(char *argument, char *name);
}

char *com_main(char *argument, char *name){
  char *alans;
  char *response;
  char *input = NULL;
  char username[50];
  char *initfile = NULL;
  char *varname[25];
  char *varval[25];
  char *arg1;
  char *arg2;
 

  int i, j = 0;
  int overide_url = 0;
 
  sprintf(username, "%s", name);
  if (initfile == NULL) {
    if (initialize("../data/alice.ini")) return(NULL);
  }
  else if (initialize(initfile)) return(NULL);
  if (username == NULL) loadvars("../log/localuser.txt");
  else loadvars(username);
  for (i=0; i < j; i++) {
    setvar(varname[i],varval[i]);
  }
  response = respond( argument );
  if (!(overide_url)) stripurl(response);
//  logf( "%s", response );
  if (username == NULL) savevars("../log/localuser.txt");
  else savevars( username );
//  logf( "%s", response );
//  deinitialize();
//  logf( "%s", response );
  return response;
}  

void logf( const char *format, ... )
{
        va_list arglist;
        char *strtime;
        int status;
        va_start( arglist, format );
       time_t current_time;
       struct timeval now_time;  


       gettimeofday(&now_time, NULL);
     current_time = (time_t) now_time.tv_sec;
    strtime = ctime( &current_time );    
    strtime[strlen(strtime)-1] = '\0';
    fprintf( stderr, "%s :: ", strtime );
#ifdef DOUBLE_LOGGING
        fprintf( stdout, "%s :: ", strtime );
        vfprintf( stdout, format, arglist );
    fprintf( stdout, "\n" );
        fflush( stdout );
#endif                  
        status = vfprintf( stderr, format, arglist );
        va_end( arglist );
    fprintf( stderr, "\n" );
        fflush( stderr );
        return;
}   
