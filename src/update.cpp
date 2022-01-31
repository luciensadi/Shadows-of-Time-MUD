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
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "music.h"
#include "tables.h"
#include "lookup.h"
#include "recycle.h"

/*
 * Local functions.
 */
void check_all_games args ((void ));
void    quest_update    args( ( void ) ); /* Vassago - quest.c */
int hit_gain args ((CHAR_DATA * ch));
int mana_gain args ((CHAR_DATA * ch));
int move_gain args ((CHAR_DATA * ch));
void mobile_update args ((void));
void weather_update args ((void));
void time_update args ((void));
void char_update args ((void));
void obj_update args ((void));
//void war_update args ((void));
void aggr_update args ((void));
//void auction_update args ((void));
void copyover_update  args ((void));
void warticks_update  args ((void));
void raw_kill args ((CHAR_DATA * ch,CHAR_DATA * victim));
void dtrap_update    args( ( void ) );
void save_update     args( ( void ) );
void counter_update  args( ( void ) );
void save_vote       args( (  void ) );
void save_mudstats       args( (  void ) );
void save_gameinfo   args( (  void ) );
void save_guilds     args( (  void ) );
void save_stores      args (( void ));
void web_mudstats     args (( void ));
void initialize_list args(( void ));
void tournament_update args(( void ));
void timer_update args (( void ));
void spellmob_detect args (( CHAR_DATA *ch));
void dirtkick_update args ((void));
void member_rp args (( char *name, int clan, int level ));
void check_mudstats();
void system_check args ((void ));
void round_print args (( void ));

DECLARE_DO_FUN(do_spellup);
DECLARE_DO_FUN(do_restore);
DECLARE_DO_FUN (do_bonus_time);

/* used for saving */

int save_number = 0;



/*
 * Advancement stuff.
 */
void advance_level (CHAR_DATA * ch, bool advance, bool hide)
{
    char buf[MAX_STRING_LENGTH];
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;
    CHAR_DATA *creator;

    creator = get_char_world(ch, "Creator");

    ch->pcdata->last_level =
        (ch->played + (int) (current_time - ch->logon)) / 3600;

/*  Yuck  sprintf (buf, "the %s",
             title_table[ch->cClass][ch->level][ch->sex ==
                                               SEX_FEMALE ? 1 : 0]);  
  set_title (ch, buf);
*/
    add_hp =
        con_app[get_curr_stat (ch, STAT_CON)].hitp +
        number_range (class_table[ch->cClass].hp_min,
                      class_table[ch->cClass].hp_max);

    if (ch->clan == clan_lookup("Sedai") || ch->clan == clan_lookup("Rebel") )
      add_hp += number_range(1,2);
    add_mana    = (get_curr_stat(ch, STAT_INT) / 5) + number_range(
                    class_table[ch->cClass].mana_min,
                    class_table[ch->cClass].mana_max);                   
    //if (!class_table[ch->cClass].fMana)
    //    add_mana /= 2;
    add_move = number_range (1, (get_curr_stat (ch, STAT_CON)
                                 + get_curr_stat (ch, STAT_DEX)) / 3);
    add_prac = wis_app[get_curr_stat (ch, STAT_WIS)].practice;

    add_hp = add_hp * 9 / 10;
    add_mana = add_mana * 9 / 10;
    add_move = add_move * 9 / 10;

    add_hp = UMAX (2, add_hp);
    add_mana = UMAX (2, add_mana);
    add_move = UMAX (6, add_move);

    if (advance)
    {
        ch->level += 1;
        ch->train += 1;
        ch->max_hit += add_hp;
        ch->max_mana += add_mana;
        ch->max_move += add_move;
        ch->practice += add_prac;
        ch->pcdata->perm_hit += add_hp;
        ch->pcdata->perm_mana += add_mana;
        ch->pcdata->perm_move += add_move;
    }
    else
    {
	ch->level -= 1;
        ch->train -= 1;
        ch->max_hit -= add_hp;
        ch->max_mana -= add_mana;
	ch->max_move -= add_move;
	ch->practice -= add_prac;
	ch->pcdata->perm_hit -= add_hp;
	ch->pcdata->perm_mana -= add_mana;
	ch->pcdata->perm_move -= add_move;
    }

    if (!hide)
    {
    	if (advance)
    	{
            sprintf (buf,
                 "You gain %d hit point%s, %d mana, %d move, and %d practice%s.\n\r",
                 add_hp, add_hp == 1 ? "" : "s", add_mana, add_move,
                 add_prac, add_prac == 1 ? "" : "s");
            send_to_char (buf, ch);
        }
        else
        {
            sprintf (buf,
                 "You lose %d hit point%s, %d mana, %d move, and %d practice%s.\n\r",
                 add_hp, add_hp == 1 ? "" : "s", add_mana, add_move,
                 add_prac, add_prac == 1 ? "" : "s");
            send_to_char (buf, ch);
        }
        
    }
    do_restore(creator, ch->name); 
    return;
}

void gain_exp (CHAR_DATA * ch, int gain)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC (ch))
        return;

    if (ch->level >= 75)
    {
	ch->realexp += (gain);
	return;
    }

    ch->exp = UMAX (exp_per_level (ch, ch->pcdata->points), ch->exp + gain);
    while (ch->level < MAX_NORMAL && ch->exp >=
           exp_per_level (ch, ch->pcdata->points) * (ch->level + 1))
    {
        send_to_char ("You raise a level!!\n\r", ch);
	if (ch->level == 35 && IS_SET(ch->act, PLR_NOPK))
	{
	  send_to_char("`&Look out, your safe status has been dropped you are now pkable.`*\n\r", ch);
	  REMOVE_BIT(ch->act, PLR_NOPK);
	}

	if (ch->level == 25 && IS_SET(ch->comm, COMM_NOVICE))
	{
	  send_to_char("`&Congratulations you have graduated from your newbie class.\n\r" 
                       "The Novice channel has been turned off.`*\n\r", ch);
	  REMOVE_BIT(ch->comm, COMM_NOVICE);
	}
        sprintf (buf, "%s gained level %d\n\r", ch->name, ch->level+1);
        log_string (buf);
        sprintf (buf, "$N has attained level %d!\n\r", ch->level+1);
        wiznet (buf, ch, NULL, WIZ_LEVELS, 0, 0);
        advance_level (ch,TRUE,FALSE);
        save_char_obj (ch);
    }

    return;
}

/*
 * Regeneration stuff.
 */
int hit_gain (CHAR_DATA * ch)
{
    int gain;
    int number;

    if (ch->in_room == NULL)
        return 0;

    if (IS_NPC (ch))
    {
        gain = 5 + ch->level;
        if (IS_AFFECTED (ch, AFF_REGENERATION))
            gain *= 2;

        switch (ch->position)
        {
            default:
                gain /= 2;
                break;
            
            case POS_SLEEPING:
                gain = 3 * gain / 2;
                break;
            case POS_RESTING:
                break;
            case POS_FIGHTING:
                gain /= 3;
                break;
        }


    }
    else
    {
        gain = UMAX (3, int(get_curr_stat (ch, STAT_CON) - 3 + ch->level / 1.5));
        if (ch->pcdata->pk_timer == 0 && ch->max_hit > 2000)
          gain += UMIN(((ch->max_hit - 2000) / 50), 35);
        gain += class_table[ch->cClass].hp_max - 5;
        number = number_percent ();
        if (number < get_skill (ch, gsn_fast_healing))
        {
            gain += number * gain / 100;
            if (ch->hit < ch->max_hit)
                check_improve (ch, gsn_fast_healing, TRUE, 8);
        }

        switch (ch->position)
        {
            default:
                gain /= 4;
                break;
            case POS_SLEEPING:
                break;
            case POS_RESTING:
                gain /= 2;
                break;
            case POS_FIGHTING:
                gain /= 6;
                break;
            case POS_MEDITATING:
                //gain +=gain*((get_skill(ch,gsn_meditation)+40)/100);
                break;
        }

 /*	if ( ch->in_room->vnum == ch->pcdata->rentroom  && ch->silver > 10)
   		gain *= 2;
 	if ( IS_SET(ch->act, PLR_TENNANT) && ch->silver > 10)
   		ch->silver -= 10;                  Room Rental  */
        if ( ch->clan == 0)
                gain = int(gain * 1.5);
        if ( get_obj_list(ch, "burning fire", ch->in_room->contents) != NULL )
                gain = int(gain * 1.5); 

        if (IS_AFFECTED(ch, AFF_REGENERATION))
        {
           gain = int (gain * 1.35);
        }



    }

    gain = gain * ch->in_room->heal_rate / 100;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
        gain = gain * ch->on->value[3] / 100;

    if (IS_SET(ch->act, PLR_TOURNEY))
        gain *= 2;

    if (IS_AFFECTED (ch, AFF_CONTACT))
        gain /= 10; 
 
    if (IS_AFFECTED (ch, AFF_POISON))
        gain /= 4;

    if (IS_AFFECTED (ch, AFF_PLAGUE))
        gain /= 8;

    if (IS_AFFECTED (ch, AFF_HASTE) || IS_AFFECTED (ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_STICKY))
        gain /= 2;

    if (IS_AFFECTED(ch, AFF_RAVAGE))
        gain /= 8;   


    return UMIN (gain, ch->max_hit - ch->hit);
}



