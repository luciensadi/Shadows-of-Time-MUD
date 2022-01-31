
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
*    ROM 2.4 is copyright 1993-1998 Russ Taylor                             *
*    ROM has been brought to you by the ROM consortium                      *
*        Russ Taylor (rtaylor@hypercube.org)                                *
*        Gabrielle Taylor (gtaylor@hypercube.org)                           *
*        Brian Moore (zump@rom.org)                                         *
*    By using this code, you have agreed to follow the terms of the         *
*    ROM license, in the file Rom24/doc/rom.license                         *

****************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"

bool check_social args ((CHAR_DATA *, char *, char *,long, char *));

/*
 * Command logging types.
 */
#define LOG_NORMAL    0
#define LOG_ALWAYS    1
#define LOG_NEVER     2
#define LOG_IMMORTAL  3



/*
 * Log-all switch.
 */
bool fLogAll = FALSE;


char last_command[MSL];
/*
 * Command table.
 */
const struct cmd_type cmd_table[] = {
  //  {"note", do_note, POS_SLEEPING, 0, LOG_NEVER, 0},
/*
     * Common movement commands.
     */
    {"north", do_north, POS_STANDING, 0, LOG_NEVER, 0, CMD_INTER},
    {"east", do_east, POS_STANDING, 0, LOG_NEVER, 0, CMD_INTER},
    {"south", do_south, POS_STANDING, 0, LOG_NEVER, 0, CMD_INTER},
    {"west", do_west, POS_STANDING, 0, LOG_NEVER, 0, CMD_INTER},
    {"up", do_up, POS_STANDING, 0, LOG_NEVER, 0, CMD_INTER},
    {"down", do_down, POS_STANDING, 0, LOG_NEVER, 0, CMD_INTER},

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    {"at", do_at, POS_DEAD, L1, LOG_NORMAL, 1, CMD_INTER},
    {"weave", do_cast, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"channel", do_cast, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    // {"dice", do_crowns, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"couple", do_couple, POS_DEAD, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"showclass", do_showclass, POS_DEAD,0, LOG_NORMAL, 1, CMD_INTER},
    //{"auction", do_auction, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"look", do_look, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"bandage", do_bandage, POS_STANDING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"bomb", do_bomb, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"bond", do_bond, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"build", do_build, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"buy", do_buy, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"bestow", do_bestow, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"channels", do_channels, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"disguise", do_disguise, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"drag", do_drag, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INTER},
    {"exits", do_exits, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"enhanced", do_enhanced, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"get", do_get, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"goto", do_goto, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INTER},
    {"group", do_group, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"guild", do_guild, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INTER},
    {"guildlist", do_guildlist, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"warstatus", do_warstatus, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"hit", do_kill, POS_FIGHTING, 0, LOG_NORMAL, 0, CMD_INTER},
    {"push", do_push, POS_STANDING, 0, LOG_NORMAL, 0, CMD_INTER},
  //  {"power", do_power, POS_STANDING, ML, LOG_NORMAL, 0, CMD_INFO},
    {"dragon", do_dragontalk, POS_SLEEPING, 0, LOG_NORMAL, 0, CMD_COMM},
   // {"font", do_font, POS_SLEEPING, 0, LOG_NORMAL, 0, CMD_INFO},
    {"inventory", do_inventory, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"ignite", do_ignite, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"kill", do_kill, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"joinwar", do_joinwar, POS_SLEEPING, 11, LOG_NORMAL, 1, CMD_INTER},
    {"leavewar", do_leavewar, POS_SLEEPING, 11, LOG_NORMAL, 1, CMD_INTER},
    {"leaveguild", do_leaveguild, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"last", do_last, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"gdt", do_clantalk, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"music", do_music, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"osay", do_osay, POS_RESTING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"practice", do_practice, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"promote", do_promote, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"enchant", do_penchant, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INTER},
    {"rest", do_rest, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"restring", do_restring, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"second", do_second, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"sense", do_sense, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"simulacrum", do_simulacrum, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"sit", do_sit, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"sockets", do_sockets, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
    {"stand", do_stand, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"study", do_study, POS_RESTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"whisper", do_whisper, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"tell", do_tell, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
   // {"otell", do_tell, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"tournament", do_tournament, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"unlock", do_unlock, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"warchar", do_warchar, POS_SLEEPING, L2, LOG_NORMAL, 1, CMD_INTER},
    {"startwar", do_startwar, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"warder", do_wardertalk, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"wield", do_wear, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"cover", do_cover, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"wizhelp", do_wizhelp, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INFO},
    {"offense", do_guide_offense, POS_DEAD, 0, LOG_ALWAYS, 1, CMD_INTER},
    {"gflag", do_guide_flag, POS_DEAD, 0, LOG_ALWAYS, 1, CMD_INTER},
    {"gjail", do_guide_jail, POS_DEAD, 0, LOG_ALWAYS, 1, CMD_INTER},
    {"nstat", do_nstat, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},

    /*
     * Informational commands.
     */
    {"affects", do_affects, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"areas", do_areas, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"bug", do_bug, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"changes", do_changes, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"guide", do_guide, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"commands", do_commands, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"compare", do_compare, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"consider", do_consider, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"count", do_mudstats, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"credits", do_credits, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"elements", do_elements, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"equipment", do_equipment, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"examine", do_examine, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INFO},
/*  { "groups",        do_groups,    POS_SLEEPING,     0,  LOG_NORMAL, 1 }, */
    {"help", do_ohelp, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"helpindex", do_new_help, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"idea", do_idea, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"info", do_groups, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"mudstats", do_mudstats, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"motd", do_motd, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"news", do_news, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"read", do_read, POS_RESTING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"report", do_report, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"rules", do_rules, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"score", do_score, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"scry", do_scry, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"diem", do_who, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"skills", do_skills, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"socials", do_socials, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"show", do_show, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"spells", do_spells, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"story", do_story, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"time", do_time, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"topten", do_top_ten, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"typo", do_typo, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"vote", do_vote, POS_DEAD, 0, LOG_ALWAYS, 1, CMD_COMM},
    {"weather", do_weather, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"forget", do_forget, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INTER},
    {"who", do_who, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"whois", do_whois, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"wizlist", do_wizlist, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"worth", do_worth, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"work", do_work, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"lore", do_lore, POS_RESTING, 0, LOG_NORMAL,1, CMD_INFO},
    {"lab", do_lab_monkey, POS_DEAD, ML, LOG_NORMAL,1,CMD_INFO},
    {"worldmap", do_worldmap, POS_SLEEPING, 0, LOG_NORMAL, 0, CMD_INFO},

    /*
     * Configuration commands.
     */
    {"alia", do_alia, POS_DEAD, 0, LOG_NORMAL, 0, CMD_INFO},
    {"alias", do_alias, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"autolist", do_autolist, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"autoall", do_autoall, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"autoassist", do_autoassist, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"autoexit", do_autoexit, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"autogold", do_autogold, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"autoloot", do_autoloot, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"autosac", do_autosac, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"autosplit", do_autosplit, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"battlemeter", do_battlemeter, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"brief", do_brief, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"colour", do_colour, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"color", do_colour, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"combine", do_combine, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"compact", do_compact, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"description", do_description, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"delet", do_delet, POS_DEAD, 0, LOG_ALWAYS, 0, CMD_INTER},
    {"delete", do_delete, POS_STANDING, 0, LOG_ALWAYS, 1, CMD_INTER},
    {"hungerflag", do_hungerflag, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"ignore", do_ignore, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"noexp", do_noexp, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"nofollow", do_nofollow, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"nosummon", do_nosummon, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"norp", do_norp, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"notitle", do_notitles, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"no_damage", do_nodamage, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"no_weave", do_noweave, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"outfit", do_outfit, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"password", do_password, POS_DEAD, 0, LOG_NEVER, 1, CMD_INFO},
    {"prompt", do_prompt, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"scroll", do_scroll, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
	{"telnetga", do_telnet_ga, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"pretitle", do_pretitle, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INFO},
    {"title", do_title, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"unalias", do_unalias, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"wimpy", do_wimpy, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},

    /*
     * Communication commands.
     */
    {"afk", do_afk, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"answer", do_answer, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    { "auction",    do_auction,    POS_SLEEPING,     0,  LOG_NORMAL, 1, CMD_COMM },
    { "bitch", do_bitch, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    { "bofh", do_bofh, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
  //  { "quotester", do_diem, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"deaf", do_deaf, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"emote", do_emote, POS_RESTING, 0, LOG_NORMAL, 1, CMD_COMM},
   // {"pmote", do_pmote, POS_RESTING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"[", do_gossip, POS_SLEEPING, 0, LOG_NORMAL, 0, CMD_COMM},
    {"gossip", do_gossip, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {",", do_emote, POS_RESTING, 0, LOG_NORMAL, 0, CMD_COMM},
    {"grats", do_grats, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"gtell", do_gtell, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"ispell", do_ispell, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"kill_spell", do_ispell_done, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
    {"start_spell", do_ispell_start, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
    {";", do_gtell, POS_DEAD, 0, LOG_NORMAL, 0, CMD_COMM},
    {"msp", do_msp, POS_SLEEPING, 0, LOG_NORMAL, 0, CMD_INFO},
    {"mxp", do_mxp, POS_SLEEPING, 0, LOG_NORMAL, 0, CMD_INFO},
/*  { "music",        do_music,    POS_SLEEPING,     0,  LOG_NORMAL, 1 }, */
    {"note", do_note, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"gnote", do_guildmaster, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"rpnote", do_rp, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"snote", do_snote, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"ooc", do_ooc, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"order", do_order, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"novice", do_novice, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"ntoggle", do_ntoggle, POS_DEAD, 0, LOG_NORMAL, 1, CMD_COMM},
    {"pose", do_pose, POS_RESTING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"quest", do_quest, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"questinfo", do_questinfo, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"quote", do_quote, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
   // {"quotester", do_diem, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"quiet", do_quiet, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"qtalk", do_qtalk, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"reply", do_oreply, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"ireply", do_ireply, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"oreply", do_oreply, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"replay", do_replay, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"say", do_say, POS_RESTING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"'", do_say, POS_RESTING, 0, LOG_NORMAL, 0, CMD_COMM},
    {"shout", do_shout, POS_RESTING, 3, LOG_NORMAL, 1, CMD_COMM},
    {"radio", do_radio, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"trivia", do_trivia, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"ttalk", do_ttalk, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"unread", do_unread, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"yell", do_yell, POS_RESTING, 0, LOG_NORMAL, 1, CMD_COMM},
    {"think", do_think, POS_RESTING, 0, LOG_NORMAL, 1, CMD_COMM},

    /*
     * Object manipulation commands.
     */
    {"brandish", do_brandish, POS_RESTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"close", do_close, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"discharge", do_discharge, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"donate", do_donate, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"drink", do_drink, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"drop", do_drop, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"eat", do_eat, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"envenom", do_envenom, POS_RESTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    //{"contactpoison", do_contactpoison, POS_RESTING, 0, LOG_NORMAL, 1, CMD_SKILL},

    {"fill", do_fill, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"fix", do_fix, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"give", do_give, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"heal", do_heal, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"hold", do_wear, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"list", do_list, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"lock", do_lock, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
//    {"masturbate", do_masturbate, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"open", do_open, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"openbag", do_openbag, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"pick", do_pick, POS_RESTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"pour", do_pour, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"put", do_put, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"quaff", do_quaff, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"recite", do_recite, POS_RESTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"remove", do_remove, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"remember", do_remember, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INTER},
    {"sell", do_sell, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"take", do_get, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"sacrifice", do_sacrifice, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"junk", do_sacrifice, POS_RESTING, 0, LOG_NORMAL, 0, CMD_INTER},
    {"value", do_value, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"wear", do_wear, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"zap", do_zap, POS_RESTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"corpseretrieve",do_cr,POS_RESTING,0,LOG_NORMAL,1, CMD_INTER},

    /* Bank Code - Sandy */
    {"balance", do_balance, POS_STANDING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"withdraw", do_withdraw, POS_STANDING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"deposit", do_deposit, POS_STANDING, 0, LOG_NORMAL, 1, CMD_INTER},
    /*
     * Combat commands.
     */
    {"assassinate", do_assassinate, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"backstab", do_backstab, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"bash", do_bash, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"blackjack", do_blackjack, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"bs", do_backstab, POS_FIGHTING, 0, LOG_NORMAL, 0, CMD_SKILL},
    {"berserk", do_berserk, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"cleanse", do_cleanse, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"damane", do_damane, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"dampen", do_dampen, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"daze",   do_daze, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"dirt", do_dirt, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"disarm", do_disarm, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"ensnare", do_ensnare, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"firestorm", do_firestorm, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"flee", do_flee, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"flip", do_flip, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"focus", do_focus, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"kick", do_kick, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"tap", do_manadrain, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"medallion", do_medallion, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"murde", do_murde, POS_FIGHTING, 0, LOG_NORMAL, 0, 0},
    {"murder", do_murder, POS_FIGHTING, 5, LOG_NORMAL, 1, CMD_SKILL},
    {"pray", do_pray, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"ravage", do_ravage, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"relax", do_relax, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"rescue", do_rescue, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"restrain", do_restrain, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"stance", do_stance, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"still", do_still, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
 //   {"surrender", do_surrender, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"throw", do_throw, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"trip", do_trip, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"veil", do_veil, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"howl", do_howl, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"warcry", do_warcry, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"wolf", do_wolf, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"engage",do_engage,POS_FIGHTING,0,LOG_NORMAL,1, CMD_SKILL},
    {"shoot",do_shoot,POS_STANDING,0,LOG_NORMAL,1, CMD_SKILL},
    {"draw",do_ddraw,POS_STANDING,0,LOG_NORMAL,1, CMD_SKILL},

/* Demiscus new skills */

    {"rub", do_rub, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"dance", do_dance, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"style", do_style, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"gash", do_gash, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"caltrops", do_caltrops, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"salve", do_salve, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"dislodge", do_dislodge, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"quickening", do_quickening, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    { "trap", do_trap, POS_FIGHTING,0, 1, LOG_NORMAL, 1 },
    {"gouge", do_gouge, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"unseenfury", do_unseen_fury, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"blindworm", do_darkhound, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_SKILL},

/* RP cool commands */

    {"smoke", do_smoke, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"exhale", do_exhale, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},

/*
 *  Shadow stuff
 */
    {"enslave", do_enslave, POS_RESTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"minion", do_mintalk, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"pledge", do_pledge, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    /*
     * Mob command interpreter (placed here for faster scan...)
     */
    {"mob", do_mob, POS_DEAD, 0, LOG_NEVER, 0, 0},

    /*
     * Miscellaneous commands.
     */
    {"peek", do_peek, POS_RESTING,0,LOG_NORMAL,1, CMD_SKILL},
    {"peek_in", do_peek_in, POS_RESTING,0,LOG_NORMAL,1, CMD_SKILL},
    {"scan", do_scanning, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"ballad", do_ballad, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"dream", do_dream, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"enter", do_enter, POS_STANDING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"follow", do_follow, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"gain", do_gain, POS_STANDING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"gameinfo", do_gameinfo, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"go", do_enter, POS_STANDING, 0, LOG_NORMAL, 0, CMD_INTER},
    {"groups", do_groups, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"hide", do_hide, POS_RESTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"play", do_play, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"qui", do_qui, POS_DEAD, 0, LOG_NORMAL, 0, CMD_INTER},
    {"quit", do_quit, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INTER},
    {"recall", do_recall, POS_FIGHTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"/", do_recall, POS_FIGHTING, 0, LOG_NORMAL, 0, CMD_INTER},
    {"rent", do_rent, POS_DEAD, 0, LOG_NORMAL, 0, CMD_INTER},
    {"save", do_save, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INTER},
    {"shroud", do_shroud, POS_RESTING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"sleep", do_sleep, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"sneak", do_sneak, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"split", do_split, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"steal", do_steal, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"steal_from_bag", do_steal_from_bag, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"stealth", do_stealth, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"train", do_train, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"visible", do_visible, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"wake", do_wake, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_INTER},
    {"where", do_where, POS_RESTING, 0, LOG_NORMAL, 1, CMD_INFO},
    {"track", do_track, POS_STANDING, 0 , LOG_NORMAL, 1, CMD_SKILL},
    {"cloak", do_disguiseself, POS_STANDING, 0 , LOG_NORMAL, 1, CMD_SKILL},

    /* Mount */
    {"mount", do_mount, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"dismount", do_dismount, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"tame", do_tame, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"spur", do_spur, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},
    {"reinin", do_reinin, POS_STANDING, 0, LOG_NORMAL, 1, CMD_SKILL},


    /*
     * Immortal commands.
     */
    { "todo",           do_todo,        POS_DEAD,       IM, 1, LOG_NORMAL, 1 },
    { "tocode",         do_tocode,      POS_DEAD,       IM, 1, LOG_NORMAL, 1 },
    { "agenda",         do_agenda,      POS_DEAD,       IM, 1, LOG_NORMAL, 1 },
    { "changed",        do_changed,     POS_DEAD,       IM, 1, LOG_NORMAL, 1 },

    {"award", do_award, POS_DEAD, 78, LOG_IMMORTAL, 1, CMD_INTER},
    {"arealinks", do_arealinks, POS_DEAD, ML, LOG_NORMAL, 1 },
    {"advance", do_advance, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"olevel", do_olevel, POS_DEAD,  ML, LOG_NORMAL, 1 },
    {"mlevel", do_mlevel, POS_DEAD, ML, LOG_NORMAL, 1 },
    {"omni", do_omni, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INFO},
    {"copyover", do_copyover, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"otransfer", do_otransfer, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"pload", do_pload, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"punload", do_punload, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"slookup", do_slookup, POS_DEAD, L2, LOG_NORMAL, 1, CMD_INFO},
    {"slot", do_slot, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INTER},
    {"pipe", do_pipe, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"dump", do_dump, POS_DEAD, ML, LOG_IMMORTAL, 0, CMD_INFO},
    {"trust", do_trust, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"rwhere", do_rwhere, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
    {"violate", do_violate, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"rstpass",  do_resetpassword,  POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"confiscate", do_confiscate,  POS_DEAD, L2, LOG_IMMORTAL, 1, CMD_INTER},
    {"severchar", do_sever, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"spellup", do_spellup, POS_DEAD, L1, LOG_NORMAL, 1, CMD_INTER},
    {"unrestore", do_unrestore, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"questedit", do_questedit, POS_DEAD, IM, LOG_NORMAL, 1, CMD_INTER},

    {"add_extra", do_add_extra, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"addlag", do_addlag, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"add_weapon", do_add_weapon_flag, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"allow", do_allow, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"avedam", do_avedam, POS_DEAD, IM, LOG_NORMAL, 1, CMD_INFO},
    {"banperson", do_ban, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"bonustime", do_bonus_time, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"minustime", do_minus_time, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"bugfix", do_bugfix, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"deny", do_deny, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"disconnect", do_disconnect, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"ienchant", do_enchant, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"flag", do_flag, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"freeze", do_freeze, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"permban", do_permban, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"protect", do_protect, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
//    {"reboo", do_reboo, POS_DEAD, L1, LOG_NORMAL, 0, CMD_INTER},
//    {"reboot", do_reboot, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"rename", do_rename, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"set", do_set, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"shutdow", do_shutdow, POS_DEAD, ML, LOG_NORMAL, 0, CMD_INTER},
    {"shutdown", do_shutdown, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    { "unenchant", do_unenchant, POS_DEAD , L1, LOG_NORMAL, 1, CMD_INTER},
    {"wizlock", do_wizlock, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},

/* Castles */


    {"map", do_showthemap, POS_DEAD, 1, LOG_NORMAL, 1, CMD_INFO},
  //  {"setwilderness", do_set_wilderness_all, POS_DEAD, IM, LOG_NORMAL, 1, CMD_INFO},
    {"mapsize", do_mapsize, POS_DEAD, 1, LOG_NORMAL, 1, CMD_INFO},
    {"claim", do_claim, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
    {"assign", do_assign, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
    {"construct", do_construct, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
//    {"note_to_mysql", do_note_mysql, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"call_level", do_mobprog_clear, POS_DEAD, IM, LOG_NORMAL, 1, CMD_OLC},
    {"clearheal", do_clearheal, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"checklog", do_checklog, POS_DEAD, L1, LOG_NORMAL, 1, CMD_INFO},
    {"developer", do_developer, POS_DEAD, L1, LOG_NORMAL, 1, CMD_INTER},
    {"eqlookup", do_eqlookup, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
    {"force", do_force, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"forcetick", do_forcetick, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"gecho", do_echo, POS_DEAD, ML, LOG_NORMAL, 1, CMD_COMM},
    {"gen_name", do_generate_names, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INFO},
    {"cleartick", do_cleartick, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"grant", do_grant, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"ungrant", do_ungrant, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"load", do_load, POS_DEAD, L3, LOG_IMMORTAL, 1, CMD_INTER},
    {"istat", do_istat, POS_DEAD, 1, LOG_NORMAL, 1, CMD_INFO},
    {"idle", do_idle, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
    {"lstat", do_lstat, POS_DEAD, 1, LOG_NORMAL, 1, CMD_INFO},
    {"makegl", do_makegl, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"makemap", do_makemap, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"fillmap", do_fillmap, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"newlock", do_newlock, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"nochannels", do_nochannels, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_COMM},
    {"noemote", do_noemote, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"notell", do_notell, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"pecho", do_pecho, POS_DEAD, L1, LOG_NORMAL, 1, CMD_COMM},
    {"purge", do_purge, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INTER},
    {"questchar", do_questchar, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INTER},
    {"tourneychar", do_tourneychar, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"tagchar", do_tagchar, POS_DEAD, L1, LOG_NORMAL, 1, CMD_INTER},
    {"itchar", do_itchar, POS_DEAD, L1, LOG_ALWAYS, 1, CMD_INTER},
    {"rphp", do_rphp, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"maxstats", do_maxstats, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
    {"pardon", do_pardon, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"pmult", do_personal_mult, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"puke", do_puke, POS_DEAD, IM, LOG_IMMORTAL, 1, CMD_INFO},
    {"print_delay", do_print_delay, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"print_objects", do_print_objects, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"print_mobiles", do_print_mobiles, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"print_mud", do_printmud, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"restore", do_restore, POS_DEAD, L2, LOG_IMMORTAL, 1, CMD_INTER},
    {"rescue_test", do_rescue_test, POS_DEAD, IM, LOG_NORMAL, 1, CMD_INTER},
    {"test_act", do_test_act, POS_DEAD, IM, LOG_NORMAL, 1, CMD_INTER},
    {"roster", do_roster, POS_DEAD, 1, LOG_NORMAL, 1, CMD_INFO},
    {"save_vote", do_save_vote, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"load_guilds", do_load_guilds, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"save_guilds", do_save_guilds, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"load_qprize", do_load_qprize, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"save_qprize", do_save_qprize, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"savegroups", do_savegroup, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
//    {"seperate", do_seperate, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
    {"skillstat", do_skillstat, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INFO},
    {"skillstrip", do_skillstrip, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
//    {"skillreset", do_skillreset, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"sla", do_sla, POS_DEAD, ML, LOG_NORMAL, 0, CMD_INTER},
    {"slay", do_slay, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"sn", do_sn, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},
    {"tan", do_tan, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"teleport", do_transfer, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"testasmo", do_testasmo, POS_DEAD, IM, LOG_NORMAL, 1, CMD_INTER},
    {"transfer", do_transfer, POS_DEAD, L2, LOG_IMMORTAL, 1, CMD_INTER},
    {"vote_create", do_vote_create, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"webhelp", do_webhelp, POS_DEAD, L2, LOG_NORMAL, 1, CMD_INTER},
    {"questrestart", do_quest_restart, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"poofin", do_bamfin, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INTER},
    {"poofout", do_bamfout, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INTER},
    {"echo", do_echo, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_COMM},
    {"holylight", do_holylight, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INTER},
    {"incognito", do_incognito, POS_DEAD, L2, LOG_NORMAL, 1, CMD_INTER},
    {"diemcloak", do_diemcloak, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    {"invis", do_invis, POS_DEAD, L3, LOG_NORMAL, 0, CMD_INTER},
    {"log", do_log, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},

    {"memory", do_memory, POS_DEAD, L2, LOG_NORMAL, 1, CMD_INFO},
    {"mwhere", do_mwhere, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INFO},
    {"owhere", do_owhere, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INFO},
    {"peace", do_peace, POS_DEAD, L2, LOG_NORMAL, 1, CMD_INTER},
    {"check", do_check, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INFO},

    {"recho", do_recho, POS_DEAD, L2, LOG_NORMAL, 1, CMD_COMM},
    {"return", do_return, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INTER},
    {"snoop", do_snoop, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"stat", do_stat, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INFO},
    {"string", do_string, POS_DEAD, L2, LOG_NORMAL, 1, CMD_INTER},
    {"switch", do_switch, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INTER},
    {"wizinvis", do_invis, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INTER},
    {"vnum", do_vnum, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INFO},
    {"zecho", do_zecho, POS_DEAD, ML, LOG_NORMAL, 1, CMD_COMM},

    {"clone", do_clone, POS_DEAD, L2, LOG_IMMORTAL, 1, CMD_INTER},

    {"illegal", do_illegal, POS_DEAD, L2, LOG_NORMAL, 1, CMD_INTER},
    {"wiznet", do_wiznet, POS_DEAD, L3, LOG_NORMAL, 1, CMD_COMM},
    {"immtalk", do_immtalk, POS_DEAD, L3, LOG_NORMAL, 1, CMD_COMM},
    {"plaidstring", do_plaidstring, POS_DEAD, ML, LOG_ALWAYS, 1, CMD_INTER},
    {"imptalk", do_imptalk, POS_DEAD, ML, LOG_NORMAL, 1, CMD_COMM},
    {"imotd", do_imotd, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INFO},
    {":", do_immtalk, POS_DEAD, L3, LOG_NORMAL, 0, CMD_COMM},
    {"smote", do_smote, POS_DEAD, 1, LOG_NORMAL, 1, CMD_COMM},
    {"prefi", do_prefi, POS_DEAD, L3, LOG_NORMAL, 0, CMD_INTER},
    {"prefix", do_prefix, POS_DEAD, L3, LOG_NORMAL, 1, CMD_INTER},
    {"mpdump", do_mpdump, POS_DEAD, L2, LOG_NEVER, 1, CMD_OLC},
    {"mpstat", do_mpstat, POS_DEAD, L2, LOG_NEVER, 1, CMD_OLC},
    {"immnote", do_imm, POS_SLEEPING, 0, LOG_NORMAL, 1, CMD_COMM},

    /*
     * OLC
     */
    {"edit", do_olc, POS_DEAD, IM, LOG_NORMAL, 1, CMD_OLC},
    {"asave", do_asave, POS_DEAD, IM, LOG_NORMAL, 1, CMD_OLC},
    {"alist", do_alist, POS_DEAD, IM, LOG_NORMAL, 1, CMD_OLC},
    {"resets", do_resets, POS_DEAD, IM, LOG_ALWAYS, 1, CMD_OLC},
    {"redit", do_redit, POS_DEAD, IM, LOG_NORMAL, 1, CMD_OLC},
    {"medit", do_medit, POS_DEAD, IM, LOG_NORMAL, 1, CMD_OLC},
    {"aedit", do_aedit, POS_DEAD, IM, LOG_NORMAL, 1, CMD_OLC},
    {"areset", do_areset, POS_DEAD, IM, LOG_NORMAL, 1, CMD_OLC},
    {"oedit", do_oedit, POS_DEAD, IM, LOG_NORMAL, 1, CMD_OLC},
    {"mpedit", do_mpedit, POS_DEAD, IM, LOG_NORMAL, 1, CMD_OLC},
    { "opedit", do_opedit, POS_DEAD, IM,  LOG_ALWAYS, 1, CMD_OLC },
    { "rpedit", do_rpedit, POS_DEAD, IM,  LOG_ALWAYS, 1, CMD_OLC },
    { "opdump", do_opdump, POS_DEAD, IM,  LOG_NEVER,  1, CMD_OLC },
    { "opstat", do_opstat, POS_DEAD, IM,  LOG_NEVER,  1, CMD_OLC },
    { "rpdump", do_rpdump, POS_DEAD, IM,  LOG_NEVER,  1, CMD_OLC },
    { "rpstat", do_rpstat, POS_DEAD, IM,  LOG_NEVER,  1, CMD_OLC },
    {"hedit", do_hedit, POS_DEAD, ML, LOG_NORMAL, 1, CMD_OLC},
    {"gedit", do_gedit, POS_DEAD, ML, LOG_NORMAL, 1, CMD_OLC},
    {"qedit", do_qedit, POS_DEAD, ML, LOG_NORMAL, 1, CMD_OLC},
    {"freevnums",do_freevnums, POS_DEAD, L3, LOG_NORMAL, 1, CMD_OLC},
    {"plist", do_prog_list, POS_DEAD, IM, LOG_NORMAL, 1, CMD_OLC},

   // { "load_store", do_load_store, POS_DEAD, ML, LOG_NORMAL, 1, CMD_OLC},
    { "save_store", do_save_store, POS_DEAD, ML, LOG_NORMAL, 1, CMD_INTER},
    { "sedit", do_store_edit, POS_DEAD, 0, LOG_NORMAL, 1, CMD_SKILL},

    { "ridedit", do_riddle_edit, POS_DEAD, ML, LOG_NORMAL, 1, CMD_OLC},
/*
 *Martial Art Stuff
*/
    {"strike",do_strike,POS_FIGHTING,0,LOG_NORMAL,1, CMD_SKILL},
    {"ironskin",do_ironskin,POS_STANDING,0,LOG_NORMAL,1, CMD_SKILL},
    {"fkick",do_flying_kick,POS_FIGHTING,0,LOG_NORMAL,1, CMD_SKILL},
    {"skick",do_shadow_kick,POS_FIGHTING,0,LOG_NORMAL,1, CMD_SKILL},
    {"meditation",do_meditation,POS_DEAD,0,LOG_NORMAL,1, CMD_SKILL},
    {"awareness",do_awareness,POS_RESTING,0,LOG_NORMAL,1, CMD_SKILL},

     /*
      * Stuff I don't want shortened versions catching
      */
    {"pkon", do_pkon, POS_DEAD, 0, LOG_NORMAL, 1, CMD_INTER},

	/*
	* RP commands
	*/

    {"grantrp", do_grantrp, POS_DEAD, L1, LOG_IMMORTAL, 1, CMD_INTER},
    {"rpadvance", do_rpadvance, POS_DEAD, ML, LOG_IMMORTAL, 1, CMD_INTER},
    {"roleprize", do_roleprize,POS_RESTING,0,LOG_NORMAL,1, CMD_INFO},
	/*
     * End of list.
     */
    {"", 0, POS_DEAD, 0, LOG_NORMAL, 0, 0}
};




/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret (CHAR_DATA * ch, char *argument)
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    int trust;
    bool found;

    int string_count = nAllocString ;
    int perm_count = nAllocPerm ;
    char cmd_copy[MAX_INPUT_LENGTH] ;
    char buf[MAX_STRING_LENGTH] ;

    strcpy(cmd_copy, argument) ;

    /*
     * Strip leading spaces.
     */
    while (isspace (*argument))
        argument++;
    if (argument[0] == '\0')
        return;

    while (argument[strlen(argument) - 1] == '`')
      argument[strlen(argument) - 1] = '\0';


    if (IS_SET(ch->comm, COMM_AFK) && str_cmp(ch->name, "Plaid") && !IS_IMMORTAL(ch))
    {
       REMOVE_BIT(ch->comm, COMM_AFK);
       send_to_char("AFK mode removed.\n\r\n\r", ch);
    }

    /*
     * No hiding.
     *
    REMOVE_BIT (ch->affected_by, AFF_HIDE);
*/
    /*
     * Implement freeze command.
     */
    if (!IS_NPC (ch) && IS_SET (ch->act, PLR_FREEZE))
    {
        send_to_char ("You're totally frozen!\n\r", ch);
        return;
    }

    if (!IS_NPC (ch) && IS_AFFECTED(ch, AFF_RESTRAIN))
    {
       char *restrain;
       char res_arg[MIL];

       restrain = str_dup(argument);
       restrain = one_argument(restrain, res_arg);
       if (str_prefix(res_arg, "inventory")
	   && str_prefix(res_arg, "quaff")
	   && str_prefix(res_arg, "eat")
	   && str_prefix(res_arg, "gdt")
	   && str_prefix(res_arg, "get")
           && str_prefix(res_arg, "look"))
      {
        send_to_char ("You can't do that you are restrained!\n\r", ch);
        return;
      }
      free_string(restrain);
    }
    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy (logline, argument);

    /*Lets see who is doing what? -Ferric*/
    sprintf(last_command,"[%ld] %s in room[%5ld] %s : %s\n",
        IS_NPC(ch) ? ch->pIndexData->vnum : 0,
        IS_NPC(ch) ? ch->short_descr : IS_DISGUISED(ch)?ch->pcdata->disguise.orig_name:ch->name,
        ch->in_room ? ch->in_room->vnum : 0,
        ch->in_room ? ch->in_room->name : "(not in a room)",
        logline);

    if (!isalpha (argument[0]) && !isdigit (argument[0]))
    {
        command[0] = argument[0];
        command[1] = '\0';
        argument++;
        while (isspace (*argument))
            argument++;
    }
    else
    {
        argument = one_argument (argument, command);
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust (ch);
    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
    {
        if (command[0] == cmd_table[cmd].name[0]
            && !str_prefix (command, cmd_table[cmd].name)
            && ((cmd_table[cmd].level <= trust) ||
            (!IS_SWITCHED(ch) && !IS_NPC(ch) && strstr(ch->pcdata->granted, cmd_table[cmd].name)) ))
        {
            found = TRUE;
            break;
        }
    }

    /*
     * Log and snoop.
     */
    if (cmd_table[cmd].log == LOG_NEVER)
        strcpy (logline, "");

    if ((!IS_NPC (ch) && IS_SET (ch->act, PLR_LOG))
        || fLogAll || cmd_table[cmd].log == LOG_ALWAYS
	|| cmd_table[cmd].log == LOG_IMMORTAL)
    {
        sprintf (log_buf, "Log %s: %s", ch->name, logline);
        wiznet (log_buf, ch, NULL, WIZ_SECURE, 0, get_trust (ch));
	if (cmd_table[cmd].log == LOG_IMMORTAL)
          log_special(log_buf, IMM_TYPE);
        else
          log_string (log_buf);
    }

    if (ch->desc != NULL && ch->desc->snoop_by != NULL)
    {
        write_to_buffer (ch->desc->snoop_by, "% ", 2);
        write_to_buffer (ch->desc->snoop_by, logline, 0);
        write_to_buffer (ch->desc->snoop_by, "\n\r", 2);
    }

    if (!found)
    {
        /*
         * Look for command in socials table.
         */
        if (!check_social (ch, command, argument,0,""))
            send_to_char ("Huh?\n\r", ch);
        return;
    }

    /*
     * Character not in position for command?
     */
    if (ch->position < cmd_table[cmd].position)
    {
        switch (ch->position)
        {
            case POS_DEAD:
                send_to_char ("Lie still; you are DEAD.\n\r", ch);
                break;

            case POS_MORTAL:
            case POS_INCAP:
                send_to_char ("You are hurt far too bad for that.\n\r", ch);
                break;

            case POS_STUNNED:
                send_to_char ("You are too stunned to do that.\n\r", ch);
                break;

            case POS_SLEEPING:
                send_to_char ("In your dreams, or what?\n\r", ch);
                break;

            case POS_RESTING:
                send_to_char ("Nah... You feel too relaxed...\n\r", ch);
                break;

            case POS_SITTING:
                send_to_char ("Better stand up first.\n\r", ch);
                break;

            case POS_FIGHTING:
                send_to_char ("No way!  You are still fighting!\n\r", ch);
                break;
            case POS_MEDITATING:
                send_to_char ("Your too relaxed for that.\n\r", ch);
                break;

        }
        return;
    }

    /*
     * Dispatch the command.
     */
    (*cmd_table[cmd].do_fun) (ch, argument);
    sprintf(last_command,"(Finished)[%ld] %s in room[%5ld] %s : %s\n",
        IS_NPC(ch) ? ch->pIndexData->vnum : 0,
        IS_NPC(ch) ? ch->short_descr : IS_DISGUISED(ch)?ch->pcdata->disguise.orig_name:ch->name,
        ch->in_room ? ch->in_room->vnum : 0,
        ch->in_room ? ch->in_room->name : "(not in a room)",
        logline);
    if (string_count < nAllocString)
    {
	sprintf(buf,
	"Memcheck : Increase in strings :: %s : %s", ch->name, cmd_copy) ;
	wiznet(buf, NULL, NULL, WIZ_MEMCHECK,0,0) ;
    }

    if (perm_count < nAllocPerm)
    {
	sprintf(buf,
	"Increase in perms :: %s : %s", ch->name, cmd_copy) ;
	wiznet(buf, NULL, NULL, WIZ_MEMCHECK, 0,0) ;
    }


    tail_chain ();
    return;
}

/* function to keep argument safe in all commands -- no static strings */
void do_function (CHAR_DATA * ch, DO_FUN * do_fun, char *argument)
{
    char *command_string;

    /* copy the string */
    command_string = str_dup (argument);

    /* dispatch the command */
    (*do_fun) (ch, command_string);

    /* free the string */
    free_string (command_string);
}

bool check_social (CHAR_DATA * ch, char *command, char *argument, long comm, char *prefix)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MIL];
    CHAR_DATA *victim = NULL;
    int cmd = 0;
    bool found;


#define useact(tbuf,to) sprintf(buf, "%s%s`*", prefix, tbuf);\
if (comm==0)act(buf, ch, NULL, victim, to); else \
chan_glob_act(buf, ch, NULL, victim, to, comm);

    found = FALSE;
    for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++)
    {
        if (command[0] == social_table[cmd].name[0]
            && !str_prefix (command, social_table[cmd].name))
        {
            found = TRUE;
            break;
        }
    }

    if (!found)
    {
       char temp[MSL];
       if ( comm != COMM_NOWIZ && comm != COMM_NOIMP && comm != COMM_NOOOC)
         return FALSE;

       sprintf(temp, "$n %s %s", command, argument);
       useact (temp, TO_ROOM);
       useact (temp, TO_CHAR);
       return FALSE;

    }
    if (!IS_NPC (ch) && IS_SET (ch->comm, COMM_NOEMOTE))
    {
        send_to_char ("You are anti-social!\n\r", ch);
        return TRUE;
    }

    if (comm == 0) {
      switch (ch->position)
      {
        case POS_DEAD:
            send_to_char ("Lie still; you are DEAD.\n\r", ch);
            return TRUE;

        case POS_INCAP:
        case POS_MORTAL:
            send_to_char ("You are hurt far too bad for that.\n\r", ch);
            return TRUE;

        case POS_STUNNED:
            send_to_char ("You are too stunned to do that.\n\r", ch);
            return TRUE;
         case POS_MEDITATING:
            send_to_char ("Your too relaxed for that.\n\r", ch);
            return TRUE;
        case POS_SLEEPING:
            /*
             * I just know this is the path to a 12" 'if' statement.  :(
             * But two players asked for it already!  -- Furey
             */
            if (!str_cmp (social_table[cmd].name, "snore"))
                break;
            send_to_char ("In your dreams, or what?\n\r", ch);
            return TRUE;
      }

    }
    one_argument (argument, arg);
    victim = NULL;
    if (arg[0] == '\0')
    {
        /* This is what I think you should do to all lines */
        if (!IS_NULLSTR(social_table[cmd].others_no_arg))
        {
          useact (social_table[cmd].others_no_arg, TO_ROOM);
        }

        if (!IS_NULLSTR(social_table[cmd].char_no_arg))
        {
          useact (social_table[cmd].char_no_arg, TO_CHAR);
        }
        return TRUE;
    }
    else if (comm == 0 && (victim = get_char_room (ch, NULL, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return TRUE;
    }
    else if (comm != 0 && (victim = get_char_world(ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return FALSE;
    }
    else if (victim == ch)
    {
	if (!IS_NULLSTR(social_table[cmd].others_auto))
        {
          useact (social_table[cmd].others_auto, TO_ROOM);
        }
	if (!IS_NULLSTR(social_table[cmd].char_auto))
        {
          useact (social_table[cmd].char_auto, TO_CHAR);
        }
    }
    else
    {
	if (!IS_NULLSTR(social_table[cmd].others_found))
        {
          useact (social_table[cmd].others_found, TO_NOTVICT);
        }
	if (!IS_NULLSTR(social_table[cmd].char_found))
        {
          useact (social_table[cmd].char_found, TO_CHAR);
        }
	if (!IS_NULLSTR(social_table[cmd].vict_found))
        {
          useact (social_table[cmd].vict_found, TO_VICT);
        }

        if (!IS_NPC (ch) && IS_NPC (victim)
            && !IS_AFFECTED (victim, AFF_CHARM) &&
               !IS_SET(victim->act, ACT_MOUNT)
            && IS_AWAKE (victim) && victim->desc == NULL && comm==0)
        {
            switch (number_bits (4))
            {
                case 0:

                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
		    if (!IS_NULLSTR(social_table[cmd].others_found))
                    {
		         act (social_table[cmd].others_found,
                         victim, NULL, ch, TO_NOTVICT);
		    }
		    if (!IS_NULLSTR(social_table[cmd].char_found))
		    {
                         act (social_table[cmd].char_found, victim, NULL, ch,
                         TO_CHAR);
		    }
		    if (!IS_NULLSTR(social_table[cmd].vict_found))
                    {
                         act (social_table[cmd].vict_found, victim, NULL, ch,
                         TO_VICT);
		    }
                    break;

                case 9:
                case 10:
                case 11:
                case 12:
                    act ("$n slaps $N.", victim, NULL, ch, TO_NOTVICT);
                    act ("You slap $N.", victim, NULL, ch, TO_CHAR);
                    act ("$n slaps you.", victim, NULL, ch, TO_VICT);
                    break;
            }
        }
    }

    return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number (char *arg)
{

    if (*arg == '\0')
        return FALSE;

    if (*arg == '+' || *arg == '-')
        arg++;

    for (; *arg != '\0'; arg++)
    {
        if (!isdigit (*arg))
            return FALSE;
    }

    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument (char *argument, char *arg)
{
    char *pdot;
    int number;

    for (pdot = argument; *pdot != '\0'; pdot++)
    {
        if (*pdot == '.')
        {
            *pdot = '\0';
            number = atoi (argument);
            *pdot = '.';
            strcpy (arg, pdot + 1);
            return number;
        }
    }

    strcpy (arg, argument);
    return 1;
}

/*
 * Given a string like 14*foo, return 14 and 'foo'
*/
int mult_argument (char *argument, char *arg)
{
    char *pdot;
    int number;

    for (pdot = argument; *pdot != '\0'; pdot++)
    {
        if (*pdot == '*')
        {
            *pdot = '\0';
            number = atoi (argument);
            *pdot = '*';
            strcpy (arg, pdot + 1);
            return number;
        }
    }

    strcpy (arg, argument);
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument (char *argument, char *arg_first)
{
    char cEnd;

    while (isspace (*argument))
        argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
        cEnd = *argument++;

    while (*argument != '\0')
    {
        if (*argument == cEnd)
        {
            argument++;
            break;
        }
        *arg_first = LOWER (*argument);
        arg_first++;
        argument++;
    }
    *arg_first = '\0';

    while (isspace (*argument))
        argument++;

    return argument;
}

char *one_argument_special (char *argument, char *arg_first)
{
    char cEnd;

    while (isspace (*argument))
        argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
        cEnd = *argument++;

    while (*argument != '\0')
    {
        if (*argument == cEnd)
        {
            argument++;
            break;
        }
        *arg_first = LOWER (*argument);
        arg_first++;
        argument++;
    }
    *arg_first = '\0';

    return argument;
}

/*
 * Contributed by Alander.
 */
void do_commands (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    bool restriction = FALSE;
    bool grep = FALSE;
    char grepfor[MIL];
    char arg[MIL];
    int type = 0;
    int cmd;
    int col;

    argument = one_argument(argument, arg);
    if (arg[0] == '\0')
    {
      send_to_char("Pick a command type:\n\r", ch);
      send_to_char("  all     - list all commands\n\r", ch);
      send_to_char("  comm    - list communication commands\n\r", ch);
      send_to_char("  info    - list information commands\n\r", ch);
      send_to_char("  inter   - list interactional commands (could be mud, character or object interaction)\n\r", ch);
      if (IS_IMMORTAL(ch))
      {
      	send_to_char("  olc     - list OLC commands\n\r", ch);
      }
      send_to_char("  skill   - list skill related commands\n\r", ch);
      send_to_char("\n\rNOTE: You can also post fix all these with a | and a search string to grep for a skill\n\r", ch);
      send_to_char("Example: command all | s (This would search through all commands and list those starting with s\n\r", ch);
      return;
    }

    if (!str_cmp(arg, "all"))
    {
      // Do nothing
    }
    else if (!str_cmp(arg, "comm"))
    {
      restriction = TRUE;
      type = CMD_COMM;
    }
    else if (!str_cmp(arg, "info"))
    {
      restriction = TRUE;
      type = CMD_INFO;
    }
    else if (!str_cmp(arg, "inter"))
    {
      restriction = TRUE;
      type = CMD_INTER;
    }
    else if (!str_cmp(arg, "olc"))
    {
      restriction = TRUE;
      type = CMD_OLC;
    }
    else if (!str_cmp(arg, "skill"))
    {
      restriction = TRUE;
      type = CMD_SKILL;
    }
    else
    {
      send_to_char("Pick a command type:\n\r", ch);
      send_to_char("  all     - list all commands\n\r", ch);
      send_to_char("  comm    - list communication commands\n\r", ch);
      send_to_char("  info    - list information commands\n\r", ch);
      send_to_char("  inter   - list interactional commands (could be mud, character or object interaction)\n\r", ch);
      if (IS_IMMORTAL(ch))
      {
      	send_to_char("  olc     - list OLC commands\n\r", ch);
      }
      send_to_char("  skill   - list skill related commands\n\r", ch);
      send_to_char("\n\rNOTE: You can also post fix all these with a | and a search string to grep for a skill\n\r", ch);
      send_to_char("Example: command all | s (This would search through all commands and list those starting with s\n\r", ch);
      return;
    }

    argument = one_argument(argument, grepfor);
    if (!str_cmp(grepfor, "|"))
    {
      grep = TRUE;
    }

    col = 0;
    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
    {
        if (cmd_table[cmd].level <= get_trust (ch)
        && cmd_table[cmd].show
        && (!restriction || cmd_table[cmd].type == type)
        && (!grep || !str_prefix(argument, cmd_table[cmd].name)))
        {
            sprintf (buf, "%-15s", cmd_table[cmd].name);
            send_to_char (buf, ch);
            if (++col % 5 == 0)
                send_to_char ("\n\r", ch);
        }
    }

    if (col % 5 != 0)
        send_to_char ("\n\r", ch);
    return;
}

/*
void do_wizhelp (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MIL];
    int cmd;
    int col;
    int i = 0;
    bool grep = FALSE;

    col = 0;

    if (argument[0] == '|')
    {
      argument = one_argument(argument, arg);
      grep = TRUE;
    }

    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
    {
        if (cmd_table[cmd].level >= LEVEL_HERO
            && cmd_table[cmd].level <= get_trust (ch) && cmd_table[cmd].show &&
            (!grep || !str_prefix(argument, cmd_table[cmd].name)))
        {
            i++;
            sprintf (buf, "%-15s [%2d]  ", cmd_table[cmd].name, cmd_table[cmd].level);
            send_to_char (buf, ch);
            if (++col % 3 == 0)
                send_to_char ("\n\r", ch);
        }
    }


    if (col % 3 != 0)
        send_to_char ("\n\r", ch);
    sprintf(buf, "%d immortal commands found.\n\r", i);
    send_to_char(buf, ch);
    return;
}
*/

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    BUFFER *final;
    char buf[MAX_STRING_LENGTH];
    int cmd, col = 0, lvl = get_trust( ch ), level;

    final = new_buf();

    for( level = MAX_LEVEL; level >= LEVEL_HERO; level-- )
    {
        for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
        {
            if ( cmd_table[cmd].level == level
            &&   cmd_table[cmd].show
            &&   ( cmd_table[cmd].level <= lvl ) )
            {
                if ( col == 0 )
                {
                    sprintf( buf, "\n\r`8Level `5%3d`&:`7 ", level );
                    add_buf( final, buf );
                }

                sprintf( buf, "%-15s", cmd_table[cmd].name );
                add_buf( final, buf );

                if ( ++col % 4 == 0 )
                    add_buf( final, "\n\r           " );
            }
        }

        if ( col % 4 != 0 )
            add_buf( final, "\n\r" );
        col = 0;
    }

    add_buf( final, "" );

    page_to_char( final->string, ch );
    free_buf( final );

    return;
}
