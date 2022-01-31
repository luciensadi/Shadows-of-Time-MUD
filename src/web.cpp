/* Print Mobiles */
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "lookup.h"
#include "web.h"

void print_specifics  args ((FILE *fp, OBJ_INDEX_DATA *pObjIndex, int even_odd));
bool is_stat args ((const struct flag_type *flag_table));

const char *color_codes[2] =
{
  "\"#FFCC99\"",
  "\"#FFFFCC\""
};

const struct point_type point_table[] =
{
   {APPLY_AC, -2, 3},
   {APPLY_HIT, 1, 1},
   {APPLY_MANA, 1, 1},
   {APPLY_MOVE, 2, 1},
   {APPLY_HITROLL, 1, 5},
   {APPLY_DAMROLL, 1, 5},
   {APPLY_SAVES, -1, 5},
   {APPLY_STR, 1, 5},
   {APPLY_DEX, 1, 5},
   {APPLY_INT, 1, 5},
   {APPLY_WIS, 1, 5},
   {APPLY_CON, 1, 5},
   {APPLY_SAVING_PARA, -1, 5},
   {APPLY_SAVING_ROD, -1, 5},
   {APPLY_SAVING_PETRI, -1, 5},
   {APPLY_SAVING_BREATH, -1, 5},
   {APPLY_SAVING_SPELL, -1, 5},
   {-69, -69, -69},
};

int calculate_flag_points (const struct flag_type *flag_table, int bits)
{
    static int cnt = 0;
    int sum = 0;
    int flag;

    if (++cnt > 1)
        cnt = 0;


    for (flag = 0; flag_table[flag].name != NULL; flag++)
    {
        if (!is_stat (flag_table) && IS_SET (bits, flag_table[flag].bit))
        {
	  sum += flag_table[flag].points;
        }
        else if (flag_table[flag].bit == bits)
        {
  	    sum += flag_table[flag].points;
            break;
        }
    }
    return sum;
}

int calculate_affect_points(OBJ_INDEX_DATA *obj)
{ 
   AFFECT_DATA *paf;
   int flag;
   int point = 0;
   int sum = 0;
   bool found;

   for (paf = obj->affected;paf;paf = paf->next)
   {
      point = 0;
      found = FALSE;
      for (flag = 0; point_table[flag].bit != -69; flag++)
      {
        if (paf->location == point_table[flag].bit)
        {
           found = TRUE;
           if (paf->modifier == 0)
             point = 0;
           else
           {
             point = (paf->modifier / point_table[flag].point) * 
                                        point_table[flag].cost;
	   }
           break;
	}
      }
      if (!found && 
         (paf->location == APPLY_NONE || paf->location == APPLY_SPELL_AFFECT))
        point = 25;
      sum += point;
   }
   return sum;
}
    
int calculate_points(OBJ_INDEX_DATA *obj)
{
   int sum = 0;
   sum += calculate_affect_points(obj);
   sum += calculate_flag_points(extra_flags, obj->extra_flags);
   if (obj->item_type == ITEM_WEAPON)   
     sum += calculate_flag_points(weapon_type2, obj->value[4]);   

   return sum;
}

int calculate_allowed (OBJ_INDEX_DATA *obj)
{
  int points = 0;
  points = obj->level + 30;
  if (IS_OBJ_STAT(obj, ITEM_QUEST))
    points = int(points * 1.20);

  return points;
}

