/***********************************
**				  **
**  For Guild Skills              **
**     				  **
************************************/

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
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "tournament.h"
bool check_dispel args (( int dis_level, CHAR_DATA *victim, int sn));

void do_skillreset (CHAR_DATA * ch, char *argument)
{
/*
    int sn;
    int col = 0;
    int lev = 0;
    int skill_lev;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char("Skill reset whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_world(ch, argument)) == NULL)
    {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }


    for ( lev = 1; lev < LEVEL_IMMORTAL; lev ++)
    {
        for ( sn = 0; sn < MAX_SKILL; sn ++)
        {
           skill_lev = skill_table[sn].skill_level[victim->cClass];

	   if ( victim->pcdata->learned[sn].skill_level[victim->cClass])
	 	   victim->pcdata->learned[sn] = 0;
        }
    }

    send_to_char("They have been resetted for their skills.\n\r", ch);
    send_to_char("You have been resetted for your skills.\n\r", victim);
*/
    return;
}

void do_skillstrip (CHAR_DATA * ch, char *argument)
{
    int sn, i;
    CHAR_DATA *victim;

    if ((victim = get_char_world(ch, argument)) == NULL)
    {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim->clan == 0)
    {
    	send_to_char ("They dont have a guild!\n\r", ch);
    	return;
    }
    
    for (i = 0 ; i < clan_table[victim->clan].top_gskill; i++)
    {
      if (clan_table[victim->clan].gskill[i] != NULL)
      {
        sn = skill_lookup(clan_table[victim->clan].gskill[i]);
        victim->pcdata->learned[sn] = 0;
      }
    }

    send_to_char("They have been stripped of their guild skills.\n\r", ch);
    send_to_char("You have been stripped of your guild skills.\n\r", victim);
    SET_BIT(victim->act2, PLR_NOSKILLS); 

    return;
}

   
void do_bestow(CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  char arg2[MIL];
  CHAR_DATA *victim;
  int i, sn;
  bool found = FALSE;
 
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);


  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("Bestow who what?\n\r", ch);
    return;
  }

  if (( victim = get_char_world(ch, arg1)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("Not on NPCs\n\r", ch);
    return;
}
  if (!str_cmp(ch->name, "Rand"))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (!is_clan(ch) && !IS_IMMORTAL(ch))
  {
    send_to_char("You aren't even guilded.\n\r", ch);
    return;
  }

  if (ch->rank < 4 && !IS_IMMORTAL(ch))
  {
    send_to_char("You don't have that ability.\n\r", ch);
    return;
  }

  if (ch->clan != victim->clan && !IS_IMMORTAL(ch))
  {
    send_to_char("But they aren't even in your guild.\n\r", ch);
    return;
  }

  for (i = 0 ; i < clan_table[victim->clan].top_gskill; i++)
  {
    if (!str_cmp(arg2, clan_table[victim->clan].gskill[i]))
    {
      found = TRUE;
      break;
    }
  }

   if (!found)
   {
     send_to_char("No such guild skill exists for that guild.\n\r", ch);
     return;
   }
 
   sn = skill_lookup(arg2); 
   victim->pcdata->learned[sn] = 75;
   send_to_char("Skill bestowed.\n\r", ch);
   return; 
}

