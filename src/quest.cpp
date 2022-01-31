#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "quest.h"

bool is_safe_silent args ((CHAR_DATA *ch, CHAR_DATA *victim)); 
DECLARE_DO_FUN( do_tell );
DECLARE_DO_FUN( do_tell );
DECLARE_DO_FUN( do_restore );
/* Object vnums for object quest 'tokens'. In Moongate, the tokens are
   things like 'the Shield of Moongate', 'the Sceptre of Moongate'. These
   items are worthless and have the rot-death flag, as they are placed
   into the world when a player receives an object quest. */

#define QUEST_OBJQUEST1 8326
#define QUEST_OBJQUEST2 8327
#define QUEST_OBJQUEST3 8328
#define QUEST_OBJQUEST4 8329
#define QUEST_OBJQUEST5 8330
#define QUEST_OBJQUEST6 6414
#define QUEST_OBJQUEST7 6415
#define QUEST_OBJQUEST8 6416
#define QUEST_OBJQUEST9 6417
#define QUEST_OBJQUEST10 6418
#define QUEST_OBJQUEST11 6419
#define QUEST_OBJQUEST12 6420
#define QUEST_OBJQUEST13 6421
#define QUEST_OBJQUEST14 6422
#define QUEST_OBJQUEST15 6423
#define QUEST_OBJQUEST16 8331
#define QUEST_OBJQUEST17 8332
#define QUEST_OBJQUEST18 8333
#define QUEST_OBJQUEST19 8334
#define QUEST_OBJQUEST20 8335
#define QUEST_OBJQUEST21 8336
#define QUEST_OBJQUEST22 8337
#define QUEST_OBJQUEST23 8338
#define QUEST_OBJQUEST24 8339
#define QUEST_OBJQUEST25 8340
#define QUEST_OBJQUEST26 8341

/* Local functions */

void generate_quest	args(( CHAR_DATA *ch, CHAR_DATA *questman ));
void generate_rescue_quest  args(( CHAR_DATA *ch, CHAR_DATA *questman, int number, bool test ));
void quest_update	args(( void ));
bool chance		args(( int num ));
bool riddle_lookup      args(( RIDDLE_DATA *riddle, char *guess));
void riddle_say      args(( RIDDLE_DATA *riddle, CHAR_DATA *questman, CHAR_DATA *ch));
char *getline        args((char *str, char *buf));
void show_obj_stat      args(( CHAR_DATA *ch, OBJ_DATA *obj));      
void member_quest       args(( CHAR_DATA *ch, int points));
void abort_rescue       args (( CHAR_DATA *ch, bool bPullAttacker ));


 
ROOM_INDEX_DATA *find_location_vnum args ((CHAR_DATA * ch, int vnum));
ROOM_INDEX_DATA *find_location args ((CHAR_DATA * ch, char *arg));

/* CHANCE function. I use this everywhere in my code, very handy :> */

bool chance(int num)
{
    if (number_range(1,100) <= num) return TRUE;
    else return FALSE;
}

/* The main quest function */

bool inlist(int choice, int upto)
{
   int j;

   for (j = 0; j < upto ; j++)
   {
     if (quest_list[j] == choice)
       return TRUE;
   }
   return FALSE;
}

void initialize_list()
{
  int i;
  int max = 0;
  int item;
  
  for (i = 0; i < MAX_LIST; i++)
  {
    quest_list[i] = -1;
  }
  
  for (i = 2; i < MAX_LIST; i++)
  {
   
    item = number_range(0, top_qprize - 1);

    if (quest_list[i] == 4)
	break;
    
    if (quest_list[i] == 27)
	break;

    while (inlist(item, i))
    {
      item = number_range(0, top_qprize - 1);
      max++;
      if (max >= 5000)
        break;
    }
    quest_list[i] = item;
    quest_list[0] = 4;    
    quest_list[1] = 27;
  }
} 

