/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.    *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
 *    ROM 2.4 is copyright 1993-1998 Russ Taylor                           *
 *    ROM has been brought to you by the ROM consortium                    *
 *        Russ Taylor (rtaylor@hypercube.org)                              *
 *        Gabrielle Taylor (gtaylor@hypercube.org)                         *
 *        Brian Moore (zump@rom.org)                                       *
 *    By using this code, you have agreed to follow the terms of the       *
 *    ROM license, in the file Rom24/doc/rom.license                       *
 ***************************************************************************/


/*
 * Accommodate old non-Ansi compilers.
 */
#ifdef MCCP
   #include <zlib.h>
#endif

#include <mysql/mysql.h>

#if defined(TRADITIONAL)
#define DECLARE_OBJ_FUN( fun )          void fun( )
#define DECLARE_ROOM_FUN( fun )         void fun( )
#define const
#define args( list )            ( )
#define DECLARE_DO_FUN( fun )        void fun( )
#define DECLARE_SPEC_FUN( fun )        bool fun( )
#define DECLARE_SPELL_FUN( fun )    void fun( )
#define DECLARE_BOMB_FUN( fun )    void fun( )
#define DECLARE_OBJ_FUN( fun )          OBJ_FUN  fun( )
#define DECLARE_ROOM_FUN( fun )         ROOM_FUN  fun( )


#else
#define args( list )            list
#define DECLARE_OBJ_FUN( fun )          OBJ_FUN fun
#define DECLARE_ROOM_FUN( fun )         ROOM_FUN fun
#define DECLARE_DO_FUN( fun )        DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )        SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )    SPELL_FUN fun
#define DECLARE_BOMB_FUN( fun )    BOMB_FUN fun
#endif

#if     defined(NOCRYPT)
#define crypt(s1, s2)   (s1)
#else
extern "C" {char *  crypt           args( ( const char *key, const char *salt ) );};
#endif

/* system calls */
int unlink(const char *pathname);
int system();

#define stc(a,b) send_to_char(a,b)
#define obj_list object_list
long atoh(const char *);
char *CHAR2(long);
/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if    !defined(FALSE)
#define FALSE     0
#endif
#if    !defined(false)
#define false     0
#endif

#if    !defined(TRUE)
#define TRUE     1
#endif
#if    !defined(true)
#define true     1
#endif

#if    defined(_AIX)
#if    !defined(const)
#define const
#endif
typedef int                sh_int;
typedef int                bool;
#define unix
#else
typedef short   int            sh_int;
// typedef unsigned char            bool;
#endif

/* ea */
#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH

/* UBit */
typedef int              FLAG;
#define FLAG_BITSIZE     16
#define AFF_FLAGS         4
#define MAX_FLAGS         4

/*
 * Structure types.
 */
typedef struct    affect_data        AFFECT_DATA;
typedef struct    area_data        AREA_DATA;
typedef struct    ban_data        BAN_DATA;
typedef struct     buf_type         BUFFER;
typedef struct    fortune_type      FORTUNE_DATA;
typedef struct    quotester_type    QUOTESTER_DATA;
typedef struct    char_data        CHAR_DATA;
typedef struct    descriptor_data        DESCRIPTOR_DATA;
typedef struct    exit_data        EXIT_DATA;
typedef struct    extra_descr_data    EXTRA_DESCR_DATA;
typedef struct    help_data        HELP_DATA;
typedef struct    help_area_data        HELP_AREA;
typedef struct    kill_data        KILL_DATA;
typedef struct    mem_data        MEM_DATA;
typedef struct    mob_index_data        MOB_INDEX_DATA;
typedef struct    note_data        NOTE_DATA;
typedef struct    obj_data        OBJ_DATA;
typedef struct    obj_index_data        OBJ_INDEX_DATA;
typedef struct    pc_data            PC_DATA;
typedef struct    pk_type            PK_DATA;
typedef struct  gen_data        GEN_DATA;
typedef struct    reset_data        RESET_DATA;
typedef struct    room_index_data        ROOM_INDEX_DATA;
typedef struct    shop_data        SHOP_DATA;
typedef struct    time_info_data        TIME_INFO_DATA;
typedef struct    weather_data        WEATHER_DATA;
typedef struct  prog_list        PROG_LIST;
typedef struct  prog_code        PROG_CODE;
typedef struct  delay_prog_code        DELAY_CODE;
typedef struct        colour_data             COLOUR_DATA;
//typedef struct  auction_data            AUCTION_DATA;
typedef struct         guild_rank          rankdata;
typedef struct         gskill_name             gskilldata;
typedef struct         mudstats_type           MUDSTATS_DATA;
typedef struct         voters_type             VOTERS_DATA;
typedef struct         vote_type               VOTE_DATA;
typedef struct         riddle_type             RIDDLE_DATA;
typedef struct         account_type            ACCOUNT_DATA;
typedef struct         account_plr_type        ACCT_PLR_DATA;
typedef struct         member_type             MEMBER_DATA;
typedef struct         roster_type             ROSTER_DATA;
typedef struct         clan_type               CLAN_DATA;
typedef struct         qprize_type               QPRIZE_DATA;
//typedef struct         mark_type               MARK_DATA;

typedef struct         seed_data	       SEED_DATA;
typedef struct         plant_data              PLANT_DATA;
typedef struct         herb_data               HERB_DATA;
typedef struct         tournament_type         TOURNAMENT_DATA;
typedef struct         store_type              STORE_DATA;
typedef struct         store_list_type         STORE_LIST_DATA;
typedef struct         sold_list_type          SOLD_LIST_DATA;
typedef struct         said_to_type            SAID_TO_DATA;
typedef struct         quest_type              QUEST_DATA;
typedef struct         quest_info_type         QUEST_INFO_DATA;

struct stance_type
{
    char *      name;
    int         stance;
};

struct    said_to_type
{
  SAID_TO_DATA *next;
  long ch_id;
  long bit;
};

#define QUEST_INPROGRESS 0
#define QUEST_COMPLETED  1
#define QUEST_FAILED     2

struct    quest_type
{
  QUEST_DATA *next;
  sh_int number;
  sh_int status;
  long bit;
};

struct quest_info_type
{
  QUEST_INFO_DATA *next;
  sh_int number;
  sh_int topstep;
  char *name;
  char *step[32];
};


/*
 * Function types.
 */
typedef void DO_FUN    args( ( CHAR_DATA *ch, char *argument ) );
typedef bool SPEC_FUN    args( ( CHAR_DATA *ch ) );
typedef void SPELL_FUN    args( ( int sn, int level, CHAR_DATA *ch, void *vo,
                int target ) );
typedef void BOMB_FUN    args( ( OBJ_DATA *obj, CHAR_DATA *victim ) );
typedef void OBJ_FUN    args( ( OBJ_DATA *obj, char *argument ) );
typedef void ROOM_FUN   args( ( ROOM_INDEX_DATA *room, char *argument ) );


/*
 * String and memory management parameters.
 */
#define MAX_KEY_HASH         2048
#define MAX_STRING_LENGTH     9216
#define MAX_INPUT_LENGTH      256
#define PAGELEN               22

/* I am lazy :) */
#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH



/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_SOCIALS            260
#define MAX_SKILL              255
#define MAX_GROUP              47
#define MAX_ILLEGAL_NAMES      50
#define MAX_IN_GROUP           25
#define MAX_ALIAS              30
#define MAX_CLASS              12
#define MAX_PC_RACE            17
#define MAX_CLAN               20
#define MAX_PRIZE              40
#define MAX_PRIZE_COUPON       25
#define MAX_RP_PRIZE           10
#define MAX_RPLEVEL            100
#define MAX_LIST               18
#define MAX_GUILD_SKILL        10
#define MAX_FORSAKEN           16
#define MAX_RANK               10
#define MAX_COUPLE             5
#define MAX_COUPLE_SPELLS      9
#define MAX_DAMAGE_MESSAGE     50
#define NEW_DAM_MSG            (MAX_DAMAGE_MESSAGE -39)
#define MAX_MAR_DAM_MSG        18
#define MAX_MAR_THROW_MSG      6
#define MAX_LEVEL              81
#define MAX_EXPLORE           8192
#define MAX_NORMAL           (MAX_LEVEL - 6)
#define LEVEL_HERO           (MAX_LEVEL - 5)
#define LEVEL_GL             (MAX_LEVEL - 4)
#define LEVEL_IMMORTAL       (MAX_LEVEL - 3)
#define MAX_HEAL               11
#define MAX_SEED               3
#define MAX_PLANT              3
#define MAX_HERB               3


// Reduce damage and healing spells by a certain percent
#define DAMAGE_REDUCER 0.65;
#define DAMAGE_VEIL    1.35;


/*Loots*/
#define REGULAR_LOOT 0;
#define ENHANCED_LOOT 0;
#define OTHER_LOOT 0;


/* Added this for "orphaned help" code. Check do_help() -- JR */
#define MAX_CMD_LEN			50

#define PULSE_PER_SECOND        4
#define PULSE_MOBHUNT           (5 * PULSE_PER_SECOND)
#define PULSE_VIOLENCE          ( 3 * PULSE_PER_SECOND)
#define PULSE_MOBILE          ( 4 * PULSE_PER_SECOND)
#define PULSE_MUSIC          ( 6 * PULSE_PER_SECOND)
#define PULSE_TICK          (40 * PULSE_PER_SECOND)
#define PULSE_AREA          (100 * PULSE_PER_SECOND)
#define PULSE_SAVE          (600 * PULSE_PER_SECOND)
#define PULSE_SYSTEM        (5 * 60 * 60 * PULSE_PER_SECOND)
#define PULSE_DEATHTRAP   ( 15 * PULSE_PER_SECOND)
#define PULSE_QUESTLIST     ( 8 * 60 * 60 * PULSE_PER_SECOND)

#define IMPLEMENTOR      MAX_LEVEL
#define CREATOR          (MAX_LEVEL - 1)
#define SUPREME          (MAX_LEVEL - 2)
#define BUILDER          (MAX_LEVEL - 3)
#define LEGEND           LEVEL_HERO


/* Reduce damage and heals in pk */

/* Log Types by Asmo */
#define RP_TYPE        0
#define PK_TYPE        1
#define IMM_TYPE       2

/*
 * ColoUr stuff v2.0, by Lope.
 */
#define CLEAR        "\e[0;37m"        /* Resets Colour    */
#define C_RED        "\e[0;31m"    /* Normal Colours    */
#define C_GREEN        "\e[0;32m"
#define C_YELLOW    "\e[0;33m"
#define C_BLUE        "\e[0;34m"
#define C_MAGENTA    "\e[0;35m"
#define C_CYAN        "\e[0;36m"
#define C_WHITE        "\e[0;37m"
#define C_D_GREY    "\e[1;30m"      /* Light Colors        */
#define C_B_RED        "\e[1;31m"
#define C_B_GREEN    "\e[1;32m"
#define C_B_YELLOW    "\e[1;33m"
#define C_B_BLUE    "\e[1;34m"
#define C_B_MAGENTA    "\e[1;35m"
#define C_B_CYAN    "\e[1;36m"
#define C_B_WHITE    "\e[1;37m"


#define COLOUR_NONE    7        /* White, hmm...    */
#define RED        1        /* Normal Colours    */
#define GREEN        2
#define YELLOW        3
#define BLUE        4
#define MAGENTA        5
#define CYAN        6
#define WHITE        7
#define BLACK        0
void    pull_slot  args( ( CHAR_DATA *ch ) );

#define NORMAL        0        /* Bright/Normal colours */
#define BRIGHT        1

#define ALTER_COLOUR( type )    if( !str_prefix( argument, "red" ) )        \
                {                        \
                    ch->pcdata->type[0] = NORMAL;        \
                    ch->pcdata->type[1] = RED;            \
                }                        \
                else if( !str_prefix( argument, "hi-red" ) )    \
                {                        \
                    ch->pcdata->type[0] = BRIGHT;        \
                    ch->pcdata->type[1] = RED;            \
                }                        \
                else if( !str_prefix( argument, "green" ) )    \
                {                        \
                    ch->pcdata->type[0] = NORMAL;        \
                    ch->pcdata->type[1] = GREEN;        \
                }                        \
                else if( !str_prefix( argument, "hi-green" ) )    \
                {                        \
                    ch->pcdata->type[0] = BRIGHT;        \
                    ch->pcdata->type[1] = GREEN;        \
                }                        \
                else if( !str_prefix( argument, "yellow" ) )    \
                {                        \
                    ch->pcdata->type[0] = NORMAL;        \
                    ch->pcdata->type[1] = YELLOW;        \
                }                        \
                else if( !str_prefix( argument, "hi-yellow" ) )    \
                {                        \
                    ch->pcdata->type[0] = BRIGHT;        \
                    ch->pcdata->type[1] = YELLOW;        \
                }                        \
                else if( !str_prefix( argument, "blue" ) )    \
                {                        \
                    ch->pcdata->type[0] = NORMAL;        \
                    ch->pcdata->type[1] = BLUE;        \
                }                        \
                else if( !str_prefix( argument, "hi-blue" ) )    \
                {                        \
                    ch->pcdata->type[0] = BRIGHT;        \
                    ch->pcdata->type[1] = BLUE;        \
                }                        \
                else if( !str_prefix( argument, "magenta" ) )    \
                {                        \
                    ch->pcdata->type[0] = NORMAL;        \
                    ch->pcdata->type[1] = MAGENTA;        \
                }                        \
                else if( !str_prefix( argument, "hi-magenta" ) ) \
                {                        \
                    ch->pcdata->type[0] = BRIGHT;        \
                    ch->pcdata->type[1] = MAGENTA;        \
                }                        \
                else if( !str_prefix( argument, "cyan" ) )    \
                {                        \
                    ch->pcdata->type[0] = NORMAL;        \
                    ch->pcdata->type[1] = CYAN;            \
                }                        \
                else if( !str_prefix( argument, "hi-cyan" ) )    \
                {                        \
                    ch->pcdata->type[0] = BRIGHT;        \
                    ch->pcdata->type[1] = CYAN;            \
                }                        \
                else if( !str_prefix( argument, "white" ) )    \
                {                        \
                    ch->pcdata->type[0] = NORMAL;        \
                    ch->pcdata->type[1] = WHITE;        \
                }                        \
                else if( !str_prefix( argument, "hi-white" ) )    \
                {                        \
                    ch->pcdata->type[0] = BRIGHT;        \
                    ch->pcdata->type[1] = WHITE;        \
                }                        \
                else if( !str_prefix( argument, "grey" ) )    \
                {                        \
                    ch->pcdata->type[0] = BRIGHT;        \
                    ch->pcdata->type[1] = BLACK;        \
                }                        \
                else if( !str_prefix( argument, "beep" ) )    \
                {                        \
                    ch->pcdata->type[2] = 1;            \
                }                        \
                else if( !str_prefix( argument, "nobeep" ) )    \
                {                        \
                    ch->pcdata->type[2] = 0;            \
                }                        \
                else                        \
                {                        \
        send_to_char_bw( "Unrecognised colour, unchanged.\n\r", ch );    \
                    return;                    \
                }

#define LOAD_COLOUR( field )    fread_number( fp );

#define WEAVE_FIRE   0
#define WEAVE_EARTH  1
#define WEAVE_AIR    2
#define WEAVE_WATER  3
#define WEAVE_SPIRIT 4
#define MAX_WEAVES   5


/*
 * Thanks Dingo for making life a bit easier ;)
 */
#define CH(d)                   ((d)->original ? (d)->original : (d)->character)

/*
 * Site ban structure.
 */

#define BAN_SUFFIX        A
#define BAN_PREFIX        B
#define BAN_NEWBIES        C
#define BAN_ALL            D
#define BAN_PERMIT        E
#define BAN_PERMANENT        F

struct    ban_data
{
    BAN_DATA *    next;
    bool    valid;
    sh_int    ban_flags;
    sh_int    level;
    char *    name;
};

struct buf_type
{
    BUFFER *    next;
    bool        valid;
    sh_int      state;  /* error state of the buffer */
    sh_int      size;   /* size in k */
    char *      string; /* buffer's string */
};


/*
 * Time and weather stuff.
 */
#define SUN_DARK            0
#define SUN_RISE            1
#define SUN_LIGHT            2
#define SUN_SET                3

#define SKY_CLOUDLESS            0
#define SKY_CLOUDY            1
#define SKY_RAINING            2
#define SKY_LIGHTNING            3

struct    time_info_data
{
    int        hour;
    int        minute;
    int        day;
    int        month;
    int        year;
    bool       first;
};

struct    weather_data
{
    int        mmhg;
    int        change;
    int        sky;
    int        sunlight;
};



/*
 * Connected state for a channel.
 */

