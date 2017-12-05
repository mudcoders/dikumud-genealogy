/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *  Mitchell Tse.                                                          *
 *                                                                         *
 *  EnvyMud 2.2 improvements copyright (C) 1996, 1997 by Michael Quan.     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"



char *	const	dir_name	[ ]		=
{
    "north", "east", "south", "west", "up", "down"
};

char *  const   dir_rev		[ ]		=
{
    "the south", "the west", "the north", "the east", "below", "above"
};

const	int	rev_dir		[ ]		=
{
    2, 3, 0, 1, 5, 4
};

const	int	movement_loss	[ SECT_MAX ]	=
{
    1, 2, 2, 3, 4, 6, 4, 1, 5, 10, 6, 10
};



/*
 * Local functions.
 */
#define OD      OBJ_DATA

int	find_door	args( ( CHAR_DATA *ch, char *arg ) );
OD*	has_key		args( ( CHAR_DATA *ch, int key ) );

#undef OD

void move_char( CHAR_DATA *ch, int door )
{
    CHAR_DATA       *fch;
    CHAR_DATA       *fch_next;
    EXIT_DATA       *pexit;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    int              moved = BV17; /* Matches ACT & PLR bits */

    if ( door < 0 || door > 5 )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    /*
     * Prevents infinite move loop in
     * maze zone when group has 2 leaders - Kahn
     */
    if ( IS_SET( ch->act, moved ) )
        return;

    if ( IS_AFFECTED( ch, AFF_HOLD ) ) 
    {
	send_to_char( "You are stuck in a snare!  You can't move!\n\r", ch );
	return;
    }

    if ( ch->riding && IS_AFFECTED( ch->riding, AFF_HOLD ) )
    {
        send_to_char( "Your ride can't move.\n\r", ch );
        return;
    }

    in_room = ch->in_room;
    if ( !( pexit = in_room->exit[door] ) || !( to_room = pexit->to_room ) )
    {
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return;
    }

    if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
    {
	if ( ch->riding )
	{
	    if (   !IS_AFFECTED( ch->riding, AFF_PASS_DOOR )
		&& !IS_SET( race_table[ ch->riding->race ].race_abilities, RACE_PASSDOOR )
		&& !IS_IMMORTAL ( ch->riding ) )
	    {
		act( "The $d is closed so your mount is unable to pass.",
		    ch, NULL, pexit->keyword, TO_CHAR );
		return;
	    }

	    if ( IS_SET( pexit->exit_info, EX_PASSPROOF )
		&& !IS_IMMORTAL( ch->riding ) )
	    {
		act( "Your mount is unable to pass through the $d.  Ouch!",
		    ch, NULL, pexit->keyword, TO_CHAR );
		return;
	    }
	}
	else
	{
	    if (   !IS_AFFECTED( ch, AFF_PASS_DOOR )
		&& !IS_SET( race_table[ ch->race ].race_abilities, RACE_PASSDOOR )
		&& !IS_IMMORTAL ( ch ) )
	    {
		act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
		return;
	    }
	
	    if ( IS_SET( pexit->exit_info, EX_PASSPROOF )
		&& !IS_IMMORTAL( ch ) )
	    {
		act( "You are unable to pass through the $d.  Ouch!",
		    ch, NULL, pexit->keyword, TO_CHAR );
		return;
	    }
	}
    }

    if ( IS_AFFECTED( ch, AFF_CHARM )
	&& ch->master
	&& in_room == ch->master->in_room )
    {
	send_to_char( "What?  And leave your beloved master?\n\r", ch );
	return;
    }

    if ( ch->riding && ch->riding->position < POS_STANDING )
    {
        send_to_char( "Your ride doesn't want to move right now.\n\r", ch );
        return;
    }

    if ( room_is_private( to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    if ( ch->riding )
    {
	if (   to_room->sector_type != SECT_WATER_SWIM
	    && to_room->sector_type != SECT_WATER_NOSWIM
	    && to_room->sector_type != SECT_UNDERWATER
	    && str_cmp( race_table[ ch->riding->race ].name, "God"  )
	    && str_cmp( race_table[ ch->riding->race ].name, "Bear" )
	    && IS_SET(  race_table[ ch->riding->race ].race_abilities, RACE_SWIM ) )
	{
	    send_to_char( "Your mount flaps around but can't move!\n\r", ch );
	    return;
        }
    }
    else
    {
	if (   to_room->sector_type != SECT_WATER_SWIM
	    && to_room->sector_type != SECT_WATER_NOSWIM
	    && to_room->sector_type != SECT_UNDERWATER
	    && str_cmp( race_table[ ch->race ].name, "God"  )
	    && str_cmp( race_table[ ch->race ].name, "Bear" )
	    && IS_SET(  race_table[ ch->race ].race_abilities, RACE_SWIM ) )
	{
	    send_to_char( "You flap around but you can't move!\n\r", ch );
	    return;
	}
    }

    if ( !IS_NPC( ch ) )
    {
	int iClass;
	int move;

	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    if ( iClass != ch->class
		&& to_room->vnum == class_table[iClass]->guild
		&& ch->level < LEVEL_IMMORTAL )
	    {
		send_to_char( "You aren't allowed in there.\n\r", ch );
		return;
	    }
	}

	if (   in_room->sector_type == SECT_AIR
	    || to_room->sector_type == SECT_AIR )
	{
	    if ( ch->riding )
	    {
		if ( !IS_AFFECTED( ch->riding, AFF_FLYING )
		    && !IS_SET( race_table[ ch->riding->race ].race_abilities, RACE_FLY ) )
		{
		    send_to_char( "Your mount can't fly.\n\r", ch );
		    return;
		}
	    }
	    else
	    {
		if ( !IS_AFFECTED( ch, AFF_FLYING )
		    && !IS_SET( race_table[ ch->race ].race_abilities, RACE_FLY ) )
		{
		    send_to_char( "You can't fly.\n\r", ch );
		    return;
		}
	    }
	}

	if (   in_room->sector_type == SECT_WATER_NOSWIM
	    || to_room->sector_type == SECT_WATER_NOSWIM )
	{
	    OBJ_DATA *obj;
	    bool      found;

	    /*
	     * Look for a boat.
	     */
	    found = FALSE;

	    /*
	     * Suggestion for flying above water by Sludge
	     */
	    if ( ch->riding
		&& ( IS_AFFECTED( ch->riding, AFF_FLYING )
		    || IS_SET( race_table[ ch->riding->race ].race_abilities, RACE_FLY )
		    || IS_SET( race_table[ ch->riding->race ].race_abilities, RACE_WATERWALK )
		    || IS_SET( race_table[ ch->riding->race ].race_abilities, RACE_SWIM )
		    || ( !IS_NPC( ch->riding )
			&& number_percent( ) <= ch->pcdata->learned[gsn_swim] ) ) )
	        found = TRUE;

	    if ( IS_AFFECTED( ch, AFF_FLYING )
		|| IS_SET( race_table[ ch->race ].race_abilities, RACE_FLY )
		|| IS_SET( race_table[ ch->race ].race_abilities, RACE_WATERWALK )
		|| IS_SET( race_table[ ch->race ].race_abilities, RACE_SWIM )
		|| number_percent( ) <= ch->pcdata->learned[gsn_swim] )
	        found = TRUE;

	    for ( obj = ch->carrying; obj; obj = obj->next_content )
	    {
		if ( obj->item_type == ITEM_BOAT )
		{
		    found = TRUE;
		    break;
		}
	    }
	    if ( !found )
	    {
		send_to_char( "You need a boat to go there.\n\r", ch );
		return;
	    }
	}

	if ( ch->riding )
	{
	    if ( ( in_room->sector_type == SECT_UNDERWATER
		|| to_room->sector_type == SECT_UNDERWATER )
		&& !IS_SET( race_table[ ch->riding->race ].race_abilities, RACE_SWIM )
		&& ( !IS_NPC( ch->riding )
		    && number_percent( ) > ch->pcdata->learned[gsn_swim] ) )
	    {
		send_to_char( "Your mount needs to be able to swim better to go there.\n\r", ch );
		return;
	    }
	}
	else
	{
	    if ( ( in_room->sector_type == SECT_UNDERWATER
		|| to_room->sector_type == SECT_UNDERWATER )
		&& !IS_SET( race_table[ ch->race ].race_abilities, RACE_SWIM )
		&& number_percent( ) > ch->pcdata->learned[gsn_swim] )
	    {
		send_to_char( "You need to be able to swim better to go there.\n\r", ch );
		return;
	    }
	}

	move = movement_loss[UMIN( SECT_MAX-1, in_room->sector_type )]
	     + movement_loss[UMIN( SECT_MAX-1, to_room->sector_type )]
	     ;

	if ( ch->riding )
	    move /= ( ch->riding->level / 10 ) + 1;

	/* Flying persons lose constant minimum movement. */
	if (   IS_SET( race_table[ ch->race ].race_abilities, RACE_FLY )
	    || IS_AFFECTED( ch, AFF_FLYING ) )
	    move = 2;

	if ( ch->move < move )
	{
	    send_to_char( "You are too exhausted.\n\r", ch );
	    return;
	}

	WAIT_STATE( ch, 1 );
	ch->move -= move;
    }

    if ( !IS_AFFECTED( ch, AFF_SNEAK )
	&& !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
        if ( ( (     in_room->sector_type == SECT_WATER_SWIM )
		|| ( in_room->sector_type == SECT_UNDERWATER ) )
	    && (   ( to_room->sector_type == SECT_WATER_SWIM )
		|| ( to_room->sector_type == SECT_UNDERWATER ) ) )
	{
	    act( "$n swims $T.",  ch, NULL, dir_name[door], TO_ROOM );
	}
	else
	{
	    if ( ch->riding )
		act( "$n rides $N $t.", ch, dir_name[door], ch->riding, TO_ROOM );
	    else
	    {
		if ( !ch->rider )
		    act( "$n leaves $T.", ch, NULL, dir_name[door], TO_ROOM );
	    }
	}
    }

    char_from_room( ch );
    char_to_room( ch, to_room );

    if ( !IS_AFFECTED( ch, AFF_SNEAK )
	&& ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) )
    {
	     if ( ch->riding )
	    act( "$n has arrived mounted on $N.", ch, NULL, ch->riding, TO_ROOM );
	else if ( !ch->rider )
	    act( "$n has arrived.", ch, NULL, NULL, TO_ROOM );
    }

    /* Because of the addition of the deleted flag, we can do this -Kahn */
    if ( !IS_IMMORTAL( ch ) && !str_cmp( race_table[ch->race].name, "Vampire" )
	&& to_room->sector_type == SECT_UNDERWATER )
    {
	send_to_char( "Arrgh!  Large body of water!\n\r", ch );
	act( "$n thrashes underwater!", ch, NULL, NULL, TO_ROOM );
	damage( ch, ch, 20, TYPE_UNDEFINED, WEAR_NONE, DAM_DROWNING );
    }
    else if ( !IS_IMMORTAL( ch )
	     && ( to_room->sector_type == SECT_UNDERWATER
		 && !IS_AFFECTED( ch, AFF_GILLS )
		 && !IS_SET( race_table[ ch->race ].race_abilities,
			    RACE_WATERBREATH ) ) )
    {
	send_to_char( "You can't breathe!\n\r", ch );
	act( "$n sputters and chokes!", ch, NULL, NULL, TO_ROOM );
	damage( ch, ch, 2, TYPE_UNDEFINED, WEAR_NONE, DAM_DROWNING );
    }

    /*
     * Suggested by D'Sai from A Moment in Tyme Mud.  Why have mobiles
     * see the room?  -Kahn
     */
    if ( ch->desc )
    {
        if ( !IS_NPC( ch )
	    && IS_SET( ch->act, PLR_EDIT_INFO )
	    && pexit->exit_info )
	{
	    char  buf  [ MAX_STRING_LENGTH ];
	    char  buf1 [ MAX_STRING_LENGTH ];

	    buf1[0] = '\0';
	    sprintf( buf, "{w* You just passed trough an exit of type %s.{x\n\r",
		    flag_string( exit_flags, pexit->exit_info ) );
	    strcat( buf1, buf );
	    sprintf( buf, "{w* It had been reset to [%d] %s.{x\n\r",
		    pexit->rs_flags,
		    flag_string( exit_flags, pexit->rs_flags ) );
	    strcat( buf1, buf );
	    send_to_char( buf1, ch );
	}

	do_look( ch, "auto" );
    }

    SET_BIT( ch->act, moved );

    for ( fch = in_room->people; fch; fch = fch_next )
    {
        fch_next = fch->next_in_room;

        if ( fch->deleted )
	    continue;
      
	if ( fch->master == ch && fch->position == POS_STANDING )
	{
	    act( "You follow $N.", fch, NULL, ch, TO_CHAR );
	    move_char( fch, door );
	}
    }

    REMOVE_BIT( ch->act, moved );
    mprog_entry_trigger( ch );
    mprog_greet_trigger( ch );
    return;
}



