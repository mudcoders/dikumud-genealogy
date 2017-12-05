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
 * Local functions.
 */
void	say_spell	args( ( CHAR_DATA *ch, int sn ) );

/* Globals (from magic.c) */
extern char *target_name;

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
    if ( is_affected( victim, gsn_plague ) )	/* Plague addition by Maniac */
    {
	affect_strip( victim, gsn_plague );
	send_to_char( "Your plague sores seem to disappear.\n\r", victim );
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

    if ( !affect_free )
    {
	paf	    = alloc_perm( sizeof( *paf ) );
    }
    else
    {
	paf         = affect_free;
	affect_free = affect_free->next;
    }

    paf->type	   = sn;
    paf->duration  = -1;
    paf->location  = APPLY_AC;
    paf->bitvector = 0;
    paf->next	   = obj->affected;
    obj->affected  = paf;

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

void spell_animate_dead( int sn, int level, CHAR_DATA *ch, void *vo)
/* Animate Dead by Garion */
{
    CHAR_DATA   *zombie;
    OBJ_DATA    *obj;


    obj = get_obj_here( ch, "corpse");

    if ( obj == NULL )
    {
        send_to_char("There is no corpse here.\n\r", ch);
        return;
    }

    if ( obj->item_type != ITEM_CORPSE_NPC )
    {
        send_to_char("You cannot animate that.\n\r", ch);
        return;
    }
    else
    {
        OBJ_DATA *equip, *equip_next;

        zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE ) );
        SET_BIT( zombie->act, ACT_PET );
        SET_BIT( zombie->affected_by, AFF_CHARM );
        char_to_room( zombie, ch->in_room );
        zombie->level = ( ch->level - 5 );
        zombie->max_hit = ( ch->max_hit /2 );
        zombie->hit = ( ch->max_hit /2 );


        for ( equip = obj->contains; equip != NULL; equip = equip_next )
        {
            equip_next = equip->next_content;
            obj_from_obj( equip );
            obj_to_char( equip, zombie );
        }

        extract_obj( obj );
        if ( ch->alignment >= -1000 )
        {
            ch->alignment = UMAX( -1000, ch->alignment - 200 );
        }

        send_to_char("You call upon dark powers to animate the corpse.\n\r",
                ch);
        act("$n calls down evil spirits to inhabit the corpse.", ch, NULL,
                NULL, TO_ROOM);
        add_follower( zombie, ch );
        zombie->leader = ch;
        interpret( zombie, "wear all" );
        return;
    }
}

void spell_age ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = ( CHAR_DATA *) vo;

    if( (ch != victim ) && saves_spell( level, victim ) )
    {
	send_to_char( "You have failed.\n\r", ch );
	return;
    }

    victim->current_age += ( ( level / 2 ) * MUD_YEAR );

    act( "$N suddenly looks older!", ch, NULL, victim, TO_ROOM );
    act( "$N suddenly looks older!", ch, NULL, victim, TO_CHAR );
    send_to_char ("You feel a lot older.\n\r", victim );

    return;
}

void spell_rejuvenate ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = ( CHAR_DATA *) vo;

    if ( ch != victim )
    {
	if( saves_spell( level, victim ) )
	{
	    send_to_char( "You have failed.\n\r", ch );
	    return;
	}
    }

    victim->current_age -= ( ( level / 2 ) * MUD_YEAR );

    if ( get_age( victim ) < race_table[victim->race].start_age )
	victim->current_age = MUD_YEAR * race_table[victim->race].start_age;

    act( "$N suddenly looks younger than before", ch, NULL, victim, TO_ROOM );
    act( "$N suddenly looks younger than before", ch, NULL, victim, TO_CHAR );
    send_to_char( "You feel a lot younger.\n\r", victim );

    return;
}
