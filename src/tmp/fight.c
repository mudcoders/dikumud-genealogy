/**************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
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

#define unix 1
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
bool	check_dodge	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_sidestep       args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	check_killer	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	dam_message	     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
				    int dt ) );
void	death_cry	     args( ( CHAR_DATA *ch ) );
void	death_xp_loss	     args( ( CHAR_DATA *victim ) );
void	group_gain	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	     args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
bool	is_safe		     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_bare_hand	     args( ( CHAR_DATA *ch ) );
bool    is_wielding_poisoned args( ( CHAR_DATA *ch ) );
bool    is_wielding_flaming  args( ( CHAR_DATA *ch ) );
bool    is_wielding_chaos    args( ( CHAR_DATA *ch ) );
bool    is_wielding_icy      args( ( CHAR_DATA *ch ) );

void	make_corpse	     args( ( CHAR_DATA *ch ) );
void	one_hit		     args( ( CHAR_DATA *ch, CHAR_DATA *victim,
				    int dt ) );
void	one_dual	     args( ( CHAR_DATA *ch, CHAR_DATA *victim,
				    int dt ) );
void	raw_kill	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	set_fighting	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm		     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	trip		     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    item_damage             args( ( CHAR_DATA *ch, int dam ) );


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
    int stun;

    for ( ch = char_list; ch; ch = ch->next )
    {
	if ( !ch->in_room || ch->deleted )
	    continue;

	for (stun = 0; stun < STUN_MAX; stun++)
	{
	  if ( IS_STUNNED( ch, stun ) )
	    ch->stunned[stun]--;
	}

	if ( ( victim = ch->fighting ) )
	{
	    if ( IS_AWAKE( ch ) && ch->in_room == victim->in_room )
	        multi_hit( ch, victim, TYPE_UNDEFINED );
	    else
	        stop_fighting( ch, FALSE );
	    continue;
	}


	if ( IS_AFFECTED( ch, AFF_BLIND )
	    || ( IS_NPC( ch ) && ch->pIndexData->pShop ) )
	    continue;

	/* Ok. So ch is not fighting anyone.
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
		    && ( vch->level > 5 )
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
        mprog_hitprcnt_trigger( ch, victim );
        mprog_fight_trigger( ch, victim );
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
    int chance2;

    if ( IS_NPC( ch ) )
    {
      mprog_hitprcnt_trigger( ch, victim );
      mprog_fight_trigger( ch, victim );
    }

    if ( ( IS_AFFECTED2( ch, AFF_CONFUSED ) )
       && number_percent ( ) < 10 )
    {
      act(AT_YELLOW, "$n looks around confused at what's going on.", ch, NULL, NULL, TO_ROOM );
      send_to_char( AT_YELLOW, "You stand confused.\n\r", ch );
      return;
    }

    one_hit( ch, victim, dt );
    if(IS_AFFECTED(ch, AFF_HASTE))
      one_hit( ch, victim, dt );
    if (IS_AFFECTED2( ch, AFF_RUSH ) )
      one_hit( ch, victim, dt );
    if ( ch->fighting != victim || dt == gsn_backstab )
	return;

    if ( !IS_NPC( ch )
    && ( ( is_class( ch, CLASS_MONK ) && ch->pcdata->learned[gsn_blackbelt] > 0 )
    || ( is_class( ch, CLASS_WEREWOLF ) && ch->pcdata->learned[gsn_dualclaw] > 0 )
    || ch->pcdata->learned[gsn_dual] > 0 ) )
       {
        one_dual( ch, victim, dt );
	if(IS_AFFECTED(ch, AFF_HASTE))
	  one_dual( ch, victim, dt );
	if ( IS_AFFECTED2( ch, AFF_RUSH ) )
	  one_dual( ch, victim, dt );
        if ( ch->fighting != victim )
           return;
       }
    if ( ( IS_NPC( ch ) ) && ( ch->level > 15 ) )
       {
        one_dual( ch, victim, dt );
        if ( ch->fighting != victim )
           return;
       }
    chance = IS_NPC( ch ) ? ch->level - 5
                          : ch->pcdata->learned[gsn_second_attack]/2;
    if ( is_class( ch, CLASS_WEREWOLF ) )
    chance2 = IS_NPC( ch ) ? ch->level / 2 - 5
			   : ch->pcdata->learned[gsn_dualclaw] / 2;
    else if ( is_class( ch, CLASS_MONK ) && !get_eq_char( ch, WEAR_WIELD_2 ) )
    chance2 = IS_NPC( ch ) ? ch->level / 2 - 5
			   : ch->pcdata->learned[gsn_blackbelt] / 2;
    else
    chance2 = IS_NPC( ch ) ? ch->level/2 - 5
                          : ch->pcdata->learned[gsn_dual]/2;
    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt );
	update_skpell( ch, gsn_second_attack );
	if ( ch->fighting != victim )
	    return;
        if ( number_percent( ) < chance2 )
        {
         one_dual( ch, victim, dt );
         if ( ch->fighting != victim )
           return;
        }
    }

    chance = IS_NPC( ch ) ? ch->level
                          : ch->pcdata->learned[gsn_third_attack]/4;
    if ( is_class( ch, CLASS_WEREWOLF ) )
    chance2 = IS_NPC( ch ) ? ch->level / 2
			   : ch->pcdata->learned[gsn_dualclaw] / 4;
    else if ( is_class( ch, CLASS_MONK ) && !get_eq_char( ch, WEAR_WIELD_2 ) )
    chance2 = IS_NPC( ch ) ? ch->level / 2
			   : ch->pcdata->learned[gsn_blackbelt] / 4;
    else
    chance2 = IS_NPC( ch ) ? ch->level/2
                           :ch->pcdata->learned[gsn_dual]/4;
    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt );
	update_skpell( ch, gsn_third_attack );
	if ( ch->fighting != victim )
	    return;
        if ( number_percent( ) < chance2 )
        {
         one_dual( ch, victim, dt );
         if ( ch->fighting != victim )
           return;
        }
    }

    chance = IS_NPC( ch ) ? ch->level/2
                          : ch->pcdata->learned[gsn_fourth_attack]/4;
    if ( number_percent( ) < chance && ch->wait == 0 )
    {
	one_hit( ch, victim, dt );
	update_skpell( ch, gsn_fourth_attack );
	if ( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC( ch ) ? ch->level/2
                          : ch->pcdata->learned[gsn_fifth_attack]/8;
    if ( is_class( ch, CLASS_WEREWOLF ) )
    chance2 = IS_NPC( ch ) ? ch->level / 6
			   : ch->pcdata->learned[gsn_dualclaw] / 8;
    else if ( is_class( ch, CLASS_MONK ) && !get_eq_char( ch, WEAR_WIELD_2 ) )
    chance2 = IS_NPC( ch ) ? ch->level / 6
			   : ch->pcdata->learned[gsn_blackbelt] / 8;
    else
    chance2 = IS_NPC( ch ) ? ch->level/6
                          : ch->pcdata->learned[gsn_dual]/8;
    if ( number_percent( ) < chance && ch->wait == 0 )
    {
	one_hit( ch, victim, dt );
	update_skpell( ch, gsn_fifth_attack );
	if ( ch->fighting != victim )
	    return;
        if ( number_percent( ) < chance2 )
        {
         one_dual( ch, victim, dt );
         if ( ch->fighting != victim )
           return;
        }
    }

    chance = IS_NPC( ch ) ? ch->level/4
                          : ch->pcdata->learned[gsn_sixth_attack]/16;
    if ( number_percent( ) < chance && ch->wait == 0 )
    {
	one_hit( ch, victim, dt );
	update_skpell( ch, gsn_sixth_attack );
	if ( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC( ch ) ? ch->level/4
                          : ch->pcdata->learned[gsn_seventh_attack]/32;
    if ( is_class( ch, CLASS_WEREWOLF ) )
    chance2 = IS_NPC( ch ) ? ch->level / 4
			   : ch->pcdata->learned[gsn_dualclaw] / 32;
    else if ( is_class( ch, CLASS_MONK ) && !get_eq_char( ch, WEAR_WIELD_2 ) )
    chance2 = IS_NPC( ch ) ? ch->level / 4
			   : ch->pcdata->learned[gsn_blackbelt] / 22;
    else
    chance2 = IS_NPC( ch ) ? ch->level/4
                          : ch->pcdata->learned[gsn_dual]/32;
    if ( number_percent( ) < chance && ch->wait == 0 )
    {
	one_hit( ch, victim, dt );
	update_skpell( ch, gsn_seventh_attack );
	if ( ch->fighting != victim )
	    return;
        if ( number_percent( ) < chance2 )
        {
         one_dual( ch, victim, dt );
         if ( ch->fighting != victim )
           return;
        }
    }

    if ( IS_AFFECTED2(ch, AFF_BERSERK) )
    {
      one_hit( ch, victim, dt );
      if (ch->fighting != victim )
	return;
      one_dual( ch, victim, dt );
      if (ch->fighting != victim )
	return;
    }

    chance = IS_NPC( ch ) ? ch->level / 4
                          : 0;
    chance2 = IS_NPC( ch ) ? ch->level / 8
                          : 0;
    if ( number_percent( ) < chance )
	one_hit( ch, victim, dt );
    if ( number_percent( ) < chance2 )
         one_dual( ch, victim, dt );

    return;
}



/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *wield;
    char      buf [ MAX_STRING_LENGTH ];
    int       victim_ac;
    int       thac0;
    int       thac0_00;
    int       thac0_97;
    int       dam;
    int       diceroll;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    if ( IS_STUNNED( ch, STUN_NON_MAGIC ) ||
	 IS_STUNNED( ch, STUN_TOTAL ) )
      return;

    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, WEAR_WIELD );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else if ( is_class( ch, CLASS_WEREWOLF ) && !wield )
	    dt += 5;
	else if ( is_class( ch, CLASS_MONK ) && !wield )
	    dt += 14;
    }

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC( ch ) )
    {
	thac0_00 =  18;
	thac0_97 = -24;
    }
    else
    {
	thac0_00 = class_table[prime_class(ch)].thac0_00;
	thac0_97 = class_table[prime_class(ch)].thac0_97;
    }

    if (!IS_NPC(ch))
        thac0     = interpolate( ch->level, thac0_00, thac0_97 )
                  - GET_HITROLL( ch );
    else
        thac0     = interpolate(ch->level, thac0_00, thac0_97 )
                  - (ch->level + ch->level/2);

    if ( ( !IS_NPC( ch ) ) && ( ch->pcdata->learned[gsn_enhanced_hit] > 0 ) )
    {
       thac0 -= ch->pcdata->learned[gsn_enhanced_hit] / 5;
       update_skpell( ch, gsn_enhanced_hit );
    }

    victim_ac = UMAX( -15, GET_AC( victim ) / 10 );
/*
    if ( victim->level < L_APP && victim->class == CLASS_VAMPIRE )
     if ( !IS_SET( victim->in_room->room_flags, ROOM_INDOORS ) )
     {
      if ( time_info.hour > 6 && time_info.hour < 18 )
      {
	victim_ac += victim->level * 2;
      }
    }
*/
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if (     diceroll == 0
	|| ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
         if ( IS_AFFECTED( victim, AFF_FIRESHIELD ) )
           {
             if ( number_percent( ) < 50 )
	     {
	       if ( !IS_NPC(victim) )
		 spell_fireball ( skill_lookup("fireball"), 15, victim, ch );
	       else
		 spell_fireball ( skill_lookup("fireball"), 25, victim, ch );
	     }
           }
  if ( !victim || victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;
	if ( IS_AFFECTED( victim, AFF_ICESHIELD ) )
           {
             if ( ( number_percent( ) < 50 )
	       || ( number_percent( ) < 17 ) )
	     {
	       if ( !IS_NPC(victim) )
		 spell_icestorm ( skill_lookup("icestorm"), 20, victim, ch );
	       else
		 spell_fireball ( skill_lookup("icestorm"), 40, victim, ch );
	     }
	   }
  if ( !victim || victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;
	if ( IS_AFFECTED( victim, AFF_SHOCKSHIELD ) )
           {
             if ( ( number_percent( ) < 50 )
	       || ( number_percent( ) < 17 ) )
	     {
	       if ( !IS_NPC(victim) )
		 spell_lightning_bolt ( skill_lookup("lightning bolt"), 15, victim, ch );
	       else
		 spell_lightning_bolt ( skill_lookup("lightning bolt"), 35, victim, ch );
	     }
	   }
  if ( !victim || victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;
	if ( IS_AFFECTED( victim, AFF_CHAOS ) )
           {
             if ( number_percent( ) < 50 )
	     {
	       if ( !IS_NPC(victim) )
		 spell_energy_drain ( skill_lookup("energy drain"), 30, victim, ch );
	       else
		 spell_energy_drain ( skill_lookup("energy drain"), 40, victim, ch );
	     }
           }
  if ( !victim || victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;
	if ( IS_AFFECTED( victim, AFF_VIBRATING ) )
           {
             if ( ( number_percent( ) < 50 )
	       || ( number_percent( ) < 17 ) )
	       spell_psionic_blast ( skill_lookup("psionic blast"), 25, victim, ch );
           }
  if ( !victim || victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;
	if ( IS_AFFECTED2( victim, AFF_BLADE ) )
           {
             if ( number_percent( ) < 40 )
                spell_holy_fires( skill_lookup("holy fires"), 45, victim, ch );
           }
  if ( !victim || victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;
	if ( IS_AFFECTED2( victim, AFF_FIELD ) )
	   {
	     if ( number_percent( ) < 10 )
		spell_death_field( skill_lookup("death field"),
		victim->level, victim, ch );
	   }
  if ( !victim || victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;
    if ( IS_NPC( ch ) )
    {
	dam = number_range( ch->level / 3, ch->level * 3 / 2 );
	if ( wield )
	    dam += dam / 3;
    }
    else
    {
	if ( wield )
	    dam = number_range( wield->value[1], wield->value[2] );
	else if ( (is_class( ch, CLASS_WEREWOLF )
	     || is_class( ch, CLASS_MONK ))
	     && !wield )
	    dam = UMAX( number_fuzzy( ch->level / 2 + ch->level / 15 ),
			number_fuzzy( 5 ) );
	else
	    dam = number_range( 1, 4 );
	if ( wield && dam > 1000 && !IS_IMMORTAL(ch) )
	{
	    sprintf( buf, "One_hit dam range > 1000 from %d to %d",
		    wield->value[1], wield->value[2] );
	    bug( buf, 0 );
	    if ( wield->name )
	      bug( wield->name, 0 );
	}
    }

    /*
     * Bonuses.
     */
    dam += GET_DAMROLL( ch );
    if ( wield && IS_SET( wield->extra_flags, ITEM_POISONED ) )
        dam += dam / 8;
    if (
	(wield && IS_SET( wield->extra_flags, ITEM_FLAME ) )
    ||	(!wield && is_affected( ch, gsn_flamehand ) )
       )
        dam += dam / 8;
     if (
	(wield && IS_SET( wield->extra_flags, ITEM_CHAOS ) )
     ||	(!wield && is_affected( ch, gsn_chaoshand ) )
        )
        dam += dam / 4;
    if (
	(wield && IS_SET( wield->extra_flags, ITEM_ICY   ) )
    ||	(!wield && is_affected( ch, gsn_frosthand ) )
       )
        dam += dam / 8;
    if ( is_class( ch, CLASS_WEREWOLF ) && !wield )
	dam += dam / 8;
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
    {
	dam += dam * ch->pcdata->learned[gsn_enhanced_damage] / 150;
	update_skpell( ch, gsn_enhanced_damage );
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_enhanced_claw] > 0
    && !wield )
    {
	dam += dam / 4 * ch->pcdata->learned[gsn_enhanced_claw] / 150;
	update_skpell( ch, gsn_enhanced_claw );
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_enhanced_two] > 0 )
    {
        dam += dam / 4 * ch->pcdata->learned[gsn_enhanced_two] / 150;
	update_skpell( ch, gsn_enhanced_two );
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_enhanced_three] > 0 )
    {
        dam += dam / 4 * ch->pcdata->learned[gsn_enhanced_three] / 150;
	update_skpell( ch, gsn_enhanced_three );
    }
    if ( !IS_AWAKE( victim ) )
	dam *= 2;
    if ( dt == gsn_backstab )
       {
        if (ch->clan == 4)
           dam *= number_range ( 3, 8 );
	else
	   dam *= 2 + UMIN( ( ch->level / 8) , 4 );
       }
    if ( dam <= 0 )
	dam = 1;
    damage( ch, victim, dam, dt );
    tail_chain( );
    return;
}

void one_dual( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *wield;
    char      buf [ MAX_STRING_LENGTH ];
    int       victim_ac;
    int       thac0;
    int       thac0_00;
    int       thac0_97;
    int       dam;
    int       diceroll;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( !(get_eq_char( ch, WEAR_WIELD_2 ))
    && !is_class( ch, CLASS_WEREWOLF )
    && !is_class( ch, CLASS_MONK ) )
 	return;
    if ( is_class( ch, CLASS_WEREWOLF )
    && ( get_eq_char( ch, WEAR_SHIELD )
    || get_eq_char( ch, WEAR_HOLD ) ) )
	return;
    if ( is_class( ch, CLASS_MONK )
    && ( get_eq_char( ch, WEAR_SHIELD )
    || get_eq_char( ch, WEAR_HOLD ) ) )
	return;
    if ( ( is_class( ch, CLASS_MONK ) && ch->pcdata->learned[gsn_blackbelt] == 0 )
    && ( is_class( ch, CLASS_WEREWOLF ) && ch->pcdata->learned[gsn_dualclaw] == 0 )
    && !get_eq_char( ch, WEAR_WIELD_2 ) )
	return;
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    if ( IS_STUNNED( ch, STUN_NON_MAGIC ) ||
	 IS_STUNNED( ch, STUN_TOTAL ) )
      return;

    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, WEAR_WIELD_2 );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else if ( is_class( ch, CLASS_WEREWOLF ) && !wield )
	    dt += 5;
	else if ( is_class( ch, CLASS_MONK ) && !wield )
	    dt += 15;
    }

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC( ch ) )
    {
	thac0_00 =  20;
	thac0_97 = -20;
    }
    else
    {
	thac0_00 = class_table[prime_class(ch)].thac0_00;
	thac0_97 = class_table[prime_class(ch)].thac0_97;
    }
    thac0     = interpolate( ch->level, thac0_00, thac0_97 )
              - GET_HITROLL( ch );
    victim_ac = UMAX( -15, GET_AC( victim ) / 10 );
/*
    if ( victim->level < L_APP && victim->class == CLASS_VAMPIRE )
     if ( !IS_SET( victim->in_room->room_flags, ROOM_INDOORS ) )
     {
      if ( time_info.hour > 6 && time_info.hour < 18 )
      {
	victim_ac += victim->level * 2;
      }
    }
*/
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if (     diceroll == 0
	|| ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt );
	tail_chain( );
	return;
    }

    update_skpell( ch, gsn_dual );

    /*
     * Hit.
     * Calc damage.
     */
/*	if ( IS_AFFECTED( victim, AFF_FIRESHIELD ) )
           {
             if ( number_percent( ) < 50 )
                spell_fireball ( skill_lookup("fireball"), 5, victim, ch );
           }
	if ( IS_AFFECTED( victim, AFF_SHOCKSHIELD ) )
           {
             if ( number_percent( ) < 50 )
                spell_lightning_bolt ( skill_lookup("lightning bolt"), 15, victim, ch );
           }
	if ( IS_AFFECTED( victim, AFF_CHAOS ) )
           {
	     if ( number_percent( ) < 50 )
                spell_energy_drain ( skill_lookup("energy drain"), 25, victim, ch );
           }
	if ( IS_AFFECTED( victim, AFF_VIBRATING ) )
           {
             if ( number_percent( ) < 50 )
                spell_psionic_blast ( skill_lookup("psionic blast"), 25, victim, ch );
           }*/

    if ( IS_NPC( ch ) )
    {
	dam = number_range( ch->level / 3, ch->level );
	if ( wield )
	    dam += dam / 4;
    }
    else
    {
	if ( wield )
	    dam = number_range( wield->value[1], wield->value[2] );
	else if ( (is_class( ch, CLASS_WEREWOLF )
	     || is_class( ch, CLASS_MONK ))
	     && !wield )
	    dam = UMAX( number_fuzzy( ch->level / 2 + ch->level / 15 ),
			number_fuzzy( 5 ) );
	else
	    dam = number_range( 1, 4 );
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
    dam += GET_DAMROLL( ch );
    if ( wield && IS_SET( wield->extra_flags, ITEM_POISONED ) )
        dam += dam / 4;
    if (
	(wield && IS_SET( wield->extra_flags, ITEM_FLAME ) )
    ||	(!wield && is_affected( ch, gsn_flamehand ) )
       )
        dam += dam / 8;
    if (
	(wield && IS_SET( wield->extra_flags, ITEM_CHAOS ) )
    ||	(!wield && is_affected( ch, gsn_chaoshand ) )
       )
        dam += dam / 8;
    if (
	(wield && IS_SET( wield->extra_flags, ITEM_ICY   ) )
    ||	(!wield && is_affected( ch, gsn_frosthand ) )
       )
        dam += dam / 4;
    if ( is_class( ch, CLASS_WEREWOLF ) && !wield )
	dam += dam / 4;
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
    {
	dam += dam * ch->pcdata->learned[gsn_enhanced_damage] / 150;
	update_skpell( ch, gsn_enhanced_damage );
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_enhanced_claw] > 0
    && !wield )
    {
	dam += dam * ch->pcdata->learned[gsn_enhanced_claw] / 150;
	update_skpell( ch, gsn_enhanced_claw );
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_enhanced_two] > 0 )
    {
        dam += dam * ch->pcdata->learned[gsn_enhanced_two] / 150;
	update_skpell( ch, gsn_enhanced_two );
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_enhanced_three] > 0 )
    {
        dam += dam * ch->pcdata->learned[gsn_enhanced_three] / 150;
	update_skpell( ch, gsn_enhanced_three );
    }
    if ( !IS_AWAKE( victim ) )
	dam *= 2;
    if ( dt == gsn_backstab )
	dam *= 2 + UMIN( ( ch->level / 8) , 4 );

    if ( dam <= 0 )
	dam = 1;

    damage( ch, victim, dam, dt );
    tail_chain( );
    return;
}


