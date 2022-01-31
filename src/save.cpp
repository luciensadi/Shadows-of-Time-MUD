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
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

#if !defined(macintosh)
extern int _filbuf args ((FILE *));
#endif

void fread_rle( char *explored, FILE *fp );
void fwrite_rle( char *explored, FILE *fp );


/* int rename(const char *oldfname, const char *newfname); viene en stdio.h */

char *print_flags (int flag)
{
    int count, pos = 0;
    static char buf[52];


    for (count = 0; count < 32; count++)
    {
        if (IS_SET (flag, 1 << count))
        {
            if (count < 26)
                buf[pos] = 'A' + count;
            else
                buf[pos] = 'a' + (count - 26);
            pos++;
        }
    }

    if (pos == 0)
    {
        buf[pos] = '0';
        pos++;
    }

    buf[pos] = '\0';

    return buf;
}


/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST    100
static OBJ_DATA *rgObjNest[MAX_NEST];



/*
 * Local functions.
 */
void fwrite_char args ((CHAR_DATA * ch, FILE * fp));
void fwrite_obj args ((CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest));
void fwrite_pet args ((CHAR_DATA * pet, FILE * fp));
void fread_char args ((CHAR_DATA * ch, FILE * fp));
void fread_pet args ((CHAR_DATA * ch, FILE * fp));
void fread_obj args ((CHAR_DATA * ch, FILE * fp));



/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj (CHAR_DATA * ch)
{
    char strsave[MAX_INPUT_LENGTH];
    FILE *fp;

    if (IS_NPC (ch))
        return;

    if (ch->desc != NULL && ch->desc->original != NULL)
        ch = ch->desc->original;

#if defined(unix)
    /* create god log */
    if (IS_IMMORTAL (ch) || ch->level >= LEVEL_IMMORTAL)
    {
        fclose (fpReserve);
        sprintf (strsave, "%s%s", GOD_DIR, capitalize (ch->name));
        if ((fp = fopen (strsave, "w")) == NULL)
        {
            bug ("Save_char_obj: fopen", 0);
            perror (strsave);
        }

        fprintf (fp, "Lev %2d Trust %2d  %s%s\n",
                 ch->level, get_trust (ch), ch->name, ch->pcdata->title);
        fclose (fp);
        fpReserve = fopen (NULL_FILE, "r");
    }
#endif

    fclose (fpReserve);
    sprintf (strsave, "%s%s", PLAYER_DIR, capitalize (IS_DISGUISED(ch)?ch->pcdata->disguise.orig_name:ch->name));
    if ((fp = fopen (TEMP_FILE, "w")) == NULL)
    {
        bug ("Save_char_obj: fopen", 0);
        perror (strsave);
    }
    else
    {
        fwrite_char (ch, fp);
        if (ch->carrying != NULL)
            fwrite_obj (ch, ch->carrying, fp, 0);
        /* save the pets */
  /*       if (ch->pet != NULL && ch->pet->in_room == ch->in_room)
            fwrite_pet (ch->pet, fp); */
        fprintf(fp, "#RoomRLE  ");
        fwrite_rle(ch->pcdata->explored, fp);
        fprintf(fp, "#MobRLE  ");
        fwrite_rle(ch->pcdata->mobbed, fp);
        fprintf (fp, "#END\n");
    }
    fclose (fp);
    rename (TEMP_FILE, strsave);
    fpReserve = fopen (NULL_FILE, "r");
    return;
}



/*
 * Write the char.
 */
void fwrite_char (CHAR_DATA * ch, FILE * fp)
{
    AFFECT_DATA *paf;
    int sn, gn, pos;
    int i;
    struct idName *names;

    fprintf (fp, "#%s\n", IS_NPC (ch) ? "MOB" : "PLAYER");

    fprintf (fp, "Name %s~\n", IS_DISGUISED(ch)?ch->pcdata->disguise.orig_name:ch->name);
    fprintf (fp, "Id   %ld\n", ch->id);
    fprintf (fp, "LogO %ld\n", current_time);
    fprintf (fp, "Vers %d\n", 6);
    if (ch->short_descr[0] != '\0')
        fprintf (fp, "ShD  %s~\n", IS_DISGUISED(ch)?ch->pcdata->disguise.orig_short:ch->short_descr);
    if (ch->long_descr[0] != '\0')
        fprintf (fp, "LnD  %s~\n", IS_DISGUISED(ch)?ch->pcdata->disguise.orig_long:ch->long_descr);
    if (ch->description[0] != '\0')
        fprintf (fp, "Desc %s~\n", IS_DISGUISED(ch)?ch->pcdata->disguise.orig_desc:ch->description);
    if (ch->prompt != NULL || !str_cmp (ch->prompt, "<%hhp %mm %vmv> <%T>")
        || !str_cmp (ch->prompt, "{c<%hhp %mm %vmv>{x "))
        fprintf (fp, "Prom %s~\n", ch->prompt);
    fprintf (fp, "Host %s~\n", IS_SET(ch->act2, PLR_NOIP) ? "Unknown" : IS_NULLSTR(ch->pcdata->Host) ? "Missing" : ch->pcdata->Host); 
    fprintf (fp, "Race %s~\n", pc_race_table[ch->race].name);
    fprintf (fp, "XpMultFac %d\n", ch->pcdata->xpmultiplier.factor);
    fprintf (fp, "XpMultTim %d\n", ch->pcdata->xpmultiplier.time);
    if (ch->clan > 0)
    {
        fprintf (fp, "Clan %s~\n", clan_table[ch->clan].name);
        fprintf (fp, "ClanN %d\n", ch->clan);
    }
    fprintf (fp, "Rank %d\n", ch->rank);
    fprintf (fp, "Sex  %d\n", ch->sex);
    fprintf (fp, "Cla  %d\n", ch->cClass);
    fprintf (fp, "Murd %d\n", ch->penalty.murder);
    fprintf (fp, "Thief %d\n", ch->penalty.thief);
    fprintf (fp, "Jail %d\n", ch->penalty.jail);
    fprintf (fp, "Punish %d\n", ch->penalty.punishment);
    fprintf (fp, "Levl %d\n", ch->level);
    fprintf (fp, "BarO %d\n", ch->pcdata->barOn ? 1 : 0);
    fprintf (fp, "MinO %d\n", ch->pcdata->isMinion ? 1 : 0);
    fprintf (fp, "FontN %s~\n", ch->pcdata->fonts.notes);
    fprintf (fp, "FontR %s~\n", ch->pcdata->fonts.rpnote);
    fprintf (fp, "Stance %d\n", ch->pcdata->stance);
    fprintf (fp, "TKills %d\n", ch->pcdata->tournament.kills);
    fprintf (fp, "TDeaths %d\n", ch->pcdata->tournament.deaths);
    fprintf (fp, "TRep %d\n", ch->pcdata->tournament.reputation);
    fprintf (fp, "TRank %d\n", ch->pcdata->tournament.rank);
    fprintf (fp, "TLRank %d\n", ch->pcdata->tournament.last_rank);    
    fprintf (fp, "TLKill %ld\n", ch->pcdata->tournament.last_killed);    
    fprintf (fp, "Forsaken %s~\n", ch->pcdata->forsaken_master);
   if (ch->trust != 0)
        fprintf (fp, "Tru  %d\n", ch->trust);
    fprintf (fp, "Sec  %d\n", ch->pcdata->security);    /* OLC */
    if (ch->questpoints != 0)
        fprintf( fp, "QuestPnts %ld\n",  ch->questpoints );
        fprintf( fp, "GameTick %d\n", ch->gametick );
        fprintf( fp, "BrkTit %s~\n", ch->plaid );
        fprintf( fp, "RpLevel %d\n",  ch->rplevel );
        fprintf( fp, "RpExp   %d\n",  ch->rpexp );
	fprintf( fp, "RlExp   %d\n",  ch->realexp );
        fprintf( fp, "Simskill  %s~\n",  ch->simskill );
	fprintf (fp, "Mapsz %d\n", ch->mapsize);
    if (ch->nextquest != 0)
        fprintf( fp, "QuestNext %d\n",  ch->nextquest   );
    else if (ch->countdown != 0)
        fprintf( fp, "QuestNext %d\n",  15              );
	fprintf(fp, "RP %d\n",ch->rp_points); /*RP POINTS*/

    fprintf (fp, "Plyd %d\n", ch->played + (int) (current_time -
ch->logon));
    fprintf (fp, "Not  %ld %ld %ld %ld %ld %ld %ld\n",
             ch->pcdata->last_note, ch->pcdata->last_idea,
             ch->pcdata->last_guildmaster, ch->pcdata->last_news,
             ch->pcdata->last_changes, ch->pcdata->last_imm,
             ch->pcdata->last_rp);
    fprintf (fp, "NoTG %ld\n", ch->pcdata->last_guide);
    fprintf (fp, "NoTS %ld\n", ch->pcdata->last_store);
    fprintf(fp, "Couple");
    for (i = 0; i < MAX_COUPLE; i++)
    {
      fprintf(fp, " %d %d",
              ch->pcdata->couple[i].cast,
              ch->pcdata->couple[i].couple);
    }
    fprintf(fp, "\n");                  

    fprintf(fp, "Weaves");
    for (i = 0; i < MAX_WEAVES; i++)
    {
      fprintf(fp, " %d",
              ch->pcdata->weaves[i]);
    }
    fprintf(fp, "\n");                  
              
    fprintf (fp, "Scro %d\n", ch->lines);
    fprintf (fp, "Fame %d\n", ch->fame);
    fprintf (fp, "Honor %d\n", ch->honor);
    fprintf (fp, "Update %d\n", ch->update);
    if (!IS_NULLSTR(ch->pretit))	
      fprintf (fp, "Pretitle %s~\n", ch->pretit);
    fprintf (fp, "Room %ld\n", (ch->in_room == get_room_index (ROOM_VNUM_LIMBO)
                               && ch->was_in_room != NULL)
             ? ch->was_in_room->vnum
             : ch->in_room == NULL ? 3001 : ch->in_room->vnum);

    fprintf (fp, "HMV  %ld %ld %d %d %d %d\n",
             ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move,
             ch->max_move);
    if (ch->gold > 0)
        fprintf (fp, "Gold %ld\n", ch->gold);
    else
        fprintf (fp, "Gold %d\n", 0);
    if (ch->silver > 0)
        fprintf (fp, "Silv %ld\n", ch->silver);
    else
        fprintf (fp, "Silv %d\n", 0);
    fprintf( fp, "GoldB %ld", ch->gold_in_bank);
    fprintf( fp, "SilvB %ld", ch->silver_in_bank);
 
    fprintf (fp, "Exp  %d\n", ch->exp);
    if (ch->act != 0)
        fprintf (fp, "Act  %s\n", print_flags (ch->act));
    if (ch->act2 != 0)
        fprintf (fp, "Act2  %s\n", print_flags (ch->act2));
    if (!STR_IS_ZERO(ch->affected_by, AFF_FLAGS) )
        fprintf (fp, "AfBy %s\n", str_print_flags (ch->affected_by, AFF_FLAGS));
    fprintf (fp, "Comm %s\n", print_flags (ch->comm));
    if (ch->wiznet)
        fprintf (fp, "Wizn %s\n", print_flags (ch->wiznet));
    if (ch->invis_level)
        fprintf (fp, "Invi %d\n", ch->invis_level);
    if (ch->incog_level)
        fprintf (fp, "Inco %d\n", ch->incog_level);
    fprintf (fp, "Pos  %d\n",
             ch->position == POS_FIGHTING ? POS_STANDING : ch->position);
    if (ch->practice != 0)
        fprintf (fp, "Prac %d\n", ch->practice);
    if (ch->train != 0)
        fprintf (fp, "Trai %d\n", ch->train);
    if (ch->saving_throw != 0)
        fprintf (fp, "Save  %d\n", ch->saving_throw);
    fprintf (fp, "Alig  %d\n", ch->alignment);
    if (ch->hitroll != 0)
        fprintf (fp, "Hit   %d\n", ch->hitroll);
    if (ch->damroll != 0)
        fprintf (fp, "Dam   %d\n", ch->damroll);
    fprintf (fp, "ACs %d %d %d %d\n",
             ch->armor[0], ch->armor[1], ch->armor[2], ch->armor[3]);
    if (ch->wimpy != 0)
        fprintf (fp, "Wimp  %d\n", ch->wimpy);
    fprintf (fp, "Attr %d %d %d %d %d\n",
             ch->perm_stat[STAT_STR],
             ch->perm_stat[STAT_INT],
             ch->perm_stat[STAT_WIS],
             ch->perm_stat[STAT_DEX], ch->perm_stat[STAT_CON]);

    fprintf (fp, "AMod %d %d %d %d %d\n",
             ch->mod_stat[STAT_STR],
             ch->mod_stat[STAT_INT],
             ch->mod_stat[STAT_WIS],
             ch->mod_stat[STAT_DEX], ch->mod_stat[STAT_CON]);

    if (IS_NPC (ch))
    {
        fprintf (fp, "Vnum %ld\n", ch->pIndexData->vnum);
    }
    else
    {
        fprintf (fp, "Pass %s~\n", ch->pcdata->pwd);
        if (ch->pcdata->bamfin[0] != '\0')
            fprintf (fp, "Bin  %s~\n", ch->pcdata->bamfin);
        if (ch->pcdata->bamfout[0] != '\0')
            fprintf (fp, "Bout %s~\n", ch->pcdata->bamfout);
        fprintf (fp, "Titl %s~\n", IS_DISGUISED(ch)?ch->pcdata->disguise.orig_title:ch->pcdata->title);
 
        fprintf (fp, "Pnts %d\n", ch->pcdata->points);
        fprintf (fp, "TSex %d\n", ch->pcdata->true_sex);
        fprintf (fp, "LLev %d\n", ch->pcdata->last_level);
        if (!IS_NULLSTR(ch->pcdata->account))
          fprintf (fp, "Account %s~\n", ch->pcdata->account);
        fprintf (fp, "HMVP %ld %d %d\n", ch->pcdata->perm_hit,
                 ch->pcdata->perm_mana, ch->pcdata->perm_move);
        fprintf (fp, "Cnd  %d %d %d %d\n",
                 ch->pcdata->condition[0],
                 ch->pcdata->condition[1],
                 ch->pcdata->condition[2], 
		 ch->pcdata->condition[3]);
		fprintf (fp,"BldCND %d\n", ch->pcdata->condition[4]);

       /*if (ch->pcdata->rentroom != 0)
       fprintf( fp, "Rroom %d\n",  ch->pcdata->rentroom  );    Room Rental */  
        
        /* write alias */
        for (pos = 0; pos < MAX_ALIAS; pos++)
        {
            if (ch->pcdata->alias[pos] == NULL
                || ch->pcdata->alias_sub[pos] == NULL)
                break;

            fprintf (fp, "Alias %s %s~\n", ch->pcdata->alias[pos],
                     ch->pcdata->alias_sub[pos]);
        }

        for (sn = 0; sn < MAX_SKILL; sn++)
        {
            if (skill_table[sn].name != NULL && ch->pcdata->learned[sn] > 0)
            {
                fprintf (fp, "Sk %d '%s'\n",
                         ch->pcdata->learned[sn], skill_table[sn].name);
            }
        }

        for (gn = 0; gn < MAX_GROUP; gn++)
        {
            if (group_table[gn].name != NULL && ch->pcdata->group_known[gn])
            {
                fprintf (fp, "Gr '%s'\n", group_table[gn].name);
            }
        }
    }

    fprintf( fp, "Granted %s~\n", ch->pcdata->granted );
    fprintf( fp, "Bond %s~\n", ch->pcdata->bond );

    for (paf = ch->affected; paf != NULL; paf = paf->next)
    {
        if (paf->type < 0 || paf->type >= MAX_SKILL)
            continue;

        fprintf (fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
                 skill_table[paf->type].name,
                 paf->where,
                 paf->level,
                 paf->duration, paf->modifier, paf->location, paf->bitvector);
    }

    for( names = ch->pcdata->names; names; names = names->next )
    {
	fprintf( fp, "Known %ld %s~\n", names->id, names->name );
    }
        
    for (QUEST_DATA *quest = ch->pcdata->quests; quest != NULL; quest = quest->next)
    {
      fprintf(fp, "Quest %d %d %s\n", quest->number, quest->status, print_flags(quest->bit));
    }

    fprintf (fp, "End\n\n");
    return;
}

