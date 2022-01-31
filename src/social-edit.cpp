/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Starfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
 
/* Online Social Editting Module, 
 * (c) 1996,97 Erwin S. Andreasen <erwin@pip.dknet.dk>
 * See the file "License" for important licensing information
 */ 

/* This version contains minor modifications to support ROM 2.4b4. */


/* Changes by Clerve:
 * Socials are now a double linked list. Every social knows the next social
 * and the previous in the list.
 * Socials when added, get added in alphabetical order.
 * Changed sedit command to work with list, not array.
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "db.h"
#include "interp.h"

#ifdef CONST_SOCIAL
#define SOCIAL_FILE "../area/social.are"
#else
#define SOCIAL_FILE "../area/social.txt"
#endif

void insert_social(struct social_type *);
void remove_social(char*);
void clear_social(struct social_type *);
int count_socials();

#ifndef CONST_SOCIAL 
struct social_type *social_table_head;	   /* and social table */
struct social_type *social_table_tail;	   /* and social table */

void load_social (FILE *fp, struct social_type *social)
{
	strcpy(social->name, fread_string (fp));
	social->char_no_arg =   fread_string (fp);
	social->others_no_arg = fread_string (fp);
	social->char_found =    fread_string (fp);
	social->others_found =  fread_string (fp);
	social->vict_found =    fread_string (fp);
	social->char_auto =     fread_string (fp);
	social->others_auto =   fread_string (fp);
}

void load_social_table ()
{
	FILE *fp;
	int i;
	struct social_type *new_social;
	
	fclose(fpReserve);
	fp = fopen (SOCIAL_FILE, "r");
	
	if (!fp)
	{	bug ("Could not open " SOCIAL_FILE " for reading.",0);
		fpReserve=fopen(NULL_FILE,"r");
		return;
	}
	
	fscanf (fp, "%d\n", &maxSocial);

	social_table_head=alloc_mem(sizeof(struct social_type));
	social_table_tail=alloc_mem(sizeof(struct social_type));
	
	social_table_head->next=social_table_tail;
	social_table_tail->previous=social_table_head;

	for(i=0;i<maxSocial;i++)
	{	new_social=alloc_mem(sizeof(struct social_type));
		load_social(fp,new_social);
		insert_social(new_social);
	}

	fclose (fp);
	fpReserve=fopen(NULL_FILE,"r");
}

/*
 * May 9th by Clerve.
 * Inserting a social alphabeticly
 */
void insert_social(struct social_type *s)
{	struct social_type *iterator;

	/* First element 
	if (strlen(social_table->name)<1)
	{	social_table=s;
		return;
	}
	*/
	
	/* find the right place */	
	iterator=social_table_head->next;
	while(iterator!=social_table_tail)
	{	if (strcasecmp(s->name,iterator->name)<1)
		{	s->previous=iterator->previous;
			s->previous->next=s;
			s->next=iterator;
			iterator->previous=s;
			
			return;
		}
		iterator=iterator->next;
	}
	
	/* append it */
	iterator=social_table_tail->previous;
	iterator->next=s;
	s->previous=iterator;
	s->next=social_table_tail;
	social_table_tail->previous=s;
}

/*
 * May 9th by Clerve.
 * Removing a social
 */
void remove_social(char *name)
{	struct social_type *iterator;
	
	for (iterator=social_table_head->next;iterator!=social_table_tail;iterator=iterator->next)
	{	if (!strcasecmp(name,iterator->name))
		{	struct social_type *p=iterator->previous;
			struct social_type *n=iterator->next;
			p->next=n;
			n->previous=p;

			clear_social(iterator);
			free_mem(iterator,sizeof(iterator));
			return;
		}
	}
}

void clear_social(struct social_type *social)
{
	/*free_string (social->name);*/
	free_string (social->char_no_arg);
	free_string (social->others_no_arg);
	free_string (social->char_found);
	free_string (social->others_found);
	free_string (social->vict_found);
	free_string (social->char_auto);
	free_string (social->others_auto);
}

/*
 * May 9th by Clerve.
 * Counting all socials
 */
int count_socials()
{	struct social_type *iterator;
	int socials=0;
	
	/* set to first social */
	iterator=social_table_head->next;

	while(iterator!=social_table_tail)
	{	socials++;
		iterator=iterator->next;
	}
	return socials;
}


#endif /* CONST_SOCIAL */

void save_social (const struct social_type *s, FILE *fp)
{
	/* get rid of (null) */
	fprintf (fp, "%s~\n%s~\n%s~\n%s~\n%s~\n%s~\n%s~\n%s~\n\n",
			       s->name 		 ? s->name          : "" , 
			       s->char_no_arg 	 ? s->char_no_arg   : "" , 
			       s->others_no_arg  ? s->others_no_arg : "" ,
			       s->char_found     ? s->char_found    : "" , 
			       s->others_found   ? s->others_found  : "" , 
			       s->vict_found     ? s->vict_found    : "" ,
			       s->char_auto      ? s->char_auto     : "" , 
			       s->others_auto    ? s->others_auto   : ""
			       );
}


void save_social_table()
{
	FILE *fp;
	struct social_type *iterator;
	
        fclose(fpReserve);
	fp = fopen (SOCIAL_FILE, "w");
	
	if (!fp)
	{	bug ("Could not open " SOCIAL_FILE " for writing.",0);
		return;
	}

#ifdef CONST_SOCIAL /* If old table still in use, count socials first */
	
	for (maxSocial = 0 ; social_table[maxSocial].name[0] ; maxSocial++)
		; /* empty */
#endif	

	fprintf (fp, "%d\n", count_socials());
	
	/* set to first social */
	iterator=social_table_head->next;

	while(iterator!=social_table_tail)
	{	save_social(iterator,fp);
		iterator=iterator->next;
	}
	
	fclose (fp);
        fpReserve = fopen( NULL_FILE, "r" );
}


