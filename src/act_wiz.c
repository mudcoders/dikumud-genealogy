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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/* Conversion of Immortal powers to Immortal skills done by Thelonius */

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       buf  [ MAX_STRING_LENGTH ];
    char       buf1 [ MAX_STRING_LENGTH ];
    int        cmd;
    int        col;

    rch = get_char( ch );
    
    if ( !authorized( rch, "wizhelp" ) )
        return;

    buf1[0] = '\0';
    col     = 0;

    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level < LEVEL_HERO
	    || str_infix( cmd_table[cmd].name, rch->pcdata->immskll ) )
	    continue;

	sprintf( buf, "%-10s", cmd_table[cmd].name );
	strcat( buf1, buf );
	if ( ++col % 8 == 0 )
	    strcat( buf1, "\r\n" );
    }
 
    if ( col % 8 != 0 )
	strcat( buf1, "\r\n" );
    send_to_char( buf1, ch );
    return;
}



void do_bamfin( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "bamfin" ) )
        return;

    if ( !IS_NPC( ch ) )
    {
        if ( longstring( ch, argument ) )
	    return;

	smash_tilde( argument );
	free_string( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );
	send_to_char( "Ok.\r\n", ch );
    }
    return;
}



void do_bamfout( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "bamfout" ) )
        return;

    if ( !IS_NPC( ch ) )
    {
        if ( longstring( ch, argument ) )
	    return;

	smash_tilde( argument );
	free_string( ch->pcdata->bamfout );
	ch->pcdata->bamfout = str_dup( argument );
	send_to_char( "Ok.\r\n", ch );
    }
    return;
}



void do_deny( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "deny" ) )
        return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Deny whom?\r\n", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\r\n", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\r\n", ch );
	return;
    }

    SET_BIT( victim->act, PLR_DENY );
    send_to_char( "You are denied access!\r\n", victim );
    send_to_char( "OK.\r\n", ch );
    if ( victim->level < 2 )
      victim->level = 2;
    do_quit( victim, "" );

    return;
}



void do_disconnect( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "disconnect" ) )
        return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Disconnect whom?\r\n", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( !victim->desc )
    {
	act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d == victim->desc )
	{
	    close_socket( d );
	    send_to_char( "Ok.\r\n", ch );
	    return;
	}
    }

    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char( "Descriptor not found!\r\n", ch );
    return;
}



void do_pardon( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "pardon" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: pardon <character> <k(iller)|t(hieaf)|a(ll)>.\r\n", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\r\n", ch );
	return;
    }

    if ( !str_prefix( arg2, "all" ) )
    {
	if ( IS_SET( victim->act, PLR_KILLER ) )
	{
	    REMOVE_BIT( victim->act, PLR_KILLER );
	    send_to_char( "Killer flag removed.\r\n",		ch	);
	    send_to_char( "You are no longer a killer.\r\n",	victim	);
	}
	if ( IS_SET( victim->act, PLR_THIEF ) )
	{
	    REMOVE_BIT( victim->act, PLR_THIEF );
	    send_to_char( "Thief flag removed.\r\n",		ch	);
	    send_to_char( "You are no longer a thief.\r\n",	victim	);
	}
	return;
    }

    if ( !str_prefix( arg2, "killer" ) )
    {
	if ( IS_SET( victim->act, PLR_KILLER ) )
	{
	    REMOVE_BIT( victim->act, PLR_KILLER );
	    send_to_char( "Killer flag removed.\r\n",        ch     );
	    send_to_char( "You are no longer a KILLER.\r\n", victim );
	}
	return;
    }

    if ( !str_prefix( arg2, "thief" ) )
    {
	if ( IS_SET( victim->act, PLR_THIEF  ) )
	{
	    REMOVE_BIT( victim->act, PLR_THIEF  );
	    send_to_char( "Thief flag removed.\r\n",        ch     );
	    send_to_char( "You are no longer a THIEF.\r\n", victim );
	}
	return;
    }

    send_to_char( "Syntax: pardon <character> <k(iller)|t(hief)|a(ll)>.\r\n", ch );
    return;
}



void do_echo( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    
    rch = get_char( ch );

    if ( !authorized( rch, "echo" ) )
        return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Echo what?\r\n", ch );
	return;
    }

    strcat( argument, "\r\n" );
    send_to_all_char( argument );

    return;
}

void do_pecho( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if ( !authorized( ch, "pecho" ) )
	return;

    argument = one_argument(argument, arg);

    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
        send_to_char("pecho what?\r\n", ch);
        return;
    }

    if  ( (victim = get_char_world(ch, arg) ) == NULL )
    {
        send_to_char("Target not found.\r\n",ch);
        return;
    }

    if ( get_trust(victim) >= get_trust(ch) )
        send_to_char( "pecho> ",victim);

    send_to_char(argument,victim);
    send_to_char("\r\n",victim);
    send_to_char( "pecho> ",ch);
    send_to_char(argument,ch);
    send_to_char("\r\n",ch);
}

void do_recho( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    DESCRIPTOR_DATA *d;
    
    rch = get_char( ch );

    if ( !authorized( rch, "recho" ) )
        return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Recho what?\r\n", ch );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	    && d->character->in_room == ch->in_room )
	{
	    send_to_char( argument, d->character );
	    send_to_char( "\r\n",   d->character );
	}
    }

    return;
}



ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;

    if ( is_number( arg ) )
	return get_room_index( atoi( arg ) );

    if ( ( victim = get_char_world( ch, arg ) ) )
	return victim->in_room;

    if ( ( obj = get_obj_world( ch, arg ) ) )
	return obj->in_room;

    return NULL;
}



void do_transfer( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    ROOM_INDEX_DATA *location;
    char             arg1 [ MAX_INPUT_LENGTH ];
    char             arg2 [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "transfer" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Transfer whom (and where)?\r\n", ch );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
		&& d->character != ch
		&& d->character->in_room
		&& can_see( ch, d->character ) )
	    {
		char buf [ MAX_STRING_LENGTH ];

		sprintf( buf, "%s %s", d->character->name, arg2 );
		do_transfer( ch, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( !( location = find_location( ch, arg2 ) ) )
	{
	    send_to_char( "No such location.\r\n", ch );
	    return;
	}

	if ( ( room_is_private( location ) ) && (!get_trust(ch) >= MAX_LEVEL ) )
	{
	    send_to_char( "That room is private right now.\r\n", ch );
	    return;
	}
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( !victim->in_room )
    {
	send_to_char( "They are in limbo.\r\n", ch );
	return;
    }

    if ( victim->fighting )
	stop_fighting( victim, TRUE );
    act( "$n disappears in a mushroom cloud.", victim, NULL, NULL,   TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    act( "$n arrives from a puff of smoke.",   victim, NULL, NULL,   TO_ROOM );
    if ( ch != victim )
	act( "$n has transferred you.",        ch,     NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    send_to_char( "Ok.\r\n", ch );
}



void do_at( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *wch;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    char             arg [ MAX_INPUT_LENGTH ];
    
    rch = get_char( ch );

    if ( !authorized( rch, "at" ) )
        return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "At where what?\r\n", ch );
	return;
    }

    if ( !( location = find_location( ch, arg ) ) )
    {
	send_to_char( "No such location.\r\n", ch );
	return;
    }

    if ( ( room_is_private( location ) ) && (!get_trust(ch) >= MAX_LEVEL ) )
    {
	send_to_char( "That room is private right now.\r\n", ch );
	return;
    }

    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    break;
	}
    }

    return;
}



void do_goto( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    ROOM_INDEX_DATA *location;
    char             arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "goto" ) )
        return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Goto where?\r\n", ch );
	return;
    }

    if ( !( location = find_location( ch, arg ) ) )
    {
	send_to_char( "No such location.\r\n", ch );
	return;
    }

    if ( ( room_is_private( location ) ) && (!get_trust(ch) >= MAX_LEVEL ) )
    {
	send_to_char( "That room is private right now.\r\n", ch );
	return;
    }

    if ( ch->fighting )
	stop_fighting( ch, TRUE );
    if ( !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
	act( "$n $T.", ch, NULL,
	    ( ch->pcdata && ch->pcdata->bamfout[0] != '\0' )
	    ? ch->pcdata->bamfout : "leaves in a swirling mist",  TO_ROOM );
    }

    char_from_room( ch );
    char_to_room( ch, location );

    if ( !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
	act( "$n $T.", ch, NULL,
	    ( ch->pcdata && ch->pcdata->bamfin[0] != '\0' )
	    ? ch->pcdata->bamfin : "appears in a swirling mist", TO_ROOM );
    }

    do_look( ch, "auto" );
    return;
}



