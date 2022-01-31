/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.    *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                            *
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
*    ROM 2.4 is copyright 1993-1998 Russ Taylor                             *
*    ROM has been brought to you by the ROM consortium                      *
*        Russ Taylor (rtaylor@hypercube.org)                                *
*        Gabrielle Taylor (gtaylor@hypercube.org)                           *
*        Brian Moore (zump@rom.org)                                         *
*    By using this code, you have agreed to follow the terms of the         *
*    ROM license, in the file Rom24/doc/rom.license                         *
****************************************************************************/

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
#include "tables.h"
#include "recycle.h"
#include "lookup.h"
#include "magic.h"

/*
 * Local functions.
 */

void abort_rescue args (( CHAR_DATA *ch, bool bPullAttacker ));
void check_assist args ((CHAR_DATA * ch, CHAR_DATA * victim));
bool check_air args ((CHAR_DATA * ch, CHAR_DATA * victim, bool arrow));
bool check_dodge args ((CHAR_DATA * ch, CHAR_DATA * victim, bool arrow));
bool check_parry args ((CHAR_DATA * ch, CHAR_DATA * victim, bool arrow));
bool check_shield_block args ((CHAR_DATA * ch, CHAR_DATA * victim, bool arrow));
bool check_evade  args ((CHAR_DATA * ch, CHAR_DATA * victim, bool arrow));
bool check_block  args ((CHAR_DATA * ch, CHAR_DATA * victim, bool arrow));
bool check_counter   args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt) );
void dam_message args ((CHAR_DATA * ch, CHAR_DATA * victim, int dam,int dt, bool immune));
void obj_dam_message args ((OBJ_DATA * obj, CHAR_DATA * victim, int dam,int dt, bool immune));
void death_cry args ((CHAR_DATA * ch));
void group_gain args ((CHAR_DATA * ch, CHAR_DATA * victim));
int xp_compute args ((CHAR_DATA * gch, CHAR_DATA * victim, int total_levels, int members));
bool is_safe args ((CHAR_DATA * ch, CHAR_DATA * victim));
void make_corpse args ((CHAR_DATA * ch, CHAR_DATA * killer));
void one_hit args ((CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool secondary));
void mob_hit args ((CHAR_DATA * ch, CHAR_DATA * victim, int dt));
void raw_kill args ((CHAR_DATA * ch, CHAR_DATA * victim));
void set_fighting args ((CHAR_DATA * ch, CHAR_DATA * victim));
void disarm args ((CHAR_DATA * ch, CHAR_DATA * victim));
void dislodge args ((CHAR_DATA *ch, CHAR_DATA *victim));
void tournament_death args ((CHAR_DATA *ch,CHAR_DATA *victim));
void throw_mount args ((CHAR_DATA *mount));
CHAR_DATA *char_here args (( CHAR_DATA *ch ));
void bond_death args (( CHAR_DATA *ch ));
void fail_rescue args (( CHAR_DATA *ch, CHAR_DATA *mob ));
bool  defence_chance args (( CHAR_DATA *ch, CHAR_DATA *victim,bool arrow, bool defense, int gsn));
bool calc_hit args (( CHAR_DATA *ch, CHAR_DATA *victim,bool secondary,int dt,int dam_type));
void update_mobbed args (( char *name, int count));
//void damage_equipment args (( int dam ));



// Extern calls.
extern void set_fighting(CHAR_DATA *ch, CHAR_DATA *victim);

bool IS_INPKRANGE ( CHAR_DATA *ch, CHAR_DATA *victim)
{
    int difference;
    int change = 0;
    
    if (ch->level < 11)
        change = 0;
    else if (ch->level < 16)
        change = 2; 
    else if (ch->level < 26)
        change = 3; 
    else if (ch->level < 36)
        change = 4; 
    else if (ch->level < 46)
        change = 5;
    else if (ch->level < 56)
        change = 6;
    else if (ch->level < 66)
        change = 7;
    else if (ch->level <= 78)
        change = 8;
    else
        change = 0;
    if (IS_NPC (ch)||IS_NPC(victim)||IS_IMMORTAL(ch))
        return TRUE;
    difference = ch->level - victim->level;
    if (difference < 0)
        difference *= -1;
    if (difference > change && victim->level < ch->level && !IS_NPC(victim))
        return FALSE;
    return TRUE;
}

void auto_guidenote ( CHAR_DATA *ch, CHAR_DATA *victim, int type)
{
    char buf[MSL];
    if (IS_INPKRANGE(ch,victim)|| IS_SET(ch->act, PLR_TOURNEY) || IS_SET(ch->act, PLR_QUESTING))
        return;
    if (type == GUIDE_MURDR)
    {
        sprintf(buf,"%s (Level %d) has been killed by %s (Level %d) and OOL'd\nPlease check and verify if Murder Flag is appropriate.",victim->name,victim->level,ch->name,ch->level);
        system_note("Auto Guide",NOTE_NEWS,"Guide IMP","ATTN Guide Player: OOL has occured",buf);
    }
    else if (type == GUIDE_THIEF)
    {
    	sprintf(buf,"%s (Level %d) has been ganked by %s (Level %d) OOL steal!\nPlease check and verify if Thief Flag is appropriate.",IS_DISGUISED(victim)?victim->pcdata->disguise.orig_name:victim->name,victim->level,IS_DISGUISED(ch)?ch->pcdata->disguise.orig_name:ch->name,ch->level);
        system_note("Auto Guide",NOTE_NEWS,"Guide IMP","ATTN Guide Player: OOL has occured",buf);
    }

    return;
}

void auto_guildnote ( CHAR_DATA *ch, CHAR_DATA *victim)
{
    char buf[MSL];
    char def_guild[MSL];
    char vic_guild[MSL];
    if (IS_NPC(ch)||IS_SET(ch->act, PLR_TOURNEY) || IS_SET(ch->act, PLR_QUESTING) || !IS_GUILDED(victim))
        return;
    sprintf(def_guild,"%s",clan_table[victim->clan].name);
    if (ch == victim)    
      return;

    if (IS_GUILDED(ch))
    {
        sprintf(vic_guild,"%s",clan_table[ch->clan].name);
        sprintf(buf,"Most Prestigious Guild leader,\n\nWe regret to report that %s has been defeated at the hands of %s from the %s. %s is recovering well and we expect him to be back to adequate potential soon.\n\nPella the Healer\n",
        IS_DISGUISED(victim)?victim->pcdata->disguise.orig_name:victim->name,IS_DISGUISED(ch)?ch->pcdata->disguise.orig_name:ch->name,vic_guild,IS_DISGUISED(victim)?victim->pcdata->disguise.orig_name:victim->name);
        system_note("Messenger",NOTE_GUILDMASTER,def_guild,"A rolled up parchment tainted with blood",buf);
        
        sprintf(buf,"Great news my Lord!!\n\n%s has defeated %s this day and struck a terrible blow to the %s!\n\nTonight, we scouts shall celebrate the occasion!\n",
        IS_DISGUISED(ch)?ch->pcdata->disguise.orig_name:ch->name,IS_DISGUISED(victim)?victim->pcdata->disguise.orig_name:victim->name,def_guild);
        system_note("Scout",NOTE_GUILDMASTER,vic_guild,"A scout arrives bearing news!",buf);

    }
    else
    {
    	sprintf(buf,"Most Prestigious Guild leader,\n\nWe regret to report that %s has been defeated at the hands of %s. %s is recovering well and we expect him to be back to adequate potential soon.\n\nPella the Healer\n",
        IS_DISGUISED(victim)?victim->pcdata->disguise.orig_name:victim->name,IS_DISGUISED(ch)?ch->pcdata->disguise.orig_name:ch->name,IS_DISGUISED(victim)?victim->pcdata->disguise.orig_name:victim->name);
        system_note("Messenger",NOTE_GUILDMASTER,def_guild,"A rolled up parchment tainted with blood",buf);
    }

    return;
}

/*void remove_pk ( CHAR_DATA *ch, PK_DATA *pk, sh_int type)
{
}

void add_attack(CHAR_DATA *ch, CHAR_DATA *victim)
{
}

void check_attack(CHAR_DATA *ch, CHAR_DATA *victim)    
{ 
  PK_DATA *pk, *pk_next;
  bool attack_found = FALSE;
  bool kill_found = FALSE;  
  long time;

  time = current_time;

  for (pk = ch->pcdata->atkby;pk != NULL;pk = pk_next)      
  {
    pk_next = pk->next;
    if (time - pk->time > (60 * 60 * 24))   
    {
      remove_pk(ch, pk, 0);
    }

    if (victim->id == pk->id)
      attack_found = TRUE;
  }

  if (!attack_found)
    add_attack(ch, victim);

  for (pk = ch->pcdata->kill; pk != NULL; pk = pk_next)
  {
    pk_next = pk->next;
    if (time - pk->time > (60 * 60 * 24))   
    {
      remove_pk(ch, pk, 1);
    }

    if (victim->id == pk->id)
      kill_found = TRUE;
  }

  if (kill_found)
  {
    send_to_char("You have just attacked someone you killed within 24 hours.  Murder status set.\n\r", ch);
    ch->murd += 300;
  }    

}*/
    

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update (void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;
     OBJ_DATA *obj, *obj_next;
     bool room_trig = FALSE;


    for (ch = char_list; ch != NULL; ch = ch_next)
    {
        ch_next = ch->next;
		
        if ((victim = ch->fighting) == NULL || ch->in_room == NULL)
            continue;

        if (!IS_NPC(ch))
        {
	  ch->pcdata->damInflicted = 0;
	  ch->pcdata->damReceived = 0;
        }

        if (IS_AWAKE (ch) && ch->in_room == victim->in_room)
            multi_hit (ch, victim, TYPE_UNDEFINED);
        else
            stop_fighting (ch, FALSE);

        if ((victim = ch->fighting) == NULL)
            continue;

        		
	
	/*
         * Fun for the whole family!
         */
        check_assist (ch, victim);

        if (IS_NPC (ch))
        {
            if (HAS_TRIGGER_MOB (ch, TRIG_FIGHT))
                p_percent_trigger (ch, NULL, NULL, victim, NULL, NULL, TRIG_FIGHT);
            if (HAS_TRIGGER_MOB (ch, TRIG_HPCNT))
                p_hprct_trigger (ch, victim);
        }

        for ( obj = ch->carrying; obj; obj = obj_next )
        {
            obj_next = obj->next_content;
                
            if ( obj->wear_loc != WEAR_NONE && HAS_TRIGGER_OBJ( obj, TRIG_FIGHT ) )
                p_percent_trigger(  NULL, obj, NULL, victim, NULL, NULL, TRIG_FIGHT );
        }

        if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_FIGHT ) && room_trig == FALSE )
        {
            room_trig = TRUE;
            p_percent_trigger( NULL, NULL, ch->in_room, victim, NULL, NULL, TRIG_FIGHT );
        }

    }

    return;
}

void round_print (void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;

    for (ch = char_list; ch != NULL; ch = ch_next)
    {
        ch_next = ch->next;
        if (!IS_NPC(ch))
	{
	  if ((ch->pcdata->damInflicted > 0 || ch->pcdata->damReceived > 0) && !ch->fighting)
	  {
/*
	    char buf[MSL];
            sprintf(buf, "Round stats: Inflict %d Received %d\n\r",
                  ch->pcdata->damInflicted, ch->pcdata->damReceived);
            send_to_char(buf, ch);
*/
            ch->pcdata->damInflicted = 0;
            ch->pcdata->damReceived = 0;
          }
        }
     }
}

/* for auto assisting */
void check_assist (CHAR_DATA * ch, CHAR_DATA * victim)
{
    char buf [MSL];
    CHAR_DATA *rch, *rch_next;

    for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
    {
        rch_next = rch->next_in_room;

        if (IS_AWAKE (rch) && rch->fighting == NULL && can_see(rch,victim))
        {

            /* quick check for ASSIST_PLAYER */
            if (!IS_NPC (ch) && IS_NPC (rch)
                && IS_SET (rch->off_flags, ASSIST_PLAYERS)
                && rch->level + 6 > victim->level)
            {
                do_function (rch, &do_emote, "screams and attacks!");
                multi_hit (rch, victim, TYPE_UNDEFINED);
                continue;
            }

            /* PCs next */
            if (!IS_NPC (ch) || IS_AFFECTED (ch, AFF_CHARM))
            {
                if (((!IS_NPC (rch) && IS_SET (rch->act, PLR_AUTOASSIST))
                     || IS_AFFECTED (rch, AFF_CHARM))
                    && is_same_group (ch, rch) && !is_safe (rch, victim))
                {
                    if (!IS_NPC(victim)&&!IS_NPC(ch))
                    {
                    	sprintf(buf,"%s (lvl %d) auto-assisted %s (lvl %d) and attacked %s (lvl %d)",
                    	    rch->name,rch->level,ch->name,ch->level,victim->name,victim->level);
                    	log_string (buf);
                    }
                    multi_hit (rch, victim, TYPE_UNDEFINED);
                }
                continue;
            }

            /* now check the NPC cases */

            if (IS_NPC (ch) && !IS_AFFECTED (ch, AFF_CHARM))
            {
                if ((IS_NPC (rch) && IS_SET (rch->off_flags, ASSIST_ALL))
                    || (IS_NPC (rch) && rch->group && rch->group == ch->group)
                    || (IS_NPC (rch) && rch->race == ch->race
                        && IS_SET (rch->off_flags, ASSIST_RACE))
                    || (IS_NPC (rch) && IS_SET (rch->off_flags, ASSIST_ALIGN)
                        && ((IS_GOOD (rch) && IS_GOOD (ch))
                            || (IS_EVIL (rch) && IS_EVIL (ch))
                            || (IS_NEUTRAL (rch) && IS_NEUTRAL (ch))))
                    || (rch->pIndexData == ch->pIndexData
                        && IS_SET (rch->off_flags, ASSIST_VNUM)))
                {
                    CHAR_DATA *vch;
                    CHAR_DATA *target;
                    int number;

                    if (ch->rescuer && ch->fighting == ch->attacker)
			continue;

                    if (number_bits (1) == 0)
                        continue;

                    target = NULL;
                    number = 0;
                    for (vch = ch->in_room->people; vch; vch = vch->next)
                    {
                        if (can_see (rch, vch)
                            && is_same_group (vch, victim)
                            && number_range (0, number) == 0)
                        {
                            target = vch;
                            number++;
                        }
                    }

                    if (target != NULL)
                    {
                        do_function (rch, &do_emote, "screams and attacks!");
                        multi_hit (rch, target, TYPE_UNDEFINED);
                    }
                }
            }
        }
    }
}


/*
 * Do one group of attacks.
 */
void multi_hit (CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
    int chance;

    /* decrement the wait */
    if (ch->desc == NULL)
        ch->wait = UMAX (0, ch->wait - PULSE_VIOLENCE);

    if (ch->desc == NULL)
        ch->daze = UMAX (0, ch->daze - PULSE_VIOLENCE);

    if (!IS_NPC(ch) && 
          IS_SET(ch->act2, PLR_MSP_MUSIC) && 
          !IS_SET(ch->act2, PLR_MSP_PLAYING))
    {
      send_to_char("!!MUSIC(Sounds/Battle.mid V=80 L=-1 C=1 T=Fight)\n\r", ch);
      SET_BIT(ch->act2, PLR_MSP_PLAYING);
    }

    if (IS_SET(ch->act2, PLR_FLED))
      REMOVE_BIT(ch->act2, PLR_FLED);
 
    /* no attacks for stunnies -- just a check */
    if (ch->position < POS_RESTING)
        return;
	
    if (IS_AFFECTED(ch, AFF_SHROUD))
    {
       affect_strip(ch, gsn_shroud);
       STR_REMOVE_BIT(ch->affected_by, AFF_SHROUD);
    }    

    if (IS_AFFECTED(ch, AFF_HIDE))
    {
       affect_strip(ch, gsn_hide);
       STR_REMOVE_BIT(ch->affected_by, AFF_HIDE);
    }    

    if (IS_NPC (ch))
    {
        mob_hit (ch, victim, dt);
        return;
    }

    if (!IS_NPC(ch) && !IS_NPC(victim))
    {
      ch->pcdata->pk_timer = 120;
      ch->pcdata->safe_timer = 0;
      victim->pcdata->pk_timer = 120;
      victim->pcdata->wait_timer = 0;
    }

    if ((get_skill(ch, gsn_blood) / 5) > number_percent() 
       && !furies_imm(ch,victim)
       && dt != gsn_assassinate
       && dt != gsn_strike  
       && dt != gsn_backstab)
    {
    if (ch->clan != clan_lookup("Seanchan"))
       {
         return;
       }
      int blood_dam;
      blood_dam = number_range(ch->level * 2 + 20, ch->level * 3 - 20);
      ch->hit += blood_dam / 2;  
      damage (ch, victim, blood_dam, 1040, DAM_ENERGY, TRUE, FALSE);
      check_improve(ch, gsn_blood, TRUE, 1);
    }

    if ((get_skill(ch, gsn_dome) / 5) > number_percent() 
       && dt != gsn_assassinate
       && dt != gsn_strike 
       && dt != gsn_backstab)
    {
       dome(ch, victim); 
    }

    one_hit (ch, victim, dt, FALSE);

    if (get_eq_char( ch, WEAR_SECONDARY) && !IS_NPC(ch) 
          && dt != gsn_assassinate
          && dt != gsn_strike  
          && dt != gsn_backstab)
    {

     chance = int(get_skill(ch, gsn_dual_wield));
     chance -= class_table[ch->cClass].thac0_32;
      if (number_percent() < chance)
      {
        one_hit( ch, victim, dt, TRUE);
        check_improve(ch, gsn_dual_wield, TRUE, 1);
      }
      check_improve(ch, gsn_dual_wield, FALSE, 1);
    }   

    if (ch->fighting != victim)
        return;

    if (IS_AFFECTED (ch, AFF_HASTE) || (IS_AFFECTED (ch, AFF_VEIL)&& !furies_imm(ch,victim)))
        one_hit (ch, victim, dt, FALSE);

    if (ch->fighting != victim 
        || dt == gsn_backstab 
        || dt == gsn_assassinate
        || dt == gsn_strike)
        return;
    
    if ((get_skill(ch,gsn_martial_arts)>1
        || get_skill(ch,gsn_hand_to_hand)>1)
        && get_eq_char( ch, WEAR_WIELD)==NULL)
    {
    	one_hit(ch,victim,dt,FALSE);
    	if (ch->fighting != victim)
            return;
    }
    chance = int(get_skill (ch, gsn_second_attack) / 1.5);
    chance -= class_table[ch->cClass].thac0_32 * 2;

    if (IS_AFFECTED (ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_STICKY))
        chance /= 2;

    if (number_percent () < chance)
    {
        one_hit (ch, victim, dt, FALSE);
        check_improve (ch, gsn_second_attack, TRUE, 5);
        if (ch->fighting != victim)
            return;
    }

    chance = get_skill (ch, gsn_third_attack) / 2;
    chance -= class_table[ch->cClass].thac0_32 * 2;

    if (IS_AFFECTED (ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_STICKY))
        chance = 0;;

    if (number_percent () < chance)
    {
        one_hit (ch, victim, dt, FALSE);
        check_improve (ch, gsn_third_attack, TRUE, 6);
        if (ch->fighting != victim)
            return;
    }

    if (!str_cmp("warrior", class_table[ch->cClass].name) && ch->pcdata->stance == STANCE_OFFENSIVE)
    {
	chance = int(get_skill (ch, gsn_fourth_attack) / 2.5);
    }

    chance -= class_table[ch->cClass].thac0_32 * 2;

    if (IS_AFFECTED (ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_STICKY))
        chance = 0;;

    if (number_percent () < chance && (!str_cmp("warrior", class_table[ch->cClass].name) && ch->pcdata->stance != STANCE_OFFENSIVE))
    {
        one_hit (ch, victim, dt, FALSE);
        check_improve (ch, gsn_fourth_attack, TRUE, 7);
        if (ch->fighting != victim)
            return;
    }

    if ((ch->clan == clan_lookup("warder")) && (ch->sex == 1))
    {	
    chance = get_skill (ch, gsn_void) / 3;
    if (number_percent () < chance && !furies_imm(ch,victim))
    {
      int howMany;
    //  send_to_char("`#Void Check #1 - Passed`7\n", ch);
      howMany = number_range(1, 24);
      if (howMany > 22)
      {
     //   send_to_char("`#Void Check #2 - 7 Attacks\n", ch);
        send_to_char("You go into the void.\n\r", ch);
        send_to_char("You perform the sword-form '`#T`1he `!B`1oar `%R`5ushes `^D`6own`* the `8M`*ountain'...\n\r",ch);
        send_to_char("... and dance around your opponent with the form '`#C`3at `!D`1ances`* on the `8W`*all!\n\r",ch); 
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE); 
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);   
      }       
      else if (howMany > 19)
      {
     //   send_to_char("`#Void Check #2 Passed - 5 Attacks\n", ch);
        send_to_char("You go into the void.\n\r", ch);
        send_to_char("You perform the sword-form '`#T`3he `@F`2alling `@L`2eaf`*'...\n\r",ch);
        send_to_char("... but it quickly changes to '`#T`3he `&W`7ind `!B`1lows`* over the W`8all`*' in mid-form!\n\r",ch);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);     
      }       
      else if (howMany > 16)
      {
    //    send_to_char("`#Void Check #2 Passed - 4 Attacks\n", ch);
        send_to_char("You go into the void.\n\r", ch);
        send_to_char("You perform the sword-form '`#T`3he `@C`2reeper `!E`1mbraces the `#O`3ak`*'!\n\r",ch);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
      }       
      else if (howMany > 12)
      {
    //    send_to_char("`#Void Check #2 Passed - 3 Attacks\n", ch);
        send_to_char("You go into the void.\n\r", ch);
        send_to_char("You perform the sword-form '`#L`3ightning`* of `%T`5hree `!P`1rongs`*'!\n\r",ch);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
      }       
      else
      {
    //    send_to_char("`#Void Check #2 Passed - 2 Attacks\n", ch);
        send_to_char("You go into the void.\n\r", ch);
        send_to_char("You perform the sword-form 'The `#C`3at `!D`1ances`* on the W`8all`*'!\n\r",ch);
        one_hit (ch, victim, dt, FALSE);
        one_hit (ch, victim, dt, FALSE);
      }
       
      check_improve(ch, gsn_void, TRUE, 1);
    }
    return;
}}

/* procedure for all mobile attacks */
void mob_hit (CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
    int chance, number;
    CHAR_DATA *vch, *vch_next;

    if (ch->attacker == victim)
      return;
 
    one_hit (ch, victim, dt, FALSE);

    if (ch->fighting != victim)
        return;

    /* Area attack -- BALLS nasty! */

    if (IS_SET (ch->off_flags, OFF_AREA_ATTACK))
    {
        for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
        {
            vch_next = vch->next;
            if ((vch != victim && vch->fighting == ch))
                one_hit (ch, vch, dt, FALSE);
        }
    }

    if (IS_AFFECTED (ch, AFF_HASTE)
        || (IS_SET (ch->off_flags, OFF_FAST) && !IS_AFFECTED (ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_STICKY)))
        one_hit (ch, victim, dt, FALSE);

    if (ch->fighting != victim || dt == gsn_backstab)
        return;

    chance = get_skill (ch, gsn_second_attack) / 2;

    if (IS_AFFECTED (ch, AFF_SLOW) && !IS_SET (ch->off_flags, OFF_FAST))
        chance /= 2;

    if (number_percent () < chance)
    {
        one_hit (ch, victim, dt, FALSE);
        if (ch->fighting != victim)
            return;
    }

    chance = get_skill (ch, gsn_third_attack) / 4;

    if ((IS_AFFECTED(ch, AFF_STICKY) || IS_AFFECTED (ch, AFF_SLOW)) && !IS_SET (ch->off_flags, OFF_FAST))
        chance = 0;

    if (number_percent () < chance)
    {
        one_hit (ch, victim, dt, FALSE);
        if (ch->fighting != victim)
            return;
    }

    /* oh boy!  Fun stuff! */

    if (ch->wait > 0)
        return;

    number = number_range (0, 2);

    if (number == 1 && IS_SET (ch->act, ACT_MAGE))
    {
        /*  { mob_cast_mage(ch,victim); return; } */ ;
    }

    if (number == 2 && IS_SET (ch->act, ACT_CLERIC))
    {
        /* { mob_cast_cleric(ch,victim); return; } */ ;
    }

    /* now for the skills */

    number = number_range (0, 8);

    switch (number)
    {
        case (0):
            if (IS_SET (ch->off_flags, OFF_BASH))
                do_function (ch, &do_bash, "");
            break;

        case (1):
            if (IS_SET (ch->off_flags, OFF_BERSERK)
                && !IS_AFFECTED (ch, AFF_BERSERK))
                do_function (ch, &do_berserk, "");
            break;


        case (2):
            if (IS_SET (ch->off_flags, OFF_DISARM)
                || (get_weapon_sn (ch, FALSE) != gsn_hand_to_hand
                    && (IS_SET (ch->act, ACT_WARRIOR)
                        || IS_SET (ch->act, ACT_THIEF))))
                do_function (ch, &do_disarm, "");
            break;

        case (3):
            if (IS_SET (ch->off_flags, OFF_KICK))
                do_function (ch, &do_kick, "");
            break;

        case (4):
            if (IS_SET (ch->off_flags, OFF_KICK_DIRT))
                do_function (ch, &do_dirt, "");
            break;

        case (5):
            if (IS_SET (ch->off_flags, OFF_TAIL))
            {
                /* do_function(ch, &do_tail, "") */ ;
            }
            break;

        case (6):
            if (IS_SET (ch->off_flags, OFF_TRIP))
                do_function (ch, &do_trip, "");
            break;

        case (7):
            if (IS_SET (ch->off_flags, OFF_CRUSH))
            {
                /* do_function(ch, &do_crush, "") */ ;
            }
            break;
        case (8):
            if (IS_SET (ch->off_flags, OFF_BACKSTAB))
            {
                do_function (ch, &do_backstab, "");
            }
    }
}


