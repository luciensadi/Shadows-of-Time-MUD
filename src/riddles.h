/* Header for stores.c */

#if defined(TRADITIONAL)
#define DECLARE_RIDDLE_FUN(fun)      void fun( )
#else
#define DECLARE_RIDDLE_FUN(fun)      RIDDLE_FUN fun
#endif

typedef void RIDDLE_FUN   args ( ( CHAR_DATA *ch, char *argument) );

void riddle_edit      args ((CHAR_DATA *ch, char *argument));
void show_riddle      args (( CHAR_DATA *ch ));
void add_riddle        args (( CHAR_DATA *ch, char *argument ));
void riddle_interpret  args (( CHAR_DATA *ch, char *argument ));
void save_riddle       args ((void));

#define EDIT_RIDDLE(Ch, Riddle)    ( Riddle = (RIDDLE_DATA *)Ch->desc->pEdit)

extern const char *lcmd_table[]; 
extern const struct riddle_cmd riddle_cmd_table[];

struct riddle_cmd
{
  char *name;
  RIDDLE_FUN * riddle_fun;
};

/* Store Functions */
DECLARE_RIDDLE_FUN( add_riddle );
DECLARE_RIDDLE_FUN( riddle_help );
DECLARE_RIDDLE_FUN( riddle_question );
DECLARE_RIDDLE_FUN( riddle_answer );
DECLARE_RIDDLE_FUN( riddle_difficulty );
DECLARE_RIDDLE_FUN( riddle_save );

