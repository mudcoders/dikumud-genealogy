/* ************************************************************************
*   File: act.offensive.c                               Part of CircleMUD *
*  Usage: player-level commands of an offensive nature                    *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"

/* extern variables */
extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern struct room_data *world;

/* extern functions */
void	raw_kill(struct char_data *ch);
int	do_simple_move(struct char_data *ch, int cmd, int following);


ACMD(do_assist)
{
   struct char_data *helpee, *opponent;

   if (ch->specials.fighting) {
      send_to_char("You're already fighting!  How can you assist someone else?\n\r", ch);
      return;
   }

   one_argument(argument, arg);

   if (!(helpee = get_char_room_vis(ch, arg)))
      send_to_char("Whom do you wish to assist?\n\r", ch);
   else if (helpee == ch)
      send_to_char("You can't help yourself any more than this!\n\r", ch);
   else
    {
      for (opponent = world[ch->in_room].people; opponent && 
          (opponent->specials.fighting != helpee); 
          opponent = opponent->next_in_room)
	 ;

      if (!opponent)
	 act("But nobody is fighting $M!", FALSE, ch, 0, helpee, TO_CHAR);
      else if (!CAN_SEE(ch, opponent))
	 act("You can't see who is fighting $M!", FALSE, ch, 0, helpee, TO_CHAR);
      else {
	 send_to_char("You join the fight!\n\r", ch);
	 act("$N assists you!", 0, helpee, 0, ch, TO_CHAR);
	 act("$n assists $N.", FALSE, ch, 0, helpee, TO_NOTVICT);
	 hit(ch, opponent, TYPE_UNDEFINED);
      }
   }
}


ACMD(do_hit)
{
   struct char_data *victim;

   one_argument(argument, arg);

   if (*arg) {
      victim = get_char_room_vis(ch, arg);
      if (victim) {
	 if (victim == ch) {
	    send_to_char("You hit yourself...OUCH!.\n\r", ch);
	    act("$n hits $mself, and says OUCH!", FALSE, ch, 0, victim, TO_ROOM);
	    return;
	 }
	 if (!IS_NPC(victim) && !IS_NPC(ch) && (subcmd != SCMD_MURDER)) {
	    send_to_char("To avoid accidental flagging, you have to use the MURDER command to\n\r"
	        "hit another player.\n\r", ch);
	    return;
	 }
	 if (IS_AFFECTED(ch, AFF_CHARM) && !IS_NPC(ch->master) && !IS_NPC(victim))
	    return; /* you can't order a charmed pet to attack a player */
	 if (IS_AFFECTED(ch, AFF_CHARM) && (ch->master == victim)) {
	    act("$N is just such a good friend, you simply can't hit $M.", FALSE, ch, 0, victim, TO_CHAR);
	    return;
	 }
	 if ((GET_POS(ch) == POSITION_STANDING) && (victim != ch->specials.fighting)) {
	    hit(ch, victim, TYPE_UNDEFINED);
	    WAIT_STATE(ch, PULSE_VIOLENCE + 2); /* HVORFOR DET?? */
	 } else
	    send_to_char("You do the best you can!\n\r", ch);
      } else
	 send_to_char("They aren't here.\n\r", ch);
   } else
      send_to_char("Hit who?\n\r", ch);
}



ACMD(do_kill)
{
   struct char_data *victim;

   if ((GET_LEVEL(ch) < LEVEL_IMPL) || IS_NPC(ch)) {
      do_hit(ch, argument, cmd, subcmd);
      return;
   }

   one_argument(argument, arg);

   if (!*arg) {
      send_to_char("Kill who?\n\r", ch);
   } else {
      if (!(victim = get_char_room_vis(ch, arg)))
	 send_to_char("They aren't here.\n\r", ch);
      else if (ch == victim)
	 send_to_char("Your mother would be so sad.. :(\n\r", ch);
      else {
	 act("You chop $M to pieces!  Ah!  The blood!", FALSE, ch, 0, victim, TO_CHAR);
	 act("$N chops you to pieces!", FALSE, victim, 0, ch, TO_CHAR);
	 act("$n brutally slays $N!", FALSE, ch, 0, victim, TO_NOTVICT);
	 raw_kill(victim);
      }
   }
}



