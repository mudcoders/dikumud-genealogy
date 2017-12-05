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
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/*
 * Mythran Mud clan system
 * Version 0.1
 * Written by Maniac
 *
 * History:
 * july 17, 1996:	First version, features various clan commands, like
 *			apply, initiate, recruit, advance, kick, talk
 * july 18, 1996:	Added some minor commands, and fixed a few.
 *			Added clan donate (in act_obj.c)
 * july 23, 1996:	Added clan recall (in act_move.c), and crecall command
 * july 25, 1996:	Fixed some bugs with leader and advance
 *
 */

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

/* External functions */
void    talk_channel    args( ( CHAR_DATA *ch, char *argument,
                            int channel, const char *verb ) );


int clan_lookup(const char *name)
{
	int clan;

	for (clan = 0; clan_table[clan].vnum != 0 ; clan++)
	{
		if (LOWER(name[0]) == LOWER(clan_table[clan].name[0])
		    &&  !str_prefix(name,clan_table[clan].name))
			return clan_table[clan].vnum;
	}

	return CLAN_NONE;
}

char * clan_name( int clan )
{
	int	i;

	if (clan == CLAN_NONE)
		return (char *)"None";
	if (clan == CLAN_EXILE)
		return (char *)"Exile";

	for(i = 0; clan_table[i].vnum != 0; i++)
		if (clan_table[i].vnum == clan)
			return (char *)clan_table[clan].name;

	return "Error";
}

/* Is same clan... written by Maniac */
bool is_same_clan( CHAR_DATA * ch, CHAR_DATA * victim )
{
	if (IS_NPC(victim) )	/* No npc's... */
		return FALSE;
	if (IS_NPC(ch) )
		return FALSE;

	/* 2x clan none or exile still doesn't make it a clan... */
	if (ch->pcdata->clan <= CLAN_NONE)
		return FALSE;
	if (victim->pcdata->clan <= CLAN_NONE)
		return FALSE;

	if (victim->pcdata->clan == ch->pcdata->clan)
		return TRUE;
	return FALSE;
}

bool is_in_clan( CHAR_DATA *ch )
{
	if (IS_NPC(ch) )
		return FALSE;

	if (ch->pcdata->clanlevel < CLAN_LEVEL_1 )
		return FALSE;		/* Only full members */

	if (ch->pcdata->clan > CLAN_NONE )
		return TRUE;

	return FALSE;
}

bool is_clan_member( CHAR_DATA *ch, int vnum )
{
	if (ch->pcdata->clan == vnum )
		return TRUE;
	return FALSE;
}

