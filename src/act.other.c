/* ************************************************************************
*   File: act.other.c                                   Part of CircleMUD *
*  Usage: Miscellaneous player-level commands                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "screen.h"
#include "limits.h"

/* extern variables */
extern struct str_app_type str_app[];
extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern struct room_data *world;
extern struct dex_skill_type dex_app_skill[];
extern struct spell_info_type spell_info[];
extern struct index_data *mob_index;
extern char	*class_abbrevs[];

/* extern procedures */
SPECIAL(shop_keeper);


ACMD(do_quit)
{
   void	die(struct char_data *ch);

   if (IS_NPC(ch) || !ch->desc)
      return;

   if (subcmd != SCMD_QUIT) {
      send_to_char("You have to type quit - no less, to quit!\n\r", ch);
      return;
   }

   if (GET_POS(ch) == POSITION_FIGHTING) {
      send_to_char("No way!  You're fighting for your life!\n\r", ch);
      return;
   }

   if (GET_POS(ch) < POSITION_STUNNED) {
      send_to_char("You die before your time...\n\r", ch);
      die(ch);
      return;
   }

   act("Goodbye, friend.. Come back soon!", FALSE, ch, 0, 0, TO_CHAR);
   if (!GET_INVIS_LEV(ch))
      act("$n has left the game.", TRUE, ch, 0, 0, TO_ROOM);
   sprintf(buf, "%s has quit the game.", GET_NAME(ch));
   mudlog(buf, NRM, MAX(LEVEL_GOD, GET_INVIS_LEV(ch)), TRUE);
   extract_char(ch); /* Char is saved in extract char */
}



ACMD(do_save)
{
   if (IS_NPC(ch) || !ch->desc)
      return;

   if (cmd != 0) {
      sprintf(buf, "Saving %s.\n\r", GET_NAME(ch));
      send_to_char(buf, ch);
   }
   save_char(ch, NOWHERE);
   Crash_crashsave(ch);
}


ACMD(do_not_here)
{
   send_to_char("Sorry, but you can't do that here!\n\r", ch);
}



ACMD(do_sneak)
{
   struct affected_type af;
   byte percent;

   send_to_char("Ok, you'll try to move silently for a while.\n\r", ch);
   if (IS_AFFECTED(ch, AFF_SNEAK))
      affect_from_char(ch, SKILL_SNEAK);

   percent = number(1, 101); /* 101% is a complete failure */

   if (percent > GET_SKILL(ch, SKILL_SNEAK) + dex_app_skill[GET_DEX(ch)].sneak)
      return;

   af.type = SKILL_SNEAK;
   af.duration = GET_LEVEL(ch);
   af.modifier = 0;
   af.location = APPLY_NONE;
   af.bitvector = AFF_SNEAK;
   affect_to_char(ch, &af);
}



ACMD(do_hide)
{
   byte percent;

   send_to_char("You attempt to hide yourself.\n\r", ch);

   if (IS_AFFECTED(ch, AFF_HIDE))
      REMOVE_BIT(ch->specials.affected_by, AFF_HIDE);

   percent = number(1, 101); /* 101% is a complete failure */

   if (percent > GET_SKILL(ch, SKILL_HIDE) + dex_app_skill[GET_DEX(ch)].hide)
      return;

   SET_BIT(ch->specials.affected_by, AFF_HIDE);
}