/*
 * Inflict damage from a hit.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    if ( victim->position == POS_DEAD )
	return;

    if(!IS_NPC(ch) && !IS_NPC(victim))
    {
      if (!(ch == victim) )
      {
       ch->combat_timer = 90;
       victim->combat_timer = 90;
      }
    }

    /*
     * Stop up any residual loopholes.
     */
    if ( dam > 3500 )
    {
        char buf [ MAX_STRING_LENGTH ];

      if ( dt != 91 && ch->level <= LEVEL_HERO
      && dt != 40 )
      {
	if ( IS_NPC( ch ) && ch->desc && ch->desc->original )
	    sprintf( buf,
		    "Damage: %d from %s by %s: > 3500 points with %d dt!",
		    dam, ch->name, ch->desc->original->name, dt );
	else
	    sprintf( buf,
		    "Damage: %d from %s: > 3500 points with %d dt!",
		    dam, ch->name, dt );

	bug( buf, 0 );
      }
    }

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
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( !ch->fighting )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if (   IS_NPC( ch )
		&& IS_NPC( victim )
		&& IS_AFFECTED( victim, AFF_CHARM )
		&& victim->master
		&& victim->master->in_room == ch->in_room
		&& number_bits( 3 ) == 0 )
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
	    act(AT_GREY, "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
	}
	if (IS_AFFECTED2( ch, AFF_PHASED ) )
	{
            affect_strip ( ch, skill_lookup("phase shift") );
	    affect_strip ( ch, skill_lookup("mist form") );
	    REMOVE_BIT( ch->affected_by2, AFF_PHASED );
	    act(AT_GREY, "$n returns from an alternate plane.", ch, NULL, NULL, TO_ROOM );
	}
	/*
	 * Damage modifiers.
	 */
	if ( ch->race == RACE_OGRE )
            dam -= dam / 20;
/*
    if ( ch->level < L_APP && ch->class == CLASS_VAMPIRE )
     if ( !IS_SET( ch->in_room->room_flags, ROOM_INDOORS ) )
     {
      if ( time_info.hour > 6 && time_info.hour < 18 )
      {
       send_to_char( AT_RED,
       "The sunlight has weakened your attack.\n\r", ch );
       dam -= dam / 2;
      }
    }

    if ( victim->level < L_APP && victim->class == CLASS_VAMPIRE )
     if ( !IS_SET( victim->in_room->room_flags, ROOM_INDOORS ) )
     {
      if ( time_info.hour > 6 && time_info.hour < 18 )
      {
       send_to_char( AT_RED,
       "The sunlight has weakened your defence.\n\r", victim );
       dam += dam / 2;
      }
    }

    if ( ch->class == CLASS_VAMPIRE )
     if ( !IS_SET( ch->in_room->room_flags, ROOM_INDOORS ) )
     {
      if ( time_info.hour > 23 || time_info.hour < 1 )
      {
	dam *= 2;
      }
    }

    if ( victim->class == CLASS_VAMPIRE )
     if ( !IS_SET( victim->in_room->room_flags, ROOM_INDOORS ) )
     {
      if ( time_info.hour > 23 || time_info.hour < 1 )
      {
	dam /= 2;
      }
    }
*/
	/*
	 * Check for disarm, trip, parry, and dodge.
	 */
	if (    dt >= TYPE_HIT           || dt == gsn_feed
	     || dt == gsn_jab_punch      || dt == gsn_kidney_punch
	     || dt == gsn_cross_punch    || dt == gsn_roundhouse_punch
	     || dt == gsn_uppercut_punch || dt == gsn_punch
	     || dt == gsn_flykick        || dt == gsn_high_kick
	     || dt == gsn_jump_kick      || dt == gsn_spin_kick
	     || dt == gsn_kick
  	   )
	{
	    int leveldiff = ch->level - victim->level;

	    if ( IS_NPC( ch ) && number_percent( )
		< ( leveldiff < -5 ? ch->level / 2 : UMAX( 10, leveldiff ) )
		&& dam == 0 && number_bits(4) == 0)
		disarm( ch, victim );
	    if ( IS_NPC( ch ) && number_percent( )
		< ( leveldiff < -5 ? ch->level / 2 : UMAX( 20, leveldiff ) )
		&& dam == 0 && number_bits(4) == 0)
		trip( ch, victim );
	    if ( check_parry( ch, victim ) && dam > 0 )
		return;
	    if ( check_dodge( ch, victim ) && dam > 0 )
		return;
	    if ( check_sidestep( ch, victim ) && dam > 0 )
		return;
       }
    if ( !IS_NPC( ch )
    && ( dt == 1014 || dt == 1015                   /* FIST & CLAW   */
		    || ( dt == 1005                 /* DAMAGE - Hann */
			 && is_class( ch, CLASS_WEREWOLF )
			 && is_class( ch, CLASS_MONK )
			 && is_bare_hand( ch ) ) )
    && ch->pcdata->learned[gsn_anatomyknow] > 0
    && number_percent( ) <= ch->pcdata->learned[gsn_anatomyknow] / 9 )
	{
	update_skpell( ch, gsn_anatomyknow );
	send_to_char( AT_RED, "You hit a pressure point!\n\r", ch );
	act( AT_RED, "$n hit one of $N's pressure points!",
		     ch, NULL, victim, TO_NOTVICT );
	act( AT_RED, "$n hit you with a precise shot.",
		     ch, NULL, victim, TO_VICT );
	if ( number_percent( ) < 2 )
		{
		victim->hit = 1;
		}
	else if ( number_percent( ) < 10 )
		{
		STUN_CHAR( victim, 2, STUN_TOTAL );
		victim->position = POS_STUNNED;
		dam *= 4;
		}
	else
		dam += 500;
	}


	if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
	    dam /= 2;
        if ( IS_AFFECTED2( victim, AFF_GOLDEN ) )
	    dam /= 4;
	if ( IS_AFFECTED2( victim, AFF_DANCING ) )
	    dam += dam/2;
	if ( IS_AFFECTED( victim, AFF_FIRESHIELD )
        && !( dt == gsn_backstab && chance( number_range( 5, 10 ) ) ) )
	    dam -= dam / 8;
	if ( IS_AFFECTED( victim, AFF_ICESHIELD )
        && !( dt == gsn_backstab && chance( number_range( 5, 10 ) ) ) )
	    dam -= dam / 8;
	if ( IS_AFFECTED( victim, AFF_CHAOS )
        && !( dt == gsn_backstab && chance( number_range( 5, 10 ) ) ) )
	    dam -= dam / 4;
	if ( IS_AFFECTED( victim, AFF_SHOCKSHIELD )
        && !( dt == gsn_backstab && chance( number_range( 5, 10 ) ) ) )
	    dam -= dam / 4;
	if ( IS_AFFECTED( victim, AFF_VIBRATING )
        && !( dt == gsn_backstab && chance( number_range( 5, 10 ) ) ) )
	    dam -= dam / 4;
	if ( IS_AFFECTED2( victim, AFF_INERTIAL ) )
	    dam -= dam / 8;
        if ( IS_SET( victim->act, UNDEAD_TYPE( victim ) ) )
            dam -= dam / 8;
	if ( IS_AFFECTED2( victim, AFF_BLADE )
        && !( dt == gsn_backstab && chance( number_range( 5, 10 ) ) ) )
	    dam -= dam / 4;
	if ( IS_AFFECTED2( victim, AFF_FIELD )
        && !( dt == gsn_backstab && chance( number_range( 5, 10 ) ) ) )
	    dam -= dam / 4;
	if ( IS_AFFECTED( victim, AFF_PROTECT   )
	    && IS_EVIL( ch ) )
	    dam -= dam / 4;
	if ( IS_AFFECTED2( victim, AFF_PROTECTION_GOOD )
	    && IS_EVIL( ch ) )
	    dam -= dam / 4;
	if ( dam < 0 )
	    dam = 0;

    }

    /* We moved dam_message out of the victim != ch if above
     * so self damage would show.  Other valid type_undefined
     * damage is ok to avoid like mortally wounded damage - Kahn
     */
    if ( ( !IS_NPC(ch) ) && ( !IS_NPC(victim) ) )
       dam -= dam/4;
    if ( dt != TYPE_UNDEFINED )
        dam_message( ch, victim, dam, dt );

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    if ( !IS_NPC(ch) && prime_class( ch ) == CLASS_WARRIOR )
       dam += dam/2;
    if ( ( !IS_NPC(ch) ) && (ch->race == RACE_OGRE ) )
       dam += dam/10;
    if (!IS_NPC(ch) && !IS_NPC(victim))
      dam /= number_range(2, 4);
    if ( dam > 25 && number_range( 0, 100 ) <= 15 )
      item_damage(victim, dam);

    victim->hit -= dam;
    if ( ( ( !IS_NPC( victim )                  /* so imms only die by */
	   && IS_NPC( ch )                      /* the hands of a PC   */
	   && victim->level >= LEVEL_IMMORTAL )
	 ||
	   ( !IS_NPC( victim )                   /* so imms don,t die  */
	   && victim->level >= LEVEL_IMMORTAL    /* by poison type dmg */
	   && ch == victim ) )                   /* since an imm == pc */
	 && victim->hit < 1 )
	    victim->hit = 1;

    if ( dam > 0 && dt > TYPE_HIT
	&& ( ( is_wielding_poisoned( ch )
	&& !saves_spell( ch->level, victim ) )
	|| ( is_class( ch, CLASS_WEREWOLF ) && is_bare_hand( ch ) ) )
        && victim->race != RACE_GHOUL )
    {
	AFFECT_DATA af;

	af.type      = gsn_poison;
	af.duration  = 1;
	af.location  = APPLY_STR;
	af.modifier  = -2;
	af.bitvector = AFF_POISON;
	affect_join( victim, &af );
    }
    if ( dam > 0 && dt > TYPE_HIT
       && is_wielding_flaming( ch )
       && number_percent( ) < 10 )
          spell_fireball ( skill_lookup("fireball"), 20, ch, victim );
  if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    if ( dam > 0 && ( dt == 1014
		 || dt == 1015
		 || ( dt == 1005
		      && is_class( ch, CLASS_WEREWOLF )
		      && is_class( ch, CLASS_MONK )
		      && is_bare_hand( ch ) ) )
    && is_affected( ch, gsn_flamehand )
    && number_percent( ) < 33 )
	  spell_burning_hands( skill_lookup("burning hands"),
	  ch->level, ch, victim );
  if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    if ( dam > 0 && dt > TYPE_HIT
       && is_wielding_icy( ch )
       && number_percent( ) < 20 )
          spell_icestorm ( skill_lookup("icestorm"), 30, ch, victim );
  if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;
    if ( dam > 0 && ( dt == 1014
		 || dt == 1015
		 || ( dt == 1005
		      && is_class( ch, CLASS_WEREWOLF )
		      && is_class( ch, CLASS_MONK )
		      && is_bare_hand( ch ) ) )
    && is_affected( ch, gsn_frosthand )
    && number_percent( ) < 20 )
	spell_chill_touch( skill_lookup("chill touch"), ch->level * 3,
	ch, victim );
  if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    if ( dam > 0 && dt > TYPE_HIT
       && is_wielding_chaos( ch )
       && number_percent( ) < 20 )
          spell_energy_drain ( skill_lookup("energy drain"), 45, ch, victim );
   if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;
   if ( dam > 0 && ( dt == 1014
		 || dt == 1015
		 || ( dt == 1005
		      && is_class( ch, CLASS_WEREWOLF )
		      && is_class( ch, CLASS_MONK )
		      && is_bare_hand( ch ) ) )
  && is_affected( ch, gsn_chaoshand )
  && number_percent( ) < 10 )
	spell_energy_drain( skill_lookup("energy drain"),
	ch->level * 1.5, ch, victim );
  if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    if (  IS_SET( ch->act, UNDEAD_TYPE( ch ) )
       && !saves_spell(ch->level, victim )
       && !is_class( victim, CLASS_NECROMANCER )
       && victim->race != RACE_GHOUL )
    {
	AFFECT_DATA af;

	af.type      = gsn_poison;
	af.duration  = 2;
	af.location  = APPLY_CON;
	af.modifier  = -1;
	af.bitvector = AFF_POISON;
	affect_join( victim, &af );
    }

    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	send_to_char(AT_RED,
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
	act(AT_RED, "$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	break;

    case POS_INCAP:
	send_to_char(AT_RED,
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
	act(AT_RED, "$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	break;

    case POS_STUNNED:
	send_to_char(AT_WHITE,"You are stunned, but will probably recover.\n\r",
	    victim );
	act(AT_WHITE, "$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM );
	break;

    case POS_DEAD:
	send_to_char(AT_BLOOD, "You have been KILLED!!\n\r\n\r", victim );
	act(AT_BLOOD, "$n is DEAD!!", victim, NULL, NULL, TO_ROOM );
	break;

    default:
	if ( dam > MAX_HIT(victim) / 4 )
	    send_to_char(AT_RED, "That really did HURT!\n\r", victim );
	if ( victim->hit < MAX_HIT(victim) / 4 )
	    send_to_char(AT_RED, "You sure are BLEEDING!\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE( victim ) )
	stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
        if ( !IS_ARENA(ch) )
        {
	  group_gain( ch, victim );

          if(((ch->guild != NULL) ? ch->guild->type & GUILD_CHAOS : 0)
           && ch->guild == victim->guild
           && victim->guild_rank > ch->guild_rank)
          {
            int temp;
            temp = ch->guild_rank;
            ch->guild_rank = victim->guild_rank;
            victim->guild_rank = temp;
          }
          if ( ( !IS_NPC(ch) ) && ( !IS_NPC(victim) ) )
          {
            CLAN_DATA  *pClan;
            CLAN_DATA  *Cland;
            if ( ch->clan != victim->clan )
            {
              if ( (pClan = get_clan_index(ch->clan)) != NULL )
                pClan->pkills++;
              if ( (Cland = get_clan_index(victim->clan)) != NULL )
                Cland->pdeaths++;
            }
/*            REMOVE_BIT(victim->act, PLR_THIEF);*/
          }
          if ( ( !IS_NPC(ch) ) && ( IS_NPC(victim) ) )
          {
           CLAN_DATA    *pClan;
           if ( (pClan=get_clan_index(ch->clan)) != NULL )
             pClan->mkills++;
          }
          if ( ( IS_NPC(ch) ) && (!IS_NPC(victim)) )
          {
           CLAN_DATA   *pClan;
           if ( (pClan=get_clan_index(victim->clan)) != NULL )
             pClan->mdeaths++;
          }

	  if ( !IS_NPC( victim ) )
	  {
	    /*
	     * Dying penalty:
	     * 1/2 way back to previous level.
	     */
	    if ( victim->level < LEVEL_HERO
	    || ( victim->level >= LEVEL_HERO && IS_NPC( ch ) ) )
		death_xp_loss( victim );
	    sprintf( log_buf, "%s killed by %s at %d.", victim->name,
	        ch->name, victim->in_room->vnum );
	    log_string( log_buf, CHANNEL_LOG, -1 );
            wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);
	    if ( !IS_NPC( ch )
	    && IS_SET( victim->act, PLR_THIEF )
	    && ch->guild
	    && !strcmp( ch->guild->name, "MERCENARY" ) )
		{
		REMOVE_BIT( victim->act, PLR_THIEF );
		info( "%s the puny thief gets destroyed by the &rMERCENARY&C %s!",
		      (int)victim->name, (int)ch->name );
		}
	    else if ( !IS_NPC( ch )
	    && IS_SET( ch->act, PLR_THIEF )
	    && victim->guild
	    && !strcmp( victim->guild->name, "MERCENARY" ) )
		info( "%s, the sly thief, has killed the &rMERCENARY&C %s.",
		      (int)ch->name, (int)victim->name );
	    else
	        info( "%s gets slaughtered by %s!", (int)victim->name,
                  (int)(IS_NPC(ch) ? ch->short_descr : ch->name) );
	    save_clans();
	  }
	}
	else
	{
	  sprintf(log_buf, "&C%s &chas defeated &C%s &cin the arena!",
	          ch->name, victim->name);
	  wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);
	  log_string(log_buf, CHANNEL_LOG, -1);
	  challenge(log_buf, 0, 0);
	}

	raw_kill( ch, victim );

	/* Ok, now we want to remove the deleted flag from the
	 * PC victim.
	 */
	if ( !IS_NPC( victim ) )
	    victim->deleted = FALSE;

	if ( !IS_NPC( ch ) && IS_NPC( victim ) )
	{
	    if ( IS_SET( ch->act, PLR_AUTOLOOT ) )
		do_get( ch, "all corpse" );
	    else
		do_look( ch, "in corpse" );

	    if ( IS_SET( ch->act, PLR_AUTOCOINS ) )
	        do_get( ch, "all.coin corpse" );
	    if ( IS_SET( ch->act, PLR_AUTOSAC  ) )
		do_sacrifice( ch, "corpse" );
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
	      && victim->hit < MAX_HIT(victim) / 2 )
	    || ( IS_AFFECTED( victim, AFF_CHARM ) && victim->master
		&& victim->master->in_room != victim->in_room ) )
	    do_flee( victim, "" );
    }

    if ( !IS_NPC( victim )
	&& victim->hit   > 0
	&& victim->hit  <= victim->wimpy
	&& victim->wait == 0 )
	do_flee( victim, "" );

    tail_chain( );
    return;
}

void item_damage( CHAR_DATA *ch, int dam )
{
        OBJ_DATA  *obj_lose;
        OBJ_DATA  *obj_next;

	for ( obj_lose = ch->carrying; obj_lose; obj_lose = obj_next )
	{
            char *msg;

	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted )
	        continue;
	    if ( number_bits( 2 ) != 0 )
   		continue;
#ifdef NEW_MONEY
/* Check if total cost in copper is less than 500,000 */

	    if ( ( (obj_lose->pIndexData->cost.gold*C_PER_G) +
		    (obj_lose->pIndexData->cost.silver*S_PER_G) +
		    (obj_lose->pIndexData->cost.copper) ) < 5000*100 )
		continue;
#else
            if ( obj_lose->pIndexData->cost < 5000 )
                continue;
#endif
	    if ( obj_lose->wear_loc == WEAR_NONE )
	        continue;
	    if ( IS_SET( obj_lose->extra_flags, ITEM_NO_DAMAGE ) )
	        continue;
       	    switch ( obj_lose->item_type )
	    {
	    default:
	      msg = "Your $p gets ruined!";
	      extract_obj( obj_lose );
	      break;
	    case ITEM_DRINK_CON:
	    case ITEM_POTION:
	    case ITEM_CONTAINER:
	    case ITEM_LIGHT:
	      msg = "Your $p shatters!";
	      extract_obj( obj_lose );
	      break;
            case ITEM_WEAPON:
	    case ITEM_ARMOR:
#ifdef NEW_MONEY
/*		 if ( ( obj_lose->cost.gold +
		      (obj_lose->cost.silver/SILVER_PER_GOLD) +
		      (obj_lose->cost.copper/COPPER_PER_GOLD) ) != 0 )
*/
		 if ( ( (obj_lose->cost.gold*100) +
			(obj_lose->cost.silver*10) +
			(obj_lose->cost.copper) ) != 0 )
		 {
		    obj_lose->cost.gold   = ( obj_lose->cost.gold > 0 ) ?
					    (obj_lose->cost.gold - dam/6) : 0;
		    obj_lose->cost.silver = ( obj_lose->cost.silver > 0 ) ?
					    (obj_lose->cost.silver - dam/6) : 0;
		    obj_lose->cost.copper = ( obj_lose->cost.copper > 0 ) ?
					    (obj_lose->cost.copper - dam/6) : 0;
		 }
/*		 if ( (obj_lose->cost.gold +
 		      (obj_lose->cost.silver/SILVER_PER_GOLD) +
		      (obj_lose->cost.copper/COPPER_PER_GOLD) ) < 0 )
*/
		 if ( ( (obj_lose->cost.gold*100) +
			(obj_lose->cost.silver*10) +
			(obj_lose->cost.copper) ) < 0 )
#else
              	 if ( obj_lose->cost != 0 )
              	    obj_lose->cost -= dam/6;
              	 if (obj_lose->cost < 0)
#endif
              	   {
              	    OBJ_DATA       *pObj;
              	    OBJ_INDEX_DATA *pObjIndex;
              	    char           *name;
              	    char           buf[MAX_STRING_LENGTH];

              	    pObjIndex = get_obj_index(4);
              	    pObj = create_object(pObjIndex, obj_lose->level);
              	    name = obj_lose->short_descr;
              	    sprintf(buf, pObj->description, name);
              	    free_string(pObj->description);
              	    pObj->description = str_dup(buf);
              	    pObj->weight = obj_lose->weight;
              	    pObj->timer = obj_lose->level;
              	    msg = "$p has been destroyed!";
              	    extract_obj( obj_lose );
              	    obj_to_room ( pObj, ch->in_room );
              	   }
              	  else
              	   {
              	    msg = "$p has been damaged!";
              	   }
              	  break;

	    }

	    act(AT_YELLOW, msg, ch, obj_lose, NULL, TO_CHAR );
            return;
  	}
    return;
}

bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
  CLAN_DATA *pClan;

  if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) ||
       IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
    return TRUE;

  if ( !IS_NPC(ch) && !IS_NPC(victim) &&
       (IS_SET(ch->in_room->room_flags, ROOM_NO_PKILL) ||
	IS_SET(victim->in_room->room_flags, ROOM_NO_PKILL)) )
    return TRUE;

  if ( IS_AFFECTED( ch, AFF_PEACE ) )
      return TRUE;

  if ( IS_SET( ch->in_room->room_flags, ROOM_PKILL ) &&
       IS_SET( victim->in_room->room_flags, ROOM_PKILL ) )
    return FALSE;

  if ( IS_NPC( victim ) )
    return FALSE;

  if ( IS_SET( victim->act, PLR_THIEF )
  && ( ch->guild && !strcmp( ch->guild->name, "MERCENARY" ) ) )
	return FALSE;
  if ( IS_SET( ch->act, PLR_THIEF )
  && ( victim->guild && !strcmp( victim->guild->name, "MERCENARY" ) ) )
/*	return FALSE; */
        return TRUE;

/* SIGH
  if ( !(IS_SET(ch->act, PLR_PKILLER)) || ( (IS_SET(ch->act, PLR_PKILLER)) &&
       !(IS_SET(victim->act, PLR_PKILLER)) ) )
  {
    send_to_char(AT_WHITE, "You cannot pkill unless you are BOTH pkillers!\n\r", ch );
    return TRUE;
  }
*/

  if ( abs(ch->level - victim->level) > 5 && ( !IS_NPC(ch) ) )
  {
    send_to_char(AT_WHITE, "That is not in the pkill range... valid range is +/- 5 levels.\n\r", ch );
    return TRUE;
  }

  if ( IS_NPC( ch ) )
  {
    if ( IS_SET(ch->affected_by, AFF_CHARM) && ch->master )
    {
      CHAR_DATA *nch;

      for ( nch = ch->in_room->people; nch; nch = nch->next )
	if ( nch == ch->master )
	  break;

      if ( nch == NULL )
	return FALSE;
      else
	ch = nch; /* Check person who ordered mob for clan stuff.. */
    }
    else
      return FALSE;
  }

/* give pkill guilds ability to pkill */
/* Err.. we might not want pkill guilds attacking unguilded.. */
  if ( (ch->guild && (ch->guild->type & GUILD_PKILL)) &&
       (victim->guild && (victim->guild->type & GUILD_PKILL)) )
  {
    return FALSE;
  }

  pClan = get_clan_index( ch->clan );

  if ( ( ch->clan == 0 ) && ( !IS_SET(pClan->settings, CLAN_PKILL) ) )
  {
    send_to_char(AT_WHITE, "You must be clanned to murder.\n\r", ch );
    return TRUE;
  }

  pClan = get_clan_index( victim->clan);

  if ( ( victim->clan == 0 ) && ( !IS_SET(pClan->settings, CLAN_PKILL) ) )
  {
    send_to_char(AT_WHITE, "You can only murder clanned players.\n\r",ch);
    return TRUE;
  }

  pClan = get_clan_index( ch->clan );

  if ( ch->clan == victim->clan &&
       IS_SET( pClan->settings, CLAN_CIVIL_PKILL) )
  {
   return FALSE;
  }

/* can murder self for testing =) */
  if ( ch->clan == victim->clan && ch != victim && ch->clan != 0 )
  {
    send_to_char(AT_WHITE, "You cannot murder your own clan member.\n\r",ch);
    return TRUE;
  }

  if ( !IS_SET(pClan->settings, CLAN_PKILL) )
  {
    send_to_char(AT_WHITE, "Peaceful clan members cannot murder.\n\r",ch);
    return TRUE;
  }

  pClan = get_clan_index( victim->clan );
  if ( !IS_SET(pClan->settings, CLAN_PKILL ))
  {
    send_to_char(AT_WHITE, "You may not murder peaceful clan members.\n\r",ch);
    return TRUE;
  }

  if ( IS_SET( victim->act, PLR_KILLER ) )
    return FALSE;


  return FALSE;
}



/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CLAN_DATA *pClan;
    char 	buf [ MAX_STRING_LENGTH ];

    if ( IS_ARENA(victim) )
      return;
    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if (   IS_NPC( victim )
	|| IS_SET( victim->act, PLR_KILLER )
	|| IS_SET( victim->act, PLR_THIEF  ) )
	return;

    /*
     * NPC's are cool of course
     * Hitting yourself is cool too (bleeding).
     * And current killers stay as they are.
     */
    if ( IS_NPC( ch )
	|| ch == victim
	|| IS_SET( ch->act, PLR_KILLER )
	|| IS_SET( ch->act, PLR_THIEF ) )
	return;
    pClan = get_clan_index( ch->clan );
    if ( /*ch->clan != 0 ||*/ ( IS_SET(pClan->settings, CLAN_PKILL) ) ||
( ch->clan == victim->clan && IS_SET(pClan->settings, CLAN_CIVIL_PKILL)) )
       return;
    send_to_char(AT_RED, "*** You are now a KILLER!! ***\n\r", ch );
    sprintf(buf,"$N is attempting to murder %s",victim->name);
    wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
    SET_BIT(ch->act, PLR_KILLER);
    save_char_obj( ch, FALSE );
    return;
}

bool is_bare_hand( CHAR_DATA *ch )
{
    if ( !get_eq_char( ch, WEAR_WIELD )
    && !get_eq_char( ch, WEAR_WIELD_2 ) )
	return TRUE;
    return FALSE;
}

/*
 * Check to see if weapon is poisoned.
 */
bool is_wielding_poisoned( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) )
	&& IS_SET( obj->extra_flags, ITEM_POISONED ) )
        return TRUE;

    return FALSE;

}
bool is_wielding_flaming( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) )
	&& IS_SET( obj->extra_flags, ITEM_FLAME ) )
        return TRUE;

    return FALSE;

}

bool is_wielding_icy( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) )
	&& IS_SET( obj->extra_flags, ITEM_ICY ) )
        return TRUE;

    return FALSE;

}
bool is_wielding_chaos( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) )
	&& IS_SET( obj->extra_flags, ITEM_CHAOS ) )
        return TRUE;

    return FALSE;

}


/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE( victim ) )
	return FALSE;

    if ( IS_NPC( victim ) )
    {
	/* Tuan was here.  :) */
	chance	= UMIN( 60, 2 * victim->level );
	if ( !get_eq_char( victim, WEAR_WIELD ) )
	    chance = chance / 2;
    }
    else
    {
	if ( !get_eq_char( victim, WEAR_WIELD ) )
	    return FALSE;
	chance	= victim->pcdata->learned[gsn_parry] / 2;
    }

    if ( ch->wait != 0 )
      chance /= 4;

    if ( number_percent( ) >= chance + victim->level - ch->level )
	return FALSE;

    update_skpell( ch, gsn_parry );

    if ( IS_SET( ch->act, PLR_COMBAT ) )
      act(AT_GREEN, "$N parries your attack.", ch, NULL, victim, TO_CHAR    );
    if ( IS_SET( victim->act, PLR_COMBAT ) )
      act(AT_GREEN, "You parry $n's attack.",  ch, NULL, victim, TO_VICT    );
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
        chance  = victim->pcdata->learned[gsn_dodge] / 2
		+ victim->pcdata->learned[gsn_dodge_two] / 2;

    if ( ch->wait != 0 )
      chance /= 4;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    update_skpell( ch, gsn_dodge );

    if ( IS_SET( ch->act, PLR_COMBAT ) )
      act(AT_GREEN, "$N dodges your attack.", ch, NULL, victim, TO_CHAR    );
    if ( IS_SET( victim->act, PLR_COMBAT ) )
      act(AT_GREEN, "You dodge $n's attack.", ch, NULL, victim, TO_VICT    );
    return TRUE;
}

bool check_sidestep( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE( victim ) )
        return FALSE;

    if ( IS_NPC( victim ) )
        /* Tuan was here.  :) */
        return FALSE;

    if ( victim->race != RACE_SHADOW )
        return FALSE;

    chance = 50;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    if ( IS_SET( ch->act, PLR_COMBAT ) )
      act(AT_GREEN, "$N sidesteps your attack.", ch, NULL, victim, TO_CHAR    );
    if ( IS_SET( victim->act, PLR_COMBAT ) )
      act(AT_GREEN, "You sidestep $n's attack.", ch, NULL, victim, TO_VICT    );
    return TRUE;
}

/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if ( victim->hit > 0 )
    {
    	if ( victim->position < POS_STUNNED )
	    victim->position = POS_STANDING;
	return;
    }

    if ( IS_NPC( victim ) || victim->hit <= -11 )
    {
	victim->position = POS_DEAD;
	return;
    }

         if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

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
	bug( buf , 0 );
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
	    fch->hunting        = NULL;
	    fch->position	= POS_STANDING;
	    if ( is_affected( fch, gsn_berserk ) )
	    {
	      affect_strip( fch, gsn_berserk );
	      send_to_char(C_DEFAULT, skill_table[gsn_berserk].msg_off,fch);
	      send_to_char(C_DEFAULT, "\n\r",fch);

	      act(C_DEFAULT, skill_table[gsn_berserk].room_msg_off,fch,
		  NULL, NULL, TO_ROOM);
	    }
	    else if ( is_affected( fch, gsn_frenzy ) )
	    {
	      affect_strip( fch, gsn_frenzy );
	      send_to_char(C_DEFAULT, skill_table[gsn_frenzy].msg_off,fch);
	      send_to_char(C_DEFAULT, "\n\r",fch);
	      act(C_DEFAULT,skill_table[gsn_frenzy].room_msg_off,fch,
		  NULL, NULL, TO_ROOM);
	    }
	    if ( IS_AFFECTED2(fch, AFF_BERSERK) )
              REMOVE_BIT(fch->affected_by2, AFF_BERSERK);
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
/*    ROOM_INDEX_DATA *location;*/
    char            *name;
    char             buf [ MAX_STRING_LENGTH ];

    /* No corpses in the arena. -- Altrag */
    if ( IS_ARENA(ch) )
    {
      CHAR_DATA *gch = (ch == arena.fch ? arena.sch : arena.fch);
      int award;

      /* Arena master takes 1/5.. *wink* */
      award = (arena.award * 4) / 5;
      sprintf(log_buf, "&C%s &chas been awarded &W%d &ccoins for %s victory.",
              gch->name, award, (gch->sex == SEX_NEUTRAL ? "its" :
              (gch->sex == SEX_MALE ? "his" : "her")));
      /* wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0); */
      log_string(log_buf, CHANNEL_LOG, -1);
      challenge(log_buf, 0, 0);
#ifdef NEW_MONEY
      gch->money.gold += award;
#else
      gch->gold += award;
#endif
      sprintf(log_buf, "You have been awarded %d gold coins for your victory."
              "\n\r", award);
      send_to_char(AT_YELLOW, log_buf, gch);
      char_from_room(gch);
      char_to_room(gch, get_room_index(ROOM_VNUM_ALTAR));
      return;
    }
      if ( !IS_NPC( ch ) && ch->level <= 20 )
    {
	char_from_room( ch );
	char_to_room( ch, get_room_index(ROOM_VNUM_MORGUE) );
    }
    if ( IS_NPC( ch ) )
    {
        /*
	 * This longwinded corpse creation routine comes about because
	 * we dont want anything created AFTER a corpse to be placed
	 * INSIDE a corpse.  This had caused crashes from obj_update()
	 * in extract_obj() when the updating list got shifted from
	 * object_list to obj_free.          --- Thelonius (Monk)
	 */
#ifdef NEW_MONEY
	if ( (ch->money.gold > 0) || (ch->money.silver > 0) ||
	     (ch->money.copper > 0) )
#else
	if ( ch->gold > 0 )
#endif
	{
	    OBJ_DATA * coins;
#ifdef NEW_MONEY
	    coins	  = create_money( &ch->money );
#else
	    coins         = create_money( ch->gold );
#endif
	    name	  = ch->short_descr;
	    corpse	  = create_object(
					  get_obj_index( OBJ_VNUM_CORPSE_NPC ),
					  0 );
	    corpse->timer = number_range( 2, 4 );
	    obj_to_obj( coins, corpse );
#ifdef NEW_MONEY
	    ch->money.gold = ch->money.silver = ch->money.copper = 0;
#else
	    ch->gold = 0;
#endif
	}
	else
	{
	    name	  = ch->short_descr;
	    corpse	  = create_object(
					  get_obj_index( OBJ_VNUM_CORPSE_NPC ),
					  0 );
	    corpse->timer = number_range( 2, 4 );
	}
    }
    else
    {
	name		= ch->name;
	corpse		= create_object(
					get_obj_index( OBJ_VNUM_CORPSE_PC ),
					0 );
	corpse->timer	= number_range( 25, 40 );
#ifdef NEW_MONEY
/* Check if ch has any money, doesn't matter about converting */

	if ( ( ( ch->money.gold + ch->money.silver +
		 ch->money.copper ) > 0 ) &&
	        ( ch->level > 5 ) )
	{
	  OBJ_DATA * coins;
	  coins = create_money( &ch->money );
	  obj_to_obj( coins, corpse );
	  ch->money.gold = ch->money.silver = ch->money.copper = 0;
	}
#else
	if ( ( ch->gold > 0 ) && ( ch->level > 5 ) )
	{
	  OBJ_DATA * coins;
	  coins = create_money( ch->gold );
	  obj_to_obj( coins, corpse );
	  ch->gold = 0;
	}
#endif
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
	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    extract_obj( obj );
	else
	    obj_to_obj( obj, corpse );
    }

