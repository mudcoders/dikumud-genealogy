/* ************************************************************************
*   File: spec_procs.c                                  Part of CircleMUD *
*  Usage: implementation of special procedures for mobiles/objects/rooms  *
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
#include <sys/types.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"


/*   external vars  */
extern struct room_data *world;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct time_info_data time_info;

/* extern functions */
void	add_follower(struct char_data *ch, struct char_data *leader);


struct social_type {
   char	*cmd;
   int	next_line;
};


/* ********************************************************************
*  Special procedures for rooms                                       *
******************************************************************** */

char	*how_good(int percent)
{
   static char	buf[256];

   if (percent == 0)
      strcpy(buf, " (not learned)");
   else if (percent <= 10)
      strcpy(buf, " (awful)");
   else if (percent <= 20)
      strcpy(buf, " (bad)");
   else if (percent <= 40)
      strcpy(buf, " (poor)");
   else if (percent <= 55)
      strcpy(buf, " (average)");
   else if (percent <= 70)
      strcpy(buf, " (fair)");
   else if (percent <= 80)
      strcpy(buf, " (good)");
   else if (percent <= 85)
      strcpy(buf, " (very good)");
   else
      strcpy(buf, " (Superb)");

   return (buf);
}


SPECIAL(guild)
{
   int	number, i, percent;

   extern char	*spells[];
   extern struct spell_info_type spell_info[MAX_SPL_LIST];
   extern struct int_app_type int_app[26];

   static char	*w_skills[] = {
      "kick",  /* No. 50 */
      "bash",
      "rescue",
      "\n"
   };

   static char	*t_skills[] = {
      "sneak",   /* No. 45 */
      "hide",
      "steal",
      "backstab",
      "pick",
      "\n"
   };

   if (IS_NPC(ch) || ((cmd != 164) && (cmd != 170)))
      return(FALSE);

   for (; *arg == ' '; arg++)
      ;

   switch (GET_CLASS(ch)) {
   case CLASS_MAGIC_USER :
      if (!*arg) {
	 sprintf(buf, "You have got %d practice sessions left.\n\r"
	     "You can practice any of these spells:\n\r",
	     SPELLS_TO_LEARN(ch));
	 for (i = 0; *spells[i] != '\n'; i++)
	    if (spell_info[i+1].spell_pointer && 
	        (spell_info[i+1].min_level_magic <= GET_LEVEL(ch))) {
	       sprintf(buf2, "%-20s %s\n\r", spells[i],
	           how_good(GET_SKILL(ch, i + 1)));
	       strcat(buf, buf2);
	    }
	 send_to_char(buf, ch);
	 return(TRUE);
      }

      number = old_search_block(arg, 0, strlen(arg), spells, FALSE);
      if (number == -1) {
	 send_to_char("You do not know of this spell...\n\r", ch);
	 return(TRUE);
      }
      if (GET_LEVEL(ch) < spell_info[number].min_level_magic) {
	 send_to_char("You do not know of this spell...\n\r", ch);
	 return(TRUE);
      }
      if (SPELLS_TO_LEARN(ch) <= 0) {
	 send_to_char("You do not seem to be able to practice now.\n\r", ch);
	 return(TRUE);
      }
      if (GET_SKILL(ch, number) >= 95) {
	 send_to_char("You are already learned in this area.\n\r", ch);
	 return(TRUE);
      }

      send_to_char("You Practice for a while...\n\r", ch);
      SPELLS_TO_LEARN(ch)--;

      percent = GET_SKILL(ch, number) + MAX(25, int_app[GET_INT(ch)].learn);
      SET_SKILL(ch, number, MIN(95, percent));

      if (GET_SKILL(ch, number) >= 95) {
	 send_to_char("You are now learned in this area.\n\r", ch);
	 return(TRUE);
      }

      break;

   case CLASS_THIEF:
      if (!*arg) {
	 sprintf(buf, "You have got %d practice sessions left.\n\r"
	     "You can practice any of these skills:\n\r",
	     SPELLS_TO_LEARN(ch));
	 for (i = 0; *t_skills[i] != '\n'; i++) {
	    sprintf(buf2, "%-20s %s\n\r", t_skills[i],
	        how_good(GET_SKILL(ch, i + SKILL_SNEAK)));
	    strcat(buf, buf2);
	 }
	 send_to_char(buf, ch);
	 return(TRUE);
      }

      number = search_block(arg, t_skills, FALSE);
      if (number == -1) {
	 send_to_char("You do not know of this skill...\n\r", ch);
	 return(TRUE);
      }
      if (SPELLS_TO_LEARN(ch) <= 0) {
	 send_to_char("You do not seem to be able to practice now.\n\r", ch);
	 return(TRUE);
      }
      if (GET_SKILL(ch, number + SKILL_SNEAK) >= 85) {
	 send_to_char("You are already learned in this area.\n\r", ch);
	 return(TRUE);
      }
      send_to_char("You Practice for a while...\n\r", ch);
      SPELLS_TO_LEARN(ch)--;

      percent = GET_SKILL(ch, number + SKILL_SNEAK) + MIN(int_app[GET_INT(ch)].learn, 12);
      SET_SKILL(ch, number + SKILL_SNEAK, MIN(85, percent));

      if (GET_SKILL(ch, number + SKILL_SNEAK) >= 85) {
	 send_to_char("You are now learned in this area.\n\r", ch);
	 return(TRUE);
      }

      break;

   case CLASS_CLERIC     :
      if (!*arg) {
	 sprintf(buf, "You have got %d practice sessions left.\n\r"
	     "You can practice any of these spells:\n\r",
	     SPELLS_TO_LEARN(ch));
	 for (i = 0; *spells[i] != '\n'; i++)
	    if (spell_info[i+1].spell_pointer && 
	        (spell_info[i+1].min_level_cleric <= GET_LEVEL(ch))) {
	       sprintf(buf2, "%-20s %s\n\r", spells[i],
	           how_good(GET_SKILL(ch, i + 1)));
	       strcat(buf, buf2);
	    }
	 send_to_char(buf, ch);
	 return(TRUE);
      }
      number = old_search_block(arg, 0, strlen(arg), spells, FALSE);
      if (number == -1) {
	 send_to_char("You do not know of this spell...\n\r", ch);
	 return(TRUE);
      }
      if (GET_LEVEL(ch) < spell_info[number].min_level_cleric) {
	 send_to_char("You do not know of this spell...\n\r", ch);
	 return(TRUE);
      }
      if (SPELLS_TO_LEARN(ch) <= 0) {
	 send_to_char("You do not seem to be able to practice now.\n\r", ch);
	 return(TRUE);
      }
      if (GET_SKILL(ch, number) >= 95) {
	 send_to_char("You are already learned in this area.\n\r", ch);
	 return(TRUE);
      }
      send_to_char("You Practice for a while...\n\r", ch);
      SPELLS_TO_LEARN(ch)--;

      percent = GET_SKILL(ch, number) + MAX(25, int_app[GET_INT(ch)].learn);
      SET_SKILL(ch, number, MIN(95, percent));

      if (GET_SKILL(ch, number) >= 95) {
	 send_to_char("You are now learned in this area.\n\r", ch);
	 return(TRUE);
      }

      break;

   case CLASS_WARRIOR:
      if (!*arg) {
	 sprintf(buf, "You have got %d practice sessions left.\n\r"
	     "You can practice any of these skills:\n\r",
	     SPELLS_TO_LEARN(ch));
	 for (i = 0; *w_skills[i] != '\n'; i++) {
	    sprintf(buf2, "%-20s %s\n\r", w_skills[i],
	        how_good(GET_SKILL(ch, i + SKILL_KICK)));
	    strcat(buf, buf2);
	 }
	 send_to_char(buf, ch);
	 return(TRUE);
      }
      number = search_block(arg, w_skills, FALSE);
      if (number == -1) {
	 send_to_char("You do not have ability to practice this skill!\n\r", ch);
	 return(TRUE);
      }
      if (SPELLS_TO_LEARN(ch) <= 0) {
	 send_to_char("You do not seem to be able to practice now.\n\r", ch);
	 return(TRUE);
      }
      if (GET_SKILL(ch, number + SKILL_KICK) >= 80) {
	 send_to_char("You are already learned in this area.\n\r", ch);
	 return(TRUE);
      }
      send_to_char("You Practice for a while...\n\r", ch);
      SPELLS_TO_LEARN(ch)--;

      percent = GET_SKILL(ch, number + SKILL_KICK) + MIN(12, int_app[GET_INT(ch)].learn);
      SET_SKILL(ch, number + SKILL_KICK, MIN(80, percent));

      if (GET_SKILL(ch, number + SKILL_KICK) >= 80) {
	 send_to_char("You are now learned in this area.\n\r", ch);
	 return(TRUE);
      }
      break;
   }

   return (TRUE);
}



