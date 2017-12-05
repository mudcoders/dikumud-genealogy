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



OBJ_FUN *spec_obj_lookup( const char *name )
{
    int cmd;

    for ( cmd = 0; *spec_obj_table[cmd].spec_name; cmd++ )
        if ( !str_cmp( name, spec_obj_table[cmd].spec_name ) )
            return spec_obj_table[cmd].spec_fun;

    return 0;
}


char *spec_obj_string( OBJ_FUN *fun )
{
    int cmd;

    for ( cmd = 0; *spec_obj_table[cmd].spec_fun; cmd++ )
        if ( fun == spec_obj_table[cmd].spec_fun )
            return spec_obj_table[cmd].spec_name;

    return 0;
}

/*
 * Object special function commands.
 */
const	struct	spec_obj_type	spec_obj_table	[ ]	=
{
    { "spec_giggle",		spec_giggle		},
    { "spec_soul_moan",		spec_soul_moan		},
    { "",			0			}
};  



/*
 * Special procedures for objects.
 */
bool spec_giggle( OBJ_DATA *obj, CHAR_DATA *keeper )
{
    if ( !keeper || !keeper->in_room )
	return FALSE;

    if ( number_percent( ) < 5 )
    {
	act( "$p carried by $n starts giggling to itself!",
	    keeper, obj, NULL, TO_ROOM );
	act( "$p carried by you starts giggling to itself!",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    return FALSE;
}


bool spec_soul_moan( OBJ_DATA *obj, CHAR_DATA *keeper )
{
    if ( !keeper || !keeper->in_room )
	return FALSE;

    if ( number_percent( ) < 2 )
    {
	act( "The soul in $p carried by $n moans in agony.",
	    keeper, obj, NULL, TO_ROOM );
	act( "The soul in $p carried by you moans to be set free!",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    if ( number_percent( ) < 2 )
    {
	act( "The soul in $p carried by $n tries to free itself!",
	    keeper, obj, NULL, TO_ROOM );
	act( "The soul in $p carried by you starts writhing!",
	    keeper, obj, NULL, TO_CHAR );
	return TRUE;
    }

    return FALSE;
}