#define CON_CHECK_ATTACH_PASSWORD            -27
#define CON_CONFIRM_NEW_ACCOUNT_PASSWORD     -26
#define CON_GET_NEW_ACCOUNT_PASSWORD         -25
#define CON_ACCOUNT_ACTION                   -24
#define CON_SHOW_ACCOUNT            -23
#define CON_GETDNS                  -22
#define CON_GET_ACCOUNT_PASSWORD    -21
#define CON_CONFIRM_NEW_ACCOUNT     -20
#define CON_GET_ACCOUNT_NAME        -19
#define CON_GET_POWER		    -18
#define CON_GET_NEW_NAME            -17
#define CON_GET_DRAGON_PASSWORD     -16
#define CON_GET_FORSAKEN_PASSWORD   -15
#define CON_GET_NAME                -14
#define CON_GET_OLD_PASSWORD        -13
#define CON_CONFIRM_NEW_NAME        -12
#define CON_GET_NEW_PASSWORD        -11
#define CON_CONFIRM_NEW_PASSWORD    -10
#define CON_ANSI                    -9
#define CON_GET_TELNETGA	    -8
#define CON_GET_NEW_RACE            -7
#define CON_GET_NEW_SEX             -6
#define CON_GET_NEW_CLASS           -5
#define CON_GET_ALIGNMENT           -4
#define CON_DEFAULT_CHOICE          -3
#define CON_GEN_GROUPS              -2
#define CON_PICK_WEAPON             -1
#define CON_PLAYING                  0
#define CON_READ_IMOTD               1
#define CON_READ_MOTD                2
#define CON_BREAK_CONNECT            3
#define CON_COPYOVER_RECOVER         4
#define CON_KINKBOOT_RECOVER         5
#define OPTION_UPDATE1               6
struct quotester_type
{
  char *text[500];
  int top;
};
struct fortune_type
{
  char *text[500];
  int top;
};

/*
 * Descriptor (channel) structure.
 */
struct    descriptor_data
{
    sh_int      descriptor;
    DESCRIPTOR_DATA *    next;
    DESCRIPTOR_DATA *    snoop_by;
    CHAR_DATA *          character;
    CHAR_DATA *          original;
    ACCOUNT_DATA *       account;
    bool        valid;
	bool        ansi;
    #ifdef THREADED_DNS
      char      Host[4];
    #elif defined (DNS_SLAVE)
      int                 wait;   /* wait for how many loops */
      long                site_info;
      char                host[60];
      u_long              addr;
      time_t              contime;
      char * 		  Host;
    #else
      char *      host;
      char *      Host;
    #endif

    sh_int      connected;
    bool        fcommand;
    char        inbuf        [4 * MAX_INPUT_LENGTH];
    char        incomm        [MAX_INPUT_LENGTH];
    char        inlast        [MAX_INPUT_LENGTH];
    int         repeat;
    char *      outbuf;
    int         outsize;
    int         outtop;
    char *      showstr_head;
    char *      showstr_point;
    void *      pEdit;        /* OLC */
    char **     pString;    /* OLC */
    int         editor;        /* OLC */
    bool        mxp; /* Mud HTML */
#ifdef MCCP
    unsigned char	compressing;
    z_stream *          out_compress;
    unsigned char *     out_compress_buf;
#endif
};


struct account_type
{
  sh_int version;
  char   *host;
  char   *name;
  char   *password;
  ACCT_PLR_DATA *player;
  ACCOUNT_DATA *next;
};

struct account_plr_type
{
  char *name;
  sh_int password;
  sh_int primary;
  ACCT_PLR_DATA *next;
};


/*
 * Attribute bonus structures.
 */
struct    str_app_type
{
    sh_int    tohit;
    sh_int    todam;
    sh_int    carry;
    sh_int    wield;
};

struct    int_app_type
{
    sh_int    learn;
};

struct    wis_app_type
{
    sh_int    practice;
};

struct    dex_app_type
{
    sh_int    defensive;
    sh_int    offensive;
};

struct    con_app_type
{
    sh_int    hitp;
    sh_int    shock;
};

/* For Murder/PK tracking */

struct pk_type
{
  sh_int type;
  long time;
  long id;
  PK_DATA *next;
};

#define PK_LOWBIE  0 // Lowbie iniated
#define PK_NORMAL    1 // Kill


/*
 * TO types for act.
 */
#define TO_ROOM            0
#define TO_NOTVICT        1
#define TO_VICT            2
#define TO_CHAR            3
#define TO_ALL            4

// Guide Flags

#define GUIDE_MURDR 0
#define GUIDE_THIEF 1

/*
 * Help table types.
 */
struct    help_data
{
    HELP_DATA *    next;
    HELP_DATA * next_area;
    sh_int    level;
    char *    keyword;
    char *    text;
};

struct help_area_data
{
    HELP_AREA *    next;
    HELP_DATA *    first;
    HELP_DATA *    last;
    AREA_DATA *    area;
    char *        filename;
    bool        changed;
};

struct tournament_type
{
  sh_int  status;
  sh_int  players;
  sh_int  tickstill;
  sh_int  maxrank;
  bool    noguild;
};

struct mudstats_type
{
  /* Current Day Stats */
  int day;
  int current_players;
  int current_max_players;
  int current_logins;
  int current_newbies;
  int current_heros;
  /* Total Stats */
  int total_max_players;
  long total_logins;
  long total_newbies;
  int total_heros;
  /* Non Day Specific */
  int player_shops;
};


struct voters_type
{
  VOTERS_DATA * next;
  bool valid;
  int id;
  int vote;
  char *host;
  char *reason;
};

struct vote_type
{
   bool on;
   char *question;
   int forit;
   int against;
   VOTERS_DATA * voters;
};

struct riddle_type
{
  RIDDLE_DATA *next;
  bool valid;
  int vnum;
  char *question;
  char *answer;
  int difficulty;
};

/*
 * Shop types.
 */
#define MAX_TRADE     5

struct    shop_data
{
    SHOP_DATA *    next;            /* Next shop in list        */
    long    keeper;            /* Vnum of shop keeper mob    */
    sh_int    buy_type [MAX_TRADE];    /* Item types shop will buy    */
    sh_int    profit_buy;        /* Cost multiplier for buying    */
    sh_int    profit_sell;        /* Cost multiplier for selling    */
    sh_int    open_hour;        /* First opening hour        */
    sh_int    close_hour;        /* First closing hour        */
};



/*
 * Per-class stuff.
 */

#define MAX_GUILD     2
#define MAX_STATS     5
#define STAT_STR     0
#define STAT_INT    1
#define STAT_WIS    2
#define STAT_DEX    3
#define STAT_CON    4

#define FMANA_FIGHTER 0
#define FMANA_CHANNEL 1
#define FMANA_BOTH    3

struct    class_type
{
    char *    name;            /* the full name of the class */
    char     who_name    [11];    /* Three-letter name for 'who'    */
    sh_int   attr_prime   ;
    sh_int    weapon;            /* First weapon            */
    long    guild[MAX_GUILD];    /* Vnum of guild rooms        */
    sh_int    skill_adept;        /* Maximum skill level        */
    sh_int    thac0_00;        /* Thac0 for level  0        */
    sh_int    thac0_32;        /* Thac0 for level 32        */
    sh_int    hp_min;            /* Min hp gained on leveling    */
    sh_int    hp_max;            /* Max hp gained on leveling    */
    sh_int    mana_min;
    sh_int    mana_max;
    sh_int    fMana;            /* Class gains mana on level    */
    char *    base_group;        /* base skills gained        */
    char *    default_group;        /* default skills gained    */
    sh_int    modstat[MAX_STATS];
    sh_int    weaves[MAX_WEAVES];
};

struct item_type
{
    int        type;
    char *    name;
};

struct explosive_type
{
    int        type;
    char *    name;
    BOMB_FUN * bomb_fun;
};

struct weapon_type
{
    char *    name;
    long    vnum;
    sh_int    type;
    sh_int    *gsn;
};

struct wiznet_type
{
    char *    name;
    long     flag;
    int        level;
};

struct member_type
{
   MEMBER_DATA * next;
   char * name;
   char * action;
   int rank;
   int qp;
   int rp;
   bool valid;
   long lastLog;
};

struct idName
{
  char *name;
  long id;
  struct idName *next;
};

struct roster_type
{
  MEMBER_DATA *member;
  int questpoints;
  int withdraw;
  bool approve;
  char *requestor;
};

struct qprize_type
{
  char *keyword;
  char *showname;
  int level;
  long vnum;
  int cost;
  bool special;
};

struct clan_type
{
  char *name;
  char *who_name;
  char *display;
  int vnum;
  int top_rank;
  char *rank[MAX_RANK];
  int top_gskill;
  char *gskill[MAX_GUILD_SKILL];
  ROSTER_DATA roster;
};

/*struct mark_type
{
    MARK_DATA * next;
    char *     	contractor;
    sh_int      type;
    char *	target;
    char *      desc;
    sh_int      status;
    int         price;
    int         extra;
    bool        valid;
};
*/

struct attack_type
{
    char *    name;            /* name */
    char *    noun;            /* message */
    int       damage;            /* damage class */
};

struct mar_attack_type
{

    char *    noun;            /* message */
    int       damage;            /* damage class */
    int       mindam;
    int       maxdam;
};
struct throw_attack_type
{
    char *    noun;            /* message */
    int       damage;            /* damage class */
};

struct race_type
{
    char *    name;            /* call name of the race */
    bool    pc_race;        /* can be chosen by pcs */
    long    act;            /* act bits for the race */
    FLAG    aff[AFF_FLAGS];            /* aff bits for the race */
    long    off;            /* off bits for the race */
    long    imm;            /* imm bits for the race */
    long        res;            /* res bits for the race */
    long    vuln;            /* vuln bits for the race */
    long    form;            /* default form flag for the race */
    long    parts;            /* default parts for the race */

};

struct pc_race_type  /* additional data for pc races */
{
    char *    name;            /* MUST be in race_type */
    char     who_name[6];
    sh_int    points;            /* cost in points of the race */
    sh_int    class_mult[MAX_CLASS];    /* exp multiplier for class, * 100 */
    char *    skills[5];        /* bonus skills for the race */
    sh_int     stats[MAX_STATS];    /* starting stats */
    sh_int    max_stats[MAX_STATS];    /* maximum stats */
    sh_int    size;            /* aff bits for the race */
    sh_int    modstats[MAX_STATS];
};
struct seed_type  /*Diem Herbalist Code*/
{
    long    vnum;
    char *    name;
    sh_int    level;
    sh_int    growsplant;
};
struct plant_type  /*Diem Herbalist Code*/
{
    char *    name;
    long    vnum;
    sh_int    level;
    sh_int    growmin;
    sh_int    growmax;
    sh_int    herbproduced;
};
struct herb_type  /*Diem Herbalist Code*/
{
    char *    name;
    long    vnum;
    sh_int    level;
    sh_int    exp;
};


struct spec_type
{
    char *     name;            /* special function name */
    SPEC_FUN *    function;        /* the function */
};



/*
 * Data structure for notes.
 */

#define NOTE_NOTE       0
#define NOTE_IDEA       1
#define NOTE_GUILDMASTER    2
#define NOTE_NEWS       3
#define NOTE_CHANGES    4
#define NOTE_IMM        5
#define NOTE_RP         6
#define NOTE_GUIDE      7
#define NOTE_STORE      8
#define NOTE_MAX        9 // Make sure this is always last
struct    note_data
{
    NOTE_DATA *    next;
    bool     valid;
    sh_int    type;
    char *    sender;
    char *    date;
    char *    to_list;
    char *    subject;
    char *    text;
    time_t      date_stamp;
};



/*
 * An affect.
 */
struct    affect_data
{
    AFFECT_DATA *    next;
    bool             valid;
    sh_int           where;
    sh_int        type;
    sh_int        level;
    sh_int        duration;
    sh_int        location;
    sh_int        modifier;
    int            bitvector;
    CHAR_DATA      *caster;
    affect_data (void) { caster = NULL; };
};

/* where definitions */
#define TO_AFFECTS   0
#define TO_OBJECT    1
#define TO_IMMUNE    2
#define TO_RESIST    3
#define TO_VULN      4
#define TO_WEAPON    5
#define TO_AFFECTS2  6
#define TO_SIM       7

/*
 * A kill structure (indexed by level).
 */
struct    kill_data
{
    sh_int        number;
    sh_int        killed;
};

struct guild_rank
{
  char *rankname;
};

struct rank_type
{
   rankdata rank_name[MAX_RANK];
};

struct gskill_name
{
  char *sname;
};

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_FIDO           3090
#define MOB_VNUM_CITYGUARD      3060
#define MOB_VNUM_VAMPIRE        3404
#define MOB_VNUM_GWOLF           4
#define MOB_VNUM_DAMANE         6
#define MOB_VNUM_DARKHOUND      8

#define MOB_VNUM_PATROLMAN       2106
#define GROUP_VNUM_TROLLS       2100
#define GROUP_VNUM_OGRES       2101

#define BURNT_FIRE             27
#define STANDARD_FIRE          29
#define DUMMY_BOMB             33
#define TINKER_SAFE            7581
#define TOKEN_VNUM             35

/* RT ASCII conversions -- used so we can have letters in this file */

#define A              1
#define B            2
#define C            4
#define D            8
#define E            16
#define F            32
#define G            64
#define H            128

#define I            256
#define J            512
#define K                1024
#define L             2048
#define M            4096
#define N             8192
#define O            16384
#define P            32768

#define Q            65536
#define R            131072
#define S            262144
#define T            524288
#define U            1048576
#define V            2097152
#define W            4194304
#define X            8388608

#define Y            16777216
#define Z            33554432
#define aa            67108864     /* doubled due to conflicts */
#define bb            134217728
#define cc            268435456
#define dd            536870912
#define ee            1073741824

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC        (A)        /* Auto set for mobs    */
#define ACT_SENTINEL            (B)        /* Stays in one room    */
#define ACT_SCAVENGER              (C)        /* Picks up objects    */
#define ACT_MOUNT             (D)
#define ACT_AGGRESSIVE        (F)            /* Attacks PC's        */
#define ACT_STAY_AREA        (G)        /* Won't leave area    */
#define ACT_WIMPY        (H)
#define ACT_PET              (I)        /* Auto set for pets    */
#define ACT_TRAIN            (J)        /* Can train PC's    */
#define ACT_PRACTICE         (K)        /* Can practice PC's    */
#define ACT_TINY             (L)
#define ACT_GUILDMOB         (M)       /* Damane Wofl etc */
#define ACT_UNDEAD           (O)
#define ACT_CLERIC           (Q)
#define ACT_MAGE             (R)
#define ACT_THIEF            (S)
#define ACT_WARRIOR          (T)
#define ACT_NOALIGN          (U)
#define ACT_NOPURGE          (V)
#define ACT_OUTDOORS         (W)
#define ACT_INDOORS          (Y)
#define ACT_NOQUEST          (Z)
#define ACT_IS_HEALER        (aa)
#define ACT_GAIN             (bb)
#define ACT_UPDATE_ALWAYS    (cc)
#define ACT_IS_CHANGER        (dd)
#define ACT_NOHUNT          (ee)

/* damage classes */
#define DAM_NONE                0
#define DAM_BASH                1
#define DAM_PIERCE              2
#define DAM_SLASH               3
#define DAM_FIRE                4
#define DAM_COLD                5
#define DAM_LIGHTNING           6
#define DAM_ACID                7
#define DAM_POISON              8
#define DAM_NEGATIVE            9
#define DAM_HOLY                10
#define DAM_ENERGY              11
#define DAM_MENTAL              12
#define DAM_DISEASE             13
#define DAM_DROWNING            14
#define DAM_LIGHT        15
#define DAM_OTHER               16
#define DAM_HARM        17
#define DAM_CHARM        18
#define DAM_SOUND        19
#define DAM_BOMB         20

/* OFF bits for mobiles */
#define OFF_AREA_ATTACK         (A)
#define OFF_BACKSTAB            (B)
#define OFF_BASH                (C)
#define OFF_BERSERK             (D)
#define OFF_DISARM              (E)
#define OFF_DODGE               (F)
#define OFF_FADE                (G)
#define OFF_FAST                (H)
#define OFF_KICK                (I)
#define OFF_KICK_DIRT           (J)
#define OFF_PARRY               (K)
#define OFF_RESCUE              (L)
#define OFF_TAIL                (M)
#define OFF_TRIP                (N)
#define OFF_CRUSH        (O)
#define ASSIST_ALL           (P)
#define ASSIST_ALIGN            (Q)
#define ASSIST_RACE                 (R)
#define ASSIST_PLAYERS          (S)
#define ASSIST_GUARD            (T)
#define ASSIST_VNUM        (U)

/* return values for check_imm */
#define IS_NORMAL        0
#define IS_IMMUNE        1
#define IS_RESISTANT        2
#define IS_VULNERABLE        3

/* IMM bits for mobs */
#define IMM_SUMMON              (A)
#define IMM_CHARM               (B)
#define IMM_MAGIC               (C)
#define IMM_WEAPON              (D)
#define IMM_BASH                (E)
#define IMM_PIERCE              (F)
#define IMM_SLASH               (G)
#define IMM_FIRE                (H)
#define IMM_COLD                (I)
#define IMM_LIGHTNING           (J)
#define IMM_ACID                (K)
#define IMM_POISON              (L)
#define IMM_NEGATIVE            (M)
#define IMM_HOLY                (N)
#define IMM_ENERGY              (O)
#define IMM_MENTAL              (P)
#define IMM_DISEASE             (Q)
#define IMM_DROWNING            (R)
#define IMM_LIGHT        (S)
#define IMM_SOUND        (T)
#define IMM_WOOD                (X)
#define IMM_SILVER              (Y)
#define IMM_IRON                (Z)
#define IMM_BOMB                (aa)
#define IMM_ARROW               (bb)

