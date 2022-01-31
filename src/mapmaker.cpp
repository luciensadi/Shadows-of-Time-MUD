/* Asmo's random generation */
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "mapmaker.h"

const char *forest_desc[MAX_FOREST] =
{
  "The creaking of tree limbs overhead mingle with the cries of birds and\n"
  "the chittering of small mammals in the woods all around.  Fallen leaves\n"
  "rustle with the stirring of the wind, crackling underfoot and exuding\n"
  "the fragrance of slow decay.  The trail here winds around thick tree\n"
  "trunks and between short ferns growing green beside it.\n",

  "The trail narrows to squeeze between mossy trunks of trees, then widens\n"
  "into bright clearings, thick with the verdant grass and blooming flowers.\n"
  "The dappled shape through overhead boughs shelter a miasma of pale fungi,\n"
  "from tiny caps to the large shelf mushrooms springing from the sides of\n"
  "the old trees.\n",

  "As you walk through the low vines and leaves that inhabit the ground,\n"
  "you notice the density of the forest surrounding you.  Fallen trees\n"
  "and huge limbs make it difficult to travel.  In some parts, sunlight\n"
  "can barely penetrate the canopy, making the terrain hard to judge.  A\n"
  "few animals can be seen from time to time, seeming startled by your\n"
  "presence.  Fortunately the forest looks a little less rough in the\n"
  "distance.\n"
};       
   
     
  


void create_room(int vnum)
{
  AREA_DATA *pArea;
  ROOM_INDEX_DATA *pRoom;
  int iHash;
 
    pArea = get_vnum_area(vnum); 

    if ( get_room_index( vnum ))
    {
      logf("Layout: Room already exists.\n\r", 0);
    }

    pRoom = new_room_index();
    pRoom->area = pArea;
    pRoom->vnum = vnum;
    
    if (vnum > top_vnum_room)
      top_vnum_room = vnum;
   
    iHash = vnum % MAX_KEY_HASH;
    pRoom->next = room_index_hash[iHash];
    room_index_hash[iHash] = pRoom;
}

void layout_map(int minvnum, int maxvnum, sh_int type)
{
  int vnum;
  int in_room_vnum;
  AREA_DATA *pArea;

  vnum = minvnum;
  pArea = get_vnum_area(vnum); 

  if (!pArea)
  {
    logf("Layout: That vnum is not assigned an area.\n\r", 0);
    return;
  }

  create_room(vnum);
  in_room_vnum = vnum;  
  for ( ; ; )
  {
    int door;
    ROOM_INDEX_DATA *inRoom;
    EXIT_DATA *pExit;
  
    inRoom = get_room_index(in_room_vnum);
    switch (type)
    {
      case MAP_RANDOM:  // Normal Area
        door = number_range(0, 3);
        break;
      case MAP_EAST: // East and West Road
        {
          int stray;
          stray = number_range(0, 4);
          if (stray < 4)
          {
            door = number_range(0, 1);
            if (door == 0)
              door = 3;
          }
          else
          {
            door = number_range(0,3);
          }
        }
        break;            
     case MAP_NORTH: // North and South Road
         {
          int stray;
          stray = number_range(0, 4);
          if (stray < 4) 
          {
            door = number_range(0, 1);
            if (door == 1)
              door = 2;
          }
          else
          {
            door = number_range(0,3);
          }
        }            
        break;
      default:
        door = number_range(0, 3);
        break;
    }

    if ((pExit = inRoom->exit[door]) == NULL)
    {
      ROOM_INDEX_DATA *toRoom;
      vnum++;
      create_room(vnum);
      toRoom = get_room_index(vnum);
      
      if ( !inRoom->exit[door] )
        inRoom->exit[door] = new_exit();
   
      inRoom->exit[door]->u1.to_room = toRoom;
      inRoom->exit[door]->orig_door = door;

      door = rev_dir[door];
      pExit = new_exit();
      pExit->u1.to_room = inRoom;
      pExit->orig_door = door;
      toRoom->exit[door] = pExit;
     
      if (vnum + 1 >= maxvnum)
        break;
   }
   else
   {
     in_room_vnum = inRoom->exit[door]->u1.to_room->vnum;
   }
 }
}       

void fill_map(int minvnum, int maxvnum, sh_int type)
{
  int i, random;
  ROOM_INDEX_DATA *pRoom;

  for (i = minvnum; i <= maxvnum; i++)
  {
    if ((pRoom = get_room_index(i)) == NULL)
      continue;

    switch (type)
    {
      case FOREST:
        {
           random = number_range(0, MAX_FOREST - 1);
           pRoom->description = str_dup(forest_desc[random]);
           pRoom->name = str_dup("Walking in the Forest");         
        if (number_percent() > 80)
        {
          RESET_DATA *pReset = NULL;
 
          pReset = new_reset_data();
          pReset->command = 'M';
          pReset->arg1 = number_range(4452, 4453);
          pReset->arg2 = 2;
          pReset->arg3 = pRoom->vnum;
          pReset->arg4 = (maxvnum - minvnum) / 5;
 
          if (!pRoom->reset_first)
          {
            pRoom->reset_first = pReset;
            pRoom->reset_last = pReset;
            pReset->next = NULL;
            break;
          }   

          pReset->next = pRoom->reset_first;
          pRoom->reset_first = pReset;
        } 
       }
        break;
      break;
    }
  }
}

void do_fillmap(CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];
  int min, max;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("Min and Max Vnum?\n\r", ch);
    return;
  }

  if (arg3[0] == '\0')
  {
    send_to_char("Forest, Ocean or Road?\n\r", ch);
    return;
  }

  min = atoi(arg1);
  max = atoi(arg2);

  if (!str_cmp(arg3, "forest"))
  {
    fill_map(min, max, FOREST);
    send_to_char("Done.\n\r", ch);
  }
  else
  {
    send_to_char("Not currently implemented.\n\r", ch);
  }
}


void do_makemap(CHAR_DATA *ch, char *argument)
{
  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];
  int min, max, i;
 

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("Min and Max Vnum?\n\r", ch);
    return;
  }

  if (arg3[0] == '\0')
  {
    send_to_char("Random Map, Northern Road or Eastern Road?\n\r", ch);
    return;
  }

  min = atoi(arg1);
  max = atoi(arg2);

  for ( i = min; i < max + 1;i++)
  {
     if (get_room_index(i))
     {
       send_to_char("That area already has rooms in it.\n\r", ch);
       return;
     }
  }

  if (!str_cmp(arg3, "random"))
  {
    layout_map(min, max, 0);
  }
  else if (!str_cmp(arg3, "east"))
  {
    layout_map(min, max, 1);
  }
  else if (!str_cmp(arg3, "north"))
  {
    layout_map(min, max, 2);
  }
  else
  {
    send_to_char("Choices are north, east or random\n\r", ch);
    return;
  } 

}