ACMD(do_steal)
{
   struct char_data *victim;
   struct obj_data *obj;
   char	victim_name[240];
   char	obj_name[240];
   int	percent, gold, eq_pos, pcsteal = 0;
   extern int	pt_allowed;
   bool ohoh = FALSE;

   ACMD(do_gen_com);

   argument = one_argument(argument, obj_name);
   one_argument(argument, victim_name);

   if (!(victim = get_char_room_vis(ch, victim_name))) {
      send_to_char("Steal what from who?\n\r", ch);
      return;
   } else if (victim == ch) {
      send_to_char("Come on now, that's rather stupid!\n\r", ch);
      return;
   }

   if (!pt_allowed) {
      if (!IS_NPC(victim) && !PLR_FLAGGED(victim, PLR_THIEF) && 
          !PLR_FLAGGED(victim, PLR_KILLER) && !PLR_FLAGGED(ch, PLR_THIEF)) {
          /*  SET_BIT(ch->specials.act, PLR_THIEF);
	      send_to_char("OK, you're the boss... you're now a THIEF!\n\r",ch);
	      sprintf(buf, "PC Thief bit set on %s", GET_NAME(ch));
	      log(buf);
          */
           pcsteal = 1;
      }

      if (PLR_FLAGGED(ch, PLR_THIEF))
         pcsteal = 1;

      /* We'll try something different... instead of having a thief flag,
         just have PC Steals fail all the time.
      */
   }

   /* 101% is a complete failure */
   percent = number(1, 101) - dex_app_skill[GET_DEX(ch)].p_pocket;

   if (GET_POS(victim) < POSITION_SLEEPING)
      percent = -1; /* ALWAYS SUCCESS */

   /* NO NO With Imp's and Shopkeepers! */
   if ((GET_LEVEL(victim) >= LEVEL_IMMORT) || pcsteal || 
       (IS_MOB(victim) && mob_index[victim->nr].func == shop_keeper))
      percent = 101; /* Failure */

   if (str_cmp(obj_name, "coins") && str_cmp(obj_name, "gold")) {

      if (!(obj = get_obj_in_list_vis(victim, obj_name, victim->carrying))) {

	 for (eq_pos = 0; (eq_pos < MAX_WEAR); eq_pos++)
	    if (victim->equipment[eq_pos] && 
	        (isname(obj_name, victim->equipment[eq_pos]->name)) && 
	        CAN_SEE_OBJ(ch, victim->equipment[eq_pos])) {
	       obj = victim->equipment[eq_pos];
	       break;
	    }

	 if (!obj) {
	    act("$E hasn't got that item.", FALSE, ch, 0, victim, TO_CHAR);
	    return;
	 } else { /* It is equipment */
	    if ((GET_POS(victim) > POSITION_STUNNED)) {
	       send_to_char("Steal the equipment now?  Impossible!\n\r", ch);
	       return;
	    } else {
	       act("You unequip $p and steal it.", FALSE, ch, obj , 0, TO_CHAR);
	       act("$n steals $p from $N.", FALSE, ch, obj, victim, TO_NOTVICT);
	       obj_to_char(unequip_char(victim, eq_pos), ch);
	    }
	 }
      } else {  /* obj found in inventory */

	 percent += GET_OBJ_WEIGHT(obj); /* Make heavy harder */

	 if (AWAKE(victim) && (percent > GET_SKILL(ch, SKILL_STEAL))) {
	    ohoh = TRUE;
	    act("Oops..", FALSE, ch, 0, 0, TO_CHAR);
	    act("$n tried to steal something from you!", FALSE, ch, 0, victim, TO_VICT);
	    act("$n tries to steal something from $N.", TRUE, ch, 0, victim, TO_NOTVICT);
	 } else { /* Steal the item */
	    if ((IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))) {
	       if ((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) < CAN_CARRY_W(ch)) {
		  obj_from_char(obj);
		  obj_to_char(obj, ch);
		  send_to_char("Got it!\n\r", ch);
	       }
	    } else
	       send_to_char("You cannot carry that much.\n\r", ch);
	 }
      }
   } else { /* Steal some coins */
      if (AWAKE(victim) && (percent > GET_SKILL(ch, SKILL_STEAL))) {
	 ohoh = TRUE;
	 act("Oops..", FALSE, ch, 0, 0, TO_CHAR);
	 act("You discover that $n has $s hands in your wallet.", FALSE, ch, 0, victim, TO_VICT);
	 act("$n tries to steal gold from $N.", TRUE, ch, 0, victim, TO_NOTVICT);
      } else {
	 /* Steal some gold coins */
	 gold = (int) ((GET_GOLD(victim) * number(1, 10)) / 100);
	 gold = MIN(1782, gold);
	 if (gold > 0) {
	    GET_GOLD(ch) += gold;
	    GET_GOLD(victim) -= gold;
	    sprintf(buf, "Bingo!  You got %d gold coins.\n\r", gold);
	    send_to_char(buf, ch);
	 } else {
	    send_to_char("You couldn't get any gold...\n\r", ch);
	 }
      }
   }

   if (ohoh && IS_NPC(victim) && AWAKE(victim))
      if (IS_SET(MOB_FLAGS(victim), MOB_NICE_THIEF)) {
	 sprintf(buf, "%s is a bloody thief!", GET_NAME(ch));
	 do_gen_com(victim, buf, 0, SCMD_SHOUT);
	 log(buf);
	 send_to_char("Don't you ever do that again!\n\r", ch);
      } 
      else
	 hit(victim, ch, TYPE_UNDEFINED);
}



