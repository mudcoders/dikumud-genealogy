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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
bool	check_dodge	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_shield_block   args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	dam_message	     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
				    int dt, int wpn, bool immune ) );
void	death_cry	     args( ( CHAR_DATA *ch ) );
void	group_gain	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	     args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
bool    is_wielding_poisoned args( ( CHAR_DATA *ch, int wpn ) );
void	make_corpse	     args( ( CHAR_DATA *ch ) );
void	one_hit		     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt,
				    int wpn ) );
void	raw_kill	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	set_fighting	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm		     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	trip		     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

bool    check_race_special   args( ( CHAR_DATA *ch ) );
void    use_magical_item     args( ( CHAR_DATA *ch ) );


/*
 * Control the fights going on.
 * Called periodically by update_handler.
 * Slightly less efficient than Merc 2.2.  Takes 10% of 
 *  total CPU time.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    bool       mobfighting;

    for ( ch = char_list; ch; ch = ch->next )
    {
	if ( !ch->in_room || ch->deleted )
	    continue;

	if ( ( victim = ch->fighting ) )
	{
	    mprog_hitprcnt_trigger( ch, victim );
	    mprog_fight_trigger( ch, victim );

	    if ( IS_AWAKE( ch ) && ch->in_room == victim->in_room
		&& !victim->deleted )
	    {
	        /* Ok here we test for switch if victim is charmed */
	        if ( IS_AFFECTED( victim, AFF_CHARM )
		    && victim->master
		    && victim->in_room == victim->master->in_room
		    && ch != victim
		    && number_percent( ) > 40 )
		{
		    stop_fighting( ch, FALSE );
		    multi_hit( ch, victim->master, TYPE_UNDEFINED );
		}
		else
		{
		    multi_hit( ch, victim, TYPE_UNDEFINED );
		}
	    }
	    else
	    {
	        stop_fighting( ch, FALSE );
	    }
	    continue;
	}


	if ( IS_AFFECTED( ch, AFF_BLIND )
	    || ( IS_NPC( ch ) && ch->pIndexData->pShop )
	    || ( IS_NPC( ch ) && ch->pIndexData->pGame ) )
	    continue;

	/*
	 * Ok. So ch is not fighting anyone.
	 * Is there a fight going on?
	 */

	mobfighting = FALSE;

	for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	{
	    if ( rch->deleted
		|| !IS_AWAKE( rch )
		|| !( victim = rch->fighting ) )
	        continue;

	    if ( !IS_NPC( ch )
		&& ( !IS_NPC( rch ) || IS_AFFECTED( rch, AFF_CHARM ) )
		&& is_same_group( ch, rch )
		&& IS_NPC( victim ) )
		break;

	    if ( IS_NPC( ch )
		&& IS_NPC( rch )
		&& !IS_NPC( victim ) )
	    {
		mobfighting = TRUE;
		break;
	    }
	}

	if ( !victim || !rch )
	    continue;

	/*
	 * Now that someone is fighting, consider fighting another pc
	 * or not at all.
	 */
	if ( mobfighting )
	{
	    CHAR_DATA *vch;
	    int        number;

	    number = 0;
	    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	    {
		if ( can_see( ch, vch )
		    && is_same_group( vch, victim )
		    && number_range( 0, number ) == 0 )
		{
		    victim = vch;
		    number++;
		}
	    }

	    if ( ( rch->pIndexData != ch->pIndexData && number_bits( 3 ) != 0 )
		|| ( IS_GOOD( ch ) && IS_GOOD( victim ) )
		|| abs( victim->level - ch->level ) > 3 )
	        continue;
	}

	multi_hit( ch, victim, TYPE_UNDEFINED );

    }

    return;
}

/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int chance;

    /*
     * Set the fighting fields now.
     */
    if ( victim->position > POS_STUNNED )
    {
	if ( !victim->fighting )
	    set_fighting( victim, ch );
	victim->position = POS_FIGHTING;
	if ( !ch->fighting )
	    set_fighting( ch, victim );
    }

    if ( !IS_NPC( ch ) && ch->fighting )
    {
        if ( check_race_special( ch ) )
	    return;
    }

    one_hit( ch, victim, dt, WEAR_WIELD );
    if ( ch->fighting != victim || dt == gsn_backstab || dt == gsn_circle )
	return;

    chance = IS_NPC( ch ) ? ch->level
                          : ch->pcdata->learned[gsn_second_attack] / 2;
    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, WEAR_WIELD );
	if ( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC( ch ) ? ch->level
                          : ch->pcdata->learned[gsn_third_attack] / 4;
    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, WEAR_WIELD );
	if ( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC( ch ) ? ch->level
                          : ch->pcdata->learned[gsn_fourth_attack] / 4;
    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, WEAR_WIELD );
	if ( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC( ch ) ? ch->level / 2
                          : 0;
    if ( number_percent( ) < chance )
    {
        one_hit( ch, victim, dt, WEAR_WIELD );
        if ( ch->fighting != victim )
            return;
    }

    if ( get_eq_char( ch, WEAR_WIELD_2 ) )
    {
        chance = IS_NPC( ch ) ? ch->level : ch->pcdata->learned[gsn_dual] / 2 ;
        if ( number_percent( ) < chance )
            one_hit( ch, victim, dt, WEAR_WIELD_2 );
    }

    return;
}



/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int wpn )
{
    OBJ_DATA *wield;
    char      buf [ MAX_STRING_LENGTH ];
    int       victim_ac;
    int       thac0;
    int       thac0_00;
    int       thac0_47;
    int       dam;
    int       diceroll;
    int       race_hit_bonus;
    int       wpn_gsn;
    int       dam_type;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
    {
	sprintf( buf, "one_hit: ch %s not with victim %s, or victim POS_DEAD",
		ch->name, victim->name );
	bug( buf, 0 );
	return;
    }

    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, wpn );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
    }

    /*
     * Weapon proficiencies.
     */
    wpn_gsn	= gsn_hit;
    dam_type	= DAM_BASH;
    if ( wield && wield->item_type == ITEM_WEAPON )
    {
	if ( wield->value[3] >= 0 && wield->value[3] < MAX_ATTACK )
	{
	    wpn_gsn	= (*attack_table[wield->value[3]].wpn_gsn);
	    dam_type	= attack_table[wield->value[3]].dam_type;
	}
	else
	{
	    sprintf( buf, "one_hit: bad weapon dt %d caused by %s.",
		    dt, ch->name );
	    bug( buf, 0 );
	    wield->value[3] = 0;
	}
    }


    /*
     * Figure out the racial hit bonuses.
     */
    race_hit_bonus = ( race_table[victim->race].size
                   - race_table[ch->race].size ) / 3;

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC( ch ) )
    {
	thac0_00 =  20;
	thac0_47 = -10;
    }
    else
    {
	thac0_00 = class_table[ch->class]->thac0_00;
	thac0_47 = class_table[ch->class]->thac0_47;
    }
    /* Weapon-specific hitroll and damroll */

    thac0     = interpolate( ch->level, thac0_00, thac0_47 )
              - get_hitroll( ch, wpn )
	      - race_hit_bonus;	
    victim_ac = UMAX( -15, GET_AC( victim ) / 10 );
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

    /* Weapon proficiencies */
    if ( wield && wield->item_type == ITEM_WEAPON
	&& ( IS_NPC( ch )
	   ? UMIN( 60, 2 * ch->level ) : ch->pcdata->learned[wpn_gsn] ) >
	   ( IS_NPC( victim )
	   ? UMIN( 60, 2 * victim->level ) : victim->pcdata->learned[wpn_gsn] ) )
         victim_ac += 2;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if (     diceroll == 0
	|| ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt, wpn, dam_type );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC( ch ) )
    {
	dam = number_range( ch->level / 2, ch->level * 3 / 2 );
	if ( wield )
	    dam += dam / 2;
    }
    else
    {
	if ( wield )
	    dam = number_range( wield->value[1], wield->value[2] );
	else
	    dam = number_range( 1, 2 ) * race_table[ ch->race ].size;
	if ( wield && dam > 1000 )
	{
	    sprintf( buf, "One_hit dam range > 1000 from %d to %d",
		    wield->value[1], wield->value[2] );
	    bug( buf, 0 );
	}
    }

    /*
     * Bonuses.
     */
    dam += get_damroll( ch, wpn );

    if ( wield && IS_SET( wield->extra_flags, ITEM_POISONED ) )
        dam += dam / 4;
    /* Weapon proficiencies */
    if ( wield && !IS_NPC( ch ) && ch->pcdata->learned[wpn_gsn] > 0 )
	dam += dam * ch->pcdata->learned[wpn_gsn] / 150;
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
	dam += dam * ch->pcdata->learned[gsn_enhanced_damage] / 150;
    if ( !IS_AWAKE( victim ) )
	dam *= 2;
    if ( dt == gsn_backstab )
	dam *= 2 + UMIN( ( ch->level / 8 ), 4 );
    else if ( dt == gsn_circle )              /* 150% to 200% at lev. 50 */
	dam += dam / 2 + ( dam * ch->level ) / 100;
    if ( !str_cmp( race_table[ch->race].name, "Vampire" ) && !wield )
        dam += dam / 2;

    if ( dam <= 0 )
	dam = 1;

    if ( wield && wield->item_type == ITEM_WEAPON
	&& attack_table[wield->value[3]].hit_fun
	&& (*attack_table[wield->value[3]].hit_fun)(ch, victim, diceroll, dam) )
	return;

    damage( ch, victim, dam, dt, wpn, dam_type );
    tail_chain( );
    return;
}