void do_north( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NORTH );
    return;
}



void do_east( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_EAST );
    return;
}



void do_south( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SOUTH );
    return;
}



void do_west( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_WEST );
    return;
}



void do_up( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_UP );
    return;
}



void do_down( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_DOWN );
    return;
}



int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int        door;

	 if ( !str_prefix( arg, "north" ) ) door = 0;
    else if ( !str_prefix( arg, "east"  ) ) door = 1;
    else if ( !str_prefix( arg, "south" ) ) door = 2;
    else if ( !str_prefix( arg, "west"  ) ) door = 3;
    else if ( !str_prefix( arg, "up"    ) ) door = 4;
    else if ( !str_prefix( arg, "down"  ) ) door = 5;
    else
    {
	for ( door = 0; door < MAX_DIR; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] )
		&& IS_SET( pexit->exit_info, EX_ISDOOR )
		&& pexit->keyword
		&& is_name( arg, pexit->keyword ) )
		return door;
	}
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !( pexit = ch->in_room->exit[door] ) )
    {
	act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !IS_SET( pexit->exit_info, EX_ISDOOR ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return -1;
    }

    return door;
}



void do_open( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'open door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED )  )
	    { send_to_char( "It's already open.\n\r",     ch ); return; }
	if (  IS_SET( pexit->exit_info, EX_LOCKED )  )
	    { send_to_char( "It's locked.\n\r",           ch ); return; }

	REMOVE_BIT( pexit->exit_info, EX_CLOSED );
	act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* open the other side */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch; rch = rch->next_in_room )
	    {
		if ( rch->deleted )
		    continue;
		act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	    }
	}

	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'open portal' */
	if ( obj->item_type == ITEM_PORTAL )
	{
	    if ( !IS_SET( obj->value[1], PORTAL_CLOSEABLE ) )
		{ send_to_char( "You can't do that.\n\r",      ch ); return; }
	    if ( !IS_SET( obj->value[1], PORTAL_CLOSED )    )
		{ send_to_char( "It's already open.\n\r",      ch ); return; }
	    if (  IS_SET( obj->value[1], PORTAL_LOCKED )    )
		{ send_to_char( "It's locked.\n\r",            ch ); return; }

	    REMOVE_BIT( obj->value[1], PORTAL_CLOSED );
	    send_to_char( "Ok.\n\r", ch );
	    act( "$n opens $p.", ch, obj, NULL, TO_ROOM );
	    return;
        }

	/* 'open object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED )    )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSEABLE ) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if (  IS_SET( obj->value[1], CONT_LOCKED )    )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT( obj->value[1], CONT_CLOSED );
	send_to_char( "Ok.\n\r", ch );
	act( "$n opens $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'close door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit	= ch->in_room->exit[door];
	if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char( "It's already closed.\n\r",    ch );
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_BASHED ) )
	{
	    act( "The $d has been bashed open and cannot be closed.",
		ch, NULL, pexit->keyword, TO_CHAR );
	    return;
	}

	SET_BIT( pexit->exit_info, EX_CLOSED );
	act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* close the other side */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch; rch = rch->next_in_room )
	    {
		if ( rch->deleted )
		    continue;
		act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	    }
	}

	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'close portal' */
	if ( obj->item_type == ITEM_PORTAL )
	{
	    if (  IS_SET( obj->value[1], PORTAL_CLOSED )    )
		{ send_to_char( "It's already closed.\n\r",    ch ); return; }
	    if ( !IS_SET( obj->value[1], PORTAL_CLOSEABLE ) )
		{ send_to_char( "You can't do that.\n\r",      ch ); return; }

	    SET_BIT( obj->value[1], PORTAL_CLOSED );
	    send_to_char( "Ok.\n\r", ch );
	    act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
	    return;
	}

	/* 'close object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if (  IS_SET( obj->value[1], CONT_CLOSED )    )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSEABLE ) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }

	SET_BIT( obj->value[1], CONT_CLOSED );
	send_to_char( "Ok.\n\r", ch );
	act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    return;
}



OBJ_DATA* has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == key )
	    return obj;
    }

    return NULL;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'lock door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit	= ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, pexit->key ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if (  IS_SET( pexit->exit_info, EX_LOCKED ) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* lock the other side */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}

	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'lock portal' */
	if ( obj->item_type == ITEM_PORTAL )
	{
	    if ( !IS_SET( obj->value[1], PORTAL_CLOSED ) )
		{ send_to_char( "It's not closed.\n\r",        ch ); return; }
	    if ( obj->value[2] < 0 )
		{ send_to_char( "It can't be locked.\n\r",     ch ); return; }
	    if ( !has_key( ch, obj->value[2] ) )
		{ send_to_char( "You lack the key.\n\r",       ch ); return; }
	    if (  IS_SET( obj->value[1], PORTAL_LOCKED ) )
		{ send_to_char( "It's already locked.\n\r",    ch ); return; }

	    SET_BIT( obj->value[1], PORTAL_LOCKED );
	    send_to_char( "*Click*\n\r", ch );
	    act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
	    return;
	}

	/* 'lock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED ) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if (  IS_SET( obj->value[1], CONT_LOCKED ) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT( obj->value[1], CONT_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'unlock door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, pexit->key ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET( pexit->exit_info, EX_LOCKED ) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	if ( IS_SET( pexit->exit_info, EX_EAT_KEY ) )
	{
	    extract_obj( has_key( ch, pexit->key ) );
	    act( "The $d eats the key!", ch, NULL, pexit->keyword, TO_CHAR );
	    act( "The $d eats the key!", ch, NULL, pexit->keyword, TO_ROOM );
	}

	/* unlock the other side */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}

	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'unlock portal' */
	if ( obj->item_type == ITEM_PORTAL )
	{
	    if ( !IS_SET( obj->value[1], PORTAL_CLOSED ) )
		{ send_to_char( "It's not closed.\n\r",        ch ); return; }
	    if ( obj->value[2] < 0 )
		{ send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	    if ( !has_key( ch, obj->value[2] ) )
		{ send_to_char( "You lack the key.\n\r",       ch ); return; }
	    if ( !IS_SET( obj->value[1], PORTAL_LOCKED ) )
		{ send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	    REMOVE_BIT( obj->value[1], PORTAL_LOCKED );
	    send_to_char( "*Click*\n\r", ch );
	    act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	    return;
	}

	/* 'unlock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED ) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_LOCKED ) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT( obj->value[1], CONT_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    return;
}



void do_pick( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *gch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't do that while mounted.\n\r", ch );
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

    /* look for guards */
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if ( gch->deleted )
	    continue;
	if ( IS_NPC( gch ) && IS_AWAKE( gch ) && ch->level + 5 < gch->level )
	{
	    act( "$N is standing too close to the lock.",
		ch, NULL, gch, TO_CHAR );
	    return;
	}
    }

    /* Check skill roll for player-char, make sure mob isn't charmed */
    if ( (  !IS_NPC( ch )
	  && number_percent( ) > ch->pcdata->learned[gsn_pick_lock] )
	|| ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) ) )
    {
	send_to_char( "You failed.\n\r", ch);
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'pick door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED )    )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET( pexit->exit_info, EX_LOCKED )    )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if (  IS_SET( pexit->exit_info, EX_PICKPROOF ) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* pick the other side */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}

	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'pick portal' */
	if ( obj->item_type == ITEM_PORTAL )
	{
	    if ( !IS_SET( obj->value[1], PORTAL_CLOSED )    )
		{ send_to_char( "It's not closed.\n\r",        ch ); return; }
	    if ( obj->value[2] < 0 )
		{ send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	    if ( !IS_SET( obj->value[1], PORTAL_LOCKED )    )
		{ send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	    if (  IS_SET( obj->value[1], PORTAL_PICKPROOF ) )
		{ send_to_char( "You failed.\n\r",             ch ); return; }

	    REMOVE_BIT( obj->value[1], PORTAL_LOCKED );
	    send_to_char( "*Click*\n\r", ch );
	    act( "$n picks $p.", ch, obj, NULL, TO_ROOM );
	    return;
	}

	/* 'pick object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED )    )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_LOCKED )    )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if (  IS_SET( obj->value[1], CONT_PICKPROOF ) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT( obj->value[1], CONT_LOCKED );
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    return;
}




void do_stand( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED( ch, AFF_SLEEP ) )
	    { send_to_char( "You can't wake up!\n\r", ch ); return; }

	send_to_char( "You wake and stand up.\n\r", ch );
	act( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_RESTING:
	send_to_char( "You stand up.\n\r", ch );
	act( "$n stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r",  ch );
	break;

    case POS_STANDING:
	send_to_char( "You are already standing.\n\r",  ch );
	break;
    }

    return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r",  ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r",   ch );
	break;

    case POS_FIGHTING:
	send_to_char( "Not while you're fighting!\n\r", ch );
	break;

    case POS_STANDING:
	send_to_char( "You rest.\n\r", ch );
	act( "$n rests.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_RESTING;
	break;
    }

    return;
}



void do_sleep( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r",  ch );
	break;

    case POS_RESTING:
    case POS_STANDING: 
	send_to_char( "You sleep.\n\r", ch );
	act( "$n sleeps.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SLEEPING;
	break;

    case POS_FIGHTING:
	send_to_char( "Not while you're fighting!\n\r", ch );
	break;
    }

    return;
}



void do_wake( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	{ do_stand( ch, argument ); return; }

    if ( !IS_AWAKE( ch ) )
	{ send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( !( victim = get_char_room( ch, arg ) ) )
	{ send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE( victim ) )
	{ act( "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( IS_AFFECTED( victim, AFF_SLEEP ) )
	{ act( "You can't wake $M!",   ch, NULL, victim, TO_CHAR ); return; }

    victim->position = POS_STANDING;
    act( "You wake $M.",  ch, NULL, victim, TO_CHAR );
    act( "$n wakes you.", ch, NULL, victim, TO_VICT );
    return;
}



void do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    /* Don't allow charmed mobs to do this, check player's skill */
    if ( ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	|| ( !IS_NPC( ch )
	    && ch->level < skill_table[gsn_sneak].skill_level[ch->class] ) )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't do that while mounted.\n\r", ch );
        return;
    }

    send_to_char( "You attempt to move silently.\n\r", ch );
    affect_strip( ch, gsn_sneak );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_sneak] )
    {
	af.type      = gsn_sneak;
	af.duration  = ch->level;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );
    }

    return;
}