void do_veil(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;
  int sn, chance;
  /*
  if (!str_cmp("warrior", class_table[ch->cClass].name) && 
     (!str_cmp("formmaster", class_table[ch->cClass].name) && 
     (!str_cmp("assassin", class_table[ch->cClass].name) && 
     (!str_cmp("rogue", class_table[ch->cClass].name)))))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
  */

  if (!str_cmp(ch->name, "Rand"))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if ( IS_SET (ch->in_room->room_flags, ROOM_SAFE) )
  {
    send_to_char("You can't veil in safe rooms.", ch);
    return;
  }

  if (!str_cmp(argument, "lower"))
  {
    if (!IS_AFFECTED(ch, AFF_VEIL))
    {
      send_to_char("It's already around your neck.\n\r", ch);
      return;
    }
 
    sn = skill_lookup("veil");
    affect_strip(ch, sn);
    send_to_char("You lower your veil.\n\r", ch);
    act("$n lowers $s veil.", ch, NULL, NULL, TO_ROOM);
    return;
  }    
  
  sn = skill_lookup("veil");
  chance = get_skill(ch, sn);

  if (IS_AFFECTED(ch, AFF_VEIL))
  {
   send_to_char("You cannot veil yourself again...\n\r",ch);
   return;
  }

  if (chance < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (ch->mana < 50)
  {
    send_to_char("You are not ready to veil.\n\r", ch);
    return;
  }
 
  if (number_percent() < chance)
  { 
    send_to_char("You wear a veil.\n\r", ch);	 
    act("$n dons a black veil.", ch, NULL, NULL, TO_ROOM);
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = ch->level;
    af.duration = 3;   
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_VEIL;
    affect_to_char(ch, &af);
    check_improve(ch, sn, TRUE, 1);
    ch->mana -= 50;
  }
  else
  {
    send_to_char("You failed to veil.\n\r", ch);
    check_improve(ch, sn, FALSE, 1);
  }
}

void do_dream(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  int sn, chance;
 
 if (ch->clan != clan_lookup("aiel") && !IS_IMMORTAL(ch) && 
    (ch->clan != clan_lookup("Shaido")))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  sn = skill_lookup("dream");
  chance = get_skill(ch, sn);

  if (chance < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

   if ( ch->position != POS_SLEEPING)
   {
     send_to_char("You need to be sleeping to dream.\n\r", ch);
     return;
   }


   
   if (number_percent() < chance)
   {
     
     if ((victim = get_char_world (ch, argument)) == NULL
        || victim == ch
        || victim->in_room == NULL
        || !can_see_room (ch, victim->in_room)
        || IS_SET (victim->in_room->room_flags, ROOM_PRIVATE)
        || IS_SET (victim->in_room->room_flags, ROOM_SOLITARY)
        || IS_SET (victim->in_room->room_flags, ROOM_NODREAM)
        || IS_SET (victim->in_room->area->area_flags, AREA_NO_DREAM)
        || (strstr(victim->in_room->area->builders, "Unlinked") != NULL)
        || (!IS_NPC(victim) && ((IS_IMMORTAL(victim)) && (ch->level < victim->level)))
        || victim->clan == clan_lookup("furies")
        || victim->in_room->vnum == ROOM_VNUM_MORGUE )  
      {
        ch->pcdata->dream_to_room = ch->in_room;
        ch->pcdata->dream = TRUE;
        send_to_char("You can't picture that place in your mind.\n\r", ch);
        return;
      }

      act("A black void fills your mind and you attempt to dream.", ch, NULL, NULL, TO_CHAR);
      ch->pcdata->dream_to_room = victim->in_room;
      ch->pcdata->dream = TRUE;
      check_improve(ch, sn, TRUE, 1);
      return;
   }
   else 
   {
     send_to_char("Your mind is too full from the days activities to picture that place.\n\r", ch);
      check_improve(ch, sn, FALSE, 1);
   }

 
}

bool furies_imm(CHAR_DATA *ch, CHAR_DATA *victim)
{
  if (tournament.status == TOURNAMENT_ON && tournament.noguild &&
      IS_SET(ch->act, PLR_TOURNEY))
        return TRUE;
  return FALSE;
}
void dream_to(CHAR_DATA *ch)
{
  char_from_room(ch);
  char_to_room(ch, ch->pcdata->dream_to_room);
  act("$n suddenly phases into existance.", ch, NULL, NULL, TO_ROOM);
  act_new("You dream of new places.", ch, NULL, NULL, TO_CHAR, POS_SLEEPING);
  ch->pcdata->dream = FALSE;
  ch->pcdata->dream_to_room = ch->in_room;
}

void do_ravage(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;
  CHAR_DATA *victim;
  int chance, sn, sn2, dam;

  if (ch->clan != clan_lookup("Seanchan") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  
  if ((victim = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  
  sn = skill_lookup("Ravage");
  chance = get_skill(ch, sn);
  
  if (chance < 1 )
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
  {
    return;
  }

  if (furies_imm(ch, victim))
  {
    send_to_char("They are immune to your guild skill.\n\r", ch);
    return;
  }

  if (ch->mana < 50)
  {
    send_to_char("Not enough mana.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(victim, AFF_RAVAGE))
  {
    send_to_char("They are already ravaged.\n\r", ch);
    return;
  }
  if (victim->clan == clan_lookup("furies"))
  {
    send_to_char("You cannot effeciently ravage this person.\n\r", ch);
    return;
  }

  if (number_percent() < chance)
  {
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = ch->level;
    af.duration = IS_SET(ch->act, PLR_TOURNEY) ? ch->level / 10 : ch->level / 6;  
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_RAVAGE;
    affect_to_char(victim, &af);
    act("You call on the powers of your ancestor and ravage $N.", ch, NULL, victim, TO_CHAR);
    act("$n ravages you.", ch, NULL, victim, TO_VICT);
    act("$n ravages $N.", ch, NULL, victim, TO_NOTVICT);  
    dam = ch->level * (number_percent() / 20) + 10;
    damage(ch, victim, dam, 1040, DAM_ENERGY, TRUE, FALSE); 
    if (number_percent() < 50)
    {
      sn2 = skill_lookup("Blind");
      af.where = TO_AFFECTS;
      af.type = sn2;
      af.level = ch->level;
      af.duration = 5;   
      af.modifier = 0;
      af.location = APPLY_NONE;
      af.bitvector = AFF_BLIND;
      affect_to_char(victim, &af);
      act("$N is blinded by your ravage.", ch, NULL, victim, TO_CHAR);
      act("$n's ravage blinds you.", ch, NULL, victim, TO_CHAR);
      
    }
    check_improve(ch, sn, TRUE, 1);
    ch->mana -= 50;
    
  }
  else
  {
    check_improve(ch, sn, FALSE, 1);
    send_to_char("You fail to ravage your victim.\n\r", ch);
    WAIT_STATE(ch, 12);
    return;
  }  
}

void do_warcry(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;

  if (ch->clan != clan_lookup("Redhand") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }


  if (get_skill(ch, gsn_warcry) < 1)
  {
    send_to_char("Huh?", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_WARCRY))
  {
    send_to_char("You are already ready for battle.\n\r", ch);
    return;
  }

  if (ch->mana < 50)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }

  if (number_percent() < get_skill(ch, gsn_warcry))
  {
    af.where     = TO_AFFECTS;
    af.type      = gsn_warcry;
    af.level     = ch->level;
    af.duration  = ch->level / 6;
    af.modifier  = ch->level / 6;
    af.location  = APPLY_DAMROLL;
    af.bitvector = AFF_WARCRY;
    affect_to_char( ch, &af );
    af.location  = APPLY_HITROLL;
    affect_to_char(ch, &af );
    af.modifier  = -(ch->level / 6);
    af.location  = APPLY_SAVES;
    affect_to_char(ch, &af );
    ch->mana -= 50;
    act("You yell a battle cry.", ch, NULL, NULL, TO_CHAR);
    act("$n screams loudly.", ch, NULL, NULL, TO_ROOM);
    check_improve(ch, gsn_warcry, TRUE, 1);

  }
  else
  {
    ch->mana -= 50;
    send_to_char("You voice goes hoarse.\n\r", ch);
    check_improve(ch, gsn_warcry, FALSE, 1);
    return;
  }    

}

void do_restrain(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  AFFECT_DATA af;

  if (!str_cmp("channeler", class_table[ch->cClass].name) ||
      (!str_cmp("cleric", class_table[ch->cClass].name)))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (ch->clan != clan_lookup("ashaman") && !IS_IMMORTAL(ch)&&!IS_DRAGON(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
 /* if ((ch->class != 0) && (ch->class != 1))
  {
    send_to_char("huh?\n\r", ch);
    return;
  } */
  if (get_skill(ch, gsn_restrain) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if ((victim = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (furies_imm(ch, victim))
  {
    send_to_char("They are immune to your guild skill.\n\r", ch);
    return;
  }

  if (ch == victim)
  {
    send_to_char("That'd be dumb\n\r", ch);
    return;
  }

  if (victim->timed_affect.seconds > 0)
  {
    send_to_char("They are already in a similar disposition.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(victim, AFF_WILLPOWER))
  {
    send_to_char("Their will is too strong for that right now.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
  {
    return;
  }
 
  if (IS_AFFECTED(ch, AFF_RESTRAIN2))
  {
    send_to_char("It's to early to attempt that again.\n\r", ch);
    return;
  }
  
  if (IS_AFFECTED(victim, AFF_BLACKJACK))
  {
    send_to_char("It would be unfair to restrain an unconscious person.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(victim, AFF_RESTRAIN))
  {
    send_to_char("They are already restrained.\n\r", ch);
    return;
  }
  
  if (ch->fighting)
  {
    send_to_char("You are too busy fighting.\n\r", ch);
    return;
  } 

  if (victim->fighting)
  {
    send_to_char("They are moving too much for you to restrain them.\n\r", ch);
    return;
  }

  if (victim->clan == clan_lookup("furies"))
  {
    send_to_char("You are unable to effeciently restrain this target.\n\r", ch);
    return;
  }
  
  if (number_percent() < get_skill(ch, gsn_restrain))
  {
    af.where     = TO_AFFECTS;
    af.type      = gsn_restrain;
    af.level     = ch->level;
    if (ch->level >= 60)
    {af.duration = 3;}
    af.duration = 3;    
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_RESTRAIN;
    affect_to_char( victim, &af );
    if (ch->level >= 60)
      victim->timed_affect.seconds = 50;
    else
      victim->timed_affect.seconds = 30; 
    victim->timed_affect.sn = gsn_restrain;
    af.bitvector   = AFF_RESTRAIN2;
    af.duration = 2; 
    affect_to_char(ch, &af );
    ch->mana -= 50;
    act("You restrain $N.", ch, NULL, victim, TO_CHAR);
    act("$n restrains $N.", ch, NULL, victim, TO_NOTVICT);
    act("$n restrain you!", ch, NULL, victim, TO_VICT);  
    check_improve(ch, gsn_restrain, TRUE, 1);
  }
  else
  {
    ch->mana -= 50;
    send_to_char("You failed.\n\r", ch);
    check_improve(ch, gsn_restrain, FALSE, 1);
  }

  damage(ch, victim, 0, 0, 0, FALSE, FALSE);
  WAIT_STATE (ch, PULSE_VIOLENCE);  
  return;
}

void do_manadrain(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  AFFECT_DATA af;
  
  if (!str_cmp("channeler", class_table[ch->cClass].name) ||
     (!str_cmp("cleric", class_table[ch->cClass].name)))
     {
       send_to_char("Huh?\n\r", ch);
       return;
     }

  if (ch->clan != clan_lookup("ashaman") && !IS_IMMORTAL(ch) && !IS_DRAGON(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if (get_skill(ch, gsn_manadrain) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if ((victim = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  /*
  if (!IS_SET(ch->level == LEVEL_HERO))
  {
    send_to_char("No way!.\n\r", ch);
    return;
  }*/

  if (ch == victim)
  {
    send_to_char("That'd be dumb\n\r", ch);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("Not on NPCs.\n\r", ch);
    return;
  }
 
  if (is_safe(ch, victim))
  {
    return;
  }
 
  if (furies_imm(ch, victim))
  {
    send_to_char("They are immune to your guild skill.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_MANADRAIN2))
  {
    send_to_char("It's to early to attempt that again.\n\r", ch);
    return;
  }
  
  if (IS_AFFECTED(victim, AFF_MANADRAIN))
  {
    send_to_char("They are already tapped.\n\r", ch);
    return;
  }
  
  if (ch->fighting)
  {
    send_to_char("You are too busy fighting.\n\r", ch);
    return;
  } 
  
  if (victim->clan == clan_lookup("furies"))
    {
      send_to_char("You are unable to tap into this person's mana pool.\n\r", ch);
      return;
    }
  if (number_percent() < get_skill(ch, gsn_manadrain))
  {
    af.where     = TO_AFFECTS;
    af.type      = gsn_manadrain;
    af.level     = ch->level;
    af.duration = ch->level / 10;    
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_MANADRAIN;
    affect_to_char( victim, &af );
    af.bitvector   = AFF_MANADRAIN2;
    af.duration = 5; 
    affect_to_char(ch, &af );
    ch->mana -= 30;
    ch->pcdata->manadrainee = str_dup(victim->name);
    act("You tap into $N's mana pool.", ch, NULL, victim, TO_CHAR);
    act("$n taps into $N's mana pool.", ch, NULL, victim, TO_NOTVICT);
    act("$n taps your mana pool you feel $s draining your mana!", ch, NULL, victim, TO_VICT);  
    check_improve(ch, gsn_manadrain, TRUE, 1);
  }
  else
  {
    ch->mana -= 50;
    send_to_char("You failed.\n\r", ch);
    check_improve(ch, gsn_manadrain, FALSE, 1);
  }
  WAIT_STATE (ch, PULSE_VIOLENCE);  
  return;
}


void do_firestorm(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;
  
  if (ch->clan != clan_lookup("ashaman") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if (IS_SET(ch->act, PLR_TOURNEY) && tournament.noguild)
  {
    send_to_char("Not while in this tournament.\n\r", ch);
    return;
  }

  if (get_skill(ch, gsn_firestorm) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_FIRESTORM))
  {
    send_to_char("You are doing the best you can.\n\r", ch);
    return;
  }
  
  if (get_skill(ch, gsn_firestorm) > number_percent())
  {
    send_to_char("You bow your head in concentration.\n\r", ch);
    af.where     = TO_AFFECTS;
    af.type      = gsn_firestorm;
    af.level     = ch->level;
    af.duration  = ch->level / 10;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_FIRESTORM;
    affect_to_char( ch, &af );
    check_improve(ch, gsn_firestorm, TRUE, 1);
  }
  else
  {
    send_to_char("You fail to concentrate.\n\r", ch);
    check_improve(ch, gsn_firestorm, FALSE, 1);
  }
  
  return;
}

void do_relax(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;

  if (ch->clan != clan_lookup("ashaman") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (!IS_AFFECTED(ch, AFF_FIRESTORM))
  {
    send_to_char("Your mind is already at ease.\n\r", ch);
    return;
  }

  for (paf = ch->affected; paf != NULL; paf = paf_next)
  {
    paf_next = paf->next;
    if (paf->type == skill_lookup("Firestorm") && paf->duration > 0)
    {
     
      paf->duration = 0;
      send_to_char("You attempt to put your mind at rest.\n\r", ch);
      break;
     }
   }
}

void do_still(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  AFFECT_DATA af;
  
  if (ch->sex != 2)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (ch->clan != clan_lookup("aessedai") && ch->clan != clan_lookup("Rebel") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if ((victim = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("They aren't here.", ch);
    return;
  }
  
  if (victim->fighting)
  {
    send_to_char("You can't seem to still a fighting person.\n\r", ch);
    return;
  }

  if (furies_imm(ch, victim))
  {
    send_to_char("They are immune to your guild skill.\n\r", ch);
    return;
  }

  
  if (victim == ch)
  {
    send_to_char("What would be the point of that?\n\r", ch);
    return;
  }

  if (IS_AFFECTED(victim, AFF_BLACKJACK))
  {
    send_to_char("You can't find the strength to still an unconscious person.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(victim, AFF_STILL))
  {
    send_to_char("They have already been cut from the One Source.\n\r", ch);
    return;
  }
  
  if (victim->timed_affect.seconds > 0)
  {
    send_to_char("They are already in a similar disposition.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(victim, AFF_WILLPOWER))
  {
    send_to_char("Their will is too strong for that right now.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
  {
    return;
  }
  
  if (get_skill(ch, gsn_still) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (victim->clan == clan_lookup("furies") && !IS_NPC(victim))
  {
    send_to_char ("You are unable to still this person.\n\r",ch);
    return;
  }

  if (number_percent() < get_skill(ch, gsn_still))
  {
    act("You still $N.", ch, NULL, victim, TO_CHAR);
    act("You are stilled!", ch, NULL, victim, TO_VICT);
    af.where     = TO_AFFECTS;
    af.type      = gsn_still;
    af.level     = ch->level;
    af.duration  = 5;
    if (ch->level >= 60)
      victim->timed_affect.seconds = 100;
    else
      victim->timed_affect.seconds = 70;
    victim->timed_affect.sn = gsn_still;     
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_STILL;
    affect_to_char( victim, &af );
    check_improve(ch, gsn_still, TRUE, 1);
  }
  else
  {
    send_to_char("You fail to still them.\n\r", ch);
    check_improve(ch, gsn_still, FALSE, 1);
  }
  damage(ch, victim, 0, 0, 0, FALSE, FALSE);

}
    
void do_ballad(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *fch, *fch_next;
  AFFECT_DATA af;
  int result_mana, result_hit, result_move;
  int factor;

  if (ch->clan != clan_lookup("Gleeman") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_SET(ch->act, PLR_TOURNEY) && tournament.noguild)
  {
    send_to_char("Not while in this tournament.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_BALLAD))
  {
    send_to_char("You aren't prepared to perform again.\n\r", ch);
    return;
  }
  
  if (ch->mana < (ch->max_mana / 40))
  {
    send_to_char("Perhaps you should rest some before attempting to perform.\n\r", ch);
    return;
  }
  
  if (number_percent() < get_skill(ch, gsn_ballad))
  {
    act("You perform a beautiful ballad.", ch, NULL, NULL, TO_CHAR);
    act("$n performs a ballad.", ch, NULL, NULL, TO_ROOM);
    ch->mana -= ch->max_mana / 40;
    for (fch = ch->in_room->people; fch != NULL; fch = fch_next)
    {
      fch_next = fch->next_in_room;
      if (fch == ch) 
        factor = 5;
      else
        factor = 10;
      result_hit = fch->hit + (fch->max_hit / factor);
      result_mana = fch->mana + (fch->max_mana / factor);
      result_move = fch->move + (fch->max_move / factor);
      

       if (fch->clan == clan_lookup("furies"))
      {
        fch->move = fch->move;
        fch->mana = fch->mana;
        fch->hit = fch->hit;  
}
 

     if (result_hit > fch->max_hit)
               {
        fch->hit = fch->max_hit;
      }
      else
      {
        fch->hit = result_hit;
      }
      
      if (result_mana > fch->max_mana)
      {
        fch->mana = fch->max_mana;
      }
      
      else
      {
        fch->mana = result_mana;
      }
      
      if (result_move > fch->max_move)
      {
        fch->move = fch->max_move;
      }
      else
      {
        fch->move = result_move;
      }
      
      act("You feel better.", ch, NULL, fch, TO_VICT);
    }
    af.where     = TO_AFFECTS;
    af.type      = gsn_ballad;
    af.level     = ch->level;
    af.duration  = 4;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_BALLAD;
    affect_to_char( ch, &af );
    check_improve(ch, gsn_ballad, TRUE, 1);
  }
  else
  {
    send_to_char("You get stage fright.\n\r", ch);
    ch->mana -= (ch->max_mana / 40);
    check_improve(ch, gsn_ballad, FALSE, 1);
    return;   
  }
}

void dome(CHAR_DATA *ch, CHAR_DATA *victim)
{
  AFFECT_DATA af;
  int dam;

  if (ch->clan != clan_lookup("Gleeman") && !IS_IMMORTAL(ch))
  {
    return;
  }
  
  if (is_safe(ch, victim))
  {
    return;
  }
  
  if (furies_imm(ch, victim))
  {
    return;
  }

  
  if (victim->clan == clan_lookup("furies"))
  {
    send_to_char ("You cannot effeciently dome this person.\n\r", ch);
    return;
  }
  dam = ch->level * (number_percent() / 30) + 10;
  damage(ch, victim, dam, 1042, DAM_ENERGY, TRUE, FALSE); 
  
  if (get_skill(ch, gsn_dome) > number_percent())
  {
  	if (!IS_AFFECTED(victim,AFF_DOME))
  {
    af.where     = TO_AFFECTS;
    af.type      = gsn_dome;
    af.level     = ch->level;
    af.duration  = 1;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DOME;
    affect_to_char( victim, &af );
  }
    check_improve(ch, gsn_dome, TRUE, 1);
    act("$N looks very disoriented.", ch, NULL, victim, TO_NOTVICT);
    act("$N looks very disoriented.", ch, NULL, victim, TO_CHAR);
    act("You suddenly feel very disoriented.", ch, NULL, victim, TO_VICT);

  }

}

void do_daze(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  AFFECT_DATA af;
  
  if (ch->clan != clan_lookup("Gleeman") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if ((victim = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("They aren't here.", ch);
    return;
  }
  
  if (victim == ch)
  {
    send_to_char("What would be the point of that?\n\r", ch);
    return;
  }
  
  if (IS_AFFECTED(victim, AFF_DAZE))
  {
    send_to_char("They are already dazed.\n\r", ch);
    return;
  }
  
  if (is_safe(ch, victim))
  {
    return;
  }
  
  if (furies_imm(ch, victim))
  {
    send_to_char("They are immune to your guild skill.\n\r", ch);
    return;
  }

  if (get_skill(ch, gsn_daze) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (victim->clan == clan_lookup("furies"))
  {
    send_to_char("You cannot effeciently daze this person.\n\r", ch);
    return;
  }
  if (number_percent() < get_skill(ch, gsn_daze))
  {
    int i, armor;

    act("You daze $N.", ch, NULL, victim, TO_CHAR);
    act("You feel very weak and dazed.", ch, NULL, victim, TO_VICT);
    af.where     = TO_AFFECTS;
    af.type      = gsn_daze;
    af.level     = ch->level;
    af.duration  = ch->level / 20;
    af.bitvector = AFF_DAZE;
    af.location  = APPLY_NONE;
  
    af.location = APPLY_SAVES;
    af.modifier = abs((victim->saving_throw / 5));
    affect_to_char( victim, &af );

    af.location = APPLY_HITROLL;
    af.modifier = - (victim->hitroll / 5);
    affect_to_char( victim, &af );

    armor = 1;
    for (i = 0; i < 4; i++)
      armor += abs(victim->armor[i]);
    armor /= 4;

    af.location = APPLY_AC;
    af.modifier = armor / 5;
    affect_to_char( victim, &af );
  
    check_improve(ch, gsn_daze, TRUE, 1);
  }
  else
  {
    send_to_char("You fail to daze them.\n\r", ch);
    check_improve(ch, gsn_daze, FALSE, 1);
  }
  damage(ch, victim, 0, 0, 0, FALSE, FALSE);

}


void do_stance(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;

  if (ch->clan != clan_lookup("whitecloak") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (get_skill(ch, gsn_stance) < 1)
  {
    send_to_char("Huh?", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_STANCE))
  {
    send_to_char("You are already ready for battle.\n\r", ch);
    return;
  }

  if (IS_SET(ch->act, PLR_TOURNEY) && tournament.noguild)
  {
    send_to_char("Not while in this tournament.\n\r", ch);
    return;
  }

  if (ch->mana < 25)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }

  if (number_percent() < get_skill(ch, gsn_stance))
  {
    af.where     = TO_AFFECTS;
    af.type      = gsn_stance;
    af.level     = ch->level;
    af.duration  = ch->level / 10;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_STANCE;
    affect_to_char( ch, &af );
    ch->mana -= 25;
    act("You drop into a battle stance.", ch, NULL, NULL, TO_CHAR);
    act("$n drops into a battle stance.", ch, NULL, NULL, TO_ROOM);
    check_improve(ch, gsn_stance, TRUE, 1);

  }
  else
  {
    ch->mana -= 25;
    send_to_char("You miss your manuever.\n\r", ch);
    check_improve(ch, gsn_stance, FALSE, 1);
    return;
  }    

}

void do_cleanse (CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  AFFECT_DATA af;
  int dam;
  int chance;

  if (ch->clan != clan_lookup("Whitecloak") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if (get_skill(ch, gsn_cleanse) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if ((victim = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (victim == ch)
  {
    send_to_char("Why would you want to do that to yourself?\n\r", ch);
    return;
  }
  
  if (is_safe(ch, victim))
  {
    return;
  }
  
  if (furies_imm(ch, victim))
  {
    send_to_char("They are immune to your guild skill.\n\r", ch);
    return;
  }

  if (get_skill(ch, gsn_cleanse) > number_percent())
  {
    if (victim->clan != clan_lookup("Shadow") && !IS_MINION(victim))
      dam = ch->level * (number_percent() / 35);
    else
      dam = ch->level * (number_percent() / 22);    
   damage(ch, victim, dam, 1043, DAM_ENERGY, TRUE, FALSE); 
    if (!IS_AFFECTED(victim,AFF_CLEANSE))
   {
    af.where     = TO_AFFECTS;
    af.type      = gsn_cleanse;
    af.level     = ch->level;
    af.duration  = 2;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_CLEANSE;
    affect_to_char( victim, &af );
 
  chance = ((number_range(1, 100) - (ch->level - 100)) + (get_skill(ch, gsn_cleanse) / 2));
   
  if (chance >= 80)
{
  if (IS_AFFECTED (victim, AFF_SANCTUARY))
  {
  
      if (check_dispel (ch->level, victim, gsn_sanctuary))
         {
            send_to_char ("`&You have `#vanquished`& thier protection.`7\n\r", ch);
            send_to_char ("You feel less protected.\n\r", victim);
         }
  } 
  if (IS_AFFECTED (victim, AFF_IRONSKIN))
  {

      if (check_dispel (ch->level, victim, gsn_marironskin))
         {
            send_to_char ("`&You have `#vanquished`& thier protection.`7\n\r", ch);
            send_to_char ("You feel less protected.\n\r", victim);
         }
  }
}
}
    check_improve(ch, gsn_cleanse, TRUE, 1);
  }
  else
  {
    send_to_char("You fail.\n\r", ch);
    check_improve(ch, gsn_cleanse, FALSE, 1);
    return;
  }
}

void do_pray(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;

  if (ch->clan != clan_lookup("Shadow") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if (get_skill(ch, gsn_pray) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if (IS_SET(ch->act, PLR_TOURNEY) && tournament.noguild)
  {
    send_to_char("Not while in this tournament.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_PRAY))
  {
    send_to_char("You already have the protection of your master.\n\r", ch);
    return;
  }
  
  if (number_percent() < get_skill(ch, gsn_pray))
  {
    af.where     = TO_AFFECTS;
    af.type      = gsn_pray;
    af.level     = ch->level;
    af.duration  = ch->level / 10;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_PRAY;
    affect_to_char( ch, &af );
    act("You pray to your Master for protection, and he grants it.", ch, NULL, NULL, TO_CHAR);
    act("$n kneels and says a prayer in a tongue you do not understand.", ch, NULL, NULL, TO_ROOM);
    check_improve(ch, gsn_pray, TRUE, 1);
  }
  else
  {
    act("You pray to your Master for protection, and he laughs at you.", ch, NULL, NULL, TO_CHAR);
    act("$n kneels and says a prayer in a tongue you do not understand.", ch, NULL, NULL, TO_ROOM);
    check_improve(ch, gsn_pray, FALSE, 1);
  }
}

void do_shroud(CHAR_DATA *ch, char *argument)
{
AFFECT_DATA af;
  if (IS_NPC(ch))
  {
    send_to_char("This is a PC only skill.\n\r",ch);
    return;
  }
 
  if (ch->clan != clan_lookup("Shadow") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if ( IS_SET (ch->in_room->room_flags, ROOM_SAFE) )
  {
    send_to_char("You can't shroud in safe rooms.", ch);
    return;
  }
        
  if (IS_SET(ch->act, PLR_TOURNEY) && tournament.noguild)
  {
    send_to_char("Not while in this tournament.\n\r", ch);
    return;
  }

  if ((ch->penalty.murder > 0) || ch->penalty.thief > 0)
  {
    send_to_char("Not while you're a murderer.\n\r", ch);
    return;
  }

  send_to_char ("You attempt to shroud yourself in darkness.\n\r", ch);
  affect_strip (ch, gsn_shroud);
 
  if (IS_AFFECTED (ch, AFF_SHROUD))
    return;
  if (ch->pcdata->pk_timer >= 1)
  {
  	send_to_char ("You fail to grasp enough concentration to attempt this now.\n\r", ch);
  	return;
  }
  if (number_percent () < get_skill (ch, gsn_shroud))
  {
  	if (ch->move < 100)
  	{
  	    send_to_char("You don't have the energy to shroud yourself properly.\n\r",ch);
  	    return;
  	}
      	check_improve (ch, gsn_shroud, TRUE, 1);
        af.where = TO_AFFECTS;
        af.type = gsn_shroud;
        af.level = ch->level;
        af.duration = 6;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_SHROUD;
        affect_to_char (ch, &af);
        ch->move = ch->move - 100;
  }
  else
    check_improve (ch, gsn_shroud, FALSE, 1);
    
  return; 
}

void do_stealth(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;

  if (ch->clan != clan_lookup("Shadow") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
        
  if (IS_SET(ch->act, PLR_TOURNEY) && tournament.noguild)
  {
    send_to_char("Not while in this tournament.\n\r", ch);
    return;
  }

  send_to_char ("You attempt to move with the furtiveness of the Dark One.\n\r", ch);
  affect_strip (ch, gsn_stealth);
 
  if (IS_AFFECTED (ch, AFF_STEALTH))
    return;
    
  if (number_percent () < get_skill (ch, gsn_stealth))
  {
        check_improve (ch, gsn_stealth, TRUE, 1);
        af.where = TO_AFFECTS;
        af.type = gsn_stealth;
        af.level = ch->level;
        af.duration = ch->level;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_STEALTH;
        affect_to_char (ch, &af);
  }
  else
    check_improve (ch, gsn_stealth, FALSE, 1);
    
  return;
}

void do_wolf(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *wolf, *gch;
  MOB_INDEX_DATA *pMobIndex;
  AFFECT_DATA af;
  int sn;

  if (ch->clan != clan_lookup("wolfbrother") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if (get_skill(ch, gsn_wolf) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if (IS_SET(ch->act, PLR_TOURNEY) && tournament.noguild)
  {
    send_to_char("Not while in this tournament.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_WOLF))
  {
    send_to_char("It's to early to call another wolf.\n\r", ch);
    return;
  }
  
  for (gch = char_list; gch != NULL; gch = gch->next)
  {
     if (is_same_group (gch, ch))
     {
       if (IS_NPC(gch))
       {
         if (gch->pIndexData == get_mob_index( MOB_VNUM_GWOLF ))
         {
           send_to_char("You already have a wolf.\n\r", ch);
           return;
         }
       }
     }
  }
  
  if (number_percent() < get_skill(ch, gsn_wolf))
  {
    af.where = TO_AFFECTS;
    af.type = gsn_wolf;
    af.level = ch->level;
    af.duration = 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_WOLF;
    affect_to_char (ch, &af);
    
    pMobIndex = get_mob_index( MOB_VNUM_GWOLF );
    if ((wolf = create_mobile( pMobIndex )) == NULL)
      return;
     char_to_room( wolf, ch->in_room );
    add_follower( wolf, ch);
    wolf->leader = ch;
    wolf->master = ch;
    wolf->max_hit = int(ch->max_hit * .8);
    wolf->hit = wolf->max_hit;
    wolf->level = int(ch->level * .8);
    wolf->hitroll = int(ch->hitroll * .8);
    wolf->damroll = int(ch->damroll * .8);
    ch->pet = wolf; 
    sn = skill_lookup("Charm");
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = 200;
    af.duration = -1;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char (wolf, &af);
    
    act( "$n has called a wolf to $s side.\n\r", ch, NULL, NULL, TO_ROOM);   
    send_to_char( "You have called upon a wolf.\n\r", ch);
    check_improve(ch, gsn_wolf, FALSE, 1);
  }
  else
  {
    send_to_char("You fail to call a wolf.\n\r", ch);
    check_improve(ch, gsn_wolf, FALSE, 1);
  }
}

void do_fix(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int chance;

  if (ch->clan != clan_lookup("Tinker") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  argument = one_argument(argument, arg);
  
  if (get_skill(ch, gsn_fix) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if ((obj = get_obj_carry(ch, arg, ch)) == NULL)  
  {
    if ((obj = get_obj_list(ch, arg, ch->in_room->contents)) == NULL)
    {
      send_to_char("You don't see that anywhere.", ch);
      return;
    }
  }
  
  if (obj->condition > 90)
  {
    send_to_char("It's still in good shape, no repairs necessary.", ch);
    return;
  }
  
  if (obj->condition < 0)
  {
    send_to_char("It's totally destroyed, you can't fix that.\n\r", ch);
    return;
  }

  chance = get_skill(ch, gsn_fix);
  if (obj->condition < 50)
    chance -= 5;
  if (obj->condition < 20)
    chance -= 2;
  chance -= (obj->level - ch->level) / 5;
  
  if (IS_IMMORTAL(ch))
    chance = 100;
  
  if (number_percent() < chance)
  {
    act("You fix $p.", ch, obj, NULL, TO_CHAR);
    act("$n fixes $p.", ch, obj, NULL, TO_ROOM);
    obj->condition = 100;
    check_improve(ch, gsn_fix, TRUE, 1);
  }
  else
  {
    chance -= 10;
    if (number_percent() < chance)
    {
      act("While attempting to fix $p you damage it further.", ch, obj, NULL, TO_CHAR);
      act("While attempting to fix $p $n damages it further.", ch, obj, NULL, TO_ROOM);
      obj->condition = 0;
    }
    else
    {
      act("While attemtping to fix $p you destroy it.", ch, obj, NULL, TO_CHAR);
      act("While attemtping to fix $p $n destroys it.", ch, obj, NULL, TO_ROOM);
      obj->condition = -10;
    }
    check_improve(ch, gsn_fix, FALSE, 1);
  }
}

void do_enslave(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;

  if (!IS_FORSAKEN(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  } 

  if (argument[0] == '\0')
  {
    send_to_char("Enslave who?\n\r", ch);
    return;
  } 

  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("They are not present.\n\r", ch);
    return;
  }

  if (str_cmp(victim->pcdata->forsaken_master, ch->name))
  {
    act("You must use your ability of compulsion on this one.  $S has not fallen under your charms yet.", 
         ch, NULL, victim, TO_CHAR);
    return;
  }

  victim->pcdata->isMinion = TRUE;
  act("$N has become your slave for $S pitiful life.", ch, NULL, victim, TO_CHAR);     
  act("You now serve $n and $s Dark Lord.", ch, NULL, victim, TO_VICT);
  return;
}

void do_pledge(CHAR_DATA *ch, char *argument)
{
  bool found = FALSE;
  DESCRIPTOR_DATA *d;
  int i;
  char buf[MSL]; 

  if (argument[0] == '\0')
  {
    send_to_char("Pledge your servitude to who?\n\r", ch);
    return;
  }

  for (i = 0;i < MAX_FORSAKEN; i++)
  {
    if (!str_cmp(argument, forsaken_table[i].name))
      found = TRUE;
  }      

  if (!found)
  {
    send_to_char("No such forsaken to serve.\n\r", ch);
    return;
  }
  if (IS_NPC(ch))
  {
    send_to_char("Only players can pledge.\n\r", ch);
    return;
  }
  if (ch->pcdata->isMinion)
  {
    if (!str_cmp(argument, ch->pcdata->forsaken_master))
    {
      send_to_char("You are already serving them.\n\r", ch);
      return;
    }
    
    send_to_char("You dare not leave your current master, they would not be plesed with you.\n\r", ch);
    return;
  }
       
  ch->pcdata->forsaken_master = str_dup(argument);
  sprintf(buf, "You pledge your soul to %s.\n\r", capitalize(argument));
  send_to_char(buf, ch);
 
  for ( d = descriptor_list; d != NULL; d = d->next )
  {

    if ( d->connected == CON_PLAYING && 
         !str_cmp(ch->pcdata->forsaken_master, d->character->name))
    {
      act_new( "$n pledges their faith to you and wishes you to be $s master.",
               ch, argument, d->character, TO_VICT, POS_DEAD);
    }
  } 
}   

void spell_taint (int sn, int level, CHAR_DATA * ch, void *vo,
                           int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int chance;

  if (ch->clan != clan_lookup("Shadow") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

    chance = get_skill(ch, sn);
 
    if (number_percent() > chance)
    {
      send_to_char("Your taint etches through the item making it totally unusable.\n\r", ch);
      extract_obj(obj);
      return;
    }

   
    act("You channel your dark magic into $p.", ch, obj, NULL, TO_CHAR);
    obj->extra_flags = 0;
    SET_BIT(obj->extra_flags, ITEM_NOBREAK);
    SET_BIT(obj->extra_flags, ITEM_GLOW);
    SET_BIT(obj->extra_flags, ITEM_HUM);
    SET_BIT(obj->extra_flags, ITEM_TAINT);
    SET_BIT(obj->extra_flags, ITEM_MELT_DROP);
    SET_BIT(obj->extra_flags, ITEM_BURN_PROOF); 
    return;
}   


void do_ensnare(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;

  if (ch->clan != clan_lookup("Furies") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if (get_skill(ch, gsn_ensnare) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if ((victim = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  /*
  if (!IS_SET(ch->level == LEVEL_HERO))
  {
    send_to_char("No way!.\n\r", ch);
    return;
  }*/

  if (ch == victim)
  {
    send_to_char("That'd be dumb\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
  {
    return;
  }
     
  if (number_percent() < get_skill(ch, gsn_ensnare))
  {
    victim->move = int(victim->move / 1.2);
    ch->mana -= 10;
    act("You ensnare $N.", ch, NULL, victim, TO_CHAR);
    act("$n ensnares $N.", ch, NULL, victim, TO_NOTVICT);
    act("$n ensnares you!", ch, NULL, victim, TO_VICT);  
    check_improve(ch, gsn_ensnare, TRUE, 1);
  }
  else
  {
    ch->mana -= 50;
    send_to_char("You failed.\n\r", ch);
    check_improve(ch, gsn_ensnare, FALSE, 1);
  }
  WAIT_STATE (ch, PULSE_VIOLENCE);  
  return;
}

void dream_clone(CHAR_DATA *ch)
{
  CHAR_DATA *clone;
  MOB_INDEX_DATA *index;
  char buf[MIL];
  int i;

  index = get_mob_index(1);    
  
  clone = create_mobile (index); 
  clone->act = ch->act;
  clone->act = ACT_IS_NPC;
  sprintf(buf, "%sDream", ch->name); 
  clone->name = str_dup(buf);
  sprintf(buf, "%s", ch->name);
  clone->short_descr = str_dup(buf);
  sprintf(buf, "%s is here.", ch->name);
  clone->long_descr = str_dup(buf);
  clone->max_hit = ch->max_hit;
  clone->hit = ch->hit;
  clone->max_mana = ch->max_mana;
  clone->mana = ch->mana;
  clone->max_move = ch->max_move;
  clone->move = ch->move;
  clone->level = ch->level;
  clone->alignment = ch->alignment;
  clone->sex = ch->sex;
  clone->cClass = ch->cClass;
  clone->race = ch->race;
  for (i = 0; i < MAX_STATS; i++)
  {
    clone->perm_stat[i] = ch->perm_stat[i];
    clone->mod_stat[i] = ch->mod_stat[i];
  }
  char_to_room(clone, ch->in_room);
  send_to_char("Mob made.\n\r", ch);
}

void dream_switch(CHAR_DATA *ch)
{
  char buf[MIL];
  CHAR_DATA *victim;

  if (ch->desc->editor != 0)
  {
    send_to_char("Not while in OLC", ch);
    return;
  }

  if (ch->desc == NULL)
    return;
  
  if (ch->desc->original != NULL)
  {
     send_to_char("Not while switched.\n\r", ch);
     return;
  }

  sprintf(buf, "%sDream", ch->name); 

  if ((victim = get_char_world(ch, buf)) == NULL)
  {
    send_to_char("Dream error #1 please report to imm.\n\r", ch);
    return;
  }

 
   ch->desc->character = victim;
   ch->desc->original = ch;
   victim->desc = ch->desc;
   ch->desc = NULL;
   
   if (ch->prompt != NULL)
     victim->prompt = str_dup(ch->prompt);
   victim->comm = ch->comm;
   victim->lines = ch->lines;
   victim->act2 = PLR_DREAM; 
   return;
}   
  
void do_seperate(CHAR_DATA *ch, char *argument)
{
  dream_clone(ch);
  dream_switch(ch);
  send_to_char("Ok.\n\r", ch);
}
    
void do_makegl(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char buf[MSL];

  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (!IS_SET(victim->act2, PLR_GUILDLEADER))
  {
    sprintf(buf, "%s has been made GL.\n", victim->name);
    SET_BIT(victim->act2, PLR_GUILDLEADER);
    send_to_char(buf, ch);
    return;
  }

  sprintf(buf, "%s has been stripped of GL status.\n", victim->name);
  REMOVE_BIT(victim->act2, PLR_GUILDLEADER);
  send_to_char(buf, ch);
  return;
}



void do_leaveguild (CHAR_DATA * ch, char *argument)
{
    char arg1[MIL];
    int sn, i;

    argument = one_argument (argument, arg1);


    if (arg1[0] == '\0' || str_cmp(arg1, "now"))
    {
        send_to_char ("Syntax: leaveguild now\n\r", ch);
        return;
    }

    if (ch->clan == clan_lookup("tinker") && strcmp(argument, "yes"))
    { 
    	send_to_char ("Once a Tinker always a Tinker!\n\r", ch);
        send_to_char ("Type leaveguild now yes to remove yourself\n\r", ch);
        send_to_char ("from tinker.  Note you will lose 20% of your\n\r", ch);
        send_to_char ("hps and mana.\n\r", ch);
    	return;
    }

    if (ch->clan == clan_lookup("Tinker") && !str_cmp(argument, "yes"))
    {
      ch->max_hit = int(ch->max_hit*.8);
      ch->max_mana = int(ch->max_mana*.8);
      ch->pcdata->perm_hit = int(ch->pcdata->perm_hit*.8);
      ch->pcdata->perm_mana = int(ch->pcdata->perm_mana*.8);
    }      

    if (ch->clan == 0)
    {
    	send_to_char ("You dont have a guild!\n\r", ch);
    	return;
    }
    
    
        send_to_char ("You have choosen to leave your guild.\n\r", ch);
        send_to_char ("Damn that was expensive!\n\r", ch);
   
        for (sn = 0; sn < MAX_SKILL; sn++)
        {
          if (skill_table[sn].name != NULL && ch->pcdata->learned[sn] > 0)
          {
            if (ch->pcdata->learned[sn] > 30)
              ch->pcdata->learned[sn] -= 30;
            else
              ch->pcdata->learned[sn] = 1;
          }
        }  
                   
        for (i = 0 ; i < clan_table[ch->clan].top_gskill; i++)
        {
          if (clan_table[ch->clan].gskill[i] != NULL)
          {
           sn = skill_lookup(clan_table[ch->clan].gskill[i]);
           ch->pcdata->learned[sn] = 0;
          }
        }
        remove_member(ch->name, ch->clan);
        save_roster();
        ch->clan = 0;
        return;
    }

void do_bond(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  
  if (ch->sex != 2)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (ch->clan != clan_lookup("aessedai"))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (ch->rank == 0)
  {
    send_to_char("You are not of significant rank.\n\r", ch);
    return;
  } 

  if (ch->rank == 1)
  {
    send_to_char("You could get busted down to novice just for thinking about that!\n\r", ch);
    return;
  }

  if (argument[0] == '\0')
  {
    send_to_char(ch->pcdata->bond, ch);
    return;
  }

  if ((victim = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  } 

  if (IS_BONDED(victim))
  {
    send_to_char("They already belong to another.\n\r", ch);
    return;
  }
  if (victim->sex != 1)
  {
    send_to_char("Betcha that made you feel a little funny.\n\r", ch);
    return;
  }
  if (victim->clan != clan_lookup("warder"))
  {
    send_to_char("They are not trained to serve a Sedai.\n\r", ch);
    return;
  }
  
  free_string(victim->pcdata->bond);
  victim->pcdata->bond = str_dup(ch->name);
  if (ch->pcdata->bond[0] == '\0')
  {
    free_string(ch->pcdata->bond);
    ch->pcdata->bond = str_dup(victim->name);
  }
  else
  {
    char buf[MSL];

    buf[0] = '\0';
    strcat(buf, ch->pcdata->bond);
    strcat(buf, " ");
    strcat(buf, victim->name);
    free_string(ch->pcdata->bond); 
    ch->pcdata->bond = str_dup(buf); 
  }
  send_to_char("You perform a bonding cermony.\n\r", ch);
}  

bool IS_BONDED(CHAR_DATA *ch)
{
  if (ch == NULL)
    return FALSE;

  if (IS_NPC(ch)) 
    return FALSE;

  if (IS_NULLSTR(ch->pcdata->bond))
    return FALSE;

  return TRUE;
}

void do_sense(CHAR_DATA *ch, char *argument)
{

  if (ch->clan != clan_lookup("aessedai") && ch->clan != clan_lookup("Rebel")  && ch->clan != clan_lookup("warder"))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (!IS_BONDED(ch))
  {
    send_to_char("You have no bond to sense.\n\r", ch);
    return;
  }

  if (ch->clan == clan_lookup("warder") || ch->clan == clan_lookup("aessedai"))
  {
    CHAR_DATA *victim;
    int health;
    char condition[MSL];
    char buf[MSL];

    if ((victim = get_char_world(ch, ch->pcdata->bond)) == NULL)
    {
       send_to_char("They are too far away to sense.\n\r", ch);
       return;
    }

    health = (victim->hit / victim->max_hit) * 100;

    if (health > 90)
       sprintf(condition, "is strong as a bull.");
    else if (health > 70)
       sprintf(condition, "is a little tired.");
    else if (health > 50)
       sprintf(condition, "definitely needs to rest.");
    else if (health > 30)
       sprintf(condition, "is about to collapse.");
    else 
       sprintf(condition, "is nearing death.");   
    act("$N $t", ch, condition, victim, TO_CHAR);
    if (ch->in_room->area == victim->in_room->area)
      send_to_char("You sense their presence in town.\n\r", ch);
    else
    {
      sprintf(buf, "You feel them in the direction of %s.\n\r", 
		victim->in_room->area->name);
      send_to_char(buf, ch);
    } 

  }

  if ((ch->clan == clan_lookup("aessedai") && (ch->sex = 2)) || ch->clan == clan_lookup("Rebel") )
  {
    CHAR_DATA *victim;
    int health;
    char condition[MSL];
    char *bonded;
    char lookfor[MIL];
    char buf[MSL];
 
    bonded = str_dup(ch->pcdata->bond);

  
    while(bonded[0] != '\0') 
    {
      bonded = one_argument(bonded, lookfor);
      if ((victim = get_char_world(ch, lookfor)) == NULL)
      {
        send_to_char("They are too far away to sense.\n\r", ch);
        continue;
      }

    health = (victim->hit / victim->max_hit) * 100;

    if (health > 90)
       sprintf(condition, "is strong as a bull.");
    else if (health > 70)
       sprintf(condition, "is a little tired.");
    else if (health > 50)
       sprintf(condition, "definitely needs to rest.");
    else if (health > 30)
       sprintf(condition, "is about to collapse.");
    else 
       sprintf(condition, "is nearing death.");   
    act("$N $t", ch, condition, victim, TO_CHAR);
      if (ch->in_room->area == victim->in_room->area)
        send_to_char("You sense their presence in town.\n\r", ch);
      else
      {
        sprintf(buf, "You feel them in the direction of %s.\n\r", 
 		victim->in_room->area->name);
        send_to_char(buf, ch);
      } 
    }
  }
}     

void bond_death(CHAR_DATA *ch)
{
  CHAR_DATA *victim;
  char *bonded;
  char lookfor[MIL];
 
  bonded = str_dup(ch->pcdata->bond);
  
  while(bonded[0] != '\0') 
  {
    bonded = one_argument(bonded, lookfor);
    if ((victim = get_char_world_special(lookfor)) == NULL)
    {
      continue;
    }
 
    send_to_char("You feel a sharp loss in the bond and it hurts you.\n", victim);
    damage(ch, victim, (ch->max_hit / 20), 1047, DAM_ENERGY, TRUE, FALSE);
  }
  free_string(bonded);
}  

void do_damane(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *damane, *gch;
  MOB_INDEX_DATA *pMobIndex;
  AFFECT_DATA af;
  int sn;

  if (ch->clan != clan_lookup("seanchan") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if (get_skill(ch, gsn_damane) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_SET(ch->act, PLR_TOURNEY) && tournament.noguild)
  {
    send_to_char("Not while in this tournament.\n\r", ch);
    return;
  }
  
  if (IS_AFFECTED(ch, AFF_DAMANE))
  {
    send_to_char("It's to early to call upon another slave.\n\r", ch);
    return;
  }
  
  for (gch = char_list; gch != NULL; gch = gch->next)
  {
     if (is_same_group (gch, ch))
     {
       if (IS_NPC(gch))
       {
         if (gch->pIndexData == get_mob_index( MOB_VNUM_DAMANE ))
         {
           send_to_char("You already have a slave.\n\r", ch);
           return;
         }
       }
     }
  }
  
  if (number_percent() < get_skill(ch, gsn_damane))
  {
    af.where = TO_AFFECTS;
    af.type = gsn_damane;
    af.level = ch->level + 5;
    af.duration = 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_DAMANE;
    affect_to_char (ch, &af);
    
    pMobIndex = get_mob_index( MOB_VNUM_DAMANE );
    if ((damane = create_mobile( pMobIndex )) == NULL)
      return;
     char_to_room( damane, ch->in_room );
    add_follower( damane, ch);
    damane->leader = ch;
    damane->master = ch;
    damane->max_hit = int(ch->max_hit * .9);
    damane->hit = damane->max_hit;
    damane->level = ch->level + 5;
    damane->hitroll = int(ch->hitroll * .9);
    damane->damroll = int(ch->damroll * .9);
    ch->pet = damane; 
    sn = skill_lookup("Charm");
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = 600;
    af.duration = -1;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char (damane, &af);
    
    act( "$n has called a slave aid $s.\n\r", ch, NULL, NULL, TO_ROOM);   
    send_to_char( "You have called upon a slave.\n\r", ch);
    check_improve(ch, gsn_damane, FALSE, 1);
  }
  else
  {
    send_to_char("You fail to call upon a servant.\n\r", ch);
    check_improve(ch, gsn_damane, FALSE, 1);
  }
}

void do_build(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *obj;
  char arg1[MIL];
  int number;
  int duration;
  int i;
  bool failed = FALSE;
  
  argument = one_argument(argument, arg1);

  if (arg1[0] == '\0')
  {
    send_to_char("Build a fire using what?\n\r", ch);
    return;
  }

  if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS) &&
       ch->in_room->vnum != TINKER_SAFE) 
  {
    send_to_char("Not indoors.\n\r", ch);
    return;
  }

  if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) && 
       ch->in_room->vnum != TINKER_SAFE) 
  {
    send_to_char("You can't build a fire here.\n\r", ch);
    return;
  }


  if (get_skill(ch, gsn_build) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if ((obj = get_obj_carry(ch, arg1, ch)) == NULL)
  {
    send_to_char("You don't have that.\n\r", ch);
    return;
  }   

  if (obj->item_type != ITEM_FUEL)
  {
    send_to_char("That wouldn't make a very good fire.\n\r", ch);
    return;
  }

  if (char_has_carry(ch, obj) < obj->value[1])
  {
    send_to_char("You don't have enough fuel to make a fire.\n\r", ch);
    return;
  }

  if (number_percent() > (get_skill(ch, gsn_build)))
  {
    failed = TRUE;
    check_improve(ch, gsn_build, TRUE, 1);
  }
  else
    check_improve(ch, gsn_build, FALSE, 1);

  duration = obj->value[0];
  number = obj->value[1];
  if (!failed)
    extract_obj(obj);
  else
  {
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);
  }
  for (i = 0; i < number - 1; i++)
  {
    if ((obj = get_obj_carry(ch, arg1, ch)) == NULL)
    {
      send_to_char("Error too many looks too few items.\n\r", ch);
      return;
    }
    if (!failed)
      extract_obj(obj);
    else
    {
      obj_from_char(obj);
      obj_to_room(obj, ch->in_room);
    }
  }
 
  if (failed)
  {
    act("You throw the $p on the ground making a pile that would never light", ch, obj, NULL, TO_CHAR);
    act("$n piles $p on the ground in a messy heap.", ch, obj, NULL, TO_ROOM);
    return;
  }  

  obj = create_object(get_obj_index(STANDARD_FIRE), ch->level);
  obj->value[0] = duration;
  free_string(obj->name);
  obj->name = str_dup("unlit fire");
  free_string(obj->description);
  obj->description = str_dup("The preperations for a fire are on the ground here.");
  free_string(obj->short_descr);
  obj->short_descr = str_dup("Preperations for a fire..");   
  obj_to_room(obj, ch->in_room);
  send_to_char("You stack up the fuel and prepare it for lighting.\n\r", ch);
  act("$n stacks up some fuel on the ground and prepares to light it.", ch, NULL, NULL, TO_ROOM);   
}

void do_ignite(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *obj, *source;

  if ((obj = get_obj_list(ch, "unlit fire", ch->in_room->contents)) == NULL)
  {
    send_to_char("Nothing to light here.\n\r", ch);
    return;
  }

  if (argument[0] == '\0')
  {
    send_to_char("Ignite the tinder using what?\n\r", ch);
    return;
  }

  if ((source = get_obj_carry(ch, argument, ch)) == NULL) 
  {
     send_to_char("You don't have that.n\r", ch);
     return;
  }

  if (source->item_type != ITEM_IGNITER)
  {
    send_to_char("That won't make a spark.\n\r", ch);
    return;
  }

  obj_from_room(obj);
  free_string(obj->name);
  obj->name = str_dup("burning fire");
  free_string(obj->description);
  obj->description = str_dup("A burning campfire lights up the surrounding landscape.");
  free_string(obj->short_descr);
  obj->short_descr = str_dup("A fire burning.");
  obj->extra_flags = ITEM_BURNING;
  obj->timer = obj->value[0];
  obj_to_room(obj, ch->in_room);
  act("You strike the $p igniting the tinder.", ch, source, NULL, TO_CHAR);
  act("$n strikes $p lighting the fuel and the fire starts up.", ch, source, NULL, TO_ROOM);
   
}
void do_simulacrum (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    

  if (ch->clan != clan_lookup("furie"))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  
  if ((victim = get_char_room (ch, NULL, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (victim->level > LEVEL_HERO)
  {
    send_to_char ("Not on Immortals\n\r", ch);
    return;
  }
  
  if (IS_NPC(victim))
  {
    send_to_char ("You cannot mimic the actions of a mob!\n\r", ch);
    return;
  }

  if (victim == ch)
  {
    send_to_char ("Doesnt that make you feel silly?\n\r", ch);
    return;
  }

  if (victim->cClass == class_lookup("Tinker"))
  {
    send_to_char ("You move and mimic the skills of a Tinker!\n\r", ch);
    ch->simskill = "tinker";
    return;
  }
  
  if (victim->cClass == class_lookup("Sedai"))
  {
    send_to_char ("You move and mimic the skills of an Aes Sedai!\n\r", ch);
    ch->simskill = "sedai";
    return;
  }
 
  if (victim->cClass == class_lookup("Aiel"))
  {
    send_to_char ("You move and mimic the skills of an Aiel!\n\r", ch);
    ch->simskill = "aiel";
    return;
  }

  if (victim->cClass == class_lookup("Gleeman"))
  {
    send_to_char ("You move and mimic the skills of a Gleeman!\n\r", ch);
    ch->simskill = "gleeman";
    return;
  }

  if (victim->cClass == class_lookup("Shadow"))
  {
    send_to_char ("You move and mimic the skills of a Shadow!\n\r", ch);
    ch->simskill = "shadow";
    return;
  }
if (victim->cClass == class_lookup("Redhand"))
  {
    send_to_char ("You move and mimic the skills of a Red Hand!\n\r", ch);
    ch->simskill = "redhand";
    return;
  }
if (victim->cClass == class_lookup("Warder"))
  {
    send_to_char ("You move and mimic the skills of a Warder!\n\r", ch);
    ch->simskill = "sedai";
    return;
  }
if (victim->cClass == class_lookup("Wolfbrother"))
  {
    send_to_char ("You move and mimic the skills of a Wolfbrother!\n\r", ch);
    ch->simskill = "wolfbrother";
    return;
  }
if (victim->cClass == class_lookup("Seanchan"))
  {
    send_to_char ("You move and mimic the skills of a Seanchan!\n\r", ch);
    ch->simskill = "seanchan";
    return; 
 }
 }

void do_howl(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;

  if (ch->clan != clan_lookup("Wolfbrother") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }


  if (get_skill(ch, gsn_howl) < 1)
  {
    send_to_char("Huh?", ch);
    return;
  }

  if (IS_SET(ch->act, PLR_TOURNEY) && tournament.noguild)
  {
    send_to_char("Not while in this tournament.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_HOWL))
  {
    send_to_char("You are already ready for battle.\n\r", ch);
    return;
  }

  if (ch->mana < 50)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }

  if (number_percent() < get_skill(ch, gsn_howl))
  {
    af.where     = TO_AFFECTS;
    af.type      = gsn_howl;
    af.level     = ch->level;
    af.duration  = ch->level / 5;
    af.modifier  = ch->level / 5;
    af.location  = APPLY_DAMROLL;
    af.bitvector = AFF_HOWL;
    affect_to_char( ch, &af );
    af.location  = APPLY_HITROLL;
    affect_to_char(ch, &af );
    ch->mana -= 50;
    act("You tilt your head and let out a howl.", ch, NULL, NULL, TO_CHAR);
    act("$n howls towards the moon.", ch, NULL, NULL, TO_ROOM);
    check_improve(ch, gsn_howl, TRUE, 1);

  }
  else
  {
    ch->mana -= 50;
    send_to_char("You voice goes horse.\n\r", ch);
    check_improve(ch, gsn_howl, FALSE, 1);
    return;
  }    

}

void do_medallion(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;

  if (ch->clan != clan_lookup("Redhand") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_SET(ch->act, PLR_TOURNEY) && tournament.noguild)
  {
    send_to_char("Not while in this tournament.\n\r", ch);
    return;
  }

  if (get_skill(ch, gsn_medallion) < 1)
  {
    send_to_char("Huh?", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_MEDALLION))
  {
    send_to_char("You are already wearing the medallion.\n\r", ch);
    return;
  }

  if (ch->mana < 50)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }

  if (number_percent() < get_skill(ch, gsn_medallion))
  {
    af.where     = TO_AFFECTS;
    af.type      = gsn_medallion;
    af.level     = ch->level;
    af.duration  = ch->level / 6;
    af.modifier  = ch->level / 6;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_MEDALLION;
    affect_to_char( ch, &af );
    ch->mana -= 50;
    act("You place the medallion around your neck.", ch, NULL, NULL, TO_CHAR);
    act("$n puts on a medallion.", ch, NULL, NULL, TO_ROOM);
    check_improve(ch, gsn_medallion, TRUE, 1);

  }
  else
  {
    ch->mana -= 50;
    send_to_char("You don't feel like wearing it.\n\r", ch);
    check_improve(ch, gsn_medallion, FALSE, 1);
    return;
  }    
}

