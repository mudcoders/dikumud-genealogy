/***************************************************************************
 *  file: limits.c , Limit and gain control module.        Part of DIKUMUD *
 *  Usage: Procedures controling gain and limit.                           *
 *  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
 *                                                                         *
 *  Copyright (C) 1992, 1993 Michael Chastain, Michael Quan, Mitchell Tse  *
 *  Performance optimization and bug fixes by MERC Industries.             *
 *  You can use our stuff in any way you like whatsoever so long as this   *
 *  copyright notice remains intact.  If you like it please drop a line    *
 *  to mec@garnet.berkeley.edu.                                            *
 *                                                                         *
 *  This is free software and you are benefitting.  We hope that you       *
 *  share your changes too.  What goes around, comes around.               *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "limits.h"
#include "utils.h"
#include "spells.h"
#include "db.h"


extern struct char_data *character_list;
extern struct obj_data *object_list;
extern struct room_data *world;

/* External procedures */

void update_pos( struct char_data *victim );                 /* in fight.c */
struct time_info_data age(struct char_data *ch);



/* When age < 15 return the value p0 */
/* When age in 15..29 calculate the line between p1 & p2 */
/* When age in 30..44 calculate the line between p2 & p3 */
/* When age in 45..59 calculate the line between p3 & p4 */
/* When age in 60..79 calculate the line between p4 & p5 */
/* When age >= 80 return the value p6 */
int graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6)
{

    if (age < 15)
	return(p0);                               /* < 15   */
    else if (age <= 29) 
	return (int) (p1+(((age-15)*(p2-p1))/15));  /* 15..29 */
    else if (age <= 44)
	return (int) (p2+(((age-30)*(p3-p2))/15));  /* 30..44 */
    else if (age <= 59)
	return (int) (p3+(((age-45)*(p4-p3))/15));  /* 45..59 */
    else if (age <= 79)
	return (int) (p4+(((age-60)*(p5-p4))/20));  /* 60..79 */
    else
	return(p6);                               /* >= 80 */
}


/* The three MAX functions define a characters Effective maximum */
/* Which is NOT the same as the ch->points.max_xxxx !!!          */
int mana_limit(struct char_data *ch)
{
    int max;

    if (!IS_NPC(ch))
      max = (ch->points.max_mana);
    else
      max = 100;
    
    return(max);
}


int hit_limit(struct char_data *ch)
{
    int max;

    if (!IS_NPC(ch))
      max = (ch->points.max_hit) +
	(graf(age(ch).year, 2,4,17,14,8,4,3));
    else 
      max = (ch->points.max_hit);


/* Class/Level calculations */

/* Skill/Spell calculations */
    
  return (max);
}


int move_limit(struct char_data *ch)
{
    int max;

    if (!IS_NPC(ch))
	/* HERE SHOULD BE CON CALCULATIONS INSTEAD */
	max = (ch->points.max_move) + 
	  graf(age(ch).year, 50,70,160,120,100,40,20);
    else
	max = ch->points.max_move;

/* Class/Level calculations */

/* Skill/Spell calculations */

  return (max);
}




/* manapoint gain pr. game hour */
int mana_gain(struct char_data *ch)
{
    int gain;
    int divisor = 100000;

    if(IS_NPC(ch)) {
	/* Neat and fast */
	gain = GET_LEVEL(ch);
    } else {
	gain = graf(age(ch).year, 2,4,6,8,6,5,8);

	/* Class calculations */

	/* Skill/Spell calculations */

	/* Position calculations    */
	switch (GET_POS(ch)) {
	    case POSITION_SLEEPING: divisor = 2; break;
	    case POSITION_RESTING:  divisor = 4; break;
	    case POSITION_SITTING:  divisor = 8; break;
	}
	gain += (GET_INT(ch) + GET_WIS(ch)) / divisor;

	if (GET_CLASS(ch) == CLASS_MAGIC_USER || GET_CLASS(ch) == CLASS_CLERIC)
	    gain += gain;
    }

    if (IS_AFFECTED(ch,AFF_POISON))
	gain >>= 2;

    if((GET_COND(ch,FULL)==0)||(GET_COND(ch,THIRST)==0))
	gain >>= 2;
 
  return (gain);
}


int hit_gain(struct char_data *ch)
/* Hitpoint gain pr. game hour */
{
    int gain;
    int divisor = 100000;

    if(IS_NPC(ch)) {
	gain = (GET_LEVEL(ch) * 3 ) / 2;
	/* Neat and fast */
    } else {

	gain = graf(age(ch).year, 2,5,10,18,6,4,2);

	/* Class/Level calculations */

	/* Skill/Spell calculations */

	/* Position calculations    */
	switch (GET_POS(ch)) {
	    case POSITION_SLEEPING: divisor = 1; break;
	    case POSITION_RESTING:  divisor = 2; break;
	    case POSITION_SITTING:  divisor = 4; break;
	}
	gain += GET_CON(ch) / divisor;

	if (GET_CLASS(ch) == CLASS_MAGIC_USER || GET_CLASS(ch) == CLASS_CLERIC)
	    gain >>= 1;
  }

  if (IS_AFFECTED(ch,AFF_POISON))
    {
	gain >>= 2;
	damage(ch,ch,2,SPELL_POISON);
    }

    if((GET_COND(ch,FULL)==0)||(GET_COND(ch,THIRST)==0))
	gain >>= 2;

  return (gain);
}



