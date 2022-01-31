/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
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
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "web.h"
#include "lookup.h"
#include "mysql.h"

/* globals from db.c for load_notes */
#if !defined(macintosh)
extern int _filbuf args ((FILE *));
#endif
extern FILE *fpArea;
extern char strArea[MAX_INPUT_LENGTH];

/* local procedures */
void load_thread (char *name, NOTE_DATA ** list, int type, time_t free_time);
bool have_store args ((CHAR_DATA *ch));
void parse_note (CHAR_DATA * ch, char *argument, int type);
bool hide_note (CHAR_DATA * ch, NOTE_DATA * pnote);
void note_echo (NOTE_DATA *pnote, CHAR_DATA *ch);
char *fontnote (char *string, CHAR_DATA *ch, int type);
char *note_name(int type);
NOTE_DATA *imm_list;
NOTE_DATA *rp_list;
NOTE_DATA *note_list;
NOTE_DATA *idea_list;
NOTE_DATA *guildmaster_list;
NOTE_DATA *news_list;
NOTE_DATA *changes_list;
NOTE_DATA *guide_list;
NOTE_DATA *storenote_list;

int count_spool (CHAR_DATA * ch, NOTE_DATA * spool)
{
    int count = 0;
    NOTE_DATA *pnote;

    for (pnote = spool; pnote != NULL; pnote = pnote->next)
        if (!hide_note (ch, pnote))
            count++;

    return count;
}

void do_unread (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int count;
    bool found = FALSE;

    if (IS_NPC (ch))
        return;

    if (!str_cmp(argument, "catchup"))
    {
      int i;
      for (i = 0; i < NOTE_MAX; i++)
      {
        switch (i)
        {
            default:
	        break;
            case NOTE_NOTE:
                ch->pcdata->last_note = current_time;
                break;
            case NOTE_IDEA:
                ch->pcdata->last_idea = current_time;
                break;
            case NOTE_GUILDMASTER:
                ch->pcdata->last_guildmaster = current_time;
                break;
            case NOTE_NEWS:
                ch->pcdata->last_news = current_time;
                break;
            case NOTE_CHANGES:
                ch->pcdata->last_changes = current_time;
                break;
            case NOTE_IMM:
                ch->pcdata->last_imm = current_time;
                break;
            case NOTE_RP:
                ch->pcdata->last_rp = current_time;
                break;
            case NOTE_GUIDE:
                ch->pcdata->last_guide = current_time;
                break;
            case NOTE_STORE:
                ch->pcdata->last_store = current_time;
                break;
        }
      }
      send_to_char("All Boards Caught Up.\n\r", ch);
    }

    send_to_char("\n\r`$Note Boards:`*\n\r\n\r", ch);
 
    if ((count = count_spool (ch, news_list)) >= 0)
    {
        found = TRUE;
        sprintf (buf, "New News:          `#%d`*\n\r", count);
        send_to_char (buf, ch);
    }
    if ((count = count_spool (ch, changes_list)) >= 0)
    {
        found = TRUE;
        sprintf (buf, "New Changes:       `#%d`*\n\r", count);               
        send_to_char (buf, ch);
    }
    if ((count = count_spool (ch, note_list)) >= 0)
    {
        found = TRUE;
        sprintf (buf, "New Notes:         `#%d`*\n\r", count);            
        send_to_char (buf, ch);
    }
    if ((count = count_spool (ch, idea_list)) >= 0)
    {
        found = TRUE;
        sprintf (buf, "New Ideas:         `#%d`*\n\r", count);          
        send_to_char (buf, ch);
    }

    if ((count = count_spool (ch, storenote_list)) >= 0)
    {
        found = TRUE;
        sprintf (buf, "New Store:         `#%d`*\n\r", count);          
        send_to_char (buf, ch);
    }

     if ((count = count_spool (ch, imm_list)) >= 0)
    {
        found = TRUE;
        sprintf (buf, "New Immortal:      `#%d`*\n\r", count);          
        send_to_char (buf, ch);
    }
    if ((count = count_spool (ch, rp_list)) >= 0)
    {
        found = TRUE;
        sprintf (buf, "New RP Notes:      `#%d`*\n\r", count);          
        send_to_char (buf, ch);
    }
    if ((ch->clan == clan_lookup("Guide"))&&(count = count_spool (ch, guide_list)) >= 0)
    {
        found = TRUE;
        sprintf (buf, "New Guide Notes:   `#%d`*\n\r", count);          
        send_to_char (buf, ch);
    }
    if ((IS_TRUSTED (ch, IMPLEMENTOR)||IS_GUILDMASTER(ch)||IS_FORSAKEN(ch)||IS_DRAGON(ch))
        && (count = count_spool (ch, guildmaster_list)) >= 0)
    {
        found = TRUE;
        sprintf (buf, "New Guildmaster:   `#%d`*\n\r", count);
        send_to_char (buf, ch);
    }

    if (!found)
        send_to_char ("You have no unread notes.\n\r", ch);
}

void do_note (CHAR_DATA * ch, char *argument)
{
    parse_note (ch, argument, NOTE_NOTE);
}
void do_rp (CHAR_DATA * ch, char *argument)
{
    parse_note (ch, argument, NOTE_RP);
}
void do_imm (CHAR_DATA * ch, char *argument)
{
    parse_note (ch, argument, NOTE_IMM);
}

void do_idea (CHAR_DATA * ch, char *argument)
{
    parse_note (ch, argument, NOTE_IDEA);
}

void do_guide (CHAR_DATA * ch, char *argument)
{
    parse_note (ch, argument, NOTE_GUIDE);
}

void do_snote (CHAR_DATA * ch, char *argument)
{
    parse_note (ch, argument, NOTE_STORE);
}

void do_guildmaster (CHAR_DATA * ch, char *argument)
{
    if (IS_TRUSTED (ch, IMPLEMENTOR)||IS_GUILDMASTER(ch)||IS_FORSAKEN(ch)||IS_DRAGON(ch))
        parse_note (ch, argument, NOTE_GUILDMASTER);
    else
        send_to_char("Huh?\n\r",ch);
}

void do_news (CHAR_DATA * ch, char *argument)
{
    parse_note (ch, argument, NOTE_NEWS);
}

