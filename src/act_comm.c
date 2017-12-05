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



/*
 * Local functions.
 */
char	*initial	args( ( const char *str ) );
bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	note_attach	args( ( CHAR_DATA *ch ) );
void	note_remove	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb ) );

/* RT code to delete yourself */

void do_delet( CHAR_DATA *ch, char *argument)
{
    send_to_char("You must type the full command to delete yourself.\r\n",ch);
}

void do_delete( CHAR_DATA *ch, char *argument)
{
	char strsave[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';

	if (IS_NPC(ch))
		return;

	if (ch->pcdata->confirm_delete)
	{
		if (argument[0] != '\0')
		{
			send_to_char("Delete status removed.\r\n",ch);
			ch->pcdata->confirm_delete = FALSE;
			sprintf (buf, "%s has decided not to self-delete.\r\n", ch->name );
			wiznet(ch, WIZ_LOGINS,0, buf);
			return;
		}
		else
		{
			sprintf (buf, "%s has been turned into line noise.\r\n", ch->name );
			wiznet(ch, WIZ_LOGINS,0, buf);

#if !defined( macintosh ) && !defined( MSDOS )
			sprintf( strsave, "%s%s%s%s", PLAYER_DIR,
			initial( ch->name ), "/", capitalize( ch->name ) );
#else
			sprintf( strsave, "%s%s", PLAYER_DIR,
			capitalize( ch->name ) );
#endif

			do_quit(ch,"");
			unlink(strsave);
			return;
		}
	}

	if (argument[0] != '\0')
	{
		send_to_char("Just type delete. No argument.\r\n",ch);
		return;
	}

	send_to_char("Type delete again to confirm this command.\r\n",ch);
	send_to_char("WARNING: this command is irreversible.\r\n",ch);
	send_to_char("Typing delete with an argument will undo.\r\n", ch);
	ch->pcdata->confirm_delete = TRUE;

	sprintf (buf, "%s is contemplating deletion.\r\n", ch->name );
	wiznet(ch, WIZ_LOGINS,0, buf);
}

bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    if ( !str_cmp( ch->name, pnote->sender ) )
	return TRUE;

    if ( is_name( "all", pnote->to_list ) )
	return TRUE;

    if ( IS_HERO( ch ) && (   is_name( "immortal",  pnote->to_list )
			   || is_name( "immortals", pnote->to_list )
			   || is_name( "imm",       pnote->to_list )
			   || is_name( "immort",    pnote->to_list ) ) )
	
	return TRUE;

    if ( is_name( ch->name, pnote->to_list ) )
	return TRUE;

    /* For clan mail, by Maniac */
    if ( is_name( clan_name(ch->pcdata->clan), pnote->to_list ) &&
		( ch->pcdata->clanlevel >= CLAN_APPLICATOR ) )
	return TRUE;

    return FALSE;
}



void note_attach( CHAR_DATA *ch )
{
    NOTE_DATA *pnote;

    if ( ch->pnote )
	return;

    if ( !note_free )
    {
	pnote	  = alloc_perm( sizeof( *ch->pnote ) );
    }
    else
    {
	pnote	  = note_free;
	note_free = note_free->next;
    }

    pnote->next		= NULL;
    pnote->sender	= str_dup( ch->name );
    pnote->date		= str_dup( "" );
    pnote->to_list	= str_dup( "" );
    pnote->subject	= str_dup( "" );
    pnote->text		= str_dup( "" );
    ch->pnote		= pnote;
    return;
}