int move_gain(struct char_data *ch)
/* move gain pr. game hour */
{
    int gain;
    int divisor = 100000;

    if(IS_NPC(ch)) {
	return(GET_LEVEL(ch));  
	/* Neat and fast */
    } else {
	gain = graf(age(ch).year, 6,9,11,10,7,5,3);

	/* Class/Level calculations */

	/* Skill/Spell calculations */


	/* Position calculations    */
	switch (GET_POS(ch)) {
	    case POSITION_SLEEPING: divisor = 1; break;
	    case POSITION_RESTING:  divisor = 3; break;
	    case POSITION_SITTING:  divisor = 6; break;
	}
	gain += (GET_CON(ch) + GET_DEX(ch)) / divisor;
    }

    if (IS_AFFECTED(ch,AFF_POISON))
	gain >>= 2;

    if((GET_COND(ch,FULL)==0)||(GET_COND(ch,THIRST)==0))
	gain >>= 2;

    return (gain);
}



/* Gain maximum in various points */
void advance_level(struct char_data *ch)
{
    int add_hp;
    int add_mana = 0;
    int add_moves = 0;
    int add_practices;
    int i;
    char buf[MAX_STRING_LENGTH];

    extern struct wis_app_type wis_app[];
    extern struct con_app_type con_app[];

    
    add_hp = con_app[GET_CON(ch)].hitp;

    switch(GET_CLASS(ch))
    {
    case CLASS_MAGIC_USER:
	add_hp      += number(6, 8);
	add_mana    += number(2, (GET_INT(ch) + GET_WIS(ch))/6);
	add_moves   += number(5, (GET_CON(ch) + 2 * GET_DEX(ch)) / 5);
	break;

    case CLASS_CLERIC:
	add_hp      += number(7, 10);
	add_mana    += number(2, (GET_INT(ch) + GET_WIS(ch))/6);
	add_moves   += number(5, (GET_CON(ch) + 2 * GET_DEX(ch)) / 5);
	break;

    case CLASS_THIEF:
	add_hp      += number(8, 13);
	add_moves   += number(5, (GET_CON(ch) + 2 * GET_DEX(ch)) / 5);
	break;

    case CLASS_WARRIOR:
	add_hp      += number(11, 15);
	add_moves   += number(5, (GET_CON(ch) + 2 * GET_DEX(ch)) / 5);
	break;
    }

    add_hp			 = MAX( 1, add_hp);
    add_mana			 = MAX( 0, add_mana);
    add_moves			 = MAX(10, add_moves);
    add_practices		 = wis_app[GET_WIS(ch)].bonus;
    ch->points.max_hit		+= add_hp;
    ch->points.max_mana		+= add_mana;
    ch->points.max_move		+= add_moves;
    ch->specials.practices	+= add_practices;

    sprintf( buf,
	"Your gain is: %d/%d hp, %d/%d m, %d/%d mv %d/%d prac.\n\r",
	add_hp,        GET_MAX_HIT(ch),
	add_mana,      GET_MAX_MANA(ch),
	add_moves,     GET_MAX_MOVE(ch),
	add_practices, ch->specials.practices
	);
    send_to_char( buf, ch );

    if (GET_LEVEL(ch) > 31)
	for (i = 0; i < 3; i++)
	    ch->specials.conditions[i] = -1;
}   


void set_title(struct char_data *ch)
{
    FREE( GET_TITLE(ch) );
    GET_TITLE(ch) = str_dup(
	title_table [GET_CLASS(ch)-1]
	    [(int) GET_LEVEL(ch)] [GET_SEX(ch)==SEX_FEMALE]
	);
    return;
}



void gain_exp( CHAR_DATA *ch, int gain )
{
    if ( !IS_NPC(ch) )
    {
	if ( GET_LEVEL(ch) >= 31 )
	    return;

	switch ( GET_CLASS(ch) )
	{
	case CLASS_THIEF:      gain += gain / 8; break;
	case CLASS_MAGIC_USER: gain += gain / 4; break;
	case CLASS_CLERIC:     gain += gain / 2; break;
	}
    }

    GET_EXP(ch) += gain;
    if ( GET_EXP(ch) < 0 )
	GET_EXP(ch) = 0;

    if ( IS_NPC(ch) )
	return;

    while ( GET_EXP(ch) >= exp_table[GET_LEVEL(ch)+1] )
    {
	send_to_char( "You raise a level!!  ", ch );
	GET_LEVEL(ch) += 1;
	advance_level( ch );
	set_title( ch );
    }

    return;
}


