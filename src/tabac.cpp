/***************************************************************************

                      _____  __      __  __      __
                     /  _  \/  \    /  \/  \    /  \
                    /  /_\  \   \/\/   /\   \/\/   /
                   /    |    \        /  \        /
                   \____|__  /\__/\  /    \__/\  /
                           \/      \/          \/

    As the Wheel Weaves based on ROM 2.4. Original code by Dalsor.
    See changes.log for a list of changes from the original ROM code.
    Credits for code created by other authors have been left
 	intact at the head of each function.

    Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,
    Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.

    Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael
    Chastain, Michael Quan, and Mitchell Tse.

    In order to use any part of this Merc Diku Mud, you must comply with
    both the original Diku license in 'license.doc' as well the Merc
    license in 'license.txt'.  In particular, you may not remove either of
    these copyright notices.

    Much time and thought has gone into this software and you are
    benefitting.  We hope that you share your changes too.  What goes
    around, comes around.

	ROM 2.4 is copyright 1993-1998 Russ Taylor
	ROM has been brought to you by the ROM consortium
	    Russ Taylor (rtaylor@hypercube.org)
	    Gabrielle Taylor (gtaylor@hypercube.org)
	    Brian Moore (zump@rom.org)
	By using this code, you have agreed to follow the terms of the
	ROM license, in the file Rom24/doc/rom.license

 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "merc.h"
#include "tables.h"

/* This is NOT for POT smoking! I do not condone the use of this code for
 * MU* games that have pot smoking.
 *
 * If you use this code for that, I will not help you in any way, shape,
 * or form. I live in the USA, where pot smoking is illegal.
 */

/* When compiling, make sure to change aww.h to the primary header file
 * for your MU* type. for Merc and ROM it's merc.h
 * For others, try mud.h
 *
 * Parents of younger types will likely not condone this snippet, but in
 * the world of WoT, pipe smoking is a pleasure many of the male characters
 * enjoy. Any of you who enjoy a pipe now and again may attest to the
 * pleasing aroma of a cherry or (plug) Captain Black pipe of tabacco. I
 * personally prefer the Marlonique with a slightly curved stem and a bowl
 * of Captain Black Light. The stem cools the smoke before you taste it,
 * allowing you to enjoy the flavor without the harshness.... But I digress.
 *
 * Those two things have influenced me to release this snip.
 * There's a lot that can be done here. I've tried to include ideas for
 * improvement by commenting out psuedo code at the bottom.
 *
 * This code is free for you. If you use it, I would appreciate an email,
 * but it's not required. How many people ever email snippet writers anyway?
 * If you want to say hey thanks, or this sucks, send an email to me at
 * dalsor@cfl.rr.com, or drop by www.aww-mud.org port 6500 to see what
 * we have done with this code since it's public release.
 *
 * Then again, if you think it sucks... why are you using it?
 *
 * I'm not including the changes to olc or whatever you use, because right
 * now, there are way too many codebases out there for me to try and cover
 * in a simple header. If you have problems, the email is above. I cant
 * say yeah I will make it work for you, but I will try to help you out.
 *
 * Basic changes include, make a new item type. Make sure you save the item
 * type to your areas. Create the new item type and make it available to
 * your players. Add the smoking flags to your char data or pc data do they
 * can exhale!
 *
 * The code is commented to help.
 */

