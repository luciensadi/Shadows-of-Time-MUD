#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "web.h"
#include "lookup.h"
#include "mysql.h"

void mysql_insert(MYSQL *mysql, char *table, char *fmt, ...)
{
   va_list ap;
   bool format = false;
   bool start = true;
   long l;
   int d;
   char c, *s;
   const char *p = fmt;
   char buf[8092];
   char query_string[8092];

   sprintf(query_string, "INSERT INTO %s VALUES(", table);
   va_start(ap, fmt);

   while (*p)
   {
     if (format && !start)
       strcat(query_string, ",");

     switch(*p) {
       case '%':
          format = true;
          buf[0] = '\0';
          break;

       case 's':           /* string */
          if (!format) { sprintf(buf, "%c", *p); break;}
          format = false;start = false;
          s = va_arg(ap, char *);
          sprintf(buf, "\"%s\"", s);
          break;

       case 'd':           /* int */
          if (!format) { sprintf(buf, "%c", *p);break;}
          format = false;start=false;
          d = va_arg(ap, int);
          sprintf(buf, "%d", d);
          break;

       case 'l':           /* int */
          if (!format) {sprintf(buf, "%c", *p);break;}
          format = false;start=false;
          l = va_arg(ap, long);
          sprintf(buf, "%ld", l);
          break;

       case 'c':           /* char */
          if (!format) {
            sprintf(buf, "%c", *p);
            break;
          }
          format = false;start=false;
          c = va_arg(ap, int);
          sprintf(buf, "\"%c\"", c);
          break;

       default:
          if (format)
            format = false;
          buf[0] = '\0';
          break;

       }
      strcat(query_string, buf);
      ++p;
  }

  strcat(query_string, ");");
  va_end(ap);
  if (mysql_query(mysql, query_string))
    bugf("Couldn't insert", 0);
}

void update_room(char *name, int count)
{
  MYSQL_RES *res;
  MYSQL_ROW row;
  char query_string[MSL];

  sprintf(query_string, "SELECT id from top_explored where (name = '%s')", name);
  mysql_query(mysql, query_string);
  if (!(res = mysql_store_result(mysql)) || !mysql_num_rows(res))
  {
    mysql_query(mysql, "SELECT id FROM top_explored ORDER BY explored limit 1");
    res = mysql_store_result(mysql);
    row = mysql_fetch_row(res);
    int id = atoi(row[0]);
    sprintf(query_string, "DELETE FROM top_explored WHERE id = %d", id);
    mysql_query(mysql, query_string);
    sprintf(query_string, "INSERT INTO top_explored (name, explored) VALUES ('%s', %d)", name, count);
    mysql_query(mysql, query_string);
  }
  else
  {
    row = mysql_fetch_row(res);
    sprintf(query_string, "UPDATE top_explored SET explored = %d where ID = %s", count, row[0]);
    mysql_query(mysql, query_string);
  }
  mysql_free_result(res);
}

void update_mobbed(char *name, int count)
{
  MYSQL_RES *res;
  MYSQL_ROW row;
  char query_string[MSL];

  sprintf(query_string, "SELECT id from top_mobbed where (name = '%s')", name);
  mysql_query(mysql, query_string);
  if (!(res = mysql_store_result(mysql)) || !mysql_num_rows(res))
  {
    mysql_query(mysql, "SELECT id FROM top_mobbed ORDER BY mobbed limit 1");
    res = mysql_store_result(mysql);
    row = mysql_fetch_row(res);
    int id = atoi(row[0]);
    sprintf(query_string, "DELETE FROM top_mobbed WHERE id = %d", id);
    mysql_query(mysql, query_string);
    sprintf(query_string, "INSERT INTO top_mobbed (name, mobbed) VALUES ('%s', %d)", name, count);
    mysql_query(mysql, query_string);
  }
  else
  {
    row = mysql_fetch_row(res);
    sprintf(query_string, "UPDATE top_mobbed SET mobbed = %d where ID = %s", count, row[0]);
    mysql_query(mysql, query_string);
  }
  mysql_free_result(res);
}

