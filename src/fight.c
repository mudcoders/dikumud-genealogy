/* ************************************************************************
*   File: fight.c                                       Part of CircleMUD *
*  Usage: Combat system                                                   *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "db.h"
#include "spells.h"
#include "limits.h"
#include "screen.h"

/* Structures */

struct char_data *combat_list = 0;	    /* head of l-list of fighting chars	*/
struct char_data *combat_next_dude = 0; /* Next dude global trick           */


/* External structures */

extern struct room_data *world;
extern struct message_list fight_messages[MAX_MESSAGES];
extern struct obj_data *object_list;
extern int	pk_allowed;	/* see config.c */
extern int	auto_save;	/* see config.c */

/* External procedures */

char	*fread_string(FILE *fl, char *error);
void	stop_follower(struct char_data *ch);
ACMD(do_flee);
void	hit(struct char_data *ch, struct char_data *victim, int type);
void	forget(struct char_data *ch, struct char_data *victim);
void	remember(struct char_data *ch, struct char_data *victim);

/* Weapon attack texts */
struct attack_hit_type attack_hit_text[] = 
{
   { "hit",   "hits" },
   { "pound", "pounds" },
   { "pierce", "pierces" },
   { "slash", "slashes" },
   { "blast", "blasts" },
   { "whip", "whips" },
   { "pierce", "pierces" },
   { "claw", "claws" },
   { "bite", "bites" },
   { "sting", "stings" },
   { "crush", "crushes" }           /* TYPE_CRUSH    */
};



/* The Fight related routines */

void	appear(struct char_data *ch)
{
   act("$n slowly fades into existence.", FALSE, ch, 0, 0, TO_ROOM);

   if (affected_by_spell(ch, SPELL_INVISIBLE))
      affect_from_char(ch, SPELL_INVISIBLE);

   REMOVE_BIT(ch->specials.affected_by, AFF_INVISIBLE);
}



void	load_messages(void)
{
   FILE * f1;
   int	i, type;
   struct message_type *messages;
   char	chk[100];

   if (!(f1 = fopen(MESS_FILE, "r"))) {
      sprintf(buf2, "Error reading combat message file %s", MESS_FILE);
      perror(buf2);
      exit(0);
   }

   for (i = 0; i < MAX_MESSAGES; i++) {
      fight_messages[i].a_type = 0;
      fight_messages[i].number_of_attacks = 0;
      fight_messages[i].msg = 0;
   }

   fscanf(f1, " %s \n", chk);

   while (*chk == 'M') {
      fscanf(f1, " %d\n", &type);
      for (i = 0; (i < MAX_MESSAGES) && (fight_messages[i].a_type != type) && 
          (fight_messages[i].a_type); i++)
	 ;
      if (i >= MAX_MESSAGES) {
	 log("SYSERR: Too many combat messages.");
	 exit(0);
      }

      CREATE(messages, struct message_type, 1);
      fight_messages[i].number_of_attacks++;
      fight_messages[i].a_type = type;
      messages->next = fight_messages[i].msg;
      fight_messages[i].msg = messages;

      sprintf(buf2, "combat message #%d in file '%s'", i, MESS_FILE);

      messages->die_msg.attacker_msg      = fread_string(f1, buf2);
      messages->die_msg.victim_msg        = fread_string(f1, buf2);
      messages->die_msg.room_msg          = fread_string(f1, buf2);
      messages->miss_msg.attacker_msg     = fread_string(f1, buf2);
      messages->miss_msg.victim_msg       = fread_string(f1, buf2);
      messages->miss_msg.room_msg         = fread_string(f1, buf2);
      messages->hit_msg.attacker_msg      = fread_string(f1, buf2);
      messages->hit_msg.victim_msg        = fread_string(f1, buf2);
      messages->hit_msg.room_msg          = fread_string(f1, buf2);
      messages->god_msg.attacker_msg      = fread_string(f1, buf2);
      messages->god_msg.victim_msg        = fread_string(f1, buf2);
      messages->god_msg.room_msg          = fread_string(f1, buf2);
      fscanf(f1, " %s \n", chk);
   }

   fclose(f1);
}


