/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St`1feldt, Tom Madsen, and Katja Nyboe.   *
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

/***************************************************************************
*    ROM 2.4 is copyright 1993-1998 Russ Taylor               *
*    ROM has been brought to you by the ROM consortium           *
*        Russ Taylor (rtaylor@hypercube.org)                   *
*        Gabrielle Taylor (gtaylor@hypercube.org)               *
*        Brian Moore (zump@rom.org)                       *
*    By using this code, you have agreed to follow the terms of the       *
*    ROM license, in the file Rom24/doc/rom.license               *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "tournament.h"
#include "olc.h"
#include "web.h"
#include "mysql.h"

bool is_room_match (CHAR_DATA *ch)
{
	if (IS_SET(ch->in_room->room_flags, ROOM_AIEL) && (ch->clan == clan_lookup("Aiel")))
		return TRUE;
	else if (IS_SET(ch->in_room->room_flags, ROOM_WHITECLOAK) && (ch->clan == clan_lookup("Whitecloak")))
		return TRUE;
	else if (IS_SET(ch->in_room->room_flags, ROOM_SHADOW) && (ch->clan == clan_lookup("Shadow")))
		return TRUE;
	else if (IS_SET(ch->in_room->room_flags, ROOM_SHAIDO) && (ch->clan == clan_lookup("Shaido")))
		return TRUE;
	else if (IS_SET(ch->in_room->room_flags, ROOM_TINKER) && (ch->clan == clan_lookup("Tinker")))
		return TRUE;
	else if (IS_SET(ch->in_room->room_flags, ROOM_ASHAMAN) &&  (ch->clan == clan_lookup("Ashaman")))
		return TRUE;

	return FALSE;
}
	
void do_claim (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;

    argument = one_argument (argument, arg1);
    
    if (arg1[0] == '\0' )
    {
        send_to_char ("Type claim confirm, to claim for your guild.\n\r", ch);
        return;
    }

    location = ch->in_room;
    
   
    if (ch->clan == clan_lookup("Aiel"))
    {
	    if ( IS_SET(ch->in_room->room_flags, ROOM_AIEL))
	    {
	    	send_to_char("Your guild already owns this room.\n\r", ch);
	    	return;
	    }
	    else if (ch->in_room->sector_type == SECT_TAKEN || ch->in_room->sector_type == SECT_TOWER || ch->in_room->sector_type == SECT_GATE)
	    {
	    	send_to_char("This room is taken by another Guild already.\n\r", ch);
	    	return;
        }
	        
        SET_BIT(ch->in_room->room_flags, ROOM_AIEL);
        location->name = "OWNED BY AIEL";
        location->description = "This territory is owned by the Aiel.";
        location->sector_type = 29;
    	send_to_char ("The room is now owned by your guild.\n\r", ch);
        save_area (ch->in_room->area);
        return;
    }
    else if (ch->clan == clan_lookup("Warder") || (ch->clan == clan_lookup("Aessedai")))
    {
	    if ( IS_SET(ch->in_room->room_flags, ROOM_WHITETOWER))
	    {
	    	send_to_char("Your guild already owns this room.\n\r", ch);
	    	return;
	    }
	    else if (ch->in_room->sector_type == SECT_TAKEN)
	    {
	    	send_to_char("This room is taken by another Guild already.\n\r", ch);
	    	return;
        }
	    
        SET_BIT(ch->in_room->room_flags, ROOM_WHITETOWER);
        location->name = "OWNED BY WHITETOWER";
        location->description = "This territory is owned by the Whitetower.";
        location->sector_type = 29;
    	send_to_char ("The room is now owned by your guild.\n\r", ch);
        save_area (ch->in_room->area);
        return;
    }
    else if (ch->clan == clan_lookup("Whitecloak"))
    {
	    if ( IS_SET(ch->in_room->room_flags, ROOM_WHITECLOAK))
	    {
	    	send_to_char("Your guild already owns this room.\n\r", ch);
	    	return;
	    }
	    else if (ch->in_room->sector_type == SECT_TAKEN)
	    {
	    	send_to_char("This room is taken by another Guild already.\n\r", ch);
	    	return;
        }
	    
        SET_BIT(ch->in_room->room_flags, ROOM_WHITECLOAK);
        location->name = "OWNED BY WHITECLOAK";
        location->description = "This territory is owned by the Whitecloaks.";
        location->sector_type = 29;
    	send_to_char ("The room is now owned by your guild.\n\r", ch);
        save_area (ch->in_room->area);
        return;
    }
    else if (ch->clan == clan_lookup("Shaido"))
    {
	    if ( IS_SET(ch->in_room->room_flags, ROOM_SHAIDO))
	    {
	    	send_to_char("Your guild already owns this room.\n\r", ch);
	    	return;
	    }
	    else if (ch->in_room->sector_type == SECT_TAKEN)
	    {
	    	send_to_char("This room is taken by another Guild already.\n\r", ch);
	    	return;
        }
	    
        SET_BIT(ch->in_room->room_flags, ROOM_SHAIDO);
        location->name = "OWNED BY SHAIDO";
        location->description = "This territory is owned by the Shaido.";
        location->sector_type = 29;
    	send_to_char ("The room is now owned by your guild.\n\r", ch);
        save_area (ch->in_room->area);
        return;
    }
    else if (ch->clan == clan_lookup("Shadow"))
    {
	    if ( IS_SET(ch->in_room->room_flags, ROOM_SHADOW))
	    {
	    	send_to_char("Your guild already owns this room.\n\r", ch);
	    	return;
	    }
	    else if (ch->in_room->sector_type == SECT_TAKEN)
	    {
	    	send_to_char("This room is taken by another Guild already.\n\r", ch);
	    	return;
        }
	    
        SET_BIT(ch->in_room->room_flags, ROOM_SHADOW);
        location->name = "OWNED BY SHADOW";
        location->description = "This territory is owned by the Shadow.";
        location->sector_type = 29;
    	send_to_char ("The room is now owned by your guild.\n\r", ch);
        save_area (ch->in_room->area);
        return;
    }
    else if (ch->clan == clan_lookup("Tinker"))
    {
	    if ( IS_SET(ch->in_room->room_flags, ROOM_TINKER))
	    {
	    	send_to_char("Your guild already owns this room.\n\r", ch);
	    	return;
	    }
	    else if (ch->in_room->sector_type == SECT_TAKEN)
	    {
	    	send_to_char("This room is taken by another Guild already.\n\r", ch);
	    	return;
        }
	    
        SET_BIT(ch->in_room->room_flags, ROOM_TINKER);
        location->name = "OWNED BY TINKER";
        location->description = "This territory is owned by the Tinkers.";
        location->sector_type = 29;
    	send_to_char ("The room is now owned by your guild.\n\r", ch);
        save_area (ch->in_room->area);
        return;
    }
    else
    {
    	send_to_char("Your clan cannot build castles yet!", ch);
    	return;
    }
    return;
}


void do_assign (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;

    argument = one_argument (argument, arg1);
    
    location = ch->in_room;
    
    if (arg1[0] == '\0' )
    {
        send_to_char ("Thats not an option.\n\rThe options are: recall, safe, remove.\n\rSyntax: assign <recall, safe, remove>.\n\r", ch);
        return;
    }

    if (!is_room_match(ch))
    {
    	send_to_char("This room is not owned by your guild, you can't assign anything, claim it first!\n\r", ch);
    	return;
    }
    
	if(strcmp(arg1, "recall")
	&& strcmp(arg1, "safe")
	&& strcmp(arg1, "remove"))
	{
		send_to_char("Thats not an option.\n\rThe options are: recall, safe, remove.\n\r\n\rSyntax: assign <recall, safe, remove>.\n\r",ch);
		return;
	}

	if(!strcmp(arg1, "recall"))
	{
		clan_table[ch->clan].vnum = ch->in_room->vnum;
		send_to_char("This room is now your guild's recall point.\n\r", ch);
		return;
	}
	else if(!strcmp(arg1, "safe"))
	{
	    if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	    {
	    	send_to_char("This room is already safe.\n\r", ch);
	    	return;
	    }
	    
	    SET_BIT(ch->in_room->room_flags, ROOM_SAFE);
	    send_to_char ("The room is now safe.\n\r", ch);
	    save_area (ch->in_room->area);
	    return;
	}
	else if(!strcmp(arg1, "remove"))
	{
	    location->sector_type = 28;
	    REMOVE_BIT(ch->in_room->room_flags, ROOM_TINKER);
	    REMOVE_BIT(ch->in_room->room_flags, ROOM_SHAIDO);
	    REMOVE_BIT(ch->in_room->room_flags, ROOM_AIEL);
	    REMOVE_BIT(ch->in_room->room_flags, ROOM_WHITECLOAK);
	    REMOVE_BIT(ch->in_room->room_flags, ROOM_WHITETOWER);
	    REMOVE_BIT(ch->in_room->room_flags, ROOM_SHADOW);
	    REMOVE_BIT(ch->in_room->room_flags, ROOM_SAFE);
	    location->name = "Unowned Territory";
        location->description = "This territoy is waiting to be claimed.";
	    save_area (ch->in_room->area);
		send_to_char("You have disbanded this room for your guild.\n\r", ch);
		return;
	}

    return;
}

void do_construct (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;

    argument = one_argument (argument, arg1);
    
    location = ch->in_room;
    
    if (arg1[0] == '\0' )
    {
        send_to_char("Thats not an option.\n\rThe options are: wall, tower, gate.\n\r\n\rSyntax: construct <wall, tower, gate>.\n\r",ch);
        return;
    }

    if (!is_room_match(ch))
    {
    	send_to_char("This room is not owned by your guild, you can't construct anything, claim it first!\n\r", ch);
    	return;
    }
    
	if(strcmp(arg1, "wall")
	&& strcmp(arg1, "tower")
	&& strcmp(arg1, "gate"))
	{
		send_to_char("Thats not an option.\n\rThe options are: wall, tower, gate.\n\r\n\rSyntax: construct <wall, tower, gate>.\n\r",ch);
		return;
	}

	if(!strcmp(arg1, "wall"))
	{
		send_to_char("You have just constructed a wall.\n\rRemember to construct a gate, or you won't be able to get into the castle!\n\r", ch);
	    location->sector_type = 22;
	    save_area (ch->in_room->area);
	    return;
	}
	else if(!strcmp(arg1, "tower"))
	{
	    send_to_char("You have just constructed a tower.\n\rIf your tower is destroyed, you lose all your guild QPs.", ch);
	    location->sector_type = 24;
	    save_area (ch->in_room->area);
	    return;
	}
	else if(!strcmp(arg1, "gate"))
	{
		send_to_char("You have just constructed a gate.\n\rYou can enter the castle, only through a gate.\n\rDon't construct too many gates, they are easily destroyed.", ch);
	    location->sector_type = 23;
	    save_area (ch->in_room->area);
	    return;
	}

    return;
}

