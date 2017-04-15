/***************************************************************************
 *  file: mob_act.c , Mobile action module.                Part of DIKUMUD *
 *  Usage: Procedures generating 'intelligent' behavior in the mobiles.    *
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

#include <stdio.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "utils.h"
#include "db.h"

extern struct char_data *character_list;
extern struct index_data *mob_index;
extern struct room_data *world;
extern struct str_app_type str_app[];

void mobile_activity(void)
{
    register struct char_data *ch;
    struct char_data *tmp_ch;
    struct obj_data *obj, *best_obj;
    int door, max;

    void do_move(struct char_data *ch, char *argument, int cmd);
    void do_get(struct char_data *ch, char *argument, int cmd);

    /* Examine all mobs. */
    for ( ch = character_list; ch; ch = ch->next )
    {
	if ( !IS_MOB(ch) )
	    continue;

	/* Examine call for special procedure */
	if ( IS_SET(ch->specials.act, ACT_SPEC) )
	{
	    if (!mob_index[ch->nr].func)
	    {
		sprintf( log_buf, "Mobile_activity: MOB %d no func",
			mob_index[ch->nr].virtual );
		log( log_buf );
		REMOVE_BIT( ch->specials.act, ACT_SPEC );
	    }
	    else
	    {
		if ( (*mob_index[ch->nr].func) (ch, 0, "") )
		    continue;
	    }
	}

	/* That's all for busy monster */
	if ( !AWAKE(ch) || ch->specials.fighting )
	    continue;

	/* Scavenge */
	if ( IS_SET(ch->specials.act, ACT_SCAVENGER)
	&& world[ch->in_room].contents && number(0,10) == 0 )
	{
	    max         = 1;
	    best_obj    = 0;
	    for ( obj = world[ch->in_room].contents; obj;
		obj = obj->next_content )
	    {
		if ( CAN_GET_OBJ(ch, obj) && obj->obj_flags.cost > max )
		{
		    best_obj    = obj;
		    max         = obj->obj_flags.cost;
		}
	    }

	    if ( best_obj )
	    {
		obj_from_room( best_obj );
		obj_to_char( best_obj, ch );
		act( "$n gets $p.", FALSE, ch, best_obj, 0, TO_ROOM );
	    }
	}

	/* Wander */
	if ( !IS_SET(ch->specials.act, ACT_SENTINEL)
	&& GET_POS(ch) == POSITION_STANDING
	&& (door = number(0,45)) <= 5
	&& CAN_GO(ch, door)
	&& !IS_SET(world[EXIT(ch,door)->to_room].room_flags, NO_MOB|DEATH) )
	{
	    if (ch->specials.last_direction == door)
	    {
		ch->specials.last_direction = -1;
	    }
	    else if ( !IS_SET(ch->specials.act, ACT_STAY_ZONE)
	    || world[EXIT(ch, door)->to_room].zone == world[ch->in_room].zone )
	    {
		ch->specials.last_direction = door;
		do_move( ch, "", ++door );
	    }
	}

	/* Aggress */
	if ( IS_SET(ch->specials.act, ACT_AGGRESSIVE) )
	{
	    for ( tmp_ch = world[ch->in_room].people; tmp_ch;
	    tmp_ch = tmp_ch->next_in_room )
	    {
		if ( IS_NPC(tmp_ch) || !CAN_SEE(ch, tmp_ch) )
		    continue;
		if ( IS_SET(ch->specials.act, ACT_WIMPY) && AWAKE(tmp_ch) )
		    continue;

		if ( ( IS_SET(ch->specials.act, ACT_AGGR_EVIL)
		    && IS_EVIL(tmp_ch) ) 
		||   ( IS_SET(ch->specials.act, ACT_AGGR_NEUT)
		    && IS_NEUTRAL(tmp_ch) ) 
		||   ( IS_SET(ch->specials.act, ACT_AGGR_GOOD)
		    && IS_GOOD(tmp_ch) )
		||   ( ch->specials.act & ACT_AGGR_ALL ) == 0 )
		{
		  hit(ch, tmp_ch, 0);
		  break;
		}
	    }
	}

    }
}
