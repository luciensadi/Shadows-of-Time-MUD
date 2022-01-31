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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"

/* stuff for recyling notes */
NOTE_DATA *note_free;

NOTE_DATA *new_note ()
{
    NOTE_DATA *note;

    if (note_free == NULL)
        note = (NOTE_DATA *) alloc_perm (sizeof (*note));
    else
    {
        note = note_free;
        note_free = note_free->next;
    }
    VALIDATE (note);
    return note;
}

void free_note (NOTE_DATA * note)
{
    if (!IS_VALID (note))
        return;

    free_string (note->text);
    free_string (note->subject);
    free_string (note->to_list);
    free_string (note->date);
    free_string (note->sender);

    INVALIDATE (note);

    note->next = note_free;
    note_free = note; 
}


/* stuff for recycling ban structures */
BAN_DATA *ban_free;

BAN_DATA *new_ban (void)
{
    static BAN_DATA ban_zero;
    BAN_DATA *ban;

    if (ban_free == NULL)
        ban = (BAN_DATA *) alloc_perm (sizeof (*ban));
    else
    {
        ban = ban_free;
        ban_free = ban_free->next;
    }

    *ban = ban_zero;
    VALIDATE (ban);
    ban->name = &str_empty[0];
    return ban;
}

void free_ban (BAN_DATA * ban)
{
    if (!IS_VALID (ban))
        return;

    free_string (ban->name);
    INVALIDATE (ban);

    ban->next = ban_free;
    ban_free = ban;
}





/* stuff for recycling ban structures */
VOTERS_DATA *voter_free;

VOTERS_DATA *new_voter (void)
{
    static VOTERS_DATA voter_zero;
    VOTERS_DATA *voter;

    if (voter_free == NULL)
        voter = (VOTERS_DATA *) alloc_perm (sizeof (*voter));
    else
    {
        voter = voter_free;
        voter_free = voter_free->next;
    }

    *voter = voter_zero;
    VALIDATE (voter);
    voter->host = str_dup("none");
    voter->id = -1;
    return voter;
}

void free_voter (VOTERS_DATA * voter)
{
    if (!IS_VALID (voter))
        return;

    INVALIDATE (voter);

    free_string(voter->host);
    free_string(voter->reason);
    voter->next = voter_free;
    voter_free = voter;
}



/* stuff for recycling roster structures */
MEMBER_DATA *member_free;

MEMBER_DATA *new_member (void)
{
    static MEMBER_DATA member_zero;
    MEMBER_DATA *member;

    if (member_free == NULL)
        member = (MEMBER_DATA *) alloc_perm (sizeof (*member));
    else
    {
        member = member_free;
        member_free = member_free->next;
    }

    *member = member_zero;
    VALIDATE (member);
    member->name = str_dup("None");
    member->action = str_dup("None");
    member->rank = -1;
    member->qp = 0;
    member->rp = 0;
    return member;
}

void free_member (MEMBER_DATA * member)
{
    if (!IS_VALID (member))
        return;

    free_string(member->name);
    INVALIDATE (member);

    member->next = member_free;
    member_free = member;
}

/* stuff for recycling mark structures */
/*MARK_DATA *mark_free;

MARK_DATA *new_mark (void)
{
    static MARK_DATA mark_zero;
    MARK_DATA *mark;

    if (mark_free == NULL)
       mark = (MARK_DATA *) alloc_perm (sizeof (*mark));
    else
    {
        mark = mark_free;
        mark_free = mark_free->next;
    }

    *mark = mark_zero;
    VALIDATE (mark);
    mark->contractor = str_dup("None");
    //mark->type = -1;
    mark->target = str_dup("None");
    mark->desc = str_dup("None");
    mark->status = -1;
    mark->price = 0;
    mark->extra = 0;
    return mark;
}

void free_mark (MARK_DATA * mark)
{
    if (!IS_VALID (mark))
        return;

    free_string(mark->contractor);
    free_string(mark->target);
    free_string(mark->desc);
    INVALIDATE (mark);

    mark->next = mark_free;
    mark_free = mark;
}
*/




/* stuff for recycling descriptors */
DESCRIPTOR_DATA *descriptor_free;

