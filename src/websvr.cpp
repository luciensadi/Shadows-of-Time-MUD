/* ROM 2.4 Integrated Web Server - Version 1.0
 *
 * This is my first major snippet... Please be kind. ;-)
 * Copyright 1998 -- Defiant -- Rob Siemborski -- mud@towers.crusoe.net
 *
 * Many thanks to Russ and the rest of the developers of ROM for creating
 * such an excellent codebase to program on.
 *
 * If you use this code on your mud, I simply ask that you place my name
 * someplace in the credits.  You can put it where you feel it is
 * appropriate.
 *
 * I offer no guarantee that this will work on any mud except my own, and
 * if you can't get it to work, please don't bother me.  I wrote and 
tested
 * this only on a Linux 2.0.30 system.  Comments about bugs, are, however,
 * appreciated.
 *
 * Now... On to the installation!
 */

/*

1. Add websvr.o to the makefile.
2. Add the following to the section with all the prototypes at the end
   of merc.h:

	* websvr.c *
	void init_web(int port);
	void handle_web(void);
	void shutdown_web(void);

3. In comm.c, right after boot_db() in main(), add a call to init_web().
   Note that it takes a parameter for the port number (I use port+1, so
   if your mud runs on port 8000, it would run on 8001).  Note, this will
   not run under MS_DOS or MAC, so don't bother adding it to the dos part
   of main, only before the call to game_loop_unix.
4. Again in comm.c, after the call to game_loop_unix, add a call to
   shutdown_web()
5. Someplace inside the loop in game_loop_unix, add a call to 
handle_web().
   I put mine between input and output (to allow for possible future 
wiznet
   notifications.
6. Modify handle_web_who_request() to reflect your personal taste (the 
current
   version may not even work with your mud).
7. Recompile.
8. Go to http://your.mud.server:webport/wholist to see who is on line. 
(Webport
   is whatever was defined in step 2).

*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include "merc.h"
#include "recycle.h"
#include "lookup.h"
#include "tournament.h"

#define MAXDATA 1024

typedef struct web_descriptor WEB_DESCRIPTOR;

struct web_descriptor {
    int fd;
    char request[MAXDATA*2];
    struct sockaddr_in their_addr;
    size_t sin_size;
    WEB_DESCRIPTOR *next;	
    bool valid;
};

WEB_DESCRIPTOR *web_desc_free;

/* FUNCTION DEFS */
int send_buf(int fd, const char* buf);
void handle_web_request(WEB_DESCRIPTOR *wdesc);
void handle_web_who_request(WEB_DESCRIPTOR *wdesc);
void handle_web_who_asmo_request(WEB_DESCRIPTOR *wdesc);
WEB_DESCRIPTOR *new_web_desc(void);
void free_web_desc(WEB_DESCRIPTOR *desc);

/* The mark of the end of a HTTP/1.x request */
const char ENDREQUEST[5] = { 13, 10, 13, 10, 0 }; /* (CRLFCRLF) */

/* Externs */
extern int top_web_desc;

/* Locals */
WEB_DESCRIPTOR *web_descs;
int sockfd;

void init_web(int port) {
    struct sockaddr_in my_addr;

    web_descs = NULL;

    logf("Web features starting on port: %d", port);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("web-socket");
	exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htons(INADDR_ANY);
    bzero(&(my_addr.sin_zero),8);

    if((bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr))) == -1)
    {
	perror("web-bind");
	exit(1);
    }

    /* Only listen for 5 connects at once, do we really need more? */
    listen(sockfd, 5);
}

struct timeval ZERO_TIME = { 0, 0 };