void do_rstat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA        *obj;
    CHAR_DATA       *rch;
    ROOM_INDEX_DATA *location;
    char             buf  [ MAX_STRING_LENGTH ];
    char             buf1 [ MAX_STRING_LENGTH ];
    char             arg  [ MAX_INPUT_LENGTH  ];
    int              door;

    rch = get_char( ch );

    if ( !authorized( rch, "rstat" ) )
        return;

    one_argument( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( !location )
    {
	send_to_char( "No such location.\r\n", ch );
	return;
    }

    if ( ( ch->in_room != location && room_is_private( location ) ) &&
	(!get_trust(ch) >= MAX_LEVEL ) )
    {
	send_to_char( "That room is private right now.\r\n", ch );
	return;
    }

    buf1[0] = '\0';

    sprintf( buf, "Name: '%s.'\r\nArea: '%s'.\r\n",
	    location->name,
	    location->area->name );
    strcat( buf1, buf );

    sprintf( buf,
	    "Vnum: %d.  Sector: %s.  Light: %d.\r\n",
	    location->vnum,
	    sector_string(location->sector_type),	/* Maniac */
	    location->light );
    strcat( buf1, buf );

    /* Room flags extension by Maniac */
    sprintf( buf, "Description:\r\n%s", location->description );
    strcat( buf1, buf );

    if ( location->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "Extra description keywords: '" );
	for ( ed = location->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, " " );
	}
	strcat( buf1, "'.\r\n" );
    }

    sprintf(buf,"Room flags: %s.\r\n", room_flags_name(location->room_flags) );
    strcat( buf1, buf );

    strcat( buf1, "Characters:" );

    /* Yes, we are reusing the variable rch.  - Kahn */
    for ( rch = location->people; rch; rch = rch->next_in_room )
    {
      /* Heh.  Thanks to Zavod for this little security fix */ 
      if ( can_see( ch, rch ) )
	{
	  strcat( buf1, " " );
	  one_argument( rch->name, buf );
	  strcat( buf1, buf );
	}
    }

    strcat( buf1, ".\r\nObjects:   " );
    for ( obj = location->contents; obj; obj = obj->next_content )
    {
	strcat( buf1, " " );
	one_argument( obj->name, buf );
	strcat( buf1, buf );
    }
    strcat( buf1, ".\r\n" );

    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = location->exit[door] ) )
	{
	    sprintf( buf,
		    "Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\r\n",
		    door,
		    pexit->to_room ? pexit->to_room->vnum : 0,
		    pexit->key,
		    pexit->exit_info );
	    strcat( buf1, buf );
	    sprintf( buf,
		    "Keyword: '%s'.  Description: %s",
		    pexit->keyword,
		    pexit->description[0] != '\0' ? pexit->description
		                                  : "(none).\r\n" );
	    strcat( buf1, buf );
	}
    }

    send_to_char( buf1, ch );
    return;
}



void do_ostat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA    *obj;
    CHAR_DATA   *rch;
    AFFECT_DATA *paf;
    char         buf  [ MAX_STRING_LENGTH ];
    char         buf1 [ MAX_STRING_LENGTH ];
    char         arg  [ MAX_INPUT_LENGTH  ];

    rch = get_char( ch );

    if ( !authorized( rch, "ostat" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Ostat what?\r\n", ch );
	return;
    }

    buf1[0] = '\0';

    if ( !( obj = get_obj_world( ch, arg ) ) )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\r\n", ch);
	return;
    }

    sprintf( buf, "Name: %s.\r\n",
	    obj->name );
    strcat( buf1, buf );

    sprintf( buf, "Vnum: %d.  Type: %s.\r\n",
	    obj->pIndexData->vnum, item_type_name( obj ) );
    strcat( buf1, buf );

    sprintf( buf, "Short description: %s.\r\nLong description: %s\r\n",
	    obj->short_descr, obj->description );
    strcat( buf1, buf );

    sprintf( buf, "Wear bits: %s.\r\nExtra bits: %s.\r\n",
	    str_wear_loc(obj->wear_flags),
	    extra_bit_name( obj->extra_flags ) );
    strcat( buf1, buf );

    sprintf( buf, "Number: %d/%d.  Weight: %d/%d.\r\n",
	    1,           get_obj_number( obj ),
	    obj->weight, get_obj_weight( obj ) );
    strcat( buf1, buf );

    sprintf( buf, "Cost: %d.  Timer: %d.  Level: %d.\r\n",
	    obj->cost, obj->timer, obj->level );
    strcat( buf1, buf );

    sprintf( buf,
	    "In room: %d.  In object: %s.  Carried by: %s.  Wear_loc: %s.\r\n",
	    !obj->in_room    ?        0 : obj->in_room->vnum,
	    !obj->in_obj     ? "(none)" : obj->in_obj->short_descr,
	    !obj->carried_by ? "(none)" : obj->carried_by->name,
	    obj_eq_loc(obj->wear_loc) );
    strcat( buf1, buf );
    
    sprintf( buf, "Values: %d %d %d %d.\r\n",
	    obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
    strcat( buf1, buf );

    if ( obj->extra_descr || obj->pIndexData->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "Extra description keywords: '" );

	for ( ed = obj->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, " " );
	}

	for ( ed = obj->pIndexData->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, " " );
	}

	strcat( buf1, "'.\r\n" );
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d.\r\n",
		affect_loc_name( paf->location ), paf->modifier );
	strcat( buf1, buf );
    }

    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d.\r\n",
		affect_loc_name( paf->location ), paf->modifier );
	strcat( buf1, buf );
    }

    send_to_char( buf1, ch );
    return;
}


