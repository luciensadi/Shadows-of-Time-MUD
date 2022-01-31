/*
  SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
  See license.doc for distribution terms.   SillyMUD is based on DIKUMUD

  Modifications by Rip in attempt to port to merc 2.1
*/

/*
  Modified by Turtle for Merc22 (07-Nov-94)

  I got this one from ftp.atinc.com:/pub/mud/outgoing/track.merc21.tar.gz.
  It cointained 5 files: README, hash.c, hash.h, skills.c, and skills.h.
  I combined the *.c and *.h files in this hunt.c, which should compile
  without any warnings or errors.
*/



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"

void spellmob_detect args (( CHAR_DATA *ch));

/*				Copyright 1997, Lord Thomas Burbridge
 *
 * Mobile memory procedures.
 */


MEM_DATA *get_mem_data(CHAR_DATA *ch, CHAR_DATA *target)
{
    MEM_DATA *remember;

    if ( !IS_NPC(ch))
    {
	bug( "get_mem_data: ch not NPC", 0);
	return NULL;
    }

    if ( ch == NULL )
    {
	bug( "get_mem_data:  NULL ch", 0 );
	return NULL;
    }

    if ( target == NULL )
    {
	bug( "get_mem_data:  NULL target", 0 );
	return NULL;
    }

    for ( remember = ch->memory ; remember != NULL ; remember = remember->next )
    {
	if ( remember->id == target->id )
	    return remember;
    }

    return NULL;
}

void mob_remember(CHAR_DATA *ch, CHAR_DATA *target, int reaction)
{
    MEM_DATA *remember;

    if ( !IS_NPC(ch) )
    {
	bug( "mob_remember: ch not NPC", 0);
	return;
    }

    if (IS_NPC(target))
    {
    	return;
    }

    if ( ch == NULL )
    {
	bug( "mob_remember:  NULL ch", 0 );
	return;
    }

    if ( target == NULL )
    {
	bug( "mob_remember:  NULL target", 0 );
	return;
    }


    if ((remember=get_mem_data(ch,target))==NULL)
    {

    remember = new_mem_data();
    remember->id 	= target->id;
    remember->when	= current_time;
    remember->next = ch->memory;
    ch->memory = remember;
    SET_BIT( remember->reaction, reaction);
    }
    return;
}

void mem_fade(CHAR_DATA *ch)
{
    MEM_DATA *remember, *remember_next;

    if ( ch == NULL )
    {
	bug( "mem_fade:  NULL ch", 0 );
	return;
    }

    if ( !IS_NPC(ch))
    {
	bug("mem_fade: ch not NPC", 0);
	return;
    }

    if ( ch->memory == NULL )
	return;

    for ( remember = ch->memory ; remember != NULL ; remember = remember_next )
    {
	remember_next = remember->next;

	if (IS_NPC(ch)
        && get_char_id( remember->id)
	&&  can_see(ch, get_char_id( remember->id ))
	&&  ch->hunting == NULL
	&&  IS_SET(remember->reaction, MEM_HOSTILE))
	    remember_victim(ch, get_char_id( remember->id ));

	if ( (current_time - remember->when) > (3600 * 48) )
	    mob_forget( ch, remember );
    }

    return;
}

void mem_clear(CHAR_DATA *ch)
{
    MEM_DATA *remember, *remember_next;

    if ( ch == NULL )
    {
	bug( "mem_fade:  NULL ch", 0 );
	return;
    }

    if ( !IS_NPC(ch))
    {
	bug("mem_fade: ch not NPC", 0);
	return;
    }

    if ( ch->memory == NULL )
	return;

    for ( remember = ch->memory ; remember != NULL ; remember = remember_next )
    {
	remember_next = remember->next;

	if (IS_NPC(ch)
        && get_char_id( remember->id)
	&&  can_see(ch, get_char_id( remember->id ))
	&&  ch->hunting == NULL
	&&  IS_SET(remember->reaction, MEM_HOSTILE))
	    remember_victim(ch, get_char_id( remember->id ));

	mob_forget( ch, remember );
    }

    return;
}


