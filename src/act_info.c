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


char *	const	where_name	[] =
{
    "<used as light>     ",
    "<worn on finger>    ",
    "<worn on finger>    ",
    "<worn around neck>  ",
    "<worn around neck>  ",
    "<worn on body>      ",
    "<worn on head>      ",
    "<worn on legs>      ",
    "<worn on feet>      ",
    "<worn on hands>     ",
    "<worn on arms>      ",
    "<worn as shield>    ",
    "<worn about body>   ",
    "<worn about waist>  ",
    "<worn around wrist> ",
    "<worn around wrist> ",
    "<wielded, primary>  ",
    "<held>              ",
    "<wielded, second>   ",
    "<missile held>      "
};



/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				       bool fShort ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				       bool fShort, bool fShowNothing ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );



char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf [ MAX_STRING_LENGTH ];

    buf[0] = '\0';
    if ( IS_OBJ_STAT( obj, ITEM_INVIS)     )   strcat( buf, "{o{c(Invis){x "     );
    if ( IS_OBJ_STAT( obj, ITEM_VIS_DEATH) )   strcat( buf, "{o{w(VisDeath){x "  );
    if ( ( IS_AFFECTED( ch, AFF_DETECT_EVIL  )
	  || ( IS_SET( race_table[ ch->race ].race_abilities,
		      RACE_DETECT_ALIGN )
	      && IS_GOOD( ch ) ) )
	&& IS_OBJ_STAT( obj, ITEM_EVIL )   )   strcat( buf, "{o{r(Red Aura){x " );
    if ( ( IS_AFFECTED( ch, AFF_DETECT_GOOD  )
	  || ( IS_SET( race_table[ ch->race ].race_abilities,
		      RACE_DETECT_ALIGN )
	      && IS_EVIL( ch ) ) )
	&& IS_OBJ_STAT( obj, ITEM_BLESS )   )   strcat( buf, "{o{c(Blue Aura){x " );
    if ( IS_AFFECTED( ch, AFF_DETECT_MAGIC )
	&& IS_OBJ_STAT( obj, ITEM_MAGIC )  )   strcat( buf, "{o{y(Magical){x "   );
    if ( IS_OBJ_STAT( obj, ITEM_GLOW )     )   strcat( buf, "{o{y(Glowing){x "   );
    if ( IS_OBJ_STAT( obj, ITEM_HUM )      )   strcat( buf, "{o{y(Humming){x "   );
    if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )   strcat( buf, "{o{g(Poisoned){x "  );
    strcat ( buf, "{o{g");
    
    if ( fShort )
    {
	if ( obj->short_descr )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( obj->description )
	    strcat( buf, obj->description );
    }
    strcat ( buf, "{x");

    return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    OBJ_DATA  *obj;
    char       buf [ MAX_STRING_LENGTH ];
    char       buf1[ MAX_STRING_LENGTH * 4 ];
    char     **prgpstrShow;
    char      *pstrShow;
    int       *prgnShow;
    int        nShow;
    int        iShow;
    int        count;
    bool       fCombine;

    if ( !ch->desc || !list )
	return;

    buf1[0] = '\0';
    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj; obj = obj->next_content )
    {
	if ( obj->deleted )
	    continue;
	count++;
    }

    prgpstrShow	= alloc_mem( count * sizeof( char * ) );
    prgnShow    = alloc_mem( count * sizeof( int )    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj; obj = obj->next_content )
    { 
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );
	    fCombine = FALSE;

	    if ( IS_NPC( ch ) || IS_SET( ch->act, PLR_COMBINE ) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if ( IS_NPC( ch ) || IS_SET( ch->act, PLR_COMBINE ) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "{o{g(%2d){x ", prgnShow[iShow] );
		strcat( buf1, buf );
	    }
	    else
	    {
		strcat( buf1, "     " );
	    }
	}
	strcat( buf1, prgpstrShow[iShow] );
	strcat( buf1, "{x\n\r" );
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC( ch ) || IS_SET( ch->act, PLR_COMBINE ) )
	    strcat( buf1, "     " );
	strcat( buf1, "{o{rNothing.{x\n\r" );
    }

    /*
     * Clean up.
     */
    free_mem( prgpstrShow, count * sizeof( char * ) );
    free_mem( prgnShow,    count * sizeof( int )    );

    send_to_char( buf1, ch );
    return;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf [ MAX_STRING_LENGTH ];

    buf[0] = '\0';

    if ( !victim || !ch )
	return;

    if ( victim->rider
	&& victim->rider->in_room == ch->in_room )
        return;

    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_WIZINVIS )
	&& get_trust( victim ) < get_trust( ch ) )
                                                strcat( buf, "{o{d(Wizinvis){r "   );
    if ( IS_AFFECTED( victim, AFF_INVISIBLE )   )
                                                strcat( buf, "{o{c(Invis){r "      );
    if ( IS_AFFECTED( victim, AFF_HIDE )        )
                                                strcat( buf, "{o{y(Hide){r "       );
    if ( IS_AFFECTED( victim, AFF_CHARM )       )
                                                strcat( buf, "{o{g(Charmed){r "    );
    if ( IS_AFFECTED( victim, AFF_PASS_DOOR )
	|| ( IS_SET( race_table[ victim->race ].race_abilities, RACE_PASSDOOR )
	    && (   !str_cmp( race_table[victim->race].name, "Undead" )
		|| !str_cmp( race_table[victim->race].name, "Vampire" ) ) ) )
                                                strcat( buf, "{o{c(Translucent){r ");
    if ( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
                                                strcat( buf, "{o{m(Pink Aura){r "  );
    if ( IS_EVIL( victim )
	&& ( IS_AFFECTED( ch, AFF_DETECT_EVIL )
	    || ( IS_SET( race_table[ ch->race ].race_abilities,
			RACE_DETECT_ALIGN )
		&& IS_GOOD( ch ) ) ) )
                                                strcat( buf, "{o{r(Red Aura){r "   );
    if ( IS_GOOD( victim )
	&& ( IS_AFFECTED( ch, AFF_DETECT_GOOD )
	    || ( IS_SET( race_table[ ch->race ].race_abilities,
			RACE_DETECT_ALIGN )
		&& IS_EVIL( ch ) ) ) )
                                                strcat( buf, "{o{y(Golden Aura){r ");
    if ( IS_AFFECTED( victim, AFF_SANCTUARY )
	|| IS_SET( race_table[ victim->race ].race_abilities, RACE_SANCT ) )
                                                strcat( buf, "{o{w(White Aura){r " );
    if ( IS_AFFECTED( victim, AFF_FLAMING ) )   strcat( buf, "{o{d(Black Aura){r " );
    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER )  )
						strcat( buf, "(KILLER) "     );
    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_THIEF  )  )
						strcat( buf, "(THIEF) "      );
    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_REGISTER  )  )
						strcat( buf, "(PK) "      );
    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_AFK )  )
						strcat( buf, "[AFK] "        );

    strcat ( buf, "{o{r" );
    if ( victim->position == POS_STANDING && victim->long_descr[0] != '\0' )
    {
	strcat( buf, victim->long_descr );
        strcat( buf, "{x" );
	send_to_char( buf, ch );
	return;
    }

    strcat( buf, PERS( victim, ch ) );
    if ( !IS_NPC( victim ) && !IS_SET( ch->act, PLR_BRIEF ) )
	strcat( buf, victim->pcdata->title );

    switch ( victim->position )
    {
    case POS_DEAD:     strcat( buf, " is DEAD!!"              ); break;
    case POS_MORTAL:   strcat( buf, " is mortally wounded"   ); break;
    case POS_INCAP:    strcat( buf, " is incapacitated"      ); break;
    case POS_STUNNED:  strcat( buf, " is lying here stunned" ); break;
    case POS_SLEEPING: strcat( buf, " is sleeping here"      ); break;
    case POS_RESTING:  strcat( buf, " is resting here"       ); break;
    case POS_STANDING: strcat( buf, " is here"               ); break;
    case POS_FIGHTING:
	strcat( buf, " is here, fighting " );
	if ( !victim->fighting )
	    strcat( buf, "thin air??" );
	else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	else if ( victim->in_room == victim->fighting->in_room )
	    strcat( buf, PERS( victim->fighting, ch ) );
	else
	    strcat( buf, "someone who left??" );
	break;
    }

    if ( victim->riding
	&& victim->riding->in_room == victim->in_room )
    {
        char buf1 [ MAX_STRING_LENGTH ];

        sprintf( buf1, ", mounted on %s.", PERS( victim->riding, ch ) );
        strcat( buf, buf1 );
    }
    else
	strcat( buf, "." );

    strcat( buf, "{x\n\r" );
    buf[0] = UPPER( buf[0] );
    send_to_char( buf, ch );
    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    char      buf  [ MAX_STRING_LENGTH ];
    char      buf1 [ MAX_STRING_LENGTH ];
    int       iWear;
    int       percent;
    bool      found;

    if ( can_see( victim, ch ) )
    {
	act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
	act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
    }

    buf[0] = '\0';
    if ( victim->riding )
    {
	sprintf( buf1, "Mounted on %s, ", PERS( victim->riding, ch ) );
	strcat( buf, buf1 );
    }
    else
    if ( victim->rider )
    {
	sprintf( buf1, "Ridden by %s, ", PERS( victim->rider, ch ) );
	strcat( buf, buf1 );
    }

    sprintf( buf1, "$N the %s:\n\r", race_table[ victim->race ].name );
    strcat( buf, buf1 );

    if ( !IS_NPC( victim ) && is_clan( victim  ) )
    {
	sprintf( buf1, "%s of %s.{x\n\r", 
		flag_string( rank_flags, victim->pcdata->rank ),
		victim->pcdata->clan->name );
	buf1[0] = UPPER( buf1[0] );
	strcat( buf, buf1 );
    }
    
    act( buf, ch, NULL, victim, TO_CHAR );

    if ( victim->description[0] != '\0' )
    {
	send_to_char( victim->description, ch );
    }
    else
    {
	act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    if ( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

    strcpy( buf, PERS( victim, ch ) );

         if ( percent >= 100 ) strcat( buf, " is in perfect health.\n\r"  );
    else if ( percent >=  90 ) strcat( buf, " is slightly scratched.\n\r" );
    else if ( percent >=  80 ) strcat( buf, " has a few bruises.\n\r"     );
    else if ( percent >=  70 ) strcat( buf, " has some cuts.\n\r"         );
    else if ( percent >=  60 ) strcat( buf, " has several wounds.\n\r"    );
    else if ( percent >=  50 ) strcat( buf, " has many nasty wounds.\n\r" );
    else if ( percent >=  40 ) strcat( buf, " is bleeding freely.\n\r"    );
    else if ( percent >=  30 ) strcat( buf, " is covered in blood.\n\r"   );
    else if ( percent >=  20 ) strcat( buf, " is leaking guts.\n\r"       );
    else if ( percent >=  10 ) strcat( buf, " is almost dead.\n\r"        );
    else                       strcat( buf, " is DYING.\n\r"              );

    buf[0] = UPPER( buf[0] );
    send_to_char( buf, ch );

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) )
	    && can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		act( "{o{r$E is using{x", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    send_to_char( where_name[iWear], ch );
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
    }

    if ( ( victim != ch
	 && !IS_NPC( ch ) && number_percent( ) < ch->pcdata->learned[gsn_peek] )
	|| ch->riding == victim 
	|| ch->rider  == victim )
    {
	send_to_char( "\n\r{o{rYou peek at the inventory:{x\n\r", ch );
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }

    return;
}



