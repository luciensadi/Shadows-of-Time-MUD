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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>                /* For execl in copyover() */
#include <sys/wait.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "db.h"
#include "magic.h"
#include "rdns_cache.h"
#include "telnet.h"
#include "dns_slave.h"
#include "mysql.h"

extern AREA_DATA *area_first;
/*
 * Local functions.
 */
ROOM_INDEX_DATA *find_location args ((CHAR_DATA * ch, char *arg));
void copyover args ((CHAR_DATA *ch));
bool check_parse_name(char *);
int char_has   args( (CHAR_DATA *ch, OBJ_DATA *suspect ));
void initialize_list args((void));
void save_vote args((void));
void save_mudstats args((void));
void save_riddle args((void));
void save_gameinfo args((void));
void save_illegal args((void));
void save_stores args((void)); 
void save_groups args((void)); 
void load_groups args((void));
void load_qprize args (( void )); 
void save_qprize args (( void )); 
void clear_vote args((void));
void rdns_cache_destroy args((void));
// void rdns_cache_init args((void));
void abandon_tournament args((void));
bool compressEnd args ((DESCRIPTOR_DATA *d));
void read_from_buffer args ((DESCRIPTOR_DATA *d));
int rp_max_hp args((CHAR_DATA *ch));
void generate_rescue_quest args((CHAR_DATA *ch, CHAR_DATA *questman, int i, bool test));
char *prog_type_to_name args((int type));


#define GROUP_FILE "../data/groups.txt"

void do_prog_list (CHAR_DATA *ch, char *argument)
{
  char arg[MSL];
  argument = one_argument(argument, arg);
  if (!str_cmp(arg, "mob"))
  {
    PROG_LIST *mprg;
    char buf[MSL];
    BUFFER *buffer;
    AREA_DATA *pArea;
    MOB_INDEX_DATA *pMob;
    long vnum;    
    bool found = FALSE;
 
    send_to_char("Listing mobs.\n\r", ch);
    buffer = new_buf();
    pArea = ch->in_room->area;
    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
      if ((pMob = get_mob_index(vnum)))
      {
         if (pMob->mprogs)
         {
            sprintf(buf, "[%5ld] - %s - ", vnum, pMob->short_descr);
            add_buf(buffer, buf);
            for (mprg = pMob->mprogs; mprg != NULL ; mprg = mprg->next)
            {
              sprintf(buf, "[%5ld] %s %s", mprg->vnum, 
		prog_type_to_name(mprg->trig_type),
                mprg->trig_phrase);
              add_buf(buffer, buf);
              found = TRUE;
            } // end for
            add_buf(buffer, "\n\r");
          } // end if
      } // end if
    } // end for
    if (!found)
      send_to_char("No mobs with progs found.\n\r", ch);
    else
      page_to_char(buf_string(buffer), ch);
    free_buf(buffer);
  }

  if (!str_prefix(arg, "object"))
  {
    PROG_LIST *oprg;
    char buf[MSL];
    BUFFER *buffer;
    AREA_DATA *pArea;
    OBJ_INDEX_DATA *pObj;
    long vnum;    
    bool found = FALSE;
 
    send_to_char("Listing objects.\n\r", ch);
    buffer = new_buf();
    pArea = ch->in_room->area;
    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
      if ((pObj = get_obj_index(vnum)))
      {
         if (pObj->oprogs)
         {
            sprintf(buf, "[%5ld] - %s - ", vnum, pObj->short_descr);
            add_buf(buffer, buf);
            for (oprg = pObj->oprogs; oprg != NULL ; oprg = oprg->next)
            {
              sprintf(buf, "[%5ld] %s %s", oprg->vnum, 
		prog_type_to_name(oprg->trig_type),
                oprg->trig_phrase);
              add_buf(buffer, buf);
              found = TRUE;
            } // end for
            add_buf(buffer, "\n\r");
          } // end if
      } // end if
    } // end for
    if (!found)
      send_to_char("No objects with progs found.\n\r", ch);
    else
      page_to_char(buf_string(buffer), ch);
    free_buf(buffer);
  }

  if (!str_cmp(arg, "room"))
  {
    PROG_LIST *rprg;
    char buf[MSL];
    BUFFER *buffer;
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    long vnum;    
    bool found = FALSE;
 
    send_to_char("Listing rooms.\n\r", ch);
    buffer = new_buf();
    pArea = ch->in_room->area;
    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
      if ((pRoom = get_room_index(vnum)))
      {
         if (pRoom->rprogs)
         {
            sprintf(buf, "[%5ld] - %s - ", vnum, pRoom->name);
            add_buf(buffer, buf);
            for (rprg = pRoom->rprogs; rprg != NULL ; rprg = rprg->next)
            {
              sprintf(buf, "[%5ld] %s %s", rprg->vnum, 
		prog_type_to_name(rprg->trig_type),
                rprg->trig_phrase);
              add_buf(buffer, buf);
              found = TRUE;
            } // end for
            add_buf(buffer, "\n\r");
          } // end if
      } // end if
    } // end for
    if (!found)
      send_to_char("No rooms with progs found.\n\r", ch);
    else
      page_to_char(buf_string(buffer), ch);
    free_buf(buffer);
  }

}

void do_mobprog_clear(CHAR_DATA *ch, char *argument)
{
  if (!str_cmp(argument, "print"))
    program_flow(0, 0, NULL, NULL, NULL, ch, NULL, NULL, -32000);
  if (!str_cmp(argument, "clear"))
    program_flow(0, 0, NULL, NULL, NULL, ch, NULL, NULL, -32001);
  return;
}  

void do_illegal(CHAR_DATA *ch, char *argument)
{
  char arg[MIL];
  argument = one_argument(argument, arg);
  
  if (!str_cmp(arg, "list"))
  {
    char buf[MSL];
    send_to_char("List of illegal names:\n\r", ch);
    for (int i = 0;i < top_illegal;i++)
    {
      sprintf(buf, "%d %s\n\r", i, illegal_names[i]);
      send_to_char(buf, ch);
    }
    return;
  }

  if (!str_cmp(arg, "add"))
  {
    if (top_illegal + 1 > MAX_ILLEGAL_NAMES)
    {
      send_to_char("No more room for illegal names.\n\r", ch);
      return;
    }
    illegal_names[top_illegal] = str_dup(argument);
    top_illegal++;
    send_to_char("Name added", ch);
    return;
  }

  if (!str_cmp(arg, "change"))
  {
    char arg2[MIL];
    argument = one_argument(argument, arg2);
    if (!is_number(arg2))
    {
      send_to_char("Syntax is illegal change <#> <newname>\n\r", ch);
      return;
    }

    int number;
    number = atoi(arg2);
    if (IS_NULLSTR(illegal_names[number]))
    {
      send_to_char("No such illegal name.\n\r", ch);
      return;
    }
    free_string(illegal_names[number]);
    illegal_names[number] = str_dup(argument);
    send_to_char("Name changed.\n\r", ch);
    return;
  }
  
  if (!str_cmp(arg, "save"))
  {
    save_illegal();
    send_to_char("Saved.\n\r", ch);
    return; 
  }
  send_to_char("That command not implemented.  Possibilities are add, change, list, save.\n\r", ch);
}

  
void do_test_act (CHAR_DATA *ch, char *argument)
{
  MOB_INDEX_DATA *test;

  if (( test = get_mob_index(atoi(argument))) == NULL)
  {
    send_to_char("No such mob\n\r", ch);
    return;
  }
 
  if (IS_SET(test->act, ACT_NOQUEST))
    send_to_char("No QUEST!\n\r", ch);
  else
    send_to_char("Can QUEST!\n\r", ch);
}

void do_rescue_test (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA * questman;
    int i;
    
    for ( questman = ch->in_room->people; questman != NULL; questman = questman->next_in_room )
    {
        if (!IS_NPC(questman)) continue;
        if (questman->spec_fun == spec_lookup( "spec_questmaster" )) break;
    }
                
    if (questman == NULL || questman->spec_fun != spec_lookup( "spec_questmaster" ))
    {
        send_to_char("You can't do that here.\n\r",ch);
        return;
    }
    
    for (i = 0; i < 20; i++)
      generate_rescue_quest(ch, questman, 0, TRUE);
}
void do_print_delay( CHAR_DATA *ch, char *argument)
{
  DELAY_CODE *list;
  char buf[MSL];

  for (list = delay_list;list; list = list->next)
  {
    sprintf(buf, "Prog %ld | Line %d | Time %ld | Current T %ld\n\r", 
       list->vnum, list->line, list->time, current_time);
    send_to_char(buf, ch);
  }
}

void print_class_stats (CHAR_DATA *ch, int cClass)
{
  char buf[MSL];
  int j, k;
  BUFFER *buffer;

    buffer = new_buf();
    sprintf(buf, "Stats for %s\n\r", class_table[cClass].name);
    add_buf(buffer, buf);
    add_buf(buffer, "***************************************\n\r");
    sprintf(buf, "%-10s  %-5s %-5s %-5s %-5s %-5s\n\r",
		"Name", "Str", "Int", "Wis", "Dex", "Con");
    add_buf(buffer, buf);
    for (j = 1; j < MAX_PC_RACE; j++)
    {
      sprintf(buf, "%-10s: ", pc_race_table[j].name);
      add_buf(buffer, buf);
      for (k = 0; k < MAX_STATS; k++)
      {
        sprintf(buf, "%-5d ", UMIN(pc_race_table[j].modstats[k] + class_table[cClass].modstat[k], 25));
        add_buf(buffer, buf);
      }
      add_buf(buffer, "\n\r");
    }
    add_buf(buffer, "\n\r\n\r");
    page_to_char(buf_string(buffer), ch);
    free_buf(buffer);
}

void do_puke (CHAR_DATA *ch, char *argument)
{
  int i;
  for (i = 0; i < MAX_CLASS;i++)
  {
    print_class_stats(ch, i);
  }
}

void do_idle (CHAR_DATA *ch, char *argument)
{
  char buf[MSL];

  checkStall = TRUE;
  sprintf(buf, "Last idle time checked was %3.2f", stall_percent);
  send_to_char(buf, ch);
}

void do_ntoggle(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  
  if (ch->clan != clan_lookup("Guide") && !IS_IMMORTAL(ch))
  {
    if (IS_SET(ch->comm, COMM_NOVICE))
    {
       send_to_char("Novice channel turned off.\n\r", ch);
       REMOVE_BIT(ch->comm, COMM_NOVICE);
       return;
    }
    send_to_char("You can't turn that channel on, ask a guide member if you still need help.\n\r", ch);
    return;
  }
  
  if (argument[0] == '\0')
  {
    send_to_char("Toggle Novice Channel on who?\n\r", ch);
    return;
  }
  
  if (( victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("They aren't logged on at the moment.\n\r", ch);
    return;
  }
  
  if (IS_SET(victim->comm, COMM_NOVICE))
  {
    REMOVE_BIT(victim->comm, COMM_NOVICE);
    send_to_char("Novice channel privileges removed.\n\r", ch);
    send_to_char("Your novice channel privileges have been removed.\n\r", victim);
    return;
  }
  
  SET_BIT(victim->comm, COMM_NOVICE);
  send_to_char("Novice channel privileges given.\n\r", ch);
  send_to_char("Welcome to the novice channel.\n\r", victim);
  return;
}
  
  
    
void do_clearheal(CHAR_DATA *ch, char *argument)
{
  int min, max, i;
  char arg1[MIL];
  char arg2[MIL];
  ROOM_INDEX_DATA *pRoom;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);


  min = atoi(arg1);
  max = atoi(arg2);
  
  for (i = min; i <= max; i++)
  {
    if ((pRoom = get_room_index(i)) == NULL)
      continue;

     pRoom->heal_rate = 100;
     pRoom->mana_rate = 100;
  }
}
 

void do_addlag(CHAR_DATA *ch, char *argument)
{

	CHAR_DATA *victim;
	char arg1[MAX_STRING_LENGTH];
	int x;

	argument = one_argument(argument, arg1);

	if (arg1[0] == '\0')
	{
		send_to_char("addlag to who?", ch);
		return;
	}

	if ((victim = get_char_world(ch, arg1)) == NULL)
	{
		send_to_char("They're not here.", ch);
		return;
	}

	if ((x = atoi(argument)) <= 0)
	{
		send_to_char("That makes a LOT of sense.", ch);
		return;
	}

	if (x > 100)
	{
		send_to_char("There's a limit to cruel and unusual punishment", ch);
		return;
	}

	WAIT_STATE(victim, x);
	send_to_char("Adding lag now...", ch);
	return;
}

int calc_avedam(int num_dice, int dam_dice)
{
  return ((1 + dam_dice) * num_dice / 2);
} 

void do_eqlookup(CHAR_DATA *ch, char *argument)
{
  OBJ_INDEX_DATA *pObjIndex;
  char buf [MSL];

  if ((pObjIndex = get_obj_index(atoi(argument))) == NULL)
  {
    send_to_char("No such vnum\n\r", ch);
    return;
  }

  sprintf(buf, "%ld - %s", pObjIndex->vnum, pObjIndex->short_descr);
  send_to_char(buf, ch);
}

void do_savegroup(CHAR_DATA *ch, char *argument)
{
  save_groups();
  send_to_char("Ok\n\r", ch);
}

void do_avedam(CHAR_DATA *ch, char *argument)
{
  int i, j;
  int k = 0;
  int lookingfor;
  char buf[MSL];

  if (argument[0] == '\0')
  {
    send_to_char("What average dam are you looking for?", ch);
    return;
  }

  lookingfor = atoi(argument);
  sprintf(buf, "Possible dice combinations for ave dam: %d\n\r", 
           lookingfor);
  send_to_char(buf, ch);
  send_to_char("T_Dice D_Dice   T_Dice D_Dice\n\r", ch); 

  for (i = 1;i <= lookingfor;i++)
  {
    for (j = 1; j <= lookingfor; j++)
    {
      if (lookingfor == calc_avedam(i, j))
      {
        k++;
        sprintf(buf, "  %2d     %2d     ", i, j);
        send_to_char(buf, ch);
        if (k % 2 == 0)
          send_to_char("\n\r", ch);
      }
    }
  }
  send_to_char("\n\r", ch);
}



void do_rphp(CHAR_DATA *ch, char *argument)
{
  char buf[MSL];
    CHAR_DATA *victim;

    if ( IS_NPC(ch) )
        return;

    if ((victim = get_char_world(ch, argument)) == NULL)
      {
         send_to_char("No one by that name in the world.", ch);
         return;
      }

    sprintf(buf, "%d hp", rp_max_hp(victim));
    send_to_char(buf, ch);
}

void do_skillstat(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int sn;
    CHAR_DATA *victim;

    if ( IS_NPC(ch) )
        return;

    if ((victim = get_char_world(ch, argument)) == NULL)
      {
         send_to_char("No one by that name in the world.", ch);
         return;
      }

    if ( IS_NPC(victim) )
      {
        send_to_char("Not on NPCs.\n\r", ch);
        return;
      }


    {
        int col;

        col    = 0;
        for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
            if ( skill_table[sn].name == NULL )
                break;
            if ( victim->level < skill_table[sn].skill_level[victim->cClass]
              || victim->pcdata->learned[sn] < 1 /* skill is not known */)
                continue;

            sprintf( buf, "%-18s %3d%%  ",
                skill_table[sn].name, victim->pcdata->learned[sn] );
            send_to_char( buf, ch );
            if ( ++col % 3 == 0 )
                send_to_char( "\n\r", ch );
        }

        if ( col % 3 != 0 )
            send_to_char( "\n\r", ch );

      }
}


void do_personal_mult(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);

  if (arg1[0] == '\0' || !is_number(arg2) || !is_number(arg3))
  {
    send_to_char("Syntax is pmult <victim> <multiplier> <time>\n\r", ch);
    return;
  }

  if ((victim = get_char_world(ch, arg1)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  victim->pcdata->xpmultiplier.on = TRUE;
  victim->pcdata->xpmultiplier.factor = atoi(arg2);
  victim->pcdata->xpmultiplier.time = atoi(arg3);

  if (victim->pcdata->xpmultiplier.time < 0)
  {
    send_to_char("Time must be a positive number.\n\r", ch);
    return;
  }

  if (victim->pcdata->xpmultiplier.factor < -5 ||
       victim->pcdata->xpmultiplier.factor > 5)
  {
    send_to_char("Chose a factore between -5 and 5\n\r", ch);
    return;
  }

  if (victim->pcdata->xpmultiplier.factor < 0)
  {
    send_to_char("Congratz you earned minus time.\n\r", victim);
    return;
  }

  if (victim->pcdata->xpmultiplier.factor > 0)
  {
    send_to_char("Congratz you earned bonus time.\n\r", victim);
    return;
  }
}
  
void do_developer( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;

  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("They aren't logged on.\n\r", ch);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("Not on NPCs.\n\r", ch);
    return;
  }
  
  if (IS_SET(victim->act2, PLR_DEVELOPER))
  {
    send_to_char("They are no longer a developer.\n\r", ch);
    send_to_char("Your developer status has been removed.\n\r", victim);
    REMOVE_BIT(victim->act2, PLR_DEVELOPER);
    return;
  }

  send_to_char("They are now a developer.\n\r", ch);
  send_to_char("Welcome to developer status, you can now read ideas.\n\r", victim);
  SET_BIT(victim->act2, PLR_DEVELOPER);
}

void do_maxstats( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char buf[MSL];

  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("They aren't logged on.\n\r", ch);
    return;
  }
 
  sprintf(buf, "%s has a maximum of %d hp and %d mana.\n\r", 
	  victim->name, max_natural_hp(victim), max_natural_mana(victim));
  send_to_char(buf, ch);
}

void do_lstat( CHAR_DATA *ch, char *argument)
{
  char buf[MSL];
  int level;

  if (argument[0] == '\0' || !is_number(argument))
  {
    send_to_char("Syntax is lstat <level of char>\n\r", ch);
    return;
  }
  
  level = atoi(argument);
  if (level >= MAX_LEVEL - 5)
  {
    sprintf(buf, "A Level 75+ character can wear anything.\n\r");
  }
  else
    sprintf(buf, "A Level %d character can wear a level %d object.\n\r",
              level,
              (2*level/5) + 5 + level);
  send_to_char(buf, ch);
}

void do_istat( CHAR_DATA *ch, char *argument)
{
  char buf[MSL];
  int level, result;


  if (argument[0] == '\0' || !is_number(argument))
  {
    send_to_char("Syntax is istat <level of item>\n\r", ch);
    return;
  }
  
  level = atoi(argument);
  result = ((level - 5) * 5) / 7; 
  if (result >= MAX_LEVEL - 5)
  {
    result = MAX_LEVEL - 5; 
  }

  sprintf(buf, "A Level %d character can wear a level %d object.\n\r",
              result,
              level);
  send_to_char(buf, ch);
  send_to_char("Istat results are not valid about 33% of the time, this is known will\n\r"
               "not be fixed.  If you want to be sure use lstat.\n\r", ch);
}
 
void do_save_vote( CHAR_DATA *ch, char *argument)
{
  save_vote();
  send_to_char("Saved.\n\r", ch);
}

void do_save_guilds( CHAR_DATA *ch, char *argument)
{
  save_guilds();
  send_to_char("Saved.\n\r", ch);
}

void do_load_qprize( CHAR_DATA *ch, char *argument)
{
  load_qprize();
  send_to_char("Loaded.\n\r", ch);
}

void do_save_qprize( CHAR_DATA *ch, char *argument)
{
  save_qprize();
  send_to_char("Saved.\n\r", ch);
}

void do_load_guilds( CHAR_DATA *ch, char *argument)
{
  load_guilds();
  send_to_char("Loaded.\n\r", ch);
}

void do_vote_create ( CHAR_DATA *ch, char *argument)
{

  if (argument[0] == '\0')
  {
    send_to_char("Choose edit or clear.\n\r", ch);
    return;
  }

  if (!str_cmp(argument, "clear"))
  {
     VOTERS_DATA *vch, *vch_next;

     for (vch = voteinfo.voters ; vch != NULL ; vch = vch_next)
     {
       vch_next = vch->next;
       voteinfo.voters = vch->next;
       free_voter(vch);
     }

     voteinfo.on = FALSE; 
     voteinfo.question = str_dup("No Vote Running");
     voteinfo.forit = 0;
     voteinfo.against = 0;
     clear_vote(); 
  }

  if (!str_cmp(argument, "edit"))
  {
    string_append(ch, &voteinfo.question);
    voteinfo.on = TRUE;
  }
}


void do_quest_restart( CHAR_DATA *ch, char *argument)
{
  initialize_list();
  send_to_char("Quest list re-initilalized.\n\r", ch);
  return;
}

void do_questchar( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char buf[MSL];
  
  if (argument[0] == '\0')
  {
    send_to_char("Add who to the quest.\n\r", ch);
    return;
  }
  
  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("But they aren't logged on.\n\r", ch);
    return;
  }
  
  if (IS_SET(victim->act, PLR_QUESTING))
  {
    REMOVE_BIT(victim->act, PLR_QUESTING);
    sprintf(buf, "%s has been removed from the quest.\n\r", victim->name);
    send_to_char(buf, ch);
    send_to_char("You have been removed from the quest.\n\r", victim);
    return;
  }
  
  SET_BIT(victim->act, PLR_QUESTING);
  sprintf(buf, "%s has been added to the quest.\n\r", victim->name);
  send_to_char(buf, ch);
  send_to_char("You have been added to the quest.\n\r", victim);
}

void do_spellup( CHAR_DATA *ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  int  sn = -1;
  int target;
  void *vo;
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;

  victim = NULL;
 
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  
  if ((arg1[0] == '\0') || (arg2[0] == '\0'))
  {
    send_to_char("Spellup who with what?\n\r", ch);
    send_to_char("Syntax is:\n\r",ch);
    send_to_char("spellup <name or all> <spell>\n\r", ch);
    send_to_char("Examples:\n\r", ch);
    send_to_char("spellup demiscus sanctuary\n\r", ch);
    send_to_char("spellup all haste\n\r", ch);
    return;
  }

  if ((sn = skill_lookup(arg2)) == -1)
  {
    send_to_char("That spell doesn't exist.\n\r", ch);
    return;
  }
  
  if (skill_table[sn].target != TAR_CHAR_DEFENSIVE &&
        skill_table[sn].target != TAR_IGNORE &&
        skill_table[sn].target != TAR_CHAR_SELF &&
        skill_table[sn].target != TAR_OBJ_CHAR_DEF  )
  {
     send_to_char("Not that spell.\n\r", ch);
     return;
  }


  if (!strcmp(arg1, "all"))
  {
     for ( d = descriptor_list; d != NULL; d = d->next)
     {
       int level = 81;       
       victim = d->character ? d->character : d->original;

       if (d->connected != CON_PLAYING)
         continue;

       if (victim == NULL || IS_NPC(victim))
          continue;
            
       vo = (void *) victim;
       target = TARGET_CHAR;
       level = ch ? ch->level : 200;
       (*skill_table[sn].spell_fun) ( sn, level, ch, vo, target);
      }
      send_to_char("All Ok.", ch);
   }
   else
   {
     if ((victim = get_char_world(ch, arg1)) == NULL)
     {
       send_to_char("They aren't here.\n\r", ch);
       return;
     }
     vo = (void *) victim;
     target = TARGET_CHAR;
     (*skill_table[sn].spell_fun) ( sn, ch->level, ch, vo, target);
     send_to_char("Ok.", ch);
   }
}

void do_cleartick (CHAR_DATA *ch, char *argument) /* Jasin */
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  one_argument(argument, arg);

  if (ch->level < 81)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    send_to_char("Clear whose ticks?\n\r", ch);
    return;
  }

  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if ((victim->pcdata->pk_timer > 0) || (victim->pcdata->safe_timer > 0)
    || (victim->pcdata->wait_timer > 0))
  {
    victim->pcdata->pk_timer = 0;
    victim->pcdata->safe_timer = 0;
    victim->pcdata->wait_timer = 0;

    send_to_char("All ticks cleared.\n\r", ch);
    return;
  }

  if ((victim->pcdata->pk_timer == 0) && (victim->pcdata->safe_timer == 0)
    && (victim->pcdata->wait_timer == 0))
  {
    send_to_char("They are already clear of all ticks.\n\r", ch);
    return;
  }

}


void do_sn( CHAR_DATA * ch, char *argument)
{
  int cmd;
  char buf[MAX_STRING_LENGTH];

  if (argument[0] == '\0' || !is_number(argument))
  {
    send_to_char("Choose a sn.\n\r", ch);
    return;
  }

  cmd = atoi(argument);
  if (skill_table[cmd].name[0] == '\0')
  {
    send_to_char("No such sn.\n\r", ch);
    return;
  }

  sprintf(buf, "That command is %s\n\r", cmd_table[cmd].name);
  send_to_char(buf, ch); 
}

/*:======================================================================:
*| John Strange                                Triad Mud                |
*| gambit@wvinter.net                          triad.telmaron.com 7777  |
*:======================================================================:
*/

void do_resetpassword( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    char *pwdnew;

    if ( IS_NPC(ch) )
    return;

    argument=one_argument( argument, arg1 );
    argument=one_argument( argument, arg2 );

    victim = get_char_world(ch, arg1);

    if (  ( ch->pcdata->pwd != '\0' )
    && ( arg1[0] == '\0' || arg2[0] == '\0')  )
    {
        send_to_char( "Syntax: password <char> <new>.\n\r", ch );
        return;
    }
    if( victim == '\0' )
    {
		send_to_char( "That person isn't here, they have to be here to reset pwd's.\n\r", ch);
	 	return;
    }
    if ( IS_NPC( victim ) )
    {
		send_to_char( "You cannot change the password of NPCs!\n\r",ch);
		return;
    }

    if (( victim->level > LEVEL_IMMORTAL ) || ( get_trust(victim) > LEVEL_IMMORTAL ))
	
	{
		send_to_char( "You can't change the password of that player.\n\r",ch);
		return;
    }
    
    if ( strlen(arg2) < 5 )
    {
        send_to_char( "New password must be at least five characters long.\n\r", ch );
        return;
    }
	
    pwdnew = crypt( arg2, victim->name );
    free_string( victim->pcdata->pwd );
    victim->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( victim );
    send_to_char( "Ok.\n\r", ch );
    sprintf( buf, "Your password has been changed to %s.", arg2 );
    send_to_char( buf, victim);
    return;
}
/********************************************************
 * Confiscate code by Keridan of Benevolent Iniquity    *
 * This code is released under no license and is freely *
 * distributable. I require no credit, but would        *
 * appreciate a quick note to keridan@exile.mudsrus.com *
 * if you choose to use it in your mud.                 *
 *       http://stargazer.inetsolve.com/~keridan        *
 *            stargazer.inetsolve.com 1414              *
 ********************************************************/

void do_confiscate(CHAR_DATA *ch,char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj; 
    char arg1[MAX_INPUT_LENGTH];
    bool found = FALSE;
        
    argument = one_argument(argument,arg1);
    
    if (IS_NPC(ch))
      return;
      
    if ((arg1[0] == '0') || (argument[0] == '\0'))
    { printf_to_char(ch,"Syntax:\n\r");
      printf_to_char(ch,"confiscate <char> <item>\n\r");
      return; }
    
    if ((victim = get_char_world(ch,arg1)) == NULL)
    { printf_to_char(ch,"They aren't here.\n\r");
      return; }
    
    if (IS_NPC(victim))
    { printf_to_char(ch,"They aren't here.\n\r");
      return; }
    
    if ((victim->level >= ch->level)/* && !IS_OWNER(ch) */)
    { printf_to_char(ch,"They are too high level for you to do that.\n\r");
      return; }
    
    for ( obj = victim->carrying; obj != NULL; obj = obj->next_content )
    {
      if ( is_name( argument, obj->name )
      &&   can_see_obj( ch, obj ))
      {
        found = TRUE;
        break;
      }
    }
    
    if (!found)
    { printf_to_char(ch,"They don't have %s.\n\r",argument);
      return; }

    obj_from_char( obj );  
    obj_to_char( obj, ch );
    printf_to_char(ch,"Got it.\n\r");
    printf_to_char(victim,"%s has confiscated %s from you.\n\r",PERS(ch,victim, TRUE),obj->short_descr);
    return;
}

void do_wiznet (CHAR_DATA * ch, char *argument)
{
    int flag;
    char buf[MAX_STRING_LENGTH];

    if (argument[0] == '\0')
    {
        if (IS_SET (ch->wiznet, WIZ_ON))
        {
            send_to_char ("Signing off of Wiznet.\n\r", ch);
            REMOVE_BIT (ch->wiznet, WIZ_ON);
        }
        else
        {
            send_to_char ("Welcome to Wiznet!\n\r", ch);
            SET_BIT (ch->wiznet, WIZ_ON);
        }
        return;
    }

    if (!str_prefix (argument, "on"))
    {
        send_to_char ("Welcome to Wiznet!\n\r", ch);
        SET_BIT (ch->wiznet, WIZ_ON);
        return;
    }

    if (!str_prefix (argument, "off"))
    {
        send_to_char ("Signing off of Wiznet.\n\r", ch);
        REMOVE_BIT (ch->wiznet, WIZ_ON);
        return;
    }

    /* show wiznet status */
    if (!str_prefix (argument, "status"))
    {
        buf[0] = '\0';

        if (!IS_SET (ch->wiznet, WIZ_ON))
            strcat (buf, "off ");

        for (flag = 0; wiznet_table[flag].name != NULL; flag++)
            if (IS_SET (ch->wiznet, wiznet_table[flag].flag))
            {
                strcat (buf, wiznet_table[flag].name);
                strcat (buf, " ");
            }

        strcat (buf, "\n\r");

        send_to_char ("Wiznet status:\n\r", ch);
        send_to_char (buf, ch);
        return;
    }

    if (!str_prefix (argument, "show"))
        /* list of all wiznet options */
    {
        buf[0] = '\0';

        for (flag = 0; wiznet_table[flag].name != NULL; flag++)
        {
            if (wiznet_table[flag].level <= get_trust (ch))
            {
                strcat (buf, wiznet_table[flag].name);
                strcat (buf, " ");
            }
        }

        strcat (buf, "\n\r");

        send_to_char ("Wiznet options available to you are:\n\r", ch);
        send_to_char (buf, ch);
        return;
    }

    flag = wiznet_lookup (argument);

    if (flag == -1 || get_trust (ch) < wiznet_table[flag].level)
    {
        send_to_char ("No such option.\n\r", ch);
        return;
    }

    if (IS_SET (ch->wiznet, wiznet_table[flag].flag))
    {
        sprintf (buf, "You will no longer see %s on wiznet.\n\r",
                 wiznet_table[flag].name);
        send_to_char (buf, ch);
        REMOVE_BIT (ch->wiznet, wiznet_table[flag].flag);
        return;
    }
    else
    {
        sprintf (buf, "You will now see %s on wiznet.\n\r",
                 wiznet_table[flag].name);
        send_to_char (buf, ch);
        SET_BIT (ch->wiznet, wiznet_table[flag].flag);
        return;
    }

}

void do_bugfix(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  int i;  

  if (argument[0] == '\0')
  {
    send_to_char("Do to who?\n\r", ch);
    return;
  }

  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

   do_function(victim, &do_remove, "all");
   for (i = 0; i < 4; i++)
        victim->armor[i] = 100;
   do_function(victim, &do_wear, "all");
}