void mob_forget( CHAR_DATA *ch, MEM_DATA *memory )
{

    if ( !IS_NPC(ch) )
    {
	bug( "mob_forget:  ch not NPC", 0);
	return;
    }
    if ( ch == NULL )
    {
	bug( "mob_forget:  NULL ch", 0);
	return;
    }

    if ( memory == NULL )
	return;

    if ( memory == ch->memory )
    {
	ch->memory = memory->next;
    }
    else
    {
	MEM_DATA *prev;

	for ( prev = ch->memory ; prev != NULL ; prev = prev->next )
	{
	    if ( prev->next == memory )
	    {
		prev->next = memory->next;
		break;
	    }

	    if ( prev == NULL )
	    {
		bug( "mob_forget:  memory not found", 0);
		return;
	    }
	}
    }
    free_mem_data(memory);
    return;
}

void remember_victim( CHAR_DATA *ch, CHAR_DATA *victim )
{
MEM_DATA *remember,*remember_next;
    if(( ch->in_room == victim->in_room) && ch->level <= 0 )
    {
      act( "$n looks $N over and says grimly, '`!I remember you!`*'",
		  ch, NULL, victim, TO_NOTVICT );
      act( "$n looks you and says grimly, '`!I remember you!`*'",
		  ch, NULL, victim, TO_VICT );
      act( "You look $N over and say grimly, '`!I remember you!`*'",
		  ch, NULL, victim, TO_CHAR);
      multi_hit( ch, victim, TYPE_UNDEFINED );
      ch->hunting = victim;
      if ( ch == NULL )
    {
	bug( "mem_fade:  NULL ch", 0 );
	return;
    }

    if ( !IS_NPC(ch))
    {
	bug("mem_fade: ch not NPC", 0);
	return;
    }

    if ( ch->memory == NULL )
	return;
    for ( remember = ch->memory ; remember != NULL ; remember = remember_next )
    {
	remember_next = remember->next;

	if (IS_NPC(ch)
        &&  get_char_id( remember->id)
	&&  can_see(ch, get_char_id( remember->id ))
	&&  IS_SET(remember->reaction, MEM_HOSTILE))
	    mob_forget( ch, remember );
    }
      return;
    }

  return;
}

/*
void bcopy(char *s1,char* s2,int len);
void bzero(char *sp,int len);
*/

struct hash_link
{
  int			key;
  struct hash_link	*next;
  void			*data;
};

struct hash_header
{
  int			rec_size;
  int			table_size;
  int			*keylist, klistsize, klistlen; /* this is really lame,
							  AMAZINGLY lame */
  struct hash_link	**buckets;
};

#define WORLD_SIZE	30000
#define	HASH_KEY(ht,key)((((unsigned int)(key))*17)%(ht)->table_size)



struct hunting_data
{
  char			*name;
  struct char_data	**victim;
};

struct room_q
{
  int		room_nr;
  struct room_q	*next_q;
};

struct nodes
{
  int	visited;
  int	ancestor;
};

#define IS_DIR		(get_room_index(q_head->room_nr)->exit[i])
#define GO_OK		(!IS_SET( IS_DIR->exit_info, EX_CLOSED ))
#define GO_OK_SMARTER	1



void init_hash_table(struct hash_header	*ht,int rec_size,int table_size)
{
  ht->rec_size	= rec_size;
  ht->table_size= table_size;
  ht->buckets	= (struct hash_link **)calloc(sizeof(struct hash_link**),table_size);
  ht->keylist	= (int*) malloc(sizeof(ht->keylist)*(ht->klistsize=128));
  ht->klistlen	= 0;
}

void init_world(ROOM_INDEX_DATA *room_db[])
{
  /* zero out the world */
  bzero((char *)room_db,sizeof(ROOM_INDEX_DATA *)*WORLD_SIZE);
}

void destroy_hash_table(struct hash_header *ht)
{
  int			i;
  struct hash_link	*scan,*temp;

  for(i=0;i<ht->table_size;i++)
    for(scan=ht->buckets[i];scan;)
      {
	temp = scan->next;
//	(*gman)();
// (scan->data);
	free(scan);
	scan = temp;
      }
  free(ht->buckets);
  free(ht->keylist);
}