void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    if ( !list )
        return;

    for ( rch = list; rch; rch = rch->next_in_room )
    {
        if ( rch->deleted || rch == ch )
	    continue;

	if ( !IS_NPC( rch )
	    && IS_SET( rch->act, PLR_WIZINVIS )
	    && get_trust( ch ) < get_trust( rch ) )
	    continue;

	if ( can_see( ch, rch ) )
	{
	    show_char_to_char_0( rch, ch );
	}
	else if ( room_is_dark( ch->in_room )
		 && ( IS_AFFECTED( rch, AFF_INFRARED )
		     || IS_SET( race_table[ rch->race ].race_abilities,
			       RACE_INFRAVISION ) ) )
	{
	    send_to_char( "{o{rYou see glowing red eyes watching YOU!{x\n\r", ch );
	}
    }

    return;
} 



bool check_blind( CHAR_DATA *ch )
{
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( IS_AFFECTED( ch, AFF_BLIND ) )
    {
	send_to_char( "You can't see a thing!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}



void do_look( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    char      *pdesc;
    int        door;

    if ( !IS_NPC( ch ) && !ch->desc ) 
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "You can't see anything but stars!  See how pretty!\n\r",
		     ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "You can't see anything, you're sleeping!  Zzz.\n\r",
		     ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC( ch )
	&& !IS_SET( ch->act, PLR_HOLYLIGHT )
	&& room_is_dark( ch->in_room ) )
    {
	send_to_char( "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
	if ( !IS_SET( ch->act, PLR_EDIT_INFO ) )
	{
	    sprintf ( buf, "{o{c%s{x\n\r", ch->in_room->name );
	    send_to_char( buf, ch );
	}
	else
	    show_room_info( ch, ch->in_room );

	if ( arg1[0] == '\0'
	    || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_BRIEF ) ) ) 
	{
            sprintf ( buf, "{o{y%s{x", ch->in_room->description );
	    send_to_char( buf, ch );
	}
	
        if ( IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	    send_to_char( "{o{dIt seems eerily quiet.{x\n\r", ch );

	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOEXIT ) )
	    do_exits( ch, "auto" );

	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->people,   ch );
	return;
    }

    if ( !str_prefix( arg1, "in" ) )
    {
	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( !( obj = get_obj_here( ch, arg2 ) ) )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "That is not a container.\n\r", ch );
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "It is empty.\n\r", ch );
		break;
	    }

	    sprintf( buf, "It's %s full of a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than half" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about half"     : "more than half",
		liq_table[obj->value[2]].liq_color
		);

	    send_to_char( buf, ch );
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET( obj->value[1], CONT_CLOSED ) )
	    {
		send_to_char( "It is closed.\n\r", ch );
		break;
	    }

	    act( "$p contains:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    break;
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) )
    {
	show_char_to_char_1( victim, ch );
	return;
    }

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if ( pdesc )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if ( pdesc )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if ( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    send_to_char( "\n\r", ch );
	    return;
	}
    }

    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if ( pdesc )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if ( pdesc )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if ( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    send_to_char( "\n\r", ch );
	    return;
	}
    }

    pdesc = get_extra_descr( arg1, ch->in_room->extra_descr );
    if ( pdesc )
    {
	send_to_char( pdesc, ch );
	return;
    }

         if ( !str_prefix( arg1, "north" ) ) door = 0;
    else if ( !str_prefix( arg1, "east"  ) ) door = 1;
    else if ( !str_prefix( arg1, "south" ) ) door = 2;
    else if ( !str_prefix( arg1, "west"  ) ) door = 3;
    else if ( !str_prefix( arg1, "up"    ) ) door = 4;
    else if ( !str_prefix( arg1, "down"  ) ) door = 5;
    else
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    /* 'look direction' */
    if ( !( pexit = ch->in_room->exit[door] ) )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }

    if ( pexit->description && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );

    if (   pexit->keyword
	&& pexit->keyword[0] != '\0'
	&& pexit->keyword[0] != ' ' )
    {
             if ( IS_SET( pexit->exit_info, EX_BASHED ) )
	    act( "The $d has been bashed from its hinges.",
		ch, NULL, pexit->keyword, TO_CHAR );
	else if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	else if ( IS_SET( pexit->exit_info, EX_ISDOOR ) )
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
    }

    return;
}



void do_examine( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      buf [ MAX_STRING_LENGTH ];
    char      arg [ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Examine what?\n\r", ch );
	return;
    }

    do_look( ch, arg );

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	switch ( obj->item_type )
	{
	default:
	    break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    send_to_char( "{o{bWhen you look inside, you see:{x\n\r", ch );
	    sprintf( buf, "in %s", arg );
	    do_look( ch, buf );
	}
    }

    return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
           EXIT_DATA       *pexit;
    extern char *    const  dir_name [ ];
           char             buf      [ MAX_STRING_LENGTH ];
           int              door;
           bool             found;
           bool             fAuto;

    buf[0] = '\0';
    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
	return;

    strcpy( buf, fAuto ? "{o{m[Exits:" : "Obvious exits:\n\r" );

    found = FALSE;
    for ( door = 0; door <= 5; door++ )
    {
	if ( ( pexit = ch->in_room->exit[door] )
	    && pexit->to_room
	    && !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		strcat( buf, " " );
		strcat( buf, dir_name[door] );
	    }
	    else
	    {
		sprintf( buf + strlen( buf ), "%-5s - %s\n\r",
		    capitalize( dir_name[door] ),
		    room_is_dark( pexit->to_room )
			?  "Too dark to tell"
			: pexit->to_room->name
		    );
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " none" : "None.\n\r" );

    if ( fAuto )
	strcat( buf, "]{x\n\r" );

    send_to_char( buf, ch );
    return;
}



/*
 * Enhanced version of do_exits() done by Zen.
 */
