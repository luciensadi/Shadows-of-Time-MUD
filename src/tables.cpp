/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
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
*    ROM 2.4 is copyright 1993-1998 Russ Taylor                            *
*    ROM has been brought to you by the ROM consortium                     *
*        Russ Taylor  (rtaylor@hypercube.org)                              *
*        Gabrielle Taylor (gtaylor@hypercube.org)               *
*        Brian Moore (zump@rom.org)                       *
*    By using this code, you have agreed to follow the terms of the       *
*    ROM license, in the file Rom24/doc/rom.license               *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "tables.h"

/* for clans */
const char* weave_table[MAX_WEAVES] =
{
  "fire",
  "earth",
  "air",
  "water",
  "spirit",
};

const struct flag_type trap_type_table[TRAP_MAX+1] =
{
    { "damage",         TRAP_DAMAGE,            TRUE    },
    { "mana",           TRAP_MANA,              TRUE    },
    { "teleport",       TRAP_TELEPORT,          TRUE    },
    { "latch",          TRAP_LATCH,             TRUE    },
    { NULL,             0,                      FALSE   }
};

const char* couple_spells[MAX_COUPLE_SPELLS] =
{
  "blindness",
  "poison",
  "sleep",
  "curse",
  "weaken",
  "plague",
  "slow",
  "dispel magic",
  "fear",
};

const struct flag_type tabf_flags[] = {
    {"smoking", TABF_SMOKING, FALSE},
    {NULL, 0, 0}
};

const struct enchant_type enchant_table[MAX_ENCHANT] =
{
  {"HP", "10 Hps", APPLY_HIT, ENCH_AFFECT, 10, 1},
  {"MN", "10 Mana", APPLY_MANA, ENCH_AFFECT, 10, 1},
  {"MV", "10 Move", APPLY_MOVE, ENCH_AFFECT, 10, 1},
  {"AC", "-10 AC", APPLY_AC, ENCH_AFFECT, -10, 1},
  {"SV", "-3 Saves", APPLY_SAVES, ENCH_AFFECT, -3, 1},
  {"HD", "1/1 Hit/Dam", APPLY_HITROLL, ENCH_AFFECT, 1, 1},
  {"HP2", "25 Hps", APPLY_HIT, ENCH_AFFECT, 25, 2},
  {"MN2", "25 Mana", APPLY_MANA, ENCH_AFFECT, 25, 2},
  {"MV2", "25 Move", APPLY_MOVE, ENCH_AFFECT, 25, 2},
  {"SV2", "-6 Saves", APPLY_SAVES, ENCH_AFFECT, -6, 2},
  {"HD2", "2/2 Hit/Dam", APPLY_HITROLL, ENCH_AFFECT, 2, 2},
  {"SV3", "-10 Saves", APPLY_SAVES, ENCH_AFFECT, -10, 3},
  {"HD3", "3/3 Hit/Dam", APPLY_HITROLL, ENCH_AFFECT, 3, 3},
  {"HD4", "4/4 Hit/Dam", APPLY_HITROLL, ENCH_AFFECT, 4, 4},
  {"HD5", "5/5 Hit/Dam", APPLY_HITROLL, ENCH_AFFECT, 5, 5},
};
 
const char *copyover_table[MAX_COPYOVER] =
{
  "Copyover - Hold on to your hats this is going to be a wild ride!\n\r",
  "Copyover - Please remain seated until the mud has come to a complete stop.\n\r",
  "Copyover - Please return you tray tables to the upright and locked position.\n\r",
  "Copyover - Relax, you're in the hands of a capable professional.\n\r",
  "Copyover - You may feel a little pressure....\n\r",
  "Copyover - Stick your head between your legs and kiss your ass goodbye.\n\r",
  "Copyover - In the event of a water landing, your seat cushion acts as a flotation device\n\r",
  "Copyover - This is going to hurt me more than it's going to hurt you.\n\r",
};
 
/*
const struct qprize_type qprize_table[MAX_PRIZE] =
{
  { "name", "display name", "level", vnum, cost, special },
    { "Restring", "`$R`4estring `!C`1oupon`*", "N/A", 6400, 25, FALSE },
    { "Prize", "`@P`2rize `!C`1oupon`*", "N/A", 6401, 1000, FALSE },
    { "Enchant", "`#E`3nchant `!C`1oupon`*", "N/A", 6402, 2000, FALSE },
    { "Gold", "500 Gold", "N/A", 0, 100, TRUE },
    { "Practices", "10 Practices", "N/A", 0, 500, TRUE },
    { "Corpse","a Corpse Retrieval Coupon","N/A",6413,25,FALSE},
    { "Nobreak", "`3N`8o `3B`8reak `3C`8oupon`*", "N/A", 6411, 1800, FALSE},
    { "tattoo", "`8drag`&o`8n tatt`&oo", "75", 24422, 2000, FALSE},
    { "gauntlets", "`8Dark `1Gauntlets `&Of `#Po`7w`&er`7", "75", 17045, 3500, FALSE},
    { "study", "`#A C`&oupon `#of `#S`&tudy`*", "20", 5546, 1500, FALSE},
    { "scrying", "A Book of Scrying", "25", 21508, 1500, FALSE},
    { "coupling", "A Book of Coupling - Mages Only", "30", 21538, 1000, FALSE},
    { "save", "A Book of Save Life", "25", 5545, 2000, FALSE},
    { "store", "A Book of Storekeeping", "25", 5547, 2500, FALSE},    
    { "horseback", "A Book of Horseback Riding", "40", 718, 1000, FALSE},    
    { "pretitle", "A Pretitle Coupon", "1", 6427, 500, FALSE},
    { "taming", "A Book of Taming", "50", 21566, 500, FALSE},    
};
*/

const struct rp_prize_type rp_prize_table[MAX_RP_PRIZE] =
{
  /*{ "name", "level", vnum, cost},*/
	{ "RPlevel","N/A", 0, 15},
	{ "Legend","N/A", 0, 35},
	{ "Train","N/A", 0, 5},
	{ "Practices", "N/A", 0, 3},
	{ "Gold", "N/A", 0, 1},
	{ "Bonus2x","N/A", 0,2},
	{ "Bonus3x","N/A", 0,5}, 
        { NULL,NULL, 0, 0},
        { NULL,NULL, 0, 0},
	{ NULL,NULL, 0, 0}
};

