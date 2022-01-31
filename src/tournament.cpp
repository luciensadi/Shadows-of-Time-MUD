/* Tournament Code by Asmodeus */
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "lookup.h"
#include "tournament.h"


const struct tournament_rank_type tournament_ranks[MAX_TOURNAMENT_RANK] =
{
/*  {"Rank", Rep Required, Prize} */
    {"Newbie",          0,     0, "`&(`#Newbie`&)`* "},
    {"Apprentice",     50,   500, "`&(`#Apprentice`&)`* "},
    {"Journeyman",    100,   750, "`&(`#Journeyman`&)`* "},
    {"Mercenary",     200,  1000, "`&(`#Mercenary`&)`* "},
    {"Executioner",   400,  1500, "`&(`#Executioner`&)`* "},
    {"Soul Reaver",  1000,  2000, "`&(`#Soul Reaver`&)`* "},
    {"Master",       2000,  3000, "`&(`#Master`&)`* "},
    {"Grand Master", 3500,  5000, "`&(`#Grand Master`&)`* "},
    {"Badass",       6000, 10000, "`&(`#BadAss`&)`* "},
    {"Khan",        10000, 20000, "`&(`#Khan`&)`* "},
};

char *show_trank(CHAR_DATA *ch)
{
  if (ch->pcdata->tournament.reputation <= -25)
    return "`&(`#Helpless`&)`* ";

  return tournament_ranks[ch->pcdata->tournament.rank].who;
}
 
void init_tournament(void)
{
  tournament.status = TOURNAMENT_OFF;
  tournament.players = 0;
  tournament.tickstill = 0;
  tournament.maxrank = MAX_TOURNAMENT_RANK - 1;
  tournament.noguild = FALSE;
}  

void setup_tournament(CHAR_DATA *ch, int rank, bool noguild)
{
  tournament.status = TOURNAMENT_STARTING;
  tournament.players = 1;
  tournament.tickstill = 4;
  tournament.maxrank = rank;
  tournament.noguild = noguild;
  if (!noguild)
    global("A Tournament has been started, type tournament join to join the fun.\n\r", LEVEL_HERO - 1);
  else
    global("A Tournament without guild skills has been started, type tournament join to join the fun.\n\r", LEVEL_HERO - 1);

}

