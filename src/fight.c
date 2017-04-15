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
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	check_killer	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	dam_message	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
bool	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	make_corpse	args( ( CHAR_DATA *ch ) );
void	one_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int handtype ) );
void	raw_kill	args( ( CHAR_DATA *victim ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	trip		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	improve_wpn	args( ( CHAR_DATA *ch, int dtype, bool right_hand ) );
void	improve_stance	args( ( CHAR_DATA *ch ) );
void	skillstance	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	show_spell	args( ( CHAR_DATA *ch, int dtype ) );
void	fightaction	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int actype, int dtype, int wpntype ) );
void    crack_head      args( ( CHAR_DATA *ch, OBJ_DATA *obj, char *argument ) );
void	critical_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int dam) );
void	take_item	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	clear_stats	args( ( CHAR_DATA *ch ) );



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

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	ch_next	= ch->next;

	if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
	    continue;

	if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
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
		if ( rch->mount != NULL && rch->mount == ch )
		{
		    multi_hit( rch, victim, TYPE_UNDEFINED );
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

    if ( ch->position < POS_SLEEPING ) return;

    wield1 = get_eq_char( ch, WEAR_WIELD );
    wield2 = get_eq_char( ch, WEAR_HOLD );
    if ( wield1 != NULL && wield1->item_type == ITEM_WEAPON ) wieldorig  = 1;
    if ( wield2 != NULL && wield2->item_type == ITEM_WEAPON ) wieldorig += 2;
    wieldtype = wieldorig;

    if ( wieldorig == 3 ) { if (number_range(1,2) == 2) wieldtype = 2; else wieldtype = 1;}
    if      ( wieldtype == 2 ) wield = wield2;
    else                       wield = wield1;

    unarmed = 0;
    if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
	ch->cmbt[unarmed] != 0 && wieldorig == 0 )
	fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
    else
	one_hit( ch, victim, dt, wieldtype );

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

    if ( !IS_NPC(victim) && number_percent( ) <= victim->pcdata->learned[gsn_fastdraw] )
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
	if (wpntype  > 0) one_hit( victim, ch, TYPE_UNDEFINED,  1 );
	if (wpntype2 > 0) one_hit( victim, ch, TYPE_UNDEFINED, 2 );
    }

    if ( ch->fighting != victim || dt == gsn_backstab )
	return;

    if (IS_NPC(ch)) chance = ch->level * 2;
    else
    {
    	wieldtype = wieldorig;
    	if ( wieldorig == 3 ) { if (number_range(1,2) == 2) wieldtype = 2;
		else wieldtype = 1;}
	if (wieldtype == 1 && wield1 != NULL && wield1->value[3] >= 0 &&
		wield1->value[3] <= 12) chance = ch->wpn[wield1->value[3]];
	else if (wieldtype == 2 && wield2 != NULL && wield2->value[3] >= 0 &&
		wield2->value[3] <= 12) chance = ch->wpn[wield2->value[3]];
	else chance = ch->wpn[0];
    }
    if ( number_percent( ) < chance )
    {
    	unarmed = number_range(0,3);
    	if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
	ch->cmbt[unarmed] != 0 && wieldorig == 0 )
	    fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
	else
    	    one_hit( ch, victim, -1, wieldtype );
	if ( ch->fighting != victim ) return;
    }

    if (IS_NPC(ch)) chance = ch->level;
    else
    {
    	wieldtype = wieldorig;
    	if ( wieldorig == 3 ) { if (number_range(1,2) == 2) wieldtype = 2;
		else wieldtype = 1;}
	if (wieldtype == 1 && wield1 != NULL && wield1->value[3] >= 0 &&
		wield1->value[3] <= 12) chance = ch->wpn[wield1->value[3]] / 2;
	else if (wieldtype == 2 && wield2 != NULL && wield2->value[3] >= 0 &&
		wield2->value[3] <= 12) chance = ch->wpn[wield2->value[3]] / 2;
	else chance = ch->wpn[0] / 2;
    }
    if ( number_percent( ) < chance )
    {
    	unarmed = number_range(4,7);
    	if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
	ch->cmbt[unarmed] != 0 && wieldorig == 0 )
	    fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
	else
    	    one_hit( ch, victim, -1, wieldtype );
	if ( ch->fighting != victim ) return;
    }

    chance = IS_NPC(ch) ? ch->level / 2 : 0;
    if ( number_percent( ) < chance )
    {
    	unarmed = number_range(0,7);
    	if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
	ch->cmbt[unarmed] != 0 && wieldorig == 0 )
	    fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
	else
	{
    	    wieldtype = wieldorig;
    	    if ( wieldorig == 3 ) { if (number_range(1,2) == 2) wieldtype = 2; else wieldtype = 1;}
    	    one_hit( ch, victim, -1, wieldtype );
	}
	if ( ch->fighting != victim ) return;
    }

    unarmed = number_range(0,7);
    if ( !IS_NPC(ch) && IS_STANCE(ch, STANCE_VIPER) && number_percent() > ch->stance[STANCE_VIPER] )
    {
    	if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
	ch->cmbt[unarmed] != 0 && wieldorig == 0 )
	    fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
	else
	{
    	    wieldtype = wieldorig;
    	    if ( wieldorig == 3 ) { if (number_range(1,2) == 2) wieldtype = 2; else wieldtype = 1;}
    	    one_hit( ch, victim, -1, wieldtype );
	}
	if ( ch->fighting != victim ) return;
    }

    unarmed = number_range(0,7);
    if ( !IS_NPC(ch) && IS_STANCE(ch, STANCE_VIPER) && number_percent() > ch->stance[STANCE_VIPER] - 100 )
    {
    	if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
	ch->cmbt[unarmed] != 0 && wieldorig == 0 )
	    fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
	else
	{
    	    wieldtype = wieldorig;
    	    if ( wieldorig == 3 ) { if (number_range(1,2) == 2) wieldtype = 2; else wieldtype = 1;}
    	    one_hit( ch, victim, -1, wieldtype );
	}
	if ( ch->fighting != victim ) return;
    }

    unarmed = number_range(0,7);
    if ( !IS_NPC(ch) && IS_VAMPAFF(ch, VAM_CELERITY) && number_percent() > 33 )
    {
    	if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
	ch->cmbt[unarmed] != 0 && wieldorig == 0 )
	    fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
	else
	{
    	    wieldtype = wieldorig;
    	    if ( wieldorig == 3 ) { if (number_range(1,2) == 2) wieldtype = 2; else wieldtype = 1;}
    	    one_hit( ch, victim, -1, wieldtype );
	}
	if ( ch->fighting != victim ) return;
    }

    unarmed = number_range(0,7);
    if ( !IS_NPC(ch) && IS_VAMPAFF(ch, VAM_CELERITY) && number_percent() > 66 )
    {
    	if ( !IS_NPC(ch) && !IS_AFFECTED(ch, AFF_POLYMORPH) &&
	ch->cmbt[unarmed] != 0 && wieldorig == 0 )
	    fightaction(ch,victim,ch->cmbt[unarmed], dt, wieldtype);
	else
	{
    	    wieldtype = wieldorig;
    	    if ( wieldorig == 3 ) { if (number_range(1,2) == 2) wieldtype = 2; else wieldtype = 1;}
    	    one_hit( ch, victim, -1, wieldtype );
	}
	if ( ch->fighting != victim ) return;
    }

    if (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FANGS))
	one_hit( ch, victim, ( TYPE_HIT + 10 ), 0 );

    /* Vampires in serpent form have a chance of poisoning their opponent. */

    if (!IS_NPC(ch) && IS_VAMPAFF(ch,VAM_FANGS) && IS_VAMPAFF(ch,VAM_CHANGED)
	&& IS_POLYAFF(ch,POLY_SERPENT) && number_percent() > 75 )
	spell_poison( gsn_poison, (ch->level*number_range(5,10)),ch,victim );

    if (victim->itemaffect < 1) return;
    if (IS_NPC(victim) || victim->spl[1] < 4) level = victim->level;
    else level = (victim->spl[1]/4);
    if (IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD))
	if ( (sn = skill_lookup( "lightning bolt" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    if (IS_ITEMAFF(victim, ITEMA_FIRESHIELD))
	if ( (sn = skill_lookup( "fireball" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    if (IS_ITEMAFF(victim, ITEMA_ICESHIELD))
	if ( (sn = skill_lookup( "chill touch" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    if (IS_ITEMAFF(victim, ITEMA_ACIDSHIELD))
	if ( (sn = skill_lookup( "acid blast" ) ) > 0)
	    (*skill_table[sn].spell_fun) (sn,level,victim,ch);
    return;
}



/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int handtype)
{
    OBJ_DATA *wield;
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
	thac0_00 = class_table[ch->class].thac0_00;
	thac0_32 = class_table[ch->class].thac0_32;
    }
    thac0     = interpolate( level, thac0_00, thac0_32 ) - GET_HITROLL(ch);
    victim_ac = UMAX( -25, GET_AC(victim) / 10 );
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
	improve_wpn(ch,dt,right_hand);
    	improve_stance(ch);
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
	    dam += dam / 2;
    }
    else
    {
	if ( IS_VAMPAFF(ch,VAM_CLAWS) && wield == NULL)
	    dam = number_range( 10, 20 );
	else if ( wield != NULL )
	    dam = number_range( wield->value[1], wield->value[2] );
	else
	    dam = number_range( 1, 4 );
    }

    /*
     * Bonuses.
     */
    dam += GET_DAMROLL(ch);
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_VAMPIRE))
	dam += ch->pcdata->wolf;
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( dt == gsn_backstab )
	dam *= number_range(2,4);
    if ( !IS_NPC(ch) && IS_VAMPAFF(ch, VAM_POTENCE) )
	dam *= 1.5;
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_WEREWOLF) )
	dam *= 2;
    if ( !IS_NPC(ch) ) stance = ch->stance[0];
    if ( !IS_NPC(ch) && IS_STANCE(ch, STANCE_NORMAL) )
	dam *= 1.25;
    else if ( !IS_NPC(ch) && IS_STANCE(ch, STANCE_BULL) && ch->stance[STANCE_BULL] > 100 )
	dam = dam + (dam * ((ch->stance[STANCE_BULL]-100) / 100));
    else if ( !IS_NPC(ch) && ch->stance[0] > 0 && ch->stance[stance] < 100 )
	dam = dam * (ch->stance[stance] + 1) / 100;
    /* Vampires should be tougher at night and weaker during the day. */
    if ( IS_SET(ch->act, PLR_VAMPIRE) )
    {
	if (weather_info.sunlight == SUN_LIGHT )
	    dam *= 1.5;
	else if (weather_info.sunlight == SUN_DARK )
	    dam /= 1.5;
    }
    if ( !IS_NPC(ch) && dt >= TYPE_HIT)
	dam = dam + (dam * (ch->wpn[dt-1000] / 100));

    if ( dam <= 0 )
	dam = 1;

    damage( ch, victim, dam, dt );
    tail_chain( );
    improve_wpn(ch,dt,right_hand);
    improve_stance(ch);
    return;
}



/*
 * Inflict damage from a hit.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    if ( victim->position == POS_DEAD )
	return;

    /*
     * Stop up any residual loopholes.
     */
    if ( dam > 1000 ) dam = 1000;

    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return;
	check_killer( ch, victim );

	if ( victim->position > POS_STUNNED && ch->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch );
	    victim->position = POS_FIGHTING;
	    ch->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED && ch->position > POS_STUNNED )
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

	if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
	    dam /= 2;

	if ( IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch) )
	    dam -= dam / 4;

	if ( dam < 0 )
	    dam = 0;

	/*
	 * Check for disarm, trip, parry, and dodge.
	 */
	if ( dt >= TYPE_HIT )
	{
	    if ( IS_NPC(ch) && number_percent( ) < ch->level / 2 )
		disarm( ch, victim );
	    if ( IS_NPC(ch) && number_percent( ) < ch->level / 2 )
		trip( ch, victim );
	    if ( check_parry( ch, victim, dt ) )
		return;
	    if ( !IS_NPC(victim) && IS_STANCE(victim, STANCE_CRANE) && 
		victim->stance[STANCE_CRANE] > 100 && check_parry( ch, victim, dt ))
		return;
	    if ( check_dodge( ch, victim ) )
		return;
	    if ( !IS_NPC(victim) && IS_STANCE(victim, STANCE_MONGOOSE) && 
		victim->stance[STANCE_MONGOOSE] > 100 && check_dodge( ch, victim ))
		return;
	}

	dam_message( ch, victim, dam, dt );
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_WEREWOLF) && dam > 0)
	{
	    victim->pcdata->wolf += 1;
	    if (!IS_SET(victim->act, PLR_WOLFMAN) && victim->pcdata->wolf >= 100)
		do_werewolf(victim,"");
	}
    }

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
	    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_VAMPIRE)
		&& number_percent() < victim->beast)
		do_rage(victim,"");
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
	    if (ch->level == 1 && ch->mkill > 4)
	    {
		ch->level = 2;
		do_save(ch,"");
	    }
	}
    	if (!IS_NPC(victim) && IS_NPC(ch)) victim->mdeath = victim->mdeath + 1;
	raw_kill( victim );

	if ( IS_SET(ch->act, PLR_AUTOLOOT) )
	    do_get( ch, "all corpse" );
	else
	    do_look( ch, "in corpse" );

	if ( !IS_NPC(ch) && IS_NPC(victim) )
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

    if ( victim->level != 3 || ch->level != 3 )
    {
	send_to_char( "Both players must be avatars to fight.\n\r", ch );
	return TRUE;
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
    OBJ_DATA *obj;
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC(victim) )
    {
	/* Tuan was here.  :) */
	chance	= victim->level;
	obj = NULL;
    }
    else
    {
	if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) != NULL 
		&& obj->item_type == ITEM_WEAPON && obj->value[3] >= 0
		&& obj->value[3] <= 12)
	    chance = ch->wpn[obj->value[3]] / 3;
	else if ( ( obj = get_eq_char( victim, WEAR_HOLD ) ) != NULL 
		&& obj->item_type == ITEM_WEAPON && obj->value[3] >= 0
		&& obj->value[3] <= 12)
	    chance = ch->wpn[obj->value[3]] / 3;
	else return FALSE;
    }

    if (!IS_NPC(ch) && (dt < 1000 || dt > 1012)) return FALSE;
    if (!IS_NPC(ch))     chance = chance - (    ch->wpn[dt-1000]/10);
    if (!IS_NPC(victim)) chance = chance + (victim->wpn[dt-1000]/10);
    if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_CRANE) &&
	victim->stance[STANCE_CRANE] > 0)
	chance += victim->stance[STANCE_CRANE] / 4;
    if (chance > 95) chance = 95;

    if ( number_percent( ) < 100 && number_percent( ) >= chance + victim->level - ch->level + victim->hitroll - ch->hitroll )
	return FALSE;

    if (!IS_NPC(ch) && obj != NULL && obj->item_type == ITEM_WEAPON &&
	obj->value[3] >= 0 && obj->value[3] <= 12)
    {
    	act( "You parry $n's blow with $p.",  ch, obj, victim, TO_VICT );
    	act( "$N parries your blow with $p.", ch, obj, victim, TO_CHAR );
	return TRUE;
    }
    act( "You parry $n's attack.",  ch, NULL, victim, TO_VICT    );
    act( "$N parries your attack.", ch, NULL, victim, TO_CHAR    );
    return TRUE;
}



/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;
    int dodge1;
    int dodge2;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    if ( IS_NPC(victim) )
	/* Tuan was here.  :) */
        chance  = victim->level;
    else if (victim->wpn[0] < 20)
        chance  = 0;
    else
        chance  = (victim->wpn[0] / 2) - 5;

    dodge1 = victim->carry_weight;
    dodge2 = can_carry_w(victim);

    if (!IS_NPC(victim) && IS_STANCE(victim, STANCE_MONGOOSE) &&
	victim->stance[STANCE_MONGOOSE] > 0)
	chance += victim->stance[STANCE_MONGOOSE] / 4;
    if (chance > 95) chance = 95;

    if (dodge2 < 1) return FALSE;
    else if ( (dodge1 < 1) && (number_percent( ) >= chance) )
	return FALSE;
    else if (dodge1 > 0 && number_percent( ) >= chance - (dodge1 * 100/dodge2))
	return FALSE;

    act( "You dodge $n's attack.", ch, NULL, victim, TO_VICT    );
    act( "$N dodges your attack.", ch, NULL, victim, TO_CHAR    );
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
	    victim->position = POS_STANDING;
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



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    int vnum;

    vnum = 0;
    switch ( number_bits( 4 ) )
    {
	/* Took out cut off bits, etc.  Will include as part of the final
	 * Death moves.  KaVir.
	 */
    	default: msg  = "$n hits the ground ... DEAD.";			break;
    }

    act( msg, ch, NULL, NULL, TO_ROOM );

    if ( vnum != 0 )
    {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	name		= IS_NPC(ch) ? ch->short_descr : ch->name;
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
    if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_STAKE))
	REMOVE_BIT(victim->immune, IMM_STAKE);
    victim->itemaffect	= 0;
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
    save_char_obj( victim );
    return;
}


void behead( CHAR_DATA *victim )
{
    char buf [MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;

    if (IS_NPC(victim)) return;

    location = get_room_index(victim->in_room->vnum);

    stop_fighting( victim, TRUE );

    make_part( victim, "head" );

    make_corpse( victim );

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
    if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_STAKE))
	REMOVE_BIT(victim->immune, IMM_STAKE);
    victim->itemaffect	= 0;
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

    char_from_room(victim);
    char_to_room(victim,location);
    SET_BIT(victim->loc_hp[0],LOST_HEAD);
    SET_BIT(victim->affected_by,AFF_POLYMORPH);
    sprintf(buf,"the severed head of %s",victim->name);
    free_string(victim->morph);
    victim->morph = str_dup(buf);

    save_char_obj( victim );
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( (IS_NPC(ch) && ch->mount == NULL)  || victim == ch )
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
	if (gch->mount != NULL) send_to_char( buf, gch->mount );
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

    xp    = 300 - URANGE( -3, gch->level - victim->level, 6 ) * 50;
    align = gch->alignment - victim->alignment;

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
     * Adjust for popularity of target:
     *   -1/8 for each target over  'par' (down to -100%)
     *   +1/8 for each target under 'par' (  up to + 25%)
     */
    level  = URANGE( 0, victim->level, MAX_LEVEL - 1 );
    number = UMAX( 1, kill_table[level].number );
    extra  = victim->pIndexData->killed - kill_table[level].killed / number;
/*  xp    -= xp * URANGE( -2, extra, 8 ) / 8; */
    xp    -= xp * URANGE( -2, extra, 6 ) / 8;

    xp     = number_range( xp * 3 / 4, xp * 5 / 4 );
    xp     = UMAX( 0, xp );
    xp     = (xp * (victim->level+1)) / (gch->level+1);
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

    char buf1[256], buf2[256], buf3[256];
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
    else if ( dam <=   4 ) { vs = " lightly";	vp = " lightly";	}
    else if ( dam <=  10 ) { vs = "";		vp = "";		}
    else if ( dam <=  20 ) { vs = " hard";	vp = " hard";		}
    else if ( dam <=  40 ) { vs = " very hard";	vp = " very hard";	}
    else if ( dam <=  99 ) { vs = " extremely hard";vp = " extremely hard";}
    else                   { vs = " incredably hard";vp = " incredably hard";}
    /* If victim's hp are less/equal to 'damp', attacker gets a death blow */
    if (IS_NPC(victim)) damp = 0;
    else                damp = -10;
    if ( (victim->hit - dam > damp ) || (dt >= 0 && dt < MAX_SKILL) )
    {
    	punct   = (dam <= 40) ? '.' : '!';

    	if ( dt == TYPE_HIT && !IS_VAMPAFF(ch,VAM_CLAWS) )
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
	    if ( dt == TYPE_HIT && IS_VAMPAFF(ch,VAM_CLAWS) )
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
	    if ( !IS_NPC(victim) && IS_VAMPAFF(victim, VAM_FORTITUDE) && (dam-recover) > 0)
		victim->hit = victim->hit+number_range(1,(dam-recover));
	    victim->hit = victim->hit+recover;
        }
        act( buf1, ch, NULL, victim, TO_NOTVICT );
        act( buf2, ch, NULL, victim, TO_CHAR );
        act( buf3, ch, NULL, victim, TO_VICT );
	if (critical) critical_hit(ch,victim,dt,dam);
	return;
    }

    if ( dt == TYPE_HIT && !IS_VAMPAFF(ch,VAM_CLAWS) && !IS_VAMPAFF(ch,VAM_FANGS))
    {
	damp=number_range(1,4);
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
    else if ( attack == "bite" || IS_VAMPAFF(ch,VAM_FANGS))
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
    else if ( attack == "claw" || IS_VAMPAFF(ch,VAM_CLAWS))
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
    char buf [MAX_INPUT_LENGTH];

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
    char buf [MAX_INPUT_LENGTH];
    if (IS_AFFECTED(victim,AFF_FLYING))
	return;
    if (!IS_NPC(victim) && IS_VAMPAFF(victim,VAM_FLYING))
	return;
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
    int dam;
    int stance;
    int hitpoints;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_kick].skill_level[ch->class] )
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

    dam += GET_DAMROLL(ch);
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_VAMPIRE))
	dam += ch->pcdata->wolf;
    if (dam == 0) dam = 1;
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( !IS_NPC(ch) && IS_VAMPAFF(ch, VAM_POTENCE) )
	dam *= 1.5;
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_WEREWOLF) )
	dam *= 2;
    if ( !IS_NPC(ch) ) stance = ch->stance[0];
    if ( !IS_NPC(ch) && IS_STANCE(ch, STANCE_NORMAL) )
	dam *= 1.25;
    else if ( !IS_NPC(ch) && IS_STANCE(ch, STANCE_BULL) && ch->stance[STANCE_BULL] > 100 )
	dam = dam + (dam * ((ch->stance[STANCE_BULL]-100) / 100));
    else if ( !IS_NPC(ch) && ch->stance[0] > 0 && ch->stance[stance] < 100 )
	dam = dam * (ch->stance[stance] + 1) / 100;
    /* Vampires should be tougher at night and weaker during the day. */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_VAMPIRE) )
    {
	if (weather_info.sunlight == SUN_LIGHT )
	    dam *= 1.5;
	else if (weather_info.sunlight == SUN_DARK )
	    dam /= 1.5;
    }
    if ( !IS_NPC(ch) ) dam = dam + (dam * (ch->wpn[0] / 100));

    if ( dam <= 0 )
	dam = 1;

    hitpoints = victim->hit;
    damage( ch, victim, dam, gsn_kick );
    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_KICK) )
	victim->hit = hitpoints;
    return;
}


void do_punch( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int dam;

    one_argument(argument,arg);

    if ( IS_NPC(ch) ) return;

    if ( ch->level < skill_table[gsn_punch].skill_level[ch->class] )
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

    dam += GET_DAMROLL(ch);
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_VAMPIRE))
	dam += ch->pcdata->wolf;
    if (dam == 0) dam = 1;
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( !IS_NPC(ch) ) dam = dam + (dam * (ch->wpn[0] / 100));

    if ( dam <= 0 )
	dam = 1;

    damage( ch, victim, dam, gsn_punch );
    if (victim == NULL || victim->position == POS_DEAD) return;
    stop_fighting(victim,TRUE);
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

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( ch->level < skill_table[gsn_berserk].skill_level[ch->class] )
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
	if ( vch->in_room == NULL )
	    continue;
	if ( ch == vch )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( can_see( ch, vch) && (vch->mount == NULL || vch->mount != ch) )
		multi_hit( ch, vch, TYPE_UNDEFINED );
	}
    }
    do_beastlike(ch,"");
    return;
}