const struct prize_coupon_type prize_coupon_table[MAX_PRIZE_COUPON] =
{
   {  2208 }, /* Gas Mask */
   {  6400 }, /* Restring */
   {  2412 }, /* Brand */
   { 24405 }, /* Apoc */
   { 24501 }, /* Seanchan Boots */
   { 24475 }, /* Axe of Glory */
   { 24404 }, /* Nym */
   { 7509  }, /* Golden Greaves */
   { 6441 },  /* Golden Plate Armor */
   {  899  }, /* Aiel Spear */
   { 24427 }, /* Dagger Poison */
   { 24404 }, /* Presence of Nym */
   { 24402 }, /* Moonlight */ 
   {  6410 }, /* Mud Bag */
   {  6410 }, /* Mud Bag */
   { 11709 }, /* Oath of Vengance */
   { 6405  }, /* Finely Crafted Dagger */
   { 20977 }, /* Angreal of Power */
   { 6411 }, /* No Break Coupon */
   { 6412 }, /* Gold Coupon */
   { 6411 }, /* No Break */
   { 24431 }, /* Lamp of Foresight */
   { 24529 }, /* Rib Cage */
   { 2207 }, /* Mage Gas Mask */
   { 24407 }, /* 75 Shoulder */

};           

const struct healer_type healer_table[MAX_HEAL] = {
/*   {"keyword", "display", base cost} */

     {"light", "Cure light wounds", 500},
     {"serious", "Cure serious wounds", 1000},
     {"critic", "Cure critical wounds", 2000},
     {"heal", "Healing spell", 4000},
     {"blind", "Cure blindness", 1500},
     {"disease", "Cure disease", 1000},
     {"poison", "Cure poison", 2000},
     {"uncurse", "Remove curse", 4000},
     {"refresh", "Restore movement", 250},
     {"mana", "Restore mana", 750},
     {"disenchant", "Disenchant item", 4000},
};

/* For forsaken, just names leaves room for future expanstion */
const struct forsaken_type forsaken_table[MAX_FORSAKEN] = {
   {"Aginor"},
   {"Asmodean"},
   {"Balthamel"},
   {"Be'lal"},
   {"Cyndane"},
   {"Demandred"},
   {"Graendal"},
   {"Ishamael"},
   {"Masaana"},
   {"Moghedien"},
   {"Rahvin"},
   {"Sammael"},
   {"Moridin"},
   {"Semirhage"},
   {"Caddar"},
   {"Nemene"},

};   


/* for position */
const struct position_type position_table[] = {
    {"dead", "dead"},
    {"mortally wounded", "mort"},
    {"incapacitated", "incap"},
    {"stunned", "stun"},
    {"sleeping", "sleep"},
    {"resting", "rest"},
    {"sitting", "sit"},
    {"fighting", "fight"},
    {"standing", "stand"},
    {NULL, NULL}
};

/* for sex */
const struct sex_type sex_table[] = {
    {"none", {3, 3, 3, 3, 3} },
    {"male ", {5, 4, 2, 1, 3} },
    {"female", {1, 2, 4, 5, 3} },
    {"either", {3, 3, 3, 3, 3} },
    {NULL}
};

/* for sizes */
const struct size_type size_table[] = {
    {"tiny"},
    {"small"},
    {"medium"},
    {"large"},
    {"huge",},
    {"giant"},
    {NULL}
};

/* various flag tables */
const struct flag_type act_flags[] = {
    {"npc", A, FALSE},
    {"sentinel", B, TRUE},
    {"scavenger", C, TRUE},
    {"mount", D, TRUE},
    {"aggressive", F, TRUE},
    {"stay_area", G, TRUE},
    {"wimpy", H, TRUE},
    {"pet", I, TRUE},
    {"train", J, TRUE},
    {"practice", K, TRUE},
    {"guildmob", M, TRUE},
    {"undead", O, TRUE},
    {"cleric", Q, TRUE},
    {"mage", R, TRUE},
    {"thief", S, TRUE},
    {"warrior", T, TRUE},
    {"noalign", U, TRUE},
    {"nopurge", V, TRUE},
    {"outdoors", W, TRUE},
    {"indoors", Y, TRUE},
    {"noquest", Z, TRUE},
    {"healer", aa, TRUE},
    {"gain", bb, TRUE},
    {"update_always", cc, TRUE},
    {"changer", dd, TRUE},
    {"nohunt", ee,TRUE},
    {NULL, 0, FALSE}
};

const struct flag_type plr_flags[] = {
    {"npc", A, FALSE},
    {"autoassist", C, FALSE},
    {"autoexit", D, FALSE},
    {"autoloot", E, FALSE},
    {"autosac", F, FALSE},
    {"autogold", G, FALSE},
    {"autosplit", H, FALSE},
    {"hunger_flag", J, FALSE},
    {"holylight", N, FALSE},
    {"can_loot", P, FALSE},
    {"nosummon", Q, FALSE},
    {"nofollow", R, FALSE},
    {"colour", T, FALSE},
    {"permit", U, TRUE},
    {"log", W, FALSE},
    {"deny", X, FALSE},
    {"freeze", Y, FALSE},
    {"thief", Z, FALSE},
    {"killer", aa, FALSE},
    {"nopk", bb, FALSE},
    {NULL, 0, 0}
};

const struct flag_type plr2_flags[] = {
    {"noip", A, FALSE},
    {"developer", L, FALSE}, 
    {"norp", M, FALSE},
    {"testor", R, FALSE},
    {NULL, 0, 0}
};

