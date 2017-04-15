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
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
int	hit_gain	args( ( CHAR_DATA *ch ) );
int	mana_gain	args( ( CHAR_DATA *ch ) );
int	move_gain	args( ( CHAR_DATA *ch ) );
void	mobile_update	args( ( void ) );
void	weather_update	args( ( void ) );
void	char_update	args( ( void ) );
void	obj_update	args( ( void ) );
void	aggr_update	args( ( void ) );



/*
 * Advancement stuff.
 */
void advance_level( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;

    sprintf( buf, "the %s",
	title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
    set_title( ch, buf );

    add_hp	= con_app[get_curr_con(ch)].hitp + number_range(
		    class_table[ch->class].hp_min,
		    class_table[ch->class].hp_max );
    add_mana	= class_table[ch->class].fMana
		    ? number_range(2, (2*get_curr_int(ch)+get_curr_wis(ch))/8)
		    : 0;
    add_move	= number_range( 5, (get_curr_con(ch)+get_curr_dex(ch))/4 );
    add_prac	= wis_app[get_curr_wis(ch)].practice;

    add_hp	= UMAX(  1, add_hp   );
    add_mana	= UMAX(  0, add_mana );
    add_move	= UMAX( 10, add_move );

    ch->max_hit 	+= (add_hp);
    ch->max_mana	+= (add_mana);
    ch->max_move	+= (add_move);
    ch->practice	+= add_prac;

    if ( !IS_NPC(ch) )
	REMOVE_BIT( ch->act, PLR_BOUGHT_PET );

    sprintf( buf,
	"Your gain is: %d/%d hp, %d/%d m, %d/%d mv %d/%d prac.\n\r",
	add_hp,		ch->max_hit,
	add_mana,	ch->max_mana,
	add_move,	ch->max_move,
	add_prac,	ch->practice
	);
    send_to_char( buf, ch );
    return;
}   



void gain_exp( CHAR_DATA *ch, int gain )
{
    if ( IS_NPC(ch) && ch->mount != NULL && !IS_NPC(ch->mount))
	ch->mount->exp += gain;

    if ( !IS_NPC(ch) && ch->level <= LEVEL_HERO )
	ch->exp += gain;
    return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;
    int conamount;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	gain = number_range( 10, 20 );

	if ((conamount = (get_curr_con(ch)+1)) > 1)
	{
	    switch ( ch->position )
	    {
		case POS_SLEEPING: gain *= conamount;		break;
		case POS_RESTING:  gain *= conamount / 2;	break;
	    }
	}

	if ( ch->pcdata->condition[COND_FULL]   == 0 && !IS_HERO(ch) &&
		!IS_SET(ch->act, PLR_VAMPIRE) )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 && !IS_HERO(ch) )
	    gain /= 2;

    }

    if ( IS_AFFECTED(ch, AFF_POISON) || IS_AFFECTED(ch, AFF_FLAMING) )
	gain /= 4;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_VAMPIRE)) gain = 0;

    return UMIN(gain, ch->max_hit - ch->hit);
}



int mana_gain( CHAR_DATA *ch )
{
    int gain;
    int intamount;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	gain = UMIN( 5, ch->level / 2 );

	gain = number_range( 10, 20 );

	if ((intamount = (get_curr_int(ch)+1)) > 1)
	{
	    switch ( ch->position )
	    {
		case POS_SLEEPING: gain *= intamount;		break;
		case POS_RESTING:  gain *= intamount / 2;	break;
	    }
	}

        if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_VAMPIRE)) gain = 0;

	if ( !IS_HERO(ch) && ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;

    }

    if ( IS_AFFECTED( ch, AFF_POISON ) || IS_AFFECTED(ch, AFF_FLAMING) )
	gain /= 4;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_VAMPIRE)) gain = 0;

    return UMIN(gain, ch->max_mana - ch->mana);
}



