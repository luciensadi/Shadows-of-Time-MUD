/* Yada Yada Store Code Seperated so it can be easily snippitized */

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
#include "stores.h"
#include "lookup.h"

void fwrite_obj  args ((CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int nest));
void new_sold    args ((STORE_DATA *store, char *name, int cost));
void show_obj_stat args ((CHAR_DATA *ch, OBJ_DATA *obj));

const struct store_cmd store_cmd_table[] = {
  {"add_item", add_item},
  {"update_item", update_item},
  {"empty_coffer", empty_coffer},
  {"inventory", do_inventory},
  {"desc", store_desc},
  {"hire", store_hire},
  {"fire", store_fire},
  {"sold", store_sold},
  {"owner", store_owner},
  {"remove", store_remove},
  {"??", store_help},
  {"", 0},
};
bool have_store (CHAR_DATA * ch)// The actual bool check to see if player already has a store - Camm
{
  STORE_DATA * store;
  for (store = store_list;store != NULL;store = store->next)//For loop to search through store_list
  {
    //Do the actual if check to see if the char already owns a store if a match is found return true
    if (!str_cmp (store->owner, IS_DISGUISED(ch)?ch->pcdata->disguise.orig_name:ch->name))
    {
    	return TRUE;
    }

  }
  //If no match is found of course return false so that they can continue to purchase the store
  return FALSE;
}

bool run_store_editor( DESCRIPTOR_DATA *d )
{
  if (d->editor == ED_STORE)
  {
    store_edit(d->character, d->incomm);
    return TRUE;
  }
  return FALSE;
}

void do_store_edit( CHAR_DATA *ch, char *argument)
{
  STORE_DATA *store;
  int dues, days;
  char arg[MIL];
  char buf[MSL]; 
  
  if ( IS_NPC(ch))
    return;

  argument = one_argument(argument, arg);

  if (!str_cmp(arg, "purchase"))
  {
    bool guild = FALSE;

    if ( get_skill (ch, gsn_store) < 1)
    {
      send_to_char("Huh?\n\r", ch);
      return; 
    }
    
    if (have_store(ch))
    {//Check to see if character already owns a store - Camm
       	send_to_char ("You already own a store!!\n",ch);
        return;
    }

    if (!str_cmp(argument, "Guild"))
    {
      if (ch->clan > 0 && ch->rank > clan_table[ch->clan].top_rank - 2)
        guild = TRUE;
      else
      {
        send_to_char("You aren't a guildleader.\n\r", ch);
        return;
      }
    }

    if (!IS_SET(ch->in_room->room_flags, ROOM_EMPTY_STORE))
    {
      send_to_char("It's not for sale.\n\r", ch);
      return;
    }
    else if (!guild && ch->questpoints < 1000)
    {
      send_to_char("It cost 1000 questpoints to purchase a store.\n\r", ch);
      return;
    }
    else if (guild && clan_table[ch->clan].roster.questpoints < 2000)
    {
      send_to_char("Your guild doesn't have the necessary 2000 questpoints.\n\r", ch);
      return;
    }
    else
      make_store(ch, guild);
  }


  if ((store = get_char_store(ch)) == NULL)
  {
    send_to_char("This isn't a/your store.\n\r", ch);
    return;
  }

  if (store->shopkeeper.lastpaid != 0)
  {
    days = (current_time - store->shopkeeper.lastpaid) / (24 * 60 * 60);
    if (days < 1)
    {
      ch->desc->editor = ED_STORE;
      ch->desc->pEdit = store;
      return;
    }
 
    dues = days * STORE_DAY_COST;

    if (ch->silver + 100 * ch->gold < dues)
    {
      sprintf(buf, "You owe %d silver for your shopkeepers dues\n\r", dues);
      send_to_char(buf, ch);   
      send_to_char("You must pay your shopkeep before you can do anything\n\r"
                 "with your store.\n\r", ch);
      return;
    }
    else
    {
      sprintf(buf, "You pay your shopkeeper his salary of %d silver.\n\r", dues);
      send_to_char(buf, ch);
      deduct_cost(ch, dues);
      store->shopkeeper.lastpaid = current_time;
    }
  }
 
  ch->desc->editor = ED_STORE;
  ch->desc->pEdit = store;
  return;
}  
 