void do_clantalk( CHAR_DATA *ch, char *argument )
{
	if (!is_in_clan(ch))
	{
		send_to_char ("You are not in a clan... so don't clantalk...\r\n", ch);
		return;
	}

	if ( IS_AFFECTED( ch, AFF_MUTE )
		|| IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	{
		send_to_char( "You can't seem to break the silence.\r\n", ch );
		return;
	}

	if (argument[0] == '\0' )
	{
		TOGGLE_BIT( ch->deaf, CHANNEL_CLAN );
		send_to_char ("Clan channel toggled.\r\n", ch );
		return;
	}

	talk_channel( ch, argument, CHANNEL_CLAN, "clantalk" );
	return;
}

void do_clanrecall( CHAR_DATA *ch, char *argument)
{
	do_recall(ch, "clan");
}

void do_clan( CHAR_DATA *ch, char *argument )
{
	char		buf[MAX_STRING_LENGTH];
	char		arg1[MAX_INPUT_LENGTH];
	char		arg2[MAX_INPUT_LENGTH];
	char		arg3[MAX_INPUT_LENGTH];
	CHAR_DATA *	victim;
	int		clan;
	int		level;

	if (IS_NPC( ch ) )
	{
		send_to_char ("You are not interested in clans.\r\n", ch);
		return;
	}

	argument = one_argument( argument, arg1 );

	if ( arg1[0] == '\0' )
	{
		send_to_char ("Syntax: clan [function] [parameters].\r\n", ch );
		send_to_char ("Functions: info, who, talk, apply, kick,\r\n", ch);
		send_to_char ("advance, recruit, initiate, leader, leave\r\n", ch );
		return;
	}

	if ( !str_prefix( arg1, "info" ) )
	{
		sprintf (buf, "You are member of clan %s (level %d).\r\n",
		    clan_name(ch->pcdata->clan),
		    (ch->pcdata->clanlevel - CLAN_INITIATE ) );
		send_to_char( buf, ch );
		return;
	}

	else if (!str_prefix( arg1, "talk" ) )
	{
		do_clantalk (ch, argument );
		return;
	}

	/* These are placed here so we don't have to paste 'em back
	together for clantalk...	-- Maniac --	*/
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( !str_prefix( arg1, "apply" ) )
	{
		if ( ch->pcdata->clan == CLAN_EXILE )
		{
    send_to_char ("You have already been in a clan, and have been exiled.\r\n", ch);
    send_to_char ("This means you can't join any clans anymore...\r\n", ch);
    send_to_char ("\t\tHave a nice day.\r\n", ch);
			return;
		}

		if ( is_in_clan (ch) )
		{
			send_to_char ("You are already in a clan...\r\n", ch);
			return;
		}

		if (arg2[0] == '\0' )
		{
			send_to_char ("Usage: clan apply <clanname>\r\n", ch);
			return;
		}

		if (( clan = clan_lookup(arg2)) <= CLAN_NONE )
		{
			send_to_char ("That is not a valid clan.\r\n", ch );
			return;
		}

		if ( ch->level < 10 )
		{
			send_to_char ("You have to be level 10 to join a clan.\r\n", ch );
			return;
		}

		if (!clan_accept(ch, clan ) )
		{
			send_to_char ("You do not have the right qualities the clan\r\n", ch );
			send_to_char ("Requires from you, you have not been accepted.\r\n", ch );
			return;
		}

		sprintf (buf, "note to %s", clan_name(clan) );
		interpret (ch, buf );
		interpret (ch, "note subject I want to join the clan");
		interpret (ch, "note + Please let me join...");
		interpret (ch, "note send" );

		send_to_char ("Your application has been accepted.\r\n", ch );
		send_to_char ("The clan leaders will discuss your membership at\r\n", ch);
		send_to_char ("next clan meeting, you will hear from us.\r\n", ch );
		ch->pcdata->clan = clan;
		ch->pcdata->clanlevel = CLAN_APPLICANT;
	} /* Apply */

	else if (!str_prefix( arg1, "initiate" ))
	{
		if ( !is_in_clan(ch) )
		{
			send_to_char ("You are not with a clan.\r\n", ch );
			return;
		}

		if ( ch->pcdata->clanlevel < CLAN_APPLICATOR )
		{
			send_to_char ("You have no power to initiate members.\r\n", ch );
			return;
		}

		if (arg2[0] == '\0' )
		{
			send_to_char ("Who do you wish to initiate.\r\n", ch );
			return;
		}

		if ((victim = get_char_world(ch, arg2 ) ) == NULL )
		{
			sprintf (buf, "You can't find %s.\r\n", arg2 );
			send_to_char(buf, ch );
			return;
		}

		if ( victim->pcdata->clanlevel != CLAN_APPLICANT )
		{
			sprintf (buf, "%s has not applied for a clan.\r\n",
			    victim->name);
			send_to_char(buf, ch );
			return;
		}

		if ( !is_same_clan (ch, victim) )
		{
			sprintf (buf, "%s hasn't applied for YOUR clan.\r\n",
			    victim->name );
			send_to_char(buf, ch );
			return;
		}

		victim->pcdata->clanlevel = CLAN_INITIATE;
		sprintf (buf, "Ok, %s has been accepted as applicant.\r\n", victim->name );
		send_to_char (buf, ch );
		send_to_char ("Your application has been acccepted, you are now a clan initiate.\r\n", victim);
		return;
	} /* initiate */

	else if (!str_prefix ( arg1, "advance" ) )
	{
		if ( !is_in_clan(ch) )
		{
			send_to_char ("You are not with a clan.\r\n", ch );
			return;
		}

		if ( ch->pcdata->clanlevel < CLAN_LEADER )
		{
			send_to_char ("You have no power to advance members.\r\n", ch );
			return;
		}

		if (arg2[0] == '\0' )
		{
			send_to_char ("Who do you wish to advance.\r\n", ch );
			return;
		}

		if ((victim = get_char_world(ch, arg2 ) ) == NULL )
		{
			sprintf (buf, "You can't find %s.\r\n", arg2 );
			send_to_char(buf, ch );
			return;
		}

		if ( !is_same_clan( ch, victim ) )
		{
			sprintf (buf, "%s isn't a member of YOUR clan.\r\n",
			    victim->name );
			send_to_char(buf, ch );
			return;
		}

		if (victim->pcdata->clanlevel >= ch->pcdata->clanlevel )
		{
			sprintf (buf, "You are not powerfull enough to advance %s.\r\n",
			    victim->name );
			send_to_char( buf, ch );
			return;
		}

		if (victim->pcdata->clanlevel >= CLAN_LEVEL_5 )
		{
			sprintf (buf, "%s is already at maximum clan level.\r\n",
				victim->name );
			send_to_char (buf, ch );
			return;
		}

		if (arg3[0] == '\0')
		{
			victim->pcdata->clanlevel++;
		}
		else
		{
			level = atoi(arg3);
			if ((level < 1 ) || (level > 5 ) )
			{
				send_to_char ("Levelrange is 1 to 5.\r\n", ch );
				return;
			}
			victim->pcdata->clanlevel = (CLAN_INITIATE + level);
		}
		sprintf (buf, "%s is now clanlevel %d.\r\n",
		    victim->name, (victim->pcdata->clanlevel - CLAN_INITIATE) );
		send_to_char (buf, ch );
		sprintf (buf, "You are now clanlevel %d.\r\n",
			(victim->pcdata->clanlevel - CLAN_INITIATE ) );
		send_to_char (buf, victim );
		return;
	} /* Advance */

        else if (!str_prefix( arg1, "recruit" ))
        {
                if ( !is_in_clan( ch ) )
                {
                        send_to_char ("You are not with a clan.\r\n", ch );
                        return;
                }

                if ( ch->pcdata->clanlevel < CLAN_RECRUITER )
                {
                        send_to_char ("You have no power to recruit members.\r\n", ch );
                        return;
                }

                if (arg2[0] == '\0' )
                {
                        send_to_char ("Who do you wish to recruit.\r\n", ch );
                        return;
                }

                if ((victim = get_char_world(ch, arg2 ) ) == NULL )
                {
                        sprintf (buf, "You can't find %s.\r\n", arg2 );
                        send_to_char(buf, ch );
                        return;
                }

                if ( victim->pcdata->clanlevel != CLAN_INITIATE )
                {
                        sprintf (buf, "%s has yet been initiated for the clan.\r\n",
                            victim->name);
                        send_to_char(buf, ch );
                        return;
                }

                if ( !is_same_clan( ch, victim ) )
                {
                        sprintf (buf, "%s isn't a member of YOUR clan.\r\n",
                            victim->name );
                        send_to_char(buf, ch );
                        return;
                }

                victim->pcdata->clanlevel++;
                sprintf (buf, "Ok, %s has been accepted as a full clan member.\r\n",
		    victim->name );
		send_to_char (buf, ch );
                send_to_char ("You are now a full clan member (level 1).\r\n", victim );
                return;
        } /* recruit */

	else if (!str_prefix ( arg1, "leader" ) )
	{
		if ( !is_in_clan(ch) )
		{
			send_to_char ("You are not with a clan.\r\n", ch );
			return;
		}

		if ( ch->pcdata->clanlevel < CLAN_LEADER )
		{
			send_to_char ("You have no power to make members leaders.\r\n", ch );
			return;
		}

		if (arg2[0] == '\0' )
		{
			send_to_char ("Who do you wish to advance to a leader.\r\n", ch );
			return;
		}

		if ((victim = get_char_world(ch, arg2 ) ) == NULL )
		{
			sprintf (buf, "You can't find %s.\r\n", arg2 );
			send_to_char(buf, ch );
			return;
		}

		if ( !is_same_clan( ch, victim ) )
		{
			sprintf (buf, "%s isn't a member of YOUR clan.\r\n",
			    victim->name );
			send_to_char(buf, ch );
			return;
		}

		if (victim->pcdata->clanlevel >= ch->pcdata->clanlevel )
		{
			sprintf (buf, "You are not powerfull enough to advance %s.\r\n",                    victim->name );
			send_to_char( buf, ch );
			return;
		}

		if ((IS_IMMORTAL(ch) && (ch->pcdata->clanlevel == CLAN_GOD )))
		{
			/* Remove leadership from a leader */
			if (victim->pcdata->clanlevel == CLAN_LEADER)
			{
				victim->pcdata->clanlevel--;
				send_to_char ("Ok... done.\r\n", ch );
			}
			/* Make someone a leader */
			else
			{
				victim->pcdata->clanlevel = CLAN_LEADER;
				send_to_char ("Ok... done.\r\n", ch );
				send_to_char ("You are now a clan leader.\r\n", victim );
			}
			return;
		}
		else if (ch->pcdata->clanlevel == CLAN_LEADER )
		{
			send_to_char ("Ok... you are no longer a leader.\r\n", ch );
			ch->pcdata->clanlevel = CLAN_LEVEL_5;
			if (victim->pcdata->clanlevel < CLAN_LEADER )
			{
				victim->pcdata->clanlevel = CLAN_LEADER;
				send_to_char ("You are now clan leader.\r\n", victim );
			}
			return;
		}
		else
		{
			send_to_char ("Something is wrong with leader, notify coders.\r\n", ch );
			return;
		}
	} /* Leader */

	else if (!str_cmp ( "leave", arg1 ) )
	{
		if (!is_in_clan(ch) )
		{
			send_to_char ("But you are not in a clan...\r\n", ch);
			return;
		}

		if (arg2[0] == '\0')
		{
			send_to_char ("Are you very sure you want to leave the clan ??\r\n", ch );
			send_to_char ("If so, type: clan leave, but with the string below as argument.\r\n", ch );
			sprintf ( buf, "%s\r\n", ch->pcdata->pwd );
			send_to_char (buf, ch);
			return;
		}
		else if (!str_cmp( arg2, ch->pcdata->pwd) )
		{
			send_to_char ("OK... you are now exiled.\r\n", ch );
			ch->pcdata->clan = CLAN_EXILE;
			ch->pcdata->clanlevel = CLAN_NOLEVEL;
			return;
		}
		else
		{
			send_to_char ("That was not the correct string.\r\n", ch );
			return;
		}
	}

	else if (!str_prefix (arg1, "kick" ) )
	{
		if ( !is_in_clan(ch) )
		{
			send_to_char ("You are not with a clan.\r\n", ch );
			return;
		}

		if ( ch->pcdata->clanlevel < CLAN_RECRUITER )
		{
			send_to_char ("You have no power to kick members.\r\n", ch );
			return;
		}

		if (arg2[0] == '\0' )
		{
			send_to_char ("Who do you wish to kick.\r\n", ch );
			return;
		}

		if ((victim = get_char_world(ch, arg2 ) ) == NULL )
		{
			sprintf (buf, "You can't find %s.\r\n", arg2 );
			send_to_char(buf, ch );
			return;
		}

		if ( !is_same_clan( ch, victim ) )
		{
			sprintf (buf, "%s isn't a member of YOUR clan.\r\n",
			    victim->name );
			send_to_char(buf, ch );
			return;
		}

		if (victim->pcdata->clanlevel >= ch->pcdata->clanlevel )
		{
			sprintf (buf, "You are not powerfull enough to kick %s.\r\n",                    victim->name );
			send_to_char( buf, ch );
			return;
		}

		/* Ok... do it... */
		victim->pcdata->clan = CLAN_EXILE;
		victim->pcdata->clanlevel = CLAN_NOLEVEL;
		send_to_char ("You have been kicked from the clan.\r\n", victim );
		send_to_char ("You are now exiled.\r\n", victim );
		sprintf (buf, "%s has been exiled.", victim->name );
		send_to_char (buf, ch );
		return;
	}

	else if (!str_prefix ( arg1, "who" ) )
	{
		return;
	}

	else
	{
		send_to_char ("Unknown option.\r\n", ch );
		return;
	}
}

void do_guild( CHAR_DATA *ch, char *argument )
{
	char		arg1[MAX_INPUT_LENGTH];
	char		arg2[MAX_INPUT_LENGTH];
	char		arg3[MAX_INPUT_LENGTH];
	char		buf[MAX_STRING_LENGTH];
	CHAR_DATA *	victim;
	int		clan;
	int		level;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( arg1[0] == '\0' )
	{
		send_to_char( "Syntax: guild <char> <cln name> <level>\r\n",ch);
		return;
	}

	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	{
		send_to_char( "They aren't playing.\r\n", ch );
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char( "Only on players.\r\n", ch );
		return;
	}

	if ( arg2[0] == '\0' )
	{
		sprintf(buf,"%s is member of clan %s (level %d).\r\n",
		victim->name, capitalize(clan_table[ch->pcdata->clan].name),
		(victim->pcdata->clanlevel - CLAN_INITIATE) );
                send_to_char(buf,ch);
		return;
	}

	if (!str_prefix(arg2,"none"))
	{
		sprintf (buf, "%s is now clanless.\r\n", victim->name);
		send_to_char(buf, ch);
		send_to_char("You are now a member of no clan!\r\n",victim);
		victim->pcdata->clan = CLAN_NONE;
		victim->pcdata->clanlevel = CLAN_NOLEVEL;
		return;
	}

	if (!str_prefix(arg2,"exile"))
	{
		sprintf (buf, "%s is now exiled.\r\n", victim->name);
		send_to_char(buf, ch);
		send_to_char("You are now exiled\r\n",victim);
		victim->pcdata->clan = CLAN_EXILE;
		victim->pcdata->clanlevel = CLAN_NOLEVEL;
		return;
	}

	if (arg3[0] == '\0' )
		level = CLAN_APPLICANT;
	else
		level = (CLAN_INITIATE + atoi(arg3));

	if ( ( level < CLAN_APPLICANT ) || ( level > CLAN_GOD ) )
	{
		send_to_char ("Level range is -1 to 9.\r\n", ch );
		send_to_char ("-1 = Applicant, 0 = initiate, 1-5 = level 1-5\r\n", ch);
		send_to_char ("6 = applicator, 7 = recruiter, 8 = leader 9 = god.\r\n", ch );
		return;
	}

	if ((clan = clan_lookup(arg2)) == 0)
	{
		send_to_char("No such clan exists.\r\n",ch);
		return;
	}
	else
	{
		sprintf(buf,"%s is now a member of clan %s (level %d).\r\n",
		    victim->name, capitalize(clan_table[clan].name),
		    (level - CLAN_INITIATE) );
		send_to_char(buf,ch);
		sprintf(buf,"You are now a member of clan %s (level %d).\r\n",
		capitalize(clan_table[clan].name),
		    (level - CLAN_INITIATE ) );
	}

	victim->pcdata->clan = clan;
	victim->pcdata->clanlevel = level;
}

int clan_accept(CHAR_DATA *ch, int clan )
{
	if (clan == CLAN_MYTHRAN)
	{
		if (ch->alignment < 0 )	/* Accept good chars only */
		{
			send_to_char ("You must be of good alignment to join clan Mythran.\r\n", ch );
			return FALSE;
		}
		if (ch->level < 15 )
		{
			send_to_char ("You must be level 15 to join clan Mythran.\r\n", ch );
			return FALSE;	/* Must be level 15 */
		}
		if (ch->gold < 50000 )
		{
			send_to_char ("You don't have enough money to join clan Mythran.\r\n", ch );
			return FALSE;	/* only wealthy players... */
		}
	}
	return TRUE;
}
