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
/*  Blade Thirst was written by the Maniac from Mythran Mud                *
 *  Copyright (C) 1995-1996 The Maniac (a.k.a. Mark Janssen)               *
 ***************************************************************************/

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/* Poison weapon by Thelonius for EnvyMud */
/* Blade thirst code is a changed version of poison weapon */
/* Written by The Maniac. This skill came from the internet book */
/* The Tome of Mighty Magic */
void do_bladethirst( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *pobj;
    OBJ_DATA *wobj;
    AFFECT_DATA *paf;
    char      arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch ) 
	&& ch->level < skill_table[gsn_bladethirst].skill_level[ch->class] )
    {                                          
	send_to_char( "What do you think you are, a necromancer?\r\n", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )                                              
    { send_to_char( "What are you trying to do...?\r\n",    ch ); return; }
    if ( ch->fighting )                                       
    { send_to_char( "While you're fighting?  Nice try.\r\n", ch ); return; }
    if ( !( obj = get_obj_carry( ch, arg ) ) )
    { send_to_char( "You do not have that weapon.\r\n",      ch ); return; }
    if ( obj->item_type != ITEM_WEAPON )
    { send_to_char( "That item is not a weapon.\r\n",        ch ); return; }
    if ( IS_OBJ_STAT( obj, ITEM_BLADE_THIRST ) )
    { send_to_char( "That weapon is already thirsty.\r\n",  ch ); return; }

 /* Now we have a valid weapon...check to see if we have the bar of mithril. */
    for ( pobj = ch->carrying; pobj; pobj = pobj->next_content )
    {
	if ( pobj->pIndexData->vnum == OBJ_VNUM_MITHRIL )
	    break;
    }
    if ( !pobj )
    {
	send_to_char( "You do not have the mithril.\r\n", ch );
	return;
    }

    /* Okay, we have the mithril...do we have blood? */
    for ( wobj = ch->carrying; wobj; wobj = wobj->next_content )
    {
	if ( wobj->item_type == ITEM_DRINK_CON
	    && wobj->value[1]  >  0
	    && wobj->value[2]  == 13 )
	    break;
    }
    if ( !wobj )
    {
	send_to_char( "You need some blood for this skill.\r\n", ch );
	return;
    }

    /* Great, we have the ingredients...but is the ch smart enough? */
    if ( !IS_NPC( ch ) && get_curr_wis( ch ) < 17 )
    {
	send_to_char( "You can't quite remember what to do...\r\n", ch );
	return;
    }
    /* And does he have steady enough hands? */
    if ( !IS_NPC( ch )
	&& ( get_curr_dex( ch ) < 17
	    || ch->pcdata->condition[COND_DRUNK] > 0 ) )
    {
	send_to_char(
	"Your hands aren't steady enough to properly mix the ingredients.\r\n",
								ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_bladethirst].beats );

    /* Check the skill percentage */
    if ( !IS_NPC( ch )
	&& number_percent( ) > ch->pcdata->learned[gsn_bladethirst] )
    {
	send_to_char( "You failed and spill some on yourself.  Ouch!\r\n",
		     ch );
	damage( ch, ch, ch->level * 2, gsn_bladethirst, WEAR_NONE );
	act( "$n spills the blade thirst liquid all over!", ch, NULL, NULL, TO_ROOM );
	extract_obj( pobj );
	extract_obj( wobj );
	return;
    }

    /* Well, I'm tired of waiting.  Are you? */
    act( "You mix $p in $P, creating an evil looking potion!",
	ch, pobj, wobj, TO_CHAR );
    act( "$n mixes $p in $P, creating an evil looking potion!",
	ch, pobj, wobj, TO_ROOM );
    act( "You pour the potion over $p, which glistens wickedly!",
	ch, obj, NULL, TO_CHAR  );
    act( "$n pours the potion over $p, which glistens wickedly!",
	ch, obj, NULL, TO_ROOM  );
    SET_BIT( obj->extra_flags, ITEM_BLADE_THIRST );
    obj->cost *= ch->level;

    /* Set an object timer.  Dont want proliferation of thirsty weapons */
    obj->timer = 10 + ch->level;

    if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
	obj->timer *= 2;

    if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
	obj->timer *= 2;

    /* WHAT?  All of that, just for that one bit?  How lame. ;) */
    act( "The remainder of the potion eats through $p.",
	ch, wobj, NULL, TO_CHAR );
    act( "The remainder of the potion eats through $p.",
	ch, wobj, NULL, TO_ROOM );
    extract_obj( pobj );
    extract_obj( wobj );

    if ( !affect_free )
    {
        paf             = alloc_perm( sizeof( *paf ) );
    }
    else
    {
        paf             = affect_free;
        affect_free     = affect_free->next;
    }
 
    paf->type           = -1;
    paf->duration       = -1;
    paf->location       = APPLY_HITROLL;
    paf->modifier       = 3;
    paf->bitvector      = 0;
    paf->next           = obj->affected;
    obj->affected       = paf;

    return;
}