const struct flag_type affect_flags[] = {
    {"blindness", AFF_BLIND, TRUE},
    {"invisibility", AFF_INVISIBLE, TRUE},
    {"detect_evil", AFF_DETECT_EVIL, TRUE},
    {"detect_invis", AFF_DETECT_INVIS, TRUE},
    {"detect_magic", AFF_DETECT_MAGIC, TRUE},
    {"detect_hidden", AFF_DETECT_HIDDEN, TRUE},
    {"detect_good", AFF_DETECT_GOOD, TRUE},
    {"disorient", AFF_DISORIENT, TRUE},
    {"gouge", AFF_GOUGE, TRUE},
    {"darkhound", AFF_DARKHOUND, TRUE},
    {"sanctuary", AFF_SANCTUARY, TRUE},
    {"faerie_fire", AFF_FAERIE_FIRE, TRUE},
    {"infravision", AFF_INFRARED, TRUE},
    {"curse", AFF_CURSE, TRUE},
    {"ravage", AFF_RAVAGE, TRUE},
    {"poison", AFF_POISON, TRUE},
    {"protection_evil", AFF_PROTECT_EVIL, TRUE},
    {"protection_good", AFF_PROTECT_GOOD, TRUE},
    {"sneak", AFF_SNEAK, TRUE},
    {"hide", AFF_HIDE, TRUE},
    {"sleep", AFF_SLEEP, TRUE},
    {"charm", AFF_CHARM, TRUE},
    {"fly", AFF_FLYING, TRUE},
    {"pass_door", AFF_PASS_DOOR, TRUE},
    {"haste", AFF_HASTE, TRUE},
    {"calm", AFF_CALM, TRUE},
    {"plague", AFF_PLAGUE, TRUE},
    {"weaken", AFF_WEAKEN, TRUE},
    {"rift", AFF_RIFT, TRUE},
    {"dark_vision", AFF_DARK_VISION, TRUE},
    {"berserk", AFF_BERSERK, TRUE},
    {"regeneration", AFF_REGENERATION, TRUE},
    {"slow", AFF_SLOW, TRUE},
    {"veil", AFF_VEIL, TRUE},
    {"warcry", AFF_WARCRY, TRUE},
    {"restrain", AFF_RESTRAIN, TRUE},
    {"restrain2", AFF_RESTRAIN2, TRUE},
    {"firestorm", AFF_FIRESTORM, TRUE},
    {"still", AFF_STILL, TRUE},
    {"ballad", AFF_BALLAD, TRUE},
    {"dome", AFF_DOME, TRUE},
    {"stance", AFF_STANCE, TRUE},
    {"cleanse", AFF_CLEANSE, TRUE},
    {"pray", AFF_PRAY, TRUE},
    {"stealth", AFF_STEALTH, TRUE},
    {"shroud", AFF_SHROUD, TRUE},
    {"wolf", AFF_WOLF, TRUE},
    {"focus", AFF_FOCUS, TRUE},
    {"dimension_view", AFF_DIMENSION_VIEW, TRUE},
    {"fear", AFF_FEAR, TRUE},
    {"iceshield", AFF_ICESHIELD, TRUE},
    {"fireshield", AFF_FIRESHIELD, TRUE},
    {"shockshield", AFF_SHOCKSHIELD, TRUE},
    {"contact", AFF_CONTACT, TRUE},
    {"severed", AFF_SEVERED, TRUE},
    {"damane", AFF_DAMANE, TRUE},
    {"manadrain", AFF_MANADRAIN, TRUE},
    {"manadrain2", AFF_MANADRAIN2, TRUE},
    {"howl", AFF_HOWL, TRUE},
    {"medallion", AFF_MEDALLION, TRUE},
    {"daze", AFF_DAZE, TRUE},
    {"ironskin", AFF_IRONSKIN, TRUE},
    {"awareness", AFF_AWARENESS, TRUE},
    {"sticky", AFF_STICKY, TRUE},
    {"manashield",AFF_MANASHIELD,TRUE},
    {"teleorient",AFF_TELE_DISORIENT,FALSE},
    {"blackjack",AFF_BLACKJACK,FALSE},
    {"blackjack2",AFF_BLACKJACK2,FALSE},
    {"armor",AFF_ARMOR,TRUE},
    {"shield",AFF_SHIELD,TRUE},
    {"stoneskin",AFF_STONESKIN,TRUE},
    {"bless",AFF_BLESS,TRUE},
    {"frenzy",AFF_FRENZY,FALSE},
    {"galloping",AFF_GALLOPING,FALSE},
    {"willpower",AFF_WILLPOWER, TRUE},
    {"bandage", AFF_BANDAGE, TRUE},
    {NULL, 0, 0}
};

const struct flag_type off_flags[] = {
    {"area_attack", A, TRUE},
    {"backstab", B, TRUE},
    {"bash", C, TRUE},
    {"berserk", D, TRUE},
    {"disarm", E, TRUE},
    {"dodge", F, TRUE},
    {"fade", G, TRUE},
    {"fast", H, TRUE},
    {"kick", I, TRUE},
    {"dirt_kick", J, TRUE},
    {"parry", K, TRUE},
    {"rescue", L, TRUE},
    {"tail", M, TRUE},
    {"trip", N, TRUE},
    {"crush", O, TRUE},
    {"assist_all", P, TRUE},
    {"assist_align", Q, TRUE},
    {"assist_race", R, TRUE},
    {"assist_players", S, TRUE},
    {"assist_guard", T, TRUE},
    {"assist_vnum", U, TRUE},
    {NULL, 0, 0}
};

const struct flag_type imm_flags[] = {
    {"summon", A, TRUE},
    {"charm", B, TRUE},
    {"magic", C, TRUE},
    {"weapon", D, TRUE},
    {"bash", E, TRUE},
    {"pierce", F, TRUE},
    {"slash", G, TRUE},
    {"fire", H, TRUE},
    {"cold", I, TRUE},
    {"lightning", J, TRUE},
    {"acid", K, TRUE},
    {"poison", L, TRUE},
    {"negative", M, TRUE},
    {"holy", N, TRUE},
    {"energy", O, TRUE},
    {"mental", P, TRUE},
    {"disease", Q, TRUE},
    {"drowning", R, TRUE},
    {"light", S, TRUE},
    {"sound", T, TRUE},
    {"wood", X, TRUE},
    {"silver", Y, TRUE},
    {"iron", Z, TRUE},
    {"bomb", aa, TRUE},
    {"arrow", bb, TRUE},
    {NULL, 0, 0}
};

const struct flag_type form_flags[] = {
    {"edible", FORM_EDIBLE, TRUE},
    {"poison", FORM_POISON, TRUE},
    {"magical", FORM_MAGICAL, TRUE},
    {"instant_decay", FORM_INSTANT_DECAY, TRUE},
    {"other", FORM_OTHER, TRUE},
    {"animal", FORM_ANIMAL, TRUE},
    {"sentient", FORM_SENTIENT, TRUE},
    {"undead", FORM_UNDEAD, TRUE},
    {"construct", FORM_CONSTRUCT, TRUE},
    {"mist", FORM_MIST, TRUE},
    {"intangible", FORM_INTANGIBLE, TRUE},
    {"biped", FORM_BIPED, TRUE},
    {"centaur", FORM_CENTAUR, TRUE},
    {"insect", FORM_INSECT, TRUE},
    {"spider", FORM_SPIDER, TRUE},
    {"crustacean", FORM_CRUSTACEAN, TRUE},
    {"worm", FORM_WORM, TRUE},
    {"blob", FORM_BLOB, TRUE},
    {"mammal", FORM_MAMMAL, TRUE},
    {"bird", FORM_BIRD, TRUE},
    {"reptile", FORM_REPTILE, TRUE},
    {"snake", FORM_SNAKE, TRUE},
    {"dragon", FORM_DRAGON, TRUE},
    {"amphibian", FORM_AMPHIBIAN, TRUE},
    {"fish", FORM_FISH, TRUE},
    {"cold_blood", FORM_COLD_BLOOD, TRUE},
    {NULL, 0, 0}
};