void do_quest(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *questman;
    OBJ_DATA *obj=NULL, *obj_next;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;
    char buf [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    int i;

    argument = one_argument(argument, arg1);

    if (IS_NPC(ch))
    {
      send_to_char("Not with NPCs\n\r", ch);
      return;
    }
    
    if (!strcmp(arg1, "info"))
    {
	if (IS_SET(ch->act, PLR_QUESTOR))
	{
	    if (ch->questmob == -1 && ch->questgiver->short_descr != NULL)
	    {
		sprintf(buf, "Your quest is ALMOST complete!\n\rGet back to %s before your time runs out!\n\r",ch->questgiver->short_descr);
		send_to_char(buf, ch);
	    }
	    else if (ch->questobj > 0)
	    {
                questinfoobj = get_obj_index(ch->questobj);
		if (questinfoobj != NULL)
		{
		    sprintf(buf, "You are on a quest to recover the fabled %s!\n\r",questinfoobj->name);
		    send_to_char(buf, ch);
                    if (!IS_NULLSTR(ch->pcdata->questarea) && !IS_NULLSTR(ch->pcdata->questroom))
                    sprintf(buf, "You were told it was in %s near %s\n\r",
                         ch->pcdata->questarea,
                         ch->pcdata->questroom);
                    send_to_char(buf, ch);
		}
		else send_to_char("You aren't currently on a quest.\n\r",ch);
		return;
	    }
	    else if (ch->questmob > 0)
	    {
                questinfo = get_mob_index(ch->questmob);
		if (questinfo != NULL)
		{
	            sprintf(buf, "You are on a quest to slay the dreaded %s!\n\r",questinfo->short_descr);
		    send_to_char(buf, ch);
		}
		else send_to_char("You aren't currently on a quest.\n\r",ch);
		return;
	    }
	    else if (ch->questriddle > 0)
            {
              RIDDLE_DATA *riddle;
              if ((riddle = get_riddle_index(ch->questriddle)) == NULL)
              {
                sprintf(buf, "You aren't currently on a quest.\n\r");
                send_to_char(buf, ch);
              }
              else
              {
	    	send_to_char("Riddle:\n\r", ch);
            	send_to_char(riddle->question, ch);
		send_to_char("What am I?\n\r", ch);
              }  
            }
	}
	else
	    send_to_char("You aren't currently on a quest.\n\r",ch);
	return;
    }
    if (!strcmp(arg1, "points"))
    {
	sprintf(buf, "You have %ld quest points.\n\r",ch->questpoints);
	send_to_char(buf, ch);
	return;
    }
    else if (!strcmp(arg1, "time"))
    {
	if (!IS_SET(ch->act, PLR_QUESTOR))
	{
	    send_to_char("You aren't currently on a quest.\n\r",ch);
	    if (ch->nextquest > 1)
	    {
		sprintf(buf, "There are %d minutes remaining until you can go on another quest.\n\r",ch->nextquest);
		send_to_char(buf, ch);
	    }
	    else if (ch->nextquest == 1)
	    {
		sprintf(buf, "There is less than a minute remaining until you can go on another quest.\n\r");
		send_to_char(buf, ch);
	    }
	}
        else if (ch->countdown > 0)
        {
	    sprintf(buf, "Time left for current quest: %d\n\r",ch->countdown);
	    send_to_char(buf, ch);
	}
	return;
    }

/* Checks for a character in the room with spec_questmaster set. This special
   procedure must be defined in special.c. You could instead use an 
   ACT_QUESTMASTER flag instead of a special procedure. */

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

    if ( questman->fighting != NULL)
    {
	send_to_char("Wait until the fighting stops.\n\r",ch);
        return;
    }


/* And, of course, you will need to change the following lines for YOUR
   quest item information. Quest items on Moongate are unbalanced, very
   very nice items, and no one has one yet, because it takes awhile to
   build up quest points :> Make the item worth their while. */

    if (!strcmp(arg1, "list"))
    {
       char level[10];
        act( "$n asks $N for a list of quest items.", ch, NULL, questman, TO_ROOM); 
	act ("You ask $N for a list of quest items.", ch, NULL, questman, TO_CHAR);
	send_to_char("Current Quest Items available for Purchase:\n\r", ch); 
        send_to_char("  Cost - Level      Name             \n\r", ch);     
        for (i = 0; i < MAX_LIST; i++)
        {
          sprintf(level, "%d", qprize_table[quest_list[i]].level);
          sprintf(buf, "  %4d -  %3s - %s`*\n\r", qprize_table[quest_list[i]].cost,
                      qprize_table[quest_list[i]].level == 0 ? "N/A" : 
			level,
		        qprize_table[quest_list[i]].showname);
          send_to_char(buf, ch);
        }  
        send_to_char("To buy an item, type 'QUEST BUY <item>'.\n\r", ch);
	return;
    }
    else if (!strcmp(arg1, "stat"))
    {
      OBJ_DATA *obj;
      int i = 0;
      int prize = 0;
      bool found = FALSE;

      argument = one_argument(argument, arg2);

      for (i = 0; i < MAX_LIST; i++)
      {
        if (!str_prefix(arg2, qprize_table[quest_list[i]].keyword))
        {
          found = TRUE;
          prize = i;
          break;
        }
      }
        
      if (!found)
      {
        send_to_char("That is not a valid option.", ch);
        return;
      }

      if (!qprize_table[quest_list[prize]].special)
      {
        obj = create_object(get_obj_index(qprize_table[quest_list[prize]].vnum),ch->level);
 
        if (obj != NULL)
        {
          send_to_char("\n\r", ch);
          show_obj_stat(ch, obj);
          extract_obj(obj);
        }
      }
      else
      {
        send_to_char("No stats on that object.\n\r", ch);
        return;
      }
      return;
    }
    else if (!strcmp(arg1, "turnin"))
    {
        OBJ_DATA *obj;
        int  item, index;
        OBJ_INDEX_DATA *pObjIndex;	
        char buf[MSL];
        char tmp[MIL];
        char newarg[MIL];
        int prize;

        argument = one_argument(argument, newarg);
        if (!str_cmp(newarg, "token"))
        {
          if (( obj = get_obj_carry( ch, "token", ch)) == NULL)
          {
            send_to_char("No token found.\n\r", ch);
            return;
          }

	  if (obj->item_type != ITEM_TOKEN)
	  {
            send_to_char("Item found is not a token, try putting everything else in a bag.\n\r", ch);
            return;
	  }

          act( "$n turns a token in to $N.", ch, NULL, questman, TO_ROOM); 
	  act ("You turnin your token to $N.", ch, NULL, questman, TO_CHAR);
 
	  prize = obj->value[0];
          ch->questpoints += prize;
	  sprintf(tmp, "%s gives you %d points for the token.\n\r", questman->short_descr, prize); 
          logf("QP TURNIN: %s turned in quest token worth %d", ch->name, prize);
          send_to_char(tmp, ch);
          obj_from_char(obj);
          extract_obj(obj);
	  return;
        } 

        if (!str_cmp(newarg, "fix"))
        {
          OBJ_DATA *fixobj;

          if (( obj = get_obj_carry( ch, "fix coupon", ch)) == NULL)
          {
            send_to_char("You don't have a coupon to turn in.\n\r", ch);
            return;
          }
                    
	  if (obj->pIndexData->vnum != 6428)
          {
            send_to_char("That's not a fix coupon.\n\r", ch);
            return;
	  } 

          if ((fixobj = get_obj_carry(ch, argument, ch)) == NULL)
          {
            send_to_char("You're not carrying the item to be fixed.\n\r", ch);
            return;
          }

          if (fixobj->condition >= 0)
          {
            send_to_char("That item is not destroyed.\n\r", ch);
            return;
          }

          fixobj->condition = 80;
          extract_obj(obj);
          send_to_char("Item fixed\n\r", ch);
          return;
        }

        if (!str_cmp(newarg, "nobreak"))
        {
          OBJ_DATA *fixobj;

          if (( obj = get_obj_carry( ch, "no_break coupon", ch)) == NULL)
          {
            send_to_char("You don't have a coupon to turn in.\n\r", ch);
            return;
          }
                    
	  if (obj->pIndexData->vnum != 6411)
          {
            send_to_char("That's not a nobreak coupon.\n\r", ch);
            return;
	  } 

          if ((fixobj = get_obj_carry(ch, argument, ch)) == NULL)
          {
            send_to_char("You're not carrying the item to be flagged.\n\r", ch);
            return;
          }

          if (IS_OBJ_STAT(fixobj, ITEM_NOBREAK))
          {
            send_to_char("That item is already immune to damage.\n\r", ch);
            return;
          }

          SET_BIT(fixobj->extra_flags, ITEM_NOBREAK);
          send_to_char("Item is now immune to damage.\n\r", ch);
          extract_obj(obj);
          return;
        }

        if (!str_cmp(newarg, "prize"))
        {
          if (( obj = get_obj_carry( ch, "prize coupon", ch)) == NULL)
          {
            send_to_char("You don't have a coupon to turn in.\n\r", ch);
            return;
          }
                    
	  if (obj->pIndexData->vnum != 6401)
          {
            send_to_char("That's not a prize coupon.\n\r", ch);
            return;
	  } 
          act( "$n turns a prize coupon in to $N.", ch, NULL, questman, TO_ROOM); 
	  act ("You turnin your prize coupon to $N.", ch, NULL, questman, TO_CHAR);
          extract_obj(obj);
          item = number_range(0, MAX_PRIZE_COUPON - 1);
          index = prize_coupon_table[item].vnum;
          if (( pObjIndex = get_obj_index(index)) == NULL)
          {
            send_to_char("Bug logged, note imm.\n\r", ch);
            sprintf(buf, "%s lost out on a prize coupon.", ch->name); 
            logf(buf, 0);
            return;
          }        
          obj = create_object(pObjIndex, ch->level);
          act("You have drawn $p from the prize bag.", ch, obj, NULL, TO_CHAR);      
          act("$n draws $p from the prize bag.", ch, obj, NULL, TO_ROOM);      
          obj_to_char(obj, ch);
	  return;
	}

        if (!str_cmp(newarg, "study"))
        {
          int sn;
	  sn = skill_lookup("study");
	  if (sn < 0)
          {
	    send_to_char("No study skill.  Report to imms.\n\r", ch);
	    return;
	  }

          if (( obj = get_obj_carry( ch, "study coupon", ch)) == NULL)
          {
            send_to_char("You don't have a coupon to turn in.\n\r", ch);
            return;
          }
                    
	  if (obj->pIndexData->vnum != 5546)
          {
            send_to_char("That's not a study coupon.\n\r", ch);
            return;
	  } 
          act( "$n turns a study coupon in to $N.", ch, NULL, questman, TO_ROOM); 
	  act ("You turnin your study coupon to $N.", ch, NULL, questman, TO_CHAR);
          extract_obj(obj);
          ch->pcdata->learned[sn] = 75;
	  return;
	}

        if (!str_cmp(newarg, "restore"))
        {
          if (ch->pcdata->pk_timer > 0)
          {
	    act("$N won't help you while you're in such an aggressive mood", ch, NULL, questman, TO_CHAR);
            return;
	  }

          if (( obj = get_obj_carry( ch, "restore coupon", ch)) == NULL)
          {
            send_to_char("You don't have a coupon to turn in.\n\r", ch);
            return;
          }
                    
	  if (obj->pIndexData->vnum != 6460)
          {
            send_to_char("That's not a restore coupon.\n\r", ch);
            return;
	  } 
          act( "$n turns a restore coupon in to $N.", ch, NULL, questman, TO_ROOM); 
	  act ("You turnin your restore coupon to $N.", ch, NULL, questman, TO_CHAR);
          extract_obj(obj);
          do_restore(questman, ch->name);
	  return;
	}
    }
    
    else if (!strcmp(arg1, "buy"))
    {
       bool found = FALSE;
       int prize = 0;
//       int item = 0;
//       int max = 0;
       argument = one_argument(argument, arg2);

         
	if (arg2[0] == '\0')
	{
	    send_to_char("To buy an item, type 'QUEST BUY <item>'.\n\r",ch);
	    return;
	}

        if (!str_cmp(arg2, "token"))
        {
          char tmp[MIL];
	  char arg3[MIL];
	  int points = 0;
          int value = 0;

	  argument = one_argument(argument, arg3);
          if (arg3[0] == '\0' || !is_number(arg3))
          {
            send_to_char("How many points do you want to put into it?\n\r", ch);
            return;
          }          
	  
          points = atoi(arg3);
          if (points < 1)
          {
            send_to_char("I don't think so.\n\r", ch);
            return;
          }
          if (points > 10000)
          {
            char buf[MSL];
            sprintf (buf,"%s tried to buy a token worth %d. Hmm..\n\r",ch->name,points);
            system_note("System",NOTE_NEWS,"IMP","ATTN Player has attempted to purchase excess qps.",buf);
            send_to_char("Illegal command logged and posted to Implementors.\n\r", ch);
	    return;
	  }

          value = int(points * 1.075);
	
          if (value > ch->questpoints)
	  {
            char buf[MSL];
	    send_to_char("You don't have that many quest points.\n\r", ch);
            sprintf(buf, "It costs %d quest points to buy a token worth %d.\n\r",
                          value, points);
            send_to_char(buf, ch);
            return;
          }

	  if ((obj = create_object(get_obj_index(TOKEN_VNUM), ch->level)) == NULL)
	  {
            send_to_char("Bugged please report to imms", ch);
	    return;
	  }


          sprintf(tmp, "A Quest Point Token worth %d Qps", points);
          obj->short_descr = str_dup(tmp);
	  sprintf(tmp, "A token that is worth quest points lies here.");
          obj->description = str_dup(tmp);
          obj->value[0] = points;
    	  act( "$N gives $p to $n.", ch, obj, questman, TO_ROOM );
    	  act( "$N gives you $p.",   ch, obj, questman, TO_CHAR );
          logf("%s: %s bought quest token worth %d", ch->level >= 78 ? "IMMORTAL QP" : "MORTAL QP", ch->name, points);
	  ch->questpoints -= value;
	  obj_to_char(obj, ch);
	  return;
	}

        for (i = 0; i < MAX_LIST; i++)
        {
          if (!str_prefix(arg2, qprize_table[quest_list[i]].keyword))
          {
            found = TRUE;
            prize = i;
            break;
          }
        }
        
        if (!found)
        {
          send_to_char("That is not a valid option.", ch);
          return;
        }

        if (ch->questpoints < qprize_table[quest_list[prize]].cost)   
        {
	  sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.", ch->name);
	  do_tell(questman, buf);
	  return;
        }
       
	ch->questpoints -= qprize_table[quest_list[prize]].cost;

        if (!qprize_table[quest_list[prize]].special)
        {
	  obj = create_object(get_obj_index(qprize_table[quest_list[prize]].vnum),ch->level);
 
  	  if (obj != NULL)
	  {
    	    act( "$N gives $p to $n.", ch, obj, questman, TO_ROOM );
    	    act( "$N gives you $p.",   ch, obj, questman, TO_CHAR );
	    obj_to_char(obj, ch);
	  }
        }
        else
        {
          if (!str_cmp(qprize_table[quest_list[prize]].keyword, "gold"))
          {
            ch->gold += 1000;
            act( "$N gives 1000 gold to $n.", ch, NULL, questman, TO_ROOM);
            act( "$n gives you 1000 gold.", ch, NULL, questman, TO_CHAR);
          }

          if (!str_cmp(qprize_table[quest_list[prize]].keyword, "practices"))
          {
            ch->practice += 10;
            act( "$N gives 10 practices to $n.", ch, NULL, questman, TO_ROOM);
            act( "$N gives you 10 practices.", ch, NULL, questman, TO_CHAR);
          }

           
          }  
/*        item = number_range(0, top_qprize - 1);
        while (inlist(item, MAX_LIST))
        {
          item = number_range(0, top_qprize - 1);
          max++;
          if (max >= 1000)
            break;
        }
        quest_list[prize] = item; */
        initialize_list();
	return;
    }
    else if (!strcmp(arg1, "request"))
    {
        if (ch->position < POS_RESTING)
        {
          send_to_char("You aren't in the position to do that.\n\r", ch);
          return;
	}
 
        act( "$n asks $N for a quest.", ch, NULL, questman, TO_ROOM); 
	act ("You ask $N for a quest.",ch, NULL, questman, TO_CHAR);
	if (IS_SET(ch->act, PLR_QUESTOR))
	{
	    send_to_char("But you're already on a quest!",ch);
	  
	    return;
	}
	if (ch->nextquest > 0)
	{
	    sprintf(buf, "%s You're very brave, but let someone else have a chance.",ch->name);
	    do_tell(questman, buf);
	    sprintf(buf, "%s Come back later.", ch->name);
	    do_tell(questman, buf);
	    return;
	}

	sprintf(buf, "%s Thank you, brave %s!",ch->name, ch->name);
	do_tell(questman, buf);

        ch->questmob = 0;
        ch->questobj = 0;
        ch->questriddle = 0;
	generate_quest(ch, questman);

        if (ch->questmob > 0 || ch->questobj > 0 || ch->questriddle > 0)
	{
            ch->countdown = number_range(30,40);
	    SET_BIT(ch->act, PLR_QUESTOR);
	    sprintf(buf, "%s You have %d minutes to complete this quest.",ch->name, ch->countdown);
	    do_tell(questman, buf);
	    sprintf(buf, "%s May the gods go with you!", ch->name);
	    do_tell(questman, buf);
	}
        ch->questgiver = questman;
	return;
    }
    else if (!strcmp(arg1, "complete"))
    {
        act( "$n informs $N $e has completed $s quest.", ch, NULL, questman, TO_ROOM); 
	act ("You inform $N you have completed $s quest.",ch, NULL, questman, TO_CHAR);
	if (ch->questgiver != questman)
	{
	    sprintf(buf, "%s I never sent you on a quest! Perhaps you're thinking of someone else.", ch->name);
	    do_tell(questman,buf);
	    return;
	}

	if (IS_SET(ch->act, PLR_QUESTOR))
	{
	    if (ch->questmob == -1 && ch->countdown > 0)
	    {
		int reward, pointreward, pracreward ;

		reward = number_range(250,500);
		pointreward = number_range(60,110);
                pointreward += (ch->countdown / 2);
 		pracreward = number_range(1,3);

		sprintf(buf, "%s Congratulations on completing your quest!", ch->name);
		do_tell(questman,buf);
		sprintf(buf,"%s As a reward, I am giving you %d quest points, and %d gold.", ch->name, pointreward,reward);
		do_tell(questman,buf);
                if (ch->clan > 0)
                {
                  int qp = int(pointreward * .1);
		  clan_table[ch->clan].roster.questpoints += qp;
                  member_quest(ch, qp);
                }        
                if (chance(20))
		{
		  sprintf(buf, "You gain %d practices!\n\r", pracreward);
                  send_to_char(buf, ch);
                  ch->practice += pracreward;
                }

	        REMOVE_BIT(ch->act, PLR_QUESTOR);
	        ch->questgiver = NULL;
	        ch->countdown = 0;
	        ch->questmob = 0;
	        ch->rescuemob = 0;
 		ch->questobj = 0;
	        ch->nextquest = xp_bonus.on ? 15 : number_range(15,35);
		ch->gold += reward;
		ch->questpoints += pointreward;

	        return;
	    }
	    else if (ch->questobj > 0 && ch->countdown > 0)
	    {
		bool obj_found = FALSE;

    		for (obj = ch->carrying; obj != NULL; obj= obj_next)
    		{
        	    obj_next = obj->next_content;
        
		    if (obj != NULL && obj->pIndexData->vnum == ch->questobj)
		    {
			obj_found = TRUE;
            	        break;
		    }
        	}
		if (obj_found == TRUE)
		{
		    int reward, pointreward;

		    reward = number_range(250,500);
		    pointreward = number_range(60,110);

		    act("You hand $p to $N.",ch, obj, questman, TO_CHAR);
		    act("$n hands $p to $N.",ch, obj, questman, TO_ROOM);

                    free_string(ch->pcdata->questarea);
                    free_string(ch->pcdata->questroom);
                    ch->pcdata->questarea = str_dup("");
                    ch->pcdata->questroom = str_dup("");

	    	    sprintf(buf, "%s Congratulations on completing your quest!", ch->name);
		    do_tell(questman,buf);
		    sprintf(buf,"%s As a reward, I am giving you %d quest points, and %d gold.", ch->name, pointreward,reward);
		    do_tell(questman,buf);
                    if (ch->clan > 0)
                    {
                       int qp = int(pointreward * .1);
   		       clan_table[ch->clan].roster.questpoints += qp;
                       member_quest(ch, qp);
                    }        
		    
	            REMOVE_BIT(ch->act, PLR_QUESTOR);
	            ch->questgiver = NULL;
	            ch->countdown = 0;
	            ch->questmob = 0;
		    ch->questobj = 0;
	            ch->nextquest = xp_bonus.on ? 15 : number_range(15,35);
		    ch->gold += reward;
		    ch->questpoints += pointreward;
		    extract_obj(obj);
		    return;
		}
		else
		{
		    sprintf(buf, "%s You haven't completed the quest yet, but there is still time!", ch->name);
		    do_tell(questman, buf);
		    return;
		}
		return;
	    }
	    else if ((ch->questmob > 0 || ch->questobj > 0) && ch->countdown > 0)
	    {
		sprintf(buf, "%s You haven't completed the quest yet, but there is still time!", ch->name);
		do_tell(questman, buf);
		return;
	    }
	}
	if (ch->nextquest > 0)
	    sprintf(buf,"%sBut you didn't complete your quest in time!", ch->name);
	else sprintf(buf, "%s You have to REQUEST a quest first, %s.", ch->name, ch->name);
	do_tell(questman, buf);
	return;
    }
    else if (!strcmp(arg1, "abort"))
    {
      if (!IS_SET(ch->act, PLR_QUESTOR))
      {
        send_to_char("But you aren't on a quest.\n\r", ch);
        return;
      }  
      REMOVE_BIT(ch->act, PLR_QUESTOR);
      ch->questgiver = NULL;
      ch->countdown = 0;
      ch->questmob = 0;
      ch->questobj = 0;
      if (ch->rescuemob > 0)
        abort_rescue(ch, TRUE);
      ch->rescuemob = 0;
      free_string(ch->pcdata->questarea);     
      free_string(ch->pcdata->questroom);
      ch->pcdata->questarea = str_dup("");
      ch->pcdata->questroom = str_dup("");
      if (!IS_IMMORTAL(ch))
        ch->nextquest = xp_bonus.on ? 15 : number_range(15,35);
      else
        ch->nextquest = 0;
      sprintf(buf, "Quest aborted %d minutes till you can quest again.\n\r", ch->nextquest);
      send_to_char(buf, ch);
      return;
    }
    else if (!strcmp(arg1, "answer"))
    {
      RIDDLE_DATA *riddle;
      if (!IS_SET(ch->act, PLR_QUESTOR))
      {
        send_to_char("But you aren't on a quest.\n\r", ch);
        return;
      }
    
      if (ch->questriddle == 0)
      {
        send_to_char("But you aren't on a riddle quest.\n\r", ch);
        return;
      }

      if ((riddle = get_riddle_index(ch->questriddle)) == NULL)
      {
        send_to_char("Error resetting quest.\n\r", ch);
        REMOVE_BIT(ch->act, PLR_QUESTOR);
        ch->nextquest = 0;
        ch->questriddle = 0;
        ch->questgiver = NULL;
	ch->countdown = 0;
	ch->questmob = 0;
	ch->questobj = 0;
      }

      if (riddle_lookup(riddle, argument))
      {
	 int reward, pointreward, pracreward ;

	 reward = number_range(250,500);
	 pointreward = number_range(60,110);
         pointreward += (ch->countdown / 2);
 	 pracreward = number_range(1,4);

	sprintf(buf, "%s Congratulations on completing your quest!", ch->name);
	do_tell(questman,buf);
	sprintf(buf,"%s As a reward, I am giving you %d quest points, and %d gold.", ch->name, pointreward,reward);
	do_tell(questman,buf);
        if (ch->clan > 0)
        {
          int qp = int(pointreward * .1);
   	  clan_table[ch->clan].roster.questpoints += qp;
          member_quest(ch, qp);
        }        
	ch->gold += reward;
	ch->questpoints += pointreward;

        if (chance(20))
	{
	  sprintf(buf, "%s You gain %d practices!\n\r", ch->name, pracreward);
          send_to_char(buf, ch);
                  ch->practice += pracreward;
        }
        REMOVE_BIT(ch->act, PLR_QUESTOR);
        ch->nextquest = 0;
        ch->questriddle = 0;
        ch->questgiver = NULL;
	ch->countdown = 0;
	ch->questmob = 0;
	ch->questobj = 0;
        ch->nextquest = xp_bonus.on ? 15 : number_range(15,35);
        return;
      }  	
      else
      { 
      	sprintf(buf, "%s Sorry, but that is not the correct answer.\n\r", ch->name); 
        do_tell(questman, buf);
        return;
      }
      return; 
    }        

    send_to_char("QUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY TURNIN ABORT.\n\r",ch);
    send_to_char("Note: TURNIN is used when you have a Prize Coupon in your inventory.\n\r",ch);
    send_to_char("For more information, type 'HELP QUEST'.\n\r",ch);

    return;
}

void generate_quest(CHAR_DATA *ch, CHAR_DATA *questman)
{
    CHAR_DATA *victim;
    MOB_INDEX_DATA *vsearch = NULL;
    ROOM_INDEX_DATA *room;
    OBJ_DATA *questitem;
    char buf [MAX_STRING_LENGTH];
    long mcounter = 0;
    int level_diff, mob_vnum;
    bool IQ = FALSE;
    int vnum;
    RIDDLE_DATA *riddle;
     

    if (chance(25))
    {
      generate_rescue_quest(ch, questman, 0, FALSE);
      return; 
    }

    if (chance(3)) // Enable when I have some real riddles
    {
      vnum = number_range(1, top_vnum_riddle);
      if ((riddle = get_riddle_index(vnum)) == NULL)
      {
 	sprintf(buf, "%s I'm sorry, but I don't have any quests for you at this time.", ch->name);
	do_tell(questman, buf);
	sprintf(buf, "%s Try again later.", ch->name);
	do_tell(questman, buf);
	ch->nextquest = 0;
        return;
      }
      else 
      {
        sprintf(buf, "%s Your quest is to solve the following riddle:", ch->name);
        SET_BIT(ch->act, PLR_QUESTOR);
        ch->questriddle = riddle->vnum;
        do_tell(questman, buf);
        riddle_say(riddle, questman, ch);
        return;    
      }
    }      

    /*  Randomly selects a mob from the world mob list. If you don't
	want a mob to be selected, make sure it is immune to summon.
	Or, you could add a new mob flag called ACT_NOQUEST. The mob
	is selected for both mob and obj quests, even tho in the obj
	quest the mob is not used. This is done to assure the level
	of difficulty for the area isn't too great for the player. */

    if (chance(50))
      IQ = TRUE;  

   
     while (vsearch == NULL)
     {
	mob_vnum = number_range(50, 32600);

	if ( (vsearch = get_mob_index(mob_vnum) ) != NULL )
	{
	    level_diff = vsearch->level - ch->level;

		/* Level differences to search for. Moongate has 350
		   levels, so you will want to tweak these greater or
		   less than statements for yourself. - Vassago */
		
	    if ( ((ch->level < 10 && level_diff < 7 && level_diff > -5)
                 || (ch->level < 40 && level_diff < 15 && level_diff > -5)
                 || (ch->level < 90 && level_diff < 35 && level_diff > -5)
                 || IQ)
		&& vsearch->pShop == NULL
		&& !IS_SET(vsearch->area->area_flags, AREA_NOQUEST)
		&& !IS_SET(vsearch->area->area_flags, AREA_UNLINKED)
    		&& !IS_SET(vsearch->act,ACT_TRAIN)
    		&& !IS_SET(vsearch->act,ACT_PRACTICE)
    		&& !IS_SET(vsearch->act,ACT_IS_HEALER)
                && !IS_SET(vsearch->act,ACT_NOQUEST))
		   break;
		else
		{
		   vsearch = NULL;
		   continue;
		}
	}
        if (mcounter++ > 250000)
          break;
    }

    if ( vsearch == NULL || ( victim = get_char_world_vnum( ch, vsearch->vnum ) ) == NULL )
    {
	sprintf(buf, "%s I'm sorry, but I don't have any quests for you at this time.", ch->name);
	do_tell(questman, buf);
	sprintf(buf, "%s Try again later.", ch->name);
	do_tell(questman, buf);
	ch->nextquest = 0;
        return;
    }

    if ( ( room = find_location_vnum( ch, victim->pIndexData->vnum ) ) == NULL 
           || is_safe_silent(ch, victim))
    {
	sprintf(buf, "%s I'm sorry, but I don't have any quests for you at this time.", ch->name);
	do_tell(questman, buf);
	sprintf(buf, "%s Try again later.", ch->name);
	do_tell(questman, buf);
	ch->nextquest = 0;
        return;
    }

    /*  40% chance it will send the player on a 'recover item' quest. */
    if (IQ)
    {
	int objvnum = 0;

	switch(number_range(0,25))
	{
	    case 0:
	    objvnum = QUEST_OBJQUEST1;
	    break;

	    case 1:
	    objvnum = QUEST_OBJQUEST2;
	    break;

	    case 2:
	    objvnum = QUEST_OBJQUEST3;
	    break;

	    case 3:
	    objvnum = QUEST_OBJQUEST4;
	    break;

	    case 4:
	    objvnum = QUEST_OBJQUEST5;
	    break;
	    case 5:
	    objvnum = QUEST_OBJQUEST6;
	    break;
	    case 6:
	    objvnum = QUEST_OBJQUEST7;
	    break;
	    case 7:
	    objvnum = QUEST_OBJQUEST8;
	    break;
	    case 8:
	    objvnum = QUEST_OBJQUEST9;
	    break;
	    case 9:
	    objvnum = QUEST_OBJQUEST10;
	    break;
	    case 10:
	    objvnum = QUEST_OBJQUEST11;
	    break;
	    case 11:
	    objvnum = QUEST_OBJQUEST12;
	    break;
	    case 12:
	    objvnum = QUEST_OBJQUEST13;
	    break;
	    case 13:
	    objvnum = QUEST_OBJQUEST14;
	    break;
	    case 14:
	    objvnum = QUEST_OBJQUEST15;
	    break;
	    	    case 15:
	    objvnum = QUEST_OBJQUEST16;
	    break;
	    	    case 16:
	    objvnum = QUEST_OBJQUEST17;
	    break;
	    	    case 17:
	    objvnum = QUEST_OBJQUEST18;
	    break;
	    	    case 18:
	    objvnum = QUEST_OBJQUEST19;
	    break;
	    	    case 19:
	    objvnum = QUEST_OBJQUEST20;
	    break;
	    	    case 20:
	    objvnum = QUEST_OBJQUEST21;
	    break;
	    	    case 21:
	    objvnum = QUEST_OBJQUEST22;
	    break;
	    	    case 22:
	    objvnum = QUEST_OBJQUEST23;
	    break;
	    	    case 23:
	    objvnum = QUEST_OBJQUEST24;
	    break;
	    	    case 24:
	    objvnum = QUEST_OBJQUEST25;
	    break;
	    	    case 25:
	    objvnum = QUEST_OBJQUEST26;
	    break;

	    
	}

        questitem = create_object( get_obj_index(objvnum), ch->level );
        questitem->owner = str_dup(ch->name);
        questitem->timer = 20;/*mofo*/
	obj_to_room(questitem, room);
	ch->questobj = questitem->pIndexData->vnum;

	sprintf(buf, "%s A rouge has stolen %s from the White Tower!", ch->name, questitem->short_descr);
	do_tell(questman, buf);
	sprintf(buf, "%s My apprentice has been able to locate its position.", ch->name);
	do_tell(questman, buf);
	

	/* I changed my area names so that they have just the name of the area
	   and none of the level stuff. You may want to comment these next two
	   lines. - Vassago */


	sprintf(buf, "%s The scry has located it in %s very close to %s!", ch->name, room->area->name, room->name);
        free_string(ch->pcdata->questarea);
        free_string(ch->pcdata->questroom);
        ch->pcdata->questarea = str_dup(room->area->name);
        ch->pcdata->questroom = str_dup(room->name);
	do_tell(questman, buf);
	return;
    }

    /* Quest to kill a mob */

    else 
    {
    switch(number_range(0,1))
    {
	case 0:
        sprintf(buf, "%s Your quest is to eliminate %s. They have been threatening the city for days now.", ch->name, victim->short_descr);
        do_tell(questman, buf);
        sprintf(buf, "%s You must end their attacks and bring peace to the city!", ch->name);
        do_tell(questman, buf);
	break;

	case 1:
	sprintf(buf, "%s This city's most wanted criminal, %s, has escaped from prison!", ch->name, victim->short_descr);
	do_tell(questman, buf);
	sprintf(buf, "%s Since the escape, %s has murdered %d travellers!", ch->name, victim->short_descr, number_range(2,20));
	do_tell(questman, buf);
	sprintf(buf, "%s The penalty for this crime is death, and you are to deliver the sentence!", ch->name);
        do_tell(questman, buf);
	break;
    }

    if (room->name != NULL)
    {
        sprintf(buf, "%s Seek %s out somewhere in the vicinity of %s!",ch->name, victim->short_descr,room->name);
        do_tell(questman, buf);

	/* I changed my area names so that they have just the name of the area
	   and none of the level stuff. You may want to comment these next two
	   lines. - Vassago */

	sprintf(buf, "%s That location is in the general area of %s.", ch->name, room->area->name);
	do_tell(questman, buf);
    }
    ch->questmob = victim->pIndexData->vnum;
    ch->questobj = 0;
    }
    return;
}

/* Called from update_handler() by pulse_area */

void quest_update(void)
{
   DESCRIPTOR_DATA *d;
   CHAR_DATA *ch;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        ch = d->character ? d->character : d->original;

        if (d->connected != CON_PLAYING)
          continue;

	if (IS_NPC(ch)) 
           continue;

	if (ch->nextquest > 0) 
	{
	    ch->nextquest--;

	    if (ch->nextquest == 0)
	    {
	        send_to_char("You may now quest again.\n\r",ch);
	        continue;
	    }
	}
        else if (IS_SET(ch->act,PLR_QUESTOR))
        {
	    if (--ch->countdown <= 0)
	    {
    	        char buf [MAX_STRING_LENGTH];

	        ch->nextquest = xp_bonus.on ? 20 : 35;
	        sprintf(buf, "You have run out of time for your quest!\n\rYou may quest again in %d minutes.\n\r",ch->nextquest);
	        send_to_char(buf, ch);
	        REMOVE_BIT(ch->act, PLR_QUESTOR);
                ch->questgiver = NULL;
                ch->countdown = 0;
                ch->questmob = 0;
                if (ch->rescuemob != 0)
                  abort_rescue(ch, TRUE);
                ch->rescuemob = 0;
                free_string(ch->pcdata->questarea);
                free_string(ch->pcdata->questroom);
                ch->pcdata->questarea = str_dup("");
                ch->pcdata->questroom = str_dup("");
	    }
	    if (ch->countdown > 0 && ch->countdown < 6)
	    {
	        send_to_char("Better hurry, you're almost out of time for your quest!\n\r",ch);
	        continue;
	    }
        }
    }
    return;
}

