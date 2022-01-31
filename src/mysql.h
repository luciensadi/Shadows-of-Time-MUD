#include <mysql/mysql.h>

extern   MYSQL   *mysql;

void mysql_insert args((MYSQL my_conn, char *table, char *fmt, ...));
