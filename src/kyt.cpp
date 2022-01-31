/* BY KYTENYA 10/24/00 -- rnbweyes@hotmail.com */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "merc.h"
#include "kyt.h"
#include "tables.h"
#include "lookup.h"

int find_direction	args( ( char *argument ) );
bool check_blind args ((CHAR_DATA * ch));

//MARK_DATA * mark_first;
WAR_DATA * war_first;

/*bool check_player( char * name )
{
    char buf[100];

    sprintf( buf, "%s%s", PLAYER_DIR, capitalize( name ) );
    if ( access( buf, F_OK ) == 0 ) 
        return TRUE;

    return FALSE;
}*/

/*void save_marks( void )
{
    MARK_DATA * p;
    FILE * fp;

    if ( !( fp = fopen( "mark.txt", "w+" ) ) )
	return;

    for ( p = mark_first; p; p = p->next )
    {
	fprintf( fp, "Cont %s~\n", p->contractor 	);
	fprintf( fp, "Targ %s~\n", p->target 		);
	fprintf( fp, "Stat %s~\n", p->status 		);
	fprintf( fp, "Pric %s~\n", p->price             );
	fprintf( fp, "Extr %s~\n", p->extra             );
	fprintf( fp, "Info %s~\n", p->info              );
	  }

    fprintf( fp, "End\n" );
    fclose( fp );
    return;
}

void load_marks( void )
{
    MARK_DATA * p = NULL;
    FILE * fp;
    char * word;
    bool fMatch;

    fMatch = FALSE;
    if ( ( fp = fopen( "mark.txt", "r" ) ) == NULL )
	return;

    for ( ; ; )
    {
	word   = fread_word( fp );
	fMatch = FALSE;

	if ( !str_cmp( word,  "End" ) )
	    break;

	switch ( UPPER( word[0] ) )
	{
	    case '*':
	        fMatch = TRUE;
	        fread_to_eol( fp );
	        break;

	    case 'C':
                if ( !str_cmp( word, "Cont" ) )
	    	{
            	    p 			= alloc_mem( sizeof( MARK_DATA ) );
		    p->contractor 	= fread_string( fp );
		    p->target 		= str_dup( "" );
		    p->status           = str_dup( "" );
		    p->price            = str_dup( "" );
		    p->extra            = str_dup( "" );
		    p->info             = str_dup( "" );
		    p->next 		= mark_first;
            	    mark_first 		= p; 
	      	    fMatch 		= TRUE;
	      	    break;
	    	}
		break;

            case 'E':
                if ( !str_cmp( word, "Extr" ) )
	    	{
		    p->extra 		= fread_string( fp );
	      	    fMatch 		= TRUE;
	      	    break;
	    	}
		break;
            case 'I':
                if ( !str_cmp( word, "Info" ) )
	    	{
		    p->info 		= fread_string( fp );
	      	    fMatch 		= TRUE;
	      	    break;
	    	}
		break;
	    case 'P':
                if ( !str_cmp( word, "Pric" ) )
	    	{
		    p->price 		= fread_string( fp );
	      	    fMatch 		= TRUE;
	      	    break;
	    	}
		break;
                case 'S':
                if ( !str_cmp( word, "Stat" ) )
	    	{
		    p->status 		= fread_string( fp );
	      	    fMatch 		= TRUE;
	      	    break;
	    	}
		break;
	    case 'T':
                if ( !str_cmp( word, "Targ" ) )
	    	{
		    p->target 		= fread_string( fp );
	      	    fMatch 		= TRUE;
	      	    break;
	    	}

                
		break;

    	}

    	if ( !fMatch )
    	{
	    bug( "Load_marks: no match - ", 0 );
	    fread_to_eol( fp );
    	}
    }

    fclose( fp );
    return;
}

void do_mark( CHAR_DATA * ch, char * argument )
{
    MARK_DATA * p, * q;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];

    if ( IS_NPC( ch ) )
	return;    

    if ( !IS_CLAN( ch, "furies" ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );	
    strcpy( arg3, argument );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r"
                      "   mark show\n\r"
                      "   mark contractor <name>\n\r"
                      "   mark target <contractor> <name>\n\r"
                      "   mark status <contractor> <status>\n\r"
                      "   mark price  <contractor> <amount>\n\r"
                      "   mark extra  <contractor> <extra>\n\r"
                      "   mark info   <contractor> <info>\n\r"                  
		      "   mark clear <contractor>\n\r", ch );
	return;
    }

    if ( !str_prefix( arg1, "show" ) )
    {
        if ( !mark_first )
	{
	    send_to_char( "There are none marked.\n\r", ch );
	    return;
	}

	send_to_char( "Contractor    Target       Status       Price       Extra       Info\n\r", ch );
        for ( p = mark_first; p; p = p->next)
	{
	    sprintf( buf, " %-12s %-12s %-12s %-12s %-12s %-12s\n\r",
		p->contractor[0] != '\0' ? p->contractor : "No one",
		p->target[0]     != '\0' ? p->target     : "No one",
		p->status[0]     != '\0' ? p->status     : "Unknown",
		p->price[0]      != '\0' ? p->price      : "No Amount",
		p->extra[0]      != '\0' ? p->extra      : "Nothing",
		p->info[0]       != '\0' ? p->info       : "No info");
		send_to_char( buf, ch );
	}
	return;
    }

    if ( arg2[0] == '\0' )
    {
	do_mark( ch, "" );
	return;
    }

    if ( !str_prefix( arg1, "contractor" ) )
    {
    	for ( p = mark_first; p ; p = p->next )
	    if ( !str_cmp( arg2, p->contractor ) )
	        break;
			
    	if ( p )
    	{
	    send_to_char( "You already have a contract under that name.\n\r", ch );
	    return;
	}
	else
	{
	    if ( !check_player( arg2 ) )
	    {
		send_to_char( "The name of the contractor does not exists.\n\r", ch );
		return;
 	    }

            p 		  = alloc_mem( sizeof( MARK_DATA ) );
	    p->contractor = str_dup( arg2 );
	    p->target     = str_dup( "" );
	    p->status           = str_dup( "" );
            p->price            = str_dup( "" );
            p->extra            = str_dup( "" );
            p->info             = str_dup( "" );
	    
            p->next 	  = mark_first; 
            mark_first 	  = p;

	    sprintf( buf, "You mark %s as the contractor.\n\r", capitalize( arg2 ) );
 	    send_to_char( buf, ch );
	    save_marks( );
	}
	return;
    }

    if ( !str_prefix( arg1, "clear" ) )
    {
    	for ( p = mark_first; p ; p = p->next )
	    if ( !str_cmp( arg2, p->contractor ) )
	        break;
			
    	if ( p )
    	{
            if ( mark_first == p )
            	mark_first = p->next;
	    else
	    {
                for (q = mark_first; q->next != p; q = q->next );
		    q->next = p->next;
	    }
		
            free_string( p->contractor );
            free_string( p->target );
            free_mem( p, sizeof( MARK_DATA ) );
	    save_marks( );

	    sprintf( buf, "You clear %s's contract.\n\r", capitalize( arg2 ) );
	    send_to_char( buf, ch );
	    return;
	}
	else
	{
	    send_to_char( "There is no contractor under that name to clear.\n\r", ch );
	}
	return;
    }

    if ( arg3[0] == '\0' )
    {
	do_mark( ch, "" );
	return;
    }

    if ( !str_prefix( arg1, "target" ) )
    {
    	for ( p = mark_first; p ; p = p->next )
	    if ( !str_cmp( arg2, p->contractor ) )
	        break;
			
    	if ( p )
    	{
	    if ( !check_player( arg3 ) )
	    {
	        send_to_char( "The target does not exists.\n\r", ch );
		return;
 	    }

	    free_string( p->target );
	    p->target     = str_dup( arg3 );
	    save_marks( );

	    sprintf( buf, "You mark %s for extermination!\n\r", capitalize( arg3 ) );
 	    send_to_char( buf, ch );
	}
	else
	{
	    send_to_char( "Please enter the contractor first.\n\r", ch );
	}
	return;
    }

    if ( !str_prefix( arg1, "status" ) )
    {
    	for ( p = mark_first; p ; p = p->next )
	    if ( !str_cmp( arg2, p->contractor ) )
	        break;
			
    	if ( p )
    	{
	    free_string( p->status );
	    p->status     = str_dup( arg3 );	    
	    save_marks( );

	    sprintf( buf, "The contract status has been set.\n\r");
 	    send_to_char( buf, ch );
	}
	else
	{
	    send_to_char( "Please enter the contractor first.\n\r", ch );
	}
	return;
    }
       if ( !str_prefix( arg1, "price" ) )
    {
    	for ( p = mark_first; p ; p = p->next )
	    if ( !str_cmp( arg2, p->contractor ) )
	        break;
			
    	if ( p )
    	{
	    free_string( p->price );
	    p->price    = str_dup( arg3 );	    
	    save_marks( );

	    sprintf( buf, "The contract price has been set.\n\r" );
 	    send_to_char( buf, ch );
	}
	else
	{
	    send_to_char( "Please enter the contractor first.\n\r", ch );
	}
	return;
    }
       if ( !str_prefix( arg1, "extra" ) )
    {
    	for ( p = mark_first; p ; p = p->next )
	    if ( !str_cmp( arg2, p->contractor ) )
	        break;
			
    	if ( p )
    	{
	    free_string( p->extra );
	    p->extra     = str_dup( arg3 );	    
	    save_marks( );

	    sprintf( buf, "The contract extra has been set.\n\r");
 	    send_to_char( buf, ch );
	}
	else
	{
	    send_to_char( "Please enter the contractor first.\n\r", ch );
	}
	return;
    }
       if ( !str_prefix( arg1, "info" ) )
    {
    	for ( p = mark_first; p ; p = p->next )
	    if ( !str_cmp( arg2, p->contractor ) )
	        break;
			
    	if ( p )
    	{
	    free_string( p->info );
	    p->info     = str_dup( arg3 );	    
	    save_marks( );

	    sprintf( buf, "The contract info has been set.\n\r");
 	    send_to_char( buf, ch );
	}
	else
	{
	    send_to_char( "Please enter the contractor first.\n\r", ch );
	}
	return;
    }

    do_mark( ch, "" );
    return;
}*/

