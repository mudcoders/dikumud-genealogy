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
 * The following special functions are available for mobiles.
 */
DECLARE_MOB_FUN(	spec_breath_any		);
DECLARE_MOB_FUN(	spec_breath_acid	);
DECLARE_MOB_FUN(	spec_breath_fire	);
DECLARE_MOB_FUN(	spec_breath_frost	);
DECLARE_MOB_FUN(	spec_breath_gas		);
DECLARE_MOB_FUN(	spec_breath_lightning	);
DECLARE_MOB_FUN(	spec_cast_adept		);
DECLARE_MOB_FUN(	spec_cast_cleric	);
DECLARE_MOB_FUN(	spec_cast_ghost         );
DECLARE_MOB_FUN(	spec_cast_judge		);
DECLARE_MOB_FUN(	spec_cast_mage		);
DECLARE_MOB_FUN(	spec_cast_psionicist    );
DECLARE_MOB_FUN(	spec_cast_undead	);
DECLARE_MOB_FUN(	spec_executioner	);
DECLARE_MOB_FUN(	spec_fido		);
DECLARE_MOB_FUN(	spec_guard		);
DECLARE_MOB_FUN(	spec_janitor		);
DECLARE_MOB_FUN(	spec_mayor		);
DECLARE_MOB_FUN(	spec_poison		);
DECLARE_MOB_FUN(	spec_repairman		);
DECLARE_MOB_FUN(	spec_thief		);



MOB_FUN *spec_mob_lookup( const char *name )
{
    int cmd;

    for ( cmd = 0; *spec_mob_table[cmd].spec_name; cmd++ )
        if ( !str_cmp( name, spec_mob_table[cmd].spec_name ) )
            return spec_mob_table[cmd].spec_fun;

    return 0;
}


char *spec_mob_string( MOB_FUN *fun )
{
    int cmd;

    for ( cmd = 0; *spec_mob_table[cmd].spec_fun; cmd++ )
        if ( fun == spec_mob_table[cmd].spec_fun )
            return spec_mob_table[cmd].spec_name;

    return 0;
}

/*
 * Special function commands.
 */
const	struct	spec_mob_type	spec_mob_table	[ ]	=
{
    { "spec_breath_any",        spec_breath_any         },
    { "spec_breath_acid",       spec_breath_acid        },
    { "spec_breath_fire",       spec_breath_fire        },
    { "spec_breath_frost",      spec_breath_frost       },
    { "spec_breath_gas",        spec_breath_gas         },
    { "spec_breath_lightning",  spec_breath_lightning   },
    { "spec_cast_adept",        spec_cast_adept         },
    { "spec_cast_cleric",       spec_cast_cleric        },
    { "spec_cast_ghost",        spec_cast_ghost         },
    { "spec_cast_judge",        spec_cast_judge         },
    { "spec_cast_mage",         spec_cast_mage          },
    { "spec_cast_psionicist",   spec_cast_psionicist    },
    { "spec_cast_undead",       spec_cast_undead        },
    { "spec_executioner",       spec_executioner        },
    { "spec_fido",              spec_fido               },
    { "spec_guard",             spec_guard              },
    { "spec_janitor",           spec_janitor            },
    { "spec_mayor",             spec_mayor              },
    { "spec_poison",            spec_poison             },
    { "spec_repairman",         spec_repairman          },
    { "spec_thief",             spec_thief              },
    { "",                       0			}
};  



/*
 * If a spell casting mob is hating someone... try and summon them. -Toric (?)
 */