/*
 * Inflict damage from a hit.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int wpn,
	    int dam_type )
{
    bool immune;

    if ( victim->position == POS_DEAD )
	return;

    /*
     * Stop up any residual loopholes.
     */
    if ( dam > 1000 )
    {
        char buf [ MAX_STRING_LENGTH ];

	if ( IS_NPC( ch ) && ch->desc )
	    sprintf( buf,
		    "Damage: %d from %s by %s: > 1000 points with %d dt!",
		    dam, ch->name, ch->desc->original->name, dt );
	else
	    sprintf( buf,
		    "Damage: %d from %s: > 1000 points with %d dt!",
		    dam, IS_NPC( ch ) ? ch->short_descr : ch->name, dt );

	bug( buf, 0 );
	dam = 1000;
    }

    immune = FALSE;
    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return;
	check_killer( ch, victim );

	if ( victim->position > POS_STUNNED )
	{
	    if ( !victim->fighting )
		set_fighting( victim, ch );
	    victim->position = POS_FIGHTING;

	    if ( !ch->fighting )
		set_fighting( ch, victim );

	    /*
	     * If NPC victim is following, ch might attack victim's master.
	     * No charm check here because charm would be dispelled from
	     * tanking mobile when combat ensues thus ensuring PC charmer is
	     * not harmed.
	     * Check for is_same_group wont work as following mobile is not
	     * always grouped with PC charmer - Kahn
	     */
	    if (   IS_NPC( ch )
		&& IS_NPC( victim )
		&& victim->master
		&& victim->master->in_room == ch->in_room
		&& number_bits( 2 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		set_fighting( ch, victim->master );
		return;
	    }
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );

	/*
	 * Inviso attacks ... not.
	 */
	if ( IS_AFFECTED( ch, AFF_INVISIBLE ) )
	{
	    affect_strip( ch, gsn_invis      );
	    affect_strip( ch, gsn_mass_invis );
	    REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
	    act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
	}

	/*
	 * Hunting stuff...
	 */
	if ( dam && IS_NPC( victim ) )
	{
	    if ( !IS_SET( victim->act, ACT_SENTINEL ) )
	    {
		if ( victim->hunting )
		{
		    if ( victim->hunting->who != ch )
		    {
			free_string( victim->hunting->name );
			victim->hunting->name = str_dup( ch->name );
			victim->hunting->who  = ch;
		    }
		}
		else
		    start_hunting( victim, ch );
	    }

	    if ( victim->hating )
	    {
		if ( victim->hating->who != ch )
		{
		    free_string( victim->hating->name );
		    victim->hating->name = str_dup( ch->name );
		    victim->hating->who  = ch;
		}
	    }
	    else
		start_hating( victim, ch );
	}

	/*
	 * Damage modifiers.
	 */
	if (   IS_AFFECTED( victim, AFF_SANCTUARY      )
	    || IS_SET( race_table[ victim->race ].race_abilities,
		      RACE_SANCT ) )
	    dam /= 2;

	if ( (   IS_AFFECTED( victim, AFF_PROTECT_EVIL )
	      || IS_SET( race_table[ victim->race ].race_abilities,
			RACE_PROTECTION )              )
	    && IS_EVIL( ch )                           )
	    dam -= dam / 4;
	else if ( (   IS_AFFECTED( victim, AFF_PROTECT_GOOD )
	      || IS_SET( race_table[ victim->race ].race_abilities,
			RACE_PROTECTION )              )
	    && IS_GOOD( ch )                           )
	    dam -= dam / 4;

	if ( dam < 0 )
	    dam = 0;

	/*
	 * Check for disarm, trip, parry, dodge and shield block.
	 */
	if ( dt >= TYPE_HIT || dt == gsn_kick )
	{
	    int leveldiff = ch->level - victim->level;

	    if ( IS_NPC( ch ) && number_percent( )
		< ( leveldiff < -5 ? ch->level / 2 : UMAX( 10, leveldiff ) )
		&& dam == 0 )
	        disarm( ch, victim );
	    if ( IS_NPC( ch ) && number_percent( )
		< ( leveldiff < -5 ? ch->level / 2 : UMAX( 20, leveldiff ) )
		&& dam == 0 )
		trip( ch, victim );
	    if ( IS_NPC( ch )
		&& IS_SET( race_table[ ch->race ].race_abilities,
			  RACE_WEAPON_WIELD )
		&& number_percent( ) < UMIN( 25, UMAX( 10, ch->level ) )
		&& !IS_NPC( victim ) )
	        use_magical_item( ch );
	    if ( check_parry( ch, victim ) && dam > 0 )
		return;
	    if ( check_shield_block( ch, victim ) && dam > 0 )
	         return;
	    if ( check_dodge( ch, victim ) && dam > 0 )
		return;
	}
    }

    switch( check_ris( victim, dam_type ) )
    {
    case IS_RESISTANT:		dam -= dam / 3;				break;
    case IS_IMMUNE:		immune = TRUE; dam = 0;			break;
    case IS_SUSCEPTIBLE:	dam += dam / 2;				break;
    }

    /*
     * We moved dam_message out of the victim != ch if above
     * so self damage would show.  Other valid type_undefined
     * damage is ok to avoid like mortally wounded damage - Kahn
     */
    if ( dt != TYPE_UNDEFINED )
        dam_message( ch, victim, dam, dt, wpn, immune );

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC( victim )
	&& victim->level >= LEVEL_IMMORTAL
	&& victim->hit < 1 )
	victim->hit = 1;

    /*
     * Magic shields that retaliate
     */
    if ( ( dam > 1 ) && victim != ch )
    {
	/* For compatibility with old Envy2.2 areas & pfiles */
	if ( IS_AFFECTED( victim, AFF_FLAMING )
	    && !IS_AFFECTED( ch, AFF_FLAMING ) )
	    damage( victim, ch, dam/2, gsn_flame_shield, WEAR_NONE, DAM_FIRE );

	if ( is_affected( victim, gsn_frost_shield )
	    && !is_affected( ch, gsn_frost_shield ) )
	    damage( victim, ch, dam/2, gsn_frost_shield, WEAR_NONE, DAM_FIRE );

	if ( is_affected( victim, gsn_shock_shield )
	    && !is_affected( ch, gsn_shock_shield ) )
	    damage( victim, ch, dam/2, gsn_shock_shield, WEAR_NONE, DAM_FIRE );
    }

    if ( is_affected( victim, gsn_berserk )
	&& victim->position <= POS_STUNNED )
        affect_strip( victim, gsn_berserk );

    if ( dam > 0 && dt > TYPE_HIT
	&& is_wielding_poisoned( ch, wpn )
	&& !saves_spell( ch->level, victim, DAM_POISON ) )
    {
	AFFECT_DATA af;

	af.type      = gsn_poison;
	af.duration  = 1;
	af.location  = APPLY_STR;
	af.modifier  = -2;
	af.bitvector = AFF_POISON;
	affect_join( victim, &af );
    }

    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	send_to_char( 
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
	act( "$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	break;

    case POS_INCAP:
	send_to_char(
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
	act( "$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	break;

    case POS_STUNNED:
	send_to_char("You are stunned, but will probably recover.\n\r",
	    victim );
	act( "$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM );
	break;

    case POS_DEAD:
	send_to_char( "You have been KILLED!!\n\r\n\r", victim );
	act( "{o{r$n is DEAD!!{x", victim, NULL, NULL, TO_ROOM );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "That really did HURT!\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 )
	    send_to_char( "{o{rYou sure are BLEEDING!{x\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE( victim ) )		/* lets make NPC's not slaughter PC's */
    {
	if ( victim->fighting
	    && victim->fighting->hunting
	    && victim->fighting->hunting->who == victim )
	    stop_hunting( victim->fighting );

	if ( victim->fighting
	    && victim->fighting->hating
	    && victim->fighting->hating->who == victim )
	    stop_hating( victim->fighting );

        if ( victim->fighting
	    && !IS_NPC( victim )
	    && IS_NPC( ch ) )
          stop_fighting( victim, TRUE );
        else
          stop_fighting( victim, FALSE );
    }

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if ( !IS_NPC( victim ) )
	{
	    int  gold = 0;
	    int  exp  = 0;
	    char buf[ MAX_STRING_LENGTH ];

	    if ( !in_arena( victim ) )
	    {
		if ( IS_NPC( ch ) )
		{
		    victim->pcdata->mdeaths++;
		    if ( is_clan( victim ) )
			victim->pcdata->clan->mdeaths++;
		}
		else
		{
		    if ( !licensed( ch )
			|| ( ch->level - victim->level > 5
			    && victim->fighting != ch )
			|| !registered( ch, victim ) )
		    {
			ch->pcdata->illegal_pk++;

			if (   is_clan( ch )
			    && is_clan( victim )
			    && ch->pcdata->clan != victim->pcdata->clan )
			    ch->pcdata->clan->illegal_pk++;
		    }

		    ch->pcdata->pkills++;
		    victim->pcdata->pdeaths++;

		    if (   is_clan( ch )
			&& is_clan( victim )
			&& ch->pcdata->clan != victim->pcdata->clan )
		    {
			ch->pcdata->clan->pkills++;
			victim->pcdata->clan->pdeaths++;
		    }
		}
	    }

	    sprintf( log_buf, "%s killed by %s at %d",
		    victim->name,
		    ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
		    victim->in_room->vnum );
	    log_string( log_buf );

	    /*
	     * Dying penalty:
	     * 1/2 way back to previous 2 levels.
	     */
	    if ( IS_NPC( ch ) )
	    {
	        if ( !in_arena( victim )
		    && victim->exp > EXP_PER_LEVEL * ( victim->level - 1 ) )
		    gain_exp( victim, ( EXP_PER_LEVEL * ( victim->level - 1 ) 
				       - victim->exp ) / 4 );
	    }
	    else
	    {
	        if ( ch != victim )
		{
		    if ( IS_SET( victim->act, PLR_REGISTER ) )
		    {
			if ( !in_arena( victim ) )
			{
			    exp = number_range( 250, 750 );
			    gain_exp( victim, 0 - exp );
			    sprintf( buf, "You lose %d exps.\n\r", exp );
			    send_to_char( buf, victim );
			    if ( victim->exp < 1001 )
			    {
				send_to_char(
				    "You no longer have any exps to give.\n\r",
					     victim );
				send_to_char( "You are now DEAD.\n\r", victim );
				SET_BIT( victim->act, PLR_DENY );
			    }
			}
			else
	send_to_char( "You don't lose exps from fighting in an Arena.\n\r",
			victim );
		    }

		    gold = victim->gold * number_range( 10, 20 ) / 100;
		    ch->gold += gold;
		    victim->gold -= gold;
		    sprintf( buf, "You gain %d gold.\n\r", gold );
		    send_to_char( buf, ch );
		    sprintf( buf, "You lose %d gold.\n\r", gold );
		    send_to_char( buf, victim );
		}
	    }
		
	}
	else
	{
	    if ( !IS_NPC( ch )
		&& !in_arena( victim ) )
	    {
		ch->pcdata->mkills++;
		if ( is_clan( ch ) )
		    ch->pcdata->clan->mkills++;
	    }
	}

	raw_kill( ch, victim );

	if (   is_clan( ch )
	    && is_clan( victim )
	    && ch->pcdata->clan != victim->pcdata->clan
	    && ch->pcdata->clan->clan_type != CLAN_NOKILL
	    && victim->pcdata->clan->clan_type != CLAN_NOKILL )
	{
	    act( "{cBolts of blue energy rise from the corpse, seeping into $n.{x",  ch, NULL, NULL, TO_ROOM );
	    act( "{cBolts of blue energy rise from the corpse, seeping into you.{x", ch, NULL, NULL, TO_CHAR ); 

	    ch->pcdata->clan->score += 20;
	}

	/* 
	 * Ok, now we want to remove the deleted flag from the
	 * PC victim.
	 */
	if ( !IS_NPC( victim ) )
	    victim->deleted = FALSE;

	if ( !IS_NPC( ch ) && IS_NPC( victim ) )
	{
	    /* Autogold by Morpheus */
	    if ( IS_SET( ch->act, PLR_AUTOGOLD ) )
	        do_get( ch, "coins corpse" );  /* autogold mod by Canth */

	    if ( IS_SET( ch->act, PLR_AUTOLOOT ) )
		do_get( ch, "all corpse" );
	    else
		do_look( ch, "in corpse" );

	    if ( IS_SET( ch->act, PLR_AUTOSAC  ) )
		do_sacrifice( ch, "corpse" );
	}

	/*
	 * Remove victims of pk who no longer have exps left
	 */
	if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_DENY ) )
	{
	    do_quit( victim, "" );
	}

	return;
    }

    if ( victim == ch )
	return;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC( victim ) && !victim->desc )
    {
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, "" );
	    return;
	}
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC( victim ) && dam > 0 )
    {
	if ( ( IS_SET( victim->act, ACT_WIMPY ) && number_bits( 1 ) == 0
	      && victim->hit < victim->max_hit / 2 )
	    || ( IS_AFFECTED( victim, AFF_CHARM ) && victim->master
		&& victim->master->in_room != victim->in_room ) )
	    {
		start_fearing( victim, ch );
		stop_hunting( victim );
		do_flee( victim, "" );
	    }
    }

    if ( !IS_NPC( victim )
	&& victim->hit   > 0
	&& victim->hit  <= victim->wimpy
	&& victim->wait == 0 )
	do_flee( victim, "" );

    tail_chain( );
    return;
}



bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !IS_NPC( ch ) && IS_AFFECTED( ch, AFF_GHOUL ) )
    {
	send_to_char(
	     "You may not participate in combat while in ghoul form.\n\r",
		     ch );
	return TRUE;
    }

    if ( !IS_NPC( victim ) && IS_AFFECTED( victim, AFF_GHOUL ) )
    {
	act( "Your attack passes through $N.",  ch, NULL, victim, TO_CHAR    );
	act( "$n's attack passes through $N.",  ch, NULL, victim, TO_NOTVICT );
	act( "$n's attack passes through you.", ch, NULL, victim, TO_VICT    );
	return TRUE;
    }

    if ( IS_NPC( ch ) || IS_NPC( victim ) )
	return FALSE;

    if ( IS_SET( victim->act, PLR_KILLER )
	|| IS_SET( victim->act, PLR_THIEF ) )
        return FALSE;

    if ( in_arena( victim ) )
        return FALSE;

    if ( ch->level < 16 && !ch->fighting )
    {
	send_to_char(
	     "You may not pkill or psteal with or without registering.\n\r",
		     ch );
	return TRUE;
    }

    if ( victim->level >= LEVEL_HERO )
    {
	act( "$N is a HERO and is automatically safe.", ch, NULL, victim,
	    TO_CHAR );
        return TRUE;
    }

    if ( ch->level >= LEVEL_HERO )
    {
	send_to_char( "You may not participate in pkilling or pstealing.\n\r",
		     ch );
	return TRUE;
    }

    if ( victim->fighting )
        return FALSE;

    if ( IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
    {
	act( "$N is in a safe room.", ch, NULL, victim, TO_CHAR );
        return TRUE;
    }

    return FALSE;
}



