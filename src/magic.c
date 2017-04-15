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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
void	say_spell	args( ( CHAR_DATA *ch, int sn ) );



/*
 * Lookup a skill by name.
 */
int skill_lookup( const char *name )
{
    int sn;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( !skill_table[sn].name )
	    break;
	if ( LOWER( name[0] ) == LOWER( skill_table[sn].name[0] )
	    && !str_prefix( name, skill_table[sn].name ) )
	    return sn;
    }

    return -1;
}



/*
 * Utter mystical words for an sn.
 */
void say_spell( CHAR_DATA *ch, int sn )
{
                        CHAR_DATA *rch;
			char      *pName;
			char       buf       [ MAX_STRING_LENGTH ];
			char       buf2      [ MAX_STRING_LENGTH ];
			int        iSyl;
			int        length;

	       	 struct syl_type
	         {
		        char *	   old;
		        char *	   new;
		 };

    static const struct syl_type   syl_table [ ] =
    {
	{ " ",		" "		},
	{ "ar",		"abra"		},
	{ "au",		"kada"		},
	{ "bless",	"fido"		},
	{ "blind",	"nose"		},
	{ "bur",	"mosa"		},
	{ "cu",		"judi"		},
	{ "de",		"oculo"		},
	{ "en",		"unso"		},
	{ "light",	"dies"		},
	{ "lo",		"hi"		},
	{ "mor",	"zak"		},
	{ "move",	"sido"		},
	{ "ness",	"lacri"		},
	{ "ning",	"illa"		},
	{ "per",	"duda"		},
	{ "ra",		"gru"		},
	{ "re",		"candus"	},
	{ "son",	"sabru"		},
	{ "tect",	"infra"		},
	{ "tri",	"cula"		},
	{ "ven",	"nofo"		},
	{ "a", "a" }, { "b", "b" }, { "c", "q" }, { "d", "e" },
	{ "e", "z" }, { "f", "y" }, { "g", "o" }, { "h", "p" },
	{ "i", "u" }, { "j", "y" }, { "k", "t" }, { "l", "r" },
	{ "m", "w" }, { "n", "i" }, { "o", "a" }, { "p", "s" },
	{ "q", "d" }, { "r", "f" }, { "s", "g" }, { "t", "h" },
	{ "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
	{ "y", "l" }, { "z", "k" },
	{ "", "" }
    };

    buf[0]	= '\0';
    for ( pName = skill_table[sn].name; *pName != '\0'; pName += length )
    {
	for ( iSyl = 0;
	     ( length = strlen( syl_table[iSyl].old ) ) != 0;
	     iSyl++ )
	{
	    if ( !str_prefix( syl_table[iSyl].old, pName ) )
	    {
		strcat( buf, syl_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    sprintf( buf2, "$n utters the words, '%s'.", buf );
    sprintf( buf,  "$n utters the words, '%s'.", skill_table[sn].name );

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( rch != ch )
	    act( ch->class==rch->class ? buf : buf2, ch, NULL, rch, TO_VICT );
    }

    return;
}



/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */
bool saves_spell( int level, CHAR_DATA *victim )
{
    int save;
    int base = 50;

    if ( IS_NPC( victim ) )
        base += 25;
    save = base + ( victim->level - level - victim->saving_throw ) * 5;
    save = URANGE( 5, save, 95 );
    return number_percent( ) < save;
}



/*
 * The kludgy global is for spells who want more stuff from command line.
 */
char *target_name;

void do_cast( CHAR_DATA *ch, char *argument )
{
    void      *vo;
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    int        mana;
    int        sn;

    target_name = one_argument( argument, arg1 );
    one_argument( target_name, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Cast which what where?\n\r", ch );
	return;
    }

    if ( IS_NPC( ch ) )
        return;

    if ( ( sn = skill_lookup( arg1 ) ) < 0
	|| ch->level < skill_table[sn].skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }
  
    if ( ch->position < skill_table[sn].minimum_position )
    {
	send_to_char( "You can't concentrate enough.\n\r", ch );
	return;
    }

    if (   IS_AFFECTED( ch, AFF_MUTE )
	|| IS_SET( race_table[ch->race].race_abilities, RACE_MUTE ) )
    {
	send_to_char( "Your lips move but no sound comes out.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
	send_to_char( "You can't...you are in a Cone of Silence!\n\r", ch );
	return;
    }

    mana = MANA_COST( ch, sn );

    /*
     * Locate targets.
     */
    victim	= NULL;
    vo		= NULL;
      
    switch ( skill_table[sn].target )
    {
    default:
	bug( "Do_cast: bad target for sn %d.", sn );
	return;

    case TAR_IGNORE:
	break;

    case TAR_CHAR_OFFENSIVE:
	if ( arg2[0] == '\0' )
	{
	    if ( !( victim = ch->fighting ) )
	    {
		send_to_char( "Cast the spell on whom?\n\r", ch );
		return;
	    }
	}
	else
	{
	    if ( !( victim = get_char_room( ch, arg2 ) ) )
	    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
	    }
	}

	if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
        {
	    send_to_char( "You can't do that on your own follower.\n\r",
			 ch );
	    return;
	}

	if ( is_safe( ch, victim ) )
	    return;

	check_killer( ch, victim );

	vo = (void *) victim;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( arg2[0] == '\0' )
	{
	    victim = ch;
	}
	else
	{
	    if ( !( victim = get_char_room( ch, arg2 ) ) )
	    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
	    }
	}

	vo = (void *) victim;
	break;

    case TAR_CHAR_SELF:
	if ( arg2[0] != '\0' && !is_name( arg2, ch->name ) )
	{
	    send_to_char( "You cannot cast this spell on another.\n\r", ch );
	    return;
	}

	vo = (void *) ch;
	break;

    case TAR_OBJ_INV:
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "What should the spell be cast upon?\n\r", ch );
	    return;
	}

	if ( !( obj = get_obj_carry( ch, arg2 ) ) )
	{
	    send_to_char( "You are not carrying that.\n\r", ch );
	    return;
	}

	vo = (void *) obj;
	break;
    }
	    
    if ( ch->mana < mana )
    {
	send_to_char( "You don't have enough mana.\n\r", ch );
	return;
    }
      
    if ( str_cmp( skill_table[sn].name, "ventriloquate" ) )
	say_spell( ch, sn );
      
    WAIT_STATE( ch, skill_table[sn].beats );
      
    if ( number_percent( ) > ch->pcdata->learned[sn] )
    {
	send_to_char( "You lost your concentration.\n\r", ch );
	ch->mana -= mana / 2;
    }
    else
    {
	ch->mana -= mana;
	(*skill_table[sn].spell_fun) ( sn,
				      URANGE( 1, ch->level, LEVEL_HERO ),
				      ch, vo );
    }

    if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
	&& victim->master != ch && victim != ch && IS_AWAKE( victim ) )
    {
	CHAR_DATA *vch;

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted )
	        continue;
	    if ( victim == vch && !victim->fighting )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	}
    }

    return;
}



/*
 * Cast spells at targets using a magical object.
 */
void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim,
		    OBJ_DATA *obj )
{
    void *vo;

    if ( sn <= 0 )
	return;

    if ( sn >= MAX_SKILL || skill_table[sn].spell_fun == 0 )
    {
	bug( "Obj_cast_spell: bad sn %d.", sn );
	return;
    }

    switch ( skill_table[sn].target )
    {
    default:
	bug( "Obj_cast_spell: bad target for sn %d.", sn );
	return;

    case TAR_IGNORE:
	vo = NULL;
	break;

    case TAR_CHAR_OFFENSIVE:
	if ( !victim )
	    victim = ch->fighting;
	if ( !victim )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}

	if ( is_safe( ch, victim ) )
	    return;

	check_killer( ch, victim );

	vo = (void *) victim;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( !victim )
	    victim = ch;
	vo = (void *) victim;
	break;

    case TAR_CHAR_SELF:
	vo = (void *) ch;
	break;

    case TAR_OBJ_INV:
	if ( !obj )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	vo = (void *) obj;
	break;
    }

    target_name = "";
    (*skill_table[sn].spell_fun) ( sn, level, ch, vo );

    if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
	&& victim->master != ch && ch != victim )
    {
	CHAR_DATA *vch;

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted )
	        continue;
	    if ( victim == vch && !victim->fighting )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	}
    }

    return;
}



/*
 * Spell functions.
 */
void spell_acid_blast( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = dice( level, 8 );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_armor( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.duration  = 24;
    af.location  = APPLY_AC;
    af.modifier  = -20;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel someone protecting you.\n\r", victim );
    return;
}



void spell_bless( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.duration  = 6 + level;
    af.location  = APPLY_HITROLL;
    af.modifier  = level / 8;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 0 - level / 8;
    affect_to_char( victim, &af );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel righteous.\n\r", victim );
    return;
}



void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_BLIND ) || saves_spell( level, victim ) )
    {
	send_to_char( "You have failed.\n\r", ch );
	return;
    }

    af.type      = sn;
    af.duration  = 1 + level;
    af.location  = APPLY_HITROLL;
    af.modifier  = -4;
    af.bitvector = AFF_BLIND;
    affect_to_char( victim, &af );

    act( "$N is blinded!", ch, NULL, victim, TO_CHAR    );
    send_to_char( "You are blinded!\n\r", victim );
    act( "$N is blinded!", ch, NULL, victim, TO_NOTVICT );
    return;
}