void store_edit(CHAR_DATA *ch, char *argument)
{
  STORE_DATA *store;

  EDIT_STORE(ch, store);

  save_stores();
  if (argument[0] == '\0' || argument[1] == '\0')
  {
    show_store(ch);
    return;
  }

  if (!str_cmp(argument, "done"))
  {
    send_to_char("Exiting store editor.", ch);
    ch->desc->editor = ED_NONE;
    return;
  }

  store_interpret(ch, argument);
  return;
}

void add_item(CHAR_DATA *ch, char *argument)
{
  STORE_DATA *store;
  OBJ_DATA *obj;
  STORE_LIST_DATA *item;
  STORE_LIST_DATA *list;
  int count = 0;
  int price;
  char arg[MIL];

  EDIT_STORE(ch, store);
  if (store->itemNumber > 10)
  {
    send_to_char("You don't have the shelf space.\n\r", ch);
    return;
  }

  argument = one_argument(argument, arg); 
 
  if ((obj = get_obj_carry(ch, arg, ch)) == NULL)
  {
    send_to_char("You don't have that.\n\r", ch);
    return;
  }

  if (obj->item_type == ITEM_CONTAINER)
  {
    if (obj->contains)
    {
      send_to_char("You can only put empty containers in a store.\n\r", ch);
      return;
    }
  }

  if (argument[0] == '\0' || !is_number(argument))
    price = 0;
  else
    price = atoi(argument); 
  
  if (price <= 0)
  {
    send_to_char("It wouldn't be smart to give away items for free.",ch);
    return;
  }

  for (list = store->selling;list != NULL; list = list->next)
  {
    count++;
  }

  if (count > 20)
  {
    send_to_char("Sorry, but your store cannot sell more than 20 items at once.\n\r", ch);
    return;
  }   

  item = new_slist();
  item->item = obj;
  item->cost = price;
  item->next = store->selling;
  obj->in_store = store;
  store->selling = item;
  act("$p has been added to the store's shelves.\n\r", ch, obj, ch, TO_CHAR);
  obj_from_char(obj);
  save_stores();
  return;
}  

void store_desc(CHAR_DATA *ch, char *argument)
{
  STORE_DATA *store;
  EDIT_STORE(ch, store);   
  if (argument[0] == '\0')
  {
    string_append( ch, &ch->in_room->description );
  }
  send_to_char("Syntax is desc.\n\r", ch);
  SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
  save_area (ch->in_room->area);
}

void store_help(CHAR_DATA *ch, char *argument)
{
    send_to_char("Syntax:\n\r", ch);
    send_to_char("add_item <item>              - add an item to the store list\n\r", ch);
    send_to_char("add_item <item #> <price>    - add an item at a predefined price\n\r", ch);
    send_to_char("desc           	       - go into description editor for store\n\r", ch);
    send_to_char("done			       - exit the store editor\n\r", ch);
    send_to_char("fire			       - fire a shopkeeper\n\r", ch);
    send_to_char("hire			       - hire a shopkeeper for 10k silver a day\n\r", ch);
    send_to_char("sold			       - list last 20 items sold.\n\r", ch);	 
    send_to_char("update_item <item #> <price> - update the price of an item\n\r", ch);
    send_to_char("empty_coffer		       - get qps from the coffer\n\r", ch);
    if (IS_HIIMMORTAL(ch))
    {
    	send_to_char("owner		       - changes the owner of the store\n\r", ch);
    }

    return;
} 
    

void empty_coffer(CHAR_DATA *ch, char *argument)
{
  STORE_DATA *store;
  int total;
  char buf[MSL];

  EDIT_STORE(ch, store);

  if (store->coffer == 0)
  {
    send_to_char("There's no money in there.\n\r", ch);
    return;
  }

  total = store->coffer;

  sprintf(buf, "You get %d questpoints from the coffer.\n\r", total);
  send_to_char(buf, ch);

  ch->questpoints += total;
  store->coffer = 0;
  
}

