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
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
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
#include "olc.h"



/*
 * Local functions.
 */
AREA_DATA *get_area_data args( ( int vnum ) );


char *olc_ed_name( CHAR_DATA *ch )
{
    static char buf [10];

    buf[0] = '\0';

    if ( !ch->desc )
	return &buf[0];

    switch ( ch->desc->connected )
    {
	case CON_AEDITOR:
	    sprintf( buf, "AEditor" );
	    break;
	case CON_REDITOR:
	    sprintf( buf, "REditor" );
	    break;
	case CON_OEDITOR:
	    sprintf( buf, "OEditor" );
	    break;
	case CON_MEDITOR:
	    sprintf( buf, "MEditor" );
	    break;
	case CON_MPEDITOR:
	    sprintf( buf, "MPEditor" );
	    break;
	default:
	    sprintf( buf, " " );
	    break;
    }
    return buf;
}


char *olc_ed_vnum( CHAR_DATA *ch )
{
    AREA_DATA       *pArea;
    ROOM_INDEX_DATA *pRoom;
    OBJ_INDEX_DATA  *pObj;
    MOB_INDEX_DATA  *pMob;
    static char      buf [10];

    buf[0] = '\0';

    if ( !ch->desc )
	return &buf[0];

    switch ( ch->desc->connected )
    {
	case CON_AEDITOR:
	    pArea = (AREA_DATA *) ch->desc->olc_editing;
	    sprintf( buf, "%d", pArea ? pArea->vnum : 0 );
	    break;
	case CON_REDITOR:
	    pRoom = ch->in_room;
	    sprintf( buf, "%d", pRoom ? pRoom->vnum : 0 );
	    break;
	case CON_OEDITOR:
	    pObj = (OBJ_INDEX_DATA *) ch->desc->olc_editing;
	    sprintf( buf, "%d", pObj ? pObj->vnum : 0 );
	    break;
	case CON_MEDITOR:
	    pMob = (MOB_INDEX_DATA *) ch->desc->olc_editing;
	    sprintf( buf, "%d", pMob ? pMob->vnum : 0 );
	    break;
	case CON_MPEDITOR:
	    pMob = (MOB_INDEX_DATA *) ch->desc->olc_editing;
	    sprintf( buf, "%d(%d)", pMob ? pMob->vnum : 0,
		    ch->pcdata->mprog_edit + 1 );
	    break;
	default:
	    sprintf( buf, " " );
	    break;
    }

    return buf;
}


AREA_DATA *get_area_data( int vnum )
{
    AREA_DATA *pArea;

    for ( pArea = area_first; pArea; pArea = pArea->next )
	if ( pArea->vnum == vnum )
	    return pArea;

    return 0;
}



void do_aedit( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    AREA_DATA *pArea;
    int        value;

    rch = get_char( ch );
    
    if ( !authorized( rch, "aedit" ) )
        return;

    pArea = ch->in_room->area;

    if ( is_number( argument ) )
    {
        value = atoi( argument );
        if ( !( pArea = get_area_data( value ) ) )
        {
            send_to_char( "That area vnum does not exist.\n\r", ch );
            return;
        }
    }
    else
    {
        if ( !str_cmp( argument, "reset" ) )
        {
            reset_area( pArea );
            send_to_char( "Area reset.\n\r", ch );
            return;
        }
        else
        if ( !str_cmp( argument, "create" ) )
	    aedit_create( ch, "" );
    }

    ch->desc->olc_editing = ( void * ) pArea;
    ch->desc->connected = CON_AEDITOR;
    return;
}


void do_redit( CHAR_DATA* ch, char *argument )
{
    CHAR_DATA       *rch;
    ROOM_INDEX_DATA *pRoom;
    int              value;
    char             arg1 [MAX_STRING_LENGTH];

    rch = get_char( ch );
    
    if ( !authorized( rch, "redit" ) )
        return;

    argument = one_argument( argument, arg1 );

    pRoom = ch->in_room;

    if ( is_number( arg1 ) )
    {
        value = atoi( arg1 );
        if ( !( pRoom = get_room_index( value ) ) )
        {
            send_to_char( "That vnum does not exist.\n\r", ch );
            return;
        }
    }
    else
    {
        if ( !str_cmp( arg1, "reset" ) )
        {
            reset_room( pRoom );
            send_to_char( "Room reset.\n\r", ch );
            return;
        }
        else
        if ( !str_cmp( arg1, "create" ) )
	    if ( redit_create( ch, argument ) )
	    {
		char_from_room( ch );
		char_to_room( ch, pRoom );
	    }
    }

    ch->desc->connected = CON_REDITOR;
    return;
}


