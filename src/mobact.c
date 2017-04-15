/* ************************************************************************
*   File: mobact.c                                      Part of CircleMUD *
*  Usage: Functions for generating intelligent (?) behavior in mobiles    *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "utils.h"
#include "db.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"

/* external structs */
extern struct char_data *character_list;
extern struct index_data *mob_index;
extern struct room_data *world;
extern struct str_app_type str_app[];


void	mobile_activity(void)
{
   register struct char_data *ch;
   struct char_data *tmp_ch, *vict;
   struct obj_data *obj, *best_obj;
   int	door, found, max;
   memory_rec * names;

   extern int	no_specials;

   ACMD(do_move);
   ACMD(do_get);

   for (ch = character_list; ch; ch = ch->next)
      if (IS_MOB(ch)) {
	 /* Examine call for special procedure */
	 if (IS_SET(ch->specials2.act, MOB_SPEC) && !no_specials) {
	    if (!mob_index[ch->nr].func) {
	       sprintf(buf, "%s (#%d): Attempting to call non-existing mob func",
	           GET_NAME(ch), (int)mob_index[ch->nr].virtual);
	       log(buf);
	       REMOVE_BIT(ch->specials2.act, MOB_SPEC);
	    } else {
	       if ((*mob_index[ch->nr].func)	(ch, 0, ""))
		  continue; /* go to next char */
	    }
	 }

	 if (AWAKE(ch) && !(ch->specials.fighting)) {
	    if (IS_SET(ch->specials2.act, MOB_SCAVENGER)) { /* if scavenger */
	       if (world[ch->in_room].contents && !number(0, 10)) {
		  for (max = 1, best_obj = 0, obj = world[ch->in_room].contents; 
		      obj; obj = obj->next_content) {
		     if (CAN_GET_OBJ(ch, obj)) {
			if (obj->obj_flags.cost > max) {
			   best_obj = obj;
			   max = obj->obj_flags.cost;
			}
		     }
		  } /* for */

		  if (best_obj) {
		     obj_from_room(best_obj);
		     obj_to_char(best_obj, ch);
		     act("$n gets $p.", FALSE, ch, best_obj, 0, TO_ROOM);
		  }
	       }
	    }/* Scavenger */

	    if (!IS_SET(ch->specials2.act, MOB_SENTINEL) && 
	        (GET_POS(ch) == POSITION_STANDING) && 
	        ((door = number(0, 45)) < NUM_OF_DIRS) && CAN_GO(ch, door) && 
	        !IS_SET(world[EXIT(ch, door)->to_room].room_flags, NO_MOB) && 
	        !IS_SET(world[EXIT(ch, door)->to_room].room_flags, DEATH)) {
	       if (ch->specials.last_direction == door) {
		  ch->specials.last_direction = -1;
	       } else {
		  if (!IS_SET(ch->specials2.act, MOB_STAY_ZONE)) {
		     ch->specials.last_direction = door;
		     do_move(ch, "", ++door, 0);
		  } else {
		     if (world[EXIT(ch, door)->to_room].zone == world[ch->in_room].zone) {
			ch->specials.last_direction = door;
			do_move(ch, "", ++door, 0);
		     }
		  }
	       }
	    } /* if can go */


	    if (IS_SET(ch->specials2.act, MOB_AGGRESSIVE)) {
	       found = FALSE;
	       for (tmp_ch = world[ch->in_room].people; tmp_ch && !found; 
	           tmp_ch = tmp_ch->next_in_room) {
		  if (!IS_NPC(tmp_ch) && CAN_SEE(ch, tmp_ch) && !PRF_FLAGGED(tmp_ch, PRF_NOHASSLE)) {
		     if (!IS_SET(ch->specials2.act, MOB_WIMPY) || !AWAKE(tmp_ch)) {
			if ((IS_SET(ch->specials2.act, MOB_AGGRESSIVE_EVIL) && 
			    IS_EVIL(tmp_ch)) || 
			    (IS_SET(ch->specials2.act, MOB_AGGRESSIVE_GOOD) && 
			    IS_GOOD(tmp_ch)) || 
			    (IS_SET(ch->specials2.act, MOB_AGGRESSIVE_NEUTRAL) && 
			    IS_NEUTRAL(tmp_ch)) || 
			    (!IS_SET(ch->specials2.act, MOB_AGGRESSIVE_EVIL) && 
			    !IS_SET(ch->specials2.act, MOB_AGGRESSIVE_NEUTRAL) && 
			    !IS_SET(ch->specials2.act, MOB_AGGRESSIVE_GOOD) ) ) {
				hit(ch, tmp_ch, 0);
				found = TRUE;
			}
		     }
		  }
	       }
	    } /* if aggressive */

	    if (IS_SET(ch->specials2.act, MOB_MEMORY) && ch->specials.memory) {
	       for (vict = 0, tmp_ch = world[ch->in_room].people; 
	           tmp_ch && !vict; tmp_ch = tmp_ch->next_in_room)
		  if (!IS_NPC(tmp_ch))
		     for (names = ch->specials.memory; names && !vict; names = names->next)
			if (names->id == GET_IDNUM(tmp_ch))
			   vict = tmp_ch;
	       if (vict) {
		  act("'Hey!  You're the fiend that attacked me!!!', exclaims $n.",
		      FALSE, ch, 0, 0, TO_ROOM);
		  hit(ch, vict, 0);
	       }
	    } /* mob memory */
	 }
      } /* If IS_MOB(ch)  */
}



/* Mob Memory Routines */

/* make ch remember victim */
void	remember (struct char_data *ch, struct char_data *victim)
{
   memory_rec * tmp;
   bool present = FALSE;

   if (!IS_NPC(ch) || IS_NPC(victim)) 
      return;

   for (tmp = ch->specials.memory; tmp && !present; tmp = tmp->next)
      if (tmp->id == GET_IDNUM(victim))
	 present = TRUE;

   if (!present) {
      CREATE(tmp, memory_rec, 1);
      tmp->next = ch->specials.memory;
      tmp->id = GET_IDNUM(victim);
      ch->specials.memory = tmp;
   }
}


/* make ch forget victim */
void	forget (struct char_data *ch, struct char_data *victim)
{
   memory_rec *curr, *prev;

   if (!(curr = ch->specials.memory))
      return;

   while (curr && curr->id != GET_IDNUM(victim)) {
      prev = curr;
      curr = curr->next;
   }

   if (!curr) 
      return; /* person wasn't there at all. */

   if (curr == ch->specials.memory)
      ch->specials.memory = curr->next;
   else
      prev->next = curr->next;

   free(curr);
}


/* erase ch's memory */
void	clearMemory(struct char_data *ch)
{
   memory_rec *curr, *next;

   curr = ch->specials.memory;

   while (curr) {
      next = curr->next;
      free(curr);
      curr = next;
   }

   ch->specials.memory = NULL;
}