void do_print_mobiles(CHAR_DATA *ch, char *argument)
{
  AREA_DATA *list;
  MOB_INDEX_DATA *pMobIndex;
  long vnum;
  FILE *fp;
  int even_odd = 0;  

  if ((fp = fopen(PMOB_FILE, "w")) == NULL)
  {
    bug("Print_Mobiles: fopen", 0);
    return;
  }	

  fprintf(fp, "<HTML><TITLE>SoT Mob List</TITLE>\n");
  fprintf(fp, "<BODY BGCOLOR=\"#FFFFFF\" link=\"#FFFFFF\" vlink=\"#FFFFFF\" alink=\"FFFFFF\">\n");

  
  for (list = area_first; list ; list = list->next)
  {
    fprintf(fp, "<TABLE BORDER=\"0\" WIDTH=\"70%%\">"
                "\n<TH BGCOLOR=\"#CC9900\">Area %s</TH>\n", list->name);
    for ( vnum = list->min_vnum; vnum < list->max_vnum; vnum++ )
    {
       if ((pMobIndex = get_mob_index(vnum)) == NULL)
         continue;
       fprintf(fp, "<TR BGCOLOR=%s>"
                   "<TD width=\"15%%\">Vnum %ld</TD>"
                   "<TD width=\"70%%\">Name %s</TD>"
                   "<TD width=\"15%%\">Level %d</TD></TR>\n", 
                 color_codes[even_odd],
                 pMobIndex->vnum, pMobIndex->player_name, pMobIndex->level);
       fprintf(fp, "<TR BGCOLOR=%s>"
                   "<TD width=\"33%%\">Ave Hps %ld</TD>"
                   "<TD width=\"34%%\">Hitroll %d</TD>"
                   "<TD width=\"33%%\">Damroll %d</TD></TR>\n",
		color_codes[even_odd], 
	       ((pMobIndex->hit[0] * pMobIndex->hit[1]) / 2) +pMobIndex->hit[2],
                pMobIndex->hitroll,  
	       ((pMobIndex->damage[0] * pMobIndex->damage[1]) / 2) +pMobIndex->damage[2]);
       fprintf(fp, "<TR></TR>\n");
       if (even_odd == 1)
         even_odd = 0;
       else
         even_odd = 1;	 
    }
    fprintf(fp, "</TABLE><BR>\n"); 
  }
  fprintf(fp, "</BODY></HTML>\n");
  fclose(fp); 
}

void do_print_objects(CHAR_DATA *ch, char *argument)
{
  AREA_DATA *list;
  OBJ_INDEX_DATA *pObjIndex;
  AFFECT_DATA *paf;
  int vnum;
  FILE *fp;
  int even_odd = 0;  
  bool restrictive = FALSE;
  bool ac = FALSE;
  bool weapon = FALSE;

  if (!str_cmp(argument, "point"))
  {
    restrictive = TRUE; 
    logf("Restrictive", 0);
  }

  if (!str_cmp(argument, "ac"))
  {
    ac = TRUE; 
    logf("Restrictive", 0);
  }

  if (!str_cmp(argument, "weapon"))
  {
    weapon = TRUE; 
    logf("Restrictive", 0);
  }
 
  if ((fp = fopen(POBJ_FILE, "w")) == NULL)
  {
    bug("Print_Objects: fopen", 0);
    return;
  }	

  fprintf(fp, "<HTML><TITLE>Object List</TITLE>\n");
  fprintf(fp, "<BODY BGCOLOR=\"#FFFFFF\" link=\"#FFFFFF\" vlink=\"#FFFFFF\" alink=\"FFFFFF\">\n");
   
  for (list = area_first; list ; list = list->next)
  {
    fprintf(fp, "<TABLE BORDER=\"0\" WIDTH=\"80%%\" ALIGN=CENTER>"
                "\n<TH BGCOLOR=\"#CC9900\">Area %s</TH>\n", list->name);
    for ( vnum = list->min_vnum; vnum < list->max_vnum; vnum++ )
    {
       if ((pObjIndex = get_obj_index(vnum)) == NULL)
         continue;

       if (restrictive && 
           (pObjIndex->level + 30) > calculate_points(pObjIndex))
       {
          continue; 
       }

       if (ac && (pObjIndex->item_type != ITEM_ARMOR ||
           ((pObjIndex->level + 30) > 
           ((pObjIndex->value[0] + pObjIndex->value[1] + pObjIndex->value[2] + pObjIndex->value[3])))))
       {
          continue; 
       }

       if (weapon && pObjIndex->item_type != ITEM_WEAPON)
         continue;

       if (weapon && pObjIndex->item_type == ITEM_WEAPON &&
              (
                ( (1 + pObjIndex->value[2]) * pObjIndex->value[1] / 2) <=
                ( (pObjIndex->level + 40) / 2) 
              )
            )
       {
         continue;
       } 

       fprintf(fp, "<TR BGCOLOR=%s>"
                   "<TD width=\"15%%\">Vnum: %ld</TD>"
                   "<TD width=\"70%%\">Name: %s</TD>"
                   "<TD width=\"15%%\">Level: %d</TD></TR>\n", 
                 color_codes[even_odd],
                 pObjIndex->vnum, pObjIndex->short_descr, pObjIndex->level);
       fprintf(fp, "<TR BGCOLOR=%s>"
                   "<TD width=\"50%%\">Max Pts: %d</TD>\n"
		   "<TD colspan=\"2\">Actual Pts: %d</TD></TR>\n",
		   color_codes[even_odd],
                   pObjIndex->level + 30,
                   calculate_points(pObjIndex));
       fprintf(fp, "<TR BGCOLOR=%s>"
                   "<TD width=\"50%%\">Wear: %s</TD>\n"
		   "<TD colspan=\"2\">Extra: %s</TD></TR>\n",
		   color_codes[even_odd],
                   flag_string (wear_flags, pObjIndex->wear_flags),
                   flag_string (extra_flags, pObjIndex->extra_flags));
       if (pObjIndex->affected)
       {
         fprintf(fp, "<TR BGCOLOR=%s>"
                     "<TD colspan=\"3\"> Affects </TD></TR>\n",
	  	     color_codes[even_odd]);
         for (paf = pObjIndex->affected; paf; paf = paf->next)
         {
           fprintf(fp, "<TR BGCOLOR=%s>"
	    	       "<TD colspan=\"3\">Affect: %s %d</TD></TR>\n",
		       color_codes[even_odd],
                       flag_string (apply_flags, paf->location),
			paf->modifier);
         }
       }
       print_specifics(fp, pObjIndex, even_odd);
       if (even_odd == 1)
         even_odd = 0;
       else
         even_odd = 1;	 
    }
    fprintf(fp, "</TABLE>\n");
  }
  fprintf(fp, "</HTML>\n");
  fclose(fp);
}   

