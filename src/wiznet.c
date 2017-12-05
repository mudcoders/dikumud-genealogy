/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael         *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        * 
 *  Mitchell Tse.                                                          *
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

/* Wiznet version 0.1
 *
 * Written by The Maniac from Mythran Mud
 * (c) 1996 The Maniac
 *
 * History:
 * 07/28/96	0.1	Initial version
 *
 */

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void do_wiznet( CHAR_DATA *ch, char *argument )
{
	char	arg[MAX_STRING_LENGTH];
	char	buf[MAX_STRING_LENGTH];

	buf[0] = '\0';

	one_argument(argument, arg);

	if (arg[0] == '\0' )
	{
		if (IS_SET(ch->pcdata->wiznet, WIZ_ON))
			strcat (buf, "Wiznet is on.\r\n");
		else
			strcat (buf, "Wiznet is off.\r\n");
		strcat (buf, (WCHAN(ch, WIZ_TICKS) ? "TICKS " : "ticks " ) );
		strcat (buf, (WCHAN(ch, WIZ_LOGINS) ? "LOGINS " : "logins " ) );
		strcat (buf, (WCHAN(ch, WIZ_DEATHS) ? "DEATHS " : "deaths " ) );
		strcat (buf, (WCHAN(ch, WIZ_RESETS) ? "RESETS " : "resets " ) );
		strcat (buf, (WCHAN(ch, WIZ_FLAGS) ? "FLAGS " : "flags " ) );
		strcat (buf, (WCHAN(ch, WIZ_SACCING) ? "SACCING " : "saccing " ) );
		strcat (buf, (WCHAN(ch, WIZ_LEVELS) ? "LEVELS " : "levels " ) );
		strcat (buf, (WCHAN(ch, WIZ_SECURE) ? "SECURE " : "secure " ) );
		strcat (buf, "\r\n" );		/* Some spacing */
		strcat (buf, (WCHAN(ch, WIZ_SWITCHES) ? "SWITCHES " : "switches " ) );
		strcat (buf, (WCHAN(ch, WIZ_SNOOPS) ? "SNOOPS " : "snoops " ) );
		strcat (buf, (WCHAN(ch, WIZ_RESTORE) ? "RESTORE " : "restore " ) );
		strcat (buf, (WCHAN(ch, WIZ_LOAD) ? "LOAD " : "load " ) );
		strcat (buf, (WCHAN(ch, WIZ_NEWBIE) ? "NEWBIE " : "newbie " ) );
		strcat (buf, (WCHAN(ch, WIZ_SPAM) ? "SPAM " : "spam " ) );
		strcat (buf, (WCHAN(ch, WIZ_CHAT) ? "CHAT " : "chat " ) );
		strcat (buf, (WCHAN(ch, WIZ_DEBUG) ? "DEBUG" : "debug" ) );
		strcat (buf, "\r\n" );		/* Finishing line */
		send_to_char (buf, ch );
		return;
	}

	if ( !str_cmp(argument, "on" ) )
	{
		SET_BIT(ch->pcdata->wiznet, WIZ_ON );
		send_to_char ("Welcome to Wiznet.\r\n", ch );
	}
	else if (!str_cmp (argument, "off" ) )
	{
		REMOVE_BIT(ch->pcdata->wiznet, WIZ_ON );
		send_to_char ("Wiznet is now off.\r\n", ch );
	}
	else
	{
		if (!str_infix ("tick", argument ) )
		{
			send_to_char ("Ticks toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_TICKS );
		}
		if (!str_infix ("login", argument ) )
		{
			send_to_char ("Logins toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_LOGINS );
		}
		if (!str_infix ("death", argument ) )
		{
			send_to_char ("Deaths toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_DEATHS );
		}
		if (!str_infix ("reset", argument ) )
		{
			send_to_char ("Resets toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_RESETS );
		}
		if (!str_infix ("flag", argument ) )
		{
			send_to_char ("Flags toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_FLAGS );
		}
		if (!str_infix ("sac", argument ) )
		{
			send_to_char ("Saccing toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_SACCING );
		}
		if (!str_infix ("level", argument ) )
		{
			send_to_char ("Levels toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_LEVELS );
		}
		if (!str_infix ("secure", argument ) )
		{
			send_to_char ("Secure toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_SECURE );
		}
		if (!str_infix ("switch", argument ) )
		{
			send_to_char ("Switches toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_SWITCHES );
		}
		if (!str_infix ("snoop", argument ) )
		{
			send_to_char ("Snoops toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_SNOOPS );
		}
		if (!str_infix ("resto", argument ) )
		{
			send_to_char ("Restores toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_RESTORE );
		}
		if (!str_infix ("load", argument ) )
		{
			send_to_char ("Loading toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_LOAD );
		}
		if (!str_infix ("newb", argument ) )
		{
			send_to_char ("Newbies toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_NEWBIE );
		}
		if (!str_infix ("spam", argument ) )
		{
			send_to_char ("Spamming toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_SPAM );
		}
		if (!str_infix ("chat", argument ) )
		{
			send_to_char ("Wiznet chat toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_CHAT );
		}
		if (!str_infix ("debug", argument ) )
		{
			send_to_char ("Wiznet debug info toggled\r\n", ch );
			TOGGLE_BIT( ch->pcdata->wiznet, WIZ_DEBUG );
		}
	}
	return;
}

void wiznet( CHAR_DATA *ch, int chan, int level, char * string )
{
	DESCRIPTOR_DATA *d;
	char	buf[MAX_STRING_LENGTH];

	buf[0] = '\0';

	strcat (buf, "[WIZNET] ");

	switch (chan)
	{
		case WIZ_TICKS:
			strcat (buf, "[TICK] ");
			break;
		case WIZ_DEATHS:
			strcat (buf, "[DEATH] ");
			break;
		case WIZ_RESETS:
			strcat (buf, "[RESET] ");
			break;
		case WIZ_FLAGS:
			strcat (buf, "[FLAG] ");
			break;
		case WIZ_SACCING:
			strcat (buf, "[SACCING] ");
			break;
		case WIZ_LEVELS:
			strcat (buf, "[LEVEL] ");
			break;
		case WIZ_SECURE:
			strcat (buf, "--> ");
			break;
		case WIZ_SWITCHES:
			strcat (buf, "[SWITCH] ");
			break;
		case WIZ_SNOOPS:
			strcat (buf, "[SNOOP] ");
			break;
		case WIZ_RESTORE:
			strcat (buf, "[RESTORE] ");
			break;
		case WIZ_LOAD:
			strcat (buf, "[LOAD] ");
			break;
		case WIZ_NEWBIE:
			strcat (buf, "[NEWBIE] ");
			break;
		case WIZ_SPAM:
			strcat (buf, "[SPAM] ");
			break;
		case WIZ_CHAT:
			strcat (buf, "[CHAT] ");
			break;
		case WIZ_DEBUG:
			strcat (buf, "[**DEBUG**] ");
			break;
	}
	strcat (buf, string );
	strcat (buf, "\r\n" );

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if (d->connected == CON_PLAYING		/* Playing ??? */
		&&  (get_trust(d->character) >= level)	/* Powerfull enuf */
		&& (d->character != ch )		/* Not the sender */
		&& (WCHAN(d->character, WIZ_ON ) )	/* Is wiznet on */
		&& (WCHAN(d->character, chan ) ) )	/* Is this chan on */
		{
			send_to_char (buf, d->character );
		}
	}
	return;
}