void _hash_enter(struct hash_header *ht,int key,void *data)
{
  /* precondition: there is no entry for <key> yet */
  struct hash_link	*temp;
  int			i;

  temp		= (struct hash_link *)malloc(sizeof(struct hash_link));
  temp->key	= key;
  temp->next	= ht->buckets[HASH_KEY(ht,key)];
  temp->data	= data;
  ht->buckets[HASH_KEY(ht,key)] = temp;
  if(ht->klistlen>=ht->klistsize)
    {
      ht->keylist = (int *)realloc(ht->keylist,sizeof(*ht->keylist)*
				   (ht->klistsize*=2));
    }
  for(i=ht->klistlen;i>=0;i--)
    {
      if(ht->keylist[i-1]<key)
	{
	  ht->keylist[i] = key;
	  break;
	}
      ht->keylist[i] = ht->keylist[i-1];
    }
  ht->klistlen++;
}

ROOM_INDEX_DATA *room_find(ROOM_INDEX_DATA *room_db[],int key)
{
  return((key<WORLD_SIZE&&key>-1)?room_db[key]:0);
}

void *hash_find(struct hash_header *ht,int key)
{
  struct hash_link *scan;

  scan = ht->buckets[HASH_KEY(ht,key)];

  while(scan && scan->key!=key)
    scan = scan->next;

  return scan ? scan->data : NULL;
}

int room_enter(ROOM_INDEX_DATA *rb[],int key,ROOM_INDEX_DATA *rm)
{
  ROOM_INDEX_DATA *temp;

  temp = room_find(rb,key);
  if(temp) return(0);

  rb[key] = rm;
  return(1);
}

int hash_enter(struct hash_header *ht,int key,void *data)
{
  void *temp;

  temp = hash_find(ht,key);
  if(temp) return 0;

  _hash_enter(ht,key,data);
  return 1;
}

ROOM_INDEX_DATA *room_find_or_create(ROOM_INDEX_DATA *rb[],int key)
{
  ROOM_INDEX_DATA *rv;

  rv = room_find(rb,key);
  if(rv) return rv;

  rv = (ROOM_INDEX_DATA *)malloc(sizeof(ROOM_INDEX_DATA));
  rb[key] = rv;

  return rv;
}

void *hash_find_or_create(struct hash_header *ht,int key)
{
  void *rval;

  rval = hash_find(ht, key);
  if(rval) return rval;

  rval = (void*)malloc(ht->rec_size);
  _hash_enter(ht,key,rval);

  return rval;
}

int room_remove(ROOM_INDEX_DATA *rb[],int key)
{
  ROOM_INDEX_DATA *tmp;

  tmp = room_find(rb,key);
  if(tmp)
    {
      rb[key] = 0;
      free(tmp);
    }
  return(0);
}

void *hash_remove(struct hash_header *ht,int key)
{
  struct hash_link **scan;

  scan = ht->buckets+HASH_KEY(ht,key);

  while(*scan && (*scan)->key!=key)
    scan = &(*scan)->next;

  if(*scan)
    {
      int		i;
      struct hash_link	*temp, *aux;

      temp	= (struct hash_link *) (*scan)->data;
      aux	= *scan;
      *scan	= aux->next;
      free(aux);

      for(i=0;i<ht->klistlen;i++)
	if(ht->keylist[i]==key)
	  break;

      if(i<ht->klistlen)
	{
	  bcopy((char *)ht->keylist+i+1,(char *)ht->keylist+i,(ht->klistlen-i)
		*sizeof(*ht->keylist));
	  ht->klistlen--;
	}

      return temp;
    }

  return NULL;
}

/*
void room_iterate(ROOM_INDEX_DATA *rb[],void (*func)(),void *cdata)
{
  register int i;

  for(i=0;i<WORLD_SIZE;i++)
    {
      ROOM_INDEX_DATA *temp;

      temp = room_find(rb,i);
      if(temp) (*func)(i,temp,cdata);
    }
}
*/
/*
void hash_iterate(struct hash_header *ht,void (*func)(),void *cdata)
{
  int i;

  for(i=0;i<ht->klistlen;i++)
    {
      void		*temp;
      register int	key;

      key = ht->keylist[i];
      temp = hash_find(ht,key);
      (*func)(key,temp,cdata);
      if(ht->keylist[i]!=key) * They must have deleted this room *
	i--;		      * Hit this slot again. *
    }
}
*/


