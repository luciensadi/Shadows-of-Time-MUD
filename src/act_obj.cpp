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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"
#include "bomb.h"
#include "lookup.h"

/*
 * Local functions.
 */
#define CD CHAR_DATA
#define OD OBJ_DATA
bool remove_obj args ((CHAR_DATA * ch, int iWear, bool fReplace));
void wear_obj args ((CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace));
CD *find_keeper args ((CHAR_DATA * ch));
int get_cost args ((CHAR_DATA * keeper, OBJ_DATA * obj, bool fBuy));
void obj_to_keeper args ((OBJ_DATA * obj, CHAR_DATA * ch));
OD *get_obj_keeper
args ((CHAR_DATA * ch, CHAR_DATA * keeper, char *argument));
int clan_lookup  args( (const char *name) ); 
void member_quest args (( CHAR_DATA *ch, int points ));


void    check_trap              args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void save_one_roster args (( int clan ));

#undef OD
#undef    CD

/* RT part of the corpse looting code */

void do_masturbate( CHAR_DATA *ch, char *argument )
{
   int condition;
   AFFECT_DATA af;

  if (IS_NPC(ch))
    return;

  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HORNY] >= 40 && ch->pcdata->condition[COND_GOBLIND] == 0)
  {
        send_to_char( "You masturbate any more and you'll go blind!\n\r",ch);
        ch->pcdata->condition[COND_GOBLIND] = 1;
        return;
   }
 
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HORNY] >= 40)
    {
        if ( IS_AFFECTED(ch, AFF_BLIND))
         {
            send_to_char( "You are too blind to see the hair growing on your palms.\n\r", ch);
            if (IS_IMMORTAL(ch))
              ch->pcdata->condition[COND_GOBLIND] = 0;
            return;
         }
        af.where     = TO_AFFECTS;
        af.type      = gsn_blindness;
        af.level     = ch->level;
        af.location  = APPLY_HITROLL;
        af.modifier  = -4;
        af.duration  = 2;
        af.bitvector = AFF_BLIND;
        affect_to_char( ch, &af );
        send_to_char( "You are blinded!\n\r", ch );
        act("$n appears to be blinded.",ch,NULL,NULL,TO_ROOM);
        return;
    }
  
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HORNY] != 0)
    {
        send_to_char("You don't feel horny.\n\r", ch );
        return;
    }
         
    condition = ch->pcdata->condition[COND_HORNY];
    while (condition < 45)
    {
 
      condition = ch->pcdata->condition[COND_HORNY];
      if ( condition < 9 )
      {
         act( "$n lays on the ground and starts to masturbate.", ch, NULL, NULL, TO_ROOM );
         send_to_char( "You lay down and masturbate to naughty thoughts.\n\r", ch);
      }
      else if ( ch->pcdata->condition[COND_HORNY] >= 9 && ch->pcdata->condition[COND_HORNY] < 18)
      {
        act( "$n really starts to get frisky with himself.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "You start to get really excited.\n\r", ch);
      }
      else if ( ch->pcdata->condition[COND_HORNY] >= 18 && ch->pcdata->condition[COND_HORNY] < 27)
      {
          act( "$n is starting to make load noises.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You start to moan and groan.\n\r", ch);
       }
      else if ( ch->pcdata->condition[COND_HORNY] >= 27 && ch->pcdata->condition[COND_HORNY] < 36)
        {
          act( "$n is starting to shake visibly.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "Your leg starts to spasm uncontrollably.\n\r", ch);
        }
    else if ( ch->pcdata->condition[COND_HORNY] >= 45 )
        {
          act( "$n shakes hard and then spooges all over the place.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You shake and spooge all over the place.  You are totally sated\n\r", ch);
        }
        gain_condition( ch, COND_HORNY, 9);
    }
    ch->pcdata->condition[COND_GOBLIND] = 0;
}


void tax(CHAR_DATA *ch)
{
  int temp;
  long wealth;
  long figure;
  char buf[MSL];
  wealth = ch->gold + (ch->silver / 100);

  if (wealth > 50000)
  {
    send_to_char("You're amazingly rich, welcome to the 44% tax bracket.  Steal from the rich, give to the poor.\n\r", ch);
    temp = int(wealth * .44);
  }
  else if (wealth > 30000)
  {
    send_to_char("You're fairly wealthy, welcome to the 35% tax bracket.  Steal from the rich, give to the poor.\n\r", ch);
    temp = int(wealth * .35);
  }
  else if (wealth > 15000)
  {
    send_to_char("You're in the middle class, welcome to the 23% tax bracket.  Pay your debt to society.\n\r", ch);
    temp = int(wealth * .23);
  }
  else if (wealth > 10000)
  {
    send_to_char("You're in the lower middle class, welcome to the 12% tax bracket.  Pay your debt to society.\n\r", ch);
    temp = int(wealth * .12);
  }
  else
    return;

  figure = temp * 100;
  if (ch->silver > figure)
  {
    sprintf(buf, "You have been taxed %ld silver.\n\r", figure);
    send_to_char(buf, ch);
    ch->silver -= figure;
    return;
  }
  else
  {
    sprintf(buf, "You have been taxed %ld silver and %ld gold.\n\r", ch->silver, (figure - ch->silver) / 100);
    send_to_char(buf, ch);
    ch->gold -= (figure - ch->silver) / 100;
    ch->silver = 0;
  }
}

bool can_loot (CHAR_DATA * ch, OBJ_DATA * obj)
{
    CHAR_DATA *owner, *wch;

    if (IS_IMMORTAL (ch))
        return TRUE;

    if (!obj->owner || IS_NULLSTR(obj->owner))
        return TRUE;

    if (!str_cmp (ch->name, obj->owner))
        return TRUE;

    owner = NULL;
    for (wch = char_list; wch != NULL; wch = wch->next)
        if (!str_cmp (wch->name, obj->owner))
            owner = wch;

    if (!IS_NULLSTR(obj->lastKilled))
    {
      if (!str_cmp(ch->name, obj->lastKilled) && obj->lootsLeft > 0)
      {
        return TRUE;
      }
      else
	return FALSE;	 
    }

    if (owner == NULL)
        return TRUE;


    if (is_same_group (ch, owner))
        return TRUE;

    if (obj->item_type != ITEM_CORPSE_PC)
      return TRUE;

    return FALSE;
}

void do_throw(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *obj;
  char arg1[MIL];
  char arg2[MIL];
  CHAR_DATA *victim;
  int chance;
  bool no_arg = FALSE;
  bool found = FALSE;
  
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0')
  {
    send_to_char("Throw what at whom?\n\r", ch);
    return;
  }


  if (arg2[0] == '\0')
  { 
     if (ch->fighting)
     {
       no_arg = TRUE;
     }
     else
     {
       send_to_char("Throw what at whom?\n\r", ch);
       return;
     }
  }

  if ((obj = get_obj_carry(ch, arg1, ch)) == NULL)
  {
    found = FALSE;
  }
  else
   found = TRUE;
  
  if (!found)
  {
    OBJ_DATA *bag;
    bool has_bag = FALSE;
    
    // Check if they have a bomb bag
    for (bag = ch->carrying; bag != NULL; bag = bag->next)
    {
      if (bag->item_type == ITEM_CONTAINER && IS_OBJ_STAT(bag, ITEM_BOMB_BAG))
      {
      	has_bag = TRUE;
        break;
      }
    }
    
    if (!has_bag)
    {
      send_to_char("You don't have that.\n\r", ch);
      return;
    } 
    
    // Check if there's a bomb in that bag
    if ((obj = get_obj_list(ch, arg1, bag->contains)) == NULL)
    {
      act("You aren't carrying that and there isn't anything like that in $p .", 
      	   ch, bag, NULL, TO_CHAR);
      return;
    }	
  
    get_obj(ch, obj, bag, FALSE);
  }

  if (obj->item_type != ITEM_BOMB)
  {
    send_to_char("Why would you want to throw that?\n\r", ch);
    return;
  }

  if (no_arg)
  {
    victim = ch->fighting;
  }
  else
  { 
    if ((victim = get_char_room (ch, NULL, arg2)) == NULL)
    {
      send_to_char("They aren't here.\n\r", ch); 
      return;
    }
  }

  if (is_safe(ch, victim))
  {
    return;
  } 

  chance = number_percent();
  if (!IS_NPC(ch) && !IS_NPC(victim))
    chance -= ch->level - victim->level;
  chance -= dex_app[get_curr_stat(victim, STAT_DEX)].offensive; 
             /* lower chance, the more likely to go off */
  if (!IS_NPC(ch) && str_cmp(class_table[ch->cClass].name, "illuminator"))
  {
    chance += 60;
    /* If not an illuminator less chance to hit */
  }

   act("You throw a $p at $N.", ch, obj, victim, TO_CHAR);
   act("$n throws a $p.", ch, obj, victim, TO_ROOM);

  if (chance < 85)
  {
    if (obj->value[0] == TRG_CONTACT)
    {
       free_string(obj->owner);
       obj->owner = str_dup(ch->name);
       (*explosive_table[obj->value[1]].bomb_fun) (obj, victim);
       extract_obj(obj);
    }
    else 
    {
      act("$p hits $N with a thud then falls harmlessly to the ground.", ch, obj, victim, TO_CHAR);
      act("$p hits you with a thud then falls harmlessly to the ground.", ch, obj, victim, TO_VICT);
      act("$p hits $N with a thud then falls harmlessly to the ground.", ch, obj, victim, TO_NOTVICT);
      obj_from_char(obj);
      obj_to_room(obj, ch->in_room);
    }
  }
  else
  {
    if (obj->value[0] == TRG_CONTACT)
    {
       if (ch->cClass != class_lookup("Illuminator") && number_bits(2) == 0)
       {
         free_string(obj->owner);
         obj->owner = str_dup(ch->name);
         act("You grab too tight and $p explodes in your hand.", ch, obj, victim, TO_CHAR);
         (*explosive_table[obj->value[1]].bomb_fun) (obj, ch);
         extract_obj(obj);
       }
       else
       {
         act("$p misses $N and explodes on the ground.", ch, obj, victim, TO_CHAR);
         act("$p misses you and explodes on the ground.", ch, obj, victim, TO_VICT);
         act("$p misses $N and explodes on the ground.", ch, obj, victim, TO_NOTVICT);
         extract_obj(obj);
       }
    }
    else
    {
      act("$p misses $N and falls harmlessly to the ground.", ch, obj, victim, TO_CHAR);
      act("$p misses you and falls harmlessly to the ground.", ch, obj, victim, TO_VICT);
      act("$p misses $N and falls harmlessly to the ground.", ch, obj, victim, TO_NOTVICT);
      obj_from_char(obj);
      obj_to_room(obj, ch->in_room);
    }
   }
   WAIT_STATE(ch, 16);
   damage(ch, victim, 0, TYPE_UNDEFINED, DAM_NONE, FALSE, FALSE);
}

void do_restring(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *temp;
  OBJ_DATA *coupon;
  char arg1[MIL];
  char tempbuf[MSL];
  char buf[MSL];
  char tempshort[MSL];

  argument = one_argument(argument, arg1);


  if (arg1[0] == '\0')
  {
    send_to_char("Syntax is: restring <object>       - To turn in the object you want restrung.\n\r", ch);
    send_to_char("           restring short <desc>   - To restring the short description.\n\r", ch);
    send_to_char("           restring long <desc>    - To restring the long description.\n\r", ch);
    send_to_char("           restring show           - Show current strings.\n\r", ch); 
    send_to_char("           restring done           - Finish restring and return your item.\n\r", ch);
    return;
  }

  if (!str_cmp(arg1, "done"))
  {
    char *temparg;
    bool found = FALSE;

    if (restring_info.item == NULL)
    {
      send_to_char("No item to return.\n\r", ch);
      return;
    }

    if (restring_info.owner != ch)
    {
      send_to_char("Restringer is being used by someone else right now.\n\r", ch);
      return;
    }

    temparg = str_dup(restring_info.item->name);
    colourstrip(tempbuf, restring_info.item->short_descr);

    while (temparg[0] != '\0')
    { 
      char looking[MIL];
      temparg = one_argument(temparg, looking);
      if ((strstr(tempbuf, capitalize(looking)))
       || (strstr(tempbuf, looking)) )
      {
        found = TRUE;
        break;
      }
    }

    if (!found)
    {
      send_to_char("None of the keyword(s) are in the short.  This is a\n\r", ch);
      send_to_char("requirement, please fix.\n\r", ch);
      return;
    }
 
    obj_to_char(restring_info.item, ch);
    send_to_char("Object returned.\n\r", ch);
    restring_info.item = NULL;
    restring_info.owner = NULL;
    return;
  }

  if (!str_cmp(arg1, "cancel"))
  {
    OBJ_DATA *coupon;

    if (!IS_IMMORTAL(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if (restring_info.item == NULL)
    {
      send_to_char("No item to return.\n\r", ch);
      return;
    }

 
    obj_to_char(restring_info.item, restring_info.owner);
    send_to_char("Object returned.\n\r", ch);
    coupon = create_object(get_obj_index(6400), 90);
    obj_to_char(coupon, restring_info.owner);  
    send_to_char("Restring cancelled by immortal objects returned.\n\r", restring_info.owner);
    restring_info.item = NULL;
    restring_info.owner = NULL;
    return;
  }

  if (!str_cmp(arg1, "status"))
  {

    if (!IS_IMMORTAL(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if (restring_info.item == NULL || restring_info.owner == NULL)
      sprintf(buf, "No item being restrung.\n\r");
    else 
      sprintf(buf, "%s is currently restringing.\n\r", restring_info.owner->name);
    send_to_char(buf, ch);
    return; 
  }   

  if (!str_cmp(arg1, "show"))
  {
    char buf[MSL];

    if (restring_info.item == NULL)
    {
      send_to_char("No item to restring.\n\r", ch);
      return;
    }

    if (restring_info.owner != ch)
    {
      send_to_char("Restringer is being used by someone else right now.\n\r", ch);
      return;
    }

    sprintf(buf, "Object name:  %s\n\r", restring_info.item->name);
    send_to_char(buf, ch);
    sprintf(buf, "Object short: %s\n\r", restring_info.item->short_descr);
    send_to_char(buf, ch);
    sprintf(buf, "Object long:  %s\n\r", restring_info.item->description);
    send_to_char(buf, ch);

    return;       
  }


/*
  if (!str_cmp(arg1, "name"))
  {
    if (restring_info.item == NULL)
    {
      send_to_char("No item to restring.\n\r", ch);
      return;
    }

    if (restring_info.owner != ch)
    {
      send_to_char("Restringer is being used by someone else right now.\n\r", ch);
      return;
    }

    if (argument[0] == '\0')
    {
      send_to_char("Restring name to what?\n\r", ch);
      return;
    }

    if (!strstr(buf, tempbuf))       
    {
       send_to_char("That's not part of the short.  Remember capitalization does count\n\r", ch);
       return;
    }
    free_string(restring_info.item->name);
    restring_info.item->name = str_dup(tempbuf);
    send_to_char("Keyword changed.\n\r", ch);
    return;       
  }
*/
  if (!str_cmp(arg1, "short"))
  {
    if (restring_info.item == NULL)
    {
      send_to_char("No item to restring.\n\r", ch);
      return;
    }

    if (restring_info.owner != ch)
    {
      send_to_char("Restringer is being used by someone else right now.\n\r", ch);
      return;
    }

    if (argument[0] == '\0')
    {
      send_to_char("Restring short to what?\n\r", ch);
      return;
    }


    free_string(restring_info.item->short_descr);       
    restring_info.item->short_descr = str_dup(argument);
    colourstrip(buf, restring_info.item->short_descr);
    free_string(restring_info.item->name);
    restring_info.item->name = str_dup(buf);
    send_to_char("Short Description changed.\n\r", ch);
    return;       
  }

  if (!str_cmp(arg1, "long"))
  {
    if (restring_info.item == NULL)
    {
      send_to_char("No item to restring.\n\r", ch);
      return;
    }

    if (restring_info.owner != ch)
    {
      send_to_char("Restringer is being used by someone else right now.\n\r", ch);
      return;
    }

    if (argument[0] == '\0')
    {
      send_to_char("Restring long to what?\n\r", ch);
      return;
    }


    free_string(restring_info.item->description);       
    restring_info.item->description = str_dup(argument);
    send_to_char("Long description changed.\n\r", ch);
    return;       
  }

  if (restring_info.item != NULL)
  {
    send_to_char("Someone is already restringing something please wait.\n\r", ch);
    return;
  }

  if ((temp = get_obj_carry(ch, arg1, ch)) == NULL)
  {
    send_to_char("You aren't carrying that.\n\r", ch);
    return;
  }

  if (temp->item_type == ITEM_TOKEN)
  {
    send_to_char("You can't restring that.\n\r", ch);
    return;
  }  

  if ((coupon = get_obj_carry(ch, "restring", ch)) == NULL )
  {
    send_to_char("You don't have a coupon.\n\r", ch);
    return;
  }

  if (coupon->pIndexData->vnum != 6400)  
  {
    send_to_char("That's not a valid coupon.\n\r", ch);
    return;
  }

  obj_from_char(coupon);
  extract_obj(coupon);
  restring_info.item = temp;
  restring_info.owner = ch;
  obj_from_char(restring_info.item);
  colourstrip(tempshort, temp->short_descr); 
  sprintf(buf, "%s turned in %s for a restring.\n\r", ch->name, tempshort);
  logf(buf, 0);
  send_to_char("Ready to restring.\n\r", ch);
}

void do_add_extra(CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  char arg2[MIL];
  int i;
  bool found = FALSE;
  OBJ_DATA *obj;
 
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("syntax: add_extra <obj> <flag>\n\r", ch);
    return;
  }

  if ((obj = get_obj_carry(ch, arg1, ch)) == NULL)
  {
    send_to_char("You don't have that.\n\r", ch);
    return;
  }

  for (i = 0; extra_flags[i].name != NULL; i++)
  {
    if (!str_cmp(extra_flags[i].name, arg2))   
    {
      found = TRUE;
      break;
    }
  }

  if (!found)
  {
    send_to_char("No such extra flag found.\n\r", ch);
    return;
  }
 
  obj->extra_flags += extra_flags[i].bit;
  send_to_char("Extra flag added.\n\r", ch);

}  

void do_add_weapon_flag(CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  char arg2[MIL];
  int i;
  bool found = FALSE;
  OBJ_DATA *obj;
 
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("syntax: add_weapon_flag <obj> <flag>\n\r", ch);
    return;
  }

  if ((obj = get_obj_carry(ch, arg1, ch)) == NULL)
  {
    send_to_char("You don't have that.\n\r", ch);
    return;
  }

  for (i = 0; weapon_type2[i].name != NULL; i++)
  {
    if (!str_cmp(weapon_type2[i].name, arg2))   
    {
      found = TRUE;
      break;
    }
  }

  if (!found)
  {
    send_to_char("No such extra flag found.\n\r", ch);
    return;
  }
 
  obj->value[4] += weapon_type2[i].bit;
  send_to_char("Weapon flag added.\n\r", ch);

}  

void get_obj (CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * container, bool getAll)
{
    /* variables for AUTOSPLIT */
    CHAR_DATA *gch;
    int members;
    char buffer[100];
    char buf[MSL];

    if (!CAN_WEAR (obj, ITEM_TAKE))
    {
        send_to_char ("You can't take that.\n\r", ch);
        return;
    }

    if (ch->carry_number + get_obj_number (obj) > can_carry_n (ch))
    {
        act ("$d: you can't carry that many items.",
             ch, NULL, obj->name, TO_CHAR);
        return;
    }

    if (IS_OBJ_STAT(obj, ITEM_QUEST) && obj->owner && str_cmp(obj->owner, ch->name) &&
        str_cmp(obj->owner, "none"))
    {
      if (!IS_HIIMMORTAL(ch))
      {
        send_to_char("That quest retrieve is not yours.\n\r", ch);
        return;
      }
    }

    if (ch->clan == clan_lookup("Tinker") && obj->pk)
    {
      send_to_char("You start to pick it up then realize it has been involved in violence.  You are repulsed and drop it.\n\r", ch);
      return;
    }

    if ((!obj->in_obj || obj->in_obj->carried_by != ch)
        && (get_carry_weight (ch) + get_obj_weight (obj) > can_carry_w (ch)))
    {
        act ("$d: you can't carry that much weight.",
             ch, NULL, obj->name, TO_CHAR);
        return;
    }

    if (!can_loot (ch, obj))
    {
        act ("Corpse looting is not permitted.", ch, NULL, NULL, TO_CHAR);
        return;
    }

    if (obj->in_room != NULL)
    {
        for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room)
            if (gch->on == obj)
            {
                act ("$N appears to be using $p.", ch, obj, gch, TO_CHAR);
                return;
            }
    }

    if (obj->item_type == ITEM_CORPSE_PC)
    {
      if (str_cmp(ch->name, obj->owner) && !IS_IMMORTAL(ch))
      {
	 send_to_char("You can't do that.\n\r", ch);
         return;
      }
    }    

    if (container != NULL)
    {
        if (container->pIndexData->vnum == OBJ_VNUM_PIT
            && get_trust (ch) < obj->level)
        {
            send_to_char ("You are not powerful enough to use it.\n\r", ch);
            return;
        }

        if (container->item_type == ITEM_CORPSE_PC && str_cmp(container->owner, ch->name))
        {
           if (container->lootsLeft > 0)
           {
             char logbuf[MSL];
             container->lootsLeft -= 1;
             sprintf(buf, "That's one loot %d left.\n\r", container->lootsLeft);
             sprintf(logbuf, "%s looted %s - ID %ld - from %s's corpse.", 
			ch->name, obj->short_descr, obj->id, container->owner);
             log_special(logbuf, PK_TYPE);
             send_to_char(buf, ch);    
             obj->pk = TRUE;
           }
           else if (!IS_IMMORTAL(ch))
           {
             sprintf(buf, "That's it no more loots.\n\r");
             send_to_char(buf, ch);
             return;
           }
        }

        if (container->pIndexData->vnum == OBJ_VNUM_PIT
            && !CAN_WEAR (container, ITEM_TAKE)
            && !IS_OBJ_STAT (obj, ITEM_HAD_TIMER))
            obj->timer = 0;
        act ("You get $p from $P.", ch, obj, container, TO_CHAR);
        if (!getAll)
          act ("$n gets $p from $P.", ch, obj, container, TO_ROOM);

        REMOVE_BIT (obj->extra_flags, ITEM_HAD_TIMER);
        
        obj_from_obj (obj);
        
 
   }
    else
    {
        act ("You get $p.", ch, obj, container, TO_CHAR);
        if (!getAll)
          act ("$n gets $p.", ch, obj, container, TO_ROOM);
        obj_from_room (obj);
    }

    if (obj->item_type == ITEM_MONEY)
    {
        ch->silver += obj->value[0];
        ch->gold += obj->value[1];
        if (IS_SET (ch->act, PLR_AUTOSPLIT))
        {                        /* AUTOSPLIT code */
            members = 0;
            for (gch = ch->in_room->people; gch != NULL;
                 gch = gch->next_in_room)
            {
                if (!IS_AFFECTED (gch, AFF_CHARM) && is_same_group (gch, ch))
                    members++;
            }

            if (members > 1 && (obj->value[0] > 1 || obj->value[1]))
            {
                sprintf (buffer, "%d %d", obj->value[0], obj->value[1]);
                do_function (ch, &do_split, buffer);
            }
        }

        extract_obj (obj);
    }
    else
    {
        obj_to_char (obj, ch);
        if ( HAS_TRIGGER_OBJ( obj, TRIG_GET ) )
            p_give_trigger( NULL, obj, NULL, ch, obj, TRIG_GET );
        if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_GET ) )
            p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_GET );

	if (obj->item_type == ITEM_BOMB)
	{
          if ((obj->value[0] == TRG_GET && obj->value[2] == STATE_SET) ||
	       (obj->value[2] == STATE_UNSTABLE && number_bits(3) == 0))
          {
            (*explosive_table[obj->value[1]].bomb_fun) (obj, ch);
            extract_obj(obj);
	  }
	}
    }

    return;
}