/* Hurl skill by KaVir */
void do_hurl( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
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

    if ( victim->mount != NULL && victim->mounted == IS_MOUNT)
    {
	send_to_char("But they have someone on their back!\n\r",ch);
	return;
    }
    else if ( victim->mount != NULL && victim->mounted == IS_RIDING)
    {
	send_to_char("But they are riding!\n\r",ch);
	return;
    }

    if ( !IS_NPC(victim) && IS_IMMUNE(victim, IMM_HURL) )
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
    &&   ch->level < skill_table[gsn_disarm].skill_level[ch->class] )
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
    char buf[MAX_INPUT_LENGTH];

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

    if ( ch->level != 3 )
    {
	send_to_char( "You must be an avatar to decapitate someone.\n\r", ch );
	return;
    }

    if ( victim->level != 3 )
    {
	send_to_char( "You can only decapitate other avatars.\n\r", ch );
	return;
    }

    if ( victim->position > 1 )
    {
	send_to_char( "You can only do this to mortally wounded players.\n\r", ch );
	return;
    }
/*
    if ( IS_SET(ch->act, PLR_VAMPIRE) && IS_SET(victim->act, PLR_VAMPIRE) &&
	!str_cmp(ch->clan,victim->clan) && ch->clan != '\0' &&
	ch->clan != str_dup("") )
    {
	send_to_char( "You cannot decapitate someone of your own clan.\n\r", ch );
	return;
    }
*/
    act( "You bring your weapon down upon $N's neck!",  ch, NULL, victim, TO_CHAR    );
    send_to_char( "Your head is sliced from your shoulders!\n\r", victim);
    act( "$n swings $s weapon down towards $N's neck!",  ch, NULL, victim, TO_NOTVICT );
    behead( victim );
    do_beastlike(ch,"");
    ch->pkill = ch->pkill + 1;
    victim->pdeath = victim->pdeath + 1;
    if ( victim->race < 1 && ch->race > 0 )
    {
    	sprintf(buf,"%s has been decapitated by %s.",victim->name,ch->name);
    	do_info(ch,buf);
	return;
    }
    victim->level = victim->level -1;
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
    if (IS_SET(victim->act, PLR_VAMPIRE))
	do_mortalvamp(victim,"");
    sprintf(buf,"%s has been decapitated by %s.",victim->name,ch->name);
    do_info(ch,buf);
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
	act("You stab a pin through $p.", ch, obj, NULL, TO_CHAR); 
 	act("$n stabs a pin through $p.", ch, obj, NULL, TO_ROOM);
	if (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_VOODOO)) return;
	act("You feel an agonising pain in your chest!", victim, NULL, NULL, TO_CHAR);
 	act("$n clutches $s chest in agony!", victim, NULL, NULL, TO_ROOM);
    }
    else if ( !str_cmp(arg2, "burn") )
    {
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

/* Need to get rid of those flames somehow - KaVir */
void do_smother( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Smother whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot smother yourself.\n\r", ch );
	return;
    }

    if (!IS_AFFECTED(victim, AFF_FLAMING))
    {
	send_to_char( "But they are not on fire!\n\r", ch );
	return;
    }

    if ( number_percent() > (ch->level*2) )
    {
	act( "You try to smother the flames around $N but fail!",  ch, NULL, victim, TO_CHAR    );
	act( "$n tries to smother the flames around you but fails!", ch, NULL, victim, TO_VICT    );
	act( "$n tries to smother the flames around $N but fails!",  ch, NULL, victim, TO_NOTVICT );
	if ( number_percent() > 98 && !IS_AFFECTED(ch,AFF_FLAMING) )
	{
	    act( "A spark of flame from $N's body sets you on fire!",  ch, NULL, victim, TO_CHAR    );
	    act( "A spark of flame from your body sets $n on fire!", ch, NULL, victim, TO_VICT    );
	    act( "A spark of flame from $N's body sets $n on fire!",  ch, NULL, victim, TO_NOTVICT );
	    SET_BIT(ch->affected_by, AFF_FLAMING);
	    do_humanity(ch,"");
	}
	return;
    }

    act( "You manage to smother the flames around $M!",  ch, NULL, victim, TO_CHAR    );
    act( "$n manages to smother the flames around you!", ch, NULL, victim, TO_VICT    );
    act( "$n manages to smother the flames around $N!",  ch, NULL, victim, TO_NOTVICT );
    REMOVE_BIT(victim->affected_by, AFF_FLAMING);
    do_humanity(ch,"");
    return;
}

/* Loads of Vampire powers follow.  KaVir */

void do_fangs( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (IS_VAMPAFF(ch,VAM_FANGS) )
    {
	send_to_char("Your fangs slide back into your gums.\n\r",ch);
	act("$n's fangs slide back into $s gums.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->vampaff, VAM_FANGS);
	return;
    }
    send_to_char("Your fangs extend out of your gums.\n\r",ch);
    act("A pair of razor sharp fangs extend from $n's mouth.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->vampaff, VAM_FANGS);
    return;
}

void do_claws( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_PROTEAN) )
    {
	send_to_char("You are not trained in the Protean discipline.\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_CLAWS) )
    {
	send_to_char("Your claws slide back under your nails.\n\r",ch);
	act("$n's claws slide back under $s nails.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->vampaff, VAM_CLAWS);
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 10 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(5,10);
    send_to_char("Sharp claws extend from under your finger nails.\n\r",ch);
    act("Sharp claws extend from under $n's finger nails.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->vampaff, VAM_CLAWS);
    return;
}

void do_nightsight( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_PROTEAN) && !IS_VAMPAFF(ch,VAM_OBTENEBRATION)
	&& !IS_VAMPAFF(ch,VAM_SERPENTIS))
    {
	send_to_char("You are not trained in the correct disciplines.\n\r",ch);
	return;
    }
    if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) )
    {
	send_to_char("The red glow in your eyes fades.\n\r",ch);
	act("The red glow in $n's eyes fades.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->vampaff, VAM_NIGHTSIGHT);
	return;
    }
    if ( ch->pcdata->condition[COND_THIRST] < 10 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(5,10);
    send_to_char("Your eyes start glowing red.\n\r",ch);
    act("$n's eyes start glowing red.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
    return;
}

void do_shadowsight( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_OBTENEBRATION) )
    {
	send_to_char("You are not trained in the Obtenebration discipline.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) )
    {
	send_to_char("You can no longer see between planes.\n\r",ch);
	REMOVE_BIT(ch->affected_by, AFF_SHADOWSIGHT);
	return;
    }
    if ( ch->pcdata->condition[COND_THIRST] < 10 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(5,10);
    send_to_char("You can now see between planes.\n\r",ch);
    SET_BIT(ch->affected_by, AFF_SHADOWSIGHT);
    return;
}

void do_bite( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    int       clancount;
    bool      can_sire = FALSE;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE) || (ch->vampgen < 1))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (ch->vamppass == -1) ch->vamppass = ch->vampaff;

    if (ch->vampgen == 1 || ch->vampgen == 2) can_sire = TRUE;
    if (IS_EXTRA(ch,EXTRA_PRINCE)) can_sire = TRUE;
    if (IS_EXTRA(ch,EXTRA_SIRE)) can_sire = TRUE;
    if (ch->vampgen > 6) can_sire = FALSE;

    if (!can_sire)
    {
	send_to_char("You are not able to create any childer.\n\r",ch);
	return;
    }

    if (!str_cmp(ch->clan,"") && (ch->vampgen != 1) )
    {
	send_to_char( "First you need to found a clan.\n\r", ch );
	return;
    }

    clancount = 0;
    if (IS_VAMPPASS(ch,VAM_PROTEAN))       clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_CELERITY))      clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_FORTITUDE))     clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_POTENCE))       clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_OBFUSCATE))     clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_OBTENEBRATION)) clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_SERPENTIS))     clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_AUSPEX))        clancount = clancount + 1;
    if (IS_VAMPPASS(ch,VAM_DOMINATE))      clancount = clancount + 1;

    if ( clancount < 3 )
    {
	send_to_char( "First you need to master 3 disciplines.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Bite whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot bite yourself.\n\r", ch );
	return;
    }

    if ( victim->level < 3 )
    {
	send_to_char( "You can only bite avatars.\n\r", ch );
	return;
    }

    if (IS_SET(victim->act, PLR_VAMPIRE) && ch->beast != 100)
    {
	send_to_char( "But they are already a vampire!\n\r", ch );
	return;
    }

    if (!IS_IMMUNE(victim,IMM_VAMPIRE) && ch->beast != 100)
    {
	send_to_char( "You cannot bite an unwilling person.\n\r", ch );
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_FANGS) && ch->beast != 100)
    {
	send_to_char("First you better get your fangs out!\n\r",ch);
	return;
    }

    if (IS_VAMPAFF(ch,VAM_DISGUISED) && ch->beast != 100)
    {
	send_to_char("You must reveal your true nature to bite someone.\n\r",ch);
	return;
    }

    if (ch->exp < 1000 && ch->beast != 100)
    {
	send_to_char("You cannot afford the 1000 exp to create a childe.\n\r",ch);
	return;
    }

    if (ch->beast == 100)
    {
    	if (!IS_VAMPAFF(ch,VAM_FANGS) ) do_fangs(ch,"");
    	act("Your jaw opens wide and you leap hungrily at $N.", ch, NULL, victim, TO_CHAR);
    	act("$n's jaw opens wide and $e leaps hungrily at $N.", ch, NULL, victim, TO_NOTVICT);
    	act("$n's jaw opens wide and $e leaps hungrily at you.", ch, NULL, victim, TO_VICT);
	one_hit( ch, victim, -1, 0 );
	return;
    }

    if (ch->beast > 0)
	ch->beast += 1;
    ch->exp = ch->exp - 1000;
    if (IS_EXTRA(ch,EXTRA_SIRE)) REMOVE_BIT(ch->extra,EXTRA_SIRE);
    act("You sink your teeth into $N.", ch, NULL, victim, TO_CHAR);
    act("$n sinks $s teeth into $N.", ch, NULL, victim, TO_NOTVICT);
    act("$n sinks $s teeth into your neck.", ch, NULL, victim, TO_VICT);
    SET_BIT(victim->act, PLR_VAMPIRE);
    if ( victim->vampgen != 0 && (victim->vampgen <= ( ch->vampgen + 1 ) ) )
    {
	send_to_char( "Your vampiric status has been restored.\n\r", victim );
	return;
    }
    send_to_char( "You are now a vampire.\n\r", victim );    
    victim->vampgen = ch->vampgen + 1;
    free_string(victim->lord);
    if (ch->vampgen == 1)
	victim->lord=str_dup(ch->name);
    else
    {
	sprintf(buf,"%s %s",ch->lord,ch->name);
	victim->lord=str_dup(buf);
    }
    if (ch->vampgen != 1)
    {
    	if (victim->vamppass == -1) victim->vamppass = victim->vampaff;

	/* Remove hp bonus from fortitude */
	if (IS_VAMPPASS(victim,VAM_FORTITUDE) && !IS_VAMPAFF(victim,VAM_FORTITUDE))
	{
	    victim->max_hit = victim->max_hit - 50;
	    victim->hit = victim->hit - 50;
	    if (victim->hit < 1) victim->hit = 1;
	}

	/* Remove any old powers they might have */
	if (IS_VAMPPASS(victim,VAM_PROTEAN))
	{REMOVE_BIT(victim->vamppass, VAM_PROTEAN);
	 REMOVE_BIT(victim->vampaff, VAM_PROTEAN);}
	if (IS_VAMPPASS(victim,VAM_CELERITY))
	{REMOVE_BIT(victim->vamppass, VAM_CELERITY);
	 REMOVE_BIT(victim->vampaff, VAM_CELERITY);}
	if (IS_VAMPPASS(victim,VAM_FORTITUDE))
	{REMOVE_BIT(victim->vamppass, VAM_FORTITUDE);
	 REMOVE_BIT(victim->vampaff, VAM_FORTITUDE);}
	if (IS_VAMPPASS(victim,VAM_POTENCE))
	{REMOVE_BIT(victim->vamppass, VAM_POTENCE);
	 REMOVE_BIT(victim->vampaff, VAM_POTENCE);}
	if (IS_VAMPPASS(victim,VAM_OBFUSCATE))
	{REMOVE_BIT(victim->vamppass, VAM_OBFUSCATE);
	 REMOVE_BIT(victim->vampaff, VAM_OBFUSCATE);}
	if (IS_VAMPPASS(victim,VAM_OBTENEBRATION))
	{REMOVE_BIT(victim->vamppass, VAM_OBTENEBRATION);
	 REMOVE_BIT(victim->vampaff, VAM_OBTENEBRATION);}
	if (IS_VAMPPASS(victim,VAM_SERPENTIS))
	{REMOVE_BIT(victim->vamppass, VAM_SERPENTIS);
	 REMOVE_BIT(victim->vampaff, VAM_SERPENTIS);}
	if (IS_VAMPPASS(victim,VAM_AUSPEX))
	{REMOVE_BIT(victim->vamppass, VAM_AUSPEX);
	 REMOVE_BIT(victim->vampaff, VAM_AUSPEX);}
	if (IS_VAMPPASS(victim,VAM_DOMINATE))
	{REMOVE_BIT(victim->vamppass, VAM_DOMINATE);
	 REMOVE_BIT(victim->vampaff, VAM_DOMINATE);}
	free_string(victim->clan);
	victim->clan=str_dup(ch->clan);
	/* Give the vampire the base powers of their sire */
	if (IS_VAMPPASS(ch,VAM_FORTITUDE) && !IS_VAMPAFF(victim,VAM_FORTITUDE))
	{
	    victim->max_hit = victim->max_hit + 50;
	    victim->hit = victim->hit + 50;
	}
	if (IS_VAMPPASS(ch,VAM_PROTEAN))
	{SET_BIT(victim->vamppass, VAM_PROTEAN);
	 SET_BIT(victim->vampaff, VAM_PROTEAN);}
	if (IS_VAMPPASS(ch,VAM_CELERITY))
	{SET_BIT(victim->vamppass, VAM_CELERITY);
	 SET_BIT(victim->vampaff, VAM_CELERITY);}
	if (IS_VAMPPASS(ch,VAM_FORTITUDE))
	{SET_BIT(victim->vamppass, VAM_FORTITUDE);
	 SET_BIT(victim->vampaff, VAM_FORTITUDE);}
	if (IS_VAMPPASS(ch,VAM_POTENCE))
	{SET_BIT(victim->vamppass, VAM_POTENCE);
	 SET_BIT(victim->vampaff, VAM_POTENCE);}
	if (IS_VAMPPASS(ch,VAM_OBFUSCATE))
	{SET_BIT(victim->vamppass, VAM_OBFUSCATE);
	 SET_BIT(victim->vampaff, VAM_OBFUSCATE);}
	if (IS_VAMPPASS(ch,VAM_OBTENEBRATION))
	{SET_BIT(victim->vamppass, VAM_OBTENEBRATION);
	 SET_BIT(victim->vampaff, VAM_OBTENEBRATION);}
	if (IS_VAMPPASS(ch,VAM_SERPENTIS))
	{SET_BIT(victim->vamppass, VAM_SERPENTIS);
	 SET_BIT(victim->vampaff, VAM_SERPENTIS);}
	if (IS_VAMPPASS(ch,VAM_AUSPEX))
	{SET_BIT(victim->vamppass, VAM_AUSPEX);
	 SET_BIT(victim->vampaff, VAM_AUSPEX);}
	if (IS_VAMPPASS(ch,VAM_DOMINATE))
	{SET_BIT(victim->vamppass, VAM_DOMINATE);
	 SET_BIT(victim->vampaff, VAM_DOMINATE);}
    }
    return;
}

void do_clanname( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );
    if ( IS_NPC(ch) ) return;

    if ( !IS_SET(ch->act,PLR_VAMPIRE) || ch->vampgen != 1 )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Who's clan do you wish to name?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) ) return;
    if ( victim->vampgen != 2 )
    {
	send_to_char( "Only the Antediluvians may have clans.\n\r", ch );
	return;
    }
    if ( str_cmp(victim->clan,"") )
    {
	send_to_char( "But they already have a clan!\n\r", ch );
	return;
    }
    smash_tilde( argument );
    free_string( victim->clan );
    victim->clan = str_dup( argument );
    send_to_char( "Clan name set.\n\r", ch );
    return;
}

void do_stake( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *stake;
    char      arg [MAX_INPUT_LENGTH];
    int       blood;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Stake whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    stake = get_eq_char( ch, WEAR_HOLD );
    if ( (stake == NULL) || (stake->item_type != ITEM_STAKE) )
    {
	send_to_char( "How can you stake someone down without holding a stake?\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You cannot bite yourself.\n\r", ch );
	return;
    }

    if (!IS_SET(victim->act, PLR_VAMPIRE))
    {
	send_to_char( "You can only stake vampires.\n\r", ch );
	return;
    }

    if (victim->position > POS_MORTAL)
    {
	send_to_char( "You can only stake down a vampire who is mortally wounded.\n\r", ch );
	return;
    }

    act("You plunge $p into $N's heart.", ch, stake, victim, TO_CHAR);
    act("$n plunges $p into $N's heart.", ch, stake, victim, TO_NOTVICT);
    send_to_char( "You feel a stake plunged through your heart.\n\r", victim );
    if (IS_IMMUNE(victim,IMM_STAKE)) return;

    /* Have to make sure they have enough blood to change back */
    blood = victim->pcdata->condition[COND_THIRST];
    victim->pcdata->condition[COND_THIRST] = 666;

    /* To take care of vampires who have powers in affect. */
    if (IS_VAMPAFF(victim,VAM_DISGUISED) ) do_mask(victim,"self");
    if (IS_IMMUNE(victim,IMM_SHIELDED) ) do_shield(victim,"");
    if (IS_AFFECTED(victim,AFF_SHADOWPLANE) ) do_shadowplane(victim,"");
    if (IS_VAMPAFF(victim,VAM_FANGS) ) do_fangs(victim,"");
    if (IS_VAMPAFF(victim,VAM_CLAWS) ) do_claws(victim,"");
    if (IS_VAMPAFF(victim,VAM_NIGHTSIGHT) ) do_nightsight(victim,"");
    if (IS_AFFECTED(victim,AFF_SHADOWSIGHT) ) do_shadowsight(victim,"");
    if (IS_SET(victim->act,PLR_HOLYLIGHT) ) do_truesight(victim,"");
    if (IS_VAMPAFF(victim,VAM_CHANGED) ) do_change(victim,"human");
    if (IS_POLYAFF(victim,POLY_SERPENT) ) do_serpent(victim,"");
    victim->pcdata->wolf = 0;
    victim->pcdata->condition[COND_THIRST] = blood;

    REMOVE_BIT(victim->act, PLR_VAMPIRE);
    obj_from_char(stake);
    obj_to_char(stake,victim);
    ch->exp = ch->exp + 1000;
    victim->home = 3001;
    return;
}

void do_mask( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_OBFUSCATE) )
    {
	send_to_char("You are not trained in the Obfuscate discipline.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Change to look like whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) && victim != ch )
    {
	send_to_char( "You can only mask avatars or lower.\n\r", ch );
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 40 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(30,40);

    if ( ch == victim )
    {
	if (!IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
	{
	    send_to_char( "You already look like yourself!\n\r", ch );
	    return;
	}
	sprintf(buf,"Your form shimmers and transforms into %s.",ch->name);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"%s's form shimmers and transforms into %s.",ch->morph,ch->name);
	act(buf,ch,NULL,victim,TO_ROOM);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
    	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	return;
    }
    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
    	sprintf(buf,"Your form shimmers and transforms into a clone of %s.",victim->name);
    	act(buf,ch,NULL,victim,TO_CHAR);
    	sprintf(buf,"%s's form shimmers and transforms into a clone of %s.",ch->morph,victim->name);
    	act(buf,ch,NULL,victim,TO_NOTVICT);
    	sprintf(buf,"%s's form shimmers and transforms into a clone of you!",ch->morph);
    	act(buf,ch,NULL,victim,TO_VICT);
    	free_string( ch->morph );
    	ch->morph = str_dup( victim->name );
	return;
    }
    sprintf(buf,"Your form shimmers and transforms into a clone of %s.",victim->name);
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf,"%s's form shimmers and transforms into a clone of %s.",ch->name,victim->name);
    act(buf,ch,NULL,victim,TO_NOTVICT);
    sprintf(buf,"%s's form shimmers and transforms into a clone of you!",ch->name);
    act(buf,ch,NULL,victim,TO_VICT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    free_string( ch->morph );
    ch->morph = str_dup( victim->name );
    return;
}

void do_change( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH]; 
    char buf [MAX_INPUT_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_PROTEAN) )
    {
	send_to_char("You are not trained in the Protean discipline.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "You can change between 'human', 'bat', 'wolf' and 'mist' forms.\n\r", ch );
	return;
    }

    if ( !str_cmp(arg,"bat") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}
    	if ( ch->pcdata->condition[COND_THIRST] < 50 )
    	{
	    send_to_char("You have insufficient blood.\n\r",ch);
	    return;
    	}
    	ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
	clear_stats(ch);
	act( "You transform into bat form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a bat.", ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->vampaff, VAM_FLYING);
	SET_BIT(ch->vampaff, VAM_SONIC);
	SET_BIT(ch->polyaff, POLY_BAT);
	SET_BIT(ch->vampaff, VAM_CHANGED);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	sprintf(buf, "%s the vampire bat", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"wolf") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}
    	if ( ch->pcdata->condition[COND_THIRST] < 50 )
    	{
	    send_to_char("You have insufficient blood.\n\r",ch);
	    return;
    	}
    	ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
	act( "You transform into wolf form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a dire wolf.", ch, NULL, NULL, TO_ROOM );
	clear_stats(ch);
    	if (ch->wpn[0] > 0)
    	{
	    ch->hitroll += ch->wpn[0]/4;
	    ch->damroll += ch->wpn[0]/4;
	    ch->armor   -= ch->wpn[0];
    	}
    	ch->pcdata->mod_str = 10;
	SET_BIT(ch->polyaff, POLY_WOLF);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->vampaff, VAM_CHANGED);
	ch->max_hit = ch->max_hit + 50;
	ch->hit = ch->hit + 50;
	sprintf(buf, "%s the dire wolf", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"mist") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}
    	if ( ch->pcdata->condition[COND_THIRST] < 50 )
    	{
	    send_to_char("You have insufficient blood.\n\r",ch);
	    return;
    	}
    	ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
	act( "You transform into mist form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a white mist.", ch, NULL, NULL, TO_ROOM );
	clear_stats(ch);
	SET_BIT(ch->polyaff, POLY_MIST);
	SET_BIT(ch->vampaff, VAM_CHANGED);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->affected_by, AFF_ETHEREAL);
	sprintf(buf, "%s the white mist", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"human") )
    {
	if (!IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You are already in human form.\n\r", ch );
	    return;
	}
	if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_BAT))
	{
	    REMOVE_BIT(ch->vampaff, VAM_FLYING);
	    REMOVE_BIT(ch->vampaff, VAM_SONIC);
	    REMOVE_BIT(ch->polyaff, POLY_BAT);
	}
	else if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_WOLF))
	{
	    REMOVE_BIT(ch->polyaff, POLY_WOLF);
	    ch->max_hit = ch->max_hit - 50;
	    ch->hit = ch->hit - 50;
	    if (ch->hit < 1) ch->hit = 1;
	}
	else if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_MIST))
	{
	    REMOVE_BIT(ch->polyaff, POLY_MIST);
	    REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);
	}
	else
	{
	    /* In case they try to change to human from a non-vamp form */
	    send_to_char( "You seem to be stuck in this form.\n\r", ch );
	    return;
	}
	act( "You transform into human form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into human form.", ch, NULL, NULL, TO_ROOM );
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_CHANGED);
	clear_stats(ch);
  	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	return;
    }
    else
	send_to_char( "You can change between 'human', 'bat', 'wolf' and 'mist' forms.\n\r", ch );
    return;
}

