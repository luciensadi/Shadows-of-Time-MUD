/*
 * Bank Code 1.00b  Wow..  am I cool.... (ya, but I'm cooler)
 *        - Cosmo..  The Cossmic Coder  :)
 */

#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "magic.h"

void do_deposit(CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    find_money(ch);

    if (ch->pcdata->pk_timer > 0)
    {
      send_to_char("You're too excited from battle to go into the bank right now.\n\r", ch);
      return;
    }

    if (!IS_SET(ch->in_room->room_flags, ROOM_BANK))
    {
        send_to_char("You can't do that here!\n\r", ch);
        return;
    }

    if (IS_NPC(ch))
    {
        send_to_char("Mobiles can't deposit money!\n\r",ch);
        return;
    }

    if (arg1[0] == '\0' || !is_number(arg1) || atoi(arg1) <= 0 ||
    (strcasecmp(arg2,"gold") != 0 && strcasecmp(arg2,"silver") != 0) )
    {
        send_to_char("Try deposit <amount> <gold or silver>\n\r", ch);
        return;
    }

    if (strcasecmp(arg2,"gold")==0)
    {
      if (ch->gold < atoi(arg1))
         {
          send_to_char("You don't HAVE that much gold..\n\r", ch);
          return;
         }

      if ((ch->gold_in_bank + atoi(arg1)) > 50000)
         {
          send_to_char("Can't put more than 50k gold in the bank.\n\r", ch);
          return;
         }
      ch->gold -= atoi(arg1);
      ch->gold_in_bank += atoi(arg1);
      sprintf(buf, "Your balance is %ld gold and %ld silver.\n\r",
      ch->gold_in_bank, ch->silver_in_bank);
      send_to_char(buf, ch);
    }

    if (strcasecmp(arg2,"silver")==0)
    {
      if (ch->silver < atoi(arg1))
         {
          send_to_char("You don't HAVE that much silver.\n\r", ch);
          return;
         }

      if ((ch->silver_in_bank + atoi(arg1)) > 1000000)
         {
          send_to_char("Can't put more than 1 mil silver in the bank.\n\r", ch);
          return;
         }
      ch->silver -= atoi(arg1);
      ch->silver_in_bank += atoi(arg1);
      sprintf(buf, "Your balance is %ld gold and %ld silver.\n\r",
      ch->gold_in_bank, ch->silver_in_bank);
      send_to_char(buf, ch);
    }
  return;
}

void find_money(CHAR_DATA *ch)
{
    sh_int j;

    if ((ch->last_bank - current_time) > 86400)
    {
        for (j = ( ch->last_bank - current_time ) / 86400; j != 0 ; j--)
        {
            ch->gold_in_bank += int(ch->gold_in_bank * .02);
            ch->silver_in_bank += int(ch->silver_in_bank * .02);
        }
      ch->last_bank = time(NULL);
    }

    if ((ch->gold_in_bank > 50000) || ch->gold_in_bank < 0)
        ch->gold_in_bank = 50000;

    if ((ch->silver_in_bank > 1000000) || ch->gold_in_bank < 0)
        ch->silver_in_bank = 1000000;

}

void do_balance(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];

    if (!IS_SET(ch->in_room->room_flags, ROOM_BANK))
    {
        send_to_char("You can't do that here\n\r", ch);
        return;
    }
    find_money(ch);
    sprintf(buf, "You have %ld gold and %ld silver pieces in the bank.\n\r",
    ch->gold_in_bank, ch->silver_in_bank);
    send_to_char(buf, ch);
}

void do_withdraw(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
    {
        send_to_char("NPC's can't get money..  dumbass!!!!\n\r", ch);
        return;
    }
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!IS_SET(ch->in_room->room_flags, ROOM_BANK))
    {
        send_to_char("You can't do that here", ch);
        return;
    }
    find_money(ch);

    if (arg1[0] == '\0' || !is_number(arg1) || atoi(arg1) <= 0 ||
    (strcasecmp(arg2,"gold") != 0 && strcasecmp(arg2,"silver") != 0 ))
    {
        send_to_char("Try withdraw <amount> <gold or silver>\n\r", ch);
        return;
    }

    if (strcasecmp(arg2,"gold")==0)
    {
      if (ch->gold_in_bank < atoi(arg1))
        {
          send_to_char("You don't have that much gold in the bank!\n\r", ch);
          return;
        }
      if ( get_carry_weight(ch) + gold_weight(atoi(arg1)) > can_carry_w( ch ) )
      {
          act( "gold: you can't carry that much weight.",
            ch, NULL, NULL, TO_CHAR );
          return;
      }
      ch->gold_in_bank -= atoi(arg1);
      ch->gold += atoi(arg1);
      find_money(ch);
      sprintf(buf, "You now have %ld gold and %ld silver in the bank.\n\r",
      ch->gold_in_bank, ch->silver_in_bank);
      send_to_char(buf, ch);
    }

    if (strcasecmp(arg2,"silver")==0)
    {
      if (ch->silver_in_bank < atoi(arg1))
        {
          send_to_char("You don't have that much silver in the bank!\n\r", ch);
          return;
        }
      if ( get_carry_weight(ch) + silver_weight(atoi(arg1)) > can_carry_w( ch ) )
      {
          act( "silver: you can't carry that much weight.",
            ch, NULL, NULL, TO_CHAR );
          return;
      }
      ch->silver_in_bank -= atoi(arg1);
      ch->silver += atoi(arg1);
      find_money(ch);
      sprintf(buf, "You now have %ld gold and %ld silver in the bank.\n\r",
      ch->gold_in_bank, ch->silver_in_bank);
      send_to_char(buf, ch);
    }

  return;
}