void do_get (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
    bool found;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (!str_cmp (arg2, "from"))
        argument = one_argument (argument, arg2);

    /* Get type. */
    if (arg1[0] == '\0')
    {
        send_to_char ("Get what?\n\r", ch);
        return;
    }

    if (arg2[0] == '\0')
    {
        if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
        {
            /* 'get obj' */
            obj = get_obj_list (ch, arg1, ch->in_room->contents);
            if (obj == NULL)
            {
                act ("I see no $T here.", ch, NULL, arg1, TO_CHAR);
                return;
            }

            get_obj (ch, obj, NULL, FALSE);
        }
        else
        {
            /* 'get all' or 'get all.obj' */
            found = FALSE;
            for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
            {
                obj_next = obj->next_content;
                if ((arg1[3] == '\0' || is_name (&arg1[4], obj->name))
                    && can_see_obj (ch, obj))
                {
                    found = TRUE;
                    get_obj (ch, obj, NULL, TRUE);
                }
            }
             

            if (!found)
            {
                if (arg1[3] == '\0')
                    send_to_char ("I see nothing here.\n\r", ch);
                else
                    act ("I see no $T here.", ch, NULL, &arg1[4], TO_CHAR);
            }
            else
	    {
	      act("$n grabs several items off the ground.\n\r", ch, NULL, NULL, TO_ROOM);
            }
        }
    }
    else
    {
        /* 'get ... container' */
        if (!str_cmp (arg2, "all") || !str_prefix ("all.", arg2))
        {
            send_to_char ("You can't do that.\n\r", ch);
            return;
        }

        if ((container = get_obj_here (ch, NULL, arg2)) == NULL)
        {
            act ("I see no $T here.", ch, NULL, arg2, TO_CHAR);
            return;
        }
    if (container->item_type == ITEM_QUIVER)
    {
    	int i;
    	if (str_cmp(arg1,"all") && str_prefix("all.",arg1))
    	{
    	    if (container->value[0]==0)
    	    {
    	    	send_to_char ("You don't have any arrows in that quiver.\n\r",ch);
    	    	return;
    	    }
    	        obj = create_object(get_obj_index(container->value[3]),0);
    	    	if (obj->item_type != ITEM_ARROW)
    	    	{
    	    	    send_to_char("Your quiver is bugged. Please post a note to imms about your situation.",ch);
    	    	    return;
    	    	}
    	    	container->value[0] -= 1;
    	        act ("$n gets $p from $P.",ch,obj,container,TO_ROOM);
    	        act ("You gets $p from $P.",ch,obj,container,TO_CHAR);
    	        obj_to_char(obj,ch);    	       	
    	}
    	else
    	{
	    bool arrowDone = FALSE;
    	    if (container->value[0]==0)
    	    {
    	    	send_to_char ("You don't have any arrows in that quiver.\n\r",ch);
    	    	return;
    	    }

    	    for (i = container->value[0]; container->value[0]!=0;)
    	    {
    	    	obj = create_object(get_obj_index(container->value[3]),0);
    	    	if (obj->item_type != ITEM_ARROW)
    	    	{
    	    	    send_to_char("Your quiver is bugged. Please post a note to imms about your situation.",ch);
    	    	    return;
    	    	}
		
		if (!arrowDone)
		{
    	          act ("$n gets several of $p from $P.",ch, obj, container,TO_ROOM);
    	          act ("You get all your $p from $P.",ch, obj, container,TO_CHAR);
    	    	  arrowDone = TRUE;
		}	
    	    	container->value[0] -= 1;
    	        obj_to_char(obj,ch);   	    	
    	    }
    	
    	}
    }
    else
    {
        switch (container->item_type)
        {
            default:
                send_to_char ("That's not a container.\n\r", ch);
                return;
            case ITEM_ENVELOPE:
            case ITEM_CONTAINER:
            case ITEM_KEYRING:
            case ITEM_CORPSE_NPC:
                break;

            case ITEM_CORPSE_PC:
                {

                    /* if (IS_SET(obj->extra_flags, ITEM_TAINT))
                    {
                    	send_to_char ("That peice of eq has begun to rot, dont touch it!\n\r", ch);
                    	SET_BIT(obj->extra_flags, ITEM_ROT_DEATH);
                    	return;
                    } */
                    if (!can_loot (ch, container))
                    {
                        send_to_char ("You can't do that.\n\r", ch);
                        return;
                    }
                }
        }

        if (IS_SET (container->value[1], CONT_CLOSED))
        {
            act ("The $d is closed.", ch, NULL, container->name, TO_CHAR);
            return;
        }

        if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
        {
            /* 'get obj container' */
            obj = get_obj_list (ch, arg1, container->contains);
            if (obj == NULL)
            {
            	if (container->item_type == ITEM_KEYRING)
            	{
            	act ("I see nothing like that on the $T.",
                     ch, NULL, arg2, TO_CHAR);
                }
                else
                {    
                act ("I see nothing like that in the $T.",
                     ch, NULL, arg2, TO_CHAR);
                }
                return;
            }
            get_obj (ch, obj, container, FALSE);
        }
        else
        {
            /* 'get all container' or 'get all.obj container' */
            found = FALSE;
            for (obj = container->contains; obj != NULL; obj = obj_next)
            {
                obj_next = obj->next_content;
                if ((arg1[3] == '\0' || is_name (&arg1[4], obj->name))
                    && can_see_obj (ch, obj))
                {
                    found = TRUE;
                    if (container->pIndexData->vnum == OBJ_VNUM_PIT
                        && !IS_IMMORTAL (ch))
                    {
                        send_to_char ("Don't be so greedy!\n\r", ch);
                        return;
                    }
                    get_obj (ch, obj, container, TRUE);
                }
            }

            if (!found)
            {
            	if (container->item_type == ITEM_KEYRING)
            	   act ("I see nothing on the $T.", ch, NULL, arg2, TO_CHAR);
                else if (arg1[3] == '\0')
                    act ("I see nothing in the $T.", ch, NULL, arg2, TO_CHAR);
                else
                    act ("I see nothing like that in the $T.",
                         ch, NULL, arg2, TO_CHAR);
            }
	    else
	    {
	      act("$n gets several items from $p", ch, container, NULL, TO_ROOM);
	    }

        }
    }
    }
    return;
}