void do_oedit( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA      *rch;
    OBJ_INDEX_DATA *pObj;
    char            arg1 [MAX_STRING_LENGTH];

    rch = get_char( ch );
    
    if ( !authorized( rch, "oedit" ) )
        return;

    argument = one_argument( argument, arg1 );

    if ( is_number( arg1 ) )
    {
	if ( !( pObj = get_obj_index( atoi( arg1 ) ) ) )
	{
	    send_to_char( "That vnum does not exist.\n\r", ch );
	    return;
	}

	ch->desc->olc_editing = ( void * ) pObj;
	ch->desc->connected = CON_OEDITOR;
	return;
    }
    else
    {
        if ( !str_cmp( arg1, "create" ) )
        {
	    if ( oedit_create( ch, argument ) )
		ch->desc->connected = CON_OEDITOR;
            return;
        }
    }

    send_to_char( "Syntax: oedit <vnum>.\n\r", ch );
    return;
}


void do_medit( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA      *rch;
    MOB_INDEX_DATA *pMob;
    char            arg1 [MAX_STRING_LENGTH];

    rch = get_char( ch );
    
    if ( !authorized( rch, "medit" ) )
        return;

    argument = one_argument( argument, arg1 );

    if ( is_number( arg1 ) )
    {
	if ( !( pMob = get_mob_index( atoi( arg1 ) ) ) )
	{
	    send_to_char( "That vnum does not exist.\n\r", ch );
	    return;
	}

	ch->desc->olc_editing	= (void *) pMob;
	ch->desc->connected	= CON_MEDITOR;
	return;
    }
    else
    {
        if ( !str_cmp( arg1, "create" ) )
        {
	    if ( medit_create( ch, argument ) )
		ch->desc->connected = CON_MEDITOR;
            return;
        }
    }

    send_to_char( "Syntax: medit <vnum>.\n\r", ch );
    return;
}


void do_mpedit( CHAR_DATA * ch, char *argument )
{
    AREA_DATA      *pArea;
    MOB_INDEX_DATA *pMob;
    MPROG_DATA     *mprg;
    MPROG_DATA     *cprg;
    int             value;
    char            arg1 [MAX_STRING_LENGTH];
    char            arg2 [MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ch->in_room &&
	!is_builder( ch, ch->in_room->area ) )
    {
	send_to_char( "Insufficient security to edit mobprog.\n\r", ch );
	return;
    }

    if ( is_number( arg1 ) )
    {
	value = atoi( arg1 );
	if ( !( pMob = get_mob_index( value ) ) )
	{
	    send_to_char( "That vnum does not exist.\n\r", ch );
	    return;
	}

	if ( arg2[0] == '\0' || atoi( arg2 ) == 0 )
	{
	    if ( !pMob->mobprogs )
	    {
		send_to_char( "Mobile has no mobprogs.", ch );
		send_to_char( "  Use create.\n\r", ch );
		return;
	    }
	    else
	    {
		send_to_char( "Editing first mobprog.\n\r", ch );
		value = 1;
	    }
	}
	else if ( ( value = atoi( arg2 ) ) > mprog_count( pMob ) )
	{
	    send_to_char( "Mobile does not have that many mobprogs.\n\r", ch );
	    return;
	}

	ch->desc->olc_editing = ( void * ) pMob;
	ch->pcdata->mprog_edit = value - 1;
	ch->desc->connected = CON_MPEDITOR;
	return;
    }
    else
    {
	if ( !str_cmp( arg1, "create" ) )
	{
	    value = atoi( arg2 );
	    if ( arg2[0] == '\0' || value == 0 )
	    {
		send_to_char( "Syntax:  edit mobprog create vnum [svnum]\n\r",
			     ch );
		return;
	    }

	    pArea = get_vnum_area( value );

	    if ( mpedit_create( ch, arg2 ) )
		ch->desc->connected = CON_MPEDITOR;
	    else
		return;

	    if ( is_number( argument ) )
	    {
		MOB_INDEX_DATA *cMob, *pMob;

		if ( !( pMob = get_mob_index( value ) ) )
		{
		    send_to_char( "No destination mob exists.\n\r", ch );
		    return;
		}

		value = atoi( argument );
		if ( !( cMob = get_mob_index( value ) ) )
		{
		    send_to_char( "No such source mob exists.\n\r", ch );
		    return;
		}

		mprg = pMob->mobprogs;

		for ( cprg = cMob->mobprogs; cprg;
		     cprg = cprg->next, mprg = mprg->next )
		{
		    mprg->type = cprg->type;
		    mprg->arglist = str_dup( cprg->arglist );
		    mprg->comlist = str_dup( cprg->comlist );
		    if ( cprg->next )
			mprg->next = ( MPROG_DATA * ) alloc_perm( sizeof( 
							  MPROG_DATA ) );
		    else
			mprg->next = NULL;
		}

		send_to_char( "MOBProg copied.\n\r", ch );
	    }
	    return;
	}
    }

    send_to_char( "Syntax: mpedit <vnum>.\n\r", ch );
    return;
}