/*    sprintf( buf, "NPC killed: %s -> %d", ch->name, ch->in_room->vnum );
    log_string( buf, CHANNEL_NONE, -1 );
    wiznet(buf,NULL,NULL,WIZ_MOBDEATHS,0,0);*/
    if ( ( IS_NPC( ch ) ) && ( !IS_SET( ch->act, UNDEAD_TYPE( ch ) ) ) )
       corpse->ac_vnum=ch->pIndexData->vnum;
    obj_to_room( corpse, ch->in_room );
    if ( !IS_NPC( ch ) )
    corpse_back( ch, corpse );

    return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char            *msg;
    int              vnum;
    int              door;
    OBJ_DATA *obj;

    if ( IS_ARENA(ch) )
      return;

    vnum = 0;
    switch ( number_bits( 4 ) )
    {
    default: msg  = "You hear $n's death cry.";				break;
    case  0: msg  = "$n hits the ground ... DEAD.";			break;
    case  1: msg  = "$n splatters blood on your armor.";		break;
    case  2: msg  = "$n's innards fall to the ground with a wet splat.";     break;
    case  3: msg  = "$n's severed head plops on the ground.";
	     vnum = OBJ_VNUM_SEVERED_HEAD;				break;
    case  4: msg  = "$n's heart is torn from $s chest.";
	     vnum = OBJ_VNUM_TORN_HEART;				break;
    case  5: msg  = "$n's arm is sliced from $s dead body.";
	     vnum = OBJ_VNUM_SLICED_ARM;				break;
    case  6: msg  = "$n's leg is sliced from $s dead body.";
	     vnum = OBJ_VNUM_SLICED_LEG;				break;
    }

    act(AT_BLOOD, msg, ch, NULL, NULL, TO_ROOM );

    if ( vnum != 0 )
    {
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

	obj_to_room( obj, ch->in_room );
    }

    obj		= create_object( get_obj_index(OBJ_VNUM_FINAL_TURD), 0 );
    obj->timer	= number_range( 3, 5 );
    obj_to_room( obj, ch->in_room );
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
	    act(AT_BLOOD, msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;

    return;
}



void raw_kill( CHAR_DATA *ch, CHAR_DATA *victim )
{
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;
    bool is_arena = IS_ARENA(ch);

    stop_fighting( victim, TRUE );
    if ( ch != victim )
        mprog_death_trigger( victim, ch );
    rprog_death_trigger( victim->in_room, victim );
    make_corpse( victim );

    for ( paf = victim->affected; paf; paf = paf_next )
        {
     	 paf_next = paf->next;
         affect_remove( victim, paf );
        }
    for ( paf = victim->affected2; paf; paf = paf_next )
      {
       paf_next = paf->next;
       affect_remove2( victim, paf );
      }
    victim->affected_by	= 0;
    victim->affected_by2 = 0;
    if ( IS_NPC( victim ) )
    {
	victim->pIndexData->killed++;
	kill_table[URANGE( 0, victim->level, MAX_LEVEL-1 )].killed++;
	extract_char( victim, TRUE );
	return;
    }

    extract_char( victim, FALSE );
    if ( !is_arena )
    {
      victim->armor        = 100;
      victim->hitroll      = 0;
      victim->damroll      = 0;
      victim->saving_throw = 0;
      victim->carry_weight = 0;
      victim->carry_number = 0;
    }
    victim->position     = POS_RESTING;
    victim->hit	         = UMAX( 1, victim->hit  );
    victim->mana         = UMAX( 1, victim->mana );
    victim->bp           = UMAX( 1, victim->bp   );
    victim->move         = UMAX( 1, victim->move );
    save_char_obj( victim, FALSE );
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
     * P-killing doesn't help either.
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
	    send_to_char(AT_BLUE, "You are too high level for this group.\n\r", gch );
	    continue;
	}

	if ( gch->level - lch->level <= -6 )
	{
	    send_to_char(AT_BLUE, "You are too low level for this group.\n\r",  gch );
	    continue;
	}

	xp = xp_compute( gch, victim ) / members;
/*    sprintf( buf, "%s -> gains %dxp", gch->name, xp);
    log_string( buf, CHANNEL_NONE, -1 );*/
	sprintf( buf, "You receive %d experience points.\n\r", xp );
	send_to_char(AT_WHITE, buf, gch );
	gain_exp( gch, xp );

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->deleted )
	        continue;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

            if ( !IS_NPC( ch ) )
	    if (   ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL    )
		    && IS_EVIL   ( ch ) )
		|| ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD    )
		    && IS_GOOD   ( ch ) )
		|| ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL )
		    && IS_NEUTRAL( ch ) ) )
	    {
		act(AT_BLUE, "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
		act(AT_BLUE, "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
	    }
	}
    }

    if ( IS_SET(ch->act, PLR_QUESTOR ) && IS_NPC( victim ))
    {
        if ( ch->questmob && victim == ch->questmob )
        {
           send_to_char(AT_WHITE, "You have almost completed your QUEST!\n\r", ch);
           send_to_char(AT_WHITE, "Return to the QuestMaster before your time runs out!\n\r", ch );
           ch->questmob = NULL;
        }
    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 * NOTE: New xp system by Hannibal
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    int xp;
    int xp_cap = 350;
    int align;

    align = gch->alignment - victim->alignment;
    if ( align > 500 )
    gch->alignment = UMIN( gch->alignment + ( align - 500 ) / 4, 1000 );
    else if ( align < -500 )
    gch->alignment = UMAX( gch->alignment + ( align + 500 ) / 4, -1000 );
    else
    gch->alignment -= victim->alignment / 3;

    /* mob lvl is 5 lvls lower than pc or more */
    if ( victim->level + 5 <= gch->level )
	return 0;
    /* 3-4 levels lower */
    if ( victim->level + 3 == gch->level
    || victim->level + 4 == gch->level )
	{
	xp = ( gch->level < 10 ) ? number_range( 50, 100 ) + 10 : 0;
	return xp;
	}
    /* if same lvl or up to 2 lvls lower */
    if ( victim->level > gch->level - 3
    && victim->level <= gch->level )
	{
	xp = number_range( 0, 10 ) + number_range( 10, 15 );
	xp = ( gch->level < 10 ) ? xp + number_range( 50, 100 ) : xp;
	return xp;
	}
    /* if higher lvl then... */
    xp = ( victim->level - gch->level ) * number_range( 25, 40 );
    xp = ( gch->level < 10 ) ? xp + number_range( 35, 50 ) : xp;

    /* if they kill 5 lvls bigger then them or more add 0-50xp */
    xp = ( victim->level >= gch->level + 5 ) ? xp + number_range( 0, 50 )
					     : xp;
    /* Enforce xp cap */
    xp = UMIN( xp_cap, xp );
    xp = UMAX( 0, xp );

    /* champ bonus */
    if ( gch->level >= LEVEL_HERO && victim->level > gch->level )
	xp = xp + ( 10 * ( victim->level - gch->level ) );
    return xp;

/* OLD xp system
    int xp;
    int align;
    int extra;
    int level;
    int number;

    xp    = 150 - URANGE( -10, gch->level - victim->level, 10 ) * 30;
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
	gch->alignment -= victim->alignment / 3;
	xp = 3 * xp / 4;
    }
*/
    /*
     * Adjust for popularity of target:
     *   -1/8 for each target over  'par' (down to - 50%)
     *   +1/8 for each target under 'par' (  up to + 25%)
     */
/*    level  = URANGE( 0, victim->level, 99 );
    number = UMAX( 1, kill_table[level].number );
    if(IS_NPC(victim))
      extra  = victim->pIndexData->killed - kill_table[level].killed / number;
    else
      extra = 0;
    xp    -= xp * URANGE( -2, extra, 4 ) / 8;

    xp     = number_range( xp * 3 / 4, xp * 5 / 4 );
    xp     = UMAX( 0, xp );
    return xp;
*/
}