/* RES bits for mobs */
#define RES_SUMMON        (A)
#define RES_CHARM        (B)
#define RES_MAGIC               (C)
#define RES_WEAPON              (D)
#define RES_BASH                (E)
#define RES_PIERCE              (F)
#define RES_SLASH               (G)
#define RES_FIRE                (H)
#define RES_COLD                (I)
#define RES_LIGHTNING           (J)
#define RES_ACID                (K)
#define RES_POISON              (L)
#define RES_NEGATIVE            (M)
#define RES_HOLY                (N)
#define RES_ENERGY              (O)
#define RES_MENTAL              (P)
#define RES_DISEASE             (Q)
#define RES_DROWNING            (R)
#define RES_LIGHT        (S)
#define RES_SOUND        (T)
#define RES_WOOD                (X)
#define RES_SILVER              (Y)
#define RES_IRON                (Z)
#define RES_BOMB                (aa)
#define RES_ARROW                (bb)

/* VULN bits for mobs */
#define VULN_SUMMON        (A)
#define VULN_CHARM        (B)
#define VULN_MAGIC              (C)
#define VULN_WEAPON             (D)
#define VULN_BASH               (E)
#define VULN_PIERCE             (F)
#define VULN_SLASH              (G)
#define VULN_FIRE               (H)
#define VULN_COLD               (I)
#define VULN_LIGHTNING          (J)
#define VULN_ACID               (K)
#define VULN_POISON             (L)
#define VULN_NEGATIVE           (M)
#define VULN_HOLY               (N)
#define VULN_ENERGY             (O)
#define VULN_MENTAL             (P)
#define VULN_DISEASE            (Q)
#define VULN_DROWNING           (R)
#define VULN_LIGHT        (S)
#define VULN_SOUND        (T)
#define VULN_WOOD               (X)
#define VULN_SILVER             (Y)
#define VULN_IRON        (Z)
#define VULN_BOMB        	(aa)
#define VULN_ARROW        	(bb)

/* body form */
#define FORM_EDIBLE             (A)
#define FORM_POISON             (B)
#define FORM_MAGICAL            (C)
#define FORM_INSTANT_DECAY      (D)
#define FORM_OTHER              (E)  /* defined by material bit */

/* actual form */
#define FORM_ANIMAL             (G)
#define FORM_SENTIENT           (H)
#define FORM_UNDEAD             (I)
#define FORM_CONSTRUCT          (J)
#define FORM_MIST               (K)
#define FORM_INTANGIBLE         (L)

#define FORM_BIPED              (M)
#define FORM_CENTAUR            (N)
#define FORM_INSECT             (O)
#define FORM_SPIDER             (P)
#define FORM_CRUSTACEAN         (Q)
#define FORM_WORM               (R)
#define FORM_BLOB        (S)

#define FORM_MAMMAL             (V)
#define FORM_BIRD               (W)
#define FORM_REPTILE            (X)
#define FORM_SNAKE              (Y)
#define FORM_DRAGON             (Z)
#define FORM_AMPHIBIAN          (aa)
#define FORM_FISH               (bb)
#define FORM_COLD_BLOOD        (cc)

/* Traps */
#define TRAP_DAMAGE             0
#define TRAP_MANA               1
#define TRAP_TELEPORT           2
#define TRAP_LATCH              3
#define TRAP_MAX                4

/* Stances */
#define STANCE_NONE		 0
#define STANCE_WHIRLWIND	 1
#define	STANCE_LEAF		 2
#define STANCE_LIGHTNING	 3
#define STANCE_STRAW		 4
#define STANCE_STONE		 5
#define STANCE_CAT		 6
#define STANCE_OFFENSIVE	 7
#define STANCE_DEFENSIVE	 8

/* body parts */
#define PART_HEAD               (A)
#define PART_ARMS               (B)
#define PART_LEGS               (C)
#define PART_HEART              (D)
#define PART_BRAINS             (E)
#define PART_GUTS               (F)
#define PART_HANDS              (G)
#define PART_FEET               (H)
#define PART_FINGERS            (I)
#define PART_EAR                (J)
#define PART_EYE        (K)
#define PART_LONG_TONGUE        (L)
#define PART_EYESTALKS          (M)
#define PART_TENTACLES          (N)
#define PART_FINS               (O)
#define PART_WINGS              (P)
#define PART_TAIL               (Q)
/* for combat */
#define PART_CLAWS              (U)
#define PART_FANGS              (V)
#define PART_HORNS              (W)
#define PART_SCALES             (X)
#define PART_TUSKS        (Y)

#define ROOM_MOUNT_SHOP		(U)

/*
 * New Bits for Simulacrum
 * - Furies Skill #3
 *  Diem - Jan 1/01
 */
#define SIM_AIEL    (A)
#define SIM_WARDER  (B)
#define SIM_SEDAI   (C)
#define SIM_SHADOW  (D)
#define SIM_WOLFBROTHER (E)
#define SIM_REDHAND (F)
#define SIM_SEANCHAN (G)
#define SIM_GLEEMAN  (H)
#define SIM_WHITECLOAK  (I)
#define SIM_TINKER (J)
#define SIM_ASHA  (H)

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */

/** REMOVED FOR UBit

#define AFF_BLIND               (A)
#define AFF_INVISIBLE           (B)
#define AFF_DETECT_EVIL         (C)
#define AFF_DETECT_INVIS        (D)
#define AFF_DETECT_MAGIC        (E)
#define AFF_DETECT_HIDDEN       (F)
#define AFF_DETECT_GOOD         (G)
#define AFF_SANCTUARY           (H)
#define AFF_FAERIE_FIRE         (I)
#define AFF_INFRARED            (J)
#define AFF_CURSE               (K)
#define AFF_RAVAGE              (L)
#define AFF_POISON              (M)
#define AFF_PROTECT_EVIL        (N)
#define AFF_PROTECT_GOOD        (O)
#define AFF_SNEAK               (P)
#define AFF_HIDE                (Q)
#define AFF_SLEEP               (R)
#define AFF_CHARM               (S)
#define AFF_FLYING              (T)
#define AFF_PASS_DOOR           (U)
#define AFF_HASTE               (V)
#define AFF_CALM                (W)
#define AFF_PLAGUE              (X)
#define AFF_WEAKEN              (Y)
#define AFF_DARK_VISION         (Z)
#define AFF_BERSERK             (aa)
#define AFF_SWIM                (bb)
#define AFF_REGENERATION        (cc)
#define AFF_SLOW                (dd)
#define AFF_VEIL                (ee)

**/

/*
 * Affecte2_by
 *
 */

/** Removed for UBIT

#define AFF_WARCRY              (A)
#define AFF_RESTRAIN            (B)
#define AFF_RESTRAIN2           (C)
#define AFF_FIRESTORM           (D)
#define AFF_STILL               (E)
#define AFF_BALLAD              (F)
#define AFF_DOME                (G)
#define AFF_STANCE              (H)
#define AFF_CLEANSE             (I)
#define AFF_PRAY                (J)
#define AFF_STEALTH             (K)
#define AFF_SHROUD              (L)
#define AFF_WOLF                (M)
#define AFF_FOCUS               (N)
#define AFF_DIMENSION_VIEW	(O)
#define AFF_FEAR                (P)
#define AFF_ICESHIELD           (Q)
#define AFF_FIRESHIELD          (R)
#define AFF_SHOCKSHIELD         (S)
#define AFF_CONTACT             (T)
#define AFF_SEVERED             (U)
#define AFF_DAMANE              (V)
#define AFF_MANADRAIN           (X)
#define AFF_MANADRAIN2          (Y)
#define AFF_HOWL                (Z)
#define AFF_MEDALLION           (aa)
#define AFF_DAZE                (bb)

**/



/** Affected With UBit **/

#define AFF_BLIND               (1)
#define AFF_INVISIBLE           (2)
#define AFF_DETECT_EVIL         (3)
#define AFF_DETECT_INVIS        (4)
#define AFF_DETECT_MAGIC        (5)
#define AFF_DETECT_HIDDEN       (6)
#define AFF_DETECT_GOOD         (7)
#define AFF_SANCTUARY           (8)
#define AFF_FAERIE_FIRE         (9)
#define AFF_INFRARED            (10)
#define AFF_CURSE               (11)
#define AFF_RAVAGE              (12)
#define AFF_POISON              (13)
#define AFF_PROTECT_EVIL        (14)
#define AFF_PROTECT_GOOD        (15)
#define AFF_SNEAK               (16)
#define AFF_HIDE                (17)
#define AFF_SLEEP               (18)
#define AFF_CHARM               (19)
#define AFF_FLYING              (20)
#define AFF_PASS_DOOR           (21)
#define AFF_HASTE               (22)
#define AFF_CALM                (23)
#define AFF_PLAGUE              (24)
#define AFF_WEAKEN              (25)
#define AFF_DARK_VISION         (26)
#define AFF_BERSERK             (27)
#define AFF_SWIM                (28)
#define AFF_REGENERATION        (29)
#define AFF_SLOW                (30)
#define AFF_VEIL                (31)
#define AFF_WARCRY              (32)
#define AFF_RESTRAIN            (33)
#define AFF_RESTRAIN2           (34)
#define AFF_FIRESTORM           (35)
#define AFF_STILL               (36)
#define AFF_BALLAD              (37)
#define AFF_DOME                (38)
#define AFF_STANCE              (39)
#define AFF_CLEANSE             (40)
#define AFF_PRAY                (42)
#define AFF_STEALTH             (43)
#define AFF_SHROUD              (44)
#define AFF_WOLF                (45)
#define AFF_FOCUS               (46)
#define AFF_DIMENSION_VIEW	(47)
#define AFF_FEAR                (48)
#define AFF_ICESHIELD           (49)
#define AFF_FIRESHIELD          (50)
#define AFF_SHOCKSHIELD         (51)
#define AFF_CONTACT             (52)
#define AFF_SEVERED             (53)
#define AFF_DAMANE              (54)
#define AFF_MANADRAIN           (55)
#define AFF_MANADRAIN2          (56)
#define AFF_HOWL                (57)
#define AFF_MEDALLION           (58)
#define AFF_DAZE                (59)
#define AFF_IRONSKIN            (60)
#define AFF_AWARENESS           (61)
#define AFF_STICKY              (62)
#define AFF_MANASHIELD          (63)
#define AFF_TELE_DISORIENT      (64)
#define AFF_BLACKJACK           (65)
#define AFF_BLACKJACK2          (66)
#define AFF_ARMOR               (67)
#define AFF_SHIELD              (68)
#define AFF_STONESKIN           (69)
#define AFF_BLESS               (70)
#define AFF_FRENZY              (71)
#define AFF_AIRBARRIER          (72)
#define AFF_DAMPEN              (73)
#define AFF_DISGUISE            (74)
#define AFF_GALLOPING           (75)
#define AFF_FRIGHTEN            (76)
#define AFF_WILLPOWER           (77)
#define AFF_BANDAGE             (78)
#define AFF_SUPER_INVIS         (79)
#define AFF_DIEMCLOAK           (80)
#define AFF_LIMP		(81)
#define AFF_RIFT		(82)
#define AFF_DISORIENT		(83)
#define AFF_GOUGE		(84)
#define AFF_DARKHOUND		(85)

/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL              0
#define SEX_MALE              1
#define SEX_FEMALE              2

/* AC types */
#define AC_PIERCE            0
#define AC_BASH                1
#define AC_SLASH            2
#define AC_EXOTIC            3

/* dice */
#define DICE_NUMBER            0
#define DICE_TYPE            1
#define DICE_BONUS            2

/* size */
#define SIZE_TINY            0
#define SIZE_SMALL            1
#define SIZE_MEDIUM            2
#define SIZE_LARGE            3
#define SIZE_HUGE            4
#define SIZE_GIANT            5

/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_SILVER_ONE          1
#define OBJ_VNUM_GOLD_ONE          2
#define OBJ_VNUM_GOLD_SOME          3
#define OBJ_VNUM_SILVER_SOME          4
#define OBJ_VNUM_COINS              5

#define OBJ_VNUM_CORPSE_NPC         10
#define OBJ_VNUM_CORPSE_PC         11
#define OBJ_VNUM_SEVERED_HEAD         12
#define OBJ_VNUM_TORN_HEART         13
#define OBJ_VNUM_SLICED_ARM         14
#define OBJ_VNUM_SLICED_LEG         15
#define OBJ_VNUM_GUTS             16
#define OBJ_VNUM_BRAINS             17
#define OBJ_VNUM_BERRY		  18
#define OBJ_VNUM_MUSHROOM         20
#define OBJ_VNUM_LIGHT_BALL         21
#define OBJ_VNUM_SPRING             22
#define OBJ_VNUM_DISC             23
#define OBJ_VNUM_PORTAL             25
#define OBJ_VNUM_BLOOD              26
#define OBJ_VNUM_LEGS               28

#define OBJ_VNUM_ROSE           1001

#define OBJ_VNUM_PIT           3010

#define OBJ_VNUM_SCHOOL_MACE       3700
#define OBJ_VNUM_SCHOOL_DAGGER       3701
#define OBJ_VNUM_SCHOOL_SWORD       3702
#define OBJ_VNUM_SCHOOL_SPEAR       3717
#define OBJ_VNUM_SCHOOL_STAFF       3718
#define OBJ_VNUM_SCHOOL_AXE       3719
#define OBJ_VNUM_SCHOOL_FLAIL       3720
#define OBJ_VNUM_SCHOOL_WHIP       3721
#define OBJ_VNUM_SCHOOL_POLEARM    3722
#define OBJ_VNUM_SCHOOL_BOW        3723

#define OBJ_VNUM_SCHOOL_VEST       3703
#define OBJ_VNUM_SCHOOL_SHIELD       3704
#define OBJ_VNUM_SCHOOL_BANNER     3716
#define OBJ_VNUM_MAP           3162

#define OBJ_VNUM_WHISTLE       2116



/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT              1
#define ITEM_SCROLL             2
#define ITEM_WAND               3
#define ITEM_STAFF              4
#define ITEM_WEAPON             5
#define ITEM_TREASURE           8
#define ITEM_ARMOR              9
#define ITEM_POTION             10
#define ITEM_CLOTHING           11
#define ITEM_FURNITURE          12
#define ITEM_TRASH              13
#define ITEM_CONTAINER          15
#define ITEM_DRINK_CON          17
#define ITEM_KEY                18
#define ITEM_FOOD               19
#define ITEM_MONEY              20
#define ITEM_BOAT               22
#define ITEM_CORPSE_NPC         23
#define ITEM_CORPSE_PC          24
#define ITEM_FOUNTAIN           25
#define ITEM_PILL               26
#define ITEM_PROTECT            27
#define ITEM_MAP                28
#define ITEM_PORTAL             29
#define ITEM_WARP_STONE         30
#define ITEM_ROOM_KEY           31
#define ITEM_GEM                32
#define ITEM_JEWELRY            33
#define ITEM_JUKEBOX            34
#define ITEM_TOOL		35
#define ITEM_RESOURCE		36
#define ITEM_CREATURE		37
#define ITEM_BLOOD              38
#define ITEM_WINDOW	       	39
#define ITEM_FUEL               40
#define ITEM_IGNITER            41
#define ITEM_FIRE               42
#define ITEM_QUIVER             43
#define ITEM_ARROW              44
#define ITEM_BOMB               45
#define ITEM_INGREDIENT         46
#define ITEM_SEED               47
#define ITEM_HERB               48
#define ITEM_PLANT              49
#define ITEM_BOOK               50
#define ITEM_TOKEN              51
#define ITEM_LOADED             52
#define ITEM_KEYRING            53
#define ITEM_PAPER              54
#define ITEM_PEN                55
#define ITEM_SEAL               56
#define ITEM_WAX                57
#define ITEM_ENVELOPE           58
#define ITEM_TRAP		59
#define ITEM_PIPE_TABAC		60

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW            (A)
#define ITEM_HUM             (B)
#define ITEM_DARK            (C)
#define ITEM_LOCK            (D)
#define ITEM_EVIL            (E)
#define ITEM_INVIS           (F)
#define ITEM_MAGIC           (G)
#define ITEM_NODROP          (H)
#define ITEM_BLESS           (I)
#define ITEM_ANTI_GOOD       (J)
#define ITEM_ANTI_EVIL       (K)
#define ITEM_ANTI_NEUTRAL    (L)
#define ITEM_NOREMOVE        (M)
#define ITEM_INVENTORY       (N)
#define ITEM_NOPURGE         (O)
#define ITEM_ROT_DEATH       (P)
#define ITEM_VIS_DEATH       (Q)
#define ITEM_FLOATDROP       (R)
#define ITEM_NONMETAL        (S)
#define ITEM_NOLOCATE        (T)
#define ITEM_MELT_DROP       (U)
#define ITEM_HAD_TIMER       (V)
#define ITEM_SELL_EXTRACT    (W)
#define ITEM_BURN_PROOF      (Y)
#define ITEM_NOUNCURSE       (Z)
#define ITEM_NOBREAK         (aa)
#define ITEM_TAINT           (bb)
#define ITEM_BURNING         (cc)
#define ITEM_QUEST           (dd)
#define ITEM_BOMB_BAG        (ee)