void display_resets( CHAR_DATA * ch )
{
    ROOM_INDEX_DATA *pRoom;
    MOB_INDEX_DATA  *pMob = NULL;
    RESET_DATA      *pReset;
    char             final	[MAX_STRING_LENGTH];
    char             buf	[MAX_STRING_LENGTH];
    int              iReset = 0;

    if ( ch->desc->connected != CON_REDITOR
	|| !( pRoom = (ROOM_INDEX_DATA *) ch->desc->olc_editing ) )
	pRoom = ch->in_room;

    final[0] = '\0';

    send_to_char( "{o{c----------------------------------------------------------------------------{x\n\r", ch );
    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
    {
	OBJ_INDEX_DATA  *pObj;
	MOB_INDEX_DATA  *pMobIndex;
	OBJ_INDEX_DATA  *pObjIndex;
	OBJ_INDEX_DATA  *pObjToIndex;
	ROOM_INDEX_DATA *pRoomIndex;

	final[0] = '\0';
	sprintf( final, "{o{c%2d: {x", ++iReset );

	switch ( pReset->command )
	{
	    default:
		sprintf( buf, "Bad reset command: %c.", pReset->command );
		strcat( final, buf );
		break;

	    case 'M':
		if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
		{
		    sprintf( buf, "Load mobile - bad mob %d.\n\r", pReset->arg1 );
		    strcat( final, buf );
		    continue;
		}

		if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
		{
		    sprintf( buf, "Load mobile - bad room %d.\n\r", pReset->arg3 );
		    strcat( final, buf );
		    continue;
		}

		pMob = pMobIndex;
		sprintf( buf, "{o{rM 0 %5d %2d %5d  * %s -> %s{x\n\r",
			pReset->arg1, pReset->arg2, pReset->arg3,
			pMob->short_descr, pRoomIndex->name );
		strcat( final, buf );

		/* check for pet shop */
		{
		    ROOM_INDEX_DATA *pRoomIndexPrev;

		    pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
		    if ( pRoomIndexPrev
			&& IS_SET( pRoomIndexPrev->orig_room_flags, ROOM_PET_SHOP ) )
			final[14] = 'P';
		}

		break;

	    case 'O':
		if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
		{
		    sprintf( buf, "Load object - bad object %d.\n\r",
			    pReset->arg1 );
		    strcat( final, buf );
		    continue;
		}

		pObj = pObjIndex;

		if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
		{
		    sprintf( buf, "Load object - bad room %d.\n\r", pReset->arg3 );
		    strcat( final, buf );
		    continue;
		}

		sprintf( buf, "{o{gO 0 %5d  0 %5d  * %s -> %s{x\n",
			pReset->arg1,
			pReset->arg3,
			capitalize( pObj->short_descr ),
			pRoomIndex->name );
		strcat( final, buf );

		break;

	    case 'P':
		if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
		{
		    sprintf( buf, "Put object - bad object %d.\n\r",
			    pReset->arg1 );
		    strcat( final, buf );
		    continue;
		}

		pObj = pObjIndex;

		if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
		{
		    sprintf( buf, "Put object - bad to object %d.\n\r",
			    pReset->arg3 );
		    strcat( final, buf );
		    continue;
		}

		sprintf( buf,"{gP 0 %5d  0 %5d  * %s inside %s{x\n",
			pReset->arg1,
			pReset->arg3,
			capitalize( get_obj_index( pReset->arg1 )->short_descr ),
			pObjToIndex->short_descr );
		strcat( final, buf );

		break;

	    case 'G':
		if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
		{
		    sprintf( buf, "Give object - bad object %d.\n\r",
			    pReset->arg1 );
		    strcat( final, buf );
		    continue;
		}

		pObj = pObjIndex;

		if ( !pMob )
		{
		    sprintf( buf, "Give object - no previous mobile.\n\r" );
		    strcat( final, buf );
		    break;
		}

		    sprintf( buf, "{rG 0 %5d  0        *   %s{x\n",
			    pReset->arg1,
			    capitalize( pObjIndex->short_descr ) );
		strcat( final, buf );

		break;

	    case 'E':
		if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
		{
		    sprintf( buf, "Equip object - bad object %d.\n\r",
			    pReset->arg1 );
		    strcat( final, buf );
		    continue;
		}

		pObj = pObjIndex;

		if ( !pMob )
		{
		    sprintf( buf, "Equip object - no previous mobile.\n\r" );
		    strcat( final, buf );
		    break;
		}

		    sprintf( buf, "{rE 0 %5d  0 %5d  *   %s %s{x\n",
			    pReset->arg1,
			    pReset->arg3,
		    capitalize( get_obj_index( pReset->arg1 )->short_descr ),
		    flag_string( wear_loc_strings, pReset->arg3 ) );
		strcat( final, buf );

		break;

	    case 'R':
		if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
		{
		    sprintf( buf, "Randomize exits - bad room %d.\n\r",
			    pReset->arg1 );
		    strcat( final, buf );
		    continue;
		}

		sprintf( buf, "{o{yR 0 %5d %2d        * Randomize %s{x\n",
			pReset->arg1,
			pReset->arg2,
			pRoomIndex->name );
		strcat( final, buf );

		break;
	}
	send_to_char( final, ch );
    }

    return;
}