int mana_gain (CHAR_DATA * ch)
{
    int gain;
    int number;

    if (ch->in_room == NULL)
        return 0;

    if (IS_NPC (ch))
    {
        gain = 5 + ch->level;
        switch (ch->position)
        {
            default:
                gain /= 2;
                break;
            case POS_SLEEPING:
                gain = 3 * gain / 2;
                break;
            case POS_RESTING:
                break;
            case POS_FIGHTING:
                gain /= 3;
                break;
        }
    }
    else
    {
        gain = int((get_curr_stat (ch, STAT_WIS)
                + get_curr_stat (ch, STAT_INT) + ch->level) / 1.5);
        if (ch->pcdata->pk_timer == 0 && ch->max_mana > 1100)
          gain += UMIN(((ch->max_mana - 2000) / 50), 35);
        number = number_percent ();
        if (number < get_skill (ch, gsn_meditation))
        {
            gain += number * gain / 100 + (ch->level / 2);
            if (ch->mana < ch->max_mana)
                check_improve (ch, gsn_meditation, TRUE, 8);
        }
        //if (!class_table[ch->cClass].fMana)
        //    gain /= 2;

        switch (ch->position)
        {
            default:
                gain /= 4;
                break;
            case POS_SLEEPING:
                break;
            case POS_RESTING:
                gain /= 2;
                break;
            case POS_FIGHTING:
                gain /= 6;
                break;
            case POS_MEDITATING:
              //  gain +=gain*((get_skill(ch,gsn_meditation)+40)/100);
                break;
        }

//		if (ch->pcdata->condition[COND_BLEEDING] != 0)
//			gain /= 6;

        if ( ch->clan == 0)
                gain = int(gain * 1.5);

        if ( get_obj_list(ch, "burning fire", ch->in_room->contents) != NULL )
                gain = int(gain * 1.5); 

    }

    gain = gain * ch->in_room->mana_rate / 100;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
        gain = gain * ch->on->value[4] / 100;

    if (IS_SET(ch->act, PLR_TOURNEY))
        gain *= 2;

    if (IS_AFFECTED (ch, AFF_CONTACT))
       gain /= 10;

    if (IS_AFFECTED (ch, AFF_POISON))
        gain /= 4;

    if (IS_AFFECTED (ch, AFF_PLAGUE))
        gain /= 8;

    if (IS_AFFECTED (ch, AFF_HASTE) || IS_AFFECTED (ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_STICKY))
        gain /= 2;

    if (IS_AFFECTED(ch, AFF_RAVAGE))
        gain /= 6;   


    return UMIN (gain, ch->max_mana - ch->mana);
}



int move_gain (CHAR_DATA * ch)
{
    int gain;

    if (ch->in_room == NULL)
        return 0;

    if (IS_NPC (ch))
    {
        gain = ch->level;
    }
    else
    {
        gain = UMAX (15, ch->level);

        switch (ch->position)
        {
            case POS_SLEEPING:
                gain += get_curr_stat (ch, STAT_DEX);
                break;
            case POS_RESTING:
                gain += get_curr_stat (ch, STAT_DEX) / 2;
                break;
            case POS_MEDITATING:
               gain += get_curr_stat (ch, STAT_DEX);// + gain*((get_skill(ch,gsn_meditation)+40)/100);
                break;
        }

//		if (ch->pcdata->condition[COND_BLEEDING] != 0)
//			gain /= 6;
        if ( ch->clan == 0)
                gain = int(gain * 1.25);

        if ( get_obj_list(ch, "burning fire", ch->in_room->contents) != NULL )
                gain = int(gain * 1.5); 

    }

    gain = gain * ch->in_room->heal_rate / 100;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
        gain = gain * ch->on->value[3] / 100;

    if (IS_SET(ch->act, PLR_TOURNEY))
        gain *= 2;

    if (IS_AFFECTED (ch, AFF_CONTACT))
        gain /= 10;
    if (IS_AFFECTED (ch, AFF_POISON))
        gain /= 4;

    if (IS_AFFECTED (ch, AFF_PLAGUE))
        gain /= 8;

    if (IS_AFFECTED (ch, AFF_HASTE) || IS_AFFECTED (ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_STICKY))
        gain /= 2;

    if (IS_AFFECTED(ch, AFF_RAVAGE))
        gain /= 8;   

   
   return UMIN (gain, ch->max_move - ch->move);
}



void gain_condition (CHAR_DATA * ch, int iCond, int value)
{
    int condition;

    if ((value == 0 || IS_NPC (ch) || ch->level >= LEVEL_IMMORTAL) && iCond != COND_HORNY)
        return;

    condition = ch->pcdata->condition[iCond];
    if (condition == -1)
        return;

    if (ch->sex == 1 &&(ch->clan != clan_lookup("Whitetower")) && ch->clan != clan_lookup("Guide"))
    {
      ch->pcdata->condition[iCond] = URANGE (0, condition + value, 60);
    }
    else
    {
      ch->pcdata->condition[iCond] = URANGE (2, condition + value, 60);
    }

    if (ch->pcdata->condition[iCond] == 0)
    {
        switch (iCond)
        {
            case COND_DRUNK:
                if (condition != 0)
                    send_to_char ("You are sober.\n\r", ch);
                break;

            case COND_HORNY:
                send_to_char ("You are horny.\n\r", ch);
                break;
        }
    }

    return;
}

void aggro_update( void )
{
    CHAR_DATA *ch;
    DESCRIPTOR_DATA *d;
   
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        ch = d->original ? d->original : d->character;
          
        if ( d->connected == CON_PLAYING &&
             !IS_NPC(ch))
        {   
           if (ch->pcdata->aggro_timer > 1)
             ch->pcdata->aggro_timer--;
           if (ch->pcdata->aggro_timer == 1)
           {
              ch->pcdata->aggro_timer--;
              if (IS_SET(ch->act2, PLR_MSP_MUSIC) && !IS_SET(ch->act2, PLR_FLED))
              {
                send_to_char("!!MUSIC(Sounds/Victory.mid V=80 L=0 C=1 T=Fight)\n\r", ch);
              }
              if (IS_SET(ch->act2, PLR_MSP_PLAYING)) 
                REMOVE_BIT(ch->act2, PLR_MSP_PLAYING);

            }
        }
     }
}

