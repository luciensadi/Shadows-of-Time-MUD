/** Bomb Code by Asmodeus **/ 

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "bomb.h"
#include "tables.h"


/** Declarations **/
OBJ_DATA *get_ingredient  args((CHAR_DATA *ch, int bit));
OBJ_DATA *bomb_in_bag     args((OBJ_DATA *container));
char *flag_string               args ( ( const struct flag_type *flag_table,
                                         int bits ) );

/** Tables **/

const struct bomb_type bomb_table[] = {
/*   {"name", {INGREDIENTS}, 
     level, avedam, stability, trigger, explosion type
     "desc",
     "recipe information"}, */

     {"Pipe Bomb", {ING_SULFUR, ING_CHARCOL, ING_SALT_PETER, 0, 0}, 
      10, 220, 80, TRG_GET, EXP_EXPLOSION,
      "A pipe bomb lies here.",
      "A relatively easy bomb to make, that packs a small punch when grabbed."},

     {"Contact Explosive", {ING_SULFUR, ING_CHARCOL, ING_SALT_PETER, 0, 0}, 
      12, 280, 80, TRG_CONTACT, EXP_CONTACT,
      "A pipe bomb lies here.",
      "A relatively easy bomb to make, that can be thrown and explodes on contact."},

     {"Contact Smoke Explosive", {ING_SECRET_WATER, ING_SALT_PETER, ING_SUGAR, 0, 0}, 
      20, 220, 60, TRG_CONTACT, EXP_SMOKE,
      "You see a small pliable ball.",
      "A small charge that emits a cloud of black smoke."},

     {"Mail Bomb", {ING_SECRET_WATER, ING_MANEUR, ING_AMMONIA, 0, 0}, 
      25, 220, 60, TRG_OPEN, EXP_EXPLOSION,
      "A mail bomb waits to be stuck in a package here.", 
      "A small charge when put inside a container will surprise the next one to open it."},

     {"Exploding Mine", {ING_SECRET_WATER, ING_BLUBBER, ING_SULFUR, 0, 0}, 
      35, 400, 60, TRG_ENTER_ROOM, EXP_EXPLOSION,
      "You see a small needle sticking out of the ground.",
      "A decent charge when placed on the ground will explode upon anyone stepping on it."},

     {"Sleeper Mine", {ING_SECRET_WATER, ING_BLUBBER, ING_SULFUR, ING_SLEEPING_POWDER, 0}, 
      35, 180, 60, TRG_ENTER_ROOM, EXP_SLEEPER,
      "You see a small needle sticking out of the ground.",
      "A small charge that when set off puts out a cloud of sleeping powder."},

     {"Sticky Bomb", {ING_SECRET_WATER, ING_BLUBBER, ING_SULFUR, ING_OIL, 0}, 
      40, 180, 60, TRG_CONTACT, EXP_STICKY,
      "You large jar of a brown substance.",
      "A large jar of oil that explodes on contact covering the victim and leaving them almost immobile."},

     {"Shaped Explosive Mine", {ING_SECRET_WATER, ING_BLUBBER, ING_SULFUR, ING_SALT_PETER, 0}, 
      50, 750, 60, TRG_ENTER_ROOM, EXP_CONTACT,
      "You see a small needle sticking out of the ground.",
      "A small charge that deals a deadly blow to one target to due it's special shaping."},

     {"Shaped Explosive Pipe", {ING_SECRET_WATER, ING_BLUBBER, ING_SULFUR, ING_SALT_PETER, 0}, 
      52, 750, 60, TRG_ENTER_ROOM, EXP_CONTACT,
      "You see a small bit of goo stuffed in a pouch.",
      "A small charge that deals a deadly blow to one target due to it's special shaping."},

     {"Shaped Contact Explosive", {ING_SECRET_WATER, ING_BLUBBER, ING_SULFUR, ING_SALT_PETER, 0}, 
      55, 550, 60, TRG_CONTACT, EXP_CONTACT,
      "You see a small pliable ball.",
      "A small charge that when contacts an enemy deals a deadly blow to one target due to it's special shaping."},

     {"Poison Contact Explosive", {ING_SECRET_WATER, ING_BLUBBER, ING_SULFUR, ING_SALT_PETER, ING_POISON_POWDER}, 
      60, 300, 50, TRG_CONTACT, EXP_POISON,
      "You see a small pliable ball.",
      "A small charge that when contacts an emits a poisonous cloud."},

     {NULL, {0, 0, 0, 0, 0}, 0, 0, 0, 0, 0, NULL, NULL}
};


