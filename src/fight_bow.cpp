/*Cammy Bow Code
 *based upon Kjoda's and Feudal Realms bow firing code
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



// Functions in fight.c

bool    is_safe               args( (CHAR_DATA * ch, CHAR_DATA * victim ) );
// Local Functions
void    range_hit             args( (CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *arrow) );
void set_fighting             args ((CHAR_DATA * ch, CHAR_DATA * victim));
void show_char_condition      args ((CHAR_DATA * ch, CHAR_DATA * victim));
bool calc_hit                 args ((CHAR_DATA * ch, CHAR_DATA * victim, bool secondary, int dt, int dam_type));
int spell_dam                 args ((int base, int number, int die));
void explosion_effect         args ((CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA * arrow, ROOM_INDEX_DATA * tar_room));

void remove_arrow (CHAR_DATA *ch, OBJ_DATA * arrow, ROOM_INDEX_DATA *tar_room)
{
    obj_from_char(arrow);
    if (IS_WEAPON_STAT (arrow, WEAPON_EXPLOSIVE))
    {
        extract_obj(arrow);
        return;
    }
    if (IS_SET(ch->act, PLR_TOURNEY) || IS_SET(ch->act, PLR_QUESTING))
        obj_to_char(arrow,ch);
    else
	obj_to_room(arrow, tar_room);
    
    return;
}

void explosion_effect (CHAR_DATA *ch, CHAR_DATA * victim, OBJ_DATA * arrow, ROOM_INDEX_DATA * tar_room)
{
    CHAR_DATA *tmp_vict, *next_vict;
    char buf [MSL];
    int dam;
    int level = arrow->level;
    
   

    if (victim != NULL)
    {
        if (IS_WEAPON_STAT (arrow, WEAPON_EXPLOSIVE))       
        for (tmp_vict = victim->in_room->people;tmp_vict != NULL; tmp_vict = next_vict)
        {
            next_vict = tmp_vict->next_in_room;
            if (!is_safe_spell (ch, tmp_vict, TRUE))
            {
                if (!IS_SET(tmp_vict->act2, PLR_NODAMAGE))
                    act ("$N is struck by $p and it explodes and the flames engulf you!.", ch, arrow, victim, TO_NOTVICT);
                if (!IS_SET(victim->act2, PLR_NODAMAGE))
                    act ("You are struck by $p and a explosion engulfs you!", victim, arrow, NULL, TO_CHAR);
                if (!IS_SET(ch->act2, PLR_NODAMAGE))
                {
                    sprintf(buf,"Your %s explodes upon impact and engulfs %s's general area!\n\r",arrow->short_descr,IS_NPC(victim)?victim->short_descr:victim->name);
                    send_to_char (buf,ch);
                }
                dam = spell_dam(30,level/6,12);
                if (saves_spell (level, tmp_vict, DAM_FIRE))
                   dam /= 3;
                damage (ch, tmp_vict, dam, 0, DAM_FIRE, FALSE, FALSE);                                       
            }           
                
        }
    }
    else
    {
        if (IS_WEAPON_STAT (arrow, WEAPON_EXPLOSIVE))       
        for (tmp_vict = tar_room->people;tmp_vict != NULL; tmp_vict = next_vict)
        {
            next_vict = tmp_vict->next_in_room;
            if (!is_safe_spell (ch, tmp_vict, TRUE))
            {
                if (!IS_SET(tmp_vict->act2, PLR_NODAMAGE))
                    act ("An $p flies in explodes and the flames engulf you!.", ch, arrow, tmp_vict, TO_VICT);
                if (!IS_SET(tmp_vict->act2, PLR_NODAMAGE))
                    act ("$N is engulfed in the flames!.",ch,arrow,tmp_vict,TO_NOTVICT);
                if (!IS_SET(ch->act2, PLR_NODAMAGE))
                {
                    sprintf(buf,"Your %s explodes upon impact and engulfs the general area!\n\r",arrow->short_descr);
                    send_to_char (buf,ch);
                }
                dam = spell_dam(30,level/5,12);
                if (saves_spell (level, tmp_vict, DAM_FIRE))
                   dam /= 3;
                damage (ch, tmp_vict, dam, 0, DAM_FIRE, FALSE, FALSE);                                       
            }           
                
        }    
    }
    return;
}

int hand_count(CHAR_DATA *ch)
{
    OBJ_DATA *arrow;
    int numberhands=0;
    
    if (get_eq_char(ch,WEAR_SHIELD)!=NULL)numberhands++;    
    if (get_eq_char(ch,WEAR_HOLD)!=NULL)numberhands++;    
    if (get_eq_char(ch,WEAR_WIELD)!=NULL)numberhands++;    
    if (get_eq_char(ch,WEAR_SECONDARY)!=NULL)numberhands++;
    if ((arrow = get_eq_char(ch,WEAR_HOLD))!=NULL)
    {
    	if ((arrow->item_type)==ITEM_ARROW)
    	numberhands--;
    }
    return numberhands;
}

bool do_draw(CHAR_DATA *ch)
{
    OBJ_DATA *bow;
    OBJ_DATA *quiver;
    OBJ_DATA *arrow;
    if (get_skill(ch,gsn_bow)<1)
    {
    	send_to_char("Eh?\n\r",ch);
    	return FALSE;
    }
    if ((bow=get_eq_char(ch,WEAR_WIELD))== NULL)
    {
    	send_to_char("There is no purpose in drawing an arrow if your not wielding a bow.\n\r",ch);
    	return FALSE;
    }

    if ((bow=get_eq_char(ch,WEAR_WIELD))->value[0] != WEAPON_BOW)
    {
    	send_to_char("You aren't wielding a bow.. there isn't any reason to draw an arrow.\n\r",ch);
    	return FALSE;
    }
    if ((quiver=get_eq_char(ch,WEAR_QUIVER))==NULL)
    {
    	send_to_char("You aren't wearing a quiver where you can get to your arrows quickly.\n\r",ch);
    	return FALSE;
    }
    if (quiver->item_type!=ITEM_QUIVER)
    {
    	send_to_char("You can only draw from quivers.\n\r",ch);
    	return FALSE;
    }
    if (quiver->value[0]==0 && quiver->value[0]!=-1)
    {
    	send_to_char("You are out of arrows!\n\r",ch);
    	return FALSE;
    }
    if ((arrow=get_eq_char(ch,WEAR_HOLD))!=NULL)
    {
    	if (arrow->item_type == ITEM_ARROW)
    	{
    	    send_to_char ("Your already holding an arrow!\n\r",ch);
    	    return FALSE;
    	}
    	unequip_char(ch,arrow);
    }


    if (hand_count(ch) > 1)
    {
    	send_to_char("You need a free hand to draw and fire arrows.\n\r",ch);
    	return FALSE;
    }
    if (quiver->value[0]>0)
    {
    	act("$n draws an arrow from $p.",ch,quiver,NULL,TO_ROOM);
    	act("You draw an arrow from $p.",ch,quiver,NULL,TO_CHAR);
    	arrow = create_object(get_obj_index(quiver->value[3]),0);
    	obj_to_char(arrow,ch); 	
        WAIT_STATE (ch, skill_table[gsn_bow].beats / 2);
        if (is_obj_outlevel(ch, arrow))
        {
          if (number_percent() < 25)
          {
            act("You prick your finger while trying to draw $p and drop it.", ch, arrow, NULL, TO_CHAR);
            act("$n pricks $s finger while trying to draw $p and drops it.", ch, arrow, NULL, TO_ROOM);
	    arrow->owner = str_dup(ch->name);
	    obj_damage(arrow, ch, 20, 1048, DAM_PIERCE, TRUE);
	    remove_arrow(ch,arrow,ch->in_room);
	    return TRUE;
          }
	  else
	  {
	    char buf[MSL];
            sprintf (buf, "You don't have the skills necessary to use that yet.\n\r");
            send_to_char (buf, ch);
            act ("$n tries to use $p, but is too inexperienced.",
              ch, arrow, NULL, TO_ROOM);
	  }
        }
	else
	{
          equip_char (ch, arrow, WEAR_HOLD);
	}

        if (quiver->value[0]!= -1)
    	    quiver->value[0]--;
    	if (quiver->value[0]<=0 && quiver->value[0]!=-1)
    	{
    	    act("Your $p is out of arrows, you need to fill it once again.\n\r",ch,quiver,NULL,TO_CHAR);
    	    act("$n draws the last arrow from his $p!\n\r",ch,quiver,NULL,TO_ROOM);
    	    return TRUE;
    	}
    }
    return TRUE;
}

void do_ddraw(CHAR_DATA *ch, char *argument)
{
  do_draw(ch);
}

void show_char_condition (CHAR_DATA *ch,CHAR_DATA * victim)
{
    char buf [MSL];
    int percent;
    
	if (victim->max_hit > 0)
        percent = (100 * victim->hit) / victim->max_hit;
    else
        percent = -1;

    strcpy (buf, PERS (victim, ch, TRUE));

    if (percent >= 100)
        strcat (buf, " is in excellent condition.\n\r");
    else if (percent >= 90)
        strcat (buf, " has a few scratches.\n\r");
    else if (percent >= 75)
        strcat (buf, " has some small wounds and bruises.\n\r");
    else if (percent >= 50)
        strcat (buf, " has quite a few wounds.\n\r");
    else if (percent >= 30)
        strcat (buf, " has some big nasty wounds and scratches.\n\r");
    else if (percent >= 15)
        strcat (buf, " looks pretty hurt.\n\r");
    else if (percent >= 0)
        strcat (buf, " is in awful condition.\n\r");
    else
        strcat (buf, " is bleeding to death.\n\r");

    buf[0] = UPPER (buf[0]);
    send_to_char (buf, ch);
}
//Standard shooting msg
void range_msg ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *arrow,ROOM_INDEX_DATA * room, int dir, int dist)
{
    char buf [MSL];
    act("A $p flys in from the $T straight at $n!", victim, arrow,rev_move[dir], TO_NOTVICT );
    act("A $p flys in from the $T straight at you!", victim, arrow,rev_move[dir], TO_CHAR );
    act("$n aims carefully and shoots $t!",ch, u_dir_name[dir], victim, TO_NOTVICT );    
    sprintf( buf, "Your $p flies %d rooms %s straight at $N!",dist ,u_dir_name[dir] );
    act(buf, ch, arrow, victim, TO_CHAR );
    unequip_char(ch,arrow);
    remove_arrow(ch,arrow,room);
    range_hit(ch,victim,arrow);
    return;  
}

void do_shoot( CHAR_DATA *ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim = NULL;
  ROOM_INDEX_DATA *to_room;
  ROOM_INDEX_DATA *in_room;
  EXIT_DATA *pexit;
  int dir = 0;
  int dist = 0;
  int MAX_DIST = 5;
  OBJ_DATA *bow;
  OBJ_DATA *arrow;

  argument = one_argument( argument, arg1 );//target
  argument = one_argument( argument, arg2 );//direction

  if (get_skill( ch, gsn_bow )<1 )
	{ 
	    send_to_char("Maybe you should learn ranged weapons.\n\r", ch );
  	    return;
	}
  if ( ( bow = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	|| bow->value[0]!=WEAPON_BOW )
    {
	send_to_char("You aren't holding a ranged weapon.\n\r", ch);
	return;
    }
  if (hand_count(ch) > 1)
    {
    	send_to_char("You need a free hand to draw and fire arrows.\n\r",ch);
    	return;
    }

  if ((arrow = get_eq_char(ch,WEAR_HOLD))==NULL)
  {
      send_to_char("You aren't holding a object that can be fired!\n\r",ch);
      return;
  }
 
  if (( arrow = get_eq_char(ch,WEAR_HOLD))->item_type != ITEM_ARROW)
  {
      send_to_char("You aren't holding a object that can be fired!\n\r",ch);
      return;
  }

  in_room = ch->in_room;
  to_room = ch->in_room;

  if ( arg1[0] == '\0' )
  {
    send_to_char("Who do you intend on shooting?\n\r", ch);
    return;
  }
  if (!str_cmp(arg1,"none")|| !str_cmp(arg1,"self") || victim == ch)
  {
     send_to_char("Wouldn't that be bright?? Shooting yourself! INDEED!!\n\r",ch);
     return;
  }
      if ( get_curr_stat( ch, STAT_DEX ) >= 20 )
      {
        MAX_DIST = 7;
        if ( get_curr_stat( ch, STAT_DEX ) == 25 )
          MAX_DIST = 8;
      }

      if ( !str_cmp( arg2, "n" ) || !str_cmp( arg2, "north" ) ) dir =0;
    else if ( !str_cmp( arg2, "e" ) || !str_cmp( arg2, "east"  ) ) dir =1;
    else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "south" ) ) dir =2;
    else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "west"  ) ) dir =3;
    else if ( !str_cmp( arg2, "u" ) || !str_cmp( arg2, "up"    ) ) dir =4;
    else if ( !str_cmp( arg2, "d" ) || !str_cmp( arg2, "down"  ) ) dir =5;
    else
    {
      send_to_char("Shoot in which direction?\n\r", ch );
	return;
    }
	

      if ( ( pexit = to_room->exit[dir] ) == NULL ||
	   ( to_room = pexit->u1.to_room ) == NULL )

      {
	send_to_char("You cannot shoot in that direction.\n\r",ch );
	return;
      }

      if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
      {
	send_to_char("You cannot shoot through a door.\n\r",ch );
	return;
      }
   
    if ( number_percent() > get_skill(ch,gsn_bow) + (get_curr_stat( ch,STAT_DEX ) / 2))
    {
        send_to_char("You fumble your bow!\n\r", ch );
        return;
    }

  
    if (IS_SET(in_room->room_flags,ROOM_SAFE)&& ch->pcdata->pk_timer == 0)
    {
        send_to_char("It wouldn't be proper to shoot here\n\r",ch);
        return;
    }

  

    victim = NULL;
      for ( dist = 1; dist <= MAX_DIST; dist++ )
      {
	char_from_room( ch );
	char_to_room( ch, to_room );
	if ((victim=get_char_room (ch, NULL,arg1))==NULL)
        {
            sprintf( buf, "A $p flys in from $T pass you and heads off to the %s!\n\r",u_dir_name[dir] );
	    act(buf, ch, arrow, rev_move[dir], TO_ROOM );                
        }
        else
        break;      
        if ( ( pexit = to_room->exit[dir] ) == NULL ||
	     ( to_room = pexit->u1.to_room ) == NULL ||
	       IS_SET( pexit->exit_info, EX_CLOSED ))
	{
	  sprintf( buf, "A $p flys in from $T and hits the %s wall.\n\r",u_dir_name[dir] );
	  act(buf, ch, arrow,rev_move[dir], TO_ROOM );
	  sprintf( buf, "You shoot your $p %d room%s $T, where it hits a wall.\n\r", dist, dist > 1 ? "s" : "" );
	  act(buf, ch, arrow, u_dir_name[dir], TO_CHAR );
	  char_from_room( ch );
	  char_to_room( ch, in_room );
	  explosion_effect(ch,victim,arrow,to_room);
	  unequip_char(ch,arrow);
	  remove_arrow(ch,arrow,to_room);
	  return;
	}
	
	   
    }
    if ( dist > 0 && victim != NULL )
    {
      char_from_room( ch );
      char_to_room( ch, in_room );
      if (is_safe(ch,victim))
         return;
      if (victim == ch)
      {
         send_to_char("Shooting at yourself isn't too bright is it?\n\r",ch);
         return;
      }
      range_msg (ch, victim, arrow,to_room, dir, dist);
      check_improve (ch, gsn_bow, TRUE, 2);
      if (IS_AFFECTED(ch,AFF_HASTE))
      {
      	  if (victim->position == POS_DEAD )
                return;  
      	  if (do_draw(ch))
      	  {
      	      arrow = get_eq_char(ch,WEAR_HOLD);
              range_msg (ch, victim, arrow,to_room, dir, dist);
          }
      
      }
      if (get_skill(ch,gsn_second_shot)>1 && number_percent () < (get_skill(ch,gsn_second_shot)/2))
      {
      	  if (victim->position == POS_DEAD )
                return;  
      	  if (do_draw(ch))
      	  {
      	      arrow = get_eq_char(ch,WEAR_HOLD);
              range_msg (ch, victim, arrow, to_room,dir, dist);
              check_improve (ch, gsn_second_shot, TRUE, 4);
          }
      }
      if (get_skill(ch,gsn_third_shot)>1 && number_percent () < (get_skill(ch,gsn_third_shot)/3))
      {
      	  if (victim->position == POS_DEAD )
                return; 
      	  if (do_draw(ch))
      	  {
      	      arrow = get_eq_char(ch,WEAR_HOLD);
              range_msg (ch, victim, arrow,to_room, dir, dist);
              check_improve (ch, gsn_third_shot, TRUE, 6);
          }
      }
        show_char_condition(ch,victim);
        stop_fighting(ch,FALSE);
        if (victim->fighting == ch)
            stop_fighting(victim,FALSE);      
        WAIT_STATE (ch, skill_table[gsn_bow].beats / 2);
        if ( IS_NPC( victim ) )
        {
         if ( victim->level > 3 )
             victim->hunting = ch;
        }
    
            return;
        }
        if (dist >=MAX_DIST)
	{
	  sprintf( buf, "A $p flys in from $T and hits the ground harmlessly.");
	  act(buf, ch, arrow, rev_move[dir], TO_ROOM );
	  sprintf( buf, "You shoot your $p %d room%s $T, where it hits the ground harmlessly.",dist, dist > 1 ? "s" : "" );
	  act(buf, ch, arrow, u_dir_name[dir], TO_CHAR );
	  char_from_room( ch );
	  char_to_room( ch, in_room );
	  explosion_effect(ch,victim,arrow,to_room);
	  unequip_char(ch,arrow);
	  remove_arrow(ch,arrow,to_room);
	  return;
	}
    return;
}
/*
 * Hit one guy once.
 */