ACMD(do_backstab)
{
   struct char_data *victim;
   byte percent, prob;

   one_argument(argument, buf);

   if (!(victim = get_char_room_vis(ch, buf))) {
      send_to_char("Backstab who?\n\r", ch);
      return;
   }

   if (victim == ch) {
      send_to_char("How can you sneak up on yourself?\n\r", ch);
      return;
   }

   if (!ch->equipment[WIELD]) {
      send_to_char("You need to wield a weapon to make it a success.\n\r", ch);
      return;
   }

   if (ch->equipment[WIELD]->obj_flags.value[3] != 11) {
      send_to_char("Only piercing weapons can be used for backstabbing.\n\r", ch);
      return;
   }

   if (victim->specials.fighting) {
      send_to_char("You can't backstab a fighting person, too alert!\n\r", ch);
      return;
   }

   percent = number(1, 101); /* 101% is a complete failure */

   prob = GET_SKILL(ch, SKILL_BACKSTAB);

   if (AWAKE(victim) && (percent > prob))
      damage(ch, victim, 0, SKILL_BACKSTAB);
   else
      hit(ch, victim, SKILL_BACKSTAB);
}



ACMD(do_order)
{
   char	name[100], message[256];
   char	buf[256];
   bool found = FALSE;
   int	org_room;
   struct char_data *victim;
   struct follow_type *k;

   half_chop(argument, name, message);

   if (!*name || !*message)
      send_to_char("Order who to do what?\n\r", ch);
   else if (!(victim = get_char_room_vis(ch, name)) && !is_abbrev(name, "followers"))
      send_to_char("That person isn't here.\n\r", ch);
   else if (ch == victim)
      send_to_char("You obviously suffer from skitzofrenia.\n\r", ch);

   else {
      if (IS_AFFECTED(ch, AFF_CHARM)) {
	 send_to_char("Your superior would not aprove of you giving orders.\n\r", ch);
	 return;
      }

      if (victim) {
	 sprintf(buf, "$N orders you to '%s'", message);
	 act(buf, FALSE, victim, 0, ch, TO_CHAR);
	 act("$n gives $N an order.", FALSE, ch, 0, victim, TO_ROOM);

	 if ( (victim->master != ch) || !IS_AFFECTED(victim, AFF_CHARM) )
	    act("$n has an indifferent look.", FALSE, victim, 0, 0, TO_ROOM);
	 else {
	    send_to_char("Ok.\n\r", ch);
	    command_interpreter(victim, message);
	 }
      } else {  /* This is order "followers" */
	 sprintf(buf, "$n issues the order '%s'.", message);
	 act(buf, FALSE, ch, 0, victim, TO_ROOM);

	 org_room = ch->in_room;

	 for (k = ch->followers; k; k = k->next) {
	    if (org_room == k->follower->in_room)
	       if (IS_AFFECTED(k->follower, AFF_CHARM)) {
		  found = TRUE;
		  command_interpreter(k->follower, message);
	       }
	 }
	 if (found)
	    send_to_char("Ok.\n\r", ch);
	 else
	    send_to_char("Nobody here is a loyal subject of yours!\n\r", ch);
      }
   }
}



ACMD(do_flee)
{
   int	i, attempt, loose, die;

   void	gain_exp(struct char_data *ch, int gain);
   int	special(struct char_data *ch, int cmd, char *arg);

   if (!(ch->specials.fighting)) {
      for (i = 0; i < 6; i++) {
	 attempt = number(0, NUM_OF_DIRS-1);  /* Select a random direction */
	 if (CAN_GO(ch, attempt) && 
	     !IS_SET(world[EXIT(ch, attempt)->to_room].room_flags, DEATH)) {
	    act("$n panics, and attempts to flee!", TRUE, ch, 0, 0, TO_ROOM);
	    if ((die = do_simple_move(ch, attempt, FALSE)) == 1) {
	       /* The escape has succeded */
	       send_to_char("You flee head over heels.\n\r", ch);
	    } else {
	       if (!die)
		  act("$n tries to flee, but is too exhausted!", TRUE, ch, 0, 0, TO_ROOM);
	    }
	    return;
	 }
      } /* for */
      /* No exits was found */
      send_to_char("PANIC!  You couldn't escape!\n\r", ch);
      return;
   }

   for (i = 0; i < 6; i++) {
      attempt = number(0, NUM_OF_DIRS-1);  /* Select a random direction */
      if (CAN_GO(ch, attempt) && 
          !IS_SET(world[EXIT(ch, attempt)->to_room].room_flags, DEATH)) {
	 act("$n panics, and attempts to flee.", TRUE, ch, 0, 0, TO_ROOM);
	 if ((die = do_simple_move(ch, attempt, FALSE)) == 1) {
	    /* The escape has succeded */
	    loose = GET_MAX_HIT(ch->specials.fighting) - GET_HIT(ch->specials.fighting);
	    loose *= GET_LEVEL(ch->specials.fighting);

	    if (!IS_NPC(ch))
	       gain_exp(ch, -loose);

	    send_to_char("You flee head over heels.\n\r", ch);

	    /* Insert later when using huntig system        */
	    /* ch->specials.fighting->specials.hunting = ch */

	    if (ch->specials.fighting->specials.fighting == ch)
	       stop_fighting(ch->specials.fighting);
	    stop_fighting(ch);
	    return;
	 } else {
	    if (!die)
	       act("$n tries to flee, but is too exhausted!", TRUE, ch, 0, 0, TO_ROOM);
	    return;
	 }
      }
   } /* for */

   /* No exits was found */
   send_to_char("PANIC!  You couldn't escape!\n\r", ch);
}