void do_changes (CHAR_DATA * ch, char *argument)
{
    parse_note (ch, argument, NOTE_CHANGES);
}

void save_notes (int type)
{
    FILE *fp;
    char *name;
    NOTE_DATA *pnote;

    switch (type)
    {
        default:
            return;
        case NOTE_NOTE:
            name = NOTE_FILE;
            pnote = note_list;
            break;
        case NOTE_IDEA:
            name = IDEA_FILE;
            pnote = idea_list;
            break;
        case NOTE_GUILDMASTER:
            name = GUILDMASTER_FILE;
            pnote = guildmaster_list;
            break;
        case NOTE_NEWS:
            name = NEWS_FILE;
            pnote = news_list;
            break;
        case NOTE_CHANGES:
            name = CHANGES_FILE;
            pnote = changes_list;
            break;
        case NOTE_IMM:
            name = IMM_FILE;
            pnote = imm_list;
            break;
        case NOTE_RP:
            name = RP_FILE;
            pnote = rp_list;
            break;        
        case NOTE_GUIDE:
            name = GUIDE_FILE;
            pnote = guide_list;
            break;        
        case NOTE_STORE:
            name = STORENOTE_FILE;
            pnote = storenote_list;
            break;        
    }

    fclose (fpReserve);
    if ((fp = fopen (name, "w")) == NULL)
    {
        perror (name);
    }
    else
    {
        for (; pnote != NULL; pnote = pnote->next)
        {
            fprintf (fp, "Sender  %s~\n", pnote->sender);
            fprintf (fp, "Date    %s~\n", pnote->date);
            fprintf (fp, "Stamp   %ld\n", pnote->date_stamp);
            fprintf (fp, "To      %s~\n", pnote->to_list);
            fprintf (fp, "Subject %s~\n", pnote->subject);
            fprintf (fp, "Text\n%s~\n", pnote->text);
        }
        fclose (fp);
        fpReserve = fopen (NULL_FILE, "r");
        return;
    }
}
void load_notes (void)
{
    load_thread (NOTE_FILE, &note_list, NOTE_NOTE, 10 * 24 * 60 * 60);
    load_thread (IDEA_FILE, &idea_list, NOTE_IDEA, 182 * 24 * 60 * 60);
    load_thread (GUILDMASTER_FILE, &guildmaster_list, NOTE_GUILDMASTER, 28 * 24 * 60 * 60);
    load_thread (NEWS_FILE, &news_list, NOTE_NEWS, 0);
    load_thread (CHANGES_FILE, &changes_list, NOTE_CHANGES, 365 * 24 * 60 * 60);
    load_thread (IMM_FILE, &imm_list, NOTE_IMM, 28 * 14 * 60 * 60);
    load_thread (RP_FILE, &rp_list, NOTE_RP, 28 * 24 * 60 * 60);
    load_thread (GUIDE_FILE, &guide_list, NOTE_GUIDE, 14 * 24 * 60 * 60);
    load_thread (STORENOTE_FILE, &storenote_list, NOTE_STORE, 10 * 24 * 60 * 60);
}

void load_thread (char *name, NOTE_DATA ** list, int type, time_t free_time)
{
    FILE *fp;
    NOTE_DATA *pnotelast;

    if ((fp = fopen (name, "r")) == NULL)
        return;

    pnotelast = NULL;
    for (;;)
    {
        NOTE_DATA *pnote;
        char letter;

        do
        {
            letter = getc (fp);
            if (feof (fp))
            {
                fclose (fp);
                return;
            }
        }
        while (isspace (letter));
        ungetc (letter, fp);

        pnote = (NOTE_DATA *) alloc_perm (sizeof (*pnote));

        if (str_cmp (fread_word (fp), "sender"))
        {
          free(pnote);
          break;
        }
        pnote->sender = fread_string (fp);

        if (str_cmp (fread_word (fp), "date"))
        {
          free(pnote);
          break;
        }
        pnote->date = fread_string (fp);

        if (str_cmp (fread_word (fp), "stamp"))
        {
          free(pnote);
          break;
        }
        pnote->date_stamp = fread_number (fp);

        if (str_cmp (fread_word (fp), "to"))
        {
          free(pnote);
          break;
        }
        pnote->to_list = fread_string (fp);

        if (str_cmp (fread_word (fp), "subject"))
        {
          free(pnote);
          break;
        }
        pnote->subject = fread_string (fp);

        if (str_cmp (fread_word (fp), "text"))
        {
          free(pnote);
          break;
        }
        pnote->text = fread_string (fp);

        if (free_time && pnote->date_stamp < current_time - free_time)
        {
            free_note (pnote);
            continue;
        }

        pnote->type = type;

        if (*list == NULL)
            *list = pnote;
        else
            pnotelast->next = pnote;

        pnotelast = pnote;
    }

    strcpy (strArea, NOTE_FILE);
    fpArea = fp;
    bug ("Load_notes: bad key word.", 0);
    exit (1);
    return;
}

void append_note (NOTE_DATA * pnote)
{
    FILE *fp;
    char *name;
    NOTE_DATA **list;
    NOTE_DATA *last;

    switch (pnote->type)
    {
        default:
            return;
        case NOTE_NOTE:
            name = NOTE_FILE;
            list = &note_list;
            break;
        case NOTE_IDEA:
            name = IDEA_FILE;
            list = &idea_list;
            break;
        case NOTE_GUILDMASTER:
            name = GUILDMASTER_FILE;
            list = &guildmaster_list;
            break;
        case NOTE_NEWS:
            name = NEWS_FILE;
            list = &news_list;
            break;
        case NOTE_CHANGES:
            name = CHANGES_FILE;
            list = &changes_list;
            break;
        case NOTE_IMM:
            name = IMM_FILE;
            list = &imm_list;
            break;
        case NOTE_RP:
            name = RP_FILE;
            list = &rp_list;
            break;
        case NOTE_GUIDE:
            name = GUIDE_FILE;
            list = &guide_list;
            break;
        case NOTE_STORE:
            name = STORENOTE_FILE;
            list = &storenote_list;
            break;
    }

    if (*list == NULL)
        *list = pnote;
    else
    {
        for (last = *list; last->next != NULL; last = last->next);
        last->next = pnote;
    }

    fclose (fpReserve);
    if ((fp = fopen (name, "a")) == NULL)
    {
        perror (name);
    }
    else
    {
        fprintf (fp, "Sender  %s~\n", pnote->sender);
        fprintf (fp, "Date    %s~\n", pnote->date);
        fprintf (fp, "Stamp   %ld\n", pnote->date_stamp);
        fprintf (fp, "To      %s~\n", pnote->to_list);
        fprintf (fp, "Subject %s~\n", pnote->subject);
        fprintf (fp, "Text\n%s~\n", pnote->text);
        fclose (fp);
    }
    fpReserve = fopen (NULL_FILE, "r");
}

