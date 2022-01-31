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


int check_roster args ((char *name, int clan));
MEMBER_DATA *find_member args ((char *name, int clan));

void do_roster(CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  MEMBER_DATA *mbr;
  bool found = FALSE;
  int clan;

  if (!is_clan(ch))
  {
    send_to_char("You aren't in a guild, therefore you have no roster.\n\r", ch);
    return;
  }

  if (!is_clan(ch))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  argument = one_argument(argument, arg1);
  
  if (arg1[0] == '\0')
  {
    send_to_char("Syntax is:\n\r", ch);
    send_to_char("roster \n\r", ch);
    send_to_char("       show                        - Show your guilds roster.\n\r", ch);
    send_to_char("       remove <name>               - Remove someone from roster.\n\r", ch);
    send_to_char("       action <name> outcast       - Outcast a member.\n\r", ch);
    send_to_char("       action <name> promote #     - Promote a member.\n\r", ch);
    send_to_char("       action <name> demote #      - Demote a member.\n\r", ch);
    send_to_char("       withdraw #                  - Withdraw X amount of qp.\n\r", ch);

    if (IS_IMMORTAL(ch))
    {
      send_to_char("       add <name> <rank>           - Add someone to roster.\n\r", ch);
      send_to_char("       change <name> <rank>        - Change someone's rank on roster.\n\r", ch);
    }
    if (ch->level == MAX_LEVEL)
    {
      send_to_char("       save                        - Save the roster.\n\r", ch);
      send_to_char("       points <#>                  - Set roster points.\n\r", ch);
    }
    return;
  }

  clan = ch->clan;
 
 if (!str_cmp(arg1, "cancel"))
 
 {

  if (str_cmp(clan_table[ch->clan].roster.requestor, ch->name))
 {
  send_to_char ("You are not the person who requested the withdrawl.\n\r", ch);
  return;
 }
  clan_table[ch->clan].roster.withdraw = 0;
  clan_table[ch->clan].roster.approve = TRUE;
  send_to_char("You have canceled your withdrawl request.\n\r", ch);
  return;
 }

  if (!str_cmp(arg1, "deny"))
  {

   if (clan_table[ch->clan].roster.withdraw < 1)
   {
     send_to_char ("There's nothing to deny.\n\r", ch);
     return;
   }

   if (!str_cmp(clan_table[ch->clan].roster.requestor, ch->name))
   {
     send_to_char ("You can't deny your own withdrawl.\n\r", ch);
     return;
   }
   
   clan_table[ch->clan].roster.withdraw = 0;
   clan_table[ch->clan].roster.approve = FALSE;
   do_function(ch, &do_clantalk, "I have denied your request to withdraw from the guild coffer.\n\r");
   return;
  }

  if (!str_cmp(arg1, "approve"))
  {
    if (clan_table[ch->clan].roster.withdraw < 1)
    {
      send_to_char("There's nothing to approve.\n\r", ch);
      return;
    }
 
    if (clan_table[ch->clan].roster.approve == TRUE)
    {
      send_to_char("It has already been approved.\n\r", ch);
      return;
    }

    if (!str_cmp(clan_table[ch->clan].roster.requestor, ch->name))
    {
      send_to_char("You can't approve your own withdrawal.\n\r", ch);
      return;
    }
  
    clan_table[ch->clan].roster.approve = TRUE;  
    send_to_char("Approved.\n\r", ch);
    logf("ROSTER: %s approved a withdrawl of %d quest points.\n\r", ch->name, clan_table[ch->clan].roster.withdraw); 
    do_function(ch, &do_clantalk, "I have just approved the withdrawal request.");
 
    return;
  }

  if (!str_cmp(arg1, "last"))
  {
    char buf[MSL];
    if (ch->rank < clan_table[ch->clan].top_rank - 2 &&
		 !IS_SET(ch->act2, PLR_GUILDLEADER))
    {
      send_to_char("You don't have the authority for that.\n\r", ch);
      return;
    }
    sprintf(buf, "%-15s %-20s\n\r", "Name", "Last Logged" );
    send_to_char(buf, ch);
    sprintf(buf, "%-15s %-20s\n\r", "----", "-----------" );
    send_to_char(buf, ch);
    for (mbr = clan_table[ch->clan].roster.member; \
               mbr != NULL ; mbr = mbr->next )
    {
      sprintf(buf, "%-15s %-20s\n\r", capitalize(mbr->name), 
				mbr->lastLog == 0 ? "Unknown" : 
				chomp((char *) ctime (&mbr->lastLog)));
      send_to_char(buf, ch);
    }
    return;
  }

  if (!str_cmp(arg1, "show"))
  {
    char buf[MSL];

    if (ch->rank >= clan_table[ch->clan].top_rank - 2)
    {
      int nummem = 0;
      sprintf(buf, "`&Roster for `*%s`&:`*\n\r", clan_table[ch->clan].who_name);    
      send_to_char(buf, ch);
      sprintf(buf, "Guild Quest Points: %d\n\r", 
              clan_table[ch->clan].roster.questpoints);
      send_to_char(buf, ch);
      if (clan_table[ch->clan].roster.withdraw > 0)
      {
        send_to_char("\n\r", ch);
        sprintf(buf, "Withdrawal request by %s made for %d Qps.\n\r", 
                 clan_table[ch->clan].roster.requestor, 
                 clan_table[ch->clan].roster.withdraw);
        send_to_char(buf, ch);
        sprintf(buf, "The request has %s\n\r", 
		clan_table[ch->clan].roster.approve ? "been approved." : 
		"not been approved.");   
        send_to_char(buf, ch);
        send_to_char("\n\r", ch);
      }
      sprintf(buf, "%-15s %-25s %-5s %-5s %-15s\n\r", "Name", "Rank", "Qps", "Rp", "Pending Actions"); 
      send_to_char(buf, ch);
      for (mbr = clan_table[ch->clan].roster.member; \
		mbr != NULL ; mbr = mbr->next )
      {
        int length;
	length = strlen(clan_table[ch->clan].rank[mbr->rank - 1]) - 
		 colorstrlen(clan_table[ch->clan].rank[mbr->rank -1]);
        if (mbr->name[0] != '\0')
        {
          nummem++;
          sprintf(buf, "%-15s %-*s %-5d %-5d %-15s\n\r", 
                    capitalize(mbr->name), 
		    25 + length,
		    clan_table[ch->clan].rank[mbr->rank - 1], 
		    mbr->qp, mbr->rp, mbr->action);
          send_to_char(buf, ch);
          found = TRUE;
        }
      }
      sprintf(buf, "Total number of members %d.\n\r", nummem);
      send_to_char(buf, ch);
    }
    else
    {
      sprintf(buf, "`&Roster for `*%s`&:`*\n\r", clan_table[ch->clan].who_name);    
      send_to_char(buf, ch);
      sprintf(buf, "Guild Quest Points: %d\n\r", 
		    clan_table[ch->clan].roster.questpoints);
      send_to_char(buf, ch); 
      send_to_char("\n\r", ch);
      if (clan_table[ch->clan].roster.withdraw > 0)
      {
        sprintf(buf, "Withdrawal request by %s made for %d Qps.\n\r", 
                     clan_table[ch->clan].roster.requestor, 
		     clan_table[ch->clan].roster.withdraw);
        send_to_char(buf, ch);
        sprintf(buf, "The request has %s\n\r", 
		clan_table[ch->clan].roster.approve ? "been approved." : "not been approved.");   
        send_to_char(buf, ch);
        send_to_char("\n\r", ch);
      }
      sprintf(buf, "%-15s %-25s %-5s %-5s\n\r", "Name", "Rank", "Qps", "Rp"); 
      send_to_char(buf, ch);
      for (mbr = clan_table[ch->clan].roster.member; 
		mbr != NULL ; mbr = mbr->next )
      {
        int length;
	length = strlen(clan_table[ch->clan].rank[mbr->rank - 1]) - 
		 colorstrlen(clan_table[ch->clan].rank[mbr->rank -1]);
        if (mbr->name[0] != '\0')
        {
          sprintf(buf, "%-15s %-*s %-5d %-5d\n\r", 
                    capitalize(mbr->name), 
		    25 + length,
		    clan_table[ch->clan].rank[mbr->rank - 1],
                    mbr->qp, mbr->rp);
          send_to_char(buf, ch);
          found = TRUE;
        }
      }
    }

    if (!found)
    {
      send_to_char("No members found.\n\r", ch);
      return;
    }
  }

  if (!str_cmp(arg1, "add"))
  {
    char arg2[MIL];
    char arg3[MIL];
    char buf[MIL];
    int rank;
 
    if (!IS_IMMORTAL(ch))
  {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if (arg2[0] == '\0' || arg3[0] == '\0' || !is_number(arg3))
    {
      send_to_char("Please give the name and rank #\n\r", ch);
      return;
    }

    rank = atoi(arg3);
 
    if (rank < 1 || rank > clan_table[clan].top_rank)
    {
      sprintf(buf, "Pick a rank between 1-%d\n\r", clan_table[clan].top_rank);
      send_to_char(buf, ch);
      return;
    }
	if(check_roster(arg2,clan)>0)
	{
		remove_member(arg2,clan);
		send_to_char("Duplicate found...deleting duplicate\n\r",ch);
	}
	
    add_member(arg2, rank, clan);
    send_to_char("Roster updated.\n\r", ch);
	
  }  


  if (!str_cmp(arg1, "change"))
  {
    char arg2[MIL];
    char arg3[MIL];
    int rank;

    if (!IS_IMMORTAL(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }


    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if (arg2[0] == '\0' || arg3[0] == '\0' || !is_number(arg3))
    {
      send_to_char("Please give the name and new rank #\n\r", ch);
      return;
    }

    rank = atoi(arg3);
 
    if (rank < 1 || rank > 6)
    {
      send_to_char("Pick a rank between 1-6\n\r", ch);
      return;
    }


    if (!change_member(arg2, rank, clan))
    {
      send_to_char("Member not found in roster.\n\r", ch);
      return;
    }


    send_to_char("Roster updated.\n\r", ch);
  }  


  if (!str_cmp(arg1, "withdraw"))
    {
      char command[MSL];
      int points; 

      if (!IS_IMMORTAL(ch) && ch->rank < 4)
      {
        send_to_char("Huh?\n\r", ch);
        return;
      }  

      argument = one_argument(argument, command);

      if (!str_cmp(command, "clear"))
      {
        if (!IS_IMMORTAL(ch) && 
                str_cmp(clan_table[ch->clan].roster.requestor, 
		ch->name))
        {
          send_to_char("That's not your request.  You can't clear it.\n\r", ch);
          return;
        }    
        clan_table[ch->clan].roster.withdraw = 0;
        clan_table[ch->clan].roster.approve = FALSE;
        clan_table[ch->clan].roster.requestor = str_dup("None");
        send_to_char("Withdraw cleared.\n\r", ch);
        return;
      }

      if (clan_table[ch->clan].roster.withdraw > 0)
      {
        if (clan_table[ch->clan].roster.approve == TRUE)
        {
          OBJ_DATA *token; 
          char tmp[MSL];
          int points;

          if (!IS_IMMORTAL(ch) && 
                str_cmp(clan_table[ch->clan].roster.requestor, 
			ch->name))
          {
            send_to_char("That's not your request.  You can't clear it.\n\r", ch);
            return;
          }    

          if ((token = create_object(get_obj_index(TOKEN_VNUM), ch->level)) == NULL)
          {
            send_to_char("Bugged please report.\n\r", ch);
            return;
          }
          points = clan_table[ch->clan].roster.withdraw;
          sprintf(tmp, "A Quest Point Token worth %d Qps", points);
          token->short_descr = str_dup(tmp);
          sprintf(tmp, "A token that is worth quest points lies here.");
          token->description = str_dup(tmp);
          token->value[0] = points;
          obj_to_char(token, ch);
          clan_table[ch->clan].roster.questpoints -= points;
          send_to_char("Token created.\n\r", ch);
          clan_table[ch->clan].roster.withdraw = 0;
          clan_table[ch->clan].roster.approve = FALSE;
          clan_table[ch->clan].roster.requestor = str_dup("None");

          return;
       }
       else
       {
         send_to_char("Your request for a withdrawal has not been approved.\n\r", ch);
         return;
       }
     } 

     if (!is_number(command))
     {
       send_to_char("Please choose an amount to withdraw.\n\r", ch);
       return;
     }
    
     points = atoi(command);
     if (points < 0 || points > clan_table[ch->clan].roster.questpoints)
     {
       send_to_char("Please choose a number between 0 and your roster's Qps.\n\r", ch);
       return;
     }

     clan_table[ch->clan].roster.withdraw = points;
     clan_table[ch->clan].roster.approve = FALSE;
     clan_table[ch->clan].roster.requestor = str_dup(ch->name);
     send_to_char("Request made.\n\r", ch);
  }  




  if (!str_cmp(arg1, "action"))
  {
    char name[MIL];
    char action[MIL];
    MEMBER_DATA *member;

    if (!IS_IMMORTAL(ch) && ch->rank < 4)
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    argument = one_argument(argument, name);
    argument = one_argument(argument, action);
      
    if (name[0] == '\0' || action[0] == '\0')
    {
      send_to_char("You must provide a name and action to do.\n\r", ch);
      return;
    }

    if ((member = find_member(name, ch->clan)) == NULL)
    {
      send_to_char("They are not in the roster.\n\r", ch);
      return;
    }


      

    if (!str_cmp(action, "outcast"))
    {
      if (ch->clan == clan_lookup("Tinker"))
      {
        send_to_char("Not in tinker.\n\r", ch);
        return;
      }

      member->action = str_dup("Outcast");
      send_to_char("They are set to be outcasted.\n\r", ch);
      return;
    }

    if (!str_cmp(action, "promote"))
    {
      char level[MIL];
      char buf[MSL];
      int rank;

      argument = one_argument(argument, level);
      if (level[0] == '\0' || !is_number(level))
      {
        send_to_char("You must provide a rank.\n\r", ch);
        return;
      }

      rank = atoi(level);
      if (rank <= member->rank)
      {
        send_to_char("That's not a promotion.\n\r", ch);      
        return;
      }
 
      if ((rank - 1) >= ch->rank) 
      {
        send_to_char("You don't have that power.\n\r", ch);
        return;
      }

      sprintf(buf, "Promote %d", rank);
      member->action = str_dup(buf);
      send_to_char("Promotion set.\n\r", ch);
    }

    if (!str_cmp(action, "demote"))
    {
      char level[MIL];
      char buf[MSL];
      int rank;

      argument = one_argument(argument, level);
      if (level[0] == '\0' || !is_number(level))
      {
        send_to_char("You must provide a rank.\n\r", ch);
        return;
      }

      rank = atoi(level);
      if ((rank - 1) >= member->rank)
      {
        send_to_char("That's not a demotion.\n\r", ch);      
        return;
      }
 
      if (rank - 1 >= ch->rank) 
      {
        send_to_char("You don't have that power.\n\r", ch);
        return;
      }

      sprintf(buf, "Demote %d", rank);
      member->action = str_dup(buf);
      send_to_char("Demotion set.\n\r", ch);
    }

    if (!str_cmp(action, "bestow"))
    {
      char arg[MIL];
      char buf[MSL];
      int i;    

      argument = one_argument(argument, arg);
      for (i = 0; i < clan_table[ch->clan].top_gskill; i++)
      {
        if (!str_cmp(arg, clan_table[ch->clan].gskill[i]))
        {
	  found = TRUE;
          break;
        }
      }

      if (!found)
      {
        send_to_char("No such guild skill in your guild.\n\r", ch);
        return;
      }

      sprintf(buf, "Bestow '%s'", arg);
      member->action = str_dup(buf);
      send_to_char("Bestow set.\n\r", ch); 
    }
           
 
  }
  if (!str_cmp(arg1, "remove"))
  {
    char arg2[MIL];

    if (!IS_IMMORTAL(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    argument = one_argument(argument, arg2);

    if (arg2[0] == '\0')
    {
      send_to_char("Please give the name and new rank #\n\r", ch);
      return;
    }

    if (!remove_member(arg2, clan))
    {
      send_to_char("Member not found in roster.\n\r", ch);
      return;
    }

    send_to_char("Member removed.\n\r", ch);
  }  

  if (!str_cmp(arg1, "points"))
  {
    char arg2[MIL];
    int points;

    argument = one_argument(argument, arg2);

    if (!IS_IMMORTAL(ch))
    { 
      return;
    }

    if (!is_number(arg2) || arg2[0] == '\0')
    {
      send_to_char("Please add a number of points.\n\r", ch);
      return;
    }  

    points = atoi(arg2);
    if (points < 0 || points > 32000)
    {
      send_to_char("Points must be between 0 and 32000\n\r", ch);
      return;
    }

    clan_table[ch->clan].roster.questpoints = points;
    send_to_char("Points added.\n\r", ch);
  }

  if (!str_cmp(arg1, "save"))
  {
    if (ch->level < MAX_LEVEL)
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }    
    save_roster();
    send_to_char("Saved.\n\r", ch);
  }
}

void save_one_roster(int clan)
{
  FILE *fp;
  char filename[MIL];
  sprintf(filename, "%s%s.txt", ROSTER_DIR, clan_table[clan].name);
  if ((fp = fopen(filename, "w")) == NULL)
  {
    bug("Save_roster: fopen", 0);
    return;
  }  
  else
  {
    int i = clan;
    MEMBER_DATA *mbr;
 
    fprintf(fp, "#"); 
    fprintf(fp, "%d \n", i);
    fprintf(fp, "%s~\n", clan_table[i].name[0] != '\0' ? clan_table[i].name : "None");
    fprintf(fp, "QuestPoints %d\n", clan_table[i].roster.questpoints);
 
    for (mbr = clan_table[i].roster.member ; mbr != NULL ; mbr = mbr->next)
    {
      fprintf(fp, "%s~\n%d\n", mbr->name, mbr->rank);
      fprintf(fp, "%d\n", mbr->qp); 
      fprintf(fp, "%d\n", mbr->rp); 
      fprintf(fp, "Pending %s~\n", mbr->action[0] == '\0' ? "None" : mbr->action);
      fprintf(fp, "LastLog %ld\n", mbr->lastLog);
    }
    fprintf(fp, "#End~\n");
    fclose(fp);
  }  
} 

void save_roster()
{
  int i;
  for (i = 0;i < top_clan;i++)
  {
    save_one_roster(i);
  }
}

bool load_one_roster(int clan)
{
  int i = clan;
  char filename[MIL];
  FILE *fp;
  char *word; 
  MEMBER_DATA *mbr;

  word = str_dup(" ");

  clan_table[i].roster.member = NULL;
  clan_table[i].roster.withdraw = -1;
  clan_table[i].roster.approve = FALSE;
  clan_table[i].roster.requestor = str_dup("None");

  sprintf(filename, "%s%s.txt", ROSTER_DIR, clan_table[i].name);
  if ((fp = fopen(filename, "r")) == NULL)
  {
    logf("Load_roster: fopen %s", clan_table[i].name);
    return FALSE;
  }  

  word[0] = fread_letter(fp);

  if (word[0] == '$')
    return FALSE;

//  logf("Roster Clan #%d", i);
  fread_number(fp);
  fread_string(fp);
  fread_word(fp);
  clan_table[i].roster.questpoints = fread_number(fp);
  for ( ; ; )
  {
     word = fread_string(fp); 
     if (!str_cmp(word, "#End"))
       break;
     else
     {   
       mbr = new_member();
       mbr->name = str_dup(word);
       mbr->rank = fread_number(fp);
       mbr->qp = fread_number(fp);
       mbr->rp = fread_number(fp);
       fread_word(fp);
       mbr->action = fread_string(fp);
       fread_word(fp);
       mbr->lastLog = fread_number(fp);
       mbr->next = clan_table[i].roster.member;
       clan_table[i].roster.member = mbr;
     }
  }
  free_string(word);
  fclose(fp);
  return TRUE;
}

void load_roster(void)             
{
  int i;
  for (i = 0; i < top_clan; i++)
  {
    load_one_roster(i);
  }
}


int check_roster (char *name,int clan)
{
	MEMBER_DATA *check;
	int count=0;
	for (check = clan_table[clan].roster.member ; check != NULL ; check = check->next)
    {
      if (!str_cmp(check->name, name))
      {
        count++;
      }
	}
	return count;
}

void add_member(char *name, int rank, int clan)
{
  MEMBER_DATA *nmem;
      
	nmem = new_member();
	nmem->name = str_dup(name);
	nmem->rank = rank;
	nmem->next = clan_table[clan].roster.member;
	clan_table[clan].roster.member = nmem;
	return;

}

bool change_member(char *name, int rank, int clan)
{
  MEMBER_DATA *mbr;

  for (mbr = clan_table[clan].roster.member ; mbr != NULL ; mbr = mbr->next)
  {
    if (!str_cmp(mbr->name, name))
    {
      mbr->rank = rank;
      return TRUE;
    }
  }
  return FALSE;
}

void member_rp(char *name, int clan, int level)
{
  MEMBER_DATA *mbr;


  for (mbr = clan_table[clan].roster.member ; mbr != NULL ; mbr = mbr->next)
  {
    if (!str_cmp(mbr->name, name))
    {
      mbr->rp += level;
      return;
    }
  }
  return;
}

MEMBER_DATA *find_member(char *name, int clan)
{
  MEMBER_DATA *mbr;

  for (mbr = clan_table[clan].roster.member ; mbr != NULL ; mbr = mbr->next)
  {
    if (!str_cmp(mbr->name, name))
    {
      return mbr;
    }
  }
  return NULL;
}

void member_quest(CHAR_DATA *ch, int points)
{
  MEMBER_DATA *mbr;

  if ((mbr = find_member(ch->name, ch->clan)) == NULL)
    return;

  mbr->qp += points;
} 

bool remove_member(char *name, int clan)
{
  MEMBER_DATA *mbr;
  //MEMBER_DATA *mbr_next;
  MEMBER_DATA *temp;

  if (!clan_table[clan].roster.member || 
	clan_table[clan].roster.member->name[0] == '\0')
    return FALSE;

  if (!str_cmp(name, clan_table[clan].roster.member->name))
  {
    mbr = clan_table[clan].roster.member;
    clan_table[clan].roster.member = clan_table[clan].roster.member->next;
    free_member(mbr);
    return TRUE;
  } 
  else
  {
    for (mbr = clan_table[clan].roster.member ; mbr != NULL ; mbr = mbr->next)
    {
      if (mbr->next && !str_cmp(mbr->next->name, name))
      {
        temp = mbr->next;
        mbr->next = mbr->next->next;
        free_member(temp);
        return TRUE;
      }
    }
  }
  return FALSE;
}

void check_action(CHAR_DATA *ch)
{
  MEMBER_DATA *member;
  char *action;
  char command[MIL];
  
  if ((member = find_member(ch->name, ch->clan)) == NULL)
    return;

  member->lastLog = current_time;
  member->rp = ch->rplevel;  
  action = str_dup(member->action);

  action = one_argument(action, command);
  if (!str_cmp(command, "Outcast"))
  {
    remove_member(ch->name, ch->clan);
    ch->clan = clan_lookup("Outcast");
    ch->rank = 0;
    send_to_char("You have been outcasted.\n\r", ch);
    member->action = str_dup("None");
    return;
  }

  if (!str_cmp(command, "Promote"))
  {
    char arg[MIL];
    char buf[MSL];
    int rank;

    action = one_argument(action, arg);
    
    rank = atoi(arg);
    ch->rank = rank - 1;
    sprintf(buf, "You have been promoted to %s.\n\r", clan_table[ch->clan].rank[ch->rank]);
    send_to_char(buf, ch);
    change_member(ch->name, rank, ch->clan);
    member->action = str_dup("None");
    return;
  }  

  if (!str_cmp(command, "Demote"))
  {
    char arg[MIL];
    char buf[MSL];
    int rank;

    action = one_argument(action, arg);
    
    rank = atoi(arg);
    ch->rank = rank - 1;
    sprintf(buf, "You have been demoted to %s.\n\r", clan_table[ch->clan].rank[ch->rank]);
    send_to_char(buf, ch);
    change_member(ch->name, rank, ch->clan);
    member->action = str_dup("None");
    return;
  }  
     
  if (!str_cmp(command, "Bestow"))
  {
    char arg[MIL];
    char buf[MSL]; 
    int sn = 0;

    action = one_argument(action, arg);
    sn = skill_lookup(arg);
    sprintf(buf, "You have been bestowed %s.\n\r", skill_table[sn].name);     
    send_to_char(buf, ch);
    ch->pcdata->learned[sn] = 75;
    member->action = str_dup("None");
    return;
  }
 
}

void save_guilds(void)
{
  FILE *fp;
  char filename[MIL];
  
  sprintf(filename, "%s%s", DATA_DIR, GUILDS_FILE);
  if ((fp = fopen(filename, "w")) == NULL)
  {
    bug("Save_guilds: fopen", 0);
    return;
  }
  else
  {
    int i, j;
    for (i = 0; i < top_clan; i++)
    {
      fprintf(fp, "Clan %d\n", i);
      fprintf(fp, "Name %s~\n", 
		clan_table[i].name[0] != '\0' ? 
		clan_table[i].name : "None");
      fprintf(fp, "WhoName %s~\n", 
		clan_table[i].who_name[0] != '\0' ? 
		clan_table[i].who_name : "None");
      fprintf(fp, "Vnum %d~\n",
		clan_table[i].vnum);
      fprintf(fp, "Display %s~\n", 
		clan_table[i].display[0] != '\0' ? 
		clan_table[i].display : "None");
      fprintf(fp, "Ranks %d\n", clan_table[i].top_rank);
      for (j = 0; j < clan_table[i].top_rank; j++)
      {
	fprintf(fp, "%d %s~\n", j, clan_table[i].rank[j]);    
      }
      fprintf(fp, "GSkills %d\n", clan_table[i].top_gskill);
      for (j = 0; j < clan_table[i].top_gskill; j++)
      {
	fprintf(fp, "%d %s~\n", j, clan_table[i].gskill[j]);    
      }
    
      fprintf(fp, "\n");
    }
    fprintf(fp, "#END\n");
    fclose(fp);
  }
}  

void load_guilds(void)
{
  FILE *fp;
  char filename[MIL];

  logf("Load_Guilds", 0);
  top_clan = 0;
  sprintf(filename, "%s%s", DATA_DIR, GUILDS_FILE);
  if ((fp = fopen(filename, "r")) == NULL)
  {
    bug("Load_guilds: fopen", 0);
    return;
  }
  else
  {
    char *word;
    int i;
    for ( ; ; )
    {
      word = fread_word(fp);
      if (!str_cmp(word, "#END"))
      {
        return;
      }

      fread_number(fp); // Get Clan number
      fread_word(fp); // Get rid of Name header
      clan_table[top_clan].name = fread_string(fp);

      fread_word(fp); // Get rid of WhoName header
      clan_table[top_clan].who_name = fread_string(fp);

      fread_word(fp); // Get rid of vnum header
      clan_table[top_clan].vnum = fread_number(fp);

      fread_word(fp); // Get rid of Display header
      clan_table[top_clan].display = fread_string(fp);

      fread_word(fp); // Get rid of ranks header

      clan_table[top_clan].top_rank = fread_number(fp); // Allow different numbers of ranks

      for (i = 0; i < clan_table[top_clan].top_rank; i++)
      {
        fread_number(fp);
        clan_table[top_clan].rank[i] = fread_string(fp);
      }

      fread_word(fp); // Get rid of ranks header

      // Allow different numbers of ranks
      clan_table[top_clan].top_gskill = fread_number(fp);
      for (i = 0; i < clan_table[top_clan].top_gskill; i++)
      {
        fread_number(fp);
        clan_table[top_clan].gskill[i] = fread_string(fp);
      }


      if (load_one_roster(top_clan) == FALSE)
      {
        clan_table[top_clan].roster.questpoints = 0;
      }
      top_clan++;
    }
    fclose(fp);
  }
}

