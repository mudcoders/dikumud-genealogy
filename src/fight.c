/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined(macintosh)
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
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	check_killer	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	dam_message	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_counter	args( ( CHAR_DATA *ch) );
bool	can_bypass	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	number_attacks	args( ( CHAR_DATA *ch ) );
int	dambonus	args( ( CHAR_DATA *ch, CHAR_DATA *victim, 
			    int dam, int stance) );
void	update_damcap	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );



/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    CHAR_DATA *rch_next;
    CHAR_DATA *mount;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	ch_next	= ch->next;

	if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
	    continue;

	if ( IS_AWAKE(ch) && IS_AWAKE(victim) && 
		ch->in_room == victim->in_room )
	    multi_hit( ch, victim, TYPE_UNDEFINED );
	else
	    stop_fighting( ch, FALSE );

	if ( ( victim = ch->fighting ) == NULL )
	    continue;

	/*
	 * Fun for the whole family!
	 */
	for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
	{
	    rch_next = rch->next_in_room;

	    if ( IS_AWAKE(rch) && rch->fighting == NULL )
	    {
		/*
		 * Mount's auto-assist their riders and vice versa.
		 */
		if ( (mount = rch->mount) != NULL )
		{
		    if (mount == ch) multi_hit( rch, victim, TYPE_UNDEFINED );
		    continue;
		}
		/*
		 * PC's auto-assist others in their group.
		 */
		if ( !IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM) )
		{
		    if ( ( !IS_NPC(rch) || IS_AFFECTED(rch, AFF_CHARM) )
		    &&   is_same_group(ch, rch) )
			multi_hit( rch, victim, TYPE_UNDEFINED );
		    continue;
		}

		/*
		 * NPC's assist NPC's of same type or 12.5% chance regardless.
		 */
		if ( IS_NPC(rch) && !IS_AFFECTED(rch, AFF_CHARM) )
		{
		    if ( rch->pIndexData == ch->pIndexData
		    ||   number_bits( 3 ) == 0 )
		    {
			CHAR_DATA *vch;
			CHAR_DATA *target;
			int number;

			target = NULL;
			number = 0;
			for ( vch = ch->in_room->people; vch; vch = vch->next )
			{
			    if ( can_see( rch, vch )
			    &&   is_same_group( vch, victim )
			    &&   number_range( 0, number ) == 0 )
			    {
				target = vch;
				number++;
			    }
			}

			if ( target != NULL )
			    multi_hit( rch, target, TYPE_UNDEFINED );
		    }
		}
	    }
	}
    }

    return;
}



/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *wield;
    OBJ_DATA *wield1;
    OBJ_DATA *wield2;
    int	      sn;
    int	      level;
    int       chance;
    int       unarmed;
    int       wieldorig = 0;
    int       wieldtype = 0;
    int       maxcount;
    int       countup;

    if ( ch->position < POS_SLEEPING ) return;

    if ( dt == gsn_headbutt )
    {
	one_hit( ch, victim, dt, 1 );
	return;
    }

    if (!IS_NPC(ch) && IS_ITEMAFF(ch, ITEMA_PEACE))
    {
	send_to_char( "You are unable to attack them.\n\r", ch );
	return;
    }

    if (!IS_NPC(victim) && IS_ITEMAFF(victim, ITEMA_PEACE))
    {
	send_to_char( "You can't seem to attack them.\n\r", ch );
	return;
    }

    wield1 = get_eq_char( ch, WEAR_WIELD );
    wield2 = get_eq_char( ch, WEAR_HOLD );
    if ( wield1 != NULL && wield1->item_type == ITEM_WEAPON ) wieldorig  = 1;
    if ( wield2 != NULL && wield2->item_type == ITEM_WEAPON ) wieldorig += 2;
    wieldtype = wieldorig;

    if ( wieldorig == 3 ) { if (number_range(1,2) == 2) wieldtype = 2; else wieldtype = 1;}
    if      ( wieldtype == 2 ) wield = wield2;
    else                       wield = wield1;

    if (!IS_NPC(ch) && ch->stance[0] > 0 && number_percent() == 1)
    {
	int stance = ch->stance[0];
	if (ch->stance[stance] >= 200)
	{
	    special_move(ch,victim);
	    return;
	}
    }

    unarmed = 0;
/*
    if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
	ch->cmbt[unarmed] != 0 && wieldorig == 0 )
	fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
    else
*/
	one_hit( ch, victim, dt, wieldtype );

    if ( victim == NULL || victim->position != POS_FIGHTING ) return;

    /* Only want one spell per round from spell weapons...otherwise it's 
     * too powerful, and would unbalance player killing (as this is a PK mud).
     */

    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	{
	    dt += wield->value[3];
	    if (wield->value[0] >= 1)
	    {
		/* Look, just don't ask...   KaVir */
		if (wield->value[0] >= 1000)
		    sn = wield->value[0] - ((wield->value[0] / 1000) * 1000);
		else
		    sn = wield->value[0];
		if (sn != 0 && victim->position == POS_FIGHTING)
		    (*skill_table[sn].spell_fun) (sn,wield->level,ch,victim);
	    }
	}
    }

    if ( victim == NULL || victim->position != POS_FIGHTING ) return;

    if ( ch->fighting != victim || dt == gsn_backstab || dt == gsn_headbutt )
	return;

    if ( !IS_NPC(victim) && !IS_SET(victim->special, SPC_WOLFMAN) &&
	number_percent( ) <= victim->pcdata->learned[gsn_fastdraw] )
    {
    	OBJ_DATA *obj;
    	int wpntype  = 0;
    	int wpntype2 = 0;

	if ((obj = get_eq_char(victim,WEAR_SCABBARD_R)) != NULL)
	{
	    wpntype = obj->value[3];
	    if (wpntype < 0 || wpntype > 12) wpntype = 0;
	    if ((obj = get_eq_char(victim,WEAR_WIELD)) != NULL)
	    {
		obj_from_char(obj);
		obj_to_room(obj,victim->in_room);
		act("You hurl $p aside.",victim,obj,NULL,TO_CHAR);
		act("$n hurls $p aside.",victim,obj,NULL,TO_ROOM);
	    }
	    do_draw(victim,"right");
	}
	if ((obj = get_eq_char(victim,WEAR_SCABBARD_L)) != NULL)
	{
	    wpntype2 = obj->value[3];
	    if (wpntype2 < 0 || wpntype2 > 12) wpntype2 = 0;
	    if ((obj = get_eq_char(victim,WEAR_HOLD)) != NULL)
	    {
		obj_from_char(obj);
		obj_to_room(obj,victim->in_room);
		act("You hurl $p aside.",victim,obj,NULL,TO_CHAR);
		act("$n hurls $p aside.",victim,obj,NULL,TO_ROOM);
	    }
	    do_draw(victim,"left");
	}
	if (wpntype  > 0) one_hit( victim, ch, TYPE_UNDEFINED, 1 );
	if ( victim == NULL || victim->position != POS_FIGHTING ) return;
	if (wpntype2 > 0) one_hit( victim, ch, TYPE_UNDEFINED, 2 );
	if ( victim == NULL || victim->position != POS_FIGHTING ) return;
    }

    maxcount = number_attacks(ch);
    if (!IS_NPC(ch))
    {
	chance = 0;
	if (wield != NULL && wield->item_type == ITEM_WEAPON)
	{
	    int tempnum = wield->value[3];
	    chance = (ch->wpn[tempnum]) * 0.5;
	}
	else chance = (ch->wpn[0]) * 0.5;
	if (number_percent() <= chance) maxcount += 1;
    }
    if (wieldorig == 3) maxcount += 1;
    for (countup = 0; countup <= maxcount; countup++)
    {
/*
	if (!IS_NPC(ch) && wieldorig == 0)
	{
	    if      (countup == 0) unarmed = number_range(0,3);
	    else if (countup == 1) unarmed = number_range(4,7);
    	    else                   unarmed = number_range(0,7);
    	    if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
		ch->cmbt[unarmed] != 0 && wieldorig == 0 )
	    {
		fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
		continue;
	    }
	}
*/
	wieldtype = wieldorig;
	if ( wieldorig == 3 )
	{
	    if (number_range(1,2) == 2) wieldtype = 2;
	    else wieldtype = 1;
	}
	one_hit( ch, victim, -1, wieldtype );
	if ( victim == NULL || victim->position != POS_FIGHTING ) return;
	if ( ch->fighting != victim ) return;
    }

    if (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FANGS))
	one_hit( ch, victim, ( TYPE_HIT + 10 ), 0 );
    if ( victim == NULL || victim->position != POS_FIGHTING ) return;
    if (!IS_NPC(ch))
    {
	if (IS_CLASS(ch, CLASS_DEMON)  && IS_DEMAFF(ch,DEM_HORNS) 
		&& number_range(1,3) == 1)
	multi_hit( ch, victim, gsn_headbutt );
    }
    if ( victim == NULL || victim->position != POS_FIGHTING ) return;

    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) && IS_VAMPAFF(ch, VAM_SERPENTIS))
	spell_poison( gsn_poison, (ch->level*number_range(5,10)),ch,victim );
    else if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->powers[WPOWER_SPIDER] > 0)
	spell_poison( gsn_poison, (ch->level*number_range(5,10)),ch,victim );

    if (victim->itemaffect < 1) return;
    if (IS_NPC(victim) || victim->spl[1] < 4) level = victim->level;
    else level = (victim->spl[1] * 0.25);
    if (IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD) && ch->position == POS_FIGHTING)
	if ( (sn = skill_lookup( "lightning bolt" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    if (IS_ITEMAFF(victim, ITEMA_FIRESHIELD) && ch->position == POS_FIGHTING)
	if ( (sn = skill_lookup( "fireball" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    if (IS_ITEMAFF(victim, ITEMA_ICESHIELD) && ch->position == POS_FIGHTING)
	if ( (sn = skill_lookup( "chill touch" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    if (IS_ITEMAFF(victim, ITEMA_ACIDSHIELD) && ch->position == POS_FIGHTING)
	if ( (sn = skill_lookup( "acid blast" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    if (IS_ITEMAFF(victim, ITEMA_CHAOSSHIELD) && ch->position == POS_FIGHTING)
	if ( (sn = skill_lookup( "chaos blast" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    return;
}


int number_attacks( CHAR_DATA *ch )
{
    int count = 1;

    if ( IS_NPC(ch) )
    {
	if (ch->level >= 50) count += 1;
	if (ch->level >= 100) count += 1;
	return count;
    }

    if ( IS_STANCE(ch, STANCE_VIPER) && number_percent() > 
	ch->stance[STANCE_VIPER] * 0.5) count += 1;
    else if ( IS_STANCE(ch, STANCE_MANTIS) && number_percent() > 
	ch->stance[STANCE_MANTIS] * 0.5) count += 1;
    else if ( IS_STANCE(ch, STANCE_TIGER) && number_percent() > 
	ch->stance[STANCE_TIGER] * 0.5) count += 1;

    if (!IS_NPC(ch))
    {
    	if ( IS_CLASS(ch, CLASS_VAMPIRE) && IS_VAMPAFF(ch, VAM_CELERITY) )
		count += 1;
    	else if (IS_CLASS(ch, CLASS_WEREWOLF) && ch->pcdata->powers[WPOWER_LYNX] > 2 )
		count += 1;
   	else if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_DEMON) && IS_DEMPOWER( ch, DEM_SPEED) )
		count += 1;
    }

    if ( IS_ITEMAFF(ch, ITEMA_SPEED) ) count += 1;

    return count;
}


/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int handtype)
{
    OBJ_DATA *wield;
    OBJ_DATA *gloves;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;
    int level;
    int stance;
    bool right_hand;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    if (!IS_NPC(ch) && IS_ITEMAFF(ch, ITEMA_PEACE))
    {
	send_to_char( "You are unable to attack them.\n\r", ch );
	return;
    }

    if (!IS_NPC(victim) && IS_ITEMAFF(victim, ITEMA_PEACE))
    {
	send_to_char( "You can't seem to attack them.\n\r", ch );
	return;
    }

    /*
     * Figure out the type of damage message.
     */
    if (handtype == 2)
    {
    	wield = get_eq_char( ch, WEAR_HOLD );
	right_hand = FALSE;
    }
    else
    {
    	wield = get_eq_char( ch, WEAR_WIELD );
	right_hand = TRUE;
    }
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
    }

    if (ch->wpn[dt-1000] > 5) level = (ch->wpn[dt-1000] / 5);
    else level = 1;
    if (level > 40) level = 40;

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC(ch) )
    {
	thac0_00 = 20;
	thac0_32 =  0;
    }
    else
    {
	thac0_00 = SKILL_THAC0_00;
	thac0_32 = SKILL_THAC0_32;
    }
    thac0     = interpolate( level, thac0_00, thac0_32 ) - char_hitroll(ch);
    victim_ac = UMAX( -75, char_ac(victim) / 10 );
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt );
	tail_chain( );
	if (!is_safe(ch,victim))
	{
	    improve_wpn(ch,dt,right_hand);
    	    improve_stance(ch);
	}
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC(ch) )
    {
	dam = number_range( ch->level / 2, ch->level * 3 / 2 );
	if ( wield != NULL )
	    dam += dam * 0.5;
    }
    else
    {
	if ( IS_VAMPAFF(ch,VAM_CLAWS) && wield == NULL)
	    dam = number_range( 10, 20 );
	else if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dam = number_range( wield->value[1], wield->value[2] );
	else
	    dam = number_range( 1, 4 );
    }

    /*
     * Bonuses.
     */
    dam += char_damroll(ch);
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( dt == gsn_backstab )
	dam *= number_range(2,4);
    if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) &&
	IS_VAMPAFF(ch, VAM_POTENCE) )
	dam *= 1.5;
    else if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_DEMON) && 
		IS_DEMPOWER(ch, DEM_MIGHT) )
	dam *= 1.5;

    if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) )
    {
	if (IS_SET(victim->special, SPC_WOLFMAN)) dam *= 0.5;
	if (victim->pcdata->powers[WPOWER_BOAR] > 2 ) dam *= 0.5;
	if (wield != NULL && IS_SET(wield->spectype, SITEM_SILVER)) 
	    dam *= 2;
	else if ( ( gloves = get_eq_char( ch, WEAR_HANDS ) ) != NULL
	    && IS_SET(gloves->spectype, SITEM_SILVER)) dam *= 2;
    }

    /* Vampires should be tougher at night and weaker during the day. */
    if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	if (weather_info.sunlight == SUN_LIGHT && dam > 1)
	    dam /= 1.5;
	else if (weather_info.sunlight == SUN_DARK)
	    dam *= 1.5;
    }
    if ( !IS_NPC(ch) && dt >= TYPE_HIT)
	dam = dam + (dam * ((ch->wpn[dt-1000]+1) / 100));

    if ( !IS_NPC(ch) )
    {
	stance = ch->stance[0];
	if ( IS_STANCE(ch, STANCE_NORMAL) ) dam *= 1.25;
	else dam = dambonus(ch,victim,dam,stance);
    }

    if ( dam <= 0 )
	dam = 1;

    damage( ch, victim, dam, dt );
    tail_chain( );
    if (!is_safe(ch,victim))
    {
	improve_wpn(ch,dt,right_hand);
	improve_stance(ch);
    }
    return;
}

