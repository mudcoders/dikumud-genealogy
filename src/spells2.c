/* ************************************************************************
*   File: spells2.c                                     Part of CircleMUD *
*  Usage: step 1 implementation of non-off spells (step 2 in magic.c)     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "db.h"
#include "interpreter.h"
#include "spells.h"
#include "handler.h"


/* Global data */
extern struct room_data *world;
extern struct char_data *character_list;
extern struct spell_info_type spell_info[MAX_SPL_LIST];
extern struct obj_data *object_list;


/* extern functions */
void	clone_char(struct char_data *ch);
void	say_spell( struct char_data *ch, int si );
bool saves_spell(struct char_data *ch, sh_int spell);
void	add_follower(struct char_data *ch, struct char_data *victim);


ACAST(cast_armor)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      if ( affected_by_spell(victim, SPELL_ARMOR) ) {
	 send_to_char("Nothing seems to happen.\n\r", ch);
	 return;
      }
      if (ch != victim)
	 act("$N is protected by your deity.", FALSE, ch, 0, victim, TO_CHAR);

      spell_armor(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      if ( affected_by_spell(ch, SPELL_ARMOR) )
	 return;
      spell_armor(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj)
	 return;
      if (!victim)
	 victim = ch;
      if ( affected_by_spell(victim, SPELL_ARMOR) )
	 return;
      spell_armor(level, ch, ch, 0);
      break;
   case SPELL_TYPE_WAND:
      if (tar_obj)
	 return;
      if ( affected_by_spell(victim, SPELL_ARMOR) )
	 return;
      spell_armor(level, ch, ch, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in armor!");
      break;
   }
}



ACAST(cast_teleport)
{
   switch (type) {
   case SPELL_TYPE_SCROLL:
   case SPELL_TYPE_POTION:
   case SPELL_TYPE_SPELL:
      if (!victim)
	 victim = ch;
      spell_teleport(level, ch, victim, 0);
      break;

   case SPELL_TYPE_WAND:
      if (!victim)
	 return;
      spell_teleport(level, ch, victim, 0);
      break;

   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_teleport(level, ch, victim, 0);
      break;

   default :
      log("SYSERR: Serious screw-up in teleport!");
      break;
   }
}


ACAST(cast_bless)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      if (tar_obj) {        /* It's an object */
	 if ( IS_SET(tar_obj->obj_flags.extra_flags, ITEM_BLESS) ) {
	    send_to_char("Nothing seems to happen.\n\r", ch);
	    return;
	 }
	 spell_bless(level, ch, 0, tar_obj);

      } else {              /* Then it is a PC | NPC */

	 if ( affected_by_spell(victim, SPELL_BLESS) || 
	     (GET_POS(victim) == POSITION_FIGHTING)) {
	    send_to_char("Nothing seems to happen.\n\r", ch);
	    return;
	 }
	 spell_bless(level, ch, victim, 0);
      }
      break;
   case SPELL_TYPE_POTION:
      if ( affected_by_spell(ch, SPELL_BLESS) || 
          (GET_POS(ch) == POSITION_FIGHTING))
	 return;
      spell_bless(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj) {        /* It's an object */
	 if ( IS_SET(tar_obj->obj_flags.extra_flags, ITEM_BLESS) )
	    return;
	 spell_bless(level, ch, 0, tar_obj);

      } else {              /* Then it is a PC | NPC */

	 if (!victim)
	    victim = ch;

	 if ( affected_by_spell(victim, SPELL_BLESS) || 
	     (GET_POS(victim) == POSITION_FIGHTING))
	    return;
	 spell_bless(level, ch, victim, 0);
      }
      break;
   case SPELL_TYPE_WAND:
      if (tar_obj) {        /* It's an object */
	 if ( IS_SET(tar_obj->obj_flags.extra_flags, ITEM_BLESS) )
	    return;
	 spell_bless(level, ch, 0, tar_obj);

      } else {              /* Then it is a PC | NPC */

	 if ( affected_by_spell(victim, SPELL_BLESS) || 
	     (GET_POS(victim) == POSITION_FIGHTING))
	    return;
	 spell_bless(level, ch, victim, 0);
      }
      break;
   default :
      log("SYSERR: Serious screw-up in bless!");
      break;
   }
}