void do_bomb(CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];

  if (get_skill(ch, gsn_bomb) < 1)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
 

  argument = one_argument(argument, arg1);

  if (arg1[0] == '\0')	
  {
    send_to_char("Syntax is:\n\r", ch);
    send_to_char("Known              - List known bombs.\n\r", ch);
    send_to_char("Recipe <name>      - Print out bomb recipe.\n\r", ch);
    send_to_char("Make <name>        - Make bomb\n\r", ch);
    send_to_char("Set <name>         - Activate the bomb.\n\r", ch);
    return;
  }    	   

  if (!str_cmp(arg1, "known"))
  {
    bool found = FALSE;
    char buf[MSL];
    int i;


    send_to_char("\n\rBombs Known:\n\r", ch);
    send_to_char("Lvl    Name\n\r", ch);
 
    for (i = 0;bomb_table[i].name != NULL;i++)
    {
        if (bomb_table[i].level < ch->level)
        {
           sprintf(buf, "%2d - %s\n\r", bomb_table[i].level, bomb_table[i].name);
           send_to_char(buf, ch);
	   found = TRUE;
        }
     }
     if (!found)
       send_to_char("No Bombs Known.\n\r", ch);
     return;
   }      

   if (!str_prefix(arg1, "recipe"))
   {
      char arg2[MIL];
      bool found = FALSE;
      int i, j;
      char buf[MSL];      
    
      argument = one_argument(argument, arg2);
      for (i = 0;bomb_table[i].name != NULL;i++)
      {
        if (bomb_table[i].level < ch->level && 
            !str_prefix(bomb_table[i].name, arg2))
        {
	  found = TRUE;
          break;
        }
      }
 
      if (!found)
      {
	send_to_char("You don't know any such bomb.\n\r", ch);
        return;
      }

      sprintf(buf, "Recipe for %s\n\r\n\r", bomb_table[i].name);
      send_to_char(buf, ch);

      sprintf(buf, "Description: %s\n\r\n\rIngredients:\n\r", bomb_table[i].recipe);
      send_to_char(buf, ch);

      for (j = 0;j < MAX_INGREDIENT;j++)
      {
        if (bomb_table[i].bit[j] > 0)
        {
          sprintf(buf, "%d. %s\n\r", j, flag_string(ingredient_flags, bomb_table[i].bit[j]));
          send_to_char(buf, ch);
        }
      }      
      send_to_char("\n\r", ch);
   }             

   if (!str_cmp(arg1, "make"))
   {
     OBJ_DATA *ing1, *ing2, *ing3, *ing4, *ing5;
     OBJ_DATA *bomb;
     int bombNumber = 0;
     int i, ingNum;
     char arg2[MIL];       
     char buf[MSL];
     bool found = FALSE; 

     ing1 = ing2 = ing3 = ing4 = ing5 = NULL;

     argument = one_argument(argument, arg2);
     for (i = 0;bomb_table[i].name != NULL;i++)
     {
       if (bomb_table[i].level < ch->level && 
           !str_prefix(bomb_table[i].name, arg2))
       {
	  found = TRUE;
          bombNumber = i;
          break;
       }
     }
 
     if (!found)
     {
       send_to_char("You don't know any such bomb.\n\r", ch);
       return;
     }
     ingNum = 0;    

     for (i = 0;i < MAX_INGREDIENT;i++)
     {
       switch(i)
       {
           default: 
              send_to_char("Error report to Asmo.\n\r", ch);
              break;
           case 0:
              if (bomb_table[bombNumber].bit[i] > 0)
              {
                 ingNum++; 
                 if ((ing1 = get_ingredient(ch, bomb_table[bombNumber].bit[i])) == NULL)
                 {
                   sprintf(buf, "You don't have any %s\n\r", flag_string(ingredient_flags, bomb_table[bombNumber].bit[i]));
                   send_to_char(buf, ch);  
                   return;
                 }
              }
              break;
           case 1:
              if (bomb_table[bombNumber].bit[i] > 0)
              {
                 ingNum++; 
                 if ((ing2 = get_ingredient(ch, bomb_table[bombNumber].bit[i])) == NULL)
                 {
                   sprintf(buf, "You don't have any %s\n\r", flag_string(ingredient_flags, bomb_table[bombNumber].bit[i]));
                   send_to_char(buf, ch);
                   return;     
                 }
              }
              break;
           case 2:
              if (bomb_table[bombNumber].bit[i] > 0)
              {
                 ingNum++; 
                 if ((ing3 = get_ingredient(ch, bomb_table[bombNumber].bit[i])) == NULL)
                 {
                   sprintf(buf, "You don't have any %s\n\r", flag_string(ingredient_flags, bomb_table[bombNumber].bit[i]));
                   send_to_char(buf, ch);
                   return; 
                 }
              }
              break;
           case 3:
              if (bomb_table[bombNumber].bit[i] > 0)
              {
                 ingNum++; 
                 if ((ing4 = get_ingredient(ch, bomb_table[bombNumber].bit[i])) == NULL)
                 {
                   sprintf(buf, "You don't have any %s\n\r", flag_string(ingredient_flags, bomb_table[bombNumber].bit[i]));
                   send_to_char(buf, ch);
                   return; 
                 }
              }
              break;
           case 4:
              if (bomb_table[bombNumber].bit[i] > 0)
              {
                 ingNum++; 
                 if ((ing5 = get_ingredient(ch, bomb_table[bombNumber].bit[i])) == NULL)
                 {
                   sprintf(buf, "You don't have any %s\n\r", flag_string(ingredient_flags, bomb_table[bombNumber].bit[i]));
                   send_to_char(buf, ch);
                   return;
                 }
              }
              break;
       }              
     }

     for (i = 0;i < ingNum; i++)
     {
       switch (i)
       {
          default:
               send_to_char("Ack bug, talk to Asmo!\n\r", ch);
               return;
               break;
          case 0:
               extract_obj(ing1);
               break;
          case 1:
               extract_obj(ing2);
               break;
          case 2:
               extract_obj(ing3);
               break;
          case 3:
               extract_obj(ing4);
               break;
          case 4:
               extract_obj(ing5);
               break;
       }
     }
     
     bomb = create_object(get_obj_index(DUMMY_BOMB), ch->level);
     bomb->level = ch->level;
     bomb->weight = 20;
     bomb->value[0] = bomb_table[bombNumber].trigger;
     bomb->value[1] = bomb_table[bombNumber].explosive;
     bomb->value[2] = STATE_NOT_SET;
     bomb->value[3] = bomb_table[bombNumber].avedam + (ch->level * 3) + number_range(1, 25);
     free_string(bomb->name);
     bomb->name = str_dup(bomb_table[bombNumber].name);
     free_string(bomb->description);
     bomb->description = str_dup(bomb_table[bombNumber].description);
     free_string(bomb->short_descr);
     free_string(bomb->owner);
     bomb->owner = str_dup(ch->name);
     bomb->short_descr = str_dup(bomb_table[bombNumber].name);
     obj_to_char(bomb, ch);

     if ((number_percent() > bomb_table[bombNumber].stability) && 
         (number_percent() > 3 * get_skill(ch, gsn_bomb) / 4))
     {
	send_to_char("Oops!\n\r", ch);
       (*explosive_table[bomb->value[1]].bomb_fun) (bomb, ch);
       extract_obj(bomb);   
       check_improve(ch, gsn_bomb, FALSE, 1);
     }
     else
     {
       send_to_char("You made a bomb!\n\r", ch);
       check_improve(ch, gsn_bomb, TRUE, 1);
     }      
   }

   if (!str_cmp(arg1, "set"))
   {
     OBJ_DATA *bomb;
     char arg2[MIL];
     
     argument = one_argument(argument, arg2);
    
     if ((bomb = get_obj_carry(ch, arg2, ch)) == NULL)
     {
	send_to_char("You don't have that.\n\r", ch);
        return;
     }
  	
     if (bomb->item_type != ITEM_BOMB)
     {
       send_to_char("That's not a bomb.\n\r", ch);
       return;
     }
 
     bomb->value[2] = STATE_SET;
     send_to_char("Bomb set.\n\r", ch);
     return;
  }
    
 
}

