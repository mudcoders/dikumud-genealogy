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
 * Version 0.2
 * Written by Maniac
 *
 * History:
 * july 17, 1996:	First version, features various clan commands, like
 *			apply, initiate, recruit, advance, kick, talk
 * july 18, 1996:	Added some minor commands, and fixed a few.
 *			Added clan donate (in act_obj.c)
 * july 23, 1996:	Added clan recall (in act_move.c), and crecall command
 * july 25, 1996:	Fixed some bugs with leader and advance
 * october 21, 1996:	Improved on info, if char exiled or in no clan...
 * october 22, 1996:	Restructured clan none and exile, to fix a bug.
 *			Added clan list command (mainly for debugging)
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

	for (clan = 1; clan_table[clan].vnum != 0 ; clan++)
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

	for(i = 1; clan_table[i].vnum != 0; i++)
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
	if (ch->pcdata->clan == CLAN_NONE)
		return FALSE;
	if (victim->pcdata->clan == CLAN_NONE)
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
		send_to_char ("You are not in a clan... so don't clantalk...\n\r", ch);
		return;
	}

	if ( IS_AFFECTED( ch, AFF_MUTE )
		|| IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	{
		send_to_char( "You can't seem to break the silence.\n\r", ch );
		return;
	}

	if (argument[0] == '\0' )
	{
		TOGGLE_BIT( ch->deaf, CHANNEL_CLAN );
		send_to_char ("Clan channel toggled.\n\r", ch );
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
		send_to_char ("You are not interested in clans.\n\r", ch);
		return;
	}

	argument = one_argument( argument, arg1 );

	if ( arg1[0] == '\0' )
	{
		send_to_char( "Syntax: clan [function] [parameters].\n\r", ch );
		send_to_char( "Functions: info, list, talk, apply, kick,\n\r", ch);
		send_to_char( "advance, recruit, initiate, leader, leave\n\r", ch );
		send_to_char( "where, who\n\r", ch );
		return;
	}

	if (!str_prefix( arg1, "talk" ) )
	{
		do_clantalk (ch, argument );
		return;
	}

	/* These are placed here so we don't have to paste 'em back
	together for clantalk...	-- Maniac --	*/
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( !str_prefix( arg1, "info" ) )
	{
		if (ch->pcdata->clanlevel == CLAN_EXILE)
		{
			send_to_char ("You have been exiled from your clan.\n\r", ch );
			return;
		}
		if (ch->pcdata->clan == CLAN_NONE)
		{
			send_to_char ("You are not in a clan.\n\r", ch );
			return;
		}
		if ( arg2[0] == '\0' )
		{	/* No argument, so info about ch */
		    sprintf (buf, "You are member of clan %s (level %d).\n\r",
			clan_name(ch->pcdata->clan),
			(ch->pcdata->clanlevel - CLAN_INITIATE ) );
		    send_to_char( buf, ch );
		    switch( ch->pcdata->clanlevel )
		    {
			default:
			    break;
			case CLAN_GOD:
			    send_to_char( "You are the clan God.\n\r", ch );
			    break;
			case CLAN_LEADER:
			    send_to_char( "You are the clan leader.\n\r", ch );
			    break;
			case CLAN_RECRUITER:
			    send_to_char( "You are the clan recruiter.\n\r", ch );
			    break;
			case CLAN_APPLICATOR:
			    send_to_char( "You are the clan applicater.\n\r", ch );
			    break;
		    }
		    return;
		}
		else
		{	/* argument, so info about victim */
		    if ( !(victim = get_char_world( ch, arg2 ) ) || ( IS_NPC( victim ) ) )
		    {
			sprintf( buf, "You can't find %s.\n\r", arg2 );
			send_to_char( buf, ch );
			return;
		    }
		    if ( ( !is_same_clan( ch, victim ) ) && ( ch->pcdata->clanlevel == CLAN_GOD ) )
		    {	/* Not same clan, but CLAN_GOD can see anyways */
			sprintf( buf, "%s is a member of clan %s (level %d).\n\r",
			    victim->name, clan_name( victim->pcdata->clan ),
			    (victim->pcdata->clanlevel - CLAN_INITIATE ) );
			send_to_char( buf, ch );
			return;
		    }
		    if ( ( is_same_clan( ch, victim ) ) && ( ch->pcdata->clanlevel >= CLAN_LEADER ) )
		    {	/* Same clan, and char is at least CLAN_LEADER */
			sprintf( buf,"%s is a member of clan %s (level %d ).\n\r",
			    victim->name,clan_name( victim->pcdata->clan ),
			    (victim->pcdata->clanlevel - CLAN_INITIATE ) );
			send_to_char( buf, ch );
			return;
		    }
		    send_to_char( "You cannot do that.\n\r", ch );
		    return;
		}
	}


	if ( !str_prefix( arg1, "apply" ) )
	{
		if ( ch->pcdata->clanlevel == CLAN_EXILE )
		{
    send_to_char ("You have already been in a clan, and have been exiled.\n\r", ch);
    send_to_char ("This means you can't join any clans anymore...\n\r", ch);
    send_to_char ("\t\tHave a nice day.\n\r", ch);
			return;
		}

		if ( is_in_clan (ch) )
		{
			send_to_char ("You are already in a clan...\n\r", ch);
			return;
		}

		if (arg2[0] == '\0' )
		{
			send_to_char ("Usage: clan apply <clanname>\n\r", ch);
			return;
		}

		if (( clan = clan_lookup(arg2)) <= CLAN_NONE )
		{
			send_to_char ("That is not a valid clan.\n\r", ch );
			return;
		}

		if ( ch->level < 10 )
		{
			send_to_char ("You have to be level 10 to join a clan.\n\r", ch );
			return;
		}

		if (!clan_accept(ch, clan ) )
		{
			send_to_char ("You do not have the right qualities the clan\n\r", ch );
			send_to_char ("Requires from you, you have not been accepted.\n\r", ch );
			return;
		}

		sprintf (buf, "note to %s", clan_name(clan) );
		interpret (ch, buf );
		interpret (ch, "note subject I want to join the clan");
		interpret (ch, "note + Please let me join...");
		interpret (ch, "note send" );

		send_to_char ("Your application has been accepted.\n\r", ch );
		send_to_char ("The clan leaders will discuss your membership at\n\r", ch);
		send_to_char ("next clan meeting, you will hear from us.\n\r", ch );
		ch->pcdata->clan = clan;
		ch->pcdata->clanlevel = CLAN_APPLICANT;
	} /* Apply */

	else if (!str_prefix( arg1, "initiate" ))
	{
		if ( !is_in_clan(ch) )
		{
			send_to_char ("You are not with a clan.\n\r", ch );
			return;
		}

		if ( ch->pcdata->clanlevel < CLAN_APPLICATOR )
		{
			send_to_char ("You have no power to initiate members.\n\r", ch );
			return;
		}

		if (arg2[0] == '\0' )
		{
			send_to_char ("Who do you wish to initiate.\n\r", ch );
			return;
		}

		if ((victim = get_char_world(ch, arg2 ) ) == NULL )
		{
			sprintf (buf, "You can't find %s.\n\r", arg2 );
			send_to_char(buf, ch );
			return;
		}

		if ( !is_same_clan (ch, victim) )
		{
			sprintf (buf, "%s hasn't applied for YOUR clan.\n\r",
			    victim->name );
			send_to_char(buf, ch );
			return;
		}

		switch( victim->pcdata->clanlevel )
		{
		    default:
			sprintf (buf, "%s has not applied for a clan.\n\r",
			    victim->name);
			send_to_char(buf, ch );
			break;
		    case CLAN_APPLICANT:
			victim->pcdata->clanlevel++;
			sprintf (buf, "Ok, %s has been accepted as applicant.\n\r", victim->name );
			send_to_char (buf, ch );
			send_to_char ("Your application has been acccepted, you are now a clan initiate.\n\r", victim);
			break;
		    case CLAN_LEVEL_5:
			if ( ch->pcdata->clanlevel < CLAN_RECRUITER )
			{
			    send_to_char( "You can't make anyone an applicator.\n\r",ch );
			    break;
			}
			victim->pcdata->clanlevel++;
			sprintf( buf, "Ok, %s has been made applicator.\n\r", victim->name );
			send_to_char (buf, ch );
			send_to_char( "You are now clan applicater.\n\r", victim );
			break;
		}
		return;
	} /* initiate */

	else if (!str_prefix ( arg1, "advance" ) )
	{
		if ( !is_in_clan(ch) )
		{
			send_to_char ("You are not with a clan.\n\r", ch );
			return;
		}

		if ( ch->pcdata->clanlevel < CLAN_LEADER )
		{
			send_to_char ("You have no power to advance members.\n\r", ch );
			return;
		}

		if (arg2[0] == '\0' )
		{
			send_to_char ("Who do you wish to advance.\n\r", ch );
			return;
		}

		if ((victim = get_char_world(ch, arg2 ) ) == NULL )
		{
			sprintf (buf, "You can't find %s.\n\r", arg2 );
			send_to_char(buf, ch );
			return;
		}

		if ( !is_same_clan( ch, victim ) )
		{
			sprintf (buf, "%s isn't a member of YOUR clan.\n\r",
			    victim->name );
			send_to_char(buf, ch );
			return;
		}

		if (victim->pcdata->clanlevel >= ch->pcdata->clanlevel )
		{
			sprintf (buf, "You are not powerfull enough to advance %s.\n\r",
			    victim->name );
			send_to_char( buf, ch );
			return;
		}

		if (victim->pcdata->clanlevel >= CLAN_LEVEL_5 )
		{
			sprintf (buf, "%s is already at maximum clan level.\n\r",
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
				send_to_char ("Levelrange is 1 to 5.\n\r", ch );
				return;
			}
			victim->pcdata->clanlevel = (CLAN_INITIATE + level);
		}
		sprintf (buf, "%s is now clanlevel %d.\n\r",
		    victim->name, (victim->pcdata->clanlevel - CLAN_INITIATE) );
		send_to_char (buf, ch );
		sprintf (buf, "You are now clanlevel %d.\n\r",
			(victim->pcdata->clanlevel - CLAN_INITIATE ) );
		send_to_char (buf, victim );
		return;
	} /* Advance */

        else if (!str_prefix( arg1, "recruit" ))
        {
                if ( !is_in_clan( ch ) )
                {
                        send_to_char ("You are not with a clan.\n\r", ch );
                        return;
                }

                if ( ch->pcdata->clanlevel < CLAN_RECRUITER )
                {
                        send_to_char ("You have no power to recruit members.\n\r", ch );
                        return;
                }

                if (arg2[0] == '\0' )
                {
                        send_to_char ("Who do you wish to recruit.\n\r", ch );
                        return;
                }

                if ((victim = get_char_world(ch, arg2 ) ) == NULL )
                {
                        sprintf (buf, "You can't find %s.\n\r", arg2 );
                        send_to_char(buf, ch );
                        return;
                }

                if ( !is_same_clan( ch, victim ) )
                {
                        sprintf (buf, "%s isn't a member of YOUR clan.\n\r",
                            victim->name );
                        send_to_char(buf, ch );
                        return;
                }

		switch( victim->pcdata->clanlevel )
		{
		    default:
                        sprintf (buf, "%s has yet been initiated for the clan.\n\r",
                            victim->name);
                        send_to_char(buf, ch );
                        break;
		    case CLAN_INITIATE:
			victim->pcdata->clanlevel++;
			sprintf (buf, "Ok, %s has been accepted as a full clan member.\n\r",
			    victim->name );
			send_to_char (buf, ch );
			send_to_char ("You are now a full clan member (level 1).\n\r", victim );
			break;
		    case CLAN_APPLICATOR:
			victim->pcdata->clanlevel++;
			sprintf( buf, "Ok, %s has been made clan recruiter.\n\r",
			    victim->name );
			send_to_char( buf, ch );
			send_to_char ( "You are now a clan recruiter.\n\r", victim );
			break;
		}
		return;
        } /* recruit */

	else if (!str_prefix ( arg1, "leader" ) )
	{
		if ( !is_in_clan(ch) )
		{
			send_to_char ("You are not with a clan.\n\r", ch );
			return;
		}

		if ( ch->pcdata->clanlevel < CLAN_LEADER )
		{
			send_to_char ("You have no power to make members leaders.\n\r", ch );
			return;
		}

		if (arg2[0] == '\0' )
		{
			send_to_char ("Who do you wish to advance to a leader.\n\r", ch );
			return;
		}

		if ((victim = get_char_world(ch, arg2 ) ) == NULL )
		{
			sprintf (buf, "You can't find %s.\n\r", arg2 );
			send_to_char(buf, ch );
			return;
		}

		if ( !is_same_clan( ch, victim ) )
		{
			sprintf (buf, "%s isn't a member of YOUR clan.\n\r",
			    victim->name );
			send_to_char(buf, ch );
			return;
		}

		if (victim->pcdata->clanlevel >= ch->pcdata->clanlevel )
		{
			sprintf (buf, "You are not powerfull enough to advance %s.\n\r",                    victim->name );
			send_to_char( buf, ch );
			return;
		}

		if ((IS_IMMORTAL(ch) && (ch->pcdata->clanlevel == CLAN_GOD )))
		{
			/* Remove leadership from a leader */
			if (victim->pcdata->clanlevel == CLAN_LEADER)
			{
				victim->pcdata->clanlevel--;
				send_to_char ("Ok... done.\n\r", ch );
			}
			/* Make someone a leader */
			else
			{
				victim->pcdata->clanlevel = CLAN_LEADER;
				send_to_char ("Ok... done.\n\r", ch );
				send_to_char ("You are now a clan leader.\n\r", victim );
			}
			return;
		}
		else if (ch->pcdata->clanlevel == CLAN_LEADER )
		{
			send_to_char ("Ok... you are no longer a leader.\n\r", ch );
			ch->pcdata->clanlevel--;
			if (victim->pcdata->clanlevel < CLAN_LEADER )
			{
				victim->pcdata->clanlevel = CLAN_LEADER;
				send_to_char ("You are now clan leader.\n\r", victim );
			}
			return;
		}
		else
		{
			send_to_char ("Something is wrong with leader, notify coders.\n\r", ch );
			return;
		}
	} /* Leader */

	else if (!str_cmp ( "leave", arg1 ) )
	{
		if (!is_in_clan(ch) )
		{
			send_to_char ("But you are not in a clan...\n\r", ch);
			return;
		}

		if (arg2[0] == '\0')
		{
			send_to_char ("Are you very sure you want to leave the clan ??\n\r", ch );
			send_to_char ("If so, type: clan leave, but with the string below as argument.\n\r", ch );
			sprintf ( buf, "%s\n\r", ch->pcdata->pwd );
			send_to_char (buf, ch);
			return;
		}
		else if (!str_cmp( arg2, ch->pcdata->pwd) )
		{
			send_to_char ("OK... you are now exiled.\n\r", ch );
			ch->pcdata->clan = CLAN_NONE;
			ch->pcdata->clanlevel = CLAN_EXILE;
			return;
		}
		else
		{
			send_to_char ("That was not the correct string.\n\r", ch );
			return;
		}
	}

	else if (!str_prefix (arg1, "kick" ) )
	{
		if ( !is_in_clan(ch) )
		{
			send_to_char ("You are not with a clan.\n\r", ch );
			return;
		}

		if ( ch->pcdata->clanlevel < CLAN_RECRUITER )
		{
			send_to_char ("You have no power to kick members.\n\r", ch );
			return;
		}

		if (arg2[0] == '\0' )
		{
			send_to_char ("Who do you wish to kick.\n\r", ch );
			return;
		}

		if ((victim = get_char_world(ch, arg2 ) ) == NULL )
		{
			sprintf (buf, "You can't find %s.\n\r", arg2 );
			send_to_char(buf, ch );
			return;
		}

		if ( !is_same_clan( ch, victim ) )
		{
			sprintf (buf, "%s isn't a member of YOUR clan.\n\r",
			    victim->name );
			send_to_char(buf, ch );
			return;
		}

		if (victim->pcdata->clanlevel >= ch->pcdata->clanlevel )
		{
			sprintf (buf, "You are not powerfull enough to kick %s.\n\r",                    victim->name );
			send_to_char( buf, ch );
			return;
		}

		/* Ok... do it... */
		victim->pcdata->clan = CLAN_NONE;
		victim->pcdata->clanlevel = CLAN_EXILE;
		send_to_char ("You have been kicked from the clan.\n\r", victim );
		send_to_char ("You are now exiled.\n\r", victim );
		sprintf (buf, "%s has been exiled.\n\r", victim->name );
		send_to_char (buf, ch );
		return;
	}

	else if (!str_prefix ( arg1, "list" ) )
	{
		sprintf (buf, "Clan List\n=========\n\n");
		send_to_char (buf, ch );
		for(clan = 1; clan_table[clan].vnum != 0; clan++)
		{
			sprintf (buf, "%s\n", clan_table[clan].name);
			send_to_char (buf, ch );
		}
		return;
	}

	else if ( !str_prefix( arg1, "where" ) )
	{
	    sprintf( buf, "Clan member:  Where:\n\r" );
	    send_to_char( buf, ch );
	    for( victim = char_list; victim; victim = victim->next )
	    {
		if( is_same_clan( ch, victim ) )
		{
		    sprintf( buf, "%-13s %s\n\r",
			victim->name, victim->in_room->name );
		    send_to_char( buf, ch );
		}
	    }
	    return;
	}

	else if ( !str_prefix( arg1, "who" ) )
	{
	    interpret( ch, "who clan" );
	    return;
	}

	else
	{
		send_to_char ("Unknown option.\n\r", ch );
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

	if ( !authorized( ch, "guild" ) )
	    return;

	if ( arg1[0] == '\0' )
	{
		send_to_char( "Syntax: guild <char> <cln name> <level>\n\r",ch);
		return;
	}

	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	{
		send_to_char( "They aren't playing.\n\r", ch );
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char( "Only on players.\n\r", ch );
		return;
	}

	if ( arg2[0] == '\0' )
	{
		sprintf(buf,"%s is member of clan %s (level %d).\n\r",
		victim->name, capitalize(clan_table[ch->pcdata->clan].name),
		(victim->pcdata->clanlevel - CLAN_INITIATE) );
                send_to_char(buf,ch);
		return;
	}

	if (!str_prefix(arg2,"none"))
	{
		sprintf (buf, "%s is now clanless.\n\r", victim->name);
		send_to_char(buf, ch);
		send_to_char("You are now a member of no clan!\n\r",victim);
		victim->pcdata->clan = CLAN_NONE;
		victim->pcdata->clanlevel = CLAN_NOLEVEL;
		return;
	}

	if (!str_prefix(arg2,"exile"))
	{
		sprintf (buf, "%s is now exiled.\n\r", victim->name);
		send_to_char(buf, ch);
		send_to_char("You are now exiled\n\r",victim);
		victim->pcdata->clan = CLAN_NONE;
		victim->pcdata->clanlevel = CLAN_EXILE;
		return;
	}

	if (arg3[0] == '\0' )
		level = CLAN_APPLICANT;
	else
		level = (CLAN_INITIATE + atoi(arg3));

	if ( ( level < CLAN_APPLICANT ) || ( level > CLAN_GOD ) )
	{
		send_to_char ("Level range is -1 to 9.\n\r", ch );
		send_to_char ("-1 = Applicant, 0 = initiate, 1-5 = level 1-5\n\r", ch);
		send_to_char ("6 = applicator, 7 = recruiter, 8 = leader 9 = god.\n\r", ch );
		return;
	}

	if ((clan = clan_lookup(arg2)) == 0)
	{
		send_to_char("No such clan exists.\n\r",ch);
		return;
	}
	else
	{
		sprintf(buf,"%s is now a member of clan %s (level %d).\n\r",
		    victim->name, capitalize(clan_table[clan].name),
		    (level - CLAN_INITIATE) );
		send_to_char(buf,ch);
		sprintf(buf,"You are now a member of clan %s (level %d).\n\r",
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
			send_to_char ("You must be of good alignment to join clan Mythran.\n\r", ch );
			return FALSE;
		}
		if (ch->level < 15 )
		{
			send_to_char ("You must be level 15 to join clan Mythran.\n\r", ch );
			return FALSE;	/* Must be level 15 */
		}
		if (ch->gold < 50000 )
		{
			send_to_char ("You don't have enough money to join clan Mythran.\n\r", ch );
			return FALSE;	/* only wealthy players... */
		}
	}
	else if ( clan == CLAN_DRAGON )
	{
	    if ( ch->alignment < 0 ) /* Accept good chars only */
	    {
		send_to_char ( "You are too evil to join the Dragon clan.\n\r", ch );
		return FALSE;	/* Only kind/good chars */
	    }
	    if ( ch->level < 20 )
	    {
		send_to_char ( "You are too inexperienced. You must be level 20 to join Dragokn clan.\n\r", ch );
		return FALSE;	/* Must be level 20 */
	    }
	    if ( ch->pcdata->title == title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0 ] )
	    {
		send_to_char ( "You are not orriginal enough to join clan Dragon.\n\r", ch );
		return FALSE;	/* Must have set his own title */
	    }
	}
	return TRUE;
}
