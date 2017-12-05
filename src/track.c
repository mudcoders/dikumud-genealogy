/***************************************************************************
 *  SillyMUD Distribution V1.1b copyright (C) 1993 SillyMUD Developement.  *
 *                                                                         *
 *  Modifications by Rip in attempt to port to Merc 2.1                    *
 *  Modified by Turtle for Merc 2.2 (07-Nov-94)                            *
 *                                                                         *
 *  Got code from ftp.atinc.com:/pub/mud/outgoing/track.merc21.tar.gz      *
 *  It cointained 5 files: README, hash.c, hash.h, skills.c, and skills.h  *
 *  I combined the *.c and *.h files in this hunt.c, which should compile  *
 *  without any warnings or errors.                                        *
 *                                                                         *
 *  Modified by Zen    for Envy 2.2 (14-Oct-97)                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



extern int	             top_vnum_room;

/*
 * Structure types.
 */
typedef	struct	hash_link		HASH_LINK;
typedef	struct	hash_header		HASH_HEADER;
typedef	struct	hunting_data		HUNTING_DATA;
typedef	struct	room_q			ROOM_Q;
typedef	struct	nodes			NODES;

/*
 * Hunting parameters.
 * Make sure area_last has the last room vnum!
 */
#define IS_DIR			( get_room_index( q_head->room_nr )->exit[i] )
#define GO_OK			( !IS_SET( IS_DIR->exit_info, EX_CLOSED ) )
#define GO_OK_SMARTER		1
#define WORLD_SIZE		top_vnum_room
#define	HASH_KEY( ht, key )	((((unsigned int)(key))*17)%(ht)->table_size)


struct hash_link
{
    int			 key;
    HASH_LINK	 	*next;
    void		*data;
};

struct hash_header
{
    int			 rec_size;
    int			 table_size;
    int			 klistsize;
    int			 klistlen; 
    int			*keylist;    /* this is really lame, AMAZINGLY lame */
    HASH_LINK	       **buckets;
};

struct hunting_data
{
    char		*name;
    CHAR_DATA	       **victim;
};

struct room_q
{
    int			 room_nr;
    ROOM_Q		*next_q;
};

struct nodes
{
    int visited;
    int ancestor;
};



void init_hash_table( HASH_HEADER *ht, int rec_size, int table_size )
{
    ht->rec_size	= rec_size;
    ht->table_size	= table_size;
    ht->buckets		= (void *) calloc( sizeof( HASH_LINK ** ), table_size );
    ht->keylist		= (void *) malloc( sizeof( ht->keylist ) * ( ht->klistsize = 128 ) );
    ht->klistlen	= 0;
}

void init_world( ROOM_INDEX_DATA *room_db[] )
{
    /* zero out the world */
    memset( room_db, 0, sizeof( ROOM_INDEX_DATA * ) * WORLD_SIZE );
    return;
}

void destroy_hash_table( HASH_HEADER *ht, void ( *gman ) ( ) )
{
    HASH_LINK *scan;
    HASH_LINK *temp;
    int        i;

    for ( i = 0; i < ht->table_size; i++ )
	for ( scan = ht->buckets[i]; scan; )
	{
	    temp = scan->next;
	    ( *gman ) ( scan->data );
	    free( scan );
	    scan = temp;
	}

    free( ht->buckets );
    free( ht->keylist );
    return;
}

void _hash_enter( HASH_HEADER *ht, int key, void *data )
{
    /* precondition: there is no entry for <key> yet */
    HASH_LINK *temp;
    int        i;

    temp	= (HASH_LINK *) malloc( sizeof( HASH_LINK ) );
    temp->key	= key;
    temp->next	= ht->buckets[HASH_KEY( ht, key )];
    temp->data	= data;
    ht->buckets[HASH_KEY( ht, key )] = temp;
    if ( ht->klistlen >= ht->klistsize )
    {
	ht->keylist = (void *) realloc( ht->keylist, sizeof( *ht->keylist ) *
					 ( ht->klistsize *= 2 ) );
    }
    for ( i = ht->klistlen; i >= 0; i-- )
    {
	if ( ht->keylist[i - 1] < key )
	{
	    ht->keylist[i] = key;
	    break;
	}
	ht->keylist[i] = ht->keylist[i - 1];
    }
    ht->klistlen++;
    return;
}