void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote )
{
    FILE      *fp;
    NOTE_DATA *prev;
    char      *to_list;
    char       to_new [ MAX_INPUT_LENGTH ];
    char       to_one [ MAX_INPUT_LENGTH ];

    /*
     * Build a new to_list.
     * Strip out this recipient.
     */
    to_new[0]	= '\0';
    to_list	= pnote->to_list;
    while ( *to_list != '\0' )
    {
	to_list	= one_argument( to_list, to_one );
	if ( to_one[0] != '\0' && str_cmp( ch->name, to_one ) )
	{
	    strcat( to_new, " "    );
	    strcat( to_new, to_one );
	}
    }

    /*
     * Just a simple recipient removal?
     */
    if ( str_cmp( ch->name, pnote->sender ) && to_new[0] != '\0' )
    {
	free_string( pnote->to_list );
	pnote->to_list = str_dup( to_new + 1 );
	return;
    }

    /*
     * Remove note from linked list.
     */
    if ( pnote == note_list )
    {
	note_list = pnote->next;
    }
    else
    {
	for ( prev = note_list; prev; prev = prev->next )
	{
	    if ( prev->next == pnote )
		break;
	}

	if ( !prev )
	{
	    bug( "Note_remove: pnote not found.", 0 );
	    return;
	}

	prev->next = pnote->next;
    }

    free_string( pnote->text    );
    free_string( pnote->subject );
    free_string( pnote->to_list );
    free_string( pnote->date    );
    free_string( pnote->sender  );
    pnote->next	= note_free;
    note_free	= pnote;

    /*
     * Rewrite entire list.
     */
    fclose( fpReserve );
    if ( !( fp = fopen( NOTE_FILE, "w" ) ) )
    {
	perror( NOTE_FILE );
    }
    else
    {
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    fprintf( fp, "Sender  %s~\n", pnote->sender     );
	    fprintf( fp, "Date    %s~\n", pnote->date       );
	    fprintf( fp, "Stamp   %ld\n", (unsigned long)pnote->date_stamp );
	    fprintf( fp, "To      %s~\n", pnote->to_list    );
	    fprintf( fp, "Subject %s~\n", pnote->subject    );
	    fprintf( fp, "Text\n%s~\n\n", pnote->text       );
	}
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/* Date stamp idea comes from Alander of ROM */
void do_note( CHAR_DATA *ch, char *argument )
{
    NOTE_DATA *pnote;
    char       buf  [ MAX_STRING_LENGTH   ];
    char       buf1 [ MAX_STRING_LENGTH*7 ];
    char       arg  [ MAX_INPUT_LENGTH    ];
    int        vnum;
    int        anum;

    if ( IS_NPC( ch ) )
	return;

    argument = one_argument( argument, arg );
    smash_tilde( argument );

    if ( arg[0] == '\0' )
    {
	do_note( ch, "read" );
	return;
    }

    if ( !str_cmp( arg, "list" ) )
    {
	vnum    = 0;
	buf1[0] = '\0';
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) )
	    {
		sprintf( buf, "[%3d%s] %s: %s\r\n",
			vnum,
			( pnote->date_stamp > ch->last_note
			 && str_cmp( pnote->sender, ch->name ) ) ? "N" : " ",
			pnote->sender, pnote->subject );
		strcat( buf1, buf );
		vnum++;
	    }
	}
	send_to_char( buf1, ch );
	return;
    }

    if ( !str_cmp( arg, "read" ) )
    {
	bool fAll;

	if ( !str_cmp( argument, "all" ) )
	{
	    fAll = TRUE;
	    anum = 0;
	}
	else if ( argument[0] == '\0' || !str_prefix( argument, "next" ) )
	  /* read next unread note */
	{
	    vnum    = 0;
	    buf1[0] = '\0';
	    for ( pnote = note_list; pnote; pnote = pnote->next )
	    {
		if ( is_note_to( ch, pnote )
		    && str_cmp( ch->name, pnote->sender )
		    && ch->last_note < pnote->date_stamp )
		{
		    break;
		}
		else
		{
		    if ( is_note_to( ch, pnote ) )
		        vnum++;
		}
	    }
	    if ( pnote )
	    {
		sprintf( buf, "[%3d] %s: %s\r\n%s\r\nTo: %s\r\n",
			vnum,
			pnote->sender,
			pnote->subject,
			pnote->date,
			pnote->to_list );
		strcat( buf1, buf );
		strcat( buf1, pnote->text );
		ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
		send_to_char( buf1, ch );
		return;
	    }
	    send_to_char( "You have no unread notes.\r\n", ch );
	    return;
	}
	else if ( is_number( argument ) )
	{
	    fAll = FALSE;
	    anum = atoi( argument );
	}
	else
	{
	    send_to_char( "Note read which number?\r\n", ch );
	    return;
	}

	vnum    = 0;
	buf1[0] = '\0';
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) )
	    {
	        if ( vnum == anum || fAll )
		{
		    sprintf( buf, "[%3d] %s: %s\r\n%s\r\nTo: %s\r\n",
			    vnum,
			    pnote->sender,
			    pnote->subject,
			    pnote->date,
			    pnote->to_list );
		    strcat( buf1, buf );
		    strcat( buf1, pnote->text );
		    if ( !fAll )
		      send_to_char( buf1, ch );
		    else
		      strcat( buf1, "\r\n" );
		    ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
		    if ( !fAll )
		      return;
		}
		vnum++;
	    }
	}

	if ( !fAll )
	    send_to_char( "No such note.\r\n", ch );
	else
	    send_to_char( buf1, ch );
	return;
    }

    if ( !str_cmp( arg, "+" ) )
    {
	note_attach( ch );
	strcpy( buf, ch->pnote->text );
	if ( strlen( buf ) + strlen( argument ) >= MAX_STRING_LENGTH - 200 )
	{
	    send_to_char( "Note too long.\r\n", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\r\n"   );
	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf );
	send_to_char( "Ok.\r\n", ch );
	return;
    }

    if (!str_cmp(arg,"-"))
    {
        int len;
        bool found = FALSE;

        note_attach(ch);

        if (ch->pnote->text == NULL || ch->pnote->text[0] == '\0')
        {
            send_to_char("No lines left to remove.\r\n",ch);
            return;
        }

        strcpy(buf,ch->pnote->text);

        for (len = strlen(buf); len > 0; len--)
        {
            if (buf[len] == '\r')
            {
                if (!found)  /* back it up */
                {
                    if (len > 0)
                        len--;
                    found = TRUE;
                }
                else /* found the second one */
                {
                    buf[len + 1] = '\0';
                    free_string(ch->pnote->text);
                    ch->pnote->text = str_dup(buf);
                    return;
                }
            }
        }
        buf[0] = '\0';
        free_string(ch->pnote->text);
        ch->pnote->text = str_dup(buf);
        return;
    }

    if ( !str_cmp( arg, "subject" ) )
    {
	note_attach( ch );
	free_string( ch->pnote->subject );
	ch->pnote->subject = str_dup( argument );
	send_to_char( "Ok.\r\n", ch );
	return;
    }

    if ( !str_cmp( arg, "to" ) )
    {
	note_attach( ch );
	free_string( ch->pnote->to_list );
	ch->pnote->to_list = str_dup( argument );
	send_to_char( "Ok.\r\n", ch );
	return;
    }

    if ( !str_cmp( arg, "clear" ) )
    {
	if ( ch->pnote )
	{
	    free_string( ch->pnote->text    );
	    free_string( ch->pnote->subject );
	    free_string( ch->pnote->to_list );
	    free_string( ch->pnote->date    );
	    free_string( ch->pnote->sender  );
	    ch->pnote->next	= note_free;
	    note_free		= ch->pnote;
	    ch->pnote		= NULL;
	}

	send_to_char( "Ok.\r\n", ch );
	return;
    }

    if ( !str_cmp( arg, "show" ) )
    {
	if ( !ch->pnote )
	{
	    send_to_char( "You have no note in progress.\r\n", ch );
	    return;
	}

	sprintf( buf, "%s: %s\r\nTo: %s\r\n",
		ch->pnote->sender,
		ch->pnote->subject,
		ch->pnote->to_list );
	send_to_char( buf, ch );
	send_to_char( ch->pnote->text, ch );
	return;
    }

    if ( !str_cmp( arg, "post" ) || !str_prefix( arg, "send" ) )
    {
	FILE *fp;
	char *strtime;

	if ( !ch->pnote )
	{
	    send_to_char( "You have no note in progress.\r\n", ch );
	    return;
	}

	if ( !str_cmp( ch->pnote->to_list, "" ) )
	{
	    send_to_char(
	      "You need to provide a recipient (name, all, or immortal).\r\n",
			 ch );
	    return;
	}

	if ( !str_cmp( ch->pnote->subject, "" ) )
	{
	    send_to_char( "You need to provide a subject.\r\n", ch );
	    return;
	}

	ch->pnote->next			= NULL;
	strtime				= ctime( &current_time );
	strtime[strlen(strtime)-1]	= '\0';
	free_string( ch->pnote->date );
	ch->pnote->date			= str_dup( strtime );
	ch->pnote->date_stamp           = current_time;

	if ( !note_list )
	{
	    note_list	= ch->pnote;
	}
	else
	{
	    for ( pnote = note_list; pnote->next; pnote = pnote->next )
		;
	    pnote->next	= ch->pnote;
	}
	pnote		= ch->pnote;
	ch->pnote       = NULL;

	fclose( fpReserve );
	if ( !( fp = fopen( NOTE_FILE, "a" ) ) )
	{
	    perror( NOTE_FILE );
	}
	else
	{
	    fprintf( fp, "Sender  %s~\n", pnote->sender     );
	    fprintf( fp, "Date    %s~\n", pnote->date       );
	    fprintf( fp, "Stamp   %ld\n", (unsigned long)pnote->date_stamp );
	    fprintf( fp, "To      %s~\n", pnote->to_list    );
	    fprintf( fp, "Subject %s~\n", pnote->subject    );
	    fprintf( fp, "Text\n%s~\n\n", pnote->text       );
	    fclose( fp );
	}
	fpReserve = fopen( NULL_FILE, "r" );

	send_to_char( "Ok.\r\n", ch );
	return;
    }

    if ( !str_cmp( arg, "remove" ) )
    {
	if ( !is_number( argument ) )
	{
	    send_to_char( "Note remove which number?\r\n", ch );
	    return;
	}

	anum = atoi( argument );
	vnum = 0;
	for ( pnote = note_list; pnote; pnote = pnote->next )
	{
	    if ( is_note_to( ch, pnote ) && vnum++ == anum )
	    {
		note_remove( ch, pnote );
		send_to_char( "Ok.\r\n", ch );
		return;
	    }
	}

	send_to_char( "No such note.\r\n", ch );
	return;
    }

    send_to_char( "Huh?  Type 'help note' for usage.\r\n", ch );
    return;
}



