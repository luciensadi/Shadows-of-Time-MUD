/* Header for Web.c */

#define MUDSTAT_FILE "/home/aod/FoTN/public_html/data/mudstat.html"
#define PMOB_FILE "/home/aod/FoTN/public_html/data/mobs.html"
#define POBJ_FILE "/home/aod/FoTN/public_html/data/objs.html"
void help_letter      args (( char letter, FILE *fp ));
void print_one_help   args (( HELP_DATA *help, char *name));
void helps_to_html    args (( void ));
char colour_tag       args (( char type, FILE *fp, char last_type));  


int calculate_allowed args ((OBJ_INDEX_DATA *obj)); 
int calculate_points args ((OBJ_INDEX_DATA *obj)); 
int calculate_affect_points args ((OBJ_INDEX_DATA *obj));
int calculate_flag_points args (( const struct flag_type *flag_table, int bits ));

void print_head   args ((FILE *fp, char *title));
void print_tail   args ((FILE *fp));

void export_note  args ((NOTE_DATA *note));
void export_note  args ((NOTE_DATA *note, FILE *fp));

/** Stat Flags **/

#define AC       1
#define HP       2
#define MANA     3
#define MOVE     4
#define HITROLL  5
#define DAMROLL  6
#define SAVES    7
#define RESIS    8
#define STAT     9
#define SARMOR   10
#define SSHIELD  11
#define SHASTE   12
#define SSTONE   13

#define EXPORT_NOTE_FOLDER "/home/mud/dsmud/public_html/data/notes/"

extern const struct point_type point_table[];

struct point_type
{
  int bit;
  int point;
  int cost;
};