void range_hit (CHAR_DATA * ch, CHAR_DATA * victim,OBJ_DATA *arrow)
{
    OBJ_DATA *bow;
    char buf [MSL];
    int dt=0;
    int dam=0;
    int diceroll;
    int sn, skill;
    int dam_type=0;
    bool result;
    bool secondary = FALSE;
    sn = -1;


    /* just in case */
    if (victim == ch || ch == NULL || victim == NULL)
        return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if (victim->position == POS_DEAD)
        return;
    

    /*
     * Figure out the type of damage message.
     */
    if ((bow = get_eq_char (ch, WEAR_WIELD))->value[0]!=WEAPON_BOW)
    {
    	send_to_char("You aren't wielding a bow!\n\r",ch);
    	return;
    }
  
    if (arrow->item_type!=ITEM_ARROW)
    {
    	send_to_char("You aren't holding an arrow to shoot!\n\r",ch);
    	return;
    }

    if (!IS_NPC(ch) && !IS_NPC(victim))
    {
      ch->pcdata->pk_timer = 120;
      ch->pcdata->safe_timer =0;
      victim->pcdata->pk_timer = 120;
    }
    if(IS_AFFECTED(ch, AFF_SHROUD))
    {
	affect_strip(ch, gsn_shroud);
	STR_REMOVE_BIT(ch->affected_by, AFF_SHROUD);
    }
    if (arrow->item_type==ITEM_ARROW)
    {
        dt = TYPE_HIT;
        dam_type = DAM_PIERCE;     
    }
    sn = get_weapon_sn (ch, secondary);
    skill = 20 + get_weapon_skill (ch, sn);

    if (IS_NPC(victim) && IS_SET(victim->imm_flags, IMM_ARROW))
    {
      act("$N is unaffected by $p.", ch, arrow, victim, TO_CHAR);
      act("$N is unaffected by $p.", ch, arrow, victim, TO_ROOM);
      return;
    }


    if (!calc_hit(ch,victim,secondary,dt,dam_type))
        return;
    /*
     * Hit.
     * Calc damage.
     */
        if (sn != -1)
            check_improve (ch, sn, TRUE, 5);
        if (arrow != NULL)
        {
            if (arrow->pIndexData->new_format)
                dam = int(dice (arrow->value[1], arrow->value[2]) * skill / 95.0);
            else
                dam = number_range (int(arrow->value[1] * skill / 95.0),
                                    int(arrow->value[2] * skill / 95.0));

            if (get_eq_char (ch, WEAR_SHIELD) == NULL)    /* no shield = more */
                dam = dam * 11 / 10;

            
        }
        
    if (IS_NPC(victim) && IS_SET(victim->res_flags, RES_ARROW))
      dam /= 2;

    if (IS_NPC(victim) && IS_SET(victim->vuln_flags, VULN_ARROW))
      dam *= 2;

    /*
     * Bonuses.
     */
    if (get_skill (ch, gsn_enhanced_damage) > 0)
    {
        diceroll = number_percent ();
        if (diceroll <= get_skill (ch, gsn_enhanced_damage))
        {
            check_improve (ch, gsn_enhanced_damage, TRUE, 6);
            dam += int(2 * (dam * diceroll / 300.0));
        }
    }
    if (get_skill (ch, gsn_third_shot) > 0)
    {
        diceroll = number_percent ();
        if (diceroll <= get_skill (ch, gsn_third_shot))
        {
            check_improve (ch, gsn_third_shot, TRUE, 6);
            dam = int(dam * 1.25);
        }
    }
    
      /*if ( IS_AFFECTED(ch, AFF_VEIL) )
      {
	double tmp = dam * DAMAGE_VEIL;
        dam = int(tmp);
      } */
      
 
    /* sharpness! */
            if (IS_WEAPON_STAT (arrow, WEAPON_SHARP)
                && !IS_WEAPON_STAT (arrow, WEAPON_VORPAL))
            {
                int percent;

                if ((percent = number_percent ()) <= (skill / 6))
                    dam = 2 * dam + (dam * 2 * percent / 100);
            }
            /* vorpal! */
            if (IS_WEAPON_STAT (arrow, WEAPON_VORPAL)
                && !IS_WEAPON_STAT (arrow, WEAPON_SHARP))
            {
            	int percent;
            	if ((percent = number_percent()) <= (skill / 3))
            	    dam = 2 * dam + (dam * 2 * percent / 100);
            }
    if (!IS_AWAKE (victim))
        dam *= 2;
    else if (victim->position < POS_FIGHTING)
        dam = dam * 3 / 2;

   
    if (dam > 4000)
    {
      dam = 4000;
    }

    dam += GET_DAMROLL (ch) * UMIN (100, skill) / 100;

    if (dam <= 0)
        dam = 1;

    result = damage (ch, victim, dam, dt, dam_type, TRUE,TRUE);

    if (result && arrow != NULL)
    {
        int dam;
        if (victim->position == POS_DEAD )
                return; 
        if (IS_WEAPON_STAT (arrow, WEAPON_POISON))
        {
            int level;
            AFFECT_DATA *poison, af;

            if ((poison = affect_find (arrow->affected, gsn_poison)) == NULL)
                level = arrow->level;
            else
                level = poison->level;

            if (!saves_spell (level / 2, victim, DAM_POISON))
            {
                send_to_char ("You feel poison coursing through your veins.",
                              victim);
                act ("$n is poisoned by the venom on $p.",
                     victim, arrow, NULL, TO_ROOM);
                sprintf(buf,"You see %s's face contort in agony!\n\r",IS_NPC(victim)?victim->short_descr:victim->name);
                send_to_char (buf,ch);

                af.where = TO_AFFECTS;
                af.type = gsn_poison;
                af.level = level * 3 / 4;
                af.duration = level / 2;
                af.location = APPLY_STR;
                af.modifier = -1;
                af.bitvector = AFF_POISON;
                affect_join (victim, &af);
            }

            /* weaken the poison if it's temporary */
            if (poison != NULL)
            {
                poison->level = UMAX (0, poison->level - 2);
                poison->duration = UMAX (0, poison->duration - 1);

                if (poison->level == 0 || poison->duration == 0)
                    act ("The poison on $p is wearing off.", ch, arrow, NULL,
                         TO_CHAR);
            }
        }
        if (IS_WEAPON_STAT (arrow, WEAPON_CONTACT))
        {
            int level;
            AFFECT_DATA *contact, af;

            if ((contact = affect_find (arrow->affected, gsn_contact)) == NULL)
                level = arrow->level;
            else
                level = contact->level;

            
           
                send_to_char ("You feel poison coursing through your veins.",
                              victim);
                act ("$n is poisoned by the venom on $p.",
                     victim, arrow, NULL, TO_ROOM);
                sprintf(buf,"You see %s's face contort in agony!\n\r",IS_NPC(victim)?victim->short_descr:victim->name);
                send_to_char (buf,ch);

                if IS_AFFECTED(ch, AFF_CONTACT)
                {
                af.where = TO_AFFECTS;
                af.type = gsn_contact;
                af.level = level;
                af.duration = level / 15;
                af.location = 0;
                af.modifier = 0;
                af.bitvector = AFF_CONTACT;
                affect_join (victim, &af);
                }
                else 
                {               
                af.where = TO_AFFECTS;
                af.type = gsn_contact;
                af.level = level; 
                af.duration = level / 20;
                af.location = APPLY_SAVING_SPELL;
                af.modifier = +5;
                af.bitvector = 0;
                affect_join (victim, &af);
                }           

            /* weaken the poison if it's temporary */
            if (contact != NULL)
            {
                contact->level = UMAX (0, contact->level - 2);
                contact->duration = UMAX (0, contact->duration - 1);

                if (contact->level == 0 || contact->duration == 0)
                    act ("The poison on $p is wearing off.", ch, arrow, NULL,
                         TO_CHAR);
            }
        }

        if (IS_WEAPON_STAT (arrow, WEAPON_VAMPIRIC))
        {
            dam = number_range (1, arrow->level / 5 + 1);
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
              act ("$p draws life from $n.", victim, arrow, NULL, TO_ROOM);
            if (!IS_SET(victim->act2, PLR_NODAMAGE))
              act ("You feel $p drawing your life away.",
                  victim, arrow, NULL, TO_CHAR);
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
            {
                sprintf(buf,"Your %s draw life from %s!\n\r",arrow->short_descr,IS_NPC(victim)?victim->short_descr:victim->name);
                send_to_char (buf,ch);
            }
            damage (ch, victim, dam, 0, DAM_NEGATIVE, FALSE, FALSE);
            ch->alignment = UMAX (-1000, ch->alignment - 1);
            ch->hit += dam / 2;
        }

        if (IS_WEAPON_STAT (arrow, WEAPON_FLAMING))
        {
            dam = number_range (1, arrow->level / 4 + 1);
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
               act ("$n is burned by $p.", victim, arrow, NULL, TO_ROOM);
            if (!IS_SET(victim->act2, PLR_NODAMAGE))
              act ("$p sears your flesh.", victim, arrow, NULL, TO_CHAR);
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
            {
                sprintf(buf,"Your %s burns %s!\n\r",arrow->short_descr,IS_NPC(victim)?victim->short_descr:victim->name);
                send_to_char (buf,ch);
            }
             fire_effect ((void *) victim, arrow->level / 2, dam, TARGET_CHAR);
            damage (ch, victim, dam, 0, DAM_FIRE, FALSE, FALSE);
        }

        if (IS_WEAPON_STAT (arrow, WEAPON_FROST))
        {
            dam = number_range (1, arrow->level / 6 + 2);
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
              act ("$p freezes $n.", victim, arrow, NULL, TO_ROOM);
            if (!IS_SET(victim->act2, PLR_NODAMAGE))
              act ("The cold touch of $p surrounds you with ice.",
                 victim, arrow, NULL, TO_CHAR);
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
            {
                sprintf(buf,"Your %s freezes %s!\n\r",arrow->short_descr,IS_NPC(victim)?victim->short_descr:victim->name);
                send_to_char (buf,ch);
            }
            cold_effect (victim, arrow->level / 2, dam, TARGET_CHAR);
            damage (ch, victim, dam, 0, DAM_COLD, FALSE, FALSE);
        }

        if (IS_WEAPON_STAT (arrow, WEAPON_SHOCKING))
        {
            dam = number_range (1, arrow->level / 5 + 2);
    
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
              act ("$n is struck by lightning from $p.", victim, arrow, NULL,
                 TO_ROOM);
            if (!IS_SET(victim->act2, PLR_NODAMAGE))
               act ("You are shocked by $p.", victim, arrow, NULL, TO_CHAR);
            if (!IS_SET(ch->act2, PLR_NODAMAGE))
            {
                sprintf(buf,"Your %s shockes %s!\n\r",arrow->short_descr,IS_NPC(victim)?victim->short_descr:victim->name);
                send_to_char (buf,ch);
            }
           
            shock_effect (victim, arrow->level / 2, dam, TARGET_CHAR);
            damage (ch, victim, dam, 0, DAM_LIGHTNING, FALSE, FALSE);
        }
        if (IS_WEAPON_STAT (arrow, WEAPON_EXPLOSIVE))
        {
            explosion_effect(ch,victim,arrow,NULL);
        }
    }

    tail_chain ();
    return;
}
//Some Martial art stuff involving ranges
void do_flying_kick( CHAR_DATA *ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf1[MAX_STRING_LENGTH];//TO_NOTVICT Buffer
  char buf2[MAX_STRING_LENGTH];//TO_VICT
  char buf3[MAX_STRING_LENGTH];//TO_CHAR
  char tmp[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *to_room;
  ROOM_INDEX_DATA *in_room;
  EXIT_DATA *pexit;
  int dir = 0;
  int dist = 1;
  int MAX_DIST = 2;
  int dam;
  int skill=get_skill( ch, gsn_flying_kick);
 
  extern char *const dir_name[];

  argument = one_argument( argument, arg1 );//target
  argument = one_argument( argument, arg2 );//direction

  if (get_skill( ch, gsn_flying_kick )<1)
  { 
	    send_to_char("Maybe you should learn some martial arts.\n\r", ch );
  	    return;
  }
 
  in_room = ch->in_room;
  to_room = ch->in_room;
  
  if (ch->fighting != NULL)
  {
     send_to_char ("The fighting is too intense to accomplish such a move.\n\r",ch);
     return;
  }
  if ( arg1[0] == '\0' )
  {
    send_to_char("Who do you intend on kicking?\n\r", ch);
    return;
  }
  if ((victim=get_char_world(ch,arg1))==NULL)
  {
      send_to_char("You dont see that person around.\n\r",ch);
      return;
  }
  if (!IS_NPC(victim))
  {
      if (IS_SET(in_room->room_flags,ROOM_SAFE)&& ch->pcdata->pk_timer == 0 && victim->pcdata->pk_timer == 0)
      {
          send_to_char("It wouldn't be proper to attempt such a kick here\n\r",ch);
          return;
      }
  }
  else
  {
      if (IS_SET(in_room->room_flags,ROOM_SAFE))
      {
          send_to_char("It wouldn't be proper to attempt such a kick here\n\r",ch);
          return;
      }
}

  if (!str_cmp(arg1,"none")|| !str_cmp(arg1,"self") || victim == ch)
  {
     send_to_char("Wouldn't that be bright?? Kicking yourself! INDEED!!\n\r",ch);
     return;
  }

    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

  if ( get_curr_stat( ch, STAT_DEX ) == 25 )
     MAX_DIST += 1;
      

      if ( !str_cmp( arg2, "n" ) || !str_cmp( arg2, "north" ) ) dir =0;
    else if ( !str_cmp( arg2, "e" ) || !str_cmp( arg2, "east"  ) ) dir =1;
    else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "south" ) ) dir =2;
    else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "west"  ) ) dir =3;
    else if ( !str_cmp( arg2, "u" ) || !str_cmp( arg2, "up"    ) ) dir =4;
    else if ( !str_cmp( arg2, "d" ) || !str_cmp( arg2, "down"  ) ) dir =5;
    else
    {
      send_to_char("Attempt to do a flying kick in which direction?\n\r", ch );
      return;
    }
	


      if ( ( pexit = to_room->exit[dir] ) == NULL ||
	   ( to_room = pexit->u1.to_room ) == NULL )

      {
	send_to_char("You cannot do a jump kick in that direction.\n\r",ch );
	return;
      }

      if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
      {
	send_to_char("The door's too solid to jump kick through!.\n\r",ch );
	return;
      }
   
      if (ch->mana < 50 && ch->move < 50)
      {
         send_to_char ("You feel too tired to attempt that move at this time.\n\r",ch);
         return;
      }
      else
      {
      	 ch->mana -= 35;
      	 ch->move -= 20*MAX_DIST;
      }
      WAIT_STATE (ch, skill_table[gsn_flying_kick].beats);
      for ( dist = 1; dist <= MAX_DIST; dist++ )
      {
	char_from_room( ch );
	char_to_room( ch, to_room );
	if ((victim=get_char_room (ch, NULL,arg1))==NULL)
        {
            char tmp2[MSL];
            colourstrip(tmp,dir_name[rev_dir[dir]]);
            colourstrip(tmp2,dir_name[dir]);           
            sprintf( buf1, "$n runs in from the %s passing you and heads off to the %s!\n\r", tmp, tmp2);
	    act(buf1, ch,NULL,victim, TO_ROOM );                
        }
        else
        break;      
        if ( ( pexit = to_room->exit[dir] ) == NULL ||
	     ( to_room = pexit->u1.to_room ) == NULL ||
	       IS_SET( pexit->exit_info, EX_CLOSED ))
	{
          colourstrip(tmp,dir_name[rev_dir[dir]]);
	  sprintf( buf1, "%s runs in from %s and looks around confused.",ch->name,tmp);	  
	  act(buf1, ch,NULL, victim, TO_ROOM );
	  colourstrip(tmp,dir_name[dir]);
	  sprintf( buf2, "You ran %d room%s %s, but you couldn't find your intended target!\n\r", dist, dist > 1 ? "s" : "",tmp);
	  act(buf2, ch,tmp, victim, TO_CHAR );
	  return;
	}
	
	   
    }