void do_hide( CHAR_DATA *ch, char *argument )
{
    /* Dont allow charmed mobiles to do this, check player's skill */
    if ( ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	|| ( !IS_NPC( ch )
	    && ch->level < skill_table[gsn_hide].skill_level[ch->class] ) )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't do that while mounted.\n\r", ch );
        return;
    }

    send_to_char( "You attempt to hide.\n\r", ch );

    if ( IS_AFFECTED( ch, AFF_HIDE ) )
	REMOVE_BIT( ch->affected_by, AFF_HIDE );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_hide] )
	SET_BIT( ch->affected_by, AFF_HIDE );

    return;
}



/*
 * Contributed by Alander.
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
    affect_strip ( ch, gsn_invis			);
    affect_strip ( ch, gsn_mass_invis			);
    affect_strip ( ch, gsn_sneak			);
    affect_strip ( ch, gsn_shadow                       );
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
    REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE	);
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_recall( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    ROOM_INDEX_DATA *location;
    char             buf [ MAX_STRING_LENGTH ];
    int              place;

    act( "$n prays for transportation!", ch, NULL, NULL, TO_ROOM );

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
	|| IS_AFFECTED( ch, AFF_CURSE ) )
    {
	send_to_char( "God has forsaken you.\n\r", ch );
	return;
    }

    place = ch->in_room->area->recall;
    if ( !( location = get_room_index( place ) ) )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    if ( ( victim = ch->fighting ) )
    {
	int lose;

	if ( number_bits( 1 ) == 0 )
	{
	    WAIT_STATE( ch, 4 );
	    lose = ( ch->desc ) ? 25 : 50;
	    gain_exp( ch, 0 - lose );
	    sprintf( buf, "You failed!  You lose %d exps.\n\r", lose );
	    send_to_char( buf, ch );
	    return;
	}

	lose = ( ch->desc ) ? 50 : 100;
	gain_exp( ch, 0 - lose );
	sprintf( buf, "You recall from combat!  You lose %d exps.\n\r", lose );
	send_to_char( buf, ch );
	stop_fighting( ch, TRUE );
    }

    ch->move /= 2;
    act( "$n disappears.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );

    return;
}



void do_train( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mob;
    char      *pOutput;
    char       buf [ MAX_STRING_LENGTH ];
    bool       ok = FALSE;
    int       *pAbility;
    int        cost;
    int        money;
    int        bone_flag = 0; /*Added for training of hp ma mv */

    if ( IS_NPC( ch ) )
	return;

    /*
     * Check for trainer.
     */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
	if ( IS_NPC( mob ) && IS_SET( mob->act, ACT_TRAIN ) )
	    break;
    }

    if ( !mob )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	sprintf( buf, "{o{cYou have %d practice sessions.{x\n\r", ch->practice );
	send_to_char( buf, ch );
	argument = "foo";
    }

    cost  = 5;
    money = ch->level * ch->level * 100;

    if ( !str_cmp( argument, "str" ) )
    {
	if ( class_table[ch->class]->attr_prime == APPLY_STR )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_str;
	pOutput     = "strength";
    }

    else if ( !str_cmp( argument, "int" ) )
    {
	if ( class_table[ch->class]->attr_prime == APPLY_INT )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_int;
	pOutput     = "intelligence";
    }

    else if ( !str_cmp( argument, "wis" ) )
    {
	if ( class_table[ch->class]->attr_prime == APPLY_WIS )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_wis;
	pOutput     = "wisdom";
    }

    else if ( !str_cmp( argument, "dex" ) )
    {
	if ( class_table[ch->class]->attr_prime == APPLY_DEX )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_dex;
	pOutput     = "dexterity";
    }

    else if ( !str_cmp( argument, "con" ) )
    {
	if ( class_table[ch->class]->attr_prime == APPLY_CON )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_con;
	pOutput     = "constitution";
    }

    /* ---------------- By Bonecrusher ------------------- */

    else if ( !str_cmp( argument, "hp" ) )
    {
 	    cost    = 1;
	    money   = ch->level * ch->level * 20;
	bone_flag   = 1;
        pAbility    = &ch->max_hit;
        pOutput     = "hit points";
    }
	    
    else if ( !str_cmp( argument, "mana" ) )
    {
 	    cost    = 1;
	    money   = ch->level * ch->level * 20;
	bone_flag   = 1;
        pAbility    = &ch->max_mana;
        pOutput     = "mana points";
    }

    else if ( !str_cmp( argument, "move" ) )
    {
 	    cost    = 1;
	    money   = ch->level * ch->level * 20;
	bone_flag   = 2;
        pAbility    = &ch->max_move;
        pOutput     = "move points";
    }

    /* --------------------------------------------*/

    else
    {
	strcpy( buf, "{o{cYou can train:" );
	if ( ch->pcdata->perm_str < 18 + race_table[ ch->race ].str_mod )
	    strcat( buf, " str" );
	if ( ch->pcdata->perm_int < 18 + race_table[ ch->race ].int_mod )
	    strcat( buf, " int" );
	if ( ch->pcdata->perm_wis < 18 + race_table[ ch->race ].wis_mod )
	    strcat( buf, " wis" );
	if ( ch->pcdata->perm_dex < 18 + race_table[ ch->race ].dex_mod )
	    strcat( buf, " dex" );
	if ( ch->pcdata->perm_con < 18 + race_table[ ch->race ].con_mod )
	    strcat( buf, " con" );

	strcat( buf, " hp mana move" );

	if ( buf[strlen( buf )-1] != ':' )
	{
	    strcat( buf, ".{x\n\r" );
	    send_to_char( buf, ch );
	    sprintf( buf, "{o{cCost is %d gold coins for attributes.{x\n\r", money );
	    send_to_char( buf, ch );
	    money   = ch->level * ch->level * 20;
	    sprintf( buf, "{o{cCost is %d gold coins per hp/mana/move.{x\n\r",
		    money );
	    send_to_char( buf, ch );
	}

	return;
    }

    if ( bone_flag == 0 )
      {
	if ( !str_cmp( argument, "str" ) )
	{
	    if ( *pAbility < 18 + race_table[ ch->race ].str_mod )
	      {
	        ok = TRUE;
	      }
	}
	else if ( !str_cmp( argument, "int" ) )
	{ 
	    if ( *pAbility < 18 + race_table[ ch->race ].int_mod )
	      {
	        ok = TRUE;
	      }
	}
	else if ( !str_cmp( argument, "wis" ) )
	{
	    if ( *pAbility < 18 + race_table[ ch->race ].wis_mod )
	      {
		ok = TRUE;
	      }
	}
	else if ( !str_cmp( argument, "dex" ) )
	{
	    if ( *pAbility < 18 + race_table[ ch->race ].dex_mod )
	      {
	        ok = TRUE;
	      }
	}
	else if ( !str_cmp( argument, "con" ) )
	{
	    if ( *pAbility < 18 + race_table[ ch->race ].con_mod )
	      {
	        ok = TRUE;
	      }
	}

	if ( !ok )
	{
	    act( "Your $T is already at maximum.",
		ch, NULL, pOutput, TO_CHAR );
	    return;
	}
    }

    if ( cost > ch->practice )
    {
	send_to_char( "You don't have enough practices.\n\r", ch );
	return;
    }
    else if ( money > ch->gold )
    {
	send_to_char( "You don't have enough money.\n\r", ch );
	return;
    }

    ch->practice        	-= cost;
    ch->gold                    -= money;

    if ( bone_flag == 0 )
        *pAbility		+= 1;
    else if ( bone_flag == 1 )
        *pAbility               += dice( 1, 2 );
    else
        *pAbility               += dice( 1, 5 );

    if ( bone_flag == 0 )
    {
        act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
        act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
	return;
    }

    act( "Your $T increase!", ch, NULL, pOutput, TO_CHAR );
    act( "$n's $T increase!", ch, NULL, pOutput, TO_ROOM );

    return;
}

