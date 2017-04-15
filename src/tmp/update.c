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
#include <string.h>
#include <time.h>
#include "merc.h"

/*
 * Externals
 */
extern  char *  const   month_name[];
extern  char *  const   day_name[];
extern  bool            merc_down;
extern  int             auc_count;
extern  void            auc_channel( char *auction );
extern  void            chat_update( void );
extern  int             port;

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
void	mobile_update   args( ( void ) );
void	weather_update  args( ( void ) );
void	char_update     args( ( void ) );
void	obj_update      args( ( void ) );
void	aggr_update     args( ( void ) );
void	comb_update     args( ( void ) );	/* XOR */
void    auc_update      args( ( void ) );       /* Altrag */
void    rdam_update     args( ( void ) );       /* Altrag */
/*void    vamdam_update   args( ( void ) );*/       /* Decklarean */
void	arena_update	args( ( void ) );	/* Altrag */
void    strew_corpse    args( ( OBJ_DATA *obj, AREA_DATA *inarea ) );
/*
void    wind_update     args( ( void ) );    */   /* Altrag & TRI */
void    orprog_update   args( ( void ) );
void    trap_update     args( ( void ) );
void    rtime_update    args( ( void ) );   /* Timed room progs */
void    quest_update    args( ( void ) );   /* quest.c */
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

    add_hp      = con_app[get_curr_con( ch )].hitp + number_range(
		    class_table[prime_class(ch)].hp_min,
		    class_table[prime_class(ch)].hp_max );
    add_mana    = has_spells( ch )
        ? number_range(2, ( 2 * get_curr_int( ch ) + get_curr_wis( ch ) ) / 6 )
	: 0;
    add_move    =
      number_range( 5, ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 4 );
    add_prac    = wis_app[get_curr_wis( ch )].practice;

    add_hp               = UMAX(  1, add_hp   );
    add_mana             = UMAX(  0, add_mana );
    add_move             = UMAX( 10, add_move );

    ch->perm_hit 	+= add_hp;
    if ( !is_class( ch, CLASS_VAMPIRE ) )
        ch->perm_mana	+= add_mana;
    else
        {
         add_mana -= add_mana / 2;
         ch->perm_bp	+= add_mana;
        }
    ch->perm_move	+= add_move;
    ch->practice	+= add_prac;

    if ( !IS_NPC( ch ) )
	REMOVE_BIT( ch->act, PLR_BOUGHT_PET );
    sprintf( buf,
	"Your gain is: %d/%d hp, %d/%d %s, %d/%d mv %d/%d prac.\n\r",
	add_hp,		MAX_HIT( ch ),
	add_mana,	MT_MAX( ch ),
	is_class( ch, CLASS_VAMPIRE ) ? "bp" : "m",
	add_move,	MAX_MOVE( ch ),
	add_prac,	ch->practice );
/*
    if ( !is_class( ch, CLASS_VAMPIRE ) )
       sprintf( buf,
	    "Your gain is: %d/%d hp, %d/%d m, %d/%d mv %d/%d prac.\n\r",
	    add_hp,	ch->perm_hit,
	    add_mana,	ch->perm_mana,
	    add_move,	ch->perm_move,
	    add_prac,	ch->practice );
    else
       sprintf( buf,
	    "Your gain is: %d/%d hp, %d/%d bp, %d/%d mv %d/%d prac.\n\r",
	    add_hp,	ch->perm_hit,
	    add_mana,	ch->perm_bp,
	    add_move,	ch->perm_move,
	    add_prac,	ch->practice );
*/
    send_to_char(AT_WHITE, buf, ch );
    save_char_obj( ch, FALSE );

    return;
}



void gain_exp( CHAR_DATA *ch, int gain )
{
    char buf [ MAX_STRING_LENGTH ];
    if ( IS_NPC( ch ) || ch->level >= L_CHAMP3 )
	return;
    ch->exp = UMAX( 1000, ch->exp + gain );
/*
    while ( ( ch->level < LEVEL_HERO && ch->exp >= 1000 * ( ch->level + 1 ) )
    || ( ch->level < L_CHAMP1 && ch->exp >= 400000 )
    || ( ch->level < L_CHAMP2 && ch->exp >= 1000000 )
    || ( ch->level < L_CHAMP3 && ch->exp >= 2000000 ) ) */
    while ( ch->exp >= xp_tolvl( ch ) )
    {
	send_to_char(AT_BLUE, "You raise a level!!  ", ch );
	ch->level += 1;
        sprintf( buf, "%s just made level %d!", ch->name, ch->level);
        wiznet(buf,ch,NULL,WIZ_LEVELS,0,0);
        info("%s advances to level %d!", (int)ch->name, ch->level);
	advance_level( ch );
    }

    return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;


    if ( IS_NPC( ch ) )
    {
	gain = ch->level * 3 / 2;
    }
    else
    {
	gain = UMIN( 5, ch->level );
/*
    if ( ch->level < LEVEL_IMMORTAL && ch->class == CLASS_VAMPIRE )
     if ( !IS_SET( ch->in_room->room_flags, ROOM_INDOORS ) )
     {
      if ( time_info.hour > 6 && time_info.hour < 18 )
      {
	send_to_char(AT_RED, "The harsh rays of the sun prevent you from healing.\n\r", ch );
	return 0;
      }
    }
*/

	switch ( ch->position )
	{
	case POS_SLEEPING: gain += ( ch->pcdata->learned[gsn_fastheal] > 0
				&& ch->pcdata->learned[gsn_fastheal] > number_percent( ) ) ?
				get_curr_con( ch ) * 2 :
				get_curr_con( ch );		break;
	case POS_RESTING:  gain += ( ch->pcdata->learned[gsn_fastheal] > 0
				&& ch->pcdata->learned[gsn_fastheal] > number_percent( ) ) ?
				get_curr_con( ch ) :
				get_curr_con( ch ) / 2;		break;
	default: gain += ( ch->pcdata->learned[gsn_fastheal] > 0
		      && ch->pcdata->learned[gsn_fastheal] > number_percent( ) ) ?
		      get_curr_con( ch ) : 0;
	}

	if ( ch->pcdata->condition[COND_FULL  ] == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0
	&& !is_class( ch, CLASS_VAMPIRE ) )
	    gain /= 2;

    }
/*
    if ( ch->class == CLASS_VAMPIRE )
     if ( !IS_SET( ch->in_room->room_flags, ROOM_INDOORS ) )
     {
      if ( time_info.hour > 23 || time_info.hour < 1 )
      {
	gain *= 4;
      }
    }
*/
    if ( IS_AFFECTED( ch, AFF_POISON ) && gain > 0 )
	gain /= 10;

/* Trolls gain twice as much hp per tick */
    if ( ch->race == RACE_TROLL )
       gain *= 5;
/* Ward of Healing */
    if ( is_raffected( ch->in_room, gsn_ward_heal ) )
      {
      send_to_char(AT_WHITE, "The wards of healing soothe your wounds.\n\r", ch );
      gain += 100;
      }
    return UMIN( gain, MAX_HIT(ch) - ch->hit );
}



