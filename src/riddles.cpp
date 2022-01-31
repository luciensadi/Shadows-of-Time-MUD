/* Yada Yada Riddle Code Seperated so it can be easily snippitized */

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "olc.h"
#include "riddles.h"

char *getline        args((char *str, char *buf));

const struct riddle_cmd riddle_cmd_table[] = {
  {"add_riddle", add_riddle},
  {"question", riddle_question},
  {"answer", riddle_answer},
  {"difficulty", riddle_difficulty}, 
  {"save", riddle_save}, 
  {"?", riddle_help},
  {"", 0},
};

bool run_riddle_editor( DESCRIPTOR_DATA *d )
{
  if (d->editor == ED_RIDDLE)
  {
    riddle_edit(d->character, d->incomm);
    return TRUE;
  }
  return FALSE;
}
RIDDLE_DATA *get_riddle_index(int vnum)
{
  RIDDLE_DATA *riddle;
  
  for (riddle = riddle_hash[vnum % MAX_KEY_HASH];
        riddle != NULL; riddle = riddle->next)
  {
    if (riddle->vnum == vnum)
      return riddle;
  }
  return NULL;
}


void do_riddle_edit( CHAR_DATA *ch, char *argument)
{
  RIDDLE_DATA *riddle;
  char arg[MIL];
  if ( IS_NPC(ch))
    return;

  argument = one_argument(argument, arg); 
  if (!str_cmp(arg, "add"))
  {
     add_riddle(ch, argument);
     return;
  }

  if ((riddle = get_riddle_index(atoi(arg))) == NULL)
  {
    send_to_char("No such riddle.\n\r", ch);
    return;
  }
  
  ch->desc->editor = ED_RIDDLE;
  ch->desc->pEdit = riddle;
  return;
}  

void riddle_edit(CHAR_DATA *ch, char *argument)
{
  RIDDLE_DATA *riddle;

  EDIT_RIDDLE(ch, riddle);


  if (argument[0] == '\0' || argument[1] == '\0')
  {
    show_riddle(ch);
    return;
  }

  if (!str_cmp(argument, "done"))
  {
    send_to_char("Exiting riddle editor.", ch);
    ch->desc->editor = ED_NONE;
    return;
  }

  riddle_interpret(ch, argument);
  return;
}

void riddle_help(CHAR_DATA *ch, char *argument)
{
} 
    
void show_riddle(CHAR_DATA *ch)
{
  RIDDLE_DATA *riddle;
  char buf[MSL];

  EDIT_RIDDLE(ch, riddle);
 
  sprintf(buf, "Vnum:   %d\n\r", riddle->vnum);
  send_to_char(buf, ch);
  sprintf(buf, "Difficulty: %d\n\r\n\r", riddle->difficulty);
  send_to_char(buf, ch);
  sprintf(buf, "Question: %s\n\r", riddle->question);
  send_to_char(buf, ch);
  sprintf(buf, "Answer: %s\n\r", riddle->answer);
  send_to_char(buf, ch);
}

void add_riddle(CHAR_DATA *ch, char *argument)
{
  RIDDLE_DATA *riddle;
  int vnum;
  int iHash;  

  vnum = top_vnum_riddle + 1;

  iHash = vnum % MAX_KEY_HASH;
  riddle = new_riddle();
  riddle->next = riddle_hash[iHash];
  riddle_hash[iHash] = riddle;
  top_vnum_riddle++;   
  riddle->vnum = vnum;
  riddle->difficulty = 15;
  riddle->question = str_dup("Question");
  riddle->answer = str_dup("Answer");
  riddle_list = riddle; 
  ch->desc->editor = ED_RIDDLE;
  ch->desc->pEdit = riddle;
  return;
}

void riddle_interpret(CHAR_DATA *ch, char *argument)
{
  bool found = FALSE;
  char arg[MIL];
  int cmd;

  argument = one_argument(argument, arg);


  for ( cmd = 0; riddle_cmd_table[cmd].name[0] != '\0';cmd++)
  {
    if ( !str_prefix(arg, riddle_cmd_table[cmd].name) )
    {
      found = TRUE;
      break;
    }
  }

  if (found)
  {
    (riddle_cmd_table[cmd].riddle_fun) (ch, argument);
  }
  else
  {
    show_riddle(ch);
  }
}

void riddle_question(CHAR_DATA *ch, char *argument)
{
  RIDDLE_DATA *riddle;

  EDIT_RIDDLE(ch, riddle);
  free_string(riddle->question);
  if (!str_cmp(riddle->question, "Question"))
    riddle->question = str_dup("");
  if (argument[0] != '\0')
  {
    send_to_char("Type question to enter editting mode.\n\r", ch);
    return;
  } 
  string_append(ch, &riddle->question);
}

void riddle_answer(CHAR_DATA *ch, char *argument)
{
  RIDDLE_DATA *riddle;

  EDIT_RIDDLE(ch, riddle);
  free_string(riddle->answer);
  if (!str_cmp(riddle->answer, "Answer")) 
    riddle->answer = str_dup("");
  if (argument[0] != '\0')
  {
    send_to_char("Type answer to enter editting mode.\n\r", ch);
    return;
  } 
  string_append(ch, &riddle->answer);
}

void riddle_difficulty(CHAR_DATA *ch, char *argument)
{
  RIDDLE_DATA *riddle;

  EDIT_RIDDLE(ch, riddle);
  if (!is_number(argument))
  {
    send_to_char("In number format please.\n\r", ch);
    return;
  }
  
  riddle->difficulty = atoi(argument);  
}

void riddle_save(CHAR_DATA *ch, char *argument)
{
  char buf[MIL];
  save_riddle();
  sprintf(buf, "%d", top_vnum_riddle);
  send_to_char(buf, ch); 
  send_to_char("Saved.\n\r", ch);
}

bool riddle_lookup(RIDDLE_DATA *riddle, char *guess)
{ 
  char buf[MAX_STRING_LENGTH];
  char *string; 

  buf[0] = '\0';

  string = str_dup(riddle->answer);
  while (*string) 
  {
     string = getline (string, buf);
     if (!str_cmp(guess, buf))
       return TRUE;
  }

  return FALSE;
}   