void do_chameleon ( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC( ch )
	&& ch->level < skill_table[gsn_chameleon].skill_level[ch->class] )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    send_to_char( "You attempt to blend in with your surroundings.\n\r", ch);

    if ( IS_AFFECTED( ch, AFF_HIDE ) )
        REMOVE_BIT( ch->affected_by, AFF_HIDE );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_chameleon] )
        SET_BIT( ch->affected_by, AFF_HIDE );

    return;
}

/*
===========================================================================
This snippet was written by Erwin S. Andreasen, erwin@pip.dknet.dk. You may 
use this code freely, as long as you retain my name in all of the files. You
also have to mail me telling that you are using it.

All my snippets are publically available at: http://pip.dknet.dk/~pip1773/

The classic SCAN command, shows the mobs surrounding the character.

===========================================================================
*/
int scan_room( CHAR_DATA *ch, const ROOM_INDEX_DATA *room, char *buf )
{
    CHAR_DATA *target		= room->people;
    int        number_found	= 0;

    while ( target )
    {
        if ( can_see( ch, target ) )
        {
            strcat ( buf, " - " );
            strcat ( buf, IS_NPC(target) ? target->short_descr : target->name );
            strcat ( buf, "\n\r" );
            number_found++;
        }
        target = target->next_in_room;
    }
    return number_found;
}

