/*** Bomb Functions and types. ***/
/*** for bomb.c by Asmodeus ***/

extern  const   struct  bomb_type       bomb_table[];

void bomb_entry args ((CHAR_DATA *ch));

#define MAX_INGREDIENT 5

struct bomb_type
{
    char *name;
    int bit[MAX_INGREDIENT];
    int level;
    int avedam;
    int stability;
    int trigger;
    int explosive;
    char *description;
    char *recipe;
};  


/* Bomb Funs */ 
DECLARE_BOMB_FUN(     bomb_null     );
DECLARE_BOMB_FUN(     bomb_contact     );
DECLARE_BOMB_FUN(     bomb_explosion     );
DECLARE_BOMB_FUN(     bomb_sleeper     );
DECLARE_BOMB_FUN(     bomb_sticky     );
DECLARE_BOMB_FUN(     bomb_poison     );
DECLARE_BOMB_FUN(     bomb_smoke      );