/* write a pet */
void fwrite_pet (CHAR_DATA * pet, FILE * fp)
{
    AFFECT_DATA *paf;

    fprintf (fp, "#PET\n");

    fprintf (fp, "Vnum %ld\n", pet->pIndexData->vnum);

    fprintf (fp, "Name %s~\n", pet->name);
    fprintf (fp, "LogO %ld\n", current_time);
    if (pet->short_descr != pet->pIndexData->short_descr)
        fprintf (fp, "ShD  %s~\n", pet->short_descr);
    if (pet->long_descr != pet->pIndexData->long_descr)
        fprintf (fp, "LnD  %s~\n", pet->long_descr);
    if (pet->description != pet->pIndexData->description)
        fprintf (fp, "Desc %s~\n", pet->description);
    if (pet->race != pet->pIndexData->race)
        fprintf (fp, "Race %s~\n", race_table[pet->race].name);
    if (pet->clan)
        fprintf (fp, "Clan %s~\n", clan_table[pet->clan].name);
    fprintf (fp, "Sex  %d\n", pet->sex);
    if (pet->level != pet->pIndexData->level)
        fprintf (fp, "Levl %d\n", pet->level);
    fprintf (fp, "HMV  %ld %ld %d %d %d %d\n",
             pet->hit, pet->max_hit, pet->mana, pet->max_mana, pet->move,
             pet->max_move);
    if (pet->gold > 0)
        fprintf (fp, "Gold %ld\n", pet->gold);
    if (pet->silver > 0)
        fprintf (fp, "Silv %ld\n", pet->silver);
    if (pet->exp > 0)
        fprintf (fp, "Exp  %d\n", pet->exp);
    if (pet->act != pet->pIndexData->act)
        fprintf (fp, "Act  %s\n", print_flags (pet->act));
    if (!STR_SAME_STR(pet->affected_by, pet->pIndexData->affected_by, AFF_FLAGS))
        fprintf (fp, "AfBy %s\n", str_print_flags (pet->affected_by, AFF_FLAGS));
    if (pet->comm != 0)
        fprintf (fp, "Comm %s\n", print_flags (pet->comm));
    fprintf (fp, "Pos  %d\n", pet->position =
             POS_FIGHTING ? POS_STANDING : pet->position);
    if (pet->saving_throw != 0)
        fprintf (fp, "Save %d\n", pet->saving_throw);
    if (pet->alignment != pet->pIndexData->alignment)
        fprintf (fp, "Alig %d\n", pet->alignment);
    if (pet->hitroll != pet->pIndexData->hitroll)
        fprintf (fp, "Hit  %d\n", pet->hitroll);
    if (pet->damroll != pet->pIndexData->damage[DICE_BONUS])
        fprintf (fp, "Dam  %d\n", pet->damroll);
    fprintf (fp, "ACs  %d %d %d %d\n",
             pet->armor[0], pet->armor[1], pet->armor[2], pet->armor[3]);
    fprintf (fp, "Attr %d %d %d %d %d\n",
             pet->perm_stat[STAT_STR], pet->perm_stat[STAT_INT],
             pet->perm_stat[STAT_WIS], pet->perm_stat[STAT_DEX],
             pet->perm_stat[STAT_CON]);
    fprintf (fp, "AMod %d %d %d %d %d\n",
             pet->mod_stat[STAT_STR], pet->mod_stat[STAT_INT],
             pet->mod_stat[STAT_WIS], pet->mod_stat[STAT_DEX],
             pet->mod_stat[STAT_CON]);

    for (paf = pet->affected; paf != NULL; paf = paf->next)
    {
        if (paf->type < 0 || paf->type >= MAX_SKILL)
            continue;

        fprintf (fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
                 skill_table[paf->type].name,
                 paf->where, paf->level, paf->duration, paf->modifier,
                 paf->location, paf->bitvector);
    }

    fprintf (fp, "End\n");
    return;
}

/*
 * Write an object and its contents.
 */