/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    /*
     * NPC's are fair game.
     */
    if ( IS_NPC( victim ) )
	return;

    /*
     * NPC's are cool of course
     * Hitting yourself is cool too (bleeding).
     * Hitting immortals are fine.
     */
    if ( IS_NPC( ch )
	|| ch == victim
	|| victim->level > LEVEL_HERO )
	return;

    /*
     * KILLERs are fair game.
     * THIEVES are fair game too.
     * KILLER aggressors should not be penalized 1000 exps per attack but
     * per combat started.
     * ARENA combat is fair game.
     */
    if ( IS_SET( victim->act, PLR_KILLER )
	|| IS_SET( victim->act, PLR_THIEF )
	|| ( IS_SET( ch->act, PLR_KILLER ) && ch->fighting )
        || ( in_arena( ch ) && in_arena( victim ) ) )
        return;

    /*
     * Vampires are fair game.
     */
    if ( !str_cmp( race_table[victim->race].name, "Vampire" ) )
        return;

    if ( !licensed( ch ) )
    {
	send_to_char( "You are not licensed!  You lose 400 exps.\n\r", ch );
	gain_exp( ch, -400 );
	if ( registered( ch, victim )
	    && ch->level - victim->level < 6 )
	{
	    return;
	}
    }
	
    if ( ( ch->level - victim->level > 5 && victim->fighting != ch )
	|| !registered( ch, victim ) )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
        char      buf [ MAX_STRING_LENGTH ];

        send_to_char( "You are a KILLER!  You lose 600 exps.\n\r", ch );
	sprintf( buf, "Help!  I'm being attacked by %s!", ch->name );
	do_shout( victim, buf );
	SET_BIT( ch->act, PLR_KILLER );
	gain_exp( ch, -600 );
	demote_level( ch );

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	
	    if ( obj->deleted )
	        continue;
	    obj_from_char( obj );
	    
	    /*
	     * Remove item inventories
	     * Includes licenses to kill
	     */
	    if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    {
		extract_obj( obj );
		continue;
	    }

	    obj_to_char( obj, ch );
	}

	save_char_obj( ch );
    }

    return;
}

/*
 * Check to see if weapon is poisoned.
 */
bool is_wielding_poisoned( CHAR_DATA *ch, int wpn )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, wpn ) )
	&& IS_SET( obj->extra_flags, ITEM_POISONED ) )
        return TRUE;

    return FALSE;

}


/*
 * Hunting, Hating and Fearing code. -Thoric
 */
bool is_hunting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->hunting || ch->hunting->who != victim )
      return FALSE;
    
    return TRUE;    
}

bool is_hating( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->hating || ch->hating->who != victim )
      return FALSE;
    
    return TRUE;    
}

bool is_fearing( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->fearing || ch->fearing->who != victim )
      return FALSE;
    
    return TRUE;    
}

void stop_hunting( CHAR_DATA *ch )
{
    if ( ch->hunting )
    {
	free_string( ch->hunting->name );
	free_mem( ch->hunting, sizeof( HHF_DATA ) );
	ch->hunting = NULL;
    }
    return;
}

void stop_hating( CHAR_DATA *ch )
{
    if ( ch->hating )
    {
	free_string( ch->hating->name );
	free_mem( ch->hating, sizeof( HHF_DATA ) );
	ch->hating = NULL;
    }
    return;
}

void stop_fearing( CHAR_DATA *ch )
{
    if ( ch->fearing )
    {
	free_string( ch->fearing->name );
	free_mem( ch->fearing, sizeof( HHF_DATA ) );
	ch->fearing = NULL;
    }
    return;
}

void start_hunting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->hunting )
      stop_hunting( ch );

    ch->hunting = alloc_mem( sizeof( HHF_DATA ) );
    ch->hunting->name = str_dup( victim->name );
    ch->hunting->who  = victim;
    return;
}

void start_hating( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->hating )
      stop_hating( ch );

    ch->hating = alloc_mem( sizeof( HHF_DATA ) );
    ch->hating->name = str_dup( victim->name );
    ch->hating->who  = victim;
    return;
}

void start_fearing( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fearing )
      stop_fearing( ch );

    ch->fearing = alloc_mem( sizeof( HHF_DATA ) );
    ch->fearing->name = str_dup( victim->name );
    ch->fearing->who  = victim;
    return;
}

/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE( victim ) )
        return FALSE;

    if (  IS_NPC(   victim ) )
    {
        /* Tuan was here.  :) */
        chance  = UMIN( 60, 2 * victim->level );
        if ( !get_eq_char( victim, WEAR_WIELD ) )
        {
            if ( !get_eq_char( victim, WEAR_WIELD_2 ) )
                chance /= 2;
            else
                chance  = 3 * chance / 4;
        }

    }
    else
    {
        if ( !get_eq_char( victim, WEAR_WIELD ) )
        {
            if ( !get_eq_char( victim, WEAR_WIELD_2 ) )
                return FALSE;
            chance = victim->pcdata->learned[gsn_parry] / 4;
        }
        else
            chance = victim->pcdata->learned[gsn_parry] / 2;
    }

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    act( "{o{g$N parries your attack.{x", ch, NULL, victim, TO_CHAR );
    act( "{o{gYou parry $n's attack.{x",  ch, NULL, victim, TO_VICT );
    return TRUE;
}



/*
 * Check for block.
 */
bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;
 
    if ( !IS_AWAKE( victim ) )
        return FALSE;
 
    if ( !get_eq_char( victim, WEAR_SHIELD ) )
	return FALSE;

    if ( IS_NPC( victim ) )
	/* Zen was here.  :) */
        chance  = UMIN( 60, 2 * victim->level );
    else
	chance  = victim->pcdata->learned[gsn_shield_block] / 2;
 
    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;
 
    act( "{o{gYou block $n's attack with your shield.{x", ch, NULL, victim, TO_VICT );
    act( "{o{g$N blocks your attack with a shield.{x",    ch, NULL, victim, TO_CHAR    );
    return TRUE;
}



/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE( victim ) )
	return FALSE;

    if ( IS_NPC( victim ) )
	/* Tuan was here.  :) */
        chance  = UMIN( 60, 2 * victim->level );
    else
        chance  = victim->pcdata->learned[gsn_dodge] / 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    act( "{o{g$N dodges your attack.{x", ch, NULL, victim, TO_CHAR    );
    act( "{o{gYou dodge $n's attack.{x", ch, NULL, victim, TO_VICT    );
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	    victim->position = POS_STANDING;
	return;
    }

    if ( IS_NPC( victim ) || victim->hit <= -11 )
    {
	if ( victim->riding )
	{
	    act( "$n falls from $N.", victim, NULL, victim->riding, TO_ROOM );
	    victim->riding->rider = NULL;
	    victim->riding        = NULL;
	}

	victim->position = POS_DEAD;
	return;
    }

         if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    if ( victim->riding )
    {
	act( "$n falls unconscious from $N.", victim, NULL, victim->riding, TO_ROOM );
	victim->riding->rider = NULL;
	victim->riding        = NULL;
    }

    return;
}



/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{

    char buf [ MAX_STRING_LENGTH ];

    if ( ch->fighting )
    {
	bug( "Set_fighting: already fighting", 0 );
	sprintf( buf, "...%s attacking %s at %d", 
		( IS_NPC( ch )     ? ch->short_descr     : ch->name     ),
		( IS_NPC( victim ) ? victim->short_descr : victim->name ),
		victim->in_room->vnum );
	bug( buf, 0 );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_SLEEP ) )
	affect_strip( ch, gsn_sleep );

    ch->fighting = victim;
    ch->position = POS_FIGHTING;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    for ( fch = char_list; fch; fch = fch->next )
    {
	if ( fch == ch || ( fBoth && fch->fighting == ch ) )
	{
	    fch->fighting	= NULL;
	    fch->position	= POS_STANDING;
            if ( is_affected( fch, gsn_berserk ) )
                affect_strip( fch, gsn_berserk );
	    update_pos( fch );
	}
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    OBJ_DATA        *corpse;
    OBJ_DATA        *obj;
    OBJ_DATA        *obj_next;
    ROOM_INDEX_DATA *location;
    char            *name;
    char             buf [ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch ) )
    {
	/*
	 * This longwinded corpse creation routine comes about because
	 * we dont want anything created AFTER a corpse to be placed  
	 * INSIDE a corpse.  This had caused crashes from obj_update()
	 * in extract_obj() when the updating list got shifted from
	 * object_list to obj_free.          --- Thelonius (Monk)
	 */

        name              = ch->short_descr;
	if ( ch->gold > 0 )
	{
	    OBJ_DATA * coins;
	    
	    coins         = create_money( ch->gold );
	    corpse        = create_object(
					  get_obj_index( OBJ_VNUM_CORPSE_NPC ),
					  0 );
	    obj_to_obj( coins, corpse );
	    ch->gold = 0;
	}
	else
	{
	    corpse        = create_object(
					  get_obj_index( OBJ_VNUM_CORPSE_NPC ),
					  0 );
	}

	corpse->timer = number_range( 2, 4 );
    }
    else
    {
        name          = ch->name;
	corpse        = create_object( get_obj_index( OBJ_VNUM_CORPSE_PC ),
				   0 );
	corpse->timer = number_range( 25, 40 );
    }

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );
	
    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    for ( obj = ch->carrying; obj; obj = obj_next )
    {
	obj_next = obj->next_content;
	
	if ( obj->deleted )
	  continue;
	obj_from_char( obj );

	REMOVE_BIT( obj->extra_flags, ITEM_VIS_DEATH );

	/*
	 * Remove item inventories from all corpses.
	 * Includes licenses to kill
	 */
	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	{
	    extract_obj( obj );
	}
	else
	{
	    if ( IS_NPC( ch ) )     /* remove this line and else{} below    */
	    {                       /* if you want player to not have their */
	        obj_to_obj( obj, corpse ); /* eq upon death - Kahn          */
	    }
	    else
	    {
		obj_to_char( obj, ch );
	    }
	}
    }

    if ( IS_NPC( ch ) )
    {
	obj_to_room( corpse, ch->in_room );
    }
    else
    {
	if ( !( location = get_room_index( ROOM_VNUM_GRAVEYARD_A ) ) )
	{
	    bug( "GRAVEYARD A does not exist!", 0 );
	    obj_to_room( corpse, ch->in_room );
	}
	else
	  obj_to_room( corpse, location );
    }

    return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char            *msg;
    char             mesg [ MAX_STRING_LENGTH ];
    int              vnum;
    int              door;
    int              parts;

    vnum = 0;
    
    msg   = "You hear $n's death cry.";
    parts = race_table[ch->race].parts;

    switch ( number_bits( 4 ) )
    {
    default: msg  = "You hear $n's death cry.";				break;
    case  0: msg  = "$n hits the ground ... DEAD.";			break;
    case  1: msg  = "$n splatters blood on your armor.";		break;
    case  2: msg  = "You smell $n's sphincter releasing in death.";
	     vnum = OBJ_VNUM_FINAL_TURD;				break;
    case  3: if ( IS_SET( parts, PART_GUTS ) )
	     {
		msg = "$n spills $s guts all over the floor.";
		vnum = OBJ_VNUM_GUTS;
	     }								break;
    case  4: if ( IS_SET( parts, PART_HEAD ) )
	     {
		msg  = "$n's severed head plops on the ground.";
		vnum = OBJ_VNUM_SEVERED_HEAD;				
	     }								break;
    case  5: if ( IS_SET( parts, PART_HEART ) )
	     {
		msg  = "$n's heart is torn from $s chest.";
		vnum = OBJ_VNUM_TORN_HEART;				
	     }								break;
    case  6: if ( IS_SET( parts, PART_ARMS ) )
	     {
		msg  = "$n's arm is sliced from $s dead body.";
		vnum = OBJ_VNUM_SLICED_ARM;				
	     }								break;
    case  7: if ( IS_SET( parts, PART_LEGS ) )
	     {
		msg  = "$n's leg is sliced from $s dead body.";
		vnum = OBJ_VNUM_SLICED_LEG;				
	     }								break;
    case  8: if ( IS_SET( parts, PART_BRAINS ) )
	     {
		msg = "$n's head is shattered, and $s brains splash all over you.";
		vnum = OBJ_VNUM_BRAINS;
	     }								break;
    }

    sprintf( mesg, "{r%s{x", msg );
    act( mesg, ch, NULL, NULL, TO_ROOM );

    if ( vnum != 0 )
    {
	OBJ_DATA *obj;
	char     *name;
	char      buf [ MAX_STRING_LENGTH ];

	name		= IS_NPC( ch ) ? ch->short_descr : ch->name;
	obj		= create_object( get_obj_index( vnum ), 0 );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	if ( IS_AFFECTED( ch, AFF_POISON ) )
	    obj->value[3] = 1;

	obj_to_room( obj, ch->in_room );
    }

    if ( IS_NPC( ch ) )
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] )
	    && pexit->to_room
	    && pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;

    return;
}