void update_item(CHAR_DATA *ch, char *argument)
{
  int number, cost, i;
  char arg1[MIL];
  char arg2[MIL];
  STORE_LIST_DATA *list;
  STORE_DATA *store;
  bool found = FALSE;

  EDIT_STORE(ch, store);

  if ((store->guild && ch->rank < 4)
     && ch->level < 81)
  {
    send_to_char("You can't do that, only GLs can update prices.\n\r", ch);
    return;
  } 

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  
  if (!is_number(arg1) || !is_number(arg2))
  {
    send_to_char("Pick an item number and a cost.\n\r", ch);
    return;
  }

  number = atoi(arg1);
  cost = atoi(arg2);

  i = 1;
  for (list = store->selling;list != NULL;list = list->next)
  {
    if (i == number)
    { 
      found = TRUE;
      break;
    }
    i++;
  }

  if (!found)
  {
    send_to_char("That item number does not exist.\n\r", ch);
    return;
  }

  list->cost = cost;
  act("$p's cost has been updated.", ch, list->item, NULL, TO_CHAR);
  return;
}
void store_owner(CHAR_DATA *ch,char *argument)
{
    ROOM_INDEX_DATA *room;
    STORE_DATA *store;
    char arg1[MIL];
    char arg2[MIL];
    bool guild;
    char owner[MSL];
    char name[MSL];
    char buf[MSL];
    CHAR_DATA *keeper;

    argument = one_argument(argument,arg1);
    argument = one_argument(argument,arg2);
    EDIT_STORE (ch,store);
    guild = FALSE;
    room = ch->in_room;
    if (!IS_HIIMMORTAL(ch))
    {
    	send_to_char("You do not have access to this command.\n\r",ch);
    	return;
    }
    if ((arg1[0]=='\0')||(arg2[0]=='\0'))
    {
    	send_to_char("Syntax: owner <guild/char> <name>\n\r",ch);
    	return;
    }
    if (!str_cmp(arg1,"guild"))
    {    	
    	if (clan_lookup(arg2)!=-1)
    	    guild = TRUE;
    	else
    	{
    	   send_to_char("That is not a valid clan.\n\r",ch);
    	   return;
    	}    	
    }
    if (!str_cmp(arg1,"char"))
    {
    	if (!check_player(arg2))
    	{
    	   send_to_char("That is not a valid player.\n\r",ch);
    	   return;
    	}
    	sprintf (name,"%s",str_dup(capitalize(arg2)));
    }

    if (guild)
        sprintf(owner, "%s", clan_table[clan_lookup(arg2)].display);
    else
        sprintf(owner, "%s", name);

    free_string(room->name);
    sprintf(buf, "%s's Store", owner);
    room->name = str_dup(buf);
    free_string(store->owner);
    store->owner = str_dup(owner);
    store->guild = guild ? ch->clan : 0;
    free_string(room->description);
    sprintf(buf, "%s's store is waiting to be decorated and filled.", owner);
    room->description = str_dup(buf);
    SET_BIT(room->area->area_flags, AREA_CHANGED);

    if((keeper = get_char_room (ch, NULL, store->shopkeeper.keeper)))
    {
	free_string(keeper->name);
	sprintf(buf, "%s Shopkeep", owner);
	keeper->name = str_dup(buf);
	send_to_char("Renamed current shopkeeper.\n\r", ch);
    }
    return;
}
  
  

void show_store(CHAR_DATA *ch)
{
  STORE_DATA *store;
  STORE_LIST_DATA *list;
  int i;
  char buf[MSL];

  EDIT_STORE(ch, store);
 
  sprintf(buf, "Vnum:   %ld\n\r", store->vnum);
  send_to_char(buf, ch);
  if (store->guild > 0)
  {
    send_to_char("Guild Store.\n\r", ch);
  }  
  sprintf(buf, "Keeper: %s\n\r", store->shopkeeper.keeper);
  send_to_char(buf, ch);
  sprintf(buf, "Coffer: %d\n\r\n\r", store->coffer);
  send_to_char(buf, ch);
  send_to_char("List of wares:\n\r", ch);

  i = 1;
  for (list = store->selling;list != NULL; list = list->next)
  {
    sprintf(buf, "%d - %s  : %d\n\r", i, list->item->short_descr, list->cost);
    send_to_char(buf, ch);
    i++;
  }
}

