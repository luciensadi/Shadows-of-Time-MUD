/***************************************************************************
*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@efn.org)				   *
*	    Gabrielle Taylor						   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
*	Room rental code by Kimber Boh					   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "merc.h"
#include "recycle.h"




void do_rental( CHAR_DATA *ch )
{
   

    if ( IS_SET(ch->in_room->room_flags, ROOM_RENTAL) )
    {

     if ( IS_NPC(ch))
       return;
    
     if ( IS_SET(ch->act, PLR_TENNANT))
      {
       send_to_char("You haven't checked out from last time, yet!\n\r", ch);
       return;
      }
    
     send_to_char("You rent the room.\n\r", ch);
     ch->pcdata->rentroom = ch->in_room->vnum;
     REMOVE_BIT(ch->in_room->room_flags, ROOM_RENTAL);
     SET_BIT(ch->in_room->room_flags, ROOM_PRIVATE);
     SET_BIT(ch->in_room->room_flags, ROOM_RENTED);
     SET_BIT(ch->act, PLR_TENNANT);
     ch->gold -= 10;
     return;
    }
    else
    {
     send_to_char("You can't rent this room.\n\r", ch);
     return;
    }
}

void do_checkout( CHAR_DATA *ch )
{
    
    
    if ( IS_SET(ch->in_room->room_flags, ROOM_RENTED) )
    {
	
     if ( IS_NPC(ch))
       return;
    
     if ( IS_SET(ch->act, PLR_TENNANT) && ch->pcdata->rentroom == ch->in_room->vnum)
      {
       send_to_char("You check out of the room.\n\r", ch);
       REMOVE_BIT(ch->act, PLR_TENNANT);
       REMOVE_BIT(ch->in_room->room_flags, ROOM_RENTED);
       SET_BIT(ch->in_room->room_flags, ROOM_RENTAL);
       ch->pcdata->rentroom = 0;
       return;
      }
     else
     {
       send_to_char("This is not your room.\n\r", ch);
       return;
     }
    }
}