DESCRIPTOR_DATA *new_descriptor (void)
{
    static DESCRIPTOR_DATA d_zero;
    DESCRIPTOR_DATA *d;

    if (descriptor_free == NULL)
        d = (DESCRIPTOR_DATA *) alloc_perm (sizeof (*d));
    else
    {
        d = descriptor_free;
        descriptor_free = descriptor_free->next;
    }

    *d = d_zero;
    VALIDATE (d);

    d->account = NULL;
    d->connected = CON_GET_NAME;
    d->showstr_head = NULL;
    d->showstr_point = NULL;
    d->outsize = 2000;
    d->outbuf = (char *) alloc_mem (d->outsize);

    return d;
}

void free_descriptor (DESCRIPTOR_DATA * d)
{
    if (!IS_VALID (d))
        return;

    #ifndef THREADED_DNS
       #ifndef DNS_SLAVE
         free_string( d->Host );
       #endif
    #endif
    free_mem (d->outbuf, d->outsize);
    INVALIDATE (d);
    d->next = descriptor_free;
    descriptor_free = d;
}

/* stuff for recycling gen_data */
GEN_DATA *gen_data_free;

GEN_DATA *new_gen_data (void)
{
    static GEN_DATA gen_zero;
    GEN_DATA *gen;

    if (gen_data_free == NULL)
        gen = (GEN_DATA *) alloc_perm (sizeof (*gen));
    else
    {
        gen = gen_data_free;
        gen_data_free = gen_data_free->next;
    }
    *gen = gen_zero;
    VALIDATE (gen);
    return gen;
}

void free_gen_data (GEN_DATA * gen)
{
    if (!IS_VALID (gen))
        return;

    INVALIDATE (gen);

    gen->next = gen_data_free;
    gen_data_free = gen;
}

/* stuff for recycling extended descs */
EXTRA_DESCR_DATA *extra_descr_free;

EXTRA_DESCR_DATA *new_extra_descr (void)
{
    EXTRA_DESCR_DATA *ed;

    if (extra_descr_free == NULL)
        ed = (EXTRA_DESCR_DATA *) alloc_perm (sizeof (*ed));
    else
    {
        ed = extra_descr_free;
        extra_descr_free = extra_descr_free->next;
    }

    ed->keyword = &str_empty[0];
    ed->description = &str_empty[0];
    VALIDATE (ed);
    return ed;
}

void free_extra_descr (EXTRA_DESCR_DATA * ed)
{
    if (!IS_VALID (ed))
        return;

    free_string (ed->keyword);
    free_string (ed->description);
    INVALIDATE (ed);

    ed->next = extra_descr_free;
    extra_descr_free = ed;
}


/* stuff for recycling affects */
AFFECT_DATA *affect_free;

AFFECT_DATA *new_affect (void)
{
    static AFFECT_DATA af_zero;
    AFFECT_DATA *af;

    if (affect_free == NULL)
        af = (AFFECT_DATA *) alloc_perm (sizeof (*af));
    else
    {
        af = affect_free;
        affect_free = affect_free->next;
    }

    *af = af_zero;


    VALIDATE (af);
    return af;
}

void free_affect (AFFECT_DATA * af)
{
    if (!IS_VALID (af))
        return;

    INVALIDATE (af);
    af->next = affect_free;
    affect_free = af;
}

/* stuff for recycling objects */
OBJ_DATA *obj_free;

OBJ_DATA *new_obj (void)
{
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;

    if (obj_free == NULL)
        obj = (OBJ_DATA *) alloc_perm (sizeof (*obj));
    else
    {
        obj = obj_free;
        obj_free = obj_free->next;
    }
    *obj = obj_zero;
    VALIDATE (obj);

    return obj;
}

void free_obj (OBJ_DATA * obj)
{
    AFFECT_DATA *paf, *paf_next;
    EXTRA_DESCR_DATA *ed, *ed_next;

    if (!IS_VALID (obj))
        return;

    for (paf = obj->affected; paf != NULL; paf = paf_next)
    {
        paf_next = paf->next;
        free_affect (paf);
    }
    obj->affected = NULL;

    for (ed = obj->extra_descr; ed != NULL; ed = ed_next)
    {
        ed_next = ed->next;
        free_extra_descr (ed);
    }
    obj->extra_descr = NULL;

    free_string (obj->name);
    free_string (obj->description);
    free_string (obj->short_descr);
    free_string (obj->owner);
    INVALIDATE (obj);

    obj->next = obj_free;
    obj_free = obj;
}


/* stuff for recyling characters */
CHAR_DATA *char_free;