void spell_breathe_water( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_GILLS ) )
	return;

    af.type      = sn;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_GILLS;
    affect_to_char( victim, &af );

    send_to_char( "You can now breathe underwater.\n\r", victim );
    act( "$n can now breathe underwater.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_burning_hands( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] = 
    {
	 0,
	 0,  0,  0,  0,	14,	17, 20, 23, 26, 29,
	29, 29, 30, 30,	31,	31, 32, 32, 33, 33,
	34, 34, 35, 35,	36,	36, 37, 37, 38, 38,
	39, 39, 40, 40,	41,	41, 42, 42, 43, 43,
	44, 44, 45, 45,	46,	46, 47, 47, 48, 48
    };
                 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_call_lightning( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    int        dam;

    if ( !IS_OUTSIDE( ch ) )
    {
	send_to_char( "You must be out of doors.\n\r", ch );
	return;
    }

    if ( weather_info.sky < SKY_RAINING )
    {
	send_to_char( "You need bad weather.\n\r", ch );
	return;
    }

    dam = dice( level / 2, 8 );

    send_to_char( "God's lightning strikes your foes!\n\r", ch );
    act( "$n calls God's lightning to strike $s foes!",
	ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch; vch = vch->next )
    {
        if ( vch->deleted || !vch->in_room )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch
		&& ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
                damage( ch, vch, saves_spell( level, vch ) ? dam/2 : dam,
                       sn, WEAR_NONE );
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area
	    && IS_OUTSIDE( vch )
	    && IS_AWAKE( vch ) )
	    send_to_char( "Lightning flashes in the sky.\n\r", vch );
    }

    return;
}



void spell_cause_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    damage( ch, (CHAR_DATA *) vo, dice( 1, 8 ) + level / 3, sn, WEAR_NONE );
    return;
}



void spell_cause_critical( int sn, int level, CHAR_DATA *ch, void *vo )
{
    damage( ch, (CHAR_DATA *) vo, dice( 3, 8 ) + level - 6, sn, WEAR_NONE );
    return;
}



void spell_cause_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
    damage( ch, (CHAR_DATA *) vo, dice( 2, 8 ) + level / 2, sn, WEAR_NONE );
    return;
}



void spell_change_sex( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_CHANGE_SEX ) )
    {
	act( "$E is already changed.", ch, NULL, victim, TO_CHAR );
	return;
    }

    af.type      = sn;
    af.duration  = 10 * level;
    af.location  = APPLY_SEX;
    do
    {
	af.modifier  = number_range( 0, 2 ) - victim->sex;
    }
    while ( af.modifier == 0 );
    af.bitvector = AFF_CHANGE_SEX;
    affect_to_char( victim, &af );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel different.\n\r", victim );
    return;
}



void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    char        buf[ MAX_STRING_LENGTH ];

    if ( victim == ch )
    {
	send_to_char( "You like yourself even better!\n\r", ch );
	return;
    }

    if ( get_trust( ch ) > LEVEL_HERO && get_trust( ch ) < L_SEN )
    {
	send_to_char( "There is no need for you to have a charmed mobile\n\r",
		     ch );
	sprintf( buf, "%s charming", ch->name );
	log_string( buf );
	return;
    }

    if (   IS_AFFECTED( victim, AFF_CHARM )
	|| IS_AFFECTED( ch,     AFF_CHARM )
	|| level < victim->level
	|| saves_spell( level, victim ) )
	return;

    if ( victim->master )
	stop_follower( victim );
    add_follower( victim, ch );
    af.type      = sn;
    af.duration  = number_fuzzy( level / 4 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );

    send_to_char( "Ok.\n\r", ch );
    act( "Isn't $n just so nice?", ch, NULL, victim, TO_VICT );
    return;
}



void spell_chill_touch( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA  *victim       = (CHAR_DATA *) vo;
		 AFFECT_DATA af;
    static const int         dam_each [ ] = 
    {
	 0,
	 0,  0,  6,  7,  8,	 9, 12, 13, 13, 13,
	14, 14, 14, 15, 15,	15, 16, 16, 16, 17,
	17, 17, 18, 18, 18,	19, 19, 19, 20, 20,
	20, 21, 21, 21, 22,	22, 22, 23, 23, 23,
	24, 24, 24, 25, 25,	25, 26, 26, 26, 27
    };
		 int         dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( !saves_spell( level, victim ) )
    {
	af.type      = sn;
	af.duration  = 6;
	af.location  = APPLY_STR;
	af.modifier  = -1;
	af.bitvector = 0;
	affect_join( victim, &af );
    }
    else
    {
	dam /= 2;
    }
    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_colour_spray( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] = 
    {
	 0,
	 0,  0,  0,  0,  0,	 0,  0,  0,  0,  0,
	30, 35, 40, 45, 50,	55, 55, 55, 56, 57,
	58, 58, 59, 60, 61,	61, 62, 63, 64, 64,
	65, 66, 67, 67, 68,	69, 70, 70, 71, 72,
	73, 73, 74, 75, 76,	76, 77, 78, 79, 79
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_cone_of_silence( int sn, int level, CHAR_DATA *ch, void *vo )
{
    ROOM_INDEX_DATA *pRoomIndex;

    if ( !( pRoomIndex = ch->in_room ) )
        return;

    if ( IS_SET( pRoomIndex->room_flags, ROOM_SAFE ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( !IS_SET( pRoomIndex->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
	SET_BIT( pRoomIndex->room_flags, ROOM_CONE_OF_SILENCE );
	send_to_char( "You have created a cone of silence!\n\r", ch );
	act( "$n has created a cone of silence!", ch, NULL, NULL, TO_ROOM );
    }

    return;
}


void spell_continual_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *light;

    light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), 0 );
    obj_to_room( light, ch->in_room );

    act( "You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR );
    act( "$n twiddles $s thumbs and $p appears.",   ch, light, NULL, TO_ROOM );
    return;
}



void spell_control_weather( int sn, int level, CHAR_DATA *ch, void *vo )
{
    if ( !str_cmp( target_name, "better" ) )
	weather_info.change += dice( level / 3, 4 );
    else if ( !str_cmp( target_name, "worse" ) )
	weather_info.change -= dice( level / 3, 4 );
    else
	send_to_char ("Do you want it to get better or worse?\n\r", ch );

    send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_create_food( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *mushroom;

    mushroom = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), 0 );
    mushroom->value[0] = 5 + level;
    obj_to_room( mushroom, ch->in_room );

    act( "$p suddenly appears.", ch, mushroom, NULL, TO_CHAR );
    act( "$p suddenly appears.", ch, mushroom, NULL, TO_ROOM );
    return;
}



void spell_create_spring( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *spring;

    spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), 0 );
    spring->timer = level;
    obj_to_room( spring, ch->in_room );

    act( "$p flows from the ground.", ch, spring, NULL, TO_CHAR );
    act( "$p flows from the ground.", ch, spring, NULL, TO_ROOM );
    return;
}



void spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj   = (OBJ_DATA *) vo;
    int       water;

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "It is unable to hold water.\n\r", ch );
	return;
    }

    if ( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
    {
	send_to_char( "It contains some other liquid.\n\r", ch );
	return;
    }

    water = UMIN( level * ( weather_info.sky >= SKY_RAINING ? 4 : 2 ),
		 obj->value[0] - obj->value[1] );
  
    if ( water > 0 )
    {
	obj->value[2] = LIQ_WATER;
	obj->value[1] += water;
	if ( !is_name( "water", obj->name ) )
	{
	    char buf [ MAX_STRING_LENGTH ];

	    sprintf( buf, "%s water", obj->name );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	}
	act( "$p is filled.", ch, obj, NULL, TO_CHAR );
    }

    return;
}



void spell_cure_blindness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !is_affected( victim, gsn_blindness ) )
	return;

    affect_strip( victim, gsn_blindness );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "Your vision returns!\n\r", victim );
    return;
}



void spell_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    heal = dice( 3, 8 ) + level - 6;
    victim->hit = UMIN( victim->hit + heal, victim->max_hit );
    update_pos( victim );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel better!\n\r", victim );
    return;
}



void spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    heal = dice( 1, 8 ) + level / 3;
    victim->hit = UMIN( victim->hit + heal, victim->max_hit );
    update_pos( victim );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel better!\n\r", victim );
    return;
}



void spell_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !is_affected( victim, gsn_poison ) )
        return;

    affect_strip( victim, gsn_poison );

    send_to_char( "Ok.\n\r",                                    ch     );
    send_to_char( "A warm feeling runs through your body.\n\r", victim );
    act( "$N looks better.", ch, NULL, victim, TO_NOTVICT );

    return;
}



void spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    heal = dice( 2, 8 ) + level / 2 ;
    victim->hit = UMIN( victim->hit + heal, victim->max_hit );
    update_pos( victim );
    send_to_char( "You feel better!\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_CURSE ) || saves_spell( level, victim ) )
    {
	send_to_char( "You have failed.\n\r", ch );
	return;
    }

    af.type      = sn;
    af.duration  = 2 * level;
    af.location  = APPLY_HITROLL;
    af.modifier  = -1;
    af.bitvector = AFF_CURSE;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 1;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel unclean.\n\r", victim );
    return;
}


void spell_destroy_cursed( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        yesno  = 0;

    for ( obj = victim->carrying; obj; obj = obj_next )
    {
        obj_next = obj->next_content;

	if ( IS_SET( obj->extra_flags, ITEM_NODROP )
	    && obj->wear_loc == WEAR_NONE )
	{
    act( "You convulse as you toss $p to the ground, destroying it.",
		victim, obj, NULL, TO_CHAR );
    act( "$n convulses as $e tosses $p to the ground, destroying it.",
		victim, obj, NULL, TO_ROOM );
	    extract_obj( obj );
	    yesno = 1;
	}
    }

    if ( ch != victim && yesno )
        send_to_char( "Ok.\n\r", ch );
    return;
}


void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_DETECT_EVIL ) )
	return;

    af.type      = sn;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_EVIL;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "Your eyes tingle.\n\r", victim );
    return;
}



void spell_detect_hidden( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_DETECT_HIDDEN ) )
	return;

    af.type      = sn;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "Your awareness improves.\n\r", victim );
    return;
}



void spell_detect_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_DETECT_INVIS ) )
	return;

    af.type      = sn;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_INVIS;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "Your eyes tingle.\n\r", victim );
    return;
}



void spell_detect_magic( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_DETECT_MAGIC ) )
	return;

    af.type      = sn;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_MAGIC;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "Your eyes tingle.\n\r", victim );
    return;
}



void spell_detect_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
    {
	if ( obj->value[3] != 0 )
	    send_to_char( "You smell poisonous fumes.\n\r", ch );
	else
	    send_to_char( "It looks very delicious.\n\r", ch );
    }
    else
    {
	send_to_char( "It doesn't look poisoned.\n\r", ch );
    }

    return;
}


void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
  
    if ( !IS_NPC( ch ) && IS_EVIL( ch ) )
    {
	send_to_char( "You are too EVIL to cast this.\n\r", ch );
	return;
    }
  
    if ( IS_GOOD( victim ) )
    {
	act( "God protects $N.", ch, NULL, victim, TO_ROOM );
	act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( IS_NEUTRAL( victim ) )
    {
	act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
	return;
    }

    dam = dice( level, 4 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



/* Dispel Magic recoded by Thelonius for EnvyMud */
void spell_dispel_magic ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA   *victim = (CHAR_DATA *) vo;
    AFFECT_DATA *paf;

    if ( !IS_NPC( ch )
	&& ( !IS_NPC( victim ) 
	    || ( IS_AFFECTED( victim, AFF_CHARM )
		&&   is_same_group( ch, victim->master ) ) ) )
      /* Defensive spell - remove ALL effects */
    {
	for ( paf = victim->affected; paf; paf = paf->next )
	{
	    if ( paf->deleted )
	        continue;
	    if ( paf->type == skill_lookup( "polymorph other" )
		&& get_trust( ch ) < LEVEL_IMMORTAL )
	    {
		send_to_char( "You were unable to remove Polymorph other.\n\r",
			     ch );
	        continue;
	    }
	    if ( paf->type == gsn_vampiric_bite 
		&& get_trust( ch ) < LEVEL_IMMORTAL )
	    {
		send_to_char(
			    "You were unable to remove the Vampire curse.\n\r",
			     ch );
		continue;
	    }
	    affect_remove( victim, paf );
	}

	if ( victim == ch )
	{
	    act( "You have removed all magic effects from yourself.",
			ch, NULL, NULL, TO_CHAR );
	    act( "$n has removed all magic effects from $mself.",
			ch, NULL, NULL, TO_ROOM );
	}
	else
	{
	    act( "You have removed all magic effects from $N.",
			ch, NULL, victim, TO_CHAR );
	    act( "$n has removed all magic effects from you.",
			ch, NULL, victim, TO_VICT );
	    act( "$n has removed all magic effects from $N.",
			ch, NULL, victim, TO_NOTVICT );
	}
	return;
    }
    else
      /* Offensive spell - enforced by multi_hit whether succeeds or fails */
    {
	for ( paf = victim->affected; paf; paf = paf->next )
	{
	    if ( paf->deleted )
	        continue;
	    if ( !saves_spell( level, victim ) )
	    {
		send_to_char( skill_table[paf->type].msg_off, victim );
		send_to_char( "\n\r", victim );
		act( "$n is no longer affected by '$t'.",
			victim, skill_table[paf->type].name, NULL, TO_ROOM );
		affect_strip( victim, paf->type );
		break;
	    }
	}
	/* ALWAYS give a shot at removing sanctuary */
	if ( IS_AFFECTED( victim, AFF_SANCTUARY)
	    && !saves_spell( level, victim ) )
	{
	    REMOVE_BIT( victim->affected_by, AFF_SANCTUARY );
	    send_to_char( "The white aura around your body fades.\n\r",
								victim );
	    act( "The white aura around $n's body fades.",
			victim, NULL, NULL, TO_ROOM );
	}
	if ( !victim->fighting || !is_same_group( ch, victim->fighting ) )
	  multi_hit( victim, ch, TYPE_UNDEFINED );
    }
    return;
}

void spell_earthquake( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;

    send_to_char( "The earth trembles beneath your feet!\n\r", ch );
    act( "$n makes the earth tremble and shiver.", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch; vch = vch->next )
    {
        if ( vch->deleted || !vch->in_room )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch )
			                     :  IS_NPC( vch ) ) )
		damage( ch, vch, level + dice( 2, 8 ), sn, WEAR_NONE );
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char( "The earth trembles and shivers.\n\r", vch );
    }

    return;
}