int mana_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC( ch ) )
    {
	gain = ch->level;
    }
    else
    {
	gain = UMIN( 5, ch->level / 2 );

	switch ( ch->position )
	{
	case POS_SLEEPING: gain += get_curr_int( ch ) * 2;	break;
	case POS_RESTING:  gain += get_curr_int( ch );		break;
	}

	if ( ch->pcdata->condition[COND_FULL  ] == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0
	&& !is_class( ch, CLASS_VAMPIRE ) )
	    gain /= 2;
	if ( ch->race == RACE_ELF || ch->race == RACE_ELDER )
	    gain *= 2;
    }

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 10;

    return UMIN( gain, MAX_MOVE(ch) - ch->mana );
}



int move_gain( CHAR_DATA *ch )
{
    int gain;

    if ( IS_NPC( ch ) )
    {
	gain = ch->level;
    }
    else
    {
	gain = UMAX( 15, 2 * ch->level );

	switch ( ch->position )
	{
	case POS_SLEEPING: gain += get_curr_dex( ch );		break;
	case POS_RESTING:  gain += get_curr_dex( ch ) / 2;	break;
	}

	if ( ch->pcdata->condition[COND_FULL  ] == 0 )
            gain /= 2;

        if ( ch->pcdata->condition[COND_THIRST] == 0
	&& !is_class( ch, CLASS_VAMPIRE ) )
	    gain /= 2;
    }

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 10;

    return UMIN( gain, MAX_MOVE(ch) - ch->move );
}



