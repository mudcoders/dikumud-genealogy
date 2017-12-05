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

 /* Games.c was written by maniac to support the GAMES section in the
    Envy 2 area format... it was discribed in the area.txt but the code
    was not implemented... so i wrote my own... -- Maniac --

    This stuff is (C) 1996 The Maniac from Mythran Mud

    snippets can be found at my snippets page:
    	http://www.hhs.nl/~v942346/snippets/snippets.html

    Well... so much for the legal bullshit... now the code... */

/* Games: Version 2.0, now has support for cheating and bankrolls
	Written by The Maniac from Mythran Mud */

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
 * The following game functions are available for the mud.
 */
DECLARE_GAME_FUN( game_u_l_t     );
DECLARE_GAME_FUN( game_high_dice );
DECLARE_GAME_FUN( game_seven     );

/*
 * GAME Functions Table.     OLC, Maniac
 */
const   struct  game_type       game_table      [ ] =
{
    /*
     * game function commands.
     */
    { "game_u_l_t",		game_u_l_t		},
    { "game_high_dice",		game_high_dice		},
    { "game_seven",		game_seven		},

    /*
     * End of list.
     */
    { "",                       0       }
};


/*
 * Given a name, return the appropriate spec fun.
 */
GAME_FUN *game_lookup( const char *name )       /* OLC, Maniac */
{
    int cmd;

    for ( cmd = 0; *game_table[cmd].game_name; cmd++ )  /* OLC 1.1b */
        if ( !str_cmp( name, game_table[cmd].game_name ) )
            return game_table[cmd].game_fun;

    return 0;
}



char *game_string( GAME_FUN *fun )      /* OLC */
{
    int cmd;

    for ( cmd = 0; *game_table[cmd].game_fun; cmd++ )   /* OLC 1.1b */
        if ( fun == game_table[cmd].game_fun )
            return game_table[cmd].game_name;

    return 0;
}



/*
 *  Menu for all game functions.
 *  Thelonius (Monk)  5/94
 *  Modified for use with Maniac's game system by Maniac from Mythran
 */

void do_bet( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *	croupier;
    char	arg[MAX_INPUT_LENGTH];
    int		amount;
    int		cheat = FALSE;

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
        send_to_char( "You can't seem to break the silence.\r\n", ch );
        return;
    }

    for( croupier = ch->in_room->people; croupier;
        croupier = croupier->next_in_room )
    {
        if ( IS_NPC( croupier )
            && (croupier->game_fun != 0 )
            && !IS_AFFECTED( croupier, AFF_MUTE )
            && croupier != ch )
            break;
    }

    if ( !croupier )
    {
        send_to_char( "You can't gamble here.\r\n", ch );
        return;
    }

    /* I have added some more check's here, so we don't have to include
	them in every single game fun we write... -- Maniac -- */

    /* First check for the arguments */
    argument = one_argument (argument, arg);
    if (arg[0] == '\0')
    {
	send_to_char ("Bet <amount> [option].\r\n", ch );
	send_to_char ("option can be: (shortcut)\r\n", ch );
	send_to_char ("upper-lower-triple: upper(+), lower(-) or triple(3)\r\n", ch );
	send_to_char ("seven: under(-), over(+) or seven(7)\r\n", ch );
	send_to_char ("\r\nFor more info see the help for games\r\n", ch );
	return;
    }
    amount = advatoi (arg);

    if (amount < 0)
    {
	send_to_char ("Only positive amounts allowed.\r\n", ch );
	return;
    }

    if (amount > ch->gold)
    {
	send_to_char ("You don't have enough money.\r\n", ch );
	return;
    }

    if (amount > croupier->gold )
    {
	act( "$n tells you, 'Sorry, I don't have that much money.'",
	    croupier, NULL, ch, TO_VICT );
	ch->reply = croupier;
	return;
    }

    if (amount > BET_LIMIT)
    {
	act( "$n tells you, 'Sorry, the house limit is $t.'",
	    croupier, BET_LIMIT_STR, ch, TO_VICT );
	ch->reply = croupier;
	return;
    }

#if defined (GAME_CHEAT)
    cheat = MOB_GAME_CHEAT(croupier);
#else
    cheat = FALSE;
#endif

    /* Now for the new game approach... by Maniac */
    if ( croupier->game_fun == game_lookup( "game_u_l_t" ) )
	game_u_l_t	( ch, croupier, amount, cheat, argument );
    if ( croupier->game_fun == game_lookup( "game_high_dice" ) )
	game_high_dice	( ch, croupier, amount, cheat, argument );
    if ( croupier->game_fun == game_lookup( "game_seven" ) )
	game_seven	( ch, croupier, amount, cheat, argument );

    return;
}

/*
 * Upper-Lower-Triple
 * Game idea by Partan
 * Coded by Thelonius
 */