/* Find a social based on name */ 
struct social_type *social_lookup (const char *name)
{
	struct social_type *iterator;	

	/* set to first social */
	iterator=social_table_head->next;

	while(iterator!=social_table_tail)
	{	if(!str_cmp(name,iterator->name))
			return iterator;

		iterator=iterator->next;
	}

	return NULL;
}

/*
 * Social editting command
 */

#ifndef CONST_SOCIAL
void do_sedit (CHAR_DATA *ch, char *argument)
{
	char cmd[MAX_INPUT_LENGTH], social[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	struct social_type *iSocial;	

	smash_tilde (argument);
	
	argument = one_argument (argument,cmd);
	argument = one_argument (argument,social);
	
	if (!cmd[0])
	{
		send_to_char ("Huh? Type HELP SEDIT to see syntax.\n\r",ch);
		return;
	}
		
	if (!social[0])
	{
		send_to_char ("What social do you want to operate on?\n\r",ch);
		return;
	}
	
	iSocial = social_lookup(social);
	
	if (str_cmp(cmd,"new") && (iSocial == NULL))
	{
		send_to_char ("No such social exists.\n\r",ch);
		return;
	}

	if (!str_cmp(cmd, "delete")) /* Remove a social */
	{
		remove_social(iSocial->name);
		send_to_char ("That social is history now.\n\r",ch);
	}
	
	else if (!str_cmp(cmd, "new")) /* Create a new social */
	{
		struct social_type *new_social=alloc_mem(sizeof(struct social_type));
                int x;
		
		if (iSocial != NULL)
		{
			send_to_char ("A social with that name already exists\n\r",ch);
			return;
		}
                for ( x = 0; cmd_table[x].name[0] != '\0'; x++ )
                {
                    if (!str_prefix1( social, cmd_table[x].name ))
                    {
                        send_to_char("A command with that name already exists.\n",ch);
                        return;     
                    }
                }
		
		strcpy(new_social->name, str_dup (social));
		new_social->char_no_arg = str_dup ("");
		new_social->others_no_arg = str_dup ("");
		new_social->char_found = str_dup ("");
		new_social->others_found = str_dup ("");
		new_social->vict_found = str_dup ("");
		new_social->char_auto = str_dup ("");
		new_social->others_auto = str_dup ("");

		insert_social(new_social);
		
		send_to_char ("New social added.\n\r",ch);			
	}
	
	else if (!str_cmp(cmd, "show")) /* Show a certain social */
	{
		sprintf (buf, "{HSocial: %s{x\n\r"
		              "{G[cnoarg]{c No argument given, character sees:\n\r"
		              "{Y         %s\n\r"
		              "{G[onoarg]{c No argument given, others see:\n\r"
		              "{Y         %s\n\r"
		              "{G[cfound]{c Target found, character sees:\n\r"
		              "{Y         %s\n\r"
		              "{G[ofound]{c Target found, others see:\n\r"
		              "{Y         %s\n\r"
		              "{G[vfound]{c Target found, victim sees:\n\r"
		              "{Y         %s\n\r"
		              "{G[cself]{c  Target is self:\n\r"
		              "{Y         %s\n\r"
		              "{G[oself]{c  Target is self, others see:\n\r"
		              "{Y         %s{x\n\r",
		              
		              iSocial->name,
		              iSocial->char_no_arg,
		              iSocial->others_no_arg,
		              iSocial->char_found,
		              iSocial->others_found,
		              iSocial->vict_found,
		              iSocial->char_auto,
		              iSocial->others_auto);

		send_to_char (buf,ch);		          
		return; /* return right away, do not save the table */
	}
	
	else if (!str_cmp(cmd, "cnoarg")) /* Set that argument */
	{
		free_string (iSocial->char_no_arg);
		iSocial->char_no_arg = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Character will now see nothing when this social is used without arguments.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
	}
	
	else if (!str_cmp(cmd, "onoarg"))
	{
		free_string (iSocial->others_no_arg);
		iSocial->others_no_arg = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Others will now see nothing when this social is used without arguments.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
			
	}
	
	else if (!str_cmp(cmd, "cfound"))
	{
		free_string (iSocial->char_found);
		iSocial->char_found = str_dup(argument);		

		if (!argument[0])
			send_to_char ("The character will now see nothing when a target is found.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
			
	}
	
	else if (!str_cmp(cmd, "ofound"))
	{
		free_string (iSocial->others_found);
		iSocial->others_found = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Others will now see nothing when a target is found.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
			
	}
	
	else if (!str_cmp(cmd, "vfound"))
	{
		free_string (iSocial->vict_found);
		iSocial->vict_found = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Victim will now see nothing when a target is found.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
	}
	
	else if (!str_cmp(cmd, "cself"))
	{
		free_string (iSocial->char_auto);
		iSocial->char_auto = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Character will now see nothing when targetting self.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);

	}
	
	else if (!str_cmp(cmd, "oself"))
	{
		free_string (iSocial->others_auto);
		iSocial->others_auto = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Others will now see nothing when character targets self.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
	}
	
	else
	{	send_to_char ("Huh? Try HELP SEDIT.\n\r",ch);
		return;
	}
	
	/* We have done something. update social table */
	
	save_social_table();
}
#endif /* CONST_SOCIAL */