bool is_note_to (CHAR_DATA * ch, NOTE_DATA * pnote)
{
    if (IS_NPC(ch))
      return FALSE;

    if (!str_cmp (ch->name, pnote->sender))
        return TRUE;

    if (ch->clan && is_exact_name (clan_table[ch->clan].name, pnote->to_list))
        return TRUE;

    if (is_exact_name (ch->name, pnote->to_list))
        return TRUE;

    if (pnote->type == NOTE_IDEA && !IS_IMMORTAL(ch) && !IS_SET(ch->act2, PLR_DEVELOPER) && IS_GUILDMASTER(ch))
        return FALSE;

    if (pnote->type == NOTE_GUIDE && (ch->clan != clan_lookup("Guide")))
	return FALSE;

    if (pnote->type == NOTE_GUILDMASTER && 
    (!IS_IMMORTAL(ch)&& !IS_GUILDMASTER(ch)&&!IS_FORSAKEN(ch)&&!IS_DRAGON(ch)) )
        return FALSE;

    if (is_exact_name ("all", pnote->to_list) && ch->pcdata->primary)
        return TRUE;

    if ((is_exact_name ("gl", pnote->to_list) || 
        is_exact_name("guildleader", pnote->to_list)) &&
        (IS_SET(ch->act2, PLR_GUILDLEADER) || IS_TRUSTED (ch, IMPLEMENTOR) || IS_GUILDMASTER(ch)||IS_FORSAKEN(ch)||IS_DRAGON(ch)) )
        return TRUE;
    

    if (IS_IMMORTAL (ch) && (is_exact_name ("immortal", pnote->to_list)|| 
is_exact_name("imm", pnote->to_list) || is_exact_name("imms",
pnote->to_list)))
        return TRUE;
        
    if ((ch->level == MAX_LEVEL) && (is_exact_name ("implementor", pnote->to_list)|| 
is_exact_name("imp", pnote->to_list) || is_exact_name("imps",
pnote->to_list)))
        return TRUE;

    return FALSE;
}

bool is_immortal_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if ( IS_IMMORTAL(ch) && (is_exact_name( "immortal", pnote->to_list )
       || is_exact_name( "imm", pnote->to_list )) )
	return TRUE;

    return FALSE;
}

bool is_implementor_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if ( IS_IMMORTAL(ch) && (is_exact_name( "implementor", pnote->to_list )
       || is_exact_name( "imp", pnote->to_list) || is_exact_name("imps",
pnote->to_list)))
	return TRUE;

    return FALSE;
}

bool is_gl_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if ( IS_IMMORTAL(ch) && (is_exact_name( "guildleader", pnote->to_list )
       || is_exact_name( "gl", pnote->to_list )) )
	return TRUE;

    if ( IS_SET(ch->act2, PLR_GUILDLEADER) && (is_exact_name( "guildleader", pnote->to_list )
       || is_exact_name( "gl", pnote->to_list )) )
	return TRUE;
	
    if ( IS_GUILDMASTER(ch) && (is_exact_name( "guildleader", pnote->to_list )
       || is_exact_name( "gl", pnote->to_list )) )
        return TRUE;

    return FALSE;
}


bool is_guild_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if (ch->clan && is_exact_name(clan_table[ch->clan].name,pnote->to_list))
	return TRUE;

    return FALSE;
}

bool is_personal_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if (is_exact_name( ch->name, pnote->to_list ) )
	return TRUE;

    return FALSE;
}


void note_attach (CHAR_DATA * ch, int type)
{
    NOTE_DATA *pnote;

    if (ch->pnote != NULL)
        return;

    pnote = new_note ();

    pnote->next = NULL;
    pnote->sender = str_dup (ch->name);
    pnote->date = str_dup ("");
    pnote->to_list = str_dup ("");
    pnote->subject = str_dup ("");
    pnote->text = str_dup ("");
    pnote->type = type;
    ch->pnote = pnote;
    return;
}

void system_note (char * sender,int type,char *to, char *subject, char *txt)
{
    NOTE_DATA *pnote;
    char *strtime;
    pnote = new_note ();

    pnote->next = NULL;
    pnote->sender = str_dup (sender);
    pnote->to_list = str_dup (to);
    pnote->subject = str_dup (subject);
    pnote->text = str_dup("");
    pnote->type = type;
    strtime = ctime (&current_time);
    strtime[strlen (strtime) - 1] = '\0';
    pnote->date = str_dup (strtime);
    pnote->date_stamp = current_time;
    free_string (pnote->text);
    pnote->text = str_dup (txt);
    append_note (pnote);
    note_echo(pnote, NULL);
}

