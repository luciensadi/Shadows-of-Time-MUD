/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St`1feldt, Tom Madsen, and Katja Nyboe.   *
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
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "tournament.h"
#include "olc.h"
#include "web.h"
#include "mysql.h"

int find_direction	args( ( char *argument ) );
// char *com_main (char *argument, char *name);
void deinitialize(void);

char *const where_name[] = {
    "`&<`^U`$sed as light`&>`*     ",
    "`&<`^W`$orn on finger`&>`*    ",
    "`&<`^W`$orn on finger`&>`*    ",
    "`&<`^W`$orn around neck`&>`*  ",
    "`&<`^W`$orn around neck`&>`*  ",
    "`&<`^W`$orn on torso`&>`*     ",
    "`&<`^W`$orn on head`&>`*      ",
    "`&<`^W`$orn on legs`&>`*      ",
    "`&<`^W`$orn on feet`&>`*      ",
    "`&<`^W`$orn on hands`&>`*     ",
    "`&<`^W`$orn on arms`&>`*      ",
    "`&<`^W`$orn as shield`&>`*    ",
    "`&<`^W`$orn about body`&>`*   ",
    "`&<`^W`$orn about waist`&>`*  ",
    "`&<`^W`$orn on belt`&>`*      ",
    "`&<`^W`$orn around wrist`&>`* ",
    "`&<`^W`$orn around wrist`&>`* ",
    "`&<`^W`$ielded`&>`*           ",
    "`&<`^S`$econd`&>`*            ",
    "`&<`^H`$eld`&>`*              ",
    "`&<`^F`$loating nearby`&>`*   ",
    "`&<`^W`$orn on face`&>`*      ",
    "`&<`^W`$orn on shoulder`&>`*  ",
};


/* for  keeping track of the player count */
int max_on = 0;

/*
 * Local functions.
 */
char *format_obj_to_char args ((OBJ_DATA * obj, CHAR_DATA * ch, bool fShort));
void show_list_to_char args ((OBJ_DATA * list, CHAR_DATA * ch,
                              bool fShort, bool fShowNothing, sh_int type));
void show_char_to_char_0 args ((CHAR_DATA * victim, CHAR_DATA * ch));
void show_char_to_char_1 args ((CHAR_DATA * victim, CHAR_DATA * ch));
void show_char_to_char args ((CHAR_DATA * list, CHAR_DATA * ch));
bool check_blind args ((CHAR_DATA * ch));
void show_room_blind( CHAR_DATA *ch );
void port_exits args ((CHAR_DATA *ch, ROOM_INDEX_DATA *to_room));
void look_window( CHAR_DATA *ch, OBJ_DATA *obj ); //Window code by Kimber Boh
char *show_trank args ((CHAR_DATA *ch));


void do_top_ten(CHAR_DATA *ch, char *argument)
{
  MYSQL_RES *res;
  MYSQL_ROW row;
  char buf[MSL];

  mysql_query(mysql, "SELECT name, explored FROM top_explored ORDER BY explored DESC");
  res = mysql_store_result(mysql);
  send_to_char("Top 10 explorers:\n\r", ch);
  send_to_char("Name        % Explored\n\r", ch);
  while ((row = mysql_fetch_row(res)))
  {
    sprintf(buf, "%-12s  %3.2f%%\n\r", row[0], (((atoi(row[1]) * 1.0) / top_room) * 100.0));
    send_to_char(buf, ch);
  }
  mysql_free_result(res);

  send_to_char("\n\r", ch);
  mysql_query(mysql, "SELECT name, mobbed FROM top_mobbed ORDER BY mobbed DESC");
  res = mysql_store_result(mysql);
  send_to_char("Top 10 mobbers:\n\r", ch);
  send_to_char("Name        % Mobbed\n\r", ch);
  while ((row = mysql_fetch_row(res)))
  {
    sprintf(buf, "%-12s  %3.2f%%\n\r", row[0], (((atoi(row[1]) * 1.0) / top_mob_index) * 100.0));
    send_to_char(buf, ch);
  }
  mysql_free_result(res);
}
/*
void do_diem(CHAR_DATA *ch, char *argument)
{
  int i;
  char buf [MSL];

  if (!is_number(argument))
 {
    if (number_range(1,4) = 1)
    i = number_range(1, 300);
    if (number_range(1, 4) = 2)
    i = number_range(301, 600);
    if (number_range(1, 4) = 3)
    i = number_range(601, 900);
    if (number_range(1, 4) = 4)
    i = number_range(901, 1025)
  else
 {
  i = atoi(argument);
  if (i > quotester.top || i <= 0)
  {
   sprintf(buf, "Valid numbers are 0 through %d\n\r", quotester.top);
   send_to_char(buf, ch);
   return;
  }
 }

  sprintf(buf, "`&QUO`!T`&ES`!T`&ER:`!#%d`# %s", i, quotester.text[i]);
  do_function(ch, &do_quote, buf);
  WAIT_STATE(ch, 18);
}
*/
void do_cover(CHAR_DATA *ch, char *argument)
{
  if IS_NPC(ch)
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_SET (ch->act2, PLR_NOEQ))
  {
    send_to_char("What you wear will now be visible to others.\n\r", ch);
    REMOVE_BIT(ch->act2, PLR_NOEQ);
    return;
  }

  send_to_char("What you wear is no longer visible to others.\n\r", ch);
  SET_BIT(ch->act2, PLR_NOEQ);

  return;
}
void do_bofh(CHAR_DATA *ch, char *argument)
{
  int i;
  char buf[MSL];

  if (!is_number(argument))
    i = number_range(0, fortune.top);
  else
  {
    i = atoi(argument);
    if (i > fortune.top || i <= 0)
    {
      sprintf(buf, "Valid numbers are 0 through %d\n\r", fortune.top);
      send_to_char(buf, ch);
      return;
    }
  }

  sprintf(buf, "`&BOFH `!%d:`# %s", i, fortune.text[i]);
  do_function(ch, &do_quote, buf);
  WAIT_STATE(ch, 18);
}

void print_time_since(CHAR_DATA *ch, long difference)
{
  char buf[MSL];
  char sDay[MSL];
  char sHour[MSL];
  char sMinute[MSL];
  char sSecond[MSL];

  int days = difference / (60 * 60 * 24);
  difference %= (60 * 60 * 24);
  int hours = difference / (60 * 60);
  difference %= (60 * 60);
  int minutes = difference / 60;
  int seconds = difference % 60;

  sprintf(sDay, "%d day%s, ", days, days > 1 ? "s" : "");
  sprintf(sHour, "%d hour%s, ", hours, hours > 1 ? "s" : "");
  sprintf(sMinute, "%d minute%s, ", minutes, minutes > 1 ? "s" : "");
  sprintf(sSecond, "%d second%s.", seconds, seconds == 1 ? "" : "s");

  sprintf(buf, "%s%s%s%s\n\r", days > 0 ? sDay : "", hours > 0 ? sHour : "", minutes > 0 ? sMinute : "", sSecond );
  send_to_char(buf, ch);
}


/*
void alice(CHAR_DATA *ch, CHAR_DATA *mob, char *argument)
{
  char response[MSL];
  if (IS_NULLSTR(argument) || argument == NULL)
    return;
  sprintf(response, "%s", com_main(argument, ch->name));
  do_function(mob, &do_say, response);
  deinitialize();
}
*/

bool check_max(OBJ_DATA *obj, int stat, int modifier)
{
  int previous = 0;
  int wearlevel;
  int max;
  AFFECT_DATA *paf;

  if (obj->affected)
  {
    for (paf = obj->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == stat)
      {
        previous += paf->modifier;
      }
    }
  }
  else
  {
    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == stat)
      {
        previous += paf->modifier;
      }
    }
  }

  wearlevel = UMIN( (((obj->level -5) * 5) / 7), MAX_LEVEL - 5);
  if (wearlevel < 1)
    wearlevel = 1;

  if (stat == APPLY_HIT || stat == APPLY_MANA)
  {
    max  = int(wearlevel * 1.333);
    if (max > 98)
      max = 100;
  }
  else if (stat == APPLY_MOVE)
  {
    max  = int(wearlevel * 1.666);
    if (max > 123)
      max = 100;
  }
  else
  {
    if (wearlevel < 5)
      max = 1;
    else if (wearlevel < 10)
      max = 2;
    else if (wearlevel < 15)
      max = 3;
    else if (wearlevel < 25)
      max = 4;
    else if (wearlevel < 35)
      max = 5;
    else if (wearlevel < 45)
      max = 6;
    else if (wearlevel < 55)
      max = 7;
    else if (wearlevel < 65)
      max = 8;
    else if (wearlevel < 75)
      max = 9;
    else
      max = 10;
  }

  logf ("Pre %d Mod %d WearLvl Max %d", previous, modifier, wearlevel, max);
  if (abs(previous + modifier) > max)
    return TRUE;
  return FALSE;
}

/* scan code */
/* for scanning and do_scanning */
char * const dist_name [] =
{
  "right here",
  "close by", "not far off", "a brief walk away",
  "rather far off", "in the distance", "almost out of sight"
};

char * const dir_desc [] =
{
  "to the north", "to the east", "to the south", "to the west",
  "upwards", "downwards"
};

/*
 * Scan command.
 * (by Turtle 12-Jun-94)
 */
void scanning( CHAR_DATA *ch, int door )
{
    int distance, visibility;
    char buf[MAX_STRING_LENGTH];
    bool found;
    ROOM_INDEX_DATA *was_in_room;

    visibility = 6;
    if( IS_SET( ch->act, PLR_HOLYLIGHT ) )
    {
      	switch( weather_info.sunlight )
	{
	    case SUN_SET:   visibility = 4; break;
	    case SUN_DARK:  visibility = 2; break;
	    case SUN_RISE:  visibility = 4; break;
	    case SUN_LIGHT: visibility = 6; break;
	}
        switch( weather_info.sky )
	{
	    case SKY_CLOUDLESS: break;
	    case SKY_CLOUDY:    visibility -= 1; break;
	    case SKY_RAINING:   visibility -= 2; break;
	    case SKY_LIGHTNING: visibility -= 3; break;
	}
    }

    if ( IS_IMMORTAL( ch ) )
     	visibility = 6;

    was_in_room = ch->in_room;
    found = FALSE;
    for( distance = 1; distance <= 6; distance++ )
    {
      	EXIT_DATA *pexit;
      	CHAR_DATA *list;
      	CHAR_DATA *rch;

	if( ( pexit = ch->in_room->exit[door] ) != NULL
	&& pexit->u1.to_room != NULL
	&& pexit->u1.to_room != was_in_room )
	{
       	    if ( !can_see_room( ch, pexit->u1.to_room ) )
		continue;

	    /* If the door is closed, stop looking... */
	    if( IS_SET( pexit->exit_info, EX_CLOSED ) )
	    {
	      	char door_name[80];

	      	one_argument( pexit->keyword, door_name );
	      	if ( door_name[0] == '\0' )
		    strcat( door_name, "door" );
	        sprintf( buf, "A closed %s %s %s.\n\r",
		    door_name, dist_name[distance-1], dir_desc[door] );
	      	send_to_char( buf, ch );
	      	found = TRUE;
	        break;
	    }

	    ch->in_room = pexit->u1.to_room;
	    if( IS_OUTSIDE(ch) ? distance > visibility : distance > 4 )
		break;

	    list = ch->in_room->people;
	    for( rch = list; rch != NULL; rch = rch->next_in_room )
	    	if( can_see( ch, rch ) )
	      	{
                  if (!IS_NPC(ch) && ch->pcdata->pk_timer > 0 && IS_NPC(rch))
                  {
                     found = FALSE;
                     continue;
                  }
		    found = TRUE;
		    sprintf( buf, "%s%s is %s %s.\n\r",
			PERS( rch, ch, FALSE ),
			IS_NPC(rch) ? "" : IS_DISGUISED(rch)?"":" (PLAYER)",
			dist_name[distance],
			dir_desc[door] );
		    send_to_char( buf, ch );
	      	}
	}
    }

    ch->in_room = was_in_room;
    if ( !found )
    {
	sprintf( buf, "You can't see anything %s.\n\r",
	    dir_desc[door] );
      	send_to_char( buf, ch );
    }
    return;
}


void do_scanning( CHAR_DATA *ch, char *argument )
{
    int dir=0;
    bool found;
    EXIT_DATA *pexit;
    char buf[MAX_STRING_LENGTH];

    if( !check_blind( ch ) )
    	return;

    if ( argument[0] == '\0' )
    {
      	act( "$n scans intensely all around.", ch, NULL, NULL, TO_ROOM );
      	found = FALSE;
	pexit = NULL;
      	for( dir = 0; dir <= 5; dir++ )
	{
    	    if ( ( pexit   = ch->in_room->exit[dir] ) == NULL
    	    ||	 !can_see_room(ch,pexit->u1.to_room ) )
	 	continue;

	   sprintf( buf, "\n\r**** %s ****\n\r",dir_desc[dir]);
 	   send_to_char( buf, ch);
	   scanning( ch, dir );
	   found = TRUE;
	}

  	if ( !found )
	    send_to_char( "There are no exits here!\n\r", ch );
    }
    else
    {
    	if ( ( dir = find_direction( argument ) ) == -1 )
    	{
	    send_to_char( "That's not a direction!\n\r", ch );
	    return;
    	}

	act( "$n scans intensly $T.", ch, NULL, dir_desc[dir], TO_ROOM );
    	if ( ( pexit   = ch->in_room->exit[dir] ) == NULL
    	||     !can_see_room(ch,pexit->u1.to_room ) )
	{
	    send_to_char( "There is no exit in that direction!\n\r", ch );
	    return;
	}

	sprintf(buf, "\n\r**** %s ****\n\r",dir_desc[dir]);
	send_to_char(buf, ch);
	scanning( ch, dir );
    }
    return;
}

void do_penchant( CHAR_DATA *ch, char *argument )
{
  char arg1[MIL];
  char arg2[MIL];
  OBJ_DATA *obj;
  bool found = FALSE;
  int i, num;
  OBJ_DATA *coupon1 = NULL;
  OBJ_DATA *coupon2 = NULL;
  OBJ_DATA *coupon3 = NULL;
  OBJ_DATA *coupon4 = NULL;
  OBJ_DATA *coupon5 = NULL;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0')
  {
    send_to_char("Syntax is enchant <item> <type>\n\r", ch);
    send_to_char("Type enchant type for a list of types\n\r", ch);
    return;
  }

  if (!str_cmp(arg1, "type"))
  {
    char buf[MSL];

    send_to_char("Keyword    Description   Coupons Needed.\n\r", ch);
    for (i = 0; i < MAX_ENCHANT;i++)
    {
      sprintf(buf, "%-12s %-15s %2d\n\r",
	    enchant_table[i].keyword,
	    enchant_table[i].desc,
	    enchant_table[i].coupons);
      send_to_char(buf, ch);
    }
  }

  if (arg2[0] == '\0')
  {
    send_to_char("Syntax is enchant <item> <type>\n\r", ch);
    send_to_char("Type enchant type for a list of types\n\r", ch);
    return;
  }

  for (i = 0; i < MAX_ENCHANT;i++)
  {
    if (!str_cmp(enchant_table[i].keyword, arg2))
    {
      found = TRUE;
      break;
    }
  }

  num = i;
  if (!found)
  {
    send_to_char("That's not a valid keyword type penchant type for a list.\n\r", ch);
    return;
  }

  if ((obj = get_obj_carry(ch, arg1, ch)) == NULL)
  {
    send_to_char("You aren't carrying that.\n\r", ch);
    return;
  }

  if (obj->enchants + enchant_table[num].coupons > 5)
  {
    send_to_char("That item can't hold any more enchants.\n\r", ch);
    return;
  }

  if (check_max(obj, enchant_table[num].apply, enchant_table[num].modifier))
  {
    send_to_char("That would exceed the maximum allowed on that stat.\n\r", ch);
    return;
  }

  for (i = 0; i < enchant_table[num].coupons; i++)
  {
    switch(i)
    {
      case 0:
	 if ((coupon1 = get_obj_carry(ch, "enchant coupon", ch)) == NULL ||
		coupon1->pIndexData->vnum != 6402)
	 {
           send_to_char("You don't have the necessary coupons or one of them is illegal.\n\r", ch);
           return;
         }
         obj_from_char(coupon1);
         if (num == i + 1)
         {
           logf("%s turned in %d coupons for %s", ch->name, num, obj->short_descr);
           extract_obj(coupon1);
         }
         break;
      case 1:
	 if ((coupon2 = get_obj_carry(ch, "enchant coupon", ch)) == NULL ||
		coupon2->pIndexData->vnum != 6402)
	 {
           send_to_char("You don't have the necessary coupons or one of them is illegal.\n\r", ch);
           obj_to_char(coupon1, ch);
           return;
         }
         obj_from_char(coupon2);
         if (num == i + 1)
         {
           logf("%s turned in %d coupons for %s", ch->name, num, obj->short_descr);
           extract_obj(coupon1);
           extract_obj(coupon2);
         }
         break;
      case 2:
	 if ((coupon3 = get_obj_carry(ch, "enchant coupon", ch)) == NULL ||
		coupon3->pIndexData->vnum != 6402)
	 {
           send_to_char("You don't have the necessary coupons or one of them is illegal.\n\r", ch);
           obj_to_char(coupon1, ch);
           obj_to_char(coupon2, ch);
           return;
         }
         obj_from_char(coupon3);
         if (num == i + 1)
         {
           logf("%s turned in %d coupons for %s", ch->name, num, obj->short_descr);
           extract_obj(coupon1);
           extract_obj(coupon2);
           extract_obj(coupon3);
         }
         break;
      case 3:
	 if ((coupon4 = get_obj_carry(ch, "enchant coupon", ch)) == NULL ||
		coupon4->pIndexData->vnum != 6402)
	 {
           send_to_char("You don't have the necessary coupons or one of them is illegal.\n\r", ch);
           obj_to_char(coupon1, ch);
           obj_to_char(coupon2, ch);
           obj_to_char(coupon3, ch);
           return;
         }
         obj_from_char(coupon4);
         if (num == i + 1)
         {
           logf("%s turned in %d coupons for %s", ch->name, num, obj->short_descr);
           extract_obj(coupon1);
           extract_obj(coupon2);
           extract_obj(coupon3);
           extract_obj(coupon4);
         }
         break;
      case 4:
	 if ((coupon5 = get_obj_carry(ch, "enchant coupon", ch)) == NULL ||
		coupon5->pIndexData->vnum != 6402)
	 {
           send_to_char("You don't have the necessary coupons or one of them is illegal.\n\r", ch);
           obj_to_char(coupon1, ch);
           obj_to_char(coupon2, ch);
           obj_to_char(coupon3, ch);
           obj_to_char(coupon4, ch);
           return;
         }
         obj_from_char(coupon5);
         if (num == i + 1)
         {
           extract_obj(coupon1);
           extract_obj(coupon2);
           extract_obj(coupon3);
           extract_obj(coupon4);
           extract_obj(coupon5);
         }
         break;
    }
  }

  if (enchant_table[num].type == ENCH_AFFECT)
  {
     AFFECT_DATA paf;

     affect_enchant(obj);
     paf.where	= TO_AFFECTS;
     paf.type	= 0;
     paf.level	= 555;
     paf.duration = -1;
     paf.location = enchant_table[num].apply;
     paf.modifier  = enchant_table[num].modifier;
     paf.bitvector = 0;
     affect_to_obj(obj, &paf);

     if (enchant_table[num].apply == APPLY_HITROLL)
     {
       affect_enchant(obj);
       paf.where	= TO_AFFECTS;
       paf.type	= 0;
       paf.level	= 555;
       paf.duration = -1;
       paf.location = APPLY_DAMROLL;
       paf.modifier  = enchant_table[num].modifier;
       paf.bitvector = 0;
       affect_to_obj(obj, &paf);
     }
     obj->enchants += enchant_table[num].coupons;
     send_to_char("Enchant added.\n\r", ch);
  }

}
void help_parse (char *point, char *buf)
{
  char last = '\0';
  while (*point != '\0')
  {
    if (*point != '$' || last == '`')
    {
      *buf = *point;
      last = *point;
      buf++;
      point++;
      continue;
    }

    last = *point;
    point++;
    switch(*point)
    {
      case 'c':
	    {
	      char data[MIL];
	      char cClass[MIL];
	      char num[1];
	      int sn, clss;
	      int cp;
              point++;
	      point = one_argument_special(point, data);
	      point = one_argument_special(point, cClass);
	      sn = skill_lookup(data);
	      clss = class_lookup(cClass);
	      cp = skill_table[sn].rating[clss];
	      if (cp > 9)
	      {
                sprintf(num, "%d", cp / 10);
                *buf++ = num[0];
	      }
              sprintf(num, "%d", cp % 10);
              *buf++ = num[0];
              logf("%s - %d %s - %d %d", data, sn, cClass, clss, cp);
	      break;
	    }

      case 'g':
	    {
	      char data[MIL];
	      char cClass[MIL];
	      char num[1];
	      int gn, clss;
	      int cp;
              point++;
	      point = one_argument_special(point, data);
	      point = one_argument_special(point, cClass);
	      gn = group_lookup(data);
	      clss = class_lookup(cClass);
	      cp = group_table[gn].rating[clss];
	      if (cp > 9)
	      {
                sprintf(num, "%d", cp / 10);
                *buf++ = num[0];
	      }
              sprintf(num, "%d", cp % 10);
              *buf++ = num[0];
              logf("%s - %d %s - %d %d", data, gn, cClass, clss, cp);
	      break;
	    }

      case 'l':
	    {
	      char data[MIL];
	      char cClass[MIL];
	      char num[1];
	      int sn, clss;
	      int cp;
              point++;
	      point = one_argument_special(point, data);
	      point = one_argument_special(point, cClass);
	      sn = skill_lookup(data);
	      clss = class_lookup(cClass);
	      cp = skill_table[sn].skill_level[clss];
	      if (cp > 9)
	      {
                sprintf(num, "%d", cp / 10);
                *buf++ = num[0];
	      }
              sprintf(num, "%d", cp % 10);
              *buf++ = num[0];
              logf("%s - %d %s - %d %d", data, sn, cClass, clss, cp);
	      break;
	    }
    }

  }
  *buf = '\0';
  return;
}

void do_testasmo(CHAR_DATA *ch, char*argument)
{
  char temp[MSL];
  char buf[MSL];
  HELP_DATA *pHelp;
  bool found = FALSE;

  for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
  {
    if (is_name(argument, pHelp->keyword))
    {
      found = TRUE;
      help_parse(pHelp->text, temp);
    }
  }
  if (!found)
    return;
  sprintf(buf, "Output:\n\r %s", temp);
  send_to_char(buf, ch);
}

void do_elements (CHAR_DATA * ch, char *argument)
{
  send_to_char("\e[1z <!ELEMENT RName FLAG=\"RoomName\">", ch);
  send_to_char("\e[1z <!ELEMENT RDesc FLAG=\"RoomDesc\">", ch);
  send_to_char("\e[1z <!ELEMENT RExits FLAG=\"RoomExit\">", ch);
  send_to_char("\e[1z <!ELEMENT Hp FLAG=\"Set hp\">", ch);
  send_to_char("\e[1z <!ELEMENT MaxHp FLAG=\"Set maxhp\">", ch);
  send_to_char("\e[1z <!ELEMENT Mana FLAG=\"Set mana\">", ch);
  send_to_char("\e[1z <!ELEMENT MaxMana FLAG=\"Set maxmana\">", ch);
  send_to_char("\e[1z <!ELEMENT EnmyHp '<FONT COLOR=Black>' FLAG=\"Set enmyhp\">", ch);
  send_to_char("\e[1z <!ELEMENT Ticks FLAG=\"Set ticks\">", ch);
  send_to_char("\e[1z <!ELEMENT Prompt FLAG=\"prompt\">", ch);
  if (IS_IMMORTAL(ch))
    send_to_char("\e[1z <Script>#ST Hp @Hp Mana @Mana Ticks @Ticks  Enemy Hp @enmyhp</Script>\n", ch);
  else
    send_to_char("\e[1z <Script>#ST Hp @Hp Mana @Mana Ticks @Ticks</Script>\n", ch);
  if (str_cmp(argument, "auto"))
    send_to_char("Done.\n\r", ch);
}


