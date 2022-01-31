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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"
#include "lookup.h"

void bomb_entry args ((CHAR_DATA *ch));
void show_room_blind( CHAR_DATA *ch );

char *const dir_name[] = 
{
    "`!N`1orth`*", "`@E`2ast`*", "`#S`3outh`*", "`$W`4est`*", "`%U`5p`*", "`&D`7own`*"
};

char *const rev_move[] =
{
    "the south", "the west", "the north", "the east", "below", "above"
};
char *const u_dir_name[] =
{
    "north","east","south","west","up","down"
};

const sh_int rev_dir[] = {
    2, 3, 0, 1, 5, 4
};

const sh_int movement_loss[SECT_MAX] = {
    2, 2, 2, 3, 4, 2, 4, 2, 2, 4, 2, 4, 4, 1, 1, 3, 3, 2
};

int find_direction( char *argument )
{
	 if ( !str_cmp( argument, "n" ) || !str_cmp( argument, "north" ) ) return DIR_NORTH;
    else if ( !str_cmp( argument, "e" ) || !str_cmp( argument, "east"  ) ) return DIR_EAST;
    else if ( !str_cmp( argument, "s" ) || !str_cmp( argument, "south" ) ) return DIR_SOUTH;
    else if ( !str_cmp( argument, "w" ) || !str_cmp( argument, "west"  ) ) return DIR_WEST;
    else if ( !str_cmp( argument, "u" ) || !str_cmp( argument, "up"    ) ) return DIR_UP;
    else if ( !str_cmp( argument, "d" ) || !str_cmp( argument, "down"  ) ) return DIR_DOWN;
    return -1;
}


/*
 * Local functions.
 */
int find_door args ((CHAR_DATA * ch, char *arg));
bool has_key args ((CHAR_DATA * ch, long key));
OBJ_DATA *bomb_in_bag args ((OBJ_DATA *obj));


