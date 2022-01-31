#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list.h>
#include "merc.h"
#include "dice.h"

list<Crowns> CrownsList;

const char* dot_table[3][6] = {
{ "`!|     |`7", "`!|`&*`!    |`7", "`!|`&*`!    |`7", "`!|`&*   *`!|`7", "`!|`&*   *`!|`7", "`!|`&*   *`!|`7" },
{ "`!|  `&*`!  |`7", "`!|     |`7", "`!|  `&*  `!|`7", "`!|     |`7", "`!|  `&*`!  |`7", "`!|`&*   *`!|`7" },
{ "`!|     |`7", "`!|    `&*`!|`7", "`!|    `&*`!|`7", "`!|`&*   *`!|`7", "`!|`&*   *`!|`7", "`!|`&*   *`!|`7" },
};

const char* crown_table[3][6] = {
{ "`!| `&*** `!|`7", "`!| `&_*_ `!|`7", "`!|`&*****`!|`7", "`!|`&**  **`!|`7", "`!| `&***`! |`7", "`!|`&* * *`!|`7" },
{ "`!|  `&*`!  |`7", "`!| `&\\+/ `!|`7", "`!|`&*+++*`!|`7", "`!|`&**++**`!|`7", "`!| `&+*+ `!|`7", "`!|`&*****`!|`7" },
{ "`!| `&*** `!|`7", "`!| `&|+| `!|`7", "`!| `&***`! |`7", "`!|`&**++**`!|`7", "`!|  `&*`!  |`7", "`!|`&*****`!|`7" },
};

void Die::roll (void)
{
  value = number_range(1, 6);
}

void Crowns::roll(void)
{
  int i;
  for (i = 0;i < numDice;i++)
    dice[i].roll();
}

int Crowns::get_value()
{
  int i, sum = 0;
  for (i= 0;i < numDice;i++)
    sum += dice[i].get_value();
  return sum;
}

void Crowns::show_value(CHAR_DATA *ch)
{
  char buf[MSL];
  int i, j;

  sprintf(buf, "You rolled %d dice.\n\r\n\r", numDice);
  send_to_char(buf, ch);

  for (i = 0; i < 3; i++)
  {
    for (j = 0;j < numDice;j++)
    {
      if (type == 0)
        sprintf(buf, "%s  ", dot_table[i][dice[j].get_value() - 1]);
      else
        sprintf(buf, "%s  ", crown_table[i][dice[j].get_value() - 1]);
      send_to_char(buf, ch);
    }
    send_to_char("\n\r", ch);
  }

  sprintf(buf, "You rolled a total of `&%d`7\n\r",
               get_value());
  send_to_char(buf, ch);
  sprintf(buf, "`&%s rolled a total of %d`7\n\r",
               ch->name, get_value());
  cecho(buf, ch->name);

}


void Crowns::play(CHAR_DATA *ch)
{
  char buf2[MSL];
  bool remove = FALSE;
  Player *player;

  if ((player = get_player(ch->name)) == NULL)
  {
    send_to_char("Couldn't find player.\n\r", ch);
    return;
  }

  if (str_cmp(player->get_name(), turn->get_name()))
  {
    send_to_char("It's not your turn to roll.\n\r", ch);
    return;
  }

  if (get_potType() == 0)
  {
    if (ch->questpoints < get_bet())
    {
      send_to_char("You don't have enough questpoints to play with.\n\r",
                    ch);
      remove = TRUE;
    }
    else
    {
      char buf[MSL];
      ch->questpoints -= get_bet();
      sprintf(buf, "You ante up %d quest points.\n\r", get_bet());
      send_to_char(buf, ch);
      pot += get_bet();
      roll();
      show_value(ch);
      player->set_score(get_value());
    }
  }
  else
  {
    if ((ch->silver / 100 + ch->gold) < get_bet())
    {
      send_to_char("You don't have enough gold and silver to play with.\n\r",
                    ch);
      remove = TRUE;
    }
    else
    {
      char buf[MSL];
      deduct_cost(ch, (get_bet() * 100));
      sprintf(buf, "You ante up %d gold.\n\r", get_bet());
      send_to_char(buf, ch);
      pot += get_bet();
      roll();
      show_value(ch);
      player->set_score(get_value());
    }
  }

  list<Player>::iterator i;
  for (i = players.begin();i != players.end();i++)
  {
    if (!str_cmp(i->get_name(), player->get_name()))
    {
      if ((++i) == players.end())
      {
        turn = &(*players.begin());
      }
      else
        turn = &(*i);
      break;
    }
  }

  if (remove)
    remove_player(player);
  if (turn == &(*players.begin()))
    check_win();

  sprintf(buf2, "It's %s's turn to roll!\n\r", turn->get_name());
  cecho(buf2);
  set_timer(3);
}