void spell_enchant_weapon( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    if ( obj->item_type != ITEM_WEAPON
	|| IS_OBJ_STAT( obj, ITEM_MAGIC )
	|| obj->affected )
    {
	send_to_char( "That item cannot be enchanted.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
        remove_obj( ch, obj->wear_loc, TRUE );

    /* Thanks to Magoo for this little insight */
    if ( obj->level > get_trust( ch ) )
    {
	send_to_char( "You cannot enchant a weapon higher then yourself.\n\r",
		     ch);
	return;
    }

    paf			= new_affect();

    paf->type		= sn;
    paf->duration	= -1;
    paf->location	= APPLY_HITROLL;
    paf->modifier	= level / 5;
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;

    paf			= new_affect();

    paf->type		= sn;
    paf->duration	= -1;
    paf->location	= APPLY_DAMROLL;
    paf->modifier	= level / 10;
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;
    obj->level		= number_fuzzy( ch->level - 5 );

    if ( IS_GOOD( ch ) )
    {
	SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL);
	act( "$p glows blue.",   ch, obj, NULL, TO_CHAR );
    }
    else if ( IS_EVIL( ch ) )
    {
	SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
	act( "$p glows red.",    ch, obj, NULL, TO_CHAR );
    }
    else
    {
	SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
	SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
	act( "$p glows yellow.", ch, obj, NULL, TO_CHAR );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}



/*
 * Drain XP, MANA, HP.
 * Caster gains HP.
 */
void spell_energy_drain( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    if ( saves_spell( level, victim ) )
    {
	send_to_char( "Nothing seems to happen.\n\r", ch );
	return;
    }

    ch->alignment = UMAX(-1000, ch->alignment - 200);
    if ( victim->level <= 2 )
    {
	dam		 = ch->hit + 1;
    }
    else
    {
	gain_exp( victim, 0 - number_range( level / 2, 3 * level / 2 ) );
	victim->mana	/= 2;
	victim->move	/= 2;
	dam		 = dice( 1, level );
	ch->hit		+= dam;
    }

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_fireball( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] = 
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,  30,	 35,  40,  45,  50,  55,
	 60,  65,  70,  75,  80,	 82,  84,  86,  88,  90,
	 92,  94,  96,  98, 100,	102, 104, 106, 108, 110,
	112, 114, 116, 118, 120,	122, 124, 126, 128, 130
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = dice( 6, level );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_faerie_fire( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int         effect;

    if ( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
	return;

    effect       = UMIN( 2 * level, 50 );
    if ( saves_spell( level, victim ) )
        effect  /= 2;

    af.type      = sn;
    af.duration  = level;
    af.location  = APPLY_AC;
    af.modifier  = effect;
    af.bitvector = AFF_FAERIE_FIRE;

    affect_to_char( victim, &af );

    send_to_char( "You are surrounded by a pink outline.\n\r", victim );
    act( "$n is surrounded by a pink outline.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_faerie_fog( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *ich;

    send_to_char( "You conjure a cloud of purple smoke.\n\r", ch );
    act( "$n conjures a cloud of purple smoke.", ch, NULL, NULL, TO_ROOM );

    for ( ich = ch->in_room->people; ich; ich = ich->next_in_room )
    {
	if ( !IS_NPC( ich ) && IS_SET( ich->act, PLR_WIZINVIS ) )
	    continue;

	if ( ich == ch || saves_spell( level, ich ) )
	    continue;

	affect_strip ( ich, gsn_invis			);
	affect_strip ( ich, gsn_mass_invis		);
	affect_strip ( ich, gsn_sneak			);
	affect_strip ( ich, gsn_shadow			);
	REMOVE_BIT   ( ich->affected_by, AFF_HIDE	);
	REMOVE_BIT   ( ich->affected_by, AFF_INVISIBLE	);
	REMOVE_BIT   ( ich->affected_by, AFF_SNEAK	);
	act( "$n is revealed!", ich, NULL, NULL, TO_ROOM );
	send_to_char( "You are revealed!\n\r", ich );
    }

    return;
}



void spell_fly( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_FLYING ) )
	return;

    af.type      = sn;
    af.duration  = level + 3;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_FLYING;
    affect_to_char( victim, &af );

    send_to_char( "Your feet rise off the ground.\n\r", victim );
    act( "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_gate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *gch;
    int        npccount  = 0;
    int        pccount   = 0;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if ( IS_NPC( gch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
	    npccount++;
	if ( !IS_NPC( gch ) ||
	    ( IS_NPC( gch ) && IS_AFFECTED( gch, AFF_CHARM ) ) )
	    pccount++;
    }

    if ( npccount > pccount )
    {
	do_say( ch, "There are too many of us here!  One must die!" );
        return;
    }

    do_say( ch, "Come brothers!  Join me in this glorious bloodbath!" );
    char_to_room( create_mobile( get_mob_index( MOB_VNUM_VAMPIRE ) ),
		 ch->in_room );
    return;
}



/*
 * Spell for mega1.are from Glop/Erkenbrand.
 */
void spell_general_purpose( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = number_range( 25, 100 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_giant_strength( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = 1 + (level >= 18) + (level >= 25);
    af.bitvector = 0;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel stronger.\n\r", victim );
    return;
}



void spell_harm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = UMAX(  20, victim->hit - dice( 1,4 ) );
    if ( saves_spell( level, victim ) )
	dam = UMIN( 50, dam / 4 );
    dam = UMIN( 100, dam );

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_heal( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    victim->hit = UMIN( victim->hit + 100, victim->max_hit );
    update_pos( victim );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "A warm feeling fills your body.\n\r", victim );
    return;
}



/*
 * Spell for mega1.are from Glop/Erkenbrand.
 */
void spell_high_explosive( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = number_range( 30, 120 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_identify( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    char         buf [ MAX_STRING_LENGTH ];

    sprintf( buf,
	    "Object '%s' is type %s, extra flags %s.\n\r",
	    obj->name,
	    item_type_name( obj ),
	    extra_bit_name( obj->extra_flags ) );
    send_to_char( buf, ch );
    sprintf( buf,
	    "Weight is %d, value is %d, level is %d.\n\r",
	    obj->weight,
	    obj->cost,
	    obj->level );
    send_to_char( buf, ch );

    switch ( obj->item_type )
    {
    case ITEM_PILL:  
    case ITEM_SCROLL: 
    case ITEM_POTION:
	sprintf( buf, "Level %d spells of:", obj->value[0] );
	send_to_char( buf, ch );

	if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[1]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[2]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	send_to_char( ".\n\r", ch );
	break;

    case ITEM_WAND: 
    case ITEM_STAFF: 
	sprintf( buf, "Has %d(%d) charges of level %d",
		obj->value[1], obj->value[2], obj->value[0] );
	send_to_char( buf, ch );
      
	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	send_to_char( ".\n\r", ch );
	break;
      
    case ITEM_WEAPON:
	sprintf( buf, "Damage is %d to %d (average %d).\n\r",
		obj->value[1], obj->value[2],
		( obj->value[1] + obj->value[2] ) / 2 );
	send_to_char( buf, ch );
	break;

    case ITEM_ARMOR:
	sprintf( buf, "Armor class is %d.\n\r", obj->value[0] );
	send_to_char( buf, ch );
	break;
    }

    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d.\n\r",
		    affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
	}
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d.\n\r",
		    affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
	}
    }

    return;
}



void spell_infravision( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_INFRARED ) )
	return;

    af.type      = sn;
    af.duration  = 2 * level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INFRARED;
    affect_to_char( victim, &af );

    send_to_char( "Your eyes glow red.\n\r", victim );
    act( "$n's eyes glow red.\n\r", ch, NULL, NULL, TO_ROOM );
    return;
}



void spell_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_INVISIBLE ) )
	return;

    af.type      = sn;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INVISIBLE;
    affect_to_char( victim, &af );

    send_to_char( "You fade out of existence.\n\r", victim );
    act( "$n fades out of existence.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_know_alignment( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    char      *msg;
    int        ap;

    ap = victim->alignment;

         if ( ap >  700 ) msg = "$N has an aura as white as the driven snow.";
    else if ( ap >  350 ) msg = "$N is of excellent moral character.";
    else if ( ap >  100 ) msg = "$N is often kind and thoughtful.";
    else if ( ap > -100 ) msg = "$N doesn't have a firm moral commitment.";
    else if ( ap > -350 ) msg = "$N lies to $S friends.";
    else if ( ap > -700 ) msg = "$N's slash DISEMBOWELS you!";
    else msg = "I'd rather just not say anything at all about $N.";

    act( msg, ch, NULL, victim, TO_CHAR );
    return;
}



void spell_lightning_bolt( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] = 
    {
	 0,
	 0,  0,  0,  0,  0,	 0,  0,  0, 25, 28,
	31, 34, 37, 40, 40,	41, 42, 42, 43, 44,
	44, 45, 46, 46, 47,	48, 48, 49, 50, 50,
	51, 52, 52, 53, 54,	54, 55, 56, 56, 57,
	58, 58, 59, 60, 60,	61, 62, 62, 63, 64
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_locate_object( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    char      buf [ MAX_INPUT_LENGTH ];
    bool      found;

    found = FALSE;
    for ( obj = object_list; obj; obj = obj->next )
    {
	if ( !can_see_obj( ch, obj ) || !is_name( target_name, obj->name ) )
	    continue;

	found = TRUE;

	for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by )
	{
	    sprintf( buf, "%s carried by %s.\n\r",
		    obj->short_descr, PERS( in_obj->carried_by, ch ) );
	}
	else
	{
	    sprintf( buf, "%s in %s.\n\r",
		    obj->short_descr, !in_obj->in_room
		    ? "somewhere" : in_obj->in_room->name );
	}

	buf[0] = UPPER( buf[0] );
	send_to_char( buf, ch );
    }

    if ( !found )
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );

    return;
}



void spell_magic_missile( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim      = (CHAR_DATA *) vo;
    static const int       dam_each [ ] = 
    {
	 0,
	 3,  3,  4,  4,  5,	 6,  6,  6,  6,  6,
	 7,  7,  7,  7,  7,	 8,  8,  8,  8,  8,
	 9,  9,  9,  9,  9,	10, 10, 10, 10, 10,
	11, 11, 11, 11, 11,	12, 12, 12, 12, 12,
	13, 13, 13, 13, 13,	14, 14, 14, 14, 14
    };
		 int       dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_mass_heal( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *gch;
    int        heal;

    for( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) )
	    continue;

	heal = dice( 17, 2 ) + level;
	gch->hit = UMIN( gch->hit + heal, gch->max_hit );
	update_pos( gch );
	
	send_to_char( "You feel better!\n\r", gch );
    }
    send_to_char( "OK.\n\r", ch );

    return;
}



void spell_mass_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) || IS_AFFECTED( gch, AFF_INVISIBLE ) )
	    continue;

	send_to_char( "You slowly fade out of existence.\n\r", gch );
	act( "$n slowly fades out of existence.", gch, NULL, NULL, TO_ROOM );

	af.type      = sn;
	af.duration  = 24;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_INVISIBLE;
	affect_to_char( gch, &af );
    }
    send_to_char( "Ok.\n\r", ch );

    return;
}



void spell_mute( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_MUTE )
	|| saves_spell( level, victim ) )
	return;
    af.type      = sn;
    af.duration  = level;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_MUTE;
    affect_to_char( victim, &af );
    
    act( "You have silenced $N!", ch, NULL, victim, TO_CHAR    );
    act( "$n has silenced you!",  ch, NULL, victim, TO_VICT    );
    act( "$n has silenced $N!",   ch, NULL, victim, TO_NOTVICT );

    return;
}



void spell_null( int sn, int level, CHAR_DATA *ch, void *vo )
{
    send_to_char( "That's not a spell!\n\r", ch );
    return;
}