void do_scan( CHAR_DATA *ch, char *argument )
{
    EXIT_DATA       *pexit;
    ROOM_INDEX_DATA *room;
    extern char     *const dir_name[];
    char             buf[ MAX_STRING_LENGTH ];
    int              dir;
    int              distance;

    sprintf( buf, "Right here you see:\n\r" );
    if ( scan_room( ch, ch->in_room, buf ) == 0 )
        strcat( buf, "Noone\n\r" );
    send_to_char( buf, ch );

    for ( dir = 0; dir < MAX_DIR; dir++ )
    {
        room = ch->in_room;

        for ( distance = 1 ; distance < 4; distance++ )
        {
            pexit = room->exit[dir];
            if ( !pexit
		|| !pexit->to_room
		|| IS_SET( pexit->exit_info, EX_CLOSED ) )
                break;

            sprintf( buf, "%d %s from here you see:\n\r", distance,
		    dir_name[dir] );
            if ( scan_room( ch, pexit->to_room, buf ) )
                send_to_char ( buf, ch );

            room = pexit->to_room;
        }
    }
    return;

}

void do_heighten ( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if ( !IS_NPC( ch )
	&& ch->level < skill_table[gsn_heighten].skill_level[ch->class] )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_heighten ) )
        return;

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_heighten] )
    {
	af.type      = gsn_heighten;
	af.duration  = 24;
	af.modifier  = 0;
	af.location  = APPLY_NONE;
	af.bitvector = AFF_DETECT_INVIS;
	affect_to_char( ch, &af );

	af.bitvector = AFF_DETECT_HIDDEN;
	affect_to_char( ch, &af );
	
	af.bitvector = AFF_INFRARED;
	affect_to_char( ch, &af );
	
	send_to_char( "Your senses are heightened.\n\r", ch );
    }
    return;

}