/* scan code */
/* for scanning and do_scanning */
char * const dist_name [] =
{
  "right here",
  "close by", "not far off", "a brief walk away",
  "rather far off", "in the distance", "almost out of sight"
};

char * const dir_desc [] =
{
  "to the north", "to the east", "to the south", "to the west",
  "upwards", "downwards"
};

/*
 * Scan command.
 * (by Turtle 12-Jun-94)
 */
void scanning( CHAR_DATA *ch, int door )
{
    int distance, visibility;
    char buf[MAX_STRING_LENGTH];
    bool found;
    ROOM_INDEX_DATA *was_in_room;

    visibility = 6;
    if( IS_SET( ch->act, PLR_HOLYLIGHT ) )
    {
      	switch( weather_info.sunlight )
	{
	    case SUN_SET:   visibility = 4; break;
	    case SUN_DARK:  visibility = 2; break;
	    case SUN_RISE:  visibility = 4; break;
	    case SUN_LIGHT: visibility = 6; break;
	}
        switch( weather_info.sky )
	{
	    case SKY_CLOUDLESS: break;
	    case SKY_CLOUDY:    visibility -= 1; break;
	    case SKY_RAINING:   visibility -= 2; break;
	    case SKY_LIGHTNING: visibility -= 3; break;
	}
    }

    if ( IS_IMMORTAL( ch ) )
     	visibility = 6;

    was_in_room = ch->in_room;
    found = FALSE;
    for( distance = 1; distance <= 6; distance++ )
    {
      	EXIT_DATA *pexit;
      	CHAR_DATA *list;
      	CHAR_DATA *rch;

	if( ( pexit = ch->in_room->exit[door] ) != NULL
	&& pexit->u1.to_room != NULL
	&& pexit->u1.to_room != was_in_room )
	{
       	    if ( !can_see_room( ch, pexit->u1.to_room ) )
		continue;

	    /* If the door is closed, stop looking... */
	    if( IS_SET( pexit->exit_info, EX_CLOSED ) )
	    {
	      	char door_name[80];

	      	one_argument( pexit->keyword, door_name );
	      	if ( door_name[0] == '\0' )
		    strcat( door_name, "door" );
	        sprintf( buf, "A closed %s %s %s.\n\r",
		    door_name, dist_name[distance-1], dir_desc[door] );
	      	send_to_char( buf, ch );
	      	found = TRUE;
	        break;
	    }

	    ch->in_room = pexit->u1.to_room;
	    if( IS_OUTSIDE(ch) ? distance > visibility : distance > 4 )
		break;

	    list = ch->in_room->people;
	    for( rch = list; rch != NULL; rch = rch->next_in_room )
	    	if( can_see( ch, rch ) )
	      	{
		    found = TRUE;
		    sprintf( buf, "%s%s is %s %s.\n\r",
			PERS( rch, ch, FALSE ),
			IS_NPC(rch) ? "" : IS_DISGUISED(rch)?"":" (PLAYER)",
			dist_name[distance],
			dir_desc[door] );
		    send_to_char( buf, ch );
	      	}
	}
    }

    ch->in_room = was_in_room;
    if ( !found )  	
    {
	sprintf( buf, "You can't see anything %s.\n\r",
	    dir_desc[door] );
      	send_to_char( buf, ch );
    }
    return;
}


