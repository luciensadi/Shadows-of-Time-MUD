/* Preprocessor.cpp */

#include "alice.h"

/* cc is checkcount,
hp is have we had a pattern?  If we have it's set to one.
hh is had that?
he is had template. */

int cc = 0, hp = 0, hh = 0, he = 0;

/* topic holds our topic which we convert to conditional tags. */
/* this isn't coded yet */
char *topic;

FILE *unsorted;

void replacer(char *line){
  char *pos;
  char *scrap = new char[MAX_LINE_SIZE];
  
  replace(line,"<alice>","");
  replace(line,"<category>","");
  replace(line,"</category>","");
  replace(line,"</alice>","");
  if((pos = strstr(line,"<topic name=\""))){
    strcpy(topic,pos);
    *strstr(topic,"\"") = 0;
  }
  if (strstr(line,"</topic>")){
    topic[0] = 0;
  }
  strremove(line,"<topic",">");
  replace(line,"</topic>","");
  while (replace( line, "<getversion/>", "<getvar name=\"version\"/>"));
  while (replace( line, "<set_animagent/>", "<setvar name=\"animagent\">on</setvar>"));
  while (replace( line, "<person/>", "<person><star/></person>"));
  while (replace( line, "<person2/>", "<person2><star/></person2>"));
  while (replace( line, "<personf/>", "<personf><star/></personf>"));
  while (replace( line, "<setname/>", "<setvar name=\"name\"/><star/></setvar>"));
  while (replace( line, "<birthday/>", "<getvar name=\"botbirthday\"/>"));
  while (replace( line, "<birthplace/>", "<getvar name=\"botbirthplace\"/>"));
  while (replace( line, "<botasmter/>", "<getvar name=\"botasmter\"/>"));
  while (replace( line, "<botmaster/>", "<getvar name=\"botmaster\"/>"));
  while (replace( line, "<boyfriend/>", "<getvar name=\"botboyfriend\"/>"));
  while (replace( line, "<favorite_band/>", "<getvar name=\"botband\"/>"));
  while (replace( line, "<favorite_book/>", "<getvar name=\"botbook\"/>"));
  while (replace( line, "<favorite_color/>", "<getvar name=\"botcolor\"/>"));
  while (replace( line, "<favorite_food/>", "<getvar name=\"botfood\"/>"));
  while (replace( line, "<favorite_movie/>", "<getvar name=\"botmovie\"/>"));
  while (replace( line, "<favorite_song/>", "<getvar name=\"botsong\"/>"));
  while (replace( line, "<for_fun/>", "<getvar name=\"botfun\"/>"));
  while (replace( line, "<friends/>", "<getvar name=\"botfriends\"/>"));
  while (replace( line, "<gender/>", "<getvar name=\"botgender\"/>"));
  while (replace( line, "<getname/>", "<getvar name=\"name\"/>"));
  while (replace( line, "<get_gender/>", "<getvar name=\"gender\"/>"));
  while (replace( line, "<getsize/>", "<getvar name=\"botsize\"/>"));
  while (replace( line, "<gettopic/>", "<getvar name=\"topic\"/>"));
  while (replace( line, "<girlfriend/>", "<getvar name=\"botgirlfriend\"/>"));
  while (replace( line, "<location/>", "<getvar name=\"botlocation\"/>"));
  while (replace( line, "<look_like/>", "<getvar name=\"botlooks\"/>"));
  while (replace( line, "<name/>", "<getvar name=\"botname\"/>"));
  while (replace( line, "<kind_music/>", "<getvar name=\"botmusic\"/>"));
  while (replace( line, "<question/>", "<getvar name=\"question\"/>"));
  while (replace( line, "<sign/>", "<getvar name=\"botsign\"/>"));
  while (replace( line, "<talk_about/>", "<getvar name=\"bottalk\"/>"));
  while (replace( line, "<they/>", "<getvar name=\"they\"/>"));
  while (replace( line, "<wear/>", "<getvar name=\"botwear\"/>"));
  while (replace( line, "<setname>", "<setvar name=\"name\">"));
  while (replace( line, "</setname>", "</setvar>"));
  while (replace( line, "<settopic>", "<setvar name=\"topic\">"));
  while (replace( line, "</settopic>", "</setvar>"));
  while (replace( line, "<set_female/>","<setvar name=\"gender\">she</setvar>"));
  while (replace( line, "<set_male/>","<setvar name=\"gender\">he</setvar>"));
  /* need code to handle get_??? and set_??? */
  while((pos = strstr(line, "<get_"))) {
    replace(pos,"<get_","<getvar name=\"");
    replace(pos,"/>","\"/>");
  }
  while((pos = strstr(line,"<set_"))){
    strcpy(scrap,pos+5);
    * (strstr(scrap,">") + 1)= 0;
    replace(pos,"<set_","<setvar name=\"");
    replace(pos,">","\">");
    replace(pos,"</set_","</set");
    replace(pos,scrap,"var>");
    }
  while (replace(line, "<sr/>", "<srai><star/></srai>"));
  while(replace(line, "\n", ""));
  delete(scrap);
}  