/* Smoking flag */
#define TABF_SMOKING  (A)

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE             (A)
#define ITEM_WEAR_FINGER      (B)
#define ITEM_WEAR_NECK        (C)
#define ITEM_WEAR_BODY        (D)
#define ITEM_WEAR_HEAD        (E)
#define ITEM_WEAR_LEGS        (F)
#define ITEM_WEAR_FEET        (G)
#define ITEM_WEAR_HANDS       (H)
#define ITEM_WEAR_ARMS        (I)
#define ITEM_WEAR_SHIELD      (J)
#define ITEM_WEAR_ABOUT       (K)
#define ITEM_WEAR_WAIST       (L)
#define ITEM_WEAR_WRIST       (M)
#define ITEM_WIELD            (N)
#define ITEM_HOLD             (O)
#define ITEM_NO_SAC           (P)
#define ITEM_WEAR_FLOAT       (Q)
#define ITEM_WEAR_FACE        (R)
#define ITEM_WEAR_QUIVER      (S)
#define ITEM_POWER            (T)
#define ITEM_WEAR_ON_BELT     (U)

/* weapon class */
#define WEAPON_EXOTIC         0
#define WEAPON_SWORD          1
#define WEAPON_DAGGER         2
#define WEAPON_SPEAR          3
#define WEAPON_MACE           4
#define WEAPON_AXE            5
#define WEAPON_FLAIL          6
#define WEAPON_WHIP           7
#define WEAPON_POLEARM        8
#define WEAPON_BOW            9
#define WEAPON_STAFF          10

/* weapon types */
#define WEAPON_FLAMING        (A)
#define WEAPON_FROST        (B)
#define WEAPON_VAMPIRIC        (C)
#define WEAPON_SHARP        (D)
#define WEAPON_VORPAL        (E)
#define WEAPON_TWO_HANDS    (F)
#define WEAPON_SHOCKING        (G)
#define WEAPON_POISON        (H)
#define WEAPON_CONTACT       (I)
#define WEAPON_EXPLOSIVE     (J)
/* gate flags */
#define GATE_NORMAL_EXIT    (A)
#define GATE_NOCURSE        (B)
#define GATE_GOWITH        (C)
#define GATE_BUGGY        (D)
#define GATE_RANDOM        (E)

/* furniture flags */
#define STAND_AT        (A)
#define STAND_ON        (B)
#define STAND_IN        (C)
#define SIT_AT            (D)
#define SIT_ON            (E)
#define SIT_IN            (F)
#define REST_AT            (G)
#define REST_ON            (H)
#define REST_IN            (I)
#define SLEEP_AT        (J)
#define SLEEP_ON        (K)
#define SLEEP_IN        (L)
#define PUT_AT            (M)
#define PUT_ON            (N)
#define PUT_IN            (O)
#define PUT_INSIDE        (P)
#define MEDITATE_AT       (Q)
#define MEDITATE_ON       (R)
#define MEDITATE_IN       (S)

/* ingredient flags */
#define ING_SULFUR       (A)
#define ING_CHARCOL      (B)
#define ING_SALT_PETER   (C)
#define ING_PIPE         (D)
#define ING_MANEUR       (E)
#define ING_AMMONIA      (F)
#define ING_ICE          (G)
#define ING_SECRET_WATER (H)
#define ING_BLUBBER      (I)
#define ING_SLEEPING_POWDER  (J)
#define ING_OIL          (K)
#define ING_POISON_POWDER (L)
#define ING_SUGAR         (M)

/* Trigger Flags */
#define TRG_GET         (A)
#define TRG_ENTER_ROOM  (B)
#define TRG_LOOK_AT     (C)
#define TRG_OPEN        (D)
#define TRG_CONTACT     (E)

/* State Flags */
#define STATE_SET       (A)
#define STATE_NOT_SET   (B)
#define STATE_UNSTABLE  (C)
#define STATE_WTF       (D)

/* EXPLOSIVE Flags */
#define EXP_NONE         0
#define EXP_EXPLOSION    1
#define EXP_SLEEPER      2
#define EXP_CONTACT      3
#define EXP_STICKY       4
#define EXP_POISON       5
#define EXP_SMOKE        6

/* Wear Class flags */
#define CLASS_MAGE             (A)
#define CLASS_CLERIC           (B)
#define CLASS_ASSASSIN         (C)
#define CLASS_WARRIOR          (D)
#define CLASS_ARCHER           (E)
#define CLASS_ROGUE            (F)
#define CLASS_ILLUMINATOR      (G)
#define CLASS_FORM             (H)
#define CLASS_THIEF            (I)
#define CLASS_FORSAKEN         (J)
#define CLASS_DRAGON           (K)
#define CLASS_HERBALIST        (L)


/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE                  0
#define APPLY_STR                   1
#define APPLY_DEX                   2
#define APPLY_INT                   3
#define APPLY_WIS                   4
#define APPLY_CON                   5
#define APPLY_SEX                   6
#define APPLY_CLASS                 7
#define APPLY_LEVEL                 8
#define APPLY_AGE                   9
#define APPLY_HEIGHT               10
#define APPLY_WEIGHT               11
#define APPLY_MANA                 12
#define APPLY_HIT                  13
#define APPLY_MOVE                 14
#define APPLY_GOLD                 15
#define APPLY_EXP                  16
#define APPLY_AC                   17
#define APPLY_HITROLL              18
#define APPLY_DAMROLL              19
#define APPLY_SAVES                20
#define APPLY_SAVING_PARA          20
#define APPLY_SAVING_ROD           21
#define APPLY_SAVING_PETRI         22
#define APPLY_SAVING_BREATH        23
#define APPLY_SAVING_SPELL         24
#define APPLY_SPELL_AFFECT         25
#define APPLY_SPELL_STR                  26
#define APPLY_SPELL_DEX                  27
#define APPLY_SPELL_INT                  28
#define APPLY_SPELL_WIS                  29
#define APPLY_SPELL_CON                  30
#define APPLY_SPELL_SEX                  31
#define APPLY_SPELL_CLASS                32
#define APPLY_SPELL_LEVEL                33
#define APPLY_SPELL_AGE                  34
#define APPLY_SPELL_HEIGHT               35
#define APPLY_SPELL_WEIGHT               36
#define APPLY_SPELL_MANA                 37
#define APPLY_SPELL_HIT                  38
#define APPLY_SPELL_MOVE                 39
#define APPLY_SPELL_AC                   40
#define APPLY_SPELL_HITROLL              41
#define APPLY_SPELL_DAMROLL              42
#define APPLY_SPELL_SAVES                43
#define APPLY_SPELL_SAVING_ROD           44
#define APPLY_SPELL_SAVING_PETRI         45
#define APPLY_SPELL_SAVING_BREATH        46
#define APPLY_SPELL_SAVING_SPELL         47
#define APPLY_SPELL_SAVING_PARA          48
/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE              1
#define CONT_PICKPROOF              2
#define CONT_CLOSED              4
#define CONT_LOCKED              8
#define CONT_PUT_ON             16



/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO              2
#define ROOM_VNUM_DONATION        3399
#define ROOM_VNUM_CHAT           1200
#define ROOM_VNUM_TEMPLE       3001
#define ROOM_VNUM_ALTAR           3054
#define ROOM_VNUM_SCHOOL       3700
#define ROOM_VNUM_BALANCE       4500
#define ROOM_VNUM_CIRCLE       4400
#define ROOM_VNUM_DEMISE       4201
#define ROOM_VNUM_HONOR           4300
#define ROOM_VNUM_QUEST        1
#define ROOM_VNUM_MORGUE        15
#define ROOM_VNUM_JAIL          10


#define VNUM_MOB_SHOPKEEP       10
/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK        (A)
#define ROOM_NO_MOB        (C)
#define ROOM_INDOORS        (D)

#define ROOM_PRIVATE        (J)
#define ROOM_SAFE        (K)
#define ROOM_SOLITARY        (L)
#define ROOM_PET_SHOP        (M)
#define ROOM_NO_RECALL        (N)
#define ROOM_IMP_ONLY        (O)
#define ROOM_GODS_ONLY        (P)
#define ROOM_HEROES_ONLY    (Q)
#define ROOM_NEWBIES_ONLY    (R)
#define ROOM_LAW        (S)
#define ROOM_NOWHERE        (T)
#define ROOM_BANK	(U)
#define ROOM_NOMAGIC	(V)
#define ROOM_NOPORT     (W)
#define ROOM_DEATHTRAP  (X)

#define ROOM_ROLEPRIZE  (Y)
#define ROOM_EMPTY_STORE (Z)
#define ROOM_NOPUSH (aa)
#define ROOM_NODREAM (bb)
#define ROOM_NOPORT_FROM (cc)
#define ROOM_WILDERNESS (dd)

#define ROOM_AIEL	   (B)
#define ROOM_SHADOW	   (E)
#define ROOM_WHITECLOAK    (F)
#define ROOM_SHAIDO        (G)
#define ROOM_WHITETOWER    (H)
#define ROOM_TINKER        (I)
#define ROOM_ASHAMAN       (ee)

/*#define ROOM_RENTAL (W)    room rental -used X declaration sorry -Cammy
#define	ROOM_RENTED (X)*/


/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH              0
#define DIR_EAST              1
#define DIR_SOUTH              2
#define DIR_WEST              3
#define DIR_UP                  4
#define DIR_DOWN              5



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR              (A)
#define EX_CLOSED              (B)
#define EX_LOCKED              (C)
#define EX_PICKPROOF              (F)
#define EX_NOPASS              (G)
#define EX_EASY                  (H)
#define EX_HARD                  (I)
#define EX_INFURIATING              (J)
#define EX_NOCLOSE              (K)
#define EX_NOLOCK              (L)
//#define EX_ISWALL              (M)



/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE                0
#define SECT_CITY                  1
#define SECT_FIELD                 2
#define SECT_FOREST                3
#define SECT_HILLS                 4
#define SECT_MOUNTAIN              5
#define SECT_WATER_SWIM            6
#define SECT_WATER_NOSWIM          7
#define SECT_ROAD                  8
#define SECT_AIR                   9
#define SECT_DESERT               10
#define SECT_ROCK_MOUNTAIN        11
#define SECT_SNOW_MOUNTAIN        12
#define SECT_ENTER                13
#define SECT_UNUSED               14
#define SECT_SWAMP                15
#define SECT_JUNGLE               16
#define SECT_RUINS                17
#define SECT_REGION               18
#define SECT_COAST                19
#define SECT_VOLCANIC_MOUNTAIN    20
#define SECT_MAX                  30
#define SECT_WALL		  22
#define SECT_GATE		  23
#define SECT_TOWER		  24
#define SECT_JUNCTION		  25
#define SECT_EDGE		  26
#define SECT_VEDGE		  27
#define SECT_EMPTY		  28
#define SECT_TAKEN		  29

// #define SECT_LEGEND          12

/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE             -1
#define WEAR_LIGHT             0
#define WEAR_FINGER_L          1
#define WEAR_FINGER_R          2
#define WEAR_NECK_1            3
#define WEAR_NECK_2            4
#define WEAR_BODY              5
#define WEAR_HEAD              6
#define WEAR_LEGS              7
#define WEAR_FEET              8
#define WEAR_HANDS             9
#define WEAR_ARMS              10
#define WEAR_SHIELD            11
#define WEAR_ABOUT             12
#define WEAR_WAIST             13
#define WEAR_ON_BELT           14
#define WEAR_WRIST_L           15
#define WEAR_WRIST_R           16
#define WEAR_WIELD             17
#define WEAR_SECONDARY         18
#define WEAR_HOLD              19
#define WEAR_FLOAT             20
#define WEAR_FACE              21
#define WEAR_QUIVER            22
#define MAX_WEAR               23



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK               0
#define COND_FULL                1
#define COND_THIRST              2
#define COND_HUNGER              3
#define COND_BLEEDING            4
#define COND_HORNY               5
#define COND_GOBLIND             6



/*
 * Positions.
 */
#define POS_DEAD                  0
#define POS_MORTAL                1
#define POS_INCAP                 2
#define POS_STUNNED               3
#define POS_SLEEPING              4
#define POS_RESTING               5
#define POS_SITTING               6
#define POS_FIGHTING              7
#define POS_STANDING              8
#define POS_MEDITATING            9



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC        (A)        /* Don't EVER set.    */


/* RT auto flags */
#define PLR_AUTOASSIST        (C)
#define PLR_AUTOEXIT        (D)
#define PLR_AUTOLOOT        (E)
#define PLR_AUTOSAC             (F)
#define PLR_AUTOGOLD        (G)
#define PLR_AUTOSPLIT        (H)
#define PLR_QUESTOR       (I)
#define PLR_HUNGER_FLAG     (J)

#define IS_QUESTOR(ch)     (IS_SET((ch)->act, PLR_QUESTOR))
#define gold_weight(amount) ((amount) / 100 )
#define silver_weight(amount) ((amount) / 200)

/* RT personal flags */
#define PLR_HOLYLIGHT        (N)
#define PLR_CANLOOT        (P)
#define PLR_NOSUMMON        (Q)
#define PLR_NOFOLLOW        (R)
#define PLR_COLOUR        (T)
/* 1 bit reserved, S */

/* penalty flags */
#define PLR_PERMIT        (U)
#define PLR_LOG            (W)
#define PLR_DENY        (X)
#define PLR_FREEZE        (Y)
#define PLR_TAG        (Z)
#define PLR_IT        (aa)
#define PLR_NOPK          (bb)
#define PLR_QUESTING      (cc)
//#define	PLR_TENNANT 	(dd) room rental
#define PLR_TOURNEY    (ee)

/*
 *  Flag for player2
 */

#define PLR_NOIP             (A)
#define PLR_DREAM            (B)
#define PLR_NOSKILLS         (C)
#define PLR_GUILDLEADER      (D)
#define PLR_MSP_MUSIC        (E)
#define PLR_MSP_SOUND        (E)
#define PLR_MSP_PLAYING      (F)
#define PLR_FLED             (G)
#define PLR_NOTITLE          (H)
#define PLR_NODAMAGE         (I)
#define PLR_WAR              (J)
#define PLR_NOEXP            (K)
#define PLR_DEVELOPER        (L)
#define PLR_NORP             (M)
#define PLR_TOURNAMENT_START (N)
#define PLR_MXP              (O)
#define PLR_TOURNAMENT_SPECT (P)
#define PLR_NOWEAVE          (Q)
#define PLR_TESTOR           (R)
#define PLR_NOEQ             (S)
#define PLR_LOOTABLE         (T)
#define PLR_WORLDMAP	     (X)

/* RT comm flags -- may be used on both mobs and chars */
#define COMM_QUIET              (A)
#define COMM_DEAF                (B)
#define COMM_NOWIZ              (C)
#define COMM_NOAUCTION          (D)
#define COMM_NOGOSSIP           (E)
#define COMM_NOQUESTION         (F)
#define COMM_NOMUSIC            (G)
#define COMM_NOCLAN        (H)
#define COMM_NOQUOTE        (I)
#define COMM_SHOUTSOFF        (J)
#define COMM_NOOOC            (K)
/* display flags */
#define COMM_COMPACT        (L)
#define COMM_BRIEF        (M)
#define COMM_PROMPT        (N)
#define COMM_COMBINE        (O)
#define COMM_TELNET_GA        (P)
#define COMM_SHOW_AFFECTS    (Q)
#define COMM_NOGRATS        (R)
#define COMM_NOTRIVIA       (S)
#define COMM_NOIMP          (T)
#define COMM_NOVICE         (U)

/* penalties */
#define COMM_NOEMOTE        (cc)
#define COMM_NOTELL        (V)
#define COMM_NOCHANNELS        (W)
#define COMM_SNOOP_PROOF    (Y)
#define COMM_AFK        (Z)
#define COMM_NOBITCH       (aa)
#define COMM_NORADIO         (bb)
#define COMM_NOTOURNEY       (dd)

#define CHANNEL_AUCTION		(A)
/* WIZnet flags */
#define WIZ_ON            (A)
#define WIZ_TICKS        (B)
#define WIZ_LOGINS        (C)
#define WIZ_SITES        (D)
#define WIZ_LINKS        (E)
#define WIZ_DEATHS        (F)
#define WIZ_RESETS        (G)
#define WIZ_MOBDEATHS        (H)
#define WIZ_FLAGS        (I)
#define WIZ_PENALTIES        (J)
#define WIZ_SACCING        (K)
#define WIZ_LEVELS        (L)
#define WIZ_SECURE        (M)
#define WIZ_SWITCHES        (N)
#define WIZ_SNOOPS        (O)
#define WIZ_RESTORE        (P)
#define WIZ_LOAD        (Q)
#define WIZ_NEWBIE        (R)
#define WIZ_PREFIX        (S)
#define WIZ_SPAM        (T)
#define WIZ_RP          (U)
#define WIZ_MEMCHECK     (V)
#define WIZ_PVP		(W)