void handle_web(void) {
	int max_fd;
	WEB_DESCRIPTOR *current, *prev, *next;
	fd_set readfds;

	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);

	/* it *will* be atleast sockfd */
	max_fd = sockfd;

	/* add in all the current web descriptors */
	for(current=web_descs; current != NULL; current = current->next) {
	    FD_SET(current->fd, &readfds);
	    if(max_fd < current->fd)
		max_fd = current->fd;
	}
	
	/* Wait for ONE descriptor to have activity */
	select(max_fd+1, &readfds, NULL, NULL, &ZERO_TIME);

	if(FD_ISSET(sockfd, &readfds)) {
            /* NEW CONNECTION -- INIT & ADD TO LIST */

	    current = new_web_desc();
	    current->sin_size  = sizeof(struct sockaddr_in);
	    current->request[0] = '\0';

	    if((current->fd = accept(sockfd, (struct sockaddr *) &(current->their_addr), &(current->sin_size))) == -1) 
            {
	    	perror("web-accept");
	    	exit(1);
	    }

	    current->next = web_descs;
	    web_descs = current;

	    /* END ADDING NEW DESC */
	}

	/* DATA IN! */
	for(current=web_descs; current != NULL; current = current->next) {
	    if (FD_ISSET(current->fd, &readfds)) /* We Got Data! */
	    {
	    	char buf[MAXDATA];
		int numbytes;

		if((numbytes=read(current->fd,buf,sizeof(buf))) == -1) {
		    perror("web-read");
		    exit(1);
		}

		buf[numbytes] = '\0';

		strcat(current->request,buf);
	    }
	} /* DONE WITH DATA IN */

	/* DATA OUT */
	for(current=web_descs; current != NULL; current = next ){
	    next = current->next;

	    if(strstr(current->request, "HTTP/1.") /* 1.x request (vernum on FIRST LINE) */
	      && strstr(current->request, ENDREQUEST))
		handle_web_request(current);
	    else if(!strstr(current->request, "HTTP/1.")
		 &&  strchr(current->request, '\n')) /* HTTP/0.9 (no ver number) */
		handle_web_request(current);		
	    else {
		continue; /* Don't have full request yet! */
	    }

	    close(current->fd);

	    if(web_descs == current) {
		web_descs = current->next;
	    } else {
		for(prev=web_descs; prev->next != current; prev = prev->next)
			; /* Just ititerate through the list */
		prev->next = current->next;
	    }

	    free_web_desc(current);
	}   /* END DATA-OUT */
}

/* Generic Utility Function */

int send_buf(int fd, const char* buf) {
	return send(fd, buf, strlen(buf), 0);
}

void handle_web_request(WEB_DESCRIPTOR *wdesc) 
{
	    /* process request */
	    /* are we using HTTP/1.x? If so, write out header stuff.. */
	    if(!strstr(wdesc->request, "GET")) {
		send_buf(wdesc->fd,"HTTP/1.0 501 Not Implemented");
		return;
	    } else if(strstr(wdesc->request, "HTTP/1.")) {
		send_buf(wdesc->fd,"HTTP/1.0 200 OK\n");
		send_buf(wdesc->fd,"Content-type: text/html\n\n");
	    }

	    /* Handle the actual request */
	    if(strstr(wdesc->request, "/wholist")) {
		log_string("Web Hit: WHOLIST");
		handle_web_who_request(wdesc);
            }
	    else if (strstr(wdesc->request, "/asmolist")) {
		log_string("Web Hit: WHOLIST");
		handle_web_who_asmo_request(wdesc);
	    } else {
		log_string("Web Hit: INVALID URL");
		send_buf(wdesc->fd,"Sorry, ROM Integrated Webserver 1.0 only supports /wholist");
	    }
}

void shutdown_web (void) 
{
    WEB_DESCRIPTOR *current,*next;

    /* Close All Current Connections */
    for(current=web_descs; current != NULL; current = next) {
	next = current->next;
	close(current->fd);
	free_web_desc(current);
    }

    /* Stop Listening */
    close(sockfd);
}