void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;
    int hploss = 1;

    if ( value == 0 || IS_NPC( ch ) || ch->level >= L_APP )
	return;

    if ( ch->level >= LEVEL_HERO && iCond != COND_DRUNK )
      return;

    condition				= ch->pcdata->condition[ iCond ];
					 /* Used to be 48 */
    ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 58 );
    hploss = (ch->level < 10) ? 1 : (ch->level < 20) ? 2 : 3;

    if ( ch->pcdata->condition[iCond] == 0 )
    {
	switch ( iCond )
	{
	case COND_FULL:
	    send_to_char(AT_ORANGE, "You are hungry.\n\r",    ch );
	    break;

	case COND_THIRST:
	    if ( !is_class( ch, CLASS_VAMPIRE ) )
		send_to_char(AT_BLUE, "You are thirsty.\n\r",   ch );
	    break;

	case COND_DRUNK:
	    if ( condition != 0 )
		send_to_char(AT_BLUE, "You are sober.\n\r", ch );
	    break;
	}

	if ( (iCond != COND_DRUNK) && (ch->hit - hploss) > 5 )
	      ch->hit = (ch->hit - hploss);
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

    /* Examine all mobs. */
    for ( ch = char_list; ch; ch = ch->next )
    {
        if ( ch->deleted )
	    continue;

	if ( IS_NPC(ch) && (ch->wait -= PULSE_MOBILE) < 0 )
	  ch->wait = 0;

	if ( !IS_NPC( ch )
	    || !ch->in_room
	    || IS_AFFECTED( ch, AFF_STUN )
	    || IS_AFFECTED( ch, AFF_CHARM )
	    || ch->wait > 0 )
	    continue;

	/* Examine call for special procedure */
	if ( ch->spec_fun != 0 )
	{
	    if ( ( *ch->spec_fun ) ( ch ) )
		continue;
	}

	/* That's all for sleeping / busy monster */
	if ( ch->position < POS_STANDING )
	    continue;

        if ( ch->in_room->area->nplayer > 0 )
          {
           mprog_random_trigger( ch );
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
#ifdef NEW_MONEY
		if ( CAN_WEAR( obj, ITEM_TAKE ) &&
		   ( ( (obj->cost.gold*C_PER_G) + (obj->cost.silver*S_PER_G) +
		     (obj->cost.copper) ) > max ) && can_see_obj(ch, obj) )
		{
		   obj_best	= obj;
		   max		= ( (obj->cost.gold*C_PER_G) + (obj->cost.silver*S_PER_G) +
				    (obj->cost.copper) );
		}
#else
		if ( CAN_WEAR( obj, ITEM_TAKE )
		    && obj->cost > max
		    && can_see_obj( ch, obj ) )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
#endif
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act(AT_GREY, "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

	/* Wander */
	if ( !IS_SET( ch->act, ACT_SENTINEL )
	    && ( door = number_bits( 5 ) ) <= 5
	    && ( pexit = ch->in_room->exit[door] )
	    &&   pexit->to_room
	    &&   !IS_SET( pexit->exit_info, EX_CLOSED )
	    &&   !IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB )
	    &&   !ch->hunting
	    && ( !IS_SET( ch->act, ACT_STAY_AREA )
		||   pexit->to_room->area == ch->in_room->area ) )
	{
	    move_char( ch, door, FALSE );
	    if ( ch->position < POS_STANDING )
	        continue;
	}

	/* Flee */
	if ( ch->hit < ( MAX_HIT(ch) / 2 )
	    && ( door = number_bits( 3 ) ) <= 5
	    && ( pexit = ch->in_room->exit[door] )
	    &&   pexit->to_room
	    &&   !IS_SET( pexit->exit_info, EX_CLOSED )
	    &&   !IS_SET( pexit->to_room->room_flags, ROOM_NO_MOB ) )
	{
	    CHAR_DATA *rch;
	    bool       found;

	    found = FALSE;
	    for ( rch  = pexit->to_room->people;
		  rch;
		  rch  = rch->next_in_room )
	    {
	        if ( rch->deleted )
		    continue;
		if ( !IS_NPC( rch ) )
		{
		    found = TRUE;
		    break;
		}
	    }
	    if ( !found )
		move_char( ch, door, FALSE );
	}
	if ( ch->hunting )
	  hunt_victim( ch );
    }
    MOBtrigger = TRUE;
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
    char *suf;
    int   day;

    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  1:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "&cThe witching hour has passed.\n\r" );
	break;

    case  5:
	weather_info.sunlight = SUN_DAWN;
	strcat( buf, "&cThe darkness begins to lift.\n\r" );
	break;

    case 6:
	weather_info.sunlight = SUN_MORNING;
	strcat( buf, "The &Ysun&B rises triumphantly in the east.\n\r" );
	break;

    case 11:
	weather_info.sunlight = SUN_NOON;
	strcat( buf, "&YThe sun sits high in its heavenly cradle.&w\n\r" );
	break;

    case 13:
	weather_info.sunlight = SUN_AFTERNOON;
	strcat( buf, "&YThe afternoon sun bakes the land.&w\n\r" );
	break;

    case 18:
	weather_info.sunlight = SUN_DUSK;
	strcat( buf, "&cThe sun begins its slow descent in the west.&w\n\r" );
	break;

    case 19:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "&cDarkness spreads its embrace about the land.&w\n\r" );
	break;

    case 24:
	weather_info.sunlight = SUN_MIDNIGHT;
	strcat( buf, "&rThe witching hour has come.&w\n\r" );
	time_info.hour = 0;
	time_info.day++;

        day = time_info.day + 1;
             if ( day > 4 && day <  20 ) suf = "th";
        else if ( day % 10 ==  1       ) suf = "st";
        else if ( day % 10 ==  2       ) suf = "nd";
        else if ( day % 10 ==  3       ) suf = "rd";
        else                             suf = "th";

        sprintf( buf,
            "The day of %s, %d%s of the Month of %s has begun.",
            day_name[day % 7],
            day, suf,
            month_name[time_info.month] );
        info( buf, 0, 0 );
        buf[0] = '\0';
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
	    strcat( buf, "The sky is getting &zcloudy.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_CLOUDY:
	if (     weather_info.mmhg <  970
	    || ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "It starts to rain.\n\r" );
	    weather_info.sky = SKY_RAINING;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "The &zclouds&B disappear.\n\r" );
	    weather_info.sky = SKY_CLOUDLESS;
	}
	break;

    case SKY_RAINING:
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "&WLightning&B flashes in the sky.\n\r" );
	    weather_info.sky = SKY_LIGHTNING;
	}

	if (     weather_info.mmhg > 1030
	    || ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The rain stopped.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_LIGHTNING:
	if (     weather_info.mmhg > 1010
	    || ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The &Wlightning&B has stopped.\n\r" );
	    weather_info.sky = SKY_RAINING;
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
		&& IS_AWAKE  ( d->character )
		&& !IS_SET( d->character->in_room->room_flags, ROOM_INDOORS ) )
		send_to_char(AT_BLUE, buf, d->character );
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
    CHAR_DATA *ch_next;	/* XOR */
    time_t     save_time;

    ch_save	= NULL;
    ch_quit	= NULL;
    save_time	= current_time;

    for ( ch = char_list; ch; ch = ch->next )
    {
	AFFECT_DATA *paf;
	ROOM_AFFECT_DATA *raf;
	POWERED_DATA *pd, *pd_next;
	if ( ch->deleted )
	    continue;

	/*
	 * Find dude with oldest save time.
	 */
	if ( !IS_NPC( ch )
	    && ( !ch->desc || ch->desc->connected == CON_PLAYING )
	    &&   ch->level >= 2
	    &&   ch->save_time < save_time )
	{
	    ch_save	= ch;
	    save_time	= ch->save_time;
	}

	if ( ch->position >= POS_STUNNED )
	{
	    if ( ch->hit  < MAX_HIT(ch) )
		ch->hit  += hit_gain( ch );

	    if ( ch->mana < MAX_MANA(ch) )
		ch->mana += mana_gain( ch );

	    if ( ch->move < MAX_MOVE(ch) )
		ch->move += move_gain( ch );
	    if ( ch->position == POS_STUNNED )
	      {
	       ch->position = POS_STANDING;
	       update_pos( ch );
	      }
	}
        if (is_affected(ch, gsn_berserk)
          && !ch->fighting )
        {
          affect_strip( ch, gsn_berserk );
        }

        if (IS_AFFECTED2(ch, AFF_BERSERK)
          && !ch->fighting )
        {
	  if (is_affected( ch, gsn_frenzy ) )
	  send_to_char(AT_WHITE, "You feel less frenzied.\n\r", ch );
	  else
          send_to_char(AT_WHITE, "The rage leaves you.\n\r", ch );
          REMOVE_BIT( ch->affected_by2, AFF_BERSERK );
        }

	if ( ch->position == POS_STUNNED )
	    update_pos( ch );

	if ( !IS_NPC( ch ) && ( ch->level < LEVEL_IMMORTAL
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
		    act(C_DEFAULT, "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act(C_DEFAULT, "$p goes out.", ch, obj, NULL, TO_CHAR );
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
		    send_to_char(AT_GREEN, "You disappear into the void.\n\r", ch );
		    act(AT_GREEN, "$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM );
		    save_char_obj( ch, FALSE );
		    char_from_room( ch );
		    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }

	    if ( ch->timer > 20 && !IS_SWITCHED( ch ) )
		ch_quit = ch;

	    gain_condition( ch, COND_DRUNK,  -8 );
	    gain_condition( ch, COND_FULL,   -1 );
	    gain_condition( ch, COND_THIRST, -1 );
	}

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
			send_to_char(C_DEFAULT, skill_table[paf->type].msg_off, ch );
			send_to_char(C_DEFAULT, "\n\r", ch );
		        if ( skill_table[paf->type].room_msg_off )
		        {
			 act(C_DEFAULT,skill_table[paf->type].room_msg_off,
			     ch, NULL, NULL, TO_ROOM );
			}
		    }
		}

		affect_remove( ch, paf );
/*		if ( paf->bitvector == AFF_FLYING )
		  check_nofloor( ch ); --Angi */
	    }
	}
	for ( paf = ch->affected2; paf; paf = paf->next )
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
			send_to_char(C_DEFAULT, skill_table[paf->type].msg_off, ch );
			send_to_char(C_DEFAULT, "\n\r", ch );
		        if ( skill_table[paf->type].room_msg_off )
		        {
			 act(C_DEFAULT,skill_table[paf->type].room_msg_off,
			     ch, NULL, NULL, TO_ROOM );
			}
		    }
		}

		affect_remove2( ch, paf );
             }
	  }
	for ( pd = ch->powered; pd; pd = pd_next )
	  {
	  if ( !pd )
	    break;
	  pd_next = pd->next;
	  raf = pd->raf;
	  if ( MT( ch ) < pd->cost )
	    raffect_remove( raf->room, ch, raf );
	  else
      if ( is_class( ch, CLASS_VAMPIRE ) ) {
        ch->bp -= pd->cost;
      } else {
        ch->mana -= pd->cost;
      }
	  }
	if ( ch->gspell && --ch->gspell->timer <= 0 )
	{
	  send_to_char(AT_BLUE, "You slowly lose your concentration.\n\r",ch);
	  end_gspell( ch );
	}

	if ( ch->ctimer > 0 )
	  ch->ctimer--;

	/*
	 * Careful with the damages here,
	 *   MUST NOT refer to ch after damage taken,
	 *   as it may be lethal damage (on NPC).
	 */
	if ( IS_AFFECTED( ch, AFF_POISON ) )
	{
	    send_to_char(AT_GREEN, "You shiver and suffer.\n\r", ch );
	    act(AT_GREEN, "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
	    damage( ch, ch, 10, gsn_poison );
	}
	if ( is_affected( ch, gsn_drowfire ) )
	{
	    send_to_char(AT_RED, "The drow fire burns your skin.\n\r", ch );
	    damage( ch, ch, 25, gsn_drowfire );
	}
	if ( IS_AFFECTED( ch, AFF_FLAMING ) )
	{
	    send_to_char(AT_RED, "Your skin blisters and burns.\n\r", ch );
	    act(AT_RED, "$n's body blisters and burns as it is licked in flames.", ch, NULL, NULL, TO_ROOM );
	    damage( ch, ch, ch->level/4, gsn_incinerate );
	}
	else if ( ch->position == POS_INCAP )
	{
	    damage( ch, ch, 1, TYPE_UNDEFINED );
	}
	else if ( ch->position == POS_MORTAL )
	{
	    damage( ch, ch, 2, TYPE_UNDEFINED );
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
	    {
/*	        send_to_char( AT_ORANGE, "Autosaved.\n\r", ch ); */
		save_char_obj( ch, FALSE );
	    }
	    if ( ch == ch_quit )
		do_quit( ch, "" );
	}
    }
