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
#include "olc.h"

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
	    strcat( buf1, "\n\r" );
    }
 
    if ( col % 8 != 0 )
	strcat( buf1, "\n\r" );
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
	send_to_char( "Ok.\n\r", ch );
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
	send_to_char( "Ok.\n\r", ch );
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
	send_to_char( "Deny whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    SET_BIT( victim->act, PLR_DENY );
    send_to_char( "You are denied access!\n\r", victim );
    send_to_char( "OK.\n\r", ch );
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
	send_to_char( "Disconnect whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
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
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }

    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char( "Descriptor not found!\n\r", ch );
    return;
}



void do_pardon( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    char       buf  [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "pardon" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: pardon <character> <killer|thief|pk>.\n\r", ch );
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

    if ( !str_cmp( arg2, "killer" ) )
    {
	if ( IS_SET( victim->act, PLR_KILLER ) )
	{
	    REMOVE_BIT( victim->act, PLR_KILLER );
	    send_to_char( "Killer flag removed.\n\r",        ch     );
	    send_to_char( "You are no longer a KILLER.\n\r", victim );
            sprintf( buf, "%s was pardonned for KILLER by %s",
		    victim->name, ch->name );
            wiznet( ch, WIZ_FLAGS, get_trust( rch ), buf );
	}
	return;
    }

    if ( !str_cmp( arg2, "thief" ) )
    {
	if ( IS_SET( victim->act, PLR_THIEF  ) )
	{
	    REMOVE_BIT( victim->act, PLR_THIEF  );
	    send_to_char( "Thief flag removed.\n\r",        ch     );
	    send_to_char( "You are no longer a THIEF.\n\r", victim );
	    send_to_char( "You are no longer a KILLER.\n\r", victim );
            sprintf( buf, "%s was pardonned for THIEF by %s",
		    victim->name, ch->name );
            wiznet( ch, WIZ_FLAGS, get_trust( rch ), buf );
	}
	return;
    }

    if ( !str_cmp( arg2, "pk" ) )
    {
	if ( IS_SET( victim->act, PLR_REGISTER  ) )
	{
	    REMOVE_BIT( victim->act, PLR_REGISTER );
	    send_to_char( "PK flag removed.\n\r", ch );
	    send_to_char( "You can no longer PK.\n\r", victim );
	    sprintf( buf, "%s was pardonned for PK by %s", victim->name,
		    ch->name );
	    wiznet( ch, WIZ_FLAGS, get_trust( rch ), buf );
	}
	return;
    }

    send_to_char( "Syntax: pardon <character> <killer|thief|pk>.\n\r", ch );
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
	send_to_char( "Echo what?\n\r", ch );
	return;
    }

    strcat( argument, "{x\n\r" );
    send_to_all_char( argument );

    return;
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
	send_to_char( "Recho what?\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	    && d->character->in_room == ch->in_room )
	{
	    send_to_char( argument, d->character );
	    send_to_char( "{x\n\r",   d->character );
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
	send_to_char( "Transfer whom (and where)?\n\r", ch );
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
	    send_to_char( "No such location.\n\r", ch );
	    return;
	}

	if ( room_is_private( location ) )
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !victim->in_room )
    {
	send_to_char( "They are in limbo.\n\r", ch );
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
    send_to_char( "Ok.\n\r", ch );
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
	send_to_char( "At where what?\n\r", ch );
	return;
    }

    if ( !( location = find_location( ch, arg ) ) )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
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
	send_to_char( "Goto where?\n\r", ch );
	return;
    }

    if ( !( location = find_location( ch, arg ) ) )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
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