void show_room_info( CHAR_DATA *ch, ROOM_INDEX_DATA *room )
{
    char buf	[ MAX_STRING_LENGTH ];
    char buf1	[ MAX_STRING_LENGTH ];

    if ( !IS_SET( ch->act, PLR_EDIT_INFO ) )
	return;

    buf[0] = '\0';
    strcpy ( buf1, flag_string( room_flags, room->room_flags ) );
    sprintf( buf, "{m[{o%d{x{m]{o{c %s{x ( %s ) [ %s ]{x\n\r",
	    room->vnum,
	    room->name,
	    flag_string( sector_flags, room->sector_type ),
	    all_capitalize( buf1 ) );
    buf1[0] = '\0';
    strcat( buf1, buf );
    send_to_char( buf1, ch );

    return;
}



void do_score( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf;
    char         buf  [ MAX_STRING_LENGTH ];
    char         buf1 [ MAX_STRING_LENGTH ];
    char         buf2 [ MAX_STRING_LENGTH ];

    buf1[0] = '\0';

    sprintf( buf,
	    "{o{cScore for %s%s.{x\n\r",
	    ch->name,
	    IS_NPC( ch ) ? "" : ch->pcdata->title );
    strcat( buf1, buf );

    if ( get_trust( ch ) != ch->level )
    {
	sprintf( buf, "{o{cYou are trusted at level %d.{x\n\r",
		get_trust( ch ) );
	strcat( buf1, buf );
    }

    strcat( buf1, "{o{c-----------------------------------------------------------------------------{x\n\r" );

    sprintf( buf,
	    "{o{cLEVEL: %-3d         Race : %-10.10s        Played: %d hours{x\n\r",
	    ch->level,
	    race_table[ ch->race ].name,
	    ( ch->played / 3600 ) );
    strcat( buf1, buf );

    sprintf( buf,
	    "{o{cYEARS: %-6d      Class: %-11.11s       Time:   %s{x\r",
	    get_age( ch ),
	    class_table[ ch->class ]->name,
	    ctime ( &current_time ) );
    strcat( buf1, buf );

    sprintf( buf,
	    "{o{cSTR  : %2d(%2d)      Armor: ",
	    get_curr_str( ch ), get_max_str( ch ) );
    strcat( buf1, buf );

    if ( ch->level >= 25 )
    {
	sprintf( buf, "%4.4d, ", GET_AC( ch ) );
	strcat( buf1, buf );
    }

         if ( GET_AC( ch ) >=  101 ) strcat( buf1, "WORSE than naked!" );
    else if ( GET_AC( ch ) >=   80 ) strcat( buf1, "naked."            );
    else if ( GET_AC( ch ) >=   60 ) strcat( buf1, "wearing clothes."  );
    else if ( GET_AC( ch ) >=   40 ) strcat( buf1, "slightly armored." );
    else if ( GET_AC( ch ) >=   20 ) strcat( buf1, "somewhat armored." );
    else if ( GET_AC( ch ) >=    0 ) strcat( buf1, "armored."          );
    else if ( GET_AC( ch ) >= - 20 ) strcat( buf1, "well armored."     );
    else if ( GET_AC( ch ) >= - 40 ) strcat( buf1, "strongly armored." );
    else if ( GET_AC( ch ) >= - 60 ) strcat( buf1, "heavily armored."  );
    else if ( GET_AC( ch ) >= - 80 ) strcat( buf1, "superbly armored." );
    else if ( GET_AC( ch ) >= -100 ) strcat( buf1, "divinely armored." );
    else                           strcat( buf1, "invincible!"       );
    strcat( buf1, "{x\n\r" );

    sprintf( buf,
	    "{o{cINT  : %2d(%2d)      Align: ",
	    get_curr_int( ch ), get_max_int( ch ) );
    strcat( buf1, buf );

         if ( ch->alignment >  900 ) strcpy( buf2, "angelic." );
    else if ( ch->alignment >  700 ) strcpy( buf2, "saintly." );
    else if ( ch->alignment >  350 ) strcpy( buf2, "good."    );
    else if ( ch->alignment >  100 ) strcpy( buf2, "kind."    );
    else if ( ch->alignment > -100 ) strcpy( buf2, "neutral." );
    else if ( ch->alignment > -350 ) strcpy( buf2, "mean."    );
    else if ( ch->alignment > -700 ) strcpy( buf2, "evil."    );
    else if ( ch->alignment > -900 ) strcpy( buf2, "demonic." );
    else                             strcpy( buf2, "satanic." );

    if ( ch->level >= 10 )
	sprintf( buf, "%+4.4d, %-14.14s   ", ch->alignment, buf2 );
    else
	sprintf( buf, "%-20.20s   ", buf2 );
    strcat( buf1, buf );

    sprintf( buf,
	    "Items: %5.5d   (max %5.5d){x\n\r",
	    ch->carry_number, can_carry_n( ch ) );
    strcat( buf1, buf );

    switch ( ch->position )
    {
    case POS_DEAD:     
	strcpy( buf2, "DEAD!!"            ); break;
    case POS_MORTAL:
	strcpy( buf2, "mortally wounded." ); break;
    case POS_INCAP:
	strcpy( buf2, "incapacitated."    ); break;
    case POS_STUNNED:
	strcpy( buf2, "stunned."          ); break;
    case POS_SLEEPING:
	strcpy( buf2, "sleeping."         ); break;
    case POS_RESTING:
	strcpy( buf2, "resting."          ); break;
    case POS_STANDING:
	strcpy( buf2, "standing."         ); break;
    case POS_FIGHTING:
	strcpy( buf2, "fighting."         ); break;
    }

    sprintf( buf,
	    "{o{cWIS  : %2d(%2d)      Pos'n: %-21.21s  Weight: %5.5d (max %7.7d){x\n\r",
	    get_curr_wis( ch ), get_max_wis( ch ), buf2,
	    ch->carry_weight, can_carry_w( ch ) );
    strcat( buf1, buf );

    sprintf( buf,
	    "{o{cDEX  : %2d(%2d)      Wimpy: %d{x\n\r",
	    get_curr_dex( ch ), get_max_dex( ch ), ch->wimpy );
    strcat( buf1, buf );

    sprintf( buf,
	    "{o{cCON  : %2d(%2d)                                                    Autosac (%c){x\n\r",
	    get_curr_con( ch ), get_max_con( ch ),
	    ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOSAC  ) ) ? 'X'
	                                                         : ' ' );
    strcat( buf1, buf );

    sprintf( buf,
	    "{o{cPRACT: %3.3d         Hitpoints: %-5d of %5d   Pager: (%c) %3d    Autoexit(%c){x\n\r",
	    ch->practice,
	    ch->hit,  ch->max_hit,
	    ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_PAGER    ) ) ? 'X'
	                                                         : ' ',
	    ( IS_NPC( ch ) ? 0 : ch->pcdata->pagelen ),
	    ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOEXIT ) ) ? 'X'
	                                                         : ' ' );
    strcat( buf1, buf );

    sprintf( buf,
	    "{o{cXP   : %-9d        Mana: %-5d of %5d   MKills:  %-5.5d    Autoloot(%c){x\n\r",
	    ch->exp,
	    ch->mana, ch->max_mana,
	    ( IS_NPC( ch ) ? 0 : ch->pcdata->mkills ),
	    ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOLOOT ) ) ? 'X'
	                                                         : ' ' );
    strcat( buf1, buf );

    sprintf( buf,
	    "{o{cGOLD : %-10d       Move: %-5d of %5d   MDeaths: %-5.5d    Autogold(%c){x\n\r",
	    ch->gold,
	    ch->move, ch->max_move,
	    ( IS_NPC( ch ) ? 0 : ch->pcdata->mdeaths ),
	    ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOGOLD ) ) ? 'X'
	                                                         : ' ' );
    strcat( buf1, buf );

    if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_DRUNK]   > 10 )
	strcat( buf1, "{o{cYou are drunk.{x\n\r"   );
    if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_THIRST] ==  0
	&& ch->level < LEVEL_IMMORTAL )
	strcat( buf1, "{o{cYou are thirsty.{x\n\r" );
    if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL]   ==  0
	&& ch->level < LEVEL_IMMORTAL )
	strcat( buf1, "{o{cYou are hungry.{x\n\r"  );

        strcat( buf1, "{x\n\r" );

    if ( ch->level >= 15 )
    {
        if ( IS_NPC( ch )
	    || ch->level >=
                skill_table[gsn_dual].skill_level[ch->class] )
            strcat ( buf1, "{o{cPrimary weapon       " );
        sprintf( buf, "{o{cHitRoll: %-3d              DamRoll: %-3d",
            get_hitroll( ch, WEAR_WIELD ), get_damroll( ch, WEAR_WIELD) );
        strcat( buf1, buf );
        strcat( buf1, "{x\n\r" );
        if ( get_eq_char( ch, WEAR_WIELD_2 ) )
        {
            sprintf( buf, "{o{cSecondary weapon     HitRoll: %-3d              DamRoll: %-3d{x\n\r",
		get_hitroll( ch, WEAR_WIELD_2 ),
		get_damroll( ch, WEAR_WIELD_2 ) );
            strcat( buf1, buf );
        }
    }
    
    if ( !IS_NPC( ch ) &&
	( IS_SET( ch->act, PLR_REGISTER ) ||
	  !str_cmp( race_table[ch->race].name, "Vampire" ) ) )
    {
	strcat( buf1, "{o{c-----------------------------------------------------------------------------{x\n\r" );

	sprintf( buf,
		"{o{cPKILL DATA:  PKills: %3.3d    PDeaths: %3.3d  Illegal PKills: %3.3d{x\n\r",
		ch->pcdata->pkills, ch->pcdata->pdeaths,
		ch->pcdata->illegal_pk );
	strcat( buf1, buf );
    }

    if ( !IS_NPC( ch ) && is_clan( ch )
	&& ch->pcdata->clan->clan_type != CLAN_ORDER
	&& ch->pcdata->clan->clan_type != CLAN_GUILD )
    {
	strcat( buf1, "{o{c-----------------------------------------------------------------------------{x\n\r" );

	sprintf( buf,
		"{o{cCLAN STATS:  %-15.15s  Clan PKills:  %-6.6d     Clan PDeaths:  %-6.6d{x\n\r",
		ch->pcdata->clan->name, ch->pcdata->clan->pkills,
		ch->pcdata->clan->pdeaths );
	strcat( buf1, buf );

	sprintf( buf, "{o{cYou are %s of %s.{x\n\r",
		flag_string( rank_flags, ch->pcdata->rank ),
		ch->pcdata->clan->name );
	strcat( buf1, buf );
    }

    if ( !IS_NPC( ch ) && is_clan( ch )
	&& ch->pcdata->clan->clan_type == CLAN_ORDER )
    {
	strcat( buf1, "{o{c-----------------------------------------------------------------------------{x\n\r" );

	sprintf( buf,
		"{o{cOrder:  %-20s  Order MKills:  %-6.6d   Order MDeaths:  %-6.6d{x\n\r",
		ch->pcdata->clan->name, ch->pcdata->clan->mkills,
		ch->pcdata->clan->mdeaths );
	strcat( buf1, buf );

	sprintf( buf, "{o{cYou are %s of %s.{x\n\r",
		flag_string( rank_flags, ch->pcdata->rank ),
		ch->pcdata->clan->name );
	strcat( buf1, buf );
    }

    if ( !IS_NPC( ch ) && is_clan( ch )
	&& ch->pcdata->clan->clan_type == CLAN_GUILD )
    {
	strcat( buf1, "{o{c-----------------------------------------------------------------------------{x\n\r" );

	sprintf( buf,
		"{o{cGuild:  %-20s  Guild MKills:  %-6.6d   Guild MDeaths:  %-6.6d{x\n\r",
		ch->pcdata->clan->name, ch->pcdata->clan->mkills,
		ch->pcdata->clan->mdeaths );
	strcat( buf1, buf );

	sprintf( buf, "{o{cYou are %s of %s.{x\n\r",
		flag_string( rank_flags, ch->pcdata->rank ),
		ch->pcdata->clan->name );
	strcat( buf1, buf );
    }

    if ( ch->affected )
    {
        bool printed = FALSE;

	for ( paf = ch->affected; paf; paf = paf->next )
	{
	    if ( paf->deleted )
	        continue;

	    if ( !printed )
	    {
		strcat( buf1, "{o{c-----------------------------------------------------------------------------{x\n\r" );
		strcat( buf1, "{o{cAFFECT DATA:{x\n\r" );
		printed = TRUE;
	    }

	    sprintf( buf, "{o{c[ '%s'", skill_table[paf->type].name );
	    strcat( buf1, buf );

	    if ( ch->level >= 20 )
	    {
		sprintf( buf,
			" modifies %s by %d for %d hours",
			affect_loc_name( paf->location ),
			paf->modifier,
			paf->duration );
		strcat( buf1, buf );
	    }

	    strcat( buf1, " ].{x\n\r" );
	}
    }

    send_to_char( buf1, ch );
    return;
}



