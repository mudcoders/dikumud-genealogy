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

/*
 * Lookup a language by name.
 */
int lang_lookup( const char *name )
{
    int ln;

    for ( ln = 0; ln < MAX_LANGUAGE; ln++ )
    {
        if ( !lang_table[ln].name )
            break;
        if ( LOWER( name[0] ) == LOWER( lang_table[ln].name[0] )
            && !str_prefix( name, lang_table[ln].name ) )
            return ln;
    }

    return -1;
}

void do_speak( CHAR_DATA *ch, char *argument )
{
	char	arg [ MAX_INPUT_LENGTH ];
	char	buf [ MAX_STRING_LENGTH];
	int	speaking;
	int	canspeak;

	argument = one_argument(argument, arg);

	buf[0] = '\0';

	if (IS_NPC(ch))
	{
		send_to_char ("Mobiles can't speak !\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		sprintf (buf, "You currently speak %s.\n\r", lang_table[ch->pcdata->speaking].name);
		send_to_char (buf, ch);
	}
	else
	{
		if ((speaking = lang_lookup(arg)) != -1)
		{
			if ((canspeak = ch->pcdata->language[speaking]) == 0)
			{
				sprintf (buf, "But you don't know how to speak %s.\n\r", lang_table[speaking].name);
				send_to_char(buf, ch);
			}
			else
			{
				ch->pcdata->speaking = speaking;
				sprintf (buf, "You will speak %s from now on.\n\r", lang_table[ch->pcdata->speaking].name);
				send_to_char(buf, ch);
			}
		}
		else
		{
			sprintf (buf, "%s is not a valid language!\n\r", arg);
			send_to_char( buf, ch);
		}
	}
}
		
void do_lset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1  [ MAX_INPUT_LENGTH ];
    char       arg2  [ MAX_INPUT_LENGTH ];
    char       arg3  [ MAX_INPUT_LENGTH ];
    int        value;
    int        ln;
    bool       fAll;

    rch = get_char( ch );

    if ( !authorized( rch, "lset" ) )
        return;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char( "Syntax: lset <victim> <lang> <value>\n\r",	ch );
        send_to_char( "or:     lset <victim> all    <value>\n\r",	ch );
	send_to_char( "or:     lset <victim> learn  <value>\n\r",	ch );
        send_to_char( "Lang being any language.\n\r",			ch );
        return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( ch->level <= victim->level && ch != victim )
    {
        send_to_char( "You may not lset your peer nor your superior.\n\r", ch );
        return;
    }

    if (!str_prefix(arg2, "learn") )
    {
	    if (!is_number(arg3))
	    {
		send_to_char ("Value must be numeric.\n\r", ch);
		return;
	    }
	    value = atoi(arg3);

	    if (value < 0)
		    value = 0;
	    if (value > 100)
		    value = 100;
	    victim->pcdata->learn = value;
	    send_to_char("OK\n\r", ch);
	    return;
    }

    fAll = !str_cmp( arg2, "all" );
    ln   = 0;
    if ( !fAll && ( ln = lang_lookup( arg2 ) ) < 0 )
    {
        send_to_char( "No such language.\n\r", ch );
        return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
        send_to_char( "Value must be numeric.\n\r", ch );
        return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
        send_to_char( "Value range is 0 to 100.\n\r", ch );
        return;
    }

    if ( fAll )
    {
        if ( get_trust( ch ) < L_SEN )
        {
            send_to_char( "Only Seniors may lset all.\n\r", ch );
            return;
        }
        for ( ln = 0; ln < MAX_LANGUAGE; ln++ )
            if ( lang_table[ln].name )
                victim->pcdata->language[ln] = value;
    }
    else
    {
        victim->pcdata->language[ln] = value;
    }
    return;
}

