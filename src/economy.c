/*
 * The Mythran Mud Economy Snippet Version 2 (used to be banking.c)
 *
 * Copyrights and rules for using the economy system:
 *
 *	The Mythran Mud Economy system was written by The Maniac, it was
 *	loosly based on the rather simple 'Ack!'s banking system'
 *
 *	If you use this code you must follow these rules.
 *		-Keep all the credits in the code.
 *		-Mail Maniac (v942346@si.hhs.nl) to say you use the code
 *		-Send a bug report, if you find 'it'
 *		-Credit me somewhere in your mud.
 *		-Follow the envy/merc/diku license
 *		-If you want to: send me some of your code
 *
 * All my snippets can be found on http://www.hhs.nl/~v942346/snippets.html
 * Check it often because it's growing rapidly	-- Maniac --
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


int	share_value = SHARE_VALUE;	/* External share_value by Maniac */

void do_bank( CHAR_DATA *ch, char *argument )
{
	/* The Mythran mud economy system (bank and trading)
	*
	* based on:
	* Simple banking system. by -- Stephen --
	*
	* The following changes and additions where
	* made by the Maniac from Mythran Mud
	* (v942346@si.hhs.nl)
	*
	* History:
	* 18/05/96:	Added the transfer option, enables chars to transfer
	*		money from their account to other players' accounts
        * 18/05/96:	Big bug detected, can deposit/withdraw/transfer
	*		negative amounts (nice way to steal is
	*		bank transfer -(lots of dogh) <some rich player>
	*		Fixed it (thought this was better... -= Maniac =-)
	* 21/06/96:	Fixed a bug in transfer (transfer to MOBS)
	*		Moved balance from ch->balance to ch->pcdata->balance
	* 21/06/96:	Started on the invest option, so players can invest
	*		money in shares, using buy, sell and check
	*		Finished version 1.0 releasing it monday 24/06/96
	* 24/06/96:	Mythran Mud Economy System V1.0 released by Maniac
	*
	*/
    
	CHAR_DATA *mob;
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	if ( IS_NPC( ch ) )
	{
		send_to_char( "Banking Services are only available to players!\r\n", ch );
		return;
	}
  
	/* Check for mob with act->banker */
	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
		if ( IS_NPC(mob) && IS_SET(mob->act, ACT_BANKER ) )
			break;
	}
 
	if ( mob == NULL )
	{
		send_to_char( "You can't do that here.\r\n", ch );
		return;
	}

	if ((time_info.hour < 9) || (time_info.hour > 17))
	{
		send_to_char( "The bank is closed, it is open from 9 to 5.\r\n", ch);
		return;
	}

	if ( argument[0] == '\0' )
	{
		send_to_char( "Bank Options:\r\n\r\n", ch );
		send_to_char( "Bank balance: Displays your balance.\r\n", ch );
		send_to_char( "Bank deposit <amount>: Deposit gold into your account.\r\n", ch );
		send_to_char( "Bank withdraw <amount>: Withdraw gold from your account.\r\n", ch );
#if defined BANK_TRANSFER
		send_to_char( "Bank transfer <amount> <player>: Transfer <amount> gold to <player> account.\r\n", ch); 
		send_to_char( "Bank buy #: Buy # shares (in developement)\r\n", ch);
		send_to_char( "Bank sell #: Sell # shares (in developement)\r\n", ch);
		send_to_char( "Bank check: Check the current rates of the shares. (in developement)\r\n", ch);
#endif
		return;
	}

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
   
	/* Now work out what to do... */
	if ( !str_prefix( arg1, "balance" ) )
	{
		sprintf(buf,"Your current balance is: %d GP.",ch->pcdata->balance );
		do_say(mob, buf);
		return;
	}

	if ( !str_prefix( arg1, "deposit" ) )
	{
		int amount; 

		if ( is_number ( arg2 ) )
		{
			amount = atoi( arg2 );
			if (amount > ch->gold )
			{
				sprintf( buf, "How can you deposit %d GP when you only have %d?", amount, ch->gold );
				do_say(mob, buf );
				return;
			}

			if (amount < 0 )
			{
				do_say (mob, "Only positive amounts allowed...");
				return;
			}

			ch->gold -= amount;
			ch->pcdata->balance += amount;
			sprintf ( buf, "You deposit %d GP.  Your new balance is %d GP.\r\n",  
			amount, ch->pcdata->balance );
			send_to_char( buf, ch );
			do_save( ch, "" );
			return;
		}
	}

	/* We only allow transfers if this is true... so define it... */