void do_shadow ( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if ( !IS_NPC( ch )
	&& ch->level < skill_table[gsn_shadow].skill_level[ch->class] )
    {
        send_to_char( "Huh?\n\r", ch );
	return;
    }

    send_to_char( "You attempt to move in the shadows.\n\r", ch );
    affect_strip( ch, gsn_shadow );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_shadow] )
    {
	af.type      = gsn_shadow;
	af.duration  = ch->level;
	af.modifier  = APPLY_NONE;
	af.location  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );
    }
    return;

}

/*
 * Bash code by Thelonius for EnvyMud (originally bash_door)
 * Damage modified using Morpheus's code
 * Message for bashproof doors by that wacky guy Kahn
 */
void do_bash( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        door;

    if ( IS_NPC( ch ) || ( !IS_NPC( ch )
			  && ch->level
			  < skill_table[gsn_bash].skill_level[ch->class] ) )
    {
	send_to_char( "You're not enough of a warrior to bash doors!\n\r",
		     ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Bash what?\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char( "You can't break off your fight.\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	int              chance;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char( "Calm down.  It is already open.\n\r", ch );
	    return;
	}

	WAIT_STATE( ch, skill_table[gsn_bash].beats );

	if ( !IS_NPC( ch ) )
	  chance = ch->pcdata->learned[gsn_bash]/2;
	else
	  chance = 0;

	if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
	    chance /= 2;

	if ( IS_SET( pexit->exit_info, EX_BASHPROOF ) )
	{
	    act( "WHAAAAM!!!  You bash against the $d, but it doesn't budge.",
		ch, NULL, pexit->keyword, TO_CHAR );
	    act( "WHAAAAM!!!  $n bashes against the $d, but it holds strong.",
		ch, NULL, pexit->keyword, TO_ROOM );
	    damage( ch, ch, ( ch->max_hit /  5 ), gsn_bash, WEAR_NONE, DAM_BASH );
	    return;
	}

	if ( ( get_curr_str( ch ) >= 20 )
	    && number_percent( ) <
	        ( chance + 4 * ( get_curr_str( ch ) - 20 ) ) )
	{
	    /* Success */

	    REMOVE_BIT( pexit->exit_info, EX_CLOSED );
	    if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
	        REMOVE_BIT( pexit->exit_info, EX_LOCKED );
	    
	    SET_BIT( pexit->exit_info, EX_BASHED );

	    act( "Crash!  You bashed open the $d!",
		ch, NULL, pexit->keyword, TO_CHAR );
	    act( "$n bashes open the $d!",
		ch, NULL, pexit->keyword, TO_ROOM );

	    damage( ch, ch, ( ch->max_hit / 20 ), gsn_bash, WEAR_NONE, DAM_BASH );

	    /* Bash through the other side */
	    if (   ( to_room   = pexit->to_room               )
		&& ( pexit_rev = to_room->exit[rev_dir[door]] )
		&& pexit_rev->to_room == ch->in_room        )
	    {
		CHAR_DATA *rch;

		REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
		if ( IS_SET( pexit_rev->exit_info, EX_LOCKED ) )
		    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );

		SET_BIT( pexit_rev->exit_info, EX_BASHED );

		for ( rch = to_room->people; rch; rch = rch->next_in_room )
		{
		    if ( rch->deleted )
		        continue;
		    act( "The $d crashes open!",
			rch, NULL, pexit_rev->keyword, TO_CHAR );
		}

	    }
	}
	else
	{
	    /* Failure */
	    
	    act( "OW!  You bash against the $d, but it doesn't budge.",
		ch, NULL, pexit->keyword, TO_CHAR );
	    act( "$n bashes against the $d, but it holds strong.",
		ch, NULL, pexit->keyword, TO_ROOM );
	    damage( ch, ch, ( ch->max_hit / 10 ), gsn_bash, WEAR_NONE, DAM_BASH );
	}
    }

    /*
     * Check for "guards"... anyone bashing a door is considered as
     * a potential aggressor, and there's a 25% chance that mobs
     * will do unto before being done unto.
     * But first...let's make sure ch ain't dead?  That'd be a pain.
     */

    if ( ch->deleted || ch->hit <= 1 )
        return;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !gch->deleted
	    && gch != ch
	    && IS_AWAKE( gch )
	    && can_see( gch, ch )
	    && !gch->fighting
	    && ( IS_NPC( gch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
	    && ( ch->level - gch->level <= 4 )
	    && number_bits( 2 ) == 0 )
	    multi_hit( gch, ch, TYPE_UNDEFINED );
    }

    return;

}