bool can_counter( CHAR_DATA *ch)
{
    if (!IS_NPC(ch) && IS_STANCE(ch, STANCE_MONKEY)) return TRUE;
    return FALSE;
}

bool can_bypass( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if (IS_NPC(ch) || IS_NPC(victim)) return FALSE;
    else if (IS_STANCE(ch, STANCE_VIPER)) return TRUE;
    else if (IS_STANCE(ch, STANCE_MANTIS)) return TRUE;
    else if (IS_STANCE(ch, STANCE_TIGER)) return TRUE;
    return FALSE;
}

void update_damcap( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int max_dam = 1000;

    if (!IS_NPC(ch))
    {
	if ((IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special, SPC_CHAMPION)) &&
	    ch->in_room != NULL)
	{
	    if (ch->in_room->vnum == 30000) max_dam = 10000; /* was5000Rotain*/
	    max_dam += (ch->pcdata->stats[DEMON_POWER] * 20);
	}
	else if (IS_CLASS(ch, CLASS_VAMPIRE))
	{
	    if      (ch->pcdata->stats[UNI_GEN] < 3) max_dam = 1500;
	    else if (ch->pcdata->stats[UNI_GEN] < 4) max_dam = 1400;
	    else if (ch->pcdata->stats[UNI_GEN] < 5) max_dam = 1300;
	    else                      max_dam = 1200;
	    max_dam += (ch->pcdata->stats[UNI_RAGE] * 10);
	    if (IS_VAMPAFF(ch, VAM_POTENCE)) max_dam += 100;
	    if (ch->pcdata->rank == AGE_METHUSELAH) max_dam += 300;
	    else if (ch->pcdata->rank == AGE_ELDER) max_dam += 200;
	    else if (ch->pcdata->rank == AGE_ANCILLA) max_dam += 100;
	}
	else if (IS_SET(ch->special, SPC_WOLFMAN))
	{
	    if      (ch->pcdata->stats[UNI_GEN] < 2) max_dam = 2000;
	    else if (ch->pcdata->stats[UNI_GEN] < 3) max_dam = 1900;
	    else if (ch->pcdata->stats[UNI_GEN] < 4) max_dam = 1800;
	    else                      max_dam = 1700;
	    max_dam += ch->pcdata->stats[UNI_RAGE];
	}
	else if (IS_CLASS(ch, CLASS_DEMON)) max_dam += 500;
	else if (IS_SET(ch->special, SPC_CHAMPION)) max_dam += 250;
	else if (IS_CLASS(ch, CLASS_HIGHLANDER) && IS_ITEMAFF(ch, ITEMA_HIGHLANDER))
	{
	    int wpn = ch->pcdata->powers[HPOWER_WPNSKILL];
	    if (wpn == 1 || wpn == 3)
		max_dam += ch->wpn[wpn];
	}
	if (IS_ITEMAFF(ch, ITEMA_ARTIFACT)) max_dam += 500;
	if (IS_ITEMAFF(victim, ITEMA_ARTIFACT)) max_dam -= 500;
	if (IS_NPC(victim) || victim->stance[0] != STANCE_MONKEY)
	{
	    if      (ch->stance[0] == STANCE_BULL)   max_dam += 250;
	    else if (ch->stance[0] == STANCE_DRAGON) max_dam += 250;
	    else if (ch->stance[0] == STANCE_TIGER)  max_dam += 250;
	}
    }
    else max_dam += (ch->level * 2);
    if (!IS_NPC(victim))
    {
	int silver_tol = (victim->pcdata->powers[WPOWER_SILVER] * 2.5);
	if (IS_CLASS(victim, CLASS_WEREWOLF) && 
	    IS_ITEMAFF(ch, ITEMA_RIGHT_SILVER))
	    max_dam += (250 - silver_tol);
	if (IS_CLASS(victim, CLASS_WEREWOLF) && 
	    IS_ITEMAFF(ch, ITEMA_LEFT_SILVER))
	    max_dam += (250 - silver_tol);
	if (IS_NPC(ch) || ch->stance[0] != STANCE_MONKEY)
	{
	    if      (victim->stance[0] == STANCE_CRAB)    max_dam -= 250;
	    else if (victim->stance[0] == STANCE_DRAGON)  max_dam -= 250;
	    else if (victim->stance[0] == STANCE_SWALLOW) max_dam -= 250;
	}
	if ((IS_CLASS(victim,CLASS_DEMON) || IS_SET(victim->special,SPC_CHAMPION)) 
	    && victim->in_room != NULL && victim->in_room->vnum == 30000)
	    max_dam *= 0.2;
    }
    if (max_dam < 1000) max_dam = 1000;
    ch->damcap[DAM_CAP] = max_dam;
    ch->damcap[DAM_CHANGE] = 0;
    return;
}

/*
 * Inflict damage from a hit.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    int max_dam = ch->damcap[DAM_CAP];

    if ( victim->position == POS_DEAD )
	return;

    /*
     * Stop up any residual loopholes.
     */
    if (ch->damcap[DAM_CHANGE] == 1) update_damcap(ch,victim);

    if ( dam > max_dam ) dam = max_dam;

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
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch );
	    victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if ( IS_NPC(ch)
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return;
	    }
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );

	/*
	 * Damage modifiers.
	 */
	if ( IS_AFFECTED(ch, AFF_HIDE) )
	{
	    if (!can_see(victim,ch)) {dam *= 1.5;
		send_to_char("You use your concealment to get a surprise attack!\n\r",ch);}
	    REMOVE_BIT( ch->affected_by, AFF_HIDE );
	    act( "$n leaps from $s concealment.", ch, NULL, NULL, TO_ROOM );
	}

	if ( IS_AFFECTED(victim, AFF_SANCTUARY) && dam > 1  )
	    dam *= 0.5;

	if ( IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch) && dam > 1  )
	    dam -= dam * 0.25;

	if ( dam < 0 )
	    dam = 0;

	/*
	 * Check for disarm, trip, parry, and dodge.
	 */
	if ( dt >= TYPE_HIT )
	{
	    if ( IS_NPC(ch) && number_percent( ) < ch->level * 0.5 )
		disarm( ch, victim );
	    if ( IS_NPC(ch) && number_percent( ) < ch->level * 0.5 )
		trip( ch, victim );
	    if ( check_parry( ch, victim, dt ) )
		return;
	    if ( !IS_NPC(victim) && IS_STANCE(victim, STANCE_CRANE) && 
		victim->stance[STANCE_CRANE] > 100 && !can_counter(ch)
		&& !can_bypass(ch,victim) && check_parry( ch, victim, dt ))
		return;
	    else if ( !IS_NPC(victim) && IS_STANCE(victim, STANCE_MANTIS) && 
		victim->stance[STANCE_MANTIS] > 100 && !can_counter(ch)
		&& !can_bypass(ch,victim) && check_parry( ch, victim, dt ))
		return;
	    if ( check_dodge( ch, victim, dt ) )
		return;
	    if ( !IS_NPC(victim) && IS_STANCE(victim, STANCE_MONGOOSE) && 
		victim->stance[STANCE_MONGOOSE] > 100 && !can_counter(ch)
		&& !can_bypass(ch,victim) && check_dodge( ch, victim, dt ))
		return;
	    else if ( !IS_NPC(victim) && IS_STANCE(victim, STANCE_SWALLOW) && 
		victim->stance[STANCE_SWALLOW] > 100 && !can_counter(ch)
		&& !can_bypass(ch,victim) && check_dodge( ch, victim, dt ))
		return;
	}

	dam_message( ch, victim, dam, dt );
    }
    hurt_person(ch,victim,dam);
    return;
}

void adv_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
    if ( victim->position == POS_DEAD )
	return;

    if ( dam > 1000 ) dam = 1000;

    if ( victim != ch )
    {
	if ( is_safe( ch, victim ) )
	    return;
	check_killer( ch, victim );

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch );
	    victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if ( IS_NPC(ch)
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return;
	    }
	}

	if ( victim->master == ch )
	    stop_follower( victim );

	if ( IS_AFFECTED(victim, AFF_SANCTUARY) && dam > 1 )
	    dam /= 2;

	if ( IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch) && dam > 1  )
	    dam -= dam / 4;

	if ( dam < 0 )
	    dam = 0;
    }
    return;
}

void hurt_person( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
    bool is_npc = FALSE;
    int vnum;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
	victim->hit = 1;
    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	act( "$n is mortally wounded, and spraying blood everywhere.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "You are mortally wounded, and spraying blood everywhere.\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "$n is incapacitated, and bleeding badly.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "You are incapacitated, and bleeding badly.\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "$n is stunned, but will soon recover.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("You are stunned, but will soon recover.\n\r",
	    victim );
	break;

    case POS_DEAD:
	act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
	send_to_char( "You have been KILLED!!\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "That really did HURT!\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 && dam > 0 )
	{
	    if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE)
		&& number_percent() < victim->beast)
		vamp_rage(victim);
	    else
		send_to_char( "You sure are BLEEDING!\n\r", victim );
	}
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if ( !IS_NPC(victim) )
	{
	    sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->in_room->vnum );
	    log_string( log_buf );

	    /*
	     * Dying penalty:
	     * 1/2 your current exp.
	     */
	    if ( victim->exp > 0 )
		victim->exp = victim->exp / 2;
	}

    	if (IS_NPC(victim) && !IS_NPC(ch))
	{
	    ch->mkill += 1;
	    if (IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special, SPC_CHAMPION))
	    {
		if ((vnum = victim->pIndexData->vnum) > 29000)
		{
		    switch (vnum)
		    {
		    default:
			ch->pcdata->stats[DEMON_CURRENT] += victim->level;
			ch->pcdata->stats[DEMON_TOTAL]   += victim->level;
			break;
		    case 29600:
		    case 30001:
		    case 30006:
		    case 30007:
		    case 30008:
		    case 30009:
                    case 30000:
			break;
		    }
		}
		else if (IS_NPC(victim) && !IS_SET(victim->act, ACT_NOEXP))
		{
		    ch->pcdata->stats[DEMON_CURRENT] += victim->level;
		    ch->pcdata->stats[DEMON_TOTAL] += victim->level;
		}
	    }
	    if (ch->level == 1 && ch->mkill > 4)
	    {
		ch->level = 2;
		do_save(ch,"");
	    }
	}
    	if (!IS_NPC(victim) && IS_NPC(ch)) 
        {
           victim->mdeath = victim->mdeath + 1;
        }

	raw_kill( victim );

	if ( IS_SET(ch->act, PLR_AUTOLOOT) )
	    do_get( ch, "all corpse" );
	else
	    do_look( ch, "in corpse" );

	if ( !IS_NPC(ch) && is_npc )
	{
	    if ( IS_SET(ch->act, PLR_AUTOSAC) )
		do_sacrifice( ch, "corpse" );
	}

	return;
    }

    if ( victim == ch )
	return;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC(victim) && victim->desc == NULL )
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
    if ( IS_NPC(victim) && dam > 0 )
    {
	if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 1 ) == 0
	&&   victim->hit < victim->max_hit / 2 )
	||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
	&&     victim->master->in_room != victim->in_room ) )
	    do_flee( victim, "" );
    }

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait == 0 )
	do_flee( victim, "" );

    tail_chain( );
    return;
}



bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
    /* Ethereal people can only attack other ethereal people */
    if ( IS_AFFECTED(ch, AFF_ETHEREAL) && !IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot while ethereal.\n\r", ch );
	return TRUE;
    }
    if ( !IS_AFFECTED(ch, AFF_ETHEREAL) && IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot fight an ethereal person.\n\r", ch );
	return TRUE;
    }

    /* You cannot attack across planes */
    if ( IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE) )
    {
	act( "You are too insubstantial!", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }
    if ( !IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE) )
    {
	act( "$E is too insubstantial!", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }

    if ( IS_SET(ch->in_room->room_flags,ROOM_SAFE) )
    {
	send_to_char( "You cannot fight in a safe room.\n\r", ch );
	return TRUE;
    }

    if ( IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH) )
    {
	send_to_char( "Objects cannot fight!\n\r", ch );
	return TRUE;
    }
    else if ( IS_HEAD(victim,LOST_HEAD) || IS_EXTRA(victim,EXTRA_OSWITCH))
    {
	send_to_char( "You cannot attack an object.\n\r", ch );
	return TRUE;
    }

    if ( IS_NPC(ch) || IS_NPC(victim) )
	return FALSE;

    /* Thx Josh! */
    if ( victim->fighting == ch )
	return FALSE;

    if (IS_ITEMAFF(ch, ITEMA_PEACE))
    {
	send_to_char( "You are unable to attack them.\n\r", ch );
	return TRUE;
    }

    if (IS_ITEMAFF(victim, ITEMA_PEACE))
    {
	send_to_char( "You can't seem to attack them.\n\r", ch );
	return TRUE;
    }

    if ( ch->trust > LEVEL_BUILDER )
    {
	send_to_char( "You cannot fight if you have implementor powers!\n\r", ch );
	return TRUE;
    }

    if ( victim->trust > LEVEL_BUILDER )
    {
	send_to_char( "You cannot fight someone with implementor powers!\n\r", ch );
	return TRUE;
    }

    if ( !CAN_PK(ch) || !CAN_PK(victim) )
    {
	send_to_char( "Both players must be avatars to fight.\n\r", ch );
	return TRUE;
    }

    if ( ch->desc == NULL || victim->desc == NULL )
    {
	if (victim->position != POS_FIGHTING)
	{
	    send_to_char( "They are currently link dead.\n\r", ch );
	    return TRUE;
	}
    }
    return FALSE;
}

bool no_attack( CHAR_DATA *ch, CHAR_DATA *victim )
{
    /* Ethereal people can only attack other ethereal people */
    if ( IS_AFFECTED(ch, AFF_ETHEREAL) && !IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot while ethereal.\n\r", ch );
	return TRUE;
    }
    if ( !IS_AFFECTED(ch, AFF_ETHEREAL) && IS_AFFECTED(victim, AFF_ETHEREAL) )
    {
	send_to_char( "You cannot fight an ethereal person.\n\r", ch );
	return TRUE;
    }

    /* You cannot attack across planes */
    if ( IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE) )
    {
	act( "You are too insubstantial!", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }
    if ( !IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE) )
    {
	act( "$E is too insubstantial!", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }

    if ( IS_SET(ch->in_room->room_flags,ROOM_SAFE) )
    {
	send_to_char( "You cannot fight in a safe room.\n\r", ch );
	return TRUE;
    }

    if ( IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH) )
    {
	send_to_char( "Objects cannot fight!\n\r", ch );
	return TRUE;
    }
    else if ( IS_HEAD(victim,LOST_HEAD) || IS_EXTRA(victim,EXTRA_OSWITCH) )
    {
	send_to_char( "You cannot attack objects.\n\r", ch );
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
     * Follow charm thread to responsible character.
     * Attacking someone's charmed char is hostile!
     */
    while ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL )
	victim = victim->master;

    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if ( IS_NPC(victim) )
	return;

    /*
     * Charm-o-rama.
     */
    if ( IS_SET(ch->affected_by, AFF_CHARM) )
    {
	if ( ch->master == NULL )
	{
	    affect_strip( ch, gsn_charm_person );
	    REMOVE_BIT( ch->affected_by, AFF_CHARM );
	    return;
	}
	stop_follower( ch );
	return;
    }
    return;
}