void spell_pass_door( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_PASS_DOOR) )
	return;

    af.type      = sn;
    af.duration  = number_fuzzy( level / 4 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PASS_DOOR;
    affect_to_char( victim, &af );

    send_to_char( "You turn translucent.\n\r", victim );
    act( "$n turns translucent.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (   saves_spell( level, victim )
	|| victim->race == race_lookup( "Vampire" ) 
	|| victim->race == race_lookup( "Undead" ) )
	return;

    af.type      = sn;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = AFF_POISON;
    affect_join( victim, &af );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel very sick.\n\r", victim );
    return;
}


void spell_polymorph_other( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    char        buf[ MAX_STRING_LENGTH ];
    int         newrace;

    if ( IS_AFFECTED( victim, AFF_POLYMORPH ) )
    {
	act( "$E is already changed.", ch, NULL, victim, TO_CHAR );
	return;
    }

    af.type      = sn;
    af.duration  = dice( 10, level );
    af.location  = APPLY_RACE;

    do
    {
	af.modifier  = number_range( 0, MAX_RACE - 1 ) - victim->race;
    }
    while ( af.modifier == 0 );

    af.bitvector = AFF_POLYMORPH;
    newrace = victim->race + af.modifier;
    affect_to_char( victim, &af );
    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel different.\n\r", victim );

    if ( get_trust( ch ) > LEVEL_HERO )
    {
	sprintf( buf, "Log %s: casting Polymorph other", ch->name );
	log_string( buf );
    }

    return;

}



void spell_protection( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_PROTECT ) )
	return;

    af.type      = sn;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PROTECT;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel protected.\n\r", victim );
    return;
}



void spell_recharge_item ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj= (OBJ_DATA *) vo;

    if (   obj->item_type != ITEM_WAND
	&& obj->item_type != ITEM_STAFF )
    {
	send_to_char( "That item cannot be recharged.\n\r", ch );
	return;
    }

    if ( number_percent( ) <
	ch->pcdata->learned[sn] / 2 + 3 * ( ch->level - obj->value[0] )
	&& obj->value[2] < obj->value[1] )
    {
	obj->value[2] = obj->value[1];
	obj->cost     = 1;
	act( "$p shines brightly, then returns to normal.",
	    ch, obj, NULL, TO_CHAR );
	act( "$p shines brightly, then returns to normal.",
	    ch, obj, NULL, TO_ROOM );
    }
    else
    {
	act( "$p shines brightly, then explodes into fragments!",
	    ch, obj, NULL, TO_CHAR );
	act( "$p shines brightly, then explodes into fragments!",
	    ch, obj, NULL, TO_ROOM );
	extract_obj( obj );

	damage( ch, ch, ch->max_hit / 16, sn, WEAR_NONE );
    }

    return;
}



void spell_refresh( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    victim->move = UMIN( victim->move + level, victim->max_move );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel less tired.\n\r", victim );
    return;
}


/* Remove Alignment by Thelonius for EnvyMud */
void spell_remove_alignment( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if ( !IS_SET( obj->extra_flags, ITEM_EVIL		)
    &&   !IS_SET( obj->extra_flags, ITEM_ANTI_GOOD	)
    &&   !IS_SET( obj->extra_flags, ITEM_ANTI_EVIL	)
    &&   !IS_SET( obj->extra_flags, ITEM_ANTI_NEUTRAL	) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    }

    if ( number_percent( ) < ch->pcdata->learned[sn] *
	 ( 33 +
	  ( 33 * ( ch->level - obj->level ) / (float)LEVEL_HERO ) ) / 100.0 )
    {
	REMOVE_BIT( obj->extra_flags, ITEM_EVIL		);
	REMOVE_BIT( obj->extra_flags, ITEM_ANTI_GOOD	);
	REMOVE_BIT( obj->extra_flags, ITEM_ANTI_EVIL	);
	REMOVE_BIT( obj->extra_flags, ITEM_ANTI_NEUTRAL	);
	act( "$p hums briefly, then lies quiet.", ch, obj, NULL, TO_CHAR );
	act( "$p hums briefly, then lies quiet.", ch, obj, NULL, TO_ROOM );
	return;
    }

    SET_BIT( obj->extra_flags, ITEM_NODROP );
    obj->wear_flags = 1;			/* Useless   */
    obj->cost       = 0;			/* Worthless */
    act( "$p blazes brightly, then turns grey.", ch, obj, NULL, TO_CHAR );
    act( "$p blazes brightly, then turns grey.", ch, obj, NULL, TO_ROOM );

    return;

}



/* Expulsion of ITEM_NOREMOVE addition by Katrina */
void spell_remove_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        iWear;
    int        yesno  = 0;

    for ( iWear = 0; iWear < MAX_WEAR; iWear ++ )
    {
	if ( !( obj = get_eq_char( victim, iWear ) ) )
	    continue;

	if ( IS_SET( obj->extra_flags, ITEM_NOREMOVE ) )
	{
	    unequip_char( victim, obj );
	    obj_from_char( obj );
	    obj_to_room( obj, victim->in_room );
	    act( "You toss $p to the ground.",  victim, obj, NULL, TO_CHAR );
	    act( "$n tosses $p to the ground.", victim, obj, NULL, TO_ROOM );
	    yesno = 1;
	}
    }
    if ( is_affected( victim, gsn_curse ) )
    {
	affect_strip( victim, gsn_curse );
	send_to_char( "You feel better.\n\r", victim );
	yesno = 1;
    }
    
    if ( ch != victim && yesno )
        send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_remove_silence( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    /*
     *  Note: because this spell is declared as TAR_CHAR_DEFENSIVE,
     *  do_cast will set vo = ch with no argument.  Since it is
     *  impossible for a silenced character to cast anyway, if
     *  victim == ch, that means there was no argument supplied
     *  (or the spellcaster tried to specify themself), i.e., trying
     *  to remove a "Cone of Silence" from ch->in_room.
     *  This might seem pointless, but it will work within obj_cast_spell.
     *  Thelonius (Monk) 5/94
     */
    if ( victim == ch )
    {
	ROOM_INDEX_DATA *pRoomIndex;
	bool             DidSomething;

	pRoomIndex   = ch->in_room;
	DidSomething = FALSE;

	if ( IS_SET( pRoomIndex->room_flags, ROOM_CONE_OF_SILENCE ) )
        {
	    REMOVE_BIT( pRoomIndex->room_flags, ROOM_CONE_OF_SILENCE );
	    send_to_char( "You have lifted the cone of silence!\n\r", ch );
	    act( "$n has lifted the cone of silence!",
		ch, NULL, NULL,   TO_ROOM );
	    DidSomething = TRUE;
        }

	if ( IS_AFFECTED( victim, AFF_MUTE ) )
        {
	    affect_strip( victim, gsn_mute );
	    send_to_char( "You lift the veil of silence from yourself.\n\r",
			 ch );
	    act( "$n lifts the veil of silence from $mself.",
		ch, NULL, victim, TO_VICT );
	    DidSomething = TRUE;
	}

	if ( !DidSomething )
	    send_to_char( "Fzzzzzzzzz...\n\r", ch );

	return;
    }

    if ( IS_AFFECTED( victim, AFF_MUTE ) )
    {
	affect_strip( victim, gsn_mute );
	act( "You lift the veil of silence from $N.",
					ch, NULL, victim, TO_CHAR    );
	act( "$n lifts the veil of silence from you.",
					ch, NULL, victim, TO_VICT    );
	act( "$n lifts the veil of silence from $N.",
					ch, NULL, victim, TO_NOTVICT );
    }
    else
	act( "$N is not silenced.",     ch, NULL, victim, TO_CHAR    );

    return;
}



