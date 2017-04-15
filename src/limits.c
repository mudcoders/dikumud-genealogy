/* ************************************************************************
*   File: limits.c                                      Part of CircleMUD *
*  Usage: limits & gain funcs for HMV, exp, hunger/thirst, idle time      *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include "structs.h"
#include "limits.h"
#include "utils.h"
#include "spells.h"
#include "comm.h"
#include "db.h"
#include "handler.h"

#define READ_TITLE(ch) (GET_SEX(ch) == SEX_MALE ?   \
	titles[(int)GET_CLASS(ch)-1][(int)GET_LEVEL(ch)].title_m :  \
	titles[(int)GET_CLASS(ch)-1][(int)GET_LEVEL(ch)].title_f)


extern struct char_data *character_list;
extern struct obj_data *object_list;
extern struct title_type titles[4][35];
extern struct room_data *world;


/* When age < 15 return the value p0 */
/* When age in 15..29 calculate the line between p1 & p2 */
/* When age in 30..44 calculate the line between p2 & p3 */
/* When age in 45..59 calculate the line between p3 & p4 */
/* When age in 60..79 calculate the line between p4 & p5 */
/* When age >= 80 return the value p6 */
int	graf(int age, int p0, int p1, int p2, int p3, int p4, int p5, int p6)
{

   if (age < 15)
      return(p0);						/* < 15   */
   else if (age <= 29)
      return (int) (p1 + (((age - 15) * (p2 - p1)) / 15));	/* 15..29 */
   else if (age <= 44)
      return (int) (p2 + (((age - 30) * (p3 - p2)) / 15));	/* 30..44 */
   else if (age <= 59)
      return (int) (p3 + (((age - 45) * (p4 - p3)) / 15));	/* 45..59 */
   else if (age <= 79)
      return (int) (p4 + (((age - 60) * (p5 - p4)) / 20));	/* 60..79 */
   else
      return(p6);						/* >= 80 */
}


/* The hit_limit, mana_limit, and move_limit functions are gone.  They
   added an unnecessary level of complexity to the internal structure,
   weren't particularly useful, and led to some annoying bugs.  From the
   players' point of view, the only difference the removal of these
   functions will make is that a character's age will now only affect
   the HMV gain per tick, and _not_ the HMV maximums.
*/

/* manapoint gain pr. game hour */
int	mana_gain(struct char_data *ch)
{
   int	gain;

   if (IS_NPC(ch)) {
      /* Neat and fast */
      gain = GET_LEVEL(ch);
   } else {
      gain = graf(age(ch).year, 4, 8, 12, 16, 12, 10, 8);

      /* Class calculations */

      /* Skill/Spell calculations */

      /* Position calculations    */
      switch (GET_POS(ch)) {
      case POSITION_SLEEPING:
	 gain <<= 1;
	 break;
      case POSITION_RESTING:
	 gain += (gain >> 1);  /* Divide by 2 */
	 break;
      case POSITION_SITTING:
	 gain += (gain >> 2); /* Divide by 4 */
	 break;
      }

      if ((GET_CLASS(ch) == CLASS_MAGIC_USER) || (GET_CLASS(ch) == CLASS_CLERIC))
	 gain <<= 1;
   }

   if (IS_AFFECTED(ch, AFF_POISON))
      gain >>= 2;

   if ((GET_COND(ch, FULL) == 0) || (GET_COND(ch, THIRST) == 0))
      gain >>= 2;

   return (gain);
}