SPECIAL(dump)
{
   struct obj_data *k;
   struct char_data *tmp_char;
   int	value = 0;

   ACMD(do_drop);
   char	*fname(char *namelist);

   for (k = world[ch->in_room].contents; k ; k = world[ch->in_room].contents) {
      sprintf(buf, "The %s vanishes in a puff of smoke.\n\r" , fname(k->name));
      for (tmp_char = world[ch->in_room].people; tmp_char; 
          tmp_char = tmp_char->next_in_room)
	 if (CAN_SEE_OBJ(tmp_char, k))
	    send_to_char(buf, tmp_char);
      extract_obj(k);
   }

   if (cmd != 60)
      return(FALSE);

   do_drop(ch, arg, cmd, 0);

   value = 0;

   for (k = world[ch->in_room].contents; k ; k = world[ch->in_room].contents) {
      sprintf(buf, "The %s vanishes in a puff of smoke.\n\r", fname(k->name));
      for (tmp_char = world[ch->in_room].people; tmp_char; 
          tmp_char = tmp_char->next_in_room)
	 if (CAN_SEE_OBJ(tmp_char, k))
	    send_to_char(buf, tmp_char);
      value += MAX(1, MIN(50, k->obj_flags.cost / 10));

      extract_obj(k);
   }

   if (value) {
      act("You are awarded for outstanding performance.", FALSE, ch, 0, 0, TO_CHAR);
      act("$n has been awarded for being a good citizen.", TRUE, ch, 0, 0, TO_ROOM);

      if (GET_LEVEL(ch) < 3)
	 gain_exp(ch, value);
      else
	 GET_GOLD(ch) += value;
   }

   return(TRUE);
}


