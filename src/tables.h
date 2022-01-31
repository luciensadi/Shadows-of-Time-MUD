/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are	   *
 *  benefitting.  We hope that you share your changes too.  What goes	   *
 *  around, comes around.						   *
 ***************************************************************************/
 
/***************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@efn.org)				   *
*	    Russ Taylor (rtaylor@hypercube.org)				   *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

#define MAX_COPYOVER 8
#define MAX_ENCHANT 15 
#define ENCH_AFFECT 0

/* game tables */

// extern	const	struct	clan_type	clan_table[MAX_CLAN];
//extern  const   struct  rank_type       rank_table[MAX_CLAN];
extern  const   struct  forsaken_type   forsaken_table[MAX_FORSAKEN];
//extern  const   struct  qprize_type   qprize_table[MAX_PRIZE];
extern  const   struct  rp_prize_type rp_prize_table[MAX_RP_PRIZE];
extern  const   struct  prize_coupon_type prize_coupon_table[MAX_PRIZE_COUPON]; 
extern  const   struct healer_type       healer_table[MAX_HEAL];

extern  const   struct  flag_type       trap_type_table[TRAP_MAX+1];
extern  const char   *    copyover_table[MAX_COPYOVER];
extern  const char   *    rank_table[MAX_CLAN][6];
extern  const char   *    gskill_table[MAX_CLAN][MAX_GUILD_SKILL];
extern	const	struct	position_type	position_table[];
extern	const	struct	sex_type	sex_table[];
extern	const	struct	size_type	size_table[];
extern  const char *couple_spells[MAX_COUPLE_SPELLS];
extern  const char *weave_table[MAX_WEAVES];

extern const struct enchant_type enchant_table[MAX_ENCHANT]; 

/* flag tables */

extern	const	struct	flag_type	act_flags[];
extern	const	struct	flag_type	plr_flags[];
extern	const	struct	flag_type	plr2_flags[];
extern	const	struct	flag_type	affect_flags[];
extern	const	struct	flag_type	off_flags[];
extern	const	struct	flag_type	imm_flags[];
extern	const	struct	flag_type	form_flags[];
extern	const	struct	flag_type	part_flags[];
extern	const	struct	flag_type	comm_flags[];
extern	const	struct	flag_type	extra_flags[];
extern	const	struct	flag_type	wear_flags[];
extern	const	struct	flag_type	class_flags[];
extern	const	struct	flag_type	weapon_flags[];
extern	const	struct	flag_type	container_flags[];
extern	const	struct	flag_type	portal_flags[];
extern	const	struct	flag_type	room_flags[];
extern	const	struct	flag_type	exit_flags[];
extern 	const	struct  flag_type	mprog_flags[];
extern 	const	struct  flag_type	oprog_flags[];
extern 	const	struct  flag_type	rprog_flags[];
extern	const	struct	flag_type	area_flags[];
extern	const	struct	flag_type	sector_flags[];
extern	const	struct	flag_type	door_resets[];
extern	const	struct	flag_type	wear_loc_strings[];
extern	const	struct	flag_type	wear_loc_flags[];
extern	const	struct	flag_type	res_flags[];
extern	const	struct	flag_type	imm_flags[];
extern	const	struct	flag_type	vuln_flags[];
extern	const	struct	flag_type	type_flags[];
extern	const	struct	flag_type	explosion_flags[];
extern	const	struct	flag_type	apply_flags[];
extern	const	struct	flag_type	sex_flags[];
extern	const	struct	flag_type	furniture_flags[];
extern	const	struct	flag_type	weapon_class[];
extern	const	struct	flag_type	apply_types[];
extern	const	struct	flag_type	weapon_type2[];
extern	const	struct	flag_type	apply_types[];
extern	const	struct	flag_type	size_flags[];
extern	const	struct	flag_type	position_flags[];
extern	const	struct	flag_type	ac_type[];
extern	const	struct	flag_type	state_flags[];
extern	const	struct	flag_type	trigger_flags[];
extern	const	struct	flag_type	ingredient_flags[];
extern const struct     seed_type       seed_type[MAX_SEED];
extern const struct     plant_type      plant_type[MAX_PLANT];
extern const struct     herb_type       herb_type[MAX_HERB];
extern	const	struct	bit_type	bitvector_type[];
extern  const   struct  flag_type       tabf_flags[];



struct flag_type
{
    char *name;
    int bit;
    bool settable;
    int points;
};

struct gskill_type
{
    char *name;
    bool settable;
};

struct gskill_table_type
{
  struct gskill_type skills[MAX_GUILD_SKILL];   
};

struct healer_type
{
  char *keyword;
  char *display;
  int cost;
}; 

/*
struct clan_type
{
    char 	*name;
    char 	*who_name;
    char *	display;
    sh_int 	hall;
    bool	independent;
};
*/  

struct forsaken_type
{
  char *name;
};

/*
struct qprize_type
{
  char *keyword;
  char *showname;
  char *level;
  int vnum;
  int cost;
  bool special;
};
*/

struct prize_coupon_type
{
  int vnum;
};

struct enchant_type
{
  char *keyword;
  char *desc;
  sh_int apply;
  sh_int type;
  sh_int modifier;
  sh_int coupons;
};

struct rp_prize_type
{
	char *name;
	char *level;
	int vnum;
	int cost;
};

struct position_type
{
    char *name;
    char *short_name;
};

struct sex_type
{
    char *name;
    sh_int weaves[MAX_WEAVES];
};

struct size_type
{
    char *name;
};

struct	bit_type
{
	const	struct	flag_type *	table;
	char *				help;
};