int	hit_gain(struct char_data *ch)
/* Hitpoint gain pr. game hour */
{
   int	gain;

   if (IS_NPC(ch)) {
      gain = GET_LEVEL(ch);
      /* Neat and fast */
   } else {

      gain = graf(age(ch).year, 8, 12, 20, 32, 16, 10, 4);

      /* Class/Level calculations */

      /* Skill/Spell calculations */

      /* Position calculations    */

      switch (GET_POS(ch)) {
      case POSITION_SLEEPING:
	 gain += (gain >> 1); /* Divide by 2 */
	 break;
      case POSITION_RESTING:
	 gain += (gain >> 2);  /* Divide by 4 */
	 break;
      case POSITION_SITTING:
	 gain += (gain >> 3); /* Divide by 8 */
	 break;
      }

      if ((GET_CLASS(ch) == CLASS_MAGIC_USER) || (GET_CLASS(ch) == CLASS_CLERIC))
	 gain >>= 1;
   }

   if (IS_AFFECTED(ch, AFF_POISON))
      gain >>= 2;

   if ((GET_COND(ch, FULL) == 0) || (GET_COND(ch, THIRST) == 0))
      gain >>= 2;

   return (gain);
}



int	move_gain(struct char_data *ch)
/* move gain pr. game hour */
{
   int	gain;

   if (IS_NPC(ch)) {
      return(GET_LEVEL(ch));
      /* Neat and fast */
   } else {
      gain = graf(age(ch).year, 16, 20, 24, 20, 16, 12, 10);

      /* Class/Level calculations */

      /* Skill/Spell calculations */


      /* Position calculations    */
      switch (GET_POS(ch)) {
      case POSITION_SLEEPING:
	 gain += (gain >> 1); /* Divide by 2 */
	 break;
      case POSITION_RESTING:
	 gain += (gain >> 2);  /* Divide by 4 */
	 break;
      case POSITION_SITTING:
	 gain += (gain >> 3); /* Divide by 8 */
	 break;
      }
   }

   if (IS_AFFECTED(ch, AFF_POISON))
      gain >>= 2;

   if ((GET_COND(ch, FULL) == 0) || (GET_COND(ch, THIRST) == 0))
      gain >>= 2;

   return (gain);
}