void clear_stats( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if (IS_NPC(ch)) return;

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->wear_loc != WEAR_NONE ) unequip_char(ch, obj);
    }
    while ( ch->affected )
	affect_remove( ch, ch->affected );
    ch->armor			= 100;
    ch->hitroll			= 0;
    ch->damroll			= 0;
    ch->saving_throw		= 0;
    ch->pcdata->mod_str		= 0;
    ch->pcdata->mod_int		= 0;
    ch->pcdata->mod_wis		= 0;
    ch->pcdata->mod_dex		= 0;
    ch->pcdata->mod_con		= 0;
    save_char_obj( ch );
    return;
}


void do_clandisc( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    int clancount;
    int clanmax;
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( !IS_SET(ch->act,PLR_VAMPIRE) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (ch->vamppass == -1) ch->vamppass = ch->vampaff;

    if      (ch->vampgen == 1) clanmax = 8;
    else if (ch->vampgen == 2) clanmax = 6;
    else if (ch->vampgen <= 4) clanmax = 5;
    else                       clanmax = 4;

    clancount = 0;
    if (IS_VAMPAFF(ch,VAM_PROTEAN) || IS_VAMPPASS(ch,VAM_PROTEAN))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_CELERITY) || IS_VAMPPASS(ch,VAM_CELERITY))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_FORTITUDE) || IS_VAMPPASS(ch,VAM_FORTITUDE))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_POTENCE) || IS_VAMPPASS(ch,VAM_POTENCE))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_OBFUSCATE) || IS_VAMPPASS(ch,VAM_OBFUSCATE))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_OBTENEBRATION) || IS_VAMPPASS(ch,VAM_OBTENEBRATION))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_SERPENTIS) || IS_VAMPPASS(ch,VAM_SERPENTIS))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_AUSPEX) || IS_VAMPPASS(ch,VAM_AUSPEX))
	clancount = clancount + 1;
    if (IS_VAMPAFF(ch,VAM_DOMINATE) || IS_VAMPPASS(ch,VAM_DOMINATE))
	clancount = clancount + 1;
    if ( arg[0] == '\0' )
    {
	send_to_char("Current powers:",ch);
	if (IS_VAMPAFF(ch,VAM_PROTEAN) && !IS_VAMPPASS(ch,VAM_PROTEAN))
	    send_to_char(" Protean",ch);
	else if (IS_VAMPAFF(ch,VAM_PROTEAN))
	    send_to_char(" PROTEAN",ch);
	if (IS_VAMPAFF(ch,VAM_CELERITY) && !IS_VAMPPASS(ch,VAM_CELERITY))
	    send_to_char(" Celerity",ch);
	else if (IS_VAMPAFF(ch,VAM_CELERITY))
	    send_to_char(" CELERITY",ch);
	if (IS_VAMPAFF(ch,VAM_FORTITUDE) && !IS_VAMPPASS(ch,VAM_FORTITUDE))
	    send_to_char(" Fortitude",ch);
	else if (IS_VAMPAFF(ch,VAM_FORTITUDE))
	    send_to_char(" FORTITUDE",ch);
	if (IS_VAMPAFF(ch,VAM_POTENCE) && !IS_VAMPPASS(ch,VAM_POTENCE))
	    send_to_char(" Potence",ch);
	else if (IS_VAMPAFF(ch,VAM_POTENCE))
	    send_to_char(" POTENCE",ch);
	if (IS_VAMPAFF(ch,VAM_OBFUSCATE) && !IS_VAMPPASS(ch,VAM_OBFUSCATE))
	    send_to_char(" Obfuscate",ch);
	else if (IS_VAMPAFF(ch,VAM_OBFUSCATE))
	    send_to_char(" OBFUSCATE",ch);
	if (IS_VAMPAFF(ch,VAM_OBTENEBRATION) && !IS_VAMPPASS(ch,VAM_OBTENEBRATION))
	    send_to_char(" Obtenebration",ch);
	else if (IS_VAMPAFF(ch,VAM_OBTENEBRATION))
	    send_to_char(" OBTENEBRATION",ch);
	if (IS_VAMPAFF(ch,VAM_SERPENTIS) && !IS_VAMPPASS(ch,VAM_SERPENTIS))
	    send_to_char(" Serpentis",ch);
	else if (IS_VAMPAFF(ch,VAM_SERPENTIS))
	    send_to_char(" SERPENTIS",ch);
	if (IS_VAMPAFF(ch,VAM_AUSPEX) && !IS_VAMPPASS(ch,VAM_AUSPEX))
	    send_to_char(" Auspex",ch);
	else if (IS_VAMPAFF(ch,VAM_AUSPEX))
	    send_to_char(" AUSPEX",ch);
	if (IS_VAMPAFF(ch,VAM_DOMINATE) && !IS_VAMPPASS(ch,VAM_DOMINATE))
	    send_to_char(" Dominate",ch);
	else if (IS_VAMPAFF(ch,VAM_DOMINATE))
	    send_to_char(" DOMINATE",ch);
    	if (!IS_VAMPAFF(ch,VAM_PROTEAN) && !IS_VAMPAFF(ch,VAM_CELERITY) && 
	    !IS_VAMPAFF(ch,VAM_FORTITUDE) && !IS_VAMPAFF(ch,VAM_POTENCE) &&
	    !IS_VAMPAFF(ch,VAM_OBFUSCATE) && !IS_VAMPAFF(ch,VAM_AUSPEX) &&
	    !IS_VAMPAFF(ch,VAM_OBTENEBRATION) && !IS_VAMPAFF(ch,VAM_SERPENTIS) &&
    	    !IS_VAMPPASS(ch,VAM_PROTEAN) && !IS_VAMPPASS(ch,VAM_CELERITY) && 
	    !IS_VAMPPASS(ch,VAM_FORTITUDE) && !IS_VAMPPASS(ch,VAM_POTENCE) &&
	    !IS_VAMPPASS(ch,VAM_OBFUSCATE) && !IS_VAMPPASS(ch,VAM_AUSPEX) &&
	    !IS_VAMPPASS(ch,VAM_DOMINATE) &&
	    !IS_VAMPPASS(ch,VAM_OBTENEBRATION) && !IS_VAMPPASS(ch,VAM_SERPENTIS))
	    send_to_char(" None",ch);
	send_to_char(".\n\r",ch);
	if (clancount < clanmax) {sprintf(buf,"Select %d from:",(clanmax-clancount));
	    send_to_char(buf,ch);}
	else return;
	if (!IS_VAMPAFF(ch,VAM_PROTEAN)) send_to_char(" Protean",ch);
	if (!IS_VAMPAFF(ch,VAM_CELERITY)) send_to_char(" Celerity",ch);
	if (!IS_VAMPAFF(ch,VAM_FORTITUDE)) send_to_char(" Fortitude",ch);
	if (!IS_VAMPAFF(ch,VAM_POTENCE)) send_to_char(" Potence",ch);
	if (!IS_VAMPAFF(ch,VAM_OBFUSCATE)) send_to_char(" Obfuscate",ch);
	if (!IS_VAMPAFF(ch,VAM_OBTENEBRATION)) send_to_char(" Obtenebration",ch);
	if (!IS_VAMPAFF(ch,VAM_SERPENTIS)) send_to_char(" Serpentis",ch);
	if (!IS_VAMPAFF(ch,VAM_AUSPEX)) send_to_char(" Auspex",ch);
	if (!IS_VAMPAFF(ch,VAM_DOMINATE)) send_to_char(" Dominate",ch);
	send_to_char(".\n\r",ch);
	return;
    }
    if ( clancount >= clanmax )
    {
	if ( !str_cmp(arg,"protean") && (IS_VAMPAFF(ch,VAM_PROTEAN) || IS_VAMPPASS(ch,VAM_PROTEAN)))
	    send_to_char( "Powers: Nightsight, Claws, Change.\n\r", ch );
	else if ( !str_cmp(arg,"celerity") && (IS_VAMPAFF(ch,VAM_CELERITY) || IS_VAMPPASS(ch,VAM_CELERITY)) )
	    send_to_char( "Powers: 66% extra attack, 33% extra attack.\n\r", ch );
	else if ( !str_cmp(arg,"fortitude") && (IS_VAMPAFF(ch,VAM_FORTITUDE) || IS_VAMPPASS(ch,VAM_FORTITUDE)) )
	    send_to_char( "Powers: 1-100% damage reduction, +50 one time hp bonus.\n\r", ch );
	else if ( !str_cmp(arg,"potence") && (IS_VAMPAFF(ch,VAM_POTENCE) || IS_VAMPPASS(ch,VAM_POTENCE)) )
	    send_to_char( "Powers: 150% normal damage in combat.\n\r", ch );
	else if ( !str_cmp(arg,"obfuscate") && (IS_VAMPAFF(ch,VAM_OBFUSCATE) || IS_VAMPPASS(ch,VAM_OBFUSCATE)) )
	    send_to_char( "Powers: Mask, Mortal, Shield.\n\r", ch );
	else if ( !str_cmp(arg,"obtenebration") && (IS_VAMPAFF(ch,VAM_OBTENEBRATION) || IS_VAMPPASS(ch,VAM_OBTENEBRATION)) )
	    send_to_char( "Powers: Shadowplane, Shadowsight, Nightsight.\n\r", ch );
	else if ( !str_cmp(arg,"serpentis") && (IS_VAMPAFF(ch,VAM_SERPENTIS) || IS_VAMPPASS(ch,VAM_SERPENTIS)) )
	    send_to_char( "Powers: Darkheart, Serpent, Poison, Nightsight.\n\r", ch );
	else if ( !str_cmp(arg,"auspex") && (IS_VAMPAFF(ch,VAM_AUSPEX) || IS_VAMPPASS(ch,VAM_AUSPEX)) )
	    send_to_char( "Powers: Truesight, Scry, Readaura.\n\r", ch );
	else if ( !str_cmp(arg,"dominate") && (IS_VAMPAFF(ch,VAM_DOMINATE) || IS_VAMPPASS(ch,VAM_DOMINATE)) )
	    send_to_char( "Powers: Evileye, Command, Shield.\n\r", ch );
	else
	    send_to_char( "You don't know any such Discipline.\n\r", ch );
	return;
    }
    if ( !str_cmp(arg,"protean") )
    {
	if (IS_VAMPAFF(ch,VAM_PROTEAN) || IS_VAMPPASS(ch,VAM_PROTEAN))
	{
	    send_to_char( "Powers: Nightsight, Claws, Change.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Protean.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->vamppass,VAM_PROTEAN);
	SET_BIT(ch->vampaff,VAM_PROTEAN);
	return;
    }
    else if ( !str_cmp(arg,"celerity") )
    {
	if (IS_VAMPAFF(ch,VAM_CELERITY) || IS_VAMPPASS(ch,VAM_CELERITY))
	{
	    send_to_char( "Powers: 66% extra attack, 33% extra attack.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Celerity.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->vamppass,VAM_CELERITY);
	SET_BIT(ch->vampaff,VAM_CELERITY);
	return;
    }
    else if ( !str_cmp(arg,"fortitude") )
    {
	if (IS_VAMPAFF(ch,VAM_FORTITUDE) || IS_VAMPPASS(ch,VAM_FORTITUDE))
	{
	    send_to_char( "Powers: 1-100% damage reduction, +50 one time hp bonus.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Fortitude.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->vamppass,VAM_FORTITUDE);
	SET_BIT(ch->vampaff,VAM_FORTITUDE);
	ch->max_hit = ch->max_hit + 50;
	ch->hit = ch->hit + 50;
	return;
    }
    else if ( !str_cmp(arg,"potence") )
    {
	if (IS_VAMPAFF(ch,VAM_POTENCE) || IS_VAMPPASS(ch,VAM_POTENCE))
	{
	    send_to_char( "Powers: 150% normal damage in combat.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Potence.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->vamppass,VAM_POTENCE);
	SET_BIT(ch->vampaff,VAM_POTENCE);
	return;
    }
    else if ( !str_cmp(arg,"obfuscate") )
    {
	if (IS_VAMPAFF(ch,VAM_OBFUSCATE) || IS_VAMPPASS(ch,VAM_OBFUSCATE))
	{
	    send_to_char( "Powers: Mask, Mortal, Shield.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Obfuscate.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->vamppass,VAM_OBFUSCATE);
	SET_BIT(ch->vampaff,VAM_OBFUSCATE);
	return;
    }
    else if ( !str_cmp(arg,"obtenebration") )
    {
	if (IS_VAMPAFF(ch,VAM_OBTENEBRATION) || IS_VAMPPASS(ch,VAM_OBTENEBRATION))
	{
	    send_to_char( "Powers: Shadowplane, Shadowsight, Nightsight.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Obtenebration.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->vamppass,VAM_OBTENEBRATION);
	SET_BIT(ch->vampaff,VAM_OBTENEBRATION);
	return;
    }
    else if ( !str_cmp(arg,"serpentis") )
    {
	if (IS_VAMPAFF(ch,VAM_SERPENTIS) || IS_VAMPPASS(ch,VAM_SERPENTIS))
	{
	    send_to_char( "Powers: Darkheart, Serpent, Poison, Nightsight.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Serpentis.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->vamppass,VAM_SERPENTIS);
	SET_BIT(ch->vampaff,VAM_SERPENTIS);
	return;
    }
    else if ( !str_cmp(arg,"auspex") )
    {
	if (IS_VAMPAFF(ch,VAM_AUSPEX) || IS_VAMPPASS(ch,VAM_AUSPEX))
	{
	    send_to_char( "Powers: Truesight, Scry, Readaura.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Auspex.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->vamppass,VAM_AUSPEX);
	SET_BIT(ch->vampaff,VAM_AUSPEX);
	return;
    }
    else if ( !str_cmp(arg,"dominate") )
    {
	if (IS_VAMPAFF(ch,VAM_DOMINATE) || IS_VAMPPASS(ch,VAM_DOMINATE))
	{
	    send_to_char( "Powers: Evileye, Command, Shield.\n\r", ch );
	    return;
	}
	send_to_char( "You master the discipline of Dominate.\n\r", ch );
	if (clancount < 3) SET_BIT(ch->vamppass,VAM_DOMINATE);
	SET_BIT(ch->vampaff,VAM_DOMINATE);
	return;
    }
    else
	send_to_char( "No such discipline.\n\r", ch );
    return;
}

void do_vampire( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_IMMUNE(ch, IMM_VAMPIRE))
    {
	send_to_char("You will now allow vampires to bite you.\n\r",ch);
	SET_BIT(ch->immune, IMM_VAMPIRE);
	return;
    }
    send_to_char("You will no longer allow vampires to bite you.\n\r",ch);
    REMOVE_BIT(ch->immune, IMM_VAMPIRE);
    return;
}

void do_shadowplane( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_OBTENEBRATION) )
    {
	send_to_char("You are not trained in the Obtenebration discipline.\n\r",ch);
	return;
    }
    if ( ch->pcdata->condition[COND_THIRST] < 75 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(65,75);
    if ( arg[0] == '\0' )
    {
    	if (!IS_AFFECTED(ch, AFF_SHADOWPLANE))
    	{
	    send_to_char("You fade into the plane of shadows.\n\r",ch);
	    act("The shadows flicker and swallow up $n.",ch,NULL,NULL,TO_ROOM);
	    SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    do_look(ch,"auto");
	    return;
    	}
    	REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
    	send_to_char("You fade back into the real world.\n\r",ch);
	act("The shadows flicker and $n fades into existance.",ch,NULL,NULL,TO_ROOM);
	do_look(ch,"auto");
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char( "What do you wish to toss into the shadow plane?\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch, AFF_SHADOWPLANE))
    	send_to_char( "You toss it to the ground and it vanishes.\n\r", ch );
    else
    	send_to_char( "You toss it into a shadow and it vanishes.\n\r", ch );
    return;
}

void do_immune( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    send_to_char("                                -= Immunities =-\n\r",ch);
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    /* Display weapon resistances */
    send_to_char("Weapons:",ch);
    if (!(!IS_IMMUNE(ch,IMM_SLASH) && !IS_IMMUNE(ch,IMM_STAB) && 
	  !IS_IMMUNE(ch,IMM_SMASH) && !IS_IMMUNE(ch,IMM_ANIMAL) &&
	  !IS_IMMUNE(ch,IMM_MISC )) )
    {
    	if (IS_IMMUNE(ch,IMM_SLASH )) send_to_char(" Slash Slice",ch);
    	if (IS_IMMUNE(ch,IMM_STAB  )) send_to_char(" Stab Pierce",ch);
    	if (IS_IMMUNE(ch,IMM_SMASH )) send_to_char(" Blast Crush Pound",ch);
    	if (IS_IMMUNE(ch,IMM_ANIMAL)) send_to_char(" Claw Bite",ch);
    	if (IS_IMMUNE(ch,IMM_MISC  )) send_to_char(" Grep Whip Suck",ch);
    }
    else
	send_to_char(" None",ch);
    send_to_char(".\n\r",ch);

    /* Display spell immunities */
    send_to_char("Spells :",ch);
    if (!(!IS_IMMUNE(ch,IMM_CHARM)  && !IS_IMMUNE(ch,IMM_HEAT)      && 
	  !IS_IMMUNE(ch,IMM_COLD)   && !IS_IMMUNE(ch,IMM_LIGHTNING) &&
	  !IS_IMMUNE(ch,IMM_ACID)   && !IS_IMMUNE(ch,IMM_SUMMON)    &&
	  !IS_IMMUNE(ch,IMM_VOODOO) && !IS_IMMUNE(ch,IMM_SLEEP)    &&
	  !IS_IMMUNE(ch,IMM_DRAIN)))
    {
    	if (IS_IMMUNE(ch,IMM_CHARM     )) send_to_char(" Charm",ch);
    	if (IS_IMMUNE(ch,IMM_HEAT      )) send_to_char(" Heat",ch);
    	if (IS_IMMUNE(ch,IMM_COLD      )) send_to_char(" Cold",ch);
    	if (IS_IMMUNE(ch,IMM_LIGHTNING )) send_to_char(" Lightning",ch);
    	if (IS_IMMUNE(ch,IMM_ACID      )) send_to_char(" Acid",ch);
    	if (!IS_IMMUNE(ch,IMM_SUMMON   )) send_to_char(" Summon",ch);
    	if (IS_IMMUNE(ch,IMM_VOODOO    )) send_to_char(" Voodoo",ch);
    	if (IS_IMMUNE(ch,IMM_SLEEP     )) send_to_char(" Sleep",ch);
    	if (IS_IMMUNE(ch,IMM_DRAIN     )) send_to_char(" Drain",ch);
    }
    else
	send_to_char(" None",ch);
    send_to_char(".\n\r",ch);

    /* Display skill immunities */
    send_to_char("Skills :",ch);
    if (!(!IS_IMMUNE(ch,IMM_HURL) && !IS_IMMUNE(ch,IMM_BACKSTAB) && 
	  !IS_IMMUNE(ch,IMM_KICK) && !IS_IMMUNE(ch,IMM_DISARM)   &&
	  !IS_IMMUNE(ch,IMM_STEAL)))
    {
    	if (IS_IMMUNE(ch,IMM_HURL      )) send_to_char(" Hurl",ch);
    	if (IS_IMMUNE(ch,IMM_BACKSTAB  )) send_to_char(" Backstab",ch);
    	if (IS_IMMUNE(ch,IMM_KICK      )) send_to_char(" Kick",ch);
    	if (IS_IMMUNE(ch,IMM_DISARM    )) send_to_char(" Disarm",ch);
    	if (IS_IMMUNE(ch,IMM_STEAL     )) send_to_char(" Steal",ch);
    }
    else
	send_to_char(" None",ch);
    send_to_char(".\n\r",ch);

    /* Display vampire immunities */
    send_to_char("Vampire:",ch);
    if (!( IS_IMMUNE(ch,IMM_VAMPIRE) && !IS_IMMUNE(ch,IMM_STAKE) && 
	  !IS_IMMUNE(ch,IMM_SUNLIGHT)))
    {
    	if (!IS_IMMUNE(ch,IMM_VAMPIRE  )) send_to_char(" Bite",ch);
    	if  (IS_IMMUNE(ch,IMM_STAKE    )) send_to_char(" Stake",ch);
    	if  (IS_IMMUNE(ch,IMM_SUNLIGHT )) send_to_char(" Sunlight",ch);
    }
    else
	send_to_char(" None",ch);
    send_to_char(".\n\r",ch);
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    return;
}

void do_introduce( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE) || (ch->vampgen < 1) || (ch->vampgen > 7))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    do_tradition(ch,ch->lord);
    return;
}

void do_tradition( CHAR_DATA *ch, char *argument )
{
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    char      arg3 [MAX_INPUT_LENGTH];
    char      arg4 [MAX_INPUT_LENGTH];
    char      arg5 [MAX_INPUT_LENGTH];
    char      arg6 [MAX_INPUT_LENGTH];
    char      buf  [MAX_INPUT_LENGTH];
    char      buf2 [MAX_INPUT_LENGTH];
    char      buf3 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    argument = one_argument( argument, arg5 );
    argument = one_argument( argument, arg6 );

    if (IS_NPC(ch)) return;

    if (arg1 != '\0') arg1[0] = UPPER(arg1[0]);
    if (arg2 != '\0') arg2[0] = UPPER(arg2[0]);
    if (arg3 != '\0') arg3[0] = UPPER(arg3[0]);
    if (arg4 != '\0') arg4[0] = UPPER(arg4[0]);
    if (arg5 != '\0') arg5[0] = UPPER(arg5[0]);
    if (arg6 != '\0') arg6[0] = UPPER(arg6[0]);

    if (!str_cmp(arg1,"kavir")) strcpy(arg1,"KaVir");

    if (!IS_SET(ch->act, PLR_VAMPIRE) || (ch->vampgen < 1) || (ch->vampgen > 7))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if ( !str_cmp(ch->clan,"") && ch->vampgen != 1 )
    {
	send_to_char( "Not until you've created your own clan!\n\r", ch );
	return;
    }

    if      (ch->vampgen == 7) {sprintf(buf2,"Seventh");sprintf(buf3,"Fledgling");}
    else if (ch->vampgen == 6) {sprintf(buf2,"Sixth");sprintf(buf3,"Neonate");}
    else if (ch->vampgen == 5) {sprintf(buf2,"Fifth");sprintf(buf3,"Ancilla");}
    else if (ch->vampgen == 4) {sprintf(buf2,"Fourth");sprintf(buf3,"Elder");}
    else if (ch->vampgen == 3) {sprintf(buf2,"Third");sprintf(buf3,"Methuselah");}
    else if (ch->vampgen == 2) {sprintf(buf2,"Second");sprintf(buf3,"Antediluvian");}
    if (ch->vampgen == 1)
	sprintf(buf,"As is the tradition, I recite the lineage of %s, Sire of all Kindred.",ch->name);
    else
	sprintf(buf,"As is the tradition, I recite the lineage of %s, %s of the %s Generation.",ch->name,buf3,buf2);
    do_say(ch,buf);
    if ( ch->vampgen != 1 )
    {
	if      (ch->vampgen == 7) sprintf(buf2,"%s",arg6);
	else if (ch->vampgen == 6) sprintf(buf2,"%s",arg5);
	else if (ch->vampgen == 5) sprintf(buf2,"%s",arg4);
	else if (ch->vampgen == 4) sprintf(buf2,"%s",arg3);
	else if (ch->vampgen == 3) sprintf(buf2,"%s",arg2);
	else if (ch->vampgen == 2) sprintf(buf2,"%s",arg1);

	if (ch->vampgen == 2)
	    sprintf(buf,"I am %s, founder of clan %s.  My sire is %s.", ch->name,ch->clan,buf2);
	else
	    sprintf(buf,"I am %s of clan %s.  My sire is %s.", ch->name,ch->clan,buf2);
	do_say(ch,buf);
    }
    if ( arg6[0] != '\0' )
    {
	sprintf(buf,"I am %s of clan %s.  My sire is %s.",
	arg6,ch->clan,arg5);
	do_say(ch,buf);
    }
    if ( arg5[0] != '\0' )
    {
	sprintf(buf,"I am %s of clan %s.  My sire is %s.",
	arg5,ch->clan,arg4);
	do_say(ch,buf);
    }
    if ( arg4[0] != '\0' )
    {
	sprintf(buf,"I am %s of clan %s.  My sire is %s.",
	arg4,ch->clan,arg3);
	do_say(ch,buf);
    }
    if ( arg3[0] != '\0' )
    {
	sprintf(buf,"I am %s of clan %s.  My sire is %s.",
	arg3,ch->clan,arg2);
	do_say(ch,buf);
    }
    if ( arg2[0] != '\0' )
    {
	if ( arg1[0] != '\0' )
	    sprintf(buf,"I am %s, founder of %s.  My sire is %s.",arg2,ch->clan,arg1);
	else
	    sprintf(buf,"I am %s of clan %s.  My sire is %s.",arg2,ch->clan,arg1);
	do_say(ch,buf);
    }
    if ( ch->vampgen == 1 )
	sprintf(buf,"I am %s.  All Kindred are my childer.",ch->name);
    else
	sprintf(buf,"I am %s.  All Kindred are my childer.",arg1);
    do_say(ch,buf);
    if ( ch->vampgen == 7 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg6,arg5,arg4,arg3,arg2,arg1);
    if ( ch->vampgen == 6 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg5,arg4,arg3,arg2,arg1);
    if ( ch->vampgen == 5 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg4,arg3,arg2,arg1);
    if ( ch->vampgen == 4 ) sprintf(buf,"My name is %s, childe of %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg3,arg2,arg1);
    if ( ch->vampgen == 3 ) sprintf(buf,"My name is %s, childe of %s, childe of %s.  Recognize my lineage.",ch->name,arg2,arg1);
    if ( ch->vampgen == 2 ) sprintf(buf,"My name is %s, childe of %s.  Recognize my lineage.",ch->name,arg1);
    if ( ch->vampgen == 1 ) sprintf(buf,"My name is %s.  Recognize my lineage.",ch->name);
    do_say(ch,buf);
    return;
}

void do_darkheart( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_SERPENTIS) )
    {
	send_to_char("You are not trained in the Serpentis discipline.\n\r",ch);
	return;
    }
    if (IS_IMMUNE(ch,IMM_STAKE) )
    {
	send_to_char("But you've already torn your heart out!\n\r",ch);
	return;
    }
    if ( ch->pcdata->condition[COND_THIRST] < 100 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= 100;
    send_to_char("You rip your heart from your body and toss it to the ground.\n\r",ch);
    act("$n rips $s heart out and tosses it to the ground.", ch, NULL, NULL, TO_ROOM);
    make_part( ch, "heart" );
    ch->hit = ch->hit - number_range(10,20);
    update_pos(ch);
    if (ch->position == POS_DEAD && !IS_HERO(ch))
    {
	send_to_char( "You have been KILLED!!\n\r\n\r", ch );
	raw_kill(ch);
	return;
    }
    SET_BIT(ch->immune, IMM_STAKE);
    return;
}

void do_truesight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if (!IS_SET(ch->act, PLR_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_AUSPEX) )
    {
	send_to_char("You are not trained in the Auspex discipline.\n\r",ch);
	return;
    }

    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Your senses return to normal.\n\r", ch );
    }
    else
    {
	SET_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Your senses increase to incredable proportions.\n\r", ch );
    }

    return;
}

void do_scry( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *chroom;
    ROOM_INDEX_DATA *victimroom;
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_AUSPEX) )
    {
	send_to_char("You are not trained in the Auspex discipline.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Scry on whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 25 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(15,25);

    if (!IS_NPC(victim) && IS_IMMUNE(victim,IMM_SHIELDED) )
    {
	send_to_char("You are unable to locate them.\n\r",ch);
	return;
    }

    chroom = ch->in_room;
    victimroom = victim->in_room;

    char_from_room(ch);
    char_to_room(ch,victimroom);
    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) && (!IS_AFFECTED(victim, AFF_SHADOWPLANE)))
    {
	REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
	do_look(ch,"auto");
	SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
    }
    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) && (IS_AFFECTED(victim, AFF_SHADOWPLANE)))
    {
	SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
	do_look(ch,"auto");
	REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
    }
    else
	do_look(ch,"auto");
    char_from_room(ch);
    char_to_room(ch,chroom);

    return;
}

void do_readaura( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    int       hr;
    int       dr;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_AUSPEX) )
    {
	send_to_char("You are not trained in the Auspex discipline.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Read the aura on what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "Read the aura on what?\n\r", ch );
	    return;
	}
    	if ( ch->pcdata->condition[COND_THIRST] < 50 )
    	{
	    send_to_char("You have insufficient blood.\n\r",ch);
	    return;
    	}
    	ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
	act("$n examines $p intently.",ch,obj,NULL,TO_ROOM);
	spell_identify( skill_lookup( "identify" ), ch->level, ch, obj );
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 50 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
    if (!IS_NPC(victim) && IS_IMMUNE(victim,IMM_SHIELDED) )
    {
	send_to_char("You are unable to read their aura.\n\r",ch);
	return;
    }

    act("$n examines $N intently.",ch,NULL,victim,TO_NOTVICT);
    act("$n examines you intently.",ch,NULL,victim,TO_VICT);
    if (IS_NPC(victim)) sprintf(buf, "%s is an NPC.\n\r",victim->short_descr);
    else 
    {
	if (victim->level == 9) sprintf(buf, "%s is an Implementor.\n\r", victim->name);
	if (victim->level == 8) sprintf(buf, "%s is a High Judge.\n\r", victim->name);
	if (victim->level == 7) sprintf(buf, "%s is a Judge.\n\r", victim->name);
	if (victim->level == 6) sprintf(buf, "%s is an Enforcer.\n\r", victim->name);
	if (victim->level == 5) sprintf(buf, "%s is a Quest Maker.\n\r", victim->name);
	if (victim->level == 4) sprintf(buf, "%s is a Builder.\n\r", victim->name);
	if (victim->level == 3) sprintf(buf, "%s is an Avatar.\n\r", victim->name);
	else sprintf(buf, "%s is a Mortal.\n\r", victim->name);
    }
    send_to_char(buf,ch);
    if (!IS_NPC(victim))
    {
	sprintf(buf,"Str:%d, Int:%d, Wis:%d, Dex:%d, Con:%d.\n\r",get_curr_str(victim),get_curr_int(victim),get_curr_wis(victim),get_curr_dex(victim),get_curr_con(victim));
	send_to_char(buf,ch);
    }
    hr = GET_HITROLL(victim); dr = GET_DAMROLL(victim);
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_VAMPIRE))
    	{ hr += victim->pcdata->wolf; dr += victim->pcdata->wolf; }
    sprintf(buf,"Hp:%d/%d, Mana:%d/%d, Move:%d/%d.\n\r",victim->hit,victim->max_hit,victim->mana,victim->max_mana,victim->move,victim->max_move);
    send_to_char(buf,ch);
    if (!IS_NPC(victim)) sprintf(buf,"Hitroll:%d, Damroll:%d, AC:%d.\n\r",hr,dr,GET_AC(victim));
    else sprintf(buf,"AC:%d.\n\r",GET_AC(victim));
    send_to_char(buf,ch);
    if (!IS_NPC(victim))
    {
	sprintf(buf,"Status:%d, ",victim->race);
	send_to_char(buf,ch);
	if (IS_SET(victim->act, PLR_VAMPIRE))    
	{
	    sprintf(buf,"Blood:%d, ",victim->pcdata->condition[COND_THIRST]);
	    send_to_char(buf,ch);
	}
    }
    sprintf(buf,"Alignment:%d.\n\r",victim->alignment);
    send_to_char(buf,ch);
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_VAMPIRE))    
    {
	send_to_char("Disciplines:",ch);
	if (IS_VAMPAFF(victim, VAM_PROTEAN)) send_to_char(" Protean",ch);
	if (IS_VAMPAFF(victim, VAM_CELERITY)) send_to_char(" Celerity",ch);
	if (IS_VAMPAFF(victim, VAM_FORTITUDE)) send_to_char(" Fortitude",ch);
	if (IS_VAMPAFF(victim, VAM_POTENCE)) send_to_char(" Potence",ch);
	if (IS_VAMPAFF(victim, VAM_OBFUSCATE)) send_to_char(" Obfuscate",ch);
	if (IS_VAMPAFF(victim, VAM_OBTENEBRATION)) send_to_char(" Obtenebration",ch);
	if (IS_VAMPAFF(victim, VAM_SERPENTIS)) send_to_char(" Serpentis",ch);
	if (IS_VAMPAFF(victim, VAM_AUSPEX)) send_to_char(" Auspex",ch);
	if (IS_VAMPAFF(victim, VAM_DOMINATE)) send_to_char(" Dominate",ch);
	send_to_char(".\n\r",ch);
    }
    return;
}

void do_mortal( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    int       blood;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_SET(ch->act, PLR_VAMPIRE) && !IS_VAMPAFF(ch, VAM_MORTAL))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_OBFUSCATE) )
    {
	send_to_char("You are not trained in the Obfuscate discipline.\n\r",ch);
	return;
    }
    if (IS_SET(ch->act,PLR_VAMPIRE) )
    {
        if ( ch->pcdata->condition[COND_THIRST] < 100 )
        {
	    send_to_char("You must be at full blood to use this power.\n\r",ch);
	    return;
    	}
    	/* Have to make sure they have enough blood to change back */
    	blood = ch->pcdata->condition[COND_THIRST];
    	ch->pcdata->condition[COND_THIRST] = 666;

	/* Remove physical vampire attributes when you take mortal form */
    	if (IS_VAMPAFF(ch,VAM_DISGUISED) ) do_mask(ch,"self");
    	if (IS_IMMUNE(ch,IMM_SHIELDED) ) do_shield(ch,"");
    	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) ) do_shadowplane(ch,"");
    	if (IS_VAMPAFF(ch,VAM_FANGS) ) do_fangs(ch,"");
    	if (IS_VAMPAFF(ch,VAM_CLAWS) ) do_claws(ch,"");
    	if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) ) do_nightsight(ch,"");
    	if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) ) do_shadowsight(ch,"");
    	if (IS_SET(ch->act,PLR_HOLYLIGHT) ) do_truesight(ch,"");
    	if (IS_VAMPAFF(ch,VAM_CHANGED) ) do_change(ch,"human");
    	if (IS_POLYAFF(ch,POLY_SERPENT) ) do_serpent(ch,"");
	ch->pcdata->wolf = 0;
    	ch->pcdata->condition[COND_THIRST] = blood;

    	send_to_char("Colour returns to your skin and you warm up a little.\n\r",ch);
    	act("Colour returns to $n's skin.", ch, NULL, NULL, TO_ROOM);

    	REMOVE_BIT(ch->act, PLR_VAMPIRE);
    	SET_BIT(ch->vampaff, VAM_MORTAL);
	return;
    }
    send_to_char("You skin pales and cools.\n\r",ch);
    act("$n's skin pales slightly.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->act, PLR_VAMPIRE);
    REMOVE_BIT(ch->vampaff, VAM_MORTAL);
    return;
}