if ( dist > 0 && victim != NULL )
    {
      if (is_safe(ch,victim))
         return;
      if (victim == ch)
      {
         send_to_char("Trying to kick yourself isn't too bright is it?\n\r",ch);
         return;
      }
      colourstrip(tmp,dir_name[rev_dir[dir]]);
      sprintf(buf1,"%s runs in from the %s and does a flying side kick straight at %s!", ch->name,tmp, IS_NPC(victim)?victim->short_descr:victim->name);
      sprintf(buf3,"%s runs in from the %s and does a flying side kick straight at you!", ch->name, tmp);
      sprintf(buf2,"You ran %d rooms from the %s and do a flying side kick straight at %s!",dist,tmp, IS_NPC(victim)?victim->short_descr:victim->name);
      act (buf1, ch, NULL, victim, TO_NOTVICT);
      act (buf2, ch, NULL, victim, TO_CHAR);
      act (buf3, ch, NULL, victim, TO_VICT);                  
      skill=get_skill(ch,gsn_flying_kick);
      dam = number_range (int((ch->level)*2.5*(1+skill)/100), (ch->level)* 4*(1+skill)/100);
      dam += int(GET_DAMROLL (ch) * UMIN (100, skill) / 100.0);
      damage (ch, victim,dam, gsn_flying_kick, DAM_BASH, TRUE, FALSE);
      check_improve (ch, gsn_flying_kick, TRUE, 2);
      return;
    
     }
      if (dist >=MAX_DIST)
	{
	  colourstrip(tmp,dir_name[rev_dir[dir]]);
	  sprintf( buf1, "%s runs in from %s and looks around confused.",ch->name,tmp);	  
	  act(buf1, ch,NULL, victim, TO_ROOM );
	  colourstrip(tmp,dir_name[dir]);
	  sprintf( buf2, "You ran %d room%s %s, but you couldn't find your intended target!\n\r", MAX_DIST, dist > 1 ? "s" : "",tmp);
	  act(buf2, ch,NULL, victim, TO_CHAR );
	  return;
	}
    return;

}