ACMD(do_practice)
{
   SPECIAL(guild);

   one_argument(argument, arg);

   if (*arg)
      send_to_char("You can only practice skills in your guild.\n\r", ch);
   else
      (void) guild(ch, cmd, "");
}



ACMD(do_visible)
{
   void	appear(struct char_data *ch);

   if IS_AFFECTED(ch, AFF_INVISIBLE) {
      appear(ch);
      send_to_char("You break the spell of invisibility.\n\r", ch);
   } else
      send_to_char("You are already visible.\n\r", ch);
}



ACMD(do_title)
{
   for (; isspace(*argument); argument++)
      ;

   if (IS_NPC(ch))
      send_to_char("Your title is fine... go away.\n\r", ch);
   else if (PLR_FLAGGED(ch, PLR_NOTITLE))
      send_to_char("You can't title yourself -- you shouldn't have abused it!\n\r", ch);
   else if (!*argument)
      send_to_char("What kind of title is THAT!?\n\r", ch);
   else if (strstr(argument, "(") || strstr(argument, ")"))
      send_to_char("Titles can't contain the ( or ) characters.\n\r", ch);
   else if (strlen(argument) > 80)
      send_to_char("Sorry, titles can't be longer than 80 characters.\n\r", ch);
   else {
      if (GET_TITLE(ch))
         RECREATE(GET_TITLE(ch), char, strlen(argument) + 1);
      else
         CREATE(GET_TITLE(ch), char, strlen(argument) + 1);
      strcpy(GET_TITLE(ch), argument);

      sprintf(buf, "OK, you're now %s %s.\n\r", GET_NAME(ch), GET_TITLE(ch));
      send_to_char(buf, ch);
   }
}


ACMD(do_group)
{
   struct char_data *victim, *k;
   struct follow_type *f;
   bool found;

   one_argument(argument, buf);

   if (!*buf) {
      if (!IS_AFFECTED(ch, AFF_GROUP)) {
	 send_to_char("But you are not the member of a group!\n\r", ch);
      } else {
	 send_to_char("Your group consists of:\n\r", ch);

	 k = (ch->master ? ch->master : ch);

	 if (IS_AFFECTED(k, AFF_GROUP)) {
	    sprintf(buf, "     [%3dH %3dM %2dV] [%2d %s] $N (Head of group)",
	        GET_HIT(k), GET_MANA(k), GET_MOVE(k), GET_LEVEL(k), CLASS_ABBR(k));
	    act(buf, FALSE, ch, 0, k, TO_CHAR);
	 }

	 for (f = k->followers; f; f = f->next)
	    if (IS_AFFECTED(f->follower, AFF_GROUP)) {
	       sprintf(buf, "     [%3dH %3dM %2dV] [%2d %s] $N",
	           GET_HIT(f->follower), GET_MANA(f->follower),
	           GET_MOVE(f->follower), GET_LEVEL(f->follower),
	           CLASS_ABBR(f->follower));
	       act(buf, FALSE, ch, 0, f->follower, TO_CHAR);
	    }
      }

      return;
   }

   if (ch->master) {
      act("You can not enroll group members without being head of a group.",
          FALSE, ch, 0, 0, TO_CHAR);
      return;
   }

   if (!str_cmp(buf, "all")) {
      SET_BIT(ch->specials.affected_by, AFF_GROUP);
      for (f = ch->followers; f; f = f->next) {
	 victim = f->follower;
	 if (!IS_AFFECTED(victim, AFF_GROUP)) {
	    act("$N is now a member of your group.", FALSE, ch, 0, victim, TO_CHAR);
	    act("You are now a member of $n's group.", FALSE, ch, 0, victim, TO_VICT);
	    act("$N is now a member of $n's group.", FALSE, ch, 0, victim, TO_NOTVICT);
	    SET_BIT(victim->specials.affected_by, AFF_GROUP);
	 }
      }
      return;
   }

   if (!(victim = get_char_room_vis(ch, buf))) {
      send_to_char("No one here by that name.\n\r", ch);
   } else {
      found = FALSE;

      if (victim == ch)
	 found = TRUE;
      else {
	 for (f = ch->followers; f; f = f->next) {
	    if (f->follower == victim) {
	       found = TRUE;
	       break;
	    }
	 }
      }

      if (found) {
	 if (IS_AFFECTED(victim, AFF_GROUP)) {
	    act("$N is no longer a member of your group.", FALSE, ch, 0, victim, TO_CHAR);
	    act("You have been kicked out of $n's group!", FALSE, ch, 0, victim, TO_VICT);
	    act("$N has been kicked out of $n's group!", FALSE, ch, 0, victim, TO_NOTVICT);
	    REMOVE_BIT(victim->specials.affected_by, AFF_GROUP);
	 } else {
	    act("$N is now a member of your group.", FALSE, ch, 0, victim, TO_CHAR);
	    act("You are now a member of $n's group.", FALSE, ch, 0, victim, TO_VICT);
	    act("$N is now a member of $n's group.", FALSE, ch, 0, victim, TO_NOTVICT);
	    SET_BIT(victim->specials.affected_by, AFF_GROUP);
	 }
      } else
	 act("$N must follow you to enter your group.", FALSE, ch, 0, victim, TO_CHAR);
   }
}