void fwrite_obj (CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest)
{
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *paf;

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if (obj->next_content != NULL)
        fwrite_obj (ch, obj->next_content, fp, iNest);

    /*
     * Castrate storage characters.
     */
/*    if ((ch->level < obj->level - 2 && obj->item_type != ITEM_CONTAINER)
        || obj->item_type == ITEM_KEY
        || (obj->item_type == ITEM_MAP && !obj->value[0]))
        return;
*/

    fprintf (fp, "#O\n");
    fprintf (fp, "Vnum %ld\n", obj->pIndexData->vnum);
    if (obj->owner)
      fprintf (fp, "Owner %s~", obj->owner);  
    fprintf (fp, "ID %ld\n", obj->id);
    fprintf (fp, "Enchants %d\n", obj->enchants);
    fprintf (fp, "PK %d\n", obj->pk ? 1 : 0); 
    if (!obj->pIndexData->new_format)
        fprintf (fp, "Oldstyle\n");
    if (obj->enchanted)
        fprintf (fp, "Enchanted\n");
    //if (obj->spellaffected)
    //    fprintf (fp, "Spellaffected\n");
    fprintf (fp, "Nest %d\n", iNest);

    /* these data are only used if they do not match the defaults */

    if (obj->name != obj->pIndexData->name)
        fprintf (fp, "Name %s~\n", obj->name);
    if (obj->short_descr != obj->pIndexData->short_descr)
        fprintf (fp, "ShD  %s~\n", obj->short_descr);
    if (obj->description != obj->pIndexData->description)
        fprintf (fp, "Desc %s~\n", obj->description);
    if (obj->extra_flags != obj->pIndexData->extra_flags)
        fprintf (fp, "ExtF %d\n", obj->extra_flags);
    if (obj->wear_flags != obj->pIndexData->wear_flags)
        fprintf (fp, "WeaF %d\n", obj->wear_flags);
    if (obj->class_flags != obj->pIndexData->class_flags)
        fprintf (fp, "ClaF %d\n", obj->class_flags);
    if (obj->item_type != obj->pIndexData->item_type)
        fprintf (fp, "Ityp %d\n", obj->item_type);
    if (obj->weight != obj->pIndexData->weight)
        fprintf (fp, "Wt   %d\n", obj->weight);
    if (obj->condition != obj->pIndexData->condition)
        fprintf (fp, "Cond %d\n", obj->condition);

    /* variable data */

    fprintf (fp, "Wear %d\n", obj->wear_loc);
    if (obj->level != obj->pIndexData->level)
        fprintf (fp, "Lev  %d\n", obj->level);
    if (obj->honor_level != obj->pIndexData->honor_level)
        fprintf (fp, "HonLev  %d\n", obj->honor_level);
    if (obj->timer != 0)
        fprintf (fp, "Time %d\n", obj->timer);
    fprintf (fp, "Cost %d\n", obj->cost);
    if (obj->value[0] != obj->pIndexData->value[0]
        || obj->value[1] != obj->pIndexData->value[1]
        || obj->value[2] != obj->pIndexData->value[2]
        || obj->value[3] != obj->pIndexData->value[3]
        || obj->value[4] != obj->pIndexData->value[4])
        fprintf (fp, "Val  %d %d %d %d %d\n",
                 obj->value[0], obj->value[1], obj->value[2], obj->value[3],
                 obj->value[4]);

    switch (obj->item_type)
    {
        case ITEM_POTION:
        case ITEM_SCROLL:
        case ITEM_PILL:
            if (obj->value[1] > 0)
            {
                fprintf (fp, "Spell 1 '%s'\n",
                         skill_table[obj->value[1]].name);
            }

            if (obj->value[2] > 0)
            {
                fprintf (fp, "Spell 2 '%s'\n",
                         skill_table[obj->value[2]].name);
            }

            if (obj->value[3] > 0)
            {
                fprintf (fp, "Spell 3 '%s'\n",
                         skill_table[obj->value[3]].name);
            }

            break;

        case ITEM_STAFF:
        case ITEM_WAND:
            if (obj->value[3] > 0)
            {
                fprintf (fp, "Spell 3 '%s'\n",
                         skill_table[obj->value[3]].name);
            }

            break;
    }

    for (paf = obj->affected; paf != NULL; paf = paf->next)
    {
    	
        if (paf->type < 0 || paf->type >= MAX_SKILL || !obj->enchanted)  
            continue;
        
        fprintf (fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
                 skill_table[paf->type].name,
                 paf->where,
                 paf->level,
                 paf->duration, paf->modifier, paf->location, paf->bitvector);
    }

    for (ed = obj->extra_descr; ed != NULL; ed = ed->next)
    {
        fprintf (fp, "ExDe %s~ %s~\n", ed->keyword, ed->description);
    }

    fprintf (fp, "End\n\n");

    if (obj->contains != NULL)
        fwrite_obj (ch, obj->contains, fp, iNest + 1);

    return;
}