/* XOR */
    for(ch = char_list;ch != NULL;ch = ch_next)
    {
      ch_next = ch->next;
      if(ch->summon_timer <= 0)
        ;
      else
        ch->summon_timer--;
      if(IS_NPC(ch) && ch->summon_timer == 0)
      {
        act(AT_BLUE, "$n is consumed by a swirling vortex.",
         ch, NULL, NULL, TO_ROOM);
        extract_char(ch, TRUE);
      }
    }
/* END */
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

    for ( obj = object_list; obj; obj = obj_next )
    {
	CHAR_DATA *rch;
	char      *message;

	obj_next = obj->next;
	if ( obj->deleted )
	    continue;

	if ( obj->timer < -1 )
	    obj->timer = -1;

	if ( obj->timer < 0 )
	    continue;

	/*
	 *  Bug fix:  used to shift to obj_free if an object whose
	 *  timer ran out contained obj->next.  Bug was reported
	 *  only when the object(s) inside also had timer run out
	 *  during the same tick.     --Thelonius (Monk)
	 */
	if ( --obj->timer == 0 )
	{
	  AREA_DATA *inarea = NULL;
	  bool pccorpse = FALSE;

	    switch ( obj->item_type )
	    {
	    	default:              message = "$p vanishes.";         break;
    		case ITEM_FOUNTAIN:   message = "$p dries up.";         break;
    		case ITEM_CORPSE_NPC: message = "$p decays into dust."; break;
    		case ITEM_CORPSE_PC:  message = "$p decays into dust.";
    		                      pccorpse = TRUE; break;
    		case ITEM_FOOD:       message = "$p decomposes.";       break;
    		case ITEM_BLOOD:      message = "$p soaks into the ground."; break;
    		case ITEM_PORTAL:     message = "$p shimmers and is gone."; break;
	        case ITEM_VODOO:      message = "$p slowly fades out of existance."; break;
	        case ITEM_BERRY:      message = "$p rots away."; break;
	    }

	    if ( obj->carried_by )
	    {
	        act(C_DEFAULT, message, obj->carried_by, obj, NULL, TO_CHAR );
	    }
	    else
	      if ( obj->in_room
		  && ( rch = obj->in_room->people ) )
	      {
		  act(C_DEFAULT, message, rch, obj, NULL, TO_ROOM );
		  act(C_DEFAULT, message, rch, obj, NULL, TO_CHAR );
	      }

            if ( obj->in_room )
              inarea = obj->in_room->area;

	    if ( obj == object_list )
	    {
	      if ( !pccorpse || !inarea )
	      {
	        extract_obj( obj );
	        obj_next = object_list;
	      }
	     else
	       strew_corpse( obj, inarea );
	    }
	    else				/* (obj != object_list) */
	    {
	        OBJ_DATA *previous;

	        for ( previous = object_list; previous;
		     previous = previous->next )
	        {
		    if ( previous->next == obj )
	     		break;
	        }

		if ( !previous )  /* Can't see how, but... */
		    bug( "Obj_update: obj %d no longer in object_list",
    			obj->pIndexData->vnum );

              if ( !pccorpse || !inarea )
              {
	        extract_obj( obj );
	        obj_next = previous->next;
	      }
	     else
	       strew_corpse( obj, inarea );
	    }
	}
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
    /*
     * Let's not worry about link dead characters. -Kahn
     */
    for ( d = descriptor_list; d; d = d->next )
    {
	ch = d->character;

	if ( d->connected != CON_PLAYING
	    || !ch
/*	    || ch->level >= LEVEL_IMMORTAL*/
	    || !ch->in_room )
	    continue;

	/* mch wont get hurt */
	for ( mch = ch->in_room->people; mch; mch = mch->next_in_room )
	{
	    int count;

	    if ( !IS_NPC( mch )
		|| mch->deleted
		|| IS_AFFECTED( mch, AFF_STUN )
/*		|| !IS_SET( mch->act, ACT_AGGRESSIVE )*/
		|| mch->fighting
		|| IS_AFFECTED( mch, AFF_CHARM )
		|| !IS_AWAKE( mch )
/*		|| ( IS_SET( mch->act, ACT_WIMPY ) && IS_AWAKE( ch ) )*/
		|| !can_see( mch, ch )
/*		|| IS_AFFECTED( ch, AFF_PEACE )*/
		|| mch->wait > 0
		|| ch->wait > 0 )
		continue;

	    if ( IS_NPC( mch ) && mch->mpactnum > 0
		&& mch->in_room->area->nplayer > 0 )
	    {
	      MPROG_ACT_LIST * tmp_act, *tmp2_act;
	      for ( tmp_act = mch->mpact; tmp_act != NULL;
		    tmp_act = tmp_act->next )
	      {
		mprog_wordlist_check( tmp_act->buf,mch, tmp_act->ch,
				      tmp_act->obj, tmp_act->vo, ACT_PROG );
		free_string( tmp_act->buf );
	      }
	      for ( tmp_act = mch->mpact; tmp_act != NULL; tmp_act = tmp2_act )
	      {
		tmp2_act = tmp_act->next;
		free_mem( tmp_act, sizeof( MPROG_ACT_LIST ) );
	      }
	      mch->mpactnum = 0;
	      mch->mpact    = NULL;
	    }

	    if ( !IS_SET( mch->act, ACT_AGGRESSIVE )
	      || (IS_SET( mch->act, ACT_WIMPY ) && IS_AWAKE( ch ) )
	      || IS_AFFECTED( ch, AFF_PEACE ) )
	      continue;

	    /*
	     * Ok we have a 'ch' player character and a 'mch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     */
            count = 0;
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
		    if ( number_range( 0, count ) == 0 )
			victim = vch;
		    count++;
		}
	    }

	    if ( !victim )
	        continue;

	    multi_hit( mch, victim, TYPE_UNDEFINED );


/*        if ( IS_NPC( mch ) && mch->mpactnum > 0
	    && mch->in_room->area->nplayer > 0 )
        {
            MPROG_ACT_LIST * tmp_act, *tmp2_act;
	    for ( tmp_act = mch->mpact; tmp_act != NULL;
		 tmp_act = tmp_act->next )
	    {
                 mprog_wordlist_check( tmp_act->buf,mch, tmp_act->ch,
                               	      tmp_act->obj, tmp_act->vo, ACT_PROG );
                 free_string( tmp_act->buf );
            }
            for ( tmp_act = mch->mpact; tmp_act != NULL; tmp_act = tmp2_act )
	    {
                 tmp2_act = tmp_act->next;
                 free_mem( tmp_act, sizeof( MPROG_ACT_LIST ) );
            }
            mch->mpactnum = 0;
            mch->mpact    = NULL;
        }*/
	} /* mch loop */

    } /* descriptor loop */

    return;
}

