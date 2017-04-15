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
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#define unix 1
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



char *  const   dir_noun        [ ]             =
{
    "the north", "the east", "the south", "the west", "above", "below"
};

char *	const	dir_name	[ ]		=
{
    "north", "east", "south", "west", "up", "down"
};

const	int	rev_dir		[ ]		=
{
    2, 3, 0, 1, 5, 4
};

const	int	movement_loss	[ SECT_MAX ]	=
{
    1, 2, 2, 3, 4, 5, 4, 1, 6, 10, 6
};



/*
 * Local functions.
 */
int	find_door	args( ( CHAR_DATA *ch, char *arg, bool pMsg ) );
OBJ_DATA *has_key	args( ( CHAR_DATA *ch, int key ) );


void move_char( CHAR_DATA *ch, int door, bool Fall )
/* void move_char( CHAR_DATA *ch, int door ) */
{
    CHAR_DATA       *fch;
    CHAR_DATA       *fch_next;
    EXIT_DATA       *pexit;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;

    if ( ( IS_AFFECTED(ch, AFF_ANTI_FLEE) ) && ( !Fall ) )
     {
       if ( is_affected( ch, gsn_howlfear ) )
	 send_to_char(AT_WHITE, "You are too frightened to move.\n\r", ch );
       else
         send_to_char(AT_WHITE, "You cannot move.\n\r", ch);
       return;
     }
    if ( door < 0 || door > 5 )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    in_room = ch->in_room;

    if ( !IS_NPC( ch ) && ( !Fall ) )
    {
      int drunk = 0;
      int nd = 0;
      drunk = ch->pcdata->condition[COND_DRUNK];

      if ( number_percent() < drunk )
      {
        for ( nd = door; nd == door; nd = number_door() );
        door = nd;
        send_to_char( AT_BLUE, "You're too drunk to think clearly! You wander"
        " off in the wrong direction.\n\r", ch );
      }
    }

    if ( !( pexit = in_room->exit[door] ) || !( to_room = pexit->to_room ) )
    {
	send_to_char(AT_GREY, "Alas, you cannot go that way.\n\r", ch );
	return;
    }

    if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
    {
        if ( !IS_AFFECTED( ch, AFF_PASS_DOOR ) && ch->race != RACE_SHADOW )
        {
	    act(AT_GREY, "The &W$d&w is closed.",
		ch, NULL, pexit->keyword, TO_CHAR );
	    return;
	}
	if ( IS_SET( pexit->exit_info, EX_PASSPROOF ) )
        {
	    act(AT_GREY, "You are unable to pass through the &W$d&w.",
		ch, NULL, pexit->keyword, TO_CHAR );
	    return;
	}
    }

    if ( IS_AFFECTED( ch, AFF_CHARM )
	&& ch->master
	&& in_room == ch->master->in_room && ( !Fall ) )
    {
	send_to_char(AT_GREY, "What?  And leave your beloved master?\n\r", ch );
	return;
    }

    if ( room_is_private( to_room ) )
    {
	send_to_char(AT_GREY, "That room is private right now.\n\r", ch );
	return;
    }
    if ( to_room->vnum == ROOM_VNUM_SMITHY && ch->race != RACE_DWARF && ( !Fall ) )
	{
	send_to_char( AT_GREY, "That room is for dwarves only.\n\r", ch );
	return;
	}
    if ( !IS_NPC( ch ) )
    {
	int iClass;
	int move;

	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    if ( iClass != prime_class( ch )
		&& to_room->vnum == class_table[iClass].guild )
	    {
		send_to_char(AT_GREY, "You aren't allowed in there.\n\r", ch );
		return;
	    }
	}

	if ( ( in_room->sector_type == SECT_AIR
	    || to_room->sector_type == SECT_AIR ) && ( !Fall ) )
	{
	    if ( ( !IS_AFFECTED( ch, AFF_FLYING ) )
	       && ( ch->race != RACE_PIXIE )
	       && ( ch->race != RACE_ELDER ) )
	    {
		send_to_char(AT_GREY, "You can't fly.\n\r", ch );
		return;
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
	    if ( ( IS_AFFECTED( ch, AFF_FLYING ) )
	        || ( ch->race == RACE_PIXIE )
	        || ( ch->race == RACE_ELDER ) )
	        found = TRUE;

	    for ( obj = ch->carrying; obj; obj = obj->next_content )
	    {
		if ( obj->item_type == ITEM_BOAT )
		{
		    found = TRUE;
		    break;
		}
	    }
	    if ( ( !found ) && ( !Fall ) )
	    {
		send_to_char(AT_GREY, "You need a boat to go there.\n\r", ch );
		return;
	    }
	}

	move = movement_loss[UMIN( SECT_MAX-1, in_room->sector_type )]
	     + movement_loss[UMIN( SECT_MAX-1, to_room->sector_type )]
	     ;

	if ( ( ch->move < move ) && ( !Fall ) )
	{
	    send_to_char(AT_GREY, "You are too exhausted.\n\r", ch );
	    return;
	}

	WAIT_STATE( ch, 1 );
	if ( (!IS_AFFECTED( ch, AFF_FLYING ) )
	   && ( ch->race != RACE_PIXIE     )
	   && ( ch->race != RACE_LIZARDMAN )
           && ( ch->race != RACE_ELDER     ) )
	   ch->move -= move;
    }

    if ( !IS_AFFECTED( ch, AFF_SNEAK )
	&& ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) )
	&& ( ch->race != RACE_HALFLING ) )
	  {
	   if ( ch->hit < MAX_HIT(ch) /2 )
	   {
	       OBJ_DATA *obj;
	       char      buf[MAX_STRING_LENGTH];

	       char *	const	dir_blood[ ] =
	       {
		 "north",
		 "east",
		 "south",
		 "west",
		 "above",
		 "below"
	       };

/* Blood trickles from your wounds.\n\r */

	       send_to_char( AT_BLOOD, "Blood trickles from your wounds.\n\r", ch );
	       act( AT_RED, "$n leaves a trail of blood.", ch , NULL, NULL, TO_ROOM );

	       obj		= create_object(get_obj_index( OBJ_VNUM_BLOOD ), 0 );
	       obj->timer	= number_range( 2, 4 );

	       sprintf( buf , obj->description, dir_blood[door] );
	       free_string( obj->description );
	       obj->description = str_dup( buf );

	       obj_to_room( obj, ch->in_room );
	   }
	   if ( !Fall )
	   {
	     if ( ( !IS_AFFECTED( ch, AFF_FLYING ) )
	       && ( ch->race != RACE_PIXIE )
	       && ( ch->race != RACE_ELDER ) )
	       act(AT_GREY, "&B$n&w leaves $T.", ch, NULL, dir_name[door], TO_ROOM );
             else
               act(AT_GREY, "&B$n&w flies $T.", ch, NULL, dir_name[door], TO_ROOM );
	   }
          }
    eprog_enter_trigger( pexit, ch->in_room, ch );
    if ( ch->in_room != to_room )
    {
      char_from_room( ch );
      char_to_room( ch, to_room );
    }

    if ( !IS_AFFECTED( ch, AFF_SNEAK )
	&& ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) )
	&& ( ch->race != RACE_HALFLING ) && ( !Fall ) )
          act(AT_GREY, "&B$n&w arrives from $T.", ch, NULL,
	      dir_noun[rev_dir[door]], TO_ROOM );

    do_look( ch, "auto" );

    if ( Fall )
        act( AT_WHITE, "$n falls down from above.", ch, NULL, NULL, TO_ROOM );

    if ( to_room->exit[rev_dir[door]] &&
	 to_room->exit[rev_dir[door]]->to_room == in_room )
      eprog_exit_trigger( to_room->exit[rev_dir[door]], ch->in_room, ch );
    else
      rprog_enter_trigger( ch->in_room, ch );

    for ( fch = in_room->people; fch; fch = fch_next )
    {
        fch_next = fch->next_in_room;

        if ( fch->deleted )
	    continue;

	if ( fch->master == ch && fch->position == POS_STANDING )
	{
	    act(AT_GREY, "You follow $N.", fch, NULL, ch, TO_CHAR );
	    move_char( fch, door, FALSE );
	}
    }

    if ( IS_SET( to_room->room_flags, ROOM_NOFLOOR ) &&
     !IS_AFFECTED( ch, AFF_FLYING ) && ( ( pexit = to_room->exit[5] ) != NULL )
       && ( ( to_room = pexit->to_room ) != NULL )
       && ( ch->race != RACE_PIXIE ) && ( ch->race != RACE_ELDER ) )
    {
      act( AT_WHITE, "$n falls down to the room below.\n\r", ch,
       NULL, NULL, TO_ROOM );
      act( AT_RED, "You fall through where the floor should have been!\n\r",
       ch, NULL, NULL, TO_CHAR );
/*
     char_from_room( ch );
     char_to_room( ch, to_room );
     do_look( ch, "auto" );
*/
     move_char( ch, 5, TRUE );
/*      act( AT_WHITE, "$n falls down from above.", ch, NULL, NULL, TO_ROOM );   */
      damage( ch, ch, 5, TYPE_UNDEFINED );
    }

    if ( !IS_NPC( ch ) )
      if ( !IS_SET( ch->act, PLR_WIZINVIS ) )
      {
	mprog_greet_trigger( ch );
	return;
      }
      else return;

    mprog_entry_trigger( ch );
    mprog_greet_trigger( ch );
    return;
}