void	update_pos( struct char_data *victim )
{

   if ((GET_HIT(victim) > 0) && (GET_POS(victim) > POSITION_STUNNED))
      return;
   else if (GET_HIT(victim) > 0 )
      GET_POS(victim) = POSITION_STANDING;
   else if (GET_HIT(victim) <= -11)
      GET_POS(victim) = POSITION_DEAD;
   else if (GET_HIT(victim) <= -6)
      GET_POS(victim) = POSITION_MORTALLYW;
   else if (GET_HIT(victim) <= -3)
      GET_POS(victim) = POSITION_INCAP;
   else
      GET_POS(victim) = POSITION_STUNNED;

}


void	check_killer(struct char_data *ch, struct char_data *vict)
{
   if (!PLR_FLAGGED(vict, PLR_KILLER) && !PLR_FLAGGED(vict, PLR_THIEF)
        && !PLR_FLAGGED(ch, PLR_KILLER) && !IS_NPC(ch) && !IS_NPC(vict) && 
       (ch != vict)) {
      char	buf[200];

      SET_BIT(PLR_FLAGS(ch), PLR_KILLER);
      sprintf(buf, "PC Killer bit set on %s for initiating attack on %s at %s.",
          GET_NAME(ch), GET_NAME(vict), world[vict->in_room].name);
      mudlog(buf, BRF, LEVEL_IMMORT, TRUE);
      send_to_char("If you want to be a PLAYER KILLER, so be it...\n\r", ch);
   }
}


/* start one char fighting another (yes, it is horrible, I know... )  */
void	set_fighting(struct char_data *ch, struct char_data *vict)
{
   if (ch == vict)
      return;

   assert(!ch->specials.fighting);

   ch->next_fighting = combat_list;
   combat_list = ch;

   if (IS_AFFECTED(ch, AFF_SLEEP))
      affect_from_char(ch, SPELL_SLEEP);

   ch->specials.fighting = vict;
   GET_POS(ch) = POSITION_FIGHTING;

   if (!pk_allowed)
      check_killer(ch, vict);
}



/* remove a char from the list of fighting chars */
void	stop_fighting(struct char_data *ch)
{
   struct char_data *tmp;

   assert(ch->specials.fighting);

   if (ch == combat_next_dude)
      combat_next_dude = ch->next_fighting;

   if (combat_list == ch)
      combat_list = ch->next_fighting;
   else {
      for (tmp = combat_list; tmp && (tmp->next_fighting != ch); 
          tmp = tmp->next_fighting)
	 ;
      if (!tmp) {
	 log("SYSERR: Char fighting not found Error (fight.c, stop_fighting)");
	 abort();
      }
      tmp->next_fighting = ch->next_fighting;
   }

   ch->next_fighting = 0;
   ch->specials.fighting = 0;
   GET_POS(ch) = POSITION_STANDING;
   update_pos(ch);
}



void	make_corpse(struct char_data *ch)
{
   struct obj_data *corpse, *o;
   struct obj_data *money;
   int	i;
   extern int max_npc_corpse_time, max_pc_corpse_time;

   struct obj_data *create_money(int amount);

   CREATE(corpse, struct obj_data, 1);
   clear_object(corpse);

   corpse->item_number = NOWHERE;
   corpse->in_room = NOWHERE;
   corpse->name = str_dup("corpse");

   sprintf(buf2, "The corpse of %s is lying here.", GET_NAME(ch));
   corpse->description = str_dup(buf2);

   sprintf(buf2, "the corpse of %s", GET_NAME(ch));
   corpse->short_description = str_dup(buf2);

   corpse->contains = ch->carrying;
   if (GET_GOLD(ch) > 0) {
      /* following 'if' clause added to fix gold duplication loophole */
      if (IS_NPC(ch) || (!IS_NPC(ch) && ch->desc)) {
	 money = create_money(GET_GOLD(ch));
	 obj_to_obj(money, corpse);
      }
      GET_GOLD(ch) = 0;
   }

   corpse->obj_flags.type_flag = ITEM_CONTAINER;
   corpse->obj_flags.wear_flags = ITEM_TAKE;
   corpse->obj_flags.extra_flags = ITEM_NODONATE;
   corpse->obj_flags.value[0] = 0; /* You can't store stuff in a corpse */
   corpse->obj_flags.value[3] = 1; /* corpse identifyer */
   corpse->obj_flags.weight = GET_WEIGHT(ch) + IS_CARRYING_W(ch);
   corpse->obj_flags.cost_per_day = 100000;
   if (IS_NPC(ch))
      corpse->obj_flags.timer = max_npc_corpse_time;
   else
      corpse->obj_flags.timer = max_pc_corpse_time;

   for (i = 0; i < MAX_WEAR; i++)
      if (ch->equipment[i])
	 obj_to_obj(unequip_char(ch, i), corpse);

   ch->carrying = 0;
   IS_CARRYING_N(ch) = 0;
   IS_CARRYING_W(ch) = 0;

   corpse->next = object_list;
   object_list = corpse;

   for (o = corpse->contains; o; o->in_obj = corpse, o = o->next_content)
      ;
   object_list_new_owner(corpse, 0);

   obj_to_room(corpse, ch->in_room);
}