void do_mortalvamp( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    int       blood;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (IS_SET(ch->act,PLR_VAMPIRE) )
    {
    	/* Have to make sure they have enough blood to change back */
    	blood = ch->pcdata->condition[COND_THIRST];
    	ch->pcdata->condition[COND_THIRST] = 666;

	/* Remove physical vampire attributes when you take mortal form */
    	if (IS_VAMPAFF(ch,VAM_DISGUISED) ) do_mask(ch,"self");
    	if (IS_IMMUNE(ch,IMM_SHIELDED) ) do_shield(ch,"");
    	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) ) do_shadowplane(ch,"");
    	if (IS_VAMPAFF(ch,VAM_FANGS) ) do_fangs(ch,"");
    	if (IS_VAMPAFF(ch,VAM_CLAWS) ) do_claws(ch,"");
    	if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) ) do_nightsight(ch,"");
    	if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) ) do_shadowsight(ch,"");
    	if (IS_SET(ch->act,PLR_HOLYLIGHT) ) do_truesight(ch,"");
    	if (IS_VAMPAFF(ch,VAM_CHANGED) ) do_change(ch,"human");
    	if (IS_POLYAFF(ch,POLY_SERPENT) ) do_serpent(ch,"");
	ch->pcdata->wolf = 0;
    	ch->pcdata->condition[COND_THIRST] = blood;

    	send_to_char("You loose your vampire powers.\n\r",ch);

    	REMOVE_BIT(ch->act, PLR_VAMPIRE);
    	SET_BIT(ch->vampaff, VAM_MORTAL);
	return;
    }
    send_to_char("You regain your vampire powers.\n\r",ch);
    SET_BIT(ch->act, PLR_VAMPIRE);
    REMOVE_BIT(ch->vampaff, VAM_MORTAL);
    return;
}

void do_shield( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_SET(ch->act, PLR_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!IS_VAMPAFF(ch,VAM_OBFUSCATE) && !IS_VAMPAFF(ch,VAM_DOMINATE) )
    {
	send_to_char("You are not trained in the Obfuscate or Dominate disciplines.\n\r",ch);
	return;
    }
    if ( ch->pcdata->condition[COND_THIRST] < 60 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(50,60);
    if (!IS_IMMUNE(ch,IMM_SHIELDED) )
    {
    	send_to_char("You shield your aura from those around you.\n\r",ch);
    	SET_BIT(ch->immune, IMM_SHIELDED);
	return;
    }
    send_to_char("You stop shielding your aura.\n\r",ch);
    REMOVE_BIT(ch->immune, IMM_SHIELDED);
    return;
}

void do_serpent( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH]; 
    char buf [MAX_INPUT_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_SERPENTIS) )
    {
	send_to_char("You are not trained in the Serpentis discipline.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch, AFF_POLYMORPH))
    {
	if (!IS_POLYAFF(ch,POLY_SERPENT))
	{
	    send_to_char( "You cannot polymorph from this form.\n\r", ch );
	    return;
	}
	act( "You transform back into human.", ch, NULL, NULL, TO_CHAR );
	act( "$n transform into human form.", ch, NULL, NULL, TO_ROOM );
	REMOVE_BIT(ch->polyaff, POLY_SERPENT);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	clear_stats(ch);
	free_string( ch->morph );
	ch->morph = str_dup( "" );
	ch->max_hit = ch->max_hit - 25;
	ch->hit = ch->hit - 25;
	if (ch->hit < 1) ch->hit = 1;
	ch->max_mana = ch->max_mana + 50;
	return;
    }
    if ( ch->pcdata->condition[COND_THIRST] < 50 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
    clear_stats(ch);
    if (ch->wpn[0] > 0)
    {
    	ch->hitroll += ch->wpn[0]/4;
    	ch->damroll += ch->wpn[0]/4;
    	ch->armor   -= ch->wpn[0];
    }
    ch->pcdata->mod_str = 10;
    act( "You transform into a huge serpent.", ch, NULL, NULL, TO_CHAR );
    act( "$n transforms into a huge serpent.", ch, NULL, NULL, TO_ROOM );
    SET_BIT(ch->polyaff, POLY_SERPENT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    sprintf(buf, "%s the huge serpent", ch->name);
    free_string( ch->morph );
    ch->morph = str_dup( buf );
    ch->max_hit = ch->max_hit + 25;
    ch->hit = ch->hit + 25;
    ch->max_mana = ch->max_mana - 50;
    return;
}

void do_poison( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    char       arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if (!IS_VAMPAFF(ch,VAM_SERPENTIS) )
    {
	send_to_char("You are not trained in the Serpentis discipline.\n\r",ch);
	return;
    }

    if ( ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    &&   ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL ) )
    {
	send_to_char( "You must wield the weapon you wish to poison.\n\r", ch );
	return;
    }

    if ( obj->value[0] != 0 )
    {
	send_to_char( "This weapon cannot be poisoned.\n\r", ch );
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 15 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(5,15);
    act("You run your tongue along $p, poisoning it.",ch,obj,NULL,TO_CHAR);
    act("$n runs $s tongue along $p, poisoning it.",ch,obj,NULL,TO_ROOM);
    obj->value[0] = 53;
    obj->timer = number_range(10,20);

    return;
}

void do_regenerate( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];
    int min = 5;
    int max = 10;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SET(ch->act, PLR_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char("You cannot regenerate while fighting.\n\r",ch);
	return;
    }

    if ( ch->hit >= ch->max_hit && ch->mana >= ch->max_mana && ch->move >= ch->max_move )
    {
	send_to_char("But you are already completely regenerated!\n\r",ch);
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 5 )
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }

    ch->pcdata->condition[COND_THIRST] -= number_range(2,5);
    if ( ch->hit >= ch->max_hit && ch->mana >= ch->max_mana && ch->move >= ch->max_move )
    {
    	send_to_char("Your body has completely regenerated.\n\r",ch);
    	act("$n's body completely regenerates itself.",ch,NULL,NULL,TO_ROOM);
    }
    else
    	send_to_char("Your body slowly regenerates itself.\n\r",ch);
    if (ch->hit < 1 )
    {
	ch->hit = ch->hit + 1;
	update_pos(ch);
	WAIT_STATE( ch, 24 );
    }
    else
    {
	min += 10 - ch->vampgen;
	min += 20 - (ch->vampgen * 2);
    	ch->hit = UMIN(ch->hit + number_range(min,max), ch->max_hit);
    	ch->mana = UMIN(ch->mana + number_range(min,max), ch->max_mana);
    	ch->move = UMIN(ch->move + number_range(min,max), ch->max_move);
	update_pos(ch);
    }
    return;
}

void do_rage( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];
    char       buf [MAX_INPUT_LENGTH];
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    send_to_char("You scream with rage as the beast within consumes you!\n\r",ch);
    if (IS_AFFECTED(ch,AFF_POLYMORPH))
	sprintf(buf,"%s screams with rage as $s inner beast consumes $m!.",ch->morph);
    else
	sprintf(buf,"$n screams with rage as $s inner beast consumes $m!.");
    act(buf, ch, NULL, NULL, TO_ROOM);
    do_beastlike(ch,"");
    if ( ch->pcdata->wolf < (12 - ch->vampgen) ) ch->pcdata->wolf += 1;
    if ( ch->position == POS_FIGHTING ) return;
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( ch == vch )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( can_see( ch, vch) )
		multi_hit( ch, vch, TYPE_UNDEFINED );
	}
    }
    return;
}

void do_humanity( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (!IS_NPC(ch) && IS_SET(ch->act,PLR_VAMPIRE) && ch->beast > 0
	&& ch->beast < 100 && number_range(1, 500) <= ch->beast)
    {
	if (ch->beast == 1)
	{
	    send_to_char("You have attained Golconda!\n\r", ch);
	    ch->exp += 1000000;
	    if (!IS_IMMUNE(ch, IMM_SUNLIGHT))
		SET_BIT(ch->immune, IMM_SUNLIGHT);
	}
	else
	    send_to_char("You feel slightly more in control of your beast.\n\r", ch);
	ch->beast -= 1;
    }
    return;
}

void do_beastlike( CHAR_DATA *ch, char *argument )
{
    char       arg [MAX_INPUT_LENGTH];
    int blood;

    argument = one_argument( argument, arg );

    if (!IS_NPC(ch) && IS_SET(ch->act,PLR_VAMPIRE) && ch->beast < 100
	&& ch->beast > 0 && number_range(1, 500) <= ch->beast)
    {
	if (ch->beast < 99)
	    send_to_char("You feel your beast take more control over your actions.\n\r", ch);
	else
	    send_to_char("Your beast has fully taken over control of your actions!\n\r", ch);
	ch->beast += 1;
    	blood = ch->pcdata->condition[COND_THIRST];
    	ch->pcdata->condition[COND_THIRST] = 666;
    	if ((IS_VAMPAFF(ch,VAM_PROTEAN) || (IS_VAMPAFF(ch,VAM_OBTENEBRATION)))
		&& !IS_VAMPAFF(ch,VAM_NIGHTSIGHT) ) do_nightsight(ch,"");
    	if (!IS_VAMPAFF(ch,VAM_FANGS) ) do_fangs(ch,"");
    	if (IS_VAMPAFF(ch,VAM_PROTEAN) &&
		!IS_VAMPAFF(ch,VAM_CLAWS) ) do_claws(ch,"");
    	ch->pcdata->condition[COND_THIRST] = blood;
    }
    return;
}

void do_feed( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    char      bufch [MAX_INPUT_LENGTH];
    char      bufvi [MAX_INPUT_LENGTH];
    int       blood;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_SET(ch->act, PLR_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (IS_NPC(victim))
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }
    if (!IS_SET(victim->act, PLR_VAMPIRE))
    {
	send_to_char("Blood does them no good at all.\n\r",ch);
	return;
    }
    if (!IS_IMMUNE(victim, IMM_VAMPIRE))
    {
	send_to_char("They refuse to drink your blood.\n\r",ch);
	return;
    }
    if ( ch->pcdata->condition[COND_THIRST] < 20 )
    {
	send_to_char("You don't have enough blood.\n\r",ch);
	return;
    }

    blood = number_range(5,10);
    ch->pcdata->condition[COND_THIRST] -= (blood * 2);
    victim->pcdata->condition[COND_THIRST] += blood;

    if (IS_AFFECTED(ch,AFF_POLYMORPH)) sprintf(bufch,ch->morph);
    else sprintf(bufch,ch->name);
    if (IS_AFFECTED(victim,AFF_POLYMORPH)) sprintf(bufvi,victim->morph);
    else sprintf(bufvi,victim->name);
    sprintf(buf,"You cut open your wrist and feed some blood to %s.",bufvi);
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf,"%s cuts open $s wrist and feeds some blood to %s.",bufch,bufvi);
    act(buf, ch, NULL, victim, TO_NOTVICT);
    if (victim->position < 5)
	send_to_char("You feel some blood poured down your throat.\n\r",victim);
    else
    {
	sprintf(buf,"%s cuts open $s wrist and feeds you some blood.",bufch);
	act(buf, ch, NULL, victim, TO_VICT);
    }
    return;
}