void spell_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
	return;

    af.type      = sn;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SANCTUARY;
    affect_to_char( victim, &af );

    send_to_char( "You are surrounded by a white aura.\n\r", victim );
    act( "$n is surrounded by a white aura.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.duration  = 8 + level;
    af.location  = APPLY_AC;
    af.modifier  = -20;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You are surrounded by a force shield.\n\r", victim );
    act( "$n is surrounded by a force shield.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_shocking_grasp( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] = 
    {
	 0,
	 0,  0,  0,  0,  0,	 0, 20, 25, 29, 33,
	36, 39, 39, 39, 40,	40, 41, 41, 42, 42,
	43, 43, 44, 44, 45,	45, 46, 46, 47, 47,
	48, 48, 49, 49, 50,	50, 51, 51, 52, 52,
	53, 53, 54, 54, 55,	55, 56, 56, 57, 57
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_sleep( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
  
    if ( IS_AFFECTED( victim, AFF_SLEEP )
	|| level < victim->level
	|| saves_spell( level, victim )
	|| victim->race == race_lookup( "Vampire" ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    af.type      = sn;
    af.duration  = 4 + level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SLEEP;
    affect_join( victim, &af );

    if ( IS_AWAKE( victim ) )
    {
	send_to_char( "You feel very sleepy ..... zzzzzz.\n\r", victim );
	if ( victim->position == POS_FIGHTING )
	   stop_fighting( victim, TRUE );
	do_sleep( victim, "" );
    }

    return;
}



void spell_stone_skin( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
	return;

    af.type      = sn;
    af.duration  = level;
    af.location  = APPLY_AC;
    af.modifier  = -40;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "Your skin turns to stone.\n\r", victim );
    act( "$n's skin turns to stone.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_summon( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;

    if ( !( victim = get_char_world( ch, target_name ) )
	|| victim == ch
	|| !victim->in_room
	|| IS_SET( victim->in_room->room_flags, ROOM_SAFE      )
	|| IS_SET( victim->in_room->room_flags, ROOM_PRIVATE   )
	|| IS_SET( victim->in_room->room_flags, ROOM_SOLITARY  )
	|| IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL )
	|| IS_AFFECTED( victim, AFF_CURSE )
	|| victim->level >= level + 3
	|| victim->fighting
	|| victim->in_room->area != ch->in_room->area
	|| ( IS_NPC( victim ) && saves_spell( level, victim ) ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    act( "$n disappears suddenly.", victim, NULL, NULL,     TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    act( "$n has summoned you!",    ch,     NULL, victim,   TO_VICT );
    act( "$n arrives suddenly.",    victim, NULL, NULL,     TO_ROOM );
    do_look( victim, "auto" );
    return;
}



void spell_teleport( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA       *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *pRoomIndex;

    if ( !victim->in_room
	|| IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL)
	|| ( !IS_NPC( ch ) && victim->fighting )
	|| ( victim != ch
	    && ( saves_spell( level, victim )
		|| saves_spell( level, victim ) ) ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    for ( ; ; )
    {
	pRoomIndex = get_room_index( number_range( 0, 65535 ) );
	if ( pRoomIndex )
	    if (   !IS_SET( pRoomIndex->room_flags, ROOM_PRIVATE  )
		&& !IS_SET( pRoomIndex->room_flags, ROOM_SOLITARY ) )
	    break;
    }

    if ( victim->fighting )
        stop_fighting( victim, TRUE );
    act( "$n slowly fades out of existence.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    act( "$n slowly fades into existence.",   victim, NULL, NULL, TO_ROOM );
    do_look( victim, "auto" );
    return;
}



void spell_ventriloquate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    char       buf1    [ MAX_STRING_LENGTH ];
    char       buf2    [ MAX_STRING_LENGTH ];
    char       speaker [ MAX_INPUT_LENGTH  ];

    target_name = one_argument( target_name, speaker );

    sprintf( buf1, "%s says '%s'.\n\r",              speaker, target_name );
    sprintf( buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name );
    buf1[0] = UPPER( buf1[0] );

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
	if ( !is_name( speaker, vch->name ) )
	    send_to_char( saves_spell( level, vch ) ? buf2 : buf1, vch );
    }

    return;
}



void spell_weaken( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || saves_spell( level, victim ) )
	return;

    af.type      = sn;
    af.duration  = level / 2;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    send_to_char( "You feel weaker.\n\r", victim );
    return;
}



/*
 * This is for muds that want scrolls of recall.
 */
void spell_word_of_recall( int sn, int level, CHAR_DATA *ch, void *vo )
{
    do_recall( (CHAR_DATA *) vo, "" );
    return;
}



/*
 * NPC spells.
 */
void spell_acid_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( level, victim ) )
    {
	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
	    int iWear;

	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted )
	        continue;

	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    case ITEM_ARMOR:
		if ( obj_lose->value[0] > 0 )
		{
		    /* Correction on math by IvoryTiger */ 
		    act( "$p is pitted and etched!",
			victim, obj_lose, NULL, TO_CHAR );
		    if ( ( iWear = obj_lose->wear_loc ) != WEAR_NONE )
			victim->armor += apply_ac( obj_lose, iWear );
		    obj_lose->value[0] -= 1;
		    obj_lose->cost      = 0;
		    if ( iWear != WEAR_NONE )
			victim->armor -= apply_ac( obj_lose, iWear );
		}
		break;

	    case ITEM_CONTAINER:
	    case ITEM_FURNITURE:
		act( "$p fumes and dissolves!",
		    victim, obj_lose, NULL, TO_CHAR );
		extract_obj( obj_lose );
		break;
	    }
	}
    }

    hpch = UMAX( 10, ch->hit );
    dam  = number_range( hpch / 8 + 1, hpch / 4 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_fire_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( level, victim ) )
    {
	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
	    char *msg;

	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted )
	        continue;
	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    default:             continue;
	    case ITEM_CONTAINER: msg = "$p ignites and burns!";   break;
	    case ITEM_POTION:    msg = "$p bubbles and boils!";   break;
	    case ITEM_SCROLL:    msg = "$p crackles and burns!";  break;
	    case ITEM_STAFF:     msg = "$p smokes and chars!";    break;
	    case ITEM_WAND:      msg = "$p sparks and sputters!"; break;
	    case ITEM_FOOD:      msg = "$p blackens and crisps!"; break;
	    case ITEM_PILL:      msg = "$p melts and drips!";     break;
	    case ITEM_FURNITURE: msg = "$p crackles and burns!";  break;
	    }

	    act( msg, victim, obj_lose, NULL, TO_CHAR );
	    extract_obj( obj_lose );
	}
    }

    hpch = UMAX( 10, ch->hit );
    dam  = number_range( hpch / 8 + 1, hpch / 4 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_frost_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( level, victim ) )
    {
	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
	    char *msg;

	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted )
	        continue;
	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    default:            continue;
	    case ITEM_CONTAINER:
	    case ITEM_DRINK_CON:
	    case ITEM_POTION:   msg = "$p freezes and shatters!"; break;
	    }

	    act( msg, victim, obj_lose, NULL, TO_CHAR );
	    extract_obj( obj_lose );
	}
    }

    hpch = UMAX( 10, ch->hit );
    dam  = number_range( hpch / 8 + 1, hpch / 4 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int        dam;
    int        hpch;

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
        vch_next = vch->next_in_room;
        if ( vch->deleted )
	    continue;

	if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
	{
	    hpch = UMAX( 10, ch->hit );
	    dam  = number_range( hpch / 8 + 1, hpch / 4 );
	    if ( saves_spell( level, vch ) )
		dam /= 2;
	    spell_poison( gsn_poison, level, ch, vch );
	    damage( ch, vch, dam, sn, WEAR_NONE );
	}
    }
    return;
}



void spell_lightning_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    hpch = UMAX( 10, ch->hit );
    dam = number_range( hpch / 8 + 1, hpch / 4 );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}

/*
 * Code for Psionicist spells/skills by Thelonius
 */
void spell_adrenaline_control ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.duration	 = level - 5;
    af.location	 = APPLY_DEX;
    af.modifier	 = 2;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location	= APPLY_CON;
    affect_to_char( victim, &af );

    send_to_char( "You have given yourself an adrenaline rush!\n\r", ch );
    act( "$n has given $mself an adrenaline rush!", ch, NULL, NULL,
	TO_ROOM );
   
    return;
}



void spell_agitation ( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	0,
	 0,  0,  0,  0,  0,      12, 15, 18, 21, 24,
	24, 24, 25, 25, 26,      26, 26, 27, 27, 27,
	28, 28, 28, 29, 29,      29, 30, 30, 30, 31,
	31, 31, 32, 32, 32,      33, 33, 33, 34, 34,
	34, 35, 35, 35, 36,      36, 36, 37, 37, 37
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
   
    if ( saves_spell( level, victim ) )
      dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_aura_sight ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    sn = skill_lookup( "know alignment" );

    (*skill_table[sn].spell_fun) ( sn, level, ch, vo );

    return;
}



void spell_awe ( int sn, int level, CHAR_DATA *ch, void *vo )
  {
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( victim->fighting == ch && !saves_spell( level, victim ) )
    {
	stop_fighting ( victim, TRUE);
	act( "$N is in AWE of you!", ch, NULL, victim, TO_CHAR    );
	act( "You are in AWE of $n!",ch, NULL, victim, TO_VICT    );
	act( "$N is in AWE of $n!",  ch, NULL, victim, TO_NOTVICT );
    }
    return;
}



void spell_ballistic_attack ( int sn, int level, CHAR_DATA *ch, void *vo )
  {
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	 0,
	 3,  4,  4,  5,  6,       6,  6,  7,  7,  7,
	 7,  7,  8,  8,  8,       9,  9,  9, 10, 10,
	10, 11, 11, 11, 12,      12, 12, 13, 13, 13,
	14, 14, 14, 15, 15,      15, 16, 16, 16, 17,
	17, 17, 18, 18, 18,      19, 19, 19, 20, 20
    };
		 int        dam;
	
    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim ) )
      dam /= 2;

    act( "You chuckle as a stone strikes $N.", ch, NULL, victim,
	TO_CHAR );

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_biofeedback ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    sn = skill_lookup( "Sanctuary" );

    (*skill_table[sn].spell_fun) ( sn, level, ch, vo );

    return;
}



