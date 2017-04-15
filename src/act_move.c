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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



char *	const	dir_name	[]		=
{
    "north", "east", "south", "west", "up", "down"
};

const	sh_int	rev_dir		[]		=
{
    2, 3, 0, 1, 5, 4
};

const	sh_int	movement_loss	[SECT_MAX]	=
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};



/*
 * Local functions.
 */
int	find_door	args( ( CHAR_DATA *ch, char *arg ) );
bool	has_key		args( ( CHAR_DATA *ch, int key ) );
int	count_imms	args( ( CHAR_DATA *ch ) );



void move_char( CHAR_DATA *ch, int door )
{
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    char buf  [MAX_INPUT_LENGTH];
    char poly [MAX_INPUT_LENGTH];
    char mount [MAX_INPUT_LENGTH];
    char leave [20];
    int revdoor;

    if ( door < 0 || door > 5 )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    in_room = ch->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->to_room      ) == NULL )
    {
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return;
    }

    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    &&   !IS_AFFECTED(ch, AFF_PASS_DOOR)
    &&   !IS_AFFECTED(ch, AFF_ETHEREAL)
    &&   !IS_AFFECTED(ch, AFF_SHADOWPLANE) )
    {
	act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM)
    &&   ch->master != NULL
    &&   in_room == ch->master->in_room )
    {
	send_to_char( "What?  And leave your beloved master?\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) && ch->mount != NULL && IS_SET(ch->mounted,IS_MOUNT))
    {
	send_to_char( "You better wait for instructions from your rider.\n\r", ch );
	return;
    }

    if ( room_is_private( to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    if ( (IS_LEG_L(ch,BROKEN_LEG) || IS_LEG_L(ch,LOST_LEG)) &&
	 (IS_LEG_R(ch,BROKEN_LEG) || IS_LEG_R(ch,LOST_LEG)) &&
	 (IS_ARM_L(ch,BROKEN_ARM) || IS_ARM_L(ch,LOST_ARM) ||
	    get_eq_char(ch, WEAR_HOLD) != NULL) &&
	 (IS_ARM_R(ch,BROKEN_ARM) || IS_ARM_R(ch,LOST_ARM) ||
	    get_eq_char(ch, WEAR_WIELD) != NULL))
    {
	send_to_char( "You need at least one free arm to drag yourself with.\n\r", ch );
	return;
    }
    else if ( IS_BODY(ch,BROKEN_SPINE) &&
	 (IS_ARM_L(ch,BROKEN_ARM) || IS_ARM_L(ch,LOST_ARM) ||
	    get_eq_char(ch, WEAR_HOLD) != NULL) &&
	 (IS_ARM_R(ch,BROKEN_ARM) || IS_ARM_R(ch,LOST_ARM) ||
	    get_eq_char(ch, WEAR_WIELD) != NULL))
    {
	send_to_char( "You need at least one free arm to drag yourself with.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) )
    {
	int move;
	if ( in_room->sector_type == SECT_AIR
	||   to_room->sector_type == SECT_AIR )
	{
	    if ( !IS_AFFECTED(ch, AFF_FLYING) &&
		 (!IS_NPC(ch) && !IS_VAMPAFF(ch, VAM_FLYING))
	    &&   !(ch->mount != NULL && IS_SET(ch->mounted,IS_RIDING) &&
		 IS_AFFECTED(ch->mount, AFF_FLYING) ) )
	    {
		send_to_char( "You can't fly.\n\r", ch );
		return;
	    }
	}

	if ( in_room->sector_type == SECT_WATER_NOSWIM
	||   to_room->sector_type == SECT_WATER_NOSWIM )
	{
	    OBJ_DATA *obj;
	    bool found;

	    /*
	     * Look for a boat.
	     */
	    found = FALSE;
	    if ( !IS_NPC(ch) && IS_SET(ch->act,PLR_VAMPIRE) )
	    {
		if ( IS_VAMPAFF(ch, VAM_FLYING) )
		    found = TRUE;
		else if ( IS_POLYAFF(ch, POLY_SERPENT) )
		    found = TRUE;
		else if ( IS_AFFECTED(ch, AFF_SHADOWPLANE) )
		    found = TRUE;
		else if (ch->mount != NULL && IS_SET(ch->mounted, IS_RIDING) &&
		    IS_AFFECTED(ch->mount, AFF_FLYING))
		    found = TRUE;
		else
		{
		    send_to_char( "You are unable to cross running water.\n\r", ch );
		    return;
		}
	    }
	    if ( IS_AFFECTED(ch, AFF_FLYING) )
		found = TRUE;
	    if ( !found )
	    {
	    	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
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
	}

	move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
	     + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)]
	     ;

	if (IS_HERO(ch)) move = 0;

	if ( ch->move < move || ch->move <= 0 )
	{
	    send_to_char( "You are too exhausted.\n\r", ch );
	    return;
	}

	WAIT_STATE( ch, 1 );
	ch->move -= move;
    }

    /* Check for mount message - KaVir */
    if ( ch->mount != NULL && ch->mounted == IS_RIDING )
    {
	if (IS_NPC(ch->mount))
	    sprintf(mount," on %s.",ch->mount->short_descr);
	else
	    sprintf(mount," on %s.",ch->mount->name);
    }
    else
	sprintf(mount,".");

    if ( IS_HEAD(ch,LOST_HEAD) )
	sprintf(leave,"rolls");
    else if ( IS_BODY(ch,BROKEN_SPINE) )
	sprintf(leave,"drags $mself");
    else if ( IS_LEG_L(ch,LOST_LEG) && IS_LEG_R(ch,LOST_LEG) )
	sprintf(leave,"drags $mself");
    else if ( (IS_LEG_L(ch,BROKEN_LEG) || IS_LEG_L(ch,LOST_LEG) || IS_LEG_L(ch,LOST_FOOT)) &&
	 (IS_LEG_R(ch,BROKEN_LEG) || IS_LEG_R(ch,LOST_LEG) || IS_LEG_R(ch,LOST_FOOT)) )
	sprintf(leave,"crawls");
    else if ( ch->hit < (ch->max_hit/4) )
	sprintf(leave,"crawls");
    else if ( (IS_LEG_R(ch,LOST_LEG) || IS_LEG_R(ch,LOST_FOOT)) &&
	 (!IS_LEG_L(ch,BROKEN_LEG) && !IS_LEG_L(ch,LOST_LEG) && 
	  !IS_LEG_L(ch,LOST_FOOT)) )
	sprintf(leave,"hops");
    else if ( (IS_LEG_L(ch,LOST_LEG) || IS_LEG_L(ch,LOST_FOOT)) &&
	 (!IS_LEG_R(ch,BROKEN_LEG) && !IS_LEG_R(ch,LOST_LEG) &&
	  !IS_LEG_R(ch,LOST_FOOT)) )
	sprintf(leave,"hops");
    else if ( (IS_LEG_L(ch,BROKEN_LEG) || IS_LEG_L(ch,LOST_FOOT)) &&
	 (!IS_LEG_R(ch,BROKEN_LEG) && !IS_LEG_R(ch,LOST_LEG) &&
	  !IS_LEG_R(ch,LOST_FOOT)) )
	sprintf(leave,"limps");
    else if ( (IS_LEG_R(ch,BROKEN_LEG) || IS_LEG_R(ch,LOST_FOOT)) &&
	 (!IS_LEG_L(ch,BROKEN_LEG) && !IS_LEG_L(ch,LOST_LEG) &&
	  !IS_LEG_L(ch,LOST_FOOT)) )
	sprintf(leave,"limps");
    else if ( ch->hit < (ch->max_hit/3) )
	sprintf(leave,"limps");
    else if ( ch->hit < (ch->max_hit/2) )
	sprintf(leave,"staggers");
    else
	sprintf(leave,"walks");

    if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_SNEAK) && IS_AFFECTED(ch,AFF_POLYMORPH)
    && ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ))
    {
	if ((ch->mount != NULL && ch->mounted == IS_RIDING &&
	IS_AFFECTED(ch->mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	(!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
	    sprintf(poly,"%s flies $T%s",ch->morph,mount);
	else if ( ch->mount != NULL && ch->mounted == IS_RIDING )
	    sprintf(poly,"%s rides $T%s",ch->morph,mount);
	else
	    sprintf(poly,"%s %s $T%s",ch->morph,leave,mount);
	act( poly, ch, NULL, dir_name[door], TO_ROOM );
    }
    else if ( !IS_AFFECTED(ch, AFF_SNEAK)
    && ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ) )
    {
	if ((ch->mount != NULL && ch->mounted == IS_RIDING &&
	IS_AFFECTED(ch->mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	(!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
	    sprintf(poly,"$n flies $T%s",mount);
	else if ( ch->mount != NULL && ch->mounted == IS_RIDING )
	    sprintf(poly,"$n rides $T%s",mount);
	else
	    sprintf(poly,"$n %s $T%s",leave,mount);
	act( poly, ch, NULL, dir_name[door], TO_ROOM );
    }

    if (!IS_NPC(ch) && ch->stance[0] != -1) do_stance(ch,"");
    char_from_room( ch );
    char_to_room( ch, to_room );
    if      ( door == 0 ) {revdoor = 2;sprintf(buf,"the south");}
    else if ( door == 1 ) {revdoor = 3;sprintf(buf,"the west");}
    else if ( door == 2 ) {revdoor = 0;sprintf(buf,"the north");}
    else if ( door == 3 ) {revdoor = 1;sprintf(buf,"the east");}
    else if ( door == 4 ) {revdoor = 5;sprintf(buf,"below");}
    else                  {revdoor = 4;sprintf(buf,"above");}

    if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_SNEAK) && IS_AFFECTED(ch,AFF_POLYMORPH)
    && ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ))
    {
	if ((ch->mount != NULL && ch->mounted == IS_RIDING &&
	IS_AFFECTED(ch->mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	(!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
	    sprintf(poly,"%s flies in from %s%s",ch->morph,buf,mount);
	else if ( ch->mount != NULL && ch->mounted == IS_RIDING )
	    sprintf(poly,"%s rides in from %s%s",ch->morph,buf,mount);
	else
	    sprintf(poly,"%s %s in from %s%s",ch->morph,leave,buf,mount);
	act( poly, ch, NULL, NULL, TO_ROOM );
    }
    else if ( !IS_AFFECTED(ch, AFF_SNEAK)
    && ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ) )
    {
	if ((ch->mount != NULL && ch->mounted == IS_RIDING &&
	IS_AFFECTED(ch->mount,AFF_FLYING)) || IS_AFFECTED(ch,AFF_FLYING) ||
	(!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FLYING)) )
	    sprintf( poly, "$n flies in from %s%s",buf,mount);
	else if ( ch->mount != NULL && ch->mounted == IS_RIDING )
	    sprintf(poly,"$n rides in from %s%s",buf,mount);
	else
	    sprintf( poly, "$n %s in from %s%s",leave,buf,mount);
	act( poly, ch, NULL, NULL, TO_ROOM );
    }

    do_look( ch, "auto" );

    for ( fch = in_room->people; fch != NULL; fch = fch_next )
    {
	fch_next = fch->next_in_room;
	if ( fch->mount == ch && IS_SET(fch->mounted,IS_MOUNT) )
	{
	    act( "$N digs $S heels into you.", fch, NULL, ch, TO_CHAR );
	    char_from_room(fch);
	    char_to_room(fch,ch->in_room);
	/*
	    move_char( fch, door );
	*/
	}

	if ( fch->master == ch && fch->position == POS_STANDING )
	{
	    act( "You follow $N.", fch, NULL, ch, TO_CHAR );
	    move_char( fch, door );
	}
    }
    room_text(ch,">ENTER<");
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
    if (!IS_NPC(ch) && ch->stance[0] != -1) do_stance(ch,"");
    move_char( ch, DIR_UP );
    return;
}



void do_down( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch) && ch->stance[0] != -1) do_stance(ch,"");
    move_char( ch, DIR_DOWN );
    return;
}



int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int door;

	 if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else
    {
	for ( door = 0; door <= 5; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] ) != NULL
	    &&   IS_SET(pexit->exit_info, EX_ISDOOR)
	    &&   pexit->keyword != NULL
	    &&   is_name( arg, pexit->keyword ) )
		return door;
	}
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return -1;
    }

    return door;
}

/* Designed for the portal spell, but can also have other uses...KaVir
 * V0 = Where the portal will take you.
 * V1 = Number of uses (0 is infinate).
 * V2 = if 2, cannot be entered.
 * V3 = The room the portal is currently in.
 */
void do_enter( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *location;
    char arg[MAX_INPUT_LENGTH];
    char poly [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *portal;
    OBJ_DATA *portal_next;
    CHAR_DATA *mount;
    bool found;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Enter what?\n\r", ch );
	return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
    {
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return;
    }
    if ( obj->item_type != ITEM_PORTAL )
    {
	act( "You cannot enter that.", ch, NULL, arg, TO_CHAR );
	return;
    }

    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
	!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) )
    {
	send_to_char( "You are too insubstantual.\n\r", ch );
	return;
    }
    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
	IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) )
    {
	send_to_char( "It is too insubstantual.\n\r", ch );
	return;
    }

    if ( obj->value[2] == 2 || obj->value[2] == 3 )
    {
	act( "It seems to be closed.", ch, NULL, arg, TO_CHAR );
	return;
    }

    pRoomIndex = get_room_index(obj->value[0]);
    location = ch->in_room;

    if ( pRoomIndex == NULL )
    {
	act( "You are unable to enter.", ch, NULL, arg, TO_CHAR );
	return;
    }

    act( "You step into $p.",  ch, obj, NULL, TO_CHAR );
    if ( !IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH) )
	sprintf(poly,"%s steps into $p.",ch->morph);
    else
	sprintf(poly,"$n steps into $p.");
    act( poly,  ch, obj, NULL, TO_ROOM );
    char_from_room(ch);
    char_to_room(ch,pRoomIndex);
    if ( !IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH) )
	sprintf(poly,"%s steps out of $p.",ch->morph);
    else
	sprintf(poly,"$n steps out of $p.");
    act( poly,  ch, obj, NULL, TO_ROOM );
    char_from_room(ch);
    char_to_room(ch,location);