void do_shadow_kick( CHAR_DATA *ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf1[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char buf3[MAX_STRING_LENGTH];
  char tmp[MAX_STRING_LENGTH];
  CHAR_DATA *victim ;
  ROOM_INDEX_DATA *to_room;
  ROOM_INDEX_DATA *in_room;
  EXIT_DATA *pexit;
  int dir = 0;
  int dist = 0;
  int MAX_DIST = 2;
  int dam;
  int skill = get_skill(ch,gsn_shadow_kick);

 
  extern char *const dir_name[];

  argument = one_argument( argument, arg1 );//target
  argument = one_argument( argument, arg2 );//direction

  if (get_skill( ch, gsn_shadow_kick ) < 1 )
  { 
     send_to_char("Maybe you should learn some martial arts.\n\r", ch );
     return;
  }
 
  in_room = ch->in_room;
  to_room = ch->in_room;
  
  if ( arg1[0] == '\0' )
  {
    send_to_char("Who do you intend on kicking?\n\r", ch);
    return;
  }
  if ((victim=get_char_room (ch, NULL,arg1))==NULL)
  {
      send_to_char("You can't find that person here.\n\r",ch);
      return;
  }
  if (!str_cmp(arg1,"none")|| !str_cmp(arg1,"self") || victim == ch)
  {
     send_to_char("Wouldn't that be bright?? Kicking yourself! INDEED!!\n\r",ch);
     return;
  }

    if ( IS_AFFECTED (ch, AFF_RIFT) )
    {
        send_to_char ("The powerful rift of air blocks your attempt.\n\r", ch);
        return;
    }

  if (is_safe(ch,victim))
         return;
	
         if ( !str_cmp( arg2, "n" ) || !str_cmp( arg2, "north" ) ) dir =0;
    else if ( !str_cmp( arg2, "e" ) || !str_cmp( arg2, "east"  ) ) dir =1;
    else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "south" ) ) dir =2;
    else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "west"  ) ) dir =3;
    else if ( !str_cmp( arg2, "u" ) || !str_cmp( arg2, "up"    ) ) dir =4;
    else if ( !str_cmp( arg2, "d" ) || !str_cmp( arg2, "down"  ) ) dir =5;
    else
    {
      send_to_char("Attempt to do a shadow kick in which direction?\n\r", ch );
      return;
    }
      
      if (ch->mana < 50 || ch->move < 50)
      {
         send_to_char ("You feel to tired to attempt that move at this time.\n\r",ch);
         return;
      }
      else
      {
      	 ch->mana -= 15;
         ch->move -= 15;
      	 ch->move -= 10*MAX_DIST;
      }
      if (number_percent()<(get_skill(ch,gsn_shadow_kick)-(get_skill(victim,gsn_dodge)/5)-(get_skill(victim,gsn_evade)/5)+ dex_app[get_curr_stat(ch,STAT_DEX)].offensive))
      {
      for ( dist = 1; dist <= MAX_DIST; dist++ )
      {
	
	if (victim == ch)
        {
            send_to_char("Trying to kick yourself isn't too bright is it?\n\r",ch);
            return;
        }
        
	
	
	if ( ( pexit = to_room->exit[dir] ) == NULL ||
	     ( to_room = pexit->u1.to_room ) == NULL ||
	       IS_SET( pexit->exit_info, EX_CLOSED ))
	{
	    if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
               return;
	    colourstrip(tmp,dir_name[dir]);
            sprintf(buf1,"%s seems to levitate into the air while his feet pummels you into the %s wall!", ch->name, tmp );
            sprintf(buf2,"You levitate into the air while your feet pummels %s into the %s wall!",IS_NPC(victim)?victim->short_descr:victim->name, tmp);
            sprintf(buf3,"%s seems to levitate into the air while his feet pummels %s into the %s wall!", ch->name,IS_NPC(victim)?victim->short_descr:victim->name, tmp );
            act (buf3, ch, NULL, victim, TO_NOTVICT);
            act (buf2, ch, NULL, victim, TO_CHAR);
            act (buf1, ch, NULL, victim, TO_VICT);
            dam =number_range (int((ch->level)*2.0*(1+skill)/100.0), int((ch->level)* 3.0*(1+skill)/100.0));
            dam += int(GET_DAMROLL (ch) * UMIN (100, skill) / 100.0);
            damage (ch, victim,dam, gsn_shadow_kick, DAM_BASH, TRUE, FALSE);
            check_improve (ch, gsn_shadow_kick, TRUE, 2);
	}	
	if (pexit != NULL
	    && to_room != NULL
	    && !IS_SET(pexit->exit_info, EX_CLOSED))
        {   
            if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
                return;    
            //stop_fighting( ch, FALSE );
            //stop_fighting( victim, FALSE );
            char_from_room( ch );
 	    char_to_room( ch, to_room );
            char_from_room( victim );
	    char_to_room(victim, to_room );
	    set_fighting(ch,victim);
	    set_fighting(victim,ch);
	    
	    
            colourstrip(tmp,dir_name[dir]);
            sprintf(buf1,"%s seems to levitate into the air while his feet pummels you %s!", ch->name, tmp );
            sprintf(buf2,"You levitate into the air while your feet pummels %s to the %s!",IS_NPC(victim)?victim->short_descr:victim->name, tmp);
            sprintf(buf3,"%s seems to levitate into the air while his feet pummels %s %s!", ch->name,IS_NPC(victim)?victim->short_descr:victim->name, tmp );
            act (buf3, ch, NULL, victim, TO_NOTVICT);
            act (buf2, ch, NULL, victim, TO_CHAR);
            act (buf1, ch, NULL, victim, TO_VICT);
            dam =number_range (int((ch->level)*1.2*(1+skill)/100), (ch->level)* 2*(1+skill)/100);
            dam += int(GET_DAMROLL (ch) * UMIN (100, skill) / 100.0);
            damage (ch, victim,dam, gsn_shadow_kick, DAM_BASH, TRUE, FALSE);
            check_improve (ch, gsn_shadow_kick, TRUE, 2);
        }
          
    }
    }
    else
    {
    	 if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
         return;
    	 sprintf(buf1,"%s seems to levitate into the air while his feet pummels you but managed to dodge most of his kicks!", ch->name);
         sprintf(buf2,"You levitate into the air while your feet pummels %s but he managed to dodge most of his kicks!",IS_NPC(victim)?victim->short_descr:victim->name);
         sprintf(buf3,"%s seems to levitate into the air while his feet pummels %s but he managed to dodge most of your kicks !", ch->name,IS_NPC(victim)?victim->short_descr:victim->name);
         act (buf3, ch, NULL, victim, TO_NOTVICT);
         act (buf2, ch, NULL, victim, TO_CHAR);
         act (buf1, ch, NULL, victim, TO_VICT);
         dam =number_range (int((ch->level)*0.85*(1+skill)/100), (ch->level)* 2*(1+skill)/100);
         dam += GET_DAMROLL (ch) * UMIN (100, skill) / 100;
         damage (ch, victim,dam, gsn_shadow_kick, DAM_BASH, TRUE, FALSE);
         check_improve (ch, gsn_shadow_kick, FALSE, 2);
    }
    WAIT_STATE (ch, skill_table[gsn_shadow_kick].beats);
 
      

}
void do_ironskin (CHAR_DATA * ch, char *arguement)
{    
    AFFECT_DATA af;
    if (get_skill(ch,gsn_marironskin)<1)
    {
    	send_to_char ("Huh?\n\r",ch);
    	return;
    }
    if (IS_AFFECTED (ch, AFF_IRONSKIN))
    {
        send_to_char ("You are already concentrating on maintaining iron skin.\n\r", ch);
        return;
    }
    if (IS_AFFECTED (ch, AFF_SANCTUARY))
    {
        send_to_char ("You are already concentrating on maintaining sanctuary to be involved on maintaining iron skin.\n\r", ch);
        return;
    }
    if (ch->move <0||ch->mana<0)
    {
    	send_to_char ("You cannot relax your mind nor body enough to achieve the proper discipline for iron skin.\n\r",ch);
    	return;
    }
    ch->move -= 3+ch->level/3;
    ch->mana -= 3+ch->level/5;
    WAIT_STATE (ch, skill_table[gsn_marironskin].beats);
    if (get_skill(ch,gsn_marironskin)>number_percent())
    {
    af.where = TO_AFFECTS;
    af.type = gsn_marironskin;
    af.level = int(ch->level / 1.1);
    af.duration = 3+ch->level / 5;
    af.location = APPLY_AC;
    af.modifier = int(-10 - ch->level*1.2);
    af.bitvector = AFF_IRONSKIN; 
    affect_to_char (ch, &af);

    act ("$n face turns stoic and appears to be in deep concentration.", ch, NULL, NULL, TO_ROOM);
    send_to_char ("You focus your mind and body causing it to harden like iron.\n\r", ch);
    check_improve (ch, gsn_marironskin, TRUE, 2);
    }
    else
    send_to_char ("You fail to focus your mind and body enough to achieve iron skin.\n\r",ch);
    check_improve (ch, gsn_marironskin, FALSE, 2);
    return;
}
/*Cammy - Some Martial Artist skills*/
void do_meditation (CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj = NULL;

    switch (ch->position)
    {
    	case POS_MEDITATING:
    	    send_to_char ("You are already meditating.\n\r",ch);
    	    break;
        case POS_SLEEPING:
        case POS_RESTING:
        case POS_SITTING:
        case POS_STANDING:
            
            if (ch->fighting != NULL)
            {
            	send_to_char("You can't do that while your fighting!!",ch);
            	return;
            }
            if (str_cmp("form master", class_table[ch->cClass].name))
            {
            	act("$n assumes a very comfortable position and begins to meditate.",ch,NULL,NULL,TO_NOTVICT);
            	act("You assume a very comfortable position and begin to meditate.",ch,NULL,NULL,TO_CHAR);
            	return;
            }
            if (number_percent()<get_skill(ch,gsn_meditation))
            {
            check_improve(ch,gsn_meditation,TRUE,10);
            if (argument[0] == '\0' && ch->on == NULL)
            {
                send_to_char ("You sit down cross-legged and begin to meditate.\n\r", ch);
                act ("$n sits down and begins meditating.", ch, NULL, NULL, TO_ROOM);
                ch->position = POS_MEDITATING;
                
            }
            else
            {                    /* find an object and meditate on it */

                if (argument[0] == '\0')
                    obj = ch->on;
                else
                    obj = get_obj_list (ch, argument, ch->in_room->contents);

                if (obj == NULL)
                {
                    send_to_char ("You don't see that here.\n\r", ch);
                    return;
                }
                if (obj->item_type != ITEM_FURNITURE
                    || (!IS_SET (obj->value[2], MEDITATE_ON)
                        && !IS_SET (obj->value[2], MEDITATE_IN)
                        && !IS_SET (obj->value[2], MEDITATE_AT)))
                {
                    send_to_char ("You can't meditate on that!\n\r", ch);
                    return;
                }

                if (ch->on != obj && count_users (obj) >= obj->value[0])
                {
                    act_new ("There is no room on $p for you.",
                             ch, obj, NULL, TO_CHAR, POS_DEAD);
                    return;
                }

                ch->on = obj;
                if (IS_SET (obj->value[2], MEDITATE_AT))
                {
                    act ("You go to meditate at $p.", ch, obj, NULL, TO_CHAR);
                    act ("$n goes to meditate at $p.", ch, obj, NULL, TO_ROOM);
                }
                else if (IS_SET (obj->value[2], MEDITATE_ON))
                {
                    act ("You go to meditate on $p.", ch, obj, NULL, TO_CHAR);
                    act ("$n goes to meditate on $p.", ch, obj, NULL, TO_ROOM);
                }
                else
                {
                    act ("You go to meditate in $p.", ch, obj, NULL, TO_CHAR);
                    act ("$n goes to meditate in $p.", ch, obj, NULL, TO_ROOM);
                }
                ch->position = POS_MEDITATING;
            }
            }
            else
            {
            	check_improve(ch,gsn_meditation,FALSE,10);
            	send_to_char("You have failed to achieve a tranquil state of mind.\n\r",ch);
            	return;
            }
            break;

        case POS_FIGHTING:
            send_to_char ("You are already fighting!\n\r", ch);
            break;
    }

    return;
}
void do_awareness(CHAR_DATA *ch, char *argument) 
{ 
 AFFECT_DATA af; 

 if (IS_NPC(ch)) 
     return; 
 if (get_skill(ch,gsn_awareness)<1)
 {
     send_to_char("You look around yourself like a paranoid lunatic.",ch);
     act("$n glances about like a paranoid lunatic!", ch, NULL, NULL, TO_ROOM);
     return;
 }

 if ( IS_AFFECTED(ch, AFF_AWARENESS)) 
 { 
     send_to_char("You are already aware.\n\r",ch); 
     return; 
 } 

 WAIT_STATE( ch, skill_table[gsn_awareness].beats ); 

 if (number_percent() > get_skill(ch,gsn_awareness)) 
 { 
     send_to_char("You are aware of everything, you think.\n\r", ch); 
     act("$n peers intently around themselves.", ch, NULL, NULL, TO_ROOM);
      check_improve(ch,gsn_awareness,FALSE,2);
        return; 
 } 

 af.where     = TO_AFFECTS; 
 af.type      = gsn_awareness; 
 af.level     = ch->level; 
 af.duration  = ch->level/3; 
 af.location  = APPLY_NONE; 
 af.modifier  = 0; 
 af.bitvector = AFF_AWARENESS; 
 affect_to_char( ch, &af );
 check_improve(ch,gsn_awareness,TRUE,2);
 send_to_char("Your eyes glows with intensity as you become aware to everything.\n\r",ch); 
 return; 
} 

void spell_manashield (int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (victim != ch)
    {
    	send_to_char("Not on another target.\n\r",ch);
    	return;
    }
    if (IS_AFFECTED (victim, AFF_MANASHIELD))
    {       
        send_to_char ("You have already woven weaves to protect yourself.\n\r", ch);   
        return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = ch->level/10;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_MANASHIELD;
    affect_to_char (victim, &af);
    send_to_char ("You feel protected by the source.\n\r", victim);
    return;
}