void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    static char * const  attack_table [ ] =
    {
	"hit",
	"slice",  "stab",    "slash", "whip", "claw",
	"blast",  "pound",   "crush", "grep", "bite",
	"pierce", "suction", "chop", "left fist", "right fist"
    };
    const  char         *vs;
    const  char         *vp;
    const  char         *attack;
           char          buf          [ MAX_STRING_LENGTH ];
           char          buf1         [ 256 ];
           char          buf2         [ 256 ];
           char          buf3         [ 256 ];
           char          buf4         [ 256 ];
           char          buf5         [ 256 ];
           char          punct;

	 if ( dam ==   0 ) { vs = "miss";	vp = "misses";		}
    else if ( dam <=   4 ) { vs = "scratch";	vp = "scratches";	}
    else if ( dam <=   8 ) { vs = "graze";	vp = "grazes";		}
    else if ( dam <=  12 ) { vs = "hit";	vp = "hits";		}
    else if ( dam <=  16 ) { vs = "injure";	vp = "injures";		}
    else if ( dam <=  20 ) { vs = "wound";	vp = "wounds";		}
    else if ( dam <=  24 ) { vs = "maul";       vp = "mauls";		}
    else if ( dam <=  28 ) { vs = "decimate";	vp = "decimates";	}
    else if ( dam <=  32 ) { vs = "devastate";	vp = "devastates";	}
    else if ( dam <=  36 ) { vs = "maim";	vp = "maims";		}
    else if ( dam <=  40 ) { vs = "MUTILATE";	vp = "MUTILATES";	}
    else if ( dam <=  44 ) { vs = "DISEMBOWEL";	vp = "DISEMBOWELS";	}
    else if ( dam <=  48 ) { vs = "EVISCERATE";	vp = "EVISCERATES";	}
    else if ( dam <=  52 ) { vs = "MASSACRE";	vp = "MASSACRES";	}
    else if ( dam <= 100 ) { vs = "*** DEMOLISH ***";
			     vp = "*** DEMOLISHES ***";			}
    else if ( dam <= 150 ) { vs = "*** DEVASTATE ***";
			     vp = "*** DEVASTATES ***";			}
    else if ( dam <= 250 ) { vs = "*** OBLITERATE ***";
			     vp = "*** OBLITERATES ***";		}
    else if ( dam <= 300 ) { vs = "=== OBLITERATE ===";
			     vp = "=== OBLITERATES ===";		}
    else if ( dam <= 500 ) { vs = "*** ANNIHILATE ***";
			     vp = "*** ANNIHILATES ***";		}
    else if ( dam <= 750 ) { vs = ">>> ANNIHILATE <<<";
			     vp = ">>> ANNIHILATES <<<";		}
    else if ( dam <= 1000) { vs = "<<< ERADICATE >>>";
			     vp = "<<< ERADICATES >>>";			}
    else                   { vs = "&Xdo &rUNSPEAKABLE&X things to";
			     vp = "&Xdoes &rUNSPEAKABLE&X things to";
}

    punct   = ( dam <= 24 ) ? '.' : '!';

    if ( dt == TYPE_HIT )
    {
	sprintf( buf1, "You &r%s&X $N%c",       vs, punct );
	sprintf( buf2, "$n &G%s&X you%c",       vp, punct );
	sprintf( buf3, "$n &z%s&X $N%c",        vp, punct );
	sprintf( buf4, "You &G%s&X yourself%c", vs, punct );
	sprintf( buf5, "$n &z%s&X $mself%c",    vp, punct );
    }
    else
    {
	if ( is_sn(dt) )
	    attack	= skill_table[dt].noun_damage;
	else if (   dt >= TYPE_HIT
		 && dt  < TYPE_HIT
		        + sizeof( attack_table )/sizeof( attack_table[0] ) )
	    attack	= attack_table[dt - TYPE_HIT];
	else
	{
	    sprintf( buf, "Dam_message: bad dt %d caused by %s.", dt,
		    ch->name );
	    bug( buf, 0 );
	    dt      = TYPE_HIT;
	    attack  = attack_table[0];
	}

	if ( dt > TYPE_HIT && is_wielding_poisoned( ch ) )
	{
    sprintf( buf1, "Your &gpoisoned&X %s &r%s&X $N%c",  attack, vp, punct );
    sprintf( buf2, "$n's &gpoisoned&X %s &G%s&X you%c", attack, vp, punct );
    sprintf( buf3, "$n's &gpoisoned&X %s &z%s&X $N%c",  attack, vp, punct );
    sprintf( buf4, "Your &gpoisoned&X %s &G%s&X you%c", attack, vp, punct );
    sprintf( buf5, "$n's &gpoisoned&X %s &z%s&X $n%c",  attack, vp, punct );
	}
	else
	{
	    sprintf( buf1, "Your %s &r%s&X $N%c",  attack, vp, punct );
	    sprintf( buf2, "$n's %s &G%s&X you%c", attack, vp, punct );
	    sprintf( buf3, "$n's %s &z%s&X $N%c",  attack, vp, punct );
	    sprintf( buf4, "Your %s &G%s&X you%c", attack, vp, punct );
	    sprintf( buf5, "$n's %s &z%s&X $n%c",  attack, vp, punct );
	}
    }

    if ( victim != ch )
    {
        if ( dam != 0 || IS_SET( ch->act, PLR_COMBAT ) )
	  act(AT_WHITE, buf1, ch, NULL, victim, TO_CHAR    );
	if ( dam != 0 || IS_SET( victim->act, PLR_COMBAT ) )
	  act(AT_WHITE, buf2, ch, NULL, victim, TO_VICT    );
	act(AT_GREY, buf3, ch, NULL, victim,
	    dam == 0 ? TO_COMBAT : TO_NOTVICT );
    }
    else
    {
        if ( dam != 0 || IS_SET( ch->act, PLR_COMBAT ) )
	  act(AT_WHITE, buf4, ch, NULL, victim, TO_CHAR    );
  	act(AT_GREY, buf5, ch, NULL, victim,
	    dam == 0 ? TO_COMBAT : TO_NOTVICT );
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

    if ( !( obj = get_eq_char( victim, WEAR_WIELD ) ) )
	if ( !( obj = get_eq_char( victim, WEAR_WIELD_2 ) ) )
	   return;

    if ( !get_eq_char( ch, WEAR_WIELD ) && number_bits( 1 ) == 0 )
	if ( !get_eq_char( ch, WEAR_WIELD_2 ) && number_bits( 1 ) == 0 )
	   return;

    act(AT_YELLOW, "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
    act(AT_YELLOW, "$n DISARMS you!", ch, NULL, victim, TO_VICT    );
    act(AT_GREY, "$n DISARMS $N!",  ch, NULL, victim, TO_NOTVICT );

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
    if ( ( IS_AFFECTED( victim, AFF_FLYING ) )
       || (victim->race == RACE_PIXIE )
       || (victim->race == RACE_ELDER ) )
       return;

    if ( !IS_STUNNED( victim, STUN_COMMAND ) && !IS_STUNNED(ch, STUN_TO_STUN) )
    {
	act(AT_CYAN, "You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR    );
	act(AT_CYAN, "$n trips you and you go down!", ch, NULL, victim, TO_VICT    );
	act(AT_GREY, "$n trips $N and $N goes down!", ch, NULL, victim, TO_NOTVICT );

	WAIT_STATE( ch, PULSE_VIOLENCE );
	STUN_CHAR(victim, 2, STUN_COMMAND);
	STUN_CHAR(ch, 3, STUN_TO_STUN);
	victim->position = POS_RESTING;
    }

    return;
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
      send_to_char( AT_WHITE, "That person is not here.\n\r", ch );
      return;
    }

    if ( is_safe( ch, victim ) )
      {
       send_to_char( AT_WHITE, "You cannot.\n\r", ch );
       return;
      }

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Kill whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_PEACE ))
    {
      send_to_char(AT_WHITE, "A wave of peace overcomes you.\n\r", ch);
      return;
    }
    if ( IS_AFFECTED( ch, AFF_PEACE ) )
    {
	    affect_strip( ch, skill_lookup("aura of peace" ));
	    REMOVE_BIT( ch->affected_by, AFF_PEACE );
    }
    if ( !IS_NPC( victim ) && !IS_ARENA(victim) )
    {
	if (   !IS_SET( victim->act, PLR_KILLER )
	    && !IS_SET( victim->act, PLR_THIEF  ) )
	{
	    send_to_char(AT_WHITE, "You must MURDER a player.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( IS_AFFECTED( victim, AFF_CHARM ) && victim->master )
	{
	    send_to_char(AT_WHITE, "You must MURDER a charmed creature.\n\r", ch );
	    return;
	}
    }

    if ( victim == ch )
    {
	send_to_char(AT_RED, "You hit yourself.  Stupid!\n\r", ch );
	multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
	act(AT_BLUE, "$N is your beloved master!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char(C_DEFAULT, "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, PULSE_VIOLENCE );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_murde( CHAR_DATA *ch, char *argument )
{
    send_to_char(C_DEFAULT, "If you want to MURDER, spell it out.\n\r", ch );
    return;
}



void do_murder( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Murder whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_PEACE ))
    {
      send_to_char(AT_WHITE, "A wave of peace overcomes you.\n\r", ch);
      return;
    }
    if ( IS_AFFECTED( ch, AFF_PEACE ))
    {
	    affect_strip( ch, skill_lookup("aura of peace") );
	    REMOVE_BIT( ch->affected_by, AFF_PEACE );
    }

    if ( victim == ch )
    {
	send_to_char(C_DEFAULT, "Suicide is a mortal sin.\n\r", ch );
	return;
    }
    if ( is_safe( ch, victim ) )
	return;
    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
	act(C_DEFAULT, "$N is your beloved master!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !IS_SET( ch->act, PLR_PKILLER ) && !IS_ARENA( ch ) &&
	 !IS_NPC( victim ) )  /* chars can murder mobs */
    {
	send_to_char(C_DEFAULT,
 		"You must be a Pkiller to kill another mortal!\n\r", ch );
	return;
    }

    if ( !IS_SET( victim->act, PLR_PKILLER ) && !IS_ARENA( victim )
	 && !IS_NPC( victim ) ) /* chars can murder mobs */
    {
	send_to_char(C_DEFAULT, "You can only pkill other Pkillers.\n\r", ch );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char(C_DEFAULT, "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, PULSE_VIOLENCE );
    if ( !IS_NPC( victim ) )
    {
        sprintf( buf, "Help!  I am being attacked by %s!", ch->name );
        do_shout( victim, buf );
    }
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}


void do_backstab( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

      if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_backstab ) )
      {
	send_to_char(C_DEFAULT,
		   "You better leave the assassin trade to thieves.\n\r", ch );
	return;
      }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Backstab whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char(C_DEFAULT, "How can you sneak up on yourself?\n\r", ch );
	return;
    }
    if ( is_safe( ch, victim ) )
	return;

    if ( !( obj = get_eq_char( ch, WEAR_WIELD ) )
	|| ( obj->value[3] != 11 && obj->value[3] != 2 ) )
    {
	send_to_char(C_DEFAULT, "You need to wield a piercing or stabbing weapon.\n\r", ch );
	return;
    }

    if ( victim->fighting )
    {
	send_to_char(C_DEFAULT, "You can't backstab a fighting person.\n\r", ch );
	return;
    }

    if ( victim->hit < MAX_HIT(victim) * 0.9 )
    {
	act(C_DEFAULT, "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    if ( !IS_AWAKE( victim )
	|| IS_NPC( ch )
	|| number_percent( ) < ch->pcdata->learned[gsn_backstab] )
    {
	multi_hit( ch, victim, gsn_backstab );
	update_skpell( ch, gsn_backstab );
	if ( ( obj = get_eq_char( ch, WEAR_WIELD_2 ) )
	&& ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_backstab_2] > 0 )
	&& ( obj->value[3] == 11 || obj->value[3] == 2 ) )
	    {
	    multi_hit( ch, victim, gsn_backstab );
	    update_skpell( ch, gsn_backstab_2 );
	    }
    }
    else
	damage( ch, victim, 0, gsn_backstab );

    return;
}
void do_assasinate( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    if (ch->clan != 4)
      {
	send_to_char( C_DEFAULT, "Huh?\n\r", ch );
	return;
      }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Assasinate whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char(C_DEFAULT, "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( !( obj = get_eq_char( ch, WEAR_WIELD ) )
	|| obj->value[3] != 11 )
    {
	send_to_char(C_DEFAULT, "You need to wield a piercing weapon.\n\r", ch );
	return;
    }

    if ( victim->fighting )
    {
	send_to_char(C_DEFAULT, "You can't assasinate a fighting person.\n\r", ch );
	return;
    }

    if ( victim->hit < (MAX_HIT(victim)-50) )
    {
	act(C_DEFAULT, "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    multi_hit( ch, victim, gsn_backstab );

    return;
}



void do_flee( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    int              attempt;

    if ( IS_AFFECTED( ch, AFF_ANTI_FLEE ) )
    {
      send_to_char( AT_RED, "You cannot!", ch );
      return;
    }

    if ( !( victim = ch->fighting ) )
    {
	if ( ch->position == POS_FIGHTING )
	    ch->position = POS_STANDING;
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_FLEE ) )
    {
      if ( !IS_ARENA(ch) )
      {
        send_to_char(C_DEFAULT, "You failed!  You lose 10 exps.\n\r", ch );
        gain_exp( ch, -10 );
      }
      else
        send_to_char(C_DEFAULT, "You failed!\n\r", ch);
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

	move_char( ch, door, FALSE );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act(C_DEFAULT, "$n has fled!", ch, NULL, NULL, TO_ROOM );
	if ( IS_ARENA(ch) )
	  act(AT_RED, "$n has escaped!  After $m!", ch, NULL, victim,
	      TO_VICT);
	ch->in_room = now_in;

	if ( !IS_NPC( ch ) )
	{
	  if ( !IS_ARENA(ch) )
	  {
	    send_to_char(C_DEFAULT, "You flee from combat!  You lose 25 exps.\n\r", ch );
	    gain_exp( ch, -25 );
	  }
	  else
	    send_to_char(C_DEFAULT, "You flee from combat!\n\r", ch);
	}

        if ( ch->fighting && IS_NPC( ch->fighting ) )
          if ( IS_SET( ch->fighting->act, ACT_TRACK ) )
            ch->fighting->hunting = ch;

	stop_fighting( ch, TRUE );
	return;
    }

    if ( !IS_ARENA(ch) )
    {
      send_to_char(C_DEFAULT, "You failed!  You lose 10 exps.\n\r", ch );
      gain_exp( ch, -10 );
    }
    else
      send_to_char(C_DEFAULT, "You failed!\n\r", ch );
    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *fch;
    char       arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_rescue ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the heroic acts to warriors.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Rescue whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char(C_DEFAULT, "What about fleeing instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && IS_NPC( victim ) )
    {
	send_to_char(C_DEFAULT, "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char(C_DEFAULT, "Too late.\n\r", ch );
	return;
    }

    if ( !( fch = victim->fighting ) )
    {
	send_to_char(C_DEFAULT, "That person is not fighting right now.\n\r", ch );
	return;
    }

    if ( !is_same_group( ch, victim ) )
    {
	send_to_char(C_DEFAULT, "Why would you want to?\n\r", ch );
	return;
    }

    if ( !check_blind ( ch ) )
        return;

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( !IS_NPC( ch ) && number_percent( ) > ch->pcdata->learned[gsn_rescue] )
    {
	send_to_char(C_DEFAULT, "You fail the rescue.\n\r", ch );
	return;
    }

    update_skpell( ch, gsn_rescue );

    act(C_DEFAULT, "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
    act(C_DEFAULT, "$n rescues you!", ch, NULL, victim, TO_VICT    );
    act(C_DEFAULT, "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );

    stop_fighting( fch, FALSE );

    set_fighting( fch, ch );

    return;
}



void do_gouge( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_gouge ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to thieves.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
        return;
    }

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
        if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_gouge].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_gouge] )
    {
	update_skpell( ch, gsn_gouge );
	damage( ch, victim, number_range( 100, ch->level*5 ), gsn_gouge );
        if ( number_percent( ) < 10 )
        {
	AFFECT_DATA af;

	af.type      = gsn_blindness;
	af.duration  = 5;
	af.location  = APPLY_HITROLL;
	af.modifier  = -10;
	af.bitvector = AFF_BLIND;
	affect_join( victim, &af );
	act( AT_GREY, "$N is blinded!", ch, NULL, victim, TO_CHAR );
    }

    update_pos( victim );
    }
    else
	damage( ch, victim, 0, gsn_gouge );

    return;
}
void do_circle( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_circle ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to thieves.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( !( obj = get_eq_char( ch, WEAR_WIELD ) )
	|| ( obj->value[3] != 11 && obj->value[3] != 2 ) )
    {
	send_to_char(C_DEFAULT, "You need to wield a piercing or stabbing weapon.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
        return;
    }

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
        if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_circle].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_circle] )
    {
	update_skpell( ch, gsn_circle );
	damage( ch, victim, number_range( 400, ch->level*9 ), gsn_circle );
        update_pos( victim );
    }
    else
	damage( ch, victim, 0, gsn_circle );

    return;
}

void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_kick ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to warriors.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
        return;
    }

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
        if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_kick] )
    {
	damage( ch, victim, number_range( 100, ch->level*3 ), gsn_kick );
	update_skpell( ch, gsn_kick );
    }
    else
	damage( ch, victim, 0, gsn_kick );

    return;
}

void do_high_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        chance = 0;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_high_kick ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to warriors.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
	return;
    }

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_high_kick].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_high_kick] )
    {
	damage( ch, victim, number_range( 150, ch->level*4 ), gsn_high_kick );
	update_skpell( ch, gsn_high_kick );
    }
    else
	damage( ch, victim, 0, gsn_high_kick );

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room )
      return;

    if ( !IS_NPC( ch ) )
	chance = ch->pcdata->mod_str + ( ch->level - victim->level ) + ch->pcdata->perm_str;

    if ( ( number_percent( ) < chance ) || IS_NPC( ch ) )
       {
	 act( AT_RED, "You hear a crunch as you connect with $N's head.", ch, NULL, victim, TO_CHAR );
	 act( AT_RED, "$n's high kick connects firmly with your head!", ch, NULL, victim, TO_VICT );
	 act( C_DEFAULT, "$n's high kick hit $N's head!", ch, NULL, victim, TO_ROOM );
	 damage( ch, victim, number_range( 20, ch->level ), gsn_high_kick );
	}

    return;
}

void do_jump_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        chance = 0;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_jump_kick ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to warriors.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
	return;
    }

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_jump_kick].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_jump_kick] )
    {
	damage( ch, victim, number_range( 200, ch->level*4 ), gsn_jump_kick );
	update_skpell( ch, gsn_jump_kick );
    }
    else
	damage( ch, victim, 0, gsn_jump_kick );

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room )
      return;

    if ( !IS_NPC( ch ) )
	chance = ch->pcdata->mod_str + ( ch->level - victim->level ) + ch->pcdata->perm_str;

    if ( ( number_percent( ) < chance ) || IS_NPC( ch ) )
       {
	 act( AT_RED, "You hear a crunch as you connect with $N's head.", ch, NULL, victim, TO_CHAR );
	 act( AT_RED, "$n's jump kick connects firmly with your head!", ch, NULL, victim, TO_VICT );
	 act( C_DEFAULT, "$n's jump kick hit $N's head!", ch, NULL, victim, TO_ROOM );
	 damage( ch, victim, number_range( 30, ch->level ), gsn_jump_kick );
	}

    return;
}

void do_spin_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        chance = 0;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_spin_kick ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to warriors.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
	return;
    }

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_spin_kick].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_spin_kick] )
    {
	damage( ch, victim, number_range( 250, ch->level*5 ), gsn_spin_kick );
	update_skpell( ch, gsn_spin_kick );
    }
    else
	damage( ch, victim, 0, gsn_spin_kick );

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room )
      return;

    if ( !IS_NPC( ch ) )
	chance = ch->pcdata->mod_str + ( ch->level - victim->level ) + ch->pcdata->perm_str;

    if ( ( number_percent( ) < chance ) || IS_NPC( ch ) )
       {
	 act( AT_RED, "You hear a crunch as you connect with $N's head.", ch, NULL, victim, TO_CHAR );
	 act( AT_RED, "$n's spin kick connects firmly with your head!", ch, NULL, victim, TO_VICT );
	 act( C_DEFAULT, "$n's spin kick hit $N's head!", ch, NULL, victim, TO_ROOM );
	 damage( ch, victim, number_range( 40, ch->level ), gsn_spin_kick  );
	}

    return;
}

void do_punch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        chance = 0;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_punch ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
	return;
    }
    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_punch].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_punch] )
    {
	damage( ch, victim, number_range( 100, ch->level*3 ), gsn_punch );
	update_skpell( ch, gsn_punch );
    }
    else
	damage( ch, victim, 0, gsn_punch );

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room )
      return;

    if ( !IS_NPC( ch ) )
	chance = ch->pcdata->mod_str + ( ch->level - victim->level ) + ch->pcdata->perm_str;

    if ( ( number_percent( ) < chance ) || IS_NPC( ch ) )
       {
	 act( AT_RED, "You hear a crunch as you connect with $N's head.", ch, NULL, victim, TO_CHAR );
	 act( AT_RED, "$n's punch connects firmly with your head!", ch, NULL, victim, TO_VICT );
	 act( C_DEFAULT, "$n's punch hit's home!", ch, NULL, victim, TO_ROOM );
	 damage( ch, victim, number_range( 1, ch->level ), gsn_punch );
	}

    return;
}

void do_jab_punch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        chance = 0;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_jab_punch ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
	return;
    }
    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_jab_punch].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_jab_punch] )
    {
	damage( ch, victim, number_range( 125, ch->level*3 ), gsn_jab_punch );
	update_skpell( ch, gsn_jab_punch );
    }
    else
	damage( ch, victim, 0, gsn_jab_punch );

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room )
      return;

    if ( !IS_NPC( ch ) )
	chance = ch->pcdata->mod_str + ( ch->level - victim->level ) + ch->pcdata->perm_str;

    if ( ( number_percent( ) < chance ) || IS_NPC( ch ) )
       {
	 act( AT_RED, "You hear a crunch as you connect with $N's head.", ch, NULL, victim, TO_CHAR );
	 act( AT_RED, "$n's jab connects firmly with your head!", ch, NULL, victim, TO_VICT );
	 act( C_DEFAULT, "$n's jab hit $N's head!", ch, NULL, victim, TO_ROOM );
	 damage( ch, victim, number_range( 1, ch->level ), gsn_jab_punch );
	}

    return;
}

void do_kidney_punch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_kidney_punch ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
	return;
    }
    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_kidney_punch].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_kidney_punch] )
    {
	damage( ch, victim, number_range( 150, ch->level*3 ), gsn_kidney_punch );
	update_skpell( ch, gsn_kidney_punch );
    }
    else
	damage( ch, victim, 0, gsn_kidney_punch );

    return;
}

void do_cross_punch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        chance = 0;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_cross_punch ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
	return;
    }
    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_cross_punch].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_cross_punch] )
    {
	damage( ch, victim, number_range( 175, ch->level*3 ), gsn_cross_punch );
	update_skpell( ch, gsn_cross_punch );
    }
    else
	damage( ch, victim, 0, gsn_cross_punch );

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room )
      return;

    if ( !IS_NPC( ch ) )
	chance = ch->pcdata->mod_str + ( ch->level - victim->level ) + ch->pcdata->perm_str;

    if ( ( number_percent( ) < chance ) || IS_NPC( ch ) )
       {
	 act( AT_RED, "You hear a crunch as you connect with $N's head.", ch, NULL, victim, TO_CHAR );
	 act( AT_RED, "$n's cross connects firmly with your head!", ch, NULL, victim, TO_VICT );
	 act( C_DEFAULT, "$n's cross hit $N's head!", ch, NULL, victim, TO_ROOM );
	 damage( ch, victim, number_range( 1, ch->level ), gsn_cross_punch );
	}

    return;
}

void do_roundhouse_punch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        chance = 0;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_roundhouse_punch ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
	return;
    }
    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_roundhouse_punch].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_roundhouse_punch] )
    {
	damage( ch, victim, number_range( 200, ch->level*3 ), gsn_roundhouse_punch );
	update_skpell( ch, gsn_roundhouse_punch );
    }
    else
	damage( ch, victim, 0, gsn_roundhouse_punch );

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room )
      return;

    if ( !IS_NPC( ch ) )
	chance = ch->pcdata->mod_str + ( ch->level - victim->level ) + ch->pcdata->perm_str;

    if ( ( number_percent( ) < chance ) || IS_NPC( ch ) )
       {
	 act( AT_RED, "You hear a crunch as you connect with $N's head.", ch, NULL, victim, TO_CHAR );
	 act( AT_RED, "$n's roundhouse connects firmly with your head!", ch, NULL, victim, TO_VICT );
	 act( C_DEFAULT, "$n's roundhouse hit $N's head!", ch, NULL, victim, TO_ROOM );
	 damage( ch, victim, number_range( 1, ch->level ), gsn_roundhouse_punch );
	}

    return;
}

void do_uppercut_punch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        chance = 0;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_uppercut_punch ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
	return;
    }
    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_uppercut_punch].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_uppercut_punch] )
    {
	damage( ch, victim, number_range( 225, ch->level*3 ), gsn_uppercut_punch );
	update_skpell( ch, gsn_uppercut_punch );
    }
    else
	damage( ch, victim, 0, gsn_uppercut_punch );

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room )
      return;

    if ( !IS_NPC( ch ) )
	chance = ch->pcdata->mod_str + ( ch->level - victim->level ) + ch->pcdata->perm_str;

    if ( ( number_percent( ) < chance ) || IS_NPC( ch ) )
       {
	 act( AT_RED, "You hear a crunch as you connect with $N's head.", ch, NULL, victim, TO_CHAR );
	 act( AT_RED, "$n's uppercut connects firmly with your head!", ch, NULL, victim, TO_VICT );
	 act( C_DEFAULT, "$n's uppercut hit $N's head!", ch, NULL, victim, TO_ROOM );
	 damage( ch, victim, number_range( 1, ch->level ), gsn_uppercut_punch );
	}

    return;
}