void raw_kill( CHAR_DATA *ch, CHAR_DATA *victim )
{
    AFFECT_DATA *paf;

    stop_fighting( victim, TRUE );
    if ( ch != victim )
	mprog_death_trigger( victim );
    make_corpse( victim );

    if ( victim->rider )
    {
        act( "$n dies suddenly, and you fall to the ground.", 
	    victim, NULL, victim->rider, TO_VICT );
        victim->rider->riding	  = NULL;
        victim->rider->position	  = POS_RESTING;
        victim->rider		  = NULL;
    }

    if ( victim->riding )
    {
        act( "$n falls off you, dead.", 
	    victim, NULL, victim->riding, TO_VICT );
        victim->riding->rider	   = NULL;
        victim->riding		   = NULL;
    }

    if ( !IS_NPC( victim ) && IS_AFFECTED( victim, AFF_VAMP_BITE ) )
        victim->race = race_lookup( "Vampire" );

    for ( paf = victim->affected; paf; paf = paf->next )
    {
	if ( paf->deleted )
	    continue;

	/* Keep the ghoul affect */
	if ( !IS_NPC( victim ) && IS_AFFECTED( victim, AFF_GHOUL ) )
	    continue;

        affect_remove( victim, paf );
    }

    if ( IS_NPC( victim ) )
    {
	victim->pIndexData->killed++;
	kill_table[URANGE( 0, victim->level, MAX_LEVEL-1 )].killed++;
	extract_char( victim, TRUE );
	return;
    }

    extract_char( victim, FALSE );
    victim->armor        = 100;
    victim->position     = POS_RESTING;
    victim->hit	         = UMAX( 1, victim->hit  );
    victim->mana         = UMAX( 1, victim->mana );
    victim->move         = UMAX( 1, victim->move );
    victim->hitroll      = 0;
    victim->damroll      = 0;
    victim->saving_throw = 0;
    victim->pcdata->mod_str = 0;
    victim->pcdata->mod_int = 0;
    victim->pcdata->mod_wis = 0;
    victim->pcdata->mod_dex = 0;
    victim->pcdata->mod_con = 0;

    /*
     * Pardon crimes... -Thoric
     */
    if ( IS_SET( victim->act, PLR_KILLER ) )
    {
      REMOVE_BIT( victim->act, PLR_KILLER );
      send_to_char( "The gods have pardoned you for your murderous acts.\n\r", victim );
    }

    if ( IS_SET( victim->act, PLR_THIEF ) )
    {
      REMOVE_BIT( victim->act, PLR_THIEF );
      send_to_char( "The gods have pardoned you for your thievery.\n\r", victim );
    }

    if ( !str_cmp( race_table[ch->race].name, "Vampire" ) )
    {
	victim->pcdata->condition[COND_FULL  ] = 0;
	victim->pcdata->condition[COND_THIRST] = 0;
    }

    save_char_obj( victim );
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    char       buf[ MAX_STRING_LENGTH ];
    int        members;
    int        xp;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( IS_NPC( ch ) || victim == ch )
	return;
    
    members = 0;
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
    }

    lch = ( ch->leader ) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if ( !is_same_group( gch, ch ) )
	    continue;

	if ( gch->level - lch->level >= 6 )
	{
	    send_to_char( "{o{rYou are too high level for this group.{x\n\r", gch );
	    continue;
	}

	if ( gch->level - lch->level <= -6 )
	{
	    send_to_char( "{o{rYou are too low level for this group.{x\n\r",  gch );
	    continue;
	}

        if ( in_arena( victim ) )
        {
	    send_to_char( "{o{rNo exps from fighting in an Arena.{x\n\r",  gch );
	    continue;
        }

	xp = xp_compute( gch, victim ) / members;

	if ( !str_infix( race_table[lch->race].name,
			race_table[gch->race].hate) && members > 1 )
	  {
	    send_to_char( "{o{rYou lost a third of your exps due to grouping with scum.{x\n\r", ch );
	    xp -= xp/3;
	  }
	sprintf( buf, "{o{rYou receive %d experience points.{x\n\r", xp );
	send_to_char( buf, gch );
	gain_exp( gch, xp );

	for ( obj = gch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->deleted )
	        continue;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

	    if (   ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL    )
		    && IS_EVIL   ( gch ) )
		|| ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD    )
		    && IS_GOOD   ( gch ) )
		|| ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL )
		    && IS_NEUTRAL( gch ) ) )
	    {
		act( "You are zapped by $p.", gch, obj, NULL, TO_CHAR );
		act( "$n is zapped by $p.",   gch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, gch->in_room );
	    }
	}
    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;
    double    bonus;
    int       xp;
    int       align;
    int       extra;
    int       level;
    int       number;

    bonus = 1.0;
    xp    = 105 - URANGE( -4, gch->level - victim->level, 6 ) * 25;
    align = gch->alignment - victim->alignment;

    if ( align >  500 )
    {
	gch->alignment  = UMIN( gch->alignment + ( align - 500 ) / 4,  1000 );
	xp = 5 * xp / 4;
    }
    else if ( align < -500 )
    {
	gch->alignment  = UMAX( gch->alignment + ( align + 500 ) / 4, -1000 );
	xp = 5 * xp / 4;
    }
    else
    {
	gch->alignment -= gch->alignment / 4;
	xp = 3 * xp / 4;
    }

    if ( IS_AFFECTED( victim, AFF_SANCTUARY )
	|| IS_SET( race_table[ victim->race ].race_abilities, RACE_SANCT ) )
    {
	bonus += 1.0/2.0;
    }

    if ( IS_AFFECTED( victim, AFF_FLAMING ) )
    {
	bonus += 4.0/10.0;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) )
    {
	bonus += 1.0/4.0;
    }
    
    if ( ( obj = get_eq_char( victim, WEAR_WIELD_2 ) ) )
    {
	bonus += 1.0/5.0;
    }

    if ( !str_infix( race_table[victim->race].name,
		    race_table[gch->race].hate ) )
    {
	bonus += 1.0/10.0;
    }
    
    if ( victim->race == gch->race )
    {
	bonus -= 1.0/8.0;
    }

    if ( IS_NPC( victim ) )
    {
	if ( IS_SET( victim->act, ACT_AGGRESSIVE ) )
	{
	    bonus += 1.0/20.0;
	}

	if ( victim->pIndexData->pShop != 0 )
	    bonus -= 1.0/4.0;

	if ( victim->spec_fun != 0 )
	{
	    if (   victim->spec_fun == spec_mob_lookup( "spec_breath_any"       )
		|| victim->spec_fun == spec_mob_lookup( "spec_cast_psionicist"  )
		|| victim->spec_fun == spec_mob_lookup( "spec_cast_undead"      )
		|| victim->spec_fun == spec_mob_lookup( "spec_breath_gas"       )
		|| victim->spec_fun == spec_mob_lookup( "spec_cast_mage"        ) )
	        bonus += 1.0/3.0;

	    if (   victim->spec_fun == spec_mob_lookup( "spec_breath_fire"      )
		|| victim->spec_fun == spec_mob_lookup( "spec_breath_cold"      )
		|| victim->spec_fun == spec_mob_lookup( "spec_breath_acid"      )
		|| victim->spec_fun == spec_mob_lookup( "spec_breath_lightning" )
		|| victim->spec_fun == spec_mob_lookup( "spec_cast_cleric"      )
		|| victim->spec_fun == spec_mob_lookup( "spec_cast_judge"       )
		|| victim->spec_fun == spec_mob_lookup( "spec_cast_ghost"       ) )
	        bonus += 1.0/5.0;

	    if (   victim->spec_fun == spec_mob_lookup( "spec_poison"           )
		|| victim->spec_fun == spec_mob_lookup( "spec_thief"            ) )
	        bonus += 1.0/20.0;

	    if ( victim->spec_fun == spec_mob_lookup( "spec_cast_adept"         ) )
	        bonus -= 1.0/2.0;
	}
    }
    else
    {
        if ( !IS_SET( victim->act, PLR_REGISTER ) )
	    bonus  = 0.0;
	else
	    bonus *= 2.0;
    }
    xp = (int) ( xp * bonus );

    /*
     * Adjust for popularity of target:
     *   -1/8 for each target over  'par' (down to - 50%)
     *   +1/8 for each target under 'par' (  up to + 25%)
     */
    if ( IS_NPC( victim ) )
    {
	level  = URANGE( 0, victim->level, MAX_LEVEL - 1 );
	number = UMAX( 1, kill_table[level].number );
	extra  = victim->pIndexData->killed - kill_table[level].killed
	  / number;
	xp    -= xp * URANGE( -2, extra, 4 ) / 8;
    }

    xp     = number_range( xp * 3 / 4, xp * 5 / 4 );
    xp     = UMAX( 0, xp );

    if ( !IS_NPC( victim ) )
        xp = UMIN( xp, 250 );

    return xp;
}



