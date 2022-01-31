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
*	ROM 2.4 is copyright 1993-1998 Russ Taylor                             *
*	ROM has been brought to you by the ROM consortium                      *
*	    Russ Taylor (rtaylor@hypercube.org)                                *
*	    Gabrielle Taylor (gtaylor@hypercube.org)                           *
*	    Brian Moore (zump@rom.org)                                         *
*	By using this code, you have agreed to follow the terms of the         *
*	ROM license, in the file Rom24/doc/rom.license                         *
****************************************************************************/

/* this is a listing of all the commands and command related data */

/* wrapper function for safe command execution */
void do_function args((CHAR_DATA *ch, DO_FUN *do_fun, char *argument));

/* for command types */
#define ML 	MAX_LEVEL	/* implementor */
#define L1	MAX_LEVEL - 1  	/* creator */
#define L2	MAX_LEVEL - 2	/* supreme being */
#define L3	MAX_LEVEL - 3	/* head builder */
#define IM 	MAX_LEVEL - 3	/* builder */
#define HE	LEVEL_HERO	/* hero */

#define COM_INGORE	1

#define CMD_NONE      0
#define CMD_COMM      1
#define CMD_INTER     2
#define CMD_INFO      3
#define CMD_SKILL     4
#define CMD_OLC       5


/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
    sh_int		position;
    sh_int		level;
    sh_int		log;
    sh_int              show;
    sh_int              type;
};

/* the command table itself */
extern	const	struct	cmd_type	cmd_table	[];

/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN( do_rphp         ); // yank this

DECLARE_DO_FUN( do_clearheal   );
DECLARE_DO_FUN( do_rescue_test );
DECLARE_DO_FUN( do_test_act );

DECLARE_DO_FUN( do_mobprog_clear);

DECLARE_DO_FUN(do_note_mysql);

/* mount */
DECLARE_DO_FUN(do_masturbate );
DECLARE_DO_FUN( do_mount        );
DECLARE_DO_FUN( do_dismount     );
DECLARE_DO_FUN( do_tame         );
DECLARE_DO_FUN( do_spur         );
DECLARE_DO_FUN( do_reinin       );

DECLARE_DO_FUN( do_checklog     );

DECLARE_DO_FUN( do_remember     );
DECLARE_DO_FUN( do_forget       );