OBJ_DATA *get_ingredient(CHAR_DATA *ch, int bit)
{  
  OBJ_DATA *obj;

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (obj->wear_loc == WEAR_NONE && (can_see_obj( ch, obj)) &&
        obj->item_type == ITEM_INGREDIENT && obj->value[0] == bit)
    { 
       return obj;
    }
  }
  return NULL;
}

OBJ_DATA *bomb_in_bag(OBJ_DATA *container)
{  
  OBJ_DATA *obj, *obj_next;

  for (obj = container->contains; obj != NULL; obj = obj_next)
  {
    obj_next = obj->next_content;
    if (obj->item_type == ITEM_BOMB && obj->value[0] == TRG_OPEN &&
        obj->value[2] == STATE_SET)
    { 
       return obj;
    }
  }
  return NULL;
}

void bomb_entry(CHAR_DATA *ch)
{
  OBJ_DATA *obj, *obj_next;
  ROOM_INDEX_DATA *location;
  location = ch->in_room;
  for (obj = location->contents; obj != NULL; obj = obj_next)
  {
    obj_next = obj->next;
    if (obj->item_type == ITEM_BOMB && obj->value[0] == TRG_ENTER_ROOM 
        && obj->value[2] == STATE_SET && obj->in_room!=NULL)
    {
      (*explosive_table[obj->value[1]].bomb_fun) (obj, ch);
      extract_obj(obj);
    }
  }
  return;
}