char *	const	day_name	[] =
{
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *	const	month_name	[] =
{
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
           char  buf           [ MAX_STRING_LENGTH ];
    extern char  str_boot_time[];
           char *suf;
           int   day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
	    "{o{yIt is %d o'clock %s, Day of %s, %d%s the Month of %s.{x\n\r",
	    ( time_info.hour % 12 == 0 ) ? 12 : time_info.hour % 12,
	    time_info.hour >= 12 ? "pm" : "am",
	    day_name[day % 7],
	    day, suf,
	    month_name[time_info.month] );
    send_to_char( buf, ch );
    sprintf( buf,
	    "{o{yEnvyMud started up %s\rThe system time is %s{x\r",
	    str_boot_time,
	    (char *) ctime( &current_time ) );
    send_to_char( buf, ch );
    if ( down_time > 0 )
    {
	sprintf( buf, "{o{y%s will be at %s{x\r", Reboot ? "Reboot" : "Shutdown",
		(char *) ctime( &down_time ) );
	send_to_char( buf, ch );
    }
    return;
}



void do_weather( CHAR_DATA *ch, char *argument )
{
           char         buf     [ MAX_STRING_LENGTH ];
           char         descr   [ MAX_STRING_LENGTH ];
           int          wind;
    static char * const sky_look[ 8 ] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning",
	"slightly overcast",
	"scattered with a few flakes",
	"filled with flakes",
	"a blizzard of white"
    };

    if ( !IS_OUTSIDE( ch ) )
    {
	send_to_char( "You can't see the weather indoors.\n\r", ch );
	return;
    }

    wind = weather_info.windspeed;
    descr[0] = '\0';
    sprintf( buf, "The sky is %s ",
	    (time_info.month <= 4 || time_info.month >= 15)
	    ? sky_look[weather_info.sky + 4]
	    : sky_look[weather_info.sky] );
    strcat( descr, buf );
    sprintf( buf, "and a %s %sward %s blows.\n\r",
	    ( weather_info.temperature < 35 ) ? "cold" :
	    ( weather_info.temperature < 60 ) ? "cool" :
	    ( weather_info.temperature < 90 ) ? "warm" : "hot",
	    dir_name[abs( weather_info.winddir ) % 3],
	    wind <= 20 ? "breeze" :
	    wind <= 50 ? "wind" :
	    wind <= 80 ? "gust" :
			 "torrent" );
    strcat( descr, buf );

    send_to_char( descr, ch );
    return;
}



void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    bool       hfound = FALSE; /* modification to show multiple helps
				  by Robin Goodall from merc mailing list */

    if ( argument[0] == '\0' )
	argument = "summary";

    for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
    {
	if ( pHelp->level > get_trust( ch ) )
	    continue;

	if ( is_name( argument, pHelp->keyword ) )
	{
	    if ( hfound )
	        send_to_char( "\n\r----------------------------------------------------------------------------\n\r\n\r", ch );

	    if ( pHelp->level >= 0 && str_cmp( argument, "imotd" ) )
	    {
		send_to_char( pHelp->keyword, ch );
		send_to_char( "\n\r", ch );
	    }

	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if ( pHelp->text[0] == '.' )
		send_to_char( pHelp->text+1, ch );
	    else
		send_to_char( pHelp->text  , ch );
	    hfound = TRUE;
	}
    }
    
    if ( !hfound )
        send_to_char( "No help on that word.\n\r", ch );
    return;
}