CHAR_DATA *new_char (void)
{
    static CHAR_DATA ch_zero;
    CHAR_DATA *ch;
    int i;

    if (char_free == NULL)
        ch = (CHAR_DATA *) alloc_perm (sizeof (*ch));
    else
    {
        ch = char_free;
        char_free = char_free->next;
    }

    *ch = ch_zero;
    VALIDATE (ch);
    ch->name = &str_empty[0];
    ch->short_descr = &str_empty[0];
    ch->long_descr = &str_empty[0];
    ch->description = &str_empty[0];
    ch->prompt = &str_empty[0];
    ch->prefix = &str_empty[0];
    ch->logon = current_time;
    ch->lines = PAGELEN;
    for (i = 0; i < 4; i++)
        ch->armor[i] = 100;
    ch->position = POS_STANDING;
    ch->stunned = 0;
    ch->hit = 20;
    ch->max_hit = 20;
    ch->mana = 100;
    ch->max_mana = 100;
    ch->move = 100;
    ch->max_move = 100;
    for (i = 0; i < MAX_STATS; i++)
    {
        ch->perm_stat[i] = 13;
        ch->mod_stat[i] = 0;
    }

    return ch;
}


void free_char (CHAR_DATA * ch)
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    if (!IS_VALID (ch))
        return;

    if (IS_NPC (ch))
        mobile_count--;

    for (obj = ch->carrying; obj != NULL; obj = obj_next)
    {
        obj_next = obj->next_content;
        extract_obj (obj);
    }

    for (paf = ch->affected; paf != NULL; paf = paf_next)
    {
        paf_next = paf->next;
        affect_remove (ch, paf);
    }

    free_string (ch->name);
    free_string (ch->short_descr);
    free_string (ch->long_descr);
    free_string (ch->description);
    free_string (ch->prompt);
    free_string (ch->prefix);
    free_note (ch->pnote);
    free_pcdata (ch->pcdata);

    ch->next = char_free;
    char_free = ch;

    INVALIDATE (ch);
    return;
}

PC_DATA *pcdata_free;

PC_DATA *new_pcdata (void)
{
    int alias;

    static PC_DATA pcdata_zero;
    PC_DATA *pcdata;

    if (pcdata_free == NULL)
        pcdata = (PC_DATA *) alloc_perm (sizeof (*pcdata));
    else
    {
        pcdata = pcdata_free;
        pcdata_free = pcdata_free->next;
    }

    *pcdata = pcdata_zero;

    for (alias = 0; alias < MAX_ALIAS; alias++)
    {
        pcdata->alias[alias] = NULL;
        pcdata->alias_sub[alias] = NULL;
    }

    pcdata->buffer = new_buf ();

    VALIDATE (pcdata);
    return pcdata;
}


void free_pcdata (PC_DATA * pcdata)
{
    int alias;
    struct idName *cName, *cName_next;

    if (!IS_VALID (pcdata))
        return;

    free_string (pcdata->pwd);
    free_string (pcdata->bamfin);
    free_string (pcdata->bamfout);
    free_string (pcdata->title);
    free_string (pcdata->mxpVersion);
    free_string (pcdata->clientVersion);
    free_string (pcdata->client);
    free_string (pcdata->forsaken_master);
    free_string (pcdata->granted);
    free_string (pcdata->manadrainee);


    free_buf (pcdata->buffer);

    for (cName = pcdata->names; cName != NULL; cName = cName_next)
    {
      cName_next = cName->next;
      freeName(cName);
    }

    for (alias = 0; alias < MAX_ALIAS; alias++)
    {
        free_string (pcdata->alias[alias]);
        free_string (pcdata->alias_sub[alias]);
    }
    INVALIDATE (pcdata);
    pcdata->next = pcdata_free;
    pcdata_free = pcdata;

    return;
}




/* stuff for setting ids */
long last_pc_id;
long last_mob_id;

long get_pc_id (void)
{
    int val;

    val = (current_time <= last_pc_id) ? last_pc_id + 1 : current_time;
    last_pc_id = val;
    return val;
}

long get_mob_id (void)
{
    last_mob_id++;
    return last_mob_id;
}

MEM_DATA *mem_data_free;

/* procedures and constants needed for buffering */

BUFFER *buf_free;

MEM_DATA *new_mem_data (void)
{
    MEM_DATA *memory;

    if (mem_data_free == NULL)
        memory = (MEM_DATA *) alloc_mem (sizeof (*memory));
    else
    {
        memory = (MEM_DATA *) mem_data_free;
        mem_data_free = mem_data_free->next;
    }

    memory->next = NULL;
    memory->id = 0;
    memory->reaction = 0;
    memory->when = 0;
    VALIDATE (memory);

    return memory;
}