ACMD(do_ungroup)
{
   struct follow_type *f, *next_fol;
   struct char_data *tch;
   void	stop_follower(struct char_data *ch);

   one_argument(argument, buf);

   if (!*buf) {
      if (ch->master || !(IS_AFFECTED(ch, AFF_GROUP))) {
	 send_to_char("But you lead no group!\n\r", ch);
	 return;
      }

      sprintf(buf2, "%s has disbanded the group.\n\r", GET_NAME(ch));
      for (f = ch->followers; f; f = next_fol) {
	 next_fol = f->next;
	 if (IS_AFFECTED(f->follower, AFF_GROUP)) {
	    REMOVE_BIT(f->follower->specials.affected_by, AFF_GROUP);
	    send_to_char(buf2, f->follower);
	    stop_follower(f->follower);
	 }
      }

      send_to_char("You have disbanded the group.\n\r", ch);
      return;
   }

   if (!(tch = get_char_room_vis(ch, buf))) {
      send_to_char("There is no such person!\n\r", ch);
      return;
   }

   if (tch->master != ch) {
      send_to_char("That person is not following you!\n\r", ch);
      return;
   }

   if (IS_AFFECTED(tch, AFF_GROUP))
      REMOVE_BIT(tch->specials.affected_by, AFF_GROUP);

   act("$N is no longer a member of your group.", FALSE, ch, 0, tch, TO_CHAR);
   act("You have been kicked out of $n's group!", FALSE, ch, 0, tch, TO_VICT);
   act("$N has been kicked out of $n's group!", FALSE, ch, 0, tch, TO_NOTVICT);
   stop_follower(tch);
}




ACMD(do_report)
{
   struct char_data *k;
   struct follow_type *f;

   if (!IS_AFFECTED(ch, AFF_GROUP)) {
      send_to_char("But you are not a member of any group!\n\r", ch);
      return;
   }

   sprintf(buf, "%s reports: %3d/%3dH  %3d/%3dM\n\r",
       GET_NAME(ch), GET_HIT(ch), GET_MAX_HIT(ch),
       GET_MANA(ch), GET_MAX_MANA(ch));

   k = (ch->master ? ch->master : ch);

   for (f = k->followers; f; f = f->next)
      if (IS_AFFECTED(f->follower, AFF_GROUP) && f->follower != ch)
	 send_to_char(buf, f->follower);
   if (k != ch)
      send_to_char(buf, k);
   send_to_char("You report to the group.\n\r", ch);
}