/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char             buf      [ MAX_STRING_LENGTH*3 ];
    int              iClass;
    int              iLevelLower;
    int              iLevelUpper;
    int              nNumber;
    int              nMatch;
    bool             rgfClass [ MAX_CLASS ];
    bool             fClassRestrict;
    bool             fImmortalOnly;

    WHO_DATA *curr_who		= NULL;
    WHO_DATA *next_who		= NULL;
    WHO_DATA *first_mortal	= NULL;
    WHO_DATA *first_imm		= NULL;
    WHO_DATA *first_pk		= NULL;

    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = L_DIR; /*Used to be Max_level */
    fClassRestrict = FALSE;
    fImmortalOnly  = FALSE;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	rgfClass[iClass] = FALSE;

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
	    if ( !str_cmp( arg, "imm" ) )
	    {
		fImmortalOnly = TRUE;
	    }
	    else
	    {
		fClassRestrict = TRUE;
		for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		{
		    if ( !str_cmp( arg, class_table[iClass]->who_name ) )
		    {
			rgfClass[iClass] = TRUE;
			break;
		    }
		}

		if ( iClass == MAX_CLASS )
		{
		    send_to_char( "That's not a class.\n\r", ch );
		    return;
		}
	    }
	}
    }

    /*
     * Now show matching chars.
     */
    nMatch = 0;
    for ( d = descriptor_list; d; d = d->next )
    {
	CHAR_DATA       *wch;
	char      const *class;

	wch   = ( d->original ) ? d->original : d->character;

	/*
	 * Check for match against restrictions.
	 * Don't use trust as that exposes trusted mortals.
	 */
	if ( d->connected != CON_PLAYING || !can_see( ch, wch ) )
	    continue;

	if (   wch->level < iLevelLower
	    || wch->level > iLevelUpper
	    || ( fImmortalOnly  && wch->level < LEVEL_HERO )
	    || ( fClassRestrict && !rgfClass[wch->class] ) )
	    continue;

	nMatch++;
	buf[0] = '\0';

	/*
	 * Figure out what to print for class.
	 */
	class = class_table[wch->class]->name;
	if ( wch->level >= LEVEL_IMMORTAL )
	    switch ( wch->level )
	      {
	      default: break;
	      case L_DIR: class = "Director";	break;
	      case L_SEN: class = "Senior";	break;
	      case L_JUN: class = "Junior";	break;
	      case L_APP: class = "Apprentice";	break;
	      }

	/*
	 * Format it up.
	 */
	if ( wch->level < LEVEL_IMMORTAL )
	{
	    char buf1 [ MAX_STRING_LENGTH ];
	    char buf2 [ MAX_STRING_LENGTH ];
	    char leftletter;
	    char rightletter;

	    sprintf( buf1, "%s%s%s%s%s",
		    IS_SET( wch->act, PLR_KILLER   ) ? "{o{r(KILLER) {g" : "",
		    IS_SET( wch->act, PLR_THIEF    ) ? "{o{r(THIEF) {g"  : "",
		    IS_SET( wch->act, PLR_AFK      ) ? "{y[AFK] {g"    : "",
		    wch->name,
		    wch->pcdata->title );
	    leftletter = !is_clan( wch ) 
			? ' '
			: ( wch->pcdata->clan->clan_type == CLAN_GUILD ) ? '<'
			: '(';
	    rightletter = !is_clan( wch ) 
			? ' '
			: ( wch->pcdata->clan->clan_type == CLAN_GUILD ) ? '>'
			: ')';
	    sprintf( buf2, " %c%s%c",
		    leftletter,
		    !is_clan( wch ) ? "" : wch->pcdata->clan->who_name,
		    rightletter );
	    sprintf( buf + strlen( buf ), "{o{g%-2d %-12s %s.%s{x\n\r",
		    wch->level,
		    class,
		    buf1,
		    buf2 );
	}
	else
	{
	    char buf1 [ MAX_STRING_LENGTH ];
	    char buf2 [ MAX_STRING_LENGTH ];
	    char leftletter;
	    char rightletter;

	    sprintf( buf1, "%s%s%s%s%s%s",
		    IS_SET( wch->act, PLR_WIZINVIS ) ? "(WIZINVIS) " : "", 
		    IS_SET( wch->act, PLR_KILLER   ) ? "(KILLER) " : "",
		    IS_SET( wch->act, PLR_THIEF    ) ? "(THIEF) "  : "",
		    IS_SET( wch->act, PLR_AFK      ) ? "{y[AFK] {o{g"    : "",
		    wch->name,
		    wch->pcdata->title );
	    leftletter = !is_clan( wch ) 
			? ' '
			: ( wch->pcdata->clan->clan_type == CLAN_GUILD ) ? '<'
			: '(';
	    rightletter = !is_clan( wch ) 
			? ' '
			: ( wch->pcdata->clan->clan_type == CLAN_GUILD ) ? '>'
			: ')';
	    sprintf( buf2, " %c%s%c",
		    leftletter,
		    !is_clan( wch ) ? "" : wch->pcdata->clan->who_name,
		    rightletter );
	    sprintf( buf + strlen( buf ), "{o{g%-15s %s.%s{x\n\r",
		    class,
		    buf1,
		    buf2 );
	}

	curr_who	= alloc_mem( sizeof( WHO_DATA ) );
	curr_who->text	= str_dup( buf );

	if ( wch->level < LEVEL_IMMORTAL )
	{
	    if ( IS_SET( wch->act, PLR_REGISTER ) )
	    {
		curr_who->next	= first_pk;
		first_pk	= curr_who;
	    }
	    else
	    {
		curr_who->next	= first_mortal;
		first_mortal	= curr_who;
	    }
	}
	else
	{
	    curr_who->next	= first_imm;
	    first_imm		= curr_who;
	}
    }

    for ( curr_who = first_mortal; curr_who; curr_who = next_who )
    {
	send_to_char( curr_who->text, ch );
	next_who = curr_who->next;
	free_string( curr_who->text );
	free_mem( curr_who, sizeof( WHO_DATA ) ); 
    }

    if ( first_pk )
	send_to_char( "\n\r{o{g---------------------------------[ PK CHARACTERS ]----------------------------{x\n\r\n\r", ch );

    for ( curr_who = first_pk; curr_who; curr_who = next_who )
    {
	send_to_char( curr_who->text, ch );
	next_who = curr_who->next;
	free_string( curr_who->text );
	free_mem( curr_who, sizeof( WHO_DATA ) ); 
    }

    if ( first_imm )
	send_to_char( "\n\r{o{g-----------------------------------[ IMMORTALS ]------------------------------{x\n\r\n\r", ch );

    for ( curr_who = first_imm; curr_who; curr_who = next_who )
    {
	send_to_char( curr_who->text, ch );
	next_who = curr_who->next;
	free_string( curr_who->text );
	free_mem( curr_who, sizeof( WHO_DATA ) ); 
    }

    sprintf( buf, "{o{yYou see %d player%s in the game.{x\n\r",
	    nMatch, nMatch == 1 ? "" : "s" );
    send_to_char( buf, ch );
    return;
}

/* Contributed by Kaneda */
void do_whois( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char             buf  [ MAX_STRING_LENGTH  ];
    char             name [ MAX_INPUT_LENGTH   ];
 
    one_argument( argument, name );

    if( name[0] == '\0' )
    {
	send_to_char( "Usage:  whois <name>\n\r", ch );
	return;
    }

    name[0] = UPPER( name[0] );

    buf[0] = '\0';
    for( d = descriptor_list ; d ; d = d->next )
    {
	CHAR_DATA       *wch;
	char      const *class;
	
	wch = ( d->original ) ? d->original : d->character; 

	if( d->connected != CON_PLAYING || !can_see( ch, wch ) )
	    continue;
  
	if( str_prefix( name, wch->name ) )
	    continue;

	class = class_table[ wch->class ]->name;
	if( wch->level >= LEVEL_IMMORTAL )
	    switch( wch->level )
	    {
	      case L_DIR: class = "Director";	break;
	      case L_SEN: class = "Senior";	break;
	      case L_JUN: class = "Junior";	break;
	      case L_APP: class = "Apprentice";	break;
	    }
    
	/*
	 * Format it up.
	 */
	if ( wch->level < LEVEL_IMMORTAL )
	{
	    sprintf( buf + strlen( buf ), "{o{g%s%s.{x\n\r",
		    wch->name,
		    wch->pcdata->title );
	    sprintf( buf + strlen( buf ), "{o{g%s%s%s%s{x\n\r",
		    IS_SET( wch->act, PLR_REGISTER ) ? "(PK) "     : "",
		    IS_SET( wch->act, PLR_KILLER   ) ? "{o{r(KILLER) {g" : "",
		    IS_SET( wch->act, PLR_THIEF    ) ? "{o{r(THIEF) {g"  : "",
		    IS_SET( wch->act, PLR_AFK      ) ? "{y[AFK] {g"    : "" );
	    sprintf( buf + strlen( buf ), "{o{glevel %d %s %s.{x\n\r",
		    wch->level,
		    class,
		    race_table[wch->race].name );
	}
	else
	{
	    sprintf( buf + strlen( buf ), "{o{g%s%s.{x\n\r",
		    wch->name,
		    wch->pcdata->title );
	    sprintf( buf + strlen( buf ), "{o{g%s%s%s%s%s{x\n\r",
		    IS_SET( wch->act, PLR_WIZINVIS ) ? "(WIZINVIS) " : "", 
		    IS_SET( wch->act, PLR_REGISTER ) ? "(PK) "     : "",
		    IS_SET( wch->act, PLR_KILLER   ) ? "{o{r(KILLER) {g" : "",
		    IS_SET( wch->act, PLR_THIEF    ) ? "{o{r(THIEF) {g"  : "",
		    IS_SET( wch->act, PLR_AFK      ) ? "{y[AFK] {g"    : "" );
	    sprintf( buf + strlen( buf ), "{o{g%s.{x\n\r",
		    class );
	}

	if ( is_clan( wch ) )
	{
	    sprintf( buf + strlen( buf ), "{o{g%s of %s.{x\n\r",
		    flag_string( rank_flags, wch->pcdata->rank ),
		    wch->pcdata->clan->name );
	}

	strcat( buf, "{o{g-----------------------------------------------------------------------------{x\n\r" );
      }

    if ( buf[0] == '\0' )
        send_to_char( "No one matches the given criteria.\n\r", ch );
    else
        send_to_char( buf, ch );
    return;
}