ROOM_INDEX_DATA *room_find( ROOM_INDEX_DATA *room_db[], int key )
{
    return ( ( key < WORLD_SIZE && key > -1 ) ? room_db[key] : 0 );
}

void *hash_find( HASH_HEADER *ht, int key )
{
    HASH_LINK *scan;

    scan = ht->buckets[HASH_KEY( ht, key )];

    while ( scan && scan->key != key )
	scan = scan->next;

    return scan ? scan->data : NULL;
}

int room_enter( ROOM_INDEX_DATA * rb[], int key, ROOM_INDEX_DATA *rm )
{
    ROOM_INDEX_DATA *temp;

    temp = room_find( rb, key );
    if ( temp )
	return ( 0 );

    rb[key] = rm;
    return ( 1 );
}

int hash_enter( HASH_HEADER *ht, int key, void *data )
{
    void *temp;

    temp = hash_find( ht, key );
    if ( temp )
	return 0;

    _hash_enter( ht, key, data );
    return 1;
}

ROOM_INDEX_DATA *room_find_or_create( ROOM_INDEX_DATA * rb[], int key )
{
    ROOM_INDEX_DATA *rv;

    rv = room_find( rb, key );
    if ( rv )
	return rv;

    rv = (ROOM_INDEX_DATA *) malloc( sizeof( ROOM_INDEX_DATA ) );
    rb[key] = rv;

    return rv;
}

void *hash_find_or_create( HASH_HEADER *ht, int key )
{
    void *rval;

    rval = hash_find( ht, key );
    if ( rval )
	return rval;

    rval = (void *) malloc( ht->rec_size );
    _hash_enter( ht, key, rval );

    return rval;
}

int room_remove( ROOM_INDEX_DATA * rb[], int key )
{
    ROOM_INDEX_DATA *tmp;

    tmp = room_find( rb, key );
    if ( tmp )
    {
	rb[key] = 0;
	free( tmp );
    }
    return ( 0 );
}

void *hash_remove( HASH_HEADER *ht, int key )
{
    HASH_LINK **scan;

    scan = ht->buckets + HASH_KEY( ht, key );

    while ( *scan && ( *scan )->key != key )
	scan = &( *scan )->next;

    if ( *scan )
    {
	HASH_LINK *temp;
	HASH_LINK *aux;
	int        i;

	temp = ( *scan )->data;
	aux = *scan;
	*scan = aux->next;
	free( aux );

	for ( i = 0; i < ht->klistlen; i++ )
	    if ( ht->keylist[i] == key )
		break;

	if ( i < ht->klistlen )
	{
	    memmove( ht->keylist + i, ht->keylist + i + 1, ( ht->klistlen - i )
		   * sizeof( *ht->keylist ) );
	    ht->klistlen--;
	}

	return temp;
    }

    return NULL;
}

void room_iterate( ROOM_INDEX_DATA *rb[], void ( *func ) ( ), void *cdata )
{
    register int i;

    for ( i = 0; i < WORLD_SIZE; i++ )
    {
	ROOM_INDEX_DATA *temp;

	temp = room_find( rb, i );
	if ( temp )
	    ( *func ) ( i, temp, cdata );
    }
}

void hash_iterate( HASH_HEADER *ht, void ( *func ) ( ), void *cdata )
{
    int i;

    for ( i = 0; i < ht->klistlen; i++ )
    {
	void          *temp;
	register int   key;

	key = ht->keylist[i];
	temp = hash_find( ht, key );
	( *func ) ( key, temp, cdata );
	if ( ht->keylist[i] != key )	/* They must have deleted this room */
	    i--;		/* Hit this slot again. */
    }
}

