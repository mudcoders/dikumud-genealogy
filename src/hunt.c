/*
 * SillyMUD Distribution V1.1b             (c) 1993 SillyMUD Developement
 * See license.doc for distribution terms.   SillyMUD is based on DIKUMUD
 *
 * Modifications by Rip in attempt to port to merc 2.1
 */

/*
 * Modified by Turtle for Merc22 (07-Nov-94)
 *
 * I got this one from ftp.atinc.com:/pub/mud/outgoing/track.merc21.tar.gz.
 * It cointained 5 files: README, hash.c, hash.h, skills.c, and skills.h.
 * I combined the *.c and *.h files in this hunt.c, which should compile
 * without any warnings or errors.
 */

/*
 * Some systems don't have bcopy and bzero functions in their linked libraries.
 * If compilation fails due to missing of either of these functions,
 * define NO_BCOPY or NO_BZERO accordingly.                 -- Turtle 31-Jan-95
 */



#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "merc.h"



void bcopy(register char *s1,register char *s2,int len);
void bzero(register char *sp,int len);

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



#if defined( NO_BCOPY )
void bcopy(register char *s1,register char *s2,int len)
{
  while( len-- ) *(s2++) = *(s1++);
}
#endif

#if defined( NO_BZERO )
void bzero(register char *sp,int len)
{
  while( len-- ) *(sp++) = '\0';
}
#endif



void init_hash_table(struct hash_header	*ht,int rec_size,int table_size)
{
  ht->rec_size	= rec_size;
  ht->table_size= table_size;
  ht->buckets	= (void*)calloc(sizeof(struct hash_link**),table_size);
  ht->keylist	= (void*)malloc(sizeof(ht->keylist)*(ht->klistsize=128));
  ht->klistlen	= 0;
}

void init_world(ROOM_INDEX_DATA *room_db[])
{
  /* zero out the world */
  bzero((char *)room_db,sizeof(ROOM_INDEX_DATA *)*WORLD_SIZE);
}

void destroy_hash_table(struct hash_header *ht,void (*gman)())
{
  int			i;
  struct hash_link	*scan,*temp;

  for(i=0;i<ht->table_size;i++)
    for(scan=ht->buckets[i];scan;)
      {
	temp = scan->next;
	(*gman)(scan->data);
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
      ht->keylist = (void*)realloc(ht->keylist,sizeof(*ht->keylist)*
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

      temp	= (*scan)->data;
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
      if(ht->keylist[i]!=key) /* They must have deleted this room */
	i--;		      /* Hit this slot again. */
    }
}



int exit_ok( EXIT_DATA *pexit )
{
  ROOM_INDEX_DATA *to_room;

  if ( ( pexit == NULL )
  ||   ( to_room = pexit->to_room ) == NULL )
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
  int			i, tmp_room, count=0, thru_doors;
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
		  tmp_room = herep->exit[i]->to_room->vnum;
		  if( tmp_room != out_room_vnum )
		    {
		      /* shall we add room to queue ?
			 count determines total breadth and depth */
		      if( !hash_find( &x_room, tmp_room )
			 && ( count < depth ) )
			/* && !IS_SET( RM_FLAGS(tmp_room), DEATH ) ) */
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
				     ((int)hash_find(&x_room,q_head->room_nr)
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
		      if ((int)hash_find(&x_room,tmp_room)==-1)
			{
			  if (x_room.buckets)
			    {
			      /* junk left over from a previous track */
			      destroy_hash_table(&x_room, donothing);
			    }
			  return(i);
			}
		      else
			{
			  /* else return the ancestor */
			  int i;
			  
			  i = (int)hash_find(&x_room,tmp_room);
			  if (x_room.buckets)
			    {
			      /* junk left over from a previous track */
			      destroy_hash_table(&x_room, donothing);
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
      destroy_hash_table( &x_room, donothing );
    }
  return -1;
}



void do_hunt( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int direction;
  bool fArea;

  one_argument( argument, arg );

  if( arg[0] == '\0' )
    {
      send_to_char( "Whom are you trying to hunt?\r\n", ch );
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
      send_to_char("No-one around by that name.\r\n", ch );
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
      send_to_char( "You're too exhausted to hunt anyone!\r\n", ch );
      return;
    }

  act( "$n carefully sniffs the air.", ch, NULL, NULL, TO_ROOM );
  WAIT_STATE( ch, skill_table[gsn_hunt].beats );
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
      send_to_char( "Hmm... Something seems to be wrong.\r\n", ch );
      return;
    }

  /*
   * Give a random direction if the player misses the die roll.
   */
  if( ( IS_NPC (ch) && number_percent () > 75)        /* NPC @ 25% */
     || (!IS_NPC (ch) && number_percent () >          /* PC @ norm */
	 ch->pcdata->learned[gsn_hunt] ) )
    {
      do
	{
	  direction = number_door();
	}
      while( ( ch->in_room->exit[direction] == NULL )
	    || ( ch->in_room->exit[direction]->to_room == NULL) );
    }

  /*
   * Display the results of the search.
   */
  sprintf( buf, "$N is %s from here.", dir_name[direction] );
  act( buf, ch, NULL, victim, TO_CHAR );

  return;
}



void hunt_victim( CHAR_DATA *ch )
{
  int		dir;
  bool		found;
  CHAR_DATA	*tmp;

  if( ch == NULL || ch->hunting == NULL || !IS_NPC(ch) )
    return;

  /*
   * Make sure the victim still exists.
   */
  for( found = 0, tmp = char_list; tmp && !found; tmp = tmp->next )
    if( ch->hunting == tmp )
      found = 1;

  if( !found || !can_see( ch, ch->hunting ) )
    {
      do_say( ch, "Damn!  My prey is gone!!" );
      ch->hunting = NULL;
      return;
    }

  if( ch->in_room == ch->hunting->in_room )
    {
      act( "$n glares at $N and says, 'Ye shall DIE!'",
	  ch, NULL, ch->hunting, TO_NOTVICT );
      act( "$n glares at you and says, 'Ye shall DIE!'",
	  ch, NULL, ch->hunting, TO_VICT );
      act( "You glare at $N and say, 'Ye shall DIE!",
	  ch, NULL, ch->hunting, TO_CHAR);
      multi_hit( ch, ch->hunting, TYPE_UNDEFINED );
      ch->hunting = NULL;
      return;
    }

  WAIT_STATE( ch, skill_table[gsn_hunt].beats );
  dir = find_path( ch->in_room->vnum, ch->hunting->in_room->vnum,
		  ch, -40000, TRUE );

  if( dir < 0 || dir > 5 )
    {
      act( "$n says 'Damn!  Lost $M!'", ch, NULL, ch->hunting, TO_ROOM );
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
	    || ( ch->in_room->exit[dir]->to_room == NULL ) );
    }


  if( IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED ) )
    {
      do_open( ch, (char *) dir_name[dir] );
      return;
    }

  move_char( ch, dir );
  return;
}