void wiznet (char *string, CHAR_DATA * ch, OBJ_DATA * obj,
             long flag, long flag_skip, int min_level)
{
    DESCRIPTOR_DATA *d;

    for (d = descriptor_list; d != NULL; d = d->next)
    {
        if (d->connected == CON_PLAYING && IS_IMMORTAL (d->character)
            && IS_SET (d->character->wiznet, WIZ_ON)
            && (!flag || IS_SET (d->character->wiznet, flag))
            && (!flag_skip || !IS_SET (d->character->wiznet, flag_skip))
            && get_trust (d->character) >= min_level && d->character != ch)
        {
            if (IS_SET (d->character->wiznet, WIZ_PREFIX))
                send_to_char ("`&--> ", d->character);
            else
                send_to_char ("`&", d->character);
            act_new (string, d->character, obj, ch, TO_CHAR, POS_DEAD);
            send_to_char ("`*", d->character);
        }
    }

    return;
}

void do_guild (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int clan;
    char lookup[MSL];
    int oldclan;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (!IS_HIIMMORTAL(ch) && ch->rank < 8)
    {
      do_function(ch, &do_guildlist, ""); 
      return;
    }

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char ("Syntax: guild <char> <cln name>\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg1)) == NULL)
    {
        send_to_char ("They aren't playing.\n\r", ch);
        return;
    }

    if (IS_NPC(victim))
    {
      send_to_char("They don't want to join your stinkin' guild.\n\r", ch);
      return;
    }
 
    if (!IS_IMMORTAL(ch) && IS_IMMORTAL(victim))
    {
      send_to_char("You don't have the ability to change their guild.\n\r", ch);
      return;
    }

    if (!str_prefix (arg2, "none") && IS_IMMORTAL(ch))
    {
        send_to_char ("They are now clanless.\n\r", ch);
        send_to_char ("You have been removed from your guild!\n\r", victim);
        remove_member(victim->name, victim->clan);
        save_roster();
        victim->clan = 0;
        victim->rank = 0;
        return;
    }

    if (!str_prefix (arg2, "none") && !IS_IMMORTAL(ch))
    {
        send_to_char ("They are now outcasted.\n\r", ch);
        send_to_char ("You have been banished from your guild!\n\r", victim);
        do_function(ch, &do_skillstrip, victim->name);
        remove_member(victim->name, victim->clan);
        save_roster();
        victim->clan = clan_lookup("Outcast");
        victim->rank = 0;
        return;
    }

    if (!str_prefix (arg2, "none") && IS_IMMORTAL(ch))
    {
      do_function(ch, &do_skillstrip, victim->name);
    }
   
    if ((clan = clan_lookup (arg2)) == -1)
    {
        send_to_char ("No such clan exists.\n\r", ch);
        return;
    }

    if (clan != ch->clan && !IS_IMMORTAL(ch)) 
    {
      if (!str_prefix(arg2, "none") && victim->clan == ch->clan)
      {
      	return;
      }
      else
      {
        send_to_char("You can only guild into your own guild.\n\r", ch);
        return;
      }
    }

    if (victim->clan != 0 && !IS_IMMORTAL(ch) && str_cmp(arg2, "none"))
    {
      send_to_char("You can't change their guild.\n\r", ch);
      return;
    }
      
    if (TRUE)
    {
        sprintf (buf, "They are now a %s.\n\r", clan_table[clan].name);
        send_to_char (buf, ch);
        sprintf (buf, "You are now a %s.\n\r", clan_table[clan].name);
        send_to_char (buf, victim);
    }
    else
    {
        sprintf (buf, "They are now a member of %s.\n\r",
                 capitalize (clan_table[clan].name));
        send_to_char (buf, ch);
        sprintf (buf, "You are now a member of %s.\n\r",
                 capitalize (clan_table[clan].name));
    }
    sprintf( lookup, "%s", victim->name);
    oldclan = victim->clan;
    if (IS_SET(victim->act, PLR_NOPK))
       REMOVE_BIT(victim->act, PLR_NOPK);
    if (!remove_member(capitalize(lookup), oldclan))
      send_to_char("Couldn't find.\n\r", ch);
    victim->clan = clan;
    victim->rank = 0;
    add_member(victim->name, 1, victim->clan);
    save_roster();
}

void do_forcetick(CHAR_DATA *ch, char *argument)
{
  send_to_char("Forcing tick.\n\r", ch);
  forceTick = TRUE;
  return;
}


void do_outfit ( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int i,sn;
    long vnum;

    if (IS_NPC(ch))
    {
	     send_to_char("Find it yourself!\n\r",ch);
	    return;
    }


    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) == NULL )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0 );
        if (char_has(ch, obj) > 1)
        {
          extract_obj(obj);
        }
        else
        {
	        obj->cost = 0;
          obj_to_char( obj, ch );
          equip_char( ch, obj, WEAR_LIGHT );
        }
    }
 
    if ( ( obj = get_eq_char( ch, WEAR_BODY ) ) == NULL )
    {
	     obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0 );
       if (char_has(ch, obj) > 1)
       {
          extract_obj(obj);
       }
	     else
 	     {
	        obj->cost = 0;
          obj_to_char( obj, ch );
          equip_char( ch, obj, WEAR_BODY );
	     }
    }

  

    /* do the weapon thing */
    if ((obj = get_eq_char(ch,WEAR_WIELD)) == NULL)
    {
    	sn = 0; 
    	vnum = OBJ_VNUM_SCHOOL_SWORD; /* just in case! */

    	for (i = 0; weapon_table[i].name != NULL; i++)
    	{
	      if (ch->pcdata->learned[*weapon_table[i].gsn] == 40)
	      {
	    	  sn = *weapon_table[i].gsn;
	    	  vnum = weapon_table[i].vnum;
	      }
    	}

    	  obj = create_object(get_obj_index(vnum),0);
        if (char_has(ch, obj) > 1)
        {
	        extract_obj(obj);
	      }
	      else
      	{
      	  obj_to_char(obj,ch);
    	    equip_char(ch,obj,WEAR_WIELD);
	      }
    }

    if (((obj = get_eq_char(ch,WEAR_WIELD)) == NULL 
    ||   !IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)) 
    &&  (obj = get_eq_char( ch, WEAR_SHIELD ) ) == NULL 
    &&  (obj = get_eq_char( ch, WEAR_SECONDARY ) ) == NULL )

    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0 );
        if (char_has(ch, obj) > 1)
        {
	         extract_obj(obj);
	      }
	      else
	      {
	        obj->cost = 0;
          obj_to_char( obj, ch );
          equip_char( ch, obj, WEAR_SHIELD );
	      }
    }

    send_to_char("You have been equipped by The Creator.\n\r",ch);
}
void do_war( CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];

argument = one_argument(argument, arg1);

if (arg1[0] == '\0')
{
  send_to_char ("syntax: war on/off\n\r", ch);
  return;
} 

if (arg1 == "on")
{
  send_to_char ("War Enabled\n\r", ch);
  do_echo(ch, "`&A `!Global War `&has started! Flee! Flee the Village!\n\r");
  war.on = TRUE;
  return;
}
if (arg1 == "off")
{
  send_to_char ("War Disabled\n\r", ch);
  do_echo(ch, "`&The `!Global War `&has ended, the area is safe!\n\r");
  war.on = FALSE;
  return;
}
}
void do_bonus_time( CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  char arg2[MIL];
  char buf[MSL];  
  int mult, time;
  DESCRIPTOR_DATA *d;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    sprintf(buf, "Currently Bonus time is %s multiplying xp by %d for %d more ticks\n\r",
             xp_bonus.on ? "on" : "off", xp_bonus.multiplier,
             xp_bonus.time_left); 
    send_to_char(buf, ch);
    send_to_char("\n\rSyntax is : bonus <multiplier> <time>\n\r", ch);
    send_to_char(" The multiplier can range from 2 to 5\n\r", ch);
    send_to_char(" The time from -1 to 1000 : -1 being\n\r ", ch);
    send_to_char(" infinite, 0 turning it off.\n\r", ch);
  }

  mult = atoi(arg1);
  time = atoi(arg2);

  if (mult < 2 || mult > 5)
  {
    send_to_char("Choose a multiplier between 2 and 5", ch);
    return;
  }

  if (time < -1 || time > 1000)
  {
    send_to_char("Chose a time between -1 and 1000\n\r", ch);
    return;
  }

  if (time == 0)
  {
    xp_bonus.time_left = 0;
    send_to_char("Bonus XP turned off.\n\r", ch);
    return;
  }

  xp_bonus.on = TRUE;
  xp_bonus.negative = FALSE;
  xp_bonus.time_left = time;
  xp_bonus.multiplier = mult;

  sprintf(buf, "`&XP gains pumped up it's bonus time`!!!!`*\n\r");

  for (d = descriptor_list; d != NULL; d = d->next)
  {
     CHAR_DATA *victim;
          
     victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING)
      {  
        if (victim->nextquest > 1)
          victim->nextquest = 1;
        send_to_char(buf, victim);
        if (IS_SET(victim->act2, PLR_MSP_MUSIC))
        {
          send_to_char("!!MUSIC(Sounds/BonusS.mid V=80 L=-1 C=1 T=BonusTime)", victim); 
	  send_to_char("\n\r", victim);
        }
      }
  } 
}

void do_minus_time( CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  char arg2[MIL];
  char buf[MSL];  
  int mult, time;
  DESCRIPTOR_DATA *d;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    sprintf(buf, "Currently Minus time is %s multiplying xp by %d for %d more ticks\n\r",
             xp_bonus.on ? "on" : "off", xp_bonus.multiplier,
             xp_bonus.time_left); 
    send_to_char(buf, ch);
    send_to_char("\n\rSyntax is : minus <multiplier> <time>\n\r", ch);
    send_to_char(" The multiplier can range from 2 to 5\n\r", ch);
    send_to_char(" The time from -1 to 1000 : -1 being\n\r ", ch);
    send_to_char(" infinite, 0 turning it off.\n\r", ch);
  }

  mult = atoi(arg1);
  time = atoi(arg2);

  if (mult < 2 || mult > 5)
  {
    send_to_char("Choose a multiplier between 2 and 5", ch);
    return;
  }

  if (time < -1 || time > 1000)
  {
    send_to_char("Chose a time between -1 and 1000\n\r", ch);
    return;
  }

  if (time == 0)
  {
    xp_bonus.time_left = 0;
    send_to_char("Minus XP turned off.\n\r", ch);
    return;
  }

  xp_bonus.on = TRUE;
  xp_bonus.negative = TRUE;
  xp_bonus.time_left = time;
  xp_bonus.multiplier = mult;

  sprintf(buf, "`&XP gains pumped down it's minus time`!!!!`*\n\r");
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected == CON_PLAYING)
      send_to_char(buf, d->character);
  } 
}
 
 
void do_rename (CHAR_DATA* ch, char* argument)
{
	char old_name[MAX_INPUT_LENGTH], 
	     new_name[MAX_INPUT_LENGTH],
	     strsave [MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
        bool roster = FALSE;
	CHAR_DATA* victim;
	FILE* file;
	STORE_DATA *store;

	argument = one_argument(argument, old_name); /* find new/old name */
	one_argument (argument, new_name);
	
	/* Trivial checks */
	if (!old_name[0])
	{
		send_to_char ("Rename who?\n\r",ch);
		return;
	}
	
	victim = get_char_world (ch, old_name);
	
	if (!victim)
	{
		send_to_char ("There is no such a person online.\n\r",ch);
		return;
	}
	
	if (IS_NPC(victim))
	{   
		send_to_char ("You cannot use Rename on NPCs.\n\r",ch);
		return;
	}

	/* allow rename self new_name,but otherwise only lower level */	
	if ( (victim != ch) && (get_trust (victim) >= get_trust (ch)) )
	{
		send_to_char ("You failed.\n\r",ch);
		return;
	}
	
	if (!victim->desc || (victim->desc->connected != CON_PLAYING) )
	{
		send_to_char ("This player has lost his link or is inside a pager or the like.\n\r",ch);
		return;
	}

	if (!new_name[0])
	{
        send_to_char ("Rename to what new name?\n\r",ch);
		return;
	}
	
	/* Insert check for clan here!! */
	/*

	if (victim->clan)
	{
		send_to_char ("This player is member of a clan, remove him from there first.\n\r",ch);
		return;
	}
	*/
	
	if (!check_parse_name(new_name))
	{
		send_to_char ("The new name is illegal.\n\r",ch);
		return;
	}

	/* First, check if there is a player named that off-line */	

        sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( new_name ) );




	fclose (fpReserve); /* close the reserve file */
	file = fopen (strsave, "r"); /* attempt to to open pfile */
	if (file)
	{
             char buf[MSL];
		send_to_char ("A player with that name already exists!\n\r",ch);
		sprintf(buf, "Name %s", new_name);
		send_to_char(buf, ch);
		fclose (file);
    	fpReserve = fopen( NULL_FILE, "r" ); /* is this really necessary these days? */
		return;		
	}
   	fpReserve = fopen( NULL_FILE, "r" );  /* reopen the extra file */

	/* Check .gz file ! */

      sprintf( strsave, "%s%s.gz", PLAYER_DIR, capitalize( new_name ) );


	fclose (fpReserve); /* close the reserve file */
	file = fopen (strsave, "r"); /* attempt to to open pfile */
	if (file)
	{
		send_to_char ("A player with that name already exists in a compressed file!\n\r",ch);
		fclose (file);
    	fpReserve = fopen( NULL_FILE, "r" ); 
		return;		
	}
   	fpReserve = fopen( NULL_FILE, "r" );  /* reopen the extra file */

	if (get_char_world_ooc(ch,new_name)) /* check for playing level-1 non-saved */
	{
		send_to_char ("A player with the name you specified already exists!\n\r",ch);
		return;
	}

	/* Save the filename of the old name */


        sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( victim->name ) );



	/* Rename the character and save him to a new file */
	/* NOTE: Players who are level 1 do NOT get saved under a new name */

	if (victim->clan != 0)
        {
          roster = TRUE;
          remove_member(victim->name, victim->clan);
        }
        free_string (victim->name);
	victim->name = str_dup (capitalize(new_name));
        victim->id = get_pc_id();
        if (roster)
        {
          add_member(victim->name, victim->rank + 1, victim->clan);	
        }
	save_char_obj (victim);
	
	/* unlink the old file */
	unlink (strsave); /* unlink does return a value.. but we do not care */
        unlink (old_name);

	/* That's it! */
	
	send_to_char ("Character renamed.\n\r",ch);

	victim->position = POS_STANDING; /* I am laaazy */
	act ("$n has renamed you to $N!",ch,NULL,victim,TO_VICT);
			
	for (store = store_list;store != NULL; store = store->next)
	{
		if (!str_cmp(store->owner, old_name))
			break;
	}
	
        if(store)
        {
	    ROOM_INDEX_DATA *room = get_room_index(store->vnum);
	    CHAR_DATA *keeper = get_char_world(ch, store->shopkeeper.keeper);

	    free_string(store->owner);
	    store->owner = str_dup(victim->name);

	    if(room)
	    {
		free_string(room->name);
		sprintf(buf, "%s's Store", victim->name);
		room->name = str_dup(buf);
		free_string(room->description);
		sprintf(buf, "%s's store is waiting to be decorated and filled.", victim->name);
		room->description = str_dup(buf);
		SET_BIT(room->area->area_flags, AREA_CHANGED);

		send_to_char("Renamed current store.\n\r", ch);
		if(keeper)
		{
		    free_string(keeper->name);
		    sprintf(buf, "%s Shopkeep", victim->name);
		    keeper->name = str_dup(buf);
		    send_to_char("Renamed current shopkeeper.\n\r", ch);
		}
	    }
    
        }
} /* do_rename */

/*
* Add this somewhere in act_wiz.c
*/
void do_enchant(CHAR_DATA *ch, char *argument)
{
	OBJ_DATA *obj;
	AFFECT_DATA paf;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	int affect_modify = 0, bit = 0, enchant_type, pos, i;
	bool found = TRUE;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	
	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
	{
		send_to_char("Syntax for applies: enchant <object> <apply type> <value>\n\r",ch);
		send_to_char("Apply Types: hp str dex int wis con mana\n\r", ch);
		send_to_char("             ac move hitroll damroll saves\n\r\n\r", ch);
		send_to_char("Syntax for affects: enchant <object> affect <affect name>\n\r",ch);
		send_to_char("Affect Names: blind invisibility detect_evil detect_invis detect_magic\n\r",ch);
		send_to_char("              detect_hidden detect_good sanctuary faerie_fire infrared\n\r",ch);
		send_to_char("              curse poison protection_evil protection_good sneak hide sleep\n\r", ch);
		send_to_char("              charm fly pass_door haste calm plague weaken berserk slow\n\r", ch);
		send_to_char("              armor shield stoneskin bless frenzy\n\r", ch);
		return;
	}
	
	if ((obj = get_obj_here (ch, NULL,arg1)) == NULL)
	{
		send_to_char("No such object exists!\n\r",ch);
		return;
	}
	
	if (!str_prefix(arg2,"hp"))
		enchant_type=APPLY_HIT;
	else if (!str_prefix(arg2,"str"))
		enchant_type=APPLY_STR;
        else if (!str_prefix(arg3, "swim"))
               {
               send_to_char("That apply is not possible!\n\r", ch);
               return; 
               }
	else if (!str_prefix(arg2,"dex"))
		enchant_type=APPLY_DEX;
	else if (!str_prefix(arg2,"int"))
		enchant_type=APPLY_INT;
	else if (!str_prefix(arg2,"wis"))
		enchant_type=APPLY_WIS;
	else if (!str_prefix(arg2,"con"))
		enchant_type=APPLY_CON;
	else if (!str_prefix(arg2,"mana"))
		enchant_type=APPLY_MANA;
	else if (!str_prefix(arg2,"move"))
		enchant_type=APPLY_MOVE;
	else if (!str_prefix(arg2,"ac"))
		enchant_type=APPLY_AC;
	else if (!str_prefix(arg2,"hitroll"))
		enchant_type=APPLY_HITROLL;
	else if (!str_prefix(arg2,"damroll"))
		enchant_type=APPLY_DAMROLL;
	else if (!str_prefix(arg2,"saves"))
		enchant_type=APPLY_SAVING_SPELL;
	else if (!str_prefix(arg2,"affect"))
             {  
		enchant_type=APPLY_SPELL_AFFECT;
                found = FALSE;
             }
	else
	{
		send_to_char("That apply is not possible!\n\r",ch);
		return;
	}
	
	if (IS_SPELL_AFFECT(enchant_type))
	{
		for (pos = 0; affect_flags[pos].name != NULL; pos++)
			if (!str_prefix(arg3, affect_flags[pos].name))
                        {
                   	   bit = affect_flags[pos].bit;
                           found = TRUE;
                        } 
	}
	else
	{
		if ( is_number(arg3) )
			affect_modify=atoi(arg3);  
		else
		{
			send_to_char("Applies require a value.\n\r", ch);
			return;
		}
	}
		
        if (found && str_cmp(arg3, "regeneration") && str_cmp(arg3, "dark_vision")) 
	  affect_enchant(obj);
        else
        {
          send_to_char("Please pick from the list.\n\r", ch);
          return;
        }

        /* create the affect */
    	paf.where	= TO_AFFECTS;
    	paf.type	= 0;
	paf.level	= ch->level;
	paf.duration	= -1;
	paf.location	= enchant_type;
	paf.modifier	= affect_modify;
	paf.bitvector	= bit;
	
	if ( IS_SPELL_AFFECT(enchant_type))
	{
		/* make table work with skill_lookup */		
        	for ( i=0 ; arg3[i] != '\0'; i++ )
        	{
            		if ( arg3[i] == '_' )
                		arg3[i] = ' ';
        	}
	        paf.type = skill_lookup(arg3);		
	}

/* check for bugs        sprintf(buf, "Arg3 %s paf type %ld", arg3, skill_lookup(arg3));
        send_to_char(buf, ch); */
	affect_to_obj(obj,&paf);

	send_to_char("Ok.\n\r", ch);
} 

void do_unenchant(CHAR_DATA *ch, char *argument)
{
	OBJ_DATA *obj;
	AFFECT_DATA *paf, *paf_next;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
        char arg3[MAX_INPUT_LENGTH];
	int bit = 0, enchant_type = -1, pos;
	bool found = TRUE;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
        argument = one_argument( argument, arg3 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char("Syntax for applies: unenchant <object> <apply type>\n\r",ch);
		send_to_char("Apply Types: hp str dex int wis con mana\n\r", ch);
		send_to_char("             ac move hitroll damroll saves\n\r\n\r", ch);
		send_to_char("Syntax for affects: unenchant <object> affect <affect name>\n\r",ch);
		send_to_char("Affect Names: blind invisible detect_evil detect_invis detect_magic\n\r",ch);
		send_to_char("              detect_hidden detect_good sanctuary faerie_fire infrared\n\r",ch);
		send_to_char("              curse poison protect_evil protect_good sneak hide sleep charm\n\r", ch);
		send_to_char("              flying pass_door haste calm plague weaken dark_vision berserk slow\n\r", ch);
		send_to_char("              armor shield stoneskin bless frenzy\n\r", ch);
                send_to_char("Other options: all - removes everything\n\r", ch);	
                send_to_char("               extra - gets rid of glow/hum etc.\n\r", ch); 
                return;
	}
	
	if ((obj = get_obj_here (ch, NULL,arg1)) == NULL)
	{
		send_to_char("No such object exists!\n\r",ch);
		return;
	}
	
	if (!str_prefix(arg2,"hp"))
		enchant_type=APPLY_HIT;
	else if (!str_prefix(arg2,"str"))
		enchant_type=APPLY_STR;
	else if (!str_prefix(arg2,"dex"))
		enchant_type=APPLY_DEX;
	else if (!str_prefix(arg2,"int"))
		enchant_type=APPLY_INT;
	else if (!str_prefix(arg2,"wis"))
		enchant_type=APPLY_WIS;
	else if (!str_prefix(arg2,"con"))
		enchant_type=APPLY_CON;
	else if (!str_prefix(arg2,"mana"))
		enchant_type=APPLY_MANA;
	else if (!str_prefix(arg2,"move"))
		enchant_type=APPLY_MOVE;
	else if (!str_prefix(arg2,"ac"))
		enchant_type=APPLY_AC;
	else if (!str_prefix(arg2,"hitroll"))
		enchant_type=APPLY_HITROLL;
	else if (!str_prefix(arg2,"damroll"))
		enchant_type=APPLY_DAMROLL;
	else if (!str_prefix(arg2,"saves"))
		enchant_type=APPLY_SAVING_SPELL;
	else if (!str_prefix(arg2,"affect"))
             {  
		enchant_type=APPLY_SPELL_AFFECT;
                found = FALSE;
             }
        else if (!str_prefix(arg2,"all"))
        {
          for (paf = obj->affected;paf != NULL; paf = paf_next)
          {
             paf_next = paf->next;
             if (paf->location == enchant_type)
               free_affect(paf);
          }              
          obj->affected = NULL;
          obj->extra_flags = 0;
          return; 
        }
        else if (!str_prefix(arg2,"extra"))
        {
           obj->extra_flags = 0;
           return;
        }      
        else
	{
		send_to_char("That apply is not possible!\n\r",ch);
		return;
	}
	
        
	if (IS_SPELL_AFFECT(enchant_type))
	{
		for (pos = 0; affect_flags[pos].name != NULL; pos++)
			if (!str_cmp(affect_flags[pos].name,arg3))
                        {
                   	   bit = affect_flags[pos].bit;
                           found = TRUE;
                        } 
	}
		
        if (!found)
        {
          send_to_char("Pick a valid affect dildo.\n\r", ch);
          return;
        }

        for (paf = obj->affected;paf != NULL; paf = paf_next)
        {
           paf_next = paf->next;
           if (paf->location == enchant_type)
             affect_remove_obj(obj, paf);
        }              

/*	if ( IS_SPELL_AFFECT(enchant_type))
	{
		* make table work with skill_lookup *		
        	for ( i=0 ; arg3[i] != '\0'; i++ )
        	{
            		if ( arg3[i] == '_' )
                		arg3[i] = ' ';
        	}
				
        	if ((paf.type =  skill_lookup(arg3)) == NULL)
                {
                  send_to_char("Dildo\n\r", ch);
                  return;
                }
	}
*/

	send_to_char("Ok.\n\r", ch);
} 

 
/* RT nochannels command, for those spammers */
void do_nochannels (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Nochannel whom?", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (get_trust (victim) >= 78)
    {
        send_to_char ("You failed.\n\r", ch);
        return;
    }

    if (IS_SET (victim->comm, COMM_NOCHANNELS))
    {
        REMOVE_BIT (victim->comm, COMM_NOCHANNELS);
        send_to_char ("The gods have restored your channel priviliges.\n\r",
                      victim);
        send_to_char ("NOCHANNELS removed.\n\r", ch);
        sprintf (buf, "$N restores channels to %s", victim->name);
        wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else
    {
        SET_BIT (victim->comm, COMM_NOCHANNELS);
        send_to_char ("The gods have revoked your channel priviliges.\n\r",
                      victim);
        send_to_char ("NOCHANNELS set.\n\r", ch);
        sprintf (buf, "$N revokes %s's channels.", victim->name);
        wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
}


void do_smote (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *vch;
    char *letter, *name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if (!IS_NPC (ch) && IS_SET (ch->comm, COMM_NOEMOTE))
    {
        send_to_char ("You can't show your emotions.\n\r", ch);
        return;
    }

    if (argument[0] == '\0')
    {
        send_to_char ("Emote what?\n\r", ch);
        return;
    }

    if (strstr (argument, ch->name) == NULL)
    {
        send_to_char ("You must include your name in an smote.\n\r", ch);
        return;
    }

    send_to_char (argument, ch);
    send_to_char ("\n\r", ch);

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
        if (vch->desc == NULL || vch == ch)
            continue;

        if ((letter = strstr (argument, vch->name)) == NULL)
        {
            send_to_char (argument, vch);
            send_to_char ("\n\r", vch);
            continue;
        }

        strcpy (temp, argument);
        temp[strlen (argument) - int(strlen (letter))] = '\0';
        last[0] = '\0';
        name = vch->name;

        for (; *letter != '\0'; letter++)
        {
            if (*letter == '\'' && matches == int(strlen (vch->name)))
            {
                strcat (temp, "r");
                continue;
            }

            if (*letter == 's' && matches == int(strlen (vch->name)))
            {
                matches = 0;
                continue;
            }

            if (matches == int(strlen (vch->name)))
            {
                matches = 0;
            }

            if (*letter == *name)
            {
                matches++;
                name++;
                if (matches == int(strlen (vch->name)))
                {
                    strcat (temp, "you");
                    last[0] = '\0';
                    name = vch->name;
                    continue;
                }
                strncat (last, letter, 1);
                continue;
            }

            matches = 0;
            strcat (temp, last);
            strncat (temp, letter, 1);
            last[0] = '\0';
            name = vch->name;
        }

        send_to_char (temp, vch);
        send_to_char ("\n\r", vch);
    }

    return;
}

void do_bamfin (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char tempbuf[MSL];

    if (!IS_NPC (ch))
    {
        smash_tilde (argument);

        if (argument[0] == '\0')
        {
            sprintf (buf, "Your poofin is %s\n\r", ch->pcdata->bamfin);
            send_to_char (buf, ch);
            return;
        }

        colourstrip(tempbuf, argument);
        if (strstr (tempbuf, ch->name) == NULL)
        {
            send_to_char ("You must include your name.\n\r", ch);
            return;
        }

        free_string (ch->pcdata->bamfin);
        ch->pcdata->bamfin = str_dup (argument);

        sprintf (buf, "Your poofin is now %s\n\r", ch->pcdata->bamfin);
        send_to_char (buf, ch);
    }
    return;
}

void do_bamfout (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char tempbuf[MSL];

    if (!IS_NPC (ch))
    {
        smash_tilde (argument);

        if (argument[0] == '\0')
        {
            sprintf (buf, "Your poofout is %s\n\r", ch->pcdata->bamfout);
            send_to_char (buf, ch);
            return;
        }

        colourstrip(tempbuf, argument);
        if (strstr (tempbuf, ch->name) == NULL)
        {
            send_to_char ("You must include your name.\n\r", ch);
            return;
        }

        free_string (ch->pcdata->bamfout);
        ch->pcdata->bamfout = str_dup (argument);

        sprintf (buf, "Your poofout is now %s\n\r", ch->pcdata->bamfout);
        send_to_char (buf, ch);
    }
    return;
}



void do_deny (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);
    if (arg[0] == '\0')
    {
        send_to_char ("Deny whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (IS_NPC (victim))
    {
        send_to_char ("Not on NPC's.\n\r", ch);
        return;
    }

    if (get_trust (victim) >= get_trust (ch))
    {
        send_to_char ("You failed.\n\r", ch);
        return;
    }

    SET_BIT (victim->act, PLR_DENY);
    send_to_char ("You are denied access!\n\r", victim);
    sprintf (buf, "$N denies access to %s", victim->name);
    wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    send_to_char ("OK.\n\r", ch);
    save_char_obj (victim);
    stop_fighting (victim, TRUE);
    do_function (victim, &do_quit, "");

    return;
}



void do_disconnect (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    one_argument (argument, arg);
    if (arg[0] == '\0')
    {
        send_to_char ("Disconnect whom?\n\r", ch);
        return;
    }

    if (is_number (arg))
    {
        int desc;

        desc = atoi (arg);
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            if (d->descriptor == desc)
            {
                close_socket (d);
                send_to_char ("Ok.\n\r", ch);
                return;
            }
        }
    }

    if ((victim = get_char_world (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (get_trust(ch) <= get_trust(victim))
    {
      send_to_char("Think again, buddy.\n\r", ch);
      return;
    }

    if (victim->desc == NULL)
    {
        act ("$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR);
        return;
    }

    for (d = descriptor_list; d != NULL; d = d->next)
    {
        if (d == victim->desc)
        {
            close_socket (d);
            send_to_char ("Ok.\n\r", ch);
            return;
        }
    }

    bug ("Do_disconnect: desc not found.", 0);
    send_to_char ("Descriptor not found!\n\r", ch);
    return;
}



void do_pardon (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char ("Syntax: pardon <character> <skills>.\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg1)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (IS_NPC (victim))
    {
        send_to_char ("Not on NPC's.\n\r", ch);
        return;
    }

    if (!str_cmp (arg2, "skills"))
    {
        if (IS_SET (victim->act2, PLR_NOSKILLS))
        {
          REMOVE_BIT(victim->act2, PLR_NOSKILLS);
          send_to_char("You have been pardoned of your guild crimes.\n\r", victim);
          send_to_char("They have been pardoned of their guild crimes.\n\r", ch);
          return;
        }
    }

    send_to_char ("Syntax: pardon <character> <skills>.\n\r", ch);
    return;
}

void do_echo (CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
        send_to_char ("Global echo what?\n\r", ch);
        return;
    }

    for (d = descriptor_list; d; d = d->next)
    {
        if (d->connected == CON_PLAYING)
        {
            if (get_trust (d->character) >= get_trust (ch))
              send_to_char ("", d->character);
            send_to_char (argument, d->character);
            send_to_char ("\n\r", d->character);
        }
    }

    return;
}

void echo (char *argument)
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
        return;
    }

    for (d = descriptor_list; d; d = d->next)
    {
        if (d->connected == CON_PLAYING)
        {
            send_to_char (argument, d->character);
            send_to_char ("\n\r", d->character);
        }
    }

    return;
}



void do_recho (CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
        send_to_char ("Local echo what?\n\r", ch);

        return;
    }

    for (d = descriptor_list; d; d = d->next)
    {
        if (d->connected == CON_PLAYING
            && d->character->in_room == ch->in_room)
        {
            if (get_trust (d->character) >= get_trust (ch))
                send_to_char ("local> ", d->character);
            send_to_char (argument, d->character);
            send_to_char ("\n\r", d->character);
        }
    }

    return;
}

void do_zecho (CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
        send_to_char ("Zone echo what?\n\r", ch);
        return;
    }

    for (d = descriptor_list; d; d = d->next)
    {
        if (d->connected == CON_PLAYING
            && d->character->in_room != NULL && ch->in_room != NULL
            && d->character->in_room->area == ch->in_room->area)
        {
            if (get_trust (d->character) >= get_trust (ch))
                send_to_char ("zone> ", d->character);
            send_to_char (argument, d->character);
            send_to_char ("\n\r", d->character);
        }
    }
}

void do_pecho (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument (argument, arg);

    if (argument[0] == '\0' || arg[0] == '\0')
    {
        send_to_char ("Personal echo what?\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg)) == NULL)
    {
        send_to_char ("Target not found.\n\r", ch);
        return;
    }

    if (get_trust (victim) >= get_trust (ch) && get_trust (ch) != MAX_LEVEL)
        send_to_char ("personal> ", victim);

    send_to_char (argument, victim);
    send_to_char ("\n\r", victim);
    send_to_char ("personal> ", ch);
    send_to_char (argument, ch);
    send_to_char ("\n\r", ch);
}


ROOM_INDEX_DATA *find_location (CHAR_DATA * ch, char *arg)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if (is_number (arg))
        return get_room_index (atoi (arg));

    if ((victim = get_char_world (ch, arg)) != NULL)
        return victim->in_room;

    if ((obj = get_obj_world (ch, arg)) != NULL)
        return obj->in_room;

    return NULL;
}

ROOM_INDEX_DATA *find_location_vnum (CHAR_DATA * ch, int vnum)
{
    CHAR_DATA *victim;

    if ((victim = get_char_world_vnum (ch, vnum)) != NULL)
        return victim->in_room;

    return NULL;
}



void do_transfer (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (arg1[0] == '\0')
    {
        send_to_char ("Transfer whom (and where)?\n\r", ch);
        return;
    }

    if (!str_cmp (arg1, "all"))
    {
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            if (d->connected == CON_PLAYING
                && d->character != ch
                && d->character->in_room != NULL
                && can_see (ch, d->character))
            {
                char buf[MAX_STRING_LENGTH];
                sprintf (buf, "%s %s", d->character->name, arg2);
                do_function (ch, &do_transfer, buf);
            }
        }
        return;
    }

    if (!str_cmp (arg1, "room"))
    {
      for (d = descriptor_list; d != NULL; d = d->next)
      {
            if (d->connected == CON_PLAYING
                && d->character != ch
                && d->character->in_room == ch->in_room 
                && can_see (ch, d->character))
            {
                char buf[MAX_STRING_LENGTH];
                sprintf (buf, "%s %s", d->character->name, arg2);
                do_function (ch, &do_transfer, buf);
            }
        }
        return;
    } 
       

    if (!str_cmp (arg1, "quest"))
    {
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            if (d->connected == CON_PLAYING
                && d->character != ch
                && d->character->in_room != NULL
                && can_see (ch, d->character)
                && IS_SET(d->character->act, PLR_QUESTING))
            {
                char buf[MAX_STRING_LENGTH];
                sprintf (buf, "%s %s", d->character->name, arg2);
                do_function (ch, &do_transfer, buf);
            }
        }
        return;
    } 
     

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if (arg2[0] == '\0')
    {
        location = ch->in_room;
    }
    else
    {
        if ((location = find_location (ch, arg2)) == NULL)
        {
            send_to_char ("No such location.\n\r", ch);
            return;
        }

        if (!is_room_owner (ch, location) && room_is_private (location)
            && get_trust (ch) < MAX_LEVEL)
        {
            send_to_char ("That room is private right now.\n\r", ch);
            return;
        }
    }

    if ((victim = get_char_world (ch, arg1)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim->in_room == NULL)
    {
        send_to_char ("They are in limbo.\n\r", ch);
        return;
    }

    if (victim->fighting != NULL)
        stop_fighting (victim, TRUE);
    act ("$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM);
    char_from_room (victim);
    char_to_room (victim, location);
    act ("$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM);
    if (ch != victim)
        act ("$n has transferred you.", ch, NULL, victim, TO_VICT);
    do_function (victim, &do_look, "auto");
    send_to_char ("Ok.\n\r", ch);
}



void do_at (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    OBJ_DATA *on;
    CHAR_DATA *wch;

    argument = one_argument (argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0')
    {
        send_to_char ("At where what?\n\r", ch);
        return;
    }

    if ((location = find_location (ch, arg)) == NULL)
    {
        send_to_char ("No such location.\n\r", ch);
        return;
    }

    if (!is_room_owner (ch, location) && room_is_private (location)
        && get_trust (ch) < MAX_LEVEL)
    {
        send_to_char ("That room is private right now.\n\r", ch);
        return;
    }

    original = ch->in_room;
    on = ch->on;
    char_from_room (ch);
    char_to_room (ch, location);
    interpret (ch, argument);

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for (wch = char_list; wch != NULL; wch = wch->next)
    {
        if (wch == ch)
        {
            char_from_room (ch);
            char_to_room (ch, original);
            ch->on = on;
            break;
        }
    }

    return;
}



void do_goto (CHAR_DATA * ch, char *argument)
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;
    int count = 0;

    if (argument[0] == '\0')
    {
        send_to_char ("Goto where?\n\r", ch);
        return;
    }

    if ((location = find_location (ch, argument)) == NULL)
    {
        send_to_char ("No such location.\n\r", ch);
        return;
    }

    count = 0;
    for (rch = location->people; rch != NULL; rch = rch->next_in_room)
        count++;

    if (!is_room_owner (ch, location) && room_is_private (location)
        && (count > 1 || get_trust (ch) < MAX_LEVEL))
    {
        send_to_char ("That room is private right now.\n\r", ch);
        return;
    }

    if (ch->fighting != NULL)
        stop_fighting (ch, TRUE);

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if (get_trust (rch) >= ch->invis_level)
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
                act ("$t", ch, ch->pcdata->bamfout, rch, TO_VICT);
            else
                act ("$n leaves in a swirling mist.", ch, NULL, rch, TO_VICT);
        }
    }

    char_from_room (ch);
    char_to_room (ch, location);

    if (ch->is_mounted && ch->mount)
    {
      char_from_room (ch->mount);
      char_to_room (ch->mount, location);
    }

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if (get_trust (rch) >= ch->invis_level)
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
                act ("$t", ch, ch->pcdata->bamfin, rch, TO_VICT);
            else
                act ("$n appears in a swirling mist.", ch, NULL, rch,
                     TO_VICT);
        }
    }

    do_function (ch, &do_look, "auto");
    return;
}