void do_scanning( CHAR_DATA *ch, char *argument )
{
    int dir=0;
    bool found;
    EXIT_DATA *pexit;
    char buf[MAX_STRING_LENGTH];

    if( !check_blind( ch ) )
    	return;

    if ( argument[0] == '\0' )
    {
      	act( "$n scans intensely all around.", ch, NULL, NULL, TO_ROOM );
      	found = FALSE;
	pexit = NULL;
      	for( dir = 0; dir <= 5; dir++ )
	{
    	    if ( ( pexit   = ch->in_room->exit[dir] ) == NULL
    	    ||	 !can_see_room(ch,pexit->u1.to_room ) )
	 	continue;

	   sprintf( buf, "\n\r**** %s ****\n\r",dir_desc[dir]);
 	   send_to_char( buf, ch);
	   scanning( ch, dir );
	   found = TRUE;
	}
      	    
  	if ( !found )
	    send_to_char( "There are no exits here!\n\r", ch );
    }
    else
    {
    	if ( ( dir = find_direction( argument ) ) == -1 )
    	{
	    send_to_char( "That's not a direction!\n\r", ch );
	    return;
    	}

	act( "$n scans intensly $T.", ch, NULL, dir_desc[dir], TO_ROOM );
    	if ( ( pexit   = ch->in_room->exit[dir] ) == NULL
    	||     !can_see_room(ch,pexit->u1.to_room ) )
	{
	    send_to_char( "There is no exit in that direction!\n\r", ch );
	    return;
	}

	sprintf(buf, "\n\r**** %s ****\n\r",dir_desc[dir]);
	send_to_char(buf, ch);
	scanning( ch, dir );
    }
    return;
}