/* add do smoke to interp.c and interp.h */
/* Tabac is the WoT equivalent of tabacoo */
void do_smoke (CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *pObj;
    int amount, range;

    if ( argument[0] == '\0' )
    {
		send_to_char( "`3Smoke what?`7\n\r", ch );
		return;
    }
 
    if (ch->pcdata->pk_timer > 0)
    {
	send_to_char("Your heart is beating too fast to smoke.", ch);
	return;
    }
	if ( ( pObj = get_obj_carry( ch, argument, ch ) ) == NULL )
	{
		send_to_char( "`3You need a pipe if you want to smoke tabac.`7\n\r",ch);
		return;
	}
	if ( pObj->item_type != ITEM_PIPE_TABAC)
	{
		send_to_char( "`3You need a pipe if you want to smoke tabac.`7\n\r",ch);
		return;
	}
	/* When a pipe is empty, it poofs, buy another pipe */
	if ( pObj->value[1] <= 0 )
	{
	    send_to_char( "`3Your pipe is out of tabac.`7\n\r", ch );
	    act( "`3$n frowns regretfully at $p.`7",  ch, pObj, NULL, TO_ROOM );
        extract_obj(pObj);
	    return;
	}
	/* do some randomness */
	range = number_range(1,4);
	switch( range )
	{
		default:
	    	act( "`3$n smokes tabac from $p.`7",  ch, pObj, NULL, TO_ROOM );
		    act( "`3You smoke tabac from $p.`7", ch, pObj, NULL, TO_CHAR );
	     if (ch->mana != ch->max_mana)
		ch->mana += number_range(1,50);
	    break;
		case 1:
	    	act( "`3$n inhales deeply from $p.`7",  ch, pObj, NULL, TO_ROOM );
		    act( "`3You inhale deeply from $p.`7", ch, pObj, NULL, TO_CHAR );
		if (ch->mana != ch->max_mana)
			ch->mana += number_range(1,50);
	    break;
		case 2:
	    	act( "`3$n puffs on $p for a moment.`7",  ch, pObj, NULL, TO_ROOM );
		    act( "`3You puff on $p for a moment.`7", ch, pObj, NULL, TO_CHAR );
		if (ch->mana != ch->max_mana)
			ch->mana += number_range(1,50);
	    break;
		case 3:
	    	act( "`3$n draws deeply on $p.`7",  ch, pObj, NULL, TO_ROOM );
	        act( "`3You draw deeply $p.`7", ch, pObj, NULL, TO_CHAR );
		if (ch->mana != ch->max_mana)
			ch->mana += number_range(1,50);
	    break;
	}

	/* if the the char isn't already smoking, add some smoke
	 * if the char is not set to smoke, they cant exhale. can change
	 * this to act or whatever. in AWW, tabac flags are used for other
	 * things, as well.
	 */
	if ( !IS_SET( ch->tabf, TABF_SMOKING ) )
		SET_BIT(ch->tabf,TABF_SMOKING);

	/* Mix up the numbers a bit */
	amount = number_range(1,2);
    if (pObj->value[0] > 0)
    {
        pObj->value[1] -= amount;
//        capit2(pObj->short_descr);
        amount = number_range(1,6);
        switch ( amount )
        {
			case 1:
			    act( "`3$p crackles and fumes.`7",  ch, pObj, NULL, TO_ROOM );
			    act( "`3$p crackles and fumes.`7",  ch, pObj, NULL, TO_CHAR );
		    break;
		    case 2:
			    act( "`3$p leaks spiralling fingers of smoke.`7",  ch, pObj, NULL, TO_ROOM );
			    act( "`3$p leaks spiralling fingers of smoke.`7",  ch, pObj, NULL, TO_CHAR );
		    break;
		    case 3:
			    act( "`3$p spits tiny red sparks up from it's bowl.`7",  ch, pObj, NULL, TO_ROOM );
			    act( "`3$p spits tiny red sparks up from it's bowl.`7",  ch, pObj, NULL, TO_CHAR );
		    break;
		    case 4:
				act( "`3A thin curl of smoke leaks up from $p's bowl.`7", ch, pObj, NULL, TO_ROOM );
				act( "`3A thin curl of smoke leaks up from $p's bowl.`7", ch, pObj, NULL, TO_CHAR );
			break;
		    case 5:
				act( "`3Long threads of smoke rise up from $p`7", ch, pObj, NULL, TO_ROOM );
				act( "`3Long threads of smoke rise up from $p`7", ch, pObj, NULL, TO_CHAR );
			break;
			default:
			act( "`3$p glows a deep, dark red color.`7", ch, pObj, NULL, TO_ROOM );
			act( "`3$p glows a deep, dark red color.`7", ch, pObj, NULL, TO_CHAR );
			break;
		}
	}
    return;
}