void do_tournament(CHAR_DATA *ch, char *argument)
{
  char arg[MIL];
  
  if (argument[0] == '\0')
  {
    send_to_char("Syntax for tournament:\n\r", ch);  
    send_to_char("tournament start - start tournament\n\r", ch);
    send_to_char("tournament join  - join tournament\n\r", ch);
    send_to_char("tournament leave - leave the tournament\n\r", ch);
    return;
  } 

  argument = one_argument(argument, arg);

  if (!str_cmp(arg, "start"))
  {
    int rank_restrict = MAX_TOURNAMENT_RANK - 1;

    if (ch->pcdata->pk_timer > 0)
    {
      send_to_char("You're way to pumped up for that.\n\r", ch);
      return;
    }

    if (ch->level < 75)
    {
      send_to_char("Tournaments are for level 75 and legend only.\n\r", ch);
      return;
    }
  
    if (tournament.status != TOURNAMENT_OFF)
    {
      send_to_char("A tournament is already running.\n\r", ch);
      return;
    }

    if (argument[0] != '\0')
    {
      int i;
      bool found = FALSE;

      for (i = 0;i < MAX_TOURNAMENT_RANK;i++)
      {
        if (!str_cmp(tournament_ranks[i].name, argument))
        {
          found = TRUE;
          break;
        }
      }

      if (!found)
      {
        if (!str_cmp(argument, "noguild"))
        {
          setup_tournament(ch, rank_restrict, TRUE);
          return;
        }
        else
        {
          send_to_char("No such rank.\n\r", ch);
          return;
        }
      }
      rank_restrict = i;  
    }          
    
    SET_BIT(ch->act2, PLR_TOURNAMENT_START);
    setup_tournament(ch, rank_restrict, false);
  } 

  if (!str_cmp(arg, "spectate"))
  {
    if (IS_SET(ch->act, PLR_TOURNEY) || IS_SET(ch->act2, PLR_TOURNAMENT_START))
    {
      send_to_char("You've already joined, why spectate?!\n\r", ch);
      return;
    } 

    if (IS_SET(ch->act2, PLR_TOURNAMENT_SPECT))
    {
      send_to_char("You're already spectating.\n\r", ch);
      return;
    }
 
    SET_BIT(ch->act2, PLR_TOURNAMENT_SPECT);
    send_to_char("You become a violence voyeur.\n\r", ch);
    return;
  } 

  if (!str_cmp(arg, "join"))
  {
    char buf[MSL]; 
    if (ch->level <  LEVEL_HERO - 1)
    {
      send_to_char("Tournaments are for level 75 and legend only.\n\r", ch);
      return;
    }

    if (ch->pcdata->pk_timer > 0)
    {
      send_to_char("You're way to pumped up for that.\n\r", ch);
      return;
    }

  
    if (tournament.status == TOURNAMENT_OFF)
    {
      send_to_char("There is no tournament running.\n\r", ch);
      return;
    }
  
    if (IS_SET(ch->act, PLR_TOURNEY) || IS_SET(ch->act2, PLR_TOURNAMENT_START))
    {
      send_to_char("You've already joined!\n\r", ch);
      return;
    } 
  
    if (ch->pcdata->tournament.rank > tournament.maxrank)
    {
      send_to_char("Sorry you're too high rank for this tournament.\n\r", ch);
      return;
    }

    tournament.players += 1;
    if (tournament.status == TOURNAMENT_ON)
    { 
      SET_BIT(ch->act, PLR_TOURNEY);
      send_to_char("You have joined the tournament, watch your back!\n\r", ch);
      sprintf(buf, "`@%s `&has joined the tournament.`*\n\r", ch->name);
      techo(buf);
      return;
    }
    else
    {
      SET_BIT(ch->act2, PLR_TOURNAMENT_START);
      send_to_char("You have joined the tournament, waiting for tournament to start.\n\r", ch);
      return;
    }
  }

  if (!str_cmp(arg, "leave"))
  {
    char buf[MSL];

    if (!IS_SET(ch->act, PLR_TOURNEY) && 
        !IS_SET(ch->act2, PLR_TOURNAMENT_START) && 
        !IS_SET(ch->act2, PLR_TOURNAMENT_SPECT))
    {
      send_to_char("You aren't in a tournament.\n\r", ch);
      return;
    }

    if (!IS_SET(ch->act, PLR_TOURNEY) && 
        !IS_SET(ch->act2, PLR_TOURNAMENT_START) && 
        IS_SET(ch->act2, PLR_TOURNAMENT_SPECT))
    {
      send_to_char("You stop watching the tournament.\n\r", ch);
      REMOVE_BIT(ch->act2, PLR_TOURNAMENT_SPECT);
      return;
    }

    if (ch->pcdata->pk_timer > 0)
    {
      send_to_char("You can't leave in the heat of battle.\n\r", ch);
      return;
    }

    send_to_char("You have been removed from the tournament.\n\r", ch);
    sprintf(buf, "`@%s `&has left the tournament.`*\n\r", ch->name);
    techo(buf);
    tournament.players -= 1;
    if (tournament.players < TOURNAMENT_NEEDED_PLAYERS)
      tournament.tickstill = 2;

    if (IS_SET(ch->act, PLR_TOURNEY))
    {
      REMOVE_BIT(ch->act, PLR_TOURNEY);
    }
    if (IS_SET(ch->act2, PLR_TOURNAMENT_START))
    {
      REMOVE_BIT(ch->act2, PLR_TOURNAMENT_START);
    }
    ch->pcdata->tournament.last_killed = 0; 
  }  

  if (!str_cmp(arg, "quiet"))
  {
    if (!IS_SET(ch->comm, COMM_NOTOURNEY))
    {
       send_to_char("Turning off tournament annoncements.\n\r", ch);
       SET_BIT(ch->comm, COMM_NOTOURNEY);
       return;
    }
    send_to_char("Turning on tournament announcements.\n\r", ch);
    REMOVE_BIT(ch->comm, COMM_NOTOURNEY);
    return;
  }

  if (!str_cmp(arg, "status"))
  {
    char buf[MSL];
    sprintf(buf, "Tournament Status:   %-10s.\n\r", 
      tournament.status == TOURNAMENT_ON ? "on" :
      tournament.status == TOURNAMENT_STARTING ? "starting" :
      "off");
    send_to_char(buf, ch);
    if (tournament.status != TOURNAMENT_OFF)
    {
      sprintf(buf, "Number of players:    %-2d\n\r",
        tournament.players);
      send_to_char(buf, ch);
      sprintf(buf, "Max Rank:             %-10s\n\r", 
        tournament_ranks[tournament.maxrank].name);
      send_to_char(buf, ch);
    }
    return;	
  }

  if (!str_cmp(arg, "who"))
  {
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char buf[MSL];

    sprintf(buf, "`&%10s - Kills  Deaths  Reputation  Rank`*\n\r", "Name");
    send_to_char(buf, ch);
    send_to_char("`$-----------------------------------------------`*\n\r", ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      victim = d->character;
      if (d->connected != CON_PLAYING)
        continue;
      if (IS_NPC(victim))
        continue;
      if (victim->level < LEVEL_HERO - 1 || victim->level >= LEVEL_IMMORTAL || 
	   victim->clan == clan_lookup("Guide"))
        continue;
      if (!can_see(ch, victim))
        continue;
      sprintf(buf, "%10s - %3d    %3d     %5d      %-15s\n\r", victim->name,
              victim->pcdata->tournament.kills,
              victim->pcdata->tournament.deaths,
              victim->pcdata->tournament.reputation,
              tournament_ranks[victim->pcdata->tournament.rank].name);
      send_to_char(buf, ch);
    }
    send_to_char("`$-----------------------------------------------`*\n\r", ch);

  }

  if (!str_cmp(arg, "ranks"))
  {
    int i;
    char buf[MSL];

    send_to_char("Rank Table:\n\r\n\r", ch);
    sprintf(buf, "`&%-15s    %5s   %5s`*\n\r", "Rank", "Rep Req", "Prize");
    send_to_char(buf, ch);
    send_to_char("`$----------------------------------`*\n\r", ch);
    sprintf(buf, "%-15s     %5d    %5d\n\r", "Helpless", 
	-25, 0);
    send_to_char(buf, ch);
    for (i = 0; i < MAX_TOURNAMENT_RANK;i++)
    {
      sprintf(buf, "%-15s     %5d    %5d\n\r", tournament_ranks[i].name, 
		tournament_ranks[i].reputation, tournament_ranks[i].prize);
      send_to_char(buf, ch);
    }
    send_to_char("`$----------------------------------`*\n\r", ch);
  }


}