const struct flag_type part_flags[] = {
    {"head", PART_HEAD, TRUE},
    {"arms", PART_ARMS, TRUE},
    {"legs", PART_LEGS, TRUE},
    {"heart", PART_HEART, TRUE},
    {"brains", PART_BRAINS, TRUE},
    {"guts", PART_GUTS, TRUE},
    {"hands", PART_HANDS, TRUE},
    {"feet", PART_FEET, TRUE},
    {"fingers", PART_FINGERS, TRUE},
    {"ear", PART_EAR, TRUE},
    {"eye", PART_EYE, TRUE},
    {"long_tongue", PART_LONG_TONGUE, TRUE},
    {"eyestalks", PART_EYESTALKS, TRUE},
    {"tentacles", PART_TENTACLES, TRUE},
    {"fins", PART_FINS, TRUE},
    {"wings", PART_WINGS, TRUE},
    {"tail", PART_TAIL, TRUE},
    {"claws", PART_CLAWS, TRUE},
    {"fangs", PART_FANGS, TRUE},
    {"horns", PART_HORNS, TRUE},
    {"scales", PART_SCALES, TRUE},
    {"tusks", PART_TUSKS, TRUE},
    {NULL, 0, 0}
};

const struct flag_type comm_flags[] = {
    {"quiet", COMM_QUIET, TRUE},
    {"deaf", COMM_DEAF, TRUE},
    {"nowiz", COMM_NOWIZ, TRUE},
    {"noclangossip", COMM_NOAUCTION, TRUE},
    {"nogossip", COMM_NOGOSSIP, TRUE},
    {"noquestion", COMM_NOQUESTION, TRUE},
    {"nomusic", COMM_NOMUSIC, TRUE},
    {"noclan", COMM_NOCLAN, TRUE},
    {"noquote", COMM_NOQUOTE, TRUE},
    {"shoutsoff", COMM_SHOUTSOFF, TRUE},
    {"compact", COMM_COMPACT, TRUE},
    {"brief", COMM_BRIEF, TRUE},
    {"prompt", COMM_PROMPT, TRUE},
    {"combine", COMM_COMBINE, TRUE},
    {"telnet_ga", COMM_TELNET_GA, TRUE},
    {"show_affects", COMM_SHOW_AFFECTS, TRUE},
    {"nograts", COMM_NOGRATS, TRUE},
    {"noemote", COMM_NOEMOTE, FALSE},
    {"novice", COMM_NOVICE, FALSE},
    {"notell", COMM_NOTELL, FALSE},
    {"nochannels", COMM_NOCHANNELS, FALSE},
    {"snoop_proof", COMM_SNOOP_PROOF, FALSE},
    {"afk", COMM_AFK, TRUE},
    {NULL, 0, 0}
};

const struct flag_type mprog_flags[] = {
    {"act", TRIG_ACT, TRUE, 0},
    {"bribe", TRIG_BRIBE, TRUE, 0},
    {"death", TRIG_DEATH, TRUE, 0},
    {"entry", TRIG_ENTRY, TRUE, 0},
    {"fight", TRIG_FIGHT, TRUE, 0},
    {"give", TRIG_GIVE, TRUE, 0},
    {"greet", TRIG_GREET, TRUE, 0},
    {"grall", TRIG_GRALL, TRUE, 0},
    {"kill", TRIG_KILL, TRUE, 0},
    {"hpcnt", TRIG_HPCNT, TRUE, 0},
    {"random", TRIG_RANDOM, TRUE, 0},
    {"speech", TRIG_SPEECH, TRUE, 0},
    {"exit", TRIG_EXIT, TRUE, 0},
    {"exall", TRIG_EXALL, TRUE, 0},
    {"delay", TRIG_DELAY, TRUE, 0},
    {"surr", TRIG_SURR, TRUE, 0},
    {NULL, 0, TRUE, 0}
};

const struct flag_type area_flags[] = {
    {"none", AREA_NONE, FALSE},
    {"changed", AREA_CHANGED, TRUE},
    {"added", AREA_ADDED, TRUE},
    {"loading", AREA_LOADING, FALSE},
    {"noquest", AREA_NOQUEST, TRUE},
    {"unlinked", AREA_UNLINKED, TRUE},
    {"no_port", AREA_NO_PORT, TRUE},
    {"no_port_from", AREA_NO_PORT_FROM, TRUE},
    {"no_dream", AREA_NO_DREAM, TRUE},
    {"no_recall", AREA_NO_RECALL, TRUE},
    {"no_summon", AREA_NO_SUMMON, TRUE},
    {"no_summon_from", AREA_NO_SUMMON_FROM, TRUE},
    {"no_magic", AREA_NO_MAGIC, TRUE},
    {NULL, 0, 0}
};



const struct flag_type sex_flags[] = {
    {"male", SEX_MALE, TRUE},
    {"female", SEX_FEMALE, TRUE},
    {"neutral", SEX_NEUTRAL, TRUE},
    {"random", 3, TRUE},        /* ROM */
    {"none", SEX_NEUTRAL, TRUE},
    {NULL, 0, 0}
};



const struct flag_type exit_flags[] = {
    {"door", EX_ISDOOR, TRUE},
    {"closed", EX_CLOSED, TRUE},
    {"locked", EX_LOCKED, TRUE},
    {"pickproof", EX_PICKPROOF, TRUE},
    {"nopass", EX_NOPASS, TRUE},
    {"easy", EX_EASY, TRUE},
    {"hard", EX_HARD, TRUE},
    {"infuriating", EX_INFURIATING, TRUE},
    {"noclose", EX_NOCLOSE, TRUE},
    {"nolock", EX_NOLOCK, TRUE},
    {NULL, 0, 0}
};



const struct flag_type door_resets[] = {
    {"open and unlocked", 0, TRUE},
    {"closed and unlocked", 1, TRUE},
    {"closed and locked", 2, TRUE},
    {NULL, 0, 0}
};



const struct flag_type room_flags[] = {
    {"dark", ROOM_DARK, TRUE},
    {"no_mob", ROOM_NO_MOB, TRUE},
    {"indoors", ROOM_INDOORS, TRUE},
    {"private", ROOM_PRIVATE, TRUE},
    {"safe", ROOM_SAFE, TRUE},
    {"solitary", ROOM_SOLITARY, TRUE},
    {"pet_shop", ROOM_PET_SHOP, TRUE},
    {"no_recall", ROOM_NO_RECALL, TRUE},
    {"imp_only", ROOM_IMP_ONLY, TRUE},
    {"gods_only", ROOM_GODS_ONLY, TRUE},
    {"heroes_only", ROOM_HEROES_ONLY, TRUE},
    {"newbies_only", ROOM_NEWBIES_ONLY, TRUE},
    {"law", ROOM_LAW, TRUE},
    {"nowhere", ROOM_NOWHERE, TRUE},
    {"bank", ROOM_BANK, TRUE},
    {"no_magic", ROOM_NOMAGIC,TRUE},
    {"no_push", ROOM_NOPUSH,TRUE},
    {"no_dream", ROOM_NODREAM,TRUE},
	{"no_port", ROOM_NOPORT,TRUE},
	{"deathtrp", ROOM_DEATHTRAP,TRUE},
	{"roleprize", ROOM_ROLEPRIZE,TRUE},
    {"empty store", ROOM_EMPTY_STORE, TRUE},
    {"wilderness", ROOM_WILDERNESS, TRUE},
    {"no_port_from", ROOM_NOPORT_FROM, TRUE}, 
    {"aiel", ROOM_AIEL, TRUE},
    {"shadow", ROOM_SHADOW, TRUE},
    {"whitecloak", ROOM_WHITECLOAK, TRUE},
    {"shaido", ROOM_SHAIDO, TRUE},
    {"tinker", ROOM_TINKER, TRUE},
    {"ashaman", ROOM_ASHAMAN, TRUE},
    {NULL, 0, 0}
};