void do_upkeep( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];
    char      buf2 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_SET(ch->act, PLR_VAMPIRE))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    send_to_char("                              -= Vampire upkeep =-\n\r",ch);
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    send_to_char("Staying alive...upkeep 1.\n\r",ch);

    	if (IS_VAMPAFF(ch,VAM_DISGUISED) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You are disguised as %s...no upkeep.\n\r",ch->morph);
	    else if (ch->beast == 100) sprintf(buf,"You are disguised as %s...upkeep 10-20.\n\r",ch->morph);
	    else sprintf(buf,"You are disguised as %s...upkeep 5-10.\n\r",ch->morph);
	    send_to_char(buf,ch);
	}
    	if (IS_IMMUNE(ch,IMM_SHIELDED) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You are shielded...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You are shielded...upkeep 2-6.\n\r");
	    else sprintf(buf,"You are shielded...upkeep 1-3.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_AFFECTED(ch,AFF_SHADOWPLANE) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You are in the shadowplane...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You are in the shadowplane...no upkeep.\n\r");
	    else sprintf(buf,"You are in the shadowplane...no upkeep.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_VAMPAFF(ch,VAM_FANGS) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You have your fangs out...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You have your fangs out...upkeep 2.\n\r");
	    else sprintf(buf,"You have your fangs out...upkeep 1.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_VAMPAFF(ch,VAM_CLAWS) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You have your claws out...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You have your claws out...upkeep 2-6.\n\r");
	    else sprintf(buf,"You have your claws out...upkeep 1-3.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You have nightsight...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You have nightsight...upkeep 2.\n\r");
	    else sprintf(buf,"You have nightsight...upkeep 1.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_AFFECTED(ch,AFF_SHADOWSIGHT) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You have shadowsight...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You are shadowsight...upkeep 2-6.\n\r");
	    else sprintf(buf,"You are shadowsight...upkeep 1-3.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_SET(ch->act,PLR_HOLYLIGHT) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You have truesight...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You have truesight...upkeep 2-10.\n\r");
	    else sprintf(buf,"You have truesight...upkeep 1-5.\n\r");
	    send_to_char(buf,ch);
	}
    	if (IS_VAMPAFF(ch,VAM_CHANGED) )
	{
	    if      (IS_POLYAFF(ch,POLY_BAT))  sprintf(buf2,"bat" );
	    else if (IS_POLYAFF(ch,POLY_WOLF)) sprintf(buf2,"wolf");
	    else                               sprintf(buf2,"mist");
	    if      (ch->beast == 0  ) sprintf(buf,"You have changed into %s form...no upkeep.\n\r",buf2);
	    else if (ch->beast == 100) sprintf(buf,"You have changed into %s form...upkeep 10-20.\n\r",buf2);
	    else sprintf(buf,"You have changed into %s form...upkeep 5-10.\n\r",buf2);
	    send_to_char(buf,ch);
	}
    	if (IS_POLYAFF(ch,POLY_SERPENT) )
	{
	    if      (ch->beast == 0  ) sprintf(buf,"You are in serpent form...no upkeep.\n\r");
	    else if (ch->beast == 100) sprintf(buf,"You are in serpent form...upkeep 6-8.\n\r");
	    else sprintf(buf,"You are in serpent form...upkeep 1-3.\n\r");
	    send_to_char(buf,ch);
	}
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    return;
}

void do_vclan( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_SET(ch->act, PLR_VAMPIRE) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

    sprintf( buf, "The %s clan:\n\r", ch->clan );
    send_to_char( buf, ch );
    send_to_char("[      Name      ] [ Gen ] [ Hits  % ] [ Mana  % ] [ Move  % ] [  Exp  ] [Blood]\n\r", ch );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( IS_NPC(gch) ) continue;
	if ( !IS_SET(gch->act, PLR_VAMPIRE) ) continue;
	if ( !str_cmp(ch->clan,gch->clan) )
	{
	    sprintf( buf,
	    "[%-16s] [  %d  ] [%-6d%3d] [%-6d%3d] [%-6d%3d] [%7d] [ %3d ]\n\r",
		capitalize( gch->name ),
		gch->vampgen,
		gch->hit,  (gch->hit  * 100 / gch->max_hit ),
		gch->mana, (gch->mana * 100 / gch->max_mana),
		gch->move, (gch->move * 100 / gch->max_move),
		gch->exp, gch->pcdata->condition[COND_THIRST] );
		send_to_char( buf, ch );
	}
    }
    return;
}

void improve_wpn( CHAR_DATA *ch, int dtype, bool right_hand )
{
    OBJ_DATA *wield;
    char bufskill [20];
    char buf [MAX_INPUT_LENGTH];
    int dice1;
    int dice2;
    int trapper;

    dice1 = number_percent();
    dice2 = number_percent();

    if (IS_NPC(ch)) return;

    if (right_hand) wield = get_eq_char( ch, WEAR_WIELD );
    else            wield = get_eq_char( ch, WEAR_HOLD  );

    if (wield == NULL) dtype = TYPE_HIT;
    if ( dtype == TYPE_UNDEFINED )
    {
	dtype = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dtype += wield->value[3];
    }
    if ( dtype < 1000 || dtype > 1012 ) return;
    dtype -= 1000;
    if (ch->wpn[dtype] >= 200) return;
    trapper = ch->wpn[dtype];
    if ((dice1 > ch->wpn[dtype] && dice2 > ch->wpn[dtype]) || (dice1==100 || dice2==100)) ch->wpn[dtype] += 1;
    else return;
    if (trapper == ch->wpn[dtype]) return;

         if (ch->wpn[dtype] == 1  ) sprintf(bufskill,"slightly skilled");
    else if (ch->wpn[dtype] == 26 ) sprintf(bufskill,"reasonable");
    else if (ch->wpn[dtype] == 51 ) sprintf(bufskill,"fairly competent");
    else if (ch->wpn[dtype] == 76 ) sprintf(bufskill,"highly skilled");
    else if (ch->wpn[dtype] == 101) sprintf(bufskill,"very dangerous");
    else if (ch->wpn[dtype] == 126) sprintf(bufskill,"extremely deadly");
    else if (ch->wpn[dtype] == 151) sprintf(bufskill,"an expert");
    else if (ch->wpn[dtype] == 176) sprintf(bufskill,"a master");
    else if (ch->wpn[dtype] == 200) sprintf(bufskill,"a grand master");
    else return;
    if (wield == NULL || dtype == 0)
	sprintf(buf,"You are now %s at unarmed combat.\n\r",bufskill);
    else
	sprintf(buf,"You are now %s with %s.\n\r",bufskill,wield->short_descr);
    ADD_COLOUR(ch,buf,WHITE);
    send_to_char(buf,ch);
    return;
}

void improve_stance( CHAR_DATA *ch )
{
    char buf        [MAX_INPUT_LENGTH];
    char bufskill   [25];
    char stancename [10];
    int dice1;
    int dice2;
    int stance;

    dice1 = number_percent();
    dice2 = number_percent();

    if (IS_NPC(ch)) return;

    stance = ch->stance[0];
    if (stance < 1 || stance > 10) return;
    if ((dice1 > ch->stance[stance] && dice2 > ch->stance[stance]) || (dice1==100 || dice2==100)) ch->stance[stance] += 1;
    else return;
    if (stance == ch->stance[stance]) return;

         if (ch->stance[stance] == 1  ) sprintf(bufskill,"an apprentice of");
    else if (ch->stance[stance] == 26 ) sprintf(bufskill,"a trainee of");
    else if (ch->stance[stance] == 51 ) sprintf(bufskill,"a student of");
    else if (ch->stance[stance] == 76 ) sprintf(bufskill,"fairly experienced in");
    else if (ch->stance[stance] == 101) sprintf(bufskill,"well trained in");
    else if (ch->stance[stance] == 126) sprintf(bufskill,"highly skilled in");
    else if (ch->stance[stance] == 151) sprintf(bufskill,"an expert of");
    else if (ch->stance[stance] == 176) sprintf(bufskill,"a master of");
    else if (ch->stance[stance] == 200) sprintf(bufskill,"a grand master of");
    else return;
         if (stance == STANCE_VIPER   ) sprintf(stancename,"viper"   );
    else if (stance == STANCE_CRANE   ) sprintf(stancename,"crane"   );
    else if (stance == STANCE_CRAB    ) sprintf(stancename,"crab"    );
    else if (stance == STANCE_MONGOOSE) sprintf(stancename,"mongoose");
    else if (stance == STANCE_BULL    ) sprintf(stancename,"bull"    );
    else return;
    sprintf(buf,"You are now %s the %s stance.\n\r",bufskill,stancename);
    ADD_COLOUR(ch,buf,WHITE);
    send_to_char(buf,ch);
    return;
}

void do_alignment( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;
/*
    if (ch->level > 2)
    {
	send_to_char( "Only mortals have the power to change alignment.\n\r", ch );
	return;
    }
*/
    if ( arg[0] == '\0' )
    {
	send_to_char( "Please enter 'good', 'neutral', or 'evil'.\n\r", ch );
	return;
    }
    if      (!str_cmp(arg,"good"))
    {
	ch->alignment = 1000;
	send_to_char( "Alignment is now angelic.\n\r", ch );
    }
    else if (!str_cmp(arg,"neutral"))
    {
	ch->alignment = 0;
	send_to_char( "Alignment is now neutral.\n\r", ch );
    }
    else if (!str_cmp(arg,"evil"))
    {
	ch->alignment = -1000;
	send_to_char( "Alignment is now satanic.\n\r", ch );
    }
    else     send_to_char( "Please enter 'good', 'neutral', or 'evil'.\n\r", ch );
    return;
}

void do_side( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( arg[0] != '\0' && !str_cmp(arg,"renegade") && 
	( ch->class == 1 || ch->class == 2 ))
    {
	ch->class = 3;
	send_to_char( "You are now a renegade.\n\r", ch );
	sprintf(buf,"%s has turned renegade!",ch->name);
	do_info(ch,buf);
	return;
    }

    if (ch->class == 1)
    {
	send_to_char( "You are a scavenger.\n\r", ch );
	return;
    }
    else if (ch->class == 2)
    {
	send_to_char( "You are a mutant.\n\r", ch );
	return;
    }
    else if (ch->class == 3)
    {
	send_to_char( "You are a renegade.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "You have not yet picked a side to fight on.\n\r", ch );
	send_to_char( "Please enter 'Scavenger' or 'Mutant'.\n\r", ch );
	return;
    }
    if (!str_cmp(arg,"scavenger"))
    {
	ch->class = 1;
	send_to_char( "You are now a scavenger.\n\r", ch );
	sprintf(buf,"%s has sided with the scavengers!",ch->name);
	do_info(ch,buf);
    }
    else if (!str_cmp(arg,"mutant"))
    {
	ch->class = 2;
	send_to_char( "You are now a mutant.\n\r", ch );
	sprintf(buf,"%s has sided with the mutants!",ch->name);
	do_info(ch,buf);
    }
    else send_to_char( "Please enter 'Scavenger' or 'Mutant'.\n\r", ch );
    return;
}

void do_skill( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf       [MAX_INPUT_LENGTH];
    char buf2      [MAX_INPUT_LENGTH];
    char bufskill  [25];
    char bufskill2 [25];
    char arg       [MAX_INPUT_LENGTH];
    OBJ_DATA *wield;
    OBJ_DATA *wield2;
    int dtype;
    int dtype2;

    one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( arg[0] == '\0' )
	sprintf(arg,ch->name);

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_NPC(victim))
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    wield = get_eq_char( victim, WEAR_WIELD );
    wield2 = get_eq_char( victim, WEAR_HOLD );

    dtype = TYPE_HIT;
    dtype2 = TYPE_HIT;
    if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	dtype += wield->value[3];
    if ( wield2 != NULL && wield2->item_type == ITEM_WEAPON )
	dtype2 += wield2->value[3];
    dtype -= 1000;
    dtype2 -= 1000;

    if      (victim->wpn[dtype] == 00 ) sprintf(bufskill,"totally unskilled");
    else if (victim->wpn[dtype] <= 25 ) sprintf(bufskill,"slightly skilled");
    else if (victim->wpn[dtype] <= 50 ) sprintf(bufskill,"reasonable");
    else if (victim->wpn[dtype] <= 75 ) sprintf(bufskill,"fairly competent");
    else if (victim->wpn[dtype] <= 100) sprintf(bufskill,"highly skilled");
    else if (victim->wpn[dtype] <= 125) sprintf(bufskill,"very dangerous");
    else if (victim->wpn[dtype] <= 150) sprintf(bufskill,"extremely deadly");
    else if (victim->wpn[dtype] <= 175) sprintf(bufskill,"an expert");
    else if (victim->wpn[dtype] <= 199) sprintf(bufskill,"a master");
    else if (victim->wpn[dtype] >= 200) sprintf(bufskill,"a grand master");
    else return;

    if      (victim->wpn[dtype2] == 00 ) sprintf(bufskill2,"totally unskilled");
    else if (victim->wpn[dtype2] <= 25 ) sprintf(bufskill2,"slightly skilled");
    else if (victim->wpn[dtype2] <= 50 ) sprintf(bufskill2,"reasonable");
    else if (victim->wpn[dtype2] <= 75 ) sprintf(bufskill2,"fairly competent");
    else if (victim->wpn[dtype2] <= 100) sprintf(bufskill2,"highly skilled");
    else if (victim->wpn[dtype2] <= 125) sprintf(bufskill2,"very dangerous");
    else if (victim->wpn[dtype2] <= 150) sprintf(bufskill2,"extremely deadly");
    else if (victim->wpn[dtype2] <= 175) sprintf(bufskill2,"an expert");
    else if (victim->wpn[dtype2] <= 199) sprintf(bufskill2,"a master");
    else if (victim->wpn[dtype2] >= 200) sprintf(bufskill2,"a grand master");
    else return;

    if (ch == victim)
    {
    	if (dtype == 0 && dtype2 == 0)
	    sprintf(buf,"You are %s at unarmed combat.\n\r",bufskill);
    	else
	{
	    if (dtype != 0)
		sprintf(buf,"You are %s with %s.\n\r",bufskill,wield->short_descr);
	    if (dtype2 != 0)
		sprintf(buf2,"You are %s with %s.\n\r",bufskill2,wield2->short_descr);
	}
    }
    else
    {
    	if (dtype == 0 && dtype2 == 0)
	    sprintf(buf,"%s is %s at unarmed combat.\n\r",victim->name,bufskill);
    	else
	{
	    if (dtype != 0)
		sprintf(buf,"%s is %s with %s.\n\r",victim->name,bufskill,wield->short_descr);
	    if (dtype2 != 0)
		sprintf(buf2,"%s is %s with %s.\n\r",victim->name,bufskill2,wield2->short_descr);
	}
    }
    if (!(dtype == 0 && dtype2 != 0)) send_to_char(buf,ch);
    if (dtype2 != 0) send_to_char(buf2,ch);
    skillstance(ch,victim);
    return;
}

void skillstance( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf        [MAX_INPUT_LENGTH];
    char bufskill   [25];
    char stancename [10];
    int stance;

    if (IS_NPC(victim)) return;

    stance = victim->stance[0];
    if (stance < 1 || stance > 10) return;
         if (victim->stance[stance] <= 0  ) sprintf(bufskill,"completely unskilled in");
    else if (victim->stance[stance] <= 25 ) sprintf(bufskill,"an apprentice of");
    else if (victim->stance[stance] <= 50 ) sprintf(bufskill,"a trainee of");
    else if (victim->stance[stance] <= 75 ) sprintf(bufskill,"a student of");
    else if (victim->stance[stance] <= 100) sprintf(bufskill,"fairly experienced in");
    else if (victim->stance[stance] <= 125) sprintf(bufskill,"well trained in");
    else if (victim->stance[stance] <= 150) sprintf(bufskill,"highly skilled in");
    else if (victim->stance[stance] <= 175) sprintf(bufskill,"an expert of");
    else if (victim->stance[stance] <= 199) sprintf(bufskill,"a master of");
    else if (victim->stance[stance] >= 200) sprintf(bufskill,"a grand master of");
    else return;
         if (stance == STANCE_VIPER   ) sprintf(stancename,"viper"   );
    else if (stance == STANCE_CRANE   ) sprintf(stancename,"crane"   );
    else if (stance == STANCE_CRAB    ) sprintf(stancename,"crab"    );
    else if (stance == STANCE_MONGOOSE) sprintf(stancename,"mongoose");
    else if (stance == STANCE_BULL    ) sprintf(stancename,"bull"    );
    else return;
    if (ch == victim)
    	sprintf(buf,"You are %s the %s stance.",bufskill,stancename);
    else
    	sprintf(buf,"$N is %s the %s stance.",bufskill,stancename);
    act(buf,ch,NULL,victim,TO_CHAR);
    return;
}

void do_spell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	show_spell(ch,0);
	show_spell(ch,1);
	show_spell(ch,2);
	show_spell(ch,3);
	show_spell(ch,4);
    }
    else if (!str_cmp(arg,"purple")) show_spell(ch,0);
    else if (!str_cmp(arg,"red"))    show_spell(ch,1);
    else if (!str_cmp(arg,"blue"))   show_spell(ch,2);
    else if (!str_cmp(arg,"green"))  show_spell(ch,3);
    else if (!str_cmp(arg,"yellow")) show_spell(ch,4);
    else send_to_char("You know of no such magic.\n\r",ch);
    return;
}

void show_spell( CHAR_DATA *ch, int dtype )
{
    char buf[MAX_INPUT_LENGTH];
    char bufskill[MAX_INPUT_LENGTH];

    if (IS_NPC(ch)) return;

    if      (ch->spl[dtype] == 00 ) sprintf(bufskill,"untrained");
    else if (ch->spl[dtype] <= 25 ) sprintf(bufskill,"an apprentice");
    else if (ch->spl[dtype] <= 50 ) sprintf(bufskill,"a student");
    else if (ch->spl[dtype] <= 75 ) sprintf(bufskill,"a scholar");
    else if (ch->spl[dtype] <= 100) sprintf(bufskill,"a magus");
    else if (ch->spl[dtype] <= 125) sprintf(bufskill,"an adept ");
    else if (ch->spl[dtype] <= 150) sprintf(bufskill,"a mage");
    else if (ch->spl[dtype] <= 175) sprintf(bufskill,"a warlock");
    else if (ch->spl[dtype] <= 199) sprintf(bufskill,"a master wizard");
    else if (ch->spl[dtype] >= 200) sprintf(bufskill,"a grand sorcerer");
    else return;
    if      (dtype == 0)
	sprintf(buf,"You are %s at purple magic.\n\r",bufskill);
    else if (dtype == 1)
	sprintf(buf,"You are %s at red magic.\n\r",bufskill);
    else if (dtype == 2)
	sprintf(buf,"You are %s at blue magic.\n\r",bufskill);
    else if (dtype == 3)
	sprintf(buf,"You are %s at green magic.\n\r",bufskill);
    else if (dtype == 4)
	sprintf(buf,"You are %s at yellow magic.\n\r",bufskill);
    else return;
    send_to_char(buf,ch);
    return;
}

/* In case we need to remove our pfiles, or wanna turn mortal for a bit */
void do_relevel( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!str_cmp(ch->name,"KaVir") || !str_cmp(ch->name,"Lucifur") )
    {
	ch->level = MAX_LEVEL;
	ch->trust = MAX_LEVEL;
	send_to_char("Done.\n\r",ch);
    }
    else if (!str_cmp(ch->name,"Malice") || !str_cmp(ch->name,"Shaddai") )
    {
	ch->level = MAX_LEVEL-1;
	ch->trust = MAX_LEVEL-1;
	send_to_char("Done.\n\r",ch);
    }
    else
	send_to_char("Huh?\n\r",ch);
    return;
}

void do_scan( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;

    location = ch->in_room;

    send_to_char("[North]\n\r",ch);
    do_spydirection(ch,"n");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("[East]\n\r",ch);
    do_spydirection(ch,"e");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("[South]\n\r",ch);
    do_spydirection(ch,"s");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("[West]\n\r",ch);
    do_spydirection(ch,"w");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("[Up]\n\r",ch);
    do_spydirection(ch,"u");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("[Down]\n\r",ch);
    do_spydirection(ch,"d");
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}

void do_spy( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    char            arg1      [MAX_INPUT_LENGTH];
    int             door;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char("Which direction do you wish to spy?\n\r", ch);
	return;
    }

    if      ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) )
	door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east" ) )
	door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) )
	door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west" ) )
	door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up" ) )
	door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down" ) )
	door = 5;
    else
    {
	send_to_char("You can only spy people north, south, east, west, up or down.\n\r", ch);
	return;
    }

    location = ch->in_room;

    send_to_char("[Short Range]\n\r",ch);
    do_spydirection(ch,arg1);
    send_to_char("\n\r",ch);
    send_to_char("[Medium Range]\n\r",ch);
    do_spydirection(ch,arg1);
    send_to_char("\n\r",ch);
    send_to_char("[Long Range]\n\r",ch);
    do_spydirection(ch,arg1);
    send_to_char("\n\r",ch);

    /* Move them back */
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}

void do_spydirection( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA       *pexit;
    CHAR_DATA       *vch;
    CHAR_DATA       *vch_next;
    char            arg      [MAX_INPUT_LENGTH];
    char            buf      [MAX_INPUT_LENGTH];
    int             door;
    bool            is_empty;

    argument = one_argument( argument, arg );

    is_empty = TRUE;

    if ( arg[0] == '\0' )
	return;

    if      ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else return;

    if (( pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
    {
	send_to_char("   No exit.\n\r",ch);
	return;
    }
    pexit = ch->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) )
    {
	send_to_char("   Closed door.\n\r",ch);
	return;
    }
    char_from_room(ch);
    char_to_room(ch,to_room);
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch == ch )
	    continue;
	if ( !can_see(ch,vch) )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if (!IS_NPC(vch) && !IS_AFFECTED(vch,AFF_POLYMORPH))
		sprintf(buf,"   %s (Player)\n\r",vch->name);
	    else if (!IS_NPC(vch) && IS_AFFECTED(vch,AFF_POLYMORPH))
		sprintf(buf,"   %s (Player)\n\r",vch->morph);
	    else if (IS_NPC(vch))
		sprintf(buf,"   %s\n\r",capitalize(vch->short_descr));
	    send_to_char(buf,ch);
	    is_empty = FALSE;
	    continue;
	}
    }
    if (is_empty)
    {
	send_to_char("   Nobody here.\n\r",ch);
	return;
    }
    return;
}

