/* ************************ Maestro Noting System ********************* */      
/* **********************************************************************
----------
Copyright 1997 by Amadeus <Andy McMahan>
amadeus@myself.com

Complete permission given to use this code granted if user retains this header,
and does not change the name of the system.  It must always be called "Maestro",
by Amadeus.

************************************************************************ */ 


/*
 * little tidbits of needed stuff for notes.
 */
#define CAN_READ_BOARD(ch, sn)   ( IS_SET( ( ch )->board_config, (sn) ) )


/*
 * Set this to the minimum level required to post a System Bulletin
 */

#define CAN_POST_BUL	MAX_LEVEL - 1

/*
 * Connected states for writing notes
 */

#define NOTING_NO_ARGUMENT			1
#define NOTING_TO_QUESTION			2
#define NOTING_TO				3
#define NOTING_SUBJECT				4
#define NOTING_BOARD				5
#define NOTING_EDIT				6
#define NOTING_POST				7

#define NOTING_QUESTION_READ_NEXT		8
#define NOTING_READ_PREVIOUS			9
#define NOTING_QUESTION_READ_NEXT_SPECIAL       10
#define NOTING_QUESTION_READ_NEXT_BULLETIN	11
#define NOTING_READ4				12

#define NOTING_BULLETIN_SUBJECT 		13

#define NOTING_CONFIG_NO_ARGUMENT       	14
#define NOTING_CONFIG_QUESTION_CHANGE   	15
#define NOTING_CONFIG_QUESTION_ADD_REMOVE  	16
#define NOTING_CONFIG_WHICH_ADD			17
#define NOTING_CONFIG_WHICH_REMOVE		18
#define NOTING_CONFIG_CHANGE	        	19
#define NOTING_CONFIG_QUESTION_ECHO		20

#define NOTING_QUESTION_DELETE_NOTE    	        21

#define NOTING_HELP_QUESTION			22
#define NOTING_HELP_CONTINUE			23

#define NOTING_LIST_QUESTION			24

#define NOTING_CONTINUE		        	25
#define NOTING_FINAL_QUESTION			26

#define NOTING_LIST_BULLETIN_QUESTION		27
#define NOTING_READ_BULLETIN_PREVIOUS		28
#define NOTING_BULLETIN_EDIT			29

/*
 *  Boards bits
 */ 

#define BOARD_GENERAL			1
#define BOARD_PERSONAL			2
#define BOARD_QUEST			4

#define BOARD_IMMORTAL			8
#define BOARD_BUILDER			16
#define BOARD_CODER			32
#define BOARD_SENIOR			64
#define BOARD_EXEC			128

#define BOARD_CREATIVITY		256

/*
 *  Special note flags  ( pnote->special )
 *	   
 * 
 *   
 */

#define NOTE_SPECIAL_PERSON_TO_PERSON		1
#define NOTE_SPECIAL_IS_BULLETIN		2
#define NOTE_SPECIAL_FLAG3			4
#define NOTE_SPECIAL_FLAG4			8


/*
 *  Special Note System Configuration bits ( ch->note_config )
 *
 *  (Only the first three are currently in use..others are there as 
 *   example templates)
 */

#define NOTE_CONFIG_CLIENT			1
#define NOTE_CONFIG_FIRST_TIME			2
#define NOTE_CONFIG_CURRENTLY_NOTING	        4
#define NOTE_CONFIG_FLAG4			8
#define NOTE_CONFIG_FLAG5			16
#define NOTE_CONFIG_FLAG6			32


/* *************************************************************************
 * Some vt100 compatable escape codes I want to use.  If they don't at 
 * LEAST have a vt compatible by now..there is something wrong.  I really
 * had no choice, this note system won't look as asthetically pleasing 
 * without the clear screens.  At worst, the players with severly ancient
 * terminal emulations will see the little gibberish of escape codes below,
 * but I would estimate VERY little percent of mudders nowadays will have
 * any problems with this.
 *
 *				- Amadeus
 ************************************************************************** */

#define CLR		"\x1B[H\x1B[J"          /* clear screen */
#define FLSH		"\x1b[5m"		/* Flash */
#define BLD		"\x1b[1m"		/* Bold */
#define NTXT		"\x1b[0m"		/* Normal text */
#define HWHITE		"\x1B[1;37m"		/* High White */
