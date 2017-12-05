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
 ***************************************************************************/

/*
    This clans code was made for the HomeCooked release, it was completely
    built from the ground up, though got ideas from here and there...
    Painful! -- Zen --

    This file was written by Zen <vasc@camoes.rnl.ist.utl.pt>
 */

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
#include "olc.h"


bool is_clan( CHAR_DATA *ch )
{
    if ( !IS_NPC( ch )
	&& ch->pcdata->clan
	&& ch->pcdata->rank > RANK_EXILED )
	return TRUE;

    return FALSE;
}


void remove_from_clan( CHAR_DATA *ch )
{
    CLAN_DATA *clan;

    if ( !is_clan( ch ) )
	return;

    clan = ch->pcdata->clan;

    switch ( ch->pcdata->rank )
    {
    default:								  break;
    case RANK_CLANSMAN:							  break;
    case RANK_CLANHERO:  clan->clanheros--;				  break;
    case RANK_SUBCHIEF:  clan->subchiefs--;				  break;
    case RANK_CHIEFTAIN: free_string( clan->chieftain );		  break;
    case RANK_OVERLORD:  free_string( clan->overlord );			  break;
    }

    clan->members--;
    ch->pcdata->clan = NULL;

    return;    
}


void do_initiate( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CLAN_DATA *clan;
    OBJ_DATA  *card;
    OBJ_DATA  *ring;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Initiate whom?\n\r", ch );
        return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( !is_clan( ch )
        || ch->pcdata->rank < RANK_CHIEFTAIN
        || victim->level > ch->level )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    if ( victim == ch )
        return;

    card = get_eq_char( victim, WEAR_HOLD );
    if ( is_clan( victim )
	 || IS_NPC( victim )
         || !card
         || card->pIndexData->vnum != OBJ_VNUM_CLAN_CARD
	 || victim->pcdata->rank == RANK_EXILED
         || victim->level < 20 )
    {
        act( "$N hasn't what's required to be initiated.",
	    ch, NULL, victim, TO_CHAR );
        act( "You can't be initiated to $t!",
	    ch, clan->name, victim, TO_VICT );
        return;
    }

    if ( clan->clan_type == CLAN_GUILD && clan->class != victim->class )
	send_to_char( "You may only initiate those of your class.\n\r", ch );

    victim->pcdata->clan      = clan;
    victim->pcdata->rank      = RANK_CLANSMAN;
    clan->members++;

    ring = create_object( get_obj_index( clan->clanobj1 ), victim->level );

    if ( ring )
	obj_to_char( ring, victim );

    sprintf( buf, "Log %s: initiated %s to %s",
	    ch->name,
	    victim->name,
	    clan->name  );
    log_clan( buf );

    sprintf( buf, "'I %s %s, hereby declare you %s a member of %s!'\n\r"
		  "Take %s as a symbol of this union for all time.\n\r"
		  "Forever remember our motto: \"%s\"\n\r",
		  flag_string( rank_flags, ch->pcdata->rank ),
		  ch->name,
		  victim->name,
		  clan->name,
		  ring ? ring->short_descr : "my handshake",
		  clan->motto );

    send_to_char( buf, victim );

    act( "$N has been initiated to $t!", ch, clan->name, victim, TO_ROOM );
    act( "You have initiated $N to $t!", ch, clan->name, victim, TO_CHAR );
    save_char_obj( victim );
    save_clan( clan );
    
    return;
}


/*
 * "exil" command is a trap to the "exile" command.
 */
void do_exil( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to EXILE, you have to spell it out.\n\r", ch );

    return;
}