ACAST(cast_blindness)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      if ( IS_AFFECTED(victim, AFF_BLIND) ) {
	 send_to_char("Nothing seems to happen.\n\r", ch);
	 return;
      }
      spell_blindness(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      if ( IS_AFFECTED(ch, AFF_BLIND) )
	 return;
      spell_blindness(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj)
	 return;
      if (!victim)
	 victim = ch;
      if ( IS_AFFECTED(ch, AFF_BLIND) )
	 return;
      spell_blindness(level, ch, ch, 0);
      break;
   case SPELL_TYPE_WAND:
      if (victim) {
	 if ( IS_AFFECTED(victim, AFF_BLIND) )
	    return;
	 spell_blindness(level, ch, victim, 0);
      }
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    if (!(IS_AFFECTED(victim, AFF_BLIND)))
	       spell_blindness(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in blindness!");
      break;
   }
}


ACAST(cast_clone)
{
   send_to_char("Not *YET* implemented.", ch);
   return;

   /* clone both char and obj !!*/

   /*
   struct char_data *vict;

  switch (type) {
    case SPELL_TYPE_SPELL:
			if (victim) {	
				sprintf(buf, "You create a duplicate of %s.\n\r", GET_NAME(victim));
				send_to_char(buf, ch);
				sprintf(buf, "%%s creates a duplicate of %s,\n\r", GET_NAME(victim));
				perform(buf, ch, FALSE);

				spell_clone(level,ch,victim,0);
			} else {
				sprintf(buf, "You create a duplicate of %s %s.\n\r",SANA(tar_obj),tar_obj->short_description);
				send_to_char(buf, ch);
				sprintf(buf, "%%s creates a duplicate of %s %s,\n\r",SANA(tar_obj),tar_obj->short_description);
				perform(buf, ch, FALSE);

				spell_clone(level,ch,0,tar_obj);
			};
			break;


    default : 
         log("SYSERR: Serious screw-up in clone!");
         break;
	}
*/
   /* MISSING REST OF SWITCH -- POTION, SCROLL, WAND */
}


ACAST(cast_control_weather)
{
   char	buffer[MAX_STRING_LENGTH];

   switch (type) {
   case SPELL_TYPE_SPELL:

      one_argument(arg, buffer);

      if (str_cmp("better", buffer) && str_cmp("worse", buffer)) {
	 send_to_char("Do you want it to get better or worse?\n\r", ch);
	 return;
      }

      if (!str_cmp("better", buffer))
	 weather_info.change += (dice(((level) / 3), 4));
      else
	 weather_info.change -= (dice(((level) / 3), 4));
      break;
   default :
      log("SYSERR: Serious screw-up in control weather!");
      break;
   }
}



ACAST(cast_create_food)
{

   switch (type) {
   case SPELL_TYPE_SPELL:
      act("$n magically creates a mushroom.", FALSE, ch, 0, 0, TO_ROOM);
      spell_create_food(level, ch, 0, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj)
	 return;
      if (victim)
	 return;
      spell_create_food(level, ch, 0, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in create food!");
      break;
   }
}



ACAST(cast_create_water)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      if ((tar_obj->obj_flags.type_flag != ITEM_DRINKCON) && 
          (tar_obj->obj_flags.type_flag != ITEM_FOUNTAIN)) {
	 send_to_char("It is unable to hold water.\n\r", ch);
	 return;
      }
      spell_create_water(level, ch, 0, tar_obj);
      break;
   default :
      log("SYSERR: Serious screw-up in create water!");
      break;
   }
}