/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj (DESCRIPTOR_DATA * d, char *name)
{
    char strsave[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *ch;
    FILE *fp;
    bool found;
    int stat, i;

    ch = new_char ();
    ch->pcdata = new_pcdata ();

    d->character = ch;
    ch->desc = d;
    ch->name = str_dup (name);
    ch->mount = NULL;
    ch->ireply = NULL;
    ch->oreply = NULL;

    ch->is_mounted = FALSE;
    ch->id = get_pc_id ();
    ch->race = race_lookup ("human");
    ch->act = PLR_NOSUMMON;
    SET_BIT(ch->act, PLR_AUTOEXIT);
    SET_BIT(ch->act, PLR_AUTOLOOT);
    SET_BIT(ch->act, PLR_AUTOSAC);
    SET_BIT(ch->act, PLR_AUTOGOLD);
    SET_BIT(ch->act, PLR_AUTOSPLIT);
    ch->comm = COMM_COMBINE | COMM_PROMPT;
    ch->prompt = str_dup ("<%hhp %mm %vmv> <%T>");
    ch->pretit = str_dup("");
    sprintf(ch->pcdata->explored, " ");
    ch->pcdata->primary = TRUE;
    ch->pcdata->damInflicted = 0;
    ch->pcdata->damReceived = 0;
    ch->pcdata->xpmultiplier.on = FALSE;
    ch->pcdata->xpmultiplier.factor = 0;
    ch->pcdata->confirm_delete = FALSE;
    ch->pcdata->confirm_pkon = FALSE;
    ch->pcdata->pwd = str_dup ("");
    ch->pcdata->bamfin = str_dup ("");
    ch->pcdata->bamfout = str_dup ("");
    ch->pcdata->account = str_dup("");
    ch->pcdata->title = str_dup ("");
    ch->pcdata->questroom = str_dup ("");
    ch->pcdata->questarea = str_dup ("");
    ch->pcdata->client = str_dup ("");
    ch->pcdata->clientVersion = str_dup ("");
    ch->pcdata->mxpVersion = str_dup ("");
    ch->pcdata->bond = str_dup ("");
    ch->pcdata->fonts.rpnote = str_dup ("lucida handwriting");
    ch->pcdata->fonts.notes = str_dup ("lucida handwriting");
    ch->pcdata->manadrainee = str_dup ("");
    ch->pcdata->last_guide = 0;
    ch->pcdata->last_store = 0;
    ch->pcdata->initiator = 0;
    for (stat = 0; stat < MAX_STATS; stat++)
        ch->perm_stat[stat] = 13;
    ch->pcdata->condition[COND_THIRST] = 48;
    ch->pcdata->condition[COND_FULL] = 48;
    ch->pcdata->condition[COND_HUNGER] = 48;
    ch->pcdata->condition[COND_GOBLIND] = 0;
    ch->pcdata->condition[COND_HORNY] = 30;
    ch->pcdata->stance                  = 0;
	ch->pcdata->condition[COND_BLEEDING] = 0;
    ch->pcdata->names = NULL;
    for (i = 0; i < MAX_COUPLE; i++)
    {
      ch->pcdata->couple[i].cast = 0;
      ch->pcdata->couple[i].couple = 0;
    }

    for (i = 0; i < MAX_WEAVES; i++)
    {
      ch->pcdata->weaves[i] = 2;
    }
    ch->mapsize = 15;
    ch->pcdata->security = 0;    /* OLC */
    ch->pcdata->granted = str_dup("none");
    ch->pcdata->forsaken_master = str_dup("");
    ch->pcdata->ignore = str_dup("");
    ch->pcdata->isMinion = FALSE;
    if (d->mxp)
      SET_BIT(ch->act2, PLR_MXP);

    ch->pcdata->barOn = TRUE;
    ch->pcdata->tournament.reputation = 0;
    ch->pcdata->tournament.deaths = 0;
    ch->pcdata->tournament.kills = 0;
    ch->pcdata->tournament.rank = 0;
    ch->pcdata->tournament.last_rank = 0;
    ch->pcdata->tournament.last_killed = 0;


    found = FALSE;
    fclose (fpReserve);

#if defined(unix)
    /* decompress if .gz file exists */
    sprintf (strsave, "%s%s%s", PLAYER_DIR, capitalize (name), ".gz");
    if ((fp = fopen (strsave, "r")) != NULL)
    {
        fclose (fp);
        sprintf (buf, "gzip -dfq %s", strsave);
        system (buf);
    }
#endif

    sprintf (strsave, "%s%s", PLAYER_DIR, capitalize (name));
    if ((fp = fopen (strsave, "r")) != NULL)
    {
        int iNest;

        for (iNest = 0; iNest < MAX_NEST; iNest++)
            rgObjNest[iNest] = NULL;

        found = TRUE;
        for (;;)
        {
            char letter;
            char *word;

            letter = fread_letter (fp);
            if (letter == '*')
            {
                fread_to_eol (fp);
                continue;
            }

            if (letter != '#')
            {
                bug ("Load_char_obj: # not found.", 0);
                break;
            }

            word = fread_word (fp);
            if (!str_cmp (word, "PLAYER"))
                fread_char (ch, fp);
            else if (!str_cmp (word, "OBJECT"))
                fread_obj (ch, fp);
            else if (!str_cmp (word, "O"))
                fread_obj (ch, fp);
            else if (!str_cmp (word, "RoomRLE"))
                fread_rle (ch->pcdata->explored, fp);
            else if (!str_cmp (word, "MobRLE"))
                fread_rle (ch->pcdata->mobbed, fp);
            else if (!str_cmp (word, "PET"))
            {
	           //      fread_pet (ch, fp); REMOVED TO TEMP FIX BUG
               continue;
             }
            else if (!str_cmp (word, "END"))
                break;
            else
            {
                bug ("Load_char_obj: bad section.", 0);
                break;
            }
        }
        fclose (fp);
    }

    fpReserve = fopen (NULL_FILE, "r");


    /* initialize race */
    if (found)
    {
        int i;

        if (ch->race == 0)
            ch->race = race_lookup ("human");

        ch->size = pc_race_table[ch->race].size;
        ch->dam_type = 17;        /*punch */

        for (i = 0; i < 5; i++)
        {
            if (pc_race_table[ch->race].skills[i] == NULL)
                break;
            group_add (ch, pc_race_table[ch->race].skills[i], FALSE);
        }

        STR_OR_STR( ch->affected_by, race_table[ch->race].aff, AFF_FLAGS);
 
        ch->imm_flags = ch->imm_flags | race_table[ch->race].imm;
        ch->res_flags = ch->res_flags | race_table[ch->race].res;
        ch->vuln_flags = ch->vuln_flags | race_table[ch->race].vuln;
        ch->form = race_table[ch->race].form;
        ch->parts = race_table[ch->race].parts;
    }


    /* RT initialize skills */

    if (found && ch->version < 2)
    {                            /* need to add the new skills */
        group_add (ch, "rom basics", FALSE);
        group_add (ch, class_table[ch->cClass].base_group, FALSE);
        group_add (ch, class_table[ch->cClass].default_group, TRUE);
        ch->pcdata->learned[gsn_recall] = 50;
    }

    /* fix levels */
    if (found && ch->version < 3 && (ch->level > 35 || ch->trust > 35))
    {
        switch (ch->level)
        {
            case (40):
                ch->level = 60;
                break;            /* imp -> imp */
            case (39):
                ch->level = 58;
                break;            /* god -> supreme */
            case (38):
                ch->level = 56;
                break;            /* deity -> god */
            case (37):
                ch->level = 53;
                break;            /* angel -> demigod */
        }

        switch (ch->trust)
        {
            case (40):
                ch->trust = 60;
                break;            /* imp -> imp */
            case (39):
                ch->trust = 58;
                break;            /* god -> supreme */
            case (38):
                ch->trust = 56;
                break;            /* deity -> god */
            case (37):
                ch->trust = 53;
                break;            /* angel -> demigod */
            case (36):
                ch->trust = 51;
                break;            /* hero -> hero */
        }
    }

    /* ream gold */
    if (found && ch->version < 4)
    {
        ch->gold /= 100;
    }
    if (!d->mxp)
      REMOVE_BIT(ch->act2, PLR_MXP); 

    return found;
}



/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                    \
                if ( !str_cmp( word, literal ) )    \
                {                    \
                    field  = value;            \
                    fMatch = TRUE;            \
                    break;                \
                }

#if defined(KEYF)
#undef KEYF
#endif

#define KEYF( literal, field, value, size) if (!str_cmp(word, literal)) \
	{ STR_COPY_STR(field, value, size); fMatch = TRUE; break;}

/* provided to free strings */
#if defined(KEYS)
#undef KEYS
#endif


#define KEYS( literal, field, value )                    \
                if ( !str_cmp( word, literal ) )    \
                {                    \
                    free_string(field);            \
                    field  = value;            \
                    fMatch = TRUE;            \
                    break;                \
                }

bool load_account_obj (DESCRIPTOR_DATA * d, char *name)
{
  FILE *fp;
  char strsave[MIL];
  char *word;
  bool fMatch = FALSE;
  ACCOUNT_DATA *account;

  account = new_account();
  account->name = str_dup(name);
  account->password = NULL;
  account->host = str_dup("");
  account->player = NULL;

  d->account = account;

  sprintf(strsave, "%s%s", ACCOUNT_DIR, capitalize (name));
  if ((fp = fopen (strsave, "r")) == NULL)
  {
    return FALSE;
  }

  word = fread_word(fp);
  if (!str_cmp(word, "#End"))
  {
     send_to_desc("That account is corrupted talk to immortal staff.\n\r", d);
     fclose(fp);
     return FALSE;
  }

  account->version = fread_number(fp);
//  logf("New Account", 0);
  for ( ;  ; )
  {
    fMatch = FALSE;
    word = fread_word(fp);
//    logf("Word %s", word);
    switch (word[0])
    {
      case 'A':
//        logf("Case A", 0);
	KEY ("AccountName", account->name, fread_string(fp));
	break;
      case 'P':
//        logf("Case P", 0);
	KEY ("Password", account->password, fread_string(fp));
        break;
      case 'H':
        KEYS("Host", account->host, fread_string(fp));
        break;

      case '#':
        if (!str_cmp(word, "#End"))
	{
	  fclose(fp);
	  account->next = account_list;
	  account_list = account;
	  return TRUE;  
        }
	
	if (!str_cmp(word, "#Player"))
	{
	  ACCT_PLR_DATA *player;
//          logf("Player",0);
	  player = new_acct_plr();
	  player->name = fread_string(fp);
	  player->password = fread_number(fp);
	  player->primary = fread_number(fp);
	  player->next = account->player;
	  account->player = player;
	  fMatch = TRUE;
	  break;
        }
        break;

      default:
	if (!fMatch)
	{
	  logf("Load_acct_obj %s not found.", word);
	  break;
	}
        break;
    }
  }
  return TRUE;
}


void fread_char (CHAR_DATA * ch, FILE * fp)
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    int count = 0;
    int lastlogoff = current_time;
    int percent;

    sprintf (buf, "Loading %s.", ch->name);
    log_string (buf);

    for (;;)
    {
        word = feof (fp) ? str_dup("End") : fread_word (fp);
        fMatch = FALSE;

        switch (UPPER (word[0]))
        {
            case '*':
                fMatch = TRUE;
                fread_to_eol (fp);
                break;

            case 'A':
                KEYS ("Account", ch->pcdata->account, fread_string(fp));
                KEY ("Act", ch->act, fread_flag (fp));
                KEY ("Act2", ch->act2, fread_flag (fp));
                KEY ("Alignment", ch->alignment, fread_number (fp));
                KEY ("Alig", ch->alignment, fread_number (fp));

                if ( ch->version < 6)
                {
                   KEYF("AffectedBy", ch->affected_by, 
		     aff_convert_fread_flag( fread_flag(fp )), AFF_FLAGS); 
                   KEYF("AfBy", ch->affected_by, 
		     aff_convert_fread_flag( fread_flag(fp )), AFF_FLAGS); 
                }
		else
                {
		  KEYF("AfBy", ch->affected_by, str_fread_flag(fp, AFF_FLAGS),
                               AFF_FLAGS );   
                }

 
                if (!str_cmp (word, "Alia"))
                {
                    if (count >= MAX_ALIAS)
                    {
                        fread_to_eol (fp);
                        fMatch = TRUE;
                        break;
                    }

                    ch->pcdata->alias[count] = str_dup (fread_word (fp));
                    ch->pcdata->alias_sub[count] = str_dup (fread_word (fp));
                    count++;
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "Alias"))
                {
                    if (count >= MAX_ALIAS)
                    {
                        fread_to_eol (fp);
                        fMatch = TRUE;
                        break;
                    }

                    ch->pcdata->alias[count] = str_dup (fread_word (fp));
                    ch->pcdata->alias_sub[count] = fread_string (fp);
                    count++;
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "AC") || !str_cmp (word, "Armor"))
                {
                    fread_to_eol (fp);
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "ACs"))
                {
                    int i;

                    for (i = 0; i < 4; i++)
                        ch->armor[i] = fread_number (fp);
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "AffD"))
                {
                    AFFECT_DATA *paf;
                    int sn;

                    paf = new_affect ();

                    sn = skill_lookup (fread_word (fp));
                    if (sn < 0)
                        bug ("Fread_char: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->level = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->location = fread_number (fp);
                    paf->bitvector = fread_number (fp);
                    paf->next = ch->affected;
                    ch->affected = paf;
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "Affc"))
                {
                    AFFECT_DATA *paf;
                    int sn;

                    paf = new_affect ();

                    sn = skill_lookup (fread_word (fp));
                    if (sn < 0)
                        bug ("Fread_char: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->where = fread_number (fp);
                    paf->level = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->location = fread_number (fp);
                    paf->bitvector = fread_number (fp);
                    paf->next = ch->affected;
                    ch->affected = paf;
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "AttrMod") || !str_cmp (word, "AMod"))
                {
                    int stat;
                    for (stat = 0; stat < MAX_STATS; stat++)
                        ch->mod_stat[stat] = fread_number (fp);
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "AttrPerm") || !str_cmp (word, "Attr"))
                {
                    int stat;

                    for (stat = 0; stat < MAX_STATS; stat++)
                        ch->perm_stat[stat] = fread_number (fp);
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'B':
                KEY ("Bamfin", ch->pcdata->bamfin, fread_string (fp));
                KEY ("Bamfout", ch->pcdata->bamfout, fread_string (fp));
                KEY ("Bin", ch->pcdata->bamfin, fread_string (fp));
                KEY ("Bout", ch->pcdata->bamfout, fread_string (fp));
                KEY ("BrkTit", ch->plaid, fread_string (fp));
                KEY ("Bond", ch->pcdata->bond, fread_string (fp));
                if (!str_cmp(word, "BarO"))
                {
                  int temp;
                  temp = fread_number(fp);
                  ch->pcdata->barOn = TRUE;
                  if (temp == 0)
                    ch->pcdata->barOn = FALSE;
                  fMatch = TRUE;
                }
				KEY ("BldCND", ch->pcdata->condition[COND_BLEEDING], fread_number (fp));
                break;

            case 'C':
                KEY ("Crim", ch->penalty.thief, fread_number (fp));
                KEY ("Class", ch->cClass, fread_number (fp));
                KEY ("Cla", ch->cClass, fread_number (fp));
                KEY ("Comm", ch->comm, fread_flag (fp));
                if ( !str_cmp( word, "Couple" )) {
                  int i;
                  for (i = 0;i < MAX_COUPLE;i++)
                  {
                    ch->pcdata->couple[i].cast = fread_number(fp);
                    ch->pcdata->couple[i].couple = fread_number(fp);
                  }
                  fMatch = TRUE;
                  break;
                }

                if ( !str_cmp( word, "Clan" ) ) {
    		  char *tmp = fread_string(fp);
    		  ch->clan = clan_lookup(tmp);
    		  free_string(tmp);
    		  fMatch = TRUE;
    		  break;
		}

                if ( !str_cmp( word, "ClanN" ) ) {
    		  ch->clan = fread_number(fp);
    		  fMatch = TRUE;
    		  break;
		}

                if (!str_cmp (word, "Condition") || !str_cmp (word, "Cond"))
                {
                    ch->pcdata->condition[COND_DRUNK] = fread_number (fp);
                    ch->pcdata->condition[COND_FULL] = fread_number (fp);
                    ch->pcdata->condition[COND_THIRST] = fread_number (fp);
					ch->pcdata->condition[COND_HUNGER] = fread_number (fp);
                    fMatch = TRUE;
                    break;
                }
                if (!str_cmp (word, "Cnd"))
                {
                    ch->pcdata->condition[COND_DRUNK] = fread_number (fp);
                    ch->pcdata->condition[COND_FULL] = fread_number (fp);
                    ch->pcdata->condition[COND_THIRST] = fread_number (fp);
                    ch->pcdata->condition[COND_HUNGER] = fread_number (fp);
					
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "Coloura"))
                {
                    LOAD_COLOUR (text)
                        LOAD_COLOUR (auction)
                        LOAD_COLOUR (gossip)
                        LOAD_COLOUR (music)
                        LOAD_COLOUR (question) fMatch = TRUE;
                    break;
                }
                if (!str_cmp (word, "Colourb"))
                {
                    LOAD_COLOUR (answer)
                        LOAD_COLOUR (quote)
                        LOAD_COLOUR (quote_text)
                        LOAD_COLOUR (immtalk_text)
                        LOAD_COLOUR (immtalk_type) fMatch = TRUE;
                    break;
                }
                if (!str_cmp (word, "Colourc"))
                {
                    LOAD_COLOUR (info)
                        LOAD_COLOUR (tell)
                        LOAD_COLOUR (reply)
                        LOAD_COLOUR (gtell_text)
                        LOAD_COLOUR (gtell_type) fMatch = TRUE;
                    break;
                }
                if (!str_cmp (word, "Colourd"))
                {
                    LOAD_COLOUR (room_title)
                        LOAD_COLOUR (room_text)
                        LOAD_COLOUR (room_exits)
                        LOAD_COLOUR (room_things)
                        LOAD_COLOUR (prompt) fMatch = TRUE;
                    break;
                }
                if (!str_cmp (word, "Coloure"))
                {
                    LOAD_COLOUR (fight_death)
                        LOAD_COLOUR (fight_yhit)
                        LOAD_COLOUR (fight_ohit)
                        LOAD_COLOUR (fight_thit)
                        LOAD_COLOUR (fight_skill) fMatch = TRUE;
                    break;
                }
                if (!str_cmp (word, "Colourf"))
                {
                    LOAD_COLOUR (wiznet)
                        LOAD_COLOUR (say)
                        LOAD_COLOUR (say_text)
                        LOAD_COLOUR (tell_text)
                        LOAD_COLOUR (reply_text) fMatch = TRUE;
                    break;
                }
                if (!str_cmp (word, "Colourg"))
                {
                    LOAD_COLOUR (auction_text)
                        LOAD_COLOUR (gossip_text)
                        LOAD_COLOUR (music_text)
                        LOAD_COLOUR (question_text)
                        LOAD_COLOUR (answer_text) fMatch = TRUE;
                    break;
                }

                break;

            case 'D':
                KEY ("Damroll", ch->damroll, fread_number (fp));
                KEY ("Dam", ch->damroll, fread_number (fp));
                KEY ("Description", ch->description, fread_string (fp));
                KEY ("Desc", ch->description, fread_string (fp));
                break;

            case 'E':
                if (!str_cmp (word, "End"))
                {
                    /* adjust hp mana move up  -- here for speed's sake */
                    percent =
                        (current_time - lastlogoff) * 25 / (2 * 60 * 60);

                    percent = UMIN (percent, 100);

                    if (percent > 0 && !IS_AFFECTED (ch, AFF_POISON)
                        && !IS_AFFECTED (ch, AFF_PLAGUE))
                    {
                        ch->hit += (ch->max_hit - ch->hit) * percent / 100;
                        ch->mana += (ch->max_mana - ch->mana) * percent / 100;
                        ch->move += (ch->max_move - ch->move) * percent / 100;
                    }
                    return;
                }
                KEY ("Exp", ch->exp, fread_number (fp));
                break;

            case 'F':
                KEY ("Forsaken", ch->pcdata->forsaken_master, fread_string (fp));
		KEY ("Fame", ch->fame, fread_number (fp));
		KEY ("FontN", ch->pcdata->fonts.notes, fread_string (fp));
		KEY ("FontR", ch->pcdata->fonts.rpnote, fread_string (fp));
		break;

            case 'G':
                KEY ("GameTick", ch->gametick, fread_number (fp));
                KEY ("Gold", ch->gold, fread_number (fp));
                KEY ("GoldB", ch->gold_in_bank, fread_number (fp));
                if (!str_cmp (word, "Group") || !str_cmp (word, "Gr"))
                {
                    int gn;
                    char *temp;

                    temp = fread_word (fp);
                    gn = group_lookup (temp);
                    /* gn    = group_lookup( fread_word( fp ) ); */
                    if (gn < 0)
                    {
                        fprintf (stderr, "%s", temp);
                        bug ("Fread_char: unknown group. ", 0);
                    }
                    else
                        gn_add (ch, gn);
                    fMatch = TRUE;
                }
                KEY ("Granted", ch->pcdata->granted, fread_string (fp));
                break;

           

            case 'H':
                KEY ("Hitroll", ch->hitroll, fread_number (fp));
                KEY ("Hit", ch->hitroll, fread_number (fp));
                if (!str_cmp (word, "Host"))
                {
                  fread_string(fp);
                  fMatch = TRUE;
                  break;
                }

  
                if (!str_cmp (word, "HpManaMove") || !str_cmp (word, "HMV"))
                {
                    ch->hit = fread_long (fp);
                    ch->max_hit = fread_long (fp);
                    ch->mana = fread_number (fp);
                    ch->max_mana = fread_number (fp);
                    ch->move = fread_number (fp);
                    ch->max_move = fread_number (fp);
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "HpManaMovePerm")
                    || !str_cmp (word, "HMVP"))
                {
                    ch->pcdata->perm_hit = fread_number (fp);
                    ch->pcdata->perm_mana = fread_number (fp);
                    ch->pcdata->perm_move = fread_number (fp);
                    fMatch = TRUE;
                    break;
                }
                KEY ("Honor", ch->honor, fread_number (fp));
                if (ch->honor < 0){ch->honor = 0;}
                break;

            case 'I':
                KEY ("Id", ch->id, fread_number (fp));
                KEY ("InvisLevel", ch->invis_level, fread_number (fp));
                KEY ("Inco", ch->incog_level, fread_number (fp));
                KEY ("Invi", ch->invis_level, fread_number (fp));
                break;

            case 'J':
                KEY ("Jail", ch->penalty.jail, fread_number (fp));
                break;

            case 'K':
	      if ( !str_cmp( word, "Know" ) )
              {
	        int id;
                char *temp;
    
                id = fread_number(fp);
  	        temp = fread_word(fp);
	        addKnow( ch, id, temp );
	        fMatch = TRUE;
                break;
              }

	      if ( !str_cmp( word, "Known" ) )
              {
	        int id;
                char *temp;
    
                id = fread_number(fp);
  	        temp = fread_string(fp);
	        addKnow( ch, id, temp );
	        fMatch = TRUE;
                break;
              }
	      break;

            case 'L':
                KEY ("LastLevel", ch->pcdata->last_level, fread_number (fp));
                KEY ("LLev", ch->pcdata->last_level, fread_number (fp));
                KEY ("Level", ch->level, fread_number (fp));
                KEY ("Lev", ch->level, fread_number (fp));
                KEY ("Levl", ch->level, fread_number (fp));
                KEY ("LogO", lastlogoff, fread_number (fp));
                KEY ("LongDescr", ch->long_descr, fread_string (fp));
                KEY ("LnD", ch->long_descr, fread_string (fp));
                break;

            case 'M':
		KEY ("Mapsz", ch->mapsize, fread_number (fp));
                KEY ("Murd", ch->penalty.murder, fread_number (fp));
                if (!str_cmp(word, "MinO"))
                {
                  int temp;
                  temp = fread_number(fp);
                  ch->pcdata->isMinion = TRUE;
                  if (temp == 0)
                    ch->pcdata->isMinion = FALSE;
                  fMatch = TRUE;
                }    
                break;

            case 'N':
                KEYS ("Name", ch->name, fread_string (fp));
                KEY ("Note", ch->pcdata->last_note, fread_number (fp));
                if (!str_cmp (word, "Not"))
                {
                    ch->pcdata->last_note = fread_number (fp);
                    ch->pcdata->last_idea = fread_number (fp);
                    ch->pcdata->last_guildmaster = fread_number (fp);
                    ch->pcdata->last_news = fread_number (fp);
                    ch->pcdata->last_changes = fread_number (fp);
                    ch->pcdata->last_imm = fread_number(fp);
                    ch->pcdata->last_rp = fread_number(fp);
                    fMatch = TRUE;
                    break;
                }
		if (!str_cmp (word, "NotG"))
		{
                    ch->pcdata->last_guide = fread_number(fp);
                    fMatch = TRUE;
                    break;
		}		
		if (!str_cmp (word, "NotS"))
		{
                    ch->pcdata->last_store = fread_number(fp);
                    fMatch = TRUE;
                    break;
		}		
                break;

            case 'P':
                KEY ("Pretitle", ch->pretit, fread_string (fp));
                KEY ("Password", ch->pcdata->pwd, fread_string (fp));
                KEY ("Pass", ch->pcdata->pwd, fread_string (fp));
                KEY ("Played", ch->played, fread_number (fp));
                KEY ("Plyd", ch->played, fread_number (fp));
                KEY ("Points", ch->pcdata->points, fread_number (fp));
                KEY ("Pnts", ch->pcdata->points, fread_number (fp));
                KEY ("Position", ch->position, fread_number (fp));
                KEY ("Pos", ch->position, fread_number (fp));
                KEY ("Punish", ch->penalty.punishment, fread_number(fp));
                KEY ("Practice", ch->practice, fread_number (fp));
                KEY ("Prac", ch->practice, fread_number (fp));
                KEYS ("Prompt", ch->prompt, fread_string (fp));
                KEYS ("Prom", ch->prompt, fread_string (fp));
                break;
            case 'Q':
            KEY( "QuestPnts",   ch->questpoints,        fread_number( fp) );
            KEY( "QuestNext",   ch->nextquest,          fread_number( fp) );
            if (!str_cmp(word, "quest"))
                {
                  QUEST_DATA *quest = new QUEST_DATA;
                  quest->number = fread_number(fp);
                  quest->status = fread_number(fp);
                  quest->bit = fread_flag(fp);
                  quest->next = ch->pcdata->quests;
                  ch->pcdata->quests = quest;
                  fMatch = TRUE;
                  break;
                }
            break;

            case 'R':
                KEY ("RpExp", ch->rpexp, fread_number( fp) );
		KEY ("RlExp", ch->realexp, fread_number( fp) );
                KEY ("RpLevel", ch->rplevel, fread_number( fp) );
                KEY ("Rank", ch->rank, fread_number( fp) );
	//	KEY( "Rroom",    ch->pcdata->rentroom,  fread_number( fp ) );    room rental


		if ( !str_cmp( word, "Race" ) ) {
     		  char *tmp = fread_string(fp);
     		  ch->race = race_lookup(tmp);
     		  free_string(tmp);
     		  fMatch = TRUE;
     		  break;
		}

                if (!str_cmp (word, "Room"))
                {
                    ch->in_room = get_room_index (fread_number (fp));
                    if (ch->in_room == NULL)
                        ch->in_room = get_room_index (ROOM_VNUM_LIMBO);
                    fMatch = TRUE;
                    break;
                }
				KEY ("RP", ch->rp_points, fread_number(fp));
                break;

            case 'S':
                KEY ("SavingThrow", ch->saving_throw, fread_number (fp));
                KEY ("Save", ch->saving_throw, fread_number (fp));
                KEY ("Scro", ch->lines, fread_number (fp));
                KEY ("Sex", ch->sex, fread_number (fp));
                KEY ("ShortDescr", ch->short_descr, fread_string (fp));
                KEY ("ShD", ch->short_descr, fread_string (fp));
                KEY ("Sec", ch->pcdata->security, fread_number (fp));    /* OLC */
                KEY ("Silv", ch->silver, fread_number (fp));
                KEY ("SilvB", ch->silver_in_bank, fread_number (fp));
                KEY ("Simskill", ch->simskill, fread_string (fp));
		KEY ("Stance", ch->pcdata->stance, fread_number( fp ) );

                if (!str_cmp (word, "Skill") || !str_cmp (word, "Sk"))
                {
                    int sn;
                    int value;
                    char *temp;

                    value = fread_number (fp);
                    temp = fread_word (fp);
                    sn = skill_lookup (temp);
                    /* sn    = skill_lookup( fread_word( fp ) ); */
                    if (sn < 0)
                    {
                        fprintf (stderr, "%s", temp);
                        bug ("Fread_char: unknown skill. ", 0);
                    }
                    else
                        ch->pcdata->learned[sn] = value;
                    fMatch = TRUE;
                }

                break;

            case 'T':
                KEY ("TrueSex", ch->pcdata->true_sex, fread_number (fp));
                KEY ("TSex", ch->pcdata->true_sex, fread_number (fp));
                KEY ("Trai", ch->train, fread_number (fp));
                KEY ("Trust", ch->trust, fread_number (fp));
                KEY ("Tru", ch->trust, fread_number (fp));
                KEY ("TKills", ch->pcdata->tournament.kills, fread_number(fp));
                KEY ("TDeaths", ch->pcdata->tournament.deaths, fread_number(fp));
                KEY ("TRep", ch->pcdata->tournament.reputation, fread_number(fp));
                KEY ("TRank", ch->pcdata->tournament.rank, fread_number(fp));
                KEY ("TLRank", ch->pcdata->tournament.last_rank, fread_number(fp));
                KEY ("TLKill", ch->pcdata->tournament.last_killed, fread_number(fp));
                KEY ("Thief", ch->penalty.thief,fread_number(fp));
                if (!str_cmp (word, "Title") || !str_cmp (word, "Titl"))
                {
                    ch->pcdata->title = fread_string (fp);
                    if (ch->pcdata->title[0] != '.'
                        && ch->pcdata->title[0] != ','
                        && ch->pcdata->title[0] != '!'
                        && ch->pcdata->title[0] != '?')
                    {
                        sprintf (buf, " %s", ch->pcdata->title);
                        free_string (ch->pcdata->title);
                        ch->pcdata->title = str_dup (buf);
                    }
                    fMatch = TRUE;
                    break;
                }

                break;
            case 'U':
                KEY ("Update", ch->update, fread_number (fp));
                break;
            case 'V':
                KEY ("Version", ch->version, fread_number (fp));
                KEY ("Vers", ch->version, fread_number (fp));
                if (!str_cmp (word, "Vnum"))
                {
                    ch->pIndexData = get_mob_index (fread_number (fp));
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'W':
                KEY ("Wimpy", ch->wimpy, fread_number (fp));
                KEY ("Wimp", ch->wimpy, fread_number (fp));
                KEY ("Wizn", ch->wiznet, fread_flag (fp));
                if ( !str_cmp( word, "Weaves" )) {
                  int i;
                  for (i = 0;i < MAX_WEAVES;i++)
                  {
                    ch->pcdata->weaves[i] = fread_number(fp);
                  }
                  fMatch = TRUE;
                  break;
		}
                break;
            case 'X':
                if (!str_cmp(word, "XpMultFac"))
                {
                  ch->pcdata->xpmultiplier.on = TRUE;
                  ch->pcdata->xpmultiplier.factor = fread_number(fp);
                  fMatch = TRUE;
                  break;
                }

                if (!str_cmp(word, "XpMultTim"))
                {
                  ch->pcdata->xpmultiplier.on = TRUE;
                  ch->pcdata->xpmultiplier.time = fread_number(fp);
                  fMatch = TRUE;
                  break;
                }
                break;
        }

        if (!fMatch)
        {
            bug ("Fread_char: no match.", 0);
            bug (word, 0);
            fread_to_eol (fp);
        }
    }
}

