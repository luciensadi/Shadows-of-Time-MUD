/* ************************ Maestro Noting System ********************* */
/* **********************************************************************
----------
Copyright 1997 by Amadeus <Andy McMahan>
amadeus@myself.com


Complete permission given to use this code granted if user retains this header,
and does not change the name of the system.  It must always be called "Maestro",
by Amadeus.

************************************************************************ */


#if defined( macintosh )
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
#include "act_note.h"

/*
 * UNCOMMENT this if you're running ROM 2.4 with do_replay command.
 *
 * Please note that right now this only activates the code associated
 * with do_replay() -- as per stuff.txt, you'll have to add much more
 * code to other files dealing with channels to make it work correctly.
 * ..see stuff.txt for information.
 */
  #define rom24mae
 


/*
 * UNCOMMENT this if you're running InterMudChat
 *
 * #define imcmae
 */

/*
 * Local functions.
 */
bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	note_attach	args( ( CHAR_DATA *ch ) );
void	note_remove	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void    write_note	args( ( DESCRIPTOR_DATA *d, char *argument ) );

char *  board_name      args( ( int board_config ) );
int     board_config;
void    do_note		args( ( CHAR_DATA *ch, char *argument ) );


/*
 * Externals
 */

extern           NOTE_DATA         *     note_free;

#if defined imcmae
extern 	void imc_post_mail(CHAR_DATA *from,
                   const char *sender,
                   const char *to_list,
                   const char *subject,
                   const char *date,
                   const char *text);
#endif

#if defined (rom24mae)
extern void do_replay( CHAR_DATA *ch, char *argument );
#endif

/*
 * Code dealing with the Configuration of Boards read by Player
 */


char *board_name( int board_config )
{

    static  char  buf[512];

    buf[0] = '\0';

    if ( board_config & BOARD_GENERAL  )  strcat( buf, " GENERAL"  );
    if ( board_config & BOARD_PERSONAL )  strcat( buf, " PERSONAL" );
    if ( board_config & BOARD_QUEST    )  strcat( buf, " QUEST"    );    
    if ( board_config & BOARD_CREATIVITY) strcat( buf, " CREATIVITY" );

    if ( board_config & BOARD_IMMORTAL ) strcat( buf, " IMMORTAL" );
    if ( board_config & BOARD_BUILDER  ) strcat( buf, " BUILDER"  );
    if ( board_config & BOARD_CODER    ) strcat( buf, " CODER"    );
    if ( board_config & BOARD_SENIOR   ) strcat( buf, " SENIOR"   );
    if ( board_config & BOARD_EXEC     ) strcat( buf, " EXEC"     );
        
    return ( buf[0] != '\0' ) ? buf+1 : "<None>";
}

int board_value( char *buf )
{
     if ( !str_cmp( buf, "general" ) )    return BOARD_GENERAL;
     if ( !str_cmp( buf, "personal" ) )   return BOARD_PERSONAL;
     if ( !str_cmp( buf, "quest" ) )      return BOARD_QUEST;
     if ( !str_cmp( buf, "immortal" ) )   return BOARD_IMMORTAL;
     if ( !str_cmp( buf, "builder" ) )    return BOARD_BUILDER;
     if ( !str_cmp( buf, "coder" ) )	  return BOARD_CODER;
     if ( !str_cmp( buf, "senior" ) )     return BOARD_SENIOR;
     if ( !str_cmp( buf, "exec" ) )	  return BOARD_EXEC;
     if ( !str_cmp( buf, "creativity") )  return BOARD_CREATIVITY;
     return 0;
}


/* MAIN MENU */