void do_north( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NORTH, FALSE );
    return;
}



void do_east( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_EAST, FALSE );
    return;
}



void do_south( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SOUTH, FALSE );
    return;
}



void do_west( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_WEST, FALSE );
    return;
}



void do_up( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_UP, FALSE );
    return;
}



void do_down( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_DOWN, FALSE );
    return;
}



int find_door( CHAR_DATA *ch, char *arg, bool pMsg )
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
	for ( door = 0; door <= 5; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] )
		&& IS_SET( pexit->exit_info, EX_ISDOOR )
		&& pexit->keyword
		&& is_name( ch, arg, pexit->keyword ) )
		return door;
	}
	if ( pMsg )
	  act(AT_GREY, "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !( pexit = ch->in_room->exit[door] ) )
    {
	if ( pMsg )
	  act(AT_GREY, "I see no door $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !IS_SET( pexit->exit_info, EX_ISDOOR ) )
    {
	if ( pMsg )
	  send_to_char(AT_GREY, "You can't do that.\n\r", ch );
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
	send_to_char(C_DEFAULT, "Open what?\n\r", ch );
	return;
    }
    if ( ( obj = get_obj_here( ch, arg ) )
    && find_door( ch, arg, FALSE ) == -1 )
    {
	/* 'open object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char(C_DEFAULT, "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED )    )
	    { send_to_char(C_DEFAULT, "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSEABLE ) )
	    { send_to_char(C_DEFAULT, "You can't do that.\n\r",      ch ); return; }
	if (  IS_SET( obj->value[1], CONT_LOCKED )    )
	    { send_to_char(C_DEFAULT, "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT( obj->value[1], CONT_CLOSED );
	send_to_char(C_DEFAULT, "Ok.\n\r", ch );
	act(C_DEFAULT, "$n opens $p.", ch, obj, NULL, TO_ROOM );
	oprog_open_trigger( obj, ch );
	return;
    }

    if ( ( door = find_door( ch, arg, TRUE ) ) >= 0 )
    {
	/* 'open door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED )  )
	    { send_to_char(C_DEFAULT, "It's already open.\n\r",     ch ); return; }
	if (  IS_SET( pexit->exit_info, EX_LOCKED )  )
	    { send_to_char(C_DEFAULT, "It's locked.\n\r",           ch ); return; }

	REMOVE_BIT( pexit->exit_info, EX_CLOSED );
	act(C_DEFAULT, "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char(C_DEFAULT, "Ok.\n\r", ch );
	eprog_open_trigger( pexit, ch->in_room, ch );

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
		act(C_DEFAULT, "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	    }
	}
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
	send_to_char(C_DEFAULT, "Close what?\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg, TRUE ) ) >= 0 )
    {
	/* 'close door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit	= ch->in_room->exit[door];
	if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char(C_DEFAULT, "It's already closed.\n\r",    ch );
	    return;
	}

	if ( IS_SET( pexit->exit_info, EX_BASHED ) )
	{
	    act(C_DEFAULT, "The $d has been bashed open and cannot be closed.",
		ch, NULL, pexit->keyword, TO_CHAR );
	    return;
	}

	SET_BIT( pexit->exit_info, EX_CLOSED );
	act(C_DEFAULT, "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char(C_DEFAULT, "Ok.\n\r", ch );
	eprog_close_trigger( pexit, ch->in_room, ch );

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
		act(C_DEFAULT, "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	    }
	}
    return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	/* 'close object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char(C_DEFAULT, "That's not a container.\n\r", ch ); return; }
	if (  IS_SET( obj->value[1], CONT_CLOSED )    )
	    { send_to_char(C_DEFAULT, "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSEABLE ) )
	    { send_to_char(C_DEFAULT, "You can't do that.\n\r",      ch ); return; }

	SET_BIT( obj->value[1], CONT_CLOSED );
	send_to_char(C_DEFAULT, "Ok.\n\r", ch );
	act(C_DEFAULT, "$n closes $p.", ch, obj, NULL, TO_ROOM );
	oprog_close_trigger( obj, ch );
    }
    return;
}



OBJ_DATA *has_key( CHAR_DATA *ch, int key )
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
    OBJ_DATA *key;
    char      arg [ MAX_INPUT_LENGTH ];
    int       door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Lock what?\n\r", ch );
	return;
    }
    if ( ( door = find_door( ch, arg, TRUE ) ) >= 0 )
    {
	/* 'lock door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit	= ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	    { send_to_char(C_DEFAULT, "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char(C_DEFAULT, "It can't be locked.\n\r",     ch ); return; }
	if ( !(key = has_key( ch, pexit->key )) )
	    { send_to_char(C_DEFAULT, "You lack the key.\n\r",       ch ); return; }
	if (  IS_SET( pexit->exit_info, EX_LOCKED ) )
	    { send_to_char(C_DEFAULT, "It's already locked.\n\r",    ch ); return; }

	SET_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char(C_DEFAULT, "*Click*\n\r", ch );
	act(C_DEFAULT, "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	eprog_lock_trigger( pexit, ch->in_room, ch, key );

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
	/* 'lock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char(C_DEFAULT, "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED ) )
	    { send_to_char(C_DEFAULT, "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char(C_DEFAULT, "It can't be locked.\n\r",     ch ); return; }
	if ( !(key = has_key( ch, obj->value[2] )) )
	    { send_to_char(C_DEFAULT, "You lack the key.\n\r",       ch ); return; }
	if (  IS_SET( obj->value[1], CONT_LOCKED ) )
	    { send_to_char(C_DEFAULT, "It's already locked.\n\r",    ch ); return; }

	SET_BIT( obj->value[1], CONT_LOCKED );
	send_to_char(C_DEFAULT, "*Click*\n\r", ch );
	act(C_DEFAULT, "$n locks $p.", ch, obj, NULL, TO_ROOM );
	oprog_lock_trigger( obj, ch, key );
    }
    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *key;
    char      arg [ MAX_INPUT_LENGTH ];
    int       door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Unlock what?\n\r", ch );
	return;
    }
    if ( ( door = find_door( ch, arg, FALSE ) ) >= 0 )
    {
	/* 'unlock door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	    { send_to_char(C_DEFAULT, "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char(C_DEFAULT, "It can't be unlocked.\n\r",   ch ); return; }
	if ( !(key = has_key( ch, pexit->key )) )
	    { send_to_char(C_DEFAULT, "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET( pexit->exit_info, EX_LOCKED ) )
	    { send_to_char(C_DEFAULT, "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char(C_DEFAULT, "*Click*\n\r", ch );
	act(C_DEFAULT, "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	eprog_unlock_trigger( pexit, ch->in_room, ch, key );

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
	/* 'unlock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char(C_DEFAULT, "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED ) )
	    { send_to_char(C_DEFAULT, "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char(C_DEFAULT, "It can't be unlocked.\n\r",   ch ); return; }
	if ( !(key = has_key( ch, obj->value[2] )) )
	    { send_to_char(C_DEFAULT, "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_LOCKED ) )
	    { send_to_char(C_DEFAULT, "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT( obj->value[1], CONT_LOCKED );
	send_to_char(C_DEFAULT, "*Click*\n\r", ch );
	act(C_DEFAULT, "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	oprog_unlock_trigger( obj, ch, key );
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
	send_to_char(C_DEFAULT, "Pick what?\n\r", ch );
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
	    act(C_DEFAULT, "$N is standing too close to the lock.",
		ch, NULL, gch, TO_CHAR );
	    return;
	}
    }

    if ( !IS_NPC( ch ) && number_percent( ) > ch->pcdata->learned[gsn_pick_lock] )
    {
	send_to_char(C_DEFAULT, "You failed.\n\r", ch);
	return;
    }
    if ( ( door = find_door( ch, arg, TRUE ) ) >= 0 )
    {
	/* 'pick door' */
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	ROOM_INDEX_DATA *to_room;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED )    )
	    { send_to_char(C_DEFAULT, "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char(C_DEFAULT, "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET( pexit->exit_info, EX_LOCKED )    )
	    { send_to_char(C_DEFAULT, "It's already unlocked.\n\r",  ch ); return; }
	if (  IS_SET( pexit->exit_info, EX_PICKPROOF ) )
	    { send_to_char(C_DEFAULT, "You failed.\n\r",             ch ); return; }

	REMOVE_BIT( pexit->exit_info, EX_LOCKED );
	send_to_char(C_DEFAULT, "*Click*\n\r", ch );
	if ( !IS_AFFECTED(ch,AFF_HIDE) )
          act(C_DEFAULT, "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	eprog_pick_trigger( pexit, ch->in_room, ch );

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
	/* 'pick object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char(C_DEFAULT, "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_CLOSED )    )
	    { send_to_char(C_DEFAULT, "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char(C_DEFAULT, "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET( obj->value[1], CONT_LOCKED )    )
	    { send_to_char(C_DEFAULT, "It's already unlocked.\n\r",  ch ); return; }
	if (  IS_SET( obj->value[1], CONT_PICKPROOF ) )
	    { send_to_char(C_DEFAULT, "You failed.\n\r",             ch ); return; }

	REMOVE_BIT( obj->value[1], CONT_LOCKED );
	send_to_char(C_DEFAULT, "*Click*\n\r", ch );
        if ( !IS_AFFECTED(ch,AFF_HIDE) )
          act(C_DEFAULT, "$n picks $p.", ch, obj, NULL, TO_ROOM );
	oprog_pick_trigger( obj, ch );
    }

    update_skpell( ch, gsn_pick_lock );
    return;
}

void do_stand( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED( ch, AFF_SLEEP ) )
	    { send_to_char(AT_CYAN, "You can't wake up!\n\r", ch ); return; }

	send_to_char(AT_CYAN, "You wake and stand up.\n\r", ch );
        if ( !IS_AFFECTED(ch,AFF_HIDE) )
	  act(AT_CYAN, "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	rprog_wake_trigger( ch->in_room, ch );
	break;

    case POS_RESTING:
	send_to_char(AT_CYAN, "You stand up.\n\r", ch );
	if ( !IS_AFFECTED(ch,AFF_HIDE) )
          act(AT_CYAN, "$n stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	rprog_wake_trigger( ch->in_room, ch );
	break;

    case POS_FIGHTING:
	send_to_char(AT_CYAN, "You are already fighting!\n\r",  ch );
	break;

    case POS_STANDING:
	send_to_char(AT_CYAN, "You are already standing.\n\r",  ch );
	break;
    }

    return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char(AT_CYAN, "You wake up and start resting.\n\r", ch );
  	if ( !IS_AFFECTED(ch,AFF_HIDE) )
	  act(AT_CYAN, "$n wakes up and rests.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_RESTING;
/*
	send_to_char(AT_CYAN, "You are already sleeping.\n\r",  ch );
*/
	rprog_rest_trigger( ch->in_room, ch );
	break;

    case POS_RESTING:
	send_to_char(AT_CYAN, "You are already resting.\n\r",   ch );
	break;

    case POS_FIGHTING:
	send_to_char(AT_CYAN, "Not while you're fighting!\n\r", ch );
	break;

    case POS_STANDING:
	send_to_char(AT_CYAN, "You rest.\n\r", ch );
	if ( !IS_AFFECTED(ch,AFF_HIDE) )
          act(AT_CYAN, "$n rests.", ch, NULL, NULL, TO_ROOM );
	rprog_rest_trigger( ch->in_room, ch );
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
	send_to_char(AT_CYAN, "You are already sleeping.\n\r",  ch );
	break;

    case POS_RESTING:
    case POS_STANDING:
	send_to_char(AT_CYAN, "You sleep.\n\r", ch );
	if ( !IS_AFFECTED(ch,AFF_HIDE) )
          act(AT_CYAN, "$n sleeps.", ch, NULL, NULL, TO_ROOM );
	rprog_sleep_trigger( ch->in_room, ch );
	ch->position = POS_SLEEPING;
	break;

    case POS_FIGHTING:
	send_to_char(AT_CYAN, "Not while you're fighting!\n\r", ch );
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
	{ send_to_char(AT_CYAN, "You are asleep yourself!\n\r",       ch ); return; }

    if ( !( victim = get_char_room( ch, arg ) ) )
	{ send_to_char(AT_CYAN, "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE( victim ) )
	{ act(AT_CYAN, "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( IS_AFFECTED( victim, AFF_SLEEP ) )
	{ act(AT_CYAN, "You can't wake $M!",   ch, NULL, victim, TO_CHAR ); return; }

    victim->position = POS_STANDING;
    act(AT_CYAN, "You wake $M.",  ch, NULL, victim, TO_CHAR );
    act(AT_CYAN, "$n wakes you.", ch, NULL, victim, TO_VICT );
    rprog_wake_trigger( victim->in_room, victim );
    return;
}


void do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_sneak ) )
    {
        send_to_char(C_DEFAULT, "Huh?\n\r", ch );
	return;
    }

    send_to_char(AT_LBLUE, "You attempt to move silently.\n\r", ch );
    affect_strip( ch, gsn_sneak );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_sneak] )
    {
	af.type      = gsn_sneak;
        af.level     = ch->level;
	af.duration  = ch->level;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );
    }

    update_skpell( ch, gsn_sneak );
    return;
}



void do_hide( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_hide ) )
    {
        send_to_char(C_DEFAULT, "Huh?\n\r", ch );
	return;
    }

    send_to_char(AT_LBLUE, "You attempt to hide.\n\r", ch );

    if ( IS_AFFECTED( ch, AFF_HIDE ) )
       affect_strip(ch, gsn_hide);

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_hide] )
    {
	af.type      = gsn_hide;
        af.level     = ch->level;
	af.duration  = ch->level;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_HIDE;
	affect_to_char( ch, &af );
    }

    update_skpell( ch, gsn_hide );
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
    affect_strip ( ch, gsn_hide                         );
    affect_strip ( ch, gsn_chameleon                    );
    affect_strip ( ch, skill_lookup("phase shift")      );
    affect_strip ( ch, skill_lookup("mist form")        );
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE            );
    REMOVE_BIT   ( ch->affected_by, AFF_PHASED          );
    REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE	);
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
    send_to_char(AT_WHITE, "Ok.\n\r", ch );
    return;
}