void abort_quest(CHAR_DATA *ch)
{
  if (IS_SET(ch->act, PLR_QUESTOR))
  {
      REMOVE_BIT(ch->act, PLR_QUESTOR);
      ch->questgiver = NULL;
      ch->countdown = 0;
      ch->questmob = 0;
      ch->questobj = 0;
      ch->nextquest = xp_bonus.on ? 20 : 35;
      send_to_char("Quest aborted 20 minutes till you can quest again.\n\r", ch);
      return;
   }
   return;
}

void riddle_say(RIDDLE_DATA *riddle, CHAR_DATA *questman, CHAR_DATA *ch)    
{
  char buf[MAX_STRING_LENGTH];
  char *string;
  
  buf[0] = '\0';
  
  string = str_dup(riddle->question);
  while (*string)
  {
     char buf2[MSL];
     string = getline (string, buf);
     sprintf(buf2, "%s %s", ch->name, buf);
     do_tell(questman, buf2);
  }
  sprintf(buf, "%s What am I?", ch->name);
  do_tell(questman, buf);
 
}

void generate_rescue_quest(CHAR_DATA *ch, CHAR_DATA *questman, int number, bool test)
{
    MOB_INDEX_DATA *vsearch = NULL;
    ROOM_INDEX_DATA *room;
    RESCUE_DATA rescue;
    char buf [MAX_STRING_LENGTH];
    long mcounter = 0;
    int level_diff, mob_vnum;
    int good = 0;
    int evil = 0;
    long tmcounter = 0;
    int iRandomEvil;
    int iRandomGood;
    int i;
    MOB_INDEX_DATA *goodMobIndex;
    MOB_INDEX_DATA *evilMobIndex;
    CHAR_DATA *goodMob;
    CHAR_DATA *evilMob;
 

    for (i = 0; i < MAX_RESCUE; i++)
    {
      rescue.vnumGood[i] = 3011;
      rescue.vnumEvil[i] = 11697;
    }

     while (good < MAX_RESCUE && evil < MAX_RESCUE)
     {
	mob_vnum = number_range(50, 32600);

	if ( (vsearch = get_mob_index(mob_vnum) ) != NULL )
	{
	    level_diff = vsearch->level - ch->level;

		/* Level differences to search for. Moongate has 350
		   levels, so you will want to tweak these greater or
		   less than statements for yourself. - Vassago */
		
	    if ( ((ch->level <= 10 && level_diff < 7 && level_diff > -5)
                 || (ch->level > 10 && ch->level <= 40 && level_diff < 20 && level_diff > -5)
                 || (ch->level > 40 && ch->level <= 81 && level_diff < 35 && level_diff > -5))
		&& (!IS_SET(vsearch->area->area_flags, AREA_NOQUEST))
		&& (!IS_SET(vsearch->area->area_flags, AREA_UNLINKED))
		&& vsearch->pShop == NULL
    		&& !IS_SET(vsearch->act,ACT_TRAIN)
    		&& !IS_SET(vsearch->act,ACT_PRACTICE)
    		&& !IS_SET(vsearch->act,ACT_IS_HEALER)
		&& !IS_SET(vsearch->act,ACT_NOQUEST))
            {
            	if (IS_SET(vsearch->act,ACT_NOQUEST))
            	tmcounter++;
                if (vsearch->alignment >= 0)
                {
                    if (good >= MAX_RESCUE)
                      continue;  
                    rescue.vnumGood[good] = vsearch->vnum;
                    good++;
                }
                else
                {
                    if (evil >= MAX_RESCUE)
                      continue;  
                    rescue.vnumEvil[evil] = vsearch->vnum;
                    evil++;
                }
            }
            
            if (mcounter++ > 250000)
                break;
            continue;             	
	}      
    }

    iRandomEvil = number_range(0, (evil-1));
    iRandomGood = number_range(0, (good-1));

    if ((goodMobIndex = get_mob_index(rescue.vnumGood[iRandomGood])) == NULL)
    {
      if (number < 3)
      {
        generate_rescue_quest(ch, questman, number + 1, test);
        return;
      }
  
      if (!test)
      {
        sprintf(buf, "%s I'm sorry, but I don't have any quests for you at this time.", ch->name);
        do_tell(questman, buf);
        sprintf(buf, "%s Try again later.", ch->name);
        do_tell(questman, buf);
      }
      ch->nextquest = 0;
      return;
    }

    if ((goodMob = get_char_world_vnum( ch, rescue.vnumGood[iRandomGood])) == NULL
         || goodMob->attacker != NULL)
    {
      if (number < 3)
      {
        generate_rescue_quest(ch, questman, number + 1, test);
        return;
      }
      
      if (!test)
      {
        sprintf(buf, "%s I'm sorry, but I don't have any quests for you at this time.", ch->name);
        do_tell(questman, buf);
        sprintf(buf, "%s Try again later.", ch->name);
        do_tell(questman, buf);
      }
      ch->nextquest = 0;
      return;
    }

    if ( ( room = goodMob->in_room) == NULL || is_safe_silent(ch, goodMob))
    {
      if (number < 3)
      {
        generate_rescue_quest(ch, questman, number + 1, test);
        return;
      }
      if (!test)
      {
        sprintf(buf, "%s I'm sorry, but I don't have any quests for you at this time.", ch->name);
        do_tell(questman, buf);
        sprintf(buf, "%s Try again later.", ch->name);
        do_tell(questman, buf);
      }
      ch->nextquest = 0;
      return;
    }

    if ((evilMobIndex = get_mob_index(rescue.vnumEvil[iRandomEvil])) == NULL)
    {
      if (number < 3)
      {
        generate_rescue_quest(ch, questman, number + 1, test);
        return;
      }
      if (!test)
      {
        sprintf(buf, "%s I'm sorry, but I don't have any quests for you at this time.", ch->name);
        do_tell(questman, buf);
        sprintf(buf, "%s Try again later.", ch->name);
        do_tell(questman, buf);
      }
      ch->nextquest = 0;
      return;
    }

    if ((evilMob = create_mobile(evilMobIndex)) == NULL)
    {
      if (number < 3)
      {
        generate_rescue_quest(ch, questman, number + 1, test);
        return;
      }

      if (!test)
      {
        sprintf(buf, "%s I'm sorry, but I don't have any quests for you at this time.", ch->name);
        do_tell(questman, buf);
        sprintf(buf, "%s Try again later.", ch->name);
        do_tell(questman, buf);
      }
      ch->nextquest = 0;
      return;
    }

    evilMob->max_hit = int(goodMob->max_hit * 1.4);  
    evilMob->hit = int(goodMob->max_hit * 1.4);  
    evilMob->damroll = int(goodMob->damroll * 1.5);  
    evilMob->hitroll = int(goodMob->hitroll * 1.5);  
    char_to_room(evilMob, room);    
    multi_hit(evilMob, goodMob, TYPE_UNDEFINED);

    if (!test)
    {
    sprintf(buf, "%s Your quest is to rescue %s and kill %s.", 
	         ch->name, goodMob->short_descr, evilMob->short_descr);
    do_tell(questman, buf);
    sprintf(buf, "%s You must end their attacks and save %s!", 
                ch->name, goodMob->short_descr);
    do_tell(questman, buf);
    sprintf(buf, "%s Seek %s out somewhere in the vicinity of %s!", 
		ch->name, evilMob->short_descr, room->name);
    do_tell(questman, buf);
    sprintf(buf, "%s That location is in the general area of %s.", 
	        ch->name, room->area->name);
    do_tell(questman, buf);
    }

    goodMob->rescuer = ch;
    goodMob->attacker = evilMob;
    goodMob->spec_fun = spec_lookup("spec_rescue");
    ch->rescuemob = goodMob->pIndexData->vnum;
    ch->questmob = evilMob->pIndexData->vnum;  
    evilMob->attacking = ch->rescuemob;
    if (test)
    {
    sprintf(buf, "Good %-35s NoQuest %-5s Lcycle %ld\nBad  %-35s NoQuest %-5s Lcycle %ld\n\r", 
            goodMob->short_descr, IS_SET(goodMob->act, ACT_NOQUEST) ? "True" : "False",tmcounter,
            evilMob->short_descr, IS_SET(evilMob->act, ACT_NOQUEST) ? "True" : "False",tmcounter);
    send_to_char(buf, ch);
    }
    ch->pcdata->questarea = str_dup(room->area->name);
    ch->pcdata->questarea = str_dup(room->name);
    return;   
}