void do_flury( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        count = 0;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_punch ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_blindfight] > 0
    && ch->pcdata->learned[gsn_blindfight] < number_percent( ) )
    {
    if ( !check_blind( ch ) )
	return;
    }
    one_argument( argument, arg );

    if ( ( get_eq_char( ch, WEAR_WIELD ) ) || ( get_eq_char( ch, WEAR_WIELD_2 ) ) ||
	 ( get_eq_char( ch, WEAR_HOLD ) ) )
    {
	send_to_char(C_DEFAULT, "You cannot begin such a flury while your hands are full!\n\r",  ch);
	return;
    }

    victim = ch->fighting;

    if ( arg[0] != '\0' )
	if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_punch].beats );

    act( AT_RED, "You throw a flury of punches and kicks at $N!", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n throws a flury of punches and kicks at you!", ch, NULL, victim, TO_VICT );
    act( C_DEFAULT, "$n throws a flury of punches and kicks at $N!", ch, NULL, victim, TO_ROOM );

    if ( can_use_skpell( ch, gsn_punch ) && number_percent( ) < 10 )
    {
      do_punch( ch, arg );
      count++;
    }

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room || count > 2 )
      return;

    if ( can_use_skpell( ch, gsn_kick ) && number_percent( ) < 10 )
    {
      do_kick( ch, arg );
      count++;
    }

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room || count > 2 )
      return;

    if ( can_use_skpell( ch, gsn_jab_punch ) && number_percent( ) < 10 )
    {
      do_jab_punch( ch, arg );
      count++;
    }

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room || count > 2 )
      return;

    if ( can_use_skpell( ch, gsn_kidney_punch ) && number_percent( ) < 10 )
    {
      do_kidney_punch( ch, arg );
      count++;
    }

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room || count > 2 )
      return;

    if ( can_use_skpell( ch, gsn_cross_punch ) && number_percent( ) < 10 )
    {
      do_cross_punch( ch, arg );
      count++;
    }

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room || count > 2 )
      return;

    if ( can_use_skpell( ch, gsn_high_kick ) && number_percent( ) < 10 )
    {
      do_high_kick( ch, arg );
      count++;
    }

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room || count > 2 )
      return;

    if ( can_use_skpell( ch, gsn_roundhouse_punch ) && number_percent( ) < 10 )
    {
      do_roundhouse_punch( ch, arg );
      count++;
    }

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room || count > 2 )
      return;

    if ( can_use_skpell( ch, gsn_jump_kick ) && number_percent( ) < 10 )
    {
      do_jump_kick( ch, arg );
      count++;
    }

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room || count > 2 )
      return;

    if ( can_use_skpell( ch, gsn_uppercut_punch ) && number_percent( ) < 10 )
    {
      do_uppercut_punch( ch, arg );
      count++;
    }

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room || count > 2 )
      return;

    if ( can_use_skpell( ch, gsn_spin_kick ) && number_percent( ) < 10 )
    {
      do_spin_kick( ch, arg );
      count++;
    }

    if ( !victim || victim->position == POS_DEAD || !victim->in_room
	 || victim->in_room != ch->in_room || count > 2 )
      return;

    /* nothing hit */
    if ( count == 0 )
    {
    act( AT_RED, "But none of your attacks hit $N!", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "But none of $n attacks hit you!", ch, NULL, victim, TO_VICT );
    act( C_DEFAULT, "But none of $n attacks hit $N!", ch, NULL, victim, TO_ROOM );
    }
    return;
}


void do_feed( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    if ( !is_class( ch, CLASS_VAMPIRE ) )
       {
	 send_to_char( AT_WHITE, "You may not feed on the living.\n\r", ch );
	 return;
       }

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_feed ) )
    {
	send_to_char(AT_RED,
	    "You are yet to young to feast on the blood of the living.\n\r", ch );
	return;
    }
    if ( !ch->fighting )
    {
	send_to_char(AT_WHITE, "You aren't fighting anyone.\n\r", ch );
	return;
    }
/*
    if ( ch->level < L_APP && ch->class == CLASS_VAMPIRE )
     if ( !IS_SET( ch->in_room->room_flags, ROOM_INDOORS ) )
     {
      if ( time_info.hour > 6 && time_info.hour < 18 )
      {
	send_to_char(AT_RED, "You may not feed during the cursed day.\n\r", ch );
	return;
      }
    }
*/
    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
        if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	    return;
	}

    WAIT_STATE( ch, skill_table[gsn_feed].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_feed] )
       {
	  int        amnt;

  	  update_skpell( ch, gsn_feed );
	  amnt = number_range( 5, 20 );

	  if ( ( ch->bp + amnt ) > MAX_BP(ch))
	    ch->bp = MAX_BP(ch);
	  else
	    ch->bp += amnt;

	  damage( ch, victim, number_range( 1, ch->level ), gsn_feed );
       }
    else
	damage( ch, victim, 0, gsn_feed );

    return;
}



void do_disarm( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        percent;

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_disarm ) )
    {
	send_to_char(C_DEFAULT, "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if ( ( !get_eq_char( ch, WEAR_WIELD ) )
    && ( !get_eq_char( ch, WEAR_WIELD_2 ) )
    && !is_class( ch, CLASS_WEREWOLF ) )
    {
	send_to_char(C_DEFAULT, "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
        if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}

    if ( victim->fighting != ch && ch->fighting != victim )
    {
	act(C_DEFAULT, "$E is not fighting you!", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !( obj = get_eq_char( victim, WEAR_WIELD ) ) )
    {
	if ( !( obj = get_eq_char( victim, WEAR_WIELD_2 ) ) )
	{
	  send_to_char(C_DEFAULT, "Your opponent is not wielding a weapon.\n\r", ch );
	  return;
        }
    }

    if ( number_percent( ) < victim->antidisarm )
       {
         send_to_char( C_DEFAULT, "You failed.\n\r", ch );
         return;
       }

    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
    percent = number_percent( ) + victim->level - ch->level;
    if ( ( IS_NPC( ch ) && percent < 20 ) || ( ( !IS_NPC(ch) ) &&
	( percent < ch->pcdata->learned[gsn_disarm] * 2 / 3 ) ) )
    {
	disarm( ch, victim );
 	update_skpell( ch, gsn_disarm );
    }
    else
	send_to_char(C_DEFAULT, "You failed.\n\r", ch );
    return;
}



void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char(C_DEFAULT, "If you want to SLAY, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Slay whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	return;
    }

/*    if ( ch == victim )
    {
	send_to_char(C_DEFAULT, "Suicide is a mortal sin.\n\r", ch );
	return;
    }*/

    if ( ( !IS_NPC( victim ) && victim->level >= ch->level && victim != ch ) ||
	    (IS_NPC( ch ) && !IS_NPC( victim )) )
    {
	send_to_char(C_DEFAULT, "You failed.\n\r", ch );
	return;
    }
/*
    act(C_DEFAULT, "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR    );
    act(C_DEFAULT, "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
    act(C_DEFAULT, "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
*/


    sprintf( buf, "You %s.",
        ( ch->pcdata && ch->pcdata->slayusee[0] != '\0' )
        ? ch->pcdata->slayusee : "slay $N in cold blood." );
    act( AT_RED, buf, ch, NULL, victim, TO_CHAR );

    sprintf( buf, "%s %s.", ch->name,
            ( ch->pcdata && ch->pcdata->slayvict[0] != '\0' )
            ? ch->pcdata->slayvict : "slays you in cold blood!" );
    act(AT_RED, buf, ch, NULL, victim, TO_VICT );

    sprintf( buf, "%s %s.", ch->name,
            ( ch->pcdata && ch->pcdata->slayroom[0] != '\0' )
            ? ch->pcdata->slayroom : "slays $N in cold blood!");
    act(AT_RED, buf, ch, NULL, victim, TO_NOTVICT );

    sprintf( log_buf, "%s slays %s at %d.\n\r", ch->name, victim->name,
             victim->in_room->vnum );
    log_string( log_buf, CHANNEL_LOG, ch->level - 1 );
    if ( !IS_NPC( victim ) )
      wiznet( log_buf,ch, NULL, WIZ_DEATHS, 0, 0 );
    raw_kill( ch, victim );
    return;
}

int per_type( CHAR_DATA *ch, OBJ_DATA *Obj )
{
  switch ( Obj->item_type )
  {
    case ITEM_WEAPON:
      return number_range( 5, ch->level + 5 );
    case ITEM_STAFF:
      return number_range( 3, ch->level + 3 );
    case ITEM_WAND:
      return number_range( 2, ch->level + 2 );
    default:
      return number_range( 1, ch->level );
  }
  return number_range( 1, ch->level );
}

void do_throw( CHAR_DATA *ch, char *argument )
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *to_room;
  ROOM_INDEX_DATA *in_room;
  OBJ_DATA *Obj;
  EXIT_DATA *pexit;
  int dir = 0;
  int dist = 0;
  int MAX_DIST = 2;
  extern char *dir_noun [];

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  if ( arg1[0] == '\0' )
  {
    send_to_char( C_DEFAULT, "Throw what item?\n\r", ch );
    return;
  }

  if ( ( Obj = get_obj_wear( ch, arg1 ) ) == NULL )
  {
    send_to_char( C_DEFAULT,
		 "You are not wearing, wielding, or holding that item.\n\r",
		 ch );
    return;
  }

  if ( Obj->wear_loc != WEAR_WIELD && Obj->wear_loc != WEAR_WIELD_2 &&
       Obj->wear_loc != WEAR_HOLD )
  {
    send_to_char( C_DEFAULT,
		 "You are not wielding or holding that item.\n\r", ch );
    return;
  }

  if ( IS_SET( Obj->extra_flags, ITEM_NOREMOVE ) || IS_SET( Obj->extra_flags,
							    ITEM_NODROP ) )
  {
    send_to_char( C_DEFAULT, "You can't let go of it!\n\r", ch );
    return;
  }

  in_room = ch->in_room;
  to_room = ch->in_room;

  if ( ( victim = ch->fighting ) == NULL )
  {
    if ( arg2[0] == '\0' )
    {
      send_to_char( C_DEFAULT, "Throw it at who?\n\r", ch );
      return;
    }

    if ( arg3[0] == '\0' )
    {
      if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
      {
	send_to_char( C_DEFAULT, "They aren't here.\n\r", ch );
	return;
      }
    }
    else
    {
      if ( get_curr_str( ch ) >= 20 )
      {
        MAX_DIST = 3;
        if ( get_curr_str( ch ) == 25 )
          MAX_DIST = 4;
      }

      for ( dir = 0; dir < 6; dir++ )
	if ( arg2[0] == dir_name[dir][0] && !str_prefix( arg2,
							 dir_name[dir] ) )
	  break;

      if ( dir == 6 )
      {
	send_to_char( C_DEFAULT, "Throw in which direction?\n\r", ch );
	return;
      }

      if ( ( pexit = to_room->exit[dir] ) == NULL ||
	   ( to_room = pexit->to_room ) == NULL )
      {
	send_to_char( C_DEFAULT, "You cannot throw in that direction.\n\r",
		     ch );
	return;
      }

      if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
      {
	send_to_char( C_DEFAULT, "You cannot throw through a door.\n\r", ch );
	return;
      }

      for ( dist = 1; dist <= MAX_DIST; dist++ )
      {
	char_from_room( ch );
	char_to_room( ch, to_room );
	if ( ( victim = get_char_room( ch, arg3 ) ) != NULL )
	  break;

	if ( ( pexit = to_room->exit[dir] ) == NULL ||
	     ( to_room = pexit->to_room ) == NULL ||
	       IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	  sprintf( buf, "A $p flys in from $T and hits the %s wall.",
		   dir_name[dir] );
	  act( AT_WHITE, buf, ch, Obj, dir_noun[rev_dir[dir]], TO_ROOM );
	  sprintf( buf, "You throw your $p %d room%s $T, where it hits a wall.",
		   dist, dist > 1 ? "s" : "" );
	  act( AT_WHITE, buf, ch, Obj, dir_name[dir], TO_CHAR );
	  char_from_room( ch );
	  char_to_room( ch, in_room );
	  oprog_throw_trigger( Obj, ch );
	  unequip_char( ch, Obj );
	  obj_from_char( Obj );
	  obj_to_room( Obj, to_room );
	  return;
	}
      }

      if ( victim == NULL )
      {
	act( AT_WHITE,
	    "A $p flies in from $T and falls harmlessly to the ground.",
	    ch, Obj, dir_noun[rev_dir[dir]], TO_ROOM );
	sprintf( buf,
		"Your $p falls harmlessly to the ground %d room%s $T of here.",
		dist, dist > 1 ? "s" : "" );
	act( AT_WHITE, buf, ch, Obj, dir_name[dir], TO_CHAR );
	char_from_room( ch );
	char_to_room( ch, in_room );
	oprog_throw_trigger( Obj, ch );
	unequip_char( ch, Obj );
	obj_from_char( Obj );
	obj_to_room( Obj, to_room );
	return;
      }
    }
    if ( dist > 0 )
    {
      char_from_room( ch );
      char_to_room( ch, in_room );
      act( AT_WHITE, "A $p flys in from $T and hits $n!", victim, Obj,
	  dir_noun[rev_dir[dir]], TO_NOTVICT );
      act( AT_WHITE, "A $p flys in from $T and hits you!", victim, Obj,
	  dir_noun[rev_dir[dir]], TO_CHAR );
      sprintf( buf, "Your $p flew %d rooms %s and hit $N!", dist,
	      dir_name[dir] );
      act( AT_WHITE, buf, ch, Obj, victim, TO_CHAR );
      oprog_throw_trigger( Obj, ch );
      unequip_char( ch, Obj );
      obj_from_char( Obj );
      obj_to_room( Obj, to_room );
      damage( ch, victim, per_type( ch, Obj ), gsn_throw );

      update_skpell( ch, gsn_throw ); /* Throw not given to any class though */

      if ( IS_NPC( victim ) )
      {
         if ( victim->level > 3 )
             victim->hunting = ch;
      }
      return;
    }
  }
  unequip_char( ch, Obj );
  obj_from_char( Obj );
  obj_to_room( Obj, to_room );
  act( AT_WHITE, "$n threw a $p at $N!", ch, Obj, victim, TO_ROOM );
  act( AT_WHITE, "You throw your $p at $N.", ch, Obj, victim, TO_CHAR );
  oprog_throw_trigger( Obj, ch );
  damage( ch, victim, per_type( ch, Obj ), gsn_throw );
  multi_hit( victim, ch, TYPE_UNDEFINED );
  return;
}


/*
void do_track( CHAR_DATA *ch, char *argument )
{
   ROOM_INDEX_DATA *room[30];
   ROOM_INDEX_DATA *to_room;
   ROOM_INDEX_DATA *in_room;
   EXIT_DATA *pexit[30];
   char arg[MAX_STRING_LENGTH];
   int vnums[30];
   int dist[30];
   int sdir[30];
   int dir[6];
   int nsdir;

   nsdir = 1;

   argument = one_argument( argument, arg );

   if ( is_number( arg ) || arg[0] == \0' )
   {
      send_to_char( AT_WHITE, "Track what?\n\r", ch );
      return;
   }

   in_room = ch->in_room;
   to_room = ch->in_room;

   for ( dir = 0; dir != -1; dir++ )
   {
      if ( !( pexit[nsdir] = ch->in_room->exit[dir] )
        || ( !( to_room = pexit[nsdir]->to_room ) ) )
      {
        if ( dir == 6 )
          dir = -1;
        continue;

        char_from_room( ch );
        char_to_room( ch, to_room );

        if ( get_char_room( ch, arg ) )
           break;


      }

   }

   if ( dir != -1 )
   {
     sprintf( log_buf, "You sense the trail of %s to the %s.\n\r",
          arg, dir_name[dir] );
     send_to_char( AT_WHITE, log_buf, ch );
     char_from_room( ch );
     char_to_room( ch, in_room );
   }

   if ( dir == -1 )
   {
     sprintf( log_buf, "You can't sense any %s from here.\n\r", arg );
     send_to_char( AT_WHITE, log_buf, ch );
     return;
   }

   return;
}
*/

void do_drain_life(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  int	     dam;

  if(!IS_NPC(ch)
   && !can_use_skpell( ch, gsn_drain_life ) )
  {
    send_to_char(C_DEFAULT, "You failed.\n\r", ch);
    return;
  }

  if(argument[0] != '\0')
  {
    if(!(victim = get_char_room(ch, argument)))
    {
      send_to_char(C_DEFAULT, "They aren't here.\n\r", ch);
      return;
    }
  }
  else
  {
    if(!(victim = ch->fighting))
    {
      send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch);
      return;
    }
  }

  if(is_safe(ch, victim))
    return;

  WAIT_STATE(ch, skill_table[gsn_drain_life].beats);
  /* 3xlevel + 1dlevel*/
  if(IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_drain_life])
  {
    dam = ch->level + number_range(ch->level / 2, ch->level);
    damage(ch, victim, dam, gsn_drain_life);
    update_skpell(ch, gsn_drain_life);
    ch->hit = UMIN(ch->hit + dam/2, MAX_HIT(ch));
  }
  else
  {
    send_to_char(C_DEFAULT, "You failed.", ch);
    damage(ch, victim, 0, gsn_drain_life);
  }

  return;
}