void do_lowrecall( CHAR_DATA *ch, char *argument )
{
    if ( ch->level < 11 )
	do_recall( ch, "" );
    else
	send_to_char( AT_GREY, "Huh?\n\r", ch );

}

void do_recall( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    CHAR_DATA       *pet;
    ROOM_INDEX_DATA *location;
    char             buf [ MAX_STRING_LENGTH ];
    int              place;
    char             name[ MAX_STRING_LENGTH ];
    CLAN_DATA       *pClan;
    if ( !strcmp( ch->name, "Jekka" ) )
	{
	location = get_room_index( 2301 );
        act(C_DEFAULT, "$n disappears.", ch, NULL, NULL, TO_ROOM );
        char_from_room( ch );
        char_to_room( ch, location );
        act(C_DEFAULT, "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
        do_look( ch, "auto" );
	return;
	}

    if (!(pClan=get_clan_index(ch->clan)))
     {
       ch->clan = 0;
       pClan=get_clan_index(ch->clan);
     }
    sprintf( name, "%s", pClan->diety );
    act(C_DEFAULT, "$n prays for transportation!", ch, NULL, NULL, TO_ROOM );

    if ( ( ch->clan != 0 ) && ( ch->combat_timer < 1 ) && !IS_ARENA(ch) )
	place = pClan->recall;
    else
       place = ch->in_room->area->recall;
    if ( place == ROOM_VNUM_JEZ && ch->alignment < -350 )
	place = ROOM_VNUM_LAIK;
    if ( !( location = get_room_index( place ) ) )
    {
	send_to_char(C_DEFAULT, "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
	|| IS_AFFECTED( ch, AFF_CURSE ) )
    {
	act(C_DEFAULT, "$T has forsaken you.", ch, NULL, name, TO_CHAR );
	return;
    }

    if ( ( victim = ch->fighting ) )
    {
	int lose;

	if ( number_bits( 1 ) == 0 )
	{
	    WAIT_STATE( ch, 4 );
	    if ( !IS_ARENA(ch) )
	    {
	      lose = ( ch->desc ) ? 50 : 100;
	      gain_exp( ch, 0 - lose );
	      sprintf( buf, "You failed!  You lose %d exps.\n\r", lose );
	    }
	    else
	      strcpy(buf, "You failed!");
	    send_to_char(C_DEFAULT, buf, ch );
	    return;
	}

	if ( !IS_ARENA(ch) )
	{
	  lose = ( ch->desc ) ? 100 : 200;
	  gain_exp( ch, 0 - lose );
	  sprintf( buf, "You recall from combat!  You lose %d exps.\n\r", lose );
	}
	else
	  strcpy(buf, "You recall from combat!");
	send_to_char(C_DEFAULT, buf, ch );
	stop_fighting( ch, TRUE );
    }

    for ( pet = ch->in_room->people; pet; pet = pet->next_in_room )
    {
      if ( IS_NPC( pet ) )
        if ( IS_SET( pet->act, ACT_PET ) && ( pet->master == ch ) )
        {
          if ( pet->fighting )
            stop_fighting( pet, TRUE );
          break;
        }
    }
    act(C_DEFAULT, "$n disappears.", ch, NULL, NULL, TO_ROOM );
    if ( victim != NULL && IS_ARENA(ch) )
      act(AT_RED, "$n has escaped!  Hunt $m down!", ch, NULL, victim,
          TO_VICT);
    char_from_room( ch );
    char_to_room( ch, location );
    act(C_DEFAULT, "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    if ( pet )
    {
      act( C_DEFAULT, "$n disappears.", pet, NULL, NULL, TO_ROOM );
      char_from_room( pet );
      char_to_room( pet, location );
      act(C_DEFAULT, "$n appears in the room.", pet, NULL, NULL, TO_ROOM );
    }

    return;
}



void do_train( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mob;
    char      arg[ MAX_INPUT_LENGTH ];
    char      arg1[ MAX_STRING_LENGTH ];
    int        amt = 1;
    int        cnt = 1;
    char      *pOutput;
    char       buf [ MAX_STRING_LENGTH ];
    int       *pAbility;
    int        cost;
    int        bone_flag = 0; /*Added for training of hp ma mv */

    if ( IS_NPC( ch ) )
	return;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

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
	send_to_char(AT_WHITE, "You can't do that here.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	sprintf( buf, "You have %d practice sessions.\n\r", ch->practice );
	send_to_char(AT_CYAN, buf, ch );
	argument = "foo";
    }
    if ( arg1[0] == '\0' )
       amt = 1;
    else
       amt = atoi( arg1 );

    cost = 5;

    if ( amt < 0 )
    {
    send_to_char(C_DEFAULT, "Try again, cheater.\n\r", ch );
    return;
    }

    if ( !str_cmp( arg, "str" ) )
    {
	if ( class_table[prime_class( ch )].attr_prime == APPLY_STR )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_str;
	pOutput     = "strength";
    }

    else if ( !str_cmp( arg, "int" ) )
    {
	if ( class_table[prime_class( ch )].attr_prime == APPLY_INT )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_int;
	pOutput     = "intelligence";
    }

    else if ( !str_cmp( arg, "wis" ) )
    {
	if ( class_table[prime_class( ch )].attr_prime == APPLY_WIS )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_wis;
	pOutput     = "wisdom";
    }

    else if ( !str_cmp( arg, "dex" ) )
    {
	if ( class_table[prime_class( ch )].attr_prime == APPLY_DEX )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_dex;
	pOutput     = "dexterity";
    }

    else if ( !str_cmp( arg, "con" ) )
    {
	if ( class_table[prime_class( ch )].attr_prime == APPLY_CON )
	    cost    = 3;
	pAbility    = &ch->pcdata->perm_con;
	pOutput     = "constitution";
    }

    /* ---------------- By Bonecrusher ------------------- */

    else if ( !str_cmp( arg, "hp" ) )
    {
 	    cost    = 1;
	bone_flag   = 1;
        pAbility    = &ch->perm_hit;
        pOutput     = "hit points";
    }

    else if ( !is_class( ch, CLASS_VAMPIRE )
	 && !str_cmp( arg, "mana" ) )
    {
 	    cost    = 1;
	bone_flag   = 1;
        pAbility    = &ch->perm_mana;
        pOutput     = "mana points";
    }
    else if ( is_class( ch, CLASS_VAMPIRE )
	 && !str_cmp( arg, "bp" ) )
    {
	cost	    = 3;
	bone_flag   = 1;
	pAbility    = &ch->perm_bp;
	pOutput     = "blood points";
    }
    else if ( !str_cmp( arg, "move" ) )
    {
 	    cost    = 1;
	bone_flag   = 2;
        pAbility    = &ch->perm_move;
        pOutput     = "move points";
    }

    /* --------------------------------------------*/

    else
    {
	strcpy( buf, "You can train:" );
	if ( ch->pcdata->perm_str < 18 ) strcat( buf, " str" );
	if ( ch->pcdata->perm_int < 18 ) strcat( buf, " int" );
	if ( ch->pcdata->perm_wis < 18 ) strcat( buf, " wis" );
	if ( ch->pcdata->perm_dex < 18 ) strcat( buf, " dex" );
	if ( ch->pcdata->perm_con < 18 ) strcat( buf, " con" );

	if ( !is_class( ch, CLASS_VAMPIRE ) )
	    strcat( buf, " hp mana move" );
	else
	    strcat( buf, " hp bp move" );
	if ( buf[strlen( buf )-1] != ':' )
	{
	    strcat( buf, ".\n\r" );
	    send_to_char(AT_CYAN, buf, ch );
	}

	return;
    }

    if ( *pAbility >= 18 && bone_flag == 0 )
    {
	act(AT_CYAN, "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }

    if ( ( cost*amt ) > ch->practice || amt < 0 )
    {
	send_to_char(AT_CYAN, "You don't have enough practices.\n\r", ch );
	return;
    }

    ch->practice        	-= cost*amt;

    for( cnt = 1; cnt <= amt; cnt++ )
    {
        if ( bone_flag == 0 )
            *pAbility		+= 1;
        else if ( bone_flag == 1 )
            *pAbility               += dice( 1, 5 );
        else
            *pAbility               += dice( 1, 10 );
    }

    if ( bone_flag == 0 && amt == 1 )
    {
        act(AT_CYAN, "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
        act(AT_CYAN, "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
	return;
    }

    act(AT_CYAN, "Your $T increase!", ch, NULL, pOutput, TO_CHAR );
    act(AT_CYAN, "$n's $T increase!", ch, NULL, pOutput, TO_ROOM );

    return;
}

void do_raise( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mob;
    char      arg[ MAX_INPUT_LENGTH ];
    char      arg1[ MAX_STRING_LENGTH ];
    int        amt = 1;
    char      *pOutput;
    char       buf [ MAX_STRING_LENGTH ];
    int       *pAbility = NULL;
    int        bone_flag = 0;

    if ( IS_NPC( ch ) )
        return;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC( mob ) && IS_SET( mob->act, ACT_TRAIN ) )
            break;
    }

    if ( !mob )
    {
        send_to_char(AT_WHITE, "You can't do that here.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        sprintf( buf, "You have %d raise points.\n\r", ch->raisepts );
        send_to_char(AT_CYAN, buf, ch );
        argument = "foo";
    }
    if ( arg1[0] == '\0' )
       amt = 1;
    else
       amt = atoi( arg1 );

    if ( !str_cmp( arg, "str" ) )
    {
        pAbility    = &ch->pcdata->perm_str;
        pOutput     = "strength";
    }

    else if ( !str_cmp( arg, "int" ) )
    {
        pAbility    = &ch->pcdata->perm_int;
        pOutput     = "intelligence";
    }

    else if ( !str_cmp( arg, "wis" ) )
    {
        pAbility    = &ch->pcdata->perm_wis;
        pOutput     = "wisdom";
    }

    else if ( !str_cmp( arg, "dex" ) )
    {
        pAbility    = &ch->pcdata->perm_dex;
        pOutput     = "dexterity";
    }

    else if ( !str_cmp( arg, "con" ) )
    {
        pAbility    = &ch->pcdata->perm_con;
        pOutput     = "constitution";
    }


    else if ( !str_cmp( arg, "hp" ) )
    {
        bone_flag   = 1;
        pAbility    = &ch->perm_hit;
        pOutput     = "hit points";
    }

    else if ( !is_class( ch, CLASS_VAMPIRE )
         && !str_cmp( arg, "mana" ) )
    {
        bone_flag   = 1;
        pAbility    = &ch->perm_mana;
        pOutput     = "mana points";
    }
    else if ( is_class( ch, CLASS_VAMPIRE )
         && !str_cmp( arg, "bp" ) )
    {
        bone_flag   = 1;
        pAbility    = &ch->perm_bp;
        pOutput     = "blood points";
    }
    else if ( !str_cmp( arg, "move" ) )
    {
        bone_flag   = 2;
        pAbility    = &ch->perm_move;
        pOutput     = "move points";
    }



    else
    {
        strcpy( buf, "You can raise:" );
        if ( ch->pcdata->perm_str < 18 ) strcat( buf, " str" );
        if ( ch->pcdata->perm_int < 18 ) strcat( buf, " int" );
        if ( ch->pcdata->perm_wis < 18 ) strcat( buf, " wis" );
        if ( ch->pcdata->perm_dex < 18 ) strcat( buf, " dex" );
        if ( ch->pcdata->perm_con < 18 ) strcat( buf, " con" );

        if ( !is_class( ch, CLASS_VAMPIRE ) )
            strcat( buf, " hp mana move" );
        else
            strcat( buf, " hp bp move" );
        if ( buf[strlen( buf )-1] != ':' )
        {
            strcat( buf, ".\n\r" );
            send_to_char(AT_CYAN, buf, ch );
        }

        return;
    }

    if ( *pAbility >= 18 && bone_flag == 0 )
    {
        act(AT_CYAN, "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
        return;
    }

    if ( amt > ch->raisepts )
    {
        send_to_char(AT_CYAN, "You don't have enough raise points.\n\r", ch );
        return;
    }

    ch->raisepts                -= amt;

        if ( bone_flag == 0 )
            *pAbility           += 1;
        else if ( bone_flag == 1 )
            *pAbility               += amt;
        else
            *pAbility               += amt;

    if ( bone_flag == 0 && amt == 1 )
    {
       act(AT_CYAN, "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
        act(AT_CYAN, "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
        return;
    }

   act(AT_CYAN, "Your $T increase!", ch, NULL, pOutput, TO_CHAR );
    act(AT_CYAN, "$n's $T increase!", ch, NULL, pOutput, TO_ROOM );


    return;
}


void do_chameleon ( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_chameleon ) )
    {
        send_to_char(C_DEFAULT, "Huh?\n\r", ch );
	return;
    }

    send_to_char(AT_DGREY, "You attempt to blend in with your surroundings.\n\r", ch);

    if ( IS_AFFECTED( ch, AFF_HIDE ) )
        REMOVE_BIT( ch->affected_by, AFF_HIDE );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_chameleon] )
    {
	af.type      = gsn_chameleon;
	af.level     = ch->level;
	af.duration  = ch->level;
        af.location  = APPLY_NONE;
 	af.modifier  = 0;
 	af.bitvector = AFF_HIDE;
	affect_to_char( ch, &af );
    }

    update_skpell( ch, gsn_chameleon );
     /*   SET_BIT( ch->affected_by, AFF_HIDE );*/

    return;
}

void do_heighten ( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_heighten ) )
    {
        send_to_char(C_DEFAULT, "Huh?\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_heighten ) )
        return;

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_heighten] )
    {
	af.type      = gsn_heighten;
        af.level     = ch->level;
	af.duration  = ch->level;
	af.modifier  = 0;
	af.location  = APPLY_NONE;
	af.bitvector = AFF_DETECT_INVIS;
	affect_to_char( ch, &af );

	af.bitvector = AFF_DETECT_HIDDEN;
	affect_to_char( ch, &af );

	af.bitvector = AFF_INFRARED;
	affect_to_char( ch, &af );

	send_to_char(AT_BLUE, "Your senses are heightened.\n\r", ch );
        update_skpell( ch, gsn_heighten );
    }

    return;

}

void do_shadow ( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_shadow ) )
    {
        send_to_char(C_DEFAULT, "Huh?\n\r", ch );
	return;
    }

    send_to_char(AT_DGREY, "You attempt to move in the shadows.\n\r", ch );
    affect_strip( ch, gsn_shadow );

    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_shadow] )
    {
	af.type      = gsn_shadow;
        af.level     = ch->level;
	af.duration  = ch->level;
	af.modifier  = APPLY_NONE;
	af.location  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );

	update_skpell( ch, gsn_shadow );
    }
    return;

}