/* Object List Type */
#define ON_GROUND                1
#define ON_INVENTORY             2
#define ON_INCONTAINER           3

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct    mob_index_data
{
    MOB_INDEX_DATA *    next;
    SPEC_FUN *        spec_fun;
    SHOP_DATA *        pShop;
    PROG_LIST *        mprogs;
    AREA_DATA *        area;        /* OLC */
    long        vnum;
    sh_int        group;
    bool        new_format;
    sh_int count;

    sh_int        killed;
    char *        player_name;
    char *        short_descr;
    char *        long_descr;
    char *        description;
    long         act;
    FLAG        affected_by[MAX_FLAGS];
    long        affected_by2;
    long        simaff;
    sh_int        alignment;
    sh_int        level;
    sh_int        hitroll;
    long          hit[3];
    sh_int        mana[3];
    sh_int        damage[3];
    sh_int        ac[4];
    sh_int         dam_type;
    long        off_flags;
    long        imm_flags;
    long        res_flags;
    long        vuln_flags;
    sh_int        start_pos;
    sh_int        default_pos;
    sh_int        sex;
    sh_int        race;
    long        wealth;
    long        form;
    long        parts;
    sh_int        size;
    char *        material;
    long        mprog_flags;
    struct
    {
      int start;
      int end;
    } load_time;
};



/* memory settings */
#define MEM_CUSTOMER    A
#define MEM_SELLER    B
#define MEM_HOSTILE    C
#define MEM_AFRAID    D

/* memory for mobs */
struct mem_data
{
    MEM_DATA     *next;
    bool    valid;
    long        id;
    int     reaction;
    time_t     when;
};


/*
 * One character (PC or NPC).
 */
struct    char_data
{
    PC_DATA *        pcdata;
    DESCRIPTOR_DATA *    desc;
    long        hit;
    SAID_TO_DATA         *saidto;
    long        max_hit;
    sh_int        mana;
    sh_int        max_mana;
    sh_int        move;
    sh_int        max_move;
    sh_int        level;
    sh_int        rplevel;
    sh_int        rpexp;
    sh_int        trust;
    sh_int        fame;
    sh_int        honor;
    sh_int        update;
    sh_int        darkcounter;
    sh_int	  mapsize;
    char *       pretit;
    long        gold;
    long        silver;
    long	gold_in_bank;
    long	silver_in_bank;
    int            stunned;
    int            exp;
    int		   realexp;
    CHAR_DATA *        rescuer;
    CHAR_DATA *        attacker;
    int                attacking;

    CHAR_DATA *        next;
    CHAR_DATA *        next_in_room;
    CHAR_DATA *        master;
    CHAR_DATA *        leader;
    CHAR_DATA *        fighting;
    CHAR_DATA *        oreply;
    CHAR_DATA *        ireply;
    CHAR_DATA *        pet;
    CHAR_DATA *        mprog_target;
    MEM_DATA *        memory;
    SPEC_FUN *        spec_fun;
    MOB_INDEX_DATA *    pIndexData;
    AFFECT_DATA *    affected;
    NOTE_DATA *        pnote;
    OBJ_DATA *        carrying;
    OBJ_DATA *        on;
    ROOM_INDEX_DATA *    in_room;
    ROOM_INDEX_DATA *    was_in_room;
    AREA_DATA *        zone;
    GEN_DATA *        gen_data;
    int			interdimension;
    /* Asmo Mount */
    CHAR_DATA *mount;
    bool is_mounted;


    bool        valid;
    char *        name;
    long        id;
    sh_int        version;
    char *        short_descr;
    char *        long_descr;
    char *        description;
    char *        prompt;
    char *        prefix;
    char *        simskill;
    sh_int        group;
    sh_int        clan;
    sh_int        rank;
    sh_int        sex;
    sh_int        cClass;
    sh_int        race;
    int            played;
    int            lines;  /* for the pager */
    time_t        logon;
    sh_int        timer;
    sh_int        wait;
    sh_int        daze;
    long        act;
    long        act2;
    long        comm;   /* RT added to pad the vector */
    long        wiznet; /* wiz stuff */
    long        imm_flags;
    long        res_flags;
    long        vuln_flags;
    long        tabf;
    sh_int        invis_level;
    sh_int        incog_level;
    FLAG            affected_by[AFF_FLAGS];
    long          affected_by2;
    long          simaff;
    sh_int        position;
    sh_int        practice;
    sh_int        train;
    sh_int        carry_weight;
    sh_int        carry_number;
    sh_int        saving_throw;
    sh_int        alignment;
    sh_int        hitroll;
    sh_int        damroll;
    sh_int        armor[4];
    sh_int        wimpy;
    sh_int	  state;
    sh_int	  action;
    sh_int	  quality;
    /* stats */
    sh_int        perm_stat[MAX_STATS];
    sh_int        mod_stat[MAX_STATS];
    /* parts stuff */
    long        form;
    long        parts;
    sh_int        size;
    char*        material;
    /* mobile stuff */
    long        off_flags;
    sh_int        damage[3];
    sh_int        dam_type;
    sh_int        start_pos;
    sh_int        default_pos;

    sh_int        mprog_delay;
    long        mprog_delay_vnum;
    sh_int        gametick;
    char*         plaid;
    CHAR_DATA *         questgiver; /* Vassago */
    long                 questpoints;  /* Vassago */
    sh_int              countdown; /* Vassago */
    sh_int              questobj; /* Vassago */
    long              questmob; /* Vassago */
    sh_int              rescuemob; /* Vassago */
    sh_int              nextquest;
    sh_int              questriddle; /* Asmo */
    sh_int		auction;
    time_t		last_bank;
//    int			auction;
	int           rp_points;
	CHAR_DATA * hunting;
    struct
    {
    	int         thief;
        int         murder;
        int         jail;
        int         punishment;
    }penalty;

    struct {
             int sn;
	     int seconds;
	   } timed_affect;

};



/*
 * Data which only PC's have.
 */
struct    pc_data
{
    PC_DATA *        next;
    BUFFER *         buffer;
    COLOUR_DATA *    code;        /* Data for coloUr configuration    */
    QUEST_DATA *     quests;
    sh_int        weaves[MAX_WEAVES];
    bool          valid;
    bool          barOn;
    char *        ignore;
    char *        bond;
    char *        pwd;
    char *        bamfin;
    char *        bamfout;
    char *        title;
    bool          primary;
    char *        account;
    char *        pfullname;
    char *        pemail;
    char *        page;
    char *        pcity;
    char *        pstate;
    char *        pcountry;
    char *        pwebpage;
    char *        manadrainee;
    time_t              last_note;
    time_t              last_idea;
    time_t              last_guildmaster;
    time_t              last_news;
    time_t              last_changes;
    time_t              last_imm;
    time_t              last_rp;
    time_t              last_guide;
    time_t              last_store;
    long       	        perm_hit;
    sh_int              perm_mana;
    sh_int      	 perm_move;
    sh_int      	 true_sex;
    char *       	 lastKilled; /* who killed player last you can loot */
    int           	 last_level;
    sh_int               condition    [7];
    sh_int               learned        [MAX_SKILL];
    bool      	         group_known    [MAX_GROUP];
    sh_int      	 points;
    bool                 confirm_delete;
    bool                 confirm_pkon;
    ROOM_INDEX_DATA *    dream_to_room;
    bool                 dream;
    char *               alias[MAX_ALIAS];
    char *               alias_sub[MAX_ALIAS];
    char *               granted;
    char *       	 forsaken_master;
    bool         	 isMinion;
    int                  security;    /* OLC */ /* Builder security */
    int                  stance;
    PK_DATA * 		 kill;  // Tracking pk stuff
    PK_DATA * 		 atkon; // Tracking pk stuff
    PK_DATA * 		 atkby; // Tracking pk stuff
    char                 explored[MAX_EXPLORE];
    char                 mobbed[MAX_EXPLORE];

    struct idName *  names;

    long          initiator;
     long			gold_in_bank;
     long			silver_in_bank;
     long			last_bank;
     sh_int                    pk_timer;   /* Safe or not */
     sh_int		       portcounter;
     sh_int                    aggro_timer;   /* For music */
     sh_int                    safe_timer; /* Just Died */
     sh_int                    wait_timer; /* For ticks after coming in game */

    char *              mxpVersion;
    char *              client;
    char *              clientVersion;

    char *              questarea;
    char *              questroom;



    int 		damInflicted;
    int 		damReceived;
     struct {
              sh_int cast;
              sh_int couple;
     } couple[5];

     bool compressed;

     struct {
               bool on;
	       int factor;
	       int time;
     } xpmultiplier;
     /*int			rentroom;  room rental*/
    struct
    {
	sh_int kills;
	sh_int deaths;
	int reputation;
        sh_int rank;
	sh_int last_rank;
        long last_killed;
    } tournament;

    struct
    {
        char *rpnote;
        char *notes;
    } fonts;
    struct
    {
    	char * orig_name;
    	char * orig_short;
    	char * orig_long;
    	char * orig_desc;
    	char * orig_title;
    	sh_int orig_sex;

    }disguise;
    char      *Host;

};

/* Data for generating characters -- only used during generation */
struct gen_data
{
    GEN_DATA    *next;
    bool    valid;
    bool    skill_chosen[MAX_SKILL];
    bool    group_chosen[MAX_GROUP];
    int        points_chosen;
};

struct couple_type
{
  sh_int cast;
  sh_int couple;
};

struct store_type
{
  STORE_DATA * next;
  bool valid;
  long vnum;
  char *owner;
  STORE_LIST_DATA *selling;
  SOLD_LIST_DATA *sold;
  int itemNumber;
  int coffer;
  int guild;

  struct
  {
    bool mob;
    long vnum;
    char *keeper;
    long lastpaid;
  } shopkeeper;

};

struct store_list_type
{
  bool valid;
  OBJ_DATA *item;
  STORE_LIST_DATA *next;
  int       cost;
};

struct sold_list_type
{
  bool valid;
  SOLD_LIST_DATA *next;
  char *name;
  int cost;
};

/*
 * Liquids.
 */
#define LIQ_WATER        0

struct    liq_type
{
    char *    liq_name;
    char *    liq_color;
    sh_int    liq_affect[5];
};



/*
 * Extra description data for a room or object.
 */
struct    extra_descr_data
{
    EXTRA_DESCR_DATA *next;    /* Next in list                     */
    bool valid;
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
};



/*
 * Prototype for an object.
 */
struct    obj_index_data
{
    OBJ_INDEX_DATA *    next;
    EXTRA_DESCR_DATA *    extra_descr;
    AFFECT_DATA *    affected;
    AREA_DATA *        area;        /* OLC */
    PROG_LIST *        oprogs;
    long               oprog_flags;
    bool        new_format;
    char *        name;
    char *        short_descr;
    char *        description;
    long        vnum;
    sh_int        reset_num;
    char *        material;
    sh_int        item_type;
    int            extra_flags;
    int            wear_flags;
    int            class_flags;
    sh_int        level;
    sh_int        honor_level;
    int         condition;
    sh_int        count;
    sh_int        weight;
    int            cost;
    long            value[5];
    sh_int        timer;
    char *        timer_msg;
};



/*
 * One object.
 */
struct    obj_data
{
    OBJ_DATA *        next;
    OBJ_DATA *        next_content;
    OBJ_DATA *        contains;
    OBJ_DATA *        in_obj;
    OBJ_DATA *        on;
    CHAR_DATA *        carried_by;
    EXTRA_DESCR_DATA *    extra_descr;
    AFFECT_DATA *    affected;
    OBJ_INDEX_DATA *    pIndexData;
    ROOM_INDEX_DATA *    in_room;
    STORE_DATA *         in_store;
    CHAR_DATA *     oprog_target;
    sh_int                  oprog_delay;

    bool        valid;
    bool        enchanted;
    //bool        spellaffected;
    char *            owner;
    char *        lastKilled; /* Person who killed last */
    int           lootsLeft; /* Loots left */
    char *        name;
    char *        short_descr;
    char *        description;
    sh_int        item_type;
    int            extra_flags;
    int            wear_flags;
    int            class_flags;
    sh_int        wear_loc;
    sh_int        weight;
    int            cost;
    sh_int        level;
    sh_int        honor_level;
    sh_int         condition;
    char *        material;
    sh_int        timer;
    char *        timer_msg;
    int            value    [5];
    long          id;
    sh_int        enchants;
    bool          pk;
};



/*
 * Exit data.
 */