int move_gain( CHAR_DATA *ch )
{
    int gain;
    int dexamount;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	gain = number_range( 10, 20 );

	if ((dexamount = (get_curr_dex(ch)+1)) > 1)
	{
	    switch ( ch->position )
	    {
		case POS_SLEEPING: gain *= dexamount;		break;
		case POS_RESTING:  gain *= dexamount / 2;	break;
	    }
	}

    	if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_VAMPIRE)) gain = 0;

	if ( !IS_HERO(ch) && ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;
    }

    if ( IS_AFFECTED(ch, AFF_POISON) || IS_AFFECTED(ch, AFF_FLAMING) )
	gain /= 4;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_VAMPIRE)) gain = 0;

    return UMIN(gain, ch->max_move - ch->move);
}



void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;

    if ( value == 0 || IS_NPC(ch) )
	return;

    if (!IS_NPC(ch) && IS_HERO(ch) && !IS_SET(ch->act, PLR_VAMPIRE) )
	return;

    condition				= ch->pcdata->condition[iCond];
    if (!IS_NPC(ch) && !IS_SET(ch->act, PLR_VAMPIRE) )
	ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 48 );
    else
	ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 100 );

    if ( ch->pcdata->condition[iCond] == 0 )
    {
	switch ( iCond )
	{
	case COND_FULL:
	    if (!IS_SET(ch->act, PLR_VAMPIRE))
	    {
		send_to_char( "You are REALLY hungry.\n\r",  ch );
		act( "You hear $n's stomach rumbling.", ch, NULL, NULL, TO_ROOM );
	    }
	    break;

	case COND_THIRST:
	    if (!IS_SET(ch->act, PLR_VAMPIRE)) 
		send_to_char( "You are REALLY thirsty.\n\r", ch );
	    else if (ch->hit > 0)
	    {
		send_to_char( "You are DYING from lack of blood!\n\r", ch );
		act( "$n gets a hungry look in $s eyes.", ch, NULL, NULL, TO_ROOM );
		ch->hit = ch->hit - number_range(2,5);
		if (number_percent() <= ch->beast && ch->beast > 0) do_rage(ch,"");
		if (!IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	    }
	    break;

	case COND_DRUNK:
	    if ( condition != 0 )
		send_to_char( "You are sober.\n\r", ch );
	    break;
	}
    }
    else if ( ch->pcdata->condition[iCond] < 10 )
    {
	switch ( iCond )
	{
	case COND_FULL:
	    if (!IS_SET(ch->act, PLR_VAMPIRE)) 
		send_to_char( "You feel hungry.\n\r",  ch );
	    break;

	case COND_THIRST:
	    if (!IS_SET(ch->act, PLR_VAMPIRE)) 
		send_to_char( "You feel thirsty.\n\r", ch );
	    else
	    {
		send_to_char( "You crave blood.\n\r", ch );
		if (number_range(1,1000) <= ch->beast && ch->beast > 0) do_rage(ch,"");
		if (number_percent() > (ch->pcdata->condition[COND_THIRST]+75)
		    && !IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	    }
	    break;
	}
    }

    return;
}



/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    EXIT_DATA *pexit;
    int door;

    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next = ch->next;

	if ( !IS_NPC(ch) || ch->in_room == NULL || IS_AFFECTED(ch, AFF_CHARM) )
	    continue;

	/* Examine call for special procedure */
	if ( ch->spec_fun != 0 )
	{
	    if ( (*ch->spec_fun) ( ch ) )
		continue;
	}

	/* That's all for sleeping / busy monster */
	if ( ch->position != POS_STANDING )
	    continue;

	/* Scavenge */
	if ( IS_SET(ch->act, ACT_SCAVENGER)
	&&   ch->in_room->contents != NULL
	&&   number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max         = 1;
	    obj_best    = 0;
	    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	    {
		if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( "$n picks $p up.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

	/* Wander */
	if ( !IS_SET(ch->act, ACT_SENTINEL)
	&& ( door = number_bits( 5 ) ) <= 5
	&& ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
	&& ( !IS_SET(ch->act, ACT_STAY_AREA)
	||   pexit->to_room->area == ch->in_room->area ) )
	{
	    move_char( ch, door );
	}

	/* Flee */
	if ( ch->hit < ch->max_hit / 2
	&& ( door = number_bits( 3 ) ) <= 5
	&& ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) )
	{
	    CHAR_DATA *rch;
	    bool found;

	    found = FALSE;
	    for ( rch  = pexit->to_room->people;
		  rch != NULL;
		  rch  = rch->next_in_room )
	    {
		if ( !IS_NPC(rch) )
		{
		    found = TRUE;
		    break;
		}
	    }
	    if ( !found )
		move_char( ch, door );
	}

    }

    return;
}



/*
 * Update the weather.
 */
void weather_update( void )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int diff;

    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  5:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "The day has begun.\n\r" );
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "The sun rises in the east.\n\r" );
	break;

    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "The sun slowly disappears in the west.\n\r" );
	break;

    case 20:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "The night has begun.\n\r" );
	break;

    case 24:
	time_info.hour = 0;
	time_info.day++;
	break;
    }

    if ( time_info.day   >= 35 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if ( time_info.month >= 17 )
    {
	time_info.month = 0;
	time_info.year++;
    }

    /*
     * Weather change.
     */
    if ( time_info.month >= 9 && time_info.month <= 16 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change    = UMAX(weather_info.change, -12);
    weather_info.change    = UMIN(weather_info.change,  12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
    weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

    switch ( weather_info.sky )
    {
    default: 
	bug( "Weather_update: bad sky %d.", weather_info.sky );
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if ( weather_info.mmhg <  990
	|| ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The sky is getting cloudy.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_CLOUDY:
	if ( weather_info.mmhg <  970
	|| ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "It starts to rain.\n\r" );
	    weather_info.sky = SKY_RAINING;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "The clouds disappear.\n\r" );
	    weather_info.sky = SKY_CLOUDLESS;
	}
	break;

    case SKY_RAINING:
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "Lightning flashes in the sky.\n\r" );
	    weather_info.sky = SKY_LIGHTNING;
	}

	if ( weather_info.mmhg > 1030
	|| ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The rain stopped.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_LIGHTNING:
	if ( weather_info.mmhg > 1010
	|| ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The lightning has stopped.\n\r" );
	    weather_info.sky = SKY_RAINING;
	    break;
	}
	break;
    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   IS_OUTSIDE(d->character)
	    &&   IS_AWAKE(d->character) )
		send_to_char( buf, d->character );
	}
    }

    return;
}