/*
 * Generic channel function.
 */
void talk_channel( CHAR_DATA *ch, char *argument, int channel,
		  const char *verb )
{
    DESCRIPTOR_DATA *d;
    char             buf [ MAX_STRING_LENGTH ];
    int              position;

    if ( argument[0] == '\0' )
    {
	sprintf( buf, "%s what?\r\n", verb );
	buf[0] = UPPER( buf[0] );
	return;
    }

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
    {
	sprintf( buf, "You can't %s.\r\n", verb );
	send_to_char( buf, ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
        send_to_char( "You can't seem to break the silence.\r\n", ch );
        return;
    }

    REMOVE_BIT( ch->deaf, channel );

    switch ( channel )
    {
    default:
	sprintf( buf, "You %s '%s'\r\n", verb, argument );
	send_to_char( buf, ch );
	sprintf( buf, "$n %ss '$t'",     verb );
	break;

    case CHANNEL_IMMTALK:
	sprintf( buf, "$n: $t" );
	position	= ch->position;
	ch->position	= POS_STANDING;
	act( buf, ch, argument, NULL, TO_CHAR );
	ch->position	= position;
	break;
    }

    /* Make the words drunk if needed... */
    argument = makedrunk(argument,ch);

    for ( d = descriptor_list; d; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( d->connected == CON_PLAYING
	    && vch != ch
	    && !IS_SET( och->deaf, channel )
	    && !IS_SET( och->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	{
	    if ( channel == CHANNEL_IMMTALK && !IS_HERO( och ) )
		continue;
	    if ( ( channel == CHANNEL_CLAN ) && ( !is_same_clan( ch, vch) ) )
		continue;
	    if ( channel == CHANNEL_YELL
		&& vch->in_room->area != ch->in_room->area )
	        continue;

	    position		= vch->position;
	    if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
		vch->position	= POS_STANDING;
	    act( buf, ch, argument, vch, TO_VICT );
	    vch->position	= position;
	}
    }
    return;
}

void do_chat( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_CHAT, "chat" );
    return;
}


/*
 * Alander's new channels.
 */
void do_music( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_MUSIC, "music" );
    return;
}



void do_question( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "question" );
    return;
}