void do_astat( CHAR_DATA *ch, char *argument )
{
    AREA_DATA       *pArea;
    CHAR_DATA       *rch;
    char             buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "astat" ) )
        return;

    smash_tilde( argument );

    if ( is_number( argument ) )
        pArea = get_area_data( atoi( argument ) );
    else
        pArea = ch->in_room->area;
        
    sprintf( buf, "{cName:     [%5d] '{x%s{c'.{x\n\r",
	    pArea->vnum, pArea->name );
    send_to_char( buf, ch );

    sprintf( buf, "{cRange:    {{{x%2d %2d{c}  {cAuthor:   {x%s{c.\n\r",
	    pArea->llv, pArea->ulv, pArea->author );
    send_to_char( buf, ch );

    sprintf( buf, "{cFile:     {x%s{c.{x\n\r",
	    pArea->filename );
    send_to_char( buf, ch );

    sprintf( buf, "{cAge:      [{x%5d{c]  Players: [{x%5d{c]  Security: [{x%5d{c]{x\n\r",
	    pArea->age, pArea->nplayer, pArea->security );
    send_to_char( buf, ch );

    sprintf( buf, "{cVnums:    {x%d{c - {x%d\n\r",
	    pArea->lvnum, pArea->uvnum );
    send_to_char( buf, ch );

    sprintf( buf, "{cRooms:    {x%d{c - {x%d{c Objs: {x%d{c - {x%d{c Mobs: {x%d{c - {x%d\n\r",
	    pArea->low_r_vnum, pArea->hi_r_vnum, pArea->low_o_vnum,
	    pArea->hi_o_vnum, pArea->low_m_vnum, pArea->hi_m_vnum );
    send_to_char( buf, ch );

    sprintf( buf, "{cRecall:   [{x%5d{c] '{x%s{c'.{x\n\r", pArea->recall,
	    get_room_index( pArea->recall )
	    ? get_room_index( pArea->recall )->name
	    : "none" );
    send_to_char( buf, ch );

    sprintf( buf, "{cFlags:    {x%s\n\r",
	    flag_string( area_flags, pArea->area_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "{cReset:    {x%s\n\r", pArea->resetmsg );
    send_to_char( buf, ch );

    sprintf( buf, "{cBuilders: {x%s\n\r", pArea->builders );
    send_to_char( buf, ch );

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
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if ( ch->in_room != location && room_is_private( location ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    buf1[0] = '\0';

    sprintf( buf, "{cName: '{x%s{c'.{x\n\r{cArea: [%5d] '{x%s{c'.{x\n\r",
	    location->name,
	    location->area->vnum,
	    location->area->name );
    strcat( buf1, buf );

    sprintf( buf,
	    "{cVnum: {x%d{c.  Light: {x%d{c.  Sector: {x%s{c.{x\n\r",
	    location->vnum,
	    location->light,
	    flag_string( sector_flags, location->sector_type ) );
    strcat( buf1, buf );

    sprintf( buf,
	    "{cTemp Room flags: {x%s{c.{x\n\r",
	    flag_string( room_flags, location->room_flags ) );
    strcat( buf1, buf );

    sprintf( buf,
	    "{cOrig Room flags: {x%s{c.{x\n\r{cDescription:{x\n\r{x%s",
	    flag_string( room_flags, location->orig_room_flags ),
	    location->description );
    strcat( buf1, buf );

    if ( location->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "{cExtra description keywords: '{x" );
	for ( ed = location->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, " " );
	}
	strcat( buf1, "{c'.{x\n\r" );
    }

    strcat( buf1, "{cCharacters:{x" );

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

    strcat( buf1, "{c.\n\rObjects:   {x" );
    for ( obj = location->contents; obj; obj = obj->next_content )
    {
	strcat( buf1, " " );
	one_argument( obj->name, buf );
	strcat( buf1, buf );
    }
    strcat( buf1, "{c.{x\n\r" );

    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = location->exit[door] ) )
	{
	    sprintf( buf,
		    "{x%-5s{c:  To: {x%d{c.  Key: {x%d{c.  Exit flags: {x%s{c.{x\n\r",
		    capitalize( dir_name[door] ),
		    pexit->to_room ? pexit->to_room->vnum : 0,
		    pexit->key,
		    flag_string( exit_flags, pexit->exit_info ) );
	    strcat( buf1, buf );
	    sprintf( buf,
		    "{cKeyword: '{x%s{c'.  Description: {x%s",
		    pexit->keyword,
		    pexit->description[0] != '\0' ? pexit->description
		                                  : "(none).\n\r" );
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
	send_to_char( "Ostat what?\n\r", ch );
	return;
    }

    buf1[0] = '\0';

    if ( !( obj = get_obj_world( ch, arg ) ) )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch);
	return;
    }

    sprintf( buf, "{cName:{x %s{c.{x\n\r",
	    obj->name );
    strcat( buf1, buf );

    sprintf( buf, "{cVnum: {x%d{c.  Type: {x%s{c.{x\n\r",
	    obj->pIndexData->vnum, item_type_name( obj ) );
    strcat( buf1, buf );

    sprintf( buf, "{cShort description: {x%s{c.{x\n\r{cLong description: {x%s\n\r",
	    obj->short_descr, obj->description );
    strcat( buf1, buf );

    sprintf( buf, "{cWear bits: {x%d{c.  Extra bits: {x%s{c.{x\n\r",
	    obj->wear_flags, extra_bit_name( obj->extra_flags ) );
    strcat( buf1, buf );

    sprintf( buf, "{cNumber: {x%d{c/{x%d{c.  Weight: {x%d{c/{x%d{c.{x\n\r",
	    1,           get_obj_number( obj ),
	    obj->weight, get_obj_weight( obj ) );
    strcat( buf1, buf );

    sprintf( buf, "{cCost: {x%d{c.  Timer: {x%d{c.  Level: {x%d{c.{x\n\r",
	    obj->cost, obj->timer, obj->level );
    strcat( buf1, buf );

    sprintf( buf,
	    "{cIn room: {x%d{c.  In object: {x%s{c.  Carried by: {x%s{c.  Wear_loc: {x%d{c.{x\n\r",
	    !obj->in_room    ?        0 : obj->in_room->vnum,
	    !obj->in_obj     ? "(none)" : obj->in_obj->short_descr,
	    !obj->carried_by ? "(none)" : obj->carried_by->name,
	    obj->wear_loc );
    strcat( buf1, buf );
    
    sprintf( buf, "{cValues: {x%d %d %d %d %d.\n\r",
	    obj->value[0], obj->value[1], obj->value[2], obj->value[3],
								obj->value[4] );
    strcat( buf1, buf );

    if ( obj->extra_descr || obj->pIndexData->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "{cExtra description keywords: '{x" );

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

	strcat( buf1, "{c'.{x\n\r" );
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
	sprintf( buf, "{cAffects {x%s{c by {x%d{c, Bits {x%s{c.{x\n\r",
		affect_loc_name( paf->location ), paf->modifier,
		affect_bit_name( paf->bitvector ) );
	strcat( buf1, buf );
    }

    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
    {
	sprintf( buf, "{cAffects {x%s{c by {x%d{c, Bits {x%s{c.{x\n\r",
		affect_loc_name( paf->location ), paf->modifier,
		affect_bit_name( paf->bitvector ) );
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
	send_to_char( "Mstat whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    buf1[0] = '\0';

    sprintf( buf, "{cName: {x%s{c.{x\n\r",
	    victim->name );
    strcat( buf1, buf );

    sprintf( buf, "{cRace: {x%s{c.{x\n\r", race_table[victim->race].name );
    strcat( buf1, buf );

    sprintf( buf, "{cParts: {x%s{c.{x\n\r",
	    parts_bit_name( race_table[victim->race].parts ) );
    strcat( buf1, buf );

    sprintf( buf, "{cVnum: {x%d{c.  Sex: {x%s{c.  Room: {x%d{c.{x\n\r",
	    IS_NPC( victim ) ? victim->pIndexData->vnum : 0,
	    victim->sex == SEX_MALE    ? "male"   :
	    victim->sex == SEX_FEMALE  ? "female" : "neuter",
	    !victim->in_room           ?        0 : victim->in_room->vnum );
    strcat( buf1, buf );

    sprintf( buf, "{cStr: {x%d{c.  Int: {x%d{c.  Wis: {x%d{c.  Dex: {x%d{c.  Con: {x%d{c.{x\n\r",
	    get_curr_str( victim ),
	    get_curr_int( victim ),
	    get_curr_wis( victim ),
	    get_curr_dex( victim ),
	    get_curr_con( victim ) );
    strcat( buf1, buf );

    sprintf( buf, "{cHp: {x%d{c/{x%d{c.  Mana: {x%d{c/{x%d{c.  Move: {x%d{c/{x%d{c.  Practices: {x%d{c.{x\n\r",
	    victim->hit,         victim->max_hit,
	    victim->mana,        victim->max_mana,
	    victim->move,        victim->max_move,
	    victim->practice );
    strcat( buf1, buf );
	
    sprintf( buf,
	"{cLv: {x%d{c.  Class: {x%d{c.  Align: {x%d{c.  AC: {x%d{c.  Gold: {x%d{c.  Exp: {x%d{c.{x\n\r",
	    victim->level,       victim->class,        victim->alignment,
	    GET_AC( victim ),    victim->gold,         victim->exp );
    strcat( buf1, buf );

    sprintf( buf, "{cPosition: {x%d{c.  Wimpy: {x%d{c.{x\n\r",
            victim->position,    victim->wimpy );
    strcat( buf1, buf );

    if ( IS_NPC( victim )
        || victim->level >= skill_table[gsn_dual].skill_level[victim->class] )
	strcat ( buf1, "{cPrimary Weapon {x" );
    sprintf( buf, "{cHitroll: {x%d{c  Damroll: {x%d{c.{x\n\r",
            get_hitroll( victim, WEAR_WIELD ),
            get_damroll( victim, WEAR_WIELD ) );
    strcat( buf1, buf );

    if ( get_eq_char( victim, WEAR_WIELD_2 ) )
    {
	sprintf( buf, "{c Second Weapon Hitroll: {x%d{c  Damroll: {x%d{c.{x\n\r",
		get_hitroll( victim, WEAR_WIELD_2 ),
		get_damroll( victim, WEAR_WIELD_2 ) );
	strcat( buf1, buf );
    }

    if ( !IS_NPC( victim ) )
    {
	sprintf( buf, "{cPage Lines: {x%d{c.{x\n\r", victim->pcdata->pagelen );
	strcat( buf1, buf );
    }

    sprintf( buf, "{cFighting: {x%s{c.{x\n\r",
	    victim->fighting ? victim->fighting->name : "(none)" );
    strcat( buf1, buf );

    if ( !IS_NPC( victim ) )
    {
	sprintf( buf,
		"{cThirst: {x%d{c.  Full: {x%d{c.  Drunk: {x%d{c.  Saving throw: {x%d{c.{x\n\r",
		victim->pcdata->condition[COND_THIRST],
		victim->pcdata->condition[COND_FULL  ],
		victim->pcdata->condition[COND_DRUNK ],
		victim->saving_throw );
	strcat( buf1, buf );
	if ( is_clan( victim ) )
	{
	    sprintf( buf,
		    "{cClan: {x%s{c.  Rank: {x%s{c.{x\n\r",
		    victim->pcdata->clan->name,
		    flag_string( rank_flags, victim->pcdata->rank ) );
	    strcat( buf1, buf );
	}
    }

    sprintf( buf, "{cCarry number: {x%d{c.  Carry weight: {x%d{c.{x\n\r",
	    victim->carry_number, victim->carry_weight );
    strcat( buf1, buf );

    sprintf( buf, "{cAge: {x%d{c.  Played: {x%d{c.  Timer: {x%d{c.  Act: {x%d{c.{x\n\r",
	    get_age( victim ),
	    (int) victim->played,
	    victim->timer,
	    victim->act );
    strcat( buf1, buf );

    sprintf( buf, "{cRiding:  {x%s{c.  {cRider:  {x%s{c.{x\n\r",
	    victim->riding      ? victim->riding->name   : "(none)",
	    victim->rider       ? victim->rider->name    : "(none)" );
    strcat( buf1, buf );

    sprintf( buf, "{cMaster:  {x%s{c.  Leader: {x%s{c.{x\n\r",
	    victim->master      ? victim->master->name   : "(none)",
	    victim->leader      ? victim->leader->name   : "(none)" );
    strcat( buf1, buf );

    sprintf( buf, "{cHunting: {x%s{c.  Hating: {x%s{c.  Fearing: {x%s{c.{x\n\r",
	    victim->hunting     ? victim->hunting->name  : "(none)",
	    victim->hating      ? victim->hating->name   : "(none)",
	    victim->fearing     ? victim->fearing->name  : "(none)" );
    strcat( buf1, buf );

    sprintf( buf, "{cResistant:   {x%s{c.{x\n\r",
	    ris_bit_name( victim->resistant ) );
    strcat( buf1, buf );

    sprintf( buf, "{cImmune:      {x%s{c.{x\n\r",
	    ris_bit_name( victim->immune ) );
    strcat( buf1, buf );

    sprintf( buf, "{cSusceptible: {x%s{c.{x\n\r",
	    ris_bit_name( victim->susceptible ) );
    strcat( buf1, buf );

    sprintf( buf, "{cAffected by: {x%s{c.{x\n\r",
	    affect_bit_name( victim->affected_by ) );
    strcat( buf1, buf );


    if ( !IS_NPC( victim ) )
    {
        sprintf( buf, "{cSecurity: {x%d{c.{x\n\r",
		victim->pcdata->security );
        strcat( buf1, buf );
    }

    sprintf( buf, "{cShort description: {x%s{c.{x\n\r{cLong  description: {x%s",
	    victim->short_descr,
	    victim->long_descr[0] != '\0' ? victim->long_descr
	                                  : "(none).\n\r" );
    strcat( buf1, buf );

    if ( IS_NPC( victim ) && victim->spec_fun != 0 )
    {
	sprintf( buf, "{cMobile has {x%s{c.{x\n\r",
		spec_mob_string( victim->spec_fun ) );
        strcat( buf1, buf );
    }

    if ( IS_NPC( victim )
	&& victim->pIndexData
	&& victim->pIndexData->pGame
	&& victim->pIndexData->pGame->game_fun != 0 )
    {
	sprintf( buf, "{cMobile has {x%s{c.{x\n\r",
		game_string( victim->pIndexData->pGame->game_fun ) );
        strcat( buf1, buf );
    }

    for ( paf = victim->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
	sprintf( buf,
		"{cSpell: '{x%s{c' modifies {x%s{c by {x%d{c for {x%d{c hours with bits {x%s{c.{x\n\r",
		skill_table[paf->type].name,
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
           AREA_DATA      *pArea;
           char            buf  [ MAX_STRING_LENGTH   ];
           char            arg  [ MAX_INPUT_LENGTH    ];
           char            arg1 [ MAX_INPUT_LENGTH    ];
    extern int             top_vnum_mob;
           int             vnum;
	   int             bottom;
	   int             top;
	   bool            fAll;
	   bool            fWorld;
	   bool            found;

    rch = get_char( ch );

    if ( !authorized( rch, "mfind" ) )
        return;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

    if ( arg[0] == '\0' ||
	( str_cmp( arg, "world" ) && str_cmp( arg, "area" ) ) )
    {
	send_to_char( "Syntax: mfind world <mobile>|all\n\r", ch );
	send_to_char( "or:     mfind area  <mobile>|all\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Mfind whom?\n\r", ch );
	return;
    }

    fAll    = !str_cmp( arg1, "all" );
    found   = FALSE;
    pArea   = ch->in_room->area;
    fWorld  = !str_cmp( arg, "world" );

    if ( fWorld )
    {
	bottom	= 0;
	top	= top_vnum_mob;
    }
    else
    {
	bottom	= pArea->low_m_vnum;
	top	= pArea->hi_m_vnum + 1;
    } 

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = bottom; vnum < top; vnum++ )
    {
	if ( !( pMobIndex = get_mob_index( vnum ) ) )
	    continue;

	if ( !fWorld && pArea != pMobIndex->area )
	    continue;

	if ( fAll || is_name( arg1, pMobIndex->player_name ) )
	{
	    found = TRUE;
	    sprintf( buf, "[%5d] %s\n\r",
		pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
	    send_to_char( buf, ch );
	}
    }

    if ( !found )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch);
	return;
    }

    return;
}



void do_ofind( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA      *rch;
	   OBJ_INDEX_DATA *pObjIndex;
           AREA_DATA      *pArea;
	   char            buf  [ MAX_STRING_LENGTH   ];
	   char            arg  [ MAX_INPUT_LENGTH    ];
           char            arg1 [ MAX_INPUT_LENGTH    ];
    extern int             top_vnum_obj;
           int             vnum;
	   int             bottom;
	   int             top;
	   bool            fAll;
	   bool            fWorld;
	   bool            found;

    rch = get_char( ch );

    if ( !authorized( rch, "ofind" ) )
        return;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

    if ( arg[0] == '\0' ||
	( str_cmp( arg, "world" ) && str_cmp( arg, "area" ) ) )
    {
	send_to_char( "Syntax: ofind world <object>|all\n\r", ch );
	send_to_char( "or:     ofind area  <object>|all\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Ofind what?\n\r", ch );
	return;
    }

    fAll    = !str_cmp( arg1, "all" );
    found   = FALSE;
    pArea   = ch->in_room->area;
    fWorld  = !str_cmp( arg, "world" );

    if ( fWorld )
    {
	bottom	= 0;
	top	= top_vnum_obj;
    }
    else
    {
	bottom	= pArea->low_o_vnum;
	top	= pArea->hi_o_vnum + 1;
    } 

    for ( vnum = bottom; vnum < top; vnum++ )
    {
	if ( !( pObjIndex = get_obj_index( vnum ) ) )
	    continue;

	if ( !fWorld && pArea != pObjIndex->area )
	    continue;

	if ( fAll || is_name( arg1, pObjIndex->name ) )
	{
	    found = TRUE;
	    sprintf( buf, "[%5d] %s\n\r",
		pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
	    send_to_char( buf, ch );
	}
    }

    if ( !found )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch);
	return;
    }

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
	send_to_char( "Mwhere whom?\n\r", ch );
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
	    sprintf( buf, "[%5d] %-28s [%5d] %s\n\r",
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

    send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
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

    send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
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
    strcat( buf, "\n\r" );
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
    char             buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "snoop" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Snoop whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !victim->desc )
    {
	send_to_char( "No descriptor to snoop.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Cancelling all snoops.\n\r", ch );
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->snoop_by == ch->desc )
		d->snoop_by = NULL;
	}
	return;
    }

    if ( victim->desc->snoop_by )
    {
	send_to_char( "Busy already.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( ch->desc )
    {
	for ( d = ch->desc->snoop_by; d; d = d->snoop_by )
	{
	    if ( d->character == victim || d->original == victim )
	    {
		send_to_char( "No snoop loops.\n\r", ch );
		return;
	    }
	}
    }

    victim->desc->snoop_by = ch->desc;
    send_to_char( "Ok.\n\r", ch );
    sprintf( buf, "%s is snooping %s", ch->name, victim->name );
    wiznet( ch, WIZ_SNOOPS, get_trust( rch ), buf );
    return;
}



void do_switch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "switch" ) )
        return;

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Switch into whom?\n\r", ch );
	return;
    }

    if ( !ch->desc )
	return;
    
    if ( ch->desc->original )
    {
	send_to_char( "You are already switched.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    /*
     * Pointed out by Da Pub (What Mud)
     */
    if ( !IS_NPC( victim ) )
    {
        send_to_char( "You cannot switch into a player!\n\r", ch );
	return;
    }

    if ( victim->desc )
    {
	send_to_char( "Character in use.\n\r", ch );
	return;
    }

    ch->pcdata->switched  = TRUE;
    ch->desc->character   = victim;
    ch->desc->original    = ch;
    victim->desc          = ch->desc;
    ch->desc              = NULL;
    send_to_char( "Ok.\n\r", victim );
    sprintf( buf, "%s switched into %s", rch->name, victim->short_descr );
    wiznet( ch, WIZ_SWITCHES, get_trust( rch ), buf );
    return;
}



void do_return( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !ch->desc )
	return;

    if ( !ch->desc->original )
    {
	send_to_char( "You aren't switched.\n\r", ch );
	return;
    }

/* Note that we dont check for immortal ability to have return here.
 * We assume we will automatically allow immortals with switch to return.
 * Dont want to have our immortals stuck in a mobile's body do we?  :)
 * -Kahn */

    send_to_char( "You return to your original body.\n\r", ch );
    ch->desc->original->pcdata->switched = FALSE;
    ch->desc->character                  = ch->desc->original;
    ch->desc->original                   = NULL;
    ch->desc->character->desc            = ch->desc; 
    ch->desc                             = NULL;

    sprintf( buf, "%s returns to his original body", rch->name );
    wiznet( rch, WIZ_SWITCHES, get_trust( rch ), buf );
    return;
}