void add_reset( ROOM_INDEX_DATA * room, RESET_DATA * pReset, int index )
{
    RESET_DATA *reset;
    int         iReset = 0;

    if ( !room->reset_first )
    {
	room->reset_first	= pReset;
	room->reset_last	= pReset;
	pReset->next		= NULL;
	return;
    }

    index--;

    if ( index == 0 )
    {
	pReset->next		= room->reset_first;
	room->reset_first	= pReset;
	return;
    }

    for ( reset = room->reset_first; reset->next; reset = reset->next )
    {
	if ( ++iReset == index )
	    break;
    }

    pReset->next		= reset->next;
    reset->next			= pReset;
    if ( !pReset->next )
	room->reset_last	= pReset;
    return;
}


void do_resets( CHAR_DATA * ch, char *argument )
{
    ROOM_INDEX_DATA  *pRoom;
    AREA_DATA        *pArea;
    RESET_DATA       *pReset;
    char              arg1 [ MAX_INPUT_LENGTH ];
    char              arg2 [ MAX_INPUT_LENGTH ];
    char              arg3 [ MAX_INPUT_LENGTH ];
    char              arg4 [ MAX_INPUT_LENGTH ];
    char              arg5 [ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    argument = one_argument( argument, arg5 );

    pReset = NULL;
    pRoom  = ch->in_room;
    pArea  = pRoom->area;

    if ( !is_builder( ch, pArea ) )
    {
	send_to_char( "Invalid security for editing this area.\n\r", ch );
	return;
    }


    if ( arg1[0] == '\0' )
    {
	if ( pRoom->reset_first )
	{
	    send_to_char( "{c{oResets: M = mobile, R = room, ",        ch );
	    send_to_char( "O = object, P = pet, S = shopkeeper{x\n\r", ch );
	    display_resets( ch );
	}
	else
	    send_to_char( "There are no resets in this room.\n\r", ch );

	return;
    }


    if ( !is_number( arg1 ) )
    {
	send_to_char( "Syntax: RESET <number> OBJ <vnum> <wearloc>\n\r", ch );
	send_to_char( "        RESET <number> OBJ <vnum> in <vnum>\n\r", ch );
	send_to_char( "        RESET <number> OBJ <vnum> room     \n\r", ch );
	send_to_char( "        RESET <number> MOB <vnum> [<max #>]\n\r", ch );
	send_to_char( "        RESET <number> DELETE              \n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "delete" ) )
    {
	int insert_loc = atoi( arg1 );

	if ( !pRoom->reset_first )
	{
	    send_to_char( "There are no resets in this area.\n\r", ch );
	    return;
	}

	if ( insert_loc - 1 <= 0 )
	{
	    pReset = pRoom->reset_first;
	    pRoom->reset_first = pRoom->reset_first->next;
	    if ( !pRoom->reset_first )
		pRoom->reset_last = NULL;
	}
	else
	{
	    RESET_DATA *prev   = NULL;
	    int         iReset = 0;

	    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
	    {
		if ( ++iReset == insert_loc )
		    break;
		prev = pReset;
	    }

	    if ( !pReset )
	    {
		send_to_char( "Reset not found.\n\r", ch );
		return;
	    }

	    if ( prev )
		prev->next = prev->next->next;
	    else
		pRoom->reset_first = pRoom->reset_first->next;

	    for ( pRoom->reset_last = pRoom->reset_first;
		 pRoom->reset_last->next;
		 pRoom->reset_last = pRoom->reset_last->next );
	}

	free_reset_data( pReset );
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Reset deleted.\n\r", ch );
	return;
    }

    if ( ( !str_cmp( arg2, "mob" ) || !str_cmp( arg2, "obj" ) )
	&& is_number( arg3 ) )
    {
	if ( !str_cmp( arg2, "mob" ) )
	{
	    pReset          = new_reset_data( );
	    pReset->command = 'M';
	    pReset->arg1    = atoi( arg3 );
	    pReset->arg2    = is_number( arg4 ) ? atoi( arg4 ) : 1;
	    pReset->arg3    = pRoom->vnum;
	}
	else
	if ( !str_cmp( arg2, "obj" ) )
	{
	    pReset       = new_reset_data( );
	    pReset->arg1 = atoi( arg3 );

	    if ( !str_prefix( arg4, "inside" ) )
	    {
		pReset->command = 'P';
		pReset->arg2    = 0;
		pReset->arg3    = is_number( arg5 ) ? atoi( arg5 ) : 1;
	    }
	    else
	    if ( !str_cmp( arg4, "room" ) )
	    {
		pReset->command = 'O';
		pReset->arg1    = atoi( arg3 );
		pReset->arg2    = 0;
		pReset->arg3    = pRoom->vnum;
	    }
	    else
	    {
		if ( flag_value( wear_loc_flags, arg4 ) == NO_FLAG )
		{
		    send_to_char( "Resets: '? wear-loc'\n\r", ch );
		    return;
		}

		pReset->arg3 = flag_value( wear_loc_flags, arg4 );
		if ( pReset->arg2 == WEAR_NONE )
		    pReset->command = 'G';
		else
		    pReset->command = 'E';
	    }
	}

	add_reset( pRoom, pReset, atoi( arg1 ) );
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Reset added.\n\r", ch );
    }

    return;
}


void do_alist( CHAR_DATA * ch, char *argument )
{
    AREA_DATA *pArea;
    char       buf  [MAX_STRING_LENGTH];
    char       buf1 [MAX_STRING_LENGTH*2];

    buf1[0] = '\0';

    sprintf( buf, "{c[%3s] [%-27s] [%-5s/%5s] [%-10s] %3s [%-10s]{x\n\r",
    "Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders" );
    strcat( buf1, buf );

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	sprintf( buf, "{c[%3d] {g%-29.29s {c[{x%5d{c/{x%-5d{c] {c%-12.12s [{x%d{c] [{x%-10.10s{c]{x\n\r",
		pArea->vnum,
		&pArea->name[8],
		pArea->lvnum,
		pArea->uvnum,
		pArea->filename,
		pArea->security,
		pArea->builders );
	strcat( buf1, buf );
    }
    send_to_char( buf1, ch );

    return;
}