/* Leave this out for now, as it doesn't seem to work properly. KaVir
    if ((obj->value[2] == 1) && (CAN_WEAR(obj,ITEM_TAKE)))
    {
	obj->value[2] = 2;
	REMOVE_BIT(obj->wear_flags, ITEM_TAKE);
    }
*/
    if (obj->value[1] != 0)
    {
	obj->value[1] = obj->value[1] - 1;
	if (obj->value[1] < 1)
	{
	    act( "$p vanishes.",  ch, obj, NULL, TO_CHAR );
	    act( "$p vanishes.",  ch, obj, NULL, TO_ROOM );
	    extract_obj(obj);
	}
    }
    char_from_room(ch);
    char_to_room(ch,pRoomIndex);
    found = FALSE;
    for ( portal = ch->in_room->contents; portal != NULL; portal = portal_next )
    {
	portal_next = portal->next_content;
	if ( ( obj->value[0] == portal->value[3]  )
	    && (obj->value[3] == portal->value[0]) )
	{
	    found = TRUE;
/* Leave this out for now, as it doesn't seem to work properly. KaVir
	    if ((portal->value[2] == 2) && (!CAN_WEAR(obj,ITEM_TAKE)))
	    {
		portal->value[2] = 1;
		SET_BIT(obj->wear_flags, ITEM_TAKE);
	    }
*/
	    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
		!IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
	    {
		REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
		break;
	    }
	    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
		IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
	    {
		SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
		break;
	    }
    	    if (portal->value[1] != 0)
    	    {
		portal->value[1] = portal->value[1] - 1;
		if (portal->value[1] < 1)
		{
		    act( "$p vanishes.",  ch, portal, NULL, TO_CHAR );
		    act( "$p vanishes.",  ch, portal, NULL, TO_ROOM );
		    extract_obj(portal);
		}
	    }
    	}
    }
    do_look(ch,"auto");
    if ( ( mount = ch->mount ) == NULL ) return;
    char_from_room( mount );
    char_to_room( mount, ch->in_room );
    return;
}