void do_exile( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CLAN_DATA *clan;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Exile whom?\n\r", ch );
        return;
    }

    if ( !is_clan( ch )
        || ch->pcdata->rank != RANK_OVERLORD )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) || victim == ch || victim->level > ch->level )
        return;

    if ( !is_same_clan( ch, victim ) )
    {
        act( "$N isn't even from $t.", ch, clan->name, victim, TO_CHAR );
        return;
    }

    sprintf( buf, "Log %s: exiling %s from %s",
	    ch->name,
	    victim->name,
	    ch->pcdata->clan->name  );
    log_clan( buf );

    remove_from_clan( victim );
    victim->pcdata->rank = RANK_EXILED;

    sprintf( buf,
	    "The grand Overlord of %s %s says:\n\r\n\r"
	    "'Then so be done, you %s shall be exiled from %s!'\n\r"
	    "You hear a thundering sound...\n\r\n\r"
	    "A booming voice says: 'You have been exiled. Only the gods can allow you\n\r"
	    "to join another clan, order or guild!'\n\r",
	    clan->name,
	    clan->overlord,
	    victim->name,
	    clan->name );

    send_to_char( buf, victim );

    act( "You have exiled $N from $t!", ch, clan->name, victim, TO_CHAR );
    save_char_obj( victim );
    save_clan( clan );
    
    return;
}


void do_promote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CLAN_DATA *clan;
    OBJ_DATA  *obj;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    int        newrank;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Promote whom?\n\r", ch );
        return;
    }

    if ( !is_clan( ch )
        || ch->pcdata->rank != RANK_OVERLORD )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) || victim == ch )
        return;

    if ( !is_same_clan( ch, victim ) )
    {
        act( "$N isn't even from $t.", ch, clan->name, victim, TO_CHAR );
        return;
    }

    obj = NULL;

    switch ( victim->pcdata->rank )
    {
    default:
	return;
	break;
    case RANK_CLANSMAN:
	if ( clan->clanheros >= clan->members / 3 )
	{
	    send_to_char( "You may have only 1 clanhero per 3 clansmen.\n\r", ch );
	    return;
	}
	obj = create_object( get_obj_index( clan->clanobj2 ), victim->level );
	clan->clanheros++;
	break;
    case RANK_CLANHERO:
	if ( clan->subchiefs >= clan->members / 6 )
	{
	    send_to_char( "You may have only 1 subchief per 6 clansmen.\n\r", ch );
	    return;
	}
	obj = create_object( get_obj_index( clan->clanobj3 ), victim->level );
	clan->clanheros--;
	clan->subchiefs++;
	break;
    case RANK_SUBCHIEF:
	if ( clan->chieftain[0] != '\0' )
	{
	    send_to_char( "You may have only 1 chieftain.\n\r", ch );
	    return;
	}
	else if ( clan->members < 9 )
	{
	    send_to_char( "You need to have 9 clansmen before you can have a chieftain.\n\r", ch );
	    return;
	}

	clan->subchiefs--;
	clan->chieftain = str_dup( victim->name );
	break;
    case RANK_CHIEFTAIN:
    case RANK_OVERLORD:
	send_to_char( "You may not promote any further that person.\n\r", ch );
	return;
	break;
    }

    victim->pcdata->rank++;
    newrank = victim->pcdata->rank;

    if ( obj )
	obj_to_char( obj, victim );

    sprintf( buf, "Log %s: promoting %s to %s",
	    ch->name,
	    victim->name,
	    ch->pcdata->clan->name  );
    log_clan( buf );

    sprintf( buf,
	    "The grand Overlord %s says:\n\r\n\r"
	    "'I hereby promote you %s to %s!'\n\r"
	    "Take %s as a prize for your superb performance.\n\r"
	    "%s\n\r",
	    ch->name,
	    victim->name,
	    flag_string( rank_flags, newrank ),
	    obj ? obj->short_descr : "this promotion",
	    clan->motto );

    send_to_char( buf, victim );

    act( "You have promoted $N to $t.",
	ch, flag_string( rank_flags, newrank ), victim, TO_CHAR );
    save_char_obj( victim );
    save_clan( clan );
    
    return;
}