void do_mstat( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *rch;
    CHAR_DATA   *victim;
    AFFECT_DATA *paf;
    char         buf  [ MAX_STRING_LENGTH ];
    char         buf1 [ MAX_STRING_LENGTH ];
    char         arg  [ MAX_INPUT_LENGTH  ];

    rch = get_char( ch );

    if ( !authorized( rch, "mstat" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mstat whom?\r\n", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    buf1[0] = '\0';

    sprintf( buf, "Name: %s.\r\n",
	    victim->name );
    strcat( buf1, buf );

    sprintf( buf, "Race: %s.\r\n", race_table[victim->race].name );
    strcat( buf1, buf );

    sprintf( buf, "Vnum: %d.  Sex: %s.  Room: %d.\r\n",
	    IS_NPC( victim ) ? victim->pIndexData->vnum : 0,
	    victim->sex == SEX_MALE    ? "male"   :
	    victim->sex == SEX_FEMALE  ? "female" : "neuter",
	    !victim->in_room           ?        0 : victim->in_room->vnum );
    strcat( buf1, buf );

    sprintf( buf, "Str: %d.  Int: %d.  Wis: %d.  Dex: %d.  Con: %d.\r\n",
	    get_curr_str( victim ),
	    get_curr_int( victim ),
	    get_curr_wis( victim ),
	    get_curr_dex( victim ),
	    get_curr_con( victim ) );
    strcat( buf1, buf );

    sprintf( buf, "Hp: %d/%d.  Mana: %d/%d.  Move: %d/%d.  Practices: %d.\r\n",
	    victim->hit,         victim->max_hit,
	    victim->mana,        victim->max_mana,
	    victim->move,        victim->max_move,
	    victim->practice );
    strcat( buf1, buf );
	
    sprintf( buf,
	"Lv: %d.  Class: %s.  Align: %d.  AC: %d.  Exp: %d.  ",
	    victim->level, class_table[victim->class].who_name,
	    victim->alignment, GET_AC( victim ), victim->exp );
    strcat( buf1, buf );

    /* Display balance and shares by Maniac */
    if (IS_NPC(victim))
    {
	sprintf ( buf, "Gold: %d\r\n", victim->gold);
	strcat( buf1, buf );
    }
    else
    {
	sprintf (buf, "\r\nGold: %d.  Balance: %d.  Shares: %d. Securety: %d\r\n",
	victim->gold, victim->pcdata->balance, victim->pcdata->shares, victim->pcdata->security );
	strcat( buf1, buf );
    }

    sprintf( buf, "Position: %d.  Wimpy: %d.\r\n",
            victim->position,    victim->wimpy );
    strcat( buf1, buf );

    if ( IS_NPC( victim )
        || victim->level >= skill_table[gsn_dual].skill_level[victim->class] )
	strcat ( buf1, "Primary Weapon " );
    sprintf( buf, "Hitroll: %d  Damroll: %d.\r\n",
            get_hitroll( victim, WEAR_WIELD ),
            get_damroll( victim, WEAR_WIELD ) );
    strcat( buf1, buf );

    if ( IS_NPC(victim) && victim->hunting != NULL )
    {
        sprintf(buf, "Hunting victim: %s (%s)\r\n",
                IS_NPC(victim->hunting) ? victim->hunting->short_descr
                                        : victim->hunting->name,
                IS_NPC(victim->hunting) ? "MOB" : "PLAYER" );
        strcat(buf1, buf);
    }

    if ( get_eq_char( victim, WEAR_WIELD_2 ) )
    {
	sprintf( buf, " Second Weapon Hitroll: %d  Damroll: %d.\r\n",
		get_hitroll( victim, WEAR_WIELD_2 ),
		get_damroll( victim, WEAR_WIELD_2 ) );
	strcat( buf1, buf );
    }

    if ( !IS_NPC( victim ) )
    {
	sprintf( buf, "Page Lines: %d.\r\n", victim->pcdata->pagelen );
	strcat( buf1, buf );
    }

    sprintf( buf, "Fighting: %s.\r\n",
	    victim->fighting ? victim->fighting->name : "(none)" );
    strcat( buf1, buf );

    if ( !IS_NPC( victim ) )
    {
	sprintf( buf,
		"Thirst: %d.  Full: %d.  Drunk: %d.  Saving throw: %d.\r\n",
		victim->pcdata->condition[COND_THIRST],
		victim->pcdata->condition[COND_FULL  ],
		victim->pcdata->condition[COND_DRUNK ],
		victim->saving_throw );
	strcat( buf1, buf );
    }

    sprintf( buf, "Carry number: %d.  Carry weight: %d.\r\n",
	    victim->carry_number, victim->carry_weight );
    strcat( buf1, buf );

    sprintf( buf, "Age: %d.  Played: %d.  Timer: %d.\r\n",
	    get_age( victim ),
	    (int) victim->played,
	    victim->timer);
    strcat( buf1, buf );

    /* Act flag extension by Maniac */
    if (IS_NPC(victim))
	sprintf( buf, "Act: %s.\r\n", mob_act_name(victim->act) );
    else
	sprintf( buf, "Act: %s.\r\n", plr_act_name(victim->act) );
    strcat (buf1, buf );

    sprintf( buf, "Master: %s.  Leader: %s.  Affected by: %s.\r\n",
	    victim->master      ? victim->master->name   : "(none)",
	    victim->leader      ? victim->leader->name   : "(none)",
	    affect_bit_name( victim->affected_by ) );
    strcat( buf1, buf );

    sprintf( buf, "Short description: %s.\r\nLong  description: %s",
	    victim->short_descr,
	    victim->long_descr[0] != '\0' ? victim->long_descr
	                                  : "(none).\r\n" );
    strcat( buf1, buf );

    if ( IS_NPC( victim ) && victim->spec_fun != 0 )
	strcat( buf1, "Mobile has spec fun.\r\n" );

    if ( IS_NPC( victim ) && victim->game_fun != 0 )
	strcat( buf1, "Mobile has game fun.\r\n" );

    for ( paf = victim->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
	sprintf( buf,
		"Spell: '%s' modifies %s by %d for %d hours with bits %s.\r\n",
		skill_table[(int) paf->type].name,
		affect_loc_name( paf->location ),
		paf->modifier,
		paf->duration,
		affect_bit_name( paf->bitvector ) );
	strcat( buf1, buf );
    }

    send_to_char( buf1, ch );
    return;
}



void do_mfind( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA      *rch;
           MOB_INDEX_DATA *pMobIndex;
           char            buf  [ MAX_STRING_LENGTH   ];
           char            buf1 [ MAX_STRING_LENGTH*2 ];
           char            arg  [ MAX_INPUT_LENGTH    ];
    extern int             top_mob_index;
           int             vnum;
	   int             nMatch;
	   bool            fAll;
	   bool            found;

    rch = get_char( ch );

    if ( !authorized( rch, "mfind" ) )
        return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Mfind whom?\r\n", ch );
	return;
    }

    buf1[0] = '\0';
    fAll    = !str_cmp( arg, "all" );
    found   = FALSE;
    nMatch  = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    nMatch++;
	    if ( fAll || is_name( arg, pMobIndex->player_name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %s\r\n",
		    pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
		if ( !fAll )
		    strcat( buf1, buf );
		else
		    send_to_char( buf, ch );
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\r\n", ch);
	return;
    }

    if ( !fAll )
        send_to_char( buf1, ch );
    return;
}

void do_ofind( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA      *rch;
	   OBJ_INDEX_DATA *pObjIndex;
	   char            buf  [ MAX_STRING_LENGTH   ];
	   char            buf1 [ MAX_STRING_LENGTH*2 ];
	   char            arg  [ MAX_INPUT_LENGTH    ];
    extern int             top_obj_index;
	   int             vnum;
	   int             nMatch;
	   bool            fAll;
	   bool            found;

    rch = get_char( ch );

    if ( !authorized( rch, "ofind" ) )
        return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Ofind what?\r\n", ch );
	return;
    }

    buf1[0] = '\0';
    fAll    = !str_cmp( arg, "all" );
    found   = FALSE;
    nMatch  = 0;

    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
	if ( ( pObjIndex = get_obj_index( vnum ) ) )
	{
	    nMatch++;
	    if ( fAll || is_name( arg, pObjIndex->name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %s\r\n",
		    pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
		if ( !fAll )
		    strcat( buf1, buf );
		else
		    send_to_char( buf, ch );
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\r\n", ch);
	return;
    }

    if ( !fAll )
        send_to_char( buf1, ch );
    return;
}

void do_mwhere( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       buf  [ MAX_STRING_LENGTH   ];
    char       buf1 [ MAX_STRING_LENGTH*5 ];
    char       arg  [ MAX_INPUT_LENGTH    ];
    bool       found;

    rch = get_char( ch );

    if ( !authorized( rch, "mwhere" ) )
        return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Mwhere whom?\r\n", ch );
	return;
    }

    buf1[0] = '\0';
    found   = FALSE;
    for ( victim = char_list; victim; victim = victim->next )
    {
	if ( IS_NPC( victim )
	    && victim->in_room
	    && is_name( arg, victim->name ) )
	{
	    found = TRUE;
	    sprintf( buf, "[%5d] %-28s [%5d] %s\r\n",
		    victim->pIndexData->vnum,
		    victim->short_descr,
		    victim->in_room->vnum,
		    victim->in_room->name );
	    strcat( buf1, buf );
	}
    }

    if ( !found )
    {
	act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
	return;
    }

    send_to_char( buf1, ch );
    return;
}



void do_reboo( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "reboot" ) )
        return;

    send_to_char( "If you want to REBOOT, spell it out.\r\n", ch );
    return;
}



void do_reboot( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA *rch;
           char       buf [ MAX_STRING_LENGTH ];
    extern bool       merc_down;

    rch = get_char( ch );

    if ( !authorized( rch, "reboot" ) )
        return;

    sprintf( buf, "Reboot by %s.", ch->name );
    do_echo( ch, buf );

    end_of_game( );

    merc_down = TRUE;
    return;
}



void do_shutdow( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "shutdown" ) )
        return;

    send_to_char( "If you want to SHUTDOWN, spell it out.\r\n", ch );
    return;
}



void do_shutdown( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA *rch;
           char       buf [ MAX_STRING_LENGTH ];
    extern bool       merc_down;

    rch = get_char( ch );

    if ( !authorized( rch, "shutdown" ) )
        return;

    sprintf( buf, "Shutdown by %s.", ch->name );
    append_file( ch, SHUTDOWN_FILE, buf );
    strcat( buf, "\r\n" );
    do_echo( ch, buf );

    end_of_game( );

    merc_down = TRUE;
    return;
}



void do_snoop( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "snoop" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Snoop whom?\r\n", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( !victim->desc )
    {
	send_to_char( "No descriptor to snoop.\r\n", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Cancelling all snoops.\r\n", ch );
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->snoop_by == ch->desc )
		d->snoop_by = NULL;
	}
	return;
    }

    if ( victim->desc->snoop_by )
    {
	send_to_char( "Busy already.\r\n", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\r\n", ch );
	return;
    }

    if ( ch->desc )
    {
	for ( d = ch->desc->snoop_by; d; d = d->snoop_by )
	{
	    if ( d->character == victim || d->original == victim )
	    {
		send_to_char( "No snoop loops.\r\n", ch );
		return;
	    }
	}
    }

    victim->desc->snoop_by = ch->desc;
    send_to_char( "Ok.\r\n", ch );
    return;
}