void note_remove (CHAR_DATA * ch, NOTE_DATA * pnote, bool bDelete)
{
    char to_new[MAX_INPUT_LENGTH];
    char to_one[MAX_INPUT_LENGTH];
    NOTE_DATA *prev;
    NOTE_DATA **list;
    char *to_list;

    if (!bDelete)
    {
        /* make a new list */
        to_new[0] = '\0';
        to_list = pnote->to_list;
        while (*to_list != '\0')
        {
            to_list = one_argument (to_list, to_one);
            if (to_one[0] != '\0' && str_cmp (ch->name, to_one))
            {
                strcat (to_new, " ");
                strcat (to_new, to_one);
            }
        }
        /* Just a simple recipient removal? */
        if (str_cmp (ch->name, pnote->sender) && to_new[0] != '\0')
        {
            free_string (pnote->to_list);
            pnote->to_list = str_dup (to_new + 1);
            return;
        }
    }
    /* nuke the whole note */

    switch (pnote->type)
    {
        default:
            return;
        case NOTE_NOTE:
            list = &note_list;
            break;
        case NOTE_IDEA:
            list = &idea_list;
            break;
        case NOTE_GUILDMASTER:
            list = &guildmaster_list;
            break;
        case NOTE_NEWS:
            list = &news_list;
            break;
        case NOTE_CHANGES:
            list = &changes_list;
            break;
        case NOTE_IMM:
            list = &imm_list;
            break;
        case NOTE_RP:
            list = &rp_list;
            break;
        case NOTE_GUIDE:
            list = &guide_list;
            break;
        case NOTE_STORE:
            list = &storenote_list;
            break;
    }

    /*
     * Remove note from linked list.
     */
    if (pnote == *list)
    {
        *list = pnote->next;
    }
    else
    {
        for (prev = *list; prev != NULL; prev = prev->next)
        {
            if (prev->next == pnote)
                break;
        }

        if (prev == NULL)
        {
            bug ("Note_remove: pnote not found.", 0);
            return;
        }

        prev->next = pnote->next;
    }

    save_notes (pnote->type);
    free_note (pnote);
    return;
}

bool hide_note (CHAR_DATA * ch, NOTE_DATA * pnote)
{
    time_t last_read;

    if (IS_NPC (ch))
        return TRUE;

    switch (pnote->type)
    {
        default:
            return TRUE;
        case NOTE_NOTE:
            last_read = ch->pcdata->last_note;
            break;
        case NOTE_IDEA:
            last_read = ch->pcdata->last_idea;
            break;
        case NOTE_GUILDMASTER:
            last_read = ch->pcdata->last_guildmaster;
            break;
        case NOTE_NEWS:
            last_read = ch->pcdata->last_news;
            break;
        case NOTE_CHANGES:
            last_read = ch->pcdata->last_changes;
            break;
        case NOTE_IMM:
            last_read = ch->pcdata->last_imm;
            break;
        case NOTE_RP:
            last_read = ch->pcdata->last_rp;
            break; 
        case NOTE_GUIDE:
            last_read = ch->pcdata->last_guide;
            break; 
        case NOTE_STORE:
            last_read = ch->pcdata->last_store;
            break; 
           
    }

    if (pnote->date_stamp <= last_read)
        return TRUE;

    if (!str_cmp (ch->name, pnote->sender))
        return TRUE;

    if (!is_note_to (ch, pnote))
        return TRUE;

    return FALSE;
}

void update_read (CHAR_DATA * ch, NOTE_DATA * pnote)
{
    time_t stamp;

    if (IS_NPC (ch))
        return;

    stamp = pnote->date_stamp;

    switch (pnote->type)
    {
        default:
            return;
        case NOTE_NOTE:
            ch->pcdata->last_note = UMAX (ch->pcdata->last_note, stamp);
            break;
        case NOTE_IDEA:
            ch->pcdata->last_idea = UMAX (ch->pcdata->last_idea, stamp);
            break;
        case NOTE_GUILDMASTER:
            ch->pcdata->last_guildmaster = UMAX (ch->pcdata->last_guildmaster, stamp);
            break;
        case NOTE_NEWS:
            ch->pcdata->last_news = UMAX (ch->pcdata->last_news, stamp);
            break;
        case NOTE_CHANGES:
            ch->pcdata->last_changes = UMAX (ch->pcdata->last_changes, stamp);
            break;
        case NOTE_IMM:
            ch->pcdata->last_imm = UMAX (ch->pcdata->last_imm, stamp);
            break;
        case NOTE_RP:
            ch->pcdata->last_rp = UMAX (ch->pcdata->last_rp, stamp);
            break;
        case NOTE_GUIDE:
            ch->pcdata->last_guide = UMAX (ch->pcdata->last_guide, stamp);
            break;
        case NOTE_STORE:
            ch->pcdata->last_store = UMAX (ch->pcdata->last_store, stamp);
            break;
    }
}