SPECIAL(mayor)
{
   static char	open_path[] = 
   "W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";

   static char	close_path[] = 
   "W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";

   /*
  const struct social_type open_path[] = {
	 {"G",0}
  };

  static void *thingy = 0;
  static int cur_line = 0;

  for (i=0; i < 1; i++) {
    if (*(open_path[cur_line].cmd) == '!') {
      i++;
      exec_social(ch, (open_path[cur_line].cmd)+1,
        open_path[cur_line].next_line, &cur_line, &thingy);
  } else {
      exec_social(ch, open_path[cur_line].cmd,
        open_path[cur_line].next_line, &cur_line, &thingy);
  }
*/
   static char	*path;
   static int	index;
   static bool move = FALSE;

   ACMD(do_move);
   ACMD(do_open);
   ACMD(do_lock);
   ACMD(do_unlock);
   ACMD(do_close);

   if (!move) {
      if (time_info.hours == 6) {
	 move = TRUE;
	 path = open_path;
	 index = 0;
      } else if (time_info.hours == 20) {
	 move = TRUE;
	 path = close_path;
	 index = 0;
      }
   }

   if (cmd || !move || (GET_POS(ch) < POSITION_SLEEPING) || 
       (GET_POS(ch) == POSITION_FIGHTING))
      return FALSE;

   switch (path[index]) {
   case '0' :
   case '1' :
   case '2' :
   case '3' :
      do_move(ch, "", path[index] - '0' + 1, 0);
      break;

   case 'W' :
      GET_POS(ch) = POSITION_STANDING;
      act("$n awakens and groans loudly.", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'S' :
      GET_POS(ch) = POSITION_SLEEPING;
      act("$n lies down and instantly falls asleep.", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'a' :
      act("$n says 'Hello Honey!'", FALSE, ch, 0, 0, TO_ROOM);
      act("$n smirks.", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'b' :
      act("$n says 'What a view!  I must get something done about that dump!'",
          FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'c' :
      act("$n says 'Vandals!  Youngsters nowadays have no respect for anything!'",
          FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'd' :
      act("$n says 'Good day, citizens!'", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'e' :
      act("$n says 'I hereby declare the bazaar open!'", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'E' :
      act("$n says 'I hereby declare Midgaard closed!'", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'O' :
      do_unlock(ch, "gate", 0, 0);
      do_open(ch, "gate", 0, 0);
      break;

   case 'C' :
      do_close(ch, "gate", 0, 0);
      do_lock(ch, "gate", 0, 0);
      break;

   case '.' :
      move = FALSE;
      break;

   }

   index++;
   return FALSE;
}


/* ********************************************************************
*  General special procedures for mobiles                             *
******************************************************************** */

/* SOCIAL GENERAL PROCEDURES

If first letter of the command is '!' this will mean that the following
command will be executed immediately.

"G",n      : Sets next line to n
"g",n      : Sets next line relative to n, fx. line+=n
"m<dir>",n : move to <dir>, <dir> is 0,1,2,3,4 or 5
"w",n      : Wake up and set standing (if possible)
"c<txt>",n : Look for a person named <txt> in the room
"o<txt>",n : Look for an object named <txt> in the room
"r<int>",n : Test if the npc in room number <int>?
"s",n      : Go to sleep, return false if can't go sleep
"e<txt>",n : echo <txt> to the room, can use $o/$p/$N depending on
             contents of the **thing
"E<txt>",n : Send <txt> to person pointed to by thing
"B<txt>",n : Send <txt> to room, except to thing
"?<num>",n : <num> in [1..99]. A random chance of <num>% success rate.
             Will as usual advance one line upon sucess, and change
             relative n lines upon failure.
"O<txt>",n : Open <txt> if in sight.
"C<txt>",n : Close <txt> if in sight.
"L<txt>",n : Lock <txt> if in sight.
"U<txt>",n : Unlock <txt> if in sight.    */

/* Execute a social command.                                        */
void	exec_social(struct char_data *npc, char *cmd, int next_line,
int *cur_line, void **thing)
{
   bool ok;

   ACMD(do_move);
   ACMD(do_open);
   ACMD(do_lock);
   ACMD(do_unlock);
   ACMD(do_close);

   if (GET_POS(npc) == POSITION_FIGHTING)
      return;

   ok = TRUE;

   switch (*cmd) {

   case 'G' :
      *cur_line = next_line;
      return;

   case 'g' :
      *cur_line += next_line;
      return;

   case 'e' :
      act(cmd + 1, FALSE, npc, *thing, *thing, TO_ROOM);
      break;

   case 'E' :
      act(cmd + 1, FALSE, npc, 0, *thing, TO_VICT);
      break;

   case 'B' :
      act(cmd + 1, FALSE, npc, 0, *thing, TO_NOTVICT);
      break;

   case 'm' :
      do_move(npc, "", *(cmd + 1) - '0' + 1, 0);
      break;

   case 'w' :
      if (GET_POS(npc) != POSITION_SLEEPING)
	 ok = FALSE;
      else
	 GET_POS(npc) = POSITION_STANDING;
      break;

   case 's' :
      if (GET_POS(npc) <= POSITION_SLEEPING)
	 ok = FALSE;
      else
	 GET_POS(npc) = POSITION_SLEEPING;
      break;

   case 'c' :  /* Find char in room */
      *thing = get_char_room_vis(npc, cmd + 1);
      ok = (*thing != 0);
      break;

   case 'o' : /* Find object in room */
      *thing = get_obj_in_list_vis(npc, cmd + 1, world[npc->in_room].contents);
      ok = (*thing != 0);
      break;

   case 'r' : /* Test if in a certain room */
      ok = (npc->in_room == atoi(cmd + 1));
      break;

   case 'O' : /* Open something */
      do_open(npc, cmd + 1, 0, 0);
      break;

   case 'C' : /* Close something */
      do_close(npc, cmd + 1, 0, 0);
      break;

   case 'L' : /* Lock something  */
      do_lock(npc, cmd + 1, 0, 0);
      break;

   case 'U' : /* UnLock something  */
      do_unlock(npc, cmd + 1, 0, 0);
      break;

   case '?' : /* Test a random number */
      if (atoi(cmd + 1) <= number(1, 100))
	 ok = FALSE;
      break;

   default:
      break;
   }  /* End Switch */

   if (ok)
      (*cur_line)++;
   else
      (*cur_line) += next_line;
}



void	npc_steal(struct char_data *ch, struct char_data *victim)
{
   int	gold;

   if (IS_NPC(victim))
      return;
   if (GET_LEVEL(victim) >= LEVEL_IMMORT)
      return;

   if (AWAKE(victim) && (number(0, GET_LEVEL(ch)) == 0)) {
      act("You discover that $n has $s hands in your wallet.", FALSE, ch, 0, victim, TO_VICT);
      act("$n tries to steal gold from $N.", TRUE, ch, 0, victim, TO_NOTVICT);
   } else {
      /* Steal some gold coins */
      gold = (int) ((GET_GOLD(victim) * number(1, 10)) / 100);
      if (gold > 0) {
	 GET_GOLD(ch) += gold;
	 GET_GOLD(victim) -= gold;
      }
   }
}


SPECIAL(snake)
{
   if (cmd)
      return FALSE;

   if (GET_POS(ch) != POSITION_FIGHTING)
      return FALSE;

   if (ch->specials.fighting && 
       (ch->specials.fighting->in_room == ch->in_room) && 
       (number(0, 42 - GET_LEVEL(ch)) == 0)) {
      act("$n bites $N!", 1, ch, 0, ch->specials.fighting, TO_NOTVICT);
      act("$n bites you!", 1, ch, 0, ch->specials.fighting, TO_VICT);
      cast_poison( GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL,
          ch->specials.fighting, 0);
      return TRUE;
   }
   return FALSE;
}


SPECIAL(thief)
{
   struct char_data *cons;

   if (cmd)
      return FALSE;

   if (GET_POS(ch) != POSITION_STANDING)
      return FALSE;

   for (cons = world[ch->in_room].people; cons; cons = cons->next_in_room )
      if ((!IS_NPC(cons)) && (GET_LEVEL(cons) < 21) && (number(1, 5) == 1))
	 npc_steal(ch, cons);

   return TRUE;
}


SPECIAL(magic_user)
{
   struct char_data *vict;

   if (cmd)
      return FALSE;

   if (GET_POS(ch) != POSITION_FIGHTING)
      return FALSE;

   if (!ch->specials.fighting)
      return FALSE;


   /* Find a dude to to evil things upon ! */

   /*	for (vict = world[ch->in_room].people; vict; vict = vict->next_in_room )
		if (vict->specials.fighting==ch && (number(0,4)==0))
			break;
  */

   vict = ch->specials.fighting;

   if (!vict)
      return FALSE;


   if ( (vict != ch->specials.fighting) && (GET_LEVEL(ch) > 13) && (number(0, 7) == 0) ) {
      act("$n utters the words 'dilan oso'.", 1, ch, 0, 0, TO_ROOM);
      cast_sleep(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
      return TRUE;
   }

   if ( (GET_LEVEL(ch) > 7) && (number(0, 5) == 0) ) {
      act("$n utters the words 'koholian dia'.", 1, ch, 0, 0, TO_ROOM);
      cast_blindness(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
      return TRUE;
   }

   if ( (GET_LEVEL(ch) > 12) && (number(0, 8) == 0) && IS_EVIL(ch)) {
      act("$n utters the words 'ib er dranker'.", 1, ch, 0, 0, TO_ROOM);
      cast_energy_drain(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
      return TRUE;
   }

   switch (GET_LEVEL(ch)) {
   case 1:
   case 2:
   case 3:
      break;
   case 4:
   case 5:
      act("$n utters the words 'hahili duvini'!", 1, ch, 0, 0, TO_ROOM);
      cast_magic_missile(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
      break;
   case 6:
   case 7:
      act("$n utters the words 'fridra enton'!", 1, ch, 0, 0, TO_ROOM);
      cast_chill_touch(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
      break;
   case 8:
   case 9:
      act("$n utters the words 'grynt oef'!", 1, ch, 0, 0, TO_ROOM);
      cast_burning_hands(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
      break;
   case 10:
   case 11:
      act("$n utters the words 'juier vrahe'!", 1, ch, 0, 0, TO_ROOM);
      cast_shocking_grasp(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
      break;
   case 12:
   case 13:
      act("$n utters the words 'sjulk divi'!", 1, ch, 0, 0, TO_ROOM);
      cast_lightning_bolt(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
      break;
   case 14:
   case 15:
   case 16:
   case 17:
      act("$n utters the words 'nasson hof'!", 1, ch, 0, 0, TO_ROOM);
      cast_colour_spray(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
      break;
   default:
      act("$n utters the words 'tuborg luca'!", 1, ch, 0, 0, TO_ROOM);
      cast_fireball(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
      break;
   }
   return TRUE;

}


/* ********************************************************************
*  Special procedures for mobiles                                      *
******************************************************************** */

SPECIAL(guild_guard)
{
   char	buf[256], buf2[256];

   if (cmd > 6 || cmd < 1)
      return FALSE;

   strcpy(buf,  "The guard humiliates you, and blocks your way.\n\r");
   strcpy(buf2, "The guard humiliates $n, and blocks $s way.");

   if ( ((ch->in_room == real_room(3017)) && (cmd == 3)) || 
       ((ch->in_room == real_room(21075)) && (cmd == 1)) ) {
      if (GET_CLASS(ch) != CLASS_MAGIC_USER) {
	 act(buf2, FALSE, ch, 0, 0, TO_ROOM);
	 send_to_char(buf, ch);
	 return TRUE;
      }
   } else if ( ((ch->in_room == real_room(3004)) && (cmd == 1)) || 
       ((ch->in_room == real_room(21019)) && (cmd == 4)) ) {
      if (GET_CLASS(ch) != CLASS_CLERIC) {
	 act(buf2, FALSE, ch, 0, 0, TO_ROOM);
	 send_to_char(buf, ch);
	 return TRUE;
      }
   } else if ( ((ch->in_room == real_room(3027)) && (cmd == 2)) || 
       ((ch->in_room == real_room(21014)) && (cmd == 3)) ) {
      if (GET_CLASS(ch) != CLASS_THIEF) {
	 act(buf2, FALSE, ch, 0, 0, TO_ROOM);
	 send_to_char(buf, ch);
	 return TRUE;
      }
   } else if ( ((ch->in_room == real_room(3021)) && (cmd == 2)) || 
       ((ch->in_room == real_room(21023)) && (cmd == 3)) ) {
      if (GET_CLASS(ch) != CLASS_WARRIOR) {
	 act(buf2, FALSE, ch, 0, 0, TO_ROOM);
	 send_to_char(buf, ch);
	 return TRUE;
      }
   }
   return FALSE;
}


SPECIAL(brass_dragon)
{
   char	buf[256], buf2[256];

   if (cmd > 6 || cmd < 1)
      return FALSE;

   strcpy(buf,  "The brass dragon humiliates you, and blocks your way.\n\r");
   strcpy(buf2, "The brass dragon humiliates $n, and blocks $s way.");

   if ((ch->in_room == real_room(5065)) && (cmd == 4)) {
      act(buf2, FALSE, ch, 0, 0, TO_ROOM);
      send_to_char(buf, ch);
      return TRUE;
   }

   return FALSE;

}


SPECIAL(puff)
{
   ACMD(do_say);

   if (cmd)
      return(0);

   switch (number(0, 60)) {
   case 0:
      do_say(ch, "My god!  It's full of stars!", 0, 0);
      return(1);
   case 1:
      do_say(ch, "How'd all those fish get up here?", 0, 0);
      return(1);
   case 2:
      do_say(ch, "I'm a very female dragon.", 0, 0);
      return(1);
   case 3:
      do_say(ch, "I've got a peaceful, easy feeling.", 0, 0);
      return(1);
   default:
      return(0);
   }
}



SPECIAL(fido)
{

   struct obj_data *i, *temp, *next_obj;

   if (cmd || !AWAKE(ch))
      return(FALSE);

   for (i = world[ch->in_room].contents; i; i = i->next_content) {
      if (GET_ITEM_TYPE(i) == ITEM_CONTAINER && i->obj_flags.value[3]) {
	 act("$n savagely devours a corpse.", FALSE, ch, 0, 0, TO_ROOM);
	 for (temp = i->contains; temp; temp = next_obj) {
	    next_obj = temp->next_content;
	    obj_from_obj(temp);
	    obj_to_room(temp, ch->in_room);
	 }
	 extract_obj(i);
	 return(TRUE);
      }
   }
   return(FALSE);
}



SPECIAL(janitor)
{
   struct obj_data *i;

   if (cmd || !AWAKE(ch))
      return(FALSE);

   for (i = world[ch->in_room].contents; i; i = i->next_content) {
      if (IS_SET(i->obj_flags.wear_flags, ITEM_TAKE) && 
          ((i->obj_flags.type_flag == ITEM_DRINKCON) || 
          (i->obj_flags.cost <= 10))) {
	 act("$n picks up some trash.", FALSE, ch, 0, 0, TO_ROOM);

	 obj_from_room(i);
	 obj_to_char(i, ch);
	 return(TRUE);
      }
   }
   return(FALSE);
}


SPECIAL(cityguard)
{
   struct char_data *tch, *evil;
   int	max_evil;

   if (cmd || !AWAKE(ch) || (GET_POS(ch) == POSITION_FIGHTING))
      return (FALSE);

   max_evil = 1000;
   evil = 0;

   for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
      if (!IS_NPC(tch) && IS_SET(PLR_FLAGS(tch), PLR_KILLER)) {
	 act("$n screams 'HEY!!!  You're one of those PLAYER KILLERS!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
	 hit(ch, tch, TYPE_UNDEFINED);
	 return(TRUE);
      }
   }

   for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
      if (!IS_NPC(tch) && IS_SET(PLR_FLAGS(tch), PLR_THIEF)) {
	 act("$n screams 'HEY!!!  You're one of those PLAYER THIEVES!!!!!!'", FALSE, ch, 0, 0, TO_ROOM);
	 hit(ch, tch, TYPE_UNDEFINED);
	 return(TRUE);
      }
   }

   for (tch = world[ch->in_room].people; tch; tch = tch->next_in_room) {
      if (tch->specials.fighting) {
	 if ((GET_ALIGNMENT(tch) < max_evil) && 
	     (IS_NPC(tch) || IS_NPC(tch->specials.fighting))) {
	    max_evil = GET_ALIGNMENT(tch);
	    evil = tch;
	 }
      }
   }

   if (evil && (GET_ALIGNMENT(evil->specials.fighting) >= 0)) {
      act("$n screams 'PROTECT THE INNOCENT!  BANZAI!  CHARGE!  ARARARAGGGHH!'", FALSE, ch, 0, 0, TO_ROOM);
      hit(ch, evil, TYPE_UNDEFINED);
      return(TRUE);
   }

   return(FALSE);
}


SPECIAL(pet_shops)
{
   char	buf[MAX_STRING_LENGTH], pet_name[256];
   int	pet_room;
   struct char_data *pet;

   pet_room = ch->in_room + 1;

   if (cmd == 59) { /* List */
      send_to_char("Available pets are:\n\r", ch);
      for (pet = world[pet_room].people; pet; pet = pet->next_in_room) {
	 sprintf(buf, "%8d - %s\n\r", 3 * GET_EXP(pet), pet->player.short_descr);
	 send_to_char(buf, ch);
      }
      return(TRUE);
   } else if (cmd == 56) { /* Buy */

      arg = one_argument(arg, buf);
      arg = one_argument(arg, pet_name);
      /* Pet_Name is for later use when I feel like it */

      if (!(pet = get_char_room(buf, pet_room))) {
	 send_to_char("There is no such pet!\n\r", ch);
	 return(TRUE);
      }

      if (GET_GOLD(ch) < (GET_EXP(pet) * 3)) {
	 send_to_char("You don't have enough gold!\n\r", ch);
	 return(TRUE);
      }

      GET_GOLD(ch) -= GET_EXP(pet) * 3;

      pet = read_mobile(pet->nr, REAL);
      GET_EXP(pet) = 0;
      SET_BIT(pet->specials.affected_by, AFF_CHARM);

      if (*pet_name) {
	 sprintf(buf, "%s %s", pet->player.name, pet_name);
	 /* free(pet->player.name); don't free the prototype! */
	 pet->player.name = str_dup(buf);

	 sprintf(buf, "%sA small sign on a chain around the neck says 'My Name is %s'\n\r",
	     pet->player.description, pet_name);
	 /* free(pet->player.description); don't free the prototype! */
	 pet->player.description = str_dup(buf);
      }

      char_to_room(pet, ch->in_room);
      add_follower(pet, ch);

      /* Be certain that pet's can't get/carry/use/weild/wear items */
      IS_CARRYING_W(pet) = 1000;
      IS_CARRYING_N(pet) = 100;

      send_to_char("May you enjoy your pet.\n\r", ch);
      act("$n buys $N as a pet.", FALSE, ch, 0, pet, TO_ROOM);

      return(TRUE);
   }

   /* All commands except list and buy */
   return(FALSE);
}


/* Idea of the LockSmith is functionally similar to the Pet Shop */
/* The problem here is that each key must somehow be associated  */
/* with a certain player. My idea is that the players name will  */
/* appear as the another Extra description keyword, prefixed     */
/* by the words 'item_for_' and followed by the player name.     */
/* The (keys) must all be stored in a room which is (virtually)  */
/* adjacent to the room of the lock smith.                       */

SPECIAL(pray_for_items)
{
   char	buf[256];
   int	key_room, gold;
   bool found;
   struct obj_data *tmp_obj, *obj;
   struct extra_descr_data *ext;

   if (cmd != 176) /* You must pray to get the stuff */
      return FALSE;

   key_room = 1 + ch->in_room;

   strcpy(buf, "item_for_");
   strcat(buf, GET_NAME(ch));

   gold = 0;
   found = FALSE;

   for (tmp_obj = world[key_room].contents; tmp_obj; tmp_obj = tmp_obj->next_content)
      for (ext = tmp_obj->ex_description; ext; ext = ext->next)
	 if (str_cmp(buf, ext->keyword) == 0) {
	    if (gold == 0) {
	       gold = 1;
	       act("$n kneels and at the altar and chants a prayer to Odin.",
	           FALSE, ch, 0, 0, TO_ROOM);
	       act("You notice a faint light in Odin's eye.",
	           FALSE, ch, 0, 0, TO_CHAR);
	    }
	    obj = read_object(tmp_obj->item_number, REAL);
	    obj_to_room(obj, ch->in_room);
	    act("$p slowly fades into existence.", FALSE, ch, obj, 0, TO_ROOM);
	    act("$p slowly fades into existence.", FALSE, ch, obj, 0, TO_CHAR);
	    gold += obj->obj_flags.cost;
	    found = TRUE;
	 }


   if (found) {
      GET_GOLD(ch) -= gold;
      GET_GOLD(ch) = MAX(0, GET_GOLD(ch));
      return TRUE;
   }

   return FALSE;
}



/* ********************************************************************
*  Special procedures for objects                                     *
******************************************************************** */


SPECIAL(bank)
{
   int	amount;

   switch (cmd) {
   case 233: /* balance */
      if (GET_BANK_GOLD(ch) > 0)
	 sprintf(buf, "Your current balance is %d coins.\n\r",
	     GET_BANK_GOLD(ch));
      else
	 sprintf(buf, "You currently have no money deposited.\n\r");
      send_to_char(buf, ch);
      return(1);
      break;
   case 234: /* deposit */
      if ((amount = atoi(arg)) <= 0) {
	 send_to_char("How much do you want to deposit?\n\r", ch);
	 return(1);
      }
      if (GET_GOLD(ch) < amount) {
	 send_to_char("You don't have that many coins!\n\r", ch);
	 return(1);
      }
      GET_GOLD(ch) -= amount;
      GET_BANK_GOLD(ch) += amount;
      sprintf(buf, "You deposit %d coins.\n\r", amount);
      send_to_char(buf, ch);
      act("$n makes a bank transaction.", TRUE, ch, 0, FALSE, TO_ROOM);
      return(1);
      break;
   case 235: /* withdraw */
      if ((amount = atoi(arg)) <= 0) {
	 send_to_char("How much do you want to withdraw?\n\r", ch);
	 return(1);
      }
      if (GET_BANK_GOLD(ch) < amount) {
	 send_to_char("You don't have that many coins deposited!\n\r", ch);
	 return(1);
      }
      GET_GOLD(ch) += amount;
      GET_BANK_GOLD(ch) -= amount;
      sprintf(buf, "You withdraw %d coins.\n\r", amount);
      send_to_char(buf, ch);
      act("$n makes a bank transaction.", TRUE, ch, 0, FALSE, TO_ROOM);
      return(1);
      break;
   default:
      return(0);
      break;
   }
}