ACAST(cast_cure_blind)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_cure_blind(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_cure_blind(level, ch, ch, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_cure_blind(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in cure blind!");
      break;
   }
}



ACAST(cast_cure_critic)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_cure_critic(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_cure_critic(level, ch, ch, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_cure_critic(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in cure critic!");
      break;

   }
}



ACAST(cast_cure_light)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_cure_light(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_cure_light(level, ch, ch, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_cure_light(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in cure light!");
      break;
   }
}


ACAST(cast_curse)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      if (tar_obj)   /* It is an object */

	 spell_curse(level, ch, 0, tar_obj);
      else {              /* Then it is a PC | NPC */
	 spell_curse(level, ch, victim, 0);
      }
      break;
   case SPELL_TYPE_POTION:
      spell_curse(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj)   /* It is an object */

	 spell_curse(level, ch, 0, tar_obj);
      else {              /* Then it is a PC | NPC */
	 if (!victim)
	    victim = ch;
	 spell_curse(level, ch, victim, 0);
      }
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_curse(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in curse!");
      break;
   }
}


ACAST(cast_detect_evil)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      if ( affected_by_spell(victim, SPELL_DETECT_EVIL) ) {
	 send_to_char("Nothing seems to happen.\n\r", victim);
	 return;
      }
      spell_detect_evil(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      if ( affected_by_spell(ch, SPELL_DETECT_EVIL) )
	 return;
      spell_detect_evil(level, ch, ch, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    if (!(IS_AFFECTED(victim, SPELL_DETECT_EVIL)))
	       spell_detect_evil(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in detect evil!");
      break;
   }
}



ACAST(cast_detect_invisibility)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      if ( affected_by_spell(victim, SPELL_DETECT_INVISIBLE) ) {
	 send_to_char("Nothing seems to happen.\n\r", victim);
	 return;
      }
      spell_detect_invisibility(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      if ( affected_by_spell(ch, SPELL_DETECT_INVISIBLE) )
	 return;
      spell_detect_invisibility(level, ch, ch, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    if (!(IS_AFFECTED(victim, SPELL_DETECT_INVISIBLE)))
	       spell_detect_invisibility(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in detect invisibility!");
      break;
   }
}



ACAST(cast_detect_magic)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      if ( affected_by_spell(victim, SPELL_DETECT_MAGIC) ) {
	 send_to_char("Nothing seems to happen.\n\r", victim);
	 return;
      }
      spell_detect_magic(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      if ( affected_by_spell(ch, SPELL_DETECT_MAGIC) )
	 return;
      spell_detect_magic(level, ch, ch, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    if (!(IS_AFFECTED(victim, SPELL_DETECT_MAGIC)))
	       spell_detect_magic(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in detect magic!");
      break;
   }
}



ACAST(cast_detect_poison)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_detect_poison(level, ch, victim, tar_obj);
      break;
   case SPELL_TYPE_POTION:
      spell_detect_poison(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj) {
	 spell_detect_poison(level, ch, 0, tar_obj);
	 return;
      }
      if (!victim)
	 victim = ch;
      spell_detect_poison(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in detect poison!");
      break;
   }
}



ACAST(cast_dispel_evil)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_dispel_evil(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_dispel_evil(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj)
	 return;
      if (!victim)
	 victim = ch;
      spell_dispel_evil(level, ch, victim, 0);
      break;
   case SPELL_TYPE_WAND:
      if (tar_obj)
	 return;
      spell_dispel_evil(level, ch, victim, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_dispel_evil(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in dispel evil!");
      break;
   }
}


ACAST(cast_enchant_weapon)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_enchant_weapon(level, ch, 0, tar_obj);
      break;

   case SPELL_TYPE_SCROLL:
      if (!tar_obj)
	 return;
      spell_enchant_weapon(level, ch, 0, tar_obj);
      break;
   default :
      log("SYSERR: Serious screw-up in enchant weapon!");
      break;
   }
}


ACAST(cast_heal)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      act("$n heals $N.", FALSE, ch, 0, victim, TO_NOTVICT);
      act("You heal $N.", FALSE, ch, 0, victim, TO_CHAR);
      spell_heal(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_heal(level, ch, ch, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_heal(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in heal!");
      break;
   }
}


ACAST(cast_invisibility)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      if (tar_obj) {
	 if ( IS_SET(tar_obj->obj_flags.extra_flags, ITEM_INVISIBLE) )
	    send_to_char("Nothing new seems to happen.\n\r", ch);
	 else
	    spell_invisibility(level, ch, 0, tar_obj);
      } else { /* victim */
	 if ( IS_AFFECTED(victim, AFF_INVISIBLE) )
	    send_to_char("Nothing new seems to happen.\n\r", ch);
	 else
	    spell_invisibility(level, ch, victim, 0);
      }
      break;
   case SPELL_TYPE_POTION:
      if (!IS_AFFECTED(ch, AFF_INVISIBLE) )
	 spell_invisibility(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj) {
	 if (!(IS_SET(tar_obj->obj_flags.extra_flags, ITEM_INVISIBLE)) )
	    spell_invisibility(level, ch, 0, tar_obj);
      } else { /* victim */
	 if (!victim)
	    victim = ch;

	 if (!( IS_AFFECTED(victim, AFF_INVISIBLE)) )
	    spell_invisibility(level, ch, victim, 0);
      }
      break;
   case SPELL_TYPE_WAND:
      if (tar_obj) {
	 if (!(IS_SET(tar_obj->obj_flags.extra_flags, ITEM_INVISIBLE)) )
	    spell_invisibility(level, ch, 0, tar_obj);
      } else { /* victim */
	 if (!( IS_AFFECTED(victim, AFF_INVISIBLE)) )
	    spell_invisibility(level, ch, victim, 0);
      }
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    if (!( IS_AFFECTED(victim, AFF_INVISIBLE)) )
	       spell_invisibility(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in invisibility!");
      break;
   }
}




ACAST(cast_locate_object)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_locate_object(level, ch, 0, tar_obj);
      break;
   default :
      log("SYSERR: Serious screw-up in locate object!");
      break;
   }
}


ACAST(cast_poison)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_poison(level, ch, victim, tar_obj);
      break;
   case SPELL_TYPE_POTION:
      spell_poison(level, ch, ch, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_poison(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in poison!");
      break;
   }
}


ACAST(cast_protection_from_evil)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_protection_from_evil(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_protection_from_evil(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj)
	 return;
      if (!victim)
	 victim = ch;
      spell_protection_from_evil(level, ch, victim, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_protection_from_evil(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in protection from evil!");
      break;
   }
}


ACAST(cast_remove_curse)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_remove_curse(level, ch, victim, tar_obj);
      break;
   case SPELL_TYPE_POTION:
      spell_remove_curse(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj) {
	 spell_remove_curse(level, ch, 0, tar_obj);
	 return;
      }
      if (!victim)
	 victim = ch;
      spell_remove_curse(level, ch, victim, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_remove_curse(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in remove curse!");
      break;
   }
}



ACAST(cast_remove_poison)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_remove_poison(level, ch, victim, tar_obj);
      break;
   case SPELL_TYPE_POTION:
      spell_remove_poison(level, ch, ch, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_remove_poison(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in remove poison!");
      break;
   }
}



ACAST(cast_sanctuary)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_sanctuary(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_sanctuary(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj)
	 return;
      if (!victim)
	 victim = ch;
      spell_sanctuary(level, ch, victim, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_sanctuary(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in sanctuary!");
      break;
   }
}


ACAST(cast_sleep)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_sleep(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_sleep(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj)
	 return;
      if (!victim)
	 victim = ch;
      spell_sleep(level, ch, victim, 0);
      break;
   case SPELL_TYPE_WAND:
      if (tar_obj)
	 return;
      spell_sleep(level, ch, victim, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_sleep(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in sleep!");
      break;
   }
}


ACAST(cast_strength)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_strength(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_strength(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj)
	 return;
      if (!victim)
	 victim = ch;
      spell_strength(level, ch, victim, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_strength(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in strength!");
      break;
   }
}


ACAST(cast_ventriloquate)
{
   struct char_data *tmp_ch;
   char	buf1[MAX_STRING_LENGTH];
   char	buf2[MAX_STRING_LENGTH];
   char	buf3[MAX_STRING_LENGTH];

   if (type != SPELL_TYPE_SPELL) {
      log("SYSERR: Attempt to ventriloquate by non-cast-spell.");
      return;
   }
   for (; *arg && (*arg == ' '); arg++)
      ;
   if (tar_obj) {
      sprintf(buf1, "The %s says '%s'\n\r", fname(tar_obj->name), arg);
      sprintf(buf2, "Someone makes it sound like the %s says '%s'.\n\r",
          fname(tar_obj->name), arg);
   }	 else {
      sprintf(buf1, "%s says '%s'\n\r", GET_NAME(victim), arg);
      sprintf(buf2, "Someone makes it sound like %s says '%s'\n\r",
          GET_NAME(victim), arg);
   }

   sprintf(buf3, "Someone says, '%s'\n\r", arg);

   for (tmp_ch = world[ch->in_room].people; tmp_ch; 
       tmp_ch = tmp_ch->next_in_room) {

      if ((tmp_ch != ch) && (tmp_ch != victim)) {
	 if ( saves_spell(tmp_ch, SAVING_SPELL) )
	    send_to_char(buf2, tmp_ch);
	 else
	    send_to_char(buf1, tmp_ch);
      } else {
	 if (tmp_ch == victim)
	    send_to_char(buf3, victim);
      }
   }
}



ACAST(cast_word_of_recall)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_word_of_recall(level, ch, ch, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_word_of_recall(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (tar_obj)
	 return;
      if (!victim)
	 victim = ch;
      spell_word_of_recall(level, ch, victim, 0);
      break;
   case SPELL_TYPE_WAND:
      if (tar_obj)
	 return;
      spell_word_of_recall(level, ch, victim, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_word_of_recall(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in word of recall!");
      break;
   }
}



ACAST(cast_summon)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_summon(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in summon!");
      break;
   }
}



ACAST(cast_charm_person)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_charm_person(level, ch, victim, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (!victim)
	 return;
      spell_charm_person(level, ch, victim, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_charm_person(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in charm person!");
      break;
   }
}



ACAST(cast_sense_life)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_sense_life(level, ch, ch, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_sense_life(level, ch, ch, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_sense_life(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in sense life!");
      break;
   }
}


ACAST(cast_identify)
{
   switch (type) {
   case SPELL_TYPE_SCROLL:
      spell_identify(level, ch, victim, tar_obj);
      break;
   default :
      log("SYSERR: Serious screw-up in identify!");
      break;
   }
}


ACAST(cast_fire_breath)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_fire_breath(level, ch, victim, 0);
      break;   /* It's a spell.. But people can'c cast it! */
   default :
      log("SYSERR: Serious screw-up in firebreath!");
      break;
   }
}


ACAST(cast_frost_breath)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_frost_breath(level, ch, victim, 0);
      break;   /* It's a spell.. But people can'c cast it! */
   default :
      log("SYSERR: Serious screw-up in frostbreath!");
      break;
   }
}


ACAST(cast_acid_breath)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_acid_breath(level, ch, victim, 0);
      break;   /* It's a spell.. But people can'c cast it! */
   default :
      log("SYSERR: Serious screw-up in acidbreath!");
      break;
   }
}


ACAST(cast_gas_breath)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room)
	 if (victim != ch)
	    spell_gas_breath(level, ch, victim, 0);
      break;
      /* THIS ONE HURTS!! */
   default :
      log("SYSERR: Serious screw-up in gasbreath!");
      break;
   }
}


ACAST(cast_lightning_breath)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_lightning_breath(level, ch, victim, 0);
      break;   /* It's a spell.. But people can'c cast it! */
   default :
      log("SYSERR: Serious screw-up in lightningbreath!");
      break;
   }
}