/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update (void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    EXIT_DATA *pexit;
    int door;

    /* Examine all mobs. */
    for (ch = char_list; ch != NULL; ch = ch_next)
    {
        ch_next = ch->next;
		
        if (!IS_NPC (ch) || ch->in_room == NULL
            || IS_AFFECTED (ch, AFF_CHARM) || IS_AFFECTED(ch, AFF_SUPER_INVIS))
               continue;

        if (ch->in_room->area->empty && !IS_SET (ch->act, ACT_UPDATE_ALWAYS))
            continue;

        /* Examine call for special procedure */
        if (ch->spec_fun != 0 && (!ch->rescuer || (ch->rescuer && ch->fighting != ch->attacker) || (ch->spec_fun == spec_lookup("spec_rescue")) ))
        {
            if ((*ch->spec_fun) (ch))
                continue;
        }

        if (ch->pIndexData->pShop != NULL)    /* give him some gold */
            if ((ch->gold * 100 + ch->silver) < ch->pIndexData->wealth)
            {
                ch->gold +=
                    ch->pIndexData->wealth * number_range (1, 20) / 4000000;
                ch->silver +=
                    ch->pIndexData->wealth * number_range (1, 20) / 40000;
            }

        /*
         * Check triggers only if mobile still in default position
         */
        if (ch->position == ch->pIndexData->default_pos)
        {
            /* Delay */
            if (HAS_TRIGGER_MOB (ch, TRIG_DELAY) && ch->mprog_delay > 0)
            {
                if (--ch->mprog_delay <= 0)
                {
                    p_percent_trigger (ch, NULL, NULL, NULL, NULL, NULL, TRIG_DELAY);
                    continue;
                }
            }
            if (HAS_TRIGGER_MOB (ch, TRIG_RANDOM))
            {
                if (p_percent_trigger (ch, NULL, NULL, NULL, NULL, NULL, TRIG_RANDOM))
                    continue;
            }
        }

        /* That's all for sleeping / busy monster, and empty zones */
        if (ch->position != POS_STANDING)
            continue;

        /* Scavenge */
        if (IS_SET (ch->act, ACT_SCAVENGER)
            && ch->in_room->contents != NULL && number_bits (6) == 0)
        {
            OBJ_DATA *obj;
            OBJ_DATA *obj_best;
            int max;

            max = 1;
            obj_best = 0;
            for (obj = ch->in_room->contents; obj; obj = obj->next_content)
            {
                if (CAN_WEAR (obj, ITEM_TAKE) && can_loot (ch, obj)
                    && obj->cost > max && obj->cost > 0)
                {
                    obj_best = obj;
                    max = obj->cost;
                }
            }

            if (obj_best)
            {
                obj_from_room (obj_best);
                obj_to_char (obj_best, ch);
                act ("$n gets $p.", ch, obj_best, NULL, TO_ROOM);
            }
        }

        /* Wander */
        if (!IS_SET (ch->act, ACT_SENTINEL)
            && !IS_SET(ch->act, ACT_MOUNT)
            && number_bits (3) == 0
            && (door = number_bits (5)) <= 5
            && (pexit = ch->in_room->exit[door]) != NULL
            && pexit->u1.to_room != NULL
            && !IS_SET (pexit->exit_info, EX_CLOSED)
            && !IS_SET (pexit->u1.to_room->room_flags, ROOM_NO_MOB)
            && (!IS_SET (ch->act, ACT_STAY_AREA)
                || pexit->u1.to_room->area == ch->in_room->area)
            && (!IS_SET (ch->act, ACT_OUTDOORS)
                || !IS_SET (pexit->u1.to_room->room_flags, ROOM_INDOORS))
            && (!IS_SET (ch->act, ACT_INDOORS)
                || IS_SET (pexit->u1.to_room->room_flags, ROOM_INDOORS)))
        {
            move_char (ch, door, FALSE, FALSE);
        }
    }

    return;
}
//Update the time.
void time_update (void)
{
	
    char buf [MSL];
    DESCRIPTOR_DATA *d;
    buf[0] = '\0';
    time_info.minute += 9;
    if (time_info.minute >= 60)
    {
      time_info.hour++;
      time_info.first = TRUE;
      time_info.minute -= 60;
    }

   
    if (time_info.first)
    {
      time_info.first = FALSE;
      switch (time_info.hour)
      {
        case 5:
            weather_info.sunlight = SUN_LIGHT;
            strcat (buf, "The sun slowly rises in the east.\n\r");
            break;

        case 6:
            weather_info.sunlight = SUN_RISE;
            strcat (buf, "The morning has begun.\n\r");
            break;

        case 12:
            strcat (buf,"The sun shines from above.\n\r");
            break;
        case 19:
            weather_info.sunlight = SUN_SET;
            strcat (buf, "The sun slowly disappears in the west.\n\r");
            break;

        case 20:
            weather_info.sunlight = SUN_DARK;
            strcat (buf, "The night has begun.\n\r");
            break;

        case 24:
            time_info.hour = 0;
            time_info.day++;
            break;
      }
    }

    if (time_info.day >= 35)
    {
        time_info.day = 0;
        time_info.month++;
    }

    if (time_info.month >= 17)
    {
        time_info.month = 0;
        time_info.year++;
    }
    if (buf[0] != '\0')
    {
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            if (d->connected == CON_PLAYING && IS_OUTSIDE (d->character)
                && IS_AWAKE (d->character) && !IS_IMMORTAL(d->character))
                send_to_char (buf, d->character);
        }
    }
}



/*
 * Update the weather.
 */
void weather_update (void)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int diff;

    buf[0] = '\0';

    

    /*
     * Weather change.
     */
    if (time_info.month >= 9 && time_info.month <= 16)
        diff = weather_info.mmhg > 985 ? -2 : 2;
    else
        diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change += diff * dice (1, 4) + dice (2, 6) - dice (2, 6);
    weather_info.change = UMAX (weather_info.change, -12);
    weather_info.change = UMIN (weather_info.change, 12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg = UMAX (weather_info.mmhg, 960);
    weather_info.mmhg = UMIN (weather_info.mmhg, 1040);

    switch (weather_info.sky)
    {
        default:
            bug ("Weather_update: bad sky %d.", weather_info.sky);
            weather_info.sky = SKY_CLOUDLESS;
            break;

        case SKY_CLOUDLESS:
            if (weather_info.mmhg < 990
                || (weather_info.mmhg < 1010 && number_bits (2) == 0))
            {
                strcat (buf, "The sky is getting cloudy.\n\r");
                weather_info.sky = SKY_CLOUDY;
            }
            break;

        case SKY_CLOUDY:
            if (weather_info.mmhg < 970
                || (weather_info.mmhg < 990 && number_bits (2) == 0))
            {
                strcat (buf, "It starts to rain.\n\r");
                weather_info.sky = SKY_RAINING;
            }

            if (weather_info.mmhg > 1030 && number_bits (2) == 0)
            {
                strcat (buf, "The clouds disappear.\n\r");
                weather_info.sky = SKY_CLOUDLESS;
            }
            break;

        case SKY_RAINING:
            if (weather_info.mmhg < 970 && number_bits (2) == 0)
            {
                strcat (buf, "Lightning flashes in the sky.\n\r");
                weather_info.sky = SKY_LIGHTNING;
            }

            if (weather_info.mmhg > 1030
                || (weather_info.mmhg > 1010 && number_bits (2) == 0))
            {
                strcat (buf, "The rain stopped.\n\r");
                weather_info.sky = SKY_CLOUDY;
            }
            break;

        case SKY_LIGHTNING:
            if (weather_info.mmhg > 1010
                || (weather_info.mmhg > 990 && number_bits (2) == 0))
            {
                strcat (buf, "The lightning has stopped.\n\r");
                weather_info.sky = SKY_RAINING;
                break;
            }
            break;
    }

    if (buf[0] != '\0')
    {
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            if (d->connected == CON_PLAYING && IS_OUTSIDE (d->character)
                && IS_AWAKE (d->character) && !IS_IMMORTAL(d->character))
                send_to_char (buf, d->character);
        }
    }

    return;
}

void timer_update(void)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *ch;

  for ( d = descriptor_list; d != NULL; d = d->next )
  {
    if ( d->connected != CON_PLAYING)
      continue;

    ch = d->original ? d->original : d->character;

    if (!IS_NPC(ch))
    {
      if (ch->pcdata->pk_timer > 0)
      {
        ch->pcdata->pk_timer--;
        if (ch->pcdata->pk_timer == 0)
	  ch->pcdata->initiator = 0;
      }
      if ((ch->pcdata->pk_timer == 0) && (ch->pcdata->portcounter > 0))
	{
          send_to_char("Portal counter reset.`*\n\r\n\r", ch);
	  ch->pcdata->portcounter = 0;
	  return;
	}

      if (ch->pcdata->safe_timer > 0)
      {
        ch->pcdata->safe_timer--;
      }
      if (ch->pcdata->wait_timer > 0)
      {
        ch->pcdata->wait_timer--;
      }
      if (ch->timed_affect.seconds > 0)
      {
        ch->timed_affect.seconds--;
	if (ch->timed_affect.seconds == 0)
	{
          AFFECT_DATA af;
          affect_strip(ch, ch->timed_affect.sn);
          af.where     = TO_AFFECTS;
          af.type      = gsn_willpower;
          af.duration  = 1;
    	  af.level     = ch->level;
          af.location  = APPLY_NONE;
          af.bitvector = AFF_WILLPOWER;
          affect_to_char( ch, &af );
        }
      }
    }  
  }
  return;
}