DECLARE_DO_FUN( do_prog_list    );
DECLARE_DO_FUN( do_print_delay  );
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN( do_add_extra    );
DECLARE_DO_FUN( do_addlag);
DECLARE_DO_FUN( do_add_weapon_flag  );
DECLARE_DO_FUN( do_affects	);
DECLARE_DO_FUN( do_afk		);
DECLARE_DO_FUN( do_alia		);
DECLARE_DO_FUN( do_alias	);
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN( do_answer	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN( do_asmo         ); 
DECLARE_DO_FUN( do_assassinate  );
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN(	do_auction	);
DECLARE_DO_FUN( do_autoall	);
DECLARE_DO_FUN( do_autoassist	);
DECLARE_DO_FUN( do_autoexit	);
DECLARE_DO_FUN( do_autogold	);
DECLARE_DO_FUN( do_autolist	);
DECLARE_DO_FUN( do_autoloot	);
DECLARE_DO_FUN( do_autosac	);
DECLARE_DO_FUN( do_autosplit	);
DECLARE_DO_FUN( do_avedam       );
DECLARE_DO_FUN( do_awareness	);
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN( do_ballad       );
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN(	do_ban		);
DECLARE_DO_FUN( do_bandage      );
DECLARE_DO_FUN( do_bash		);
DECLARE_DO_FUN( do_battlemeter  );
DECLARE_DO_FUN( do_berserk	);
DECLARE_DO_FUN( do_bitch        );
DECLARE_DO_FUN( do_bofh         );
DECLARE_DO_FUN( do_joinwar      );
DECLARE_DO_FUN( do_leavewar     );
DECLARE_DO_FUN( do_leaveguild   );
DECLARE_DO_FUN( do_bestow       );
DECLARE_DO_FUN( do_blackjack    );
DECLARE_DO_FUN( do_bomb         );
DECLARE_DO_FUN( do_bond         );
DECLARE_DO_FUN( do_bonus_time   );
DECLARE_DO_FUN(	do_brandish	);
DECLARE_DO_FUN( do_brief	);
DECLARE_DO_FUN( do_build        );
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN( do_bugfix       );
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN( do_changes	);
DECLARE_DO_FUN( do_channels	);
DECLARE_DO_FUN( do_cleanse      );
DECLARE_DO_FUN( do_cleartick    );
DECLARE_DO_FUN( do_clone	);
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN(	do_colour	);
DECLARE_DO_FUN(	do_commands	);
DECLARE_DO_FUN( do_combine	);
DECLARE_DO_FUN( do_compact	);
DECLARE_DO_FUN(	do_compare	);
DECLARE_DO_FUN( do_compress     );
DECLARE_DO_FUN( do_confiscate	);
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN(	do_couple	);
DECLARE_DO_FUN( do_cover        );
DECLARE_DO_FUN( do_cr           );
DECLARE_DO_FUN( do_crowns       );
DECLARE_DO_FUN( do_daze         );
DECLARE_DO_FUN( do_dampen       );
DECLARE_DO_FUN( do_developer    ); 
DECLARE_DO_FUN( do_discharge    ); 
DECLARE_DO_FUN( do_disguise     );
DECLARE_DO_FUN( do_disguiseself ); 
DECLARE_DO_FUN( do_elements     );
DECLARE_DO_FUN( do_enhanced     );
DECLARE_DO_FUN( do_engage       );
DECLARE_DO_FUN( do_enslave      );
DECLARE_DO_FUN( do_fillmap      );
DECLARE_DO_FUN( do_penchant     );
DECLARE_DO_FUN( do_pledge       );
DECLARE_DO_FUN( do_print_mobiles);
DECLARE_DO_FUN( do_print_objects);
DECLARE_DO_FUN( do_power	);
DECLARE_DO_FUN( do_puke         );
DECLARE_DO_FUN( do_makemap );
DECLARE_DO_FUN( do_maxstats     );
DECLARE_DO_FUN( do_mintalk      );
DECLARE_DO_FUN( do_minus_time   );
DECLARE_DO_FUN( do_msp          );
DECLARE_DO_FUN( do_mudstats     );
DECLARE_DO_FUN( do_mxp          );
DECLARE_DO_FUN( do_pload        );
DECLARE_DO_FUN( do_savegroup    );
DECLARE_DO_FUN( do_sense        );
DECLARE_DO_FUN( do_skillstat    );
DECLARE_DO_FUN( do_slookup      );
DECLARE_DO_FUN( do_slot         );
DECLARE_DO_FUN( do_study        );
DECLARE_DO_FUN( do_snote        );
DECLARE_DO_FUN( do_punload      );
DECLARE_DO_FUN( do_omni         );
DECLARE_DO_FUN( do_otransfer    );
DECLARE_DO_FUN(	do_copyover	);
DECLARE_DO_FUN( do_donate       );
DECLARE_DO_FUN( do_beseech      );
DECLARE_DO_FUN( do_skillstat    );
DECLARE_DO_FUN( do_sn           );
DECLARE_DO_FUN( do_think        );
// DECLARE_DO_FUN( do_push_drag    );
DECLARE_DO_FUN( do_push         );
DECLARE_DO_FUN( do_drag         );
DECLARE_DO_FUN( do_spellstat    );
DECLARE_DO_FUN( do_warchar	        );
DECLARE_DO_FUN( do_startwar	);
DECLARE_DO_FUN( do_pkon         );
DECLARE_DO_FUN( do_norp         );
DECLARE_DO_FUN( do_nowar        );
DECLARE_DO_FUN( do_novice       );
DECLARE_DO_FUN( do_ntoggle      );
DECLARE_DO_FUN( do_spellup      );
DECLARE_DO_FUN( do_personal_mult); 
DECLARE_DO_FUN( do_pipe         );
DECLARE_DO_FUN( do_mquest       );
DECLARE_DO_FUN( do_quest        );
DECLARE_DO_FUN( do_questedit    );
DECLARE_DO_FUN( do_questinfo    );
DECLARE_DO_FUN( do_quest_restart);
DECLARE_DO_FUN( do_kinkboot     );
DECLARE_DO_FUN( do_count	);
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN( do_deaf		);
DECLARE_DO_FUN( do_delet	);
DECLARE_DO_FUN( do_delete	);
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN(	do_description	);
DECLARE_DO_FUN( do_dirt		);
DECLARE_DO_FUN(	do_disarm	);
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN( do_dragontalk   );
DECLARE_DO_FUN( do_ddraw         );
DECLARE_DO_FUN( do_dream        );
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN( do_dump		);
DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN( do_enchant      );
DECLARE_DO_FUN( do_engage       );
DECLARE_DO_FUN( do_enter	);
DECLARE_DO_FUN( do_envenom	);
DECLARE_DO_FUN( do_contactpoison);
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN( do_eqlookup     );
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN( do_ensnare      );
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN( do_firestorm    );
DECLARE_DO_FUN( do_fix          );
DECLARE_DO_FUN( do_flag		);
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN(	do_flip		);
DECLARE_DO_FUN( do_flying_kick  );
DECLARE_DO_FUN( do_focus        );
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN( do_font         );
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN(	do_forcetick    );
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN( do_gain		);
DECLARE_DO_FUN( do_gameinfo     );
DECLARE_DO_FUN( do_generate_names);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN( do_gossip	);
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN( do_grant        );
DECLARE_DO_FUN( do_grantrp      );
DECLARE_DO_FUN( do_grats	);
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN( do_groups	);
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN( do_guild    	);
DECLARE_DO_FUN( do_guide    	);
DECLARE_DO_FUN( do_manadrain    );
DECLARE_DO_FUN( do_meditation   );
DECLARE_DO_FUN( do_guildlist    );
DECLARE_DO_FUN( do_heal		);
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN(	do_mhelp		);
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN( do_howl         );
DECLARE_DO_FUN( do_hungerflag   );
DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN( do_idle         );
DECLARE_DO_FUN( do_ignite       );
DECLARE_DO_FUN( do_ignore       );
DECLARE_DO_FUN( do_illegal      );
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN( do_imptalk      );
DECLARE_DO_FUN( do_incognito	);
DECLARE_DO_FUN( do_diemcloak    );
DECLARE_DO_FUN( do_ironskin   	);
DECLARE_DO_FUN( do_ispell       );
DECLARE_DO_FUN( do_ispell_done  );
DECLARE_DO_FUN( do_ispell_start );
DECLARE_DO_FUN( do_clantalk	);
DECLARE_DO_FUN( do_imm          );
DECLARE_DO_FUN( do_imotd	);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN( do_push         );
DECLARE_DO_FUN( do_damane       );
DECLARE_DO_FUN( do_drag         );
DECLARE_DO_FUN( do_lab_monkey	);
DECLARE_DO_FUN( do_last		);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN( do_load		);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN( do_lore         );
DECLARE_DO_FUN( do_istat        );
DECLARE_DO_FUN( do_lstat        );
DECLARE_DO_FUN( do_makegl       );
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN( do_medallion    );
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN(	do_mload	);
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN( do_mob		);
DECLARE_DO_FUN( do_motd		);
DECLARE_DO_FUN( do_mpstat	);
DECLARE_DO_FUN( do_mpdump	);
DECLARE_DO_FUN(	do_murde	);
DECLARE_DO_FUN(	do_murder	);
DECLARE_DO_FUN( do_music	);
DECLARE_DO_FUN( do_newlock	);
DECLARE_DO_FUN( do_new_help      );
DECLARE_DO_FUN( do_news		);
DECLARE_DO_FUN( do_nochannels	);
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN( do_noexp        );
DECLARE_DO_FUN( do_nofollow	);
DECLARE_DO_FUN( do_noloot	);
DECLARE_DO_FUN( do_nodamage     );
DECLARE_DO_FUN( do_notitles     );
DECLARE_DO_FUN( do_noweave      );
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN(	do_noshout	);
DECLARE_DO_FUN( do_nosummon	);
DECLARE_DO_FUN( do_note         );
DECLARE_DO_FUN(	do_notell	);
DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN( do_ooc          );
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN(	do_openbag	);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN( do_osay         );
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN( do_outfit	);
DECLARE_DO_FUN( do_owhere	);
DECLARE_DO_FUN(	do_pardon	);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN(	do_peace	);
DECLARE_DO_FUN( do_plaidstring  );
DECLARE_DO_FUN( do_check        );
DECLARE_DO_FUN( do_pecho	);
DECLARE_DO_FUN( do_peek         );
DECLARE_DO_FUN( do_peek_in      );
DECLARE_DO_FUN( do_guildmaster	);
DECLARE_DO_FUN( do_permban	);
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN( do_play		);
DECLARE_DO_FUN( do_pmote	);
DECLARE_DO_FUN(	do_pose		);
DECLARE_DO_FUN( do_pour		);
DECLARE_DO_FUN(	do_practice	);
DECLARE_DO_FUN( do_pray         );
DECLARE_DO_FUN( do_prefi	);
DECLARE_DO_FUN( do_prefix	);
DECLARE_DO_FUN( do_promote      );
DECLARE_DO_FUN( do_prompt	);
DECLARE_DO_FUN( do_protect	);
DECLARE_DO_FUN(	do_purge	);
DECLARE_DO_FUN(	do_put		);
DECLARE_DO_FUN( do_qtalk        );
DECLARE_DO_FUN(	do_quaff	);
DECLARE_DO_FUN( do_questchar    );
DECLARE_DO_FUN( do_tan          ); 
DECLARE_DO_FUN( do_top_ten      );
DECLARE_DO_FUN( do_tourneychar  );
DECLARE_DO_FUN( do_tournament   ); 
DECLARE_DO_FUN( do_tagchar      );
DECLARE_DO_FUN( do_ttalk        );
DECLARE_DO_FUN( do_throw        );
DECLARE_DO_FUN( do_track        );
DECLARE_DO_FUN( do_trivia       );
DECLARE_DO_FUN( do_radio        );
DECLARE_DO_FUN( do_restring     );
DECLARE_DO_FUN( do_itchar       );
DECLARE_DO_FUN( do_question	);
DECLARE_DO_FUN(	do_qui		);
DECLARE_DO_FUN( do_quiet	);
DECLARE_DO_FUN(	do_quit		);
DECLARE_DO_FUN( do_quote	);
//DECLARE_DO_FUN( do_quotester    );
DECLARE_DO_FUN( do_ravage       );
DECLARE_DO_FUN( do_read		);
DECLARE_DO_FUN(	do_reboo	);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN(	do_recall	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN( do_rename       );
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN( do_relax        );
DECLARE_DO_FUN( do_replay	);
DECLARE_DO_FUN(	do_hreply	);
DECLARE_DO_FUN(	do_ireply	);
DECLARE_DO_FUN(	do_oreply	);
DECLARE_DO_FUN(	do_report	);
DECLARE_DO_FUN(	do_rescue	);
//DECLARE_DO_FUN( do_rental   	);
DECLARE_DO_FUN( do_resetpassword );
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN( do_restrain     );
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN( do_roleprize    );
DECLARE_DO_FUN( do_roster       );
DECLARE_DO_FUN( do_rp           );
DECLARE_DO_FUN( do_rpadvance    );
DECLARE_DO_FUN(	do_rset		);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN( do_rules	);
DECLARE_DO_FUN( do_rwhere       );
DECLARE_DO_FUN(	do_sacrifice	);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN(	do_save_vote	);
DECLARE_DO_FUN(	do_load_guilds	);
DECLARE_DO_FUN(	do_save_guilds	);
DECLARE_DO_FUN( do_load_qprize  );
DECLARE_DO_FUN( do_save_qprize  );
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN(	do_scan		);
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN( do_scry         );
DECLARE_DO_FUN( do_seperate     );
DECLARE_DO_FUN( do_diem         );
DECLARE_DO_FUN( do_scroll	);
DECLARE_DO_FUN( do_second       );
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN( do_set		);
DECLARE_DO_FUN( do_sever        );
DECLARE_DO_FUN( do_shadow_kick  );
DECLARE_DO_FUN( do_shoot        );
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN( do_show		);
DECLARE_DO_FUN( do_shroud       );
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN( do_skillstrip   );
DECLARE_DO_FUN( do_sit		);
DECLARE_DO_FUN( do_simulacrum   );
DECLARE_DO_FUN( do_skills	);
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN( do_smote	);
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN( do_socials	);
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN( do_sockets	);
DECLARE_DO_FUN( do_spells	);
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN( do_stance       );
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN( do_stat		);
DECLARE_DO_FUN(	do_steal	);
DECLARE_DO_FUN(	do_steal_from_bag	);
DECLARE_DO_FUN( do_stealth      );
DECLARE_DO_FUN( do_still        );
DECLARE_DO_FUN( do_story	);
DECLARE_DO_FUN( do_strike       );
DECLARE_DO_FUN( do_string	);
DECLARE_DO_FUN(	do_surrender	);
DECLARE_DO_FUN(	do_switch	);
DECLARE_DO_FUN(	do_otell	);
DECLARE_DO_FUN( do_tell         );
DECLARE_DO_FUN(	do_htell	);
DECLARE_DO_FUN(	do_whisper	);
DECLARE_DO_FUN( do_telnet_ga	);
DECLARE_DO_FUN( do_testasmo     );
DECLARE_DO_FUN(	do_time		);
DECLARE_DO_FUN( do_pretitle     );
DECLARE_DO_FUN(	do_title	);
DECLARE_DO_FUN(	do_train	);
DECLARE_DO_FUN(	do_transfer	);
DECLARE_DO_FUN( do_trip		);
DECLARE_DO_FUN(	do_trust	);
DECLARE_DO_FUN(	do_typo		);
DECLARE_DO_FUN( do_unalias	);
DECLARE_DO_FUN( do_unenchant    );
DECLARE_DO_FUN( do_ungrant	);
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN( do_unread	);
DECLARE_DO_FUN( do_unrestore    );
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN( do_veil         );
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN( do_violate	);
DECLARE_DO_FUN( do_vote         );
DECLARE_DO_FUN( do_vote_create  );
DECLARE_DO_FUN( do_vnum		);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN( do_warcry       );
DECLARE_DO_FUN( do_wardertalk   );
DECLARE_DO_FUN( do_webhelp      );
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN(	do_where	);
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN( do_whois	);
DECLARE_DO_FUN(	do_wimpy	);
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN( do_wizlist	);
DECLARE_DO_FUN( do_wiznet	);
DECLARE_DO_FUN( do_printmud     );
DECLARE_DO_FUN( do_wolf         );
DECLARE_DO_FUN( do_work         );
DECLARE_DO_FUN( do_worth	);
DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN(	do_zap		);
DECLARE_DO_FUN( do_zecho	);

DECLARE_DO_FUN( do_mount	);
DECLARE_DO_FUN( do_dismount	);
DECLARE_DO_FUN( do_olc		);
DECLARE_DO_FUN( do_asave	);
DECLARE_DO_FUN( do_alist	);
DECLARE_DO_FUN( do_resets	);
DECLARE_DO_FUN( do_redit	);
DECLARE_DO_FUN( do_aedit	);
DECLARE_DO_FUN( do_areset       );
DECLARE_DO_FUN( do_medit	);
DECLARE_DO_FUN( do_oedit	);
DECLARE_DO_FUN( do_mpedit	);
DECLARE_DO_FUN( do_hedit	);
DECLARE_DO_FUN( do_gedit	);
DECLARE_DO_FUN( do_qedit	);
DECLARE_DO_FUN( do_opedit );
DECLARE_DO_FUN( do_rpedit );
DECLARE_DO_FUN( do_opdump );
DECLARE_DO_FUN( do_opstat );
DECLARE_DO_FUN( do_rpdump );
DECLARE_DO_FUN( do_rpstat );


/* Bank Code - Sandy */
DECLARE_DO_FUN( do_deposit	);
DECLARE_DO_FUN( do_withdraw	);
DECLARE_DO_FUN(	do_balance	);

/* Wilderness Code - Sandy */
DECLARE_DO_FUN( do_build 	);
DECLARE_DO_FUN( do_chop		);
DECLARE_DO_FUN( do_dig		);
DECLARE_DO_FUN( do_evolve	);
DECLARE_DO_FUN( do_fish		);
DECLARE_DO_FUN( do_hoe		);
DECLARE_DO_FUN( do_shome	);
DECLARE_DO_FUN( do_landscape	);
DECLARE_DO_FUN( do_lay		);
DECLARE_DO_FUN( do_mine		);
DECLARE_DO_FUN( do_prospect	);
DECLARE_DO_FUN( do_saw		);
DECLARE_DO_FUN( do_scythe	);
DECLARE_DO_FUN( do_seed		);
DECLARE_DO_FUN( do_trim		);
DECLARE_DO_FUN( do_wsave	);
DECLARE_DO_FUN( do_freevnums	);
DECLARE_DO_FUN( do_scanning	);

/* STORE */
DECLARE_DO_FUN( do_load_store   );
DECLARE_DO_FUN( do_save_store   );
DECLARE_DO_FUN( do_store_edit   );
 
DECLARE_DO_FUN( do_riddle_edit  );

DECLARE_DO_FUN( do_guide_offense   );
DECLARE_DO_FUN( do_guide_flag      );
DECLARE_DO_FUN( do_guide_jail      );

DECLARE_DO_FUN( do_nstat	);
DECLARE_DO_FUN( do_agenda       );
DECLARE_DO_FUN( do_todo         );
DECLARE_DO_FUN( do_tocode       );
DECLARE_DO_FUN( do_changed      );

DECLARE_DO_FUN( do_arealinks	);
DECLARE_DO_FUN( do_olevel	);
DECLARE_DO_FUN( do_mlevel	);
DECLARE_DO_FUN( do_showclass    );

DECLARE_DO_FUN( do_rub		);
DECLARE_DO_FUN( do_skillreset	);
DECLARE_DO_FUN( do_dance	);
DECLARE_DO_FUN( do_style	);
DECLARE_DO_FUN( do_gash		);
DECLARE_DO_FUN( do_caltrops	);
DECLARE_DO_FUN( do_salve	);
DECLARE_DO_FUN( do_dislodge	);
DECLARE_DO_FUN( do_quickening	);
DECLARE_DO_FUN( do_trap		);
DECLARE_DO_FUN( do_ohelp		);
DECLARE_DO_FUN( do_warstatus	);
DECLARE_DO_FUN( do_gouge	);
DECLARE_DO_FUN( do_unseen_fury  );
DECLARE_DO_FUN( do_darkhound	);
DECLARE_DO_FUN( do_worldmap	);
DECLARE_DO_FUN( do_mapsize	);
DECLARE_DO_FUN( do_map		);
DECLARE_DO_FUN( do_claim	);
DECLARE_DO_FUN( do_assign	);
DECLARE_DO_FUN( do_construct	);
DECLARE_DO_FUN( do_showthemap	);
DECLARE_DO_FUN( do_award	);
DECLARE_DO_FUN( do_set_wilderness_all );
DECLARE_DO_FUN( do_smoke	);
DECLARE_DO_FUN( do_exhale	);


