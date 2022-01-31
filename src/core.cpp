/*
--Program C: an implementation of ALICE and AIML in C.
--Copyright (C) 2000 Jacco Bikker
--This program is free software; you can redistribute it and/or
modify
--it under the terms of the GNU General Public License as published
by
--the Free Software Foundation.     
*/

/* 
--This program has been modified significantly since Jacco's
--original release, please see THANKS in the text directory 
--for contributions.
*/

#include "alice.h"

extern "C"
{
  void deinitialize();
}

/* star holds "*" and "_", that holds our that, pbuffer is sort of 
a general scrap variable, although it does eventually hold our
answer. */

char *star, *that, *pbuffer; 

/* best holds the pattern matched */
char *best; 

/* var's for our init file. */
FILE *aimlfile, *errorfile;
FILE *substitutefile, *templatefile, *patternfile;
FILE *logfile;
FILE *gossipfile, *personffile, *personfile, *person2file;
char *datafile, *unsortedfile;

/* should we log? */
int logging;
int log_error;

fpos_t chunk[28];

struct _varlist {
	var_def **vars;
	int	count;
} varlist;

void alierror(char* text){
  if (log_error) fprintf(errorfile,text);
  else printf(text);
}

char *get_pattern_matched(){
  return(best);
}

static var_def *seekvar(char *varname ) {
	var_def *curvar;
	if (!varlist.vars) return NULL;
	
	for (int i = 0; i < varlist.count; i++) {
		curvar = varlist.vars[i];
		if (!strcmp(varname, curvar->varname)) 
			return (curvar);
	}
	return NULL;							
}

int setvar(char *varname, char *value) {

	var_def *curvar;
	lowercase(varname);
	curvar = seekvar(varname);

	if (curvar) /* the variable exists. */
		free(curvar->value);
	else {
		/* we need to create it */
		varlist.count++;
		varlist.vars = (var_def **)realloc((void*)varlist.vars, sizeof(var_def) * varlist.count);

		curvar = (var_def *)malloc(sizeof(var_def));
		varlist.vars[varlist.count - 1] = curvar;
	
		curvar->varname = strdup(varname);
		if (*value == '*') {
			curvar->def = 1;
		value++;
		} else
			curvar->def = 0;
	}

	curvar->value = strdup(value);
	if (!curvar->def) return (1);
	return (2);
}		

char *getvar(char *varname, char *vardefault) {
	lowercase(varname);
	var_def *curvar;
	if (strlen(varname) > MAX_VARNAME_SIZE) return NULL;

	curvar = seekvar(varname );
	if (curvar) return(curvar->value);
	if((!vardefault) || (strlen(vardefault) > MAX_VARVAL_SIZE)) 
	return(NULL);
	setvar(varname, vardefault);
	return(vardefault);
}

char *respond(char *text){
  /* Thanks to Dr. Wallace for writing this into the CGI,
     it was stripped out of there, modified slightly, 
     then added to here. */
  char *scrap = new char[MAX_LINE_SIZE];
  char *ltimestr;
  char *uip;
  char *oneline;
  char *output;
  struct tm *ltime;
  time_t clock;

  time(&clock);
  ltime = localtime(&clock); 
  ltimestr = asctime(ltime);

  strcpy(scrap, text);
  substitute(substitutefile,scrap);
  /* perhaps some code to deal with remaining periods that are not
     sentence breaks.  (Like 5.02 or H.I.P.P.i.e.) */
  while(replace(scrap,"?","."));
  while(replace(scrap,"!","."));
  while(replace(scrap,"\n",""));
  /* we need to make sure our sentence ends in a period. */
  if(scrap[strlen(scrap)-2] != '.'){
    strcat(scrap,".");  
  }
  uip = scrap;
  oneline = new char[MAX_LINE_SIZE];
  output = new char[MAX_LINE_SIZE];
  output[0] = 0;
  do {    
    memset(oneline,0, MAX_LINE_SIZE);
    /* skip white space:*/
    while(uip[0] == ' ') uip++;
    strcpy(oneline,uip); 
    if (strstr(uip, "."))  {
      /* advance to next sentence:*/
      uip = strstr(uip, ".")+1;
      /* strip out first sentence:*/
      *strstr(oneline, ".") = 0;
    }   
    if (oneline != NULL && strlen(oneline) > 0) {
      /* Robot Respose:*/
      cleaner(oneline);
      strcat(output, respond2( oneline ));   

      if (logging) {
        fprintf(logfile, "%s'%s' %s/%s %s/%s %s\n",
         ltimestr,getvar("name",NULL),getvar("age",NULL),
         getvar("personality","normal"),getvar("gender",NULL),
         getvar("location",NULL),getvar("ip",NULL));
        fprintf(logfile, "Client: %s\n",oneline);
        /* DW says we sould strip HTML out here (shrug) */
        fprintf(logfile, "Robot: %s\n",output);
      }

      strcat(output,"  ");
    }
  } while (strstr(uip, ".")); /* any more sentences? */
  delete(oneline);
  delete(scrap);
  strcpy(pbuffer, output);
  strcat(pbuffer,"<br>");
  delete(output);
  print(pbuffer);
  return(pbuffer);
}