void spell_cell_adjustment ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
   
    if ( is_affected( victim, gsn_poison ) )
    {
	affect_strip( victim, gsn_poison );
	send_to_char( "A warm feeling runs through your body.\n\r", victim );
	act( "$N looks better.", ch, NULL, victim, TO_NOTVICT );
    }
    if ( is_affected( victim, gsn_curse  ) )
    {
	affect_strip( victim, gsn_curse  );
	send_to_char( "You feel better.\n\r", victim );
    }	
    send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_combat_mind ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if ( victim == ch )
	  send_to_char( "You already understand battle tactics.\n\r",
		       victim );
	else
	  act( "$N already understands battle tactics.",
	      ch, NULL, victim, TO_CHAR );
	return;
    }

    af.type	 = sn;
    af.duration	 = level + 3;
    af.location	 = APPLY_HITROLL;
    af.modifier	 = level / 6;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location	 = APPLY_AC;
    af.modifier	 = - level/2 - 5;
    affect_to_char( victim, &af );

    if ( victim != ch )
        send_to_char( "OK.\n\r", ch );
    send_to_char( "You gain a keen understanding of battle tactics.\n\r",
		 victim );
    return;
}



void spell_complete_healing ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    victim->hit = victim->max_hit;
    update_pos( victim );
    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    send_to_char( "Ahhhhhh...You are completely healed!\n\r", victim );
    return;
}



void spell_control_flames ( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] = 
    {
	 0,
	 0,  0,  0,  0,  0,       0,  0, 16, 20, 24,
	28, 32, 35, 38, 40,      42, 44, 45, 45, 45,
	46, 46, 46, 47, 47,      47, 48, 48, 48, 49,
	49, 49, 50, 50, 50,      51, 51, 51, 52, 52,
	52, 53, 53, 53, 54,      54, 54, 55, 55, 55
    };
		 int        dam;

    if ( !get_eq_char( ch, WEAR_LIGHT ) )
    {
	send_to_char( "You must be carrying a light source.\n\r", ch );
	return;
    }

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim ) )
        dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_create_sound ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    char       buf1    [ MAX_STRING_LENGTH ];
    char       buf2    [ MAX_STRING_LENGTH ];
    char       speaker [ MAX_INPUT_LENGTH  ];

    target_name = one_argument( target_name, speaker );

    sprintf( buf1, "%s says '%s'.\n\r", speaker, target_name );
    sprintf( buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name );
    buf1[0] = UPPER( buf1[0] );

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
	if ( !is_name( speaker, vch->name ) )
	    send_to_char( saves_spell( level, vch ) ? buf2 : buf1, vch );
    }
    return;
}



void spell_death_field ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int        dam;
    int        hpch;

    if ( !IS_EVIL( ch ) )
    {
	send_to_char( "You are not evil enough to do that!\n\r", ch);
	return;
    }

    send_to_char( "A black haze emanates from you!\n\r", ch );
    act ( "A black haze emanates from $n!", ch, NULL, ch, TO_ROOM );

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
        vch_next = vch->next_in_room;
	if ( vch->deleted )
	  continue;

	if ( !IS_NPC( ch ) ? IS_NPC( vch ) : IS_NPC( vch ) )
	{
	    hpch = URANGE( 10, ch->hit, 999 );
	    if ( !saves_spell( level, vch )
		&& (   level <= vch->level + 5
		    && level >= vch->level - 5 ) )
            {
		dam = 4; /* Enough to compensate for sanct. and prot. */
		vch->hit = 1;
		update_pos( vch );
		send_to_char( "The haze envelops you!\n\r", vch );
		act( "The haze envelops $N!", ch, NULL, vch, TO_ROOM );
            }
	    else
	        dam = number_range( hpch / 16 + 1, hpch / 8 );

	    damage( ch, vch, dam, sn, WEAR_NONE );
        }
    }
    return;
}



void spell_detonate ( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	  0,
	  0,   0,   0,   0,   0,        0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,        0,   0,   0,   0,  75,
	 80,  85,  90,  95, 100,      102, 104, 106, 108, 110,
	112, 114, 116, 118, 120,      122, 124, 126, 128, 130,
	132, 134, 136, 138, 140,      142, 144, 146, 148, 150
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim ) )
        dam /= 2;
    damage( ch, victim, dam, sn, WEAR_NONE );
    return;
}



void spell_disintegrate ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;

    if ( number_percent( ) < 2 * level && !saves_spell( level, victim ) )
      for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
      {
	  obj_next = obj_lose->next_content;
	  if ( obj_lose->deleted )
	      continue;

	  if ( number_bits( 2 ) != 0 )
	      continue;

	  act( "$p disintegrates!", victim, obj_lose, NULL, TO_CHAR );
	  act( "$p disintegrates!", victim, obj_lose, NULL, TO_ROOM );
	  extract_obj( obj_lose ) ;
      }

    if ( !saves_spell( level, victim ) )
    /*
     * Disintegrate char, do not generate a corpse, do not
     * give experience for kill.  Extract_char will take care
     * of items carried/wielded by victim.  Needless to say,
     * it would be bad to be a target of this spell!
     * --- Thelonius (Monk)
     */
    {
	act( "You have DISINTEGRATED $N!",         ch, NULL, victim, TO_CHAR );
	act( "You have been DISINTEGRATED by $n!", ch, NULL, victim, TO_VICT );
	act( "$n's spell DISINTEGRATES $N!",       ch, NULL, victim, TO_ROOM );
	
	if ( IS_NPC( victim ) )
	    extract_char( victim, TRUE );
	else
	    extract_char( victim, FALSE );
    }
    return;
}



void spell_displacement ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;

    af.type	 = sn;
    af.duration	 = level - 4;
    af.location	 = APPLY_AC;
    af.modifier	 = 4 - level;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "Your form shimmers, and you appear displaced.\n\r",
		 victim );
    act( "$N shimmers and appears in a different location.",
	ch, NULL, victim, TO_NOTVICT );
    return;
}



void spell_domination ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim == ch )
    {
	send_to_char( "Dominate yourself?  You're weird.\n\r", ch );
	return;
    }

    if (   IS_AFFECTED( victim, AFF_CHARM )
	|| IS_AFFECTED( ch,     AFF_CHARM )
	|| level < victim->level
	|| saves_spell( level, victim ) )
        return;

    if ( victim->master )
        stop_follower( victim );
    add_follower( victim, ch );

    af.type	 = sn;
    af.duration	 = number_fuzzy( level / 4 );
    af.location	 = APPLY_NONE;
    af.modifier	 = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );

    act( "Your will dominates $N!", ch, NULL, victim, TO_CHAR );
    act( "Your will is dominated by $n!", ch, NULL, victim, TO_VICT );
    return;
}



void spell_ectoplasmic_form ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_PASS_DOOR ) )
        return;

    af.type	 = sn;
    af.duration	 = number_fuzzy( level / 4 );
    af.location	 = APPLY_NONE;
    af.modifier	 = 0;
    af.bitvector = AFF_PASS_DOOR;
    affect_to_char( victim, &af );

    send_to_char( "You turn translucent.\n\r", victim );
    act( "$n turns translucent.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_ego_whip ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || saves_spell( level, victim ) )
        return;

    af.type	 = sn;
    af.duration	 = level;
    af.location	 = APPLY_HITROLL;
    af.modifier	 = -2;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location	 = APPLY_SAVING_SPELL;
    af.modifier	 = 2;
    affect_to_char( victim, &af );

    af.location	 = APPLY_AC;
    af.modifier	 = level / 2;
    affect_to_char( victim, &af );

    act( "You ridicule $N about $S childhood.", ch, NULL, victim, TO_CHAR    );
    send_to_char( "Your ego takes a beating.\n\r", victim );
    act( "$N's ego is crushed by $n!",          ch, NULL, victim, TO_NOTVICT );

    return;
}



void spell_energy_containment ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.duration	 = level / 2 + 7;
    af.modifier	 = -level / 5;
    af.location  = APPLY_SAVING_SPELL;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "You can now absorb some forms of energy.\n\r", ch );
    return;
}



void spell_enhance_armor (int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    if ( obj->item_type != ITEM_ARMOR
	|| IS_OBJ_STAT( obj, ITEM_MAGIC )
	|| obj->affected )
    {
	send_to_char( "That item cannot be enhanced.\n\r", ch );
	return;
    }

    paf			= new_affect();

    paf->type		= sn;
    paf->duration	= -1;
    paf->location	= APPLY_AC;
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;

    if ( number_percent() < ch->pcdata->learned[sn]/2
	+ 3 * ( ch->level - obj->level ) )

    /* Good enhancement */
    {
	paf->modifier   = -level / 8;

	     if ( IS_GOOD( ch ) )
	{
	    SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
	    act( "$p glows blue.",   ch, obj, NULL, TO_CHAR );
	}
	else if ( IS_EVIL( ch ) )
        {
	    SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
	    act( "$p glows red.",    ch, obj, NULL, TO_CHAR );
	}
	else
	{
	    SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
	    SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
	    act( "$p glows yellow.", ch, obj, NULL, TO_CHAR );
	}
       
	send_to_char( "Ok.\n\r", ch );
    }
    else
    /* Bad Enhancement ... opps! :) */
    {
	paf->modifier   = level / 8;
	obj->cost       = 0;

	SET_BIT( obj->extra_flags, ITEM_NODROP );
	act( "$p turns black.", ch, obj, NULL, TO_CHAR );
    }

    return;
}



