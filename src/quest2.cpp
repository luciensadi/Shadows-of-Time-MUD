/* Asmo Quest System */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "merc.h"
#include "quest2.h"

void do_questinfo(CHAR_DATA *ch, char *argument)
{
  QUEST_DATA *list;
  CHAR_DATA *victim;

  char buf[MSL];
  char arg1[MIL];
  char vname[MIL];

  if (IS_IMMORTAL(ch))
    argument = one_argument(argument, vname);
  else
    sprintf(vname, "self");
  argument = one_argument(argument, arg1);
 
  if ((victim = get_char_world(ch, vname)) == NULL)   
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("Not on NPCs.\n\r", ch);
    return;
  }

  if (!str_cmp(arg1, "clear"))
  {
    int number;
    
    if (!IS_IMMORTAL(ch))
      return;

    if (!is_number(argument))
      return;

    number = atoi(argument);

    list = victim->pcdata->quests;
    if (list->number == number)
    {
      victim->pcdata->quests = list->next; 
      delete list;
      send_to_char("Cleared.\n\r", ch);
      return;
    }

    for (list = victim->pcdata->quests; list != NULL; list = list->next)
    {
      if (list->next && list->next->number == number)
      {
        QUEST_DATA *quest = list->next;
        list->next = list->next->next;
        delete quest;
        send_to_char("Cleared.\n\r", ch);
        return;
      }
   }

   send_to_char("No such quest.\n\r", ch);
   return;
  }
 
  if (!str_cmp(arg1, "raw"))
  {
    if (!IS_IMMORTAL(ch))
      return;

    for (list = victim->pcdata->quests; list != NULL; list = list->next)
    {
      sprintf(buf, "Quest #%d\n\r", list->number);
      send_to_char(buf, ch);
      sprintf(buf, "Status %d\n\r", list->status);
      send_to_char(buf, ch);
      send_to_char("Steps done: ", ch);
      for (int i = 0;i < 32; i++)
      {
        if (IS_SET(list->bit, long(pow(2, i))) )
        {
          sprintf(buf, "%d ", i);
          send_to_char(buf, ch);
        }
      }
      send_to_char("\n\r", ch);
    }
  }




  for (list = victim->pcdata->quests;list != NULL;list = list->next)
  {
    if (list->status != 0)
      continue;
    else
    {
      QUEST_INFO_DATA *quest;   
      bool found = FALSE;      
      for (quest = quest_info_list;quest != NULL;quest = quest->next)
      {
        if (quest->number == list->number)
        {
          found = TRUE;
          break;
        }
      }

      int i = 0;
      for(i = 0;i < 32;i++)
        if ( !IS_SET(list->bit, (long)(pow(2, i)) ))
          break;

      int stepdone = i - 1;

      if (!found)
      {
        sprintf(buf, "You are on quest %d step %d\n\r", list->number, stepdone + 1);
        send_to_char(buf, ch);
        send_to_char("-----------------------------------------------------------------\n\r", ch);
        continue;
      }

      sprintf(buf, "`![`1Quest:`!]`7 %s\n\r", quest->name);
      send_to_char(buf, ch);
      sprintf(buf, "`![`1Step:`!]`7\n\r%s\n\r", quest->step[stepdone] ? quest->step[stepdone] : "No information");
      send_to_char(buf, ch); 
      send_to_char("`&-----------------------------------------------------------------`7\n\r", ch);
    }
  }
}


void add_quest(CHAR_DATA *ch, int number)
{
  QUEST_DATA *list;


  for (list = ch->pcdata->quests; list != NULL; list = list->next)
  {
    if (list->number == number)
    {
      logf("Tried to add quest that was already there %d", number);
      return;
    }
  }

  list = new QUEST_DATA;
  list->number = number;
  list->bit = 1;
  list->status = QUEST_INPROGRESS;
  list->next = ch->pcdata->quests;
  ch->pcdata->quests = list;
}

bool has_quest(CHAR_DATA *ch, int number)
{
  QUEST_DATA *list;

  for (list = ch->pcdata->quests; list != NULL; list = list->next)
  {
    if (list->number == number)
      return TRUE;
  }

  return FALSE;
}

bool on_quest(CHAR_DATA *ch, int number)
{
  QUEST_DATA *list;

  for (list = ch->pcdata->quests; list != NULL; list = list->next)
  {
    if (list->number == number && list->status == QUEST_INPROGRESS)
      return TRUE;
  }

  return FALSE;
}
 


bool has_completed_quest(CHAR_DATA *ch, int number)
{
  QUEST_DATA *list;

  for (list = ch->pcdata->quests; list != NULL; list = list->next)
  {
    if (list->number == number)
    {
      return (list->status == QUEST_COMPLETED);
    }
  }

  return FALSE;
}  

void quest_completed_step(CHAR_DATA *ch, int number, int bit)
{
  QUEST_DATA *list;
  bool found = FALSE;

  logf("Completed %d %d", number, bit);

  for (list = ch->pcdata->quests; list != NULL; list = list->next)
  {
    if (list->number == number)
    {
      found = TRUE;
      break;
    }
  }

  if (!found)
    return;

  SET_BIT(list->bit, long(pow(2, bit)));
}

bool has_completed_quest_step(CHAR_DATA *ch, int number, int step)
{
  QUEST_DATA *list;
  bool found = FALSE;

  logf("Has completed %d %d", number, step);
  for (list = ch->pcdata->quests; list != NULL; list = list->next)
  {
    if (list->number == number)
    {
      found = TRUE;
      break;
    }
  }

  if (!found)
    return FALSE;

  return IS_SET(list->bit, long(pow(2, step)));  
} 