void savevars( char* varfile ) {
	FILE* vars;
        char filename[MAX_FILENAME_SIZE];
	var_def *curvar;

        sprintf(filename, "../alicedata/%s", varfile);
	vars = fopen(filename, "w");
	if (!vars) alierror("Error opening varfile/n");

	/* first we print out that. */
	fprintf(vars, "%s\n", that);

	/* we print out each variable. */
	if (!varlist.vars) return;
	for (int i = 0; i < varlist.count; i++) {
		curvar = varlist.vars[i];
		if (curvar->def)
			fprintf(vars,"%s=*%s\n",curvar->varname,curvar->value);
		else
			fprintf(vars,"%s=%s\n",curvar->varname,curvar->value);
	}

	fclose(vars);
}

void loadvars( char* varfile ) {
	char *scrap = new char[MAX_LINE_SIZE];
        char filename[MAX_FILENAME_SIZE];

	// here must be removevars()
	/* next we see if varfile exsists. */
        
        sprintf(filename, "../alicedata/%s", varfile);
	FILE *vars = fopen(filename, "r");
	if (vars) {
		fgets(scrap, MAX_LINE_SIZE, vars);
		strcpy(that, scrap);
		/* now we load up our vars. */
		while(!feof(vars)) {
			fgets(scrap, MAX_LINE_SIZE, vars);
			replace(scrap, "\n", "");
			spacetrim(scrap);
			if (!strlen(scrap)) continue;
			char *equals;
			equals = strstr(scrap, "=");
			if (!equals) continue;
			*equals++ = 0;
			setvar(scrap, equals);
		}
	
		fclose(vars);
	} else {
		/*if not, we load up defvars.txt into our varfile, and into memory.
		betcha thought this was gonna be hard :) */
		loadvars(datafile);
		savevars(varfile);
	}  
	delete(scrap);
}