void do_mload( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA      *rch;
    CHAR_DATA      *victim;
    MOB_INDEX_DATA *pMobIndex;
    char            arg [ MAX_INPUT_LENGTH ];
    char            buf [ MAX_STRING_LENGTH ];
    
    rch = get_char( ch );

    if ( !authorized( rch, "mload" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
	send_to_char( "Syntax: mload <vnum>.\n\r", ch );
	return;
    }

    if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
    {
	send_to_char( "No mob has that vnum.\n\r", ch );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    send_to_char( "Ok.\n\r", ch );
    act( "$n has created $N!", ch, NULL, victim, TO_ROOM );
    sprintf( buf, "%s has mloaded %s at %s [%d]", ch->name, victim->short_descr,
	    ch->in_room->name, ch->in_room->vnum );
    wiznet( ch, WIZ_LOAD, get_trust( rch ), buf );
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
    char            buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "oload" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );
 
    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
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
	    send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
	    return;
        }
        level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
        {
	    send_to_char( "Limited to your trust level.\n\r", ch );
	    return;
        }
    }

    if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
    {
	send_to_char( "No object has that vnum.\n\r", ch );
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
    send_to_char( "Ok.\n\r", ch );
    sprintf( buf, "%s has loaded %s at %s [%d]", ch->name, obj->short_descr,
	    ch->in_room->name, ch->in_room->vnum );
    wiznet( ch, WIZ_LOAD, get_trust( rch ), buf );
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

	send_to_char( "Ok.\n\r", ch );
	act( "$n purges the room!", ch, NULL, NULL, TO_ROOM);
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "Not on PC's.\n\r", ch );
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
    char       buf  [ MAX_STRING_LENGTH ];
    int        level;
    int        iLevel;

    rch = get_char( ch );

    if ( !authorized( rch, "advance" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: advance <char> <level>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    
    level = atoi( arg2 );

    if ( level < 1 || level > MAX_LEVEL )
    {
	char buf [ MAX_STRING_LENGTH ];

	sprintf( buf, "Advance within range 1 to %d.\n\r", MAX_LEVEL );
	send_to_char( buf, ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust level.\n\r", ch );
	return;
    }

    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     */
    if ( level <= victim->level )
    {
	int sn;
	
	send_to_char( "Lowering a player's level!\n\r", ch );
	send_to_char( "**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r",    victim );
	victim->level    = 1;
	victim->exp      = EXP_PER_LEVEL;
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
	sprintf( buf, "%s has been demoted to level %d by %s", victim->name,
		level, ch->name );
	wiznet( victim, WIZ_LEVELS, get_trust( rch ), buf );
    }
    else
    {
	send_to_char( "Raising a player's level!\n\r", ch );
	send_to_char( "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim );
	sprintf( buf, "%s has been advanced to level %d by %s", victim->name,
		level, ch->name );
	wiznet( victim, WIZ_LEVELS, get_trust( rch ), buf );
    }

    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
	send_to_char( "You raise a level!!  ", victim );
	victim->level += 1;
	advance_level( victim );
    }
    victim->exp   = EXP_PER_LEVEL * UMAX( 1, victim->level );
    victim->trust = 0;
    return;
}



void do_trust( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    char       buf  [ MAX_STRING_LENGTH ];
    int        level;

    rch = get_char( ch );

    if ( !authorized( rch, "trust" ) )
        return;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	send_to_char( "That player is not here.\n\r", ch );
	return;
    }

    level = atoi( arg2 );

    if ( level < 1 || level > MAX_LEVEL )
    {
	char buf [ MAX_STRING_LENGTH ];

	sprintf( buf, "Trust within range 1 to %d.\n\r", MAX_LEVEL );
	send_to_char( buf, ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust.\n\r", ch );
	return;
    }

    sprintf( buf, "%s has been trusted at level %d by %s", victim->name,
	    level, ch->name );
    wiznet( ch, WIZ_LEVELS, get_trust( rch ), buf );

    victim->trust = level;
    return;
}



void do_restore( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CHAR_DATA *vch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "restore" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !str_cmp( arg, "room" ) )
    {
        for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted )
	        continue;
	    vch->hit = vch->max_hit;
	    vch->mana = vch->max_mana;
	    vch->move = vch->max_move;
	    update_pos( vch );
	    act( "$n has restored you.", ch, NULL, vch, TO_VICT );
	}
        sprintf( buf, "%s has restored room %d.",
		rch->name, ch->in_room->vnum );
        wiznet( ch, WIZ_RESTORE, get_trust( rch ), buf );
	send_to_char( "Room restored.\n\r", ch );
	return;
    }

    /* Restore All feature coded by Katrina */
    if ( !str_cmp( arg, "all" ) )
    {
        for ( victim = char_list; victim; victim = victim->next )
	{
	    if ( victim->deleted )
	        continue;
	    victim->hit = victim->max_hit;
	    victim->mana = victim->max_mana;
	    victim->move = victim->max_move;
	    update_pos( victim );
	    act( "$n has restored you.", ch, NULL, victim, TO_VICT );
	}
        sprintf( buf, "%s has restored the whole mud.", ch->name );
        wiznet( ch, WIZ_RESTORE, get_trust( rch ), buf );
	send_to_char( "Aww...how sweet :)...Done.\n\r", ch );
    }
    else
    {
	if ( !( victim = get_char_world( ch, arg ) ) )
	  {
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	  }

	victim->hit  = victim->max_hit;
	victim->mana = victim->max_mana;
	victim->move = victim->max_move;
	update_pos( victim );
	act( "$n has restored you.", ch, NULL, victim, TO_VICT );
        sprintf( buf, "%s has restored %s.", ch->name, victim->name );
        wiznet( ch, WIZ_RESTORE, get_trust( rch ), buf );
	send_to_char( "Ok.\n\r", ch );
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
	send_to_char( "Freeze whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_FREEZE ) )
    {
	REMOVE_BIT( victim->act, PLR_FREEZE );
	send_to_char( "FREEZE removed.\n\r",     ch     );
	send_to_char( "You can play again.\n\r", victim );
    }
    else
    {
	SET_BIT(    victim->act, PLR_FREEZE );
	send_to_char( "FREEZE set.\n\r",            ch     );
	send_to_char( "You can't do ANYthing!\n\r", victim );
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
	send_to_char( "Log whom?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	if ( fLogAll )
	{
	    fLogAll = FALSE;
	    send_to_char( "Log ALL off.\n\r", ch );
	}
	else
	{
	    fLogAll = TRUE;
	    send_to_char( "Log ALL on.\n\r",  ch );
	}
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    /*
     * No level check, gods can log anyone.
     */
    if ( IS_SET( victim->act, PLR_LOG ) )
    {
	REMOVE_BIT( victim->act, PLR_LOG );
	send_to_char( "LOG removed.\n\r", ch );
    }
    else
    {
	SET_BIT(    victim->act, PLR_LOG );
	send_to_char( "LOG set.\n\r",     ch );
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
	send_to_char( "Noemote whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_NO_EMOTE ) )
    {
	REMOVE_BIT( victim->act, PLR_NO_EMOTE );
	send_to_char( "NO_EMOTE removed.\n\r",    ch     );
	send_to_char( "You can emote again.\n\r", victim );
    }
    else
    {
	SET_BIT(    victim->act, PLR_NO_EMOTE );
	send_to_char( "You can't emote!\n\r",    victim );
	send_to_char( "NO_EMOTE set.\n\r",       ch     );
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
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_NO_TELL ) )
    {
	REMOVE_BIT( victim->act, PLR_NO_TELL );
	send_to_char( "NO_TELL removed.\n\r",    ch );
	send_to_char( "You can tell again.\n\r", victim );
    }
    else
    {
	SET_BIT(    victim->act, PLR_NO_TELL );
	send_to_char( "NO_TELL set.\n\r",        ch     );
	send_to_char( "You can't tell!\n\r",     victim );
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
	send_to_char( "Silence whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_SILENCE ) )
    {
	REMOVE_BIT( victim->act, PLR_SILENCE );
	send_to_char( "You can use channels again.\n\r", victim );
	send_to_char( "SILENCE removed.\n\r",            ch     );
    }
    else
    {
	SET_BIT(    victim->act, PLR_SILENCE );
	send_to_char( "You can't use channels!\n\r",     victim );
	send_to_char( "SILENCE set.\n\r",                ch     );
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
	if ( rch->deleted )
	    continue;

	if ( rch->fighting )
	    stop_fighting( rch, TRUE );

	stop_hating( rch );
	stop_hunting( rch );
	stop_fearing( rch );
    }

    send_to_char( "Ok.\n\r", ch );
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
	strcpy( buf, "Banned sites:\n\r" );
	for ( pban = ban_list; pban; pban = pban->next )
	{
	    strcat( buf, pban->name );
	    strcat( buf, "\n\r" );
	}
	send_to_char( buf, ch );
	return;
    }

    for ( pban = ban_list; pban; pban = pban->next )
    {
	if ( !str_cmp( arg, pban->name ) )
	{
	    send_to_char( "That site is already banned!\n\r", ch );
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
    send_to_char( "Ok.\n\r", ch );
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
	send_to_char( "Remove which site from the ban list?\n\r", ch );
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
	    send_to_char( "Ok.\n\r", ch );
	    ban_update( );
	    return;
	}
    }

    send_to_char( "Site is not banned.\n\r", ch );
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
	send_to_char( "Game wizlocked.\n\r", ch );
    else
	send_to_char( "Game un-wizlocked.\n\r", ch );

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
	send_to_char( "Slookup what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        buf1[0] = '\0';
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( !skill_table[sn].name )
		break;
	    sprintf( buf, "Sn: %4d Skill/spell: '%s'\n\r",
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
		sprintf( buf, "Sn: %4d Skill/spell: '%s'\n\r",
			sn, skill_table[sn].name );
		send_to_char( buf, ch );
		return;
	    }
	}

        if ( ( sn = skill_lookup( arg ) ) < 0 )
	{
	    send_to_char( "No such skill or spell.\n\r", ch );
	    return;
	}

	sprintf( buf, "Sn: %4d Skill/spell: '%s'\n\r",
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
	send_to_char( "Syntax: sset <victim> <skill> <value>\n\r",	ch );
	send_to_char( "or:     sset <victim> all     <value>\n\r",	ch );
	send_to_char( "Skill being any skill or spell.\n\r",		ch );
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
	send_to_char( "You may not sset your peer nor your superior.\n\r",
		     ch );
	return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such skill or spell.\n\r", ch );
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
        if ( get_trust( ch ) < L_DIR )
	{
	    send_to_char( "Only Directors may sset all.\n\r", ch );
	    return;
	}
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name )
		if ( skill_table[sn].skill_level[victim->class] <= LEVEL_HERO
		    || IS_IMMORTAL( victim ) )
		    victim->pcdata->learned[sn] = value;
	    else
		if ( skill_table[sn].skill_level[victim->class]
		    <= get_trust( victim ) )
		    victim->pcdata->learned[sn] = 1;

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
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    char       arg3 [ MAX_INPUT_LENGTH  ];
    int        value;
    int        max;

    rch = get_char( ch );

    if ( !authorized( rch, "mset" ) )
        return;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax: mset <victim> <field>  <value>\n\r",	ch );
	send_to_char( "or:     mset <victim> <string> <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  str int wis dex con class sex race level\n\r",	ch );
	send_to_char( "  gold hp mana move practice align\n\r",		ch );
	send_to_char( "  thirst drunk full security",			ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "String being one of:\n\r",			ch );
	send_to_char( "  name short long title spec clan rank\n\r",	ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
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
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[ch->class]->attr_prime == APPLY_STR )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Strength range is 3 to %d.\n\r", max );
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
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[ch->class]->attr_prime == APPLY_INT )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Intelligence range is 3 to %d.\n\r", max );
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
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[ch->class]->attr_prime == APPLY_WIS )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Wisdom range is 3 to %d.\n\r", max );
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
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[ch->class]->attr_prime == APPLY_DEX )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Dexterity range is 3 to %d.\n\r", max );
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
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[ch->class]->attr_prime == APPLY_CON )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Constitution range is 3 to %d.\n\r", max );
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
	    send_to_char( "This person is affect by change sex.\n\r", ch );
	    send_to_char( "Try again later.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 2 )
	{
	    send_to_char( "Sex range is 0 to 2.\n\r", ch );
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
	    send_to_char( "This person is affected by polymorph other.\n\r",
			 ch );
	    send_to_char( "Try again later.\n\r", ch );
	    return;
	}

	race = race_lookup( arg3 );

	if ( race < 0 )
	{
	    send_to_char( "Invalid race.\n\r", ch );
	    return;
	}

	if (  !IS_SET( race_table[ race ].race_abilities, RACE_PC_AVAIL )
	    && get_trust( ch ) < L_DIR )
	{
	    send_to_char( "You may not set a race not available to PC's.\n\r",
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
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 50 )
	{
	    send_to_char( "Level range is 0 to 50.\n\r", ch );
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

    if ( !str_cmp( arg2, "hp" ) )
    {
	if ( value < -10 || value > 30000 )
	{
	    send_to_char( "Hp range is -10 to 30,000 hit points.\n\r", ch );
	    return;
	}
	if ( victim->fighting && value < 0 )
	{
	    send_to_char( "You cannot set a fighting person's hp below 0.\n\r",
			 ch );
	    return;
	}
	victim->max_hit = value;
	return;
    }

    if ( !str_cmp( arg2, "mana" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Mana range is 0 to 30,000 mana points.\n\r", ch );
	    return;
	}
	victim->max_mana = value;
	return;
    }

    if ( !str_cmp( arg2, "move" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Move range is 0 to 30,000 move points.\n\r", ch );
	    return;
	}
	victim->max_move = value;
	return;
    }

    if ( !str_cmp( arg2, "practice" ) )
    {
	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Practice range is 0 to 100 sessions.\n\r", ch );
	    return;
	}
	victim->practice = value;
	return;
    }

    if ( !str_cmp( arg2, "align" ) )
    {
	if ( value < -1000 || value > 1000 )
	{
	    send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
	    return;
	}
	victim->alignment = value;
	return;
    }

    if ( !str_cmp( arg2, "thirst" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( ( value < 0 || value > 100 )
	    && get_trust( victim ) < LEVEL_IMMORTAL )
	{
	    send_to_char( "Thirst range is 0 to 100.\n\r", ch );
	    return;
	}
	else
	    if ( value < -1 || value > 100 )
	    {
		send_to_char( "Thirst range is -1 to 100.\n\r", ch );
		return;
	    }

	victim->pcdata->condition[COND_THIRST] = value;
	return;
    }

    if ( !str_cmp( arg2, "drunk" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char( "Drunk range is 0 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_DRUNK] = value;
	return;
    }

    if ( !str_cmp( arg2, "full" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( ( value < 0 || value > 100 )
	    && get_trust( victim ) < LEVEL_IMMORTAL )
	{
	    send_to_char( "Full range is 0 to 100.\n\r", ch );
	    return;
	}
	else
	    if ( value < -1 || value > 100 )
	    {
		send_to_char( "Full range is -1 to 100.\n\r", ch );
		return;
	    }

	victim->pcdata->condition[COND_FULL] = value;
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
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
	strcat( arg3, "\n\r" );
	victim->long_descr = str_dup( arg3 );
	return;
    }

    if ( !str_cmp( arg2, "clan" ) )
    {
	CLAN_DATA *clan;

	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( !( clan = clan_lookup( arg3 ) ) )
	{
	    send_to_char( "That clan doesn't exist.\n\r", ch );
	    return;
	}

	victim->pcdata->clan = clan;
	victim->pcdata->clan->name = str_dup( clan->name );
	return;
    }

    if ( !str_cmp( arg2, "title" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	set_title( victim, arg3 );
	return;
    }

    if ( !str_cmp( arg2, "spec" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( ( victim->spec_fun = spec_mob_lookup( arg3 ) ) == 0 )
	{
	    send_to_char( "No such spec fun.\n\r", ch );
	    return;
	}

	return;
    }

    if ( !str_cmp( arg2, "rank" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( ( value = flag_value( rank_flags, arg3 ) ) == NO_FLAG )
	{
	    send_to_char( "No such rank.\n\r", ch );
	    return;
	}

	victim->pcdata->rank = value;
	return;
    }

    if ( !str_cmp( arg2, "security" ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( ( value > ch->pcdata->security && get_trust( ch ) < L_DIR )
	    || value < 0 )
        {
	    if ( ch->pcdata->security > 0 )
	    {
		sprintf( buf, "Valid security is 0-%d.\n\r",
		    ch->pcdata->security );
		send_to_char( buf, ch );
	    }
	    else
	    {
		send_to_char( "Valid security is 0 only.\n\r", ch );
	    }
	    return;
        }
        victim->pcdata->security = value;
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
	send_to_char( "Syntax: oset <object> <field>  <value>\n\r",	ch );
	send_to_char( "or:     oset <object> <string> <value>\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  value0 value1 value2 value3 value4\n\r",		ch );
	send_to_char( "  extra wear level weight cost timer\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "String being one of:\n\r",			ch );
	send_to_char( "  name short long ed\n\r",			ch );
	return;
    }

    if ( !( obj = get_obj_world( ch, arg1 ) ) )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
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

    if ( !str_cmp( arg2, "value4" ) || !str_cmp( arg2, "v4" ) )
    {
	obj->value[4] = value;
	return;
    }

    if ( !str_cmp( arg2, "extra" ) )
    {
	if ( !is_number( arg3 ) )
	{
	    if ( ( value = flag_value( extra_flags, arg3 ) ) != NO_FLAG )
	    {
		TOGGLE_BIT( obj->extra_flags, value );
		send_to_char( "Extra flag toggled.\n\r", ch );
	    }
	
	    return;
	}

	obj->extra_flags = value;
	send_to_char( "Extra flags set.\n\r", ch );
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
		"You may not modify an item's weight while on a PC.\n\r",
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
	    send_to_char( "Syntax: oset <object> ed <keyword> <string>\n\r",
		ch );
	    return;
	}

	ed			= new_extra_descr();

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
	send_to_char( "Syntax: rset <location> <field> value\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  flags sector\n\r",				ch );
	return;
    }

    if ( !( location = find_location( ch, arg1 ) ) )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    for ( person = location->people; person; person = person->next_in_room )
	if ( !IS_NPC( person ) && person != ch && person->level >= ch->level )
	  {
	    send_to_char(
		"Your superior is in this room, no rsetting now.\n\r", ch );
	    return;
	  }

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "flags" ) )
    {
	if ( !is_number( arg3 ) )
	{
	    if ( ( value = flag_value( room_flags, arg3 ) ) != NO_FLAG )
	    {
		TOGGLE_BIT( location->room_flags, value );
		send_to_char( "Room flag toggled.\n\r", ch );
	    }

	    return;
	}

	location->room_flags = value;
	send_to_char( "Room flags set.\n\r", ch );
	location->room_flags	= value;
	return;
    }

    if ( !str_cmp( arg2, "sector" ) )
    {
	if ( !is_number( arg3 )
	    && ( value = flag_value( sector_flags, arg3 ) ) == NO_FLAG )
		return;

	location->sector_type = value;
	send_to_char( "Sector type set.\n\r", ch );

	return;
    }

    /*
     * Generate usage message.
     */
    do_rset( ch, "" );
    return;
}



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
	    sprintf( buf + strlen( buf ), "[%3d %2d] %s@%s\n\r",
		    d->descriptor,
		    d->connected,
		    d->original  ? d->original->name  :
		    d->character ? d->character->name : "(none)",
		    d->host );
	}
    }

    sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
    strcat( buf, buf2 );
    send_to_char( buf, ch );
    return;
}



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
	send_to_char( "Force whom to do what?\n\r", ch );
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
	  send_to_char( "You can't even do that yourself!\n\r", ch );
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
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}

	act( "$n forces you to '$t'.", ch, argument, victim, TO_VICT );
	interpret( victim, argument );
    }

    send_to_char( "Ok.\n\r", ch );
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
	send_to_char( "You slowly fade back into existence.\n\r", ch    );
	act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
	SET_BIT(    ch->act, PLR_WIZINVIS );
	send_to_char( "You slowly vanish into thin air.\n\r",     ch    );
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
	send_to_char( "Holy light mode off.\n\r", ch );
    }
    else
    {
	SET_BIT(    ch->act, PLR_HOLYLIGHT );
	send_to_char( "Holy light mode on.\n\r", ch );
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
	send_to_char( "Syntax: wizify <name>\n\r" , ch );
	return;
    }
    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r" , ch );
	return;
    }
    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on mobs.\n\r", ch );
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

/* Idea from Talen of Vego's do_where command */

void do_owhere( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    OBJ_DATA  *in_obj;
    CHAR_DATA *rch;
    char       buf  [ MAX_STRING_LENGTH   ];
    char       buf1 [ MAX_STRING_LENGTH*6 ];
    char       arg  [ MAX_INPUT_LENGTH    ];
    int        obj_counter = 1;
    bool       found = FALSE;

    rch = get_char( ch );

    if ( !authorized( rch, "owhere" ) )
        return;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  owhere <object>.\n\r", ch );
	return;
    }
    else
    {
        buf1[0] = '\0';
	for ( obj = object_list; obj; obj = obj->next )
	{
	    if ( !can_see_obj( ch, obj ) || !is_name( arg, obj->name ) )
	        continue;

	    found = TRUE;

	    for ( in_obj = obj; in_obj->in_obj;
		 in_obj = in_obj->in_obj )
	        ;

	    if ( in_obj->carried_by )
	    {
	        if ( !can_see( ch, in_obj->carried_by ) )
		    continue;
		sprintf( buf, "[%2d] %s carried by %s at [%4d].\n\r",
			obj_counter, obj->short_descr,
			PERS( in_obj->carried_by, ch ),
			in_obj->carried_by->in_room->vnum );
	    }
	    else
	    {
		sprintf( buf, "[%2d] %s in %s at [%4d].\n\r", obj_counter,
			obj->short_descr, ( !in_obj->in_room ) ?
			"somewhere" : in_obj->in_room->name,
			( !in_obj->in_room ) ?
			0 : in_obj->in_room->vnum );
	    }
	    
	    obj_counter++;
	    buf[0] = UPPER( buf[0] );
	    strcat( buf1, buf );

	    /* Only see the first 101 */
	    if ( obj_counter > 100 )
	        break;
	}

	send_to_char( buf1, ch );
    }

    if ( !found )
	send_to_char(
		"Nothing like that in hell, earth, or heaven.\n\r" , ch );

    return;


}

void do_numlock( CHAR_DATA *ch, char *argument )  /*By Globi*/
{
           CHAR_DATA *rch;
	   char       buf  [ MAX_STRING_LENGTH ];
	   char       arg1 [ MAX_INPUT_LENGTH  ];
    extern int        numlock;
           int        temp;

    rch = get_char( ch );

    if ( !authorized( rch, "numlock" ) )
        return;

    one_argument( argument, arg1 );

    temp = atoi( arg1 );

    if ( arg1[0] == '\0' ) /* Prints out the current value */
    {
	sprintf( buf, "Current numlock setting is:  %d.\n\r", numlock );
	send_to_char( buf, ch );
	return;
    }

    if ( ( temp < 0 ) || ( temp > LEVEL_HERO ) )
    {
	sprintf( buf, "Level must be between 0 and %d.\n\r", LEVEL_HERO );
	send_to_char( buf, ch );
	return;
    }

    numlock = temp;  /* Only set numlock if arg supplied and within range */

    if ( numlock != 0 )
    {
	sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
	send_to_char( buf, ch );
    }
    else
        send_to_char( "Game now open to all levels.\n\r", ch );

    return;

}

void do_newlock( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA *rch;
    extern int        numlock;
           char       buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "newlock" ) )
        return;

    if ( numlock != 0 && get_trust( ch ) < L_SEN )
    {
	send_to_char( "You may not change the current numlock setting\n\r",
		     ch );
	sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
	send_to_char( buf, ch );
	return;
    }

    if ( numlock != 0 )
    {
	sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
	send_to_char( buf, ch );
	send_to_char( "Changing to: ", ch );
    }

    numlock = 1;
    send_to_char( "Game locked to new characters.\n\r", ch );
    return;

}