void do_switch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "switch" ) )
        return;

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Switch into whom?\r\n", ch );
	return;
    }

    if ( !ch->desc )
	return;
    
    if ( ch->desc->original )
    {
	send_to_char( "You are already switched.\r\n", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Ok.\r\n", ch );
	return;
    }

    /*
     * Pointed out by Da Pub (What Mud)
     */
    if ( !IS_NPC( victim ) )
    {
        send_to_char( "You cannot switch into a player!\r\n", ch );
	return;
    }

    if ( victim->desc )
    {
	send_to_char( "Character in use.\r\n", ch );
	return;
    }

    ch->pcdata->switched  = TRUE;
    ch->desc->character   = victim;
    ch->desc->original    = ch;
    victim->desc          = ch->desc;
    ch->desc              = NULL;
    send_to_char( "Ok.\r\n", victim );
    return;
}



void do_return( CHAR_DATA *ch, char *argument )
{
    if ( !ch->desc )
	return;

    if ( !authorized( ch, "return" ) )
	return;

    if ( !ch->desc->original )
    {
	send_to_char( "You aren't switched.\r\n", ch );
	return;
    }

/* Note that we dont check for immortal ability to have return here.
 * We assume we will automatically allow immortals with switch to return.
 * Dont want to have our immortals stuck in a mobile's body do we?  :)
 * -Kahn */
/* Well, note that I, Canth, find it mildly amusing to have an immortal
 * in a mobile's body, and added the authorization check -Canth */

    send_to_char( "You return to your original body.\r\n", ch );
    ch->desc->original->pcdata->switched = FALSE;
    ch->desc->character                  = ch->desc->original;
    ch->desc->original                   = NULL;
    ch->desc->character->desc            = ch->desc; 
    ch->desc                             = NULL;
    return;
}



void do_mload( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA      *rch;
    CHAR_DATA      *victim;
    MOB_INDEX_DATA *pMobIndex;
    char            arg [ MAX_INPUT_LENGTH ];
    
    rch = get_char( ch );

    if ( !authorized( rch, "mload" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
	send_to_char( "Syntax: mload <vnum>.\r\n", ch );
	return;
    }

    if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
    {
	send_to_char( "No mob has that vnum.\r\n", ch );
	return;
    }

    victim = create_mobile( pMobIndex );	/* Add func by Maniac */
    victim->spec_fun = pMobIndex->spec_fun;	/* Add special function */
    victim->game_fun = pMobIndex->game_fun;	/* Add game function */
    char_to_room( victim, ch->in_room );
    send_to_char( "Ok.\r\n", ch );
    act( "$n has created $N!", ch, NULL, victim, TO_ROOM );
    return;
}



void do_oload( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA       *obj;
    CHAR_DATA      *rch;
    OBJ_INDEX_DATA *pObjIndex;
    char            arg1 [ MAX_INPUT_LENGTH ];
    char            arg2 [ MAX_INPUT_LENGTH ];
    int             level;

    rch = get_char( ch );

    if ( !authorized( rch, "oload" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );
 
    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        send_to_char( "Syntax: oload <vnum> <level>.\r\n", ch );
        return;
    }
 
    if ( arg2[0] == '\0' )
    {
	level = get_trust( ch );
    }
    else
    {
	/*
	 * New feature from Alander.
	 */
        if ( !is_number( arg2 ) )
        {
	    send_to_char( "Syntax: oload <vnum> <level>.\r\n", ch );
	    return;
        }
        level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
        {
	    send_to_char( "Limited to your trust level.\r\n", ch );
	    return;
        }
    }

    if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
    {
	send_to_char( "No object has that vnum.\r\n", ch );
	return;
    }

    obj = create_object( pObjIndex, level );
    if ( CAN_WEAR( obj, ITEM_TAKE ) )
    {
	obj_to_char( obj, ch );
    }
    else
    {
	obj_to_room( obj, ch->in_room );
	act( "$n has created $p!", ch, obj, NULL, TO_ROOM );
    }
    send_to_char( "Ok.\r\n", ch );
    return;
}



void do_purge( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "purge" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	/* 'purge' */
        OBJ_DATA  *obj_next;
	CHAR_DATA *vnext;

	for ( victim = ch->in_room->people; victim; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( victim->deleted )
	        continue;

	    if ( IS_NPC( victim ) && victim != ch )
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->contents; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->deleted )
	        continue;
	    extract_obj( obj );
	}

	send_to_char( "Ok.\r\n", ch );
	act( "$n purges the room!", ch, NULL, NULL, TO_ROOM);
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "Not on PC's.\r\n", ch );
	return;
    }

    act( "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
    extract_char( victim, TRUE );
    return;
}



