
/* ************************************************************************
*  file: spec_procs.c , Special module.                   Part of DIKUMUD *
*  Usage: Procedures handling special procedures for object/room/mobile   *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
************************************************************************* */

#include <stdio.h>
#include <strings.h>
#include <ctype.h>

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
extern struct index_data *obj_index;
extern struct time_info_data time_info;
extern struct title_type titles[4][25];

/* extern procedures */

void hit(struct char_data *ch, struct char_data *victim, int type);
void gain_exp(struct char_data *ch, int gain);
char *strdup(char *source);


/* Data declarations */

struct social_type {
  char *cmd;
  int next_line;
};


/* ********************************************************************
*  Special procedures for rooms                                       *
******************************************************************** */

char *how_good(int percent)
{
	static char buf[256];

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

int guild(struct char_data *ch, int cmd, char *arg) {

	char arg1[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int number, i, percent;

	extern char *spells[];
	extern struct spell_info_type spell_info[MAX_SPL_LIST];
	extern struct int_app_type int_app[26];

	static char *w_skills[] = {
		"kick",  /* No. 50 */
		"bash",
		"rescue",
		"\n"
	};

	static char *t_skills[] = {
		"sneak",   /* No. 45 */
		"hide",
		"steal",
		"backstab",
		"pick",
		"\n"
	};

	if ((cmd != 164) && (cmd != 170)) return(FALSE);

	for(; *arg==' '; arg++);

	switch (GET_CLASS(ch)) {
		case CLASS_MAGIC_USER :{
			if (!*arg) {
				sprintf(buf,"You have got %d practice sessions left.\n\r", ch->specials.spells_to_learn);
				send_to_char(buf, ch);
				send_to_char("You can practise any of these spells:\n\r", ch);
				for(i=0; *spells[i] != '\n'; i++)
					if (spell_info[i+1].spell_pointer &&
					    (spell_info[i+1].min_level_magic <= GET_LEVEL(ch))) {
						send_to_char(spells[i], ch);
						send_to_char(how_good(ch->skills[i+1].learned), ch);
						send_to_char("\n\r", ch);
				}
				return(TRUE);
			}

			number = old_search_block(arg,0,strlen(arg),spells,FALSE);
			if(number == -1) {
				send_to_char("You do not know of this spell...\n\r", ch);
				return(TRUE);
			}
			if (GET_LEVEL(ch) < spell_info[number].min_level_magic) {
				send_to_char("You do not know of this spell...\n\r", ch);
				return(TRUE);
			}
			if (ch->specials.spells_to_learn <= 0) {
				send_to_char("You do not seem to be able to practice now.\n\r", ch);
				return(TRUE);
			}
			if (ch->skills[number].learned >= 95) {
				send_to_char("You are already learned in this area.\n\r", ch);
				return(TRUE);
			}

			send_to_char("You Practice for a while...\n\r", ch);
			ch->specials.spells_to_learn--;

			percent = ch->skills[number].learned+MAX(25,int_app[GET_INT(ch)].learn);
			ch->skills[number].learned = MIN(95, percent);

			if (ch->skills[number].learned >= 95) {
				send_to_char("You are now learned in this area.\n\r", ch);
				return(TRUE);
			}

		} break;

		case CLASS_THIEF: {
			if (!*arg) {
				sprintf(buf,"You have got %d practice sessions left.\n\r", ch->specials.spells_to_learn);
				send_to_char(buf, ch);
				send_to_char("You can practise any of these skills:\n\r", ch);
				for(i=0; *t_skills[i] != '\n';i++) {
					send_to_char(t_skills[i], ch);
					send_to_char(how_good(ch->skills[i+45].learned), ch);
					send_to_char("\n\r", ch);
				}
				return(TRUE);
			}
			number = search_block(arg,t_skills,FALSE);
			if(number == -1) {
				send_to_char("You do not know of this spell...\n\r", ch);
				return(TRUE);
			}
			if (ch->specials.spells_to_learn <= 0) {
				send_to_char("You do not seem to be able to practice now.\n\r", ch);
				return(TRUE);
			}
			if (ch->skills[number+SKILL_SNEAK].learned >= 85) {
				send_to_char("You are already learned in this area.\n\r", ch);
				return(TRUE);
			}
			send_to_char("You Practice for a while...\n\r", ch);
			ch->specials.spells_to_learn--;

			percent = ch->skills[number+SKILL_SNEAK].learned +
			          MIN(int_app[GET_INT(ch)].learn, 12);
			ch->skills[number+SKILL_SNEAK].learned = MIN(85, percent);

			if (ch->skills[number+SKILL_SNEAK].learned >= 85) {
				send_to_char("You are now learned in this area.\n\r", ch);
				return(TRUE);
			}

		} break;

		case CLASS_CLERIC     :{
			if (!*arg) {
				sprintf(buf,"You have got %d practice sessions left.\n\r", ch->specials.spells_to_learn);
				send_to_char(buf, ch);
				send_to_char("You can practise any of these spells:\n\r", ch);
				for(i=0; *spells[i] != '\n'; i++)
					if (spell_info[i+1].spell_pointer &&
					   (spell_info[i+1].min_level_cleric <= GET_LEVEL(ch))) {
						send_to_char(spells[i], ch);
						send_to_char(how_good(ch->skills[i+1].learned), ch);
						send_to_char("\n\r", ch);
				}
				return(TRUE);
			}
			number = old_search_block(arg,0,strlen(arg),spells,FALSE);
			if(number == -1) {
				send_to_char("You do not know of this spell...\n\r", ch);
				return(TRUE);
			}
			if (GET_LEVEL(ch) < spell_info[number].min_level_cleric) {
				send_to_char("You do not know of this spell...\n\r", ch);
				return(TRUE);
			}
			if (ch->specials.spells_to_learn <= 0) {
				send_to_char("You do not seem to be able to practice now.\n\r", ch);
				return(TRUE);
			}
			if (ch->skills[number].learned >= 95) {
				send_to_char("You are already learned in this area.\n\r", ch);
				return(TRUE);
			}
			send_to_char("You Practice for a while...\n\r", ch);
			ch->specials.spells_to_learn--;

			percent = ch->skills[number].learned+MAX(25,int_app[GET_INT(ch)].learn);
			ch->skills[number].learned = MIN(95, percent);

			if (ch->skills[number].learned >= 95) {
				send_to_char("You are now learned in this area.\n\r", ch);
				return(TRUE);
			}
		} break;

		case CLASS_WARRIOR: {
			if (!*arg) {
				sprintf(buf,"You have got %d practice sessions left.\n\r", ch->specials.spells_to_learn);
				send_to_char(buf, ch);
				send_to_char("You can practise any of these skills:\n\r", ch);
				for(i=0; *w_skills[i] != '\n';i++) {
					send_to_char(w_skills[i], ch);
					send_to_char(how_good(ch->skills[i+SKILL_KICK].learned), ch);
					send_to_char("\n\r", ch);
				}
				return(TRUE);
			}
			number = search_block(arg, w_skills, FALSE);
			if(number == -1) {
				send_to_char("You do not have ability to practise this skill!\n\r", ch);
				return(TRUE);
			}
			if (ch->specials.spells_to_learn <= 0) {
				send_to_char("You do not seem to be able to practice now.\n\r", ch);
				return(TRUE);
			}
			if (ch->skills[number+SKILL_KICK].learned >= 80) {
				send_to_char("You are already learned in this area.\n\r", ch);
				return(TRUE);
			}
			send_to_char("You Practice for a while...\n\r", ch);
			ch->specials.spells_to_learn--;

			percent = ch->skills[number+SKILL_KICK].learned +
			          MIN(12, int_app[GET_INT(ch)].learn);
			ch->skills[number+SKILL_KICK].learned = MIN(80, percent);

			if (ch->skills[number+SKILL_KICK].learned >= 80) {
				send_to_char("You are now learned in this area.\n\r", ch);
				return(TRUE);
			}
		} break;
	}
}



int dump(struct char_data *ch, int cmd, char *arg) 
{
   struct obj_data *k;
	char buf[100];
   struct char_data *tmp_char;
   int value=0;

	void do_drop(struct char_data *ch, char *argument, int cmd);
	char *fname(char *namelist);

	for(k = world[ch->in_room].contents; k ; k = world[ch->in_room].contents)
	{
		sprintf(buf, "The %s vanish in a puff of smoke.\n\r" ,fname(k->name));
		for(tmp_char = world[ch->in_room].people; tmp_char;
			tmp_char = tmp_char->next_in_room)
			if (CAN_SEE_OBJ(tmp_char, k))
				send_to_char(buf,tmp_char);
		extract_obj(k);
	}

	if(cmd!=60) return(FALSE);

	do_drop(ch, arg, cmd);

	value = 0;

	for(k = world[ch->in_room].contents; k ; k = world[ch->in_room].contents)
	{
		sprintf(buf, "The %s vanishes in a puff of smoke.\n\r",fname(k->name));
		for(tmp_char = world[ch->in_room].people; tmp_char;
			tmp_char = tmp_char->next_in_room)
			if (CAN_SEE_OBJ(tmp_char, k))
				send_to_char(buf,tmp_char);
			value += MAX(1, MIN(50, k->obj_flags.cost/10));

		extract_obj(k);
	}

	if (value) 
	{
		act("You are awarded for outstanding performance.", FALSE, ch, 0, 0, TO_CHAR);
		act("$n has been awarded for being a good citizen.", TRUE, ch, 0,0, TO_ROOM);

		if (GET_LEVEL(ch) < 3)
			gain_exp(ch, value);
		else
			GET_GOLD(ch) += value;
	}
}

int mayor(struct char_data *ch, int cmd, char *arg)
{
  static char open_path[] =
    "W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";

  static char close_path[] =
    "W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";

/*
  const struct social_type open_path[] = {
	 {"G",0}
  };

  static void *thingy = 0;
  static int cur_line = 0;

  for (i=0; i < 1; i++)
  {
    if (*(open_path[cur_line].cmd) == '!') {
      i++;
      exec_social(ch, (open_path[cur_line].cmd)+1,
        open_path[cur_line].next_line, &cur_line, &thingy);
  } else {
      exec_social(ch, open_path[cur_line].cmd,
        open_path[cur_line].next_line, &cur_line, &thingy);
  }
*/
  static char *path;
  static int index;
  static bool move = FALSE;

  void do_move(struct char_data *ch, char *argument, int cmd);
  void do_open(struct char_data *ch, char *argument, int cmd);
  void do_lock(struct char_data *ch, char *argument, int cmd);
  void do_unlock(struct char_data *ch, char *argument, int cmd);
  void do_close(struct char_data *ch, char *argument, int cmd);


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
      do_move(ch,"",path[index]-'0'+1);
      break;

		case 'W' :
			GET_POS(ch) = POSITION_STANDING;
			act("$n awakens and groans loudly.",FALSE,ch,0,0,TO_ROOM);
			break;

		case 'S' :
			GET_POS(ch) = POSITION_SLEEPING;
			act("$n lies down and instantly falls asleep.",FALSE,ch,0,0,TO_ROOM);
			break;

    case 'a' :
      act("$n says 'Hello Honey!'",FALSE,ch,0,0,TO_ROOM);
      act("$n smirks.",FALSE,ch,0,0,TO_ROOM);
      break;

    case 'b' :
      act("$n says 'What a view! I must get something done about that dump!'",
        FALSE,ch,0,0,TO_ROOM);
      break;

    case 'c' :
      act("$n says 'Vandals! Youngsters nowadays have no respect for anything!'",
        FALSE,ch,0,0,TO_ROOM);
      break;

    case 'd' :
      act("$n says 'Good day, citizens!'", FALSE, ch, 0,0,TO_ROOM);
      break;

    case 'e' :
      act("$n says 'I hereby declare the bazaar open!'",FALSE,ch,0,0,TO_ROOM);
      break;

    case 'E' :
      act("$n says 'I hereby declare Midgaard closed!'",FALSE,ch,0,0,TO_ROOM);
      break;

    case 'O' :
      do_unlock(ch, "gate", 0);
      do_open(ch, "gate", 0);
      break;

    case 'C' :
      do_close(ch, "gate", 0);
      do_lock(ch, "gate", 0);
      break;

    case '.' :
      move = FALSE;
      break;

  }

  index++;
  return FALSE;
}

/* ********************************************************************
*  General special procedures for mobiles                                      *
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
void exec_social(struct char_data *npc, char *cmd, int next_line,
                 int *cur_line, void **thing)
{
  bool ok;

  void do_move(struct char_data *ch, char *argument, int cmd);
  void do_open(struct char_data *ch, char *argument, int cmd);
  void do_lock(struct char_data *ch, char *argument, int cmd);
  void do_unlock(struct char_data *ch, char *argument, int cmd);
  void do_close(struct char_data *ch, char *argument, int cmd);

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
      act(cmd+1, FALSE, npc, *thing, *thing, TO_ROOM);
      break;

    case 'E' :
      act(cmd+1, FALSE, npc, 0, *thing, TO_VICT);
      break;

    case 'B' :
      act(cmd+1, FALSE, npc, 0, *thing, TO_NOTVICT);
      break;

    case 'm' :
      do_move(npc, "", *(cmd+1)-'0'+1);
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
      *thing = get_char_room_vis(npc, cmd+1);
      ok = (*thing != 0);
      break;

    case 'o' : /* Find object in room */
      *thing = get_obj_in_list_vis(npc, cmd+1, world[npc->in_room].contents);
      ok = (*thing != 0);
      break;

    case 'r' : /* Test if in a certain room */
      ok = (npc->in_room == atoi(cmd+1));
      break;

    case 'O' : /* Open something */
      do_open(npc, cmd+1, 0);
      break;

    case 'C' : /* Close something */
      do_close(npc, cmd+1, 0);
      break;

    case 'L' : /* Lock something  */
      do_lock(npc, cmd+1, 0);
      break;

    case 'U' : /* UnLock something  */
      do_unlock(npc, cmd+1, 0);
      break;

    case '?' : /* Test a random number */
      if (atoi(cmd+1) <= number(1,100))
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



void npc_steal(struct char_data *ch,struct char_data *victim)
{
	int gold;

	if(IS_NPC(victim)) return;
	if(IS_TRUSTED(victim)) return;

	if (AWAKE(victim) && (number(0,GET_LEVEL(ch)) == 0)) {
		act("You discover that $n has $s hands in your wallet.",FALSE,ch,0,victim,TO_VICT);
		act("$n tries to steal gold from $N.",TRUE, ch, 0, victim, TO_NOTVICT);
	} else {
		/* Steal some gold coins */
		gold = (int) ((GET_GOLD(victim)*number(1,10))/100);
		if (gold > 0) {
			GET_GOLD(ch) += gold;
			GET_GOLD(victim) -= gold;
		}
	}
}


int snake(struct char_data *ch, int cmd, char *arg)
{
	void cast_poison( byte level, struct char_data *ch, char *arg, int type,
	  struct char_data *tar_ch, struct obj_data *tar_obj );

   if(cmd) return FALSE;

	if(GET_POS(ch)!=POSITION_FIGHTING) return FALSE;
	
	if(ch->specials.fighting && 
		(ch->specials.fighting->in_room == ch->in_room) &&
		(number(0,32-GET_LEVEL(ch))==0))
		{
			act("$n bites $N!", 1, ch, 0, ch->specials.fighting, TO_NOTVICT);
			act("$n bites you!", 1, ch, 0, ch->specials.fighting, TO_VICT);
			cast_poison( GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL,
				 ch->specials.fighting, 0);
			return TRUE;
		}
	return FALSE;
}

int thief(struct char_data *ch, int cmd, char *arg)
{
	struct char_data *cons;

   if(cmd) return FALSE;

	if(GET_POS(ch)!=POSITION_STANDING)return FALSE;

	for(cons = world[ch->in_room].people; cons; cons = cons->next_in_room )
		if(!IS_NPC(cons) && !IS_TRUSTED(cons) && (number(1,5)==1))
			npc_steal(ch,cons); 

	return TRUE;
}

int magic_user(struct char_data *ch, int cmd, char *arg)
{
	struct char_data *vict;

	void cast_burning_hands( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_chill_touch( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_colour_spray( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_energy_drain( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_fireball( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_magic_missile( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_blindness( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
	void cast_curse( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
	void cast_sleep( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );

	if(cmd) return FALSE;

	if(GET_POS(ch)!=POSITION_FIGHTING) return FALSE;
	
	if(!ch->specials.fighting) return FALSE;


	/* Find a dude to to evil things upon ! */

	for (vict = world[ch->in_room].people; vict; vict = vict->next_in_room )
		if (vict->specials.fighting==ch)
			break;

	if (!vict)
		return FALSE;


	if( (vict!=ch->specials.fighting) && (GET_LEVEL(ch)>13) && (number(0,7)==0) )
	{
		act("$n utters the words 'dilan oso'.", 1, ch, 0, 0, TO_ROOM);
		cast_sleep(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
		return TRUE;
	}

	if( (GET_LEVEL(ch)>12) && (number(0,6)==0) )
	{
		act("$n utters the words 'gharia miwi'.", 1, ch, 0, 0, TO_ROOM);
		cast_curse(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
		return TRUE;
	}

	if( (GET_LEVEL(ch)>7) && (number(0,5)==0) )
	{
		act("$n utters the words 'koholian dia'.", 1, ch, 0, 0, TO_ROOM);
		cast_blindness(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
		return TRUE;
	}

	if( (GET_LEVEL(ch)>12) && (number(0,8)==0) && IS_EVIL(ch))
	{
		act("$n utters the words 'ib er dranker'.", 1, ch, 0, 0, TO_ROOM);
		cast_energy_drain(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
		return TRUE;
	}

	switch (GET_LEVEL(ch)) {
		case 1:
		case 2:
		case 3:
		case 4:
			act("$n utters the words 'hahili duvini'.", 1, ch, 0, 0, TO_ROOM);
			cast_magic_missile(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
			break;
		case 5:
		case 6:
		case 7:
		case 8:
			act("$n utters the words 'grynt oef'.", 1, ch, 0, 0, TO_ROOM);
			cast_burning_hands(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
			break;
		case 9:
		case 10:
			act("$n utters the words 'sjulk divi'.", 1, ch, 0, 0, TO_ROOM);
			cast_lightning_bolt(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
			break;
		case 11:
		case 12:
		case 13:
		case 14:
			act("$n utters the words 'nasson hof'.", 1, ch, 0, 0, TO_ROOM);
			cast_colour_spray(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
			break;
		default:
			act("$n utters the words 'tuborg'.", 1, ch, 0, 0, TO_ROOM);
			cast_fireball(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
			break;
	}
	return TRUE;

}


/* ********************************************************************
*  Special procedures for mobiles                                      *
******************************************************************** */

int guild_guard(struct char_data *ch, int cmd, char *arg)
{
	char buf[256], buf2[256];

	if (cmd>6 || cmd<1)
		return FALSE;
	if (IS_TRUSTED(ch)){
		return FALSE;
	}

	strcpy(buf,  "The guard humiliates you, and block your way.\n\r");
	strcpy(buf2, "The guard humiliates $n, and blocks $s way.");

	if ((ch->in_room == real_room(3017)) && (cmd == 3)) {
		if ((GET_CLASS(ch) != CLASS_MAGIC_USER)&&
		    (!IS_SET(ch->specials.act,PLR_ISMULTIMU))) {
			act(buf2, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return TRUE;
		}
	} else if ((ch->in_room == real_room(3004)) && (cmd == 1)) {
		if ((GET_CLASS(ch) != CLASS_CLERIC)&&
		    (!IS_SET(ch->specials.act,PLR_ISMULTICL))) {
			act(buf2, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return TRUE;
		}
	} else if ((ch->in_room == real_room(3027)) && (cmd == 2)) {
		if ((GET_CLASS(ch) != CLASS_THIEF)&&
		    (!IS_SET(ch->specials.act,PLR_ISMULTITH))) {
			act(buf2, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return TRUE;
		}
	} else if ((ch->in_room == real_room(3021)) && (cmd == 2)) {
		if ((GET_CLASS(ch) != CLASS_WARRIOR)&&
		    (!IS_SET(ch->specials.act,PLR_ISMULTIWA))) {
			act(buf2, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return TRUE;
		}

	}

	return FALSE;

}

int remove_team(struct char_data *ch, int cmd, char *arg)
{
	char buf[256], buf2[256];

	if (cmd!=4)
		return FALSE;

	strcpy(buf,  "The Arena Gods bid you farewell.\n\r");
	strcpy(buf2, "The Arena Gods bid $n farewell.\n\r");

	if ((ch->in_room == real_room(3751)) && (cmd == 4)) {/*EXIT room*/
		act(buf2, FALSE, ch, 0, 0, TO_ROOM);
		send_to_char(buf, ch);
		if(ch->specials.arena > ARENA_NOTPLAYING){
			ch->specials.arena=ARENA_NOTPLAYING;
			GET_MOVE(ch)=ch->specials.arena_move;
			GET_MANA(ch)=ch->specials.arena_mana;
			GET_HIT(ch)=ch->specials.arena_hits;
		}
		return FALSE;
	}

	return FALSE;

}


int arena_guard(struct char_data *ch, int cmd, char *arg)
{
	char buf[256], buf2[256];

	if ((cmd!=3) && (cmd!=1))
		return FALSE;
	if (IS_NPC(ch)) {
		return FALSE;
	}

	strcpy(buf,  "The Arena Gods prevent you from entering since you're an outlaw.\n\r");
	strcpy(buf2, "The Arena Gods prevent the outlaw, $n, from entering.");

	if ((ch->in_room == real_room(3702)) && (cmd == 1)) {/*BLUE room*/
		if (IS_SET(ch->specials.act,PLR_ISTHIEF)||
		    IS_SET(ch->specials.act,PLR_ISKILLER)) {
			act(buf2, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return TRUE;
		}

		if (ch->specials.arena!=ARENA_BLUE_PLR) {
			act("The Arena Gods bar $n's way since this is the BLUE team's exit\n\r",FALSE,ch,0,0,TO_ROOM);
			send_to_char("You can't enter the ARENA here without the BLUE team color!\n\r",ch);
			return TRUE;
		}

	} else if ((ch->in_room == real_room(3703)) && (cmd == 3)) { /*RED*/
		if (IS_SET(ch->specials.act,PLR_ISTHIEF)||
		    IS_SET(ch->specials.act,PLR_ISKILLER)) {
			act(buf2, FALSE, ch, 0, 0, TO_ROOM);
			send_to_char(buf, ch);
			return TRUE;
		}
		if (ch->specials.arena!=ARENA_RED_PLR) {
			act("The Arena Gods bar $n's way since this is the RED team's entrance.\n\r",FALSE,ch,0,0,TO_ROOM);
			send_to_char("You can't enter the ARENA here without the RED team color!\n\r",ch);
			return TRUE;
		}
	}
	/* Save stats so we can restore them when you leave the Arena */
	ch->specials.arena_move=GET_MOVE(ch);
	ch->specials.arena_mana=GET_MANA(ch);
	ch->specials.arena_hits=GET_HIT(ch);
	return FALSE;

}

int brass_dragon(struct char_data *ch, int cmd, char *arg)
{
	char buf[256], buf2[256];

	if (cmd>6 || cmd<1)
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

int puff(struct char_data *ch, int cmd, char *arg)
{
	void do_say(struct char_data *ch, char *argument, int cmd);

	if (cmd)
		return(0);

	switch (number(0, 60))
	{
		case 0:
			do_say(ch, "My god! It's full of stars!", 0);
		   return(1);
		case 1:
			do_say(ch, "How'd all those fish get up here?", 0);
			return(1);
		case 2:
			do_say(ch, "I'm a very female dragon.", 0);
			return(1);
		case 3:
			do_say(ch, "I've got a peaceful, easy feeling.", 0);
			return(1);
		default:
			return(0);
	}
}
					
int fido(struct char_data *ch, int cmd, char *arg)
{

	struct obj_data *i, *temp, *next_obj;

	if (cmd || !AWAKE(ch))
		return(FALSE);

	for (i = world[ch->in_room].contents; i; i = i->next_content) {
		if (GET_ITEM_TYPE(i)==ITEM_CONTAINER && i->obj_flags.value[3]) {
			act("$n savagely devours a corpse.", FALSE, ch, 0, 0, TO_ROOM);
			for(temp = i->contains; temp; temp=next_obj)
			{
				next_obj = temp->next_content;
				obj_from_obj(temp);
				obj_to_room(temp,ch->in_room);
			}
			extract_obj(i);
			return(TRUE);
		}
	}
	return(FALSE);
}



int janitor(struct char_data *ch, int cmd, char *arg)
{
	struct obj_data *i, *temp, *next_obj;

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


int cityguard(struct char_data *ch, int cmd, char *arg)
{
	struct char_data *tch, *evil;
	int max_evil;

	if (cmd || !AWAKE(ch) || (GET_POS(ch) == POSITION_FIGHTING))
		return (FALSE);

	max_evil = 1000;
	evil = 0;

	for (tch=world[ch->in_room].people; tch; tch = tch->next_in_room) {
		if (tch->specials.fighting && CAN_SEE(ch,tch)) {
         if ((GET_ALIGNMENT(tch) < max_evil) &&
             (IS_NPC(tch) || IS_NPC(tch->specials.fighting))) {
				max_evil = GET_ALIGNMENT(tch);
				evil = tch;
			}
		}
	}

	if (evil && (GET_ALIGNMENT(evil->specials.fighting) >= 0)) {
		act("$n screams 'PROTECT THE INNOCENT!  BANZAI!!! CHARGE!!! ARARARAGGGHH!'", FALSE, ch, 0, 0, TO_ROOM);
		hit(ch, evil, TYPE_UNDEFINED);
		return(TRUE);
	}

	for(tch=world[ch->in_room].people; tch; tch=tch->next_in_room) {
		if (!IS_NPC(tch) && CAN_SEE(ch, tch) &&
		    (IS_SET(tch->specials.act,PLR_ISKILLER)||
 		     IS_SET(tch->specials.act,PLR_ISTHIEF))){
			act("$n screams 'Outlaw! Fresh blood! Kill!'", FALSE, ch, 0, 0, TO_ROOM);
			hit(ch, tch, TYPE_UNDEFINED);
			return(TRUE);
		}
	}
		
	return(FALSE);
}


int pet_shops(struct char_data *ch, int cmd, char *arg)
{
	char buf[MAX_STRING_LENGTH], pet_name[256];
	int pet_room;
	struct char_data *pet;

	pet_room = ch->in_room+1;

	if (cmd==59) { /* List */
		send_to_char("Available pets are:\n\r", ch);
		for(pet = world[pet_room].people; pet; pet = pet->next_in_room) {
			sprintf(buf, "%8d - %s\n\r", 3*GET_EXP(pet), pet->player.short_descr);
			send_to_char(buf, ch);
		}
		return(TRUE);
	} else if (cmd==56) { /* Buy */

		arg = one_argument(arg, buf);
		arg = one_argument(arg, pet_name);
		/* Pet_Name is for later use when I feel like it */

		if (!(pet = get_char_room(buf, pet_room))) {
			send_to_char("There is no such pet!\n\r", ch);
			return(TRUE);
		}

		if (GET_GOLD(ch) < (GET_EXP(pet)*3)) {
			send_to_char("You don't have enough gold!\n\r", ch);
			return(TRUE);
		}

		GET_GOLD(ch) -= GET_EXP(pet)*3;

		pet = read_mobile(pet->nr, REAL);
		GET_EXP(pet) = 0;
		SET_BIT(pet->specials.affected_by, AFF_CHARM);

		if (*pet_name) {
			sprintf(buf,"%s %s", pet->player.name, pet_name);
			free(pet->player.name);
			pet->player.name = strdup(buf);		

			sprintf(buf,"%sA small sign on a chain around the neck says 'My Name is %s'\n\r",
			  pet->player.description, pet_name);
			free(pet->player.description);
			pet->player.description = strdup(buf);
		}

		char_to_room(pet, ch->in_room);
		add_follower(pet, ch);

		/* Be certain that pet's can't get/carry/use/weild/wear items */
		IS_CARRYING_W(pet) = 1000;
		IS_CARRYING_N(pet) = 100;

		send_to_char("May you enjoy your pet.\n\r", ch);
		act("$n bought $N as a pet.",FALSE,ch,0,pet,TO_ROOM);

		return(TRUE);
	}

	/* All commands except list and buy */
	return(FALSE);
}

#define PER_LEVEL	10	/* coins per level needed to play in arena */

int arena_shop(struct char_data *ch, int cmd, char *arg)
{
	char buf[MAX_STRING_LENGTH];
	int gold_needed;

	gold_needed=GET_LEVEL(ch)*PER_LEVEL;

	if (cmd==59) { /* List */
		send_to_char("Your choice of team colors are:\n\r", ch);
		send_to_char("\n\rRED\n\rBLUE\n\r\n\r",ch);
		send_to_char("You may JOIN RED, or JOIN BLUE\n\r\n\r",ch);
		send_to_char("No outlaws are allowed in the Holy Arena\n\r",ch);
		sprintf(buf,"It will cost you %d coins to join a team.\n\r",
			gold_needed);
		send_to_char(buf,ch);
		return(TRUE);
	} else if (cmd==228) { /* JOIN */

		if(IS_SET(ch->specials.act,PLR_ISTHIEF)||
		   IS_SET(ch->specials.act,PLR_ISKILLER)){
			send_to_char("The Arena Gods forbid outlaws from joining!\n\r",ch);
			return(FALSE);
		}

		arg = one_argument(arg, buf);
		if(!strcmp(buf,"red")&&!strcmp(buf,"blue")) {
			send_to_char("You can only join a RED or BLUE team.\n\r",ch);
			return(TRUE);
		}

		if(GET_GOLD(ch)<gold_needed){
			sprintf(buf,"You need %d coins to join!! Go away\n\r",
				gold_needed);
			send_to_char(buf,ch);
			return(TRUE);
		}

		if (!strcmp(buf,"red")) {
			ch->specials.arena=ARENA_RED_PLR;
			send_to_char("You are now a member of the RED team!\n\r",ch);
			return(TRUE);
		}

		if (!strcmp(buf,"blue")) {
			ch->specials.arena=ARENA_BLUE_PLR;
			send_to_char("You are now a member of the BLUE team!\n\r",ch);
			return(TRUE);
		}

		send_to_char("The Gods decree that you may only join red or blue teams!\n\r",ch);
		return(TRUE);
			
	}

	/* All commands except list and buy */
	return(FALSE);
}

void
new_class(struct char_data *ch, int newclass)
{
	char oldclass[80];
	char sendbuf[80];
	char buf[80];

	switch(GET_CLASS(ch)){
	  case CLASS_THIEF:
		SET_BIT(ch->specials.act,PLR_ISMULTITH);
		strcpy(oldclass,"Thief"); break;
	  case CLASS_MAGIC_USER:
		SET_BIT(ch->specials.act,PLR_ISMULTIMU);
		strcpy(oldclass,"Magic User"); break;
	  case CLASS_CLERIC:
		SET_BIT(ch->specials.act,PLR_ISMULTICL);
		strcpy(oldclass,"Cleric"); break;
	  case CLASS_WARRIOR:
		SET_BIT(ch->specials.act,PLR_ISMULTIWA);
		strcpy(oldclass,"Warrior"); break;
	  default:
		send_to_char("Invalid class!\n\r",ch);
		return;
	}
	GET_CLASS(ch)=newclass;
	switch(GET_CLASS(ch)){
	  case CLASS_WARRIOR:
		sprintf(buf,"Warrior/%s",oldclass);break;
	  case CLASS_THIEF:
		sprintf(buf,"Thief/%s",oldclass);break;
	  case CLASS_CLERIC:
		sprintf(buf,"Cleric/%s",oldclass);break;
	  case CLASS_MAGIC_USER:
		sprintf(buf,"Magic User/%s",oldclass);break;
	  default:
		send_to_char("Serious error in new class!\n\r",ch);
		return;
	}
	sprintf(sendbuf,"You are now a %s!\n\r",buf);
	send_to_char(sendbuf,ch);
}

#define MULTI_MIN_LEV		5	/* minimum level to change class*/
#define MULTI_COST_PER_LEV	100000	/* 100k per level to change */

int
do_multiclass(struct char_data *ch, int cmd, char *arg)
{

	char buf[300];
	long cost;

	if (cmd!=233){
		return(FALSE);
	}

	if (IS_NPC(ch)) {
		send_to_char("Monsters have no class, go away!\n\r",ch);
		return(TRUE);
	}

	arg = one_argument(arg, buf);
	if(strcmp(buf,"cleric")&&strcmp(buf,"mu")&&
	   strcmp(buf,"thief")&&strcmp(buf,"warrior")) {
		send_to_char("You can only switch to a THIEF, CLERIC, MU, or WARRIOR.\n\r",ch);
		return(TRUE);
	}

	if(IS_SET(ch->specials.act,PLR_ISMULTITH)||
	   IS_SET(ch->specials.act,PLR_ISMULTIWA)||
	   IS_SET(ch->specials.act,PLR_ISMULTIMU)||
	   IS_SET(ch->specials.act,PLR_ISMULTICL)){
		send_to_char("You can't switch more than once (yet).\n\r",ch);
		return(TRUE);
	}

	if (GET_LEVEL(ch)<MULTI_MIN_LEV){
		send_to_char("You need more levels first!\n\r",ch);
		return(TRUE);
	}

	if (GET_LEVEL(ch)>LV_IMMORTAL || IS_TRUSTED(ch)){
		send_to_char("Gods may not multiclass!\n\r",ch);
		return(TRUE);
	}

	cost=GET_LEVEL(ch)*MULTI_COST_PER_LEV;

	if(GET_GOLD(ch)<cost){
		sprintf(buf,"You need %d coins to change class at your level.\n\r",cost);
		send_to_char(buf,ch);
		return(TRUE);
	}

	if(GET_EXP(ch)<titles[GET_CLASS(ch)-1][GET_LEVEL(ch)].exp){
		sprintf(buf,"You need %d experience points to multiclass at your level.\n\r",titles[GET_CLASS(ch)-1][GET_LEVEL(ch)].exp);
		send_to_char(buf,ch);
		return(TRUE);
	}

	if(!strcmp(buf,"cleric")&&(GET_CLASS(ch)!=CLASS_CLERIC)){
		new_class(ch,CLASS_CLERIC);
	} else if (!strcmp(buf,"mu")&&(GET_CLASS(ch)!=CLASS_MAGIC_USER)){
		new_class(ch,CLASS_MAGIC_USER);
	} else if (!strcmp(buf,"thief")&&(GET_CLASS(ch)!=CLASS_THIEF)){
		new_class(ch,CLASS_THIEF);
	} else if (!strcmp(buf,"warrior")&&(GET_CLASS(ch)!=CLASS_WARRIOR)){
		new_class(ch,CLASS_WARRIOR);
	} else {
		send_to_char("You are already that class!\n\r",ch);
		return(TRUE);
	}

	GET_GOLD(ch) -= cost;
	sprintf(buf,"It costs you %d coins to switch class.\n\r",cost);
	send_to_char(buf,ch);

	/* Change class. Set player's level at 1. Zero experience. Give
	necessary stats to new class.*/

	GET_LEVEL(ch) = 1;
	GET_EXP(ch) = 1;
	ch->specials.spells_to_learn = 0;
	set_title(ch);

	switch (GET_CLASS(ch)) {

		case CLASS_THIEF : {
			ch->skills[SKILL_SNEAK].learned = 10;
			ch->skills[SKILL_HIDE].learned =  5;
			ch->skills[SKILL_STEAL].learned = 15;
			ch->skills[SKILL_BACKSTAB].learned = 10;
			ch->skills[SKILL_PICK_LOCK].learned = 10;
		} break;

	}
	send_to_char("You feel awesome power surge coarse through your veins.\n\r",ch);
	send_to_char("You feel like a new person!\n\r",ch);
}

/* Idea of the LockSmith is functionally similar to the Pet Shop */
/* The problem here is that each key must somehow be associated  */
/* with a certain player. My idea is that the players name will  */
/* appear as the another Extra description keyword, prefixed     */
/* by the words 'item_for_' and followed by the player name.     */
/* The (keys) must all be stored in a room which is (virtually)  */
/* adjacent to the room of the lock smith.                       */

int pray_for_items(struct char_data *ch, int cmd, char *arg)
{
  char buf[256];
  int key_room, gold;
  bool found;
  struct obj_data *tmp_obj, *obj;
	struct extra_descr_data *ext;

	if (cmd != 176) /* You must pray to get the stuff */
		return FALSE;

	key_room = 1+ch->in_room;

  strcpy(buf, "item_for_");
  strcat(buf, GET_NAME(ch));

  gold = 0;
  found = FALSE;

  for (tmp_obj = world[key_room].contents; tmp_obj; tmp_obj = tmp_obj->next_content)
    for(ext = tmp_obj->ex_description; ext; ext = ext->next)
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
		  act("$p slowly fades into existence.",FALSE,ch,obj,0,TO_ROOM);
		  act("$p slowly fades into existence.",FALSE,ch,obj,0,TO_CHAR);
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



#define CHAL_ACT \
"You are torn out of reality!\n\r\
You roll and tumble through endless voids for what seems like eternity...\n\r\
\n\r\
After a time, a new reality comes into focus... you are elsewhere.\n\r"


int chalice(struct char_data *ch, int cmd, char *arg)
{
	/* 222 is the normal chalice, 223 is chalice-on-altar */

	struct obj_data *chalice;
	char buf1[MAX_INPUT_LENGTH], buf2[MAX_INPUT_LENGTH];
	static int chl = -1, achl = -1;

	if (chl < 1)
	{
		chl = real_object(222);
		achl = real_object(223);
	}

	switch(cmd)
	{
		case 10:    /* get */
			if (!(chalice = get_obj_in_list_num(chl,
				world[ch->in_room].contents))
				&& CAN_SEE_OBJ(ch, chalice))
				if (!(chalice = get_obj_in_list_num(achl,
					world[ch->in_room].contents)) && CAN_SEE_OBJ(ch, chalice))
					return(0);
	
			/* we found a chalice.. now try to get us */			
			do_get(ch, arg, cmd);
			/* if got the altar one, switch her */
			if (chalice == get_obj_in_list_num(achl, ch->carrying))
			{
				extract_obj(chalice);
				chalice = read_object(chl, VIRTUAL);
				obj_to_char(chalice, ch);
			}
			return(1);
		break;
		case 67: /* put */
			if (!(chalice = get_obj_in_list_num(chl, ch->carrying)))
				return(0);

			argument_interpreter(arg, buf1, buf2);
			if (!str_cmp(buf1, "chalice") && !str_cmp(buf2, "altar"))
			{
				extract_obj(chalice);
				chalice = read_object(achl, VIRTUAL);
				obj_to_room(chalice, ch->in_room);
				send_to_char("Ok.\n\r", ch);
			}
			return(1);
		break;
		case 176: /* pray */
			if (!(chalice = get_obj_in_list_num(achl,
				world[ch->in_room].contents)))
				return(0);

			do_action(ch, arg, cmd);  /* pray */
			send_to_char(CHAL_ACT, ch);
			extract_obj(chalice);
			act("$n is torn out of existence!", TRUE, ch, 0, 0, TO_ROOM);
			char_from_room(ch);
			char_to_room(ch, real_room(2500));   /* before the fiery gates */
			do_look(ch, "", 15);
			return(1);
		break;
		default:
			return(0);
		break;
	}
}




int kings_hall(struct char_data *ch, int cmd, char *arg)
{
	if (cmd != 176)
		return(0);

	do_action(ch, arg, 176);

	send_to_char("You feel as if some mighty force has been offended.\n\r", ch);
	send_to_char(CHAL_ACT, ch);
	act("$n is struck by an intense beam of light and vanishes.",
		TRUE, ch, 0, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, real_room(1420));  /* behind the altar */
	do_look(ch, "", 15);
	return(1);
}

int offensive(struct char_data *ch, int cmd, char *arg)
{
	struct char_data *vict;

	void cast_burning_hands( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_chill_touch( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_colour_spray( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_energy_drain( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_fireball( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_magic_missile( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *victim, struct obj_data *tar_obj );
	void cast_blindness( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
	void cast_curse( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );
	void cast_sleep( byte level, struct char_data *ch, char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj );

	if(cmd) return FALSE;

	if(!AWAKE(ch)) return FALSE;

	/* Find a pc-killer or a thief to do battle with */
	if(GET_POS(ch)!=POSITION_FIGHTING || !ch->specials.fighting) {
		for(vict=world[ch->in_room].people; vict; vict=vict->next_in_room){
			if(!IS_NPC(vict) && CAN_SEE(ch, vict) &&
			   (IS_SET(vict->specials.act, PLR_ISKILLER) ||
			    IS_SET(vict->specials.act, PLR_ISTHIEF))){
				act("$n screams 'Outlaw! Fresh blood! Kill!'", FALSE, ch, 0, 0, TO_ROOM);
				hit(ch, vict, TYPE_UNDEFINED);
				return(TRUE);
			}
		}
		return (FALSE);
	}

	/* We're fighting someone, so
	 * Find a dude to to evil things upon ! */

	for (vict = world[ch->in_room].people; vict; vict = vict->next_in_room )
		if (vict->specials.fighting==ch)
			break;

	if (!vict)
		return FALSE;


	if( (vict!=ch->specials.fighting) && (GET_LEVEL(ch)>13) && (number(0,4)==0) )
	{
		act("$n utters the words 'dilan oso'.", 1, ch, 0, 0, TO_ROOM);
		cast_sleep(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
		return TRUE;
	}

	if( (GET_LEVEL(ch)>12) && (number(0,4)==0) )
	{
		act("$n utters the words 'gharia miwi'.", 1, ch, 0, 0, TO_ROOM);
		cast_curse(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
		return TRUE;
	}

	if( (GET_LEVEL(ch)>7) && (number(0,3)==0) )
	{
		act("$n utters the words 'k4holian dia'.", 1, ch, 0, 0, TO_ROOM);
		cast_blindness(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
		return TRUE;
	}

	if( (GET_LEVEL(ch)>12) && (number(0,5)==0) && IS_EVIL(ch))
	{
		act("$n utters the words 'ib er dranker'.", 1, ch, 0, 0, TO_ROOM);
		cast_energy_drain(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
		return TRUE;
	}

	switch (number(0,16)) {
		case 0: break; /* do nothing */
		case 1:
		case 2:
		case 3:
		case 4:
			act("$n utters the words 'hahili duvini'.", 1, ch, 0, 0, TO_ROOM);
			cast_magic_missile(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
			break;
		case 5:
		case 6:
		case 7:
		case 8:
			act("$n utters the words 'grynt oef'.", 1, ch, 0, 0, TO_ROOM);
			cast_burning_hands(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
			break;
		case 9:
		case 10:
			act("$n utters the words 'sjulk divi'.", 1, ch, 0, 0, TO_ROOM);
			cast_lightning_bolt(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
			break;
		case 11:
		case 12:
		case 13:
		case 14:
			act("$n utters the words 'nasson hof'.", 1, ch, 0, 0, TO_ROOM);
			cast_colour_spray(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
			break;
		default:
			act("$n utters the words 'tuborg'.", 1, ch, 0, 0, TO_ROOM);
			cast_fireball(GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, vict, 0);
			break;
	}
	return TRUE;

}

int jester(struct char_data *ch, int cmd, char *arg)
{
	void do_say(struct char_data *ch, char *argument, int cmd);

	if (cmd)
		return(0);

	switch (number(0, 60))
	{
		case 0:
			do_say(ch, "You are a real stinker!", 0);
		   return(1);
		case 1:
			do_say(ch, "Have you considered getting a lobotomy?", 0);
			return(1);
		case 2:
			do_say(ch, "You're as stupid as you look!", 0);
			return(1);
		case 3:
			do_say(ch, "Get a real hair-cut!", 0);
			return(1);
		default:
			return(0);
	}
}