bool calc_hit (CHAR_DATA * ch, CHAR_DATA *victim, bool secondary, int dt, int dam_type)
{
    int victim_ac;
    int diceroll;
    int sn, skill;
    int thac0;
    int thac0_00;
    int thac0_32;
    char buf[MSL];
    
    sn = get_weapon_sn (ch, secondary);
    skill = 20 + get_weapon_skill (ch, sn);
    if (!IS_AWAKE(victim))
        return TRUE;
    if (dt == 1045)//Critical Hit
        return TRUE;
    if (IS_NPC (ch))
    {
        thac0_00 = 20;
        thac0_32 = -4;           
        if (IS_SET (ch->act, ACT_WARRIOR))
            thac0_32 = -8;
        else if (IS_SET (ch->act, ACT_THIEF))
            thac0_32 = -4;
        else if (IS_SET (ch->act, ACT_CLERIC))
            thac0_32 = 2;
        else if (IS_SET (ch->act, ACT_MAGE))
            thac0_32 = 6;
    }
    else
    {
        thac0_00 = class_table[ch->cClass].thac0_00;
        thac0_32 = class_table[ch->cClass].thac0_32;
    }
    thac0 = interpolate (ch->level, thac0_00, thac0_32);

    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
    {
      sprintf(buf, "Interpolate - %d", thac0);
      send_to_char(buf, ch);
    } 

    if (thac0 < 0)
        thac0 = int(thac0 / 2.0);

    if (thac0 < -10)
        thac0 = -10 + int((thac0 + 10) / 2.0);

    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
    {
      sprintf(buf, "      Thac0 Reduced - %d\n\r", thac0);
      send_to_char(buf, ch);
    } 
  
    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
    {
      sprintf(buf, "Skill Rating %d\n\r", skill);
      send_to_char(buf, ch);
    }
  
    if (!str_cmp("warrior", class_table[ch->cClass].name))
        thac0 -= 5;
    thac0 -= int(GET_HITROLL (ch) * skill / 100.0);
    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
    {
       sprintf(buf, "Thac0 post Hit %d", thac0);
       send_to_char(buf, ch);
    }
    thac0 += int(5 * (100 - skill) / 100.0);
    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
    {
       sprintf(buf, "   Thac0 post Skill %d\n\r", thac0);
       send_to_char(buf, ch);
    }
    
    if (dt == gsn_backstab)
        thac0 -= int(20 * ( get_skill (ch, gsn_backstab)/100.0));

    if (dt == gsn_assassinate)
        thac0 -= int(20 * ( get_skill (ch, gsn_assassinate)/100.0));

    if (dt == gsn_strike)
        thac0 -= int(20 * ( get_skill (ch,gsn_strike)/100.0));
    
    
    switch (dam_type)
    {
        case (DAM_PIERCE):
            victim_ac = int(GET_AC (victim, AC_PIERCE) / 5.0);
            break;
        case (DAM_BASH):
            victim_ac = int(GET_AC (victim, AC_BASH) / 5.0);
            break;
        case (DAM_SLASH):
            victim_ac = int(GET_AC (victim, AC_SLASH) / 5.0);
            break;
        default:
            victim_ac = int(GET_AC (victim, AC_EXOTIC) / 5.0);
            break;
    };

    if (!IS_NPC(victim) && victim_ac < -100)
        victim_ac = (victim_ac + 100) / 5 - 100;
    else if (IS_NPC(victim) && victim_ac < -35)
       victim_ac = (victim_ac + 35) / 5 - 35;

    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
    {
      sprintf(buf, "Victim AC rating %d", victim_ac);
      send_to_char(buf, ch);
    }


    if (!can_see (ch, victim))
        victim_ac -= 20;

    if (victim->position < POS_FIGHTING)
        victim_ac += 12;

    if (victim->position < POS_RESTING)
        victim_ac += 20;

    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
    {
      sprintf(buf, "   Victim AC modif %d\b\r", victim_ac);
      send_to_char(buf, ch);
    }

    while ((diceroll = number_bits (5)) >= 20);
    if (diceroll == 0 || (diceroll < 18 && diceroll < thac0 - (victim_ac * 1.25)))
    {
      if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
      {
        sprintf(buf, "Dice %d < Thac0 %d - (AC * 1.25 = %4.2f) = %4.2f\n\r",
			diceroll, 
			thac0, victim_ac * 1.25,
			thac0 - (victim_ac * 1.25));
        send_to_char(buf, ch);
      }
        
        damage (ch, victim, 0, dt, dam_type, TRUE, FALSE);
        tail_chain ();
        return FALSE;
    }
    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
    {
      sprintf(buf, "Dice %d < Thac0 %d - (AC * 1.25 = %4.2f) = %4.2f\n\r",
			diceroll, 
			thac0, victim_ac * 1.25,
			thac0 - (victim_ac * 1.25));
      send_to_char(buf, ch);
    }
    

    return TRUE;
}


void one_hit (CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool secondary)
{
    OBJ_DATA *wield;
    int dam;
    int mar_msg=0;
    int diceroll;
    int sn, skill,mskill;
    int dam_type;
    int dam_min;
    int dam_max;
    bool result;
    bool CriticalHit = FALSE; 
    sn = -1;


    /* just in case */
    if (victim == ch || ch == NULL || victim == NULL)
        return;

    if (IS_NPC(ch) && ch->attacker == victim)
      return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
        return;    
    /*
     * Figure out the type of damage message.
     */
    if (!secondary)
      wield = get_eq_char (ch, WEAR_WIELD);
    else
      wield = get_eq_char (ch, WEAR_SECONDARY );
    
    mar_msg =number_range(0,(MAX_MAR_DAM_MSG-2));
    if (dt == TYPE_UNDEFINED)
    {
        if (wield != NULL && wield->item_type == ITEM_WEAPON)
            dt = TYPE_HIT + wield->value[3];
        else if (get_skill(ch,gsn_martial_arts)>0 && get_eq_char( ch, WEAR_WIELD)==NULL)
            dt = TYPE_HIT + mar_msg;
        else
            dt = TYPE_HIT + ch->dam_type;
    }

    if (dt < TYPE_HIT)
    {
        if (wield != NULL)
            dam_type = attack_table[wield->value[3]].damage;
        else
        {        
        if (get_skill(ch,gsn_martial_arts)>0)
            dam_type = mar_attack_table[mar_msg].damage;
        else
            dam_type = attack_table[ch->dam_type].damage;
        }
    }        
    else
        dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
        dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn (ch, secondary);
    skill = 20 + get_weapon_skill (ch, sn);

    if (!str_cmp("warrior", class_table[ch->cClass].name))
    {
      if (skill == 120 && number_percent() >= 97)
      {
      	 dt = 1045;
         CriticalHit = TRUE;
      }
      else
        CriticalHit = FALSE;
    }

    if (!calc_hit(ch,victim,secondary,dt,dam_type))
        return;

    /*
     * Hit.
     * Calc damage.
     */ 
    if (IS_NPC (ch) && (!ch->pIndexData->new_format || wield == NULL))
        if (!ch->pIndexData->new_format)
        {
            dam = number_range (ch->level / 2, ch->level * 3 / 2);
            if (wield != NULL)
                dam += dam / 2;
        }
        else
            dam = dice (ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE]);

    else
    {
        if (sn != -1)
            check_improve (ch, sn, TRUE, 5);
        if (wield != NULL)
        {
            if (wield->pIndexData->new_format)
            {
               dam = int(dice (wield->value[1], wield->value[2]) * skill / 100.0);
               if (dt == gsn_assassinate || dt == gsn_backstab)
               {
                 int tmp = int(dice (wield->value[1], wield->value[2]) * skill / 100.0);
                 if (tmp > dam)
                   dam = (tmp + dam) / 2;
               }
            }
            else
                dam = number_range (int(wield->value[1] * skill / 100.0),
                                    int(wield->value[2] * skill / 100.0));

            if (get_eq_char (ch, WEAR_SHIELD) == NULL)    /* no shield = more */
                dam = dam * 11 / 10;

            /* sharpness! */
            if (IS_WEAPON_STAT (wield, WEAPON_SHARP)
                && !IS_WEAPON_STAT (wield, WEAPON_VORPAL))
            {
                dam = int(dam * 1.1);
            }
            /* vorpal! */
            if (IS_WEAPON_STAT (wield, WEAPON_VORPAL)
                && !IS_WEAPON_STAT (wield, WEAPON_SHARP))
            {
            	dam = int(dam * 1.1);
            }
            if (IS_OBJ_STAT (wield, ITEM_TAINT)&& !furies_imm(ch,victim))
            {
            	dam = int(dam * 1.05);
            }    	
        }
		else if (get_skill(ch,gsn_martial_arts)>0
			 && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE - NEW_DAM_MSG 
			 && dt != gsn_strike
			 && get_eq_char( ch, WEAR_WIELD)==NULL)
			{
			mskill = get_skill(ch,gsn_martial_arts); 
			dam = number_range (int(mar_attack_table[dt - TYPE_HIT].mindam * mskill /100 + 2.5 * ch->level/4.0 * mskill /120.0),
				  int(mar_attack_table[dt - TYPE_HIT].maxdam * mskill /100.0 + 2.5 * ch->level / 3.0 * mskill /120.0));

		if (ch->fighting == victim && ch->pcdata->stance == STANCE_STRAW)
		{
			int realDam = 0;
            
			realDam = damage (ch, victim, dam/25, 0, DAM_NEGATIVE, FALSE, FALSE);
			if (!IS_SET(ch->act2, PLR_NODAMAGE))
			{
		  char shwn[MSL];
		  sprintf (shwn, "You draw life from %s. [`!%d`*]\n\r", 
			   PERS(victim, ch, FALSE), realDam);
		  send_to_char(shwn, ch);
		}	
			ch->move -= number_range(25,50);
			act ("$N gets his life sucked by $N.", ch, NULL, victim,
				 TO_NOTVICT);
			if (!IS_SET(victim->act2, PLR_NODAMAGE))
			  act ("You feel $N drawing your life away.",
				  victim, NULL, ch, TO_CHAR);
			if (((ch->level < LEVEL_HERO && !IS_DRAGON(ch) && !IS_FORSAKEN(ch))
				 || !IS_SET(ch->act2, PLR_NOEXP)) && !IS_SET(ch->act, PLR_QUESTING))
			  ch->alignment = UMAX (-1000, ch->alignment - 1);
			ch->hit += dam / 25;         
		}

        if (ch->fighting == victim && ch->pcdata->stance == STANCE_LEAF && dam > 0)
        {
            int level;
            AFFECT_DATA af;

                level = (ch->level);

            if (number_percent() > level)
            {
                send_to_char ("You feel poison coursing through your veins.\n\r",
                              victim);
                act ("$n is poisoned by the venom from $N.",
                     ch, NULL, victim, TO_NOTVICT);
		send_to_char("You poison your opponent with the Leaf dance.\n\r", ch);

                af.where = TO_AFFECTS;
                af.type = gsn_poison;
                af.level = level/2;
                af.duration = 1;
                af.location = APPLY_STR;
                af.modifier = -1;
                af.bitvector = AFF_POISON;
                affect_join (victim, &af);
            }
        }

		if (ch->fighting == victim && ch->pcdata->stance == STANCE_STONE && dam > 0)
		{
		  int realDam = 0;

			realDam = damage (ch, victim, dam/9, 0, DAM_FIRE, FALSE, FALSE);
			if (!IS_SET(ch->act2, PLR_NODAMAGE))
			{
		  char shwn[MSL];
		  sprintf (shwn, "%s is burned by you. [`!%d`*]\n\r", 
			   PERS(victim, ch, FALSE), realDam);
		  send_to_char(shwn, ch);
		}	
			act ("$n is burned by $N.", ch, NULL, victim,
				 TO_NOTVICT);
			if (!IS_SET(victim->act2, PLR_NODAMAGE))
			  act ("$N sears your flesh.", victim, NULL, ch, TO_CHAR);
			 fire_effect ((void *) victim, 0, dam, TARGET_CHAR);
		}

		if (ch->fighting == victim && ch->pcdata->stance == STANCE_WHIRLWIND && dam > 0)
		{
			int realDam = 0;
            
			realDam = damage (ch, victim, dam/9, 0, DAM_COLD, FALSE, FALSE);
			if (!IS_SET(ch->act2, PLR_NODAMAGE))
			{
		  char shwn[MSL];
		  sprintf (shwn, "%s is frozen by you. [`!%d`*]\n\r", 
			   PERS(victim, ch, FALSE),  realDam);
		  send_to_char(shwn, ch);
		}	
			act ("$n freezes $N.", ch, NULL, victim,
				 TO_NOTVICT);

			if (!IS_SET(victim->act2, PLR_NODAMAGE))
			  act ("The cold touch surrounds you with ice.",
				 victim, NULL, NULL, TO_CHAR);
			cold_effect (victim, 0, dam, TARGET_CHAR);
		}

		if (ch->fighting == victim && ch->pcdata->stance == STANCE_LIGHTNING && dam > 0)
		{
		int realDam = 0;
    
			realDam = damage (ch, victim, dam/9, 0, DAM_LIGHTNING, FALSE, FALSE);
			if (!IS_SET(ch->act2, PLR_NODAMAGE))
			{
		  char shwn[MSL];
		  sprintf (shwn, "%s is struck by lightning from you. [`!%d`*]\n\r", 
			   PERS(victim, ch, FALSE), realDam);
		  send_to_char(shwn, ch);
		}	
			act ("$N is struck by lightning.", ch, NULL, victim,
				 TO_NOTVICT);
			if (!IS_SET(victim->act2, PLR_NODAMAGE))
			   act ("You are shocked by $N.", victim, NULL, ch, TO_CHAR);
			shock_effect (victim, 0, dam, TARGET_CHAR);
		}

			}
		else if (get_skill(ch,gsn_martial_arts)>0
			 && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE - NEW_DAM_MSG 
			 && dt == gsn_strike
			 && get_eq_char( ch, WEAR_WIELD)==NULL)
			{
			mskill = get_skill(ch,gsn_strike); 
			dam = number_range (int(ch->level/4 * mskill /100.0 + 25 * mskill /100.0),int(ch->level/2 * mskill /100.0+30 * mskill /100.0));
			if (IS_AFFECTED(ch, AFF_STANCE) && !furies_imm(ch, victim))
			  dam = int(dam * 1.10); 
			}
		else
			dam = number_range (int(1 + 4 * skill / 100.0), int(2 * ch->level / 3.0 * skill / 100.0));        
    }
    
    /*
     * Bonuses.
     */
    if (get_skill (ch, gsn_enhanced_damage) > 0)
    {
        diceroll = number_percent ();
        if (diceroll <= get_skill (ch, gsn_enhanced_damage))
        {
            check_improve (ch, gsn_enhanced_damage, TRUE, 6);
            dam += int(2 * (dam * diceroll / 300.0));
        }
    }

    if (CriticalHit)
    {

      /*if ( IS_AFFECTED(ch, AFF_VEIL)&& !furies_imm(ch,victim) )
        dam *= DAMAGE_VEIL;
      else*/
        dam = int(dam * 2.25);
    }

    if (ch->interdimension > 0)
    {
	act("$n pops in to the normal dimension.", ch,0,0,TO_ROOM);
	stc("You pop in to the normal dimension.\n\r", ch);
	ch->interdimension = 0;
    }

    if (!IS_AWAKE (victim))
        dam *= 2;
    else if (victim->position < POS_FIGHTING)
        dam = dam * 3 / 2;

    if (dt == gsn_backstab && wield != NULL)
    {
        if (wield->value[0] != 2)
            dam = dam * (2 + (ch->level / 15));
        else
            dam = dam * (2 + (ch->level / 12));
    }

    if (dt == gsn_assassinate && wield != NULL)
    {
          if (wield->value[0] != 2)
             dam = int(dam * (5 + (ch->level / 10)));
          else
            dam = int(dam * (5 + (ch->level / 8)));
     
    }

    if (dt == gsn_strike && wield == NULL)
       dam = dam * (2 + (ch->level / 12));     

    if (dam > 8000
      && (dt == gsn_strike
      || dt == gsn_assassinate
      || dt == gsn_backstab))
    {
      dam = 8000;
    }

    dam += int(GET_DAMROLL (ch) * UMIN (100, skill) / 100.0);

    if (dam <= 0)
        dam = 1;
        
    if ( !check_counter( ch, victim, dam, dt) )
        result = damage( ch, victim, dam, dt, dam_type, TRUE, FALSE );
    else return;
    /*
    if (result)
    {
      if(!IS_NPC (victim))
      {
    	if (wield != NULL)
    	{
    	    if ((IS_OBJ_STAT(wield,ITEM_TAINT)
    	        && !furies_imm(ch,victim)
    	        && dt >= TYPE_HIT 
    	        && !IS_NPC(ch))
    	        && (number_percent()>30))
    	    
        }
      }
    } */
   /*Damage shields by Tamarae - (tamarae@zdnetonebox.com)*/
	
	if ( IS_AFFECTED( victim , AFF_SHOCKSHIELD ))
	{
	
	act("You are `#z`3apped`* by $N's crackling shield.",ch, NULL, victim, TO_CHAR);
	act("$n is `#z`3apped`* by your crackling shield!",ch,NULL,victim,TO_VICT);
	act("$N is `#z`3apped`* by $n's crackling shield!",victim,NULL,ch,TO_NOTVICT);
	/*Damage Mods*/
	dam_min = get_skill(ch,gsn_shockshield)/14;
	dam_min += wis_app[get_curr_stat (ch, STAT_WIS)].practice;
	dam_min += ch->level/10;
	dam_min +=4;
	dam_max = get_skill(ch,gsn_shockshield)/10;
	dam_max += wis_app[get_curr_stat (ch, STAT_WIS)].practice;
	dam_max += ch->level/4;
	dam_max +=4;
	dam = number_range( dam_min, dam_max);
        damage( victim, ch, dam, gsn_shockshield, DAM_LIGHTNING ,TRUE, FALSE);
	return;
	}

	if ( IS_AFFECTED( victim , AFF_ICESHIELD ))
	{
	
	act("You are `&chilled`* by $N's icy shield.",ch, NULL, victim, TO_CHAR);
	act("$n is `&chilled`* by your icy shield!",ch,NULL,victim,TO_VICT);
	act("$N is `&chilled`* by $n's icy shield!",victim, NULL,ch,TO_NOTVICT);

	dam_min = get_skill(ch,gsn_shockshield)/14;
	dam_min += wis_app[get_curr_stat (ch, STAT_WIS)].practice;
	dam_min += ch->level/10;
	dam_max = get_skill(ch,gsn_shockshield)/10;
	dam_max += wis_app[get_curr_stat (ch, STAT_WIS)].practice;
	dam_max += ch->level/4;	
	dam = number_range( dam_min, dam_max);
        damage( victim, ch, dam, gsn_iceshield, DAM_COLD ,TRUE, FALSE);
	return;
	}


	if ( IS_AFFECTED( victim , AFF_FIRESHIELD ))
	{
	
	act("You are `!b`1urned`* by $N's flaming shield.",ch, NULL, victim, TO_CHAR);
	act("$n is `!b`1urned`* by your flaming shield!",ch,NULL,victim,TO_VICT);
	act("$N is `!b`1urned`* by $n's flaming shield!",victim,NULL,ch,TO_NOTVICT);

	dam_min = get_skill(ch,gsn_shockshield)/14;
	dam_min += wis_app[get_curr_stat (ch, STAT_WIS)].practice;
	dam_min += ch->level/10;
	dam_min += 2;
	dam_max = get_skill(ch,gsn_shockshield)/10;
	dam_max += wis_app[get_curr_stat (ch, STAT_WIS)].practice;
	dam_max += ch->level/4;
	dam_max += 2;
	dam = number_range( dam_min, dam_max);
        damage( victim, ch, dam, gsn_fireshield, DAM_FIRE ,TRUE, FALSE);
	return;
	}


    /* but do we have a funky weapon? */
    if (result && wield != NULL)
    {
        int dam;

        if (ch->fighting == victim && IS_WEAPON_STAT (wield, WEAPON_POISON))
        {
            int level;
            AFFECT_DATA *poison, af;

            if ((poison = affect_find (wield->affected, gsn_poison)) == NULL)
                level = wield->level;
            else
                level = poison->level;

            if (!saves_spell (level / 2, victim, DAM_POISON))
            {
                send_to_char ("You feel poison coursing through your veins.",
                              victim);
                act ("$N is poisoned by the venom on $p.",
                     ch, wield, victim, TO_NOTVICT);

                af.where = TO_AFFECTS;
                af.type = gsn_poison;
                af.level = level * 3 / 4;
                af.duration = level / 2;
                af.location = APPLY_STR;
                af.modifier = -1;
                af.bitvector = AFF_POISON;
                affect_join (victim, &af);
            }

            /* weaken the poison if it's temporary */
            if (poison != NULL)
            {
                poison->level = UMAX (0, poison->level - 2);
                poison->duration = UMAX (0, poison->duration - 1);

                if (poison->level == 0 || poison->duration == 0)
                    act ("The poison on $p is wearing off.", ch, wield, NULL,
                         TO_CHAR);
            }
        }
/*
        if (ch->fighting == victim)
        {
            int level;
            AFFECT_DATA *contact, af;

            if ((contact = affect_find (wield->affected, gsn_contact)) == NULL)
                level = wield->level;
            else
                level = contact->level;

            
           
                send_to_char ("You feel poison coursing through your veins.",
                              victim);
                act ("$n is poisoned by the venom on $p.",
                     victim, wield, NULL, TO_ROOM);
if IS_AFFECTED(ch, AFF_CONTACT)
{
                af.where = TO_AFFECTS;
                af.type = gsn_contact;
                af.level = level;
                af.duration = level / 50;
                af.location = 0;
                af.modifier = 0;
                af.bitvector = AFF_CONTACT;
                affect_join (victim, &af);
}
else 
{               af.where = TO_AFFECTS;
                af.type = gsn_contact;
                af.level = level; 
                af.duration = level / 50;
                af.location = APPLY_SAVING_SPELL;
                af.modifier = +1;
                af.bitvector = AFF_CONTACT;
                affect_join (victim, &af);
}           

            // weaken the poison if it's temporary 
            if (contact != NULL)
            {
                contact->level = UMAX (0, contact->level - 2);
                contact->duration = UMAX (0, contact->duration - 1);

                if (contact->level == 0 || contact->duration == 0)
                    act ("The poison on $p is wearing off.", ch, wield, NULL,
                         TO_CHAR);
            }
        }*/

        if (ch->fighting == victim && (IS_WEAPON_STAT (wield, WEAPON_VAMPIRIC)))
        {
            int realDam = 0;
            dam = number_range (1, wield->level / 5 + 1);
            
            
            realDam = damage (ch, victim, dam, 0, DAM_NEGATIVE, FALSE, FALSE);
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
            {
	      char shwn[MSL];
	      sprintf (shwn, "Your %s draws life from %s. [`!%d`*]\n\r", 
		       wield->short_descr, PERS(victim, ch, FALSE), realDam);
	      send_to_char(shwn, ch);
	    }	
            act ("$N gets his life sucked by $p.", ch, wield, victim,
                 TO_NOTVICT);
            if (!IS_SET(victim->act2, PLR_NODAMAGE))
              act ("You feel $p drawing your life away.",
                  victim, wield, NULL, TO_CHAR);
            if (((ch->level < LEVEL_HERO && !IS_DRAGON(ch) && !IS_FORSAKEN(ch))
                 || !IS_SET(ch->act2, PLR_NOEXP)) && !IS_SET(ch->act, PLR_QUESTING))
              ch->alignment = UMAX (-1000, ch->alignment - 1);
            ch->hit += dam / 2;         
        }

        if (ch->fighting == victim && (IS_WEAPON_STAT (wield, WEAPON_FLAMING)))
        {
          int realDam = 0;
          dam = number_range (1, wield->level / 4 + 1);
            
            
            realDam = damage (ch, victim, dam, 0, DAM_FIRE, FALSE, FALSE);
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
            {
	      char shwn[MSL];
	      sprintf (shwn, "%s is burned by your %s. [`!%d`*]\n\r", 
		       PERS(victim, ch, FALSE), wield->short_descr, realDam);
	      send_to_char(shwn, ch);
	    }	
            act ("$N is burned by $p.", ch, wield, victim,
                 TO_NOTVICT);
            if (!IS_SET(victim->act2, PLR_NODAMAGE))
              act ("$p sears your flesh.", victim, wield, NULL, TO_CHAR);
             fire_effect ((void *) victim, wield->level / 2, dam, TARGET_CHAR);
        }

        if (ch->fighting == victim && (IS_WEAPON_STAT (wield, WEAPON_FROST)))
        {
            int realDam = 0;
            dam = number_range (1, wield->level / 6 + 2);

            
            realDam = damage (ch, victim, dam, 0, DAM_COLD, FALSE, FALSE);
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
            {
	      char shwn[MSL];
	      sprintf (shwn, "%s is frozen by your %s. [`!%d`*]\n\r", 
		       PERS(victim, ch, FALSE), wield->short_descr, realDam);
	      send_to_char(shwn, ch);
	    }	
            act ("$p freezes $N.", ch, wield, victim,
                 TO_NOTVICT);

            if (!IS_SET(victim->act2, PLR_NODAMAGE))
              act ("The cold touch of $p surrounds you with ice.",
                 victim, wield, NULL, TO_CHAR);
            cold_effect (victim, wield->level / 2, dam, TARGET_CHAR);
        }

        if (ch->fighting == victim && (IS_WEAPON_STAT (wield, WEAPON_SHOCKING)))
        {
	    int realDam = 0;
	    dam = number_range (1, wield->level / 5 + 2);

    
            realDam = damage (ch, victim, dam, 0, DAM_LIGHTNING, FALSE, FALSE);
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
            {
	      char shwn[MSL];
	      sprintf (shwn, "%s is struck by lightning from your %s. [`!%d`*]\n\r", 
		       PERS(victim, ch, FALSE), wield->short_descr, realDam);
	      send_to_char(shwn, ch);
	    }	
            act ("$N is struck by lightning from $p.", ch, wield, victim,
                 TO_NOTVICT);
            if (!IS_SET(victim->act2, PLR_NODAMAGE))
               act ("You are shocked by $p.", victim, wield, NULL, TO_CHAR);
            shock_effect (victim, wield->level / 2, dam, TARGET_CHAR);
        }
    }
    tail_chain ();
 //   damage_equipment(1);
    return;
}


/*
 * Inflict damage from a hit.
 */
