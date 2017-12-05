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
 * Externals
 */ 
extern  bool            merc_down;

/*
 * Globals
 */
bool    delete_obj;
bool    delete_char;

/*
 * Local functions.
 */
int	hit_gain        args( ( CHAR_DATA *ch ) );
int	mana_gain       args( ( CHAR_DATA *ch ) );
int	move_gain       args( ( CHAR_DATA *ch ) );
void	room_update	args( ( void ) );
void	mobile_update   args( ( void ) );
void	object_update	args( ( void ) );
void	weather_update  args( ( void ) );
void	char_update     args( ( void ) );
void	obj_update      args( ( void ) );
void	aggr_update     args( ( void ) );



/*
 * Advancement stuff.
 */
void advance_level( CHAR_DATA *ch )
{
    char buf [ MAX_STRING_LENGTH ];
    int  add_hp;
    int  add_mana;
    int  add_move;
    int  add_prac;

    sprintf( buf, "the %s",
	title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
    set_title( ch, buf );

    add_hp      = con_app[get_curr_con( ch )].hitp + number_range(
		    class_table[ch->class]->hp_min,
		    class_table[ch->class]->hp_max );
    add_mana    = class_table[ch->class]->fMana
        ? number_range(2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 8 )
	: 0;
    add_move    =
      number_range( 5, ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 4 );
    add_prac    = wis_app[get_curr_wis( ch )].practice;

    add_hp               = UMAX(  1, add_hp   );
    add_mana             = UMAX(  0, add_mana );
    add_move             = UMAX( 10, add_move );

    ch->max_hit 	+= add_hp;
    ch->max_mana	+= add_mana;
    ch->max_move	+= add_move;
    ch->practice	+= add_prac;

    if ( !IS_NPC( ch ) )
	REMOVE_BIT( ch->act, PLR_BOUGHT_PET );

    sprintf( buf,
	    "{o{wYour gain is: %d/%d hp, %d/%d m, %d/%d mv %d/%d prac.{x\n\r",
	    add_hp,	ch->max_hit,
	    add_mana,	ch->max_mana,
	    add_move,	ch->max_move,
	    add_prac,	ch->practice );
    send_to_char( buf, ch );
    return;
}   


/*
 * Demote stuff.
 */
void demote_level( CHAR_DATA *ch )
{
    char buf [ MAX_STRING_LENGTH ];
    int  add_hp;
    int  add_mana;
    int  add_move;
    int  add_prac;

    if ( ch->level == 1 )
        return;

    sprintf( buf, "the %s",
	title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
    set_title( ch, buf );

    add_hp      = con_app[get_curr_con( ch )].hitp + number_range(
		    class_table[ch->class]->hp_min,
		    class_table[ch->class]->hp_max );
    add_mana    = class_table[ch->class]->fMana
        ? number_range(2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 8 )
	: 0;
    add_move    =
      number_range( 5, ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 4 );
    add_prac    = wis_app[get_curr_wis( ch )].practice;

    add_hp               = UMAX(  1, add_hp   );
    add_mana             = UMAX(  0, add_mana );
    add_move             = UMAX( 10, add_move );

    ch->max_hit 	-= add_hp;
    ch->max_mana	-= add_mana;
    ch->max_move	-= add_move;
    ch->practice	-= add_prac;

    if ( !IS_NPC( ch ) )
	REMOVE_BIT( ch->act, PLR_BOUGHT_PET );

    ch->level -= 1;
    sprintf( buf,
	    "Your loss is: %d/%d hp, %d/%d m, %d/%d mv %d/%d prac.\n\r",
	    add_hp,	ch->max_hit,
	    add_mana,	ch->max_mana,
	    add_move,	ch->max_move,
	    add_prac,	ch->practice );
    send_to_char( buf, ch );
    return;
}   



void gain_exp( CHAR_DATA *ch, int gain )
{
    char buf [ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch ) || ch->level >= LEVEL_HERO )
	return;

    ch->exp = UMAX( EXP_PER_LEVEL, ch->exp + gain );
    while ( ch->level < LEVEL_HERO && ch->exp >= EXP_PER_LEVEL
	   * ( ch->level + 1 ) )
    {
	send_to_char( "{o{wYou raise a level!!  {x", ch );
	ch->level += 1;
	advance_level( ch );
	sprintf( buf, "%s has levelled and is now level %d.",
		ch->name, ch->level );
	wiznet( ch, WIZ_LEVELS, get_trust( ch ), buf );
    }

    return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;
    int number;
    int racehpgainextra = ( ch->max_hit * race_table[ ch->race ].hp_gain )
                            / ch->max_hit;
    
    if ( IS_NPC( ch ) )
    {
	gain = ch->level * 3 / 2;
    }
    else
    {
	gain = UMIN( 5, ch->level );

	number = number_percent();
	if (number < ch->pcdata->learned[gsn_fast_healing])
	     gain += number * gain / 100;

	switch ( ch->position )
	{
	case POS_SLEEPING: gain += get_curr_con( ch ) * 2;	break;
	case POS_RESTING:  gain += get_curr_con( ch );  	break;
	}

	if ( ch->pcdata->condition[COND_FULL  ] == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;

    }

    gain += racehpgainextra;

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 4;

    return UMIN( gain, ch->max_hit - ch->hit );
}



int mana_gain( CHAR_DATA *ch )
{
    int gain;
    int number;
    int racemanaextra = ( ch->max_mana * race_table[ ch->race ].mana_gain )
                          / ch->max_mana;
    
    if ( IS_NPC( ch ) )
    {
	gain = ch->level;
    }
    else
    {
	gain = UMIN( 5, ch->level / 2 );

	number = number_percent();

	switch ( ch->position )
	{
	case POS_SLEEPING: gain += get_curr_int( ch ) * 2;
	                   if (number < ch->pcdata->learned[gsn_meditate])
             		     gain += number * gain / 100;
  			   break;
	case POS_RESTING:  gain += get_curr_int( ch );
	                   if (number < ch->pcdata->learned[gsn_meditate])
             		     gain += number * gain / 100;
  			   break;
	}

	if ( ch->pcdata->condition[COND_FULL  ] == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;

    }

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 4;

    gain += racemanaextra;

    return UMIN( gain, ch->max_mana - ch->mana );
}



int move_gain( CHAR_DATA *ch )
{
    int gain;
    int racemoveextra = ( ch->max_move * race_table[ ch->race ].move_gain )
                          / ch->max_move;

    if ( IS_NPC( ch ) )
    {
	gain = ch->level;
    }
    else
    {
	gain = UMAX( 15, 2 * ch->level );

	switch ( ch->position )
	{
	case POS_SLEEPING: gain += get_curr_dex( ch ) * 2;	break;
	case POS_RESTING:  gain += get_curr_dex( ch );  	break;
	}

	if ( ch->pcdata->condition[COND_FULL  ] == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;
    }

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 4;

    gain += racemoveextra;

    return UMIN( gain, ch->max_move - ch->move );
}



void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;

    if ( value == 0 || IS_NPC( ch ) || ch->level >= LEVEL_HERO )
	return;

    condition				= ch->pcdata->condition[ iCond ];
    ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 48 );

    if ( ch->pcdata->condition[iCond] == 0 )
    {
	switch ( iCond )
	{
	case COND_FULL:
	    send_to_char( "You are hungry.\n\r",    ch );
	    break;

	case COND_THIRST:
	    send_to_char( "You are thirsty.\n\r",   ch );
	    break;

	case COND_DRUNK:
	    if ( condition != 0 )
		send_to_char( "You are sober.\n\r", ch );
	    break;
	}
    }

    return;
}



/*
 * Mob autonomous action.
 * This function takes 25% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update( void )
{
    CHAR_DATA *ch;
    EXIT_DATA *pexit;
    int        door;
    char       buf [ MAX_STRING_LENGTH ];

    /* Examine all mobs. */
    for ( ch = char_list; ch; ch = ch->next )
    {
        int rnum;

        if ( ch->deleted )
	    continue;

	if ( !ch->in_room )
	    continue;

	if ( !IS_NPC( ch )
	    || IS_AFFECTED( ch, AFF_CHARM ) )
	    continue;

	if ( !IS_SET( ch->act, ACT_SENTINEL )
	    && !ch->fighting
	    && ch->hunting
	    && ch->position == POS_STANDING  )
	{
	    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
	    hunt_victim( ch );
	    continue;
	}  

	/* Examine call for special procedure */
	if ( ch->spec_fun != 0 )
	{
	    if ( ( *ch->spec_fun ) ( ch ) )
		continue;
	}

	/* That's all for sleeping / busy monster */
	if ( ch->position < POS_STANDING )
	    continue;

        if ( ch->rider )
        {
            if ( IS_SET( ch->act, ACT_AGGRESSIVE ) )
                do_emote( ch, "snarls and growls." );
            continue;
        }

        if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE )
	    && IS_SET( ch->act, ACT_AGGRESSIVE ) )
	    do_emote( ch, "glares around and snarls." );

        /* MOBprogram random trigger */
        if ( ch->in_room->area->nplayer > 0 )
        {
            mprog_random_trigger( ch );
                                                /* If ch dies or changes
                                                position due to it's random
                                                trigger, continue - Kahn */
            if ( ch->position < POS_STANDING )
                continue;
        }

	/* Scavenge */
	if ( IS_SET( ch->act, ACT_SCAVENGER )
	    && ch->in_room->contents
	    && number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int       max;

	    max         = 1;
	    obj_best    = 0;
	    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	    {
		if ( CAN_WEAR( obj, ITEM_TAKE )
		    && obj->cost > max
		    && can_see_obj( ch, obj ) )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

	/* Wander or Flee  - modifications by Jason Dinkel */
	if ( ch->hit < ch->max_hit / 2 )
	    rnum = 3;
	else
	    rnum = 5;

	if ( !IS_SET( ch->act, ACT_SENTINEL )
	    && ( door = number_bits( rnum ) ) <= 5
	    && ( pexit = ch->in_room->exit[door] )
	    &&   pexit->to_room
	    &&   !IS_SET( pexit->exit_info, EX_CLOSED )
	    &&   !IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
	    && ( !IS_SET( ch->act, ACT_STAY_AREA )
		||   pexit->to_room->area == ch->in_room->area ) )
	{
	    /* Give message if hurt */
	    if ( rnum == 3 )
	        act( "$n wanders off terrified!", ch, NULL, NULL, TO_ROOM );
		  
	    move_char( ch, door );
	                                        /* If ch changes
	                                        position due
                                                to it's or someother mob's
                                                movement via MOBProgs,
                                                continue - Kahn */
            if ( ch->position < POS_STANDING )
                continue;

	}

	/* If people are in the room, then flee. */
	if ( rnum == 3
	    && !IS_SET( ch->act, ACT_SENTINEL ) )
	{
	    CHAR_DATA *rch;

	    for ( rch  = ch->in_room->people; rch; rch  = rch->next_in_room )
	    {
		if ( rch->deleted )
		    continue;

		/* If NPC can't see PC it shouldn't feel fear - Zen */
		if ( !IS_NPC( rch ) && can_see( ch, rch ) )
		{
		    int direction;

		    door = -1;

		    /* SMAUG had this. It makes some nice FX - Zen */
		    if ( is_fearing( ch, rch ) )
		    {
			switch( number_bits( 2 ) )
			{
			default: sprintf( buf, "May i have some peace %s?",
					 rch->name );			break;
			case  0: sprintf( buf, "Get away from me, %s!",
					 rch->name );			break;
			case  1: sprintf( buf, "Leave me be, %s!",
					    rch->name );		break;
			case  2: sprintf( buf, "%s is trying to kill me! Help!",
					 rch->name );			break;
			case  3: sprintf( buf, "Someone save me from %s!",
					    rch->name );		break;
			}
			do_yell( ch, buf );
		    }

		    act( "$n flees in terror!", ch, NULL, NULL, TO_ROOM );

		    /* Find an exit giving each one an equal chance */
		    for ( direction = 0; direction < MAX_DIR; direction++ )
		    {
			if ( ch->in_room->exit[direction]
			    && number_range( 0, direction ) == 0 )
			    door = direction;
		    }

		    /* If no exit, attack.  Else flee! */
		    if ( door == -1 )
			multi_hit( ch, rch, TYPE_UNDEFINED );
		    else
			move_char( ch, door );
		    break;
		}
	    }
	}
    }
    return;
}



/*
 * Update the weather.
 */
void weather_update( void )
{
    DESCRIPTOR_DATA *d;
    char             buf [ MAX_STRING_LENGTH ];
    int              diff;

    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  4:
	weather_info.sunlight = MOON_SET;
	strcat( buf, "{dThe moon sets.{x\n\r" );
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "{yThe sun rises in the east.{x\n\r" );
	weather_info.temperature += number_fuzzy( 10 );
	break;

    case  7:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "{o{yThe day has begun.{x\n\r" );
	if ( time_info.month <= 4 || time_info.month >= 15 )
	    weather_info.temperature = number_fuzzy( 20 );
	else
	    weather_info.temperature = number_fuzzy( 50 );
	break;

    case  12:
	strcat( buf, "{o{yIt is noon.{x\n\r" );
	weather_info.temperature += number_fuzzy( 20 );
	break;

    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "{rThe sun slowly disappears in the west.{x\n\r" );
	weather_info.temperature -= number_fuzzy( 20 );
	break;

    case 20:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "{dThe night has begun.{x\n\r" );
	weather_info.temperature -= number_fuzzy( 10 );
	break;

    case 24:
	weather_info.sunlight = MOON_RISE;
	strcat( buf, "{wThe moon rises, casting a silver glow over the night.{x\n\r" );
	weather_info.temperature -= number_fuzzy( 10 );
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
    weather_info.winddir += number_range( 0, 2 ) - 1;

    if ( time_info.month >= 9 && time_info.month <= 16 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice( 1, 4 ) + dice( 2, 6 ) - dice( 2, 6 );
    weather_info.change    = UMAX( weather_info.change, -12 );
    weather_info.change    = UMIN( weather_info.change,  12 );

    weather_info.mmhg     += weather_info.change;
    weather_info.mmhg      = UMAX( weather_info.mmhg,  960 );
    weather_info.mmhg      = UMIN( weather_info.mmhg, 1040 );

    switch ( weather_info.sky )
    {
    default: 
	bug( "Weather_update: bad sky %d.", weather_info.sky );
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if (     weather_info.mmhg <  990
	    || ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "{wA few flakes of snow are falling.{x\n\r" );
		weather_info.temperature -= 10;
	    }
	    else
		strcat( buf, "{dThe sky is getting cloudy.{x\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	    weather_info.windspeed += 10;
	}
	break;

    case SKY_CLOUDY:
	if (     weather_info.mmhg <  970
	    || ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{

	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "{o{wIt starts to snow.{x\n\r" );
		weather_info.temperature -= 10;
	    }
	    else
		strcat( buf, "{o{bIt starts to rain.{x\n\r" );
	    weather_info.sky = SKY_RAINING;
	    weather_info.windspeed += 10;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "{o{wThe snow lets up.{x\n\r" );
		weather_info.temperature += 10;
	    }
	    else
		strcat( buf, "{o{wThe clouds disappear.{x\n\r" );
	    weather_info.sky = SKY_CLOUDLESS;
	    weather_info.windspeed -= 10;
	}
	break;

    case SKY_RAINING:
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "{dYou are caught in a blizzard.{x\n\r" );
		weather_info.temperature -= 30;
	    }
	    else
		strcat( buf, "{o{yLightning flashes in the sky.{x\n\r" );
	    weather_info.sky = SKY_LIGHTNING;
	    weather_info.windspeed += 10;
	}

	if (     weather_info.mmhg > 1030
	    || ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "{o{wThe snow is letting up.{x\n\r" );
		weather_info.temperature += 30;
	    }
	    else
		strcat( buf, "{o{wThe rain stopped.{x\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	    weather_info.windspeed -= 10;
	}
	break;

    case SKY_LIGHTNING:
	if (     weather_info.mmhg > 1010
	    || ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    if ( time_info.month <= 4 || time_info.month >= 15 )
	    {
		strcat( buf, "{dThe blizzard subsides.{x\n\r" );
		weather_info.temperature += 10;
	    }
	    else
		strcat( buf, "{dThe lightning has stopped.{x\n\r" );
	    weather_info.sky = SKY_RAINING;
	    weather_info.windspeed -= 10;
	    break;
	}
	break;
    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
		&& IS_OUTSIDE( d->character )
		&& IS_AWAKE  ( d->character ) )
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
    CHAR_DATA *ch_save;
    CHAR_DATA *ch_quit;
    time_t     save_time;

    ch_save	= NULL;
    ch_quit	= NULL;
    save_time	= current_time;

    for ( ch = char_list; ch; ch = ch->next )
    {
	AFFECT_DATA *paf;

	if ( ch->deleted )
	    continue;

	if ( ch->position >= POS_STUNNED )
	{
	    if ( ch->hit  < ch->max_hit  )
		ch->hit  += hit_gain( ch );

	    if ( ch->mana < ch->max_mana )
		ch->mana += mana_gain( ch );

	    if ( ch->move < ch->max_move )
		ch->move += move_gain( ch );
	}

	if ( ch->position == POS_STUNNED )
	    update_pos( ch );

	for ( paf = ch->affected; paf; paf = paf->next )
	{
	    if ( paf->deleted )
	        continue;
	    if ( paf->duration > 0 )
		paf->duration--;
	    else if ( paf->duration < 0 )
		;
	    else
	    {
		if ( !paf->next
		    || paf->next->type != paf->type
		    || paf->next->duration > 0 )
		{
		    if ( paf->type > 0 && skill_table[paf->type].msg_off )
		    {
			send_to_char( skill_table[paf->type].msg_off, ch );
			send_to_char( "\n\r", ch );
		    }
		}

		if ( paf->type == gsn_vampiric_bite )
		    ch->race = race_lookup( "Vampire" );

		affect_remove( ch, paf );
	    }
	}

        /*
         * Careful with the damages here,
         *   MUST NOT refer to ch after damage taken,
         *   as it may be lethal damage (on NPC).
         */
	if ( is_affected( ch, gsn_plague ) )
	{
	    AFFECT_DATA *af;
	    CHAR_DATA   *vch;
	    AFFECT_DATA  plague;
	    int          save;
	    int          dam;

	    act( "$n writhes in agony as plague sores erupt from $s skin.",
		ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You writhe in agony from the plague.\n\r", ch );
	    for ( af = ch->affected; af; af = af->next )
                if ( af->type == gsn_plague )				break;

	    if ( !af )
	    {
                REMOVE_BIT( ch->affected_by, AFF_PLAGUE );
                continue;
	    }

	    if ( af->level == 1 )
		continue;

	    plague.type		= gsn_plague;
	    plague.level	= af->level - 1;
	    plague.duration	= number_range( 1, 2 * plague.level );
	    plague.location	= APPLY_STR;
	    plague.modifier	= -5;
	    plague.bitvector	= AFF_PLAGUE;
	    save		= plague.level;

	    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	    {
		if ( vch->deleted )
		    continue;

		if ( save && !saves_spell( save, vch, DAM_DISEASE )
		    && !IS_IMMORTAL( vch )
		    && !IS_AFFECTED( vch, AFF_PLAGUE )
		    && number_bits( 4 ) == 0 )
		{
		    send_to_char( "You feel hot and feverish.\n\r", vch );
		    act( "$n shivers and looks very ill.", vch, NULL, NULL,
			TO_ROOM );
		    affect_join( vch, &plague );
                }
	    }

	    dam = UMIN( ch->level, 5 );
	    ch->mana -= dam;
	    ch->move -= dam;
	    damage( ch, ch, dam, gsn_plague, WEAR_NONE, DAM_DISEASE );
        }

	if ( ( time_info.hour > 5 && time_info.hour < 21 )
	    && CHECK_SUS( ch, RIS_LIGHT )
	    && !IS_SET( ch->in_room->room_flags, ROOM_DARK ) )
	{
	    int dmg = 0;

	    if ( ch->in_room->sector_type == SECT_INSIDE )
	    {
	        dmg = 10;
	    }
	    else
	    {
		if ( ch->in_room->sector_type == SECT_FOREST )
		{
		    dmg = 25;
		}
		else
		{
		    dmg = 50;
		}
	    }
	    
	    if ( weather_info.sky == SKY_CLOUDY )
	        dmg /= 2;
	    if ( weather_info.sky == SKY_RAINING )
	      {
	        dmg /= 4;
		dmg *= 3;
	      }

	    damage( ch, ch, dmg, gsn_poison, WEAR_NONE, DAM_LIGHT );
	}
	else
	    if ( (   ch->in_room->sector_type == SECT_UNDERWATER
		  && ( !IS_IMMORTAL( ch ) && !IS_AFFECTED( ch, AFF_GILLS )
		      && !IS_SET( race_table[ ch->race ].race_abilities,
				 RACE_WATERBREATH ) ) )
		|| ( (    ch->in_room->sector_type != SECT_UNDERWATER
		       && ch->in_room->sector_type != SECT_WATER_NOSWIM
		       && ch->in_room->sector_type != SECT_WATER_SWIM )
		    && IS_SET( race_table[ ch->race ].race_abilities,
			      RACE_WATERBREATH )
		    && (   str_cmp( race_table[ch->race].name, "Object" )
			&& str_cmp( race_table[ch->race].name, "God" ) ) ) )
        {
            send_to_char( "You can't breathe!\n\r", ch );
            act( "$n sputters and chokes!", ch, NULL, NULL, TO_ROOM );
            damage( ch, ch, 5, gsn_breathe_water, WEAR_NONE, DAM_DROWNING );
        }
        else if ( IS_AFFECTED( ch, AFF_POISON ) )
        {
            send_to_char( "You shiver and suffer.\n\r", ch );
            act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
            damage( ch, ch, 2, gsn_poison, WEAR_NONE, DAM_POISON );
        }
        else if ( ch->position == POS_INCAP )
        {
            damage( ch, ch, 1, TYPE_UNDEFINED, WEAR_NONE, DAM_NONE );
        }
        else if ( ch->position == POS_MORTAL )
        {
            damage( ch, ch, 2, TYPE_UNDEFINED, WEAR_NONE, DAM_NONE );
        }

	/* Thats all for mobs */
        if ( IS_NPC( ch ) )
	    continue;

	/*
	 * Find dude with oldest save time.
	 */
	if (   ( !ch->desc || ch->desc->connected == CON_PLAYING )
	    &&   ch->level >= 2
	    &&   ch->save_time < save_time )
	{
	    ch_save	= ch;
	    save_time	= ch->save_time;
	}

	if ( ( ch->level < LEVEL_IMMORTAL
	    || ( !ch->desc && !IS_SWITCHED( ch ) ) ) )
	{
	    OBJ_DATA *obj;

	    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
		&& obj->item_type == ITEM_LIGHT
		&& obj->value[2] > 0 )
	    {
		if ( --obj->value[2] == 0 && ch->in_room )
		{
		    --ch->in_room->light;
		    act( "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( "$p goes out.", ch, obj, NULL, TO_CHAR );
		    extract_obj( obj );
		}
	    }

	    if ( ++ch->timer >= 10 )
	    {
		if ( !ch->was_in_room && ch->in_room )
		{
		    ch->was_in_room = ch->in_room;
		    if ( ch->fighting )
			stop_fighting( ch, TRUE );
		    send_to_char( "You disappear into the void.\n\r", ch );
		    act( "$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM );
		    save_char_obj( ch );
		    char_from_room( ch );
		    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }

	    if ( ch->timer > 20 && !IS_SWITCHED( ch ) )
		ch_quit = ch;

	    gain_condition( ch, COND_DRUNK,  -1 );
	    gain_condition( ch, COND_FULL,
			   ( -1 - race_table[ch->race].hunger_mod ) );
	    gain_condition( ch, COND_THIRST,
			   ( -1 - race_table[ch->race].thirst_mod ) );
	}
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    if ( ch_save || ch_quit )
    {
	for ( ch = char_list; ch; ch = ch->next )
	{
	    if ( ch->deleted )
	        continue;
	    if ( ch == ch_save )
		save_char_obj( ch );
	    if ( ch == ch_quit )
		do_quit( ch, "" );
	}
    }

    return;
}



/*
 * Update all rooms.
 * This function is performance sensitive.
 */
void room_update( void )
{   
    OBJ_DATA        *obj;
    OBJ_DATA        *obj_next;
    CHAR_DATA       *rch;
    CHAR_DATA       *rch_next;
    ROOM_INDEX_DATA *new_room;
    ROOM_INDEX_DATA *pRoomIndex;
    int              iHash;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( pRoomIndex  = room_index_hash[iHash];
              pRoomIndex;
              pRoomIndex  = pRoomIndex->next )
        {
	    if ( pRoomIndex->sector_type != SECT_AIR
		|| !pRoomIndex->exit[5]
		|| !( new_room = pRoomIndex->exit[5]->to_room ) )
		continue;

	    for ( obj = pRoomIndex->contents; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;

		if ( obj->deleted )
		    continue;

		if ( !IS_SET( obj->wear_flags, ITEM_TAKE ) )
		    continue;

		if ( ( rch = obj->in_room->people ) )
		{
		    act( "$p falls away.", rch, obj, NULL, TO_ROOM );
		    act( "$p falls away.", rch, obj, NULL, TO_CHAR );
		}
	
		obj_from_room( obj );
		obj_to_room( obj, new_room );
	
		if ( ( rch = obj->in_room->people ) )
		{
		    act( "$p falls by.", rch, obj, NULL, TO_ROOM );
		    act( "$p falls by.", rch, obj, NULL, TO_CHAR );
		}
	    }

	    for ( rch = pRoomIndex->people; rch; rch = rch_next )
	    {
		rch_next = rch->next_in_room;

		if ( rch->deleted )
		    continue;

		if ( IS_AFFECTED( rch, AFF_FLYING )
        	    || IS_SET( race_table[rch->race].race_abilities, RACE_FLY ) )
		    continue;

		if ( rch->in_room->people )
		{
		    act( "You are falling down!", rch, NULL, NULL, TO_CHAR );
		    act( "$n falls away.",        rch, NULL, NULL, TO_ROOM );
		}
		
		char_from_room( rch );
		char_to_room( rch, new_room );
	
		if ( rch->in_room->people )
		    act( "$n falls by.", rch, NULL, NULL, TO_ROOM );
	    }
        }
    }

    return;
}



/*
 * Update all objects.
 * This function is performance sensitive.
 */
void object_update( void )
{   
    OBJ_DATA        *obj;
    OBJ_DATA        *obj_next;

    for ( obj = object_list; obj; obj = obj_next )
    {
	obj_next = obj->next;

	if ( obj->deleted )
	    continue;

	/* Examine call for special procedure */
	if ( obj->spec_fun != 0 )
	{
	    if ( ( *obj->spec_fun ) ( obj, obj->carried_by ) )
		continue;
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
    OBJ_DATA  *obj;
    OBJ_DATA  *obj_next;
    OBJ_DATA  *obj_prev;
    CHAR_DATA *rch;
    char      *message;

    for ( obj = object_list, obj_prev = NULL; obj; obj = obj_next )
    {
	obj_next = obj->next;

	if ( obj->deleted )
	{
	    obj_prev = obj;
	    continue;
	}

	if ( obj->timer < -1 )
	    obj->timer = -1;

	if ( obj->timer < 0 )
	{
	    obj_prev = obj;
	    continue;
	}

	/*
	 *  Bug fix:  used to shift to obj_free if an object whose
	 *  timer ran out contained obj->next.  Bug was reported
	 *  only when the object(s) inside also had timer run out
	 *  during the same tick.     --Thelonius (Monk)
	 */
	if ( --obj->timer == 0 )
	{
	    switch ( obj->item_type )
	    {
	    	default:              message = "$p vanishes.";         break;
    		case ITEM_FOUNTAIN:   message = "$p dries up.";         break;
    		case ITEM_CORPSE_NPC: message = "$p decays into dust."; break;
    		case ITEM_CORPSE_PC:  message = "$p decays into dust."; break;
    		case ITEM_FOOD:       message = "$p decomposes.";       break;
		case ITEM_PORTAL:     message = "$p fades out of existence."; 
									break;
	    }
    
	    if ( obj->carried_by )
	    {
	        act( message, obj->carried_by, obj, NULL, TO_CHAR );
	    }
	    else
	      if ( obj->in_room
		  && ( rch = obj->in_room->people ) )
	      {
		  act( message, rch, obj, NULL, TO_ROOM );
		  act( message, rch, obj, NULL, TO_CHAR );
	      }
    
	    if ( obj == object_list )
	    {
	        extract_obj( obj );
   
	        obj_next = object_list;
	        obj_prev = NULL;
	        continue;
	    }

					    /* (obj != object_list) */
	    if ( !obj_prev )  /* Can't see how, but... */
		bug( "Obj_update: obj %d no longer in object_list",
		    obj->pIndexData->vnum );

	    extract_obj( obj );
    
	    obj_next = obj_prev->next;
	}

	obj_prev = obj;
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
 * This function takes .2% of total CPU time.
 *
 * -Kahn
 */
void aggr_update( void )
{
    CHAR_DATA       *ch;
    CHAR_DATA       *mch;
    CHAR_DATA       *vch;
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    ACT_PROG_DATA   *apdtmp;

    while ( ( apdtmp = mob_act_list ) )
    {
        mch = mob_act_list->vo;
        if ( !mch->deleted && mch->mpactnum > 0 )
	{
	    MPROG_ACT_LIST *tmp_act;

	    while ( ( tmp_act = mch->mpact ) )
            {
		if ( tmp_act->obj && tmp_act->obj->deleted )
		  tmp_act->obj = NULL;
		if ( tmp_act->ch && !tmp_act->ch->deleted )
		  mprog_wordlist_check( tmp_act->buf, mch, tmp_act->ch,
					tmp_act->obj, tmp_act->vo, ACT_PROG );
		mch->mpact = tmp_act->next;
		free_string( tmp_act->buf );
		tmp_act->buf = NULL;
		free_mem( tmp_act, sizeof( MPROG_ACT_LIST ) );
	    }
	    mch->mpactnum = 0;
	    mch->mpact    = NULL;
	}
        mob_act_list = apdtmp->next;
        free_mem( apdtmp, sizeof( ACT_PROG_DATA ) );
    }

    /*
     * Let's not worry about link dead characters. -Kahn
     */
    for ( d = descriptor_list; d; d = d->next )
    {
	ch = d->character;

	if ( d->connected != CON_PLAYING
	    || ch->level >= LEVEL_IMMORTAL
	    || !ch->in_room )
	    continue;

	/* mch wont get hurt */
	for ( mch = ch->in_room->people; mch; mch = mch->next_in_room )
	{
	    int count;
	    bool hate = FALSE;

	    if ( !IS_NPC( mch )
		|| mch->deleted
		|| mch->fighting
		|| IS_AFFECTED( mch, AFF_CHARM )
		|| !IS_AWAKE( mch )
		|| ( IS_SET( mch->act, ACT_WIMPY ) && IS_AWAKE( ch ) )
		|| !can_see( mch, ch )
		|| ( !IS_SET( mch->act, ACT_AGGRESSIVE )
		    && ( str_infix( race_table[ch->race].name,
				   race_table[mch->race].hate )
			|| ( !str_infix( race_table[ch->race].name,
					race_table[mch->race].hate )
			    && abs( mch->level - ch->level ) > 4 ) ) ) )
		continue;

	    if ( !str_infix( race_table[ch->race].name,
			    race_table[mch->race].hate ) )
	        hate = TRUE;

            if ( is_hating( mch, ch ) )
            {
                found_prey( mch, ch );
                continue;
            }

	    /*
	     * Ok we have a 'ch' player character and a 'mch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     */
	    count  = 0;
	    victim = NULL;
	    for ( vch = mch->in_room->people; vch; vch = vch->next_in_room )
	    {
	        if ( IS_NPC( vch )
		    || vch->deleted
		    || vch->level >= LEVEL_IMMORTAL )
		    continue;

		if ( ( !IS_SET( mch->act, ACT_WIMPY ) || !IS_AWAKE( vch ) )
		    && can_see( mch, vch ) )
		{
		    if (   !hate
			|| ( hate
			    && !str_infix( race_table[vch->race].name,
					  race_table[mch->race].hate ) ) )
		    {
		        if ( number_range( 0, count ) == 0 )
			    victim = vch;
			count++;
		    }
		}
	    }

	    if ( !victim )
	        continue;

	    multi_hit( mch, victim, TYPE_UNDEFINED );

	} /* mch loop */

    } /* descriptor loop */

    return;
}

/* Update the check on time for autoshutdown */
void time_update( void )
{
    FILE            *fp;
    char             buf [ MAX_STRING_LENGTH ];
    
    if ( down_time <= 0 )
        return;
    if ( current_time > warning1 && warning1 > 0 )
    {
	sprintf( buf, "First Warning!\n\r%s in %d minutes or %d seconds.\n\r",
		Reboot ? "Reboot" : "Shutdown",
		(int) ( down_time - current_time ) / 60,
		(int) ( down_time - current_time ) );
	send_to_all_char( buf );
	warning1 = 0;
    }
    if ( current_time > warning2 && warning2 > 0 )
    {
	sprintf( buf, "Second Warning!\n\r%s in %d minutes or %d seconds.\n\r",
		Reboot ? "Reboot" : "Shutdown",
		(int) ( down_time - current_time ) / 60,
		(int) ( down_time - current_time ) );
	send_to_all_char( buf );
	warning2 = 0;
    }
    if ( current_time + 10 > down_time && warning2 == 0 )
    {
	sprintf( buf, "Final Warning!\n\r%s in 10 seconds.\n\r",
		Reboot ? "Reboot" : "Shutdown" );
	send_to_all_char( buf );
	warning2--;
    }
    if ( current_time > down_time )
    {
	if ( IS_SET( sysdata.act, MUD_AUTOSAVE_DB ) )
	    do_asave( NULL, "" );
        sprintf( buf, "%s by system.\n\r", Reboot ? "Reboot" : "Shutdown" );
	send_to_all_char( buf );
	log_string( buf );

	end_of_game( );

	if ( !Reboot )
	{
	    fclose( fpReserve );
	    if ( !( fp = fopen( SHUTDOWN_FILE, "a" ) ) )
	      {
		perror( SHUTDOWN_FILE );
		bug( "Could not open the Shutdown file!", 0 );
	      }
	    else
	      {
		fprintf( fp, "Shutdown by System\n" );
		fclose ( fp );
	      }
	    fpReserve = fopen ( NULL_FILE, "r" );
	}
	merc_down = TRUE;
    }
    
    return;

}

/*
 * Remove deleted EXTRA_DESCR_DATA from objects.
 * Remove deleted AFFECT_DATA from chars and objects.
 * Remove deleted CHAR_DATA and OBJ_DATA from char_list and object_list.
 */
void list_update( void )
{
            CHAR_DATA   *ch;
            CHAR_DATA   *ch_next;
            OBJ_DATA    *obj;
            OBJ_DATA    *obj_next;
    extern  bool         delete_obj;
    extern  bool         delete_char;

    if ( delete_char )
        for ( ch = char_list; ch; ch = ch_next )
	  {
	    AFFECT_DATA *paf;
	    AFFECT_DATA *paf_next;
	    
	    for ( paf = ch->affected; paf; paf = paf_next )
	      {
		paf_next = paf->next;
		
		if ( paf->deleted || ch->deleted )
		  {
		    if ( ch->affected == paf )
		      {
			ch->affected = paf->next;
		      }
		    else
		      {
			AFFECT_DATA *prev;
			
			for ( prev = ch->affected; prev; prev = prev->next )
			  {
			    if ( prev->next == paf )
			      {
				prev->next = paf->next;
				break;
			      }
			  }
			
			if ( !prev )
			  {
			    bug( "List_update: cannot find paf on ch.", 0 );
			    continue;
			  }
		      }
		    
		    paf->next   = affect_free;
		    affect_free = paf;
		  }
	      }

	    ch_next = ch->next;
	    
	    if ( ch->deleted )
	      {
		if ( ch == char_list )
		  {
		    char_list = ch->next;
		  }
		else
		  {
		    CHAR_DATA *prev;

		    for ( prev = char_list; prev; prev = prev->next )
		      {
			if ( prev->next == ch )
			  {
			    prev->next = ch->next;
			    break;
			  }
		      }
		    
		    if ( !prev )
		      {
			char buf [ MAX_STRING_LENGTH ];
			
			sprintf( buf, "List_update: char %s not found.",
				ch->name );
			bug( buf, 0 );
			continue;
		      }
		  }
		
		free_char( ch );
	      }
	  }

    if ( delete_obj )
      for ( obj = object_list; obj; obj = obj_next )
	{
	  AFFECT_DATA      *paf;
	  AFFECT_DATA      *paf_next;
	  EXTRA_DESCR_DATA *ed;
	  EXTRA_DESCR_DATA *ed_next;

	  for ( ed = obj->extra_descr; ed; ed = ed_next )
	    {
	      ed_next = ed->next;
	      
	      if ( obj->deleted )
		{
		  free_string( ed->description );
		  free_string( ed->keyword     );
		  ed->next         = extra_descr_free;
		  extra_descr_free = ed;
		}
	    }

	  for ( paf = obj->affected; paf; paf = paf_next )
	    {
	      paf_next = paf->next;
	      
	      if ( obj->deleted )
		{
		  if ( obj->affected == paf )
		    {
		      obj->affected = paf->next;
		    }
		  else
		    {
		      AFFECT_DATA *prev;
		      
		      for ( prev = obj->affected; prev; prev = prev->next )
			{
			  if ( prev->next == paf )
			    {
			      prev->next = paf->next;
			      break;
			    }
			}

		      if ( !prev )
			{
			  bug( "List_update: cannot find paf on obj.", 0 );
			  continue;
			}
		    }
		  
		  paf->next   = affect_free;
		  affect_free = paf;
		}
	    }

	  obj_next = obj->next;

	  if ( obj->deleted )
	    {
	      if ( obj == object_list )
		{
		  object_list = obj->next;
		}
	      else
		{
		  OBJ_DATA *prev;
		  
		  for ( prev = object_list; prev; prev = prev->next )
		    {
		      if ( prev->next == obj )
			{
			  prev->next = obj->next;
			  break;
			}
		    }
		  
		  if ( !prev )
		    {
		      bug( "List_update: obj %d not found.",
			  obj->pIndexData->vnum );
		      continue;
		    }
		}


	      free_string( obj->name        );
	      free_string( obj->description );
	      free_string( obj->short_descr );
	      --obj->pIndexData->count;

	      obj->next	= obj_free;
	      obj_free	= obj;
	    }
	}

    delete_obj		= FALSE;
    delete_char		= FALSE;
    return;
}

/*
 * Update the ban file upon call.
 * Written by Tre of EnvyMud and modified by Kahn
 */
void ban_update( void )
{
    FILE      *fp;
    BAN_DATA  *pban;
    char       strsave [ MAX_INPUT_LENGTH ];

    fclose( fpReserve );

    sprintf( strsave, "%s%s", SYSTEM_DIR, BAN_FILE );

    if ( !( fp = fopen ( strsave, "w" ) ) )
    {
	bug( "Ban_update:  fopen of BAN_FILE failed", 0 );
    }
    else
    {
	for ( pban = ban_list; pban; pban = pban->next )
	    fprintf( fp, "%s~\n", pban->name );

	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );

    return;

}

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static int pulse_area;
    static int pulse_fast;
    static int pulse_violence;
    static int pulse_point;
    static int pulse_db_dump = PULSE_DB_DUMP;

    if ( IS_SET( sysdata.act, MUD_AUTOSAVE_DB ) )
    {
	if ( --pulse_db_dump  <= 0 )
	{
	    wiznet( NULL, WIZ_TICKS, L_DIR, "Dump Area pulse (OLC)" );
	    pulse_db_dump   = PULSE_DB_DUMP;
	    do_asave( NULL, "" );
	}
    }

    /* Maniac, added this warning so it can be delayed on time... */
    switch ( pulse_db_dump )
    {
    case   5: wiznet( NULL, WIZ_TICKS, L_JUN,
		     "Dump Area pulse coming soon... beware of lag" );
    case 100: wiznet( NULL, WIZ_TICKS, L_SEN,
		     "Dump Area Pulse in 100 pulses..." );
    }

    if ( --pulse_area     <= 0 )
    {
	wiznet( NULL, WIZ_TICKS, L_APP, "Area update pulse" );
	pulse_area	= number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
	area_update	( );
    }

    if ( --pulse_violence <= 0 )
    {
        wiznet( NULL, WIZ_TICKS, L_APP, "Violence update pulse" );
	pulse_violence  = PULSE_VIOLENCE;
	violence_update ( );
    }

    if ( --pulse_fast   <= 0 )
    {
        wiznet( NULL, WIZ_TICKS, L_APP, "Fast pulse point" );
	pulse_fast	= number_range( PULSE_FAST / 2, 3 * PULSE_FAST / 2 );
	room_update     ( );
	object_update   ( );
	mobile_update   ( );
    }

    if ( --pulse_point    <= 0 )
    {
        wiznet( NULL, WIZ_TICKS, LEVEL_HERO, "Pulse point" );
	pulse_point	= number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 );
	weather_update  ( );
	char_update     ( );
	obj_update      ( );
	list_update     ( );
    }

    time_update( );
    aggr_update( );
    tail_chain( );
    return;
}
