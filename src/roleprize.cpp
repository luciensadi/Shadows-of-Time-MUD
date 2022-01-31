
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"

int rp_max_hp args ((CHAR_DATA *ch));
int rp_max_mana args ((CHAR_DATA *ch));
void legend_req args ((CHAR_DATA *ch));
DECLARE_DO_FUN (do_bonus_time);
DECLARE_DO_FUN (do_echo);

int rp_max_hp(CHAR_DATA *ch)
{
  int temp;
  temp = con_app[ch->perm_stat[STAT_CON]].hitp +
         class_table[ch->cClass].hp_max; /* Max Gain */
  temp += 9;  /* %% Train */
  temp += wis_app[get_curr_stat(ch, STAT_WIS)].practice; /* Converted PRacs */

  temp *= 75;  /* Multiply times level */ 
  temp += 30; /* Original Trains */
  temp += 20; //Initial Hp
//  temp += 1500;//eq
// base of of base stats not eq
  return temp;
}

int rp_max_mana(CHAR_DATA *ch)
{
  int temp;
  temp = (ch->perm_stat[STAT_INT] /5);
  temp += class_table[ch->cClass].mana_max;
  temp += 1;// %%trains
  //if (!class_table[ch->cClass].fMana)
  //  temp /= 2;
 
  temp *= 75;
  temp += 100; //Initial Mana
  temp = int(temp * .90); // Make mana a little more reasonable
  return temp;
}   
void legend_req (CHAR_DATA *ch)
{
   char buf [MSL];

   if (IS_NPC(ch))
     return;

   if (ch->level >= 76)
   {
     sprintf(buf, "You have a maximum of %d hp and %d mana.\n\r",
          max_natural_hp(ch), max_natural_mana(ch));
     send_to_char(buf, ch);
   }
   else
   {   
   sprintf(buf,	"The Requirements are :\n"
                            "    - Gold   : 10000\n"
		            "    - Level  : 75\n"
		            "    - RP lvl : 12\n"	            
		            "    - HP     : %d\n"
		            "    - MANA   : %d\n\r",
		            rp_max_hp(ch),
		            rp_max_mana(ch));
   send_to_char(buf,ch);
   sprintf(buf, "You are %ld hps, %d mana,", 
	        UMAX(0, rp_max_hp(ch) - ch->pcdata->perm_hit),
		UMAX(0, rp_max_mana(ch) - ch->pcdata->perm_mana));
   send_to_char(buf, ch);
   sprintf(buf, "%ld gold and %d rp levels short right now.\n\r", 
	        UMAX(0, 10000 - ch->gold),
		UMAX(0, 12 - ch->rplevel));
   send_to_char(buf, ch);
   }
   return;		
}