void parse_note (CHAR_DATA * ch, char *argument, int type)
{
    BUFFER *buffer;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    NOTE_DATA *pnote;
    NOTE_DATA **list;
    char *list_name;
    int vnum;
    int anum;
    
    if (IS_NPC (ch))
        return;

    switch (type)
    {
        default:
            return;
        case NOTE_NOTE:
            list = &note_list;
            list_name = "notes";
            break;
        case NOTE_IDEA:
            list = &idea_list;
            list_name = "ideas";
            break;
        case NOTE_GUILDMASTER:
            list = &guildmaster_list;
            list_name = "guildmaster";
            break;
        case NOTE_NEWS:
            list = &news_list;
            list_name = "news";
            break;
        case NOTE_CHANGES:
            list = &changes_list;
            list_name = "changes";
            break;
        case NOTE_IMM:
            list = &imm_list;
            list_name = "imm";
            break;
        case NOTE_RP:
            list = &rp_list;
            list_name = "RP";
            break;
        case NOTE_GUIDE:
            list = &guide_list;
            list_name = "Guide";
            break;
        case NOTE_STORE:
            list = &storenote_list;
            list_name = "Store Note";
            break;
    }

    argument = one_argument (argument, arg);
    smash_tilde (argument);

    if (arg[0] == '\0' || !str_prefix (arg, "read"))
    {
        bool fAll;

        if (!str_cmp (argument, "all"))
        {
            fAll = TRUE;
            anum = 0;
        }

        else if (argument[0] == '\0' || !str_prefix (argument, "next"))
            /* read next unread note */
        {
            vnum = 0;
            for (pnote = *list; pnote != NULL; pnote = pnote->next)
            {
                if (!hide_note (ch, pnote))
                {
		    if (IS_SET(ch->act2, PLR_MXP))	
                      sprintf (buf, "\e[1z <font '%s'>`$[`&%3d`$] `&%s:`* %s</font>\n\r"
		  		    "\e[1z <font '%s'>`&%s`*</font>\n\r"
				    "\e[1z <font '%s'>`&To:`* %s`*</font>\n\r",
                             pnote->type == NOTE_RP ? ch->pcdata->fonts.rpnote : ch->pcdata->fonts.notes,
                             vnum,
                             pnote->sender,
                             pnote->subject,
			     pnote->type == NOTE_RP ? ch->pcdata->fonts.rpnote : ch->pcdata->fonts.notes,
			     pnote->date, 
			     pnote->type == NOTE_RP ? ch->pcdata->fonts.rpnote : ch->pcdata->fonts.notes,
			     pnote->to_list);
		    else
                      sprintf (buf, "`$[`&%3d`$] `&%s:`* %s\n\r"
		  		    "`&%s`*\n\r"
				    "`&To:`* %s`*\n\r",
                             vnum,
                             pnote->sender,
                             pnote->subject, pnote->date, pnote->to_list);

                    send_to_char (buf, ch);
		    if (IS_SET(ch->act2, PLR_MXP))
                    {
			page_to_char(fontnote(pnote->text, ch, pnote->type), ch);
                    }  
		    else
                      page_to_char (pnote->text, ch);
                    send_to_char ("`*", ch); 
                    update_read (ch, pnote);
                    return;
                }
                else if (is_note_to (ch, pnote))
                    vnum++;
            }
            sprintf (buf, "You have no unread %s.\n\r", list_name);
            send_to_char (buf, ch);
            return;
        }

        else if (is_number (argument))
        {
            fAll = FALSE;
            anum = atoi (argument);
        }
        else
        {
            send_to_char ("Read which number?\n\r", ch);
            return;
        }

        vnum = 0;
        for (pnote = *list; pnote != NULL; pnote = pnote->next)
        {
            if (is_note_to (ch, pnote) && (vnum++ == anum || fAll))
            {
                 if (IS_SET(ch->act2, PLR_MXP))
                   sprintf (buf, "\e[1z<font '%s'>`$[`&%3d`$]`& %s:`* %s</font>\n\r"
				  "\e[1z<font '%s'>`&%s`*</font>\n\r"
				  "\e[1z<font '%s'>`&To:`* %s`*</font>\n\r",
			 pnote->type == NOTE_RP ? ch->pcdata->fonts.rpnote : ch->pcdata->fonts.notes,
                         vnum - 1,
                         pnote->sender,
                         pnote->subject, 
			 pnote->type == NOTE_RP ? ch->pcdata->fonts.rpnote : ch->pcdata->fonts.notes,
                         pnote->date,
			 pnote->type == NOTE_RP ? ch->pcdata->fonts.rpnote : ch->pcdata->fonts.notes,
                         pnote->to_list);
                 else
                   sprintf (buf, "`$[`&%3d`$]`& %s:`* %s`*\n\r`&%s`*\n\r`&To:`* %s`*\n\r",
                         vnum - 1,
                         pnote->sender,
                         pnote->subject, pnote->date, pnote->to_list);
                send_to_char (buf, ch);
		if (IS_SET(ch->act2, PLR_MXP))
                {
	 	  page_to_char(fontnote(pnote->text, ch, pnote->type), ch);
                }  
		else
                  page_to_char (pnote->text, ch);
                send_to_char ("`*", ch); 
                update_read (ch, pnote);
                return;
            }
        }

        sprintf (buf, "There aren't that many %s.\n\r", list_name);
        send_to_char (buf, ch);
        return;
    }

    if ( !str_prefix(arg, "forward") )
    {
        bool fAll;
        NOTE_DATA *onote;
        char nBuf[MAX_INPUT_LENGTH];

        if ( is_number( argument ) )
        {
            fAll = FALSE;
            anum = atoi( argument );
        }
        else
        {
            send_to_char( "Read which number?\n\r", ch );
            return;
        }
 
        vnum = 0;
        for ( onote = *list; onote != NULL; onote = onote->next )
        {
            if ( is_note_to( ch, onote ) && ( vnum++ == anum || fAll ) )
            {
                sprintf( buf, "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
                    vnum - 1,
                    onote->sender,
                    onote->subject,
                    onote->date,
                    onote->to_list
                    );
                note_attach( ch, type );
                ch->pnote->next                  = NULL;
                free_string(nBuf);
                sprintf(nBuf, "FW: %s", onote->subject);
                free_string(ch->pnote->subject);
                ch->pnote->subject               = str_dup(nBuf);
              
                free_string(nBuf);
         	buffer = new_buf();
                sprintf(nBuf,"**FORWARD - Original note by %s**\n\r\n\r", onote->sender);
		add_buf(buffer, nBuf);
        	add_buf(buffer, onote->text);
                add_buf(buffer, "\n\r**END OF FORWARD TEXT**\n\r");
	        free_string( ch->pnote->text );
	        ch->pnote->text = str_dup( buf_string(buffer) );
         	free_buf(buffer);
	        send_to_char( "Ok.\n\r", ch );
                return;
            }
        }
 
	sprintf(buf,"There aren't that many %s.\n\r",list_name);
	send_to_char(buf,ch);
        return;
    }

    if ( !str_prefix(arg, "export") && IS_HIIMMORTAL(ch))
    {
        bool fAll;
        NOTE_DATA *onote;

        if ( is_number( argument ) )
        {
            fAll = FALSE;
            anum = atoi( argument );
        }
        else
        {
            send_to_char( "Export which number?\n\r", ch );
            return;
        }
 
        vnum = 0;
        for ( onote = *list; onote != NULL; onote = onote->next )
        {
            if ( is_note_to( ch, onote ) && ( vnum++ == anum || fAll ) )
	    {
                char file[MSL];
		export_note(onote);
		if (IS_SET(ch->act2, PLR_MXP))
                {
  		  sprintf(file, "\e[1z Note exported to <a \"http://www.shadowsoftime.com/export/notes/%s%ld.html\">here</a>.\n\r", 
				onote->sender, onote->date_stamp);
		}
		else
		{
		  sprintf(file, "Note exported to http://www.shadowsoftime.com/export/notes/%s%ld.html\n\r", 
				onote->sender, onote->date_stamp);
		}
		send_to_char(file, ch);
                return;
            }
        }
 
	sprintf(buf,"There aren't that many %s.\n\r",list_name);
	send_to_char(buf,ch);
        return;
    }

    if ( !str_prefix(arg, "export_list") && IS_HIIMMORTAL(ch))
    {
        NOTE_DATA *onote;
        FILE *fp;
        char buf[MSL];
         
        sprintf(buf, "%s%s.html", EXPORT_NOTE_FOLDER, note_name(type));
        if ((fp = fopen(buf, "w")) == NULL)
        {
          bug("Export_note_list: fopen", 0);
          return;
        }

        sprintf(buf, "%s list", note_name(type));
        print_head(fp, buf);
        for ( onote = *list; onote != NULL; onote = onote->next )
        {
          if (is_note_to(ch, onote))
          {
	    export_note(onote, fp);
            fprintf(fp, "</font><font color=#666666><hr color =\"red\"\n");
          }
        }
        print_tail(fp);
        fclose(fp);
        send_to_char("Exported.\n\r", ch);
        return;
    }

    if (!str_prefix (arg, "list"))
    {
      char arg2[MSL];
      char arg3[MSL];
      bool grep = FALSE;
      bool fullSearch = FALSE;
      bool guildRestrict = FALSE;
      bool personalRestrict = FALSE;
      int min, max;
      bool restrict = FALSE;
      bool timeRestrict = FALSE;
      long rtime = 0;
      
      
      min = 0;
      max = 32000;      

      argument = one_argument(argument, arg2);
      argument = one_argument(argument, arg3);
  
      if (is_number(arg2) && is_number(arg3))
      {
        min = atoi(arg2);
        max = atoi(arg3);
        restrict = TRUE;
      }   
 
      if (!str_cmp(arg2, "guild"))
      {
        guildRestrict = TRUE;
      }

      if (!str_cmp(arg2, "personal"))
      {
        personalRestrict = TRUE;
      }

      if (!str_cmp(arg2, "@"))
      {
        int seconds;
        if (!is_number(arg3))
        {
          send_to_char("How many days to show?\n\r", ch);
          return;
        }
        timeRestrict = TRUE;
        seconds = atoi(arg3) * 24 * 60 * 60;
        rtime = current_time - seconds;
      }

      if (!str_cmp(arg2, "|") && arg3[0] != '\0')
      {
        grep = TRUE;
      }

      if (!str_cmp(arg2, "*") && arg3[0] != '\0')
      {
        grep = TRUE;
        fullSearch = TRUE;
      }
        buffer = new_buf ();
        buf[0] = '\0';
        vnum = 0;
        for (pnote = *list; pnote != NULL; pnote = pnote->next)
        {
            if (is_note_to (ch, pnote))
            {
              if ((!grep && !restrict && !timeRestrict && !guildRestrict && !personalRestrict) ||
		  (guildRestrict && is_guild_note_to(ch, pnote)) ||
		  (personalRestrict && is_personal_note_to(ch, pnote)) ||
                   (timeRestrict && pnote->date_stamp > rtime) ||
                   (restrict && vnum >= min && vnum <= max) ||
	             (grep && (!str_prefix(pnote->sender, arg3) ||
	              (strstr(pnote->subject, capitalize(arg3)) ||
	               strstr(pnote->subject, arg3)) || 
                       (fullSearch && 
                           ( strstr(pnote->text, arg3) ||
                            strstr(pnote->text, capitalize(arg3))) ))))
               {
	        if (1 == 2) // coming back to this later
	        {
                  sprintf (buf, "\e[1z<font 'lucida handwriting'>[%3d%s] %s`*: %s`*</font>\n\r",
                         vnum, hide_note (ch, pnote) ? " " : "N",
                         pnote->sender, pnote->subject);
                  add_buf (buffer, buf);
                }
                else
                {
                  sprintf (buf, "[%3d%s] %s`*: %s`*\n\r",
                         vnum, hide_note (ch, pnote) ? " " : "N",
                         pnote->sender, pnote->subject);
                  add_buf (buffer, buf);
                }

                //send_to_char (buf, ch);
                if (1 == 2) // coming back to this later
                {
                  if ( is_personal_note_to( ch, pnote ) )
                    add_buf(buffer,"\e[1z<font 'lucida handwriting'>      `!Personal Note`*</font>\n\r");
                  if ( is_immortal_note_to( ch, pnote ) )
                    add_buf(buffer,"\e[1z<font 'lucida handwriting'>      `&Immortal Note`*</font>\n\r");
                  if ( is_implementor_note_to( ch, pnote ) )
                    add_buf(buffer,"\e[1z<font 'lucida handwriting'>      `#Implementor Note`*</font>\n\r");
                    
                  if ( is_gl_note_to( ch, pnote ) )
                    add_buf(buffer,"\e[1z<font 'lucida handwriting'>      `$GuildLeader Note`*</font>\n\r");
                  if ( is_guild_note_to( ch, pnote ) )
                  {
                    sprintf(buf, "\e[1z<font 'lucida handwriting'>      `4%s`* Note</font>\n\r", capitalize(clan_table[ch->clan].name));
                    add_buf(buffer, buf);
                  }
		}   
		else
                {
                  if ( is_personal_note_to( ch, pnote ) )
                    add_buf(buffer,"      `!Personal Note`*\n\r");
                  if ( is_immortal_note_to( ch, pnote ) )
                    add_buf(buffer,"      `&Immortal Note`*\n\r");
                  if ( is_implementor_note_to( ch, pnote ) )
                    add_buf(buffer,"      `#Implementor Note`*\n\r");
                    
                  if ( is_gl_note_to( ch, pnote ) )
                    add_buf(buffer,"      `$GuildLeader Note`*\n\r");
                  if ( is_guild_note_to( ch, pnote ) )
                  {
                    sprintf(buf, "      `4%s`* Note\n\r", capitalize(clan_table[ch->clan].name));
                    add_buf(buffer, buf);
                  }
		}   
                vnum++;
              }
              else
                vnum++; 
            }
        }
        page_to_char (buf_string (buffer), ch);
        free_buf (buffer);
        if (!vnum)
        {
            switch (type)
            {
                case NOTE_NOTE:
                    send_to_char ("There are no notes for you.\n\r", ch);
                    break;
                case NOTE_IDEA:
                    send_to_char ("There are no ideas for you.\n\r", ch);
                    break;
                case NOTE_GUILDMASTER:
                    send_to_char ("There are no guildmasters for you.\n\r", ch);
                    break;
                case NOTE_NEWS:
                    send_to_char ("There is no news for you.\n\r", ch);
                    break;
                case NOTE_CHANGES:
                    send_to_char ("There are no changes for you.\n\r", ch);
                    break;
                case NOTE_IMM:
                    send_to_char ("There are no imm notes for you.\n\r", ch);
                    break;
                case NOTE_RP:
                    send_to_char ("There are no rp notes for you.\n\r", ch);
                    break;
                case NOTE_GUIDE:
                    send_to_char ("There are no Guide notes for you.\n\r", ch);
                    break;
                case NOTE_STORE:
                    send_to_char ("There are no Store notes for you.\n\r", ch);
                    break;
            }
        }
        return;
    }

    if (!str_prefix (arg, "remove"))
    {
        if (!is_number (argument))
        {
            send_to_char ("Note remove which number?\n\r", ch);
            return;
        }

        anum = atoi (argument);
        vnum = 0;
        for (pnote = *list; pnote != NULL; pnote = pnote->next)
        {
            if (is_note_to (ch, pnote) && vnum++ == anum)
            {
                note_remove (ch, pnote, FALSE);
                send_to_char ("Ok.\n\r", ch);
                return;
            }
        }

        sprintf (buf, "There aren't that many %s.", list_name);
        send_to_char (buf, ch);
        return;
    }

    if (!str_prefix (arg, "delete") && get_trust (ch) >= MAX_LEVEL - 1)
    {
        if (!is_number (argument))
        {
            send_to_char ("Note delete which number?\n\r", ch);
            return;
        }

        anum = atoi (argument);
        vnum = 0;
        for (pnote = *list; pnote != NULL; pnote = pnote->next)
        {
            if (is_note_to (ch, pnote) && vnum++ == anum)
            {
                note_remove (ch, pnote, TRUE);
                send_to_char ("Ok.\n\r", ch);
                return;
            }
        }

        sprintf (buf, "There aren't that many %s.", list_name);
        send_to_char (buf, ch);
        return;
    }

    if (!str_prefix (arg, "catchup"))
    {
        switch (type)
        {
            case NOTE_NOTE:
                ch->pcdata->last_note = current_time;
                break;
            case NOTE_IDEA:
                ch->pcdata->last_idea = current_time;
                break;
            case NOTE_GUILDMASTER:
                ch->pcdata->last_guildmaster = current_time;
                break;
            case NOTE_NEWS:
                ch->pcdata->last_news = current_time;
                break;
            case NOTE_CHANGES:
                ch->pcdata->last_changes = current_time;
                break;
            case NOTE_IMM:
                ch->pcdata->last_imm = current_time;
                break;
            case NOTE_RP:
                ch->pcdata->last_rp = current_time;
                break;
            case NOTE_GUIDE:
                ch->pcdata->last_guide = current_time;
                break;
            case NOTE_STORE:
                ch->pcdata->last_store = current_time;
                break;

        }
        return;
    }

    /* below this point only certain people can edit notes */
    if ((type == NOTE_NEWS && !IS_TRUSTED (ch, SUPREME))
        || (type == NOTE_CHANGES && !IS_TRUSTED (ch, CREATOR)))
    {
        sprintf (buf, "You aren't high enough level to write %s.", list_name);
        send_to_char (buf, ch);
        return;
    }

    if (type == NOTE_STORE)
    {
      if ((ch->clan > 0 && ch->rank < clan_table[ch->clan].top_rank - 2) && !have_store(ch) && !IS_IMMORTAL(ch))
      {
        send_to_char("You don't have a store!\n\r", ch);
        return;
      }
    }   

    if (!str_cmp (arg, "format"))
    {
        note_attach (ch, type);
        if (ch->pnote->type != type)
        {
            send_to_char
                ("You already have a different note in progress.\n\r", ch);
            return;
        }

        ch->pnote->text = format_string(ch->pnote->text);
        send_to_char("Body formatted.\n\r",ch);
        return;
    }

    if (!str_cmp (arg, "+"))
    {
        note_attach (ch, type);
        if (ch->pnote->type != type)
        {
            send_to_char
                ("You already have a different note in progress.\n\r", ch);
            return;
        }

        if (strlen (ch->pnote->text) + strlen (argument) >= 4096)
        {
            send_to_char ("Note too long.\n\r", ch);
            return;
        }

        buffer = new_buf ();

        add_buf (buffer, ch->pnote->text);
        add_buf (buffer, argument);
        add_buf (buffer, "\n\r");
        free_string (ch->pnote->text);
        ch->pnote->text = str_dup (buf_string (buffer));
        free_buf (buffer);
        send_to_char ("Ok.\n\r", ch);
        return;
    }

    if (!str_cmp (arg, "edit"))
    {
        note_attach (ch, type);
        if (ch->pnote->type != type)
        {
            send_to_char
                ("You already have a different note in progress.\n\r", ch);
            return;
        }

        if (argument[0] != '\0')
        {
            send_to_char ("Syntax is notetype edit.\n\r", ch);
            return;
        }

        string_append(ch, &ch->pnote->text);
        return;
    }


    if (!str_cmp (arg, "-"))
    {
        int len;
        bool found = FALSE;

        note_attach (ch, type);
        if (ch->pnote->type != type)
        {
            send_to_char
                ("You already have a different note in progress.\n\r", ch);
            return;
        }

        if (ch->pnote->text == NULL || ch->pnote->text[0] == '\0')
        {
            send_to_char ("No lines left to remove.\n\r", ch);
            return;
        }

        strcpy (buf, ch->pnote->text);

        for (len = strlen (buf); len > 0; len--)
        {
            if (buf[len] == '\r')
            {
                if (!found)
                {                /* back it up */
                    if (len > 0)
                        len--;
                    found = TRUE;
                }
                else
                {                /* found the second one */

                    buf[len + 1] = '\0';
                    free_string (ch->pnote->text);
                    ch->pnote->text = str_dup (buf);
                    return;
                }
            }
        }
        buf[0] = '\0';
        free_string (ch->pnote->text);
        ch->pnote->text = str_dup (buf);
        return;
    }

    if (!str_prefix (arg, "subject"))
    {
        note_attach (ch, type);
        if (ch->pnote->type != type)
        {
            send_to_char
                ("You already have a different note in progress.\n\r", ch);
            return;
        }

        free_string (ch->pnote->subject);
        ch->pnote->subject = str_dup (argument);
        send_to_char ("Ok.\n\r", ch);
        return;
    }

    if (!str_prefix (arg, "to"))
    {
        note_attach (ch, type);
        if (ch->pnote->type != type)
        {
            send_to_char
                ("You already have a different note in progress.\n\r", ch);
            return;
        }
        free_string (ch->pnote->to_list);
        ch->pnote->to_list = str_dup (argument);
        send_to_char ("Ok.\n\r", ch);
        return;
    }

    if (!str_prefix (arg, "as"))
    {
        if (get_trust(ch) != MAX_LEVEL)
          return;

        note_attach (ch, type);
        if (ch->pnote->type != type)
        {
            send_to_char
                ("You already have a different note in progress.\n\r", ch);
            return;
        }
        free_string (ch->pnote->sender);
        ch->pnote->sender = str_dup (argument);
        send_to_char ("Ok.\n\r", ch);
        return;
    }

    if (!str_prefix (arg, "clear"))
    {
        if (ch->pnote != NULL)
        {
            free_note (ch->pnote);
            ch->pnote = NULL;
        }

        send_to_char ("Ok.\n\r", ch);
        return;
    }

    if (!str_prefix (arg, "show"))
    {
        if (ch->pnote == NULL)
        {
            send_to_char ("You have no note in progress.\n\r", ch);
            return;
        }

        if (ch->pnote->type != type)
        {
            send_to_char ("You aren't working on that kind of note.\n\r", ch);
            return;
        }

	if (IS_SET(ch->act2, PLR_MXP))
        {
          sprintf (buf, "\e[1z<font '%s'> %s`*: %s`*</font>\n\r"
                  "\e[1z<font '%s'>`&To:`* %s`*</font>\n\r",
			ch->pnote->type == NOTE_RP ? ch->pcdata->fonts.rpnote : ch->pcdata->fonts.notes,
                        ch->pnote->sender, ch->pnote->subject, 
			ch->pnote->type == NOTE_RP ? ch->pcdata->fonts.rpnote : ch->pcdata->fonts.notes,
			ch->pnote->to_list);
          send_to_char (buf, ch);
 	}
	else
	{
        sprintf (buf, "%s`*: %s`*\n\r`&To: %s`*\n\r",
                 ch->pnote->sender, ch->pnote->subject, ch->pnote->to_list);
        send_to_char (buf, ch);
	}
	if (IS_SET(ch->act2, PLR_MXP))
        {
	  page_to_char(fontnote(ch->pnote->text, ch, ch->pnote->type), ch);
	}
	else       
          send_to_char (ch->pnote->text, ch);
        send_to_char ("`*", ch);
        return;
    }

    if (!str_prefix (arg, "post") || !str_prefix (arg, "send"))
    {
        char *strtime;

        if (ch->pnote == NULL)
        {
            send_to_char ("You have no note in progress.\n\r", ch);
            return;
        }

        if (ch->pnote->type != type)
        {
            send_to_char ("You aren't working on that kind of note.\n\r", ch);
            return;
        }

        if (!str_cmp (ch->pnote->to_list, ""))
        {
            send_to_char
                ("You need to provide a recipient (name, all, or immortal).\n\r",
                 ch);
            return;
        }

        if (!str_cmp (ch->pnote->subject, ""))
        {
            send_to_char ("You need to provide a subject.\n\r", ch);
            return;
        }

        ch->pnote->next = NULL;
        strtime = ctime (&current_time);
        strtime[strlen (strtime) - 1] = '\0';
        ch->pnote->date = str_dup (strtime);
        ch->pnote->date_stamp = current_time;
        note_echo(ch->pnote, ch);
        append_note (ch->pnote);
        ch->pnote = NULL;
        return;
    }

    send_to_char ("You can't do that.\n\r", ch);
    return;
}