void do_open( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'open object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	send_to_char( "Ok.\n\r", ch );
	act( "$n opens $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'open door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* open the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'close object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }

	SET_BIT(obj->value[1], CONT_CLOSED);
	send_to_char( "Ok.\n\r", ch );
	act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'close door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* close the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



bool has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == key )
	    return TRUE;
    }

    return FALSE;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'lock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'lock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* lock the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'unlock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'unlock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* unlock the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_pick( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

    /* look for guards */
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
	{
	    act( "$N is standing too close to the lock.",
		ch, NULL, gch, TO_CHAR );
	    return;
	}
    }

    if ( !IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[gsn_pick_lock] )
    {
	send_to_char( "You failed.\n\r", ch);
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'pick object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'pick door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* pick the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}




void do_stand( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
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

    case POS_STANDING:
	send_to_char( "You are already standing.\n\r", ch );
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_STANDING:
	send_to_char( "You rest.\n\r", ch );
	act( "$n rests.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_RESTING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_sleep( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
    case POS_STANDING: 
	send_to_char( "You sleep.\n\r", ch );
	act( "$n sleeps.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SLEEPING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_wake( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	{ do_stand( ch, argument ); return; }

    if ( !IS_AWAKE(ch) )
	{ send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{ send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE(victim) )
	{ act( "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( IS_AFFECTED(victim, AFF_SLEEP) )
	{ act( "You can't wake $M!",   ch, NULL, victim, TO_CHAR );  return; }

    if (victim->position < 4)
	{ act( "$E doesn't respond!",   ch, NULL, victim, TO_CHAR );  return; }

    act( "You wake $M.", ch, NULL, victim, TO_CHAR );
    act( "$n wakes you.", ch, NULL, victim, TO_VICT );
    victim->position = POS_STANDING;
    return;
}



void do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    send_to_char( "You attempt to move silently.\n\r", ch );
    affect_strip( ch, gsn_sneak );

    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_sneak] )
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
    send_to_char( "You attempt to hide.\n\r", ch );

    if ( IS_AFFECTED(ch, AFF_HIDE) )
	REMOVE_BIT(ch->affected_by, AFF_HIDE);

    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_hide] )
	SET_BIT(ch->affected_by, AFF_HIDE);

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
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
    REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE	);
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_recall( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *mount;
    ROOM_INDEX_DATA *location;
  
    act( "$n's body flickers with green energy.", ch, NULL, NULL, TO_ROOM );
    act( "Your body flickers with green energy.", ch, NULL, NULL, TO_CHAR );

    if ( ( location = get_room_index( ch->home ) ) == NULL )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE) )
    {
	send_to_char( "You are unable to recall.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) != NULL )
    {
	if ( number_bits( 1 ) == 0 )
	{
	    WAIT_STATE( ch, 4 );
	    sprintf( buf, "You failed!\n\r" );
	    send_to_char( buf, ch );
	    return;
	}
	sprintf( buf, "You recall from combat!\n\r");
	send_to_char( buf, ch );
	stop_fighting( ch, TRUE );
    }

    act( "$n disappears.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    if ( ( mount = ch->mount ) == NULL ) return;
    char_from_room( mount );
    char_to_room( mount, ch->in_room );
    return;
}

void do_home( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if ( arg[0] == '\0' || str_cmp(arg,"here") )
    {
	send_to_char( "If you wish this to be your room, you must type 'home here'.\n\r", ch );
	return;
    }

    if ( ch->in_room->vnum == ch->home )
    {
	send_to_char( "But this is already your home!\n\r", ch );
	return;
    }

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) ||
	 IS_SET(ch->in_room->room_flags, ROOM_SAFE) )
    {
	send_to_char( "You are unable to make this room your home.\n\r", ch );
	return;
    }

    ch->home = ch->in_room->vnum;
    send_to_char( "This room is now your home.\n\r", ch );

    return;
}