void tournament_update(void)
{
  if (tournament.tickstill > 0) 
    tournament.tickstill -= 1;
  
  if (tournament.status == TOURNAMENT_STARTING)
  {
    char buf[MSL];
    if (tournament.tickstill > 0)
    {
      if (tournament.players < TOURNAMENT_NEEDED_PLAYERS)
      {
        sprintf(buf, "`&`!%d`& ticks before tournament `!%d`& players needed to start.`*\n\r",
                     tournament.tickstill, TOURNAMENT_NEEDED_PLAYERS - tournament.players);
      }
      else
        sprintf(buf, "`&`!%d`& ticks before tournament starts.`*\n\r", tournament.tickstill);
      global(buf, LEVEL_HERO - 1);
      return; 
    }
    else
    {
      if (tournament.players >= TOURNAMENT_NEEDED_PLAYERS)
      {
        start_tournament();
	return;
      }
      else
      {
          abandon_tournament();

	return;
      }	 
    }
  }

  if (tournament.status == TOURNAMENT_ON)
  {
    DESCRIPTOR_DATA *d;

    tournament.players = 0;
    for (d = descriptor_list;d != NULL; d = d->next)
    {
       CHAR_DATA *vct;
       vct = d->original ? d->original : d->character;
     
       if ( d->connected == CON_PLAYING &&
	    IS_SET(vct->act, PLR_TOURNEY))
            tournament.players++;
    }
  }

  if (tournament.status == TOURNAMENT_ON && tournament.players < TOURNAMENT_NEEDED_PLAYERS)
  {
    char buf[MSL];
    if (tournament.tickstill > 0) 
    {
      sprintf(buf, "`&Not enough players to continue tournament, `!%d`& ticks left to get `!%d`& players.`*\n\r",
              tournament.tickstill, TOURNAMENT_NEEDED_PLAYERS - tournament.players);
      global(buf, LEVEL_HERO - 1);
      return;
    }
    else
    {
       bool fighting = FALSE;
       DESCRIPTOR_DATA *d;
       CHAR_DATA *victim; 

       for (d = descriptor_list; d != NULL; d = d->next)
       {
         victim = d->character;
         if (victim == NULL || IS_NPC(victim))
           continue;
         if (IS_SET(victim->act, PLR_TOURNEY) && (victim->pcdata->pk_timer > 0))
           fighting = TRUE;
       }

      if (!fighting)   
       abandon_tournament();
    }
  }
}
      