void print_specifics(FILE *fp, OBJ_INDEX_DATA *pObjIndex, int even_odd)
{
  OBJ_DATA *obj;

  if ((obj = create_object(pObjIndex, 70)) == NULL)
    return; 

    switch (obj->item_type)
    {
        default:                /* No values. */
            break;

	    case ITEM_WINDOW:
	    fprintf( fp, 
		"<TR BGCOLOR=%s><TD colspan=\"3\">[v0] Room to view: [%d]</TD>\n",
                color_codes[even_odd],
		obj->value[0] );
		break;	

        case ITEM_LIGHT:
            if (obj->value[2] == -1 || obj->value[2] == 999)    /* ROM OLC */
                fprintf (fp, "<TR BGCOLOR=%s><TD colspan=\"3\">[v2] Light:  Infinite[-1]</TD>\n",
			color_codes[even_odd]);
            else
                fprintf (fp, "<TR BGCOLOR=%s> <TD colspan=\"3\">[v2] Light:  [%d]</TD></TR>\n", 
			color_codes[even_odd], obj->value[2]);
            break;

        case ITEM_WAND:
        case ITEM_STAFF:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Level:          [%d]</TD>\n"
                     "<TD>[v1] Charges Total:  [%d]</TD>\n"
                     "<TD>[v2] Charges Left:   [%d]</TD></TR>\n"
                     "<TR BGCOLOR=%s><TD colspan=\"3\">[v3] Spell:          %s</TD></TR>\n",
                     color_codes[even_odd],
                     obj->value[0],
                     obj->value[1],
                     obj->value[2],
                     color_codes[even_odd],  
                     obj->value[3] != -1 ? skill_table[obj->value[3]].name
                     : "none");
            break;

        case ITEM_PORTAL:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Charges:        [%d]</TD>\n"
                     "<TD>[v1] Exit Flags:     %s</TD>\n"
                     "<TD>[v2] Portal Flags:   %s</TD</TR>\n"
                     "<TR BGCOLOR=%s><TD colspan=\"3\">[v3] Goes to (vnum): [%d]</TD></TR>\n",
        	     color_codes[even_odd],	
                     obj->value[0],
                     flag_string (exit_flags, obj->value[1]),
                     flag_string (portal_flags, obj->value[2]),
		     color_codes[even_odd], 	
                     obj->value[3]);
            break;

        case ITEM_FURNITURE:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Max people:      [%d]</TD>\n"
                     "<TD>[v1] Max weight:      [%d]</TD>\n"
                     "<TD>[v2] Furniture Flags: %s</TD></TR>\n"
                     "<TR BGCOLOR=%s><TD>[v3] Heal bonus:      [%d]</TD>\n"
                     "<TD colspan=\"2\">[v4] Mana bonus:      [%d]</TD></TR>\n",
		     color_codes[even_odd],
                     obj->value[0],
                     obj->value[1],
                     flag_string (furniture_flags, obj->value[2]),
		     color_codes[even_odd],
                     obj->value[3], obj->value[4]);
            break;

        case ITEM_FIRE:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD colspan=\"3\">[v0] Burn Duration :           [%d]</TD></TR>\n",
		     color_codes[even_odd],
                     obj->value[0]);
            break;

        case ITEM_FUEL:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Burn Duration :           [%d]</TD>\n"
                     "<TD colspan=\"2\">[v1] Amount needed :           [%d]</TD></TR>\n",
      		     color_codes[even_odd],	
	             obj->value[0],
                     obj->value[1]);
            break;
    

        case ITEM_IGNITER:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Uses :           [%d]</TD>\n"
                     "<TD colspan=\"2\">[v1] Strike string:   %d</TD></TR>\n",
		     color_codes[even_odd],
                     obj->value[0],
                     obj->value[1]);
            break;
            
            
        case ITEM_INGREDIENT:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD colspan=\"3\">[v0] Ingredient Type :           [%s]</TD></TR>\n",
		     color_codes[even_odd],	
                     flag_string(ingredient_flags, obj->value[0]));
            break;
 
        case ITEM_BOMB:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Trigger :           [%s]</TD>\n"
                     "<TD>[v1] Explosion Affect:   [%s]</TD>\n"
                     "<TD>[v2] Bomb State          [%s]</TD></TR>\n"
                     "<TR BGCOLOR=%s><TD colspan=\"3\">[v3] Ave dam             [%d]</TD></TR>\n",
        	     color_codes[even_odd],	
                     flag_string(trigger_flags, obj->value[0]),
                     explosive_name(obj->value[1]),
                     flag_string(state_flags, obj->value[2]),
		     color_codes[even_odd],
                     obj->value[3]);
            break;

	      case ITEM_QUIVER:
	          fprintf (fp,
	             "<TR BGCOLOR=%s><TD>[v0] Number of Arrows:  %d</TD>\n"
	             "<TD>[v1] MAX # of Arrows:   %d</TD>\n"
	             "<TD>[v2] Reserved:          %d</TD></TR>\n"
	             "<TR BGCOLOR=%s><TD colspan=\"3\">[v3] Arrow Vnum         %d</TD></TR>\n",
	    	     color_codes[even_odd],	
                     obj->value[0],
	             obj->value[1],
	             obj->value[2],
		     color_codes[even_odd],	
	             obj->value[3]);
	    break;
	case ITEM_ARROW:
	    fprintf (fp,
	           "<TR BGCOLOR=%s><TD>[v1] Number of Dice: %d</TD>\n"
	           "<TD>[v2] Type of Dice:   %d</TD>\n"
	           "<TD>[v3] Spell:          %s</TD></TR>\n"
	           "<TR BGCOLOR=%s><TD colspan=\"3\">[v4] Special type:   %s</TD></TR>\n",           
		   color_codes[even_odd],
	           obj->value[1],
	           obj->value[2],		
	           obj->value[3]!= -1 ? skill_table[obj->value[3]].name : "none",
    		   color_codes[even_odd],	
                   flag_string (weapon_type2, obj->value[4]));
	    break;
	             
        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Level:  [%d]</TD>\n"
                     "<TD>[v1] Spell:  %s</TD>\n"
                     "<TD>[v2] Spell:  %s</TD></TR>\n"
                     "<TR BGCOLOR=%s><TD>[v3] Spell:  %s</TD>\n"
                     "<TD colspan=\"2\">[v4] Spell:  %s</TD></TR>\n",
		     color_codes[even_odd],
                     obj->value[0],
                     obj->value[1] != -1 ? skill_table[obj->value[1]].name
                     : "none",
                     obj->value[2] != -1 ? skill_table[obj->value[2]].name
                     : "none",
		     color_codes[even_odd],	
                     obj->value[3] != -1 ? skill_table[obj->value[3]].name
                     : "none",
                     obj->value[4] != -1 ? skill_table[obj->value[4]].name
                     : "none");
            break;