void do_throw( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA       *pexit;
    CHAR_DATA       *victim;
    OBJ_DATA        *obj;
    char            arg1      [MAX_INPUT_LENGTH];
    char            arg2      [MAX_INPUT_LENGTH];
    char            buf       [MAX_INPUT_LENGTH];
    char            revdoor   [MAX_INPUT_LENGTH];
    int             door;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
    	if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
	{
	    send_to_char( "You are not holding anything to throw.\n\r", ch );
	    return;
	}
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char("Which direction do you wish to throw?\n\r", ch);
	return;
    }

    if      ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) )
	{door = 0;sprintf(arg1,"north");}
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east" ) )
	{door = 1;sprintf(arg1,"east");}
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) )
	{door = 2;sprintf(arg1,"south");}
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west" ) )
	{door = 3;sprintf(arg1,"west");}
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up" ) )
	{door = 4;sprintf(arg1,"up");}
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down" ) )
	{door = 5;sprintf(arg1,"down");}
    else
    {
	send_to_char("You can only throw north, south, east, west, up or down.\n\r", ch);
	return;
    }

    if      ( !str_cmp( arg1, "north" ) ) {door = 0;sprintf(revdoor,"south");}
    else if ( !str_cmp( arg1, "east"  ) ) {door = 1;sprintf(revdoor,"west" );}
    else if ( !str_cmp( arg1, "south" ) ) {door = 2;sprintf(revdoor,"north");}
    else if ( !str_cmp( arg1, "west"  ) ) {door = 3;sprintf(revdoor,"east" );}
    else if ( !str_cmp( arg1, "up"    ) ) {door = 4;sprintf(revdoor,"down" );}
    else if ( !str_cmp( arg1, "down"  ) ) {door = 5;sprintf(revdoor,"up"   );}
    else return;

    location = ch->in_room;

    sprintf(buf,"You hurl $p %s.",arg1);
    act(buf,ch,obj,NULL,TO_CHAR);
    sprintf(buf,"$n hurls $p %s.",arg1);
    act(buf,ch,obj,NULL,TO_ROOM);
    /* First room */
    if (( pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
    {
	sprintf(buf,"$p bounces off the %s wall.",arg1);
	act(buf,ch,obj,NULL,TO_ROOM);
	act(buf,ch,obj,NULL,TO_CHAR);
	obj_from_char(obj);
	obj_to_room(obj, ch->in_room);
	return;
    }
    pexit = ch->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) )
    {
	sprintf(buf,"$p bounces off the %s door.",arg1);
	act(buf,ch,obj,NULL,TO_ROOM);
	act(buf,ch,obj,NULL,TO_CHAR);
	obj_from_char(obj);
	obj_to_room(obj, ch->in_room);
	return;
    }
    char_from_room(ch);
    char_to_room(ch,to_room);
    if ( ( victim = get_char_room( ch, arg2 ) ) != NULL )
    {
	sprintf(buf,"$p comes flying in from the %s and lands in $N's hands.",revdoor);
	act(buf,ch,obj,victim,TO_NOTVICT);
	sprintf(buf,"$p comes flying in from the %s and lands in your hands.",revdoor);
	act(buf,ch,obj,victim,TO_VICT);
	obj_from_char(obj);
	obj_to_char(obj, victim);
    	char_from_room(ch);
    	char_to_room(ch,location);
	return;
    }

    /* Second room */
    if (( pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
    {
	sprintf(buf,"$p comes flying in from the %s and strikes %s wall.",revdoor,arg1);
	act(buf,ch,obj,NULL,TO_ROOM);
	obj_from_char(obj);
	obj_to_room(obj, ch->in_room);
    	char_from_room(ch);
    	char_to_room(ch,location);
	return;
    }
    pexit = ch->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) )
    {
	sprintf(buf,"$p comes flying in from the %s and strikes the %s door.",revdoor,arg1);
	act(buf,ch,obj,NULL,TO_ROOM);
	obj_from_char(obj);
	obj_to_room(obj, ch->in_room);
    	char_from_room(ch);
    	char_to_room(ch,location);
	return;
    }
    sprintf(buf,"$p comes flying in from the %s and carries on %s.",revdoor, arg1);
    act(buf,ch,obj,NULL,TO_ROOM);
    char_from_room(ch);
    char_to_room(ch,to_room);
    if ( ( victim = get_char_room( ch, arg2 ) ) != NULL )
    {
	sprintf(buf,"$p comes flying in from the %s and lands in $N's hands.",revdoor);
	act(buf,ch,obj,victim,TO_NOTVICT);
	sprintf(buf,"$p comes flying in from the %s and lands in your hands.",revdoor);
	act(buf,ch,obj,victim,TO_VICT);
	obj_from_char(obj);
	obj_to_char(obj, victim);
    	char_from_room(ch);
    	char_to_room(ch,location);
	return;
    }

    /* Third room */
    if (( pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
    {
	sprintf(buf,"$p comes flying in from the %s and strikes %s wall.",revdoor,arg1);
	act(buf,ch,obj,NULL,TO_ROOM);
	obj_from_char(obj);
	obj_to_room(obj, ch->in_room);
    	char_from_room(ch);
    	char_to_room(ch,location);
	return;
    }
    pexit = ch->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) )
    {
	sprintf(buf,"$p comes flying in from the %s and strikes the %s door.",revdoor,arg1);
	act(buf,ch,obj,NULL,TO_ROOM);
	obj_from_char(obj);
	obj_to_room(obj, ch->in_room);
    	char_from_room(ch);
    	char_to_room(ch,location);
	return;
    }
    sprintf(buf,"$p comes flying in from the %s and carries on %s.",revdoor, arg1);
    act(buf,ch,obj,NULL,TO_ROOM);
    char_from_room(ch);
    char_to_room(ch,to_room);
    if ( ( victim = get_char_room( ch, arg2 ) ) != NULL )
    {
	sprintf(buf,"$p comes flying in from the %s and lands in $N's hands.",revdoor);
	act(buf,ch,obj,victim,TO_NOTVICT);
	sprintf(buf,"$p comes flying in from the %s and lands in your hands.",revdoor);
	act(buf,ch,obj,victim,TO_VICT);
	obj_from_char(obj);
	obj_to_char(obj, victim);
    	char_from_room(ch);
    	char_to_room(ch,location);
	return;
    }

    sprintf(buf,"$p comes flying in from the %s and drops at your feet.",revdoor);
    act(buf,ch,obj,NULL,TO_ROOM);
    obj_from_char(obj);
    obj_to_room(obj, ch->in_room);

    /* Move them back */
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}

void do_shoot( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA       *pexit;
    CHAR_DATA       *victim;
    OBJ_DATA        *obj;
    OBJ_DATA        *ammo;
    char            arg1      [MAX_INPUT_LENGTH];
    char            arg2      [MAX_INPUT_LENGTH];
    char            buf       [MAX_INPUT_LENGTH];
    char            revdoor   [MAX_INPUT_LENGTH];
    int             door;
    bool	    missed;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	|| obj->item_type != ITEM_MISSILE )
    {
    	if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
	    || obj->item_type != ITEM_MISSILE )
	{
	    send_to_char( "You are not holding anything to shoot.\n\r", ch );
	    return;
	}
    }

    if ( obj->value[1] < 1 )
    {
	send_to_char("First you need to load it.\n\r", ch);
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char("Please specify a direction to shoot.\n\r", ch);
	return;
    }

    if ( arg2[0] == '\0' )
    {
	send_to_char("Please specify a target to shoot at.\n\r", ch);
	return;
    }

    if      ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) )
	{door = 0;sprintf(arg1,"north");}
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east" ) )
	{door = 1;sprintf(arg1,"east");}
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) )
	{door = 2;sprintf(arg1,"south");}
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west" ) )
	{door = 3;sprintf(arg1,"west");}
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up" ) )
	{door = 4;sprintf(arg1,"up");}
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down" ) )
	{door = 5;sprintf(arg1,"down");}
    else
    {
	send_to_char("You can only shoot north, south, east, west, up or down.\n\r", ch);
	return;
    }

    if      ( !str_cmp( arg1, "north" ) ) {door = 0;sprintf(revdoor,"south");}
    else if ( !str_cmp( arg1, "east"  ) ) {door = 1;sprintf(revdoor,"west" );}
    else if ( !str_cmp( arg1, "south" ) ) {door = 2;sprintf(revdoor,"north");}
    else if ( !str_cmp( arg1, "west"  ) ) {door = 3;sprintf(revdoor,"east" );}
    else if ( !str_cmp( arg1, "up"    ) ) {door = 4;sprintf(revdoor,"down" );}
    else if ( !str_cmp( arg1, "down"  ) ) {door = 5;sprintf(revdoor,"up"   );}
    else return;

    if ( get_obj_index( obj->value[0] ) == NULL ) return;

    location = ch->in_room;
    ammo = create_object( get_obj_index( obj->value[0] ), 0 );

    if ( number_percent() <= ammo->value[0] ) missed = FALSE;
    else missed = TRUE;
    obj->value[1] -= 1;

    sprintf(buf,"You point $p %s and shoot.",arg1);
    act(buf,ch,obj,NULL,TO_CHAR);
    sprintf(buf,"$n points $p %s and shoots.",arg1);
    act(buf,ch,obj,NULL,TO_ROOM);
    /* First room */
    if (( pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
    {
	sprintf(buf,"$p bounces off the %s wall.",arg1);
	act(buf,ch,ammo,NULL,TO_ROOM);
	act(buf,ch,ammo,NULL,TO_CHAR);
	obj_to_room(ammo, ch->in_room);
	return;
    }
    pexit = ch->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) )
    {
	sprintf(buf,"$p bounces off the %s door.",arg1);
	act(buf,ch,ammo,NULL,TO_ROOM);
	act(buf,ch,ammo,NULL,TO_CHAR);
	obj_to_room(ammo, ch->in_room);
	return;
    }
    char_from_room(ch);
    char_to_room(ch,to_room);
    if ( ( victim = get_char_room( ch, arg2 ) ) != NULL )
    {
	if (missed)
	{
	    sprintf(buf,"$p flies in from the %s, just missing $N, and carrying on %s.",revdoor, arg1);
	    act(buf,ch,ammo,victim,TO_NOTVICT);
	    sprintf(buf,"$p flies in from the %s, just missing you, and carrying on %s.",revdoor, arg1);
	    act(buf,ch,ammo,victim,TO_VICT);
	}
	else
	{
	    sprintf(buf,"$p comes flying in from the %s and strikes $N.",revdoor);
	    act(buf,ch,ammo,victim,TO_NOTVICT);
	    sprintf(buf,"$p comes flying in from the %s and strikes you.",revdoor);
	    act(buf,ch,ammo,victim,TO_VICT);
	    obj_to_char(ammo, victim);
    	    char_from_room(ch);
    	    char_to_room(ch,location);
	    victim->hit -= number_range(ammo->value[1],ammo->value[2]);
	    update_pos(victim);
	    if (victim->position == POS_DEAD && !IS_HERO(victim))
	    {
		send_to_char( "You have been KILLED!!\n\r\n\r", victim );
		raw_kill(victim);
	    }
	    return;
	}
    }

    /* Second room */
    if (( pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
    {
	sprintf(buf,"$p comes flying in from the %s and strikes %s wall.",revdoor,arg1);
	act(buf,ch,ammo,NULL,TO_ROOM);
	obj_to_room(ammo, ch->in_room);
    	char_from_room(ch);
    	char_to_room(ch,location);
	return;
    }
    pexit = ch->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) )
    {
	sprintf(buf,"$p comes flying in from the %s and strikes the %s door.",revdoor,arg1);
	act(buf,ch,ammo,NULL,TO_ROOM);
	obj_to_room(ammo, ch->in_room);
    	char_from_room(ch);
    	char_to_room(ch,location);
	return;
    }
    if (!missed)
    {
    	sprintf(buf,"$p comes flying in from the %s and carries on %s.",revdoor, arg1);
    	act(buf,ch,ammo,NULL,TO_ROOM);
    }
    char_from_room(ch);
    char_to_room(ch,to_room);
    if ( ( victim = get_char_room( ch, arg2 ) ) != NULL )
    {
	if (missed)
	{
	    sprintf(buf,"$p flies in from the %s, just missing $N, and carrying on %s.",revdoor, arg1);
	    act(buf,ch,ammo,victim,TO_NOTVICT);
	    sprintf(buf,"$p flies in from the %s, just missing you, and carrying on %s.",revdoor, arg1);
	    act(buf,ch,ammo,victim,TO_VICT);
	}
	else
	{
	    sprintf(buf,"$p comes flying in from the %s and strikes $N.",revdoor);
	    act(buf,ch,ammo,victim,TO_NOTVICT);
	    sprintf(buf,"$p comes flying in from the %s and strikes you.",revdoor);
	    act(buf,ch,ammo,victim,TO_VICT);
	    obj_to_char(ammo, victim);
    	    char_from_room(ch);
    	    char_to_room(ch,location);
	    victim->hit -= number_range(ammo->value[1],ammo->value[2]);
	    update_pos(victim);
	    if (victim->position == POS_DEAD && !IS_HERO(victim))
	    {
		send_to_char( "You have been KILLED!!\n\r\n\r", victim );
		raw_kill(victim);
	    }
	    return;
	}
    }

    /* Third room */
    if (( pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
    {
	sprintf(buf,"$p comes flying in from the %s and strikes %s wall.",revdoor,arg1);
	act(buf,ch,ammo,NULL,TO_ROOM);
	obj_to_room(ammo, ch->in_room);
    	char_from_room(ch);
    	char_to_room(ch,location);
	return;
    }
    pexit = ch->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) )
    {
	sprintf(buf,"$p comes flying in from the %s and strikes the %s door.",revdoor,arg1);
	act(buf,ch,ammo,NULL,TO_ROOM);
	obj_to_room(ammo, ch->in_room);
    	char_from_room(ch);
    	char_to_room(ch,location);
	return;
    }
    if (!missed)
    {
    	sprintf(buf,"$p comes flying in from the %s and carries on %s.",revdoor, arg1);
    	act(buf,ch,ammo,NULL,TO_ROOM);
    }
    char_from_room(ch);
    char_to_room(ch,to_room);
    if ( ( victim = get_char_room( ch, arg2 ) ) != NULL )
    {
	if (missed)
	{
	    sprintf(buf,"$p flies in from the %s, just missing $N, and carrying on %s.",revdoor, arg1);
	    act(buf,ch,ammo,victim,TO_NOTVICT);
	    sprintf(buf,"$p flies in from the %s, just missing you, and carrying on %s.",revdoor, arg1);
	    act(buf,ch,ammo,victim,TO_VICT);
	}
	else
	{
	    sprintf(buf,"$p comes flying in from the %s and strikes $N.",revdoor);
	    act(buf,ch,ammo,victim,TO_NOTVICT);
	    sprintf(buf,"$p comes flying in from the %s and strikes you.",revdoor);
	    act(buf,ch,ammo,victim,TO_VICT);
	    obj_to_char(ammo, victim);
    	    char_from_room(ch);
    	    char_to_room(ch,location);
	    victim->hit -= number_range(ammo->value[1],ammo->value[2]);
	    update_pos(victim);
	    if (victim->position == POS_DEAD && !IS_HERO(victim))
	    {
		send_to_char( "You have been KILLED!!\n\r\n\r", victim );
		raw_kill(victim);
	    }
	    return;
	}
    }

    sprintf(buf,"$p comes flying in from the %s and drops at your feet.",revdoor);
    act(buf,ch,ammo,NULL,TO_ROOM);
    obj_to_room(ammo, ch->in_room);

    /* Move them back */
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}

void do_reload( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    OBJ_DATA *ammo;
    OBJ_DATA *missile;

    obj = get_eq_char( ch, WEAR_WIELD );
    obj2 = get_eq_char( ch, WEAR_HOLD );

    if ( obj == NULL || obj2 == NULL )
    {
	send_to_char( "You must hold the weapon to reload in one hand and the ammo in the other.\n\r", ch );
	return;
    }

    if ( !(obj->item_type  == ITEM_MISSILE && obj2->item_type == ITEM_AMMO)
    &&   !(obj2->item_type == ITEM_MISSILE && obj->item_type  == ITEM_AMMO) )
    {
	send_to_char( "You must hold the weapon to reload in one hand and the ammo in the other.\n\r", ch );
	return;
    }

    if ( obj->item_type == ITEM_MISSILE )
    {
	missile = obj;
	ammo = obj2;
    }
    else
    {
	missile = obj2;
	ammo = obj;
    }

    if ( missile->value[1] >= missile->value[2] )
    {
	send_to_char( "It is already fully loaded.\n\r", ch );
	return;
    }

    if ( missile->value[3] != ammo->value[3] )
    {
	send_to_char( "That weapon doesn't take that sort of ammo.\n\r", ch );
	return;
    }

    if ( missile->value[0] != 0 && missile->value[1] != 0 &&
	 missile->value[0] != ammo->pIndexData->vnum )
    {
	send_to_char( "You cannot mix ammo types.\n\r", ch );
	return;
    }

    act("You load $p into $P.",ch,ammo,missile,TO_CHAR);
    act("$n loads $p into $P.",ch,ammo,missile,TO_ROOM);
    missile->value[1] += 1;
    missile->value[0] = ammo->pIndexData->vnum;
    obj_from_char( ammo );
    extract_obj( ammo );
    return;
}

void do_unload( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *missile;
    OBJ_DATA *ammo;

    if ( ( missile = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	|| missile->item_type != ITEM_MISSILE )
    {
    	if ( ( missile = get_eq_char( ch, WEAR_HOLD ) ) == NULL
	    || missile->item_type != ITEM_MISSILE )
	{
	    send_to_char( "You are not holding anything to unload.\n\r", ch );
	    return;
	}
    }

    if ( missile->value[1] <= 0 )
    {
	send_to_char( "But it is already empty!\n\r", ch );
	return;
    }

    if ( get_obj_index( missile->value[0] ) == NULL ) return;

    ammo = create_object( get_obj_index( missile->value[0] ), 0 );
    act("You remove $p from $P.",ch,ammo,missile,TO_CHAR);
    act("$n removes $p from $P.",ch,ammo,missile,TO_ROOM);
    missile->value[1] -= 1;
    obj_to_char(ammo, ch);
    return;
}

void do_stance( CHAR_DATA *ch, char *argument )
{
    char            arg      [MAX_INPUT_LENGTH];
    int             selection;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (IS_SET(ch->mounted, IS_RIDING))
    {
	send_to_char("Not while mounted.\n\r",ch);
	return;
    }
    if ( arg[0] == '\0' )
    {
	if (ch->stance[0] == -1)
	{
	    ch->stance[0] = 0;
	    send_to_char("You drop into a fighting stance.\n\r",ch);
	    act("$n drops into a fighting stance.",ch,NULL,NULL,TO_ROOM);
	}
	else
	{
	    ch->stance[0] = -1;
	    send_to_char("You relax from your fighting stance.\n\r",ch);
	    act("$n relaxes from $s fighting stance.",ch,NULL,NULL,TO_ROOM);
	}
	return;
    }
    if (ch->stance[0] > 0)
    {
	send_to_char("You cannot change stances until you come up from the one you are currently in.\n\r",ch);
	return;
    }
    if (!str_cmp(arg,"none")    ) {selection = STANCE_NONE;
	send_to_char("You drop into a general fighting stance.\n\r",ch);
	act("$n drops into a general fighting stance.",ch,NULL,NULL,TO_ROOM);}
    else if (!str_cmp(arg,"viper")   ) {selection = STANCE_VIPER;
	send_to_char("You arch your body into the viper fighting stance.\n\r",ch);
	act("$n arches $s body into the viper fighting stance.",ch,NULL,NULL,TO_ROOM);}
    else if (!str_cmp(arg,"crane")   ) {selection = STANCE_CRANE;
	send_to_char("You swing your body into the crane fighting stance.\n\r",ch);
	act("$n swings $s body into the crane fighting stance.",ch,NULL,NULL,TO_ROOM);}
    else if (!str_cmp(arg,"crab")    ) {selection = STANCE_CRAB;
	send_to_char("You squat down into the crab fighting stance.\n\r",ch);
	act("$n squats down into the crab fighting stance. ",ch,NULL,NULL,TO_ROOM);}
    else if (!str_cmp(arg,"mongoose")) {selection = STANCE_MONGOOSE;
	send_to_char("You twist into the mongoose fighting stance.\n\r",ch);
	act("$n twists into the mongoose fighting stance. ",ch,NULL,NULL,TO_ROOM);}
    else if (!str_cmp(arg,"bull")    ) {selection = STANCE_BULL;
	send_to_char("You hunch down into the bull fighting stance.\n\r",ch);
	act("$n hunches down into the bull fighting stance. ",ch,NULL,NULL,TO_ROOM);}
    else
    {
	send_to_char("Syntax is: stance <style>.\n\r",ch);
	send_to_char("Stance being one of: None, Viper, Crane, Crab, Mongoose, Bull.\n\r",ch);
	return;
    }
    ch->stance[0] = selection;
    WAIT_STATE( ch, 12 );
    return;
}

void do_fightstyle( CHAR_DATA *ch, char *argument )
{
    char            arg1     [MAX_INPUT_LENGTH];
    char            arg2     [MAX_INPUT_LENGTH];
    char            buf      [MAX_INPUT_LENGTH];
    int             selection;
    int             value;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("Syntax is: fightstyle <number> <style>.\n\r",ch);
	send_to_char("Style can be selected from the following (enter style in text form):\n\r",ch);
	send_to_char("[ 1]*Trip      [ 2]*Kick      [ 3] Bash      [ 4] Elbow     [ 5] Knee\n\r",ch);
	send_to_char("[ 6] Headbutt  [ 7]*Disarm    [ 8] Bite      [ 9]*Dirt      [10] Grapple\n\r",ch);
	send_to_char("[11] Punch     [12]*Gouge     [13] Rip       [14]*Stamp     [15] Backfist\n\r",ch);
	send_to_char("[16] Jumpkick  [17] Spinkick  [18] Hurl      [19] Sweep     [20] Charge\n\r",ch);
	sprintf(buf,"Selected options: 1:[%d] 2:[%d] 3:[%d] 4:[%d] 5:[%d] 6:[%d] 7:[%d] 8:[%d].\n\r",ch->cmbt[0],ch->cmbt[1],
	ch->cmbt[2],ch->cmbt[3],ch->cmbt[4],ch->cmbt[5],ch->cmbt[6],ch->cmbt[7]);
	send_to_char(buf,ch);
	send_to_char("\n\r* This has been coded (others are not yet in).\n\r",ch);
	return;
    }
    value = is_number( arg1 ) ? atoi( arg1 ) : -1;
    if (value < 1 || value > 8)
    {
	send_to_char("Please enter a value between 1 and 8.\n\r",ch);
	return;
    }
         if (!str_cmp(arg2,"clear")   ) selection = 0;
    else if (!str_cmp(arg2,"trip")    ) selection = 1;
    else if (!str_cmp(arg2,"kick")    ) selection = 2;
    else if (!str_cmp(arg2,"bash")    ) selection = 3;
    else if (!str_cmp(arg2,"elbow")   ) selection = 4;
    else if (!str_cmp(arg2,"knee")    ) selection = 5;
    else if (!str_cmp(arg2,"headbutt")) selection = 6;
    else if (!str_cmp(arg2,"disarm")  ) selection = 7;
    else if (!str_cmp(arg2,"bite")    ) selection = 8;
    else if (!str_cmp(arg2,"dirt")    ) selection = 9;
    else if (!str_cmp(arg2,"grapple") ) selection = 10;
    else if (!str_cmp(arg2,"punch")   ) selection = 11;
    else if (!str_cmp(arg2,"gouge")   ) selection = 12;
    else if (!str_cmp(arg2,"rip")     ) selection = 13;
    else if (!str_cmp(arg2,"stamp")   ) selection = 14;
    else if (!str_cmp(arg2,"backfist")) selection = 15;
    else if (!str_cmp(arg2,"jumpkick")) selection = 16;
    else if (!str_cmp(arg2,"spinkick")) selection = 17;
    else if (!str_cmp(arg2,"hurl")    ) selection = 18;
    else if (!str_cmp(arg2,"sweep")   ) selection = 19;
    else if (!str_cmp(arg2,"charge")  ) selection = 20;
    else
    {
	send_to_char("Syntax is: fightstyle <number> <style>.\n\r",ch);
	send_to_char("Style can be selected from the following (enter style in text form):\n\r",ch);
	send_to_char("[ 1]*Trip      [ 2]*Kick      [ 3] Bash      [ 4] Elbow     [ 5] Knee\n\r",ch);
	send_to_char("[ 6] Headbutt  [ 7]*Disarm    [ 8] Bite      [ 9]*Dirt      [10] Grapple\n\r",ch);
	send_to_char("[11] Punch     [12]*Gouge     [13] Rip       [14]*Stamp     [15] Backfist\n\r",ch);
	send_to_char("[16] Jumpkick  [17] Spinkick  [18] Hurl      [19] Sweep     [20] Charge\n\r",ch);
	sprintf(buf,"Selected options: 1:[%d] 2:[%d] 3:[%d] 4:[%d] 5:[%d] 6:[%d] 7:[%d] 8:[%d].\n\r",ch->cmbt[0],ch->cmbt[1],
	ch->cmbt[2],ch->cmbt[3],ch->cmbt[4],ch->cmbt[5],ch->cmbt[6],ch->cmbt[7]);
	send_to_char(buf,ch);
	send_to_char("\n\r* This has been coded (others are not yet in).\n\r",ch);
	return;
    }
    ch->cmbt[(value-1)] = selection;
    sprintf(buf,"Combat option %d now set to %s (%d).\n\r",value,arg2,ch->cmbt[0]);
    send_to_char(buf,ch);
    return;
}