/* Update the check on time for autoshutdown */
void time_update( void )
{
    FILE            *fp;
    char            *curr_time;
    char             buf [ MAX_STRING_LENGTH ];

    if ( down_time == "*" )
        return;
    curr_time = ctime( &current_time );
    if ( !str_infix( warning1, curr_time ) )
    {
	sprintf( buf, "First Warning!\n\r%s at %s system time\n\r"
		      "Current time is %s\n\r",
		(stype == 0 ? "Reboot" : "Shutdown"),
		down_time,
		curr_time );
	send_to_all_char( buf );
	free_string( warning1 );
	warning1 = str_dup( "*" );
    }
    if ( !str_infix( warning2, curr_time ) )
    {
	sprintf( buf, "Second Warning!\n\r%s at %s system time\n\r"
		      "Current time is %s\n\r",
		(stype == 0 ? "Reboot" : "Shutdown"),
		down_time,
		curr_time );
	send_to_all_char( buf );
	free_string( warning2 );
	warning2 = str_dup( "*" );
    }
    if ( !str_infix( down_time, curr_time ) )
    {
	/* OLC 1.1b */
	do_asave( NULL, "" );

        if ( stype == 1 )
        {
	send_to_all_char( "Shutdown by system.\n\r" );
	log_string( "Shutdown by system.", CHANNEL_GOD, -1 );

	end_of_game( );
	}
       else
        {
          send_to_all_char( "Reboot by system.\n\r" );
          log_string( "Reboot by system.", CHANNEL_GOD, -1 );
          end_of_game( );
          merc_down = TRUE;
          return;
        }

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
            CHAR_DATA *ch;
            CHAR_DATA *ch_next;
            OBJ_DATA  *obj;
            OBJ_DATA  *obj_next;
    extern  bool       delete_obj;
    extern  bool       delete_char;

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
		    sprintf( log_buf, "Char: %s", ch->name );
		    bug( log_buf, 0 );
		    continue;
		  }
		}