void do_inventory( CHAR_DATA *ch, char *argument )
{
    send_to_char( "{o{rYou are carrying:{x\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int       iWear;
    bool      found;

    send_to_char( "{o{rYou are using:{x\n\r", ch );
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( !( obj = get_eq_char( ch, iWear ) ) )
	    continue;

	send_to_char( where_name[iWear], ch );
	if ( can_see_obj( ch, obj ) )
	{
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
	else
	{
	    send_to_char( "something.\n\r", ch );
	}
	found = TRUE;
    }

    if ( !found )
	send_to_char( "{o{rNothing.{x\n\r", ch );

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    char     *msg;
    char      arg1 [ MAX_INPUT_LENGTH ];
    char      arg2 [ MAX_INPUT_LENGTH ];
    int       value1;
    int       value2;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if ( !( obj1 = get_obj_carry( ch, arg1 ) ) )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	for ( obj2 = ch->carrying; obj2; obj2 = obj2->next_content )
	{
	    if ( obj2->wear_loc != WEAR_NONE
		&& can_see_obj( ch, obj2 )
		&& obj1->item_type == obj2->item_type
		&& ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if ( !obj2 )
	{
	    send_to_char( "You aren't wearing anything comparable.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( !( obj2 = get_obj_carry( ch, arg2 ) ) )
	{
	    char new_arg2 [ MAX_INPUT_LENGTH ];
	    int  number;

	    /*  Strip off number argument, subtract one, paste it together */
	    number = number_argument( arg2, arg2 );
	    if ( number > 1 )  number--;
	    sprintf( new_arg2, "%d.%s", number, arg2 );

	    if ( !( obj2 = get_obj_wear( ch, new_arg2 ) ) )
	    {
		send_to_char( "You do not have that item.\n\r", ch );
		return;
	    }
	
	    if ( ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE ) == 0 )
	    {
		send_to_char( "They are not comparable items.\n\r", ch );
		return;
	    }

	}
    }
	    
    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = "You can't compare $p and $P.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0];
	    value2 = obj2->value[0];
	    break;

	case ITEM_CLOTHING:
	    value1 = obj1->value[0];
	    value2 = obj2->value[0];
	    break;

	case ITEM_WEAPON:
	    value1 = obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if ( !msg )
    {
        if ( obj2->wear_loc != WEAR_NONE )
        {
                 if ( value1 == value2 )
                     msg = "$p and $P (equipped) look about the same.";
           else if ( value1  > value2 )
                     msg = "$p looks better than $P (equipped).";
            else
                     msg = "$p looks worse than $P (equipped).";
        }
        else
        {
             if ( value1 == value2 ) msg = "$p and $P look about the same.";
        else if ( value1  > value2 ) msg = "$p looks better than $P.";
        else                         msg = "$p looks worse than $P.";
        }

    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
    do_help( ch, "diku" );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             buf [ MAX_STRING_LENGTH ];
    char             arg [ MAX_INPUT_LENGTH  ];
    bool             found;

    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Players near you:\n\r", ch );
	found = FALSE;
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
		&& ( victim = d->character )
		&& !IS_NPC( victim )
		&& victim->in_room
		&& victim->in_room->area == ch->in_room->area
		&& can_see( ch, victim ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
			victim->name, victim->in_room->name );
		send_to_char( buf, ch );
	    }
	}
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = char_list; victim; victim = victim->next )
	{
	    if ( !victim->in_room
		|| IS_AFFECTED( victim, AFF_HIDE ) 
		|| IS_AFFECTED( victim, AFF_SNEAK ) )
	        continue;

	    if ( victim->in_room->area == ch->in_room->area
		&& can_see( ch, victim )
		&& is_name( arg, victim->name ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
			PERS( victim, ch ), victim->in_room->name );
		send_to_char( buf, ch );
		break;
	    }
	}
	if ( !found )
	    act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      *buf                      = '\0';
    char      *msg;
    char       arg [ MAX_INPUT_LENGTH ];
    int        diff;
    int        hpdiff;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && !IS_SET( victim->act, PLR_KILLER ) )
    {
	send_to_char( "The gods do not accept this type of sacrifice.\n\r",
		     ch );
	return;
    }

    diff = victim->level - ch->level;

         if ( diff <= -10 ) msg = "You can kill $N naked and weaponless.";
    else if ( diff <=  -5 ) msg = "$N is no match for you.";
    else if ( diff <=  -2 ) msg = "$N looks like an easy kill.";
    else if ( diff <=   1 ) msg = "The perfect match!";
    else if ( diff <=   4 ) msg = "$N says 'Do you feel lucky, punk?'.";
    else if ( diff <=   9 ) msg = "$N laughs at you mercilessly.";
    else                    msg = "Death will thank you for your gift.";

    act( msg, ch, NULL, victim, TO_CHAR );

    /* additions by king@tinuviel.cs.wcu.edu */
    hpdiff = ( ch->hit - victim->hit );

    if ( ( ( diff >= 0) && ( hpdiff <= 0 ) )
	|| ( ( diff <= 0 ) && ( hpdiff >= 0 ) ) )
    {
        send_to_char( "Also,", ch );
    }
    else
    {
        send_to_char( "However,", ch );
    }

    if ( hpdiff >= 101 )
        buf = " you are currently much healthier than $E.";
    if ( hpdiff <= 100 )
        buf = " you are currently healthier than $E.";
    if ( hpdiff <= 50 ) 
        buf = " you are currently slightly healthier than $E.";
    if ( hpdiff <= 25 )
        buf = " you are a teensy bit healthier than $E.";
    if ( hpdiff <= 0 )
        buf = " $E is a teensy bit healthier than you.";
    if ( hpdiff <= -25 )
        buf = " $E is slightly healthier than you.";
    if ( hpdiff <= -50 )
        buf = " $E is healthier than you.";
    if ( hpdiff <= -100 )
        buf = " $E is much healthier than you.";

    act( buf, ch, NULL, victim, TO_CHAR );
    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf [ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch ) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    buf[0] = '\0';

    if ( isalpha( title[0] ) || isdigit( title[0] ) )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    if ( strlen( argument ) > 50 )
	argument[50] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}



void do_description( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
        return;

    string_append( ch, &ch->description );
    return;
}



void do_report( CHAR_DATA *ch, char *argument )
{
    char buf [ MAX_INPUT_LENGTH ];

    sprintf( buf,
	    "You report: %d/%d hp %d/%d mana %d/%d mv %d xp.\n\r",
	    ch->hit,  ch->max_hit,
	    ch->mana, ch->max_mana,
	    ch->move, ch->max_move,
	    ch->exp );

    send_to_char( buf, ch );

    sprintf( buf,
	    "$n reports: %d/%d hp %d/%d mana %d/%d mv %d xp.",
	    ch->hit,  ch->max_hit,
	    ch->mana, ch->max_mana,
	    ch->move, ch->max_move,
	    ch->exp );

    act( buf, ch, NULL, NULL, TO_ROOM );

    return;
}