/* When ch kills victim */
void	change_alignment(struct char_data *ch, struct char_data *victim)
{
   int	align;

   align = GET_ALIGNMENT(ch) - GET_ALIGNMENT(victim);

   if (GET_ALIGNMENT(victim) < 0) {
      if (align > 650)
	 GET_ALIGNMENT(ch) = MIN(1000, GET_ALIGNMENT(ch) + ((align - 650) / 8) + ((1000 - GET_ALIGNMENT(victim)) / 5));
      else
	 GET_ALIGNMENT(ch) = MIN(1000, GET_ALIGNMENT(ch) - (GET_ALIGNMENT(victim) / 8));

   } else {
      if (align < -650)
	 GET_ALIGNMENT(ch) = MAX(-1000, GET_ALIGNMENT(ch) + ((align + 650) / 8) + ((-1000 + GET_ALIGNMENT(victim)) / 5));
      else
	 GET_ALIGNMENT(ch) = MAX(-1000, GET_ALIGNMENT(ch) - (GET_ALIGNMENT(victim) / 8));
   }
}



void	death_cry(struct char_data *ch)
{
   int	door, was_in;

   act("Your blood freezes as you hear $n's death cry.", FALSE, ch, 0, 0, TO_ROOM);
   was_in = ch->in_room;

   for (door = 0; door < NUM_OF_DIRS; door++) {
      if (CAN_GO(ch, door))	 {
	 ch->in_room = world[was_in].dir_option[door]->to_room;
	 act("Your blood freezes as you hear someone's death cry.", FALSE, ch, 0, 0, TO_ROOM);
	 ch->in_room = was_in;
      }
   }
}



void	raw_kill(struct char_data *ch)
{
   if (ch->specials.fighting)
      stop_fighting(ch);

   while (ch->affected)
      affect_remove(ch, ch->affected);

   death_cry(ch);

   make_corpse(ch);
   extract_char(ch);
}



void	die(struct char_data *ch)
{
   gain_exp(ch, -(GET_EXP(ch) / 2));
   if (!IS_NPC(ch))
      REMOVE_BIT(PLR_FLAGS(ch), PLR_KILLER | PLR_THIEF);
   raw_kill(ch);
}



void	group_gain(struct char_data *ch, struct char_data *victim)

{
   int	no_members, share;
   struct char_data *k;
   struct follow_type *f;

   if (!(k = ch->master))
      k = ch;


   if (IS_AFFECTED(k, AFF_GROUP) && 
       (k->in_room == ch->in_room))
      no_members = 1;
   else
      no_members = 0;

   for (f = k->followers; f; f = f->next)
      if (IS_AFFECTED(f->follower, AFF_GROUP) && 
          (f->follower->in_room == ch->in_room))
	 no_members++;

   if (no_members >= 1)
      share = MIN(450000 / no_members, (GET_EXP(victim) / 3) / no_members);
   else
      share = 0;

   if (IS_AFFECTED(k, AFF_GROUP) && 
       (k->in_room == ch->in_room)) {
      if (share > 1) {
	 sprintf(buf2, "You receive your share of experience -- %d points.",
	     share);
      } else
	 strcpy(buf2, "You receive your share of experience -- 1 measly little point.");
      act(buf2, FALSE, k, 0, 0, TO_CHAR);
      gain_exp(k, share);
      change_alignment(k, victim);
      if (auto_save) {
         /* save 25% of the time to avoid lag in big groups */
         if (!(number(0, 3)))
	    save_char(k, NOWHERE);
      }
   }

   for (f = k->followers; f; f = f->next) {
      if (IS_AFFECTED(f->follower, AFF_GROUP) && 
          (f->follower->in_room == ch->in_room)) {
	 if (share > 1)
	    sprintf(buf2, "You receive your share of experience -- %d points.", share);
	 else
	    strcpy(buf2, "You receive your share of experience -- 1 measly little point.");
	 act(buf2, FALSE, f->follower, 0, 0, TO_CHAR);
	 gain_exp(f->follower, share);
	 change_alignment(f->follower, victim);
	 if (auto_save) {
	    /* save only 25% of the time to avoid lag in big groups */
	    if (!(number(0, 3)))
	       save_char(f->follower, NOWHERE);
	 }
      }
   }
}