/* ARMOR for ROM */

        case ITEM_ARMOR:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Ac pierce       [%d]</TD>\n"
                     "<TD>[v1] Ac bash         [%d]</TD>\n"
                     "<TD>[v2] Ac slash        [%d]</TD></TR>\n"
                     "<TR BGCOLOR=%s><TD colspan=3>[v3] Ac exotic       [%d]</TD></TR>\n",
		     color_codes[even_odd],
                     obj->value[0], obj->value[1], obj->value[2],
		     color_codes[even_odd],	
                     obj->value[3]);
            break;

/* WEAPON changed in ROM: */
/* I had to split the output here, I have no idea why, but it helped -- Hugin */
/* It somehow fixed a bug in showing scroll/pill/potions too ?! */
        case ITEM_WEAPON:
            fprintf (fp, "<TR BGCOLOR=%s><TD>[v0] Weapon class:   %s</TD>\n",
		     color_codes[even_odd],
                     flag_string (weapon_class, obj->value[0]));
            fprintf (fp, "<TD>[v1] Number of dice: [%d]</TD>\n", obj->value[1]);
            fprintf (fp, "<TD>[v2] Type of dice:   [%d]</TD></TR>\n", obj->value[2]);
            fprintf (fp, "<TR BGCOLOR=%s><TD>[v3] Type:           %s</TD>\n",
		     color_codes[even_odd],
                     attack_table[obj->value[3]].name);
            fprintf (fp, "<TD>[v4] Special type:   %s</TD>\n",
                     flag_string (weapon_type2, obj->value[4]));
            fprintf (fp, "<TD>Ave Dam:   %d</TD></TR>\n",
                     ((1 + obj->value[2]) * obj->value[1] / 2));
            break;

        case ITEM_CONTAINER:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Weight:     [%d kg]</TD>\n"
                     "<TD>[v1] Flags:      [%s]</TD>\n"
                     "<TD>[v2] Key:     %s [%d]</TD></TR>\n"
                     "<TR BGCOLOR=%s><TD>[v3] Capacity    [%d]</TD>\n"
                     "<TD colspan=2>[v4] Weight Mult [%d]</TD></TR>\n",
		     color_codes[even_odd],
                     obj->value[0],
                     flag_string (container_flags, obj->value[1]),
                     get_obj_index (obj->value[2])
                     ? get_obj_index (obj->value[2])->short_descr
                     : "none", obj->value[2], 
		     color_codes[even_odd],
	    	     obj->value[3], obj->value[4]);
            break;

        case ITEM_DRINK_CON:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Liquid Total: [%d]</TD>\n"
                     "<TD>[v1] Liquid Left:  [%d]</TD>\n"
                     "<TD>[v2] Liquid:       %s</TD></TR>\n"
                     "<TR BGCOLOR=%s><TD colspan=3>[v3] Poisoned:     %s</TD></TR>\n",
		     color_codes[even_odd],
                     obj->value[0],
                     obj->value[1],
                     liq_table[obj->value[2]].liq_name,
		     color_codes[even_odd],	
                     obj->value[3] != 0 ? "Yes" : "No");
            break;

        case ITEM_FOUNTAIN:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Liquid Total: [%d]</TD>\n"
                     "<TD>[v1] Liquid Left:  [%d]</TD>\n"
                     "<TD>[v2] Liquid:        %s</TD></TR>\n",
		     color_codes[even_odd],
                     obj->value[0],
                     obj->value[1], liq_table[obj->value[2]].liq_name);
            break;

        case ITEM_FOOD:
            fprintf (fp,
                     "<TR BGCOLOR=%s><TD>[v0] Food hours: [%d]</TD>\n"
                     "<TD>[v1] Full hours: [%d]</TD>\n"
                     "<TD>[v3] Poisoned:   %s</TD></TR>\n",
		     color_codes[even_odd],
                     obj->value[0],
                     obj->value[1], obj->value[3] != 0 ? "Yes" : "No");
            break;

        case ITEM_MONEY:
            fprintf (fp, "<TR BGCOLOR=%s><TD colspan=3>[v0] Gold:   [%d]</TD></TR>\n", 
		     color_codes[even_odd],	obj->value[0]);
            break;
    }
    extract_obj(obj);
}