void do_practice( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH   ];
    char buf1 [ MAX_STRING_LENGTH*2 ];
    int  sn;
    int  money = ch->level * ch->level * 20;

    if ( IS_NPC( ch ) )
	return;

    buf1[0] = '\0';

    if ( ch->level < 3 )
    {
	send_to_char(
	    "You must be third level to practice.  Go train instead!\n\r",
	    ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	CHAR_DATA *mob;
	int        col;

	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
	    if ( mob->deleted )
	        continue;
	    if ( IS_NPC( mob ) && IS_SET( mob->act, ACT_PRACTICE ) )
	        break;
	}

	col    = 0;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( !skill_table[sn].name )
		break;
	    if ( ch->level < skill_table[sn].skill_level[ch->class]
		|| skill_table[sn].skill_level[ch->class] > LEVEL_HERO )
		continue;

	    if ( ( mob ) || ( ch->pcdata->learned[sn] > 0 ) )
	    {
		sprintf( buf, "{o{b%18s {o{r%3d%%{x  ",
			skill_table[sn].name, ch->pcdata->learned[sn] );
		strcat( buf1, buf );
		if ( ++col % 3 == 0 )
		    strcat( buf1, "\n\r" );
	    }
	}

	if ( col % 3 != 0 )
	    strcat( buf1, "\n\r" );

	sprintf( buf, "{o{cYou have %d practice sessions left.{x\n\r",
		ch->practice );
	strcat( buf1, buf );
	sprintf( buf, "{o{cCost of practicing is %d gold coins.{x\n\r", money );
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
	    if ( IS_NPC( mob ) && IS_SET( mob->act, ACT_PRACTICE ) )
		break;
	}

	if ( !mob )
	{
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	if ( ch->practice <= 0 )
	{
	    send_to_char( "You have no practice sessions left.\n\r", ch );
	    return;
	}
	else if ( money > ch->gold )
	{
	    send_to_char( "You don't have enough money to practice.\n\r", ch );
	    return;
	}

	if ( ( sn = skill_lookup( argument ) ) < 0
	    || ( !IS_NPC( ch )
		&& ch->level < skill_table[sn].skill_level[ch->class] ) )
	{
	    send_to_char( "You can't practice that.\n\r", ch );
	    return;
	}

	adept = IS_NPC( ch ) ? 100 : class_table[ch->class]->skill_adept;

	if ( ch->pcdata->learned[sn] >= adept )
	{
	    sprintf( buf, "You are already an adept of %s.\n\r",
		skill_table[sn].name );
	    send_to_char( buf, ch );
	}
	else
	{
	    ch->practice--;
	    ch->gold                -= money;
	    ch->pcdata->learned[sn] += int_app[get_curr_int( ch )].learn;
	    if ( ch->pcdata->learned[sn] < adept )
	    {
		act( "You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n practices $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	    else
	    {
		ch->pcdata->learned[sn] = adept;
		act( "You are now an adept of $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n is now an adept of $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	}
    }
    return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char buf [ MAX_STRING_LENGTH ];
    char arg [ MAX_INPUT_LENGTH  ];
    int  wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if ( wimpy > ch->max_hit )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
    send_to_char( buf, ch );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    extern const char echo_off_str [ ];

    if ( IS_NPC( ch ) )
	return;

    write_to_buffer( ch->desc, "Password: ", 0 );
    write_to_buffer( ch->desc, echo_off_str, 0 );
    ch->desc->connected = CON_PASSWD_GET_OLD;
    return;
}



void do_socials( CHAR_DATA *ch, char *argument )
{
    SOC_INDEX_DATA *social;
    char            buf  [ MAX_STRING_LENGTH ];
    char            buf1 [ MAX_STRING_LENGTH ];
    int             col;
    int             x;
    

    buf1[0] = '\0';
    col = 0;

    for ( x = 0; x < MAX_WORD_HASH; x++ )
    {
	for ( social = soc_index_hash[x]; social; social = social->next )
	{
	    if ( !social->name || social->name[0] == '\0' )
		continue;

	sprintf( buf, "%-12s", social->name );
	strcat( buf1, buf );
	if ( ++col % 6 == 0 )
	    strcat( buf1, "\n\r" );
	}
    }
 
    if ( col % 6 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}



/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  cmd;
    int  col;

    buf1[0] = '\0';
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if (   cmd_table[cmd].level <  LEVEL_HERO
	    && cmd_table[cmd].level <= get_trust( ch )
	    && cmd_table[cmd].show )
	{
		sprintf( buf, "%-16s", cmd_table[cmd].name );
		strcat( buf1, buf );
		if ( ++col % 5 == 0 )
		    strcat( buf1, "\n\r" );
	}
    }

    if ( col % 5 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}



void do_channels( CHAR_DATA *ch, char *argument )
{
    char arg [ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
	{
	    send_to_char( "You are silenced.\n\r", ch );
	    return;
	}

	send_to_char( "Channels:", ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_AUCTION  )
		     ? " +AUCTION"
		     : " -auction",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_CHAT     )
		     ? " +CHAT"
		     : " -chat",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_MUSIC    )
		     ? " +MUSIC"
		     : " -music",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_GRATS    )
		     ? " +GRATS"
		     : " -grats",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_QUESTION )
		     ? " +QUESTION"
		     : " -question",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_SHOUT    )
		     ? " +SHOUT"
		     : " -shout",
		     ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_YELL     )
		     ? " +YELL"
		     : " -yell",
		     ch );

	if ( is_clan( ch ) )
	{
	    send_to_char( !IS_SET( ch->deaf, CHANNEL_CLANTALK )
			 ? " +CLANTALK"
			 : " -clantalk",
			 ch );
	}


	if ( IS_HERO( ch ) )
	{
	    send_to_char( "\n\r", ch );
	    send_to_char( !IS_SET( ch->deaf, CHANNEL_IMMTALK )
			 ? " +IMMTALK"
			 : " -immtalk",
			 ch );
	}

	send_to_char( ".\n\r", ch );
    }
    else
    {
	int  bit;
	bool fClear;

	     if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
	    send_to_char( "Channels -channel or +channel?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;
        else if ( !str_cmp( arg+1, "chat"     ) ) bit = CHANNEL_CHAT;
	else if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
	else if ( !str_cmp( arg+1, "clantalk" ) ) bit = CHANNEL_CLANTALK;
	else if ( !str_cmp( arg+1, "music"    ) ) bit = CHANNEL_MUSIC;
	else if ( !str_cmp( arg+1, "grats"    ) ) bit = CHANNEL_GRATS;
	else if ( !str_cmp( arg+1, "question" ) ) bit = CHANNEL_QUESTION;
	else if ( !str_cmp( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
	else if ( !str_cmp( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
	else
	{
	    send_to_char( "Set or clear which channel?\n\r", ch );
	    return;
	}

	if ( fClear )
	    REMOVE_BIT ( ch->deaf, bit );
	else
	    SET_BIT    ( ch->deaf, bit );

	send_to_char( "Ok.\n\r", ch );
    }

    return;
}



/*
 * Contributed by Grodyn.
 */
void do_config( CHAR_DATA *ch, char *argument )
{
    char arg [ MAX_INPUT_LENGTH ];

    if ( IS_NPC( ch ) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "[ Keyword  ] Option\n\r", ch );

	if ( IS_IMMORTAL( ch ) )
	send_to_char(  IS_SET( ch->act, PLR_EDIT_INFO  )
            ? "[+EDITINFO ] You see extra info on rooms.\n\r"
	    : "[-editinfo ] You don't see extra info on rooms.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_AUTOEXIT  )
            ? "[+AUTOEXIT ] You automatically see exits.\n\r"
	    : "[-autoexit ] You don't automatically see exits.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_AUTOLOOT  )
	    ? "[+AUTOLOOT ] You automatically loot corpses.\n\r"
	    : "[-autoloot ] You don't automatically loot corpses.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_AUTOGOLD  )
	    ? "[+AUTOGOLD ] You automatically get gold from corpses.\n\r"
	    : "[-autogold ] You don't automatically get gold from corpses.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_AUTOSAC   )
	    ? "[+AUTOSAC  ] You automatically sacrifice corpses.\n\r"
	    : "[-autosac  ] You don't automatically sacrifice corpses.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_BLANK     )
	    ? "[+BLANK    ] You have a blank line before your prompt.\n\r"
	    : "[-blank    ] You have no blank line before your prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_BRIEF     )
	    ? "[+BRIEF    ] You see brief descriptions.\n\r"
	    : "[-brief    ] You see long descriptions.\n\r"
	    , ch );
         
	send_to_char(  IS_SET( ch->act, PLR_COMBINE   )
	    ? "[+COMBINE  ] You see object lists in combined format.\n\r"
	    : "[-combine  ] You see object lists in single format.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_COLOUR    )
	    ? "[+COLOUR   ] You see ANSI colours.\n\r"
	    : "[-colour   ] You don't see ANSI colours.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_PAGER     )
	    ? "[+PAGER    ] You have a pager.\n\r"
	    : "[-pager    ] You don't have a pager.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_PROMPT    )
	    ? "[+PROMPT   ] You have a prompt.\n\r"
	    : "[-prompt   ] You don't have a prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_TELNET_GA )
	    ? "[+TELNETGA ] You receive a telnet GA sequence.\n\r"
	    : "[-telnetga ] You don't receive a telnet GA sequence.\n\r"
	    , ch );

	send_to_char(  IS_SET( ch->act, PLR_SILENCE   )
	    ? "[+SILENCE  ] You are silenced.\n\r"
	    : ""
	    , ch );

	send_to_char( !IS_SET( ch->act, PLR_NO_EMOTE  )
	    ? ""
	    : "[-emote    ] You can't emote.\n\r"
	    , ch );

	send_to_char( !IS_SET( ch->act, PLR_NO_TELL   )
	    ? ""
	    : "[-tell     ] You can't use 'tell'.\n\r"
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
	    send_to_char( "Config -option or +option?\n\r", ch );
	    return;
	}

             if ( !str_cmp( arg+1, "editinfo" )
		 && IS_IMMORTAL( ch ) ) bit = PLR_EDIT_INFO;
        else if ( !str_cmp( arg+1, "autoexit" ) ) bit = PLR_AUTOEXIT;
	else if ( !str_cmp( arg+1, "autoloot" ) ) bit = PLR_AUTOLOOT;
	else if ( !str_cmp( arg+1, "autogold" ) ) bit = PLR_AUTOGOLD;
	else if ( !str_cmp( arg+1, "autosac"  ) ) bit = PLR_AUTOSAC;
	else if ( !str_cmp( arg+1, "blank"    ) ) bit = PLR_BLANK;
	else if ( !str_cmp( arg+1, "brief"    ) ) bit = PLR_BRIEF;
	else if ( !str_cmp( arg+1, "combine"  ) ) bit = PLR_COMBINE;
	else if ( !str_cmp( arg+1, "colour"   ) ) bit = PLR_COLOUR;
        else if ( !str_cmp( arg+1, "pager"    ) ) bit = PLR_PAGER;
        else if ( !str_cmp( arg+1, "prompt"   ) ) bit = PLR_PROMPT;
	else if ( !str_cmp( arg+1, "telnetga" ) ) bit = PLR_TELNET_GA;
	else
	{
	    send_to_char( "Config which option?\n\r", ch );
	    return;
	}

	if ( fSet )
	{
	    SET_BIT    ( ch->act, bit );
	    sprintf( buf, "%s is now ON.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}
	else
	{
	    REMOVE_BIT ( ch->act, bit );
	    sprintf( buf, "%s is now OFF.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char( buf, ch );
	}

    }

    return;
}

void do_wizlist ( CHAR_DATA *ch, char *argument )
{

    do_help ( ch, "wizlist" );
    return;

}

void do_spells ( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  sn;
    int  col;

    if ( IS_NPC( ch )
	|| ( !IS_NPC( ch ) && !class_table[ch->class]->fMana ) )
    {  
       send_to_char ( "You don't need no stinking spells!\n\r", ch );
       return;
    }

    buf1[0] = '\0';

    col = 0;
    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
        if ( !skill_table[sn].name )
	   break;
	if ( ( ch->level < skill_table[sn].skill_level[ch->class] )
	    || ( skill_table[sn].skill_level[ch->class] > LEVEL_HERO ) )
	   continue;

	sprintf ( buf, "{o{b%18s {o{r%3dpts{x ",
           skill_table[sn].name, MANA_COST( ch, sn ) );
	strcat( buf1, buf );
	if ( ++col % 3 == 0 )
	   strcat( buf1, "\n\r" );
    }

    if ( col % 3 != 0 )
      strcat( buf1, "\n\r" );

    send_to_char ( buf1, ch );
    return;

}

void do_slist ( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  sn;
    int  col;
    int  level;
    bool pSpell;

    if ( IS_NPC( ch ) )
    {  
       send_to_char ( "You do not need any stinking spells!\n\r", ch );
       return;
    }

    buf1[0] = '\0';

    strcat ( buf1, "{mALL Abilities available for your class.{x\n\r\n\r" );
    strcat ( buf1, "{o{rLv          Abilities{x\n\r\n\r" );

    for ( level = 1; level <= LEVEL_HERO; level++ )
    {

      col = 0;
      pSpell = TRUE;

      for ( sn = 0; sn < MAX_SKILL; sn++ )
      {
	if ( !skill_table[sn].name )
	  break;
	if ( skill_table[sn].skill_level[ch->class] != level )
	  continue;

	if ( pSpell )
	{
	  sprintf ( buf, "{o{r%2d:{x", level );
	  strcat ( buf1, buf );
	  pSpell = FALSE;
	}

	/* format fix by Koala */ 
	if ( ++col % 4 == 0 )
	  strcat ( buf1, "   " );

	sprintf ( buf, "{o{b%18s{x", skill_table[sn].name );
	strcat ( buf1, buf );

	if ( col % 4 == 0 )
	  strcat ( buf1, "\n\r" );

      }

      if ( col % 4 != 0 )
	strcat ( buf1, "\n\r" );

    }

    send_to_char( buf1, ch );
    return;

}

/* bypassing the config command - Kahn */

void do_autoexit ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_AUTOEXIT )
     ? sprintf( buf, "-autoexit" )
     : sprintf( buf, "+autoexit" ) );

    do_config( ch, buf );

    return;

}

void do_autoloot ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_AUTOLOOT )
     ? sprintf( buf, "-autoloot" )
     : sprintf( buf, "+autoloot" ) );

    do_config( ch, buf );

    return;
}