/*
 * Update all chars, including mobs.
*/
void char_update (void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_quit;

    ch_quit = NULL;

    /* update save counter */
    save_number++;

    if (save_number > 29)
        save_number = 0;

    for (ch = char_list; ch != NULL; ch = ch_next)
    {
        AFFECT_DATA *paf;
        AFFECT_DATA *paf_next;

        ch_next = ch->next;
	if ( IS_NPC(ch) )
        {
            mem_fade(ch);
	}

    if (!IS_NPC(ch)) 
    {
      if (ch->pcdata->xpmultiplier.time > -1 && ch->pcdata->xpmultiplier.on)
      {      
        ch->pcdata->xpmultiplier.time--;

        if (ch->pcdata->xpmultiplier.time < 3 && ch->pcdata->xpmultiplier.time > 0)
        {
          char buf[MSL];
          if (ch->pcdata->xpmultiplier.factor > 0)
          {
            sprintf(buf,"`!%d `&tick%s left in bonus time`!!`*\n\r\n\r",
              xp_bonus.time_left + 1, xp_bonus.time_left == 1 ? "" : "s");
          }
          else
          { 
            sprintf(buf,"`!%d `&tick%s left in minus time`!!`*\n\r\n\r",
              xp_bonus.time_left, xp_bonus.time_left == 1 ? "" : "s");
          }
          send_to_char(buf, ch); 
        } 
    
        if (ch->pcdata->xpmultiplier.time < 1)   
        {
          ch->pcdata->xpmultiplier.on = FALSE;
          if (ch->pcdata->xpmultiplier.factor > 0) 
          {
            send_to_char("`@Xp Bonus time has ended :(`*\n\r\n\r", ch);
          }
          else 
          { 
             send_to_char("`@Xp Minus time has ended, a tear rolls down your face :(`*\n\r", ch);
          }
        }
      }    
    }

    if ((ch->penalty.murder > 0) && !IS_SET(ch->comm, COMM_AFK) && (ch->penalty.murder > 1 || ch->pcdata->pk_timer == 0))
        ch->penalty.murder--;
    if ((ch->penalty.thief > 0) && !IS_SET(ch->comm, COMM_AFK) && (ch->penalty.thief > 1 || ch->pcdata->pk_timer == 0))    
        ch->penalty.thief--;
    if ((ch->penalty.jail > 0) && !IS_SET(ch->comm, COMM_AFK))
    {
        if(ch->penalty.jail == 1)
        {
                char_from_room(ch);
                char_to_room(ch, get_room_index(ROOM_VNUM_ALTAR));
                send_to_char("You are set free from your cell.\n\r", ch);
                if (IS_SET(ch->comm, COMM_NOCHANNELS))
                    REMOVE_BIT (ch->comm, COMM_NOCHANNELS);
                ch->penalty.jail--;
        }
        else
                ch->penalty.jail--;
    }

    if (IS_NPC(ch) && (ch->pIndexData->load_time.start != 0 || 
             ch->pIndexData->load_time.end != 0))
    {
      int start = ch->pIndexData->load_time.start;
      int end = ch->pIndexData->load_time.end;

      if (start < end)
      {
        if (time_info.hour >= start && time_info.hour <= end)
           STR_REMOVE_BIT(ch->affected_by, AFF_SUPER_INVIS);
        else if (ch->hit == ch->max_hit && !ch->fighting)
        {
          STR_SET_BIT(ch->affected_by, AFF_SUPER_INVIS);
        }
      }
      else
      {
        if (time_info.hour >= start || time_info.hour <= end)
           STR_REMOVE_BIT(ch->affected_by, AFF_SUPER_INVIS);
        else if (ch->hit == ch->max_hit && !ch->fighting)
        {
          STR_SET_BIT(ch->affected_by, AFF_SUPER_INVIS);
        }
      }
    }

    if (!IS_NPC(ch) && ch->pcdata->safe_timer == 0 &&
          ch->in_room && ch->in_room->vnum == ROOM_VNUM_MORGUE)
    {
       ROOM_INDEX_DATA *room;
       if ((room = get_room_index (ROOM_VNUM_ALTAR)) != NULL)
       {
          char_from_room(ch);
          char_to_room(ch, room);  
       }
    }



        if (ch->timer > 5 && !ch->desc)
        {
            if (IS_SET(ch->act, PLR_QUESTOR))
            {
              REMOVE_BIT(ch->act, PLR_QUESTOR);
              ch->questgiver = NULL;
              ch->countdown = 0;
              ch->questmob = 0;
              ch->questobj = 0;
              ch->questriddle = 0;
              ch->nextquest = 15;
            }
            if (IS_SET(ch->act, PLR_TOURNEY) || IS_SET(ch->act2, PLR_TOURNAMENT_START))      
            {
              if (IS_SET(ch->act, PLR_TOURNEY))
                REMOVE_BIT(ch->act, PLR_TOURNEY);
              if (IS_SET(ch->act2, PLR_TOURNAMENT_START))
                REMOVE_BIT(ch->act2, PLR_TOURNAMENT_START);
            }         
        }

        if (ch->timer > 25 && !ch->desc)
            ch_quit = ch;

	if (ch->interdimension > 0 && --(ch->interdimension) == 0)
	{
	    act("$n pops back in to this dimension.", ch,0,0,TO_ROOM);
	    stc("You pop back in to the normal dimension.\n\r", ch);
	}

        if (ch->position >= POS_STUNNED)
        {
            /* check to see if we need to go home */
            if (IS_NPC (ch) && ch->zone != NULL
                && ch->zone != ch->in_room->area && ch->desc == NULL
                && ch->fighting == NULL && !IS_AFFECTED (ch, AFF_CHARM)
                && number_percent () < 5)
            {
                act ("$n wanders on home.", ch, NULL, NULL, TO_ROOM);
                extract_char (ch, TRUE);
                continue;
            }

            if (ch->hit < ch->max_hit)
                ch->hit += hit_gain (ch);
            else
                ch->hit = ch->max_hit;

            if (ch->mana < ch->max_mana)
                ch->mana += mana_gain (ch);
            else
                ch->mana = ch->max_mana;

            if (ch->move < ch->max_move)
                ch->move += move_gain (ch);
            else
                ch->move = ch->max_move;
        }

        if (ch->position == POS_STUNNED)
            update_pos (ch);

        if (!IS_NPC (ch))
        {
            OBJ_DATA *obj;

            if ((obj = get_eq_char (ch, WEAR_LIGHT)) != NULL
                && obj->item_type == ITEM_LIGHT && obj->value[2] > 0)
            {
                if (--obj->value[2] == 0 && ch->in_room != NULL && !IS_IMMORTAL(ch))
                {
                    --ch->in_room->light;
                    act ("$p goes out.", ch, obj, NULL, TO_ROOM);
                    act ("$p flickers and goes out.", ch, obj, NULL, TO_CHAR);
                    extract_obj (obj);
                }
                else if (obj->value[2] <= 5 && ch->in_room != NULL)
                    act ("$p flickers.", ch, obj, NULL, TO_CHAR);
            }

	    ch->timer++;
            if (ch->timer >= 35 && !IS_IMMORTAL(ch))
            {
                if (ch->was_in_room == NULL && ch->in_room != NULL)
                {
                    ch->was_in_room = ch->in_room;
                    if (ch->fighting != NULL)
                        stop_fighting (ch, TRUE);
                    act ("$n rejoins the Wheel.",
                         ch, NULL, NULL, TO_ROOM);
		    send_to_char("You rejoin the Wheel in the void.\n\r", ch);
                    if (ch->level > 1)
                        save_char_obj (ch);
                    char_from_room (ch);
                    char_to_room (ch, get_room_index (ROOM_VNUM_LIMBO));
//		    do_function (ch, &do_quit, "");

				}
            }
			else if (ch->timer >= 15 && !IS_IMMORTAL(ch))
			{
				if (!IS_SET(ch->comm,COMM_AFK))
                                {
				  SET_BIT (ch->comm, COMM_AFK);         
			          send_to_char("You are now in AFK mode.\n\r", ch);
                                }
       			}

            if (!IS_NPC(ch) && ch->timer > 99)
            {
		ch->timer = 99;
            }

            ch->pcdata->condition[COND_GOBLIND] = 0;
        }

        for (paf = ch->affected; paf != NULL; paf = paf_next)
        {
            paf_next = paf->next;
            if (paf->duration > 0)
            {
                paf->duration--;
                if (number_range (0, 4) == 0 && paf->level > 0)
                    paf->level--;    /* spell strength fades with time */
            }
            else if (paf->duration < 0);
            else
            {

                if (paf->type == skill_lookup("veil") &&
		   ch->fighting != NULL)
                {
		  break;
                }

                if (paf->type == skill_lookup("stance") &&
		   ch->fighting != NULL)
                {
		  break;
                }

                if (paf_next == NULL
                    || paf_next->type != paf->type || paf_next->duration > 0)
                {
                    if (paf->type > 0 && skill_table[paf->type].msg_off)
                    {
                        send_to_char (skill_table[paf->type].msg_off, ch);
                        send_to_char ("\n\r", ch);
                    }
                }
                if (IS_DISGUISED(ch) && paf->bitvector == AFF_DISGUISE)
                {
                   ch->name = ch->pcdata->disguise.orig_name;
                   ch->pcdata->title = ch->pcdata->disguise.orig_title;
                   ch->description = ch->pcdata->disguise.orig_desc;
                   ch->short_descr = ch->pcdata->disguise.orig_short;
                   ch->long_descr = ch->pcdata->disguise.orig_long;
                   STR_REMOVE_BIT(ch->affected_by, paf->bitvector); 
                   affect_remove (ch, paf);
    
                   act ("$n removes $s disguise.\n\r",ch,NULL,NULL,TO_ROOM);
                   send_to_char ("You remove your disguise.\n\r",ch);
                   continue;
                }
                STR_REMOVE_BIT(ch->affected_by, paf->bitvector); 
                affect_remove (ch, paf);
            }
        }

     

        /*
         * Careful with the damages here,
         *   MUST NOT refer to ch after damage taken,
         *   as it may be lethal damage (on NPC).
         */

      if(IS_AFFECTED(ch, AFF_SEVERED))
      {
		  STR_REMOVE_BIT(ch->affected_by, AFF_SEVERED);
          act("With a last gasp of breath, $n dies due to massive lower body trauma.",
                ch,NULL,NULL,TO_ROOM);
          if(!IS_NPC(ch))
          {
			  send_to_char("Your injuries prove too much, and you die.\n\r",ch);
			  raw_kill(ch, ch);
			  return;
		  }
		  else
		  {
			  raw_kill(ch,ch);
			  return;
		  }
	  }
        bleed_char(ch,TRUE);
        if (is_affected (ch, gsn_plague) && ch != NULL)
        {
            AFFECT_DATA *af, plague;
            CHAR_DATA *vch;
            int dam;

            if (ch->in_room == NULL)
                continue;

            act ("$n writhes in agony as plague sores erupt from $s skin.",
                 ch, NULL, NULL, TO_ROOM);
            send_to_char ("You writhe in agony from the plague.\n\r", ch);
            for (af = ch->affected; af != NULL; af = af->next)
            {
                if (af->type == gsn_plague)
                    break;
            }

            if (af == NULL)
            {
                STR_REMOVE_BIT (ch->affected_by, AFF_PLAGUE);
                continue;
            }

            if (af->level == 1)
                continue;

            plague.where = TO_AFFECTS;
            plague.type = gsn_plague;
            plague.level = af->level - 1;
            plague.duration = number_range (1, 2 * plague.level);
            plague.location = APPLY_STR;
            plague.modifier = -5;
            plague.bitvector = AFF_PLAGUE;

            for (vch = ch->in_room->people; vch != NULL;
                 vch = vch->next_in_room)
            {
                if (!saves_spell (plague.level - 2, vch, DAM_DISEASE)
                    && !IS_IMMORTAL (vch)
                    && !IS_AFFECTED (vch, AFF_PLAGUE) && number_bits (4) == 0)
                {
                    send_to_char ("You feel hot and feverish.\n\r", vch);
                    act ("$n shivers and looks very ill.", vch, NULL, NULL,
                         TO_ROOM);
                    affect_join (vch, &plague);
                }
            }

            dam = UMIN (ch->level, af->level / 5 + 1);
            ch->mana -= dam;
            ch->move -= dam;
            
            damage (ch, ch, dam, gsn_plague, DAM_DISEASE, FALSE, FALSE);
            
        }
        else if (IS_AFFECTED (ch, AFF_POISON) && ch != NULL
                 && !IS_AFFECTED (ch, AFF_SLOW))
        {
            AFFECT_DATA *poison;

            poison = affect_find (ch->affected, gsn_poison);

            if (poison != NULL)
            {
                act ("$n shivers and suffers.", ch, NULL, NULL, TO_ROOM);
                send_to_char ("You shiver and suffer.\n\r", ch);
                  damage (ch, ch, poison->level / 10 + 1, gsn_poison,DAM_POISON, FALSE, FALSE);
                
            }
        }
        else if (IS_AFFECTED (ch, AFF_RAVAGE) && ch != NULL
                 && !IS_AFFECTED (ch, AFF_SLOW))
        {

            AFFECT_DATA *ravage;
            ravage = affect_find (ch->affected, gsn_ravage);

            if (ravage != NULL)
            {
                act ("$n is hit by ravage.", ch, NULL, NULL, TO_ROOM);
                send_to_char ("You feel the whiplash of the Seanchan.\n\r", ch);
                ch->mana -= ravage->level / 3 + 15;
                ch->move -= ravage->level / 3 + 15;
                
                damage (ch, ch, ravage->level / 2 + 15, gsn_ravage,
                        DAM_POISON, FALSE, FALSE);
                
            }
        }
else if (IS_AFFECTED (ch, AFF_CONTACT) && ch != NULL
                 && !IS_AFFECTED (ch, AFF_SLOW))
        {

            AFFECT_DATA *contact;
            contact = affect_find (ch->affected, gsn_contact);

            if (contact != NULL)
            {
                act ("$n screams from the pain of poison.", ch, NULL, NULL, TO_ROOM);
                send_to_char ("You poison burn through your veins.\n\r", ch);
                ch->mana -= ch->level / 2 + 5;
                ch->move -= ch->level / 2 + 15;
                
                damage (ch, ch, ch->level / 2 + 5, gsn_contact, DAM_POISON, FALSE, FALSE);
                
            }
        }
        else if (ch->position == POS_INCAP && number_range (0, 1) == 0)
        {
			if (!IS_NPC(ch) 
				&&  ch->pcdata->condition[COND_BLEEDING] > 0)  
			{
				SET_BLEED(ch,FALSE);
			}
	
	   	
            damage (ch, ch, 2, TYPE_UNDEFINED, DAM_NONE, FALSE, FALSE);

        } 
        else if (ch->position == POS_MORTAL)
        {
            if (!IS_NPC(ch) 
		&&  ch->pcdata->condition[COND_BLEEDING] > 0)  
			{
				SET_BLEED(ch,FALSE);
			}
            damage (ch, ch, 2, TYPE_UNDEFINED, DAM_NONE, FALSE, FALSE);
	
        }
    
        if (!IS_NPC(ch) && ch->pcdata->dream)
        {  
          if (ch->position == POS_SLEEPING)
          {
	    dream_to(ch);
          }  
          else
          {
            ch->pcdata->dream = FALSE;
            ch->pcdata->dream_to_room = ch->in_room;
          }
        }
		
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    for (ch = char_list; ch != NULL; ch = ch_next)
    {
        ch_next = ch->next;

        if (ch->desc != NULL && ch->desc->descriptor % 30 == save_number)
        {
            save_char_obj (ch);
        }

        if (ch == ch_quit)
        {
            do_function (ch, &do_quit, "");
        }
    }
	
    return;
}




/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update (void)
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf, *paf_next;

    for (obj = object_list; obj != NULL; obj = obj_next)
    {
        CHAR_DATA *rch;
        char *message;

        obj_next = obj->next;

        /* go through affects and decrement */
        for (paf = obj->affected; paf != NULL; paf = paf_next)
        {

            paf_next = paf->next;
            if (obj->wear_loc <= 0)
              continue;

            if (paf->duration > 0)
            {
                paf->duration--;
                if (number_range (0, 4) == 0 && paf->level > 0)
                    paf->level--;    /* spell strength fades with time */
            }
            else if (paf->duration < 0);
            else
            {
                if (paf_next == NULL
                    || paf_next->type != paf->type || paf_next->duration > 0)
                {
                    if (paf->type > 0 && skill_table[paf->type].msg_obj)
                    {
                        if (obj->carried_by != NULL)
                        {
                            rch = obj->carried_by;
                            act (skill_table[paf->type].msg_obj,
                                 rch, obj, NULL, TO_CHAR);
                        }
                        if (obj->in_room != NULL
                            && obj->in_room->people != NULL)
                        {
                            rch = obj->in_room->people;
                            act (skill_table[paf->type].msg_obj,
                                 rch, obj, NULL, TO_ALL);
                        }
                    }
                }

                affect_remove_obj (obj, paf);
            }
        }

        /*                              
         * Oprog triggers!
         */
        if ( obj->in_room || (obj->carried_by && obj->carried_by->in_room))
        {
            if ( HAS_TRIGGER_OBJ( obj, TRIG_DELAY )
              && obj->oprog_delay > 0 )
            {
                if ( --obj->oprog_delay <= 0 )
                    p_percent_trigger( NULL, obj, NULL, NULL, NULL, NULL, TRIG_DELAY );
            }
            else if ( ((obj->in_room && !obj->in_room->area->empty)
                || obj->carried_by ) && HAS_TRIGGER_OBJ( obj, TRIG_RANDOM ) )
                    p_percent_trigger( NULL, obj, NULL, NULL, NULL, NULL, TRIG_RANDOM );
         }
        /* Make sure the object is still there before proceeding */
        if ( !obj )
            continue;

        if (obj->timer <= 0 || --obj->timer > 0)
            continue;
        if (!str_cmp(obj->timer_msg,"none"))
        {
        switch (obj->item_type)
        {
            default:
                message = "$p crumbles into dust.";
                break;
            case ITEM_FOUNTAIN:
                message = "$p dries up.";
                break;
            case ITEM_CORPSE_NPC:
                message = "$p decays into dust.";
                break;
            case ITEM_CORPSE_PC:
                message = "$p decays into dust.";
                break;
            case ITEM_FOOD:
                message = "$p decomposes.";
                break;
            case ITEM_POTION:
                message = "$p has evaporated from disuse.";
                break;
            case ITEM_PORTAL:
                message = "$p fades out of existence.";
                break;
            case ITEM_FIRE:
                message = "$p burns out and turns to ash.";
                break;
	    case ITEM_BLOOD:
		message = "$p dries up and disappears.";
	        break;
            case ITEM_CONTAINER:
                if (CAN_WEAR (obj, ITEM_WEAR_FLOAT))
                    if (obj->contains)
                        message =
                            "$p flickers and vanishes, spilling its contents on the floor.";
                    else
                        message = "$p flickers and vanishes.";
                else
                    message = "$p crumbles into dust.";
                break;
        }
        
        }

        else
            message = str_dup(obj->timer_msg);
        if (IS_OBJ_STAT(obj, ITEM_QUEST) && !IS_NULLSTR(obj->owner))
        {
          extract_obj(obj);
          continue;
        }

        if (obj->carried_by != NULL)
        {
            if (IS_NPC (obj->carried_by)
                && obj->carried_by->pIndexData->pShop != NULL)
                obj->carried_by->silver += obj->cost / 5;
            else
            {
                act (message, obj->carried_by, obj, NULL, TO_CHAR);
                if (obj->wear_loc == WEAR_FLOAT)
                    act (message, obj->carried_by, obj, NULL, TO_ROOM);
            }
        }
        else if (obj->in_room != NULL && (rch = obj->in_room->people) != NULL)
        {
            if (!(obj->in_obj && obj->in_obj->pIndexData->vnum == OBJ_VNUM_PIT
                  && !CAN_WEAR (obj->in_obj, ITEM_TAKE)))
            {
                act (message, rch, obj, NULL, TO_ROOM);
                act (message, rch, obj, NULL, TO_CHAR);
            }
        }

        if ((obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT)
            && obj->contains)
        {                        /* save the contents */
            OBJ_DATA *t_obj, *next_obj;

            for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
            {
                next_obj = t_obj->next_content;
                obj_from_obj (t_obj);

                if (obj->in_obj)    /* in another object */
                    obj_to_obj (t_obj, obj->in_obj);

                else if (obj->carried_by)    /* carried */
                    if (obj->wear_loc == WEAR_FLOAT)
                        if (obj->carried_by->in_room == NULL)
                            extract_obj (t_obj);
                        else
                            obj_to_room (t_obj, obj->carried_by->in_room);
                    else
                        obj_to_char (t_obj, obj->carried_by);

                else if (obj->in_room == NULL)    /* destroy it */
                    extract_obj (t_obj);

                else            /* to a room */
                    obj_to_room (t_obj, obj->in_room);
            }
        }

        extract_obj (obj);
    }

    return;
}