int damage (CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int dam_type, bool show, bool arrow)
{
    OBJ_DATA *corpse;
    bool immune;
    char buf[MAX_STRING_LENGTH];

    if (victim->position == POS_DEAD)
        return 0;



	/*
     * Stop up any residual loopholes.
     */
    if (dam > 4200 && dt >= TYPE_HIT && !IS_NPC(ch))
    {
        bug ("Damage: %d: more than 4200 points!", dam);
        dam = 4200;
        if (!IS_IMMORTAL (ch))
        {
            OBJ_DATA *obj;
            obj = get_eq_char (ch, WEAR_WIELD);
            send_to_char ("You really shouldn't cheat.\n\r", ch);
            if (obj != NULL)
                extract_obj (obj);
        }

    }


    /* damage reduction */
    if (dam > 80)
        dam = int((dam - 80) / 2.1 + 80);

    if (dam > 250)
        dam = int((dam - 250) / 2.1 + 250);




    if (victim != ch && dt != gsn_poison)
    {
        /*
         * Certain attacks are forbidden.
         * Most other attacks are returned.
         */
        if (is_safe (ch, victim))
            return 0;
      
        if (victim->position > POS_STUNNED)
        {
            if (victim->fighting == NULL)
            {
                set_fighting (victim, ch);
                if (IS_NPC (victim) && HAS_TRIGGER_MOB (victim, TRIG_KILL))
                    p_percent_trigger (victim, NULL, NULL, ch, NULL, NULL, TRIG_KILL);
            }
            if (victim->timer <= 4)
                victim->position = POS_FIGHTING;
        }

        if (ch->position > POS_STUNNED)
        {
            if (ch->fighting == NULL)
                set_fighting (ch, victim);
        }

        /*
         * More charm stuff.
         */
        if (victim->master == ch)
            stop_follower (victim);
    }

    /*
     * Inviso attacks ... not.
     */
    if (IS_AFFECTED (ch, AFF_SUPER_INVIS))
    {
      STR_REMOVE_BIT(ch->affected_by, AFF_SUPER_INVIS);
    }

    if (IS_AFFECTED (ch, AFF_INVISIBLE) && (dt != gsn_poison || ch->in_room == victim->in_room))
    {
        affect_strip (ch, gsn_invis);
        affect_strip (ch, gsn_mass_invis);
        STR_REMOVE_BIT (ch->affected_by, AFF_INVISIBLE);
        act ("$n fades into existence.", ch, NULL, NULL, TO_ROOM);
    }

    if (!IS_NPC(ch) && !IS_NPC(victim) && victim != ch)
    {
      ch->pcdata->pk_timer = 120;
      ch->pcdata->safe_timer = 0;
      victim->pcdata->pk_timer = 120;
      victim->pcdata->safe_timer = 0;	
    }

    /*
     * Damage modifiers.
     */

    if (dam > 1 && !IS_NPC (victim)
        && victim->pcdata->condition[COND_DRUNK] > 10)
        dam = 9 * dam / 10;

    if (dam > 1 && IS_AFFECTED (victim, AFF_SANCTUARY) && dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam /= 2;

    if (!str_cmp("warrior", class_table[victim->cClass].name) && victim->pcdata->stance == STANCE_DEFENSIVE)
	dam /= 3;

    if (!str_cmp("warrior", class_table[victim->cClass].name) && victim->pcdata->stance == STANCE_OFFENSIVE)
    {
	if (!str_cmp("form master", class_table[ch->cClass].name))
	        dam /= (int) 1.75;
	else
		dam /= (int) 1.35;
    }

    if (dam > 1 && IS_AFFECTED (victim, AFF_IRONSKIN)&& victim->level > 70&& dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam =int( dam/1.7);
    else if (dam > 1 && IS_AFFECTED (victim, AFF_IRONSKIN)&& victim->level > 60 && victim->level <= 70&& dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam = int(dam/1.6);
    else if (dam > 1 && IS_AFFECTED (victim, AFF_IRONSKIN)&& victim->level > 40 && victim->level  <= 60&& dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam = int(dam/1.5);
    else if (dam > 1 && IS_AFFECTED (victim, AFF_IRONSKIN)&& victim->level > 20 &&  victim->level <= 40&& dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam = int(dam/1.4);
    else if (dam > 1 && IS_AFFECTED (victim, AFF_IRONSKIN)&& victim->level <= 20&& dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam = int(dam/1.3);

    if (dam > 1 && ((IS_AFFECTED (victim, AFF_PROTECT_EVIL) && IS_EVIL (ch))
                    || (IS_AFFECTED (victim, AFF_PROTECT_GOOD)
                        && IS_GOOD (ch))))
        dam -= dam / 4;
    if (dam > 1 && IS_AFFECTED (victim, AFF_FIRESHIELD))
        dam -= dam/ 10;
    if (dam > 1 && IS_AFFECTED (victim, AFF_ICESHIELD))
        dam -= dam/ 12;
    if (dam > 1 && IS_AFFECTED (victim, AFF_SHOCKSHIELD))
        dam -= dam/ 14;
    
    immune = FALSE;

    if ( IS_AFFECTED(ch, AFF_VEIL)&& !furies_imm(ch,victim))
    {
      double tmp = dam * DAMAGE_VEIL;//Defined in Merc
      dam = int(tmp);
    }

    if ( IS_AFFECTED(ch, AFF_STANCE) && (dt > TYPE_HIT || dt == gsn_assassinate || dt == gsn_backstab) 
            && !furies_imm(ch,victim) )
    {
      dam = int(dam * 1.10);
    }

    if ( IS_MOUNTED(ch) && !IS_MOUNTED(victim) && dt > TYPE_HIT)
    {
      dam = int(dam * 1.10);
    }
    
    /* Jasin
    if ((get_eq_wear(ch, WEAR_WIELD) && get_eq_wear(ch, WEAR_WIELD)->item_type == WEAPON_POLEARM) && IS_MOUNTED(ch))
    {
      dam = int(dam * 1.15);
    }
    */

    if (!IS_NPC(ch) && IS_BONDED(ch) && !furies_imm(ch,victim))
    {
      CHAR_DATA *bch;
      bool bondInRoom = FALSE;
      int door;
      EXIT_DATA *pExit;

      for (bch = ch->in_room->people; bch != NULL; bch = bch->next_in_room)
      {
        if (can_see(ch, bch) && strstr(ch->pcdata->bond, bch->name) &&
               bch->fighting != NULL)
          bondInRoom = TRUE;
      }

      for (door = 0; door < 6; door++)
      {
        if ((pExit = ch->in_room->exit[door]) != NULL)
        {
          for (bch = pExit->u1.to_room->people; bch != NULL; bch = bch->next_in_room)
          {
            if (can_see(ch, bch) && strstr(ch->pcdata->bond, bch->name) &&
                bch->fighting != NULL)
               bondInRoom = TRUE;
          }
        }
      } 

      if (bondInRoom)
      {
        if ((ch->clan == clan_lookup("aessedai") || ch->clan == clan_lookup("Whitetower"))  && dt < TYPE_HIT)
        {
          dam = int(dam * 1.25);
        }             
        else if (ch->clan == clan_lookup("Warder") && dt >= TYPE_HIT)
        {
          dam = int(dam * 1.25);
        } 
      }
    }           

    if (!IS_NPC(ch) && !IS_NPC(victim))
    {
      double tmp = dam * DAMAGE_REDUCER;
      dam = int(tmp);
    }

    /*
     * Check for parry, and dodge.
     */
    if (dt >= TYPE_HIT && ch != victim && dt < TYPE_SPECIAL && dam != 0)
    {   
    	int random = number_range(1,3);
    	switch(random)
    	{
    	    case 1:
                if (check_parry (ch, victim,arrow))
                   return FALSE;
                if (check_dodge (ch, victim,arrow))
                   return FALSE;
                if (check_shield_block (ch, victim,arrow))
                   return FALSE;
                if (!IS_NPC(victim))
                {
                if (check_evade (ch, victim,arrow))
                   return FALSE;
                if (check_block (ch,victim,arrow))
                   return FALSE;
                if (check_air (ch,victim,arrow))
                   return FALSE;
                }
                break;
            case 2:
               if (check_dodge (ch, victim,arrow))
                  return FALSE;
               if (check_shield_block (ch, victim,arrow))
                  return FALSE;
               if (check_parry (ch, victim,arrow))
                  return FALSE;
               if (!IS_NPC(victim))
                {
                if (check_evade (ch, victim,arrow))
                   return FALSE;
                if (check_block (ch,victim,arrow))
                   return FALSE;
                if (check_air (ch,victim,arrow))
                   return FALSE;
                }
                break;
            case 3:
               if (check_shield_block (ch, victim,arrow))
                  return FALSE;
               if (check_parry (ch, victim,arrow))
                  return FALSE;
               if (check_dodge (ch, victim,arrow))
                  return FALSE;
               if (!IS_NPC(victim))
                {
                if (check_evade (ch, victim,arrow))
                   return FALSE;
                if (check_block (ch,victim,arrow))
                   return FALSE;
                if (check_air (ch,victim,arrow))
                   return FALSE;
                }
                break;
            default:
                if (check_parry (ch, victim,arrow))
                   return FALSE;
                if (check_dodge (ch, victim,arrow))
                   return FALSE;
                if (check_shield_block (ch, victim,arrow))
                   return FALSE;
                if (!IS_NPC(victim))
                {
                if (check_evade (ch, victim,arrow))
                   return FALSE;
                if (check_block (ch,victim,arrow))
                   return FALSE;
                if (check_air (ch,victim,arrow))
                   return FALSE;
                }
                break;
        }        
    }
 
    if (dt < TYPE_HIT && IS_AFFECTED(ch, AFF_FOCUS)
       && dt != gsn_assassinate
       && dt != gsn_strike 
       && dt != gsn_backstab
       && dt != gsn_bash
       && dt != gsn_trip
       && dt != gsn_kick
       && dt != gsn_dirt)
    {
     if IS_AFFECTED(ch, AFF_VEIL)
     {
      dam = int((dam * 1.30 * (get_skill(ch,gsn_focus) / 100.0) + (ch->level / 200.0)) * 0.80);
     }
    
      dam = int(dam * 1.30 * (get_skill(ch,gsn_focus) / 100.0) + (ch->level / 200.0));
    }

    if (dt < TYPE_HIT && IS_AFFECTED(victim, AFF_DAMPEN)
       && dt != gsn_assassinate
       && dt != gsn_strike 
       && dt != gsn_backstab
       && dt != gsn_bash
       && dt != gsn_trip
       && dt != gsn_kick
       && dt != gsn_dirt)
    {
      dam = int( dam / 1.10);
    }


    if ( number_percent() < (get_skill(ch, gsn_luck) / 7) && dam != 0 && ch != victim && !furies_imm(ch,victim))
    {
    if (ch->clan == clan_lookup("Redhand"))
      {
       
     
      send_to_char("You get lucky!\n\r", ch);
      dam = int(dam * 2.25);
      check_improve(ch, gsn_luck, TRUE, 1);
   }

    }

    if ( number_percent() < (get_skill(ch, gsn_hunt) / 7) && dam != 0 && ch != victim && !furies_imm(ch,victim))
    {
      if (ch->clan == clan_lookup("Wolfbrother"))
      {   
     
      
      if (IS_EVIL(victim) && !IS_EVIL(ch))
        dam = int(dam  * 1.75);
      send_to_char("You hunt!\n\r", ch);
      dam = int(dam * 1.5);
      check_improve(ch, gsn_hunt, TRUE, 1);
   }    
}

    if ( ((number_percent() < get_skill(ch, gsn_balefire) / 4) && !furies_imm(ch,victim)) 
      && dam != 0 
      && ch != victim 
      && (dt < TYPE_HIT) )
    if ((ch->clan == clan_lookup("aessedai") && (ch->sex == 2)))
    {
      send_to_char("You feel yourself grabbing more of the One Power than usual.\n\r", ch);
      act("$N is seared by a blast of flame", ch, NULL, victim, TO_CHAR);
      dam = int(dam * 1.5);
      check_improve(ch, gsn_balefire, TRUE, 1);
    }

    if ( IS_AFFECTED(victim, AFF_PRAY) && dam > 0 && ch != victim && 
         !furies_imm(ch, victim))
    {
      int tempdam = 0;
      int chance;
      chance = number_percent();
      if (chance > 40)
      {
        tempdam = int(dam * .60);
        dam = int(dam * .8);
        send_to_char("Shaitan comes to your defense and reflects some of your damage.\n\r", 
                  victim);
        damage(victim, ch, tempdam, 1044, DAM_ENERGY, TRUE, FALSE);
      }
    }
    //Stop 0 damages COLD!! unless immune && miss && less than max skill
    //if (dam <= 0 && dt < TYPE_HIT && dt > MAX_SKILL)
    //    dam=1;
        
    switch (check_immune (victim, dam_type))
    {
        case (IS_IMMUNE):
            immune = TRUE;
            dam = 0;
            break;
        case (IS_RESISTANT):
            dam -= dam / 3;
            break;
        case (IS_VULNERABLE):
            dam += dam / 2;
            break;
    }

    if (show)
        dam_message (ch, victim, dam, dt, immune);

    if (dam == 0)
        return FALSE;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    if (IS_AFFECTED(victim,AFF_MANASHIELD))
    {
	if (victim->level <= 75)
	{
    		victim->mana -= int(dam * .15);
    	        victim->hit  -= int(dam * .7);
	}
	else
	{
                victim->mana -= int(dam * .4);
                victim->hit  -= int(dam * .6);
	}
    	if (victim->mana < 50 || victim->mana < victim->wimpy)
    	{
    	    STR_REMOVE_BIT (victim->affected_by, AFF_MANASHIELD);
            send_to_char("The protective weaves around you vanishes.", victim);
        }
    }
    else
    victim->hit -= dam;
    
    if (!IS_NPC (victim)
        && victim->level >= LEVEL_IMMORTAL && victim->hit < 1)
        victim->hit = 1;
    update_pos (victim);

    switch (victim->position)
    {
        case POS_MORTAL:
            act ("$n is mortally wounded, and will die soon, if not aided.",
                 victim, NULL, NULL, TO_ROOM);
            send_to_char
                ("You are mortally wounded, and will die soon, if not aided.\n\r",
                 victim);
            break;

        case POS_INCAP:
            act ("$n is incapacitated and will slowly die, if not aided.",
                 victim, NULL, NULL, TO_ROOM);
            send_to_char
                ("You are incapacitated and will slowly die, if not aided.\n\r",
                 victim);
            break;

        case POS_STUNNED:
            act ("$n is stunned, but will probably recover.",
                 victim, NULL, NULL, TO_ROOM);
            send_to_char ("You are stunned, but will probably recover.\n\r",
                          victim);
            break;

        case POS_DEAD:
            act ("$n is DEAD!!", victim, 0, 0, TO_ROOM);
            send_to_char ("You have been KILLED!!\n\r\n\r", victim);
            break;

        default:
           if (dam > victim->max_hit / 4)
                send_to_char ("That really did HURT!\n\r", victim);
		if ( victim->hit < (victim->max_hit * 1/3)
		     && !IS_NPC(victim))
		     if (victim->pcdata->condition[COND_BLEEDING]==0)
	        {
	            send_to_char ("You sure are `1B`!leeding`*!!\n\r",victim);
		    SET_BLEED(victim,TRUE);
	        }  
            break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if (!IS_AWAKE (victim))
        stop_fighting (victim, FALSE);

    /*
     * Payoff for killing things.
     */
    if (victim->position == POS_DEAD)
    {
        group_gain (ch, victim);
        
     if (!IS_SET(ch->act2, PLR_TOURNEY))
     {
        if (!IS_NPC (ch) && !IS_NPC (victim))
        {
 
         if ((victim->honor == 0) && (ch->honor > 0))
         {
          send_to_char("This was an honorless kill.\n\r", ch);
         }
         else
         {
         send_to_char("You have lost honor.\n\r", victim);
         victim->honor--;
         send_to_char("You have gained in honor.\n\r", ch);
         ch->honor++;
         if (IS_FORSAKEN(ch) || IS_DRAGON(ch))
	 {
		int prac_gain = number_range(1,5);
		ch->practice += prac_gain;
	 	sprintf(buf, "`1You receive `!%d`1 practices!`7\n\r", prac_gain);
		send_to_char(buf, ch);
         }
	 else
	 {
                int prac_gain = number_range(5,15);
                ch->practice += prac_gain;
                sprintf(buf, "`1You receive `!%d`1 practices!`7\n\r", prac_gain);
                send_to_char(buf, ch);
	 }
       }
     } 
   }        if (!IS_NPC (victim)) 
        {
            if (!IS_NPC(ch) && !IS_SET(victim->act, PLR_TOURNEY))
	    {
              sprintf (log_buf, "%s (lvl %d) killed by %s (lvl %d) at %ld",
                     victim->name,
		     victim->level,
                     (IS_NPC (ch) ? ch->short_descr : ch->name),
		     ch->level,
                     ch->in_room->vnum);
              log_special (log_buf, PK_TYPE);
            }
            /*
             * Dying penalty:
             * 2/3 way back to previous level.
             */
            if (victim->exp > exp_per_level (victim, victim->pcdata->points)
                * victim->level && 
                 !IS_SET(victim->act, PLR_QUESTING) &&
                 !IS_SET(victim->act, PLR_TOURNEY) &&
                 !IS_SET(victim->act, PLR_IT) &&
                 !IS_SET(victim->act, PLR_TAG) &&
                 !IS_SET(victim->act2, PLR_WAR))
                 
                gain_exp (victim,                          (2 *
                           (exp_per_level (victim, victim->pcdata->points) *
                            victim->level - victim->exp) / 3) + 50);

	    if (ch->level > 75 &&
                 !IS_SET(victim->act, PLR_QUESTING) &&
                 !IS_SET(victim->act, PLR_TOURNEY) &&
                 !IS_SET(victim->act, PLR_IT) &&
                 !IS_SET(victim->act, PLR_TAG) &&
                 !IS_SET(victim->act2, PLR_WAR))
                gain_exp (victim,                          (2 *
                           (exp_per_level (victim, victim->pcdata->points) *
                            victim->level - victim->exp) / 3) + 50);

        }

        sprintf (log_buf, "%s got toasted by %s at %s [room %ld]",
                 (IS_NPC (victim) ? victim->short_descr : victim->name),
                 (IS_NPC (ch) ? ch->short_descr : ch->name),
                 ch->in_room->name, ch->in_room->vnum);

        if (IS_NPC (victim))
            wiznet (log_buf, NULL, NULL, WIZ_MOBDEATHS, 0, 0);
        else
            wiznet (log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);

        /*
         * Death trigger
         */
        if (IS_NPC (victim) && HAS_TRIGGER_MOB (victim, TRIG_DEATH))
        {
            victim->position = POS_STANDING;
            p_percent_trigger (victim, NULL, NULL, ch,  NULL, NULL, TRIG_DEATH);
        }
        if (!IS_NPC(victim) && !IS_NPC(ch))
          victim->pcdata->lastKilled = str_dup(ch->name);
       

    if (IS_SET(victim->act, PLR_IT))
    {
    	REMOVE_BIT(victim->act, PLR_IT);
    	SET_BIT(victim->act, PLR_TAG);
    	REMOVE_BIT(ch->act, PLR_TAG);
    	SET_BIT(ch->act, PLR_IT);
    	sprintf (log_buf, "%s has lost the IT flag to %s!",
    	victim->name,
    	ch->name);
    	do_echo( ch, log_buf);
    }	                    

        raw_kill (ch,victim);
     

        /* dump the flags */
        
        /* RT new auto commands */

        if (!IS_NPC (ch)
            && (corpse =
                get_obj_list (ch, "corpse", ch->in_room->contents)) != NULL
            && corpse->item_type == ITEM_CORPSE_NPC
            && can_see_obj (ch, corpse))
        {
            OBJ_DATA *coins;

            corpse = get_obj_list (ch, "corpse", ch->in_room->contents);

            if (IS_SET (ch->act, PLR_AUTOLOOT) && corpse && corpse->contains)
            {                    /* exists and not empty */
                do_function (ch, &do_get, "all corpse");
            }

            if (IS_SET (ch->act, PLR_AUTOGOLD) && corpse && corpse->contains &&    /* exists and not empty */
                !IS_SET (ch->act, PLR_AUTOLOOT))
            {
                if ((coins = get_obj_list (ch, "gcash", corpse->contains))
                    != NULL)
                {
                    do_function (ch, &do_get, "all.gcash corpse");
                }
            }

            if (IS_SET (ch->act, PLR_AUTOSAC))
            {
                if (IS_SET (ch->act, PLR_AUTOLOOT) && corpse
                    && corpse->contains)
                {
		    if (!IS_NPC(victim))
		      victim->pcdata->damReceived += dam;
                    if (!IS_NPC(ch))
		      ch->pcdata->damInflicted += dam;
                    return dam;    /* leave if corpse has treasure */
                }
                else
                {
                    do_function (ch, &do_sacrifice, "corpse");
                }
            }
  	}
        if (!IS_NPC(victim))
          victim->pcdata->damReceived += dam;
        if (!IS_NPC(ch))
	  ch->pcdata->damInflicted += dam;
        return dam;
    }

    if (victim == ch)
    {
        if (!IS_NPC(victim))
          victim->pcdata->damReceived += dam;
        if (!IS_NPC(ch))
	  ch->pcdata->damInflicted += dam;
        return dam;
    }
    /*
     * Take care of link dead people.
     */
    if (!IS_NPC (victim) && victim->desc == NULL)
    {
        if (number_range (0, victim->wait) == 0)
        {
            do_function (victim, &do_recall, "");
            if (!IS_NPC(victim))
              victim->pcdata->damReceived += dam;
            if (!IS_NPC(ch))
	       ch->pcdata->damInflicted += dam;
            return dam;
        }
    }

    /*
     * Wimp out?
     */
    if (IS_NPC (victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2)
    {
        if ((IS_SET (victim->act, ACT_WIMPY) && number_bits (2) == 0
             && victim->hit < victim->max_hit / 5)
            || (IS_AFFECTED (victim, AFF_CHARM) && victim->master != NULL
                && victim->master->in_room != victim->in_room))
        {
            do_function (victim, &do_flee, "");
        }
    }

    if (!IS_NPC (victim)
        && victim->hit > 0
        && victim->hit <= victim->wimpy && victim->wait < PULSE_VIOLENCE / 2)
    {
        do_function (victim, &do_flee, "");
    }

    tail_chain ();
    if (!IS_NPC(victim))
      victim->pcdata->damReceived += dam;
    if (!IS_NPC(ch))
      ch->pcdata->damInflicted += dam;

    return dam;
}

bool is_safe (CHAR_DATA * ch, CHAR_DATA * victim)
{
    char buf [MSL];
    if (victim->in_room == NULL || ch->in_room == NULL)
        return TRUE;

    if (victim->fighting == ch || victim == ch)
       return FALSE;

    if (ch->level > LEVEL_IMMORTAL)
       return FALSE;       

    if (ch->clan == clan_lookup("Guide") || victim->clan == clan_lookup("Guide")) // Hardcoded for speed sake
    {
      send_to_char("Violence is not becoming of a guide member.\n\r", ch);
      return TRUE;
    }

    /* killing mobiles */
    if (IS_NPC (victim))
    {

        /* safe room? */
        if (IS_SET (victim->in_room->room_flags, ROOM_SAFE) 
        && !IS_SET(victim->act, PLR_TOURNEY) 
        && !IS_SET(victim->act, PLR_TAG)
	&& !IS_SET(victim->act, PLR_QUESTING) 
        && !IS_SET(victim->act, PLR_IT) 
        && !IS_SET(victim->act2, PLR_WAR) 
        && !IS_MURDERER(victim)
        && !IS_THIEF(victim))
        {
            send_to_char ("Not in this room.\n\r", ch);
            return TRUE;
        }

        if (victim->pIndexData->pShop != NULL)
        {
            send_to_char ("The shopkeeper wouldn't like that.\n\r", ch);
            return TRUE;
        }

        /* no killing healers, trainers, etc */
        if (IS_SET (victim->act, ACT_TRAIN)
            || IS_SET (victim->act, ACT_PRACTICE)
            || IS_SET (victim->act, ACT_IS_HEALER)
            || IS_SET (victim->act, ACT_IS_CHANGER))
        {
            send_to_char ("I don't think The Creator would approve.\n\r", ch);
            return TRUE;
        }

        if (!IS_NPC (ch))
        {
            /* no pets */
            if (IS_SET (victim->in_room->room_flags, ROOM_SAFE) && !IS_SET(victim->act2, PLR_WAR))
            {
              send_to_char ("Not in this room.\n\r", ch);
              return TRUE;
            }

	    if (IS_SET(ch->act, PLR_TOURNEY) && !IS_SET(victim->act, ACT_GUILDMOB) && 
			!IS_SET(victim->act, ACT_MOUNT))
            {
	      send_to_char("Tourney is for killing players, not mobs.\n\r", ch);
              return TRUE;
            }	

            if (IS_SET (victim->act, ACT_PET))
            {
                act ("But $N looks so cute and cuddly...",
                     ch, NULL, victim, TO_CHAR);
                return TRUE;
            }

            /* no charmed creatures unless owner */
            if (IS_AFFECTED (victim, AFF_CHARM) && ch != victim->master &&
		!IS_SET(victim->act, ACT_GUILDMOB) && !IS_SET(victim->act, ACT_MOUNT))
            {
                send_to_char ("You don't own that monster.\n\r", ch);
                return TRUE;
            }

        }
    }
    /* killing players */
    else
    {
        /* NPC doing the killing */
        if (IS_NPC (ch))
        {
            /* safe room check */
            if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
            {
                send_to_char ("Not in this room.\n\r", ch);
                return TRUE;
            }

            /* charmed mobs and pets cannot attack players while owned */
            if (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL
                && ch->master->fighting != victim)
            {
                send_to_char ("Players are your friends!\n\r", ch);
                return TRUE;
            }
        }
        /* player doing the killing */
        else
        {
            /* safe room check */
            if (IS_SET (victim->in_room->room_flags, ROOM_SAFE) && (!IS_SET(victim->act2, PLR_WAR)) && victim->pcdata->pk_timer <= 0 &&
                 !IS_MURDERER(victim) && !IS_THIEF(victim))
            {
                send_to_char ("Not in this room.\n\r", ch);
                return TRUE;
            }

	    if (IS_SET(ch->act, PLR_TOURNEY) && 
                IS_SET(victim->act, PLR_TOURNEY) &&
		(ch->pcdata->tournament.last_killed == victim->id ||
		victim->pcdata->tournament.last_killed == ch->id))
            {
		send_to_char("It's a tourney attack someone else.\n\r", ch);
		return TRUE;
            }  

            if (ch->pcdata->wait_timer > 0)
            {
               send_to_char("You need to play a while before attempting to pk.\n\r", ch);
               return TRUE;
            }

            if (IS_SET(victim->act, PLR_TOURNEY) && !IS_SET(ch->act, PLR_TOURNEY))
            {
              send_to_char("Not while they are in the tournament.\n\r", ch);
              return TRUE;
            }
            if (IS_SET(victim->act2, PLR_WAR) && !IS_SET(ch->act2, PLR_WAR))
            {
              send_to_char("Not while they are in the war.\n\r", ch);
              return TRUE;
            }
            
	    if (!IS_SET(victim->act, PLR_TOURNEY) && IS_SET(ch->act, PLR_TOURNEY))
            {
              send_to_char("Not while you're in the tournament.\n\r", ch);
              return TRUE;
            }
            if (!IS_SET(victim->act2, PLR_WAR) && IS_SET(ch->act2, PLR_WAR))
            {
              send_to_char("Not while you're in the war.\n\r", ch);
              return TRUE;
            }

            if (IS_SET(victim->act, PLR_QUESTING) && !IS_SET(ch->act, PLR_QUESTING))
            {
              send_to_char("Not while they are questing.\n\r", ch);
              return TRUE;
            }
                        if (!IS_SET(victim->act, PLR_QUESTING) && IS_SET(ch->act, PLR_QUESTING))
            {
              send_to_char("Not while you're questing.\n\r", ch);
              return TRUE;
            }

            if (((victim->pcdata->safe_timer > 0) || (ch->pcdata->safe_timer > 0)) && (!IS_SET(victim->act,
PLR_TOURNEY) && (!IS_SET(victim->act, PLR_TAG) && (!IS_SET(victim->act, PLR_IT)))))
            {
	       send_to_char("Give them a chance to catch their breath.\n\r", ch);
               return TRUE;
            }
     
            if (ch->pcdata->safe_timer > 0)
            {
               send_to_char("You cannot attack someone recently after dieing\n\r", ch);
               return TRUE;
            }
   
            if (!str_cmp("Tinker", clan_table[victim->clan].name)&&
            (!IS_SET(victim->act, PLR_TOURNEY)) && 
            (!IS_SET(victim->act, PLR_QUESTING)) && 
            (!IS_SET(victim->act, PLR_TAG)) &&
            (!IS_SET(victim->act, PLR_IT)))
            {
              send_to_char("You can't bring yourself to do harm to a Tinker.\n\r", ch); 
              return TRUE;
            }
	    if (victim->timer > 10)
            {
	      send_to_char("It's not nice to kill daydreamers.\n\r", ch);
	      return TRUE;
	    }
            if (!IS_NPC(victim) && IS_SET(victim->act, PLR_TOURNEY) && victim->fighting &&
		victim->fighting != ch->pet)
            {
              send_to_char("One at a time during tourney please!\n\r", ch);
              return TRUE;
            } 

            if (!str_cmp("Tinker", clan_table[ch->clan].name) &&
            (!IS_SET(ch->act, PLR_TOURNEY)) && 
            (!IS_SET(ch->act, PLR_QUESTING)) && 
            (!IS_SET(ch->act, PLR_TAG)) &&
            (!IS_SET(ch->act, PLR_IT)))  
            {  
              send_to_char("As a Tinker you could never harm another!\n\r", ch);
              return TRUE;
            } 

	    if (IS_SET(victim->act, PLR_NOPK))
	    {
	      send_to_char("They aren't pkable right now.\n\r", ch);	
	      return TRUE;
	    }	

	    if (IS_SET(ch->act, PLR_NOPK))
	    {
	      send_to_char("You have to turn off the nopk flag first.\n\r", ch);	
	      return TRUE;
	    }	
        }
    }

    if (!IS_NPC(victim)&&!IS_NPC(ch) && ch != victim 
	    && !IS_SET(ch->act, PLR_TOURNEY) && !IS_SET(ch->act, PLR_QUESTING)
	    && ch->id != victim->pcdata->initiator)
    {
    	    sprintf(buf,"%s (lvl %d) initiated attack on %s (lvl %d)",
    	        ch->name,ch->level,victim->name,victim->level);
            log_special(buf, PK_TYPE);
            victim->pcdata->initiator = ch->id;
    }
    
    return FALSE;
}

bool is_safe_silent (CHAR_DATA * ch, CHAR_DATA * victim)
{
    char buf [MSL];
    if (victim->in_room == NULL || ch->in_room == NULL)
        return TRUE;

    if (victim->fighting == ch || victim == ch)
       return FALSE;

    if (ch->level > LEVEL_IMMORTAL)
       return FALSE;       


    /* killing mobiles */
    if (IS_NPC (victim))
    {

        /* safe room? */
        if (IS_SET (victim->in_room->room_flags, ROOM_SAFE) &&
(!IS_SET(victim->act, PLR_TOURNEY) && (!IS_SET(victim->act, PLR_TAG) &&
(!IS_SET(victim->act, PLR_IT) && (!IS_SET(victim->act2, PLR_WAR))))))
        {
            return TRUE;
        }

        if (victim->pIndexData->pShop != NULL)
        {
            return TRUE;
        }

        /* no killing healers, trainers, etc */
        if (IS_SET (victim->act, ACT_TRAIN)
            || IS_SET (victim->act, ACT_PRACTICE)
            || IS_SET (victim->act, ACT_IS_HEALER)
            || IS_SET (victim->act, ACT_IS_CHANGER))
        {
            return TRUE;
        }

        if (!IS_NPC (ch))
        {
            /* no pets */
            if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
            {
              return TRUE;
            }

            if (IS_SET (victim->act, ACT_PET))
            {
                return TRUE;
            }

            /* no charmed creatures unless owner */
            if (IS_AFFECTED (victim, AFF_CHARM) && ch != victim->master)
            {
                return TRUE;
            }
        }
    }
    /* killing players */
    else
    {
        /* NPC doing the killing */
        if (IS_NPC (ch))
        {
            /* safe room check */
            if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
            {
                return TRUE;
            }

            /* charmed mobs and pets cannot attack players while owned */
            if (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL
                && ch->master->fighting != victim)
            {
                return TRUE;
            }
        }
        /* player doing the killing */
        else
        {
            /* safe room check */
            if (IS_SET (victim->in_room->room_flags, ROOM_SAFE) && victim->pcdata->pk_timer <= 0)
           {            
                return TRUE;
            }
            if (IS_SET(victim->act, PLR_TOURNEY) && !IS_SET(ch->act, PLR_TOURNEY))
            {
              return TRUE;
            }
                        if (!IS_SET(victim->act, PLR_TOURNEY) && IS_SET(ch->act, PLR_TOURNEY))
            {
              return TRUE;
            }

            if (IS_SET(victim->act, PLR_QUESTING) && !IS_SET(ch->act, PLR_QUESTING))
            {
              return TRUE;
            }
                        if (!IS_SET(victim->act, PLR_QUESTING) && IS_SET(ch->act, PLR_QUESTING))
            {
              return TRUE;
            }

            if ((victim->pcdata->safe_timer > 0) && (!IS_SET(victim->act,
PLR_TOURNEY) && (!IS_SET(victim->act, PLR_TAG) && (!IS_SET(victim->act,
PLR_IT) && (!IS_SET(victim->act2, PLR_WAR))))))
            {
               return TRUE;
            }
    
            if (ch->pcdata->safe_timer > 0)
            {
               return TRUE;
            }

            if (!str_cmp("Tinker", clan_table[victim->clan].name)&&
            (!IS_SET(victim->act, PLR_TOURNEY)) && 
            (!IS_SET(victim->act, PLR_QUESTING)) && 
            (!IS_SET(victim->act, PLR_TAG)) &&
            (!IS_SET(victim->act, PLR_IT)))
            {
              return TRUE;
            }

            if (!str_cmp("Tinker", clan_table[ch->clan].name) &&
            (!IS_SET(ch->act, PLR_TOURNEY)) && 
            (!IS_SET(ch->act, PLR_QUESTING)) && 
            (!IS_SET(ch->act, PLR_TAG)) &&
            (!IS_SET(ch->act, PLR_IT)))  
            {  
              return TRUE;
            } 

	    if (IS_SET(victim->act, PLR_NOPK))
	    {
	      return TRUE;
	    }	

	    if (IS_SET(ch->act, PLR_NOPK))
	    {
	      return TRUE;
	    }	

        }
    }

    if (!IS_NPC(victim)&&!IS_NPC(ch) && ch != victim 
	    && !IS_SET(ch->act, PLR_TOURNEY) && !IS_SET(ch->act, PLR_QUESTING)
	    && ch->id != victim->pcdata->initiator)
    	{
    	    sprintf(buf,"%s (lvl %d) initiated attack on %s (lvl %d)",
    	        ch->name,ch->level,victim->name,victim->level);
            log_special(buf, PK_TYPE);
            victim->pcdata->initiator = ch->id;
    	}
        
    return FALSE;
}



bool is_safe_spell (CHAR_DATA * ch, CHAR_DATA * victim, bool area)
{
    char buf [MSL];
    if (victim->in_room == NULL || ch->in_room == NULL)
        return TRUE;

    if (victim == ch && area)
        return TRUE;

    if (IS_AFFECTED(victim, AFF_SUPER_INVIS) && !IS_SET(ch->act, PLR_HOLYLIGHT))
      return TRUE;

    if (victim->fighting == ch || victim == ch)
       return FALSE;

    if (ch->level > LEVEL_IMMORTAL)
       return FALSE;       

    if (ch->clan == clan_lookup("Guide") || victim->clan == clan_lookup("Guide")) // Hardcoded for speed sake
    {
      return TRUE;
    }

    /* killing mobiles */
    if (IS_NPC (victim))
    {
        /* safe room? */
        if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
            return TRUE;

        if (victim->pIndexData->pShop != NULL)
            return TRUE;

        /* no killing healers, trainers, etc */
        if (IS_SET (victim->act, ACT_TRAIN)
            || IS_SET (victim->act, ACT_PRACTICE)
            || IS_SET (victim->act, ACT_IS_HEALER)
            || IS_SET (victim->act, ACT_IS_CHANGER))
            return TRUE;

        if (!IS_NPC (ch))
        {
            /* no pets */
            if (IS_SET (victim->act, ACT_PET))
                return TRUE;
	
	    if (IS_SET (victim->act, ACT_MOUNT) && is_same_group(ch, victim->leader))
	       return TRUE;

            if (IS_SET(ch->act, PLR_TOURNEY) && !IS_SET(victim->act, ACT_GUILDMOB) &&
			!IS_SET(victim->act, ACT_MOUNT))
            {
              send_to_char("Tourney is for killing players, not mobs.\n\r", ch);
              return TRUE;
            }

            /* no charmed creatures unless owner */
            if (IS_AFFECTED (victim, AFF_CHARM)
                && (area || ch != victim->master) &&
		!IS_SET(victim->act, ACT_GUILDMOB) && !IS_SET(victim->act, ACT_MOUNT))
                return TRUE;

            /* legal kill? -- cannot hit mob fighting non-group member */
            if (victim->fighting != NULL
                && !is_same_group (ch, victim->fighting)) return TRUE;
        }
        else
        {
            /* area effect spells do not hit other mobs */
            if (area && !is_same_group (victim, ch->fighting))
                return TRUE;
            
        }
    }
    /* killing players */
    else
    {
        if (area && IS_IMMORTAL (victim) && victim->level > LEVEL_IMMORTAL)
            return TRUE;

        /* NPC doing the killing */
        if (IS_NPC (ch))
        {
            /* charmed mobs and pets cannot attack players while owned */
            if (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL
                && ch->master->fighting != victim)
                return TRUE;

            /* safe room? */
            if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
                return TRUE;

            /* legal kill? -- mobs only hit players grouped with opponent */
            if (ch->fighting != NULL && !is_same_group (ch->fighting, victim))
                return TRUE;
        }

        /* player doing the killing */
        else
        {
	    if (IS_SET(ch->act, PLR_TOURNEY) && 
                IS_SET(victim->act, PLR_TOURNEY) &&
		(ch->pcdata->tournament.last_killed == victim->id ||
		victim->pcdata->tournament.last_killed == ch->id))
            {
		send_to_char("It's a tourney attack someone else.\n\r", ch);
		return TRUE;
            }  

            if (is_same_group(ch, victim))
                return TRUE;

            if (ch->pcdata->wait_timer > 0)
            {
               send_to_char("You need to play a while before attempting to pk.\n\r", ch);
               return TRUE;
            }

            if ((victim->clan == clan_lookup("Tinker")) &&
              (!IS_SET(victim->act, PLR_TOURNEY)) && 
              (!IS_SET(victim->act, PLR_QUESTING)) && 
              (!IS_SET(victim->act, PLR_TAG)) &&
              (!IS_SET(victim->act, PLR_IT)))
		return TRUE;

            if (!IS_NPC(victim) && IS_SET(victim->act, PLR_TOURNEY) && victim->fighting &&
		victim->fighting != ch->pet)
            {
              return TRUE;
            } 

	    if (IS_SET(victim->act, PLR_NOPK) || IS_SET(ch->act, PLR_NOPK))
	    {
	      return TRUE;
	    }

        }

        }
	
	if (!IS_NPC(victim)&&!IS_NPC(ch) && ch != victim 
	    && !IS_SET(ch->act, PLR_TOURNEY) && !IS_SET(ch->act, PLR_QUESTING)
	    && ch->id != victim->pcdata->initiator)
    	{
    	    sprintf(buf,"%s (lvl %d) initiated attack on %s (lvl %d)",
    	        ch->name,ch->level,victim->name,victim->level);
            log_special(buf, PK_TYPE);
            victim->pcdata->initiator = ch->id;
    	}
        
    return FALSE;
}

//Adjusting for hitdam and skill of weapon Range is modifiend in each individual function of dodge parry shield etc.
bool defence_chance (CHAR_DATA *ch,CHAR_DATA *victim, bool defense, bool arrow,int gsn)
{   
    OBJ_DATA * obj;
    char buf [MSL];
    int chance;
    int skill;
    int adj_chance;
    int diff;
    int sn;
    int diceroll;
    chance = 0;
    if (gsn == gsn_parry)
    {
    	chance = int(get_skill (victim, gsn_parry) / 2.0);
    	if ((obj = get_eq_char (victim, WEAR_WIELD)) == NULL)
        {
            if (IS_NPC (victim))
                chance /= 2;
            else
                return FALSE; 
        }
        if ((obj = get_eq_char (victim, WEAR_SECONDARY)) != NULL)
        {
            chance = (int)((double)chance * 1.2);
        }
    }    
    if (gsn == gsn_dodge)
        chance = get_skill (victim, gsn_dodge) / 3;
    
    if (gsn == gsn_shield_block)
        chance = get_skill (victim, gsn_shield_block) / 4;
    
    if (gsn == (sn = skill_lookup("air barrier")))      	
        chance = get_skill (victim, sn) / 6;
    
    
    if (gsn == gsn_evade)
        chance = get_skill (victim, gsn_evade) / 2;
    
    if (gsn == gsn_block)   	
        chance = get_skill (victim, gsn_block) / 3;
        
 
    skill = get_weapon_skill(victim,get_weapon_sn(ch,FALSE));//check to see if victim is proficient in the attackers weapon(unarmed should be accounted for in get_weapon_skill
    if (IS_NPC(victim))
    {
       diff = victim->level - ch->level;       
       if (diff > 20)
          diff = int(20 + ((victim->level - ch->level)*.05));
       if (diff < -20)
          diff = int(-20 + ((victim->level - ch->level)*.05));
       adj_chance = diff ;
    }
    else
        adj_chance = 2*(victim->level - ch->level);//Player Level difference
    if (!IS_NPC(ch)&&!IS_NPC(victim))
    {
        adj_chance += (get_curr_stat(victim,STAT_DEX) - 
                       get_curr_stat(ch,STAT_DEX)) / 2;
        adj_chance += GET_HITROLL(victim)/100;//account for hitdam of victim (hitdam is already accounted for in the chance to hit the victim

// This is where FM and Warrior are getting the big difference in 
// parry/dodge levels    
/*        if (defense && skill<=0)
            adj_chance -= get_weapon_skill(ch,get_weapon_sn(ch,FALSE))/10;//if not proficient then not so likely to defend
        else if (defense && skill>0)
            adj_chance += skill/10;//account for weapon skill (checks for skill on victim(person on defence against) the attackers weapon)
*/
    }
    if (IS_SET (ch->off_flags, OFF_FAST) 
        || (IS_AFFECTED (victim, AFF_HASTE)
        && !IS_AFFECTED (ch, AFF_HASTE)))
        chance += 10;
    if (IS_SET (victim->off_flags, OFF_FAST)
        || (IS_AFFECTED (victim, AFF_HASTE)
        && !IS_AFFECTED (ch, AFF_HASTE)))
        chance -= 10;
    if (class_table[victim->cClass].fMana==FMANA_CHANNEL)//check to see if class is a chanelling class   
        adj_chance -= 5;
    
    if (IS_MOUNTED(ch) && !IS_MOUNTED(victim) && defense)   
        adj_chance -= int(chance * .05); // If horseback riding a little harder to parry/dodge/block/etc
    
    if (ch->clan == clan_lookup("Whitetower") && victim->clan != clan_lookup("Whitetower") && !furies_imm(ch,victim))
        adj_chance -= int(chance * .2);

    if (!str_cmp("warrior", class_table[victim->cClass].name) && get_eq_char( victim, WEAR_WIELD)==NULL
        && str_cmp("warrior", class_table[ch->cClass].name) && get_eq_char( ch, WEAR_WIELD)==NULL)
        adj_chance += int(chance *.05);
    
    if ( (victim->clan == clan_lookup("Whitecloak") && IS_AFFECTED(victim, AFF_STANCE)
        && ch->clan != clan_lookup("Whitecloak") && IS_AFFECTED(ch, AFF_STANCE))
        && !furies_imm(ch,victim)
        && (victim->clan != clan_lookup("Whitetower"))
        && defense)
        adj_chance += int(chance * .10);
    
    if ( (!IS_NPC(victim) && (get_skill(victim,gsn_luck)>0))
        &&(!IS_NPC(ch) && (get_skill(ch,gsn_luck)==0)))
        adj_chance += int(chance *.3);
        
        
    if (!can_see (victim, ch))
        chance = (chance + adj_chance )/(int)1.4;
    else
        chance = chance + adj_chance;   

    if (chance > 80)//Cut off point
        chance = 80;
    if (chance <= 0)
        chance = 1;
    if (arrow)
    {
    	if (gsn == gsn_parry && chance > 5)
    	    chance = 5;
    	if (gsn != gsn_shield_block && gsn != gsn_parry)
    	    chance /=2;
    }
    while ((diceroll = number_bits (5)) >= 20);
    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
    {
    	sprintf(buf,"Defence %s:  diceroll %d < chance %d Arrow:%s\n\r",skill_table[gsn].name,diceroll,chance/5,arrow?"True":"False");
    	send_to_char(buf,ch);
    }
    
    if ((diceroll==20)||(diceroll > chance/5))    	
        return FALSE;
    
    return TRUE;
	
}
bool calc_chance (CHAR_DATA * ch,CHAR_DATA * victim,int gsn,bool strength)
{
    int chance=0;
    int skill;
    int adjustment;
    int diceroll;
    int diff;
    char buf[MSL];
    
    skill = int(get_skill(ch,gsn) *.80);
    if (!IS_AWAKE(ch))
        return TRUE;
    

    if (IS_NPC(victim))
    {
       diff = ch->level - victim->level;
       if (diff > 20)
          diff = int(20 + ((ch->level - victim->level)*.05));
       if (diff < -20)
          diff = int(-20 + ((ch->level - victim->level)*.05));
       adjustment = diff ;
    }
    else
        adjustment = 2 * (ch->level - victim->level);//Player Level difference
    
    if (!IS_NPC(ch)&&!IS_NPC(victim))
    {
        adjustment += 2 * (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX));
        adjustment += GET_HITROLL(ch)/100;//account for hitdam of victim (hitdam is already accounted for in the chance to hit the victim
        if (strength)
            adjustment += 2 * (get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_STR));
        adjustment += 2 * (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX));
        

}

    if (IS_SET (ch->off_flags, OFF_FAST) 
        || (IS_AFFECTED (victim, AFF_HASTE)
        && !IS_AFFECTED (ch, AFF_HASTE)))
        chance += 10;
    if (IS_SET (victim->off_flags, OFF_FAST)
        || (IS_AFFECTED (victim, AFF_HASTE)
        && !IS_AFFECTED (ch, AFF_HASTE)))
        chance -= 10;

    if (!str_cmp("warrior", class_table[ch->cClass].name))
        chance += 8;

    if (class_table[victim->cClass].fMana==FMANA_CHANNEL)//check to see if class is a chanelling class   
        adjustment += 5;
    
    
    if (IS_MOUNTED(ch) && !IS_MOUNTED(victim))   
        adjustment += int(skill * .05); 
    
    
    if (ch->clan == clan_lookup("Whitetower") && victim->clan != clan_lookup("Whitetower") && !furies_imm(ch,victim))
        adjustment += int(skill * .2);


    if (!str_cmp("warrior", class_table[ch->cClass].name) && get_eq_char( ch, WEAR_WIELD)==NULL
        && str_cmp("warrior", class_table[victim->cClass].name) && get_eq_char( victim, WEAR_WIELD)==NULL)
        adjustment += int(skill *.05);
    
    if ( (ch->clan == clan_lookup("Whitecloak") && IS_AFFECTED(ch, AFF_STANCE)
        && victim->clan != clan_lookup("Whitecloak") && IS_AFFECTED(victim, AFF_STANCE))        
        && (victim->clan != clan_lookup("Whitetower")))
        adjustment += int(skill * .10);
    
    if ( (!IS_NPC(ch) && (get_skill(ch,gsn_luck)>0))
        &&(!IS_NPC(victim) && (get_skill(victim,gsn_luck)==0)))
        adjustment += int(skill *.3);
    
        if (IS_NPC(victim)&& (gsn == gsn_backstab || gsn == gsn_assassinate || gsn == gsn_strike))
            adjustment += 15;
        if (gsn == gsn_dirt)
            switch (ch->in_room->sector_type)
            {
                case (SECT_INSIDE):
                    adjustment -= 15;
                    break;
                case (SECT_CITY):
                    adjustment -= 10;
                    break;
                case (SECT_FIELD):
                    adjustment += 5;
                    break;
                case (SECT_FOREST):
                    break;
                case (SECT_HILLS):
                    break;
                case (SECT_MOUNTAIN):
                    adjustment -= 10;
                    break;
                case (SECT_WATER_SWIM):
                    adjustment = 0;
                    break;
                case (SECT_WATER_NOSWIM):
                    adjustment = 0;
                    break;
                case (SECT_AIR):
                    adjustment = 0;
                    break;
                case (SECT_DESERT):
                    adjustment += 10;
                    break;
            }
            
        if (gsn == gsn_bash)
        {
            adjustment += ch->carry_weight / 250;
            adjustment -= victim->carry_weight / 200;

            if (ch->size < victim->size)
                adjustment += (ch->size - victim->size) * 15;
            else
                adjustment += (ch->size - victim->size) * 10;
        }
        if (gsn == gsn_flip)
        {
            adjustment += ch->carry_weight / 250;
            adjustment -= victim->carry_weight / 200;

            if (ch->size < victim->size)
                adjustment += (ch->size - victim->size) * 15;
            else
                adjustment += (ch->size - victim->size) * 10;
        }
        if (gsn==gsn_berserk)
        {
            /* fighting */
            if (ch->position == POS_FIGHTING)
                adjustment += 10;

            /* damage -- below 50% of hp helps, above hurts */
            adjustment += 25 - (100 * ch->hit / ch->max_hit) / 2;
        }
        if (gsn==gsn_disarm)
        {
             OBJ_DATA * obj;
             
             adjustment -= 40;
             /* find weapon skills */

             if ((obj = get_eq_char (victim, WEAR_WIELD)) == NULL)
                
             if (IS_OBJ_STAT (obj, ITEM_NOREMOVE))
                 adjustment -= 20;
             
             if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
             {
    	         sprintf(buf,"adjchance %d\n\r",adjustment);
    	         send_to_char(buf,ch);
    }
             
             
        }
        if(gsn == gsn_trip)
        {
             if (ch->size < victim->size)
                 adjustment += (ch->size - victim->size) * 10;    /* bigger = harder to trip */
        }
        /*if (gsn == gsn_assassinate)
            adjustment -= 10;
    
        if (gsn == gsn_backstab)
            adjustment -= 5;
        */    
        if (gsn == gsn_blackjack)
            adjustment -= 20;

             
    
     
    if (!can_see (ch, victim))
        chance = (skill + adjustment )/2;
    else
        chance = skill + adjustment;
    
    if (chance > 90)//Cut off point
        chance = 90;
    
    while ((diceroll = number_bits (5)) >= 20);
    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_TESTOR))
    {
    	sprintf(buf,"diceroll %d < chance %d\n\r",diceroll,chance/5);
    	send_to_char(buf,ch);
    }
    
    if ((diceroll==20)||(diceroll > chance/5))    	
        return FALSE;
    
    return TRUE;
}



/*
 * Check for parry.
 */
 //victim is the person parrying
 //ch is the attacker
 //chance is the % where the victim parries the attack GREATER= parry LESS= hit
bool check_parry (CHAR_DATA * ch, CHAR_DATA * victim,bool arrow)
{
    int break_chance=3;
    OBJ_DATA *obj;
    OBJ_DATA *weapon;
    bool found = TRUE;

    if (!IS_AWAKE (victim))
        return FALSE;
    if (!IS_NPC(victim)&&get_skill (victim,gsn_parry)<1)
        return FALSE;

    if ((obj = get_eq_char (victim, WEAR_WIELD)) == NULL)
       found = FALSE;
    
    if ((weapon = get_eq_char (ch,WEAR_WIELD)) != NULL)
    {
    	if (IS_WEAPON_STAT(weapon,WEAPON_SHARP))
    	    break_chance +=7;
    	if (IS_WEAPON_STAT(weapon,WEAPON_VORPAL))
    	    break_chance +=17;
    }
    if (found)
    {
      if ((number_percent() < break_chance) && (!IS_OBJ_STAT(obj, ITEM_NOBREAK)) &&
           !IS_SET(victim->act, PLR_TOURNEY) )
         obj->condition -= 1;
      if (obj->condition <= 0)
      {
        act("`!$n destroys your weapon!`*", ch, NULL, victim, TO_VICT); 
        act( "`!You destroy $N's weapon!`*", ch, NULL, victim, TO_CHAR);
        unequip_char(victim, obj);
        return FALSE;
      }
    }
    
        
    //Actual Check for Parry
    if (!defence_chance(ch,victim,TRUE,arrow,gsn_parry))
        return FALSE;

    act ("You parry $n's attack.", ch, NULL, victim, TO_VICT);
    act ("$N parries your attack.", ch, NULL, victim, TO_CHAR);
    check_improve (victim, gsn_parry, TRUE, 6);
    return TRUE;
}

/*
 * Check for shield block.
 */
 //victim is the person block
 //ch is the attacker
 //chance is the % where the victim block the attack GREATER= block LESS= hit
bool check_shield_block (CHAR_DATA * ch, CHAR_DATA * victim,bool arrow)
{
    
    int break_chance=3;
    //OBJ_DATA *bow;
    OBJ_DATA *weapon;
    OBJ_DATA *obj;

    if (!IS_AWAKE (victim))
        return FALSE;
    if (!IS_NPC(victim)&&get_skill (victim,gsn_shield_block)<1)
        return FALSE;
   
    if ((obj = get_eq_char (victim, WEAR_SHIELD)) == NULL)
        return FALSE;
        
    if ((weapon = get_eq_char (ch,WEAR_WIELD)) != NULL)
    {
    	if (IS_WEAPON_STAT(weapon,WEAPON_SHARP))
    	    break_chance +=7;
    	if (IS_WEAPON_STAT(weapon,WEAPON_VORPAL))
    	    break_chance +=17;
    }

    if ((number_percent() < break_chance) && (!IS_OBJ_STAT(obj, ITEM_NOBREAK))
         && !IS_SET(victim->act, PLR_TOURNEY))
       obj->condition -= 1;

    if (obj->condition <= 0)
    {
      act("`!$n destroys your shield!`*", ch, NULL, victim, TO_VICT); 
      act( "`!You destroy $N's shield!`*", ch, NULL, victim, TO_CHAR);
      unequip_char(victim, obj);
    }
    //Actual Check for Shield Block
    if (!defence_chance(ch,victim,TRUE,arrow,gsn_shield_block))
        return FALSE;

    act ("You block $n's attack with your shield.", ch, NULL, victim,
         TO_VICT);
    act ("$N blocks your attack with a shield.", ch, NULL, victim, TO_CHAR);
    check_improve (victim, gsn_shield_block, TRUE, 6);
    return TRUE;
}

bool check_air (CHAR_DATA * ch, CHAR_DATA * victim,bool arrow)
{
    int sn;
    if (!IS_AWAKE (victim))
        return FALSE;
    if (!IS_NPC(victim) && !IS_AFFECTED(victim, AFF_AIRBARRIER))
        return FALSE;
    sn = skill_lookup("air barrier");
    //Actual IF check
    if (!defence_chance(ch,victim,TRUE,arrow,sn))
        return FALSE;

    send_to_char ("Your attack is repulsed by a barrier of air!\n\r",ch);
    act ("Your weave of air holds off $n's attack!",ch,NULL,victim, TO_VICT);

    return TRUE;
}


/*
 * Check for dodge.
 */
 //victim is the person dodging
 //ch is the attacker
 //chance is the % where the victim dodges the attack GREATER= miss LESS= hit
bool check_dodge (CHAR_DATA * ch, CHAR_DATA * victim,bool arrow)
{
    
    if (!IS_AWAKE (victim))
        return FALSE;
    if (!IS_NPC(victim)&&get_skill (victim,gsn_dodge)<1)
        return FALSE;
        
    //Actual IF check
    if (!defence_chance(ch,victim,TRUE,arrow,gsn_dodge))
        return FALSE;

    act ("You dodge $n's attack.", ch, NULL, victim, TO_VICT);
    act ("$N dodges your attack.", ch, NULL, victim, TO_CHAR);
    check_improve (victim, gsn_dodge, TRUE, 6);
    return TRUE;
}
/*
 * Check for evade
 */
 //victim is the person evading
 //ch is the attacker
 //chance is the % where the victim evades the attack GREATER= miss LESS= hit
bool check_evade (CHAR_DATA * ch, CHAR_DATA * victim,bool arrow)
{

    if (!IS_AWAKE (victim))
        return FALSE;
    if (!IS_NPC(victim)&&get_skill (victim,gsn_evade)<1)
        return FALSE;
        
    //Actual IF check
    if (!defence_chance(ch,victim,TRUE,arrow,gsn_evade))
        return FALSE;

    act ("You evade $n's attack.", ch, NULL, victim, TO_VICT);
    act ("$N evades your attack.", ch, NULL, victim, TO_CHAR);
    check_improve (victim, gsn_evade, TRUE, 6);
    return TRUE;
}
/*
 * Check for block
 */
 //victim is the person blocking
 //ch is the attacker
 //chance is the % where the victim blocks the attack GREATER= miss LESS= hit
bool check_block (CHAR_DATA * ch, CHAR_DATA * victim,bool arrow)
{

    if (!IS_AWAKE (victim))
        return FALSE;
    if (!IS_NPC(victim)&&get_skill (victim,gsn_block)<1)
        return FALSE; 
  
    //Actual IF check
    if (!defence_chance(ch,victim,TRUE,arrow,gsn_block))
        return FALSE;

    act ("You block $n's attack.", ch, NULL, victim, TO_VICT);
    act ("$N blocks your attack.", ch, NULL, victim, TO_CHAR);
    check_improve (victim, gsn_block, TRUE, 6);
    return TRUE;
}


/*
 * Set position of a victim.
 */
void update_pos (CHAR_DATA * victim)
{
    if (victim->hit > 0)
    {
        if (victim->position <= POS_STUNNED)
            victim->position = POS_STANDING;
        return;
    }

    if (IS_NPC (victim) && victim->hit < 1)
    {
        victim->position = POS_DEAD;
        return;
    }

    if (victim->hit <= -11)
    {
        victim->position = POS_DEAD;
        return;
    }

    if (victim->hit <= -6)
        victim->position = POS_MORTAL;
    else if (victim->hit <= -3)
        victim->position = POS_INCAP;
    else
        victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting (CHAR_DATA * ch, CHAR_DATA * victim)
{
    if (ch->fighting != NULL)
    {
        bug ("Set_fighting: already fighting", 0);
        return;
    }

    if (IS_AFFECTED (ch, AFF_SLEEP))
        affect_strip (ch, gsn_sleep);
    if (IS_AFFECTED (ch, AFF_BLACKJACK))
        affect_strip (ch, gsn_blackjack);

    ch->fighting = victim;
    ch->position = POS_FIGHTING;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting (CHAR_DATA * ch, bool fBoth)
{
    CHAR_DATA *fch;

    for (fch = char_list; fch != NULL; fch = fch->next)
    {
        if (fch == ch || (fBoth && fch->fighting == ch))
        {
            fch->fighting = NULL;
            fch->position = IS_NPC (fch) ? fch->default_pos : POS_STANDING;
            update_pos (fch);
            if (!IS_NPC(fch) && IS_SET(fch->act2, PLR_MSP_MUSIC))
            {
		fch->pcdata->aggro_timer = 6; 
            }
        }
    }
    
    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse (CHAR_DATA * ch, CHAR_DATA * killer)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;


    if (IS_NPC (ch))
    {
        name = ch->short_descr;
        corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_NPC), 0);
        corpse->timer = number_range (3, 6);
        if (ch->gold >= 0)
        {
            obj_to_obj (create_money (ch->gold, ch->silver), corpse);
            ch->gold = 0;
            ch->silver = 0;
        }
        corpse->cost = 0;
    }
    else
    {
    	if (IS_DISGUISED(ch))
	    REM_DISGUISE(ch);
        name = ch->name;
        corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_PC), 0);
        corpse->timer = number_range (25, 40);
        corpse->owner = str_dup (ch->name);
	if (ch->pcdata->lastKilled && *(ch->pcdata->lastKilled))
	{
	  corpse->lastKilled = str_dup (ch->pcdata->lastKilled);
	  
          if (IS_SET(ch->act2, PLR_LOOTABLE) && IS_SET(killer->act2, PLR_LOOTABLE))
          {
          if (IS_SET(ch->act2, PLR_WAR))
	  {
	    corpse->lootsLeft = REGULAR_LOOT;   
          }
	  
	  if (((killer = get_char_world_special(ch->pcdata->lastKilled)) != NULL)
	      && (IS_DRAGON(killer) || IS_FORSAKEN(killer)))  
	  {
	    corpse->lootsLeft = ENHANCED_LOOT;
	  }

	  else
          {   
            corpse->lootsLeft = OTHER_LOOT;
          }
        }
}
	if (ch->gold > 1 || ch->silver > 1)
        {
           obj_to_obj (create_money (ch->gold / 2, ch->silver / 2),
                            corpse);
           ch->gold -= ch->gold / 2;
           ch->silver -= ch->silver / 2;
        }

        corpse->cost = 0;
    }

    corpse->level = ch->level;

    sprintf (buf, corpse->short_descr, name);
    free_string (corpse->short_descr);
    corpse->short_descr = str_dup (buf);

    sprintf (buf, corpse->description, name);
    free_string (corpse->description);
    corpse->description = str_dup (buf);

    for (obj = ch->carrying; obj != NULL; obj = obj_next)
    {
        bool floating = FALSE;

        obj_next = obj->next_content;
        if (obj->wear_loc == WEAR_FLOAT)
            floating = TRUE;
        obj_from_char (obj);
        if (obj->item_type == ITEM_POTION)
            obj->timer = number_range (500, 1000);
        if (obj->item_type == ITEM_SCROLL)
            obj->timer = number_range (1000, 2500);
        if (IS_SET (obj->extra_flags, ITEM_ROT_DEATH) && !floating)
        {
            obj->timer = number_range (5, 10);
            REMOVE_BIT (obj->extra_flags, ITEM_ROT_DEATH);
        }
        REMOVE_BIT (obj->extra_flags, ITEM_VIS_DEATH);

        if (IS_SET (obj->extra_flags, ITEM_INVENTORY))
            extract_obj (obj);
      /*  else if (floating)
        {
            if (IS_OBJ_STAT (obj, ITEM_ROT_DEATH))
            {                    
                if (obj->contains != NULL)
                {
                    OBJ_DATA *in, *in_next;

                    act ("$p evaporates,scattering its contents.",
                         ch, obj, NULL, TO_ROOM);
                    for (in = obj->contains; in != NULL; in = in_next)
                    {
                        in_next = in->next_content;
                        obj_from_obj (in);
                        obj_to_room (in, ch->in_room);
                    }
                }
                else
                    act ("$p evaporates.", ch, obj, NULL, TO_ROOM);
                extract_obj (obj);
            }
            else
            {
                act ("$p falls to the floor.", ch, obj, NULL, TO_ROOM);
                obj_to_room (obj, ch->in_room);
            }
        }*/

        else
            obj_to_obj (obj, corpse);
    }
    
    obj_to_room (corpse, ch->in_room);
    /*if (corpse->item_type==ITEM_CORPSE_PC)
    {
        add_corpse (corpse);
        //del_corpse (get_corpse_data(corpse));
        save_corpses ();
    }*/
    return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry (CHAR_DATA * ch)
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    long vnum;

    vnum = 0;
    msg = "You hear $n's death cry.";

    switch (number_bits (4))
    {
        case 0:
            msg = "$n hits the ground ... DEAD.";
            break;
        case 1:
            if (ch->material == 0)
            {
                msg = "$n splatters blood on your armor.";
                break;
            }
        case 2:
            if (IS_SET (ch->parts, PART_GUTS))
            {
                msg = "$n spills $s guts all over the floor.";
                vnum = OBJ_VNUM_GUTS;
            }
            break;
        case 3:
            if (IS_SET (ch->parts, PART_HEAD))
            {
                msg = "$n's severed head plops on the ground.";
                vnum = OBJ_VNUM_SEVERED_HEAD;
            }
            break;
        case 4:
            if (IS_SET (ch->parts, PART_HEART))
            {
                msg = "$n's heart is torn from $s chest.";
                vnum = OBJ_VNUM_TORN_HEART;
            }
            break;
        case 5:
            if (IS_SET (ch->parts, PART_ARMS))
            {
                msg = "$n's arm is sliced from $s dead body.";
                vnum = OBJ_VNUM_SLICED_ARM;
            }
            break;
        case 6:
            if (IS_SET (ch->parts, PART_LEGS))
            {
                msg = "$n's leg is sliced from $s dead body.";
                vnum = OBJ_VNUM_SLICED_LEG;
            }
            break;
        case 7:
            if (IS_SET (ch->parts, PART_BRAINS))
            {
                msg =
                    "$n's head is shattered, and $s brains splash all over you.";
                vnum = OBJ_VNUM_BRAINS;
            }
    }

    act (msg, ch, NULL, NULL, TO_ROOM);

    if (vnum != 0)
    {
        char buf[MAX_STRING_LENGTH];
        OBJ_DATA *obj;
        char *name;

        name = IS_NPC (ch) ? ch->short_descr : ch->name;
        obj = create_object (get_obj_index (vnum), 0);
        obj->timer = number_range (4, 7);

        sprintf (buf, obj->short_descr, name);
        free_string (obj->short_descr);
        obj->short_descr = str_dup (buf);

        sprintf (buf, obj->description, name);
        free_string (obj->description);
        obj->description = str_dup (buf);

        if (obj->item_type == ITEM_FOOD)
        {
            if (IS_SET (ch->form, FORM_POISON))
                obj->value[3] = 1;
            else if (!IS_SET (ch->form, FORM_EDIBLE))
                obj->item_type = ITEM_TRASH;
        }

        obj_to_room (obj, ch->in_room);
    }

    if (IS_NPC (ch))
        msg = "You hear something's death cry.";
    else
        msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for (door = 0; door <= 5; door++)
    {
        EXIT_DATA *pexit;

        if ((pexit = was_in_room->exit[door]) != NULL
            && pexit->u1.to_room != NULL && pexit->u1.to_room != was_in_room)
        {
            ch->in_room = pexit->u1.to_room;
            act (msg, ch, NULL, NULL, TO_ROOM);
        }
    }
    ch->in_room = was_in_room;

    return;
}

void float_drop(CHAR_DATA *ch)
{
  OBJ_DATA *obj, *obj_next;

    for (obj = ch->carrying; obj != NULL; obj = obj_next)
    {
        obj_next = obj->next_content;
        if (obj->wear_loc == WEAR_FLOAT && IS_OBJ_STAT (obj, ITEM_FLOATDROP))
        {
	  act ("$p falls to the floor.", ch, obj, NULL, TO_ROOM);
          obj_from_char(obj);
	  obj_to_room(obj, ch->in_room);
        }
    }
}

void raw_kill (CHAR_DATA *ch,CHAR_DATA * victim)
{
    int i;
    int penalty;
    stop_fighting (victim, TRUE);
    stop_fighting (ch, TRUE);

/*
    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_MSP_SOUND))
    {
      send_to_char("!!SOUND(Sounds/DeathC.wav V=100 L=1 P=70 T=Death)\n\r", ch);
    } 
*/

    if (!IS_NPC(ch) && IS_NPC(victim) && !getbit(ch->pcdata->mobbed, victim->pIndexData->vnum))
    {
      setbit( ch->pcdata->mobbed, victim->pIndexData->vnum);
      int count = mobcount(ch);
      if (count > low_mobbed)
      {
        update_mobbed(ch->name, count);
        init_top_ten();
      }
    }


    if (!IS_NPC(victim) && !IS_NPC(ch) && IS_BONDED(victim))
    {
      bond_death(victim);
    } 
    death_cry (victim);

    if (IS_NPC(victim) || (victim->level > 10 && 
    !IS_SET(victim->act, PLR_QUESTING) && 
    !IS_SET(victim->act, PLR_TOURNEY) && 
    !IS_SET(victim->act, PLR_TAG) &&
    !IS_SET(victim->act, PLR_IT)) ) 
      make_corpse (victim, ch);

    if (IS_SET(victim->act, PLR_QUESTING))
    {
      float_drop(victim);
    }

    if (IS_NPC(victim) && victim->rescuer != NULL)
    {
      if (char_here(victim->rescuer) != NULL)
      {
        fail_rescue(victim->rescuer, victim);
      }    
    }

    if (IS_NPC(victim) && IS_SET(victim->act, ACT_MOUNT) && victim->is_mounted)
    {
      throw_mount(victim);
    }

    if (!IS_NPC(victim))
    {
      victim->pcdata->pk_timer = 0;
      victim->pcdata->safe_timer = 250;
    }
    
    if (IS_MURDERER(victim)||IS_THIEF(victim))
    {
       if (IS_MURDERER(victim))
           victim->penalty.murder -= 10;
       if (IS_THIEF(victim))
           victim->penalty.thief -= 10;
       penalty = number_range(1,45);
       victim->max_hit -= penalty;
       victim->max_mana -= penalty;
       victim->max_move -= penalty;
       victim->pcdata->perm_hit -= penalty;
       victim->pcdata->perm_mana -= penalty;
       victim->pcdata->perm_move -= penalty;
    }
    if (IS_NPC (victim))
    {
        victim->pIndexData->killed++;
        kill_table[URANGE (0, victim->level, MAX_LEVEL - 1)].killed++;
        extract_char (victim, TRUE);
        return;
    }
	
    
	if (victim->level < 11)
		{
			char_from_room(victim);   
			char_to_room(victim, (get_room_index(ROOM_VNUM_MORGUE)));
			victim->position = POS_RESTING;
			do_look(victim, "auto");
			send_to_char("\n", victim);
			send_to_char("Luckily you are still a newbie.\n\r", victim);
			send_to_char("\n", victim);
		}
         else  if (IS_SET(victim->act, PLR_TOURNEY))
		{
			char_from_room(victim);   
			char_to_room(victim, (get_room_index(ROOM_VNUM_MORGUE)));
			victim->position = POS_RESTING;
			do_look(victim, "auto");
			send_to_char("\n", victim);
			send_to_char("WhooHoo. Luckily you are in a tourney!! Get healed and GO GO GO!\n\r", victim);
			send_to_char("\n", victim);
		}
         else if (IS_SET(victim->act, PLR_TAG) && !IS_SET(victim->act,PLR_IT))
		{
			char_from_room(victim);   
			char_to_room(victim, (get_room_index(ROOM_VNUM_MORGUE)));
			victim->position = POS_RESTING;
			do_look(victim, "auto");
			send_to_char("\n", victim);
			send_to_char("Dying in a game sucks doesn't it. Luckily you didn't lose anything.\n\r", victim);
			send_to_char("\n", victim);
		}
	 else if (IS_SET(victim->act, PLR_IT))
		{
			char_from_room(victim);   
			char_to_room(victim, (get_room_index(ROOM_VNUM_MORGUE)));
			victim->position = POS_RESTING;
			do_look(victim, "auto");
			send_to_char("\n", victim);
			send_to_char("Wow... They really must hate you.. CAUSE YOUR IT!!\n\r.", victim);	
			send_to_char("\n", victim);
		}
         else if (IS_SET(victim->act, PLR_QUESTING))
		{
			char_from_room(victim);   
			char_to_room(victim, (get_room_index(ROOM_VNUM_MORGUE)));
			victim->position = POS_RESTING;
			do_look(victim, "auto");
			send_to_char("\n", victim);
			send_to_char("You died... but luckily you are in a quest!\n\r.", victim);
			send_to_char("\n", victim);
		}
	  else	
            extract_char (victim, FALSE);

	if (!IS_NPC(victim) && 
	(victim->level > 10 && 
    !IS_SET(victim->act, PLR_QUESTING)&& 
    !IS_SET(victim->act, PLR_TOURNEY) && 
    !IS_SET(victim->act, PLR_TAG) &&
    !IS_SET(victim->act, PLR_IT)) ) 
    {
//	char_to_room(victim, (get_room_index(ROOM_VNUM_MORGUE)));
	victim->position = POS_RESTING;
	do_look(victim, "auto");
	send_to_char("\n", victim);
        WAIT_STATE(victim, 5 * PULSE_VIOLENCE);
        
        
        

   
}
        auto_guidenote(ch,victim,GUIDE_MURDR);
        auto_guildnote(ch,victim);
        //TEMP REMOVED - CAMM
        /* if ( !IS_NPC(ch) && !IS_SET(ch->act, PLR_TOURNEY) && 
	     !IS_SET(ch->act, PLR_QUESTING))
        {
            if (IS_INPKRANGE(ch,victim))
               {
                  ch->murd = ch->murd + 300;
                  send_to_char("You are now a `1MURDERER!`7\n\r", ch);
               }
        }*/
                
	if ( IS_NPC(ch) && ch->hunting != NULL )
	{
		ch->hunting = NULL;	
	}
	if (IS_NPC(ch) && ch->memory != NULL )
	{
		mob_forget(ch,get_mem_data(ch,victim));
	} 
	if ((!IS_NPC (victim) && victim->pcdata->condition[COND_BLEEDING] != 0))
	{
		victim->pcdata->condition[COND_BLEEDING]=0;
	} 
	
	if (IS_DISGUISED(victim))
	    REM_DISGUISE(victim);
	
	while (victim->affected)
        affect_remove (victim, victim->affected);

        STR_COPY_STR(victim->affected_by, race_table[victim->race].aff, AFF_FLAGS);

    if (!IS_SET(victim->act, PLR_QUESTING) &&
       (!IS_SET(victim->act, PLR_TOURNEY)) && 
       (victim->level > 11) &&
       (!IS_SET(victim->act, PLR_IT)) &&
       (!IS_SET(victim->act, PLR_TAG)))
	  for (i = 0; i < 4; i++)
           victim->armor[i] = 100;
    victim->hit = UMAX (1, victim->hit);
    victim->mana = UMAX (1, victim->mana);
    victim->move = UMAX (1, victim->move);
/*  save_char_obj( victim ); we're stable enough to not need this :) */
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_TOURNEY))
    {
      tournament_death(ch, victim);
    }
    
    return;
}