void make_store(CHAR_DATA *ch, bool guild)
{
  ROOM_INDEX_DATA *room;
  STORE_DATA *store;
  char buf[MIL];
  char owner[MSL];
  room = ch->in_room;
  REMOVE_BIT(room->room_flags, ROOM_EMPTY_STORE); 


  if (guild)
    sprintf(owner, "%s", clan_table[ch->clan].display);
  else
    sprintf(owner, "%s", ch->name);

  free_string(room->name);
  sprintf(buf, "%s's Store", owner);
  room->name = str_dup(buf);

  free_string(room->description);
  sprintf(buf, "%s's store is waiting to be decorated and filled.", owner);
  room->description = str_dup(buf);
  SET_BIT(room->area->area_flags, AREA_CHANGED);

  store = new_store();
  store->vnum = room->vnum;
  store->owner = str_dup(owner);
  store->guild = guild ? ch->clan : 0;
  store->shopkeeper.keeper = str_dup(ch->name);
  send_to_char(store->shopkeeper.keeper, ch);
  store->shopkeeper.mob = FALSE;
  store->selling = NULL;
  store->itemNumber = 0;
  VALIDATE(store);
  store->next = store_list;
  store_list = store;
  room->store = store;
  SET_BIT(room->room_flags, ROOM_SAFE);
  ch->desc->pEdit = store;
  if (guild)
    clan_table[ch->clan].roster.questpoints -= 2000;
  else
    ch->questpoints -= 1000;
  mudstats.player_shops += 1;
  sprintf(buf, "%s bought a store", ch->name);
  logf(buf, 0);
  return;
}

STORE_DATA *get_char_store(CHAR_DATA *ch)
{
  STORE_DATA *store;
  

  for (store = store_list;store != NULL; store = store->next)
  {
    if (store->vnum == ch->in_room->vnum 
      && (!str_cmp(store->owner, ch->name) 
      ||(store->guild > 0 
      && store->guild == ch->clan)|| IS_HIIMMORTAL(ch)))
       return store;
  }
  return NULL;
}  

void store_do_list(CHAR_DATA *ch, char *argument)
{
  STORE_DATA *store;
  STORE_LIST_DATA *list;
  STORE_LIST_DATA *list2;
  char holder[MSL];
  char buf[MSL];
  char arg1[MIL];
  int count = 1;

  store = ch->in_room->store;

  if (store == NULL)
    return;
  
  if (store->selling == NULL)
  {
    send_to_char("There's nothing being sold here.\n\r", ch);
    return;
  }

  argument = one_argument(argument, arg1);

  if (!str_cmp(arg1, "stat"))
  {
    int count;
    STORE_LIST_DATA *purchase = NULL;
    char arg[MIL];
    bool found = FALSE;
    int number;

    number = number_argument(argument, arg);
    count = 0;
    for ( list = store->selling; list != NULL; list = list->next )
    {

      if (is_name(arg, list->item->name))
      {
        if (++count == number)
        {
          purchase = list;
          found = TRUE;
          break;
        }
      }
    }

    if (!found)
    {
      send_to_char("That's not being sold here type list.\n\r", ch);
      return;
    }

    show_obj_stat(ch, purchase->item);
    return;
  }
  send_to_char("[Lv  Price Qty] Item\n\r", ch);
  for (list = store->selling;list != NULL; list = list->next)
  {
    char tmp[MIL];
    count = 1;
    sprintf(tmp, "%ld", list->item->pIndexData->vnum);
    if (strstr(holder, tmp))
      continue;
    for (list2 = list->next; list2 != NULL; list2 = list2->next)
    {
      if (!str_cmp(list->item->short_descr, list2->item->short_descr))
        count++; 
    }
    sprintf(tmp, "%ld ", list->item->pIndexData->vnum);
    strcat(holder, tmp); 
    sprintf(buf, "[%3d %5d  %d ] %s\n\r", 
        list->item->level, list->cost, count, list->item->short_descr);
    send_to_char(buf, ch);
  }
  sprintf(holder, " ");
}