/* load a pet from the forgotten reaches */
void fread_pet (CHAR_DATA * ch, FILE * fp)
{
    char *word;
    CHAR_DATA *pet;
    bool fMatch;
    int lastlogoff = current_time;
    int percent;


    /* first entry had BETTER be the vnum or we barf */
    word = feof (fp) ? str_dup("END") : fread_word (fp);
    if (!str_cmp (word, "Vnum"))
    {
        int vnum;

        vnum = fread_number (fp);
        if (get_mob_index (vnum) == NULL)
        {
            bug ("Fread_pet: bad vnum %d.", vnum);
            pet = create_mobile (get_mob_index (MOB_VNUM_FIDO));
        }
        else
            pet = create_mobile (get_mob_index (vnum));
    }
    else
    {
        bug ("Fread_pet: no vnum in file.", 0);
        pet = create_mobile (get_mob_index (MOB_VNUM_FIDO));
    }

    for (;;)
    {
        word = feof (fp) ? str_dup("END") : fread_word (fp);
        fMatch = FALSE;

        switch (UPPER (word[0]))
        {
            case '*':
                fMatch = TRUE;
                fread_to_eol (fp);
                break;

            case 'A':
                KEY ("Act", pet->act, fread_flag (fp));
                KEY ("Alig", pet->alignment, fread_number (fp));

                if (ch->version < 6)
                {
		  KEYF( "AfBy", pet->affected_by, 
			aff_convert_fread_flag( fread_flag(fp)), AFF_FLAGS);
                }
		else
                {
		  KEYF("AfBy", pet->affected_by, str_fread_flag(fp, AFF_FLAGS), AFF_FLAGS);
		} 

                if (!str_cmp (word, "ACs"))
                {
                    int i;

                    for (i = 0; i < 4; i++)
                        pet->armor[i] = fread_number (fp);
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "AffD"))
                {
                    AFFECT_DATA *paf;
                    int sn;

                    paf = new_affect ();

                    sn = skill_lookup (fread_word (fp));
                    if (sn < 0)
                        bug ("Fread_char: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->level = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->location = fread_number (fp);
                    paf->bitvector = fread_number (fp);
                    paf->next = pet->affected;
                    pet->affected = paf;
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "Affc"))
                {
                    AFFECT_DATA *paf;
                    int sn;

                    paf = new_affect ();

                    sn = skill_lookup (fread_word (fp));
                    if (sn < 0)
                        bug ("Fread_char: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->where = fread_number (fp);
                    paf->level = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->location = fread_number (fp);
                    paf->bitvector = fread_number (fp);
                    paf->next = pet->affected;
                    pet->affected = paf;
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "AMod"))
                {
                    int stat;

                    for (stat = 0; stat < MAX_STATS; stat++)
                        pet->mod_stat[stat] = fread_number (fp);
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "Attr"))
                {
                    int stat;

                    for (stat = 0; stat < MAX_STATS; stat++)
                        pet->perm_stat[stat] = fread_number (fp);
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'C':
                KEY ("Clan", pet->clan, clan_lookup (fread_string (fp)));
                KEY ("Comm", pet->comm, fread_flag (fp));
                break;

            case 'D':
                KEY ("Dam", pet->damroll, fread_number (fp));
                KEY ("Desc", pet->description, fread_string (fp));
                break;

            case 'E':
                if (!str_cmp (word, "End"))
                {
                    pet->leader = ch;
                    pet->master = ch;
                    ch->pet = pet;
                    /* adjust hp mana move up  -- here for speed's sake */
                    percent =
                        (current_time - lastlogoff) * 25 / (2 * 60 * 60);

                    if (percent > 0 && !IS_AFFECTED (ch, AFF_POISON)
                        && !IS_AFFECTED (ch, AFF_PLAGUE))
                    {
                        percent = UMIN (percent, 100);
                        pet->hit += (pet->max_hit - pet->hit) * percent / 100;
                        pet->mana +=
                            (pet->max_mana - pet->mana) * percent / 100;
                        pet->move +=
                            (pet->max_move - pet->move) * percent / 100;
                    }
                    return;
                }
                KEY ("Exp", pet->exp, fread_number (fp));
                break;

            case 'G':
                KEY ("Gold", pet->gold, fread_number (fp));
                break;

            case 'H':
                KEY ("Hit", pet->hitroll, fread_number (fp));

                if (!str_cmp (word, "HMV"))
                {
                    pet->hit = fread_long (fp);
                    pet->max_hit = fread_long (fp);
                    pet->mana = fread_number (fp);
                    pet->max_mana = fread_number (fp);
                    pet->move = fread_number (fp);
                    pet->max_move = fread_number (fp);
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'L':
                KEY ("Levl", pet->level, fread_number (fp));
                KEY ("LnD", pet->long_descr, fread_string (fp));
                KEY ("LogO", lastlogoff, fread_number (fp));
                break;

            case 'N':
                KEY ("Name", pet->name, fread_string (fp));
                break;

            case 'P':
                KEY ("Pos", pet->position, fread_number (fp));
                break;

            case 'R':
                KEY ("Race", pet->race, race_lookup (fread_string (fp)));
                break;

            case 'S':
                KEY ("Save", pet->saving_throw, fread_number (fp));
                KEY ("Sex", pet->sex, fread_number (fp));
                KEY ("ShD", pet->short_descr, fread_string (fp));
                KEY ("Silv", pet->silver, fread_number (fp));
                break;

                if (!fMatch)
                {
                    bug ("Fread_pet: no match.", 0);
                    fread_to_eol (fp);
                }

        }
    }
}