void fightaction( CHAR_DATA *ch, CHAR_DATA *victim, int actype, int dtype, int wpntype)
{
    AFFECT_DATA af;
/*
    char            buf      [MAX_INPUT_LENGTH]; */

    if ( IS_NPC(ch) ) return;
/*  if ( IS_AFFECTED(ch,AFF_POLYMORPH) ) return; *** Just in case...KaVir */
    if ( actype < 1 || actype > 20 ) return;
    if ( victim == NULL ) return;

    /* Trip */
         if ( actype == 1 )
    {
	if ( number_percent( ) <= ch->wpn[0] )
	    trip( ch, victim );
    	return;
    }
    else if ( actype == 2 && number_percent() < 75 )
    {
	do_kick(ch,"");
    	return;
    }
    else if ( actype == 7 )
    {
	if ( number_percent() < 25 )
	    disarm( ch, victim );
    	return;
    }
    else if ( actype == 9 )
    {
	act( "You kick a spray of dirt into $N's face.",ch,NULL,victim,TO_CHAR );
	act( "$n kicks a spray of dirt into your face.",ch,NULL,victim,TO_VICT );
	act( "$n kicks a spray of dirt into $N's face.",ch,NULL,victim,TO_NOTVICT );
    	if ( IS_AFFECTED(victim, AFF_BLIND) || number_percent() < 25 )
	    return;
    	af.type      = 4;
    	af.location  = APPLY_HITROLL;
    	af.modifier  = -4;
    	af.duration  = 1;
    	af.bitvector = AFF_BLIND;
    	affect_to_char( victim, &af );
	act( "$N is blinded!",ch,NULL,victim,TO_CHAR );
	act( "$N is blinded!",ch,NULL,victim,TO_NOTVICT );
    	send_to_char( "You are blinded!\n\r", victim );
    	return;
    }
    else if ( actype == 12 )
    {
	act( "You gouge your fingers into $N's eyes.",ch,NULL,victim,TO_CHAR );
	act( "$n gouges $s fingers into your eyes.",ch,NULL,victim,TO_VICT );
	act( "$n gouges $s fingers into $N's eyes.",ch,NULL,victim,TO_NOTVICT );
    	if ( IS_AFFECTED(victim, AFF_BLIND) || number_percent() < 75 )
	{
	    one_hit( ch, victim, dtype, wpntype );
	    return;
	}
    	af.type      = 4;
    	af.location  = APPLY_HITROLL;
    	af.modifier  = -4;
    	af.duration  = 1;
    	af.bitvector = AFF_BLIND;
    	affect_to_char( victim, &af );
	act( "$N is blinded!",ch,NULL,victim,TO_CHAR );
	act( "$N is blinded!",ch,NULL,victim,TO_NOTVICT );
    	send_to_char( "You are blinded!\n\r", victim );
    	return;
    }
    else if ( actype == 14 )
    {
    	if ( victim->move < 1 ) return;
	act( "You leap in the air and stamp on $N's feet.",ch,NULL,victim,TO_CHAR );
	act( "$n leaps in the air and stamps on your feet.",ch,NULL,victim,TO_VICT );
	act( "$n leaps in the air and stamps on $N's feet.",ch,NULL,victim,TO_NOTVICT );
	victim->move -= number_range(25,50);
	if ( victim->move < 0 ) victim->move = 0;
    	return;
    }
    return;
}

