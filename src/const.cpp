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
*    ROM 2.4 is copyright 1993-1998 Russ Taylor               *
*    ROM has been brought to you by the ROM consortium           *
*        Russ Taylor (rtaylor@hypercube.org)                   *
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
#include "magic.h"
#include "interp.h"
#include "bomb.h"


/* Stance table name */
const   struct stance_type      stance_table       [] =
{
 { "Normal",           STANCE_NONE },
 { "Whirlwind",        STANCE_WHIRLWIND },
 { "Leaf",             STANCE_LEAF },
 { "Lightning",        STANCE_LIGHTNING },
 { "Straw",            STANCE_STRAW },
 { "Stone",            STANCE_STONE },
 { "Cat",              STANCE_CAT },
 { "Offensive",	       STANCE_OFFENSIVE },
 { "Defensive",        STANCE_DEFENSIVE },
 { NULL,              0 }
};

/* item type list */
const struct item_type item_table[] = {
    {ITEM_LIGHT, "light"},
    {ITEM_SCROLL, "scroll"},
    {ITEM_WAND, "wand"},
    {ITEM_STAFF, "staff"},
    {ITEM_WEAPON, "weapon"},
    {ITEM_TREASURE, "treasure"},
    {ITEM_ARMOR, "armor"},
    {ITEM_POTION, "potion"},
    {ITEM_CLOTHING, "clothing"},
    {ITEM_FURNITURE, "furniture"},
    {ITEM_TRASH, "trash"},
    {ITEM_CONTAINER, "container"},
    {ITEM_DRINK_CON, "drink"},
    {ITEM_KEY, "key"},
    {ITEM_FOOD, "food"},
    {ITEM_MONEY, "money"},
    {ITEM_BOAT, "boat"},
    {ITEM_CORPSE_NPC, "npc_corpse"},
    {ITEM_CORPSE_PC, "pc_corpse"},
    {ITEM_FOUNTAIN, "fountain"},
    {ITEM_PILL, "pill"},
    {ITEM_PROTECT, "protect"},
    {ITEM_MAP, "map"},
    {ITEM_PORTAL, "portal"},
    {ITEM_WARP_STONE, "warp_stone"},
    {ITEM_ROOM_KEY, "room_key"},
    {ITEM_GEM, "gem"},
    {ITEM_JEWELRY, "jewelry"},
    {ITEM_JUKEBOX, "jukebox"},
    {ITEM_TOOL, "tool"},
    {ITEM_RESOURCE, "resource"},
    {ITEM_CREATURE, "creature"},
    {ITEM_BLOOD, "blood"},
    {ITEM_WINDOW, "window"},
    {ITEM_FUEL, "fuel"},
    {ITEM_IGNITER, "igniter"},
    {ITEM_FIRE, "fire"},
    {ITEM_QUIVER,"quiver"},
    {ITEM_ARROW,"arrow"},
    {ITEM_BOMB,"bomb"},
    {ITEM_INGREDIENT,"ingredient"},
    {ITEM_BOOK, "book"},
    {ITEM_TOKEN, "token"},
    {ITEM_KEYRING,"keyring"},
    {ITEM_PAPER,"paper"},
    {ITEM_PEN,"pen"},
    {ITEM_SEAL,"seal"},
    {ITEM_WAX,"wax"},
    {ITEM_ENVELOPE,"envelope"},
    {ITEM_TRAP,"trap"},
    {ITEM_PIPE_TABAC,"pipetabac"},
    {0, NULL}
};

const struct explosive_type explosive_table[] = {
     {EXP_NONE,      "none",  bomb_null},
     {EXP_EXPLOSION, "bomb_explosion",  bomb_explosion},
     {EXP_SLEEPER, "bomb_sleeper",  bomb_sleeper},
     {EXP_CONTACT, "bomb_contact",  bomb_contact},
     {EXP_STICKY, "bomb_sticky",  bomb_sticky},
     {EXP_POISON, "bomb_poison", bomb_poison},
     {EXP_SMOKE, "bomb_smoke", bomb_smoke},
     { 0, NULL },
};  

/* weapon selection table */
const struct weapon_type weapon_table[] = {
    {"sword", OBJ_VNUM_SCHOOL_SWORD, WEAPON_SWORD, &gsn_sword},
    {"mace", OBJ_VNUM_SCHOOL_MACE, WEAPON_MACE, &gsn_mace},
    {"dagger", OBJ_VNUM_SCHOOL_DAGGER, WEAPON_DAGGER, &gsn_dagger},
    {"axe", OBJ_VNUM_SCHOOL_AXE, WEAPON_AXE, &gsn_axe},
    {"spear", OBJ_VNUM_SCHOOL_SPEAR, WEAPON_SPEAR, &gsn_spear},
    {"flail", OBJ_VNUM_SCHOOL_FLAIL, WEAPON_FLAIL, &gsn_flail},
    {"whip", OBJ_VNUM_SCHOOL_WHIP, WEAPON_WHIP, &gsn_whip},
    {"polearm", OBJ_VNUM_SCHOOL_POLEARM, WEAPON_POLEARM, &gsn_polearm},
    {"bow", OBJ_VNUM_SCHOOL_BOW, WEAPON_BOW, &gsn_bow},
    {"staff", OBJ_VNUM_SCHOOL_STAFF, WEAPON_STAFF, &gsn_staff},
    {NULL, 0, 0, NULL}
};



/* wiznet table and prototype for future flag setting */
const struct wiznet_type wiznet_table[] = {
    {"on", WIZ_ON, IM},
    {"prefix", WIZ_PREFIX, IM},
    {"ticks", WIZ_TICKS, IM},
    {"logins", WIZ_LOGINS, IM},
    {"sites", WIZ_SITES, L2},
    {"links", WIZ_LINKS, L2},
    {"newbies", WIZ_NEWBIE, IM},
    {"spam", WIZ_SPAM, L3},
    {"deaths", WIZ_DEATHS, IM},
    {"resets", WIZ_RESETS, L3},
    {"mobdeaths", WIZ_MOBDEATHS, L3},
    {"flags", WIZ_FLAGS, L3},
    {"penalties", WIZ_PENALTIES, L3},
    {"saccing", WIZ_SACCING, L3},
    {"levels", WIZ_LEVELS, IM},
    {"load", WIZ_LOAD, L2},
    {"restore", WIZ_RESTORE, L2},
    {"snoops", WIZ_SNOOPS, L2},
    {"switches", WIZ_SWITCHES, L2},
    {"secure", WIZ_SECURE, L1},
    {"rp", WIZ_RP,IM},
    {"pvp", WIZ_PVP, IM},
    {"mem", WIZ_MEMCHECK,ML},
    {NULL, 0, 0}
};

/* attack table  -- not very organized :( */
const struct attack_type attack_table[MAX_DAMAGE_MESSAGE] = {
    {"none", "hit", -1},        /*  0 */
    {"slice", "slice", DAM_SLASH},
    {"stab", "stab", DAM_PIERCE},
    {"slash", "slash", DAM_SLASH},
    {"whip", "whip", DAM_SLASH},
    {"claw", "claw", DAM_SLASH},    /*  5 */
    {"blast", "blast", DAM_BASH},
    {"pound", "pound", DAM_BASH},
    {"crush", "crush", DAM_BASH},
    {"grep", "grep", DAM_SLASH},
    {"bite", "bite", DAM_PIERCE},    /* 10 */
    {"pierce", "pierce", DAM_PIERCE},
    {"suction", "suction", DAM_BASH},
    {"beating", "beating", DAM_BASH},
    {"digestion", "digestion", DAM_ACID},
    {"charge", "charge", DAM_BASH},    /* 15 */
    {"slap", "slap", DAM_BASH},
    {"punch", "punch", DAM_BASH},
    {"wrath", "wrath", DAM_ENERGY},
    {"magic", "magic", DAM_ENERGY},
    {"divine", "divine power", DAM_HOLY},    /* 20 */
    {"cleave", "cleave", DAM_SLASH},
    {"scratch", "scratch", DAM_PIERCE},
    {"peck", "peck", DAM_PIERCE},
    {"peckb", "peck", DAM_BASH},
    {"chop", "chop", DAM_SLASH},    /* 25 */
    {"sting", "sting", DAM_PIERCE},
    {"smash", "smash", DAM_BASH},
    {"shbite", "shocking bite", DAM_LIGHTNING},
    {"flbite", "flaming bite", DAM_FIRE},
    {"frbite", "freezing bite", DAM_COLD},    /* 30 */
    {"acbite", "acidic bite", DAM_ACID},
    {"chomp", "chomp", DAM_PIERCE},
    {"drain", "life drain", DAM_NEGATIVE},
    {"thrust", "thrust", DAM_PIERCE},
    {"slime", "slime", DAM_ACID},   /* 35 */
    {"shock", "shock", DAM_LIGHTNING},
    {"thwack", "thwack", DAM_BASH},
    {"flame", "flame", DAM_FIRE},
    {"chill", "chill", DAM_COLD},
    {"blood", "power of Hawking's Descendants", DAM_ENERGY}, /* 40 */
    {"firestorm", "earth and fire", DAM_FIRE},
    {"dome", "performance", DAM_ENERGY},
    {"cleanse", "inquisition", DAM_ENERGY},
    {"prayer", "prayer", DAM_ENERGY},
    {"critical", "`&critical hit`*", DAM_SLASH}, /* 45 */
    {"explosion", "`&explosion`*", DAM_FIRE},
    {"bond", "`&bond`*", DAM_ENERGY}, /* 47 */ 
    {"prick", "`!prick`*", DAM_PIERCE},
    {NULL, NULL, 0}
};
//Do not exceed 39 on martial this table
const struct mar_attack_type mar_attack_table[MAX_MAR_DAM_MSG] = {
    {"jab",DAM_BASH,20,40},// 0
    {"knee",DAM_BASH,30,50},
    {"round house",DAM_BASH,70,100},
    {"chop",DAM_BASH,0,50},
    {"open palm",DAM_BASH,50,70},
    {"elbow strike",DAM_BASH,60,90},// 5
    {"snap kick",DAM_BASH,40,70},
    {"back fist",DAM_BASH,40,70},
    {"uppercut",DAM_BASH,70,110},
    {"ridgehand",DAM_BASH,30,50},
    {"crescent kick",DAM_BASH,40,70},// 10
    {"tiger knuckle",DAM_BASH,50,80},
    {"surplice slash",DAM_BASH,40,80},
    {"thunder kick",DAM_BASH,50,80},
    {"twist knuckle",DAM_BASH,30,60},
    {"head-butt",DAM_BASH,40,80},// 15
    {"side kick",DAM_BASH,70,120},
    {NULL,0,0,0}
};
const struct throw_attack_type throw_attack_table[MAX_MAR_THROW_MSG] = {
    {"vortex throw",DAM_BASH}, // 0
    {"mist reaper",DAM_BASH},
    {"dema drop",DAM_BASH},
    {"shoulder buster",DAM_BASH},
    {"back-twist drop",DAM_BASH},
    {NULL,0}
};


/* race table */
const struct race_type race_table[] = {
/*
    {
    name,        pc_race?,
    act bits,    aff_by bits,    off bits,
    imm,        res,        vuln,
    form,        parts 
    },
*/
    {"unique", FALSE, 0, {0, 0, 0, 0}, 0, 0, 0, 0, 0, 0},


    {
     "arafel", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "shienar", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "cairhien", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "arad_domon", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "tear", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
    	
     "saldaea", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
    
    	
     "murandy", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
    	"tarabon", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
    	"kandor", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
    	"amadicia", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
    	"ghealdan", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
    	"altara", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
    	"andor", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
    	"illian", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
    	"seanchan", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
    	"aiel", TRUE,
     0, {1024, 0, 0, 0}, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "giant", FALSE,
     0, {0, 0, 0, 0}, 0,
     0, RES_FIRE | RES_COLD, VULN_MENTAL | VULN_LIGHTNING,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "bat", FALSE,
     0, {0, 1040, 0, 0}, OFF_DODGE | OFF_FAST,
     0, 0, VULN_LIGHT,
     A | G | V, A | C | D | E | F | H | J | K | P},

    {
     "bear", FALSE,
     0, {0, 0, 0, 0}, OFF_CRUSH | OFF_DISARM | OFF_BERSERK,
     0, RES_BASH | RES_COLD, 0,
     A | G | V, A | B | C | D | E | F | H | J | K | U | V},

    {
     "cat", FALSE,
     0, {0, 1024, 0, 0}, OFF_FAST | OFF_DODGE,
     0, 0, 0,
     A | G | V, A | C | D | E | F | H | J | K | Q | U | V},

    {
     "centipede", FALSE,
     0, {0, 1024, 0, 0}, 0,
     0, RES_PIERCE | RES_COLD, VULN_BASH,
     A | B | G | O, A | C | K},

    {
     "dog", FALSE,
     0, {0, 0, 0, 0}, OFF_FAST,
     0, 0, 0,
     A | G | V, A | C | D | E | F | H | J | K | U | V},

    {
     "doll", FALSE,
     0, {0, 0, 0, 0}, 0,
     IMM_COLD | IMM_POISON | IMM_HOLY | IMM_NEGATIVE | IMM_MENTAL |
     IMM_DISEASE | IMM_DROWNING, RES_BASH | RES_LIGHT,
     VULN_SLASH | VULN_FIRE | VULN_ACID | VULN_LIGHTNING | VULN_ENERGY,
     E | J | M | cc, A | B | C | G | H | K},

    {"dragon", FALSE,
     0, {1024, 16, 0, 0}, 0,
     0, RES_FIRE | RES_BASH | RES_CHARM,
     VULN_PIERCE | VULN_COLD,
     A | H | Z, A | C | D | E | F | G | H | I | J | K | P | Q | U | V | X},

    {
     "fido", FALSE,
     0, {0, 0, 0, 0}, OFF_DODGE | ASSIST_RACE,
     0, 0, VULN_MAGIC,
     A | B | G | V, A | C | D | E | F | H | J | K | Q | V},

    {
     "fox", FALSE,
     0, {0, 1024, 0, 0}, OFF_FAST | OFF_DODGE,
     0, 0, 0,
     A | G | V, A | C | D | E | F | H | J | K | Q | V},

    {
     "goblin", FALSE,
     0, {1024, 0, 0, 0}, 0,
     0, RES_DISEASE, VULN_MAGIC,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "hobgoblin", FALSE,
     0, {1024, 0, 0, 0}, 0,
     0, RES_DISEASE | RES_POISON, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K | Y},

    {
     "kobold", FALSE,
     0, {1024, 0, 0, 0}, 0,
     0, RES_POISON, VULN_MAGIC,
     A | B | H | M | V, A | B | C | D | E | F | G | H | I | J | K | Q},

    {
     "lizard", FALSE,
     0, {0, 0, 0, 0}, 0,
     0, RES_POISON, VULN_COLD,
     A | G | X | cc, A | C | D | E | F | H | K | Q | V},

    {
     "modron", FALSE,
     0, {1024, 0, 0, 0}, ASSIST_RACE | ASSIST_ALIGN,
     IMM_CHARM | IMM_DISEASE | IMM_MENTAL | IMM_HOLY | IMM_NEGATIVE,
     RES_FIRE | RES_COLD | RES_ACID, 0,
     H, A | B | C | G | H | J | K},

    {
     "orc", FALSE,
     0, {1024, 0, 0, 0}, 0,
     0, RES_DISEASE, VULN_LIGHT,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "pig", FALSE,
     0, {0, 0, 0, 0}, 0,
     0, 0, 0,
     A | G | V, A | C | D | E | F | H | J | K},

    {
     "rabbit", FALSE,
     0, {0, 0, 0, 0}, OFF_DODGE | OFF_FAST,
     0, 0, 0,
     A | G | V, A | C | D | E | F | H | J | K},

    {
     "school monster", FALSE,
     ACT_NOALIGN, {0, 0, 0, 0}, 0,
     IMM_CHARM | IMM_SUMMON, 0, VULN_MAGIC,
     A | M | V, A | B | C | D | E | F | H | J | K | Q | U},

    {
     "snake", FALSE,
     0, {0, 0, 0, 0}, 0,
     0, RES_POISON, VULN_COLD,
     A | G | X | Y | cc, A | D | E | F | K | L | Q | V | X},

    {
     "song bird", FALSE,
     0, {0, 16, 0, 0}, OFF_FAST | OFF_DODGE,
     0, 0, 0,
     A | G | W, A | C | D | E | F | H | K | P},

    {
     "troll", FALSE,
     0, {1088, 8192, 0, 0},
     OFF_BERSERK,
     0, RES_CHARM | RES_BASH, VULN_FIRE | VULN_ACID,
     A | B | H | M | V, A | B | C | D | E | F | G | H | I | J | K | U | V},

    {
     "water fowl", FALSE,
     0, {0,  4112, 0, 0}, 0,
     0, RES_DROWNING, 0,
     A | G | W, A | C | D | E | F | H | K | P},

    {
     "wolf", FALSE,
     0, {0, 1024, 0, 0}, OFF_FAST | OFF_DODGE,
     0, 0, 0,
     A | G | V, A | C | D | E | F | J | K | Q | V},

    {
    	
     "damane", FALSE,
     0, {0, 1024, 0, 0}, OFF_FAST | OFF_DODGE,
     0, 0, 0,
     A | G | V, A | C | D | E | F | J | K | Q | V},

    {
     "wyvern", FALSE,
     0, {80, 16, 0, 0},
     OFF_BASH | OFF_FAST | OFF_DODGE,
     IMM_POISON, 0, VULN_LIGHT,
     A | B | G | Z, A | C | D | E | F | H | J | K | Q | V | X},

    {
     "unique", FALSE,
     0, {0, 0, 0, 0}, 0,
     0, 0, 0,
     0, 0},


    {
     NULL, 0, 0, {0, 0, 0, 0}, 0, 0, 0, 0, 0}
};