/*		paf->next   = affect_free;
		affect_free = paf;*/
/*		free_mem( paf, sizeof( *paf ) );*/
		free_affect( paf );
	      }
	    }


	    for ( paf = ch->affected2; paf; paf = paf_next )
	      {
		paf_next = paf->next;

		if ( paf->deleted || ch->deleted )
		  {
		    if ( ch->affected2 == paf )
		      {
			ch->affected2 = paf->next;
		      }
		    else
		      {
			AFFECT_DATA *prev;

			for ( prev = ch->affected2; prev; prev = prev->next )
			  {
			    if ( prev->next == paf )
			      {
				prev->next = paf->next;
				break;
			      }
			  }

			if ( !prev )
			  {
			    bug( "List_update2: cannot find paf on ch.", 0 );
			    sprintf( log_buf, "Char: %s", ch->name ? ch->name
			     : "(Unknown)" );
			    bug( log_buf, 0 );
			    continue;
			  }
		      }

/*		    paf->next   = affect_free;
		    affect_free = paf;*/
/*		    free_mem( paf, sizeof( *paf ) ); */
 		    free_affect ( paf );
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
/*		  free_string( ed->description );
		  free_string( ed->keyword     ); */
/*		  ed->next         = extra_descr_free;
		  extra_descr_free = ed;*/
/*		  free_mem( ed, sizeof( *ed ) );*/
  		  free_extra_descr( ed );
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

/*		  paf->next   = affect_free;
		  affect_free = paf;*/
/*		  free_mem( paf, sizeof( *paf ) ); */
		  free_affect ( paf );
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

/*	      obj->next	= obj_free;
	      obj_free	= obj;*/
	      free_mem( obj, sizeof( *obj ) );
	    }
	}

    delete_obj  = FALSE;
    delete_char = FALSE;
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
    static int pulse_mobile;
    static int pulse_violence;
    static int pulse_point;
    static int pulse_db_dump;	/* OLC 1.1b */
    static int pulse_combat;	/* XOR pkill */

    /* OLC 1.1b */
    if ( --pulse_db_dump  <= 0 && port != 4000 )
    {
	pulse_db_dump	= PULSE_DB_DUMP;
	do_asave( NULL, "" );
        /*save_player_list(); */
    }

    if ( --pulse_area     <= 0 )
    {
	pulse_area	= number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
	area_update	( );
    }

    if ( --pulse_violence <= 0 )
    {
	pulse_violence  = PULSE_VIOLENCE;
	violence_update ( );
    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile    = PULSE_MOBILE;
	mobile_update   ( );
	rdam_update ( );
        orprog_update( );
    }

    if ( --pulse_point    <= 0 )
    {
        wiznet("TICK! ;p",NULL,NULL,WIZ_TICKS,0,0);
	pulse_point     = number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 );
	weather_update  ( );
	rtime_update    ( );
/*	vamdam_update   ( );
	wind_update     ( ); */
        quest_update    ( );
	char_update     ( );
	obj_update      ( );
	list_update     ( );
    }

/* XOR causing alot of lag */
/* yeah yeah, see if its fixed now =) */
    if(--pulse_combat <= 0)
    {
      pulse_combat = PULSE_PER_SECOND;
      comb_update();
    }
/* END */

/* Auction timer update -- Altrag */
    if ( auc_count >= 0 && ++auc_count % (8 * PULSE_PER_SECOND) == 0 )
      auc_update ( );

    if ( arena.cch && ++arena.count % (5 * PULSE_PER_SECOND) == 0 )
      arena_update ( );

    chat_update( );

//    time_update( );
    aggr_update( );
    tail_chain( );
    return;
}

/* X combat timer update */
void comb_update()
{
  CHAR_DATA *ch;
  for ( ch = char_list; ch != NULL; ch = ch->next )
  {
    if ( ch->deleted )
      continue;
    if(--ch->combat_timer < 0)
      ch->combat_timer = 0;
  }
}

void arena_update()
{
  if ( !arena.cch )
    return;
  switch ( arena.count / (5 * PULSE_PER_SECOND) )
  {
  case 1:
  case 2:
  case 3:
    if ( arena.och )
    {
    char buf[MAX_INPUT_LENGTH];
    sprintf(buf, "&C%s &coffering &C%s &ca challenge for &W%d &ccoins.",
	    arena.cch->name, arena.och->name, arena.award);
    challenge(buf, 0, 0);
    }
    else
    challenge("&C%s &coffering challenge for &W%d &ccoins.",
		(int)(arena.cch->name), arena.award);
    return;
  }
  if ( arena.och )
  challenge("&C%s &cwimps out and refuses &C%s&c's challenge.",
	   (int)(arena.och->name), (int)(arena.cch->name) );
  else
  challenge("&C%s&c's challenge not accepted.  Opening arena for new challenger.",
       (int)(arena.cch->name), 0);
  send_to_char( C_DEFAULT, "Your challenge was not accepted.  Refunding "
                "award money.\n\r", arena.cch );
  /* Arena master takes 1/5.. *wink */
#ifdef NEW_MONEY
  arena.cch->money.gold += ((arena.award*4)/5);
#else
  arena.cch->gold += ((arena.award*4)/5);
#endif
  arena.cch = NULL;
  arena.och = NULL;
  arena.award = 0;
  return;
}