int exit_ok( EXIT_DATA *pexit )
{
    ROOM_INDEX_DATA *to_room;

    if ( ( !pexit )
	|| !( to_room = pexit->to_room ) )
	return 0;

    return 1;
}

void donothing( )
{
    return;
}

int find_path( int in_room_vnum, int out_room_vnum, CHAR_DATA *ch,
	      int depth, int in_zone )
{
    ROOM_INDEX_DATA *herep;
    ROOM_INDEX_DATA *startp;
    EXIT_DATA       *exitp;
    ROOM_Q          *tmp_q;
    ROOM_Q          *q_head;
    ROOM_Q          *q_tail;
    HASH_HEADER      x_room;
    bool             thru_doors;
    long             i;
    int              tmp_room;
    int              count = 0;

    if ( depth < 0 )
    {
	thru_doors = TRUE;
	depth = -depth;
    }
    else
    {
	thru_doors = FALSE;
    }

    startp = get_room_index( in_room_vnum );

    init_hash_table( &x_room, sizeof( int ), 2048 );
    hash_enter( &x_room, in_room_vnum, (void *) - 1 );

    /* initialize queue */
    q_head = (ROOM_Q *) malloc( sizeof( ROOM_Q ) );
    q_tail = q_head;
    q_tail->room_nr = in_room_vnum;
    q_tail->next_q = 0;

    while ( q_head )
    {
	herep = get_room_index( q_head->room_nr );
	/* for each room test all directions */
	if ( herep->area == startp->area || !in_zone )
	{
	    /*
	     * only look in this zone...
	     * saves cpu time and makes world safer for players
	     */
	    for ( i = 0; i < MAX_DIR; i++ )
	    {
		exitp = herep->exit[i];
		if ( exit_ok( exitp ) && ( thru_doors ? GO_OK_SMARTER : GO_OK ) )
		{
		    /* next room */
		    tmp_room = herep->exit[i]->to_room->vnum;
		    if ( tmp_room != out_room_vnum )
		    {
			/* 
			 * shall we add room to queue ?
			 * count determines total breadth and depth
			 */
			if ( !hash_find( &x_room, tmp_room )
			    && ( count < depth ) )
			    /* && !IS_SET( RM_FLAGS(tmp_room), DEATH ) ) */
			{
			    count++;
			    /* mark room as visted and put on queue */

			    tmp_q = (ROOM_Q *)
				malloc( sizeof( ROOM_Q ) );
			    tmp_q->room_nr = tmp_room;
			    tmp_q->next_q = 0;
			    q_tail->next_q = tmp_q;
			    q_tail = tmp_q;

			    /* ancestor for first layer is the direction */
			    hash_enter( &x_room, tmp_room,
			    ( (long) hash_find( &x_room, q_head->room_nr ) == -1 )
				? (void *) ( i + 1 )
				: hash_find( &x_room, q_head->room_nr ) );
			}
		    }
		    else
		    {
			/* have reached our goal so free queue */
			tmp_room = q_head->room_nr;
			for ( ; q_head; q_head = tmp_q )
			{
			    tmp_q = q_head->next_q;
			    free( q_head );
			}
			/* return direction if first layer */
			if ( (long) hash_find( &x_room, tmp_room ) == -1 )
			{
			    if ( x_room.buckets )
			    {
				/* junk left over from a previous track */
				destroy_hash_table( &x_room, donothing );
			    }
			    return ( i );
			}
			else
			{
			    /* else return the ancestor */
			    long i;

			    i = (long) hash_find( &x_room, tmp_room );
			    if ( x_room.buckets )
			    {
				/* junk left over from a previous track */
				destroy_hash_table( &x_room, donothing );
			    }
			    return ( -1 + i );
			}
		    }
		}
	    }
	}

	/* free queue head and point to next entry */
	tmp_q = q_head->next_q;
	free( q_head );
	q_head = tmp_q;
    }

    /* couldn't find path */
    if ( x_room.buckets )
    {
	/* junk left over from a previous track */
	destroy_hash_table( &x_room, donothing );
    }
    return -1;
}



