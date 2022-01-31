/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
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
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"

OBJ_DATA *get_portal args (( CHAR_DATA *ch, OBJ_DATA * list, int vnum ));
bool check_dispel args (( int dis_level, CHAR_DATA *victim, int sn));
extern char *target_name;

void spell_airbarrier(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  if (victim != ch)
  {
    send_to_char("Not on another target.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(victim, AFF_AIRBARRIER))
  {
    send_to_char("A barrier of air already surrounds you.\n\r", ch);
    return;
  }

  

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.location = APPLY_NONE;
  af.duration = level/4;
  af.bitvector = AFF_AIRBARRIER;
  af.modifier = 0;
  affect_to_char(victim, &af);
  act("$n creates a protective weave of air.", ch, victim, victim, TO_VICT);
  act("You create a protective weave of air around yourself.", ch, victim, victim, TO_CHAR); 
  return;
}
void spell_farsight (int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    if (IS_AFFECTED (ch, AFF_BLIND))
    {
        send_to_char ("Maybe it would help if you could see?\n\r", ch);
        return;
    }
/*
    do_function (ch, &do_scan, target_name);
*/
}


void spell_portal (int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *portal;

    if ((victim = get_char_world (ch, target_name)) == NULL
        || victim == ch
        || victim->in_room == NULL
        || (IS_SET (ch->in_room->room_flags, ROOM_NOPORT_FROM))
        || (IS_SET (ch->in_room->area->area_flags, AREA_NO_PORT_FROM))
        || !can_see_room (ch, victim->in_room)
        || (!IS_NPC(victim) && ((IS_IMMORTAL(victim)) && (ch->level < victim->level))))
        {
            send_to_char ("You failed.\n\r",ch);
            return;
        }

    if (!IS_NPC(victim)&& !IS_NPC(ch))
    {
    	if ((IS_SET (victim->in_room->room_flags, ROOM_PRIVATE) && (victim->pcdata->pk_timer == 0|| ch->pcdata->pk_timer == 0))
        || (IS_SET (victim->in_room->room_flags, ROOM_SOLITARY) && (victim->pcdata->pk_timer == 0|| ch->pcdata->pk_timer == 0))
        || (IS_AFFECTED(ch, AFF_DOME) && (victim->pcdata->pk_timer == 0 || ch->pcdata->pk_timer == 0))
        || (IS_SET (victim->in_room->area->area_flags, AREA_NO_PORT) &&(victim->pcdata->pk_timer == 0))
        || (IS_SET(victim->in_room->room_flags, ROOM_NOPORT)&& (victim->pcdata->pk_timer == 0 || ch->pcdata->pk_timer == 0))
        || victim->in_room->vnum == ROOM_VNUM_MORGUE)
        {
            send_to_char ("You failed.\n\r",ch);
            return;
        }
    }
    else
    {
    	if ((IS_SET (victim->in_room->room_flags, ROOM_PRIVATE))
        || (IS_SET (victim->in_room->room_flags, ROOM_SOLITARY))
        || (IS_AFFECTED(ch, AFF_DOME))
        || (IS_SET(victim->in_room->area->area_flags, AREA_UNLINKED))
	|| IS_SET(victim->in_room->room_flags, ROOM_NOPORT))
    {
        send_to_char ("You failed.\n\r", ch);
        return;
    }
    }

    if (ch->pcdata->pk_timer > 0)
	    ch->pcdata->portcounter += 1;

    if (ch->pcdata->portcounter >= 15)
    {
        send_to_char("Your heart is pounding too fast to use the Art of Travel.\n\r", ch);
        return;
    }

    portal = create_object (get_obj_index (OBJ_VNUM_PORTAL), 0);
    portal->timer = 2 + level / 25;
    portal->value[3] = victim->in_room->vnum;

    obj_to_room (portal, ch->in_room);
    
    act ("$p rises up from the ground.", ch, portal, NULL, TO_ROOM);
    act ("$p rises up before you.", ch, portal, NULL, TO_CHAR);
    if (ch->pcdata->pk_timer > 0)
	{
	  sprintf(buf,
	  "%d weaves remaining.\n\r",
	  15 - ch->pcdata->portcounter);
	  send_to_char(buf, ch);
	}
}

void spell_nexus (int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *portal;
    ROOM_INDEX_DATA *to_room, *from_room;

    from_room = ch->in_room;

    if ((victim = get_char_world (ch, target_name)) == NULL
        || victim == ch
        || (to_room = victim->in_room) == NULL
        || (IS_SET (ch->in_room->room_flags, ROOM_NOPORT_FROM))
        || (IS_SET (ch->in_room->area->area_flags, AREA_NO_PORT_FROM))
        || (IS_SET(victim->in_room->area->area_flags, AREA_UNLINKED))
        || !can_see_room (ch, to_room) || !can_see_room (ch, from_room)
        || (!IS_NPC(victim) && ((IS_IMMORTAL(victim)) && (ch->level < victim->level)))  
		|| IS_SET(victim->in_room->room_flags, ROOM_NOPORT)
        || (IS_SET(victim->in_room->area->area_flags, AREA_NO_PORT))
        || victim->in_room->vnum == ROOM_VNUM_MORGUE )
    {
        send_to_char ("You failed.\n\r", ch);
        return;
    }

    /* portal one */
    portal = create_object (get_obj_index (OBJ_VNUM_PORTAL), 0);
    portal->timer = 1 + level / 10;
    portal->value[3] = to_room->vnum;

    obj_to_room (portal, from_room);

    act ("$p rises up from the ground.", ch, portal, NULL, TO_ROOM);
    act ("$p rises up before you.", ch, portal, NULL, TO_CHAR);

    /* no second portal if rooms are the same */
    if (to_room == from_room)
        return;

    /* portal two */
    portal = create_object (get_obj_index (OBJ_VNUM_PORTAL), 0);
    portal->timer = 1 + level / 10;
    portal->value[3] = from_room->vnum;

    obj_to_room (portal, to_room);

    if (to_room->people != NULL)
    {
        act ("$p rises up from the ground.", to_room->people, portal, NULL,
             TO_ROOM);
        act ("$p rises up from the ground.", to_room->people, portal, NULL,
             TO_CHAR);
    }
}

void spell_curse_obj (int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;

  if (number_percent () > get_skill(ch, sn))
  {
    send_to_char("You fail.\n\r", ch);
    return;
  }

  //obj->extra_flags += ITEM_NOREMOVE;
  if (IS_SET(obj->extra_flags,ITEM_NODROP))
  {
      send_to_char("It is already cursed.\n\r",ch);
      return;
  }
  obj->extra_flags += ITEM_NODROP;
  act("$p glows with an evil red aura.", ch, obj, NULL, TO_CHAR);
  act("$p glows with an evil red aura.", ch, obj, NULL, TO_ROOM);
  return;
}
void spell_iron_grip (int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;

  if (number_percent () > get_skill(ch, sn))
  {
    send_to_char("You fail.\n\r", ch);
    return;
  }
  if (IS_SET(obj->extra_flags,ITEM_NOREMOVE))
  {
      send_to_char("Its already enchanted with iron grip.\n\r",ch);
      return;
  }
  obj->extra_flags += ITEM_NOREMOVE;
  //obj->extra_flags += ITEM_NODROP;
  act("The $p's handle glows slightly.", ch, obj, NULL, TO_CHAR);
  act("The $p's handle glows slightly.", ch, obj, NULL, TO_ROOM);
  return;
}

void spell_fear(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED(victim, AFF_FEAR))
  {
    send_to_char("They are already scared half to death.\n\r", ch);
    return;
  }

  if (saves_spell(int(level * 1.2), victim, DAM_OTHER))
  {
     act("You fail to scare $M.\n\r", ch, victim, victim, TO_CHAR);
     return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.location = APPLY_NONE;
  af.duration = 0;
  af.bitvector = AFF_FEAR;
  af.modifier = 0;
  affect_to_char(victim, &af);
  act("$n scares you to death.", ch, victim, victim, TO_VICT);
  act("$n creates a weave of terrible images infront of $N", ch, victim, victim, TO_VICT);
  act("You send fear into the depths of $N soul.", ch, victim, victim, TO_CHAR); 
  return;
}

void spell_frighten(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED(victim, AFF_FRIGHTEN))
  {
    send_to_char("They are already frightened.\n\r", ch);
    return;
  }

  if (victim->fighting!=NULL)
  {
	  send_to_char("They are too concentrated on killing each other to notice your illusion.\n\r",ch);
	  return;
  }
  
  if (ch->fighting!=NULL)
  {
	  send_to_char("$n is too busy killing you to notice your illusion!.\n\r",ch);
	  return;
  }

  if (saves_spell(int(level * 2.6), victim, DAM_OTHER))
  {
     act("You fail to frighten $M.\n\r", ch, victim, victim, TO_CHAR);
     return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.location = APPLY_NONE;
  af.duration = level;
  af.bitvector = AFF_FRIGHTEN;
  af.modifier = 0;
  affect_to_char(victim, &af);
  act("$n frightens you.", ch, victim, victim, TO_VICT);
  act("$n frightens the living daylights out of $N", ch, victim, victim, TO_VICT);
  act("You frighten $N.", ch, victim, victim, TO_CHAR); 
  return;
}


/* Iceshield spell by Tamarae - (tamarae@zdnetonebox.com) */

void spell_iceshield( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

  if (victim != ch)
  {
	  send_to_char("Only you can properly handle such a shield.\n\r", ch);
	  return;
  }
   
   if (IS_AFFECTED(victim, AFF_ICESHIELD))
      {
         if(victim == ch)
			send_to_char("You are already surrounded by an icy shield.\n\r",ch);
         else
            act("$N is already surrounded by an icy shield.",ch,NULL,victim,
            TO_CHAR);
         return;
      }

 if ((IS_AFFECTED(victim, AFF_FIRESHIELD)))
   {
	   if (victim == ch)
		   send_to_char("You are already defensively shielded.\n\r", ch);
	   else
		   act("$N is already defensively shielded.",ch,NULL,victim,TO_CHAR);
	   return;
   }

   af.where	= TO_AFFECTS;
   af.type	= sn;
   af.level     = level;
   af.duration  = level/6;
   af.location  = APPLY_AC;
   af.modifier  = -40;
   af.bitvector = AFF_ICESHIELD;

   affect_to_char(victim, &af);
   act("$n is surrounded by an icy shield.",victim, NULL,NULL,TO_ROOM);
   send_to_char("You are surrounded by an icy shield.\n\r", victim);
   return;
}


/* Fireshield spell by Tamarae - (tamarae@zdnetonebox.com) */

void spell_fireshield(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

if (victim != ch)
  {
	  send_to_char("Only you can properly handle such a shield.\n\r", ch);
	  return;
  }

   if (IS_AFFECTED(victim, AFF_FIRESHIELD))
   {
      if (victim == ch)
         send_to_char("You are already surrounded by a firey shield.\r\n", ch);
      else
         act("$N is already surrounded by a fiery shield.",ch,NULL,victim,TO_CHAR);
      return;
   }
   
 if ((IS_AFFECTED (victim, AFF_ICESHIELD)))
   {
	   if (victim == ch)
		   send_to_char("You are already defensively shielded.\n\r", ch);
	   else
		   act("$N is already defensively shielded.",ch,NULL,victim,TO_CHAR);
	   return;
   }
   af.where 	= TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level/6;
   af.location  = APPLY_AC;
   af.modifier  = -40;
   af.bitvector = AFF_FIRESHIELD;

   affect_to_char(victim, &af);
   act("$n is surrounded by a fiery shield.",victim, NULL,NULL,TO_ROOM);
   send_to_char("You are surrounded by a fiery shield.\n\r", victim);  
   return;

}

/* Shockshield spell by Tamarae - (tamarae@zdnetonebox.com) */

void spell_shockshield(int sn, int level, CHAR_DATA *ch, void *vo, int target)     
{

   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

if (victim != ch)
  {
	  send_to_char("Only you can properly handle such a shield.\n\r", ch);
	  return;
  }

   if (IS_AFFECTED(victim, AFF_SHOCKSHIELD))
   {
     if (victim == ch)
         send_to_char("You are already surrounded in a crackling shield.\n\r", ch);
      else
         act("$N is already surrounded by a crackling shield.",ch, NULL, victim, TO_CHAR);
      return;
   }
	
   af.where 	= TO_AFFECTS;
   af.type	= sn;
   af.level     = level;
   af.duration  = level/6;
   af.location  = APPLY_AC;
   af.modifier  = -40;
   af.bitvector = AFF_SHOCKSHIELD;

   affect_to_char(victim, &af);
   act("$n is surrounded by a crackling shield.",victim, NULL,NULL, TO_ROOM);
   send_to_char("You are surrounded by a crackling field.\n\r",victim);
   return;
}

void spell_unravel(int sn, int level, CHAR_DATA *ch, void *vo, int target)     
{
  if (target == TARGET_NONE)
  {
    char *spellname;
    int spell;

    spellname = (char *) vo;
    if ((spell = skill_lookup(spellname)) == -1)
    {
      send_to_char("No such spell.\n\r", ch);
      return;
    }   

    if (!str_prefix(spellname, "bandaging"))
      return;

    if (!str_prefix(spellname, "restrain"))
      return;

    if (!str_prefix(spellname, "gouge"))
      return;

    if (!str_prefix(spellname, "ravage"))
      return;

    if (!str_prefix(spellname, "still"))
      return;

    if (!str_prefix(spellname, "dome"))
      return;

    if (!str_prefix(spellname, "daze"))
      return;

    if (!str_prefix(spellname, "cleanse"))
      return;

    if (!str_cmp(spellname, "tap"))
      return;

    if (!str_cmp(spellname, "trapset"))
      return;

    if (is_affected(ch, spell))
    { 
      if (check_dispel(int(level * 1.05), ch, spell))
        send_to_char("Ok.\n\r", ch);
      else
         send_to_char("You failed to unravel the weave.\n\r", ch);
    }
    else
      send_to_char("You aren't affected by that.\n\r", ch);
  }
  if (target == TARGET_OBJ)
  {
    OBJ_DATA *portal;
    OBJ_DATA *portal2;
    ROOM_INDEX_DATA *location = NULL;


    portal = (OBJ_DATA *) vo;

    if (portal->item_type != ITEM_PORTAL)
    {
      send_to_char("You don't know how to unravel that.\n\r", ch);
      return;
    }

    if (portal->value[3])
      location = get_room_index(portal->value[3]);

    if (location)
    {
      if ((portal2 = get_portal (ch, location->contents, portal->in_room->vnum)) != NULL)
      {
          obj_from_room(portal2);
          extract_obj(portal2);
          if (location->people)
          {
            act( "With a loud boom $p disappears.", location->people, portal2, NULL, TO_ROOM );
            act( "With a loud boom $p disappears.", location->people, portal2, NULL, TO_CHAR );
          }
      }
    }

    act("With a loud boom $p disappears.", ch, portal, NULL, TO_ROOM); 
    act("With a loud boom $p disappears.", ch, portal, NULL, TO_CHAR); 
    obj_from_room(portal);
    extract_obj(portal);      
  }
}