void do_exhale(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	/* that smoking flag has to be set */
	if (!IS_SET(ch->tabf,TABF_SMOKING))
	{
		send_to_char("`&But you aren't smoking from a pipe.`7\n\r",ch);
		return;
	}

    if (ch->pcdata->pk_timer > 0)
    {
        send_to_char("Your heart is beating too fast to smoke.", ch);
        return;
    }

	one_argument( argument, arg );

	/* do the basics */
	if ( arg == NULL)
	{
		stc("You can use many different exhales to enhance your smoking pleasure.\n\r",ch);
		stc("Try these others:\n\r",ch);
		stc("RINGS LINES BALLS WEB PHOENIX HORNS or try EXHALE HELP\n\r",ch);
		act("`3$n exhales some pale gray smoke.`7", ch, NULL, NULL, TO_ROOM );
		act("`3You exhale some pale gray smoke.`7", ch, NULL, NULL, TO_CHAR );
		REMOVE_BIT(ch->tabf,TABF_SMOKING);
		return;
	}
	/* send some help */
	if ( !str_prefix( arg, "help" ))
	{
		stc("Before you can exhale, you must first smoke from your pipe.\n\r", ch);
		stc("After you have enjoyed that flavorful inhalation of vapors...\n\r", ch);
		stc("You can use many different exhales to enhance your smoking pleasure.\n\r",ch);
		stc("Try these different types of exhales to impress and astound:\n\r",ch);
		stc("\n\rRINGS LINES BALLS WEB PHOENIX HORNS or try EXHALE HELP\n\r",ch);
		return;
	}
	if ( !str_prefix( arg, "rings" ))
	{
		act("`3$n exhales large round smoke rings.`7", ch, NULL, NULL, TO_ROOM );
		act("`3You exhale large round smoke rings.`7", ch, NULL, NULL, TO_CHAR );
                REMOVE_BIT(ch->tabf,TABF_SMOKING);
		if (ch->mana != ch->max_mana)
			ch->mana += number_range(1,50);
		return;
	}
	if ( !str_prefix( arg, "lines" ))
	{
		act("`3$n exhales a long line of smoke.`7", ch, NULL, NULL, TO_ROOM );
		act("`3You exhale a long line of smoke.`7", ch, NULL, NULL, TO_CHAR );
                REMOVE_BIT(ch->tabf,TABF_SMOKING);
		if (ch->mana != ch->max_mana)
			ch->mana += number_range(1,50);
		return;
	}
	if ( !str_prefix( arg, "balls" ))
	{
		act("`3$n puffs a big round ball of smoke.`7", ch, NULL, NULL, TO_ROOM );
		act("`3You puff a big round ball of smoke.`7", ch, NULL, NULL, TO_CHAR );
                REMOVE_BIT(ch->tabf,TABF_SMOKING);
		if (ch->mana != ch->max_mana)
			ch->mana += number_range(1,50);
		return;
	}
	if ( !str_prefix( arg, "web" ))
	{
		act("`3$n exhales thin, wispy streams of smoke that merge to form a delicate web.`7", ch, NULL, NULL, TO_ROOM );
		act("`3You exhale thin, wispy streams of smoke that merge to form a delicate web.`7", ch, NULL, NULL, TO_CHAR );
                REMOVE_BIT(ch->tabf,TABF_SMOKING);
		if (ch->mana != ch->max_mana)
			ch->mana += number_range(1,50);
		return;
	}
	if ( !str_prefix( arg, "phoenix" ))
	{
		act("`3$n exhales lines of smoke that form into a phoenix, only to disperse into ashes.`7", ch, NULL, NULL, TO_ROOM );
		act("`3You exhale lines of smoke that form into a phoenix, only to disperse into ashes.`7", ch, NULL, NULL, TO_CHAR );
                REMOVE_BIT(ch->tabf,TABF_SMOKING);
		if (ch->mana != ch->max_mana)
			ch->mana += number_range(1,50);
		return;
	}
	if ( !str_prefix( arg, "horns" ))
	{
		act("`3$n exhales two lines of smoke that settle over $m like a pair of horns.`7", ch, NULL, NULL, TO_ROOM );
		act("`3You exhale two lines of smoke that settle over you like a pair of horns.`7", ch, NULL, NULL, TO_CHAR );
                REMOVE_BIT(ch->tabf,TABF_SMOKING);
		if (ch->mana != ch->max_mana)
			ch->mana += number_range(1,50);
		return;
	}
}