ACMD(do_split)
{
   int	amount, num, share;
   struct char_data *k;
   struct follow_type *f;

   if (IS_NPC(ch))
      return;

   one_argument(argument, buf);

   if (is_number(buf)) {
      amount = atoi(buf);
      if (amount <= 0) {
	 send_to_char("Sorry, you can't do that.\n\r", ch);
	 return;
      }

      if (amount > GET_GOLD(ch)) {
	 send_to_char("You don't seem to have that much gold to split.\n\r", ch);
	 return;
      }

      k = (ch->master ? ch->master : ch);

      if (IS_AFFECTED(k, AFF_GROUP) && (k->in_room == ch->in_room))
	 num = 1;
      else
	 num = 0;

      for (f = k->followers; f; f = f->next)
	 if (IS_AFFECTED(f->follower, AFF_GROUP) && 
	     (!IS_NPC(f->follower)) && 
	     (f->follower->in_room == ch->in_room))
	    num++;

      if (num && IS_AFFECTED(ch, AFF_GROUP))
	 share = amount / num;
      else {
	 send_to_char("With whom do you wish to share your gold?\n\r", ch);
	 return;
      }

      GET_GOLD(ch) -= share * (num - 1);

      if (IS_AFFECTED(k, AFF_GROUP) && (k->in_room == ch->in_room)
           && !(IS_NPC(k)) &&  k != ch) {
	 GET_GOLD(k) += share;
	 sprintf(buf, "%s splits %d coins; you receive %d.\n\r", GET_NAME(ch),
	     amount, share);
	 send_to_char(buf, k);
      }

      for (f = k->followers; f; f = f->next) {
	 if (IS_AFFECTED(f->follower, AFF_GROUP) && 
	     (!IS_NPC(f->follower)) && 
	     (f->follower->in_room == ch->in_room) && 
	     f->follower != ch) {
	    GET_GOLD(f->follower) += share;
	    sprintf(buf, "%s splits %d coins; you receive %d.\n\r", GET_NAME(ch),
	        amount, share);
	    send_to_char(buf, f->follower);
	 }
      }
      sprintf(buf, "You split %d coins among %d members; %d coins each.\n\r",
          amount, num, share);
      send_to_char(buf, ch);
   } else {
      send_to_char("How many coins do you wish to split with your group?\n\r", ch);
      return;
   }
}



ACMD(do_quaff)
{
   struct obj_data *temp;
   int	i;
   bool equipped;

   equipped = FALSE;

   one_argument(argument, buf);

   if (!(temp = get_obj_in_list_vis(ch, buf, ch->carrying))) {
      temp = ch->equipment[HOLD];
      equipped = TRUE;
      if ((temp == 0) || !isname(buf, temp->name)) {
	 act("You do not have that item.", FALSE, ch, 0, 0, TO_CHAR);
	 return;
      }
   }

   if (temp->obj_flags.type_flag != ITEM_POTION) {
      act("You can only quaff potions.", FALSE, ch, 0, 0, TO_CHAR);
      return;
   }

   act("$n quaffs $p.", TRUE, ch, temp, 0, TO_ROOM);
   act("You quaff $p which dissolves.", FALSE, ch, temp, 0, TO_CHAR);

   for (i = 1; i < 4; i++)
      if (temp->obj_flags.value[i] >= 1)
	 ((*spell_info[temp->obj_flags.value[i]].spell_pointer)
	     ((byte) temp->obj_flags.value[0], ch, "", SPELL_TYPE_POTION, ch, 0));

   if (equipped)
      unequip_char(ch, HOLD);

   extract_obj(temp);
}


ACMD(do_recite)
{
   struct obj_data *scroll, *obj;
   struct char_data *victim;
   int	i, bits;
   bool equipped;

   equipped = FALSE;
   obj = 0;
   victim = 0;

   argument = one_argument(argument, buf);

   if (!(scroll = get_obj_in_list_vis(ch, buf, ch->carrying))) {
      scroll = ch->equipment[HOLD];
      equipped = TRUE;

      if ((scroll == 0) || !isname(buf, scroll->name)) {
	 act("You do not have that item.", FALSE, ch, 0, 0, TO_CHAR);
	 return;
      }
   }

   if (scroll->obj_flags.type_flag != ITEM_SCROLL) {
      act("Recite is normally used for scrolls.", FALSE, ch, 0, 0, TO_CHAR);
      return;
   }

   if (*argument) {
      bits = generic_find(argument, FIND_OBJ_INV | FIND_OBJ_ROOM | 
          FIND_OBJ_EQUIP | FIND_CHAR_ROOM, ch, &victim, &obj);
      if (bits == 0) {
	 send_to_char("No such thing around to recite the scroll on.\n\r", ch);
	 return;
      }
   } else
    {
      victim = ch;
   }

   act("$n recites $p.", TRUE, ch, scroll, 0, TO_ROOM);
   act("You recite $p which dissolves.", FALSE, ch, scroll, 0, TO_CHAR);

   if (equipped)
      unequip_char(ch, HOLD);

   for (i = 1; i < 4; i++)
      if (scroll->obj_flags.value[i] >= 1)
	 ((*spell_info[scroll->obj_flags.value[i]].spell_pointer)
	     ((byte) scroll->obj_flags.value[0], ch, "", SPELL_TYPE_SCROLL, victim, obj));

   extract_obj(scroll);

}