void web_mudstats()
{
  char buf[MSL];
  FILE *fp;

  if ((fp = fopen(MUDSTAT_FILE, "w")) == NULL)
  {
    bug("Web_Mudstat: fopen", 0);
    return;
  }

  fprintf(fp, "<HTML><TITLE>Shadows of Time Statistics</TITLE>\n");
  fprintf(fp, "<BODY BGCOLOR=#000000 TEXT=#D0CC2F>\n");
  fprintf(fp, "<p align=center>\n");
  fprintf(fp, "</p>\n"); 
  fprintf(fp, "<FONT FACE=\"Verdana\" SIZE=3 COLOR=#D0CC2F>\n");
  fprintf(fp, "<TABLE CELLPADDING=4 FONT COLOR=#666666 ALIGN=CENTER>\n");  
  fprintf(fp, "<TH><FONT COLOR=#0000FF>%25s</TH>"
              "<TH><FONT COLOR=#06F918>%8s</TH>"
              "<TH><FONT COLOR=#FF0000>%5s</TH>\n", 
		"Stats since 06/07", "Total", "Today");
  fprintf(fp, "<TR><TD ALIGN=LEFT><FONT COLOR=#02687D>%s</TD>"
              "<TD ALIGN=RIGHT><FONT COLOR=#4ABB44>%d</TD>"
              "<TD ALIGN=RIGHT><FONT COLOR=#BF4640>%d</TD></TR>\n", 
	      "Max Players", mudstats.total_max_players,
                             mudstats.current_max_players);
  fprintf(fp, "<TR><TD ALIGN=LEFT><FONT COLOR=#02687D>%s</TD>"
	      "<TD ALIGN=RIGHT><FONT COLOR=#4ABB44>%ld</TD>"
	      "<TD ALIGN=RIGHT><FONT COLOR=#BF4640>%d</TD></TR>\n", 
	      "Logins", mudstats.total_logins,
                             mudstats.current_logins);
  fprintf(fp, "<TR><TD ALIGN=LEFT><FONT COLOR=#02687D>%s</TD>"
	      "<TD ALIGN=RIGHT><FONT COLOR=#4ABB44>%ld</TD>"
              "<TD ALIGN=RIGHT><FONT COLOR=#BF4640>%d</TD></TR>\n", 
	      "Newbies", mudstats.total_newbies,
                             mudstats.current_newbies);
  fprintf(fp, "<TR><TD ALIGN=LEFT><FONT COLOR=#02687D>%s</TD>"
	      "<TD ALIGN=RIGHT><FONT COLOR=#4ABB44>%d</TD>"
              "<TD ALIGN=RIGHT><FONT COLOR=#BF4640>%d</TD>\n", 
	      "Legends", mudstats.total_heros,
                             mudstats.current_heros);
  fprintf(fp, "<TR COLSPAN=3><TD>&nbsp;</TD></TR>\n");
  fprintf(fp, "<TR><TD ALIGN=LEFT><FONT COLOR=#02687D>%s</TD>"
	      "<TD ALIGN=RIGHT COLSPAN=2><FONT COLOR=#BF4640>%ld</TD></TR>\n", 
	      "Areas", top_area);
  fprintf(fp, "<TR><TD ALIGN=LEFT><FONT COLOR=#02687D>%s</TD>"
	      "<TD ALIGN=RIGHT COLSPAN=2><FONT COLOR=#BF4640>%ld</TD></TR>\n", 
	      "Rooms", top_room);
  fprintf(fp, "<TR><TD ALIGN=LEFT><FONT COLOR=#02687D>%s</TD>"
	      "<TD ALIGN=RIGHT COLSPAN=2><FONT COLOR=#BF4640>%ld</TD></TR>\n", 
	      "Unique Mobs",
		top_mob_index);
  fprintf(fp, "<TR><TD ALIGN=LEFT><FONT COLOR=#02687D>%s</TD>"
	      "<TD ALIGN=RIGHT COLSPAN=2><FONT COLOR=#BF4640>%ld</TD></TR>\n", 
	      "Unique Objects", 
		top_obj_index);
  fprintf(fp, "<TR><TD ALIGN=LEFT><FONT COLOR=02687D>%s</TD>"
	      "<TD ALIGN=RIGHT COLSPAN=2><FONT COLOR=#BF4640>%d</TD></TR>\n", 
	       "Player Run Shops", 
		mudstats.player_shops);
  fprintf(fp, "</TABLE>\n");
  fprintf(fp, "</BODY></HTML>\n");
  fclose(fp);
  return;

  sprintf(buf, "%-25s %14ld\n\r", "Areas", top_area);
  sprintf(buf, "%-25s %14ld\n\r", "Rooms", top_room);
  sprintf(buf, "%-25s %14ld\n\r", "Mobs", top_mob_index);
  sprintf(buf, "%-25s %14ld\n\r", "Objects", top_obj_index);
  sprintf(buf, "%-25s %14d\n\r", "Player Run Stores", mudstats.player_shops);
}