void show_obj_stat (CHAR_DATA * ch, OBJ_DATA *obj)
{
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;

    sprintf (buf,
             "Object '%s' is type %s, extra flags %s.\n\rWeight is %d, value is %d, level is %d.\n\r",
             obj->name,
             item_name (obj->item_type),
             extra_bit_name (obj->extra_flags),
             obj->weight / 10, obj->cost, obj->level);
    send_to_char (buf, ch);
    sprintf (buf, "Class restrictions: %s\n\r", class_bit_name(obj->class_flags));
    send_to_char( buf, ch);

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
            sprintf (buf, "Has %d charges of level %d",
                     obj->value[2], obj->value[0]);
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

        case ITEM_WEAPON:
            send_to_char ("Weapon type is ", ch);
            switch (obj->value[0])
            {
                case (WEAPON_EXOTIC):
                    send_to_char ("exotic.\n\r", ch);
                    break;
                case (WEAPON_SWORD):
                    send_to_char ("sword.\n\r", ch);
                    break;
                case (WEAPON_DAGGER):
                    send_to_char ("dagger.\n\r", ch);
                    break;
                case (WEAPON_SPEAR):
                    send_to_char ("spear\n\r", ch);
                    break;
                case (WEAPON_MACE):
                    send_to_char ("mace/club.\n\r", ch);
                    break;
                case (WEAPON_AXE):
                    send_to_char ("axe.\n\r", ch);
                    break;
                case (WEAPON_FLAIL):
                    send_to_char ("flail.\n\r", ch);
                    break;
                case (WEAPON_WHIP):
                    send_to_char ("whip.\n\r", ch);
                    break;
                case (WEAPON_POLEARM):
                    send_to_char ("polearm.\n\r", ch);
                    break;
                case (WEAPON_BOW):
                    send_to_char ("bow.\n\r", ch);
                    break;
                case (WEAPON_STAFF):
                    send_to_char ("staff.\n\r", ch);
                    break;
                default:
                    send_to_char ("unknown.\n\r", ch);
                    break;
            }
            if (obj->pIndexData->new_format)
                sprintf (buf, "Damage is %dd%d (average %d).\n\r",
                         obj->value[1], obj->value[2],
                         (1 + obj->value[2]) * obj->value[1] / 2);
            else
                sprintf (buf, "Damage is %d to %d (average %d).\n\r",
                         obj->value[1], obj->value[2],
                         (obj->value[1] + obj->value[2]) / 2);
            send_to_char (buf, ch);
            if (obj->value[4])
            {                    /* weapon flags */
                sprintf (buf, "Weapons flags: %s\n\r",
                         weapon_bit_name (obj->value[4]));
                send_to_char (buf, ch);
            }
            break;

        case ITEM_ARMOR:
	    sprintf (buf, "Wear: %s\n\r", wear_bit_name(obj->wear_flags - 1));
	    send_to_char(buf, ch);
            sprintf (buf,
                     "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.\n\r",
                     obj->value[0], obj->value[1], obj->value[2],
                     obj->value[3]);
            send_to_char (buf, ch);
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

    if (!obj->enchanted)
        for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
        {
            if (paf->location != APPLY_NONE && paf->modifier != 0)
            {
                sprintf (buf, "Affects %s by %d.\n\r",
                         affect_loc_name (paf->location), paf->modifier);
                send_to_char (buf, ch);
                if (paf->bitvector)
                {
                    switch (paf->where)
                    {
                        case TO_AFFECTS:
                            sprintf (buf, "Adds %s affect.\n",
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
        }

    for (paf = obj->affected; paf != NULL; paf = paf->next)
    {
        if (paf->location != APPLY_NONE && paf->modifier != 0)
        {
            sprintf (buf, "Affects %s by %d",
                     affect_loc_name (paf->location), paf->modifier);
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
                    case TO_OBJECT:
                        sprintf (buf, "Adds %s object flag.\n",
                                 extra_bit_name (paf->bitvector));
                        break;
                    case TO_WEAPON:
                        sprintf (buf, "Adds %s weapon flags.\n",
                                 weapon_bit_name (paf->bitvector));
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
    }

    return;
}

void do_mudstats(CHAR_DATA *ch, char *argument)
{
  char buf[MSL];

  send_to_char("`4Mud Statistics since `&10/01/2006`*\n\r\n\r", ch);
  sprintf(buf, "%-25s %3s %5s\n\r", " ", "Total", "Today");
  send_to_char(buf, ch);
  sprintf(buf, "%-25s %8d  %4d\n\r", "`$Max Players`&", mudstats.total_max_players,
                             mudstats.current_max_players);
  send_to_char(buf, ch);
  sprintf(buf, "%-25s %8ld  %4d\n\r", "`$Logins`&", mudstats.total_logins,
                             mudstats.current_logins);
  send_to_char(buf, ch);
  sprintf(buf, "%-25s %8ld  %4d\n\r", "`$Newbies`&", mudstats.total_newbies,
                             mudstats.current_newbies);
  send_to_char(buf, ch);
  sprintf(buf, "%-25s %8d  %4d\n\r", "`$Legends`&", mudstats.total_heros,
                             mudstats.current_heros);
  send_to_char(buf, ch);
  send_to_char("\n\r\n\r", ch);
  sprintf(buf, "%-25s %14ld\n\r", "`$Areas`&", top_area);
  send_to_char(buf, ch);
  sprintf(buf, "%-25s %14ld\n\r", "`$Rooms`&", top_room);
  send_to_char(buf, ch);
  sprintf(buf, "%-25s %14ld\n\r", "`$Mobs`&", top_mob_index);
  send_to_char(buf, ch);
  sprintf(buf, "%-25s %14ld\n\r", "`$Objects`&", top_obj_index);
  send_to_char(buf, ch);
  sprintf(buf, "%-25s %14d\n\r", "`$Player Run Stores`&", mudstats.player_shops);
  send_to_char(buf, ch);
  if (IS_IMMORTAL(ch))
  {
    sprintf(buf, "`*Current players `&%d`*\n\r", mudstats.current_players);
    send_to_char(buf, ch);
  }


  if (!str_cmp("clear", argument) && IS_IMMORTAL(ch) && FALSE) // disabled
  {
    mudstats.current_max_players = 0;
    mudstats.current_logins = 0;
    mudstats.current_newbies = 0;
    mudstats.current_heros = 0;
    mudstats.total_max_players = 0;
    mudstats.total_logins = 0;
    mudstats.total_newbies = 0;
    mudstats.total_heros = 0;
    mudstats.player_shops = 0;
    send_to_char("done.\n\r", ch);
  }
}

void do_pkon(CHAR_DATA *ch, char *argument)
{
  char buf[MIL];

  if (ch->pcdata->confirm_pkon)
  {
    if (argument[0] != '\0')
    {
      send_to_char("No PK status unchanged.\n\r", ch);
      ch->pcdata->confirm_pkon = FALSE;
      return;
    }
    else
    {
      REMOVE_BIT(ch->act, PLR_NOPK);
      send_to_char("No PK status removed you can now be pked.\n\r", ch);
      ch->pcdata->confirm_pkon = FALSE;
      sprintf(buf, "%s turned on PK.", ch->name);
      logf(buf, 0);
      wiznet(buf, ch, NULL, 0, 0, get_trust(ch));
      return;
    }
  }

  if (argument[0] != '\0')
  {
    send_to_char("Just type pkon.  No argument.\n\r", ch);
    return;
  }

  send_to_char("Type pkon again to confirm removal of this and entry into pk.\n\r", ch);
  send_to_char("This is irreversible.  Typing nopk with an argument will undo\n\r", ch);
  send_to_char("this command.\n\r", ch);
  ch->pcdata->confirm_pkon = TRUE;
}

/*
void do_font(CHAR_DATA *ch, char *argument)
{
  char buf[MSL];
  char arg1[MIL];

  argument = one_argument(argument, arg1);

  if (arg1[0] == '\0')
  {
    sprintf(buf, "RP Note font %s\n\r", ch->pcdata->fonts.rpnote);
    send_to_char(buf, ch);
    sprintf(buf, "Other Note font %s\n\r", ch->pcdata->fonts.notes);
    send_to_char(buf, ch);
  }

  if (!str_cmp(arg1, "rpnote"))
  {
    if (argument[0] == '\0')
    {
      send_to_char("Set the RP font to what?\n\r", ch);
      return;
    }
    free_string(ch->pcdata->fonts.rpnote);
    ch->pcdata->fonts.rpnote = str_dup(argument);
    send_to_char("Rp note font changed.\n\r", ch);
  }

  if (!str_cmp(arg1, "notes"))
  {
    if (argument[0] == '\0')
    {
      send_to_char("Set the note font to what?\n\r", ch);
      return;
    }
    free_string(ch->pcdata->fonts.notes);
    ch->pcdata->fonts.notes = str_dup(argument);
    send_to_char("Note font changed.\n\r", ch);
  }
}
*/


void do_remember( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg1[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg1 );

  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act, PLR_QUESTING) || IS_SET(ch->act, PLR_TOURNEY))
  {
    send_to_char("Why don't you wait till you're less busy to make an introduction.\n\r", ch);
    return;
  }

  if ( (arg1[0] == '\0')  || (argument[0] == '\0') )
  {
     send_to_char("Remember who as what?\n\r", ch);
     return;
  }

  if ( (victim = get_char_world(ch, arg1)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  addKnow( ch, victim->id, argument);
  sprintf( buf, "You will now remember %s as %s.\n\r",
	       ( victim->sex == SEX_FEMALE ) ? "her" :
               ( victim->sex == SEX_MALE ) ? "him" : "it",
		argument );
  send_to_char( buf, ch );
  return;
}

void do_forget( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  struct idName *name, *name_next;
  bool found = FALSE;

  if (IS_NPC(ch))
    return;

  if ( (argument[0] == '\0') )
  {
     send_to_char("Forget who.\n\r", ch);
     return;
  }

  if (ch->pcdata->names == NULL)
  {
    send_to_char("You don't know them.\n\r", ch);
    return;
  }

  if (!IS_NULLSTR(ch->pcdata->names->name) && !str_cmp(argument, ch->pcdata->names->name))
  {
    name = ch->pcdata->names;
    ch->pcdata->names = name->next;
    freeName(name);
    sprintf(buf, "%s forgotten.\n\r", argument);
    send_to_char(buf, ch);
    return;
  }

  for (name = ch->pcdata->names; name != NULL; name = name_next)
  {
    name_next = name->next;
    if (name->next && !str_cmp(argument, name->next->name))
    {
      struct idName *temp;
      temp = name->next;
      name->next = temp->next;
      freeName(temp);
      found = TRUE;
      break;
    }
  }

  if (found)
  {
    sprintf(buf, "%s forgotten.\n\r", argument);
    send_to_char(buf, ch);
    return;
  }

  send_to_char("You don't know them.\n\r", ch);
  return;
}

void addKnow( CHAR_DATA *ch, long id, char *name )
{
  struct idName *cName;

  for ( cName = ch->pcdata->names; cName; cName = cName->next )
  {
    if ( cName->id == id )
      break;
  }

  if (!cName )
  {
     cName = newName();
     cName->name = str_dup( capitalize( name ) );
     cName->id = id;
     cName->next = ch->pcdata->names;
     ch->pcdata->names = cName;
     return;
  }

  free_string( cName->name );
  cName->name = str_dup( capitalize( name ));
}

char *indefinite( char *str )
{
  char first;
  first = tolower( str[0] );
  switch( first )
  {
    case 'a' :
    case 'e' :
    case 'i' :
    case 'o' :
    case 'u' :
       return "An";
    default :
      return "A";
  }
  return "a";
}

char *known_name(CHAR_DATA *ch, CHAR_DATA *looker)
{
  struct idName *name;

  if (!IS_NPC(looker))
  {
    if (looker->level >= LEVEL_IMMORTAL ||
	ch->level >= LEVEL_IMMORTAL)
    {
      return ch->name;
    }

    for ( name = looker->pcdata->names; name; name = name->next )
    {
       if ( name->id == ch->id )
       {
         return name->name;
       }
    }
    return NULL;
  }
  return ch->name;
}

char *PERS( CHAR_DATA *ch, CHAR_DATA *looker, bool ooc)
{
  static char tmp[MSL];

  if ( !can_see(looker, ch) && (!IS_AFFECTED(ch, AFF_SHROUD)) && IS_IMMORTAL(ch))
    return "`!A`1n `!I`1m`1m`!o`!r`1t`!a`1l`7";

  if ( !can_see(looker, ch) && (!IS_AFFECTED(ch, AFF_SHROUD)) )
    return "Someone";

  if ( IS_NPC( ch ) )
    return ch->short_descr;

  if ( IS_IMMORTAL( looker ) ||
       ch == looker )
  {
    return IS_DISGUISED(ch) ? ch->pcdata->disguise.orig_name : ch->name;
  }

  if ( IS_AFFECTED(ch, AFF_VEIL) && !furies_imm (ch,looker) && ch->clan != looker->clan)
  {
      return "A veiled figure";
  }

  if ( IS_AFFECTED(ch, AFF_SHROUD) && !furies_imm (ch,looker) && (ch->clan != looker->clan ||
       (ch->clan == looker->clan && !IS_GUILDMASTER(looker))))
  {
    return "A `8shrouded`* figure";
  }

  if ( can_see(looker, ch) )
  {
    if (IS_SET(ch->act, PLR_QUESTING) && IS_SET(looker->act, PLR_QUESTING))
      return ch->name;

    if (IS_SET(ch->act, PLR_TOURNEY) && IS_SET(looker->act, PLR_TOURNEY))
      return ch->name;

    if (IS_DISGUISED(ch))
      return ch->short_descr;

    if (ooc)
      return ch->name;

    if (ch->level >= LEVEL_IMMORTAL)
      return ch->name;


/*shroud test boobie*/
//    if (looker->clan == clan_lookup("Shadow"))
//      return ch->name;

    if (!IS_NPC(looker) && known_name(ch, looker))
      return known_name(ch, looker);

    // Introduction code
    {
      sprintf( tmp, "%s %s %s",
              ( get_age(ch) < 21 ) ? "A young" :
              ( get_age(ch) < 35 ) ? "A mature" :
              ( get_age(ch) < 50 ) ? "A middle-aged" :
	      ( get_age(ch) < 75 ) ? "An old" :
              ( get_age(ch) < 100 ) ? "A venerable" :
              ( get_age(ch) < 150 ) ? "An ancient" :
                "An antediluvian",
              race_table[ch->race].name,
              ( ch->sex == SEX_FEMALE ) ? "woman" :
              ( ch->sex == SEX_MALE ) ? "man" : "person");
      return tmp;
    }
  }
  return "Someone";
}

char *GET_CHAR_PERS( CHAR_DATA *ch, CHAR_DATA *looker, bool ooc)
{
  static char tmp[MSL];
  struct idName *name;

  if ( !can_see(looker, ch) && (!IS_AFFECTED(ch, AFF_SHROUD)) )
    return "Someone";


  if ( IS_NPC( ch ) )
    return ch->short_descr;

  if ( IS_IMMORTAL( looker ) ||
       ch == looker )
  {
    return ch->name;
  }

  if ( can_see(looker, ch) )
  {
    if (IS_SET(ch->act, PLR_QUESTING) && IS_SET(looker->act, PLR_QUESTING))
    {
      return ch->name;
    }

    if (IS_SET(ch->act, PLR_TOURNEY) && IS_SET(looker->act, PLR_TOURNEY))
      return ch->name;

    if (ooc)
      return ch->name;

    if (ch->level >= LEVEL_IMMORTAL || ch->clan == clan_lookup("Guide"))
      return ch->name;

    if (looker->clan == clan_lookup("Guide"))
      return ch->name;

    if (IS_NPC(looker))
      return ch->name;

    if (!IS_NPC(looker))
    {
      for ( name = looker->pcdata->names; name; name = name->next )
      {
         if ( name->id == ch->id )
         {
           return name->name;
         }
      }
    }

    // Introduction code
    {
      sprintf( tmp, "%s %s %s %s",
              "A",
              ( get_age(ch) < 35 ) ? "young" :
              ( get_age(ch) < 70 ) ? "middle-aged" :
                "old",
              race_table[ch->race].name,
              ( ch->sex == SEX_FEMALE ) ? "woman" :
              ( ch->sex == SEX_MALE ) ? "man" : "");
      return tmp;
    }
  }
  return "Someone";
}

void do_gameinfo (CHAR_DATA *ch, char * argument)
{
  char buf[MSL];
  int currentcounter;

  currentcounter = armorcounter;
  sprintf(buf, "`&%4d `4Mob kills until `$Armor`4 Bonus!\n\r", currentcounter);
  send_to_char(buf,ch);
  currentcounter = 0;
  currentcounter = shieldcounter;
  sprintf(buf, "`&%4d `4Mob kills until `$Shield`4 Bonus!\n\r", currentcounter);
  send_to_char(buf,ch);
  currentcounter = 0;
  currentcounter = blesscounter;
  sprintf(buf, "`&%4d `4Mob kills until `$Bless`4 Bonus!\n\r", currentcounter);
  send_to_char(buf,ch);
  currentcounter = 0;
  currentcounter = sanccounter;
  sprintf(buf, "`&%4d `4Mob kills until `$Sanctuary`4 Bonus!\n\r", currentcounter);
  send_to_char(buf,ch);
  currentcounter = 0;
  currentcounter = restorecounter;
  sprintf(buf, "`&%4d `4Mob kills until `$Restore`4 Bonus!\n\r", currentcounter);
  send_to_char(buf,ch);
  currentcounter = 0;
  currentcounter = bonuscounter;
  sprintf(buf, "`&%4d `4Mob kills until `$Bonus`4 Time!\n\r", currentcounter);
  send_to_char(buf,ch);
  currentcounter = 0;
  currentcounter = questcounter;
  sprintf(buf, "`&%4d `4Mob kills until `$Quest`4 Reset!\n\r", currentcounter);
  send_to_char(buf,ch);

  return;
}

char *format_obj_to_char (OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
        || (obj->description == NULL || obj->description[0] == '\0'))
        return buf;

    if (obj->condition == 0)
        strcat (buf, "(`3Broken`*) ");
    if (obj->condition < 0)
        strcat (buf, "(`3Destroyed`*) ");
    if (IS_OBJ_STAT(obj, ITEM_QUEST))
        strcat (buf, "`7(`&Quest`7) ");
    if (IS_OBJ_STAT (obj, ITEM_INVIS))
        strcat (buf, "(`&Invis`*) ");
    if (IS_AFFECTED (ch, AFF_DETECT_EVIL) && IS_OBJ_STAT (obj, ITEM_EVIL))
        strcat (buf, "`&(`!R`1ed Aura`&)`* ");
    if (IS_AFFECTED (ch, AFF_DETECT_GOOD) && IS_OBJ_STAT (obj, ITEM_BLESS))
        strcat (buf, "`&(`$B`4lue Aura`&)`* ");
    if ((IS_AFFECTED (ch, AFF_DETECT_MAGIC))  && (IS_OBJ_STAT (obj, ITEM_QUEST)) )
        strcat (buf, "`&(`%M`5agical`&)`* ");
/*
    if (IS_OBJ_STAT (obj, ITEM_GLOW))
        strcat (buf, "`&(`@G`2lowing`&)`* ");
    if (IS_OBJ_STAT (obj, ITEM_HUM))
        strcat (buf, "`&(`@H`2umming`&)`* ");
*/
    if (IS_OBJ_STAT (obj, ITEM_BURNING))
        strcat (buf, "`!(`#Burning`!)`* ");

    if (fShort)
    {
        if (obj->short_descr != NULL)
            strcat (buf, obj->short_descr);
    }
    else
    {
        if (obj->description != NULL)
            strcat (buf, obj->description);
    }
    return buf;
}

void add_vote (char *host, long id, int vote, char *reason)
{
  VOTERS_DATA *temp;

  if IS_NULLSTR(host)
  {
    logf("Bugged host", 0);
    return;
  }

  temp = new_voter();
  temp->id = id;
  temp->vote = vote;
  temp->host = str_dup(host);
  temp->reason = reason[0] != '\0' ? str_dup(reason) : str_dup("None");
  temp->next = voteinfo.voters;
  voteinfo.voters = temp;
}

void change_vote (char *host, long id, int vote, char *reason)
{
  VOTERS_DATA *vch;

  if IS_NULLSTR(host)
  {
    logf("Bugged host", 0);
    return;
  }

  for ( vch = voteinfo.voters ; vch != NULL; vch = vch->next)
  {
    if ( vch->id == id )
    {
      vch->vote = vote;
      vch->host = str_dup(host);
      vch->reason = reason[0] != '\0' ? str_dup(reason) : str_dup("None");
    }
  }
}

void do_mxp( CHAR_DATA *ch, char *argument )
{
  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act2, PLR_MXP))
  {
    send_to_char("MXP Turned off.\n\r", ch);
    REMOVE_BIT(ch->act2, PLR_MXP);
    return;
  }

  SET_BIT(ch->act2, PLR_MXP);
  send_to_char("MXP turned on.\n\r", ch);
  send_to_char("\e[1z<VERSION>", ch);
}

void do_enhanced( CHAR_DATA *ch, char *argument)
{
  char buf [MSL];
  CHAR_DATA *victim;

  if (!IS_IMMORTAL(ch) || argument[0] == '\0')
    victim = ch;
  else if ((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("Not on NPCs.\n\r", ch);
    return;
  }

  send_to_char("Enhanced Features\n\r\n\r", ch);
  send_to_char("Compression:   ", ch);
  if (victim->desc->out_compress)
    send_to_char("ON\n\r", ch);
  else
    send_to_char("OFF\n\r", ch);
  send_to_char("MXP:           ", ch);
  if (IS_SET(victim->act2, PLR_MXP))
    send_to_char("ON\n\r", ch);
  else
    send_to_char("OFF\n\r", ch);
  send_to_char("MSP Sound:     ", ch);
  if (IS_SET(victim->act2, PLR_MSP_SOUND))
    send_to_char("ON\n\r", ch);
  else
    send_to_char("OFF\n\r", ch);
  send_to_char("MSP Music:     ", ch);
  if (IS_SET(victim->act2, PLR_MSP_MUSIC))
    send_to_char("ON\n\r", ch);
  else
    send_to_char("OFF\n\r", ch);

//  if (!IS_SET(ch->enh, ENH_MXP))
  {
    send_to_char("\n\rClient Info:\n\r", ch);
    sprintf(buf, "MXP Version: %s\n\r",
      victim->pcdata->mxpVersion[0] == '\0' ? "Not Detected" : victim->pcdata->mxpVersion);
    send_to_char(buf, ch);
    sprintf(buf, "Client: %s\n\r",
      victim->pcdata->client[0] == '\0' ? "Not Detected" : victim->pcdata->client);
    send_to_char(buf, ch);
    sprintf(buf, "Client Version: %s\n\r",
      victim->pcdata->clientVersion[0] == '\0' ? "Not Detected" : victim->pcdata->clientVersion);
    send_to_char(buf, ch);
  }

}


int check_vote(CHAR_DATA *ch)
{
  VOTERS_DATA *vch;

  for ( vch = voteinfo.voters ; vch != NULL; vch = vch->next)
  {
    if ( vch->id == ch->id )
    {
      return vch->vote;
    }

    if (!str_cmp(vch->host, dns_gethostname(ch->desc->Host)))
    {
      return -2;
    }

  }
  return -1;
}

void do_vote (CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  char buf[MSL];

  argument = one_argument(argument, arg1);

  if (arg1[0] == '\0')
  {
    send_to_char("Syntax is vote show\n\r", ch);
    send_to_char("          vote for\n\r", ch);
    send_to_char("          vote against\n\r", ch);
    return;
  }


  if (!str_cmp(arg1, "show"))
  {
    float total = voteinfo.forit + voteinfo.against;
    float forper, agper;
    forper = voteinfo.forit / UMAX(1, total) * 100;
    agper = voteinfo.against / UMAX(1, total) * 100;

    total = voteinfo.forit + voteinfo.against;
send_to_char("`4-----------------------------------------------------------------------`*\n\r", ch);
    send_to_char("`4|                    `&The current vote is on:                          `4|`* \n\r", ch);
send_to_char("`4-----------------------------------------------------------------------`*\n\r", ch);
    send_to_char("`*\n\r", ch);
    sprintf(buf, "`^%s\n\r", voteinfo.question);
    send_to_char(buf, ch);
    send_to_char("`4|---------------------------------------------------------------------|`*\n\r", ch);
    send_to_char("`4|               `$For:                 `4|            `$Against             `4|`*\n\r", ch);
    sprintf(buf, "`4|                `&%2d                  `4|              `&%2d                `4|`*\n\r",
                   voteinfo.forit, voteinfo.against);
    if (IS_IMMORTAL(ch))
      send_to_char(buf, ch);
    sprintf(buf, "`4|               `^%3.0f%%                 `4|             `^%3.0f%%               `4|`*\n\r",
                   forper, agper);
    send_to_char(buf, ch);
    send_to_char("`4-----------------------------------------------------------------------`*\n\r", ch);
    if (!str_cmp(argument, "full") && IS_IMMORTAL(ch))
    {
      VOTERS_DATA *vch;
      BUFFER *forit;
      BUFFER *against;
      char tmp[MSL];
      int i = 1;
      int j = 1;

      forit = new_buf();
      against = new_buf();
      for (vch = voteinfo.voters; vch != NULL; vch = vch->next)
      {
        if (vch->vote == 0)
        {
           sprintf(tmp, "%d - %s\n\r", i, vch->reason);
           i++;
           add_buf(forit, tmp);
        }
        else
        {
           sprintf(tmp, "%d - %s\n\r", j, vch->reason);
           j++;
           add_buf(against, tmp);
        }
      }
      send_to_char("For reasons: \n\r\n\r", ch);
      page_to_char(buf_string(forit), ch);
      send_to_char("Against reasons: \n\r\n\r", ch);
      page_to_char(buf_string(against), ch);
      free_buf(forit);
      free_buf(against);
    }

    return;
  }

  if (check_vote(ch) == -2)
  {
     send_to_char("Someone else has already voted from that host.\n\r", ch);
     return;
  }

  if (!str_cmp(arg1, "for"))
  {
    if (!voteinfo.on)
    {
      send_to_char("There is currently no vote running.\n\r", ch);
      return;
    }

    if (check_vote(ch) == 0)
    {
      send_to_char("That's already your vote.\n\r", ch);
      return;
    }

    if (check_vote(ch) == 1)
    {
      send_to_char("You have changed your vote to for it.\n\r", ch);
      voteinfo.forit += 1;
      voteinfo.against -= 1;
      change_vote(dns_gethostname(ch->desc->Host), ch->id, 0, argument);
      return;
    }

    send_to_char("You have voted for it.\n\r", ch);
    voteinfo.forit += 1;
    add_vote(dns_gethostname(ch->desc->Host), ch->id, 0, argument);
    return;
  }

  if (!str_cmp(arg1, "against"))
  {
    if (!voteinfo.on)
    {
      send_to_char("There is currently no vote running.\n\r", ch);
      return;
    }

    if (check_vote(ch) == 1)
    {
      send_to_char("That's already your vote.\n\r", ch);
      return;
    }

    if (check_vote(ch) == 0)
    {
      send_to_char("You have changed your vote to against it.\n\r", ch);
      voteinfo.forit -= 1;
      voteinfo.against += 1;
      change_vote(dns_gethostname(ch->desc->Host), ch->id, 1, argument);
      return;
    }

    send_to_char("You have voted against it.\n\r", ch);
    voteinfo.against += 1;
    add_vote(dns_gethostname(ch->desc->Host), ch->id, 1, argument);
    return;
  }
}

/*
void do_guildlist (CHAR_DATA *ch, char *argument)
{
  int i;
  char buf[MAX_STRING_LENGTH];
  char *name;
  MEMBER_DATA *mbr;

  send_to_char("Guilds:\n\r\n\r", ch);

  for (i = 1;i < top_clan;i++)
  {
 	 for (mbr = clan_table[ch->clan].roster.member;mbr != NULL ; mbr = mbr->next )
  	 {
		 mbr = clan_table[i].roster.member;

             if (ch->rank >= clan_table[ch->clan].top_rank - 2)
             {
                 name = mbr->name;

   		 sprintf(buf, "Ldr: %s   `&%2d`7. %s", mbr->name, i,
               		   clan_table[i].who_name);
   		 send_to_char(buf, ch);
   		 send_to_char("\n\r",ch);
	     }
         }
   break;
  }
  return;
}
*/

void do_guildlist (CHAR_DATA *ch, char *argument)
{
  do_function (ch, &do_mhelp, "* 'guildlist'");

  return;
}


void do_warstatus (CHAR_DATA *ch, char *argument)
{
  do_function (ch, &do_mhelp, "* 'warstatus'");

  return;
}

/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char (OBJ_DATA * list, CHAR_DATA * ch, bool fShort,
                        bool fShowNothing, sh_int type)
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    char **prgpstrShow;
    char **prgpstrName;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if (ch->desc == NULL)
        return;

    /*
     * Alloc space for output lines.
     */
    output = new_buf ();

    count = 0;
    for (obj = list; obj != NULL; obj = obj->next_content)
        count++;
    prgpstrShow = (char **) alloc_mem(count * sizeof (char *));
    prgpstrName = (char **) alloc_mem (count * sizeof (char *));
    prgnShow = (int *) alloc_mem (count * sizeof (int));
    nShow = 0;

    /*
     * Format the list of objects.
     */
    for (obj = list; obj != NULL; obj = obj->next_content)
    {
        if (obj->wear_loc == WEAR_NONE && can_see_obj (ch, obj))
        {
            pstrShow = format_obj_to_char (obj, ch, fShort);

            fCombine = FALSE;

            if (IS_NPC (ch) || IS_SET (ch->comm, COMM_COMBINE))
            {
                /*
                 * Look for duplicates, case sensitive.
                 * Matches tend to be near end so run loop backwords.
                 */
                for (iShow = nShow - 1; iShow >= 0; iShow--)
                {
                    if (!strcmp (prgpstrShow[iShow], pstrShow))
                    {
                        prgnShow[iShow]++;
                        fCombine = TRUE;
                        break;
                    }
                }
            }

            /*
             * Couldn't combine, or didn't want to.
             */
            if (!fCombine)
            {
                char temp[MIL], temp2[MIL];
                prgpstrShow [nShow] = str_dup( pstrShow );
                prgnShow    [nShow] = 1;
                strcpy(temp, obj->name);
                first_arg(temp, temp2, FALSE);
                prgpstrName [nShow] = str_dup(temp2);
                nShow++;
            }
        }
    }

    /*
     * Output the formatted list.
     */
    for (iShow = 0; iShow < nShow; iShow++)
    {
        if (prgpstrShow[iShow][0] == '\0')
        {
            free_string (prgpstrShow[iShow]);
            continue;
        }

        if (IS_SET(ch->act2, PLR_MXP))
        {
           add_buf(output, "\e[1z");
        }

        if (IS_NPC (ch) || IS_SET (ch->comm, COMM_COMBINE))
        {
            if (prgnShow[iShow] != 1)
            {
                sprintf (buf, "(%2d) ", prgnShow[iShow]);
                add_buf (output, buf);
            }
            else
            {
                add_buf (output, "     ");
            }
        }

      if (IS_SET(ch->act2, PLR_MXP))
      {
        switch (type)
	{
          char eil[MSL];

          default:
	    add_buf(output,prgpstrShow[iShow]);
            break;
          case ON_GROUND:
            sprintf( eil, "<send \"look %s|get %s|donate %s|do\" hint=\" look %s|get %s|donate %s\">%s</send>",
		            prgpstrName[iShow], prgpstrName[iShow],
                            prgpstrName[iShow],
                            prgpstrName[iShow],
                            prgpstrName[iShow],
                            prgpstrName[iShow],
                            prgpstrShow[iShow]);
            add_buf(output, eil);
            break;
          case ON_INVENTORY:
            sprintf( eil, "<send \"look %s|drop %s|wear %s|do\" hint=\" look %s|drop %s|wear %s\">%s</send>",
		            prgpstrName[iShow], prgpstrName[iShow],
                            prgpstrName[iShow],
                            prgpstrName[iShow],
                            prgpstrName[iShow],
                            prgpstrName[iShow],
                            prgpstrShow[iShow]);
            add_buf(output, eil);
            break;
          case ON_INCONTAINER:
            sprintf( eil, "<send \"get %s|do\" hint=\"click|get %s\">%s</send>",
		            prgpstrName[iShow], prgpstrName[iShow],
                            prgpstrShow[iShow]);
            add_buf(output, eil);
            break;
	  }
        }
        else
	{
          add_buf (output, prgpstrShow[iShow]);
	}

        add_buf (output, "\n\r");
        free_string (prgpstrShow[iShow]);
        free_string (prgpstrName[iShow]);
    }

    if (fShowNothing && nShow == 0)
    {
        if (IS_NPC (ch) || IS_SET (ch->comm, COMM_COMBINE))
            send_to_char ("     ", ch);
        send_to_char ("Nothing.\n\r", ch);
    }
    page_to_char (buf_string (output), ch);

    /*
     * Clean up.
     */
    free_buf (output);
    free_mem (prgpstrShow, count * sizeof (char *));
    free_mem (prgpstrName, count * sizeof (char *));
    free_mem (prgnShow, count * sizeof (int));

    return;
}



void show_char_to_char_0 (CHAR_DATA * victim, CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH], message[MAX_STRING_LENGTH];
    char buf2[MSL];

    buf[0] = '\0';
    if (IS_NPC(victim) && ch->questmob > 0
    && victim->pIndexData->vnum == ch->questmob
    && !str_cmp("quest", victim->name)
    && (ch->rescuemob == 0
    || (victim->fighting
    && victim->fighting->attacker == victim)))
        strcat( buf, "[TARGET] ");

    if (IS_NPC(victim) && ch->questmob > 0 && victim->pIndexData->vnum == ch->questmob 
        && (ch->rescuemob == 0 || (victim->fighting && victim->fighting->attacker == victim)))
        strcat( buf, "[TARGET] ");
    if (victim && !IS_NPC(ch))
      if (!IS_NPC(victim) && victim->desc == NULL)
        strcat( buf, "`8(`&LINK-DEAD`8) `*");
    if (!IS_NPC(victim) && victim->is_mounted)
        strcat( buf, "`8(`3Riding`8)`* ");
    if (IS_NPC(victim) && victim->is_mounted)
        strcat( buf, "`8(`3Ridden`8)`* ");
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_TOURNEY))
        strcat( buf, "`3(`#TOURNEY`3)`* ");
     if (IS_SET (victim->comm, COMM_AFK))
        strcat (buf, "(`^AFK`*) ");
    if (IS_AFFECTED (victim, AFF_INVISIBLE))
        strcat (buf, "(`4Invis`*) ");
    if (IS_AFFECTED (victim, AFF_SUPER_INVIS))
        strcat (buf, "(`4Super_Invis`*) ");
    if (victim->invis_level >= LEVEL_HERO)
        strcat (buf, "(`5Wizi`*) ");
    if (IS_AFFECTED (victim, AFF_HIDE))
        strcat (buf, "(`!Hide`*) ");
    if (IS_AFFECTED(victim, AFF_VEIL))
        strcat  (buf, "(`4Veiled`7) ");
    if (IS_AFFECTED(victim, AFF_SHROUD))
        strcat  (buf, "(`8Shrouded`7) ");
    if (IS_AFFECTED (victim, AFF_CHARM))
        strcat (buf, "(`3Charmed`*) ");
    if (IS_AFFECTED (victim, AFF_PASS_DOOR))
        strcat (buf, "(`1Translucent`*) ");
    if (IS_AFFECTED (victim, AFF_FAERIE_FIRE))
        strcat (buf, "(`%Pink Aura`*) ");
    if (IS_EVIL (victim) && IS_AFFECTED (ch, AFF_DETECT_EVIL))
        strcat (buf, "(`!Red Aura`*) ");
    if (IS_GOOD (victim) && IS_AFFECTED (ch, AFF_DETECT_GOOD))
        strcat (buf, "`#(`3Golden Aura`#)`* ");
    if (IS_AFFECTED (victim, AFF_SANCTUARY))
        strcat (buf, "`&(White Aura)`* ");
    if (IS_AFFECTED (victim, AFF_ICESHIELD))
	strcat (buf, "`4(`$Icy Aura`4)`* ");
    if (IS_AFFECTED (victim, AFF_FIRESHIELD))
	strcat (buf, "`1(`#F`!iery `#A`!ura`1)`* ");
    if (IS_AFFECTED (victim, AFF_SHOCKSHIELD))
	strcat (buf, "`#(`$C`3rackling `$A`3ura`#)`* ");

    if (IS_DISGUISED(victim) && IS_IMMORTAL(ch))
    {
    	sprintf(buf2,"`#[`3%s`#]`* ",victim->pcdata->disguise.orig_name);
    	strcat (buf,buf2);
    }

    if ((victim->position == victim->start_pos
        && !IS_AFFECTED(victim, AFF_SEVERED)
        && victim->long_descr[0] != '\0')||IS_DISGUISED(victim))
    {
        strcat (buf, victim->long_descr);
        send_to_char (buf, ch);
        return;
    }

    strcat (buf, PERS (victim, ch, FALSE));