void do_lstat( CHAR_DATA *ch, char *argument )
/* lstat by Maniac && Canth */
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int ln;
    int col;

    one_argument( argument, arg );
    col = 0;

    if ( arg[0] == '\0' )
    {
        send_to_char("lstat whom\n\r?", ch);
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char("That person isn't logged on.\n\r", ch);
        return;
    }

    if ( IS_NPC( victim ) )
    {
        send_to_char("Not on NPC's.\n\r", ch);
        return;
    }

    buf2[0] = '\0';

    for ( ln = 0; ln < MAX_LANGUAGE ; ln++ )
    {
        if ( lang_table[ln].name == NULL )
            break;
        sprintf( buf1, "%18s %3d %% ", lang_table[ln].name,
                victim->pcdata->language[ln] );
        strcat( buf2, buf1 );
        if ( ++col %3 == 0 )
            strcat( buf2, "\n\r" );
    }
    if ( col % 3 != 0 )
         strcat( buf2, "\n\r" );
    sprintf( buf1, "%s has %d learning sessions left.\n\r", victim->name,
                victim->pcdata->learn );
    strcat( buf2, buf1 );

    send_to_char( buf2, ch );
    return;

}

void do_learn( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH   ];
    char buf1 [ MAX_STRING_LENGTH*2 ];
    int  ln;
    int  money = ch->level * ch->level * 20;

    if ( IS_NPC( ch ) )
        return;

    buf1[0] = '\0';

    if ( argument[0] == '\0' )
    {
        CHAR_DATA *mob;
        int        col;

        for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
        {
            if ( mob->deleted )
                continue;
            if ( IS_NPC( mob ) && IS_SET( mob->act, ACT_TEACHER ) )
                break;
        }

        col    = 0;
        for ( ln = 0; ln < MAX_LANGUAGE; ln++ )
        {
            if ( !lang_table[ln].name )
                break;

            if ( ( mob ) || ( ch->pcdata->language[ln] > 0 ) )
            {
                sprintf( buf, "%18s %3d%%  ",
                        lang_table[ln].name, ch->pcdata->language[ln] );
                strcat( buf1, buf );
                if ( ++col % 3 == 0 )
                    strcat( buf1, "\n\r" );
            }
        }

        if ( col % 3 != 0 )
            strcat( buf1, "\n\r" );

        sprintf( buf, "You have %d learning sessions left.\n\r",
                ch->pcdata->learn);
        strcat( buf1, buf );
        sprintf( buf, "Cost of lessons is %d gold coins.\n\r", money );
        strcat( buf1, buf );
        send_to_char( buf1, ch );
    }
    else
    {
        CHAR_DATA *mob;
        int        adept;

        if ( !IS_AWAKE( ch ) )
        {
            send_to_char( "In your dreams, or what?\n\r", ch );
            return;
        }

        for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
        {
            if ( mob->deleted )
                continue;
            if ( IS_NPC( mob ) && IS_SET( mob->act, ACT_TEACHER ) )
                break;
        }

        if ( !mob )
        {
            send_to_char( "You can't do that here.\n\r", ch );
            return;
        }

        if ( ch->pcdata->learn <= 0 )
        {
            send_to_char( "You have no lessons left.\n\r", ch );
            return;
        }
        else if ( money > ch->gold )
        {
            send_to_char( "You don't have enough money to take lessons.\n\r", ch );
            return;
        }

        if (( ln = lang_lookup( argument ) ) < 0)
        {
            send_to_char( "That's not a language.\n\r", ch );
            return;
        }

        adept = (get_curr_int(ch) * 5);		/* Max learned = int*5 */
	if ( ch->level < LEVEL_HERO )		/* Max is 95% */
		if ( adept > 95 )
			adept = 95;
	else					/* Hero and higher have */
		if ( adept > 100 )		/* a MAX of 100% */
			adept = 100;

        if ( ch->pcdata->language[ln] >= adept )
        {
            sprintf( buf, "You are already fluent in %s.\n\r",
                lang_table[ln].name );
            send_to_char( buf, ch );
        }
        else
        {
            ch->pcdata->learn--;
            ch->gold                -= money;
            ch->pcdata->language[ln] += int_app[get_curr_int( ch )].learn;
            if ( ch->pcdata->language[ln] < adept )
            {
                act( "You take lessons in $T.",
                    ch, NULL, lang_table[ln].name, TO_CHAR );
                act( "$n practices $T.",
                    ch, NULL, lang_table[ln].name, TO_ROOM );
            }
            else
            {
                ch->pcdata->language[ln] = adept;
                act( "You are now fluent in $T.",
                    ch, NULL, lang_table[ln].name, TO_CHAR );
                act( "$n is now fluent in $T.",
                    ch, NULL, lang_table[ln].name, TO_ROOM );
            }
        }
    }
    return;
}

