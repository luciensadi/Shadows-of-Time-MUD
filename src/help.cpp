#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "mysql.h"

void do_ohelp(CHAR_DATA *ch, char *argument)
{
  char query_string[MSL];
  MYSQL_RES *res;
  MYSQL_ROW row;
  char buf[MSL]; 
  char arg1[MIL];
  BUFFER *buffer;  
  bool gameHelp = FALSE;

  argument = one_argument(argument, arg1);
  if (!str_cmp(arg1, "*"))
  {
    gameHelp = TRUE;
    argument = one_argument(argument, arg1);
  }  
 
  if (!str_prefix(arg1, "category"))
  {
    if (!str_cmp(argument, "list"))
    {
       sprintf(query_string, "SELECT name FROM help_categories ORDER BY name");
       mysql_query(mysql, query_string);
       res = mysql_store_result(mysql);
       send_to_char("Help categories\n\r", ch);
       while ((row = mysql_fetch_row(res)))
       {
         sprintf(buf, "%s\n\r", row[0]);
         send_to_char(buf, ch);
       }
       mysql_free_result(res);
       return;
    }
    
     sprintf(query_string, "SELECT h.name FROM helps h, help_categories c WHERE (h.catID = c.catID && c.name = '%s' and h.level <= %d)", argument, ch->level);
     mysql_query(mysql, query_string);

     if (!(res = mysql_store_result(mysql)) || !mysql_num_rows(res))
     {
       send_to_char("No such helps found.\n\r", ch);
       return;
     }
     
     send_to_char("Helps in that category: \n\r", ch);
     int i = 0;
     while ((row = mysql_fetch_row(res)))
     {
       i++;
       sprintf(buf, "%-20s", row[0]);
       send_to_char(buf, ch);
       if (i % 4 == 0)
         send_to_char("\n\r", ch);
     }
     mysql_free_result(res);
     return;
  }    

  sprintf(query_string, "SELECT h.name, h.body, h.related, c.name FROM helps h, help_categories c "
                        "WHERE (h.catID = c.catID and h.level <= %d and (h.keyword LIKE '%%%s%%' or h.name = '%s'))", ch->level,  arg1, arg1);
  mysql_query(mysql, query_string);

  if (!(res = mysql_store_result(mysql)) || !mysql_num_rows(res))
  {
    send_to_char("No such helps found.\n\r", ch);
    return;
  }


  buffer = new_buf();

  while ((row = mysql_fetch_row(res)))
  {

	sprintf(buf, "\n\r`&Helpfile Name:`7 %s `&\n\r", IS_NULLSTR(row[0]) ? "None" : (row[0]));
	add_buf(buffer, buf);

    sprintf(buf, "`7--------------------------------------------------------------------------------\n\r\n\r");
    add_buf(buffer, buf);

	sprintf(buf, "%s\n\r", row[1]);
	add_buf(buffer, buf);

	sprintf(buf, "`7--------------------------------------------------------------------------------\n\r");
    add_buf(buffer, buf);

	sprintf(buf, "`&CATEGORY:`7   %-15s   `&Related Helps:`7  %-40s\n\r",row[3],IS_NULLSTR(row[2]) ? "None" : row[2]);
    add_buf(buffer, buf);
	page_to_char(buf_string(buffer), ch);
	free_buf(buffer);
	mysql_free_result(res);

	return;
  }

  /*
  while ((row = mysql_fetch_row(res)))
  {
    if (found)    
      add_buf(buffer, "\n\r`1************************************************************************`*\n\r");
    if (!gameHelp)
    {
      sprintf(buf, "             `$Name: %s $Category: `&%-15s`*\n\r", IS_NULLSTR(row[0]) ? "None" : row[0], row[3]);
      add_buf(buffer, buf);
      add_buf(buffer, "`1      ----------------------------------------------------------`*\n\r");
    }
    sprintf(buf, "%s\n\r", row[1]);
    add_buf(buffer, buf);

    if (!gameHelp)
    {
      add_buf(buffer, "`1      ----------------------------------------------------------`*\n\r");
      sprintf(buf, "         `$Related helps: `&%-40s\n\r", IS_NULLSTR(row[2]) ? "None" : row[2]);
      add_buf(buffer, buf);
    }
    found = TRUE;

  }*/

  page_to_char(buf_string(buffer), ch);
  free_buf(buffer);
  mysql_free_result(res);
}