void do_put (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (!str_cmp (arg2, "in") || !str_cmp (arg2, "on"))
        argument = one_argument (argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char ("Put what in what?\n\r", ch);
        return;
    }

    if (!str_cmp (arg2, "all") || !str_prefix ("all.", arg2))
    {
        send_to_char ("You can't do that.\n\r", ch);
        return;
    }

    if ((container = get_obj_here (ch, NULL, arg2)) == NULL)
    {
        act ("I see no $T here.", ch, NULL, arg2, TO_CHAR);
        return;
    }
    
    if (container->item_type == ITEM_KEYRING)
    {
      if (str_cmp(arg1, "all") && str_prefix("all.", arg1))
      {
      	if ((obj= get_obj_carry (ch,arg1,ch))==NULL)
        {
    	  send_to_char ("You don't see that.\n\r",ch);
    	  return;
        }
        if ((obj->item_type)!= ITEM_KEY)
        {
    	  send_to_char ("That's not a key.\n\r",ch);
    	  return;
    	}
    	
    	if (!can_drop_obj(ch,obj))
    	{
    	  send_to_char ("You can't let go of it!\n\r",ch);
    	  return;
    	}
    	
    	if (get_obj_number(container) + 1 > container->value[0] + 1)
    	{
    	  send_to_char("It can't hold any more rings.\n\r", ch);
    	  return;
    	}
    	
    	obj_from_char (obj);
        obj_to_obj (obj, container);
        act ("$n puts $p on $P.", ch, obj, container, TO_ROOM);
        act ("You put $p on $P.", ch, obj, container, TO_CHAR);
      }
      else
      {
         for (obj = ch->carrying; obj != NULL; obj = obj_next)
         {
            obj_next = obj->next_content;

            if ((arg1[3] == '\0' || is_name (&arg1[4], obj->name))
                && can_see_obj (ch, obj)
                && obj->wear_loc == WEAR_NONE
                && obj->item_type == ITEM_KEY)
            {
                if (get_obj_number(container) + 1 > container->value[0] + 1)
    		{
    	  	  send_to_char("It can't hold any more rings.\n\r", ch);
    	  	  return;
    		}
                obj_from_char (obj);
                obj_to_obj (obj, container);
            }
         }
       }
       return;
    }
    	
    		
    if (container->item_type == ITEM_QUIVER)
    {

    	if (str_cmp(arg1,"all") && str_prefix("all.",arg1))
    	{
    	    if ((obj= get_obj_carry (ch,arg1,ch))==NULL)
    	    {
    	    	send_to_char ("You don't have any arrows.\n\r",ch);
    	    	return;
    	    }
    	    if ((obj->item_type)!=ITEM_ARROW)
    	    {
    	    	send_to_char ("You don't have any arrows.\n\r",ch);
    	    	return;
    	    }
    	    if (!can_drop_obj(ch,obj))
    	    {
    	    	send_to_char ("You can't let go of it!\n\r",ch);
    	    	return;
    	    }
    	    if (container->value[1] != -1
    	        && (container->value[1] <= container->value[0]))
    	    {
    	    	send_to_char("You don't have enough room in there for anymore ammunition.\n\r",ch);
    	    	return;
    	    }
    	        
    	    if (obj->pIndexData->vnum== container->value[3])
    	    {
    	    	container->value[0] += 1;
    	        act ("$n puts $p in $P.",ch,obj,container,TO_ROOM);
    	        act ("You put $p in $P.",ch,obj,container,TO_CHAR);
    	        extract_obj(obj);
    	    }
    	    else if (container->value[0]==0)
    	    {
    	        container->value[0] += 1;
    	        container->value[2] = obj->value[2];
    	        container->value[3] = obj->pIndexData->vnum;
    	        act ("$n puts $p in $P.",ch,obj,container,TO_ROOM);
    	        act ("You put $p in $P.",ch,obj,container,TO_CHAR);
    	        extract_obj(obj);
    	    }
    	    else
    	    {
    	    	send_to_char ("The arrows in the quiver are not the same as those you wish to place in.\n\r",ch);
    	    	return;
    	    }   	
    	}
    	else
    	{
            bool arrowDone = FALSE;
    	    for (obj = ch->carrying; obj != NULL; obj=obj_next)
    	    {
    	    	obj_next = obj->next_content;
    	    	if ((obj->item_type == ITEM_ARROW)
    	    	&& (obj->pIndexData->vnum==(container->value[3])))
    	    	{
    	    	    if (container->value[1] != -1
    	               && (container->value[1] <= container->value[0]))
    	            {
    	    	        send_to_char("You don't have enough room in there for anymore ammunition.\n\r",ch);
    	    	        return;
    	            }
    	    	    container->value[0] += 1;
    	            container->value[2] = obj->value[2];
                    if (!arrowDone)
                    {
    	              act ("$n puts several of $p in $P.",ch,obj,container,TO_ROOM);
    	              act ("You put all of your $p in $P.",ch,obj,container,TO_CHAR);
		      arrowDone = TRUE;
		    }
    	            extract_obj(obj);
    	        }    	    	
    	    }
    	
    	}
    }
    else
    {
    if (container->item_type != ITEM_CONTAINER && container->item_type != ITEM_ENVELOPE)
    {
        send_to_char ("That's not a container.\n\r", ch);
        return;
    }

    if (IS_SET (container->value[1], CONT_CLOSED))
    {
        act ("The $d is closed.", ch, NULL, container->name, TO_CHAR);
        return;
    }

    if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
    {
        /* 'put obj container' */
        if ((obj = get_obj_carry (ch, arg1, ch)) == NULL)
        {
            send_to_char ("You do not have that item.\n\r", ch);
            return;
        }

        if (obj == container)
        {
            send_to_char ("You can't fold it into itself.\n\r", ch);
            return;
        }

        if (!can_drop_obj (ch, obj))
        {
            send_to_char ("You can't let go of it.\n\r", ch);
            return;
        }

        if (WEIGHT_MULT (obj) != 100)
        {
            send_to_char ("You have a feeling that would be a bad idea.\n\r",
                          ch);
            return;
        }

        if (get_obj_weight (obj) + get_true_weight (container)
            > (container->value[0] * 10)
            || get_obj_weight (obj) > (container->value[3] * 10))
        {
            send_to_char ("It won't fit.\n\r", ch);
            return;
        }

        if (container->pIndexData->vnum == OBJ_VNUM_PIT
            && !CAN_WEAR (container, ITEM_TAKE))
        {
            if (obj->timer)
                SET_BIT (obj->extra_flags, ITEM_HAD_TIMER);
            else
                obj->timer = number_range (100, 200);
        }

        obj_from_char (obj);
        obj_to_obj (obj, container);

        if (IS_SET (container->value[1], CONT_PUT_ON))
        {
            act ("$n puts $p on $P.", ch, obj, container, TO_ROOM);
            act ("You put $p on $P.", ch, obj, container, TO_CHAR);
        }
        else
        {
            act ("$n puts $p in $P.", ch, obj, container, TO_ROOM);
            act ("You put $p in $P.", ch, obj, container, TO_CHAR);
        }
    }
    else
    {
        /* 'put all container' or 'put all.obj container' */
        for (obj = ch->carrying; obj != NULL; obj = obj_next)
        {
            obj_next = obj->next_content;

            if ((arg1[3] == '\0' || is_name (&arg1[4], obj->name))
                && can_see_obj (ch, obj)
                && WEIGHT_MULT (obj) == 100
                && obj->wear_loc == WEAR_NONE
                && obj != container && can_drop_obj (ch, obj)
                && get_obj_weight (obj) + get_true_weight (container)
                <= (container->value[0] * 10)
                && get_obj_weight (obj) < (container->value[3] * 10))
            {
                if (container->pIndexData->vnum == OBJ_VNUM_PIT
                    && !CAN_WEAR (obj, ITEM_TAKE))
                {
                    if (obj->timer)
                        SET_BIT (obj->extra_flags, ITEM_HAD_TIMER);
                    else
                        obj->timer = number_range (100, 200);
                }

                obj_from_char (obj);
                obj_to_obj (obj, container);

                if (IS_SET (container->value[1], CONT_PUT_ON))
                {
                    act ("$n puts $p on $P.", ch, obj, container, TO_ROOM);
                    act ("You put $p on $P.", ch, obj, container, TO_CHAR);
                }
                else
                {
                    act ("$n puts $p in $P.", ch, obj, container, TO_ROOM);
                    act ("You put $p in $P.", ch, obj, container, TO_CHAR);
                }
            }
        }
    }
    }
    return;
}



void do_drop (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;

    argument = one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Drop what?\n\r", ch);
        return;
    }

    if (!IS_NPC(ch) && (IS_MURDERER(ch)||IS_THIEF(ch)))
    {
        send_to_char("You are on the run. There is no time for that!\n\r",ch);
        return;
    }
    if (is_number (arg))
    {
        /* 'drop NNNN coins' */
        int amount, gold = 0, silver = 0;

        if (!IS_NPC(ch) && ch->pcdata->pk_timer > 0)
        {
            send_to_char("Wait till the battle has settled before dropping anything.\n\r", ch);
            return;
        }
        
        


        amount = atoi (arg);
        argument = one_argument (argument, arg);
        if (amount <= 0
            || (str_cmp (arg, "coins") && str_cmp (arg, "coin") &&
                str_cmp (arg, "gold") && str_cmp (arg, "silver")))
        {
            send_to_char ("Sorry, you can't do that.\n\r", ch);
            return;
        }

        if (!str_cmp (arg, "coins") || !str_cmp (arg, "coin")
            || !str_cmp (arg, "silver"))
        {
            if (ch->silver < amount)
            {
                send_to_char ("You don't have that much silver.\n\r", ch);
                return;
            }

            ch->silver -= amount;
            silver = amount;
        }

        else
        {
            if (ch->gold < amount)
            {
                send_to_char ("You don't have that much gold.\n\r", ch);
                return;
            }

            ch->gold -= amount;
            gold = amount;
        }

        for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
        {
            obj_next = obj->next_content;

            switch (obj->pIndexData->vnum)
            {
                case OBJ_VNUM_SILVER_ONE:
                    silver += 1;
                    extract_obj (obj);
                    break;

                case OBJ_VNUM_GOLD_ONE:
                    gold += 1;
                    extract_obj (obj);
                    break;

                case OBJ_VNUM_SILVER_SOME:
                    silver += obj->value[0];
                    extract_obj (obj);
                    break;

                case OBJ_VNUM_GOLD_SOME:
                    gold += obj->value[1];
                    extract_obj (obj);
                    break;

                case OBJ_VNUM_COINS:
                    silver += obj->value[0];
                    gold += obj->value[1];
                    extract_obj (obj);
                    break;
            }
        }

        obj_to_room (create_money (gold, silver), ch->in_room);
        act ("$n drops some coins.", ch, NULL, NULL, TO_ROOM);
        send_to_char ("OK.\n\r", ch);
        return;
    }

    if (str_cmp (arg, "all") && str_prefix ("all.", arg))
    {
        /* 'drop obj' */
        if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
        {
            send_to_char ("You do not have that item.\n\r", ch);
            return;
        }

        if (!IS_NPC(ch) && ch->pcdata->pk_timer > 0 &&
		(obj->item_type != ITEM_BOMB &&
                 obj->item_type != ITEM_PORTAL))
        {
          send_to_char("Wait till the battle has settled before dropping anything.\n\r", ch);
          return;
        }


        if (!can_drop_obj (ch, obj))
        {
            send_to_char ("You can't let go of it.\n\r", ch);
            return;
        }

        if (obj->item_type == ITEM_BOMB)
        {
          OBJ_DATA *bomb, *bomb_next;
          for (bomb = ch->in_room->contents; bomb != NULL; bomb = bomb_next)
          {
            bomb_next = bomb->next_content;
            
            if (bomb->item_type == ITEM_BOMB)
            {
              send_to_char("There's already a bomb here.\n\r", ch);
              return;
            }
          }

          if ((obj->value[0] == TRG_ENTER_ROOM )
             && (obj->value[2] == STATE_SET))
          {
             ch->in_room->entry_bomb = TRUE;
          }
        } 
   
        obj_from_char (obj);
        obj_to_room (obj, ch->in_room);
        act ("$n drops $p.", ch, obj, NULL, TO_ROOM);
        act ("You drop $p.", ch, obj, NULL, TO_CHAR);
        if ( HAS_TRIGGER_OBJ( obj, TRIG_DROP ) )
            p_give_trigger( NULL, obj, NULL, ch, obj, TRIG_DROP );
        if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_DROP ) )
            p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_DROP );

        if (IS_OBJ_STAT (obj, ITEM_MELT_DROP))
        {
            act ("$p dissolves into smoke.", ch, obj, NULL, TO_ROOM);
            act ("$p dissolves into smoke.", ch, obj, NULL, TO_CHAR);
            extract_obj (obj);
        }
    }
    else
    {
        /* 'drop all' or 'drop all.obj' */
        found = FALSE;

        if (!IS_NPC(ch) && ch->pcdata->pk_timer > 0)
        {
          send_to_char("Wait till the battle has settled before dropping anything.\n\r", ch);
          return;
        }

        for (obj = ch->carrying; obj != NULL; obj = obj_next)
        {
            obj_next = obj->next_content;

            if ((arg[3] == '\0' || is_name (&arg[4], obj->name))
                && can_see_obj (ch, obj)
                && obj->wear_loc == WEAR_NONE && can_drop_obj (ch, obj))
            {

             if (obj->item_type == ITEM_BOMB)
             {
               OBJ_DATA *bomb, *bomb_next;
               for (bomb = ch->in_room->contents; bomb != NULL; bomb = bomb_next)
               {
                  bomb_next = bomb->next_content;
            
                  if (bomb->item_type == ITEM_BOMB)
                  {
                    send_to_char("There's already a bomb here.\n\r", ch);
                    return;
                  }
                }

                  if (obj->value[0] == TRG_GET && obj->value[2] == STATE_SET)
                  {
                    ch->in_room->entry_bomb = TRUE;
                  }
                } 

                found = TRUE;
                obj_from_char (obj);
                obj_to_room (obj, ch->in_room);
                act ("You drop $p.", ch, obj, NULL, TO_CHAR);
	        if ( HAS_TRIGGER_OBJ( obj, TRIG_DROP ) )
        	    p_give_trigger( NULL, obj, NULL, ch, obj, TRIG_DROP );
	        if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_DROP ) )
        	    p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_DROP );

                if (IS_OBJ_STAT (obj, ITEM_MELT_DROP))
                {
                    act ("$p dissolves into smoke.", ch, obj, NULL, TO_ROOM);
                    act ("$p dissolves into smoke.", ch, obj, NULL, TO_CHAR);
                    extract_obj (obj);
                }
            }
        }

        if (!found)
        {
            if (arg[3] == '\0')
                act ("You are not carrying anything.",
                     ch, NULL, arg, TO_CHAR);
            else
                act ("You are not carrying any $T.",
                     ch, NULL, &arg[4], TO_CHAR);
        }
	else
	{
          act ("$n drops several items onto the ground.", ch, obj, NULL, TO_ROOM);
	}
    }

    return;
}