void do_common( CHAR_DATA *ch, char *argument)
{
	do_language(ch, argument, COMMON);
}

void do_human( CHAR_DATA *ch, char *argument)
{
	do_language(ch,argument, HUMAN);
}

void do_dwarvish( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, DWARVISH);
}

void do_elvish( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, ELVISH);
}

void do_gnomish( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, GNOMISH);
}

void do_goblin( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, GOBLIN);
}

void do_orcish( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, ORCISH);
}

void do_ogre( CHAR_DATA *ch, char *argument)
{
	do_language(ch,argument, OGRE);
}

void do_drow( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, DROW);
}

void do_kobold( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, KOBOLD);
}

void do_trollish( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, TROLLISH);
}

void do_hobbit( CHAR_DATA *ch, char *argument)
{
        do_language(ch,argument, HOBBIT);
}

/* ========== Language ======================== */
void do_language( CHAR_DATA *ch, char *argument, int language)
{
	CHAR_DATA	*och;
	int		chance;
	int		chance2;
	char		*lan_str;
	char		buf	[ 256 ];

	lan_str = lang_table[language].name;

	buf[0] = '\0';
	/* Now find out if we can speak it... */
	if((chance = ch->pcdata->language[language]) == 0)
	{
		sprintf(buf, "You don't know how to speak %s.\n\r", lan_str);
		send_to_char(buf ,ch);
		return;
	}

	if(argument[0] == '\0')
	{
		buf[0] = '\0';
		sprintf(buf, "Say WHAT in %s ??\n\r", lan_str);
		send_to_char(buf ,ch);
		return;
	}

	if (!IS_NPC(ch))
		argument = makedrunk(argument,ch);

	if(number_percent( ) <= chance )
	{
	   buf[0] = '\0';
	   sprintf (buf,"In %s, you say '%s'\n\r", lan_str, argument);
	   send_to_char(buf, ch);
	   for(och = ch->in_room->people; och != NULL; och = och->next_in_room )
	   {
            if(!IS_NPC(och) && (och != ch))
	    {
	    if((chance2 = och->pcdata->language[language]) == 0)
	      act("$n says something in a strange tongue.",ch,NULL,och,TO_VICT);
	    else
		if(number_percent( ) <= chance2)
		{
		   buf[0] = '\0';
		   sprintf (buf, "In %s, %s says, '%s'\n\r", lan_str, ch->name, argument);
		   send_to_char(buf, och);
		}
		else
		{
		   buf[0] = '\0';
		   sprintf (buf, "In %s, %s says something you can't understand.\n\r", lan_str, ch->name);
		   send_to_char(buf, och);
		}
             }
	   }
	}
	else
	{
	   buf[0] = '\0';
	   sprintf (buf, "In %s, you try to say '%s', but it doesn't sound correct.\n\r", lan_str, argument);
	   send_to_char(buf, ch);
           for(och = ch->in_room->people; och != NULL; och = och->next_in_room )
           {
            if(!IS_NPC(och) && (och != ch))
            {
            if((chance2 = och->pcdata->language[language]) == 0)
              act("$n says something in a strange tongue.",ch,NULL,och,TO_VICT);
	    else
                if(number_percent( ) <= chance2)
		{
		   buf[0] = '\0';
		   sprintf (buf, "In a weird form of %s, %s says something uncomprehensible.\n\r", lan_str, ch->name);
                   send_to_char(buf, och);
		}
                else
		{
		   buf[0] = '\0';
		   sprintf (buf, "In %s, %s says something you can't understand.\n\r", lan_str, ch->name);
		   send_to_char(buf, och);
		}
             }
           }
	}
}