void store_do_buy(CHAR_DATA *ch, char *argument)
{
  char buf[MSL];
  bool found = FALSE;
  int cost;
  OBJ_DATA *obj;
  STORE_DATA *store;
  STORE_LIST_DATA *list, *list_next;
  STORE_LIST_DATA *purchase = NULL;
  char arg[MIL];
  int number, count;

  store = ch->in_room->store;

  if (get_char_room (ch, NULL, store->shopkeeper.keeper) == NULL)
  {
    send_to_char("There's no one here to sell to you.\n\r", ch);
    return;
  }    

  if ( store == NULL )
    return;

  number = number_argument(argument, arg);
  count = 0;

  for ( list = store->selling; list != NULL; list = list_next )
  {
    list_next = list->next;

    if (is_name(arg, list->item->name))
    {
      if (++count == number)
      {
        purchase = list;
        found = TRUE;
        break;
      }
    }
  }

  if (!found)
  {
    send_to_char("That's not being sold here type list.\n\r", ch);
    return;
  }

  obj = purchase->item;
  cost = purchase->cost;

  if (cost < 1)
  {
    send_to_char("It's not ready for sale yet.\n\r", ch);
    return;
  }

  if (ch->clan == clan_lookup("Tinker") && obj->pk == TRUE)
  {
    send_to_char("You look at buying that item, then realize that it has been touched by violence.\n\r", ch);
    send_to_char("You suddenly feel ill to your stomach and decide to look for something else.\n\r", ch);
    return;
  }

  if ( ch->questpoints < cost)
  {
    send_to_char("You can't afford that.\n\r", ch);
    return;
  }

  if (ch->carry_number + get_obj_number(obj) > can_carry_n(ch))
  {
    send_to_char("You can't carry that many items.\n\r", ch);
    return;
  }

  if ( ch->carry_weight + get_obj_weight(obj) > can_carry_w(ch))
  {
    send_to_char("You can't carry that much weight.\n\r", ch);
    return;
  }

  ch->questpoints -= cost;
  sprintf( buf, "%s sells %s to %s", store->shopkeeper.keeper, obj->short_descr, ch->name); 
  act(buf, ch, obj, NULL, TO_ROOM);
  act("You buy $p", ch, obj, NULL, TO_CHAR);  
  save_stores();
  if (cost > 1)
    new_sold(store, obj->short_descr, cost);
  else
    logf("%s sold to %s for %d qp.", obj->short_descr, ch->name, cost);
  obj_to_char(obj, ch);
  obj->in_store = NULL;
  if (store->guild > 0)
    clan_table[store->guild].roster.questpoints += cost;
  else
    store->coffer += cost;

  if (store->selling == purchase)
  {
    store->selling = purchase->next;
  }

  for (list = store->selling; list != NULL; list = list_next)
  {
    list_next = list->next;
    if (list->next == purchase)
    {
      list->next = purchase->next;
      break;
    }
  }
  free_slist(purchase);

}

void store_interpret(CHAR_DATA *ch, char *argument)
{
  bool found = FALSE;
  char arg[MIL];
  int cmd;

  argument = one_argument(argument, arg);


  for ( cmd = 0; store_cmd_table[cmd].name[0] != '\0';cmd++)
  {
    if ( !str_prefix(arg, store_cmd_table[cmd].name) )
    {
      found = TRUE;
      break;
    }
  }

  if (found)
  {
    (store_cmd_table[cmd].store_fun) (ch, argument);
  }
  else
  {
    show_store(ch);
  }
}

void do_save_store(CHAR_DATA *ch, char *argument)
{
  save_stores();
  send_to_char("Ok.\n\r", ch);
}
  
void save_one_store(STORE_DATA *store)
{
  char buf[MIL];
  char tmp[MIL];
  FILE *fp;

  colourstrip(tmp, store->owner);
  sprintf(buf, "../data/store/%s", tmp);
  if ((fp = fopen(buf, "w")) == NULL)
  {
    bug("Save one store: fopen", 0);
    return;
  }

  fwrite_store(store, fp);
  fprintf(fp, "#End\n");
  fclose(fp);
}

void save_stores()
{
  STORE_DATA *list;
  FILE *fp;

  if ((fp = fopen("../data/store/stores.lst", "w")) == NULL)
  {
    bug( "Save_stores: fopen", 0);
  }

  fprintf(fp, "#STORES\n");
  for (list = store_list;list != NULL;list = list->next)
  {
    char tmp[MIL];
    colourstrip(tmp, list->owner);
    fprintf(fp, "\"%s\"\n", tmp);
    save_one_store(list);
  }
  
  fprintf(fp, "#End\n");
  fclose(fp);
}