void gain_exp_regardless( CHAR_DATA *ch, int gain )
{
    GET_EXP(ch) += gain;
    if ( GET_EXP(ch) < 0 )
	GET_EXP(ch) = 0;

    if ( IS_NPC(ch) )
	return;

    while ( GET_EXP(ch) >= exp_table[GET_LEVEL(ch)+1] )
    {
	send_to_char( "You raise a level!!  ", ch );
	GET_LEVEL(ch) += 1;
	advance_level( ch );
	set_title( ch );
    }

    return;
}

void gain_condition(struct char_data *ch,int condition,int value)
{
    bool intoxicated;

    if(GET_COND(ch, condition)==-1) /* No change */
	return;

    intoxicated=(GET_COND(ch, DRUNK) > 0);

    GET_COND(ch, condition)  += value;

    GET_COND(ch,condition) = MAX(0,GET_COND(ch,condition));
    GET_COND(ch,condition) = MIN(24,GET_COND(ch,condition));

    if(GET_COND(ch,condition))
	return;

    switch(condition){
	case FULL :
	{
	    send_to_char("You are hungry.\n\r",ch);
	    return;
	}
	case THIRST :
	{
	    send_to_char("You are thirsty.\n\r",ch);
	    return;
	}
	case DRUNK :
	{
	    if(intoxicated)
		send_to_char("You are now sober.\n\r",ch);
	    return;
	}
	default : break;
    }

}



/* Update both PC's & NPC's and objects*/
void point_update( void )
{   
    void update_char_objects( struct char_data *ch ); /* handler.c */
    void extract_obj(struct obj_data *obj); /* handler.c */
    struct char_data *i, *next_dude;
    struct obj_data *j, *next_thing;

  /* characters */
    for (i = character_list; i; i = next_dude) {
	next_dude = i->next;
	if (GET_POS(i) > POSITION_STUNNED) {
	    GET_HIT(i)  = MIN(GET_HIT(i)  + hit_gain(i),  hit_limit(i));
	    GET_MANA(i) = MIN(GET_MANA(i) + mana_gain(i), mana_limit(i));
	    GET_MOVE(i) = MIN(GET_MOVE(i) + move_gain(i), move_limit(i));
	} else if (GET_POS(i) == POSITION_STUNNED) {
	    GET_HIT(i)  = MIN(GET_HIT(i)  + hit_gain(i),  hit_limit(i));
	    GET_MANA(i) = MIN(GET_MANA(i) + mana_gain(i), mana_limit(i));
	    GET_MOVE(i) = MIN(GET_MOVE(i) + move_gain(i), move_limit(i));
	    update_pos( i );
	} else if   (GET_POS(i) == POSITION_INCAP)
	    damage(i, i, 1, TYPE_SUFFERING);
	else if (!IS_NPC(i) && (GET_POS(i) == POSITION_MORTALLYW))
	    damage(i, i, 2, TYPE_SUFFERING);
	if (!IS_NPC(i))
	{
	    update_char_objects(i);
	    if (GET_LEVEL(i) < 35)
	    {
		if ( i->desc && i->desc->tick_wait > 0 )
		    i->desc->tick_wait--;
		check_idling(i);
	    }
	}
	gain_condition(i,FULL,-1);
	gain_condition(i,DRUNK,-1);
	gain_condition(i,THIRST,-1);
    } /* for */

  /* objects */
    for(j = object_list; j ; j = next_thing){
	next_thing = j->next; /* Next in object list */

    /* If this is a corpse */
	if ( (GET_ITEM_TYPE(j) == ITEM_CONTAINER) && (j->obj_flags.value[3]) )
	{
	    /* timer count down */
	    if (j->obj_flags.timer > 0) j->obj_flags.timer--;

	    if (!j->obj_flags.timer) {

		if (j->carried_by)
		    act("$p decay in your hands.",
		    FALSE, j->carried_by, j, 0, TO_CHAR);
		else if ((j->in_room != NOWHERE) && (world[j->in_room].people))
		{
		    act("A quivering hoard of maggots consume $p.",
		      TRUE, world[j->in_room].people, j, 0, TO_ROOM);
		    act("A quivering hoard of maggots consume $p.",
		      TRUE, world[j->in_room].people, j, 0, TO_CHAR);
		}

#if defined(KEEP_CORPSE_CONTENTS)
	for(jj = j->contains; jj; jj = next_thing2) {
		    next_thing2 = jj->next_content; /* Next in inventory */
		    obj_from_obj(jj);

		    if (j->in_obj)
			obj_to_obj(jj,j->in_obj);
		    else if (j->carried_by)
			obj_to_room(jj,j->carried_by->in_room);
		    else if (j->in_room != NOWHERE)
			obj_to_room(jj,j->in_room);
		    else
			assert(FALSE);
		}
#endif
		extract_obj(j);
	    }
	}
    }
}