struct    exit_data
{
    union
    {
      ROOM_INDEX_DATA *    to_room;
      long            vnum;
    } u1;
    sh_int        exit_info;
    sh_int        key;
    char *        keyword;
    char *        description;
    EXIT_DATA *        next;        /* OLC */
    int            rs_flags;    /* OLC */
    int            orig_door;    /* OLC */
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct    reset_data
{
    RESET_DATA *    next;
    char        command;
    long        arg1;
    long        arg2;
    long        arg3;
    long        arg4;
    sh_int        percent;
};



/*
 * Area definition.
 */
struct    area_data
{
    AREA_DATA *        next;
    HELP_AREA *        helps;
    char *        file_name;
    char *        name;
    char *        credits;
    sh_int        age;
    sh_int	  x;
    sh_int	  y;
    sh_int	  start;
    sh_int        nplayer;
    sh_int        low_range;
    sh_int        high_range;
    long         min_vnum;
    long        max_vnum;
    bool        empty;
    sh_int         version; // Added by Asmo
    char *        builders;    /* OLC */ /* Listing of */
    long            vnum;        /* OLC */ /* Area vnum  */
    int            area_flags;    /* OLC */
    int            security;    /* OLC */ /* Value 1-9  */
};



/*
 * Room type.
 */
struct    room_index_data
{
    ROOM_INDEX_DATA *    next;
    CHAR_DATA *        people;
    OBJ_DATA *        contents;
    EXTRA_DESCR_DATA *    extra_descr;
    AREA_DATA *        area;
    EXIT_DATA *        exit    [6];
    RESET_DATA *    reset_first;    /* OLC */
    RESET_DATA *    reset_last;    /* OLC */
    STORE_DATA *  store;
    PROG_LIST *         rprogs;
    CHAR_DATA *         rprog_target;
    long                rprog_flags;
    sh_int              rprog_delay;
    bool          entry_bomb;
    char *        name;
    char *        description;
    char *        owner;
    long        vnum;
    int            room_flags;
    sh_int        light;
    sh_int        sector_type;
    sh_int	  wood;
    sh_int	  build;
    sh_int	  ground;
    sh_int	  x;
    sh_int	  y;
    sh_int	  room;
    sh_int        heal_rate;
    sh_int         mana_rate;
    sh_int        clan;
};



/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000
#define TYPE_SPECIAL                 1040
#define TYPE_EXPLOSION               1046
#define TYPE_MAGIC                   2000


/*
 *  Target types.
 */
#define TAR_IGNORE            0
#define TAR_CHAR_OFFENSIVE        1
#define TAR_CHAR_DEFENSIVE        2
#define TAR_CHAR_SELF            3
#define TAR_OBJ_INV            4
#define TAR_OBJ_CHAR_DEF        5
#define TAR_OBJ_CHAR_OFF        6
#define TAR_OBJ_SPELL           7

#define TARGET_CHAR            0
#define TARGET_OBJ            1
#define TARGET_ROOM            2
#define TARGET_NONE            3



/*
 * Skills include spells as a particular case.
 */
struct    skill_type
{
    char *    name;            /* Name of skill        */
    sh_int    skill_level[MAX_CLASS];    /* Level needed by class    */
    sh_int    rating[MAX_CLASS];    /* How hard it is to learn    */
    sh_int    weaves[MAX_WEAVES];   /* What weaves are the spells based off of */
    SPELL_FUN *    spell_fun;        /* Spell pointer (for spells)    */
    sh_int    target;            /* Legal targets        */
    sh_int    minimum_position;    /* Position for caster / user    */
    sh_int *    pgsn;            /* Pointer to associated gsn    */
    sh_int    slot;            /* Slot for #OBJECT loading    */
    sh_int    min_mana;        /* Minimum mana used        */
    sh_int    beats;            /* Waiting time after use    */
    char *    noun_damage;        /* Damage message        */
    char *    msg_off;        /* Wear off message        */
    char *    msg_obj;        /* Wear off message for obects    */
};

struct  group_type
{
    char *    name;
    sh_int    rating[MAX_CLASS];
    char *    spells[MAX_IN_GROUP];
};

/*
 * MOBprog definitions
 */
#define TRIG_ACT    (A)
#define TRIG_BRIBE    (B)
#define TRIG_DEATH    (C)
#define TRIG_ENTRY    (D)
#define TRIG_FIGHT    (E)
#define TRIG_GIVE    (F)
#define TRIG_GREET    (G)
#define TRIG_GRALL    (H)
#define TRIG_KILL    (I)
#define TRIG_HPCNT    (J)
#define TRIG_RANDOM    (K)
#define TRIG_SPEECH    (L)
#define TRIG_EXIT    (M)
#define TRIG_EXALL    (N)
#define TRIG_DELAY    (O)
#define TRIG_SURR    (P)
#define TRIG_GET        (Q)
#define TRIG_DROP       (R)
#define TRIG_SIT        (S)
#define TRIG_YELL       (T)

/*
 * Prog types
 */
#define PRG_MPROG       0
#define PRG_OPROG       1
#define PRG_RPROG       2


struct prog_list
{
    int                 trig_type;
    char *              trig_phrase;
    long              vnum;
    char *              code;
    PROG_LIST *         next;
    bool                valid;
};

struct delay_prog_code
{
  long         time;
  long       vnum;
  char *       code;
  CHAR_DATA   * mob;
  ROOM_INDEX_DATA   * room;
  OBJ_DATA    * obj;
  CHAR_DATA     *ch;
  const void  * arg1;
  const void  * arg2;
  sh_int       line;
  DELAY_CODE  * next;
};

struct prog_code
{
    long              vnum;
    char *              code;
    PROG_CODE *         next;
};

/*
 * These are skill_lookup return values for common skills and spells.
 */
extern    sh_int    gsn_backstab;
extern    sh_int    gsn_dodge;
extern    sh_int    gsn_envenom;
extern    sh_int    gsn_hide;
extern    sh_int    gsn_peek;
extern    sh_int    gsn_pick_lock;
extern    sh_int    gsn_sneak;
extern    sh_int    gsn_steal;

extern    sh_int    gsn_disarm;
extern    sh_int    gsn_enhanced_damage;
extern    sh_int    gsn_kick;
extern    sh_int    gsn_parry;
extern    sh_int    gsn_rescue;
extern    sh_int    gsn_second_attack;
extern    sh_int    gsn_third_attack;
extern    sh_int    gsn_fourth_attack;

extern    sh_int    gsn_blindness;
extern    sh_int    gsn_charm_person;
extern    sh_int    gsn_curse;
extern    sh_int    gsn_invis;
extern    sh_int    gsn_mass_invis;
extern  sh_int  gsn_plague;
extern    sh_int    gsn_poison;
extern    sh_int    gsn_sleep;
extern  sh_int  gsn_fly;
extern  sh_int  gsn_sanctuary;
extern  sh_int  gsn_disenchant;

/* new gsns */
extern sh_int  gsn_axe;
extern sh_int  gsn_dagger;
extern sh_int  gsn_flail;
extern sh_int  gsn_mace;
extern sh_int  gsn_polearm;
extern sh_int  gsn_shield_block;
extern sh_int  gsn_spear;
extern sh_int  gsn_staff;
extern sh_int  gsn_sword;
extern sh_int  gsn_whip;

extern sh_int  gsn_bow;
extern sh_int  gsn_second_shot;
extern sh_int  gsn_third_shot;

extern sh_int  gsn_bash;
extern sh_int  gsn_berserk;
extern sh_int  gsn_dirt;
extern sh_int  gsn_hand_to_hand;
extern sh_int  gsn_martial_arts;
extern sh_int  gsn_trip;

extern sh_int  gsn_fast_healing;
extern sh_int  gsn_haggle;
extern sh_int  gsn_lore;
extern sh_int  gsn_meditation;
extern sh_int  gsn_bandage;

extern sh_int  gsn_scrolls;
extern sh_int  gsn_staves;
extern sh_int  gsn_veil;
extern sh_int  gsn_wands;
extern sh_int  gsn_recall;
extern sh_int  gsn_dream;
extern sh_int  gsn_ensnare;
extern sh_int  gsn_ravage;
extern sh_int  gsn_contact;
extern sh_int  gsn_blood;
extern sh_int  gsn_luck;
extern sh_int  gsn_warcry;
extern sh_int  gsn_void;
extern sh_int  gsn_restrain;
extern sh_int  gsn_firestorm;
extern sh_int  gsn_still;
extern sh_int  gsn_balefire;
extern sh_int  gsn_ballad;
extern sh_int  gsn_dome;
extern sh_int  gsn_stance;
extern sh_int  gsn_cleanse;
extern sh_int  gsn_pray;
extern sh_int  gsn_plant;
extern sh_int  gsn_stealth;
extern sh_int  gsn_shroud;
extern sh_int  gsn_wolf;
extern sh_int  gsn_damane;
extern sh_int  gsn_hunt;
extern sh_int  gsn_fix;
extern sh_int  gsn_simulacrum;
extern sh_int  gsn_focus;
extern sh_int  gsn_assassinate;
extern sh_int  gsn_dual_wield;
extern sh_int  gsn_engage;

extern sh_int gsn_iceshield;
extern sh_int gsn_fireshield;
extern sh_int gsn_shockshield;
extern sh_int gsn_manashield;
extern sh_int gsn_simulacrum;

extern sh_int gsn_strike;
extern sh_int gsn_evade;
extern sh_int gsn_block;
extern sh_int gsn_flying_kick;
extern sh_int gsn_shadow_kick;
extern sh_int gsn_marironskin;
extern sh_int gsn_awareness;
extern sh_int gsn_flip;
extern sh_int gsn_counter;
extern sh_int gsn_blackjack;
extern sh_int gsn_blackjack2;
extern sh_int gsn_cloak;

extern sh_int gsn_track;
extern sh_int gsn_build;

extern sh_int gsn_manadrain;

extern sh_int gsn_howl;
extern sh_int gsn_medallion;
extern sh_int gsn_daze;
extern sh_int gsn_bomb;
extern sh_int gsn_openbag;
extern sh_int gsn_steal_from_bag;
extern sh_int gsn_peek_in;
extern sh_int gsn_disguise;
extern sh_int gsn_sticky;

extern sh_int gsn_study;
extern sh_int gsn_store;
extern sh_int gsn_horseback;
extern sh_int gsn_scry;
extern sh_int gsn_dampen;
extern sh_int gsn_couple;
extern sh_int gsn_galloping;
extern sh_int gsn_tame;
extern sh_int gsn_willpower;

/* Demiscus new skills */

extern sh_int gsn_darkhound;
extern sh_int gsn_unseen_fury;
extern sh_int gsn_gouge;
extern sh_int gsn_blind_sight;
extern sh_int gsn_trapdisarm;
extern sh_int gsn_trapset;
extern sh_int gsn_trapset2;
extern sh_int gsn_disorient;
extern sh_int gsn_rift;
extern sh_int gsn_quickening;
extern sh_int gsn_rub;
extern sh_int gsn_dance;
extern sh_int gsn_style;
extern sh_int gsn_gash;
extern sh_int gsn_caltraps;
extern sh_int gsn_salve;
extern sh_int gsn_dislodge;

/* UBit */

#define STR_ZERO_STR(var, size)  { int i; for(i=0;i<(size);i++){(var)[i]=0;} }
#define STR_SAME_STR(a, b, size) (memcmp((a),(b),(size))==0)
void STR_COPY_STR args(( FLAG * a, const FLAG * b, const int size) );
bool STR_IS_ZERO   args(( FLAG *var, int size ));
bool STR_AND       args(( FLAG *var1, FLAG *var2, int size));

#define STR_AND_STR(a,b, size) \
	{ int i;for (i=0;i<(size);i++){(a)[i]&=(b)[i];}}
#define STR_OR_STR(a,b, size) \
	{ int i;for (i=0;i<(size);i++){(a)[i]|=(b)[i];}}
#define STR_XOR_STR(a,b, size) \
	{ int i;for (i=0;i<(size);i++){(a)[i]^=(b)[i];}}
#define STR_NOT_STR(a,b, size) \
	{ int i;for (i=0;i<(size);i++){(a)[i]=~(b)[i];}}
#define STR_REMOVE_STR(a,b, size) \
	{ int i;for (i=0;i<(size);i++){(a)[i]&=~(b)[i];}}

#define STR_IS_SET(var, bit) \
	((((FLAG *)(var))[((bit)/FLAG_BITSIZE)]) &   ((1<<((bit)%FLAG_BITSIZE))))
#define STR_SET_BIT(var, bit) \
	((((FLAG *)(var))[((bit)/FLAG_BITSIZE)]) |=   ((1<<((bit)%FLAG_BITSIZE))))
#define STR_REMOVE_BIT(var, bit) \
	((((FLAG *)(var))[((bit)/FLAG_BITSIZE)]) &=   ~((1<<((bit)%FLAG_BITSIZE))))
#define STR_TOGGLE_BIT(var, bit) \
	((((FLAG *)(var))[((bit)/FLAG_BITSIZE)]) ^=   ((1<<((bit)%FLAG_BITSIZE))))

/*
 * Utility macros.
 */
#define IS_VALID(data)        ((data) != NULL && (data)->valid)
#define VALIDATE(data)        ((data)->valid = TRUE)
#define INVALIDATE(data)    ((data)->valid = FALSE)
#define UMIN(a, b)        ((a) < (b) ? (a) : (b))
#define UMAX(a, b)        ((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)        ((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)        ((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)        ((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)    ((flag) & (bit))
#define SET_BIT(var, bit)    ((var) |= (bit))
#define REMOVE_BIT(var, bit)    ((var) &= ~(bit))
#define IS_NULLSTR(str)        ((str) == NULL || (str)[0] == '\0')
#define ENTRE(min,num,max)    ( ((min) < (num)) && ((num) < (max)) )
#define CHECK_POS(a, b, c)    {                            \
                    (a) = (b);                    \
                    if ( (a) < 0 )                    \
                    bug( "CHECK_POS : " c " == %d < 0", a );    \
                }                            \

/*
 * Character macros.
 */

#define SHOW_DF(ch, victim)   ( victim->pcdata->isMinion && (ch->level == MAX_LEVEL || IS_FORSAKEN(ch) \
                               || ch == victim) )
#define IS_MINION(ch) (!IS_NPC(ch) && ch->pcdata->isMinion)

#define SEE_TAG(ch, wch) ( IS_IMMORTAL(ch) || ch->clan == clan_lookup("Guide") || wch->clan == clan_lookup("Guide") || wch->clan == clan_lookup("furies") )
#define IS_NPC(ch)        (IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)        (get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_HIIMMORTAL(ch)        (get_trust(ch) >= (LEVEL_IMMORTAL + 2))
#define IS_HERO(ch)        (get_trust(ch) >= LEVEL_HERO)
#define IS_TRUSTED(ch,level)    (get_trust((ch)) >= (level))
#define IS_AFFECTED(ch, sn)    (STR_IS_SET((ch)->affected_by, (sn)))
#define IS_AFFECTED2(ch, sn)    (IS_SET((ch)->affected_by2, (sn)))
#define IS_SIMAFF(ch, sn)     (IS_SET((ch)->simaff, (sn)))
#define IS_TINKER(ch)      ch->clan == clan_lookup("Tinker")
#define GET_AGE(ch)        ((int) (17 + ((ch)->played \
                    + current_time - (ch)->logon )/72000))

#define IS_GOOD(ch)        (ch->alignment >= 350)
#define IS_EVIL(ch)        (ch->alignment <= -350)
#define IS_NEUTRAL(ch)        (!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)        (ch->position > POS_SLEEPING)
#define GET_AC(ch,type)        ((ch)->armor[type]                \
                + ( IS_AWAKE(ch)                \
            ? dex_app[get_curr_stat(ch,STAT_DEX)].defensive : 0 ))
#define GET_HITROLL(ch)    \
        ((ch)->hitroll+str_app[get_curr_stat(ch,STAT_STR)].tohit)
#define GET_DAMROLL(ch) \
        ((ch)->damroll+str_app[get_curr_stat(ch,STAT_STR)].todam)

#define IS_OUTSIDE(ch)        (!IS_SET(                    \
                    (ch)->in_room->room_flags,            \
                    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)    ((ch)->wait = UMAX((ch)->wait, (npulse)))
#define DAZE_STATE(ch, npulse)  ((ch)->daze = UMAX((ch)->daze, (npulse)))
#define get_carry_weight(ch)    ((ch)->carry_weight + (ch)->silver / 200 \
				+ (ch)->gold / 100)

#define act(format,ch,arg1,arg2,type)\
    act_new((format),(ch),(arg1),(arg2),(type),POS_RESTING)

#define HAS_TRIGGER_MOB(ch,trig)    (IS_SET((ch)->pIndexData->mprog_flags,(trig)))
#define HAS_TRIGGER_OBJ(obj,trig) (IS_SET((obj)->pIndexData->oprog_flags,(trig)))
#define HAS_TRIGGER_ROOM(room,trig) (IS_SET((room)->rprog_flags,(trig)))
#define IS_SWITCHED( ch )       ( ch->desc && ch->desc->original )
#define IS_BUILDER(ch, Area)    ( !IS_NPC(ch) && !IS_SWITCHED( ch ) &&      \
                ( ch->pcdata->security >= Area->security  \
                || strstr( Area->builders, ch->name )      \
                || strstr( Area->builders, "All" ) ) )

/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)    (IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)    (IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)    ((obj)->item_type == ITEM_CONTAINER ? \
    (obj)->value[4] : 100)



/*
 * Description macros.
 */

/*  Replaced by PERS function in act_info */
/*
#define PERS(ch, looker)    ( can_see( looker, (ch) ) ?        \
                ( IS_NPC(ch) ? (ch)->short_descr    \
                : (ch)->name ) : "someone" )
*/


/*
 * Structure for a social in the socials table.
 */
struct    social_type
{
    char      name[20];
    char *    char_no_arg;
    char *    others_no_arg;
    char *    char_found;
    char *    others_found;
    char *    vict_found;
    char *    char_not_found;
    char *      char_auto;
    char *      others_auto;
};

struct    war_time
{
   bool on;
   bool off;
};
struct    bonus_type
{
   bool on;
   bool negative;
   int time_left;
   sh_int multiplier;
};

/*
 * Global constants.
 */


extern    const    struct    stance_type     stance_table     [];
extern    const    struct    str_app_type    str_app        [26];
extern    const    struct    int_app_type    int_app        [26];
extern    const    struct    wis_app_type    wis_app        [26];
extern    const    struct    dex_app_type    dex_app        [26];
extern    const    struct    con_app_type    con_app        [27];

extern    const    struct    class_type    class_table    [MAX_CLASS];
extern    const    struct    weapon_type    weapon_table    [];
extern  const   struct  item_type    item_table    [];
extern  const   struct  explosive_type    explosive_table    [];
extern    const    struct    wiznet_type    wiznet_table    [];
extern    const    struct    attack_type    attack_table    [];
extern    const    struct    mar_attack_type    mar_attack_table    [];
extern    const    struct    throw_attack_type    throw_attack_table    [];
extern  const    struct  race_type    race_table    [];
extern    const    struct    pc_race_type    pc_race_table    [];
extern  const    struct    spec_type    spec_table    [];
extern    const    struct    liq_type    liq_table    [];
extern    const    struct    skill_type    skill_table    [MAX_SKILL];
extern  const   struct  group_type      group_table    [MAX_GROUP];
extern          struct social_type      social_table    [MAX_SOCIALS];
extern    char *    const            title_table    [MAX_CLASS]
                            [MAX_LEVEL+1]
                            [2];



/*
 * Global variables.
 */

#ifdef DNS_SLAVE
  extern pid_t slave_pid;
  extern int slave_socket;
#endif

extern        FORTUNE_DATA        fortune;
extern        QUOTESTER_DATA      quotester;
extern        int                 slotland;
extern        HELP_DATA      *    help_first;
extern        SHOP_DATA      *    shop_first;

extern        char           *    illegal_names[MAX_ILLEGAL_NAMES];
extern        int                 top_illegal;

extern        CHAR_DATA      *    char_list;
extern        DESCRIPTOR_DATA   *    descriptor_list;
extern        OBJ_DATA      *    object_list;
extern        QUEST_INFO_DATA *  quest_info_list;

extern        DELAY_CODE     *    delay_list;
extern        PROG_CODE      *    mprog_list;
extern        PROG_CODE      *    oprog_list;
extern        PROG_CODE      *    rprog_list;