/* Snare skill by Binky for EnvyMud */
void do_snare( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *victim;
    AFFECT_DATA  af;
    char         arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    /*
     *  First, this checks for case of no second argument (valid only
     *  while fighting already).  Later, if an argument is given, it 
     *  checks validity of argument.  Unsuccessful snares flow through
     *  and receive messages at the end of the function.
     */

    if ( arg[0] == '\0' )
    {
	if ( !( victim = ch->fighting ) )
	{
	    send_to_char( "Ensnare whom?\n\r", ch );
	    return;
	}
	/* No argument, but already fighting: valid use of snare */
	WAIT_STATE( ch, skill_table[gsn_snare].beats );

	/* Only appropriately skilled PCs and uncharmed mobs */
	if ( ( IS_NPC( ch ) && !IS_AFFECTED( ch, AFF_CHARM ) )
	    || ( !IS_NPC( ch )
		&& number_percent( ) < ch->pcdata->learned[gsn_snare] ) )
	{    
	    affect_strip( victim, gsn_snare );  

	    af.type      = gsn_snare;
	    af.duration  = 1 + ( ( ch->level ) / 8 );
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_HOLD;

	    affect_to_char( victim, &af );

	    act( "You have ensnared $M!", ch, NULL, victim, TO_CHAR    );
	    act( "$n has ensnared you!",  ch, NULL, victim, TO_VICT    );
	    act( "$n has ensnared $N.",   ch, NULL, victim, TO_NOTVICT );
	}
	else
	{
	    act( "You failed to ensnare $M.  Uh oh!",
		ch, NULL, victim, TO_CHAR    );
	    act( "$n tried to ensnare you!  Get $m!",
		ch, NULL, victim, TO_VICT    );
	    act( "$n attempted to ensnare $N, but failed!",
		ch, NULL, victim, TO_NOTVICT );
	}
    }
    else				/* argument supplied */
    {
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( !IS_NPC( ch ) && !IS_NPC( victim ) )
	{
	    send_to_char( "You can't ensnare another player.\n\r", ch );
	    return;
	}

	if ( victim != ch->fighting ) /* TRUE if not fighting, or fighting  */
	{                             /* if person other than victim        */
	    if ( ch->fighting )       /* TRUE if fighting other than vict.  */ 
	    {		
		send_to_char(
		    "Take care of the person you are fighting first!\n\r",
			     ch );
		return;
	    }                             
	    WAIT_STATE( ch, skill_table[gsn_snare].beats );

	    /* here, arg supplied, ch not fighting */
	    /* only appropriately skilled PCs and uncharmed mobs */
	    if ( ( IS_NPC( ch ) && !IS_AFFECTED( ch, AFF_CHARM ) )
		|| ( !IS_NPC( ch )
		    && number_percent( ) < ch->pcdata->learned[gsn_snare] ) )
	    {
		affect_strip( victim, gsn_snare );  

		af.type      = gsn_snare;
		af.duration  = 3 + ( (ch->level ) / 8 );
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = AFF_HOLD;

		affect_to_char( victim, &af );

		act( "You have ensnared $M!", ch, NULL, victim, TO_CHAR    );
		act( "$n has ensnared you!",  ch, NULL, victim, TO_VICT    );
		act( "$n has ensnared $N.",   ch, NULL, victim, TO_NOTVICT );
	    }
	    else
	    {
		act( "You failed to ensnare $M.  Uh oh!",
		    ch, NULL, victim, TO_CHAR    );
		act( "$n tried to ensnare you!  Get $m!",
		    ch, NULL, victim, TO_VICT    );
		act( "$n attempted to ensnare $N, but failed!",
		    ch, NULL, victim, TO_NOTVICT );
	    }
	    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	    {
		/* go for the one who wanted to fight :) */
		multi_hit( victim, ch->master, TYPE_UNDEFINED );
	    }
	    else
	    {
	        multi_hit( victim, ch, TYPE_UNDEFINED );
	    }
	}
	else
	{
	    /* we are already fighting the intended victim */
	    WAIT_STATE( ch, skill_table[gsn_snare].beats );

	    /* charmed mobs not allowed to do this */
	    if ( ( IS_NPC( ch ) && !IS_AFFECTED( ch, AFF_CHARM ) )
		|| ( !IS_NPC( ch )
		    && number_percent( ) < ch->pcdata->learned[gsn_snare] ) )
	    {
		affect_strip( victim, gsn_snare );  

		af.type      = gsn_snare;
		af.duration  = 1 + ( ( ch->level ) / 8 );
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = AFF_HOLD;

		affect_to_char( victim, &af );

		act( "You have ensnared $M!", ch, NULL, victim, TO_CHAR    );
		act( "$n has ensnared you!",  ch, NULL, victim, TO_VICT    );
		act( "$n has ensnared $N.",   ch, NULL, victim, TO_NOTVICT );
	    }
	    else
	    {
		act( "You failed to ensnare $M.  Uh oh!",
		    ch, NULL, victim, TO_CHAR    );
		act( "$n tried to ensnare you!  Get $m!",
		    ch, NULL, victim, TO_VICT    );
		act( "$n attempted to ensnare $N, but failed!",
		    ch, NULL, victim, TO_NOTVICT );
	    }
	}
    }

    return;
}