void fwrite_store(STORE_DATA *store, FILE *fp)
{
  STORE_LIST_DATA *list;
  SOLD_LIST_DATA *sold;

  fprintf(fp, "#STOREOBJ\n");
  fprintf(fp, "Vnum %ld\n", store->vnum);
  fprintf(fp, "Owner %s~\n", store->owner);
  fprintf(fp, "Guild %d\n", store->guild);
  fprintf(fp, "Coffer %d\n", store->coffer);
  fprintf(fp, "Keeper %s~\n", store->shopkeeper.keeper);
  fprintf(fp, "KeeperVnum %ld\n", store->shopkeeper.vnum); 
  fprintf(fp, "LPaid %ld\n", store->shopkeeper.lastpaid);
  
  for (list = store->selling; list != NULL; list = list->next)
  {
    fprintf(fp, "#ITEM\n");
    fprintf(fp, "Cost %d\n", list->cost);
    fwrite_obj(NULL, list->item, fp, 0);
  }

  for (sold = store->sold; sold != NULL; sold = sold->next)
  {
    fprintf(fp, "#SOLD\n");
    fprintf(fp, "Name %s~\n", sold->name);
    fprintf(fp, "Cost %d\n", sold->cost);
  }
}  

void do_load_store(CHAR_DATA *ch, char *argument)
{
  load_stores();
  store_reset();  
  send_to_char("Ok\n\r", ch);
}

void load_one_store(char *name)
{
  STORE_DATA *store = NULL;
  FILE *fp;
  char *word;
  char tmp[MIL];
 
  sprintf(tmp, "../data/store/%s", name);
  if ((fp = fopen(tmp, "r")) == NULL)
  {
    bugf("Load_one_store: fopen %s", tmp);
    return;
  }
  

  for ( ; ; )
  {
    word = fread_word(fp);
    switch (word[0])
    {
       case '#':
         if (!str_cmp("#STOREOBJ", word))
         {
            store = new_store();
            store->next = store_list;
            store_list = store;
            mudstats.player_shops += 1;
            word = fread_word(fp);
            store->vnum = fread_number(fp);
	    word = fread_word(fp);
            store->owner = fread_string(fp);
	    word = fread_word(fp);
	    store->guild = fread_number(fp);
            word = fread_word(fp);
	    store->coffer = fread_number(fp);
            word = fread_word(fp);
            store->shopkeeper.keeper = fread_string(fp);
            if (str_cmp(store->shopkeeper.keeper, store->owner) &&
                str_cmp(store->shopkeeper.keeper, "Shopkeep"))
            {
              free_string(store->shopkeeper.keeper);
              store->shopkeeper.keeper = str_dup("Shopkeep");
            }
            word = fread_word(fp);
            store->shopkeeper.vnum = fread_number(fp);
            if (store->shopkeeper.vnum != 0)
             store->shopkeeper.mob = TRUE;
            word = fread_word(fp);
            store->shopkeeper.lastpaid = fread_number(fp);
            break;
         }

         if (!str_cmp("#ITEM", word))
         {
           fread_item(store, fp);
           break;
         }

         if (!str_cmp("#SOLD", word))
         {
           fread_sold(store, fp);
           break;
         }

         if (!str_cmp("#END", word))
         {
	   fclose(fp);	
           return;
         }
         break;

 /*      case 'C':
         if (!str_cmp("Coffer", word))
         {
           store->coffer = fread_number(fp);
           break;
         }
	 break;

       case 'G':
         if (!str_cmp("Guild", word))
         {
           store->guild = fread_number(fp);
           break;
         }
         break; 

       case 'K':
	 if (!str_cmp("Keeper", word))
         {
           store->shopkeeper.keeper = fread_string(fp);
           break;
         }
         
         if (!str_cmp("KeeperVnum", word))
         {
           store->shopkeeper.vnum = fread_number(fp);
           if (store->shopkeeper.vnum != 0)
             store->shopkeeper.mob = TRUE;
           break;
         }
         break;

       case 'L':
         if (!str_cmp("LPaid", word))
         {
           store->shopkeeper.lastpaid = fread_number(fp);
           break;
         }       
	 break;

       case 'O':
         if (!str_cmp("Owner", word))
         {
           store->owner = fread_string(fp);
           break;
         }
         break;

       case 'V':
         if (!str_cmp("Vnum", word))
         {
           store->vnum = fread_number(fp);
           break;
         }
         break; */        
     }
  }
}