int exit_ok( EXIT_DATA *pexit )
{
  ROOM_INDEX_DATA *to_room;

  if ( ( pexit == NULL )
  ||   ( to_room = pexit->u1.to_room ) == NULL )
    return 0;

  return 1;
}

void donothing()
{
  return;
}

int find_path( int in_room_vnum, int out_room_vnum, CHAR_DATA *ch,
	       int depth, int in_zone )
{
  struct room_q		*tmp_q, *q_head, *q_tail;
  struct hash_header	x_room;
  int			i, count=0, thru_doors;
  int tmp_room=0;
  ROOM_INDEX_DATA	*herep;
  ROOM_INDEX_DATA	*startp;
  EXIT_DATA		*exitp;

  if ( depth <0 )
    {
      thru_doors = TRUE;
      depth = -depth;
    }
  else
    {
      thru_doors = FALSE;
    }

  startp = get_room_index( in_room_vnum );

  init_hash_table( &x_room, sizeof(int), 2048 );
  hash_enter( &x_room, in_room_vnum, (void *) - 1 );

  /* initialize queue */
  q_head = (struct room_q *) malloc(sizeof(struct room_q));
  q_tail = q_head;
  q_tail->room_nr = in_room_vnum;
  q_tail->next_q = 0;

  while(q_head)
    {
      herep = get_room_index( q_head->room_nr );
      /* for each room test all directions */
      if( herep->area == startp->area || !in_zone )
	{
	  /* only look in this zone...
	     saves cpu time and  makes world safer for players  */
	  for( i = 0; i <= 5; i++ )
	    {
	      exitp = herep->exit[i];
	      if( exit_ok(exitp) && ( thru_doors ? GO_OK_SMARTER : GO_OK ) )
		{
		  /* next room */
		  tmp_room = herep->exit[i]->u1.to_room->vnum;
		  if( tmp_room != out_room_vnum )
		    {
		      /* shall we add room to queue ?
			 count determines total breadth and depth */
		      if( !hash_find( &x_room, tmp_room )
			 && ( count < depth ))
			{
			  count++;
			  /* mark room as visted and put on queue */

			  tmp_q = (struct room_q *)
			    malloc(sizeof(struct room_q));
			  tmp_q->room_nr = tmp_room;
			  tmp_q->next_q = 0;
			  q_tail->next_q = tmp_q;
			  q_tail = tmp_q;

			  /* ancestor for first layer is the direction */
			  hash_enter( &x_room, tmp_room,
				     ((long)hash_find(&x_room,q_head->room_nr)
				      == -1) ? (void*)(i+1)
				     : hash_find(&x_room,q_head->room_nr));
			}
		    }
		  else
		    {
		      /* have reached our goal so free queue */
		      tmp_room = q_head->room_nr;
		      for(;q_head;q_head = tmp_q)
			{
			  tmp_q = q_head->next_q;
			  free(q_head);
			}
		      /* return direction if first layer */
		      if ((long)hash_find(&x_room,tmp_room)==-1)
			{
			  if (x_room.buckets)
			    {
			      /* junk left over from a previous track */
			      destroy_hash_table(&x_room);
			    }
			  return(i);
			}
		      else
			{
			  /* else return the ancestor */
			  int i;

			  i = (long)hash_find(&x_room,tmp_room);
			  if (x_room.buckets)
			    {
			      /* junk left over from a previous track */
			      destroy_hash_table(&x_room);
			    }
			  return( -1+i);
			}
		    }
		}
	    }
	}

      /* free queue head and point to next entry */
      tmp_q = q_head->next_q;
      free(q_head);
      q_head = tmp_q;
    }

  /* couldn't find path */
  if( x_room.buckets )
    {
      /* junk left over from a previous track */
      destroy_hash_table( &x_room);
    }
  return -1;
}