void do_answer( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "answer" );
    return;
}



void do_shout( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_SHOUT, "shout" );
    WAIT_STATE( ch, 12 );
    return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_YELL, "yell" );
    return;
}



void do_immtalk( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
  
    rch = get_char( ch );

    if ( !authorized( rch, "immtalk" ) )
        return;

    talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
    return;
}



void do_say( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\r\n", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
        send_to_char( "You can't seem to break the silence.\r\n", ch );
        return;
    }

#if defined ( FORCE_LANGUAGE )
	if (!IS_NPC(ch))
	{
		do_language(ch, argument, ch->pcdata->speaking);
		mprog_speech_trigger( argument, ch );
		return;
	}
	else
	{
		act( "$n says '$T'", ch, NULL, argument, TO_ROOM );
		act( "You say '$T'", ch, NULL, argument, TO_CHAR );
		mprog_speech_trigger( argument, ch );
		return;
	}
#else
	argument = makedrunk(argument,ch);
	act( "$n says '$T'", ch, NULL, argument, TO_ROOM );
	act( "You say '$T'", ch, NULL, argument, TO_CHAR );
	mprog_speech_trigger( argument, ch );
	return;
#endif
}



void do_tell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        position;

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
        send_to_char( "You can't seem to break the silence.\r\n", ch );
        return;
    }

    one_argument( argument, arg );

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( !( victim = get_char_world( ch, arg ) )
	|| ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( ( !IS_NPC( ch ) && (   IS_SET( ch->act, PLR_SILENCE )
			     || IS_SET( ch->act, PLR_NO_TELL ) ) )
	|| IS_SET( victim->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
	send_to_char( "Your message didn't get through.\r\n", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\r\n", ch );
	return;
    }

    if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    argument = makedrunk(argument,ch);
    act( "You tell $N '$t'", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
    act( "$n tells you '$t'", ch, argument, victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;

    if ( IS_SET( victim->act, PLR_AFK ) )
        act( "Just so you know, $E is AFK.", ch, NULL, victim, TO_CHAR );

    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int        position;

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
        send_to_char( "You can't seem to break the silence.\r\n", ch );
        return;
    }

    if ( !( victim = ch->reply ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( ( !IS_NPC( ch ) && (   IS_SET( ch->act, PLR_SILENCE )
 			     || IS_SET( ch->act, PLR_NO_TELL ) ) )
	|| IS_SET( victim->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
	send_to_char( "Your message didn't get through.\r\n", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Reply what?\r\n", ch );
        return;
    }

    if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    argument = makedrunk(argument,ch);
    act( "You tell $N '$t'",  ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
    act( "$n tells you '$t'", ch, argument, victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;

    if ( IS_SET( victim->act, PLR_AFK ) )
        act( "Just so you know, $E is AFK.", ch, NULL, victim, TO_CHAR );

    return;
}



void do_emote( CHAR_DATA *ch, char *argument )
{
    char  buf [ MAX_STRING_LENGTH ];
    char *plast;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
    {
	send_to_char( "You can't show your emotions.\r\n", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Emote what?\r\n", ch );
	return;
    }

    for ( plast = argument; *plast != '\0'; plast++ )
	;

    strcpy( buf, argument );
    if ( isalpha( plast[-1] ) )
	strcat( buf, "." );

    act( "$n $T", ch, NULL, buf, TO_ROOM );
    act( "$n $T", ch, NULL, buf, TO_CHAR );
    return;
}



/*
 * All the posing stuff.
 */
struct	pose_table_type
{
    char * message[ 2*MAX_CLASS ];
};

/*
 * Pose table
 *
 * MAGE
 * CLERIC
 * THIEF
 * WARRIOR
 * PSIONIC
 * NECROMANCER
 * DRUID
 * RANGER
 */

const	struct	pose_table_type	pose_table	[]	=
{
    {
	{
	    "You sizzle with energy.",
	    "$n sizzles with energy.",
	    "You feel very holy.",
	    "$n looks very holy.",
	    "You perform a small card trick.",
	    "$n performs a small card trick.",
	    "You show your bulging muscles.",
	    "$n shows $s bulging muscles.",
            "Stop it with the Ouija board, will ya?",
            "Great, $n is playing with $s Ouija board again."
	    "You create a door of blackness and step thru it."
	    "$n creates a door of blackness and steps thru it."
	    "You throw some herbs on the floor: 'It's going to rain'."
	    "$n throws some herbs on the floor and says: 'It's going to rain'."
	    "You look around, looking for lost animals."
	    "$n is peering around."
	}
    },

    {
	{
	    "You turn into a butterfly, then return to your normal shape.",
	    "$n turns into a butterfly, then returns to $s normal shape.",
	    "You nonchalantly turn wine into water.",
	    "$n nonchalantly turns wine into water.",
	    "You wiggle your ears alternately.",
	    "$n wiggles $s ears alternately.",
	    "You crack nuts between your fingers.",
	    "$n cracks nuts between $s fingers.",
            "You read everyone's mind....and shudder with disgust.",
            "$n reads your mind...eww, you pervert!"
	}
    },

    {
	{
	    "Blue sparks fly from your fingers.",
	    "Blue sparks fly from $n's fingers.",
	    "A halo appears over your head.",
	    "A halo appears over $n's head.",
	    "You nimbly tie yourself into a knot.",
	    "$n nimbly ties $mself into a knot.",
	    "You grizzle your teeth and look mean.",
	    "$n grizzles $s teeth and looks mean.",
            "You show everyone your awards for perfect school attendance",
            "You aren't impressed by $n's school attendance awards.  Geek."
	}
    },

    {
	{
	    "Little red lights dance in your eyes.",
	    "Little red lights dance in $n's eyes.",
	    "You recite words of wisdom.",
	    "$n recites words of wisdom.",
	    "You juggle with daggers, apples, and eyeballs.",
	    "$n juggles with daggers, apples, and eyeballs.",
	    "You hit your head, and your eyes roll.",
	    "$n hits $s head, and $s eyes roll.",
            "A will-o-the-wisp arrives with your slippers.",
            "A will-o-the-wisp arrives with $n's slippers."
	}
    },

    {
	{
	    "A slimy green monster appears before you and bows.",
	    "A slimy green monster appears before $n and bows.",
	    "Deep in prayer, you levitate.",
	    "Deep in prayer, $n levitates.",
	    "You steal the underwear off every person in the room.",
	    "Your underwear is gone!  $n stole it!",
	    "Crunch, crunch -- you munch a bottle.",
	    "Crunch, crunch -- $n munches a bottle.",
            "What's with the extra leg?",
            "Why did $n sprout an extra leg just now?"
	}
    },

    {
	{
	    "You turn everybody into a little pink elephant.",
	    "You are turned into a little pink elephant by $n.",
	    "An angel consults you.",
	    "An angel consults $n.",
	    "The dice roll ... and you win again.",
	    "The dice roll ... and $n wins again.",
	    "... 98, 99, 100 ... you do pushups.",
	    "... 98, 99, 100 ... $n does pushups.",
            "The spoons flee as you begin to concentrate.",
            "The spoons flee as $n begins to concentrate."
	}
    },

    {
	{
	    "A small ball of light dances on your fingertips.",
	    "A small ball of light dances on $n's fingertips.",
	    "Your body glows with an unearthly light.",
	    "$n's body glows with an unearthly light.",
	    "You count the money in everyone's pockets.",
	    "Check your money, $n is counting it.",
	    "Arnold Schwarzenegger admires your physique.",
	    "Arnold Schwarzenegger admires $n's physique.",
            "Stop wiggling your brain at people.",
            "Make $n stop wiggling $s brain at you!"
	}
    },

    {
	{
	    "Smoke and fumes leak from your nostrils.",
	    "Smoke and fumes leak from $n's nostrils.",
	    "A spotlight hits you.",
	    "A spotlight hits $n.",
	    "You balance a pocket knife on your tongue.",
	    "$n balances a pocket knife on your tongue.",
	    "Watch your feet, you are juggling granite boulders.",
	    "Watch your feet, $n is juggling granite boulders.",
            "MENSA called...they want your opinion on something.",
            "MENSA just called $n for consultation."
	}
    },

    {
	{
	    "The light flickers as you rap in magical languages.",
	    "The light flickers as $n raps in magical languages.",
	    "Everyone levitates as you pray.",
	    "You levitate as $n prays.",
	    "You produce a coin from everyone's ear.",
	    "$n produces a coin from your ear.",
	    "Oomph!  You squeeze water out of a granite boulder.",
	    "Oomph!  $n squeezes water out of a granite boulder.",
            "Chairs fly around the room at your slightest whim.",
            "Chairs fly around the room at $n's slightest whim."
	}
    },

    {
	{
	    "Your head disappears.",
	    "$n's head disappears.",
	    "A cool breeze refreshes you.",
	    "A cool breeze refreshes $n.",
	    "You step behind your shadow.",
	    "$n steps behind $s shadow.",
	    "You pick your teeth with a spear.",
	    "$n picks $s teeth with a spear.",
            "Oof...maybe you shouldn't summon any more hippopotamuses.",
            "Oof!  Guess $n won't be summoning any more hippos for a while."
	}
    },

    {
	{
	    "A fire elemental singes your hair.",
	    "A fire elemental singes $n's hair.",
	    "The sun pierces through the clouds to illuminate you.",
	    "The sun pierces through the clouds to illuminate $n.",
	    "Your eyes dance with greed.",
	    "$n's eyes dance with greed.",
	    "Everyone is swept off their foot by your hug.",
	    "You are swept off your feet by $n's hug.",
            "Oops...your hair is sizzling from thinking too hard.",
            "Oops...$n's hair is sizzling from thinking too hard."
	}
    },

    {
	{
	    "The sky changes color to match your eyes.",
	    "The sky changes color to match $n's eyes.",
	    "The ocean parts before you.",
	    "The ocean parts before $n.",
	    "You deftly steal everyone's weapon.",
	    "$n deftly steals your weapon.",
	    "Your karate chop splits a tree.",
	    "$n's karate chop splits a tree.",
            "What?  You were too busy concentrating.",
            "What?  Oh, $n was lost in thought...again."
	}
    },

    {
	{
	    "The stones dance to your command.",
	    "The stones dance to $n's command.",
	    "A thunder cloud kneels to you.",
	    "A thunder cloud kneels to $n.",
	    "The Grey Mouser buys you a beer.",
	    "The Grey Mouser buys $n a beer.",
	    "A strap of your armor breaks over your mighty thews.",
	    "A strap of $n's armor breaks over $s mighty thews.",
            "Will you get down here before you get hurt?",
            "Quick, get a stick, $n is doing $s pinata impression again."
	}
    },

    {
	{
	    "The heavens and grass change colour as you smile.",
	    "The heavens and grass change colour as $n smiles.",
	    "The Burning Man speaks to you.",
	    "The Burning Man speaks to $n.",
	    "Everyone's pocket explodes with your fireworks.",
	    "Your pocket explodes with $n's fireworks.",
	    "A boulder cracks at your frown.",
	    "A boulder cracks at $n's frown.",
            "Careful...don't want to disintegrate anyone!",
            "LOOK OUT!  $n is trying to disintegrate something!"
	}
    },

    {
	{
	    "Everyone's clothes are transparent, and you are laughing.",
	    "Your clothes are transparent, and $n is laughing.",
	    "An eye in a pyramid winks at you.",
	    "An eye in a pyramid winks at $n.",
	    "Everyone discovers your dagger a centimeter from their eye.",
	    "You discover $n's dagger a centimeter from your eye.",
	    "Mercenaries arrive to do your bidding.",
	    "Mercenaries arrive to do $n's bidding.",
            "You run off at the mouth about 'mind over matter'.",
            "Yeah, yeah, mind over matter.  Shut up, $n."
	}
    },

    {
	{
	    "A black hole swallows you.",
	    "A black hole swallows $n.",
	    "Valentine Michael Smith offers you a glass of water.",
	    "Valentine Michael Smith offers $n a glass of water.",
	    "Where did you go?",
	    "Where did $n go?",
	    "Four matched Percherons bring in your chariot.",
	    "Four matched Percherons bring in $n's chariot.",
            "Thud.",
            "Thud."
	}
    },

    {
	{
	    "The world shimmers in time with your whistling.",
	    "The world shimmers in time with $n's whistling.",
	    "The great god Mota gives you a staff.",
	    "The great god Mota gives $n a staff.",
	    "Click.",
	    "Click.",
	    "Atlas asks you to relieve him.",
	    "Atlas asks $n to relieve him.",
            "You charm the pants off everyone...and refuse to give them back.",
            "Your pants are charmed off by $n, and $e won't give them back."
	}
    }
};

void do_pose( CHAR_DATA *ch, char *argument )
{
    int level;
    int pose;

    if ( IS_NPC( ch ) )
	return;

    level = UMIN( ch->level,
		 sizeof( pose_table ) / sizeof( pose_table[0] ) - 1 );
    pose  = number_range( 0, level );

    act( pose_table[pose].message[2*ch->class+0], ch, NULL, NULL, TO_CHAR );
    act( pose_table[pose].message[2*ch->class+1], ch, NULL, NULL, TO_ROOM );

    return;
}



void do_bug( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	send_to_char( "The Implementors look at you quizzically.\r\n", ch );
	return;
    }

    append_file( ch, BUG_FILE,  argument );
    send_to_char( "Ok.  Thanks.\r\n", ch );
    return;
}



void do_idea( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	send_to_char( "The Implementors look at you quizzically.\r\n", ch );
	return;
    }

    append_file( ch, IDEA_FILE, argument );
    send_to_char( "Ok.  Thanks.\r\n", ch );
    return;
}



void do_typo( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	send_to_char( "The Implementors look at you quizzically.\r\n", ch );
	return;
    }

    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Ok.  Thanks.\r\n", ch );
    return;
}



void do_rent( CHAR_DATA *ch, char *argument )
{
    send_to_char( "There is no rent here.  Just save and quit.\r\n", ch );
    return;
}



void do_qui( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to QUIT, you have to spell it out.\r\n", ch );
    return;
}



void do_quit( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( IS_NPC( ch ) )
	return;

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "No way! You are fighting.\r\n", ch );
	return;
    }

    if ( ch->position  < POS_STUNNED  )
    {
	send_to_char( "You're not DEAD yet.\r\n", ch );
	return;
    }

    if ( auction->item != NULL && (ch == auction->buyer) )
    {
	send_to_char ("Wait until you have bought the item on auction.\r\n", ch);
	return;
    }
    if ( auction->item != NULL && (ch == auction->seller) )
    {
        send_to_char ("Wait till you have sold the item on auction.\r\n", ch);
        return;
    }

    send_to_char( "Adde parvum parvo magnus acervus erit.\r\n", ch );
    send_to_char( "[Add little to little ",                     ch );
    send_to_char( "and there will be a big pile]\r\n\r\n",      ch );

    act( "$n has left the game.", ch, NULL, NULL, TO_ROOM );
    sprintf( log_buf, "%s has quit.", ch->name );
    log_string( log_buf );
    wiznet (ch, WIZ_LOGINS, 0, log_buf );

    /*
     * After extract_char the ch is no longer valid!
     */
    save_char_obj( ch );
    d = ch->desc;
    extract_char( ch, TRUE );
    if ( d )
	close_socket( d );

    return;
}



void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( ch->level < 2 )
    {
	send_to_char( "You must be at least second level to save.\r\n", ch );
	return;
    }

    save_char_obj( ch );
    send_to_char( "Ok.\r\n", ch );
    return;
}



void do_follow( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Follow whom?\r\n", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master )
    {
	act( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	if ( !ch->master )
	{
	    send_to_char( "You already follow yourself.\r\n", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }

    if ( ( ch->level - victim->level < -5
	 /*  || ch->level - victim->level >  5 */ )
	&& !IS_HERO( ch ) )
    {
	send_to_char( "You are not of the right caliber to follow.\r\n", ch );
	return;
    }

    if ( ( ( (ch->alignment - victim->alignment) < -900 ) ||
	( ( ch->alignment - victim->alignment) > 900 ) ) && !IS_HERO( ch ) )
    {
	send_to_char( "You are not of the right alignment to follow.\r\n", ch);
	return;
    }

    if ( ch->master )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}

void do_lose( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Lose whom?\r\n", ch );
        return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
        send_to_char( "They aren't here.\r\n", ch );
        return;
    }

    if ( victim == ch )
    {
        if ( !ch->master )
        {
            send_to_char( "You can't lose yourself.\r\n", ch );
            return;
        }
        return;
    }

    if (victim->master != ch)
    {
	send_to_char( "That player is not following you.\r\n", ch);
	return;
    }

    /*	This code came from drop_follower, but it's modified a bit
	to make it more apropriate for the lose command */
    if ( !victim->master )
    {
        bug( "Stop_follower: null master.", 0 );
        return;
    }

    if ( IS_AFFECTED( victim, AFF_CHARM ) )
    {
        REMOVE_BIT( victim->affected_by, AFF_CHARM );
        affect_strip( victim, gsn_charm_person );
        affect_strip( victim, gsn_domination   );
    }

    if ( can_see( victim->master, victim ) )
        act( "You lost $n.",
            victim, NULL, victim->master, TO_VICT );
    act( "$N doesn't want you following $M around, lose it buster.",
        victim, NULL, victim->master, TO_CHAR );

    victim->master = NULL;
    victim->leader = NULL;
    return;
}

void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
  
    if ( ch->master )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    if ( can_see( master, ch ) )
	act( "$n now follows you.", ch, NULL, master, TO_VICT );

    act( "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{

    if ( !ch->master )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
	affect_strip( ch, gsn_domination   );
    }

    if ( can_see( ch->master, ch ) )
	act( "$n stops following you.",
	    ch, NULL, ch->master, TO_VICT );
    act( "You stop following $N.",
	ch, NULL, ch->master, TO_CHAR );

    ch->master = NULL;
    ch->leader = NULL;
    return;
}



void die_follower( CHAR_DATA *ch, char *name )
{
    CHAR_DATA *fch;

    if ( ch->master )
	stop_follower( ch );

    ch->leader = NULL;

    for ( fch = char_list; fch; fch = fch->next )
    {
        if ( fch->deleted )
	    continue;
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = NULL;
    }

    return;
}



void do_order( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    char       arg [ MAX_INPUT_LENGTH ];
    bool       found;
    bool       fAll;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Order whom to do what?\r\n", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You feel like taking, not giving, orders.\r\n", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\r\n", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\r\n", ch );
	    return;
	}

	if ( !IS_AFFECTED( victim, AFF_CHARM ) || victim->master != ch )
	{
	    send_to_char( "Do it yourself!\r\n", ch );
	    return;
	}
    }

    found = FALSE;
    for ( och = ch->in_room->people; och; och = och_next )
    {
        och_next = och->next_in_room;

        if ( och->deleted )
	    continue;

	if ( IS_AFFECTED( och, AFF_CHARM )
	    && och->master == ch
	    && ( fAll || och == victim ) )
	{
	    found = TRUE;
	    act( "$n orders you to '$t'.", ch, argument, och, TO_VICT );
	    interpret( och, argument );
	}
    }

    if ( found )
	send_to_char( "Ok.\r\n", ch );
    else
	send_to_char( "You have no followers here.\r\n", ch );
    return;
}



void do_group( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = ( ch->leader ) ? ch->leader : ch;
	sprintf( buf, "%s's group:\r\n", PERS( leader, ch ) );
	send_to_char( buf, ch );

	for ( gch = char_list; gch; gch = gch->next )
	{
	    if ( gch->deleted )
	        continue;
	    if ( is_same_group( gch, ch ) )
	    {
		sprintf( buf,
		"[%2d %s] %-16s %4d/%4d hp %4d/%4d mana %4d/%4d mv %5d xp\r\n",
			gch->level,
			IS_NPC( gch ) ? "Mob"
			              : (char *)class_table[gch->class].who_name,
			capitalize( PERS( gch, ch ) ),
			gch->hit,   gch->max_hit,
			gch->mana,  gch->max_mana,
			gch->move,  gch->max_move,
			gch->exp );
		send_to_char( buf, ch );
	    }
	}
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( ch->master || ( ch->leader && ch->leader != ch ) )
    {
	send_to_char( "But you are following someone else!\r\n", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
	act( "$N isn't following you.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
	act( "You remove $N from your group.", ch, NULL, victim, TO_CHAR    );
	act( "$n removes you from $s group.",  ch, NULL, victim, TO_VICT    );
	act( "$n removes $N from $s group.",   ch, NULL, victim, TO_NOTVICT );
	return;
    }

    if (   ch->level - victim->level < -5
	|| ch->level - victim->level >  5 )
    {
	act( "$N cannot join your group.",  ch, NULL, victim, TO_CHAR       );
	act( "You cannot join $n's group.", ch, NULL, victim, TO_VICT       );
	act( "$N cannot join $n's group.",  ch, NULL, victim, TO_NOTVICT    );
	return;
    }

    if ( ch->alignment - victim->alignment < -700 ||
	ch->alignment - victim->alignment > 700 )
    {
        act( "$N cannot join your group.",  ch, NULL, victim, TO_CHAR       );
        act( "You cannot join $n's group.", ch, NULL, victim, TO_VICT       );
        act( "$N cannot join $n's group.",  ch, NULL, victim, TO_NOTVICT    );
        return;
    }


    victim->leader = ch;
    act( "$N joins your group.", ch, NULL, victim, TO_CHAR    );
    act( "You join $n's group.", ch, NULL, victim, TO_VICT    );
    act( "$N joins $n's group.", ch, NULL, victim, TO_NOTVICT );
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH  ];
    int        members;
    int        amount;
    int        share;
    int        extra;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Split how much?\r\n", ch );
	return;
    }
    
    amount = atoi( arg );

    if ( amount < 0 )
    {
	send_to_char( "Your group wouldn't like that.\r\n", ch );
	return;
    }

    if ( amount == 0 )
    {
	send_to_char( "You hand out zero coins, but no one notices.\r\n", ch );
	return;
    }

    if ( ch->gold < amount )
    {
	send_to_char( "You don't have that much gold.\r\n", ch );
	return;
    }
  
    members = 0;
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if ( gch->deleted )
	    continue;
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\r\n", ch );
	return;
    }
	    
    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\r\n", ch );
	return;
    }

    ch->gold -= amount;
    ch->gold += share + extra;

    sprintf( buf,
	"You split %d gold coins.  Your share is %d gold coins.\r\n",
	amount, share + extra );
    send_to_char( buf, ch );

    sprintf( buf, "$n splits %d gold coins.  Your share is %d gold coins.",
	amount, share );

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if ( gch->deleted )
	    continue;
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    act( buf, ch, NULL, gch, TO_VICT );
	    gch->gold += share;
	}
    }

    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    char       buf [ MAX_STRING_LENGTH ];

    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\r\n", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
        send_to_char( "You can't seem to break the silence.\r\n", ch );
        return;
    }

    if ( IS_SET( ch->act, PLR_NO_TELL ) )
    {
	send_to_char( "Your message didn't get through!\r\n", ch );
	return;
    }

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
    sprintf( buf, "%s tells the group '%s'.\r\n", ch->name, argument );
    for ( gch = char_list; gch; gch = gch->next )
    {
	if ( is_same_group( gch, ch )
	    && !IS_SET( gch->in_room->room_flags, ROOM_CONE_OF_SILENCE )
	    && !IS_AFFECTED( gch, AFF_MUTE ) )
	    send_to_char( buf, gch );
    }

    return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach->deleted || bch->deleted )
        return FALSE;
/*
    if ( ach->leader ) ach = ach->leader;
    if ( bch->leader ) bch = bch->leader; */

    while (ach->leader && ach->leader != ach) ach=ach->leader;
    while (bch->leader && bch->leader != bch) bch=bch->leader;

    return ach == bch;
}

/*
 * this function sends raw argument over the AUCTION: channel
 * I am not too sure if this method is right..
 */

void talk_auction (char *argument)
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *original;

    sprintf (buf,"AUCTION: %s", argument);

    for (d = descriptor_list; d != NULL; d = d->next)
    {
        original = d->original ? d->original : d->character; /* if switched */
        if ((d->connected == CON_PLAYING) && !IS_SET(original->deaf,CHANNEL_AUCTION) )
            act (buf, original, NULL, NULL, TO_CHAR);

    }
}