const struct pc_race_type pc_race_table[] = {
    {"null race", "", 0, {100, 100, 100, 100},
     {""}, {13, 13, 13, 13}, {18, 18, 18, 18}, 0},

/*
    {
    "race name",     short name,     points,    { class multipliers },
    { bonus skills },
    { base stats },        { max stats },        size, { mod stats } 
    },
*/
    {
     "arafel", "Araf", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {5, 4, 1, 3, 2}
     },

    {
     "shienar", "Shie", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {5, 2, 3, 2, 3}
    },

    {     "cairhien", "Cair", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {4, 1, 2, 3, 5}
},
{
     "arad_domon", "Arad", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {5, 3, 2, 3, 2}
},
{
     "tear", "Tear", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {3, 3, 3, 3, 3}
},
{
     "saldaea", "Sald", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {3, 4, 3, 4, 1}
},
{
     "murandy", "Mura", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {3, 5, 2, 3, 2}
},
{
     "tarabon", "Tara", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {2, 5, 3, 3, 2}
},
{
     "kandor", "Kand", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {1, 3, 5, 1, 5}
},
{
     "amadicia", "Amad", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {3, 2, 4, 1, 5}
},
{
     "ghealdan", "Ghea", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {1, 2, 4, 5, 3}
},
{
     "altara", "Alta", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {2, 1, 5, 5, 2}
},
{
     "andor", "Ando", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {1, 5, 5, 2, 2}
},
{
     "illian", "Illi", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {2, 3, 1, 4, 5}
},
{
     "seanchan", "Sean", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {4, 4, 3, 2, 2}
},
{
     "aiel", "Aiel", 0, {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
     {""},
     {5, 3, 0, 2, 0}, {5, 3, 0, 2, 0}, SIZE_MEDIUM, {3, 4, 1, 2, 5}
},


};




/*
 * Class table.
 */
const struct class_type class_table[MAX_CLASS] = {
    {
     "mage", "Channeler", STAT_INT, OBJ_VNUM_SCHOOL_DAGGER,
     {3018, 9618}, 75, 30, 15, 8, 10, 12, 20, FMANA_CHANNEL,
     "mage basics", "mage default",
     {16, 21, 19, 17, 17},
     {5, 4, 5, 5, 6}, //weaves - five power	
     },//0

     {
     "cleric", "Healer   ", STAT_WIS, OBJ_VNUM_SCHOOL_MACE,
     {3003, 9619}, 75, 25, 12, 10, 12, 9, 17,  FMANA_CHANNEL,
     "cleric basics", "cleric default",
     {17, 18, 20, 17, 18},
     {3, 3, 7, 7, 5}, 
     },//1

    {
     "assassin", "Assassin ", STAT_DEX, OBJ_VNUM_SCHOOL_DAGGER,
     {3028, 9639}, 75, 20, 5, 15, 20, 4, 5, FMANA_FIGHTER,
     "assassin basics", "assassin default",
     {18, 16, 17, 20, 19},
     {6, 4, 6, 4, 5},
     },//2

    {
     "warrior", "Warrior  ", STAT_STR, OBJ_VNUM_SCHOOL_SWORD,
     {3022, 9633}, 75, 20, -4, 19, 29, 4, 5, FMANA_FIGHTER,
     "warrior basics", "warrior default",
     {20, 16, 16, 16, 22},
     {5, 7, 4, 5, 4},
     },//3

     {
     "archer", "Archer   ", STAT_DEX, OBJ_VNUM_SCHOOL_BOW,
     {3022, 9633}, 75, 20, 2, 12, 22, 4, 5, FMANA_FIGHTER,
     "archer basics", "archer default",
     {19, 17, 17, 20, 17},
     {6, 1, 10, 3, 5}, 
     },//4
     {
     "rogue", "Rogue    ", STAT_DEX, OBJ_VNUM_SCHOOL_DAGGER,
     {3028, 9639}, 75, 20, 8, 14, 18, 4, 5, FMANA_FIGHTER,
     "rogue basics", "rogue default",
     {18, 17, 16, 21, 18},
     {4, 6, 5, 4, 6},
     },//5
     
     {
     "illuminator", "Illumina ", STAT_INT, OBJ_VNUM_SCHOOL_DAGGER,
     {3028, 9639}, 75, 20, 10, 11, 16, 5, 6, FMANA_FIGHTER,
     "illuminator basics", "illuminator default",
     {18, 20, 18, 18, 16},
     {5, 6, 5, 5, 4},
     },//6
     
     {
     "form master", "Form Mas ", STAT_CON, OBJ_VNUM_SCHOOL_DAGGER,
     {3022, 9633}, 75, 20, 7, 19, 26, 4, 5, FMANA_FIGHTER,
     "form master basics", "form master default",
     {19, 16, 16, 20, 19},
     {3, 8, 8, 3, 3},
     },//7

     {
     "thief catcher", "ThiefCat ", STAT_CON, OBJ_VNUM_SCHOOL_DAGGER,
     {3022, 9633}, 75, 20, -6, 12, 18, 4, 5, FMANA_FIGHTER,
     "thief catcher basics", "thief catcher default",
     {18, 17, 16, 20, 19},
     {5, 5, 5, 5, 5,},
     },//8
 
     {
      "forsaken", "Forsaken ", STAT_INT, OBJ_VNUM_SCHOOL_DAGGER,
      {331}, 75, 20, 2, 28, 28, 18, 20, FMANA_BOTH,
     "forsaken basics", "forsaken default",
     {20, 20, 20, 20, 20},
     {6, 6, 6, 6, 6}, 
     },//9
     
     {
      "Dragon", "Dragon   ", STAT_WIS, OBJ_VNUM_SCHOOL_SWORD,
      {331}, 80, 20, 2, 28, 28, 12, 20, FMANA_BOTH,
     "dragon basics", "dragon default",
     {20, 20, 20, 20, 20},
     {6, 6, 6, 6, 6},
     },//10
     
     {
     "herbalist", "Herbal   ", STAT_WIS, OBJ_VNUM_SCHOOL_STAFF,
     {3028, 9639}, 75, 20, 4, 11, 16, 5, 6, FMANA_CHANNEL,
     "mage basics", "mage default",
     {17, 20, 19, 17, 18},
     {5, 5, 5, 5, 5},
     }//11
     
     
};




/*
 * Titles.
 */
char *const title_table[MAX_CLASS][MAX_LEVEL + 1][2] = {
    {
     {"Man", "Woman"},

     {"Apprentice of Magic", "Apprentice of Magic"},
     {"Spell Student", "Spell Student"},
     {"Scholar of Magic", "Scholar of Magic"},
     {"Delver in Spells", "Delveress in Spells"},
     {"Medium of Magic", "Medium of Magic"},

     {"Scribe of Magic", "Scribess of Magic"},
     {"Seer", "Seeress"},
     {"Sage", "Sage"},
     {"Illusionist", "Illusionist"},
     {"Abjurer", "Abjuress"},

     {"Invoker", "Invoker"},
     {"Enchanter", "Enchantress"},
     {"Conjurer", "Conjuress"},
     {"Magician", "Witch"},
     {"Creator", "Creator"},

     {"Savant", "Savant"},
     {"Magus", "Craftess"},
     {"Wizard", "Wizard"},
     {"Warlock", "War Witch"},
     {"Sorcerer", "Sorceress"},

     {"Elder Sorcerer", "Elder Sorceress"},
     {"Grand Sorcerer", "Grand Sorceress"},
     {"Great Sorcerer", "Great Sorceress"},
     {"Golem Maker", "Golem Maker"},
     {"Greater Golem Maker", "Greater Golem Maker"},

     {"Maker of Stones", "Maker of Stones",},
     {"Maker of Potions", "Maker of Potions",},
     {"Maker of Scrolls", "Maker of Scrolls",},
     {"Maker of Wands", "Maker of Wands",},
     {"Maker of Staves", "Maker of Staves",},

     {"Demon Summoner", "Demon Summoner"},
     {"Greater Demon Summoner", "Greater Demon Summoner"},
     {"Dragon Charmer", "Dragon Charmer"},
     {"Greater Dragon Charmer", "Greater Dragon Charmer"},
     {"Master of all Magic", "Master of all Magic"},

     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},

     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},

     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},
     {"Master Mage", "Master Mage"},

     {"Mage Hero", "Mage Heroine"},
     {"Avatar of Magic", "Avatar of Magic"},
     {"Angel of Magic", "Angel of Magic"},
     {"Demigod of Magic", "Demigoddess of Magic"},
     {"Immortal of Magic", "Immortal of Magic"},
     {"God of Magic", "Goddess of Magic"},
     {"Deity of Magic", "Deity of Magic"},
     {"Supremity of Magic", "Supremity of Magic"},
     {"Creator", "Creator"},
     {"Implementor", "Implementress"}
     },

    {
     {"Man", "Woman"},

     {"Believer", "Believer"},
     {"Attendant", "Attendant"},
     {"Acolyte", "Acolyte"},
     {"Novice", "Novice"},
     {"Missionary", "Missionary"},

     {"Adept", "Adept"},
     {"Deacon", "Deaconess"},
     {"Vicar", "Vicaress"},
     {"Priest", "Priestess"},
     {"Minister", "Lady Minister"},

     {"Canon", "Canon"},
     {"Levite", "Levitess"},
     {"Curate", "Curess"},
     {"Monk", "Nun"},
     {"Healer", "Healess"},

     {"Chaplain", "Chaplain"},
     {"Expositor", "Expositress"},
     {"Bishop", "Bishop"},
     {"Arch Bishop", "Arch Lady of the Church"},
     {"Patriarch", "Matriarch"},

     {"Elder Patriarch", "Elder Matriarch"},
     {"Grand Patriarch", "Grand Matriarch"},
     {"Great Patriarch", "Great Matriarch"},
     {"Demon Killer", "Demon Killer"},
     {"Greater Demon Killer", "Greater Demon Killer"},

     {"Cardinal of the Sea", "Cardinal of the Sea"},
     {"Cardinal of the Earth", "Cardinal of the Earth"},
     {"Cardinal of the Air", "Cardinal of the Air"},
     {"Cardinal of the Ether", "Cardinal of the Ether"},
     {"Cardinal of the Heavens", "Cardinal of the Heavens"},

     {"Avatar of an Immortal", "Avatar of an Immortal"},
     {"Avatar of a Deity", "Avatar of a Deity"},
     {"Avatar of a Supremity", "Avatar of a Supremity"},
     {"Avatar of an Implementor", "Avatar of an Implementor"},
     {"Master of all Divinity", "Mistress of all Divinity"},

     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},

     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},

     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},
     {"Master Cleric", "Master Cleric"},

     {"Holy Hero", "Holy Heroine"},
     {"Holy Avatar", "Holy Avatar"},
     {"Angel", "Angel"},
     {"Demigod", "Demigoddess",},
     {"Immortal", "Immortal"},
     {"God", "Goddess"},
     {"Deity", "Deity"},
     {"Supreme Master", "Supreme Mistress"},
     {"Creator", "Creator"},
     {"Implementor", "Implementress"}
     },

    {
     {"Man", "Woman"},

     {"Pilferer", "Pilferess"},
     {"Footpad", "Footpad"},
     {"Filcher", "Filcheress"},
     {"Pick-Pocket", "Pick-Pocket"},
     {"Sneak", "Sneak"},

     {"Pincher", "Pincheress"},
     {"Cut-Purse", "Cut-Purse"},
     {"Snatcher", "Snatcheress"},
     {"Sharper", "Sharpress"},
     {"Rogue", "Rogue"},

     {"Robber", "Robber"},
     {"Magsman", "Magswoman"},
     {"Highwayman", "Highwaywoman"},
     {"Burglar", "Burglaress"},
     {"Thief", "Thief"},

     {"Knifer", "Knifer"},
     {"Quick-Blade", "Quick-Blade"},
     {"Killer", "Murderess"},
     {"Brigand", "Brigand"},
     {"Cut-Throat", "Cut-Throat"},

     {"Spy", "Spy"},
     {"Grand Spy", "Grand Spy"},
     {"Master Spy", "Master Spy"},
     {"Assassin", "Assassin"},
     {"Greater Assassin", "Greater Assassin"},

     {"Master of Vision", "Mistress of Vision"},
     {"Master of Hearing", "Mistress of Hearing"},
     {"Master of Smell", "Mistress of Smell"},
     {"Master of Taste", "Mistress of Taste"},
     {"Master of Touch", "Mistress of Touch"},

     {"Crime Lord", "Crime Mistress"},
     {"Infamous Crime Lord", "Infamous Crime Mistress"},
     {"Greater Crime Lord", "Greater Crime Mistress"},
     {"Master Crime Lord", "Master Crime Mistress"},
     {"Godfather", "Godmother"},

     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},

     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},

     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},
     {"Master Thief", "Master Thief"},

     {"Assassin Hero", "Assassin Heroine"},
     {"Avatar of Death", "Avatar of Death",},
     {"Angel of Death", "Angel of Death"},
     {"Demigod of Assassins", "Demigoddess of Assassins"},
     {"Immortal Assasin", "Immortal Assassin"},
     {"God of Assassins", "God of Assassins",},
     {"Deity of Assassins", "Deity of Assassins"},
     {"Supreme Master", "Supreme Mistress"},
     {"Creator", "Creator"},
     {"Implementor", "Implementress"}
     },

    {
     {"Man", "Woman"},

     {"Swordpupil", "Swordpupil"},
     {"Recruit", "Recruit"},
     {"Sentry", "Sentress"},
     {"Fighter", "Fighter"},
     {"Soldier", "Soldier"},

     {"Warrior", "Warrior"},
     {"Veteran", "Veteran"},
     {"Swordsman", "Swordswoman"},
     {"Fencer", "Fenceress"},
     {"Combatant", "Combatess"},

     {"Hero", "Heroine"},
     {"Myrmidon", "Myrmidon"},
     {"Swashbuckler", "Swashbuckleress"},
     {"Mercenary", "Mercenaress"},
     {"Swordmaster", "Swordmistress"},

     {"Lieutenant", "Lieutenant"},
     {"Champion", "Lady Champion"},
     {"Dragoon", "Lady Dragoon"},
     {"Cavalier", "Lady Cavalier"},
     {"Knight", "Lady Knight"},

     {"Grand Knight", "Grand Knight"},
     {"Master Knight", "Master Knight"},
     {"Paladin", "Paladin"},
     {"Grand Paladin", "Grand Paladin"},
     {"Demon Slayer", "Demon Slayer"},

     {"Greater Demon Slayer", "Greater Demon Slayer"},
     {"Dragon Slayer", "Dragon Slayer"},
     {"Greater Dragon Slayer", "Greater Dragon Slayer"},
     {"Underlord", "Underlord"},
     {"Overlord", "Overlord"},

     {"Baron of Thunder", "Baroness of Thunder"},
     {"Baron of Storms", "Baroness of Storms"},
     {"Baron of Tornadoes", "Baroness of Tornadoes"},
     {"Baron of Hurricanes", "Baroness of Hurricanes"},
     {"Baron of Meteors", "Baroness of Meteors"},

     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},

     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},

     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},
     {"Master Warrior", "Master Warrior"},

     {"Knight Hero", "Knight Heroine"},
     {"Avatar of War", "Avatar of War"},
     {"Angel of War", "Angel of War"},
     {"Demigod of War", "Demigoddess of War"},
     {"Immortal Warlord", "Immortal Warlord"},
     {"God of War", "God of War"},
     {"Deity of War", "Deity of War"},
     {"Supreme Master of War", "Supreme Mistress of War"},
     {"Creator", "Creator"},
     {"Implementor", "Implementress"}
     }
};