void do_track( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char tmp[MAX_STRING_LENGTH];
  char arg[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int direction;
  bool fArea;

  one_argument( argument, arg );

  if( arg[0] == '\0' )
    {
      send_to_char( "Whom are you trying to hunt?\n\r", ch );
      return;
    }

  /* only imps can hunt to different areas */
  fArea = ( get_trust(ch) < MAX_LEVEL );

  if( fArea )
    victim = get_char_area( ch, arg );
  else
    victim = get_char_world( ch, arg );

  if( victim == NULL )
    {
      send_to_char("No-one around by that name.\n\r", ch );
      return;
    }

  if( ch->in_room == victim->in_room )
    {
      act( "$N is here!", ch, NULL, victim, TO_CHAR );
      return;
    }

  /*
   * Deduct some movement.
   */
  if( ch->move > 2 )
    ch->move -= 3;
  else
    {
      send_to_char( "You're too exhausted to hunt anyone!\n\r", ch );
      return;
    }

  act( "$n carefully sniffs the air.", ch, NULL, NULL, TO_ROOM );
  WAIT_STATE( ch, skill_table[gsn_track].beats );
  direction = find_path( ch->in_room->vnum, victim->in_room->vnum,
			ch, -40000, fArea );

  if( direction == -1 )
    {
      act( "You couldn't find a path to $N from here.",
	  ch, NULL, victim, TO_CHAR );
      return;
    }

  if( direction < 0 || direction > 5 )
    {
      send_to_char( "Hmm... Something seems to be wrong.\n\r", ch );
      return;
    }

  /*
   * Give a random direction if the player misses the die roll.
   */
  if( ( IS_NPC (ch) && number_percent () > ch->pcdata->learned[gsn_track] )
     || (!IS_NPC (ch) && number_percent () > ch->pcdata->learned[gsn_track] ) )
    {
      do
	{
	  direction = number_door();
	}
      while( ( ch->in_room->exit[direction] == NULL )
	    || ( ch->in_room->exit[direction]->u1.to_room == NULL) );
    }

  /*
   * Display the results of the search.
   */

  move_char(ch,direction,FALSE, FALSE);
  colourstrip(tmp,dir_name[direction]);
  sprintf( buf, "The trail $N left leads %s from here.\n\rYou follow his trail.\n\r", tmp );
  act( buf, ch, NULL, victim, TO_CHAR );
  check_improve(ch,gsn_track,TRUE,3);
  return;
}

//Randomly used to spell mob up with detects or cure blind
void spellmob_detect ( CHAR_DATA *ch)
{
   void *vo;
   int sn;
   int randnum;
   int target = TARGET_CHAR;
   vo =(void *) ch;
   if (!IS_NPC(ch))
       return;
   randnum = number_range(1,3);
   switch(randnum)
   {
   	case 1 :
           if (!IS_AFFECTED(ch, AFF_DETECT_INVIS))
           {
   	       sn = skill_lookup("detect invis");
   	       (*skill_table[sn].spell_fun) ( sn, ch->level, ch, vo, target);
           }
           break;
        case 2 :
           if (!IS_AFFECTED(ch, AFF_DETECT_HIDDEN))
           {
   	       sn =skill_lookup("detect hidden");
   	       (*skill_table[sn].spell_fun) ( sn, ch->level, ch, vo, target);
           }
           break;
        case 3 :
           if (IS_AFFECTED(ch, AFF_BLIND))
           {
   	       sn = skill_lookup("cure blindness");
   	       (*skill_table[sn].spell_fun) ( sn, ch->level, ch, vo, target);
           }
           break;
   }
   return;
}



void hunt_victim( CHAR_DATA *ch )
{
  int		dir;
  bool		found;
  CHAR_DATA	*tmp;

  if( ch == NULL || ch->hunting == NULL || !IS_NPC(ch)  )
    return;

  /*
   * Make sure the victim still exists.
   */
  for( found = 0, tmp = char_list; tmp && !found; tmp = tmp->next )
    if( ch->hunting == tmp )
      found = 1;

  if( !found || !can_see( ch, ch->hunting ) )
    {
      if (number_bits(2) == 0)
        spellmob_detect(ch);
      act( "$n spits in utter disgust and says, '`!Damn!  My prey is gone!!`*",
		  ch, NULL, NULL, TO_ROOM);
      act( "You spit in utter disgust and say, 'Damn! My prey is gone!!'",
		  ch,NULL,NULL, TO_CHAR);
      mob_remember(ch,ch->hunting,MEM_HOSTILE);
      ch->hunting = NULL;
      return;
    }

  /*
 *Is the mob afraid wimpy or low in hp???
 */
  if ((IS_SET (ch->act, ACT_WIMPY) && number_bits (2) == 0)
	  || ch->hit < ch->max_hit / 5
      || (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL
      && ch->master->in_room != ch->in_room))
      {
	    mob_remember(ch,ch->hunting,MEM_AFRAID);
            do_function (ch, &do_flee, "");
			ch->hunting = NULL;
			return;
      }

  if( ch->in_room == ch->hunting->in_room )
    {
      act( "$n glares at $N and screams, '`!You shall DIE for attacking me!`*'",
	  ch, NULL, ch->hunting, TO_NOTVICT );
      act( "$n glares at you and screams, '`!You shall DIE for attacking me!`*'",
	  ch, NULL, ch->hunting, TO_VICT );
      act( "You glare at $N and say, '`!You shall DIE for attacking me!`!",
	  ch, NULL, ch->hunting, TO_CHAR);
      multi_hit( ch, ch->hunting, TYPE_UNDEFINED );
      //ch->hunting = NULL;
      return;
    }


  dir = find_path( ch->in_room->vnum, ch->hunting->in_room->vnum,
		  ch, -40000, TRUE );

  if( dir < 0 || dir > 5 )
    {
      act( "$n says '`!Damn!  Lost $M`*!'", ch, NULL, ch->hunting, TO_ROOM );
      mob_remember(ch,ch->hunting,MEM_HOSTILE);
      ch->hunting = NULL;
      return;
    }

  /*
   * Give a random direction if the mob misses the die roll.
   */
  if( number_percent () > 75 )        /* @ 25% */
    {
      do
        {
	  dir = number_door();
        }
      while( ( ch->in_room->exit[dir] == NULL )
	    || ( ch->in_room->exit[dir]->u1.to_room == NULL ) );
    }


  if( IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED ) )
    {
      do_open( ch, (char *) dir_name[dir] );
      return;
    }



  if( IS_SET(ch->in_room->exit[dir]->u1.to_room->room_flags, ROOM_SAFE ))
  {
	  return;
  }
  else
  {
  move_char( ch, dir,FALSE, FALSE);
  act( "$n looks around for someone with death in his eyes.", ch, NULL, ch->hunting, TO_ROOM );
  }
  return;
}