bool is_safe_bomb(OBJ_DATA *bomb, CHAR_DATA *victim)
{
  int leveldiff;
  if (!IS_NPC(victim))
  {
      if (IS_SET (victim->in_room->room_flags, ROOM_SAFE) && 
	  victim->pcdata->pk_timer <= 0)
         return TRUE;
      
      if (victim->level < 11)
        return TRUE;

      leveldiff = bomb->level - victim->level;
 
      if (leveldiff < 0) 
        return FALSE;
 /* Not working right     
      if (victim->level < 16 && leveldiff > 2)
        return TRUE; 

      if (victim->level < 26 && leveldiff > 3)
        return TRUE; 

      if (victim->level < 36 && leveldiff > 4)
        return TRUE; 

      if (victim->level < 46 && leveldiff > 5)
        return TRUE; 

      if (victim->level < 56 && leveldiff > 6)
        return TRUE; 

      if (victim->level < 66 && leveldiff > 7)
        return TRUE; 

      if (victim->level < 76 && leveldiff > 8)
        return TRUE; 
    */
   }
   return FALSE;
}


void sleeper_effect(CHAR_DATA *victim, int level)
{
   int sn;
   AFFECT_DATA af;

   if (IS_AFFECTED (victim, AFF_SLEEP)
     || (IS_NPC (victim) && IS_SET (victim->act, ACT_UNDEAD))
     || saves_spell (int(level * 1.8), victim, DAM_CHARM))
       return;

    sn = skill_lookup("Sleep");

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 1;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_SLEEP;
    affect_join (victim, &af);

    if (IS_AWAKE (victim))
    {
        send_to_char ("You feel very sleepy ..... zzzzzz.\n\r", victim);
        act ("$n goes to sleep.", victim, NULL, NULL, TO_ROOM);
        victim->position = POS_SLEEPING;
    }
    return;
}