/*
 * Bash code by Thelonius for EnvyMud (originally bash_door)
 * Damage modified using Morpheus's code
 */
void do_enter ( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;
    ROOM_INDEX_DATA *in_room;
    OBJ_DATA *obj;
    char      arg1 [ MAX_INPUT_LENGTH ];
    int       destination;
    ROOM_INDEX_DATA *location;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Enter what?\n\r", ch );
	return;
    }
    if ( !( obj = get_obj_list( ch, arg1, ch->in_room->contents ) ) )
      {
       act(AT_WHITE, "You cannot enter that", ch, NULL, NULL, TO_CHAR );
       return;
      }
    if ( obj->item_type != ITEM_PORTAL )
      {
       send_to_char(AT_WHITE, "There is nothing to enter here.\n\r", ch );
       return;
      }
    in_room = ch->in_room;
    destination = obj->value[0];
    if ( !( location = get_room_index( destination ) ) )
    {
	act(AT_BLUE, "You try to enter $p but can't.", ch, obj, NULL, TO_CHAR );
	return;
    }
    act( AT_BLUE, "You step into the $p.",ch,obj,NULL, TO_CHAR );
    act(AT_BLUE, "$n steps into the $p and is gone.", ch, obj, NULL, TO_ROOM );
    rprog_exit_trigger( ch->in_room, ch );
    char_from_room( ch );
    char_to_room( ch, location );
    send_to_char(AT_BLUE, "You feel dizzy, and when finally you emerge you are elsewhere.\n\r", ch );
    act(AT_BLUE, "$n steps out of the $p before you.", ch, obj, NULL, TO_ROOM);
    do_look( ch, "auto" );
    for ( fch = in_room->people; fch; fch = fch_next )
    {
        fch_next = fch->next_in_room;

        if ( fch->deleted )
	    continue;

	if ( fch->master == ch && fch->position == POS_STANDING )
	{
	    act(AT_WHITE, "You follow $N through $p.", fch, obj, ch, TO_CHAR );
	    do_enter( fch, arg1 );
	}
    }
    rprog_enter_trigger( ch->in_room, ch );
    return;
}