void do_advance( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    int        level;
    int        iLevel;

    rch = get_char( ch );

    if ( !authorized( rch, "advance" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: advance <char> <level>.\r\n", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	send_to_char( "That player is not here.\r\n", ch);
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\r\n", ch );
	return;
    }
    
    level = atoi( arg2 );

    if ( level < 1 || level > MAX_LEVEL )
    {
	char buf [ MAX_STRING_LENGTH ];

	sprintf( buf, "Advance within range 1 to %d.\r\n", MAX_LEVEL );
	send_to_char( buf, ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust level.\r\n", ch );
	return;
    }

    /*
     * Little extra check by Canth (phule@xs4all.nl)
     * This works seeing as I'm trusted 55 ;-)
     */
    if ( ( get_trust(ch) <= get_trust(victim) ) && (ch != victim) )
    {
	send_to_char( "You cannot advance your peer nor your superior.\r\n", ch );
	return;
    }

    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     *   Not anymore
     *   -- Canth
     */
    if ( level <= victim->level )
    {
	int sn;
	
	send_to_char( "Lowering a player's level!\r\n", ch );
	send_to_char( "**** OOOOHHHHHHHHHH  NNNNOOOO ****\r\n",    victim );
	victim->level    = 1;
	victim->exp      = 1000;
	victim->max_hit  = 10;
	victim->max_mana = 100;
	victim->max_move = 100;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	  victim->pcdata->learned[sn] = 0;
	victim->practice = 0;
	victim->hit      = victim->max_hit;
	victim->mana     = victim->max_mana;
	victim->move     = victim->max_move;
	advance_level( victim );
    }
    else
    {
	send_to_char( "Raising a player's level!\r\n", ch );
	send_to_char( "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\r\n", victim );
    }

    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
	send_to_char( "You raise a level!!  ", victim );
	victim->level += 1;
	advance_level( victim );
    }
    victim->exp   = 1000 * UMAX( 1, victim->level );
    victim->trust = 0;
    return;
}



void do_trust( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    int        level;

    rch = get_char( ch );

    if ( !authorized( rch, "trust" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: trust <char> <level>.\r\n", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	send_to_char( "That player is not here.\r\n", ch );
	return;
    }

    level = atoi( arg2 );

    if ( level < 1 || level > MAX_LEVEL )
    {
	char buf [ MAX_STRING_LENGTH ];

	sprintf( buf, "Trust within range 1 to %d.\r\n", MAX_LEVEL );
	send_to_char( buf, ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust.\r\n", ch );
	return;
    }

    /*
     * Little extra check by Canth (phule@xs4all.nl)
     * This works seeing as I'm trusted 55 ;-)
     */
    if ( get_trust(ch) <= get_trust(victim) )
    {
	send_to_char( "You cannot trust your peer nor your superior.\r\n", ch );
	return;
    }

    victim->trust = level;
    return;
}



void do_restore( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "restore" ) )
        return;

    one_argument( argument, arg );

    if (arg[0] == '\0' || !str_cmp(arg,"room"))
    {
    /* cure room */

        for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
        {
            affect_strip(rch,gsn_plague);
            affect_strip(rch,gsn_poison);
            affect_strip(rch,gsn_blindness);
            affect_strip(rch,gsn_sleep);
            affect_strip(rch,gsn_curse);

            rch->hit    = rch->max_hit;
            rch->mana   = rch->max_mana;
            rch->move   = rch->max_move;
            update_pos( rch);
            act("$n has restored you.",ch,NULL, rch,TO_VICT);
        }
	wiznet (ch, WIZ_RESTORE, ch->level + 1, ch->name );
        send_to_char("Room restored.\r\n",ch);
        return;

    }

    /* Restore All feature coded by Katrina */
    if ( !str_cmp( arg, "all" ) )
    {
        for ( victim = char_list; victim; victim = victim->next )
        {
            if ( victim->deleted )
                continue;

            affect_strip(victim,gsn_plague);
            affect_strip(victim,gsn_poison);
            affect_strip(victim,gsn_blindness);
            affect_strip(victim,gsn_sleep);
            affect_strip(victim,gsn_curse);

            victim->hit = victim->max_hit;
            victim->mana = victim->max_mana;
            victim->move = victim->max_move;
            update_pos( victim );
	    wiznet (ch, WIZ_RESTORE, ch->level + 1, ch->name );
            act( "$n has restored you.", ch, NULL, victim, TO_VICT );
        }
        send_to_char( "Aww...how sweet :)...Done.\r\n", ch );
    }
    else
    {
	if ( !( victim = get_char_world( ch, arg ) ) )
	{
		send_to_char( "They aren't here.\r\n", ch );
		return;
	}

	affect_strip(victim,gsn_plague);
	affect_strip(victim,gsn_poison);
	affect_strip(victim,gsn_blindness);
	affect_strip(victim,gsn_sleep);
	affect_strip(victim,gsn_curse);

        victim->hit  = victim->max_hit;
        victim->mana = victim->max_mana;
        victim->move = victim->max_move;
        update_pos( victim );
        act( "$n has restored you.", ch, NULL, victim, TO_VICT );
	wiznet (ch, WIZ_RESTORE, ch->level + 1, ch->name );
        send_to_char( "Ok.\r\n", ch );
    }

    return;
}


void do_freeze( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "freeze" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Freeze whom?\r\n", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\r\n", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\r\n", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_FREEZE ) )
    {
	REMOVE_BIT( victim->act, PLR_FREEZE );
	send_to_char( "FREEZE removed.\r\n",     ch     );
	send_to_char( "You can play again.\r\n", victim );
    }
    else
    {
	SET_BIT(    victim->act, PLR_FREEZE );
	send_to_char( "FREEZE set.\r\n",            ch     );
	send_to_char( "You can't do ANYthing!\r\n", victim );
    }

    save_char_obj( victim );

    return;
}



void do_log( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "log" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Log whom?\r\n", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        if ( fLogAll )
        {
            fLogAll = FALSE;
            send_to_char( "Log ALL off.\r\n", ch );
        }
        else
        {
            fLogAll = TRUE;
            send_to_char( "Log ALL on.\r\n",  ch );
        }
        return;
    }

    if ( !str_cmp( arg, "allmobs" ) )
    {
	if ( fLogAllMobs )
	{
	    fLogAllMobs = FALSE;
	    send_to_char( "Log ALL MOBS off.\r\n", ch );
	}
	else
	{
	    fLogAllMobs = TRUE;
	    send_to_char( "Log ALL MOBS on.\r\n",  ch );
	}
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\r\n", ch );
	return;
    }

    /*
     * No level check, gods can log anyone.
     */
    /*
     * Level check added by Canth, who hates being logged :)
     * But you can log yourself !!! by Maniac... who wants to find bugs
     */
    if (( get_trust(victim) >= get_trust(ch) ) && (ch != victim ))
    {
	send_to_char( "You cannot log your peer nor your superior.\r\n", ch);
	return;
    }

    if ( IS_SET( victim->act, PLR_LOG ) )
    {
	REMOVE_BIT( victim->act, PLR_LOG );
	send_to_char( "LOG removed.\r\n", ch );
    }
    else
    {
	SET_BIT(    victim->act, PLR_LOG );
	send_to_char( "LOG set.\r\n",     ch );
    }

    return;
}



void do_noemote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "noemote" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Noemote whom?\r\n", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\r\n", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\r\n", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_NO_EMOTE ) )
    {
	REMOVE_BIT( victim->act, PLR_NO_EMOTE );
	send_to_char( "NO_EMOTE removed.\r\n",    ch     );
	send_to_char( "You can emote again.\r\n", victim );
    }
    else
    {
	SET_BIT(    victim->act, PLR_NO_EMOTE );
	send_to_char( "You can't emote!\r\n",    victim );
	send_to_char( "NO_EMOTE set.\r\n",       ch     );
    }

    return;
}



void do_notell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "notell" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Notell whom?", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\r\n", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\r\n", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_NO_TELL ) )
    {
	REMOVE_BIT( victim->act, PLR_NO_TELL );
	send_to_char( "NO_TELL removed.\r\n",    ch );
	send_to_char( "You can tell again.\r\n", victim );
    }
    else
    {
	SET_BIT(    victim->act, PLR_NO_TELL );
	send_to_char( "NO_TELL set.\r\n",        ch     );
	send_to_char( "You can't tell!\r\n",     victim );
    }

    return;
}



void do_silence( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "silence" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Silence whom?\r\n", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\r\n", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\r\n", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_SILENCE ) )
    {
	REMOVE_BIT( victim->act, PLR_SILENCE );
	send_to_char( "You can use channels again.\r\n", victim );
	send_to_char( "SILENCE removed.\r\n",            ch     );
    }
    else
    {
	SET_BIT(    victim->act, PLR_SILENCE );
	send_to_char( "You can't use channels!\r\n",     victim );
	send_to_char( "SILENCE set.\r\n",                ch     );
    }

    return;
}


void do_peace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "peace" ) )
        return;

    /* Yes, we are reusing rch.  -Kahn */
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( rch->fighting )
	    stop_fighting( rch, TRUE );
    }

    send_to_char( "Ok.\r\n", ch );
    return;
}



BAN_DATA *		ban_free;
BAN_DATA *		ban_list;

void do_ban( CHAR_DATA *ch, char *argument )
{
    BAN_DATA  *pban;
    CHAR_DATA *rch;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH  ];

    if ( IS_NPC( ch ) )
	return;

    rch = get_char( ch );

    if ( !authorized( rch, "ban" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	strcpy( buf, "Banned sites:\r\n" );
	for ( pban = ban_list; pban; pban = pban->next )
	{
	    strcat( buf, pban->name );
	    strcat( buf, "\r\n" );
	}
	send_to_char( buf, ch );
	return;
    }

    for ( pban = ban_list; pban; pban = pban->next )
    {
	if ( !str_cmp( arg, pban->name ) )
	{
	    send_to_char( "That site is already banned!\r\n", ch );
	    return;
	}
    }

    if ( !ban_free )
    {
	pban		= alloc_perm( sizeof( *pban ) );
    }
    else
    {
	pban		= ban_free;
	ban_free	= ban_free->next;
    }

    pban->name	= str_dup( arg );
    pban->next	= ban_list;
    ban_list	= pban;
    send_to_char( "Ok.\r\n", ch );
    ban_update( );
    return;
}



void do_allow( CHAR_DATA *ch, char *argument )
{
    BAN_DATA  *prev;
    BAN_DATA  *curr;
    CHAR_DATA *rch;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "allow" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove which site from the ban list?\r\n", ch );
	return;
    }

    prev = NULL;
    for ( curr = ban_list; curr; prev = curr, curr = curr->next )
    {
	if ( !str_cmp( arg, curr->name ) )
	{
	    if ( !prev )
		ban_list   = ban_list->next;
	    else
		prev->next = curr->next;

	    free_string( curr->name );
	    curr->next	= ban_free;
	    ban_free	= curr;
	    send_to_char( "Ok.\r\n", ch );
	    ban_update( );
	    return;
	}
    }

    send_to_char( "Site is not banned.\r\n", ch );
    return;
}



void do_wizlock( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA *rch;
    extern bool       wizlock;

    rch = get_char( ch );

    if ( !authorized( rch, "wizlock" ) )
        return;

    wizlock = !wizlock;

    if ( wizlock )
	send_to_char( "Game wizlocked.\r\n", ch );
    else
	send_to_char( "Game un-wizlocked.\r\n", ch );

    return;
}