void write_note( DESCRIPTOR_DATA *d, char *argument )
{
    CHAR_DATA *ch;
    NOTE_DATA *pnote;
    FILE *fp;
    char *strtime;
    char       buf  [ MAX_STRING_LENGTH   ];
    char       tmp  [ MAX_STRING_LENGTH   ];
    char       buf1 [ MAX_STRING_LENGTH   ];

    while ( isspace( *argument ) )
        argument++;

    ch          = d->character;

    switch ( d->writing_note )
    {

    case NOTING_NO_ARGUMENT:
	switch(*argument) 
	  {
		case '1':
		    write_to_buffer( d, CLR, 0 );
	            buf1[0] = '\0';
	            for ( pnote = note_list; pnote; pnote = pnote->next )
	              {
		         if ( is_note_to( ch, pnote )
		              && str_cmp( ch->name, pnote->sender )
		              && ch->last_note_special < pnote->date_stamp 
			      && IS_SET(pnote->special, NOTE_SPECIAL_PERSON_TO_PERSON))
		          {
		             break;
		          }
	              }
	           if ( pnote )
	              {
			if (!IS_SET( pnote->special, NOTE_SPECIAL_IS_BULLETIN) )
              		   {
			  sprintf( buf, "Number: %4d                     Board: %s\n\rFrom: %10s                    To: %s\n\rDate:    %s\n\rSubject: %s\n\r==============================================================\n\r",
				  pnote->vnum,
				  board_name(pnote->board),
				  pnote->sender,
				  pnote->to_list,
				  pnote->date,
				  pnote->subject );
		          strcat( buf1, buf );
		          strcat( buf1, pnote->text );
		          ch->last_note_special = UMAX( ch->last_note_special, pnote->date_stamp );
		          send_to_char( buf1, ch );

             	          write_to_buffer( d, "\n\r\n\r", 0);
		          write_to_buffer( d, "Would you like to read your next unread person-to-person note? [y/n] : ", 0 );
		          ch->desc->writing_note = NOTING_QUESTION_READ_NEXT_SPECIAL;
		          break;
                         }
			}
		    else
			{
			send_to_char( "There are no unread notes for you.\n\r", ch );
	            
		        write_to_buffer( d, "\n\r\n\r", 0);
		    	write_to_buffer( d, "Hit <RETURN> to go to the main menu: ", 0 );
		        ch->desc->writing_note = NOTING_CONTINUE;
		        break;
			}
		
		case 'r':
		case 'R':
		case '2':
		    write_to_buffer( d, CLR, 0 );
		    do_note( ch, "read" );
				
		    write_to_buffer( d, "\n\r\n\r", 0 );
		    write_to_buffer( d, "Would you like to read your next unread note,\n\r", 0 );
		    write_to_buffer( d, "or return to the main menu? [y/n/M]: ", 0 );	
	   	    ch->desc->writing_note = NOTING_QUESTION_READ_NEXT;
		    break;

		case 'p':
		case 'P':
		case '3':
		    write_to_buffer( d, "\n\r\n\rWhich number note would you like to display? [ # , <L>ist or <M>ain menu]? ", 0 );
		    ch->desc->writing_note = NOTING_READ_PREVIOUS;
		    break;
    

		case 'w':
		case 'W':
		case '4':
		    
                  if ( ch->level < 2 )
	           {
	             write_to_buffer( d, "You may not write notes until you are level 2.\n\r", 0 );
	             return;
	           }
 
              	  note_attach(ch);
	          write_to_buffer( d, CLR, 0 );
		  write_to_buffer( d, HWHITE, 0 );
	          write_to_buffer( d, "---------------==============> Note System <===============---------------\n\r\n\r", 0 );
		  write_to_buffer( d, NTXT, 0 );
		  write_to_buffer( d, "Answer the following questions regarding the nature of the note\n\r", 0 );
		  write_to_buffer( d, "you want to write.  Answer 'M' to any question to\n\r", 0 );
		  write_to_buffer( d, "return to the main menu at any point.  More detailed instructions\n\r", 0 );
		  write_to_buffer( d, "are under the <H>elp files option in the main menu.\n\r", 0 );
	          write_to_buffer( ch->desc, HWHITE, 0 );
	          write_to_buffer( ch->desc, "-------------==============>******************<=============--------------\n\r\n\r", 0 );
	          write_to_buffer( ch->desc, NTXT, 0 );
		  write_to_buffer( d, "Is this note to a specific individual or individuals? [y/n]: ", 0 );
		  ch->desc->writing_note = NOTING_TO_QUESTION;  
       		  break;

		case 'd':
		case 'D':
                case '5':
		  write_to_buffer( d,"\n\r\n\rDelete which note from your list? [# , <L>ist or <M>ain menu: ", 0 );
		  ch->desc->writing_note = NOTING_QUESTION_DELETE_NOTE;
		  break;

		case 'b':
		case 'B':
		    write_to_buffer( d, CLR, 0 );
	            buf1[0] = '\0';
	            for ( pnote = note_list; pnote; pnote = pnote->next )
	              {
	      		 if ( is_note_to( ch, pnote )
		         && str_cmp( ch->name, pnote->sender )
		         && ch->last_note_bulletin < pnote->date_stamp 
			 && IS_SET(pnote->special, NOTE_SPECIAL_IS_BULLETIN))
		          {
		             break;
		          }
	              }
	           if ( pnote )
	              {
			  sprintf( buf, "Number: %4d                     Board: %s\n\rFrom: %10s                    To: %s\n\rDate:    %s\n\rSubject: %s\n\r==============================================================\n\r",
				  pnote->vnum,
				  board_name(pnote->board),
				  pnote->sender,
				  pnote->to_list,
				  pnote->date,
				  pnote->subject );
		          strcat( buf1, buf );
		          strcat( buf1, pnote->text );
		          ch->last_note_bulletin = UMAX( ch->last_note_bulletin, pnote->date_stamp );
		          send_to_char( buf1, ch );
			  write_to_buffer( d, "\n\r\n\r", 0);
		          write_to_buffer( d, "Would you like to read your next unread bulletin? [y/n] : ", 0 );
		          ch->desc->writing_note = NOTING_QUESTION_READ_NEXT_BULLETIN;
		          break;
			}
	            else
		        {
			send_to_char( "There are no unread bulletins for you.\n\r", ch );
	                write_to_buffer( d, "\n\r\n\r", 0);
		        write_to_buffer( d, "Press <RETURN> to go to the main menu: ", 0 );
			ch->desc->writing_note = NOTING_CONTINUE;
		        break;
			}

		case 'c':
		case 'C':
		  write_to_buffer( d, CLR, 0 );
		  write_to_buffer( d, HWHITE, 0 );
	          write_to_buffer( d, "---------------===============> Note System <==============---------------\n\r\n\r", 0 );
		  write_to_buffer( d, NTXT, 0 );
		  write_to_buffer( d, "Which aspect of your configuration would you like to change?\n\r\n\r", 0 );
		  write_to_buffer( d, "               1.  Boards on which you currently subscribe\n\r", 0 );
		  write_to_buffer( d, "               2.  String Echo (if you're having problems with the note\n\r", 0 );
		  write_to_buffer( d, "                                system because of using a MUD client)\n\r\n\r", 0 );
		  write_to_buffer( d, "     [1 - 2, or <M>ain menu]: ", 0 );
		  ch->desc->writing_note = NOTING_CONFIG_NO_ARGUMENT;
		  break;


		case '6':
		  write_to_buffer( d, CLR, 0 );
		  write_to_buffer( d, HWHITE, 0 );
	          write_to_buffer( d, "---------------==============> Note System <===============---------------\n\r\n\r", 0 );
		  write_to_buffer( d, NTXT, 0 );
		  do_note( ch, "listb" );
		  write_to_buffer( d, "\n\r\n\r", 0 );
		  write_to_buffer( d, "Would you like to:\n\r", 0 );
		  write_to_buffer( d, "          1.  Read a bulletin on this list\n\r", 0 );
		  write_to_buffer( d, "     [1 or <M>ain menu]: ", 0 );
		  ch->desc->writing_note = NOTING_LIST_BULLETIN_QUESTION;
		  break;


		case '7':
                  if ( get_trust(ch) < CAN_POST_BUL )
	           {
	             write_to_buffer( d, "This option is not available to you.\n\r", 0 );
	             return;
	           }
 
              	  note_attach(ch);
	          write_to_buffer( d, CLR, 0 );
		  write_to_buffer( d, HWHITE, 0 );
	          write_to_buffer( d, "--------------=============> System Bulletins <==============-------------\n\r\n\r", 0 );
		  write_to_buffer( d, NTXT, 0 );
		  write_to_buffer( d, "What is the subject of this bulletin?: ", 0 );
		  ch->desc->writing_note = NOTING_BULLETIN_SUBJECT;  
       		  break;


		case 'h':
		case 'H':
		  write_to_buffer( d, CLR, 0 );
		  write_to_buffer( d, HWHITE, 0 );
	          write_to_buffer( d, "---------------===============> Note System <==============---------------\n\r\n\r", 0 );
		  write_to_buffer( d, NTXT, 0 );
		  write_to_buffer( d, "Would you like to obtain:\n\r", 0 );
		  write_to_buffer( d, "               1.  Help with understanding BOARDS.\n\r", 0 );
		  write_to_buffer( d, "               2.  Help with understanding PERSON-TO-PERSON notes.\n\r", 0 );
		  write_to_buffer( d, "               3.  Help with CONFIGURING your note system.\n\r", 0 );
		  write_to_buffer( d, "               4.  Help with understanding BULLETINS.\n\r", 0 );
		  write_to_buffer( d, "               5.  Help on how to use the note EDITOR.\n\r\n\r", 0 );
		  write_to_buffer( d, "               6.  Information about the different boards,\n\r", 0 );
		  write_to_buffer( d, "                   and which ones to which you should subscribe\n\r", 0 );
		  write_to_buffer( d, "               7.  Common problems/troubleshooting solutions\n\r", 0 );
		  write_to_buffer( d, "               8.  General Information ABOUT the Maestro Note System.\n\r\n\r", 0 );
		  write_to_buffer( d, "   [1-8 or <M>ain menu]: ", 0 );
		  ch->desc->writing_note = NOTING_HELP_QUESTION;
		  break;

		case 'l':
		case 'L':
		  write_to_buffer( d, CLR, 0 );
		  write_to_buffer( d, HWHITE, 0 );
	          write_to_buffer( d, "---------------==============> Note System <===============---------------\n\r\n\r", 0 );
		  write_to_buffer( d, NTXT, 0 );
		  do_note( ch, "list" );
		  write_to_buffer( d, "\n\r\n\r", 0 );
		  write_to_buffer( d, "Would you like to:\n\r", 0 );
		  write_to_buffer( d, "          1.  Read a note on this list\n\r", 0 );
		  write_to_buffer( d, "          2.  Remove a note on this list\n\r\n\r", 0 );
		  write_to_buffer( d, "     [1-2 or <M>ain menu]: ", 0 );
		  ch->desc->writing_note = NOTING_LIST_QUESTION;
		  break;
		

	        case 'x':
		case 'X':
		  write_to_buffer( d, CLR, 0 );
		  write_to_buffer( d, "Resuming regular game play.\n\r", 0 );

#if defined (rom24mae)
		  if (strcmp( d->character->pcdata->buffer->string, "" ) )
		   {
		     write_to_buffer( d, "\n\rInformation from the MUD has been saved while you were noting:\n\r--------------------------------------------------------------\n\r", 0 );	
		     do_replay( ch, "" );
		   }
#endif
		  ch->desc->writing_note = 0;
		  ch->desc->connected = CON_PLAYING;
		  REMOVE_BIT( ch->note_config, NOTE_CONFIG_CURRENTLY_NOTING );
		  return;

		default:
		  write_to_buffer( d, "\n\rYou must enter a valid choice: ", 0 );
		  ch->desc->writing_note = NOTING_NO_ARGUMENT;
	          return;
             }
	 break;

/* Note List cases */

	case NOTING_LIST_QUESTION:
	  switch(*argument)
	    {
		case '1':
		    write_to_buffer( d, "\n\r\n\rWhich number note would you like to display? [ # , <L>ist or <M>ain menu]? ", 0 );
		    ch->desc->writing_note = NOTING_READ_PREVIOUS;
		    break;

		case '2':
		  write_to_buffer( d,"\n\r\n\rDelete which note from your list? [# , <L>ist or <M>ain menu]: ", 0 );
		  ch->desc->writing_note = NOTING_QUESTION_DELETE_NOTE;
		  break;

		case 'm':
		case 'M':
		default:
		  do_note( ch, "" );
		  return;
            }
	break;		


/* Note Help cases */

	case NOTING_HELP_QUESTION:
	    switch(*argument)
	      {
		  case 'm':
		  case 'M':
		  case 's':
		  case 'S':
		    do_note( ch, "" );
		    return;

		  case 'q':
		  case 'Q':
		    write_to_buffer( ch->desc, CLR, 0 );
		    write_to_buffer( ch->desc, "Resuming regular game play.\n\r", 0 );

#if defined (rom24mae)

		  if ( strcmp( ch->pcdata->buffer->string, "" ) )
		   {
		     write_to_buffer( ch->desc, "\n\rInformation from the MUD has been saved while you were noting:\n\r--------------------------------------------------------------\n\r", 0 );	
		     do_replay( ch, "" );
		   }
#endif
		    ch->desc->writing_note = 0;
		    ch->desc->connected = CON_PLAYING;
		    REMOVE_BIT(ch->note_config, NOTE_CONFIG_CURRENTLY_NOTING );
		    return;

	  	  default:
		  write_to_buffer( d, CLR, 0 );
		  write_to_buffer( d, HWHITE, 0 );
	          write_to_buffer( d, "---------------===============> Note System <==============---------------\n\r\n\r", 0 );
		  write_to_buffer( d, NTXT, 0 );
		  write_to_buffer( d, "Help files not yet available -- Press <RETURN> to continue.", 0 );
		  ch->desc->writing_note = NOTING_CONTINUE;
		  return;
              }
         break;
	   

	case NOTING_HELP_CONTINUE:
	    switch(*argument)
               {	
		 case 'y':
		 case 'Y':
		  write_to_buffer( d, CLR, 0 );
		  write_to_buffer( d, HWHITE, 0 );
	          write_to_buffer( d, "---------------==============> Note System <===============---------------\n\r\n\r", 0 );
		  write_to_buffer( d, NTXT, 0 );
		  write_to_buffer( d, "Would you like to obtain:\n\r", 0 );
		  write_to_buffer( d, "               1.  Help with understanding boards\n\r", 0 );
		  write_to_buffer( d, "               2.  Help with understanding person-to-person notes\n\r", 0 );
		  write_to_buffer( d, "               3.  Help with configuring your note system.\n\r", 0 );
		  write_to_buffer( d, "               4.  Information about the different boards,\n\r", 0 );
		  write_to_buffer( d, "                   and which ones to which you should subscribe\n\r\n\r", 0 );
		  write_to_buffer( d, "               5.  Help on how to use the note editor.\n\r", 0 );
		  write_to_buffer( d, "               6.  Common problems/troubleshooting solutions\n\r", 0 );
		  write_to_buffer( d, "               7.  General Information ABOUT AVATAR's Note System.\n\r\n\r", 0 );
		  write_to_buffer( d, "   [1-7 or <M>ain menu]: ", 0 );
		  ch->desc->writing_note = NOTING_HELP_QUESTION;
		  break;
		    
	     case 'n':
	     case 'N':
		 do_note( ch, "" );
		 return;
       
	     default:
		 write_to_buffer( d, "\n\rPlease enter a Y or N: ", 0 );
		 ch->desc->writing_note = NOTING_HELP_CONTINUE;
		 return;
	  }
	
	break;

/* Note List cases */

	case NOTING_LIST_BULLETIN_QUESTION:
	  switch(*argument)
	    {
		case '1':
		    write_to_buffer( d, "\n\r\n\rWhich number note would you like to display? [ # , <L>ist or <M>ain menu]? ", 0 );
		    ch->desc->writing_note = NOTING_READ_BULLETIN_PREVIOUS;
		    break;

		case 'm':
		case 'M':
		default:
		  do_note( ch, "" );
		  return;
            }
	break;		


/* Note Config options */

	case NOTING_CONFIG_NO_ARGUMENT:
	   switch(*argument)
	    {

		case '1':
		   write_to_buffer( d, "\n\r\n\rYou currently subscribe to the following board(s):\n\r\n\r", 0 );
		   write_to_buffer( d, HWHITE, 0 );		
		   sprintf( buf, "%s", board_name( ch->board_config) );
		   write_to_buffer( d, buf, 0 );		
		   write_to_buffer( d, NTXT, 0 );
		 	
		   /* More ch->level qualifiers here */
		   if (IS_IMMORTAL(ch))
		    { if ( ch->board_config < (1+2+4+8+16+32+64+128+256) )
		        write_to_buffer( d, "\n\r\n\r(Note: You are NOT currently subscribing to all available boards.)\n\r\n\r", 0 );
		   else write_to_buffer( d, "\n\r\n\r(Note: You are currently subscribing to all available boards.)\n\r\n\r", 0 );
		    }
		   else
		    {
		       if (ch->board_config < 1+2+4 )
		          write_to_buffer( d, "\n\r\n\r(Note: You are NOT currently subscribing to all available boards.)\n\r\n\r", 0 );
   		       else write_to_buffer( d, "\n\r\n\r(Note: You are currently subscribing to all available boards.)\n\r\n\r", 0 );
		    }

	           write_to_buffer( d, "\n\r\n\rWould you like to change this? [<Y>es, <N>o, <M>ain menu]: ", 0 );		
		   ch->desc->writing_note = NOTING_CONFIG_QUESTION_CHANGE;
		   break;

	
                case '2':
         	   write_to_buffer( d, CLR, 0 );
		   write_to_buffer( d, HWHITE, 0 );
		   write_to_buffer( ch->desc, "---------------===============> Note System <==============---------------\n\r\n\r", 0 );
		   write_to_buffer( d, NTXT, 0 );
		   write_to_buffer( d, "\n\rThere are some MUD clients and/or TELNET clients that\n\r", 0 );
		   write_to_buffer( d, "do not echo each line of text as you enter it into the note\n\r", 0 );
		   write_to_buffer( d, "editor.  If the note system editor (ie, when you type\n\r", 0 );
		   write_to_buffer( d, "in a note) is working perfectly for you, answer <N>o\n\r", 0 );
		   write_to_buffer( d, "to the question below.  However, if you're having problems\n\r", 0 );
		   write_to_buffer( d, "'seeing' what you're writing while using the note editor,\n\r", 0 );
		   write_to_buffer( d, "try answering <Y>es to the question below.\n\r", 0 );
		   write_to_buffer( d, "    If you still have problems, contact an immortal for\n\r", 0 );
		   write_to_buffer( d, "an explanation of this configuration option.\n\r\n\r", 0 );
		   write_to_buffer( d, "\n\rDo you want an echo of each line entered in the string editor? [y/n]: ", 0 );
		   ch->desc->writing_note = NOTING_CONFIG_QUESTION_ECHO;
		   break;

 		case 'm':
		case 'M':
        	default:
		   ch->desc->writing_note = 0;
		   ch->desc->connected = CON_PLAYING;
		   do_note( ch, "" );
		   return;
	    }

	 break;


     case NOTING_CONFIG_QUESTION_ECHO:
	switch(*argument)
	  {
	     case 'y':
	     case 'Y':
		write_to_buffer( d, "\n\r\n\rYou will now receive an echo of each line entered in the note\n\r", 0 );
		write_to_buffer( d, "editor.  If you have any problems with this setting, please\n\r", 0 );
		write_to_buffer( d, "return to this configuration option and try answering <N>o.\n\r\n\r", 0 );
		write_to_buffer( d, "Press <return> to continue: ", 0 );
		SET_BIT( ch->note_config, NOTE_CONFIG_CLIENT );
		ch->desc->writing_note = NOTING_CONTINUE;
		break;

	     case 'n':
	     case 'N':
	        write_to_buffer( d, "\n\r\n\rYou will not receive any text echoes while using the note\n\r", 0 );
		write_to_buffer( d, "editor.  This is the default setting, and should work with most\n\r", 0 );
		write_to_buffer( d, "players.  If you still have problems with using the note editor,\n\r", 0 );
		write_to_buffer( d, "please try answering <Y>es to this configuration option question,\n\r", 0 );
		write_to_buffer( d, "and/or talk to an immortal.\n\r\n\r", 0 );
		write_to_buffer( d, "Press <return> to continue: ", 0 );
		REMOVE_BIT( ch->note_config, NOTE_CONFIG_CLIENT );
		ch->desc->writing_note = NOTING_CONTINUE;
		break;

	    default:
		write_to_buffer( d, "\n\r\n\rPlease answer either Y or N: ", 0 );
		ch->desc->writing_note = NOTING_CONFIG_QUESTION_ECHO;
		return;
         }
     break;



     case NOTING_CONFIG_QUESTION_CHANGE:
	switch(*argument)
	  {
	     case 'y':
	     case 'Y':
		write_to_buffer( d, "\n\r\n\rWould you like to <A>dd or <R>emove a board\n\rfrom your subscription list? [a/r]: ", 0 );
		ch->desc->writing_note = NOTING_CONFIG_QUESTION_ADD_REMOVE;
		break;

	    case 'n':
	    case 'N':
	        do_note( ch, "" );
		return;

	    case 'm':
	    case 'M':
		do_note( ch, "" );
		return;

	    default:
		write_to_buffer( d, "\n\rPlease answer <y>es, <n>o, or <M>ain menu: ", 0 );
		ch->desc->writing_note = NOTING_CONFIG_QUESTION_CHANGE;
		return;
          }
	break;

     case NOTING_CONFIG_QUESTION_ADD_REMOVE:
	switch(*argument)
	  {
		case 'a':
		case 'A':
		   write_to_buffer( d, CLR, 0 );
		   write_to_buffer( d, "Here are the Boards currently available for you to subscribe:\n\r\n\r", 0);

		   /* More ch->level qualifying here */

		   write_to_buffer( d, "0  - GENERAL\n\r", 0 );
		   write_to_buffer( d, "1  - PERSONAL\n\r", 0 );
		   write_to_buffer( d, "2  - QUEST\n\r", 0 );
		   write_to_buffer( d, "3  - CREATIVITY\n\r", 0 );
		if ( IS_IMMORTAL(ch))
		 {
		   write_to_buffer( d, "\n\r4  - IMMORTAL\n\r", 0 );
		   write_to_buffer( d, "5  - BUILDER\n\r", 0 );		
		   write_to_buffer( d, "6  - CODER\n\r", 0 );
		   write_to_buffer( d, "7  - SENIOR\n\r", 0 );
		   write_to_buffer( d, "8  - EXEC\n\r\n\r", 0 );
		   write_to_buffer( d, "Which board do you want to add? [0-9 or <M>ain menu]: ", 0 );
		 }
		else 
		   write_to_buffer( d, "\n\rWhich board do you want to add? [0-3 or <M>ain menu]: ", 0 );  
		   ch->desc->writing_note = NOTING_CONFIG_WHICH_ADD;
		   break;

	     case 'r':
	     case 'R':
		   write_to_buffer( d, "\n\r\n\rEnter the name of the board you wish to\n\rremove from your list (lowercase): ", 0 );
		   ch->desc->writing_note = NOTING_CONFIG_WHICH_REMOVE;
		   break;

	     default:
		   write_to_buffer( d, "\n\rPlease enter an A or R: ", 0 );
		   ch->desc->writing_note = NOTING_CONFIG_QUESTION_ADD_REMOVE;
		   return;
	  }
	break;


   case NOTING_CONFIG_WHICH_ADD:
	switch(*argument)
	  {
		case '0':
		    ch->board_config += BOARD_GENERAL;
		    break;
		case '1':
		    ch->board_config += BOARD_PERSONAL;
		    break;
		case '2':
		    ch->board_config += BOARD_QUEST;
		    break;
		case '3':
		    ch->board_config += BOARD_CREATIVITY;		
		    break;
		case 'm':
		case 'M':
		    do_note(ch, "" );
		    return;
		case '4':
		    if (IS_IMMORTAL(ch))
		       ch->board_config += BOARD_IMMORTAL;
		    else {
		     write_to_buffer( ch->desc, "\n\rYou cannot subscribe to an immortal board.\n\rPress <return> to continue.", 0 );
		     ch->desc->writing_note = NOTING_CONFIG_NO_ARGUMENT; }
		    break;
		case '5':
		    if (IS_IMMORTAL(ch))
		       ch->board_config += BOARD_BUILDER;
		    else {
		     write_to_buffer( ch->desc, "\n\rYou cannot subscribe to an immortal board.\n\rPress <return> to continue.", 0 );
		     ch->desc->writing_note = NOTING_CONFIG_NO_ARGUMENT; }
		    break;
		case '6':
		    if (IS_IMMORTAL(ch))
		       ch->board_config += BOARD_CODER;
		    else {
		     write_to_buffer( ch->desc, "\n\rYou cannot subscribe to an immortal board.\n\rPress <return> to continue.", 0 );
		     ch->desc->writing_note = NOTING_CONFIG_NO_ARGUMENT; }
		    break;
		case '7':
		    if (IS_IMMORTAL(ch))
		       ch->board_config += BOARD_SENIOR;
		    else {
		     write_to_buffer( ch->desc, "\n\rYou cannot subscribe to an immortal board.\n\rPress <return> to continue.", 0 );
		     ch->desc->writing_note = NOTING_CONFIG_NO_ARGUMENT; }
		    break;
		case '8':
		    if (IS_IMMORTAL(ch))
		       ch->board_config += BOARD_EXEC;
		    else {
		     write_to_buffer( ch->desc, "\n\rYou cannot subscribe to an immortal board.\n\rPress <return> to continue.", 0 );
		     ch->desc->writing_note = NOTING_CONFIG_NO_ARGUMENT; }
		    break; 
		default:
		    if (IS_IMMORTAL(ch)) 
		      write_to_buffer( d, "\n\rPlease enter a number from 0 to 8: ", 0 );
		    else write_to_buffer( d, "\n\rPlease enter a number from 0 to 3: ", 0 );
		    ch->desc->writing_note = NOTING_CONFIG_WHICH_ADD;
		    return;
	     }

	write_to_buffer( d, "\n\rBoard added to your subscription list.\n\r", 0 );
	write_to_buffer( d, "Press <return> to continue.", 0 );
	ch->desc->writing_note = NOTING_CONFIG_NO_ARGUMENT;
	break;


    case NOTING_CONFIG_WHICH_REMOVE:
	  if ( argument[0] == '\0' )
	    {
	      write_to_buffer( d, "\n\rNo Boards removed from subscription list.\n\r", 0 );
	      write_to_buffer( d, "\n\rPress <return> to continue", 0 );
	      do_note( ch, "" );
	      return;
            }

	  if ( !board_value( argument ) )
	    {
	      write_to_buffer( d, "\n\rThat is not a valid choice.\n\r", 0 );
	      write_to_buffer( d, "Press <return> to continue.", 0 );
	      ch->desc->writing_note = NOTING_CONTINUE;
	      return;
            }

	  ch->board_config -= board_value( argument );
	  
	  sprintf( buf, str_dup( argument) );
	  write_to_buffer( d, HWHITE, 0 );
	  write_to_buffer( d, capitalize(buf), 0 );
	  write_to_buffer( d, NTXT, 0 );
	  write_to_buffer( d, " removed.  Press <return> to continue.", 0 );
	  ch->desc->writing_note = NOTING_CONTINUE;
	  return;
     

     case NOTING_CONTINUE:
	switch(*argument)
	   {
		default:
		   do_note( ch, "" );
		   return;
	   }
        break;


     case NOTING_CONFIG_CHANGE:
	switch(*argument)
	  {
	     default:
		do_note( ch, "" );	
		return;
          }
        break;


/* Noting Delete cases */

     case NOTING_QUESTION_DELETE_NOTE:
         if ( !str_cmp( argument, "L" ) || !str_cmp( argument, "l" ))
	   {
	     do_note( ch, "list" );
	     write_to_buffer( d, "\n\r\n\rDelete which note? [# , <L>ist or <M>ain menu]: ", 0 );
	     ch->desc->writing_note = NOTING_QUESTION_DELETE_NOTE;
	     return;
	   }
	
	 else if ( !str_cmp( argument, "m" ) || !str_cmp( argument, "M" ))
	   {
	      do_note( ch, "" );
	      return;
	   }

	 else
	   {
	      sprintf( tmp, "remove %d", atoi(argument) );
	      do_note( ch, tmp );
	      sprintf( buf, "\n\r\n\rNote #%d removed.  Press <return> to continue.", atoi( argument ) );
	      write_to_buffer( d, buf, 0 );
	      ch->desc->writing_note = NOTING_CONTINUE;
	      return;
           }

       break;


/* Bulletin Read cases */

     case NOTING_READ_BULLETIN_PREVIOUS:
	 
	if ( !str_cmp( argument, "L" ) || !str_cmp( argument, "l" ) )
	  {
	    do_note( ch, "listb" );
	    write_to_buffer( d, "\n\r\n\rWhich bulletin would you like to read? [# , <L>ist, or <M>ain menu]: ", 0 );
	    ch->desc->writing_note = NOTING_READ_BULLETIN_PREVIOUS;
	    return;
	  }

	else if ( !str_cmp( argument, "m") || !str_cmp( argument, "M" ) 
				           || argument[0] == '\0' )
	  {
	    do_note( ch, "" );
	    return;
	  }


	else
	  {
	     sprintf( tmp, "readb %d", atoi(argument) );
	     do_note( ch, tmp );
	     write_to_buffer( d, "\n\r\n\rPress <return> to continue.", 0 );
	     ch->desc->writing_note = NOTING_CONTINUE;
	     return;
	  }

       break;


/* Note Read cases */

     case NOTING_READ_PREVIOUS:
	 
	if ( !str_cmp( argument, "L" ) || !str_cmp( argument, "l" ) )
	  {
	    do_note( ch, "list" );
	    write_to_buffer( d, "\n\r\n\rWhich note would you like to read? [# , <L>ist, or <M>ain menu]: ", 0 );
	    ch->desc->writing_note = NOTING_READ_PREVIOUS;
	    return;
	  }

	else if ( !str_cmp( argument, "m") || !str_cmp( argument, "M" ) 
				           || argument[0] == '\0' )
	  {
	    do_note( ch, "" );
	    return;
	  }


	else
	  {
	     sprintf( tmp, "read %d", atoi(argument) );
	     do_note( ch, tmp );
	     write_to_buffer( d, "\n\r\n\rPress <return> to continue.", 0 );
	     ch->desc->writing_note = NOTING_CONTINUE;
	     return;
	  }

       break;


     case NOTING_QUESTION_READ_NEXT:
	switch(*argument)
	  {
		case 'y':
	 	case 'Y':
		   write_to_buffer( d, CLR, 0 );
		   do_note( ch, "read" );
		   write_to_buffer( d, "\n\r\n\r", 0 );
		   write_to_buffer( d, "Would you like to read your next unread note,\n\r", 0);
		   write_to_buffer( d, "or return to the main menu? [y/n/M]: ", 0 );    
		   ch->desc->writing_note = NOTING_QUESTION_READ_NEXT;
		   return;

	  	case 'm':
		case 'M':
	        case 'n':
		case 'N':
		default:
		   do_note( ch, "" );
		   return;
           }
	break;


     case NOTING_QUESTION_READ_NEXT_SPECIAL:
	switch (*argument)
	 {
	     case 'y':
	     case 'Y':
		 write_to_buffer( d, CLR, 0 );
		
	         buf1[0] = '\0';
	         for ( pnote = note_list; pnote; pnote = pnote->next )
	           {
			if ( is_note_to( ch, pnote )
		    	&& str_cmp( ch->name, pnote->sender )
		    	&& ch->last_note_special < pnote->date_stamp 
			&& IS_SET(pnote->special, NOTE_SPECIAL_PERSON_TO_PERSON))
		     {
		        break;
		     }
	           }
	         if ( pnote )
	           {
		        sprintf( buf, "Number: %4d                     Board: %s\n\rFrom: %10s                    To: %s\n\rDate:    %s\n\rSubject: %s\n\r==============================================================\n\r",
				  pnote->vnum,
				  board_name(pnote->board),
				  pnote->sender,
				  pnote->to_list,
				  pnote->date,
				  pnote->subject );
			strcat( buf1, buf );
			strcat( buf1, pnote->text );
			ch->last_note_special = UMAX( ch->last_note_special, pnote->date_stamp );
			send_to_char( buf1, ch );
	           }
	       else 
		  send_to_char( "There are no unread notes for you.\n\r", ch );
	       	
		write_to_buffer( d, "\n\r\n\r", 0 );
		write_to_buffer( d, "Would you like to read your next unread person-to-person note? [y/n] : ", 0 );
		ch->desc->writing_note = NOTING_QUESTION_READ_NEXT_SPECIAL;
		break;
		
	   case 'n':
	   case 'N':
	   default:
		do_note( ch, "" );
		return;
	}
    break;
	       
     case NOTING_QUESTION_READ_NEXT_BULLETIN:
	switch (*argument)
	 {
	     case 'y':
	     case 'Y':
		 write_to_buffer( d, CLR, 0 );
		
	         buf1[0] = '\0';
	         for ( pnote = note_list; pnote; pnote = pnote->next )
	           {
			if ( is_note_to( ch, pnote )
		    	&& str_cmp( ch->name, pnote->sender )
		    	&& ch->last_note_bulletin < pnote->date_stamp 
			&& IS_SET(pnote->special, NOTE_SPECIAL_IS_BULLETIN))
		     {
		        break;
		     }
	           }
	         if ( pnote )
	           {
		        sprintf( buf, "Number: %4d                     Board: %s\n\rFrom: %10s                    To: %s\n\rDate:    %s\n\rSubject: %s\n\r==============================================================\n\r",
				  pnote->vnum,
				  board_name(pnote->board),
				  pnote->sender,
				  pnote->to_list,
				  pnote->date,
				  pnote->subject );
			strcat( buf1, buf );
			strcat( buf1, pnote->text );
			ch->last_note_bulletin = UMAX( ch->last_note_bulletin, pnote->date_stamp );
			send_to_char( buf1, ch );
	           }
	       else 
		  send_to_char( "There are no unread notes for you.\n\r", ch );
	       	
		write_to_buffer( d, "\n\r\n\r", 0 );
		write_to_buffer( d, "Would you like to read your next unread bulletin? [y/n] : ", 0 );
		ch->desc->writing_note = NOTING_QUESTION_READ_NEXT_BULLETIN;
		break;
		
	   case 'n':
	   case 'N':
	   default:
		do_note( ch, "" );
		return;
	}
    break;
	

/* All the writing cases */

    case NOTING_TO_QUESTION:
	switch(*argument)
	  {
		case 'y':
		case 'Y':
		   SET_BIT( ch->pnote->special, NOTE_SPECIAL_PERSON_TO_PERSON);
		   write_to_buffer( d, "\n\r\n\r<<Note will be sent priority>>\n\r\n\r", 0 );
                   write_to_buffer( d, "Please enter the recipient(s) of your note: ", 0 );	
                   ch->desc->writing_note = NOTING_TO;
		   break;

		case 'n':
		case 'N':
		  write_to_buffer( d, "\n\r\n\rPlease enter the recipient(s) of your note (default:", 0 );
		  write_to_buffer( d, HWHITE, 0 );
		  write_to_buffer( d, " ALL", 0 );
		  write_to_buffer( d, NTXT, 0 );
		  write_to_buffer( d, "): ", 0 );	
                  ch->desc->writing_note = NOTING_TO;
		  break;
	
		case 'M':
		case 'm':
		  do_note( ch, "" );
		  break;

		default:
		  write_to_buffer( d, "\n\r\n\rPlease enter a Y or N: ", 0 );
		  ch->desc->writing_note = NOTING_TO_QUESTION;
		  return;
	    }
	break;





    case NOTING_TO:
	if ( !str_cmp( argument, "M" ) || !str_cmp( argument, "m" ) )
	 {
	   do_note(ch, "" );
	   return;
	 }

	if ( argument[0] == '\0' || !str_cmp( argument, "all") || !str_cmp( argument, "ALL") || !str_cmp(argument, "All" ) )
	  {
          	free_string( ch->pnote->to_list );

		if ( IS_SET(ch->pnote->special, NOTE_SPECIAL_PERSON_TO_PERSON) )
		 {
		   write_to_buffer( d, "\n\rYou are writing a person-to-person note.  You cannot\n\r", 0 );
		   write_to_buffer( d, "send this to all.\n\r", 0 );
		   write_to_buffer( d, "\n\rPlease enter the recipient of your note: ", 0 );
		   ch->desc->writing_note = NOTING_TO;
		   return;
 		 }		

	        ch->pnote->to_list = str_dup( "all");


                write_to_buffer( d, "\n\rHere are the boards on which you subscribe:\n\r", 0 );
		write_to_buffer( d, HWHITE, 0 );
		write_to_buffer( d, board_name(ch->board_config), 0 );
		write_to_buffer( d, NTXT, 0 );
		write_to_buffer( d, "\n\r           (default:", 0 );
		write_to_buffer( d, HWHITE, 0 );
		write_to_buffer( d, "  GENERAL", 0 );
		write_to_buffer( d, NTXT, 0 );
		write_to_buffer( d, " )", 0 );
		write_to_buffer( d, "\n\r\n\rPlease type the board name on which you wish to post (lowercase): ", 0 );
		d->writing_note = NOTING_BOARD;
	   	return;
	  }

	free_string( ch->pnote->to_list );


	ch->pnote->to_list = str_dup( argument );	
       
	write_to_buffer( d, "\n\rHere are the boards on which you subscribe:\n\r", 0 );
	write_to_buffer( d, HWHITE, 0 );
	write_to_buffer( d, board_name(ch->board_config), 0 );
	write_to_buffer( d, NTXT, 0 );
	write_to_buffer( d, "\n\r           (default:", 0 );
	write_to_buffer( d, HWHITE, 0 );
	write_to_buffer( d, "  GENERAL", 0 );
	write_to_buffer( d, NTXT, 0 );
	write_to_buffer( d, " )", 0 );		
	write_to_buffer( d, "\n\rPlease type the board name on which you wish to post (lowercase): ", 0 );
	d->writing_note = NOTING_BOARD;
       	break;


    case NOTING_BOARD:
	if ( !str_cmp( argument, "M" ) || !str_cmp( argument, "m" ) )
	 {
	   do_note(ch, "" );
	   return;
	 }

	if ( argument[0] == '\0' )
	  {	

	     ch->pnote->board = BOARD_GENERAL;
	     
	     write_to_buffer( d, "\n\rPlease enter a subject for your note (default:", 0 );
	     write_to_buffer( d, HWHITE, 0 );
	     write_to_buffer( d, " <no subject>", 0 );
	     write_to_buffer( d, NTXT, 0 );
	     write_to_buffer( d, "): ", 0 );
	     d->writing_note = NOTING_SUBJECT;
	     return;
	  }

	  if ( !board_value( argument ) )
	    {
	      write_to_buffer( d, "\n\rThat is not a valid choice.\n\r", 0 );
              write_to_buffer( d, "\n\r\n\rHere are the boards on which you subscribe:\n\r", 0 );
	      write_to_buffer( d, HWHITE, 0 );
	      write_to_buffer( d, board_name(ch->board_config), 0 );
	      write_to_buffer( d, NTXT, 0 );
	      write_to_buffer( d, "\n\r           (default:", 0 );
	      write_to_buffer( d, HWHITE, 0 );
	      write_to_buffer( d, "  GENERAL", 0 );
	      write_to_buffer( d, NTXT, 0 );
	      write_to_buffer( d, " )", 0 );	
	      write_to_buffer( d, "\n\r\n\rPlease type the board name on which you wish to post (lowercase): ", 0 );
	      d->writing_note = NOTING_BOARD;
	      return;
	    }

	  if ( !str_cmp( ch->pnote->to_list, "all" ) 
	    && !str_cmp( argument, "personal" ) )
	    {
	      write_to_buffer( d, "You cannot write a note to all on the personal board.\n\r", 0 );
              write_to_buffer( d, "\n\rHere are the boards on which you subscribe:\n\r", 0 );
	      write_to_buffer( d, HWHITE, 0 );
	      write_to_buffer( d, board_name(ch->board_config), 0 );
	      write_to_buffer( d, NTXT, 0 );
	      write_to_buffer( d, "\n\r           (default:", 0 );
	      write_to_buffer( d, HWHITE, 0 );
	      write_to_buffer( d, "  GENERAL", 0 );
	      write_to_buffer( d, NTXT, 0 );
	      write_to_buffer( d, " )", 0 );	
	      write_to_buffer( d, "\n\rPlease type the board name on which you wish to post (lowercase): ", 0 );
	      d->writing_note = NOTING_BOARD;
	      break;
	    }	

	     ch->pnote->board = board_value( argument );
	     
	     write_to_buffer( d, "\n\rPlease enter a subject for your note (default:", 0 );
	     write_to_buffer( d, HWHITE, 0 );
	     write_to_buffer( d, " <no subject>)", 0 );
	     write_to_buffer( d, NTXT, 0 );
	     write_to_buffer( d, ": ", 0 );
	     d->writing_note = NOTING_SUBJECT;
	break;


    case NOTING_SUBJECT:
	if ( !str_cmp( argument, "M" ) || !str_cmp( argument, "m" ) )
	 {
	   do_note(ch, "" );
	   return;
	 }
	if ( argument[0] == '\0' )
	  {	
	     free_string( ch->pnote->subject );
	     ch->pnote->subject = str_dup( "<no subject>" );

	     string_append( ch, &ch->pnote->text );
	     return;
	  }

	     free_string( ch->pnote->subject );
	     ch->pnote->subject = str_dup( argument );
	     
	     string_append( ch, &ch->pnote->text );
	break;


    case NOTING_EDIT:
	
	switch( *argument )
	  {
		case 'y':
		case 'Y':
			ch->pnote->next			= NULL;
			strtime				= ctime( &current_time );
			strtime[strlen(strtime)-1]	= '\0';
			free_string( ch->pnote->date );
			ch->pnote->date			= str_dup( strtime );
			ch->pnote->date_stamp           = current_time;

			
#if defined imcmae
		       /* handle IMC notes */

		       if (strchr(ch->pnote->to_list, '@')!=NULL)
         		  imc_post_mail(ch, ch->pnote->sender,
                          ch->pnote->to_list,
                          ch->pnote->date,
                          ch->pnote->subject,
			  ch->pnote->text );
#endif
			if ( !note_list )
			{
	    		   note_list	= ch->pnote;
			}
			else
			{
	    		for ( pnote = note_list; pnote->next; pnote = pnote->next );
			    pnote->next	= ch->pnote;
			}
			pnote		= ch->pnote;
			ch->pnote       = NULL;

			fclose( fpReserve ); 
			
			if ( !( fp = fopen( NOTE_V_FILE, "r" ) ) )
			  {
			  }
			else
			  {
			    pnote->vnum = fread_number( fp );
			    fclose(fp);
			  }


		        if ( !(fp = fopen( NOTE_V_FILE, "w" ) ) )
			  {
			  }
			else
			  {
			    fprintf( fp, "%d\n", pnote->vnum + 1 );
			    fclose(fp);
			  }

			if ( !( fp = fopen( NOTE_FILE, "a" ) ) )
			{
	    		perror( NOTE_FILE );
			}
			else
			{
			fprintf( fp, "Vnum    %d\n",  pnote->vnum	);
	    		fprintf( fp, "Sender  %s~\n", pnote->sender     );
	    		fprintf( fp, "Date    %s~\n", pnote->date       );
	    		fprintf( fp, "Stamp   %ld\n", (unsigned long)pnote->date_stamp );
	    		fprintf( fp, "To      %s~\n", pnote->to_list    );
			fprintf( fp, "Special %d \n", pnote->special    );
	    		fprintf( fp, "Board   %d\n",  pnote->board	    );
	    		fprintf( fp, "Subject %s~\n", pnote->subject    );
	    		fprintf( fp, "Text\n%s~\n\n", pnote->text       );
	    		fclose( fp );
			}
			fpReserve = fopen( NULL_FILE, "r" );
			write_to_buffer( ch->desc, CLR, 0 );
			send_to_char( "\n\rNote posted.  Resuming regular game play.\n\r", ch );


#if defined (rom24mae)

		  if ( strcmp( d->character->pcdata->buffer->string, "" ) )
		   {
		     write_to_buffer( d, "\n\rInformation from the MUD has been saved while you were noting:\n\r--------------------------------------------------------------\n\r", 0 );	
		     do_replay( ch, "" );
		   }
#endif
			d->writing_note = 0;
			d->connected	= CON_PLAYING;
		        REMOVE_BIT( ch->note_config, NOTE_CONFIG_CURRENTLY_NOTING );
			return;

		case 'n':
		case 'N':
		        free_string( ch->pnote->text    );
	    		free_string( ch->pnote->subject );
	    		free_string( ch->pnote->to_list );
	    		free_string( ch->pnote->date    );
	    		free_string( ch->pnote->sender  );
	    		ch->pnote->next	= note_free;
	    		note_free		= ch->pnote;
	    		ch->pnote		= NULL;
			write_to_buffer( ch->desc, CLR, 0 );			
			write_to_buffer( ch->desc, "\n\rNote Cleared.  Resuming regular game play.\n\r", 0 );

#if defined (rom24mae)

		  if (strcmp(d->character->pcdata->buffer->string, "" ) )
		   {
		     write_to_buffer( d, "\n\rInformation from the MUD has been saved while you were noting:\n\r--------------------------------------------------------------\n\r", 0 );	
		     do_replay( ch, "" );
		   }

#endif
		        d->writing_note = 0;
			d->connected = CON_PLAYING;
			REMOVE_BIT( ch->note_config, NOTE_CONFIG_CURRENTLY_NOTING );
			return;

		case 'c':
		case 'C':
		        free_string( ch->pnote->text    );
	    		free_string( ch->pnote->subject );
	    		free_string( ch->pnote->to_list );
	    		free_string( ch->pnote->date    );
	    		free_string( ch->pnote->sender  );
	    		ch->pnote->next	= note_free;
	    		note_free		= ch->pnote;
	    		ch->pnote		= NULL;

			write_to_buffer( d, CLR, 0 );
			send_to_char( "\n\rNote Cleared.  Resuming regular game play.\n\r", ch );

#if defined (rom24mae)

		  if (strcmp(d->character->pcdata->buffer->string, "" ) )
		   {
		     write_to_buffer( d, "\n\rInformation from the MUD has been saved while you were noting:\n\r--------------------------------------------------------------\n\r", 0 );	
		     do_replay( ch, "" );
		   }
#endif
		        d->writing_note = 0;
			d->connected = CON_PLAYING;
			REMOVE_BIT( ch->note_config, NOTE_CONFIG_CURRENTLY_NOTING );
			return;

		default:
			send_to_char( "\n\rPlease enter a Y, N, or C: ", ch );
			d->writing_note = NOTING_EDIT;
			return;
		}
			

/* All the bulletin writing cases */


    case NOTING_BULLETIN_SUBJECT:
	if ( argument[0] == '\0' )
	  {	
	     free_string( ch->pnote->subject );
	     ch->pnote->subject = str_dup( "<no subject>" );

	     string_append( ch, &ch->pnote->text );
	     return;
	  }

	     free_string( ch->pnote->subject );
	     ch->pnote->to_list = str_dup( "all" );
	     ch->pnote->subject = str_dup( argument );
	     ch->pnote->board = 1;
	     SET_BIT( ch->pnote->special, NOTE_SPECIAL_IS_BULLETIN );
	     
	     string_append( ch, &ch->pnote->text );
	break;


    default:
        bug( "Nanny: bad d->writing_note %d.", d->writing_note );
        close_socket( d );

     }


   return;
}

bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if ( !str_cmp( ch->name, pnote->sender ) )
	return TRUE;

    if ( is_name( "all", pnote->to_list ) )
	return TRUE;

    if ( IS_HERO( ch ) && (   is_name( "immortal",  pnote->to_list )
			   || is_name( "immortals", pnote->to_list )
			   || is_name( "imm",       pnote->to_list )
			   || is_name( "immort",    pnote->to_list ) ) )
	
	return TRUE;

    if ( is_name( ch->name, pnote->to_list ) )
	return TRUE;

    return FALSE;
}



void note_attach( CHAR_DATA *ch )
{
    NOTE_DATA *pnote;

    if ( ch->pnote )
	return;

    if ( !note_free )
    {
	pnote	  = alloc_perm( sizeof( *ch->pnote ) );
    }
    else
    {
	pnote	  = note_free;
	note_free = note_free->next;
    }

    pnote->next		= NULL;
    pnote->sender	= str_dup( ch->name );
    pnote->date		= str_dup( "" );
    pnote->to_list	= str_dup( "" );
    pnote->subject	= str_dup( "" );
    pnote->vnum		= -1;
    pnote->board	= 0;
    pnote->text		= str_dup( "" );
    pnote->special      = 0;
    ch->pnote		= pnote;
    return;
}