char	*replace_string(char *str, char *weapon_singular, char *weapon_plural)
{
   static char	buf[256];
   char	*cp;

   cp = buf;

   for (; *str; str++) {
      if (*str == '#') {
	 switch (*(++str)) {
	 case 'W' :
	    for (; *weapon_plural; *(cp++) = *(weapon_plural++)) ;
	    break;
	 case 'w' :
	    for (; *weapon_singular; *(cp++) = *(weapon_singular++)) ;
	    break;
	 default :
	    *(cp++) = '#';
	    break;
	 }
      } else
	 *(cp++) = *str;

      *cp = 0;
   } /* For */

   return(buf);
}



void	dam_message(int dam, struct char_data *ch, struct char_data *victim,
int w_type)
{
   struct obj_data *wield;
   char	*buf;
   int	msgnum;

   static struct dam_weapon_type {
      char	*to_room;
      char	*to_char;
      char	*to_victim;
   } dam_weapons[] = {

   /* use #w for singular (i.e. "slash") and #W for plural (i.e. "slashes") */

      { "$n misses $N with $s #w.",			/* 0: 0     */
      "You miss $N with your #w.",
      "$n misses you with $s #w." },

      { "$n tickles $N with $s #w.",			/* 1: 1..2  */
      "You tickle $N as you #w $M.",
      "$n tickles you as $e #W you." },

      { "$n barely #W $N.",				/* 2: 3..4  */
      "You barely #w $N.",
      "$n barely #W you." },

      { "$n #W $N.",					/* 3: 5..6  */
      "You #w $N.",
      "$n #W you." },

      { "$n #W $N hard.",				/* 4: 7..10  */
      "You #w $N hard.",
      "$n #W you hard." },

      { "$n #W $N very hard.",				/* 5: 11..14  */
      "You #w $N very hard.",
      "$n #W you very hard." },

      { "$n #W $N extremely hard.", 			/* 6: 15..19  */
      "You #w $N extremely hard.",
      "$n #W you extremely hard." },

      { "$n massacres $N to small fragments with $s #w.",/* 7: 19..23 */
      "You massacre $N to small fragments with your #w.",
      "$n massacres you to small fragments with $s #w." },

      { "$n OBLITERATES $N with $s deadly #w!!",	/* 8: > 23   */
      "You OBLITERATE $N with your deadly #w!!",
      "$n OBLITERATES you with $s deadly #w!!" }
   };   



   w_type -= TYPE_HIT;   /* Change to base of table with text */

   wield = ch->equipment[WIELD];

   if (dam == 0)	msgnum = 0;
   else if (dam <= 2)	msgnum = 1;
   else if (dam <= 4)	msgnum = 2;
   else if (dam <= 6)	msgnum = 3;
   else if (dam <= 10)	msgnum = 4;
   else if (dam <= 14)	msgnum = 5;
   else if (dam <= 19)	msgnum = 6;
   else if (dam <= 23)	msgnum = 7;
   else			msgnum = 8;

   /* damage message to onlookers */
   buf = replace_string(dam_weapons[msgnum].to_room,
            attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
   act(buf, FALSE, ch, wield, victim, TO_NOTVICT);

   /* damage message to damager */
   send_to_char(CCYEL(ch, C_CMP), ch);
   buf = replace_string(dam_weapons[msgnum].to_char,
	    attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
   act(buf, FALSE, ch, wield, victim, TO_CHAR);
   send_to_char(CCNRM(ch, C_CMP), ch);

   /* damage message to damagee */
   send_to_char(CCRED(victim, C_CMP), victim);
   buf = replace_string(dam_weapons[msgnum].to_victim,
            attack_hit_text[w_type].singular, attack_hit_text[w_type].plural);
   act(buf, FALSE, ch, wield, victim, TO_VICT);
   send_to_char(CCNRM(victim, C_CMP), victim);
}


void	damage(struct char_data *ch, struct char_data *victim, int dam, int attacktype)
{
   struct message_type *messages;
   int	i, j, nr, exp;

   if (GET_POS(victim) <= POSITION_DEAD) {
      log("SYSERR: Attempt to damage a corpse.");
      return;   /* -je, 7/7/92 */
   }

   assert(GET_POS(victim) > POSITION_DEAD);

   /* You can't damage an immortal! */
   if ((GET_LEVEL(victim) >= LEVEL_IMMORT) && !IS_NPC(victim))
      dam = 0;

   if (victim != ch) {
      if (GET_POS(ch) > POSITION_STUNNED) {
	 if (!(ch->specials.fighting))
	    set_fighting(ch, victim);

	 if (IS_NPC(ch) && IS_NPC(victim) && victim->master && 
	     !number(0, 10) && IS_AFFECTED(victim, AFF_CHARM) && 
	     (victim->master->in_room == ch->in_room)) {
	    if (ch->specials.fighting)
	       stop_fighting(ch);
	    hit(ch, victim->master, TYPE_UNDEFINED);
	    return;
	 }
      }

      if (GET_POS(victim) > POSITION_STUNNED) {
	 if (!(victim->specials.fighting))
	    set_fighting(victim, ch);
	 if (IS_NPC(victim) && IS_SET(victim->specials2.act, MOB_MEMORY) && 
	     !IS_NPC(ch) && (GET_LEVEL(ch) < LEVEL_IMMORT))
	    remember(victim, ch);
	 GET_POS(victim) = POSITION_FIGHTING;
      }
   }

   if (victim->master == ch)
      stop_follower(victim);

   if (IS_AFFECTED(ch, AFF_INVISIBLE))
      appear(ch);

   if (IS_AFFECTED(victim, AFF_SANCTUARY))
      dam = dam / 2;  /* 1/2 damage when sanctuary */

   if (!pk_allowed) {
      check_killer(ch, victim);

      if (PLR_FLAGGED(ch, PLR_KILLER))
         dam = 0;
   }

   dam = MIN(dam, 100);

   dam = MAX(dam, 0);

   GET_HIT(victim) -= dam;

   if (ch != victim)
      gain_exp(ch, GET_LEVEL(victim) * dam);

   update_pos(victim);


   if ((attacktype >= TYPE_HIT) && (attacktype <= TYPE_CRUSH)) {
      if (!ch->equipment[WIELD])
	 dam_message(dam, ch, victim, TYPE_HIT);
      else
	 dam_message(dam, ch, victim, attacktype);
   } else {

      for (i = 0; i < MAX_MESSAGES; i++) {
	 if (fight_messages[i].a_type == attacktype) {
	    nr = dice(1, fight_messages[i].number_of_attacks);
	    for (j = 1, messages = fight_messages[i].msg; (j < nr) && (messages); j++)
	       messages = messages->next;

	    if (!IS_NPC(victim) && (GET_LEVEL(victim) >= LEVEL_IMMORT)) {
	       act(messages->god_msg.attacker_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR);
	       act(messages->god_msg.victim_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_VICT);
	       act(messages->god_msg.room_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT);
	    } else if (dam != 0) {
	       if (GET_POS(victim) == POSITION_DEAD) {
		  act(messages->die_msg.attacker_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR);
		  act(messages->die_msg.victim_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_VICT);
		  act(messages->die_msg.room_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT);
	       } else {
		  act(messages->hit_msg.attacker_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR);
		  act(messages->hit_msg.victim_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_VICT);
		  act(messages->hit_msg.room_msg, FALSE, ch, ch->equipment[WIELD], victim,
		      TO_NOTVICT);
	       }
	    } else { /* Dam == 0 */
	       act(messages->miss_msg.attacker_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR);
	       act(messages->miss_msg.victim_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_VICT);
	       act(messages->miss_msg.room_msg, FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT);
	    }
	 }
      }
   }

   /* Use send_to_char -- act() doesn't send message if you are DEAD. */
   switch (GET_POS(victim)) {
   case POSITION_MORTALLYW:
      act("$n is mortally wounded, and will die soon, if not aided.", TRUE, victim, 0, 0, TO_ROOM);
      send_to_char("You are mortally wounded, and will die soon, if not aided.\n\r", victim);
      break;
   case POSITION_INCAP:
      act("$n is incapacitated and will slowly die, if not aided.", TRUE, victim, 0, 0, TO_ROOM);
      send_to_char("You are incapacitated an will slowly die, if not aided.\n\r", victim);
      break;
   case POSITION_STUNNED:
      act("$n is stunned, but will probably regain consciousness again.", TRUE, victim, 0, 0, TO_ROOM);
      send_to_char("You're stunned, but will probably regain consciousness again.\n\r", victim);
      break;
   case POSITION_DEAD:
      act("$n is dead!  R.I.P.", TRUE, victim, 0, 0, TO_ROOM);
      send_to_char("You are dead!  Sorry...\n\r", victim);
      break;

   default:  /* >= POSITION SLEEPING */
      if (dam > (GET_MAX_HIT(victim) / 5))
	 act("That Really did HURT!", FALSE, victim, 0, 0, TO_CHAR);

      if (GET_HIT(victim) < (GET_MAX_HIT(victim) / 5)) {
	 act("You wish that your wounds would stop BLEEDING so much!", FALSE, victim, 0, 0, TO_CHAR);
	 if (IS_NPC(victim)) {
	    if (IS_SET(victim->specials2.act, MOB_WIMPY))
	       do_flee(victim, "", 0, 0);
	 }
      }

      if (!IS_NPC(victim) && WIMP_LEVEL(victim) && victim != ch && 
          GET_HIT(victim) < WIMP_LEVEL(victim)) {
	 send_to_char("You wimp out, and attempt to flee!\n\r", victim);
	 do_flee(victim, "", 0, 0);
      }

      break;
   }

   if (!IS_NPC(victim) && !(victim->desc)) {
      do_flee(victim, "", 0, 0);
      if (!victim->specials.fighting) {
	 act("$n is rescued by divine forces.", FALSE, victim, 0, 0, TO_ROOM);
	 victim->specials.was_in_room = victim->in_room;
	 char_from_room(victim);
	 char_to_room(victim, 0);
      }
   }

   /* this code removed due to popular request -- JE 6/14/93 

   if (GET_POS(victim) < POSITION_STUNNED)
      if (ch->specials.fighting == victim)
	 stop_fighting(ch);
   */

   if (!AWAKE(victim))
      if (victim->specials.fighting)
	 stop_fighting(victim);

   if (GET_POS(victim) == POSITION_DEAD) {
      if (IS_NPC(victim) || victim->desc)
	 if (IS_AFFECTED(ch, AFF_GROUP)) {
	    group_gain(ch, victim);
	 } 
	 else {
	    /* Calculate level-difference bonus */
	    exp = GET_EXP(victim) / 3;
	    if (IS_NPC(ch))
	       exp += (exp * MIN(4, (GET_LEVEL(victim) - GET_LEVEL(ch)))) >> 3;
	    else
	       exp += (exp * MIN(8, (GET_LEVEL(victim) - GET_LEVEL(ch)))) >> 3;
	    exp = MAX(exp, 1);
	    if (exp > 1)
	       sprintf(buf2, "You receive %d experience points.\n\r", exp);
	    else
	       strcpy(buf2, "You receive 1 lousy experience point.\n\r");
	    send_to_char(buf2, ch);
	    gain_exp(ch, exp);
	    change_alignment(ch, victim);
	    if (auto_save) {
	       /* individuals saved 50% of the time to avoid lag */
	       if (!(number(0, 1)))
	          save_char(ch, NOWHERE);
	    }
	 }
      if (!IS_NPC(victim)) {
	 sprintf(buf2, "%s killed by %s at %s", GET_NAME(victim), GET_NAME(ch),
	     world[victim->in_room].name);
	 mudlog(buf2, BRF, LEVEL_IMMORT, TRUE);
      }
      if (IS_NPC(ch) && !IS_NPC(victim) && IS_SET(ch->specials2.act, MOB_MEMORY))
	 forget(ch, victim);
      die(victim);
   }
}



void	hit(struct char_data *ch, struct char_data *victim, int type)
{
   struct obj_data *wielded = 0;
   struct obj_data *held = 0;
   int	w_type;
   int	victim_ac, calc_thaco;
   int	dam;
   byte diceroll;

   extern int	thaco[4][35];
   extern byte backstab_mult[];
   extern struct str_app_type str_app[];
   extern struct dex_app_type dex_app[];

   if (ch->in_room != victim->in_room) {
      log("SYSERR: NOT SAME ROOM WHEN FIGHTING!");
      return;
   }

   if (ch->equipment[HOLD])
      held = ch->equipment[HOLD];

   if (ch->equipment[WIELD] && 
       (ch->equipment[WIELD]->obj_flags.type_flag == ITEM_WEAPON)) {
      wielded = ch->equipment[WIELD];
      switch (wielded->obj_flags.value[3]) {
      case 0  :
      case 1  :
      case 2  :
	 w_type = TYPE_WHIP;
	 break;
      case 3  :
	 w_type = TYPE_SLASH;
	 break;
      case 4  :
      case 5  :
      case 6  :
	 w_type = TYPE_CRUSH;
	 break;
      case 7  :
	 w_type = TYPE_BLUDGEON;
	 break;
      case 8  :
      case 9  :
      case 10 :
	 w_type = TYPE_BLAST;
	 break;
      case 11 :
	 w_type = TYPE_PIERCE;
	 break;
      case 12 :
	 w_type = TYPE_NO_BS_PIERCE;
	 break;

      default :
	 w_type = TYPE_HIT;
	 break;
      }
   }	 else {
      if (IS_NPC(ch) && (ch->specials.attack_type >= TYPE_HIT))
	 w_type = ch->specials.attack_type;
      else
	 w_type = TYPE_HIT;
   }

   /* Calculate the raw armor including magic armor */
   /* The lower AC, the better                      */

   if (!IS_NPC(ch))
      calc_thaco  = thaco[GET_CLASS(ch)-1][(int)GET_LEVEL(ch)];
   else
      /* THAC0 for monsters is set in the HitRoll */
      calc_thaco = 20;

   calc_thaco -= str_app[STRENGTH_APPLY_INDEX(ch)].tohit;
   calc_thaco -= GET_HITROLL(ch);
   calc_thaco -= (int)((GET_INT(ch) - 13) / 1.5);   /* Smartness helps! */
   calc_thaco -= (int)((GET_WIS(ch) - 13) / 1.5);   /* So does wisdom */
   diceroll = number(1, 20);

   victim_ac  = GET_AC(victim) / 10;

   if (AWAKE(victim))
      victim_ac += dex_app[GET_DEX(victim)].defensive;

   victim_ac = MAX(-10, victim_ac);  /* -10 is lowest */

   if ((((diceroll < 20) && AWAKE(victim)) && 
       ((diceroll == 1) || ((calc_thaco - diceroll) > victim_ac)))) {
      if (type == SKILL_BACKSTAB)
	 damage(ch, victim, 0, SKILL_BACKSTAB);
      else
	 damage(ch, victim, 0, w_type);
   } else {

      dam  = str_app[STRENGTH_APPLY_INDEX(ch)].todam;
      dam += GET_DAMROLL(ch);

      if (!wielded) {
	 if (IS_NPC(ch)) {
	    dam += dice(ch->specials.damnodice, ch->specials.damsizedice);
	 } else
	    dam += number(0, 2);  /* Max. 2 dam with bare hands */
      } else
	 dam += dice(wielded->obj_flags.value[1], wielded->obj_flags.value[2]);

      if (GET_POS(victim) < POSITION_FIGHTING)
	 dam *= 1 + (POSITION_FIGHTING - GET_POS(victim)) / 3;
      /* Position  sitting  x 1.33 */
      /* Position  resting  x 1.66 */
      /* Position  sleeping x 2.00 */
      /* Position  stunned  x 2.33 */
      /* Position  incap    x 2.66 */
      /* Position  mortally x 3.00 */

      dam = MAX(1, dam);  /* Not less than 0 damage */

      if (type == SKILL_BACKSTAB) {
	 dam *= backstab_mult[(int)GET_LEVEL(ch)];
	 damage(ch, victim, dam, SKILL_BACKSTAB);
      } else
	 damage(ch, victim, dam, w_type);
   }
}



/* control the fights going on */
void	perform_violence(void)
{
   struct char_data *ch;

   for (ch = combat_list; ch; ch = combat_next_dude) {
      combat_next_dude = ch->next_fighting;
      assert(ch->specials.fighting);

      if (AWAKE(ch) && (ch->in_room == ch->specials.fighting->in_room)) {
	 hit(ch, ch->specials.fighting, TYPE_UNDEFINED);
      } else { /* Not in same room */
	 stop_fighting(ch);
      }
   }
}