void free_mem_data (MEM_DATA * memory)
{
    if (!IS_VALID (memory))
        return;

    memory->next = mem_data_free;
    mem_data_free = memory;
    INVALIDATE (memory);
}



/* buffer sizes */
const int buf_size[MAX_BUF_LIST] = {
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384
};

/* local procedure for finding the next acceptable size */
/* -1 indicates out-of-boundary error */
int get_size (int val)
{
    int i;

    for (i = 0; i < MAX_BUF_LIST; i++)
        if (buf_size[i] >= val)
        {
            return buf_size[i];
        }

    return -1;
}

BUFFER *new_buf ()
{
    BUFFER *buffer;

    if (buf_free == NULL)
        buffer = (BUFFER *) alloc_perm (sizeof (*buffer));
    else
    {
        buffer = buf_free;
        buf_free = buf_free->next;
    }

    buffer->next = NULL;
    buffer->state = BUFFER_SAFE;
    buffer->size = get_size (BASE_BUF);

    buffer->string = (char *) alloc_mem (buffer->size);
    buffer->string[0] = '\0';
    VALIDATE (buffer);

    return buffer;
}

BUFFER *new_buf_size (int size)
{
    BUFFER *buffer;

    if (buf_free == NULL)
        buffer = (BUFFER *) alloc_perm (sizeof (*buffer));
    else
    {
        buffer = buf_free;
        buf_free = buf_free->next;
    }

    buffer->next = NULL;
    buffer->state = BUFFER_SAFE;
    buffer->size = get_size (size);
    if (buffer->size == -1)
    {
        bug ("new_buf: buffer size %d too large.", size);
        exit (1);
    }
    buffer->string = (char *) alloc_mem (buffer->size);
    buffer->string[0] = '\0';
    VALIDATE (buffer);

    return buffer;
}


void free_buf (BUFFER * buffer)
{
    if (!IS_VALID (buffer))
        return;

    free_mem (buffer->string, buffer->size);
    buffer->string = NULL;
    buffer->size = 0;
    buffer->state = BUFFER_FREED;
    INVALIDATE (buffer);

    buffer->next = buf_free;
    buf_free = buffer;
}


bool add_buf (BUFFER * buffer, char *string)
{
    int len;
    char *oldstr;
    int oldsize;

    oldstr = buffer->string;
    oldsize = buffer->size;

    if (buffer->state == BUFFER_OVERFLOW)    /* don't waste time on bad strings! */
        return FALSE;

    len = strlen (buffer->string) + strlen (string) + 1;

    while (len >= buffer->size)
    {                            /* increase the buffer size */
        buffer->size = get_size (buffer->size + 1);
        {
            if (buffer->size == -1)
            {                    /* overflow */
                buffer->size = oldsize;
                buffer->state = BUFFER_OVERFLOW;
                bug ("buffer overflow past size %d", buffer->size);
                return FALSE;
            }
        }
    }

    if (buffer->size != oldsize)
    {
        buffer->string = (char *) alloc_mem (buffer->size);

        strcpy (buffer->string, oldstr);
        free_mem (oldstr, oldsize);
    }

    strcat (buffer->string, string);
    return TRUE;
}


void clear_buf (BUFFER * buffer)
{
    buffer->string[0] = '\0';
    buffer->state = BUFFER_SAFE;
}


char *buf_string (BUFFER * buffer)
{
    return buffer->string;
}

/* stuff for recycling mobprograms */
PROG_LIST *mprog_free;
PROG_LIST *oprog_free;
PROG_LIST *rprog_free;

PROG_LIST *new_mprog (void)
{
    static PROG_LIST mp_zero;
    PROG_LIST *mp;

    if (mprog_free == NULL)
        mp = (PROG_LIST *) alloc_perm (sizeof (*mp));
    else
    {
        mp = mprog_free;
        mprog_free = mprog_free->next;
    }

    *mp = mp_zero;
    mp->vnum = 0;
    mp->trig_type = 0;
    mp->code = str_dup ("");
    VALIDATE (mp);
    return mp;
}

void free_mprog (PROG_LIST * mp)
{
    if (!IS_VALID (mp))
        return;

    INVALIDATE (mp);
    mp->next = mprog_free;
    mprog_free = mp;
}

HELP_AREA *had_free;

HELP_AREA *new_had (void)
{
    HELP_AREA *had;
    static HELP_AREA zHad;

    if (had_free)
    {
        had = had_free;
        had_free = had_free->next;
    }
    else
        had = (HELP_AREA *) alloc_perm (sizeof (*had));

    *had = zHad;

    return had;
}