void handle_web_who_asmo_request(WEB_DESCRIPTOR *wdesc)
{
    char buf[MAX_STRING_LENGTH];
    char tmp[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    BUFFER *immortals;
    BUFFER *mortals;
    DESCRIPTOR_DATA *d;
    int iClass;
    int iRace;
    int iClan;
    int iLevelLower;
    int iLevelUpper;
    int nMatch;
    bool rgfClass[MAX_CLASS];
    bool rgfRace[MAX_PC_RACE];
    bool rgfClan[top_clan];
    int  preTitleLen;
    int  spacer;

    /*
     * Set default arguments.
     */
    iLevelLower = 0;
    iLevelUpper = MAX_LEVEL;
    for (iClass = 0; iClass < MAX_CLASS; iClass++)
        rgfClass[iClass] = FALSE;
    for (iRace = 0; iRace < MAX_PC_RACE; iRace++)
        rgfRace[iRace] = FALSE;
    for (iClan = 0; iClan < top_clan; iClan++)
        rgfClan[iClan] = FALSE;


    /*
     * Now show matching chars.
     */

     if (xp_bonus.on)
     {
       if (!xp_bonus.negative)
         send_buf(wdesc->fd, "                          Bonus time is ON<BR>");
       else
         send_buf(wdesc->fd, "                          Minus time is ON<BR>");
     }   


    nMatch = 0;
    buf[0] = '\0';
    mortals = new_buf ();
    immortals = new_buf ();
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        CHAR_DATA *wch;
        const char *cClass;

        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if (d->connected != CON_PLAYING || IS_SWITCHED(d->character) || IS_NPC(d->character) || d->character->invis_level > 0)
            continue;

        wch = (d->original != NULL) ? d->original : d->character;

        nMatch++;

        /*
         * Figure out what to print for class.
         */
     
       {
        cClass = class_table[wch->cClass].who_name;
        switch (wch->level)
        {
            default:
                break;
                {
            case MAX_LEVEL - 0:
                    cClass = "     Imp   ";
                    break;
            case MAX_LEVEL - 3:
                    cClass = "   Builder ";
                    break;
            case MAX_LEVEL - 4:
                    cClass = "  G Leader ";
                    break;
            case MAX_LEVEL - 5:
                    cClass = "  Legend   ";
                    break;
            case MAX_LEVEL - 1:
                    cClass = "  Creator  ";
                    break;
            case MAX_LEVEL - 2:
                    cClass = "  Supreme  ";
                    break;
                
             }
         }
}

         if (IS_FORSAKEN(wch))
         {
            cClass = "  Forsaken ";
         }

         if (IS_DRAGON(wch))
         {
            cClass = "  Dragon   ";
         }

         if (wch->level >= LEVEL_IMMORTAL && !IS_NULLSTR(wch->pretit))
         {
           int front, back;
           char temp[MIL];
 
           preTitleLen = colorstrlen(wch->pretit);
           spacer = 11 - preTitleLen;
           front = spacer / 2;
           back = spacer / 2 + (spacer % 2 == 0 ? 0 : 1);               
           sprintf(temp, "%*s%s%*s", front, "", wch->pretit, back, "");
           cClass = str_dup(temp); 
         }
  
        /*
         * Format it up.
         */




    if (wch->level > 77 || IS_DRAGON(wch) || IS_FORSAKEN(wch))  
    {
	 
                 sprintf (buf, "`8[`1+ %s `1+`8]`7[`&%2d`7] 
%s%s%s%s%s`*%s`*%s%s%s%s%s%s`7%s%s`*<BR>",
                 cClass,
                 (wch->honor < 0) ? 0 : wch->honor,
                 str_cmp(clan_table[wch->clan].who_name, "None") ? clan_table[wch->clan].who_name : "",                 
                 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
                 IS_SET(wch->act, PLR_TOURNEY) ? show_trank(wch) : "",
                 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
                 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
                 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
                 wch->incog_level >= LEVEL_HERO ? "`7(`6I`7) " : "",
                 wch->invis_level >= LEVEL_HERO ? "`7(`5W`7) " : "",
                 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
                 IS_MURDERER(wch)?"(`!MURDERER`*) " : "",
		 IS_JAILED(wch)?"(`!JAILED`*) " : "",
                 IS_THIEF(wch)?"(`!THIEF`*) " : "",
                 IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_name : wch->name,
                 wch->pcdata->title); 
                 colourstrip(tmp, buf);
                 add_buf (immortals, tmp);
    }
    else 
    {
                 sprintf (buf, "`8[`1+`7 %2d `1+`8]`7[`&%2d`7] 
%s`7%s%s`*%s%s%s`*%s%s%s%s%s%s`7%s%s`*<BR>",
                 wch->level,
//                 wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name
//                 : "     ",
//                 cClass,
//                 IS_SET(wch->act2, PLR_LOOTABLE) ? "`!" : "`&",
                 (wch->honor < 0) ? 0 : wch->honor,
                 str_cmp(clan_table[wch->clan].who_name, "None") ? clan_table[wch->clan].who_name: "",                 
                 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
                 IS_SET(wch->act, PLR_TOURNEY) ? show_trank(wch) : "",
                 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
                 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
                 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
                 wch->incog_level >= LEVEL_HERO ? "`7(`6I`7) " : "",
                 wch->invis_level >= LEVEL_HERO ? "`7(`5W`7)`7 " : "",
                 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
                 IS_MURDERER(wch)?"(`!MURDER`*) " : "",
		 IS_JAILED(wch)?"(`!JAILED`*) " : "",
                 IS_THIEF(wch)?"(`!THIEF`*) " : "",
                 IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_name : wch->name,
                 wch->pcdata->title);
        colourstrip(tmp, buf);
        add_buf (mortals, tmp);

   }
}

    send_buf(wdesc->fd, "<BR>-------------------------(Immortals/Legends)----------------------------<BR>");
    send_buf(wdesc->fd, buf_string (immortals));
    send_buf(wdesc->fd, "<BR>------------------------------(Mortals)---------------------------------<BR>");
    send_buf(wdesc->fd, buf_string (mortals));

    sprintf (buf2, "<BR>-------------------------- Players found: %d ----------------------------<BR>", nMatch);
    send_buf(wdesc->fd, buf2);
    free_buf (immortals);
    free_buf (mortals);
    return;
}

