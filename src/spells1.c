/* ************************************************************************
*   File: spells1.c                                     Part of CircleMUD *
*  Usage: step 1 implementation of offensive magic (step 2 in magic.c)    *
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


ACAST(cast_burning_hands)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_burning_hands(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in burning hands!");
      break;
   }
}


ACAST(cast_call_lightning)
{

   switch (type) {
   case SPELL_TYPE_SPELL:
      if (OUTSIDE(ch) && (weather_info.sky >= SKY_RAINING)) {
	 spell_call_lightning(level, ch, victim, 0);
      } else {
	 send_to_char("You fail to call upon the lightning from the sky!\n\r", ch);
      }
      break;
   case SPELL_TYPE_POTION:
      if (OUTSIDE(ch) && (weather_info.sky >= SKY_RAINING)) {
	 spell_call_lightning(level, ch, ch, 0);
      }
      break;
   case SPELL_TYPE_SCROLL:
      if (OUTSIDE(ch) && (weather_info.sky >= SKY_RAINING)) {
	 if (victim)
	    spell_call_lightning(level, ch, victim, 0);
	 else if (!tar_obj)
	    spell_call_lightning(level, ch, ch, 0);
      }
      break;
   case SPELL_TYPE_STAFF:
      if (OUTSIDE(ch) && (weather_info.sky >= SKY_RAINING)) {
	 for (victim = world[ch->in_room].people ; 
	     victim ; victim = victim->next_in_room )
	    if (victim != ch)
	       spell_call_lightning(level, ch, victim, 0);
      }
      break;
   default :
      log("SYSERR: Serious screw-up in call lightning!");
      break;
   }
}


ACAST(cast_chill_touch)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_chill_touch(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in chill touch!");
      break;
   }
}


ACAST(cast_shocking_grasp)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_shocking_grasp(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in shocking grasp!");
      break;
   }
}


ACAST(cast_colour_spray)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_colour_spray(level, ch, victim, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (victim)
	 spell_colour_spray(level, ch, victim, 0);
      else if (!tar_obj)
	 spell_colour_spray(level, ch, ch, 0);
      break;
   case SPELL_TYPE_WAND:
      if (victim)
	 spell_colour_spray(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in colour spray!");
      break;
   }
}


ACAST(cast_earthquake)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
   case SPELL_TYPE_SCROLL:
   case SPELL_TYPE_STAFF:
      spell_earthquake(level, ch, 0, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in earthquake!");
      break;
   }
}


ACAST(cast_energy_drain)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_energy_drain(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_energy_drain(level, ch, ch, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (victim)
	 spell_energy_drain(level, ch, victim, 0);
      else if (!tar_obj)
	 spell_energy_drain(level, ch, ch, 0);
      break;
   case SPELL_TYPE_WAND:
      if (victim)
	 spell_energy_drain(level, ch, victim, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room )
	 if (victim != ch)
	    spell_energy_drain(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in energy drain!");
      break;
   }
}


ACAST(cast_fireball)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_fireball(level, ch, victim, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (victim)
	 spell_fireball(level, ch, victim, 0);
      else if (!tar_obj)
	 spell_fireball(level, ch, ch, 0);
      break;
   case SPELL_TYPE_WAND:
      if (victim)
	 spell_fireball(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in fireball!");
      break;

   }
}


ACAST(cast_harm)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_harm(level, ch, victim, 0);
      break;
   case SPELL_TYPE_POTION:
      spell_harm(level, ch, ch, 0);
      break;
   case SPELL_TYPE_STAFF:
      for (victim = world[ch->in_room].people ; 
          victim ; victim = victim->next_in_room )
	 if (victim != ch)
	    spell_harm(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in harm!");
      break;

   }
}


ACAST(cast_lightning_bolt)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_lightning_bolt(level, ch, victim, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (victim)
	 spell_lightning_bolt(level, ch, victim, 0);
      else if (!tar_obj)
	 spell_lightning_bolt(level, ch, ch, 0);
      break;
   case SPELL_TYPE_WAND:
      if (victim)
	 spell_lightning_bolt(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in lightning bolt!");
      break;

   }
}


ACAST(cast_magic_missile)
{
   switch (type) {
   case SPELL_TYPE_SPELL:
      spell_magic_missile(level, ch, victim, 0);
      break;
   case SPELL_TYPE_SCROLL:
      if (victim)
	 spell_magic_missile(level, ch, victim, 0);
      else if (!tar_obj)
	 spell_magic_missile(level, ch, ch, 0);
      break;
   case SPELL_TYPE_WAND:
      if (victim)
	 spell_magic_missile(level, ch, victim, 0);
      break;
   default :
      log("SYSERR: Serious screw-up in magic missile!");
      break;

   }
}