void group_gain (CHAR_DATA * ch, CHAR_DATA * victim)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;
    int group_levels;


    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if (victim == ch)
        return;

    members = 0;
    group_levels = 0;
    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
        if (is_same_group (gch, ch))
        {
            int dif = gch->level - ch->level;
           members++;
     
           if (gch == ch)
              group_levels += IS_NPC (gch) ? gch->level / 2 : gch->level;  
           else if (abs(dif) < ch->level / 6)
              group_levels += IS_NPC (gch) ? gch->level / 4 : gch->level; 
           else
              group_levels += IS_NPC (gch) ? gch->level / 2 : gch->level;
        }
    }
    if (members == 0)
    {
        bug ("Group_gain: members.", members);
        members = 1;
        group_levels = ch->level;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
//        OBJ_DATA *obj;
//        OBJ_DATA *obj_next;

        if (!is_same_group (gch, ch) || IS_NPC (gch))
            continue;

        if (IS_SET(ch->act, PLR_QUESTOR) && IS_NPC(victim))
        {
            if (ch->questmob == victim->pIndexData->vnum &&
               (ch->rescuemob == 0 || victim->attacking == ch->rescuemob))
            {
                if (ch->rescuemob != 0)
                  abort_rescue(ch, FALSE);
		send_to_char("You have almost completed your QUEST!\n\r",ch);
                send_to_char("Return to the questmaster before your time runs out!\n\r",ch);
                ch->questmob = -1;
            }
        }


        xp = xp_compute (gch, victim, group_levels, members);
        armorcounter--;
        shieldcounter--;
        sanccounter--;
        restorecounter--;
        bonuscounter--;
        blesscounter--;       
        questcounter--;     

    if(!IS_NPC(gch))
    {
        if(IS_SET(gch->act2, PLR_NOEXP))        
        {
            send_to_char("You have NOEXP turned on! You gained no exp!\n\r", gch);
            xp = 0;        
        }
    }

     /*Added check for Questing Char to gain xp for killing quest mobs*/        
		if (IS_SET(gch->act, PLR_QUESTING))
		{
			xp = xp/20;
			sprintf (buf, "You receive %d experience points.\n\r", xp);
			send_to_char (buf, gch);
			gain_exp (gch, xp);
		}
		else
		{
		   if (ch->level < 75)
		   {
			sprintf (buf, "You receive %d experience points.\n\r", xp);
			send_to_char (buf, gch);
		   }
			gain_exp (gch,xp);
		}

/*
        for (obj = ch->carrying; obj != NULL; obj = obj_next)
        {
            obj_next = obj->next_content;
            if (obj->wear_loc == WEAR_NONE)
                continue;

            if ((IS_OBJ_STAT (obj, ITEM_ANTI_EVIL) && IS_EVIL (ch))
                || (IS_OBJ_STAT (obj, ITEM_ANTI_GOOD) && IS_GOOD (ch))
                || (IS_OBJ_STAT (obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL (ch)))
            {
                act ("You are zapped by $p.", ch, obj, NULL, TO_CHAR);
                act ("$n is zapped by $p.", ch, obj, NULL, TO_ROOM);
                obj_from_char (obj);
                obj_to_room (obj, ch->in_room);
            }
        }
*/
    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute (CHAR_DATA * gch, CHAR_DATA * victim, int total_levels, int members)
{
    int xp, base_exp;
    int align, level_range;
    int change;
  //  int time_per_level;

    level_range = victim->level - gch->level;

    /* compute the base exp */
    switch (level_range)
    {
        default:
            base_exp = 1;
            break;
        case -9:
            base_exp = 7;
            break;
        case -8:
            base_exp = 8;
            break;
        case -7:
            base_exp = 11;
            break;
        case -6:
            base_exp = 15;
            break;
        case -5:
            base_exp = 20;
            break;
        case -4:
            base_exp = 24;
            break;
        case -3:
            base_exp = 27;
            break;
        case -2:
            base_exp = 30;
            break;
        case -1:
            base_exp = 35;
            break;
        case 0:
            base_exp = 42;
            break;
        case 1:
            base_exp = 47;
            break;
        case 2:
            base_exp = 50;
            break;
        case 3:
            base_exp = 55;
            break;
        case 4:
            base_exp = 62;
            break;
    }

    if (level_range > 4)
        base_exp = 95 + 25 * (level_range + 10);

	if (!IS_NPC(victim) && 
        (IS_SET(victim->act, PLR_TOURNEY) 
	|| (IS_SET(victim->act,PLR_IT) 
	|| (IS_SET(victim->act,PLR_QUESTING)
	|| (IS_SET(victim->act, PLR_TAG)
        || (IS_SET(victim->act2, PLR_WAR)))))))
	{
		base_exp = 0;
	}  
    /* do alignment computations */

    align = victim->alignment - gch->alignment;

    if (IS_SET (victim->act, ACT_NOALIGN) ||
        (!IS_NPC(gch) 
           && (gch->level >= LEVEL_HERO || IS_DRAGON(gch) || IS_FORSAKEN(gch))
           && IS_SET(gch->act2, PLR_NOEXP) || IS_SET(gch->act, PLR_QUESTING)))
    {
        /* no change */
    }
    else if (align > 500)
    {                            /* monster is more good than slayer */
        change = (align - 500) * base_exp / 500 * gch->level / total_levels;
        change = UMAX (1, change);
        if ((gch->level < LEVEL_HERO && !IS_FORSAKEN(gch) && !IS_DRAGON(gch))
           || !IS_SET(gch->act2, PLR_NOEXP) || !IS_SET(gch->act, PLR_QUESTING))
        {
          gch->alignment = UMAX (-1000, gch->alignment - change);
          if (gch->alignment > 1000)
              gch->alignment = 1000;
        }
    } 

    else if (align < -500)
    {                            /* monster is more evil than slayer */
        change =
            (-1 * align - 500) * base_exp / 500 * gch->level / total_levels;
        change = UMAX (1, change);
        if ((gch->level < LEVEL_HERO && !IS_FORSAKEN(gch) && !IS_DRAGON(gch)) 
           || !IS_SET(gch->act2, PLR_NOEXP) || (!IS_SET(gch->act, PLR_QUESTING))) 
        {
          gch->alignment = UMIN (1000, gch->alignment + change);
          if (gch->alignment < -1000)
              gch->alignment = -1000;
        }
    }

    else
    {                            /* improve this someday */

        change = gch->alignment * base_exp / 500 * gch->level / total_levels;
        if ((gch->level < LEVEL_HERO && !IS_DRAGON(gch) && !IS_FORSAKEN(gch))
           || !IS_SET(gch->act2, PLR_NOEXP) || (!IS_SET(gch->act, PLR_QUESTING))) 
          gch->alignment -= change;
    }

    /* calculate exp multiplier */
    if (IS_SET (victim->act, ACT_NOALIGN))
        xp = base_exp;

    
    else if (gch->alignment > 500)
    {                            /* for goodie two shoes */
        if (victim->alignment < -750)
            xp = (base_exp * 4) / 3;

        else if (victim->alignment < -500)
            xp = (base_exp * 5) / 4;

        else if (victim->alignment > 750)
            xp = base_exp / 4;

        else if (victim->alignment > 500)
            xp = base_exp / 2;

        else if (victim->alignment > 250)
            xp = (base_exp * 3) / 4;

        else
            xp = base_exp;
    }

    else if (gch->alignment < -500)
    {                            /* for baddies */
        if (victim->alignment > 750)
            xp = (base_exp * 5) / 4;

        else if (victim->alignment > 500)
            xp = (base_exp * 11) / 10;

        else if (victim->alignment < -750)
            xp = base_exp / 2;

        else if (victim->alignment < -500)
            xp = (base_exp * 3) / 4;

        else if (victim->alignment < -250)
            xp = (base_exp * 9) / 10;

        else
            xp = base_exp;
    }

    else if (gch->alignment > 200)
    {                            /* a little good */

        if (victim->alignment < -500)
            xp = (base_exp * 6) / 5;

        else if (victim->alignment > 750)
            xp = base_exp / 2;

        else if (victim->alignment > 0)
            xp = (base_exp * 3) / 4;

        else
            xp = base_exp;
    }

    else if (gch->alignment < -200)
    {                            /* a little bad */
        if (victim->alignment > 500)
            xp = (base_exp * 6) / 5;

        else if (victim->alignment < -750)
            xp = base_exp / 2;

        else if (victim->alignment < 0)
            xp = (base_exp * 3) / 4;

        else
            xp = base_exp;
    }

    else
    {                            /* neutral */


        if (victim->alignment > 500 || victim->alignment < -500)
            xp = (base_exp * 4) / 3;

        else if (victim->alignment < 200 && victim->alignment > -200)
            xp = base_exp / 2;

        else
            xp = base_exp;
    }

    /* more exp at the low levels */
    if (gch->level < 6)
        xp = 10 * xp / (gch->level + 4);

    /* less at high */
    if (gch->level > 35)
        xp = 15 * xp / (gch->level - 25);

    /* randomize the rewards */
    xp = number_range (xp * 3 / 4, xp * 5 / 4);

    /* lower gains just a tad */
     xp = int(xp * .50);

/*
     if (members > 1 && total_levels > 15)
     { // Plevelling modifier
       double modifier;
       
       modifier = float(gch->level) / ((total_levels - gch->level) / (members - 1));
       if (modifier < 1.0 && modifier > 0)
         xp = int(xp * (modifier * .75));
//       xp = int(xp * (2 / members)); // Gank for big group
     } 
*/
    if (xp_bonus.on)
    { 
      if (!xp_bonus.negative)
      {
         xp = xp * xp_bonus.multiplier;
      }
      else
      {
         xp = xp / xp_bonus.multiplier;
      }                        
    }

    if (!IS_NPC(gch))
    {
      if (gch->pcdata->xpmultiplier.on)
      {
        if (gch->pcdata->xpmultiplier.factor > 0)
        {
          xp = xp * gch->pcdata->xpmultiplier.factor;
        }
       
        if (gch->pcdata->xpmultiplier.factor < 0)
        {
          xp = xp / abs(gch->pcdata->xpmultiplier.factor);
        }
      }
    }
     
 
    return xp;
}


void dam_message (CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
                  bool immune)
{
    char buf1[256], buf2[256], buf3[256];
    const char *vs;
    const char *vp;
    const char *attack;
    char punct;

    if (ch == NULL || victim == NULL)
        return;

    if (dam == 0)
    {
        vs = "miss";
        vp = "misses";
    }
    else if (dam <= 4)
    {
        vs = "scratch";
        vp = "scratches";
    }
    else if (dam <= 8)
    {
        vs = "graze";
        vp = "grazes";
    }
    else if (dam <= 12)
    {
        vs = "hit";
        vp = "hits";
    }
    else if (dam <= 16)
    {
        vs = "injure";
        vp = "injures";
    }
    else if (dam <= 20)
    {
        vs = "wound";
        vp = "wounds";
    }
    else if (dam <= 24)
    {
        vs = "maul";
        vp = "mauls";
    }
    else if (dam <= 28)
    {
        vs = "decimate";
        vp = "decimates";
    }
    else if (dam <= 32)
    {
        vs = "devastate";
        vp = "devastates";
    }
    else if (dam <= 36)
    {
        vs = "`!maim`*";
        vp = "`!maims`*";
    }
    else if (dam <= 40)
    {
        vs = "`!MUTILATE`*";
        vp = "`!MUTILATES`*";
    }
    else if (dam <= 44)
    {
        vs = "`!DISEMBOWEL`*";
        vp = "`!DISEMBOWELS`*";
    }
    else if (dam <= 48)
    {
        vs = "`!DISMEMBER`*";
        vp = "`!DISMEMBERS`*";
    }
    else if (dam <= 52)
    {
        vs = "`!MASSACRE`*";
        vp = "`!MASSACRES`*";
    }
    else if (dam <= 56)
    {
        vs = "`!MANGLE`*";
        vp = "`!MANGLES`*";
    }
    else if (dam <= 60)
    {
        vs = "`1*** DEMOLISH ***`*";
        vp = "`1*** DEMOLISHES ***`*";
    }
    else if (dam <= 75)
    {
        vs = "`1*** DEVASTATE ***`*";
        vp = "`1*** DEVASTATES ***`*";
    }
    else if (dam <= 100)
    {
        vs = "`1=== OBLITERATE ===`*";
        vp = "`1=== OBLITERATES ===`*";
    }
    else if (dam <= 125)
    {
        vs = "`1>>> ANNIHILATE <<<`*";
        vp = "`1>>> ANNIHILATES <<<`*";
    }
    else if (dam <= 150)
    {
        vs = "`1<<< ERADICATE >>>`*";
        vp = "`1<<< ERADICATES >>>`*";
    }
    else if (dam <= 200)
    {
        vs = "`1<*< REND >*>`*";
        vp = "`1<*< RENDS >*>`*";
    }
    else if (dam <= 250)
    {
        vs = "`!<> `1SHRED`! <>`*";
        vp = "`!<> `1SHREDS`! <>`*";
    }
    else if (dam <= 300)
    {
        vs = "`!<> `1FISSURE`! <>`*";
        vp = "`!<> `1FISSURES`! <>`*";
    }
    else if (dam <= 350)
    {
        vs = "`!<> `&CREM`8ATE`! <>`*";
        vp = "`!<> `&CREM`8ATES`! <>`*";
    }
    else if (dam <= 400)
    {
        vs = "`&*** ATOMIZE ***`*";
        vp = "`&*** ATOMIZES ***`*";
    }
    else if (dam <= 500)
    {
        vs = "do `&UNSPEAKABLE`* things to`*";
        vp = "does `&UNSPEAKABLE`* things to`*";
    }
    else
    {
        vs = "do `8UNTHINKABLE`* things to`*";
        vp = "does `8UNTHINKABLE`* things to`*";
}
    punct = (dam <= 100) ? '.' : '!';

	
    if (dt == TYPE_HIT)
    {
        
        if (ch == victim)
        {
            sprintf (buf1, "$n %s $melf%c", vp, punct);
            sprintf (buf2, "You %s yourself%c", vs, punct);
        }
        else
        {
            sprintf (buf1, "$n %s $N%c", vp, punct);
            sprintf (buf2, "You %s $N%c [`!%d`*]", vs, punct, dam);
            sprintf (buf3, "$n %s you%c", vp, punct);
        }
    }
    else
    {
        if (dt >= 0 && dt < MAX_SKILL)
            attack = skill_table[dt].noun_damage;
        else if (dt >= TYPE_HIT && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE)
        {
            if (get_skill(ch,gsn_martial_arts)>0
                && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE - NEW_DAM_MSG
                && get_eq_char( ch, WEAR_WIELD)==NULL)
                attack = mar_attack_table[dt - TYPE_HIT].noun;
            else
                attack = attack_table[dt - TYPE_HIT].noun;
        }
        else
        {
            bug ("Dam_message: bad dt %d.", dt);
            dt = TYPE_HIT;
            attack = attack_table[0].name;
        }
		
        if (immune)
        {
            if (ch == victim)
            {
                sprintf (buf1, "$n is unaffected by $s own %s.", attack);
                sprintf (buf2, "Luckily, you are immune to that.");
            }
            else
            {
                sprintf (buf1, "$N is unaffected by $n's %s!", attack);
                sprintf (buf2, "$N is unaffected by your %s!", attack);
                sprintf (buf3, "$n's %s is powerless against you.", attack);
            }
        }

        else
        {
            
            if (ch == victim)
            {
                sprintf (buf1, "$n's %s %s $m%c", attack, vp, punct);
                sprintf (buf2, "Your %s %s you%c", attack, vp, punct);
            }
            else
            {
		sprintf (buf1, "$n's %s %s $N%c", attack, vp, punct);
                sprintf (buf2, "Your %s %s $N%c [`!%d`*]", attack, vp, punct, dam);
                sprintf (buf3, "$n's %s %s you%c", attack, vp, punct);
            }
			
        }
    }

    if (ch == victim)
    {
        act (buf1, ch, NULL, NULL, TO_ROOM);
        act (buf2, ch, NULL, NULL, TO_CHAR);
    }
    else
    {
        act (buf1, ch, NULL, victim, TO_NOTVICT);
        act (buf2, ch, NULL, victim, TO_CHAR);
        act (buf3, ch, NULL, victim, TO_VICT);
    }
    return;
}

void do_kill (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Kill whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("You hit yourself.  Ouch!\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

   /* if (victim->fighting != NULL && !is_same_group (ch,
victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }*/

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (ch->position == POS_FIGHTING)
    {
        send_to_char ("You do the best you can!\n\r", ch);
        return;
    }

    WAIT_STATE (ch, 1 * PULSE_VIOLENCE);
    multi_hit (ch, victim, TYPE_UNDEFINED);
    return;
}



void do_murde (CHAR_DATA * ch, char *argument)
{
    send_to_char ("If you want to MURDER, spell it out.\n\r", ch);
    return;
}



void do_murder (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Murder whom?\n\r", ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM)
        || (IS_NPC (ch) && IS_SET (ch->act, ACT_PET)))
        return;

    if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("Suicide is a mortal sin.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

   /* if (IS_NPC (victim) &&
        victim->fighting != NULL && !is_same_group (ch, victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }*/

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (ch->position == POS_FIGHTING)
    {
        send_to_char ("You do the best you can!\n\r", ch);
        return;
    }

    WAIT_STATE (ch, 1 * PULSE_VIOLENCE);
    if (IS_NPC (ch))
        sprintf (buf, "Help! I am being attacked by %s!", ch->short_descr);
    else
        sprintf (buf, "Help!  I am being attacked by %s!", ch->name);
    do_function (victim, &do_yell, buf);
    multi_hit (ch, victim, TYPE_UNDEFINED);
    return;
}




void do_flee (CHAR_DATA * ch, char *argument)
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if (IS_AFFECTED(ch, AFF_FRIGHTEN))
    {
      send_to_char("You are too frightened to leave.\n\r", ch);
      return;
    }

    if ((victim = ch->fighting) == NULL)
    {
        if (ch->position == POS_FIGHTING)
            ch->position = POS_STANDING;
        send_to_char ("You aren't fighting anyone.\n\r", ch);
        return;
    }

    was_in = ch->in_room;
    for (attempt = 0; attempt < 6; attempt++)
    {
        EXIT_DATA *pexit;
        int door;
	//Attempt Flee
        door = number_door ();
        if ((pexit = was_in->exit[door]) == 0
            || pexit->u1.to_room == NULL
            || (IS_SET (pexit->exit_info, EX_CLOSED)
            && (!IS_AFFECTED (ch, AFF_PASS_DOOR)
                || IS_SET (pexit->exit_info, EX_NOPASS)))
            || number_range (0, ch->daze) != 0 
            || (IS_NPC (ch)
            && IS_SET (pexit->u1.to_room->room_flags,ROOM_NO_MOB)))
            continue;

        move_char (ch, door, FALSE, TRUE);
        //Flee Successful??
        if ((now_in = ch->in_room) == was_in)
            continue;

        ch->in_room = was_in;
        act ("$n has fled!", ch, NULL, NULL, TO_ROOM);
        ch->in_room = now_in;
        stop_fighting (ch, TRUE);
        WAIT_STATE(ch, 2);
	if (!IS_NPC(ch))
	{
/*
	   char buf[MSL];
	   sprintf(buf, "Round stats: Inflict %d Received %d\n\r", 
		  ch->pcdata->damInflicted, ch->pcdata->damReceived);
	   send_to_char(buf, ch);
*/
	   ch->pcdata->damInflicted = 0;
	   ch->pcdata->damReceived = 0;
         }

        if (!IS_NPC (ch))
        {
            send_to_char ("You flee from combat!\n\r", ch);
            SET_BIT(ch->act2, PLR_FLED);  
            if ((ch->cClass == 2) && (number_percent () < 3 * (ch->level / 2)))
                send_to_char ("You snuck away safely.\n\r", ch);
            else
            {
                send_to_char ("You lost 10 exp.\n\r", ch);
                gain_exp (ch, -10);
            return;          
            }
        }

        
        return;
    }

    send_to_char ("PANIC! You couldn't escape!\n\r", ch);
    return;
}

/*
void do_surrender (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *mob;
    if ((mob = ch->fighting) == NULL)
    {
        send_to_char ("But you're not fighting!\n\r", ch);
        return;
    }
    act ("You surrender to $N!", ch, NULL, mob, TO_CHAR);
    act ("$n surrenders to you!", ch, NULL, mob, TO_VICT);
    act ("$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT);
    stop_fighting (ch, TRUE);

    if (!IS_NPC (ch) && IS_NPC (mob)
        && (!HAS_TRIGGER_MOB (mob, TRIG_SURR)
            || !p_percent_trigger (mob, NULL, NULL, ch, NULL, NULL, TRIG_SURR)))
    {
        act ("$N seems to ignore your cowardly act!", ch, NULL, mob, TO_CHAR);
        multi_hit (mob, ch, TYPE_UNDEFINED);
    }
}
*/
void do_sla (CHAR_DATA * ch, char *argument)
{
    send_to_char ("If you want to SLAY, spell it out.\n\r", ch);
    return;
}



void do_slay (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument (argument, arg);
    if (arg[0] == '\0')
    {
        send_to_char ("Slay whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (ch == victim)
    {
        send_to_char ("Suicide is a mortal sin.\n\r", ch);
        return;
    }

    if (!IS_NPC (victim) && victim->level >= get_trust (ch))
    {
        send_to_char ("You failed.\n\r", ch);
        return;
    }

    act ("`1You slay $M in cold blood!`*", ch, NULL, victim, TO_CHAR);
    act ("`1$n slays you in cold blood!`*", ch, NULL, victim, TO_VICT);
    act ("`1$n slays $N in cold blood!`*", ch, NULL, victim, TO_NOTVICT);
    raw_kill (ch,victim);
    return;
}

/*
 * Engage Combat code
 *
 *  Written by John Patrick (j.s.patrick@ieee.org)
 *   for Ansalon (ansalon.mudservices.com 8679)
 *
*/
/*
 * Engage skill, allows changing of targets in battle.
 * Increasing ability with Level.
 */

void do_engage(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int chance;

  /* Check for skill.  */
  if ((get_skill(ch,gsn_engage) == 0 )
      &&  (ch->level < skill_table[gsn_engage].skill_level[ch->cClass]))
    {
      send_to_char("Engage?  Engage who? Engage what!\n\r",ch);
      return;
    }

  /* Must be fighting.  */
  if (ch->fighting == NULL)
    {
      send_to_char("You're not fighting anyone.\n\r",ch);
      return;
    }

  one_argument( argument, arg );

  /* Check for argument.  */
  if (arg[0] == '\0')
    {
      send_to_char("Engage who?\n\r",ch);
      return;
    }

  /* Check for victim.  */
  if ((victim = get_char_room (ch, NULL,arg)) == NULL)
    {
      send_to_char("You shadowbox some other time.\n\r",ch);
      return;
    }

  if (victim == ch)
    {
      send_to_char("Attacking yourself in combat isn't a smart thing.\n\r",ch);
      return;
    }

  if (ch->fighting == victim)
    {
      send_to_char("You're already pummelling that target as hard as you can!\n\r",ch);
      return;
    }

  /* Check for safe.  */
  if (is_safe(ch, victim))
    return;

  /* Check for charm.  */
  if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
      act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
      return;
    }

  
  if ((victim->fighting != ch) && (ch->level < 35))
    {
      send_to_char("But they're not fighting you!\n\r",ch);
      return;
    }

  /* Get chance of success, and allow max 95%.  */
  chance = get_skill(ch,gsn_engage);
  chance = UMIN(chance,95);

  if (number_percent() < chance)
    {
      /* It worked!  */
      stop_fighting(ch,FALSE);

      set_fighting(ch,victim);
      if (victim->fighting == NULL)
        set_fighting(victim,ch);

      check_improve(ch,gsn_engage,TRUE,3);
      act("$n has turned $s attacks toward you!",ch,NULL,victim,TO_VICT);
      act("You turn your attacks toward $N.",ch,NULL,victim,TO_CHAR);
      act("$n has turned $s attacks toward $N!",ch,NULL,victim,TO_NOTVICT);
    }
  else
    {
      /* It failed!  */
      send_to_char("You couldn't get your attack in.\n\r",ch);
      check_improve(ch,gsn_engage,FALSE,3);
    }
}


/*
 * Inflict damage from a hit.
 */
bool obj_damage (OBJ_DATA * obj, CHAR_DATA * victim, int dam, int dt, int dam_type, bool show)
{
    //OBJ_DATA *corpse;
    bool immune;
    CHAR_DATA *ch;
    bool CharHere = TRUE;
    

    if (!obj)
     return FALSE;   

    if (!IS_NULLSTR(obj->owner))
    {
      if ((ch = get_char_world_special(obj->owner)) == NULL)
      {
        CharHere = FALSE;
      }
    }
    else
    {
      CharHere = FALSE;
      ch = NULL;
    }

    if (victim->position == POS_DEAD)
        return FALSE;



    /* damage reduction */
    if (dam > 80)
        dam = int((dam - 80) / 1.8 + 80);

    if (dam > 250)
        dam = int((dam - 250) / 1.8 + 250);


    if (CharHere)
    {
      if (is_safe_silent(ch, victim))
         return TRUE;
    }
    else
    {     
    if (is_safe_silent (victim, victim))
        return FALSE;
    }
    
    /*
     * Damage modifiers.
     */
    if (!IS_NPC(victim))
    {
      double tmp = dam * DAMAGE_REDUCER;
      dam = int(tmp);
    }

    if (CharHere)
    {
      if (ch->in_room == victim->in_room)
      {
        if (IS_AFFECTED(ch, AFF_VEIL))
        {
	  double tmp = dam * DAMAGE_VEIL;//Defined in Merc
          dam = int(tmp);
        }

        if ( number_percent() < (get_skill(ch, gsn_luck) / 7) && dam != 0 && ch != victim)
        {  
          send_to_char("You get lucky!\n\r", ch);
          dam = int(dam * 2.25);
          check_improve(ch, gsn_luck, TRUE, 1);
        }       
      }
    }

    if (dam > 1 && !IS_NPC (victim)
        && victim->pcdata->condition[COND_DRUNK] > 10)
        dam = 9 * dam / 10;

    if (dam > 1 && IS_AFFECTED (victim, AFF_SANCTUARY)&& dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam /= 2;

     if (dam > 1 && IS_AFFECTED (victim, AFF_IRONSKIN)&& victim->level > 70 && dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam = int(dam/2.6);
    else if (dam > 1 && IS_AFFECTED (victim, AFF_IRONSKIN)&& victim->level > 60 && victim->level <= 70&& dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam = int(dam/2.3);
    else if (dam > 1 && IS_AFFECTED (victim, AFF_IRONSKIN)&& victim->level > 40 && victim->level  <= 60&& dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam = dam/2;
    else if (dam > 1 && IS_AFFECTED (victim, AFF_IRONSKIN)&& victim->level > 20 &&  victim->level <= 40&& dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam = int(dam/1.7);
    else if (dam > 1 && IS_AFFECTED (victim, AFF_IRONSKIN)&& victim->level <= 20&& dt != gsn_assassinate && dt != gsn_backstab && dt != gsn_strike)
        dam = int(dam/1.4);
        
    if (dam > 1 && IS_AFFECTED (victim, AFF_FIRESHIELD))
        dam -= dam/ 10;
    if (dam > 1 && IS_AFFECTED (victim, AFF_ICESHIELD))
        dam -= dam/ 12;
    if (dam > 1 && IS_AFFECTED (victim, AFF_SHOCKSHIELD))
        dam -= dam/ 14;
    
    immune = FALSE;

    if ( IS_AFFECTED(victim, AFF_PRAY) && dam > 0)
    {
      int tempdam = 0;
      int chance;
      chance = number_percent();
      if (chance > 60)
      {
        tempdam = int(dam * .60);
        dam = int(dam * .9);
        send_to_char("Shaitan comes to your defense and reflects some of your damage.\n\r", 
                  victim);
      }
    }
            
    switch (check_immune (victim, dam_type))
    {
        case (IS_IMMUNE):
            immune = TRUE;
            dam = 0;
            break;
        case (IS_RESISTANT):
            dam -= dam / 3;
            break;
        case (IS_VULNERABLE):
            dam += dam / 2;
            break;
    }

    if (show)
    {
        obj_dam_message (obj, victim, dam, dt, immune);
    }

    if (dam == 0)
        return FALSE;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if (!IS_NPC (victim)
        && victim->level >= LEVEL_IMMORTAL && victim->hit < 1)
        victim->hit = 1;
    update_pos (victim);

    switch (victim->position)
    {
        case POS_MORTAL:
            act ("$n is mortally wounded, and will die soon, if not aided.",
                 victim, NULL, NULL, TO_ROOM);
            send_to_char
                ("You are mortally wounded, and will die soon, if not aided.\n\r",
                 victim);
            break;

        case POS_INCAP:
            act ("$n is incapacitated and will slowly die, if not aided.",
                 victim, NULL, NULL, TO_ROOM);
            send_to_char
                ("You are incapacitated and will slowly die, if not aided.\n\r",
                 victim);
            break;

        case POS_STUNNED:
            act ("$n is stunned, but will probably recover.",
                 victim, NULL, NULL, TO_ROOM);
            send_to_char ("You are stunned, but will probably recover.\n\r",
                          victim);
            break;

        case POS_DEAD:
            act ("$n is DEAD!!", victim, 0, 0, TO_ROOM);
            send_to_char ("You have been KILLED!!\n\r\n\r", victim);
            break;

        default:
           if (dam > victim->max_hit / 4)
                send_to_char ("That really did HURT!\n\r", victim);
		if ( victim->hit < (victim->max_hit * 1/3)
		     && !IS_NPC(victim))
		     if (victim->pcdata->condition[COND_BLEEDING]==0)
	        {
	            send_to_char ("You sure are `1B`!leeding`*!!\n\r",victim);
		    SET_BLEED(victim,TRUE);
	        }   
            break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if (!IS_AWAKE (victim))
        stop_fighting (victim, FALSE);

    /*
     * Payoff for killing things.
     */
    if (victim->position == POS_DEAD)
    {
        if (CharHere)
        {
          group_gain (ch, victim);
        
          if (!IS_NPC (victim)) 
          {
            if (!IS_NPC(ch) && !IS_SET(victim->act, PLR_TOURNEY))
            {
              sprintf (log_buf, "%s (lvl %d) killed by %s (lvl %d) at %ld",
                     victim->name,
		     victim->level,
                     (IS_NPC (ch) ? ch->short_descr : ch->name),
		     ch->level,
                     ch->in_room->vnum);
              log_special (log_buf, PK_TYPE);
            }

            /*
             * Dying penalty:
             * 2/3 way back to previous level.
             */
            if (victim->exp > exp_per_level (victim, victim->pcdata->points)
                * victim->level && 
                 !IS_SET(victim->act, PLR_QUESTING) &&
                 !IS_SET(victim->act, PLR_TOURNEY) &&
                 !IS_SET(victim->act, PLR_IT) &&
                 !IS_SET(victim->act, PLR_TAG) &&
                 !IS_SET(victim->act2, PLR_WAR))
                 
                gain_exp (victim,
                          (2 *
                           (exp_per_level (victim, victim->pcdata->points) *
                            victim->level - victim->exp) / 3) + 50);

	     if (ch->level > 75 &&
                 !IS_SET(victim->act, PLR_QUESTING) &&
                 !IS_SET(victim->act, PLR_TOURNEY) &&
                 !IS_SET(victim->act, PLR_IT) &&
                 !IS_SET(victim->act, PLR_TAG) &&
                 !IS_SET(victim->act2, PLR_WAR))
                gain_exp (victim,
                          (2 *
                           (exp_per_level (victim, victim->pcdata->points) *
                            victim->level - victim->exp) / 3) + 50);

          }

          sprintf (log_buf, "%s got toasted by %s at %s [room %ld]",
                 (IS_NPC (victim) ? victim->short_descr : victim->name),
                 (IS_NPC (ch) ? ch->short_descr : ch->name),
                 ch->in_room->name, ch->in_room->vnum);

          if (IS_NPC (victim))
            wiznet (log_buf, NULL, NULL, WIZ_MOBDEATHS, 0, 0);
          else
            wiznet (log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
        }
        else
        {
          sprintf (log_buf, "%s got toasted by %s",
                 (IS_NPC (victim) ? victim->short_descr : victim->name),
                 obj->short_descr);

          if (IS_NPC (victim))
            wiznet (log_buf, NULL, NULL, WIZ_MOBDEATHS, 0, 0);
          else
            wiznet (log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
        }
        
        
        /*
         * Death trigger
         */
        if (IS_NPC (victim) && HAS_TRIGGER_MOB (victim, TRIG_DEATH))
        {
            victim->position = POS_STANDING;
            p_percent_trigger (victim, NULL, NULL, ch, NULL, NULL, TRIG_DEATH);
        }
        
        if (CharHere)
          if (!IS_NPC(victim) && !IS_NPC(ch))
            victim->pcdata->lastKilled = str_dup(ch->name);
       
    if (CharHere)
    {
      if (IS_SET(victim->act, PLR_IT))
      {
    	  REMOVE_BIT(victim->act, PLR_IT);
    	  SET_BIT(victim->act, PLR_TAG);
    	  REMOVE_BIT(ch->act, PLR_TAG);
    	  SET_BIT(ch->act, PLR_IT);
    	  sprintf (log_buf, "%s has lost the IT flag to %s!",
    	  victim->name,
    	  ch->name);
    	  do_echo( ch, log_buf);
      }
    }	                    
    
    if (CharHere)
      raw_kill (ch,victim);
    else
      raw_kill (victim, victim);
      
      return TRUE;
    }

    tail_chain ();
    return TRUE;
}




void obj_dam_message (OBJ_DATA * obj, CHAR_DATA * victim, int dam, int dt,
                  bool immune)
{
    char buf1[256], buf3[256];
    char buf2[256];
    const char *vs;
    const char *vp;
    const char *attack;
    char punct;
    CHAR_DATA *ch = NULL;
    bool charHere = FALSE; 


    if (!IS_NULLSTR(obj->owner))
    {
      if ((ch = get_char_world_special(obj->owner)) != NULL)
      {
         if (ch->in_room == victim->in_room)
         {  
           charHere = TRUE;
         }
      }
    }  
    

    if (obj == NULL || victim == NULL)
        return;


    if (dam == 0)
    {
        vs = "miss";
        vp = "misses";
    }
    else if (dam <= 4)
    {
        vs = "scratch";
        vp = "scratches";
    }
    else if (dam <= 8)
    {
        vs = "graze";
        vp = "grazes";
    }
    else if (dam <= 12)
    {
        vs = "hit";
        vp = "hits";
    }
    else if (dam <= 16)
    {
        vs = "injure";
        vp = "injures";
    }
    else if (dam <= 20)
    {
        vs = "wound";
        vp = "wounds";
    }
    else if (dam <= 24)
    {
        vs = "maul";
        vp = "mauls";
    }
    else if (dam <= 28)
    {
        vs = "decimate";
        vp = "decimates";
    }
    else if (dam <= 32)
    {
        vs = "devastate";
        vp = "devastates";
    }
    else if (dam <= 36)
    {
        vs = "`!maim`*";
        vp = "`!maims`*";
    }
    else if (dam <= 40)
    {
        vs = "`!MUTILATE`*";
        vp = "`!MUTILATES`*";
    }
    else if (dam <= 44)
    {
        vs = "`!DISEMBOWEL`*";
        vp = "`!DISEMBOWELS`*";
    }
    else if (dam <= 48)
    {
        vs = "`!DISMEMBER`*";
        vp = "`!DISMEMBERS`*";
    }
    else if (dam <= 52)
    {
        vs = "`!MASSACRE`*";
        vp = "`!MASSACRES`*";
    }
    else if (dam <= 56)
    {
        vs = "`!MANGLE`*";
        vp = "`!MANGLES`*";
    }
    else if (dam <= 60)
    {
        vs = "`1*** DEMOLISH ***`*";
        vp = "`1*** DEMOLISHES ***`*";
    }
    else if (dam <= 75)
    {
        vs = "`1*** DEVASTATE ***`*";
        vp = "`1*** DEVASTATES ***`*";
    }
    else if (dam <= 100)
    {
        vs = "`1=== OBLITERATE ===`*";
        vp = "`1=== OBLITERATES ===`*";
    }
    else if (dam <= 125)
    {
        vs = "`1>>> ANNIHILATE <<<`*";
        vp = "`1>>> ANNIHILATES <<<`*";
    }
    else if (dam <= 150)
    {
        vs = "`1<<< ERADICATE >>>`*";
        vp = "`1<<< ERADICATES >>>`*";
    }
    else if (dam <= 200)
    {
        vs = "`1<*< REND >*>`*";
        vp = "`1<*< RENDS >*>`*";
    }
    else if (dam <= 250)
    {
        vs = "`!<> `1SHRED`! <>`*";
        vp = "`!<> `1SHREDS`! <>`*";
    }
    else if (dam <= 300)
    {
        vs = "`!<> `1FISSURE`! <>`*";
        vp = "`!<> `1FISSURES`! <>`*";
    }
    else if (dam <= 350)
    {
        vs = "`!<> `&CREM`8ATE`! <>`*";
        vp = "`!<> `&CREM`8ATES`! <>`*";
    }
    else if (dam <= 400)
    {
        vs = "`&*** ATOMIZE ***`*";
        vp = "`&*** ATOMIZES ***`*";
    }
    else if (dam <= 500)
    {
        vs = "do `&UNSPEAKABLE`* things to`*";
        vp = "does `&UNSPEAKABLE`* things to`*";
    }
    else
    {
        vs = "do `8UNTHINKABLE`* things to`*";
        vp = "does `8UNTHINKABLE`* things to `*";
    }
    punct = (dam <= 100) ? '.' : '!';

	
		
    attack = attack_table[dt - TYPE_HIT].noun;
		
    if (immune) 
    {
          sprintf (buf1, "$N is unaffected by $p's %s!", attack);
          sprintf (buf3, "$p's %s is powerless against you.", attack);  
    }
    else
    {
      	sprintf (buf1, "$p's %s %s $N%c", attack, vp, punct);
        if (charHere && ch != victim)
      	  sprintf (buf2, "Your $p's %s %s $N%c [`!%d`*]", attack, vp, punct, dam);
	else
      	  sprintf (buf2, "Your $p's %s %s you.%c [`!%d`*]", attack, vp, punct, dam);
        sprintf (buf3, "$p's %s %s you%c", attack, vp, punct);
    }


      
    {
        act (buf1, victim, obj, victim, TO_NOTVICT);
        if (charHere && !immune)
          act (buf2, ch, obj, victim, TO_CHAR);
        act (buf3, victim, obj, victim, TO_CHAR);
    }
    return;
}
void disarm (CHAR_DATA * ch, CHAR_DATA * victim)
{
    OBJ_DATA *obj;

    if ((obj = get_eq_char (victim, WEAR_WIELD)) == NULL)
        return;


    act ("$n DISARMS you and sends your weapon flying!`*",
         ch, NULL, victim, TO_VICT);
    act ("You disarm $N!`*", ch, NULL, victim, TO_CHAR);
    act ("$n disarms $N!`*", ch, NULL, victim, TO_NOTVICT);

    obj_from_char (obj);
    if (IS_OBJ_STAT (obj, ITEM_NODROP) || IS_OBJ_STAT (obj, ITEM_INVENTORY))
        obj_to_char (obj, victim);
    else
    {
        if (!IS_NPC(victim) && (IS_SET(victim->act, PLR_TOURNEY) ||
            IS_SET(victim->act, PLR_QUESTING)))
        {
          obj_to_char(obj, victim);
        }
        else
          !IS_NPC(victim)?obj_to_room (obj,victim->in_room):obj_to_char(obj,victim);
        if (IS_NPC (victim) && !obj->carried_by && victim->wait == 0 && can_see_obj (victim, obj))
            get_obj (victim, obj, NULL, FALSE);
    }

    return;
}



void do_backstab (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Backstab whom?\n\r", ch);
        return;
    }

    if (ch->fighting != NULL)
    {
        send_to_char ("You're facing the wrong end.\n\r", ch);
        return;
    }

    else if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("How can you sneak up on yourself?\n\r", ch);
        return;
    }

    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;
/*
(IS_NPC (victim) &&
        victim->fighting != NULL && !is_same_group (ch, victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }*/

    if ((obj = get_eq_char (ch, WEAR_WIELD)) == NULL)
    {
        send_to_char ("You need to wield a weapon to backstab.\n\r", ch);
        return;
    }

    if (victim->hit < victim->max_hit / 2)
    {
        act ("$N is hurt and suspicious ... you can't sneak up.",
             ch, NULL, victim, TO_CHAR);
        return;
    }

    WAIT_STATE (ch, skill_table[gsn_backstab].beats);
    if (calc_chance(ch,victim,gsn_backstab,FALSE) && ( (get_skill(ch, gsn_backstab) >= 1) || victim->position == POS_SLEEPING) )
    {
        check_improve (ch, gsn_backstab, TRUE, 1);
        multi_hit (ch, victim, gsn_backstab);
    }
    else
    {
        check_improve (ch, gsn_backstab, FALSE, 1);
        damage (ch, victim, 0, gsn_backstab, DAM_NONE, TRUE, FALSE);
    }

    return;
}

void do_assassinate(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  int chance;

  one_argument (argument, arg);

  if (get_skill(ch, gsn_assassinate) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    send_to_char ("Assassinate whom?\n\r", ch);
    return;
  }

  if (ch->fighting != NULL)
  {
    send_to_char ("You can't do that during battle.\n\r", ch);
    return;
  }
  else if ((victim = get_char_room (ch, NULL, arg)) == NULL)
  {
    send_to_char ("They aren't here.\n\r", ch);
    return;
  }
  
  if (victim == ch)
  {
    send_to_char ("That would be silly.\n\r", ch);
    return;
  }

    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

  if (is_safe (ch, victim))
    return;

  if (ch->hit < (ch->max_hit / 2) && IS_NPC(victim))
  {
    send_to_char("You're too weak from battle to attempt that.\n\r", ch);
    return;
  }

  if ((obj = get_eq_char (ch, WEAR_WIELD)) == NULL)
  {
    send_to_char ("You need to wield a weapon to assassinate.\n\r", ch);
    return;
  }

  if (victim->hit < (victim->max_hit * .85))
  {
    act ("$N is hurt and suspicious ... you can't sneak up.",
             ch, NULL, victim, TO_CHAR);
    return;
  }

  
  

  WAIT_STATE (ch, skill_table[gsn_assassinate].beats);
  if (calc_chance(ch,victim,gsn_assassinate,FALSE))
  {
    chance = get_skill(ch, gsn_assassinate) == 100 ? 96 : 98;
    if (number_percent() > chance)
    {
      act("$n assassinates you!", ch, NULL, victim, TO_VICT);
      act("$n assassinates $N in cold blood!", ch, NULL, victim, TO_ROOM);
      act("With great skill you sneak up behind $N and assassinate $M.",
            ch, NULL, victim, TO_CHAR);
      raw_kill(ch,victim);
      check_improve(ch, gsn_assassinate, TRUE, 2);
      return;
    }
    check_improve (ch, gsn_assassinate, TRUE, 1);
    multi_hit (ch, victim, gsn_assassinate);
  }
  else
  {
    check_improve (ch, gsn_assassinate, FALSE, 1);
    damage (ch, victim, 0, gsn_assassinate, DAM_NONE, TRUE, FALSE);
  }

  return;
}
void do_strike( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
   

    one_argument( argument, arg );
    if (get_skill(ch,gsn_strike)<1)
    {
    	send_to_char ("Huh?\n\r",ch);
    	return;
    }
    if (arg[0] == '\0')
    {
        send_to_char("Strike whom?\n\r",ch);
        return;
    }

    if ((get_eq_char(ch,WEAR_WIELD)) != NULL)
	{
	send_to_char("You cannot effectively strike with a weapon.\n\r", ch);
	return;
	}
	
	if (ch->fighting != NULL)
    {
	send_to_char("You can't do that now!.\n\r",ch);
	return;
    }
     
    else if ((victim = get_char_room (ch, NULL,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if ( victim == ch )
    {
	send_to_char( "Not to yourself, silly!\n\r", ch );
	return;
    }

    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

    if ( is_safe( ch, victim ) )
      return;
    
    if (victim->fighting !=NULL) 
    {
        send_to_char("Not while they are fighting!.\n\r",ch);
        return;
    }
    if ( victim->hit < victim->max_hit / 2)
    {
	act( "$N is hurt and suspicious ... you can't seem to strike.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE);
    if ( calc_chance(ch,victim,gsn_strike,FALSE) )
    {
	check_improve(ch,gsn_strike,TRUE,1);
        multi_hit( ch, victim, gsn_strike );
    }
    else
    {
	check_improve(ch,gsn_strike,FALSE,1);
	damage( ch, victim, 0, gsn_strike,DAM_NONE,TRUE, FALSE);
    }

    return;
}
void do_rescue (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    one_argument (argument, arg);

    if (!str_cmp("warrior", class_table[ch->cClass].name) && ch->pcdata->stance != STANCE_DEFENSIVE)
    {
        send_to_char ("You must be in an defensive style to use this skill.\n\r", ch);
        return;
    }

    if (arg[0] == '\0')
    {
        send_to_char ("Rescue whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_room (ch, NULL, arg)) == NULL || !can_see(ch,victim))
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("What about fleeing instead?\n\r", ch);
        return;
    }

    if (!IS_NPC (ch) && IS_NPC (victim))
    {
        send_to_char ("Doesn't need your help!\n\r", ch);
        return;
    }

    if (ch->fighting == victim)
    {
        send_to_char ("Too late.\n\r", ch);
        return;
    }

    if ((fch = victim->fighting) == NULL)
    {
        send_to_char ("That person is not fighting right now.\n\r", ch);
        return;
    }

 /*   if (IS_NPC (fch) && !is_same_group (ch, victim))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }*/

    WAIT_STATE (ch, skill_table[gsn_rescue].beats);
    if (calc_chance(ch,victim,gsn_rescue,TRUE))
    {
        act ("You rescue $N!`*", ch, NULL, victim, TO_CHAR);
        act ("$n rescues you!`*", ch, NULL, victim, TO_VICT);
        act ("$n rescues $N!`*", ch, NULL, victim, TO_NOTVICT);
        check_improve (ch, gsn_rescue, TRUE, 1);

        stop_fighting (fch, FALSE);
        stop_fighting (victim, FALSE);
        set_fighting (ch, fch);
        set_fighting (fch, ch);
        return; 
    }
    send_to_char ("You fail the rescue.\n\r", ch);
    check_improve (ch, gsn_rescue, FALSE, 1);
    return;
    
}



void do_kick (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    char arg [MIL];
    int skill;
    int dam;
    
    one_argument (argument, arg);
    
    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char ("But you aren't fighting anyone!\n\r", ch);
            return;
        }
    }
    
    else if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

     
    if (!IS_NPC (ch)
        && (ch->level < skill_table[gsn_kick].skill_level[ch->cClass]
        || get_skill(ch, gsn_kick) < 1))
    {
        send_to_char ("You better leave the martial arts to fighters.\n\r",
                      ch);
        return;
    }

    if (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_KICK))
        return;

    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

    /*if ((victim = ch->fighting) == NULL)
    {
        send_to_char ("You aren't fighting anyone.\n\r", ch);
        return;
    }*/

    WAIT_STATE (ch, skill_table[gsn_kick].beats);
    if (calc_chance(ch,victim,gsn_kick,TRUE))
    {
    	if ((skill = get_skill(ch,gsn_martial_arts))>0)    	
    	{
    	    dam =number_range (int(1+(ch->level/2.5)), int(ch->level/1.5));
    	    dam += int(GET_DAMROLL (ch) * UMIN (100, skill) / 100.0);
            damage (ch, victim,dam, gsn_kick,DAM_BASH, TRUE, FALSE);
        }
        else
        {
            skill = get_skill(ch,gsn_kick);
            dam =number_range (1+(ch->level/3), int(ch->level/1.8));
            dam += int(GET_DAMROLL (ch) * UMIN (100, skill) / 100.0);
            damage (ch, victim,dam, gsn_kick,DAM_BASH, TRUE, FALSE);
        }
        check_improve (ch, gsn_kick, TRUE, 1);
    }
    else
    {
        damage (ch, victim, 0, gsn_kick, DAM_BASH, TRUE, FALSE);
        check_improve (ch, gsn_kick, FALSE, 1);
    }
    return;
}




void do_disarm (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
   

    if ((get_skill (ch, gsn_disarm)) == 0)
    {
        send_to_char ("You don't know how to disarm opponents.\n\r", ch);
        return;
    }
    
    if ((victim = ch->fighting) == NULL)
    {
        send_to_char ("You aren't fighting anyone.\n\r", ch);
        return;
    }
   
    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

    if (!str_cmp("warrior", class_table[ch->cClass].name) && ch->pcdata->stance != STANCE_OFFENSIVE)
    {
	send_to_char ("You must be in an offensive style to use this skill.\n\r", ch);
        return;
    }

    if ((obj = get_eq_char (victim, WEAR_WIELD)) == NULL)
    {
        send_to_char ("Your opponent is not wielding a weapon.\n\r", ch);
        return;
    }


    /* and now the attack */
    if (calc_chance(ch,victim,gsn_disarm,TRUE))
    {
        WAIT_STATE (ch, skill_table[gsn_disarm].beats);
        disarm (ch, victim);
        check_improve (ch, gsn_disarm, TRUE, 1);
    }
    else
    {
        WAIT_STATE (ch, skill_table[gsn_disarm].beats);
        act ("You fail to disarm $N.`*", ch, NULL, victim, TO_CHAR);
        act ("$n tries to disarm you, but fails.`*", ch, NULL, victim,
             TO_VICT);
        act ("$n tries to disarm $N, but fails.`*", ch, NULL, victim,
             TO_NOTVICT);
        check_improve (ch, gsn_disarm, FALSE, 1);
    }
    return;
}


void do_berserk (CHAR_DATA * ch, char *argument)
{
    int chance;
    

    if (( chance = get_skill (ch, gsn_berserk)) == 0
        || (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_BERSERK))
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_berserk].skill_level[ch->cClass]))
    {
        send_to_char ("You turn red in the face, but nothing happens.\n\r",
                      ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_BERSERK) || is_affected (ch, gsn_berserk)
        || is_affected (ch, skill_lookup ("frenzy")))
    {
        send_to_char ("You get a little madder.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CALM))
    {
        send_to_char ("You're feeling to mellow to berserk.\n\r", ch);
        return;
    }

    if (ch->mana < 50)
    {
        send_to_char ("You can't get up enough energy.\n\r", ch);
        return;
    }

            if (ch->position == POS_FIGHTING)
                chance += 10;

            /* damage -- below 50% of hp helps, above hurts */
            chance += 25 - (100 * ch->hit / ch->max_hit) / 2;
    
    if (number_percent()<chance)
    {
        AFFECT_DATA af;

        WAIT_STATE (ch, PULSE_VIOLENCE);
        ch->mana -= 50;
        ch->move /= 2;

        /* heal a little damage */
        ch->hit += ch->level * 2;
        ch->hit = UMIN (ch->hit, ch->max_hit);

        send_to_char ("Your pulse races as you are consumed by rage!\n\r",
                      ch);
        act ("$n gets a wild look in $s eyes.", ch, NULL, NULL, TO_ROOM);
        check_improve (ch, gsn_berserk, TRUE, 2);

        af.where = TO_AFFECTS;
        af.type = gsn_berserk;
        af.level = ch->level;
        af.duration = number_fuzzy (ch->level / 8);
        af.modifier = UMAX (1, ch->level / 5);
        af.bitvector = AFF_BERSERK;

        af.location = APPLY_HITROLL;
        affect_to_char (ch, &af);

        af.location = APPLY_DAMROLL;
        affect_to_char (ch, &af);

        af.modifier = UMAX (10, 10 * (ch->level / 5));
        af.location = APPLY_AC;
        affect_to_char (ch, &af);
    }

    else
    {
        WAIT_STATE (ch, 3 * PULSE_VIOLENCE);
        ch->mana -= 25;
        ch->move /= 2;

        send_to_char ("Your pulse speeds up, but nothing happens.\n\r", ch);
        check_improve (ch, gsn_berserk, FALSE, 2);
    }
}

void do_bash (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int dam;
    int skill= get_skill (ch, gsn_bash);

    one_argument (argument, arg);

    if ((get_skill (ch, gsn_bash)) == 0
        || (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_BASH))
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_bash].skill_level[ch->cClass]))
    {
        send_to_char ("Bashing? What's that?\n\r", ch);
        return;
    }
    
    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

    if (!str_cmp("warrior", class_table[ch->cClass].name) && ch->pcdata->stance != STANCE_OFFENSIVE)
    {
        send_to_char ("You must be in an offensive style to use this skill.\n\r", ch);
        return;
    }
    
    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char ("But you aren't fighting anyone!\n\r", ch);
            return;
        }
    }
    
    else if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }
    
    if (victim->position < POS_FIGHTING)
    {
        act ("You'll have to let $M get back up first.", ch, NULL, victim,
             TO_CHAR);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("You try to bash your brains out, but fail.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

   /* if (IS_NPC (victim) &&
        victim->fighting != NULL && !is_same_group (ch, victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }*/

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("But $N is your friend!", ch, NULL, victim, TO_CHAR);
        return;
    }
    if (IS_AFFECTED (victim,AFF_AIRBARRIER) && number_percent() < 35)
    {
    	send_to_char ("Your attack is repulsed by a barrier of air!\n\r",ch);
    	WAIT_STATE(ch,skill_table[gsn_bash].beats);
	act ("Your weave of air holds off $n's attack!",ch,NULL,victim, TO_VICT);
    	return;
    }


    /* modifiers */

    /* size  and weight */
    

 
    if (!IS_NPC (victim) && get_skill (victim, gsn_dodge)>0)
    {
        if (number_percent() < (get_skill (victim, gsn_dodge)-75))
        {                            
            act("$n tries to bash you, but you dodge it.`*",ch,NULL,victim,TO_VICT);
            act("$N dodges your bash, you fall flat on your face.`*",ch,NULL,victim,TO_CHAR);
            WAIT_STATE(ch,skill_table[gsn_bash].beats);
            return; 
        
        }
    }
    else if (!IS_NPC (victim) && get_skill(victim,gsn_evade)>0)
    {
       if (number_percent() < (get_skill (victim, gsn_evade)-75))
        {                            
            act("$n tries to bash you, but you evade it.`*",ch,NULL,victim,TO_VICT);
            act("$N evades your bash, you fall flat on your face.`*",ch,NULL,victim,TO_CHAR);
            WAIT_STATE(ch,skill_table[gsn_bash].beats);
            return; 
        }
    }
    /* now the attack */
    if (calc_chance(ch,victim,gsn_bash,TRUE))
    {

        act ("$n sends you sprawling with a powerful bash!`*",
             ch, NULL, victim, TO_VICT);
        act ("You slam into $N, and send $M flying!`*", ch, NULL, victim,
             TO_CHAR);
        act ("$n sends $N sprawling with a powerful bash.`*", ch, NULL,
             victim, TO_NOTVICT);
        check_improve (ch, gsn_bash, TRUE, 1);

        WAIT_STATE (victim, 2 * PULSE_VIOLENCE);
        WAIT_STATE (ch, skill_table[gsn_bash].beats);
        victim->position = POS_RESTING;
        dam =number_range ((ch->level / 2), (ch->level / 2) + 10 * ch->size + number_percent() / 2);
        dam += int(GET_DAMROLL (ch) * UMIN (100, skill) / 100.0);
        damage (ch, victim, dam, gsn_bash, DAM_BASH, TRUE, FALSE);

    }
    else
    {
        damage (ch, victim, 0, gsn_bash, DAM_BASH, FALSE, FALSE);
        act ("You fall flat on your face!`*", ch, NULL, victim, TO_CHAR);
        act ("$n falls flat on $s face.`*", ch, NULL, victim, TO_NOTVICT);
        act ("You evade $n's bash, causing $m to fall flat on $s face.`*",
             ch, NULL, victim, TO_VICT);
        check_improve (ch, gsn_bash, FALSE, 1);
        ch->position = POS_RESTING;
        WAIT_STATE (ch, skill_table[gsn_bash].beats * 3 / 2);
    }
  
}


void do_dirt (CHAR_DATA * ch, char *argument)
{


    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int skill=get_skill (ch, gsn_dirt);
    int dam;

    one_argument (argument, arg);

    if (( get_skill (ch, gsn_dirt)) == 0
        || (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_KICK_DIRT))
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_dirt].skill_level[ch->cClass]))
    {
        send_to_char ("You get your feet dirty.\n\r", ch);
        return;
    }
    
    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

    if (!str_cmp("warrior", class_table[ch->cClass].name) && ch->pcdata->stance != STANCE_OFFENSIVE)
    {
        send_to_char ("You must be in an offensive style to use this skill.\n\r", ch);
        return;
    }

    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char ("But you aren't in combat!\n\r", ch);
            return;
        }
    }
    
    else if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (victim, AFF_BLIND))
    {
        act ("$E's already been blinded.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("Very funny.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

   /* if (IS_NPC (victim) &&
        victim->fighting != NULL && !is_same_group (ch, victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }*/
    if (IS_AFFECTED (victim,AFF_AIRBARRIER) && number_percent() < 40)
    {
    	send_to_char ("Your attack is repulsed by a barrier of air!\n\r",ch);
    	act ("Your weave of air holds off $n's attack!",ch,NULL,victim, TO_VICT);
        WAIT_STATE (ch, 20);
    	return;
    }
    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("But $N is such a good friend!", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (ch->in_room->sector_type == SECT_WATER_SWIM
      || ch->in_room->sector_type == SECT_WATER_NOSWIM
      || ch->in_room->sector_type == SECT_AIR)
    {
        send_to_char ("There isn't any dirt to kick.\n\r", ch);
        return;
    }
   
    /* now the attack */
    if (calc_chance(ch,victim,gsn_dirt,FALSE))
    {
        AFFECT_DATA af;
        act ("$n is blinded by the dirt in $s eyes!`*", victim, NULL, NULL,
             TO_ROOM);
        act ("$n kicks dirt in your eyes!`*", ch, NULL, victim, TO_VICT);
        dam = number_range (2, 5);
        dam += int(GET_DAMROLL (ch) * UMIN (100, skill) / 100.0);
        damage (ch, victim, dam, gsn_dirt, DAM_NONE, TRUE, FALSE);
        send_to_char ("You can't see a thing!`*\n\r", victim);
        check_improve (ch, gsn_dirt, TRUE, 2);
        WAIT_STATE (ch, skill_table[gsn_dirt].beats);

        af.where = TO_AFFECTS;
        af.type = gsn_dirt;
        af.level = ch->level;
        af.duration = 0;
        af.location = APPLY_HITROLL;
        af.modifier = -4;
        af.bitvector = AFF_BLIND;

        affect_to_char (victim, &af);
    }
    else
    {
        damage (ch, victim, 0, gsn_dirt, DAM_NONE, TRUE, FALSE);
        check_improve (ch, gsn_dirt, FALSE, 2);
        WAIT_STATE (ch, skill_table[gsn_dirt].beats);
    }
    
}


/* -------------------------------------------
 * Counter Skill for ROM24, 1.02 - 3/30/98
 * -------------------------------------------
 * The counter skill is an advanced defensive
 * fighting technique that allows a skilled
 * fighter to reverse his opponents attack and
 * actually hit him back with it.
 *
 * As with all else, please email me if you use
 * this code so I know my time is not going
 * to waste. :) And also, with this one I could
 * really use some suggesstions for improvement!
 *
 * Brian Babey (aka Varen)
 * [bribe@erols.com]
 * ------------------------------------------- */
bool check_counter( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt)
{
        int chance;
        int dam_type;
        OBJ_DATA *wield;
        char buf [MSL];
        int diceroll;
        

        if (    ( !IS_AWAKE(victim) ) ||
                ( !can_see(victim,ch) ) ||
                ( get_skill(victim,gsn_counter) < 1 ) ||
                dt == gsn_backstab ||
                dt == gsn_assassinate ||
                dt == gsn_strike
           )
           return FALSE;

        wield = get_eq_char(victim,WEAR_WIELD);
        chance = get_skill(victim,gsn_counter) / 13;
        if (!IS_NPC(victim))
        {
            chance += ( victim->level - ch->level );        
            chance += 2 * (get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX));
            chance += get_weapon_skill(victim,get_weapon_sn(victim,FALSE)) -
                        get_weapon_skill(ch,get_weapon_sn(ch,FALSE));
            chance += (get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR) );
        }
        
    while ((diceroll = number_bits (5)) >= 20);
    if (!IS_NPC(victim) && IS_SET(victim->act2, PLR_TESTOR))
    {
    	sprintf(buf,"`1Counter : diceroll %d < chance %d`*\n\r",diceroll,chance/5);
    	send_to_char(buf,victim);
    }
    
    if (((diceroll==20) ||((diceroll > chance/5)) && (diceroll != 0)))
        	
    {
    	check_improve(victim,gsn_counter,FALSE,10);
        return FALSE;
    }

        dt = gsn_counter;

    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else 
            dt += ch->dam_type;
    }

    if (dt < TYPE_HIT)
    	if (wield != NULL)
    	    dam_type = attack_table[wield->value[3]].damage;
    	else
    	    dam_type = attack_table[ch->dam_type].damage;
    else
    	dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
	dam_type = DAM_BASH;

    act( "You reverse $n's attack and counter with your own!", ch, NULL, victim, TO_VICT    );
    act( "$N reverses your attack!", ch, NULL, victim, TO_CHAR    );

    damage(victim,ch,dam, gsn_counter , dam_type ,TRUE , FALSE); /* DAM MSG NUMBER!! */

    check_improve(victim,gsn_counter,TRUE,12);

    return TRUE;
}
void do_trip (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
 
    int skill=get_skill (ch, gsn_trip);
    int dam;

    one_argument (argument, arg);

    if ((get_skill (ch, gsn_trip)) == 0
        || (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_TRIP))
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_trip].skill_level[ch->cClass]))
    {
        send_to_char ("Tripping?  What's that?\n\r", ch);
        return;
    }
    
    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

    if (!str_cmp("warrior", class_table[ch->cClass].name) && ch->pcdata->stance != STANCE_OFFENSIVE)
    {
        send_to_char ("You must be in an offensive style to use this skill.\n\r", ch);
        return;
    }

    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char ("But you aren't fighting anyone!\n\r", ch);
            return;
        }
    }
    
    else if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

   /* if (IS_NPC (victim) &&
        victim->fighting != NULL && !is_same_group (ch, victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    } */

    if (IS_AFFECTED (victim, AFF_FLYING))
    {
        act ("$S feet aren't on the ground.", ch, NULL, victim, TO_CHAR);
        return;
    }
    if (IS_AFFECTED (victim,AFF_AIRBARRIER) && number_percent()<35)
    {
    	send_to_char ("Your attack is repulsed by a barrier of air!\n\r",ch);
    	act ("Your weave of air holds off $n's attack!",ch,NULL,victim, TO_VICT);
        WAIT_STATE (ch, 2 * skill_table[gsn_trip].beats);
    	return;
    }

    if (victim->position < POS_FIGHTING)
    {
        act ("$N is already down.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("You fall flat on your face!`*\n\r", ch);
        WAIT_STATE (ch, 2 * skill_table[gsn_trip].beats);
        act ("$n trips over $s own feet!`*", ch, NULL, NULL, TO_ROOM);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
        return;
    }



    /* now the attack */
    if (calc_chance(ch,victim,gsn_trip,TRUE))
    {
        act ("$n trips you and you go down!`*", ch, NULL, victim, TO_VICT);
        act ("You trip $N and $N goes down!`*", ch, NULL, victim, TO_CHAR);
        act ("$n trips $N, sending $M to the ground.`*", ch, NULL, victim,
             TO_NOTVICT);
        check_improve (ch, gsn_trip, TRUE, 1);

        WAIT_STATE (victim, 2 * PULSE_VIOLENCE);
        WAIT_STATE (ch, skill_table[gsn_trip].beats);
        victim->position = POS_RESTING;
        dam =number_range (2, 2 + 2 * victim->size);
        dam += int(GET_DAMROLL (ch) * UMIN (100, skill) / 100.0);
        damage (ch, victim,dam , gsn_trip,
                DAM_BASH, TRUE, FALSE);
    }
    else
    {
        damage (ch, victim, 0, gsn_trip, DAM_BASH, TRUE, FALSE);
        WAIT_STATE (ch, skill_table[gsn_trip].beats * 2 / 3);
        check_improve (ch, gsn_trip, FALSE, 1);
    }
 }