void do_printmud(CHAR_DATA *ch, char *argument)
{
  web_mudstats();
}

void do_webhelp(CHAR_DATA *ch, char *argument)
{
  helps_to_html();
  send_to_char("Done.\n\r", ch);
}

void helps_to_html()
{
  char letter;
  FILE *fp;

  if ((fp = fopen("/home/httpd/html/accounts/shadows/help.html", "w")) == NULL)
  {
    logf("Fopen: helps_to_html", 0);
    return;
  }

  fprintf(fp, "<html><head><title>Shadows of Time Helps</title>\n");
  fprintf(fp, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\"></head>\n");
  fprintf(fp, "<frameset rows=\"95,451\" cols=\"*\">\n"); 
  fprintf(fp, "<frame name=\"header\" src=\"help_header.html\">\n");
  fprintf(fp, "<frame name=\"body\" src=\"help_body.html\">\n");
  fprintf(fp, "</frameset>\n");
  fclose(fp);

  if ((fp = fopen("/home/httpd/html/accounts/shadows/help_header.html", "w")) == NULL)
  {
    logf("Fopen: helps_to_html", 0);
    return;
  }

  fprintf(fp, "<HTML><TITLE>Shadows of Time Helps</TITLE>\n");
  fprintf(fp, "<BODY BGCOLOR=\"#000000\" FGCOLOR=\"#66666\">\n");
  fprintf(fp, "<FORM METHOD=\"get\" TARGET=\"body\" ACTION=\"http://www.shadowsoftime.com/cgi/search/search.pl\">\n");
  fprintf(fp, "<FONT COLOR=#666666>Keyword:&nbsp</FONT><INPUT NAME=\"Terms\">\n");
  fprintf(fp, "<INPUT TYPE=\"submit\"><BR><BR>\n");
  for (letter = 'A' ; letter <= 'Z' ; letter++)
  {
    fprintf(fp, "<A HREF=\"help_body.html#%c\" target=\"body\">%c</A>&nbsp&nbsp\n", 
                 letter, letter);
  }

  fclose(fp);

  if ((fp = fopen("/home/httpd/html/accounts/shadows/help_body.html", "w")) == NULL)
  {
    logf("Fopen: helps_to_html", 0);
    return;
  }

  fprintf(fp, "<HTML><TITLE>Shadows of Time Helps</TITLE>\n");
  fprintf(fp, "<BODY BGCOLOR=\"#000000\" FGCOLOR=\"#666666\">\n");


  for (letter = 'A' ; letter <= 'Z' ; letter++)
  {
//    fprintf(fp, "<BR><IMG SRC=\"http://www.shadowsoftime.com/images/immredbar.gif\">\n");
    fprintf(fp, "<BR><A NAME=\"%c\"</A><B><FONT COLOR=#666666>%c</FONT></B><BR><BR>\n", 
                 letter, letter);
    help_letter(letter, fp);
    fprintf(fp, "<br><hr color=\"red\">\n");
  }
  fprintf(fp, "</BODY></HTML>\n");
  fclose(fp);
}