void do_sstime( CHAR_DATA *ch, char *argument )
{
           CHAR_DATA *rch;
    extern time_t     down_time;
    extern time_t     warning1;
    extern time_t     warning2;
    extern bool       Reboot;
           char       buf  [ MAX_STRING_LENGTH ];
           char       arg1 [ MAX_INPUT_LENGTH  ];
	   int        number;

    rch = get_char( ch );

    if ( !authorized( rch, "sstime" ) )
        return;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );

    if ( !str_cmp( arg1, "reboot" ) )
    {
        Reboot = !Reboot;
	sprintf( buf, "Reboot is %s.\n\r", Reboot ? "on" : "off" );
	send_to_char( buf, ch );
	return;
    }

    number   = atoi( arg1 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) || number < 0 )
    {
	send_to_char( "Syntax: sstime <value>/reboot\n\r",                ch );
	send_to_char( "\n\r",                                             ch );
	send_to_char( "Value is number of minutes till reboot/shutdown.", ch );
	send_to_char( "  Or 0 to turn off.\n\r",                          ch );
	send_to_char( "Reboot will toggle reboot on or off.\n\r",         ch );
	send_to_char( "\n\r",                                             ch );
	if ( down_time > 0 )
	{
	    sprintf( buf, "1st warning:  %d minutes (%d seconds).\n\r",
		    UMAX( ( (int) warning1 - (int) current_time ) / 60, 0 ),
		    UMAX( ( (int) warning1 - (int) current_time ), 0 ) );
	    send_to_char( buf,                                            ch );
	    sprintf( buf, "2nd warning:  %d minutes (%d seconds).\n\r",
		    UMAX( ( (int) warning2 - (int) current_time ) / 60, 0 ),
		    UMAX( ( (int) warning2 - (int) current_time ), 0 ) );
	    send_to_char( buf,                                            ch );
	    sprintf( buf, "%s%d minutes (%d seconds).\n\r",
		    Reboot ? "Reboot:       " : "Shutdown:     ",
		    UMAX( ( (int) down_time - (int) current_time ) / 60, 0 ),
		    UMAX( ( (int) down_time - (int) current_time ), 0 ) );
	    send_to_char( buf,                                            ch );
	}
	else
	    send_to_char( "Automatic reboot/shutdown:  off.\n\r",         ch );
	return;
    }

    /* Set something */

    if ( number > 0 )
    {
        down_time = current_time + ( number * 60 );
        if ( number < 6 )
	{
	    warning2  = down_time - 150;
	    warning1  = warning2  - 75;
	}
	else
	{
	    warning2  = down_time - 150;
	    warning1  = warning2  - 150;
	}
	sprintf( buf, "%s will be in %d minutes (%d seconds).\n\r",
		Reboot ? "Reboot" : "Shutdown",
		( (int) down_time - (int) current_time ) / 60,
		( (int) down_time - (int) current_time ) );
	send_to_char( buf, ch );
	sprintf( buf, "1st Warning will be in %d minutes (%d seconds).\n\r",
		( (int) warning1 - (int) current_time ) / 60,
		( (int) warning1 - (int) current_time ) );
	send_to_char( buf, ch );
	sprintf( buf, "2nd Warning will be in %d minutes (%d seconds).\n\r",
		( (int) warning2 - (int) current_time ) / 60,
		( (int) warning2 - (int) current_time ) );
	send_to_char( buf, ch );
    }
    else
    {
	down_time = 0;
	sprintf( buf, "Auto%s is now off.\n\r",
		Reboot ? "reboot" : "shutdown" );
	send_to_char( buf, ch );
    }

    return;

}

