/***************************************************
**                                                **
**      For random skills anything other than     **
**      guild skills.                             **
**	                                          **
****************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"
#include "bomb.h"

void raw_kill    args( (CHAR_DATA *ch,CHAR_DATA *victim) );
void group_gain  args( (CHAR_DATA *ch, CHAR_DATA *victim) ); 
void show_list_to_char args ((OBJ_DATA * list, CHAR_DATA * ch,
                              bool fShort, bool FShowNothing, sh_int type));
void show_char_to_char args ((CHAR_DATA *list, CHAR_DATA *ch));
char *flag_string       args(( const struct flag_type *flag_table, int bits));
OBJ_DATA *bomb_in_bag   args((OBJ_DATA *container));


void check_couple(CHAR_DATA *ch, sh_int cast, void *vo, int target)
{
  int i;
  bool found = FALSE;
  int mana;  
  int sn;

  if (IS_NPC(ch))
     return;
     
  for (i = 0; i < MAX_COUPLE; i++)
  {
    if (ch->pcdata->couple[i].cast == cast)
    {
      found = TRUE;
      break;
    }
  }

  if (!found)
  {
    return;
  }

  if (ch->pcdata->couple[i].cast == skill_lookup("gas breath"))
  {
    send_to_char("Illegal couple - Gas breath.\n\r", ch);
    return;
  }

  sn = ch->pcdata->couple[i].couple;
 
  mana = UMAX (skill_table[sn].min_mana,
                     100 / (2 + ch->level -
                            skill_table[sn].skill_level[ch->cClass]));
  mana /= 2;

  
  if (ch->mana < mana)
    return;

  if (number_bits(2) == 0) 
  {
    (*skill_table[sn].spell_fun) (sn, ch->level, ch, vo, target);
    ch->mana -= mana;
  }
}

void do_couple (CHAR_DATA *ch, char *argument)
{
  int i;
  char buf[MSL];
  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];
  int number, sn1, sn2;
  bool found = FALSE;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);

  if (get_skill(ch, gsn_couple) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (!str_cmp(arg1, "show"))
  {
    for (i = 0;i < MAX_COUPLE;i++)
    {
        sprintf(buf, "%d: %s couples %s.\n\r", i, 
           skill_table[ch->pcdata->couple[i].cast].name, 
           skill_table[ch->pcdata->couple[i].couple].name);
        send_to_char(buf, ch);
    }
    return;
  }

  if (!is_number(arg1))
  {
    send_to_char("Which couple to modify?\n\r", ch);send_to_char("Commands:\nCouple show                   -  shows a list of all current couples\nCouple <num> <spell> <spell>  -  creates a new couple\nCouple <num> clear            -  clears the given couple\n", ch);
    return;
  }

  number = atoi(arg1);

  if (!str_cmp("clear", arg2))
  {
    ch->pcdata->couple[number].cast = 0;
    ch->pcdata->couple[number].couple = 0;
    send_to_char("Couple cleared.\n\r", ch);
    return;
  } 

  if (arg2[0] == '\0' || arg3[0] == '\0')
  {
    send_to_char("Couple what with what?\n\r", ch);
    return;
  }

  sn1 = skill_lookup(arg2);

  if (sn1 == -1)
  {
    sprintf(buf, "%s is not a valid spell.\n\r", arg2);
    send_to_char(buf, ch);
    return;
  }

  if (skill_table[sn1].target != TAR_CHAR_OFFENSIVE)
  {
    send_to_char("You can't couple anything with that spell.\n\r", ch);
    return;
  }

  sn2 = skill_lookup(arg3);

  if (sn2 == -1)
  {
    sprintf(buf, "%s is not a valid spell.\n\r", arg3);
    send_to_char(buf, ch);
    return;
  }

  if (ch->pcdata->learned[sn1] < 1)
  {
    sprintf(buf, "You don't know %s.\n\r", skill_table[sn1].name);
    send_to_char(buf, ch);
    return;
  }  

  if (ch->pcdata->learned[sn2] < 1)
  {
    sprintf(buf, "You don't know %s.\n\r", skill_table[sn2].name);
    send_to_char(buf, ch);
    return;
  }  

  found = FALSE;

  for (i = 0; i < MAX_COUPLE_SPELLS;i++)
  {
    if (!str_prefix(arg3, couple_spells[i]))
    {
      found = TRUE;
      break;
    }
  }

  if (!found)
  {
    send_to_char("That can't be coupled, possible couplers are:\n\r   ", ch);
    for (i = 0; i < MAX_COUPLE_SPELLS;i++)
    {
      sprintf(buf, " %s", couple_spells[i]);
      send_to_char(buf, ch);
    }
    send_to_char("\n\r", ch);
    return;
  }

  ch->pcdata->couple[number].cast = sn1;
  ch->pcdata->couple[number].couple = sn2;
  send_to_char("Coupled.\n\r", ch);

}


void do_discharge (CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  
  if ((victim = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (victim->master != ch)
  {
    send_to_char("They aren't yours to discharge.\n\r", ch);
    return;
  }

  if (!IS_NPC(victim))
  {
    send_to_char("Not on a player!\n\r", ch);
    return;
  }

  act("With a swift kick in the but you send $N packing.", ch, NULL, victim, TO_CHAR);
  act("$n kicks $N in the butt and sends $M packing.", ch, NULL, victim, TO_ROOM); 
  extract_char(victim, TRUE);
}


void do_bandage (CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim;
	char arg [MAX_INPUT_LENGTH];
	int chance;
	int rndchance = number_range(0, 100);

	argument = one_argument(argument,arg);
	victim = get_char_room (ch, NULL, arg);
        if (IS_AFFECTED(ch, AFF_BANDAGE))
        {
          send_to_char("You aren't ready to do that right now.\n\r", ch);
          return;
        }

	if ( arg[0] == '\0' )
    {
	send_to_char("Bandage who?\n\r", ch );
	return;
    }

	if (victim == NULL)
    {
        send_to_char("They aren't here to be bandaged.\n\r", ch);
        return;
    }
	if (ch->fighting != NULL)
	{   
		send_to_char("You can't bandage in the midst of a battle!\n\r", ch);
		return;
	}

	if (victim->fighting != NULL)
	{   
		send_to_char("You can't bandage in the midst of a battle!\n\r", ch);
		return;
	}

	if (get_skill(ch, gsn_bandage) < 1)
	{
           send_to_char("You don't know where to begin.\n\r", ch);
           return;
	}
	if (IS_NPC(victim))
	{
		send_to_char("You can't bandage when there aren't any wounds!\n\r",ch);
		return;
	}
	
	chance = get_skill(ch,gsn_bandage)*3/4;
	chance += wis_app[get_curr_stat(ch,STAT_WIS)].practice;
	chance += (int_app[get_curr_stat(ch,STAT_INT)].learn)/6;
	chance += (dex_app[get_curr_stat(ch,STAT_DEX)].offensive)/2;
	
	WAIT_STATE (ch, skill_table[gsn_bandage].beats);
	WAIT_STATE (victim, skill_table[gsn_bandage].beats);

	if ( rndchance < chance)
        {
           AFFECT_DATA af;
           int sn;
           sn = skill_lookup("bandaging");
           af.where = TO_AFFECTS;
           af.type = sn;
           af.level = 20000;
           af.duration = number_range(2,5);
           af.location = APPLY_NONE;
           af.modifier = 0;
           af.bitvector = AFF_BANDAGE;
           affect_to_char(ch, &af);
           sn = skill_lookup("bandage");
           af.duration = number_range(2,3);
           af.type = sn;
           af.bitvector = AFF_REGENERATION;
           affect_to_char(victim, &af);
	  if (victim == ch)
	  {
             act( "You bandage yourself properly.", ch, NULL, victim, TO_CHAR);
	     act( "$n bandages $mself properly.", ch, NULL, victim, TO_NOTVICT);
	     victim->hit += number_range(1,40);   
	     {
		act ("$n's wounds close and bleeding stops..\n\r",victim,NULL,NULL,TO_NOTVICT);
		send_to_char("Your wounds close and stops your bleeding.\n\r",victim);
		SET_BLEED(victim, FALSE);
	     }
	  }
	  else
	  {
		act( "You bandage $N properly.", ch, NULL, victim, TO_CHAR);
		act( "$n bandages you properly.", ch, NULL, victim, TO_VICT);
		act( "$n bandages $N properly.", ch, NULL, victim, TO_NOTVICT);
		victim->hit += number_range(1,40);
		if (!IS_NPC(victim) && victim->pcdata->condition[COND_BLEEDING] == 1 )
	    {
		SET_BLEED(victim, FALSE);
		act ("$n's wounds close and bleeding stops..\n\r",victim,NULL,NULL,TO_NOTVICT);
		send_to_char("Their wounds close and stops your bleeding.\n\r",victim);
	    }
	    }
	    check_improve(ch,gsn_bandage,TRUE,3);
	    
	    return;
	}
    else
    {
        if (victim == ch)
	{
	    act( "You bandage yourself miserably.", ch, NULL, victim, TO_CHAR);
	    act( "$n bandages $mself miserably.", ch, NULL, victim, TO_NOTVICT);
	    SET_BLEED(victim, FALSE);
	}
	else
	{
	    act( "You failed to bandage $N.", ch, NULL, victim, TO_CHAR);
            act( "$n tried to bandage you but fails miserably.", ch, NULL, victim, TO_VICT);
	    act( "$n tried to bandage $N but fails miserably.", ch, NULL, victim, TO_NOTVICT);
        }
	check_improve(ch,gsn_bandage,FALSE,1);

    }
    return;
}

void do_peek( CHAR_DATA *ch, char *argument )
 
{
    CHAR_DATA *victim;
    int chance;
    char buf[MAX_STRING_LENGTH];
    bool PK = FALSE;

    if ((victim = get_char_room (ch, NULL, argument)) == NULL)
    {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }
    
    if (!IS_NPC(victim) && !IS_NPC(ch))
    {
      if (victim->pcdata->pk_timer >= 1 || ch->pcdata->pk_timer >= 1)
          PK = TRUE;
      if (victim->penalty.murder >= 1 || victim->penalty.thief >= 1)
          PK = TRUE;
    }

    if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) && !PK && !IS_IMMORTAL(ch))
    {
      send_to_char("Not in this room.\n\r", ch);
      return;
    }

    if (ch->fighting != NULL || victim->fighting != NULL)
    {
      send_to_char("Moving a little fast to peek, don't you think?\n\r", ch);
      return;
    }
    chance = (int_app[get_curr_stat (victim, STAT_INT)].learn)/10;
 
    if ( number_percent( ) < get_skill(ch,gsn_peek))
    {
        act( "You peek at $N's inventory:", ch, NULL, victim, TO_CHAR );
        check_improve(ch,gsn_peek,TRUE,4);
        show_list_to_char( victim->carrying, ch, TRUE, TRUE, 0 );
        sprintf(buf,"They are carrying %ld gold.\n\r", victim->gold);
        send_to_char(buf, ch);
        chance -= (dex_app[get_curr_stat (ch, STAT_DEX)].offensive)/4;
	chance += get_skill(victim,gsn_peek)/8;
        if ( chance > number_percent())
            act("$n peeks inside your inventory!", ch, NULL, victim, TO_VICT );
    }
    else
    {
        act( "You failed to peek inside $N's inventory!", ch, NULL, victim, TO_CHAR);
        act( "$n tried to peek inside your inventory!", ch, NULL, victim, TO_VICT);
        act( "$n tried to peek inside $N's inventory!", ch, NULL, victim, TO_NOTVICT);
        check_improve(ch,gsn_peek,FALSE,4);
    }
return;
}

void do_focus(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;

  if (get_skill(ch, gsn_focus) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if (IS_AFFECTED(ch, AFF_FOCUS))
  {
    send_to_char("You are as focused as you can get.\n\r", ch);
    return;
  }
 
  if (number_percent() < get_skill(ch, gsn_focus))
  {
    act("You focus your thoughts.", ch, NULL, NULL, TO_CHAR);
    af.where     = TO_AFFECTS;
    af.type      = gsn_focus;
    af.level     = ch->level;
    af.duration  = ch->level / 10;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_FOCUS;
    affect_to_char( ch, &af );
    check_improve(ch, gsn_focus, TRUE, 1);
  }
  else
  {
    send_to_char("You fail to focus your thoughts.\n\r", ch);
    check_improve(ch, gsn_focus, FALSE, 1);
  }
}

void do_dampen(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;

  if (get_skill(ch, gsn_dampen) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if (IS_AFFECTED(ch, AFF_DAMPEN))
  {
    send_to_char("You are already focusing on dampen.\n\r", ch);
    return;
  }
 
  if (number_percent() < get_skill(ch, gsn_dampen))
  {
    act("You throw up a spell dampening shield.", ch, NULL, NULL, TO_CHAR);
    af.where     = TO_AFFECTS;
    af.type      = gsn_dampen;
    af.level     = ch->level;
    af.duration  = ch->level / 10;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DAMPEN;
    affect_to_char( ch, &af );
    check_improve(ch, gsn_dampen, TRUE, 1);
  }
  else
  {
    send_to_char("You fail to dampen spells.\n\r", ch);
    check_improve(ch, gsn_dampen, FALSE, 1);
  }
}


void do_push_drag( CHAR_DATA *ch, char *argument, char *verb )
{
    char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    OBJ_DATA *obj;
    int door;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    
    obj = get_obj_list( ch, arg1, ch->in_room->contents );
	
	victim = get_char_room (ch, NULL,arg1);
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	sprintf( buf, "%s whom or what where?\n\r", capitalize(verb));
	send_to_char( buf, ch );
	return;
    }

    if ( (!victim || !can_see(ch,victim))
    && (!obj || !can_see_obj(ch,obj)) )
    {
	sprintf(buf,"%s whom or what where?\n\r", capitalize(verb));
	send_to_char( buf, ch );
        return;
    }
	
    if (IS_SET(ch->in_room->room_flags, ROOM_NOPUSH))
    {
        send_to_char("You cannot move them from this room!\n\r", ch);
        return;
    }	
    if (obj==NULL)
	{
		if (victim->fighting)
		{
			sprintf(buf,"They are fighting and you wouldn't want to get too close to %s",verb);
			send_to_char(buf,ch);
			return;
		}
	}

         if ( !str_cmp( arg2, "n" ) || !str_cmp( arg2, "north" ) ) door =
0;
    else if ( !str_cmp( arg2, "e" ) || !str_cmp( arg2, "east"  ) ) door =
1;
    else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "south" ) ) door =
2;
    else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "west"  ) ) door =
3;
    else if ( !str_cmp( arg2, "u" ) || !str_cmp( arg2, "up"    ) ) door =
4;
    else if ( !str_cmp( arg2, "d" ) || !str_cmp( arg2, "down"  ) ) door =
5;
    else
    {
      sprintf( buf, "Alas, you cannot %s in that direction.\n\r", verb );
      send_to_char( buf, ch );
      return;
    }

  if ( obj )
  {
    in_room = obj->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->u1.to_room   ) == NULL 
    ||	 !can_see_room(ch,pexit->u1.to_room))
    {
        sprintf( buf, "Alas, you cannot %s in that direction.\n\r", verb
);
        send_to_char( buf, ch );
	return;
    }

    if ( !IS_SET(obj->wear_flags, ITEM_TAKE) || !can_loot(ch,obj) )
    {
	send_to_char( "It won't budge.\n\r", ch );
	return;
    }

    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    || IS_SET(pexit->exit_info,EX_NOPASS) )
    {
	act( "You cannot $t it through the $d.", ch, verb, pexit->keyword,TO_CHAR );
	act( "$n decides to $t $P around!", ch, verb, obj, TO_ROOM );
	return;
    }

    act( "You attempt to $T $p out of the room.", ch, obj, verb, TO_CHAR);
    act( "$n is attempting to $T $p out of the room.", ch, obj, verb,TO_ROOM );

    if ( obj->weight >  can_carry_w (ch)) 
    {
      act( "$p is too heavy to $T.\n\r", ch, obj, verb, TO_CHAR);
      act( "$n attempts to $T $p, but it is too heavy.\n\r", ch, obj,verb, TO_ROOM);
      return;
    }
    if 	 ( !IS_IMMORTAL(ch)
    ||   IS_SET(ch->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    || 	 (number_percent() > 25) )
    {
	send_to_char( "It won't budge.\n\r", ch );
	return;
    }

    if ( ch->move > 10 )
    {
	ch->move -= 10;
	send_to_char( "You succeed!\n\r", ch );
	act( "$n succeeds!", ch, NULL, NULL, TO_ROOM );
	obj_from_room( obj );
	obj_to_room( obj, to_room );
	if (!str_cmp( verb, "drag" ))
	  move_char( ch, door, FALSE, FALSE );
    }
    else
    {
      sprintf( buf, "You are too tired to %s anything around!\n\r", verb);
      send_to_char( buf, ch );
    }
  }
  else
  {
    if ( ch == victim )
    {
	act( "You $t yourself about the room and look very silly.", ch,verb, NULL, TO_CHAR );
	act( "$n decides to be silly and $t $mself about the room.", ch,verb, NULL, TO_ROOM );
	return;
    }

    in_room = victim->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->u1.to_room   ) == NULL 
    ||	 !can_see_room(victim,pexit->u1.to_room))
    {
        sprintf( buf, "Alas, you cannot %s them that way.\n\r", verb );
        send_to_char( buf, ch );
	return;
    }

    if (IS_SET(pexit->exit_info, EX_CLOSED) &&  (!IS_AFFECTED(victim, AFF_PASS_DOOR) || IS_SET(pexit->exit_info,EX_NOPASS)))
    {
	act( "You try to $t them through the $d.", ch, verb,pexit->keyword, TO_CHAR );
	act( "$n decides to $t you around!", ch, verb, victim, TO_VICT );
	act( "$n decides to $t $N around!", ch, verb, victim, TO_NOTVICT);
	return;
    }

    act( "You attempt to $t $N out of the room.", ch, verb, victim,TO_CHAR );
    act( "$n is attempting to $t you out of the room!", ch, verb, victim,TO_VICT );
    act( "$n is attempting to $t $N out of the room.", ch, verb, victim,TO_NOTVICT );

    if   (!IS_IMMORTAL(ch)
    &&    IS_NPC(victim)  
    &&	  ( IS_SET(victim->act,ACT_TRAIN)
    ||	     IS_SET(victim->act,ACT_PRACTICE)
    ||	     IS_SET(victim->act,ACT_IS_HEALER)
    ||	     IS_SET(victim->act,ACT_IS_CHANGER)
    ||	     IS_SET(victim->imm_flags,IMM_SUMMON)
    ||	     victim->pIndexData->pShop )) 
    {
	send_to_char( "They won't budge.\n\r", ch );
	return;
    }

    if (!IS_IMMORTAL(ch)
    &&    (((victim->in_room == NULL)
    ||     (IS_AFFECTED(victim,AFF_FEAR))
    ||	   (!str_cmp( verb, "drag" ) && (victim->position >= POS_STANDING))
    ||     (!str_cmp( verb, "push" ) && (victim->position != POS_STANDING))
    || 	   is_safe(ch,victim)
    || 	   (get_carry_weight(ch) > can_carry_w(ch) &&  number_percent() < 95)
    ||     (number_percent() < (victim->size - ch->size) * 17))
    ||     (number_percent() < 10)  
    ||     (!IS_NPC(victim) && (victim->level >= LEVEL_IMMORTAL) )) )
    {
	send_to_char( "They won't budge.\n\r", ch );
	return;
    }



    if ( ch->move > 10 )
    {
	ch->move -= 10;
	send_to_char( "You succeed!\n\r", ch );
	act( "$n succeeds!", ch, NULL, NULL, TO_ROOM );
	if (!str_cmp( verb, "drag" ))
	  move_char( ch, door, FALSE, FALSE );
	move_char( victim, door, FALSE, FALSE );
    }
    else
    {
      sprintf( buf, "You are too tired to %s anybody around!\n\r", verb );
      send_to_char( buf, ch );
    }
  }

  return;
}
               
void do_push( CHAR_DATA *ch, char *argument )
{
    do_push_drag( ch, argument, "push" );
    return;
}

void do_drag( CHAR_DATA *ch, char *argument )
{
    do_push_drag( ch, argument, "drag" );
    return;
}

void do_lore( CHAR_DATA *ch, char *argument )

{ 
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    AFFECT_DATA *paf;


    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_lore].skill_level[ch->cClass] )
    {
        send_to_char(
            "You would like to what?\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "What would you like to know more about?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "You cannot lore that because you do not have that.\n\r", ch );
        return;
    }


    if ( number_percent( ) < get_skill(ch,gsn_lore))
    {
        send_to_char( "You learn more about this object:\n\r", ch );
        check_improve(ch,gsn_lore,TRUE,4);

        sprintf( buf, "It has the following wear flags: %s\n\rIt has the following characteristics: %s\n\r",
        wear_bit_name(obj->wear_flags), extra_bit_name( obj->extra_flags ) );
        send_to_char( buf, ch );

    /* now give out vital statistics as per identify */
    sprintf (buf,
             "Object '%s' is type %s, extra flags %s.\n\rWeight is %d, value is %d, level is %d.\n\r",
             obj->name,
             item_name (obj->item_type),
             extra_bit_name (obj->extra_flags),
             obj->weight / 10, obj->cost, obj->level);
    send_to_char (buf, ch);

    switch ( obj->item_type )
    {
        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
            sprintf( buf, "Level %d spells of:", obj->value[0] );
            send_to_char( buf, ch );

            if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
            {
                send_to_char( " '", ch );
                send_to_char( skill_table[obj->value[1]].name, ch );
                send_to_char( "'", ch );
            }

            if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
            {
                send_to_char( " '", ch );
                send_to_char( skill_table[obj->value[2]].name, ch );
                send_to_char( "'", ch );
            }

            if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
            {
                send_to_char( " '", ch );
                send_to_char( skill_table[obj->value[3]].name, ch );
                send_to_char( "'", ch );
            }

            if (obj->value[4] >= 0 && obj->value[4] < MAX_SKILL)
            {
                send_to_char(" '",ch);
                send_to_char(skill_table[obj->value[4]].name,ch);
                send_to_char("'",ch);
            }

            send_to_char( ".\n\r", ch );
        break;

        case ITEM_WAND:
        case ITEM_STAFF:
            sprintf( buf, "Has %d(%d) charges of level %d",
                obj->value[1], obj->value[2], obj->value[0] );
            send_to_char( buf, ch );

            if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
            {
                send_to_char( " '", ch );
                send_to_char( skill_table[obj->value[3]].name, ch );
                send_to_char( "'", ch );
            }

            send_to_char( ".\n\r", ch );
        break;

        case ITEM_DRINK_CON:
            sprintf(buf,"It holds %s-colored %s.\n\r",
                liq_table[obj->value[2]].liq_color,
                liq_table[obj->value[2]].liq_name);
            send_to_char(buf,ch);
            break;


        case ITEM_WEAPON:
            send_to_char("Weapon type is ",ch);
            switch (obj->value[0])
            {
                case(WEAPON_EXOTIC):
                    send_to_char("exotic\n\r",ch);
                    break;
                case(WEAPON_SWORD):
                    send_to_char("sword\n\r",ch);
                    break;
                case(WEAPON_DAGGER):
                    send_to_char("dagger\n\r",ch);
                    break;
                case(WEAPON_SPEAR):
                    send_to_char("spear\n\r",ch);
                    break;
                case(WEAPON_MACE):
                    send_to_char("mace/club\n\r",ch);
                    break;
                case(WEAPON_AXE):
                    send_to_char("axe\n\r",ch);
                    break;
                case(WEAPON_FLAIL):
                    send_to_char("flail\n\r",ch);
                    break;
                case(WEAPON_WHIP):
                    send_to_char("whip\n\r",ch);
                    break;
                case(WEAPON_POLEARM):
                    send_to_char("polearm\n\r",ch);
                    break;
                case(WEAPON_BOW):
                    send_to_char("bow\n\r",ch);
                    break;
                case(WEAPON_STAFF):
                    send_to_char("staff\n\r",ch);
                    break;
                default:
                    send_to_char("unknown\n\r",ch);
                    break;
            }
            if (obj->pIndexData->new_format)
                sprintf(buf,"Damage is %dd%d (average %d)\n\r",
                    obj->value[1],obj->value[2],
                    (1 + obj->value[2]) * obj->value[1] / 2);
            else
                sprintf( buf, "Damage is %d to %d (average %d)\n\r",
                    obj->value[1], obj->value[2],
                    ( obj->value[1] + obj->value[2] ) / 2 );
            send_to_char( buf, ch );

            sprintf(buf,"Damage noun is %s.\n\r",
                (obj->value[3] > 0 && obj->value[3] < MAX_DAMAGE_MESSAGE) ?
                    attack_table[obj->value[3]].noun : "undefined");
            send_to_char(buf,ch);

            if (obj->value[4])  /* weapon flags */
            {
                sprintf(buf,"Weapons flags: %s\n\r",
                    weapon_bit_name(obj->value[4]));
                send_to_char(buf,ch);
            }
        break;

        case ITEM_ARMOR:
            sprintf( buf,
            "Armor class is %d pierce, %d bash, %d slash, and %d magic\n\r",
                obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
            send_to_char( buf, ch );
        break;

        case ITEM_CONTAINER:
            sprintf(buf,"Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
                obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
            send_to_char(buf,ch);
            if (obj->value[4] != 100)
            {
                sprintf(buf,"Weight multiplier: %d%%\n\r",
                    obj->value[4]);
                send_to_char(buf,ch);
            }
        break;
    }


    if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
    {
        EXTRA_DESCR_DATA *ed;

        send_to_char( "Extra description keywords: '", ch );

        for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
        {
            send_to_char( ed->keyword, ch );
            if ( ed->next != NULL )
                send_to_char( " ", ch );
        }

        for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
        {
            send_to_char( ed->keyword, ch );
            if ( ed->next != NULL )
                send_to_char( " ", ch );
        }

        send_to_char( "'\n\r", ch );
    }


    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
        sprintf( buf, "Affects %s by %d, level %d",
            affect_loc_name( paf->location ), paf->modifier,paf->level );
        send_to_char(buf,ch);
        if ( paf->duration > -1)
            sprintf(buf,", %d hours.\n\r",paf->duration);
        else
            sprintf(buf,".\n\r");
        send_to_char( buf, ch );
        if (paf->bitvector)
        {
            switch(paf->where)
            {
                case TO_AFFECTS:
                    sprintf(buf,"Adds %s affect.\n",
                        affect_bit_name(paf->bitvector));
                    break;
                case TO_WEAPON:
                    sprintf(buf,"Adds %s weapon flags.\n",
                        weapon_bit_name(paf->bitvector));
                    break;
                case TO_OBJECT:
                    sprintf(buf,"Adds %s object flag.\n",
                        extra_bit_name(paf->bitvector));
                    break;
                case TO_IMMUNE:
                    sprintf(buf,"Adds immunity to %s.\n",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_RESIST:
                    sprintf(buf,"Adds resistance to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_VULN:
                    sprintf(buf,"Adds vulnerability to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                default:
                    sprintf(buf,"Unknown bit %d: %d\n\r",
                        paf->where,paf->bitvector);
                    break;
            }
            send_to_char(buf,ch);
        }
    }

    if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
        sprintf( buf, "Affects %s by %d, level %d.\n\r",
            affect_loc_name( paf->location ), paf->modifier,paf->level );
        send_to_char( buf, ch );
        if (paf->bitvector)
        {
            switch(paf->where)
            {
                case TO_AFFECTS:
                    sprintf(buf,"Adds %s affect.\n",
                        affect_bit_name(paf->bitvector));
                    break;
                case TO_OBJECT:
                    sprintf(buf,"Adds %s object flag.\n",
                        extra_bit_name(paf->bitvector));
                    break;
                case TO_IMMUNE:
                    sprintf(buf,"Adds immunity to %s.\n",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_RESIST:
                    sprintf(buf,"Adds resistance to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_VULN:
                    sprintf(buf,"Adds vulnerability to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                default:
                    sprintf(buf,"Unknown bit %d: %d\n\r",
                        paf->where,paf->bitvector);
                    break;
            }
            send_to_char(buf,ch);
        }
        WAIT_STATE(ch,skill_table[gsn_lore].beats);
    }
  }
    return;
}

void do_openbag(CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  OBJ_DATA *obj, *bomb;
  CHAR_DATA *victim;
  int chance;
  char buf[MSL]; 

  argument = one_argument(argument, arg1);
  
  if (get_skill(ch, gsn_openbag) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  } 

  if (arg1[0] == '\0' || argument[0] == '\0')
  {
    send_to_char("Open what bag on whom?\n\r", ch);
    return;
  }

  if ((victim = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (victim == ch)
  {
    send_to_char ("It's your own bag. No skill required here.\n\r",ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if ((obj = get_obj_carry(victim, arg1, ch)) == NULL)
  {
    send_to_char("You don't see that on them.\n\r", ch);
    return;
  }

  if (obj->item_type != ITEM_CONTAINER)
  {
    send_to_char("That's not a container.\n\r", ch);
    return;
  }
  if (!IS_SET(obj->value[1], CONT_CLOSED))
  {
    send_to_char("It's already open.\n\r", ch);
    return;
  }
  if (!IS_SET(obj->value[1], CONT_CLOSEABLE))
  {
    send_to_char("You can't do that.\n\r", ch);
    return;
  }
  if (IS_SET(obj->value[1], CONT_LOCKED))
  {
    send_to_char("It's locked.\n\r", ch);
    return;
  }

  if (can_see(victim, ch))
    chance = 15;
  else
    chance = 30;

  chance += (dex_app[get_curr_stat(ch, STAT_DEX)].offensive); 
  chance += get_skill(ch, gsn_openbag) / 3;
  chance -= (victim->level - ch->level) / 2;

  if (number_percent() > chance)
  {
    send_to_char("Oops.\n\r", ch); 
    affect_strip(ch, gsn_sneak);
    STR_REMOVE_BIT(ch->affected_by, AFF_SNEAK);
    sprintf(buf, "%s tried to open your bag.\n\r", can_see(victim, ch)? ch->name : "Someone");
    act(buf, ch, NULL, victim, TO_VICT);
    sprintf(buf, "%s tried to open $N's bag.\n\r", can_see(victim, ch)? ch->name : "Someone");
    act(buf, ch, NULL, victim, TO_NOTVICT);

    if (!IS_NPC(ch))
    { 
        check_improve(ch, gsn_openbag, FALSE, 2);
        multi_hit(victim, ch, TYPE_UNDEFINED);
    
      if (!IS_NPC(victim))
      {
        sprintf(buf, "$N tried to open %s's bag.", victim->name);
        wiznet(buf, ch, NULL, WIZ_FLAGS, 0, 0);
       }
    }
  }
  else
  {
    REMOVE_BIT(obj->value[1], CONT_CLOSED);
    if ((bomb = bomb_in_bag(obj)) != NULL)
    {
       act("$n set off $p in your bag!", ch, bomb, victim, TO_VICT);
       act("You set off $p in $N's bag!", ch, bomb, victim, TO_CHAR);
       act("$n set off $p in $N's bag!", ch, bomb, victim, TO_NOTVICT);
       (*explosive_table[bomb->value[1]].bomb_fun) (bomb, ch);
       fire_effect(obj, bomb->level, bomb->value[3], TARGET_OBJ);
       extract_obj(bomb);
       return;
    }
    act("You stealthily open $p.", ch, obj, victim, TO_CHAR);
    return;
  }
}

void do_steal_from_bag (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *container;
    int percent;
    int npercent;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);
    argument = one_argument (argument, arg3);

    if (get_skill(ch, gsn_steal_from_bag) < 1)
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
    {
        send_to_char ("Steal what from whom in what?\n\r", ch);
        return;
    }
    if ((victim = get_char_room (ch, NULL, arg2)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }
    
/*    
    if (victim->level >= MAX_LEVEL - 4)
    {
    	send_to_char ("Why would you want anything from an immortal?\n\r", ch);
    	return;
    }	
*/
    if (victim == ch)
    {
        send_to_char ("That's pointless.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

    if ((container = get_obj_carry(victim, arg3, ch)) == NULL)
    {
      send_to_char("They don't have a container like that.\n\r", ch);
      return;
    }
    
    if (container->item_type != ITEM_CONTAINER)
    {
      send_to_char("That's not a container.\n\r", ch);
      return;
    }
    if (IS_SET(container->value[1], CONT_CLOSED))
    {
      send_to_char("You can't steal out of something closed!\n\r", ch);
      return;
    }
  
  
    WAIT_STATE (ch, skill_table[gsn_steal_from_bag].beats);
    if (can_see (victim, ch))
        percent = 25;
    else
        percent = 50;
   
    percent += (get_skill (ch, gsn_steal_from_bag)/2);
    percent += int((dex_app[get_curr_stat(ch,STAT_DEX)].offensive)*1.5);
    percent += (int_app[get_curr_stat(ch,STAT_INT)].learn)/10;
    percent -= (int_app[get_curr_stat(victim,STAT_INT)].learn)/6;
    percent += (ch->level - victim->level);
    if (!str_cmp("assassin", class_table[ch->cClass].name))
       percent /= 2;
    if (!str_cmp("rogue", class_table[ch->cClass].name))
       percent += 10;
    if (!IS_AWAKE(victim))
       percent += 50;
    percent = URANGE(5,percent,95);
    npercent = number_range (1,100);
    sprintf(buf,"%d percent %d npercent\n\r",percent,npercent);
    if (IS_IMMORTAL(ch))
    send_to_char(buf,ch);
    
    if (!IS_NPC (ch) && (npercent > percent))
       {
        /*
         * Failure.
         */
        send_to_char ("Oops.\n\r", ch);
        affect_strip (ch, gsn_sneak);
        STR_REMOVE_BIT (ch->affected_by, AFF_SNEAK);
	      sprintf (buf,"%s tried to steal from you.\n\r", can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone");
        act (buf, ch, NULL, victim, TO_VICT);
        sprintf (buf,"%s tried to steal from $N.\n\r", can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone");
        act (buf, ch, NULL, victim, TO_NOTVICT);
        switch (number_range (0, 3))
        {
            case 0:
                sprintf (buf, "%s is a lousy thief!", can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone");
                break;
            case 1:
                sprintf (buf, "%s couldn't rob %s way out of a paper bag!",
                         can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone", (ch->sex == 2) ? "her" : "his");
                break;
            case 2:
                sprintf (buf, "%s tried to rob me!", can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone");
                break;
            case 3:
                sprintf (buf, "Keep your hands out of there, %s!", can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone");
                break;
        }
        if (!IS_AWAKE (victim))
            do_function (victim, &do_wake, "");
        if (IS_AWAKE (victim))
            do_function (victim, &do_yell, buf);
        if (!IS_NPC (ch))
        {
            if (IS_NPC (victim))
            {
                check_improve (ch, gsn_steal_from_bag, FALSE, 2);
                multi_hit (victim, ch, TYPE_UNDEFINED);
            }
            else
            {
                sprintf (buf, "$N tried to steal from %s.", victim->name);
                wiznet (buf, ch, NULL, WIZ_FLAGS, 0, 0);  
            }
        }

        return;
    }

    if ((obj = get_obj_list (victim, arg1, container->contains)) == NULL)
    {
        send_to_char ("You can't find it.\n\r", ch);
        return;
    }

    if (!can_drop_obj (ch, obj)
        || IS_SET (obj->extra_flags, ITEM_INVENTORY)
        || ((ch->level < 70)
            && ch->level <= obj->level))
    {
        send_to_char ("You can't pry it away.\n\r", ch);
        return;
    }

    if (ch->carry_number + get_obj_number (obj) > can_carry_n (ch))
    {
        send_to_char ("You have your hands full.\n\r", ch);
        return;
    }

    if (ch->carry_weight + get_obj_weight (obj) > can_carry_w (ch))
    {
        send_to_char ("You can't carry that much weight.\n\r", ch);
        return;
    }

    obj_from_obj(obj);
    obj_to_char (obj, victim);
    obj_from_char(obj);
    obj_to_char(obj, ch);
    act ("You pocket $p.", ch, obj, NULL, TO_CHAR);
    check_improve (ch, gsn_steal_from_bag, TRUE, 2);
    send_to_char ("Got it!\n\r", ch);
    return;
}

void do_peek_in( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *container;
    char arg1[MIL];
    char arg2[MIL];
    int chance;
    bool PK = FALSE;

    if (get_skill(ch, gsn_peek_in) < 1)
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
      send_to_char("Look in what on who?\n\r", ch);
      return;
    }

    if ((victim = get_char_room (ch, NULL, arg2)) == NULL)
    {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }
    if (victim == ch)
    {
        send_to_char ("It's your own bag. No skill required here.\n\r",ch);
        return;
    }
    
    if (!IS_NPC(victim) && !IS_NPC(ch))
    {
      if (victim->pcdata->pk_timer >= 1 || ch->pcdata->pk_timer >= 1)
          PK = TRUE;
      if (victim->penalty.murder >= 1 || victim->penalty.thief >= 1)
          PK = TRUE;
    }

    if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) && !PK && !IS_IMMORTAL(ch))
    {
      send_to_char("Not in this room.\n\r", ch);
      return;
    }

    if (ch->fighting != NULL || victim->fighting != NULL)
    {
      send_to_char("Moving a little fast to peek, don't you think?\n\r", ch);
      return;
    }

    if ((container = get_obj_carry(victim, arg1, ch)) == NULL)
    {
      send_to_char("They don't have anything like that.\n\r", ch);
      return;
    }

    chance = (int_app[get_curr_stat (victim, STAT_INT)].learn)/10;
 
    if ( number_percent( ) < get_skill(ch,gsn_peek))
    {
        act( "You peek in $p", ch, container, victim, TO_CHAR );
        check_improve(ch,gsn_peek_in,TRUE,4);
        show_list_to_char( container->contains, ch, TRUE, TRUE, 0 );
        chance -= (dex_app[get_curr_stat (ch, STAT_DEX)].offensive)/4;
	      chance += get_skill(victim,gsn_peek_in)/8;
        if ( chance > number_percent())
            act("$n peeks inside your inventory!", ch, NULL, victim, TO_VICT );
    }
    else
    {
        act( "You failed to peek inside $p!", ch, container, victim, TO_CHAR);
        act( "$n tried to peek inside $p!", ch, container, victim, TO_VICT);
        act( "$n tried to peek inside $N's bag!", ch, NULL, victim, TO_NOTVICT);
        check_improve(ch,gsn_peek_in,FALSE,4);
    }
return;
}

void do_disguise(CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  char arg2[MIL];

  OBJ_DATA *bomb;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("Disguise what as what?\n\r", ch);
    return;
  }

  if ((bomb = get_obj_carry(ch, arg1, ch)) == NULL)
  {
    send_to_char("You don't have anything like that.\n\r", ch);
    return;
  }

  if (bomb->item_type != ITEM_BOMB)
  {
    send_to_char("That's not a bomb, why would you try to disguise it?\n\r", ch);
    return;
  }

  if (!str_cmp(arg2, "coins"))
  {
     if (ch->silver < 100)
     {
       send_to_char("You need 100 silver coins to disguise the bomb.\n\r", ch);
       return;
     }
   
     ch->silver -= 100;

     free_string(bomb->name);
     bomb->name = str_dup("pile coins silver");
     free_string(bomb->short_descr);
     bomb->short_descr = str_dup("A pile of silver coins");
     free_string(bomb->description);
     bomb->description = str_dup("A pile of silver coins.");

     send_to_char("You have disguised your bomb as a pile of coins.\n\r", ch);
     return;
   }
}

void do_study(CHAR_DATA *ch, char *argument)
{
  int sn;
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;

  one_argument(argument, arg);
  
  if (get_skill(ch, gsn_study) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
    {
      send_to_char("What do you wish to study?", ch);
      return;
    }
   
  if ( (obj = get_obj_carry(ch, arg, ch)) == NULL)
    {
      send_to_char("You don't have that item.", ch);
      return;
    }
  
  if (obj->item_type != ITEM_BOOK)
    {
      send_to_char("You may only study books.", ch);
      return;
    }

  sn = obj->value[1];

  if ( ch->level < skill_table[sn].skill_level[ch->cClass])
  {
    send_to_char("You start to study, then realize it's beyond your ability.\n\r", ch);
    return;
  }

  if ( ch->pcdata->learned[sn])
    {
      send_to_char("After studying the scroll you realize it is already familiar to you.\n\r",ch);
      return;
    }

  ch->pcdata->learned[sn] = 75;
  act("You have mastered $t use it wisely.", ch, skill_table[sn].name, NULL, TO_CHAR);
  check_improve(ch, gsn_study, TRUE, 2);
  act( "$n crumples up $p and discards it on the ground.", ch, obj, NULL, TO_CHAR);
  extract_obj( obj );
  return;

}

void do_scry( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *to_room, *from_room;
    char buf[MSL];

    if (get_skill(ch, gsn_scry) < 1)
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if (number_percent() > get_skill(ch, gsn_scry))
    {
      send_to_char("You fail to scry them.\n\r", ch);
      check_improve(ch, gsn_scry, FALSE, 1);
      return;
    }

    from_room = ch->in_room;

    argument = one_argument( argument, arg );

    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    ||   victim == ch
    ||   (ch->level + 10 < victim->level )
    ||   (victim->level >= LEVEL_IMMORTAL)
    ||   (to_room = victim->in_room) == NULL
    ||   !can_see_room(ch,to_room)
    || !can_see_room(ch,from_room))
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    send_to_char("You scry out their position.\n\r", ch);
    send_to_char( "`%", ch);
    send_to_char( victim->in_room->name, ch );
        
    if ( (IS_IMMORTAL(ch) && (IS_NPC(ch) ||
           IS_SET(ch->act,PLR_HOLYLIGHT))) )
    {
      sprintf(buf,"`1 [`!Room %ld`1]`7", victim->in_room->vnum);
      send_to_char(buf,ch);
    }
    
   send_to_char( "\n\r", ch );
        
   if ( !IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF) )
   {
     send_to_char( "`7 ",ch);
     send_to_char( victim->in_room->description, ch );
   }
    
   if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT) )
   {
     send_to_char("\n\r",ch);
     port_exits(ch, victim->in_room);
   }
        
   show_list_to_char( victim->in_room->contents, ch, FALSE, FALSE, 0 );
   send_to_char( "`7", ch);
   show_char_to_char( victim->in_room->people,   ch );
   send_to_char("`7",ch);   
   check_improve(ch, gsn_scry, TRUE, 1);
}

