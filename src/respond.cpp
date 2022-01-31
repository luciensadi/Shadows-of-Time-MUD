/* c_respond.cpp */

#include "alice.h"

void substitute( FILE *subst,char* text ){
  if(!subst) return;
  uppercase(text);
  char *spaces = new char[MAX_LINE_SIZE];
  char *string1 = new char[MAX_LINE_SIZE];
  char *string2 = new char[MAX_LINE_SIZE];
  char *line = new char[MAX_LINE_SIZE];
  int pos1, pos2, pos3, pos4, l;
  unsigned int i;
  /* replace "'" with "`" */
  for ( i = 0; i < strlen( text ); i++ ) if (text[i] == '\'') text[i] = '`';
  strcpy( spaces + 1, text );
  spaces[0] = ' ';
  l = strlen( spaces );
  spaces[l] = ' ';
  spaces[l + 1] = 0;
  /* do replacing from our filename */
  while (!feof( subst )){
    fgets( line, MAX_LINE_SIZE, subst );
    /* allow comments. */
    if (strstr(line,"#")) *strstr(line,"#") = 0;
    if ((pos1 = strstr( line, "'" ) - line) < 0) continue;
    if ((pos2 = strstr( line + pos1 + 1, "'" ) - line) < 0) continue;
    if ((pos3 = strstr( line + pos2 + 1, "'" ) - line) < 0) continue;
    if ((pos4 = strstr( line + pos3 + 1, "'" ) - line) < 0) continue;
    strcpy( string1, line + pos1 + 1 );
    string1[pos2 - pos1] = 0;
    uppercase( string1 );
    strcpy( string2, line + pos3 + 1 );
    string2[pos4 - pos3] = 0;
    string1[pos2 - pos1 - 1] = string2[pos4 - pos3 - 1] = 0;
    while(replace( spaces, string1, string2 ));
  }
  strcpy( text, spaces + 1 );
  delete spaces;
  delete string1;
  delete string2;
  delete line;
}

int match( char* text, char* pattern, int final ) 
{
	int starpos, pos2;
	/* if it's a perfect match, return with a YIPPE! */
	if (!strcmp( pattern, text )){
 	  return(1);
	}
	/* if we just have a '*' */
	if ((pattern[0] == '*') && (!pattern[1])) { 
	  if (final) strcpy( star, text );
	  return(1); 
	}
	/* check to see if we have a star.. if we don't return false. */
	if (((starpos = strstr( pattern, "*" ) - pattern) < 0) &&
		((starpos = strstr( pattern, "_" ) - pattern) < 0)) {
	  return(0);
	}
	/* make sure the star isn't in the front. */
	if (starpos > 0)
	{
		/* if the stuff in the front of the star doesn't match,
		return false */
		if (strncmp( pattern, text, starpos )){
		  return(0);
		}
		/* if we have nothing after the star, return true. */
		if (!pattern[starpos + 1]) 
		{
			if (final) strcpy( star, text + starpos ); 
			return(1); 
		}
	}
	/* at this point, we either have a star in the front and text
	  * HELLO
	 or a star in the middle and the text in front matches.
	  HELLO * COMPUTER
	 this code should be simplified using strcspn()'s and strspn()
	 actually.. a better idea..  we reverse both of them
	 then we do the same thing we did to begin with.
	 nonetheless.. for now.. this code works.. :) */
	char* pluseoln = new char[MAX_LINE_SIZE];
	char* texteoln = new char[MAX_LINE_SIZE];
	strcpy( texteoln, text );
	int len = strlen( text );
	texteoln[len] = 10;
	texteoln[len + 1] = 0;
	strcpy( pluseoln, pattern + starpos + 1 );
	len = strlen( pluseoln );
	pluseoln[len] = 10;
	pluseoln[len + 1] = 0;
	if ((pos2 = strstr( texteoln, pluseoln ) - texteoln) > -1) 
	{
		if (text[pos2 + strlen( pattern + starpos + 1)] == 0) 
		{
			if (final) 
			{
				strcpy( star, text + starpos ); 
				star[pos2 - starpos] = 0; 
			}
			delete(pluseoln); delete(texteoln);
			return(1);
		}
	}
	delete(pluseoln); delete(texteoln);
	return(0);
}