void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt,
		 int wpn, bool immune )
{
    const  char         *vp;
    const  char         *attack;
           char          buf            [ MAX_STRING_LENGTH ];
           char          buf1           [ 256 ];
           char          buf2           [ 256 ];
           char          buf3           [ 256 ];
           char          buf4           [ 256 ];
           char          buf5           [ 256 ];
           char          punct;

	     if ( dam ==   0 ) { vp = "misses";              }
    else
    {
        dam *= 100;
	if ( victim->hit > 0 )
	    dam /= victim->hit;

             if ( dam <=   1 ) { vp = "scratches";           }
	else if ( dam <=   2 ) { vp = "grazes";              }
	else if ( dam <=   4 ) { vp = "hits";                }
	else if ( dam <=   6 ) { vp = "injures";             }
	else if ( dam <=   8 ) { vp = "wounds";              }
	else if ( dam <=  10 ) { vp = "mauls";               }
	else if ( dam <=  20 ) { vp = "decimates";           }
	else if ( dam <=  30 ) { vp = "devastates";          }
	else if ( dam <=  40 ) { vp = "maims";               }
	else if ( dam <=  50 ) { vp = "MUTILATES";           }
	else if ( dam <=  60 ) { vp = "DISEMBOWELS";         }
	else if ( dam <=  70 ) { vp = "EVISCERATES";         }
	else if ( dam <=  80 ) { vp = "MASSACRES";           }
	else if ( dam <=  90 ) { vp = "*** DEMOLISHES ***";  }
	else                   { vp = "*** ANNIHILATES ***"; }
    }

    punct   = ( dam <= 10 ) ? '.' : '!';

    if ( dt == TYPE_HIT )
    {
        if ( ch->race > MAX_RACE )
	{
	    bug( "Dam_message:  %d invalid race", ch->race );
	    ch->race = 0;
	}

        attack = race_table[ch->race].dmg_message;

	sprintf( buf1, "{o{wYour %s %s $N%c{x",	     attack, vp, punct );
	sprintf( buf2, "{o{y$n's %s %s you%c{x",     attack, vp, punct );
	sprintf( buf3, "$n's %s %s $N%c",	     attack, vp, punct );
	sprintf( buf4, "{o{yYou %s %s yourself%c{x", attack, vp, punct );
	sprintf( buf5, "$n's %s %s $m%c",	     attack, vp, punct );
    }
    else
    {
	if ( dt >= 0 && dt < MAX_SKILL )
	    attack	= skill_table[dt].noun_damage;
	else if (   dt >= TYPE_HIT
		 && dt  < TYPE_HIT + MAX_ATTACK )
	    attack	= attack_table[dt - TYPE_HIT].name;
	else
	{
	    sprintf( buf, "Dam_message: bad dt %d caused by %s.", dt,
		    ch->name );
	    bug( buf, 0 );
	    dt      = TYPE_HIT;
	    attack  = attack_table[0].name;
	}

	if ( immune )
	{
	    sprintf( buf1, "{o{w$N seems unaffected by your %s!{x",	 attack );
	    sprintf( buf2, "{o{y$n's %s seems powerless against you.{x", attack );
	    sprintf( buf3, "$N seems unaffected by $n's %s!",		 attack );
	    sprintf( buf4, "{o{y$n seems unaffected by $s own %s.{x",	 attack );
	    sprintf( buf5, "Luckily, you seem immune to %s.",		 attack );
	}
	else
	{
	    if ( dt > TYPE_HIT && is_wielding_poisoned( ch, wpn ) )
	    {
		sprintf( buf1, "{o{wYour poisoned %s %s $N%c{x",  attack, vp, punct );
		sprintf( buf2, "{o{y$n's poisoned %s %s you%c{x", attack, vp, punct );
		sprintf( buf3, "$n's poisoned %s %s $N%c",	  attack, vp, punct );
		sprintf( buf4, "{o{yYour poisoned %s %s you%c{x", attack, vp, punct );
		sprintf( buf5, "$n's poisoned %s %s $m%c",	  attack, vp, punct );
	    }
	    else
	    {
		sprintf( buf1, "{o{wYour %s %s $N%c{x",	 attack, vp, punct );
		sprintf( buf2, "{o{y$n's %s %s you%c{x", attack, vp, punct );
		sprintf( buf3, "$n's %s %s $N%c",	 attack, vp, punct );
		sprintf( buf4, "{o{yYour %s %s you%c{x", attack, vp, punct );
		sprintf( buf5, "$n's %s %s $m%c",	 attack, vp, punct );
	    }
	}
    }

    if ( victim != ch )
    {
	act( buf1, ch, NULL, victim, TO_CHAR    );
	act( buf2, ch, NULL, victim, TO_VICT    );
	act( buf3, ch, NULL, victim, TO_NOTVICT );
    }
    else
    {
	act( buf4, ch, NULL, victim, TO_CHAR    );
	act( buf5, ch, NULL, victim, TO_ROOM    );
    }
      
    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;

    if ( ( race_table[ ch->race ].size - race_table[ victim->race ].size )
	< -2 )
        return;

    if ( !( obj = get_eq_char( victim, WEAR_WIELD ) ) )
	if ( !( obj = get_eq_char( victim, WEAR_WIELD_2 ) ) )
	    return;

    if ( IS_SET( obj->extra_flags, ITEM_NOREMOVE )
	|| IS_SET( obj->extra_flags, ITEM_NODROP ) )
	return;

    if (   !get_eq_char( ch, WEAR_WIELD   )
	&& !get_eq_char( ch, WEAR_WIELD_2 )
	&& number_bits( 1 ) == 0 )
	return;

    act( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n DISARMS you!", ch, NULL, victim, TO_VICT    );
    act( "$n DISARMS $N!",  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );
    if ( IS_NPC( victim ) )
	obj_to_char( obj, victim );
    else
	obj_to_room( obj, victim->in_room );

    return;
}



/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( IS_AFFECTED( victim, AFF_FLYING )
	|| IS_SET( race_table[ victim->race ].race_abilities, RACE_FLY ) )
        return;

    if ( victim->riding )
    {
	if ( IS_AFFECTED( victim->riding, AFF_FLYING )
	    || IS_SET( race_table[ victim->riding->race ].race_abilities, RACE_FLY ) )
	return;

        act( "{o{g$n trips your mount and you fall off!{x", ch, NULL, victim, TO_VICT    );
        act( "{o{gYou trip $N's mount and $N falls off!{x", ch, NULL, victim, TO_CHAR    );
        act( "{o{g$n trips $N's mount and $N falls off!{x", ch, NULL, victim, TO_NOTVICT );
	victim->riding->rider = NULL;
        victim->riding        = NULL;

	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
	victim->position = POS_RESTING;
        return;         
    }

    if ( victim->wait == 0 )
    {
	act( "{o{gYou trip $N and $N goes down!{x", ch, NULL, victim, TO_CHAR    );
	act( "{o{g$n trips you and you go down!{x", ch, NULL, victim, TO_VICT    );
	act( "{o{g$n trips $N and $N goes down!{x", ch, NULL, victim, TO_NOTVICT );

	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
	victim->position = POS_RESTING;
    }

    return;
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	if (   !IS_SET( victim->act, PLR_KILLER )
	    && !IS_SET( victim->act, PLR_THIEF  ) )
	{
	    send_to_char( "You must MURDER a player.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( IS_AFFECTED( victim, AFF_CHARM ) && victim->master )
	{
	    send_to_char( "You must MURDER a charmed creature.\n\r", ch );
	    return;
	}
    }

    if ( victim == ch )
    {
	send_to_char( "You hit yourself.  Ouch!\n\r", ch );
	multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED( ch, AFF_CHARM ) && !IS_NPC( victim ) )
    {
        send_to_char( "You cannot start a fight with a player!\n\r", ch );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}


void do_murder( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Murder whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) && !IS_AFFECTED( victim, AFF_CHARM )
	&& !victim->master )
    {
        do_kill( ch, arg );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "You hit yourself.  Ouch!\n\r", ch );
	multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }

    if ( is_safe( ch, victim ) )
        return;

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;

}

/*
 * I'm only allowing backstabbing with the primary weapon...immortals
 * who wield two weapons, with the first not being a dagger, will be
 * unable to backstab or circle.  Tough cookie.  --- Thelonius
 */
void do_backstab( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch )
	&& ch->level < skill_table[gsn_backstab].skill_level[ch->class] )
    {
	send_to_char(
	    "You better leave the assassin trade to thieves.\n\r", ch );
	return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't get close enough while mounted.\n\r", ch );
        return;
    }

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Backstab whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( !( obj = get_eq_char( ch, WEAR_WIELD ) )
	|| (*attack_table[obj->value[3]].wpn_gsn) != gsn_pierce )
    {
	send_to_char( "You need to wield a piercing weapon.\n\r", ch );
	return;
    }

    if ( victim->fighting )
    {
	send_to_char( "You can't backstab a fighting person.\n\r", ch );
	return;
    }

    if ( IS_AWAKE( victim ) && victim->hit < victim->max_hit )
    {
	act( "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    if ( !IS_AWAKE( victim )
	|| IS_NPC( ch )
	|| number_percent( ) < ch->pcdata->learned[gsn_backstab] )
	multi_hit( ch, victim, gsn_backstab );
    else
	damage( ch, victim, 0, gsn_backstab, WEAR_WIELD, DAM_PIERCE );

    return;
}