extern        char            bug_buf        [];
extern        time_t            current_time;
extern        time_t            boot_time;
extern        bool            fLogAll;
extern        FILE *            fpReserve;
extern        char            str_boot_time[MIL];
extern        KILL_DATA        kill_table    [];
extern        char            log_buf        [];
extern        TIME_INFO_DATA        time_info;
extern        WEATHER_DATA        weather_info;
extern        bool            MOBtrigger;
extern        bool             forceTick;
extern        bool             iscopyover;
extern        bool             iswar;
extern        int              copytimer;
extern        int              wartimer;
extern        int              armorcounter;
extern        int              shieldcounter;
extern        int              blesscounter;
extern        int              sanccounter;
extern        int              levelcounter;
extern        int              logincounter;
extern        int              pkcounter;
extern        int              mkcounter;
extern        int              restorecounter;
extern        int              bonuscounter;
extern        int              questcounter;
extern        int                     quest_list[MAX_LIST];
extern        CHAR_DATA               *copych;
extern        CHAR_DATA               *warch;
extern        struct bonus_type       xp_bonus;
extern        struct war_time         war;
extern        VOTE_DATA               voteinfo;
extern        MUDSTATS_DATA           mudstats;
extern        RIDDLE_DATA           * riddle_list;
extern        ACCOUNT_DATA	    * account_list;
extern        int		      nAllocString;
extern        int                     nAllocPerm;
// extern        ROSTER_DATA             roster[MAX_CLAN];
//extern        MARK_DATA               *mark_list;
extern        long            equip_id;
extern        TOURNAMENT_DATA         tournament;
extern        STORE_DATA      * store_list;
extern        bool                    checkStall;
extern        float                   stall_percent;
extern        int                     top_clan;
extern        CLAN_DATA               clan_table[MAX_CLAN];
extern        int                     top_qprize;
extern        QPRIZE_DATA             qprize_table[MAX_PRIZE];
extern        char                    last_command[MSL];
extern        struct timeval          now_time;

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if    defined(_AIX)
extern "C" {char *  crypt           args( ( const char *key, const char *salt ) );};
#endif

#if    defined(apollo)
int    atoi        args( ( const char *string ) );
void *    calloc        args( ( unsigned nelem, size_t size ) );
extern "C" {char *  crypt           args( ( const char *key, const char *salt ) );};
#endif

#if    defined(hpux)
extern "C" {char *  crypt           args( ( const char *key, const char *salt ) );};
#endif

#if    defined(linux)
extern "C" {char *  crypt           args( ( const char *key, const char *salt ) );};
#endif

#if    defined(macintosh)
#define NOCRYPT
#if    defined(unix)
#undef    unix
#endif
#endif

#if    defined(MIPS_OS)
extern "C" {char *  crypt           args( ( const char *key, const char *salt ) );};
#endif

#if    defined(MSDOS)
#define NOCRYPT
#if    defined(unix)
#undef    unix
#endif
#endif

#if    defined(NeXT)
extern "C" {char *  crypt           args( ( const char *key, const char *salt ) );};
#endif

#if    defined(sequent)
extern "C" {char *  crypt           args( ( const char *key, const char *salt ) );};
int    fclose        args( ( FILE *stream ) );
int    fprintf        args( ( FILE *stream, const char *format, ... ) );
int    fread        args( ( void *ptr, int size, int n, FILE *stream ) );
int    fseek        args( ( FILE *stream, long offset, int ptrname ) );
void    perror        args( ( const char *s ) );
int    ungetc        args( ( int c, FILE *stream ) );
#endif

#if    defined(sun)
extern "C" {char *  crypt           args( ( const char *key, const char *salt ) );};
int    fclose        args( ( FILE *stream ) );
int    fprintf        args( ( FILE *stream, const char *format, ... ) );
#if    defined(SYSV)
siz_t    fread        args( ( void *ptr, size_t size, size_t n,
                FILE *stream) );
#elif !defined(__SVR4)
int    fread        args( ( void *ptr, int size, int n, FILE *stream ) );
#endif
int    fseek        args( ( FILE *stream, long offset, int ptrname ) );
void    perror        args( ( const char *s ) );
int    ungetc        args( ( int c, FILE *stream ) );
#endif

#if    defined(ultrix)
extern "C" {char *  crypt           args( ( const char *key, const char *salt ) );};
#endif



/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if    defined(NOCRYPT)
#define crypt(s1, s2)    (s1)
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
#define PLAYER_DIR    ""            /* Player files    */
#define TEMP_FILE    "romtmp"
#define NULL_FILE    "proto.are"        /* To reserve one stream */
#endif

#if defined(MSDOS)
#define PLAYER_DIR    ""            /* Player files */
#define TEMP_FILE    "romtmp"
#define NULL_FILE    "nul"            /* To reserve one stream */
#endif

#if defined(unix)
#define PLAYER_DIR      "../player/"            /* Player files */
#define ACCOUNT_DIR     "../player/accounts/"       /* Account Files */
#define GOD_DIR         "../gods/"          /* list of gods */
#define LOG_RP_DIR      "../../public_html/rp_logs/"
#define LOG_PK_DIR      "../../public_html/pk_logs/"
#define LOG_IMM_DIR     "../../public_html/imm_logs/"
#define DATA_DIR        "../data/"
#define ROSTER_DIR      "../data/roster/"
#define TEMP_FILE       "../player/romtmp"
#define NULL_FILE       "/dev/null"        /* To reserve one stream */
#endif

#define LOG_RP          "rp.log"
#define LOG_PK          "pk.log"
#define LOG_IMM         "imm.log"

#define AREA_LIST       "area.lst"  /* List of areas*/
#define BUG_FILE        "bugs.txt" /* For 'bug' and bug()*/
#define TYPO_FILE       "typos.txt" /* For 'typo'*/
#define NOTE_FILE       "notes.not"/* For 'notes'*/
#define MPROG_FILE "/home/aod/SoT/public_html/data/mprog.html"
#define HELP_FILE  "/home/aod/SoT/public_html/data/help.txt"

/* For undefined helps */
#define IDEA_FILE       "ideas.not"
#define GUILDMASTER_FILE    "guildmaster.not"
#define NEWS_FILE       "news.not"
#define CHANGES_FILE    "chang.not"
#define IMM_FILE        "imm.not"
#define RP_FILE         "rp.not"
#define GUIDE_FILE         "guide.not"
#define STORENOTE_FILE     "store.not"
#define SHUTDOWN_FILE   "shutdown.txt"/* For 'shutdown'*/
#define BAN_FILE    "ban.txt"
#define MUSIC_FILE    "music.txt"
#define OHELPS_FILE	"orphaned_helps.txt"	/* Unmet 'help' requests */
#define RIDDLE_FILE     "../data/riddles.txt"
#define GUILDS_FILE     "guilds.txt"
#define LAST_COMM_FILE  "../data/lastcomm.txt"
#define TO_DO_FILE      "../data/to_do.txt"
#define TO_CODE_FILE    "../data/to_code.txt"
#define AGENDA_FILE     "../data/agenda.txt"
#define CHANGED_FILE    "../data/changed.txt"


/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD    CHAR_DATA
#define MID    MOB_INDEX_DATA
#define OD    OBJ_DATA
#define OID    OBJ_INDEX_DATA
#define RID    ROOM_INDEX_DATA
#define SF    SPEC_FUN
#define AD    AFFECT_DATA
#define PC    PROG_CODE

#define MD    MEM_DATA

/* act_comm.c */
void      check_sex    args( ( CHAR_DATA *ch) );
void    add_follower    args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void    stop_follower    args( ( CHAR_DATA *ch ) );
void     nuke_pets    args( ( CHAR_DATA *ch ) );
void    die_follower    args( ( CHAR_DATA *ch ) );
void    die_mount    args( ( CHAR_DATA *ch ) );
bool    is_same_group    args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void     logf             args((const char * fmt, ...));
void     mplogf             args((const char * fmt, ...));
char    *dns_gethostname     args(( char *s));

/* act_enter.c */
RID  *get_random_room   args ( (CHAR_DATA *ch) );

/* act_info.c */
void    set_title    args( ( CHAR_DATA *ch, char *title ) );
char    *PERS        args( ( CHAR_DATA *ch, CHAR_DATA *looker, bool ooc ) );
char    *GET_CHAR_PERS        args( ( CHAR_DATA *ch, CHAR_DATA *looker, bool ooc ) );
void    port_exits   args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *to_room ));
void    addKnow      args( ( CHAR_DATA *ch, long id, char *name ) );

/* act_move.c */
void    move_char    args( ( CHAR_DATA *ch, int door, bool follow, bool ignoreCharm ) );

/* act_obj.c */
bool can_loot        args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void    wear_obj    args( (CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace) );
void    get_obj         args( ( CHAR_DATA *ch, OBJ_DATA *obj,
                            OBJ_DATA *container, bool getAll ) );

/* act_wiz.c */
void wiznet        args( (char *string, CHAR_DATA *ch, OBJ_DATA *obj,
                   long flag, long flag_skip, int min_level ) );
void copyover_recover args((void));
void echo args (( char *argument ));

/* alias.c */
void     substitute_alias args( (DESCRIPTOR_DATA *d, char *input) );

/* ban.c */
bool    check_ban    args( ( char *site, int type) );

/* bitsys.c */
FLAG * str_fread_flag  args( ( FILE *fp, const int size ) );
FLAG * aff_convert_fread_flag  args(( long bits ));
int    aff_bit_convert_fread_flag  args(( long bits ));
char *str_print_flags args(( FLAG * flags, const int size )) ;
char *str_fwrite_flag args(( FLAG *flags, const int size, char *out ));
char * affect_str_bit_name  args( ( FLAG *vector ) );

/* bank.c */
void	find_money	args( ( CHAR_DATA *ch ) );

/* comm.c */
void chan_glob_act (const char *, CHAR_DATA *, const void *,const void *,
			int, long);
void    show_string    args( ( struct descriptor_data *d, char *input) );
void    close_socket    args( ( DESCRIPTOR_DATA *dclose ) );
void    write_to_buffer    args( ( DESCRIPTOR_DATA *d, const char *txt,
                int length ) );
void    send_to_desc    args( ( const char *txt, DESCRIPTOR_DATA *d ) );
void    send_to_char    args( ( const char *txt, CHAR_DATA *ch ) );
void    page_to_char    args( ( const char *txt, CHAR_DATA *ch ) );
void    act        args( ( const char *format, CHAR_DATA *ch,
                const void *arg1, const void *arg2, int type ) );
void    act_new        args( ( const char *format, CHAR_DATA *ch,
                const void *arg1, const void *arg2, int type,
                int min_pos) );
void    printf_to_char    args( ( CHAR_DATA *, char *, ... ) );
void    bugf        args( ( char *, ... ) );
bool    write_to_descriptor args ( ( DESCRIPTOR_DATA *d, char *txt, int length ) );
void    echoall     args( ( char * argument ) );

/* db.c */
void free_space args ((void));
void log_special args ((const char *str, sh_int type));
void    reset_area      args( ( AREA_DATA * pArea ) );        /* OLC */
void    reset_room    args( ( ROOM_INDEX_DATA *pRoom ) );    /* OLC */
char *    print_flags    args( ( int flag ));
void    boot_db        args( ( void ) );
void    area_update    args( ( void ) );
CD *    create_mobile    args( ( MOB_INDEX_DATA *pMobIndex ) );
void    clone_mobile    args( ( CHAR_DATA *parent, CHAR_DATA *clone) );
OD *    create_object    args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void    clone_object    args( ( OBJ_DATA *parent, OBJ_DATA *clone ) );
void    clear_char    args( ( CHAR_DATA *ch ) );
char *    get_extra_descr    args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *    get_mob_index    args( ( long vnum ) );
OID *    get_obj_index    args( ( long vnum ) );
RIDDLE_DATA * get_riddle_index  args(( int vnum ));
RID *    get_room_index    args( ( long vnum ) );
PC *    get_prog_index args( ( long vnum, int type ) );
char    fread_letter    args( ( FILE *fp ) );
int    fread_number    args( ( FILE *fp ) );
long   fread_long      args(( FILE *fp ));
long     fread_flag    args( ( FILE *fp ) );
char *    fread_string    args( ( FILE *fp ) );
char *  fread_string_eol args(( FILE *fp ) );
void    fread_to_eol    args( ( FILE *fp ) );
char *    fread_word    args( ( FILE *fp ) );
long    flag_convert    args( ( char letter) );
void *    alloc_mem    args( ( size_t sMem ) );
void *    alloc_perm    args( ( size_t sMem ) );
void    free_mem    args( ( void *pMem, size_t sMem ) );
char *    str_dup        args( ( const char *str ) );
void    free_string    args( ( char *pstr ) );
int    number_fuzzy    args( ( int number ) );
int    number_range    args( ( int from, int to ) );
int    number_percent    args( ( void ) );
int    number_door    args( ( void ) );
int    number_bits    args( ( int width ) );
long     number_mm       args( ( void ) );

int    dice        args( ( int number, int size ) );
int    interpolate    args( ( int level, int value_00, int value_32 ) );
void    smash_tilde    args( ( char *str ) );
bool    str_cmp        args( ( const char *astr, const char *bstr ) );
bool    str_prefix    args( ( const char *astr, const char *bstr ) );
bool    str_infix    args( ( const char *astr, const char *bstr ) );
bool    str_suffix    args( ( const char *astr, const char *bstr ) );
char *    chomp    args(( char *string));
char *    str_remove    args(( char *string, char *remove));
char *    capitalize    args( ( const char *str ) );
void    append_file    args( ( CHAR_DATA *ch, char *file, char *str ) );
void    bug        args( ( const char *str, int param ) );
void    log_string    args( ( const char *str ) );
void    tail_chain    args( ( void ) );
void    save_equipid  args(( void));
void    load_equipid  args ((void));
void    do_print_mobiles args ((CHAR_DATA *ch, char *argument));
void init_top_ten args((void));

 /*
  * Colour stuff by Lope
  */
 int   colour          args( ( char type, CHAR_DATA *ch, char *string ) );
 void  colourconv      args( ( char *buffer, const char *txt, CHAR_DATA*ch));
 void  colourstrip     args( ( char *buffer, const char *txt ));
 void  send_to_char_bw args( ( const char *txt, CHAR_DATA *ch ) );
 void  page_to_char_bw args( ( const char *txt, CHAR_DATA *ch ) );


/* effect.c */
void    acid_effect    args( (void *vo, int level, int dam, int target) );
void    cold_effect    args( (void *vo, int level, int dam, int target) );
void    fire_effect    args( (void *vo, int level, int dam, int target) );
void    poison_effect    args( (void *vo, int level, int dam, int target) );
void    shock_effect    args( (void *vo, int level, int dam, int target) );


/* fight.c */
bool    IS_INPKRANGE    args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void    auto_guidenote  args( (CHAR_DATA *ch, CHAR_DATA *victim, int type) );
bool     is_safe        args( (CHAR_DATA *ch, CHAR_DATA *victim ) );
bool     is_safe_spell    args( (CHAR_DATA *ch, CHAR_DATA *victim, bool area ) );
void    violence_update    args( ( void ) );
void    multi_hit    args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
int    damage        args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                    int dt, int cClass, bool show, bool arrow ) );
bool    damage_old      args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                                int dt, int cClass, bool show ) );
void    update_pos    args( ( CHAR_DATA *victim ) );
void    stop_fighting    args( ( CHAR_DATA *ch, bool fBoth ) );
void    check_killer    args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
bool    obj_damage        args( ( OBJ_DATA *obj, CHAR_DATA *victim, int dam,
                    int dt, int cClass, bool show ) );



/* guild_skills.c */
void    dream_to        args( ( CHAR_DATA *ch ) );
void    save_roster     args( ( void ) );
void    load_roster     args( ( void ) );
bool    remove_member   args( (char *name, int clan) );
void    add_member      args( (char *name, int rank, int clan) );
bool    change_member   args( (char *name, int rank, int clan) );
bool    IS_BONDED       args(( CHAR_DATA *ch) );
void    dome            args (( CHAR_DATA *ch, CHAR_DATA *victim ));
bool    furies_imm      args ((CHAR_DATA *ch, CHAR_DATA *victim ));