/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *obj = NULL;
    int chance = 0;
    bool claws = FALSE;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC(victim) )
	obj = NULL;
    else if ( IS_CLASS(victim, CLASS_WEREWOLF)	&& 
	victim->pcdata->powers[WPOWER_BEAR] > 2	&& 
	IS_VAMPAFF(victim, VAM_CLAWS)		&&
	get_eq_char(victim, WEAR_WIELD) == NULL	&&
	get_eq_char(victim, WEAR_HOLD) == NULL)
    {
	obj = NULL;
	claws = TRUE;
    }
    else
    {
	if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL 
		|| obj->item_type != ITEM_WEAPON )
	{
	    if ( ( obj = get_eq_char( victim, WEAR_HOLD ) ) == NULL 
		|| obj->item_type != ITEM_WEAPON ) return FALSE;
	}
    }

    if (dt < 1000 || dt > 1012) return FALSE;
    if (!IS_NPC(ch))     chance -= (ch->wpn[dt-1000] * 0.1);
	else             chance -= (ch->level * 0.2);
    if (!IS_NPC(victim)) chance += (victim->wpn[dt-1000] * 0.5);
	else             chance += victim->level;
    if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_CRANE) &&
	victim->stance[STANCE_CRANE] > 0 && !can_counter(ch) &&
	!can_bypass(ch,victim))
	chance += (victim->stance[STANCE_CRANE] * 0.25);
    else if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_MANTIS) &&
	victim->stance[STANCE_MANTIS] > 0 && !can_counter(ch) &&
	!can_bypass(ch,victim))
	chance += (victim->stance[STANCE_MANTIS] * 0.25);
    chance -= (char_hitroll(ch) * 0.1);

    if (claws) 
    {
	if ( victim->pcdata->powers[WPOWER_LYNX] > 3 )
	    chance += (char_hitroll(victim) * 0.1);
	else
	    chance += (char_hitroll(victim) * 0.075);
    }
    else chance += (char_hitroll(victim) * 0.1);

    if (!IS_NPC(ch))
    {
	if (IS_VAMPAFF(ch, VAM_CELERITY) && 
	    IS_CLASS(ch, CLASS_VAMPIRE)) 
	    chance -= 20;
	else if (IS_CLASS(ch, CLASS_DEMON) && IS_DEMPOWER(ch, DEM_SPEED) )
	    chance -= 10;
	else if (IS_CLASS(ch, CLASS_WEREWOLF) &&
	    ch->pcdata->powers[WPOWER_MANTIS] < 5 )
	    chance -= (ch->pcdata->powers[WPOWER_MANTIS] * 5);
    }
    if (!IS_NPC(victim))
    {
	if (IS_VAMPAFF(victim, VAM_CELERITY) && 
	    IS_CLASS(victim, CLASS_VAMPIRE)) 
	    chance += 20;
	else if (IS_CLASS(victim, CLASS_DEMON) && IS_DEMPOWER( victim, DEM_SPEED) )
	    chance += 10;
    }

    if      (chance > 75) chance = 75;
    else if (chance < 25) chance = 25;

    if (!IS_NPC(ch) && IS_VAMPAFF(ch, VAM_CELERITY) && 
	IS_CLASS(ch, CLASS_VAMPIRE))
    {
	if (ch->pcdata->rank == AGE_METHUSELAH) chance -= 15;
	else if (ch->pcdata->rank == AGE_ELDER) chance -= 10;
	else if (ch->pcdata->rank == AGE_ANCILLA) chance -= 5;
    }
    if (!IS_NPC(victim) && IS_VAMPAFF(victim, VAM_CELERITY) && 
	IS_CLASS(victim, CLASS_VAMPIRE))
    {
	if (victim->pcdata->rank == AGE_METHUSELAH) chance += 15;
	else if (victim->pcdata->rank == AGE_ELDER) chance += 10;
	else if (victim->pcdata->rank == AGE_ANCILLA) chance += 5;
    }
    if ( number_percent( ) < 100 && number_percent( ) >= chance )
	return FALSE;

    if (claws)
    {
	if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF))
	    act( "You parry $n's blow with your claws.",  ch, NULL, victim, TO_VICT );
	if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF))
	    act( "$N parries your blow with $S claws.", ch, NULL, victim, TO_CHAR );
	return TRUE;
    }
    if (!IS_NPC(victim) && obj != NULL && obj->item_type == ITEM_WEAPON &&
	obj->value[3] >= 0 && obj->value[3] <= 12)
    {
	if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF))
	    act( "You parry $n's blow with $p.",  ch, obj, victim, TO_VICT );
	if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF))
	    act( "$N parries your blow with $p.", ch, obj, victim, TO_CHAR );
	return TRUE;
    }
    if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF))
	act( "You parry $n's attack.",  ch, NULL, victim, TO_VICT    );
    if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF))
	act( "$N parries your attack.", ch, NULL, victim, TO_CHAR    );
    return TRUE;
}



/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int chance = 0;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if (!IS_NPC(ch))     chance -= (ch->wpn[dt-1000] * 0.1);
	else             chance -= (ch->level * 0.2);
    if (!IS_NPC(victim)) chance += (victim->wpn[0] * 0.5);
	else             chance += victim->level;

    if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_MONGOOSE) &&
	victim->stance[STANCE_MONGOOSE] > 0 && !can_counter(ch) &&
	!can_bypass(ch,victim))
	(chance += victim->stance[STANCE_MONGOOSE] * 0.25);
    if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_SWALLOW) &&
	victim->stance[STANCE_SWALLOW] > 0 && !can_counter(ch) &&
	!can_bypass(ch,victim))
	(chance += victim->stance[STANCE_SWALLOW] * 0.25);

    if (!IS_NPC(ch))
    {
	if (IS_VAMPAFF(ch, VAM_CELERITY) && 
	    IS_CLASS(ch, CLASS_VAMPIRE)) 
	    chance -= 20;
	else if (IS_CLASS(ch, CLASS_DEMON) && IS_DEMPOWER( ch, DEM_SPEED) )
	    chance -= 10;
	else if (IS_CLASS(ch, CLASS_WEREWOLF) &&
	    ch->pcdata->powers[WPOWER_MANTIS] < 5 )
	    chance -= (ch->pcdata->powers[WPOWER_MANTIS] * 10);
    }
    if (!IS_NPC(victim))
    {
	if (IS_VAMPAFF(victim, VAM_CELERITY) && 
	    IS_CLASS(victim, CLASS_VAMPIRE)) 
	    chance += 20;
	else if (IS_CLASS(victim, CLASS_DEMON) && IS_DEMPOWER( victim, DEM_SPEED) )
	    chance += 10;
    }

    if      (chance > 75) chance = 75;
    else if (chance < 25) chance = 25;

    if (!IS_NPC(ch) && IS_VAMPAFF(ch, VAM_CELERITY) && 
	IS_CLASS(ch, CLASS_VAMPIRE))
    {
	if (ch->pcdata->rank == AGE_METHUSELAH) chance -= 15;
	else if (ch->pcdata->rank == AGE_ELDER) chance -= 10;
	else if (ch->pcdata->rank == AGE_ANCILLA) chance -= 5;
    }
    if (!IS_NPC(victim) && IS_VAMPAFF(victim, VAM_CELERITY) && 
	IS_CLASS(victim, CLASS_VAMPIRE))
    {
	if (victim->pcdata->rank == AGE_METHUSELAH) chance += 15;
	else if (victim->pcdata->rank == AGE_ELDER) chance += 10;
	else if (victim->pcdata->rank == AGE_ANCILLA) chance += 5;
    }

    if ( number_percent( ) < 100 && number_percent( ) >= chance )
	return FALSE;

    if (IS_NPC(victim) || !IS_SET(victim->act, PLR_BRIEF))
	act( "You dodge $n's attack.", ch, NULL, victim, TO_VICT );
    if (IS_NPC(ch) || !IS_SET(ch->act, PLR_BRIEF))
	act( "$N dodges your attack.", ch, NULL, victim, TO_CHAR );
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    CHAR_DATA *mount;

    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	{
	    bool gm_stance = FALSE;
	    victim->position = POS_STANDING;
	    if (!IS_NPC(victim) && victim->stance[0] > 0)
	    {
		int stance = victim->stance[0];
		if (victim->stance[stance] >= 200) gm_stance = TRUE;
	    }
	    if (IS_NPC(victim) || victim->max_hit * 0.25 > victim->hit || 
		!gm_stance)
	    {
		act( "$n clambers back to $s feet.", victim, NULL, NULL, TO_ROOM);
		act( "You clamber back to your feet.", victim, NULL, NULL, TO_CHAR);
	    }
	    else
	    {
		act( "$n flips back up to $s feet.", victim, NULL, NULL, TO_ROOM);
		act( "You flip back up to your feet.", victim, NULL, NULL, TO_CHAR);
	    }
	}
	return;
    }
    else if ( (mount = victim->mount) != NULL)
    {
	if (victim->mounted == IS_MOUNT)
	{
	    act("$n rolls off $N.",mount,NULL,victim,TO_ROOM);
	    act("You roll off $N.",mount,NULL,victim,TO_CHAR);
	}
	else if (victim->mounted == IS_RIDING)
	{
	    act("$n falls off $N.",victim,NULL,mount,TO_ROOM);
	    act("You fall off $N.",victim,NULL,mount,TO_CHAR);
	}
	mount->mount    = NULL;
	victim->mount   = NULL;
	mount->mounted  = IS_ON_FOOT;
	victim->mounted = IS_ON_FOOT;
    }

    if (!IS_NPC(victim) && victim->hit <=-11 && IS_HERO(victim))
    {
        victim->hit = -10;
        if (victim->position == POS_FIGHTING) 
           stop_fighting(victim,TRUE);
        return;
    }

    if ( IS_NPC(victim) || victim->hit <= -11 )
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
    if ( ch->fighting != NULL )
    {
	bug( "Set_fighting: already fighting", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
	affect_strip( ch, gsn_sleep );

    ch->fighting = victim;
    ch->position = POS_FIGHTING;
    ch->damcap[DAM_CHANGE] = 1;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch == ch || ( fBoth && fch->fighting == ch ) )
	{
	    fch->fighting	= NULL;
	    fch->position	= POS_STANDING;
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
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;

    if ( IS_NPC(ch) )
    {
	name		= ch->short_descr;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
	corpse->timer	= number_range( 4, 8 );
	corpse->value[2]= ch->pIndexData->vnum;
	if ( ch->gold > 0 )
	{
	    obj_to_obj( create_money( ch->gold ), corpse );
	    ch->gold = 0;
	}
    }
    else
    {
	name		= ch->name;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
	corpse->timer	= number_range( 25, 40 );
	/* Why should players keep their gold? */
	if ( ch->gold > 0 )
	{
	    obj = create_money( ch->gold );
	    if (IS_AFFECTED(ch,AFF_SHADOWPLANE) &&
	        (!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) ) )
	        SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
	    obj_to_obj( obj, corpse );
	    ch->gold = 0;
	}
    }

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;

	obj_from_char( obj );
	if ( IS_SET( obj->extra_flags, ITEM_VANISH ) )
	    extract_obj( obj );
	else
	{
	    if (IS_AFFECTED(ch,AFF_SHADOWPLANE) &&
	        (!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) ) )
	        SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);
	    obj_to_obj( obj, corpse );
	}
    }

    /* If you die in the shadowplane, your corpse should stay there */
    if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
	SET_BIT(corpse->extra_flags, ITEM_SHADOWPLANE);

    obj_to_room( corpse, ch->in_room );
    return;
}



void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;

    if ( IS_NPC(ch) )
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;
    return;
}

void make_part( CHAR_DATA *ch, char *argument )
{
    char  arg [MAX_INPUT_LENGTH];
    int vnum;

    argument = one_argument(argument,arg);
    vnum = 0;

    if (arg[0] == '\0') return;
    if (!str_cmp(arg,"head")) vnum = OBJ_VNUM_SEVERED_HEAD;
    else if (!str_cmp(arg,"arm")) vnum = OBJ_VNUM_SLICED_ARM;
    else if (!str_cmp(arg,"leg")) vnum = OBJ_VNUM_SLICED_LEG;
    else if (!str_cmp(arg,"heart")) vnum = OBJ_VNUM_TORN_HEART;
    else if (!str_cmp(arg,"turd")) vnum = OBJ_VNUM_TORN_HEART;
    else if (!str_cmp(arg,"entrails")) vnum = OBJ_VNUM_SPILLED_ENTRAILS;
    else if (!str_cmp(arg,"brain")) vnum = OBJ_VNUM_QUIVERING_BRAIN;
    else if (!str_cmp(arg,"eyeball")) vnum = OBJ_VNUM_SQUIDGY_EYEBALL;
    else if (!str_cmp(arg,"blood")) vnum = OBJ_VNUM_SPILT_BLOOD;
    else if (!str_cmp(arg,"face")) vnum = OBJ_VNUM_RIPPED_FACE;
    else if (!str_cmp(arg,"windpipe")) vnum = OBJ_VNUM_TORN_WINDPIPE;
    else if (!str_cmp(arg,"cracked_head")) vnum = OBJ_VNUM_CRACKED_HEAD;
    else if (!str_cmp(arg,"ear")) vnum = OBJ_VNUM_SLICED_EAR;
    else if (!str_cmp(arg,"nose")) vnum = OBJ_VNUM_SLICED_NOSE;
    else if (!str_cmp(arg,"tooth")) vnum = OBJ_VNUM_KNOCKED_TOOTH;
    else if (!str_cmp(arg,"tongue")) vnum = OBJ_VNUM_TORN_TONGUE;
    else if (!str_cmp(arg,"hand")) vnum = OBJ_VNUM_SEVERED_HAND;
    else if (!str_cmp(arg,"foot")) vnum = OBJ_VNUM_SEVERED_FOOT;
    else if (!str_cmp(arg,"thumb")) vnum = OBJ_VNUM_SEVERED_THUMB;
    else if (!str_cmp(arg,"index")) vnum = OBJ_VNUM_SEVERED_INDEX;
    else if (!str_cmp(arg,"middle")) vnum = OBJ_VNUM_SEVERED_MIDDLE;
    else if (!str_cmp(arg,"ring")) vnum = OBJ_VNUM_SEVERED_RING;
    else if (!str_cmp(arg,"little")) vnum = OBJ_VNUM_SEVERED_LITTLE;
    else if (!str_cmp(arg,"toe")) vnum = OBJ_VNUM_SEVERED_TOE;

    if ( vnum != 0 )
    {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	name		= IS_NPC(ch) ? ch->short_descr : ch->name;
	obj		= create_object( get_obj_index( vnum ), 0 );
	if (IS_NPC(ch))   obj->timer = number_range(2,5);
	else              obj->timer = -1;
	if (!str_cmp(arg,"head") && IS_NPC(ch))
	    obj->value[1] = ch->pIndexData->vnum;
	else if (!str_cmp(arg,"head") && !IS_NPC(ch))
	    {ch->pcdata->chobj = obj; obj->chobj = ch;obj->timer = number_range(2,3);}
	else if (!str_cmp(arg,"brain") && !IS_NPC(ch) && 
	IS_AFFECTED(ch,AFF_POLYMORPH) && IS_HEAD(ch, LOST_HEAD))
	{
	    if (ch->pcdata->chobj != NULL) ch->pcdata->chobj->chobj = NULL;
	    ch->pcdata->chobj = obj;
	    obj->chobj = ch;
	    obj->timer = number_range(2,3);
	}

	/* For blood :) KaVir */
	if (vnum == OBJ_VNUM_SPILT_BLOOD) obj->timer = 2;

	/* For voodoo dolls - KaVir */
	if (!IS_NPC(ch))
	{
	    sprintf( buf, obj->name, name );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	}
	else
	{
	    sprintf( buf, obj->name, "mob" );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	}

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

    	if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
	    SET_BIT(obj->extra_flags, ITEM_SHADOWPLANE);

	obj_to_room( obj, ch->in_room );
    }
    return;
}