void do_autogold ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_AUTOGOLD )
     ? sprintf( buf, "-autogold" )
     : sprintf( buf, "+autogold" ) );

    do_config( ch, buf );

    return;

}

void do_autosac ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_AUTOSAC )
     ? sprintf( buf, "-autosac" )
     : sprintf( buf, "+autosac" ) );

    do_config( ch, buf );

    return;

}

void do_blank ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_BLANK )
     ? sprintf( buf, "-blank" )
     : sprintf( buf, "+blank" ) );

    do_config( ch, buf );

    return;

}

void do_brief ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_BRIEF )
     ? sprintf( buf, "-brief" )
     : sprintf( buf, "+brief" ) ) ; 

    do_config( ch, buf );

    return;

}

void do_combine ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_COMBINE )
     ? sprintf( buf, "-combine" )
     : sprintf( buf, "+combine" ) );

    do_config( ch, buf );

    return;

}
 
void do_pagelen ( CHAR_DATA *ch, char *argument )
{
    char buf [ MAX_STRING_LENGTH ];
    char arg [ MAX_INPUT_LENGTH  ];
    int  lines;

    if ( IS_NPC( ch ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	lines = 20;
    else
	lines = atoi( arg );

    if ( lines < 19 )
    {
	send_to_char( "Less than 19 lines of paged text is not allowed", ch );
	return;
    }

    if ( lines > 60 )
    {
	send_to_char(
		"I don't know of a screen that is larger than 60 lines!\n\r",
		     ch );
	lines = 60;
    }

    ch->pcdata->pagelen = lines;
    sprintf( buf, "Page pause set to %d lines.\n\r", lines );
    send_to_char( buf, ch );
    return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_prompt( CHAR_DATA *ch, char *argument )
{
   char buf [ MAX_STRING_LENGTH ];

   buf[0] = '\0';

   /* Unswitched NPC's get kicked out */
   if ( !ch->desc )
       return;

   /* Will always have a pc ch after this */
   ch = ( ch->desc->original ? ch->desc->original : ch->desc->character );

   if ( argument[0] == '\0' )
   {
       ( IS_SET ( ch->act, PLR_PROMPT )
	? sprintf( buf, "-prompt" )
	: sprintf( buf, "+prompt" ) );

       do_config( ch, buf );

       return;
   }

   if( !strcmp( argument, "all" ) )
      strcat( buf, "<%hhp %mm %vmv> ");
   else
   {
      if ( strlen( argument ) > 50 )
	  argument[50] = '\0';
      smash_tilde( argument );
      strcat( buf, argument );
   }

   free_string( ch->pcdata->prompt );
   ch->pcdata->prompt = str_dup( buf );
   send_to_char( "Ok.\n\r", ch );
   return;
} 

void do_auto( CHAR_DATA *ch, char *argument )
{

    do_config( ch, "" );
    return;

}

void do_afk( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
        return;

    if ( IS_SET( ch->act, PLR_AFK ) )
    {
	REMOVE_BIT( ch->act, PLR_AFK );
	send_to_char( "You are back at your keyboard.\n\r", ch        );
	act( "$n has returned to $s keyboard.", ch, NULL, ch, TO_ROOM );
    }
    else
    {
	SET_BIT( ch->act, PLR_AFK );
	send_to_char( "You are now away from keyboard.\n\r", ch       );
	act( "$n has left $s keyboard.", ch, NULL, ch, TO_ROOM        );
    }
    
    return;
}