/*
 * Modifications contributed by
 * Canth <phule@xs4all.nl>
 * Maniac <v942346@si.hhs.nl>
 * Vego <v942429@si.hhs.nl>
 */
void do_imtlset( CHAR_DATA *ch, char *argument )
{

    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       buf  [ MAX_STRING_LENGTH ];
    char       buf1 [ MAX_STRING_LENGTH ];
    bool       fAll = FALSE;
    int        cmd;
    int        col  = 0;

    rch = get_char( ch );
    
    if ( !authorized( rch, "imtlset" ) )
        return;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: imtlset <victim> +|- <immortal skill>\n\r",ch );
	send_to_char( "or:     imtlset <victim> +|- all\n\r",             ch );
	send_to_char( "or:     imtlset <victim>\n\r",                     ch );
	return;
    }

    if ( !( victim = get_char_world( rch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( rch->level <= victim->level && rch != victim )
    {
	send_to_char( "You may not imtlset your peer nor your superior.\n\r",
		     ch );
	return;
    }

    if ( argument[0] == '+' || argument[0] == '-' )
    {
	buf[0] = '\0';
	smash_tilde( argument );
	
	argument = one_argument( argument, arg1 );

	if ( !str_cmp( argument, "all" ) )
	    fAll = TRUE;

	if ( arg1[0] == '+' )
	{
	    if ( !fAll )
	    {
		if ( victim->pcdata->immskll )
		    strcpy( buf, victim->pcdata->immskll );

		if ( is_name( argument, victim->pcdata->immskll ) )
		{
		    send_to_char( "That skill has already been set.\n\r", ch );
		    return;
		}

	    }

	    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
	    {
		if ( cmd_table[cmd].level > get_trust( rch ) )
		    continue;
		if ( fAll )
		{
		    if (   cmd_table[cmd].level <= get_trust( victim )
			&& cmd_table[cmd].level >= LEVEL_HERO
			&& str_infix( cmd_table[cmd].name,
				     "delet reboo sla shutdow :" ) )
		    {
			strcat( buf, cmd_table[cmd].name );
			strcat( buf, " " );
		    }
		} 
		else /* Add only one skill */
		{
		    if ( !str_cmp( argument, cmd_table[cmd].name ) )
		        break;
		}
	    }

	    if ( !fAll )
	    {
		if (   cmd_table[cmd].name[0] == '\0'
		    || is_name( argument, "delet reboo sla shutdow :" ) )
		{
		    send_to_char( "That is not an immskill.\n\r", ch );
		    return;
		}

		strcat( buf, argument );
		strcat( buf, " " ); /* This line is really not needed but makes
				       pfile look nice - Kahn */
	    }
	}
	else /* arg1[0] == '-' */
	{
	    if ( fAll )
	    {
		free_string( victim->pcdata->immskll );
		victim->pcdata->immskll = str_dup( "" );
		send_to_char( "All Immskills have been deleted.\n\r", ch );
		return;
	    }
	    else /* Remove one skill */
	    {
	        char  buf2[ MAX_STRING_LENGTH ];
		char  arg3[ MAX_INPUT_LENGTH ];
		char  arg2[ MAX_INPUT_LENGTH ];

		argument = one_argument( argument, arg2 );

		strcpy( buf2, victim->pcdata->immskll );

		if ( !is_name( arg2, victim->pcdata->immskll ) )
		{
		    send_to_char( "That Immskill is not set.\n\r", ch );
		    return;
		}

		for ( ; ; )
		{
		    strcpy( buf2, one_argument( buf2, arg3 ) );
		    if ( arg3[0] == '\0' )
			break;
		    if ( str_cmp( arg3, arg2 ) )
		    {
			strcat( buf, arg3 );
			strcat( buf, " " );
		    }
		}
	    }
	}

	free_string( victim->pcdata->immskll );
	victim->pcdata->immskll = str_dup( buf );

    }

    sprintf( buf, "Immortal skills set for %s:\n\r", victim->name );
    send_to_char( buf, ch );
    buf1[0] = '\0';
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level < LEVEL_HERO
	    || !is_name( cmd_table[cmd].name, victim->pcdata->immskll ) )
	    continue;

	sprintf( buf, "%-10s", cmd_table[cmd].name );
	strcat( buf1, buf );
	if ( ++col % 8 == 0 )
	    strcat( buf1, "\n\r" );
    }
 
    if ( col % 8 != 0 )
	strcat( buf1, "\n\r" );
    send_to_char( buf1, ch );

    return;

}