#if defined BANK_TRANSFER
	if ( !str_prefix( arg1, "transfer" ) )
	{
		int amount;
		CHAR_DATA *victim;

		if ( is_number ( arg2 ) )
		{
			amount = atoi( arg2 );
			if ( amount > ch->pcdata->balance )
			{
				sprintf( buf, "How can you transfer %d GP when your balance is %d?",
				amount, ch->pcdata->balance );
				do_say( mob, buf);
				return;
			}

                        if (amount < 0 )
                        {
                                do_say (mob, "Only positive amounts allowed...");
                                return;
                        }


			if ( !( victim = get_char_world( ch, argument ) ) )
			{
				sprintf (buf, "%s doesn't have a bank account.", argument );
				do_say( mob, buf );
				return;
			}

			if (IS_NPC(victim))
			{
				do_say( mob, "You can only transfer money to players.");
				return;
			}

			ch->pcdata->balance     -= amount;
 			victim->pcdata->balance += amount;
			sprintf( buf, "You transfer %d GP. Your new balance is %d GP.\r\n",
			amount, ch->pcdata->balance );
			send_to_char( buf, ch );
			sprintf (buf, "[BANK] %s has transferred %d gold's to your account.\r\n", ch->name, amount);
			send_to_char( buf, victim );
			do_save( ch, "" );
			do_save( victim, "");
			return;
		}
	}
#endif

	if ( !str_prefix( arg1, "withdraw" ) )
	{
		int amount; 

		if ( is_number ( arg2 ) )
		{
			amount = atoi( arg2 );
			if ( amount > ch->pcdata->balance )
			{
				sprintf( buf, "How can you withdraw %d GP when your balance is %d?",
				amount, ch->pcdata->balance );
				do_say (mob, buf );
				return;
			}

                        if (amount < 0 )
                        {
                                do_say( mob, "Only positive amounts allowed...");
                                return;
                        }

			ch->pcdata->balance -= amount;
			ch->gold += amount;
			sprintf( buf, "You withdraw %d GP.  Your new balance is %d GP.", amount, ch->pcdata->balance );
			send_to_char( buf, ch );
			do_save( ch, "" );
			return;
 		}
	}

	/* If you want to have an invest option... define BANK_INVEST */

#if defined BANK_INVEST
        if ( !str_prefix( arg1, "buy" ) )
        {
                int amount;
		if (share_value < 1)
		{
			do_say (mob, "There is something wrong with shares, notify the GODS.");
			return;
		}

                if ( is_number ( arg2 ) )
                {
                        amount = atoi( arg2 );
                        if ( (amount * share_value) > ch->pcdata->balance )
                        {
                                sprintf( buf, "%d shares will cost you %d, get more money.", amount, (amount * share_value) );
                                do_say(mob, buf);
                                return;
                        }

                        if (amount < 0 )
                        {
                                do_say(mob, "If you want to sell shares you have to say so...");
                                return;
                        }

                        ch->pcdata->balance -= (amount * share_value);
                        ch->pcdata->shares  += amount;
                        sprintf( buf, "You buy %d shares for %d GP, you now have %d shares.", amount, (amount * share_value), ch->pcdata->shares );
                        do_say(mob, buf);
                        do_save( ch, "" );
                        return;
                }
        }

        if ( !str_prefix( arg1, "sell" ) )
        {
                int amount;

		if (share_value < 1)
		{
			do_say (mob, "There is something wrong with the shares, notify the GODS.");
			return;
		}

                if ( is_number ( arg2 ) )
                {
                        amount = atoi( arg2 );
                        if ( amount > ch->pcdata->shares )
                        {
                                sprintf( buf, "You only have %d shares.", ch->pcdata->shares );
                                do_say(mob, buf);
                                return;
                        }

                        if (amount < 0 )
                        {
                                do_say (mob, "If you want to buy shares you have to say so...");
                                return;
                        }

                        ch->pcdata->balance += (amount * share_value);
                        ch->pcdata->shares  -= amount;
                        sprintf( buf, "You sell %d shares for %d GP, you now have %d shares.", amount, (amount * share_value), ch->pcdata->shares );
                        do_say (mob, buf);
                        do_save( ch, "" );
                        return;
                }
        }

        if ( !str_prefix( arg1, "check" ) )
        {
		sprintf (buf, "The current shareprice is %d.",share_value);
		do_say(mob, buf);
		if (ch->pcdata->shares)
		{
		    sprintf (buf, "You have %d shares, (%d a share) worth totally %d gold.",
			ch->pcdata->shares, share_value, (ch->pcdata->shares * share_value) );
                    do_say(mob, buf);
		}
                return;
        }
#endif

	do_say(mob, "I don't know what you mean");
	do_bank( ch, "" );		/* Generate Instructions */
	return;
}