void firestorm_update (void)
{
   CHAR_DATA *ch;
   CHAR_DATA *ch_next;
        
   for (ch = char_list; ch != NULL; ch = ch_next)
   { 
     ch_next = ch->next;  
     if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_FIRESTORM))
     {
          CHAR_DATA *vch, *vch_next;
          int dam;
          dam = number_range(int(ch->level * 1.95), int(ch->level * 2.9 + (number_percent() / 5)));
          if (ch->mana < 20)
          {
            affect_strip(ch, gsn_firestorm);
            send_to_char("Your mind relaxes.\n\r", ch);
            continue;
          }
          
          ch->mana -= 10;

          for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
          {
          vch_next = vch->next_in_room;
    
          if (is_safe_spell (ch, vch, TRUE)
          || (vch == ch)
          || (IS_NPC(vch))
          || furies_imm(ch,vch))
             continue; 
          damage(ch, vch, dam, 1041, DAM_FIRE, TRUE, FALSE);
          }
     }
   }
   return;
}

void bonus_update()
{
  DESCRIPTOR_DATA *d;
  char buf[MSL];

  if (xp_bonus.on && (xp_bonus.time_left != -1)) 
  {

    if (xp_bonus.time_left > 0)
    {
      xp_bonus.time_left--;
      for (d = descriptor_list ; d != NULL ; d = d->next)
      {
         if (d->connected == CON_PLAYING)
         {
            if (IS_SET(d->character->act2, PLR_MSP_MUSIC) && !xp_bonus.negative)             
    	      send_to_char("!!MUSIC(Sounds/BonusS.mid V=80 L=-1 C=1 T=BonusTime)\n\r", d->character);
         } 
      }            


      if (xp_bonus.time_left < 3)
      {
         if (!xp_bonus.negative)
         { 
           sprintf(buf,"`!%d `&tick%s left in bonus time`!!`*",
              xp_bonus.time_left + 1, xp_bonus.time_left == 1 ? "" : "s");
         }
         else
         { 
           sprintf(buf,"`!%d `&tick%s left in minus time`!!`*",
              xp_bonus.time_left, xp_bonus.time_left == 1 ? "" : "s");
         }


         for (d = descriptor_list ; d != NULL ; d = d->next)
         {
           if (d->connected == CON_PLAYING)
           {
             send_to_char(buf, d->character); 
             if (IS_SET(d->character->act2, PLR_MSP_MUSIC) && !xp_bonus.negative)            
   		send_to_char("\n\r!!MUSIC(Sounds/BonusS.mid V=80 L=-1 C=1 T=BonusTime)\n\r", d->character);
           } 
         }            
      }
    }
    else
    {
      xp_bonus.on = FALSE;
      for (d = descriptor_list ; d != NULL ; d= d->next)
      {
         if (d->connected == CON_PLAYING)
         {
            if (!xp_bonus.negative) 
            {
              send_to_char("`@Xp Bonus time has ended :(`*\n\r\n\r", d->character);
              if (IS_SET(d->character->act2, PLR_MSP_MUSIC))
              {
                send_to_char("!!MUSIC(Sounds/BonusE.mid V=80 L=1 C=1 T=BonusTime)\n\r", d->character);
              }
	    }
            else  
              send_to_char("`@Xp Minus time has ended, a tear rolls down your face :(`*\n\r", d->character);
         }
      }    
    }        
 }
 return; 
}



/* Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update (void)
{
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim;

    for (wch = char_list; wch != NULL; wch = wch_next)
    {
        wch_next = wch->next;
		
        if (IS_NPC (wch)
            || wch->level >= LEVEL_IMMORTAL
            || wch->in_room == NULL || wch->in_room->area->empty) continue;

        for (ch = wch->in_room->people; ch != NULL; ch = ch_next)
        {
            int count;

            ch_next = ch->next_in_room;
            
            if (!IS_NPC (ch)
                || !IS_SET (ch->act, ACT_AGGRESSIVE)
                || IS_AFFECTED(ch, AFF_SUPER_INVIS)
                || IS_SET (ch->in_room->room_flags, ROOM_SAFE)
                || IS_AFFECTED (ch, AFF_CALM)
                || ch->fighting != NULL || IS_AFFECTED (ch, AFF_CHARM)
                || !IS_AWAKE (ch)
                || (IS_SET (ch->act, ACT_WIMPY) && IS_AWAKE (wch))
                || !can_see (ch, wch) || number_bits (1) == 0)
            {
              if ( IS_NPC(ch) && can_see(ch, wch) )
              {
                MEM_DATA *remember;
                if ( (remember = get_mem_data(ch, wch)) != NULL)
                {
                    if ( IS_SET(remember->reaction, MEM_AFRAID)
                      && IS_SET(ch->act, ACT_WIMPY)
                      && ch->wait < PULSE_VIOLENCE / 2
                      && number_bits(2) == 1)
                        do_flee(ch, "self");
                    if ( IS_SET(remember->reaction, MEM_HOSTILE)   
                        && ch->fighting == NULL )
                        remember_victim(ch, wch);
                }
              }
            continue;
	    }
            /*
             * Ok we have a 'wch' player character and a 'ch' npc aggressor.
             * Now make the aggressor fight a RANDOM pc victim in the room,
             *   giving each 'vch' an equal chance of selection.
             */
            count = 0;
            victim = NULL;
            for (vch = wch->in_room->people; vch != NULL; vch = vch_next)
            {
                vch_next = vch->next_in_room;

                if (!IS_NPC (vch)
                    && !IS_SET(vch->act, PLR_TOURNEY) 
                    && vch->level < LEVEL_IMMORTAL
                    && ch->level >= vch->level - 5
                    && (!IS_SET (ch->act, ACT_WIMPY) || !IS_AWAKE (vch))
                    && can_see (ch, vch))
                {
                    if (number_range (0, count) == 0)
                        victim = vch;
                    count++;
                }
            }

            if (victim == NULL)
                continue;

            multi_hit (ch, victim, TYPE_UNDEFINED);
        }
    }

    return;
}

