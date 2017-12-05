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
 *  Wiznet 0.1 improvements copyright (C) 1996 by The Maniac.              *
 *  Written by The Maniac from Mythran Mud.                                *
 *                                                                         *
 *  History:                                                               *
 *  07/28/96     0.1     Initial version                                   *
 *  08/14/97     0.1a    Rewrote do_wiznet from scratch             - Zen  *
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

void do_wiznet( CHAR_DATA* ch, char *argument )
{
    char        arg[ MAX_STRING_LENGTH ];
    CHAR_DATA * rch;

    rch = get_char( ch );

    if ( !authorized( rch, "wiznet" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "[ Keyword  ] Option\n\r", ch );

	send_to_char( IS_SET( rch->pcdata->wiznet, WIZ_ON )
	    ? "[+WIZNET   ] Wiznet is on.\n\r"
	    : "[-wiznet   ] Wiznet is off.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_TICKS )
	    ? "[+TICKS    ] You see are notified of ticks.\n\r"
	    : "[-ticks    ] You aren't notified of ticks.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_LOGINS )
	    ? "[+LOGINS   ] You are notified of logins.\n\r"
	    : "[-logins   ] You aren't notified of logins.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_DEATHS )
	    ? "[+DEATHS   ] You are notified of deaths.\n\r"
	    : "[-deaths   ] You aren't notified of deaths.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_RESETS )
	    ? "[+RESETS   ] You are notified of area resets.\n\r"
	    : "[-resets   ] You aren't notified of area resets.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_FLAGS )
	    ? "[+FLAGS    ] You are notified of KILLER, THIEF & PK flags.\n\r"
	    : "[-flags    ] You aren't notified of KILLER, THIEF & PK flags.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_SACCING )
	    ? "[+SACCING  ] \n\r"
	    : "[-saccing  ] \n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_LEVELS )
	    ? "[+LEVELS   ] You are notified when a player levels.\n\r"
	    : "[-levels   ] You aren't notified when a player levels.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_SECURE )
	    ? "[+SECURE   ] You are notified of all logs.\n\r"
	    : "[-secure   ] You aren't notified of logs.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_SWITCHES )
	    ? "[+SWITCHES ] You are notified of switching immortals.\n\r"
	    : "[-switches ] You aren't notified of switching immortals.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_SNOOPS )
	    ? "[+SNOOPS   ] You are notified of other immortals snoops.\n\r"
	    : "[-snoops   ] You aren't notified of other immortals snoops.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_RESTORE )
	    ? "[+RESTORE  ] You are notified of all restores.\n\r"
	    : "[-restore  ] You aren't notified of restores.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_LOAD )
	    ? "[+LOAD     ] You are notified of all mloads & oloads.\n\r"
	    : "[-load     ] You aren't notified of mloads & oloads.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_NEWBIE )
	    ? "[+NEWBIE   ] You are notified of incoming newbies.\n\r"
	    : "[-newbie   ] You aren't notified of incoming newbies\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_SPAM )
	    ? "[+SPAM     ] You are notified of input spamming.\n\r"
	    : "[-spam     ] You aren't notified of input spamming.\n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_CHAT )
	    ? "[+CHAT     ] \n\r"
	    : "[-chat     ] \n\r"
	    , ch );

	send_to_char( WCHAN( rch, WIZ_DEBUG )
	    ? "[+DEBUG    ] You are notified of all bugs.\n\r"
	    : "[-debug    ] You aren't notified of bugs.\n\r"
	    , ch );
    }
    else
    {
	char buf [ MAX_STRING_LENGTH ];
	int  bit;
	bool fSet;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char( "Wiznet: config -option or +option?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "ticks"    ) ) bit = WIZ_TICKS;
	else if ( !str_cmp( arg+1, "logins"   ) ) bit = WIZ_LOGINS;
	else if ( !str_cmp( arg+1, "deaths"   ) ) bit = WIZ_DEATHS;
	else if ( !str_cmp( arg+1, "resets"   ) ) bit = WIZ_RESETS;
	else if ( !str_cmp( arg+1, "flags"    ) ) bit = WIZ_FLAGS;
	else if ( !str_cmp( arg+1, "saccing"  ) ) bit = WIZ_SACCING;
	else if ( !str_cmp( arg+1, "levels"   ) ) bit = WIZ_LEVELS;
	else if ( !str_cmp( arg+1, "secure"   ) ) bit = WIZ_SECURE;
	else if ( !str_cmp( arg+1, "switches" ) ) bit = WIZ_SWITCHES;
	else if ( !str_cmp( arg+1, "snoops"   ) ) bit = WIZ_SNOOPS;
	else if ( !str_cmp( arg+1, "restore"  ) ) bit = WIZ_RESTORE;
	else if ( !str_cmp( arg+1, "load"     ) ) bit = WIZ_LOAD;
	else if ( !str_cmp( arg+1, "newbie"   ) ) bit = WIZ_NEWBIE;
	else if ( !str_cmp( arg+1, "spam"     ) ) bit = WIZ_SPAM;
	else if ( !str_cmp( arg+1, "chat"     ) ) bit = WIZ_CHAT;
	else if ( !str_cmp( arg+1, "debug"    ) ) bit = WIZ_DEBUG;
	else if ( !str_cmp( arg+1, "wiznet"   ) ) bit = WIZ_ON;
	else
	{
	    send_to_char( "Wiznet: config which option?\n\r", ch );
	    return;
	}

	if ( fSet )
	{
	    SET_BIT    ( rch->pcdata->wiznet, bit );
	    sprintf( buf, "%s is now ON.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}
	else
	{
	    REMOVE_BIT ( rch->pcdata->wiznet, bit );
	    sprintf( buf, "%s is now OFF.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}

    }

    return;
}

void wiznet( CHAR_DATA *ch, int chan, int level, const char *string )
{
    DESCRIPTOR_DATA *d;
    char             buf[ MAX_STRING_LENGTH ];

    buf[ 0 ] = '\0';

    strcat( buf, "{o{c{B[WIZNET]{x {R{o{y" );

    switch ( chan )
    {
	default:
	    strcat( buf, "{x" );
	    break;
	case WIZ_TICKS:
	    strcat( buf, "[TICK]{x " );
	    break;
	case WIZ_DEATHS:
	    strcat( buf, "[DEATH]{x " );
	    break;
	case WIZ_RESETS:
	    strcat( buf, "[RESET]{x " );
	    break;
	case WIZ_FLAGS:
	    strcat( buf, "[FLAG]{x " );
	    break;
	case WIZ_SACCING:
	    strcat( buf, "[SACCING]{x " );
	    break;
	case WIZ_LEVELS:
	    strcat( buf, "[LEVEL]{x " );
	    break;
	case WIZ_SECURE:
	    strcat( buf, ">>{x " );
	    break;
	case WIZ_SWITCHES:
	    strcat( buf, "[SWITCH]{x " );
	    break;
	case WIZ_SNOOPS:
	    strcat( buf, "[SNOOP]{x " );
	    break;
	case WIZ_RESTORE:
	    strcat( buf, "[RESTORE]{x " );
	    break;
	case WIZ_LOAD:
	    strcat( buf, "[LOAD]{x " );
	    break;
	case WIZ_NEWBIE:
	    strcat( buf, "[NEWBIE]{x " );
	    break;
	case WIZ_SPAM:
	    strcat( buf, "[SPAM]{x " );
	    break;
	case WIZ_CHAT:
	    strcat( buf, "[CHAT]{x " );
	    break;
	case WIZ_DEBUG:
	    strcat( buf, "[**DEBUG**]{x " );
	    break;
    }
    strcat( buf, string );
    strcat( buf, "{x\n\r" );

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	    && !d->original
	    && IS_IMMORTAL( d->character )
	    && get_trust( d->character ) >= level
	    && WCHAN( d->character, chan )
	    && WCHAN( d->character, WIZ_ON )
	    && d->character != ch
	    && ( ch ? can_see( d->character, ch ) : TRUE ) )
	    send_to_char( buf, d->character );
    }
    return;
}
