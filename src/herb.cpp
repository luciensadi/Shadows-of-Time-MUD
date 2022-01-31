/***********************************
**				  **
**  Herbalist Code - Diem         **
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


void do_plant(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int chance;
  int charlevel;
  int objlevel;
//  int newplant;
	argument = one_argument(argument, arg);
  
  if (get_skill(ch, gsn_plant) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if ((obj = get_obj_carry(ch, arg, ch)) == NULL)
  {
    send_to_char("You aren't carrying that", ch);
    return;
  }
  if (obj->item_type != ITEM_SEED)
  {
  	send_to_char("You can only plant seeds.\n\r", ch);
  	return;
  }

  
  
  charlevel = ch->level;
  objlevel = obj->level;
  
  
  chance = get_skill(ch, gsn_plant);
  chance -= (objlevel - charlevel);
  
  
  
  
  if (IS_IMMORTAL(ch))
    chance = 100;
  
  
  
  if (number_percent() < chance)
  {
    act("You plant $p.", ch, obj, NULL, TO_CHAR);
    act("$n plants $p.", ch, obj, NULL, TO_ROOM);
    obj->item_type = ITEM_PLANT;
    obj_from_char(obj);
//    do_function(ch, &do_oload, seed_type[obj->pIndexData->vnum].growsplant);
    check_improve(ch, gsn_plant, TRUE, 1);
  }
  else
  {
  	send_to_char("No Deal!\n\r", ch);
  	return;
  	
  }
 }
 