void start_tournament()
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;

  tournament.status = TOURNAMENT_ON;
  
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    victim = d->character; 
    if (d->connected != CON_PLAYING)
      continue;

    if (victim == NULL || IS_NPC(victim))
      continue;

    if (!IS_SET(victim->act2, PLR_TOURNAMENT_START))
      continue;

    REMOVE_BIT(victim->act2, PLR_TOURNAMENT_START);
    SET_BIT(victim->act, PLR_TOURNEY);
    affect_strip (victim, gsn_plague);
    affect_strip (victim, gsn_poison);
    affect_strip (victim, gsn_blindness);
    affect_strip (victim, gsn_sleep);
    affect_strip (victim, gsn_curse);

    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    update_pos (victim);
  }
  techo("`!The tournament has now started, let the killing begin.`*\n\r");
}      

void abandon_tournament()
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;
  tournament.status = TOURNAMENT_OFF;
  
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    victim = d->character; 
    if (d->connected != CON_PLAYING)
      continue;

    if (victim == NULL || IS_NPC(victim))
      continue;

    if (IS_SET(victim->act2, PLR_TOURNAMENT_SPECT))
      REMOVE_BIT(victim->act2, PLR_TOURNAMENT_SPECT); 

    if (!IS_SET(victim->act, PLR_TOURNEY) && !IS_SET(victim->act2, PLR_TOURNAMENT_START))
      continue;

    send_to_char("`!The tournament has been abandoned.`*\n\r", victim);
    REMOVE_BIT(victim->act, PLR_TOURNEY);
    REMOVE_BIT(victim->act2, PLR_TOURNAMENT_START);
    char_from_room(victim);
    char_to_room(victim, get_room_index(ROOM_VNUM_ALTAR));
    victim->pcdata->pk_timer = 0;
    stop_fighting(victim, true);
    victim->pcdata->safe_timer = 30;     
    victim->pcdata->tournament.last_killed = 0;

  }
}      

void techo(char *buf)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    victim = d->character; 
    if (d->connected != CON_PLAYING)
      continue;

    if (victim == NULL || IS_NPC(victim))
      continue;

    if (!IS_SET(victim->act, PLR_TOURNEY) 
        && !IS_SET(victim->act2, PLR_TOURNAMENT_START)
        && !IS_SET(victim->act2, PLR_TOURNAMENT_SPECT))
      continue;

    send_to_char(buf, victim);
  }
}      

