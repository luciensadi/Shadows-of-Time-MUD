/* Rom Account System mostly called from comm.cpp */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "telnet.h"
#include "account.h"
#include "interp.h"

const char echo_off_str[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const char echo_on_str[] = { IAC, WONT, TELOPT_ECHO, '\0' };

bool check_reconnect args ((DESCRIPTOR_DATA * d, char *name, bool fConn));
bool check_playing args ((DESCRIPTOR_DATA * d, char *name));
bool load_account_obj args (( DESCRIPTOR_DATA *d, char *name));

void get_account_name(DESCRIPTOR_DATA *d, char *argument)
{
      bool fOld;
      if (strlen(argument) < 1)
      {
         send_to_desc("That is not a valid option.\n\r", d);
         return;
      }
      fOld = load_account_obj (d, argument);
      if (!fOld)
      {
         char buf[MSL];
         sprintf (buf, "Did I get that right, %s (Y/N)? ", argument);
         send_to_desc (buf, d);
         d->connected = CON_CONFIRM_NEW_ACCOUNT;
         return;
      }
      
      send_to_desc("Password: ", d);
      send_to_desc(echo_off_str, d);
      d->connected = CON_GET_ACCOUNT_PASSWORD;
}

void confirm_new_account(DESCRIPTOR_DATA *d, char *argument)
{
  char buf[MSL];
  switch (*argument)
  {
    case 'y':
    case 'Y':
       sprintf (buf,
              "New account.\n\rGive me a password for %s: %s",
               d->account->name, echo_off_str);
       send_to_desc (buf, d); 
       d->connected = CON_GET_NEW_ACCOUNT_PASSWORD;
       break;
               
    case 'n':
    case 'N':
       sprintf(buf, "What would you like the account name to be then:");
       send_to_desc(buf, d);
       d->connected = CON_GET_ACCOUNT_NAME;
       break;
  }
}

void get_new_account_password(DESCRIPTOR_DATA *d, char *argument)
{
  char *pwdnew;
  char *p;
            if (strlen (argument) < 5)
            {
                send_to_desc
                    ("Password must be at least five characters long.\n\rPassword:\n\r",
                      d);
                return;
            }
                    
            pwdnew = crypt (argument, d->account->name);
            for (p = pwdnew; *p != '\0'; p++)
            {
                if (*p == '~')
                {
                    send_to_desc
                        ("New password not acceptable, try again.\n\rPassword: ",
                        d);
                    return;
                }
            }

            free_string (d->account->password);
            d->account->password = str_dup (pwdnew);
            send_to_desc ("Please retype password: ", d);
            d->connected = CON_CONFIRM_NEW_ACCOUNT_PASSWORD;
            return;
}

void confirm_new_account_password(DESCRIPTOR_DATA *d, char *argument)
{
   if (strcmp (crypt (argument, d->account->password), d->account->password))
   {
     send_to_desc ("Passwords don't match.\n\rRetype password: ",
                    d);
     d->connected = CON_GET_NEW_ACCOUNT_PASSWORD;
     return;
   }
   d->connected = CON_ACCOUNT_ACTION;       
   send_to_desc (echo_on_str, d); 
   show_account(d, argument);
}

void get_account_password(DESCRIPTOR_DATA *d, char *argument)
{
   if (strcmp (crypt (argument, d->account->password), d->account->password))
   {
     send_to_desc ("Wrong password.",
                    d);
     close_socket(d);
     return;
   }
   send_to_desc(echo_on_str, d); 
   show_account(d, argument);
}

void show_account(DESCRIPTOR_DATA *d, char *argument)
{
  char buf[MSL];
  ACCT_PLR_DATA *list;
  bool found = FALSE;

  sprintf(buf, "\n\r`$Account name: `&%s`*\n\r", d->account->name); 
  send_to_desc(buf, d);
  if (!IS_NULLSTR(d->account->host))
  {
    sprintf(buf, "`$Last host logged from: `&%s`*\n\r", d->account->host);
    send_to_desc(buf, d); 
  }
  send_to_desc("`&Players:\n\r", d);
  if (d->account->player)
    send_to_desc("`$Prim     Name           Password`*\n\r", d);
  for (list = d->account->player;list != NULL; list = list->next)
  {
    found = TRUE;
    sprintf(buf, "  `!%s    `&%-10s          %s`*\n\r", 
                 list->primary == 1 ? "*" : " ", 
                 list->name,
                 list->password == 1 ? "On" : "Off");
    send_to_desc(buf, d);
  } 

  if (!found)
    send_to_desc("No players found on this account.\n\r", d);
  send_to_desc("`*Type help commands for a list of possible acctions.\n\r", d);
  send_to_desc("`&Next Action: `*", d);
  d->connected = CON_ACCOUNT_ACTION;
  return;
}

void get_account_action(DESCRIPTOR_DATA *d, char *argument)
{
//  logf("get_account_action", 0);
  if (!str_cmp(argument, "exit"))
  {
    send_to_desc("Good-bye\n\r", d);
    close_socket(d);
  }
  else
  {
    char arg[MIL];
    argument = one_argument(argument, arg);
    if (!str_cmp(arg, "login"))
    {
      ACCT_PLR_DATA *list;
      bool found = FALSE;
      for (list = d->account->player; list != NULL; list = list->next)
      {
         if (!str_prefix(argument, list->name))
         {
           found = TRUE;
           break;
          }
       }

       if (!found)
       {
         send_to_desc("No such character found under your account.\n\r", d);
         show_account(d, argument);
         return;
       } 
       
       if (load_char_obj(d, list->name))
       {
         save_account(d->account, dns_gethostname(d->host));
         free_string(d->character->pcdata->account);
         d->character->pcdata->account = str_dup(d->account->name);
         save_char_obj(d->character);
         if (IS_SET (d->character->act, PLR_DENY))
         {
              sprintf (log_buf, "Denying access to %s@%s.", argument,
                       dns_gethostname(d->host));
              log_string (log_buf);
              send_to_desc ("You are denied access.\n\r", d);
              return;
         }
         check_reconnect(d, list->name, FALSE);
           

         if (list->password)
         {
           send_to_desc("Password: ", d);
           if (!list->primary)
             d->character->pcdata->primary = FALSE;
           d->connected = CON_GET_OLD_PASSWORD;
           return;
         }
         else
         {
            char buf[MIL];
            if (check_reconnect(d, list->name, TRUE))
              return;

            if (check_playing(d, list->name))
              return;
            mudstats.current_players += 1;
            mudstats.current_logins += 1;
            mudstats.total_logins +=1;
            if (mudstats.current_players > mudstats.current_max_players)
            {
               mudstats.current_max_players = mudstats.current_players;
               if (mudstats.total_max_players < mudstats.current_max_players)
                 mudstats.total_max_players = mudstats.current_max_players;
            }   
                
                
            sprintf (log_buf, "%s@%s has connected.", d->character->name, 
                              dns_gethostname(d->host));
            log_string (log_buf);
                
            if (IS_SET(d->character->act2, PLR_NOIP))
              sprintf (log_buf, "%s has connected.", d->character->name);
            wiznet (log_buf, NULL, NULL, WIZ_SITES, 0, get_trust (d->character));
             d->character->pcdata->Host = str_dup(dns_gethostname(d->host));
           if (!list->primary)
             d->character->pcdata->primary = FALSE;
            if (IS_IMMORTAL (d->character))
            {

                sprintf( buf, "\n\r`!Welcome `@%s`7@`8%s`7!\n\r", 
                         d->character->name, dns_gethostname(d->host));
                send_to_char( buf, d->character );
                sprintf( buf, "\n\r`&Please be advised `@%s`& that upon enterance, you may be logged`7\n\r", d->character->name);
                send_to_char( buf, d->character );
                do_function (d->character, &do_mhelp, "* imotd");
                d->connected = CON_READ_IMOTD;
            }
            else
            {
                sprintf( buf, "\n\rWelcome %s@%s!\n\r", d->character->name, 
                        dns_gethostname(d->host));
                send_to_char( buf, d->character );

                do_function (d->character, &do_mhelp, "* motd");
                d->connected = CON_READ_MOTD;
            }
            if (voteinfo.on)
            {
              send_to_char("\n\r`&There is currently a vote running.  Type`*\n\r", d->character);
              send_to_char("`&vote show to see it or vote for syntax.`*\n\r", d->character);
            }
            return;
         }
       }
       else
       {
         send_to_desc("Can't find that player file.\n\r", d);
       }
     }

     if (!str_cmp(arg, "save"))
     {
       save_account(d->account, dns_gethostname(d->host));
       send_to_desc("Saved\n\r", d);
       send_to_desc("Next Action: ", d);
       d->connected = CON_ACCOUNT_ACTION;
     }

     if (!str_cmp(arg, "attach"))
     {
       char buf[MSL];
       bool fOld = FALSE;

       fOld = load_char_obj(d, argument);
       if (!fOld)
       {
         send_to_desc("No such player found.\n\r", d);
         show_account(d, argument);
         return;
       }

       if (!IS_NULLSTR(d->character->pcdata->account) && 
             str_cmp(d->character->pcdata->account, d->account->name))
       {
         send_to_desc("That character is already attached to another account.\n\r", d);
         show_account(d, argument);
         return;
       }
      
       sprintf(buf, "Enter %s's password: %s", d->character->name,
                    echo_off_str);
       send_to_desc(buf, d);
       d->connected = CON_CHECK_ATTACH_PASSWORD;
       return;
     }

     if (!str_cmp(arg, "primary"))
     {
        ACCT_PLR_DATA *list;
        ACCT_PLR_DATA *primary = NULL, *newprim = NULL;
        bool found = FALSE;
        for (list = d->account->player; list != NULL; list = list->next)
        { 
           if (!str_prefix(argument, list->name))
           {
             found = TRUE;
             newprim = list;
           }
           if (list->primary == 1)
             primary = list;
         }
       
         if (!found)
         {
           send_to_desc("No such character found under your account.\n\r", d);
           send_to_desc("Next Action: ", d);
           d->connected = CON_ACCOUNT_ACTION;
           return;
         }
         if (primary)
           primary->primary = 0;
         newprim->primary = 1;
         send_to_desc("Primay set\n\r", d);
         show_account(d, argument);
     }

     if (!str_cmp(arg, "delete"))
     {
        bool found = FALSE;
        ACCT_PLR_DATA *player = NULL;   

        
        if (d->account->player && !str_prefix(argument, d->account->player->name))
        {
           player = d->account->player;
           d->account->player = d->account->player->next;
           send_to_desc("Successfully deleted.\n\r", d);
           found = TRUE;
        }
        else
        {
          ACCT_PLR_DATA *list;
          for (list = d->account->player; list != NULL; list = list->next)
          { 
             if (!list->next)
               break;
             if (!str_prefix(argument, list->next->name))
             {
               player = list->next;
               list->next = list->next->next;
               found = TRUE;   
               send_to_desc("Successfully deleted.\n\r", d);
               break;
             }
          }
        }

        if (found)
        {
          load_char_obj(d, player->name);
          free_string(d->character->pcdata->account);
          d->character->pcdata->account = str_dup("");
          save_char_obj(d->character);
          free_acct_plr(player);
        }

        if (!found)
        {
          send_to_desc("No such character found under your account.\n\r", d);
          send_to_desc("Next Action: ", d);
          d->connected = CON_ACCOUNT_ACTION;
          return;
        }
        save_account(d->account, dns_gethostname(d->host));
     }

     if (!str_cmp(arg, "acctpass"))
     {
        char arg2[MIL];
        char *p, *pwdnew;
        
        argument = one_argument(argument, arg2);
        if (strcmp (crypt (arg2, d->account->password), d->account->password))
        {
          send_to_desc ("Wrong password.\n\r", d);
          show_account(d, argument);
          return;
        }
         
        if (strlen (argument) < 5)
        {
           send_to_desc
              ("Password must be at least five characters long.\n\r", d);
           show_account(d, argument);
           return;
        }

        pwdnew = crypt (argument, d->account->name);
        for (p = pwdnew; *p != '\0'; p++)
        {
           if (*p == '~')
           {
              send_to_desc
                ("New password not acceptable, try again.\n\r", d);
              show_account(d, argument);
              return;
           }
        }

        free_string (d->account->password);
        d->account->password = str_dup (pwdnew);
        send_to_desc("Password changed successfully.\n\r", d);
        show_account(d, argument);
     }    

     if (!str_cmp(arg, "password"))
     {
        ACCT_PLR_DATA *list;
        char arg2[MIL];
        bool found = FALSE;
        
        argument = one_argument(argument, arg2);
        for (list = d->account->player; list != NULL; list = list->next)
        { 
           if (!str_prefix(arg2, list->name))
           {
             found = TRUE;
             break;
           }
         }
       
         if (!found)
         {
           send_to_desc("No such character found under your account.\n\r", d);
           send_to_desc("Next Action: ", d);
           d->connected = CON_ACCOUNT_ACTION;
           return;
         }

         load_char_obj(d, list->name);

         if (strcmp (crypt (argument, d->character->pcdata->pwd), 
              d->character->pcdata->pwd))
         {   
            send_to_desc ("Wrong password.\n\r", d);
            show_account(d, argument);
            return;
         }

         if (list->password == 0)
         {
           send_to_desc("Password toggled on.\n\r", d);
           send_to_desc("Next Action: ", d);
           list->password = 1;
         }
         else
         {
           send_to_desc("Password toggled off.\n\r", d);
           send_to_desc("Next Action: ", d);
           list->password = 0;
         }
         save_account(d->account, dns_gethostname(d->host));
         show_account(d, argument);
         return;
      }

      if (!str_cmp(arg, "show"))
      {
        show_account(d, argument);
        return;
      }

      if (!str_cmp(arg, "help"))
      {
        account_help(d, argument);
        return;
      }

      send_to_desc("Not a valid action.  Next Action:", d);
      show_account(d, "");
      return;
   }

   
}

void account_help(DESCRIPTOR_DATA *d, char *argument)
{
  if (!str_prefix(argument, "commands"))
  {
    send_to_desc("`$Possible commands are: `*\n\r", d);
    send_to_desc("  attach <name>          - Attach a character to account\n\r", d);
    send_to_desc("  delete                 - Delete a player from account.\n\r", d);
    send_to_desc("  exit                   - Log off the account system.\n\r", d);
    send_to_desc("  login <name>           - Log into player <name>\n\r", d);
    send_to_desc("  password <name> <pass> - Toggle character level password\n\r", d);
    send_to_desc("  primary <name>         - Set primary character (Used for notes)\n\r", d);
    send_to_desc("  acctpass <old> <new>   - Change account login password\n\r", d);
    show_account(d, argument);
    return;
  }
  send_to_desc("No help on that word.\n\r", d);
  show_account(d, argument);
  return;
}

void check_attach_password(DESCRIPTOR_DATA *d, char *argument)
{
   ACCT_PLR_DATA *player;
   if (strcmp (crypt (argument, d->character->pcdata->pwd), 
              d->character->pcdata->pwd))
   {   
      send_to_desc ("Wrong password.\n\r", d);
      close_socket (d);
      return;
   }
   write_to_buffer (d, echo_on_str, 0);
   player = new_acct_plr();
   player->name = str_dup(d->character->name);
   player->password = 1;
   player->primary = 0;
   if (!d->account->player)
     player->primary = 1;
   player->next = d->account->player;
   d->account->player = player;
   save_account(d->account, dns_gethostname(d->host));
   free_string(d->character->pcdata->account);
   d->character->pcdata->account = str_dup(d->account->name);
   save_char_obj(d->character);
   show_account(d, argument);
}

void save_account(ACCOUNT_DATA *account, char *host)
{
  FILE *fp;
  char strsave[MIL];
  ACCT_PLR_DATA *list;

  sprintf(strsave, "%s%s", ACCOUNT_DIR, capitalize (account->name));
  if ((fp = fopen (strsave, "w")) == NULL)
  {
    return;
  }
  fprintf(fp, "#Account 2\n");
  fprintf(fp, "AccountName %s~\n", capitalize(account->name));
  fprintf(fp, "Password %s~\n", account->password);
  fprintf(fp, "Host %s~\n", host);
  for (list = account->player; list != NULL; list = list->next)
  {
    fprintf(fp, "#PLAYER\n");
    fprintf(fp, "%s~\n", list->name);
    fprintf(fp, "%d\n", list->password);
    fprintf(fp, "%d\n", list->primary);
  }
  fprintf(fp, "#End\n");
  fclose(fp);
}