void do_violate (CHAR_DATA * ch, char *argument)
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;

    if (argument[0] == '\0')
    {
        send_to_char ("Goto where?\n\r", ch);
        return;
    }

    if ((location = find_location (ch, argument)) == NULL)
    {
        send_to_char ("No such location.\n\r", ch);
        return;
    }

    if (!room_is_private (location))
    {
        send_to_char ("That room isn't private, use goto.\n\r", ch);
        return;
    }

    if (ch->fighting != NULL)
        stop_fighting (ch, TRUE);

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if (get_trust (rch) >= ch->invis_level)
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
                act ("$t", ch, ch->pcdata->bamfout, rch, TO_VICT);
            else
                act ("$n leaves in a swirling mist.", ch, NULL, rch, TO_VICT);
        }
    }

    char_from_room (ch);
    char_to_room (ch, location);


    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if (get_trust (rch) >= ch->invis_level)
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
                act ("$t", ch, ch->pcdata->bamfin, rch, TO_VICT);
            else
                act ("$n appears in a swirling mist.", ch, NULL, rch,
                     TO_VICT);
        }
    }

    do_function (ch, &do_look, "auto");
    return;
}

/* RT to replace the 3 stat commands */

void do_stat (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char *string;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim;

    string = one_argument (argument, arg);
    if (arg[0] == '\0')
    {
        send_to_char ("Syntax:\n\r", ch);
        send_to_char ("  stat <name>\n\r", ch);
        send_to_char ("  stat obj <name>\n\r", ch);
        send_to_char ("  stat mob <name>\n\r", ch);
        send_to_char ("  stat room <number>\n\r", ch);
        return;
    }

    if (!str_cmp (arg, "room"))
    {
        do_function (ch, &do_rstat, string);
        return;
    }

    if (!str_cmp (arg, "obj"))
    {
        do_function (ch, &do_ostat, string);
        return;
    }

    if (!str_cmp (arg, "char") || !str_cmp (arg, "mob"))
    {
        do_function (ch, &do_mstat, string);
        return;
    }

    /* do it the old way */

    obj = get_obj_world (ch, argument);
    if (obj != NULL)
    {
        do_function (ch, &do_ostat, argument);
        return;
    }

    victim = get_char_world (ch, argument);
    if (victim != NULL)
    {
        do_function (ch, &do_mstat, argument);
        return;
    }

    location = find_location (ch, argument);
    if (location != NULL)
    {
        do_function (ch, &do_rstat, argument);
        return;
    }

    send_to_char ("Nothing by that name found anywhere.\n\r", ch);
}

void do_rstat (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj;
    CHAR_DATA *rch;
    int door;

    one_argument (argument, arg);
    location = (arg[0] == '\0') ? ch->in_room : find_location (ch, arg);
    if (location == NULL)
    {
        send_to_char ("No such location.\n\r", ch);
        return;
    }

    if (!is_room_owner (ch, location) && ch->in_room != location
        && room_is_private (location) && !IS_TRUSTED (ch, IMPLEMENTOR))
    {
        send_to_char ("That room is private right now.\n\r", ch);
        return;
    }

    sprintf (buf, "Name: '%s'\n\rArea: '%s'\n\r",
             location->name, location->area->name);
    send_to_char (buf, ch);

    sprintf (buf,
             "Vnum: %ld  Sector: %d  Light: %d  Healing: %d  Mana: %d\n\r",
             location->vnum,
             location->sector_type,
             location->light, location->heal_rate, location->mana_rate);
    send_to_char (buf, ch);

    sprintf (buf,
             "Room flags: %d.\n\rDescription:\n\r%s",
             location->room_flags, location->description);
    send_to_char (buf, ch);

    if (location->extra_descr != NULL)
    {
        EXTRA_DESCR_DATA *ed;

        send_to_char ("Extra description keywords: '", ch);
        for (ed = location->extra_descr; ed; ed = ed->next)
        {
            send_to_char (ed->keyword, ch);
            if (ed->next != NULL)
                send_to_char (" ", ch);
        }
        send_to_char ("'.\n\r", ch);
    }

    send_to_char ("Characters:", ch);
    for (rch = location->people; rch; rch = rch->next_in_room)
    {
        if (can_see (ch, rch))
        {
            send_to_char (" ", ch);
            one_argument (rch->name, buf);
            send_to_char (buf, ch);
        }
    }

    send_to_char (".\n\rObjects:   ", ch);
    for (obj = location->contents; obj; obj = obj->next_content)
    {
        send_to_char (" ", ch);
        one_argument (obj->name, buf);
        send_to_char (buf, ch);
    }
    send_to_char (".\n\r", ch);

    for (door = 0; door <= 5; door++)
    {
        EXIT_DATA *pexit;

        if ((pexit = location->exit[door]) != NULL)
        {
            sprintf (buf,
                     "Door: %d.  To: %ld.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",
                     door,
                     (pexit->u1.to_room ==
                      NULL ? -1 : pexit->u1.to_room->vnum), pexit->key,
                     pexit->exit_info, pexit->keyword,
                     pexit->description[0] !=
                     '\0' ? pexit->description : "(none).\n\r");
            send_to_char (buf, ch);
        }
    }

    return;
}



void do_ostat (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Stat what?\n\r", ch);
        return;
    }

    if ((obj = get_obj_world (ch, argument)) == NULL)
    {
        send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
        return;
    }

    sprintf (buf, "Name(s): %s\n\r", obj->name);
    send_to_char (buf, ch);

    sprintf (buf, "Vnum: %ld  Format: %s  Type: %s  Resets: %d\n\r",
             obj->pIndexData->vnum,
             obj->pIndexData->new_format ? "new" : "old",
             item_name (obj->item_type), obj->pIndexData->reset_num);
    send_to_char (buf, ch);
    if (obj->id)
    {
      sprintf (buf, "ID: %ld\n\r", obj->id);
      send_to_char ( buf, ch);
    }
    if (obj->enchants);
    {
      sprintf( buf, "Enchants: %d\n\r", obj->enchants);
      send_to_char( buf, ch);
    }
    sprintf (buf, "Short description: %s\n\rLong description: %s\n\r",
             obj->short_descr, obj->description);
    send_to_char (buf, ch);

    sprintf (buf, "Wear bits: %s\n\rExtra bits: %s\n\r",
             wear_bit_name (obj->wear_flags),
             extra_bit_name (obj->extra_flags));
    send_to_char (buf, ch);

    sprintf (buf, "Class bits: %s\n\r",
             class_bit_name (obj->class_flags));
    send_to_char (buf, ch);

    sprintf (buf, "Number: %d/%d  Weight: %d/%d/%d (10th pounds)\n\r",
             1, get_obj_number (obj),
             obj->weight, get_obj_weight (obj), get_true_weight (obj));
    send_to_char (buf, ch);

    sprintf (buf, "Level: %d  Cost: %d  Condition: %d  Timer: %d\n\r",
             obj->level, obj->cost, obj->condition, obj->timer);
    send_to_char (buf, ch);

    sprintf (buf,
             "In room: %ld  In object: %s  Carried by: %s  Wear_loc: %d\n\r",
             obj->in_room == NULL ? 0 : obj->in_room->vnum,
             obj->in_obj == NULL ? "(none)" : obj->in_obj->short_descr,
             obj->carried_by == NULL ? "(none)" :
             can_see (ch, obj->carried_by) ? obj->carried_by->name
             : "someone", obj->wear_loc);
    send_to_char (buf, ch);

    sprintf (buf, "Values: %d %d %d %d %d\n\r",
             obj->value[0], obj->value[1], obj->value[2], obj->value[3],
             obj->value[4]);
    send_to_char (buf, ch);

    /* now give out vital statistics as per identify */

    switch (obj->item_type)
    {
        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
            sprintf (buf, "Level %d spells of:", obj->value[0]);
            send_to_char (buf, ch);

            if (obj->value[1] >= 0 && obj->value[1] < MAX_SKILL)
            {
                send_to_char (" '", ch);
                send_to_char (skill_table[obj->value[1]].name, ch);
                send_to_char ("'", ch);
            }

            if (obj->value[2] >= 0 && obj->value[2] < MAX_SKILL)
            {
                send_to_char (" '", ch);
                send_to_char (skill_table[obj->value[2]].name, ch);
                send_to_char ("'", ch);
            }

            if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
            {
                send_to_char (" '", ch);
                send_to_char (skill_table[obj->value[3]].name, ch);
                send_to_char ("'", ch);
            }

            if (obj->value[4] >= 0 && obj->value[4] < MAX_SKILL)
            {
                send_to_char (" '", ch);
                send_to_char (skill_table[obj->value[4]].name, ch);
                send_to_char ("'", ch);
            }

            send_to_char (".\n\r", ch);
            break;

        case ITEM_WAND:
        case ITEM_STAFF:
            sprintf (buf, "Has %d(%d) charges of level %d",
                     obj->value[1], obj->value[2], obj->value[0]);
            send_to_char (buf, ch);

            if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
            {
                send_to_char (" '", ch);
                send_to_char (skill_table[obj->value[3]].name, ch);
                send_to_char ("'", ch);
            }

            send_to_char (".\n\r", ch);
            break;

        case ITEM_DRINK_CON:
            sprintf (buf, "It holds %s-colored %s.\n\r",
                     liq_table[obj->value[2]].liq_color,
                     liq_table[obj->value[2]].liq_name);
            send_to_char (buf, ch);
            break;


        case ITEM_WEAPON:
            send_to_char ("Weapon type is ", ch);
            switch (obj->value[0])
            {
                case (WEAPON_EXOTIC):
                    send_to_char ("exotic\n\r", ch);
                    break;
                case (WEAPON_SWORD):
                    send_to_char ("sword\n\r", ch);
                    break;
                case (WEAPON_DAGGER):
                    send_to_char ("dagger\n\r", ch);
                    break;
                case (WEAPON_SPEAR):
                    send_to_char ("spear/staff\n\r", ch);
                    break;
                case (WEAPON_MACE):
                    send_to_char ("mace/club\n\r", ch);
                    break;
                case (WEAPON_AXE):
                    send_to_char ("axe\n\r", ch);
                    break;
                case (WEAPON_FLAIL):
                    send_to_char ("flail\n\r", ch);
                    break;
                case (WEAPON_WHIP):
                    send_to_char ("whip\n\r", ch);
                    break;
                case (WEAPON_POLEARM):
                    send_to_char ("polearm\n\r", ch);
                    break;
                case (WEAPON_BOW):
                    send_to_char ("bow\n\r",ch);
                    break;
                default:
                    send_to_char ("unknown\n\r", ch);
                    break;
            }
            if (obj->pIndexData->new_format)
                sprintf (buf, "Damage is %dd%d (average %d)\n\r",
                         obj->value[1], obj->value[2],
                         (1 + obj->value[2]) * obj->value[1] / 2);
            else
                sprintf (buf, "Damage is %d to %d (average %d)\n\r",
                         obj->value[1], obj->value[2],
                         (obj->value[1] + obj->value[2]) / 2);
            send_to_char (buf, ch);

            sprintf (buf, "Damage noun is %s.\n\r",
                     (obj->value[3] > 0
                      && obj->value[3] <
                      MAX_DAMAGE_MESSAGE) ? attack_table[obj->value[3]].noun :
                     "undefined");
            send_to_char (buf, ch);

            if (obj->value[4])
            {                    /* weapon flags */
                sprintf (buf, "Weapons flags: %s\n\r",
                         weapon_bit_name (obj->value[4]));
                send_to_char (buf, ch);
            }
            break;

        case ITEM_ARMOR:
            sprintf (buf,
                     "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic\n\r",
                     obj->value[0], obj->value[1], obj->value[2],
                     obj->value[3]);
            send_to_char (buf, ch);
            break;
        case ITEM_ENVELOPE:
        case ITEM_CONTAINER:
            sprintf (buf, "Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
                     obj->value[0], obj->value[3],
                     cont_bit_name (obj->value[1]));
            send_to_char (buf, ch);
            if (obj->value[4] != 100)
            {
                sprintf (buf, "Weight multiplier: %d%%\n\r", obj->value[4]);
                send_to_char (buf, ch);
            }
            break;
        case ITEM_QUIVER:
            sprintf (buf, "It holds %d of a maximum of %d arrows.\n\r",obj->value[0],obj->value[1]);
            send_to_char(buf,ch);
            break;
        case ITEM_ARROW:
            sprintf (buf, "The arrow will do %dd%d damage for an average of %d\n\r",obj->value[1],obj->value[2], (obj->value[1]*obj->value[2])/2);
            send_to_char(buf,ch);
            break;
        case ITEM_PEN:
        case ITEM_WAX:
            sprintf (buf, "Used: %d  Maximum: %d\n\r",obj->value[0],obj->value[1]);
            send_to_char(buf,ch);
            break;
    }


    if (obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL)
    {
        EXTRA_DESCR_DATA *ed;

        send_to_char ("Extra description keywords: '", ch);

        for (ed = obj->extra_descr; ed != NULL; ed = ed->next)
        {
            send_to_char (ed->keyword, ch);
            if (ed->next != NULL)
                send_to_char (" ", ch);
        }

        for (ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next)
        {
            send_to_char (ed->keyword, ch);
            if (ed->next != NULL)
                send_to_char (" ", ch);
        }

        send_to_char ("'\n\r", ch);
    }

    for (paf = obj->affected; paf != NULL && obj->enchanted; paf = paf->next)
    {
        sprintf (buf, "Affects %s by %d, level %d (%s)",
                 affect_loc_name (paf->location), paf->modifier, paf->level,skill_table[paf->type].name);
        send_to_char (buf, ch);
        if (paf->duration > -1)
            sprintf (buf, ", %d hours.\n\r", paf->duration);
        else
            sprintf (buf, ".\n\r");
        send_to_char (buf, ch);
        if (paf->bitvector)
        {
            switch (paf->where)
            {
                case TO_AFFECTS:
                    sprintf (buf, "Adds %s affect.\n",
                             new_affect_bit_name (paf->bitvector));
                    break;
                case TO_WEAPON:
                    sprintf (buf, "Adds %s weapon flags.\n",
                             weapon_bit_name (paf->bitvector));
                    break;
                case TO_OBJECT:
                    sprintf (buf, "Adds %s object flag.\n",
                             extra_bit_name (paf->bitvector));
                    break;
                case TO_IMMUNE:
                    sprintf (buf, "Adds immunity to %s.\n",
                             imm_bit_name (paf->bitvector));
                    break;
                case TO_RESIST:
                    sprintf (buf, "Adds resistance to %s.\n\r",
                             imm_bit_name (paf->bitvector));
                    break;
                case TO_VULN:
                    sprintf (buf, "Adds vulnerability to %s.\n\r",
                             imm_bit_name (paf->bitvector));
                    break;
                default:
                    sprintf (buf, "Unknown bit %d: %d\n\r",
                             paf->where, paf->bitvector);
                    break;
            }
            send_to_char (buf, ch);
        }
    }

    if (!obj->enchanted)
        for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
        {
            sprintf (buf, "Affects %s by %d, level %d.\n\r",
                     affect_loc_name (paf->location), paf->modifier,
                     paf->level);
            send_to_char (buf, ch);
            if (paf->bitvector)
            {
                switch (paf->where)
                {
                    case TO_AFFECTS:
                        sprintf (buf, "Adds %s effect.\n",
                                 new_affect_bit_name (paf->bitvector));
                        break;
                    case TO_OBJECT:
                        sprintf (buf, "Adds %s object flag.\n",
                                 extra_bit_name (paf->bitvector));
                        break;
                    case TO_IMMUNE:
                        sprintf (buf, "Adds immunity to %s.\n",
                                 imm_bit_name (paf->bitvector));
                        break;
                    case TO_RESIST:
                        sprintf (buf, "Adds resistance to %s.\n\r",
                                 imm_bit_name (paf->bitvector));
                        break;
                    case TO_VULN:
                        sprintf (buf, "Adds vulnerability to %s.\n\r",
                                 imm_bit_name (paf->bitvector));
                        break;
                    default:
                        sprintf (buf, "Unknown bit %d: %d\n\r",
                                 paf->where, paf->bitvector);
                        break;
                }
                send_to_char (buf, ch);
            }
        }
    if (obj->owner)
    {
      sprintf(buf, "Owner: %s", obj->owner);
      send_to_char(buf, ch);
    }
    return;
}



void do_mstat (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    //char tmp[MSL];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    CHAR_DATA *victim;
    //MEM_DATA *remember;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Stat whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, argument)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    sprintf (buf, "Name: %s\n\r", IS_DISGUISED(victim)?victim->pcdata->disguise.orig_name:victim->name);
    send_to_char (buf, ch);

    sprintf (buf,
             "Vnum: %ld  Format: %s  Race: %s  Group: %d  Sex: %s  Room: %ld\n\r",
             IS_NPC (victim) ? victim->pIndexData->vnum : 0,
             IS_NPC (victim) ? victim->
             pIndexData->new_format ? "new" : "old" : "pc",
             race_table[victim->race].name,
             IS_NPC (victim) ? victim->group : 0, sex_table[victim->sex].name,
             victim->in_room == NULL ? 0 : victim->in_room->vnum);
    send_to_char (buf, ch);

    if (IS_NPC (victim))
    {
        sprintf (buf, "Count: %d  Killed: %d\n\r",
                 victim->pIndexData->count, victim->pIndexData->killed);
        send_to_char (buf, ch);
    }

    sprintf (buf,
             "Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
             pc_race_table[ch->race].stats[STAT_STR],
             get_curr_stat (victim, STAT_STR),
             pc_race_table[ch->race].stats[STAT_INT],
             get_curr_stat (victim, STAT_INT),
             pc_race_table[ch->race].stats[STAT_WIS],
             get_curr_stat (victim, STAT_WIS),
             pc_race_table[ch->race].stats[STAT_DEX],
             get_curr_stat (victim, STAT_DEX),
             pc_race_table[ch->race].stats[STAT_CON], get_curr_stat (victim, STAT_CON));
    send_to_char (buf, ch);

    sprintf (buf, "Hp: %ld/%ld Mana: %d/%d Move: %d/%d Trains: %d Practices: %d\n\r",
             victim->hit, victim->max_hit,
             victim->mana, victim->max_mana,
             victim->move, victim->max_move,
             IS_NPC (victim) ? 0 : victim->train,
             IS_NPC (victim) ? 0 : victim->practice);
    send_to_char (buf, ch);

    sprintf (buf,
             "Lv: %d  Class: %s  Align: %d  Gold: %ld  Silver: %ld  Exp: %d\n\r",
             victim->level,
             IS_NPC (victim) ? "mobile" : class_table[victim->cClass].name,
             victim->alignment, victim->gold, victim->silver, victim->exp);
    send_to_char (buf, ch);

    if (!IS_NPC(victim))
    {
       sprintf (buf,
             "RP Lv: %d  RP Exp: %d    Cps: %d   Xp Mult: %d Xp Time: %d Explored: %2.2f\n\r",
             victim->rplevel,
             victim->rpexp,
             IS_NPC(victim)?0:victim->pcdata->points,
             victim->pcdata->xpmultiplier.factor,
             victim->pcdata->xpmultiplier.time,
             ((double(roomcount(victim)) / top_room) * 100.0));
      send_to_char (buf, ch);
    }

    sprintf (buf, "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
             GET_AC (victim, AC_PIERCE), GET_AC (victim, AC_BASH),
             GET_AC (victim, AC_SLASH), GET_AC (victim, AC_EXOTIC));
    send_to_char (buf, ch);

    sprintf (buf,
             "Hit: %d  Dam: %d  Saves: %d  Size: %s  Position: %s  Wimpy: %d\n\r",
             GET_HITROLL (victim), GET_DAMROLL (victim), victim->saving_throw,
             size_table[victim->size].name,
             position_table[victim->position].name, victim->wimpy);
    send_to_char (buf, ch);

    if (IS_NPC (victim) && victim->pIndexData->new_format)
    {
        sprintf (buf, "Damage: %dd%d  Message:  %s\n\r",
                 victim->damage[DICE_NUMBER], victim->damage[DICE_TYPE],
                 attack_table[victim->dam_type].noun);
        send_to_char (buf, ch);
    }
    sprintf (buf, "Fighting: %s\n\r",
             victim->fighting ? victim->fighting->name : "(none)");
    send_to_char (buf, ch);

    if (!IS_NPC (victim))
    {
        sprintf (buf,
                 "Thirst: %d  Hunger: %d  Full: %d  Drunk: %d Horniness %d\n\r",
                 victim->pcdata->condition[COND_THIRST],
                 victim->pcdata->condition[COND_HUNGER],
                 victim->pcdata->condition[COND_FULL],
                 victim->pcdata->condition[COND_DRUNK],
		 victim->pcdata->condition[COND_HORNY]);
        send_to_char (buf, ch);
    }

    sprintf (buf, "Carry number: %d  Carry weight: %ld\n\r",
             victim->carry_number, get_carry_weight (victim) / 10);
    send_to_char (buf, ch);


    if (!IS_NPC (victim))
    {
        sprintf (buf,
                 "Age: %d  Played: %d  Last Level: %d  Timer: %d\n\r",
                 get_age (victim),
                 (int) (victim->played + current_time - victim->logon) / 3600,
                 victim->pcdata->last_level, victim->timer);
        send_to_char (buf, ch);
    }

    sprintf (buf, "Act: %s\n\r", act_bit_name (victim->act));
    send_to_char (buf, ch);
    sprintf (buf, "Act2: %s\n\r", act2_bit_name (victim->act2));
    send_to_char (buf, ch);

    if (victim->comm)
    {
        sprintf (buf, "Comm: %s\n\r", comm_bit_name (victim->comm));
        send_to_char (buf, ch);
    }

    if (IS_NPC (victim) && victim->off_flags)
    {
        sprintf (buf, "Offense: %s\n\r", off_bit_name (victim->off_flags));
        send_to_char (buf, ch);
    }

    if (victim->imm_flags)
    {
        sprintf (buf, "Immune: %s\n\r", imm_bit_name (victim->imm_flags));
        send_to_char (buf, ch);
    }

    if (victim->res_flags)
    {
        sprintf (buf, "Resist: %s\n\r", imm_bit_name (victim->res_flags));
        send_to_char (buf, ch);
    }

    if (victim->vuln_flags)
    {
        sprintf (buf, "Vulnerable: %s\n\r",
                 imm_bit_name (victim->vuln_flags));
        send_to_char (buf, ch);
    }

    sprintf (buf, "Form: %s\n\rParts: %s\n\r",
             form_bit_name (victim->form), part_bit_name (victim->parts));
    send_to_char (buf, ch);

    if (!STR_IS_ZERO(victim->affected_by, AFF_FLAGS))
    {
        sprintf (buf, "Affected by %s\n\r",
                 affect_str_bit_name (victim->affected_by));
        send_to_char (buf, ch);
    }

    sprintf (buf, "Master: %s  Leader: %s  Pet: %s\n\r",
             victim->master ? victim->master->name : "(none)",
             victim->leader ? victim->leader->name : "(none)",
             victim->pet ? victim->pet->name : "(none)");
    send_to_char (buf, ch);

    if (!IS_NPC (victim))
    {
        sprintf (buf, "Security: %d.\n\r", victim->pcdata->security);    /* OLC */
        send_to_char (buf, ch);    /* OLC */
    }

    sprintf (buf, "Short description: %s\n\rLong  description: %s",
             victim->short_descr,
             victim->long_descr[0] !=
             '\0' ? victim->long_descr : "(none)\n\r");
    send_to_char (buf, ch);

    sprintf (buf, "Murder: %i  Thief: %i  Jail: %i  Punishments: `!%i`*\n\r",
             victim->penalty.murder,
             victim->penalty.thief,
             victim->penalty.jail,
             victim->penalty.punishment);
    send_to_char(buf, ch);

    if (IS_NPC (victim) && victim->spec_fun != 0)
    {
        sprintf (buf, "Mobile has special procedure %s.\n\r",
                 spec_name (victim->spec_fun));
        send_to_char (buf, ch);
    }
	
    if ( IS_NPC(victim) && victim->hunting != NULL )
    {
        sprintf(buf, "Hunting victim: %s (%s)\n\r",
                IS_NPC(victim->hunting) ? victim->hunting->short_descr
                                        : victim->hunting->name,
                IS_NPC(victim->hunting) ? "MOB" : "PLAYER" );
        send_to_char(buf, ch);
    }
    /*if (IS_NPC(victim))
    {	
    	sprintf(buf, "Remembering target: ");
    	for (remember = victim->memory;remember != NULL; remember=remember->next)
    	{
    	    sprintf (tmp, "%s ",get_char_id(remember->id)->name);
    	    strcat (buf,tmp);
    	}
    	if ((remember = victim->memory) == NULL)
    	{
    	    sprintf (tmp, "None\n\r");
    	    strcat (buf,tmp);
    	}
    	else
    	{
            strcat(buf,"\n\r");
        }
        send_to_char(buf,ch);
    } */
    for (paf = victim->affected; paf != NULL; paf = paf->next)
    {
        sprintf (buf,
                 "Spell: '%s' modifies %s by %d for %d hours with bits %s, level %d.\n\r",
                 skill_table[(int) paf->type].name,
                 affect_loc_name (paf->location),
                 paf->modifier,
                 paf->duration, new_affect_bit_name (paf->bitvector), paf->bitvector);
        send_to_char (buf, ch);
    }
   
    if (IS_NPC(victim) && IS_SET(victim->act, ACT_MOUNT))
    {
      sprintf(buf, "Mount %s", victim->mount ? victim->mount->name : "none");
      send_to_char(buf, ch);
    }

    return;
}