void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    FILE      *fp;
    NOTE_DATA *prev;
    char      *to_list;
    char       to_new [ MAX_INPUT_LENGTH ];
    char       to_one [ MAX_INPUT_LENGTH ];

    /*
     * Build a new to_list.
     * Strip out this recipient.
     */
    to_new[0]	= '\0';
    to_list	= pnote->to_list;
    while ( *to_list != '\0' )
    {
	to_list	= one_argument( to_list, to_one );
	if ( to_one[0] != '\0' && str_cmp( ch->name, to_one ) )
	{
	    strcat( to_new, " "    );
	    strcat( to_new, to_one );
	}
    }

    /*
     * Just a simple recipient removal?
     */
    if ( str_cmp( ch->name, pnote->sender ) && to_new[0] != '\0' )
    {
	free_string( pnote->to_list );
	pnote->to_list = str_dup( to_new + 1 );
	return;
    }

    /*
     * Remove note from linked list.
     */
    if ( pnote == note_list )
    {
	note_list = pnote->next;
    }
    else
    {
	for ( prev = note_list; prev; prev = prev->next )
	{
	    if ( prev->next == pnote )
		break;
	}

	if ( !prev )
	{
	    bug( "Note_remove: pnote not found.", 0 );
	    return;
	}

	prev->next = pnote->next;
    }

    free_string( pnote->text    );
    free_string( pnote->subject );
    free_string( pnote->to_list );
    free_string( pnote->date    );
    free_string( pnote->sender  );
    pnote->special = 0;
    pnote->next	= note_free;
    note_free	= pnote;

    /*
     * Rewrite entire list.
     */
    fclose( fpReserve );
    if ( !( fp = fopen( NOTE_FILE, "w" ) ) )
    {
	perror( NOTE_FILE );
    }
    else
    {
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    fprintf( fp, "Vnum    %d\n",  pnote->vnum	    );
	    fprintf( fp, "Sender  %s~\n", pnote->sender     );
	    fprintf( fp, "Date    %s~\n", pnote->date       );
	    fprintf( fp, "Stamp   %ld\n", (unsigned long)pnote->date_stamp );
	    fprintf( fp, "To      %s~\n", pnote->to_list    );
	    fprintf( fp, "Special %d \n", pnote->special    );
	    fprintf( fp, "Board   %d\n", pnote->board 	    );
	    fprintf( fp, "Subject %s~\n", pnote->subject    );
	    fprintf( fp, "Text\n%s~\n\n", pnote->text       );
	}
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/* Date stamp idea comes from Alander of ROM */
void do_note( CHAR_DATA *ch, char *argument )
{
    NOTE_DATA *pnote;
    char       buf  [ MAX_STRING_LENGTH   ];
    char       buf1 [ MAX_STRING_LENGTH*7 ];
    char       arg  [ MAX_INPUT_LENGTH    ];
    int        anum;

    if ( IS_NPC( ch ) )
	return;

    argument = one_argument( argument, arg );
    smash_tilde( argument );



    if ( arg[0] == '\0' )
    {
	if ( ch->pnote )
	{
	    free_string( ch->pnote->text    );
	    free_string( ch->pnote->subject );
	    free_string( ch->pnote->to_list );
	    free_string( ch->pnote->date    );
	    free_string( ch->pnote->sender  );
	    ch->pnote->special = 0;
	    ch->pnote->board = 0;
	    ch->pnote->next	= note_free;
	    note_free		= ch->pnote;
	    ch->pnote		= NULL;
	}

         if ( IS_SET( ch->note_config, NOTE_CONFIG_FIRST_TIME) )
	   {
             	SET_BIT( ch->note_config, NOTE_CONFIG_CURRENTLY_NOTING );
		write_to_buffer( ch->desc, CLR, 0 );
		do_mhelp( ch, "* note_system_first_time" );
		REMOVE_BIT( ch->note_config, NOTE_CONFIG_FIRST_TIME );
	        ch->desc->connected = CON_NOTING;
		ch->desc->writing_note = NOTING_CONTINUE;
		return;
	   }
	sprintf( buf, "%sHelp%s", CLR, NTXT );
	write_to_buffer( ch->desc, CLR, 0 );
	write_to_buffer( ch->desc, HWHITE, 0 );
	write_to_buffer( ch->desc, "---------=========> Maestro Noting System -- Main Menu <=========---------\n\r\n\r", 0 ); 
	write_to_buffer( ch->desc, NTXT, 0 );
	write_to_buffer( ch->desc, "Would you like to:\n\r", 0 );
	write_to_buffer( ch->desc, "                   1 - Read unread notes written directly to you.\n\r", 0 );
	write_to_buffer( ch->desc, "                   2 - <R>ead unread notes on subscribed boards.\n\r", 0 );
	write_to_buffer( ch->desc, "                   3 - Read a <p>reviously read note\n\r", 0 );
	write_to_buffer( ch->desc, "                   4 - <W>rite a new note\n\r", 0 );
	write_to_buffer( ch->desc, "                   5 - <D>elete a note from your list\n\r", 0 );
	write_to_buffer( ch->desc, "                   L - List notes posted on subscribed boards\n\r\n\r", 0 );
	write_to_buffer( ch->desc, HWHITE, 0 );
	write_to_buffer( ch->desc, "--------------=============> System Bulletins <=============--------------\n\r", 0 ); 
	write_to_buffer( ch->desc, NTXT, 0 );
	write_to_buffer( ch->desc, "System Bulletins are special announcements posted by the administrators.\n\rThey", 0 );	
	write_to_buffer( ch->desc, " are considered priority, and should not be missed by any player.\n\r\n\r", 0 );
	write_to_buffer( ch->desc, "                   B - Read unread system bulletins.\n\r", 0 );
	write_to_buffer( ch->desc, "                   6 - List system bulletins\n\r", 0 );
	if ( get_trust(ch) >= CAN_POST_BUL )
	{
   	  write_to_buffer( ch->desc, "                   7 - Post a new system bulletin\n\r\n\r", 0 );
	  write_to_buffer( ch->desc, HWHITE, 0 );
	  write_to_buffer( ch->desc, "--------------=============>******************<=============--------------\n\r\n\r", 0 );
	  write_to_buffer( ch->desc, NTXT, 0 );
 	  write_to_buffer( ch->desc, "                   C - Configure your note/bulletin system preferences\n\r", 0 );
	  write_to_buffer( ch->desc, "                   H - Get HELP using the system.\n\r", 0 );
	  write_to_buffer( ch->desc, "                   X - Exit the note/bulletin system.\n\r\n\r", 0 );
	}
	else
        {
	  write_to_buffer( ch->desc, HWHITE, 0 );
	  write_to_buffer( ch->desc, "--------------=============>******************<==============--------------\n\r\n\r", 0 );
	  write_to_buffer( ch->desc, NTXT, 0 );
          write_to_buffer( ch->desc, "                   C - Configure your note/bulletin system preferences\n\r", 0 );
	  write_to_buffer( ch->desc, "                   H - Get HELP using the system.\n\r", 0 );
          write_to_buffer( ch->desc, "                   X - Exit the note/bulletin system.\n\r\n\r", 0 );
	}
	if ( get_trust(ch) >= CAN_POST_BUL )
	{
	  write_to_buffer( ch->desc, HWHITE, 0 );
	  write_to_buffer( ch->desc, "    [1-7, B, C, H, L, or X]: ", 0 );
	  write_to_buffer( ch->desc, NTXT, 0 );
	}
	else
	{
	  write_to_buffer( ch->desc, HWHITE, 0 );
	  write_to_buffer( ch->desc, "    [1-6, B, C, H, L, or X]: ", 0 );
	  write_to_buffer( ch->desc, NTXT, 0 );
	}	
	ch->desc->writing_note = NOTING_NO_ARGUMENT;
	SET_BIT( ch->note_config, NOTE_CONFIG_CURRENTLY_NOTING );
	ch->desc->connected = CON_NOTING;
	return;
    }
    

    if ( !str_cmp( arg, "list" ) )
    {
	buf1[0] = '\0';
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) 
	         && CAN_READ_BOARD(ch, pnote->board) 
		 && !IS_SET(pnote->special, NOTE_SPECIAL_IS_BULLETIN))
	    {
		sprintf( buf, "[%3d] From: %-11s      Subject: %-15s      Board: %s\n\r",
		 	pnote->vnum,
			pnote->sender,
			pnote->subject,
			board_name(pnote->board));
		strcat( buf1, buf );
	    }
	}
	strcat( buf1, HWHITE );
	strcat( buf1, "\n\r                            {End of note list}\n\r" );
	strcat( buf1, NTXT );
	send_to_char( buf1, ch );
	return;
    }
    if ( !str_cmp( arg, "listb" ) )
    {
	buf1[0] = '\0';
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) 
	         && CAN_READ_BOARD(ch, pnote->board) 
		 && IS_SET(pnote->special, NOTE_SPECIAL_IS_BULLETIN))
	    {
		sprintf( buf, "[%3d] From: %-11s      Subject: %-15s      Board: %s\n\r",
		 	pnote->vnum,
			pnote->sender,
			pnote->subject,
			board_name(pnote->board));
		strcat( buf1, buf );
	    }
	}
	strcat( buf1, HWHITE );
	strcat( buf1, "\n\r                            {End of bulletin list}\n\r" );
	strcat( buf1, NTXT );
	send_to_char( buf1, ch );
	return;
    }

    if ( !str_cmp( arg, "help" ) )
      {

		  write_to_buffer( ch->desc, CLR, 0 );
		  write_to_buffer( ch->desc, HWHITE, 0 );
	          write_to_buffer( ch->desc, "----------------================> Note System <================----------------\n\r\n\r", 0 );
		  write_to_buffer( ch->desc, NTXT, 0 );
		  write_to_buffer( ch->desc, "Would you like to obtain:\n\r", 0 );
		  write_to_buffer( ch->desc, "               1.  Help with understanding BOARDS.\n\r", 0 );
		  write_to_buffer( ch->desc, "               2.  Help with understanding PERSON-TO-PERSON notes.\n\r", 0 );
		  write_to_buffer( ch->desc, "               3.  Help with CONFIGURING your note system.\n\r", 0 );
		  write_to_buffer( ch->desc, "               4.  Help with understanding BULLETINS.\n\r", 0 );
		  write_to_buffer( ch->desc, "               5.  Help on how to use the note EDITOR.\n\r\n\r", 0 );
		  write_to_buffer( ch->desc, "               6.  Information about the different boards,\n\r", 0 );
		  write_to_buffer( ch->desc, "                   and which ones to which you should subscribe\n\r", 0 );
		  write_to_buffer( ch->desc, "               7.  Common problems/troubleshooting solutions\n\r", 0 );
		  write_to_buffer( ch->desc, "               8.  General Information ABOUT AVATAR's Note System.\n\r\n\r", 0 );
		  write_to_buffer( ch->desc, "   [1-8, <Q>uit, or <S>tart note system]: ", 0 );
		  ch->desc->writing_note = NOTING_HELP_QUESTION;
	          SET_BIT( ch->note_config, NOTE_CONFIG_CURRENTLY_NOTING );
	          ch->desc->connected = CON_NOTING;
	          return;
       }



    if ( !str_cmp( arg, "readb" ) )
    {
	bool fAll;

	if ( !str_cmp( argument, "all" ) )
	{
	    fAll = TRUE;
	    anum = 0;
	}
	else if ( argument[0] == '\0' || !str_prefix( argument, "next" ) )
	  /* read next unread note */
	{
	    buf1[0] = '\0';
	    for ( pnote = note_list; pnote; pnote = pnote->next )
	    {
		if ( is_note_to( ch, pnote )
		    && str_cmp( ch->name, pnote->sender )
		    && ch->last_note < pnote->date_stamp 
		    && IS_SET(pnote->special, NOTE_SPECIAL_IS_BULLETIN)
		    && !IS_SET(pnote->special, NOTE_SPECIAL_PERSON_TO_PERSON))
		{
		    break;
		}
	    }
	    if ( pnote )
	    {
	       if ( IS_SET( pnote->special, NOTE_SPECIAL_IS_BULLETIN ) )
		{
		    sprintf( buf, "Number: %4d                     Board: %s\n\rFrom: %10s                    To: %s\n\rDate:    %s\n\rSubject: %s\n\r==============================================================\n\r",
				  pnote->vnum,
				  board_name(pnote->board),
				  pnote->sender,
				  pnote->to_list,
				  pnote->date,
				  pnote->subject );
		strcat( buf1, buf );
		strcat( buf1, pnote->text );
	 	ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
		send_to_char( buf1, ch );
		return;
	     }
	    }
	    send_to_char( "There are no unread bulletins for you to read.\n\r", ch );
	    return;
	}
	else if ( is_number( argument ) )
	{
	    fAll = FALSE;
	    anum = atoi( argument );
	}
	else
	{
	    send_to_char( "Bulletin read which number?\n\r", ch );
	    return;
	}

	buf1[0] = '\0';
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) && CAN_READ_BOARD( ch, pnote->board) )
	    {
	        if ( pnote->vnum == anum || fAll )
		{
		  if ( IS_SET( pnote->special, NOTE_SPECIAL_IS_BULLETIN ) )
              	    sprintf( buf, "Number: %4d                     Board: %s\n\rFrom: %10s                    To: %s\n\rDate:    %s\n\rSubject: %s\n\r==============================================================\n\r",
				  pnote->vnum,
				  board_name(pnote->board),
				  pnote->sender,
				  pnote->to_list,
				  pnote->date,
				  pnote->subject );
		  else return;
		    strcat( buf1, buf );
		    strcat( buf1, pnote->text );
		    if ( !fAll )
		      send_to_char( buf1, ch );
		    else
		      strcat( buf1, "\n\r" );
		    ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
		    if ( !fAll )
		      return;
		}
	    }
	}
	if ( !fAll )
	    send_to_char( "That bulletin doesn't exist.\n\r", ch );
	else
	    send_to_char( buf1, ch );
	return;
    }


    if ( !str_cmp( arg, "read" ) )
    {
	bool fAll;

	if ( !str_cmp( argument, "all" ) )
	{
	    fAll = TRUE;
	    anum = 0;
	}
	else if ( argument[0] == '\0' || !str_prefix( argument, "next" ) )
	  /* read next unread note */
	{
	    buf1[0] = '\0';
	    for ( pnote = note_list; pnote; pnote = pnote->next )
	    {
		if ( is_note_to( ch, pnote )
		    && str_cmp( ch->name, pnote->sender )
		    && ch->last_note < pnote->date_stamp 
		    && !IS_SET(pnote->special, NOTE_SPECIAL_IS_BULLETIN)
		    && !IS_SET(pnote->special, NOTE_SPECIAL_PERSON_TO_PERSON))
		{
		    break;
		}
	    }
	    if ( pnote )
	    {
	       if ( !IS_SET( pnote->special, NOTE_SPECIAL_IS_BULLETIN ) )
		{
		    sprintf( buf, "Number: %4d                     Board: %s\n\rFrom: %10s                    To: %s\n\rDate:    %s\n\rSubject: %s\n\r==============================================================\n\r",
				  pnote->vnum,
				  board_name(pnote->board),
				  pnote->sender,
				  pnote->to_list,
				  pnote->date,
				  pnote->subject );
		strcat( buf1, buf );
		strcat( buf1, pnote->text );
	 	ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
		send_to_char( buf1, ch );
		return;
	     }
	    }
	    send_to_char( "There are no unread notes for you to read.\n\r", ch );
	    return;
	}
	else if ( is_number( argument ) )
	{
	    fAll = FALSE;
	    anum = atoi( argument );
	}
	else
	{
	    send_to_char( "Note read which number?\n\r", ch );
	    return;
	}

	buf1[0] = '\0';
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) && CAN_READ_BOARD( ch, pnote->board) )
	    {
	        if ( pnote->vnum == anum || fAll )
		{
		  if ( !IS_SET( pnote->special, NOTE_SPECIAL_IS_BULLETIN ) )
              	    sprintf( buf, "Number: %4d                     Board: %s\n\rFrom: %10s                    To: %s\n\rDate:    %s\n\rSubject: %s\n\r==============================================================\n\r",
				  pnote->vnum,
				  board_name(pnote->board),
				  pnote->sender,
				  pnote->to_list,
				  pnote->date,
				  pnote->subject );
		  else return;
		    strcat( buf1, buf );
		    strcat( buf1, pnote->text );
		    if ( !fAll )
		      send_to_char( buf1, ch );
		    else
		      strcat( buf1, "\n\r" );
		    ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
		    if ( !fAll )
		      return;
		}
	    }
	}

	if ( !fAll )
	    send_to_char( "That note doesn't exist.\n\r", ch );
	else
	    send_to_char( buf1, ch );
	return;
    }



    if ( !str_cmp( arg, "clear" ) )
    {
	if ( ch->pnote )
	{
	    free_string( ch->pnote->text    );
	    free_string( ch->pnote->subject );
	    free_string( ch->pnote->to_list );
	    free_string( ch->pnote->date    );
	    free_string( ch->pnote->sender  );
	    ch->pnote->special = 0;
	    ch->pnote->board = 0;
	    ch->pnote->next	= note_free;
	    note_free		= ch->pnote;
	    ch->pnote		= NULL;
	}
	send_to_char( "Note in progress cleared.\n\r", ch );
	if ( IS_SET( ch->note_config, NOTE_CONFIG_CURRENTLY_NOTING ))
	   REMOVE_BIT(ch->note_config, NOTE_CONFIG_CURRENTLY_NOTING );
	return;
    }

    if ( !str_cmp( arg, "show" ) )
    {
	if ( !ch->pnote )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}

	sprintf( buf, "%s: %s\n\rTo: %s\n\r",
		ch->pnote->sender,
		ch->pnote->subject,
		ch->pnote->to_list );
	send_to_char( buf, ch );
	send_to_char( ch->pnote->text, ch );

	return;
    }


    if ( !str_cmp( arg, "remove" ) )
    {
	if ( !is_number( argument ) )
	{
	    send_to_char( "Note remove which number?\n\r", ch );
	    return;
	}

	anum = atoi( argument );
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) && pnote->vnum == anum )
	    {
		note_remove( ch, pnote );
		send_to_char( "Note removed.\n\r", ch );
		return;
	    }
	}

	send_to_char( "No such note.\n\r", ch );
	return;
    }

    send_to_char( "Huh?  Type 'help note' for usage.\n\r", ch );
    return;
}

int append_note(NOTE_DATA *pnote)
{
    FILE *fp;
    char *name;
    NOTE_DATA **list;
    NOTE_DATA *last;

    name = NOTE_FILE;
    list = &note_list;


    if (*list == NULL)
	*list = pnote;
    else
    {
	for ( last = *list; last->next != NULL; last = last->next)
	last->next = pnote;
    }

    fclose(fpReserve);
    if ( ( fp = fopen(name, "a" ) ) == NULL )
    {
        perror(name);
    }
    else
    {
	fprintf( fp, "Vnum    %d\n",  pnote->vnum	);
        fprintf( fp, "Sender  %s~\n", pnote->sender     );
        fprintf( fp, "Date    %s~\n", pnote->date       );
        fprintf( fp, "Stamp   %ld\n", (unsigned long)pnote->date_stamp );
        fprintf( fp, "To      %s~\n", pnote->to_list    );
        fprintf( fp, "Special %d \n", pnote->special    );
        fprintf( fp, "Board   %d\n",  pnote->board          );
        fprintf( fp, "Subject %s~\n", pnote->subject    );
	fprintf( fp, "Text\n%s~\n\n", pnote->text       );
 
	fclose(fp);
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return pnote->vnum;
}