void do_delet( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "delete" ) )
        return;

    send_to_char( "If you want to DELETE, spell it out.\n\r", ch );
    return;
}

void do_delete( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "delete" ) )
        return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Delete whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( rch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !victim->desc )
    {
	act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( rch == victim )
    {
	send_to_char( "You may not delete yourself, use RETIRE.\n\r",
		     ch );
	return;
    }

    if ( rch->level <= victim->level && rch != victim )
    {
	send_to_char( "You may not delete your peer nor your superior.\n\r",
		     ch );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d == victim->desc && victim->level >= 2 )
	{
            /* By saving first i assure i am not removing a non existing file
             * i know it's stupid and probably useless but... Zen.
             */
            save_char_obj( victim );
    	    backup_char_obj( victim ); /* handy function huh? :) */
    	    delete_char_obj( victim ); /* handy function huh? :) */
	    extract_char( victim, TRUE );
	    close_socket( d );
	    send_to_char( "Ok.\n\r", rch );
	    return;
	}
    }

    bug( "Do_delete: desc not found.", 0 );
    send_to_char( "Descriptor not found!\n\r", ch );
    return;
}



/* 
 * Simple function to let any imm make any player instantly sober.
 * Saw no need for level restrictions on this.
 * Written by Narn, Apr/96 
 */