/* ofind and mfind replaced with vnum, vnum skill also added */

void do_vnum (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char *string;

    string = one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Syntax:\n\r", ch);
        send_to_char ("  vnum obj <name>\n\r", ch);
        send_to_char ("  vnum mob <name>\n\r", ch);
        send_to_char ("  vnum skill <skill or spell>\n\r", ch);
        return;
    }

    if (!str_cmp (arg, "obj"))
    {
        do_function (ch, &do_ofind, string);
        return;
    }

    if (!str_cmp (arg, "mob") || !str_cmp (arg, "char"))
    {
        do_function (ch, &do_mfind, string);
        return;
    }

    if (!str_cmp (arg, "skill") || !str_cmp (arg, "spell"))
    {
        do_function (ch, &do_slookup, string);
        return;
    }
    /* do both */
    do_function (ch, &do_mfind, argument);
    do_function (ch, &do_ofind, argument);
}


void do_mfind (CHAR_DATA * ch, char *argument)
{
    extern long top_mob_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    long vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument (argument, arg);
    if (arg[0] == '\0')
    {
        send_to_char ("Find whom?\n\r", ch);
        return;
    }

    fAll = FALSE;                /* !str_cmp( arg, "all" ); */
    found = FALSE;
    nMatch = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for (vnum = 0; nMatch < top_mob_index; vnum++)
    {
        if ((pMobIndex = get_mob_index (vnum)) != NULL)
        {
            nMatch++;
            if (fAll || is_name (argument, pMobIndex->player_name))
            {
                found = TRUE;
                sprintf (buf, "[%5ld] %s\n\r",
                         pMobIndex->vnum, pMobIndex->short_descr);
                send_to_char (buf, ch);
            }
        }
    }

    if (!found)
        send_to_char ("No mobiles by that name.\n\r", ch);

    return;
}



void do_ofind (CHAR_DATA * ch, char *argument)
{
    extern long top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    long vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument (argument, arg);
    if (arg[0] == '\0')
    {
        send_to_char ("Find what?\n\r", ch);
        return;
    }

    fAll = FALSE;                /* !str_cmp( arg, "all" ); */
    found = FALSE;
    nMatch = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for (vnum = 0; nMatch < top_obj_index; vnum++)
    {
        if ((pObjIndex = get_obj_index (vnum)) != NULL)
        {
            nMatch++;
            if (fAll || is_name (argument, pObjIndex->name))
            {
                found = TRUE;
                sprintf (buf, "[%5ld] %s\n\r",
                         pObjIndex->vnum, pObjIndex->short_descr);
                send_to_char (buf, ch);
            }
        }
    }

    if (!found)
        send_to_char ("No objects by that name.\n\r", ch);

    return;
}


void do_owhere (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];
    BUFFER *buffer;
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;
    int number = 0, max_found;

    found = FALSE;
    number = 0;
    max_found = 200;

    buffer = new_buf ();

    if (argument[0] == '\0')
    {
        send_to_char ("Find what?\n\r", ch);
        return;
    }

    for (obj = object_list; obj != NULL; obj = obj->next)
    {
        if (!can_see_obj (ch, obj) || !is_name (argument, obj->name)
            || (ch->level < obj->level && ch->level != MAX_LEVEL &&
                !strstr(obj->pIndexData->area->builders, ch->name)))
            continue;

        found = TRUE;
        number++;

        for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj);

        if (in_obj->carried_by != NULL && can_see (ch, in_obj->carried_by)
            && in_obj->carried_by->in_room != NULL)
            sprintf (buf, "%3d) %s is carried by %s [Room %ld]\n\r",
                     number, obj->short_descr, PERS (in_obj->carried_by, ch, TRUE),
                     in_obj->carried_by->in_room->vnum);
        else if (in_obj->in_room != NULL
                 && can_see_room (ch, in_obj->in_room)) sprintf (buf,
                                                                 "%3d) %s is in %s [Room %ld]\n\r",
                                                                 number,
                                                                 obj->short_descr,
                                                                 in_obj->in_room->name,
                                                                 in_obj->in_room->vnum);
        else if (in_obj->in_store == NULL)
            sprintf (buf, "%3d) %s is somewhere\n\r", number,
                     obj->short_descr);
        else
            sprintf (buf, "%3d) %s is in %s's Store [Room %ld]\n\r", number,
                     obj->short_descr, in_obj->in_store->owner,
                     in_obj->in_store->vnum);

        buf[0] = UPPER (buf[0]);
        add_buf (buffer, buf);

        if (number >= max_found)
            break;
    }

    if (!found)
        send_to_char ("Nothing like that in heaven or earth.\n\r", ch);
    else
        page_to_char (buf_string (buffer), ch);

    free_buf (buffer);
}


void do_mwhere (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *buffer;
    CHAR_DATA *victim;
    bool found;
    int count = 0;

    if (argument[0] == '\0')
    {
        DESCRIPTOR_DATA *d;

        /* show characters logged */

        buffer = new_buf ();
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            if (d->character != NULL && d->connected == CON_PLAYING
                && d->character->in_room != NULL && can_see (ch, d->character)
                && can_see_room (ch, d->character->in_room))
            {
                victim = d->character;
                count++;
                if (d->original != NULL)
                    sprintf (buf,
                             "%3d) %s (in the body of %s) is in %s [%ld]\n\r",
                             count, d->original->name, victim->short_descr,
                             victim->in_room->name, victim->in_room->vnum);
                else
                    sprintf (buf, "%3d) %s is in %s [%ld]\n\r", count,
                             victim->name, victim->in_room->name,
                             victim->in_room->vnum);
                add_buf (buffer, buf);
            }
        }

        page_to_char (buf_string (buffer), ch);
        free_buf (buffer);
        return;
    }

    found = FALSE;
    buffer = new_buf ();
    for (victim = char_list; victim != NULL; victim = victim->next)
    {
        if (victim->in_room != NULL && is_name (argument, victim->name))
        {
            found = TRUE;
            count++;
            sprintf (buf, "%3d) [%5ld] %-28s [%5ld] %s\n\r", count,
                     IS_NPC (victim) ? victim->pIndexData->vnum : 0,
                     IS_NPC (victim) ? victim->short_descr : victim->name,
                     victim->in_room->vnum, victim->in_room->name);
            add_buf (buffer, buf);
        }
    }

    if (!found)
        act ("You didn't find any $T.", ch, NULL, argument, TO_CHAR);
    else
        page_to_char (buf_string (buffer), ch);

    free_buf (buffer);

    return;
}

void do_rwhere(CHAR_DATA *ch, char *argument)
{
  ROOM_INDEX_DATA *pRoom;
  AREA_DATA *pArea;
  int start, end;
  int i;
  char buf[MSL];
  long vnum;
  char arg[MIL];
  bool found = FALSE;
  bool foundRoom = FALSE;

  argument = one_argument(argument, arg);

  if (!is_number(arg) || argument[0] == '\0')
  {
     send_to_char("Synatax <area vnum> <search>\n\r", ch);
     return;
  }

  vnum = atol(arg);

  for (pArea = area_first; pArea != NULL; pArea = pArea->next)
  {
    if (pArea->vnum == vnum)
    {
      found = TRUE;
      break;
    }
  }

  if (!found)
  {
    send_to_char("No such area.\n\r", ch);
    return;
  }

  start = pArea->min_vnum;
  end = pArea->max_vnum;

  for ( i = start; i <= end; i++ )
  {
    if ( ( pRoom = get_room_index(i) ) == NULL )
      continue;

    if (strstr(pRoom->name, argument))
    {
      foundRoom = TRUE;
      sprintf(buf, "Found in room %d - %s.\n\r", i, pRoom->name );
      send_to_char(buf, ch);
    }
  }

  if (!foundRoom)
  {
    send_to_char("No room with that name.\n\r", ch);
    return;
  }

}    
   
	 	  

void do_reboo (CHAR_DATA * ch, char *argument)
{
    send_to_char ("If you want to REBOOT, spell it out.\n\r", ch);
    return;
}



void do_reboot (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;
    DESCRIPTOR_DATA *d, *d_next;
    CHAR_DATA *vch;

    if (ch->invis_level < LEVEL_HERO)
    {
        sprintf (buf, "Reboot by %s.", ch->name);
        do_function (ch, &do_echo, buf);
    }

    merc_down = TRUE;
    for (d = descriptor_list; d != NULL; d = d_next)
    {
        d_next = d->next;
        vch = d->original ? d->original : d->character;
        if (vch != NULL)
            save_char_obj (vch);
        close_socket (d);
    }

    return;
}

void do_shutdow (CHAR_DATA * ch, char *argument)
{
    send_to_char ("If you want to SHUTDOWN, spell it out.\n\r", ch);
    return;
}

void do_shutdown (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;
    DESCRIPTOR_DATA *d, *d_next;
    CHAR_DATA *vch;

    if (ch->invis_level < LEVEL_HERO)
        sprintf (buf, "Shutdown by %s.", ch->name);
    append_file (ch, SHUTDOWN_FILE, buf);
    strcat (buf, "\n\r");
    if (ch->invis_level < LEVEL_HERO)
    {
        do_function (ch, &do_echo, buf);
    }
    merc_down = TRUE;
    for (d = descriptor_list; d != NULL; d = d_next)
    {
        d_next = d->next;
        vch = d->original ? d->original : d->character;
        if (vch != NULL)
            save_char_obj (vch);
        close_socket (d);
    }
    return;
}

void do_protect (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;

    if (argument[0] == '\0')
    {
        send_to_char ("Protect whom from snooping?\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, argument)) == NULL)
    {
        send_to_char ("You can't find them.\n\r", ch);
        return;
    }

    if (IS_SET (victim->comm, COMM_SNOOP_PROOF))
    {
        act_new ("$N is no longer snoop-proof.", ch, NULL, victim, TO_CHAR,
                 POS_DEAD);
        send_to_char ("Your snoop-proofing was just removed.\n\r", victim);
        REMOVE_BIT (victim->comm, COMM_SNOOP_PROOF);
    }
    else
    {
        act_new ("$N is now snoop-proof.", ch, NULL, victim, TO_CHAR,
                 POS_DEAD);
        send_to_char ("You are now immune to snooping.\n\r", victim);
        SET_BIT (victim->comm, COMM_SNOOP_PROOF);
    }
}



void do_snoop (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Snoop whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim->desc == NULL)
    {
        send_to_char ("No descriptor to snoop.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("Cancelling all snoops.\n\r", ch);
        wiznet ("$N stops being such a snoop.",
                ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust (ch));
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            if (d->snoop_by == ch->desc)
                d->snoop_by = NULL;
        }
        return;
    }

    if (victim->desc->snoop_by != NULL)
    {
        send_to_char ("Busy already.\n\r", ch);
        return;
    }

    if (!is_room_owner (ch, victim->in_room) && ch->in_room != victim->in_room
        && room_is_private (victim->in_room) && !IS_TRUSTED (ch, IMPLEMENTOR))
    {
        send_to_char ("That character is in a private room.\n\r", ch);
        return;
    }

    if (get_trust (victim) >= get_trust (ch)
        || IS_SET (victim->comm, COMM_SNOOP_PROOF))
    {
        send_to_char ("You failed.\n\r", ch);
        return;
    }

    if (ch->desc != NULL)
    {
        for (d = ch->desc->snoop_by; d != NULL; d = d->snoop_by)
        {
            if (d->character == victim || d->original == victim)
            {
                send_to_char ("No snoop loops.\n\r", ch);
                return;
            }
        }
    }

    victim->desc->snoop_by = ch->desc;
    sprintf (buf, "$N starts snooping on %s",
             (IS_NPC (ch) ? victim->short_descr : victim->name));
    wiznet (buf, ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust (ch));
    send_to_char ("Ok.\n\r", ch);
    return;
}



void do_switch (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);
    
    if (ch->desc->editor != 0)
    {
    send_to_char("Not while in OLC", ch);
    return;
    }
    if (arg[0] == '\0')
    {
        send_to_char ("Switch into whom?\n\r", ch);
        return;
    }

    if (ch->desc == NULL)
        return;

    if (ch->desc->original != NULL)
    {
        send_to_char ("You are already switched.\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("Ok.\n\r", ch);
        return;
    }

    if (!IS_NPC (victim))
    {
        send_to_char ("You can only switch into mobiles.\n\r", ch);
        return;
    }

    if (!is_room_owner (ch, victim->in_room) && ch->in_room != victim->in_room
        && room_is_private (victim->in_room) && !IS_TRUSTED (ch, IMPLEMENTOR))
    {
        send_to_char ("That character is in a private room.\n\r", ch);
        return;
    }

    if (victim->desc != NULL)
    {
        send_to_char ("Character in use.\n\r", ch);
        return;
    }

    sprintf (buf, "$N switches into %s", victim->short_descr);
    wiznet (buf, ch, NULL, WIZ_SWITCHES, WIZ_SECURE, get_trust (ch));

    ch->desc->character = victim;
    ch->desc->original = ch;
    victim->desc = ch->desc;
    ch->desc = NULL;
    /* change communications to match */
    if (ch->prompt != NULL)
        victim->prompt = str_dup (ch->prompt);
    victim->comm = ch->comm;
    victim->lines = ch->lines;
    send_to_char ("Ok.\n\r", victim);
    return;
}



void do_return (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (ch->desc == NULL)
        return;

    if (ch->desc->original == NULL)
    {
        send_to_char ("You aren't switched.\n\r", ch);
        return;
    }

    send_to_char
        ("You return to your original body. Type replay to see any missed tells.\n\r",
         ch);
    if (ch->prompt != NULL)
    {
        free_string (ch->prompt);
        ch->prompt = NULL;
    }

    sprintf (buf, "$N returns from %s.", ch->short_descr);
    wiznet (buf, ch->desc->original, 0, WIZ_SWITCHES, WIZ_SECURE,
            get_trust (ch));
    ch->desc->character = ch->desc->original;
    ch->desc->original = NULL;
    ch->desc->character->desc = ch->desc;
    ch->desc = NULL;
    return;
}

/* trust levels for load and clone */
bool obj_check (CHAR_DATA * ch, OBJ_DATA * obj)
{
    if (IS_TRUSTED (ch, SUPREME)
        || (IS_TRUSTED (ch, BUILDER) && obj->level <= 20
            && obj->cost <= 1000) || (IS_TRUSTED (ch, SUPREME)
                                      && obj->level <= 10 && obj->cost <= 500)
        || (IS_TRUSTED (ch, CREATOR) && obj->level <= 5 && obj->cost <= 250)
        || (IS_TRUSTED (ch, SUPREME) && obj->level == 0 && obj->cost <= 100))
        return TRUE;
    else
        return FALSE;
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone (CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * clone)
{
    OBJ_DATA *c_obj, *t_obj;


    for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content)
    {
        if (obj_check (ch, c_obj))
        {
            t_obj = create_object (c_obj->pIndexData, 0);
            clone_object (c_obj, t_obj);
            obj_to_obj (t_obj, clone);
            recursive_clone (ch, c_obj, t_obj);
        }
    }
}

/* command that is similar to load */
void do_clone (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char *rest;
    CHAR_DATA *mob;
    OBJ_DATA *obj;

    rest = one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Clone what?\n\r", ch);
        return;
    }

    if (!str_prefix (arg, "object"))
    {
        mob = NULL;
        obj = get_obj_here (ch, NULL, rest);
        if (obj == NULL)
        {
            send_to_char ("You don't see that here.\n\r", ch);
            return;
        }
    }
    else if (!str_prefix (arg, "mobile") || !str_prefix (arg, "character"))
    {
        obj = NULL;
        mob = get_char_room (ch, NULL, rest);
        if (mob == NULL)
        {
            send_to_char ("You don't see that here.\n\r", ch);
            return;
        }
    }
    else
    {                            /* find both */

        mob = get_char_room (ch, NULL, argument);
        obj = get_obj_here (ch, NULL, argument);
        if (mob == NULL && obj == NULL)
        {
            send_to_char ("You don't see that here.\n\r", ch);
            return;
        }
    }

    /* clone an object */
    if (obj != NULL)
    {
        OBJ_DATA *clone;

        if (!obj_check (ch, obj))
        {
            send_to_char
                ("Your powers are not great enough for such a task.\n\r", ch);
            return;
        }

        clone = create_object (obj->pIndexData, 0);
        clone_object (obj, clone);
        if (obj->carried_by != NULL)
            obj_to_char (clone, ch);
        else
            obj_to_room (clone, ch->in_room);
        recursive_clone (ch, obj, clone);

        act ("$n has created $p.", ch, clone, NULL, TO_ROOM);
        act ("You clone $p.", ch, clone, NULL, TO_CHAR);
        wiznet ("$N clones $p.", ch, clone, WIZ_LOAD, WIZ_SECURE,
                get_trust (ch));
        return;
    }
    else if (mob != NULL)
    {
        CHAR_DATA *clone;
        OBJ_DATA *new_obj;
        char buf[MAX_STRING_LENGTH];

        if (!IS_NPC (mob))
        {
            send_to_char ("You can only clone mobiles.\n\r", ch);
            return;
        }

        if ((mob->level > 20 && !IS_TRUSTED (ch, CREATOR))
            || (mob->level > 10 && !IS_TRUSTED (ch, SUPREME))
            || (mob->level > 0 && !IS_TRUSTED (ch, BUILDER))
            || !IS_TRUSTED (ch, SUPREME))
        {
            send_to_char
                ("Your powers are not great enough for such a task.\n\r", ch);
            return;
        }

        clone = create_mobile (mob->pIndexData);
        clone_mobile (mob, clone);

        for (obj = mob->carrying; obj != NULL; obj = obj->next_content)
        {
            if (obj_check (ch, obj))
            {
                new_obj = create_object (obj->pIndexData, 0);
                clone_object (obj, new_obj);
                recursive_clone (ch, obj, new_obj);
                obj_to_char (new_obj, clone);
                new_obj->wear_loc = obj->wear_loc;
            }
        }
        char_to_room (clone, ch->in_room);
        act ("$n has created $N.", ch, NULL, clone, TO_ROOM);
        act ("You clone $N.", ch, NULL, clone, TO_CHAR);
        sprintf (buf, "$N clones %s.", clone->short_descr);
        wiznet (buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust (ch));
        return;
    }
}

/* RT to replace the two load commands */

void do_load (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Syntax:\n\r", ch);
        send_to_char ("  load mob <vnum>\n\r", ch);
        send_to_char ("  load obj <vnum> <level>\n\r", ch);
        return;
    }

    if (!str_cmp (arg, "mob") || !str_cmp (arg, "char"))
    {
        do_function (ch, &do_mload, argument);
        return;
    }

    if (!str_cmp (arg, "obj"))
    {
        do_function (ch, &do_oload, argument);
        return;
    }
    /* echo syntax */
    do_function (ch, &do_load, "");
}


void do_mload (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    one_argument (argument, arg);

    if (arg[0] == '\0' || !is_number (arg))
    {
        send_to_char ("Syntax: load mob <vnum>.\n\r", ch);
        return;
    }

    if ((pMobIndex = get_mob_index (atoi (arg))) == NULL)
    {
        send_to_char ("No mob has that vnum.\n\r", ch);
        return;
    }

    victim = create_mobile (pMobIndex);
    char_to_room (victim, ch->in_room);
    act ("$n has created $N!", ch, NULL, victim, TO_ROOM);
    sprintf (buf, "$N loads %s.", victim->short_descr);
    wiznet (buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust (ch));
    send_to_char ("Ok.\n\r", ch);
    return;
}



void do_oload (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    int level;

    argument = one_argument (argument, arg1);
    one_argument (argument, arg2);

    if (arg1[0] == '\0' || !is_number (arg1))
    {
        send_to_char ("Syntax: load obj <vnum> <level>.\n\r", ch);
        return;
    }

    level = get_trust (ch);        /* default */

    if (arg2[0] != '\0')
    {                            /* load with a level */
        if (!is_number (arg2))
        {
            send_to_char ("Syntax: oload <vnum> <level>.\n\r", ch);
            return;
        }
        level = atoi (arg2);
        if (level < 0 || level > get_trust (ch))
        {
            send_to_char ("Level must be be between 0 and your level.\n\r",
                          ch);
            return;
        }
    }

    if ((pObjIndex = get_obj_index (atoi (arg1))) == NULL)
    {
        send_to_char ("No object has that vnum.\n\r", ch);
        return;
    }

    obj = create_object (pObjIndex, level);

    if (get_trust(ch) < MAX_LEVEL)
      obj->timer = 7;

    if (CAN_WEAR (obj, ITEM_TAKE))
    
  
    {
        char log_buf[MSL];
        obj_to_char (obj, ch);
        wiznet ("$N loads $p.", ch, obj, WIZ_LOAD, WIZ_SECURE, get_trust (ch));
        sprintf(log_buf, "%s loaded %ld number %ld.", 
	     ch->name, obj->pIndexData->vnum, obj->id);
        log_special(log_buf, IMM_TYPE);
    }
    else
    {
        obj_to_room (obj, ch->in_room);
        wiznet ("$N loads $p.", ch, obj, WIZ_LOAD, WIZ_SECURE, get_trust (ch));
    }
    act ("$n has created $p!", ch, obj, NULL, TO_ROOM);
    if (ch->level != MAX_LEVEL)
    {
//    SET_BIT(obj->extra_flags, ITEM_LOADED);
    }
    send_to_char ("Ok.\n\r", ch);
    return;
}