void do_blackjack( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int skill;
    int dam;
    char buf[MIL];
    

    one_argument(argument,arg);

    if ( (skill=get_skill(ch,gsn_blackjack)) == 0)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ((victim = get_char_room (ch, NULL,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (ch->fighting != NULL)
    {
	act("Not while you're fighting!",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim->fighting)
    {
	send_to_char("They are moving too fast to blackjack.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(victim,AFF_BLACKJACK))
    {
	act("$E's already been blackjacked.",ch,NULL,victim,TO_CHAR);
	return;
    }
  
    if (IS_AFFECTED(victim,AFF_STILL))
    {
        act("They are stilled and seem too harmless to be blackjacked.",ch,NULL,victim,TO_CHAR);
        return;
    }

    if (IS_AFFECTED(victim,AFF_RESTRAIN))
    {
        act("They are restrained and seem way too harmless to be blackjacked.",ch,NULL,victim,TO_CHAR);
        return;
    }

    if (IS_AFFECTED(victim,AFF_BLACKJACK2))
    {
	act("They are too paranoid to attempt that again.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (victim == ch)
    {
	send_to_char("Very funny.\n\r",ch);
	return;
    }

 

    if (is_safe(ch,victim))
	return;

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
	return;
    }

    WAIT_STATE (ch, skill_table[gsn_blackjack].beats);
    if (calc_chance(ch,victim,gsn_blackjack,FALSE))
    {
	AFFECT_DATA af;
	act("$n is knocked out cold!",victim,NULL,NULL,TO_ROOM);
	act("$n wacks you upside the head!",ch,NULL,victim,TO_VICT);
        dam = dice(5,5);
        dam += int(GET_DAMROLL (ch) * UMIN (100, skill) / 100.0);
        damage(ch,victim,dam,gsn_blackjack,DAM_NONE,TRUE, FALSE);
	stop_fighting (ch, FALSE);
        stop_fighting (victim, FALSE);
	send_to_char("You are knocked out cold!\n\r",victim);
	check_improve(ch,gsn_blackjack,TRUE,2);

	af.where	= TO_AFFECTS;
	af.type 	= gsn_blackjack;
	af.level 	= ch->level;
	af.duration	= 1;
	af.location	= APPLY_NONE;
	af.modifier	= 0;
	af.bitvector 	= AFF_BLACKJACK;
	affect_to_char(victim,&af);
        victim->position = POS_SLEEPING;
        af.type         = gsn_blackjack2;
        af.bitvector    = AFF_BLACKJACK2;
        af.duration     = 10;
        affect_to_char(victim,&af);

    }
    else
    {
       sprintf(buf, "%s tried to BJ from %s, but failed.\n\r", ch->name, victim->name);
       if (!IS_NPC(victim))
         wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
        send_to_char ("Oops you missed!\n\r", ch);
        affect_strip (ch, gsn_sneak);
        STR_REMOVE_BIT (ch->affected_by, AFF_SNEAK);
        affect_strip (ch, gsn_hide);
        STR_REMOVE_BIT (ch->affected_by, AFF_HIDE);
        affect_strip (ch, gsn_shroud);
        STR_REMOVE_BIT (ch->affected_by, AFF_SHROUD);
        sprintf (buf,"%s tried to blackjack you.\n\r", can_see(victim,ch)?PERS(ch,victim, FALSE) : "Someone");
        act (buf, ch, NULL, victim, TO_VICT);
        sprintf (buf,"%s tried to blackjack $N.\n\r", can_see(victim,ch)?PERS(ch,victim, FALSE) : "Someone");
        act (buf, ch, NULL, victim, TO_NOTVICT);
        multi_hit(victim, ch, TYPE_UNDEFINED);
        if (!IS_NPC(victim))
        {
          sprintf (buf,"%s (level %d) tried to BJ %s (level %d)",
                ch->name,ch->level,victim->name,victim->level);
            log_special (buf, PK_TYPE);
        }
	check_improve(ch,gsn_blackjack,FALSE,2);
    }
}
void do_flip (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;


    one_argument (argument, arg);

    if ((get_skill (ch, gsn_flip)) == 0
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_flip].skill_level[ch->cClass]))
    {
        send_to_char ("Flipping? What's that?\n\r", ch);
        return;
    }

    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char ("But you aren't fighting anyone!\n\r", ch);
            return;
        }
    }

    else if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("You do several flips and land on your feet!\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("But $N is your friend!", ch, NULL, victim, TO_CHAR);
        return;
    }
    if (IS_AFFECTED (victim,AFF_AIRBARRIER) && number_percent()<35)
    {
    	send_to_char ("Your attack is repulsed by a barrier of air!\n\r",ch);
    	act ("Your weave of air holds off $N's attack!",ch,NULL,victim, TO_VICT);
        WAIT_STATE(ch,skill_table[gsn_flip].beats);  
	return;
    }


    if (!IS_NPC (victim) && get_skill (victim, gsn_dodge)>0)
    {
    if (number_percent() < (get_skill (victim, gsn_dodge)-75))
    {                            
        act("$n tries to grab you, but you dodge it.`*",ch,NULL,victim,TO_VICT);
        act("$N dodges your attempt to grab him.`*",ch,NULL,victim,TO_CHAR);
        WAIT_STATE(ch,skill_table[gsn_flip].beats);
        return; 
    }
    }
    else if (!IS_NPC (victim) && get_skill(victim,gsn_evade)>0)
    {
    if (number_percent() < (get_skill (victim, gsn_evade)-75))
    {                            
        act("$n tries to grab you, but you evade it.`*",ch,NULL,victim,TO_VICT);
        act("$N evades your attempt to grab him.`*",ch,NULL,victim,TO_CHAR);
        WAIT_STATE(ch,skill_table[gsn_flip].beats);
        return; 
    }
    }
    /* now the attack */
    if (calc_chance(ch,victim,gsn_flip,TRUE))
    {

        act ("$n sends you sprawling with a powerful throw!`*",
             ch, NULL, victim, TO_VICT);
        act ("You grab and throw $N, and send $M flying!`*", ch, NULL, victim,
             TO_CHAR);
        act ("$n sends $N sprawling with a powerful throw.`*", ch, NULL,
             victim, TO_NOTVICT);
        check_improve (ch, gsn_flip, TRUE, 1);

        WAIT_STATE (victim, 2 * PULSE_VIOLENCE);
        WAIT_STATE (ch, skill_table[gsn_flip].beats);
        victim->position = POS_RESTING;
        damage (ch, victim, number_range ((ch->level / 2), (ch->level / 2) + 10 * ch->size + number_percent() / 2),
                gsn_flip, DAM_BASH, TRUE, FALSE);

    }
    else
    {
        damage (ch, victim, 0, gsn_flip, DAM_BASH, FALSE, FALSE);
        act ("You failed to grab $N!`*", ch, NULL, victim, TO_CHAR);
        act ("$n fails to grab $N.`*", ch, NULL, victim, TO_NOTVICT);
        act ("You evade $n's attempt to grab you.`*",
             ch, NULL, victim, TO_VICT);
        check_improve (ch, gsn_flip, FALSE, 1);
        WAIT_STATE (ch, skill_table[gsn_flip].beats * 3 / 2);
    }
  
}

void do_rub( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance=0, dex;

    if ((get_skill (ch, gsn_rub)) == 0
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_rub].skill_level[ch->cClass]))
    {
        send_to_char ("You don't know the first thing about rubbing dummy!\n\r", ch);
        return;
    }

    if (!str_cmp("warrior", class_table[ch->cClass].name) && ch->pcdata->stance != STANCE_DEFENSIVE)
    {
        send_to_char ("You must be in an defensive style to use this skill.\n\r", ch);
        return;
    }

    one_argument(argument, arg);

        if( IS_AFFECTED(ch,AFF_BLIND))
        {
                if( !is_affected(ch, skill_lookup("fire breath")) &&
                        !is_affected(ch, skill_lookup("dirt kicking")))
                {
                        send_to_char("Rubbing your eyes won't help that!\n\r",ch);
                        return;
                }
        }
        else
        {
                send_to_char("But you aren't blinded!\n\r",ch);
                return;
        }

    if( ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

        //*** Calculate chance of success.
        chance += (get_curr_stat(ch,STAT_DEX));

        chance /= 2;
	
        if( (dex = get_curr_stat(ch,STAT_DEX)) > 19)
                chance += dex;
        else
                chance -= UMIN(5, dex-15);

        if( IS_AFFECTED(ch,AFF_HASTE))
                chance += 10;

        if( IS_AFFECTED(ch,AFF_SLOW))
                chance -= 5;

    if( chance < 0)
                chance = 0;

    if( number_percent() < chance )
    {
                send_to_char("You cleared your eyes!\n\r",ch);
//        act("$n rubbed $s eyes clear!",ch,NULL,NULL,TO_ROOM);

                if( is_affected(ch, skill_lookup("fire breath")))
                        affect_strip(ch, skill_lookup("fire breath"));
        if( is_affected(ch, skill_lookup("dirt kicking")))
            affect_strip(ch, skill_lookup("dirt kicking"));

        check_improve(ch,gsn_rub,TRUE,2);
        WAIT_STATE(ch,skill_table[gsn_rub].beats);
    }
    else
    {
        send_to_char("You failed to rub your eyes clear!\n\r",ch);
        check_improve(ch,gsn_rub,FALSE,2);
        WAIT_STATE(ch,skill_table[gsn_rub].beats);
    }
}

void do_dance(CHAR_DATA *ch, char *argument )
{
 char buf[MAX_STRING_LENGTH];
 char arg1[MAX_STRING_LENGTH];

 argument = one_argument(argument, arg1);

 if(IS_NPC(ch))
    return;

    if ((get_skill (ch, gsn_dance)) == 0
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_dance].skill_level[ch->cClass]))
    {
        send_to_char ("Since you don't know any dances you begin dancing around like a fool!\n\r", ch);
        return;
    }

 if(arg1[0] == '\0')
 {
  sprintf(buf, "Current dance: %s\n\r",stance_table[ch->pcdata->stance].name);
  send_to_char(buf,ch);
  return;
 }

 if(strcmp(arg1, "whirlwind")
 && strcmp(arg1, "leaf")
 && strcmp(arg1, "lightning")
 // && strcmp(arg1, "straw")
 && strcmp(arg1, "stone")
 && strcmp(arg1, "cat")
 && strcmp(arg1, "none"))
 {
 send_to_char("Thats not a dance...\n\r",ch);
 return;
 }

 if(!strcmp(arg1, "none"))
 {
 act("$n relaxes from their dance.",ch,NULL,NULL,TO_ROOM);
 act("You relax from your dance.",ch,NULL,NULL,TO_CHAR);
 check_improve (ch, gsn_dance, TRUE, 7);
 ch->pcdata->stance = 0;
 return;
 }

 if(!strcmp(arg1, "whirlwind"))
 {
 act("$n takes in a breeze whirling it around into a dance.",ch,NULL,NULL,TO_ROOM);
 act("You take in the breeze whirling it around into a dance.",ch,NULL,NULL,TO_CHAR);
 ch->pcdata->stance = 1;
 return;
 }

 if(!strcmp(arg1, "leaf"))
 {
 act("$n forms a dance into a leaf.",ch,NULL,NULL,TO_ROOM);
 act("You form a dance into a leaf.",ch,NULL,NULL,TO_CHAR);
 ch->pcdata->stance = 2;
 return;
 }

 if(!strcmp(arg1, "lightning"))
 {
 act("$n forms a three prong dance as lightning approaches.",ch,NULL,NULL,TO_ROOM);
 act("You wield the three prong dance encompassing the air with lightning.",ch,NULL,NULL,TO_CHAR);
 ch->pcdata->stance = 3;
 return;
 }
/*
 if(!strcmp(arg1, "straw"))
 {
 act("$n bundles together to dance like a straw.",ch,NULL,NULL,TO_ROOM);
 act("You bundle your body together and begin dancing like a straw",ch,NULL,NULL,TO_CHAR);
 ch->pcdata->stance = 4;
 return;
 }
*/
 if(!strcmp(arg1, "stone"))
 {
 act("$n encompasses the surroundings dancing like a stone falling from a mountain",ch,NULL,NULL,TO_ROOM);
 act("You begin to rumble and dance as stones fall from a mountain.",ch,NULL,NULL,TO_CHAR);
 ch->pcdata->stance = 5;
 return;
 }

 if(!strcmp(arg1, "cat"))
 {
 act("$n begins to dance as a cat on hot sand.",ch,NULL,NULL,TO_ROOM);
 act("You feel the hot sand flaming your feet as you dance like a cat!",ch,NULL,NULL,TO_CHAR);
 ch->pcdata->stance = 6;
 return;
 }
}