void help_letter(char letter, FILE *fp)
{
  HELP_DATA *pHelp;
  char look;
  char name[MIL];
  char *keyword;

  
  for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
  {
    if (pHelp->level >= LEVEL_IMMORTAL - 1) // temp fix 
      continue;

    if (isalpha(pHelp->keyword[0]))
      look = pHelp->keyword[0]; 
    else
      look = pHelp->keyword[1];
    if (look != letter && look != 
             tolower(letter))
      continue;

    keyword = str_dup( pHelp->keyword); 
    keyword = one_argument(keyword, name);    
    print_one_help(pHelp, name);
    fprintf(fp, "<A HREF=\"helps/%s.html\">%s</A><BR>\n", name, name);
    free_string(keyword);
  }
}

void print_one_help(HELP_DATA *help, char *name)
{ 
  FILE *fp;
  const char *point;
  char last_color = '\0';
  char buf[MSL];

  sprintf(buf, "/home/httpd/html/accounts/shadows/helps/%s.html", name);

  if ((fp = fopen(buf, "w")) == NULL)
  {
    logf("Fopen: printone_letter: %s", buf);
    return;
  }

  fprintf(fp, "<HTML><TITLE>%s</TITLE>\n", name);
  fprintf(fp, "<H2><FONT COLOR=#666666>%s</FONT></H2>\n", help->keyword);
  fprintf(fp, "<BODY BGCOLOR=#000000>\n");
//  fprintf(fp, "<IMG SRC=\"../images/redbar.gif\"><BR><BR>\n");
    fprintf(fp, "<hr color=\"red\">\n");

  fprintf(fp, "<PRE><FONT COLOR=#666666>\n");

  for ( point = help->text; *point; point++)
  {
    if (*point == '`')
    {
      point++;
      last_color = colour_tag(*point, fp, last_color);
      continue;
    }
    fprintf(fp, "%c", *point);
  }
  fprintf(fp, "</FONT></PRE>\n");   
//  fprintf(fp, "<IMG SRC=\"../images/redbar.gif\"><BR>\n");
    fprintf(fp, "<hr color=\"red\">\n");
  fprintf(fp, "</BODY></HTML>\n");
  fclose(fp);
}

