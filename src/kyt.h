#define IS_CLAN( ch, ck_clan ) ( ch->clan == clan_lookup( ck_clan ) )


//typedef struct mark_type MARK_DATA;
typedef struct war_type WAR_DATA;

/*struct mark_type
{
    MARK_DATA * next;
    char *     	contractor;
    char * 	target;
    char *      status;
    char *      price;
    char *      extra;
    char *      info;
};*/


struct war_type
{
    WAR_DATA * 	next;
    int		clan;
    int		enemy;
    int		lose;
    int		win;
    int		tick;   /* when to join war for said guild */
};

//extern          MARK_DATA *         mark_first;
extern          WAR_DATA *         war_first;