void do_slookup( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       buf  [ MAX_STRING_LENGTH ];
    char       buf1 [ MAX_STRING_LENGTH*3];
    char       arg  [ MAX_INPUT_LENGTH ];
    int        sn;

    rch = get_char( ch );

    if ( !authorized( rch, "slookup" ) )
        return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slookup what?\r\n", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        buf1[0] = '\0';
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( !skill_table[sn].name )
		break;
	    sprintf( buf, "Sn: %4d Skill/spell: '%s'\r\n",
		    sn, skill_table[sn].name );
	    strcat( buf1, buf );
	}
	send_to_char( buf1, ch );
    }
    else
    {
	if ( is_number( arg ) )
        {
	    sn = atoi( arg );
	    if (   sn >= 0
		&& sn  < MAX_SKILL
		&& skill_table[sn].name )
	    {
		sprintf( buf, "Sn: %4d Skill/spell: '%s'\r\n",
			sn, skill_table[sn].name );
		send_to_char( buf, ch );
		return;
	    }
	}

        if ( ( sn = skill_lookup( arg ) ) < 0 )
	{
	    send_to_char( "No such skill or spell.\r\n", ch );
	    return;
	}

	sprintf( buf, "Sn: %4d Skill/spell: '%s'\r\n",
		sn, skill_table[sn].name );
	send_to_char( buf, ch );
    }

    return;
}



void do_sset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1  [ MAX_INPUT_LENGTH ];
    char       arg2  [ MAX_INPUT_LENGTH ];
    char       arg3  [ MAX_INPUT_LENGTH ];
    int        value;
    int        sn;
    bool       fAll;

    rch = get_char( ch );

    if ( !authorized( rch, "sset" ) )
        return;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: sset <victim> <skill> <value>\r\n",	ch );
	send_to_char( "or:     sset <victim> all     <value>\r\n",	ch );
	send_to_char( "Skill being any skill or spell.\r\n",		ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\r\n", ch );
	return;
    }

    if ( get_trust(ch) <= get_trust(victim) && ch != victim )
    {
	send_to_char( "You may not sset your peer nor your superior.\r\n",
		     ch );
	return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such skill or spell.\r\n", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\r\n", ch );
	return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
	send_to_char( "Value range is 0 to 100.\r\n", ch );
	return;
    }

    if ( fAll )
    {
        if ( get_trust( ch ) < L_DIR )
	{
	    send_to_char( "Only Directors may sset all.\r\n", ch );
	    return;
	}
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name )
          {
	        if ( skill_table[sn].skill_level[victim->class] <= LEVEL_HERO )
		victim->pcdata->learned[sn]	= value;
		else
		    if ( skill_table[sn].skill_level[victim->class]
			<= get_trust( victim ) )
		        victim->pcdata->learned[sn] = value;
          }
	}
    }
    else
    {
        victim->pcdata->learned[sn] = value;
    }

    return;
}