void do_mhelp(CHAR_DATA *ch, char *argument)
{
  char query_string[MSL];
  MYSQL_RES *res;
  MYSQL_ROW row;
  char buf[MSL]; 
  char arg1[MIL];
  BUFFER *buffer;  
  bool gameHelp = FALSE;


  argument = one_argument(argument, arg1);
  if (!str_cmp(arg1, "*"))
  {
    gameHelp = TRUE;
    argument = one_argument(argument, arg1);
  }  
 
  if (!str_prefix(arg1, "category"))
  {
    if (!str_cmp(argument, "list"))
    {
       sprintf(query_string, "SELECT name FROM help_categories ORDER BY name");
       mysql_query(mysql, query_string);
       res = mysql_store_result(mysql);
       send_to_char("Help categories\n\r", ch);
       while ((row = mysql_fetch_row(res)))
       {
         sprintf(buf, "%s\n\r", row[0]);
         send_to_char(buf, ch);
       }
       mysql_free_result(res);
       return;
    }
    
     sprintf(query_string, "SELECT h.name FROM helps h, help_categories c WHERE (h.catID = c.catID && c.name = '%s' and h.level <= %d)", argument, ch->level);
     mysql_query(mysql, query_string);

     if (!(res = mysql_store_result(mysql)) || !mysql_num_rows(res))
     {
       send_to_char("No such helps found.\n\r", ch);
       return;
     }
     
     send_to_char("Helps in that category: \n\r", ch);
     int i = 0;
     while ((row = mysql_fetch_row(res)))
     {
       i++;
       sprintf(buf, "%-20s", row[0]);
       send_to_char(buf, ch);
       if (i % 4 == 0)
         send_to_char("\n\r", ch);
     }
     mysql_free_result(res);
     return;
  }    

  sprintf(query_string, "SELECT h.name, h.body, h.related, c.name FROM helps h, help_categories c "
                        "WHERE (h.catID = c.catID and h.level <= %d and (h.keyword LIKE '%%%s%%' or h.name = '%s'))", ch->level,  arg1, arg1);
  mysql_query(mysql, query_string);

  if (!(res = mysql_store_result(mysql)) || !mysql_num_rows(res))
  {
    send_to_char("No such helps found.\n\r", ch);
    return;
  }


  buffer = new_buf();
  bool found = FALSE;
  while ((row = mysql_fetch_row(res)))
  {
    if (found)    
      add_buf(buffer, "\n\r\n\r`$================================================================================`*\n\r\n\r");
    if (!gameHelp)
    {
      sprintf(buf, "             `$Name`4: `&%s `$Category`4: `&%-15s`*\n\r", IS_NULLSTR(row[0]) ? "None" : row[0], row[3]);
      add_buf(buffer, buf);
      add_buf(buffer, "`4      ----------------------------------------------------------`*\n\r");
    }
    sprintf(buf, "%s\n\r", row[1]);
    add_buf(buffer, buf);
    if (!gameHelp)
    {
      add_buf(buffer, "`4      ----------------------------------------------------------`*\n\r");
      sprintf(buf, "         `$Related helps`4: `&%-40s`*\n\r", IS_NULLSTR(row[2]) ? "None" : row[2]);
      add_buf(buffer, buf);
    }
    found = TRUE;
  }

  page_to_char(buf_string(buffer), ch);
  free_buf(buffer);
  mysql_free_result(res);
}