/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_save;
    CHAR_DATA *ch_quit;
    bool is_obj;
    time_t save_time;

    save_time	= current_time;
    ch_save	= NULL;
    ch_quit	= NULL;
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	ch_next = ch->next;

	if (!IS_NPC(ch) && (IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH)))
	    is_obj = TRUE;
	else if (!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
	    {is_obj = TRUE;SET_BIT(ch->extra, EXTRA_OSWITCH);}
	else
	    is_obj = FALSE;
	/*
	 * Find dude with oldest save time.
	 */
	if ( !IS_NPC(ch)
	&& ( ch->desc == NULL || ch->desc->connected == CON_PLAYING )
	&&   ch->level >= 2
	&&   ch->save_time < save_time )
	{
	    ch_save	= ch;
	    save_time	= ch->save_time;
	}

	if ( ch->position > POS_STUNNED && !is_obj)
	{
	    if ( ch->hit  < ch->max_hit )
		ch->hit  += hit_gain(ch);

	    if ( ch->mana < ch->max_mana )
		ch->mana += mana_gain(ch);

	    if ( ch->move < ch->max_move )
		ch->move += move_gain(ch);
	}

	if ( ch->position == POS_STUNNED && !is_obj)
	{
            ch->hit = ch->hit + number_range(2,4);
	    update_pos( ch );
            if (ch->position > POS_STUNNED)
            {
                act( "$n clambers back to $s feet.", ch, NULL, NULL, TO_ROOM );
                act( "You clamber back to your feet.", ch, NULL, NULL, TO_CHAR );
            }
	}

	if ( !IS_NPC(ch) && ch->level < LEVEL_IMMORTAL && !is_obj)
	{
	    OBJ_DATA *obj;
	    int blood;

	    if ( ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 )
	    || ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 ) )
	    {
		if ( --obj->value[2] == 0 && ch->in_room != NULL )
		{
		    --ch->in_room->light;
		    act( "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( "$p goes out.", ch, obj, NULL, TO_CHAR );
		    extract_obj( obj );
		}
	    }

	    if ( ++ch->timer >= 12 )
	    {
		if ( ch->was_in_room == NULL && ch->in_room != NULL )
		{
		    ch->was_in_room = ch->in_room;
		    if ( ch->fighting != NULL )
			stop_fighting( ch, TRUE );
		    act( "$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM );
		    send_to_char( "You disappear into the void.\n\r", ch );
		    save_char_obj( ch );
		    char_from_room( ch );
		    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }

	    if ( ch->timer > 30 ) ch_quit = ch;

	    gain_condition( ch, COND_DRUNK,  -1 );
	    if (IS_SET(ch->act, PLR_WEREWOLF) && ch->position != POS_FIGHTING)
	    {
		ch->pcdata->wolf -= number_range(2,5);
		if (IS_SET(ch->act, PLR_WOLFMAN) && ch->pcdata->wolf < 100)
		    do_unwerewolf(ch,"");
	    }
	    if (!IS_SET(ch->act, PLR_VAMPIRE))
	    {
	    	gain_condition( ch, COND_FULL, -1 );
	    	gain_condition( ch, COND_THIRST, -1 );
	    }
	    else
	    {
		if ( ch->position != POS_FIGHTING && ch->pcdata->wolf > 0 )
		    ch->pcdata->wolf -= 1;
		blood = -1;
		if (ch->beast > 0)
		{
		    if (IS_VAMPAFF(ch, VAM_CLAWS)) blood -= number_range(1,3);
		    if (IS_VAMPAFF(ch, VAM_FANGS)) blood -= 1;
		    if (IS_VAMPAFF(ch, VAM_NIGHTSIGHT)) blood -= 1;
		    if (IS_VAMPAFF(ch, AFF_SHADOWSIGHT)) blood -= number_range(1,3);
		    if (IS_SET(ch->act, PLR_HOLYLIGHT)) blood -= number_range(1,5);
		    if (IS_VAMPAFF(ch, VAM_DISGUISED)) blood -= number_range(5,10);
		    if (IS_VAMPAFF(ch, VAM_CHANGED)) blood -= number_range(5,10);
		    if (IS_VAMPAFF(ch, IMM_SHIELDED)) blood -= number_range(1,3);
		    if (IS_POLYAFF(ch, POLY_SERPENT)) blood -= number_range(1,3);
		    if (ch->beast == 100) blood *= 2;
		}
		gain_condition( ch, COND_THIRST, blood );
	    }
	}

	for ( paf = ch->affected; paf != NULL; paf = paf_next )
	{
	    paf_next	= paf->next;
	    if ( paf->duration > 0 )
		paf->duration--;
	    else if ( paf->duration < 0 )
		;
	    else
	    {
		if ( paf_next == NULL
		||   paf_next->type != paf->type
		||   paf_next->duration > 0 )
		{
		    if ( paf->type > 0 && skill_table[paf->type].msg_off && !is_obj)
		    {
			send_to_char( skill_table[paf->type].msg_off, ch );
			send_to_char( "\n\r", ch );
		    }
		}
	  
		affect_remove( ch, paf );
	    }
	}

	/*
	 * Careful with the damages here,
	 *   MUST NOT refer to ch after damage taken,
	 *   as it may be lethal damage (on NPC).
	 */
	if ( ch->loc_hp[6] > 0 && !is_obj )
	{
	    int dam = 0;
	    int minhit = 0;
	    if (!IS_NPC(ch)) minhit = -11;
	    if (IS_BLEEDING(ch,BLEEDING_HEAD) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's neck.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your neck.\n\r", ch );
		dam += number_range(20,50);
	    }
	    if (IS_BLEEDING(ch,BLEEDING_THROAT) && (ch->hit-dam) > minhit )
	    {
		act( "Blood pours from the slash in $n's throat.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "Blood pours from the slash in your throat.\n\r", ch );
		dam += number_range(10,20);
	    }
	    if (IS_BLEEDING(ch,BLEEDING_ARM_L) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's left arm.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left arm.\n\r", ch );
		dam += number_range(10,20);
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_HAND_L) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's left wrist.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left wrist.\n\r", ch );
		dam += number_range(5,10);
	    }
	    if (IS_BLEEDING(ch,BLEEDING_ARM_R) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's right arm.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right arm.\n\r", ch );
		dam += number_range(10,20);
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_HAND_R) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's right wrist.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right wrist.\n\r", ch );
		dam += number_range(5,10);
	    }
	    if (IS_BLEEDING(ch,BLEEDING_LEG_L) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's left leg.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left leg.\n\r", ch );
		dam += number_range(10,20);
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_FOOT_L) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's left ankle.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your left ankle.\n\r", ch );
		dam += number_range(5,10);
	    }
	    if (IS_BLEEDING(ch,BLEEDING_LEG_R) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's right leg.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right leg.\n\r", ch );
		dam += number_range(10,20);
	    }
	    else if (IS_BLEEDING(ch,BLEEDING_FOOT_R) && (ch->hit-dam) > minhit )
	    {
		act( "A spray of blood shoots from the stump of $n's right ankle.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "A spray of blood shoots from the stump of your right ankle.\n\r", ch );
		dam += number_range(5,10);
	    }
	    if (IS_HERO(ch)) { ch->hit = ch->hit - dam;
		if (ch->hit < 1) ch->hit = 1; }
	    else ch->hit = ch->hit - dam;
	    update_pos(ch);
	    ch->in_room->blood += dam;
	    if (ch->in_room->blood > 1000) ch->in_room->blood = 1000;
	    if (ch == NULL) return;
	    if (ch->hit <=-11 || (IS_NPC(ch) && ch->hit < 1))
	    {
		do_killperson(ch,ch->name);
		return;
	    }
	}
	if ( IS_AFFECTED(ch, AFF_FLAMING) && !is_obj )
	{
	    int dam;
	    if (!IS_NPC(ch) && IS_HERO(ch)) break;
	    if (!IS_NPC(ch) && IS_IMMUNE(ch, IMM_HEAT) &&
		!IS_SET(ch->act, PLR_VAMPIRE)) break;
	    act( "$n's flesh burns and crisps.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "Your flesh burns and crisps.\n\r", ch );
	    dam = number_range(10,20);
	    if (!IS_NPC(ch) && IS_IMMUNE(ch, IMM_HEAT)) dam /= 2;
	    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_VAMPIRE)) dam *= 2;
	    ch->hit = ch->hit - dam;
	    update_pos(ch);
	    if (ch->hit <=-11)
	    {
		do_killperson(ch,ch->name);
		return;
	    }
	}
	else if ( IS_SET(ch->act, PLR_VAMPIRE) && (!IS_AFFECTED(ch,AFF_SHADOWPLANE)) &&
	    (!IS_NPC(ch) && !IS_IMMUNE(ch,IMM_SUNLIGHT)) &&
	    (!ch->in_room->sector_type == SECT_INSIDE) && !is_obj &&
	    (!room_is_dark(ch->in_room)) && (weather_info.sunlight != SUN_DARK) )
	{
	    act( "$n's flesh smolders in the sunlight!", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "Your flesh smolders in the sunlight!\n\r", ch );
	    /* This one's to keep Zarkas quiet ;) */
	    if (IS_POLYAFF(ch, POLY_SERPENT))
	    	ch->hit = ch->hit - number_range(2,4);
	    else
	    	ch->hit = ch->hit - number_range(5,10);
	    update_pos(ch);
	    if (ch->hit <=-11)
	    {
		do_killperson(ch,ch->name);
		return;
	    }
	}
	else if ( IS_AFFECTED(ch, AFF_POISON) && !is_obj )
	{
	    act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You shiver and suffer.\n\r", ch );
	    damage( ch, ch, 2, gsn_poison );
	}
	else if ( ch->position == POS_INCAP && !is_obj )
	{
	    if (IS_HERO(ch))
                ch->hit = ch->hit + number_range(2,4);
	    else
                ch->hit = ch->hit - number_range(1,2);
	    update_pos( ch );
            if (ch->position > POS_INCAP)
            {
                act( "$n's wounds stop bleeding and seal up.", ch, NULL, NULL, TO_ROOM );
                send_to_char( "Your wounds stop bleeding and seal up.\n\r", ch );
            }
            if (ch->position > POS_STUNNED)
            {
                act( "$n clambers back to $s feet.", ch, NULL, NULL, TO_ROOM );
                send_to_char( "You clamber back to your feet.\n\r", ch );
            }
	}
	else if ( ch->position == POS_MORTAL && !is_obj )
	{
	    if (IS_HERO(ch))
                ch->hit = ch->hit + number_range(2,4);
	    else
	    {
                ch->hit = ch->hit - number_range(1,2);
		if (!IS_NPC(ch) && (ch->hit <=-11))
		    do_killperson(ch,ch->name);
		return;
	    }
	    update_pos( ch );
            if (ch->position == POS_INCAP)
            {
                act( "$n's wounds begin to close, and $s bones pop back into place.", ch, NULL, NULL, TO_ROOM );
                send_to_char( "Your wounds begin to close, and your bones pop back into place.\n\r", ch );
            }
	}
	else if ( ch->position == POS_DEAD && !is_obj )
	{
	   update_pos(ch);
	   if (!IS_NPC(ch))
		do_killperson(ch,ch->name);
	   return;
	}
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    if ( ch_save != NULL || ch_quit != NULL )
    {
	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
	    ch_next = ch->next;
	    if ( ch == ch_save )
		save_char_obj( ch );
	    if ( ch == ch_quit )
		do_quit( ch, "" );
	}
    }

    return;
}