void global(char *buf, int level)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    victim = d->character; 
    if (d->connected != CON_PLAYING)
      continue;
 
    if (victim->level < level)
      continue; 

    if (victim == NULL || IS_NPC(victim))
      continue;

    if (IS_SET(victim->comm, COMM_NOTOURNEY))
      continue;

    send_to_char(buf, victim);
  }
}      

void tournament_death(CHAR_DATA *ch, CHAR_DATA *victim)
{
  int rankDiff;
  int vict_rep;
  char buf[MSL]; 
  
  if (victim->id == ch->pcdata->tournament.last_killed)
    return;

  ch->pcdata->tournament.last_killed = victim->id;

  rankDiff = victim->pcdata->tournament.rank - ch->pcdata->tournament.rank;
  vict_rep = victim->pcdata->tournament.reputation;
  {
    int random = 0;
    random = number_range(1,15);
    switch (random)
    {
    case 1:
       sprintf(buf, "`@%s`& just got a new bunghole ripped by `@%s`*\n\r", victim->name, ch->name);
       break;
    case 2:
      sprintf(buf, "`@%s`& took the `@%s`&\'s Pepsi Challenge, and got sprayed with a can of whoopass!`*\n\r", victim->name, ch->name);
      break;
    case 3:
      sprintf(buf, "`@%s\'s`& head leaves a blood trail, as `@%s`& drags it behind him!`*\n\r", victim->name, ch->name);
      break;
    case 4:
      sprintf(buf, "`@%s`& broke his foot off in `@%s`&\'s ass!`*\n\r", ch->name, victim->name);
      break;
    case 5:
      sprintf(buf, "`@%s`&\'s ass is grass, and `@%s`&\'s the lawnmower!`*\n\r", victim->name, ch->name);
      break;
    case 6:
      sprintf(buf, "`@%s`& waves his magic wand *POOF* `@%s`& is dead!`*\n\r", ch->name, victim->name);
      break;
    case 7:
      sprintf(buf, "`@%s`& points to `@%s`& and says, \"Excuse the mess.\"`*\n\r", ch->name, victim->name);
      break;
    case 8:
      sprintf(buf, "`@%s`& ripped `@%s`& into two halves!`*\n\r", ch->name, victim->name);
      break;
    case 9:
      sprintf(buf, "`@%s`& mangles `@%s`& and calls it \"Modern Art\".`*\n\r", ch->name, victim->name);
      break;
    case 10:
      sprintf(buf, "`@%s`& gives `@%s`& the boot ... TO THE HEAD!`*\n\r", ch->name, victim->name);
      break;
    case 11:
      sprintf(buf, "`@%s`& fronts to `@%s`&, \"Talk to the palm, cuz you ain\'t the bomb!\"`*\n\r", ch->name, victim->name);
      break;
    case 12:
      sprintf(buf, "`@%s`& tosses `@%s`& a nickle. \"Buy some skills, kid.\"`*\n\r", ch->name, victim->name);
      break; 
    case 13:
      sprintf(buf, "`@%s`& hit `@%s`& so hard, `@%s\'s`& mama fell over dead.`*\n\r", ch->name, victim->name, victim->name);
      break;
    case 14:
      sprintf(buf, "`@%s`& beat `@%s`& like a stuffed Pinata(sp).`*\n\r", ch->name, victim->name);
      break;
    case 15:
      sprintf(buf, "`@%s`& squishes `@%s`&, just like grape.`*\n\r", ch->name, victim->name);
      break;
    }
  }
  techo(buf);
  
  ch->pcdata->tournament.kills += 1;
  victim->pcdata->tournament.deaths += 1;
  ch->hit = UMIN(ch->max_hit, ch->hit + (ch->max_hit / 4));
  ch->mana = UMIN(ch->max_mana, ch->mana + (ch->max_mana / 4));
  char_from_room(victim);
  char_to_room(victim, get_room_index(ROOM_VNUM_MORGUE));
  victim->pcdata->pk_timer = 0;
  /*if (vict_rep <= -25)
  {
    send_to_char("Your reputation is too low for the tournament healers to even bother with you.\n
    Get working on that reputation!\n\r",ch);
    return;
  } */
  victim->hit = victim->max_hit / 2; 
  victim->mana = UMIN(victim->mana + victim->max_mana / 2, victim->max_mana);  
  

  if (victim == ch)
  {
    tournament_gain(ch, -5);
  }

  if (vict_rep <= -25)
  {
    	sprintf(buf,"You gain no reputation from killing %s!\n\r",victim->name);
        send_to_char(buf,ch);
        return;
  }

  tournament_gain(victim,
	 URANGE(-5, ch->pcdata->tournament.rank - victim->pcdata->tournament.rank - 1, -1));

  tournament_gain(ch, (1 + victim->pcdata->tournament.rank + 
			(victim->pcdata->tournament.rank > ch->pcdata->tournament.rank ? 1 : 0)));    
     
}