HELP_DATA *help_free;

HELP_DATA *new_help (void)
{
    HELP_DATA *help;

    if (help_free)
    {
        help = help_free;
        help_free = help_free->next;
    }
    else
        help = (HELP_DATA *) alloc_perm (sizeof (*help));

    return help;
}

void free_help (HELP_DATA * help)
{
    free_string (help->keyword);
    free_string (help->text);
    help->next = help_free;
    help_free = help;
}

/* stuff for recycling store structures */

STORE_DATA *store_free;

STORE_DATA *new_store(void)
{
    static STORE_DATA store_zero;
    STORE_DATA *store;

    if (store_free == NULL)
	store = (STORE_DATA *) alloc_perm(sizeof(*store));
    else
    {
	store = store_free;
	store_free = store_free->next;
    }

    *store = store_zero;
    VALIDATE(store);
    store->owner = &str_empty[0];
    store->coffer = 0;
    return store;
}

void free_store(STORE_DATA *store)
{
    if (!IS_VALID(store))
	return;

    logf("Free_store", 0);
    free_string(store->owner);
    INVALIDATE(store);

    store->next = store_free;
    store_free = store;
}


/* stuff for recycling store list structures */

STORE_LIST_DATA *slist_free;

STORE_LIST_DATA *new_slist(void)
{
    static STORE_LIST_DATA slist_zero;
    STORE_LIST_DATA *slist;

    if (slist_free == NULL)
	slist = (STORE_LIST_DATA *) alloc_perm(sizeof(*slist));
    else
    {
	slist = slist_free;
	slist_free = slist_free->next;
    }

    *slist = slist_zero;
    VALIDATE(slist);
    return slist;
}

void free_slist(STORE_LIST_DATA *slist)
{
    if (!IS_VALID(slist))
	return;

    INVALIDATE(slist);

    slist->next = slist_free;
    slist_free = slist;
}






/* stuff for recycling sold list structures */

SOLD_LIST_DATA *soldlist_free;

SOLD_LIST_DATA *new_soldlist(void)
{
    static SOLD_LIST_DATA slist_zero;
    SOLD_LIST_DATA *slist;

    if (soldlist_free == NULL)
	slist = (SOLD_LIST_DATA *) alloc_perm(sizeof(*slist));
    else
    {
	slist = soldlist_free;
	soldlist_free = soldlist_free->next;
    }

    *slist = slist_zero;
    VALIDATE(slist);
    return slist;
}

void free_soldlist(SOLD_LIST_DATA *slist)
{
    if (!IS_VALID(slist))
	return;

    INVALIDATE(slist);

    free_string(slist->name);
    slist->next = soldlist_free;
    soldlist_free = slist;
}



/* stuff for recycling riddle list structures */

RIDDLE_DATA *riddle_free;

RIDDLE_DATA *new_riddle(void)
{
    static RIDDLE_DATA riddle_zero;
    RIDDLE_DATA *riddle;

    if (riddle_free == NULL)
	riddle = (RIDDLE_DATA *) alloc_perm(sizeof(*riddle));
    else
    {
	riddle = riddle_free;
	riddle_free = riddle_free->next;
    }

    *riddle = riddle_zero;
    VALIDATE(riddle);
    return riddle;
}

void free_riddle(RIDDLE_DATA *riddle)
{
    if (!IS_VALID(riddle))
	return;

    INVALIDATE(riddle);

    free_string(riddle->question);
    free_string(riddle->answer);

    riddle->next = riddle_free;
    riddle_free = riddle;
}

/* recycling of name stuff */
struct idName *nameFree;

struct idName *newName( void )
{
  static struct idName nameZero;
  struct idName *name;

  if ( nameFree == NULL )
  {
    name = (struct idName *) alloc_perm( sizeof( *name ) );
  }
  else
  {
    name = nameFree;
    nameFree = nameFree->next;
  }

  *name = nameZero;
  name->name = &str_empty[0];

  
  return name;
}

void freeName( struct idName *name )
{
  free_string(name->name);
  name->next = nameFree;
  nameFree = name;
}

ACCOUNT_DATA *account_free;

ACCOUNT_DATA *new_account (void)
{
  static ACCOUNT_DATA account_zero;
  ACCOUNT_DATA *account;

  if (account_free == NULL)
     account = (ACCOUNT_DATA *) alloc_perm (sizeof (*account));
   else
   {
     account = account_free;
     account_free = account_free->next;
   }

  *account = account_zero;
  account->name = str_dup("None");
  account->password = str_dup("None");
  account->player = NULL;
  return account;
}

