/* Gadgets.cpp */

#include "alice.h"

char typo[26] = { 'S','H','X','F','W','G','T','J','O','K','L','K','N','B','P','O',
                  'W','E','A','Y','I','B','E','C','U','X' };

void stripurl( char* line ) {
  strremove( line, "<a href=", ">" );
  replace( line, "</a>", "" );
  strremove( line, "<A HREF=", ">" );
  replace( line, "</A>", "" );
  while(replace(line, "<br>", "\n"));
  while(replace(line, "<BR>", "\n"));
  replace(line, "<ul>", "" );
  while(replace(line, "<li>", "" ));
  while(replace(line, "</li>", "\n" ));
  replace(line, "</ul>", "" );
  strremove(line, "<APPLET", "</APPLET>");
  strremove(line,"<applet","</applet>");        
  replace(line,"&lt;","<");
  replace(line,"&gt;",">");
}

void typist( char* line ) {
  char* pos = line;
  char buffer[2];
  buffer[1] = 0;
  while (*pos) {
    delay( (int)(((float)rand() / (RAND_MAX * 20) + 0.03f) * CLOCKS_PER_SEC) );
    if (rand() < (RAND_MAX / 20)) {
      if ((*pos >= 'A') && (*pos <= 'Z'))
        buffer[0] = typo[*pos - 'A'];
      else if ((*pos >= 'a') && (*pos <= 'z')) 
        buffer[0] = typo[*pos - 'a'] - ('A' - 'a');
      else buffer[0] = *pos;
      printf( buffer );
      buffer[0] = 8;
      delay( (int)(((float)rand() / (RAND_MAX * 5) + 0.1f) * CLOCKS_PER_SEC) );
      printf( buffer );
    }
    else { 
      buffer[0] = *pos;
      printf( buffer );
      if (*pos == '.') 
        delay( (int)(((float)rand() / (RAND_MAX * 3) + 0.3f) * CLOCKS_PER_SEC));
      pos++;
    }
  }
}