void do_circle( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *rch;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch )
	&& ch->level < skill_table[gsn_circle].skill_level[ch->class] )
    {
	send_to_char(
	    "You'd better leave the assassin trade to thieves.\n\r", ch );
	return;
    }

    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	return;

    if ( !ch->fighting )
    {
	send_to_char( "You must be fighting in order to do that.\n\r", ch );
	return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't circle while mounted.\n\r", ch );
        return;
    }

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
	victim = ch->fighting;
    else
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

    if ( victim == ch )
    {
	send_to_char( "You spin around in a circle.  Whee!\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( victim != ch->fighting )
    {
	send_to_char( "One fight at a time.\n\r", ch );
	return;
    }

    if ( !victim->fighting )
    {
	act( "Why?  $E isn't bothering anyone.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !is_same_group( ch, victim->fighting ) )
    {
	send_to_char( "Why call attention to yourself?\n\r", ch );
	return;
    }

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	if ( rch->fighting == ch )
	    break;

    if ( rch )
    {
	send_to_char( "You're too busy being hit right now.\n\r", ch );
	return;
    }

    if ( !( obj = get_eq_char( ch, WEAR_WIELD ) )
	|| obj->value[3] != 11 )
    {
	send_to_char( "You need to wield a piercing weapon.\n\r", ch );
	return;
    }

    act( "You circle around behind $N...", ch, NULL, victim, TO_CHAR    );
    act( "$n circles around behind $N...", ch, NULL, victim, TO_NOTVICT );
    
    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[gsn_circle].beats );

    if ( IS_NPC( ch )
	|| number_percent( ) < ch->pcdata->learned[gsn_circle] / 2 )
      {
	stop_fighting( victim, FALSE );
	multi_hit( ch, victim, gsn_circle );
      }
    else
        act( "You failed to get around $M", ch, NULL, victim, TO_CHAR );

    return;
}



void do_flee( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    int              attempt;

    if ( !( victim = ch->fighting ) )
    {
	if ( ch->position == POS_FIGHTING )
	    ch->position = POS_STANDING;
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_berserk ) )
    {
	send_to_char( "You can't flee, you're BERSERK!\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_HOLD ) ) 
    {
	send_to_char( "You are stuck in a snare!  You can't move!\n\r", ch );
	act( "$n wants to flee, but is caught in a snare!",
	    ch, NULL, NULL, TO_ROOM );
	return;
    }

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
	EXIT_DATA *pexit;
	int        door;

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == 0
	    ||   !pexit->to_room
	    ||   IS_SET( pexit->exit_info, EX_CLOSED )
	    || ( IS_NPC( ch )
		&& ( IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
		    || ( IS_SET( ch->act, ACT_STAY_AREA )
			&& pexit->to_room->area != ch->in_room->area ) ) ) )
	    continue;

	if ( ch->riding && ch->riding->fighting )
	    stop_fighting( ch->riding, TRUE );

	move_char( ch, door );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( "$n has fled!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;

	if ( !IS_NPC( ch ) )
	{
	    send_to_char( "You flee from combat!  You lose 25 exps.\n\r", ch );
	    gain_exp( ch, -25 );
	}

	stop_fighting( ch, TRUE );
	return;
    }

    send_to_char( "You failed!  You lose 10 exps.\n\r", ch );
    gain_exp( ch, -10 );
    return;
}



void do_berserk( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    /* Don't allow charmed mobs to do this, check player's level */
    if ( ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	|| ( !IS_NPC( ch )
	    && ch->level < skill_table[gsn_berserk].skill_level[ch->class] ) )
    {
	send_to_char( "You're not enough of a warrior to go Berserk.\n\r",
		     ch );
	return;
    }

    if ( !ch->fighting )                                         
    {                                   
        send_to_char( "You aren't fighting anyone.\n\r", ch );
        return;                                                      
    }                                                                       

    if ( is_affected( ch, gsn_berserk ) )
	return;

    send_to_char(
		 "Your weapon hits your foe and blood splatters all over!\n\r",
		 ch );
    send_to_char( "The taste of blood begins to drive you crazy!\n\r",
		 ch );

    if ( IS_NPC( ch )
	|| number_percent( ) < ch->pcdata->learned[gsn_berserk] )
    {
	af.type	     = gsn_berserk;
	af.duration  = -1;
	af.location  = APPLY_HITROLL;
	af.modifier  = UMIN( ch->level / 4, 8 );
	af.bitvector = 0;
	affect_to_char( ch, &af );

	af.location  = APPLY_DAMROLL;
	affect_to_char( ch, &af );

	af.location  = APPLY_AC;
	af.modifier  = ch->level;
	affect_to_char( ch, &af );

	send_to_char( "You have gone BERSERK!\n\r", ch );
	act( "$n has gone BERSERK!", ch, NULL, NULL, TO_ROOM );

	return;
    }
    send_to_char( "You shake off the madness.\n\r", ch );

    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *fch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        count;

    /* Don't allow charmed mobs to do this, check player's level */
    if ( ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	|| ( !IS_NPC( ch )
	    && ch->level < skill_table[gsn_rescue].skill_level[ch->class] ) )
    {
	send_to_char(
	    "You'd better leave the heroic acts to warriors.\n\r", ch );
	return;
    }

    if ( is_affected( ch, gsn_berserk ) )
    {
	send_to_char( "You can't rescue anyone, you're BERSERK!\n\r", ch );
	return;
    }

    if ( ch->riding )
    {
        send_to_char( "You can't do that while mounted.\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about fleeing instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && IS_NPC( victim ) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( !victim->fighting )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    if ( !is_same_group( ch, victim ) )
    {
	send_to_char( "Why would you want to?\n\r", ch );
	return;
    }

    if ( !check_blind ( ch ) )
        return;

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );

    count = 0;
    for ( fch = victim->in_room->people; fch; fch = fch->next_in_room )
    {

	if ( !IS_NPC( fch )
	    || fch->deleted )
	    continue;

	if ( fch->fighting == victim )
	{
	    if ( number_range( 0, count ) == 0 )
	        break;
	    count++;
	}
    }

    if ( !fch
	|| ( !IS_NPC( ch )
	    && number_percent( ) > ch->pcdata->learned[gsn_rescue] ) )
    {
	send_to_char( "You fail the rescue.\n\r", ch );
	return;
    }

    act( "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n rescues you!", ch, NULL, victim, TO_VICT    );
    act( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );

    stop_fighting( fch, FALSE );

    set_fighting( fch, ch );

    return;
}



void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    /* Don't allow charmed mobs to do this, check player's level */
    if ( ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	|| ( !IS_NPC( ch )
	    && ch->level < skill_table[gsn_kick].skill_level[ch->class] ) )
    {
	send_to_char(
	    "You'd better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
    {
        CHAR_DATA * vch;

        if ( is_affected( ch, gsn_berserk )
	    && ( vch = get_char_room( ch, arg ) ) != victim ) 
        {
            send_to_char( "You can't!  You're in a fight to the death!\n\r",
                                                                ch );
            return;
        }

        if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}


    }

    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_kick] )
	damage( ch, victim, number_range( 1, ch->level ), gsn_kick,
	       WEAR_NONE, DAM_BASH );
    else
	damage( ch, victim, 0, gsn_kick, WEAR_NONE, DAM_BASH );

    return;
}


void do_dirt( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        percent;

    /* Don't allow charmed mobs to do this, check player's level */
    if ( ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
        || ( !IS_NPC( ch )
            && ch->level < skill_table[gsn_dirt].skill_level[ch->class] ) )
    {
        send_to_char( "You get your feet dirty.\n\r", ch );
        return;
    }

    if ( !ch->fighting )
    {
        send_to_char( "You aren't fighting anyone.\n\r", ch );
        return;
    }

    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
    {
        if ( !( victim = get_char_room( ch, arg ) ) )
        {
            send_to_char( "They aren't here.\n\r", ch );
            return;
        }
    }

    if ( victim == ch )
    {
	send_to_char( "Very funny.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( victim, AFF_BLIND ) )
    {
	act( "$E's already been blinded.", ch, NULL, victim, TO_CHAR );
	return;
    }

    percent = ( ch->level - victim->level ) * 2;

    percent += get_curr_dex( ch ) - 2 * get_curr_dex( victim );

    switch( ch->in_room->sector_type )
    {
	case SECT_INSIDE:		percent -= 20; break;
	case SECT_CITY:			percent -= 10; break;
	case SECT_FIELD:		percent +=  5; break;
	case SECT_FOREST:			       break;
	case SECT_HILLS:			       break;
	case SECT_MOUNTAIN:		percent -= 10; break;
	case SECT_UNDERWATER:		percent  =  0; break;
	case SECT_WATER_SWIM:		percent  =  0; break;
	case SECT_WATER_NOSWIM:		percent  =  0; break;
	case SECT_AIR:			percent  =  0; break;
	case SECT_DESERT:		percent += 10; break;
	default:				       break;
    }

    if ( percent <= 0 )
    {
	send_to_char( "There isn't any dirt to kick.\n\r", ch );
	return;
    }

    if ( percent > number_percent( ) )
    {
	AFFECT_DATA af;

	act( "$n is blinded by the dirt in $s eyes!", victim, NULL, NULL,
								    TO_ROOM );
	act( "$n kicks dirt into your eyes!", ch, NULL, victim, TO_VICT );
        damage( ch, victim, 5, gsn_dirt, WEAR_NONE, DAM_NONE );
	send_to_char( "You can't see a thing!\n\r", victim );

	af.type      = gsn_dirt;
	af.duration  = 0;
	af.location  = APPLY_HITROLL;
	af.modifier  = -4;
	af.bitvector = AFF_BLIND;
	affect_to_char( victim, &af );

    }

    WAIT_STATE( ch, skill_table[gsn_dirt].beats );

    return;
}


void do_whirlwind( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *pChar;
    CHAR_DATA *pChar_next;
    OBJ_DATA  *wield;
    bool       found = FALSE;

    if ( !IS_NPC( ch ) 
	&& ch->level < skill_table[gsn_whirlwind].skill_level[ch->class] )
    {
	send_to_char( "You don't know how to do that...\n\r", ch );
	return;
    }

    if ( !( wield = get_eq_char( ch, WEAR_WIELD ) ) )
    {
	send_to_char( "You need to wield a weapon first...\n\r", ch );
	return;
    }

    act( "$n holds $p firmly, and starts spinning round...", ch, wield, NULL,
	TO_ROOM );
    act( "You hold $p firmly, and start spinning round...",  ch, wield, NULL,
	TO_CHAR );

    pChar_next = NULL;   
    for ( pChar = ch->in_room->people; pChar; pChar = pChar_next )
    {
	pChar_next = pChar->next_in_room;
	if ( IS_NPC( pChar ) )
	{
	    found = TRUE;
	    act( "$n turns towards YOU!", ch, NULL, pChar, TO_VICT );
	    one_hit( ch, pChar, gsn_whirlwind, WEAR_WIELD );
	}
    }

    if ( !found )
    {
	act( "$n looks dizzy, and a tiny bit embarassed.", ch, NULL, NULL,
	    TO_ROOM );
	act( "You feel dizzy, and a tiny bit embarassed.", ch, NULL, NULL,
	    TO_CHAR );
    }

    WAIT_STATE( ch, skill_table[gsn_whirlwind].beats );

    if ( !found
	&& number_percent( ) < 25 )
    {
	act( "$n loses $s balance and falls into a heap.",  ch, NULL, NULL,
	    TO_ROOM );
	act( "You lose your balance and fall into a heap.", ch, NULL, NULL,
	    TO_CHAR );
	ch->position = POS_STUNNED;
    }

   return;
}      


void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        percent;

    /* Don't allow charmed mobiles to do this, check player's level */
    if ( ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	|| ( !IS_NPC( ch )
	    && ch->level < skill_table[gsn_disarm].skill_level[ch->class] ) )
    {
	send_to_char( "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if (   !get_eq_char( ch, WEAR_WIELD   )
	&& !get_eq_char( ch, WEAR_WIELD_2 ) )
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
        if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

    if ( victim->fighting != ch && ch->fighting != victim )
    {
	act( "$E is not fighting you!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if (   !get_eq_char( victim, WEAR_WIELD   )
	&& !get_eq_char( victim, WEAR_WIELD_2 ) )
    {
	send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
    percent = number_percent( ) + victim->level - ch->level;
    if ( !get_eq_char( ch, WEAR_WIELD ) )
	percent *= 2;		/* 1/2 as likely with only 2nd weapon */
    if ( IS_NPC( ch ) || percent < ch->pcdata->learned[gsn_disarm] * 2 / 3 )
	disarm( ch, victim );
    else
	send_to_char( "You failed.\n\r", ch );
    return;
}



void do_sla( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    rch = get_char( ch );

    if ( !authorized( rch, "slay" ) )
        return;

    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "slay" ) )
        return;

    one_argument( argument, arg1 );
    one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && victim->level >= ch->level )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "immolate" ) )
    {
      act( "{o{yYour fireball turns $N into a blazing inferno.{x",
						ch, NULL, victim, TO_CHAR    );
      act( "{o{y$n releases a searing fireball in your direction.{x",
						ch, NULL, victim, TO_VICT    );
      act( "{o{y$n points at $N, who bursts into a flaming inferno.{x",
						ch, NULL, victim, TO_NOTVICT );
    }
    else if ( !str_cmp( arg2, "pounce" ) && get_trust( ch ) >= L_DIR )
    {
      act( "{o{rLeaping upon $N with bared fangs, you tear open $S throat and toss the corpse to the ground...{x",
						ch, NULL, victim, TO_CHAR    );
      act( "{o{rIn a heartbeat, $n rips $s fangs through your throat!  Your blood sprays and pours to the ground as your life ends...{x",
						ch, NULL, victim, TO_VICT    );
      act( "{o{rLeaping suddenly, $n sinks $s fangs into $N's throat.  As blood sprays and gushes to the ground, $n tosses $N's dying body away.{x",
						ch, NULL, victim, TO_NOTVICT );
    }
    else if ( !str_cmp( arg2, "shatter" ) )
    {
      act( "{cYou freeze $N with a glance and shatter the frozen corpse into tiny shards.{x",
						ch, NULL, victim, TO_CHAR    );
      act( "{c$n freezes you with a glance and shatters your frozen body into tiny shards.{x",
						ch, NULL, victim, TO_VICT    );
      act( "{c$n freezes $N with a glance and shatters the frozen body into tiny shards.{x",
						ch, NULL, victim, TO_NOTVICT );
    } 
    else if ( !str_cmp( arg2, "slit" ) && get_trust( ch ) >= L_DIR )
    {
      act( "{o{rYou calmly slit $N's throat.{x",
						ch, NULL, victim, TO_CHAR    );
      act( "{o{r$n reaches out with a clawed finger and calmly slits your throat.{x",
						ch, NULL, victim, TO_VICT    );
      act( "{o{r$n calmly slits $N's throat.{x",
						ch, NULL, victim, TO_NOTVICT );
    }
    else if ( !str_cmp( arg2, "squeeze" ) )
    {
	act( "{o{rYou grasp $S head and squeeze it until it explodes!{x",
						ch, NULL, victim, TO_CHAR    );
	act( "{o{r$n grasps your head and squeezes until it explodes!{x",
						ch, NULL, victim, TO_VICT    );
	act( "{o{r$n grasps $N's head and squeezes until it explodes!{x",
						ch, NULL, victim, TO_NOTVICT );
    }
    else
    {
	act( "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR    );
	act( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
	act( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
    }

    raw_kill( ch, victim );
    return;
}

/* This code is for PC's who polymorph into dragons.
 * Yeah I know this is specialized code, but this is fun.  :)
 * Breathe on friend and enemy alike.
 * -Kahn
 */

void pc_breathe( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        sn;

    send_to_char( "You feel the urge to burp!\n\r", ch );
    act( "$n belches!", ch, NULL, NULL, TO_ROOM );
    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;
	if ( victim->deleted )
	    continue;

	if ( victim == ch )
	    continue;

        if ( !IS_NPC( victim )
            && ( !registered( ch, victim )
                || ( !licensed( ch )
                    && str_cmp( race_table[victim->race].name, "Vampire" ) ) ) )
            continue;

	sn = skill_lookup( "fire breath" );
	(*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    }

    return;
}

/* This code is for PC's who polymorph into harpies.
 * Yeah I know this is specialized code, but this is fun.  :)
 * Scream into the ears of enemy and friend alike.
 * -Kahn
 */

void pc_screech( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    int        sn;

    send_to_char( "You feel the urge to scream!\n\r", ch );
    interpret( ch, "scream" );
    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;
	if ( victim->deleted )
	    continue;

	if ( victim == ch )
	    continue;

        if ( !IS_NPC( victim )
            && ( !registered( ch, victim )
                || ( !licensed( ch )
                    && str_cmp( race_table[victim->race].name, "Vampire" ) ) ) )
            continue;

	act( "Your ears pop from $n's scream.  Ouch!", ch, NULL, victim,
	    TO_VICT );
	sn = skill_lookup( "agitation" );
	(*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    }

    return;
}



void pc_spit( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;

    send_to_char( "You feel the urge to spit!\n\r", ch );
    act( "$n spews vitriol!", ch, NULL, NULL, TO_ROOM );
    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
	victim_next = victim->next_in_room;
	if ( victim->deleted )
	    continue;

	if ( victim == ch )
	    continue;

	if ( !IS_NPC( victim )
	    && ( !registered( ch, victim )
		|| ( !licensed( ch ) 
                    && str_cmp( race_table[victim->race].name, "Vampire" ) ) ) )
	    continue;

	act( "You are splattered with $n's vitriol.  Ouch!", ch, NULL, victim,
	    TO_VICT );
	(*skill_table[gsn_poison].spell_fun) ( gsn_poison, ch->level, ch, victim );

	damage( ch, victim, number_range( 1, ch->level ),
	       gsn_poison_weapon, WEAR_NONE, DAM_POISON );
    }

    return;
}


bool check_race_special( CHAR_DATA *ch )
{
    if ( !str_cmp( race_table[ch->race].name, "Dragon" ) )
    {
	if ( number_percent( ) < ch->level )
	{
	    pc_breathe( ch );
	    return TRUE;
	}
    }

    if ( !str_cmp( race_table[ch->race].name, "Harpy" ) )
    {
	if ( number_percent( ) < ch->level )
	{
	    pc_screech( ch );
	    return TRUE;
	}
    }

    if (   !str_cmp( race_table[ch->race].name, "Arachnid" )
	|| !str_cmp( race_table[ch->race].name, "Snake"    ) )
    {
	if ( number_percent( ) < ch->level )
	{
	    pc_spit( ch );
	    return TRUE;
	}
    }

    return FALSE;
}


void do_fee( CHAR_DATA *ch, char *argument )
{
    send_to_char(
		 "IF you wish to feed on someone or something, type FEED!\n\r",
		 ch );
    return;
}

/* Vampiric bite.  Feeding */
void do_feed( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *victim;
    OBJ_DATA    *obj;
    AFFECT_DATA  af;
    char         arg [ MAX_INPUT_LENGTH ];
    int          dam;
    int          heal;

    one_argument( argument, arg );

    if ( arg[0] == '\0' && !ch->fighting )
    {
	send_to_char( "Feed on whom?\n\r", ch );
	return;
    }

    if ( !( victim = ch->fighting ) )
        if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}

    if ( ch->riding )
    {
        send_to_char( "You can't do that while mounted.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
	send_to_char( "You cannot lunge for your own throat.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
        return;
    check_killer( ch, victim );

    send_to_char( "You must have blooood!\n\r", ch );
    act( "$n lunges for $N's throat!", ch, NULL, victim, TO_NOTVICT );
    act( "$n lunges for your throat!", ch, NULL, victim, TO_VICT );

    WAIT_STATE( ch, 12 );

    if ( str_cmp( race_table[ch->race].name, "Vampire" ) )
    {
        send_to_char( "You missed!\n\r", ch );
	if ( !ch->fighting )
	    set_fighting( ch, victim );
	return;
    }

    if ( ch->fighting )
    {
	if ( number_percent( ) > ch->level )
	{
	    send_to_char( "You missed!\n\r", ch );
	    return;
	}
    }

    /* If the victim is asleep, he deserves to be bitten.  :) */
    if ( !ch->fighting )
    {
	if ( number_percent( ) > ( ch->level * 1.75 ) && IS_AWAKE( victim ) )
	{
	    send_to_char( "You missed!\n\r", ch );
	    set_fighting( ch, victim );
	    return;
	}
    }

    dam = dice( 3, ch->level );
    damage( ch, victim, dam, gsn_vampiric_bite, WEAR_NONE, DAM_DISEASE );

    /* Lets see if we can get some food from this attack */
    if (   !IS_NPC( ch )
	&& number_percent( ) < ( 34 - victim->level + ch->level ) )
    {
        /* If not hungry, thirsty or damaged, then continue */
        if (   ch->pcdata->condition[COND_FULL  ] > 40
	    && ch->pcdata->condition[COND_THIRST] > 40
	    && ( ch->hit * 100 / ch->max_hit > 75 ) )
	{
	    return;
	}
	else
	{
	    send_to_char( "Ahh!  Blood!  Food!  Drink!\n\r", ch );
	    heal = number_range( 1, dam );
	    ch->hit = UMIN( ch->max_hit, ch->hit + heal );
	    /* Get full as per drinking blood */
	    gain_condition( ch, COND_FULL, heal * 2 );
	    /* Quench thirst as per drinking blood absolute value */
	    gain_condition( ch, COND_THIRST, heal );
	}
    }

    /* Ok now we see if we can infect the victim */
    if ( IS_NPC( victim ) )
        return;

    if ( victim->level < 11 || get_age( victim ) < 21 )
        return;

    if ( saves_spell( ch->level, victim, DAM_DISEASE )
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
		  
    af.type      = gsn_vampiric_bite;
    af.duration  = UMAX( 5, 30 - ch->level );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_VAMP_BITE;
    affect_join( victim, &af );

    send_to_char( "Ahh!  Taste the power!\n\r", ch );
    send_to_char( "Your blood begins to burn!\n\r", victim );
    return;

}

void do_stake( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *victim;
    OBJ_DATA    *obj;
    AFFECT_DATA  af;
    char         arg [ MAX_INPUT_LENGTH ];
    bool         found = FALSE;
    int          chance;

    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Stake whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char( "You may not stake NPC's.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
        return;
    check_killer( ch, victim );

    /* Check here to see if they have a stake in hand */
    
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == OBJ_VNUM_STAKE
	    && obj->wear_loc == WEAR_WIELD )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
    {
	send_to_char( "You lack a primary wielded stake to stake.\n\r", ch );
	act(
	    "$n screams and lunges at $N then realizes $e doesnt have a stake",
	    ch, NULL, victim, TO_ROOM );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return;
    }

    act( "$n screams and lunges at $N with $p.",  ch, obj, victim, TO_NOTVICT );
    act( "You scream and lunge at $N with $p.",   ch, obj, victim, TO_CHAR    );
    act( "$n screams and lunges at you with $p.", ch, obj, victim, TO_VICT    );

    /* Calculate chances, give vampire the ghoul effect,
       autokill the vampire. */
    chance = ( IS_NPC( ch ) ? ch->level : ch->pcdata->learned[gsn_stake] );
    chance = chance - victim->level + ch->level;

    if ( number_percent( ) < chance
	|| !IS_AWAKE( victim )
	|| str_cmp( race_table[victim->race].name, "Vampire" ) )
    {
	if ( !str_cmp( race_table[victim->race].name, "Vampire" ) )
	{
	    damage( ch, victim, victim->hit + 11, gsn_stake, WEAR_NONE,
		   DAM_NONE );

	    af.type      = gsn_stake;
	    af.duration  = 10;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_GHOUL;
	    affect_join( victim, &af );
	}
	else
	{
	    multi_hit( ch, victim, TYPE_UNDEFINED );
	}
    }
    else
    {
	do_feed( victim, ch->name );
    }
	    
    return;
}

bool in_arena ( CHAR_DATA *ch )
{
    if ( ch->in_room )
	return ( IS_SET( ch->in_room->room_flags, ROOM_ARENA ) ? TRUE : FALSE );

    return FALSE;
}

bool licensed ( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( !str_cmp( race_table[ch->race].name, "Vampire" ) )
        return TRUE;

    if ( in_arena( ch ) )
        return TRUE;

    if ( !IS_SET( ch->act, PLR_REGISTER ) )
        return FALSE;

    /*
     * If already fighting then we just jump out.
     */
    if ( ch->fighting )
        return TRUE;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == OBJ_VNUM_LICENSE )
	    return TRUE;
    }

    return FALSE;

}


bool registered ( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( IS_SET( victim->act, PLR_REGISTER ) )
        return TRUE;

    if ( !str_cmp( race_table[ch->race].name, "Vampire" ) )
        return TRUE;

    if ( in_arena( victim ) )
        return TRUE;

    return FALSE;

}

/* 
 * Mobs using magical items by Spitfire from merc mailing list
 * Modified to give every magical item an equal chance of being used plus
 * eating pills -Kahn
 */
void use_magical_item( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *cobj     = NULL;
    int       number   = 0;
    int       i        = 0;
    int       sn       = 0;
    char      buf[ MAX_INPUT_LENGTH ];

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( (   obj->item_type == ITEM_SCROLL
	      || obj->item_type == ITEM_WAND
	      || obj->item_type == ITEM_STAFF
	      || obj->item_type == ITEM_PILL )
	    && number_range( 0, number ) == 0 )
	{
	    cobj = obj;
	    number++;
	}
    }

    if ( !cobj )
        return;

   /*
    * Modified so mobs don't cause damage to themselves and
    * don't aid players to help stop HEAVY player cheating -Zen
    */
    switch( cobj->item_type )
    {
    case ITEM_SCROLL:	for ( i = 1; i < 5; i++ )
			{
			    sn = cobj->value[i];

			    if (   sn <= 0
				|| sn >= MAX_SKILL )
			    {
				extract_obj( cobj );
				return;
			    }

			    if ( skill_table[sn].target == TAR_CHAR_DEFENSIVE )
			    {
				act( "$n discards a $p.", ch, cobj, NULL, TO_ROOM );
				extract_obj( cobj );
				return;
			    }
			}
                 	break;
    case ITEM_POTION:
    case ITEM_PILL:	for ( i = 1; i < 5; i++ )
			{
			    sn = cobj->value[i];

			    if (   sn <= 0
				|| sn >= MAX_SKILL )
			    {
				extract_obj( cobj );
				return;
			    }

			    if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE )
			    {
				act( "$n discards a $p.", ch, cobj, NULL, TO_ROOM );
				extract_obj( cobj );
				return;
			    }
			}
                 	break;
    }

    switch( cobj->item_type )
    {
        case ITEM_SCROLL: do_recite( ch, "scroll" );
	                  break;
	case ITEM_WAND:   if ( cobj->wear_loc == WEAR_HOLD )
	                      do_zap( ch, "" );
	                  break;
	case ITEM_STAFF:  if ( cobj->wear_loc == WEAR_HOLD )
			      do_brandish( ch, "" );
	                  break;
	case ITEM_POTION: do_quaff( ch, "potion" );
	                  break;
	case ITEM_PILL:   sprintf( buf, "%s", cobj->name );
	                  do_eat( ch, buf );
                 	  break;
    }
    return;

}

/*
 * From TheIsles16 code.  TheIsles by Locke <locke@lm.com>
 */    
bool hit_suck_disarm( CHAR_DATA *ch, CHAR_DATA *victim, int hit, int dam )
{
    OBJ_DATA *obj;

    if ( number_percent( ) <= ch->level )
    {
	if ( ( race_table[ch->race].size
		- race_table[victim->race].size ) < -2 )
        return FALSE;

	if ( !( obj = get_eq_char( victim, WEAR_WIELD ) ) )
	    if ( !( obj = get_eq_char( victim, WEAR_WIELD_2 ) ) )
		return FALSE;

	if ( IS_SET( obj->extra_flags, ITEM_NOREMOVE )
	    || IS_SET( obj->extra_flags, ITEM_NODROP ) )
	    return FALSE;

	if ( !get_eq_char( ch, WEAR_WIELD   )
	    && !get_eq_char( ch, WEAR_WIELD_2 )
	    && number_bits( 1 ) == 0 )
	    return FALSE;

        act( "You suck $N's weapon right out of $S hand!",
	    ch, NULL, victim, TO_CHAR );
        act( "$n sucks your weapon right out of your hand!",
	    ch, NULL, victim, TO_VICT );
        act( "$n sucks $N's weapon right out of $S hand!",
	    ch, NULL, victim, TO_NOTVICT );

	obj_from_char( obj );
	obj_to_char( obj, ch );

        return TRUE;
    }

    return FALSE;
}

/*
 * From TheIsles16 code.  TheIsles by Locke <locke@lm.com>
 */    
bool hit_vorpal( CHAR_DATA *ch, CHAR_DATA *victim, int hit, int dam )
{
    OBJ_DATA *obj;
    char     *name;
    char      buf [MAX_STRING_LENGTH];

    dam *= 100;
    if ( victim->hit > 0 )
	dam /= victim->hit;

    if ( hit >= 17 && dam >= 40 )		/* dam >= 40 is MUTILATES */
    /*
     * Cut victim's head off, generate a corpse, do not
     * give experience for kill.  Needless to say,
     * it would be bad to be a target of this weapon!
     */
    {
        act( "You slice $N's head clean off!",  ch, 0, victim, TO_CHAR );
        act( "$n slices your head clean off!",  ch, 0, victim, TO_VICT );
        act( "$n slices $N's head clean off!",  ch, 0, victim, TO_NOTVICT );
        act( "$n's severed head plops on the ground.", victim, 0, 0, TO_ROOM );
        stop_fighting( victim, TRUE );

	name		= IS_NPC( victim ) ? victim->short_descr : victim->name;
	obj		= create_object( get_obj_index( OBJ_VNUM_SEVERED_HEAD ), 0 );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	if ( IS_AFFECTED( victim, AFF_POISON ) )
	    obj->value[3] = 1;

        obj_to_room( obj, ch->in_room );
	raw_kill( ch, victim );

        return TRUE;
    }

    return FALSE;
}

/*
 * From TheIsles16 code.  TheIsles by Locke <locke@lm.com>
 * Heavily modified by Zen.
 */    
void do_shoot( CHAR_DATA *ch, char *argument )
{
    extern char *    const  dir_name	[ ];
    extern char *    const  dir_rev	[ ];

    OBJ_DATA		*obj;
    OBJ_DATA		*newobj;
    OBJ_INDEX_DATA	*pObjIndex;
    EXIT_DATA		*pexit;
    ROOM_INDEX_DATA	*to_room;
    CHAR_DATA		*mob;
    char		*msg;
    char		 arg2 [ MAX_INPUT_LENGTH ];
    char		 arg3 [ MAX_INPUT_LENGTH ];
    char		 buf  [ MAX_STRING_LENGTH ];
    int			 range = 3;
    int			 dir;
    int			 n;

    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
       return;

    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    dir = -1;
    for ( dir = 0; dir < MAX_DIR; dir++ )
    {
	if ( !str_prefix( arg2, dir_name[dir] ) )
	    break;
    }

    if ( arg2[0] == '\0' || dir < 0 )
    {
         send_to_char( "Syntax:  shoot <direction> [range]\n\r", ch );
         return;
    }

    if ( !( obj = get_eq_char( ch, WEAR_MISSILE_WIELD ) ) )
    {
	send_to_char( "You aren't using a missile weapon.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_RANGED_WEAPON
	|| obj->value[1] < 0 || obj->value[2] < 0 )
    {
         send_to_char( "You can't do that.\n\r", ch);
         return;
    }

    if ( obj->value[0] == 0 )
    {
         send_to_char( "Its payload is empty.\n\r", ch);
         return;
    }

    if ( obj->value[3] == RNG_CATAPULT )
    {
	if ( arg3[0] != '\0' )
	    range = atoi( arg3 );

	if ( range <= 0 )
	{
	    send_to_char( "Try shooting it away from you.\n\r", ch );
	    return;
	}

	if ( range > obj->value[4] )
	{
	    if ( obj->value[4] > 0 )	range = obj->value[4];
	    else			range = 1;
	}
    }

    switch ( obj->value[3] )
    {
    default:		msg = "You pull back the string on %s and fire %s!\n\r";
									break;
    case RNG_BOW:	msg = "You pull back the string on %s and fire %s!\n\r";
									break;
    case RNG_CROSSBOW:	msg = "You fire %s bolt %s!\n\r";
									break;
    case RNG_CATAPULT:	msg = "You crank back %s, and release it %sward!\n\r";
									break;
    }
    sprintf( buf, msg, obj->short_descr, dir_name[dir] );
    send_to_char( buf, ch );

    switch ( obj->value[3] )
    {
    default:		msg = "You pull back the string on %s and fire %s!";
									break;
    case RNG_BOW:	msg = "%s pulls back the string on %s and fires %s!";
									break;
    case RNG_CROSSBOW:	msg = "%s fires %s bolt %s!";
									break;
    case RNG_CATAPULT:	msg = "%s cranks back %s, and releases it %sward!";
									break;
    }
    sprintf( buf, msg, ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
	    obj->short_descr, dir_name[dir] );
    act( buf, ch, NULL, NULL, TO_ROOM );

    if ( !(pObjIndex = get_obj_index( obj->value[0] ) ) )
    {
         bug( "Ranged weapon (vnum %d) has invalid ammunition.", 0 );
         return;
    }

    if ( !ch->in_room->exit[dir]
	|| IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED ) )
    {
	newobj = create_object( get_obj_index( obj->value[0] ), 0 );
	obj->value[0] = 0;

	switch ( obj->value[3] )
	{
	default:           msg = "%s impales itself into the ground.";	break;
	case RNG_BOW:      msg = "%s impales itself into the ground.";	break;
	case RNG_CROSSBOW: msg = "%s falls to the ground.";		break;
	case RNG_CATAPULT: msg = "%s crashes to the ground.";		break;
	}

	sprintf( buf, msg, newobj->short_descr );

        act( capitalize( buf ), ch, NULL, NULL, TO_ROOM );
        act( capitalize( buf ), ch, NULL, NULL, TO_CHAR );
        obj_to_room( newobj, ch->in_room );
        return;
    }

    newobj = create_object( pObjIndex, 0 );
    obj->value[0] = 0;

    for ( n = 1, pexit = ch->in_room->exit[dir]; pexit && n <= (range+1); n++ )
    {
	if ( IS_SET( pexit->exit_info, EX_CLOSED )
	    || IS_SET( pexit->exit_info, EX_JAMMED ) )
	{
	    switch ( obj->value[3] )
	    {
	    default:           msg = "%s impales itself into the ground.";
									break;
	    case RNG_BOW:      msg = "%s impales itself into the ground.";
									break;
	    case RNG_CROSSBOW: msg = "%s falls to the ground.";
									break;
	    case RNG_CATAPULT: msg = "%s crashes to the ground.";
									break;
	    }

	    sprintf( buf, msg, newobj->short_descr );

	    if ( pexit->to_room
		&& pexit->to_room->people )
	    {
		act( buf, pexit->to_room->people, NULL, NULL, TO_ROOM );
		act( buf, pexit->to_room->people, NULL, NULL, TO_CHAR );
	    }

	    obj_to_room( newobj, pexit->to_room );
	    act( buf, ch, NULL, NULL, TO_ROOM );
	    return;
	}

	to_room = pexit->to_room;

	if ( to_room->people )
	{
	    for ( mob = to_room->people; mob; mob = mob->next_in_room )
	    {

		if ( mob->deleted )
		    continue;

	    if ( ( mob->position == POS_STANDING 
		|| mob->position == POS_FIGHTING )
		&& ( check_dodge( ch, mob ) || ch->level > number_percent( ) ) )
	    {
		int dam;

		sprintf( buf, "%s streaks into %s from %s!",
			newobj->short_descr,
			( IS_NPC( mob ) ? mob->short_descr : mob->name ),
			dir_rev[dir] );
		act( buf, mob, NULL, NULL, TO_ROOM );
		n = range;

		sprintf( buf, "%s streaks into you from %s!",
			newobj->short_descr, dir_rev[dir] );
		act( buf, mob, NULL, NULL, TO_CHAR );

		dam = number_range( obj->value[1], obj->value[2] );
		dam += newobj->value[1];

		/*
		 * Weapon Proficiencies.
		 */
		if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_shot] > 0 )
		    dam += dam * ch->pcdata->learned[gsn_shot] / 150;

		extract_obj( newobj );
		damage( ch, mob, dam, TYPE_UNDEFINED, WEAR_NONE, DAM_NONE );
		stop_fighting( mob, FALSE );

		if ( IS_NPC( mob ) )
		{
		    start_hating( mob, ch );
		    start_hunting( mob, ch );
		}
             
		if ( mob->position > POS_STUNNED
		    && obj->value[3] == RNG_CATAPULT )
		{
		    act ( "The impact puts $n to sleep!",
			 mob, NULL, NULL, TO_ROOM );
		    act ( "The impact puts you to sleep... ZzZzZ",
			 mob, NULL, NULL, TO_CHAR  );
		    mob->position = POS_SLEEPING;
		}

		return;
	    }
	    } 
	}

	if ( to_room->people )
	{
	    if ( n <= range )
	    {
		sprintf( buf, "%s streaks through the air and continues %s.",
			newobj->short_descr, dir_name[dir] );
		act( buf, to_room->people, NULL, NULL, TO_ROOM );
		act( buf, to_room->people, NULL, NULL, TO_CHAR );
	    }
	    else
	    {
		sprintf( buf, "%s zooms in and lands on the ground.",
                        newobj->short_descr );
		act( buf, to_room->people, NULL, NULL, TO_ROOM );
		act( buf, to_room->people, NULL, NULL, TO_CHAR );
		obj_to_room( newobj, to_room );
	    }
	}

	pexit = to_room->exit[dir];
    }

    return;
}