/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{   
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
	CHAR_DATA *rch;
	char *message;

	obj_next = obj->next;

	if ( obj->timer <= 0 || --obj->timer > 0 )
	    continue;

	switch ( obj->item_type )
	{
	default:              message = "$p vanishes.";         	break;
	case ITEM_FOUNTAIN:   message = "$p dries up.";         	break;
	case ITEM_CORPSE_NPC: message = "$p decays into dust."; 	break;
	case ITEM_CORPSE_PC:  message = "$p decays into dust."; 	break;
	case ITEM_FOOD:       message = "$p decomposes.";		break;
	case ITEM_TRASH:      message = "$p crumbles into dust.";	break;
	case ITEM_EGG:        message = "$p cracks open.";		break;
	case ITEM_WEAPON:     message = "The poison on $p melts through it.";		break;
	}

	if ( obj->carried_by != NULL )
	{
	    act( message, obj->carried_by, obj, NULL, TO_CHAR );
	}
	else if ( obj->in_room != NULL
	&&      ( rch = obj->in_room->people ) != NULL )
	{
	    act( message, rch, obj, NULL, TO_ROOM );
	    act( message, rch, obj, NULL, TO_CHAR );
	}

	/* If the item is an egg, we need to create a mob and shell!
	 * KaVir
	 */
	if ( obj->item_type == ITEM_EGG )
	{
	    CHAR_DATA      *creature;
	    OBJ_DATA       *egg;
	    if ( get_mob_index( obj->value[0] ) != NULL )
	    {
		if (obj->carried_by != NULL && obj->carried_by->in_room != NULL)
		{
	    	    creature = create_mobile( get_mob_index( obj->value[0] ) );
		    char_to_room(creature,obj->carried_by->in_room);
		}
		else if (obj->in_room != NULL)
		{
	    	    creature = create_mobile( get_mob_index( obj->value[0] ) );
		    char_to_room(creature,obj->in_room);
		}
		else
		{
	    	    creature = create_mobile( get_mob_index( obj->value[0] ) );
		    char_to_room(creature,get_room_index(ROOM_VNUM_HELL));
/*
		    obj->timer = 1;
		    continue;
*/
		}
	    	egg = create_object( get_obj_index( OBJ_VNUM_EMPTY_EGG ), 0 );
	    	egg->timer = 2;
	    	obj_to_room( egg, creature->in_room );
	    	act( "$n clambers out of $p.", creature, obj, NULL, TO_ROOM );
	    }
	    else if (obj->in_room != NULL)
	    {
	    	egg = create_object( get_obj_index( OBJ_VNUM_EMPTY_EGG ), 0 );
	    	egg->timer = 2;
	    	obj_to_room( egg, obj->in_room );
	    }
	}
	if (obj != NULL) extract_obj( obj );
    }

    return;
}