void do_style(CHAR_DATA *ch, char *argument )
{
 char buf[MAX_STRING_LENGTH];
 char arg1[MAX_STRING_LENGTH];

 argument = one_argument(argument, arg1);

 if(IS_NPC(ch))
    return;

    if ((get_skill (ch, gsn_style)) == 0
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_style].skill_level[ch->cClass]))
    {
        send_to_char ("Only a true Warrior can switch his style you fool.\n\r", ch);
        return;
    }

 if(arg1[0] == '\0')
 {
  sprintf(buf, "Current style: %s\n\r",stance_table[ch->pcdata->stance].name);
  send_to_char(buf,ch);
  return;
 }

 if(strcmp(arg1, "offensive")
 && strcmp(arg1, "defensive")
 && strcmp(arg1, "none"))
 {
 send_to_char("Thats not a style...\n\r",ch);
 return;
 }

 if(!strcmp(arg1, "none"))
 {
 act("$n relaxes from style coordination, taking a breath.",ch,NULL,NULL,TO_ROOM);
 act("You relax from style coordination, taking a breath.",ch,NULL,NULL,TO_CHAR);
 check_improve (ch, gsn_style, TRUE, 7);
 ch->pcdata->stance = 0;
 return;
 }

 if(!strcmp(arg1, "offensive"))
 {
 act("$n switches into an offensive style.",ch,NULL,NULL,TO_ROOM);
 act("You switch into a offensive style.",ch,NULL,NULL,TO_CHAR);
 ch->pcdata->stance = 7;
 return;
 }

 if(!strcmp(arg1, "defensive"))
 {
 act("$n switches into a defensive style.",ch,NULL,NULL,TO_ROOM);
 act("You switch into a defensive style..",ch,NULL,NULL,TO_CHAR);
 ch->pcdata->stance = 8;
 return;
 }
}