void found_prey( CHAR_DATA *ch, CHAR_DATA *victim )
{
     char buf		[ MAX_STRING_LENGTH ];
     char victname	[ MAX_STRING_LENGTH ];

     if ( !victim || victim->deleted )
     {
	bug( "Found_prey: null victim", 0 );
	return;
     }

     if ( !victim->in_room )
     {
        bug( "Found_prey: null victim->in_room", 0 );
        return;
     }

     sprintf( victname, IS_NPC( victim ) ? victim->short_descr : victim->name );

     if ( !can_see( ch, victim ) )
     {
        if ( number_percent( ) < 90 )
	  return;
	switch( number_bits( 2 ) )
 	{
	case 0: sprintf( buf, "Don't make me find you, %s!", victname );
		do_say( ch, buf );
	        break;
	case 1: act( "$n sniffs around the room for $N.", ch, NULL, victim, TO_NOTVICT );
		act( "You sniff around the room for $N.", ch, NULL, victim, TO_CHAR );
		act( "$n sniffs around the room for you.", ch, NULL, victim, TO_VICT );
		sprintf( buf, "I can smell your blood!" );
		do_say( ch, buf );
		break;
	case 2: sprintf( buf, "I'm going to tear %s apart!", victname );
		do_yell( ch, buf );
		break;
	case 3: do_say( ch, "Just wait until I find you...");
		break;
        }
	return;
     }

     if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
     {
	if ( number_percent( ) < 90 )
	  return;
	switch( number_bits( 2 ) )
	{
	case 0:	do_say( ch, "C'mon out, you coward!" );
		sprintf( buf, "%s is a bloody coward!", victname );
		do_yell( ch, buf );
		break;
	case 1: sprintf( buf, "Let's take this outside, %s", victname );
		do_say( ch, buf );
		break;
	case 2: sprintf( buf, "%s is a yellow-bellied wimp!", victname );
		do_yell( ch, buf );
		break;
	case 3: act( "$n takes a few swipes at $N.", ch, NULL, victim, TO_NOTVICT );
		act( "You try to take a few swipes $N.", ch, NULL, victim, TO_CHAR );
		act( "$n takes a few swipes at you.", ch, NULL, victim, TO_VICT );
		break;
	}
	return;
     }

     switch( number_bits( 2 ) )
     {
     case 0: sprintf( buf, "Your blood is mine, %s!", victname );
	     do_yell( ch, buf);
	     break;
     case 1: sprintf( buf, "Alas, we meet again, %s!", victname );
     	     do_say( ch, buf );
     	     break;
     case 2: sprintf( buf, "What do you want on your tombstone, %s?", victname );
     	     do_say( ch, buf );
     	     break;
     case 3: act( "$n lunges at $N from out of nowhere!", ch, NULL, victim, TO_NOTVICT );
	     act( "You lunge at $N catching $M off guard!", ch, NULL, victim, TO_CHAR );
	     act( "$n lunges at you from out of nowhere!", ch, NULL, victim, TO_VICT );
     }
     stop_hunting( ch );
     multi_hit( ch, victim, TYPE_UNDEFINED );
     return;
} 