/*    if (!IS_NPC (victim) && !IS_DISGUISED(victim) && !IS_SET (ch->comm, COMM_BRIEF)
        && victim->position == POS_STANDING && ch->on == NULL && !IS_SET(ch->act2, PLR_NOTITLE))
        strcat (buf, victim->pcdata->title);
 */

	if (IS_AFFECTED (victim, AFF_SEVERED) && !IS_NPC(victim))
    {
        sprintf(buf,"%s's upper torso is here,twitching.\n\r",victim->name);
        send_to_char(buf,ch);
        return;
    }
	if (IS_AFFECTED (victim, AFF_SEVERED) && IS_NPC(victim))
    {
        sprintf(buf,"%s's upper torso is here,twitching.\n\r",victim->short_descr);
        send_to_char(buf,ch);
        return;
    }

    switch (victim->position)
    {
        case POS_DEAD:
            strcat (buf, " is DEAD!!");
            break;
        case POS_MORTAL:
            strcat (buf, " is mortally wounded.");
            break;
        case POS_INCAP:
            strcat (buf, " is incapacitated.");
            break;
        case POS_STUNNED:
            strcat (buf, " is lying here stunned.");
            break;
        case POS_SLEEPING:
            if (victim->on != NULL)
            {
                if (IS_SET (victim->on->value[2], SLEEP_AT))
                {
                    sprintf (message, " is sleeping at %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
                else if (IS_SET (victim->on->value[2], SLEEP_ON))
                {
                    sprintf (message, " is sleeping on %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
                else
                {
                    sprintf (message, " is sleeping in %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
            }
            else
                strcat (buf, " is sleeping here.");
            break;
        case POS_RESTING:
            if (victim->on != NULL)
            {
                if (IS_SET (victim->on->value[2], REST_AT))
                {
                    sprintf (message, " is resting at %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
                else if (IS_SET (victim->on->value[2], REST_ON))
                {
                    sprintf (message, " is resting on %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
                else
                {
                    sprintf (message, " is resting in %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
            }
            else
                strcat (buf, " is resting here.");
            break;
        case POS_SITTING:
            if (victim->on != NULL)
            {
                if (IS_SET (victim->on->value[2], SIT_AT))
                {
                    sprintf (message, " is sitting at %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
                else if (IS_SET (victim->on->value[2], SIT_ON))
                {
                    sprintf (message, " is sitting on %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
                else
                {
                    sprintf (message, " is sitting in %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
            }
            else
                strcat (buf, " is sitting here.");
            break;
        case POS_STANDING:
            if (victim->on != NULL)
            {
                if (IS_SET (victim->on->value[2], STAND_AT))
                {
                    sprintf (message, " is standing at %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
                else if (IS_SET (victim->on->value[2], STAND_ON))
                {
                    sprintf (message, " is standing on %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
                else
                {
                    sprintf (message, " is standing in %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
            }
            else
                strcat (buf, " is here.");
            break;
        case POS_FIGHTING:
            strcat (buf, " is here, fighting ");
            if (victim->fighting == NULL)
                strcat (buf, "thin air??");
            else if (victim->fighting == ch)
                strcat (buf, "YOU!");
            else if (victim->in_room == victim->fighting->in_room)
            {
                strcat (buf, IS_DISGUISED(victim)?victim->short_descr:PERS (victim->fighting, ch, FALSE));
                strcat (buf, ".");
            }
            else
                strcat (buf, "someone who left??");
            break;
        case POS_MEDITATING:
            if (victim->on != NULL)
            {
                if (IS_SET (victim->on->value[2], MEDITATE_AT))
                {
                    sprintf (message, " is meditating at %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
                else if (IS_SET (victim->on->value[2], MEDITATE_ON))
                {
                    sprintf (message, " is meditating on %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
                else
                {
                    sprintf (message, " is meditating in %s.",
                             victim->on->short_descr);
                    strcat (buf, message);
                }
            }
            else
                strcat (buf, " is meditating here.");
            break;
    }

    strcat (buf, "\n\r");
    buf[0] = UPPER (buf[0]);
    send_to_char (buf, ch);
    return;
}



void show_char_to_char_1 (CHAR_DATA * victim, CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int iWear;
    int percent;
    bool found;

    if (can_see (victim, ch))
    {
        if (ch == victim)
            act ("$n looks at $mself.", ch, NULL, NULL, TO_ROOM);
        else
        {
            act ("$n looks at you.", ch, NULL, victim, TO_VICT);
            act ("$n looks at $N.", ch, NULL, victim, TO_NOTVICT);
        }
    }

    if (victim->description[0] != '\0')
    {
        send_to_char (victim->description, ch);
    }
    else
    {
        act ("You see nothing special about $M.", ch, NULL, victim, TO_CHAR);
    }

    if (victim->max_hit > 0)
        percent = (100 * victim->hit) / victim->max_hit;
    else
        percent = -1;

    strcpy (buf, IS_DISGUISED(victim)?victim->short_descr:PERS (victim, ch, FALSE));

    if (percent >= 100)
        strcat (buf, " is in excellent condition.\n\r");
    else if (percent >= 90)
        strcat (buf, " has a few scratches.\n\r");
    else if (percent >= 75)
        strcat (buf, " has some small wounds and bruises.\n\r");
    else if (percent >= 50)
        strcat (buf, " has quite a few wounds.\n\r");
    else if (percent >= 30)
        strcat (buf, " has some big nasty wounds and scratches.\n\r");
    else if (percent >= 15)
        strcat (buf, " looks pretty hurt.\n\r");
    else if (percent >= 0)
        strcat (buf, " is in awful condition.\n\r");
    else
        strcat (buf, " is bleeding to death.\n\r");

    buf[0] = UPPER (buf[0]);
    send_to_char (buf, ch);

    found = FALSE;

    if (!IS_SET (victim->act2, PLR_NOEQ) || IS_IMMORTAL(ch) || IS_SET(ch->in_room->room_flags, ROOM_SAFE))
    {
    for (iWear = 0; iWear < MAX_WEAR; iWear++)
    {
        if ((obj = get_eq_char (victim, iWear)) != NULL
            && can_see_obj (ch, obj))
        {
            if (!found)
            {
                send_to_char ("\n\r", ch);
                sprintf (buf,"%s is using:\n\r",IS_DISGUISED(victim)?victim->short_descr: PERS(victim, ch, FALSE));
                send_to_char (buf,ch);
                found = TRUE;
            }
            send_to_char (where_name[iWear], ch);
            send_to_char (format_obj_to_char (obj, ch, TRUE), ch);
            send_to_char ("\n\r", ch);
        }
    }
}
   /* if (victim != ch && !IS_NPC (ch)
        && number_percent () < get_skill (ch, gsn_peek))
    {
        send_to_char ("\n\rYou peek at the inventory:\n\r", ch);
        check_improve (ch, gsn_peek, TRUE, 4);
        show_list_to_char (victim->carrying, ch, TRUE, TRUE, 0);
    }

    return;*/
}



void show_char_to_char (CHAR_DATA * list, CHAR_DATA * ch)
{
    CHAR_DATA *rch;

    for (rch = list; rch != NULL; rch = rch->next_in_room)
    {
        if (rch == ch)
            continue;

        if (get_trust (ch) < rch->invis_level)
            continue;

        if (can_see (ch, rch))
        {
            show_char_to_char_0 (rch, ch);
        }
        else if (room_is_dark (ch->in_room)
                 && IS_AFFECTED (rch, AFF_INFRARED))
        {
            send_to_char ("You see glowing red eyes watching YOU!\n\r", ch);
        }
    }

    return;
}

void show_room_blind( CHAR_DATA *ch )
{
        char buf[MAX_STRING_LENGTH];
        EXIT_DATA *pexit;
	ROOM_INDEX_DATA *was_in_room;
        CHAR_DATA *fch;
        bool found;
        int count, door, outlet;

        send_to_char( "It is pitch black ... \n\r", ch );

        sprintf(buf,"   You sense exits: ");
    found = FALSE;
    for( door = 0; door < 6; door++ )
    {
                outlet = door;
                if((ch->alignment < 0)
                        && (pexit = ch->in_room->exit[door+6] ) != NULL)
                        outlet += 6;

		was_in_room = ch->in_room;

                if( (pexit = ch->in_room->exit[outlet] ) != NULL
                        && pexit->u1.to_room != NULL
			&& pexit->u1.to_room != was_in_room
                        && can_see_room(ch,pexit->u1.to_room))
                {
                        found = TRUE;
                        strcat( buf, " " );
                        strcat( buf, dir_name[outlet] );
                }
        }
        strcat(buf,"\n\r");

        if( found )
                send_to_char(buf,ch);
        else
                send_to_char("You don't sense any exits.\n\r",ch);

        count = 0;
        for( fch = ch->in_room->people; fch; fch = fch->next_in_room )
        {
                if( fch == ch )
                        continue;
                ++count;
        }

        if( count > 0 )
        {
                switch(count)
                {
                case 1:
                        send_to_char("You sense one other being here.\n\r",ch);
                        break;
                case 2:
                        send_to_char("You sense two other beings here.\n\r",ch);
                        break;
                case 3:
                        send_to_char("You sense three other beings here.\n\r",ch);
                        break;
                case 4:
                        send_to_char("You sense four other beings here.\n\r",ch);
                        break;
                default:
                        send_to_char("You sense many other beings here.\n\r",ch);
                        break;
                }
        }
        return;
}


bool check_blind (CHAR_DATA * ch)
{

    if (!IS_NPC (ch) && IS_SET (ch->act, PLR_HOLYLIGHT))
        return TRUE;

    if (IS_AFFECTED (ch, AFF_BLIND) || is_affected (ch, gsn_gouge))
    {
        send_to_char ("You can't see a thing!\n\r", ch);
        return FALSE;
    }

    return TRUE;
}

/* changes your scroll */
void do_scroll (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        if (ch->lines == 0)
            send_to_char ("You do not page long messages.\n\r", ch);
        else
        {
            sprintf (buf, "You currently display %d lines per page.\n\r",
                     ch->lines + 2);
            send_to_char (buf, ch);
        }
        return;
    }

    if (!is_number (arg))
    {
        send_to_char ("You must provide a number.\n\r", ch);
        return;
    }

    lines = atoi (arg);

    if (lines == 0)
    {
        send_to_char ("Paging disabled.\n\r", ch);
        ch->lines = 0;
        return;
    }

    if (lines < 10 || lines > 100)
    {
        send_to_char ("You must provide a reasonable number.\n\r", ch);
        return;
    }

    sprintf (buf, "Scroll set to %d lines.\n\r", lines);
    send_to_char (buf, ch);
    ch->lines = lines - 2;
}

/* RT does socials */
void do_socials (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;

    col = 0;

    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
    {
        sprintf (buf, "%-12s", social_table[iSocial].name);
        send_to_char (buf, ch);
        if (++col % 6 == 0)
            send_to_char ("\n\r", ch);
    }

    if (col % 6 != 0)
        send_to_char ("\n\r", ch);
    return;
}



/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd (CHAR_DATA * ch, char *argument)
{
    do_function (ch, &do_mhelp, "* 'intro note'");
}

void do_work (CHAR_DATA * ch, char *argument)
{
    do_function (ch, &do_mhelp, "* work");
}

void do_imotd (CHAR_DATA * ch, char *argument)
{
    do_function (ch, &do_mhelp, "* imotd");
}

void do_rules (CHAR_DATA * ch, char *argument)
{
    do_function (ch, &do_mhelp, "* rules");
}

void do_story (CHAR_DATA * ch, char *argument)
{
    do_function (ch, &do_mhelp, "* story");
}

void do_wizlist (CHAR_DATA * ch, char *argument)
{
    do_function (ch, &do_mhelp, "* wizlist");
}

/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

void do_autolist (CHAR_DATA * ch, char *argument)
{
    /* lists most player flags */
    if (IS_NPC (ch))
        return;

    send_to_char ("   action     status\n\r", ch);
    send_to_char ("---------------------\n\r", ch);

    send_to_char ("autoassist     ", ch);
    if (IS_SET (ch->act, PLR_AUTOASSIST))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("autoexit       ", ch);
    if (IS_SET (ch->act, PLR_AUTOEXIT))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("autogold       ", ch);
    if (IS_SET (ch->act, PLR_AUTOGOLD))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("autoloot       ", ch);
    if (IS_SET (ch->act, PLR_AUTOLOOT))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("autosac        ", ch);
    if (IS_SET (ch->act, PLR_AUTOSAC))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("autosplit      ", ch);
    if (IS_SET (ch->act, PLR_AUTOSPLIT))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("battlemeter    ", ch);
    if (ch->pcdata->barOn)
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("compact mode   ", ch);
    if (IS_SET (ch->comm, COMM_COMPACT))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);


    send_to_char ("extra dam mes  ", ch);
    if (!IS_SET (ch->act2, PLR_NODAMAGE))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("weave message  ", ch);
    if (!IS_SET (ch->act2, PLR_NOWEAVE))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("hunger flags   ", ch);
    if (IS_SET (ch->act, PLR_HUNGER_FLAG))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("prompt         ", ch);
    if (IS_SET (ch->comm, COMM_PROMPT))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("titles         ", ch);
    if (!IS_SET (ch->act2, PLR_NOTITLE))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    send_to_char ("combine items  ", ch);
    if (IS_SET (ch->comm, COMM_COMBINE))
        send_to_char ("ON\n\r", ch);
    else
        send_to_char ("OFF\n\r", ch);

    if (IS_SET (ch->act, PLR_NOSUMMON))
        send_to_char ("You cannot be summoned.\n\r", ch);
    else
        send_to_char ("You can be summoned.\n\r", ch);

    if (IS_SET (ch->act, PLR_NOFOLLOW))
        send_to_char ("You do not welcome followers.\n\r", ch);
    else
        send_to_char ("You accept followers.\n\r", ch);
}

void do_hungerflag(CHAR_DATA *ch, char *argument)
{
  if (IS_SET(ch->act, PLR_HUNGER_FLAG))
  {
    send_to_char("Hunger flags off you will now see the spam.\n\r", ch);
    REMOVE_BIT(ch->act, PLR_HUNGER_FLAG);
    return;
  }

  SET_BIT(ch->act, PLR_HUNGER_FLAG);
  send_to_char("Hunger flags on, no more spam.\n\r", ch);
  return;
}

void do_battlemeter(CHAR_DATA *ch, char *argument)
{
  if (ch->pcdata->barOn)
  {
    send_to_char("Battle meter off.\n\r", ch);
    ch->pcdata->barOn = FALSE;
  }
  else
  {
    send_to_char("Battle meter on.\n\r", ch);
    ch->pcdata->barOn = TRUE;
  }
  return;
}


void do_telnet_ga (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;

	if (IS_SET (ch->comm, COMM_TELNET_GA))
	{
		send_to_char ("Telnet Go-Ahead removed.\n\r", ch);
		REMOVE_BIT (ch->comm, COMM_TELNET_GA);
	}
	else
	{
		send_to_char ("Telnet Go-Ahead enabled.\n\r", ch);
		SET_BIT (ch->comm, COMM_TELNET_GA);
	}
}
void do_autoassist (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
        return;

    if (IS_SET (ch->act, PLR_AUTOASSIST))
    {
        send_to_char ("Autoassist removed.\n\r", ch);
        REMOVE_BIT (ch->act, PLR_AUTOASSIST);
    }
    else
    {
        send_to_char ("You will now assist when needed.\n\r", ch);
        SET_BIT (ch->act, PLR_AUTOASSIST);
    }
}

void do_autoexit (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
        return;

    if (IS_SET (ch->act, PLR_AUTOEXIT))
    {
        send_to_char ("Exits will no longer be displayed.\n\r", ch);
        REMOVE_BIT (ch->act, PLR_AUTOEXIT);
    }
    else
    {
        send_to_char ("Exits will now be displayed.\n\r", ch);
        SET_BIT (ch->act, PLR_AUTOEXIT);
    }
}

void do_autogold (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
        return;

    if (IS_SET (ch->act, PLR_AUTOGOLD))
    {
        send_to_char ("Autogold removed.\n\r", ch);
        REMOVE_BIT (ch->act, PLR_AUTOGOLD);
    }
    else
    {
        send_to_char ("Automatic gold looting set.\n\r", ch);
        SET_BIT (ch->act, PLR_AUTOGOLD);
    }
}

void do_autoloot (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
        return;

    if (IS_SET (ch->act, PLR_AUTOLOOT))
    {
        send_to_char ("Autolooting removed.\n\r", ch);
        REMOVE_BIT (ch->act, PLR_AUTOLOOT);
    }
    else
    {
        send_to_char ("Automatic corpse looting set.\n\r", ch);
        SET_BIT (ch->act, PLR_AUTOLOOT);
    }
}

void do_autosac (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
        return;

    if (IS_SET (ch->act, PLR_AUTOSAC))
    {
        send_to_char ("Autosacrificing removed.\n\r", ch);
        REMOVE_BIT (ch->act, PLR_AUTOSAC);
    }
    else
    {
        send_to_char ("Automatic corpse sacrificing set.\n\r", ch);
        SET_BIT (ch->act, PLR_AUTOSAC);
    }
}

void do_autosplit (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
        return;

    if (IS_SET (ch->act, PLR_AUTOSPLIT))
    {
        send_to_char ("Autosplitting removed.\n\r", ch);
        REMOVE_BIT (ch->act, PLR_AUTOSPLIT);
    }
    else
    {
        send_to_char ("Automatic gold splitting set.\n\r", ch);
        SET_BIT (ch->act, PLR_AUTOSPLIT);
    }
}

void do_autoall (CHAR_DATA *ch, char * argument)
{
    if (IS_NPC(ch))
        return;

    if (!strcmp (argument, "on"))
    {
        SET_BIT(ch->act,PLR_AUTOASSIST);
        SET_BIT(ch->act,PLR_AUTOEXIT);
        SET_BIT(ch->act,PLR_AUTOGOLD);
        SET_BIT(ch->act,PLR_AUTOLOOT);
        SET_BIT(ch->act,PLR_AUTOSAC);
        SET_BIT(ch->act,PLR_AUTOSPLIT);

        send_to_char("All autos turned on.\n\r",ch);
    }
    else if (!strcmp (argument, "off"))
    {
        REMOVE_BIT (ch->act, PLR_AUTOASSIST);
        REMOVE_BIT (ch->act, PLR_AUTOEXIT);
        REMOVE_BIT (ch->act, PLR_AUTOGOLD);
        REMOVE_BIT (ch->act, PLR_AUTOLOOT);
        REMOVE_BIT (ch->act, PLR_AUTOSAC);
        REMOVE_BIT (ch->act, PLR_AUTOSPLIT);

        send_to_char("All autos turned off.\n\r", ch);
    }
    else
        send_to_char("Usage: autoall [on|off]\n\r", ch);
}

void do_brief (CHAR_DATA * ch, char *argument)
{
    if (IS_SET (ch->comm, COMM_BRIEF))
    {
        send_to_char ("Full descriptions activated.\n\r", ch);
        REMOVE_BIT (ch->comm, COMM_BRIEF);
    }
    else
    {
        send_to_char ("Short descriptions activated.\n\r", ch);
        SET_BIT (ch->comm, COMM_BRIEF);
    }
}

void do_compact (CHAR_DATA * ch, char *argument)
{
    if (IS_SET (ch->comm, COMM_COMPACT))
    {
        send_to_char ("Compact mode removed.\n\r", ch);
        REMOVE_BIT (ch->comm, COMM_COMPACT);
    }
    else
    {
        send_to_char ("Compact mode set.\n\r", ch);
        SET_BIT (ch->comm, COMM_COMPACT);
    }
}

void do_show (CHAR_DATA * ch, char *argument)
{
    if (IS_SET (ch->comm, COMM_SHOW_AFFECTS))
    {
        send_to_char ("Affects will no longer be shown in score.\n\r", ch);
        REMOVE_BIT (ch->comm, COMM_SHOW_AFFECTS);
    }
    else
    {
        send_to_char ("Affects will now be shown in score.\n\r", ch);
        SET_BIT (ch->comm, COMM_SHOW_AFFECTS);
    }
}

void do_prompt (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (argument[0] == '\0')
    {
        if (IS_SET (ch->comm, COMM_PROMPT))
        {
            send_to_char ("You will no longer see prompts.\n\r", ch);
            REMOVE_BIT (ch->comm, COMM_PROMPT);
        }
        else
        {
            send_to_char ("You will now see prompts.\n\r", ch);
            SET_BIT (ch->comm, COMM_PROMPT);
        }
        return;
    }

    if (!strcmp (argument, "all"))
        strcpy (buf, "<%hhp/%Hhp> <%mm/%Mm> %vmv %gg %Xx <%T> ");
    else
    {
        if (colorstrlen (argument) > 50)
            argument[50] = '\0';
        strcpy (buf, argument);
        smash_tilde (buf);
        if (str_suffix ("%c", buf))
            strcat (buf, " ");

    }

    free_string (ch->prompt);
    ch->prompt = str_dup (buf);
    sprintf (buf, "Prompt set to %s\n\r", ch->prompt);
    send_to_char (buf, ch);
    return;
}

void do_combine (CHAR_DATA * ch, char *argument)
{
    if (IS_SET (ch->comm, COMM_COMBINE))
    {
        send_to_char ("Long inventory selected.\n\r", ch);
        REMOVE_BIT (ch->comm, COMM_COMBINE);
    }
    else
    {
        send_to_char ("Combined inventory selected.\n\r", ch);
        SET_BIT (ch->comm, COMM_COMBINE);
    }
}

void do_nofollow (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
        return;

    if (IS_SET (ch->act, PLR_NOFOLLOW))
    {
        send_to_char ("You now accept followers.\n\r", ch);
        REMOVE_BIT (ch->act, PLR_NOFOLLOW);
    }
    else
    {
        send_to_char ("You no longer accept followers.\n\r", ch);
        SET_BIT (ch->act, PLR_NOFOLLOW);
        die_follower (ch);
    }
}

void do_nosummon (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
    {
        if (IS_SET (ch->imm_flags, IMM_SUMMON))
        {
            send_to_char ("You are no longer immune to summon.\n\r", ch);
            REMOVE_BIT (ch->imm_flags, IMM_SUMMON);
        }
        else
        {
            send_to_char ("You are now immune to summoning.\n\r", ch);
            SET_BIT (ch->imm_flags, IMM_SUMMON);
        }
    }
    else
    {
        if (IS_SET (ch->act, PLR_NOSUMMON))
        {
            send_to_char ("You are no longer immune to summon.\n\r", ch);
            REMOVE_BIT (ch->act, PLR_NOSUMMON);
        }
        else
        {
            send_to_char ("You are now immune to summoning.\n\r", ch);
            SET_BIT (ch->act, PLR_NOSUMMON);
        }
    }
}

void do_look (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char *pdesc;
    int door;
    int number, count;

    if (ch->desc == NULL)
        return;

    if (ch->position < POS_SLEEPING)
    {
        send_to_char ("You can't see anything but stars!\n\r", ch);
        return;
    }

    if (ch->position == POS_SLEEPING)
    {
        send_to_char ("You can't see anything, you're sleeping!\n\r", ch);
        return;
    }

    if (!check_blind (ch))
        return;

    if (!IS_NPC (ch)
        && !IS_SET (ch->act, PLR_HOLYLIGHT) && room_is_dark (ch->in_room)
        && !IS_SET(ch->act2, PLR_DREAM))
    {
        send_to_char ("`&It is pitch black ... `*\n\r", ch);
        show_char_to_char (ch->in_room->people, ch);
        return;
    }

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);
    number = number_argument (arg1, arg3);
    count = 0;

   if (arg1[0] == '\0' || !str_cmp (arg1, "auto") || !str_cmp(arg1, "move"))
   {

        /* 'look' or 'look auto' */
        /* Automap shows all this so we'll skip it*/
       if(IS_SET(ch->in_room->room_flags, ROOM_WILDERNESS))
       {
          if (!IS_IMMORTAL(ch))
          {
           send_to_char ("`$", ch);
           send_to_char (ch->in_room->name, ch);
           send_to_char ("`7\n\r", ch);
          }
          if ((IS_IMMORTAL (ch)
             && (IS_NPC (ch) || IS_SET (ch->act, PLR_HOLYLIGHT)))
            || IS_BUILDER (ch, ch->in_room->area))
          {
            send_to_char ("`$", ch);
            send_to_char (ch->in_room->name, ch);
            send_to_char ("`7", ch);
            sprintf (buf, "`1 [`!Room %ld`1]`*\n\r", ch->in_room->vnum);
            send_to_char (buf, ch);
          }

	           do_function(ch, &do_map, "");

           do_function (ch, &do_exits, "auto");
       }
       else
       {
        send_to_char ("\n\r", ch);

        if (!IS_AFFECTED(ch, AFF_DOME))
        {
          /* 'look' or 'look auto' */
          if (IS_SET(ch->act2, PLR_MXP))
          {
            send_to_char("\e[1z <RName>", ch);
          }
          send_to_char ("`$", ch);
          send_to_char (ch->in_room->name, ch);
          send_to_char ("`*", ch);
          if (IS_SET(ch->act2, PLR_MXP))
          {
            send_to_char("\e[1z </RName>", ch);
          }

          if ((IS_IMMORTAL (ch)
             && (IS_NPC (ch) || IS_SET (ch->act, PLR_HOLYLIGHT)))
            || IS_BUILDER (ch, ch->in_room->area))
          {
            sprintf (buf, "`1 [`!Room %ld`1]`*", ch->in_room->vnum);
            send_to_char (buf, ch);
          }

          send_to_char ("\n\r", ch);

        AREA_DATA *cArea;
        cArea = ch->in_room->area;

          if (arg1[0] == '\0'
            || (!IS_NPC (ch) && !IS_SET (ch->comm, COMM_BRIEF) && (str_cmp(arg1, "move")
            ||  (ch->desc->out_compress) || IS_IMMORTAL(ch)))
            || (IS_SWITCHED(ch) && !IS_SET(ch->desc->original->comm, COMM_BRIEF)))
          {
          if (IS_SET(ch->act2, PLR_MXP))
          {
            send_to_char("\e[1z <RDesc>", ch);
          }
/*
          AREA_DATA *cArea;
          cArea = ch->in_room->area;
*/
            sprintf(buf, "`*%s", ch->in_room->description);
            send_to_char( buf, ch );

          if (IS_SET(ch->act2, PLR_MXP))
          {
            send_to_char("\e[1z </RDesc>", ch);
          }
	  /*
            send_to_char ("  ", ch);
            send_to_char ("`*", ch);
            send_to_char (ch->in_room->description, ch);
            send_to_char ("`*", ch);
         */ }
       }
       else if (IS_AFFECTED(ch, AFF_DOME))
       {
         send_to_char("`$A gleeman fantasy world`*\n\r\n\r", ch);
         if (arg1[0] == '\0'
           || (!IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF)))
         {
         send_to_char("You are running around in a gleeman fantasy world.\n\r", ch);
         send_to_char("Everything seems the same no matter which way you look.\n\r", ch);
         }
       }


        if (!IS_NPC (ch) && IS_SET (ch->act, PLR_AUTOEXIT))
        {
            send_to_char ("\n\r", ch);
            do_function (ch, &do_exits, "auto");
        }

        if (IS_SWITCHED(ch)  && IS_SET(ch->desc->original->act, PLR_AUTOEXIT))
        {
            send_to_char ("\n\r", ch);
            do_function (ch, &do_exits, "auto");
        }
      } /* End of not Wilderness Loop */
        show_list_to_char (ch->in_room->contents, ch, FALSE, FALSE, ON_GROUND);
        show_char_to_char (ch->in_room->people, ch);
        return;
    }

    if (!str_cmp (arg1, "i") || !str_cmp (arg1, "in")
        || !str_cmp (arg1, "on"))
    {
        /* 'look in' */
        if (arg2[0] == '\0')
        {
            send_to_char ("Look in what?\n\r", ch);
            return;
        }

        if ((obj = get_obj_here (ch, NULL, arg2)) == NULL)
        {
            send_to_char ("You do not see that here.\n\r", ch);
            return;
        }

        switch (obj->item_type)
        {
            default:
                send_to_char ("That is not a container.\n\r", ch);
                break;

            case ITEM_PORTAL:
              {
                ROOM_INDEX_DATA *to_room;

                to_room = get_room_index(obj->value[3]);
                send_to_char ("`$", ch);
                send_to_char (to_room->name, ch);
                send_to_char ("`*", ch);

                if ((IS_IMMORTAL (ch)
                   && (IS_NPC (ch) || IS_SET (ch->act, PLR_HOLYLIGHT)))
                   || IS_BUILDER (ch, to_room->area))
                {
                  sprintf (buf, "`1 [`!Room %ld`1]`*", to_room->vnum);
                  send_to_char (buf, ch);
                }

                send_to_char ("\n\r", ch);

                if (arg1[0] == '\0'
                  || (!IS_NPC (ch) && !IS_SET (ch->comm, COMM_BRIEF))
                  || (IS_SWITCHED(ch) && !IS_SET(ch->desc->original->comm, COMM_BRIEF)) )
                 {
	            sprintf(buf, "`*%s", to_room->description);
         	    send_to_char( buf, ch );
                 }
                 else if (IS_AFFECTED(ch, AFF_DOME))
                 {
                   send_to_char("`$A gleeman fantasy world`*\n\r\n\r", ch);
                   if (arg1[0] == '\0'
                    || (!IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF)))
                    {
                      send_to_char("You are running around in a gleeman fantasy world.\n\r", ch);
                      send_to_char("Everything seems the same no matter which way you look.\n\r", ch);
                    }
                 }
                 else
                 {
                   send_to_char("`$A Bright Blinding Room`*\n\r\n\r", ch);
                   if (arg1[0] == '\0'
                     || (!IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF)))
                   {
                      send_to_char("You are running around half blind in a bright all white room.\n\r", ch);
                      send_to_char("Everything seems the same no matter which way you look.\n\r", ch);
                   }
                 }
  
        
                 if (!IS_NPC (ch) && IS_SET (ch->act, PLR_AUTOEXIT))
                 {
                   send_to_char ("\n\r", ch);
                   port_exits(ch, to_room);
                 }

                 show_list_to_char (to_room->contents, ch, FALSE, FALSE, ON_GROUND);
                 show_char_to_char (to_room->people, ch);
               }
               break;
 
            case ITEM_DRINK_CON:
                if (obj->value[1] <= 0)
                {
                    send_to_char ("It is empty.\n\r", ch);
                    break;
                }

                sprintf (buf, "It's %sfilled with  a %s liquid.\n\r",
                         obj->value[1] < obj->value[0] / 4
                         ? "less than half-" :
                         obj->value[1] < 3 * obj->value[0] / 4
                         ? "about half-" : "more than half-",
                         liq_table[obj->value[2]].liq_color);

                send_to_char (buf, ch);
                break;

            case ITEM_CONTAINER:
            case ITEM_ENVELOPE:
            case ITEM_KEYRING:
            case ITEM_CORPSE_NPC:
            case ITEM_CORPSE_PC:
                if (IS_SET (obj->value[1], CONT_CLOSED))
                {
                    send_to_char ("It is closed.\n\r", ch);
                    break;
                }

                act ("$p holds:", ch, obj, NULL, TO_CHAR);
                show_list_to_char (obj->contains, ch, TRUE, TRUE, 0);
                break;
            case ITEM_QUIVER:
                if (obj->value[0] <= 0)
                {
                    send_to_char("This quiver is empty.\n\r",ch);
                    break;
                }
                if (obj->value[0] == 1)
                {
                    send_to_char("This quiver has 1 arrow remaining.\n\r",ch);
                    break;
                }
                if (obj->value[0] > 1)
                {
                    sprintf(buf,"This quiver has %d arrows remaining.\n\r",obj->value[0]);
                }
                send_to_char(buf,ch);
                break;

			case ITEM_WINDOW:
	for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
    {
        if (can_see_obj (ch, obj))
        {
        
        if (obj->item_type == ITEM_WINDOW)
	{
		look_window(ch, obj);
           	return;
        }

            pdesc = get_extra_descr (arg3, obj->extra_descr);
            if (pdesc != NULL)
                if (++count == number)
                {
                    send_to_char (pdesc, ch);
                    return;
                }

            pdesc = get_extra_descr (arg3, obj->pIndexData->extra_descr);
            if (pdesc != NULL)
                if (++count == number)
                {
                    send_to_char (pdesc, ch);
                    return;
                }

            if (is_name (arg3, obj->name))
                if (++count == number)
                {
                    send_to_char (obj->description, ch);
                    send_to_char ("\n\r", ch);
                    return;
                }
        }
    }


        }
        return;
    }

    if ((victim = get_char_room (ch, NULL, arg1)) != NULL)
    {
        show_char_to_char_1 (victim, ch);
        return;
    }

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
        if (can_see_obj (ch, obj))
        {                        /* player can see object */
            pdesc = get_extra_descr (arg3, obj->extra_descr);
            if (pdesc != NULL)
            {
                if (++count == number)
                {
                    send_to_char (pdesc, ch);
                    return;
                }
                else
                    continue;
            }

            pdesc = get_extra_descr (arg3, obj->pIndexData->extra_descr);
            if (pdesc != NULL)
            {
                if (++count == number)
                {
                    send_to_char (pdesc, ch);
                    return;
                }
                else
                    continue;
            }

            if (is_name (arg3, obj->name))
                if (++count == number)
                {
                    send_to_char (obj->description, ch);
                    send_to_char ("\n\r", ch);
                    if (obj->condition == 100)
                      sprintf(buf, "%s is in mint condition.", obj->short_descr);
                    else if (obj->condition >=80)
                      sprintf(buf, "%s is in good condition.", obj->short_descr);
                    else if (obj->condition >= 60)
                      sprintf(buf, "%s is in decent working order.", obj->short_descr);
                    else if (obj->condition >= 40)
                      sprintf(buf, "%s is in poor condition.", obj->short_descr);
                    else if (obj->condition >=20)
                      sprintf(buf, "%s is in terrible condition.", obj->short_descr);
                    else if (obj->condition >0)
                      sprintf(buf, "%s is on the brink of falling apart.", obj->short_descr);
                    else
                      sprintf(buf, "%s has been completely destroyed by use.", obj->short_descr);
                    send_to_char(buf, ch);
                    return;
                }
        }
    }

    
    pdesc = get_extra_descr (arg3, ch->in_room->extra_descr);
    if (pdesc != NULL)
    {
        if (++count == number)
        {
            send_to_char (pdesc, ch);
            return;
        }
    }

    if (count > 0 && count != number)
    {
        if (count == 1)
            sprintf (buf, "You only see one %s here.\n\r", arg3);
        else
            sprintf (buf, "You only see %d of those here.\n\r", count);

        send_to_char (buf, ch);
        return;
    }

    if (!str_cmp (arg1, "n") || !str_cmp (arg1, "north"))
        door = 0;
    else if (!str_cmp (arg1, "e") || !str_cmp (arg1, "east"))
        door = 1;
    else if (!str_cmp (arg1, "s") || !str_cmp (arg1, "south"))
        door = 2;
    else if (!str_cmp (arg1, "w") || !str_cmp (arg1, "west"))
        door = 3;
    else if (!str_cmp (arg1, "u") || !str_cmp (arg1, "up"))
        door = 4;
    else if (!str_cmp (arg1, "d") || !str_cmp (arg1, "down"))
        door = 5;
    else
    {
       // send_to_char ("You do not see that here.\n\r", ch);
        return;
    }

    /* 'look direction' */
    if ((pexit = ch->in_room->exit[door]) == NULL)
    {
        send_to_char ("Nothing special there.\n\r", ch);
        return;
    }
   
    if (pexit->description != NULL && pexit->description[0] != '\0')
        send_to_char (pexit->description, ch);
    else
        send_to_char ("Nothing special there.\n\r", ch);

    if (pexit->keyword != NULL
        && pexit->keyword[0] != '\0' && pexit->keyword[0] != ' ')
    {
        if (IS_SET (pexit->exit_info, EX_CLOSED))
        {
            act ("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
        }
        else if (IS_SET (pexit->exit_info, EX_ISDOOR))
        {
            act ("The $d is open.", ch, NULL, pexit->keyword, TO_CHAR);
        }
    }

    return;
}


/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits (CHAR_DATA * ch, char *argument)
{
    extern char *const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door;

    fAuto = !str_cmp (argument, "auto");

    if (!check_blind (ch))
        return;

    if (IS_AFFECTED(ch, AFF_DOME))
    {
      send_to_char("You are too dissoriented, you can't tell what leads where.\n\r", ch);
      return;
    }

     if (fAuto)
        sprintf (buf, "`7[Exits:");
    else if (IS_IMMORTAL (ch))
        sprintf (buf, "Obvious exits from room %ld:\n\r", ch->in_room->vnum);
    else
        sprintf (buf, "Obvious exits:\n\r");

    found = FALSE;

    if (IS_SET(ch->act2, PLR_MXP))
    {
      send_to_char("\e[1z <RExits>", ch);
    }

    for (door = 0; door <= 5; door++)
    {
        if ((pexit = ch->in_room->exit[door]) != NULL

            && pexit->u1.to_room != NULL
            && can_see_room (ch, pexit->u1.to_room)
            && !IS_SET (pexit->exit_info, EX_CLOSED))
        {
            found = TRUE;
            if (fAuto)
            {
                char tmp[MSL];
                char tmp2[MSL];
                strcat (buf, " ");
                if (IS_SET(ch->act2, PLR_MXP))
                {
  		  colourstrip(tmp2, dir_name[door]);
                  sprintf(tmp, "\e[1z <send href=\"%s \">%s", tmp2, dir_name[door]);
                  strcat( buf, tmp);
                  strcat( buf, "</send>");
                }
                else
		  strcat(buf, dir_name[door]);
            }
            else
            {
                sprintf (buf + strlen (buf), "%-5s - %s",
                         capitalize (dir_name[door]),
                         room_is_dark (pexit->u1.to_room)
                         ? "Too dark to tell" : pexit->u1.to_room->name);
                if (IS_IMMORTAL (ch))
                    sprintf (buf + strlen (buf),
                             " (room %ld)\n\r", pexit->u1.to_room->vnum);
                else
                    sprintf (buf + strlen (buf), "\n\r");
            }
        }
    }

    if (IS_SET(ch->act2, PLR_MXP))
    {
      send_to_char("\e[1z </RExits>", ch);
    }

    if (!found)
        strcat (buf, fAuto ? " `^N`6one`*" : "`^N`6one.`*\n\r");

    if (fAuto)
        strcat (buf, "]`*\n\r");

    send_to_char (buf, ch);
    return;
}

/*
 * Thanks to Zrin for auto-exit part.
 */
void port_exits (CHAR_DATA *ch, ROOM_INDEX_DATA *to_room)
{
    extern char *const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto = TRUE;
    int door;

    if (!check_blind (ch))
        return;

    if (IS_AFFECTED(ch, AFF_DOME))
    {
      send_to_char("You are too dissoriented, you can't tell what leads where.\n\r", ch);
      return;
    }

    if (fAuto)
        sprintf (buf, "`7[Exits:");


    found = FALSE;
    for (door = 0; door <= 5; door++)
    {
        if ((pexit = to_room->exit[door]) != NULL

            && pexit->u1.to_room != NULL
            && can_see_room (ch, pexit->u1.to_room)
            && !IS_SET (pexit->exit_info, EX_CLOSED))
        {
            found = TRUE;
            if (fAuto)
            {
                strcat (buf, " ");
                strcat (buf, dir_name[door]);
            }
            else
            {
                sprintf (buf + strlen (buf), "%-5s - %s",
                         capitalize (dir_name[door]),
                         room_is_dark (pexit->u1.to_room)
                         ? "Too dark to tell" : pexit->u1.to_room->name);
                if (IS_IMMORTAL (ch))
                    sprintf (buf + strlen (buf),
                             " (room %ld)\n\r", pexit->u1.to_room->vnum);
                else
                    sprintf (buf + strlen (buf), "\n\r");
            }
        }
    }

    if (!found)
        strcat (buf, fAuto ? " `^N`6one`*" : "`^N`6one.`*\n\r");

    if (fAuto)
        strcat (buf, "]`*\n\r");

    send_to_char (buf, ch);
    return;
}

void do_worth (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC (ch))
    {
        sprintf (buf, "You have %ld gold and %ld silver.\n\r",
                 ch->gold, ch->silver);
        send_to_char (buf, ch);
        return;
    }

    if (ch->level >= 75)
    {
        sprintf (buf,
             "You have %ld qp, %ld gold, %ld silver, and %d adventurer experience points.\n\r",
             ch->questpoints, ch->gold, ch->silver, ch->realexp);
        send_to_char (buf, ch);
        return;
    }

    sprintf (buf,
             "You have %ld qp, %ld gold, %ld silver, and %d experience (%d exp to level).\n\r",
             ch->questpoints, ch->gold, ch->silver, ch->exp,
             (ch->level + 1) * exp_per_level (ch,
                                              ch->pcdata->points) - ch->exp);

    send_to_char (buf, ch);

    return;
}


void do_score (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
	int rptnl = 1000 + (ch->rplevel*100); /*If you change you must change in update.c -CAMM*/
    //int i;

   if(IS_NPC(ch))
	return;

send_to_char("`4-----------------------------------------------------------------------------`*\n\r", ch);

// Name, Clan, and Clan Rank and Society if Aiel

sprintf (buf,
"`$Name       `4: `*%s%s`*\n\r",
             IS_DISGUISED(ch)?ch->pcdata->disguise.orig_name:ch->name,
             IS_NPC (ch) ? "" :
             IS_DISGUISED(ch)?ch->pcdata->disguise.orig_title:ch->pcdata->title);
             send_to_char (buf, ch);

if (ch->clan != 0)
{
sprintf (buf,
"`$Clan       `4: `*%s`*- %s`* ",
             clan_table[ch->clan].who_name,
             clan_table[ch->clan].rank[ch->rank]);
             send_to_char( buf, ch);
}

  send_to_char("\n\r", ch);


// Current and Maximum Hit Points

sprintf (buf,
"`$Health     `4: `1%4ld`*/`!%-4ld      ",
             ch->hit, ch->max_hit);
             send_to_char( buf, ch);

// Reputation Rank

    if (!IS_NPC(ch))
    {
      sprintf(buf, "`$Rep Rank   `4: `*%s`*", show_trank(ch));
      send_to_char(buf, ch);
    }

send_to_char("\n\r", ch);

// Current and Maximum Mana

sprintf (buf,
"`$Mana       `4: `6%4d`*/`^%-4d      ",
             ch->mana, ch->max_mana);
             send_to_char(buf, ch);

// Tournament Kills and Deaths

if (!IS_NPC(ch))
{
sprintf (buf,
"`$Kill/Death `4: `*%d/%d`*",
             ch->pcdata->tournament.kills,
             ch->pcdata->tournament.deaths);
             send_to_char(buf, ch);
}

send_to_char("\n\r", ch);

// Current and Maximum Movement

sprintf (buf,
"`$Move       `4: `4%4d`*/`$%-4d      ",
             ch->move, ch->max_move);
             send_to_char(buf, ch);

// Current Reputation

if (!IS_NPC(ch))
{
sprintf (buf,
"`$Reputation `4: `*%d`*",
             ch->pcdata->tournament.reputation);
             send_to_char(buf, ch);
}

send_to_char("\n\r", ch);
send_to_char("`4-----------------------------------------------------------------------------`*\n\r", ch);

// Level, Current Items, and Maximum Items

sprintf (buf,
"`$Level      `4: `*%-15d`$Items      `4: `*%d(%d)`*\n\r",
             ch->level, ch->carry_number, can_carry_n (ch));
             send_to_char(buf, ch);

// Experience and Experience To Next Level

sprintf (buf,
"`$Experience `4: `*%-15d`$To Next Lvl`4: `*%d`*\n\r",
             ch->exp, (ch->level + 1) * exp_per_level (ch, ch->pcdata->points) - ch->exp);
             send_to_char(buf, ch);

// Age, Current Weight, and Maximum Weight
sprintf (buf,
"`$Age (years)`4: `*%-15d`$Weight     `4: `*%ld(%d)`*\n\r",
             get_age (ch),get_carry_weight (ch) / 10,
             can_carry_w (ch) / 10);
             send_to_char(buf, ch);

// Total Hours Played and Roleplay Level

sprintf (buf,
"`$Hours      `4: `*%-15d`$RP Lvl     `4: `*%d`*\n\r",
             (ch->played + (int) (current_time - ch->logon)) / 3600,ch->rplevel);
             send_to_char(buf, ch);

// Race and Roleplay Experience

sprintf (buf,
"`$Nationality`4: `*%-15s`$RP Exp     `4: `*%d(%d)`*\n\r",
             race_table[ch->race].name,ch->rpexp,rptnl);
             send_to_char(buf, ch);

// Sex and Roleplay Points

sprintf (buf,
"`$Sex        `4: `*%-15s`$RP Points  `4: `*%d`*\n\r",
             ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
             ch->rp_points);
             send_to_char(buf, ch);

// Class and Alignment

sprintf (buf,
"`$Class      `4: `*%-15s`$Alignment  `4: `*%d (",
             IS_NPC (ch) ? "mobile" : class_table[ch->cClass].name,
             ch->alignment);
             send_to_char(buf, ch);

// Alignment Type

if (ch->alignment > 900)
        send_to_char ("`&Angelic`*)\n\r", ch);
    else if (ch->alignment > 700)
        send_to_char ("`&Saintly`*)\n\r", ch);
    else if (ch->alignment > 350)
        send_to_char ("`#Good`*)\n\r", ch);
    else if (ch->alignment > 100)
        send_to_char ("Kind)\n\r", ch);
    else if (ch->alignment > -100)
        send_to_char ("Neutral)\n\r", ch);
    else if (ch->alignment > -350)
        send_to_char ("Mean)\n\r", ch);
    else if (ch->alignment > -700)
        send_to_char ("`8Evil`*)\n\r", ch);
    else if (ch->alignment > -900)
        send_to_char ("`1Demonic`*)\n\r", ch);
    else
        send_to_char ("`1Satanic`*)\n\r", ch);

// Remaining Training Sessions and Total % of Map Explored

sprintf (buf,
"`$Trains     `4: `*%-15d`$Explored   `4: `*%3.2f%%\n\r",
             ch->train,(double(roomcount(ch)) / top_room));
             send_to_char(buf, ch);

// Remaining Practice Sessions and Total % of Mobs Killed

sprintf (buf,
"`$Pracs      `4: `*%-15d`$Mobbed     `4: `*%3.2f%%\n\r",
             ch->practice,(double(mobcount(ch)) / top_mob_index));
             send_to_char(buf, ch);

// Wimpy and Gold Coins

sprintf (buf,
"`$Wimpy      `4: `*%-15d`$Gold       `4: `#%ld`* coins\n\r",
             ch->wimpy,ch->gold);
             send_to_char(buf, ch);


// Check to display if player is a Murderer, Thief, or in PK Ticks
/*
if (!IS_NPC(ch))
{
    switch
    case (ch->pcdata->pk_timer > 0)
    {
        sprintf(buf, "`$PK Ticks   `4: `!$-15d`*", ch->pcdata->pk_timer);
        send_to_char(buf,ch);
        break;
    }
    else
*/

// Silver Coins

sprintf (buf,
"`$QPs        `4: `*%-15ld`$Silver     `4: `&%ld`* coins\n\r",
             ch->questpoints,ch->silver);
             send_to_char(buf, ch);

send_to_char("`4-----------------------------------------------------------------------------`*\n\r", ch);

// Current Strength, Maximum Strength, and Power in Fire

sprintf (buf,
"`$Str        `4: `*%2d(%-2d)         `$Fire       `4: `!%d`*\n\r",
             UMIN( ch->perm_stat[STAT_STR], 25),
             get_curr_stat (ch, STAT_STR),
	         get_weave_stat(ch, WEAVE_FIRE));
             send_to_char(buf, ch);

sprintf (buf,
"`$Int        `4: `*%2d(%-2d)         `$Earth      `4: `#%d`*\n\r",
             UMIN( ch->perm_stat[STAT_INT], 25),
             get_curr_stat (ch, STAT_INT),
	         get_weave_stat(ch, WEAVE_EARTH));
             send_to_char(buf, ch);

sprintf (buf,
"`$Wis        `4: `*%2d(%-2d)         `$Air        `4: `^%d`*\n\r",
             UMIN( ch->perm_stat[STAT_WIS], 25),
             get_curr_stat (ch, STAT_WIS),
	         get_weave_stat(ch, WEAVE_AIR));
             send_to_char(buf, ch);

sprintf (buf,
"`$Dex        `4: `*%2d(%-2d)         `$Water      `4: `$%d`*\n\r",
             UMIN( ch->perm_stat[STAT_DEX], 25),
             get_curr_stat (ch, STAT_DEX),
	         get_weave_stat(ch, WEAVE_WATER));
             send_to_char(buf, ch);

sprintf (buf,
"`$Con        `4: `*%2d(%-2d)         `$Spirit     `4: `%%%d`*\n\r",
             UMIN( ch->perm_stat[STAT_CON], 25),
             get_curr_stat (ch, STAT_CON),
	         get_weave_stat(ch, WEAVE_SPIRIT));
             send_to_char(buf, ch);

sprintf (buf,
"`$Hitroll    `4: `*%-15d`$Pierce     `4: `*%d`*\n\r",
             GET_HITROLL (ch),GET_AC (ch, AC_PIERCE));
             send_to_char(buf, ch);

sprintf (buf,
"`$Damroll    `4: `*%-15d`$Bash       `4: `*%d`*\n\r",
             GET_DAMROLL (ch),GET_AC (ch, AC_BASH));
             send_to_char(buf, ch);

sprintf (buf,
"`$Saves      `4: `*%-15d`$Slash      `4: `*%d`*\n\r",
             ch->saving_throw,GET_AC (ch, AC_SLASH));
             send_to_char(buf, ch);

send_to_char("`4-----------------------------------------------------------------------------`*\n\r", ch);
send_to_char("`$Miscellaneous Flags`4:`*\n\r", ch);

// If an Immortal, it will show Holylight

if (IS_IMMORTAL (ch))
    {
        send_to_char ("`$Holy Light `4: `*", ch);
        if (IS_SET (ch->act, PLR_HOLYLIGHT))
            send_to_char ("On\n\r", ch);
        else
            send_to_char ("Off\n\r", ch);

if (get_trust (ch) != ch->level)
{
    sprintf (buf, "You are trusted at level %d.\n\r", get_trust (ch));
    send_to_char (buf, ch);
}
if (!IS_NPC (ch) && ch->pcdata->condition[COND_DRUNK] > 10)
    send_to_char ("You are drunk.\n\r", ch);

switch (ch->position)
{
    case POS_DEAD:
        send_to_char ("You are DEAD!!\n\r", ch);
        break;
    case POS_MORTAL:
        send_to_char ("You are mortally wounded.\n\r", ch);
        break;
    case POS_INCAP:
        send_to_char ("You are incapacitated.\n\r", ch);
        break;
    case POS_STUNNED:
        send_to_char ("You are stunned.\n\r", ch);
        break;
    case POS_SLEEPING:
        send_to_char ("You are sleeping.\n\r", ch);
        break;
    case POS_RESTING:
        send_to_char ("You are resting.\n\r", ch);
        break;
    case POS_SITTING:
        send_to_char ("You are sitting.\n\r", ch);
        break;
    case POS_STANDING:
        send_to_char ("You are standing.\n\r", ch);
        break;
    case POS_FIGHTING:
        send_to_char ("You are fighting.\n\r", ch);
        break;
    case POS_MEDITATING:
        send_to_char ("You are meditating.\n\r", ch);
        break;
}
if (ch->incog_level)
{
    sprintf (buf, "Incognito: level %d", ch->incog_level);
    send_to_char (buf, ch);
}
send_to_char ("\n\r", ch);

}
if (IS_SET (ch->comm, COMM_SHOW_AFFECTS))
    do_function (ch, &do_affects, "");
}



void do_affects (CHAR_DATA * ch, char *argument)
{
    AFFECT_DATA *paf, *paf_last = NULL;
    char buf[MAX_STRING_LENGTH];
    int type;
    if (ch->affected != NULL)
    {
        send_to_char ("You are affected by the following spells:\n\r", ch);
        for (paf = ch->affected; paf != NULL; paf = paf->next)
        {
            type = paf->type;
            if (type == 0 && IS_SPELL_AFFECT(paf->location) && paf->bitvector != 0)
 	    {
 	    	type = skill_lookup(affect_flags[paf->bitvector -1].name);
 	    }
            if (paf_last != NULL && type == paf_last->type)
	    {
                if (ch->level >= 0)
                    sprintf (buf, "                      ");
                else
                    continue;
	    }
            if (type <0)
            {
                send_to_char("Range problem please report to imms\n\r", ch);
                return;
            }


            if ((skill_lookup(skill_table[type].name)) == -1)
            {
               send_to_char(affect_flags[type].name, ch);
               return;
            }

            sprintf (buf, "Spell: %-15s", skill_table[type].name);

            send_to_char (buf, ch);

            if (ch->level >= 0)
            {
                sprintf (buf,
                         ": modifies %s by %d ",
                         affect_loc_name (paf->location), paf->modifier);
                send_to_char (buf, ch);
                if (paf->duration == -1)
                    sprintf (buf, "permanently");
                else if (paf->duration == -2)
                    sprintf (buf, "while embraced");
                else
                    sprintf (buf, "for %d hours", paf->duration);
                send_to_char (buf, ch);
            }

            send_to_char ("\n\r", ch);
            paf_last = paf;
        }
    }
    else
        send_to_char ("You are not affected by any spells.\n\r", ch);

    return;
}



char *const day_name[] = {
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *const month_name[] = {
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time (CHAR_DATA * ch, char *argument)
{
//    char str_boot_time[MIL];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day = time_info.day + 1;

    if (day > 4 && day < 20)
        suf = "th";
    else if (day % 10 == 1)
        suf = "st";
    else if (day % 10 == 2)
        suf = "nd";
    else if (day % 10 == 3)
        suf = "rd";
    else
        suf = "th";

    sprintf (buf,
             "It is %d:%s%d%s %s, Day of %s, %d%s the Month of %s.\n\r",
             (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
              time_info.minute < 10 ? time_info.minute != 0 ? "0" : "" : "",
              time_info.minute, time_info.minute == 0 ? "0" : "",
             time_info.hour >= 12 ? "pm" : "am",
             day_name[day % 7], day, suf, month_name[time_info.month]);
    send_to_char (buf, ch);

    long upTime = (long) difftime(current_time, boot_time);
    send_to_char("\n\rCurrent uptime is ", ch);
    print_time_since(ch, upTime);

    sprintf (buf, "Dragonsworn started up at %sThe system time is %s\n\r",
             str_boot_time, (char *) ctime (&current_time));

    send_to_char (buf, ch);
    return;
}



void do_weather (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    static char *const sky_look[4] = {
        "cloudless",
        "cloudy",
        "rainy",
        "lit by flashes of lightning"
    };

    if (!IS_OUTSIDE (ch))
    {
        send_to_char ("You can't see the weather indoors.\n\r", ch);
        return;
    }

    sprintf (buf, "The sky is %s and %s.\n\r",
             sky_look[weather_info.sky],
             weather_info.change >= 0
             ? "a warm southerly breeze blows"
             : "a cold northern gust blows");
    send_to_char (buf, ch);
    return;
}

void do_help (CHAR_DATA * ch, char *argument)
{
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH];
    int level;
    char nohelp[MAX_STRING_LENGTH];

    send_to_char("Old Help Turned off please report this error to IMP\n\r", ch);
    return;

    output = new_buf ();
    strcpy(nohelp, argument);

    if (argument[0] == '\0')
        argument = "summary";

    if (argument[0] == '|')
    {
      char arg[MSL];
      argument = one_argument(argument, arg);
      do_function(ch, &do_new_help, argument);
      return;
    }

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0')
    {
        argument = one_argument (argument, argone);
        if (argall[0] != '\0')
            strcat (argall, " ");
        strcat (argall, argone);
    }

    for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
    {
        level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

        if (level > get_trust (ch))
            continue;

        if (is_name (argall, pHelp->keyword))
        {
            /* add seperator if found */
            if (found)
                add_buf (output,
                         "\n\r============================================================\n\r\n\r");
            if (pHelp->level >= 0 && str_cmp (argall, "imotd"))
            {
                add_buf (output, pHelp->keyword);
                add_buf (output, "\n\r");
            }

            /*
             * Strip leading '.' to allow initial blanks.
             
            if (pHelp->text[0] == '.')
                add_buf (output, pHelp->text + 1);
            else */
                add_buf (output, pHelp->text);
            found = TRUE;
			return;
            /* small hack :) */
            if (ch->desc != NULL && ch->desc->connected != CON_PLAYING
                && ch->desc->connected != CON_GEN_GROUPS)
                break;
        }
    }

    if (!found)
	{
        send_to_char ("No help on that word.\n\r", ch);
        append_file( ch, HELP_FILE, nohelp );

		/*
		 * Let's log unmet help requests so studious IMP's can improve their help files ;-)
		 * But to avoid idiots, we will check the length of the help request, and trim to
		 * a reasonable length (set it by redefining MAX_CMD_LEN in merc.h).  -- JR
		 */
		if (strlen(argall) > MAX_CMD_LEN)
		{
			argall[MAX_CMD_LEN - 1] = '\0';
			logf ("Excessive command length: %s requested %s.", ch, argall);
			send_to_char ("That was rude!\n\r", ch);
		}
		/* OHELPS_FILE is the "orphaned helps" files. Defined in merc.h -- JR */
		else
		{
			append_file (ch, OHELPS_FILE, argall);
		}
	}
    else
        page_to_char (buf_string (output), ch);
    free_buf (output);
}


void do_new_help (CHAR_DATA * ch, char *argument)
{
    HELP_DATA *pHelp;
    BUFFER *output;
    int i;
    char buf[MSL];
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH];
    int level;
    char nohelp[MAX_STRING_LENGTH];

    output = new_buf ();
    strcpy(nohelp, argument);

    if (argument[0] == '\0')
        argument = "summary";

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0')
    {
        argument = one_argument (argument, argone);
        if (argall[0] != '\0')
            strcat (argall, " ");
        strcat (argall, argone);
    }

    add_buf (output,
     "\n\r====================Possible Keywords=======================\n\r\n\r");
    i = 0;
    for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
    {
        level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

        if (level > get_trust (ch))
            continue;

        if (strstr(pHelp->text, argall)
           ||  is_name (argall, pHelp->keyword))
        {
            /* add seperator if found */
            if (pHelp->level >= -1 && str_cmp (argall, "imotd"))
            {
                i++;
                sprintf(buf, "%2d: ", i);
                add_buf (output, buf);
                add_buf (output, pHelp->keyword);
                add_buf (output, "\n\r");
            }

            found = TRUE;
            /* small hack :) */
            if (ch->desc != NULL && ch->desc->connected != CON_PLAYING
                && ch->desc->connected != CON_GEN_GROUPS)
                break;
        }
    }

    if (!found)
	{
        send_to_char("\n\r====================Possible Keywords=======================\n\r\n\r", ch);
        send_to_char ("No help on that word.\n\r", ch);
        send_to_char("\n\r============================================================\n\r", ch);

        append_file( ch, HELP_FILE, nohelp );

		/*
		 * Let's log unmet help requests so studious IMP's can improve their help files ;-)
		 * But to avoid idiots, we will check the length of the help request, and trim to
		 * a reasonable length (set it by redefining MAX_CMD_LEN in merc.h).  -- JR
		 */
		if (strlen(argall) > MAX_CMD_LEN)
		{
			argall[MAX_CMD_LEN - 1] = '\0';
			logf ("Excessive command length: %s requested %s.", ch, argall);
			send_to_char ("That was rude!\n\r", ch);
		}
		/* OHELPS_FILE is the "orphaned helps" files. Defined in merc.h -- JR */
		else
		{
			append_file (ch, OHELPS_FILE, argall);
		}
	}
    else
    {
      add_buf (output,
       "\n\r============================================================\n\r\n\r");
        page_to_char (buf_string (output), ch);
    }
    free_buf (output);
}


/*char *check_murd(CHAR_DATA *wch)
{
if (wch->murd > 0)
return "`1";
else
return "`7";
}*/
char *check_pk(CHAR_DATA *ch, CHAR_DATA *wch)
{
  int change;

  if (ch->level < 11)
   change = 0;
  else if (ch->level < 16)
   change = 2;
  else if (ch->level < 26)
    change = 3;
  else if (ch->level < 36)
    change = 4;
  else if (ch->level < 46)
    change = 5;
  else if (ch->level < 56)
   change = 6;
  else if (ch->level < 66)
    change = 7;
  else if (ch->level < LEVEL_IMMORTAL)
    change = 8;
  else
   change = 0;

  if (wch->level >= LEVEL_IMMORTAL)
   return "`$+`*";
  if (wch->clan == clan_lookup("tinker") || wch->clan == clan_lookup("tinker") || IS_SET(wch->act, PLR_NOPK))
    return "`$+`*";
  if (ch->level - wch->level > change)
    return "`#+`*";
  else
    return "`!+`*";
}


void do_whois (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    BUFFER *output;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    bool found = FALSE;
	const char *cClass;

    one_argument(argument,arg);

    if(arg[0] == '\0')
    {
                send_to_char("You must provide a name.\n\r",ch);
                return;
    }

    output = new_buf();

    for (d = descriptor_list; d != NULL; d = d->next)
    {
                CHAR_DATA *wch;

                if(d->connected != CON_PLAYING || !can_see(ch,d->character))
                        continue;

                wch = ( d->original != NULL ) ? d->original : d->character;

			    if (!can_see_ooc (ch, wch))
					continue;

				if(!str_prefix(arg, wch->name))
				{
					found = TRUE;

					        cClass = class_table[wch->cClass].who_name;
							switch (wch->level)
							{
								default:
									break;
								case MAX_LEVEL - 0:
										cClass = "`7 Immortal`7";
										break;
								case MAX_LEVEL - 3:
										cClass = "`7Builder`7 ";
										break;
								case MAX_LEVEL - 4:
										cClass = "`&Legend`7  ";
										break;
								case MAX_LEVEL - 5:
										cClass = "`&Legend`7  ";
										break;
								case MAX_LEVEL - 1:
										cClass = "`7Quest IM`7";
										break;
								case MAX_LEVEL - 2:
										cClass = "`7Builder`7 ";
										break;
					               
							 }
					if (strlen(wch->description) >1 && wch->description != NULL)
					{
					 sprintf(buf, "                              `&%s`7", wch->name);
					 send_to_char(buf, ch);
					 send_to_char("\n\r`&+--------------------------------------------------------------------------+`7\n\r", ch);
					 sprintf(buf, "%s", wch->description);
					 send_to_char(buf, ch);
					 send_to_char("`&+--------------------------------------------------------------------------+`7\n\r\n\r", ch);
					}
    if (wch->level > 77)
    {
                 sprintf (buf, "%s[%s ] %s%s%s%s%s%s%s%s`*%s`*%s%s%s%s%s`7%s`*\n\r",
                 check_pk(ch, wch),
                 cClass,
                 ((IS_IMMORTAL(ch) && str_cmp(clan_table[wch->clan].who_name, "None"))
                 || (IS_IMMORTAL(wch) && str_cmp(clan_table[wch->clan].who_name, "None"))
                 || (str_cmp(clan_table[wch->clan].who_name, "None")
                    && ((IS_GUILDED(wch) && IS_GUILDED(ch) && ch->clan == wch->clan)
                    || (ch->clan == clan_lookup("Warder") && wch->clan == clan_lookup("aessedai"))
                    || (ch->clan == clan_lookup("aessedai") && wch->clan == clan_lookup("Warder"))
                    || (wch->clan == clan_lookup("Furies"))))) ? clan_table[wch->clan].who_name : "",
                 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
                 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
                 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
                 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
                 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
                 wch->incog_level >= LEVEL_HERO ? "`7(`6Incog`7) " : "",
                 wch->invis_level >= LEVEL_HERO ? "`7(`5Wizi`7) " : "",
                 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
				 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
		 IS_JAILED(wch)?"(`!JAILED`*) " : "",
                 wch->desc->editor > 0 && wch->desc->editor != ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "",
                 wch->desc->editor == ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$STORE`&)`* " : "" : "",
                 IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_name : wch->name,
		 IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
                              "" : IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_title:wch->pcdata->title);
    }
    else
    {
      if (wch->level < 10)
      {
                 sprintf (buf, "%s[ `3Newbie`7   ] %s%s%s%s%s%s%s%s%s%s%s%s%s%s`7%s`*\n\r",
                 check_pk(ch, wch),
                 ((IS_IMMORTAL(ch) && str_cmp(clan_table[wch->clan].who_name, "None"))
                 || (IS_IMMORTAL(wch) && str_cmp(clan_table[wch->clan].who_name, "None"))
                 || (str_cmp(clan_table[wch->clan].who_name, "None")
                    && ((IS_GUILDED(wch) && IS_GUILDED(ch) && ch->clan == wch->clan)
                    || (ch->clan == clan_lookup("Warder") && wch->clan == clan_lookup("aessedai"))
                    || (ch->clan == clan_lookup("aessedai") && wch->clan == clan_lookup("Warder"))
                    || (wch->clan == clan_lookup("Furies"))))) ? clan_table[wch->clan].who_name : "",
                 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
                 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
                 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
                 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
                 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
                 wch->incog_level >= LEVEL_HERO ? "`7(`6I`7) " : "",
                 wch->invis_level >= LEVEL_HERO ? "`7(`5W`7)`7 " : "",
                 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
				 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
                 IS_JAILED(wch)?"(`!JAILED`*) " : "",
                 wch->desc->editor > 0 && wch->desc->editor != ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "",
                 wch->desc->editor == ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$STORE`&)`* " : "" : "",
                 IS_DISGUISED(wch)?wch->pcdata->disguise.orig_name:wch->name,
                 IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
                 "" : IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_title:wch->pcdata->title);
      }
      else
      {
				if (wch->level == 76 || wch->level == 77)
				{
					 sprintf (buf, "%s[ %s  ] %s%s%s%s%s%s%s%s`*%s`*%s%s%s%s%s`7%s`*\n\r",
					 check_pk(ch, wch),
					 cClass,
					 ((IS_IMMORTAL(ch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (IS_IMMORTAL(wch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (str_cmp(clan_table[wch->clan].who_name, "None")
						&& ((IS_GUILDED(wch) && IS_GUILDED(ch) && ch->clan == wch->clan)
                    || (ch->clan == clan_lookup("Warder") && wch->clan == clan_lookup("aessedai"))
                    || (ch->clan == clan_lookup("aessedai") && wch->clan == clan_lookup("Warder"))
						|| (wch->clan == clan_lookup("Furies"))))) ? clan_table[wch->clan].who_name : "",
					 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
					 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
					 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
					 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
					 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
					 wch->incog_level >= LEVEL_HERO ? "`7(`6Incog`7) " : "",
					 wch->invis_level >= LEVEL_HERO ? "`7(`5Wizi`7) " : "",
					 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
					 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
					 IS_JAILED(wch)?"(`!JAILED`*) " : "",
					 wch->desc->editor > 0 && wch->desc->editor != ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "",
					 wch->desc->editor == ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$STORE`&)`* " : "" : "",
					 IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_name : wch->name,
					 IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
					 "" : IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_title:wch->pcdata->title);
				}
		  		else if (wch->level == 75 || wch->level > 77)
				{
					 sprintf (buf, "%s[ `7%s`7 ] %s%s%s%s%s%s%s%s%s%s%s%s%s%s`7%s`*\n\r",
					 check_pk(ch, wch),
					 class_table[wch->cClass].who_name,
					 ((IS_IMMORTAL(ch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (IS_IMMORTAL(wch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (str_cmp(clan_table[wch->clan].who_name, "None")
						&& ((IS_GUILDED(wch) && IS_GUILDED(ch) && ch->clan == wch->clan)
                    || (ch->clan == clan_lookup("Warder") && wch->clan == clan_lookup("aessedai"))
                    || (ch->clan == clan_lookup("aessedai") && wch->clan == clan_lookup("Warder"))
						|| (wch->clan == clan_lookup("Furies"))))) ? clan_table[wch->clan].who_name : "",
					 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
					 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
					 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
					 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
					 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
					 wch->incog_level >= LEVEL_HERO ? "`7(`6I`7) " : "",
					 wch->invis_level >= LEVEL_HERO ? "`7(`5W`7)`7 " : "",
					 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
					 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
					 IS_JAILED(wch)?"(`!JAILED`*) " : "",
					 wch->desc->editor > 0 && wch->desc->editor != ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "",
					 wch->desc->editor == ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$STORE`&)`* " : "" : "",
					 IS_DISGUISED(wch)?wch->pcdata->disguise.orig_name:wch->name,
					 IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
					 "" : IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_title:wch->pcdata->title);
				}
	   		    else
				{
					 sprintf (buf, "%s[ `8%s`7 ] %s%s%s%s%s%s%s%s%s%s%s%s%s%s`7%s`*\n\r",
					 check_pk(ch, wch),
					 class_table[wch->cClass].who_name,
					 ((IS_IMMORTAL(ch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (IS_IMMORTAL(wch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (str_cmp(clan_table[wch->clan].who_name, "None")
						&& ((IS_GUILDED(wch) && IS_GUILDED(ch) && ch->clan == wch->clan)
                    || (ch->clan == clan_lookup("Warder") && wch->clan == clan_lookup("aessedai"))
                    || (ch->clan == clan_lookup("aessedai") && wch->clan == clan_lookup("Warder"))
						|| (wch->clan == clan_lookup("Furies"))))) ? clan_table[wch->clan].who_name : "",
					 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
					 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
					 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
					 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
					 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
					 wch->incog_level >= LEVEL_HERO ? "`7(`6I`7) " : "",
					 wch->invis_level >= LEVEL_HERO ? "`7(`5W`7)`7 " : "",
					 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
					 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
					 IS_JAILED(wch)?"(`!JAILED`*) " : "",
					 wch->desc->editor > 0 && wch->desc->editor != ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "",
					 wch->desc->editor == ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$STORE`&)`* " : "" : "",
					 IS_DISGUISED(wch)?wch->pcdata->disguise.orig_name:wch->name,
					 IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
					 "" : IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_title:wch->pcdata->title);
				}
				}
								 }
					 send_to_char(buf, ch);

					 sprintf(buf, "\n\r`8[ `&Level `8] `7%d\n\r`8[ `&Class `8] `7%s\n\r`8[ `&Honor `8] `7%d\n\r", wch->level, class_table[wch->cClass].name,wch->honor);
					 send_to_char(buf, ch);
				}
	}
		if (!found)
		{
			 send_to_char("Character not found.", ch);
			 return;
		}
	return;
}

void do_examine (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Examine what?\n\r", ch);
        return;
    }

    do_function (ch, &do_look, arg);

    if ((obj = get_obj_here (ch, NULL, arg)) != NULL)
    {
        switch (obj->item_type)
        {
            default:
                break;

            case ITEM_JUKEBOX:
                do_function (ch, &do_play, "list");
                break;

            case ITEM_MONEY:
                if (obj->value[0] == 0)
                {
                    if (obj->value[1] == 0)
                        sprintf (buf,
                                 "Odd...there's no coins in the pile.\n\r");
                    else if (obj->value[1] == 1)
                        sprintf (buf, "Wow. One gold coin.\n\r");
                    else
                        sprintf (buf,
                                 "There are %d gold coins in the pile.\n\r",
                                 obj->value[1]);
                }
                else if (obj->value[1] == 0)
                {
                    if (obj->value[0] == 1)
                        sprintf (buf, "Wow. One silver coin.\n\r");
                    else
                        sprintf (buf,
                                 "There are %d silver coins in the pile.\n\r",
                                 obj->value[0]);
                }
                else
                    sprintf (buf,
                             "There are %d gold and %d silver coins in the pile.\n\r",
                             obj->value[1], obj->value[0]);
                send_to_char (buf, ch);
                break;

            case ITEM_DRINK_CON:
            case ITEM_CONTAINER:
            case ITEM_CORPSE_NPC:
            case ITEM_CORPSE_PC:
                sprintf (buf, "in %s", argument);
                do_function (ch, &do_look, buf);
        }
    }

    return;
}

/* RT added back for the hell of it */
void do_read (CHAR_DATA * ch, char *argument)
{
    do_function (ch, &do_look, argument);
}

/* VECTOR WHO COMMAND */

void do_who (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    BUFFER *hiddenplayers;
    BUFFER *immortals;
    BUFFER *mortals;
    DESCRIPTOR_DATA *d;
    int iClass;
    int iRace;
    int iClan;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    int viscounter = 0;
    bool rgfClass[MAX_CLASS];
    bool rgfRace[MAX_PC_RACE];
    bool rgfClan[top_clan];
    bool fClassRestrict = FALSE;
    bool fClanRestrict = FALSE;
    bool fClan = FALSE;
    bool fRaceRestrict = FALSE;
    bool fImmortalOnly = FALSE;
    bool fTournRestrict = FALSE;
    bool fQuestRestrict = FALSE;
    bool grep = FALSE;
    int  preTitleLen;
    int  spacer;
    char arg[MSL];
//    char murd[MSL];

    argument = one_argument (argument, arg);
  
    if (!str_cmp(arg, "|"))
    {
      grep = TRUE;
      argument = one_argument (argument, arg);
    }
       
  if (IS_NPC (ch))
  {
    return; 
  }
    /*
     * Set default arguments.
     */
    iLevelLower = 0;
    iLevelUpper = MAX_LEVEL;
    for (iClass = 0; iClass < MAX_CLASS; iClass++)
        rgfClass[iClass] = FALSE;
    for (iRace = 0; iRace < MAX_PC_RACE; iRace++)
        rgfRace[iRace] = FALSE;
    for (iClan = 0; iClan < top_clan; iClan++)
        rgfClan[iClan] = FALSE;

    /*
     * Parse arguments.
     */
    nNumber = 0;
    for (;;)
    {
        if (arg[0] == '\0' || grep)
            break;

        if (is_number (arg))
        {
            switch (++nNumber)
            {
                case 1:
                    iLevelLower = atoi (arg);
                    break;
                case 2:
                    iLevelUpper = atoi (arg);
                    break;
                default:
                    send_to_char ("Only two level numbers allowed.\n\r", ch);
                    return;
            }
        }
        else
        {

            /*
             * Look for classes to turn on.
             */
            if (!str_prefix (arg, "immortals"))
            {
                fImmortalOnly = TRUE;
            }
            else if (!str_prefix (arg, "tournament"))
            {
              fTournRestrict = TRUE;
            }
            else if (!str_prefix (arg, "quest"))
            {
              fQuestRestrict = TRUE;
            }
            else if (!str_prefix (arg, "legend"))
            {
              iLevelLower = LEVEL_HERO;
              iLevelUpper = LEVEL_HERO + 1;
            }
     /*       else
            {
                iClass = -1;
                if (iClass == -1)
                {
                    iRace = 0;

                    if (iRace == 0 || iRace >= MAX_PC_RACE)
                    {
                        if (!str_prefix (arg, "clan"))
                            fClan = TRUE;
                        else
                        {
                            iClan = clan_lookup (arg);
                            if (iClan) 
                            {
                                fClanRestrict = TRUE;
                                rgfClan[iClan] = TRUE;
			    }
                            else
                            {
                                send_to_char
                                    ("That's not a valid race, class, or clan.\n\r",
                                     ch);
                                return;
                            }
                        }
                    }
                    else
                    {
               //         fRaceRestrict = TRUE;
               //         rgfRace[iRace] = TRUE;
                    }
                }
                else
                {
                    fClassRestrict = TRUE;
                    rgfClass[iClass] = TRUE;
                }
            }*/
        }
      argument = one_argument (argument, arg);
    }
  

    /*
     * Now show matching chars.
     */

    nMatch = 0;
    buf[0] = '\0';
    mortals = new_buf ();
    immortals = new_buf ();
    hiddenplayers = new_buf ();
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        CHAR_DATA *wch;
        const char *cClass;

        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if (d->connected != CON_PLAYING || !can_see (ch, d->character) || IS_SWITCHED(d->character))
            continue;

        wch = (d->original != NULL) ? d->original : d->character;

        if (!can_see_ooc (ch, wch))
		continue;

        if (wch->level < iLevelLower
            || wch->level > iLevelUpper
            || (fImmortalOnly && wch->level < LEVEL_IMMORTAL)
            || (fClassRestrict && !rgfClass[wch->cClass])
            || (fRaceRestrict && !rgfRace[wch->race])
            || (fClan && !is_clan (wch) && !IS_DRAGON(wch))
            || (grep && str_prefix(arg, wch->name))
            || (fClanRestrict && !rgfClan[wch->clan])
            || (fQuestRestrict && !IS_SET(wch->act, PLR_QUESTING))
            || (fTournRestrict && !IS_SET(wch->act, PLR_TOURNEY)))
            continue;

        nMatch++;

        /*
         * Figure out what to print for class.
         */
     
       {
        cClass = class_table[wch->cClass].who_name;
        switch (wch->level)
        {
            default:
                break;
                {
            case MAX_LEVEL - 0:
                    cClass = "`7 Immortal`7 ";
                    break;
            case MAX_LEVEL - 3:
                    cClass = "`7Builder`7  ";
                    break;
            case MAX_LEVEL - 4:
		    cClass = "`&Legend`7  ";
                    break;
            case MAX_LEVEL - 5:
                    cClass = "`&Legend`7  ";
                    break;
            case MAX_LEVEL - 1:
                    cClass = "`7Quest IM`7 ";
                    break;
            case MAX_LEVEL - 2:
                    cClass = "`7Builder`7  ";
                    break;
                
             }
         }
}

         if (IS_AFFECTED(wch, AFF_SHROUD))
         {
                viscounter += 1;
	 }

//         sprintf( buf, "There are %d hidden players.\n\r", viscounter);
//	 add_buf(hiddenplayers, buf);

         if (IS_FORSAKEN(wch))
         {
            cClass = "`1Forsaken`7";
         }

         if (IS_DRAGON(wch))
         {
            cClass = "`!Dragon`7  ";
         }

         if (wch->level >= LEVEL_IMMORTAL && !IS_NULLSTR(wch->pretit))
         {
           int front, back;
           char temp[MIL];
 
           preTitleLen = colorstrlen(wch->pretit);
           spacer = 10 - preTitleLen;
           front = spacer / 2;
           back = spacer / 2 + (spacer % 2 == 0 ? 0 : 1);               
           sprintf(temp, "%*s%s%*s", front, "", wch->pretit, back, "");
           cClass = str_dup(temp); 
         }
  
        /*
         * Format it up.
         */


// vector formatting
/*
    if (wch->level > 77)
    {
                 sprintf (buf, "%s[%s ] %s%s%s%s%s%s%s`*%s`*%s%s%s%s%s`7%s`*\n\r",
                 check_pk(ch, wch),
                 cClass,
                 ((IS_IMMORTAL(ch) && str_cmp(clan_table[wch->clan].who_name, "None"))
                 || (IS_IMMORTAL(wch) && str_cmp(clan_table[wch->clan].who_name, "None"))
                 || (str_cmp(clan_table[wch->clan].who_name, "None")
                    && ((IS_GUILDED(wch) && IS_GUILDED(ch) && ch->clan == wch->clan)
                    || (ch->clan == clan_lookup("Warder") && wch->clan == clan_lookup("aessedai"))
                    || (ch->clan == clan_lookup("aessedai") && wch->clan == clan_lookup("Warder"))
                    || (wch->clan == clan_lookup("Furies"))))) ? clan_table[wch->clan].who_name : "",
                 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
                 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
                 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
                 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
                 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
                 wch->incog_level >= LEVEL_HERO ? "`7(`6Incog`7) " : "",
                 wch->invis_level >= LEVEL_HERO ? "`7(`5Wizi`7) " : "",
                // IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
				 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
		 IS_JAILED(wch)?"(`!JAILED`*) " : "",
                 wch->desc->editor > 0 && wch->desc->editor != ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "",
                 wch->desc->editor == ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$STORE`&)`* " : "" : "",
                 IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_name : wch->name,
		 IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
                              "" : IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_title:wch->pcdata->title);
        add_buf (immortals, buf);
    }
    else
    {
*/
      if (wch->level < 10)
      {
                 sprintf (buf, "%s[ `3Newbie`7    ] %s%s%s%s%s%s%s%s%s%s%s%s%s`7%s`*\n\r",
                 check_pk(ch, wch),
                 ((IS_IMMORTAL(ch) && str_cmp(clan_table[wch->clan].who_name, "None"))
                 || (IS_IMMORTAL(wch) && str_cmp(clan_table[wch->clan].who_name, "None"))
                 || (str_cmp(clan_table[wch->clan].who_name, "None")
                    && ((IS_GUILDED(wch) && IS_GUILDED(ch) && ch->clan == wch->clan)
                    || (ch->clan == clan_lookup("Warder") && wch->clan == clan_lookup("aessedai"))
                    || (ch->clan == clan_lookup("aessedai") && wch->clan == clan_lookup("Warder"))
                    || (wch->clan == clan_lookup("Furies"))))) ? clan_table[wch->clan].who_name : "",
                 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
                 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
                 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
                 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
                 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
                 wch->incog_level >= LEVEL_HERO ? "`7(`6I`7) " : "",
                 wch->invis_level >= LEVEL_HERO ? "`7(`5W`7)`7 " : "",
                // IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
				 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
                 IS_JAILED(wch)?"(`!JAILED`*) " : "",
                 wch->desc->editor > 0 && wch->desc->editor != ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "",
                 wch->desc->editor == ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$STORE`&)`* " : "" : "",
                 IS_DISGUISED(wch)?wch->pcdata->disguise.orig_name:wch->name,
                 IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
                 "" : IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_title:wch->pcdata->title);
      }
      else
      {
				if (wch->level == 76 || wch->level == 77)
				{
					 sprintf (buf, "%s[ %s  ] %s%s%s%s%s%s%s`*%s`*%s%s%s%s%s`7%s`*\n\r",
					 check_pk(ch, wch),
					 cClass,
					 ((IS_IMMORTAL(ch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (IS_IMMORTAL(wch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (str_cmp(clan_table[wch->clan].who_name, "None")
						&& ((IS_GUILDED(wch) && IS_GUILDED(ch) && ch->clan == wch->clan)
                    || (ch->clan == clan_lookup("Warder") && wch->clan == clan_lookup("aessedai"))
                    || (ch->clan == clan_lookup("aessedai") && wch->clan == clan_lookup("Warder"))
						|| (wch->clan == clan_lookup("Furies"))))) ? clan_table[wch->clan].who_name : "",
					 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
					 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
					 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
					 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
					 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
					 wch->incog_level >= LEVEL_HERO ? "`7(`6Incog`7) " : "",
					 wch->invis_level >= LEVEL_HERO ? "`7(`5Wizi`7) " : "",
//					 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
					 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
					 IS_JAILED(wch)?"(`!JAILED`*) " : "",
					 wch->desc->editor > 0 && wch->desc->editor != ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "",
					 wch->desc->editor == ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$STORE`&)`* " : "" : "",
					 IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_name : wch->name,
					 IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
					 "" : IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_title:wch->pcdata->title);
				}
		  		else if (wch->level == 75 || wch->level > 77)
				{
					 sprintf (buf, "%s[ `7%s`7 ] %s%s%s%s%s%s%s%s%s%s%s%s%s`7%s`*\n\r",
					 check_pk(ch, wch),
					 class_table[wch->cClass].who_name,
					 ((IS_IMMORTAL(ch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (IS_IMMORTAL(wch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (str_cmp(clan_table[wch->clan].who_name, "None")
						&& ((IS_GUILDED(wch) && IS_GUILDED(ch) && ch->clan == wch->clan)
                    || (ch->clan == clan_lookup("Warder") && wch->clan == clan_lookup("aessedai"))
                    || (ch->clan == clan_lookup("aessedai") && wch->clan == clan_lookup("Warder"))
						|| (wch->clan == clan_lookup("Furies"))))) ? clan_table[wch->clan].who_name : "",
					 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
					 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
					 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
					 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
					 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
					 wch->incog_level >= LEVEL_HERO ? "`7(`6I`7) " : "",
					 wch->invis_level >= LEVEL_HERO ? "`7(`5W`7)`7 " : "",
					// IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
					 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
					 IS_JAILED(wch)?"(`!JAILED`*) " : "",
					 wch->desc->editor > 0 && wch->desc->editor != ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "",
					 wch->desc->editor == ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$STORE`&)`* " : "" : "",
					 IS_DISGUISED(wch)?wch->pcdata->disguise.orig_name:wch->name,
					 IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
					 "" : IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_title:wch->pcdata->title);
				}
	   		    else
				{
					 sprintf (buf, "%s[ `8%s`7 ] %s%s%s%s%s%s%s%s%s%s%s%s%s`7%s`*\n\r",
					 check_pk(ch, wch),
					 class_table[wch->cClass].who_name,
					 ((IS_IMMORTAL(ch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (IS_IMMORTAL(wch) && str_cmp(clan_table[wch->clan].who_name, "None"))
					 || (str_cmp(clan_table[wch->clan].who_name, "None")
						&& ((IS_GUILDED(wch) && IS_GUILDED(ch) && ch->clan == wch->clan)
                    || (ch->clan == clan_lookup("Warder") && wch->clan == clan_lookup("aessedai"))
                    || (ch->clan == clan_lookup("aessedai") && wch->clan == clan_lookup("Warder"))
						|| (wch->clan == clan_lookup("Furies"))))) ? clan_table[wch->clan].who_name : "",
					 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
					 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
					 IS_SET(wch->act2, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
					 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
					 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
					 wch->incog_level >= LEVEL_HERO ? "`7(`6I`7) " : "",
					 wch->invis_level >= LEVEL_HERO ? "`7(`5W`7)`7 " : "",
				//	 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
					 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
					 IS_JAILED(wch)?"(`!JAILED`*) " : "",
					 wch->desc->editor > 0 && wch->desc->editor != ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "",
					 wch->desc->editor == ED_STORE ? IS_IMMORTAL(ch) ? "`&(`$STORE`&)`* " : "" : "",
					 IS_DISGUISED(wch)?wch->pcdata->disguise.orig_name:wch->name,
					 IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
					 "" : IS_DISGUISED(wch) ? wch->pcdata->disguise.orig_title:wch->pcdata->title);
				}
       }
     add_buf (mortals, buf);        
     }

/*    send_to_char(  "\n\r`8                      <<<<<<<< `&Immortals`8 >>>>>>>>`*\n\r\n\r", ch);
    page_to_char (buf_string (immortals), ch);
*/
    send_to_char(  "\n\r`&Visible Players`*\n\r\n\r", ch);
    page_to_char (buf_string (mortals), ch);

    sprintf (buf2, "\n\r          `$+`*: NoPK          `#+`*: Out of Range          `!+`*: In Range`*  ");			
    send_to_char(buf2, ch);
    sprintf (buf2, "\n\r\n\r`&Players found: `!%d`7`*\n\r", nMatch);
    send_to_char(buf2, ch);

    page_to_char(buf_string(hiddenplayers), ch);

     if (xp_bonus.on)
     {
       if (!xp_bonus.negative)
         send_to_char("`&Bonus time is ON\n\r", ch);
       else
         send_to_char("`&Minus time is ON\n\r", ch);
     }
    free_buf (immortals);
    free_buf (mortals);
    free_buf (hiddenplayers);
    return;
}

/** New Who Code by Asmo **/


void do_asmo (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
    int iClass;
    int iClan;
    int iLevelLower;
    bool check;
    int length;
    int change;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    bool odd = FALSE;
    bool rgfClass[MAX_CLASS];
    bool rgfClan[top_clan];
    bool fClassRestrict = FALSE;
    bool fClanRestrict = FALSE;
    bool fClan = FALSE;
    bool fImmortalOnly = FALSE;
    bool grep = FALSE;
    char arg[MAX_STRING_LENGTH];

    argument = one_argument (argument, arg);

    if (!str_cmp(arg, "|"))
    {
      grep = TRUE;
      argument = one_argument (argument, arg);
    }

  if (IS_NPC (ch))
{
return;
}
    /*
     * Set default arguments.
     */
    iLevelLower = 0;
    iLevelUpper = MAX_LEVEL;
    for (iClass = 0; iClass < MAX_CLASS; iClass++)
        rgfClass[iClass] = FALSE;
    for (iClan = 0; iClan < top_clan; iClan++)
        rgfClan[iClan] = FALSE;

    /*
     * Parse arguments.
     */
    nNumber = 0;
    for (;;)
    {

        if (arg[0] == '\0' || grep)
            break;

        if (is_number (arg))
        {
            switch (++nNumber)
            {
                case 1:
                    iLevelLower = atoi (arg);
                    break;
                case 2:
                    iLevelUpper = atoi (arg);
                    break;
                default:
                    send_to_char ("Only two level numbers allowed.\n\r", ch);
                    return;
            }
        }
        else
        {

            /*
             * Look for classes to turn on.
             */
            if (!str_prefix (arg, "immortals"))
            {
                fImmortalOnly = TRUE;
            }
            else
            {
                iClass = class_lookup (arg);
                if (iClass == -1)
                {
                    if (!str_prefix (arg, "clan"))
                       fClan = TRUE;
                    else
                    {
                       iClan = clan_lookup (arg);
                       if (iClan)
                       {
                          fClanRestrict = TRUE;
                          rgfClan[iClan] = TRUE;
                       }
                       else
                       {
                          send_to_char
                         ("That's not a valid race, class, or clan.\n\r",
                                 ch);
                          return;
                       }
                    }
                }
                else
                {
                    fClassRestrict = TRUE;
                    rgfClass[iClass] = TRUE;
                }
            }
        }
      argument = one_argument (argument, arg);
    }

    /*
     * Now show matching chars.
     */

     send_to_char("`7_-_-_-_-_-_-_-_-_-_-_-_- Fall of Ten Nations `7 -_-_-_-_-_-_-_-_-_-_-_-_-_-_\n\r", ch);
     send_to_char("`8[`1  Lv Cl  RLv`8 ][`!PK`*]   `4[`&Guild`4]`*       Name\n\r", ch);
     if (xp_bonus.on)
     {
       if (!xp_bonus.negative)
         send_to_char("                          `&Bonus time is ON\n\r", ch);
       else
         send_to_char("                          `&Minus time is ON\n\r", ch);
     }

   send_to_char("\n\r", ch);

    nMatch = 0;
    buf[0] = '\0';
    output = new_buf ();
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        CHAR_DATA *wch;
        char const *cClass;

        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if (d->connected != CON_PLAYING || !can_see (ch, d->character) || IS_SWITCHED(d->character))
            continue;

        wch = (d->original != NULL) ? d->original : d->character;

        if (!can_see (ch, wch))
            continue;

        if (wch->level < iLevelLower
            || wch->level > iLevelUpper
            || (fImmortalOnly && wch->level < LEVEL_IMMORTAL)
            || (fClassRestrict && !rgfClass[wch->cClass])
            || (fClan && !is_clan (wch))
            || (grep && str_prefix(arg, wch->name))
            || (fClanRestrict && !rgfClan[wch->clan]))
            continue;

        nMatch++;

        /*
         * Figure out what to print for class.
         */
        cClass = class_table[wch->cClass].who_name;
        switch (wch->level)
        {
            default:
                break;
                {
            case MAX_LEVEL - 0:
                    cClass = "`1IMPLEMENTOR`7";
                    break;
            case MAX_LEVEL - 3:
                    cClass = "  `#BUILDER`7  ";
                    break;
            case MAX_LEVEL - 4:
                    cClass = "`2GUILDLEADER`7";
                    break;
            case MAX_LEVEL - 5:
                    cClass = "  `3LEGEND`7   ";
                    break;
            case MAX_LEVEL - 1:
                    cClass = "`4 CREATOR`7   ";
                    break;
            case MAX_LEVEL - 2:
                    cClass = " `8SUPREME`7   ";
                    break;

}
}

        /*
         * Format it up.
         */

if (ch->level < 11)
  change = 0;
else if (ch->level < 16)
  change = 2;
else if (ch->level < 26)
  change = 3;
else if (ch->level < 36)
  change = 4;
else if (ch->level < 46)
  change = 5;
else if (ch->level < 56)
  change = 6;
else if (ch->level < 66)
  change = 7;
else if (ch->level <= 76)
  change = 8;
else
  change = 0;


  if (ch->level - wch->level > change || wch->level < 11)
    {check = FALSE;}
  else
    {check = TRUE;}
  if (wch->level > 75)
   {check = FALSE;}
 if (wch->clan == clan_lookup("tinker"))
   {check = FALSE;}
 if (wch->clan == clan_lookup("jenn"))
   {check = FALSE;}

  odd = FALSE;
  if (wch->clan != 0)
    length = (15 - colorstrlen(clan_table[wch->clan].who_name)) / 2;
  else
  {
    length = 7;
  }

  if ((15 - colorstrlen(clan_table[wch->clan].who_name)) % 2 != 0)
    odd = TRUE;

if (wch->level > 75)
{

                 sprintf (buf, "`8[ %s`8 ]`7[`!*`*] %*s%s%*s`4| `*%s%s`*\n\r",
                 cClass,
                 length,
                 "",
                 wch->clan == 0 ? "" : clan_table[wch->clan].who_name,
                 odd ? length  : length - 1,
                 "",
  /*              SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
                 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
                 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
                 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
                 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
                 wch->incog_level >= LEVEL_HERO ? "`7(`6I`7) " : "",
                 wch->invis_level >= LEVEL_HERO ? "`7(`5W`7) " : "",
                 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
                 wch->desc->editor > 0 ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "", */
                 IS_DISGUISED(wch)?wch->pcdata->disguise.orig_name:wch->name, IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
                              "" : IS_DISGUISED(wch)?wch->pcdata->disguise.orig_title:wch->pcdata->title);
        add_buf (output, buf);

}
else if (wch->level <= 75)

    {
                 sprintf (buf, "`8[`1++`7%2d %s %2d`1++`8]`7[`!%s`*] %*s%s%*s`4|`* %s%s\n\r",
                 wch->level,
                 cClass,
                 wch->rplevel,
                 check ? "*" : " ",
                 length,
                 "",
                 wch->clan == 0 ? "" : clan_table[wch->clan].who_name,
                 odd ? length  : length - 1,
                 "",
/*                 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
                 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
                 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
                 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
                 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
                 wch->incog_level >= LEVEL_HERO ? "`7(`6I`7) " : "",
                 wch->invis_level >= LEVEL_HERO ? "`7(`5W`7)`7 " : "",

                 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
                 wch->desc->editor > 0 ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "", */
                 wch->name, IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
                              "" : wch->pcdata->title);
        add_buf (output, buf);

   }
else

    {
                 sprintf (buf, "`8[`1++`7%2d %s %2d`1++`8]`7[`!%s`7] %s%s%s%s%s`*%s%s`*%s%s%s%s%s`*\n\r",
                 wch->level,
                 cClass,
                 wch->rplevel,
                 check ? "*" : " ",
                 SHOW_DF(ch, wch) ? "`1[`8DF`1]`* " : "",
                 IS_SET(wch->act, PLR_QUESTING) ? "`$(`&QUEST`$)`* " : "",
                 IS_SET(wch->act, PLR_TOURNEY) ? "`&(`#TOURNEY`&)`* " : "",
                 IS_SET(wch->act, PLR_WAR) ? "`8(`&W`7A`&R`8)`* " : "",
                 IS_SET(wch->act, PLR_TAG) ? "`8(`!TAG`8)`7 " : "",
                 IS_SET(wch->act, PLR_IT) ? " `8(`$IT`8)`7 " : "",
                 wch->incog_level >= LEVEL_HERO ? "`7(`6I`7) " : "",
                 wch->invis_level >= LEVEL_HERO ? "`7(`5W`7)`7 " : "",

                 IS_SET (wch->comm, COMM_AFK) ? "`7(`^AFK`7) " : "",
                 wch->desc->editor > 0 ? IS_IMMORTAL(ch) ? "`&(`$OLC`&)`* " : "" : "",
                 wch->name, IS_NPC (wch) ? "" : IS_SET(ch->act2, PLR_NOTITLE) ?
                              "" : wch->pcdata->title);

        add_buf (output, buf);

   }
}

    sprintf (buf2, "\n\r_-_-_-_-_-_-_-_-_-_-_-_-_- Players found: `!%d`7 -_-_-_-_-_-_-_-_-_-_-_-_-_-_\n\r", nMatch);
    add_buf (output, buf2);
    page_to_char (buf_string (output), ch);
    free_buf (output);
    return;
}

/** End New Who Code **/


void do_count (CHAR_DATA * ch, char *argument)
{
    int count;
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];

    count = 0;

    for (d = descriptor_list; d != NULL; d = d->next)
        if (d->connected == CON_PLAYING && can_see (ch, d->character))
            count++;

    max_on = UMAX (count, max_on);

    if (max_on == count)
        sprintf (buf,
                 "There are %d characters on, the most so far today.\n\r",
                 count);
    else
        sprintf (buf,
                 "There are %d characters on, the most on today was %d.\n\r",
                 count, max_on);

    send_to_char (buf, ch);
}

void do_inventory (CHAR_DATA * ch, char *argument)
{
    send_to_char ("You are carrying:\n\r", ch);
    show_list_to_char (ch->carrying, ch, TRUE, TRUE, ON_INVENTORY);
    return;
}



void do_equipment (CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj;
    int iWear;
    bool found;
    bool all = FALSE;


    send_to_char ("You are using:\n\r", ch);
    found = FALSE;

    if (!str_cmp(argument, "all"))
      all = TRUE;

    for (iWear = 0; iWear < MAX_WEAR; iWear++)
    {
        if (((obj = get_eq_char (ch, iWear)) == NULL) && !all)
            continue;

        send_to_char (where_name[iWear], ch);
        if (!obj)
        {
          send_to_char ("Nothing\n\r", ch);
          found = TRUE;
          continue;
        }

        if (can_see_obj (ch, obj))
        {
            send_to_char (format_obj_to_char (obj, ch, TRUE), ch);
            send_to_char ("\n\r", ch);
        }
        else
        {
            send_to_char ("something.\n\r", ch);
        }
        found = TRUE;
    }

    if (!found)
        send_to_char ("Nothing.\n\r", ch);

    return;
}



void do_compare (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    int points1;
    int points2;
    char *msg;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);
    if (arg1[0] == '\0')
    {
        send_to_char ("Compare what to what?\n\r", ch);
        return;
    }

    if ((obj1 = get_obj_carry (ch, arg1, ch)) == NULL)
    {
        send_to_char ("You do not have that item.\n\r", ch);
        return;
    }

    if (arg2[0] == '\0')
    {
        for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
        {
            if (obj2->wear_loc != WEAR_NONE && can_see_obj (ch, obj2)
                && obj1->item_type == obj2->item_type
                && (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0)
                break;
        }

        if (obj2 == NULL)
        {
            send_to_char ("You aren't wearing anything comparable.\n\r", ch);
            return;
        }
    }

    else if ((obj2 = get_obj_carry (ch, arg2, ch)) == NULL)
    {
        send_to_char ("You do not have that item.\n\r", ch);
        return;
    }

    msg = NULL;
    value1 = 0;
    value2 = 0;
    points1 = 0;
    points2 = 0;

    if (obj1 == obj2)
    {
        msg = "You compare $p to itself.  It looks about the same.";
    }
    else if (obj1->item_type != obj2->item_type)
    {
        msg = "You can't compare $p and $P.";
    }
    else
    {
        points1 = calculate_points(obj1->pIndexData);
        points2 = calculate_points(obj2->pIndexData);

        switch (obj1->item_type)
        {
            default:
                msg = "You can't compare $p and $P.";
                break;

            case ITEM_ARMOR:
                value1 = obj1->value[0] + obj1->value[1] + obj1->value[2] + obj1->value[3];
                value2 = obj2->value[0] + obj2->value[1] + obj2->value[2] + obj2->value[3];
                break;

            case ITEM_WEAPON:
                if (obj1->pIndexData->new_format)
                    value1 = (1 + obj1->value[2]) * obj1->value[1];
                else
                    value1 = obj1->value[1] + obj1->value[2];

                if (obj2->pIndexData->new_format)
                    value2 = (1 + obj2->value[2]) * obj2->value[1];
                else
                    value2 = obj2->value[1] + obj2->value[2];
                break;
        }
    }

    if (msg == NULL)
    {
        value1 = int(value1 * .75 + points1);
        value2 = int(value2 * .75 + points2);
        if (value1 == value2)
            msg = "$p and $P look about the same.";
        else if (value1 > value2)
            msg = "$p looks better than $P.";
        else
            msg = "$p looks worse than $P.";
    }

    act (msg, ch, obj1, obj2, TO_CHAR);
    return;
}



void do_credits (CHAR_DATA * ch, char *argument)
{
    do_function (ch, &do_mhelp, "* diku");
    return;
}



void do_where (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;


    one_argument (argument, arg);

    //Added Jan8 2003 - Diem
    sprintf(buf, "You are currently in %s\n\r", ch->in_room->area->name);
    send_to_char(buf, ch);

    if (arg[0] == '\0')
    {
        send_to_char ("Players near you:\n\r", ch);
        found = FALSE;
        for (d = descriptor_list; d; d = d->next)
        {
            if (d->connected == CON_PLAYING
                && (victim = d->character) != NULL && !IS_NPC (victim)
                && victim->in_room != NULL
                && !IS_SET (victim->in_room->room_flags, ROOM_NOWHERE)
                && (is_room_owner (ch, victim->in_room)
                    || !room_is_private (victim->in_room))
                && victim->in_room->area == ch->in_room->area
                && can_see (ch, victim))
            {
                found = TRUE;
                sprintf (buf, "%-28s %s\n\r",
                         IS_DISGUISED(victim)?victim->pcdata->disguise.orig_name:
				    PERS(victim, ch, FALSE), victim->in_room->name);
                send_to_char (buf, ch);
            }
        }
        if (!found)
            send_to_char ("None\n\r", ch);
    }
    else
    {
        found = FALSE;
        for (victim = char_list; victim != NULL; victim = victim->next)
        {
            if (victim->in_room != NULL
                && victim->in_room->area == ch->in_room->area
                && !IS_AFFECTED (victim, AFF_HIDE)
                && !IS_AFFECTED (victim, AFF_SHROUD)
                && can_see (ch, victim) && is_name (arg, PERS(victim, ch, FALSE)))
            {
                found = TRUE;
                sprintf (buf, "%-28s %s\n\r",
                         PERS (victim, ch, FALSE), victim->in_room->name);
                send_to_char (buf, ch);
                break;
            }
        }
        if (!found)
            act ("You didn't find any $T.", ch, NULL, arg, TO_CHAR);
    }

    return;
}




void do_consider (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
	/*char *msg2;*/
    int diff;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Consider killing whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They're not here.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
    {
        send_to_char ("Don't even think about it.\n\r", ch);
        return;
    }

    diff = victim->level - ch->level;

/*	if (victim->alignment > 900)
        msg2 = "$N is orderly and of a lawful nature.";
    else if (victim->alignment > 700)
		msg2 = "$N appears to be a nice law-abiding citizen.";
    else if (victim->alignment > 350)
        msg2 = "$N looks of a good-nature.";
    else if (victim->alignment > 100)
        msg2 = "$N looks kind and gentle.";
   else if (victim->alignment == 0)
        msg2 = "You can't tell which side of good or evil this person tends to...";
    else if (victim->alignment > -100)
        msg2 = "$N looks mean and nasty.";
    else if (victim->alignment > -350)
        msg2 = "$N looks of a chaotic-nature.";
    else if (victim->alignment > -700)
        msg2 = "$N appears to be a very destructive and chaotic creature.";
    else
        msg2 = "$N could feel confortable with the friends of the dark.";
 */
    if (diff <= -10)
        msg = "You can kill $N naked and weaponless.";
    else if (diff <= -5)
        msg = "$N is no match for you.";
    else if (diff <= -2)
        msg = "$N looks like an easy kill.";
    else if (diff <= 1)
        msg = "The perfect match!";
    else if (diff <= 4)
        msg = "$N says 'Do you feel lucky, punk?'.";
    else if (diff <= 9)
        msg = "$N laughs at you mercilessly.";
    else
        msg = "Death will thank you for your gift.";

        act (msg, ch, NULL, victim, TO_CHAR);
    return;
}



void set_title (CHAR_DATA * ch, char *title)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC (ch))
    {
        bug ("Set_title: NPC.", 0);
        return;
    }

    if (title[0] != '.' && title[0] != ',' && title[0] != '!'
        && title[0] != '?')
    {
        buf[0] = ' ';
        strcpy (buf + 1, title);
    }
    else
    {
        strcpy (buf, title);
    }

    title[strlen(title) + 1] = '\0';
    free_string (ch->pcdata->title);
    ch->pcdata->title = str_dup (buf);
    return;
}



void do_title (CHAR_DATA * ch, char *argument)
{
    //int i;

    if (IS_NPC (ch))
        return;

    if IS_SET(ch->comm, COMM_NOEMOTE)
    {
        send_to_char("You are unable to change your title.\n\r", ch);
        return;
    }
    if (argument[0] == '\0')
    {
        send_to_char ("Change your title to what?\n\r", ch);
        return;
    }

    if (colorstrlen (argument) > 55)
    {
      send_to_char("Title too long.\n\r", ch);
      return;
    }

    strcat(argument, "`*");

    smash_tilde (argument);
    set_title (ch, argument);
    send_to_char ("Ok.\n\r", ch);
}



void do_description (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (!str_cmp(argument, "edit"))
    {
      string_append(ch, &ch->description);
      if (strlen(ch->description) > 2048)
      {
        send_to_char("Description too long, truncated.\n\r", ch);
        ch->description[1025] = '\0';
      }
      return;
    }

    if (!str_cmp(argument, "format"))
    {
      ch->description = format_string(ch->description);
      send_to_char("Description formateed.\n\r", ch);
      return;
    }

    if (argument[0] != '\0')
    {
        buf[0] = '\0';
        smash_tilde (argument);

        if (argument[0] == '-')
        {
            int len;
            bool found = FALSE;

            if (ch->description == NULL || ch->description[0] == '\0')
            {
                send_to_char ("No lines left to remove.\n\r", ch);
                return;
            }

            strcpy (buf, ch->description);

            for (len = colorstrlen (buf); len > 0; len--)
            {
                if (buf[len] == '\r')
                {
                    if (!found)
                    {            /* back it up */
                        if (len > 0)
                            len--;
                        found = TRUE;
                    }
                    else
                    {            /* found the second one */

                        buf[len + 1] = '\0';
                        free_string (ch->description);
                        ch->description = str_dup (buf);
                        send_to_char ("Your description is:\n\r", ch);
                        send_to_char (ch->description ? ch->description :
                                      "(None).\n\r", ch);
                        return;
                    }
                }
            }
            buf[0] = '\0';
            free_string (ch->description);
            ch->description = str_dup (buf);
            send_to_char ("Description cleared.\n\r", ch);
            free_string(buf);
            return;
        }
        if (argument[0] == '+')
        {
            if (ch->description != NULL)
                strcat (buf, ch->description);
            argument++;
            while (isspace (*argument))
                argument++;
        }

        if (strlen (buf) >= 2048)
        {
            send_to_char ("Description too long.\n\r", ch);
            return;
        }

        strcat (buf, argument);
        strcat (buf, "\n\r");
        free_string (ch->description);
        ch->description = str_dup (buf);
    }

    send_to_char ("Your description is:\n\r", ch);
    send_to_char (ch->description ? ch->description : "(None).\n\r", ch);
    return;
}



void do_report (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];

    sprintf (buf,
             "You say 'I have %ld/%ld hp %d/%d mana %d/%d mv %d xp.'\n\r",
             ch->hit, ch->max_hit,
             ch->mana, ch->max_mana, ch->move, ch->max_move, ch->exp);

    send_to_char (buf, ch);

    sprintf (buf, "$n says 'I have %ld/%ld hp %d/%d mana %d/%d mv %d xp.'",
             ch->hit, ch->max_hit,
             ch->mana, ch->max_mana, ch->move, ch->max_move, ch->exp);

    act (buf, ch, NULL, NULL, TO_ROOM);

    return;
}



void do_practice (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int sn;
    char arg1[MIL];
    char arg2[MIL];

    if (IS_NPC (ch))
        return;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0')
    {
        int col;

        col = 0;
        for (sn = 0; sn < MAX_SKILL; sn++)
        {
            if (skill_table[sn].name == NULL)
                 break;
            if (ch->level < skill_table[sn].skill_level[ch->cClass]
               || ch->pcdata->learned[sn] < 1 /* skill is not known */
	       || skill_table[sn].rating[ch->cClass] == 0)
                 continue;

            sprintf (buf, "`$%-18s `7%3d%%  ",
                     skill_table[sn].name, ch->pcdata->learned[sn]);
            send_to_char (buf, ch);
            if (++col % 3 == 0)
                send_to_char ("\n\r", ch);
        }

        if (col % 3 != 0)
            send_to_char ("\n\r", ch);

        sprintf (buf, "`$You have `^%d `$practice sessions left`4.`*\n\r",
                 ch->practice);
        send_to_char (buf, ch);
    }
    else if (!str_cmp(arg1, "<"))
    {
        int col;

        col = 0;
        for (sn = 0; sn < MAX_SKILL; sn++)
        {
            if (skill_table[sn].name == NULL)
                 break;
            if (ch->level < skill_table[sn].skill_level[ch->cClass]
               || ch->pcdata->learned[sn] < 1 /* skill is not known */
		   || skill_table[sn].rating[ch->cClass] == 0)
                 continue;

            sprintf (buf, "`$%-18s `7%3d%%  ",
                     skill_table[sn].name, ch->pcdata->learned[sn]);
            send_to_char (buf, ch);
            if (++col % 3 == 0)
                send_to_char ("\n\r", ch);
        }

        if (col % 3 != 0)
            send_to_char ("\n\r", ch);

        sprintf (buf, "`$You have `^%d practice sessions left`4.`*\n\r",
                 ch->practice);
        send_to_char (buf, ch);
    }
    else if (!str_cmp(arg1, "|"))
    {
        int col;

        col = 0;
        for (sn = 0; sn < MAX_SKILL; sn++)
        {
            if (skill_table[sn].name == NULL)
                 break;
            if (ch->level < skill_table[sn].skill_level[ch->cClass]
             || ch->pcdata->learned[sn] < 1 /* skill is not known */
		 || skill_table[sn].rating[ch->cClass] == 0)
                 continue;

            sprintf (buf, "%-18s %3d%%  ",
                     skill_table[sn].name, ch->pcdata->learned[sn]);
            send_to_char (buf, ch);
            if (++col % 3 == 0)
                send_to_char ("\n\r", ch);
        }

        if (col % 3 != 0)
            send_to_char ("\n\r", ch);

        sprintf (buf, "You have %d practice sessions left.\n\r",
                 ch->practice);
        send_to_char (buf, ch);
    }

    else
    {
        CHAR_DATA *mob;
        int adept;

        if (!IS_AWAKE (ch))
        {
            send_to_char ("In your dreams, or what?\n\r", ch);
            return;
        }

        for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)
        {
            if (IS_NPC (mob) && IS_SET (mob->act, ACT_PRACTICE))
                break;
        }

        if (mob == NULL)
        {
            send_to_char ("You can't do that here.\n\r", ch);
            return;
        }

        if ((sn = find_spell (ch, arg1)) < 0 || (!IS_NPC (ch)
         && (ch->level < skill_table[sn].skill_level[ch->cClass]
          || ch->pcdata->learned[sn] < 1 /* skill is not known */
          || skill_table[sn].rating[ch->cClass] == 0)))
        {
            send_to_char ("You can't practice that.\n\r", ch);
            return;
        }

        adept = IS_NPC (ch) ? 100 : class_table[ch->cClass].skill_adept;

        if (ch->pcdata->learned[sn] >= adept)
        {
            if(ch->pcdata->learned[sn] >= 100)
            {
                sprintf(buf, "You are already a master at %s.\n\r", skill_table[sn].name);
                send_to_char(buf, ch);
                return;
            }
            if(ch->gold < ch->pcdata->learned[sn]/2)
            {
                sprintf (buf, "You need %d gold to practice %s.\n\r",
                     ch->pcdata->learned[sn], skill_table[sn].name);
                send_to_char (buf, ch);
                return;
            }
            ch->gold -= ch->pcdata->learned[sn]/2;
            ch->pcdata->learned[sn]++;
            sprintf (buf, "You get a bit better at %s.\n\r",
                     skill_table[sn].name);
            send_to_char (buf, ch);
        }
        else
        {
            int div;
            if (ch->practice <= 0)
            {
                send_to_char ("You have no practice sessions left.\n\r", ch);
                return;
            }
            if (skill_table[sn].rating[ch->cClass] < 0)
              div = skill_table[sn].rating[ch->cClass] * -1;
            else
              div = skill_table[sn].rating[ch->cClass];
            if(!str_cmp(arg2, "all"))
            {
                int iprac = ((adept - ch->pcdata->learned[sn]) / (int_app[get_curr_stat(ch, STAT_INT)].learn / div));
                if(iprac <= 0)
                {
                    send_to_char("Cannot practice with 'all' anymore.\n\r", ch);
                    return;
                }
                if(iprac > ch->practice)
                    iprac = ch->practice;
                ch->pcdata->learned[sn] += (iprac * (
                    int_app[get_curr_stat (ch, STAT_INT)].learn /
                    div));
                ch->practice -= iprac;
                act ("You practice $T.",
                     ch, NULL, skill_table[sn].name, TO_CHAR);
                act ("$n practices $T.",
                     ch, NULL, skill_table[sn].name, TO_ROOM);
                return;
            }
            ch->practice--;
            ch->pcdata->learned[sn] +=
                int_app[get_curr_stat (ch, STAT_INT)].learn /
                div;
            if (ch->pcdata->learned[sn] < adept)
            {
                act ("You practice $T.",
                     ch, NULL, skill_table[sn].name, TO_CHAR);
                act ("$n practices $T.",
                     ch, NULL, skill_table[sn].name, TO_ROOM);
            }
            else
            {
                ch->pcdata->learned[sn] = adept;
                act ("You are now learned at $T.",
                     ch, NULL, skill_table[sn].name, TO_CHAR);
                act ("$n is now learned at $T.",
                     ch, NULL, skill_table[sn].name, TO_ROOM);
            }
        }
    }
    return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument (argument, arg);

    if (arg[0] == '\0')
        wimpy = ch->max_hit / 5;
    else
        wimpy = atoi (arg);

    if (wimpy < 0)
    {
        send_to_char ("Your courage exceeds your wisdom.\n\r", ch);
        return;
    }

    if (wimpy > ch->max_hit / 2)
    {
        send_to_char ("Such cowardice ill becomes you.\n\r", ch);
        return;
    }

    ch->wimpy = wimpy;
    sprintf (buf, "Wimpy set to %d hit points.\n\r", wimpy);
    send_to_char (buf, ch);
    return;
}



void do_password (CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if (IS_NPC (ch))
        return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while (isspace (*argument))
        argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
        cEnd = *argument++;

    while (*argument != '\0')
    {
        if (*argument == cEnd)
        {
            argument++;
            break;
        }
        *pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while (isspace (*argument))
        argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
        cEnd = *argument++;

    while (*argument != '\0')
    {
        if (*argument == cEnd)
        {
            argument++;
            break;
        }
        *pArg++ = *argument++;
    }
    *pArg = '\0';

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char ("Syntax: password <old> <new>.\n\r", ch);
        return;
    }

    if (strcmp (crypt ((const char *) arg1, (const char *) ch->pcdata->pwd), ch->pcdata->pwd))
    {
        WAIT_STATE (ch, 40);
        send_to_char ("Wrong password.  Wait 10 seconds.\n\r", ch);
        return;
    }

    if (strlen (arg2) < 5)
    {
        send_to_char
            ("New password must be at least five characters long.\n\r", ch);
        return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt (arg2, ch->name);
    for (p = pwdnew; *p != '\0'; p++)
    {
        if (*p == '~')
        {
            send_to_char ("New password not acceptable, try again.\n\r", ch);
            return;
        }
    }

    free_string (ch->pcdata->pwd);
    ch->pcdata->pwd = str_dup (pwdnew);
    save_char_obj (ch);
    send_to_char ("Ok.\n\r", ch);
    return;
}

void do_ddiem( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  int i;

  if (IS_NPC(ch))
    {
      send_to_char("Use the OLD score function\n\r",ch);
      return;
    }

  send_to_char("`3------------------------------------------------------------------`7\n\r",ch);
  printf_to_char(ch,
		 "{`7You are `&%s%s`7, level `&%d`7, `&%d `7years old (`&%d`7 hours).\n\r",
		 ch->name,
		 IS_NPC(ch) ? "" : ch->pcdata->title,
		 ch->level, get_age(ch),
		 ( ch->played + (int) (current_time - ch->logon) ) / 3600);


  if ( get_trust( ch ) != ch->level )
    {
      printf_to_char(ch, "`7You are trusted at level `&%d{c.\n\r",
		     get_trust( ch ) );
    }

  printf_to_char(ch, "`7Nationality: `&%s  `7Sex: `&%s  `7Class: `&%s\n\r",
		 race_table[ch->race].name,
		 ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
		 IS_NPC(ch) ? "mobile" : class_table[ch->cClass].name);

  printf_to_char(ch,
		 "`7Str: `&%d(%d)  `7Int: `&%d(%d)  `7Wis: `&%d(%d)  `7Dex: `&%d(%d)  `7Con: `&%d(%d)\n\r",
		 ch->perm_stat[STAT_STR],
		 get_curr_stat(ch,STAT_STR),
		 ch->perm_stat[STAT_INT],
		 get_curr_stat(ch,STAT_INT),
		 ch->perm_stat[STAT_WIS],
		 get_curr_stat(ch,STAT_WIS),
		 ch->perm_stat[STAT_DEX],
		 get_curr_stat(ch,STAT_DEX),
		 ch->perm_stat[STAT_CON],
		 get_curr_stat(ch,STAT_CON) );

  if ( ch->level >= 15 )
    {
      printf_to_char(ch,
		     "`7ToHit: `&%-9d                            `7ToDam: `&%d\n\r",
		     GET_HITROLL(ch), GET_DAMROLL(ch) );
    }
  printf_to_char(ch,
		 "`7HP: `&%5d/%-5d                             `7Pracs: `&%d \n\r",
		 ch->hit,  ch->max_hit,
		 ch->practice);

  printf_to_char(ch,
		 "`7MANA: `&%5d/%-5d                           `7Trains: `&%d \n\r",
		 ch->mana, ch->max_mana,
		 ch->train);
  printf_to_char(ch,
		 "`7MOVE: `&%5d/%-5d                           `7Items: `&%d/%d \n\r",
		 ch->move, ch->max_move,
		 ch->carry_number, can_carry_n(ch));
  printf_to_char(ch,
		 "`7Glory: `&%-9ld                            `7Weight: `&%d/%d \n\r",
		 ch->questpoints,
		 get_carry_weight(ch) / 10, can_carry_w(ch) /10 );

  /*
   * This section (exp, gold, balance and shares) was written by
   * The Maniac from Mythran Mud
   */
  if (IS_NPC(ch))             /* NPC's have no bank balance and shares */
    {                           /* and don't level !!       -- Maniac -- */
      printf_to_char(ch,
		     "You have scored %d exp, and have %ld gold coins\n\r",
		     ch->exp,  ch->gold );
    }

  if (!IS_NPC(ch))            /* PC's do level and can have bank accounts */
    {                           /* HERO's don't level anymore               */
      printf_to_char(ch,
		     "`7Exp: `&%-6d       `7TNL: `&%-7d       `7Wimpy: `&%d\n\r",
		     ch->exp,IS_HERO(ch) ? (1) : (ch->level + 1) *
		     exp_per_level(ch,ch->pcdata->points) - ch->exp,
		     ch->wimpy);

      printf_to_char(ch,
		     "`7Gold: `&%-6d     `7Silver: `&%d\n\r",
		     ch->gold, ch->silver);

    }

  send_to_char("`7Condition(S): ",ch);
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
    send_to_char( "`&Drunk ",   ch );
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
    send_to_char( "`&Thirsty ", ch );
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER]   ==  0 )
    send_to_char( "`&Hungry`7",  ch );
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_BLEEDING] != 0 )
    send_to_char( "`!B`1leeding`* ", ch);
  send_to_char(".\n\r",ch);

  send_to_char("`7Position: ",ch);
  switch ( ch->position )
    {
    case POS_DEAD:
      send_to_char( "`!DEAD`7!!\n\r",ch );
      break;
    case POS_MORTAL:
      send_to_char( "`&Mortally wounded`7.\n\r",ch );
      break;
    case POS_INCAP:
      send_to_char( "`&Incapacitated`7.\n\r",ch );
      break;
    case POS_STUNNED:
      send_to_char( "`&stunned`7.\n\r",ch );
      break;
    case POS_SLEEPING:
      send_to_char( "`&Sleeping`7.\n\r",ch );
      break;
    case POS_RESTING:
      send_to_char( "`&Resting`7.\n\r",ch );
      break;
    case POS_SITTING:
      send_to_char( "`&Sitting`7.\n\r",ch );
      break;
    case POS_STANDING:
      send_to_char( "`&Standing`7.\n\r",ch );
      break;
    case POS_FIGHTING:
      send_to_char( "`&Fighting`7.\n\r",ch );
      break;
    }
  /* print AC values */
  if (ch->level >= 25)
    {
      printf_to_char(ch,"`7Armor: pierce: `&%d  `7bash: `&%d  `7slash: `&%d  `7magic: `&%d\n\r",
		     GET_AC(ch,AC_PIERCE),
		     GET_AC(ch,AC_BASH),
		     GET_AC(ch,AC_SLASH),
		     GET_AC(ch,AC_EXOTIC));
    } else {
      for (i = 0; i < 4; i++)
	{
	  char * temp;

	  switch(i)
	    {
	    case(AC_PIERCE):	temp = "piercing";	break;
	    case(AC_BASH):	temp = "bashing";	break;
	    case(AC_SLASH):	temp = "slashing";	break;
	    case(AC_EXOTIC):	temp = "magic";		break;
	    default:		temp = "error";		break;
	    }

	  send_to_char("You are ", ch);

	  if      (GET_AC(ch,i) >=  101 )
	    sprintf(buf,"hopelessly vulnerable to %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= 80)
	    sprintf(buf,"defenseless against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= 60)
	    sprintf(buf,"barely protected from %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= 40)
	    sprintf(buf,"slightly armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= 20)
	    sprintf(buf,"somewhat armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= 0)
	    sprintf(buf,"armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= -20)
	    sprintf(buf,"well-armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= -40)
	    sprintf(buf,"very well-armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= -60)
	    sprintf(buf,"heavily armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= -80)
	    sprintf(buf,"superbly armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= -100)
	    sprintf(buf,"almost invulnerable to %s.\n\r",temp);
	  else
	    sprintf(buf,"divinely armored against %s.\n\r",temp);

	  send_to_char(buf,ch);
	}
    }

  /* RT wizinvis and holy light */
  if ( IS_IMMORTAL(ch))
    {
      send_to_char("`7Holy Light: ",ch);
      if (IS_SET(ch->act,PLR_HOLYLIGHT))
	send_to_char("`&On",ch);
      else
	send_to_char("`&Off",ch);

      if (ch->invis_level)
	{
	  sprintf(buf, " `7 Invisible: level `&%d",ch->invis_level);
	  send_to_char(buf,ch);
	}

      if (ch->incog_level)
	{
	  sprintf(buf,"  `7Incognito: level `&%d",ch->incog_level);
	  send_to_char(buf,ch);
	}
      send_to_char("\n\r",ch);
    }


  if ( ch->level >= 10 )
    {
      printf_to_char(ch, "`7Alignment: `&%d.  ", ch->alignment );
    }

  send_to_char( "You are ", ch );
  if ( ch->alignment >  900 ) send_to_char( "angelic.\n\r", ch );
  else if ( ch->alignment >  700 ) send_to_char( "saintly.\n\r", ch );
  else if ( ch->alignment >  350 ) send_to_char( "good.\n\r",    ch );
  else if ( ch->alignment >  100 ) send_to_char( "kind.\n\r",    ch );
  else if ( ch->alignment > -100 ) send_to_char( "neutral.\n\r", ch );
  else if ( ch->alignment > -350 ) send_to_char( "mean.\n\r",    ch );
  else if ( ch->alignment > -700 ) send_to_char( "evil.\n\r",    ch );
  else if ( ch->alignment > -900 ) send_to_char( "demonic.\n\r", ch );
  else                             send_to_char( "satanic.\n\r", ch );
  send_to_char("`3------------------------------------------------------------------`3\n\r",ch);

  if (IS_SET(ch->comm,COMM_SHOW_AFFECTS)) {
    do_function(ch,&do_affects,"");
    send_to_char("`3------------------------------------------------------------------`7\n\r",ch);
  }
}

void last(CHAR_DATA *ch, FILE *fp)
{
	char buf[MAX_STRING_LENGTH];
	char *word;
	bool fMatch;
	char *name;
        char *host;
	long date;
	int level;
	int clan;
	int charclass;

	name = NULL;
	host = NULL;
	date = 0;
	level = -1;
        clan = -1;
        charclass = -1;
	for( ; ; )
	{
	  word = feof( fp ) ? str_dup("End") : fread_word( fp );
	  fMatch = FALSE;
	  switch ( UPPER(word[0]) )
	  {
              case '*':
                  fMatch = TRUE;
		  fread_to_eol( fp );
		  break;
              case 'C':
                  if (!str_cmp( word ,"Cla"))
                  {
                      charclass = fread_number (fp);
                      fMatch = TRUE;
                      break;
                  }
                  if ( !str_cmp( word, "ClanN" ) )
                  {
    		      clan = fread_number(fp);
    		      fMatch = TRUE;
    		      break;
		  }
		  break;
              case 'H':
	          if (!str_cmp(word, "Host"))
                  {
                      host = fread_string(fp);
                      fMatch = TRUE;
                      break;
                  }
                  break;

              case 'N':
                  if(!str_cmp(word,"Name"))
                  {
                      free_string(name);
                      name = str_dup(fread_string(fp));
                      fMatch = TRUE;
                      break;
                  }
                  break;

              case 'L':
                  if(!str_cmp(word,"LogO"))
                  {
                      date = fread_number(fp);
                      fMatch = TRUE;
                      break;
                  }

                  if (!str_cmp(word,"Levl"))
                  {
                      level = fread_number(fp);
                      fMatch = TRUE;
                      break;
                  }
                  break;

              case 'E':
		  if(!str_cmp(word,"End"))
		  {
		      if((name == NULL) ||
		       (date == 0) ||
		       (level == -1) ||
		       (charclass == -1))
		      {
		          send_to_char("Information not available.\n\r",ch);
		          return;
		      }

		      if((level>=BUILDER) && (!IS_IMMORTAL(ch)))
                      {
		          send_to_char("You would try to last someone who is always on?\n\r",ch);
		          return;
	              }

	              sprintf(buf, "Name: %s\n\r", name);
                      send_to_char(buf, ch);
                      if (IS_IMMORTAL(ch) || ch->clan == clan_lookup("Guide"))
                        sprintf(buf, "Guild: %s Class: %s Level: %d\n\r",(clan!=-1)?clan_table[clan].who_name:"[ None ]",class_table[charclass].who_name,level);
                      else
                        sprintf(buf, "Level: %d\n\r", level);
       	              send_to_char(buf,ch);
	              sprintf(buf, "Last seen on: %s\n\r", ctime(&date));
                      send_to_char(buf,ch);
                      long timedif = (long) difftime(current_time, date);
                      send_to_char("Time since last log ", ch);
                      print_time_since(ch, timedif);
       	              free_string(name);

                      if (IS_IMMORTAL(ch))
		      {
		          if (host != NULL && host[0] != '\0')
                          {
		              sprintf(buf, "Host %s\n\r", host);
		              send_to_char(buf, ch);
		          }
		      }
	              return;
                      break;
                 }
          break;
    }

    if (!fMatch)
    {
      fread_to_eol (fp);
    }
  }


}


void do_last(CHAR_DATA *ch, char *argument)
{
	FILE *fp;
	char arg[MAX_INPUT_LENGTH];
	char strsave[MAX_INPUT_LENGTH];
	//char buf[MAX_STRING_LENGTH];
	char *name;
	DESCRIPTOR_DATA *d;
	bool found = FALSE;
	char letter;
	char *word;

	one_argument(argument,arg);

	if(arg[0] == '\0')
	{
		send_to_char("You must provide a name to last!\n\r",ch);
		return;
	}
	name = arg;
        if(!IS_IMMORTAL(ch))
        {
          send_to_char("Huh?", ch);
          return;
        }
	for(d = descriptor_list; d; d = d->next)
	{
		CHAR_DATA *wch;

		if(d->connected != CON_PLAYING || !can_see(ch,d->character))
		  continue;
		wch = (d->original != NULL ) ? d->original : d->character;

	  if(!can_see(ch,wch))
  	  continue;

	  if(is_exact_name(arg,wch->name))
	    found = TRUE;
	}

	if(found)
	{
		send_to_char("That character is playing right now!\n\r",ch);
		return;
	}
	else
	{
	  sprintf(strsave, "%s%s",PLAYER_DIR, capitalize(name));
	  if((fp = fopen(strsave, "r")) != NULL)
	  {
	    letter = fread_letter(fp);
	    if(letter == '*')
		    fread_to_eol(fp);
	    if(letter != '#')
  	  {
		    bug("Do_last: # not found.", 0);
		    return;
	    }
	    word = fread_word(fp);

	    if(str_cmp(word, "PLAYER"))
	    {
		    bug("BUG in last: File opened is not player's file.",0);
		    fclose(fp);
		    return;
	    }

	    last(ch,fp);
	    fclose(fp);
	  }
	  else
	  {
		  send_to_char("There is no player by that name!\n\r",ch);
		  return;
	  }
  }
}
/*
last(FILE);
return;
}
*/

/*window code, by Kimber Boh*/
void look_window( CHAR_DATA *ch, OBJ_DATA *obj )
{
	char buf[MAX_STRING_LENGTH];
    	ROOM_INDEX_DATA *window_room;

	if ( obj->value[0] == 0 )
       	{
		   sprintf(buf, "%s\n\r", obj->description );
	 	  send_to_char(buf, ch);
	 	  return;
	}

	window_room = get_room_index( obj->value[0] );

	if ( window_room == NULL )
	{
	  	 send_to_char( "!!BUG!! Window looks into a NULL room! Please report!\n\r", ch );
		   bug( "Window %d looks into a null room!!!", obj->pIndexData->vnum );
	 	  return;
	}

	if ( !IS_NPC(ch) )
	{
		  send_to_char( "Looking through the window you can see ",ch);
		  send_to_char( window_room->name, ch );
		  send_to_char( "\n\r", ch);
		  show_list_to_char( window_room->contents, ch, FALSE, FALSE, 0 );
		  show_char_to_char( window_room->people,   ch );
		  return;
	}
}

void do_notitles(CHAR_DATA *ch, char *argument)
{
  if (IS_SET(ch->act2, PLR_NOTITLE))
  {
    send_to_char("Titles turned back on.\n\r", ch);
    REMOVE_BIT(ch->act2, PLR_NOTITLE);
    return;
  }

  SET_BIT(ch->act2, PLR_NOTITLE);
  send_to_char("No titles will be shown now.\n\r", ch);
}

void do_norp(CHAR_DATA *ch, char *argument)
{
  if (IS_SET(ch->act2, PLR_NORP))
  {
    send_to_char("RP Mode  turned on.\n\r", ch);
    REMOVE_BIT(ch->act2, PLR_NORP);
    return;
  }

  SET_BIT(ch->act2, PLR_NORP);
  send_to_char("Emotes/Says won't be considered for rp.\n\r", ch);
}

void do_nodamage(CHAR_DATA *ch, char *argument)
{
  if (IS_SET(ch->act2, PLR_NODAMAGE))
  {
    send_to_char("Extra damage messages turned back on.\n\r", ch);
    REMOVE_BIT(ch->act2, PLR_NODAMAGE);
    return;
  }

  SET_BIT(ch->act2, PLR_NODAMAGE);
  send_to_char("No extra damage messages will be shown now.\n\r", ch);
}

void do_noweave(CHAR_DATA *ch, char *argument)
{
  if (IS_SET(ch->act2, PLR_NOWEAVE))
  {
    send_to_char("Weave messages turned back on.\n\r", ch);
    REMOVE_BIT(ch->act2, PLR_NOWEAVE);
    return;
  }

  SET_BIT(ch->act2, PLR_NOWEAVE);
  send_to_char("No weave messages will be shown now.\n\r", ch);
}

void do_generate_names(CHAR_DATA *ch, char *argument)
{
	int loop;
	int antal_start = 10;
	int antal_mitt = 10;
	int antal_slut = 10;
	char namn[20];                            /* name                         */
	char buf[MSL];

  char f_start[100][5] = { "A", "Ab", "Ac", "Ad", "Af", "Agr", "As", "Al", "Adw", "Adr", "Ar",
              "B", "Br", "C", "C", "C", "Cr", "Ch", "Cad", "D", "Dr", "Dw", "Ed", "Eth", "Et", "Er",
              "El", "Eow", "F", "Fr", "G", "Gr", "Gw", "Gal", "Gl", "H", "Ha", "Ib", "Jer", "K", "Ka",
              "Ked", "L", "Loth", "Lar", "Leg", "M", "Mir", "N", "Nyd", "Ol", "Oc", "On", "P", "Pr",
              "Q", "R", "Rh", "S", "Sev", "T", "Tr", "Th", "Th", "Ul", "Um", "Un", "V", "Y", "Yb",
              "Z", "W", "W", "Wic" };

  char f_mitt[100][5] = { "a", "a", "a", "ae", "ae", "are", "ale", "ali", "ay", "ardo", "e", "e", "e",
			  "ei", "ea", "eri", "era", "ela", "eli", "enda", "erra", "i", "i", "i", "ia",
			  "ie", "ire", "ira", "ila", "ili", "ira", "igo", "o", "oa", "oe", "ore", "u",
			  "y" };

  char f_slut[100][6] = { "beth", "cia", "cien", "clya","de", "dia", "dda", "dien", "dith", "dia",
		"dda", "dien", "dith", "dia", "lind", "lith", "lia", "lian", "lla", "llan", "lle",
		"ma", "mma", "mwen", "meth", "n", "n", "n", "nna", "ndra", "ng", "ni", "nia", "niel",
		"rith", "rien", "ria", "ri", "rwen", "sa", "sien", "ssa", "ssi", "swen", "thien",
		"thiel", "viel", "via", "ven", "veth", "wen", "wen", "wen", "wia", "weth", "wien",
		"wiel" };

  char m_start[100][5] = { "A", "Ab", "Ac", "Ad", "Af", "Agr", "Ast", "As", "Al",
              "Adw", "Adr", "Ar", "B", "Br", "C", "C", "C", "Cr", "Ch",
              "Cad", "D", "Dr", "Dw", "Ed", "Eth", "Et", "Er", "El",
              "Eow", "F", "Fr", "G", "Gr", "Gw", "Gw", "Gal", "Gl",
              "H", "Ha", "Ib", "J", "Jer", "K", "Ka", "Ked", "L", "Loth",
              "Lar", "Leg", "M", "Mir", "N", "Nyd", "Ol", "Oc", "On", "P",
              "Pr", "Q", "R", "Rh", "S", "Sev", "T", "Tr", "Th", "Th", "Ul",
              "Um", "Un", "V", "Y", "Yb", "Z", "W", "W", "Wic" };

  char m_mitt[100][5] = { "a", "ae", "ae", "au", "ao", "are", "ale", "ali",
              "ay", "ardo", "e", "edri", "ei", "ea", "ea", "eri", "era",
              "ela", "eli", "enda", "erra", "i", "ia", "ie", "ire", "ira",
              "ila", "ili", "ira", "igo", "o", "oha", "oma", "oa", "oi",
              "oe", "ore", "u", "y" };

  char m_slut[100][5] = { "a", "and", "b", "bwyn", "baen", "bard", "c", "ch",
              "can", "d", "dan", "don", "der", "dric", "dus", "f", "g", "gord",
              "gan", "han", "har", "jar", "jan", "k", "kin", "kith", "kath", "koth",
              "kor", "kon", "l", "li", "lin", "lith", "lath", "loth", "ld", "ldan",
              "m", "mas", "mos", "mar", "mond", "n", "nydd", "nidd", "nnon", "nwan",
              "nyth", "nad", "nn", "nnor", "nd", "p", "r", "red", "ric", "rid",
              "rin", "ron", "rd", "s", "sh", "seth", "sean", "t", "th", "th",
              "tha", "tlan", "trem", "tram", "v", "vudd", "w", "wan", "win",
              "win", "wyn", "wyn", "wyr", "wyr", "wyth" };

  if (!str_cmp(argument, "Male"))
  {
    antal_start =77;
    antal_mitt = 39;
    antal_slut = 56;

    send_to_char("Generated Male Names:\n\r\n\r", ch);
    for (loop = 0; loop < 10; loop++)/* loop through nr of names   */
    {
      strcpy(namn, m_start[rand()%antal_start]);/* get a start                  */
      strcat(namn, m_mitt[rand()%antal_mitt]);  /* get a middle                 */
      strcat(namn, m_slut[rand()%antal_slut]);  /* get an ending                */
      sprintf(buf, "%-15s\n", namn);                /* print the name               */
      send_to_char(buf, ch);
    }
  }
  else
  {
    antal_start =74;
    antal_mitt = 38;
    antal_slut = 83;

    send_to_char("Generated Female Names:\n\r\n\r", ch);
    for (loop = 0; loop < 10; loop++)/* loop through nr of names   */
    {
      strcpy(namn, f_start[rand()%antal_start]);/* get a start                  */
      strcat(namn, f_mitt[rand()%antal_mitt]);  /* get a middle                 */
      strcat(namn, f_slut[rand()%antal_slut]);  /* get an ending                */
      sprintf(buf, "%-15s\n", namn);                /* print the name               */
      send_to_char(buf, ch);
    }
  }

}

void gen_name(DESCRIPTOR_DATA *d, char *argument)
{
	int loop;
	int antal_start = 10;
	int antal_mitt = 10;
	int antal_slut = 10;
	char namn[20];                            /* name                         */
	char buf[MSL];

  char f_start[100][5] = { "A", "Ab", "Ac", "Ad", "Af", "Agr", "As", "Al", "Adw", "Adr", "Ar",
              "B", "Br", "C", "C", "C", "Cr", "Ch", "Cad", "D", "Dr", "Dw", "Ed", "Eth", "Et", "Er",
              "El", "Eow", "F", "Fr", "G", "Gr", "Gw", "Gal", "Gl", "H", "Ha", "Ib", "Jer", "K", "Ka",
              "Ked", "L", "Loth", "Lar", "Leg", "M", "Mir", "N", "Nyd", "Ol", "Oc", "On", "P", "Pr",
              "Q", "R", "Rh", "S", "Sev", "T", "Tr", "Th", "Th", "Ul", "Um", "Un", "V", "Y", "Yb",
              "Z", "W", "W", "Wic" };

  char f_mitt[100][5] = { "a", "a", "a", "ae", "ae", "are", "ale", "ali", "ay", "ardo", "e", "e", "e",
			  "ei", "ea", "eri", "era", "ela", "eli", "enda", "erra", "i", "i", "i", "ia",
			  "ie", "ire", "ira", "ila", "ili", "ira", "igo", "o", "oa", "oe", "ore", "u",
			  "y" };

  char f_slut[100][6] = { "beth", "cia", "cien", "clya","de", "dia", "dda", "dien", "dith", "dia",
		"dda", "dien", "dith", "dia", "lind", "lith", "lia", "lian", "lla", "llan", "lle",
		"ma", "mma", "mwen", "meth", "n", "n", "n", "nna", "ndra", "ng", "ni", "nia", "niel",
		"rith", "rien", "ria", "ri", "rwen", "sa", "sien", "ssa", "ssi", "swen", "thien",
		"thiel", "viel", "via", "ven", "veth", "wen", "wen", "wen", "wia", "weth", "wien",
		"wiel" };

  char m_start[100][5] = { "A", "Ab", "Ac", "Ad", "Af", "Agr", "Ast", "As", "Al",
              "Adw", "Adr", "Ar", "B", "Br", "C", "C", "C", "Cr", "Ch",
              "Cad", "D", "Dr", "Dw", "Ed", "Eth", "Et", "Er", "El",
              "Eow", "F", "Fr", "G", "Gr", "Gw", "Gw", "Gal", "Gl",
              "H", "Ha", "Ib", "J", "Jer", "K", "Ka", "Ked", "L", "Loth",
              "Lar", "Leg", "M", "Mir", "N", "Nyd", "Ol", "Oc", "On", "P",
              "Pr", "Q", "R", "Rh", "S", "Sev", "T", "Tr", "Th", "Th", "Ul",
              "Um", "Un", "V", "Y", "Yb", "Z", "W", "W", "Wic" };

  char m_mitt[100][5] = { "a", "ae", "ae", "au", "ao", "are", "ale", "ali",
              "ay", "ardo", "e", "edri", "ei", "ea", "ea", "eri", "era",
              "ela", "eli", "enda", "erra", "i", "ia", "ie", "ire", "ira",
              "ila", "ili", "ira", "igo", "o", "oha", "oma", "oa", "oi",
              "oe", "ore", "u", "y" };

  char m_slut[100][5] = { "a", "and", "b", "bwyn", "baen", "bard", "c", "ch",
              "can", "d", "dan", "don", "der", "dric", "dus", "f", "g", "gord",
              "gan", "han", "har", "jar", "jan", "k", "kin", "kith", "kath", "koth",
              "kor", "kon", "l", "li", "lin", "lith", "lath", "loth", "ld", "ldan",
              "m", "mas", "mos", "mar", "mond", "n", "nydd", "nidd", "nnon", "nwan",
              "nyth", "nad", "nn", "nnor", "nd", "p", "r", "red", "ric", "rid",
              "rin", "ron", "rd", "s", "sh", "seth", "sean", "t", "th", "th",
              "tha", "tlan", "trem", "tram", "v", "vudd", "w", "wan", "win",
              "win", "wyn", "wyn", "wyr", "wyr", "wyth" };

  if (!str_cmp(argument, "Male"))
  {
    antal_start =77;
    antal_mitt = 39;
    antal_slut = 56;

    send_to_desc("\n\rGenerated Male Names:\n\r\n\r", d);
    for (loop = 0; loop < 10; loop++)/* loop through nr of names   */
    {
      strcpy(namn, m_start[rand()%antal_start]);/* get a start                  */
      strcat(namn, m_mitt[rand()%antal_mitt]);  /* get a middle                 */
      strcat(namn, m_slut[rand()%antal_slut]);  /* get an ending                */
      sprintf(buf, "%-15s\n", namn);                /* print the name               */
      send_to_desc(buf, d);
    }
  }
  else
  {
    antal_start =74;
    antal_mitt = 38;
    antal_slut = 83;

    send_to_desc("\n\rGenerated Female Names:\n\r\n\r", d);
    for (loop = 0; loop < 10; loop++)/* loop through nr of names   */
    {
      strcpy(namn, f_start[rand()%antal_start]);/* get a start                  */
      strcat(namn, f_mitt[rand()%antal_mitt]);  /* get a middle                 */
      strcat(namn, f_slut[rand()%antal_slut]);  /* get an ending                */
      sprintf(buf, "%-15s\n", namn);                /* print the name               */
      send_to_desc(buf, d);
    }
  }

}
/*
void do_diem (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
    int iClass;
    int iRace;
    int iClan;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    bool rgfClass[MAX_CLASS];
    bool rgfRace[MAX_PC_RACE];
    bool rgfClan[top_clan];
    bool fClassRestrict = FALSE;
    bool fClanRestrict = FALSE;
    bool fClan = FALSE;
    bool fRaceRestrict = FALSE;
    bool fImmortalOnly = FALSE;
    bool fTournRestrict = FALSE;
    bool fQuestRestrict = FALSE;
    bool grep = FALSE;
    char arg[MSL];

    argument = one_argument (argument, arg);

    if (!str_cmp(arg, "|"))
    {
      grep = TRUE;
      argument = one_argument (argument, arg);
    }

  if (IS_NPC (ch))
  {
    return;
  }
    *
     * Set default arguments.
     *
    iLevelLower = 0;
    iLevelUpper = MAX_LEVEL;
    for (iClass = 0; iClass < MAX_CLASS; iClass++)
        rgfClass[iClass] = FALSE;
    for (iRace = 0; iRace < MAX_PC_RACE; iRace++)
        rgfRace[iRace] = FALSE;
    for (iClan = 0; iClan < top_clan; iClan++)
        rgfClan[iClan] = FALSE;

    *
     * Parse arguments.
     *
    nNumber = 0;
    for (;;)
    {
        if (arg[0] == '\0' || grep)
            break;

        if (is_number (arg))
        {
            switch (++nNumber)
            {
                case 1:
                    iLevelLower = atoi (arg);
                    break;
                case 2:
                    iLevelUpper = atoi (arg);
                    break;
                default:
                    send_to_char ("Only two level numbers allowed.\n\r", ch);
                    return;
            }
        }
        else
        {

            *
             * Look for classes to turn on.

            if (!str_prefix (arg, "immortals"))
            {
                fImmortalOnly = TRUE;
            }
            else if (!str_prefix (arg, "tournament"))
            {
              fTournRestrict = TRUE;
            }
            else if (!str_prefix (arg, "quest"))
            {
              fQuestRestrict = TRUE;
            }
            else if (!str_prefix (arg, "legend"))
            {
              iLevelLower = LEVEL_HERO;
              iLevelUpper = LEVEL_HERO + 1;
            }
            else
            {
                iClass = class_lookup (arg);
                if (iClass == -1)
                {
                    iRace = race_lookup (arg);

                    if (iRace == 0 || iRace >= MAX_PC_RACE)
                    {
                        if (!str_prefix (arg, "clan"))
                            fClan = TRUE;
                        else
                        {
                            iClan = clan_lookup (arg);
                            if (iClan)
                            {
                                fClanRestrict = TRUE;
                                rgfClan[iClan] = TRUE;
			    }
                            else
                            {
                                send_to_char
                                    ("That's not a valid race, class, or clan.\n\r",
                                     ch);
                                return;


*/

void do_showclass( CHAR_DATA *ch, char *argument )
{
    int sn;
    int cClass;
    int col = 0;
    int lev = 0;
    int skill_lev;
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
        send_to_char("Syntax: showclass <class name>\n\r", ch);
        return;
    }

    cClass = class_lookup( argument );

    if ( cClass == -1 ) /* no class by that name */
    {
        send_to_char("No class by that name exists.\n\r", ch);
        return;
    }

    sprintf(buf, "Spells/Skills for %s:\n\r", class_table[cClass].name);
    send_to_char(buf, ch);

    for ( lev = 1; lev < LEVEL_IMMORTAL; lev ++)
    {
        for ( sn = 0; sn < MAX_SKILL; sn ++)
        {

           skill_lev = skill_table[sn].skill_level[cClass];

           if ( ( skill_lev == lev ) 
		&& ( skill_table[sn].name != "daze" )
                && ( skill_table[sn].name != "howl" )
                && ( skill_table[sn].name != "tap" )
                && ( skill_table[sn].name != "damane" )
                && ( skill_table[sn].name != "store" )
                && ( skill_table[sn].name != "medallion" )
                && ( skill_table[sn].name != "veil" )
                && ( skill_table[sn].name != "scry" )
                && ( skill_table[sn].name != "fix" )
                && ( skill_table[sn].name != "build" )
                && ( skill_table[sn].name != "dream" )
                && ( skill_table[sn].name != "restrain" )
                && ( skill_table[sn].name != "firestorm" )
                && ( skill_table[sn].name != "still" )
                && ( skill_table[sn].name != "balefire" )
                && ( skill_table[sn].name != "void" )
                && ( skill_table[sn].name != "stealth" )
                && ( skill_table[sn].name != "shroud" )
                && ( skill_table[sn].name != "pray" )
                && ( skill_table[sn].name != "taint" )
                && ( skill_table[sn].name != "blood" )
                && ( skill_table[sn].name != "ravage" )
                && ( skill_table[sn].name != "damane" )
                && ( skill_table[sn].name != "luck" )
                && ( skill_table[sn].name != "warcry" )
                && ( skill_table[sn].name != "stance" )
                && ( skill_table[sn].name != "cleanse" )
                && ( skill_table[sn].name != "ballad" )
                && ( skill_table[sn].name != "tame" )
                && ( skill_table[sn].name != "couple" )
                && ( skill_table[sn].name != "horseback" )
                && ( skill_table[sn].name != "ensnare" )
                && ( skill_table[sn].name != "study" )
                && ( skill_table[sn].name != "save_life" )
                && ( skill_table[sn].name != "unravel" ) 
                && ( skill_table[sn].name != "dome" )
                && ( skill_table[sn].name != "hunt" )
		&& ( skill_table[sn].name != "wolf" )
                && ( skill_table[sn].name != "willpower" )
		&& ( skill_table[sn].name != "contact" ) )

           {
                sprintf(buf, "`8Level `5%-4d`7: `&%-18s`7", lev, skill_table[sn].name);
                send_to_char(buf, ch);
                col ++;
                if ( col == 2 )
                {
                        send_to_char("\n\r", ch);
                        col = 0;
                }
                continue;
           }
       }
    }

    send_to_char("\n\r", ch);

  return;

}

void do_worldmap( CHAR_DATA *ch, char *argument )
{
  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act2, PLR_WORLDMAP))
  {
    send_to_char("World Map Turned off.  Descriptions turned on.\n\r", ch);
    REMOVE_BIT(ch->act2, PLR_WORLDMAP);
    return;
  }

  SET_BIT(ch->act2, PLR_WORLDMAP);
  send_to_char("World Map Turned on.  Descriptions turned off..\n\r", ch);
}