void raw_kill( CHAR_DATA *victim )
{
    CHAR_DATA *mount;
    stop_fighting( victim, TRUE );
    death_cry( victim );
    make_corpse( victim );

    if ( (mount = victim->mount) != NULL)
    {
	if (victim->mounted == IS_MOUNT)
	{
	    act("$n rolls off the corpse of $N.",mount,NULL,victim,TO_ROOM);
	    act("You roll off the corpse of $N.",mount,NULL,victim,TO_CHAR);
	}
	else if (victim->mounted == IS_RIDING)
	{
	    act("$n falls off $N.",victim,NULL,mount,TO_ROOM);
	    act("You fall off $N.",victim,NULL,mount,TO_CHAR);
	}
	mount->mount    = NULL;
	victim->mount   = NULL;
	mount->mounted  = IS_ON_FOOT;
	victim->mounted = IS_ON_FOOT;
    }

    if ( IS_NPC(victim) )
    {
	victim->pIndexData->killed++;
	kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
	extract_char( victim, TRUE );
	return;
    }

    extract_char( victim, FALSE );
    while ( victim->affected )
	affect_remove( victim, victim->affected );
    if (IS_AFFECTED(victim,AFF_POLYMORPH) && IS_AFFECTED(victim,AFF_ETHEREAL))
    {
    	victim->affected_by	= AFF_POLYMORPH + AFF_ETHEREAL;
    }
    else if (IS_AFFECTED(victim,AFF_POLYMORPH))
    	victim->affected_by	= AFF_POLYMORPH;
    else if (IS_AFFECTED(victim,AFF_ETHEREAL))
    	victim->affected_by	= AFF_ETHEREAL;
    else
    	victim->affected_by	= 0;

    REMOVE_BIT(victim->immune, IMM_STAKE);
    REMOVE_BIT(victim->extra, TIED_UP);
    REMOVE_BIT(victim->extra, GAGGED);
    REMOVE_BIT(victim->extra, BLINDFOLDED);
    REMOVE_BIT(victim->extra, EXTRA_PREGNANT);
    REMOVE_BIT(victim->extra, EXTRA_LABOUR);
    victim->pcdata->stats[DEMON_POWER] = 0;

    victim->itemaffect	 = 0;
    victim->loc_hp[0]    = 0;
    victim->loc_hp[1]    = 0;
    victim->loc_hp[2]    = 0;
    victim->loc_hp[3]    = 0;
    victim->loc_hp[4]    = 0;
    victim->loc_hp[5]    = 0;
    victim->loc_hp[6]    = 0;
    victim->armor	 = 100;
    victim->position	 = POS_RESTING;
    victim->hit		 = UMAX( 1, victim->hit  );
    victim->mana	 = UMAX( 1, victim->mana );
    victim->move	 = UMAX( 1, victim->move );
    victim->hitroll	 = 0;
    victim->damroll	 = 0;
    victim->saving_throw = 0;
    victim->carry_weight = 0;
    victim->carry_number = 0;
    do_call(victim,"all");
    save_char_obj( victim );
    return;
}