/* Auctioneer timer update -- Altrag */
void auc_update()
{
  extern OBJ_DATA *auc_obj;
  extern CHAR_DATA *auc_held;
  extern CHAR_DATA *auc_bid;
#ifdef NEW_MONEY
  extern MONEY_DATA auc_cost;
#else
  extern int auc_cost;
#endif
  char buf[MAX_STRING_LENGTH];

  if ( !auc_obj )
    return;

  if ( !auc_held )
  {
    bug( "Auc_update: auc_obj found without auc_held.",0);
    return;
  }

  switch ( auc_count / (8 * PULSE_PER_SECOND) )
  {
  case 1:
#ifdef NEW_MONEY
    sprintf( buf, "%s for %s (going ONCE)", auc_obj->short_descr, money_string( &auc_cost ) );
#else
    sprintf( buf, "%s for %d gold coins (going ONCE).", auc_obj->short_descr, auc_cost );
#endif
    auc_channel( buf );
    sprintf( buf, "%s auctioning %s.", auc_held->name, auc_obj->name );
    log_string( buf, CHANNEL_GOD, -1 );
    return;
  case 2:
#ifdef NEW_MONEY
    sprintf( buf, "%s for %s (going TWICE)", auc_obj->short_descr, money_string( &auc_cost ) );
#else
    sprintf( buf, "%s for %d gold coins (going TWICE).", auc_obj->short_descr, auc_cost );
#endif
    auc_channel( buf );
    sprintf( buf, "%s auctioning %s.", auc_held->name, auc_obj->name );
    log_string( buf, CHANNEL_GOD, -1 );
    return;
  case 3:
#ifdef NEW_MONEY
    sprintf( buf, "%s for %s (going THRICE)", auc_obj->short_descr, money_string(&auc_cost) );
#else
    sprintf( buf, "%s for %d gold coins (going THRICE).", auc_obj->short_descr, auc_cost );
#endif
    auc_channel( buf );
    sprintf( buf, "%s auctioning %s.", auc_held->name, auc_obj->name );
    log_string( buf, CHANNEL_GOD, -1 );
    return;
  }

#ifdef NEW_MONEY

  if ( auc_bid && ( ( (auc_bid->money.gold*C_PER_G) + (auc_bid->money.silver*S_PER_G) +
       (auc_bid->money.copper) ) > ( (auc_cost.gold*C_PER_G) + (auc_cost.silver*S_PER_G) +
       (auc_cost.copper) ) ) )
  {
    sprintf( buf, "%s SOLD! to %s for %s", auc_obj->short_descr, auc_bid->name,
	     money_string( &auc_cost ) );
    add_money( &auc_held->money, &auc_cost );
    spend_money( &auc_bid->money, &auc_cost );

#else
  if ( auc_bid && auc_bid->gold >= auc_cost )
  {
    sprintf( buf, "%s for %d gold coins SOLD! to %s.", auc_obj->short_descr, auc_cost,
       auc_bid->name );
    auc_bid->gold -= auc_cost;
    auc_held->gold += auc_cost;
#endif
    obj_to_char( auc_obj, auc_bid );
    act( AT_DGREEN, "$p appears in your hands.", auc_bid, auc_obj, NULL, TO_CHAR );
    act( AT_DGREEN, "$p appears in the hands of $n.", auc_bid, auc_obj, NULL,
     TO_ROOM );
  }
  else if ( auc_bid )
   {
#ifdef NEW_MONEY
     sprintf( buf, "Amount not carried for %s, ending auction.", auc_obj->short_descr );
#else
     sprintf( buf, "%d gold coins not carried for %s, ending auction.", auc_cost, auc_obj->short_descr );
#endif
     obj_to_char( auc_obj, auc_held );
     act( AT_DGREEN, "$p appears in your hands.", auc_held, auc_obj, NULL, TO_CHAR );
     act( AT_DGREEN, "$p appears in the hands of $n.", auc_held, auc_obj, NULL, TO_ROOM );
   }
  else
   {
     sprintf( buf, "%s not sold, ending auction.", auc_obj->short_descr );
     obj_to_char( auc_obj, auc_held );
     act( AT_DGREEN, "$p appears in your hands.", auc_held, auc_obj, NULL, TO_CHAR );
     act( AT_DGREEN, "$p appears in the hands of $n.", auc_held, auc_obj, NULL,
       TO_ROOM );
   }
  auc_channel( buf );

  auc_count = -1;
#ifdef NEW_MONEY
  auc_cost.gold = auc_cost.silver = auc_cost.copper = 0;
#else
  auc_cost = 0;
#endif
  auc_obj = NULL;
  auc_held = NULL;
  auc_bid = NULL;
  return;
}

void rdam_update( )
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *ch;

  for ( d = descriptor_list; d; d = d->next )
  {
    if ( d->connected != CON_PLAYING )
      continue;

   ch = d->original ? d->original : d->character;
    if ( !( ch->in_room ) )
      continue;

   if ( ch->level < L_APP )
    if ( IS_SET( ch->in_room->room_flags, ROOM_DAMAGE ) )
    {
    /*  if ( ch->hit <= ch->in_room->rd ) */
      {
         send_to_char( AT_RED, "Pain shoots through your body.\n\r", ch );
      }
      damage( ch, ch, ch->in_room->rd, TYPE_UNDEFINED );
    }
  }

  return;
}