void tournament_gain(CHAR_DATA *ch, int points)
{
  char buf[MSL];
  /*if ((ch->pcdata->tournament.reputation + points) <= -50)
  {
    send_to_char ("Your reputation is really low.\n You need to work on your tournament pk skills and try harder.\n\r",ch);
    ch->pcdata->tournament.reputation = -50;
    return;
  }*/
  ch->pcdata->tournament.reputation = URANGE(-32000, ch->pcdata->tournament.reputation + points, 32000);
  if (points > 0)
  {
    sprintf(buf, "`&You gained `!%d `@point%c.`*\n\r", points, points > 1 ? 's' : '\0');
  }
  else
  {
    sprintf(buf, "`&You lost `!%d `@point%c.`*\n\r", points * -1, points < -1 ? 's' : '\0');
  }  
  send_to_char(buf, ch);


  if ((ch->pcdata->tournament.rank < MAX_TOURNAMENT_RANK - 1) &&
     (ch->pcdata->tournament.reputation > tournament_ranks[ch->pcdata->tournament.rank + 1].reputation))
  {
    ch->pcdata->tournament.rank += 1;
    sprintf(buf, "Congratulations you've gone up in rank you are now a %s\n\r", 
            tournament_ranks[ch->pcdata->tournament.rank].name);
    send_to_char(buf, ch); 
    sprintf(buf, "`@%s `&has gone up in rank they now a `@%s`*\n\r",
            ch->name, 
            tournament_ranks[ch->pcdata->tournament.rank].name);
    techo(buf);
    if (ch->pcdata->tournament.last_rank != ch->pcdata->tournament.rank)
    {
      sprintf(buf, "`&You earned `@%d`& quest points.`*\n\r", 
	 tournament_ranks[ch->pcdata->tournament.rank].prize);
      send_to_char(buf, ch);
      ch->questpoints += tournament_ranks[ch->pcdata->tournament.rank].prize;
      ch->pcdata->tournament.last_rank = ch->pcdata->tournament.rank;
    }
  }

  if ((ch->pcdata->tournament.rank < MAX_TOURNAMENT_RANK) &&
       ch->pcdata->tournament.rank > 0 &&
     (ch->pcdata->tournament.reputation < tournament_ranks[ch->pcdata->tournament.rank].reputation))
  {
    ch->pcdata->tournament.rank -= 1;
    sprintf(buf, "`&L0zer you just lost your rank you are now a `@%s`*\n\r", 
            tournament_ranks[ch->pcdata->tournament.rank].name);
    send_to_char(buf, ch); 
    sprintf(buf, "`@%s`& has dropped in rank they now a `@%s`*\n\r",
            ch->name, 
            tournament_ranks[ch->pcdata->tournament.rank].name);
    techo(buf);
  }
}