void bomb_poison_effect(CHAR_DATA *victim, int level)
{
   int sn;
   AFFECT_DATA af;

   if ((IS_NPC (victim) && IS_SET (victim->act, ACT_UNDEAD))
     || saves_spell (int(level * 1.8), victim, DAM_POISON))
       return;

    sn = skill_lookup("Poison");

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_POISON;
    affect_join (victim, &af);

    send_to_char ("You feel very ill.\n\r", victim);
    act ("$n looks very ill.", victim, NULL, NULL, TO_ROOM);

    return;
}

void smoke_effect(CHAR_DATA *victim, int level)
{
   int sn;
   AFFECT_DATA af;

    if (IS_AFFECTED (victim, AFF_BLIND)
        || saves_spell (int(level * 1.8), victim, DAM_OTHER))
        return;

    sn = skill_lookup("Blind");

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    if (!IS_NPC(victim))
      af.duration = 1;
    else
      af.duration = level;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_BLIND;
    affect_join (victim, &af);

    send_to_char ("The smoke burns your eyes.\n\r", victim);
    act ("The smoke is impairing $n's vision.", victim, NULL, NULL, TO_ROOM);

    return;
}

void sticky_effect(CHAR_DATA *victim, int level)
{
   int sn;
   AFFECT_DATA af;

   if (IS_AFFECTED (victim, AFF_SLEEP)
     || (IS_NPC (victim) && IS_SET (victim->act, ACT_UNDEAD))
     || saves_spell (int(level * 1.8), victim, DAM_CHARM))
       return;

    sn = skill_lookup("Sticky");

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 4;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_STICKY;
    affect_to_char (victim, &af);

    if (IS_AWAKE (victim))
    {
        send_to_char ("You are covered in oil you can barely move.\n\r", victim);
        act ("$n can barely move.", victim, NULL, NULL, TO_ROOM);
    }
    return;
}

/** Explosions **/
         
void bomb_explosion(OBJ_DATA *bomb, CHAR_DATA *victim)
{
  CHAR_DATA *vch, *vch_next;
  ROOM_INDEX_DATA *room;

  act("$n sets off $p.", victim, bomb, victim, TO_ROOM);
  act("You set off $p.", victim, bomb, victim, TO_CHAR);

  room = victim->in_room;
  if (!is_safe_bomb(bomb, victim))
    obj_damage(bomb, victim, bomb->value[3], TYPE_EXPLOSION, DAM_BOMB, TRUE);   

  for (vch = room->people; vch != NULL; vch = vch_next)
  {
     vch_next = vch->next_in_room;
   
     if (is_safe_spell(victim, vch, TRUE) || (vch == victim) || is_safe_bomb(bomb, vch))
          continue;

     if (saves_spell(bomb->level, vch, DAM_BOMB))
     {
        obj_damage(bomb, vch, bomb->value[3] / 3, TYPE_EXPLOSION, DAM_BOMB, TRUE);
     }
     else
     {
        obj_damage(bomb, vch, bomb->value[3] / 2, TYPE_EXPLOSION, DAM_BOMB, TRUE);
     }
   }
}

void bomb_contact(OBJ_DATA *bomb, CHAR_DATA *victim)
{
  act("$p goes off as it hits $n.", victim, bomb, victim, TO_ROOM);
  act("$p explodes as it hits you.", victim, bomb, victim, TO_CHAR);

  if (!is_safe_bomb(bomb, victim))
  {
    if (!IS_NPC(victim))
       obj_damage(bomb, victim, bomb->value[3], TYPE_EXPLOSION, DAM_BOMB, TRUE);   
    else
       obj_damage(bomb, victim, 3 * bomb->value[3] / 2, TYPE_EXPLOSION, DAM_BOMB, TRUE);   
  } 
}