/*
 * War - Kytenya 10/27/00
 */


void war_update( void )
{
    WAR_DATA * p;
    char buf[MAX_STRING_LENGTH];

    if ( !war_first )
	return;

    for ( p = war_first; p ; p = p->next )
    {
	if ( p->tick == 0 )
	    continue;

	p->tick--;
	if ( p->tick == 0 )
	{
	    sprintf( buf, "%senters the war.", clan_table[ p->clan ].who_name );
	    echoall( buf );
	}
/*
	else
	{
	    sprintf( buf, "War in %d tick%s.", p->tick, p->tick > 1 ? "s" : "" );
	    echoall( buf );
	}
*/
    }
}

bool at_war( int clan )
{
    WAR_DATA * p;

    for ( p = war_first; p ; p = p->next )
    {
	if ( p->clan == clan )
	 return TRUE;
    }			
    return FALSE;
}
/*
void do_war( CHAR_DATA * ch, char * argument )
{
    WAR_DATA * p, * q;
    char buf[MAX_STRING_LENGTH];
    int i;

    if ( argument[ 0 ] == '\0' )
    {
	send_to_char( "Syntax:\n\r"
		      "   war <guild>\n\r"
        	      "   war info\n\r", ch );
	return;
    }

    if ( !str_cmp( argument, "info" ) )
    {
        if ( !war_first )
	{
	    send_to_char( "It seems peaceful...\n\r", ch );
	    return;
	}

	send_to_char( "Guild        Enemy       Win   Lose  Status\n\r"
		      "-----------  ----------  ----  ----  -------\n\r", ch );

        for ( p = war_first; p; p = p->next)
	{

	    sprintf( buf, "%-11s  %-11s %4d  %4d  %-7s\n\r",
		clan_table[ p->clan ].display,
		clan_table[ p->enemy ].display,
		p->win,
		p->lose,
		p->tick == 0 ? "At War" : "Pending" );
	    send_to_char( buf, ch );
	}
	return;
    }

    if ( !str_cmp( argument, "surrender" ) )
    {
        if ( !war_first )
	{
	    send_to_char( "It seems peaceful...\n\r", ch );
	    return;
	}

    	for ( p = war_first; p ; p = p->next )
	    if ( p->clan == ch->clan )
	        break;
			
    	if ( p )
    	{
            if ( war_first == p )
            	war_first = p->next;
	    else
	    {
                for ( q = war_first; q->next != p; q = q->next );
		    q->next = p->next;
	    }
		
            free_mem( p, sizeof( WAR_DATA ) );
	    sprintf( buf, "%s waves the white flag!", clan_table[ ch->clan ].display );
	    echoall( buf );
	    return;
	}
	else
	    send_to_char( "You are not at war.\n\r", ch );
	return;
    }

    if ( !str_cmp( argument, "end" ) )
    {
        if ( !war_first )
	{
	    send_to_char( "It seems peaceful...\n\r", ch );
	    return;
	}

    	for ( p = war_first; p ; p = p->next )
	{			
    	    if ( p )
    	    {
            	if ( war_first == p )
            	    war_first = p->next;
	    	else
	    	{
                    for ( q = war_first; q->next != p; q = q->next );
		    	q->next = p->next;
	    	}
	    }

            free_mem( p, sizeof( WAR_DATA ) );
	}

    	sprintf( buf, "War ended by %s!", ch->name );
	echoall( buf );
	return;
    }

    if ( IS_CLAN( ch, "tinker" ) )
    {
	send_to_char( "You are a Tinker.  You follow the Ways of the Leaf.\n\r", ch );
	return;
    }

    if ( ( i = clan_lookup( argument ) ) == -1 )
    {
	send_to_char( "That guild does not exists.\n\r", ch );
        return;
    }

    if ( i == ch->clan )
    {
	send_to_char( "Not on your own guild.\n\r", ch );
	return;
    }

    if ( i == clan_lookup( "tinker" ) )
    {
	send_to_char( "Tinkers are not a target!\n\r", ch );
	return;
    }

    
    if ( at_war( ch->clan ) )
    {
	send_to_char( "You are a little busy defending yourself against another guild.\n\r", ch );
	return;
    }
			
    p 		= alloc_mem( sizeof( WAR_DATA ) );
    p->clan	= ch->clan;
    p->enemy	= i;
    p->tick     = 3;
    p->win	= 0;
    p->lose	= 0;
    p->next 	= war_first; 
    war_first	= p;

    if ( !at_war( i ) )
    {
    	p 		= alloc_mem( sizeof( WAR_DATA ) );
    	p->clan		= i;
    	p->enemy	= ch->clan;
    	p->tick     	= 3;
    	p->win		= 0;
    	p->lose		= 0;
    	p->next 	= war_first; 
    	war_first	= p;
    }

    sprintf( buf, "%s declares war on %s!\n\r"
                  "The battle will start in %d ticks!",
	clan_table[ ch->clan ].display,
	clan_table[ i ].display,
	p->tick );
    echoall( buf );
    return;
}
*/