/* handler.c */
int    count_mobarea args (( int mvnum, AREA_DATA *area));
AD      *affect_find args( (AFFECT_DATA *paf, int sn));
void    affect_check    args( (CHAR_DATA *ch, int where, int vector) );
bool   is_obj_outlevel args((CHAR_DATA * ch,OBJ_DATA * obj));
bool   is_obj_classrestrict args((CHAR_DATA * ch,OBJ_DATA * obj));
int    count_users    args( (OBJ_DATA *obj) );
int    count_people   args( (CHAR_DATA *ch) );
void     deduct_cost    args( (CHAR_DATA *ch, int cost) );
void    affect_enchant    args( (OBJ_DATA *obj) );
int     check_immune    args( (CHAR_DATA *ch, int dam_type) );
int     material_lookup args( ( const char *name) );
int    weapon_lookup    args( ( const char *name) );
int    weapon_type    args( ( const char *name) );
char     *weapon_name    args( ( int weapon_Type) );
char    *item_name    args( ( int item_type) );
char    *explosive_name    args( ( int explosive_type) );
int    attack_lookup    args( ( const char *name) );
long    wiznet_lookup    args( ( const char *name) );
int    class_lookup    args( ( const char *name) );
bool    is_clan        args( (CHAR_DATA *ch) );
bool    is_same_clan    args( (CHAR_DATA *ch, CHAR_DATA *victim));
bool    is_old_mob    args ( (CHAR_DATA *ch) );
int    get_skill    args( ( CHAR_DATA *ch, int sn ) );
int    get_weapon_sn    args( ( CHAR_DATA *ch, bool secondary ) );
int    get_weapon_skill args(( CHAR_DATA *ch, int sn ) );
int     get_age         args( ( CHAR_DATA *ch ) );
void    reset_char    args( ( CHAR_DATA *ch )  );
int    get_trust    args( ( CHAR_DATA *ch ) );
int    get_curr_stat    args( ( CHAR_DATA *ch, int stat ) );
int     get_weave_stat  args (( CHAR_DATA *ch, int stat ));
int     get_max_train    args( ( CHAR_DATA *ch, int stat ) );
int    can_carry_n    args( ( CHAR_DATA *ch ) );
int    can_carry_w    args( ( CHAR_DATA *ch ) );
bool    is_name        args( ( char *str, char *namelist ) );
bool    is_exact_name    args( ( char *str, char *namelist ) );
void    affect_to_char    args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_to_obj    args( ( OBJ_DATA *obj, AFFECT_DATA *paf ) );
void    affect_remove    args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_remove_obj args( (OBJ_DATA *obj, AFFECT_DATA *paf ) );
void    affect_strip    args( ( CHAR_DATA *ch, int sn ) );
void    obj_affect_strip    args( ( CHAR_DATA *ch, int affect ) );
bool    is_affected    args( ( CHAR_DATA *ch, int sn ) );
void    affect_join    args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    char_from_room    args( ( CHAR_DATA *ch ) );
void    char_to_room    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_char    args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    obj_from_char    args( ( OBJ_DATA *obj ) );
int    apply_ac    args( ( OBJ_DATA *obj, int iWear, int type ) );
OD *    get_eq_char    args( ( CHAR_DATA *ch, int iWear ) );
void    equip_char    args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void    unequip_char    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int    count_obj_list    args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
int     char_has_carry   args ( ( CHAR_DATA *ch, OBJ_DATA *suspect ) );
void    obj_from_room    args( ( OBJ_DATA *obj ) );
void    obj_to_room    args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_obj    args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void    obj_from_obj    args( ( OBJ_DATA *obj ) );
void    extract_obj    args( ( OBJ_DATA *obj ) );
void    extract_char    args( ( CHAR_DATA *ch, bool fPull ) );
bool    get_char_exists args (( CHAR_DATA *ch ));
CD *    get_char_room    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument ) );
CD *    get_char_world_special args ((char *argument));
CD *    get_char_id      args( (int id) );
CD *    get_char_world_vnum args(( CHAR_DATA *ch, int vnum ));

CD *    get_char_area    args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_world    args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_world_ooc    args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_type    args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *    get_obj_list    args( ( CHAR_DATA *ch, char *argument,
                OBJ_DATA *list ) );
OD *    get_obj_list_full    args( ( CHAR_DATA *ch, char *argument,
                OBJ_DATA *list ) );
OD *    get_obj_carry    args( ( CHAR_DATA *ch, char *argument,
                CHAR_DATA *viewer ) );
OD *    get_obj_wear    args( ( CHAR_DATA *ch, char *argument, bool character ) );
OD *    get_obj_here    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument ) );
OD *    get_obj_world    args( ( CHAR_DATA *ch, char *argument ) );
OD *    create_money    args( ( long gold, long silver ) );
int    get_obj_number    args( ( OBJ_DATA *obj ) );
int    get_obj_weight    args( ( OBJ_DATA *obj ) );
int    get_true_weight    args( ( OBJ_DATA *obj ) );
bool    room_is_dark    args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    is_room_owner    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
bool    room_is_private    args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    can_see        args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    can_see_ooc    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    can_see_obj    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool    can_see_room    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) );
bool    can_drop_obj    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool    room_is_wall    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) );
bool    room_is_gate    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) );
bool    room_is_tower    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) );

char *    affect_loc_name    args( ( int location ) );
char *    affect_bit_name    args( ( long vector ) );
char *    new_affect_bit_name    args( ( long vector ) );
char *    affect2_bit_name    args( ( int vector ) );
char *    extra_bit_name    args( ( int extra_flags ) );
char *     wear_bit_name    args( ( int wear_flags ) );
char *    act_bit_name    args( ( int act_flags ) );
char *    class_bit_name    args( ( int class_flags ) );
char *    act2_bit_name    args( ( int act2_flags ) );
char *    off_bit_name    args( ( int off_flags ) );
char *  imm_bit_name    args( ( int imm_flags ) );
char *     form_bit_name    args( ( int form_flags ) );
char *    part_bit_name    args( ( int part_flags ) );
char *    weapon_bit_name    args( ( int weapon_flags ) );
char *  comm_bit_name    args( ( int comm_flags ) );
char *    cont_bit_name    args( ( int cont_flags) );
int       colorstrlen      args( ( const char *argument ) );
void      setbit           args(( char *explored, int index ));
int      getbit           args(( char *explored, int index ));
int roomcount             args(( CHAR_DATA *ch ));
int mobcount             args(( CHAR_DATA *ch ));
int areacount             args(( CHAR_DATA *ch ));


/*
 * Colour Config
 */
void    default_colour    args( ( CHAR_DATA *ch ) );
void    all_colour    args( ( CHAR_DATA *ch, char *argument ) );

/* interp.c */
void    interpret    args( ( CHAR_DATA *ch, char *argument ) );
bool    is_number    args( ( char *arg ) );
int    number_argument    args( ( char *argument, char *arg ) );
int    mult_argument    args( ( char *argument, char *arg) );
char *    one_argument    args( ( char *argument, char *arg_first ) );
char *    one_argument_special    args( ( char *argument, char *arg_first ) );

/* ispell.c */
void   ispell_init   args((void));
void   ispell_done   args((void));

/* magic.c */
int    find_spell    args( ( CHAR_DATA *ch, const char *name) );
int     mana_cost     (CHAR_DATA *ch, int min_mana, int level);
int    skill_lookup    args( ( const char *name ) );
int    slot_lookup    args( ( int slot ) );
bool    saves_spell    args( ( int level, CHAR_DATA *victim, int dam_type ) );
int    obj_cast_spell    args( ( int sn, int level, CHAR_DATA *ch,
                    CHAR_DATA *victim, OBJ_DATA *obj ) );

/* mob_prog.c */
void add_delay_prog args ((
        sh_int pvnum,
        char *source,
        CHAR_DATA *mob,
        OBJ_DATA *obj,
        ROOM_INDEX_DATA *room,
        CHAR_DATA *ch, const void *arg1, const void *arg2, int gotoLine,
        int iDelay ));


void    program_flow    args( ( sh_int vnum, char *source, CHAR_DATA *mob,
                                OBJ_DATA *obj, ROOM_INDEX_DATA *room,
                                CHAR_DATA *ch, const void *arg1,
                                const void *arg2, int gotoLine ) );
void    p_act_trigger   args( ( char *argument, CHAR_DATA *mob,
                                OBJ_DATA *obj, ROOM_INDEX_DATA *room,
                                CHAR_DATA *ch, const void *arg1,
                                const void *arg2, int type ) );
bool    p_percent_trigger args( ( CHAR_DATA *mob, OBJ_DATA *obj,
                                ROOM_INDEX_DATA *room, CHAR_DATA *ch,
                                const void *arg1, const void *arg2, int type ) );
void    p_bribe_trigger  args( ( CHAR_DATA *mob, CHAR_DATA *ch, int amount ) );
bool    p_exit_trigger   args( ( CHAR_DATA *ch, int dir, int type ) );
void    p_give_trigger   args( ( CHAR_DATA *mob, OBJ_DATA *obj,
                                ROOM_INDEX_DATA *room, CHAR_DATA *ch,
                                OBJ_DATA *dropped, int type ) );
void    p_greet_trigger  args( ( CHAR_DATA *ch, int type ) );
void    p_hprct_trigger  args( ( CHAR_DATA *mob, CHAR_DATA *ch ) );

/* mob_cmds.c */
int    mob_interpret    args( ( CHAR_DATA *ch, char *argument ) );
int    obj_interpret   args( ( OBJ_DATA *obj, char *argument ) );
int    room_interpret     args( ( ROOM_INDEX_DATA *room, char *argument ) );


/* Mount.c */
#define IS_MOUNTED(ch)     (ch->is_mounted)
/* Note.c */
void system_note args (( char *sender,int type,char *to, char *subject, char *txt ));
/* quest.c */

void abort_quest         args(( CHAR_DATA *ch ));

/* Roster.c */
void load_guilds args (( void ));
void save_guilds args (( void ));

/* save.c */
void    save_char_obj    args( ( CHAR_DATA *ch ) );
bool    load_char_obj    args( ( DESCRIPTOR_DATA *d, char *name ) );

/* skills.c */
bool     parse_gen_groups args( ( CHAR_DATA *ch,char *argument ) );
void     list_group_costs args( ( CHAR_DATA *ch ) );
void    list_group_known args( ( CHAR_DATA *ch ) );
int     exp_per_level    args( ( CHAR_DATA *ch, int points ) );
void     check_improve    args( ( CHAR_DATA *ch, int sn, bool success,
                    int multiplier ) );
int     group_lookup    args( (const char *name) );
void    gn_add        args( ( CHAR_DATA *ch, int gn) );
void     gn_remove    args( ( CHAR_DATA *ch, int gn) );
void     group_add    args( ( CHAR_DATA *ch, const char *name, bool deduct) );
void    group_remove    args( ( CHAR_DATA *ch, const char *name) );

/* special.c */
SF *    spec_lookup    args( ( const char *name ) );
char *    spec_name    args( ( SPEC_FUN *function ) );

/* store.c */
void store_do_buy   args(( CHAR_DATA *ch, char *argument ));
void store_do_list  args(( CHAR_DATA *ch, char *argument ));
bool run_store_editor args (( DESCRIPTOR_DATA *d ));

/* riddle.c */
bool run_riddle_editor args (( DESCRIPTOR_DATA *d ));
/* teleport.c */
RID *    room_by_name    args( ( char *target, int level, bool error) );

/* update.c */
void    advance_level    args( ( CHAR_DATA *ch, bool advance, bool hide ) );
void    gain_exp    args( ( CHAR_DATA *ch, int gain ) );
void    gain_condition    args( ( CHAR_DATA *ch, int iCond, int value ) );
void    rpadvance_level    args( ( CHAR_DATA *ch, bool advance,bool hide ) );
void    rpgain_exp    args( ( CHAR_DATA *ch, int gain ) );
void    update_handler    args( ( void ) );
void    mobhunt_update    args((void));
void    bleed_char        args( (CHAR_DATA *ch, bool update) );
void    SET_BLEED         args( (CHAR_DATA *ch, bool bleed) );

/* string.c */
void    string_edit    args( ( CHAR_DATA *ch, char **pString ) );
void    string_append   args( ( CHAR_DATA *ch, char **pString ) );
char *    string_replace    args( ( char * orig, char *old, char *neww ));
void    string_add      args( ( CHAR_DATA *ch, char *argument ) );
char *  format_string   args( ( char *oldstring /*, bool fSpace */ ) );
char *  first_arg       args( ( char *argument, char *arg_first, bool fCase ) );
char *    string_unpad    args( ( char * argument ) );
char *    string_proper    args( ( char * argument ) );

/* olc.c */
bool    run_olc_editor    args( ( DESCRIPTOR_DATA *d ) );
char    *olc_ed_name    args( ( CHAR_DATA *ch ) );
char    *olc_ed_vnum    args( ( CHAR_DATA *ch ) );
void    save_area       args( ( AREA_DATA * pArea ) );

/* lookup.c */
int    race_lookup    args( ( const char *name) );
int    item_lookup    args( ( const char *name) );
int    explosive_lookup    args( ( const char *name) );
int    liq_lookup    args( ( const char *name) );
bool   IS_FORSAKEN    args(( CHAR_DATA *ch) );
bool   IS_DRAGON    args(( CHAR_DATA *ch) );
bool   IS_SPELL_AFFECT  args ((const int affect));
bool   is_guild     args((CHAR_DATA *ch, char *argument));
bool   check_player args((char *name));
int    max_natural_hp args((CHAR_DATA *ch));
int    max_natural_mana args((CHAR_DATA *ch));

/* hunt.c */

void hunt_victim  args ((CHAR_DATA *ch));
void mob_remember args ((CHAR_DATA *ch, CHAR_DATA *target, int reaction));
MD * get_mem_data args ((CHAR_DATA *ch, CHAR_DATA *target));
void mem_fade args ((CHAR_DATA *ch));
void mem_clear args ((CHAR_DATA *ch));
void mob_forget args (( CHAR_DATA *ch, MEM_DATA *memory ));
void remember_victim args ((CHAR_DATA *ch, CHAR_DATA *victim));

/* disguise.c */
bool IS_DISGUISED args ((CHAR_DATA * ch));
void REM_DISGUISE args ((CHAR_DATA * ch));

/* mark.c */
//bool run_mark_editor args ((DESCRIPTOR_DATA *d));

/* websvr.h */
void init_web(int port);
void handle_web(void);
void shutdown_web(void);

/* MKW */
// int             logf( const char *format, ... );
                        // mkw: print to log formatted

int             chprintf( CHAR_DATA* ch, const char *format, ... );
                        // mkw: print to character formatted

int             strswitch( const char* arg, ... );
                        // mkw: "bundles" any number of string comparisons neatly
                        // into one return value that can be used in a switch statement.
                        // Last argument passed must be NULL to mark end of list.
                        // Will behave badly if that is forgotten or anything other than
                        // strings are passed.
                        // First variable argument is counted as one.
                        // Returns 0 if no strings match.



void    vinterpret( CHAR_DATA *ch, char *argument, ... );
                        // mkw: This is my subcommand interpreter.
                        // Pattern for variable args:
                        // { string_1, [do_fun_1|NULL], ... } NULL, [default_do_fun|NULL]
                        // NULL passed for a string marks end of list.
                        // NULL passed for a do_fun means no action on match.

#undef    CD
#undef    MID
#undef    OD
#undef    OID
#undef    RID
#undef    SF
#undef AD

/*****************************************************************************
 *                                    OLC                                    *
 *****************************************************************************/

/*
 * Object defined in limbo.are
 * Used in save.c to load objects that don't exist.
 */
#define OBJ_VNUM_DUMMY    30

/*
 * Area flags.
 */
#define         AREA_NONE       (A)
#define         AREA_CHANGED    (B)    /* Area has been modified. */
#define         AREA_ADDED      (C)    /* Area has been added to. */
#define         AREA_LOADING    (D)    /* Used for counting in db.c */
#define         AREA_NOQUEST    (E)    /* Used for Quest */
#define         AREA_UNLINKED   (F)    /* Used for Quest */
#define         AREA_NO_PORT    (G)
#define         AREA_NO_PORT_FROM (H)
#define         AREA_NO_DREAM  (I)
#define         AREA_NO_RECALL (J)
#define         AREA_NO_SUMMON (K)
#define         AREA_NO_SUMMON_FROM (L)
#define         AREA_NO_MAGIC (M)
#define MAX_DIR    6
#define NO_FLAG -99    /* Must not be used in flags or stats. */

/*
 * Global Constants
 */
extern    char *    const    dir_name        [];
extern    const    sh_int    rev_dir         [];          /* sh_int - ROM OLC */
extern    const    struct    spec_type    spec_table    [];
extern    char *    const    rev_move        [];
extern    char *    const    u_dir_name      [];

/*
 * Global variables
 */
extern        AREA_DATA *        area_first;
extern        AREA_DATA *        area_last;
extern        SHOP_DATA *        shop_last;

extern        int            top_affect;
extern        long            top_area;
extern        long            top_ed;
extern        long            top_exit;
extern        long            top_help;
extern        long            top_mob_index;
extern        long            top_obj_index;
extern        long            top_reset;
extern        long            top_room;
extern        long            top_shop;

extern        long            top_vnum_mob;
extern        long            top_vnum_obj;
extern        long            top_vnum_room;
extern        int             low_mobbed;
extern        int             low_roomed;

extern         int            top_vnum_riddle;
extern        char            str_empty       [1];

extern        MOB_INDEX_DATA *    mob_index_hash  [MAX_KEY_HASH];
extern        OBJ_INDEX_DATA *    obj_index_hash  [MAX_KEY_HASH];
extern        ROOM_INDEX_DATA *    room_index_hash [MAX_KEY_HASH];
extern        RIDDLE_DATA *        riddle_hash [MAX_KEY_HASH];

// auction code by kimber
typedef struct auction_data		AUCTION_DATA;


struct auction_data
{
  OBJ_DATA *	item;
  CHAR_DATA *	owner;
  CHAR_DATA *	high_bidder;
  sh_int	status;
  long		current_bid;
  long		silver_held;
  long		gold_held;
  long		minimum;
};

extern		AUCTION_DATA		auction_info;

/* Restring stuff by Asmo similar to Auction :) */

typedef struct restring_data             RESTRING_DATA;

struct restring_data
{
  OBJ_DATA * item;
  CHAR_DATA * owner;
};

extern         RESTRING_DATA            restring_info;


#define PULSE_AUCTION		(20 * PULSE_PER_SECOND)
#define MINIMUM_BID		100
#define	AUCTION_LENGTH		5

void    auction_update  args( ( void ) );
void    auction_channel args( ( char *msg ) );

// rp point code display
void display_rp(CHAR_DATA *ch,CHAR_DATA *target);