void load_stores(void)
{
  FILE *fp;  
  char *sname;
  
  if ((fp = fopen("../data/store/stores.lst", "r")) == NULL)
  {
    bug("Load_stores: fopen", 0);
    return;
  }

  sname = fread_word(fp);
  if (!str_cmp(sname, "#End"))
    return;
 
  for ( ; ; )
  {
    sname = fread_word(fp);
    if (!str_cmp(sname, "#End"))
      break;
    load_one_store(sname);
  }
  store_reset();
  fclose(fp);
}

void fread_item(STORE_DATA *store, FILE *fp)
{
  STORE_LIST_DATA *item;
  OBJ_DATA *obj;

  item = new_slist();
  fread_word(fp);
  item->cost = fread_number(fp);
  fread_word(fp);
  obj = fread_obj_store(fp);
  item->item = obj;
  obj->in_store = store;
  item->next = store->selling;
  store->selling = item;
}

void fread_sold(STORE_DATA *store, FILE *fp)
{
  SOLD_LIST_DATA *sold;

  sold = new_soldlist();
  fread_word(fp);
  sold->name = fread_string(fp);
  fread_word(fp);
  sold->cost = fread_number(fp);
  sold->next = store->sold;
  store->sold = sold;
}

void store_reset(void)
{
  char buf[MSL];
  STORE_DATA *list;
  ROOM_INDEX_DATA *room;
  CHAR_DATA *keeper;

  for (list = store_list; list != NULL ; list = list->next)
  {
    if ((room = get_room_index(list->vnum)) == NULL)
    {
      logf("Store_Reset: Bad Vnum", 0);
      continue;
    }

    room->store = list;
    if (list->shopkeeper.vnum != 0)
    {
    
      if ((keeper = create_mobile( get_mob_index(list->shopkeeper.vnum) ) ) == NULL)  
      {
        continue;
      }
      sprintf(buf, "%s %s", list->owner, keeper->name);
      free_string(keeper->name);
      keeper->name =  str_dup(buf);
      char_to_room(keeper, get_room_index(list->vnum));
    }
    
    REMOVE_BIT(room->room_flags, ROOM_EMPTY_STORE);
  }
}

void store_hire(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *keeper;
  STORE_DATA *store;

  EDIT_STORE(ch, store);
  
  if (store->shopkeeper.vnum != 0)
  {
    send_to_char("You already have a shopkeeper.\n\r", ch);
    return;
  }

  if ((ch->silver + 100 * ch->gold) < STORE_DAY_COST)
  {
    send_to_char("You can't afford a shopkeep, they are 10 gold a day.\n\r", ch);
    return;
  }

  deduct_cost(ch, STORE_DAY_COST);
  store->shopkeeper.lastpaid = current_time;
  store->shopkeeper.vnum = VNUM_MOB_SHOPKEEP;
  free_string(store->shopkeeper.keeper);
  store->shopkeeper.keeper = str_dup("Shopkeep");
  if ((keeper = create_mobile( get_mob_index(store->shopkeeper.vnum) ) ) == NULL)  
  {
    send_to_char("Bug, report to Asmo.\n\r", ch);
    return;
  }
  char_to_room(keeper, get_room_index(store->vnum));
  act("$N shows up ready to work.", ch, NULL, keeper, TO_CHAR);
}

void store_sold(CHAR_DATA *ch, char *argument)
{
  STORE_DATA *store;
  SOLD_LIST_DATA *sold;
  char buf[MSL];
  int count = 0;

  EDIT_STORE(ch, store);

  if (store->sold == NULL)
  {
    send_to_char("You haven't sold anything recently.\n\r", ch);
    return;
  }

  send_to_char("Recently sold:\n\r", ch);
  for (sold = store->sold; sold != NULL; sold = sold->next)
  {
    sprintf(buf, "%-20s: %5d\n\r", sold->name, sold->cost);
    count++;
    send_to_char(buf, ch);
  }
 
  if (count > 25)
  {
    send_to_char("Automatically cleaning sold list.\n\r", ch);
    store_clear(ch, "auto", (count - 25));
    return;
  }

}