void game_u_l_t( CHAR_DATA *ch, CHAR_DATA *croupier,
	int amount, int cheat, char *argument )
{
    char msg    [ MAX_STRING_LENGTH ];
    char buf    [ MAX_STRING_LENGTH ];
    char wager  [ MAX_INPUT_LENGTH  ];
    char choice [ MAX_INPUT_LENGTH  ];
    int  ichoice;
    int  die1;
    int  die2;
    int  die3;
    int  total;

    sprintf (wager, "%d", amount);

    one_argument( argument, choice );

         if ( !str_cmp( choice, "lower"  ) ) ichoice = 1;
    else if ( !str_cmp( choice, "upper"  ) ) ichoice = 2;
    else if ( !str_cmp( choice, "triple" ) ) ichoice = 3;
    else
    {
        send_to_char( "What do you wish to bet: Upper, Lower, or Triple?\r\n",
                     ch );
        return;
    }
/*
 *  Now we have a wagering amount, and a choice.
 *  Let's place the bets and roll the dice, shall we?
 */
    act( "You place your gold coins on the table, and bet '$T'.",
        ch, NULL, choice,   TO_CHAR    );
    act( "$n places a bet with you.",
        ch, NULL,  croupier, TO_VICT    );
    act( "$n plays a dice game.",
        ch, NULL,  croupier, TO_NOTVICT );
    ch->gold -= amount;

    die1 = number_range( 1, 6 );
    die2 = number_range( 1, 6 );
    die3 = number_range( 1, 6 );
    total = die1 + die2 + die3;

    sprintf( msg, "$n rolls the dice: they come up %d, %d, and %d",
            die1, die2, die3 );

    if( die1 == die2 && die2 == die3 )
    {
        strcat( msg, "." );
        act( msg, croupier, NULL, ch, TO_VICT );

        if ( ichoice == 3 )
        {
            char haul [ MAX_STRING_LENGTH ];

            amount *= 37;
            sprintf( haul, "%d", amount );
            act( "It's a TRIPLE!  You win $t gold coins!",
                ch, haul, NULL, TO_CHAR );
            ch->gold += amount;
        }
        else
            send_to_char( "It's a TRIPLE!  You lose!\r\n", ch );

        return;
    }

    sprintf( buf, ", totalling %d.", total );
    strcat( msg, buf );
    act( msg, croupier, NULL, ch, TO_VICT );

    if (   ( ( total <= 10 ) && ( ichoice == 1 ) )
        || ( ( total >= 11 ) && ( ichoice == 2 ) ) )
    {
        char haul [ MAX_STRING_LENGTH ];

        amount *= 2;
        sprintf( haul, "%d", amount );
        act( "You win $t gold coins!", ch, haul, NULL, TO_CHAR );
        ch->gold += amount;
    }
    else
        send_to_char( "Sorry, better luck next time!\r\n", ch );

    return;
}

/*
 * High-Dice
 * Game idea by The Maniac!
 * Coded by The Maniac (based on code from Thelonius for u_l_t)
 *
 * The croupier roll's 2 dice for the player, then he roll's 2 dice
 * for himself. If the player's total is higher he wins his money*2
 * if the player's total is equal to or lower than the croupier's total
 * the bank wins... and the player loses his money
 */
void game_high_dice( CHAR_DATA *ch, CHAR_DATA *croupier,
	int amount, int cheat, char *argument )
{
    char msg    [ MAX_STRING_LENGTH ];
    char buf    [ MAX_STRING_LENGTH ];
    char wager  [ MAX_INPUT_LENGTH  ];
    int  die1;
    int  die2;
    int  die3;
    int  die4;
    int  total1;
    int  total2;

    sprintf (wager, "%d", amount);

/*
 *  Now we have a wagering amount...
 *  Let's place the bets and roll the dice, shall we?
 */
    switch (number_range(1, 5))
    {
	case 1:
	   strcpy ( msg, "Roll some dice");
	   break;
	case 2:
	   strcpy ( msg, "Lets see those dice");
	   break;
	case 3:
	   strcpy ( msg, "You'd better let me win !");
	   break;
	case 4:
	   strcpy ( msg, "Here's some cash, now roll 'em!");
	   break;
	case 5:
 	   strcpy ( msg, "Today's my lucky day...");
	   break;
    }
    act( "You place $t gold coins on the table, and say '$T'.",
        ch, wager, msg, TO_CHAR    );
    act( "$n places a bet with you.",
        ch, NULL,  croupier, TO_VICT    );
    act( "$n plays a dice game.",
        ch, NULL,  croupier, TO_NOTVICT );
    ch->gold -= amount;

    die1 = number_range( 1, 6 );
    die2 = number_range( 1, 6 );
    die3 = number_range( 1, 6 );
    die4 = number_range( 1, 6 );

    if (cheat)
    {
	if (die3 == 1)	/* Cheating mobs never throw one's */
		die3++;
	if (die4 == 1)
		die4++;
	/* And half of the time cheat a bit... */
	if ((die3 < 6) && (number_range(0,1)))
		die3++;
	if ((die4 < 6) && (number_range(0,1)))
		die4++;
    }

    total1 = die1 + die2;
    total2 = die3 + die4;

    sprintf( msg, "$n rolls your dice: they come up %d, and %d",
            die1, die2 );

    sprintf( buf, ", totalling %d.", total1 );
    strcat( msg, buf );
    act( msg, croupier, NULL, ch, TO_VICT );

    sprintf( msg, "$n rolls his dice: they come up %d, and %d",
            die3, die4 );

    sprintf( buf, ", totalling %d.", total2 );
    strcat( msg, buf );
    act( msg, croupier, NULL, ch, TO_VICT );


    if ( total1 > total2 )
    {
        char haul [ MAX_STRING_LENGTH ];

        amount *= 2;
        sprintf( haul, "%d", amount );
        act( "You win $t gold coins!", ch, haul, NULL, TO_CHAR );
        ch->gold += amount;
    }
    else
        send_to_char( "Sorry, better luck next time!\r\n", ch );
    return;
}