void do_demote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CLAN_DATA *clan;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    int        newrank;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Demote whom?\n\r", ch );
        return;
    }

    if ( !is_clan( ch )
        || ch->pcdata->rank != RANK_OVERLORD )
    {
        send_to_char( "You don't have the power to do this.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) || victim == ch )
        return;

    if ( !is_same_clan( ch, victim ) )
    {
        act( "$N isn't even from $t.", ch, clan->name, victim, TO_CHAR );
        return;
    }

    switch ( victim->pcdata->rank )
    {
    default:
	return;
	break;
    case RANK_CLANSMAN:
	send_to_char( "He's as low as they can get.\n\r", ch );
	return;
	break;
    case RANK_CLANHERO:
	clan->clanheros--;
	break;
    case RANK_SUBCHIEF:
	clan->clanheros++;
	clan->subchiefs--;
	break;
    case RANK_CHIEFTAIN:
	clan->subchiefs++;
	free_string( clan->chieftain );
	break;
    case RANK_OVERLORD:
	send_to_char( "You may not demote an Overlord!?\n\r", ch );
	return;
	break;
    }

    victim->pcdata->rank--;
    newrank = victim->pcdata->rank;

    sprintf( buf, "Log %s: demoting %s to %s",
	    ch->name,
	    victim->name,
	    ch->pcdata->clan->name  );
    log_clan( buf );

    sprintf( buf,
	    "The grand Overlord %s says:\n\r\n\r"
	    "'I hereby demote you %s to %s!!!'\n\r"
	    "You should make more efforts to improve!",
	    ch->name,
	    victim->name,
	    flag_string( rank_flags, newrank ) );

    send_to_char( buf, victim );

    act( "You have demoted $N to $t.",
	ch, flag_string( rank_flags, newrank ), victim, TO_CHAR );
    save_char_obj( victim );
    save_clan( clan );
    
    return;
}


void do_crecall( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    ROOM_INDEX_DATA *location;
    char             buf [ MAX_STRING_LENGTH ];
    int              place;

    if ( !is_clan( ch ) )
    {
	send_to_char( "You aren't a clansman!\n\r", ch );
	return;
    }

    act( "$n prays for transportation!", ch, NULL, NULL, TO_ROOM );

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
	|| IS_AFFECTED( ch, AFF_CURSE ) )
    {
	send_to_char( "God has forsaken you.\n\r", ch );
	return;
    }

    place = ch->pcdata->clan->recall;
    if ( !( location = get_room_index( place ) ) )
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    if ( ( victim = ch->fighting ) )
    {
	int lose;

	if ( number_bits( 1 ) == 0 )
	{
	    WAIT_STATE( ch, 4 );
	    lose = ( ch->desc ) ? 25 : 50;
	    gain_exp( ch, 0 - lose );
	    sprintf( buf, "You failed!  You lose %d exps.\n\r", lose );
	    send_to_char( buf, ch );
	    return;
	}

	lose = ( ch->desc ) ? 50 : 100;
	gain_exp( ch, 0 - lose );
	sprintf( buf, "You recall from combat!  You lose %d exps.\n\r", lose );
	send_to_char( buf, ch );
	stop_fighting( ch, TRUE );
    }

    ch->move /= 2;
    act( "$n disappears.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );

    return;
}


void do_cdonate( CHAR_DATA *ch, char *arg )
{
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char      arg1 [ MAX_INPUT_LENGTH ];

    if ( !is_clan( ch ) )
    {
	send_to_char( "You aren't a clansman!\n\r", ch );
	return;
    }

    arg = one_argument( arg, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Donate to your clan what?\n\r", ch );
	return;
    }

    for ( container = object_list; container; container = container->next )
    {
        if ( can_see_obj( ch, container )
	    && container->pIndexData->vnum == ch->pcdata->clan->donation )
	    break;
    }

    if ( !container )
    {
	send_to_char( "The donation pit is missing from the world.\n\r", ch );
	return;
    }

    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
	if ( !( obj = get_obj_carry( ch, arg1 ) ) )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}
	
	if ( get_obj_weight( obj ) + get_obj_weight( container ) 
	  > container->value[0] )
	{
	    send_to_char( "It won't fit.\n\r", ch );
	    return;
	}

	if ( obj->item_type == ITEM_TRASH
	    || obj->item_type == ITEM_FOOD
	    || obj->item_type == ITEM_KEY
	    || obj->item_type == ITEM_PILL )
	{
	    act( "You send $p flying to the $P.", ch, obj, container,
		TO_CHAR );
	    extract_obj( obj );
	    return;
	}

	obj_from_char( obj );
	obj_to_obj( obj, container );
	act( "$n sends $p flying to the $P.", ch, obj, container, TO_ROOM );
	act( "You send $p flying to the $P.", ch, obj, container, TO_CHAR );
	send_to_room( "A loud clank is heard from the pit!",
		     container->in_room );
    }
    else
    {
	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( obj->deleted )
		continue;

	    if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		&& can_see_obj( ch, obj )
		&& obj->wear_loc == WEAR_NONE
		&& obj != container
		&& can_drop_obj( ch, obj )
		&& get_obj_weight( obj ) + get_obj_weight( container )
		<= container->value[0] )
	    {

	        if ( obj->item_type == ITEM_TRASH
		    || obj->item_type == ITEM_FOOD
		    || obj->item_type == ITEM_KEY
		    || obj->item_type == ITEM_PILL )
		{
		    act( "You send $p flying to the $P.", ch, obj, container,
			TO_CHAR );
		    extract_obj( obj );
		    continue;
		}

		obj_from_char( obj );
		obj_to_obj( obj, container );
		act( "$n sends $p flying to the $P.", ch, obj, container,
		    TO_ROOM );
		act( "You send $p flying to the $P.", ch, obj, container,
		    TO_CHAR );
		send_to_room( "A loud clank is heard from the pit!\n\r",
		     container->in_room );
	    }
	}
    }

    return;

}


