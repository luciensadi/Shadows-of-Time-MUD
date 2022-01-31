/***************************************************************************
*       ROM 2.4 is copyright 1993-1995 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
*	Auction code by Kimber Boh					   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "tables.h"
#include "lookup.h"

DECLARE_DO_FUN( do_auction );

void show_obj_stats( CHAR_DATA *ch, OBJ_DATA *obj );
void show_obj_info( CHAR_DATA *ch, OBJ_DATA *obj );
void auction_channel( char * msg );

void do_auction( CHAR_DATA *ch, char * argument )
{
    long silver, gold=0;
    OBJ_DATA *	obj;
    char arg1[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	long min;


    argument = one_argument( argument, arg1 );

    if ( ch == NULL || IS_NPC(ch) )
	return;

    if ( arg1[0] == '\0')
    {
	if ( IS_SET(ch->comm,COMM_NOAUCTION) )
	{
	    REMOVE_BIT(ch->comm,COMM_NOAUCTION );
	    send_to_char("Auction channel is now ON.\n\r",ch);
	    return;
	}

	SET_BIT(ch->comm,COMM_NOAUCTION);
	send_to_char("Auction channel is now OFF.\n\r",ch);
	return;
    }

    if ( !str_cmp( arg1, "info" ) )
    {
	obj = auction_info.item;

	if ( !obj )
	{
	    send_to_char("There is nothing up for auction right now.\n\r",ch);
	    return;
	}

	if ( auction_info.owner == ch )
	{
	    sprintf( buf, "\n\rYou are currently auctioning %s.\n\r",
		obj->short_descr );
	    send_to_char( buf, ch );
	    return;
	}

	else 
	    show_obj_info( ch, obj );

	return;
    }

    if ( !str_cmp( arg1, "identify" ) )
    {
	obj = auction_info.item;

	if ( !obj )
	{
	    send_to_char("There is nothing up for auction right now.\n\r",ch);
	    return;
	}

	if ( auction_info.owner == ch )
	{
	    sprintf( buf, "\n\rYou are currently auctioning %s.\n\r",
		obj->short_descr );
	    send_to_char( buf, ch );
	    return;
	}
	
	if ( (ch->silver + 100 * ch->gold) < 500 )
	{
	    send_to_char("You don't have enough money to pay for the identification.\n\r",ch);
	    return;
	}	

	if(ch->silver < 500)
	{
		(ch->gold -= 5);
		show_obj_stats( ch, obj );
		return;
	}
	
	else
	{
	    (ch->silver -= 500);
	    show_obj_stats( ch, obj );
	}
	return;
    }


    if ( !str_cmp( arg1, "cancel" ) )
    {
	
	if(ch->level == MAX_LEVEL)
	{
		if (auction_info.status == 0 || !auction_info.item)
		{
		  send_to_char("No auction to cancel\n\r", ch);
		  return;
		}
		sprintf(buf,"Auction for %s cancelled by immortal.\n\r",
		capitalize(auction_info.item->short_descr));	    
		auction_channel( buf );
	
	    	obj_to_char( auction_info.item, auction_info.owner );
		if ( auction_info.high_bidder != NULL )
		{
	    		auction_info.high_bidder->gold += auction_info.gold_held;
	    		auction_info.high_bidder->silver += auction_info.silver_held;
		}

		sprintf(buf, "%s is returned to you for free due to immortal intervention.\n\r",
		capitalize(auction_info.item->short_descr) );
		send_to_char( buf, auction_info.owner );
	
		auction_info.item		= NULL;
		auction_info.owner		= NULL;
		auction_info.high_bidder	= NULL;
		auction_info.current_bid	= 0;
		auction_info.status		= 0;
		auction_info.gold_held		= 0;
		auction_info.silver_held	= 0;
	
		return;	
	}
	
	if(ch != auction_info.owner)
	{
		sprintf(buf, "This is not your auction, dummy.\n\r");
		send_to_char(buf, ch);
		return;
	}
	
	if(auction_info.status > 2)
	{
		sprintf(buf, "If you made a mistake, you needed to cancel within 2 ticks of the start of the auction...\n\rAuction INC apologizes for any inconveniences.\n\r");
		send_to_char(buf, ch);
		return;
	}
	
	sprintf(buf,"Auction for %s cancelled by seller.\n\r",
	    capitalize(auction_info.item->short_descr));	    
	auction_channel( buf );
	
	    obj_to_char( auction_info.item, auction_info.owner );
	    auction_info.owner->silver -= 50;
	if ( auction_info.high_bidder != NULL )
	{
	    auction_info.high_bidder->gold += auction_info.gold_held;
	    auction_info.high_bidder->silver += auction_info.silver_held;
	}

		sprintf(buf, "%s is returned to you, however, a 50 silver processing fee is taken.\n\r",
		capitalize(auction_info.item->short_descr) );
	send_to_char( buf, auction_info.owner );
	
	auction_info.item		= NULL;
	auction_info.owner		= NULL;
	auction_info.high_bidder	= NULL;
	auction_info.current_bid	= 0;
	auction_info.status		= 0;
	auction_info.gold_held		= 0;
	auction_info.silver_held	= 0;
	
	return;
	
    }
	    	
    
    if ( !str_cmp( arg1, "bid" ) )
    {
	long bid;
        obj = auction_info.item;

        if ( !obj )
        {
            send_to_char("There is nothing up for auction right now.\n\r",ch);
            return;
        }

        if ( obj->pk && ch->clan == clan_lookup("tinker"))
        {
	    send_to_char("That object has been touched by violence, you can't bid on that.\n\r", ch);
            return;
        }
 
	if (ch == auction_info.owner)
        {
            send_to_char("This is your item, you cannot bid.\n\r",ch);
            return;
        }
        if ( argument[0] == '\0' )
	{
	    send_to_char("You must enter an amount to bid.\n\r",ch);
	    return;
	}

	bid = atol( argument );

	if ( bid < auction_info.minimum )
	{
	    sprintf( buf, "The minimum bid is %ld silver.\n\r",auction_info.minimum);
	    send_to_char(buf,ch);
	    return;
	}

	if ( bid <= auction_info.current_bid )
	{
	    sprintf(buf, "You must bid above the current bid of %ld silver.\n\r",
		auction_info.current_bid );
	    return;
	}

	if ( bid < auction_info.current_bid * 1.05)
	{
		sprintf( buf, "You must bid by at least 5 percent more than the last bid, %ld.\n\r",auction_info.current_bid);
		send_to_char(buf,ch);
		return;
	}

	if ( (ch->silver + 100 * ch->gold) < bid )
	{
	    send_to_char("You can't cover that bid.\n\r",ch);
	    return;
	}

	sprintf(buf, "%ld silver has been offered for %s.\n\r",
	    bid, auction_info.item->short_descr);
	auction_channel( buf );

	if ( auction_info.high_bidder != NULL )
	{
	    auction_info.high_bidder->gold += auction_info.gold_held;
	    auction_info.high_bidder->silver += auction_info.silver_held;
	}

	silver = UMIN( ch->silver, bid );

	if ( silver < bid )
	{
	    gold = ((bid - silver + 99 ) / 100 );
	    silver = bid - 100* gold;
	}
  
	ch->gold -= gold;
	ch->silver -= silver;

	auction_info.gold_held		= gold;
	auction_info.silver_held	= silver;
	auction_info.high_bidder	= ch;
	auction_info.current_bid	= bid;
	auction_info.status	 	= 0;

	return;	
    }

    if ( auction_info.item != NULL )
    {
	send_to_char("There is already another item up for bid.\n\r",ch);
	return;
    }

    if ( (obj = get_obj_carry( ch, arg1, ch )) == NULL )
    {
	send_to_char("You aren't carrying that item.\n\r",ch);
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
    {
	send_to_char("You can't let go of that item.\n\r",ch);
	return;
    }

    if (obj->timer > 0)
    {
        send_to_char("You can't auction decomposing items.\n\r", ch);
        return;
    }
	
	min = atol( argument );
	
	if ( min == '\0' )
	{
	    send_to_char("You must enter a minimum bid.\n\r",ch);
	    return;
	}

	if ( min < 50 )
	{
		send_to_char("The minimum bid must be at least 50 silver, to cover Auction INC. Processing fees.\n\r",ch);
		return;
	}

    auction_info.minimum	= min;
    auction_info.owner		= ch;
    auction_info.item		= obj;
    auction_info.current_bid	= 0;
    auction_info.status		= 0;

    sprintf(buf,"Now taking bids on %s, with the minimum bid set at %ld.\n\r", obj->short_descr, auction_info.minimum );
    auction_channel( buf );

    obj_from_char( obj );

    return;

}

void auction_update( )
{
    char buf[MAX_STRING_LENGTH], temp[MAX_STRING_LENGTH],
	temp1[MAX_STRING_LENGTH];

    if ( auction_info.item == NULL )
	return;

    auction_info.status++;

    if ( auction_info.status == AUCTION_LENGTH )
    {
	sprintf(buf,"%s SOLD to %s for %ld silver.\n\r",
	    capitalize(auction_info.item->short_descr),
	    auction_info.high_bidder->name,
	    auction_info.current_bid );
	auction_channel( buf );

	auction_info.owner->gold  += int(auction_info.gold_held * 0.9);
	auction_info.owner->silver += int(auction_info.silver_held * 0.9);

	sprintf(temp1, "%ld gold ", auction_info.gold_held );
	sprintf(temp,  "%ld silver ", auction_info.silver_held );
	sprintf(buf, "You receive %s%s%scoins.\n\rA 10 percent profit is kept for Auction INC services.\n\r",
		auction_info.gold_held > 0 ? temp1 : "",
		((auction_info.gold_held > 0) &&
		 (auction_info.silver_held > 0)) ? "and " : "",
		(auction_info.silver_held > 0) ? temp : "" );
	send_to_char( buf, auction_info.owner );
		
	obj_to_char( auction_info.item, auction_info.high_bidder );

	sprintf(buf, "%s appears in your hands.\n\r",
		capitalize(auction_info.item->short_descr) );
	send_to_char( buf, auction_info.high_bidder );
	
	auction_info.item		= NULL;
	auction_info.owner		= NULL;
	auction_info.high_bidder	= NULL;
	auction_info.current_bid	= 0;
	auction_info.status		= 0;
	auction_info.gold_held		= 0;
	auction_info.silver_held	= 0;

	return;
    }

    if ( auction_info.status == AUCTION_LENGTH - 1 )
    {
	sprintf(buf, "%s - going twice at %ld silver.\n\r",
		capitalize(auction_info.item->short_descr),
		auction_info.current_bid );
	auction_channel( buf );
	return;
    }

    if ( auction_info.status == AUCTION_LENGTH - 2 )
    {
	if ( auction_info.current_bid == 0 )
	{
	    sprintf(buf, "No bids on %s - item removed.\n\r",
		auction_info.item->short_descr);
	    auction_channel( buf );

	    
//	sprintf(buf, "%s is retained by Auction INC.\n\r",
//		capitalize(auction_info.item->short_descr) );
        obj_to_char(auction_info.item, auction_info.owner);
        sprintf(buf, "%s is returned.\n\r", 
                          capitalize(auction_info.item->short_descr));
	send_to_char( buf, auction_info.owner );
	
	    auction_info.item           = NULL;
	    auction_info.owner          = NULL;
	    auction_info.current_bid    = 0;
	    auction_info.status         = 0;

	    return;
	}

        sprintf(buf, "%s - going once at %ld silver.\n\r",
                capitalize(auction_info.item->short_descr),
                auction_info.current_bid );
        auction_channel( buf );
        return;
    }

    return;

}

void auction_channel( char * msg )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    sprintf(buf, "`3[`8AUCTION`3]`7 %s`*", msg ); /* Add color if you
wish */

      for ( d = descriptor_list; d != NULL; d = d->next )
      {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if ( d->connected == CON_PLAYING &&
	     !IS_SET(victim->comm,COMM_NOAUCTION) &&
	     !IS_SET(victim->comm,COMM_QUIET) )
	      {
		send_to_char( buf, victim );
	      }
      }

    return;
}