/*
 * Attribute bonus tables.
 */
const struct str_app_type str_app[26] = {
    {-5, -4, 0, 0},                /* 0  */
    {-5, -4, 3, 1},                /* 1  */
    {-3, -2, 3, 2},
    {-3, -1, 10, 3},            /* 3  */
    {-2, -1, 25, 4},
    {-2, -1, 55, 5},            /* 5  */
    {-1, 0, 80, 6},
    {-1, 0, 90, 7},
    {0, 0, 100, 8},
    {0, 0, 100, 9},
    {0, 0, 115, 10},            /* 10  */
    {0, 0, 115, 10},
    {0, 0, 130, 10},
    {0, 0, 130, 10},            /* 13  */
    {0, 1, 140, 14},
    {1, 1, 150, 15},            /* 15  */
    {1, 2, 165, 50},
    {3, 3, 180, 50},
    {4, 3, 200, 50},            /* 18  */
    {5, 4, 225, 50},
    {6, 5, 250, 50},            /* 20  */
    {7, 6, 300, 50},
    {8, 6, 350, 50},
    {10, 14, 400, 50},
    {10, 16, 450, 55},
    {12, 18, 500, 60}                /* 25   */
};



const struct int_app_type int_app[26] = {
    {3},                        /*  0 */
    {5},                        /*  1 */
    {7},
    {8},                        /*  3 */
    {9},
    {10},                        /*  5 */
    {11},
    {12},
    {13},
    {15},
    {17},                        /* 10 */
    {19},
    {22},
    {25},
    {28},
    {31},                        /* 15 */
    {34},
    {37},
    {40},                        /* 18 */
    {50},
    {60},                        /* 20 */
    {70},
    {80},
    {90},
    {100},
    {105}                        /* 25 */
};



const struct wis_app_type wis_app[26] = {
    {0},                        /*  0 */
    {0},                        /*  1 */
    {0},
    {0},                        /*  3 */
    {0},
    {1},                        /*  5 */
    {1},
    {1},
    {1},
    {1},
    {1},                        /* 10 */
    {1},
    {1},
    {1},
    {1},
    {2},                        /* 15 */
    {2},
    {2},
    {3},                        /* 18 */
    {3},
    {4},                        /* 20 */
    {4},
    {5},
    {5},
    {5},
    {6}                            /* 25 */
};



const struct dex_app_type dex_app[26] = {
    {60, -10},                        /* 0 */
    {50, -10},                        /* 1 */
    {50, -9},
    {40, -8},
    {30, -7},
    {20, -6},                        /* 5 */
    {10, -5},
    {0, -4},
    {0, -3},
    {0, -2},
    {0, -1},                        /* 10 */
    {0, 0},
    {0, 0},
    {0, 1},
    {0, 2},
    {-10, 2},                        /* 15 */
    {-15, 3},
    {-20, 3},
    {-30, 4},
    {-40, 4},
    {-50, 4},                        /* 20 */
    {-70, 5},
    {-85, 5},
    {-100, 5},
    {-120, 8},
    {-150, 10}                        /* 25 */
};

const struct con_app_type con_app[27] = {
    {-4, 20},                    /*  0 */
    {-3, 25},                    /*  1 */
    {-2, 30},
    {-2, 35},                    /*  3 */
    {-1, 40},
    {-1, 45},                    /*  5 */
    {-1, 50},
    {0, 55},
    {0, 60},
    {0, 65},
    {0, 70},                    /* 10 */
    {0, 75},
    {0, 80},
    {0, 85},
    {0, 88},
    {1, 90},                    /* 15 */
    {2, 95},
    {2, 97},
    {3, L2},                    /* 18 */
    {3, L2},
    {4, L2},                    /* 20 */
    {4, L2},
    {5, L2},
    {6, L2},
    {7, L2},
    {9, L2},                        /* 25 */
    {9, 99}                        /* 26 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const struct liq_type liq_table[] = {
/*    name            color    proof, full, thirst, food, ssize */
    {"water", "clear", {0, 1, 10, 0, 16}},
    {"beer", "amber", {12, 1, 8, 1, 12}},
    {"red wine", "burgundy", {30, 1, 8, 1, 5}},
    {"ale", "brown", {15, 1, 8, 1, 12}},
    {"dark ale", "dark", {16, 1, 8, 1, 12}},


    {"whisky", "golden", {120, 1, 5, 0, 2}},
    {"lemonade", "pink", {0, 1, 9, 2, 12}},
    {"firebreather", "boiling", {190, 0, 4, 0, 2}},
    {"local specialty", "clear", {151, 1, 3, 0, 2}},
    {"slime mold juice", "green", {0, 2, -8, 1, 2}},

    {"milk", "white", {0, 2, 9, 3, 12}},
    {"tea", "tan", {0, 1, 8, 0, 6}},
    {"coffee", "black", {0, 1, 8, 0, 6}},
    {"blood", "red", {0, 3, 7, 5, 6}},
    {"salt water", "clear", {0, 1, -2, 0, 1}},

    {"coke", "brown", {0, 2, 9, 2, 12}},
    {"root beer", "brown", {0, 2, 9, 2, 12}},
    {"elvish wine", "green", {35, 2, 8, 1, 5}},
    {"white wine", "golden", {28, 1, 8, 1, 5}},
    {"champagne", "golden", {32, 1, 8, 1, 5}},

    {"mead", "honey-colored", {34, 2, 8, 2, 12}},
    {"rose wine", "pink", {26, 1, 8, 1, 5}},
    {"benedictine wine", "burgundy", {40, 1, 8, 1, 5}},
    {"vodka", "clear", {130, 1, 5, 0, 2}},
    {"cranberry juice", "red", {0, 1, 9, 2, 12}},

    {"orange juice", "orange", {0, 2, 9, 3, 12}},
    {"absinthe", "green", {200, 1, 4, 0, 2}},
    {"brandy", "golden", {80, 1, 5, 0, 4}},
    {"aquavit", "clear", {140, 1, 5, 0, 2}},
    {"schnapps", "clear", {90, 1, 5, 0, 2}},

    {"icewine", "purple", {50, 2, 6, 1, 5}},
    {"amontillado", "burgundy", {35, 2, 8, 1, 5}},
    {"sherry", "red", {38, 2, 7, 1, 5}},
    {"framboise", "red", {50, 1, 7, 1, 5}},
    {"rum", "amber", {151, 1, 4, 0, 2}},

    {"cordial", "clear", {100, 1, 5, 0, 2}},
    {NULL, NULL, {0, 0, 0, 0, 0}}
};



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)    n