char colour_tag(char type, FILE *fp, char last_color)
{
  if (type == '\0')
  {
    return 0;
  }

  switch(type) 
  {
    default:
      break;

    case '*':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#666666></FONT>");
        return '\0';
      } 
      break;

    case '7':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#666666></FONT>");
        return '\0';
      } 
      break;

    case '1':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#990000>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#990000>");
      }
      return type;
      break;

    case '!':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#FF0000>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#FF0000>");
      }
      return type;
      break;

    case '2':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#006600>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#006600>");
      }
      return type;
      break;

    case '@':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#00CC00>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#00CC00>");
      }
      return type;
      break;

    case '3':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#996600>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#996600>");
      }
      return type;
      break;

    case '#':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#FFCC33>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#FFCC33>");
      }
      return type;
      break;

    case '4':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#000066>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#000066>");
      } 
      return type;
      break;

    case '$':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#3333FF>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#3333FF>");
      }
      return type;
      break;

    case '5':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#663399>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#663399>");
      } 
      return type;
      break;

    case '%':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#CC00999>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#CC0099>");
      } 
      return type;
      break;

    case '6':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#009999>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#009999>");
      } 
      return type;
      break;

    case '^':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#3399FF>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#3399FF>");
      } 
      return type;
      break;

    case '&':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#BBBBBB>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#BBBBBB>");
      } 
      return type;
      break;

    case '8':
      if (last_color != '\0')
      {
        fprintf(fp, "</FONT><FONT COLOR=#444444>");
      }
      else
      {
        fprintf(fp, "<FONT COLOR=#444444>");
      } 
      return type;
      break;

  }

  return '\0'; 
}

void print_head(FILE *fp, char *title)
{
  fprintf(fp, "<HTML><HEAD><TITLE>%s</TITLE><HEAD>\n", title);
  fprintf(fp, "<BODY BGCOLOR=#000000 text=#666666>\n");
  fprintf(fp, "<H1>%s</H1>\n", title);
}

void print_tail(FILE *fp)
{
  fprintf(fp, "</BODY></HTML>\n");
}

void export_note(NOTE_DATA *note)
{
  char buf[MSL];
  FILE *fp;
  const char *point;
  char last_color = '\0';

  sprintf(buf, "%s%s%ld.html", EXPORT_NOTE_FOLDER, note->sender, note->date_stamp);
  if ((fp = fopen(buf, "w")) == NULL)
  {
    bug("export_note: fopen", 0);
    return;
  }
  
  sprintf(buf, "Note by %s entitles %s", note->sender, note->subject);
  print_head(fp, buf);
  fprintf(fp, "\n\r<PRE>\n\r");
    
  for ( point = note->text; *point; point++)
  {
    if (*point == '`')
    {
      point++;
      last_color = colour_tag(*point, fp, last_color);
      continue;
    }
    fprintf(fp, "%c", *point);
  }
  fprintf(fp, "\n\r</PRE>\n\r");
  print_tail(fp);
  fclose(fp);
}

void export_note(NOTE_DATA *note, FILE *fp)
{
  const char *point;
  char last_color = '\0';

  fprintf(fp, "<BR>");
  fprintf(fp, "</FONT><FONT COLOR=#BBBBBB>%s:</FONT>", note->sender);
  fprintf(fp, "<FONT COLOR=#666666> %s\n", note->subject);
  fprintf(fp, "<BR>%s", note->date);
  fprintf(fp, "<BR></FONT><FONT COLOR=#BBBBBB>To:</FONT>");
  fprintf(fp, "<FONT COLOR=#666666>  %s\n", note->to_list);
  fprintf(fp, "\n<PRE>\n");

  for ( point = note->text; *point; point++)
  {
    if (*point == '`')
    {
      point++;
      last_color = colour_tag(*point, fp, last_color);
      continue;
    }
    fprintf(fp, "%c", *point);
  }
  fprintf(fp, "\n</PRE>\n");
}