void free_account (ACCOUNT_DATA *account)
{
  ACCT_PLR_DATA *player;
  ACCT_PLR_DATA *player_next;

  free_string(account->name);
  free_string(account->password);
  for (player = account->player; player != NULL; player = player_next)
  {
    player_next = player->next;
    free_acct_plr(player);
  }
  account->next = account_free;
  account_free = account;
}

ACCT_PLR_DATA *acct_plr_free;

ACCT_PLR_DATA *new_acct_plr (void)
{
  static ACCT_PLR_DATA acct_plr_zero;
  ACCT_PLR_DATA *acct_plr;
  
  if (acct_plr_free == NULL)
    acct_plr = (ACCT_PLR_DATA *) alloc_perm (sizeof (*acct_plr));
  else
  {
    acct_plr = acct_plr_free;
    acct_plr_free = acct_plr_free->next;
  }

  *acct_plr = acct_plr_zero;
  acct_plr->name = str_dup("None");
  acct_plr->password = 0;
  acct_plr->primary = 0;
  return acct_plr;
}

void free_acct_plr (ACCT_PLR_DATA *acct_plr)
{
  free_string(acct_plr->name);
  acct_plr->next = acct_plr_free;
  acct_plr_free = acct_plr;
}

PROG_LIST *new_oprog(void)
{
   static PROG_LIST op_zero;
   PROG_LIST *op;
         
   if (oprog_free == NULL)
       op = (PROG_LIST *) alloc_perm(sizeof(*op));
   else
   { 
       op = oprog_free;
       oprog_free=oprog_free->next;
   }
    
   *op = op_zero;
   op->vnum             = 0;
   op->trig_type        = 0;
   op->code             = str_dup("");
   VALIDATE(op);
   return op;
}

void free_oprog(PROG_LIST *op)
{
   if (!IS_VALID(op)) 
      return;

   INVALIDATE(op);
   op->next = oprog_free;
   oprog_free = op;
}

PROG_LIST *new_rprog(void)  
{
   static PROG_LIST rp_zero;
   PROG_LIST *rp;
       
   if (rprog_free == NULL)
       rp = (PROG_LIST *) alloc_perm(sizeof(*rp));
   else
   {
       rp = rprog_free;
       rprog_free=rprog_free->next;
   }
   
   *rp = rp_zero;
   rp->vnum             = 0;
   rp->trig_type        = 0;
   rp->code             = str_dup("");
   VALIDATE(rp);
   return rp;
}
 
void free_rprog(PROG_LIST *rp)
{
   if (!IS_VALID(rp))
      return;
   
   INVALIDATE(rp); 
   rp->next = rprog_free;
   rprog_free = rp;
}

PK_DATA *pk_free;

PK_DATA *new_pk (void)
{
  static PK_DATA pk_zero;
  PK_DATA *pk;
  
  if (pk_free == NULL)
    pk = (PK_DATA *) alloc_perm (sizeof (*pk));
  else
  {
    pk = pk_free;
   pk_free = pk_free->next;
  }

  *pk = pk_zero;
  return pk;
}

void free_pk (PK_DATA *pk)
{
  pk->next = pk_free;
  pk_free = pk;
}

DELAY_CODE *delay_free;

DELAY_CODE *new_delay (void)
{
  static DELAY_CODE delay_zero;
  DELAY_CODE *delay;
  
  if (delay_free == NULL)
    delay = (DELAY_CODE *) alloc_perm (sizeof (*delay));
  else
  {
    delay = delay_free;
    delay_free = delay_free->next;
  }

  *delay = delay_zero;

  delay->time = 0;
  delay->vnum = 0;
  delay->code = str_dup("None");
  delay->mob = NULL;
  delay->obj = NULL;
  delay->room = NULL;
  delay->ch = NULL;
  delay->arg1 = NULL;
  delay->arg2 = NULL;
  delay->line = 0;
  delay->next = NULL;

  return delay;
}

void free_delay (DELAY_CODE *delay)
{
  delay->time = 0;
  delay->vnum = 0;
  free_string(delay->code);
  delay->mob = NULL;
  delay->obj = NULL;
  delay->room = NULL;
  delay->ch = NULL;
  delay->arg1 = NULL;
  delay->arg2 = NULL;
  delay->line = 0;

  delay->next = delay_free;
  delay_free = delay;
}