void critical_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int dam)
{
    OBJ_DATA *obj;
    OBJ_DATA *damaged;
    char buf  [MAX_INPUT_LENGTH];
    char buf2 [20];
    int dtype;
    int critical = 0;
    int wpn = 0;
    int wpn2 = 0;
    int count;
    int count2;

    dtype = dt - 1000;
    if (dtype < 0 || dtype > 12) return;
    if (IS_NPC(ch)) critical += ((ch->level+1) / 5);
    if (!IS_NPC(ch)) critical += ((ch->wpn[dtype]+1)/10);
    if (IS_NPC(victim)) critical -= ((victim->level+1) / 5);
    if (!IS_NPC(victim))
    {
    	obj = get_eq_char( victim, WEAR_WIELD );
    	dtype = TYPE_HIT;
    	if ( obj != NULL && obj->item_type == ITEM_WEAPON )
	    dtype += obj->value[3];
    	wpn = dtype - 1000;
	if (wpn < 0 || wpn > 12) wpn = 0;
    	obj = get_eq_char( victim, WEAR_HOLD );
    	dtype = TYPE_HIT;
    	if ( obj != NULL && obj->item_type == ITEM_WEAPON )
	    dtype += obj->value[3];
    	wpn2 = dtype - 1000;
	if (wpn2 < 0 || wpn2 > 12) wpn2 = 0;

	if (victim->wpn[wpn] > victim->wpn[wpn2])
	    critical -= ((victim->wpn[wpn]+1)/10);
	else
	    critical -= ((victim->wpn[wpn2]+1)/10);
    }
         if (critical < 1 ) critical = 1;
    else if (critical > 20) critical = 20;
    if (number_percent() > critical) return;
    critical = number_range(1,23);
    if (critical == 1)
    {
	if (IS_HEAD(victim,LOST_EYE_L) && IS_HEAD(victim,LOST_EYE_R)) return;
	if ( (damaged = get_eq_char(victim,WEAR_FACE) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevents you from loosing an eye.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevents $n from loosing an eye.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p falls broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p falls broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_HEAD(victim,LOST_EYE_L) && number_percent() < 50)
	    SET_BIT(victim->loc_hp[0],LOST_EYE_L);
	else if (!IS_HEAD(victim,LOST_EYE_R))
	    SET_BIT(victim->loc_hp[0],LOST_EYE_R);
	else if (!IS_HEAD(victim,LOST_EYE_L))
	    SET_BIT(victim->loc_hp[0],LOST_EYE_L);
	else return;
	act("Your skillful blow takes out $N's eye!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow takes out $N's eye!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow takes out your eye!",ch,NULL,victim,TO_VICT);
	make_part(victim,"eyeball");
	return;
    }
    else if (critical == 2)
    {
	if (IS_HEAD(victim,LOST_EAR_L) && IS_HEAD(victim,LOST_EAR_R)) return;
	if ( (damaged = get_eq_char(victim,WEAR_HEAD) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevents you from loosing an ear.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevents $n from loosing an ear.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p falls broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p falls broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_HEAD(victim,LOST_EAR_L) && number_percent() < 50)
	    SET_BIT(victim->loc_hp[0],LOST_EAR_L);
	else if (!IS_HEAD(victim,LOST_EAR_R))
	    SET_BIT(victim->loc_hp[0],LOST_EAR_R);
	else if (!IS_HEAD(victim,LOST_EAR_L))
	    SET_BIT(victim->loc_hp[0],LOST_EAR_L);
	else return;
	act("Your skillful blow cuts off $N's ear!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow cuts off $N's ear!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow cuts off your ear!",ch,NULL,victim,TO_VICT);
	make_part(victim,"ear");
	return;
    }
    else if (critical == 3)
    {
	if (IS_HEAD(victim,LOST_NOSE)) return;
	if ( (damaged = get_eq_char(victim,WEAR_FACE) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevents you from loosing your nose.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevents $n from loosing $s nose.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p falls broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p falls broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	SET_BIT(victim->loc_hp[0],LOST_NOSE);
	act("Your skillful blow cuts off $N's nose!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow cuts off $N's nose!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow cuts off your nose!",ch,NULL,victim,TO_VICT);
	make_part(victim,"nose");
	return;
    }
    else if (critical == 4)
    {
	if (IS_HEAD(victim,LOST_NOSE) || IS_HEAD(victim,BROKEN_NOSE)) return;
	if ( (damaged = get_eq_char(victim,WEAR_FACE) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevents you from breaking your nose.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevents $n from breaking $s nose.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p falls broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p falls broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_HEAD(victim,LOST_NOSE) && !IS_HEAD(victim,BROKEN_NOSE))
	    SET_BIT(victim->loc_hp[0],BROKEN_NOSE);
	else return;
	act("Your skillful blow breaks $N's nose!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow breaks $N's nose!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow breaks your nose!",ch,NULL,victim,TO_VICT);
	return;
    }
    else if (critical == 5)
    {
	if (IS_HEAD(victim,BROKEN_JAW)) return;
	if ( (damaged = get_eq_char(victim,WEAR_FACE) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevents you from breaking your jaw.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevents $n from breaking $s jaw.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p falls broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p falls broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_HEAD(victim,BROKEN_JAW))
	    SET_BIT(victim->loc_hp[0],BROKEN_JAW);
	else return;
	act("Your skillful blow breaks $N's jaw!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow breaks $N's jaw!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow breaks your jaw!",ch,NULL,victim,TO_VICT);
	return;
    }
    else if (critical == 6)
    {
	if (IS_ARM_L(victim,LOST_ARM)) return;
	if ( (damaged = get_eq_char(victim,WEAR_ARMS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from loosing your left arm.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from loosing $s left arm.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_ARM_L(victim,LOST_ARM))
	    SET_BIT(victim->loc_hp[2],LOST_ARM);
	else return;
	if (!IS_BLEEDING(victim,BLEEDING_ARM_L))
	    SET_BIT(victim->loc_hp[6],BLEEDING_ARM_L);
	if (IS_BLEEDING(victim,BLEEDING_HAND_L))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_L);
	act("Your skillful blow cuts off $N's left arm!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow cuts off $N's left arm!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow cuts off your left arm!",ch,NULL,victim,TO_VICT);
	make_part(victim,"arm");
	if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
	{
	    if ((obj = get_eq_char( victim, WEAR_ARMS )) != NULL)
		take_item(victim,obj);
	}
	if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_WRIST_L )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_FINGER_L )) != NULL)
	    take_item(victim,obj);
	return;
    }
    else if (critical == 7)
    {
	if (IS_ARM_R(victim,LOST_ARM)) return;
	if ( (damaged = get_eq_char(victim,WEAR_ARMS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from loosing your right arm.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from loosing $s right arm.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_ARM_R(victim,LOST_ARM))
	    SET_BIT(victim->loc_hp[3],LOST_ARM);
	else return;
	if (!IS_BLEEDING(victim,BLEEDING_ARM_R))
	    SET_BIT(victim->loc_hp[6],BLEEDING_ARM_R);
	if (IS_BLEEDING(victim,BLEEDING_HAND_R))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_HAND_R);
	act("Your skillful blow cuts off $N's right arm!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow cuts off $N's right arm!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow cuts off your right arm!",ch,NULL,victim,TO_VICT);
	make_part(victim,"arm");
	if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
	{
	    if ((obj = get_eq_char( victim, WEAR_ARMS )) != NULL)
		take_item(victim,obj);
	}
	if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_WRIST_R )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_FINGER_R )) != NULL)
	    take_item(victim,obj);
	return;
    }
    else if (critical == 8)
    {
	if (IS_ARM_L(victim,LOST_ARM) || IS_ARM_L(victim,BROKEN_ARM)) return;
	if ( (damaged = get_eq_char(victim,WEAR_ARMS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from breaking your left arm.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from breaking $s left arm.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_ARM_L(victim,BROKEN_ARM) && !IS_ARM_L(victim,LOST_ARM))
	    SET_BIT(victim->loc_hp[2],BROKEN_ARM);
	else return;
	act("Your skillful blow breaks $N's left arm!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow breaks $N's left arm!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow breaks your left arm!",ch,NULL,victim,TO_VICT);
	if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL)
	    take_item(victim,obj);
	return;
    }
    else if (critical == 9)
    {
	if (IS_ARM_R(victim,LOST_ARM) || IS_ARM_R(victim,BROKEN_ARM)) return;
	if ( (damaged = get_eq_char(victim,WEAR_ARMS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from breaking your right arm.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from breaking $s right arm.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_ARM_R(victim,BROKEN_ARM) && !IS_ARM_R(victim,LOST_ARM))
	    SET_BIT(victim->loc_hp[3],BROKEN_ARM);
	else return;
	act("Your skillful blow breaks $N's right arm!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow breaks $N's right arm!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow breaks your right arm!",ch,NULL,victim,TO_VICT);
	if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
	    take_item(victim,obj);
	return;
    }
    else if (critical == 10)
    {
	if (IS_ARM_L(victim,LOST_HAND) || IS_ARM_L(victim,LOST_ARM)) return;
	if ( (damaged = get_eq_char(victim,WEAR_HANDS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from loosing your left hand.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from loosing $s left hand.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_ARM_L(victim,LOST_HAND) && !IS_ARM_L(victim,LOST_ARM))
	    SET_BIT(victim->loc_hp[2],LOST_HAND);
	else return;
	if (IS_BLEEDING(victim,BLEEDING_ARM_L))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_ARM_L);
	if (!IS_BLEEDING(victim,BLEEDING_HAND_L))
	    SET_BIT(victim->loc_hp[6],BLEEDING_HAND_L);
	act("Your skillful blow cuts off $N's left hand!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow cuts off $N's left hand!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow cuts off your left hand!",ch,NULL,victim,TO_VICT);
	make_part(victim,"hand");
	if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_WRIST_L )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_FINGER_L )) != NULL)
	    take_item(victim,obj);
	return;
    }
    else if (critical == 11)
    {
	if (IS_ARM_R(victim,LOST_HAND) || IS_ARM_R(victim,LOST_ARM)) return;
	if ( (damaged = get_eq_char(victim,WEAR_HANDS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from loosing your right hand.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from loosing $s right hand.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_ARM_R(victim,LOST_HAND) && !IS_ARM_R(victim,LOST_ARM))
	    SET_BIT(victim->loc_hp[3],LOST_HAND);
	else return;
	if (IS_BLEEDING(victim,BLEEDING_ARM_R))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_ARM_R);
	if (!IS_BLEEDING(victim,BLEEDING_HAND_R))
	    SET_BIT(victim->loc_hp[6],BLEEDING_HAND_R);
	act("Your skillful blow cuts off $N's right hand!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow cuts off $N's right hand!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow cuts off your right hand!",ch,NULL,victim,TO_VICT);
	make_part(victim,"hand");
	if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_HANDS )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_WRIST_R )) != NULL)
	    take_item(victim,obj);
	if ((obj = get_eq_char( victim, WEAR_FINGER_R )) != NULL)
	    take_item(victim,obj);
	return;
    }
    else if (critical == 12)
    {
	if (IS_ARM_L(victim,LOST_ARM)) return;
	if (IS_ARM_L(victim,LOST_HAND)) return;
	if (IS_ARM_L(victim,LOST_THUMB) && IS_ARM_L(victim,LOST_FINGER_I) &&
	IS_ARM_L(victim,LOST_FINGER_M) && IS_ARM_L(victim,LOST_FINGER_R) &&
	IS_ARM_L(victim,LOST_FINGER_L)) return;
	if ( (damaged = get_eq_char(victim,WEAR_HANDS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from loosing some fingers from your left hand.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from loosing some fingers from $s left hand.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	count = 0;
	count2 = 0;
	if (!IS_ARM_L(victim,LOST_THUMB) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[2],LOST_THUMB);count2 += 1;
	    make_part(victim,"thumb");}
	if (!IS_ARM_L(victim,LOST_FINGER_I) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[2],LOST_FINGER_I);count += 1;
	    make_part(victim,"index");}
	if (!IS_ARM_L(victim,LOST_FINGER_M) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[2],LOST_FINGER_M);count += 1;
	    make_part(victim,"middle");}
	if (!IS_ARM_L(victim,LOST_FINGER_R) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[2],LOST_FINGER_R);count += 1;
	    make_part(victim,"ring");
	    if ((obj = get_eq_char( victim, WEAR_FINGER_L )) != NULL)
	        take_item(victim,obj);}
	if (!IS_ARM_L(victim,LOST_FINGER_L) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[2],LOST_FINGER_L);count += 1;
	    make_part(victim,"little");}
	if (count == 1) sprintf(buf2,"finger");
	else            sprintf(buf2,"fingers");
	if (count > 0 && count2 > 0)
	{
	    sprintf(buf,"Your skillful blow cuts off %d %s and the thumb from $N's left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow cuts off %d %s and the thumb from $N's left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow cuts off %d %s and the thumb from your left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	else if (count > 0)
	{
	    sprintf(buf,"Your skillful blow cuts off %d %s from $N's left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow cuts off %d %s from $N's left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow cuts off %d %s from your left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	else if (count2 > 0)
	{
	    sprintf(buf,"Your skillful blow cuts off the thumb from $N's left hand.");
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow cuts off the thumb from $N's left hand.");
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow cuts off the thumb from your left hand.");
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	return;
    }
    else if (critical == 13)
    {
	if (IS_ARM_L(victim,LOST_ARM)) return;
	if (IS_ARM_L(victim,LOST_HAND)) return;
	if ((IS_ARM_L(victim,LOST_THUMB) || IS_ARM_L(victim,BROKEN_THUMB)) &&
	    (IS_ARM_L(victim,LOST_FINGER_I) || IS_ARM_L(victim,BROKEN_FINGER_I)) &&
	    (IS_ARM_L(victim,LOST_FINGER_M) || IS_ARM_L(victim,BROKEN_FINGER_M)) &&
	    (IS_ARM_L(victim,LOST_FINGER_R) || IS_ARM_L(victim,BROKEN_FINGER_R)) &&
	    (IS_ARM_L(victim,LOST_FINGER_L) || IS_ARM_L(victim,BROKEN_FINGER_L)))
		return;
	if ( (damaged = get_eq_char(victim,WEAR_HANDS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from breaking some fingers on your left hand.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from breaking some fingers on $s left hand.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	count = 0;
	count2 = 0;
	if (IS_ARM_L(victim,LOST_ARM)) return;
	if (IS_ARM_L(victim,LOST_HAND)) return;

	if (!IS_ARM_L(victim,BROKEN_THUMB) && !IS_ARM_L(victim,LOST_THUMB) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[2],BROKEN_THUMB);count2 += 1;}
	if (!IS_ARM_L(victim,BROKEN_FINGER_I) && !IS_ARM_L(victim,LOST_FINGER_I) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[2],BROKEN_FINGER_I);count += 1;}
	if (!IS_ARM_L(victim,BROKEN_FINGER_M) && !IS_ARM_L(victim,LOST_FINGER_M) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[2],BROKEN_FINGER_M);count += 1;}
	if (!IS_ARM_L(victim,BROKEN_FINGER_R) && !IS_ARM_L(victim,LOST_FINGER_R) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[2],BROKEN_FINGER_R);count += 1;}
	if (!IS_ARM_L(victim,BROKEN_FINGER_L) && !IS_ARM_L(victim,LOST_FINGER_L) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[2],BROKEN_FINGER_L);count += 1;}
	if (count == 1) sprintf(buf2,"finger");
	else            sprintf(buf2,"fingers");
	if (count > 0 && count2 > 0)
	{
	    sprintf(buf,"Your skillful breaks %d %s and the thumb on $N's left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow breaks %d %s and the thumb on $N's left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow breaks %d %s and the thumb on your left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	else if (count > 0)
	{
	    sprintf(buf,"Your skillful blow breaks %d %s on $N's left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow breaks %d %s on $N's left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow breaks %d %s on your left hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	else if (count2 > 0)
	{
	    sprintf(buf,"Your skillful blow breaks the thumb on $N's left hand.");
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow breaks the thumb on $N's left hand.");
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow breaks the thumb on your left hand.");
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_HOLD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	return;
    }
    else if (critical == 14)
    {
	if (IS_ARM_R(victim,LOST_ARM)) return;
	if (IS_ARM_R(victim,LOST_HAND)) return;
	if (IS_ARM_R(victim,LOST_THUMB) && IS_ARM_R(victim,LOST_FINGER_I) &&
	IS_ARM_R(victim,LOST_FINGER_M) && IS_ARM_R(victim,LOST_FINGER_R) &&
	IS_ARM_R(victim,LOST_FINGER_L)) return;
	if ( (damaged = get_eq_char(victim,WEAR_HANDS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from loosing some fingers from your right hand.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from loosing some fingers from $s right hand.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	count = 0;
	count2 = 0;
	if (IS_ARM_R(victim,LOST_ARM)) return;
	if (IS_ARM_R(victim,LOST_HAND)) return;

	if (!IS_ARM_R(victim,LOST_THUMB) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[3],LOST_THUMB);count2 += 1;
	    make_part(victim,"thumb");}
	if (!IS_ARM_R(victim,LOST_FINGER_I) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[3],LOST_FINGER_I);count += 1;
	    make_part(victim,"index");}
	if (!IS_ARM_R(victim,LOST_FINGER_M) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[3],LOST_FINGER_M);count += 1;
	    make_part(victim,"middle");}
	if (!IS_ARM_R(victim,LOST_FINGER_R) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[3],LOST_FINGER_R);count += 1;
	    make_part(victim,"ring");
	    if ((obj = get_eq_char( victim, WEAR_FINGER_R )) != NULL)
	        take_item(victim,obj);}
	if (!IS_ARM_R(victim,LOST_FINGER_L) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[3],LOST_FINGER_L);count += 1;
	    make_part(victim,"little");}
	if (count == 1) sprintf(buf2,"finger");
	else            sprintf(buf2,"fingers");
	if (count > 0 && count2 > 0)
	{
	    sprintf(buf,"Your skillful blow cuts off %d %s and the thumb from $N's right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow cuts off %d %s and the thumb from $N's right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow cuts off %d %s and the thumb from your right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	else if (count > 0)
	{
	    sprintf(buf,"Your skillful blow cuts off %d %s from $N's right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow cuts off %d %s from $N's right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow cuts off %d %s from your right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	else if (count2 > 0)
	{
	    sprintf(buf,"Your skillful blow cuts off the thumb from $N's right hand.");
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow cuts off the thumb from $N's right hand.");
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow cuts off the thumb from your right hand.");
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	return;
    }
    else if (critical == 15)
    {
	if (IS_ARM_R(victim,LOST_ARM)) return;
	if (IS_ARM_R(victim,LOST_HAND)) return;
	if ((IS_ARM_R(victim,LOST_THUMB) || IS_ARM_R(victim,BROKEN_THUMB)) &&
	    (IS_ARM_R(victim,LOST_FINGER_I) || IS_ARM_R(victim,BROKEN_FINGER_I)) &&
	    (IS_ARM_R(victim,LOST_FINGER_M) || IS_ARM_R(victim,BROKEN_FINGER_M)) &&
	    (IS_ARM_R(victim,LOST_FINGER_R) || IS_ARM_R(victim,BROKEN_FINGER_R)) &&
	    (IS_ARM_R(victim,LOST_FINGER_L) || IS_ARM_R(victim,BROKEN_FINGER_L)))
		return;
	if ( (damaged = get_eq_char(victim,WEAR_HANDS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from breaking some fingers on your right hand.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from breaking some fingers on $s right hand.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	count = 0;
	count2 = 0;
	if (IS_ARM_R(victim,LOST_ARM)) return;
	if (IS_ARM_R(victim,LOST_HAND)) return;

	if (!IS_ARM_R(victim,BROKEN_THUMB) && !IS_ARM_R(victim,LOST_THUMB) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[3],BROKEN_THUMB);count2 += 1;}
	if (!IS_ARM_R(victim,BROKEN_FINGER_I) && !IS_ARM_R(victim,LOST_FINGER_I) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[3],BROKEN_FINGER_I);count += 1;}
	if (!IS_ARM_R(victim,BROKEN_FINGER_M) && !IS_ARM_R(victim,LOST_FINGER_M) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[3],BROKEN_FINGER_M);count += 1;}
	if (!IS_ARM_R(victim,BROKEN_FINGER_R) && !IS_ARM_R(victim,LOST_FINGER_R) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[3],BROKEN_FINGER_R);count += 1;}
	if (!IS_ARM_R(victim,BROKEN_FINGER_L) && !IS_ARM_R(victim,LOST_FINGER_L) && number_percent() < 25)
	    {SET_BIT(victim->loc_hp[3],BROKEN_FINGER_L);count += 1;}
	if (count == 1) sprintf(buf2,"finger");
	else            sprintf(buf2,"fingers");
	if (count > 0 && count2 > 0)
	{
	    sprintf(buf,"Your skillful breaks %d %s and the thumb on $N's right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow breaks %d %s and the thumb on $N's right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow breaks %d %s and the thumb on your right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	else if (count > 0)
	{
	    sprintf(buf,"Your skillful blow breaks %d %s on $N's right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow breaks %d %s on $N's right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow breaks %d %s on your right hand.",count,buf2);
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	else if (count2 > 0)
	{
	    sprintf(buf,"Your skillful blow breaks the thumb on $N's right hand.");
	    act(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's skillful blow breaks the thumb on $N's right hand.");
	    act(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's skillful blow breaks the thumb on your right hand.");
	    act(buf,ch,NULL,victim,TO_VICT);
	    if ((obj = get_eq_char( victim, WEAR_WIELD )) != NULL)
		take_item(victim,obj);
	    return;
	}
	return;
    }
    else if (critical == 16)
    {
	if (IS_LEG_L(victim,LOST_LEG)) return;
	if ( (damaged = get_eq_char(victim,WEAR_LEGS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from loosing your left leg.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from loosing $s left leg.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_LEG_L(victim,LOST_LEG))
	    SET_BIT(victim->loc_hp[4],LOST_LEG);
	else return;
	if (!IS_BLEEDING(victim,BLEEDING_LEG_L))
	    SET_BIT(victim->loc_hp[6],BLEEDING_LEG_L);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_L))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_FOOT_L);
	act("Your skillful blow cuts off $N's left leg!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow cuts off $N's left leg!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow cuts off your left leg!",ch,NULL,victim,TO_VICT);
	make_part(victim,"leg");
	if (IS_LEG_L(victim,LOST_LEG) && IS_LEG_R(victim,LOST_LEG))
	{
	    if ((obj = get_eq_char( victim, WEAR_LEGS )) != NULL)
		take_item(victim,obj);
	}
	if ((obj = get_eq_char( victim, WEAR_FEET )) != NULL)
	    take_item(victim,obj);
	return;
    }
    else if (critical == 17)
    {
	if (IS_LEG_R(victim,LOST_LEG)) return;
	if ( (damaged = get_eq_char(victim,WEAR_LEGS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from loosing your right leg.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from loosing $s right leg.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_LEG_R(victim,LOST_LEG))
	    SET_BIT(victim->loc_hp[5],LOST_LEG);
	else return;
	if (!IS_BLEEDING(victim,BLEEDING_LEG_R))
	    SET_BIT(victim->loc_hp[6],BLEEDING_LEG_R);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_R))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_FOOT_R);
	act("Your skillful blow cuts off $N's right leg!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow cuts off $N's right leg!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow cuts off your right leg!",ch,NULL,victim,TO_VICT);
	make_part(victim,"leg");
	if (IS_LEG_L(victim,LOST_LEG) && IS_LEG_R(victim,LOST_LEG))
	{
	    if ((obj = get_eq_char( victim, WEAR_LEGS )) != NULL)
		take_item(victim,obj);
	}
	if ((obj = get_eq_char( victim, WEAR_FEET )) != NULL)
	    take_item(victim,obj);
	return;
    }
    else if (critical == 18)
    {
	if (IS_LEG_L(victim,BROKEN_LEG) || IS_LEG_L(victim,LOST_LEG)) return;
	if ( (damaged = get_eq_char(victim,WEAR_LEGS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from breaking your left leg.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from breaking $s left leg.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_LEG_L(victim,BROKEN_LEG) && !IS_LEG_L(victim,LOST_LEG))
	    SET_BIT(victim->loc_hp[4],BROKEN_LEG);
	else return;
	act("Your skillful blow breaks $N's left leg!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow breaks $N's left leg!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow breaks your left leg!",ch,NULL,victim,TO_VICT);
	return;
    }
    else if (critical == 19)
    {
	if (IS_LEG_R(victim,BROKEN_LEG) || IS_LEG_R(victim,LOST_LEG)) return;
	if ( (damaged = get_eq_char(victim,WEAR_LEGS) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from breaking your right leg.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from breaking $s right leg.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_LEG_R(victim,BROKEN_LEG) && !IS_LEG_R(victim,LOST_LEG))
	    SET_BIT(victim->loc_hp[5],BROKEN_LEG);
	else return;
	act("Your skillful blow breaks $N's right leg!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow breaks $N's right leg!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow breaks your right leg!",ch,NULL,victim,TO_VICT);
	return;
    }
    else if (critical == 20)
    {
	if (IS_LEG_L(victim,LOST_LEG) || IS_LEG_L(victim,LOST_FOOT)) return;
	if ( (damaged = get_eq_char(victim,WEAR_FEET) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from loosing your left foot.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from loosing $s left foot.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_LEG_L(victim,LOST_LEG) && !IS_LEG_L(victim,LOST_FOOT))
	    SET_BIT(victim->loc_hp[4],LOST_FOOT);
	else return;
	if (IS_BLEEDING(victim,BLEEDING_LEG_L))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_LEG_L);
	if (!IS_BLEEDING(victim,BLEEDING_FOOT_L))
	    SET_BIT(victim->loc_hp[6],BLEEDING_FOOT_L);
	act("Your skillful blow cuts off $N's left foot!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow cuts off $N's left foot!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow cuts off your left foot!",ch,NULL,victim,TO_VICT);
	make_part(victim,"foot");
	if ((obj = get_eq_char( victim, WEAR_FEET )) != NULL)
	    take_item(victim,obj);
	return;
    }
    else if (critical == 21)
    {
	if (IS_LEG_R(victim,LOST_LEG) || IS_LEG_R(victim,LOST_FOOT)) return;
	if ( (damaged = get_eq_char(victim,WEAR_FEET) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevent you from loosing your right foot.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevent $n from loosing $s right foot.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (!IS_LEG_R(victim,LOST_LEG) && !IS_LEG_R(victim,LOST_FOOT))
	    SET_BIT(victim->loc_hp[5],LOST_FOOT);
	else return;
	if (IS_BLEEDING(victim,BLEEDING_LEG_R))
	    REMOVE_BIT(victim->loc_hp[6],BLEEDING_LEG_R);
	if (!IS_BLEEDING(victim,BLEEDING_FOOT_R))
	    SET_BIT(victim->loc_hp[6],BLEEDING_FOOT_R);
	act("Your skillful blow cuts off $N's right foot!",ch,NULL,victim,TO_CHAR);
	act("$n's skillful blow cuts off $N's right foot!",ch,NULL,victim,TO_NOTVICT);
	act("$n's skillful blow cuts off your right foot!",ch,NULL,victim,TO_VICT);
	make_part(victim,"foot");
	if ((obj = get_eq_char( victim, WEAR_FEET )) != NULL)
	    take_item(victim,obj);
	return;
    }
    else if (critical == 22)
    {
	int bodyloc = 0;
	int broken = number_range(1,3);
	if (IS_BODY(victim,BROKEN_RIBS_1 )) bodyloc += 1;
	if (IS_BODY(victim,BROKEN_RIBS_2 )) bodyloc += 2;
	if (IS_BODY(victim,BROKEN_RIBS_4 )) bodyloc += 4;
	if (IS_BODY(victim,BROKEN_RIBS_8 )) bodyloc += 8;
	if (IS_BODY(victim,BROKEN_RIBS_16)) bodyloc += 16;
	if (bodyloc >= 24) return;

	if ( (damaged = get_eq_char(victim,WEAR_BODY) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevents you from breaking some ribs.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevents $n from breaking some ribs.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (IS_BODY(victim,BROKEN_RIBS_1 ))
	    REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_1);
	if (IS_BODY(victim,BROKEN_RIBS_2 ))
	    REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_2);
	if (IS_BODY(victim,BROKEN_RIBS_4 ))
	    REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_4);
	if (IS_BODY(victim,BROKEN_RIBS_8 ))
	    REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_8);
	if (IS_BODY(victim,BROKEN_RIBS_16))
	    REMOVE_BIT(victim->loc_hp[1],BROKEN_RIBS_16);
	if (bodyloc+broken > 24) broken -= 1;
	if (bodyloc+broken > 24) broken -= 1;
	bodyloc += broken;
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
	sprintf(buf,"Your skillful blow breaks %d of $N's ribs!",broken);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"$n's skillful blow breaks %d of $N's ribs!",broken);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"$n's skillful blow breaks %d of your ribs!",broken);
	act(buf,ch,NULL,victim,TO_VICT);
	return;
    }
    else if (critical == 23)
    {
	int bodyloc = 0;
	int broken = number_range(1,3);
	if (IS_HEAD(victim,LOST_TOOTH_1 )) bodyloc += 1;
	if (IS_HEAD(victim,LOST_TOOTH_2 )) bodyloc += 2;
	if (IS_HEAD(victim,LOST_TOOTH_4 )) bodyloc += 4;
	if (IS_HEAD(victim,LOST_TOOTH_8 )) bodyloc += 8;
	if (IS_HEAD(victim,LOST_TOOTH_16)) bodyloc += 16;
	if (bodyloc >= 28) return;

	if ( (damaged = get_eq_char(victim,WEAR_FACE) ) != NULL &&
		damaged->toughness > 0)
	{
	    act("$p prevents you from loosing some teeth.",victim,damaged,NULL,TO_CHAR);
	    act("$p prevents $n from loosing some teeth.",victim,damaged,NULL,TO_ROOM);
	    if (dam - damaged->toughness < 0) return;
	    if ((dam - damaged->toughness > damaged->resistance))
	    	damaged->condition -= damaged->resistance;
	    else
	    	damaged->condition -= (dam - damaged->toughness);
	    if (damaged->condition < 1)
	    {
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_CHAR);
	    	act("$p fall broken to the ground.",ch,damaged,NULL,TO_ROOM);
	    	obj_from_char(damaged);
	    	extract_obj(damaged);
	    }
	    return;
	}
	if (IS_HEAD(victim,LOST_TOOTH_1 ))
	    REMOVE_BIT(victim->loc_hp[LOC_HEAD],LOST_TOOTH_1);
	if (IS_HEAD(victim,LOST_TOOTH_2 ))
	    REMOVE_BIT(victim->loc_hp[LOC_HEAD],LOST_TOOTH_2);
	if (IS_HEAD(victim,LOST_TOOTH_4 ))
	    REMOVE_BIT(victim->loc_hp[LOC_HEAD],LOST_TOOTH_4);
	if (IS_HEAD(victim,LOST_TOOTH_8 ))
	    REMOVE_BIT(victim->loc_hp[LOC_HEAD],LOST_TOOTH_8);
	if (IS_HEAD(victim,LOST_TOOTH_16))
	    REMOVE_BIT(victim->loc_hp[LOC_HEAD],LOST_TOOTH_16);
	if (bodyloc+broken > 28) broken -= 1;
	if (bodyloc+broken > 28) broken -= 1;
	bodyloc += broken;
	if (bodyloc >= 16) {bodyloc -= 16;
	    SET_BIT(victim->loc_hp[LOC_HEAD],LOST_TOOTH_16);}
	if (bodyloc >= 8) {bodyloc -= 8;
	    SET_BIT(victim->loc_hp[LOC_HEAD],LOST_TOOTH_8);}
	if (bodyloc >= 4) {bodyloc -= 4;
	    SET_BIT(victim->loc_hp[LOC_HEAD],LOST_TOOTH_4);}
	if (bodyloc >= 2) {bodyloc -= 2;
	    SET_BIT(victim->loc_hp[LOC_HEAD],LOST_TOOTH_2);}
	if (bodyloc >= 1) {bodyloc -= 1;
	    SET_BIT(victim->loc_hp[LOC_HEAD],LOST_TOOTH_1);}
	sprintf(buf,"Your skillful blow knocks out %d of $N's teeth!",broken);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"$n's skillful blow knocks out %d of $N's teeth!",broken);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"$n's skillful blow knocks out %d of your teeth!",broken);
	act(buf,ch,NULL,victim,TO_VICT);
	if (broken >= 1) make_part(victim,"tooth");
	if (broken >= 2) make_part(victim,"tooth");
	if (broken >= 3) make_part(victim,"tooth");
	return;
    }
    return;
}

void take_item( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if (obj == NULL) return;
    obj_from_char(obj);
    obj_to_room(obj,ch->in_room);
    act("You wince in pain and $p falls to the ground.",ch,obj,NULL,TO_CHAR);
    act("$n winces in pain and $p falls to the ground.",ch,obj,NULL,TO_ROOM);
    return;
}

void do_werewolf( CHAR_DATA *ch, char *argument )
{
    char       buf[MAX_INPUT_LENGTH];
    char       arg[MAX_INPUT_LENGTH];
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA  *obj;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_SET(ch->act, PLR_WEREWOLF)) return;
    if (IS_SET(ch->act, PLR_WOLFMAN)) return;
    SET_BIT(ch->act, PLR_WOLFMAN);

    send_to_char("You throw back your head and howl with rage!\n\r",ch);
    act("$n throws back $s head and howls with rage!.", ch, NULL, NULL, TO_ROOM);
    send_to_char("Coarse dark hair sprouts from your body.\n\r",ch);
    act("Coarse dark hair sprouts from $n's body.",ch,NULL,NULL,TO_ROOM);
    if (!IS_VAMPAFF(ch,VAM_NIGHTSIGHT))
    {
	send_to_char("Your eyes start glowing red.\n\r",ch);
	act("$n's eyes start glowing red.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
    }
    if (!IS_VAMPAFF(ch,VAM_FANGS))
    {
	send_to_char("A pair of long fangs extend from your mouth.\n\r",ch);
	act("A pair of long fangs extend from $n's mouth.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->vampaff, VAM_FANGS);
    }
    if (!IS_VAMPAFF(ch,VAM_CLAWS))
    {
	send_to_char("Razor sharp talons extend from your fingers.\n\r",ch);
	act("Razor sharp talons extend from $n's fingers.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->vampaff, VAM_CLAWS);
    }
    if ((obj = get_eq_char(ch,WEAR_WIELD)) != NULL)
    {
	act("$p drops from your right hand.",ch,NULL,NULL,TO_CHAR);
	act("$p drops from $n's right hand.",ch,NULL,NULL,TO_ROOM);
	obj_from_char(obj);
	obj_to_room(obj, ch->in_room);
    }
    if ((obj = get_eq_char(ch,WEAR_HOLD)) != NULL)
    {
	act("$p drops from your left hand.",ch,NULL,NULL,TO_CHAR);
	act("$p drops from $n's left hand.",ch,NULL,NULL,TO_ROOM);
	obj_from_char(obj);
	obj_to_room(obj, ch->in_room);
    }
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->vampaff, VAM_DISGUISED);
    sprintf(buf,"%s the werewolf",ch->name);
    free_string(ch->morph);
    ch->morph = str_dup(buf);
    ch->pcdata->wolf += 25;
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( ch == vch )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( can_see( ch, vch) )
	    {
		multi_hit( ch, vch, TYPE_UNDEFINED );
		multi_hit( ch, vch, TYPE_UNDEFINED );
		multi_hit( ch, vch, TYPE_UNDEFINED );
	    }
	}
	else if (IS_NPC(vch)) continue;
	else if (vch->in_room->area == ch->in_room->area)
	    send_to_char("You hear a fearsome howl close by!\n\r", vch);
	else
	    send_to_char("You hear a fearsome howl far off in the distance!\n\r", vch);
    }
    return;
}

void do_unwerewolf( CHAR_DATA *ch, char *argument )
{
    char       arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if (!IS_SET(ch->act, PLR_WEREWOLF)) return;
    if (!IS_SET(ch->act, PLR_WOLFMAN)) return;
    REMOVE_BIT(ch->act, PLR_WOLFMAN);
    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
    free_string(ch->morph);
    ch->morph = str_dup("");
    if (IS_VAMPAFF(ch,VAM_CLAWS))
    {
	send_to_char("Your talons slide back into your fingers.\n\r",ch);
	act("$n's talons slide back into $s fingers.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->vampaff, VAM_CLAWS);
    }
    if (IS_VAMPAFF(ch,VAM_FANGS))
    {
	send_to_char("Your fangs slide back into your mouth.\n\r",ch);
	act("$n's fangs slide back into $s mouth.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->vampaff, VAM_FANGS);
    }
    if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT))
    {
	send_to_char("The red glow in your eyes fades.\n\r",ch);
	act("The red glow in $n's eyes fades.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->vampaff, VAM_NIGHTSIGHT);
    }
    send_to_char("Your coarse hair shrinks back into your body.\n\r",ch);
    act("$n's coarse hair shrinks back into $s body.",ch,NULL,NULL,TO_ROOM);
    ch->pcdata->wolf -= 25;
    if (ch->pcdata->wolf < 0) ch->pcdata->wolf = 0;
    return;
}

void do_favour( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg1 [MAX_INPUT_LENGTH];
    char      arg2 [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( IS_NPC(ch) ) return;
    if ( !IS_SET(ch->act,PLR_VAMPIRE) && (IS_EXTRA(ch,EXTRA_PRINCE) || ch->vampgen != 2) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax is: favour <target> <prince/sire>\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Not on yourself!\n\r", ch );
	return;
    }

    if ( !IS_SET(victim->act,PLR_VAMPIRE) )
    {
	send_to_char( "But they are not a vampire!\n\r", ch );
	return;
    }

    if ( str_cmp(victim->clan,ch->clan) )
    {
	send_to_char( "You can only grant your favour to someone in your clan.\n\r", ch );
	return;
    }

    if ( ch->vampgen >= victim->vampgen )
    {
	send_to_char( "You can only grant your favour to someone of a lower generation.\n\r", ch );
	return;
    }

    if ( victim->vampgen > 6 )
    {
	send_to_char( "There is no point in doing that, as they are unable to sire any childer.\n\r", ch );
	return;
    }

    if ( !str_cmp(arg2,"prince") && ch->vampgen == 2)
    {
	if (IS_EXTRA(victim,EXTRA_PRINCE)) {
	act("You remove $N's prince privilages!",ch,NULL,victim,TO_CHAR);
	act("$n removes $N's prince privilages!",ch,NULL,victim,TO_NOTVICT);
	act("$n removes your prince privilages!",ch,NULL,victim,TO_VICT);
	if (IS_EXTRA(victim,EXTRA_SIRE)) REMOVE_BIT(victim->extra,EXTRA_SIRE);
	REMOVE_BIT(victim->extra,EXTRA_PRINCE);return;}
	act("You make $N a prince!",ch,NULL,victim,TO_CHAR);
	act("$n has made $N a prince!",ch,NULL,victim,TO_NOTVICT);
	act("$n has made you a prince!",ch,NULL,victim,TO_VICT);
	SET_BIT(victim->extra,EXTRA_PRINCE);
	if (IS_EXTRA(victim,EXTRA_SIRE)) REMOVE_BIT(victim->extra,EXTRA_SIRE);
	return;
    }
    else if ( !str_cmp(arg2,"sire") && (ch->vampgen == 2 || IS_EXTRA(ch,EXTRA_PRINCE)))
    {
	if (IS_EXTRA(victim,EXTRA_SIRE)) {
	act("You remove $N's permission to sire a childe!",ch,NULL,victim,TO_CHAR);
	act("$n has removed $N's permission to sire a childe!",ch,NULL,victim,TO_NOTVICT);
	act("$n has remove your permission to sire a childe!",ch,NULL,victim,TO_VICT);
	REMOVE_BIT(victim->extra,EXTRA_SIRE);return;}
	act("You grant $N permission to sire a childe!",ch,NULL,victim,TO_CHAR);
	act("$n has granted $N permission to sire a childe!",ch,NULL,victim,TO_NOTVICT);
	act("$n has granted you permission to sire a childe!",ch,NULL,victim,TO_VICT);
	SET_BIT(victim->extra,EXTRA_SIRE);
	return;
    }
    else send_to_char( "You are unable to grant that sort of favour.\n\r", ch );
    return;
}