void do_sober( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *rch;
    CHAR_DATA       *victim;
    char             arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "sober" ) )
	return;

    smash_tilde( argument );
    argument = one_argument( argument, arg );

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "Not on mobs.\n\r", ch );
	return;    
    }

    if ( victim->pcdata ) 
	victim->pcdata->condition[COND_DRUNK] = 0;

    send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel sober again.\n\r", victim );

    return;    
}



void do_clookup( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA         *rch;
    char               buf  [ MAX_STRING_LENGTH * 2 ];
    char               buf1 [ MAX_STRING_LENGTH * 6 ];
    int                num;
    int                level;
    int                sn;
    int                col;
    int                nNumber;
    int                iLevelLower;
    int                iLevelUpper;
    bool               pSpell;
    struct class_type *class;

    rch = get_char( ch );
    num = rch->class;

    if ( !authorized( rch, "clookup" ) )
        return;

    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    class          = class_table[rch->class];

    /*
     * Parse arguments.
     */
    nNumber = 0;
    for ( ;; )
    {
	char arg [ MAX_STRING_LENGTH ];

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	    break;

	if ( is_number( arg ) )
	{
	    switch ( ++nNumber )
	    {
	    case 1: iLevelLower = atoi( arg ); break;
	    case 2: iLevelUpper = atoi( arg ); break;
	    default:
		send_to_char( "Only two level numbers allowed.\n\r", ch );
		return;
	    }
	}
	else
	{
	    int iClass;

	    if ( strlen( arg ) < 3 )
	    {
		send_to_char( "Classes must be longer than that.\n\r", ch );
		return;
	    }

	    /*
	     * Look for classes to turn on.
	     */
	    arg[3]    = '\0';

	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	    {
		if ( !str_cmp( arg, class_table[iClass]->who_name ) )
		{
		    num = iClass;
		    class = class_table[iClass];
		    break;
		}
	    }
	}
    }

    if ( !class )
    {
        send_to_char( "No such class.\n\r", ch );
        return;
    }

    buf1[0] = '\0';

    if ( iLevelLower < 0 || iLevelUpper < 0 )
	return;

    sprintf( buf, "{rClass: %s (%s){x\n\r",
	    class->name, class->who_name );
    strcat( buf1, buf );

    sprintf( buf, "Prime Attribute: %-14s  Weapon: %-5d\n\r",
	    affect_loc_name( class->attr_prime ), class->weapon );
    strcat( buf1, buf );

    sprintf( buf, "Max Skill Adept: %-3d  Thac0: %-5d  Thac47: %d\n\r",
    	    class->skill_adept, class->thac0_00, class->thac0_47 );
    strcat( buf1, buf );

    sprintf( buf, "Hp Min/Hp Max: %d/%-2d  Mana: %s\n\r\n\r",
    	    class->hp_min, class->hp_max, class->fMana ? "yes" : "no" );
    strcat( buf1, buf );

    strcat ( buf1, "{mALL Titles available for this class.{x\n\r\n\r" );

    for ( level = iLevelLower; level <= iLevelUpper; level++ )
    {
	sprintf( buf, "{r%2d: {mMale:{b %-30s {mFemale:{b %-30s{x\n\r",
		level, title_table[num][level][0], title_table[num][level][1] );
	strcat( buf1, buf );
    }

    strcat ( buf1, "\n\r{mALL Abilities available for this class.{x\n\r\n\r" );
    strcat ( buf1, "{rLv          Abilities{x\n\r\n\r" );

    if ( iLevelUpper >= LEVEL_IMMORTAL )
	iLevelUpper = LEVEL_HERO;

    for ( level = iLevelLower; level <= iLevelUpper; level++ )
    {
      col = 0;
      pSpell = TRUE;

      for ( sn = 0; sn < MAX_SKILL; sn++ )
      {
        if ( !skill_table[sn].name )
          break;
        if ( skill_table[sn].skill_level[num] != level )
          continue;

        if ( pSpell )
        {
          sprintf ( buf, "{r%2d:{x", level );
          strcat ( buf1, buf );
          pSpell = FALSE;
        }

        /* format fix by Koala */ 
        if ( ++col % 4 == 0 )
          strcat ( buf1, "   " );

        sprintf ( buf, "{b%18s{x", skill_table[sn].name );
        strcat ( buf1, buf );

        if ( col % 4 == 0 )
          strcat ( buf1, "\n\r" );

      }

      if ( col % 4 != 0 )
        strcat ( buf1, "\n\r" );

    }

    send_to_char( buf1, rch );
    return;
}