void abort_rescue(CHAR_DATA *ch, bool bPullAttacker)
{
  CHAR_DATA *wch;
  for (wch = char_list; wch != NULL; wch = wch->next)
  {
    if (!IS_NPC(wch))
      continue;

    if (wch->rescuer == ch)
    {
      wch->rescuer = NULL;
      wch->spec_fun = wch->pIndexData->spec_fun;

      {
        AFFECT_DATA af;
        af.where = TO_AFFECTS;
        af.type = skill_lookup("calm");
        af.level = 120;
        af.modifier = 1;  
        af.duration = 5;
        af.location = APPLY_NONE;
        af.bitvector = AFF_CALM;
        affect_to_char( wch, &af);
      }
 
      if (wch->attacker)
      {
        if (bPullAttacker && get_char_exists(wch->attacker)) 
          extract_char(wch->attacker, TRUE);
        wch->attacker = NULL;
      }
      break;
    }
  }
     
}

void fail_rescue(CHAR_DATA *ch, CHAR_DATA *mob)
{
  char buf[MSL];
  if (ch->countdown <= 0 || ch->rescuemob != mob->pIndexData->vnum)
  {
    mob->rescuer = NULL;
    extract_char(mob->attacker, TRUE);
    return;
  } 
  sprintf(buf, "You have failed your quest, %s has perished.\n\r", mob->short_descr);
  send_to_char(buf, ch);
  ch->nextquest = xp_bonus.on ? 20 : 35;
  REMOVE_BIT(ch->act, PLR_QUESTOR);
  ch->questgiver = NULL;
  ch->rescuemob = 0;
  ch->countdown = 0;
  ch->questmob = 0;
  sprintf(buf, "You may quest again in %d minutes.\n\r",ch->nextquest);
  extract_char(mob->attacker, TRUE);
  send_to_char(buf, ch);
  free_string(ch->pcdata->questarea);
  free_string(ch->pcdata->questroom);
  ch->pcdata->questarea = str_dup("");
  ch->pcdata->questroom = str_dup("");
}

/*
 =============================================================================
/   ______ _______ ____   _____   ___ __    _ ______    ____  ____   _____   /
\  |  ____|__   __|  _ \ / ____\ / _ \| \  / |  ____|  / __ \|  _ \ / ____\  \
/  | |__     | |  | |_| | |     | |_| | |\/| | |___   | |  | | |_| | |       /
/  | ___|    | |  | ___/| |   __|  _  | |  | | ____|  | |  | |  __/| |   ___ \
\  | |       | |  | |   | |___| | | | | |  | | |____  | |__| | |\ \| |___| | /
/  |_|       |_|  |_|  o \_____/|_| |_|_|  |_|______|o \____/|_| \_|\_____/  \
\                                                                            /
 ============================================================================

------------------------------------------------------------------------------
ftp://ftp.game.org/pub/mud      FTP.GAME.ORG      http://www.game.org/ftpsite/
------------------------------------------------------------------------------

   This file came from FTP.GAME.ORG, the ultimate source for MUD resources.

------------------------------------------------------------------------------
*/
