
#if defined (TRADITIONAL)
#define DECLARE_MARK_FUN(fun)     void fun()
#else
#define DECLARE_MARK_FUN(fun)     MARK_FUN fun
#endif

typedef void MARK_FUN   args ((CHAR_DATA *ch, char *argument));

#define EDIT_MARK(Ch, Mark)      (Mark = (MARK_DATA *)Ch->desc->pEdit)

extern const struct mark_cmd mark_cmd_table[];

struct mark_cmd
{
    char *name;
    MARK_FUN * mark_fun;
};

extern const struct mark_tbl_type mark_type_table[];

struct mark_tbl_type
{
    char * name;
    int type;
};

extern const struct mark_status mark_status_table[];

struct mark_status
{
    char * name;
    int status;
};


void fwrite_mark      args   ((MARK_DATA *mark,FILE *fp));
void fread_mark       args   ((MARK_DATA *mark, FILE *fp));
bool IS_FURIES        args   ((CHAR_DATA *ch));
bool IS_CONTRACTOR    args   ((CHAR_DATA *ch, MARK_DATA * mark));
bool check_player     args   ((char * name));
char *str_mark_type   args   ((int type));
char *str_mark_status args   ((int status));
void mark_show        args   ((CHAR_DATA *ch, MARK_DATA * mark));
void mark_interpret   args   ((CHAR_DATA *ch,char * argument));
void mark_do_list     args   ((CHAR_DATA *ch));

DECLARE_MARK_FUN  ( mark_show_edit  );
//DECLARE_MARK_FUN  ( mark_set_type   );
DECLARE_MARK_FUN  ( mark_set_target );
DECLARE_MARK_FUN  ( mark_set_price  );
DECLARE_MARK_FUN  ( mark_set_extra  );
DECLARE_MARK_FUN  ( mark_set_status );
DECLARE_MARK_FUN  ( mark_set_descr  );
DECLARE_MARK_FUN  ( mark_accept     );
DECLARE_MARK_FUN  ( mark_help       );

void mark_edit        args   ((CHAR_DATA *ch,char * argument));

#define MARK_STAT_WRITE   -1
#define MARK_STAT_NEGOT    0
#define MARK_STAT_ACCEPT   1
#define MARK_STAT_REJECT   2
#define MARK_STAT_COMPLET  3
#define MARK_STAT_PAID     4

#define MARK_TYPE_NONE            -1
#define MARK_TYPE_PLAYER_DEATH     0
#define MARK_TYPE_PLAYER_LOOTS     1
#define MARK_TYPE_SPEC_MOB_EQ      2
#define MARK_TYPE_MOB_EQ           3         
#define MARK_TYPE_GUILD            4