void bomb_poison(OBJ_DATA *bomb, CHAR_DATA *victim)
{

  act("$p goes off as it hits $n.", victim, bomb, victim, TO_ROOM);
  act("$p explodes as it hits you.", victim, bomb, victim, TO_CHAR);
  act("A cloud of poisonous gas emits from $p.", victim, bomb, NULL, TO_ROOM);
  if (!is_safe_bomb(bomb, victim))
    obj_damage(bomb, victim, bomb->value[3], TYPE_EXPLOSION, DAM_BOMB, TRUE); 
  bomb_poison_effect(victim, bomb->level); 

}

void bomb_smoke(OBJ_DATA *bomb, CHAR_DATA *victim)
{

  act("$p goes off as it hits $n.", victim, bomb, victim, TO_ROOM);
  act("$p explodes as it hits you.", victim, bomb, victim, TO_CHAR);
  act("A cloud of black gas emits from $p.", victim, bomb, NULL, TO_ROOM);
  if (!is_safe_bomb(bomb, victim))
    obj_damage(bomb, victim, bomb->value[3], TYPE_EXPLOSION, DAM_BOMB, TRUE); 
  smoke_effect(victim, bomb->level); 

}

void bomb_sleeper(OBJ_DATA *bomb, CHAR_DATA *victim)
{
  CHAR_DATA *vch, *vch_next;
  ROOM_INDEX_DATA *room;

  act("$n sets off $p.", victim, bomb, victim, TO_ROOM);
  act("You set off $p.", victim, bomb, victim, TO_CHAR);

  room = victim->in_room;
  if (!is_safe_bomb(bomb, victim))
  {
    obj_damage(bomb, victim, bomb->value[3], TYPE_EXPLOSION, DAM_BOMB, TRUE);   
    sleeper_effect(victim, bomb->level);
  }

  for (vch = room->people; vch != NULL; vch = vch_next)
  {
     vch_next = vch->next_in_room;
   
     if (is_safe_spell(victim, vch, TRUE) || (vch == victim) || is_safe_bomb(bomb, vch))
          continue;

     if (saves_spell(bomb->level, vch, DAM_BOMB))
     {
        obj_damage(bomb, vch, bomb->value[3] / 3, TYPE_EXPLOSION, DAM_BOMB, TRUE);
     }
     else
     {
        obj_damage(bomb, vch, bomb->value[3] / 2, TYPE_EXPLOSION, DAM_BOMB, TRUE);
     }
 
     sleeper_effect(vch, bomb->level);

   }
}

void bomb_sticky(OBJ_DATA *bomb, CHAR_DATA *victim)
{
  CHAR_DATA *vch, *vch_next;
  ROOM_INDEX_DATA *room;

  act("$n sets off $p.", victim, bomb, victim, TO_ROOM);
  act("You set off $p.", victim, bomb, victim, TO_CHAR);

  room = victim->in_room;
  if (!is_safe_bomb(bomb, victim))
  {
    obj_damage(bomb, victim, bomb->value[3], TYPE_EXPLOSION, DAM_BOMB, TRUE);   
    sticky_effect(victim, bomb->level);
  }

  for (vch = room->people; vch != NULL; vch = vch_next)
  {
     vch_next = vch->next_in_room;
   
     if (is_safe_spell(victim, vch, TRUE) || (vch == victim) || is_safe_bomb(bomb, vch))
          continue;

     if (!IS_NULLSTR(bomb->owner) && !str_cmp(bomb->owner, vch->name) && number_percent() > 50)
     {
       continue;
     }

     if (saves_spell(bomb->level, vch, DAM_BOMB))
     {
        obj_damage(bomb, vch, bomb->value[3] / 3, TYPE_EXPLOSION, DAM_BOMB, TRUE);
     }
     else
     {
        obj_damage(bomb, vch, bomb->value[3] / 2, TYPE_EXPLOSION, DAM_BOMB, TRUE);
     }
 
     sticky_effect(vch, bomb->level);

   }
}

void bomb_null(OBJ_DATA *bomb, CHAR_DATA *victim)
{
  return;
}