void update_quest_status(CHAR_DATA *ch, int number, char *status)
{
  QUEST_DATA *list;
  bool found = FALSE;

  for (list = ch->pcdata->quests; list != NULL; list = list->next)
  {
    if (list->number == number)
    {
      found = TRUE;
      break;
    }
  }

  if (!found)
    return;
  

  if (!str_cmp(status, "completed"))
    list->status = QUEST_COMPLETED;

  if (!str_cmp(status, "failed"))
    list->status = QUEST_FAILED;

  if (!str_cmp(status, "inprogress"))
    list->status = QUEST_INPROGRESS;

}

void do_questedit(CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];

  argument = one_argument(argument, arg1);

  if (argument[0] == '\0')
  {
    send_to_char("The following options are available:\n\r", ch);
    send_to_char("       qedit list - shows list of all current quests\n\r", ch);
    send_to_char("       qedit save - saves the quest list in its current form\n\r", ch);
    send_to_char("       qedit create - creates a new quest\n\r", ch);
    send_to_char("       qedit # name - gives a name to a quest\n\r", ch);
    send_to_char("       qedit # step # - allowed you to edit the quest steps\n\r", ch);
    send_to_char("       qedit # show - shows the full contents of the quest\n\r", ch);
  }
  if (!str_cmp(arg1, "save"))
  {
    save_quest_info();
    send_to_char("Saved.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg1, "list"))
  {
    char buf[MSL];
    QUEST_INFO_DATA *quest;
  
    send_to_char("Current quests.\n\r", ch);
    for (quest = quest_info_list;quest != NULL;quest = quest->next)
    {
      sprintf(buf, "%d - %s\n\r", quest->number, quest->name);
      send_to_char(buf, ch);
    }
    return;
  }
  else if (!str_cmp(arg1, "create"))
  {
    char buf[MSL];

    if (!is_number(argument))
    {
      send_to_char("What quest number do you want to create?\n\r", ch);
      return;
    }

    int number = atoi(argument);
    QUEST_INFO_DATA *quest;

    for (quest = quest_info_list; quest != NULL; quest = quest->next)
    {
      if (quest->number == number)
      {
        send_to_char("That quest already exists.\n\r", ch);
        return;
      }
    }

    quest = new QUEST_INFO_DATA;
    quest->number = number;
    quest->name = str_dup("Not set");
    quest->topstep = 0;
    quest->next = quest_info_list;
    quest_info_list = quest;

    sprintf(buf, "Quest #%d added.\n\r", number);
    send_to_char(buf, ch);
    return;
  }
  else if (is_number(arg1))
  {
    char arg2[MIL];
    bool found = FALSE;
    QUEST_INFO_DATA *quest;

    int number = atoi(arg1);

    argument = one_argument(argument, arg2);

    for (quest = quest_info_list;quest != NULL;quest = quest->next)
    {
      if (quest->number == number)
      {
        found = TRUE;
        break;
      }
    }        
    
    if (!found)
    {
      send_to_char("No such quest to edit\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "name"))
    {
      if (!IS_NULLSTR(quest->name))
        free_string(quest->name);
      quest->name = str_dup(argument);
      send_to_char("Name updated.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg2, "step"))
    {
      char arg3[MIL];

      argument = one_argument(argument, arg3);
      if (!is_number(arg3))
      {
        send_to_char("Which step do you want to edit?\n\r", ch);
        return;
      }

      int stepnumber = atoi(arg3);
      if (stepnumber > 0)
        stepnumber--;

      if (stepnumber >= quest->topstep)
      {
        quest->step[stepnumber] = str_dup("");
        quest->topstep++;
      }

      string_append(ch, &quest->step[stepnumber]);
      return;
    }
    else if (!str_cmp(arg2, "show"))
    {
      char buf[MSL];
      sprintf(buf, "%d - %s\n\r", quest->number, quest->name);
      send_to_char(buf, ch);
      send_to_char("Steps:\n\r", ch);
      for (int i = 0;i < quest->topstep;i++)
      {
        sprintf(buf, "%d - %s\n\r", i + 1, quest->step[i]);
        send_to_char(buf, ch);
      }
    }
    else
    {
      send_to_char("Do what to that quest.\n\r", ch);
      return;
    }

  }
  else
  {
    send_to_char("Don't know what you want me to do.\n\r", ch);
    return;
  }
  
}

void save_quest_info()
{
  QUEST_INFO_DATA *quest;
  FILE *fp;

  if ((fp = fopen("../data/quests.txt", "w")) == NULL)
  {
    bug("save_quest_info: fopen", 0);
    return;
  }

  for (quest = quest_info_list; quest != NULL; quest = quest->next)
  {
    fprintf(fp, "Quest %d %d %s~\n", quest->number, quest->topstep, quest->name);
    for (int i = 0;i < quest->topstep;i++)
    {
      fprintf(fp, "Step %d %s~\n", i, quest->step[i]);
    }
    fprintf(fp, "End\n");
  }
  fprintf(fp, "#End\n");
  fclose(fp);
}

void load_quest_info()
{
  QUEST_INFO_DATA *quest;
  FILE *fp;
  char *word;

  if ((fp = fopen("../data/quests.txt", "r")) == NULL)
  {
    bug("Load_quest_info: fopen", 0);
  }
  else
  {
    word = fread_word(fp);
    while (str_cmp(word, "#End"))
    {
      quest = new QUEST_INFO_DATA;
      quest->number = fread_number(fp);
      quest->topstep = fread_number(fp);
      quest->name = fread_string(fp);
      quest->next = quest_info_list;
      quest_info_list = quest;
      word = fread_word(fp);
      while (str_cmp(word, "End"))
      {
        int step = fread_number(fp);
        quest->step[step] = fread_string(fp);
        word = fread_word(fp);
      }
      word = fread_word(fp);
    }
    fclose(fp);
  }
}