int checkfile(char* filepath) {
  /* This function changed via Philippe's suggestion */
  int retval = 1;
  FILE* checkedfile = fopen( filepath, "r" );
  if (!checkedfile) retval = 0; else fclose( checkedfile );
  return(retval);
}

int checkline( char* line ) {
/* this performs syntax checking on the AIML.

ok, what weve got is a global int called cc.  If we have a
tag we add the appropiate value.  If we have a /tag we subtract that
value.
The values go as follows.
<alice> = ba = +1  </alice> = ea = -1
<topic> = bt = 2
category = bc = 4
pattern = bp = 8
that = bh = 16
template = be = 32
*/
  int ba = 0, ea = 0; /* <alice> begin and end */
  int bt = 0, et = 0; /* <topic>               */
  int bc = 0, ec = 0; /* <category>            */
  int bp = 0, ep = 0; /* <pattern>             */
  int bh = 0, eh = 0; /* <that>                */
  int be = 0, ee = 0; /* <template>            */
  if (strstr(line,"<alice>")) ba = 1;
  if (strstr(line,"</alice>")) ea = 1;
  if (strstr(line,"<topic")) bt = 1;
  if (strstr(line,"</topic>")) et = 1;
  if (strstr(line,"<category>")) bc = 1;
  if (strstr(line,"</category>")) ec = 1;
  if (strstr(line,"<pattern>")) bp = 1;
  if (strstr(line,"</pattern>")) ep = 1;
  if (strstr(line,"<that>")) bh = 1;
  if (strstr(line,"</that>")) eh = 1;
  if (strstr(line,"<template>")) be = 1;
  if (strstr(line,"</template>")) ee = 1;
  if (ba){
    if (cc != 0) return(0);
    cc +=1;
  }
  if (bt){
    if (cc != 1) return(0);
    cc += 2;
  }
  if (bc){
    if (cc !=1 && cc != 3) return(0);
    cc += 4;
  }
  if (bp){
    if (!(cc & 1) || !(cc & 4)) return(0);
    cc += 8;
  }
  if (ep){
    if (!(cc & 1) || !(cc & 4) || hh || he || !(cc & 8)) return(0);
    cc -= 8; hp = 1;
  }

  if (bh){
    if (!(cc & 1) || !(cc & 4) || !(hp) || he) return(0);
    cc += 16;
  }
  if (eh){
    if (!(cc & 1) || !(cc & 4) || !(hp) || he || !(cc & 16)) return(0);
    cc -= 16; hh = 1;
  }
  if (be){
    if (!(cc & 1) || !(cc & 4) || !(hp)) return(0);
    cc += 32;
  }
  if (ee){
    if (!(cc & 1) || !(cc & 4) || !(hp) || !(cc & 32)) return(0);
    cc -= 32; he = 1;
  }
  if (ec){
    if (!(cc & 1) || !(cc & 4) || !(hp) || !(he)) return(0);
    cc -= 4; hp = 0; hh = 0; he = 0;
  }
  if (et){
    if (cc !=3) return(0);
    cc -= 2;
  }
  if (ea){
/*??    if (cc != 1) return(0); */
    cc -= 1;
  }
  return(1);
}