void print( char* line){
  int start;
  int space;
  char* pos = pbuffer;
  char *scrap;
  int bpos = 0;
  start = 1;
  space = 0;
  while (*pos){
    if (*pos == ' ') space = 1;
    else {
      /* this capitilizes I and I'm */
      if ((space) && ((*pos == 'i') || (*pos == 'I')) && 
      ((*(pos + 1) == ' ') || (*(pos + 1) == '\''))){
        line[bpos++] = ' ';
        line[bpos++] = 'I';
        space = 0;
      }	
      /* capitilizes the first word in a sentence. */
      else if ((start) && (*pos >= 'a') && (*pos <= 'z')) {
        if (space) line[bpos++] = ' ';
        line[bpos++] += 'A' - 'a';
        start = space = 0;
      } 
      else {
        if ((space) && (*(pos +1) != '.')) line[bpos++] = ' ';
        line[bpos++] = *pos;
        space = 0;
      }
      start = 0;
    }
    if ((*pos == '.') && (*pos+1 == ' '))start = 1;
    pos++;
  }
  /* Capitalize current user name */
  scrap = strstr( line, getvar("name",NULL));
  if ((scrap) && (*scrap >= 'a') && (*scrap <= 'z')) *scrap += 'A' - 'a';
  line[bpos] = 0;
  int last = strlen( line ) - 1;
  while ((line[last] == ' ') && (last >= 0)) line[last--] = 0;
  if ((last >= 0) && (line[last] >= 'a') && (line[last] <= 'z')) {
    line[last + 1] = '.';
    line[last + 2] = 0;
  }
}

void randomize( char* text ) {
  /* this function handles our random tags. */
  int pos1;
  char* rstring, *buffer;
  int list[MAX_RANDLIST_SIZE];
  rstring = new char[MAX_LINE_SIZE];
  buffer = new char[MAX_LINE_SIZE];

  while ((pos1 = strstr( text, "<random>" ) - text) > -1){
    strcpy( rstring, text + pos1 + 8 );
    if (strstr( rstring, "</random>" )){
      *strstr( rstring, "</random>" ) = 0;
      int items = 0;
      int pos, last = -1;
      while ((pos = strstr( rstring + last + 1, "<li>" ) - rstring) > -1) {
        list[items++] = pos + 4;
        last = pos;
      }
      int rnd = alirandom( items );
      strcpy( buffer, text );
      strcpy( buffer + pos1, rstring + list[rnd] );
      if (strstr( buffer, "</li>" )) *strstr( buffer, "</li>" ) = 0;
      strcpy( buffer + strlen( buffer ), " " );
      strcpy( buffer + strlen( buffer ), strstr( text, "</random>" ) + 9 );
      strcpy( text, buffer );
    }
    else{
      strcpy( rstring, text + pos1 + 8 ); /* Handle erroneous random's */
      strcpy( text, rstring );
    }
  }
  delete rstring;
  delete buffer;
}

void reevaluate( char* line ) {
  char *varname = new char[MAX_VARNAME_SIZE];
  char *value = new char[MAX_VARVAL_SIZE];
  char *scrap = new char[MAX_LINE_SIZE];
  char *pos;
  /* FILE *gossip = fopen(gossipfile,"a"); */
  lowercase(star);
  randomize(line);
  while((pos = strstr(line,"<think>"))) {
    strcpy(scrap,pos +7);
    *strstr(scrap,"</think>") = 0;
    reevaluate(scrap);
    strremove(line,"<think>","</think>");
  }
  while(replace(line,"<star/>",star));
  while(replace(line,"<that/>",that));
  while ((pos = strstr(line,"<person>"))) {
    strcpy(scrap,pos + 8);
    *strstr(scrap,"</person>")=0;
    substitute(personfile,scrap);
    lowercase(scrap);
    strremove(line,"son>","</person>"); 
    replace(line,"<per",scrap);
  }
  while ((pos = strstr(line,"<person2>"))) {
    strcpy(scrap,pos + 9);
    *strstr(scrap,"</person2>")=0;
    substitute(person2file,scrap);
    lowercase(scrap);
    strremove(line,"son2>","</person2>"); 
    replace(line,"<per",scrap);
  }
  while ((pos = strstr(line,"<personf>"))) {
    strcpy(scrap,pos + 9);
    *strstr(scrap,"</personf>")=0;
    substitute(personffile,scrap);
    lowercase(scrap);
    strremove(line,"sonf>","</personf>"); 
    replace(line,"<per",scrap);
  }
  while ((pos = strstr(line,"<getvar"))){
    if (!(pos =strstr(pos + 8,"name"))) {
      strremove(line,"<getvar","/>");
      continue;
    }
    pos = strstr(pos, "\"") + 1;
    /* ok.. now pos = foo" default="bar" /> or...
       pos = foo" */
    strcpy(scrap,pos);
    *strstr(scrap,"\"") = 0;
    strcpy(varname,scrap);
    pos = strstr(line,"<getvar");
    /* this is if we don't have a default. */
    if (!(strstr(pos + 8,"default"))) {
      strcpy(scrap,pos);
      *strstr(line,"<getvar") = 0;
      strremove(scrap,"<getvar","/>");
      strcat(line,getvar(varname,NULL));
      strcat(line,scrap);
      continue;
    }
    /* insert code here to deal with defaults. */
  }
  while((pos = strlstr(line,"<setvar"))) {
    char *holder = pos;
    int setreturn;
    pos = strstr(pos,"name=\"") + 6;
    strcpy(scrap,pos);
    *strstr(scrap,"\"") = 0;
    strcpy(varname,scrap);
    pos = strstr(pos,">") +1;
    strcpy(scrap,pos);
    *strstr(scrap,"</setvar>") = 0;
    strcpy(value,scrap);
    setreturn = setvar(varname,value);
    if(setreturn == 1){
      /* print name */
      strremove(holder,"<setvar","\"");
      strremove(holder,"\"","</setvar>");
    }
    if (setreturn == 2) {
      strremove(holder,"<setvar",">");
      replace(holder, "</setvar>","");
    }
  }
  while ((pos = strstr(line,"<srai>"))) {
    strcpy(scrap,pos + 6);
    *strstr(scrap,"</srai>")=0;
    uppercase(scrap);
    strcpy(scrap,respond2(scrap));
    /* is this too cheap? */
    strremove(line,"ai>","</srai>"); 
    replace(line,"<sr",scrap);
  }
  while ((pos = strstr(line,"<system>"))) {
    strcpy(scrap,pos +8);
    *strstr(scrap,"</system>") = 0;
    system(scrap);
    strremove(line,"<system>","</system>");
  }
  while((pos = strstr(line,"<gossip>"))) {
    strcpy(scrap,pos+8);
    *strstr(scrap,"</gossip>") = 0;
    fprintf(gossipfile,"%s\n",scrap);
    replace(line,"<gossip>","");
    replace(line,"</gossip>","");
  }
  strcpy(that,line);
  delete(varname);
  delete(value);
  delete(scrap);
}