void do_gash( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int chance;
    int dt = gsn_gash;

    if ((get_skill (ch, gsn_gash)) == 0
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_gash].skill_level[ch->cClass]))
    {
        send_to_char ("Gashing?  Hrmm?\n\r", ch);
        return;
    }

    if( (victim = ch->fighting) == NULL )
    {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
    }

    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

    if( ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_gash].beats );

        chance = (get_skill(ch,gsn_gash)/3);

        if( number_percent() <= chance )
	{
	ch->move-=25;
        one_hit(ch, victim, dt, FALSE);
        one_hit(ch, victim, dt, FALSE);
        one_hit(ch, victim, dt, FALSE);
        check_improve(ch,gsn_gash,FALSE,1);
        return;
	}
        else if( number_percent() > chance )
	{
	ch->move-=25;
        one_hit(ch, victim, dt, FALSE);
        one_hit(ch, victim, dt, FALSE);
        check_improve(ch,gsn_gash,FALSE,1);
        return;
	}
        else
	{
        ch->move-=25;
        one_hit(ch, victim, dt, FALSE);
        one_hit(ch, victim, dt, FALSE);
        check_improve(ch,gsn_gash,FALSE,1);
        return;
        }
}

void do_caltrops(CHAR_DATA *ch, char *argument)
{
        CHAR_DATA *victim;
        int chance;

        if( (chance = get_skill(ch,gsn_caltraps)) < 1)
  	  {
                send_to_char("Caltrops? Is that a dance step?\n\r",ch);
                return;
   	  }

        if( (victim = ch->fighting) == NULL)
    	{
                send_to_char("You must be in combat.\n\r",ch);
                return;
    	}

        if (ch->stunned)
        {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
        }

        if( is_safe(ch,victim))
                return;

        act("You throw a handful of sharp spikes at the feet of $N.", ch,NULL,victim,TO_CHAR);
        act("$n throws a handful of sharp spikes at your feet!", ch,NULL,victim,TO_VICT);


        if( number_percent() > chance )
    {
                damage(ch,victim,0,gsn_caltraps,DAM_PIERCE, TRUE,FALSE);
                check_improve(ch,gsn_caltraps,FALSE,1);
                WAIT_STATE(ch,skill_table[gsn_caltraps].beats);
                return;
    }

        damage(ch,victim,number_range(ch->level/2, ch->level),gsn_caltraps,DAM_PIERCE, TRUE, FALSE);
        check_improve(ch,gsn_caltraps,TRUE,1);
                WAIT_STATE(ch,skill_table[gsn_caltraps].beats);

        if( number_percent() < chance/3
                && !IS_AFFECTED2(victim, AFF_LIMP))
        {
                AFFECT_DATA af;

                af.where         = TO_AFFECTS;
                af.type      = gsn_caltraps;
                af.level         = ch->level;
                af.duration  = 1;
                af.location  = APPLY_DEX;
                af.modifier  = -4;
                af.bitvector = AFF_LIMP;
                affect_to_char(victim, &af);

                act("$n starts limping.",victim,NULL,NULL,TO_ROOM);
                send_to_char("Ouch that hurt! You start limping.\n\r",victim);
        }

}