void do_bash ( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        door;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_bash_door ) )
    {
	send_to_char(C_DEFAULT,
	  "You're not enough of a warrior to bash doors!\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Bash what?\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char(C_DEFAULT, "You can't break off your fight.\n\r", ch );
	return;
    }

    if ( ( door = find_door( ch, arg, TRUE ) ) >= 0 )
    {
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA       *pexit;
	EXIT_DATA       *pexit_rev;
	int              chance;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    send_to_char(C_DEFAULT, "Calm down.  It is already open.\n\r", ch );
	    return;
	}

	WAIT_STATE( ch, skill_table[gsn_bash_door].beats );

	chance = ch->pcdata->learned[gsn_bash_door]/2;
	if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
	    chance /= 2;

	if ( IS_SET( pexit->exit_info, EX_BASHPROOF ) )
	{
	    act(C_DEFAULT, "WHAAAAM!!!  You bash against the $d, but it doesn't budge.",
		ch, NULL, pexit->keyword, TO_CHAR );
	    act(C_DEFAULT, "WHAAAAM!!!  $n bashes against the $d, but it holds strong.",
		ch, NULL, pexit->keyword, TO_ROOM );
	    damage( ch, ch, ( MAX_HIT(ch) / 5 ), gsn_bash_door );
	    return;
	}

	if ( ( get_curr_str( ch ) >= 20 )
	    && number_percent( ) < ( chance + 4 * ( get_curr_str( ch ) - 20 ) ) )
	{
	    /* Success */

	    REMOVE_BIT( pexit->exit_info, EX_CLOSED );
	    if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
	        REMOVE_BIT( pexit->exit_info, EX_LOCKED );

	    SET_BIT( pexit->exit_info, EX_BASHED );

	    act(C_DEFAULT, "Crash!  You bashed open the $d!", ch, NULL, pexit->keyword, TO_CHAR );
	    act(C_DEFAULT, "$n bashes open the $d!",          ch, NULL, pexit->keyword, TO_ROOM );

	    damage( ch, ch, ( MAX_HIT(ch) / 20 ), gsn_bash_door );
	    update_skpell( ch, gsn_bash_door );

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
		    act(C_DEFAULT, "The $d crashes open!",
			rch, NULL, pexit_rev->keyword, TO_CHAR );
		}

	    }
	}
	else
	{
	    /* Failure */

	    act(C_DEFAULT, "OW!  You bash against the $d, but it doesn't budge.",
		ch, NULL, pexit->keyword, TO_CHAR );
	    act(C_DEFAULT, "$n bashes against the $d, but it holds strong.",
		ch, NULL, pexit->keyword, TO_ROOM );
	    damage( ch, ch, ( MAX_HIT(ch) / 10 ), gsn_bash_door );
	}
    }

    /*
     * Check for "guards"... anyone bashing a door is considered as
     * a potential aggressor, and there's a 25% chance that mobs
     * will do unto before being done unto.
     */
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if ( gch->deleted )
	    continue;
	if ( IS_AWAKE( gch )
	    && !gch->fighting
	    && ( IS_NPC( gch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
	    && ( ch->level - gch->level <= 4 )
	    && number_bits( 2 ) == 0 )
	    multi_hit( gch, ch, TYPE_UNDEFINED );
    }

    return;

}