void behead( CHAR_DATA *victim )
{
    char buf [MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;

    if (IS_NPC(victim)) return;

    location = victim->in_room;

    stop_fighting( victim, TRUE );

    make_corpse( victim );

    extract_char( victim, FALSE );

    char_from_room(victim);
    char_to_room(victim,location);

    if (victim == NULL)
    {bug( "Behead: Victim no longer exists.", 0 );return;}

    make_part( victim, "head" );

    while ( victim->affected )
	affect_remove( victim, victim->affected );
    if (IS_AFFECTED(victim,AFF_POLYMORPH) && IS_AFFECTED(victim,AFF_ETHEREAL))
    {
    	victim->affected_by	= AFF_POLYMORPH + AFF_ETHEREAL;
    }
    else if (IS_AFFECTED(victim,AFF_POLYMORPH))
    	victim->affected_by	= AFF_POLYMORPH;
    else if (IS_AFFECTED(victim,AFF_ETHEREAL))
    	victim->affected_by	= AFF_ETHEREAL;
    else
    	victim->affected_by	= 0;

    REMOVE_BIT(victim->immune, IMM_STAKE);
    REMOVE_BIT(victim->extra, TIED_UP);
    REMOVE_BIT(victim->extra, GAGGED);
    REMOVE_BIT(victim->extra, BLINDFOLDED);
    REMOVE_BIT(victim->extra, EXTRA_PREGNANT);
    REMOVE_BIT(victim->extra, EXTRA_LABOUR);
    victim->pcdata->stats[DEMON_POWER] = 0;

    victim->itemaffect	 = 0;
    victim->loc_hp[0]    = 0;
    victim->loc_hp[1]    = 0;
    victim->loc_hp[2]    = 0;
    victim->loc_hp[3]    = 0;
    victim->loc_hp[4]    = 0;
    victim->loc_hp[5]    = 0;
    victim->loc_hp[6]    = 0;
    victim->affected_by	 = 0;
    victim->armor	 = 100;
    victim->position	 = POS_STANDING;
    victim->hit		 = 1;
    victim->mana	 = UMAX( 1, victim->mana );
    victim->move	 = UMAX( 1, victim->move );
    victim->hitroll	 = 0;
    victim->damroll	 = 0;
    victim->saving_throw = 0;
    victim->carry_weight = 0;
    victim->carry_number = 0;

    SET_BIT(victim->loc_hp[0],LOST_HEAD);
    SET_BIT(victim->affected_by,AFF_POLYMORPH);
    sprintf(buf,"the severed head of %s",victim->name);
    free_string(victim->morph);
    victim->morph = str_dup(buf);
    do_call(victim,"all");
    save_char_obj( victim );
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    CHAR_DATA *mount;
    int xp;
    int members;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( (IS_NPC(ch) && (mount = ch->mount) == NULL) || victim == ch )
	return;
    
    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if ( !is_same_group( gch, ch ) )
	    continue;

	xp = xp_compute( gch, victim ) / members;
	sprintf( buf, "You receive %d experience points.\n\r", xp );
	send_to_char( buf, gch );
	if ((mount = gch->mount) != NULL) send_to_char( buf, mount );
	gain_exp( gch, xp );

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

	    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
	    {
		act( "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
		act( "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
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
    int align;
    int xp;
    int extra;
    int level;
    int number;
    int vnum;
    int victim_level; /* put to stop mass exp give outs Rotain */

    xp    = 300 - URANGE( -3, 3 - victim->level, 6 ) * 50;
    align = gch->alignment - victim->alignment;

    if (victim->level > 1000 )
	victim_level = 1000;
    else
	victim_level = victim->level;

    if (IS_HERO(gch))
    {
	/* Avatars shouldn't be able to change their alignment */
	gch->alignment  = gch->alignment;
    }
    else if ( align >  500 )
    {
	gch->alignment  = UMIN( gch->alignment + (align-500)/4,  1000 );
	xp = 5 * xp / 4;
    }
    else if ( align < -500 )
    {
	gch->alignment  = UMAX( gch->alignment + (align+500)/4, -1000 );
    }
    else
    {
	gch->alignment -= gch->alignment / 4;
	xp = 3 * xp / 4;
    }

     /* 
      * Put in mob vnum that you don't want players to gain exp for
      * Rotain
      */
    if (IS_NPC(victim) && (vnum = victim->pIndexData->vnum) > 29000)
    {
	switch (vnum)
	{
	default:
	    break;
	case 29600:
	case 30001:
	case 30006:
	case 30007:
	case 30008:
	case 30009:
        case 30000:
	    return 0;
	}
    }

    if (IS_NPC(victim) && (IS_SET(victim->act, ACT_NOEXP) || IS_IMMORTAL(gch))) 
	return 0;

    /*
     * Adjust for popularity of target:
     *   -1/8 for each target over  'par' (down to -100%)
     *   +1/8 for each target under 'par' (  up to + 25%)
     */
    level  = URANGE( 0, victim_level, MAX_LEVEL - 1 );
    number = UMAX( 1, kill_table[level].number );
    extra  = victim->pIndexData->killed - kill_table[level].killed / number;
/*  xp    -= xp * URANGE( -2, extra, 8 ) / 8;
    xp    -= xp * URANGE( -2, extra, 6 ) / 8; */

    xp    -= xp * number_range( -2, 2 ) / 8;

    xp     = number_range( xp * 3 / 4, xp * 5 / 4 );
    xp     = UMAX( 0, xp );
    xp     = (xp * (victim_level+1) * 0.25);
    xp     = xp / 2; /* Put in cause players compaling to much exp :P */

    if (!IS_NPC(gch))
    {
	gch->pcdata->score[SCORE_TOTAL_LEVEL] += victim->level;
	if (victim->level > gch->pcdata->score[SCORE_HIGH_LEVEL])
	    gch->pcdata->score[SCORE_HIGH_LEVEL] += victim->level;
	if (victim->level > 950) return 0;
	gch->pcdata->score[SCORE_TOTAL_XP] += xp;
	if (xp > gch->pcdata->score[SCORE_HIGH_XP])
	    gch->pcdata->score[SCORE_HIGH_XP] += xp;
    }

    return xp;
}



void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    static char * const attack_table[] =
    {
	"hit",
	"slice",  "stab",  "slash", "whip", "claw",
	"blast",  "pound", "crush", "grep", "bite",
	"pierce", "suck"
    };
    static char * const attack_table2[] =
    {
	"hits",
	"slices",  "stabs",  "slashes", "whips", "claws",
	"blasts",  "pounds", "crushes", "greps", "bites",
	"pierces", "sucks"
    };

    char buf1[512], buf2[512], buf3[512];
    const char *vs;
    const char *vp;
    const char *attack;
    const char *attack2;
    int damp;
    int bodyloc;
    int recover;
    bool critical = FALSE;
    char punct;

         if ( dam ==   0 ) { vs = " miss";	vp = " misses";		}
    else if ( dam <=  25 ) { vs = " lightly";	vp = " lightly";	}
    else if ( dam <=  50 ) { vs = "";		vp = "";		}
    else if ( dam <= 100 ) { vs = " hard";	vp = " hard";		}
    else if ( dam <= 250 ) { vs = " very hard";	vp = " very hard";	}
    else if ( dam <= 500 ) { vs = " extremely hard";vp = " extremely hard";}
    else                   { vs = " incredibly hard";vp = " incredibly hard";}
    /* If victim's hp are less/equal to 'damp', attacker gets a death blow */
    if (IS_NPC(victim)) damp = 0;
    else                damp = -10;
    if ( (victim->hit - dam > damp ) || (dt >= 0 && dt < MAX_SKILL) ||
	(IS_NPC(victim) && IS_SET(victim->act, ACT_NOPARTS)) )
    {
    	punct   = (dam <= 250) ? '.' : '!';

    	if ( dt == TYPE_HIT && !IS_NPC(ch) && !IS_VAMPAFF(ch,VAM_CLAWS) )
    	{
	    if (dam == 0)
	    {
	    	sprintf( buf1, "$n%s $N%c",  vp, punct );
	    	sprintf( buf2, "You%s $N%c", vs, punct );
	    	sprintf( buf3, "$n%s you%c", vp, punct );
	    }
	    else
	    {
	    	sprintf( buf1, "$n hits $N%s%c",  vp, punct );
	    	sprintf( buf2, "You hit $N%s%c", vs, punct );
	    	sprintf( buf3, "$n hits you%s%c", vp, punct );
		critical = TRUE;
	    }
    	}
    	else
    	{
	    if ( dt == TYPE_HIT && !IS_NPC(ch) && IS_VAMPAFF(ch,VAM_CLAWS))
	    {
	    	attack  = attack_table[dt - TYPE_HIT + 5];
	    	attack2 = attack_table2[dt - TYPE_HIT + 5];
	    }
	    else if ( dt >= 0 && dt < MAX_SKILL )
	    {
	    	attack = skill_table[dt].noun_damage;
	    	attack2 = skill_table[dt].noun_damage;
	    }
	    else if ( dt >= TYPE_HIT
	    && dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
	    {
	    	attack  = attack_table[dt - TYPE_HIT];
	    	attack2 = attack_table2[dt - TYPE_HIT];
	    }
	    else
	    {
	        bug( "Dam_message: bad dt %d.", dt );
	        dt  = TYPE_HIT;
	        attack  = attack_table[0];
	        attack2 = attack_table2[0];
	    }

	    if (dam == 0)
	    {
	    	sprintf( buf1, "$n's %s%s $N%c",  attack, vp, punct );
	    	sprintf( buf2, "Your %s%s $N%c",  attack, vp, punct );
	    	sprintf( buf3, "$n's %s%s you%c", attack, vp, punct );
	    }
	    else
	    {
		if ( dt >= 0 && dt < MAX_SKILL )
		{
	    	    sprintf( buf1, "$n's %s strikes $N%s%c",  attack2, vp, punct );
	    	    sprintf( buf2, "Your %s strikes $N%s%c",  attack, vp, punct );
	    	    sprintf( buf3, "$n's %s strikes you%s%c", attack2, vp, punct );
		}
		else
		{
	    	    sprintf( buf1, "$n %s $N%s%c",  attack2, vp, punct );
	    	    sprintf( buf2, "You %s $N%s%c",  attack, vp, punct );
	    	    sprintf( buf3, "$n %s you%s%c", attack2, vp, punct );
		    critical = TRUE;
		}
	    }
	    /* Check for weapon resistance - KaVir */
	    recover = 0;
	    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_SLASH) &&
		(attack == "slash" || attack == "slice") && dam > 0 )
		recover = number_range(1,dam);
	    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_STAB) &&
		(attack == "stab" || attack == "pierce") && dam > 0 )
		recover = number_range(1,dam);
	    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_SMASH) &&
		(attack == "blast" || attack == "pound" || attack == "crush") && dam > 0 )
		recover = number_range(1,dam);
	    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_ANIMAL) &&
		(attack == "bite" || attack == "claw") && dam > 0 )
		recover = number_range(1,dam);
	    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_MISC) &&
		(attack == "grep" || attack == "suck" || attack == "whip") && dam > 0 )
		recover = number_range(1,dam);
	    /* Check for fortitude - KaVir */
	    if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_VAMPIRE) &&
		IS_VAMPAFF(victim, VAM_FORTITUDE) && (dam-recover) > 0)
		victim->hit = victim->hit+number_range(1,(dam-recover));
	    else if (IS_ITEMAFF(victim, ITEMA_RESISTANCE) && (dam-recover) > 0)
		victim->hit = victim->hit+number_range(1,(dam-recover));
	    else if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_DEMON) 
		&& (dam-recover) > 0 && IS_DEMPOWER( victim, DEM_TOUGH))
		victim->hit = victim->hit+number_range(1,(dam-recover));
	    victim->hit = victim->hit+recover;
        }
        act( buf1, ch, NULL, victim, TO_NOTVICT );
        act( buf2, ch, NULL, victim, TO_CHAR );
        act( buf3, ch, NULL, victim, TO_VICT );
	if (critical) critical_hit(ch,victim,dt,dam);
	return;
    }

    if ( dt == TYPE_HIT && !IS_NPC( ch ) && !IS_VAMPAFF(ch,VAM_CLAWS) 
	&&!IS_VAMPAFF(ch,VAM_FANGS))
    {
	damp=number_range(1,5);
	if ( damp == 1 )
	{
	    act("You ram your fingers into $N's eye sockets and rip $S face off.", ch, NULL, victim, TO_CHAR);
	    act("$n rams $s fingers into $N's eye sockets and rips $S face off.", ch, NULL, victim, TO_NOTVICT);
	    act("$n rams $s fingers into your eye sockets and rips your face off.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"face");
	}
	else if ( damp == 2 )
	{
	    act("You grab $N by the throat and tear $S windpipe out.", ch, NULL, victim, TO_CHAR);
	    act("$n grabs $N by the throat and tears $S windpipe out.", ch, NULL, victim, TO_NOTVICT);
	    act("$n grabs you by the throat and tears your windpipe out.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"windpipe");
	}
	else if ( damp == 3 )
	{
	    act("You punch your fist through $N's stomach and rip out $S entrails.", ch, NULL, victim, TO_CHAR);
	    act("$n punches $s fist through $N's stomach and rips out $S entrails.", ch, NULL, victim, TO_NOTVICT);
	    act("$n punches $s fist through your stomach and rips out your entrails.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"entrails");
	}
	else if ( damp == 4 )
	{
	    if (!IS_BODY(victim,BROKEN_SPINE))
		SET_BIT(victim->loc_hp[1],BROKEN_SPINE);
	    act("You hoist $N above your head and slam $M down upon your knee.\n\rThere is a loud cracking sound as $N's spine snaps.", ch, NULL, victim, TO_CHAR);
	    act("$n hoists $N above $s head and slams $M down upon $s knee.\n\rThere is a loud cracking sound as $N's spine snaps.", ch, NULL, victim, TO_NOTVICT);
	    act("$n hoists you above $s head and slams you down upon $s knee.\n\rThere is a loud cracking sound as your spine snaps.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 5 )
	{
	    act("You lock your arm around $N's head, and give it a vicious twist.", ch, NULL, victim, TO_CHAR);
	    act("$n locks $s arm around $N's head, and gives it a vicious twist.", ch, NULL, victim, TO_NOTVICT);
	    act("$n locks $s arm around your head, and gives it a vicious twist.", ch, NULL, victim, TO_VICT);
	    if (!IS_BODY(victim,BROKEN_NECK))
	    {
		act("There is a loud snapping noise as your neck breaks.", victim, NULL, NULL, TO_CHAR);
		act("There is a loud snapping noise as $n's neck breaks.", victim, NULL, NULL, TO_ROOM);
		SET_BIT(victim->loc_hp[1],BROKEN_NECK);
	    }
	}
	return;
    }
    if ( dt >= 0 && dt < MAX_SKILL )
	attack = skill_table[dt].noun_damage;
    else if ( dt >= TYPE_HIT
    && dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
	attack = attack_table[dt - TYPE_HIT];
    else
    {
	bug( "Dam_message: bad dt %d.", dt );
	dt  = TYPE_HIT;
	attack  = attack_table[0];
    }

    if ( attack == "slash" || attack == "slice" )
    {
	damp=number_range(1,8);
	if ( damp == 1 )
	{
	    act("You swing your blade in a low arc, rupturing $N's abdominal cavity.\n\r$S entrails spray out over a wide area.", ch, NULL, victim, TO_CHAR);
	    act("$n swings $s blade in a low arc, rupturing $N's abdominal cavity.\n\r$S entrails spray out over a wide area.", ch, NULL, victim, TO_NOTVICT);
	    act("$n swings $s blade in a low arc, rupturing your abdominal cavity.\n\rYour entrails spray out over a wide area.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"entrails");
	}
	else if ( damp == 2 )
	{
	    act("You thrust your blade into $N's mouth and twist it viciously.\n\rThe end of your blade bursts through the back of $S head.", ch, NULL, victim, TO_CHAR);
	    act("$n thrusts $s blade into $N's mouth and twists it viciously.\n\rThe end of the blade bursts through the back of $N's head.", ch, NULL, victim, TO_NOTVICT);
	    act("$n thrusts $s blade into your mouth and twists it viciously.\n\rYou feel the end of the blade burst through the back of your head.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 3 )
	{
	    if (!IS_BODY(victim,CUT_THROAT))
		SET_BIT(victim->loc_hp[1],CUT_THROAT);
	    if (!IS_BLEEDING(victim,BLEEDING_THROAT))
	    	SET_BIT(victim->loc_hp[6],BLEEDING_THROAT);
	    act("Your blow slices open $N's carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_CHAR);
	    act("$n's blow slices open $N's carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_NOTVICT);
	    act("$n's blow slices open your carotid artery, spraying blood everywhere.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"blood");
	}
	else if ( damp == 4 )
	{
	    if (!IS_BODY(victim,CUT_THROAT))
		SET_BIT(victim->loc_hp[1],CUT_THROAT);
	    if (!IS_BLEEDING(victim,BLEEDING_THROAT))
	    	SET_BIT(victim->loc_hp[6],BLEEDING_THROAT);
	    act("You swing your blade across $N's throat, showering the area with blood.", ch, NULL, victim, TO_CHAR);
	    act("$n swings $s blade across $N's throat, showering the area with blood.", ch, NULL, victim, TO_NOTVICT);
	    act("$n swings $s blade across your throat, showering the area with blood.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"blood");
	}
	else if ( damp == 5 )
	{
	    if (!IS_HEAD(victim,BROKEN_SKULL))
	    {
	    	act("You swing your blade down upon $N's head, splitting it open.\n\r$N's brains pour out of $S forehead.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s blade down upon $N's head, splitting it open.\n\r$N's brains pour out of $S forehead.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s blade down upon your head, splitting it open.\n\rYour brains pour out of your forehead.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"brain");
		SET_BIT(victim->loc_hp[0],BROKEN_SKULL);
	    }
	    else
	    {
		act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
		act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
		act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
	    }
	}
	else if ( damp == 6 )
	{
	    act("You swing your blade between $N's legs, nearly splitting $M in half.", ch, NULL, victim, TO_CHAR);
	    act("$n swings $s blade between $N's legs, nearly splitting $M in half.", ch, NULL, victim, TO_NOTVICT);
	    act("$n swings $s blade between your legs, nearly splitting you in half.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 7 )
	{
	    if (!IS_ARM_L(victim,LOST_ARM))
	    {
	    	act("You swing your blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s blade in a wide arc, slicing off your arm.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"arm");
		SET_BIT(victim->loc_hp[2],LOST_ARM);
		if (!IS_BLEEDING(victim,BLEEDING_ARM_L))
		    SET_BIT(victim->loc_hp[6],BLEEDING_ARM_L);
		if (IS_BLEEDING(victim,BLEEDING_HAND_L))
		    REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_L);
	    }
	    else if (!IS_ARM_R(victim,LOST_ARM))
	    {
	    	act("You swing your blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s blade in a wide arc, slicing off $N's arm.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s blade in a wide arc, slicing off your arm.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"arm");
		SET_BIT(victim->loc_hp[3],LOST_ARM);
		if (!IS_BLEEDING(victim,BLEEDING_ARM_R))
		    SET_BIT(victim->loc_hp[6],BLEEDING_ARM_R);
		if (IS_BLEEDING(victim,BLEEDING_HAND_R))
		    REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_R);
	    }
	    else
	    {
		act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
		act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
		act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
	    }
	}
	else if ( damp == 8 )
	{
	    if (!IS_LEG_L(victim,LOST_LEG))
	    {
	    	act("You swing your blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s blade in a wide arc, slicing off your leg at the hip.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"leg");
		SET_BIT(victim->loc_hp[4],LOST_LEG);
		if (!IS_BLEEDING(victim,BLEEDING_LEG_L))
		    SET_BIT(victim->loc_hp[6],BLEEDING_LEG_L);
		if (IS_BLEEDING(victim,BLEEDING_FOOT_L))
		    REMOVE_BIT(victim->loc_hp[6],BLEEDING_FOOT_L);
	    }
	    else if (!IS_LEG_R(victim,LOST_LEG))
	    {
	    	act("You swing your blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s blade in a low arc, slicing off $N's leg at the hip.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s blade in a wide arc, slicing off your leg at the hip.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"leg");
		SET_BIT(victim->loc_hp[5],LOST_LEG);
		if (!IS_BLEEDING(victim,BLEEDING_LEG_R))
		    SET_BIT(victim->loc_hp[6],BLEEDING_LEG_R);
		if (IS_BLEEDING(victim,BLEEDING_FOOT_R))
		    REMOVE_BIT(victim->loc_hp[6],BLEEDING_FOOT_R);
	    }
	    else
	    {
		act("You plunge your blade deep into $N's chest.", ch, NULL, victim, TO_CHAR);
		act("$n plunges $s blade deep into $N's chest.", ch, NULL, victim, TO_NOTVICT);
		act("$n plunges $s blade deep into your chest.", ch, NULL, victim, TO_VICT);
	    }
	}
    }
    else if ( attack == "stab" || attack == "pierce" )
    {
	damp=number_range(1,5);
	if ( damp == 1 )
	{
	    act("You defty invert your weapon and plunge it point first into $N's chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_CHAR);
	    act("$n defty inverts $s weapon and plunge it point first into $N's chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_NOTVICT);
	    act("$n defty inverts $s weapon and plunge it point first into your chest.\n\rA shower of blood sprays from the wound, showering the area.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"blood");
	}
	else if ( damp == 2 )
	{
	    act("You thrust your blade into $N's mouth and twist it viciously.\n\rThe end of your blade bursts through the back of $S head.", ch, NULL, victim, TO_CHAR);
	    act("$n thrusts $s blade into $N's mouth and twists it viciously.\n\rThe end of the blade bursts through the back of $N's head.", ch, NULL, victim, TO_NOTVICT);
	    act("$n thrusts $s blade into your mouth and twists it viciously.\n\rYou feel the end of the blade burst through the back of your head.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 3 )
	{
	    act("You thrust your weapon up under $N's jaw and through $S head.", ch, NULL, victim, TO_CHAR);
	    act("$n thrusts $s weapon up under $N's jaw and through $S head.", ch, NULL, victim, TO_NOTVICT);
	    act("$n thrusts $s weapon up under your jaw and through your head.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 4 )
	{
	    act("You ram your weapon through $N's body, pinning $M to the ground.", ch, NULL, victim, TO_CHAR);
	    act("$n rams $s weapon through $N's body, pinning $M to the ground.", ch, NULL, victim, TO_NOTVICT);
	    act("$n rams $s weapon through your body, pinning you to the ground.", ch, NULL, victim, TO_VICT);
	}
	else if ( damp == 5 )
	{
	    act("You stab your weapon into $N's eye and out the back of $S head.", ch, NULL, victim, TO_CHAR);
	    act("$n stabs $s weapon into $N's eye and out the back of $S head.", ch, NULL, victim, TO_NOTVICT);
	    act("$n stabs $s weapon into your eye and out the back of your head.", ch, NULL, victim, TO_VICT);
	    if (!IS_HEAD(victim,LOST_EYE_L) && number_percent() < 50)
		SET_BIT(victim->loc_hp[0],LOST_EYE_L);
	    else if (!IS_HEAD(victim,LOST_EYE_R))
		SET_BIT(victim->loc_hp[0],LOST_EYE_R);
	    else if (!IS_HEAD(victim,LOST_EYE_L))
		SET_BIT(victim->loc_hp[0],LOST_EYE_L);
	}
    }
    else if ( attack == "blast" || attack == "pound" || attack == "crush" )
    {
	damp=number_range(1,3);
	bodyloc = 0;
	if ( damp == 1)
	{
	    act("Your blow smashes through $N's chest, caving in half $S ribcage.", ch, NULL, victim, TO_CHAR);
	    act("$n's blow smashes through $N's chest, caving in half $S ribcage.", ch, NULL, victim, TO_NOTVICT);
	    act("$n's blow smashes through your chest, caving in half your ribcage.", ch, NULL, victim, TO_VICT);
	    if (IS_BODY(victim,BROKEN_RIBS_1 )) {bodyloc += 1;
		REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_1);}
	    if (IS_BODY(victim,BROKEN_RIBS_2 )) {bodyloc += 2;
		REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_2);}
	    if (IS_BODY(victim,BROKEN_RIBS_4 )) {bodyloc += 4;
		REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_4);}
	    if (IS_BODY(victim,BROKEN_RIBS_8 )) {bodyloc += 8;
		REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_8);}
	    if (IS_BODY(victim,BROKEN_RIBS_16)) {bodyloc += 16;
		REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_16);}
	    bodyloc += number_range(1,3);
	    if (bodyloc > 24) bodyloc = 24;
	    if (bodyloc >= 16) {bodyloc -= 16;
		SET_BIT(victim->loc_hp[1],BROKEN_RIBS_16);}
	    if (bodyloc >= 8) {bodyloc -= 8;
		SET_BIT(victim->loc_hp[1],BROKEN_RIBS_8);}
	    if (bodyloc >= 4) {bodyloc -= 4;
		SET_BIT(victim->loc_hp[1],BROKEN_RIBS_4);}
	    if (bodyloc >= 2) {bodyloc -= 2;
		SET_BIT(victim->loc_hp[1],BROKEN_RIBS_2);}
	    if (bodyloc >= 1) {bodyloc -= 1;
		SET_BIT(victim->loc_hp[1],BROKEN_RIBS_1);}
	}
	else if ( damp == 2)
	{
	    act("Your blow smashes $N's spine, shattering it in several places.", ch, NULL, victim, TO_CHAR);
	    act("$n's blow smashes $N's spine, shattering it in several places.", ch, NULL, victim, TO_NOTVICT);
	    act("$n's blow smashes your spine, shattering it in several places.", ch, NULL, victim, TO_VICT);
	    if (!IS_BODY(victim,BROKEN_SPINE))
		SET_BIT(victim->loc_hp[1],BROKEN_SPINE);
	}
	else if ( damp == 3)
	{
	    if (!IS_HEAD(victim,BROKEN_SKULL))
	    {
	    	act("You swing your weapon down upon $N's head.\n\r$N's head cracks open like an overripe melon, leaking out brains.", ch, NULL, victim, TO_CHAR);
	    	act("$n swings $s weapon down upon $N's head.\n\r$N's head cracks open like an overripe melon, covering you with brains.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n swings $s weapon down upon your head.\n\rYour head cracks open like an overripe melon, spilling your brains everywhere.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"brain");
		SET_BIT(victim->loc_hp[0],BROKEN_SKULL);
	    }
	    else
	    {
	    	act("You hammer your weapon into $N's side, crushing bone.", ch, NULL, victim, TO_CHAR);
	    	act("$n hammers $s weapon into $N's side, crushing bone.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n hammers $s weapon into your side, crushing bone.", ch, NULL, victim, TO_VICT);
	    }
	}
    }
    else if ( !IS_NPC( ch ) && (attack == "bite" ||IS_VAMPAFF(ch,VAM_FANGS)))
    {
	act("You sink your teeth into $N's throat and tear out $S jugular vein.\n\rYou wipe the blood from your chin with one hand.", ch, NULL, victim, TO_CHAR);
	act("$n sink $s teeth into $N's throat and tears out $S jugular vein.\n\r$n wipes the blood from $s chin with one hand.", ch, NULL, victim, TO_NOTVICT);
	act("$n sink $s teeth into your throat and tears out your jugular vein.\n\r$n wipes the blood from $s chin with one hand.", ch, NULL, victim, TO_VICT);
	make_part(victim,"blood");
	if (!IS_BODY(victim,CUT_THROAT))
	    SET_BIT(victim->loc_hp[1],CUT_THROAT);
	if (!IS_BLEEDING(victim,BLEEDING_THROAT))
	    SET_BIT(victim->loc_hp[6],BLEEDING_THROAT);
    }
    else if ( !IS_NPC(ch) && (attack == "claw" || IS_VAMPAFF(ch,VAM_CLAWS)))
    {
	damp=number_range(1,2);
	if ( damp == 1 )
	{
	    act("You tear out $N's throat, showering the area with blood.", ch, NULL, victim, TO_CHAR);
	    act("$n tears out $N's throat, showering the area with blood.", ch, NULL, victim, TO_NOTVICT);
	    act("$n tears out your throat, showering the area with blood.", ch, NULL, victim, TO_VICT);
	    make_part(victim,"blood");
	    if (!IS_BODY(victim,CUT_THROAT))
		SET_BIT(victim->loc_hp[1],CUT_THROAT);
	    if (!IS_BLEEDING(victim,BLEEDING_THROAT))
	    	SET_BIT(victim->loc_hp[6],BLEEDING_THROAT);
	}
	if ( damp == 2 )
	{
	    if (!IS_HEAD(victim,LOST_EYE_L) && number_percent() < 50)
	    {
	    	act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
	    	act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"eyeball");
		SET_BIT(victim->loc_hp[0],LOST_EYE_L);
	    }
	    else if (!IS_HEAD(victim,LOST_EYE_R))
	    {
	    	act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
	    	act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"eyeball");
		SET_BIT(victim->loc_hp[0],LOST_EYE_R);
	    }
	    else if (!IS_HEAD(victim,LOST_EYE_L))
	    {
	    	act("You rip an eyeball from $N's face.", ch, NULL, victim, TO_CHAR);
	    	act("$n rips an eyeball from $N's face.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n rips an eyeball from your face.", ch, NULL, victim, TO_VICT);
	    	make_part(victim,"eyeball");
		SET_BIT(victim->loc_hp[0],LOST_EYE_L);
	    }
	    else
	    {
	    	act("You claw open $N's chest.", ch, NULL, victim, TO_CHAR);
	    	act("$n claws open $N's chest.", ch, NULL, victim, TO_NOTVICT);
	    	act("$n claws open $N's chest.", ch, NULL, victim, TO_VICT);
	    }
	}
    }
    else if ( attack == "whip" )
    {
	act("You entangle $N around the neck, and squeeze the life out of $S.", ch, NULL, victim, TO_CHAR);
	act("$n entangle $N around the neck, and squeezes the life out of $S.", ch, NULL, victim, TO_NOTVICT);
	act("$n entangles you around the neck, and squeezes the life out of you.", ch, NULL, victim, TO_VICT);
	if (!IS_BODY(victim,BROKEN_NECK))
	    SET_BIT(victim->loc_hp[1],BROKEN_NECK);
    }
    else if ( attack == "suck" || attack == "grep" )
    {
	act("You place your weapon on $N's head and suck out $S brains.", ch, NULL, victim, TO_CHAR);
	act("$n places $s weapon on $N's head and suck out $S brains.", ch, NULL, victim, TO_NOTVICT);
	act("$n places $s weapon on your head and suck out your brains.", ch, NULL, victim, TO_VICT);
    }
    else
    {
	bug( "Dam_message: bad dt %d.", dt );
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
    char buf [MAX_STRING_LENGTH];

    /* I'm fed up of being disarmed every 10 seconds - KaVir */
    if (IS_NPC(ch) && victim->level > 2 && number_percent() > 5)
	return;

    if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_DISARM))
	return;
/*
    if ( ( (obj = get_eq_char( ch, WEAR_WIELD ) == NULL) || obj->item_type != ITEM_WEAPON )
    &&   ( (obj = get_eq_char( ch, WEAR_HOLD  ) == NULL) || obj->item_type != ITEM_WEAPON ) )
	return;
*/
    if ( ( (obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL) || obj->item_type != ITEM_WEAPON )
    {
	if ( ( (obj = get_eq_char( victim, WEAR_HOLD ) ) == NULL) || obj->item_type != ITEM_WEAPON )
	return;
    }

    sprintf(buf,"$n disarms you!");
    ADD_COLOUR(ch,buf,WHITE);
    act( buf, ch, NULL, victim, TO_VICT    );
    sprintf(buf,"You disarm $N!");
    ADD_COLOUR(ch,buf,WHITE);
    act( buf,  ch, NULL, victim, TO_CHAR    );
    sprintf(buf,"$n disarms $N!");
    ADD_COLOUR(ch,buf,WHITE);
    act( buf,  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );

    /* Loyal weapons come back ;)  KaVir */
    if (IS_SET(obj->extra_flags, ITEM_LOYAL) && ( !IS_NPC(victim) ) )
    {
        act( "$p leaps back into your hand!", victim, obj, NULL, TO_CHAR );
        act( "$p leaps back into $n's hand!",  victim, obj, NULL, TO_ROOM );
	obj_to_char( obj, victim );
	do_wear( victim, obj->name);
    }
    else if ( IS_NPC(victim) )
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
    char buf [MAX_STRING_LENGTH];

    if (IS_AFFECTED(victim,AFF_FLYING))
	return;

    if (IS_NPC(ch) && victim->level > 2 && number_percent() > 5)
	return;

    if (!IS_NPC(victim))
    {
	if (IS_CLASS(victim, CLASS_VAMPIRE) && IS_VAMPAFF(victim,VAM_FLYING))
	    return;
	if (IS_CLASS(victim, CLASS_DEMON) && IS_DEMAFF(victim, DEM_UNFOLDED))
	    return;
	if (IS_SET(victim->special, SPC_CHAMPION) && IS_DEMAFF(victim,DEM_UNFOLDED))
	    return;
    }
    if ( victim->wait == 0)
    {
    	sprintf(buf,"$n trips you and you go down!");
    	ADD_COLOUR(ch,buf,WHITE);
    	act( buf, ch, NULL, victim, TO_VICT    );
    	sprintf(buf,"You trip $N and $E goes down!");
    	ADD_COLOUR(ch,buf,WHITE);
    	act( buf,  ch, NULL, victim, TO_CHAR    );
    	sprintf(buf,"$n trips $N and $E goes down!");
    	ADD_COLOUR(ch,buf,WHITE);
    	act( buf,  ch, NULL, victim, TO_NOTVICT );

	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
	victim->position = POS_RESTING;
    }

    return;
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "You cannot kill yourself!\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    check_killer( ch, victim );
    if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_WEREWOLF) && number_range(1,3) == 1
	&& ch->pcdata->powers[WPOWER_BOAR] > 1 && victim->position == POS_STANDING)
    {
/*
	ch->damroll += 50;
	multi_hit( ch, victim, TYPE_UNDEFINED );
	ch->damroll -= 50;
*/
	act("You charge into $N, knocking $M from $S feet.",ch,NULL,victim,TO_CHAR);
	act("$n charge into $N, knocking $M from $S feet.",ch,NULL,victim,TO_NOTVICT);
	act("$n charge into you, knocking you from your feet.",ch,NULL,victim,TO_VICT);
	victim->position = POS_STUNNED;
	multi_hit( ch, victim, TYPE_UNDEFINED );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return;
    }
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Backstab whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
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

    if ( ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
    ||   obj->value[3] != 11 )
    &&   ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
    ||   obj->value[3] != 11 ) )
    {
	send_to_char( "You need to wield a piercing weapon.\n\r", ch );
	return;
    }

    if ( victim->fighting != NULL )
    {
	send_to_char( "You can't backstab a fighting person.\n\r", ch );
	return;
    }

    if ( victim->hit < victim->max_hit )
    {
	act( "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_BACKSTAB) )
	damage( ch, victim, 0, gsn_backstab );
    else if ( !IS_AWAKE(victim)
    ||   IS_NPC(ch)
    ||   number_percent( ) < ch->pcdata->learned[gsn_backstab] )
	multi_hit( ch, victim, gsn_backstab );
    else
	damage( ch, victim, 0, gsn_backstab );

    return;
}



void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ( ( victim = ch->fighting ) == NULL )
    {
	if ( ch->position == POS_FIGHTING )
	    ch->position = POS_STANDING;
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch, AFF_WEBBED))
    {
	send_to_char("You are unable to move with all this sticky webbing on.\n\r",ch);
	return;
    }
    if (!IS_NPC(ch) && ch->pcdata->stats[UNI_RAGE] >= 0)
    {
	if (IS_CLASS(ch,CLASS_VAMPIRE) && number_percent() <= ch->pcdata->stats[UNI_RAGE])
	{
	    send_to_char("Your inner beast refuses to let you run!\n\r",ch);
	    WAIT_STATE(ch,12);
	    return;
	}
	else if (IS_CLASS(ch, CLASS_WEREWOLF) && number_percent() <= ch->pcdata->stats[UNI_RAGE] * 0.3)
	{
	    send_to_char("Your rage is too great!\n\r",ch);
	    WAIT_STATE(ch,12);
	    return;
	}
    }
    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
	EXIT_DATA *pexit;
	int door;

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == 0
	||   pexit->to_room == NULL
	||   IS_SET(pexit->exit_info, EX_CLOSED)
	|| ( IS_NPC(ch)
	&&   IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) ) )
	    continue;

	move_char( ch, door );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( "$n has fled!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;

	if ( !IS_NPC(ch) )
	    send_to_char( "You flee from combat!  Coward!\n\r", ch );

	stop_fighting( ch, TRUE );
	return;
    }

    send_to_char( "You were unable to escape!\n\r", ch );
    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about fleeing instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    if ( is_safe(ch, fch) || is_safe(ch, victim) ) return;

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( !IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[gsn_rescue] )
    {
	send_to_char( "You fail the rescue.\n\r", ch );
	return;
    }

    act( "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n rescues you!", ch, NULL, victim, TO_VICT    );
    act( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    check_killer( ch, fch );
    set_fighting( ch, fch );
    set_fighting( fch, ch );

    do_humanity(ch,"");
    return;
}



void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *boots;
    int dam;
    int stance;
    int hitpoints;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_kick].skill_level)
    {
	send_to_char(
	    "First you should learn to kick.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_kick] )
    	dam = number_range(1,4);
    else
    {
    	dam = 0;
    	damage( ch, victim, dam, gsn_kick );
	return;
    }

    dam += char_damroll(ch);
    if (dam == 0) dam = 1;
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) &&
	IS_VAMPAFF(ch, VAM_POTENCE) )
	dam *= 1.5;
    else if ( !IS_NPC(ch) && (IS_SET(ch->special, SPC_CHAMPION) ||
	IS_CLASS(ch, CLASS_DEMON)) && 
	IS_DEMPOWER( ch, DEM_MIGHT) )
	dam *= 1.5;
    if ( !IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) )
    {
	if (IS_SET(victim->special, SPC_WOLFMAN)) dam *= 0.5;
	if (victim->pcdata->powers[WPOWER_BOAR] > 2 ) dam *= 0.5;
	if ( ( boots = get_eq_char( ch, WEAR_FEET ) ) != NULL 
	    && IS_SET(boots->spectype, SITEM_SILVER)) dam *= 2;
    }
    /* Vampires should be tougher at night and weaker during the day. */
    if ( !IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE) )
    {
	if (weather_info.sunlight == SUN_LIGHT && dam > 1)
	    dam /= 1.5;
	else if (weather_info.sunlight == SUN_DARK)
	    dam *= 1.5;
    }
    if ( !IS_NPC(ch) ) dam = dam + (dam * ((ch->wpn[0]+1) / 100));

    if ( !IS_NPC(ch) )
    {
	stance = ch->stance[0];
	if ( IS_STANCE(ch, STANCE_NORMAL) ) dam *= 1.25;
	else dam = dambonus(ch,victim,dam,stance);
    }

    if ( dam <= 0 )
	dam = 1;

    hitpoints = victim->hit;
    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_KICK) )
    {
	victim->hit += dam;
	damage( ch, victim, dam, gsn_kick );
	victim->hit = hitpoints;
    }
    else damage( ch, victim, dam, gsn_kick );
    return;
}


int dambonus( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int stance)
{
    if (dam < 1) return 0;
    if (stance < 1) return dam;
    if (!IS_NPC(ch) && !can_counter(victim))
    {
	if ( IS_STANCE(ch, STANCE_MONKEY) )
	{
	    int mindam = dam * 0.25;
	    dam *= (ch->stance[STANCE_MONKEY]+1) / 200;
	    if (dam < mindam) dam = mindam;
	}
	else if ( IS_STANCE(ch, STANCE_BULL) && ch->stance[STANCE_BULL] > 100 )
	    dam += dam * (ch->stance[STANCE_BULL] / 100);
	else if ( IS_STANCE(ch, STANCE_DRAGON) && ch->stance[STANCE_DRAGON] > 100 )
	    dam += dam * (ch->stance[STANCE_DRAGON] / 100);
	else if ( IS_STANCE(ch, STANCE_TIGER) && ch->stance[STANCE_TIGER] > 100 )
	    dam += dam * (ch->stance[STANCE_TIGER] / 100);
	else if ( ch->stance[0] > 0 && ch->stance[stance] < 100 )
	    dam *= 0.5;
    }
    if (!IS_NPC(victim) && !can_counter(ch))
    {
	if ( IS_STANCE(victim, STANCE_CRAB) && victim->stance[STANCE_CRAB] > 100 )
	    dam /= victim->stance[STANCE_CRAB]/100;
	else if ( IS_STANCE(victim, STANCE_DRAGON) && victim->stance[STANCE_DRAGON] > 100 )
	    dam /= victim->stance[STANCE_DRAGON]/100;
	else if ( IS_STANCE(victim, STANCE_SWALLOW) && victim->stance[STANCE_SWALLOW] > 100 )
	    dam /= victim->stance[STANCE_SWALLOW]/100;
    }
    return dam;
}



void do_punch( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int dam;
    int store;
    bool broke = FALSE;

    one_argument(argument,arg);

    if ( IS_NPC(ch) ) return;

    if ( ch->level < skill_table[gsn_punch].skill_level)
    {
	send_to_char( "First you should learn to punch.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot punch yourself!\n\r", ch );
	return;
    }

    if (is_safe(ch,victim)) return;

    if ( victim->hit < victim->max_hit )
    {
	send_to_char( "They are hurt and suspicious.\n\r", ch );
	return;
    }

    if ( victim->position < POS_FIGHTING )
    {
	send_to_char( "You can only punch someone who is standing.\n\r", ch );
	return;
    }

    act("You draw your fist back and aim a punch at $N.",ch,NULL,victim,TO_CHAR);
    act("$n draws $s fist back and aims a punch at you.",ch,NULL,victim,TO_VICT);
    act("$n draws $s fist back and aims a punch at $N.",ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE( ch, skill_table[gsn_punch].beats );
    if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_punch] )
    	dam = number_range(1,4);
    else
    {
    	dam = 0;
    	damage( ch, victim, dam, gsn_punch );
	return;
    }

    dam += char_damroll(ch);
    if (dam == 0) dam = 1;
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( !IS_NPC(ch) ) dam = dam + (dam * (ch->wpn[0] / 100));

    if ( dam <= 0 )
	dam = 1;

    if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) && victim->pcdata->powers[WPOWER_BOAR] > 3)
    {
	store = victim->hit;
	victim->hit += dam;
	damage( ch, victim, dam, gsn_punch );
	victim->hit = store;
	if (number_percent() <= 25 && !IS_ARM_L(ch,LOST_ARM) && 
	!IS_ARM_L(ch,LOST_HAND))
	{
	    if (!IS_ARM_L(ch, LOST_FINGER_I) && !IS_ARM_L(ch, BROKEN_FINGER_I))
		{SET_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_I);broke = TRUE;}
	    if (!IS_ARM_L(ch, LOST_FINGER_M) && !IS_ARM_L(ch, BROKEN_FINGER_M))
		{SET_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_M);broke = TRUE;}
	    if (!IS_ARM_L(ch, LOST_FINGER_R) && !IS_ARM_L(ch, BROKEN_FINGER_R))
		{SET_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_R);broke = TRUE;}
	    if (!IS_ARM_L(ch, LOST_FINGER_L) && !IS_ARM_L(ch, BROKEN_FINGER_L))
		{SET_BIT(ch->loc_hp[LOC_ARM_L],BROKEN_FINGER_L);broke = TRUE;}
	    if (broke)
	    {
		act("The fingers on your left hand shatter under the impact of the blow!",ch,NULL,NULL,TO_CHAR);
		act("The fingers on $n's left hand shatter under the impact of the blow! ",ch,NULL,NULL,TO_ROOM);
	    }
	}
	else if (number_percent() <= 25 && !IS_ARM_R(ch,LOST_ARM) && 
	!IS_ARM_R(ch,LOST_HAND))
	{
	    if (!IS_ARM_R(ch, LOST_FINGER_I) && !IS_ARM_R(ch, BROKEN_FINGER_I))
		{SET_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_I);broke = TRUE;}
	    if (!IS_ARM_R(ch, LOST_FINGER_M) && !IS_ARM_R(ch, BROKEN_FINGER_M))
		{SET_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_M);broke = TRUE;}
	    if (!IS_ARM_R(ch, LOST_FINGER_R) && !IS_ARM_R(ch, BROKEN_FINGER_R))
		{SET_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_R);broke = TRUE;}
	    if (!IS_ARM_R(ch, LOST_FINGER_L) && !IS_ARM_R(ch, BROKEN_FINGER_L))
		{SET_BIT(ch->loc_hp[LOC_ARM_R],BROKEN_FINGER_L);broke = TRUE;}
	    if (broke)
	    {
		act("The fingers on your right hand shatter under the impact of the blow!",ch,NULL,NULL,TO_CHAR);
		act("The fingers on $n's right hand shatter under the impact of the blow! ",ch,NULL,NULL,TO_ROOM);
	    }
	}
	stop_fighting(victim,TRUE);
	return;
    }

    damage( ch, victim, dam, gsn_punch );
    if (victim == NULL || victim->position == POS_DEAD || dam < 1) return;
    if (victim->position == POS_FIGHTING) stop_fighting(victim,TRUE);
    if (number_percent() <= 25 && !IS_HEAD(victim,BROKEN_NOSE)
	 && !IS_HEAD(victim,LOST_NOSE))
    {
    	act("Your nose shatters under the impact of the blow!",victim,NULL,NULL,TO_CHAR);
    	act("$n's nose shatters under the impact of the blow!",victim,NULL,NULL,TO_ROOM);
	SET_BIT(victim->loc_hp[LOC_HEAD],BROKEN_NOSE);
    }
    else if (number_percent() <= 25 && !IS_HEAD(victim,BROKEN_JAW))
    {
    	act("Your jaw shatters under the impact of the blow!",victim,NULL,NULL,TO_CHAR);
    	act("$n's jaw shatters under the impact of the blow!",victim,NULL,NULL,TO_ROOM);
	SET_BIT(victim->loc_hp[LOC_HEAD],BROKEN_JAW);
    }
    act("You fall to the ground stunned!",victim,NULL,NULL,TO_CHAR);
    act("$n falls to the ground stunned!",victim,NULL,NULL,TO_ROOM);
    victim->position = POS_STUNNED;
    return;
}