/*
 * Show_obj_stats: code taken from stock identify spell
 */
void show_obj_stats( CHAR_DATA *ch, OBJ_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;

    sprintf( buf,
	"Object for auction is '%s'.\n\rWeight is %d, value is %d, level is %d.\n\r Current bid is %ld.\n\r",

	obj->name,
	obj->weight / 10,
	obj->cost,
	obj->level,
	auction_info.current_bid
	);
    send_to_char( buf, ch );

    switch ( obj->item_type )
    {
    case ITEM_SCROLL: 
    case ITEM_POTION:
    case ITEM_PILL:
	sprintf( buf, "Level %d spells of:", obj->value[0] );
	send_to_char( buf, ch );

	if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[1]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[2]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	if (obj->value[4] >= 0 && obj->value[4] < MAX_SKILL)
	{
	    send_to_char(" '",ch);
	    send_to_char(skill_table[obj->value[4]].name,ch);
	    send_to_char("'",ch);
	}

	send_to_char( ".\n\r", ch );
	break;

    case ITEM_WAND: 
    case ITEM_STAFF: 
	sprintf( buf, "Has %d charges of level %d",
	    obj->value[2], obj->value[0] );
	send_to_char( buf, ch );
      
	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	send_to_char( ".\n\r", ch );
	break;

    case ITEM_DRINK_CON:
        sprintf(buf,"It holds %s-colored %s.\n\r",
            liq_table[obj->value[2]].liq_color,
            liq_table[obj->value[2]].liq_name);
        send_to_char(buf,ch);
        break;
    case ITEM_ENVELOPE:
    case ITEM_CONTAINER:
	sprintf(buf,"Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
	    obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
	send_to_char(buf,ch);
	if (obj->value[4] != 100)
	{
	    sprintf(buf,"Weight multiplier: %d%%\n\r",
		obj->value[4]);
	    send_to_char(buf,ch);
	}
	break;
		
    case ITEM_WEAPON:
 	send_to_char("Weapon type is ",ch);
	switch (obj->value[0])
	{
	    case(WEAPON_EXOTIC) : send_to_char("exotic.\n\r",ch);	break;
	    case(WEAPON_SWORD)  : send_to_char("sword.\n\r",ch);	break;	
	    case(WEAPON_DAGGER) : send_to_char("dagger.\n\r",ch);	break;
	    case(WEAPON_SPEAR)	: send_to_char("spear.\n\r",ch);	break;
	    case(WEAPON_MACE) 	: send_to_char("mace/club.\n\r",ch);	break;
	    case(WEAPON_AXE)	: send_to_char("axe.\n\r",ch);		break;
	    case(WEAPON_FLAIL)	: send_to_char("flail.\n\r",ch);	break;
	    case(WEAPON_WHIP)	: send_to_char("whip.\n\r",ch);		break;
	    case(WEAPON_POLEARM): send_to_char("polearm.\n\r",ch);	break;
	    case(WEAPON_BOW)    : send_to_char("bow.\n\r",ch);	        break;
	    case(WEAPON_STAFF)  : send_to_char("staff.\n\r",ch);         break;
	    default		: send_to_char("unknown.\n\r",ch);	break;
 	}
	if (obj->pIndexData->new_format)
	    sprintf(buf,"Damage is %dd%d (average %d).\n\r",
		obj->value[1],obj->value[2],
		(1 + obj->value[2]) * obj->value[1] / 2);
	else
	    sprintf( buf, "Damage is %d to %d (average %d).\n\r",
	    	obj->value[1], obj->value[2],
	    	( obj->value[1] + obj->value[2] ) / 2 );
	send_to_char( buf, ch );
        if (obj->value[4])  // weapon flags 
        {
            sprintf(buf,"Weapons flags: %s\n\r",weapon_bit_name(obj->value[4]));
            send_to_char(buf,ch);
        }
	break;

    case ITEM_ARMOR:
	sprintf( buf, 
	"Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.\n\r", 
	    obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
	send_to_char( buf, ch );
	break;
    case ITEM_QUIVER:
            sprintf (buf, "It holds %d of a maximum of %d arrows.\n\r",obj->value[0],obj->value[1]);
            send_to_char(buf,ch);
            break;
    case ITEM_ARROW:
            sprintf (buf, "The arrow will do %dd%d damage for an average of %d\n\r",obj->value[1],obj->value[2], (obj->value[1]*obj->value[2])/2);
            send_to_char(buf,ch);
            break;
        case ITEM_PEN:
        case ITEM_WAX:
            sprintf (buf, "Used: %d  Maximum: %d\n\r",obj->value[0],obj->value[1]);
            send_to_char(buf,ch);
            break;
    }

    if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d.\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	    send_to_char(buf,ch);
            if (paf->bitvector)
            {
                switch(paf->where)
                {
                    case TO_AFFECTS:
                        sprintf(buf,"Adds %s affect.\n",
                            affect_bit_name(paf->bitvector));
                        break;
                    case TO_OBJECT:
                        sprintf(buf,"Adds %s object flag.\n",
                            extra_bit_name(paf->bitvector));
                        break;
                    case TO_IMMUNE:
                        sprintf(buf,"Adds immunity to %s.\n",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_RESIST:
                        sprintf(buf,"Adds resistance to %s.\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_VULN:
                        sprintf(buf,"Adds vulnerability to %s.\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    default:
                        sprintf(buf,"Unknown bit %d: %d\n\r",
                            paf->where,paf->bitvector);
                        break;
                }
	        send_to_char( buf, ch );
	    }
	}
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d",
	    	affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
            if ( paf->duration > -1)
                sprintf(buf,", %d hours.\n\r",paf->duration);
            else
                sprintf(buf,".\n\r");
	    send_to_char(buf,ch);
            if (paf->bitvector)
            {
                switch(paf->where)
                {
                    case TO_AFFECTS:
                        sprintf(buf,"Adds %s affect.\n",
                            affect_bit_name(paf->bitvector));
                        break;
                    case TO_OBJECT:
                        sprintf(buf,"Adds %s object flag.\n",
                            extra_bit_name(paf->bitvector));
                        break;
		    case TO_WEAPON:
			sprintf(buf,"Adds %s weapon flags.\n",
			    weapon_bit_name(paf->bitvector));
			break;
                    case TO_IMMUNE:
                        sprintf(buf,"Adds immunity to %s.\n",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_RESIST:
                        sprintf(buf,"Adds resistance to %s.\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_VULN:
                        sprintf(buf,"Adds vulnerability to %s.\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    default:
                        sprintf(buf,"Unknown bit %d: %d\n\r",
                            paf->where,paf->bitvector);
                        break;
                }
                send_to_char(buf,ch);
             }
	}
    }

    return;
}

void show_obj_info( CHAR_DATA *ch, OBJ_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];
    //AFFECT_DATA *paf;

    sprintf( buf,
	"Object for auction is '%s'.\n\rWeight is %d, value is %d, level is %d.\n\r Current bid is %ld.\n\r",

	obj->name,
	obj->weight / 10,
	obj->cost,
	obj->level,
	auction_info.current_bid
	);
    send_to_char( buf, ch );

return;
}