/* XORPHOX push/drag */
void do_push(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  EXIT_DATA *pexit;
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *from_room;
  int door;
  char buf1[256], buf2[256], buf3[256];

  argument = one_argument(argument, arg1);
  one_argument(argument, arg2);

  if(arg1[0] == '\0')
  {
    send_to_char(AT_BLUE, "Push who what where?", ch);
    return;
  }

  if((victim = get_char_room(ch, arg1)) == NULL)
  {
    send_to_char(AT_BLUE, "They aren't here.\n\r", ch);
    return;
  }

  if (    ( victim->level >= LEVEL_IMMORTAL )
       || (    IS_NPC( victim )
            && (    ( victim->pIndexData->pShop )
                 || IS_SET( ch->in_room->room_flags, ROOM_SMITHY )
                 || IS_SET( ch->in_room->room_flags, ROOM_BANK )
                 || IS_SET( victim->act, ACT_NOPUSH )
               )
          )
     )
  {
    act(AT_BLUE, "$N ignores you.", ch, NULL, victim, TO_CHAR);
    return;
  }

/*  if(abs(ch->level - victim->level) > 5 && !IS_IMMORTAL(ch))
  {
    send_to_char(AT_BLUE, "Keep it in level please.\n\r", ch);
    return;
  }

  if(IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    && ch->clan == 0)
  {
    send_to_char(AT_BLUE, "Join a clan before messing with clanned people.\n\r", ch);
    return;
  }

  if(IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    && victim->combat_timer==0
    && victim->clan == 0)*/
/*  if ( is_safe(ch, victim) )
  {
    send_to_char(AT_BLUE, "This room is safe from the likes of you.\n\r", ch);
    return;
  }*/
  if ( !str_cmp( arg2, "n" ) || !str_cmp( arg2, "north" ) ) door = 0;
  else if ( !str_cmp( arg2, "e" ) || !str_cmp( arg2, "east"  ) ) door = 1;
  else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "south" ) ) door = 2;
  else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "west"  ) ) door = 3;
  else if ( !str_cmp( arg2, "u" ) || !str_cmp( arg2, "up"    ) ) door = 4;
  else if ( !str_cmp( arg2, "d" ) || !str_cmp( arg2, "down"  ) ) door = 5;
  else door = dice(1,6) - 1;

  if(ch == victim)
  {
    send_to_char(AT_BLUE, "You attempt to push yourself, oook.\n\r", ch);
    return;
  }

  if(victim->position != POS_STANDING)
  {
    send_to_char(AT_BLUE, "Can't push someone who is not standing.\n\r", ch);
    return;
  }
  pexit = ch->in_room->exit[door];
  if(pexit == NULL || IS_SET(pexit->exit_info, EX_CLOSED))
  {
    act(AT_BLUE, "There is no exit, but you push $M around anyways.", ch, NULL, victim, TO_CHAR);
    act(AT_BLUE, "$n pushes $N against a wall.", ch, NULL, victim, TO_NOTVICT);
    act(AT_BLUE, "$n pushes you against a wall, ouch.", ch, NULL, victim, TO_VICT);
    return;
  }
  if ( pexit->to_room->vnum == ROOM_VNUM_SMITHY )
	{
	act(AT_BLUE, "You slam into $N, but a force field prevents $S entry.", ch, NULL, victim, TO_CHAR );
	act(AT_BLUE, "$n pushes $N, but a force field makes $M bounce back.", ch, NULL, victim, TO_NOTVICT );
	act(AT_BLUE, "$n slams into you, but you bounce off a force field.", ch, NULL, victim, TO_VICT );
	return;
	}
  sprintf(buf1, "You slam into $N, pushing $M %s.", dir_name[door]);
  sprintf(buf2, "$n slams into $N, pushing $M %s.", dir_name[door]);
  sprintf(buf3, "$n slams into you, pushing you %s.", dir_name[door]);
  act(AT_BLUE, buf2, ch, NULL, victim, TO_NOTVICT );
  act(AT_BLUE, buf1, ch, NULL, victim, TO_CHAR );
  act(AT_BLUE, buf3, ch, NULL, victim, TO_VICT );
  from_room = victim->in_room;
  eprog_enter_trigger( pexit, victim->in_room, victim );
  char_from_room(victim);
  char_to_room(victim, pexit->to_room);

  act(AT_BLUE, "$n comes flying into the room.", victim, NULL, NULL, TO_ROOM);
  if ( (pexit = pexit->to_room->exit[rev_dir[door]]) &&
       pexit->to_room == from_room )
    eprog_exit_trigger( pexit, victim->in_room, victim );
  else
    rprog_enter_trigger( victim->in_room, victim );

  return;
}

