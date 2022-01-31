/* Header for Tournament.c */

#define MAX_TOURNAMENT_RANK 10

extern const struct tournament_rank_type tournament_ranks[MAX_TOURNAMENT_RANK];

#define TOURNAMENT_STARTING 0
#define TOURNAMENT_ON       1
#define TOURNAMENT_OFF      2

#define TOURNAMENT_NEEDED_PLAYERS 3

struct tournament_rank_type
{
  char *name;
  sh_int reputation;
  sh_int prize;
  char *who;
};

void start_tournament args ((void));
void abandon_tournament args ((void));
void techo args ((char *buf));
char *show_trank args ((CHAR_DATA *ch));
void global args ((char *buf, int level));
void tournament_gain args ((CHAR_DATA *ch, int points));