void do_berserk( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *mount;
    int number_hit = 0;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( ch->level < skill_table[gsn_berserk].skill_level)
    {
	send_to_char( "You are not wild enough to go berserk.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_berserk].beats );
    if ( number_percent( ) > ch->pcdata->learned[gsn_berserk] )
    {
    	act("You rant and rave, but nothing much happens.",ch,NULL,NULL,TO_CHAR);
    	act("$n gets a wild look in $s eyes, but nothing much happens.",ch,NULL,NULL,TO_ROOM);
	return;
    }

    act("You go BERSERK!",ch,NULL,NULL,TO_CHAR);
    act("$n goes BERSERK!",ch,NULL,NULL,TO_ROOM);
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if (number_hit > 4) continue;
	if ( vch->in_room == NULL )
	    continue;
	if ( !IS_NPC(vch) && vch->pcdata->chobj != NULL )
	    continue;
	if ( ch == vch )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( ( mount = ch->mount ) != NULL ) {if ( mount == vch ) continue;}
	    if (can_see(ch,vch))
	    {
		multi_hit( ch, vch, TYPE_UNDEFINED );
		number_hit++;
	    }
	}
    }
    do_beastlike(ch,"");
    return;
}



/* Hurl skill by KaVir */
void do_hurl( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    CHAR_DATA       *mount;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA       *pexit;
    EXIT_DATA       *pexit_rev;
    char            buf       [MAX_INPUT_LENGTH];
    char            direction [MAX_INPUT_LENGTH];
    char            arg1      [MAX_INPUT_LENGTH];
    char            arg2      [MAX_INPUT_LENGTH];
    int             door;
    int             rev_dir;
    int             dam;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_hurl] < 1 )
    {
	send_to_char("Maybe you should learn the skill first?\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char("Who do you wish to hurl?\n\r", ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char("They are not here.\n\r", ch);
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you hurl yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( (mount = victim->mount) != NULL && victim->mounted == IS_MOUNT)
    {
	send_to_char("But they have someone on their back!\n\r",ch);
	return;
    }
    else if ( (mount = victim->mount) != NULL && victim->mounted == IS_RIDING)
    {
	send_to_char("But they are riding!\n\r",ch);
	return;
    }

    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_HURL) )
    {
	send_to_char("You are unable to get their feet of the ground.\n\r",ch);
	return;
    }

    if ( IS_NPC(victim) && victim->level > 900 )
    {
	send_to_char("You are unable to get their feet of the ground.\n\r",ch);
	return;
    }

    if ( (victim->hit < victim->max_hit)
	 || ( victim->position == POS_FIGHTING && victim->fighting != ch) )
    {
	act( "$N is hurt and suspicious, and you are unable to approach $M.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_hurl].beats );
    if ( !IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[gsn_hurl] )
    {
	    send_to_char("You are unable to get their feet of the ground.\n\r",ch);
	    multi_hit( victim, ch, TYPE_UNDEFINED );
	    return;
    }

    rev_dir = 0;

    if ( arg2[0] == '\0' )
	door = number_range(0,3);
    else
    {
    	if      ( !str_cmp( arg2, "n" ) || !str_cmp( arg2, "north" ) )
	    door = 0;
    	else if ( !str_cmp( arg2, "e" ) || !str_cmp( arg2, "east" ) )
	    door = 1;
    	else if ( !str_cmp( arg2, "s" ) || !str_cmp( arg2, "south" ) )
	    door = 2;
    	else if ( !str_cmp( arg2, "w" ) || !str_cmp( arg2, "west" ) )
	    door = 3;
    	else
    	{
	    send_to_char("You can only hurl people north, south, east or west.\n\r", ch);
	    return;
    	}
    }

    if (door == 0) {sprintf(direction,"north");rev_dir = 2;}
    if (door == 1) {sprintf(direction,"east");rev_dir = 3;}
    if (door == 2) {sprintf(direction,"south");rev_dir = 0;}
    if (door == 3) {sprintf(direction,"west");rev_dir = 1;}

    if (( pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
    {
	sprintf(buf,"$n hurls $N into the %s wall.", direction);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"You hurl $N into the %s wall.", direction);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"$n hurls you into the %s wall.", direction);
	act(buf,ch,NULL,victim,TO_VICT);
	dam = number_range(ch->level, (ch->level * 4));
	victim->hit = victim->hit - dam;
	update_pos(victim);
    	if (IS_NPC(victim) && !IS_NPC(ch)) ch->mkill = ch->mkill + 1;
    	if (!IS_NPC(victim) && IS_NPC(ch)) victim->mdeath = victim->mdeath + 1;
	if (victim->position == POS_DEAD) {raw_kill(victim);return;}
	return;
    }

    pexit = victim->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) && 
	!IS_AFFECTED(victim,AFF_PASS_DOOR ) &&
	!IS_AFFECTED(victim,AFF_ETHEREAL  ) )
    {
	if (IS_SET(pexit->exit_info, EX_LOCKED))
	    REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	if (IS_SET(pexit->exit_info, EX_CLOSED))
	    REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	sprintf(buf,"$n hoists $N in the air and hurls $M %s.", direction);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"You hoist $N in the air and hurl $M %s.", direction);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"$n hurls you %s, smashing you through the %s.", direction,pexit->keyword);
	act(buf,ch,NULL,victim,TO_VICT);
	sprintf(buf,"There is a loud crash as $n smashes through the $d.");
	act(buf,victim,NULL,pexit->keyword,TO_ROOM);

	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir] ) != NULL
	&&   pexit_rev->to_room == ch->in_room
	&&   pexit_rev->keyword != NULL )
	{
	    if (IS_SET(pexit_rev->exit_info, EX_LOCKED))
		REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	    if (IS_SET(pexit_rev->exit_info, EX_CLOSED))
		REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    if (door == 0) sprintf(direction,"south");
	    if (door == 1) sprintf(direction,"west");
	    if (door == 2) sprintf(direction,"north");
	    if (door == 3) sprintf(direction,"east");
	    char_from_room(victim);
	    char_to_room(victim,to_room);
	    sprintf(buf,"$n comes smashing in through the %s $d.", direction);
	    act(buf,victim,NULL,pexit->keyword,TO_ROOM);
	    dam = number_range(ch->level, (ch->level * 6));
	    victim->hit = victim->hit - dam;
	    update_pos(victim);
    	    if (IS_NPC(victim) && !IS_NPC(ch)) ch->mkill = ch->mkill + 1;
    	    if (!IS_NPC(victim) && IS_NPC(ch)) victim->mdeath = victim->mdeath + 1;
	    if (victim->position == POS_DEAD) {raw_kill(victim);return;}
	}
    }
    else
    {
	sprintf(buf,"$n hurls $N %s.", direction);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"You hurl $N %s.", direction);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"$n hurls you %s.", direction);
	act(buf,ch,NULL,victim,TO_VICT);
	if (door == 0) sprintf(direction,"south");
	if (door == 1) sprintf(direction,"west");
	if (door == 2) sprintf(direction,"north");
	if (door == 3) sprintf(direction,"east");
	char_from_room(victim);
	char_to_room(victim,to_room);
	sprintf(buf,"$n comes flying in from the %s.", direction);
	act(buf,victim,NULL,NULL,TO_ROOM);
	dam = number_range(ch->level, (ch->level * 2));
	victim->hit = victim->hit - dam;
	update_pos(victim);
    	if (IS_NPC(victim) && !IS_NPC(ch)) ch->mkill = ch->mkill + 1;
    	if (!IS_NPC(victim) && IS_NPC(ch)) victim->mdeath = victim->mdeath + 1;
	if (victim->position == POS_DEAD) {raw_kill(victim);return;}
    }
    return;
}