/*
 * From TheIsles16 code.  TheIsles by Locke <locke@lm.com>
 * Heavily modified by Zen.
 */    
void do_reload( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *weapon;
    OBJ_DATA *ammo;
    OBJ_DATA *ammo_next;
    char      buf [MAX_STRING_LENGTH];

    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
       return;

    if ( !( weapon = get_eq_char( ch, WEAR_MISSILE_WIELD ) ) )
    {
        send_to_char( "I don't see that weapon here.\n\r", ch );
        return;
    }

    if ( weapon->item_type != ITEM_RANGED_WEAPON )
    {
        send_to_char( "That is not a missile weapon.\n\r", ch );
        return;
    }

    for ( ammo = ch->carrying; ammo; ammo = ammo_next )
    {
	ammo_next = ammo->next_content;

	if ( ammo->deleted )
	    continue;
	    
	if (   ammo->item_type == ITEM_AMMO
	    && ammo->value[0] == weapon->value[3] )
	    break;
    }

    if ( !ammo )
    {
        send_to_char( "You do not have ammo for this weapon.\n\r", ch );
        return;
    }

    if ( weapon->value[0] != 0 )
    {
        sprintf( buf, "%s is already loaded.\n\r", weapon->short_descr );
        send_to_char( buf, ch );
        return;
    }

    weapon->value[0] = ammo->pIndexData->vnum;

    act( "You get $p.", ch, ammo, weapon, TO_CHAR );
    act( "$n get $p.", ch, ammo, weapon, TO_ROOM );

    act( "You load $P with $p.", ch, ammo, weapon, TO_CHAR );
    act( "$n loads $P with $p.", ch, ammo, weapon, TO_ROOM );

    extract_obj( ammo );
    return;
}