void Crowns::check_win(void)
{
  int highest = 0;
  int tie = -1;
  Player *winner = startedBy;
  char buf[MSL];

  list<Player>::iterator i;
  sprintf(buf, "\n\r\n\r`&Game Results:`*\n\r");
  cecho(buf);
  for (i = players.begin();i != players.end();i++)
  {
    sprintf(buf, "Player %s - Score `&%d`7\n\r", i->get_name(),
                i->get_score());
    cecho(buf);

    if (i->get_score() > highest)
    {
      highest = i->get_score();
      winner = &(*i);
    }
  }

  for (i = players.begin();i != players.end();i++)
  {
    if (i->get_score() == highest)
      tie++;
  }



  if (tie == 0)
  {
    sprintf(buf, "`&The winner is %s with the high score of `!%d`7\n\r",
                winner->get_name(), winner->get_score());
    cecho(buf);
    {
      CHAR_DATA *ch;
      if ((ch = get_player_world_special(winner->get_name())) != NULL)
      {
        if (get_potType() == 0)
        {
          sprintf(buf, "You win %d quest points!\n\r", get_pot());
          ch->questpoints += get_pot();
        }
        else
        {
          sprintf(buf, "You win %d gold!\n\r", get_pot());
          ch->gold += get_pot();
        }
        send_to_char(buf, ch);
        set_pot(0);
      }
    }
  }
  else
  {
    int iAnd = tie;
    char buf2[MIL];
    sprintf(buf, "There has been a tie between ");
    for (i = players.begin();i != players.end();i++)
    {
      if (i->get_score() == highest)
      {
        if (iAnd-- == 0)
          strcat(buf, "and ");
        sprintf(buf2, "%s ", i->get_name());
        strcat(buf, buf2);
      }
    }
    if (get_potType() == 0)
      sprintf(buf2, "each will win %d questpoints.\n\r", get_pot() / (tie + 1));
    else
      sprintf(buf2, "each will win %d gold.\n\r", get_pot() / (tie + 1));
    strcat(buf, buf2);
    cecho(buf);

    for (i = players.begin();i != players.end();i++)
    {
      CHAR_DATA *ch;
      if (i->get_score() == highest)
      {
        if ((ch = get_player_world_special(i->get_name())) != NULL)
        {
          if (get_potType() == 0)
          {
            sprintf(buf, "You win %d quest points!\n\r", get_pot() / (tie + 1));
            ch->questpoints += get_pot() / (tie + 1);
          }
	  else
          {
            sprintf(buf, "You win %d gold!\n\r", get_pot() / (tie + 1));
            ch->gold += get_pot() / (tie + 1);
          }

          send_to_char(buf, ch);
        }
      }
    }
    set_pot(0);
  }
}

void Crowns::cecho(char *buf)
{
  CHAR_DATA *wch;
  list<Player>::iterator i;
  for (i = players.begin();i != players.end();i++)
  {
     if ((wch = get_player_world_special(i->get_name())) != NULL)
       send_to_char(buf, wch);
  }
}

void Crowns::cecho(char *buf, char *exclude)
{
  CHAR_DATA *wch;
  list<Player>::iterator i;
  for (i = players.begin();i != players.end();i++)
  {
     if (!str_cmp(i->get_name(), exclude))
       continue;
     if ((wch = get_player_world_special(i->get_name())) != NULL)
       send_to_char(buf, wch);
  }
}

void Crowns::new_player(char *iName)
{
  Player *tmpPlayer;
  char buf[MSL];
  tmpPlayer = new Player(iName);
  tmpPlayer->set_score(0);
  players.push_back(*tmpPlayer);
  sprintf(buf, "%s has joined the game.\n\r", tmpPlayer->get_name());
  cecho(buf);
}

Player *Crowns::get_player(char *name)
{
  list<Player>::iterator i;
  for (i=players.begin(); i != players.end(); ++i)
  {
    if (!str_cmp(i->get_name(), name))
      return &(*i);
  }
  return NULL;
}