/*
 * Under and Over Seven
 * Game idea by Maniac
 * Coded by Maniac (with bits from Thelonius)
 *
 * This is a very simple and easy dice game... and the nice thing is...
 * the operator never goes broke (he practically always wins)
 * (better not tell the players...)
 * The player can choose to bet on: Under 7, Seven or Over 7
 * The croupier rolls 2d6 and pays double if the player chose under or
 * over 7 and was correct and the croupier pay's 5x the amount if the player
 * chose SEVEN and was right.
 */

void game_seven( CHAR_DATA *ch, CHAR_DATA *croupier,
	int amount, int cheat, char *argument )
{
    char msg    [ MAX_STRING_LENGTH ];
    char buf    [ MAX_STRING_LENGTH ];
    char wager  [ MAX_INPUT_LENGTH  ];
    char choice [ MAX_INPUT_LENGTH  ];
    int  ichoice;
    int  die1;
    int  die2;
    int  total;

    sprintf (wager, "%d", amount);

    one_argument( argument, choice );

         if ( !str_prefix( choice, "under" ) ) ichoice = 1;
    else if ( !str_cmp   ( choice, "-"     ) ) ichoice = 1;
    else if ( !str_prefix( choice, "over"  ) ) ichoice = 2;
    else if ( !str_cmp   ( choice, "+"     ) ) ichoice = 2;
    else if ( !str_prefix( choice, "seven" ) ) ichoice = 3;
    else if ( !str_cmp   ( choice, "7"     ) ) ichoice = 3;
    else
    {
        send_to_char( "What do you wish to bet: Under(-), Over(+), or Seven(7)?\r\n", ch );
        return;
    }

/*
 *  Now we have a wagering amount, and a choice.
 *  Let's place the bets and roll the dice, shall we?
 */
    act( "You place $t gold coins on the table, and bet '$T'.",
        ch, wager, choice,   TO_CHAR    );
    act( "$n places a bet with you.",
        ch, NULL,  croupier, TO_VICT    );
    act( "$n plays a dice game.",
        ch, NULL,  croupier, TO_NOTVICT );
    ch->gold -= amount;

    die1 = number_range( 1, 6 );
    die2 = number_range( 1, 6 );
    total = die1 + die2;

    sprintf( msg, "$n rolls the dice: they come up %d, and %d",
            die1, die2 );

    if( total == 7 )
    {
        strcat( msg, "." );
        act( msg, croupier, NULL, ch, TO_VICT );

        if ( ichoice == 3 )
        {
            char haul [ MAX_STRING_LENGTH ];

            amount *= 5;
            sprintf( haul, "%d", amount );
            act( "It's a SEVEN!  You win $t gold coins!",
                ch, haul, NULL, TO_CHAR );
            ch->gold += amount;
        }
        else
            send_to_char( "It's a SEVEN!  You lose!\r\n", ch );

        return;
    }

    sprintf( buf, ", totalling %d.", total );
    strcat( msg, buf );
    act( msg, croupier, NULL, ch, TO_VICT );

    if (   ( ( total < 7 ) && ( ichoice == 1 ) )
        || ( ( total > 7 ) && ( ichoice == 2 ) ) )
    {
        char haul [ MAX_STRING_LENGTH ];

        amount *= 2;
        sprintf( haul, "%d", amount );
        act( "You win $t gold coins!", ch, haul, NULL, TO_CHAR );
        ch->gold += amount;
    }
    else
        send_to_char( "Sorry, better luck next time!\r\n", ch );

    return;
}