ACMD(do_bash)
{
   struct char_data *victim;
   byte percent, prob;

   one_argument(argument, arg);

   if (GET_CLASS(ch) != CLASS_WARRIOR) {
      send_to_char("You'd better leave all the martial arts to fighters.\n\r", ch);
      return;
   }

   if (!(victim = get_char_room_vis(ch, arg))) {
      if (ch->specials.fighting) {
	 victim = ch->specials.fighting;
      } else {
	 send_to_char("Bash who?\n\r", ch);
	 return;
      }
   }

   if (victim == ch) {
      send_to_char("Aren't we funny today...\n\r", ch);
      return;
   }

   if (!ch->equipment[WIELD]) {
      send_to_char("You need to wield a weapon, to make it a success.\n\r", ch);
      return;
   }

   percent = number(1, 101); /* 101% is a complete failure */
   prob = GET_SKILL(ch, SKILL_BASH);

   if (percent > prob) {
      damage(ch, victim, 0, SKILL_BASH);
      GET_POS(ch) = POSITION_SITTING;
   } else {
      damage(ch, victim, 1, SKILL_BASH);
      GET_POS(victim) = POSITION_SITTING;
      WAIT_STATE(victim, PULSE_VIOLENCE * 2);
   }
   WAIT_STATE(ch, PULSE_VIOLENCE * 2);
}


ACMD(do_rescue)
{
   struct char_data *victim, *tmp_ch;
   byte	percent, prob;

   one_argument(argument, arg);

   if (!(victim = get_char_room_vis(ch, arg))) {
      send_to_char("Who do you want to rescue?\n\r", ch);
      return;
   }

   if (victim == ch) {
      send_to_char("What about fleeing instead?\n\r", ch);
      return;
   }

   if (ch->specials.fighting == victim) {
      send_to_char("How can you rescue someone you are trying to kill?\n\r", ch);
      return;
   }

   for (tmp_ch = world[ch->in_room].people; tmp_ch && 
       (tmp_ch->specials.fighting != victim); tmp_ch = tmp_ch->next_in_room)
      ;

   if (!tmp_ch) {
      act("But nobody is fighting $M!", FALSE, ch, 0, victim, TO_CHAR);
      return;
   }


   if (GET_CLASS(ch) != CLASS_WARRIOR)
      send_to_char("But only true warriors can do this!", ch);
   else {
      percent = number(1, 101); /* 101% is a complete failure */
      prob = GET_SKILL(ch, SKILL_RESCUE);

      if (percent > prob) {
	 send_to_char("You fail the rescue!\n\r", ch);
	 return;
      }

      send_to_char("Banzai!  To the rescue...\n\r", ch);
      act("You are rescued by $N, you are confused!", FALSE, victim, 0, ch, TO_CHAR);
      act("$n heroically rescues $N!", FALSE, ch, 0, victim, TO_NOTVICT);

      if (victim->specials.fighting == tmp_ch)
	 stop_fighting(victim);
      if (tmp_ch->specials.fighting)
	 stop_fighting(tmp_ch);
      if (ch->specials.fighting)
	 stop_fighting(ch);

      set_fighting(ch, tmp_ch);
      set_fighting(tmp_ch, ch);

      WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
   }

}



ACMD(do_kick)
{
   struct char_data *victim;
   byte percent, prob;

   if (GET_CLASS(ch) != CLASS_WARRIOR) {
      send_to_char("You'd better leave all the martial arts to fighters.\n\r", ch);
      return;
   }

   one_argument(argument, arg);

   if (!(victim = get_char_room_vis(ch, arg))) {
      if (ch->specials.fighting) {
	 victim = ch->specials.fighting;
      } else {
	 send_to_char("Kick who?\n\r", ch);
	 return;
      }
   }

   if (victim == ch) {
      send_to_char("Aren't we funny today...\n\r", ch);
      return;
   }

   percent = ((10 - (GET_AC(victim) / 10)) << 1) + number(1, 101); /* 101% is a complete failure */
   prob = GET_SKILL(ch, SKILL_KICK);

   if (percent > prob) {
      damage(ch, victim, 0, SKILL_KICK);
   } else
      damage(ch, victim, GET_LEVEL(ch) >> 1, SKILL_KICK);
   WAIT_STATE(ch, PULSE_VIOLENCE * 3);
}