char *note_name(int iType)
{
  switch (iType)
  {
    case NOTE_NOTE:
      return "note";
      break;
    case NOTE_IDEA:
      return "idea";
      break;
    case NOTE_CHANGES:
      return "change";
      break;
    case NOTE_IMM:
      return "immortal";
      break;
    case NOTE_RP:
      return "roleplay";
      break;
    case NOTE_NEWS:
      return "news";
      break;
    case NOTE_GUILDMASTER:
      return "guildmaster";
      break;
    case NOTE_GUIDE:
      return "guide";
      break;
    case NOTE_STORE:
      return "store";
      break;
    default:
      return "other";
      break;
  }
} 

void note_echo(NOTE_DATA *pnote, CHAR_DATA *ch)
{
  char type[MSL];
  char buf[MSL];
  CHAR_DATA *victim;

  DESCRIPTOR_DATA *d;

  switch (pnote->type)
  {
    case NOTE_NOTE:
      sprintf(type, "note");
      break;
    case NOTE_IDEA:
      sprintf(type, "idea");
      break;
    case NOTE_CHANGES:
      sprintf(type, "change");
      break;
    case NOTE_IMM:
      sprintf(type, "immortal");
      break;
    case NOTE_RP:
      sprintf(type, "roleplay");
      break;
    case NOTE_NEWS:
      sprintf(type, "news");
      break;
    case NOTE_GUILDMASTER:
      sprintf(type, "guildmaster");
      break;
    case NOTE_GUIDE:
      sprintf(type, "guide");
      break;
    case NOTE_STORE:
      sprintf(type, "store");
      break;
    default:
      sprintf(type, "other");
      break;
    }
 
       
    for (d = descriptor_list; d != NULL; d = d->next)
    {
       victim = d->character;
       
       if (victim == NULL || IS_NPC(victim) || d->connected != CON_PLAYING)
          continue;

       if (!is_note_to(victim, pnote))
         continue;

       if (ch != NULL && !str_cmp(ch->name, pnote->sender))
         sprintf(buf, "New `!%s`* from `$%s`* about `&%s`*.\n\r", type, PERS(ch, victim, TRUE), pnote->subject);
       else
         sprintf(buf, "New `!%s`* from `$%s`* about `&%s`*.\n\r", type, pnote->sender, pnote->subject);
       send_to_char(buf, victim); 
    }
}