/* This does the damage for vampiers when there out during the day
   basicaly a rip off of rdam_update.  -Decklarean*/
/*
void vamdam_update( )
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *ch;
  for ( d = descriptor_list; d; d = d->next )
  {
    if ( d->connected != CON_PLAYING )
     continue;

    ch = d->original ? d->original : d->character;
    if ( !( ch->in_room ) )
     continue;

    if ( ch->level < L_APP && is_class( ch, CLASS_VAMPIRE ) )
     if ( !IS_SET( ch->in_room->room_flags, ROOM_INDOORS ) )
     {
      if ( time_info.hour > 5 && time_info.hour < 18 )
      {
       send_to_char( AT_RED,
       "Intense pain washes through your body under the light of the sun.\n\r", ch );
       act( AT_RED, "Smoke rises from $n under the glarying light of the sun.", ch,
        NULL, NULL, TO_ROOM );

       damage( ch, ch, (ch->level / 2) , TYPE_UNDEFINED );
       ch->bp -= ch->bp / 6;
      }
    }
  }

  return;
}
*/




/* Wind timer routines.. needs updates for weather stuff.. -- Altrag */
const char *dir_wind [] = {"north", "northeast", "east", "southeast",
			   "south", "southwest", "west", "northwest"};
/*
const char *wind_str [] = {"almost no", "a little bit of", "a strong"};

void wind_update( void )
{
  AREA_DATA *pArea;
  DESCRIPTOR_DATA *d;

  for ( pArea = area_first; pArea; pArea = pArea->next )
  {
    pArea->windstr += number_range( UMIN(pArea->windstr - 5, -15),
				    UMAX(pArea->windstr + 5, 15) );
    if ( pArea->windstr < 0 )
    {
      pArea->windstr = -pArea->windstr;
      pArea->winddir = number_fuzzy(pArea->winddir);
      if (pArea->winddir < 0 )
	pArea->winddir = 7;
      if (pArea->winddir > 7 )
	pArea->winddir = 0;
    }
  }

  for ( d = descriptor_list; d; d = d->next )
  {
    if ( d->connected == CON_PLAYING
	&& IS_OUTSIDE( d->character )
	&& IS_AWAKE( d->character )
	&& !IS_SET(d->character->in_room->room_flags, ROOM_INDOORS ) )
    {
      char buf[MAX_STRING_LENGTH];

      pArea = d->character->in_room->area;
      if ( pArea->windstr > 0 )
	sprintf(buf, "There is %s wind blowing from the %s.\n\r",
		wind_str[pArea->windstr / 5], dir_wind[pArea->winddir]);
      else
	strcpy(buf, "There is no wind at all.\n\r");
      send_to_char(C_DEFAULT, buf, d->character);
    }
  }
  return;
}
*/
/* END */

void strew_corpse( OBJ_DATA *obj, AREA_DATA *inarea )
{
  OBJ_DATA *currobj;
  ROOM_INDEX_DATA *newroom;
  OBJ_DATA *cobj_next;

  for ( currobj = obj->contains; currobj; currobj = cobj_next )
  {
     cobj_next = currobj->next_content;
     switch( currobj->item_type )
     {
       case ITEM_FOOD:
       case ITEM_DRINK_CON:
              currobj->value[3] = 1;
              break;
       case ITEM_POTION:
              if ( number_percent( ) < 20 )
              {
                extract_obj( currobj );
                continue;
              }
              break;
       default: break;
     }

     if ( number_percent( ) < 2 )
     {
       extract_obj( currobj );
       continue;
     }
     if ( number_percent( ) < 30 )
     {
       obj_from_obj( currobj );
       obj_to_room( currobj, obj->in_room );
     }
    else
     {
       obj_from_obj( currobj );
       newroom = get_room_index( number_range( inarea->lvnum, inarea->uvnum ) );
       for ( ; !newroom; )
         newroom = get_room_index( number_range( inarea->lvnum, inarea->uvnum ) );
       obj_to_room( currobj, newroom );
     }
  }
  extract_obj( obj );
  return;
}

void orprog_update( void )
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  AREA_DATA *pArea;

  for ( obj = object_list; obj; obj = obj_next )
  {
    obj_next = obj->next;
    if ( obj->deleted )
      continue;
    /* ie: carried or in room */
    if ( !obj->in_obj && !obj->stored_by &&
	((obj->in_room && obj->in_room->area->nplayer) ||
	 (obj->carried_by && obj->carried_by->in_room &&
	  obj->carried_by->in_room->area->nplayer)) )
      oprog_random_trigger( obj );
  }

  for ( pArea = area_first; pArea; pArea = pArea->next )
    if ( pArea->nplayer > 0 )
    {
      int room;
      ROOM_INDEX_DATA *pRoom;

      for ( room = pArea->lvnum; room <= pArea->uvnum; room++ )
	if ( (pRoom = get_room_index( room )) )
	  rprog_random_trigger( pRoom );
    }

  return;
}

void trap_update( void )
{
  TRAP_DATA *pTrap;

  for ( pTrap = trap_list; pTrap; pTrap = pTrap->next )
    if ( --pTrap->disarm_dur <= 0 )
    {
      pTrap->disarm_dur = 0;
      pTrap->disarmed = FALSE;
    }
  return;
}

void rtime_update( void )
{
  AREA_DATA *pArea;

  for ( pArea = area_first; pArea; pArea = pArea->next )
    if ( pArea->nplayer )
    {
      int room;
      ROOM_INDEX_DATA *pRoom;

      for ( room = pArea->lvnum; room <= pArea->uvnum; room++ )
	if ( (pRoom = get_room_index( room )) )
	  rprog_time_trigger( pRoom, time_info.hour );
    }

  return;
}