void do_track( CHAR_DATA *ch, char *argument )
{
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_STRING_LENGTH ];
    CHAR_DATA *victim;
    int        direction;
    bool       fArea;

    if ( !IS_NPC( ch ) && !ch->pcdata->learned[gsn_track] )
    {
	send_to_char( "You do not know of this skill yet.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Whom are you trying to track?\n\r", ch );
	return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't sniff a trail mounted.\n\r", ch );
        return;
    }

    /* only imps can hunt to different areas */
    fArea = ( get_trust( ch ) < L_DIR );

    if ( fArea )
	victim = get_char_area( ch, arg );
    else
	victim = get_char_world( ch, arg );

    if ( !victim )
    {
	send_to_char( "You can't find a trail of anyone like that.\n\r", ch );
	return;
    }

    if ( ch->in_room == victim->in_room )
    {
	act( "You're already in $N's room!", ch, NULL, victim, TO_CHAR );
	return;
    }

    /*
     * Deduct some movement.
     */
    if ( ch->move > 2 )
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

    if ( direction == -1 )
    {
	act( "You can't sense $N's trail from here.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( direction < 0 || direction >= MAX_DIR )
    {
	send_to_char( "Hmm... Something seems to be wrong.\n\r", ch );
	return;
    }

    /*
     * Give a random direction if the player misses the die roll.
     */
    if (   (  IS_NPC( ch ) && number_percent( ) > 75 )	/* NPC @ 25% */
	|| ( !IS_NPC( ch ) && number_percent( ) >	/* PC @ norm */
		ch->pcdata->learned[gsn_track] ) )
    {
	do
	{
	    direction = number_door( );
	}
	while (   !( ch->in_room->exit[direction] )
	       || !( ch->in_room->exit[direction]->to_room ) );
    }

    /*
     * Display the results of the search.
     */
    sprintf( buf, "You sense $N's trail %s from here...",
	    dir_name[direction] );
    act( buf, ch, NULL, victim, TO_CHAR );

    return;
}



void hunt_victim( CHAR_DATA *ch )
{
    CHAR_DATA *tmp;
    int        dir;
    bool       found;

    if ( !ch || ch->deleted || !ch->hunting )
	return;

    /*
     * Make sure the victim still exists.
     */
    for ( found = FALSE, tmp = char_list; tmp && !found; tmp = tmp->next )
	if ( ch->hunting->who == tmp )
	    found = TRUE;

    if ( !found || !can_see( ch, ch->hunting->who ) )
    {
	do_say( ch, "Damn!  My prey is gone!!" );
	stop_hunting( ch );
	return;
    }

    if ( ch->in_room == ch->hunting->who->in_room )
    {
	if ( ch->fighting )
	    return;
	found_prey( ch, ch->hunting->who );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_track].beats );
    dir = find_path( ch->in_room->vnum, ch->hunting->who->in_room->vnum,
		    ch, -40000, TRUE );

    if ( dir < 0 || dir >= MAX_DIR )
    {
	act( "$n says 'Damn!  Lost $M!'", ch, NULL, ch->hunting->who, TO_ROOM );
	stop_hunting( ch );
	return;
    }

    /*
     * Give a random direction if the mob misses the die roll.
     */
    if ( number_percent( ) > 75 )	/* @ 25% */
    {
	do
	{
	    dir = number_door( );
	}
	while (   !( ch->in_room->exit[dir] )
	       || !( ch->in_room->exit[dir]->to_room ) );
    }


    if ( IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED ) )
    {
	do_open( ch, (char *) dir_name[dir] );
	return;
    }

    move_char( ch, dir );

    if ( !ch->hunting )
    {
	if ( !ch->in_room )
	{
	    char buf [ MAX_STRING_LENGTH ];

	    sprintf( buf, "Hunt_victim: no ch->in_room!  Mob #%d, name: %s.  Placing mob in limbo.",
		    ch->pIndexData->vnum, ch->name );
	    bug( buf, 0 );
	    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
	    return;
	} 
	do_say( ch, "Damn!  Lost my prey!" );
	return;
    }
    if ( ch->in_room == ch->hunting->who->in_room )
	found_prey( ch, ch->hunting->who );

    return;
}