const struct flag_type sector_flags[] = {
    {"inside", SECT_INSIDE, TRUE},
    {"city", SECT_CITY, TRUE},
    {"field", SECT_FIELD, TRUE},
    {"forest", SECT_FOREST, TRUE},
    {"hills", SECT_HILLS, TRUE},
    {"mountain", SECT_MOUNTAIN, TRUE},
    {"swim", SECT_WATER_SWIM, TRUE},
    {"noswim", SECT_WATER_NOSWIM, TRUE},
    {"road", SECT_ROAD, TRUE},
    {"air", SECT_AIR, TRUE},
    {"desert", SECT_DESERT, TRUE},
    {"rockmountain", SECT_ROCK_MOUNTAIN, TRUE},
    {"snowmountain", SECT_SNOW_MOUNTAIN, TRUE},
    {"enter", SECT_ENTER, TRUE},
    {"unused", SECT_UNUSED, TRUE},
    {"swamp", SECT_SWAMP, TRUE},
    {"jungle", SECT_JUNGLE, TRUE},
    {"ruins", SECT_RUINS, TRUE},
    {"region", SECT_REGION, TRUE},
    {"coast", SECT_COAST, TRUE},
    {"volcanicmountain", SECT_VOLCANIC_MOUNTAIN, TRUE},
    {"wall", SECT_WALL, TRUE},
    {"junction", SECT_JUNCTION, TRUE},
    {"gate", SECT_GATE, TRUE},
    {"edge", SECT_EDGE, TRUE},
    {"verticaledge", SECT_VEDGE, TRUE},
    {"tower", SECT_TOWER, TRUE},
    {"empty", SECT_EMPTY, TRUE},
    {"taken", SECT_TAKEN, TRUE},
  //  {"legend", SECT_LEGEND, TRUE},
    {NULL, 0, 0}
};



const struct flag_type type_flags[] = {
    {"light", ITEM_LIGHT, TRUE},
    {"scroll", ITEM_SCROLL, TRUE},
    {"wand", ITEM_WAND, TRUE},
    {"staff", ITEM_STAFF, TRUE},
    {"weapon", ITEM_WEAPON, TRUE},
    {"treasure", ITEM_TREASURE, TRUE},
    {"armor", ITEM_ARMOR, TRUE},
    {"potion", ITEM_POTION, TRUE},
    {"furniture", ITEM_FURNITURE, TRUE},
    {"trash", ITEM_TRASH, TRUE},
    {"container", ITEM_CONTAINER, TRUE},
    {"drinkcontainer", ITEM_DRINK_CON, TRUE},
    {"key", ITEM_KEY, TRUE},
    {"food", ITEM_FOOD, TRUE},
    {"money", ITEM_MONEY, TRUE},
    {"boat", ITEM_BOAT, TRUE},
    {"npccorpse", ITEM_CORPSE_NPC, TRUE},
    {"pc corpse", ITEM_CORPSE_PC, FALSE},
    {"fountain", ITEM_FOUNTAIN, TRUE},
    {"pill", ITEM_PILL, TRUE},
    {"protect", ITEM_PROTECT, TRUE},
    {"map", ITEM_MAP, TRUE},
    {"portal", ITEM_PORTAL, TRUE},
    {"warpstone", ITEM_WARP_STONE, TRUE},
    {"roomkey", ITEM_ROOM_KEY, TRUE},
    {"gem", ITEM_GEM, TRUE},
    {"jewelry", ITEM_JEWELRY, TRUE},
    {"jukebox", ITEM_JUKEBOX, TRUE},
    {"tool", ITEM_TOOL, TRUE},
    {"resource", ITEM_RESOURCE, TRUE},
    {"creature", ITEM_CREATURE, TRUE},
    {"window", ITEM_WINDOW, TRUE},
    {"blood", ITEM_BLOOD, TRUE},
    {"fuel", ITEM_FUEL, TRUE},
    {"fire", ITEM_FIRE, TRUE},
    {"igniter", ITEM_IGNITER, TRUE},
    {"quiver", ITEM_QUIVER, TRUE},
    {"arrow", ITEM_ARROW,TRUE},
    {"bomb", ITEM_BOMB, TRUE},
    {"ingredient", ITEM_INGREDIENT, TRUE},
    {"seeds", ITEM_SEED, TRUE},
    {"herbs", ITEM_HERB, TRUE},
    {"plants", ITEM_PLANT, TRUE},
    {"book", ITEM_BOOK, TRUE},
    {"token", ITEM_TOKEN, TRUE},
    {"keyring", ITEM_KEYRING, TRUE},
    {"envelope", ITEM_ENVELOPE, TRUE},
    {"paper", ITEM_PAPER, TRUE},
    {"pen", ITEM_PEN, TRUE},
    {"wax", ITEM_WAX, TRUE},
    {"seal", ITEM_SEAL, TRUE},
    {"trap", ITEM_TRAP, TRUE},
    {"pipetabac", ITEM_PIPE_TABAC, TRUE},
    {NULL, 0, 0}
};

const struct flag_type explosion_flags[] = {
    {"bomb_explosion", EXP_EXPLOSION, TRUE},
    {"bomb_sleeper", EXP_SLEEPER, TRUE},
    {"bomb_contact", EXP_CONTACT, TRUE},
    {"bomb_sticky", EXP_STICKY, TRUE},
    { NULL, 0, 0}
}; 