void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_disarm].skill_level)
    {
	send_to_char( "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if ( ( get_eq_char( ch, WEAR_WIELD ) == NULL )
    &&   ( get_eq_char( ch, WEAR_HOLD  ) == NULL ) )
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    &&   ( ( obj = get_eq_char( victim, WEAR_HOLD  ) ) == NULL ) )
    {
	send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
    percent = number_percent( ) + victim->level - ch->level;
    if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_DISARM))
	send_to_char( "You failed.\n\r", ch );
    else if ( IS_NPC(ch) || percent < ch->pcdata->learned[gsn_disarm] * 2 / 3 )
	disarm( ch, victim );
    else
	send_to_char( "You failed.\n\r", ch );
    return;
}



void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_GODLESS))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && victim->level >= ch->level )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    act( "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR    );
    act( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
    act( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
    raw_kill( victim );
    return;
}

/* Had problems with people not dying when POS_DEAD...KaVir */
void do_killperson( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	return;
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	return;
    send_to_char( "You have been KILLED!!\n\r\n\r", victim );
    if (IS_NPC(victim) && !IS_NPC(ch)) ch->mkill = ch->mkill + 1;
    if (!IS_NPC(victim) && IS_NPC(ch)) victim->mdeath = victim->mdeath + 1;
    raw_kill( victim );
    return;
}

/* For decapitating players - KaVir */
void do_decapitate( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    OBJ_DATA  *obj2;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    obj2 = get_eq_char( ch, WEAR_HOLD );
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
    	if ( ( obj2 = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    	{
	    send_to_char( "First you better get a weapon out!\n\r", ch );
	    return;
        }
    }

    if ( ( obj != NULL && obj->item_type != ITEM_WEAPON ) )
    {
    	if ( ( obj2 != NULL && obj2->item_type != ITEM_WEAPON ) )
    	{
	    send_to_char( "But you are not wielding any weapons!\n\r", ch );
	    return;
        }
    }

    if ( ( obj  != NULL && obj->value[3]  != 1 && obj->value[3]  != 3 )
    &&   ( obj2 != NULL && obj2->value[3] != 1 && obj2->value[3] != 3 ) )
    {
	send_to_char( "You need to wield a slashing or slicing weapon to decapitate.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Decapitate whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "That might be a bit tricky...\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "You can only decapitate other players.\n\r", ch );
	return;
    }

    if ( !CAN_PK(ch) )
    {
	send_to_char( "You must be an avatar to decapitate someone.\n\r", ch );
	return;
    }

    if ( !CAN_PK(victim) )
    {
	send_to_char( "You can only decapitate other avatars.\n\r", ch );
	return;
    }

    if ( victim->position > 1 )
    {
	send_to_char( "You can only do this to mortally wounded players.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim)) return;

    if ( IS_CLASS(ch, CLASS_VAMPIRE) && IS_SET(ch->special, SPC_INCONNU) && 
	(IS_CLASS(victim, CLASS_VAMPIRE) || IS_VAMPAFF(victim, VAM_MORTAL)))
    {
	send_to_char( "You cannot decapitate another vampire.\n\r", ch );
	return;
    }

    if ( IS_CLASS(victim, CLASS_VAMPIRE) && IS_SET(victim->special, SPC_INCONNU) && 
	(IS_CLASS(ch, CLASS_VAMPIRE) || IS_VAMPAFF(ch, VAM_MORTAL)))
    {
	send_to_char( "You cannot decapitate an Inconnu vampire.\n\r", ch );
	return;
    }

    if ( IS_CLASS(ch, CLASS_VAMPIRE) && IS_CLASS(victim, CLASS_VAMPIRE) &&
	strlen(ch->clan) > 1 && strlen(victim->clan) > 1)
    {
	if (!str_cmp(ch->clan,victim->clan))
	{
	    send_to_char( "You cannot decapitate someone of your own clan.\n\r", ch );
	    return;
	}
    }

    act( "You bring your weapon down upon $N's neck!",  ch, NULL, victim, TO_CHAR    );
    send_to_char( "Your head is sliced from your shoulders!\n\r", victim);
    act( "$n swings $s weapon down towards $N's neck!",  ch, NULL, victim, TO_NOTVICT );
    act( "$n's head is sliced from $s shoulders!", victim, NULL, NULL, TO_ROOM);
/*
    clan_table_dec( ch,victim );
*/
    if (IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special, SPC_CHAMPION)) 

    if ((IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special, SPC_CHAMPION)) && 
	!IS_CLASS(victim,CLASS_DEMON) && !IS_SET(victim->special,SPC_CHAMPION))
    {
	if (ch->race == 0 && victim->race == 0)
	{
	    ch->pcdata->stats[DEMON_CURRENT] += 1000;
	    ch->pcdata->stats[DEMON_TOTAL] += 1000;
	}
	else
	{
	    ch->pcdata->stats[DEMON_CURRENT] += victim->race * 1000;
	    ch->pcdata->stats[DEMON_TOTAL] += victim->race * 1000;
	}
    }
    if ( victim->race < 1 && ch->race > 0 )
    {
	ch->paradox[0] += ch->race;
	ch->paradox[1] += ch->race;
	ch->paradox[2] += PARADOX_TICK;
/*
	if (!str_cmp(ch->name,"amoeba"))
	{
	    ch->paradox[0] += (25 - ch->race);
	    ch->paradox[1] += (25 - ch->race);
 	}
*/
    	sprintf(buf,"%s has been decapitated by %s for no status.",victim->name,ch->name);
    	do_info(ch,buf);
	sprintf( log_buf, "%s decapitated by %s at %d for no status.",
	    victim->name, ch->name, victim->in_room->vnum );
	log_string( log_buf );
	if (IS_CLASS(victim, CLASS_VAMPIRE))      do_mortalvamp(victim,"");
	else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
	if (IS_CLASS(victim, CLASS_MAGE) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
	    do_unpolymorph(victim,"");
	behead( victim );
	do_beastlike(ch,"");
	ch->pkill = ch->pkill + 1;
	victim->pdeath = victim->pdeath + 1;
	return;
    }
    ch->exp = ch->exp +1000;
    if (ch->race - ((ch->race/100)*100) == 0)
    	ch->race = ch->race + 1;
    else if (ch->race - ((ch->race/100)*100) < 25)
    	ch->race = ch->race + 1;
    if (ch->race - ((ch->race/100)*100) == 0)
    	victim->race = victim->race;
    else if (victim->race - ((victim->race/100)*100) > 0)
    	victim->race = victim->race - 1;
    act( "A misty white vapour pours from $N's corpse into your body.",  ch, NULL, victim, TO_CHAR    );
    act( "A misty white vapour pours from $N's corpse into $n's body.",  ch, NULL, victim, TO_NOTVICT );
    act( "You double over in agony as raw energy pulses through your veins.",  ch, NULL, NULL, TO_CHAR    );
    act( "$n doubles over in agony as sparks of energy crackle around $m.",  ch, NULL, NULL, TO_NOTVICT );
    if (IS_CLASS(victim, CLASS_VAMPIRE))      do_mortalvamp(victim,"");
    else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
    if (IS_CLASS(victim, CLASS_MAGE) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
	do_unpolymorph(victim,"");
    behead( victim );
    do_beastlike(ch,"");
    ch->pkill = ch->pkill + 1;
    victim->pdeath = victim->pdeath + 1;
    victim->pcdata->stats[UNI_RAGE] = 0;
    victim->level = 2;
    sprintf(buf,"%s has been decapitated by %s.",victim->name,ch->name);
    do_info(ch,buf);
    sprintf( log_buf, "%s decapitated by %s at %d.",
	victim->name, ch->name, victim->in_room->vnum );
    log_string( log_buf );
    return;
}

void do_tear( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if ( !IS_CLASS(ch, CLASS_WEREWOLF))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( !IS_SET(ch->special, SPC_WOLFMAN))
    {
	send_to_char( "You can only tear heads off while in Crinos form.\n\r", ch );
	return;
    }

    if ( !IS_VAMPAFF(ch, VAM_CLAWS))
    {
	send_to_char( "You better get your claws out first.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who's head do you wish to tear off?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "That might be a bit tricky...\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "You can only tear the heads off other players.\n\r", ch );
	return;
    }

    if ( !CAN_PK(ch) )
    {
	send_to_char( "You must be an avatar to tear someone's head off.\n\r", ch );
	return;
    }

    if ( !CAN_PK(victim) )
    {
	send_to_char( "You can only tear the head off another avatar.\n\r", ch );
	return;
    }

    if ( victim->position > 1 )
    {
	send_to_char( "You can only do this to mortally wounded players.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim)) return;

    act( "You tear $N's head from $S shoulders!",  ch, NULL, victim, TO_CHAR    );
    send_to_char( "Your head is torn from your shoulders!\n\r", victim);
    act( "$n tears $N's head from $S shoulders!",  ch, NULL, victim, TO_NOTVICT );
/*
    clan_table_dec( ch,victim );
*/
    if (IS_CLASS(ch, CLASS_DEMON) || IS_SET(ch->special, SPC_CHAMPION)) 
    {
	if (ch->race == 0 && victim->race == 0)
	{
	    ch->pcdata->stats[DEMON_CURRENT] += 1000;
	    ch->pcdata->stats[DEMON_TOTAL] += 1000;
	}
	else
	{
	    ch->pcdata->stats[DEMON_CURRENT] += victim->race * 1000;
	    ch->pcdata->stats[DEMON_TOTAL] += victim->race * 1000;
	}
    }
    if ( victim->race < 1 && ch->race > 0 )
    {
	ch->paradox[0] += ch->race;
	ch->paradox[1] += ch->race;
	ch->paradox[2] += PARADOX_TICK;
/*
	if (!str_cmp(ch->name,"amoeba"))
	{
	    ch->paradox[0] += (25 - ch->race);
	    ch->paradox[1] += (25 - ch->race);
	}
*/
    	sprintf(buf,"%s has been decapitated by %s.",victim->name,ch->name);
    	do_info(ch,buf);
	sprintf( log_buf, "%s decapitated by %s at %d for no status.",
	    victim->name, ch->name, victim->in_room->vnum );
	log_string( log_buf );
	if (IS_CLASS(victim, CLASS_VAMPIRE))      do_mortalvamp(victim,"");
	else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
	if (IS_CLASS(victim, CLASS_MAGE) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
	    do_unpolymorph(victim,"");
	behead( victim );
	do_beastlike(ch,"");
	ch->pkill = ch->pkill + 1;
	victim->pdeath = victim->pdeath + 1;
	return;
    }
    ch->exp = ch->exp +1000;
    if (ch->race - ((ch->race/100)*100) == 0)
    	ch->race = ch->race + 1;
    else if (ch->race - ((ch->race/100)*100) < 25)
    	ch->race = ch->race + 1;
    if (ch->race - ((ch->race/100)*100) == 0)
    	victim->race = victim->race;
    else if (victim->race - ((victim->race/100)*100) > 0)
    	victim->race = victim->race - 1;
    act( "A misty white vapour pours from $N's corpse into your body.",  ch, NULL, victim, TO_CHAR    );
    act( "A misty white vapour pours from $N's corpse into $n's body.",  ch, NULL, victim, TO_NOTVICT );
    act( "You double over in agony as raw energy pulses through your veins.",  ch, NULL, NULL, TO_CHAR    );
    act( "$n doubles over in agony as sparks of energy crackle around $m.",  ch, NULL, NULL, TO_NOTVICT );
    if (IS_CLASS(victim, CLASS_VAMPIRE))      do_mortalvamp(victim,"");
    else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
    if (IS_CLASS(victim, CLASS_MAGE) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
	do_unpolymorph(victim,"");
    behead( victim );
    do_beastlike(ch,"");
    ch->pkill = ch->pkill + 1;
    victim->pdeath = victim->pdeath + 1;
    victim->pcdata->stats[UNI_RAGE] = 0;
    victim->level = 2;
    sprintf(buf,"%s has been decapitated by %s.",victim->name,ch->name);
    do_info(ch,buf);
    sprintf( log_buf, "%s decapitated by %s at %d.",
	victim->name, ch->name, victim->in_room->vnum );
    log_string( log_buf );
    return;
}

void do_crack( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    OBJ_DATA  *right;
    OBJ_DATA  *left;

    right = get_eq_char( ch, WEAR_WIELD );
    left = get_eq_char( ch, WEAR_HOLD );
    if (right != NULL && right->pIndexData->vnum == 12) obj = right;
    else if (left != NULL && left->pIndexData->vnum == 12) obj = left;
    else
    {
	send_to_char("You are not holding any heads.\n\r",ch);
	return;
    }
    act("You hurl $p at the floor.", ch, obj, NULL, TO_CHAR);
    act("$n hurls $p at the floor.", ch, obj, NULL, TO_ROOM);
    act("$p cracks open, leaking brains out across the floor.", ch, obj, NULL, TO_CHAR);
    if (obj->chobj != NULL)
    	{act("$p cracks open, leaking brains out across the floor.", ch, obj, obj->chobj, TO_NOTVICT);
    	act("$p crack open, leaking brains out across the floor.", ch, obj, obj->chobj, TO_VICT);}
    else
    	{act("$p cracks open, leaking brains out across the floor.", ch, obj, NULL, TO_ROOM);}
    crack_head(ch,obj,obj->name);
    obj_from_char(obj);
    extract_obj(obj);
}

void crack_head( CHAR_DATA *ch, OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *victim;
    MOB_INDEX_DATA *pMobIndex; 

    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char       buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if (str_cmp(arg2,"mob") && obj->chobj != NULL && !IS_NPC(obj->chobj) &&
	IS_AFFECTED(obj->chobj,AFF_POLYMORPH))
    {
	victim = obj->chobj;
	make_part(victim,"cracked_head");
	make_part(victim,"brain");
	sprintf(buf,"the quivering brain of %s",victim->name);
	free_string(victim->morph);
	victim->morph = str_dup(buf);
	return;
    }
    else if (!str_cmp(arg2,"mob"))
    {
	if ( ( pMobIndex = get_mob_index( obj->value[1] ) ) == NULL ) return;
	victim = create_mobile( pMobIndex );
	char_to_room(victim,ch->in_room);
	make_part(victim,"cracked_head");
	make_part(victim,"brain");
	extract_char(victim,TRUE);
	return;
    }
    else
    {
	if ( ( pMobIndex = get_mob_index( 30002 ) ) == NULL ) return;
	victim = create_mobile( pMobIndex );

	sprintf( buf, capitalize(arg2) );
	free_string( victim->short_descr );
	victim->short_descr = str_dup( buf );

	char_to_room(victim,ch->in_room);
	make_part(victim,"cracked_head");
	make_part(victim,"brain");
	extract_char(victim,TRUE);
	return;
    }
    return;
}

/* Voodoo skill by KaVir */

void do_voodoo( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char       buf [MAX_INPUT_LENGTH];
    char     part1 [MAX_INPUT_LENGTH];
    char     part2 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0')
    {
	send_to_char( "Who do you wish to use voodoo magic on?\n\r", ch );
	return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You are not holding a voodoo doll.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They are not here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    sprintf(part2,obj->name);
    sprintf(part1,"%s voodoo doll",victim->name);

    if ( str_cmp(part1,part2) )
    {
	sprintf(buf,"But you are holding %s, not %s!\n\r",obj->short_descr,victim->name);
	send_to_char( buf, ch );
	return;
    }

    if ( arg2[0] == '\0')
    {
	send_to_char( "You can 'stab', 'burn' or 'throw' the doll.\n\r", ch );
    }
    else if ( !str_cmp(arg2, "stab") )
    {
	WAIT_STATE(ch,12);
	act("You stab a pin through $p.", ch, obj, NULL, TO_CHAR); 
 	act("$n stabs a pin through $p.", ch, obj, NULL, TO_ROOM);
	if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_VOODOO)) return;
	act("You feel an agonising pain in your chest!", victim, NULL, NULL, TO_CHAR);
 	act("$n clutches $s chest in agony!", victim, NULL, NULL, TO_ROOM);
    }
    else if ( !str_cmp(arg2, "burn") )
    {
	WAIT_STATE(ch,12);
	act("You set fire to $p.", ch, obj, NULL, TO_CHAR);
 	act("$n sets fire to $p.", ch, obj, NULL, TO_ROOM);
	act("$p burns to ashes.", ch, obj, NULL, TO_CHAR);
 	act("$p burns to ashes.", ch, obj, NULL, TO_ROOM);
	obj_from_char(obj);
	extract_obj(obj);
	if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_VOODOO)) return;
	if (IS_AFFECTED(victim,AFF_FLAMING) )
	    return;
	SET_BIT(victim->affected_by, AFF_FLAMING);
	act("You suddenly burst into flames!", victim, NULL, NULL, TO_CHAR);
 	act("$n suddenly bursts into flames!", victim, NULL, NULL, TO_ROOM);
    }
    else if ( !str_cmp(arg2, "throw") )
    {
	WAIT_STATE(ch,12);
	act("You throw $p to the ground.", ch, obj, NULL, TO_CHAR);
 	act("$n throws $p to the ground.", ch, obj, NULL, TO_ROOM);
	obj_from_char(obj);
	obj_to_room(obj, ch->in_room);
	if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_VOODOO)) return;
	if (victim->position < POS_STANDING)
	    return;
	if (victim->position == POS_FIGHTING)
	    stop_fighting(victim, TRUE);
	act("A strange force picks you up and hurls you to the ground!", victim, NULL, NULL, TO_CHAR);
 	act("$n is hurled to the ground by a strange force.", victim, NULL, NULL, TO_ROOM);
	victim->position = POS_RESTING;
	victim->hit = victim->hit - number_range(ch->level,(5*ch->level));
	update_pos(victim);
	if (victim->position == POS_DEAD && !IS_NPC(victim))
	{
	    do_killperson(ch,victim->name);
	    return;
	}
    }
    else
    {
	send_to_char( "You can 'stab', 'burn' or 'throw' the doll.\n\r", ch );
    }
    return;
}