extern OBJ_DATA *obj_free;

void fread_obj (CHAR_DATA * ch, FILE * fp)
{
    OBJ_DATA *obj;
    char *word;
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;
    bool first;
    bool new_format;            /* to prevent errors */
    bool make_new;                /* update object */

    fVnum = FALSE;
    obj = NULL;
    first = TRUE;                /* used to counter fp offset */
    new_format = FALSE;
    make_new = FALSE;

    word = feof (fp) ? str_dup("End") : fread_word (fp);
    if (!str_cmp (word, "Vnum"))
    {
        int vnum;
        first = FALSE;            /* fp will be in right place */

        vnum = fread_number (fp);
        if (get_obj_index (vnum) == NULL)
        {
            bug ("Fread_obj: bad vnum %d.", vnum);
        }
        else
        {
            obj = create_object (get_obj_index (vnum), -1);
	    obj->enchants = 2;
            new_format = TRUE;
        }

    }

    if (obj == NULL)
    {                            /* either not found or old style */
        obj = new_obj ();
        obj->enchants = 2;
        obj->name = str_dup ("");
        obj->short_descr = str_dup ("");
        obj->description = str_dup ("");
    }

    fNest = FALSE;
    fVnum = TRUE;
    iNest = 0;

    for (;;)
    {
        if (first)
            first = FALSE;
        else
            word = feof (fp) ? str_dup("End") : fread_word (fp);
        fMatch = FALSE;

        switch (UPPER (word[0]))
        {
            case '*':
                fMatch = TRUE;
                fread_to_eol (fp);
                break;

            case 'A':
                if (!str_cmp (word, "AffD"))
                {
                    AFFECT_DATA *paf;
                    int sn;

                    paf = new_affect ();

                    sn = skill_lookup (fread_word (fp));
                    if (sn < 0)
                        bug ("Fread_obj: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->level = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->location = fread_number (fp);
                    paf->bitvector = fread_number (fp);
                    paf->next = obj->affected;
                    obj->affected = paf;
                    fMatch = TRUE;
                    break;
                }
                if (!str_cmp (word, "Affc"))
                {
                    AFFECT_DATA *paf;
                    int sn;
		    bool found = FALSE;

                    paf = new_affect ();

                    sn = skill_lookup (fread_word (fp));
                    if (sn < 0)
                        bug ("Fread_obj: unknown skill.", 0);
                    else
                        paf->type = sn;

                    paf->where = fread_number (fp);
                    paf->level = fread_number (fp);
                    paf->duration = fread_number (fp);
                    paf->modifier = fread_number (fp);
                    paf->location = fread_number (fp);
                    paf->bitvector = fread_number (fp);
		    {
		      AFFECT_DATA *tmp;
		      for (tmp = obj->affected;tmp != NULL;tmp = tmp->next)
		      {
			if (tmp->bitvector == paf->bitvector && paf->bitvector != 0)
			  found = TRUE;
		      }
		    }
		    if (!found)
		    {
                      paf->next = obj->affected;
                      obj->affected = paf;
		    }
		    else
		      free_affect(paf);
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'C':
                KEY ("ClaF", obj->class_flags, fread_number (fp));
                KEY ("Cond", obj->condition, fread_number (fp));
                KEY ("Cost", obj->cost, fread_number (fp));
                break;

            case 'D':
                KEY ("Description", obj->description, fread_string (fp));
                KEY ("Desc", obj->description, fread_string (fp));
                break;

            case 'E':

                if (!str_cmp (word, "Enchanted"))
                {
                    obj->enchanted = TRUE;
                    fMatch = TRUE;
                    break;
                }

                KEY ("Enchants", obj->enchants, fread_number (fp));
                KEY ("ExtraFlags", obj->extra_flags, fread_number (fp));
                KEY ("ExtF", obj->extra_flags, fread_number (fp));

                if (!str_cmp (word, "ExtraDescr") || !str_cmp (word, "ExDe"))
                {
                    EXTRA_DESCR_DATA *ed;

                    ed = new_extra_descr ();

                    ed->keyword = fread_string (fp);
                    ed->description = fread_string (fp);
                    ed->next = obj->extra_descr;
                    obj->extra_descr = ed;
                    fMatch = TRUE;
                }

                if (!str_cmp (word, "End"))
                {
                    if (!fNest || (fVnum && obj->pIndexData == NULL))
                    {
                        bug ("Fread_obj: incomplete object.", 0);
                        free_obj (obj);
                        return;
                    }
                    else
                    {
                        if (!fVnum)
                        {
                            free_obj (obj);
                            obj =
                                create_object (get_obj_index (OBJ_VNUM_DUMMY),
                                               0);
                        }

                        if (!new_format)
                        {
                            obj->next = object_list;
                            object_list = obj;
                            obj->pIndexData->count++;
                        }

                        if (!obj->pIndexData->new_format
                            && obj->item_type == ITEM_ARMOR
                            && obj->value[1] == 0)
                        {
                            obj->value[1] = obj->value[0];
                            obj->value[2] = obj->value[0];
                        }
                        if (make_new)
                        {
                            int wear;

                            wear = obj->wear_loc;
                            extract_obj (obj);

                            obj = create_object (obj->pIndexData, 0);
                            obj->wear_loc = wear;
                        }
                        if (iNest == 0 || rgObjNest[iNest] == NULL)
                            obj_to_char (obj, ch);
                        else
                            obj_to_obj (obj, rgObjNest[iNest - 1]);
                        return;
                    }
                }
                break;


            case 'H':
                KEY ("HonLev", obj->honor_level, fread_number (fp));
                break;
            case 'I':
                KEY ("ItemType", obj->item_type, fread_number (fp));
                KEY ("Ityp", obj->item_type, fread_number (fp));
                KEY ("Id", obj->id, fread_number(fp));
                break;

            case 'L':
                KEY ("Level", obj->level, fread_number (fp));
                KEY ("Lev", obj->level, fread_number (fp));
                break;

            case 'N':
                KEY ("Name", obj->name, fread_string (fp));

                if (!str_cmp (word, "Nest"))
                {
                    iNest = fread_number (fp);
                    if (iNest < 0 || iNest >= MAX_NEST)
                    {
                        bug ("Fread_obj: bad nest %d.", iNest);
                    }
                    else
                    {
                        rgObjNest[iNest] = obj;
                        fNest = TRUE;
                    }
                    fMatch = TRUE;
                }
                break;

            case 'O':
                KEY("Owner", obj->owner, fread_string(fp));
                if (!str_cmp (word, "Oldstyle"))
                {
                    if (obj->pIndexData != NULL
                        && obj->pIndexData->new_format)
                        make_new = TRUE;
                    fMatch = TRUE;
                }
                break;


            case 'P':
                if (!str_cmp (word, "PK"))
                {
                   int temp;
		   temp = fread_number(fp);
 		   obj->pk = temp == 0 ? FALSE : TRUE;
                    fMatch = TRUE;
                }
                break;


            case 'S':
                KEY ("ShortDescr", obj->short_descr, fread_string (fp));
                KEY ("ShD", obj->short_descr, fread_string (fp));

                if (!str_cmp (word, "Spell"))
                {
                    int iValue;
                    int sn;

                    iValue = fread_number (fp);
                    sn = skill_lookup (fread_word (fp));
                    if (iValue < 0 || iValue > 3)
                    {
                        bug ("Fread_obj: bad iValue %d.", iValue);
                    }
                    else if (sn < 0)
                    {
                        bug ("Fread_obj: unknown skill.", 0);
                    }
                    else
                    {
                        obj->value[iValue] = sn;
                    }
                    fMatch = TRUE;
                    break;
                }
                /*if (!str_cmp (word, "Spellaffected"))
                {
                    obj->spellaffected = TRUE;
                    fMatch = TRUE;
                    break;
                }*/
                break;

            case 'T':
                KEY ("Timer", obj->timer, fread_number (fp));
                KEY ("Time", obj->timer, fread_number (fp));
                break;

            case 'V':
                if (!str_cmp (word, "Values") || !str_cmp (word, "Vals"))
                {
                    obj->value[0] = fread_number (fp);
                    obj->value[1] = fread_number (fp);
                    obj->value[2] = fread_number (fp);
                    obj->value[3] = fread_number (fp);
                    if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
                        obj->value[0] = obj->pIndexData->value[0];
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "Val"))
                {
                    obj->value[0] = fread_number (fp);
                    obj->value[1] = fread_number (fp);
                    obj->value[2] = fread_number (fp);
                    obj->value[3] = fread_number (fp);
                    obj->value[4] = fread_number (fp);
                    fMatch = TRUE;
                    break;
                }

                if (!str_cmp (word, "Vnum"))
                {
                    int vnum;

                    vnum = fread_number (fp);
                    if ((obj->pIndexData = get_obj_index (vnum)) == NULL)
                        bug ("Fread_obj: bad vnum %d.", vnum);
                    else
                        fVnum = TRUE;
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'W':
                KEY ("WearFlags", obj->wear_flags, fread_number (fp));
                KEY ("WeaF", obj->wear_flags, fread_number (fp));
                KEY ("WearLoc", obj->wear_loc, fread_number (fp));
                KEY ("Wear", obj->wear_loc, fread_number (fp));
                KEY ("Weight", obj->weight, fread_number (fp));
                KEY ("Wt", obj->weight, fread_number (fp));
                break;

        }

        if (!fMatch)
        {
            bug ("Fread_obj: no match.", 0);
            fread_to_eol (fp);
        }
    }
}



OBJ_DATA *fread_obj_store( FILE *fp )
{
    OBJ_DATA *obj;
    char *word;
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;
    bool first;
    bool new_format;  /* to prevent errors */
    bool make_new;    /* update object */
    
    fVnum = FALSE;
    obj = NULL;
    first = TRUE;  /* used to counter fp offset */
    new_format = FALSE;
    make_new = FALSE;

    word   = feof( fp ) ? str_dup("End") : fread_word( fp );
    if (!str_cmp(word,"Vnum" ))
    {
        int vnum;
	first = FALSE;  /* fp will be in right place */
 
        vnum = fread_number( fp );
        if (  get_obj_index( vnum )  == NULL )
	{
            bug( "Fread_obj: bad vnum %d.", vnum );
	}
        else
	{
	    obj = create_object(get_obj_index(vnum),-1);
	    new_format = TRUE;
	}
	    
    }

    if (obj == NULL)  /* either not found or old style */
    {
    	obj = new_obj();
    	obj->name		= str_dup( "" );
    	obj->short_descr	= str_dup( "" );
    	obj->description	= str_dup( "" );
    }

    fNest		= FALSE;
    fVnum		= TRUE;
    iNest		= 0;

    for ( ; ; )
    {
	if (first)
	    first = FALSE;
	else
	    word   = feof( fp ) ? str_dup("End") : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    if (!str_cmp(word,"AffD"))
	    {
		AFFECT_DATA *paf;
		int sn;

		paf = new_affect();

		sn = skill_lookup(fread_word(fp));
		if (sn < 0)
		    bug("Fread_obj: unknown skill.",0);
		else
		    paf->type = sn;

		paf->level	= fread_number( fp );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		paf->next	= obj->affected;
		obj->affected	= paf;
		fMatch		= TRUE;
		break;
	    }
            if (!str_cmp(word,"Affc"))
            {
                AFFECT_DATA *paf;
                int sn;
 
                paf = new_affect();
 
                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_obj: unknown skill.",0);
                else
                    paf->type = sn;
 
		paf->where	= fread_number( fp );
                paf->level      = fread_number( fp );
                paf->duration   = fread_number( fp );
                paf->modifier   = fread_number( fp );
                paf->location   = fread_number( fp );
                paf->bitvector  = fread_number( fp );
                paf->next       = obj->affected;
                obj->affected   = paf;
                fMatch          = TRUE;
                break;
            }
	    break;

	case 'C':
            KEY ("ClaF", obj->class_flags, fread_number (fp));
	    KEY( "Cond",	obj->condition,		fread_number( fp ) );
	    KEY( "Cost",	obj->cost,		fread_number( fp ) );
	    break;

	case 'D':
	    KEY( "Description",	obj->description,	fread_string( fp ) );
	    KEY( "Desc",	obj->description,	fread_string( fp ) );
	    break;

	case 'E':
            KEY ("Enchants", obj->enchants, fread_number (fp));
	    if ( !str_cmp( word, "Enchanted"))
	    {
		obj->enchanted = TRUE;
	 	fMatch 	= TRUE;
		break;
	    }

	    KEY( "ExtraFlags",	obj->extra_flags,	fread_number( fp ) );
	    KEY( "ExtF",	obj->extra_flags,	fread_number( fp ) );

	    if ( !str_cmp( word, "ExtraDescr" ) || !str_cmp(word,"ExDe"))
	    {
		EXTRA_DESCR_DATA *ed;

		ed = new_extra_descr();

		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= obj->extra_descr;
		obj->extra_descr	= ed;
		fMatch = TRUE;
	    }

	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !fNest || ( fVnum && obj->pIndexData == NULL ) )
		{
		    bug( "Fread_obj: incomplete object.", 0 );
		    free_obj(obj);
		    return NULL;
		}
		else
	        {
		    if ( !fVnum )
		    {
			free_obj( obj );
			obj = create_object( get_obj_index( OBJ_VNUM_DUMMY ), 0 );
		    }

		    if (!new_format)
		    {
		    	obj->next	= object_list;
		    	object_list	= obj;
		    	obj->pIndexData->count++;
		    }

		    if (!obj->pIndexData->new_format 
		    && obj->item_type == ITEM_ARMOR
		    &&  obj->value[1] == 0)
		    {
			obj->value[1] = obj->value[0];
			obj->value[2] = obj->value[0];
		    }
		    if (make_new)
		    {
			int wear;
			
			wear = obj->wear_loc;
			extract_obj(obj);

			obj = create_object(obj->pIndexData,0);
			obj->wear_loc = wear;
		    }
		    if ( iNest == 0 || rgObjNest[iNest] == NULL )
			return obj;
		    else
			obj_to_obj( obj, rgObjNest[iNest-1] );
		    return obj;
		}
	    }
	    break;

	case 'I':
	    KEY( "ItemType",	obj->item_type,		fread_number( fp ) );
	    KEY( "Ityp",	obj->item_type,		fread_number( fp ) );
            KEY( "Id", obj->id, fread_number(fp));
	    break;

	case 'L':
	    KEY( "Level",	obj->level,		fread_number( fp ) );
	    KEY( "Lev",		obj->level,		fread_number( fp ) );
	    break;

	case 'N':
	    KEY( "Name",	obj->name,		fread_string( fp ) );

	    if ( !str_cmp( word, "Nest" ) )
	    {
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		{
		    bug( "Fread_obj: bad nest %d.", iNest );
		}
		else
		{
		    rgObjNest[iNest] = obj;
		    fNest = TRUE;
		}
		fMatch = TRUE;
	    }
	    break;

   	case 'O':
	    if ( !str_cmp( word,"Oldstyle" ) )
	    {
		if (obj->pIndexData != NULL && obj->pIndexData->new_format)
		    make_new = TRUE;
		fMatch = TRUE;
	    }
	    break;
		    
            case 'P':
                if (!str_cmp (word, "PK"))
                {
                   int temp;
		   temp = fread_number(fp);
 		   obj->pk = temp == 0 ? FALSE : TRUE;
                    fMatch = TRUE;
                }
                break;

	case 'S':
	    KEY( "ShortDescr",	obj->short_descr,	fread_string( fp ) );
	    KEY( "ShD",		obj->short_descr,	fread_string( fp ) );

	    if ( !str_cmp( word, "Spell" ) )
	    {
		int iValue;
		int sn;

		iValue = fread_number( fp );
		sn     = skill_lookup( fread_word( fp ) );
		if ( iValue < 0 || iValue > 3 )
		{
		    bug( "Fread_obj: bad iValue %d.", iValue );
		}
		else if ( sn < 0 )
		{
		    bug( "Fread_obj: unknown skill.", 0 );
		}
		else
		{
		    obj->value[iValue] = sn;
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'T':
	    KEY( "Timer",	obj->timer,		fread_number( fp ) );
	    KEY( "Time",	obj->timer,		fread_number( fp ) );
	    break;

	case 'V':
	    if ( !str_cmp( word, "Values" ) || !str_cmp(word,"Vals"))
	    {
		obj->value[0]	= fread_number( fp );
		obj->value[1]	= fread_number( fp );
		obj->value[2]	= fread_number( fp );
		obj->value[3]	= fread_number( fp );
		if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
		   obj->value[0] = obj->pIndexData->value[0];
		fMatch		= TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Val" ) )
	    {
		obj->value[0] 	= fread_number( fp );
	 	obj->value[1]	= fread_number( fp );
	 	obj->value[2] 	= fread_number( fp );
		obj->value[3]	= fread_number( fp );
		obj->value[4]	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Vnum" ) )
	    {
		int vnum;

		vnum = fread_number( fp );
		if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL )
		    bug( "Fread_obj: bad vnum %d.", vnum );
		else
		    fVnum = TRUE;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "WearFlags",	obj->wear_flags,	fread_number( fp ) );
	    KEY( "WeaF",	obj->wear_flags,	fread_number( fp ) );
	    KEY( "WearLoc",	obj->wear_loc,		fread_number( fp ) );
	    KEY( "Wear",	obj->wear_loc,		fread_number( fp ) );
	    KEY( "Weight",	obj->weight,		fread_number( fp ) );
	    KEY( "Wt",		obj->weight,		fread_number( fp ) );
	    break;

	}

	if ( !fMatch )
	{
	    bug( "Fread_obj: no match store.", 0 );
	    fread_to_eol( fp );
	}
    }
}

/* Called from save_char_obj to write out RLE data */
void fwrite_rle( char *explored, FILE *fp )
{
    int index;
    /* The first bit is assumed to be zero */
    int bit = 0;
    int count = 0;


    /* Cycle through every bit */
    for( index = 0; index < ( MAX_EXPLORE * 8 ) - 1; index++ )
    {
       /* If the bit is the same as last time, add to the count */
       if ( getbit( explored, index ) == bit )
       {
           count++;
       }
       else
       {
           /* Otherwise we write this length out and start over */
           fprintf( fp, " %d", count );
           count = 1;
           bit = getbit( explored, index );
       }
    }
    /* Write out any left over run, -1 terminates */
    fprintf( fp, " %d -1\n\n", count );
}

/* Called from load_char_obj to read in the saved RLE data */
void fread_rle( char *explored, FILE *fp )
{
    int index = 0;
    int bit = 0, count = 0, pos = 0;

    /* Cycle through the unknown number of pairs */
    for( ; ; )
    {
        /* Get the run length */
        count = fread_number( fp );

        /* Is it the terminating -1? */
        if ( count < 0 )
            break;

        /* The array initializes to zero, so only set on a one */
        if ( bit == 1 )
        {
            /* Fill in all the bytes in the run */
            for( index = pos; index < pos + count; index++ )
            {
                explored[index / 8] |= 1 << ( index % 8 );
            }
        }

        /* This run is over, so toggle the value */
        bit = ( bit ) ? 0 : 1;

        /* Advance to the start of the next run */
        pos += count;
    }
}