ACMD(do_use)
{
   struct char_data *tmp_char;
   struct obj_data *tmp_object, *stick;

   int	bits;

   argument = one_argument(argument, buf);

   if (ch->equipment[HOLD] == 0 || 
       !isname(buf, ch->equipment[HOLD]->name)) {
      act("You do not hold that item in your hand.", FALSE, ch, 0, 0, TO_CHAR);
      return;
   }

   stick = ch->equipment[HOLD];

   if (stick->obj_flags.type_flag == ITEM_STAFF) {
      act("$n taps $p three times on the ground.", TRUE, ch, stick, 0, TO_ROOM);
      act("You tap $p three times on the ground.", FALSE, ch, stick, 0, TO_CHAR);

      if (stick->obj_flags.value[2] > 0) {  /* Is there any charges left? */
	 stick->obj_flags.value[2]--;
	 ((*spell_info[stick->obj_flags.value[3]].spell_pointer)
	     ((byte) stick->obj_flags.value[0], ch, "", SPELL_TYPE_STAFF, 0, 0));

      } else {
	 send_to_char("The staff seems powerless.\n\r", ch);
      }
   } else if (stick->obj_flags.type_flag == ITEM_WAND) {

      bits = generic_find(argument, FIND_CHAR_ROOM | FIND_OBJ_INV | FIND_OBJ_ROOM | 
          FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);
      if (bits) {
	 if (bits == FIND_CHAR_ROOM) {
	    act("$n points $p at $N.", TRUE, ch, stick, tmp_char, TO_ROOM);
	    act("You point $p at $N.", FALSE, ch, stick, tmp_char, TO_CHAR);
	 } else {
	    act("$n points $p at $P.", TRUE, ch, stick, tmp_object, TO_ROOM);
	    act("You point $p at $P.", FALSE, ch, stick, tmp_object, TO_CHAR);
	 }

	 if (stick->obj_flags.value[2] > 0) { /* Is there any charges left? */
	    stick->obj_flags.value[2]--;
	    ((*spell_info[stick->obj_flags.value[3]].spell_pointer)
	        ((byte) stick->obj_flags.value[0], ch, "", SPELL_TYPE_WAND, tmp_char, tmp_object));
	 } else
	    send_to_char("The wand seems powerless.\n\r", ch);
      } else
	 send_to_char("What should the wand be pointed at?\n\r", ch);
   } else
      send_to_char("Use is normally only for wands and staffs.\n\r", ch);
}


ACMD(do_wimpy)
{
   int	wimp_lev;

   one_argument(argument, arg);

   if (!*arg) {
      if (WIMP_LEVEL(ch)) {
	 sprintf(buf, "Your current wimp level is %d hit points.\n\r",
	     ch->specials2.wimp_level);
	 send_to_char(buf, ch);
	 return;
      } else {
	 send_to_char("At the moment, you're not a wimp.  (sure, sure...)\n\r", ch);
	 return;
      }
   }

   if (isdigit(*arg)) {
      if ((wimp_lev = atoi(arg))) {
	 if (wimp_lev < 0) {
	    send_to_char("Heh, heh, heh.. we are jolly funny today, eh?\n\r", ch);
	    return;
	 }
	 if (wimp_lev > GET_MAX_HIT(ch)) {
	    send_to_char("That doesn't make much sense, now does it?\n\r", ch);
	    return;
	 }
	 sprintf(buf, "OK, you'll wimp out if you drop below %d hit points.\n\r",
	     wimp_lev);
	 send_to_char(buf, ch);
	 WIMP_LEVEL(ch) = wimp_lev;
      } else {
	 send_to_char("OK, you'll now tough out fights to the bitter end.\n\r", ch);
	 WIMP_LEVEL(ch) = 0;
      }
   } else
      send_to_char("Specify at how many hit points you want to wimp out at.  (0 to disable)\n\r", ch);

   return;

}