void do_drag(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  EXIT_DATA *pexit;
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *from_room;
  int door;
  char buf1[256], buf2[256], buf3[256];

  argument = one_argument(argument, arg1);
  one_argument(argument, arg2);

  if(arg1[0] == '\0')
  {
    send_to_char(AT_BLUE, "Drag who what where?", ch);
    return;
  }

  if((victim = get_char_room(ch, arg1)) == NULL)
  {
    send_to_char(AT_BLUE, "They aren't here.\n\r", ch);
    return;
  }

  if (    ( victim->level >= LEVEL_IMMORTAL )
       || (    IS_NPC( victim )
            && (    ( victim->pIndexData->pShop )
                 || IS_SET( ch->in_room->room_flags, ROOM_SMITHY )
                 || IS_SET( ch->in_room->room_flags, ROOM_BANK )
                 || IS_SET( victim->act, ACT_NODRAG )
               )
          )
     )
  {
    act(AT_BLUE, "$N ignores you.", ch, NULL, victim, TO_CHAR);
    return;
  }

/*  if(abs(ch->level - victim->level) > 5 && !IS_IMMORTAL(ch))
  {
    send_to_char(AT_BLUE, "Keep it in level please.\n\r", ch);
    return;
  }

  if(IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    && ch->clan == 0)
  {
    send_to_char(AT_BLUE, "Join a clan before messing with clanned people.\n\r", ch);
    return;
  }

  if(IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    && victim->combat_timer==0
    && victim->clan == 0)*/
/*  if ( is_safe( ch, victim ) )
  {
    send_to_char(AT_BLUE, "This room is safe from the likes of you.\n\r", ch);
    return;
  }*/
  if ( !str_cmp( arg2, "n" ) || !str_cmp( arg2, "north" ) ) door = 0;
  else if ( !str_cmp( arg2, "e" ) || !str_cmp( arg2, "east"  ) ) door = 1;
  else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "south" ) ) door = 2;
  else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "west"  ) ) door = 3;
  else if ( !str_cmp( arg2, "u" ) || !str_cmp( arg2, "up"    ) ) door = 4;
  else if ( !str_cmp( arg2, "d" ) || !str_cmp( arg2, "down"  ) ) door = 5;
  else door = dice(1,6) - 1;

  if(ch == victim)
  {
    send_to_char(AT_BLUE, "You attempt to drag yourself, oook.\n\r", ch);
    return;
  }

  if(victim->position == POS_STANDING)
  {
    send_to_char(AT_BLUE, "Can't drag someone who is standing.\n\r", ch);
    return;
  }
  pexit = ch->in_room->exit[door];
  if(pexit == NULL || IS_SET(pexit->exit_info, EX_CLOSED))
  {
    act(AT_BLUE, "There is no exit, but you drag $M around anyways.", ch, NULL, victim, TO_CHAR);
    act(AT_BLUE, "$n drags $N around the room.", ch, NULL, victim, TO_NOTVICT);
    act(AT_BLUE, "$n drags you around the room.", ch, NULL, victim, TO_VICT);
    return;
  }
  if ( pexit->to_room->vnum == ROOM_VNUM_SMITHY )
    {
    act(AT_BLUE, "You grap onto $N, but a force field prevents you entry into the smithy.", ch, NULL, victim, TO_CHAR );
    act(AT_BLUE, "$n attempts to drag $N into the smithy, but a force field stops $m.", ch, NULL, victim, TO_NOTVICT );
    act(AT_BLUE, "$n attempts to drag you into the smithy, but a force field stops $m.", ch, NULL, victim, TO_VICT );
    return;
    }
  sprintf(buf1, "You get ahold of $N, dragging $M %s.", dir_name[door]);
  sprintf(buf2, "$n gets ahold of $N, dragging $M %s.", dir_name[door]);
  sprintf(buf3, "$n gets ahold of you, dragging you %s.", dir_name[door]);
  act(AT_BLUE, buf2, ch, NULL, victim, TO_NOTVICT);
  act(AT_BLUE, buf1, ch, NULL, victim, TO_CHAR);
  act(AT_BLUE, buf3, ch, NULL, victim, TO_VICT);

  from_room = ch->in_room;
  eprog_enter_trigger( pexit, ch->in_room, ch );
  eprog_enter_trigger( pexit, victim->in_room, victim );
  char_from_room(victim);
  char_to_room(victim, pexit->to_room);
  act(AT_BLUE, "$N arrives, dragging $n with $M.", victim, NULL, ch, TO_ROOM);
  char_from_room(ch);
  char_to_room(ch, victim->in_room);
  if ( (pexit = pexit->to_room->exit[rev_dir[door]]) &&
       pexit->to_room == from_room )
  {
    eprog_exit_trigger( pexit, ch->in_room, ch );
    eprog_exit_trigger( pexit, victim->in_room, victim );
  }
  else
  {
    rprog_enter_trigger( ch->in_room, ch );
    rprog_enter_trigger( victim->in_room, victim );
  }

  return;
}
/* END */

void check_nofloor( CHAR_DATA *ch )
{
  EXIT_DATA *pexit;
  ROOM_INDEX_DATA *to_room;

  if ( IS_SET( ch->in_room->room_flags, ROOM_NOFLOOR )
      && ( ( pexit = ch->in_room->exit[5] ) != NULL )
      && ( ( to_room = pexit->to_room )  != NULL )
      && ( ch->race != RACE_PIXIE ) && ( ch->race != RACE_ELDER ) )
/*      && ( !IS_AFFECTED( ch, AFF_FLYING ) ) )  */
  {
    act( AT_RED, "You fall through where the floor should have been!", ch,
        NULL, NULL, TO_CHAR );
    act( C_DEFAULT, "$n falls down to the room below.", ch, NULL, NULL,
        TO_ROOM );
    damage( ch, ch, 5, TYPE_UNDEFINED );

/*char_from_room( ch );
char_to_room( ch, to_room );
do_look( ch, "auto" );
*/
    move_char( ch, 5, TRUE );
/*    act( AT_WHITE, "$n falls down from above.", ch, NULL, NULL, TO_ROOM ); */
  }
  return;
}
void do_shadow_walk( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg[MAX_STRING_LENGTH];
    bool found = FALSE;

   one_argument( argument, arg );

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_shadow_walk ) )
    {
	send_to_char(AT_GREY,
	    "You know nothing of walking the shadows.\n\r", ch );
	return;
    }

    if ( ch->fighting )
    {
	send_to_char(C_DEFAULT, "Not while in combat.\n\r", ch );
	return;
    }
    if ( arg[0] == '\0' )
    {
      send_to_char(AT_GREY, "Shadow walk to whom?\n\r", ch );
      return;
    }

    if ( IS_AFFECTED( ch, AFF_ANTI_FLEE ) )
    {
      send_to_char( AT_GREY, "You cannot shadow walk in your condition!\n\r", ch );
      return;
    }