void do_mset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char      *pArg;
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    char       arg3 [ MAX_INPUT_LENGTH  ];
    char       arg4 [ MAX_INPUT_LENGTH  ];
    char       cEnd;
    int        value;
    int        max;

    rch = get_char( ch );

    if ( !authorized( rch, "mset" ) )
        return;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg3;
    while ( isspace( *argument ) )
	argument++;
    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;
    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';
    while ( isspace( *argument ) )
	argument++;

    strcpy( arg4, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: mset <victim> <field>  <value>\r\n",	ch );
	send_to_char( "or:     mset <victim> <string> <value>\r\n",	ch );
	send_to_char( "or:     mset <victim> language <language> <value>", ch );
	send_to_char( "\r\n\r\n",					ch );
	send_to_char( "Field being one of:\r\n",			ch );
	send_to_char( "  str int wis dex con class sex race level\r\n",	ch );
	send_to_char( "  gold mhp mmana mmv practice learn align\r\n",	ch );
	send_to_char( "  thirst drunk full hunt language whotext\r\n",	ch );
	send_to_char( "  shares balance played chp cmana cmv exp\r\n",	ch );
	send_to_char( "  qp qtime qnext securety\r\n",			ch );
	send_to_char( "\r\n",						ch );
	send_to_char( "String being one of:\r\n",			ch );
	send_to_char( "  name short long title spec game\r\n",		ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\r\n", ch );
	return;
    }

    /*
     * Level check added by Canth (phule@xs4all.nl)
     * Maniac: Fixed the f*** check, can mset yourself !!
     */
    if ((get_trust(victim) >= get_trust(ch)) && ch != victim)
    {
	send_to_char( "You may not mset your peer nor your superior.\r\n", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "str" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\r\n", ch );
	    return;
	}

	if ( class_table[ch->class].attr_prime == APPLY_STR )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Strength range is 3 to %d.\r\n", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_str = value;
	return;
    }

    if ( !str_cmp( arg2, "int" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\r\n", ch );
	    return;
	}

	if ( class_table[ch->class].attr_prime == APPLY_INT )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Intelligence range is 3 to %d.\r\n", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_int = value;
	return;
    }

    if ( !str_cmp( arg2, "wis" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\r\n", ch );
	    return;
	}

	if ( class_table[ch->class].attr_prime == APPLY_WIS )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Wisdom range is 3 to %d.\r\n", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_wis = value;
	return;
    }

    if ( !str_cmp( arg2, "dex" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\r\n", ch );
	    return;
	}

	if ( class_table[ch->class].attr_prime == APPLY_DEX )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Dexterity range is 3 to %d.\r\n", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_dex = value;
	return;
    }

    if ( !str_cmp( arg2, "con" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\r\n", ch );
	    return;
	}

	if ( class_table[ch->class].attr_prime == APPLY_CON )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Constitution range is 3 to %d.\r\n", max );
	    send_to_char( buf, ch );
	    return;
	}

	victim->pcdata->perm_con = value;
	return;
    }

    if ( !str_cmp( arg2, "class" ) )
    {
	if ( value < 0 || value >= MAX_CLASS )
	{
	    char buf [ MAX_STRING_LENGTH ];

	    sprintf( buf, "Class range is 0 to %d.\n", MAX_CLASS-1 );
	    send_to_char( buf, ch );
	    return;
	}
	victim->class = value;
	return;
    }

    if ( !str_cmp( arg2, "sex" ) )
    {
        if ( IS_AFFECTED( victim, AFF_CHANGE_SEX ) )
	{
	    send_to_char( "This person is affect by change sex.\r\n", ch );
	    send_to_char( "Try again later.\r\n", ch );
	    return;
	}

	if ( value < 0 || value > 2 )
	{
	    send_to_char( "Sex range is 0 to 2.\r\n", ch );
	    return;
	}

	victim->sex = value;

	return;
    }

    if ( !str_cmp( arg2, "race" ) )
    {
        OBJ_DATA *wield;
	OBJ_DATA *wield2;
	int       race;

	if ( IS_AFFECTED( victim, AFF_POLYMORPH ) )
	{
	    send_to_char( "This person is affected by polymorph other.\r\n",
			 ch );
	    send_to_char( "Try again later.\r\n", ch );
	    return;
	}

	race = race_lookup( arg3 );

	if ( race < 0 )
	{
	    send_to_char( "Invalid race.\r\n", ch );
	    return;
	}

	if (  !IS_SET( race_table[ race ].race_abilities, RACE_PC_AVAIL )
	    && get_trust( ch ) < L_DIR )
	{
	    send_to_char( "You may not set a race not available to PC's.\r\n",
			 ch );
	    return;
	}

	victim->race = race;

	if ( ( wield = get_eq_char( victim, WEAR_WIELD ) )
	    && !IS_SET( race_table[ victim->race ].race_abilities,
		       RACE_WEAPON_WIELD ) )
	{
	    act( "You drop $p.", victim, wield, NULL, TO_CHAR );
	    act( "$n drops $p.", victim, wield, NULL, TO_ROOM );
	    obj_from_char( wield );
	    obj_to_room( wield, victim->in_room );
	}

	if ( ( wield2 = get_eq_char( victim, WEAR_WIELD_2 ) )
	    && !IS_SET( race_table[ victim->race ].race_abilities,
		       RACE_WEAPON_WIELD ) )
	{
	    act( "You drop $p.", victim, wield2, NULL, TO_CHAR );
	    act( "$n drops $p.", victim, wield2, NULL, TO_ROOM );
	    obj_from_char( wield2 );
	    obj_to_room( wield2, victim->in_room );
	}

	return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Not on PC's.\r\n", ch );
	    return;
	}

	if ( value < 0 || value > 50 )
	{
	    send_to_char( "Level range is 0 to 50.\r\n", ch );
	    return;
	}
	victim->level = value;
	return;
    }

    if ( !str_cmp( arg2, "gold" ) )
    {
	victim->gold = value;
	return;
    }

    if ( !str_cmp( arg2, "qp" ) )
    {
	victim->questpoints = value;
	return;
    }

    if ( !str_prefix( arg2, "qtime" ) )
    {
	if (value == 0)
		value++;
	victim->countdown = value;
	return;
    }

    if ( !str_prefix( arg2, "qnext" ) )
    {
	victim->nextquest = value;
	return;
    }

    if ( !str_prefix( arg2, "securety" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\r\n", ch );
	    return;
	}

	if ( value > ch->pcdata->security || value < 0 )
	{
	    if ( ch->pcdata->security != 0 )
	    {
		sprintf( buf, "Valid security is 0-%d.\r\n",
		   ch->pcdata->security );
		send_to_char( buf, ch );
	    }
	    else
	    {
		send_to_char( "Valid security is 0 only.\r\n", ch );
	    }
	    return;
	}
	victim->pcdata->security = value;
	return;
    }

    if ( !str_cmp( arg2, "exp" ) )	/* Maniac */
    {
	victim->exp = value;
	return;
    }

    if ( !str_cmp( arg2, "played" ) )	/* Maniac */
    {
	victim->played = value;
	return;
    }

    if ( !str_cmp( arg2, "balance" ) )		/* Maniac */
    {
	if (IS_NPC(victim))
		return;
	victim->pcdata->balance = value;
	return;
    }

    if ( !str_cmp( arg2, "shares" ) )		/* Maniac */
    {
	if (IS_NPC(victim))
		return;
	victim->pcdata->shares = value;
	return;
    }

    if ( !str_cmp( arg2, "mhp" ) )		/* Maniac */
    {
	if ( value < -10 || value > 30000 )
	{
	    send_to_char( "Hp range is -10 to 30,000 hit points.\r\n", ch );
	    return;
	}
	if ( victim->fighting && value < 0 )
	{
	    send_to_char( "You cannot set a fighting person's hp below 0.\r\n",
			 ch );
	    return;
	}
	victim->max_hit = value;
	return;
    }

    if ( !str_cmp( arg2, "chp" ) )		/* Maniac */
    {
        if ( value < -10 || value > 30000 )
        {
            send_to_char( "Hp range is -10 to 30,000 hit points.\r\n", ch );
            return;
        }
        if ( victim->fighting && value < 0 )
        {
            send_to_char( "You cannot set a fighting person's hp below 0.\r\n",
                         ch );
            return;
        }
        victim->hit = value;
        return;
    }

    if ( !str_cmp( arg2, "mmana" ) )		/* Maniac */
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Mana range is 0 to 30,000 mana points.\r\n", ch );
	    return;
	}
	victim->max_mana = value;
	return;
    }

    if ( !str_cmp( arg2, "cmana" ) )		/* Maniac */
    {
        if ( value < 0 || value > 30000 )
        {
            send_to_char( "Mana range is 0 to 30,000 mana points.\r\n", ch );
            return;
        }
        victim->mana = value;
        return;
    }

    if ( !str_cmp( arg2, "mmv" ) )		/* Maniac */
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Move range is 0 to 30,000 move points.\r\n", ch );
	    return;
	}
	victim->max_move = value;
	return;
    }

    if ( !str_cmp( arg2, "cmv" ) )		/* Maniac */
    {
        if ( value < 0 || value > 30000 )
        {
            send_to_char( "Move range is 0 to 30,000 move points.\r\n", ch );
            return;
        }
        victim->move = value;
        return;
    }

    if ( !str_cmp( arg2, "practice" ) )
    {
	victim->practice = value;
	return;
    }

    if ( !str_cmp( arg2, "learn" ) )		/* Maniac */
    {
	if (IS_NPC(ch))
		send_to_char("Not on NPC's\r\n", ch);
	else
		victim->pcdata->learn = value;
        return;
    }

    if ( !str_cmp( arg2, "align" ) )
    {
	if ( value < -1000 || value > 1000 )
	{
	    send_to_char( "Alignment range is -1000 to 1000.\r\n", ch );
	    return;
	}
	victim->alignment = value;
	return;
    }

    if ( !str_cmp( arg2, "thirst" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\r\n", ch );
	    return;
	}

	/* Check for victim's level removed by Canth (phule@xs4all.nl)
	if ( ( value < 0 || value > 100 )
	    && get_trust( victim ) < LEVEL_IMMORTAL )
	{
	    send_to_char( "Thirst range is 0 to 100.\r\n", ch );
	    return;
	}
	else */
	    if ( value < -1 || value > 100 )
	    {
		send_to_char( "Thirst range is -1 to 100.\r\n", ch );
		return;
	    }

	victim->pcdata->condition[COND_THIRST] = value;
	return;
    }

    if ( !str_cmp( arg2, "drunk" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\r\n", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Drunk range is 0 to 100.\r\n", ch );
	    return;
	}

	victim->pcdata->condition[COND_DRUNK] = value;
	return;
    }

    if ( !str_cmp( arg2, "full" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\r\n", ch );
	    return;
	}

	/* victim level check removed by Canth (phule@xs4all.nl)
	if ( ( value < 0 || value > 100 )
	    && get_trust( victim ) < LEVEL_IMMORTAL )
	{
	    send_to_char( "Full range is 0 to 100.\r\n", ch );
	    return;
	}
	else */
	    if ( value < -1 || value > 100 )
	    {
		send_to_char( "Full range is -1 to 100.\r\n", ch );
		return;
	    }

	victim->pcdata->condition[COND_FULL] = value;
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Not on PC's.\r\n", ch );
	    return;
	}

	if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->name );
	victim->name = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->short_descr );
	victim->short_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->long_descr );
	strcat( arg3, "\r\n" );
	victim->long_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "title" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\r\n", ch );
	    return;
	}

	set_title( victim, arg3 );
	return;
    }

    /*
     * Whotext by Canth, phule@xs4all.nl
     */
    if ( !str_cmp( arg2, "whotext" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\r\n", ch );
	    return;
	}

	/* If the string is longer than 23 chars, cut it off */
	if ( strlen ( arg3 ) > 23 )
	    arg3[23] = '\0';
	/*
	 * Chil (bbailey@mail.public.lib.ga.us) uses a boolean routine
	 * to check if it is more than 23 chars.
	 * This way he can call it everytime he needs it.
	 */

	/*
	 * Bugfix for resetting who_text by Chil (bbailey@mail.public.lib.ga.us)
	 */
	if ( !str_cmp( arg3, "@" ) )
	{
	    victim->pcdata->who_text = strdup( "@" );
	    return;
	}

	/* add (23-length of word)/2 spaces behind arg3 */
	/* This is needed to centre the text in the who listing */

	max = ( ( 23 - strlen(arg3) ) / 2 );
	while ( max >=1 )
	{
	    strcat ( arg3, " " );
	    max--;
	}
	free_string ( victim->pcdata->who_text );
	victim->pcdata->who_text = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "spec" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Not on PC's.\r\n", ch );
	    return;
	}

	if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
	{
	    send_to_char( "No such spec fun.\r\n", ch );
	    return;
	}

	return;
    }

    if ( !str_cmp( arg2, "game" ) )
    {
        if ( !IS_NPC( victim ) )
        {
            send_to_char( "Not on PC's.\r\n", ch );
            return;
        }

        if ( ( victim->game_fun = game_lookup( arg3 ) ) == 0 )
        {
            send_to_char( "No such game fun.\r\n", ch );
            return;
        }

        return;
    }


    if (!str_cmp(arg2, "hunt"))
    {
        CHAR_DATA *hunted = 0;

        if ( !IS_NPC(victim) )
        {
            send_to_char( "Not on PC's.\r\n", ch );
            return;
        }

        if ( str_cmp( arg3, "." ) )
          if ( (hunted = get_char_area(victim, arg3)) == NULL )
            {
              send_to_char("Mob couldn't locate the victim to hunt.\r\n", ch);
              return;
            }

        victim->hunting = hunted;
        return;
    }

    if (!str_cmp(arg2, "language"))		/* Call to do_lset by Maniac */
    {
	if (arg4[0] == '\0')
	{
	    do_mset( ch, "" );
	    return;
	}
        if ( IS_NPC(victim) )
        {
            send_to_char( "Not on NPC's.\r\n", ch );
            return;
        }
	sprintf( buf, "%s %s %s", victim->name, arg3, arg4 );
	do_lset( ch, buf );
	return;
    }



    /*
     * Generate usage message.
     */
    do_mset( ch, "" );
    return;
}