char* respond2( char* text ) {
  char* pos, *line = new char[MAX_LINE_SIZE];
  char* buffer = new char[MAX_LINE_SIZE];
  char *capthat = new char[MAX_LINE_SIZE];
  fpos_t tpos, bestpos;
  int alpha;
  int thatused;
  star[0] = 0;
  pbuffer[0] = 0;
  best[0] = 0;
  alpha = ((text[0] >= 'A') && (text[0] <= 'Z'));
  /* FILE* aiml = fopen( patternfile, "r" );
  FILE* tmpl = fopen( templatefile, "r" ); */
  int tval, bestline = -1, linenr = -1;
  if (alpha) 
    fsetpos( patternfile, &chunk[text[0] - 'A'] ); 
  else 
    fsetpos( patternfile, &chunk[26] );
  while (!feof( patternfile )){
    linenr++;
    fgets( line,MAX_LINE_SIZE, patternfile );
    /* this gets fired if Alice didn't find an exact
      match in it's own letter */
    if ((alpha) && (line[0] != text[0])) {
      alpha = 0;
      fsetpos( patternfile, &chunk[26] );
      continue;
    }
    /* this strips the position out of the end of the pattern.
      if the line doesn't contain a <tops get the next line. */
    if ((tval = strstr( line, "<tpos=" ) - line) < 0) continue;
    strcpy( buffer, line + tval + 6 );
    if (strstr( buffer, ">" )) *strstr( buffer, ">" ) = 0;
    line[tval] = 0;
    tpos.__pos = atoi( buffer );
    thatused = 0;
    while(replace(line, "<getvar name=\"botname\"/>",getvar("botname",NULL)));
    /* check to see if <that> was used. */
    if ((pos = strstr( line, "<that>" ))){
      /* we copy that into buffer. */
      strcpy( buffer, pos + 6 );
      /* we chop that off our line */
      *strstr( line, "<that>" ) = 0;
      if (strstr( buffer, "</that>" )) *strstr( buffer, "</that>" ) = 0;
      /* now buffer holds our that from the pattern
        that holds our last statement. */
      strcpy(capthat,that);
      uppercase(capthat);
      cleaner(capthat);
      if (!match( capthat, buffer, 0 )) continue;
      thatused = 1;
    }
    spacetrim(line);
  
    if (!match( text, line, 0 )) continue;
    if ((strcmp( text, line ) == 0) || (thatused)) {
      strcpy( best, line );
      bestline = linenr;
      bestpos = tpos;
      break;
    }
    if ((strcmp( line, best ) < 0) && (bestline > -1)) continue;
    strcpy( best, line );
    bestline = linenr;
    bestpos = tpos;
  }
  match( text, best, 1 );
  fsetpos( templatefile, &bestpos );
  fgets( line, MAX_LINE_SIZE, templatefile );
  buffer[0] = 10;
  buffer[1] = 0;
  if (strstr( line, "</template>" )) *strstr( line, "</template>" ) = 0;
  reevaluate( line );
  strcpy(pbuffer,line);
  delete line;
  delete buffer;
  delete capthat;
  return(pbuffer);
}