const struct skill_type skill_table[MAX_SKILL] = {

/*
 * Magic spells.
   // ORDER: {mage, cleric, assassin, warrior, racher, rogue, illuminator, form master, thief catcher, forsaken, dragon, herbalist}
 */

    {
     "reserved", {99, 99, 99, 99, 99, 99, 99, 99, 99, 99}, {99, 99, 99, 99, 99, 99, 99, 99, 99, L2},
     {2, 2, 2, 2, 2},
     0, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (0), 0, 0,
     "", "", ""},

    {
     "acid blast", {30, L2, L2, L2, L2, L2, L2, L2, L2, 5, 5, L2}, {1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, L2},
     {2, 2, 2, 2, 2},
     spell_acid_blast, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (70), 20, 12,
     "acid blast", "!Acid Blast!"},

    {
     "armor", {7, 2, L2, L2, L2, L2, L2, ML, L2, 3, 3, L2}, {1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, L2},
     {3, 7, 0, 0, 0},
     spell_armor, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT (1), 5, 12,
     "", "You feel less armored.", ""},
    
    {
     "air barrier", {15, ML,ML,ML,ML,ML,ML,ML,ML,10,10,ML}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, 1},
     {0, 0, 5, 0, 5},
     spell_airbarrier, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT (0), 12, 18,
     "", "The weave of air around you dissipates.", ""},

    {
     "bless", {L2, 7, L2, L2,L2,L2,L2,L2,L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 0, 0, 10},
     spell_bless, TAR_OBJ_CHAR_DEF, POS_STANDING,
     NULL, SLOT (3), 5, 12,
     "", "You feel less righteous.",
     "$p's holy aura fades."},

    {
     "blindness", {12, 8, L2, L2,L2,L2,L2,L2,L2, 9, 8, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {4, 0, 0, 4, 2},
     spell_blindness, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_blindness, SLOT (4), 5, 12,
     "", "You can see again.", ""},

    {
     "burning hands", {7, 16, L2, L2,L2,L2,L2,L2,L2, 5, 4, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {6, 4, 0, 0, 0},
     spell_burning_hands, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (5), 15, 12,
     "burning hands", "!Burning Hands!", ""},

    {
     "call lightning", {26, 26, L2, L2,L2,L2,L2,L2,L2, 15, 15, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 4, 4, 2, 0},
     spell_call_lightning, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT (6), 15, 12,
     "lightning bolt", "!Call Lightning!", ""},

    {"calm", {58, 16, L2, L2,L2,L2,L2,L2,L2, L2, 25, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, 1, L2},
     {2, 0, 0, 2, 6},
     spell_calm, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT (509), 30, 12,
     "", "You have lost your peace of mind.", ""},

    {
     "cancellation", {24, 24, 75, 75,75,75,75,75,75 ,24, 24, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {2, 2, 2, 2, 2},
     spell_cancellation, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT (507), 20, 12,
     "" "!cancellation!", ""},

    {
     "cause critical", {L2, 13, L2, L2,L2,L2, L2,L2, L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, -1, L2},
     {2, 0, 3, 2, 3},
     spell_cause_critical, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (63), 20, 12,
     "spell", "!Cause Critical!", ""},

    {
     "cause light", {L2, 1, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, -1, L2},
     {2, 0, 3, 2, 3},
     spell_cause_light, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (62), 15, 12,
     "spell", "!Cause Light!", ""},

    {
     "cause serious", {L2, 7, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, -1, L2},
     {2, 0, 3, 2, 3},
     spell_cause_serious, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (64), 17, 12,
     "spell", "!Cause Serious!", ""},

    {
     "chain lightning", {43, 69, L2, L2,L2,L2,L2,L2,L2, 38, 38, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 4, 4, 2, 0},
     spell_chain_lightning, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (500), 25, 12,
     "lightning", "!Chain Lightning!", ""},

    {
     "change sex", {L2, L2, L2, L2,L2,L2,L2,L2,L2, 20, 20, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {2, 2, 2, 2, 2},
     spell_change_sex, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT (82), 15, 12,
     "", "Your body feels familiar again.", ""},

    {
     "charm person", {23, 45, L2, L2,L2,L2,L2,L2,L2, 20, 20, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {2, 0, 0, 2, 6},
     spell_charm_person, TAR_CHAR_OFFENSIVE, POS_STANDING,
     &gsn_charm_person, SLOT (7), 5, 12,
     "", "You feel more self-confident.", ""},

    {
     "chill touch", {4, L2, L2, L2,L2,L2,L2,L2,L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 5, 5, 0},
     spell_chill_touch, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (8), 15, 12,
     "chilling touch", "You feel less cold.", ""},

    {
     "colour spray", {16, L2, L2, L2,L2,L2,L2,L2,L2, 12, 12, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {2, 2, 2, 2, 2},
     spell_colour_spray, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (10), 15, 12,
     "colour spray", "!Colour Spray!", ""},

    {
     "continual light", {6, L2, L2, L2,L2,L2,L2,L2,L2, 3, 3, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {6, 1, 1, 1, 1},
     spell_continual_light, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (57), 7, 12,
     "", "!Continual Light!", ""},

    {
     "control weather", {15, 15, L2, L2,L2, L2, 13, L2,L2,L2,L2,L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 4, 3, 3, 0},
     spell_control_weather, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (11), 25, 12,
     "", "!Control Weather!", ""},

    {
     "create food", {10, 5, L2, L2,L2,L2,L2,L2,L2, 7, 5, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 0, 10, 0},
     spell_create_food, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (12), 5, 12,
     "", "!Create Food!", ""},

    {
     "berry", {L2, L2, L2, L2,L2,L2, L2, L2,L2, L2, L2, L2}, {5, 5, 5, 5,5,2,2,2,2, 5, 5, L2},
     {0, 0, 0, 10, 0},
     spell_berry, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (12), 5, 12,
     "", "!Berry Madness!", ""},

    {
	"create rose", {16, 11, L2, L2,L2,L2,L2,L2,L2, 1,1 , L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 0, 10, 0},
     spell_create_rose, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (511), 30, 12,
     "", "!Create Rose!", ""},

    {
     "create spring", {14, 17, L2, L2,L2,L2,L2,L2,L2, 12, 12, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 0, 10, 0},
     spell_create_spring, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (80), 20, 12,
     "", "!Create Spring!", ""},

    {
     "create water", {8, 3, L2, L2,L2,L2,L2,L2,L2, 4, 4, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 0, 10, 0},
     spell_create_water, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT (13), 5, 12,
     "", "!Create Water!", ""},

    {
     "cure blindness", {L2, 6, L2, L2,L2,L2,L2,L2,L2, L2, 5, L2}, {1, 1, 2, 2, 2,2,2,2,2,-1, 1, L2},
     {0, 0, 3, 4, 3},
     spell_cure_blindness, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT (14), 5, 12,
     "", "!Cure Blindness!", ""},

    {
     "cure critical", {L2, 13, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {1, 1, 2, 4,-1,2,2,2,2, -1, -1, L2},
     {0, 0, 3, 4, 3},
     spell_cure_critical, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT (15), 20, 12,
     "", "!Cure Critical!", ""},

    {
     "cure disease", {L2, 13, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {1, 1, 2, 2,-1,2,2,2,2, -1, -1, L2},
     {0, 0, 3, 4, 3},
     spell_cure_disease, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT (501), 20, 12,
     "", "!Cure Disease!", ""},

    {
     "cure light", {L2, 1, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, -1, L2},
     {0, 0, 3, 4, 3},
     spell_cure_light, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT (16), 10, 12,
     "", "!Cure Light!", ""},

    {
     "cure poison", {L2, 14, L2, L2,L2,L2,L2,L2, L2,L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, -1, L2},
     {0, 0, 3, 4, 3},
     spell_cure_poison, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT (43), 5, 12,
     "", "!Cure Poison!", ""},

    {
     "cure serious", {L2, 7, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, -1, L2},
     {0, 0, 3, 4, 3},
     spell_cure_serious, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT (61), 15, 12,
     "", "!Cure Serious!", ""},

    {
     "curse", {18, 18, L2, L2,L2,L2,L2,L2,L2, 16, 16, L2}, {1, 1, 2, 2,2, 2,2,2,2,1, 1, L2},
     {4, 4, 0, 0, 2},
     spell_curse, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_curse, SLOT (17), 20, 12,
     "curse", "The curse wears off.",
     "$p is no longer impure."},

    {
     "curse object", {30, L2, L2, L2,L2,L2,L2,L2, L2, 20, 20, L2}, {2, 2, 4, 4,4,2,2,2,2, 1, 1, L2},
     {0, 5, 0, 5, 0},
     spell_curse_obj, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT (510), 100, 24,
     "", "!Curse Object!", ""},

    {
     "demonfire", {L2, 34, L2, L2,L2,L2,L2,L2, L2, 30, 30, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {6, 4, 0, 0, 0},
     spell_demonfire, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (505), 20, 12,
     "torments", "!Demonfire!", ""},

    {
     "detect evil", {14, 4, L2, L2,L2,L2,L2,L2, L2, 6, 5, L2}, {1, 1, 2, 2,2, 2,2,2,2,1, 1, L2},
     {1, 1, 1, 1, 6},
     spell_detect_evil, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT (18), 5, 12,
     "", "The red in your vision disappears.", ""},

    {
     "detect good", {14, 4, L2, L2,L2,L2,L2,L2, L2, 7, 7, L2}, {1, 1, 2, 2,2, 2,2,2,2,1, 1, L2},
     {1, 1, 1, 1, 6},
     spell_detect_good, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT (513), 5, 12,
     "", "The gold in your vision disappears.", ""},

    {
     "detect hidden", {18, 12, L2, L2,L2,L2,L2,L2,L2, 10, 10, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {1, 1, 1, 1, 6},
     spell_detect_hidden, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT (44), 5, 12,
     "", "You feel less aware of your surroundings.",
     ""},

    {
     "detect invis", {9, 10, L2, L2,L2,L2,L2,L2,L2, 7, 7, L2}, {1, 1, 2, 2,2, 2,2,2,2,1, 1, L2},
     {1, 1, 1, 1, 6},
     spell_detect_invis, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT (19), 5, 12,
     "", "You no longer see invisible objects.",
     ""},

    {
     "detect magic", {2, 6, L2, L2,L2,L2,L2,L2,L2, 3, 3, L2}, {1, 1, 2, 2,2, 2,2,2,2,1, 1, L2},
     {1, 1, 1, 1, 6},
     spell_detect_magic, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT (20), 5, 12,
     "", "The detect magic wears off.", ""},

    {
     "detect poison", {15, 7, L2, L2,L2,L2,L2,L2,L2, 5, 5, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {1, 1, 1, 1, 6},
     spell_detect_poison, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT (21), 5, 12,
     "", "!Detect Poison!", ""},
    {
     "disenchant", {60, 60, L2, L2,L2,L2,L2,L2, L2, 60, 60, L2}, {2, 2, 4, 4,4,2,2,2,2, 1, 1, L2},
     {0, 5, 0, 5, 0},
     spell_disenchant, TAR_OBJ_CHAR_DEF, POS_STANDING,
     &gsn_disenchant, SLOT (525), 100, 12,
     "", "!Disenchant!", ""},
    {
     "dispel evil", {L2, 15, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, -1, L2},
     {2, 2, 2, 2, 2},
     spell_dispel_evil, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (22), 15, 12,
     "dispel evil", "!Dispel Evil!", ""},

    {
     "dispel good", {L2, 15, L2, L2,L2,L2,L2,L2, L2, 13, 13, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {2, 2, 2, 2, 2},
     spell_dispel_good, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (512), 15, 12,
     "dispel good", "!Dispel Good!", ""},

    {
     "dispel magic", {19, 29, L2, L2,L2,L2,L2,ML,L2, 16, 16, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {2, 2, 2, 2, 2},
     spell_dispel_magic, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (59), 15, 12,
     "", "!Dispel Magic!", ""},

    {
     "earthquake", {L2, 10, L2, L2,L2,L2,L2,L2,L2, 8, 8, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 8, 0, 0, 2},
     spell_earthquake, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT (23), 15, 12,
     "earthquake", "!Earthquake!", ""},

    {
     "enchant armor", {16, L2, L2, L2,L2,L2,L2,L2, L2, 25, 25, L2}, {2, 2, 4, 4,4,2,2,2,2, 1, 1, L2},
     {0, 5, 0, 5, 0},
     spell_enchant_armor, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT (510), 100, 24,
     "", "!Enchant Armor!", ""},

    {
     "enchant weapon", {17, L2, L2, L2,L2,L2,L2,L2, L2, 25, 25, L2}, {2, 2, 4, 4,4,2,2,2,2, 1, 1, L2},
     {0, 5, 0, 5, 0},
     spell_enchant_weapon, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT (24), 100, 24,
     "", "!Enchant Weapon!", ""},

    {
     "energy drain", {19, 24, L2, L2,L2,L2,L2,L2,L2, 15, 15, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {2, 1, 0, 0, 7},
     spell_energy_drain, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (25), 35, 6,
     "energy drain", "!Energy Drain!", ""},

    {
     "faerie fire", {6, L2, L2, L2,L2,L2,L2,L2,L2, 4, 4, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 5, 5, 0},
     spell_faerie_fire, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (72), 5, 12,
     "faerie fire", "The pink aura around you fades away.",
     ""},

    {
     "faerie fog", {14, 21, L2, L2,L2,L2,L2,L2,L2, 13, 13, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 5, 5, 0},
     spell_faerie_fog, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (73), 12, 12,
     "faerie fog", "!Faerie Fog!", ""},
/*
    {
     "farsight", {14, 16, 16, 18, 13, 13}, {1, 1, 2, 2, 1, 1},
     spell_farsight, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (521), 36, 20,
     "farsight", "!Farsight!", ""},
*/
    {
     "fear", {20, L2, L2, L2,L2,L2,L2,L2, L2, 18, 18, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {4, 0, 4, 0, 2},
     spell_fear, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (521), 12, 15,
     "fear", "You feel less afraid.", ""},

    {
     "fireball", {22, 52, L2, L2,L2,L2,L2,L2,L2, 18, 18, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {7, 0, 3, 0, 0},
     spell_fireball, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (26), 15, 12,
     "fireball", "!Fireball!", ""},

    {
     "fireproof", {13, 12, L2, L2,L2,L2,L2,L2,L2, 10, 10, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {8, 2, 0, 0, 0},
     spell_fireproof, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT (523), 10, 12,
     "", "", "$p's protective aura fades."},

    {
     "flamestrike", {L2, 20, L2, L2,L2,L2,L2,L2,L2, 17, 17, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {5, 5, 0, 0, 0},
     spell_flamestrike, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (65), 20, 12,
     "flamestrike", "!Flamestrike!", ""},

    {
     "fly", {10, 18, L2, L2,L2,L2,L2,L2,L2, 8, 8, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 10, 0, 0},
     spell_fly, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT (56), 10, 18,
     "", "You slowly float to the ground.", ""},

    {
     "floating disc", {4, 10, L2, L2,L2,L2,L2,L2,L2, 2, 2, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 2, 8, 0, 0},
     spell_floating_disc, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (522), 40, 24,
     "", "!Floating disc!", ""},

    {
     "frenzy", {L2, 24, L2, L2,L2,L2,L2,L2, L2, L2, 20, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, 1, L2},
     {0, 4, 4, 2, 0},
     spell_frenzy, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT (504), 30, 24,
     "", "Your rage ebbs.", ""},

    {
     "frighten", {14, L2, L2, L2,L2,L2,L2,L2, L2, 13, 13, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {4, 0, 4, 0, 2},
     spell_frighten, TAR_CHAR_OFFENSIVE, POS_STANDING,
     NULL, SLOT (521), 12, 15,
     "frighten", "You feel less frightened.", ""},

    {
     "gate", {L2, 75, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {1, 1, 3, 1, 4},
     spell_gate, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT (83), 80, 12,
     "", "!Gate!", ""},

    {
     "giant strength", {11, 15, L2, L2,L2,L2,L2,L2,L2, 8, 8, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {3, 7, 0, 0, 0},
     spell_giant_strength, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT (39), 20, 12,
     "", "You feel weaker.", ""},

    {

     "harm", {L2, 23, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, -1, L2},
     {2, 0, 3, 2, 3},
     spell_harm, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (27), 35, 12,
     "harm spell", "!Harm!,        " ""},

    {
     "haste", {21, 22, L2, L2,L2,L2,L2,L2,L2, 16, 16, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 4, 6, 0, 0},
     spell_haste, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT (502), 30, 12,
     "", "You feel yourself slow down.", ""},

    {
     "heal", {L2, 21, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, -1, L2},
     {0, 0, 3, 4, 3},
     spell_heal, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT (28), 35, 12,
     "", "!Heal!", ""},

    {
     "heat metal", {L2, 16, L2, L2,L2,L2,L2,L2,L2, 12, 12, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {5, 5, 0, 0, 0},
     spell_heat_metal, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (516), 25, 18,
     "spell", "!Heat Metal!", ""},

    {
     "holy word", {L2, 36, L2, L2, L2,L2,L2,L2, L2,L2, L2, L2}, {2, 2, 4, 4, 4,2,2,2,2,-1, -1, L2},
     {0, 3, 3, 4, 0},
     spell_holy_word, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT (506), 200, 24,
     "divine wrath", "!Holy Word!", ""},

    {
     "identify", {15, 16, L2, L2,L2,L2,L2,L2,L2, 12, 12, L2}, {1, 1, 2, 2,2,2,2,2,2,1, 1, L2},
     {1, 1, 1, 1, 6},
     spell_identify, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT (53), 12, 6,
     "", "!Identify!", ""},

    {
     "infravision", {9, 13, L2, L2,L2,L2,L2,L2,L2, 7, 7, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {3, 0, 3, 4, 0},
     spell_infravision, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT (77), 5, 18,
     "", "You no longer see in the dark.", ""},

    {
     "invisibility", {5, 6, L2, L2,L2,L2,L2,L2,L2, 2, 2, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {4, 0, 4, 2, 0},
     spell_invis, TAR_OBJ_CHAR_DEF, POS_STANDING,
     &gsn_invis, SLOT (29), 5, 12,
     "", "You are no longer invisible.",
     "$p fades into view."},
    {
     "iron grip", {60, L2, L2, L2,L2,L2,L2,L2, L2, 60, 60, L2}, {2, 2, 4, 4,4,2,2,2,2, 1, 1, L2},
     {2, 6, 0, 2, 0},
     spell_iron_grip, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT (524), 100, 24,
     "", "!Iron Grip!", ""},
    {
     "know alignment", {12, 9, L2, L2,L2,L2,L2,L2,L2, 7, 7, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {1, 1, 1, 1, 6},
     spell_know_alignment, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT (58), 9, 12,
     "", "!Know Alignment!", ""},

    {
     "lightning bolt", {13, 23, L2, L2,L2,L2,L2,L2,L2, 12, 12, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 4, 4, 2, 0},
     spell_lightning_bolt, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (30), 15, 12,
     "lightning bolt", "!Lightning Bolt!", ""},

    {
     "locate object", {9, 15, L2, L2,L2,L2,L2,L2,L2, 6, 6, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {3, 3, 3, 0, 1},
    spell_locate_object, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (31), 20, 18,
     "", "!Locate Object!", ""},

    {
     "magic missile", {1, L2, L2, L2,L2,L2,L2,L2,L2, 1, 1, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 6, 0, 4},
     spell_magic_missile, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (32), 15, 12,
     "magic missile", "!Magic Missile!", ""},

    {
     "mass healing", {L2, 68, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {2, 2, 4, 4,4,2,2,2,2, -1, 1, L2},
     {0, 0, 3, 4, 3},
     spell_mass_healing, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (508), 100, 12,
     "", "!Mass Healing!", ""},

    {
     "mass invis", {22, 25, L2, L2,L2,L2,L2,L2, L2, 18, 18, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {4, 0, 4, 2, 0},
     spell_mass_invis, TAR_IGNORE, POS_STANDING,
     &gsn_mass_invis, SLOT (69), 20, 16,
     "", "You are no longer invisible.", ""},

    {
     "nexus", {30, 30, L2, L2,L2,L2,L2,L2,L2, 20, 20, L2}, {2, 2, 4, 4,4,2,2,2,2, -1, 1, L2},
     {1, 1, 3, 1, 4},
     spell_nexus, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (520), 80, 16,
     "", "!Nexus!", ""},

    {
     "pass door", {24, 24, L2, L2,L2,L2,L2,L2,L2, 20, 20, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 3, 6, 1, 0},
     spell_pass_door, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT (74), 20, 12,
     "", "You feel solid again.", ""},

    {
     "plague", {23, L2, L2, L2, L2,L2,L2,L2,L2,15, 15, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {3, 0, 3, 3, 1},
     spell_plague, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_plague, SLOT (503), 20, 12,
     "sickness", "Your sores vanish.", ""},

    {
     "poison", {17, L2, L2, L2, L2,L2,L2,L2,L2,10, 10, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {4, 4, 0, 0, 2},
     spell_poison, TAR_OBJ_CHAR_OFF, POS_FIGHTING,
     &gsn_poison, SLOT (33), 10, 12,
     "poison", "You feel less sick.",
     "The poison on $p dries up."},

    {
     "poison missile", {50, 61, L2, L2,L2,L2,L2,L2,L2, 40, 40, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {3, 3, 2, 0, 2},
     spell_poison_missile, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (70), 15, 12,
     "poison missile", "!Acid Blast!"},

    {
     "portal", {18, 8, 26, 21,21,22,21,24,23, 3, 3, L2}, {2, 2, 4, 4,4,4,4,4,4, 1, 1, L2},
     {1, 1, 3, 1, 4},
     spell_portal, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (519), 50, 18,
     "", "!Portal!", ""},

    {
     "protection evil", {12, 8, L2, L2, L2,L2,L2,L2,L2,5, 5, L2}, {1, 1, 2, 2, 2,2,2,2,2,1, 1, L2},
     {2, 2, 2, 2, 2},
     spell_protection_evil, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT (34), 5, 12,
     "", "You feel less protected.", ""},

    {
     "protection good", {12, 9, L2, L2,L2,L2,L2,L2,L2, 6, 6, L2}, {1, 1, 2, 2,2, 2,2,2,2,1, 1, L2},
     {2, 2, 2, 2, 2},
     spell_protection_good, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT (514), 5, 12,
     "", "You feel less protected.", ""},

    {
     "ray of truth", {L2, 45, L2, L2,L2,L2,L2,L2,L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, -1, L2},
     {5, 5, 0, 0, 0},
     spell_ray_of_truth, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (518), 20, 12,
     "ray of truth", "!Ray of Truth!", ""},

    {
     "recharge", {9, 12, L2, L2,L2,L2,L2,L2,L2, 5, 5, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {1, 1, 1, 1, 6},
     spell_recharge, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT (517), 60, 16,
     "", "!Recharge!", ""},

    {
     "refresh", {8, 5, L2, L2,L2,L2,L2,L2,L2, 3, 3, L2}, {1, 1, 2, 2,2, 2,2,2,2,1, 1, L2},
     {0, 3, 4, 3, 0},
     spell_refresh, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
     NULL, SLOT (81), 12, 18,
     "refresh", "!Refresh!", ""},

    {
     "remove curse", {L2, 18, L2, L2,L2,L2,L2,L2,L2, 15, 15, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {5, 0, 0, 0, 5},
     spell_remove_curse, TAR_OBJ_CHAR_DEF, POS_STANDING,
     NULL, SLOT (35), 5, 12,
     "", "!Remove Curse!", ""},

    {
     "sanctuary", {36, 36, L2, L2,L2,L2,L2,ML,L2, 36, 36, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 2, 0, 3, 5},
     spell_sanctuary, TAR_CHAR_DEFENSIVE, POS_STANDING,
     &gsn_sanctuary, SLOT (36), 75, 12,
     "", "The white aura around your body fades.",
     ""},
 
   {
     "save_life", {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25}, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
     {0, 0, 3, 4, 3},
     spell_save_life, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT (36), 200, 30,
     "", "!Save Life!",
     ""},

    {
     "shield", {20, 35, L2, L2,L2,L2,L2,ML,L2, 15, 15, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 5, 5, 0, 0},
     spell_shield, TAR_CHAR_DEFENSIVE, POS_STANDING,
     NULL, SLOT (67), 12, 18,
     "", "Your force shield shimmers then fades away.",
     ""},

    {
     "shocking grasp", {10, L2, L2, L2,L2,L2,L2,L2,L2, 8, 8, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {2, 4, 4, 0, 0},
     spell_shocking_grasp, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (53), 15, 12,
     "shocking grasp", "!Shocking Grasp!", ""},
    {
     "simulacrum", {L2, 41, L2, L2,L2,L2,L2,L2, L2, L2, L2, L2}, {1, 1, 2, 2,2,2,2,2,2, -1, -1, L2},
     {0, 0, 3, 4, 3},
     spell_simulacrum, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_simulacrum, SLOT (0), 40, 18,
     "`&dra`*in`8ing `*to`&uch`*", "!simulacrum!", ""},
    {
     "sleep", {10, L2, L2, L2,L2,L2,L2,L2,L2, 6, 6, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 5, 4, 1},
     spell_sleep, TAR_CHAR_OFFENSIVE, POS_STANDING,
     &gsn_sleep, SLOT (38), 15, 12,
     "", "You feel less tired.", ""},

    {
     "sticky", {L2, L2, L2, L2, L2, L2, L2, L2, L2, L2, L2, L2}, {0, 0, 0, 0,0,0,0,0,0, 0, 0, L2},
     {0, 1, 4, 5, 0},
     spell_null, TAR_CHAR_OFFENSIVE, POS_STANDING,
     &gsn_sticky, SLOT (38), 15, 12,
     "", "You feel less sticky.", ""},

    {
     "slow", {23, 30, 12, L2,L2,L2,L2,L2,L2, L2, 18, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 1, 4, 5, 0},
     spell_slow, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (515), 30, 12,
     "", "You feel yourself speed up.", ""},

    {
     "stone skin", {25, 40, L2, L2,L2,L2,L2,ML,L2, 21, 21, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {3, 7, 0, 0, 0},
     spell_stone_skin, TAR_CHAR_SELF, POS_STANDING,
     NULL, SLOT (66), 12, 18,
     "", "Your skin feels soft again.", ""},

    {
     "summon", {24, 32, L2, L2,L2,L2,L2,L2,L2, 10, 10, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 3, 3, 4},
     spell_summon, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (40), 50, 12,
     "", "!Summon!", ""},

    {
     "teleport", {13, L2, L2, L2,L2,L2,L2,L2,L2, 11, 11, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 3, 3, 4},
     spell_teleport, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (2), 15, 12,
     "", "You no longer feel disorientated.", ""},

    {
     "unravel", {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, L2}, {-2, -2, -4, -4,-4,-4,-4,-4,-4, -1, -1, L2},
     {1, 1, 3, 1, 4},
     spell_unravel, TAR_OBJ_SPELL, POS_FIGHTING,
     NULL, SLOT (519), 40, 18,
     "", "!Portal!", ""},

    {
     "ventriloquate", {1, L2, L2, L2,L2,L2,L2,L2,L2, 5, 5, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {2, 0, 8, 0, 0},
     spell_ventriloquate, TAR_IGNORE, POS_STANDING,
     NULL, SLOT (41), 5, 12,
     "", "!Ventriloquate!", ""},

    {
     "weaken", {11, 14, L2, L2,L2,L2,L2,L2,L2, 12, 12, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {1, 6, 3, 0, 0},
     spell_weaken, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (68), 20, 12,
     "spell", "You feel stronger.", ""},

    {
     "word of recall", {32, 28, 40, 30,35,33,36,38,34, 20, 20, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 3, 3, 4},
     spell_word_of_recall, TAR_CHAR_SELF, POS_RESTING,
     NULL, SLOT (42), 5, 12,
     "", "!Word of Recall!", ""},

/*
 * Dragon breath
 */
    {
     "acid breath", {31, L2, L2, L2,L2,L2,L2,L2,L2, 5, 5, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 10, 0, 0, 0},
     spell_acid_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (200), 12, 4,
     "blast of acid", "!Acid Breath!", ""},

    {
     "fire breath", {40, L2, L2, L2,L2,L2,L2,L2,L2, 5, 5, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {10, 0, 0, 0, 0},
     spell_fire_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (201), 12, 4,
     "blast of flame", "The smoke leaves your eyes.", ""},

    {

     "frost breath", {34, L2, L2, L2,L2,L2,L2,L2,L2, 5, 5, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 0, 10, 0},
     spell_frost_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (202), 12, 4,
     "blast of frost", "!Frost Breath!", ""},

    {
     "gas breath", {39, L2, L2, L2,L2,L2,L2,L2,L2, 5, 5, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 0, 0, 10},
     spell_gas_breath, TAR_IGNORE, POS_FIGHTING,
     NULL, SLOT (203), 12, 4,
     "blast of gas", "!Gas Breath!", ""},

    {
     "lightning breath", {37, L2, L2, L2,L2,L2,L2,L2,L2, 5, 5, L2}, {1, 1, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 10, 0, 0},
     spell_lightning_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (204), 12, 4,
     "blast of lightning", "!Lightning Breath!", ""},

    {
     "demi_bless", {ML, ML, ML, ML, ML, ML, ML, ML, ML, ML, ML, ML}, {-1, -1, -2, -2,-2,-2,-2,-2,-2, -1, -1, -L2},
     {0, 0, 0, 0, 10},
     spell_asmodeus_bless, TAR_OBJ_CHAR_DEF, POS_STANDING,
     NULL, SLOT (3), 5, 12,
     "", "The power of Demiscus leaves you.",
     "$p's Demiscus Aura fades."},



/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
    {
     "general purpose", {L2, L2, L2, L2,L2,L2,L2, L2, L2, L2, L2, L2}, {0, 0, 0, 0,0,0,0,0,0, 0, 0, L2},
     {2, 2, 2, 2, 2},
     spell_general_purpose, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (401), 0, 12,
     "general purpose ammo", "!General Purpose Ammo!", ""},

    {
     "high explosive", {L2, L2, L2, L2,L2,L2,L2, L2, L2, L2, L2, L2}, {0, 0, 0, 0,0,0,0,0,0, 0, 0, L2},
     {2, 2, 2, 2, 2},
     spell_high_explosive, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (402), 0, 12,
     "high explosive ammo", "!High Explosive Ammo!", ""},
	 /*
	  *Spells for Offensive Protection Spells
	  */
	
	 {
     "ice shield", {L2, 23, L2, L2,L2,L2,L2, L2, L2, L2, L2, L2}, {3, 3, 3, 3,3,2,2,2,2, 3, 3, L2},
     {0, 0, 3, 6, 1},
     spell_iceshield, TAR_CHAR_DEFENSIVE, POS_STANDING,
     &gsn_iceshield, SLOT (0), 40, 12,
     "ice shield", "The cold aura around you dissapates...", ""},
	
	 {
     "fire shield", {L2, 26, L2, L2,L2,L2,L2, L2, L2, L2, L2, L2}, {3, 3, 3, 3,3,2,2,2,2, 3, 3, L2},
     {7, 3, 0, 0, 0},
     spell_fireshield, TAR_CHAR_DEFENSIVE, POS_STANDING,
     &gsn_fireshield, SLOT (0), 40, 12,
     "fire shield", "The firey aura around you dissapates...", ""},
	{
     "shock shield", {L2, 29, L2, L2,L2,L2,L2, L2, L2, L2, L2, L2}, {3, 3, 3, 3,3,2,2,2,2, 3, 3, L2},
     {0, 0, 5, 5, 0},
     spell_shockshield, TAR_CHAR_DEFENSIVE, POS_STANDING,
     &gsn_shockshield, SLOT (0), 40, 12,
     "shock shield", "The electrical aura around you dissapates...", ""},
     {
     "mana shield", {20, L2, L2, L2,L2,L2,L2, L2, L2, 15, 15, L2}, {3, 3, 3, 3,3,2,2,2,2, 3, 3, L2},
     {2, 2, 2, 2, 2},
     spell_manashield, TAR_CHAR_DEFENSIVE, POS_STANDING,
     &gsn_manashield, SLOT (0), 30, 12,
     "mana shield", "The protective weaves around you dissapates...", ""},
	
	


/* combat and weapons skills */


    {
     "axe", {1, 1, 1, 1,1, 1, 1,1,1, 1, 1, 1}, {6, 6, 5, 4,5,6,5,4,5, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_axe, SLOT (0), 0, 0,
     "", "!Axe!", ""},

    {
     "dagger", {1, 1, 1, 1,1,1, 1,1,1, 1, 1, 1}, {2, 3, 2, 2,2,2,2,2,2, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_dagger, SLOT (0), 0, 0,
     "", "!Dagger!", ""},

    {
     "flail", {1, 1, 1, 1,1,1, 1,1,1, 1, 1, 1}, {6, 3, 6, 4,6,5,5,4,4, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_flail, SLOT (0), 0, 0,
     "", "!Flail!", ""},

    {
     "mace", {1, 1, 1, 1,1,1, 1,1,1, 1, 1, 1}, {5, 2, 3, 3,3,3,2,3,3, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_mace, SLOT (0), 0, 0,
     "", "!Mace!", ""},

    {
     "polearm", {1, 1, 1, 1,1,1, 1,1,1, 1, 1, 1}, {6, 6, 6, 4,4,6,6,4,5, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_polearm, SLOT (0), 0, 0,
     "", "!Polearm!", ""},
     
    {
     "bow", {L2, L2, L2, L2,1,L2, L2,L2,L2, L2, L2, L2}, {0, 0, 0, 0,1,0,0,0,0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_bow, SLOT (0), 0, 10,
     "", "!Bow!", ""},
     
     {
     "staff", {1, 1, 1, 1,1,1, 1,1,1, 1, 1,1}, {5, 4, 6, 3,4,4,4,2,3,1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_staff, SLOT (0), 0, 0,
     "", "!Staff!", ""},

    {
     "shield block", {1, 1, 1, 1,1,1, 1,1,1, 1, 1, 1}, {6, 6, 6, 2,5,6,6,4,5, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_shield_block, SLOT (0), 0, 0,
     "", "!Shield!", ""},

    {
     "spear", {1, 1, 1, 1,1,1, 1,1,1, 1, 1, 1}, {4, 4, 4, 3,3,4,4,3,3, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_spear, SLOT (0), 0, 0,
     "", "!Spear!", ""},
     
     {
     "sword", {1, 1, 1, 1,1,1, 1,1,1, 1, 1, 1}, {6, 6, 3, 2, 3,5,6,6,4,1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_sword, SLOT (0), 0, 0,
     "", "!sword!", ""},

    {
     "whip", {1, 1, 1, 1,1,1, 1,1,1, 1, 1, 1}, {6, 5, 5, 4, 3,5,6,-6,4,1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_whip, SLOT (0), 0, 0,
     "", "!Whip!", ""},

    {
     "assassinate", {-1, -1, 50, -1,-1,0,0,0,0, 30, 30, -1}, {0, 0, 5, 0,0,0,0,0,0, -1, -1, L2},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_assassinate, SLOT (0), 0, 24,
     "assassination", "!Assassinate!", ""},

    {
     "backstab", {-1, -1, -1, -1,-1,1,0,0,1, 1, 1, -1}, {0, 0, 0, 0,0,3,0,0,0, 1, 1, L2},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_backstab, SLOT (0), 0, 24,
     "backstab", "!Backstab!", ""},
    
    {
     "balefire", {40, 40, 40, 40,40, 40,40, 40,40, 40, 30, 40}, {-3, -3, -3, -3,-3, -3, -3, -3,-3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_balefire, SLOT(0), 0, 0,
     "balefire", "!Balefire!", ""},

     {
     "bandage", {1, 1, 1, 1,1,1, 1,1,1, 1, 1, 1}, {1, 1, 1, 1,1, 1, 1,1,1,1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_bandage, SLOT(0), 0, 12,
     "bandages", "You remove the bandages.", ""},

     {
     "bandaging", {1, 1, 1, 1,1,1, 1,1,1, 1, 1, 1}, {-2, -1, -2, -2,-2, -2, -2,-2,-2,-1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     NULL, SLOT(0), 0, 12,
     "regeneration", "You are now ready to bandage again.", ""},

    {
     "bash", {L2, L2, 50, 1,35,0,0,0,0, -1, -1, L2}, {-1, -1, -1, 4,4,0,0,0,0, 0, 0, L2},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_bash, SLOT (0), 0, 25,
     "bash", "!Bash!", ""},

    {
     "daze", {60, 60, 60, 60,60, 60,60, 60, 60, 60, 60, 60}, {-5, -5, -5, -5,-5, -5, -5, -5, -5, -5, -5, -5},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_daze, SLOT (0), 0, 24,
     "", "You feel less tired and more aware!", ""},

    {
     "berserk", {L2, L2, 30, 18,22,0,0,0,0, 30, 30, 30}, {-1, -1, -1, 5, 8,0,0,0,0, 1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_berserk, SLOT (0), 0, 16,
     "", "You feel your pulse slow down.", ""},

    {
     "blood", {40, 40, 40, 40,40,40, 40,40, 40, 40, 40, 40}, {-3,-3, -3, -3, -3, -3, -3, -3,-3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_blood, SLOT (0), 0, 24,
     "", "!Blood!", ""},

    {
     "cleanse", {40, 40, 40, 40,40,40, 40,40, 40, 40, 40, 40}, {-3,-3, -3, -3, -3, -3, -3, -3,-3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_cleanse, SLOT (0), 0, 24,
     "", "The bright light in your eyes fades!", ""},
     
    {
     "counter", { L2, L2, L2, 25,L2,L2,35,L2,L2,L2,20,20 },{ 0, 0, 0, 8,0,0,10,0,0,1,1,0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_counter, SLOT( 0), 0, 0,
     "counterattack", "!Counter!", "" },


    {
     "dirt kicking", {-1, -1, 3, 3,3,3,3,3,3, 0, 0, 0}, {0, 0, 4, 4,4,3,5,6,3, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_dirt, SLOT (0), 0, 20,
     "kicked dirt", "The dirt drifts away from your eyes.", ""},

    {
     "disarm", {-1, -1, 12, 11,15,12,0,10,12, 1, 1, 1}, {0, 0, 6, 4, 6,6,0,4,5,1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_disarm, SLOT (0), 0, 12,
     "", "!Disarm!", ""},

    {
     "dodge", {20, 22, 1, 13,13,24,30,0,22, 5, 5, 5}, {8, 8, 4, 6,6,4,7,0,4, 1, 1, 5},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_dodge, SLOT (0), 0, 0,
     "", "!Dodge!", ""},

    {
     "dome", {40, 40, 40, 40,40,40, 40,40, 40, 40, 40, 40}, {-3, -3, -3, -3,-3, -3, -3, -3,-3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_dome, SLOT (0), 0, 24,
     "", "The dizzyness in your head subsides!", ""},

  {
     "engage", {L2, L2, L2, 35,40,25,0,45,0, 3, 3, L2}, {0, 0, 0, 5,8,1,0,6,0, 1, 1, L2},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_engage, SLOT (0), 0, 24,
     "engage", "!Engage!", ""},

    {
     "enhanced damage", {-1, -1, 25, 1,10,30,50,10,26, 5, 5, 5}, {-1, 10, 5, 3,4,5,8,8,5, 1, 1, 5},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_enhanced_damage, SLOT (0), 0, 0,
     "", "!Enhanced Damage!", ""},
  
    {
     "envenom", {L2, L2, 10, L2,L2,0,0,0,0,15, L2, 1}, {0, 0, 4, 0,0,0,0,0,0, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_RESTING,
     &gsn_envenom, SLOT (0), 0, 16,
     "", "!Envenom!", ""},

    {
     "galloping", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_galloping, SLOT (0), 0, 36,
     "", "!galloping! .", ""},

    {
     "firestorm", {35, 35, 35, 35,35, 35, 35, 35,35, 35, 20, 40}, {-3, -3, -3,-3,-3, -3, -3, -3,-3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_firestorm, SLOT (0), 0, 36,
     "", "You relax your mind.", ""},

    {
     "hand to hand", {25, 10, 15, 6,10,0,0,0,0, 20, 20, 1}, {8, 5, 6, 4,6,0,0,0,0, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_hand_to_hand, SLOT (0), 0, 0,
     "", "!Hand to Hand!", ""},

    {
     "kick", {L2, 65, 14, 8,12,14,18,1,14, L2, L2, 1}, {0, 4, 6, 3,4,6,5,2,5, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_kick, SLOT (0), 0, 12,
     "kick", "!Kick!", ""},

    {
     "tap", {55, 55, 55, 55,55, 55, 55, 55,55, 55, 55, 55}, {-3, -3, -3,-3,-3, -3, -3,-3, -3, -3, -1, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_manadrain, SLOT (0), 0, 12,
     "", "The tap breaks apart.", ""},

    {
     "parry", {22, 20, 13, 1, 5,16,23,0,12,18, 15, 1}, {8, 8, 6, 4,4,6,8,0,6, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_parry, SLOT (0), 0, 0,
     "", "!Parry!", ""},

    {
     "rescue", {L2, L2, L2, 1, 50,0,0,0,4,L2, 20, 1}, {0, 0, 0, 4,6,0,0,0,5, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_rescue, SLOT (0), 0, 12,
     "", "!Rescue!", ""},

    {
     "restrain", {15, 15, 15, 15,15,15, 15, 15, 15, 15, 15, 15}, {-3, -3, -3, -3,-3, -3, -3, -3, -3, -3, -1, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_restrain, SLOT (0), 0, 12,
     "", "You can move again.", ""},

    {
     "willpower", {15, 15, 15, 15,15,15, 15, 15, 15, 15, 15, 15}, {-3, -3, -3, -3,-3, -3, -3, -3, -3, -3, -1, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_willpower, SLOT (0), 0, 12,
     "", "You lose your strength of mind.", ""},

     {
     "ensnare", {40, 40, 40, 40,40,40, 40,40, 40, 40, 40, 40}, {0, 0, 0, 0,0, 0,0, 0, 0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_ensnare, SLOT (0), 0, 12,
     "", "!ENSNARE!", ""},

    {
     "still", {15, 15, 15, 15, 15,15, 15, 15, 15,15, 15, 25}, {-3, -3, -3, -3,-3, -3, -3, -3, -3,-3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_still, SLOT(0), 0, 12,
     "", "You can feel the true source again.", ""},

    {
     "trip", {L2, L2, 1, 15,15,1,0,15,1, L2, L2, 1}, {0, 0, 2, 8,4,2,0,6,4, 1, 0, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_trip, SLOT (0), 0, 25,
     "trip", "!Trip!", ""},

    {
     "second attack", {35, 35, 12, 5,15,14,24,8,13, 10, 7, 1}, {10, 8, 4, 3,8,4,10,6,6, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_second_attack, SLOT (0), 0, 0,
     "", "!Second Attack!", ""},

    {
     "third attack", {L2, L2, 24, 12,30,0,0,14,24, 30, 25, 1}, {0, 0, 8, 4, 10,0,0,8,10, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_third_attack, SLOT (0), 0, 0,
     "", "!Third Attack!", ""},

    {
     "fourth attack", {L2, L2, L2, L2,L2,L2,L2,L2,L2, L2, L2, L2}, {0, 0, 0, 6,0,0,0,0,0, 1, 1, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_fourth_attack, SLOT (0), 0, 0,
     "", "!Third Attack!", ""},
	
    {
     "strike", {L2,L2,L2,L2,L2,L2,L2,18,L2,L2,L2,L2},{0,0,0,0,0,0,0,12,0,0,0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_strike, SLOT (0), 0, 25,
     "strike", "!Strike!",""},
    {
     "martial arts", {L2,L2,L2,L2,L2,L2,L2,1,L2,L2,L2,L2},{0,0,0,0,0,0,0,3,0,0,0,0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_martial_arts, SLOT (0), 0, 0,
     "", "!Martial Arts!",""},
    
    {
     "evade", {L2, L2, L2, L2, L2,L2,L2,1,L2,L2, L2, L2}, {0, 0, 0, 0,0,0,0,4,0, 0, 0,0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_evade, SLOT (0), 0, 0,
     "", "!Evade!", ""},
     
    {
     "block", {L2, L2, L2,L2,L2,L2, L2, 8,L2, L2, L2, L2}, {0, 0, 0, 0,0,0,0,6,0, 0, 0,0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_block, SLOT (0), 0, 0,
     "", "!Block!", ""},
     {
     "ironskin", {L2, L2, L2,L2,L2,L2, L2, 1,L2, L2, L2, L2}, {0, 0, 0, 0,0,0,0,10,0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_marironskin, SLOT (0), 0, 6,
     "", "You lose the concentration required to maintain your ironskin.", ""},
     
     {
     "cloak", {L2, L2, L2,L2,L2,75, L2, L2,L2, 75, 75, L2}, {0, 0, 4, 0,0,4,0,0,0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_cloak, SLOT (0), 0, 6,
     "", "You remove your disguise.", ""},
     
     {
     "flying kick", {L2, L2, L2,L2,L2,L2, L2, 15,L2, L2, L2, L2}, {0, 0, 0, 0,0,0,0,6,0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_flying_kick, SLOT (0), 0, 18,
     "flying side kick", "!FLYING KICK!", ""},
     
     {
     "shadow kick", {L2, L2, L2,L2,L2,L2, L2, 60,L2, L2, L2, L2}, {0, 0, 0, 0,0,0,0,12,0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_shadow_kick, SLOT (0), 0, 24,
     "shadow kick", "!SHADOW KICK!", ""},
     {
     "awareness", {L2, L2, 14,16,12,13, L2, 3,L2, 1, 1, 1}, {0, 0, 8, 7,6,7,0,4,0, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_awareness, SLOT (0), 0, 4,
     "", "You lose the concentration required to maintain your awareness.", ""},
     
     {
     "flip", {L2, L2, L2, L2,L2,L2,L2,12,L2, L2, L2, L2}, {-1, -1, -1, 0,0,0,0,6,0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_flip, SLOT (0), 0, 24,
     "flip", "!Flip!", ""},
    
    {
     "archery", {L2, L2, L2, L2,8,L2, L2, L2, L2, L2, L2, L2}, {0, 0, 0, 0,2,0,0,0,0 ,0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_second_shot, SLOT (0), 0, 0,
     "", "!Second Shot!", ""},
     
     {
     "advanced archery", {L2, L2, L2, L2,16,L2, L2, L2, L2, L2, L2, L2}, {0, 0, 0, 0,4,0,0,0,0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_third_shot, SLOT (0), 0, 0,
     "", "!Third Shot!", ""},
     {
     "blackjack", {L2, L2, L2, L2, L2, 21, L2, L2,L2, L2, L2, L2}, {0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_blackjack, SLOT (0), 0, 12,
     "blackjack", "You moan and feel intense pain in your head as your eyes open a crack and find yourself lying on the ground.", ""},
     {
     "blackjack2", {L2, L2, L2, L2, L2, L2, L2, L2, L2, L2, L2, L2}, {0, 0, 0, 0,0,0,0,0,0, 0, 0, L2},
     {2, 2, 2, 2, 2},
     spell_null, TAR_CHAR_OFFENSIVE, POS_STANDING,
     &gsn_blackjack2, SLOT (0), 15, 12,
     "", "You feel less paranoid.", ""},
/* non-combat skills */

   

    {
     "ballad", {15, 15, 15, 15,15,15, 15, 15, 15, 15, 15,15}, {-3, -3, -3, -3,-3 -3, -3, -3, -3,-3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_SLEEPING,
     &gsn_ballad, SLOT (0), 0, 0,
     "", "You are ready to perform again.", ""},

    {
     "build", {25, 25, 25, 25,25,25, 25, 25, 25, 25, 25, 25}, {-7, -7, -7, -7,-7, -7, -7, -7, -7,-7, -7, -7},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_SLEEPING,
     &gsn_build, SLOT (0), 0, 0,
     "", "!Build!", ""},

    {
     "bomb", {L2, L2, L2, L2,L2, L2, L2, L2, L2,L2, L2, L2}, {-7, -7, -7, -7,-7, -7, 6, -7, -7,-7, -7, -7},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_SLEEPING,
     &gsn_bomb, SLOT (0), 0, 0,
     "", "!Bomb!", ""},

    {
     "couple", {30, L2, L2, L2, L2, L2, L2, L2, L2, 30, 30, L2}, {0, 0, 0, 0,0,0 ,0,0,0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_couple, SLOT (0), 0, 12,
     "", "!couple", ""},

    {
     "dampen", {0, 0, 0, 0,0,0 ,35, 65,0, 0, 0, 0}, {0, 0, 0, 0,0,0 ,6, 8,0, 0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_dampen, SLOT (0), 0, 12,
     "", "You lose your dampening force.", ""},
     
    {
      "disguise", {L2, L2, L2, L2,L2, L2, L2, L2, L2,L2, L2, L2}, {-7, -7, -7, -7,-7, -7, 4, -7, -7,-7, -7, -7},
      {2, 2, 2, 2, 2},
      spell_null, TAR_IGNORE, POS_SLEEPING,
      &gsn_disguise, SLOT (0), 0, 0,
      "", "!Bomb!", ""},


    {
     "dream", {15, 15, 15, 15,15,15, 15, 15, 15, 15, 15, 15}, {0, 0, 0,0, 0,0,0,0,0, 1, 0, 0},
    {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_SLEEPING,
     &gsn_dream, SLOT (0), 0, 0,
     "", "!Dream!", ""},

    {
     "dual wield", {0, 25, 15, 12, 20, 28, 0, 0, 25, 0, 13, 1}, {0, 8, 4, 5,5,8,0,0,6, 0, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_dual_wield, SLOT (0), 0, 12,
     "", "!Dual!", ""},

    {
     "fast healing", {15, 9, 16, 6, 8,18,18,5,18,12, 10, 1}, {8, 5, 6, 4,5,8,8,6,7, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_SLEEPING,
     &gsn_fast_healing, SLOT (0), 0, 0,
     "", "!Fast Healing!", ""},

    {
     "fix", {15, 15, 15, 15,15, 15,15, 15,15, 15, 15, 15}, {-3, -3, -3, -3,-3, -3, -3, -3, -3,-3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_SLEEPING,
     &gsn_fix, SLOT (0), 0, 0,
     "", "!Fix!", ""},

    {
     "horseback", {40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40}, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_horseback, SLOT (0), 0, 24,
     "", "!Horse!", ""},

{
     "plant", {L2, L2, L2, L2,L2, L2,L2, L2,L2, L2, L2, 1}, {-3, -3, -3, -3,-3,-3,-3,-3,-3,-3,-3,1}, 
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_plant, SLOT (0), 0, 0,
     "", "!PLANT!", ""}, 

    {
     "focus", {15, 0, 0, 0,0,0,0,0,0, 20, 20, 1}, {8, 0, 0, 0,0,0,0,0,0, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_focus, SLOT (0), 0, 12,
     "", "You lose your focus.", ""},

    {
     "haggle", {7, 18, 1, 14,18,1,6,0,1, 8, 8, 1}, {5, 8, 3, 6,4,3,3,0,3, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_RESTING,
     &gsn_haggle, SLOT (0), 0, 0,
     "", "!Haggle!", ""},

    {
     "hide", {L2, L2, 1, 12,14,1,10,0,3, L2, 10, 1}, {-1, -1, 4, 6,4,2,6,0,5, -1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_RESTING,
     &gsn_hide, SLOT (0), 0, 12,
     "", "You are no longer hidden.", ""},

    {
     "hunt", {40, 40, 40, 40,40,40, 40,40, 40, 40, 40, 40}, {-3, -3, -3, -3,-3,-3, -3, -3, -3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_hunt, SLOT (0), 0, 12,
     "", "!Hunt!", ""},

    {
     "lore", {1, 1, 1, 1,1, 1, 1, 1,1, 1,1, 1}, {2, 2, 2, 3,2,2,2,2,2, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_RESTING,
     &gsn_lore, SLOT (0), 12, 6,
     "", "!Lore!", ""},

    {
     "luck", {40, 40, 40, 40,40,40, 40,40, 40, 40, 40, 40}, {-3, -3, -3, -3,-3,-3, -3, -3, -3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_luck, SLOT (0), 0, 12,
     "", "!Luck!", ""},

    {
     "meditation", {6, 6, 15, 15,15,15,15,5,15, 4, 4, 1}, {5, 5, 8, 8,7,8,8,10,8, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_SLEEPING,
     &gsn_meditation, SLOT (0), 0, 0,
     "", "Meditation", ""},

    {
     "openbag", {L2, L2, L2, L2,L2,25,L2, L2, L2, 30, L2, L2}, {0, 0, 0, 0,0,3,0,0,0, 1, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_openbag, SLOT (0), 0, 24,
     "", "!OPENBAG!", ""},


    {
     "peek", {L2, L2, 1, 14,25,1,0,0,6, 5, 5, 1}, {-1, -1, 3, -1,3,3,0,0,3, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_peek, SLOT (0), 0, 0,
     "", "!Peek!", ""},

    {
     "peek_in", {L2, L2, L2, L2,L2, 25, L2, L2, L2,20, L2, L2}, {-1, -1, -1, -1,0,3,0,0,0 ,1, 1, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_peek_in, SLOT (0), 0, 0,
     "", "!Peek!", ""},

    {
     "pick lock", {L2, L2, 7, 25,L2,4,0,0,8, 25, 25, 1}, {8, 8, 4, 8,-1,3,0,0,4, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_pick_lock, SLOT (0), 0, 12,
     "", "!Pick!", ""},

    {
     "pray", {15, 15, 15, 15,15,15, 15, 15, 15, 1, 15, 15}, {-3, -3, -3, -3,-3,-3, -3, -3, -3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_pray, SLOT (0), 0, 12,
     "", "Your prayers wear off.", ""},

    {
     "shroud", {25, 25, 25, 25,25,25, 25, 25,25, 1, 25, 25}, {-3, -3, -3,-3, -3,-3, -3, -3, -3, -1, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_shroud, SLOT (0), 0, 12,
     "", "You are no longer cloaked in darkness.", ""},

    {
     "scry", {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25}, {-3, -3, -3,-3, -3,-3, -3, -3, -3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_scry, SLOT (0), 0, 4,
     "", "!Scry!", ""},

    {
     "sneak", {L2, L2, 4, 10,12,3,0,15,6, 0, 8, 1}, {-1, -1, 4, -1,4,4,0,7,4, -1, -1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_sneak, SLOT (0), 0, 12,
     "", "You no longer move silently.", ""},

    {
     "steal", {L2, L2, 15, L2,L2,15,0,0,30, 10, 0, L2}, {0, 0, 4, 0,0,2,0,0,6, 1, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_steal, SLOT (0), 0, 24,
     "", "!Steal!", ""},

    {
     "steal_from_bag", {L2, L2, L2, L2, L2, 35,L2,L2,L2, 10, L2, L2}, {-1, -1, -1, -1,-1, 3,-1,-1,-1, 1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_steal_from_bag, SLOT (0), 0, 24,
     "", "!Steal!", ""},

    {
     "store", {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20}, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_store, SLOT (0), 0, 24,
     "", "!Steal!", ""},

    {
     "study", {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20}, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_study, SLOT (0), 0, 24,
     "", "!Steal!", ""},


    {
     "scrolls", {1, 1, 1, 1,1, 1,1, 1,1, 1, 1, 1}, {2, 3, 5, 8,8,5,8,8,5, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_scrolls, SLOT (0), 0, 24,
     "", "!Scrolls!", ""},

    {
     "stance", {15, 15, 15, 15,15, 15, 15, 15,15, 15,15, 15}, {-3, -3, -3, -3,-3,-3, -3, -3, -3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_stance, SLOT (0), 0, 12,
     "", "You drop out of your combat stance", ""},

    {
     "staves", {1, 1, 1, 1,1, 1,1, 1,1, 1, 1, 1}, {2, 3, 5, 8,8,5,8,8,5, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_staves, SLOT (0), 0, 12,
     "", "!Staves!", ""},

    {
     "stealth", {15, 15, 15, 15,15,15, 15,15, 15, 1, 15, 15}, {-3, -3, -3, -3,-3,-3, -3, -3, -3, -1, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_stealth, SLOT (0), 0, 12,
     "", "You no longer feel stealthy.", ""},

    {
     "tame", {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50}, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_CHAR_OFFENSIVE, POS_STANDING,
     &gsn_tame, SLOT (0), 0, 24,
     "", "!Horse!", ""},

    {
     "wands", {1, 1, 1, 1,1,1, 1,1, 1, 1, 1, 1}, {2, 3, 5, 8,8,5,8,8,5, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_wands, SLOT (0), 0, 12,
     "", "!Wands!", ""},

    {
     "warcry", {15, 15, 15, 15,15,15,15, 15, 15,15, 15, 15}, {-3, -3, -3, -3,-3, -3, -3, -3, -3,-3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_warcry, SLOT (0), 0, 12,
     "", "The power of your warcry wears off", ""},


    {
     "medallion", {55, 55, 55, 55,55,55, 55, 55, 55, 55, 55, 55}, {-3, -3, -3, -3,-3,-3, -3, -3, -3,-3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_medallion, SLOT (0), 0, 12,
     "", "You remove the medallion.", ""},

    {
     "howl", {60, 60, 60, 60,60,60, 60, 60, 60, 60, 60, 60}, {-3, -3, -3, -3,-3, -3, -3, -3, -3,-3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_howl, SLOT (0), 0, 12,
     "", "Your howling becomes less effective.", ""},
    {
     "recall", {1, 1, 1, 1,1, 1, 1, 1,1, 1,1, 1}, {2, 2, 2, 2,2,2, 2, 2, 2, 1, 1, 1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_recall, SLOT (0), 0, 12,
     "", "!Recall!", ""},

    {
     "ravage", {15, 15, 15, 15,15, 15,15, 15,15, 15, 15, 15}, {0, 0, 0,0, 0, 0, 0, 0,0,0, 0, 0},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_ravage, SLOT (0), 0, 12,
     "", "The memory of the Seanchan Empire leaves you.", ""},
    {
     "contact", {15, 15, 15, 15,15,15, 15,15, 15, 15, 15, 15}, {-3, -3, -3, -3,-3,-3, -3, -3,-3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_OBJ_INV, POS_STANDING,
     &gsn_contact, SLOT (0), 0, 12,
     "", "The poison leaves you.", ""},
    {
     "taint", {40, 40, 40, 40,40, 40,40, 40,40, 40, 40, 40}, {-3, -3, -3, -3,-3,-3, -3, -3,-3, -1, -3, -3},
     {2, 2, 2, 2, 2},
     spell_taint, TAR_OBJ_INV, POS_STANDING,
     NULL, SLOT (0), 100, 22,
     "", "!taint!", ""},


    {
     "veil", {40, 40, 40, 40,40,40, 40,40, 40, 40, 40, 40}, {-3, -3, -3,-3, -3, -3,-3,-3, -3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_veil, SLOT (0), 0, 12,
     "", "You lower your veil", ""},

    {
     "void", {15, 15, 15, 15,15,15, 15,15, 15, 15, 15, 15}, {-3, -3, -3,-3, -3, -3,-3, -3, -3,-3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_void, SLOT (0), 0, 12,
     "", "!Void!", ""},

    {
     "wolf", {15, 15, 15,15, 15,15, 15,15, 15, 15, 15, 15}, {-3,-3, -3, -3, -3,-3,-3, -3, -3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_wolf, SLOT (0), 0, 12,
     "", "You can call a wolf again.", ""},
	 {
     "damane", {15, 15, 15,15, 15,15, 15,15, 15, 15, 15, 15}, {-3, -3, -3,-3, -3, -3,-3, -3, -3,-3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_damane, SLOT (0), 0, 12,
     "", "You can call a slave forth again.", ""},
     {	 	
      "track",{L2, L2, L2, L2,30,L2, L2, L2, L2, 6, 6, L2}, {0, 0, 0, 0,4,0, 0, 0, 0, 1, 1, 0},
      {2, 2, 2, 2, 2},
        spell_null, TAR_IGNORE, POS_RESTING,
        &gsn_track, SLOT( 0), 0, 2,
        "", "!track!"},

/* Demiscus new skills */
   // ORDER: {mage, cleric, assassin, warrior, racher, rogue, illuminator, form master, thief catcher, forsaken, dragon, herbalist}

    {
     "rub", {L2, L2, L2, 75,L2,L2,L2,75,L2, L2, L2, L2}, {-1, -1, -1, 4,-1,-1,-1,10,-1, 1, 1, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_rub, SLOT (0), 0, 4,
     "", "!Rub!", "You rub the dirt out of your eyes."},
   
    {
     "dance", {L2, L2, L2, L2,L2,L2,L2,75,L2, L2, L2, L2}, {0, 0, 0, 0,0,0,0,6,0, 0, 0, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_dance, SLOT (0), 0, 0,
     "", "!Dance!", ""},

    {
     "style", {L2, L2, L2, 15,L2,L2,L2,L2,L2, L2, L2, L2}, {0, 0, 0, 6,0,0,0,0,0, 0, 0, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_style, SLOT (0), 0, 12,
     "", "!Style!", ""},

    {
     "gash", {L2, L2, 75, L2,L2,L2,L2,L2,L2, L2, L2, L2}, {0, 0, 5, 0,0,0,0,0,0, 0, 0, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_gash, SLOT (0), 0, 20,
     "GASH", "!Gash!", ""},

    {
     "caltrops", {L2, L2, 75, L2,L2,L2,L2,L2,L2, L2, L2, L2}, {0, 0, 6, 0,0,0,0,0,0, 0, 0, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_caltraps, SLOT (0), 0, 12,
     "caltrops", "!Caltrops!", "The power of caltrops leaves you."},

    {
     "salve", {L2, L2, L2, 75,L2,L2,L2,L2,L2, L2, L2, L2}, {0, 0, 0, 5,0,0,0,0,0, 0, 0, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_salve, SLOT (0), 0, 12,
     "", "Your salve protection leaves you.", ""},

    {
     "dislodge", {L2, L2, L2, 75,L2,L2,L2,L2,L2, L2, L2, L2}, {0, 0, 0, 5,0,0,0,0,0, 0, 0, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_dislodge, SLOT (0), 0, 20,
     "", "!Dislodge!", ""},
    {
     "quickening", {L2, L2, L2, 75,L2,L2,L2,L2,L2, L2, L2, L2}, {0, 0, 0, 5,0,0,0,0,0, 0, 0, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_FIGHTING,
     &gsn_quickening, SLOT (0), 0, 12,
     "", "You feel yourself slowing down.", ""},

    {
     "rift", {L2, 8, L2, L2,L2,L2,L2,L2,L2, L2, L2, L2}, {1, 4, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 0, 4, 1, 2},
     spell_rift, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_rift, SLOT (205), 75, 12,
     "", "The powerful rift leaves you.", ""},

    {
     "disorient", {9, L2, L2, L2,L2,L2,L2,L2,L2, L2, L2, L2}, {1, 4, 2, 2,2,2,2,2,2, 1, 1, L2},
     {0, 1, 2, 1, 2},
     spell_disorient, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_disorient, SLOT (206), 50, 6,
     "", "You feel less disoriented.", ""},

    {
     "fire blossom", {75, L2, L2, L2, L2, L2, L2, L2, L2, 5, 5, L2}, {1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, L2},
     {2, 2, 2, 2, 2},
     spell_fire_blossom, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL, SLOT (70), number_range(100,160), 20,
     "blossom of fire", "!Fire Blossom!"},

    {
     "trap disarm", {L2, L2, L2, L2, 75, L2, L2, L2, L2, L2, L2, L2}, {-1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_trapdisarm, SLOT (0), number_range(100,200), 10,
     "", ""},
    {
     "trap set", {L2, L2, L2, L2, 75, L2, L2, L2, L2, L2, L2, L2}, {-1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_trapset, SLOT (0), 0, 6,
     "trap", ""},
    {
     "trapset", {L2, L2, L2, L2, 75, L2, L2, L2, L2, L2, L2, L2}, {-1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_trapset2, SLOT (0), 0, 6,
     "trap", ""},

    {
     "blind sight", {L2, L2, L2, L2, L2, 75, L2, L2, L2, L2, L2, L2}, {-1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_blind_sight, SLOT (0), 0, 6,
     "", ""},
    {
     "gouge", {L2, L2, L2, L2, L2, 75, L2, L2, L2, L2, L2, L2}, {-1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_gouge, SLOT (0), 0, 6,
     "gouge","The soreness of your eyes becomes better.", ""},
    {
     "unseen fury", {L2, L2, 75, L2, L2, L2, L2, L2, L2, L2, L2, L2}, {-1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1},
     {2, 2, 2, 2, 2},
     spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     &gsn_unseen_fury, SLOT (0), 0, 6,
     "","You no longer have the fury.", ""},
   {
     "blindworm", {75, L2, L2,L2, L2,L2, L2,L2, L2, L2, L2, L2}, {3,-3, -3, -3, -3,-3,-3, -3, -3, -3, -3, -3},
     {2, 2, 2, 2, 2},
     spell_null, TAR_IGNORE, POS_STANDING,
     &gsn_darkhound, SLOT (0), 0, 12,
     "", "You can call a blindworm again.", ""},
         

};

const struct group_type group_table[MAX_GROUP] = {

    {
     "rom basics", {0, 0, 0, 0, 0, 0, 0,0,0,0,0,0},
     {"scrolls", "wands", "recall",}
     },

    {
     "mage basics", {0, -1, -1, -1,-1,-1,-1,-1,-1,-1,-1, -1},
     {"dagger", "shield block", "disorient", "bandage", "couple"}
     },

    {
     "cleric basics", {-1, 0, -1, -1,-1,-1,-1,-1,-1,-1,-1,-1},
     {"mace", "shield block", "rift", "bandage", "couple"}
     },

    {
     "assassin basics", {-1, -1, 0, -1,-1,-1,-1,-1,-1,-1,-1,-1},
     {"dagger", "assassinate", "caltrops", "bandage", "unseen fury"}
    },

    {
     "rogue basics", {-1, -1, -1, -1,-1,-1,-1,0,-1,-1,-1,-1},
     {"dagger", "backstab", "bandage", "gouge", "blind sight"}
    },
    
    {
     "illuminator basics", {-1, -1, -1, -1,-1,-1,-1,-1,0,-1,-1,-1},
     {"staff", "bandage"}
     },

    {
     "form master basics", {-1, -1, -1, -1,-1,-1,-1,-1,-1,0,-1,-1},
     {"staff", "bandage", "martial arts", "dance"}
    },

    {
     "thief catcher", {-1, -1, -1, -1,-1,-1,-1,-1,-1,-1,0,-1},
     {"bandage", "staff", "backstab"}
     },
    {
     "warrior basics", {-1, -1, -1, 0,-1,-1,-1,-1,-1,-1,-1,-1},
     {"bandage", "sword", "second attack", "style"}
    },
    
    {
     "archer basics", {-1, -1, -1, -1,0,-1,-1,-1,-1,-1,-1,-1},
     {"bow", "archery", "bandage"}
     },
    {/*p1*/
     "forsaken basics", {-1, -1, -1, -1},
     {"dagger", "sword", "fast healing", "meditation", "combat",
      "protective", "transportation", "detection", "illusion", "parry",
      "enchantment", "peek", "steal", "focus", "air barrier", "mana shield", "fire blossom"}
    },

    {
     "forsaken default", {-1, -1, -1, -1},
     {"lore", "assassinate", "backstab", "draconian", "enhancement", "second attack",
      "third attack", "stealth", "shroud", "maladictions", "pray", "enhanced damage",
      "hand to hand", "shield block", "dodge","bandage","disarm"}
    },

    { 
     "dragon basics", {-1, -1, -1, -1},
     {"dagger", "sword", "fast healing", "meditation", "combat",
      "protective", "transportation", "detection", "illusion", "parry",
      "enchantment", "dual wield", "beguiling", "curative", "spirit",
       "air barrier", "mana shield", "focus", "hide", "sneak","creation"}
    },

    {
     "dragon default", {-1, -1, -1, -1},
     {"benedictions", "assassinate", "backstab", "draconian", "enhancement", "second attack",
      "third attack", "firestorm", "maladictions", "restrain", "enhanced damage",
      "hand to hand", "shield block", "dodge","bandage","disarm"}
    },

     

    {
     "druidonics", {-1,-1,-1,-1,-1},
     {"stone skin", "infravision", "fly", "pass door", "combat",
      "creation", "sonic blast"}
    },
    {
     "herbalist basics", {-1, -1, -1, -1,-1,-1,-1,-1,-1,-1,-1,0 },
     {"staff"}
     },

    {
     "mage default", {89, -1, -1, -1,-1,-1,-1,-1,-1},
     { "beguiling", "combat", "detection", "draconian", "enchantment",
       "enhancement", "illusion", "maladictions", "protective", 
       "barriers", "transportation", "bandage", "dodge", "parry",
       "fire blossom", "focus", "mediation", "shield block" }
     },

    {
     "cleric default", {-1, 88, -1, -1,-1,-1,-1,-1,-1},
     { "attack", "benedictions", "curative", "detection", "enhancement",
       "healing", "offensive shields", "protective", "transporation",
       "shield block", "bandage", "dodge", "enhanced damage", "parry",
       "fast healing", "meditation" }
     },

    {
     "assassin default", {-1, -1, 123, -1,-1,-1,-1,-1,-1},
      {
         "transportation", "backstab", "bandage", 
         "dirt kicking", "dodge", "enhanced damage", "parry", "second attack",
         "third attack", "cloak", "dual wield", "hide", "peek", "sneak",
         "steal" }  
     },

    {
     "rogue default", {-1, -1, -1, -1,-1,120,-1,-1,-1},
     { 
       "transportation", "shield block", "bandage", 
       "dirt kicking", "dodge", "enhanced damage", "parry", "second attack",
       "blackjack", "hide", "openbag", "peek", "peek_in", 
       "sneak", "steal", "steal_from_bag"
     } 
     },

     {
     "illuminator default", {-1, -1, -1, -1,-1,-1, 118,-1,-1},
     { "combat", "transportation",
       "shield block", "bandage", "dirt kicking", "dodge", 
       "enhanced damage", "parry", "second attack", "bomb", "dampen",
       "disguise", "fast healing", "haggle", "counter" 
     }
     },

     {
     "form master default", {-1, -1, -1, -1,-1,-1,-1,110,-1},
     {
       "transportation", "rub",
       "shield block", "bandage", "dirt kicking", "enhanced damage",
       "second attack", "third attack", "strike", "evade", "block",
       "ironskin", "flying kick", "shadow kick", "flip", "sneak",
       
     }
     },

     {
     "thief catcher default", {-1, -1, -1, -1,-1,-1,-1,-1,40},
     {"steal", "sword", "parry", "disarm", "dodge", "second attack",
      "trip", "hide", "peek", "pick lock", "sneak", 
     	}
     },

    {
     "warrior default", {-1, -1, -1, 111,-1,-1,-1,-1,-1},
     { 
       "transportation", "bandage", "bash", "salve", "quickening",
       "fourth attack", "counter", "dirt kicking", "dodge", "enhanced damage",
       "parry", "third attack", "fourth attack", "dual wield",
       "fast healing", "hide"
     } 
     },

     {
     "archer default", {-1, -1, -1, -1, 112,-1,-1,-1,-1},
     { "transportation", 
       "bandage", "dirt kicking", "enhanced damage", "parry", 
       "second attack", "third attack", "advanced archery", "hide",
       "meditation", "peek", "dodge", "fast healing"
     }
     },
    
    {
     "weaponsmaster", {-1, -1, 40, 15,30,40,40,40,40},
     {"axe", "dagger", "flail", "mace", "polearm", "spear", "sword", "whip","staff"}
     },

    {
     "attack", {-1, 5, -1, -1,-1,-1,-1,-1,-1, -1},
     {"demonfire", "dispel evil", "dispel good", "earthquake",
      "flamestrike", "heat metal", "ray of truth"}
     },

    {
     "beguiling", {5, 3, -1, -1,-1,-1,-1,-1,-1, -1},
     {"calm", "charm person", "sleep"}
     },

    {
     "benedictions", {-1, 5, -1, -1,-1,-1,-1,-1,-1,-1}, 
     {"bless", "calm", "frenzy", "holy word", "remove curse"}
     },

    {
     "combat", {8, 4, -1, -1,-1,-1,-1,-1,-1, -1},
     {"acid blast", "burning hands", "chain lightning", "chill touch",
      "colour spray", "fireball", "lightning bolt", "magic missile",
      "shocking grasp"}
     },

    {
     "creation", {2, 2, -1, -1,-1,-1,-1,-1,-1, -1},
     {"continual light", "create food", "create spring", "create water",
      "create rose", "floating disc", "berry"}
     },

    {
     "curative", {-1, 5, -1, -1,-1,-1,-1,-1,-1, -1},
     {"cure blindness", "cure disease", "cure poison"}
     },

    {
     "detection", {6, 5, -1, -1,-1,-1,-1,-1,-1, -1},
     {"detect evil", "detect good", "detect hidden", "detect invis",
      "detect magic", "detect poison", "farsight", "identify",
      "know alignment", "locate object"}
     },

    {
     "draconian", {12, -1, -1, -1,-1,-1,-1,-1,-1, -1},
     {"acid breath", "fire breath", "frost breath", "gas breath",
      "lightning breath"}
     },

    {
     "enchantment", {9, 4, -1, -1,-1,-1,-1,-1,-1, -1},
     {"enchant armor", "enchant weapon", "fireproof", "recharge",
       "curse object","iron grip","disenchant"}
     },

    {
     "enhancement", {6, 9, -1, -1,-1,-1,-1,-1,-1, -1},
     {"giant strength", "haste", "infravision", "refresh"}
     },

    {
     "harmful", {-1, 3, -1,-1,-1,-1,-1,-1,-1, -1},
     {"cause critical", "cause light", "cause serious", "harm", "simulacrum"}
     },

    {
     "healing", {-1, 5, -1, -1,-1,-1,-1,-1,-1, -1},
     {"cure critical", "cure light", "cure serious", "heal",
      "mass healing", "refresh", "berry"}
     },

    {
     "illusion", {5, 5, -1, -1,-1,-1,-1,-1,-1, -1},
     {"invis", "mass invis", "ventriloquate", 
      "fear", "frighten"}
     },

    {
     "maladictions", {8, 6, -1, -1,-1,-1,-1,-1,-1, -1},
     {"blindness", "change sex", "curse", "energy drain", "plague",
      "poison", "slow", "weaken", "poison missile"}
     },
	
	{
     "offensive shields", {-1, 12, -1, -1,-1,-1,-1,-1,-1, -1},
     {"ice shield", "fire shield","shock shield"}
     },

    {
     "protective", {5, 5, -1, -1,-1,-1,-1,5,5, 5},
     {"armor", "cancellation", "dispel magic", "fireproof",
      "protection evil", "protection good", "sanctuary", "shield",
      "stone skin"}
     },

    {
     "barriers", {7, -1, -1, -1, -1, -1, -1, -1, -1 -1},
     {"mana shield", "air barrier"}
    },    

    {
     "transportation", {5, 6, 8, 10,10,8,6,10,8, -1},
     {"fly", "gate", "nexus", "pass door", "portal", "summon", "teleport",
      "word of recall"}
     },

    {
     "weather", {5, 4, -1, -1,-1,-1,-1,-1,-1,-1},
     {"call lightning", "control weather", "faerie fire", "faerie fog",
      "lightning bolt"}
     }



};