const struct flag_type extra_flags[] = {
    {"glow", ITEM_GLOW, TRUE, 0},
    {"hum", ITEM_HUM, TRUE, 0},
    {"dark", ITEM_DARK, TRUE, 0},
    {"lock", ITEM_LOCK, TRUE, 5},
    {"evil", ITEM_EVIL, TRUE, 0},
    {"invis", ITEM_INVIS, TRUE, 3},
    {"magic", ITEM_MAGIC, TRUE, 0},
    {"nodrop", ITEM_NODROP, TRUE, 0},
    {"bless", ITEM_BLESS, TRUE, 0},
    {"antigood", ITEM_ANTI_GOOD, TRUE, -5},
    {"antievil", ITEM_ANTI_EVIL, TRUE, -5},
    {"antineutral", ITEM_ANTI_NEUTRAL, TRUE, -10},
    {"noremove", ITEM_NOREMOVE, TRUE, 0},
    {"inventory", ITEM_INVENTORY, TRUE, 0},
    {"nopurge", ITEM_NOPURGE, TRUE, 0},
    {"rotdeath", ITEM_ROT_DEATH, TRUE, -25},
    {"visdeath", ITEM_VIS_DEATH, TRUE, 0},
    {"floatdrop", ITEM_FLOATDROP, TRUE, 10},
    {"nonmetal", ITEM_NONMETAL, TRUE, 0},
    {"meltdrop", ITEM_MELT_DROP, TRUE, 0},
    {"hadtimer", ITEM_HAD_TIMER, TRUE, 0},
    {"sellextract", ITEM_SELL_EXTRACT, TRUE, 0},
    {"burnproof", ITEM_BURN_PROOF, TRUE, 0},
    {"nolocate", ITEM_NOLOCATE, TRUE, 15},
    {"nouncurse", ITEM_NOUNCURSE, TRUE, 0},
    {"nobreak", ITEM_NOBREAK, TRUE, 10},
    {"taint", ITEM_TAINT, TRUE, 0},
    {"quest", ITEM_QUEST, TRUE, 0},
    {"bomb_bag", ITEM_BOMB_BAG, TRUE, 0},
    {NULL, 0, 0, 0}
};

const struct flag_type class_flags[] = {
   {"mage", CLASS_MAGE, TRUE},
   {"cleric", CLASS_CLERIC, TRUE},
   {"assassin", CLASS_ASSASSIN, TRUE},
   {"warrior", CLASS_WARRIOR, TRUE},
   {"archer", CLASS_ARCHER, TRUE},
   {"rogue", CLASS_ROGUE, TRUE},
   {"illuminator", CLASS_ILLUMINATOR, TRUE},
   {"form master", CLASS_FORM, TRUE},
   {"thief catcher", CLASS_THIEF, TRUE},
   {"forsaken", CLASS_FORSAKEN, TRUE},
   {"dragon", CLASS_DRAGON, TRUE},
   {"herbalist", CLASS_HERBALIST, TRUE},
   {NULL, 0, 0},
};

const struct flag_type wear_flags[] = {
    {"take", ITEM_TAKE, TRUE},
    {"finger", ITEM_WEAR_FINGER, TRUE},
    {"neck", ITEM_WEAR_NECK, TRUE},
    {"body", ITEM_WEAR_BODY, TRUE},
    {"head", ITEM_WEAR_HEAD, TRUE},
    {"face", ITEM_WEAR_FACE, TRUE},
    {"legs", ITEM_WEAR_LEGS, TRUE},
    {"feet", ITEM_WEAR_FEET, TRUE},
    {"hands", ITEM_WEAR_HANDS, TRUE},
    {"arms", ITEM_WEAR_ARMS, TRUE},
    {"shield", ITEM_WEAR_SHIELD, TRUE},
    {"about", ITEM_WEAR_ABOUT, TRUE},
    {"waist", ITEM_WEAR_WAIST, TRUE},
    {"onbelt", ITEM_WEAR_ON_BELT,TRUE},
    {"wrist", ITEM_WEAR_WRIST, TRUE},
    {"wield", ITEM_WIELD, TRUE},
    {"hold", ITEM_HOLD, TRUE},
    {"nosac", ITEM_NO_SAC, TRUE},
    {"wearfloat", ITEM_WEAR_FLOAT, TRUE},
    {"quiver", ITEM_WEAR_QUIVER,TRUE},
/*    {   "twohands",            ITEM_TWO_HANDS,         TRUE    }, */
    {NULL, 0, 0}
};

/*
 * Used when adding an affect to tell where it goes.
 * See addaffect and delaffect in act_olc.c
 */
const struct flag_type apply_flags[] = {
    {"none", APPLY_NONE, TRUE},
    {"strength", APPLY_STR, TRUE},
    {"dexterity", APPLY_DEX, TRUE},
    {"intelligence", APPLY_INT, TRUE},
    {"wisdom", APPLY_WIS, TRUE},
    {"constitution", APPLY_CON, TRUE},
    {"sex", APPLY_SEX, TRUE},
    {"class", APPLY_CLASS, TRUE},
    {"level", APPLY_LEVEL, TRUE},
    {"age", APPLY_AGE, TRUE},
    {"height", APPLY_HEIGHT, TRUE},
    {"weight", APPLY_WEIGHT, TRUE},
    {"mana", APPLY_MANA, TRUE},
    {"hp", APPLY_HIT, TRUE},
    {"move", APPLY_MOVE, TRUE},
    {"gold", APPLY_GOLD, TRUE},
    {"experience", APPLY_EXP, TRUE},
    {"ac", APPLY_AC, TRUE},
    {"hitroll", APPLY_HITROLL, TRUE},
    {"damroll", APPLY_DAMROLL, TRUE},
    {"saves", APPLY_SAVES, TRUE},
    {"savingpara", APPLY_SAVING_PARA, TRUE},
    {"savingrod", APPLY_SAVING_ROD, TRUE},
    {"savingpetri", APPLY_SAVING_PETRI, TRUE},
    {"savingbreath", APPLY_SAVING_BREATH, TRUE},
    {"savingspell", APPLY_SAVING_SPELL, TRUE},
    {"spellaffect", APPLY_SPELL_AFFECT, FALSE},
    {NULL, 0, 0}
};



/*
 * What is seen.
 */
const struct flag_type wear_loc_strings[] = {
    {"in the inventory", WEAR_NONE, TRUE},
    {"as a light", WEAR_LIGHT, TRUE},
    {"on the left finger", WEAR_FINGER_L, TRUE},
    {"on the right finger", WEAR_FINGER_R, TRUE},
    {"around the neck (1)", WEAR_NECK_1, TRUE},
    {"around the neck (2)", WEAR_NECK_2, TRUE},
    {"on the body", WEAR_BODY, TRUE},
    {"over the head", WEAR_HEAD, TRUE},
    {"on the face", WEAR_FACE, TRUE},
    {"on the legs", WEAR_LEGS, TRUE},
    {"on the feet", WEAR_FEET, TRUE},
    {"on the hands", WEAR_HANDS, TRUE},
    {"on the arms", WEAR_ARMS, TRUE},
    {"as a shield", WEAR_SHIELD, TRUE},
    {"about the shoulders", WEAR_ABOUT, TRUE},
    {"around the waist", WEAR_WAIST, TRUE},
    {"on belt", WEAR_ON_BELT, TRUE},
    {"on the left wrist", WEAR_WRIST_L, TRUE},
    {"on the right wrist", WEAR_WRIST_R, TRUE},
    {"wielded", WEAR_WIELD, TRUE},
    {"held in the hands", WEAR_HOLD, TRUE},
    {"floating nearby", WEAR_FLOAT, TRUE},
    {"over the shoulder", WEAR_QUIVER, TRUE},
    {NULL, 0, 0}
};