void do_escape( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;

    if (IS_NPC(ch) || !IS_HERO(ch))
	return;
  
    if ( ch->position >= POS_SLEEPING )
    {
	send_to_char( "You can only do this if you are dying.\n\r", ch );
	return;
    }

    if ( ( location = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    ch->move = 0;
    ch->mana = 0;
    act( "$n fades out of existance.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n fades into existance.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    sprintf(buf,"%s has escaped defenceless from a fight.",ch->name);
    do_info(ch,buf);

    return;
}


void do_train( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    sh_int *pAbility;
    char *pOutput;
    int cost;
    int immcost;
    int primal;
    bool last = TRUE;
    bool is_ok = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( IS_NPC(ch) )
	return;

    if ( arg1[0] == '\0' )
    {
	sprintf( buf, "You have %d experience points.\n\r", ch->exp );
	send_to_char( buf, ch );
	strcpy(arg1,"foo");
    }

         if (!str_cmp(arg1,"str")   ) is_ok = TRUE;
    else if (!str_cmp(arg1,"int")   ) is_ok = TRUE;
    else if (!str_cmp(arg1,"wis")   ) is_ok = TRUE;
    else if (!str_cmp(arg1,"dex")   ) is_ok = TRUE;
    else if (!str_cmp(arg1,"con")   ) is_ok = TRUE;
    else if (!str_cmp(arg1,"hp")    ) is_ok = TRUE;
    else if (!str_cmp(arg1,"mana")  ) is_ok = TRUE;
    else if (!str_cmp(arg1,"move")  ) is_ok = TRUE;
    else if (!str_cmp(arg1,"primal")) is_ok = TRUE;
    if ( arg2[0] != '\0' && is_ok )
    {
	int amount = 0;

	if (!is_number(arg2))
	{
	    send_to_char("Please enter a numeric value.\n\r",ch);
	    return;
	}
	amount = atoi( arg2 );
	if (amount < 1 || amount > 50)
	{
	    send_to_char("Please enter a value between 1 and 50.\n\r",ch);
	    return;
	}
	if (amount > 1)
	{
	    sprintf(buf,"%s %d",arg1,amount-1);
	    do_train(ch,buf);
	    last = FALSE;
	}
    }

    cost = 200;
    immcost = count_imms(ch);
    primal = (1+ch->practice)*500;

    if ( !str_cmp( arg1, "str" ) )
    {
	pAbility    = &ch->pcdata->perm_str;
	pOutput     = "strength";
    }

    else if ( !str_cmp( arg1, "int" ) )
    {
	pAbility    = &ch->pcdata->perm_int;
	pOutput     = "intelligence";
    }

    else if ( !str_cmp( arg1, "wis" ) )
    {
	pAbility    = &ch->pcdata->perm_wis;
	pOutput     = "wisdom";
    }

    else if ( !str_cmp( arg1, "dex" ) )
    {
	pAbility    = &ch->pcdata->perm_dex;
	pOutput     = "dexterity";
    }

    else if ( !str_cmp( arg1, "con" ) )
    {
	pAbility    = &ch->pcdata->perm_con;
	pOutput     = "constitution";
    }

    else if ( !str_cmp( arg1, "avatar") && ch->level == 2)
    {
        cost        = 1000;
	pAbility    = &ch->level;
	pOutput     = "level";
    }

    else if ( !str_cmp( arg1, "hp") && ch->max_hit < 30000)
    {
        cost        = (ch->max_hit - ch->pcdata->perm_con);
	pAbility    = &ch->max_hit;
	pOutput     = "hp";
    }

    else if ( !str_cmp( arg1, "mana") && ch->max_mana < 30000)
    {
        cost        = (ch->max_mana - ch->pcdata->perm_wis);
	pAbility    = &ch->max_mana;
	pOutput     = "mana";
    }

    else if ( !str_cmp( arg1, "move") && ch->max_move < 30000)
    {
        cost        = (ch->max_move - ch->pcdata->perm_con);
	pAbility    = &ch->max_move;
	pOutput     = "move";
    }

    else if ( !str_cmp( arg1, "primal") && ch->practice < 100)
    {
        cost        = primal;
	pAbility    = &ch->practice;
	pOutput     = "primal";
    }

    else if ( !str_cmp( arg1, "slash") && !IS_IMMUNE(ch,IMM_SLASH) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_SLASH);
	send_to_char( "You are now more resistant to slashing and slicing weapons.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "stab") && !IS_IMMUNE(ch,IMM_STAB) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_STAB);
	send_to_char( "You are now more resistant to stabbing and piercing weapons.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "smash") && !IS_IMMUNE(ch,IMM_SMASH) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_SMASH);
	send_to_char( "You are now more resistant to blasting, pounding and crushing weapons.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "beast") && !IS_IMMUNE(ch,IMM_ANIMAL) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_ANIMAL);
	send_to_char( "You are now more resistant to claw and bite attacks.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "grab") && !IS_IMMUNE(ch,IMM_MISC) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_MISC);
	send_to_char( "You are now more resistant to grepping, sucking and whipping weapons.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "charm") && !IS_IMMUNE(ch,IMM_CHARM) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_CHARM);
	send_to_char( "You are now immune to charm spells.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "heat") && !IS_IMMUNE(ch,IMM_HEAT) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_HEAT);
	send_to_char( "You are now immune to heat and fire spells.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "cold") && !IS_IMMUNE(ch,IMM_COLD) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_COLD);
	send_to_char( "You are now immune to cold spells.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "lightning") && !IS_IMMUNE(ch,IMM_LIGHTNING) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_LIGHTNING);
	send_to_char( "You are now immune to lightning and electrical spells.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "acid") && !IS_IMMUNE(ch,IMM_ACID) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_ACID);
	send_to_char( "You are now immune to acid spells.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "sleep") && !IS_IMMUNE(ch,IMM_SLEEP) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_SLEEP);
	send_to_char( "You are now immune to the sleep spell.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "drain") && !IS_IMMUNE(ch,IMM_DRAIN) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_DRAIN);
	send_to_char( "You are now immune to the energy drain spell.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "voodoo") && !IS_IMMUNE(ch,IMM_VOODOO) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_VOODOO);
	send_to_char( "You are now immune to voodoo magic.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "hurl") && !IS_IMMUNE(ch,IMM_HURL) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_HURL);
	send_to_char( "You are now immune to being hurled.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "backstab") && !IS_IMMUNE(ch,IMM_BACKSTAB) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_BACKSTAB);
	send_to_char( "You are now immune to being backstabbed.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "kick") && !IS_IMMUNE(ch,IMM_KICK) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_KICK);
	send_to_char( "You are now immune to being kicked.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "disarm") && !IS_IMMUNE(ch,IMM_DISARM) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_DISARM);
	send_to_char( "You are now immune to being disarmed.\n\r", ch );
	return;
    }

    else if ( !str_cmp( arg1, "steal") && !IS_IMMUNE(ch,IMM_STEAL) )
    {
	if (ch->exp < immcost)
	{
	    send_to_char( "You don't have enough exp.\n\r", ch );
	    return;
	}
	ch->exp = ch->exp - immcost;
	SET_BIT(ch->immune, IMM_STEAL);
	send_to_char( "You are now immune to being stolen from.\n\r", ch );
	return;
    }
    else
    {
	sprintf( buf, "You can train the following:\n\r" );
	send_to_char( buf, ch );

	send_to_char( "Stats:", ch );
	if ( ch->pcdata->perm_str < 18 ) send_to_char( " Str", ch );
	if ( ch->pcdata->perm_int < 18 ) send_to_char( " Int", ch );
	if ( ch->pcdata->perm_wis < 18 ) send_to_char( " Wis", ch );
	if ( ch->pcdata->perm_dex < 18 ) send_to_char( " Dex", ch );
	if ( ch->pcdata->perm_con < 18 ) send_to_char( " Con", ch );
	if ( ( ch->pcdata->perm_str >= 18 )
		&& ( ch->pcdata->perm_wis >= 18 )
		&& ( ch->pcdata->perm_int >= 18 )
		&& ( ch->pcdata->perm_dex >= 18 )
		&& ( ch->pcdata->perm_con >= 18 ) )
	    send_to_char( " None left to train.\n\r", ch );
	else
	    send_to_char( ".\n\r", ch );

	if ( ch->level == 2 )
	{
	   sprintf( buf, "Become an avatar - 1000 exp.\n\r" );
	   send_to_char( buf, ch );
	}
	if ( ch->max_hit       < 30000 )
	{
	   sprintf( buf, "hp               - %d exp per point.\n\r",(ch->max_hit - ch->pcdata->perm_con) );
	   send_to_char( buf, ch );
	}
	if ( ch->max_mana      < 30000 )
	{
	   sprintf( buf, "mana             - %d exp per point.\n\r",(ch->max_mana - ch->pcdata->perm_wis) );
	   send_to_char( buf, ch );
	}
	if ( ch->max_move      < 30000 )
	{
	   sprintf( buf, "move             - %d exp per point.\n\r",(ch->max_move - ch->pcdata->perm_con) );
	   send_to_char( buf, ch );
	}
	if ( ch->practice        < 100 )
	{
	   sprintf( buf, "primal           - %d exp per point of primal energy.\n\r", primal );
	   send_to_char( buf, ch );
	}

	sprintf( buf, "Natural resistances and immunities - %d exp each.\n\r", immcost );
	send_to_char( buf, ch );

	/* Weapon resistance affects */
	send_to_char( "Weapon resistances:", ch );
	if ( !IS_IMMUNE(ch, IMM_SLASH)  ) send_to_char( " Slash", ch );
	if ( !IS_IMMUNE(ch, IMM_STAB)   ) send_to_char( " Stab", ch );
	if ( !IS_IMMUNE(ch, IMM_SMASH)  ) send_to_char( " Smash", ch );
	if ( !IS_IMMUNE(ch, IMM_ANIMAL) ) send_to_char( " Beast", ch );
	if ( !IS_IMMUNE(ch, IMM_MISC)   ) send_to_char( " Grab", ch );
	if ( IS_IMMUNE(ch, IMM_SLASH) && IS_IMMUNE(ch, IMM_STAB) && 
	IS_IMMUNE(ch, IMM_SMASH) && IS_IMMUNE(ch, IMM_ANIMAL) && 
	IS_IMMUNE(ch, IMM_MISC) )
	   send_to_char( " None left to learn.\n\r", ch );
	else
	   send_to_char( ".\n\r", ch );

	/* Spell immunity affects */
	send_to_char( "Magical immunities:", ch );
	if ( !IS_IMMUNE(ch, IMM_CHARM)     ) send_to_char( " Charm", ch );
	if ( !IS_IMMUNE(ch, IMM_HEAT)      ) send_to_char( " Heat", ch );
	if ( !IS_IMMUNE(ch, IMM_COLD)      ) send_to_char( " Cold", ch );
	if ( !IS_IMMUNE(ch, IMM_LIGHTNING) ) send_to_char( " Lightning", ch );
	if ( !IS_IMMUNE(ch, IMM_ACID)      ) send_to_char( " Acid", ch );
	if ( !IS_IMMUNE(ch, IMM_SLEEP)     ) send_to_char( " Sleep", ch );
	if ( !IS_IMMUNE(ch, IMM_DRAIN)     ) send_to_char( " Drain", ch );
	if ( !IS_IMMUNE(ch, IMM_VOODOO)    ) send_to_char( " Voodoo", ch );
	if ( IS_IMMUNE(ch, IMM_CHARM) && IS_IMMUNE(ch, IMM_HEAT) && 
	IS_IMMUNE(ch, IMM_COLD) && IS_IMMUNE(ch, IMM_LIGHTNING) && 
	IS_IMMUNE(ch, IMM_ACID) && IS_IMMUNE(ch, IMM_SLEEP) &&
	IS_IMMUNE(ch, IMM_DRAIN) && IS_IMMUNE(ch, IMM_VOODOO) )
	   send_to_char( " None left to learn.\n\r", ch );
	else
	   send_to_char( ".\n\r", ch );

	/* Skill immunity affects */
	send_to_char( "Skill immunities:", ch );
	if ( !IS_IMMUNE(ch, IMM_HURL)     ) send_to_char( " Hurl", ch );
	if ( !IS_IMMUNE(ch, IMM_BACKSTAB) ) send_to_char( " Backstab", ch );
	if ( !IS_IMMUNE(ch, IMM_KICK)     ) send_to_char( " Kick", ch );
	if ( !IS_IMMUNE(ch, IMM_DISARM)   ) send_to_char( " Disarm", ch );
	if ( !IS_IMMUNE(ch, IMM_STEAL)    ) send_to_char( " Steal", ch );
	if ( IS_IMMUNE(ch, IMM_HURL) && IS_IMMUNE(ch, IMM_BACKSTAB) && 
	IS_IMMUNE(ch, IMM_KICK)      && IS_IMMUNE(ch, IMM_DISARM)   && 
	IS_IMMUNE(ch, IMM_STEAL) )
	   send_to_char( " None left to learn.\n\r", ch );
	else
	   send_to_char( ".\n\r", ch );
	return;
    }

    if ( (*pAbility >= 18) && (!str_cmp( arg1, "str")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= 18) && (!str_cmp( arg1, "int")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= 18) && (!str_cmp( arg1, "wis")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= 18) && (!str_cmp( arg1, "dex")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= 18) && (!str_cmp( arg1, "con")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= 30000) && (!str_cmp( arg1, "hp")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= 30000) && (!str_cmp( arg1, "mana")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= 30000) && (!str_cmp( arg1, "move")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }
    if ( (*pAbility >= 100) && (!str_cmp( arg1, "primal")))
    {
	if (last) act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }

    if ( cost > ch->exp )
    {
	if (last) send_to_char( "You don't have enough exp.\n\r", ch );
	return;
    }

    ch->exp		-= cost;
    *pAbility		+= 1;
    if ( !str_cmp( arg1, "avatar") )
    {
	act( "You become an avatar!", ch, NULL, NULL, TO_CHAR );
	sprintf( buf, "%s has become an avatar!\n\r",ch->name );
	do_info(ch,buf);
	if (!IS_NPC(ch) && IS_VAMPAFF(ch, VAM_MORTAL))
	    do_mortalvamp(ch,"");
    }
    else if (last)
	act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
    return;
}

void do_mount( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mount what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r",ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot ride on your own back!\n\r",ch );
	return;
    }

    if ( ch->mounted > 0 )
    {
	send_to_char( "You are already riding.\n\r",ch );
	return;
    }

    if ( !IS_NPC(victim) || victim->mounted > 0 || (IS_NPC(victim) && !IS_SET(victim->act, ACT_MOUNT) ) )
    {
	send_to_char( "You cannot mount them.\n\r",ch );
	return;
    }

    if ( victim->position < POS_STANDING )
    {
	if ( victim->position < POS_SLEEPING )
	    act( "$N is too badly hurt for that.", ch, NULL, victim, TO_CHAR );
	else if ( victim->position == POS_SLEEPING )
	    act( "First you better wake $m up.", ch, NULL, victim, TO_CHAR );
	else if ( victim->position == POS_RESTING )
	    act( "First $e better stand up.", ch, NULL, victim, TO_CHAR );
	else if ( victim->position == POS_SLEEPING )
	    act( "First you better wake $m up.", ch, NULL, victim, TO_CHAR );
	else if ( victim->position == POS_FIGHTING )
	    act( "Not while $e's fighting.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if (!IS_NPC(ch) && ch->stance[0] != -1) do_stance(ch,"");

    ch->mounted     = IS_RIDING;
    victim->mounted = IS_MOUNT;
    ch->mount = victim;
    victim->mount = ch;

    act( "You clamber onto $N's back.", ch, NULL, victim, TO_CHAR );
    act( "$n clambers onto $N's back.", ch, NULL, victim, TO_ROOM );
    return;
}

void do_dismount( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if ( ch->mounted == 0 )
    {
	send_to_char( "But you are not riding!\n\r",ch );
	return;
    }

    if ( (victim = ch->mount) == NULL )
    {
	send_to_char( "But you are not riding!\n\r",ch );
	return;
    }

    act( "You clamber off $N's back.", ch, NULL, victim, TO_CHAR );
    act( "$n clambers off $N's back.", ch, NULL, victim, TO_ROOM );

    ch->mounted     = IS_ON_FOOT;
    victim->mounted = IS_ON_FOOT;

    ch->mount = NULL;
    victim->mount = NULL;

    return;
}

void do_tie( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (victim == ch)
    {
	send_to_char( "You cannot tie yourself up!\n\r", ch );
	return;
    }
    if (IS_BODY(victim,TIED_UP))
    {
	send_to_char( "But they are already tied up!\n\r", ch );
	return;
    }
    if (victim->position > POS_STUNNED || victim->hit > 0)
    {
	send_to_char( "You can only tie up a defenceless person.\n\r", ch );
	return;
    }
    act("You quickly tie up $N.",ch,NULL,victim,TO_CHAR);
    act("$n quickly ties up $N.",ch,NULL,victim,TO_ROOM);
    send_to_char("You have been tied up!\n\r",victim);
    SET_BIT(victim->loc_hp[1],TIED_UP);
    return;
}

void do_untie( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (!IS_BODY(victim,TIED_UP))
    {
	send_to_char( "But they are not tied up!\n\r", ch );
	return;
    }
    if (victim == ch)
    {
	send_to_char( "You cannot untie yourself!\n\r", ch );
	return;
    }
    act("You quickly untie $N.",ch,NULL,victim,TO_CHAR);
    act("$n quickly unties $N.",ch,NULL,victim,TO_NOTVICT);
    act("$n quickly unties you.",ch,NULL,victim,TO_VICT);
    REMOVE_BIT(victim->loc_hp[1],TIED_UP);
    return;
}

void do_gag( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (victim == ch && !IS_BODY(victim,GAGGED) && IS_BODY(victim,TIED_UP))
    {
	send_to_char( "You cannot gag yourself!\n\r", ch );
	return;
    }
    if (!IS_BODY(victim,TIED_UP) && !IS_BODY(victim,GAGGED))
    {
	send_to_char( "You can only gag someone who is tied up!\n\r", ch );
	return;
    }
    if (!IS_BODY(victim,GAGGED))
    {
    	act("You place a gag over $N's mouth.",ch,NULL,victim,TO_CHAR);
    	act("$n places a gag over $N's mouth.",ch,NULL,victim,TO_NOTVICT);
    	act("$n places a gag over your mouth.",ch,NULL,victim,TO_VICT);
    	SET_BIT(victim->loc_hp[1],GAGGED);
	return;
    }
    if (ch == victim)
    {
    	act("You remove the gag from your mouth.",ch,NULL,victim,TO_CHAR);
    	act("$n removes the gag from $s mouth.",ch,NULL,victim,TO_ROOM);
    	REMOVE_BIT(victim->loc_hp[1],GAGGED);
	return;
    }
    act("You remove the gag from $N's mouth.",ch,NULL,victim,TO_CHAR);
    act("$n removes the gag from $N's mouth.",ch,NULL,victim,TO_NOTVICT);
    act("$n removes the gag from your mouth.",ch,NULL,victim,TO_VICT);
    REMOVE_BIT(victim->loc_hp[1],GAGGED);
    return;
}

void do_blindfold( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (victim == ch && !IS_BODY(victim,BLINDFOLDED) && IS_BODY(victim,TIED_UP))
    {
	send_to_char( "You cannot blindfold yourself!\n\r", ch );
	return;
    }
    if (!IS_BODY(victim,TIED_UP) && !IS_BODY(victim,BLINDFOLDED))
    {
	send_to_char( "You can only blindfold someone who is tied up!\n\r", ch );
	return;
    }
    if (!IS_BODY(victim,BLINDFOLDED))
    {
    	act("You place a blindfold over $N's eyes.",ch,NULL,victim,TO_CHAR);
    	act("$n places a blindfold over $N's eyes.",ch,NULL,victim,TO_NOTVICT);
    	act("$n places a blindfold over your eyes.",ch,NULL,victim,TO_VICT);
    	SET_BIT(victim->loc_hp[1],BLINDFOLDED);
	return;
    }
    if (ch == victim)
    {
    	act("You remove the blindfold from your eyes.",ch,NULL,victim,TO_CHAR);
    	act("$n removes the blindfold from $s eyes.",ch,NULL,victim,TO_ROOM);
    	REMOVE_BIT(victim->loc_hp[1],BLINDFOLDED);
	return;
    }
    act("You remove the blindfold from $N's eyes.",ch,NULL,victim,TO_CHAR);
    act("$n removes the blindfold from $N's eyes.",ch,NULL,victim,TO_NOTVICT);
    act("$n removes the blindfold from your eyes.",ch,NULL,victim,TO_VICT);
    REMOVE_BIT(victim->loc_hp[1],BLINDFOLDED);
    return;
}


int count_imms( CHAR_DATA *ch )
{
    int count = 0;

    if ( IS_IMMUNE(ch, IMM_SLASH)     ) count += 1;
    if ( IS_IMMUNE(ch, IMM_STAB)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_SMASH)     ) count += 1;
    if ( IS_IMMUNE(ch, IMM_ANIMAL)    ) count += 1;
    if ( IS_IMMUNE(ch, IMM_MISC)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_CHARM)     ) count += 1;
    if ( IS_IMMUNE(ch, IMM_HEAT)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_COLD)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_LIGHTNING) ) count += 1;
    if ( IS_IMMUNE(ch, IMM_ACID)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_SLEEP)     ) count += 1;
    if ( IS_IMMUNE(ch, IMM_DRAIN)     ) count += 1;
    if ( IS_IMMUNE(ch, IMM_VOODOO)    ) count += 1;
    if ( IS_IMMUNE(ch, IMM_HURL)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_BACKSTAB)  ) count += 1;
    if ( IS_IMMUNE(ch, IMM_KICK)      ) count += 1;
    if ( IS_IMMUNE(ch, IMM_DISARM)    ) count += 1;
    if ( IS_IMMUNE(ch, IMM_STEAL)     ) count += 1;
    return ( ( count * 10000 ) + 10000 );
}
