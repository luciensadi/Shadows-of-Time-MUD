/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,       *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                       *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael       *
 *  Chastain, Michael Quan, and Mitchell Tse.                   *
 *                                       *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                           *
 *                                       *
 *  Much time and thought has gone into this software and you are       *
 *  benefitting.  We hope that you share your changes too.  What goes       *
 *  around, comes around.                           *
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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "magic.h"
#include "tables.h"

void do_heal (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *mob;
    char arg[MAX_INPUT_LENGTH];
    int cost, sn,level;
    int i;
    float rate;
    SPELL_FUN *spell;
    char *words;
    char buf[MSL];

    /* check for healer */
    for (mob = ch->in_room->people; mob; mob = mob->next_in_room)
    {
        if (IS_NPC (mob) && IS_SET (mob->act, ACT_IS_HEALER))
            break;
    }

    if (mob == NULL)
    {
        send_to_char ("You can't do that here.\n\r", ch);
        return;
    }

    one_argument (argument, arg);
    level = mob->level;
    rate = 1.0;

    if (!IS_NPC(ch) && ch->clan == 0)
      rate = 0.8;
    else if (!IS_NPC(ch) && IS_SET(ch->act, PLR_TOURNEY))
      rate = 0.5;
    else
      rate = 1.0; 

    if (arg[0] == '\0')
    {
        /* display price list */
        act ("$N says 'I offer the following spells:'", ch, NULL, mob,
             TO_CHAR);
        for (i = 0; i < MAX_HEAL ; i++)
        {
          sprintf(buf, "    %10s : %-25s %5d silver\n\r", healer_table[i].keyword, 
                        healer_table[i].display, 
			(int) (healer_table[i].cost * rate));
          send_to_char(buf, ch);
        }         
        send_to_char (" Type heal <type> to be healed.\n\r", ch);
        return;
    }

    if (!str_prefix (arg, "light"))
    {
    	level = 75;
        spell = spell_cure_light;
        sn = skill_lookup ("cure light");
        words = "judicandus dies";
        cost = 500;
    }
    if (!str_prefix (arg, "disenchant"))
    {
        spell = spell_disenchant;
        sn = skill_lookup ("disenchant");
        words = "bringiton";
        cost = 4000;
    }

    else if (!str_prefix (arg, "serious"))
    {
    	level = 75;
        spell = spell_cure_serious;
        sn = skill_lookup ("cure serious");
        words = "judicandus gzfuajg";
        cost = 1000;
    }

    else if (!str_prefix (arg, "critical"))
    {
    	level = 75;
        spell = spell_cure_critical;
        sn = skill_lookup ("cure critical");
        words = "judicandus qfuhuqar";
        cost = 2000;
    }

    else if (!str_prefix (arg, "heal"))
    {
    	level = 75;
        spell = spell_heal;
        sn = skill_lookup ("heal");
        words = "pzar";
        cost = 4000;
    }

    else if (!str_prefix (arg, "blindness"))
    {
        spell = spell_cure_blindness;
        sn = skill_lookup ("cure blindness");
        words = "judicandus noselacri";
        cost = 1000;
    }

    else if (!str_prefix (arg, "disease"))
    {
        spell = spell_cure_disease;
        sn = skill_lookup ("cure disease");
        words = "judicandus eugzagz";
        cost = 1500;
    }

    else if (!str_prefix (arg, "poison"))
    {
        spell = spell_cure_poison;
        sn = skill_lookup ("cure poison");
        words = "judicandus sausabru";
        cost = 1500;
    }

    else if (!str_prefix (arg, "uncurse") || !str_prefix (arg, "curse"))
    {
        spell = spell_remove_curse;
        sn = skill_lookup ("remove curse");
        words = "candussido judifgz";
        cost = 4000;
    }

    else if (!str_prefix (arg, "mana") || !str_prefix (arg, "energize"))
    {
        spell = NULL;
        sn = -1;
        words = "energizer";
        cost = 750;
    }


    else if (!str_prefix (arg, "refresh") || !str_prefix (arg, "moves"))
    {
        spell = spell_refresh;
        sn = skill_lookup ("refresh");
        words = "candusima";
        cost = 250;
    }

    else
    {
        act ("$N says 'Type 'heal' for a list of spells.'",
             ch, NULL, mob, TO_CHAR);
        return;
    }

    cost = (int) (cost * rate);

    if (cost > (ch->gold * 100 + ch->silver))
    {
        act ("$N says 'You do not have enough gold for my services.'",
             ch, NULL, mob, TO_CHAR);
        return;
    }

    WAIT_STATE (ch, PULSE_VIOLENCE);

    deduct_cost (ch, cost);
    mob->gold += cost / 100;
    mob->silver += cost % 100;
    act ("$n utters the words '$T'.", mob, NULL, words, TO_ROOM);

    if (spell == NULL)
    {                            /* restore mana trap...kinda hackish */
        ch->mana += dice (5, 12) + mob->level / 2;
        ch->mana = UMIN (ch->mana, ch->max_mana);
        send_to_char ("A warm glow passes through you.\n\r", ch);
        return;
    }

    if (sn == -1)
        return;

    spell (sn, level, mob, ch, TARGET_CHAR);
}