void delay_update (void)
{
  DELAY_CODE *list, *list_next;
  
  if (!delay_list)
    return;

  if (current_time < delay_list->time)
    return;


  for (list = delay_list;list;list = list_next)
  {
    list_next = list->next;
    if (current_time < list->time)
      return;
    
    delay_list = list->next;
    if (!IS_VALID(list->obj) && !list->room && !IS_VALID(list->mob))
    {
      free_delay(list);
    }
    else
    {
      program_flow(list->vnum, list->code, list->mob, list->obj, list->room, list->ch, list->arg1, list->arg2, list->line);
      free_delay(list);
    }
  }
}


/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */

void update_handler (void)
{
    static long pulse_system;
    static int pulse_area;
    static int pulse_mobile;
    static int pulse_violence;
    static int pulse_point;
    static int pulse_music;
    static int pulse_auction;
    static int pulse_dtrap;
    static int pulse_save;
    static int pulse_mobhunt;
    static int pulse_second;

    if (forceTick)
    {
      pulse_area = 0;
      pulse_mobile = 0;
      pulse_violence = 0;
      pulse_point = 0;
      pulse_music = 0;
      pulse_auction = 0;
      pulse_dtrap = 0;
      pulse_mobhunt = 0;
      pulse_second = 0;
      forceTick = FALSE; 
    }

    if ( --pulse_second <=0 )
    {
        pulse_second    = PULSE_PER_SECOND;
        aggro_update       ( );
        timer_update       ( );
        delay_update       ( );
    }

    if (--pulse_save <= 0)
    {
        pulse_save = PULSE_SAVE;
        save_update ();
	}

    if (--pulse_area <= 0)
    {
        pulse_area = PULSE_AREA;
        /* number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 ); */
        area_update ();
	}

    if ( --pulse_auction        <= 0 )
    {
        pulse_auction   = PULSE_AUCTION;
        auction_update( );
    }

    if (--pulse_music <= 0)
    {
        pulse_music = PULSE_MUSIC;
        song_update ();
    }

    if (--pulse_mobile <= 0)
    {
        pulse_mobile = PULSE_MOBILE;
        mobile_update ();
    }

    if (pulse_violence == 1)
    {
      round_print();
    }

    if (--pulse_violence <= 0)
    {
        pulse_violence = PULSE_VIOLENCE;
        firestorm_update ();    
        violence_update ();
    }

    if (--pulse_point <= 0)
    {
        wiznet ("TICK!", NULL, NULL, WIZ_TICKS, 0, 0);
        pulse_point = number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 );
        char_update ();
        obj_update ();
        copyover_update();
        warticks_update();
        bonus_update( );
     	quest_update( );
     	weather_update ();
     	time_update();
      //  war_update( ); /* kyt -- war system */
        tournament_update();
    }

    if (--pulse_system <= 0)
    {
      pulse_system = PULSE_SYSTEM;
      system_check();
    }

	if ( --pulse_dtrap    <= 0)
    {
        pulse_dtrap     = PULSE_DEATHTRAP;
        dtrap_update     ( );
        check_all_games();
    }

	if (--pulse_mobhunt <= 0)
	{
		pulse_mobhunt = PULSE_MOBHUNT;
		mobhunt_update ( );
		dirtkick_update ();
	}
    counter_update ();	
    aggr_update ();
    tail_chain ();
    //auction_update ();
    return;
}
void mobhunt_update (void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;

    for (ch = char_list; ch != NULL; ch = ch_next)
    {
        ch_next = ch->next;
		
        if ( IS_NPC(ch)
            && ch->fighting == NULL
            && IS_AWAKE(ch)
            && ch->hunting != NULL
            && !IS_SET(ch->act,ACT_NOHUNT) &&
               !ch->mount)
		{
		    hunt_victim(ch);
		    continue;
		}
        		
                
        if ((victim = ch->fighting) == NULL)
            continue;

		if ( IS_NPC(ch)
			&& ch->fighting != NULL
			&& IS_AWAKE(ch)
			&& ch->hunting == NULL
			&& can_see(ch,victim)
			&& !IS_SET(ch->act,ACT_NOHUNT))
		{
			ch->hunting = victim;
			continue;
		}
		
    }

    return;
}
void counter_update (void)
{
    CHAR_DATA *ch;
    int chance;
    ch = char_list;
    ch = get_char_world_special("Creator");
    if (!ch || !ch->in_room)
      return;

    if (armorcounter < 1)
    {
    	do_spellup(ch,"all armor");
    	armorcounter = 250;
    }
    if (shieldcounter < 1)
    {
    	do_spellup(ch,"all shield");
    	shieldcounter = 750;
    }
    if (blesscounter < 1)
    {
    	do_spellup(ch,"all bless");
    	blesscounter = 1000;
    }
    if (sanccounter <1)
    {
    	do_spellup(ch,"all sanctuary");
    	sanccounter = 1500;
    }
    if (restorecounter < 1)
    {
    	do_restore(ch,"all");
    	restorecounter = 2000;
    }

    if (bonuscounter < 1)
    {
    	chance = number_percent ();
    	if (chance <= 2)
    	    do_bonus_time(ch,"4 15");
    	else if (chance > 2 && chance <= 8)
    	    do_bonus_time(ch,"3 15");
    	else
    	    do_bonus_time(ch,"2 15");    	    	
    	bonuscounter = 2500;
    }

    if (questcounter < 1)
    {
        initialize_list();
    	questcounter = 750;
        echo("`&Quest List Reset`*");        
    }
      
}

void save_update(void)
{
  save_guilds();
  save_vote();
  save_mudstats();
  save_gameinfo();
  save_equipid();
  save_stores();
  web_mudstats();
  check_mudstats();
}

void dtrap_update( void )
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *ch;

   for ( d = descriptor_list; d != NULL; d = d->next)

   {
      
       ch = d->original ? d->original : d->character;

       if (d->connected == CON_PLAYING && !IS_NPC(ch)
       && !IS_IMMORTAL(ch)
       && IS_SET(ch->in_room->room_flags, ROOM_DEATHTRAP) )
       {

          do_look( ch, "dt" );
	  if (ch->pcdata->pk_timer > 0)
	    continue;

          if (ch->hit > 20)
          {
	        if (ch->position ==  POS_STANDING)	
                  ch->position = POS_RESTING;
                ch->hit = int(ch->hit * .90);
                send_to_char("You better get out of here fast!!!!\n\r", ch);
          }
          else
          {
             ch->hit = 1;
             raw_kill(ch,ch);
             send_to_char("You are dead!!!!", ch);
          }
      }
    }


}