void move_char (CHAR_DATA * ch, int door, bool follow, bool ignoreCharm)
{
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    char buf [MSL];
    bool galloping = FALSE;

    if (door < 0 || door > 5)
    {
        bug ("Do_move: bad door %d.", door);
        return;
    }

    /*
     * Exit trigger, if activated, bail out. Only PCs are triggered.
     */
    if (!IS_NPC (ch) && 
        (p_exit_trigger (ch, door, PRG_MPROG)
        || p_exit_trigger (ch, door, PRG_OPROG)
        || p_exit_trigger (ch, door, PRG_RPROG)))
        return;

    if(IS_AFFECTED(ch, AFF_SEVERED))
    {
        send_to_char("Move without legs? How?\n\r",ch);
        return;
    }
    if (ch->position == POS_MEDITATING)
    {
    	send_to_char ("Better stand up first.\n\r", ch);
        return;
    }

    if ( is_affected( ch, gsn_trapset ) )
    {
        send_to_char( "You seemed to be trapped in this room.\n\r", ch );
        return;
    }

    if (IS_AFFECTED(ch, AFF_STICKY) && number_percent() < 50)
    {
      act("You are stuck to the ground where you stand.", ch, NULL, NULL, TO_CHAR);
      act("$n tries to leave, but is stuck to the ground.", ch, NULL, NULL, TO_ROOM);
      return;
    }

    if (IS_AFFECTED(ch, AFF_FEAR)) 
    {
      act("You are too frightened to move.", ch, NULL, NULL, TO_CHAR);
      act("$n tries to leave, but is too frightened to move.", ch, NULL, NULL, TO_ROOM);
      return;
    }

    if (IS_MOUNTED(ch) && ch->mount && IS_AFFECTED(ch->mount, AFF_FEAR)) 
    {
      act("Your $N is too afraid to move any further.", ch, NULL, ch->mount, TO_CHAR);
      act("$N tries to leave, but is too frightened to move.", ch, NULL, ch->mount, TO_ROOM);
      return;
    }

    bleed_char(ch,FALSE);
    in_room = ch->in_room;
    if ((pexit = in_room->exit[door]) == NULL
        || (to_room = pexit->u1.to_room) == NULL
        || !can_see_room (ch, pexit->u1.to_room))
    {
        send_to_char ("Alas, you cannot go that way.\n\r", ch);
        return;
    }

    if ((IS_MOUNTED(ch) && IS_AFFECTED(ch->mount, AFF_GALLOPING)) ||
         (IS_NPC(ch) && IS_MOUNTED(ch) && IS_AFFECTED(ch, AFF_GALLOPING)))
    {
      galloping = TRUE; 
      if ((pexit = to_room->exit[door]) == NULL
        || (to_room = pexit->u1.to_room) == NULL
        || !can_see_room (ch, pexit->u1.to_room))
      {
        send_to_char ("Alas, you cannot gallop that way.\n\r", ch);
        return;
      }
    }

       if(!str_cmp(ch->in_room->area->name, "Guildwar"))
       {
	      galloping = TRUE;
	
	      if ((pexit = to_room->exit[door]) == NULL
       	 	|| (to_room = pexit->u1.to_room) == NULL
       	 	|| (!can_see_room (ch, pexit->u1.to_room)
		|| !room_is_wall (ch, pexit->u1.to_room)
		|| !room_is_gate (ch, pexit->u1.to_room)
                || !room_is_tower (ch, pexit->u1.to_room)))
      		{
        		send_to_char ("Alas, you cannot move that way.\n\r", ch);
        		return;
      		}
       }

    if (IS_NPC(ch))
    {
    	if (IS_SET(to_room->room_flags, ROOM_NO_MOB) && !IS_SET(ch->act, ACT_MOUNT))
    	{
            sprintf (buf, "The %s cowers in fear of entering that room!",ch->short_descr);
            act(buf,ch,NULL,NULL,TO_ROOM);
            act("You cower in fear of entering that room!",ch,NULL,NULL,TO_CHAR);
            return;
        }        
    }


    if (IS_SET (pexit->exit_info, EX_CLOSED)
        && (!IS_AFFECTED (ch, AFF_PASS_DOOR)
            || IS_SET (pexit->exit_info, EX_NOPASS))
        && !IS_TRUSTED (ch, BUILDER))
    {
        act ("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
        return;
    }

    if (!ignoreCharm && IS_AFFECTED (ch, AFF_CHARM)
        && ch->master != NULL && in_room == ch->master->in_room)
    {
        send_to_char ("What?  And leave your beloved master?\n\r", ch);
        return;
    }

    if (!is_room_owner (ch, to_room) && room_is_private (to_room))
    {
        send_to_char ("That room is private right now.\n\r", ch);
        return;
    }

/*		//Room rental code... needs violence timer check
     if ( ch->pcdata->rentroom != to_room->vnum  && IS_SET(to_room->room_flags, ROOM_RENTED))
       {
        if ( !IS_IMMORTAL(ch))
        {
         send_to_char( "You can't go in there.\n\r", ch );
         return;
        }
       }
*/
    if (!IS_NPC (ch))
    {
        //int iClass, iGuild;
        int move;
/*
        for (iClass = 0; iClass < MAX_CLASS; iClass++)
        {
            for (iGuild = 0; iGuild < MAX_GUILD; iGuild++)
            {
                if (iClass != ch->cClass
                    && to_room->vnum == class_table[iClass].guild[iGuild])
                {
                    send_to_char ("You aren't allowed in there.\n\r", ch);
                    return;
                }
            }
        }
*/
        if (in_room->sector_type == SECT_AIR
            || to_room->sector_type == SECT_AIR)
        {
            if (!IS_AFFECTED (ch, AFF_FLYING) && !IS_IMMORTAL (ch))
            {
                send_to_char ("You can't fly.\n\r", ch);
                return;
            }
        }

        if ((in_room->sector_type == SECT_WATER_NOSWIM
             || to_room->sector_type == SECT_WATER_NOSWIM)
            && !IS_AFFECTED (ch, AFF_FLYING))
        {
            OBJ_DATA *obj;
            bool found;

            /*
             * Look for a boat.
             */
            found = FALSE;

            if (IS_IMMORTAL (ch))
                found = TRUE;

            for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
            {
                if (obj->item_type == ITEM_BOAT)
                {
                    found = TRUE;
                    break;
                }
            }
            if (!found)
            {
                send_to_char ("You need a boat to go there.\n\r", ch);
                return;
            }
        }

        move = movement_loss[UMIN (SECT_MAX - 1, in_room->sector_type)]
             + movement_loss[UMIN (SECT_MAX - 1, to_room->sector_type)];

        if (!str_cmp(ch->in_room->area->name, "World Map"))
            move *= number_range(5,8);
        else if (!str_cmp(ch->in_room->area->name, "World Map") && ch->pcdata->pk_timer > 0)
	    move *= 10;

        move /= 2;                /* i.e. the average */


        /* conditional effects */
        if (IS_AFFECTED (ch, AFF_FLYING) || IS_AFFECTED (ch, AFF_HASTE))
            move /= 2;

        if (IS_AFFECTED (ch, AFF_SLOW))
            move *= 2;

        if ((IS_MOUNTED(ch) && IS_AFFECTED(ch->mount, AFF_GALLOPING))
           || (IS_NPC(ch) && IS_MOUNTED(ch) && IS_AFFECTED(ch, AFF_GALLOPING)))
           move = int(move*2.5);


          if( IS_AFFECTED(ch, AFF_LIMP))
                WAIT_STATE(ch, 6);
          else
                WAIT_STATE (ch, 1);
        ch->move -= move;

        if (ch->move < move)
        {
            send_to_char ("You are too exhausted.\n\r", ch);
            return;
        }

    }

    if (ch->is_mounted)
    {
      if (!IS_NPC(ch) && ch->mount)
      { 
        act ("$n leaves the room riding $N to the $t.", ch, dir_name[door], ch->mount, TO_ROOM);
      }
      else if (!IS_NPC(ch))
      {
        act ("$n leaves the room riding a mount to the $t.", ch, dir_name[door], NULL, TO_ROOM);
      }
    }
/*
    else if (!IS_AFFECTED (ch, AFF_SNEAK) 
      && !IS_AFFECTED(ch, AFF_STEALTH) 
      && ch->invis_level < LEVEL_HERO)
          act ("$n leaves $t.", ch, dir_name[door], NULL, TO_ROOM);
*/
    else if ((ch->invis_level < LEVEL_HERO))
    {
      for (fch = ch->in_room->people; fch != NULL; fch = fch_next)
      { 
        fch_next = fch->next_in_room;
        if (fch == ch)
          continue;
        if ((IS_AFFECTED(ch, AFF_SNEAK) || IS_AFFECTED(ch, AFF_SHROUD)) && IS_AFFECTED(fch, AFF_AWARENESS) && number_percent() < 35)
            act ("$n leaves $t.", ch, dir_name[door], fch, TO_VICT);
        else if (!IS_AFFECTED(ch, AFF_SNEAK) && !IS_AFFECTED(ch, AFF_SHROUD))
            act ("$n leaves $t.", ch, dir_name[door], fch, TO_VICT);
      } 
    }
    else
      act ("$n leaves $t", ch, dir_name[door], NULL, TO_ROOM);    
    
    char_from_room (ch);
    char_to_room (ch, to_room);


    if (!IS_NPC(ch) && IS_AFFECTED (ch, AFF_VEIL) && ch->pcdata->pk_timer == 0 && IS_SET (ch->in_room->room_flags, ROOM_SAFE))
        affect_strip (ch, gsn_veil);
    if (!IS_NPC(ch) && IS_AFFECTED (ch, AFF_SHROUD) && ch->pcdata->pk_timer == 0 && IS_SET (ch->in_room->room_flags, ROOM_SAFE))
        affect_strip (ch, gsn_shroud);

    if (ch->is_mounted)
    {
      if (!IS_NPC(ch) && ch->mount)
      { 
        act ("$n comes riding into the room on $N from $t.", ch, rev_move[door], ch->mount, TO_ROOM);
      }
      else if (!IS_NPC(ch))
      {
        act ("$n comes into the room riding a mount from $t.", ch, rev_move[door], NULL, TO_ROOM);
      }
    }
    else if (!IS_AFFECTED (ch, AFF_SNEAK) 
      && !IS_AFFECTED(ch, AFF_STEALTH) 
      && ch->invis_level < LEVEL_HERO)
          act ("$n has arrived from $t.", ch, rev_move[door], NULL, TO_ROOM);
    else if ((ch->invis_level < LEVEL_HERO))
    {
      for (fch = ch->in_room->people; fch != NULL; fch = fch_next)
      { 
        fch_next = fch->next_in_room;
        if (fch == ch)
          continue;
        if ((IS_AFFECTED(ch, AFF_SNEAK) || IS_AFFECTED(ch, AFF_SHROUD)) && IS_AFFECTED(fch, AFF_AWARENESS) && number_percent() < 35)
            act ("$n has arrived from $t.", ch, rev_move[door], fch, TO_VICT);
      } 
    }
    else
      act ("$n has arrived from $t", ch, rev_move[door], NULL, TO_ROOM);       

    do_function (ch, &do_look, "auto");
    if( !IS_SET(ch->act, PLR_HOLYLIGHT)
		&& !IS_NPC(ch)
                && (IS_AFFECTED(ch, AFF_BLIND) || room_is_dark(ch->in_room))
                && number_percent() < get_skill(ch,gsn_blind_sight))
        {
                show_room_blind(ch);
                if(get_skill(ch,gsn_blind_sight) > 1)
                { check_improve(ch, gsn_blind_sight, TRUE, 1); }
                return;
        }

    if (in_room == to_room)
    {        /* no circular follows */
        return;
    }

    for (fch = in_room->people; fch != NULL; fch = fch_next)
    {
        fch_next = fch->next_in_room;

        if (fch->master == ch && IS_AFFECTED (fch, AFF_CHARM)
            && fch->position < POS_STANDING)
            do_function (fch, &do_stand, "");

        if (fch->mount == ch
            && fch->position < POS_STANDING)
            do_function (fch, &do_stand, "");

        if (fch->mount == ch && fch->position == POS_STANDING)
        {
          move_char(fch, door, TRUE, FALSE);
        }  

        if (fch->master == ch && fch->position == POS_STANDING
            && can_see_room (fch, to_room))
        {

            if (IS_SET (ch->in_room->room_flags, ROOM_LAW)
                && (IS_NPC (fch) && IS_SET (fch->act, ACT_AGGRESSIVE)) &&
                !IS_SET(fch->act, ACT_MOUNT))
            {
                act ("You can't bring $N into the city.",
                     ch, NULL, fch, TO_CHAR);
                act ("You aren't allowed in the city.",
                     fch, NULL, NULL, TO_CHAR);
                continue;
            }

            act ("You follow $N.", fch, NULL, ch, TO_CHAR);
            move_char (fch, door, TRUE, FALSE);
        }
    }

    if (ch->in_room->entry_bomb)
    {
      ROOM_INDEX_DATA *bomb_room;
      bomb_room = ch->in_room;
      bomb_entry(ch);
      bomb_room->entry_bomb = FALSE;
    }

    /* 
     * If someone is following the char, these triggers get activated
     * for the followers before the char, but it's safer this way...
     */
    if (IS_NPC (ch) && HAS_TRIGGER_MOB (ch, TRIG_ENTRY))
        p_percent_trigger (ch, NULL, NULL, NULL, NULL, NULL, TRIG_ENTRY);
    if (!IS_NPC (ch))
    {
        p_greet_trigger (ch, PRG_MPROG);
        p_greet_trigger (ch, PRG_OPROG);
        p_greet_trigger (ch, PRG_RPROG);
    }
    return;
}



void do_north (CHAR_DATA * ch, char *argument)
{

  if (IS_AFFECTED(ch, AFF_DISORIENT) && number_range(0,1) == 1)
  {
    int random = number_range(0,6);
    switch(random)
    {
        case 1: move_char (ch, DIR_SOUTH, FALSE, FALSE);
		send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
		break;
        case 2: move_char (ch, DIR_EAST, FALSE, FALSE);
		send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
		break;
        case 3: move_char (ch, DIR_WEST, FALSE, FALSE);
		send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
		break;
        case 4: move_char (ch, DIR_UP, FALSE, FALSE);
		send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
		break;
        case 5: move_char (ch, DIR_DOWN, FALSE, FALSE);
		send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
		break;
	default: send_to_char("\n\rYou feel to disoriented to move.\n\r", ch);
		break;
    }
    return;
  }
  else
  {	
    move_char (ch, DIR_NORTH, FALSE, FALSE);
    return;
  }
}



void do_east (CHAR_DATA * ch, char *argument)
{
  if (IS_AFFECTED(ch, AFF_DISORIENT) && number_range(0,1) == 1)
  {
    int random = number_range(0,6);
    switch(random)
    {
        case 1: move_char (ch, DIR_SOUTH, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 2: move_char (ch, DIR_EAST, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 3: move_char (ch, DIR_WEST, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 4: move_char (ch, DIR_UP, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 5: move_char (ch, DIR_DOWN, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        default: send_to_char("\n\rYou feel to disoriented to move.\n\r", ch);
                break;
    }
    return;
  }
  else
  {
    move_char (ch, DIR_EAST, FALSE, FALSE);
    return;
  }
}



void do_south (CHAR_DATA * ch, char *argument)
{
  if (IS_AFFECTED(ch, AFF_DISORIENT) && number_range(0,1) == 1)
  {
    int random = number_range(0,6);
    switch(random)
    {
        case 1: move_char (ch, DIR_SOUTH, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 2: move_char (ch, DIR_EAST, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 3: move_char (ch, DIR_WEST, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 4: move_char (ch, DIR_UP, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 5: move_char (ch, DIR_DOWN, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        default: send_to_char("\n\rYou feel to disoriented to move.\n\r", ch);
                break;
    }
    return;
  }
  else
  {
    move_char (ch, DIR_SOUTH, FALSE, FALSE);
    return;
  }
}



void do_west (CHAR_DATA * ch, char *argument)
{
  if (IS_AFFECTED(ch, AFF_DISORIENT) && number_range(0,1) == 1)
  {
    int random = number_range(0,6);
    switch(random)
    {
        case 1: move_char (ch, DIR_SOUTH, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 2: move_char (ch, DIR_EAST, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 3: move_char (ch, DIR_WEST, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 4: move_char (ch, DIR_UP, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 5: move_char (ch, DIR_DOWN, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        default: send_to_char("\n\rYou feel to disoriented to move.\n\r", ch);
                break;
    }
    return;
  }
  else
  {
    move_char (ch, DIR_WEST, FALSE, FALSE);
    return;
  }
}

void do_up (CHAR_DATA * ch, char *argument)
{
  if (IS_AFFECTED(ch, AFF_DISORIENT) && number_range(0,1) == 1)
  {
    int random = number_range(0,6);
    switch(random)
    {
        case 1: move_char (ch, DIR_SOUTH, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 2: move_char (ch, DIR_EAST, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 3: move_char (ch, DIR_WEST, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 4: move_char (ch, DIR_UP, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 5: move_char (ch, DIR_DOWN, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        default: send_to_char("\n\rYou feel to disoriented to move.\n\r", ch);
                break;
    }
    return;
  }
  else
  {
    move_char (ch, DIR_UP, FALSE, FALSE);
    return;
  }
}

void do_down (CHAR_DATA * ch, char *argument)
{
  if (IS_AFFECTED(ch, AFF_DISORIENT) && number_range(0,4) == 2)
  {
    int random = number_range(0,6);
    switch(random)
    {
        case 1: move_char (ch, DIR_SOUTH, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 2: move_char (ch, DIR_EAST, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 3: move_char (ch, DIR_WEST, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 4: move_char (ch, DIR_UP, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        case 5: move_char (ch, DIR_DOWN, FALSE, FALSE);
                send_to_char("\n\rYou stumble around disoriented.\n\r", ch);
                break;
        default: send_to_char("\n\rYou feel to disoriented to move.\n\r", ch);
                break;
    }
    return;
  }
  else
  {
    move_char (ch, DIR_DOWN, FALSE, FALSE);
    return;
  }
}



int find_door (CHAR_DATA * ch, char *arg)
{
    EXIT_DATA *pexit;
    int door;

    if (!str_cmp (arg, "n") || !str_cmp (arg, "north"))
        door = 0;
    else if (!str_cmp (arg, "e") || !str_cmp (arg, "east"))
        door = 1;
    else if (!str_cmp (arg, "s") || !str_cmp (arg, "south"))
        door = 2;
    else if (!str_cmp (arg, "w") || !str_cmp (arg, "west"))
        door = 3;
    else if (!str_cmp (arg, "u") || !str_cmp (arg, "up"))
        door = 4;
    else if (!str_cmp (arg, "d") || !str_cmp (arg, "down"))
        door = 5;
    else
    {
        for (door = 0; door <= 5; door++)
        {
            if ((pexit = ch->in_room->exit[door]) != NULL
                && IS_SET (pexit->exit_info, EX_ISDOOR)
                && pexit->keyword != NULL && is_name (arg, pexit->keyword))
                return door;
        }
        act ("I see no $T here.", ch, NULL, arg, TO_CHAR);
        return -1;
    }

    if ((pexit = ch->in_room->exit[door]) == NULL)
    {
        act ("I see no door $T here.", ch, NULL, arg, TO_CHAR);
        return -1;
    }

    if (!IS_SET (pexit->exit_info, EX_ISDOOR))
    {
        send_to_char ("You can't do that.\n\r", ch);
        return -1;
    }

    return door;
}



void do_open (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Open what?\n\r", ch);
        return;
    }

    if ((obj = get_obj_here (ch, NULL, arg)) != NULL)
    {
        OBJ_DATA *bomb = NULL;
        /* open portal */
        if (obj->item_type == ITEM_PORTAL)
        {
            if (!IS_SET (obj->value[1], EX_ISDOOR))
            {
                send_to_char ("You can't do that.\n\r", ch);
                return;
            }

            if (!IS_SET (obj->value[1], EX_CLOSED))
            {
                send_to_char ("It's already open.\n\r", ch);
                return;
            }

            if (IS_SET (obj->value[1], EX_LOCKED))
            {
                send_to_char ("It's locked.\n\r", ch);
                return;
            }

            REMOVE_BIT (obj->value[1], EX_CLOSED);
            act ("You open $p.", ch, obj, NULL, TO_CHAR);
            act ("$n opens $p.", ch, obj, NULL, TO_ROOM);
            return;
        }


		if ( obj->item_type == ITEM_WINDOW)
		{
			if ( !IS_SET ( obj->value[1], CONT_CLOSED))
			{
				send_to_char("It's already open.\n\r", ch);
				return;
			}
	        if (!IS_SET (obj->value[1], CONT_CLOSEABLE))
	        {
		        send_to_char ("This window can't be opened.\n\r", ch);
			    return;
			}
	        if (IS_SET (obj->value[1], CONT_LOCKED))
		    {
			    send_to_char ("This window really can't be opened.\n\r", ch);
				return;
			}
	        REMOVE_BIT (obj->value[1], CONT_CLOSED);
	        act ("You open $p.", ch, obj, NULL, TO_CHAR);
	        act ("$n opens $p.", ch, obj, NULL, TO_ROOM);
		    return;
		}


        
		/* 'open object' */
        if (obj->item_type != ITEM_CONTAINER)
        {
            send_to_char ("That's not a container.\n\r", ch);
            return;
        }
        if (!IS_SET (obj->value[1], CONT_CLOSED))
        {
            send_to_char ("It's already open.\n\r", ch);
            return;
        }
        if (!IS_SET (obj->value[1], CONT_CLOSEABLE))
        {
            send_to_char ("You can't do that.\n\r", ch);
            return;
        }
        if (IS_SET (obj->value[1], CONT_LOCKED))
        {
            send_to_char ("It's locked.\n\r", ch);
            return;
        }

        REMOVE_BIT (obj->value[1], CONT_CLOSED);
        if ((bomb = bomb_in_bag(obj)) != NULL)
        {
          (*explosive_table[bomb->value[1]].bomb_fun) (bomb, ch);
	   fire_effect(obj, bomb->level, bomb->value[3], TARGET_OBJ);
	   extract_obj(bomb);
	   return;
        } 
        act ("You open $p.", ch, obj, NULL, TO_CHAR);
        act ("$n opens $p.", ch, obj, NULL, TO_ROOM);
        return;
    }

    if ((door = find_door (ch, arg)) >= 0)
    {
        /* 'open door' */
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;

        pexit = ch->in_room->exit[door];
        if (!IS_SET (pexit->exit_info, EX_CLOSED))
        {
            send_to_char ("It's already open.\n\r", ch);
            return;
        }
        if (IS_SET (pexit->exit_info, EX_LOCKED))
        {
            send_to_char ("It's locked.\n\r", ch);
            return;
        }

        REMOVE_BIT (pexit->exit_info, EX_CLOSED);
        act ("$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM);
        send_to_char ("Ok.\n\r", ch);

        /* open the other side */
        if ((to_room = pexit->u1.to_room) != NULL
            && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
            && pexit_rev->u1.to_room == ch->in_room)
        {
            CHAR_DATA *rch;

            REMOVE_BIT (pexit_rev->exit_info, EX_CLOSED);
            for (rch = to_room->people; rch != NULL; rch = rch->next_in_room)
                act ("The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR);
        }
    }

    return;
}



void do_close (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Close what?\n\r", ch);
        return;
    }

    if ((obj = get_obj_here (ch, NULL, arg)) != NULL)
    {
        /* portal stuff */
        if (obj->item_type == ITEM_PORTAL)
        {

            if (!IS_SET (obj->value[1], EX_ISDOOR)
                || IS_SET (obj->value[1], EX_NOCLOSE))
            {
                send_to_char ("You can't do that.\n\r", ch);
                return;
            }

            if (IS_SET (obj->value[1], EX_CLOSED))
            {
                send_to_char ("It's already closed.\n\r", ch);
                return;
            }

            SET_BIT (obj->value[1], EX_CLOSED);
            act ("You close $p.", ch, obj, NULL, TO_CHAR);
            act ("$n closes $p.", ch, obj, NULL, TO_ROOM);
            return;
        }

        /* 'close object' */

		if(obj->item_type == ITEM_WINDOW)
		{
	        if (IS_SET (obj->value[1], CONT_CLOSED))
		    {
			    send_to_char ("It's already closed.\n\r", ch);
				return;
			}
	        if (!IS_SET (obj->value[1], CONT_CLOSEABLE))
		    {
			    send_to_char ("You can't close this window.\n\r", ch);
				return;
			}
        SET_BIT (obj->value[1], CONT_CLOSED);
        act ("You close $p.", ch, obj, NULL, TO_CHAR);
        act ("$n closes $p.", ch, obj, NULL, TO_ROOM);
        return;
		}
        
		if (obj->item_type != ITEM_CONTAINER)
        {
            send_to_char ("That's not a container.\n\r", ch);
            return;
        }
        if (IS_SET (obj->value[1], CONT_CLOSED))
        {
            send_to_char ("It's already closed.\n\r", ch);
            return;
        }
        if (!IS_SET (obj->value[1], CONT_CLOSEABLE))
        {
            send_to_char ("You can't do that.\n\r", ch);
            return;
        }

        SET_BIT (obj->value[1], CONT_CLOSED);
        act ("You close $p.", ch, obj, NULL, TO_CHAR);
        act ("$n closes $p.", ch, obj, NULL, TO_ROOM);
        return;
    }

    if ((door = find_door (ch, arg)) >= 0)
    {
        /* 'close door' */
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;

        pexit = ch->in_room->exit[door];
        if (IS_SET (pexit->exit_info, EX_CLOSED))
        {
            send_to_char ("It's already closed.\n\r", ch);
            return;
        }

        SET_BIT (pexit->exit_info, EX_CLOSED);
        act ("$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM);
        send_to_char ("Ok.\n\r", ch);

        /* close the other side */
        if ((to_room = pexit->u1.to_room) != NULL
            && (pexit_rev = to_room->exit[rev_dir[door]]) != 0
            && pexit_rev->u1.to_room == ch->in_room)
        {
            CHAR_DATA *rch;

            SET_BIT (pexit_rev->exit_info, EX_CLOSED);
            for (rch = to_room->people; rch != NULL; rch = rch->next_in_room)
                act ("The $d closes.", rch, NULL, pexit_rev->keyword,
                     TO_CHAR);
        }
    }

    return;
}



bool has_key (CHAR_DATA * ch, long key)
{
    OBJ_DATA *obj;
    OBJ_DATA *keyring;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
        if (obj->pIndexData->vnum == key)
            return TRUE;
    }
    
    if ((keyring = get_eq_char (ch, WEAR_ON_BELT)) != NULL)
    {
      if (keyring->item_type == ITEM_KEYRING)
      {
      	for (obj = keyring->contains; obj != NULL; obj = obj->next_content)
        {
          if (obj->pIndexData->vnum == key)
            return TRUE;
        }
      }
    }	
    return FALSE;
}



void do_lock (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Lock what?\n\r", ch);
        return;
    }

    if ((obj = get_obj_here (ch, NULL, arg)) != NULL)
    {
        /* portal stuff */
        if (obj->item_type == ITEM_PORTAL)
        {
            if (!IS_SET (obj->value[1], EX_ISDOOR)
                || IS_SET (obj->value[1], EX_NOCLOSE))
            {
                send_to_char ("You can't do that.\n\r", ch);
                return;
            }
            if (!IS_SET (obj->value[1], EX_CLOSED))
            {
                send_to_char ("It's not closed.\n\r", ch);
                return;
            }

            if (obj->value[4] < 0 || IS_SET (obj->value[1], EX_NOLOCK))
            {
                send_to_char ("It can't be locked.\n\r", ch);
                return;
            }

            if (!has_key (ch, obj->value[4]))
            {
                send_to_char ("You lack the key.\n\r", ch);
                return;
            }

            if (IS_SET (obj->value[1], EX_LOCKED))
            {
                send_to_char ("It's already locked.\n\r", ch);
                return;
            }

            SET_BIT (obj->value[1], EX_LOCKED);
            act ("You lock $p.", ch, obj, NULL, TO_CHAR);
            act ("$n locks $p.", ch, obj, NULL, TO_ROOM);
            return;
        }

        /* 'lock object' */
        if (obj->item_type != ITEM_CONTAINER)
        {
            send_to_char ("That's not a container.\n\r", ch);
            return;
        }
        if (!IS_SET (obj->value[1], CONT_CLOSED))
        {
            send_to_char ("It's not closed.\n\r", ch);
            return;
        }
        if (obj->value[2] < 0)
        {
            send_to_char ("It can't be locked.\n\r", ch);
            return;
        }
        if (!has_key (ch, obj->value[2]))
        {
            send_to_char ("You lack the key.\n\r", ch);
            return;
        }
        if (IS_SET (obj->value[1], CONT_LOCKED))
        {
            send_to_char ("It's already locked.\n\r", ch);
            return;
        }

        SET_BIT (obj->value[1], CONT_LOCKED);
        act ("You lock $p.", ch, obj, NULL, TO_CHAR);
        act ("$n locks $p.", ch, obj, NULL, TO_ROOM);
        return;
    }

    if ((door = find_door (ch, arg)) >= 0)
    {
        /* 'lock door' */
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;

        pexit = ch->in_room->exit[door];
        if (!IS_SET (pexit->exit_info, EX_CLOSED))
        {
            send_to_char ("It's not closed.\n\r", ch);
            return;
        }
        if (pexit->key < 0)
        {
            send_to_char ("It can't be locked.\n\r", ch);
            return;
        }
        if (!has_key (ch, pexit->key))
        {
            send_to_char ("You lack the key.\n\r", ch);
            return;
        }
        if (IS_SET (pexit->exit_info, EX_LOCKED))
        {
            send_to_char ("It's already locked.\n\r", ch);
            return;
        }

        SET_BIT (pexit->exit_info, EX_LOCKED);
        send_to_char ("*Click*\n\r", ch);
        act ("$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM);

        /* lock the other side */
        if ((to_room = pexit->u1.to_room) != NULL
            && (pexit_rev = to_room->exit[rev_dir[door]]) != 0
            && pexit_rev->u1.to_room == ch->in_room)
        {
            SET_BIT (pexit_rev->exit_info, EX_LOCKED);
        }
    }

    return;
}



void do_unlock (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Unlock what?\n\r", ch);
        return;
    }

    if ((obj = get_obj_here (ch, NULL, arg)) != NULL)
    {
        /* portal stuff */
        if (obj->item_type == ITEM_PORTAL)
        {
            if (!IS_SET (obj->value[1], EX_ISDOOR))
            {
                send_to_char ("You can't do that.\n\r", ch);
                return;
            }

            if (!IS_SET (obj->value[1], EX_CLOSED))
            {
                send_to_char ("It's not closed.\n\r", ch);
                return;
            }

            if (obj->value[4] < 0)
            {
                send_to_char ("It can't be unlocked.\n\r", ch);
                return;
            }

            if (!has_key (ch, obj->value[4]))
            {
                send_to_char ("You lack the key.\n\r", ch);
                return;
            }

            if (!IS_SET (obj->value[1], EX_LOCKED))
            {
                send_to_char ("It's already unlocked.\n\r", ch);
                return;
            }

            REMOVE_BIT (obj->value[1], EX_LOCKED);
            act ("You unlock $p.", ch, obj, NULL, TO_CHAR);
            act ("$n unlocks $p.", ch, obj, NULL, TO_ROOM);
            return;
        }

        /* 'unlock object' */
        if (obj->item_type != ITEM_CONTAINER)
        {
            send_to_char ("That's not a container.\n\r", ch);
            return;
        }
        if (!IS_SET (obj->value[1], CONT_CLOSED))
        {
            send_to_char ("It's not closed.\n\r", ch);
            return;
        }
        if (obj->value[2] < 0)
        {
            send_to_char ("It can't be unlocked.\n\r", ch);
            return;
        }
        if (!has_key (ch, obj->value[2]))
        {
            send_to_char ("You lack the key.\n\r", ch);
            return;
        }
        if (!IS_SET (obj->value[1], CONT_LOCKED))
        {
            send_to_char ("It's already unlocked.\n\r", ch);
            return;
        }

        REMOVE_BIT (obj->value[1], CONT_LOCKED);
        act ("You unlock $p.", ch, obj, NULL, TO_CHAR);
        act ("$n unlocks $p.", ch, obj, NULL, TO_ROOM);
        return;
    }

    if ((door = find_door (ch, arg)) >= 0)
    {
        /* 'unlock door' */
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;

        pexit = ch->in_room->exit[door];
        if (!IS_SET (pexit->exit_info, EX_CLOSED))
        {
            send_to_char ("It's not closed.\n\r", ch);
            return;
        }
        if (pexit->key < 0)
        {
            send_to_char ("It can't be unlocked.\n\r", ch);
            return;
        }
        if (!has_key (ch, pexit->key))
        {
            send_to_char ("You lack the key.\n\r", ch);
            return;
        }
        if (!IS_SET (pexit->exit_info, EX_LOCKED))
        {
            send_to_char ("It's already unlocked.\n\r", ch);
            return;
        }

        REMOVE_BIT (pexit->exit_info, EX_LOCKED);
        send_to_char ("*Click*\n\r", ch);
        act ("$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM);

        /* unlock the other side */
        if ((to_room = pexit->u1.to_room) != NULL
            && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
            && pexit_rev->u1.to_room == ch->in_room)
        {
            REMOVE_BIT (pexit_rev->exit_info, EX_LOCKED);
        }
    }

    return;
}



void do_pick (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    OBJ_DATA *obj;
    int door;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Pick what?\n\r", ch);
        return;
    }

    WAIT_STATE (ch, skill_table[gsn_pick_lock].beats);

    /* look for guards */
    for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
    {
        if (IS_NPC (gch) && IS_AWAKE (gch) && ch->level + 5 < gch->level)
        {
            act ("$N is standing too close to the lock.",
                 ch, NULL, gch, TO_CHAR);
            return;
        }
    }

    if (!IS_NPC (ch) && number_percent () > get_skill (ch, gsn_pick_lock))
    {
        send_to_char ("You failed.\n\r", ch);
        check_improve (ch, gsn_pick_lock, FALSE, 2);
        return;
    }

    if ((obj = get_obj_here (ch, NULL, arg)) != NULL)
    {
        /* portal stuff */
        if (obj->item_type == ITEM_PORTAL)
        {
            if (!IS_SET (obj->value[1], EX_ISDOOR))
            {
                send_to_char ("You can't do that.\n\r", ch);
                return;
            }

            if (!IS_SET (obj->value[1], EX_CLOSED))
            {
                send_to_char ("It's not closed.\n\r", ch);
                return;
            }

            if (obj->value[4] < 0)
            {
                send_to_char ("It can't be unlocked.\n\r", ch);
                return;
            }

            if (IS_SET (obj->value[1], EX_PICKPROOF))
            {
                send_to_char ("You failed.\n\r", ch);
                return;
            }

            REMOVE_BIT (obj->value[1], EX_LOCKED);
            act ("You pick the lock on $p.", ch, obj, NULL, TO_CHAR);
            act ("$n picks the lock on $p.", ch, obj, NULL, TO_ROOM);
            check_improve (ch, gsn_pick_lock, TRUE, 2);
            return;
        }





        /* 'pick object' */
        if (obj->item_type != ITEM_CONTAINER)
        {
            send_to_char ("That's not a container.\n\r", ch);
            return;
        }
        if (!IS_SET (obj->value[1], CONT_CLOSED))
        {
            send_to_char ("It's not closed.\n\r", ch);
            return;
        }
        if (obj->value[2] < 0)
        {
            send_to_char ("It can't be unlocked.\n\r", ch);
            return;
        }
        if (!IS_SET (obj->value[1], CONT_LOCKED))
        {
            send_to_char ("It's already unlocked.\n\r", ch);
            return;
        }
        if (IS_SET (obj->value[1], CONT_PICKPROOF))
        {
            send_to_char ("You failed.\n\r", ch);
            return;
        }

        REMOVE_BIT (obj->value[1], CONT_LOCKED);
        act ("You pick the lock on $p.", ch, obj, NULL, TO_CHAR);
        act ("$n picks the lock on $p.", ch, obj, NULL, TO_ROOM);
        check_improve (ch, gsn_pick_lock, TRUE, 2);
        return;
    }

    if ((door = find_door (ch, arg)) >= 0)
    {
        /* 'pick door' */
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;

        pexit = ch->in_room->exit[door];
        if (!IS_SET (pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL (ch))
        {
            send_to_char ("It's not closed.\n\r", ch);
            return;
        }
        if (pexit->key < 0 && !IS_IMMORTAL (ch))
        {
            send_to_char ("It can't be picked.\n\r", ch);
            return;
        }
        if (!IS_SET (pexit->exit_info, EX_LOCKED))
        {
            send_to_char ("It's already unlocked.\n\r", ch);
            return;
        }
        if (IS_SET (pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL (ch))
        {
            send_to_char ("You failed.\n\r", ch);
            return;
        }

        REMOVE_BIT (pexit->exit_info, EX_LOCKED);
        send_to_char ("*Click*\n\r", ch);
        act ("$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM);
        check_improve (ch, gsn_pick_lock, TRUE, 2);

        /* pick the other side */
        if ((to_room = pexit->u1.to_room) != NULL
            && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
            && pexit_rev->u1.to_room == ch->in_room)
        {
            REMOVE_BIT (pexit_rev->exit_info, EX_LOCKED);
        }
    }

    return;
}




void do_stand (CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj = NULL;

	if(IS_AFFECTED(ch, AFF_SEVERED))
	{
		send_to_char("Stand without legs? How?\n\r",ch);
		return;
	}
    if (argument[0] != '\0')
    {

        if (ch->position == POS_FIGHTING)
        {
            send_to_char ("Maybe you should finish fighting first?\n\r", ch);
            return;
        }
        obj = get_obj_list (ch, argument, ch->in_room->contents);
        if (obj == NULL)
        {
            send_to_char ("You don't see that here.\n\r", ch);
            return;
        }
        if (obj->item_type != ITEM_FURNITURE
            || (!IS_SET (obj->value[2], STAND_AT)
                && !IS_SET (obj->value[2], STAND_ON)
                && !IS_SET (obj->value[2], STAND_IN)))
        {
            send_to_char ("You can't seem to find a place to stand.\n\r", ch);
            return;
        }
        if (ch->on != obj && count_users (obj) >= obj->value[0])
        {
            act_new ("There's no room to stand on $p.",
                     ch, obj, NULL, TO_CHAR, POS_DEAD);
            return;
        }
        ch->on = obj;
        if ( HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )  
          p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );
   }

    switch (ch->position)
    {
        case POS_SLEEPING:
            if (IS_AFFECTED (ch, AFF_SLEEP)||IS_AFFECTED (ch, AFF_BLACKJACK))
            {
                send_to_char ("You can't wake up!\n\r", ch);
                return;
            }
            

            if (obj == NULL)
            {
                send_to_char ("You wake and stand up.\n\r", ch);
                act ("$n wakes and stands up.", ch, NULL, NULL, TO_ROOM);
                ch->on = NULL;
            }
            else if (IS_SET (obj->value[2], STAND_AT))
            {
                act_new ("You wake and stand at $p.", ch, obj, NULL, TO_CHAR,
                         POS_DEAD);
                act ("$n wakes and stands at $p.", ch, obj, NULL, TO_ROOM);
            }
            else if (IS_SET (obj->value[2], STAND_ON))
            {
                act_new ("You wake and stand on $p.", ch, obj, NULL, TO_CHAR,
                         POS_DEAD);
                act ("$n wakes and stands on $p.", ch, obj, NULL, TO_ROOM);
            }
            else
            {
                act_new ("You wake and stand in $p.", ch, obj, NULL, TO_CHAR,
                         POS_DEAD);
                act ("$n wakes and stands in $p.", ch, obj, NULL, TO_ROOM);
            }
            ch->position = POS_STANDING;
            do_function (ch, &do_look, "auto");
            break;
        case POS_MEDITATING:
        case POS_RESTING:
        case POS_SITTING:
            if (obj == NULL)
            {
                send_to_char ("You stand up.\n\r", ch);
                act ("$n stands up.", ch, NULL, NULL, TO_ROOM);
                ch->on = NULL;
            }
            else if (IS_SET (obj->value[2], STAND_AT))
            {
                act ("You stand at $p.", ch, obj, NULL, TO_CHAR);
                act ("$n stands at $p.", ch, obj, NULL, TO_ROOM);
            }
            else if (IS_SET (obj->value[2], STAND_ON))
            {
                act ("You stand on $p.", ch, obj, NULL, TO_CHAR);
                act ("$n stands on $p.", ch, obj, NULL, TO_ROOM);
            }
            else
            {
                act ("You stand in $p.", ch, obj, NULL, TO_CHAR);
                act ("$n stands on $p.", ch, obj, NULL, TO_ROOM);
            }
            ch->position = POS_STANDING;
            break;

        case POS_STANDING:
            send_to_char ("You are already standing.\n\r", ch);
            break;

        case POS_FIGHTING:
            send_to_char ("You are already fighting!\n\r", ch);
            break;
    }

    return;
}



void do_rest (CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj = NULL;

    if (ch->position == POS_FIGHTING)
    {
        send_to_char ("You are already fighting!\n\r", ch);
        return;
    }

    if (IS_MOUNTED(ch))
    {
      send_to_char("Why don't you dismount first.\n\r", ch);
      return;
    }

    /* okay, now that we know we can rest, find an object to rest on */
    if (argument[0] != '\0')
    {
        obj = get_obj_list (ch, argument, ch->in_room->contents);
        if (obj == NULL)
        {
            send_to_char ("You don't see that here.\n\r", ch);
            return;
        }
    }
    else
        obj = ch->on;

    if (obj != NULL)
    {
        if (obj->item_type != ITEM_FURNITURE
            || (!IS_SET (obj->value[2], REST_ON)
                && !IS_SET (obj->value[2], REST_IN)
                && !IS_SET (obj->value[2], REST_AT)))
        {
            send_to_char ("You can't rest on that.\n\r", ch);
            return;
        }

        if (obj != NULL && ch->on != obj
            && count_users (obj) >= obj->value[0])
        {
            act_new ("There's no more room on $p.", ch, obj, NULL, TO_CHAR,
                     POS_DEAD);
            return;
        }

        ch->on = obj;
        if ( HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )
          p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );

    }

    switch (ch->position)
    {
        case POS_SLEEPING:
            if (IS_AFFECTED (ch, AFF_SLEEP)||IS_AFFECTED (ch, AFF_BLACKJACK))
            {
                send_to_char ("You can't wake up!\n\r", ch);
                return;
            }

            if (obj == NULL)
            {
                send_to_char ("You wake up and start resting.\n\r", ch);
                act ("$n wakes up and starts resting.", ch, NULL, NULL,
                     TO_ROOM);
            }
            else if (IS_SET (obj->value[2], REST_AT))
            {
                act_new ("You wake up and rest at $p.",
                         ch, obj, NULL, TO_CHAR, POS_SLEEPING);
                act ("$n wakes up and rests at $p.", ch, obj, NULL, TO_ROOM);
            }
            else if (IS_SET (obj->value[2], REST_ON))
            {
                act_new ("You wake up and rest on $p.",
                         ch, obj, NULL, TO_CHAR, POS_SLEEPING);
                act ("$n wakes up and rests on $p.", ch, obj, NULL, TO_ROOM);
            }
            else
            {
                act_new ("You wake up and rest in $p.",
                         ch, obj, NULL, TO_CHAR, POS_SLEEPING);
                act ("$n wakes up and rests in $p.", ch, obj, NULL, TO_ROOM);
            }
            ch->position = POS_RESTING;
            break;

        case POS_RESTING:
            send_to_char ("You are already resting.\n\r", ch);
            break;

        case POS_STANDING:
            if (obj == NULL)
            {
                send_to_char ("You rest.\n\r", ch);
                act ("$n sits down and rests.", ch, NULL, NULL, TO_ROOM);
            }
            else if (IS_SET (obj->value[2], REST_AT))
            {
                act ("You sit down at $p and rest.", ch, obj, NULL, TO_CHAR);
                act ("$n sits down at $p and rests.", ch, obj, NULL, TO_ROOM);
            }
            else if (IS_SET (obj->value[2], REST_ON))
            {
                act ("You sit on $p and rest.", ch, obj, NULL, TO_CHAR);
                act ("$n sits on $p and rests.", ch, obj, NULL, TO_ROOM);
            }
            else
            {
                act ("You rest in $p.", ch, obj, NULL, TO_CHAR);
                act ("$n rests in $p.", ch, obj, NULL, TO_ROOM);
            }
            ch->position = POS_RESTING;
            break;

        case POS_SITTING:
            if (obj == NULL)
            {
                send_to_char ("You rest.\n\r", ch);
                act ("$n rests.", ch, NULL, NULL, TO_ROOM);
            }
            else if (IS_SET (obj->value[2], REST_AT))
            {
                act ("You rest at $p.", ch, obj, NULL, TO_CHAR);
                act ("$n rests at $p.", ch, obj, NULL, TO_ROOM);
            }
            else if (IS_SET (obj->value[2], REST_ON))
            {
                act ("You rest on $p.", ch, obj, NULL, TO_CHAR);
                act ("$n rests on $p.", ch, obj, NULL, TO_ROOM);
            }
            else
            {
                act ("You rest in $p.", ch, obj, NULL, TO_CHAR);
                act ("$n rests in $p.", ch, obj, NULL, TO_ROOM);
            }
            ch->position = POS_RESTING;
            break;
    }


    return;
}


void do_sit (CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj = NULL;

    if (IS_MOUNTED(ch))
    {
      send_to_char("Why don't you dismount first.\n\r", ch);
      return;
    }

    if (IS_AFFECTED(ch, AFF_SEVERED))
    {
        send_to_char("Sit without legs? How?\n\r",ch);
        return;
    }
    if (ch->position == POS_FIGHTING)
    {
        send_to_char ("Maybe you should finish this fight first?\n\r", ch);
        return;
    }

    /* okay, now that we know we can sit, find an object to sit on */
    if (argument[0] != '\0')
    {
        obj = get_obj_list (ch, argument, ch->in_room->contents);
        if (obj == NULL)
        {
            send_to_char ("You don't see that here.\n\r", ch);
            return;
        }
    }
    else
        obj = ch->on;

    if (obj != NULL)
    {
        if (obj->item_type != ITEM_FURNITURE
            || (!IS_SET (obj->value[2], SIT_ON)
                && !IS_SET (obj->value[2], SIT_IN)
                && !IS_SET (obj->value[2], SIT_AT)))
        {
            send_to_char ("You can't sit on that.\n\r", ch);
            return;
        }

        if (obj != NULL && ch->on != obj
            && count_users (obj) >= obj->value[0])
        {
            act_new ("There's no more room on $p.", ch, obj, NULL, TO_CHAR,
                     POS_DEAD);
            return;
        }

        ch->on = obj;
        if ( HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )
          p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );

    }
    switch (ch->position)
    {
        case POS_SLEEPING:
            if (IS_AFFECTED (ch, AFF_SLEEP)||IS_AFFECTED (ch, AFF_BLACKJACK))
            {
                send_to_char ("You can't wake up!\n\r", ch);
                return;
            }

            if (obj == NULL)
            {
                send_to_char ("You wake and sit up.\n\r", ch);
                act ("$n wakes and sits up.", ch, NULL, NULL, TO_ROOM);
            }
            else if (IS_SET (obj->value[2], SIT_AT))
            {
                act_new ("You wake and sit at $p.", ch, obj, NULL, TO_CHAR,
                         POS_DEAD);
                act ("$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM);
            }
            else if (IS_SET (obj->value[2], SIT_ON))
            {
                act_new ("You wake and sit on $p.", ch, obj, NULL, TO_CHAR,
                         POS_DEAD);
                act ("$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM);
            }
            else
            {
                act_new ("You wake and sit in $p.", ch, obj, NULL, TO_CHAR,
                         POS_DEAD);
                act ("$n wakes and sits in $p.", ch, obj, NULL, TO_ROOM);
            }

            ch->position = POS_SITTING;
            break;
        case POS_RESTING:
            if (obj == NULL)
                send_to_char ("You stop resting.\n\r", ch);
            else if (IS_SET (obj->value[2], SIT_AT))
            {
                act ("You sit at $p.", ch, obj, NULL, TO_CHAR);
                act ("$n sits at $p.", ch, obj, NULL, TO_ROOM);
            }

            else if (IS_SET (obj->value[2], SIT_ON))
            {
                act ("You sit on $p.", ch, obj, NULL, TO_CHAR);
                act ("$n sits on $p.", ch, obj, NULL, TO_ROOM);
            }
            ch->position = POS_SITTING;
            break;
        case POS_SITTING:
            send_to_char ("You are already sitting down.\n\r", ch);
            break;
        case POS_STANDING:
            if (obj == NULL)
            {
                send_to_char ("You sit down.\n\r", ch);
                act ("$n sits down on the ground.", ch, NULL, NULL, TO_ROOM);
            }
            else if (IS_SET (obj->value[2], SIT_AT))
            {
                act ("You sit down at $p.", ch, obj, NULL, TO_CHAR);
                act ("$n sits down at $p.", ch, obj, NULL, TO_ROOM);
            }
            else if (IS_SET (obj->value[2], SIT_ON))
            {
                act ("You sit on $p.", ch, obj, NULL, TO_CHAR);
                act ("$n sits on $p.", ch, obj, NULL, TO_ROOM);
            }
            else
            {
                act ("You sit down in $p.", ch, obj, NULL, TO_CHAR);
                act ("$n sits down in $p.", ch, obj, NULL, TO_ROOM);
            }
            ch->position = POS_SITTING;
            break;
    }
    return;
}


void do_sleep (CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj = NULL;

    if (IS_MOUNTED(ch))
    {
      send_to_char("Why don't you dismount first.\n\r", ch);
      return;
    }

    switch (ch->position)
    {
        case POS_SLEEPING:
            send_to_char ("You are already sleeping.\n\r", ch);
            break;

        case POS_RESTING:
        case POS_SITTING:
        case POS_STANDING:
            if (argument[0] == '\0' && ch->on == NULL)
            {
                send_to_char ("You go to sleep.\n\r", ch);
                act ("$n goes to sleep.", ch, NULL, NULL, TO_ROOM);
                ch->position = POS_SLEEPING;
            }
            else
            {                    /* find an object and sleep on it */

                if (argument[0] == '\0')
                    obj = ch->on;
                else
                    obj = get_obj_list (ch, argument, ch->in_room->contents);

                if (obj == NULL)
                {
                    send_to_char ("You don't see that here.\n\r", ch);
                    return;
                }
                if (obj->item_type != ITEM_FURNITURE
                    || (!IS_SET (obj->value[2], SLEEP_ON)
                        && !IS_SET (obj->value[2], SLEEP_IN)
                        && !IS_SET (obj->value[2], SLEEP_AT)))
                {
                    send_to_char ("You can't sleep on that!\n\r", ch);
                    return;
                }

                if (ch->on != obj && count_users (obj) >= obj->value[0])
                {
                    act_new ("There is no room on $p for you.",
                             ch, obj, NULL, TO_CHAR, POS_DEAD);
                    return;
                }

                ch->on = obj;
	        if ( HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )
        	   p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );


                if (IS_SET (obj->value[2], SLEEP_AT))
                {
                    act ("You go to sleep at $p.", ch, obj, NULL, TO_CHAR);
                    act ("$n goes to sleep at $p.", ch, obj, NULL, TO_ROOM);
                }
                else if (IS_SET (obj->value[2], SLEEP_ON))
                {
                    act ("You go to sleep on $p.", ch, obj, NULL, TO_CHAR);
                    act ("$n goes to sleep on $p.", ch, obj, NULL, TO_ROOM);
                }
                else
                {
                    act ("You go to sleep in $p.", ch, obj, NULL, TO_CHAR);
                    act ("$n goes to sleep in $p.", ch, obj, NULL, TO_ROOM);
                }
                ch->position = POS_SLEEPING;
            }
            break;

        case POS_FIGHTING:
            send_to_char ("You are already fighting!\n\r", ch);
            break;
    }

    return;
}



void do_wake (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);
    if (arg[0] == '\0')
    {
        do_function (ch, &do_stand, "");
        return;
    }

    if (!IS_AWAKE (ch))
    {
        send_to_char ("You are asleep yourself!\n\r", ch);
        return;
    }

    if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (IS_AWAKE (victim))
    {
        act ("$N is already awake.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (IS_AFFECTED (victim, AFF_SLEEP)||IS_AFFECTED (ch, AFF_BLACKJACK))
    {
        act ("You can't wake $M!", ch, NULL, victim, TO_CHAR);
        return;
    }
    act ("You rudely wake $N.",ch,NULL,victim,TO_CHAR);
    act_new ("$n rudely wakes you.", ch, NULL, victim, TO_VICT, POS_SLEEPING);
    do_function (victim, &do_stand, "");
    return;
}



void do_sneak (CHAR_DATA * ch, char *argument)
{
    AFFECT_DATA af;

    send_to_char ("You attempt to move silently.\n\r", ch);
    

    if (IS_AFFECTED (ch, AFF_SNEAK))
        affect_strip (ch, gsn_sneak);

    if (number_percent () < get_skill (ch, gsn_sneak))
    {
        check_improve (ch, gsn_sneak, TRUE, 3);
        af.where = TO_AFFECTS;
        af.type = gsn_sneak;
        af.level = ch->level;
        af.duration = ch->level;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_SNEAK;
        affect_to_char (ch, &af);
    }
    else
        check_improve (ch, gsn_sneak, FALSE, 3);

    return;
}

/*****************************************
*                                        *
*  Below is Diem's anti-insanity code    *
*  do NOT touch it. You have been warned *
*    
* ROFL OR WHAT?                          *
******************************************/

void do_diemcloak (CHAR_DATA * ch, char *argument)
{
     AFFECT_DATA af;
      
    if (IS_AFFECTED (ch, AFF_DIEMCLOAK))
      {
       STR_REMOVE_BIT(ch->affected_by, AFF_DIEMCLOAK);
       send_to_char ("!!VISIBLE!!\n\r", ch);
       return;
      }
       
    if (str_cmp(ch->name, "stigmata"))
      {
        send_to_char ("Huh?\n\r",ch);
        return;
      }
    af.where = TO_AFFECTS;
    af.type = gsn_hide;
    af.level = ch->level;
    af.duration = -1;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_DIEMCLOAK;
    affect_to_char (ch, &af);
    return;
}


void do_hide (CHAR_DATA * ch, char *argument)
{
    AFFECT_DATA af;
    send_to_char ("You attempt to hide.\n\r", ch);

    if (IS_AFFECTED (ch, AFF_HIDE))
        affect_strip (ch, gsn_hide);

    if (number_percent () < get_skill (ch, gsn_hide))
    {
        af.where = TO_AFFECTS;
        af.type = gsn_hide;
        af.level = ch->level;
        af.duration = int(ch->level * 1.5);
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_HIDE;
        affect_to_char (ch, &af);
        check_improve (ch, gsn_hide, TRUE, 3);
    }
    else
        check_improve (ch, gsn_hide, FALSE, 3);

    return;
}



/*
 * Contributed by Alander.
 */
void do_visible (CHAR_DATA * ch, char *argument)
{
    affect_strip (ch, gsn_invis);
    affect_strip (ch, gsn_mass_invis);
    affect_strip (ch, gsn_hide);
    affect_strip (ch, gsn_sneak);
    affect_strip (ch, gsn_stealth);
    affect_strip (ch, gsn_shroud);
    STR_REMOVE_BIT (ch->affected_by, AFF_HIDE);
    STR_REMOVE_BIT (ch->affected_by, AFF_INVISIBLE);
    STR_REMOVE_BIT (ch->affected_by, AFF_SNEAK);
    STR_REMOVE_BIT (ch->affected_by, AFF_SHROUD);
    STR_REMOVE_BIT (ch->affected_by, AFF_STEALTH);
    REM_DISGUISE (ch);
    send_to_char ("Ok.\n\r", ch);
    return;
}



void do_recall (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;

    if (IS_NPC (ch) && !IS_SET (ch->act, ACT_PET))
    {
        send_to_char ("Only players can recall.\n\r", ch);
        return;
    }

    if (ch->level > 10)
    {
        send_to_char("Aren't you a little old for that?\n\r", ch);
        return;
    }

    act ("$n prays for transportation!", ch, 0, 0, TO_ROOM);

    if ((location = get_room_index (ROOM_VNUM_TEMPLE)) == NULL)
    {
        send_to_char ("You are completely lost.\n\r", ch);
        return;
    }

    if (ch->in_room == location)
        return;

    if (IS_SET (ch->in_room->room_flags, ROOM_NO_RECALL)
        || IS_AFFECTED (ch, AFF_CURSE))
    {
        send_to_char ("You can't seem to recall anything.\n\r", ch);
        return;
    }

    if ((victim = ch->fighting) != NULL)
    {
        int lose, skill;

        skill = get_skill (ch, gsn_recall);

        if (number_percent () < int(80 * skill / 100.0))
        {
            check_improve (ch, gsn_recall, FALSE, 6);
            WAIT_STATE (ch, 4);
            sprintf (buf, "You failed!.\n\r");
            send_to_char (buf, ch);
            return;
        }

        lose = (ch->desc == NULL) ? 15 : 25;
        gain_exp (ch, 0 - lose);
        check_improve (ch, gsn_recall, TRUE, 4);
        sprintf (buf, "You recall from combat!  You lose %d experience points.\n\r", lose);
        send_to_char (buf, ch);
        stop_fighting (ch, TRUE);

    }

    ch->move = int(ch->move*.67);
    act ("$n closes $s eyes and disappears.", ch, NULL, NULL, TO_ROOM);
    char_from_room (ch);
    char_to_room (ch, location);
    act ("$n suddenly appears in the room.", ch, NULL, NULL, TO_ROOM);
    do_function (ch, &do_look, "auto");

    if (ch->pet != NULL)
        do_function (ch->pet, &do_recall, "");

    return;
}



void do_train (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *mob;
    sh_int stat = -1;
    char *pOutput = NULL;
    int cost;
    int permhp, permmana;
    int newhp, pgain;

    if (IS_NPC (ch))
        return;

    permhp = ch->pcdata->perm_hit;
    permmana = ch->pcdata->perm_mana;


    /*
     * Check for trainer.
     */
    for (mob = ch->in_room->people; mob; mob = mob->next_in_room)
    {
        if (IS_NPC (mob) && IS_SET (mob->act, ACT_TRAIN))
            break;
    }

    if (mob == NULL)
    {
        send_to_char ("You can't do that here.\n\r", ch);
        return;
    }

    if (argument[0] == '\0')
    {
        sprintf (buf, "You have %d training sessions.\n\r", ch->train);
        send_to_char (buf, ch);
        argument = "foo";
    }

    cost = 1;

    if (!str_cmp (argument, "str"))
    {
        if (class_table[ch->cClass].attr_prime == STAT_STR)
            cost = 1;
        stat = STAT_STR;
        pOutput = "strength";
    }

    else if (!str_cmp (argument, "int"))
    {
        if (class_table[ch->cClass].attr_prime == STAT_INT)
            cost = 1;
        stat = STAT_INT;
        pOutput = "intelligence";
    }

    else if (!str_cmp (argument, "wis"))
    {
        if (class_table[ch->cClass].attr_prime == STAT_WIS)
            cost = 1;
        stat = STAT_WIS;
        pOutput = "wisdom";
    }

    else if (!str_cmp (argument, "dex"))
    {
        if (class_table[ch->cClass].attr_prime == STAT_DEX)
            cost = 1;
        stat = STAT_DEX;
        pOutput = "dexterity";
    }

    else if (!str_cmp (argument, "con"))
    {
        if (class_table[ch->cClass].attr_prime == STAT_CON)
            cost = 1;
        stat = STAT_CON;
        pOutput = "constitution";
    }

    else if (!str_cmp (argument, "hp"))
        cost = 1;

    else if (!str_cmp (argument, "mana"))
        cost = 1;

    else if (!str_cmp (argument, "move"))
        cost = 1;

    else
    {
        strcpy (buf, "You can train:");
        if (ch->perm_stat[STAT_STR] < get_max_train (ch, STAT_STR))
            strcat (buf, " str");
        if (ch->perm_stat[STAT_INT] < get_max_train (ch, STAT_INT))
            strcat (buf, " int");
        if (ch->perm_stat[STAT_WIS] < get_max_train (ch, STAT_WIS))
            strcat (buf, " wis");
        if (ch->perm_stat[STAT_DEX] < get_max_train (ch, STAT_DEX))
            strcat (buf, " dex");
        if (ch->perm_stat[STAT_CON] < get_max_train (ch, STAT_CON))
            strcat (buf, " con");
        if (permhp + 10 < max_natural_hp(ch))
          strcat (buf, " hp");
        if (permmana + 10 < max_natural_mana(ch))
          strcat(buf, " mana");
	if (ch->max_move < 1500)
	  strcat(buf, " move");

        if (buf[strlen (buf) - 1] != ':')
        {
            strcat (buf, ".\n\r");
            send_to_char (buf, ch);
        }
        else
        {
            /*
             * This message dedicated to Jordan ... you big stud!
             */
            act ("You have nothing left to train, you $T!",
                 ch, NULL,
                 ch->sex == SEX_MALE ? "big stud" :
                 ch->sex == SEX_FEMALE ? "hot babe" : "wild thing", TO_CHAR);
        }

        return;
    }

    if (!str_cmp ("hp", argument))
    {
        if (cost > ch->train)
        {
            send_to_char ("You don't have enough training sessions.\n\r", ch);
            return;
        }	
	if (permhp > max_natural_hp(ch) + 10)
	{
	    send_to_char ("You can't train that anymore.\n\r", ch);
	    return;
	}
        if (permhp + 10 >= max_natural_hp(ch))
	{
	   char msg[MSL];
        /* char buf[MSL];*/
	/*THOM added this*/
         ch->train -= cost;
	   newhp = max_natural_hp(ch) - permhp;   /*hp differnce*/
	   ch->pcdata->perm_hit += newhp;
	   ch->max_hit += newhp;
	   ch->hit += newhp;	  
	   pgain = 10-newhp;                      /*hps to pracs*/
	   ch->practice += pgain;
	/*END of what he added*/
	   
	 /*  sprintf(buf, "CURRENT:%ld MAX:%d\n\r", ch->pcdata->perm_hit, max_natural_hp(ch));
           send_to_char(buf, ch);*/
           sprintf(msg, "You max your hit points and receive %d practice sessions.\n\r", pgain);
	   send_to_char(msg, ch);
	  /* send_to_char("You can't train that anymore right now.\n\r", ch);*/
           return;
        }

        ch->train -= cost;
        ch->pcdata->perm_hit += 10;
        ch->max_hit += 10;
        ch->hit += 10;
        act ("Your durability increases!", ch, NULL, NULL, TO_CHAR);
        act ("$n's durability increases!", ch, NULL, NULL, TO_ROOM);
        return;
    }

    if (!str_cmp ("mana", argument))
    {
        if (cost > ch->train)
        {
            send_to_char ("You don't have enough training sessions.\n\r", ch);
            return;
        }

        if (permmana + 10 > max_natural_mana(ch))
        {
	  send_to_char("You can't train that anymore right now.\n\r", ch);
          return;
        }

        ch->train -= cost;
        ch->pcdata->perm_mana += 10;
        ch->max_mana += 10;
        ch->mana += 10;
        act ("Your power increases!", ch, NULL, NULL, TO_CHAR);
        act ("$n's power increases!", ch, NULL, NULL, TO_ROOM);
        return;
    }

    if (!str_cmp("move",argument))
    {
	if (cost > ch->train)
        {
            send_to_char ("You don't have enough training sessions.\n\r", ch);
            return;
        }

        if (permmana + 10 > max_natural_mana(ch))
        {
          send_to_char("You can't train that anymore right now.\n\r", ch);
          return;
        }

        ch->train -=cost;
        ch->pcdata->perm_move += 10;
        ch->max_move += 10;
        ch->move += 10;
        act ("Your endurance increases!", ch, NULL, NULL, TO_CHAR);
        act ("$n's endurance increases!", ch, NULL, NULL, TO_ROOM);
        return;
    }

    if (ch->perm_stat[stat] >= get_max_train (ch, stat))
    {
        act ("Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR);
        return;
    }

    if (cost > ch->train)
    {
        send_to_char ("You don't have enough training sessions.\n\r", ch);
        return;
    }

    ch->train -= cost;

    ch->perm_stat[stat] += 1;
    act ("Your $T increases!", ch, NULL, pOutput, TO_CHAR);
    act ("$n's $T increases!", ch, NULL, pOutput, TO_ROOM);
    return;
}