void spell_enhanced_strength ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.duration	 = level;
    af.location	 = APPLY_STR;
    af.modifier	 = 1 + ( level >= 15 ) + ( level >= 25 );
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "You are HUGE!\n\r", victim );
    return;
}



void spell_flesh_armor ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.duration	 = level;
    af.location	 = APPLY_AC;
    af.modifier	 = -40;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "Your flesh turns to steel.\n\r", victim );
    act( "$N's flesh turns to steel.", ch, NULL, victim, TO_NOTVICT);
    return;
}



void spell_inertial_barrier ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) || IS_AFFECTED( gch, AFF_PROTECT ) )
	    continue;

	act( "An inertial barrier forms around $n.", gch, NULL, NULL,
	    TO_ROOM );
	send_to_char( "An inertial barrier forms around you.\n\r", gch );

	af.type	     = sn;
	af.duration  = 24;
	af.modifier  = 0;
	af.location  = APPLY_NONE;
	af.bitvector = AFF_PROTECT;
	affect_to_char( gch, &af );
    }
    return;
}



void spell_inflict_pain ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    damage( ch, (CHAR_DATA *) vo, dice( 2, 10 ) + level / 2, sn, WEAR_NONE );
    return;
}



void spell_intellect_fortress ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) || is_affected( gch, sn ) )
	    continue;

	send_to_char( "A virtual fortress forms around you.\n\r", gch );
	act( "A virtual fortress forms around $N.", gch, NULL, gch, TO_ROOM );

	af.type	     = sn;
	af.duration  = 24;
	af.location  = APPLY_AC;
	af.modifier  = -40;
	af.bitvector = 0;
	affect_to_char( gch, &af );
    }
    return;
}



void spell_lend_health ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        hpch;

    if ( ch == victim )
    {
	send_to_char( "Lend health to yourself?  What a weirdo.\n\r", ch );
	return;
    }
    hpch = UMIN( 50, victim->max_hit - victim->hit );
    if ( hpch == 0 )
    {
	act( "Nice thought, but $N doesn't need healing.", ch, NULL,
	    victim, TO_CHAR );
	return;
    }
    if ( ch->hit-hpch < 50 )
    {
	send_to_char( "You aren't healthy enough yourself!\n\r", ch );
	return;
    }
    victim->hit += hpch;
    ch->hit     -= hpch;
    update_pos( victim );
    update_pos( ch );

    act( "You lend some of your health to $N.", ch, NULL, victim, TO_CHAR );
    act( "$n lends you some of $s health.",     ch, NULL, victim, TO_VICT );

    return;
}



void spell_levitation ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_FLYING ) )
        return;

    af.type	 = sn;
    af.duration	 = level + 3;
    af.location	 = APPLY_NONE;
    af.modifier	 = 0;
    af.bitvector = AFF_FLYING;
    affect_to_char( victim, &af );

    send_to_char( "Your feet rise off the ground.\n\r", victim );
    act( "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_mental_barrier ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.duration	 = 24;
    af.location	 = APPLY_AC;
    af.modifier	 = -20;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "You erect a mental barrier around yourself.\n\r",
		 victim );
    return;
}



void spell_mind_thrust ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    damage( ch, (CHAR_DATA *) vo, dice( 1, 10 ) + level / 2, sn, WEAR_NONE );
    return;
}



void spell_project_force ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    damage( ch, (CHAR_DATA *) vo, dice( 4, 6 ) + level, sn, WEAR_NONE );
    return;
}



void spell_psionic_blast ( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	  0,
	  0,   0,   0,   0,   0,        0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,        0,  45,  50,  55,  60,
	 64,  68,  72,  76,  80,       82,  84,  86,  88,  90,
	 92,  94,  96,  98, 100,      102, 104, 106, 108, 100,
	112, 114, 116, 118, 120,      122, 124, 126, 128, 130
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    if ( saves_spell( level, victim ) )
        dam /= 2;

    damage( ch, victim, dam, sn, WEAR_NONE );

    return;
}



void spell_psychic_crush ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    damage( ch, (CHAR_DATA *) vo, dice( 3, 5 ) + level, sn, WEAR_NONE );
    return;
}



void spell_psychic_drain ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || saves_spell( level, victim ) )
        return;

    af.type	 = sn;
    af.duration	 = level / 2;
    af.location	 = APPLY_STR;
    af.modifier	 = -1 - ( level >= 10 ) - ( level >= 20 ) - ( level >= 30 );
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "You feel drained.\n\r", victim );
    act( "$n appears drained of strength.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_psychic_healing ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    heal = dice( 3, 6 ) + 2 * level / 3 ;
    victim->hit = UMIN( victim->hit + heal, victim->max_hit );
    update_pos( victim );

    send_to_char( "You feel better!\n\r", victim );
    return;
}



void spell_share_strength ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim == ch )
    {
	send_to_char( "You can't share strength with yourself.\n\r", ch );
	return;
    }
    if ( is_affected( victim, sn ) )
    {
	act( "$N already shares someone's strength.", ch, NULL, victim,
	    TO_CHAR );
	return;
    }
    if ( get_curr_str( ch ) <= 5 )
    {
	send_to_char( "You are too weak to share your strength.\n\r", ch );
	return;
    }

    af.type	 = sn;
    af.duration	 = level;
    af.location	 = APPLY_STR;
    af.modifier	 =  1 + ( level >= 20 ) + ( level >= 30 );
    af.bitvector = 0;
    affect_to_char( victim, &af );
    
    af.modifier	 = -1 - ( level >= 20 ) - ( level >= 30 );
    affect_to_char( ch,     &af );

    act( "You share your strength with $N.", ch, NULL, victim, TO_CHAR );
    act( "$n shares $s strength with you.",  ch, NULL, victim, TO_VICT );
    return;
}



void spell_thought_shield ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.duration	 = level;
    af.location	 = APPLY_AC;
    af.modifier	 = -20;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "You have created a shield around yourself.\n\r", ch );
    return;
}

void spell_ultrablast ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int        dam;
    int        hpch;

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
        vch_next = vch->next_in_room;
	if ( vch->deleted )
	    continue;

	if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
	{
	    hpch = UMAX( 10, ch->hit );
	    dam  = number_range( hpch / 8+1, hpch / 4 );
	    if ( saves_spell( level, vch ) )
	        dam /= 2;
	    damage( ch, vch, dam, sn, WEAR_NONE );
	}
    }
    return;
}

void spell_vampiric_bite( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    OBJ_DATA   *obj;
    AFFECT_DATA af;
    int         dam;

    dam = dice( 5, level );
    damage( ch, victim, dam, sn, WEAR_NONE );
    ch->hit = UMIN( ch->max_hit, ch->hit + dam );

    if ( victim->level < 11 || get_age( victim ) < 21 )
        return;

    if ( IS_AFFECTED( victim, AFF_POLYMORPH ) )
        return;

    if ( saves_spell( level, victim )
	|| number_bits( 1 ) == 0 )
	return;

    if ( ( obj = get_eq_char( victim, ITEM_HOLD ) ) )
    {
        if ( IS_OBJ_STAT( obj, ITEM_VAMPIRE_BANE )
	    && ch->level < 21 )
	    return;
	else
	{
	    if ( IS_OBJ_STAT( obj, ITEM_HOLY ) )
	    {
	        if ( ch->level < 32 )
		{
		    return;
		}
		else
		{
		    if ( victim->level > ch->level )
		        return;
		}
	    }
	}
    }
		  
    af.type      = sn;
    af.duration  = UMAX( 5, 30 - level );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_VAMP_BITE;
    affect_join( victim, &af );

    if ( ch != victim )
	send_to_char( "Ahh!  Taste the power!\n\r", ch );
    send_to_char( "Your blood begins to burn!\n\r", victim );
    return;
}


void spell_turn_undead( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;

    if (   victim->level >= level
	|| saves_spell( level, victim ) )
    {
        send_to_char( "You have failed.\n\r", ch );
	return;
    }

    if (   victim->race == race_lookup( "Vampire" )
	|| victim->race == race_lookup( "Undead" ) )
        do_flee( victim, "" );

    return;

}


void spell_exorcise( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !is_affected( victim, gsn_vampiric_bite ) )
        return;

    /*
     * We actually give the vampiric curse a chance to save
     * at the victim's level
     */
    if ( !saves_spell( level, victim ) )
        return;

    affect_strip( victim, gsn_vampiric_bite );

    send_to_char( "Ok.\n\r",                                    ch     );
    send_to_char( "A warm feeling runs through your body.\n\r", victim );
    act( "$N looks better.", ch, NULL, victim, TO_NOTVICT );

    return;

}

/* Flame shield spell from Malice of EnvyMud */
void spell_flaming( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_FLAMING ) )
        return;

    af.type      = sn;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_FLAMING;
    affect_to_char( victim, &af );

    send_to_char( "You are surrounded by a flaming shield.\n\r", victim );
    act( "$n is surrounded by a flaming shield.",
	victim, NULL, NULL, TO_ROOM );
    return;

}
