/* Header for stores.c */

#if defined(TRADITIONAL)
#define DECLARE_STORE_FUN(fun)      void fun( )
#else
#define DECLARE_STORE_FUN(fun)      STORE_FUN fun
#endif

#define STORE_DAY_COST 5000

typedef void STORE_FUN   args ( ( CHAR_DATA *ch, char *argument) );

void store_edit      args ((CHAR_DATA *ch, char *argument));
void make_store      args (( CHAR_DATA *ch, bool guild ));
STORE_DATA *get_char_store    args (( CHAR_DATA *ch ));
void show_store      args (( CHAR_DATA *ch ));
void add_item        args (( CHAR_DATA *ch, char *argument ));
void update_item        args (( CHAR_DATA *ch, char *argument ));
void store_interpret  args (( CHAR_DATA *ch, char *argument ));
void remove_item      args (( STORE_DATA *store, STORE_LIST_DATA *purchase ));
void fwrite_store     args (( STORE_DATA *store, FILE *fp ));
void save_stores      args (( void ));
void load_stores      args (( void ));
void fread_item       args (( STORE_DATA *store, FILE *fp));
void fread_sold       args (( STORE_DATA *store, FILE *fp));
void store_clear      args (( CHAR_DATA *ch, char *argument, int count));
OBJ_DATA *fread_obj_store args (( FILE *fp ));
void store_reset      args (( void ));
bool have_store       args (( CHAR_DATA *ch));// Here we have the declaration of the function to check to see if char already has a store.
                                              // (Not really necessary for one time functions but threw it in for kicks) -Camm

#define EDIT_STORE(Ch, Store)    ( Store = (STORE_DATA *)Ch->desc->pEdit)

extern const char *lcmd_table[]; 
extern const struct store_cmd store_cmd_table[];

struct store_cmd
{
  char *name;
  STORE_FUN * store_fun;
};

/* Store Functions */
DECLARE_STORE_FUN( add_item );
DECLARE_STORE_FUN( update_item );
DECLARE_STORE_FUN( empty_coffer );
DECLARE_STORE_FUN( store_desc );
DECLARE_STORE_FUN( store_hire );
DECLARE_STORE_FUN( store_help );
DECLARE_STORE_FUN( store_fire );
DECLARE_STORE_FUN( store_sold );
DECLARE_STORE_FUN( store_remove );
DECLARE_STORE_FUN( store_owner);

/* Other Functions */
DECLARE_STORE_FUN( do_inventory);