/*
*Corpse Retrieval Code
*/
OBJ_DATA *find_plrcorpse (CHAR_DATA *ch)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	/*Check if there is a plr corpse*/
	for (obj = object_list; obj != NULL; obj = obj_next)
	{
		obj_next=obj->next;
        if (obj->item_type == ITEM_CORPSE_PC
			&& !str_cmp(ch->name, obj->owner))
			return obj;
		else
            continue;


    }

	return NULL;
}
void cr (CHAR_DATA *ch)
{
	char buf[MSL];

	CHAR_DATA *fakech;


		if ((fakech = get_room_index(find_plrcorpse(ch)->in_room->vnum)->people) != NULL)
		{
			sprintf(buf, "A bunch of shifty looking labourers wheel up pick up %s and wheel off into the distance.", find_plrcorpse(ch)->short_descr);
			act(buf, fakech, NULL, NULL, TO_ROOM);
			sprintf(buf, "A bunch of shifty looking labourers wheel up pick up %s and wheel off into the distance.", find_plrcorpse(ch)->short_descr);
			act(buf, fakech, NULL, NULL, TO_CHAR);
		}
		obj_from_room(find_plrcorpse(ch));
		obj_to_room(find_plrcorpse(ch), get_room_index( ch->in_room->vnum ));
		send_to_char("You pray to the Creator for assistance.\n\r", ch);
		sprintf(buf, "A bunch of shifty looking labourers wheel a %s upon a cart and drops it at %s's feet!", find_plrcorpse(ch)->short_descr,ch->name);
		act(buf, ch, NULL, NULL, TO_ROOM);
		sprintf(buf, "A bunch of shifty looking labourers wheel a %s upon a cart and drops it at your feet!", find_plrcorpse(ch)->short_descr);
		act(buf, ch, NULL, NULL, TO_CHAR);


    return;
}
void do_cr (CHAR_DATA *ch, char *argument)
{
	char arg[MIL];
	char buf[MSL];
	int convsilver = 100;
	int getgold = ch->level + (20*ch->level);
	int count = 0;
	OBJ_DATA *coupon;
	OBJ_DATA *Cobj;
	OBJ_DATA *Cobj_next;


	one_argument(argument,arg);


	/*Default command shown*/
	if (arg[0] == '\0')
    {
        send_to_char ("Syntax: CORPSERETRIEVE YES\n\r", ch);
		send_to_char ("",ch);
		send_to_char ("WARNING: Using this command will result in loss of gold/silver in\n\r",ch);
		send_to_char ("         the bank. If not enough funds than a random item will be\n\r",ch);
		sprintf (buf,"         taken from your corpse. It will cost you the equivilant of %d gold\n\r",getgold);
		send_to_char (buf,ch);
		send_to_char ("",ch);
		send_to_char ("Syntax: CORPSERETRIEVE COUPON\n\r",ch);
		send_to_char ("         Retrieves your corpse with no loss. Can be purchased at the\n\r",ch);
		send_to_char ("         Questmaster.\n\r",ch);
        return;
    }


    if (!str_cmp(arg,"YES"))
	{
		if (find_plrcorpse(ch) == NULL)
		{
			send_to_char("You do not have a corpse to retrieve.\n\r",ch);
			return;
		}
		if (ch->pcdata->safe_timer > 0)
		{
			send_to_char("Your corpse is still warm and your murderer is still in the area.\n\r",ch);
			send_to_char("The labourers have refused to approach your corpse until they deem it safe. \n\r",ch);
			return;
		}

			if(ch->gold_in_bank > getgold)
			{
				ch->gold_in_bank -= getgold;
				cr(ch);
				return;
			}
			else if (ch->gold_in_bank < getgold
				&& (ch->gold_in_bank + ((ch->silver_in_bank)/convsilver))> getgold)
			{
				getgold -= ch->gold_in_bank;
				ch->gold_in_bank = 0;
				ch->silver_in_bank -= getgold*convsilver;
				cr(ch);
				return;
			}
			else
			{
				for (Cobj = find_plrcorpse(ch)->contains; Cobj != NULL; Cobj = Cobj_next)
				{
					Cobj_next = Cobj->next_content;
					if (Cobj_next!=NULL && number_range(0,count)==0)
					{
						send_to_char("You did not have sufficient funds. An random item will be taken instead.\n\r",ch);
						extract_obj(Cobj_next);
						cr(ch);
						return;
					}
					else if (Cobj_next==NULL);
					{
						send_to_char("You did not have sufficient funds. An random item will be taken instead.\n\r",ch);
						extract_obj(Cobj);
						cr(ch);
						return;
					}
                    count++;
                }
				send_to_char("Rumours have it that the labourers found nothing worth taking and left it to rot.\n\r",ch);
				return;
            }


	}
	else if (!str_cmp(arg,"COUPON"))
	{
		if (find_plrcorpse(ch) == NULL)
		{
			send_to_char("You do not have a corpse to retrieve.\n\r",ch);
			return;
		}
		if (ch->pcdata->safe_timer > 0)
		{
			send_to_char("Your corpse is still warm and your murderer is still in the area.\n\r",ch);
			send_to_char("The labourers have refused to approach your corpse until they deem it safe. \n\r",ch);
			return;
		}
		if ((coupon = get_obj_carry(ch, "cr coupon", ch)) == NULL)
                {
                   if ((coupon = get_obj_list_full(ch, "cr coupon", find_plrcorpse(ch)->contains)) == NULL)
		   {
			send_to_char("You don't have a Corpse Retrieval Coupon.\n\r",ch);
			return;
                   }
                   else
                   {
                     obj_from_obj(coupon);
                   }
		}

		extract_obj(coupon);
		cr(ch);
	}
	else
	{
		send_to_char ("Syntax: CORPSERETRIEVE YES\n\r", ch);
		send_to_char ("",ch);
		send_to_char ("WARNING: Using this command will result in loss of gold/silver in\n\r",ch);
		send_to_char ("         the bank. If not enough funds than a random item will be\n\r",ch);
		sprintf (buf,"         taken from your corpse. It will cost you the equivilant of %d gold\n\r",getgold);
		send_to_char (buf,ch);
		send_to_char ("",ch);
		send_to_char ("Syntax: CORPSERETRIEVE COUPON\n\r",ch);
		send_to_char ("         Retrieves your corpse with no loss. Can be purchased at the\n\r",ch);
		send_to_char ("         Questmaster.",ch);
	}
	return;
}