const struct flag_type wear_loc_flags[] = {
    {"none", WEAR_NONE, TRUE},
    {"light", WEAR_LIGHT, TRUE},
    {"lfinger", WEAR_FINGER_L, TRUE},
    {"rfinger", WEAR_FINGER_R, TRUE},
    {"neck1", WEAR_NECK_1, TRUE},
    {"neck2", WEAR_NECK_2, TRUE},
    {"body", WEAR_BODY, TRUE},
    {"head", WEAR_HEAD, TRUE},
    {"face", WEAR_FACE, TRUE},
    {"legs", WEAR_LEGS, TRUE},
    {"feet", WEAR_FEET, TRUE},
    {"hands", WEAR_HANDS, TRUE},
    {"arms", WEAR_ARMS, TRUE},
    {"shield", WEAR_SHIELD, TRUE},
    {"about", WEAR_ABOUT, TRUE},
    {"waist", WEAR_WAIST, TRUE},
    {"on_belt", WEAR_ON_BELT, TRUE},
    {"lwrist", WEAR_WRIST_L, TRUE},
    {"rwrist", WEAR_WRIST_R, TRUE},
    {"wielded", WEAR_WIELD, TRUE},
    {"hold", WEAR_HOLD, TRUE},
    {"floating", WEAR_FLOAT, TRUE},
    {"quiver", WEAR_QUIVER, TRUE},
    {NULL, 0, 0}
};

const struct flag_type container_flags[] = {
    {"closeable", 1, TRUE},
    {"pickproof", 2, TRUE},
    {"closed", 4, TRUE},
    {"locked", 8, TRUE},
    {"puton", 16, TRUE},
    {NULL, 0, 0}
};
// "name", vnum, level, growsplantvnum,
const struct seed_type seed_type[MAX_SEED] = {
    {8200,"Basic Blue Seeds", 1, 8201},
    {8203,"Basic Red Seeds", 1, 8203},
    {0,NULL, 0, 0}
};
//"name", vnum, level, growminticks, growmaxticks, makesherbvnum
const struct plant_type plant_type[MAX_PLANT] = {
    {"Basic Blue Plant", 8201, 1, 2, 2, 8202},
    {"Basic Red Plant", 8203, 1, 2, 2, 8204},
    {NULL, 0, 0, 0, 0, 0}
};
//"name", vnum, level, exp
const struct herb_type herb_type[MAX_HERB] = {
    {"Basic Blue Herb", 8202, 1, 100},
    {"Basic Red Herb", 8204, 1, 125},
    {NULL, 0, 0, 0}
};



/*****************************************************************************
                      ROM - specific tables:
 ****************************************************************************/




const struct flag_type ac_type[] = {
    {"pierce", AC_PIERCE, TRUE},
    {"bash", AC_BASH, TRUE},
    {"slash", AC_SLASH, TRUE},
    {"exotic", AC_EXOTIC, TRUE},
    {NULL, 0, 0}
};


const struct flag_type size_flags[] = {
    {"tiny", SIZE_TINY, TRUE},
    {"small", SIZE_SMALL, TRUE},
    {"medium", SIZE_MEDIUM, TRUE},
    {"large", SIZE_LARGE, TRUE},
    {"huge", SIZE_HUGE, TRUE},
    {"giant", SIZE_GIANT, TRUE},
    {NULL, 0, 0},
};


const struct flag_type weapon_class[] = {
    {"exotic", WEAPON_EXOTIC, TRUE},
    {"sword", WEAPON_SWORD, TRUE},
    {"dagger", WEAPON_DAGGER, TRUE},
    {"spear", WEAPON_SPEAR, TRUE},
    {"mace", WEAPON_MACE, TRUE},
    {"axe", WEAPON_AXE, TRUE},
    {"flail", WEAPON_FLAIL, TRUE},
    {"whip", WEAPON_WHIP, TRUE},
    {"polearm", WEAPON_POLEARM, TRUE},
    {"bow", WEAPON_BOW,TRUE},
    {"staff", WEAPON_STAFF,TRUE},
    {NULL, 0, 0}
};


const struct flag_type weapon_type2[] = {
    {"flaming", WEAPON_FLAMING, TRUE, 3},
    {"frost", WEAPON_FROST, TRUE, 2},
    {"vampiric", WEAPON_VAMPIRIC, TRUE, 5},
    {"sharp", WEAPON_SHARP, TRUE, 5},
    {"vorpal", WEAPON_VORPAL, TRUE, 10},
    {"twohands", WEAPON_TWO_HANDS, TRUE, -20},
    {"shocking", WEAPON_SHOCKING, TRUE, 2},
    {"poison", WEAPON_POISON, TRUE, 3},
    {"exploding", WEAPON_EXPLOSIVE, TRUE, 3},
    {NULL, 0, 0}
};

const struct flag_type res_flags[] = {
    {"summon", RES_SUMMON, TRUE},
    {"charm", RES_CHARM, TRUE},
    {"magic", RES_MAGIC, TRUE},
    {"weapon", RES_WEAPON, TRUE},
    {"bash", RES_BASH, TRUE},
    {"pierce", RES_PIERCE, TRUE},
    {"slash", RES_SLASH, TRUE},
    {"fire", RES_FIRE, TRUE},
    {"cold", RES_COLD, TRUE},
    {"lightning", RES_LIGHTNING, TRUE},
    {"acid", RES_ACID, TRUE},
    {"poison", RES_POISON, TRUE},
    {"negative", RES_NEGATIVE, TRUE},
    {"holy", RES_HOLY, TRUE},
    {"energy", RES_ENERGY, TRUE},
    {"mental", RES_MENTAL, TRUE},
    {"disease", RES_DISEASE, TRUE},
    {"drowning", RES_DROWNING, TRUE},
    {"light", RES_LIGHT, TRUE},
    {"sound", RES_SOUND, TRUE},
    {"wood", RES_WOOD, TRUE},
    {"silver", RES_SILVER, TRUE},
    {"iron", RES_IRON, TRUE},
    {"bomb", RES_BOMB, TRUE},
    {"arrow", RES_ARROW, TRUE},
    {NULL, 0, 0}
};