void summon_if_hating( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char       name [ MAX_INPUT_LENGTH  ];
    char       buf  [ MAX_STRING_LENGTH ];
    bool       found;

    if ( ch->fighting
	|| ch->fearing
	|| !ch->hating
	|| IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
	return;

    /* If player is close enough to hunt... don't summon. */
    if ( ch->hunting )
	return;

    one_argument( ch->hating->name, name );

    found = FALSE;

    /* Make sure the char exists; works even if player quits. */
    for ( victim = char_list; victim; victim = victim->next )
    {
	if ( !str_cmp( ch->hating->name, victim->name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
	return;

    if ( ch->in_room == victim->in_room )
	return;

    if ( !IS_NPC( victim ) )
	sprintf( buf, "summon 0.%s", name );
     else
	sprintf( buf, "summon %s",   name );

    do_cast( ch, buf );
    return;
}


/*
 * Core procedure for dragons.
 */
bool dragon( CHAR_DATA *ch, char *spell_name )
{
    CHAR_DATA *victim;
    int        sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if ( victim->deleted )
	    continue;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    if ( ( sn = skill_lookup( spell_name ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



/*
 * Special procedures for mobiles.
 */
bool spec_breath_any( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING )
	return FALSE;

    switch ( number_bits( 3 ) )
    {
    case 0: return spec_breath_fire		( ch );
    case 1:
    case 2: return spec_breath_lightning	( ch );
    case 3: return spec_breath_gas		( ch );
    case 4: return spec_breath_acid		( ch );
    case 5:
    case 6:
    case 7: return spec_breath_frost		( ch );
    }

    return FALSE;
}



bool spec_breath_acid( CHAR_DATA *ch )
{
    return dragon( ch, "acid breath" );
}



bool spec_breath_fire( CHAR_DATA *ch )
{
    return dragon( ch, "fire breath" );
}



bool spec_breath_frost( CHAR_DATA *ch )
{
    return dragon( ch, "frost breath" );
}



bool spec_breath_gas( CHAR_DATA *ch )
{
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( ( sn = skill_lookup( "gas breath" ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, NULL );
    return TRUE;
}



bool spec_breath_lightning( CHAR_DATA *ch )
{
    return dragon( ch, "lightning breath" );
}



bool spec_cast_adept( CHAR_DATA *ch )
{
    CHAR_DATA *victim;

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
        return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
	    break;
    }

    if ( !victim || IS_NPC( victim ) )
	return FALSE;

    if ( victim->level > 10 )
    {
        return FALSE;
    }

    switch ( number_bits( 3 ) )
    {
    case 0:
	act( "$n utters the word 'tehctah'.", ch, NULL, NULL, TO_ROOM );
	spell_armor( skill_lookup( "armor" ), ch->level, ch, victim );
	return TRUE;

    case 1:
	act( "$n utters the word 'nhak'.",    ch, NULL, NULL, TO_ROOM );
	spell_bless( skill_lookup( "bless" ), ch->level, ch, victim );
	return TRUE;

    case 2:
	act( "$n utters the word 'yeruf'.",   ch, NULL, NULL, TO_ROOM );
	spell_cure_blindness( skill_lookup( "cure blindness" ),
			     ch->level, ch, victim );
	return TRUE;

    case 3:
	act( "$n utters the word 'garf'.",    ch, NULL, NULL, TO_ROOM );
	spell_cure_light( skill_lookup( "cure light" ),
			 ch->level, ch, victim );
	return TRUE;

    case 4:
	act( "$n utters the words 'rozar'.",  ch, NULL, NULL, TO_ROOM );
	spell_cure_poison( skill_lookup( "cure poison" ),
			  ch->level, ch, victim );
	return TRUE;

    case 5:
	act( "$n utters the words 'nadroj'.", ch, NULL, NULL, TO_ROOM );
	spell_refresh( skill_lookup( "refresh" ), ch->level, ch, victim );
	return TRUE;

    case 6:
	act( "$n utters the words 'suinoleht'.", ch, NULL, NULL, TO_ROOM );
	spell_combat_mind( skill_lookup( "combat mind" ), ch->level, ch,
			  victim );
	return TRUE;
    }

    return FALSE;
}



bool spec_cast_cleric( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    summon_if_hating( ch );

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
        return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 2 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";      break;
	case  1: min_level =  3; spell = "cause serious";  break;
	case  2: min_level =  7; spell = "earthquake";     break;
	case  3: min_level =  9; spell = "cause critical"; break;
	case  4: min_level = 10; spell = "dispel evil";    break;
	case  5: min_level = 12; spell = "curse";          break;
	case  6: min_level = 12; spell = "change sex";     break;
	case  7: min_level = 13; spell = "flamestrike";    break;
	case  8:
	case  9: min_level = 14; spell = "mute";           break;
	case 10: min_level = 15; spell = "harm";           break;
	default: min_level = 16; spell = "dispel magic";   break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_judge( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
        return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 2 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    spell = "high explosive";
    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_mage( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    summon_if_hating( ch );

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
        return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 2 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";      break;
	case  1: min_level =  3; spell = "chill touch";    break;
	case  2: min_level =  7; spell = "weaken";         break;
	case  3: min_level =  8; spell = "teleport";       break;
	case  4: min_level = 11; spell = "colour spray";   break;
	case  5: min_level = 12; spell = "change sex";     break;
	case  6: min_level = 13; spell = "energy drain";   break;
	case  7:
	case  8:
	case  9: min_level = 15; spell = "fireball";       break;
	case 12: min_level = 16; spell = "polymorph other";break;
	case 13: min_level = 16; spell = "polymorph other";break;
	default: min_level = 20; spell = "acid blast";     break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_undead( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    summon_if_hating( ch );

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
        return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 2 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "curse";          break;
	case  1: min_level =  3; spell = "weaken";         break;
	case  2: min_level =  6; spell = "chill touch";    break;
	case  3: min_level =  9; spell = "blindness";      break;
	case  4: min_level = 12; spell = "poison";         break;
	case  5: min_level = 15; spell = "energy drain";   break;
	case  6: min_level = 18; spell = "harm";           break;
	case  7: min_level = 21; spell = "teleport";       break;
	case  8:
	case  9:
	case 10: if ( !str_cmp( race_table[ch->race].name, "Vampire" ) )
	         {
		     min_level = 24;
		     spell = "vampiric bite";  break;
		 }
	default: min_level = 24; spell = "gate";           break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_executioner( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char      *crime;
    char       buf [ MAX_STRING_LENGTH ];

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    crime = "";
    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if ( victim->deleted )
	    continue;

	if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER ) )
	    { crime = "KILLER"; break; }

	if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_THIEF  ) )
	    { crime = "THIEF"; break; }
    }

    if ( !victim )
	return FALSE;

    sprintf( buf, "%s is a %s!  PROTECT THE INNOCENT!  MORE BLOOOOD!!!",
	    victim->name, crime );
    do_shout( ch, buf );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    char_to_room( create_mobile( get_mob_index( MOB_VNUM_CITYGUARD ) ),
		 ch->in_room );
    char_to_room( create_mobile( get_mob_index( MOB_VNUM_CITYGUARD ) ),
		 ch->in_room );
    return TRUE;
}



bool spec_fido( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *corpse;
    OBJ_DATA *corpse_next;

    if ( !IS_AWAKE( ch ) )
	return FALSE;

    for ( corpse = ch->in_room->contents; corpse; corpse = corpse_next )
    {
        corpse_next = corpse->next_content;
        if ( corpse->deleted )
	    continue;
	if ( corpse->item_type != ITEM_CORPSE_NPC )
	    continue;

	act( "$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM );
	for ( obj = corpse->contains; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->deleted )
	        continue;
	    obj_from_obj( obj );
	    obj_to_room( obj, ch->in_room );
	}
	extract_obj( corpse );
	return TRUE;
    }

    return FALSE;
}



bool spec_guard( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *ech;
    char      *crime;
    char       buf [ MAX_STRING_LENGTH ];
    int        max_evil;

    if ( !IS_AWAKE( ch ) || ch->fighting )
	return FALSE;

    max_evil = 300;
    ech      = NULL;
    crime    = "";

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if ( victim->deleted )
	    continue;

	if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER ) )
	    { crime = "KILLER"; break; }

	if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_THIEF  ) )
	    { crime = "THIEF"; break; }

	if ( victim->fighting
	    && victim->fighting != ch
	    && victim->alignment < max_evil )
	{
	    max_evil = victim->alignment;
	    ech      = victim;
	}
    }

    if ( victim )
    {
	sprintf( buf, "%s is a %s!  PROTECT THE INNOCENT!!  BANZAI!!",
		victim->name, crime );
	do_shout( ch, buf );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return TRUE;
    }

    if ( ech )
    {
	act( "$n screams 'PROTECT THE INNOCENT!!  BANZAI!!",
	    ch, NULL, NULL, TO_ROOM );
	multi_hit( ch, ech, TYPE_UNDEFINED );
	return TRUE;
    }

    return FALSE;
}



bool spec_janitor( CHAR_DATA *ch )
{
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;

    if ( !IS_AWAKE( ch ) )
	return FALSE;

    for ( trash = ch->in_room->contents; trash; trash = trash_next )
    {
        trash_next = trash->next_content;
        if ( trash->deleted )
	    continue;
	if ( !IS_SET( trash->wear_flags, ITEM_TAKE ) )
	    continue;
	if (   trash->item_type == ITEM_DRINK_CON
	    || trash->item_type == ITEM_TRASH
	    || trash->cost < 10 )
	{
	    act( "$n picks up some trash.", ch, NULL, NULL, TO_ROOM );
	    obj_from_room( trash );
	    obj_to_char( trash, ch );
	    return TRUE;
	}
    }

    return FALSE;
}



bool spec_mayor( CHAR_DATA *ch )
{
    static const char *path;
    static const char  open_path  [ ] =
	"W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";
    static const char  close_path [ ] =
	"W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";
    static       int   pos;
    static       bool  move;

    if ( !move )
    {
	if ( time_info.hour ==  6 )
	{
	    path = open_path;
	    move = TRUE;
	    pos  = 0;
	}

	if ( time_info.hour == 20 )
	{
	    path = close_path;
	    move = TRUE;
	    pos  = 0;
	}
    }

    if ( ch->fighting )
	return spec_cast_cleric( ch );
    if ( !move || ch->position < POS_SLEEPING )
	return FALSE;

    switch ( path[pos] )
    {
    case '0':
    case '1':
    case '2':
    case '3':
	move_char( ch, path[pos] - '0' );
	break;

    case 'W':
	ch->position = POS_STANDING;
	act( "$n awakens and groans loudly.", ch, NULL, NULL, TO_ROOM );
	break;

    case 'S':
	ch->position = POS_SLEEPING;
	act( "$n lies down and falls asleep.", ch, NULL, NULL, TO_ROOM );
	break;

    case 'a':
	act( "$n says 'Hello Honey!'", ch, NULL, NULL, TO_ROOM );
	break;

    case 'b':
	act( "$n says 'What a view!  I must do something about that dump!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'c':
	act( "$n says 'Vandals!  Youngsters have no respect for anything!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'd':
	act( "$n says 'Good day, citizens!'", ch, NULL, NULL, TO_ROOM );
	break;

    case 'e':
	act( "$n says 'I hereby declare the city of Midgaard open!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'E':
	act( "$n says 'I hereby declare the city of Midgaard closed!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'O':
	do_unlock( ch, "gate" );
	do_open  ( ch, "gate" );
	break;

    case 'C':
	do_close ( ch, "gate" );
	do_lock  ( ch, "gate" );
	break;

    case '.' :
	move = FALSE;
	break;
    }

    pos++;
    return FALSE;
}



bool spec_poison( CHAR_DATA *ch )
{
    CHAR_DATA *victim;

    if ( ch->position != POS_FIGHTING
	|| !( victim = ch->fighting )
	|| number_percent( ) > 2 * ch->level )
	return FALSE;

    act( "You bite $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n bites you!", ch, NULL, victim, TO_VICT    );
    act( "$n bites $N!",  ch, NULL, victim, TO_NOTVICT );
    spell_poison( gsn_poison, ch->level, ch, victim );
    return TRUE;
}



bool spec_thief( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    int        gold;

    if ( ch->position != POS_STANDING )
	return FALSE;

    for ( victim = ch->in_room->people; victim;
	 victim = victim->next_in_room )
    {
	if ( IS_NPC( victim )
	    || victim->level >= LEVEL_IMMORTAL
	    || number_bits( 3 ) != 0
	    || !can_see( ch, victim ) )	/* Thx Glop */
	    continue;

	if ( IS_AWAKE( victim ) && victim->level > 5
	    && number_percent( ) + ch->level - victim->level >= 33 )
	{
	    act( "You discover $n's hands in your wallet!",
		ch, NULL, victim, TO_VICT );
	    act( "$N discovers $n's hands in $S wallet!",
		ch, NULL, victim, TO_NOTVICT );
	    return TRUE;
	}
	else
	{
	    gold = victim->gold * number_range( 1, 20 ) / 100;
	    ch->gold     += 3 * gold / 4;
	    victim->gold -= gold;
	    return TRUE;
	}
    }

    return FALSE;
}


/*
 * Psionicist spec_fun by Thelonius for EnvyMud.
 */
bool spec_cast_psionicist( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    summon_if_hating( ch );

    if ( ch->position != POS_FIGHTING )
        return FALSE;

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
        return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 2 ) == 0 )
            break;
    }

    if ( !victim )
        return FALSE;

    for ( ; ; )
    {
        int min_level;

        switch ( number_bits( 4 ) )
        {
        case  0: min_level =  0; spell = "mind thrust";          break;
        case  1: min_level =  4; spell = "psychic drain";        break;
        case  2: min_level =  6; spell = "agitation";            break;
        case  3: min_level =  8; spell = "psychic crush";        break;
        case  4: min_level =  9; spell = "project force";        break;
        case  5: min_level = 13; spell = "ego whip";             break;
        case  6: min_level = 14; spell = "energy drain";         break;
        case  7:
        case  8: min_level = 17; spell = "psionic blast";        break;
        case  9: min_level = 20; spell = "detonate";             break;
	case 10: min_level = 27; spell = "disintegrate";         break;
        default: min_level = 25; spell = "ultrablast";           break;
        }

        if ( ch->level >= min_level )
            break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
        return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_ghost( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    char      *spell;
    int        sn;

    summon_if_hating( ch );

    if  ( weather_info.sunlight != SUN_DARK )
    {

      if ( !ch->in_room )
      {
	  bug( "Spec_cast_ghost: NULL in_room.", 0 );
	  return FALSE;
      }

      if ( ch->fighting )
	  stop_fighting( ch, TRUE );

      act( "A beam of sunlight strikes $n, destroying $m.",
	  ch, NULL, NULL, TO_ROOM);

      extract_char ( ch, TRUE );
      return TRUE;

    }

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( IS_AFFECTED( ch, AFF_MUTE )
        || IS_SET( race_table[ch->race].race_abilities, RACE_MUTE )
        || IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
        return FALSE;

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if ( victim->deleted )
	    continue;
	if ( victim->fighting == ch && can_see( ch, victim )
	    && number_bits( 2 ) == 0 )
	    break;
    }

    if ( !victim )
	return FALSE;

    for ( ; ; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "curse";          break;
	case  1: min_level =  3; spell = "weaken";         break;
	case  2: min_level =  6; spell = "chill touch";    break;
	case  3: min_level =  9; spell = "blindness";      break;
	case  4: min_level = 12; spell = "poison";         break;
	case  5: min_level = 15; spell = "energy drain";   break;
	case  6: min_level = 18; spell = "harm";           break;
	case  7: min_level = 21; spell = "teleport";       break;
	default: min_level = 24; spell = "gate";           break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}


/*
 * spec_fun to repair bashed doors by Thelonius for EnvyMud.
 */
bool spec_repairman( CHAR_DATA *ch )
{
		 EXIT_DATA       *pexit;
		 EXIT_DATA       *pexit_rev;
                 ROOM_INDEX_DATA *to_room;
    extern const int              rev_dir [ ];
		 int              door;

    if ( !IS_AWAKE( ch ) )
	return FALSE;

    door = number_range( 0, 5 );
    /*
     *  Could search through all doors randomly, but deathtraps would 
     *  freeze the game!  And I'd prefer not to go through from 1 to 6...
     *  too boring.  Instead, just check one direction at a time.  There's
     *  a 51% chance they'll find the door within 4 tries anyway.
     *  -- Thelonius (Monk)
     */
    if ( !( pexit = ch->in_room->exit[door] ) )
	return FALSE;

    if ( IS_SET( pexit->exit_info, EX_BASHED ) )
    {
	REMOVE_BIT( pexit->exit_info, EX_BASHED );
	act( "You repair the $d.", ch, NULL, pexit->keyword, TO_CHAR );
	act( "$n repairs the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* Don't forget the other side! */
	if (   ( to_room   = pexit->to_room               )
	    && ( pexit_rev = to_room->exit[rev_dir[door]] )
	    && pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_BASHED );

	    for ( rch = to_room->people; rch; rch = rch->next_in_room )
		act( "The $d is set back on its hinges.",
		    rch, NULL, pexit_rev->keyword, TO_CHAR );
	}

	return TRUE;
    }

    return FALSE;
}