void do_mental_drain(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  int	     dam;
  int	     drain;
  int        dmana;

  if(!IS_NPC(ch)
   && !can_use_skpell( ch, gsn_mental_drain ) )
  {
    send_to_char(C_DEFAULT, "You failed.\n\r", ch);
    return;
  }

  if(argument[0] != '\0')
  {
    if(!(victim = get_char_room(ch, argument)))
    {
      send_to_char(C_DEFAULT, "They aren't here.\n\r", ch);
      return;
    }
  }
  else
  {
    if(!(victim = ch->fighting))
    {
      send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch);
      return;
    }
  }

  if(is_safe(ch, victim))
    return;
  if ( ch->race == RACE_ILLITHID )
     WAIT_STATE(ch, skill_table[gsn_mental_drain].beats/3);
  else
      WAIT_STATE(ch, skill_table[gsn_mental_drain].beats);
  /* 3xlevel + 1d100*/
  if(IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_mental_drain])
  {
    dam = number_range(ch->level / 5, ch->level);
/*    drain = 2 * ch->level + number_range(ch->level / 4, ch->level * 2);*/
    dmana = drain = ch->level + number_range(ch->level / 4, ch->level);
     if ( ch->race == RACE_ILLITHID )
	{
	dam *= 1.5;
	dmana = drain *= 2.5;
	}
    damage(ch, victim, dam, gsn_mental_drain);
    update_skpell(ch, gsn_mental_drain);
/*    ch->hit = UMIN(ch->hit + dam/4, MAX_HIT(ch));*/
    if ( !IS_NPC(victim) )
    {
      dmana = UMAX(victim->mana - dmana, 0 );
      drain = UMAX(victim->move - drain, 0 );
      victim->mana -= dmana;
      victim->move -= drain;
    }
    ch->mana = UMIN(ch->mana + dmana / 2, MAX_MANA(ch));
    ch->move = UMIN(ch->move + drain / 2, MAX_MOVE(ch));
  }
  else
  {
    send_to_char(C_DEFAULT, "You failed.", ch);
    damage(ch, victim, 0, gsn_mental_drain);
  }

  return;
}

void do_stun( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;

  if (!IS_NPC(ch) && !can_use_skpell( ch, gsn_stun ) )
  {
    send_to_char(C_DEFAULT, "You failed.\n\r", ch );
    return;
  }

  if ( !(victim = ch->fighting) )
  {
    send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r",ch);
    return;
  }

  if ( victim->position == POS_STUNNED || IS_STUNNED( ch, STUN_TO_STUN ) )
    return;

  if ( ( IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_stun] ) &&
      number_percent() < (ch->level * 75) / victim->level )
  {
    STUN_CHAR( ch, 10, STUN_TO_STUN );
    STUN_CHAR( victim, 3, STUN_TOTAL );
    victim->position = POS_STUNNED;
    act( AT_WHITE, "You stun $N!", ch, NULL, victim, TO_CHAR );
    act( AT_WHITE, "$n stuns $N!", ch, NULL, victim, TO_NOTVICT );
    act( AT_WHITE, "$n stuns you!", ch, NULL, victim, TO_VICT );
    update_skpell(ch, gsn_stun);
    return;
  }

  send_to_char(C_DEFAULT, "You failed.\n\r", ch );
  return;
}

void do_berserk( CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA af;

  if ( !ch->fighting )
    return;

  if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_berserk] < number_percent() )
  {
    send_to_char(C_DEFAULT, "You failed.\n\r",ch);
    return;
  }

  af.type = gsn_berserk;
  af.level = ch->level;
  af.duration = ch->level / 10;
  af.bitvector = AFF_BERSERK;

  af.location = APPLY_DAMROLL;
  af.modifier = IS_NPC(ch) ? ch->level / 2 : ch->damroll * 2 / 3;
  affect_to_char2(ch, &af);

  af.location = APPLY_HITROLL;
  af.modifier = IS_NPC(ch) ? ch->level / 2 : ch->hitroll * 2 / 3;
  affect_to_char2(ch, &af);

  af.location = APPLY_AC;
  af.modifier = ch->level * -2;
  affect_to_char2(ch, &af);

  send_to_char(AT_WHITE, "You suddenly go berserk.\n\r",ch);
  act(AT_WHITE, "$n suddenly goes berserk!", ch, NULL, NULL, TO_ROOM );
  update_skpell( ch, gsn_berserk );
  return;
}

void do_soulstrike( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim = ch->fighting;
  int dam;

  if ( !victim )
  {
    send_to_char( AT_WHITE, "You aren't fighting anyone!\n\r", ch );
    return;
  }

  if (!IS_NPC(ch) && ch->pcdata->learned[gsn_soulstrike] < number_percent( ))
  {
    send_to_char(AT_BLUE, "You failed.\n\r", ch );
    return;
  }

  if ( IS_NEUTRAL(ch) )
  {
    send_to_char(AT_RED, "Nothing happened.\n\r", ch );
    return;
  }

  dam = number_range( ch->level / 3, (ch->level * 2) / 3 );
  if ( ch->hit < dam * 2 )
  {
    send_to_char(AT_WHITE, "You do not have the strength.\n\r", ch );
    return;
  }

/* Don't need a check for update_pos, because of the previous check. */
  ch->hit -= dam;

  dam = number_range( dam * 3, dam * 5 );
  WAIT_STATE(ch, 2*PULSE_VIOLENCE);

  if ( IS_EVIL(ch) )
  {
    send_to_char(AT_RED, "Your soul recoils!\n\r", ch);
    damage(ch, ch, dam, gsn_soulstrike);
    return;
  }

  act( AT_BLUE, "Your soul strikes deep into $N.", ch, NULL, victim, TO_CHAR );
  act( AT_BLUE, "$n's soul strikes deep into you.", ch, NULL, victim, TO_VICT );
  act( AT_BLUE, "$n's soul strikes deep into $N.", ch, NULL, victim, TO_NOTVICT );
  damage(ch, victim, dam, gsn_soulstrike);

  update_skpell( ch, gsn_soulstrike );

  return;
}
void do_multiburst( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim = ch->fighting;
  char	arg1 [ MAX_INPUT_LENGTH ];
  char	arg2 [ MAX_INPUT_LENGTH ];
  int	sn1;
  int	sn2;
  bool	legal1 = FALSE;
  bool	legal2 = FALSE;
  int	mana = 0;

  if ( IS_NPC( ch ) )
	return;
  if ( !can_use_skpell( ch, gsn_multiburst ) )
	{
	send_to_char( C_DEFAULT, "You're not enough of a mage to do multibursts.\n\r" , ch );
	return;
	}
  if ( IS_STUNNED( ch, STUN_MAGIC ) )
	{
	send_to_char( AT_LBLUE, "You are too stunned to multiburst.\n\r", ch );
	return;
	}

  if ( ch->pcdata->learned[gsn_multiburst] < number_percent( ) )
	{
	send_to_char( C_DEFAULT, "You fail your multiburst.\n\r", ch );
	return;
	}
  if ( !ch->fighting )
	{
	send_to_char( C_DEFAULT, "You aren't fighting.\n\r", ch );
	return;
	}

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  sn1 = skill_lookup( arg1 );
  sn2 = skill_lookup( arg2 );

  if ( sn1 != -1
  && can_use_skpell( ch, sn1 )
  && skill_table[sn1].target != TAR_CHAR_SELF
  && (*skill_table[sn1].spell_fun) != (*spell_null) )
	legal1 = TRUE;
  if ( sn2 != -1
  && can_use_skpell( ch, sn2 )
  && skill_table[sn2].target != TAR_CHAR_SELF
  && (*skill_table[sn2].spell_fun) != (*spell_null) )
	legal2 = TRUE;

  if ( !legal1 && !legal2 )
	{
	WAIT_STATE( ch, skill_table[gsn_multiburst].beats );
	send_to_char( C_DEFAULT, "Your multiburst fails.\n\r", ch );
	return;
	}
  if ( legal1 )
	{
	mana += SPELL_COST( ch, sn1 );
	mana += SPELL_COST( ch, sn1 ) * 0.2;
	}
  if ( legal2 )
	{
	mana += SPELL_COST( ch, sn2 );
	mana += SPELL_COST( ch, sn2 ) * 0.2;
	}
  mana += mana * 0.1;
  if ( ch->mana < mana && ch->level < LEVEL_IMMORTAL )
	{
	WAIT_STATE( ch, skill_table[gsn_multiburst].beats );
	send_to_char( C_DEFAULT, "You don't have enough mana to multiburst these spells.\n\r", ch );
	return;
	}
  WAIT_STATE( ch, skill_table[gsn_multiburst].beats );
  send_to_char( AT_RED, "You release a burst of energy!\n\r", ch );
  act( AT_RED, "$n releases a burst of energy.", ch, NULL, NULL, TO_ROOM );

  update_skpell( ch, gsn_multiburst );

  if ( legal1 )
	(*skill_table[sn1].spell_fun) ( sn1,
					URANGE( 1, ch->level, LEVEL_HERO ),
					ch, victim );
  if ( victim->position != POS_DEAD && ch->in_room == victim->in_room )
  {
  if ( legal2 )
	(*skill_table[sn2].spell_fun) ( sn2,
					URANGE( 1, ch->level, LEVEL_HERO ),
					ch, victim );
  }
  if ( ch->level < LEVEL_IMMORTAL ){
    if ( is_class( ch, CLASS_VAMPIRE) ) {
    	ch->bp -= mana;
    } else {
    	ch->mana -= mana;
    }
  }
  return;
}

void do_rage( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *wield;
  OBJ_DATA *wield2;
  char	    buf [ MAX_INPUT_LENGTH ];
  AFFECT_DATA af;

  if ( IS_AFFECTED2( ch, AFF_RAGE ) )
	return;
  if ( !can_use_skpell( ch, gsn_rage ) )
	{
	send_to_char( C_DEFAULT, "You are not enough of a beast.\n\r", ch );
	return;
	}
  if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_rage] < number_percent( ) )
	{
	send_to_char( C_DEFAULT, "You cannot summon enough anger.\n\r", ch );
	return;
	}

  wield = get_eq_char( ch, WEAR_WIELD );
  wield2 = get_eq_char( ch , WEAR_WIELD_2 );

  if ( wield || wield2 )
	{
	sprintf( buf,
        "You become enraged and toss your weapon%s to the ground.\n\r",
	( wield && wield2 ) ? "s" : "" );
	send_to_char( AT_RED, buf, ch );
	sprintf( buf,
	"$n tosses his weapon%s to the ground in a fit of rage.",
	( wield && wield2 ) ? "s" : "" );
	act( C_DEFAULT, buf, ch, NULL, NULL, TO_ROOM );
	if ( wield )
		{
		obj_from_char( wield );
		obj_to_room( wield, ch->in_room );
		}
	if ( wield2 )
		{
		obj_from_char( wield2 );
		obj_to_room( wield2, ch->in_room );
		}
	}
  else
	{
	send_to_char( AT_RED,
	"You become enraged and surge with power.\n\r", ch );
	}

  update_skpell( ch, gsn_rage );

  af.type	 = gsn_rage;
  af.level	 = ch->level;
  af.duration	 = ch->level * 0.4;
  af.location	 = APPLY_STR;
  af.modifier	 = 5;
  af.bitvector	 = AFF_RAGE;
  affect_to_char2( ch, &af );

  af.location	 = APPLY_HITROLL;
  af.modifier	 = ch->level;
  affect_to_char2( ch, &af );

  af.location	 = APPLY_DAMROLL;
  af.modifier	 = ch->level;
  affect_to_char2( ch, &af );

  return;
}

void do_challenge(CHAR_DATA *ch, char *argument)
{
  int award;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  int rvnum = ch->in_room->vnum;
#ifdef NEW_MONEY
  MONEY_DATA amount;
#endif

  if ( IS_NPC(ch) )
  {
    send_to_char( C_DEFAULT, "NPC's can't fight in the arena.\n\r", ch );
    return;
  }
  if ( rvnum == ROOM_VNUM_HELL
  || rvnum == ROOM_VNUM_RJAIL )
  {
    send_to_char( C_DEFAULT, "Nice try, but get out the real way.\n\r", ch );
    return;
  }
  if ( arena.fch && arena.sch )
  {
    send_to_char( C_DEFAULT, "There are already two people fighting in the"
                  " arena.\n\r", ch );
    return;
  }
  if ( arena.cch )
  {
    sprintf(arg1, "%s is offering a challenge.  Type accept to accept it.\n\r",
            arena.cch->name);
    send_to_char( C_DEFAULT, arg1, ch );
    return;
  }
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if ( (is_number(arg1) && (award = atoi(arg1)) < 1000000 )
  || ( !is_number(arg1) && !is_number(arg2) )
  || ( is_number(arg2) && (award = atoi(arg2)) < 1000000 ) )
  {
    send_to_char( C_DEFAULT, "Syntax: challenge [player] <award>\n\r", ch );
    send_to_char( C_DEFAULT, " Player is the optional name of a specific person to challenge.\n\r", ch);
    send_to_char( C_DEFAULT, " Award is at least 1000000 gold coins.\n\r", ch );
    return;
  }
  else if ( (award = atoi(arg2)) )
  {
    DESCRIPTOR_DATA *d;
    bool found = FALSE;
    for ( d = descriptor_list; d; d = d->next )
	{
	if ( d->connected == CON_PLAYING
	&& is_name( NULL, arg1, d->character->name )
	&& d->character && ch )
		{
		found = TRUE;
		arena.och = d->character;
		break;
		}
	}
  if ( !found )
    {
    send_to_char( C_DEFAULT, "They aren't here.\n\r", ch );
    return;
    }
  }
  else
    award = atoi(arg1);
#ifdef NEW_MONEY
/* Convert and compare copper values: */
  if ( award*100 > ( ch->money.gold*C_PER_G + ch->money.silver*S_PER_G +
                     ch->money.copper ) )
/*  if ( award > ( ch->money.gold + (ch->money.silver/SILVER_PER_GOLD) +
               (ch->money.copper/COPPER_PER_GOLD) ) )  */
  {
#else
  if ( award > ch->gold )
  {
#endif
    send_to_char( C_DEFAULT, "You can't afford that.\n\r", ch );
    arena.och = NULL;
    return;
  }
  if ( !get_room_index(ROOM_ARENA_ENTER_F) ||
       !get_room_index(ROOM_ARENA_ENTER_S) )
  {
    send_to_char( C_DEFAULT, "An error has occured.  Please inform an Immortal.\n\r", ch );
    return;
  }
  arena.cch = ch;
  arena.count = 0;
  arena.award = award;
#ifdef NEW_MONEY
  amount.silver = amount.copper = 0;
  amount.gold = award;
  spend_money( &ch->money, &amount );
#else
  ch->gold -= award;
#endif
  if ( arena.och )
  sprintf(log_buf, "&C%s &cchallenges &C%s &cto a fight in the arena for &W%d &cgold coins.",
	  ch->name, arena.och->name, award );
  else
  sprintf(log_buf, "&C%s &coffers a challenge in the arena for &W%d &cgold coins.",
          ch->name, award);
/*  wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0); */
  log_string(log_buf, CHANNEL_LOG, -1);
  challenge(log_buf, 0, 0);
  send_to_char( C_DEFAULT, "Your challenge has been offered.\n\r", ch );
  return;
}

void do_accept(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *cch, *och;
  int rvnum = ch->in_room->vnum;
#ifdef NEW_MONEY
  MONEY_DATA amount;
#endif

  if ( IS_NPC(ch) )
  {
    send_to_char( C_DEFAULT, "NPC's may not fight in the arena.\n\r", ch );
    return;
  }
  if ( !(cch = arena.cch) )
  {
    send_to_char( C_DEFAULT, "There is no challenge being offered.\n\r", ch );
    return;
  }
  if ( rvnum == ROOM_VNUM_HELL
  || rvnum == ROOM_VNUM_RJAIL )
  {
    send_to_char( C_DEFAULT, "Nice try, but get out the real way.\n\r", ch );
    return;
  }
  if ( ch->fighting )
  {
    send_to_char( C_DEFAULT, "You are already fighting.\n\r", ch );
    return;
  }
  if ( ch == cch )
  {
    send_to_char( C_DEFAULT, "You can't accept your own challenge!\n\r", ch );
    return;
  }
  if ( ( och = arena.och ) && och != ch )
  {
    send_to_char( C_DEFAULT, "You are not the one being challenged.\n\r", ch );
    return;
  }
#ifdef NEW_MONEY
  if ( ( ch->money.gold*C_PER_G + ch->money.silver*S_PER_G +
         ch->money.copper ) < arena.award*100 )
/*  if ( (ch->money.gold + (ch->money.silver/SILVER_PER_GOLD) +
       (ch->money.copper/COPPER_PER_GOLD) ) < arena.award )  */
  {
    send_to_char( C_DEFAULT, "You cannot afford that.\n\r", ch );
    return;
  }

  amount.silver = amount.copper = 0;
  amount.gold = arena.award;
  spend_money( &ch->money, &amount );
#else
  if ( ch->gold < arena.award )
  {
    send_to_char( C_DEFAULT, "You cannot afford that.\n\r", ch );
    return;
  }
  ch->gold -= arena.award;
#endif
  arena.award *= 2;
  arena.fch = cch;
  arena.sch = ch;
  arena.cch = NULL;
  arena.och = NULL;
  send_to_char( C_DEFAULT, "Your challenge has been accepted.\n\r", cch );
  stop_fighting(cch, FALSE);
  act( AT_LBLUE, "A pentagram forms around $n and he slowly dissipates.",
       cch, NULL, NULL, TO_ROOM );
  char_from_room(cch);
  char_to_room(cch, get_room_index(ROOM_ARENA_ENTER_F));
  do_look(cch, "auto");
  act( AT_LBLUE, "A pentagram forms around $n and he slowly dissipates.",
       ch, NULL, NULL, TO_ROOM );
  char_from_room(ch);
  char_to_room(ch, get_room_index(ROOM_ARENA_ENTER_S));
  do_look(ch, "auto");
  sprintf(log_buf, "&C%s &chas accepted &C%s&c's challenge.",
          ch->name, cch->name);
/*  wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0); */
  log_string(log_buf, CHANNEL_LOG, -1);
  challenge(log_buf, 0, 0);
  send_to_char(AT_RED, "Be prepared.\n\r", ch);
  send_to_char(AT_RED, "Be prepared.\n\r", cch);
  return;
}
void do_bite(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  int	     dam;
  float	     wait_mod;

  if(!IS_NPC(ch)
   && !can_use_skpell( ch, gsn_bite ) )
  {
    send_to_char(C_DEFAULT, "You gnash your teeth wildly.\n\r", ch);
    return;
  }

  if(argument[0] != '\0')
  {
    if(!(victim = get_char_room(ch, argument)))
    {
      send_to_char(C_DEFAULT, "They aren't here.\n\r", ch);
      return;
    }
  }
  else
  {
    if(!(victim = ch->fighting))
    {
      send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch);
      return;
    }
  }

  if(is_safe(ch, victim))
    return;
  if ( ch->level < 25 )
	wait_mod = 0.5;
  else if ( ch->level < 40 )
	wait_mod = 1;
  else if ( ch->level < 60 )
	wait_mod = 1.5;
  else if ( ch->level < 80 )
	wait_mod = 2;
  else
	wait_mod = 3;
  WAIT_STATE(ch, skill_table[gsn_bite].beats / wait_mod);
  if(IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_bite])
  {
    dam = ch->level + number_range(ch->level, ch->level * 5);
    damage(ch, victim, dam, gsn_bite);
    update_skpell(ch, gsn_bite);
  }
  else
  {
    send_to_char(C_DEFAULT, "You failed.", ch);
    damage(ch, victim, 0, gsn_bite);
  }

  return;
}