void do_give (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char ("Give what to whom?\n\r", ch);
        return;
    }

    if (!IS_NPC(ch) && ch->pcdata->pk_timer > 0)
    {
      send_to_char("Wait till the battle has settled down to be giving.\n\r", ch);
      return;
    }	
    if (!IS_NPC(ch) && (IS_MURDERER(ch)||IS_THIEF(ch)))
        {
            send_to_char("You are on the run. There is no time for that!\n\r",ch);
            return;
        }	
    
    if (is_number (arg1))
    {
        /* 'give NNNN coins victim' */
        int amount;
        bool silver;

        amount = atoi (arg1);
        if (amount <= 0
            || (str_cmp (arg2, "coins") && str_cmp (arg2, "coin") &&
                str_cmp (arg2, "gold") && str_cmp (arg2, "silver")))
        {
            send_to_char ("Sorry, you can't do that.\n\r", ch);
            return;
        }

        silver = str_cmp (arg2, "gold");

        argument = one_argument (argument, arg2);
        if (arg2[0] == '\0')
        {
            send_to_char ("Give what to whom?\n\r", ch);
            return;
        }

        if ((victim = get_char_room (ch, NULL, arg2)) == NULL)
        {
            send_to_char ("They aren't here.\n\r", ch);
            return;
        }

        if ((!silver && ch->gold < amount) || (silver && ch->silver < amount))
        {
            send_to_char ("You haven't got that much.\n\r", ch);
            return;
        }

        if (silver)
        {
            ch->silver -= amount;
            victim->silver += amount;
        }
        else
        {
            ch->gold -= amount;
            victim->gold += amount;
        }

        sprintf (buf, "$n gives you %d %s.", amount,
                 silver ? "silver" : "gold");
        act (buf, ch, NULL, victim, TO_VICT);
        act ("$n gives $N some coins.", ch, NULL, victim, TO_NOTVICT);
        sprintf (buf, "You give $N %d %s.", amount,
                 silver ? "silver" : "gold");
        act (buf, ch, NULL, victim, TO_CHAR);

        /*
         * Bribe trigger
         */
        if (IS_NPC (victim) && HAS_TRIGGER_MOB (victim, TRIG_BRIBE))
            p_bribe_trigger (victim, ch, silver ? amount : amount * 100);

        if (IS_NPC (victim) && IS_SET (victim->act, ACT_IS_CHANGER))
        {
            int change;

            change = (silver ? 95 * amount / 100 / 100 : 95 * amount);


            if (!silver && change > victim->silver)
                victim->silver += change;

            if (silver && change > victim->gold)
                victim->gold += change;

            if (change < 1 && can_see (victim, ch))
            {
                act
                    ("$n tells you 'I'm sorry, you did not give me enough to change.'",
                     victim, NULL, ch, TO_VICT);
                ch->oreply = victim;
                sprintf (buf, "%d %s %s",
                         amount, silver ? "silver" : "gold", ch->name);
                do_function (victim, &do_give, buf);
            }
            else if (can_see (victim, ch))
            {
                sprintf (buf, "%d %s %s",
                         change, silver ? "gold" : "silver", ch->name);
                do_function (victim, &do_give, buf);
                if (silver)
                {
                    sprintf (buf, "%d silver %s",
                             (95 * amount / 100 - change * 100), ch->name);
                    do_function (victim, &do_give, buf);
                }
                act ("$n tells you 'Thank you, come again.'",
                     victim, NULL, ch, TO_VICT);
                ch->oreply = victim;
            }
        }
        return;
    }

    if ((obj = get_obj_carry (ch, arg1, ch)) == NULL)
    {
        send_to_char ("You do not have that item.\n\r", ch);
        return;
    }

    if (obj->wear_loc != WEAR_NONE)
    {
        send_to_char ("You must remove it first.\n\r", ch);
        return;
    }

    if (obj->item_type == ITEM_TRASH && ch->pcdata->pk_timer > 0)
    {
	send_to_char("Why would you want to give trash in the heat of battle?\n\r", ch);
	return;
    }

    if ((victim = get_char_room (ch, NULL, arg2)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (IS_NPC (victim) && victim->pIndexData->pShop != NULL)
    {
        act ("$N tells you 'Sorry, you'll have to sell that.'",
             ch, NULL, victim, TO_CHAR);
        ch->oreply = victim;
        return;
    }

    if (!can_drop_obj (ch, obj))
    {
        send_to_char ("You can't let go of it.\n\r", ch);
        return;
    }

    if (obj->item_type == ITEM_BOMB && obj->value[2] == STATE_SET)
    {
      if (victim->cClass != class_lookup("Illuminator"))
      {
        send_to_char("You wouldn't want to pass them a set bomb, they wouldn't know how to handle it.\n\r", ch);
        return;   
      }
    }

    if (victim->clan == clan_lookup("Tinker") && obj->pk)
    {
      send_to_char("That item has been involved in pk, they wouldn't think of touching it.\n\r", ch);
      return;
    }

    if (victim->carry_number + get_obj_number (obj) > can_carry_n (victim))
    {
        act ("$N has $S hands full.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (get_carry_weight (victim) + get_obj_weight (obj) >
        can_carry_w (victim))
    {
        act ("$N can't carry that much weight.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (!can_see_obj (victim, obj))
    {
        act ("$N can't see it.", ch, NULL, victim, TO_CHAR);
        return;
    }

    obj_from_char (obj);
    obj_to_char (obj, victim);
    MOBtrigger = FALSE;
    act ("$n gives $p to $N.", ch, obj, victim, TO_NOTVICT);
    act ("$n gives you $p.", ch, obj, victim, TO_VICT);
    act ("You give $p to $N.", ch, obj, victim, TO_CHAR);
    MOBtrigger = TRUE;
    if ( HAS_TRIGGER_OBJ( obj, TRIG_GIVE ) )
        p_give_trigger( NULL, obj, NULL, ch, obj, TRIG_GIVE );
    if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_GIVE ) )
        p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_GIVE );

    /*
     * Give trigger
     */
    if (IS_NPC (victim) && HAS_TRIGGER_MOB (victim, TRIG_GIVE))
        p_give_trigger (victim, NULL, NULL, ch, obj, TRIG_GIVE);

    return;
}


/* for poisoning weapons and food/drink */
void do_envenom(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    AFFECT_DATA af;
    int percent,skill;

    /* find out what */
    if (argument[0] == '\0')
    {
	send_to_char("Envenom what item?\n\r",ch);
	return;
    }

    obj =  get_obj_list(ch,argument,ch->carrying);

    if (obj== NULL)
    {
	send_to_char("You don't have that item.\n\r",ch);
	return;
    }

    if ((skill = get_skill(ch,gsn_envenom)) < 1)
    {
	send_to_char("Are you crazy? You'd poison yourself!\n\r",ch);
	return;
    }

    if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
    {
	if (IS_OBJ_STAT(obj,ITEM_BLESS) || IS_OBJ_STAT(obj,ITEM_BURN_PROOF))
	{
	    act("You fail to poison $p.",ch,obj,NULL,TO_CHAR);
	    return;
	}

	if (number_percent() < skill)  /* success! */
	{
	    act("$n treats $p with deadly poison.",ch,obj,NULL,TO_ROOM);
	    act("You treat $p with deadly poison.",ch,obj,NULL,TO_CHAR);
	    if (!obj->value[3])
	    {
		obj->value[3] = 1;
		check_improve(ch,gsn_envenom,TRUE,4);
	    }
	    WAIT_STATE(ch,skill_table[gsn_envenom].beats);
	    return;
	}

	act("You fail to poison $p.",ch,obj,NULL,TO_CHAR);
	if (!obj->value[3])
	    check_improve(ch,gsn_envenom,FALSE,4);
	WAIT_STATE(ch,skill_table[gsn_envenom].beats);
	return;
     }

    if (obj->item_type == ITEM_WEAPON)
    {
  /*      if (IS_WEAPON_STAT(obj,WEAPON_FLAMING)
        ||  IS_WEAPON_STAT(obj,WEAPON_FROST)
        ||  IS_WEAPON_STAT(obj,WEAPON_VAMPIRIC)
        ||  IS_WEAPON_STAT(obj,WEAPON_SHARP)
        ||  IS_WEAPON_STAT(obj,WEAPON_VORPAL)
        ||  IS_WEAPON_STAT(obj,WEAPON_SHOCKING)
        ||  IS_OBJ_STAT(obj,ITEM_BLESS) || IS_OBJ_STAT(obj,ITEM_BURN_PROOF))
        {
            act("You can't seem to envenom $p.",ch,obj,NULL,TO_CHAR);
            return;
        }
*/
	if (obj->value[3] < 0 
	||  attack_table[obj->value[3]].damage == DAM_BASH)
	{
	    send_to_char("You can only envenom edged weapons.\n\r",ch);
	    return;
	}

        if (IS_WEAPON_STAT(obj,WEAPON_POISON))
        {
            act("$p is already envenomed.",ch,obj,NULL,TO_CHAR);
            return;
        }

	percent = number_percent();
	if (percent < skill)
	{
 
            af.where     = TO_WEAPON;
            af.type      = gsn_poison;
            af.level     = ch->level * percent / 100;
            af.duration  = ch->level * percent / 100;
            af.location  = 0;
            af.modifier  = 0;
            af.bitvector = WEAPON_POISON;
            affect_to_obj(obj,&af);
 
            act("$n coats $p with deadly venom.",ch,obj,NULL,TO_ROOM);
	    act("You coat $p with venom.",ch,obj,NULL,TO_CHAR);
	    check_improve(ch,gsn_envenom,TRUE,3);
	    WAIT_STATE(ch,skill_table[gsn_envenom].beats);
            return;
        }
	else
	{
	    act("You fail to envenom $p.",ch,obj,NULL,TO_CHAR);
	    check_improve(ch,gsn_envenom,FALSE,3);
	    WAIT_STATE(ch,skill_table[gsn_envenom].beats);
	    return;
	}
    }
 
    act("You can't poison $p.",ch,obj,NULL,TO_CHAR);
    return;
}

void do_fill (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *fountain;
    bool found;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Fill what?\n\r", ch);
        return;
    }

    if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
    {
        send_to_char ("You do not have that item.\n\r", ch);
        return;
    }

    found = FALSE;
    for (fountain = ch->in_room->contents; fountain != NULL;
         fountain = fountain->next_content)
    {
        if (fountain->item_type == ITEM_FOUNTAIN)
        {
            found = TRUE;
            break;
        }
    }

    if (!found)
    {
        send_to_char ("There is no fountain here!\n\r", ch);
        return;
    }

    if (obj->item_type != ITEM_DRINK_CON)
    {
        send_to_char ("You can't fill that.\n\r", ch);
        return;
    }

    if (obj->value[1] != 0 && obj->value[2] != fountain->value[2])
    {
        send_to_char ("There is already another liquid in it.\n\r", ch);
        return;
    }

    if (obj->value[1] >= obj->value[0])
    {
        send_to_char ("Your container is full.\n\r", ch);
        return;
    }

    sprintf (buf, "You fill $p with %s from $P.",
             liq_table[fountain->value[2]].liq_name);
    act (buf, ch, obj, fountain, TO_CHAR);
    sprintf (buf, "$n fills $p with %s from $P.",
             liq_table[fountain->value[2]].liq_name);
    act (buf, ch, obj, fountain, TO_ROOM);
    obj->value[2] = fountain->value[2];
    obj->value[1] = obj->value[0];
    return;
}

void do_pour (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
    OBJ_DATA *out, *in;
    CHAR_DATA *vch = NULL;
    int amount;

    argument = one_argument (argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0')
    {
        send_to_char ("Pour what into what?\n\r", ch);
        return;
    }


    if ((out = get_obj_carry (ch, arg, ch)) == NULL)
    {
        send_to_char ("You don't have that item.\n\r", ch);
        return;
    }

    if (out->item_type != ITEM_DRINK_CON)
    {
        send_to_char ("That's not a drink container.\n\r", ch);
        return;
    }

    if (!str_cmp (argument, "out"))
    {
        if (out->value[1] == 0)
        {
            send_to_char ("It's already empty.\n\r", ch);
            return;
        }

        out->value[1] = 0;
        out->value[3] = 0;
        sprintf (buf, "You invert $p, spilling %s all over the ground.",
                 liq_table[out->value[2]].liq_name);
        act (buf, ch, out, NULL, TO_CHAR);

        sprintf (buf, "$n inverts $p, spilling %s all over the ground.",
                 liq_table[out->value[2]].liq_name);
        act (buf, ch, out, NULL, TO_ROOM);
        return;
    }

    if ((in = get_obj_here (ch, NULL, argument)) == NULL)
    {
        vch = get_char_room (ch, NULL, argument);

        if (vch == NULL)
        {
            send_to_char ("Pour into what?\n\r", ch);
            return;
        }

        in = get_eq_char (vch, WEAR_HOLD);

        if (in == NULL)
        {
            send_to_char ("They aren't holding anything.", ch);
            return;
        }
    }

    if (in->item_type != ITEM_DRINK_CON)
    {
        send_to_char ("You can only pour into other drink containers.\n\r",
                      ch);
        return;
    }

    if (in == out)
    {
        send_to_char ("You cannot change the laws of physics!\n\r", ch);
        return;
    }

    if (in->value[1] != 0 && in->value[2] != out->value[2])
    {
        send_to_char ("They don't hold the same liquid.\n\r", ch);
        return;
    }

    if (out->value[1] == 0)
    {
        act ("There's nothing in $p to pour.", ch, out, NULL, TO_CHAR);
        return;
    }

    if (in->value[1] >= in->value[0])
    {
        act ("$p is already filled to the top.", ch, in, NULL, TO_CHAR);
        return;
    }

    amount = UMIN (out->value[1], in->value[0] - in->value[1]);

    in->value[1] += amount;
    out->value[1] -= amount;
    in->value[2] = out->value[2];

    if (vch == NULL)
    {
        sprintf (buf, "You pour %s from $p into $P.",
                 liq_table[out->value[2]].liq_name);
        act (buf, ch, out, in, TO_CHAR);
        sprintf (buf, "$n pours %s from $p into $P.",
                 liq_table[out->value[2]].liq_name);
        act (buf, ch, out, in, TO_ROOM);
    }
    else
    {
        sprintf (buf, "You pour some %s for $N.",
                 liq_table[out->value[2]].liq_name);
        act (buf, ch, NULL, vch, TO_CHAR);
        sprintf (buf, "$n pours you some %s.",
                 liq_table[out->value[2]].liq_name);
        act (buf, ch, NULL, vch, TO_VICT);
        sprintf (buf, "$n pours some %s for $N.",
                 liq_table[out->value[2]].liq_name);
        act (buf, ch, NULL, vch, TO_NOTVICT);

    }
}

void do_drink (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int amount;
    int liquid;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        for (obj = ch->in_room->contents; obj; obj = obj->next_content)
        {
            if (obj->item_type == ITEM_FOUNTAIN)
                break;
        }

        if (obj == NULL)
        {
            send_to_char ("Drink what?\n\r", ch);
            return;
        }
    }
    else
    {
        if ((obj = get_obj_here (ch, NULL, arg)) == NULL)
        {
            send_to_char ("You can't find it.\n\r", ch);
            return;
        }
    }

    if (!IS_NPC (ch) && ch->pcdata->condition[COND_DRUNK] > 10)
    {
        send_to_char ("You fail to reach your mouth.  *Hic*\n\r", ch);
        return;
    }

    switch (obj->item_type)
    {
        default:
            send_to_char ("You can't drink from that.\n\r", ch);
            return;

        case ITEM_FOUNTAIN:
            if ((liquid = obj->value[2]) < 0)
            {
                bug ("Do_drink: bad liquid number %d.", liquid);
                liquid = obj->value[2] = 0;
            }
            amount = liq_table[liquid].liq_affect[4] * 3;
            break;

        case ITEM_DRINK_CON:
            if (obj->value[1] <= 0)
            {
                send_to_char ("It is already empty.\n\r", ch);
                return;
            }

            if ((liquid = obj->value[2]) < 0)
            {
                bug ("Do_drink: bad liquid number %d.", liquid);
                liquid = obj->value[2] = 0;
            }

            amount = liq_table[liquid].liq_affect[4];
            amount = UMIN (amount, obj->value[1]);
            break;
    }
    if (!IS_NPC (ch) && !IS_IMMORTAL (ch)
        && ch->pcdata->condition[COND_FULL] > 45)
    {
        send_to_char ("You're too full to drink more.\n\r", ch);
        return;
    }

/*    act ("$n drinks $T from $p.",
         ch, obj, liq_table[liquid].liq_name, TO_ROOM);*/
    act ("You drink $T from $p.",
         ch, obj, liq_table[liquid].liq_name, TO_CHAR);

    gain_condition (ch, COND_DRUNK,
                    amount * liq_table[liquid].liq_affect[COND_DRUNK] / 36);
    gain_condition (ch, COND_FULL,
                    amount * liq_table[liquid].liq_affect[COND_FULL] / 4);
    gain_condition (ch, COND_THIRST,
                    amount * liq_table[liquid].liq_affect[COND_THIRST] / 10);
    gain_condition (ch, COND_HUNGER,
                    amount * liq_table[liquid].liq_affect[COND_HUNGER] / 2);

    if (!IS_NPC (ch) && ch->pcdata->condition[COND_DRUNK] > 10)
    {
        send_to_char ("You feel drunk.\n\r", ch);
        gain_condition(ch, COND_HORNY, -10);
    }
    if (!IS_NPC (ch) && ch->pcdata->condition[COND_FULL] > 60)
        send_to_char ("You are full.\n\r", ch);
    if (!IS_NPC (ch) && ch->pcdata->condition[COND_THIRST] > 60)
        send_to_char ("Your thirst is quenched.\n\r", ch);

    if (obj->value[3] != 0)
    {
        /* The drink was poisoned ! */
        AFFECT_DATA af;

        act ("$n chokes and gags.", ch, NULL, NULL, TO_ROOM);
        send_to_char ("You choke and gag.\n\r", ch);
        af.where = TO_AFFECTS;
        af.type = gsn_poison;
        af.level = number_fuzzy (amount);
        af.duration = 3 * amount;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_POISON;
        affect_join (ch, &af);
    }

    if (obj->value[0] > 0)
        obj->value[1] -= amount;

    return;
}



void do_eat (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument (argument, arg);
    if (arg[0] == '\0')
    {
        send_to_char ("Eat what?\n\r", ch);
        return;
    }

    if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
    {
        send_to_char ("You do not have that item.\n\r", ch);
        return;
    }

    if (!IS_IMMORTAL (ch))
    {
        if (obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL)
        {
            send_to_char ("That's not edible.\n\r", ch);
            return;
        }
    }

    act ("$n eats $p.", ch, obj, NULL, TO_ROOM);
    act ("You eat $p.", ch, obj, NULL, TO_CHAR);

    switch (obj->item_type)
    {

        case ITEM_FOOD:
            if (!IS_NPC (ch))
            {
                int condition;

		if (obj->pIndexData->vnum == OBJ_VNUM_BERRY && ch->hit < ch->max_hit)
		{
		    ch->hit += obj->value[1];
		    if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
		}

                condition = ch->pcdata->condition[COND_HUNGER];
                gain_condition (ch, COND_FULL, obj->value[0]);
                gain_condition (ch, COND_HUNGER, obj->value[1]);
                if (condition == 0 && ch->pcdata->condition[COND_HUNGER] > 0)
                    send_to_char ("You are no longer hungry.\n\r", ch);
                else if (ch->pcdata->condition[COND_FULL] > 40)
                    send_to_char ("You are full.\n\r", ch);
            }

            if (obj->value[3] != 0)
            {
                /* The food was poisoned! */
                AFFECT_DATA af;

                act ("$n chokes and gags.", ch, 0, 0, TO_ROOM);
                send_to_char ("You choke and gag.\n\r", ch);

                af.where = TO_AFFECTS;
                af.type = gsn_poison;
                af.level = number_fuzzy (obj->value[0]);
                af.duration = 2 * obj->value[0];
                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = AFF_POISON;
                affect_join (ch, &af);
            }
            break;

        case ITEM_PILL:
            obj_cast_spell (obj->value[1], obj->value[0], ch, ch, NULL);
            obj_cast_spell (obj->value[2], obj->value[0], ch, ch, NULL);
            obj_cast_spell (obj->value[3], obj->value[0], ch, ch, NULL);
            WAIT_STATE (ch, 10);
            if (!IS_NPC(ch))
	    {
	      ch->pcdata->condition[COND_FULL] += 2;
	      ch->pcdata->condition[COND_HUNGER] += 2;
            }
            break;
    }

    extract_obj (obj);
    return;
}