void handle_web_who_request(WEB_DESCRIPTOR *wdesc)
{
  CHAR_DATA *victim;
  int count=0;
  char output[MAX_STRING_LENGTH];
  char *clan_name,wizi_string[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  send_buf(wdesc->fd,"<HTML><HEAD><TITLE>Shadows of Time Who List</TITLE></HEAD>\n\r");
  send_buf(wdesc->fd,"<BODY BGCOLOR=\"#FFFFFF\"><B>Shadows of Time Who List</B><P>\n\r");

  for (d = descriptor_list; d; d = d->next)
  {
    victim = ( d->original ) ? d->original : d->character;

    if (d->connected != CON_PLAYING || victim->invis_level > 0 ||
        IS_NPC(victim)) {
      continue;
    }

    count++;

    sprintf (wizi_string,"(W:%d) ", victim->invis_level);

    clan_name = str_dup( "" );

    sprintf(output, "%s[%s] %s%s%s%s%s<BR>",
	    IS_IMMORTAL(victim) ? "<B>" : "",
            victim->invis_level >= LEVEL_HERO ? wizi_string : "",
            clan_name,
            IS_SET(victim->comm, COMM_AFK) ? "[AFK] " : "",
            victim->name, victim->pcdata->title,
	    IS_IMMORTAL(victim) ? "</B>" : "");
      send_buf(wdesc->fd,output);
  }
  sprintf(output, "<P>Scale Towers Who-List [%d players found]</BODY></HTML>", count);
  send_buf(wdesc->fd,output);
}

/* These are memory management... they should move to recycle.c soon */

WEB_DESCRIPTOR *new_web_desc(void)
{
    WEB_DESCRIPTOR *desc;

    if(web_desc_free == NULL) {
	desc = (WEB_DESCRIPTOR *) alloc_perm(sizeof(*desc));
	top_web_desc++;
    } else {
	desc = web_desc_free;
	web_desc_free = web_desc_free->next;
    }

    VALIDATE(desc);

    return desc;	
}

void free_web_desc(WEB_DESCRIPTOR *desc)
{
    if(!IS_VALID(desc))
	return;

    INVALIDATE(desc);
    desc->next = web_desc_free;
    web_desc_free = desc;
}
 