void do_purge (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    DESCRIPTOR_DATA *d;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        /* 'purge' */
        CHAR_DATA *vnext;
        OBJ_DATA *obj_next;

        for (victim = ch->in_room->people; victim != NULL; victim = vnext)
        {
            vnext = victim->next_in_room;
            if (IS_NPC (victim) && !IS_SET (victim->act, ACT_NOPURGE)
                && victim != ch /* safety precaution */ )
                extract_char (victim, TRUE);
        }

        for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
        {
            obj_next = obj->next_content;
            if (!IS_OBJ_STAT (obj, ITEM_NOPURGE))
                extract_obj (obj);
        }

        act ("$n purges the room!", ch, NULL, NULL, TO_ROOM);
        send_to_char ("Ok.\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (!IS_NPC (victim))
    {

        if (ch == victim)
        {
            send_to_char ("Ho ho ho.\n\r", ch);
            return;
        }

        if (get_trust (ch) <= get_trust (victim))
        {
            send_to_char ("Maybe that wasn't a good idea...\n\r", ch);
            sprintf (buf, "%s tried to purge you!\n\r", ch->name);
            send_to_char (buf, victim);
            return;
        }

        act ("$n disintegrates $N.", ch, 0, victim, TO_NOTVICT);

        if (victim->level > 1)
            save_char_obj (victim);
        d = victim->desc;
        extract_char (victim, TRUE);
        if (d != NULL)
            close_socket (d);

        return;
    }

    act ("$n purges $N.", ch, NULL, victim, TO_NOTVICT);
    extract_char (victim, TRUE);
    return;
}



void do_advance (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;
    int iLevel;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0' || !is_number (arg2))
    {
        send_to_char ("Syntax: advance <char> <level>.\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg1)) == NULL)
    {
        send_to_char ("That player is not here.\n\r", ch);
        return;
    }

    if (IS_NPC (victim))
    {
        send_to_char ("Not on NPC's.\n\r", ch);
        return;
    }

    if ((level = atoi (arg2)) < 1 || level > MAX_LEVEL)
    {
        sprintf (buf, "Level must be 1 to %d.\n\r", MAX_LEVEL);
        send_to_char (buf, ch);
        return;
    }

    if (level > get_trust (ch))
    {
        send_to_char ("Limited to your trust level.\n\r", ch);
        return;
    }

    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     *   Added ability to lower current level but can also reset to level 1 - Camm
     */
    if (level == 1)
    {
    	send_to_char ("Lowering a player's level!\n\r", ch);
        send_to_char ("**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r", victim);
        victim->level = 1;
        victim->exp = exp_per_level (victim, victim->pcdata->points);
        victim->max_hit = 20;
        victim->max_mana = 100;
        victim->max_move = 100;
        victim->train = 3;
        victim->practice = 5;
        victim->hit = victim->max_hit;
        victim->mana = victim->max_mana;
        victim->move = victim->max_move;
        victim->pcdata->perm_hit = 20;
        victim->pcdata->perm_mana = 100;
        victim->pcdata->perm_move = 100;
        victim->rplevel = 0;
        victim->rpexp = 0;
    }
    else if (level != 1 && level <= victim->level)
    {
        send_to_char ("Lowering a player's level!\n\r", ch);
        send_to_char ("**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r", victim);
	for (iLevel = victim->level; iLevel > level; iLevel--)
	    advance_level (victim, FALSE, FALSE);		
    }
    else
    {
        send_to_char ("Raising a player's level!\n\r", ch);
        send_to_char ("**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim);
        for (iLevel = victim->level; iLevel < level; iLevel++)
            advance_level (victim, TRUE, TRUE);

    }

    
    sprintf (buf, "You are now level %d.\n\r", victim->level);
    send_to_char (buf, victim);
    victim->exp = exp_per_level (victim, victim->pcdata->points)
        * UMAX (1, victim->level);
    victim->trust = 0;
    save_char_obj (victim);
    return;
}



void do_trust (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int level;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0' || !is_number (arg2))
    {
        send_to_char ("Syntax: trust <char> <level>.\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg1)) == NULL)
    {
        send_to_char ("That player is not here.\n\r", ch);
        return;
    }

    if ((level = atoi (arg2)) < 0 || level > MAX_LEVEL)
    {
        sprintf (buf, "Level must be 0 (reset) or 1 to %d.\n\r", MAX_LEVEL);
        send_to_char (buf, ch);
        return;
    }

    if (level > get_trust (ch))
    {
        send_to_char ("Limited to your trust.\n\r", ch);
        return;
    }

    victim->trust = level;
    return;
}



void do_restore (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    DESCRIPTOR_DATA *d;

    one_argument (argument, arg);
    if (arg[0] == '\0' || !str_cmp (arg, "room"))
    {
        /* cure room */

        for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
        {
            if (!IS_NPC(vch) && vch->pcdata->pk_timer > 0)
              continue;

            affect_strip (vch, gsn_plague);
            affect_strip (vch, gsn_poison);
            affect_strip (vch, gsn_blindness);
            affect_strip (vch, gsn_sleep);
            affect_strip (vch, gsn_curse);
	    affect_strip (vch, gsn_ravage);

            vch->hit = vch->max_hit;
            vch->mana = vch->max_mana;
            vch->move = vch->max_move;
            if (!IS_NPC(vch))
              SET_BLEED(vch, FALSE); 
            update_pos (vch);
            act ("$n has restored you.", ch, NULL, vch, TO_VICT);
        }

        sprintf (buf, "$N restored room %ld.", ch->in_room->vnum);
        wiznet (buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust (ch));

        send_to_char ("Room restored.\n\r", ch);
        return;

    }

    if ((get_trust (ch) >= MAX_LEVEL - 1 || !str_cmp(ch->name,"Creator")) && !str_cmp (arg, "all"))
    {
        /* cure all */

        for (d = descriptor_list; d != NULL; d = d->next)
        {
            victim = d->character;

            if (victim == NULL || IS_NPC (victim) || (!IS_NPC(victim) && victim->pcdata->pk_timer > 0))
                continue;
          
            affect_strip (victim, gsn_plague);
            affect_strip (victim, gsn_poison);
            affect_strip (victim, gsn_blindness);
            affect_strip (victim, gsn_sleep);
            affect_strip (victim, gsn_curse);
	    affect_strip (victim, gsn_ravage);

            victim->hit = victim->max_hit;
            victim->mana = victim->max_mana;
            victim->move = victim->max_move;
            update_pos (victim);
            SET_BLEED(victim, FALSE);  
            if (victim->in_room != NULL)
                act ("$n has restored you.", ch, NULL, victim, TO_VICT);
        }
        send_to_char ("All active players restored.\n\r", ch);
        return;
    }

    if (!str_cmp (arg, "quest"))
    {
        /* cure all quest */

        for (d = descriptor_list; d != NULL; d = d->next)
        {
            victim = d->character;

            if (victim == NULL || IS_NPC (victim))
                continue;

  	    if (!IS_SET(victim->act, PLR_QUESTING))
                continue;   
 
            affect_strip (victim, gsn_plague);
            affect_strip (victim, gsn_poison);
            affect_strip (victim, gsn_blindness);
            affect_strip (victim, gsn_sleep);
            affect_strip (victim, gsn_curse);
	    affect_strip (victim, gsn_ravage);

            victim->hit = victim->max_hit;
            victim->mana = victim->max_mana;
            victim->move = victim->max_move;
            SET_BLEED(victim, FALSE);
            update_pos (victim);
            if (victim->in_room != NULL)
                act ("$n has restored you.", ch, NULL, victim, TO_VICT);
        }
        send_to_char ("All quest players restored.\n\r", ch);
        return;
    }
    if (!str_cmp (arg, "tournament"))
    {
        

        for (d = descriptor_list; d != NULL; d = d->next)
        {
            victim = d->character;

            if (victim == NULL || IS_NPC (victim))
                continue;

  	    if (!IS_SET(victim->act, PLR_TOURNEY))
                continue;   
 
            affect_strip (victim, gsn_plague);
            affect_strip (victim, gsn_poison);
            affect_strip (victim, gsn_blindness);
            affect_strip (victim, gsn_sleep);
            affect_strip (victim, gsn_curse);
	    affect_strip (victim, gsn_curse);

            victim->hit = victim->max_hit;
            victim->mana = victim->max_mana;
            victim->move = victim->max_move;
            SET_BLEED(victim, FALSE);  
            update_pos (victim);
            if (victim->in_room != NULL)
                act ("$n has restored you.", ch, NULL, victim, TO_VICT);
        }
        send_to_char ("All tournament players restored.\n\r", ch);
        return;
    }
    if (!str_cmp (arg, "tag"))
    {
        

        for (d = descriptor_list; d != NULL; d = d->next)
        {
            victim = d->character;

            if (victim == NULL || IS_NPC (victim))
                continue;

  	    if (!IS_SET(victim->act, PLR_TAG) || (!IS_SET(victim->act, PLR_IT)))
                continue;   
 
            affect_strip (victim, gsn_plague);
            affect_strip (victim, gsn_poison);
            affect_strip (victim, gsn_blindness);
            affect_strip (victim, gsn_sleep);
            affect_strip (victim, gsn_curse);
	    affect_strip (victim, gsn_curse);

            victim->hit = victim->max_hit;
            victim->mana = victim->max_mana;
            victim->move = victim->max_move;
            SET_BLEED(victim, FALSE);
            update_pos (victim);
            if (victim->in_room != NULL)
                act ("$n has restored you.", ch, NULL, victim, TO_VICT);
        }
        send_to_char ("All tag players restored.\n\r", ch);
        return;
    }

    if ((victim = get_char_world_special (arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (!IS_NPC(victim) && victim->pcdata->pk_timer > 0)
    {
        send_to_char ("They are in PK right now.\n\r", ch);
        return;
    }

    affect_strip (victim, gsn_plague);
    affect_strip (victim, gsn_poison);
    affect_strip (victim, gsn_blindness);
    affect_strip (victim, gsn_sleep);
    affect_strip (victim, gsn_curse);
    affect_strip (victim, gsn_ravage);
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    if (!IS_NPC(victim))
      SET_BLEED(victim, FALSE);
    update_pos (victim);
    act ("$n has restored you.", ch, NULL, victim, TO_VICT);
    sprintf (buf, "$N restored %s",
             IS_NPC (victim) ? victim->short_descr : victim->name);
    wiznet (buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust (ch));
    send_to_char ("Ok.\n\r", ch);
    return;
}


void do_freeze (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Freeze whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (IS_NPC (victim))
    {
        send_to_char ("Not on NPC's.\n\r", ch);
        return;
    }

    if (get_trust (victim) >= get_trust (ch))
    {
        send_to_char ("You failed.\n\r", ch);
        return;
    }

    if (IS_SET (victim->act, PLR_FREEZE))
    {
        REMOVE_BIT (victim->act, PLR_FREEZE);
        send_to_char ("You can play again.\n\r", victim);
        send_to_char ("FREEZE removed.\n\r", ch);
        sprintf (buf, "$N thaws %s.", victim->name);
        wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else
    {
        SET_BIT (victim->act, PLR_FREEZE);
        send_to_char ("You can't do ANYthing!\n\r", victim);
        send_to_char ("FREEZE set.\n\r", ch);
        sprintf (buf, "$N puts %s in the deep freeze.", victim->name);
        wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    save_char_obj (victim);

    return;
}



void do_log (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Log whom?\n\r", ch);
        return;
    }

    if (!str_cmp (arg, "all"))
    {
        if (fLogAll)
        {
            fLogAll = FALSE;
            send_to_char ("Log ALL off.\n\r", ch);
        }
        else
        {
            fLogAll = TRUE;
            send_to_char ("Log ALL on.\n\r", ch);
        }
        return;
    }

    if ((victim = get_char_world (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (IS_NPC (victim))
    {
        send_to_char ("Not on NPC's.\n\r", ch);
        return;
    }

    /*
     * No level check, gods can log anyone.
     */
    if (IS_SET (victim->act, PLR_LOG))
    {
        REMOVE_BIT (victim->act, PLR_LOG);
        send_to_char ("LOG removed.\n\r", ch);
    }
    else
    {
        SET_BIT (victim->act, PLR_LOG);
        send_to_char ("LOG set.\n\r", ch);
    }

    return;
}



void do_noemote (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Noemote whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }


    if (get_trust (victim) >= get_trust (ch))
    {
        send_to_char ("You failed.\n\r", ch);
        return;
    }

    if (IS_SET (victim->comm, COMM_NOEMOTE))
    {
        REMOVE_BIT (victim->comm, COMM_NOEMOTE);
        send_to_char ("You can emote again.\n\r", victim);
        send_to_char ("NOEMOTE removed.\n\r", ch);
        sprintf (buf, "$N restores emotes to %s.", victim->name);
        wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else
    {
        SET_BIT (victim->comm, COMM_NOEMOTE);
        send_to_char ("You can't emote!\n\r", victim);
        send_to_char ("NOEMOTE set.\n\r", ch);
        sprintf (buf, "$N revokes %s's emotes.", victim->name);
        wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
}



void do_notell (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Notell whom?", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (get_trust (victim) >= get_trust (ch))
    {
        send_to_char ("You failed.\n\r", ch);
        return;
    }

    if (IS_SET (victim->comm, COMM_NOTELL))
    {
        REMOVE_BIT (victim->comm, COMM_NOTELL);
        send_to_char ("You can tell again.\n\r", victim);
        send_to_char ("NOTELL removed.\n\r", ch);
        sprintf (buf, "$N restores tells to %s.", victim->name);
        wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }
    else
    {
        SET_BIT (victim->comm, COMM_NOTELL);
        send_to_char ("You can't tell!\n\r", victim);
        send_to_char ("NOTELL set.\n\r", ch);
        sprintf (buf, "$N revokes %s's tells.", victim->name);
        wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    }

    return;
}



void do_peace (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *rch;

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if (rch->fighting != NULL)
            stop_fighting (rch, TRUE);

        if (IS_NPC (rch) && IS_SET (rch->act, ACT_AGGRESSIVE))
        {
           REMOVE_BIT (rch->act, ACT_AGGRESSIVE);
        } 
  
        if (IS_NPC(rch))
          mem_clear(rch);

    }

    send_to_char ("Ok.\n\r", ch);
    return;
}

void do_wizlock (CHAR_DATA * ch, char *argument)
{
    extern bool wizlock;
    wizlock = !wizlock;

    if (wizlock)
    {
        wiznet ("$N has wizlocked the game.", ch, NULL, 0, 0, 0);
        send_to_char ("Game wizlocked.\n\r", ch);
    }
    else
    {
        wiznet ("$N removes wizlock.", ch, NULL, 0, 0, 0);
        send_to_char ("Game un-wizlocked.\n\r", ch);
    }

    return;
}

/* RT anti-newbie code */

void do_newlock (CHAR_DATA * ch, char *argument)
{
    extern bool newlock;
    newlock = !newlock;

    if (newlock)
    {
        wiznet ("$N locks out new characters.", ch, NULL, 0, 0, 0);
        send_to_char ("New characters have been locked out.\n\r", ch);
    }
    else
    {
        wiznet ("$N allows new characters back in.", ch, NULL, 0, 0, 0);
        send_to_char ("Newlock removed.\n\r", ch);
    }

    return;
}


void do_slookup (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int sn;

    one_argument (argument, arg);
    if (arg[0] == '\0')
    {
        send_to_char ("Lookup which skill or spell?\n\r", ch);
        return;
    }

    if (!str_cmp (arg, "all"))
    {
        for (sn = 0; sn < MAX_SKILL; sn++)
        {
            if (skill_table[sn].name == NULL)
                break;
            sprintf (buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",
                     sn, skill_table[sn].slot, skill_table[sn].name);
            send_to_char (buf, ch);
        }
    }
    else
    {
        if ((sn = skill_lookup (arg)) < 0)
        {
            send_to_char ("No such skill or spell.\n\r", ch);
            return;
        }

        sprintf (buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",
                 sn, skill_table[sn].slot, skill_table[sn].name);
        send_to_char (buf, ch);
    }

    return;
}

/* RT set replaces sset, mset, oset, and rset */

void do_set (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Syntax:\n\r", ch);
        send_to_char ("  set mob   <name> <field> <value>\n\r", ch);
        send_to_char ("  set obj   <name> <field> <value>\n\r", ch);
        send_to_char ("  set room  <room> <field> <value>\n\r", ch);
        send_to_char ("  set skill <name> <spell or skill> <value>\n\r", ch);
        return;
    }

    if (!str_prefix (arg, "mobile") || !str_prefix (arg, "character"))
    {
        do_function (ch, &do_mset, argument);
        return;
    }

    if (!str_prefix (arg, "skill") || !str_prefix (arg, "spell"))
    {
        do_function (ch, &do_sset, argument);
        return;
    }

    if (!str_prefix (arg, "object"))
    {
        do_function (ch, &do_oset, argument);
        return;
    }

    if (!str_prefix (arg, "room"))
    {
        do_function (ch, &do_rset, argument);
        return;
    }
    /* echo syntax */
    do_function (ch, &do_set, "");
}


void do_sset (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;
    int sn;
    bool fAll;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);
    argument = one_argument (argument, arg3);

    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
    {
        send_to_char ("Syntax:\n\r", ch);
        send_to_char ("  set skill <name> <spell or skill> <value>\n\r", ch);
        send_to_char ("  set skill <name> all <value>\n\r", ch);
        send_to_char ("   (use the name of the skill, not the number)\n\r",
                      ch);
        return;
    }

    if ((victim = get_char_world (ch, arg1)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (IS_NPC (victim))
    {
        send_to_char ("Not on NPC's.\n\r", ch);
        return;
    }

    fAll = !str_cmp (arg2, "all");
    sn = 0;
    if (!fAll && (sn = skill_lookup (arg2)) < 0)
    {
        send_to_char ("No such skill or spell.\n\r", ch);
        return;
    }

    /*
     * Snarf the value.
     */
    if (!is_number (arg3))
    {
        send_to_char ("Value must be numeric.\n\r", ch);
        return;
    }

    value = atoi (arg3);
    if (value < 0 || value > 100)
    {
        send_to_char ("Value range is 0 to 100.\n\r", ch);
        return;
    }

    if (fAll)
    {
        for (sn = 0; sn < MAX_SKILL; sn++)
        {
                if ( skill_table[sn].name != "daze" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "howl" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "tap" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "damane" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "store" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "medallion" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "veil" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "scry" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "fix" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "build" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "dream" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "restrain" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "firestorm" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "still" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "balefire" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "void" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "stealth" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "shroud" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "pray" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "taint" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "blood" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "ravage" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "damane" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "luck" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "warcry" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "stance" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "cleanse" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "ballad" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "tame" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "couple" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "horseback" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "ensnare" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "study" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "save_life" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "unravel" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "bomb" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "tame" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "dome" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "hunt" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "wolf" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "willpower" )
					victim->pcdata->learned[sn] = value;
                else if ( skill_table[sn].name != "contact" ) 
					victim->pcdata->learned[sn] = value;
			    else if ( ( skill_table[sn].name != NULL ) )
                        victim->pcdata->learned[sn] = value;
        }
    }
    else
    {
        victim->pcdata->learned[sn] = value;
    }

    return;
}


void do_mset (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *victim;
    int value;

    smash_tilde (argument);
    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);
    strcpy (arg3, argument);

    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
    {
        send_to_char ("Syntax:\n\r", ch);
        send_to_char ("  set char <name> <field> <value>\n\r", ch);
        send_to_char ("  Field being one of:\n\r", ch);
        send_to_char ("    str int wis dex con sex class level\n\r", ch);
        send_to_char ("    race group gold silver hp mana move prac\n\r", ch);
        send_to_char ("    align train thirst hunger drunk full\n\r", ch);
        send_to_char ("    security qp rpxp hunt pnts murder\n\r", ch);
        send_to_char ("    thief jail punishment\n\r", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg1)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    /* clear zones for mobs */
    victim->zone = NULL;

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number (arg3) ? atoi (arg3) : -1;

    /*
     * Set something.
     */
    if (!str_cmp (arg2, "str"))
    {
        if (value < 3 || value > get_max_train (victim, STAT_STR))
        {
            sprintf (buf,
                     "Strength range is 3 to %d\n\r.",
                     get_max_train (victim, STAT_STR));
            send_to_char (buf, ch);
            return;
        }

        victim->perm_stat[STAT_STR] = value;
        return;
    }

    if (!str_cmp (arg2, "security"))
    {                            /* OLC */
        if (IS_NPC (ch))
        {
            send_to_char ("Si, claro.\n\r", ch);
            return;
        }

        if (IS_NPC (victim))
        {
            send_to_char ("Not on NPC's.\n\r", ch);
            return;
        }

        if (value > ch->pcdata->security || value < 0)
        {
            if (ch->pcdata->security != 0)
            {
                sprintf (buf, "Valid security is 0-%d.\n\r",
                         ch->pcdata->security);
                send_to_char (buf, ch);
            }
            else
            {
                send_to_char ("Valid security is 0 only.\n\r", ch);
            }
            return;
        }
        victim->pcdata->security = value;
        return;
    }

    if (!str_cmp (arg2, "int"))
    {
        if (value < 3 || value > get_max_train (victim, STAT_INT))
        {
            sprintf (buf,
                     "Intelligence range is 3 to %d.\n\r",
                     get_max_train (victim, STAT_INT));
            send_to_char (buf, ch);
            return;
        }

        victim->perm_stat[STAT_INT] = value;
        return;
    }

    if (!str_cmp (arg2, "wis"))
    {
        if (value < 3 || value > get_max_train (victim, STAT_WIS))
        {
            sprintf (buf,
                     "Wisdom range is 3 to %d.\n\r", get_max_train (victim,
                                                                    STAT_WIS));
            send_to_char (buf, ch);
            return;
        }

        victim->perm_stat[STAT_WIS] = value;
        return;
    }

    if (!str_cmp (arg2, "dex"))
    {
        if (value < 3 || value > get_max_train (victim, STAT_DEX))
        {
            sprintf (buf,
                     "Dexterity range is 3 to %d.\n\r",
                     get_max_train (victim, STAT_DEX));
            send_to_char (buf, ch);
            return;
        }

        victim->perm_stat[STAT_DEX] = value;
        return;
    }

    if (!str_cmp (arg2, "con"))
    {
        if (value < 3 || value > get_max_train (victim, STAT_CON))
        {
            sprintf (buf,
                     "Constitution range is 3 to %d.\n\r",
                     get_max_train (victim, STAT_CON));
            send_to_char (buf, ch);
            return;
        }

        victim->perm_stat[STAT_CON] = value;
        return;
    }

    if (!str_prefix (arg2, "sex"))
    {
        if (value < 0 || value > 2)
        {
            send_to_char ("Sex range is 0 to 2.\n\r", ch);
            return;
        }
        victim->sex = value;
        if (!IS_NPC (victim))
            victim->pcdata->true_sex = value;
        return;
    }

    if (!str_prefix (arg2, "class"))
    {
        int cClass;

        if (IS_NPC (victim))
        {
            send_to_char ("Mobiles have no class.\n\r", ch);
            return;
        }

        cClass = class_lookup (arg3);
        if (cClass == -1)
        {
            char buf[MAX_STRING_LENGTH];

            strcpy (buf, "Possible classes are: ");
            for (cClass = 0; cClass < MAX_CLASS; cClass++)
            {
                if (cClass > 0)
                    strcat (buf, " ");
                strcat (buf, class_table[cClass].name);
            }
            strcat (buf, ".\n\r");

            send_to_char (buf, ch);
            return;
        }

        victim->cClass = cClass;
        return;
    }

    if (!str_prefix (arg2, "level"))
    {
        if (!IS_NPC (victim))
        {
            send_to_char ("Not on PC's.\n\r", ch);
            return;
        }

        if (value < 0 || value > MAX_LEVEL * 3)
        {
            sprintf (buf, "Level range is 0 to %d.\n\r", MAX_LEVEL);
            send_to_char (buf, ch);
            return;
        }
        victim->level = value;
        return;
    }

    if (!str_prefix (arg2, "gametickets"))
    {
        if (IS_NPC (victim))
        {
            send_to_char("Not on NPCs.\n\r", ch);
            return;
        }
        victim->gametick = value;
        return;
    }
    if (!str_prefix (arg2, "rpexp"))
    {
        if (IS_NPC (victim))
        {
            send_to_char ("Not on PC's.\n\r", ch);
            return;
        }

        if (value < -1000 || value > 5000)
        {
            sprintf (buf, "Level range is -1000 to 5000.\n\r");
            send_to_char (buf, ch);
            return;
        }
        victim->rpexp = value;
        return;
    }
    if (!str_cmp (arg2, "pnts"))
    {
    	if (IS_NPC(victim))
    	{
            send_to_char ("Not on PC's.\n\r", ch);
            return;
        }

        if (value < 40 || value > 250)
        {
            sprintf (buf,"Cps range is 40 - 250\n\r.");
            send_to_char (buf, ch);
            return;
        }
        victim->pcdata->points = value;
        return;
    }

    if (!str_prefix (arg2, "gold"))
    {
        victim->gold = value;
        return;
    }

    if (!str_prefix (arg2, "silver"))
    {
        victim->silver = value;
        return;
    }

    if (!str_prefix (arg2, "hp"))
    {
        if (value < -10 || value > 500000)
        {
            send_to_char ("Hp range is -10 to 500,000 hit points.\n\r", ch);
            return;
        }
        victim->max_hit = value;
        if (!IS_NPC (victim))
            victim->pcdata->perm_hit = value;
        return;
    }

    if (!str_prefix (arg2, "mana"))
    {
        if (value < 0 || value > 30000)
        {
            send_to_char ("Mana range is 0 to 30,000 mana points.\n\r", ch);
            return;
        }
        victim->max_mana = value;
        if (!IS_NPC (victim))
            victim->pcdata->perm_mana = value;
        return;
    }

    if (!str_prefix (arg2, "move"))
    {
        if (value < 0 || value > 30000)
        {
            send_to_char ("Move range is 0 to 30,000 move points.\n\r", ch);
            return;
        }
        victim->max_move = value;
        if (!IS_NPC (victim))
            victim->pcdata->perm_move = value;
        return;
    }
    if (!str_prefix (arg2, "played"))
    {
        if (IS_NPC(victim))
        {
            send_to_char("Player command only.\n\r",ch);
            return;
        }
        victim->played = value;
        return;
    }

    if (!str_prefix (arg2, "practice"))
    {
        if (value < 0 || value > 250)
        {
            send_to_char ("Practice range is 0 to 250 sessions.\n\r", ch);
            return;
        }
        victim->practice = value;
        return;
    }
    
   

    if (!str_prefix (arg2, "train"))
    {
        if (value < 0 || value > 100)
        {
            send_to_char ("Training session range is 0 to 100 sessions.\n\r",
                          ch);
            return;
        }
        victim->train = value;
        return;
    }

    if (!str_prefix (arg2, "align"))
    {
        if (value < -1000 || value > 1000)
        {
            send_to_char ("Alignment range is -1000 to 1000.\n\r", ch);
            return;
        }
        victim->alignment = value;
        return;
    }

    if (!str_prefix (arg2, "thirst"))
    {
        if (IS_NPC (victim))
        {
            send_to_char ("Not on NPC's.\n\r", ch);
            return;
        }

        if (value < -1 || value > 100)
        {
            send_to_char ("Thirst range is -1 to 100.\n\r", ch);
            return;
        }

        victim->pcdata->condition[COND_THIRST] = value;
        return;
    }

    if (!str_prefix (arg2, "drunk"))
    {
        if (IS_NPC (victim))
        {
            send_to_char ("Not on NPC's.\n\r", ch);
            return;
        }

        if (value < -1 || value > 100)
        {
            send_to_char ("Drunk range is -1 to 100.\n\r", ch);
            return;
        }

        victim->pcdata->condition[COND_DRUNK] = value;
        return;
    }

    if (!str_prefix (arg2, "full"))
    {
        if (IS_NPC (victim))
        {
            send_to_char ("Not on NPC's.\n\r", ch);
            return;
        }

        if (value < -1 || value > 100)
        {
            send_to_char ("Full range is -1 to 100.\n\r", ch);
            return;
        }

        victim->pcdata->condition[COND_FULL] = value;
        return;
    }

    if (!str_prefix (arg2, "hunger"))
    {
        if (IS_NPC (victim))
        {
            send_to_char ("Not on NPC's.\n\r", ch);
            return;
        }

        if (value < -1 || value > 100)
        {
            send_to_char ("Full range is -1 to 100.\n\r", ch);
            return;
        }

        victim->pcdata->condition[COND_HUNGER] = value;
        return;
    }

	if ( !str_prefix( arg2, "bleeding" ) )
    {
        if ( IS_NPC(victim) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {                    
           send_to_char( "Bleeding range is -1 to 100.\n\r", ch );
            return;
        }

        victim->pcdata->condition[COND_BLEEDING] = value;
        return;
    }            


    if (!str_prefix (arg2, "race"))
    {
        int race;

        race = race_lookup (arg3);

        if (race == 0)
        {
            send_to_char ("That is not a valid race.\n\r", ch);
            return;
        }

        if (!IS_NPC (victim) && !race_table[race].pc_race)
        {
            send_to_char ("That is not a valid player race.\n\r", ch);
            return;
        }

        victim->race = race;
        return;
    }

    if (!str_prefix (arg2, "group"))
    {
        if (!IS_NPC (victim))
        {
            send_to_char ("Only on NPCs.\n\r", ch);
            return;
        }
        victim->group = value;
        return;
    }

    if (!str_prefix (arg2, "qp"))
    {
        if (value < 1 || value > 30000)
        {
            send_to_char ("QPs range is 1 to 30,000.\n\r", ch);
            return;
        }
        victim->questpoints = value;
        return;
    }

    if (!str_prefix (arg2, "murder"))
    {
        if (value < 0 || value > 3000)
        {
            send_to_char ("Murder range is 0 to 3,000.\n\r", ch);
            return;
        }
        victim->penalty.murder = value;
        return;
    }
    if (!str_prefix (arg2, "thief"))
    {
        if (value < 0 || value > 3000)
        {
            send_to_char ("Thief range is 0 to 3,000.\n\r", ch);
            return;
        }
        victim->penalty.thief = value;
        return;
    }
    if (!str_prefix (arg2, "jail"))
    {
        if (value < 0 || value > 3000)
        {
            send_to_char ("Jail range is 0 to 3,000.\n\r", ch);
            return;
        }
        victim->penalty.jail = value;
        return;
    }
    if (!str_prefix (arg2, "punishment"))
    {
        if (value < 0 || value > 100)
        {
            send_to_char ("Punishment range is 0 to 100.\n\r", ch);
            return;
        }
        victim->penalty.punishment = value;
        return;
    }

   if (!str_prefix (arg2, "exp"))
{
victim->exp = value;
return;
}
	if (!str_cmp(arg2, "hunt"))
	{
		CHAR_DATA *hunted = 0;

		if ( !IS_NPC(victim) )
		{
			send_to_char( "Not on PC's.\n\r", ch );
			return;
		}

		if ( str_cmp( arg3, "." ) )
		  if ( (hunted = get_char_area(victim, arg3)) == NULL )
			{
				send_to_char("Mob couldn't locate the victim to hunt.\n\r", ch);
				return;
			}

		victim->hunting = hunted;
		return;
	}

    /*
     * Generate usage message.
     */
    do_function (ch, &do_mset, "");
    return;
}

void do_pretitle (CHAR_DATA * ch, char *argument)
{
  char arg1[MIL];
  CHAR_DATA *victim;

  argument = one_argument(argument, arg1);
  
  if ((victim = get_char_world(ch, arg1)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
	 
  victim->pretit = str_dup(argument);
  send_to_char("Pretitle Set.\n\r", ch);
  send_to_char("Pretitle Set.\n\r", victim);
}
void do_plaidstring (CHAR_DATA * ch, char *argument)
{
     char arg1 [MIL];
     CHAR_DATA *victim;
     
     argument = one_argument(argument, arg1);

  if ((victim = get_char_world(ch, arg1)) == NULL)
  {
   send_to_char("They aren't here.\n\r", ch);
   return;
  }
  if (victim->level > ch->level)
  {
   send_to_char("Thats not a good idea!\n\r", ch);
   send_to_char("Someone has tried to PLAIDSTRING you!\n\r", victim);
   return;
  }
  victim->pretit = str_dup(argument);
  send_to_char("You have been `?S`?t`?R`?u`?N`?g`?!`7\n\r", victim);
  send_to_char("Done!\n\r", ch);
  return;
}






 
void do_string (CHAR_DATA * ch, char *argument)
{
    char type[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    smash_tilde (argument);
    argument = one_argument (argument, type);
    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);
    strcpy (arg3, argument);

    if (type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0'
        || arg3[0] == '\0')
    {
        send_to_char ("Syntax:\n\r", ch);
        send_to_char ("  string char <name> <field> <string>\n\r", ch);
        send_to_char ("    fields: name short long desc title spec\n\r", ch);
        send_to_char ("  string obj  <name> <field> <string>\n\r", ch);
        send_to_char ("    fields: name short long extended\n\r", ch);
        return;
    }

    
    if (!str_prefix (type, "character") || !str_prefix (type, "mobile"))
    {
        if ((victim = get_char_world (ch, arg1)) == NULL)
        {
            send_to_char ("They aren't here.\n\r", ch);
            return;
        }

        /* clear zone for mobs */
        victim->zone = NULL;

        /* string something */

        if (!str_prefix (arg1, "acebo") && !str_cmp (ch->name, "Dreyus"))
	{
	  send_to_char("\n`&You can't win in this game, Drey.`*\n", ch);
	  return;
	}
	if (!str_prefix (arg2, "name"))
        {
            if (!IS_NPC (victim))
            {
                send_to_char ("Not on PC's.\n\r", ch);
                return;
            }
            free_string (victim->name);
            victim->name = str_dup (arg3);
            return;
        }

        if (!str_prefix (arg2, "description"))
        {
            free_string (victim->description);
            victim->description = str_dup (arg3);
            return;
        }

        if (!str_prefix (arg2, "short"))
        {
            free_string (victim->short_descr);
            victim->short_descr = str_dup (arg3);
            return;
        }

        if (!str_prefix (arg2, "long"))
        {
            free_string (victim->long_descr);
            strcat (arg3, "\n\r");
            victim->long_descr = str_dup (arg3);
            return;
        }

       if (!str_prefix (arg2, "pretitle"))
        {
          victim->pretit = str_dup (arg3);
          return;
        } 
       if (!str_prefix (arg2, "title"))
        {
            if (IS_NPC (victim))
            {
                send_to_char ("Not on NPC's.\n\r", ch);
                return;
            }

            set_title (victim, arg3);
            return;
        }

        if (!str_prefix (arg2, "spec"))
        {
            if (!IS_NPC (victim))
            {
                send_to_char ("Not on PC's.\n\r", ch);
                return;
            }

            if ((victim->spec_fun = spec_lookup (arg3)) == 0)
            {
                send_to_char ("No such spec fun.\n\r", ch);
                return;
            }

            return;
        }
		
    }

    if (!str_prefix (type, "object"))
    {
        /* string an obj */

        if ((obj = get_obj_world (ch, arg1)) == NULL)
        {
            send_to_char ("Nothing like that in heaven or earth.\n\r", ch);
            return;
        }

        if (!str_prefix (arg2, "name"))
        {
            free_string (obj->name);
            obj->name = str_dup (arg3);
            return;
        }

        if (!str_prefix (arg2, "short"))
        {
            free_string (obj->short_descr);
            obj->short_descr = str_dup (arg3);
            return;
        }

        if (!str_prefix (arg2, "long"))
        {
            free_string (obj->description);
            obj->description = str_dup (arg3);
            return;
        }

        if (!str_prefix (arg2, "ed") || !str_prefix (arg2, "extended"))
        {
            EXTRA_DESCR_DATA *ed;

            argument = one_argument (argument, arg3);
            if (argument == NULL)
            {
                send_to_char
                    ("Syntax: oset <object> ed <keyword> <string>\n\r", ch);
                return;
            }

            strcat (argument, "\n\r");

            ed = new_extra_descr ();

            ed->keyword = str_dup (arg3);
            ed->description = str_dup (argument);
            ed->next = obj->extra_descr;
            obj->extra_descr = ed;
            return;
        }
    }


    /* echo bad use message */
    do_function (ch, &do_string, "");
}



void do_oset (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int value;

    smash_tilde (argument);
    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);
    strcpy (arg3, argument);

    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
    {
        send_to_char ("Syntax:\n\r", ch);
        send_to_char ("  set obj <object> <field> <value>\n\r", ch);
        send_to_char ("  Field being one of:\n\r", ch);
        send_to_char ("    value0 value1 value2 value3 value4 (v1-v4)\n\r",
                      ch);
        send_to_char ("    extra wear level weight cost timer enchants\n\r", ch);
        return;
    }

    if ((obj = get_obj_world (ch, arg1)) == NULL)
    {
        send_to_char ("Nothing like that in heaven or earth.\n\r", ch);
        return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi (arg3);

    /*
     * Set something.
     */
    if (!str_cmp (arg2, "value0") || !str_cmp (arg2, "v0"))
    {
        obj->value[0] = UMIN (50, value);
        return;
    }

    if (!str_cmp (arg2, "value1") || !str_cmp (arg2, "v1"))
    {
        obj->value[1] = value;
        return;
    }

    if (!str_cmp (arg2, "value2") || !str_cmp (arg2, "v2"))
    {
        obj->value[2] = value;
        return;
    }

    if (!str_cmp (arg2, "value3") || !str_cmp (arg2, "v3"))
    {
        obj->value[3] = value;
        return;
    }

    if (!str_cmp (arg2, "value4") || !str_cmp (arg2, "v4"))
    {
        obj->value[4] = value;
        return;
    }
    if (!str_prefix (arg2, "enchants"))
    {
        obj->enchants = value;
        return;
    }
    if (!str_prefix (arg2, "extra"))
    {
        obj->extra_flags = value;
        return;
    }

    if (!str_prefix (arg2, "wear"))
    {
        obj->wear_flags = value;
        return;
    }

    if (!str_prefix (arg2, "level"))
    {
        obj->level = value;
        return;
    }

    if (!str_prefix (arg2, "weight"))
    {
        obj->weight = value;
        return;
    }

    if (!str_prefix (arg2, "cost"))
    {
        obj->cost = value;
        return;
    }

    if (!str_prefix (arg2, "timer"))
    {
        obj->timer = value;
        return;
    }

    /*
     * Generate usage message.
     */
    do_function (ch, &do_oset, "");
    return;
}



void do_rset (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    int value;

    smash_tilde (argument);
    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);
    strcpy (arg3, argument);

    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
    {
        send_to_char ("Syntax:\n\r", ch);
        send_to_char ("  set room <location> <field> <value>\n\r", ch);
        send_to_char ("  Field being one of:\n\r", ch);
        send_to_char ("    flags sector\n\r", ch);
        return;
    }

    if ((location = find_location (ch, arg1)) == NULL)
    {
        send_to_char ("No such location.\n\r", ch);
        return;
    }

    if (!is_room_owner (ch, location) && ch->in_room != location
        && room_is_private (location) && !IS_TRUSTED (ch, IMPLEMENTOR))
    {
        send_to_char ("That room is private right now.\n\r", ch);
        return;
    }

    /*
     * Snarf the value.
     */
    if (!is_number (arg3))
    {
        send_to_char ("Value must be numeric.\n\r", ch);
        return;
    }
    value = atoi (arg3);

    /*
     * Set something.
     */
    if (!str_prefix (arg2, "flags"))
    {
        location->room_flags = value;
        return;
    }

    if (!str_prefix (arg2, "sector"))
    {
        location->sector_type = value;
        return;
    }

    /*
     * Generate usage message.
     */
    do_function (ch, &do_rset, "");
    return;
}


/***** OLD SOCKET CODE

void do_sockets (CHAR_DATA * ch, char *argument)
{
    char buf[2 * MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    int count;

    count = 0;
    buf[0] = '\0';

    one_argument (argument, arg);

    for (d = descriptor_list; d != NULL; d = d->next)
    {
    	if (IS_DRAGON(d->character) || IS_FORSAKEN(d->character))
	        return;

        if (d->character != NULL && can_see (ch, d->character)
            && (arg[0] == '\0' || is_name (arg, d->character->name)
                || (d->original && is_name (arg, d->original->name))))
        {
            count++;
            sprintf (buf + strlen (buf), "[%3d %2d] %s@%s\n\r",
                     d->descriptor,
                     d->connected,
                     d->original ? d->original->name :
                     d->character ? d->character->name : "(none)", d->host);
        }
    }
    if (count == 0)
    {
        send_to_char ("No one by that name is connected.\n\r", ch);
        return;
    }

    sprintf (buf2, "%d user%s\n\r", count, count == 1 ? "" : "s");
    strcat (buf, buf2);
    page_to_char (buf, ch);
    return;
}
***/

/* Written by Stimpy, ported to rom2.4 by Silverhand 3/12
 *
 *	Added the other COMM_ stuff that wasn't defined before 4/16 -Silverhand
 */
void do_sockets( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *vch;
    DESCRIPTOR_DATA *d;
    char            buf  [ MAX_STRING_LENGTH ];
    char            buf2 [ MAX_STRING_LENGTH ];
    int             count;
    char *          st;
    char            s[100];
    char            idle[10];
    char            arg1[MIL]; 
    bool            grep = FALSE;
    int             level = 0;
     
    argument = one_argument(argument, arg1); 

    if (!str_cmp(arg1, "|"))
      grep = TRUE;

    if (is_number(arg1))
      level = atoi(arg1);

    if (!IS_NULLSTR(arg1) && !str_prefix(arg1, "immortal"))
      level = 78;

    count       = 0;
    buf[0]      = '\0';
    buf2[0]     = '\0';

    strcat( buf2, "\n\r[Num Connected_State Login@ Idl] Player Name Host\n\r" );
    strcat( buf2,
"--------------------------------------------------------------------------\n\r");  
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->character && can_see( ch, d->character )) 
        {
           if (level > 0 && d->character->level < level)
             continue;

           if (grep && 
               (str_prefix(argument, d->character->name) &&
               str_prefix(argument, dns_gethostname(d->host))))
           {
             continue;
           }   
               

           /* NB: You may need to edit the CON_ values */
           switch( d->connected )
           {
              case CON_PLAYING:              st = "    PLAYING    ";    break;
              case CON_GET_NAME:             st = "   Get Name    ";    break;
              case CON_GET_OLD_PASSWORD:     st = "Get Old Passwd ";    break;
              case CON_CONFIRM_NEW_NAME:     st = " Confirm Name  ";    break;
              case CON_GET_NEW_PASSWORD:     st = "Get New Passwd ";    break;
              case CON_CONFIRM_NEW_PASSWORD: st = "Confirm Passwd ";    break;
              case CON_GET_NEW_RACE:         st = "  Get New Race ";    break;
              case CON_GET_NEW_SEX:          st = "  Get New Sex  ";    break;
              case CON_GET_NEW_CLASS:        st = " Get New Class ";    break;
              case CON_GET_ALIGNMENT:   	 st = " Get New Align ";	break;
              case CON_DEFAULT_CHOICE:		 st = " Choosing Cust ";	break;
              case CON_GEN_GROUPS:			 st = " Customization ";	break;
              case CON_PICK_WEAPON:			 st = " Picking Weapon";	break;
			  case CON_READ_IMOTD:			 st = " Reading IMOTD "; 	break;
			  case CON_BREAK_CONNECT:		 st = "   LINKDEAD    ";	break;
              case CON_READ_MOTD:            st = "  Reading MOTD ";    break;
              default:                       st = "   !UNKNOWN!   ";    break;
           }
           count++;
           
           /* Format "login" value... */
           vch = d->original ? d->original : d->character;
           strftime( s, 100, "%I:%M%p", localtime( &vch->logon ) );
           
             if ( vch->timer > 0 )
              sprintf( idle, "%-2d", vch->timer );
           else
              sprintf( idle, "  " );
           sprintf( buf, "[%3d %s %7s %2s] %-12s %-32.32s\n\r",
              d->descriptor,
              st,
              s,
              idle,
              ( d->original ) ? d->original->name
                              : ( d->character )  ? d->character->name
                                                  : "(None!)",
              IS_SET(d->character->act2, PLR_NOIP) ? "(Unknown)" 
                                                   : dns_gethostname(d->host));
              
           strcat( buf2, buf );

        }
    }

    sprintf( buf, "\n\r%d user%s\n\r", count, count == 1 ? "" : "s" );
    strcat( buf2, buf );
    send_to_char( buf2, ch );
    return;
}




/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument (argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0')
    {
        send_to_char ("Force whom to do what?\n\r", ch);
        return;
    }

    one_argument (argument, arg2);

    if (!str_cmp (arg2, "delete") || !str_prefix (arg2, "mob"))
    {
        send_to_char ("That will NOT be done.\n\r", ch);
        return;
    }

    sprintf (buf, "$n forces you to '%s'.", argument);

    if (!str_cmp (arg, "all"))
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        if (get_trust (ch) < MAX_LEVEL - 3)
        {
            send_to_char ("Not at your level!\n\r", ch);
            return;
        }

        for (vch = char_list; vch != NULL; vch = vch_next)
        {
            vch_next = vch->next;

            if (!IS_NPC (vch) && get_trust (vch) < get_trust (ch))
            {
                act (buf, ch, NULL, vch, TO_VICT);
                interpret (vch, argument);
            }
        }
    }
    else if (!str_cmp (arg, "players"))
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        if (get_trust (ch) < MAX_LEVEL - 2)
        {
            send_to_char ("Not at your level!\n\r", ch);
            return;
        }

        for (vch = char_list; vch != NULL; vch = vch_next)
        {
            vch_next = vch->next;

            if (!IS_NPC (vch) && get_trust (vch) < get_trust (ch)
                && vch->level < LEVEL_HERO)
            {
                act (buf, ch, NULL, vch, TO_VICT);
                interpret (vch, argument);
            }
        }
    }
    else if (!str_cmp (arg, "gods"))
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        if (get_trust (ch) < MAX_LEVEL - 2)
        {
            send_to_char ("Not at your level!\n\r", ch);
            return;
        }

        for (vch = char_list; vch != NULL; vch = vch_next)
        {
            vch_next = vch->next;

            if (!IS_NPC (vch) && get_trust (vch) < get_trust (ch)
                && vch->level >= LEVEL_HERO)
            {
                act (buf, ch, NULL, vch, TO_VICT);
                interpret (vch, argument);
            }
        }
    }
    else
    {
        CHAR_DATA *victim;

        if ((victim = get_char_world (ch, arg)) == NULL)
        {
            send_to_char ("They aren't here.\n\r", ch);
            return;
        }

        if (victim == ch)
        {
            send_to_char ("Aye aye, right away!\n\r", ch);
            return;
        }

        if (!is_room_owner (ch, victim->in_room)
            && ch->in_room != victim->in_room
            && room_is_private (victim->in_room)
            && !IS_TRUSTED (ch, IMPLEMENTOR))
        {
            send_to_char ("That character is in a private room.\n\r", ch);
            return;
        }

        if (get_trust (victim) >= get_trust (ch))
        {
            send_to_char ("Do it yourself!\n\r", ch);
            return;
        }

        if (!IS_NPC (victim) && get_trust (ch) < MAX_LEVEL - 3)
        {
            send_to_char ("Not at your level!\n\r", ch);
            return;
        }

        act (buf, ch, NULL, victim, TO_VICT);
        interpret (victim, argument);
    }

    send_to_char ("Ok.\n\r", ch);
    return;
}



/*
 * New routines by Dionysos.
 */
void do_invis (CHAR_DATA * ch, char *argument)
{
    int level;
    char arg[MAX_STRING_LENGTH];

    /* RT code for taking a level argument */
    one_argument (argument, arg);

    if (arg[0] == '\0')
        /* take the default path */

        if (ch->invis_level)
        {
            ch->invis_level = 0;
            act ("$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM);
            send_to_char ("You slowly fade back into existence.\n\r", ch);
        }
        else
        {
            ch->invis_level = LEVEL_IMMORTAL;
            act ("$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM);
            send_to_char ("You slowly vanish into thin air.\n\r", ch);
        }
    else
        /* do the level thing */
    {
        level = atoi (arg);
        if (level < 2 || level > get_trust (ch))
        {
            send_to_char ("Invis level must be between 2 and your level.\n\r",
                          ch);
            return;
        }
        else
        {
            ch->oreply = NULL;
            ch->invis_level = level;
            act ("$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM);
            send_to_char ("You slowly vanish into thin air.\n\r", ch);
        }
    }

    return;
}


void do_incognito (CHAR_DATA * ch, char *argument)
{
    int level;
    char arg[MAX_STRING_LENGTH];

    /* RT code for taking a level argument */
    one_argument (argument, arg);

    if (arg[0] == '\0')
        /* take the default path */

        if (ch->incog_level)
        {
            ch->incog_level = 0;
            act ("$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM);
            send_to_char ("You are no longer cloaked.\n\r", ch);
        }
        else
        {
            ch->incog_level = get_trust (ch);
            act ("$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM);
            send_to_char ("You cloak your presence.\n\r", ch);
        }
    else
        /* do the level thing */
    {
        level = atoi (arg);
        if (level < 2 || level > get_trust (ch))
        {
            send_to_char ("Incog level must be between 2 and your level.\n\r",
                          ch);
            return;
        }
        else
        {
            ch->oreply = NULL;
            ch->incog_level = level;
            act ("$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM);
            send_to_char ("You cloak your presence.\n\r", ch);
        }
    }

    return;
}



void do_holylight (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
        return;

    if (IS_SET (ch->act, PLR_HOLYLIGHT))
    {
        REMOVE_BIT (ch->act, PLR_HOLYLIGHT);
        send_to_char ("Holy light mode off.\n\r", ch);
    }
    else
    {
        SET_BIT (ch->act, PLR_HOLYLIGHT);
        send_to_char ("Holy light mode on.\n\r", ch);
    }

    return;
}

/* prefix command: it will put the string typed on each line typed */

void do_prefi (CHAR_DATA * ch, char *argument)
{
    send_to_char ("You cannot abbreviate the prefix command.\r\n", ch);
    return;
}

void do_prefix (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];

    if (argument[0] == '\0')
    {
        if (ch->prefix[0] == '\0')
        {
            send_to_char ("You have no prefix to clear.\r\n", ch);
            return;
        }

        send_to_char ("Prefix removed.\r\n", ch);
        free_string (ch->prefix);
        ch->prefix = str_dup ("");
        return;
    }

    if (ch->prefix[0] != '\0')
    {
        sprintf (buf, "Prefix changed to %s.\r\n", argument);
        free_string (ch->prefix);
    }
    else
    {
        sprintf (buf, "Prefix set to %s.\r\n", argument);
    }

    ch->prefix = str_dup (argument);
}

/*#define CH(descriptor)  ((descriptor)->original ? \
(descriptor)->original : (descriptor)->character)
*/

/* This file holds the copyover data */
#define COPYOVER_FILE "copyover.data"

/* This is the executable file */
#define EXE_FILE      "../src/sot"


/*  Copyover - Original idea: Fusion of MUD++
 *  Adapted to Diku by Erwin S. Andreasen, <erwin@pip.dknet.dk>
 *  http://pip.dknet.dk/~pip1773
 *  Changed into a ROM patch after seeing the 100th request for it :)
 */
 
void do_copyover (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    
    one_argument(argument, arg);
    if (arg[0] == '\0')
	{
	  send_to_char("Choose a number of tics between 0 - 10", ch);
	  return;
	}
	
    if (!strcmp(arg,"cancel"))
	{
	  iscopyover = FALSE;
          if (ch->invis_level < LEVEL_HERO)
    	    sprintf(buf,"`&Copyover cancelled by`# %s.`*", ch->name);
          else
    	    sprintf(buf,"`&Copyover cancelled by`# `!A`1n `!I`1m`1m`!o`!r`1t`!a`1l`7.`*");
	  do_function(ch, &do_echo, buf);
	  return;
	}

    if (atoi(arg) == 0)
	{
	  copyover(ch);
	  return;
	}
    copytimer = atoi(arg);
    copych = ch;
    iscopyover = TRUE;    
    if (ch->invis_level < LEVEL_HERO)
      sprintf(buf,"`&Copyover in `!%d `&tick%s started by `#%s.`*", copytimer, copytimer == 1 ? "" : "s", ch->name);
      
    else
      sprintf(buf,"`&Copyover in `!%d `&tick%s started by `#%s.`*",
             copytimer, copytimer == 1 ? "" : "s", "`!A`1n `!I`1m`1m`!o`!r`1t`!a`1l`7");
    do_function(ch, &do_echo, buf);
    
    return;
}

void copyover_update (void)
{
	char buf[MAX_STRING_LENGTH];

	if (copytimer > 0 && iscopyover == TRUE)
        {
	  copytimer--;
	  sprintf(buf,"`!%d `&tick%s before copyover.`*", copytimer, copytimer == 1 ? "" : "s");
	  do_function(copych, &do_echo, buf);
	}
	
	if (copytimer == 0 && iscopyover == TRUE)
	{
	  copyover (copych);
	}
	return;
} 
 
void copyover (CHAR_DATA * ch)
{
    FILE *fp;
    DESCRIPTOR_DATA *d, *d_next;
    char buf[100], buf2[100], buf3[100];
    extern int port, control;    /* db.c */
    int random;
 
    fp = fopen (COPYOVER_FILE, "w");

    do_restring(ch, "cancel");
    do_asave(ch, "changed");
    save_vote();
    save_mudstats();
    save_riddle();
    save_gameinfo();
    save_roster();
    save_guilds();
    save_equipid();
//    save_stores();
    save_groups();	
    mysql_close(mysql);
    ispell_done();
    abandon_tournament();

    if (!fp)
    {
        send_to_char ("Copyover file not writeable, aborted.\n\r", ch);
        logf ("Could not write to copyover file: %s", COPYOVER_FILE);
        perror ("do_copyover:fopen");
        return;
    }

    /* Consider changing all saved areas here, if you use OLC */

    /* do_asave (NULL, ""); - autosave changed areas */


    random = number_range(1, MAX_COPYOVER - 1);

    /* For each playing descriptor, save its state */
    for (d = descriptor_list; d; d = d_next)
    {
        CHAR_DATA *och = CH (d);
        d_next = d->next;        /* We delete from the list , so need to save this */

        if (d->connected == CON_PLAYING && !IS_NPC(och))
        {
          if (IS_IMMORTAL(och)) 
            sprintf (buf, "\n\r *** COPYOVER by %s - please remain seated!\n\r",
              ch->name);
          else
            sprintf(buf, copyover_table[random]);   
        }

        compressEnd(d);
        if (!d->character || d->connected < CON_PLAYING || d->connected == CON_BREAK_CONNECT)
        {                        /* drop those logging on */
            write_to_descriptor (d,
                                 "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r",
                                 0);
            close_socket (d);    /* throw'em out */
        }
        else
        {
           #ifdef THREADED_DNS
           fprintf (fp, "%d %s %d.%d.%d.%d\n",
               d->descriptor, och->name,
               d->Host[0],d->Host[1],d->Host[2],d->Host[3]);
           #else
             fprintf (fp, "%d %s %s\n", d->descriptor, och->name, dns_gethostname(d->host));
           #endif 
// Old before threaded fprintf (fp, "%d %s %s\n", d->descriptor, och->name, d->host);

#if 0                            /* This is not necessary for ROM */
            if (och->level == 1)
            {
                write_to_descriptor (d,
                                     "Since you are level one, and level one characters do not save, you gain a free level!\n\r",
                                     0);
                advance_level (och);
            }
#endif
            if (IS_DISGUISED (och))
                REM_DISGUISE(och);
            save_char_obj (och);

            write_to_descriptor (d, buf, 0);
        }
    }

    fprintf (fp, "-1\n");
    fclose (fp);

    /* Close reserve and other always-open files and release other resources */

    fclose (fpReserve);

    /* exec - descriptors are inherited */

    sprintf (buf, "%d", port);
    sprintf (buf2, "%d", control);
    sprintf (buf3, "/home/shadows/SoT/log/%ld.log 2>&1", current_time);
    #ifdef THREADED_DNS
   //    rdns_cache_destroy();
    #endif

    #ifdef DNS_SLAVE
      if ( slave_socket != -1)
      {
        log_string("Terminating DNS Slave process." );
        kill(slave_pid, SIGKILL );
        waitpid(slave_pid, NULL, 0);
      }
    #endif

    execl (EXE_FILE, "../src/sot", buf, "copyover", buf2, (char *) NULL);


    /* Failed - sucessful exec will not return */

    perror ("do_copyover: execl");
    #ifdef THREADED_DNS
        rdns_cache_init();
    #endif
    send_to_char ("Copyover FAILED!\n\r", ch);

    /* Here you might want to reopen fpReserve */
    fpReserve = fopen (NULL_FILE, "r");
}

/* Recover from a copyover - load players */
void copyover_recover ()
{
    DESCRIPTOR_DATA *d;
    FILE *fp;
    char name[100];
    int desc;
    bool fOld, found;
    CHAR_DATA *fch, *fch_next;
    #ifdef THREADED_DNS
        int h1,h2,h3,h4;
    #elif defined (DNS_SLAVE)
       char host[60];
    #else
        char host[MSL];
    #endif

    const   char    compress_on_str [] = { IAC, WILL, TELOPT_COMPRESS, '\0' }; 
    const   char   mxp_will        []={ IAC, WILL, TELOPT_MXP, '\0' };

    logf ("Copyover recovery initiated");

    fp = fopen (COPYOVER_FILE, "r");

    if (!fp)
    {                            /* there are some descriptors open which will hang forever then ? */
        perror ("copyover_recover:fopen");
        logf ("Copyover file not found. Exitting.\n\r");
        exit (1);
    }

    unlink (COPYOVER_FILE);        /* In case something crashes - doesn't prevent reading  */

    for (;;)
    {
//        fscanf (fp, "%d %s %s\n", &desc, name, host);
     
        #ifdef THREADED_DNS
            fscanf (fp, "%d %s %d.%d.%d.%d\n", &desc, name, &h1,&h2,&h3,&h4);
        #else
           fscanf (fp, "%d %s %s\n", &desc, name, host);
        #endif

        if (desc == -1)
            break;

        d = new_descriptor ();
        d->descriptor = desc;

        /* Write something, and check if it goes error-free */
        if (!write_to_descriptor
            (d, "\n\rRestoring from copyover...\n\r", 0))
        {
            close_socket (d);        /* nope */
            continue;
        }


        #ifdef THREADED_DNS
           d->Host[0]=h1; 
           d->Host[1]=h2;
           d->Host[2]=h3;
           d->Host[3]=h4;
        #elif defined (DNS_SLAVE)
           strcpy( d->host, host );
        #else
           free_string(d->Host);
           d->Host=str_dup(host);
        #endif
        d->next = descriptor_list;
        descriptor_list = d;
        d->connected = CON_COPYOVER_RECOVER;    /* -15, so close_socket frees the char */


        /* Now, find the pfile */

        fOld = load_char_obj (d, name);

        if (!fOld)
        {                        /* Player file not found?! */
            write_to_descriptor (d,
                                 "\n\rSomehow, your character was lost in the copyover. Sorry.\n\r",
                                 0);
            close_socket (d);
        }
        else
        {                        /* ok! */

            write_to_descriptor (d, "\n\rCopyover recovery complete.\n\r",
                                 0);

            /* Just In Case */
            if (!d->character->in_room)
                d->character->in_room = get_room_index (ROOM_VNUM_TEMPLE);

            found = FALSE;

            for (fch = char_list; fch != NULL; fch = fch_next)
            {
              fch_next = fch->next;

              if (IS_NPC(fch))
                 continue;
              
              if (d->character->id == fch->id)
              {
                found = TRUE;
                break;
              }
            }          
            
            if (found)
            {
              
              write_to_descriptor(d, "\n\rError you have been cloned, closing socket.\n\r", 0);         
              close_socket(d);
            }
            else
            {
              /* Insert in the char_list */
              d->character->next = char_list;
              char_list = d->character;
              REMOVE_BIT(d->character->act, PLR_QUESTOR);
              d->character->questgiver = NULL;
              d->character->countdown = 0;
              d->character->questmob = 0;
              d->character->questobj = 0;
              d->character->nextquest = 0;  

              char_to_room (d->character, d->character->in_room);
              do_look (d->character, "auto");
              act ("$n materializes!", d->character, NULL, NULL, TO_ROOM);
              d->connected = CON_PLAYING;
              mudstats.current_players += 1;
              if (mudstats.current_players > mudstats.current_max_players)
              {
                 mudstats.current_max_players = mudstats.current_players;
                 if (mudstats.total_max_players < mudstats.current_max_players)
                   mudstats.total_max_players = mudstats.current_players;
              }

              if (d->character->pet != NULL)
              {
                char_to_room (d->character->pet, d->character->in_room);
                act ("$n materializes!.", d->character->pet, NULL, NULL,
                     TO_ROOM);
              }
		        write_to_buffer(d, compress_on_str, 0);
		        write_to_buffer(d, mxp_will, 0);
                        read_from_buffer(d);
            }
        }

    }
       fclose (fp);
    load_groups();

}

void do_promote( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  int rank;
  
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  
  if (arg1[0] == '\0' && ch->clan >= 1)
  {
    sprintf(buf, "%s ranks:\n\r\n\r", clan_table[ch->clan].who_name);
    for (rank = 0;rank < clan_table[ch->clan].top_rank;rank++)
    {
      sprintf(buf, "%d: %s\n\r", rank + 1, clan_table[ch->clan].rank[rank]);
      send_to_char(buf, ch);
    }
    return;
  }
  
  if (ch->rank < (clan_table[ch->clan].top_rank - 2) && !IS_IMMORTAL(ch))
  {
     send_to_char("You don't have that authority.\n\r", ch);
     return;
  }

  if (arg2[0] == '\0' || !is_number(arg2))
  {
    send_to_char("Choose a rank between 1-6.\n\r", ch);
    return;
  }
  
  if ((victim = get_char_world(ch, arg1)) == NULL)
  {
    send_to_char("No such character in the world.\n\r", ch);
    return;
  }
  
  rank = atoi(arg2);
  if (rank < 1 || rank > clan_table[victim->clan].top_rank)
  {
    char buf[MIL];
    sprintf(buf, "Choose a rank between 1-%d.\n\r", 
	    clan_table[victim->clan].top_rank);
    send_to_char(buf, ch);
    return;
  }
  
  rank -= 1;

  if (!IS_IMMORTAL(ch) && rank >= ch->rank)
  {
    send_to_char("You can't promote to your level or higher.\n\r", ch);
    return;
  }
 
  if (ch->clan != victim->clan && !IS_IMMORTAL(ch))
  {
    send_to_char("But you aren't in the same guild as them!\n\r", ch);
    return;
  }
  
  if ((ch->rank < (clan_table[ch->clan].top_rank - 2) || victim->rank > ch->rank) && !IS_IMMORTAL(ch))
  {
    send_to_char("You can't do that.\n\r", ch);
    return;
  }
  
  victim->rank = rank;
  change_member(victim->name, (rank+1), victim->clan);
  save_roster();
  sprintf(buf, "%s has been promoted to %s.\n\r", victim->name, clan_table[victim->clan].rank[rank]);
  send_to_char(buf, ch);
  sprintf(buf, "You have been promoted to %s.\n\r", clan_table[victim->clan].rank[rank]);
  send_to_char(buf, victim);
  
}
  

long atoh(const char *hex)
{
 long ret = 0;

 while (*hex == 'x' || *hex == 'X' || *hex == ' ' || *hex == '0') hex++;

 while (*hex)
 { int place;
    switch (*hex) {

        case 'a':
            place = 0xa;
            break;
        case 'b':
            place = 0xb;
            break;
        case 'c':
            place = 0xc;
            break;
        case 'd':
            place = 0xd;
            break;
        case 'e':
            place = 0xe;
            break;
        case 'f':
            place = 0xf;
            break;
        case '1':
            place = 1;
            break;
        case '2':
            place = 2;
            break;
        case '3':
            place = 3;
            break;
        case '4':
            place = 4;
            break;
        case '5':
            place = 5;
            break;
        case '6':
            place = 6;
            break;
        case '7':
            place = 7;
            break;
        case '8':
            place = 8;
            break;
        case '9':
            place = 9;
            break;
        case '0':
            place = 0;
            break;
        default:
            place = -1;
    }

    if (place == -1) return -1;

    ret *= 16;
    ret += place;

    hex++;
 }

 return ret;
}

char *CHAR2(long a)
{
 static char buf[5000];

 sprintf(buf, "%ld", a);
 return buf;
}

void do_freevnums(CHAR_DATA *ch, char *argument)
{
 long iA, free = 0, start = 0, count = 0, needed = atoi(argument);
 AREA_DATA *are;
 long high_vnum = 65000;

 bool *block = (bool *) alloc_mem(sizeof(bool[high_vnum+1]));

 for (iA = 0; iA < high_vnum; iA++) block[iA] = 0;

 for (are = area_first; are; are = are->next)
 {
    for (iA = are->min_vnum; iA <= are->max_vnum; iA++)
	block[iA] = true;
 }

 for (iA = 0; iA <high_vnum; iA++)
 {
    if (!block[iA])
    {
      if (!free) start = iA;
        free++;
    }
    else if (free)
    {
      sprintf(log_buf, "VNUMS %7ld-%7ld    ", start, start+free);
      if (free >= needed)
      {
	stc(log_buf, ch);
	count++;
      }
      if (count % 3 == 2)
      {
	count = 0;
	stc("\n\r", ch);
      }
      start = 0;
      free = 0;
    }
 }

 free_mem (block, sizeof(bool[high_vnum+1]));

 stc("\n\r", ch);

 return;
}

void do_grant(CHAR_DATA *ch, char *argument)
{
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int cmd, trust;
  bool found = FALSE;

  argument = one_argument( argument, arg1);
  argument = one_argument( argument, arg2);

  if ( ( arg1[0] == '\0') || (arg2[0] == '\0') )
  {
    send_to_char("Syntax is:  grant < command > < name >\n\r", ch);
    send_to_char("       or:  grant show <name>\n\r", ch);
  }

  if (( victim = get_char_world( ch, arg2)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("Not on mobs.\n\r", ch);
    return;
  }

  if (!strcmp(arg1, "show"))
  {
    sprintf( buf, "\n\rCurrently granted commnds for %s are:\n\r",
             victim->name);
    send_to_char(buf, ch);
    sprintf( buf, victim->pcdata->granted);
    send_to_char(buf, ch);
    send_to_char("\n\r", ch);
    return;
  } 

  trust = get_trust(ch);

  for ( cmd = 0;cmd_table[cmd].name[0] != '\0';cmd++ )
  {
    if (arg1[0] == cmd_table[cmd].name[0]
       && !str_prefix( arg1, cmd_table[cmd].name )
       && cmd_table[cmd].level <= trust )
       {
         found = TRUE;
         break;
       }
  }

  if (!found)
  {
    send_to_char("No such command available.\n\r", ch);
    return;
  }

  buf[0] = '\0';

  if (victim->pcdata->granted != '\0')
  {
    strcat( buf, victim->pcdata->granted );
    strcat( buf, " ");
  }

  strcat( buf, cmd_table[cmd].name );
  free_string( victim->pcdata->granted );
  victim->pcdata->granted = ( str_dup( buf ));
  sprintf(buf, "%s granted to %s.\n\r", cmd_table[cmd].name, victim->name);
  send_to_char(buf, ch);
} 
/** Function: do_pload
  * Descr   : Loads a player object into the mud, bringing them (and their
  *           pet) to you for easy modification.  Player must not be connected.
  *           Note: be sure to send them back when your done with them.
  * Returns : (void)
  * Syntax  : pload (who)
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@dharvest.com>
  */
void do_pload( CHAR_DATA *ch, char *argument )
{
  DESCRIPTOR_DATA d;
  bool isChar = FALSE;
  char name[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  if (argument[0] == '\0')
  {
    send_to_char("Load who?\n\r", ch);
    return;
  }

  argument[0] = UPPER(argument[0]);
  argument = one_argument(argument, name);

  /* Dont want to load a second copy of a player who's allready online! */
  victim = get_char_world_special( name);
  if (victim != NULL)
  if (IS_NPC(victim))
        victim = NULL;
    
  if (victim != NULL)
  {
    send_to_char( "That person is already connected!\n\r", ch );
    return;
  }

  isChar = load_char_obj(&d, name); /* char pfile exists? */

  if (!isChar)
  {
    send_to_char("Load Who? Are you sure? I cant seem to find them.\n\r", ch);
    return;
  }

  d.character->desc     = NULL;
  d.character->next     = char_list;
  char_list             = d.character;
  d.connected           = CON_PLAYING;
  reset_char(d.character);

  /* bring player to imm */
  if ( d.character->in_room != NULL )
  {
    char_to_room( d.character, ch->in_room); /* put in room imm is in */
  }
  send_to_char("You have pulled that character from the pattern!.\n\r",ch);
  act( "$n has pulled $N from the pattern!",
        ch, NULL, d.character, TO_ROOM );

  if (d.character->pet != NULL)
   {
     char_to_room(d.character->pet,d.character->in_room);
     act("$n has entered the game.",d.character->pet,NULL,NULL,TO_ROOM);
   }

}

/** Function: do_punload
  * Descr   : Returns a player, previously 'ploaded' back to the void from
  *           whence they came.  This does not work if the player is actually 
  *           connected.
  * Returns : (void)
  * Syntax  : punload (who)
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@dharvest.com>
  */
void do_punload( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char who[MAX_INPUT_LENGTH];

  argument = one_argument(argument, who);

  if ( ( victim = get_char_world_special( who ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  /** Person is legitametly logged on... was not ploaded.
   */
  if (victim->desc != NULL)
  {
    send_to_char("I dont think that would be a good idea...\n\r", ch);
    return;
  }

  if (victim->was_in_room != NULL) /* return player and pet to orig room */
  {
    char_to_room(victim, victim->was_in_room);
    if (victim->pet != NULL)
      char_to_room(victim->pet, victim->was_in_room);
  }

  save_char_obj(victim);
  do_quit(victim,"");
  send_to_char("You have released that character back to the Pattern.\n\r",ch);
  act("$n has released $N back to the Pattern.",
       ch, NULL, victim, TO_ROOM);
}

void do_omni(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;

	if (IS_NPC(ch)) return;

	sprintf(buf, "        Name|Level|Secur|Class|  Hit| Mana| Move|  HR|  DR| Quest|Stat\n\r");
	send_to_char(buf,ch);
	sprintf(buf,
"------------|-----|-----|-----|-----|-----|-----|----|---|----|-----|\n\r");
	send_to_char(buf,ch);

	for (d = descriptor_list; d != NULL; d = d->next)
	{
		CHAR_DATA *wch;

		if (d->connected != CON_PLAYING) continue;
		wch = ( d->original != NULL) ? d->original : d->character;


sprintf(buf,"%12s|%5d|%5d|%5d|%5ld|%5d|%5d|%4d|%4d|%5ld|%4d\n\r",
		wch->name,
		wch->level,
		wch->pcdata->security,
		wch->cClass,
		wch->max_hit,
		wch->max_mana,
		wch->max_move,
		GET_HITROLL(wch),
		GET_DAMROLL(wch),
		wch->questpoints,
		wch->race
		);	


		send_to_char(buf,ch);
	}

	return;
}

/* Jasin */
void do_ungrant (CHAR_DATA *ch, char *argument) 
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);


    if (ch->level < 81)
    {
       send_to_char("Huh?\n\r", ch);
       return;
    }

    if ((victim = get_char_world(ch, arg2)) == NULL)
    {
       send_to_char("They aren't playing.\n\r", ch);
       return;
    }

    if (IS_NPC(victim))
    {
      send_to_char("Not on mobs.\n\r", ch);
      return;
    }

    if ((arg1[0] == '\0') || (arg2[0] == '\0'))
    {
       send_to_char("Syntax is: ungrant <command> <player>\n\r", ch);
       return;
    }

    if (strstr((victim->pcdata->granted), arg1))
    {       
       char *tmp = str_dup(victim->pcdata->granted);
       free_string(victim->pcdata->granted);
       victim->pcdata->granted = str_dup(str_remove(tmp, arg1));
       send_to_char("Command removed.\n\r", ch);
    }
    
    else
    {
       send_to_char("Command not found.\n\r", ch);
    }
}



void do_otransfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *chroom;
    ROOM_INDEX_DATA *objroom;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
 
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Otransfer which object?\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' ) victim = ch;
    else if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    

    
    else if (obj->in_obj     != NULL) obj_from_obj(obj);
    else if (obj->in_room != NULL)
    {
    	chroom = ch->in_room;
    	objroom = obj->in_room;
    	char_from_room(ch);
    	char_to_room(ch,objroom);
    	act("$p vanishes from the ground in an explosion of energy.",ch,obj,NULL,TO_ROOM);
	if (chroom == objroom) act("$p vanishes from the ground in an explosion of energy.",ch,obj,NULL,TO_CHAR);
    	char_from_room(ch);
    	char_to_room(ch,chroom);
	obj_from_room(obj);
    }
    else
    {
	send_to_char( "You were unable to get it.\n\r", ch );
	return;
    }
    obj_to_char(obj,victim);
    act("$p appears in your hands in an explosion of energy.",victim,obj,NULL,TO_CHAR);
    act("$p appears in $n's hands in an explosion of energy.",victim,obj,NULL,TO_ROOM);
    return;
}

void do_note_fix(CHAR_DATA *ch, char *argument)
{
    FILE *fp;
    char name[100];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool fOld;
    

    logf ("Note recovery initiated");

    fp = fopen ("fix.txt", "r");

    if (!fp)
    {                            /* there are some descriptors open which will hang forever then ? */
        perror ("note_recover:fopen");
        logf ("Note file not found. Exitting.\n\r");
        exit (1);
    }

    unlink ("fix.txt");        /* In case something crashes - doesn't prevent reading  */

    for (;;)
    {
        fscanf (fp, "%s\n", name);
        if (!str_cmp(name, "#"))
          break;
        d = new_descriptor();
        fOld = load_char_obj(d, capitalize(name));
        if (!fOld)
        {
          send_to_char("No file found", ch);
          return;
        }      
        victim = d->character;
        victim->next = char_list;
        char_list = victim;
        send_to_char("victim loaded", ch);
	      char_to_room(victim, ch->in_room);
        do_function(victim, &do_save, "");
        do_function(victim, &do_quit, "");
     }
     fclose(fp);
} 
void do_check( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    BUFFER *buffer;
    CHAR_DATA *victim;
    int count = 1;
    
    one_argument( argument, arg );
    
    if (arg[0] == '\0'|| !str_prefix(arg,"stats"))
    {
        buffer = new_buf();
    	for (victim = char_list; victim != NULL; victim = victim->next)
    	{
    	    if (IS_NPC(victim) || !can_see(ch,victim)) 
    	    	continue;
    	    	
	    if (victim->desc == NULL)
	    {
	    	sprintf(buf,"%3d) %s is linkdead.\n\r", count, victim->name);
	    	add_buf(buffer, buf);
	    	count++;
	    	continue;	    	
	    }
	    
	    if (victim->desc->connected >= CON_GET_NEW_RACE
	     && victim->desc->connected <= CON_PICK_WEAPON)
	    {
	    	sprintf(buf,"%3d) %s is being created.\n\r",
 	    	    count, victim->name);
	    	add_buf(buffer, buf);
	    	count++;
	    	continue;
	    }
	    
	    if ( (victim->desc->connected == CON_GET_OLD_PASSWORD
	       || victim->desc->connected >= CON_READ_IMOTD)
	     && get_trust(victim) <= get_trust(ch) )
	    {
	    	sprintf(buf,"%3d) %s is connecting.\n\r",
 	    	    count, victim->name);
	    	add_buf(buffer, buf);
	    	count++;
	    	continue; 	    		 
	    }
	    
	    if (victim->desc->connected == CON_PLAYING)
	    {
	        if (get_trust(victim) > get_trust(ch))
	            sprintf(buf,"%3d) %s.\n\r", count, victim->name);
	        else
	        {
		    sprintf(buf,"%3d) %s, Level %d connected since %d hours (%d total hours)\n\r",
		    	count, victim->name,victim->level,
                        ((int)(current_time - victim->logon)) /3600, 
		    	(victim->played + (int)(current_time - victim->logon)) /3600 );
		    add_buf(buffer, buf);
		    if (arg[0]!='\0' && !str_prefix(arg,"stats"))
		    {
		      sprintf(buf,"  %ld HP %d Mana (%d %d %d %d %d) %ld golds %d Tr %d Pr %ld Qpts.\n\r",
		    	victim->max_hit, victim->max_mana,victim->perm_stat[STAT_STR],
 victim->perm_stat[STAT_INT],victim->perm_stat[STAT_WIS],
 victim->perm_stat[STAT_DEX],victim->perm_stat[STAT_CON],
		    	victim->gold + victim->silver/100,
		    	victim->train, victim->practice, victim->questpoints);
		      add_buf(buffer, buf);
		    }
		    count++;
		}
	        continue;
	    }
	    
	    sprintf(buf,"%3d) bug (oops)...please report to Loran: %s %d\n\r",
	    	count, victim->name, victim->desc->connected);
	    add_buf(buffer, buf);
	    count++;   
    	}
    	page_to_char(buf_string(buffer),ch);
    	free_buf(buffer);
    	return;
    }
    
    if (!str_prefix(arg,"eq"))
    {
        buffer = new_buf();
    	for (victim = char_list; victim != NULL; victim = victim->next)
    	{
    	    if (IS_NPC(victim) 
    	     || victim->desc->connected != CON_PLAYING
    	     || !can_see(ch,victim)
    	     || get_trust(victim) > get_trust(ch) )
    	    	continue;
    	    	
    	    sprintf(buf,"%3d) %s, %d items (weight %d) Hit:%d Dam:%d Save:%d AC:%d %d %d %d.\n\r",
    	    	count, victim->name, victim->carry_number, victim->carry_weight, 
    	    	victim->hitroll, victim->damroll, victim->saving_throw,
    	    	victim->armor[AC_PIERCE], victim->armor[AC_BASH],
    	    	victim->armor[AC_SLASH], victim->armor[AC_EXOTIC]);
    	    add_buf(buffer, buf);
    	    count++;  
    	}
    	page_to_char(buf_string(buffer),ch);
    	free_buf(buffer);    	
    	return;
    }

  if (!str_prefix(arg,"snoop")) /* this part by jerome */
    {
        char bufsnoop [100];

        if(ch->level < MAX_LEVEL )
          {
            send_to_char("You can't use this check option.\n\r",ch);
            return;
          }
        buffer = new_buf();

        for (victim = char_list; victim != NULL; victim = victim->next)
        {
            if (IS_NPC(victim)
             || victim->desc->connected != CON_PLAYING
             || !can_see(ch,victim)
             || get_trust(victim) > get_trust(ch) )
                continue;

            if(victim->desc->snoop_by != NULL)
              sprintf(bufsnoop," %15s .",victim->desc->snoop_by->character->name);
            else
              sprintf(bufsnoop,"     (none)      ." );

            sprintf(buf,"%3d %15s : %s \n\r",count,victim->name, bufsnoop);
            add_buf(buffer, buf);
            count++;
        }
        page_to_char(buf_string(buffer),ch);
        free_buf(buffer);
        return;
    }

        
    send_to_char("Syntax: 'check'       display info about players\n\r",ch);
    send_to_char("        'check stats' display info and resume stats\n\r",ch);
    send_to_char("        'check eq'    resume eq of all players\n\r",ch);
    send_to_char("Use the stat command in case of doubt about someone...\n\r",ch);
    return;
}

void do_slot( CHAR_DATA *ch, char *argument )
{
    int num1;
    int num2;
    int num3;
    int slotwin;

    if ( IS_NPC( ch ) )
       return;

    if ( ch->gold < 5 )
    {
        send_to_char( "`$You need `&5`$ gold to play the slots.\n\r", ch );
        return;
    }

    ch->gold -= 5;

    
    send_to_char( "`&Spinning the slots! Cost - `&5`$ gold`&.\n\r", ch );
    pull_slot( ch );
    num1 = slotland;
    pull_slot( ch );
    num2 = slotland;
    pull_slot( ch );
    num3 = slotland;
    send_to_char( "\n\r\n\r", ch );

    if ( ( num1 == num2
         || num2 == num3 )
         && ( num1 != num3 ) )
    {
        if ( num1 == 9 || num2 == 9 || num3 == 9 )
        {
            send_to_char( "You pulled a loser! No winnings given!\n\r", ch );
            return;
        }
        send_to_char( "`&BING`# BING`@ BING`$ BING`^ BING`W!\n\r", ch );
        send_to_char( "`$You win `%15`$ coins!\n\r", ch );
        act( "$n has just won money playing slots!", ch, NULL, NULL, TO_ROOM );
        ch->gold += 15;
        return;
    }

    if ( num1 == num2
         && num2 == num3 )
    {
        switch( num1 )
        {
            case 0:
            slotwin = 80; break; /* Apple */
            case 1:
            slotwin = 100; break; /* Lime */
            case 2:
            slotwin = 250; break; /* Lucky Seven */
            case 3:
            slotwin = 75; break; /* Cherry */
            case 4:
            slotwin = 500; break; /* Gold Bar */
            case 5:
            slotwin = 25; break; /* Lemon */
            case 6:
            slotwin = 30; break; /* Rasberry */
            case 7:
            slotwin = 50; break; /* Kiwi */
            case 8:
            slotwin = 50; break; /* Banana */
            case 9:
            if ( ch->gold > 5000 )
            {
                slotwin = -500;
            }
            else
            {
                slotwin = -250;
            }
            break; /* Loser */
            default:
            slotwin = 0;
            send_to_char( "Machine is broken. Please contact an IMM.\n\r", ch );
            break;
        }

        ch->gold += slotwin;
        send_to_char( "`&BING`# BING`@ BING`% BING`^ BING`&!\n\r", ch );
        printf_to_char( ch, "`$You win `&%d`$ playing slots!\n\r", slotwin );
        act( "$n has just won money playing slots!", ch, NULL, NULL, TO_ROOM );
        return;
    }

    send_to_char( "`7You didn't win. Sorry.\n\r", ch );
    return;
}

/* Roll the slots for do_slot - Talon */
void pull_slot( CHAR_DATA *ch )
{
    slotland = ( number_range( 0, 9 ) );
    switch( slotland )
    {
        case 0:
        send_to_char( "`!Apple`7 ", ch ); break;
        case 1:
        send_to_char( "`2Lime`7 ", ch ); break;
        case 2:
        send_to_char( "`^Lucky-Seven`7 ", ch ); break;
        case 3:
        send_to_char( "`!C`1herry`7 ", ch ); break;
        case 4:
        send_to_char( "`&G`3old-`&B`3ar`7 ", ch ); break;
        case 5:
        send_to_char( "`#L`3emon`7 ", ch ); break;
        case 6:
        send_to_char( "`1Rasberry`7 ", ch ); break;
        case 7:
        send_to_char( "`@Kiwi`7 ", ch ); break;
        case 8:
        send_to_char( "`#Banana`7 ", ch ); break;
        case 9:
        send_to_char( "`!L`@O`#S`%E`5R`7 ", ch ); break;
        default:
        send_to_char( "`&Broken Machine`7 ", ch ); break;
        break;
    }

    return;
}
void do_tourneychar( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char buf[MSL];
  
  if (argument[0] == '\0')
  {
    send_to_char("Add who to the tourney.\n\r", ch);
    return;
  }
  
  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("But they aren't logged on.\n\r", ch);
    return;
  }
  
  if (IS_SET(victim->act, PLR_TOURNEY))
  {
    REMOVE_BIT(victim->act, PLR_TOURNEY);
    sprintf(buf, "%s has been removed from the tourney.\n\r", victim->name);
    send_to_char(buf, ch);
    send_to_char("You have been removed from the killing fields.\n\r", victim);
    return;
  }
  
  SET_BIT(victim->act, PLR_TOURNEY);
  sprintf(buf, "%s has been added to the tourney.\n\r", victim->name);
  send_to_char(buf, ch);
  send_to_char("You have been added to the killing fields!\n\r", victim);
  send_to_char("Lock and Load, its time to kick some fucking ass!\n\r", victim);
}
void do_warchar( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char buf[MSL];
  
  if (argument[0] == '\0')
  {
    send_to_char("Add who to the war?\n\r", ch);
    return;
  }
  
  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("But they aren't logged on.\n\r", ch);
    return;
  }
  
  if (IS_SET(victim->act2, PLR_WAR))
  {
    REMOVE_BIT(victim->act2, PLR_WAR);
    sprintf(buf, "%s has been removed from the war.\n\r", victim->name);
    send_to_char(buf, ch);
    send_to_char("You have been removed from the killing fields.\n\r", victim);
    return;
  }
  
  SET_BIT(victim->act2, PLR_WAR);
  sprintf(buf, "%s has been added to the war!\n\r", victim->name);
  send_to_char(buf, ch);
  send_to_char("You have been added to the killing fields!\n\r", victim);
  send_to_char("Lock and Load, its time to kick some fucking ass!\n\r", victim);
}
void do_tagchar( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char buf[MSL];
  
  if (argument[0] == '\0')
  {
    send_to_char("Add who to the tourney.\n\r", ch);
    return;
  }
  
  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("But they aren't logged on.\n\r", ch);
    return;
  }
  
  if (IS_SET(victim->act, PLR_TAG))
  {
    REMOVE_BIT(victim->act, PLR_TAG);
    sprintf(buf, "%s has been removed from the TAG.\n\r", victim->name);
    send_to_char(buf, ch);
    send_to_char("You have been removed from the tag game.\n\r", victim);
    return;
  }
  
  SET_BIT(victim->act, PLR_TAG);
  sprintf(buf, "%s has been added to the TAG.\n\r", victim->name);
  send_to_char(buf, ch);
  send_to_char("You have been added to the TAG game!\n\r", victim);
  send_to_char("Lock and Load, its time to kick some fucking ass!\n\r", victim);
}
void do_itchar( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char buf[MSL];
  
  if (argument[0] == '\0')
  {
    send_to_char("Add who to the tourney.\n\r", ch);
    return;
  }
  
  if ((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("But they aren't logged on.\n\r", ch);
    return;
  }
  
  if (IS_SET(victim->act, PLR_IT))
  {
    REMOVE_BIT(victim->act, PLR_IT);
    sprintf(buf, "%s has been removed from the TAG.\n\r", victim->name);
    send_to_char(buf, ch);
    send_to_char("You have been removed from the tag game.\n\r", victim);
    return;
  }
  
  SET_BIT(victim->act, PLR_IT);
  sprintf(buf, "%s has been added to the TAG.\n\r", victim->name);
  send_to_char(buf, ch);
  send_to_char("You have been added to the TAG game!\n\r", victim);
  send_to_char("Lock and Load, its time to kick some fucking ass!\n\r", victim);
}
/*RP grant code for points and experience- CAMM*/
void do_grantrp(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA		*victim;
	int				award;
	char            arg[MAX_STRING_LENGTH];
	char			arg1[MAX_STRING_LENGTH];
	char			arg2[MAX_STRING_LENGTH];
	char			buf[MAX_STRING_LENGTH];
	
	argument = one_argument (argument,arg);
	argument = one_argument (argument,arg1);
	argument = one_argument (argument,arg2);

	if (arg[0]=='\0' || arg1[0]=='\0'|| arg2[0]=='\0' )
	{
		send_to_char("Syntax: grantrp <character> <point/exp> <value>.\n\r",ch);
		return;
	}
	
	if((victim=get_char_world_special(arg))==NULL)
	{
		send_to_char("That character is not currently online.\n\r",ch);
		return;
	}

        if (IS_IMMORTAL(victim) && !IS_TRUSTED(ch, MAX_LEVEL))
        {
          send_to_char("You're not able to judge them.\n\r", ch);
          return;
        }
    
	if(IS_NPC(victim))
	{
		send_to_char("Mobiles cannot be awarded RP points!\n\r",ch);
		return;
	}
	if(!str_cmp(arg1,"point"))
	{
		if (!is_number (arg2))
		{
			send_to_char ("Value must be numeric.\n\r", ch);
			return;
	    }

	    award=atoi(arg2);
		if (award < -6 || award > 6)
		{
			send_to_char ("Award range is -5 to 5.\n\r", ch);
			return;
	    }
	
		if (award ==0)
		{
			send_to_char("Awarding 0 RP points is very pointless isn't it?\n\r",ch);
			return;
		}
		
		victim->rp_points += award;
		if (award > 0)
		{
			sprintf(buf,"You have awarded %d role-playing points to %s!\n\r",award,victim->name);
			send_to_char(buf,ch);
			sprintf(buf,"%s has awarded you with %d RP points for your great role-playing!\n\r",ch->name,award);
			send_to_char(buf,victim);
			sprintf(buf,"%s has awarded %d rp points to %s.\n\r",ch->name,award,victim->name);
		        wiznet (buf, NULL, NULL, WIZ_RP, 0, 0);
			return;
		}
		else
		{
			sprintf(buf,"You have removed %d role-playing points to %s!\n\r",award,victim->name);
			send_to_char(buf,ch);
			sprintf(buf,"%s has removed %d RP points as a punishment!\n\r",ch->name,award);
			send_to_char(buf,victim);
			sprintf(buf,"%s has awarded %d rp points to %s.\n\r",ch->name,award,victim->name);
		wiznet (buf, NULL, NULL, WIZ_RP, 0, 0);
			return;
		}
	}
	else if(!str_cmp(arg1,"exp"))
	{
		if (!is_number (arg2))
		{
			send_to_char ("Value must be numeric.\n\r", ch);
			return;
	    }

	    award=atoi(arg2);
		if (award < -501 || award > 501)
		{
			send_to_char ("Award range is -500 to 500.\n\r", ch);
			return;
	    }
	
		if (award ==0)
		{
			send_to_char("Awarding 0 RP experience points is very pointless isn't it?\n\r",ch);
			return;
		}
		rpgain_exp(victim,award);
		if (award > 0)
		{
			sprintf(buf,"You have awarded %d role-playing experience to %s!\n\r",award,victim->name);
			send_to_char(buf,ch);
			sprintf(buf,"%s has awarded you with %d RP experience for your great role-playing!\n\r",ch->name,award);
			send_to_char(buf,victim);
			sprintf(buf,"%s has awarded %d rp exp to %s.\n\r",ch->name,award,victim->name);
		wiznet (buf, NULL, NULL, WIZ_RP, 0, 0);
			return;
		}
		else
		{
			sprintf(buf,"You have removed %d role-playing experience to %s!\n\r",award,victim->name);
			send_to_char(buf,ch);
			sprintf(buf,"%s has removed %d RP experience as a punishment!\n\r",ch->name,award);
			send_to_char(buf,victim);
			sprintf(buf,"%s has awarded %d rp exp to %s.\n\r",ch->name,award,victim->name);
		wiznet (buf, NULL, NULL, WIZ_RP, 0, 0);
			return;
		}
		
		return;
	}
	else
	{
		send_to_char ("Syntax: grantrp <character> <point/exp> <value>.\n\r",ch);
		return;
	}
}
void do_sever( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
       send_to_char("Who are you going to rip in half?\n\r",ch);
       return; 
    }
    
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL ) 
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if(victim == ch)
    {
	send_to_char("You reach your neck fine, but cant' get your ankles.\n\r",ch);
	return;
    }
    
    if(IS_IMMORTAL(victim) && ch->level <= victim->level && str_cmp (ch->name,"Cammaedros"))
    {
    	send_to_char("You can't to that.\n\r",ch);
    	return;
    }


    if(IS_SET(victim->affected_by2,AFF_SEVERED))
    {
	send_to_char("They have no legs to rip off.\n\r",ch);
	return;
    }
	victim->position = POS_RESTING;
    STR_SET_BIT(victim->affected_by,AFF_SEVERED);
    if(!IS_NPC(victim))
    act("$n picks you up and rips you in half! Oh no!",ch,NULL,victim,TO_VICT);
    act("$n picks up $N and rips $S legs off!",ch,NULL,victim,TO_NOTVICT);
    send_to_char("You rip them in half!\n\r",ch);

    obj = create_object( get_obj_index(OBJ_VNUM_LEGS ), 0 );

    obj->timer = 0;

   
if(IS_NPC(victim))
    sprintf(buf,"A pair of %s's `!b`1loody`* legs are here, twitching.",victim->short_descr);
else
    sprintf(buf,"A pair of %s's `!b`1loody`* legs are here, twitching.",victim->name);	
    free_string(obj->description);
    obj->description = str_dup( buf );


if(IS_NPC(victim))
    sprintf(buf,"a pair of %s's `!b`1loody`* legs",victim->short_descr);
else
    sprintf(buf,"A pair of %s's `!b`1loody`* legs",victim->name);    
    free_string(obj->short_descr);
    obj->short_descr = str_dup( buf );

    obj_to_char(obj,ch);

}
void do_unrestore( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    DESCRIPTOR_DATA *d;

    one_argument( argument, arg );
    if (arg[0] == '\0' || !str_cmp(arg,"room"))
    {
    /* uncure room */
    	
        for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
        {
            victim = vch;
            victim->hit  = victim->max_hit - victim->max_hit + 1;
            victim->mana = victim->max_mana - victim->max_mana + 1;
            victim->move = victim->max_move - victim->max_move + 1;
            /* Untested part of the code, it supposedly will restore you, minus your
            stats to zero, then give you 1 more hp, the complete anti-mort code :) */
            update_pos( vch);
            act("$n has unrestored you.",ch,NULL,vch,TO_VICT);
        }

        sprintf(buf,"$N unrestored room %ld.",ch->in_room->vnum);
        wiznet(buf,ch,NULL,WIZ_RESTORE,WIZ_SECURE,get_trust(ch));
        
        send_to_char("Room unrestored.\n\r",ch);
        return;

    }
    
    if ( get_trust(ch) >=  MAX_LEVEL - 1 && !str_cmp(arg,"all"))
    {
    /* cure all */
    	
        for (d = descriptor_list; d != NULL; d = d->next)
        {
	    victim = d->character;

	    if (victim == NULL || IS_NPC(victim))
		continue;
                
           victim->hit  = victim->max_hit - victim->max_hit + 1;
           victim->mana = victim->max_mana - victim->max_mana + 1;
           victim->move = victim->max_move - victim->max_move + 1;
            update_pos( victim);
	    if (victim->in_room != NULL)
                act("$n has unrestored you.",ch,NULL,victim,TO_VICT);
        }
	send_to_char("All active players unrestored.\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    victim->hit  = victim->max_hit - victim->max_hit + 1;
    victim->mana = victim->max_mana - victim->max_mana + 1;
    victim->move = victim->max_move - victim->max_move + 1;
    update_pos( victim );
    act( "$n has unrestored you.", ch, NULL, victim, TO_VICT );
    sprintf(buf,"$N unrestored %s",
	IS_NPC(victim) ? victim->short_descr : victim->name);
    wiznet(buf,ch,NULL,WIZ_RESTORE,WIZ_SECURE,get_trust(ch));
    send_to_char( "Ok.\n\r", ch );
    return;
}
void do_rpadvance (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;
    int iLevel;
	
    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0' || !is_number (arg2))
    {
        send_to_char ("Syntax: rpadvance <char> <level>.\n\rMake sure that the char has max stats while doing this.", ch);
        return;
    }

    if ((victim = get_char_world (ch, arg1)) == NULL)
    {
        send_to_char ("That player is not here.\n\r", ch);
        return;
    }

    if (IS_NPC (victim))
    {
        send_to_char ("Not on NPC's.\n\r", ch);
        return;
    }

    if ((level = atoi (arg2)) < 0 || level > MAX_RPLEVEL)
    {
        sprintf (buf, "Level must be 0 to %d.\n\r", MAX_RPLEVEL);
        send_to_char (buf, ch);
        return;
    }

    if (level > get_trust (ch))
    {
        send_to_char ("Limited to your trust level.\n\r", ch);
        return;
    }

    if (level <= victim->rplevel)
    {
        send_to_char ("Lowering a player's RP level!\n\r", ch);
        send_to_char ("**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r", victim);
		for (iLevel = victim->rplevel; iLevel > level; iLevel--)
		{
			rpadvance_level (victim,FALSE, FALSE);
		}
		victim->rpexp = 0;
    }
    else
    {
        send_to_char ("Raising a player's RP level!\n\r", ch);
        send_to_char ("**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim);
    }

    for (iLevel = victim->rplevel; iLevel < level; iLevel++)
    {
        rpadvance_level (victim,TRUE, FALSE);
    }
	victim->rpexp = 0;
    sprintf (buf, "You are now at RP level %d.\n\r", victim->rplevel);
    send_to_char (buf, victim);
    
	victim->trust = 0;
    save_char_obj (victim);
    return;
}
void do_joinwar (CHAR_DATA *ch, char *argument)
{
	
    if IS_SET(ch->act2, PLR_WAR)
    {
    	send_to_char("You are already a part of the war!\n\r", ch);
    	return;
    }
    if (ch->level <= 10)
    {
    	send_to_char("You must be level 11+ to join the war!\n\r", ch);
    	return;
}
    if (ch->clan == (clan_lookup("None") || (ch->clan == (clan_lookup("Tinker")))))
    {
    	send_to_char("You are not allowed to join the war!\n\r", ch);
    	return;
}
    if (iswar != TRUE)
    {
    	send_to_char("No war has been planed at this time!\n\r", ch);
    	return;
    }	
    
    SET_BIT(ch->act2, PLR_WAR);
    send_to_char("You have `!entered`7 the war!\n\r", ch);
    return;
}
void do_leavewar (CHAR_DATA *ch, char *argument)
{
	
    if (!IS_SET(ch->act2, PLR_WAR))
    {
    	send_to_char("You cant leave a war your not in!\n\r", ch);
    	return;
    }
    if (ch->pcdata->pk_timer > 0)
    {
    	send_to_char("You cant leave, you are in PK ticks!\n\r", ch);
    	return;
    }
    if (iswar == TRUE)
    {
    	send_to_char("Why leave the war? It has not even started!\n\r", ch);
    	return;
    }	
    
    REMOVE_BIT(ch->act2, PLR_WAR);
    send_to_char("You have `$abandoned`7 the war! COWARD\n\r", ch);
    return;
    }
    
  

void do_startwar (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    
  warch = ch;
    one_argument(argument, arg);
    if (arg[0] == '\0')
	{
	  send_to_char("Choose a number of tics between 0 - 100000", ch);
	  return;
	}
	
    if (!strcmp(arg,"cancel"))
	{
	  iswar = FALSE;
          
    	    sprintf(buf,"`&The `!War`& has been called off!`*");
                    do_function(ch, &do_echo, buf);
	  return;
	}

    if (atoi(arg) == 0)
	{
	  
	  return;
	}
    wartimer = atoi(arg);
    iswar = TRUE;    
    sprintf(buf,"`&A `!War`& has been declared by`# %s.`*", ch->name);
    do_function(ch, &do_echo, buf);
    return;
}
void warticks_update (void)
{
	char buf[MAX_STRING_LENGTH];
	
	
		
	if (wartimer >= 1 && iswar == TRUE)
        {
	  wartimer--;
	  sprintf(buf,"`&You have `!%d `&tick%s before the war starts.`*", wartimer, wartimer == 1 ? "" : "s");
	  do_echo(warch, buf);
	}
	
	else if (wartimer <= 0 && iswar == TRUE)
	
	{
	 sprintf(buf,"`!Let the slaughter commence!`*");
	 do_echo(warch, buf);
	 iswar = FALSE;
	 return;
	}
	return;
} 

void save_groups(void)
{
  DESCRIPTOR_DATA *d;
  FILE *fp;

  if ((fp = fopen(GROUP_FILE, "w")) == NULL)
  {
    bug("save_groups: fopen", 0);
  }

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *victim = CH(d);
 
    if (!d->character || d->connected != CON_PLAYING)
      continue;

    if (victim->leader == NULL)
      continue;
    
    fprintf(fp, "Leader %s~\nFollow %s~\n", victim->leader->name, victim->name);
    fprintf(fp, "#\n");
  }
  fprintf(fp, "$\n");
  fclose(fp);
}
    
void load_groups(void)
{
  FILE *fp;
  char *word;
  char *leader;
  char *follower;
  CHAR_DATA *ch, *victim;


  if ((fp = fopen(GROUP_FILE, "r")) == NULL)
  {
    bug("save_groups: fopen", 0);
  }

  
  word = fread_word(fp);

  if (word[0] == '$')
   return;

  leader = NULL;
  follower = NULL;
 
  while (word[0] != '$')
  {
    if (!str_cmp(word, "#"))
    {
      if ((ch = get_char_world_special(leader)) == NULL)
      {
        word = feof(fp) ? str_dup("$") : fread_word(fp);	
        logf("leader", 0);
        continue;
      }
      if ((victim = get_char_world_special(follower)) == NULL)
      {
         word = feof(fp) ? str_dup("$") : fread_word(fp);
         logf("fol", 0);
         continue;
      }
      victim->leader = ch;
    }
   
    if (!str_cmp(word, "Leader"))
    {
      leader = fread_string(fp);
    }

    if (!str_cmp(word, "Follow"))
    {
      follower = fread_string(fp);
    }
    word = feof(fp) ? str_dup("$") : fread_word(fp);
  }  

  fclose(fp);
  if ((fp = fopen(GROUP_FILE, "w")) == NULL)
  {
    bug("save_groups: fopen", 0);
  }
  fprintf(fp, "$\n");
  fclose(fp);
}
void do_guide_offense (CHAR_DATA * ch, char * argument)
{
  CHAR_DATA *victim;
  char arg1[MIL];
  char arg2[MIL];
  char buf [MSL];
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (!IS_NPC(ch) && ch->clan != clan_lookup("Guide") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }

  if (ch->clan == clan_lookup("Guide") && ch->rank <= 2)
  {
    send_to_char("You do not have sufficent rank to use this yet.\n\r",ch);
    return;
  }

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("Syntax: offense <char> <add|remove>\n\r",ch);
    return;
  }

  if ((victim = get_char_world_special(arg1)) == NULL)
  {
    send_to_char("They are not online.\n\r",ch);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char ("Not on mobiles.\n\r",ch);
    return;
  }
  
  if (!str_cmp(arg2,"show"))
  {
    sprintf(buf, "Player: %s  Punishments: `!%d`*", victim->name, victim->penalty.punishment);
    send_to_char(buf, ch);
  }

  else if (!str_cmp(arg2,"add"))
  {
    victim->penalty.punishment++;
    sprintf(buf,"Your punishment counter has been increased. [`!%i punishments`*]\n\r",victim->penalty.punishment);
    send_to_char(buf, victim);
    sprintf(buf, "You increased %s's punishment counter to `!%i`*.\n\r", victim->name, victim->penalty.punishment);
    send_to_char(buf, ch);
    sprintf(buf, "%s of Guide has increased %s's punishment counter\n",ch->name,victim->name);
    system_note("Auto Guide",NOTE_NEWS,"Guide IMP","ATTN Guide Player: Punishment counter has been increased!",buf);
 }
    
  else if (!str_cmp(arg2,"remove"))
  {
    victim->penalty.punishment--;
    sprintf(buf,"Your punishment counter has been decreased. [`!%i punishments`*]\n\r",victim->penalty.punishment);
    send_to_char(buf, victim);
    sprintf(buf, "You decreased %s's punishment counter to `!%i`*.\n\r", victim->name, victim->penalty.punishment);
    send_to_char(buf, ch);
    sprintf(buf, "%s of Guide has decreased %s's punishment counter\n",ch->name,victim->name);
    system_note("Auto Guide",NOTE_NEWS,"Guide IMP","ATTN Guide Player: Punishment counter has been decreased!",buf);
  }
    
  else
  {
    send_to_char("Syntax: offense <char> <add|remove>\n\r",ch);
    return;
  }

return;
}

void do_guide_flag (CHAR_DATA * ch, char * argument)
{
  CHAR_DATA *victim;
  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];
  char buf [MSL];
  int value;
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);

  if (!IS_NPC(ch) && ch->clan != clan_lookup("Guide") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }

  if (ch->rank <= 2)
  {
    send_to_char("You do not have sufficent rank to use this yet.\n\r",ch);
    return;
  }

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("Syntax: gflag <char> <murder|thief> (#ticks)\n\r",ch);
    return;
  }

  if (arg3[0] == '\0')
    value = 200;
  else
    value = atoi(arg3);

  if ((victim = get_char_world_special(arg1)) == NULL)
  {
    send_to_char("They are not online.\n\r",ch);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char ("Not on mobiles.\n\r",ch);
    return;
  }

  if (!str_cmp(arg2,"murder"))
  {
    victim->penalty.murder += value;
    sprintf(buf,"You have been found guilty of murder by %s\n",ch->name);
    send_to_char(buf,victim);
    send_to_char ("Flag added. `!Don't forget to add an offense if needed with the 'offense' command.`*\n\r", ch);
    sprintf(buf,"%s of Guide has found %s guilty of murder! %i ticks!\n",ch->name,victim->name, value);
    system_note("Auto Guide",NOTE_NEWS,"Guide IMP","ATTN Guide Player: Punishment has been rendered",buf);
    victim->hit -= 10;
    victim->mana -= 10;
    victim->move -= 10;
  }
    
  else if (!str_cmp(arg2,"thief"))
  {
    victim->penalty.thief += value;
    sprintf(buf,"You have been found guilty of thievery by %s\n",ch->name);
    send_to_char(buf,victim);
    send_to_char ("Flag added. `!Don't forget to add an offense if needed with the 'offense' command.`*\n\r", ch);
    sprintf(buf,"%s of Guide has found %s guilty of thievery! %i ticks!\n",ch->name,victim->name, value);
    system_note("Auto Guide",NOTE_NEWS,"Guide IMP","ATTN Guide Player: Punishment has been rendered",buf);
    victim->hit -= 5;
    victim->mana -= 5;
    victim->move -= 5;
  }
    
  else
  {
    send_to_char("Syntax: gflag <char> <murder|thief> (#ticks)\n\r",ch);
    return;
  }

return;
}

void do_guide_jail (CHAR_DATA * ch, char * argument)
{
  ROOM_INDEX_DATA *location;
  CHAR_DATA *victim;
  char arg1[MIL];
  char buf[MSL];
  argument = one_argument (argument,arg1);

  if (!IS_NPC(ch)&&ch->clan != clan_lookup("Guide") && !IS_IMMORTAL(ch))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }

  if (ch->rank < 2)
  {
    send_to_char("You do not have sufficent rank to use this yet.\n\r",ch);
    return;
  }

  if (arg1[0] == '\0')
  {
    send_to_char("Syntax: gjail <char>\n\r",ch);
    return;
  }

  if ((victim = get_char_world_special(arg1)) == NULL)
  {
    send_to_char("They are not online.\n\r",ch);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char ("Not on mobiles.\n\r",ch);
    return;
  }

  if ((location = get_room_index (ROOM_VNUM_JAIL)) == NULL)
  {
    send_to_char ("Jail room not availible.\n\r", ch);
    if (IS_HIIMMORTAL(ch))
      send_to_char ("ROOM_VNUM_JAIL is an invalid room.\n\r", ch);
    return;
  }

  if (victim->fighting != NULL)
    stop_fighting (victim, TRUE);

  victim->penalty.jail += 240;
  sprintf (buf,"%s chains up %s and drags him away to jail!", ch->name,victim->name);
  act (buf,victim,NULL,NULL,TO_ROOM);
  char_from_room (victim);
  char_to_room (victim, location);
  act ("$n arrives in chains.", victim, NULL, NULL, TO_ROOM);
  if (ch != victim)
    act ("$n chains you and drags you into jail!", ch, NULL, victim, TO_VICT);
  do_function (victim, &do_look, "auto");
  send_to_char ("Jailed. `!Don't forget to add an offense if needed with the 'offense' command.`*\n\r", ch);
  if (!IS_SET(victim->comm, COMM_NOCHANNELS))
  {
    SET_BIT (victim->comm, COMM_NOCHANNELS);
    send_to_char ("As part of your punishment, your channel privileges have been revoked.\n\r", victim);
  }
 
  sprintf(buf,"Due to excessive misconduct by %s has been jailed and nochanned for further review.\n",victim->name);
  system_note("Auto Guide",NOTE_NEWS,"Guide IMP","ATTN Guide Player: Player has been jailed",buf);
  return;
}
/*
void do_guide_flag (CHAR_DATA * ch, char * argument)
{
    CHAR_DATA *victim;
    char arg1[MIL];
    char arg2[MIL];
    char buf [MSL];
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    
    if (!IS_NPC(ch)&& ch->clan != clan_lookup("Guide") && !IS_IMMORTAL(ch))
    {
    	send_to_char("Huh?\n\r",ch);
    	return;
    }

    if (ch->rank < 2)
    {
    	send_to_char("You do not have sufficent rank to use this yet.\n\r",ch);
    	return;
    }
    
    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
    	send_to_char("Syntax: gflag <char> <murder/thief>\n\r",ch);
    	return;
    }
    if ((victim = get_char_world_special(arg1)) == NULL)
    {
    	send_to_char("They are not online.\n\r",ch);
    	return;
    }
    if (IS_NPC(victim))
    {
    	send_to_char ("Not on mobiles.\n\r",ch);
    	return;
    }


    if (!str_cmp(arg2,"murder"))
    {
    	victim->penalty.murder += 240;
    	sprintf(buf,"You have been found guilty of murder by %s\n",ch->name);
    	send_to_char(buf,victim);
    	sprintf(buf,"%s of Guide has found %s guilty of murder!\n",ch->name,victim->name);
    	system_note("Auto Guide",NOTE_NEWS,"Guide IMP","ATTN Guide Player: Punishment has been rendered",buf);
        victim->hit -= 10;
        victim->mana -= 10;
        victim->move -= 10;
    }
    else if (!str_cmp(arg2,"thief"))
    {
    	victim->penalty.thief += 240;
    	sprintf(buf,"You have been found guilty of thievery by %s\n",ch->name);
    	send_to_char(buf,victim);    	
    	sprintf(buf,"%s of Guide has found %s guilty of thievery!\n",ch->name,victim->name);
    	system_note("Auto Guide",NOTE_NEWS,"Guide IMP","ATTN Guide Player: Punishment has been rendered",buf);
        victim->hit -= 5;
        victim->mana -= 5;
        victim->move -= 5;
    }
    else
    {
    	send_to_char("Syntax: gflag <char> <murder/thief>\n\r",ch);
    	return;
    
    }

    return;
}
void do_guide_jail (CHAR_DATA * ch, char * argument)
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim;
    char arg1[MIL];

    char buf[MSL];
    argument = one_argument (argument,arg1);

    if (!IS_NPC(ch)&&ch->clan != clan_lookup("Guide") && !IS_IMMORTAL(ch))
    {
    	send_to_char("Huh?\n\r",ch);
    	return;
    }
    if (ch->rank < 2)
    {
    	send_to_char("You do not have sufficent rank to use this yet.\n\r",ch);
    	return;
    }

    if (arg1[0] == '\0')
    {
    	send_to_char("Syntax: gjail <char>\n\r",ch);
    	return;
    }
    if ((victim = get_char_world_special(arg1)) == NULL)
    {
    	send_to_char("They are not online.\n\r",ch);
    	return;
    }
    
    if (IS_NPC(victim))
    {
    	send_to_char ("Not on mobiles.\n\r",ch);
    	return;
    }
    if (!IS_MURDERER(victim)&&!IS_THIEF(victim))
    {
    	send_to_char ("This person has commited no crime.\n\r",ch);
    	return;
    }
    if ((location = get_room_index (ROOM_VNUM_JAIL)) == NULL)
        {
            send_to_char ("No such location.\n\r", ch);
            return;
        }
    if (victim->fighting != NULL)
        stop_fighting (victim, TRUE);
    sprintf (buf,"%s chains up %s and drags him away to jail!", ch->name,victim->name);
    act (buf,victim,NULL,NULL,TO_ROOM);
    char_from_room (victim);
    char_to_room (victim, location);
    act ("$n arrives in chains.", victim, NULL, NULL, TO_ROOM);
    if (ch != victim)
        act ("$n chains you and drags you into jail!", ch, NULL, victim, TO_VICT);
    do_function (victim, &do_look, "auto");
    send_to_char ("Ok.\n\r", ch);
    if (!IS_SET(victim->comm, COMM_NOCHANNELS))
    {
        SET_BIT (victim->comm, COMM_NOCHANNELS);
        send_to_char ("For your misconduct Guide has revoked your channel privileges.\n\r",
                      victim);
    }
    sprintf(buf,"Due to excessive misconduct by %s has been jailed and nochanned for further review.\n",victim->name);
    system_note("Auto Guide",NOTE_NEWS,"Guide IMP","ATTN Guide Player: Player has been jailed",buf);
    return;
}
*/
void do_lab_monkey (CHAR_DATA * ch, char * argument)
{
    char arg1 [MIL];

    argument = one_argument(argument,arg1);

    if (arg1[0]=='\0')
    {
    	send_to_char ("This is not a game? Why?!? Because I can change the code at any given point of time notice to reflect anything I really wished.\n",ch);
    	return;
    }
    if (!str_cmp(arg1, "blah1"))
    {
    	send_to_char ("Do you know that I'm written in C++?? Are you tellin me that I can't code in C++ on a remote server? That people cannot help me test my code?\n",ch);
    	return;
    }
    if (!str_cmp(arg1, "blah2"))
    {
    	send_to_char ("Now.. If you said that I cannot further my education by doing what is known as recreational programming. Then I demand that every programmer to be banned from this lab. IMMEDIATELY. Only then will I leave quietly.\n",ch);
    	return;
    }
    if (!str_cmp(arg1, "blah3"))
    {
    	send_to_char ("Look. I go through this every year and its getting tiring. I'm not wasting any more time and effort coding in little messages to leave for you. Unless you WANT me to prove once again that I can change this code to suit your satifaction.\n",ch);
    	return;
    }
    

    return;

}

void do_nstat (CHAR_DATA * ch, char *argument)  /* Jasin */
{

    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int gn, col;

    col = 0;

    one_argument(argument, arg);

    if (ch->clan != clan_lookup("Guide") && !IS_IMMORTAL(ch))
    {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    if (arg[0] == '\0')
    {
	send_to_char("Who do you want to stat?\n\r", ch);
	return;
    }

    victim = get_char_world(ch, argument);
    if (victim == NULL)
    {
	send_to_char("You cannot find that player to stat.\n\r", ch);
	return;
    }

    if (IS_NPC(victim))
    {
	send_to_char("Not on mobs.\n\r", ch);
	return;
    }

	send_to_char("`8-----------------------------------------------------------------`7\n\r", ch);
    	sprintf (buf, "`&Name`7: %s  `&Race`7: %s  `&Sex`7: %s `&Room`7: %ld\n\r",
		IS_DISGUISED(victim)?victim->pcdata->disguise.orig_name:victim->name,
		race_table[victim->race].name,
		sex_table[victim->sex].name,
		victim->in_room == NULL ? 0 : victim->in_room->vnum);
	send_to_char (buf, ch);

	sprintf (buf,
		"`&Level`7: %d  `&RP Level`7: %d  `&Class`7: %s  `&Align`7: %d  `&Gold`7: %ld  `&Silver`7: %ld\n\r",
		victim->level, 
		victim->rplevel,
		class_table[victim->cClass].name,
		victim->alignment,
		victim->gold, victim->silver);
	send_to_char (buf, ch);

	send_to_char ("`8-----------------------------------------------------------------`7 \n\r", ch);  
	sprintf (buf, "`&Clan`7: %s  `&Rank`7: %s\n\r",
		clan_table[victim->clan].who_name,
		clan_table[victim->clan].rank[victim->rank]);
	send_to_char (buf, ch);

	sprintf (buf, 
		"`&Hp`7: %ld/%ld  `&Mana`7: %d/%d  `&Movement`7: %d/%d  `&Trains`7: %d  `&Pracs`7: %d\n\r",
		victim->hit, victim->max_hit,
		victim->mana, victim->max_mana,
		victim->move, victim->max_move,
		victim->train, victim->practice);
	send_to_char (buf, ch);

	send_to_char ("`8-----------------------------------------------------------------`7 \n\r", ch);
	sprintf (buf, "`&Age`7: %d  `&Hours Played`7: %d\n\r", get_age (victim),
		(int) (victim->played + current_time - victim->logon) / 3600);
	send_to_char (buf, ch);

	sprintf (buf,
	    	"`&Str`7: %d`8(`7%d`8)  `&Int`7: %d`8(`7%d`8)  `&Wis`7: %d`8(`7%d`8)  `&Dex`7: %d`8(`7%d`8)  `&Con`7: %d`8(`7%d`8)\n\r",
		pc_race_table[ch->race].stats[STAT_STR],
		get_curr_stat (victim, STAT_STR),
		pc_race_table[ch->race].stats[STAT_INT],
		get_curr_stat (victim, STAT_INT),
		pc_race_table[ch->race].stats[STAT_WIS],
		get_curr_stat (victim, STAT_WIS),
		pc_race_table[ch->race].stats[STAT_DEX],
		get_curr_stat (victim, STAT_DEX),
		pc_race_table[ch->race].stats[STAT_CON],
		get_curr_stat (victim, STAT_CON));
	send_to_char (buf, ch);

	send_to_char ("`8-----------------------------------------------------------------`7 \n\r", ch);
	sprintf (buf, "`&AC`7: `&pierce`7: %d  `&bash`7: %d  `&slash`7: %d  `&magic`7: %d\n\r",
		GET_AC (victim, AC_PIERCE), GET_AC (victim, AC_BASH),
		GET_AC (victim, AC_SLASH), GET_AC (victim, AC_EXOTIC));
	send_to_char (buf, ch);

	sprintf (buf, "`&Hitroll`7: %d  `&Damroll`7: %d  `&Saves`7: %d\n\r",
		GET_HITROLL (victim), GET_DAMROLL (victim),
		victim->saving_throw);
	send_to_char (buf, ch);
	send_to_char ("`8-----------------------------------------------------------------`7 \n\r", ch);
	
	for (gn = 0; gn < MAX_GROUP; gn++)
	    {
		if (group_table[gn].name == NULL)
		    break;
	        if (victim->pcdata->group_known[gn])
	    	{
		    sprintf (buf, "`&%-20s`7", group_table[gn].name);
		        send_to_char (buf, ch);
	    	    if (++col % 3 == 0)
		        send_to_char ("\n\r", ch);
		}
	    }

	    if (col % 3 != 0)
		send_to_char ("\n\r\n\r", ch);

	    sprintf (buf, "`&Creation points`7: %d\n\r", victim->pcdata->points);
	    send_to_char (buf, ch);
	    sprintf (buf, "`&Quest points`7: %ld\n\r", victim->questpoints);
	    send_to_char (buf, ch);
	    send_to_char ("`8-----------------------------------------------------------------`7\n\r", ch);

	return;
   
}

void append_todo( CHAR_DATA *ch, char *file, char *str )
{
    FILE *fp;
    char *strtime;
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) || str[0] == '\0' )
        return;

    strtime                    = ctime( &current_time );
    strtime[strlen(strtime)-9] = '\0';
    strncpy(buf,strtime+4,6);
    buf[6] = '\0';

    if ( ( fp = fopen( file, "a" ) ) == NULL )
    {
        perror( file );
        send_to_char( "Could not open the file!\n\r", ch );
    } else {
        if( !str_cmp( file, BUG_FILE ) || !str_cmp( file, TYPO_FILE ) )
        {
            int vnum;

            vnum = ch->in_room ? ch->in_room->vnum : 0;
            fprintf( fp, "[`*%-10s - Room %d`$]`3 %s`*`#:`7 %s\n",
                ch->name, vnum, buf, str );
        }

        else
            fprintf( fp, "[`7%-10s`$]`3 %s`*`3:`* %s\n", ch->name, buf, str );

        fclose( fp );
    }
}

void parse_todo( CHAR_DATA *ch, char *argument, char *command, char *name, char *fname )
{
    FILE *fp;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];

    if ( argument[0] == '\0' )
    {
        printf_to_char(ch,"Syntax:  %s add <item>\n\r", command);
        printf_to_char(ch,"         %s show\n\r", command);
        printf_to_char(ch,"         %s delete <line no>\n\r", command);
        printf_to_char(ch,"         %s clear\n\r", command);
        return;
    }

    argument = one_argument(argument,arg);

    if( !str_cmp(arg,"show") || !str_cmp(arg,"list"))
        {
                char *line;
                int i;
                char c = '[';
                BUFFER *output = new_buf();

                fp = fopen(fname,"r");
                if( fp == NULL )
                {
                        printf_to_char(ch,"The %s is empty.\n\r",name);
                        return;
                }

                i = 1;
                while( !feof(fp))
                {
                line = fread_string_eol(fp);
                sprintf(buf,"`!%2d`1)`7 `$%s\n\r",i++,line);
                add_buf(output,buf);//send_to_char(buf,ch);
                free_string(line);

                while( !feof(fp) && (c = fread_letter(fp)) != '[' )
                        ;
                if( !feof(fp))
                        ungetc(c,fp);
                }
                page_to_char(output->string,ch);
                free_buf(output);
                fclose(fp);
                return;
        }

        if( !str_cmp(arg,"delete"))
        {
                FILE *fout;
                char *line;
                int i, num;
                bool found = FALSE;
                bool empty = TRUE;
                char c = '[';

                one_argument(argument,arg);

                if( arg[0] == '\0' || !is_number(arg))
                {
                        printf_to_char(ch,"Syntax: %s delete <line number>\n\r",command);
                        return;
                }

                num = atoi(arg);

                fp = fopen(fname,"r");
                if( fp == NULL )
                {
                        printf_to_char(ch,"The %s is empty.\n\r",name);
                        return;
                }

                fout = fopen("../data/temp.dat","w");
                if( fout == NULL )
                {
                        fclose(fp);
                        send_to_char("Error opening temporary file.\n\r",ch);
                        return;
                }

                i = 1;
                while( !feof(fp))
                {
                        line = fread_string_eol(fp);
                        if( i++ != num )
                        {
                                empty = FALSE;
                                fprintf(fout,"%s\n\r",line);
                        }
                        else
                        {
                                found = TRUE;
                        }
                        free_string(line);

                        while( !feof(fp) && (c = fread_letter(fp)) != '[' )
                                ;
                        if( !feof(fp))
                                ungetc(c,fp);
                }

                fclose(fout);
                fclose(fp);

                if( empty )
                {
                printf_to_char(ch,"The %s has been cleared.\n\r",name);
                unlink(fname);
                return;
                }

                if( !found )
                {
                        send_to_char("That line number does not exist.\n\r",ch);
                }
                else
                {
#if defined(WIN32)
                        unlink(fname);
#endif
                        rename("../data/temp.dat",fname);
                        unlink("../data/temp.dat");
                        printf_to_char(ch,"Line deleted from %s.\n\r",name);
                }

                return;
        }

        if( !str_cmp(arg,"clear"))
        {
                fp = fopen(fname,"r");
                if( fp == NULL )
                {
                        printf_to_char(ch,"The %s is empty.\n\r",name);
                        return;
                }

                fclose(fp);
                printf_to_char(ch,"The %s has been cleared.\n\r",name);
                unlink(fname);
                return;
        }

        if( !str_cmp(arg,"add"))
        {
    append_todo(ch, fname, argument);
    printf_to_char(ch,"Line added to the %s.\n\r",name);
        return;
        }

        //** Show syntax.
        parse_todo(ch,"",command,name,fname);
    return;
}

void do_todo( CHAR_DATA *ch, char *argument )
{
        parse_todo(ch,argument,"todo","todo list",TO_DO_FILE);
    return;
}

void do_tocode( CHAR_DATA *ch, char *argument )
{
        parse_todo(ch,argument,"tocode","tocode list",TO_CODE_FILE);
    return;
}

void do_agenda( CHAR_DATA *ch, char *argument )
{
        parse_todo(ch,argument,"agenda","agenda",AGENDA_FILE);
    return;
}

void do_changed( CHAR_DATA *ch, char *argument )
{
        parse_todo(ch,argument,"changed","changed list",CHANGED_FILE);
    return;
}

void do_bug( CHAR_DATA *ch, char *argument )
{
        if( IS_IMMORTAL(ch))
        {
        parse_todo(ch, argument, "bug", "bug list", BUG_FILE);
        return;
        }

    append_todo(ch, BUG_FILE, argument);
    send_to_char("Bug logged.\n\r",ch);
    return;
}

void do_typo( CHAR_DATA *ch, char *argument )
{
        if( IS_IMMORTAL(ch))
        {
        parse_todo(ch, argument, "typo", "typo list", TYPO_FILE);
        return;
        }

    append_todo(ch, TYPO_FILE, argument);
    send_to_char("Typo logged.\n\r",ch);
    return;
}

void do_arealinks(CHAR_DATA *ch, char *argument)
{
    FILE *fp;
    BUFFER *buffer;
    AREA_DATA *parea;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *from_room;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int vnum = 0;
    int iHash, door;
    bool found = FALSE;
    
    buffer = new_buf();
    /* To provide a convenient way to translate door numbers to words */
    static char * const dir_name[] = {"north","east","south","west","up","down"};

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    /* First, the 'all' option */
    if (!str_cmp(arg1,"all"))
    {
	/*
	 * If a filename was provided, try to open it for writing
	 * If that fails, just spit output to the screen.
	 */
	if (arg2[0] != '\0')
	{
	    fclose(fpReserve);
	    if( (fp = fopen(arg2, "w")) == NULL)
	    {
		send_to_char("Error opening file, printing to screen.\n\r",ch);
		fclose(fp);
		fpReserve = fopen(NULL_FILE, "r");
		fp = NULL;
	    }
	}
	else
	    fp = NULL;

	/* Open a buffer if it's to be output to the screen */
	if (!fp)
	    buffer = new_buf();

	/* Loop through all the areas */
	for (parea = area_first; parea != NULL; parea = parea->next)
	{
	    /* First things, add area name  and vnums to the buffer */
	    sprintf(buf, "*** %s (%ld to %ld) ***\n\r",
			 parea->name, parea->min_vnum, parea->max_vnum);
	    fp ? fprintf(fp, buf) : add_buf(buffer, buf);

	    /* Now let's start looping through all the rooms. */
	    found = FALSE;
	    for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	    {
		for( from_room = room_index_hash[iHash];
		     from_room != NULL;
		     from_room = from_room->next )
		{
		    /*
		     * If the room isn't in the current area,
		     * then skip it, not interested.
		     */
		    if ( from_room->vnum < parea->min_vnum
		    ||   from_room->vnum > parea->max_vnum )
			continue;

		    /* Aha, room is in the area, lets check all directions */
		    for (door = 0; door < 6; door++)
		    {
			/* Does an exit exist in this direction? */
			if( (pexit = from_room->exit[door]) != NULL )
			{
			    to_room = pexit->u1.to_room;

			    /*
			     * If the exit links to a different area
			     * then add it to the buffer/file
			     */
			    if( to_room != NULL
			    &&  (to_room->vnum < parea->min_vnum
			    ||   to_room->vnum > parea->max_vnum) )
			    {
				found = TRUE;
				sprintf(buf, "    (%ld) links %s to %s (%ld)\n\r",
				    from_room->vnum, dir_name[door],
				    to_room->area->name, to_room->vnum);

				/* Add to either buffer or file */
				if(fp == NULL)
				    add_buf(buffer, buf);
				else
				    fprintf(fp, buf);
			    }
			}
		    }
		}
	    }

	    /* Informative message for areas with no external links */
	    if (!found)
		add_buf(buffer, "    No links to other areas found.\n\r");
	}

	/* Send the buffer to the player */
	if (!fp)
	{
	    page_to_char(buf_string(buffer), ch);
	    free_buf(buffer);
	}
	/* Or just clean up file stuff */
	else
	{
	    fclose(fp);
	    fpReserve = fopen(NULL_FILE, "r");
	}

	return;
    }

    /* No argument, let's grab the char's current area */
    if(arg1[0] == '\0')
    {
	parea = ch->in_room ? ch->in_room->area : NULL;

	/* In case something wierd is going on, bail */
	if (parea == NULL)
	{
	    send_to_char("You aren't in an area right now, funky.\n\r",ch);
	    return;
	}
    }
    /* Room vnum provided, so lets go find the area it belongs to */
    else if(is_number(arg1))
    {
	vnum = atoi(arg1);

	/* Hah! No funny vnums! I saw you trying to break it... */
	if (vnum <= 0 || vnum > 65536)
	{
	    send_to_char("The vnum must be between 1 and 65536.\n\r",ch);
	    return;
	}

	/* Search the areas for the appropriate vnum range */
	for (parea = area_first; parea != NULL; parea = parea->next)
	{
	    if(vnum >= parea->min_vnum && vnum <= parea->max_vnum)
		break;
	}

	/* Whoops, vnum not contained in any area */
	if (parea == NULL)
	{
	    send_to_char("There is no area containing that vnum.\n\r",ch);
	    return;
	}
    }
    /* Non-number argument, must be trying for an area name */
    else
    {
	/* Loop the areas, compare the name to argument */
	for(parea = area_first; parea != NULL; parea = parea->next)
	{
	    if(!str_prefix(arg1, parea->name))
		break;
	}

	/* Sorry chum, you picked a goofy name */
	if (parea == NULL)
	{
	    send_to_char("There is no such area.\n\r",ch);
	    return;
	}
    }

    /* Just like in all, trying to fix up the file if provided */
    if (arg2[0] != '\0')
    {
	fclose(fpReserve);
	if( (fp = fopen(arg2, "w")) == NULL)
	{
	    send_to_char("Error opening file, printing to screen.\n\r",ch);
	    fclose(fp);
	    fpReserve = fopen(NULL_FILE, "r");
	    fp = NULL;
	}
    }
    else
	fp = NULL;

    /* And we loop the rooms */
    for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
    {
	for( from_room = room_index_hash[iHash];
	     from_room != NULL;
	     from_room = from_room->next )
	{
	    /* Gotta make sure the room belongs to the desired area */
	    if ( from_room->vnum < parea->min_vnum
	    ||   from_room->vnum > parea->max_vnum )
		continue;

	    /* Room's good, let's check all the directions for exits */
	    for (door = 0; door < 6; door++)
	    {
		if( (pexit = from_room->exit[door]) != NULL )
		{
		    to_room = pexit->u1.to_room;

		    /* Found an exit, does it lead to a different area? */
		    if( to_room != NULL
		    &&  (to_room->vnum < parea->min_vnum
		    ||   to_room->vnum > parea->max_vnum) )
		    {
			found = TRUE;
			sprintf(buf, "%s (%ld) links %s to %s (%ld)\n\r",
				    parea->name, from_room->vnum, dir_name[door],
				    to_room->area->name, to_room->vnum);

			/* File or buffer output? */
			if(fp == NULL)
			    send_to_char(buf, ch);
			else
			    fprintf(fp, buf);
		    }
		}
	    }
	}
    }

    /* Informative message telling you it's not externally linked */
    if(!found)
    {
	send_to_char("No links to other areas found.\n\r",ch);
	/* Let's just delete the file if no links found */
	if (fp)
        {
	    unlink(arg2);
            fclose(fp);
            fpReserve = fopen(NULL_FILE, "r");
        }
	return;
    }

    /* Close up and clean up file stuff */
    if(fp)
    {
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
    }

}

void do_olevel(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];
    char level[MAX_INPUT_LENGTH];
    char name[MAX_INPUT_LENGTH];
    BUFFER *buffer;
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;
    int number = 0, max_found;

    found = FALSE;
    number = 0;
    max_found = 200;

    buffer = new_buf();

    argument = one_argument(argument, level);
    if (level[0] == '\0')
    {
        send_to_char("Syntax: olevel <level>\n\r",ch);
        send_to_char("        olevel <level> <name>\n\r",ch);
        return;
    }
 
    argument = one_argument(argument, name);
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
        if ( obj->level != atoi(level) )
            continue;

	if ( name[0] != '\0' && !is_name(name, obj->name) )
	    continue;

        found = TRUE;
        number++;
 
        for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj );
 
        if ( in_obj->carried_by != NULL && can_see(ch,in_obj->carried_by)
        &&   in_obj->carried_by->in_room != NULL)
            sprintf( buf, "%3d) %s is carried by %s [Room %ld]\n\r",
                number, obj->short_descr,PERS(in_obj->carried_by, ch, TRUE),
                in_obj->carried_by->in_room->vnum );
        else if (in_obj->in_room != NULL && can_see_room(ch,in_obj->in_room))
            sprintf( buf, "%3d) %s is in %s [Room %ld]\n\r",
                number, obj->short_descr,in_obj->in_room->name, 
                in_obj->in_room->vnum);
        else
            sprintf( buf, "%3d) %s is somewhere\n\r",number, obj->short_descr);
 
        buf[0] = UPPER(buf[0]);
        add_buf(buffer,buf);
 
        if (number >= max_found)
            break;
    }
 
    if ( !found )
        send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
    else
        page_to_char(buf_string(buffer),ch);

    free_buf(buffer);
}

void do_mlevel( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    BUFFER *buffer;
    CHAR_DATA *victim;
    bool found;
    int count = 0;

    if ( argument[0] == '\0' )
    {
	send_to_char("Syntax: mlevel <level>\n\r",ch);
	return;
    }

    found = FALSE;
    buffer = new_buf();
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
        if ( victim->in_room != NULL
        &&   atoi(argument) == victim->level )
        {
            found = TRUE;
            count++;
            sprintf( buf, "%3d) [%5ld] %-28s [%5ld] %s\n\r", count,
                IS_NPC(victim) ? victim->pIndexData->vnum : 0,
                IS_NPC(victim) ? victim->short_descr : victim->name,
                victim->in_room->vnum,
                victim->in_room->name );
            add_buf(buffer,buf);
        }
    }

    if ( !found )
        act( "You didn't find any mob of level $T.", ch, NULL, argument, TO_CHAR );
    else
        page_to_char(buf_string(buffer),ch);

    free_buf(buffer);

    return;
}

void do_award(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
 
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  
  argument = one_argument(argument, arg);
  argument = one_argument(argument, arg1);
  
  if ( arg[0] == '\0' || arg1[0] == '\0'  )
  {
	send_to_char("Syntax: award <rp, train, practice, gold, qp> <player>\n\r",ch);
	return;
  }
  
  if ((victim = get_char_world (ch, arg1)) == NULL)
  {
    send_to_char ("They aren't here.\n\r", ch);
    return;
  }
      
  if (!str_cmp(arg, "rp"))
  {
  	victim->rp_points += 1;
	sprintf(buf, "%s has been awarded an RP point.", victim->name);
	send_to_char(buf, ch);
	send_to_char("You have been awarded an RP point for good roleplaying!", victim);
	return;
  }
  else if (!str_cmp(arg, "train"))
  {
  	victim->train += 1;
	sprintf(buf, "%s has been awarded a train.", victim->name);
	send_to_char(buf, ch);
	send_to_char("You have been awarded a train!", victim);
	return;
  }
  else if (!str_cmp(arg, "practice"))
  {
  	victim->practice += 3;
	sprintf(buf, "%s has been awarded 3 practices.", victim->name);
	send_to_char(buf, ch);
	send_to_char("You have been awarded 3 practices!", victim);
	return;
  }
  else if (!str_cmp(arg, "gold"))
  {
  	victim->gold += 1000;
	sprintf(buf, "%s has been awarded 1000 gold.", victim->name);
	send_to_char(buf, ch);
	send_to_char("You have been awarded 1000 gold!", victim);
	return;
  }
  else if (!str_cmp(arg, "qp"))
  {
  	victim->questpoints += 50;
	sprintf(buf, "%s has been awarded 50 quest points.", victim->name);
	send_to_char(buf, ch);
	send_to_char("You have been awarded 50 quest points!", victim);
	return;
  }
    
  send_to_char("Syntax: award <rp, train, prac, gold, qp> <player>\n\r\n\r", ch);
  return;
}