/* Gain maximum in various points */
void	advance_level(struct char_data *ch)
{
   int	add_hp, i;
   int	add_mana;

   extern struct wis_app_type wis_app[];
   extern struct con_app_type con_app[];

   add_hp = con_app[GET_CON(ch)].hitp;

   switch (GET_CLASS(ch)) {

   case CLASS_MAGIC_USER :
      add_hp += number(3, 8);
      add_mana = number(GET_LEVEL(ch), (int)(1.5 * GET_LEVEL(ch)));
      add_mana = MIN(add_mana, 10);
      break;

   case CLASS_CLERIC :
      add_hp += number(5, 10);
      add_mana = number(GET_LEVEL(ch), (int)(1.5 * GET_LEVEL(ch)));
      add_mana = MIN(add_mana, 10);
      break;

   case CLASS_THIEF :
      add_hp += number(7, 13);
      add_mana = 0;
      break;

   case CLASS_WARRIOR :
      add_hp += number(10, 15);
      add_mana = 0;
      break;
   }

   ch->points.max_hit += MAX(1, add_hp);
   if ((GET_LEVEL(ch) != 1) && (ch->points.max_mana < 160)) {
      ch->points.max_mana = ch->points.max_mana + (sh_int)add_mana;
   } /* if */

   if (GET_CLASS(ch) == CLASS_MAGIC_USER || GET_CLASS(ch) == CLASS_CLERIC)
      SPELLS_TO_LEARN(ch) += MAX(2, wis_app[GET_WIS(ch)].bonus);
   else
      SPELLS_TO_LEARN(ch) += MIN(2, MAX(1, wis_app[GET_WIS(ch)].bonus));

   if (GET_LEVEL(ch) >= LEVEL_IMMORT)
      for (i = 0; i < 3; i++)
	 GET_COND(ch, i) = (char) -1;

   save_char(ch, NOWHERE);

   sprintf(buf, "%s advanced to level %d", GET_NAME(ch), GET_LEVEL(ch));
   mudlog(buf, BRF, MAX(LEVEL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
}


void	set_title(struct char_data *ch)
{
   if (GET_TITLE(ch))
      RECREATE(GET_TITLE(ch), char, strlen(READ_TITLE(ch)) + 1);
   else
      CREATE(GET_TITLE(ch), char, strlen(READ_TITLE(ch)));

   strcpy(GET_TITLE(ch), READ_TITLE(ch));
}


void check_autowiz(struct char_data *ch)
{
   char	buf[100];
   extern int	use_autowiz;
   extern int	min_wizlist_lev;

   if (use_autowiz && GET_LEVEL(ch) >= LEVEL_IMMORT) {
      sprintf(buf, "nice ../bin/autowiz %d %s %d %s %d &", min_wizlist_lev,
         WIZLIST_FILE, LEVEL_IMMORT, IMMLIST_FILE, getpid());
      mudlog("Initiating autowiz.", CMP, LEVEL_IMMORT, FALSE);
      system(buf);
   }
}



void	gain_exp(struct char_data *ch, int gain)
{
   int	i;
   bool is_altered = FALSE;

   if (IS_NPC(ch) || ((GET_LEVEL(ch) < LEVEL_IMMORT) && (GET_LEVEL(ch) > 0))) {

      if (gain > 0) {
	 gain = MIN(100000, gain);
	 GET_EXP(ch) += gain;
	 if (!IS_NPC(ch)) {
	    for (i = 0; titles[GET_CLASS(ch)-1][i].exp <= GET_EXP(ch); i++) {
	       if (i > GET_LEVEL(ch)) {
		  send_to_char("You rise a level!\n\r", ch);
		  GET_LEVEL(ch) = i;
		  advance_level(ch);
		  is_altered = TRUE;
	       }
	    }
	 }
      }

      if (gain < 0) {
	 gain = MAX(-500000, gain);  /* Never loose more than 1/2 mil */
	 GET_EXP(ch) += gain;
	 if (GET_EXP(ch) < 0)
	    GET_EXP(ch) = 0;
      }

      if (is_altered) {
	 set_title(ch);
	 check_autowiz(ch);
      }
   }
}


void	gain_exp_regardless(struct char_data *ch, int gain)
{
   int	i;
   bool is_altered = FALSE;

   if (!IS_NPC(ch)) {
      if (gain > 0) {
	 GET_EXP(ch) += gain;
	 for (i = 0; (i <= LEVEL_IMPL) && (titles[GET_CLASS(ch)-1][i].exp <= GET_EXP(ch)); i++) {
	    if (i > GET_LEVEL(ch)) {
	       send_to_char("You rise a level!\n\r", ch);
	       GET_LEVEL(ch) = i;
	       advance_level(ch);
	       is_altered = TRUE;
	    }
	 }
      }
      if (gain < 0)
	 GET_EXP(ch) += gain;
      if (GET_EXP(ch) < 0)
	 GET_EXP(ch) = 0;
   }
   if (is_altered) {
      set_title(ch);
      check_autowiz(ch);
   }
}


void	gain_condition(struct char_data *ch, int condition, int value)
{
   bool intoxicated;

   if (GET_COND(ch, condition) == -1) /* No change */
      return;

   intoxicated = (GET_COND(ch, DRUNK) > 0);

   GET_COND(ch, condition)  += value;

   GET_COND(ch, condition) = MAX(0, GET_COND(ch, condition));
   GET_COND(ch, condition) = MIN(24, GET_COND(ch, condition));

   if (GET_COND(ch, condition) || PLR_FLAGGED(ch, PLR_WRITING))
      return;

   switch (condition) {
   case FULL :
      send_to_char("You are hungry.\n\r", ch);
      return;
   case THIRST :
      send_to_char("You are thirsty.\n\r", ch);
      return;
   case DRUNK :
      if (intoxicated)
	 send_to_char("You are now sober.\n\r", ch);
      return;
   default :
      break;
   }

}


void	check_idling(struct char_data *ch)
{
   if (++(ch->specials.timer) > 8)
      if (ch->specials.was_in_room == NOWHERE && ch->in_room != NOWHERE) {
	 ch->specials.was_in_room = ch->in_room;
	 if (ch->specials.fighting) {
	    stop_fighting(ch->specials.fighting);
	    stop_fighting(ch);
	 }
	 act("$n disappears into the void.", TRUE, ch, 0, 0, TO_ROOM);
	 send_to_char("You have been idle, and are pulled into a void.\n\r", ch);
	 save_char(ch, NOWHERE);
	 Crash_crashsave(ch);
	 char_from_room(ch);
	 char_to_room(ch, 1);
      } 
      else if (ch->specials.timer > 48) {
	 if (ch->in_room != NOWHERE) 
	    char_from_room(ch);
	 char_to_room(ch, 3);
	 if (ch->desc)
	    close_socket(ch->desc);
	 ch->desc = 0;
	 Crash_idlesave(ch);
	 sprintf(buf, "%s force-rented and extracted (idle).", GET_NAME(ch));
	 mudlog(buf, CMP, LEVEL_GOD, TRUE);
	 extract_char(ch);
      }
}



/* Update both PC's & NPC's and objects*/
void	point_update( void )
{
   void	update_char_objects( struct char_data *ch ); /* handler.c */
   void	extract_obj(struct obj_data *obj); /* handler.c */
   struct char_data *i, *next_dude;
   struct obj_data *j, *next_thing, *jj, *next_thing2;

   /* characters */
   for (i = character_list; i; i = next_dude) {
      next_dude = i->next;
      if (GET_POS(i) >= POSITION_STUNNED) {
	 GET_HIT(i)  = MIN(GET_HIT(i)  + hit_gain(i),  GET_MAX_HIT(i));
	 GET_MANA(i) = MIN(GET_MANA(i) + mana_gain(i), GET_MAX_MANA(i));
	 GET_MOVE(i) = MIN(GET_MOVE(i) + move_gain(i), GET_MAX_MOVE(i));
	 if (IS_AFFECTED(i, AFF_POISON))
	    damage(i, i, 2, SPELL_POISON);
	 if (GET_POS(i) == POSITION_STUNNED)
	    update_pos(i);
      } else if (GET_POS(i) == POSITION_INCAP)
	 damage(i, i, 1, TYPE_SUFFERING);
      else if (!IS_NPC(i) && (GET_POS(i) == POSITION_MORTALLYW))
	 damage(i, i, 2, TYPE_SUFFERING);
      if (!IS_NPC(i)) {
	 update_char_objects(i);
	 if (GET_LEVEL(i) < LEVEL_GOD)
	    check_idling(i);
      }
      gain_condition(i, FULL, -1);
      gain_condition(i, DRUNK, -1);
      gain_condition(i, THIRST, -1);
   } /* for */

   /* objects */
   for (j = object_list; j ; j = next_thing) {
      next_thing = j->next; /* Next in object list */

      /* If this is a corpse */
      if ( (GET_ITEM_TYPE(j) == ITEM_CONTAINER) && (j->obj_flags.value[3]) ) {
	 /* timer count down */
	 if (j->obj_flags.timer > 0)
	    j->obj_flags.timer--;

	 if (!j->obj_flags.timer) {

	    if (j->carried_by)
	       act("$p decays in your hands.", FALSE, j->carried_by, j, 0, TO_CHAR);
	    else if ((j->in_room != NOWHERE) && (world[j->in_room].people)) {
	       act("A quivering hoard of maggots consumes $p.",
	           TRUE, world[j->in_room].people, j, 0, TO_ROOM);
	       act("A quivering hoard of maggots consumes $p.",
	           TRUE, world[j->in_room].people, j, 0, TO_CHAR);
	    }

	    for (jj = j->contains; jj; jj = next_thing2) {
	       next_thing2 = jj->next_content; /* Next in inventory */
	       obj_from_obj(jj);

	       if (j->in_obj)
		  obj_to_obj(jj, j->in_obj);
	       else if (j->carried_by)
		  obj_to_room(jj, j->carried_by->in_room);
	       else if (j->in_room != NOWHERE)
		  obj_to_room(jj, j->in_room);
	       else
		  assert(FALSE);
	    }
	    extract_obj(j);
	 }
      }
   }
}