void do_setclan( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CLAN_DATA *clan;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "setclan" ) )
        return;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: setclan <clan> <sedit>                     \n\r", ch );
        send_to_char( "or:     setclan <clan> <field>            <value>  \n\r", ch );
        send_to_char( "or:     setclan <clan> <string>           <value>  \n\r", ch );
        send_to_char( "or:     setclan <clan> overlord|chieftain <player> \n\r", ch );
        send_to_char( "\n\rField being one of:                            \n\r", ch );
        send_to_char( " mkills mdeaths pkills pdeaths illegalpk type      \n\r", ch );
        send_to_char( " members clanheros subchiefs                       \n\r", ch );
        send_to_char( " recall donation class score                       \n\r", ch );
        send_to_char( " obj1 obj2 obj3                                    \n\r", ch );
        send_to_char( "String being one of:                               \n\r", ch );
        send_to_char( " name whoname filename motto                       \n\r", ch );
        send_to_char( "Sedit being one of:                                \n\r", ch );
        send_to_char( " desc                                              \n\r", ch );
        return;
    }

    if ( !( clan = clan_lookup( arg1 ) ) )
    {
        send_to_char( "No such clan.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg2, "overlord" ) )
    {
        free_string( clan->overlord );
        clan->overlord = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "chieftain" ) )
    {
        free_string( clan->chieftain );
        clan->chieftain = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "members" ) )
    {
        clan->members = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "clanheros" ) )
    {
        clan->clanheros = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "subchiefs" ) )
    {
        clan->subchiefs = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "mkills" ) )
    {
        clan->mkills = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "mdeaths" ) )
    {
        clan->mdeaths = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "pkills" ) )
    {
        clan->pkills = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "pdeaths" ) )
    {
        clan->pdeaths = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "illegalpk" ) )
    {
        clan->illegal_pk = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "recall" ) )
    {
        clan->recall = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "donation" ) )
    {
        clan->donation = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "obj1" ) )
    {
        clan->clanobj1 = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "obj2" ) )
    {
        clan->clanobj2 = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "obj3" ) )
    {
        clan->clanobj3 = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "type" ) )
    {
	int value;

	if ( ( value = flag_value( clan_flags, argument ) ) != NO_FLAG )
	{
	    clan->clan_type = value;
	    send_to_char( "Done.\n\r", ch );
	    save_clan( clan );
	    return;
	}
	else
	    send_to_char( "Unknown clan type.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg2, "class" ) )
    {
        clan->class = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "score" ) )
    {
        clan->score = atoi( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "name" ) )
    {
        free_string( clan->name );
        clan->name = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "whoname" ) )
    {
        free_string( clan->who_name );
        clan->who_name = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "filename" ) )
    {
        free_string( clan->filename );
        clan->filename = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        save_clan_list( );
        return;
    }
    if ( !str_cmp( arg2, "motto" ) )
    {
        free_string( clan->motto );
        clan->motto = str_dup( argument );
        send_to_char( "Done.\n\r", ch );
        save_clan( clan );
        return;
    }
    if ( !str_cmp( arg2, "desc" ) )
    {
	string_append( ch, &clan->description );
        save_clan( clan );
        return;
    }

    return;
}



void do_showclan( CHAR_DATA *ch, char *argument )
{   
    CHAR_DATA *rch;
    CLAN_DATA *clan;

    rch = get_char( ch );

    if ( !authorized( rch, "showclan" ) )
        return;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: showclan <clan>\n\r", ch );
        return;
    }

    clan = clan_lookup( argument );
    if ( !clan )
    {
        send_to_char( "No such clan.\n\r", ch );
        return;
    }

    printf_to_char( ch, "{o{cWhoName : {x%s{o{c.{x\n\r",
                        clan->who_name );
    printf_to_char( ch, "{o{cName    : {x%s{o{c.{x\n\r",
                        clan->name );
    printf_to_char( ch, "{o{cType    : [{x%5.5d{o{c]  {x%s{o{c.{x\n\r",
                        clan->clan_type,
                        flag_string( clan_flags, clan->clan_type ) );
    printf_to_char( ch, "{o{cFilename: {x%s{x\n\r{o{cMotto   : \"{x%s{o{c\".{x\n\r",
                        clan->filename,
                        clan->motto );
    printf_to_char( ch, "{o{cDescription: {x\n\r%s{x\n\r{o{cOVERLORD : {x%s{o{c.{x\n\r",
                        clan->description,
                        clan->overlord );
    printf_to_char( ch, "{o{cCHIEFTAIN: {x%s{o{c.\n\r",
			clan->chieftain );
    printf_to_char( ch, "{o{cSubchiefs: {x%5.5d{x\n\r{o{cClanheros: {x%5.5d{x\n\r",
			clan->subchiefs,
                        clan->clanheros );
    printf_to_char( ch, "{o{cMembers  : {x%5.5d{o{c  Class   : {x%5.5d{x\n\r",
                        clan->members,
                        clan->class );
    printf_to_char( ch, "{o{cMKills   : {x%5.5d{o{c  MDeaths : {x%5.5d{x\n\r",
                        clan->mkills,
                        clan->mdeaths );
    printf_to_char( ch, "{o{cPKills   : {x%5.5d{o{c  PDeaths : {x%5.5d{x\n\r",
                        clan->pkills,
                        clan->pdeaths );
    printf_to_char( ch, "{o{cIllegalPK: {x%5.5d{o{c  SCORE   : {x%5.5d{x\n\r",
                        clan->illegal_pk,
                        clan->score );
    printf_to_char( ch, "{o{cObj1     : {x%5.5d{o{c  Obj2    : {x%5.5d{o{c  Obj3: {x%5.5d{x\n\r",
                        clan->clanobj1,
                        clan->clanobj2,
                        clan->clanobj3 );
    printf_to_char( ch, "{o{cRecall   : {x%5.5d{o{c  Donation: {x%5.5d{x\n\r",
                        clan->recall, clan->donation );
    return;
}

/*
 * New mudconfig immskill by Zen.
 */
void do_mudconfig( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );
    
    if ( !authorized( rch, "mudconfig" ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "[  Keyword   ] Option\n\r", ch );

	send_to_char(  IS_SET( sysdata.act, MUD_AUTOSAVE_DB  )
            ? "[+AUTOSAVEDB ] The mud autosaves the world.\n\r"
	    : "[-autosavedb ] The mud doesn't autosave the world.\n\r"
	    , ch );

    }
    else
    {
	char buf [ MAX_STRING_LENGTH ];
	int  bit;
	bool fSet;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    if ( !str_cmp( arg, "save" ) )
	    {
		save_sysdata( );
		return;
	    }

	    if ( !str_cmp( arg, "stat" ) )
	    {
		int        minutes;
		int        seconds;

		minutes = PULSE_DB_DUMP / 60 / PULSE_PER_SECOND;
		seconds = PULSE_DB_DUMP - minutes * 60 * PULSE_PER_SECOND;

		printf_to_char( ch, "{o{cPlayers: {r%5d{c  MaxPlayers : {r%5d{x\n\r",
				num_descriptors, sysdata.max_players );
		printf_to_char( ch, "{o{cMaxEver: {r%5d{c  Recorded at: {r%s{x\n\r",
				sysdata.all_time_max, sysdata.time_of_max );

		if ( IS_SET( sysdata.act, MUD_AUTOSAVE_DB ) )
		    printf_to_char( ch, "{o{cThe server autosaves the db every %d minutes, %d seconds.{x\n\r",
				    minutes, seconds );
		else
		    send_to_char( "{o{cThe mud database isn't being autosaved.{x\n\r", ch );
		return;
	    }

	    send_to_char( "Mudconfig -option or +option?\n\r", ch );
	    send_to_char( "or:        save              \n\r", ch );
	    send_to_char( "or:        stat              \n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "autosavedb"	) ) bit = MUD_AUTOSAVE_DB;
	else
	{
	    send_to_char( "Mudconfig which option?\n\r", ch );
	    return;
	}

	if ( fSet )
	{
	    SET_BIT    ( sysdata.act, bit );
	    sprintf( buf, "%s is now ON.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}
	else
	{
	    REMOVE_BIT ( sysdata.act, bit );
	    sprintf( buf, "%s is now OFF.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}

    }

    return;
}