void store_clear(CHAR_DATA *ch, char *argument, int count)
{
  STORE_DATA *store;
  SOLD_LIST_DATA *sold, *temp;
  int i = 1;
  
  EDIT_STORE(ch, store);
 
  sold = store->sold;

  while (sold->next != NULL)
  {
    i++;
    if (i > 25)
    {
      temp = sold->next;
      sold->next = temp->next;
      free_soldlist(temp);
    }
    else
      sold = sold->next;
  }

  send_to_char("Sold list cleared.\n\r", ch);
}

void store_fire(CHAR_DATA *ch, char *argument)
{
  STORE_DATA *store;
  CHAR_DATA *keeper;

  EDIT_STORE(ch, store);
  
  if (store->shopkeeper.vnum == 0)
  {
    send_to_char("You don't have a shopkeeper.\n\r", ch);
    return;
  }

  if ((keeper = get_char_world(ch, "shopkeep")) == NULL)
  {
    send_to_char("Bug, report to Asmo.\n\r", ch);
    return;
  }

  store->shopkeeper.vnum = 0;
  free_string(store->shopkeeper.keeper);
  store->shopkeeper.keeper = str_dup(ch->name);
  store->shopkeeper.lastpaid = 0;
  act("You send $N packing.", ch, NULL, keeper, TO_CHAR);
  extract_char(keeper, TRUE);
}

void new_sold(STORE_DATA *store, char *name, int cost)
{
  SOLD_LIST_DATA *sold;

  sold = new_soldlist();
  sold->name = str_dup(name);
  sold->cost = cost;
  sold->next = store->sold;
  store->sold = sold;
}

void store_remove(CHAR_DATA *ch, char *argument)
{
  SOLD_LIST_DATA *sold, *sold_next;
  STORE_LIST_DATA *item, *item_next;
  STORE_DATA *store;
  ROOM_INDEX_DATA *room;
  char buf[MSL];

  room = ch->in_room;  

  EDIT_STORE(ch, store);

  if (!IS_HIIMMORTAL(ch))
  {
    	send_to_char("You do not have access to this command.\n\r",ch);
    	return;
  }
  if (str_cmp(argument, "yes"))
  {
    send_to_char("You must add yes to this and realize this will remove your store and will.\n\r", ch);
    send_to_char("not give you back any items or qps from the coffer, you need to do this previus.\n\r", ch);
    send_to_char("this command.\n\r", ch);
    return;
  }

  send_to_char("Exiting store editor.", ch);
  ch->desc->editor = ED_NONE;

  for (item = store->selling; item != NULL; item = item_next)
  {
    item_next = item->next;
    free_slist(item);
  }
  store->selling = NULL;

  for (sold = store->sold; sold != NULL; sold = sold_next)
  {
    sold_next = sold->next;
    free_soldlist(sold);
  }
  store->sold = NULL;

  if (store == store_list)
  {
    store_list = store->next;
  }
  else
  {
    STORE_DATA *storelist;

    for (storelist = store_list; storelist ; storelist = storelist->next)
    {
      if (storelist->next == store)
      {
        storelist->next = store->next;
        break;
       }

      if (storelist == NULL)
      {
        bug("Store not found.\n\r", 0);
        return;
      }
    }
  }

  free_store(store);  
  room->store = NULL;

  SET_BIT(room->room_flags, ROOM_EMPTY_STORE);
  free_string(room->name);
  sprintf(buf, "`*An Empty Store");
  room->name = str_dup(buf);
  free_string(room->description);
  sprintf(buf, "An empty store store is waiting to be bought.");   
  room->description = str_dup(buf);

  {
    CHAR_DATA *keeper;
    if ((keeper = get_char_room (ch, NULL, store->shopkeeper.keeper)) == NULL)
    {
      // do nothing
    }
    else
    {
      if (IS_NPC(keeper))
        extract_char(keeper, FALSE);
    }
  }


  SET_BIT(room->area->area_flags, AREA_CHANGED);   
  send_to_char("Store removed.\n\r", ch);
}
