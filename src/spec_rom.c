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
 *  GreedMud 0.88 improvements copyright (C) 1997, 1998 by Vasco Costa.    *
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



ROOM_FUN *spec_room_lookup( const char *name )
{
    int cmd;

    for ( cmd = 0; *spec_room_table[cmd].spec_name; cmd++ )
        if ( !str_cmp( name, spec_room_table[cmd].spec_name ) )
            return spec_room_table[cmd].spec_fun;

    return 0;
}


char *spec_room_string( ROOM_FUN *fun )
{
    int cmd;

    for ( cmd = 0; *spec_room_table[cmd].spec_fun; cmd++ )
        if ( fun == spec_room_table[cmd].spec_fun )
            return spec_room_table[cmd].spec_name;

    return 0;
}

/*
 * Room special function commands.
 */
const	struct	spec_room_type	spec_room_table	[ ]	=
{
    { "spec_deathtrap",		spec_deathtrap		},
    { "",			0			}
};  



/*
 * Special procedures for rooms.
 */
bool spec_deathtrap( ROOM_INDEX_DATA *room )
{
    CHAR_DATA *rch;
    int        found;

    found = FALSE;
    for ( rch = room->people; rch; rch = rch->next_in_room )
    {
	found = TRUE;

	do_look( rch, "dt" );

	if ( rch->position == POS_STANDING && rch->hit > 20 )
	{
	    rch->position = POS_RESTING;
	    rch->hit /= 2;
	    send_to_char( "You better get out of here fast!\n\r", rch );
	}
	else
	{
	    raw_kill( rch, rch );
	    send_to_char( "You are dead.\n\r", rch );
	}
    }

    return found;
}