const struct flag_type vuln_flags[] = {
    {"summon", VULN_SUMMON, TRUE},
    {"charm", VULN_CHARM, TRUE},
    {"magic", VULN_MAGIC, TRUE},
    {"weapon", VULN_WEAPON, TRUE},
    {"bash", VULN_BASH, TRUE},
    {"pierce", VULN_PIERCE, TRUE},
    {"slash", VULN_SLASH, TRUE},
    {"fire", VULN_FIRE, TRUE},
    {"cold", VULN_COLD, TRUE},
    {"lightning", VULN_LIGHTNING, TRUE},
    {"acid", VULN_ACID, TRUE},
    {"poison", VULN_POISON, TRUE},
    {"negative", VULN_NEGATIVE, TRUE},
    {"holy", VULN_HOLY, TRUE},
    {"energy", VULN_ENERGY, TRUE},
    {"mental", VULN_MENTAL, TRUE},
    {"disease", VULN_DISEASE, TRUE},
    {"drowning", VULN_DROWNING, TRUE},
    {"light", VULN_LIGHT, TRUE},
    {"sound", VULN_SOUND, TRUE},
    {"wood", VULN_WOOD, TRUE},
    {"silver", VULN_SILVER, TRUE},
    {"iron", VULN_IRON, TRUE},
    {"bomb", VULN_BOMB, TRUE},
    {"arrow", VULN_ARROW, TRUE},
    {NULL, 0, 0}
};

const struct flag_type position_flags[] = {
    {"dead", POS_DEAD, FALSE},
    {"mortal", POS_MORTAL, FALSE},
    {"incap", POS_INCAP, FALSE},
    {"stunned", POS_STUNNED, FALSE},
    {"sleeping", POS_SLEEPING, TRUE},
    {"resting", POS_RESTING, TRUE},
    {"sitting", POS_SITTING, TRUE},
    {"fighting", POS_FIGHTING, FALSE},
    {"standing", POS_STANDING, TRUE},
    {NULL, 0, 0}
};

const struct flag_type portal_flags[] = {
    {"normal_exit", GATE_NORMAL_EXIT, TRUE},
    {"no_curse", GATE_NOCURSE, TRUE},
    {"go_with", GATE_GOWITH, TRUE},
    {"buggy", GATE_BUGGY, TRUE},
    {"random", GATE_RANDOM, TRUE},
    {NULL, 0, 0}
};

const struct flag_type ingredient_flags[] = {
    {"sulfur", ING_SULFUR, TRUE},
    {"charcol", ING_CHARCOL, TRUE},
    {"salt peter", ING_SALT_PETER, TRUE},
    {"pipe", ING_PIPE, TRUE},
    {"manure", ING_MANEUR, TRUE},
    {"ammonia", ING_AMMONIA, TRUE},
    {"ice", ING_ICE, TRUE},
    {"secret water", ING_SECRET_WATER, TRUE},
    {"blubber", ING_BLUBBER, TRUE},
    {"sleeping powder", ING_SLEEPING_POWDER, TRUE},
    {"oil", ING_OIL, TRUE},
    {"poison powder", ING_POISON_POWDER, TRUE},
    {"sugar", ING_SUGAR, TRUE},
    {NULL, 0, 0}
};

const struct flag_type trigger_flags[] = {
    {"get", TRG_GET, TRUE},
    {"enter room", TRG_ENTER_ROOM, TRUE},
    {"look at", TRG_LOOK_AT, TRUE},
    {"open", TRG_OPEN, TRUE},
    {"contact", TRG_CONTACT, TRUE},
    {NULL, 0, 0}
};

const struct flag_type state_flags[] = {
    {"set", STATE_SET, TRUE},
    {"not set", STATE_NOT_SET, TRUE},
    {"unstable", STATE_UNSTABLE, TRUE},
    {"wtf", STATE_WTF, TRUE},
    {NULL, 0, 0}
};

const struct flag_type furniture_flags[] = {
    {"stand_at", STAND_AT, TRUE},
    {"stand_on", STAND_ON, TRUE},
    {"stand_in", STAND_IN, TRUE},
    {"sit_at", SIT_AT, TRUE},
    {"sit_on", SIT_ON, TRUE},
    {"sit_in", SIT_IN, TRUE},
    {"rest_at", REST_AT, TRUE},
    {"rest_on", REST_ON, TRUE},
    {"rest_in", REST_IN, TRUE},
    {"sleep_at", SLEEP_AT, TRUE},
    {"sleep_on", SLEEP_ON, TRUE},
    {"sleep_in", SLEEP_IN, TRUE},
    {"put_at", PUT_AT, TRUE},
    {"put_on", PUT_ON, TRUE},
    {"put_in", PUT_IN, TRUE},
    {"put_inside", PUT_INSIDE, TRUE},
    {"meditate_at", MEDITATE_AT, TRUE},
    {"meditate_on", MEDITATE_ON, TRUE},
    {"meditate_in", MEDITATE_IN, TRUE},
    {NULL, 0, 0}
};

const struct flag_type apply_types[] = {
    {"affects", TO_AFFECTS, TRUE},
    {"object", TO_OBJECT, TRUE},
    {"immune", TO_IMMUNE, TRUE},
    {"resist", TO_RESIST, TRUE},
    {"vuln", TO_VULN, TRUE},
    {"weapon", TO_WEAPON, TRUE},
    {NULL, 0, TRUE}
};

const struct bit_type bitvector_type[] = {
    {affect_flags, "affect"},
    {apply_flags, "apply"},
    {imm_flags, "imm"},
    {res_flags, "res"},
    {vuln_flags, "vuln"},
    {weapon_type2, "weapon"}
};

const struct flag_type oprog_flags[] =
{
    {   "act",                  TRIG_ACT,               TRUE    },
    {   "fight",                TRIG_FIGHT,             TRUE    },
    {   "give",                 TRIG_GIVE,              TRUE    },
    {   "greet",                TRIG_GRALL,             TRUE    },
    {   "grall",                TRIG_GRALL,             TRUE    },
    {   "random",               TRIG_RANDOM,            TRUE    },
    {   "speech",               TRIG_SPEECH,            TRUE    },
    {   "exall",                TRIG_EXALL,             TRUE    },
    {   "delay",                TRIG_DELAY,             TRUE    },   
    {   "drop",         TRIG_DROP,              TRUE    },
    {   "get",          TRIG_GET,               TRUE    },
    {   "sit",          TRIG_SIT,               TRUE    },
    {   NULL,                   0,                      TRUE    },
};
        
const struct flag_type rprog_flags[] =
{
    {   "act",                  TRIG_ACT,               TRUE    },
    {   "fight",                TRIG_FIGHT,             TRUE    },
    {   "drop",                 TRIG_DROP,              TRUE    },
    {   "greet",                TRIG_GRALL,             TRUE    },
    {   "grall",                TRIG_GRALL,             TRUE    },
    {   "random",               TRIG_RANDOM,            TRUE    },
    {   "speech",               TRIG_SPEECH,            TRUE    },
    {   "exall",                TRIG_EXALL,             TRUE    },
    {   "delay",                TRIG_DELAY,             TRUE    },
    {   NULL,                   0,                      TRUE    },
};                                     