ACMD(do_display)
{
   int	i;

   if (IS_NPC(ch)) {
      send_to_char("Mosters don't need displays.  Go away.\n\r", ch);
      return;
   }

   for (i = 0; argument[i] == ' '; i++) 
      ;

   if (!*(argument + i)) {
      send_to_char("Usage: prompt {H | M | V | all | none | auto}\n\r", ch);
      return;
   }

   if (!str_cmp(argument+i, "auto")) {
      REMOVE_BIT(PRF_FLAGS(ch), PRF_DISPHP | PRF_DISPMANA | PRF_DISPMOVE);
      SET_BIT(PRF_FLAGS(ch), PRF_DISPAUTO);
   } else if ((!str_cmp(argument + i, "on")) || (!str_cmp(argument + i, "all")))
      SET_BIT(PRF_FLAGS(ch), PRF_DISPHP | PRF_DISPMANA | PRF_DISPMOVE);
   else {
      REMOVE_BIT(PRF_FLAGS(ch), PRF_DISPHP | PRF_DISPMANA | PRF_DISPMOVE);

      for (; i < strlen(argument); i++) {
	 switch (LOWER(argument[i])) {
	 case 'h': SET_BIT(PRF_FLAGS(ch), PRF_DISPHP); break;
	 case 'm': SET_BIT(PRF_FLAGS(ch), PRF_DISPMANA); break;
	 case 'v': SET_BIT(PRF_FLAGS(ch), PRF_DISPMOVE); break;
	 }
      }
   }

   send_to_char("Ok.\n\r", ch);
}



ACMD(do_gen_write)
{
   FILE * fl;
   char	*tmp, *filename;
   long	ct;
   char	str[MAX_STRING_LENGTH];

   switch (subcmd) {
   case SCMD_BUG:	filename = BUG_FILE; break;
   case SCMD_TYPO:	filename = TYPO_FILE; break;
   case SCMD_IDEA:	filename = IDEA_FILE; break;
   default: return;
   }

   ct  = time(0);
   tmp = asctime(localtime(&ct));

   if (IS_NPC(ch)) {
      send_to_char("Monsters can't have ideas - Go away.\n\r", ch);
      return;
   }

   for (; isspace(*argument); argument++)
      ;
   if (!*argument) {
      send_to_char("That must be a mistake...\n\r", ch);
      return;
   }

   if (!(fl = fopen(filename, "a"))) {
      perror ("do_gen_write");
      send_to_char("Could not open the file.  Sorry.\n\r", ch);
      return;
   }
   sprintf(str, "%-8s (%6.6s) [%5d] %s\n", GET_NAME(ch), (tmp + 4),
       world[ch->in_room].number, argument);
   fputs(str, fl);
   fclose(fl);
   send_to_char("Ok.  Thanks.  :)\n\r", ch);
}


static char	*ctypes[] = {
   "off", "sparse", "normal", "complete", "\n" };

ACMD(do_color)
{
   int	tp;

   if (IS_NPC(ch))
      return;

   one_argument (argument, arg);

   if (!*arg) {
      sprintf(buf, "Your current color level is %s.\n\r", ctypes[COLOR_LEV(ch)]);
      send_to_char(buf, ch);
      return;
   }

   if (((tp = search_block(arg, ctypes, FALSE)) == -1)) {
      send_to_char ("Usage: color { Off | Sparse | Normal | Complete }\n\r", ch);
      return;
   }

   REMOVE_BIT(PRF_FLAGS(ch), PRF_COLOR_1 | PRF_COLOR_2);
   SET_BIT(PRF_FLAGS(ch), (PRF_COLOR_1 * (tp & 1)) | (PRF_COLOR_2 * (tp & 2) >> 1));

   sprintf (buf, "Your %scolor%s is now %s.\n\r", CCRED(ch, C_SPR),
       CCNRM(ch, C_OFF), ctypes[tp]);
   send_to_char(buf, ch);
}


static char	*logtypes[] = {
   "off", "brief", "normal", "complete", "\n" };

ACMD(do_syslog)
{
   int	tp;

   if (IS_NPC(ch))
      return;

   one_argument (argument, arg);

   if (!*arg) {
      tp = ((PRF_FLAGGED(ch, PRF_LOG1) ? 1 : 0) +
	    (PRF_FLAGGED(ch, PRF_LOG2) ? 2 : 0));
      sprintf(buf, "Your syslog is currently %s.\n\r", logtypes[tp]);
      send_to_char(buf, ch);
      return;
   }

   if (((tp = search_block(arg, logtypes, FALSE)) == -1)) {
      send_to_char ("Usage: syslog { Off | Brief | Normal | Complete }\n\r", ch);
      return;
   }

   REMOVE_BIT(PRF_FLAGS(ch), PRF_LOG1 | PRF_LOG2);
   SET_BIT(PRF_FLAGS(ch), (PRF_LOG1 * (tp & 1)) | (PRF_LOG2 * (tp & 2) >> 1));

   sprintf(buf, "Your syslog is now %s.\n\r", logtypes[tp]);
   send_to_char(buf, ch);
}