/* OLD CODE */
/*

    if ( !( victim = get_char_world( ch, arg ) )
	|| victim->in_room->area != ch->in_room->area
	|| IS_SET( victim->in_room->room_flags, ROOM_PRIVATE   )
	|| IS_SET( victim->in_room->room_flags, ROOM_SOLITARY  )
	|| IS_SET( ch->in_room->room_flags, ROOM_NO_SHADOW )
        || IS_SET( victim->in_room->room_flags, ROOM_NO_SHADOW )
	|| IS_AFFECTED( victim, AFF_NOASTRAL ) )
    {
	send_to_char(AT_GREY, "The shadows offer no path to that one.\n\r", ch );
	return;
    }
*/
    for ( victim = char_list; victim; victim = victim->next )
	{
	if ( victim->deleted
	||   victim->in_room->area != ch->in_room->area
	||   IS_SET( victim->in_room->room_flags, ROOM_PRIVATE )
	||   IS_SET( victim->in_room->room_flags, ROOM_SOLITARY )
	||   IS_SET( ch->in_room->room_flags, ROOM_NO_SHADOW )
	||   IS_SET( victim->in_room->room_flags, ROOM_NO_SHADOW )
	||   IS_SET( victim->act, ACT_NOSHADOW )
	||   IS_AFFECTED( victim, AFF_NOASTRAL )
	||   !is_name( ch, arg, victim->name ) )
	  continue;
	found = TRUE;
	break;
	}
    if ( !found )
    {
	send_to_char(AT_GREY, "The shadows offer no path to that one.\n\r", ch );
	return;
    }

    if ( !IS_AFFECTED(ch,AFF_HIDE) )
      act(AT_GREY, "$n steps into the shadows and is gone.", ch, NULL, NULL, TO_ROOM );
    if ( ch != victim )
    {
     char_from_room( ch );
     char_to_room( ch, victim->in_room );
    }
    if ( !IS_AFFECTED(ch,AFF_HIDE) )
      act(AT_GREY, "$n steps forth from the shadows.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    update_skpell( ch, gsn_shadow_walk );
    return;
}

void do_scent( CHAR_DATA *ch, char *argument )
{
  static const char *dir_table [ ] =
	{ "to the north",
	  "to the east",
	  "to the south",
	  "to the west",
	  "above",
	  "below" };
  static const char *dis_table [ ] =
	{ "close by",
	  "not far off" };
  CHAR_DATA *sch;
  EXIT_DATA *pexit;
  ROOM_INDEX_DATA *in_room;
  ROOM_INDEX_DATA *next_room;
  char buf [ MAX_INPUT_LENGTH * 4 ];
  int dir, dis;
  bool found = FALSE;
  const char *dir_message;
  const char *dis_message;
  if ( IS_NPC( ch ) )
	return;
  if ( !can_use_skpell( ch, gsn_scent ) )
	{
	send_to_char( C_DEFAULT, "Your sense of smell is not keen enough.\n\r",
			ch );
	return;
	}
  if ( ch->pcdata->learned[gsn_scent] < number_percent( ) )
	{
	send_to_char( C_DEFAULT, "You sniff around and start to sneeze!.\n\r",
			ch );
	return;
	}
  in_room = ch->in_room;
  for ( dir = 0; dir <= 5; dir++ )
    {
    if ( !( pexit = in_room->exit[dir] )
    || !( next_room = pexit->to_room ) )
        continue;
    for ( dis = 0; dis <= 1; dis++ )
	{
	for ( sch = next_room->people; sch; sch = sch->next_in_room )
	    {
	    if ( sch->deleted )
		return;
	    if ( !(sch->desc)
	    && !IS_NPC( sch )
	    && get_trust( ch ) < LEVEL_IMMORTAL )
		continue;
	    if ( !found )
		{
		send_to_char( C_DEFAULT, "You pick up the scent of:\n\r", ch );
		found = TRUE;
		}
	    dir_message = dir_table[dir];
	    dis_message = dis_table[dis];
	    sprintf( buf, "%s &w%s %s.\n\r", capitalize (PERS( sch, ch )),
		     dis_message, dir_message );
	    send_to_char( C_DEFAULT, buf, ch );
	    }
	if ( !( pexit = next_room->exit[dir] )
	|| !( next_room = pexit->to_room ) )
	     break;
	}
    }

  update_skpell( ch, gsn_scent );

  if ( !found )
     send_to_char( C_DEFAULT, "You did not pick up anything's scent.\n\r", ch );

  return;

}
void do_retreat( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *smokebomb;
  ROOM_INDEX_DATA *in_room;
  EXIT_DATA *pexit;
  static const char *dir_verb [ ] =
  { "north", "east", "south",
    "west", "up", "down" };
  int dir;
  const char *sdir;
  char buf[ MAX_INPUT_LENGTH ];
  if ( !can_use_skpell( ch, gsn_retreat )
  || IS_NPC( ch ) )
	{
	send_to_char( AT_GREY, "Huh?\n\r", ch );
	return;
	}
  if ( !ch->fighting )
	{
	send_to_char( AT_GREY, "Retreat from what? You're not even fighting!\n\r", ch );
	return;
	}
  if ( argument[0] == '\0' )
	{
	send_to_char(AT_GREY, "You're retreating, not fleeing!  Provide a direction!\n\r", ch );
	return;
	}
  for ( smokebomb = ch->carrying; smokebomb; smokebomb = smokebomb->next )
   {
   if ( smokebomb->pIndexData->vnum == OBJ_VNUM_SMOKEBOMB )
	break;
   }
  if ( !smokebomb )
   {
   send_to_char( AT_GREY, "You need a smoke bomb to retreat!\n\r", ch );
   return;
   }
  if ( IS_AFFECTED( ch, AFF_ANTI_FLEE ) )
   {
   if ( is_affected( ch, gsn_howlfear ) )
     send_to_char( AT_WHITE, "You are too frightened to plan an escape.\n\r", ch );
   else
     send_to_char( AT_WHITE, "You can't move, how do you expect to retreat?\n\r", ch );
   return;
   }
  send_to_char(AT_DGREY, "You throw your smoke bomb to the ground and...\n\r", ch );
  act( AT_DGREY, "$n throws $s smokebomb to the ground and...", ch, NULL,
	NULL, TO_ROOM );
  if ( number_percent( ) < ch->pcdata->learned[gsn_retreat] )
  {
   update_skpell( ch, gsn_retreat );
   extract_obj( smokebomb );
   switch( UPPER( argument[0] ) )
   {
   case 'N': dir = 0; break;
   case 'S': dir = 2; break;
   case 'E': dir = 1; break;
   case 'W': dir = 3; break;
   case 'U': dir = 4; break;
   case 'D': dir = 5; break;
   default: dir  = number_range( 0, 5 );
   };
   sdir = dir_verb[dir];
   in_room = ch->in_room;
   if ( !(pexit=in_room->exit[dir] )
   || !pexit->to_room )
    {
    send_to_char( AT_GREY, "Wham!  Ouch! Retreated straight into a wall!\n\r", ch );
    STUN_CHAR( ch, 1, STUN_TOTAL );
    ch->position = POS_STUNNED;
    return;
    }
   else
    {
    if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
      {
      sprintf( buf, "Wham!  Ouch!  Retreated straight into the closed %s.\n\r",
	       pexit->keyword );
      send_to_char( AT_GREY, buf, ch );
      STUN_CHAR( ch, 1, STUN_TOTAL );
      ch->position = POS_STUNNED;
      return;
      }
    else
      {
      sprintf( buf, "You retreat %s before the smoke clears.\n\r", sdir );
      send_to_char( AT_DGREY, buf, ch );
      act(AT_DGREY,
          "Smoke rises from the bomb... when it clears, $n is gone!",
	  ch, NULL, NULL, TO_ROOM );
      move_char( ch, dir, FALSE );
      return;
      }
    }
   }
  else
   {
   obj_from_char( smokebomb );
   obj_to_room( smokebomb, ch->in_room );
   send_to_char( AT_GREY, "The bomb failed to explode!", ch );
   act( AT_GREY, "$n's smokebomb duds.", ch, NULL, NULL, TO_ROOM );
   WAIT_STATE( ch, skill_table[gsn_retreat].beats );
   return;
   }
}