/*
* RP EXP STUFF
* Basically modified advance and gain code/
*Does advance at normal but gains divided by 4 but can be changed -CAMM*/
void rpadvance_level (CHAR_DATA * ch,bool advance, bool hide)
{
    char buf[MAX_STRING_LENGTH];
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;
    CHAR_DATA *creator;

    creator = get_char_world(ch, "Creator");

    add_hp =
        con_app[get_curr_stat (ch, STAT_CON)].hitp +
        number_range (class_table[ch->cClass].hp_min,
                      class_table[ch->cClass].hp_max);
    add_mana    = (get_curr_stat(ch, STAT_INT) / 5) + number_range(
                    class_table[ch->cClass].mana_min,
                    class_table[ch->cClass].mana_max);                   
    //if (!class_table[ch->cClass].fMana)
    //    add_mana /= 2;
    add_move = number_range (1, (get_curr_stat (ch, STAT_CON)
                                 + get_curr_stat (ch, STAT_DEX)) / 6);
    add_prac = wis_app[get_curr_stat (ch, STAT_WIS)].practice;

    add_hp = add_hp * 9 / 10;
    add_mana = add_mana * 9 / 10;
    add_move = add_move * 9 / 10;

	/*final gains for rpadvance*/
	add_prac = number_range(1,10);
	add_hp = add_hp/5;
	add_mana = add_mana/5;
	add_move = add_move/4;

    add_hp = UMAX (2, add_hp);
    add_mana = UMAX (2, add_mana);
    add_move = UMAX (6, add_move);
	if (advance)
	{
		ch->rplevel += 1;
                if (ch->clan > 0)
                  member_rp(ch->name, ch->clan, 1);           
		ch->max_hit += add_hp;
		ch->max_mana += add_mana;
		ch->max_move += add_move;
		ch->practice += add_prac;
		ch->gold += 1000;
                ch->rp_points += 1; 
		ch->pcdata->perm_hit += add_hp;
		ch->pcdata->perm_mana += add_mana;
		ch->pcdata->perm_move += add_move;
	}
	else
	{
		ch->rplevel -= 1;
                if (ch->clan > 0)
                  member_rp(ch->name, ch->clan, -1);           
		ch->max_hit -= add_hp;
		ch->max_mana -= add_mana;
		ch->max_move -= add_move;
		ch->practice -= add_prac;
 
		ch->pcdata->perm_hit -= add_hp;
		ch->pcdata->perm_mana -= add_mana;
		ch->pcdata->perm_move -= add_move;
	}
    if (!hide)
    {
        sprintf (buf,
                 "You %s %d hit point%s, %d mana, %d move, %d practice%s and 1 rp point.\n\r",
                 advance == TRUE ? "gain" : "lose",add_hp, add_hp == 1 ? "" : "s", add_mana, add_move,
                 add_prac, add_prac == 1 ? "" : "s");
        send_to_char (buf, ch);
    }
    if(ch->level <= 70)
    {
        advance_level (ch,TRUE,FALSE);
        advance_level (ch,TRUE,FALSE);
        advance_level (ch,TRUE,FALSE);
        advance_level (ch,TRUE,FALSE);
        advance_level (ch,TRUE,FALSE);
    }
    else if (ch->level == 71)
    {
        advance_level (ch,TRUE,FALSE);
        advance_level (ch,TRUE,FALSE);
        advance_level (ch,TRUE,FALSE);
        advance_level (ch,TRUE,FALSE);
    }
    else if (ch->level == 72)
    {
        advance_level (ch,TRUE,FALSE);
        advance_level (ch,TRUE,FALSE);
        advance_level (ch,TRUE,FALSE);
    }
    else if (ch->level == 73)
    {
        advance_level (ch,TRUE,FALSE);
        advance_level (ch,TRUE,FALSE);
    }
    else if (ch->level == 74)
    {
        advance_level (ch,TRUE,FALSE);
    }
    return;
}
void rpgain_exp (CHAR_DATA * ch, int gain)
{
    char buf[MAX_STRING_LENGTH];
	int rp_xp_per_lvl = 1000 + (ch->rplevel*100);/*If you change you must change in act_info.c -CAMM*/

    if (IS_NPC (ch) || ch->level >= MAX_RPLEVEL)
        return;

    if (xp_bonus.on)
    {
      if (!xp_bonus.negative)
      {
         gain = gain * (int)(xp_bonus.multiplier*0.70);
      }
      else
      {
         gain = (int)(gain*1.5);
      }  
    }

    ch->rpexp = ch->rpexp + gain;
    while (ch->rplevel < MAX_RPLEVEL && ch->rpexp >=
           rp_xp_per_lvl)
    {
        send_to_char ("You raise in role-playing level!!\n\r", ch);
        ch->rpexp -= rp_xp_per_lvl;
        sprintf (buf, "%s gained role-playing level %d\n\r", ch->name, ch->rplevel);
        log_string (buf);
        sprintf (buf, "$N has attained role-playing level %d!\n\r", ch->rplevel+1);
        wiznet (buf, ch, NULL, WIZ_LEVELS, 0, 0);
        rpadvance_level (ch,TRUE, FALSE);
        save_char_obj (ch);
    }

    return;
}

void SET_BLEED (CHAR_DATA *ch, bool bleed)
{
    if (IS_NPC(ch) || IS_IMMORTAL(ch) || ch->level < 11)
        return;
    if (bleed)
    {
        if (ch->pcdata->condition[COND_BLEEDING] == 0)
        {        
            ch->pcdata->condition[COND_BLEEDING]=1;
        }
    }
    if (!bleed)
    {
    	if (ch->pcdata->condition[COND_BLEEDING] == 1)
    	{
    	    ch->pcdata->condition[COND_BLEEDING]=0;
    	}    	
    }
    return;
}

void bleed_char (CHAR_DATA *ch, bool update)
{
    int dam;
    OBJ_DATA *blood;

    if (IS_NPC(ch))
        return;
    if (IS_IMMORTAL(ch) || ch->level < 11)
    {
    	ch->pcdata->condition[COND_BLEEDING] = 0;
    	return;
    }    
    if (update)
    {
        if (ch->pcdata->condition[COND_BLEEDING] == 1 )
        {      
            blood = create_object (get_obj_index (OBJ_VNUM_BLOOD), 0);
            blood->timer = 3;
            obj_to_room (blood, ch->in_room);
            send_to_char("You sure are `!b`1leeding`*!\n\r",ch);
            act ("$n bleeds profusively.",ch,NULL,NULL, TO_ROOM);
     //       dam = number_range(1,5);
            
	//    damage (ch,ch,dam,TYPE_UNDEFINED,DAM_NONE,FALSE, FALSE);
	    ch->hit  -= number_range(1,5);
            ch->move -= number_range(1,5);
        }
        if (ch->pcdata->condition[COND_BLEEDING] == 1  
	    && number_percent( ) > 90 )
        {
            SET_BLEED(ch,FALSE);
	    act ("$n's bleeding slows.",ch,NULL,NULL, TO_ROOM);
            send_to_char("Your `!b`1leeding`* slows.\n\r",ch);      
        }
    }
    else
    {
    	if ( ch->pcdata->condition[COND_BLEEDING] == 1 )
        {
            dam = number_range(1,2);
            blood = create_object (get_obj_index (OBJ_VNUM_BLOOD), 0);
	    blood->timer = 3;
	    obj_to_room (blood, ch->in_room);
            act ("$n `!b`1leeds`* profusively!", ch, NULL, NULL, TO_ROOM);
	    act ("You `!b`1leed`* profusively!", ch, NULL, NULL,TO_CHAR);
            ch->hit  -= number_range(1,5);
            ch->move -= number_range(1,5);

	   // damage(ch,ch,dam,TYPE_UNDEFINED,DAM_NONE,FALSE, FALSE);
	   
        }
    }

    return;
}

void dirtkick_update (void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    AFFECT_DATA *af;
    int chance = 80;
    int sn = skill_lookup("dirt kicking");

    for (ch = char_list; ch != NULL; ch = ch_next)
    {
        ch_next = ch->next;


        
        if (IS_NPC (ch) || ch->fighting != NULL || !is_affected(ch,sn) || (ch->in_room == NULL))
            continue;
        

    if (IS_AWAKE (ch)&& is_affected(ch,sn))
       
    {
    	if (number_percent()>chance )
        {
        for (af = ch->affected; af != NULL; af = af->next)
        {
            if (af->type == sn)
            {
                
                    affect_strip (ch, sn);
                    
                    send_to_char ("Your tears and rubbing has cleansed the dirt from your eyes.\n\r", ch);
                        
                    
                    return;
                
            }
        }
        }
        else
        {
    	    send_to_char("Your tears try to flush the dirt from your eyes... but you still can't see!\n\r",ch);
	    return;    
        }
    }
    

        
    }

    return;
}

