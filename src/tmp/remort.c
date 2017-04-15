/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 **************************************************************************/

/***************************************************************************
*       ROM 2.4 is copyright 1993-1996 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"

void do_remor( CHAR_DATA *ch, char *argument )
{
    send_to_char(C_DEFAULT, "If you want to REMORT, you must spell it out.\n\r", ch );
    return;
}

void do_remort( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char strsave[MAX_INPUT_LENGTH], player_name[MAX_INPUT_LENGTH];
    OBJ_DATA   *obj, *obj_next;
    RACE_DATA  *pRace;
    char       buf[MAX_STRING_LENGTH];
    int        iWear;
	char player_pwd[MAX_STRING_LENGTH];
	int player_incarnations;



    if ( IS_NPC(ch) || ( d = ch->desc ) == NULL )
	return;

    if ( ch->level < LEVEL_HERO )
    {
	sprintf( buf, "You must be level %d to remort.\n\r",
	    LEVEL_HERO );
	send_to_char(C_DEFAULT, buf, ch );
	return;
    }

    if ( ch->pcdata->confirm_remort )
    {
	if ( argument[0] != '\0' )
	{
	    send_to_char(C_DEFAULT, "Remort status removed.\n\r", ch );
	    ch->pcdata->confirm_remort = FALSE;
	    return;
	}
	else
	{

	    /*
	     * Get ready to delete the pfile, send a nice informational message.
	     */
		raw_kill( ch, ch );
		info( "%s is remorting!", (int)ch->name, 0 );
	    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	    stop_fighting( ch, TRUE );
	    send_to_char(C_DEFAULT, "&YYou have chosen to remort.  You will now be dropped in at the race\n\r", ch );
	    send_to_char(C_DEFAULT, "&Yselection section of character creation, you will keep all of your\n\r", ch );
	    send_to_char(C_DEFAULT, "&Ycurrent skills and practices. &RRemorts also gain raise points.\n\r\n\r", ch );
	    send_to_char(C_DEFAULT, "&WIn the unlikely event that you are disconnected or the MUD\n\r", ch );
	    send_to_char(C_DEFAULT, "&Wcrashes while you are creating your character, create a new character\n\r", ch );
	    send_to_char(C_DEFAULT, "&Was normal and write a note to 'immortal'.\n\r", ch );
		send_to_char(C_DEFAULT, "\n\r&G<Press Enter>\n\r", ch );
	    wiznet( "$N has remorted.", ch, NULL, WIZ_GENERAL, 0, 0 );

	    /*
	     * I quote:
	     * "After extract_char the ch is no longer valid!"
	     */
		/* info that carries over
		 */
	    sprintf( player_name, "%s", capitalize( ch->name ) );
		player_incarnations = ++ch->incarnations;
	    extract_char( ch, TRUE );

	    /*
	     * Delete the pfile, but don't boot the character.
	     * Instead, do a load_char_obj to get a new ch,
	     * saving the password, and the incarnations.  Then,
	     * set the PLR_REMORT bit and drop the player in at
	     * CON_BEGIN_REMORT.
	     */
	    unlink( strsave );
	    load_char_obj( d, player_name );
		/* rebirth here */

	    d->character->incarnations = player_incarnations;
    /* reset misc */
		d->character->affected = 0;
		d->character->affected2 = 0;
    
    d->character->pcdata->condition[COND_THIRST]  =  0;
    d->character->pcdata->condition[COND_FULL]    =  0;
    d->character->pcdata->condition[COND_DRUNK]    = 0;
    d->character->saving_throw = 0;

    /* level one stats */
    d->character->perm_hit  = 20;
    d->character->mod_hit  = 0;
    d->character->perm_mana = 100;
    d->character->mod_mana = 0;
    d->character->perm_bp   = 20;
    d->character->mod_bp = 0;
    d->character->perm_move = 100;
    d->character->mod_move = 0;
    d->character->level = 1;   
    d->character->hitroll = 0;
    d->character->damroll = 0;
    d->character->armor = 100;
#ifdef NEW_MONEY
    d->character->money.gold = d->character->money.silver = d->character->money.copper = 0;
#else
	d->character->gold = 0;
#endif
    d->character->exp = 0;
    d->character->raisepts = 0;

    /* advanced stats for remorts */

    d->character->pcdata->perm_str = 16; 
    d->character->pcdata->perm_int = 16; 
    d->character->pcdata->perm_wis = 16; 
    d->character->pcdata->perm_dex = 16; 
    d->character->pcdata->perm_con = 16; 

   /* restore */
   d->character->hit      = MAX_HIT(d->character);
   d->character->mana     = MAX_MANA(d->character);
   d->character->bp       = MAX_BP(d->character);
   d->character->move     = MAX_MOVE(d->character);

	    if( !IS_SET( d->character->act2, PLR_REMORT ) )
		SET_BIT( d->character->act2, PLR_REMORT );
	    d->connected = CON_BEGIN_REMORT;
	    return;
	}
    }

    if ( argument[0] != '\0' )
    {
	send_to_char(C_DEFAULT, "Just type remort.  No argument.\n\r", ch );
	return;
    }

    send_to_char(C_DEFAULT, "Type remort again to confirm this command.\n\r", ch );
    send_to_char(C_DEFAULT, "WARNING: This command is irreversible.\n\r", ch );
    send_to_char(C_DEFAULT, "Typing remort with an argument will &Wundo&w remort status.\n\r", ch );
    ch->pcdata->confirm_remort = TRUE;
    wiznet( "$N is contemplating remorting.",ch,NULL,WIZ_GENERAL,0,get_trust(ch));

}