#define TOG_OFF 0
#define TOG_ON  1

#define PRF_TOG_CHK(ch,flag) ((TOGGLE_BIT(PRF_FLAGS(ch), (flag))) & (flag))

ACMD(do_gen_tog)
{
   long	result;
   extern int	nameserver_is_slow;

   char	*tog_messages[][2] = {
      { "You are now safe from summoning by other players.\n\r",
      "You may now be summoned by other players.\n\r" },
      { "Nohassle disabled.\n\r", 
      "Nohassle enabled.\n\r" },
      { "Brief mode off.\n\r", 
      "Brief mode on.\n\r" },
      { "Compact mode off.\n\r", 
      "Compact mode on.\n\r" },
      { "You can now hear tells.\n\r", 
      "You are now deaf to tells.\n\r" },
      { "You can now hear auctions.\n\r", 
      "You are now deaf to auctions.\n\r" },
      { "You can now hear shouts.\n\r", 
      "You are now deaf to shouts.\n\r" },
      { "You can now hear gossip.\n\r", 
      "You are now deaf to gossip.\n\r" },
      { "You can now hear the congratulation messages.\n\r", 
      "You are now deaf to the congratulation messages.\n\r" },
      { "You can now hear the Wiz-channel.\n\r", 
      "You are now deaf to the Wiz-channel.\n\r" },
      { "You are no longer part of the Quest.\n\r",
      "Okay, you are part of the Quest!\n\r" },
      { "You will no longer see the room flags.\n\r", 
      "You will now see the room flags.\n\r" },
      { "You will now have your communication repeated.\n\r",
      "You will no longer have your communication repeated.\n\r" },
      { "HolyLight mode off.\n\r",
      "HolyLight mode on.\n\r" },
      { "Nameserver_is_slow changed to NO; IP addresses will now be resolved.\n\r",
      "Nameserver_is_slow changed to YES; sitenames will no longer be resolved.\n\r" }
   };
   

   if (IS_NPC(ch)) 
      return;

   switch (subcmd) {
   case SCMD_NOSUMMON	: result = PRF_TOG_CHK(ch, PRF_SUMMONABLE); break;
   case SCMD_NOHASSLE	: result = PRF_TOG_CHK(ch, PRF_NOHASSLE); break;
   case SCMD_BRIEF	: result = PRF_TOG_CHK(ch, PRF_BRIEF); break;
   case SCMD_COMPACT	: result = PRF_TOG_CHK(ch, PRF_COMPACT); break;
   case SCMD_NOTELL	: result = PRF_TOG_CHK(ch, PRF_NOTELL); break;
   case SCMD_NOAUCTION	: result = PRF_TOG_CHK(ch, PRF_NOAUCT); break;
   case SCMD_DEAF	: result = PRF_TOG_CHK(ch, PRF_DEAF); break;
   case SCMD_NOGOSSIP	: result = PRF_TOG_CHK(ch, PRF_NOGOSS); break;
   case SCMD_NOGRATZ 	: result = PRF_TOG_CHK(ch, PRF_NOGRATZ); break;
   case SCMD_NOWIZ	: result = PRF_TOG_CHK(ch, PRF_NOWIZ); break;
   case SCMD_QUEST	: result = PRF_TOG_CHK(ch, PRF_QUEST); break;
   case SCMD_ROOMFLAGS	: result = PRF_TOG_CHK(ch, PRF_ROOMFLAGS); break;
   case SCMD_NOREPEAT	: result = PRF_TOG_CHK(ch, PRF_NOREPEAT); break;
   case SCMD_HOLYLIGHT  : result = PRF_TOG_CHK(ch, PRF_HOLYLIGHT); break;
   case SCMD_SLOWNS	: result = (nameserver_is_slow = !nameserver_is_slow); break;
   default :
      log("SYSERR: Unknown subcmd in do_gen_toggle");
      return;
      break;
   }

   if (result)
      send_to_char(tog_messages[subcmd-SCMD_TOG_BASE][TOG_ON], ch);
   else
      send_to_char(tog_messages[subcmd-SCMD_TOG_BASE][TOG_OFF], ch);

   return;
}