/* Untangle by Thelonius for EnvyMud */
void do_untangle( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *victim;
    char         arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch )
	&& ch->level < skill_table[gsn_untangle].skill_level[ch->class] )
    {
	send_to_char( "You aren't nimble enough.\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	victim = ch;
    else if ( !( victim = get_char_room( ch, arg ) ) )
    {
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
    }

    if ( !is_affected( victim, gsn_snare ) )
	return;

    if ( ( IS_NPC( ch ) && !IS_AFFECTED( ch, AFF_CHARM ) )
	|| ( !IS_NPC( ch ) 
	    && number_percent( ) < ch->pcdata->learned[gsn_untangle] ) )
    {
	affect_strip( victim, gsn_snare );

	if ( victim != ch )
        {
	    act( "You untangle $N.",  ch, NULL, victim, TO_CHAR    );
	    act( "$n untangles you.", ch, NULL, victim, TO_VICT    );
	    act( "$n untangles $n.",  ch, NULL, victim, TO_NOTVICT );
        }
	else
        {
	    send_to_char( "You untangle yourself.\n\r", ch );
	    act( "$n untangles $mself.", ch, NULL, NULL, TO_ROOM );
        }

	return;
    }
}



/* 
 * Random room generation function.
 */
ROOM_INDEX_DATA  *get_random_room( )
{
    ROOM_INDEX_DATA *pRoomIndex;

    for ( ; ; )
    {
        pRoomIndex = get_room_index( number_range( 0, 65535 ) );
        if ( pRoomIndex )
            if (   !IS_SET( pRoomIndex->room_flags, ROOM_PRIVATE  )
                && !IS_SET( pRoomIndex->room_flags, ROOM_SOLITARY ) )
            break;
    }

    return pRoomIndex;
}



void do_enter( CHAR_DATA * ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    OBJ_DATA        *portal;
    CHAR_DATA       *fch;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Enter what?\n\r", ch );
	return;
    }

    original	= ch->in_room;
    portal	= get_obj_list( ch, argument, ch->in_room->contents );

    if ( !portal )
    {
	act( "There is no $t in here.", ch, argument, NULL, TO_CHAR );
	return;
    }

    if ( portal->item_type != ITEM_PORTAL
	|| IS_SET( portal->value[1], PORTAL_CLOSED ) )
    {
	send_to_char( "You can't seem to find a way in.\n\r", ch );
	return;
    }

    if ( portal->value[0] == 0 )
    {
	act( "$p has no power left.", ch, portal, NULL, TO_CHAR );
	return;
    }

    if ( !IS_SET( portal->value[3], PORTAL_NO_CURSED )
	&& ( IS_AFFECTED( ch, AFF_CURSE )
	    || IS_SET( original->room_flags, ROOM_NO_RECALL ) ) )
    {
	send_to_char( "God has forsaken you.\n\r", ch );
	return;
    }

    location = get_room_index( portal->value[4] );

    if ( IS_SET( portal->value[3], PORTAL_RANDOM ) )
    {
	location		= get_random_room();
	portal->value[4]	= location->vnum;
    }

    if ( IS_SET( portal->value[3], PORTAL_BUGGY ) && number_percent() < 5 )
	location = get_random_room();

    if ( !location
	|| location == original
	|| room_is_private( location ) )
    {
	act( "$p doesn't seem to go anywhere.", ch, portal, NULL, TO_CHAR );
	return;
    }

    act( "$n steps into $p.", ch, portal, NULL, TO_ROOM );
    if ( IS_SET(portal->value[3], PORTAL_RANDOM )
	|| IS_SET(portal->value[3], PORTAL_BUGGY ) )
	act( "You walk through $p and find yourself somewhere else...",
	    ch, portal, NULL, TO_CHAR );
    else
	act( "You enter $p.", ch, portal, NULL, TO_CHAR );

    char_from_room( ch );
    char_to_room( ch, location );

    if ( IS_SET( portal->value[3], PORTAL_RANDOM )
	|| IS_SET( portal->value[3], PORTAL_BUGGY ) )
	act( "$n has arrived through $p.", ch, portal, NULL, TO_ROOM );
    else
	act( "$n has arrived.", ch, portal, NULL, TO_ROOM );

    do_look( ch, "auto" );

    if ( portal->value[0] > 0 )	/*
				 * This way i prevent an underflow 
				 */
	portal->value[0]--;

    if ( portal->value[0] == 0 )/*
				 * If there are no more charges; remove 
				 */
    {
	act( "$p fades out of existence.", ch, portal, NULL, TO_CHAR );
	act( "$p fades out of existence.", ch, portal, NULL, TO_ROOM );
	extract_obj( portal );
	return;
    }

    if ( IS_SET( portal->value[3], PORTAL_GO_WITH ) )
    {
	obj_from_room( portal );
	obj_to_room( portal, location );
    }

    for ( fch = original->people; fch; fch = fch->next_in_room )
    {
	if ( fch->deleted
	    || !IS_AFFECTED( fch, AFF_CHARM )
	    || fch->master != ch )
	    continue;

	if ( fch->position < POS_STANDING )
	    do_stand( fch, "" );

	if ( fch->position == POS_STANDING )
	{
	    act( "You follow $N.", fch, NULL, ch, TO_CHAR );
	    do_enter( fch, argument );
	}
    }

    return;
}



void do_mount( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if ( ch->riding )
    {
	send_to_char( "You're already mounted!\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, argument ) ) )
    {
	send_to_char( "You can't find that here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim )
	|| !IS_SET( victim->act, ACT_MOUNTABLE ) )
    {
	send_to_char( "You can't mount that!\n\r", ch );
	return;
    }

    if ( ch->rider )
    {
        send_to_char( "You are being ridden by someone else!\n\r", ch );
        return;
    }

    if ( victim->rider )
    {
	send_to_char( "That mount already has a rider.\n\r", ch );
	return;
    }

    if ( victim->position < POS_STANDING )
    {
	send_to_char( "Your mount must be standing.\n\r", ch );
	return;
    }

    if ( victim->position == POS_FIGHTING
	|| victim->fighting )
    {
	send_to_char( "Your mount is moving around too much.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_mount].beats );
    if ( IS_NPC( ch )
	|| number_percent( ) < ch->pcdata->learned[gsn_mount] )
    {
	victim->rider = ch;
	ch->riding    = victim;
	act( "You mount $N.", ch, NULL, victim, TO_CHAR );
	act( "$n skillfully mounts $N.", ch, NULL, victim, TO_NOTVICT );
	act( "$n mounts you.", ch, NULL, victim, TO_VICT );
    }
    else
    {
	act( "You unsuccessfully try to mount $N.", ch, NULL, victim, TO_CHAR );
	act( "$n unsuccessfully attempts to mount $N.", ch, NULL, victim, TO_NOTVICT );
	act( "$n tries to mount you.", ch, NULL, victim, TO_VICT );
    }
    return;
}



void do_dismount( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if ( !( victim = ch->riding ) )
    {
	send_to_char( "You're not mounted.\n\r", ch );
	return;	
    }

    WAIT_STATE( ch, skill_table[gsn_mount].beats );
    if ( IS_NPC( ch )
	|| number_percent( ) < ch->pcdata->learned[gsn_mount] )
    {
	act( "You dismount $N.",            ch, NULL, victim, TO_CHAR );
	act( "$n skillfully dismounts $N.", ch, NULL, victim, TO_NOTVICT );
	act( "$n dismounts you.  Whew!",    ch, NULL, victim, TO_VICT );
	victim->rider = NULL;
	ch->riding    = NULL;
	ch->position  = POS_STANDING;
    }
    else
    {
	act( "You fall off while dismounting $N.  Ouch!", ch, NULL, victim, TO_CHAR );
	act( "$n falls off of $N while dismounting.", ch, NULL, victim, TO_NOTVICT );
	act( "$n falls off your back.", ch, NULL, victim, TO_VICT );
	victim->rider = NULL;
	ch->riding    = NULL;
	ch->position  = POS_RESTING;
	damage( ch, ch, 1, TYPE_UNDEFINED, WEAR_NONE, DAM_BASH );
    }
    return;
}


void do_buck( CHAR_DATA *ch, char *argument )
{
    if ( !ch->rider )
    {
        send_to_char( "There is no one riding you.\n\r", ch );
        return;
    }

    act( "$n bucks $N!",             ch, NULL, ch->rider, TO_NOTVICT );
    act( "You buck $M!",             ch, NULL, ch->rider, TO_CHAR );
    act( "$n bucks you from $m!",    ch, NULL, ch->rider, TO_VICT );

    ch->rider->riding   = NULL;
    ch->rider->position = POS_RESTING;
    ch->rider           = NULL;
    return;
}