/*
 * Remove an object.
 */
bool remove_obj (CHAR_DATA * ch, int iWear, bool fReplace)
{
    OBJ_DATA *obj;

    if ((obj = get_eq_char (ch, iWear)) == NULL)
        return TRUE;

    if (!fReplace)
        return FALSE;

    if (IS_SET (obj->extra_flags, ITEM_NOREMOVE))
    {
        act ("You can't remove $p.", ch, obj, NULL, TO_CHAR);
        return FALSE;
    }

    unequip_char (ch, obj);
    act ("$n stops using $p.", ch, obj, NULL, TO_ROOM);
    act ("You stop using $p.", ch, obj, NULL, TO_CHAR);
    return TRUE;
}



/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj (CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace)
{
    char buf[MAX_STRING_LENGTH];
    
    if (is_obj_outlevel(ch,obj))
    {
        sprintf (buf, "You don't have the skills necessary to use that yet.\n\r");
        send_to_char (buf, ch);
        act ("$n tries to use $p, but is too inexperienced.",
             ch, obj, NULL, TO_ROOM);
        return;
    }

    if (is_obj_classrestrict(ch, obj))
    {
      send_to_char("That object cannot be worn by your class.\n\r", ch);
      return;
    }

    if (obj->condition < 1)
    {
      send_to_char("You can't wear that it's broken.\n\r", ch);
      return;
    }

    if (obj->item_type == ITEM_LIGHT)
    {
        if (!remove_obj (ch, WEAR_LIGHT, fReplace))
            return;
        act ("$n lights $p and holds it.", ch, obj, NULL, TO_ROOM);
        act ("You light $p and hold it.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_LIGHT);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_FINGER))
    {
        if (get_eq_char (ch, WEAR_FINGER_L) != NULL
            && get_eq_char (ch, WEAR_FINGER_R) != NULL
            && !remove_obj (ch, WEAR_FINGER_L, fReplace)
            && !remove_obj (ch, WEAR_FINGER_R, fReplace))
            return;

        if (get_eq_char (ch, WEAR_FINGER_L) == NULL)
        {
            act ("$n wears $p on $s left finger.", ch, obj, NULL, TO_ROOM);
            act ("You wear $p on your left finger.", ch, obj, NULL, TO_CHAR);
            equip_char (ch, obj, WEAR_FINGER_L);
            return;
        }

        if (get_eq_char (ch, WEAR_FINGER_R) == NULL)
        {
            act ("$n wears $p on $s right finger.", ch, obj, NULL, TO_ROOM);
            act ("You wear $p on your right finger.", ch, obj, NULL, TO_CHAR);
            equip_char (ch, obj, WEAR_FINGER_R);
            return;
        }

        bug ("Wear_obj: no free finger.", 0);
        send_to_char ("You already wear two rings.\n\r", ch);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_NECK))
    {
        if (get_eq_char (ch, WEAR_NECK_1) != NULL
            && get_eq_char (ch, WEAR_NECK_2) != NULL
            && !remove_obj (ch, WEAR_NECK_1, fReplace)
            && !remove_obj (ch, WEAR_NECK_2, fReplace))
            return;

        if (get_eq_char (ch, WEAR_NECK_1) == NULL)
        {
            act ("$n wears $p around $s neck.", ch, obj, NULL, TO_ROOM);
            act ("You wear $p around your neck.", ch, obj, NULL, TO_CHAR);
            equip_char (ch, obj, WEAR_NECK_1);
            return;
        }

        if (get_eq_char (ch, WEAR_NECK_2) == NULL)
        {
            act ("$n wears $p around $s neck.", ch, obj, NULL, TO_ROOM);
            act ("You wear $p around your neck.", ch, obj, NULL, TO_CHAR);
            equip_char (ch, obj, WEAR_NECK_2);
            return;
        }

        bug ("Wear_obj: no free neck.", 0);
        send_to_char ("You already wear two neck items.\n\r", ch);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_BODY))
    {
        if (!remove_obj (ch, WEAR_BODY, fReplace))
            return;
        act ("$n wears $p on $s torso.", ch, obj, NULL, TO_ROOM);
        act ("You wear $p on your torso.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_BODY);
        return;
    }
    
    if (CAN_WEAR (obj, ITEM_WEAR_ON_BELT))
    {
        if (!remove_obj (ch, WEAR_ON_BELT, fReplace))
            return;

        if (get_eq_char (ch, WEAR_WAIST) == NULL)
        {
          send_to_char("You must be wearing a belt to hook the keyring on.\n\r", ch);
          return;
        }
        
        act ("$n hooks $p on $s belt.", ch, obj, NULL, TO_ROOM);
        act ("You hook $p on your belt.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_ON_BELT);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_HEAD))
    {
        if (!remove_obj (ch, WEAR_HEAD, fReplace))
            return;
        act ("$n wears $p on $s head.", ch, obj, NULL, TO_ROOM);
        act ("You wear $p on your head.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_HEAD);
        return;
    }

	if (CAN_WEAR (obj, ITEM_WEAR_FACE))
    {
        if (!remove_obj (ch, WEAR_FACE, fReplace))
            return;
        act ("$n wears $p on $s face.", ch, obj, NULL, TO_ROOM);
        act ("You wear $p on your face.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_FACE);
        return;
    }
    if (CAN_WEAR (obj, ITEM_WEAR_QUIVER))
    {
        if (get_skill(ch,gsn_bow)>0||obj->item_type==ITEM_ARMOR )
        {
        if (!remove_obj (ch, WEAR_QUIVER, fReplace))
            return;
        
        act ("$n wears $p on $s shoulder.", ch, obj, NULL, TO_ROOM);
        act ("You wear $p on your shoulder.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_QUIVER);
        }
        else
        send_to_char ("You don't have any need to wear a cumbersome quiver if you don't know how to use a bow.\n\r",ch);
        
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_LEGS))
    {
        if (!remove_obj (ch, WEAR_LEGS, fReplace))
            return;
        act ("$n wears $p on $s legs.", ch, obj, NULL, TO_ROOM);
        act ("You wear $p on your legs.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_LEGS);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_FEET))
    {
        if (!remove_obj (ch, WEAR_FEET, fReplace))
            return;
        act ("$n wears $p on $s feet.", ch, obj, NULL, TO_ROOM);
        act ("You wear $p on your feet.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_FEET);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_HANDS))
    {
        if (!remove_obj (ch, WEAR_HANDS, fReplace))
            return;
        act ("$n wears $p on $s hands.", ch, obj, NULL, TO_ROOM);
        act ("You wear $p on your hands.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_HANDS);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_ARMS))
    {
        if (!remove_obj (ch, WEAR_ARMS, fReplace))
            return;
        act ("$n wears $p on $s arms.", ch, obj, NULL, TO_ROOM);
        act ("You wear $p on your arms.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_ARMS);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_ABOUT))
    {
        if (!remove_obj (ch, WEAR_ABOUT, fReplace))
            return;
        act ("$n wears $p about $s torso.", ch, obj, NULL, TO_ROOM);
        act ("You wear $p about your torso.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_ABOUT);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_WAIST))
    {
        if (!remove_obj (ch, WEAR_WAIST, fReplace))
            return;
        act ("$n wears $p about $s waist.", ch, obj, NULL, TO_ROOM);
        act ("You wear $p about your waist.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_WAIST);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_WRIST))
    {
        if (get_eq_char (ch, WEAR_WRIST_L) != NULL
            && get_eq_char (ch, WEAR_WRIST_R) != NULL
            && !remove_obj (ch, WEAR_WRIST_L, fReplace)
            && !remove_obj (ch, WEAR_WRIST_R, fReplace))
            return;

        if (get_eq_char (ch, WEAR_WRIST_L) == NULL)
        {
            act ("$n wears $p around $s left wrist.", ch, obj, NULL, TO_ROOM);
            act ("You wear $p around your left wrist.",
                 ch, obj, NULL, TO_CHAR);
            equip_char (ch, obj, WEAR_WRIST_L);
            return;
        }

        if (get_eq_char (ch, WEAR_WRIST_R) == NULL)
        {
            act ("$n wears $p around $s right wrist.",
                 ch, obj, NULL, TO_ROOM);
            act ("You wear $p around your right wrist.",
                 ch, obj, NULL, TO_CHAR);
            equip_char (ch, obj, WEAR_WRIST_R);
            return;
        }

        bug ("Wear_obj: no free wrist.", 0);
        send_to_char ("You already wear two wrist items.\n\r", ch);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_SHIELD))
    {
        OBJ_DATA *weapon;

        if (!remove_obj (ch, WEAR_SHIELD, fReplace))
            return;

        weapon = get_eq_char (ch, WEAR_WIELD);
        if (weapon != NULL && ch->size < SIZE_LARGE
            && IS_WEAPON_STAT (weapon, WEAPON_TWO_HANDS))
        {
            send_to_char ("Your hands are tied up with your weapon!\n\r", ch);
            return;
        }

        if (get_eq_char(ch, WEAR_SECONDARY) != NULL)
        {
           send_to_char("You can't wear a shield while dual wielding.\n\r", ch);
           return;
        } 

        act ("$n wears $p as a shield.", ch, obj, NULL, TO_ROOM);
        act ("You wear $p as a shield.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_SHIELD);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WIELD))
    {
        int sn, skill;
        OBJ_DATA *second;

        if (!remove_obj (ch, WEAR_WIELD, fReplace))
            return;

        if (!IS_NPC (ch)
            && get_obj_weight (obj) >
            (str_app[get_curr_stat (ch, STAT_STR)].wield * 10))
        {
            send_to_char ("It is too heavy for you to wield.\n\r", ch);
            return;
        }

        if (!IS_NPC (ch) && ch->size < SIZE_LARGE
            && IS_WEAPON_STAT (obj, WEAPON_TWO_HANDS)
            && get_eq_char (ch, WEAR_SHIELD) != NULL)
        {
            send_to_char ("You need two hands free for that weapon.\n\r", ch);
            return;
        }

        act ("$n wields $p.", ch, obj, NULL, TO_ROOM);
        act ("You wield $p.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_WIELD);
        
        if ((second = get_eq_char(ch, WEAR_SECONDARY)) != NULL)
          unequip_char(ch, second);

        sn = get_weapon_sn (ch, FALSE);

        if (sn == gsn_hand_to_hand)
            return;

        skill = get_weapon_skill (ch, sn);

        if (skill >= 100)
            act ("$p feels like a part of you!", ch, obj, NULL, TO_CHAR);
        else if (skill > 85)
            act ("You feel quite confident with $p.", ch, obj, NULL, TO_CHAR);
        else if (skill > 70)
            act ("You are skilled with $p.", ch, obj, NULL, TO_CHAR);
        else if (skill > 50)
            act ("Your skill with $p is adequate.", ch, obj, NULL, TO_CHAR);
        else if (skill > 25)
            act ("$p feels a little clumsy in your hands.", ch, obj, NULL,
                 TO_CHAR);
        else if (skill > 1)
            act ("You fumble and almost drop $p.", ch, obj, NULL, TO_CHAR);
        else
            act ("You don't even know which end is up on $p.",
                 ch, obj, NULL, TO_CHAR);

        return;
    }

    if (CAN_WEAR (obj, ITEM_HOLD))
    {
        if (!remove_obj (ch, WEAR_HOLD, fReplace))
            return;

        if (get_eq_char(ch, WEAR_SECONDARY) != NULL)
        {
           send_to_char("You can't hold an item while dual wielding.\n\r", ch);
           return;
        }     
  
        act ("$n holds $p in $s hand.", ch, obj, NULL, TO_ROOM);
        act ("You hold $p in your hand.", ch, obj, NULL, TO_CHAR);
        equip_char (ch, obj, WEAR_HOLD);
        return;
    }

    if (CAN_WEAR (obj, ITEM_WEAR_FLOAT))
    {
        if (!remove_obj (ch, WEAR_FLOAT, fReplace))
            return;
        act ("$n releases $p to float next to $m.", ch, obj, NULL, TO_ROOM);
        act ("You release $p and it floats next to you.", ch, obj, NULL,
             TO_CHAR);
        equip_char (ch, obj, WEAR_FLOAT);
        return;
    }

    if (fReplace)
        send_to_char ("You can't wear, wield, or hold that.\n\r", ch);

    return;
}

void do_second (CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *obj;
  char buf [MIL];

  if (get_skill(ch, gsn_dual_wield) < 1)
  {
    send_to_char("You couldn't begin to even try that.\n\r", ch);
    return;
  }
 
  if (argument[0] == '\0')
  {
    send_to_char("Second what?\n\r", ch);
    return;
  } 

  if ((obj = get_obj_carry(ch, argument, ch)) == NULL)
  {
    send_to_char("You aren't carrying that.\n\r", ch);
    return;
  }

  if ( (get_eq_char(ch, WEAR_SHIELD) != NULL) ||
        (get_eq_char(ch, WEAR_HOLD) != NULL) )
  {
    send_to_char("You cannot use a secondary weapon while usin a shield or holding an item.\n\r", ch);
    return;
  }
  
  if (get_eq_char(ch, WEAR_WIELD) == NULL)
  {
    send_to_char("You have to be wearing a primary weapon first.\n\r", ch);
    return;
  }

  if (is_obj_outlevel(ch,obj))
    {
        sprintf (buf, "You don't have the skills necessary to use that yet.\n\r");
        send_to_char (buf, ch);
        act ("$n tries to use $p, but is too inexperienced.",
             ch, obj, NULL, TO_ROOM);
        return;
    }
  
  if ( (get_obj_weight(obj)*2) > get_obj_weight(get_eq_char(ch, WEAR_WIELD)))
  {
    send_to_char("Too heavy to wear as a secondary weapon.\n\r", ch);
    return;
  }

  if (!CAN_WEAR(obj, ITEM_WIELD))
  {
    send_to_char("You can't second that.\n\r", ch);
    return;
  }

  if (!remove_obj(ch, WEAR_SECONDARY, TRUE))
    return;

  act("$n wields $p in $s other hand.", ch, obj, NULL, TO_ROOM);
  act("You wield $p in your other hand.", ch, obj, NULL, TO_CHAR);
  equip_char(ch, obj, WEAR_SECONDARY);
  return;
}   
 
void do_wear (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Wear, wield, or hold what?\n\r", ch);
        return;
    }
    
    if (!str_cmp (arg, "all"))
    {
        OBJ_DATA *obj_next;

        for (obj = ch->carrying; obj != NULL; obj = obj_next)
        {
            obj_next = obj->next_content;
            if (obj->wear_loc == WEAR_NONE && can_see_obj (ch, obj))
                wear_obj (ch, obj, FALSE);
        }
        return;
    }
    else
    {
        if (((obj = get_obj_carry (ch, arg, ch)) == NULL))
        {
            send_to_char ("You do not have that item.\n\r", ch);
            return;
        }

        wear_obj (ch, obj, TRUE);
    }

    return;
}



void do_remove (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Remove what?\n\r", ch);
        return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	OBJ_DATA *obj_next;

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc != WEAR_NONE && can_see_obj( ch, obj ) )
		remove_obj( ch, obj->wear_loc, TRUE );
	}
	return;
    }

    if ((obj = get_obj_wear (ch, arg, TRUE)) == NULL)
    {
        send_to_char ("You do not have that item.\n\r", ch);
        return;
    }

    remove_obj (ch, obj->wear_loc, TRUE);
    return;
}
void donate (CHAR_DATA *ch, OBJ_DATA *obj)
{
  char buf[MSL];
  ROOM_INDEX_DATA *donation;
  CHAR_DATA *fakech;

    if ( !CAN_WEAR(obj, ITEM_TAKE))
    {
      act( "$p is not an acceptable donation.", ch, obj, 0, TO_CHAR );
      return;
    }

    donation = get_room_index( ROOM_VNUM_DONATION );

    send_to_char("You will your item to be donated.\n\r", ch);
    sprintf(buf, "%s miraculously disappears.", obj->short_descr);
    act(buf, ch, NULL, NULL, TO_ROOM);
    act(buf, ch, NULL, NULL, TO_CHAR);
    obj_from_room(obj);
    obj_to_room(obj, donation );
    if ((fakech = donation->people) == NULL)
      return;
    sprintf(buf, "With a loud BOOM %s apears in the room at your feet.\n\r", obj->short_descr);      
    act(buf, fakech, obj, NULL, TO_ROOM);
    act(buf, fakech, obj, NULL, TO_CHAR);
    return;
}
 
void do_donate(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  OBJ_DATA *obj, *obj_next;
  bool found;
  int donation;

  argument = one_argument( argument, arg);
  one_argument( argument, arg2);

  if ( arg[0] == '\0' )
  {
    send_to_char("Donate what?\n\r", ch);
    return;
  }

  if (!str_cmp (arg, "quest"))
  {
    if ( arg2[0] == '\0' )
    {
    send_to_char("How many quest points are you going to donate?\n\r", ch);
    return;
    }

    if (ch->questpoints < 0)
    {
      send_to_char("You have no quest points to donate.\n\r", ch);
      return;
    }
    donation = atoi(arg2);
    if (donation > ch->questpoints || donation < 0 || donation > 30000)
    {
     send_to_char("You dont have that many quest points or the amount is invalid.\n\r", ch);
     return;
    }
    sprintf(buf, "You donate %d quest points to your guild's coffer.\n\r", donation);
    send_to_char(buf, ch);
    clan_table[ch->clan].roster.questpoints += donation;
    member_quest(ch, donation);
    ch->questpoints -= donation;
    save_one_roster(ch->clan);
    return;    
  }

  if (str_cmp (arg, "all") && str_prefix("all.", arg))
  {
    if ((obj = get_obj_list(ch, arg, ch->in_room->contents)) == NULL)
    {
      send_to_char("You don't see that here.\n\r", ch);
      return;
    }

   donate(ch, obj);
  }
  else
  {
    found = FALSE;
    for (obj = ch->in_room->contents; obj != NULL ; obj = obj_next)
    {
      obj_next = obj->next_content;
      if ((arg[3] == '\0' || is_name (&arg[4], obj->name))
         && can_see_obj(ch, obj))
      {
        found = TRUE;
        donate(ch, obj);
      }
    }
    if (!found)
    {
      if (arg[3] == '\0')
        send_to_char("I see nothing here.\n\r", ch);
      else
        act("I see no $T here.", ch, NULL, &arg[4], TO_CHAR);
    }
  }
}

void do_sacrifice (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int silver;

    /* variables for AUTOSPLIT */
    CHAR_DATA *gch;
    int members;
    char buffer[100];


    one_argument (argument, arg);

    if (arg[0] == '\0' || !str_cmp (arg, ch->name))
    {
        act ("$n offers $mself to The Creator, who graciously declines.",
             ch, NULL, NULL, TO_ROOM);
        send_to_char
            ("The Creator appreciates your offer and may accept it later.\n\r", ch);
        return;
    }

    obj = get_obj_list (ch, arg, ch->in_room->contents);
    if (obj == NULL)
    {
        send_to_char ("You can't find it.\n\r", ch);
        return;
    }

    if (obj->item_type == ITEM_CORPSE_PC)
    {
        if (obj->contains)
        {
            send_to_char ("The Creator wouldn't like that.\n\r", ch);
            return;
        }
    }


    if (!CAN_WEAR (obj, ITEM_TAKE) || CAN_WEAR (obj, ITEM_NO_SAC))
    {
        act ("$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR);
        return;
    }

    if (obj->in_room != NULL)
    {
        for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room)
            if (gch->on == obj)
            {
                act ("$N appears to be using $p.", ch, obj, gch, TO_CHAR);
                return;
            }
    }

    if (obj->item_type == ITEM_CORPSE_PC && str_cmp(ch->name, obj->owner))
    {
      send_to_char("That's not your corpse.\n\r", ch);
      return;
    }

    silver = int(UMAX (1, obj->level * 1.25));

    if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
        silver = UMIN (silver, obj->cost);

    if (obj->item_type == ITEM_CORPSE_PC)
        silver = 1;

    if (silver == 1)
        send_to_char
            ("The Creator gives you one silver coin for your sacrifice.\n\r", ch);
    else
    {
        sprintf (buf,
                 "The Creator gives you %d silver coins for your sacrifice.\n\r",
                 silver);
        send_to_char (buf, ch);
    }

    ch->silver += silver;

    if (IS_SET (ch->act, PLR_AUTOSPLIT))
    {                            /* AUTOSPLIT code */
        members = 0;
        for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
        {
            if (is_same_group (gch, ch))
                members++;
        }

        if (members > 1 && silver > 1)
        {
            sprintf (buffer, "%d", silver);
            do_function (ch, &do_split, buffer);
        }
    }

    act ("$n sacrifices $p to The Creator.", ch, obj, NULL, TO_ROOM);
    wiznet ("$N sends up $p as a burnt offering.",
            ch, obj, WIZ_SACCING, 0, 0);
    
    extract_obj (obj);
    return;
}



void do_quaff (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Quaff what?\n\r", ch);
        return;
    }

    if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
    {
        send_to_char ("You do not have that potion.\n\r", ch);
        return;
    }

    if (obj->item_type != ITEM_POTION)
    {
        send_to_char ("You can quaff only potions.\n\r", ch);
        return;
    }

    if (ch->level < obj->level)
    {
        send_to_char ("This liquid is too powerful for you to drink.\n\r",
                      ch);
        return;
    }

    act ("$n quaffs $p.", ch, obj, NULL, TO_ROOM);
    act ("You quaff $p.", ch, obj, NULL, TO_CHAR);

    obj_cast_spell (obj->value[1], obj->value[0], ch, ch, NULL);
    obj_cast_spell (obj->value[2], obj->value[0], ch, ch, NULL);
    obj_cast_spell (obj->value[3], obj->value[0], ch, ch, NULL);

    if (!IS_NPC(ch))
    {
      ch->pcdata->condition[COND_FULL] += 4;
      ch->pcdata->condition[COND_THIRST] +=4;
    }
    WAIT_STATE (ch, 10);
    extract_obj (obj);
    return;
}



void do_recite (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *scroll;
    OBJ_DATA *obj;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if ((scroll = get_obj_carry (ch, arg1, ch)) == NULL)
    {
        send_to_char ("You do not have that scroll.\n\r", ch);
        return;
    }

    if (scroll->item_type != ITEM_SCROLL)
    {
        send_to_char ("You can recite only scrolls.\n\r", ch);
        return;
    }

    if (ch->level < scroll->level)
    {
        send_to_char ("This scroll is too complex for you to comprehend.\n\r",
                      ch);
        return;
    }

    obj = NULL;
    if (arg2[0] == '\0')
    {
        victim = ch;
    }
    else
    {
        if ((victim = get_char_room (ch, NULL, arg2)) == NULL
            && (obj = get_obj_here (ch, NULL, arg2)) == NULL)
        {
            send_to_char ("You can't find it.\n\r", ch);
            return;
        }
    }

    act ("$n recites $p.", ch, scroll, NULL, TO_ROOM);
    act ("You recite $p.", ch, scroll, NULL, TO_CHAR);

    if (number_percent () >= 20 + get_skill (ch, gsn_scrolls) * 4 / 5)
    {
        send_to_char ("You mispronounce a syllable.\n\r", ch);
        check_improve (ch, gsn_scrolls, FALSE, 2);
        WAIT_STATE(ch, 10);
    }

    else
    {
        int lsum, sum = 0, number = 0;
     
        sum += obj_cast_spell (scroll->value[1], scroll->value[0], ch, victim, obj);
        if (sum > 0) number++;
        lsum = sum;
        sum += obj_cast_spell (scroll->value[2], scroll->value[0], ch, victim, obj);
        if (sum > lsum) number++;
        lsum = sum;
        sum += obj_cast_spell (scroll->value[3], scroll->value[0], ch, victim, obj);
        if (sum > lsum) number++;
        check_improve (ch, gsn_scrolls, TRUE, 2);
        if (number >= 1)
          WAIT_STATE(ch, (sum / number) + number * 2);
    }

    extract_obj (scroll);
    return;
}



void do_brandish (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA *staff;
    int sn;

    if ((staff = get_eq_char (ch, WEAR_HOLD)) == NULL)
    {
        send_to_char ("You hold nothing in your hand.\n\r", ch);
        return;
    }

    if (staff->item_type != ITEM_STAFF)
    {
        send_to_char ("You can brandish only with a staff.\n\r", ch);
        return;
    }

    if ((sn = staff->value[3]) < 0
        || sn >= MAX_SKILL || skill_table[sn].spell_fun == 0)
    {
        bug ("Do_brandish: bad sn %d.", sn);
        return;
    }

    WAIT_STATE (ch, 2 * PULSE_VIOLENCE);

    if (staff->value[2] > 0)
    {
        act ("$n brandishes $p.", ch, staff, NULL, TO_ROOM);
        act ("You brandish $p.", ch, staff, NULL, TO_CHAR);
        if (ch->level < staff->level
            || number_percent () >= 20 + get_skill (ch, gsn_staves) * 4 / 5)
        {
            act ("You fail to invoke $p.", ch, staff, NULL, TO_CHAR);
            act ("...and nothing happens.", ch, NULL, NULL, TO_ROOM);
            check_improve (ch, gsn_staves, FALSE, 2);
        }

        else
            for (vch = ch->in_room->people; vch; vch = vch_next)
            {
                vch_next = vch->next_in_room;

                switch (skill_table[sn].target)
                {
                    default:
                        bug ("Do_brandish: bad target for sn %d.", sn);
                        return;

                    case TAR_IGNORE:
                        if (vch != ch)
                            continue;
                        break;

                    case TAR_CHAR_OFFENSIVE:
                        if (IS_NPC (ch) ? IS_NPC (vch) : !IS_NPC (vch))
                            continue;
                        break;

                    case TAR_CHAR_DEFENSIVE:
                        if (IS_NPC (ch) ? !IS_NPC (vch) : IS_NPC (vch))
                            continue;
                        break;

                    case TAR_CHAR_SELF:
                        if (vch != ch)
                            continue;
                        break;
                }

                obj_cast_spell (staff->value[3], staff->value[0], ch, vch,
                                NULL);
                check_improve (ch, gsn_staves, TRUE, 2);
            }
    }

    if (--staff->value[2] <= 0)
    {
        act ("$n's $p blazes bright and is gone.", ch, staff, NULL, TO_ROOM);
        act ("Your $p blazes bright and is gone.", ch, staff, NULL, TO_CHAR);
        extract_obj (staff);
    }

    return;
}



void do_zap (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *wand;
    OBJ_DATA *obj;

    one_argument (argument, arg);
    if (arg[0] == '\0' && ch->fighting == NULL)
    {
        send_to_char ("Zap whom or what?\n\r", ch);
        return;
    }

    if ((wand = get_eq_char (ch, WEAR_HOLD)) == NULL)
    {
        send_to_char ("You hold nothing in your hand.\n\r", ch);
        return;
    }

    if (wand->item_type != ITEM_WAND)
    {
        send_to_char ("You can zap only with a wand.\n\r", ch);
        return;
    }

    obj = NULL;
    if (arg[0] == '\0')
    {
        if (ch->fighting != NULL)
        {
            victim = ch->fighting;
        }
        else
        {
            send_to_char ("Zap whom or what?\n\r", ch);
            return;
        }
    }
    else
    {
        if ((victim = get_char_room (ch, NULL, arg)) == NULL
            && (obj = get_obj_here (ch, NULL, arg)) == NULL)
        {
            send_to_char ("You can't find it.\n\r", ch);
            return;
        }
    }

    WAIT_STATE (ch, 2 * PULSE_VIOLENCE);

    if (wand->value[2] > 0)
    {
        if (victim != NULL)
        {
            act ("$n zaps $N with $p.", ch, wand, victim, TO_NOTVICT);
            act ("You zap $N with $p.", ch, wand, victim, TO_CHAR);
            act ("$n zaps you with $p.", ch, wand, victim, TO_VICT);
        }
        else
        {
            act ("$n zaps $P with $p.", ch, wand, obj, TO_ROOM);
            act ("You zap $P with $p.", ch, wand, obj, TO_CHAR);
        }

        if (ch->level < wand->level
            || number_percent () >= 20 + get_skill (ch, gsn_wands) * 4 / 5)
        {
            act ("Your efforts with $p produce only smoke and sparks.",
                 ch, wand, NULL, TO_CHAR);
            act ("$n's efforts with $p produce only smoke and sparks.",
                 ch, wand, NULL, TO_ROOM);
            check_improve (ch, gsn_wands, FALSE, 2);
        }
        else
        {
            obj_cast_spell (wand->value[3], wand->value[0], ch, victim, obj);
            check_improve (ch, gsn_wands, TRUE, 2);
        }
    }

    if (--wand->value[2] <= 0)
    {
        act ("$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM);
        act ("Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR);
        extract_obj (wand);
    }

    return;
}



void do_steal (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;
    int npercent;

    

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char ("Steal what from whom?\n\r", ch);
        return;
    }
    if ((victim = get_char_room (ch, NULL, arg2)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL)
    {
    	send_to_char ("Why would you want anything from an immortal?\n\r", ch);
    	return;
    }	

    if (GET_HITROLL(ch) < (ch->level + 20) || GET_DAMROLL(ch) < (ch->level + 20))
    {
       sprintf(buf, "H - %d D - %d", ch->hitroll, ch->damroll);    
    //   send_to_char(buf, ch);
       send_to_char("You are too nekkid to blend in with the crowd.\n\r", ch);
       return;
    }

    if (victim == ch)
    {
        send_to_char ("That's pointless.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

    if (!IS_NPC(victim) && IS_SET(ch->act, PLR_QUESTING))
    {
      send_to_char("Not while they are questing.\n\r", ch);
      return;
    }

    if (!IS_NPC(victim) && IS_SET(ch->act, PLR_TOURNEY))
    {
      send_to_char("Not while they are in a tournament.\n\r", ch);
      return;
    }

    if (victim->position == POS_FIGHTING)
    {
        send_to_char ("You'd better not -- you might get hit.\n\r", ch);
        return;
    }
    
    if IS_AFFECTED(ch, AFF_SHROUD)
    {
        send_to_char ("You cannot reach them from the shadows.\n\r", ch);
        return; 
    }
    WAIT_STATE (ch, skill_table[gsn_steal].beats);
   
    /*Modifiers*/
      
    if (can_see (victim, ch))
        percent = 25;
    else
        percent = 50;
   
    if (!IS_NPC(ch) && (get_skill(ch, gsn_steal < 1)) && IS_AWAKE(victim))
    {
      send_to_char("You would never attempt that while they are awake, you lack the skill.\n\r", ch);
      return;
    }


    if (ch->pcdata->wait_timer > 0)
      ch->pcdata->wait_timer = 0;  
    percent += (get_skill (ch, gsn_steal)/4);
    percent += int((dex_app[get_curr_stat(ch,STAT_DEX)].offensive)*1.5);
    percent += (int_app[get_curr_stat(ch,STAT_INT)].learn)/10;
    percent -= (int_app[get_curr_stat(victim,STAT_INT)].learn)/6;
    percent += ch->level > victim->level ? (ch->level - victim->level) :
               (ch->level - victim->level) * 2;
   
    if (!str_cmp("assassin", class_table[ch->cClass].name))
       percent /= 2;
    if (!str_cmp("rogue", class_table[ch->cClass].name))
       percent += 10;
    if (!IS_AWAKE(victim))
       percent += 50;
    percent = URANGE(5,percent,95);
    npercent = number_range (1,100);
    sprintf(buf,"%d percent %d npercent\n\r",percent,npercent);
    if (IS_IMMORTAL(ch))
    send_to_char(buf,ch);
    if (!IS_NPC (ch) && (npercent > percent))
       {
        /*
         * Failure.
         */
        sprintf(buf, "%s tried to steal from %s, but failed.\n\r", ch->name, victim->name);
        if (!IS_NPC(victim))
          wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0); 
        send_to_char ("Oops.\n\r", ch);
        affect_strip (ch, gsn_sneak);
        STR_REMOVE_BIT (ch->affected_by, AFF_SNEAK);
        affect_strip (ch, gsn_hide);
        STR_REMOVE_BIT (ch->affected_by, AFF_HIDE);
        affect_strip (ch, gsn_shroud);
        STR_REMOVE_BIT (ch->affected_by, AFF_SHROUD);
	sprintf (buf,"%s tried to steal from you.\n\r", can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone");
        act (buf, ch, NULL, victim, TO_VICT);
        sprintf (buf,"%s tried to steal from $N.\n\r", can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone");
        act (buf, ch, NULL, victim, TO_NOTVICT);
        switch (number_range (0, 3))
        {
            case 0:
                sprintf (buf, "%s is a lousy thief!", can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone");
                break;
            case 1:
                sprintf (buf, "%s couldn't rob %s way out of a paper bag!",
                         can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone", (ch->sex == 2) ? "her" : "his");
                break;
            case 2:
                sprintf (buf, "%s tried to rob me!", can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone");
                break;
            case 3:
                sprintf (buf, "Keep your hands out of there, %s!", can_see(victim,ch)?PERS(ch,victim,FALSE):"Someone");
                break;
        }
        if (!IS_AWAKE (victim))
            do_function (victim, &do_wake, "");
        if (IS_AWAKE (victim))
            do_function (victim, &do_yell, buf);
        if (!IS_NPC (ch))
        {
            if (IS_NPC (victim))
            {
                check_improve (ch, gsn_steal, FALSE, 2);
                multi_hit (victim, ch, TYPE_UNDEFINED);
            }
            else
            {
                sprintf (buf, "$N tried to steal from %s.", victim->name);
                wiznet (buf, ch, NULL, WIZ_FLAGS, 0, 0);
                multi_hit (victim, ch, TYPE_UNDEFINED);
  
            }
        }
		if (!IS_NPC(victim)&&!IS_NPC(ch))
		{
        sprintf (buf,"%s (level %d) tried to steal from %s (level %d)",
		ch->name,ch->level,victim->name,victim->level);
	    log_special (buf, PK_TYPE);
		}
        return;
    }

    if (!str_cmp (arg1, "coin")
        || !str_cmp (arg1, "coins")
        || !str_cmp (arg1, "gold") || !str_cmp (arg1, "silver"))
    {
        int gold, silver;

        gold = victim->gold * number_range (1, ch->level) / MAX_LEVEL;
        silver = victim->silver * number_range (1, ch->level) / MAX_LEVEL;
        if (gold <= 0 && silver <= 0)
        {
            send_to_char ("You couldn't get any coins.\n\r", ch);
            if (!IS_NPC(victim)&&!IS_NPC(ch))
            {
	      sprintf (buf,"%s (level %d) failed to steal coins from %s (level %d)",
              ch->name,ch->level,victim->name,victim->level);
              wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0); 
	      log_special (buf, PK_TYPE);
	    }
			
            return;
        }


        if (!IS_NPC(victim)&&!IS_NPC(ch))
	{
          ch->pcdata->pk_timer = 120;
	  ch->pcdata->safe_timer = 0;        

	}
        ch->gold += gold;
        ch->silver += silver;
        victim->silver -= silver;
        victim->gold -= gold;
        if (silver <= 0)
            sprintf (buf, "Bingo!  You got %d gold coins.\n\r", gold);
        else if (gold <= 0)
            sprintf (buf, "Bingo!  You got %d silver coins.\n\r", silver);
        else
            sprintf (buf, "Bingo!  You got %d silver and %d gold coins.\n\r",
                     silver, gold);

        send_to_char (buf, ch);
        check_improve (ch, gsn_steal, TRUE, 2);
        auto_guidenote(ch,victim,GUIDE_THIEF);
		if (!IS_NPC(victim)&&!IS_NPC(ch))
		{
  		  sprintf (buf,"%s (level %d) stole %d gold %d silver from %s (level %d)",
  	 	    ch->name,ch->level,gold,silver,victim->name,victim->level);
                  wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0); 
	          log_special (buf, PK_TYPE);
		}
        return;
    }

    if ((obj = get_obj_carry (victim, arg1, ch)) == NULL)
    {
        send_to_char ("You can't find it.\n\r", ch);
        return;
    }

    if (!can_drop_obj (ch, obj)
        || IS_SET (obj->extra_flags, ITEM_INVENTORY)
        || ((ch->level < 70)
            && ch->level <= obj->level))
    {
        send_to_char ("You can't pry it away.\n\r", ch);
        return;
    }

    if (ch->carry_number + get_obj_number (obj) > can_carry_n (ch))
    {
        send_to_char ("You have your hands full.\n\r", ch);
        return;
    }

    if (ch->carry_weight + get_obj_weight (obj) > can_carry_w (ch))
    {
        send_to_char ("You can't carry that much weight.\n\r", ch);
        return;
    }
    auto_guidenote(ch,victim,GUIDE_THIEF);
    obj_from_char (obj);
    obj_to_char (obj, ch);
    obj->pk = TRUE;
    act ("You pocket $p.", ch, obj, NULL, TO_CHAR);
    check_improve (ch, gsn_steal, TRUE, 2);
    send_to_char ("Got it!\n\r", ch);
	if (!IS_NPC(victim)&&!IS_NPC(ch))
	{
  	  sprintf (buf,"%s (level %d) stole %s from %s (level %d)",
		ch->name,ch->level,obj->name,victim->name,victim->level);
          wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0); 
	  log_special (buf, PK_TYPE);
	}
        if (!IS_NPC(victim)&&!IS_NPC(ch))
	{
          ch->pcdata->pk_timer = 120;
	  ch->pcdata->safe_timer = 0;
	}
    return;
}



/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper (CHAR_DATA * ch)
{
    /*char buf[MAX_STRING_LENGTH]; */
    CHAR_DATA *keeper;
    SHOP_DATA *pShop;

    pShop = NULL;
    for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room)
    {
        if (IS_NPC (keeper) && (pShop = keeper->pIndexData->pShop) != NULL)
            break;
    }

    if (pShop == NULL)
    {
        send_to_char ("You can't do that here.\n\r", ch);
        return NULL;
    }

    /*
     * Undesirables.
     *
     if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_KILLER) )
     {
     do_function(keeper, &do_say, "Killers are not welcome!");
     sprintf(buf, "%s the KILLER is over here!\n\r", ch->name);
     do_function(keeper, &do_yell, buf );
     return NULL;
     }

     if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_THIEF) )
     {
     do_function(keeper, &do_say, "Thieves are not welcome!");
     sprintf(buf, "%s the THIEF is over here!\n\r", ch->name);
     do_function(keeper, &do_yell, buf );
     return NULL;
     }
     */
    /*
     * Shop hours.
     */
    if (time_info.hour < pShop->open_hour)
    {
        do_function (keeper, &do_say, "Sorry, I am closed. Come back later.");
        return NULL;
    }

    if (time_info.hour > pShop->close_hour)
    {
        do_function (keeper, &do_say,
                     "Sorry, I am closed. Come back tomorrow.");
        return NULL;
    }

    /*
     * Invisible or hidden people.
     */
    if (!can_see (keeper, ch))
    {
        do_function (keeper, &do_say,
                     "I don't trade with folks I can't see.");
        return NULL;
    }

    return keeper;
}

/* insert an object at the right spot for the keeper */
void obj_to_keeper (OBJ_DATA * obj, CHAR_DATA * ch)
{
    OBJ_DATA *t_obj, *t_obj_next;

    /* see if any duplicates are found */
    for (t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next)
    {
        t_obj_next = t_obj->next_content;

        if (obj->pIndexData == t_obj->pIndexData
            && !str_cmp (obj->short_descr, t_obj->short_descr))
        {
            /* if this is an unlimited item, destroy the new one */
            if (IS_OBJ_STAT (t_obj, ITEM_INVENTORY))
            {
                extract_obj (obj);
                return;
            }
            obj->cost = t_obj->cost;    /* keep it standard */
            break;
        }
    }

    if (t_obj == NULL)
    {
        obj->next_content = ch->carrying;
        ch->carrying = obj;
    }
    else
    {
        obj->next_content = t_obj->next_content;
        t_obj->next_content = obj;
    }

    obj->carried_by = ch;
    obj->in_room = NULL;
    obj->in_obj = NULL;
    ch->carry_number += get_obj_number (obj);
    ch->carry_weight += get_obj_weight (obj);
}

/* get an object from a shopkeeper's list */
OBJ_DATA *get_obj_keeper (CHAR_DATA * ch, CHAR_DATA * keeper, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument (argument, arg);
    count = 0;
    for (obj = keeper->carrying; obj != NULL; obj = obj->next_content)
    {
        if (obj->wear_loc == WEAR_NONE && can_see_obj (keeper, obj)
            && can_see_obj (ch, obj) && is_name (arg, obj->name))
        {
            if (++count == number)
                return obj;

            /* skip other objects of the same name */
            while (obj->next_content != NULL
                   && obj->pIndexData == obj->next_content->pIndexData
                   && !str_cmp (obj->short_descr,
                                obj->next_content->short_descr)) obj =
                    obj->next_content;
        }
    }

    return NULL;
}

int get_cost (CHAR_DATA * keeper, OBJ_DATA * obj, bool fBuy)
{
    SHOP_DATA *pShop;
    int cost;

    if (obj == NULL || (pShop = keeper->pIndexData->pShop) == NULL)
        return 0;

    if (fBuy)
    {
        cost = obj->cost * pShop->profit_buy / 100;
    }
    else
    {
        OBJ_DATA *obj2;
        int itype;

        cost = 0;
        for (itype = 0; itype < MAX_TRADE; itype++)
        {
            if (obj->item_type == pShop->buy_type[itype])
            {
                cost = obj->cost * pShop->profit_sell / 100;
                break;
            }
        }

        if (!IS_OBJ_STAT (obj, ITEM_SELL_EXTRACT))
            for (obj2 = keeper->carrying; obj2; obj2 = obj2->next_content)
            {
                if (obj->pIndexData == obj2->pIndexData
                    && !str_cmp (obj->short_descr, obj2->short_descr))
                {
                    if (IS_OBJ_STAT (obj2, ITEM_INVENTORY))
                        cost /= 2;
                    else
                        cost = cost * 3 / 4;
                }
            }
    }

    if (obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND)
    {
        if (obj->value[1] == 0)
            cost /= 4;
        else
            cost = cost * obj->value[2] / obj->value[1];
    }

    return cost;
}



void do_buy (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int cost, roll;

    if (argument[0] == '\0')
    {
        send_to_char ("Buy what?\n\r", ch);
        return;
    }

    if (ch->in_room->store != NULL)
    {
       store_do_buy(ch, argument);
       return;
    }
  
    if (IS_SET (ch->in_room->room_flags, ROOM_PET_SHOP))
    {
        char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        CHAR_DATA *pet;
        ROOM_INDEX_DATA *pRoomIndexNext;
        ROOM_INDEX_DATA *in_room;

        smash_tilde (argument);

        if (IS_NPC (ch))
            return;

        argument = one_argument (argument, arg);

        /* hack to make new thalos pets work */
        if (ch->in_room->vnum == 9621)
            pRoomIndexNext = get_room_index (9706);
        else
            pRoomIndexNext = get_room_index (ch->in_room->vnum + 1);
        if (pRoomIndexNext == NULL)
        {
            bug ("Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum);
            send_to_char ("Sorry, you can't buy that here.\n\r", ch);
            return;
        }

        in_room = ch->in_room;
        ch->in_room = pRoomIndexNext;
        pet = get_char_room (ch, NULL, arg);
        ch->in_room = in_room;

        if (pet == NULL || !IS_SET (pet->act, ACT_PET))
        {
            send_to_char ("Sorry, you can't buy that here.\n\r", ch);
            return;
        }

        if (ch->pet != NULL)
        {
            send_to_char ("You already own a pet.\n\r", ch);
            return;
        }

        cost = 10 * pet->level * pet->level;

        if ((ch->silver + 100 * ch->gold) < cost)
        {
            send_to_char ("You can't afford it.\n\r", ch);
            return;
        }

        if (ch->level < pet->level)
        {
            send_to_char
                ("You're not powerful enough to master this pet.\n\r", ch);
            return;
        }

        /* haggle */
        roll = number_percent ();
        if (roll < get_skill (ch, gsn_haggle))
        {
            cost -= cost / 2 * roll / 100;
            sprintf (buf, "You haggle the price down to %d coins.\n\r", cost);
            send_to_char (buf, ch);
            check_improve (ch, gsn_haggle, TRUE, 4);

        }

        deduct_cost (ch, cost);
        pet = create_mobile (pet->pIndexData);
        if (IS_SET(pet->act, ACT_MOUNT))
        {
          char_to_room(pet, ch->in_room);
          pet->mount = ch;
          send_to_char("Congratulations on your new mount.\n\r", ch);
          return;           
        }

        SET_BIT (pet->act, ACT_PET);
        STR_SET_BIT (pet->affected_by, AFF_CHARM);
        pet->comm = COMM_NOTELL | COMM_NOCHANNELS;

        argument = one_argument (argument, arg);
    /*    if (arg[0] != '\0')
        {
            sprintf (buf, "%s %s", pet->name, arg);
            free_string (pet->name);
            pet->name = str_dup (buf);
        } Removed because of player "healer" abuse */ 

        sprintf (buf, "%sA neck tag says 'I belong to %s'.\n\r",
                 pet->description, ch->name);
        free_string (pet->description);
        pet->description = str_dup (buf);

        char_to_room (pet, ch->in_room);
        add_follower (pet, ch);
        pet->leader = ch;
        ch->pet = pet;
        send_to_char ("Enjoy your pet.\n\r", ch);
        act ("$n bought $N as a pet.", ch, NULL, pet, TO_ROOM);
        return;
    }
    else
    {
        CHAR_DATA *keeper;
        OBJ_DATA *obj, *t_obj;
        char arg[MAX_INPUT_LENGTH];
        int number, count = 1;

        if ((keeper = find_keeper (ch)) == NULL)
            return;

        number = mult_argument (argument, arg);
        obj = get_obj_keeper (ch, keeper, arg);
        cost = get_cost (keeper, obj, TRUE);

        if (number < 1 || number > 99)
        {
            act ("$n tells you 'Get real!", keeper, NULL, ch, TO_VICT);
            return;
        }

        if (cost <= 0 || !can_see_obj (ch, obj))
        {
            act ("$n tells you 'I don't sell that -- try 'list''.",
                 keeper, NULL, ch, TO_VICT);
            ch->oreply = keeper;
            return;
        }

        if (!IS_OBJ_STAT (obj, ITEM_INVENTORY))
        {
            for (t_obj = obj->next_content;
                 count < number && t_obj != NULL; t_obj = t_obj->next_content)
            {
                if (t_obj->pIndexData == obj->pIndexData
                    && !str_cmp (t_obj->short_descr, obj->short_descr))
                    count++;
                else
                    break;
            }

            if (count < number)
            {
                act ("$n tells you 'I don't have that many in stock.",
                     keeper, NULL, ch, TO_VICT);
                ch->oreply = keeper;
                return;
            }
        }

        if ((ch->silver + ch->gold * 100) < cost * number)
        {
            if (number > 1)
                act ("$n tells you 'You can't afford to buy that many.",
                     keeper, obj, ch, TO_VICT);
            else
                act ("$n tells you 'You can't afford to buy $p'.",
                     keeper, obj, ch, TO_VICT);
            ch->oreply = keeper;
            return;
        }

        if (is_obj_outlevel(ch, obj))
        {
            act ("$n tells you 'You can't use $p yet'.",
                 keeper, obj, ch, TO_VICT);
            ch->oreply = keeper;
            return;
        }

        if (ch->carry_number + number * get_obj_number (obj) >
            can_carry_n (ch))
        {
            send_to_char ("You can't carry that many items.\n\r", ch);
            return;
        }

        if (ch->carry_weight + number * get_obj_weight (obj) >
            can_carry_w (ch))
        {
            send_to_char ("You can't carry that much weight.\n\r", ch);
            return;
        }

        /* haggle */
        roll = number_percent ();
        if (!IS_OBJ_STAT (obj, ITEM_SELL_EXTRACT)
            && roll < get_skill (ch, gsn_haggle))
        {
            cost -= obj->cost / 2 * roll / 100;
            act ("You haggle with $N.", ch, NULL, keeper, TO_CHAR);
            check_improve (ch, gsn_haggle, TRUE, 4);
        }

        if (number > 1)
        {
            sprintf (buf, "$n buys $p[%d].", number);
            act (buf, ch, obj, NULL, TO_ROOM);
            sprintf (buf, "You buy $p[%d] for %d silver.", number,
                     cost * number);
            act (buf, ch, obj, NULL, TO_CHAR);
        }
        else
        {
            act ("$n buys $p.", ch, obj, NULL, TO_ROOM);
            sprintf (buf, "You buy $p for %d silver.", cost);
            act (buf, ch, obj, NULL, TO_CHAR);
        }
        deduct_cost (ch, cost * number);
        keeper->gold += cost * number / 100;
        keeper->silver += cost * number - (cost * number / 100) * 100;

        for (count = 0; count < number; count++)
        {
            if (IS_SET (obj->extra_flags, ITEM_INVENTORY))
                t_obj = create_object (obj->pIndexData, obj->level);
            else
            {
                t_obj = obj;
                obj = obj->next_content;
                obj_from_char (t_obj);
            }

            if (t_obj->timer > 0 && !IS_OBJ_STAT (t_obj, ITEM_HAD_TIMER))
                t_obj->timer = 0;
            REMOVE_BIT (t_obj->extra_flags, ITEM_HAD_TIMER);
            obj_to_char (t_obj, ch);
            if (cost < t_obj->cost)
                t_obj->cost = cost;
        }
    }
}



void do_list (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (ch->in_room->store != NULL)
    {
       store_do_list(ch, argument);
       return;
    }

    if (IS_SET (ch->in_room->room_flags, ROOM_PET_SHOP))
    {
        ROOM_INDEX_DATA *pRoomIndexNext;
        CHAR_DATA *pet;
        bool found;

        /* hack to make new thalos pets work */
        if (ch->in_room->vnum == 9621)
            pRoomIndexNext = get_room_index (9706);
        else
            pRoomIndexNext = get_room_index (ch->in_room->vnum + 1);

        if (pRoomIndexNext == NULL)
        {
            bug ("Do_list: bad pet shop at vnum %d.", ch->in_room->vnum);
            send_to_char ("You can't do that here.\n\r", ch);
            return;
        }

        found = FALSE;
        for (pet = pRoomIndexNext->people; pet; pet = pet->next_in_room)
        {
            if (IS_SET (pet->act, ACT_PET))
            {
                if (!found)
                {
                    found = TRUE;
                    send_to_char ("Pets for sale:\n\r", ch);
                }
                sprintf (buf, "[%2d] %8d - %s\n\r",
                         pet->level,
                         10 * pet->level * pet->level, pet->short_descr);
                send_to_char (buf, ch);
            }
        }
        if (!found)
            send_to_char ("Sorry, we're out of pets right now.\n\r", ch);
        return;
    }
    else
    {
        CHAR_DATA *keeper;
        OBJ_DATA *obj;
        int cost, count;
        bool found;
        char arg[MAX_INPUT_LENGTH];

        if ((keeper = find_keeper (ch)) == NULL)
            return;
        one_argument (argument, arg);

        found = FALSE;
        for (obj = keeper->carrying; obj; obj = obj->next_content)
        {
            if (obj->wear_loc == WEAR_NONE && can_see_obj (ch, obj)
                && (cost = get_cost (keeper, obj, TRUE)) > 0
                && (arg[0] == '\0' || is_name (arg, obj->name)))
            {
                if (!found)
                {
                    found = TRUE;
                    send_to_char ("[Lv Price Qty] Item\n\r", ch);
                }

                if (IS_OBJ_STAT (obj, ITEM_INVENTORY))
                    sprintf (buf, "[%2d %5d -- ] %s\n\r",
                             obj->level, cost, obj->short_descr);
                else
                {
                    count = 1;

                    while (obj->next_content != NULL
                           && obj->pIndexData == obj->next_content->pIndexData
                           && !str_cmp (obj->short_descr,
                                        obj->next_content->short_descr))
                    {
                        obj = obj->next_content;
                        count++;
                    }
                    sprintf (buf, "[%2d %5d %2d ] %s\n\r",
                             obj->level, cost, count, obj->short_descr);
                }
                send_to_char (buf, ch);
            }
        }

        if (!found)
            send_to_char ("You can't buy anything here.\n\r", ch);
        return;
    }
}



void do_sell (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost, roll;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Sell what?\n\r", ch);
        return;
    }

    if ((keeper = find_keeper (ch)) == NULL)
        return;

    if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
    {
        act ("$n tells you 'You don't have that item'.",
             keeper, NULL, ch, TO_VICT);
        ch->oreply = keeper;
        return;
    }

    if (!can_drop_obj (ch, obj))
    {
        send_to_char ("You can't let go of it.\n\r", ch);
        return;
    }

    if (!can_see_obj (keeper, obj))
    {
        act ("$n doesn't see what you are offering.", keeper, NULL, ch,
             TO_VICT);
        return;
    }

    if ((cost = get_cost (keeper, obj, FALSE)) <= 0)
    {
        act ("$n looks uninterested in $p.", keeper, obj, ch, TO_VICT);
        return;
    }
    if (cost > (keeper->silver + 100 * keeper->gold))
    {
        act ("$n tells you 'I'm afraid I don't have enough wealth to buy $p.",
             keeper, obj, ch, TO_VICT);
        return;
    }

    act ("$n sells $p.", ch, obj, NULL, TO_ROOM);
    /* haggle */
    roll = number_percent ();
    if (!IS_OBJ_STAT (obj, ITEM_SELL_EXTRACT)
        && roll < get_skill (ch, gsn_haggle))
    {
        cost += obj->cost / 2 * roll / 100;
        cost = UMIN (cost, 95 * get_cost (keeper, obj, TRUE) / 100);
        cost = UMIN (cost, (keeper->silver + 100 * keeper->gold));
        if ((gold_weight(cost / 100) + silver_weight(cost - (cost / 100) * 100) + get_carry_weight(ch)) > can_carry_w(ch))
        {
          send_to_char("You can't carry that much gold, so you decide not to make the sell at this time.\n\r", ch);
          return;
        }  
        send_to_char ("You haggle with the shopkeeper.\n\r", ch);
        check_improve (ch, gsn_haggle, TRUE, 4);
    }
    if ((gold_weight(cost / 100) + silver_weight(cost - (cost / 100) * 100) + get_carry_weight(ch)) > can_carry_w(ch))
    {
      send_to_char("You can't carry that much gold, so you decide not to make the sell at this time.\n\r", ch);
      return;
    }  

    sprintf (buf, "You sell $p for %d silver and %d gold piece%s.",
             cost - (cost / 100) * 100, cost / 100, cost == 1 ? "" : "s");
    act (buf, ch, obj, NULL, TO_CHAR);
    ch->gold += cost / 100;
    ch->silver += cost - (cost / 100) * 100;
    deduct_cost (keeper, cost);
    if (keeper->gold < 0)
        keeper->gold = 0;
    if (keeper->silver < 0)
        keeper->silver = 0;

    if (obj->item_type == ITEM_TRASH || IS_OBJ_STAT (obj, ITEM_SELL_EXTRACT))
    {
        extract_obj (obj);
    }
    else
    {
        obj_from_char (obj);
        if (obj->timer)
            SET_BIT (obj->extra_flags, ITEM_HAD_TIMER);
        else
            obj->timer = number_range (50, 100);
        obj_to_keeper (obj, keeper);
    }

    return;
}



void do_value (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Value what?\n\r", ch);
        return;
    }

    if ((keeper = find_keeper (ch)) == NULL)
        return;

    if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
    {
        act ("$n tells you 'You don't have that item'.",
             keeper, NULL, ch, TO_VICT);
        ch->oreply = keeper;
        return;
    }

    if (!can_see_obj (keeper, obj))
    {
        act ("$n doesn't see what you are offering.", keeper, NULL, ch,
             TO_VICT);
        return;
    }

    if (!can_drop_obj (ch, obj))
    {
        send_to_char ("You can't let go of it.\n\r", ch);
        return;
    }

    if ((cost = get_cost (keeper, obj, FALSE)) <= 0)
    {
        act ("$n looks uninterested in $p.", keeper, obj, ch, TO_VICT);
        return;
    }

    sprintf (buf,
             "$n tells you 'I'll give you %d silver and %d gold coins for $p'.",
             cost - (cost / 100) * 100, cost / 100);
    act (buf, keeper, obj, ch, TO_VICT);
    ch->oreply = keeper;

    return;
}

void do_contactpoison (CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj;
    AFFECT_DATA af;

    /* find out what */
    if (ch->clan != clan_lookup("Furies"))
    {
    send_to_char("Huh?\n\r", ch);
    return;
    }
    if (argument[0] == '\0')
    {
        send_to_char ("Poison what item?\n\r", ch);
        return;
    }

    obj = get_obj_list (ch, argument, ch->carrying);

    if (obj == NULL)
    {
        send_to_char ("You don't have that item.\n\r", ch);
        return;
    }

    if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
    {
        if (IS_OBJ_STAT (obj, ITEM_BLESS)
            || IS_OBJ_STAT (obj, ITEM_BURN_PROOF))
        {
            act ("You fail to poison $p.", ch, obj, NULL, TO_CHAR);
            return;
        }

        if (ch->mana > 50)
        {                        /* success! */
            act ("$n imbues $p with deadly toxin.", ch, obj, NULL, TO_ROOM);
            act ("You imbues $p with deadly toxin.", ch, obj, NULL, TO_CHAR);
            if (!obj->value[3])
            {
                obj->value[3] = 1;
                            }
                       return;
        }

        act ("You fail to poison $p.", ch, obj, NULL, TO_CHAR);
        if (!obj->value[3])
        return;
    }

        if (IS_WEAPON_STAT (obj, WEAPON_CONTACT))
        {
            act ("$p is already poisoned.", ch, obj, NULL, TO_CHAR);
            return;
        }
else
        
        {

            af.where = TO_WEAPON;
            af.type = gsn_contact;
            af.level = (ch->level);
            af.duration = (ch->level / 3);
            af.location = 0;
            af.modifier = 0;
            af.bitvector = WEAPON_CONTACT;
            affect_to_obj (obj, &af);

            act ("$n coats $p with deadly venom.", ch, obj, NULL, TO_ROOM);
            act ("You coat $p with venom.", ch, obj, NULL, TO_CHAR);
            return;
        }
        
    

    
}

void do_tan( CHAR_DATA *ch, char*argument)
{
   static char *headers[]   = { "the corpse of the ", "the corpse of The ",
				"the corpse of an ", "the corpse of An ",
				"the corpse of a ", "the corpse of A ",
				"the corpse of " };
  OBJ_DATA *obj;
  char buf[MAX_STRING_LENGTH];
  int i;

  if (argument[0] == '\0')
    {
	send_to_char("Make a bag out of what corpse?\n\r", ch);
	return;
    }

  if (!(obj = get_obj_carry(ch, argument, ch)))
    {
	send_to_char("You don't have that corpse.\n\r", ch);
	return;
    }

  if ( obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
    {
	send_to_char("That's not a corpse.\n\r", ch);
	return;
    }

/*  if ( (chance = get_skill(ch,gsn_tan)) == 0)
  {
    send_to_char("Tanning what's that?\n\r", ch);
    return;
  }

  if (chance < number_percent())
  {
    act("You fail your attempt to tan the $p ruining the hide forever.", ch, obj, NULL, TO_CHAR);
    act("$n fails $s attempt to tan the $p ruining the hide forever.", ch, obj, NULL, TO_ROOM);
    extract_obj(obj);
    check_improve( ch, gsn_tan, FALSE, 1);
    return;
  }
*/
  for (i = 0; i < 7; i++)
    {
	int len = strlen(headers[i]);

	if (memcmp(obj->short_descr, headers[i], len) == 0 )
	  {
	  sprintf( buf, "bag" );
	  free_string( obj->name );
	  obj->name = str_dup(buf);

	  sprintf( buf, "A bag of fine %s hide catches your eye.  ",
	           obj->short_descr+len );
	  free_string( obj->description );
	  obj->description = str_dup( buf );
	
	  sprintf( buf, "bag made from %s hide", obj->short_descr+len );
	  free_string( obj->short_descr );
	  obj->short_descr = str_dup( buf );

	  break;
          }
    }

    obj->item_type = ITEM_CONTAINER;
    obj->wear_flags = ITEM_HOLD|ITEM_TAKE;
    obj->timer = 0;
    obj->weight = 5;
    obj->level = ch->level / 3;
    obj->cost = ch->level * 3;
    obj->value[0] = ch->level * 20;
    obj->value[1] = 1;
    obj->value[2] = -1;
    obj->value[3] = ch->level * 3;
    obj->pIndexData = get_obj_index(95);
    obj->condition = 100;

    act( "Your new $p looks pretty snazzy.", ch, obj, NULL, TO_CHAR);
    act( "$n's new $p looks pretty snazzy.", ch, obj, NULL, TO_ROOM);
//    check_improve( ch, gsn_tan, TRUE, 1);
    return;
}

void do_trap( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    int chance;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char(   "Syntax: trap set <object>\n\r"
                        "        trap disarm <object>\n\r", ch );
        return;
    }

    if ( !str_prefix( arg, "set" ) )
    {
        if ( ( chance = get_skill( ch, gsn_trapset ) ) <= 0 )
        {
            send_to_char( "You don't know how to set a trap.\n\r", ch );
            return;
        }

        else if ( is_affected( ch, gsn_trapset2 ) )
	{
	    send_to_char ( "You can't set another trap yet.\n\r", ch );
	    return;
	}

        for ( obj = object_list; obj != NULL; obj = obj->next )
        {
            if ( obj->item_type == ITEM_TRAP
            &&   obj->in_room != NULL 
            &&   !IS_SET( obj->wear_flags, ITEM_TAKE ) )
            {
                send_to_char( "You already have enough traps set.\n\r", ch );
                return;
            }
        }

        if ( ( obj = get_obj_carry( ch, argument, ch ) ) == NULL )
        {
            send_to_char( "You do not have that trap.\n\r", ch );
            return;
        }

        if ( obj->item_type != ITEM_TRAP )
        {
            act( "$p is not a trap.", ch, obj, NULL, TO_CHAR );
            return;
        }

        WAIT_STATE( ch, skill_table[gsn_trapset].beats );

        if ( number_percent( ) > chance )
        {
            send_to_char( "You failed to set your trap.\n\r", ch );
            act( "$n fumbles around with $p.",
                ch, obj, NULL, TO_ROOM );
            obj_from_char( obj );
            extract_obj( obj );
            check_improve( ch, gsn_trapset, FALSE, 3 );
        } else {
            REMOVE_BIT( obj->wear_flags, ITEM_TAKE );
            obj->timer = obj->value[4];
            obj_from_char( obj );
            obj_to_room( obj, ch->in_room );
	    ch->move -= 50;

          AFFECT_DATA af;
          affect_strip(ch, ch->timed_affect.sn);
          af.where     = TO_AFFECTS;
          af.type      = gsn_trapset2;
          af.duration  = number_range(2,4);
          af.level     = ch->level;
          af.location  = APPLY_NONE;
          affect_to_char( ch, &af );

            act( "You carefully place $p on the ground.",
                ch, obj, NULL, TO_CHAR );
            act( "$n carefully places $p on the ground.",
                ch, obj, NULL, TO_ROOM );
            check_improve( ch, gsn_trapset, TRUE, 3 );
        }
    }

    else if ( !str_prefix( arg, "disarm" ) )
    {
        if ( ( chance = get_skill( ch, gsn_trapdisarm ) ) <= 0 )
        {
            send_to_char( "You don't know how to disarm a trap.\n\r", ch );
            return;
        }

        if ( ( obj = get_obj_here( ch, NULL, argument ) ) == NULL )
        {
            send_to_char( "You do not see that trap here.\n\r", ch );
            return;
        }

        if ( obj->item_type != ITEM_TRAP )
        {
            act( "$p is not a trap.", ch, obj, NULL, TO_CHAR );
            return;
        }

        if ( IS_SET( obj->wear_flags, ITEM_TAKE ) )
        {
            act( "$p never seems to have been armed.", ch, obj, NULL, TO_CHAR );
            return;
        }

        WAIT_STATE( ch, skill_table[gsn_trapdisarm].beats );

        if ( number_percent( ) > chance )
        {
            send_to_char( "You failed to disarm your trap.\n\r", ch );
            act( "$n fumbles around with $p.",
                ch, obj, NULL, TO_ROOM );
            check_trap( ch, obj );
            check_improve( ch, gsn_trapdisarm, FALSE, 3 );
        } else {
            SET_BIT( obj->wear_flags, ITEM_TAKE );
            obj->timer = 0;
            obj_from_room( obj );
            obj_to_char( obj, ch );
            act( "You carefully grab $p from the ground.",
                ch, obj, NULL, TO_CHAR );
            act( "$n carefully grabs $p from the ground.",
                ch, obj, NULL, TO_ROOM );
            check_improve( ch, gsn_trapdisarm, TRUE, 3 );
        }
    }

    else
        do_trap( ch, "" );
}