// Constructor
Crowns::Crowns(int dice, char *name)
{
  numDice = dice;
  startedBy = NULL;
  new_player(name);
  startedBy = new Player(get_player(name));
  turn = startedBy;
  type = 0;
  timer = 3;
  pot = 0;
  potType = 0;
  bet = 0;
}

Crowns *find_game(char *name)
{
  list<Crowns>::iterator i;

  for (i=CrownsList.begin(); i != CrownsList.end(); ++i)
  {
    if (i->get_player(name) != NULL)
    {
      return &(*i);
    }
  }
  return NULL;
}

Crowns *find_new_game(char *name)
{
  list<Crowns>::iterator i;

  for (i=CrownsList.begin(); i != CrownsList.end(); ++i)
  {
    if (!str_cmp(name, i->get_starter()->get_name()))
    {
      return &(*i);
    }
  }
  return NULL;
}


void Crowns::status(CHAR_DATA *ch)
{
  char buf[MSL];
  sprintf(buf, "Started by: %s\n\r",
                  get_starter()->get_name());
  send_to_char(buf, ch);

  sprintf(buf, "Turn: %s\n\r\n\r",
                get_turn()->get_name());
  send_to_char(buf, ch);
  send_to_char("Players:\n\r", ch);
  list<Player>::iterator i;
  for (i = players.begin();i != players.end();i++)
  {
    sprintf(buf, "%s\n\r", i->get_name());
    send_to_char(buf, ch);
  }
}

void Crowns::remove_player(Player *player)
{
  list<Player>::iterator i, i_next;
  Player *tmpPlayer;
  char buf[MSL];
  bool change_turn = FALSE;

  for (i = players.begin();i != players.end();i = i_next)
  {
    i_next = i;
    i_next++;
    if (!str_cmp(i->get_name(), player->get_name()))
    {
      if (turn == &(*i))
      {
        if (i_next == players.end())
          turn = &(*players.begin());
        else
          turn = &(*i_next);
        change_turn = TRUE;
        set_timer(3);
      }
      sprintf(buf, "%s has left the game.\n\r", i->get_name());
      i->free_player();
      tmpPlayer = &(*i);
      players.erase(i);
// Done by STL delete tmpPlayer;

      cecho(buf);
      if (change_turn)
      {
        sprintf(buf, "It's %s's turn to roll!\n\r", turn->get_name());
        cecho(buf);
      }
    }
  }
}

void Crowns::clear_players()
{
  list<Player>::iterator i, i_next;
  Player *tmpPlayer;
  for (i = players.begin();i != players.end();i = i_next)
  {
    i_next = i;
    i_next++;
    i->free_player();
    tmpPlayer = &(*i);
    players.erase(i);
// Done by STL delete tmpPlayer;
  }
}

void remove_game(Crowns *game)
{
  list<Crowns>::iterator i, i_next;
  Crowns *tmpCrowns;
  for (i = CrownsList.begin();i != CrownsList.end();i = i_next)
  {
    i_next = i;
    i_next++;
    if (&(*i) == game)
    {
      i->clear_players();
      tmpCrowns = &(*i);
      CrownsList.erase(i);
// Done by STL delete tmpCrowns;
    }
  }
}

void check_all_games()
{
  list<Crowns>::iterator i;
  list<Crowns>::iterator i_next;
  for (i = CrownsList.begin(); i != CrownsList.end();i = i_next)
  {
    i_next = i;
    i_next++;
    if (i->check_game())
      CrownsList.remove(*i);
  }
}

bool Crowns::check_game(void)
{
  list<Player>::iterator i;
  list<Player>::iterator i_next;
  int num = 0;

  tick_timer();
  if (get_timer() < 1)
  {
    char buf[MSL];
    sprintf(buf, "Timer ran out %s is being booted from the game.\n\r",
            turn->get_name());
    cecho(buf);
    remove_player(turn);
  }
  for (i = players.begin(); i != players.end(); i = i_next)
  {
    i_next = i;
    i_next++;
    if (get_player_world_special(i->get_name()))
    {
   //   logf("Found one");
      num++;
    }
    else
    {
      players.erase(i);
    }
  }
  if (num == 0)
  {
    return TRUE;
  }
  return FALSE;
}