void do_rush( CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA af;

  if ( IS_NPC( ch ) )
	return;
  if ( IS_AFFECTED2( ch, AFF_RUSH ) )
	return;
  if ( !can_use_skpell( ch, gsn_rush ) )
	{
	send_to_char( C_DEFAULT, "You can't pump yourself up enough.\n\r", ch );
	return;
	}
  WAIT_STATE( ch, skill_table[gsn_rush].beats );
  if ( ch->pcdata->learned[gsn_rush] < number_percent( ) )
	{
	send_to_char( C_DEFAULT, "You failed.\n\r", ch );
	return;
	}
  send_to_char( AT_RED, "You pump yourself up and make the adrenaline flow.\n\r", ch );
  act( AT_WHITE, "$n seems to be pumped up.", ch, NULL, NULL, TO_ROOM );
  af.type	 = gsn_rush;
  af.level	 = ch->level;
  af.duration	 = ch->level / 4;
  af.location	 = APPLY_DEX;
  af.modifier	 = -2;
  af.bitvector	 = AFF_RUSH;
  affect_to_char2( ch, &af );

  update_skpell( ch, gsn_rush );

  return;
}
void do_howl_of_fear(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;

  if(!IS_NPC(ch)
   && is_class( ch, gsn_howlfear ) )
  {
    send_to_char(C_DEFAULT, "You failed.\n\r", ch);
    return;
  }
  if ( IS_NPC( ch ) )
	return;
  if(argument[0] != '\0')
  {
    if(!(victim = get_char_room(ch, argument)))
    {
      send_to_char(C_DEFAULT, "They aren't here.\n\r", ch);
      return;
    }
  }
  else
  {
    if(!(victim = ch->fighting))
    {
      send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch);
      return;
    }
  }

  if(is_safe(ch, victim))
    return;
  if (is_affected(victim, gsn_howlfear) )
    {
    act( AT_RED, "$N is already scared witless.", ch, NULL, victim, TO_CHAR );
    return;
    }
  WAIT_STATE(ch, skill_table[gsn_howlfear].beats);
  if(number_percent() < ch->pcdata->learned[gsn_howlfear])
  {
    AFFECT_DATA af;
    af.type	 = gsn_howlfear;
    af.level	 = ch->level;
    af.duration	 = 10;
    af.location	 = APPLY_DAMROLL;
    af.modifier	 = 0 - ch->level / 2;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location	 = APPLY_HITROLL;
    af.modifier	 = 0 - ch->level / 2;
    affect_to_char( victim, &af );

    af.level	 = ch->level;
    af.duration	 = 10;
    af.location	 = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_ANTI_FLEE;
    affect_to_char( victim, &af );

    act( AT_RED, "Your chilling howl instills fear in $N.",
         ch, NULL, victim, TO_CHAR );
    act( AT_RED, "You cringe in terror at the sound of $n's howl.",
	 ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n howls and $N cringes back in terror.",
	 ch, NULL, victim, TO_NOTVICT );
    if ( !victim->fighting )
	set_fighting( victim, ch );
  }

  update_skpell( ch, gsn_howlfear );

  return;
}

void do_frenzy( CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA af;
  if ( IS_AFFECTED2( ch, AFF_BERSERK ) )
    return;
  if ( !ch->fighting )
    return;

  if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_frenzy] < number_percent() )
  {
    send_to_char(C_DEFAULT, "You failed.\n\r",ch);
    return;
  }

  af.type = gsn_frenzy;
  af.level = ch->level;
  af.duration = ch->level / 10;
  af.bitvector = AFF_BERSERK;

  af.location = APPLY_DAMROLL;
  af.modifier = IS_NPC(ch) ? ch->level / 2 : ch->damroll * 2 / 3;
  affect_to_char2(ch, &af);

  af.location = APPLY_HITROLL;
  af.modifier = IS_NPC(ch) ? ch->level / 2 : ch->hitroll * 2 / 3;
  affect_to_char2(ch, &af);

  af.location = APPLY_AC;
  af.modifier = ch->level * -2;
  affect_to_char2(ch, &af);

  send_to_char(AT_WHITE, "You go into a frenzy.\n\r",ch);
  act(AT_WHITE, "$n suddenly goes into a frenzy", ch, NULL, NULL, TO_ROOM );
  update_skpell( ch, gsn_frenzy );
  return;
}

void do_reflex( CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA af;

  if ( IS_NPC( ch ) )
	return;
  if ( IS_AFFECTED( ch, AFF_HASTE ) )
	return;
  if ( !can_use_skpell( ch, gsn_reflex ) )
	return;
  WAIT_STATE( ch, skill_table[gsn_reflex].beats );
  if ( ch->pcdata->learned[gsn_reflex] < number_percent( ) )
	{
	send_to_char( C_DEFAULT, "You failed.\n\r", ch );
	return;
	}
  send_to_char( AT_RED, "You feel yourself more agile.\n\r", ch );
  act( AT_WHITE, "$n is moving more fluidly.", ch, NULL, NULL, TO_ROOM );
  af.type	 = gsn_reflex;
  af.level	 = ch->level;
  af.duration	 = ch->level / 4;
  af.location	 = APPLY_DEX;
  af.modifier	 = 4;
  af.bitvector	 = AFF_HASTE;
  affect_to_char( ch, &af );

  update_skpell( ch, gsn_reflex );

  return;
}

void do_rake( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch )
	&& !can_use_skpell( ch, gsn_rake ) )
    {
	send_to_char(C_DEFAULT,
	    "You'd better leave animalistic actions to werewolves.\n\r", ch );
	return;
    }

    if ( !ch->fighting )
    {
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    victim = ch->fighting;

    if ( arg[0] != '\0' )
        if ( !( victim = get_char_room( ch, arg ) ) )
	{
	    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
	    return;
	}
    if ( IS_AFFECTED( victim, AFF_BLIND ) )
	return;

    WAIT_STATE( ch, skill_table[gsn_rake].beats );
    if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_rake] )
    {
	damage( ch, victim, number_range( 100, ch->level*5 ), gsn_rake );
  	update_skpell( ch, gsn_rake );
        if ( number_percent( ) < 75 )
        {
	AFFECT_DATA af;

	af.type      = gsn_blindness;
	af.duration  = 5;
	af.location  = APPLY_HITROLL;
	af.modifier  = -10;
	af.bitvector = AFF_BLIND;
	affect_join( victim, &af );
    }

    update_pos( victim );
    }
    else
	damage( ch, victim, 0, gsn_rake );

    return;
}
void do_bladepalm( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;
  if ( IS_NPC( ch ) )
	return;
  if ( !can_use_skpell( ch, gsn_bladepalm ) )
	{
	send_to_char( AT_GREY, "You know nothing of martial arts.\n\r", ch );
	return;
	}
  if ( get_eq_char( ch, WEAR_WIELD ) && get_eq_char( ch, WEAR_WIELD_2 ) )
	{
	send_to_char( AT_GREY, "You need at least one free hand to perform a blade palm.\n\r", ch );
	return;
	}
  if ( argument[0] != '\0' )
	{
	if ( !(victim=get_char_room(ch, argument)) )
	   {
	   send_to_char(C_DEFAULT, "They aren't there.\n\r", ch );
	   return;
	   }
	}
  else
	{
	if ( !(victim = ch->fighting) )
	   {
 	   send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	   return;
	   }
	}

  if ( is_safe(ch, victim) )
	return;
  WAIT_STATE(ch, skill_table[gsn_bladepalm].beats);
  if ( ch->pcdata->learned[gsn_bladepalm] > number_percent( ) )
	{
	int anatomy = ch->pcdata->learned[gsn_anatomyknow];
	if ( anatomy > 0
	&& number_percent( ) < number_range( anatomy / 9, anatomy / 4.5 ) )
		{
		update_skpell( ch, gsn_anatomyknow );
		send_to_char( AT_RED, "You hit a pressure point!\n\r", ch );
		act( AT_RED, "$n hit one of $N's pressure points!",
		     ch, NULL, victim, TO_NOTVICT );
		act( AT_RED, "$n hit you with a precise shot.",
		     ch, NULL, victim, TO_VICT );
		if ( number_percent( ) < 5 )
			{
			victim->hit = 1;
			}
		else if ( number_percent( ) < 25 )
			{
			STUN_CHAR( victim, 3, STUN_TOTAL );
			victim->position = POS_STUNNED;
			dam += 500;
			}
		else
			dam += 300;
		}
	dam += number_fuzzy( ch->level / 2 + ch->level / 15 );
	dam += GET_DAMROLL( ch );
	if ( is_affected( ch, gsn_flamehand ) )
		dam += dam / 8;
	if ( is_affected( ch, gsn_frosthand ) )
		dam += dam / 4;
	if ( is_affected( ch, gsn_chaoshand ) )
		dam += dam / 4;
	if ( ch->pcdata->learned[gsn_enhanced_damage] > 0 )
		dam += dam / 4 * ch->pcdata->learned[gsn_enhanced_damage] / 150;
	damage( ch, victim, dam, gsn_bladepalm );

	update_skpell( ch, gsn_bladepalm );

	}
  else
	damage( ch, victim, 0, gsn_bladepalm );
  return;
}

void do_flyingkick( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[ MAX_INPUT_LENGTH ];

  if ( !IS_NPC( ch )
  && !can_use_skpell( ch, gsn_flykick ) )
    {
    send_to_char( AT_GREY, "You do not hav knowledge of martial arts.\n\r", ch );
    return;
    }
 if ( !ch->fighting )
    {
    send_to_char( AT_GREY, "You aren't fighting anyone.\n\r", ch );
    return;
    }
 one_argument( argument, arg );
 victim = ch->fighting;
 if ( arg[0] != '\0' )
    if ( !(victim=get_char_room( ch, arg )) )
	{
	send_to_char( AT_GREY, "They aren't here.\n\r", ch );
	return;
	}
 WAIT_STATE( ch, skill_table[gsn_flykick].beats );
 if ( IS_NPC( ch ) || number_percent( ) < ch->pcdata->learned[gsn_flykick] )
 {
	damage( ch, victim, number_range( 200, ch->level*6 ), gsn_flykick );
	update_skpell( ch, gsn_flykick );
 }
 else
	damage( ch, victim, 0, gsn_flykick );
 return;
}
void do_nerve( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char buf[MAX_INPUT_LENGTH];
  OBJ_DATA *wield;
  OBJ_DATA *dual;
  OBJ_DATA *hold;
  if ( IS_NPC( ch ) )
	return;
  if ( !can_use_skpell( ch, gsn_nerve ) )
	{
	send_to_char( AT_GREY, "Huh?\n\r", ch );
	return;
	}
  if ( !(victim = ch->fighting) )
  {
    send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r",ch);
    return;
  }
  if ( number_percent() < ch->pcdata->learned[gsn_nerve]
  && number_percent() < (ch->level * 75) / victim->level )
  {
	AFFECT_DATA af;
	act( AT_YELLOW, "You pinch $N's arm nerves!", ch, NULL, victim, TO_CHAR );
	act( AT_YELLOW, "$n pinched your arm nerves.", ch, NULL, victim, TO_VICT );
	act( AT_YELLOW, "$n pinched $N's arm nerves.", ch, NULL, victim, TO_NOTVICT );
	wield = get_eq_char( victim, WEAR_WIELD );
	dual = get_eq_char( victim, WEAR_WIELD_2 );
	if ( wield || dual )
		{
                if ( IS_SIMM( victim, IMM_NERVE ) )
		  {
		  sprintf( buf,
		  "$N shrugs off the blow and grips $S weapon%s tightly.",
		  ( wield && dual ) ? "s" : "" );
		  act( AT_GREY, buf, ch, NULL, victim, TO_NOTVICT );
		  }
		else
		  {
		  sprintf( buf, "$N drops $S weapon%s to the ground.",
		         ( wield && dual ) ? "s" : "" );
		  act( AT_GREY, buf, ch, NULL, victim, TO_CHAR );
		  act( AT_GREY, buf, ch, NULL, victim, TO_NOTVICT );
		  sprintf( buf, "You drop your weapon%s to the ground.\n\r",
			( wield && dual ) ? "s" : "" );
 		  send_to_char( AT_GREY, buf, victim );
		  if ( wield )
			{
			obj_from_char( wield );
			obj_to_room( wield, victim->in_room );
			}
		  if ( dual )
			{
			obj_from_char( dual );
			obj_to_room( dual, victim->in_room );
			}
		  }
		}
	if ( (hold = get_eq_char( victim, WEAR_HOLD )) )
		{
		if ( !IS_SIMM( victim, IMM_NERVE ) )
		  {
		  act( AT_GREY, "$N drops $S $p.", ch, hold, victim, TO_CHAR );
    		  act( AT_GREY, "$N drops $S $p.", ch, hold, victim, TO_NOTVICT );
		  act( AT_GREY, "You drop your $p.", ch, hold, victim, TO_VICT );
		  obj_from_char( hold );
		  obj_to_room( hold, victim->in_room );
		  }
		else
		  act( AT_GREY, "$N grips $S $p tightly.", ch, hold, victim, TO_NOTVICT );
		}
	if ( IS_SIMM( ch, IMM_NERVE ) )
	  return;
	af.type      = gsn_nerve;
	af.duration  = 1;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = 0;
	affect_join( victim, &af );

	update_skpell( ch, gsn_nerve );
   }

   return;
}
void do_trip( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  if ( IS_NPC( ch ) )
	return;
  if ( !can_use_skpell( ch, gsn_trip ) )
    {
    send_to_char( AT_GREY, "Huh?\n\r", ch );
    return;
    }
  if ( !(victim=ch->fighting) )
    {
    send_to_char( AT_GREY, "You aren't fighting anyone.\n\r", ch );
    return;
    }

  if ( ch->pcdata->learned[gsn_trip] > number_percent( ) )
  {
	trip( ch, victim );
	update_skpell( ch, gsn_trip );
  }

  return;

}
void do_shriek( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  OBJ_DATA *obj, *obj_next;
  OBJ_DATA *potion, *potion_next;
  int dam;
  int blown = 0;
  int max_blown = 0;
  if ( !IS_NPC( ch )
  &&   !can_use_skpell( ch, gsn_shriek ) )
	{
	send_to_char( AT_GREY, "Huh?\n\r", ch );
	return;
	}
  if ( !(victim = ch->fighting) )
	{
	send_to_char(C_DEFAULT, "You aren't fighting anyone.\n\r", ch );
	return;
	}
  max_blown = ( IS_NPC( ch ) ) ? 10 : 25;
  if ( ( obj = victim->carrying ) && !saves_spell( ch->level, victim ) )
    {
    for ( obj = victim->carrying; obj; obj = obj_next )
	{
	obj_next = obj->next_content;
	if ( blown >= max_blown )
	  break;
	if ( obj->deleted )
	  continue;
	switch ( obj->item_type )
	  {
	  default: continue;
	  case ITEM_POTION:
	  case ITEM_CONTAINER:
    break;
	  }
	if ( obj->item_type == ITEM_CONTAINER )
	  {
	  if ( !obj->contains )
	    continue;
	  for ( potion = obj->contains; potion; potion = potion_next )
	    {
	    potion_next = potion->next_content;
	    if ( blown >= max_blown )
	        break;
	    if ( potion->deleted )
		continue;
	    if ( potion->item_type != ITEM_POTION )
		continue;
	    if ( number_bits( 2 ) != 0 )
		{
		extract_obj( potion );
		act(AT_BLUE, "You feel something explode from within $p.",
		    victim, obj, NULL, TO_CHAR );
		blown++;
		}
	    }
	  continue;
	  }
	if ( number_bits( 2 ) != 0 )
	  {
	  act(AT_BLUE, "$p vibrates and explodes!", victim, obj, NULL, TO_CHAR );
	  extract_obj( obj );
	  blown++;
	  }
	}
    }
  dam = number_range( 100, ch->level * 3 );
  dam += blown * 10;
  damage( ch, victim, dam, gsn_shriek );
  update_skpell( ch, gsn_shriek );
  WAIT_STATE( ch, skill_table[gsn_shriek].beats );
}
void death_xp_loss( CHAR_DATA *victim )
{
  int base_xp, xp_lastlvl, xp_loss, classes, mod;
  classes = number_classes( victim );
  if ( victim->level < LEVEL_HERO )
    {
    xp_lastlvl = classes == 1 ? 1000 * victim->level
	       : classes * 2000 * victim->level;
    xp_loss = ( xp_lastlvl - victim->exp ) / 2;
    if ( victim->exp > xp_lastlvl )
      gain_exp( victim, xp_loss );
    }
  else if ( victim->level < L_CHAMP3 )
    {
    mod = 1;
    base_xp = classes == 1 ? 100000 : 200000;
    xp_lastlvl = base_xp * classes;
    switch ( victim->level )
      {
      case LEVEL_HERO: mod = 1;
      case L_CHAMP1:   mod = 4;
      case L_CHAMP2:   mod = 10;
      }
    xp_lastlvl = xp_lastlvl * mod;
    if ( victim->exp > xp_lastlvl )
      {
      xp_loss = (xp_lastlvl - victim->exp ) / 2;
      xp_loss = UMAX( -10000 * classes, xp_loss );
      gain_exp( victim, xp_loss );
      }
    }
  return;
}