void do_oset( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *rch;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    char       arg3 [ MAX_INPUT_LENGTH ];
    char       arg4 [ MAX_INPUT_LENGTH ];
    int        value;

    rch = get_char( ch );

    if ( !authorized( rch, "oset" ) )
        return;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: oset <object> <field>  <value>\r\n",	ch );
	send_to_char( "or:     oset <object> <string> <value>\r\n",	ch );
	send_to_char( "\r\n",						ch );
	send_to_char( "Field being one of:\r\n",			ch );
	send_to_char( "  value0 value1 value2 value3\r\n",		ch );
	send_to_char( "  extra wear level weight cost timer\r\n",	ch );
	send_to_char( "\r\n",						ch );
	send_to_char( "String being one of:\r\n",			ch );
	send_to_char( "  name short long ed\r\n",			ch );
	return;
    }

    if ( !( obj = get_obj_world( ch, arg1 ) ) )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\r\n", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
    {
	obj->value[0] = value;
	return;
    }

    if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
    {
	obj->value[1] = value;
	return;
    }

    if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
    {
	obj->value[2] = value;
	return;
    }

    if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
    {
	obj->value[3] = value;
	return;
    }

    if ( !str_cmp( arg2, "extra" ) )
    {
	obj->extra_flags = value;
	return;
    }

    if ( !str_cmp( arg2, "wear" ) )
    {
	obj->wear_flags = value;
	return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	obj->level = value;
	return;
    }
	
    if ( !str_cmp( arg2, "weight" ) )
    {
	if ( obj->carried_by != NULL && !IS_NPC( obj->carried_by ) )
	{
	    send_to_char(
		"You may not modify an item's weight while on a PC.\r\n",
			 ch);
	    return;
	}
	obj->weight = value;
	return;
    }

    if ( !str_cmp( arg2, "cost" ) )
    {
	obj->cost = value;
	return;
    }

    if ( !str_cmp( arg2, "timer" ) )
    {
	obj->timer = value;
	return;
    }
	
    if ( !str_cmp( arg2, "name" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->name );
	obj->name = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->short_descr );
	obj->short_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->description );
	obj->description = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "ed" ) )
    {
	EXTRA_DESCR_DATA *ed;

	argument = one_argument( argument, arg3 );
	argument = one_argument( argument, arg4 );
	if ( arg4[0] == '\0' )
	{
	    send_to_char( "Syntax: oset <object> ed <keyword> <string>\r\n",
		ch );
	    return;
	}

	if ( !extra_descr_free )
	{
	    ed			= alloc_perm( sizeof( *ed ) );
	}
	else
	{
	    ed			= extra_descr_free;
	    extra_descr_free	= extra_descr_free->next;
	}

	ed->keyword		= str_dup( arg3     );
	ed->description		= str_dup( argument );
	ed->deleted             = FALSE;
	ed->next		= obj->extra_descr;
	obj->extra_descr	= ed;
	return;
    }

    /*
     * Generate usage message.
     */
    do_oset( ch, "" );
    return;
}



void do_rset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *person;
    ROOM_INDEX_DATA *location;
    char             arg1 [ MAX_INPUT_LENGTH ];
    char             arg2 [ MAX_INPUT_LENGTH ];
    char             arg3 [ MAX_INPUT_LENGTH ];
    int              value;

    rch = get_char( ch );

    if ( !authorized( rch, "rset" ) )
        return;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: rset <location> <field> value\r\n",	ch );
	send_to_char( "\r\n",						ch );
	send_to_char( "Field being one of:\r\n",			ch );
	send_to_char( "  flags sector\r\n",				ch );
	return;
    }

    if ( !( location = find_location( ch, arg1 ) ) )
    {
	send_to_char( "No such location.\r\n", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\r\n", ch );
	return;
    }
    value = atoi( arg3 );

    for ( person = location->people; person;
	 person = person->next_in_room )
	if ( person != ch && person->level >= ch->level )
	  {
	    send_to_char(
		"Your superior is in this room, no rsetting now.\r\n", ch );
	    return;
	  }

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "flags" ) )
    {
	location->room_flags	= value;
	return;
    }

    if ( !str_cmp( arg2, "sector" ) )
    {
	location->sector_type	= value;
	return;
    }

    /*
     * Generate usage message.
     */
    do_rset( ch, "" );
    return;
}


/*
void do_users( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    DESCRIPTOR_DATA *d;
    char            buf  [ MAX_STRING_LENGTH ];
    char            buf2 [ MAX_STRING_LENGTH ];
    int             count;

    rch = get_char( ch );

    if ( !authorized( rch, "users" ) )
        return;

    count	= 0;
    buf[0]	= '\0';
    buf2[0]     = '\0';
    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->character && can_see( ch, d->character ) )
	{
	    count++;
	    sprintf( buf + strlen( buf ), "[%3d %2d] %s@%s\r\n",
		    d->descriptor,
		    d->connected,
		    d->original  ? d->original->name  :
		    d->character ? d->character->name : "(none)",
		    d->host );
	}
    }

    sprintf( buf2, "%d user%s\r\n", count, count == 1 ? "" : "s" );
    strcat( buf, buf2 );
    send_to_char( buf, ch );
    return;
}
*/


/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        trust;
    int        cmd;

    rch = get_char( ch );

    if ( !authorized( rch, "force" ) )
        return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Force whom to do what?\r\n", ch );
	return;
    }

    /*
     * Look for command in command table.
     */
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( argument[0] == cmd_table[cmd].name[0]
	    && !str_prefix( argument, cmd_table[cmd].name )
	    && ( cmd_table[cmd].level > trust ) )
	{
	  send_to_char( "You can't even do that yourself!\r\n", ch );
	  return;
	}
    }

    if ( !str_cmp( arg, "all" ) )
    {
	CHAR_DATA *vch;

	for ( vch = char_list; vch; vch = vch->next )
	{
	    if ( vch->deleted )
	        continue;

	    if ( !IS_NPC( vch ) && get_trust( vch ) < get_trust( ch ) )
	    {
		act( "$n forces you to '$t'.", ch, argument, vch, TO_VICT );
		interpret( vch, argument );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( !( victim = get_char_world( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\r\n", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\r\n", ch );
	    return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    send_to_char( "Do it yourself!\r\n", ch );
	    return;
	}

	act( "$n forces you to '$t'.", ch, argument, victim, TO_VICT );
	interpret( victim, argument );
    }

    send_to_char( "Ok.\r\n", ch );
    return;
}



/*
 * New routines by Dionysos.
 */
void do_invis( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( !authorized( ch, "wizinvis" ) )
        return;

    if ( IS_SET( ch->act, PLR_WIZINVIS ) )
    {
	REMOVE_BIT( ch->act, PLR_WIZINVIS );
	send_to_char( "You slowly fade back into existence.\r\n", ch    );
	act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
	SET_BIT( ch->act, PLR_WIZINVIS );
	send_to_char( "You slowly vanish into thin air.\r\n",     ch    );
	act( "$n slowly fades into thin air.",  ch, NULL, NULL, TO_ROOM );
    }

    return;
}



void do_holylight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( !authorized( ch, "holylight" ) )
        return;

    if ( IS_SET( ch->act, PLR_HOLYLIGHT ) )
    {
	REMOVE_BIT( ch->act, PLR_HOLYLIGHT );
	send_to_char( "Holy light mode off.\r\n", ch );
    }
    else
    {
	SET_BIT(    ch->act, PLR_HOLYLIGHT );
	send_to_char( "Holy light mode on.\r\n", ch );
    }

    return;
}

/* Wizify and Wizbit sent in by M. B. King */

void do_wizify( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
  
    rch = get_char( ch );

    if ( !authorized( rch, "wizify" ) )
        return;

    one_argument( argument, arg1  );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: wizify <name>\r\n" , ch );
	return;
    }
    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\r\n" , ch );
	return;
    }
    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on mobs.\r\n", ch );
	return;
    }

    
    if ( !IS_SET( victim->act, PLR_WIZBIT ) )
    {
	SET_BIT( victim->act, PLR_WIZBIT );
	act( "$N wizified.",         ch, NULL, victim, TO_CHAR );
	act( "$n has wizified you!", ch, NULL, victim, TO_VICT );
    }
    else
    {
	REMOVE_BIT( victim->act, PLR_WIZBIT );
	act( "$N dewizzed.",         ch, NULL, victim, TO_CHAR );
	act( "$n has dewizzed you!", ch, NULL, victim, TO_VICT ); 
    }

    do_save( victim, "");
    return;
}