void do_crowns (CHAR_DATA *ch, char *argument)
{
//  char buf[MSL];
  char arg[MSL];

  argument = one_argument(argument, arg);
  if (!str_cmp(arg, "roll"))
  {
    Crowns *game;
    if ((game = find_game(ch->name)) == NULL)
    {
      send_to_char("You aren't currently in a game of crowns.\n\r", ch);
      return;
    }
    game->play(ch);
    return;
  }

  if (!str_cmp(arg, "join"))
  {
    if (find_game(ch->name))
    {
      send_to_char("You are already in a game.\n\r", ch);
      return;
    }

    Crowns *game;
    if ((game = find_new_game(argument)) == NULL)
    {
      send_to_char("That game doesn't exist.\n\r", ch);
      return;
    }

    game->new_player(ch->name);

    send_to_char("You joined the game.\n\r", ch);
    return;
  }

  if (!str_cmp(arg, "start"))
  {
    char arg2[MSL];
    char arg3[MSL];
    int bet;

    argument = one_argument(argument, arg2);
    if (find_game(ch->name))
    {
      send_to_char("You are already in a game.\n\r", ch);
      return;
    }

    Crowns game(5, ch->name);
    if (arg2[0] == '\0' || argument[0] == '\0')
    {
      send_to_char("Do dice start <type> <bet>\n\r", ch);
      return;
    }

    if (!str_prefix(arg2, "crowns"))
      game.set_type(1);
    else
      game.set_type(0);

    argument = one_argument(argument, arg3);
    if (!is_number(arg3))
    {
      send_to_char("That's not a valid bet.\n\r", ch);
      return;
    }
    bet = atoi(arg3);

    if (bet < 1)
    {
      send_to_char("That's not a valid bet.\n\r", ch);
      return;
    }
    game.set_bet(bet);
    if (!str_cmp(argument, "gold"))
      game.set_potType(1);
    game.set_timer(5);
    CrownsList.push_back(game);
    send_to_char("You started a new game, it's your turn to roll.\n\r",
		  ch);
    return;
  }

  if (!str_cmp(arg, "list"))
  {
    list<Crowns>::iterator i;
    int num = 0;
    char buf[MSL];

    send_to_char("Current games:\n\r", ch);
    for (i=CrownsList.begin(); i != CrownsList.end(); i++)
    {
      num++;
      sprintf(buf, "%2d - %s - %d %s\n\r", num,
                   i->get_starter()->get_name(), i->get_bet(),
                   i->get_potType() ? "gold" : "qp");
      send_to_char(buf, ch);
    }
    return;
  }

  if (!str_cmp(arg, "status"))
  {
    Crowns *game;
    if ((game = find_game(ch->name)) == NULL)
    {
      send_to_char("You aren't in a game.\n\r", ch);
      return;
    }
    game->status(ch);
    return;
  }

  if (!str_cmp(arg, "talk"))
  {
    Crowns *game;
    char buf[MSL];
    if ((game = find_game(ch->name)) == NULL)
    {
      send_to_char("You aren't in a game.\n\r", ch);
      return;
    }
    sprintf(buf, "You dice talk '`&%s`*'\n\r", argument);
    send_to_char(buf, ch);
    sprintf(buf, "%s dice talks '`&%s`*'\n\r", ch->name, argument);
    game->cecho(buf, ch->name);
    return;
  }

  if (!str_cmp(arg, "leave"))
  {
    Crowns *game;
    Player *player;
    if ((game = find_game(ch->name)) == NULL)
    {
      send_to_char("You aren't in a game.\n\r", ch);
      return;
    }

    if ((player = game->get_player(ch->name)) == NULL)
    {
      send_to_char("Find_player: Report to imm.\n\r", ch);
      return;
    }

    game->remove_player(player);
    if (game->check_game())
      remove_game(game);
    send_to_char("Removed from game.\n\r", ch);
    return;
  }

  send_to_char("Type dice join, leave, list, roll, start, status or talk\n\r", ch);
  return;
}

bool Player::operator==(const Player &player2)
{
  if (!str_cmp(this->name, player2.name))
    return TRUE;
  return FALSE;
}

bool Crowns::operator==(const Crowns &game2)
{
  if (this->startedBy == game2.startedBy)
    return TRUE;
  return FALSE;
}


Player::Player(const Player *obj)
{
  name = str_dup(obj->name);
  score = obj->score;
}