/*
 * "leav" command is a trap to the "leave" command.
 */
void do_leav( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to LEAVE, you have to spell it out.\n\r", ch );

    return;
}


void do_leave( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    char       buf [ MAX_STRING_LENGTH ];

    if ( !is_clan( ch )  )
    {
        send_to_char( "You aren't a clansman.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    if ( ch->pcdata->rank == RANK_OVERLORD )
    {
        send_to_char( "Huh? An Overlord shouldn't leave his clan!\n\r", ch );
        return;
    }

    remove_from_clan( ch );
    ch->pcdata->rank = RANK_EXILED;

    sprintf( buf,
	    "You leave %s off into exile.\n\r"
	    "You hear a thundering sound...\n\r\n\r"
	    "A booming voice says: 'You have been exiled. Only the gods can allow you\n\r"
	    "to join another clan, order or guild!'\n\r",
	    clan->name );

    send_to_char( buf, ch );

    act( "You have left clan $t.", ch, clan->name, NULL, TO_CHAR );
    save_char_obj( ch );
    save_clan( clan );
    
    return;
}


void do_clans( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    char       buf1 [ MAX_STRING_LENGTH ];
    char       buf  [ MAX_STRING_LENGTH ];
    int        found;

    buf1[0] = '\0';

    sprintf( buf, "{o{rClan              Overlord          Chieftain         Pkills{x\n\r" );
    strcat( buf1, buf );

    for ( clan = clan_first, found = 0; clan; clan = clan->next )
    {
	if ( clan->clan_type == CLAN_ORDER || clan->clan_type == CLAN_GUILD )
	    continue;

	sprintf( buf, "{o{r%-16.16s  %-16.16s  %-16.16s   %5.5d{x\n\r",
		clan->name,
		clan->overlord,
		clan->chieftain,
		clan->pkills );
	strcat( buf1, buf );
	found++;
    }

    if ( !found )
	sprintf( buf, "{o{rThere are no Clans currently formed.{x\n\r" );
    else
	sprintf( buf, "{o{yYou see %d clan%s in the game.{x\n\r",
		found,
		found == 1 ? "" : "s" );

    strcat( buf1, buf );
    send_to_char( buf1, ch );

    return;
}


void do_orders( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *order;
    char       buf1 [ MAX_STRING_LENGTH ];
    char       buf  [ MAX_STRING_LENGTH ];
    int        found;

    buf1[0] = '\0';

    sprintf( buf, "{o{gOrder             Overlord          Chieftain         Mkills     Mdeaths{x\n\r" );
    strcat( buf1, buf );

    for ( order = clan_first, found = 0; order; order = order->next )
    {
	if ( order->clan_type != CLAN_ORDER )
	    continue;

	sprintf( buf, "{o{g%-16.16s  %-16.16s  %-16.16s   %5.5d       %5.5d{x\n\r",
		order->name,
		order->overlord,
		order->chieftain,
		order->mkills,
		order->mdeaths );
	strcat( buf1, buf );
	found++;
    }

    if ( !found )
	sprintf( buf, "{o{gThere are no Orders currently formed.{x\n\r" );
    else
	sprintf( buf, "{o{yYou see %d order%s in the game.{x\n\r",
		found,
		found == 1 ? "" : "s" );

    strcat( buf1, buf );
    send_to_char( buf1, ch );

    return;
}


void do_guilds( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *guild;
    char       buf1 [ MAX_STRING_LENGTH ];
    char       buf  [ MAX_STRING_LENGTH ];
    int        found;

    buf1[0] = '\0';

    sprintf( buf, "{o{gGuild             Overlord          Chieftain         Mkills     Mdeaths{x\n\r" );
    strcat( buf1, buf );

    for ( guild = clan_first, found = 0; guild; guild = guild->next )
    {
	if ( guild->clan_type != CLAN_GUILD )
	    continue;

	sprintf( buf, "{o{g%-16.16s  %-16.16s  %-16.16s   %5.5d       %5.5d{x\n\r",
		guild->name,
		guild->overlord,
		guild->chieftain,
		guild->mkills,
		guild->mdeaths );
	strcat( buf1, buf );
	found++;
    }

    if ( !found )
	sprintf( buf, "{o{gThere are no Guilds currently formed.{x\n\r" );
    else
	sprintf( buf, "{o{yYou see %d guild%s in the game.{x\n\r",
		found,
		found == 1 ? "" : "s" );

    strcat( buf1, buf );
    send_to_char( buf1, ch );

    return;
}


void do_claninfo( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *clan;
    char       buf  [ MAX_STRING_LENGTH ];
    char       buf1 [ MAX_STRING_LENGTH ];

    if ( !is_clan( ch )  )
    {
        send_to_char( "You aren't a clansman.\n\r", ch );
        return;
    }

    clan = ch->pcdata->clan;

    buf1[0] = '\0';

    sprintf( buf, "{o{c%s{x\n\r", clan->name );
    strcat( buf1, buf );

    sprintf( buf, "{o{cMotto: \"%s\"{x\n\r", clan->motto );
    strcat( buf1, buf );

    strcat( buf1, "{o{c-----------------------------------------------------------------------------{x\n\r" );

    sprintf( buf, "{o{c%s{x\n\r", clan->description );
    strcat( buf1, buf );
    
    sprintf( buf, "{o{cOVERLORD : %s{x\n\r",
	    clan->overlord );
    strcat( buf1, buf );

    sprintf( buf, "{o{cCHIEFTAIN: %s  %s{x\n\r",
	    clan->chieftain,
	    ( clan->members > 9 && clan->chieftain[0] == '\0' )
	    ? "Promote someone to Chieftain!" : "" );
    strcat( buf1, buf );

    sprintf( buf, "{o{cSubchiefs: %5.5d  Max Subchiefs: %5.5d{x\n\r",
	    clan->subchiefs, clan->members / 6 );
    strcat( buf1, buf );

    sprintf( buf, "{o{cClanheros: %5.5d  Max Clanheros: %5.5d{x\n\r",
	    clan->clanheros, clan->members / 3 );
    strcat( buf1, buf );

    sprintf( buf, "{o{cMembers  : %5.5d  SCORE        : %5.5d{x\n\r",
	    clan->members, clan->score );
    strcat( buf1, buf );

    send_to_char( buf1, ch );

    return;
}