/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't want the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update( void )
{
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim;

    OBJ_DATA *obj;
    ROOM_INDEX_DATA *objroom;
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&& ( ch = d->character ) != NULL
	&&   !IS_NPC(ch)
	&&   ch->pcdata->chobj != NULL)
	{
	    obj=ch->pcdata->chobj;
	    if (obj->carried_by != NULL && obj->carried_by != ch)
	    	objroom = get_room_index(obj->carried_by->in_room->vnum);
	    else if (obj->in_room != NULL)
	    	objroom = get_room_index(obj->in_room->vnum);
	    else if (obj->in_obj != NULL)
	    	objroom = get_room_index(ROOM_VNUM_IN_OBJECT);
	    else continue;
	    if (ch->in_room != objroom)
	    {
	    	char_from_room(ch);
	    	char_to_room(ch,objroom);
		do_look(ch,"auto");
	    }
	}
	else if ( d->connected == CON_PLAYING
	&& ( ch = d->character ) != NULL
	&&   !IS_NPC(ch)
	&&   (IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH) || ch->pcdata->obj_vnum != 0) )
	{
	    if (ch->pcdata->obj_vnum != 0)
	    {
		bind_char(ch);
		return;
	    }
	    if (IS_HEAD(ch,LOST_HEAD))
	    {
	    	REMOVE_BIT(ch->loc_hp[0],LOST_HEAD);
	    	send_to_char("You are able to regain a body.\n\r",ch);
	    	ch->position = POS_RESTING;
	    	ch->hit = 1;
	    } else {
	    	send_to_char("You return to your body.\n\r",ch);
		REMOVE_BIT(ch->extra,EXTRA_OSWITCH);}
	    REMOVE_BIT(ch->affected_by,AFF_POLYMORPH);
	    free_string(ch->morph);
	    ch->morph = str_dup("");
	    char_from_room(ch);
	    char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
	    ch->pcdata->chobj = NULL;
	    do_look(ch,"auto");
	}
    }

    for ( wch = char_list; wch != NULL; wch = wch_next )
    {
	wch_next = wch->next;
	if ( IS_NPC(wch)
	||   wch->position <= POS_STUNNED
	||   wch->level >= LEVEL_IMMORTAL
	||   wch->in_room == NULL )
	    continue;

	for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
	{
	    int count;

	    ch_next	= ch->next_in_room;

	    if ( !IS_NPC(ch)
	    ||   !IS_SET(ch->act, ACT_AGGRESSIVE)
	    ||   no_attack(ch, wch)
	    ||   ch->fighting != NULL
	    ||   IS_AFFECTED(ch, AFF_CHARM)
	    ||   !IS_AWAKE(ch)
	    ||   ( IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch) )
	    ||   !can_see( ch, wch ) )
		continue;

	    /*
	     * Ok we have a 'wch' player character and a 'ch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     */
	    count	= 0;
	    victim	= NULL;
	    for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
	    {
		vch_next = vch->next_in_room;

		if ( !IS_NPC(vch)
		&&   !no_attack(ch, vch)
		&&   vch->level < LEVEL_IMMORTAL
		&&   vch->position > POS_STUNNED
		&&   ( !IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch) )
		&&   can_see( ch, vch ) )
		{
		    if ( number_range( 0, count ) == 0 )
			victim = vch;
		    count++;
		}
	    }

	    if ( victim == NULL )
	    {
		bug( "Aggr_update: null victim.", count );
		continue;
	    }

	    multi_hit( ch, victim, TYPE_UNDEFINED );
	}
    }

    return;
}



/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static  int     pulse_area;
    static  int     pulse_mobile;
    static  int     pulse_violence;
    static  int     pulse_point;

    if ( --pulse_area     <= 0 )
    {
	pulse_area	= number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
	area_update	( );
    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile	= PULSE_MOBILE;
	mobile_update	( );
    }

    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
    }

    if ( --pulse_point    <= 0 )
    {
	pulse_point     = number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 );
	weather_update	( );
	char_update	( );
	obj_update	( );
    }

    aggr_update( );
    tail_chain( );
    return;
}