void do_salve( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af, *paf;
    CHAR_DATA *victim;
    bool found = FALSE;

    if ((get_skill (ch, gsn_salve)) == 0
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_salve].skill_level[ch->cClass]))
    {
        send_to_char ("Salving?  Salvage this you piece of!\n\r", ch);
        return;
    }

    if ( argument[0] == '\0' )
        victim = ch;

    else if ( ( victim = get_char_room( ch, NULL, argument ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if (is_affected (ch, gsn_salve) )
    {
        send_to_char( "You are already affected by Salve.\n\r", ch);
        return;
    }

    if ( victim->hit == victim->max_hit )
    {
        if ( ch == victim )
            send_to_char( "You aren't even wounded.\n\r", ch );
        else
            send_to_char( "They aren't even wounded.\n\r", ch );
        return;
    }

    ch->move -= 10;

    WAIT_STATE( ch, skill_table[gsn_salve].beats );

    if ( number_range(1,4) == 3 ) 
    {
        send_to_char( "You failed.\n\r", ch );
        check_improve( ch, gsn_salve, FALSE, 2 );
        return;
    }

    victim->hit = UMIN( victim->hit+100, victim->max_hit );
    check_improve( ch, gsn_salve, TRUE, 2 );

    if ( ch == victim )
    {
        act( "$n applies salve to $s wounds.",
            ch, NULL, victim, TO_ROOM);
        act( "You apply salve to your wounds.",
            ch, NULL, victim, TO_CHAR);
    } else {
        act( "$n applies salve to $N's wounds.",
            ch, NULL, victim, TO_NOTVICT);
        act( "$n applies salve to your wounds.",
            ch, NULL, victim, TO_VICT);
        act( "You apply salve to $N's wounds.",
            ch, NULL, victim, TO_CHAR);
    }

    for ( paf = victim->affected; paf != NULL; paf = paf->next )
    {
        if ( paf->where == TO_AFFECTS && paf->type == gsn_salve )
        {
            found               = TRUE;
            paf->duration       = ch->level / 20;
            paf->modifier       = UMIN( paf->modifier + 5, 100 );
        }
    }

    if ( !found )
    {
        af.where        = TO_AFFECTS;
        af.type         = gsn_salve;
        af.duration     = ch->level / 20;
        af.location     = APPLY_HITROLL;
        af.modifier     = number_range(10,35);
        af.bitvector    = 0;
        affect_to_char( victim, &af );

        af.location     = APPLY_DAMROLL;
        affect_to_char( victim, &af );
    }
}

void dislodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;

    if ((obj = get_eq_char (victim, WEAR_SHIELD)) == NULL)
        return;

    act ("$n DISLODGES your SHIELD and sends it flying!`*",
         ch, NULL, victim, TO_VICT);
    act ("You DISLODGE $N's shield!`*", ch, NULL, victim, TO_CHAR);
    act ("$n DISLODGES $N's shield!`*", ch, NULL, victim, TO_NOTVICT);

    obj_from_char (obj);
    if (IS_OBJ_STAT (obj, ITEM_NODROP) || IS_OBJ_STAT (obj, ITEM_INVENTORY))
        obj_to_char (obj, victim);
    else
    {
        if (!IS_NPC(victim) && (IS_SET(victim->act, PLR_TOURNEY) ||
            IS_SET(victim->act, PLR_QUESTING)))
        {
          obj_to_char(obj, victim);
        }
        else
          !IS_NPC(victim)?obj_to_room (obj,victim->in_room):obj_to_char(obj,victim);
        if (IS_NPC (victim) && !obj->carried_by && victim->wait == 0 && can_see_obj (victim, obj))
            get_obj (victim, obj, NULL, FALSE);
    }

    return;

}

void do_dislodge (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;


    if ((get_skill (ch, gsn_dislodge)) == 0
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_dislodge].skill_level[ch->cClass]))
    {
        send_to_char ("Dislodging? What's that?\n\r", ch);
        return;
    }

    if (!str_cmp("warrior", class_table[ch->cClass].name) && ch->pcdata->stance != STANCE_DEFENSIVE)
    {
        send_to_char ("You must be in an defensive style to use this skill.\n\r", ch);
        return;
    }

    if ((victim = ch->fighting) == NULL)
    {
        send_to_char ("You aren't fighting anyone.\n\r", ch);
        return;
    }

    if ((obj = get_eq_char (victim, WEAR_SHIELD)) == NULL)
    {
        send_to_char ("Your opponent is not wielding a shield.\n\r", ch);
        return;
    }


    /* and now the attack */
    if (calc_chance(ch,victim,gsn_dislodge,TRUE))
    {
        WAIT_STATE (ch, skill_table[gsn_dislodge].beats);
        dislodge (ch, victim);
        check_improve (ch, gsn_dislodge, TRUE, 1);
    }
    else
    {
        WAIT_STATE (ch, skill_table[gsn_dislodge].beats);
        act ("You fail to dislodge $N's shield.`*", ch, NULL, victim, TO_CHAR);
        act ("$n tries to dislodge your shield you, but fails.`*", ch, NULL, victim,
             TO_VICT);
        act ("$n tries to dislodge $N's shield, but fails.`*", ch, NULL, victim,
             TO_NOTVICT);
        check_improve (ch, gsn_dislodge, FALSE, 1);
    }
    return;
}

void do_quickening( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if ((get_skill (ch, gsn_quickening)) == 0
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_quickening].skill_level[ch->cClass]))
    {
        send_to_char ("Quicky quicky? what's that?\n\r", ch);
        return;
    }

    if ( IS_AFFECTED( ch, AFF_HASTE ) )
    {
        send_to_char( "You can't move any faster!\n\r", ch );
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_quickening].beats );

    if ( IS_AFFECTED( ch, AFF_SLOW ) )
    {
        if( number_range(1, 4) == 2)
        {
            send_to_char( "You feel momentarily faster.\n\r", ch );
            return;
        }

        act( "$n is moving less slowly.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "You feel faster.\n\r", ch );
        return;
    }

    af.where    = TO_AFFECTS;
    af.type     = gsn_quickening;
    af.level    = ch->level;
    af.duration = ch->level / 4;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector= AFF_HASTE;

    affect_to_char( ch, &af );

    send_to_char( "You feel yourself moving more quickly.\n\r", ch );
    act( "$n is moving more quickly.", ch, NULL, NULL, TO_ROOM );
}

void do_gouge (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int skill=get_skill (ch, gsn_gouge);
    int dam;
    int hitrollmod;

    one_argument (argument, arg);

    if (( get_skill (ch, gsn_gouge)) == 0
        || (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_KICK_DIRT))
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_gouge].skill_level[ch->cClass]))
    {
        send_to_char ("You don't know how to poke anyone.\n\r", ch);
        return;
    }

    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char ("But you aren't in combat!\n\r", ch);
            return;
        }
    }

    else if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim->position != POS_SLEEPING)
    {
	send_to_char( "Your victim is awake, and too wary to be gouged.", ch);
	return;
    }

    if (is_affected (victim, gsn_gouge))
    {
        send_to_char( "They have already been gouged.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (victim, AFF_BLIND))
    {
        act ("$E's already been blinded.", ch, NULL, victim, TO_CHAR);
        return;
    }


    if (victim == ch)
    {
        send_to_char ("Very funny.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("But $N is such a good friend!", ch, NULL, victim, TO_CHAR);
        return;
    }

    send_to_char ("You blindly throw your hands.\n\r", ch);

    if (IS_AFFECTED (victim,AFF_AIRBARRIER) && number_percent() < 35)
    {
        send_to_char ("Your attack is repulsed by a barrier of air!\n\r",ch);
        act ("Your weave of air holds off $n's attack!",ch,NULL,victim, TO_VICT);
        WAIT_STATE (ch, skill_table[gsn_gouge].beats / 2);
        return;
    }

    /* now the attack */
    if (calc_chance(ch,victim,gsn_gouge,FALSE))
    {
        AFFECT_DATA af;
        act ("$n is blinded by poke $s eyes!`*", victim, NULL, NULL,
             TO_ROOM);
        act ("$n pokes you in your eyes, gouging you!`*", ch, NULL, victim, TO_VICT);
        dam = number_range (2, 5);
        dam += int(GET_DAMROLL (ch) * UMIN (100, skill) / 100.0);
        damage (ch, victim, dam, gsn_gouge, DAM_NONE, TRUE, FALSE);
        send_to_char ("You can't see a thing!`*\n\r", victim);
        check_improve (ch, gsn_gouge, TRUE, 2);
        WAIT_STATE (ch, skill_table[gsn_gouge].beats);
        hitrollmod = (victim->hitroll / 5) * -1;

        af.where = TO_AFFECTS;
        af.type = gsn_gouge;
        af.level = ch->level;
        af.duration = number_range(0,1);
        af.location = APPLY_HITROLL;
        af.modifier = hitrollmod;
        af.bitvector = AFF_GOUGE;

        affect_to_char (victim, &af);

        if (number_percent() > 75)
     	{
	        af.bitvector = AFF_LIMP;
       	 	affect_to_char( victim, &af );
                act("$n starts limping.",victim,NULL,NULL,TO_ROOM);
                send_to_char("Ouch that hurt! You start limping.\n\r",victim);
		return;
	}
	else if (number_percent() < 25)
	{
                af.bitvector = AFF_FEAR;
                affect_to_char( victim, &af );
                act("$n becomes very afraid.",victim,NULL,NULL,TO_ROOM);
                send_to_char("You send fear into your opponent's soul.\n\r",ch);
		return;
	}
	else if (number_range(1,5) == 3)
	{
	        af.bitvector = AFF_DISORIENT;
                affect_to_char( victim, &af );
                act("$n gets disoriented.",victim,NULL,NULL,TO_ROOM);
                send_to_char("You become disoriented.\n\r",victim);
		return;
	}
	else
	  return;
    }
    else
    {
        damage (ch, victim, 0, gsn_gouge, DAM_NONE, TRUE, FALSE);
        check_improve (ch, gsn_gouge, FALSE, 2);
        WAIT_STATE (ch, skill_table[gsn_gouge].beats);
	return;
    }

}

void do_unseen_fury( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af, *paf;
    CHAR_DATA *victim;
    bool found = FALSE;

    if ((get_skill (ch, gsn_unseen_fury)) == 0
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_unseen_fury].skill_level[ch->cClass]))
    {
        send_to_char ("The only fury you have is your dim witted attitude!\n\r", ch);
        return;
    }

    if ( argument[0] == '\0' )
        victim = ch;

    else if ( ( victim = get_char_room( ch, NULL, argument ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if (is_affected (ch, gsn_unseen_fury) )
    {
        send_to_char( "You are already affected by an unseen fury.\n\r", ch);
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_unseen_fury].beats );

    if ( number_range(1,4) == 3 )
    {
        send_to_char( "You failed.\n\r", ch );
        check_improve( ch, gsn_unseen_fury, FALSE, 2 );
        return;
    }

    victim->hit = UMIN( victim->hit+100, victim->max_hit );
    check_improve( ch, gsn_unseen_fury, TRUE, 2 );

    if ( ch == victim )
    {
        send_to_char("Your fury begins.", ch);
    } else {
         send_to_char("Only you can be affected by this", ch);
	 return;
    }

    for ( paf = victim->affected; paf != NULL; paf = paf->next )
    {
        if ( paf->where == TO_AFFECTS && paf->type == gsn_unseen_fury )
        {
            found               = TRUE;
            paf->duration       = ch->level / 20;
            paf->modifier       = UMIN( paf->modifier + 5, 100 );
        }
    }

    if ( !found )
    {
        af.where        = TO_AFFECTS;
        af.type         = gsn_unseen_fury;
        af.duration     = 0;
        af.location     = APPLY_HITROLL;
        af.modifier     = number_range(30,55);
        af.bitvector    = 0;
        affect_to_char( victim, &af );

        af.location     = APPLY_DAMROLL;
        affect_to_char( victim, &af );
    }
}

void do_darkhound(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *darkhound, *gch;
  MOB_INDEX_DATA *pMobIndex;
  AFFECT_DATA af;
  int sn;

  if (get_skill(ch, gsn_darkhound) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_SET(ch->act, PLR_TOURNEY) && tournament.noguild)
  {
    send_to_char("Not while in this tournament.\n\r", ch);
    return;
  }

  if (ch->darkcounter >= 3)
  {
    send_to_char("You cannot call anymore blindworms to aid you.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_DARKHOUND))
  {
    send_to_char("It's to early to call another blindworms.\n\r", ch);
    return;
  }

  int counter=0;

  if (!IS_AFFECTED(ch, AFF_DARKHOUND))
  {
     counter = 0;
  }

  for (gch = char_list; gch != NULL; gch = gch->next)
  {
     if (is_same_group (gch, ch))
     {
       if (IS_NPC(gch))
       {
	 counter += 1;
       }
     }
  }

         if (counter > 2)
         {
           send_to_char("You have too many blindworms already.\n\r", ch);
           return;
         }

  if (number_percent() < get_skill(ch, gsn_darkhound))
  {
   if (counter >= 2)
   {
    af.where = TO_AFFECTS;
    af.type = gsn_darkhound;
    af.level = ch->level;
    af.duration = 5;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_DARKHOUND;

    affect_to_char (ch, &af);
   }

    pMobIndex = get_mob_index( MOB_VNUM_DARKHOUND );
    if ((darkhound = create_mobile( pMobIndex )) == NULL)
      return;
     char_to_room( darkhound, ch->in_room );
    add_follower( darkhound, ch);
    darkhound->leader = ch;
    darkhound->master = ch;
    darkhound->max_hit = int(ch->max_hit * .25);
    darkhound->hit = darkhound->max_hit;
    darkhound->level = int(ch->level * .8);
    darkhound->hitroll = int(ch->hitroll * .8);
    darkhound->damroll = int(ch->damroll * .8);
    ch->pet = darkhound;
    sn = skill_lookup("Charm");
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = 200;
    af.duration = number_range(3,4);
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char (darkhound, &af);

    act( "$n has called a blindworm to $s side.\n\r", ch, NULL, NULL, TO_ROOM);
    send_to_char( "You have called upon a blindworm.\n\r", ch);
    check_improve(ch, gsn_darkhound, FALSE, 1);
  }
  else
  {
    send_to_char("You fail to call a blindworm.\n\r", ch);
    check_improve(ch, gsn_darkhound, FALSE, 1);
  }
}