void do_roleprize(CHAR_DATA *ch, char *argument)
{

    char buf [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    
	int i;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

	if (!IS_NPC(ch) && !IS_SET(ch->in_room->room_flags, ROOM_ROLEPRIZE))
	{
		send_to_char ("You can't do that here.",ch);
		return;
	}
	if (arg1[0] == '\0')
    {
        sprintf(buf,"Syntax: ROLEPRIZE LIST           - lists available stuff to purchase.\n"
                    "        ROLEPRIZE BUY <option>   - purchases the item that is listed above\n"
                    "        ROLEPRIZE LEGEND         - awards/shows in LEGEND requirements\n\r");
        send_to_char(buf,ch);
        return;
    }
    if (!strcmp(arg1, "list"))
    {
		send_to_char("Current roleplaying prizes that are available for purchase:\n\r", ch); 
        send_to_char("  Cost - Level      Name             \n\r", ch);     
        for (i = 0; rp_prize_table[i].name!=NULL; i++)
        {
			sprintf(buf, "  %4d -  %3s - %s`*\n\r", 
				rp_prize_table[i].cost,
				rp_prize_table[i].level,
				rp_prize_table[i].name);
			send_to_char(buf, ch);
        }  
        send_to_char("To buy an item, type 'ROLEPRIZE BUY <item>'.\n\r", ch);
		return;
	}
	
	if (!strcmp(arg1, "legend"))
        {
	  if (ch->cClass == class_lookup("forsaken") || ch->cClass == class_lookup("dragon"))
	  {
	    send_to_char("You're already a legacy in your own right.\n\r", ch);
	    return;
	  }
        
      	if ((ch->level <= 74)
	|| (ch->rplevel < 12)
	|| (ch->gold < 10000) 
        || (ch->pcdata->perm_hit < rp_max_hp(ch))
        || (ch->pcdata->perm_mana < rp_max_mana(ch)))
	{
           char buf[MSL];
           sprintf(buf, "You dont meet the %s class requirements.\n\r", class_table[ch->cClass].name);
           send_to_char(buf, ch);
		legend_req(ch);
		return;
	}
	else if(ch->level <= 75)
        {
	  	send_to_char("Welcome to the highest and most honored rank possibly\n\r", ch);
	  	send_to_char("achievable as a mortal. You will be permanantly added\n\r", ch);
	  	send_to_char("to the Elite Shadows of Time list. Thank you for playing\n\r", ch);
	  	send_to_char("and putting your hard work into this game. Soon we will\n\r", ch);
	  	send_to_char("have some extra advantages to being a legend.\n\r", ch);
	  	log_string (buf); 
                mudstats.current_heros += 1;
                mudstats.total_heros += 1;
                sprintf (buf, "$N has attained legend status!\n\r");
                wiznet (buf, ch, NULL, WIZ_LEVELS, 0, 0);
                advance_level (ch, TRUE, FALSE);
                save_char_obj (ch);
                do_bonus_time(ch,"2 10");
                do_echo(ch, "The time has come to praise a new `3Legend`7, may they never be forgotten!\n\r");
	  	ch->gold = ch->gold - 10000;
	  	return;
         }
    }  
        
		
    if (!strcmp(arg1, "buy"))
    {
		bool found = FALSE;
		int prize = 0;
		
		if (arg2[0] == '\0')
		{
			send_to_char("To buy an item, type 'ROLEPRIZE BUY <item>'.\n\r",ch);
			return;
		}

        for (i = 0; rp_prize_table[i].name != NULL; i++)
        {
			if (!str_cmp(arg2, rp_prize_table[i].name))
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

        if (ch->rp_points < rp_prize_table[prize].cost)   
        {
			send_to_char("Sorry, but you don't have enough rp points for that.\n\r", ch);
			return;
        }
       
		ch->rp_points -= rp_prize_table[prize].cost;
   
        if (!str_cmp(rp_prize_table[prize].name, "Gold"))
        {
			ch->gold += 10000;
            act( "$n recieves 10000 gold for great roleplaying!", ch, NULL, NULL, TO_ROOM);
            send_to_char( "You recieve 10000 gold for your great roleplaying!\n\r",ch);
        }
		else if (!str_cmp(rp_prize_table[prize].name, "Train"))
        {
            ch->train += 2; 
            act( "$n looks strengthened.\n\r", ch, NULL, NULL, TO_ROOM);
			send_to_char("You recieve 2 trains for you great roleplaying!.\n\r",ch);
        }

	else if (!str_cmp(rp_prize_table[prize].name, "Practices"))
        {
            ch->practice += 10; 
            act( "$n looks more learned.\n\r", ch, NULL, NULL, TO_ROOM);
            send_to_char( "You recieve 10 practices for your great roleplaying!.\n\r", ch);
        }
		else if (!str_cmp(rp_prize_table[prize].name, "Legend"))
 {
          if (ch->cClass == class_lookup("forsaken") || ch->cClass == class_lookup("dragon"))
          {
            send_to_char("You're already a legacy in your own right.\n\r", ch);
            return;
          }

        if ((ch->level <= 74))
       /* || (ch->rplevel < 12)
        || (ch->gold < 10000)
        || (ch->pcdata->perm_hit < rp_max_hp(ch))
        || (ch->pcdata->perm_mana < rp_max_mana(ch))) */
        {
           char buf[MSL];
           sprintf(buf, "You dont meet the %s class requirements.\n\r", class_table[ch->cClass].name);
           send_to_char(buf, ch);
                legend_req(ch);
                return;
        }
        else if(ch->level <= 75)
        {
                send_to_char("Welcome to the highest and most honored rank possibly\n\r", ch);
                send_to_char("achievable as a mortal. You will be permanantly added\n\r", ch);
                send_to_char("to the Elite Shadows of Time list. Thank you for playing\n\r", ch);
                send_to_char("and putting your hard work into this game. Soon we will\n\r", ch);
                send_to_char("have some extra advantages to being a legend.\n\r", ch);
                log_string (buf);
                mudstats.current_heros += 1;
                mudstats.total_heros += 1;
                sprintf (buf, "$N has attained legend status!\n\r");
                wiznet (buf, ch, NULL, WIZ_LEVELS, 0, 0);
                advance_level (ch, TRUE, FALSE);
	        ch->max_hit = rp_max_hp(ch);
		ch->max_mana = rp_max_mana(ch);
                save_char_obj (ch);
                do_bonus_time(ch,"2 10");
                do_echo(ch, "The time has come to praise a new `3Legend`7, may they never be forgotten!\n\r");
                ch->gold = ch->gold - 10000;
                return;
         }
/*
	{
			if (ch->level < (MAX_LEVEL - 6))
			{
				advance_level(ch,TRUE,FALSE); 
				act( "$n looks more experienced.\n\r", ch, NULL, NULL, TO_ROOM);
				sprintf(buf, "You are now level %d",ch->level);
				send_to_char (buf,ch);
			}
			else
			{
				send_to_char ("You are already at the maximum level.\n\r",ch);
				ch->rp_points += rp_prize_table[prize].cost;
			}
        }
*/
		else if (!str_cmp(rp_prize_table[prize].name, "RPlevel"))
        {
			if (ch->rplevel<=MAX_RPLEVEL)
			{
				rpadvance_level(ch,TRUE,FALSE); 
				act( "$n looks more experienced.\n\r", ch, NULL, NULL, TO_ROOM);
				sprintf(buf, "You are now at roleplaying level %d",ch->rplevel);
				send_to_char (buf,ch);

			}
			else
			{
				send_to_char ("You are already at the maximum role-playing level.\n\r",ch);
				ch->rp_points += rp_prize_table[prize].cost;
			}
        }
		else if (!str_cmp(rp_prize_table[prize].name, "Bonus2x"))
		{
			do_bonus_time(ch,"2 15");
		}
		else if (!str_cmp(rp_prize_table[prize].name, "Bonus3x"))
		{
			do_bonus_time(ch,"3 15");
		}
		
	}
   }
}