int splitaiml(char *src){
  cc = 0;
  /* it now prepares the temp file into the template and pattern files. */
  int patterns = 0, templates = 0, bytecount = 0;

  FILE* aiml = fopen( src, "r" );
  if (!aiml){ printf("%s not found, skipping.",src); return(1); }
  printf("Adding %s...",src);

/*  FILE* ftemp = fopen ( templatefile, "a" );
  FILE* fpatt = fopen ( unsortedfile, "a" ); */

  int linenr = -1;
  char* line = new char[MAX_LINE_SIZE];
  char* next = new char[MAX_LINE_SIZE];
  char* buffer = new char[MAX_LINE_SIZE];
  fgets( line, MAX_LINE_SIZE, aiml );
  if (!checkline(line)){
    printf("\nError %i encountered in file %s, at line %i\nAborting.\n",cc,src,linenr);
    return(0);
  }
  memset(buffer, 0, MAX_LINE_SIZE);
  /* we now make the temp.txt into unsorted.txt and templates.txt
  also we exchange some tags for shorthand tags. */
  while (!feof( aiml )){
    linenr++;
    fgets(next, MAX_LINE_SIZE, aiml); 
    if (!checkline(next)){
      printf("\nError %i encountered in file %s, at line %i\nAborting.\n",cc,src,linenr);
      return(0);
    }

    replacer(line);
    if (strstr(line, "<pattern>" ) && (strstr(line, "<that>")) || strstr(next, "<that>")) {
      while (!strstr( line, "</that>" )) {
        if (strlen( line ) > MAX_LINE_SIZE) { printf( "Line too long: \n%s", line );     break; }
        strcpy( line + strlen( line ), next );
        replacer(line);
        linenr++;
        fgets( next, MAX_LINE_SIZE, aiml );
        if (!checkline(next)){
          printf("\nError %i encountered in file %s, at line %i\nAborting.\n",cc,src,linenr);
          return(0);
        }
      }
      patterns++;
      /* what we need to print out.. is line till that. */
      strcpy(buffer, strstr(line,"</that>") + 7); 
      *(strstr(line,"</that>") + 7) = 0;
      replace(line, "<pattern>", "");
      replace(line, "</pattern>", ""); 
      fprintf( unsorted, "%s", line);
      bytecount += strlen( line ) - 3;
    }
    else if (strstr(line, "<pattern>" )) {
      while (!strstr( line, "</pattern>" )) {
        if (strlen( line ) > MAX_LINE_SIZE) { printf( "Line too long: \n%s", line );     break; }
        strcpy( line + strlen( line ), next );
        replacer(line);
        linenr++;
        fgets( next, MAX_LINE_SIZE, aiml );
        if (!checkline(next)){
          printf("\nError %i encountered in file %s, at line %i\nAborting.\n",cc,src,linenr);
          return(0);
        }
      }
      patterns++;
      strcpy(buffer, strstr(line,"</pattern>") + 10); 
      *(strstr(line,"</pattern>") + 10) = 0;
      replace(line, "<pattern>", "");
      replace(line, "</pattern>", "");
      fprintf( unsorted, "%s", line);
      bytecount += strlen( line ) - 3;
    }
    if (strstr(line, "<template>" )) {
      while (!strstr (line, "</template>")) {
        if (strlen( line ) > MAX_LINE_SIZE) { printf( "Line too long: \n%s", line );     break; }
        strcpy( line + strlen( line ), next );
        replacer(line);
        linenr++;
        fgets( next, MAX_LINE_SIZE, aiml );
        if (!checkline(next)){
          printf("\nError %i encountered in file %s, at line %i\nAborting.\n",cc,src,linenr);
          return(0);
        }
      }
      fpos_t tpos;
      fgetpos( templatefile, &tpos );
      templates++;
      if (templates != patterns) {
        printf( "Pattern/Template count mismatch\n"); 
        exit (0);	
      }
      strcpy(buffer, strstr(line,"</template>") + 11); 
        *(strstr(line,"</template>") + 11) = 0;
      replace(line, "<template>", "");    
      fprintf( templatefile, "%s\n", line);
      fprintf( unsorted, "<tpos=%li>\n", tpos.__pos );
      strcpy(buffer, strstr(line,"</template>") + 11); 
      bytecount += strlen( line ) - 3;	    
    }
    strcpy( line, buffer);
    strcpy( line + strlen(line), next);

  }
  /* this shouldn't be needed */
  fprintf( templatefile, "<end of file>\n" );
  printf("Added.\n");
  delete(line);
  delete(buffer);
  delete(next);
  fclose( aiml );
  return(1);
}

int prepare(){
  topic = new char[MAX_VARVAL_SIZE];

  /* routine looks through C.aiml for what files to load. */
  printf( "Loading original ALICE AIML files:\n\n" );
  char* aline = new char[256];	
  fgets(aline,256,aimlfile);
  unsorted = fopen(unsortedfile,"w");
  while(!feof(aimlfile)){
    aline[strcspn(aline,"\n")] = 0;
    if (!splitaiml(aline)) return (0);
    memset(aline,0,256);
    fgets(aline,256,aimlfile);
  }
  delete(aline);
  /* now we alphabatize unsorted.txt by the first letter.  
     we put this in patterns.txt
     we also set chunk, which will tell us where the first letter
     is in patterns.txt */
  fclose(unsorted);
  unsorted = fopen(unsortedfile, "r");
/*  fpos_t reset;
  fgetpos( unsortedfile, &reset ); */
  /* FILE* sorted = fopen( patternfile, "w" ); */
  char *buffer = new char[MAX_LINE_SIZE];
  printf("\nSorting Patterns: ");
  for(int i = 'A'; i <=('Z' + 1); i++){
    fgetpos( patternfile, &chunk[i - 'A'] );
    buffer[0] = i;
    buffer[1] = 0;
    if(i <= 'Z') printf(buffer);
    /* fsetpos( unsortedfile, &reset); */
    rewind(unsorted);
    while(!feof(unsorted)){
      fgets(buffer,MAX_LINE_SIZE,unsorted);
      if(i <='Z'){
        if(buffer[0] ==i) fprintf(patternfile,buffer);
      }
      else if ((buffer[0] < 'A') || (buffer[0] > 'Z')) fprintf(patternfile, buffer);
    }
  } 
  delete buffer;
  delete topic;
  remove(unsortedfile); 
  return(1);
}