int initialize(char *init_file){
  /* scrap variables for our init file */
  char *aimlscrap, *logscrap, *patternscrap, *templatescrap;
  char *errorscrap, *gossipscrap;
  char *personscrap, *personfscrap, *person2scrap;
  char *substitutescrap;

  /* Buffers for '*', the last response and the print routine */
  star = new char[MAX_LINE_SIZE];
  that = new char[MAX_LINE_SIZE];
  best = new char[MAX_LINE_SIZE];
  pbuffer = new char[MAX_LINE_SIZE];

	// Must be initialized
	memset(&varlist, 0, sizeof(varlist));

  char *epos;

  datafile = new char[MAX_FILENAME_SIZE];

  aimlscrap = new char[MAX_FILENAME_SIZE];
  logscrap = new char[MAX_FILENAME_SIZE];
  patternscrap = new char[MAX_FILENAME_SIZE];
  templatescrap = new char[MAX_FILENAME_SIZE];
  unsortedfile = new char[MAX_FILENAME_SIZE];
  errorscrap = new char[MAX_FILENAME_SIZE];
  substitutescrap = new char[MAX_FILENAME_SIZE];
  personscrap = new char[MAX_FILENAME_SIZE];
  person2scrap = new char[MAX_FILENAME_SIZE];
  personfscrap = new char[MAX_FILENAME_SIZE];
  gossipscrap = new char[MAX_FILENAME_SIZE];

  /*start the radomizer for the <random> tags */
  srand(time(0));

  /* load up the init file. */
  FILE* init = fopen( init_file, "r" );
  if (!init) {
    printf("Init file not found - %s aborting\n",init_file);
    return(1);
  }
  while(!feof(init)) {
    //we use pbuffer as a scrap variable.
    fgets(pbuffer, MAX_LINE_SIZE, init);
    //eliminate comments.
    if(strchr(pbuffer,'#')) *strchr(pbuffer,'#') = 0;
    //delete all spaces
    while(replace(pbuffer," ",""));
    //remove EOL chars
    while(replace(pbuffer,"\n",""));
      epos = strchr(pbuffer,'=');
/*    if (!(epos = strchr(pbuffer,'=')) || (*pbuffer != 0)){
      printf("Error in it file aborting\n");
      return(1); 
    } */
    epos++;
    /* these really should be strncmp()'s */
    if(strstr(pbuffer,"aimlfile")) strcpy(aimlscrap,epos);
    if(strstr(pbuffer,"logfile")) strcpy(logscrap,epos);
    if(strstr(pbuffer,"datafile")) strcpy(datafile,epos);
    if(strstr(pbuffer,"personfile")) strcpy(personscrap,epos);
    if(strstr(pbuffer,"person2file")) strcpy(person2scrap,epos);
    if(strstr(pbuffer,"personffile")) strcpy(personfscrap,epos);
    if(strstr(pbuffer,"substitutefile")) strcpy(substitutescrap,epos);
    if(strstr(pbuffer,"patternfile")) strcpy(patternscrap,epos);
    if(strstr(pbuffer,"templatefile")) strcpy(templatescrap,epos);
    if(strstr(pbuffer,"unsortedfile")) strcpy(unsortedfile,epos);
    if(strstr(pbuffer,"gossipfile")) strcpy(gossipscrap,epos);
    if(strstr(pbuffer,"errorfile")) strcpy(errorscrap,epos);

    if (strstr(pbuffer,"logging")) {
      if(*epos == 'y' || *epos == 'Y' || *epos == '1')
        logging = 1;
      else 
        logging = 0;
    }

    if (strstr(pbuffer,"log_error")) {
      if(*epos == 'y' || *epos == 'Y' || *epos == '1')
        log_error = 1; 
      else 
        log_error = 0;
    }
  } /* end while feof(init) */

  /* open our logfile and errorlog */        
  if (log_error) {
    errorfile = fopen(errorscrap,"a");
    if (!errorfile) printf("Error opening errorfile\n");
  }
  if (logging) {
    logfile = fopen(logscrap, "a");
    if(!logfile) fprintf(errorfile,"Error opening logfile\n");
  }

  gossipfile = fopen(gossipscrap,"a");
  personfile = fopen(personscrap,"r");
  person2file = fopen(person2scrap,"r");
  personffile= fopen(personfscrap,"r");
  substitutefile = fopen(substitutescrap,"r");
  
  //check to see if it needs to create new patterns.txt and templates.txt

  patternfile = fopen(patternscrap,"r");
  templatefile = fopen(templatescrap,"r");

  if (!patternfile || !templatefile){
//    fclose(patternfile); fclose(templatefile);
    patternfile = fopen(patternscrap,"w");
    templatefile = fopen(templatescrap,"w");
    aimlfile = fopen(aimlscrap,"r");
    if (!prepare()) return (1);
    fclose(aimlfile);
    fclose(patternfile);
    fclose(templatefile);
    patternfile = fopen(patternscrap,"r");
    templatefile = fopen(templatescrap,"r");
  }
  else{
    /* ok, this part really should be saved to a file upon preparation */
    char last = 0;
    while (!feof( patternfile )){
      fpos_t line;
      fgetpos( patternfile, &line );
      fgets( pbuffer, MAX_LINE_SIZE, patternfile );
      char first = pbuffer[0];
      if ((first < 'A') || (first > 'Z')) first = 'Z' + 1;
      if (first != last){
        last = first;
        if ((last < 'A') || (last > 'Z')) last = 'Z' + 1;
        chunk[last - 'A'] = line;
      }
    }
  }
  delete(aimlscrap);
  delete(logscrap);
  delete(patternscrap);
  delete(templatescrap);
  delete(errorscrap);
  delete(substitutescrap);
  delete(personscrap);
  delete(person2scrap);
  delete(personfscrap);
  delete(gossipscrap);
  return(0);
}

void deinitialize() {
  if (logging) fclose(logfile);
  if (log_error) fclose(errorfile);
  delete(star);
  delete(that);
  delete(best);
  delete(pbuffer);
  /* need to delete init file variables, but don't know what to do with
them yet. */

}
