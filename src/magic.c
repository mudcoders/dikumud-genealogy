/* ************************************************************************
*   File: magic.c                                       Part of CircleMUD *
*  Usage: low-level functions for magic; spell template code              *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */


#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "spells.h"
#include "handler.h"
#include "db.h"

extern struct room_data *world;
extern struct obj_data *object_list;
extern struct char_data *character_list;
extern struct int_app_type int_app[];
extern struct index_data *obj_index;

extern struct weather_data weather_info;
extern struct descriptor_data *descriptor_list;
extern struct zone_data *zone_table;

extern int mini_mud;
extern int pk_allowed;

extern struct default_mobile_stats *mob_defaults;
extern char weapon_verbs[];
extern int *max_ac_applys;
extern struct apply_mod_defaults *apmd;

void clearMemory(struct char_data * ch);
void act(char *str, int i, struct char_data * c, struct obj_data * o,
	      void *vict_obj, int j);

void damage(struct char_data * ch, struct char_data * victim,
	         int damage, int weapontype);

void weight_change_object(struct obj_data * obj, int weight);
void add_follower(struct char_data * ch, struct char_data * leader);
int dice(int number, int size);
extern struct spell_info_type spell_info[];


struct char_data *read_mobile(int, int);


/*
 * Saving throws for:
 * MCTW
 *   PARA, ROD, PETRI, BREATH, SPELL
 *     Levels 0-40
 */

const byte saving_throws[NUM_CLASSES][5][41] = {

  {				/* Mages */
    {90, 70, 69, 68, 67, 66, 65, 63, 61, 60, 59,	/* 0 - 10 */
       /* PARA */ 57, 55, 54, 53, 53, 52, 51, 50, 48, 46,	/* 11 - 20 */
      45, 44, 42, 40, 38, 36, 34, 32, 30, 28,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 55, 53, 51, 49, 47, 45, 43, 41, 40, 39,	/* 0 - 10 */
       /* ROD */ 37, 35, 33, 31, 30, 29, 27, 25, 23, 21,	/* 11 - 20 */
      20, 19, 17, 15, 14, 13, 12, 11, 10, 9,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 65, 63, 61, 59, 57, 55, 53, 51, 50, 49,	/* 0 - 10 */
       /* PETRI */ 47, 45, 43, 41, 40, 39, 37, 35, 33, 31,	/* 11 - 20 */
      30, 29, 27, 25, 23, 21, 19, 17, 15, 13,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 75, 73, 71, 69, 67, 65, 63, 61, 60, 59,	/* 0 - 10 */
       /* BREATH */ 57, 55, 53, 51, 50, 49, 47, 45, 43, 41,	/* 11 - 20 */
      40, 39, 37, 35, 33, 31, 29, 27, 25, 23,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 60, 58, 56, 54, 52, 50, 48, 46, 45, 44,	/* 0 - 10 */
       /* SPELL */ 42, 40, 38, 36, 35, 34, 32, 30, 28, 26,	/* 11 - 20 */
      25, 24, 22, 20, 18, 16, 14, 12, 10, 8,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */
  },

  {				/* Clerics */
    {90, 50, 59, 48, 46, 45, 43, 40, 37, 35, 34,	/* 0 - 10 */
       /* PARA */ 33, 31, 30, 29, 27, 26, 25, 24, 23, 22,	/* 11 - 20 */
      21, 20, 18, 15, 14, 12, 10, 9, 8, 7,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 70, 69, 68, 66, 65, 63, 60, 57, 55, 54,	/* 0 - 10 */
       /* ROD */ 53, 51, 50, 49, 47, 46, 45, 44, 43, 42,	/* 11 - 20 */
      41, 40, 38, 35, 34, 32, 30, 29, 28, 27,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 65, 64, 63, 61, 60, 58, 55, 53, 50, 49,	/* 0 - 10 */
       /* PETRI */ 48, 46, 45, 44, 43, 41, 40, 39, 38, 37,	/* 11 - 20 */
      36, 35, 33, 31, 29, 27, 25, 24, 23, 22,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 80, 79, 78, 76, 75, 73, 70, 67, 65, 64,	/* 0 - 10 */
       /* BREATH */ 63, 61, 60, 59, 57, 56, 55, 54, 53, 52,	/* 11 - 20 */
      51, 50, 48, 45, 44, 42, 40, 39, 38, 37,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 75, 74, 73, 71, 70, 68, 65, 63, 60, 59,	/* 0 - 10 */
       /* SPELL */ 58, 56, 55, 54, 53, 51, 50, 49, 48, 47,	/* 11 - 20 */
      46, 45, 43, 41, 39, 37, 35, 34, 33, 32,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0}	/* 31 - 40 */
  },

  {				/* Thieves */
    {90, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56,	/* 0 - 10 */
       /* PARA */ 55, 54, 53, 52, 51, 50, 49, 48, 47, 46,	/* 11 - 20 */
      45, 44, 43, 42, 41, 40, 39, 38, 37, 36,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 70, 68, 66, 64, 62, 60, 58, 56, 54, 52,	/* 0 - 10 */
       /* ROD */ 50, 48, 46, 44, 42, 40, 38, 36, 34, 32,	/* 11 - 20 */
      30, 28, 26, 24, 22, 20, 18, 16, 14, 13,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 60, 59, 58, 58, 56, 55, 54, 53, 52, 51,	/* 0 - 10 */
       /* PETRI */ 50, 49, 48, 47, 46, 45, 44, 43, 42, 41,	/* 11 - 20 */
      40, 39, 38, 37, 36, 35, 34, 33, 32, 31,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71,	/* 0 - 10 */
       /* BREATH */ 70, 69, 68, 67, 66, 65, 64, 63, 62, 61,	/* 11 - 20 */
      60, 59, 58, 57, 56, 55, 54, 53, 52, 51,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* 31 - 40 */

    {90, 75, 73, 71, 69, 67, 65, 63, 61, 59, 57,	/* 0 - 10 */
       /* SPELL */ 55, 53, 51, 49, 47, 45, 43, 41, 39, 37,	/* 11 - 20 */
      35, 33, 31, 29, 27, 25, 23, 21, 19, 17,	/* 21 - 30 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0}	/* 31 - 40 */
  },

  {				/* Warriors */
    {90, 70, 68, 67, 65, 62, 58, 55, 53, 52, 50,	/* 0 - 10 */
       /* PARA */ 47, 43, 40, 38, 37, 35, 32, 28, 25, 24,	/* 11 - 20 */
      23, 22, 20, 19, 17, 16, 15, 14, 13, 12,	/* 21 - 30 */
    11, 10, 9, 8, 7, 6, 5, 4, 3, 2},	/* 31 - 40 */

    {90, 80, 78, 77, 75, 72, 68, 65, 63, 62, 60,	/* 0 - 10 */
       /* ROD */ 57, 53, 50, 48, 47, 45, 42, 38, 35, 34,	/* 11 - 20 */
      33, 32, 30, 29, 27, 26, 25, 24, 23, 22,	/* 21 - 30 */
    20, 18, 16, 14, 12, 10, 8, 6, 5, 4},	/* 31 - 40 */

    {90, 75, 73, 72, 70, 67, 63, 60, 58, 57, 55,	/* 0 - 10 */
       /* PETRI */ 52, 48, 45, 43, 42, 40, 37, 33, 30, 29,	/* 11 - 20 */
      28, 26, 25, 24, 23, 21, 20, 19, 18, 17,	/* 21 - 30 */
    16, 15, 14, 13, 12, 11, 10, 9, 8, 7},	/* 31 - 40 */

    {90, 85, 83, 82, 80, 75, 70, 65, 63, 62, 60,	/* 0 - 10 */
       /* BREATH */ 55, 50, 45, 43, 42, 40, 37, 33, 30, 29,	/* 11 - 20 */
      28, 26, 25, 24, 23, 21, 20, 19, 18, 17,	/* 21 - 30 */
    16, 15, 14, 13, 12, 11, 10, 9, 8, 7},	/* 31 - 40 */

    {90, 85, 83, 82, 80, 77, 73, 70, 68, 67, 65,	/* 0 - 10 */
       /* SPELL */ 62, 58, 55, 53, 52, 50, 47, 43, 40, 39,	/* 11 - 20 */
      38, 36, 35, 34, 33, 31, 30, 29, 28, 27,	/* 21 - 30 */
    25, 23, 21, 19, 17, 15, 13, 11, 9, 7}	/* 31 - 40 */
  }
};


int mag_savingthrow(struct char_data * ch, int type)
{
  int save;

  /* negative apply_saving_throw values make saving throws better! */

  if (IS_NPC(ch))
    save = saving_throws[CLASS_WARRIOR][type][(int) GET_LEVEL(ch)];
  else
    save = saving_throws[(int) GET_CLASS(ch)][type][(int) GET_LEVEL(ch)];

  save += GET_SAVE(ch, type);

  /* throwing a 0 is always a failure */
  if (MAX(1, save) < number(0, 99))
    return TRUE;
  else
    return FALSE;
}


/* affect_update: called from comm.c (causes spells to wear off) */
void affect_update(void)
{
  static struct affected_type *af, *next;
  static struct char_data *i;
  extern char *spell_wear_off_msg[];

  for (i = character_list; i; i = i->next)
    for (af = i->affected; af; af = next) {
      next = af->next;
      if (af->duration >= 1)
	af->duration--;
      else if (af->duration == -1)
	/* No action */
	af->duration = -1;	/* GODs only! unlimited */
      else {
	if ((af->type > 0) && (af->type <= MAX_SPELLS))
	  if (!af->next || (af->next->type != af->type) ||
	      (af->next->duration > 0))
	    if (*spell_wear_off_msg[af->type]) {
	      send_to_char(spell_wear_off_msg[af->type], i);
	      send_to_char("\r\n", i);
	    }
	affect_remove(i, af);
      }
    }
}


/*
  mag_materials:
  Checks for up to 3 vnums in the player's inventory.
*/

int mag_materials(struct char_data * ch, int item0, int item1, int item2,
		      int extract, int verbose)
{
  struct obj_data *tobj;
  struct obj_data *obj0, *obj1, *obj2;

  for (tobj = ch->carrying; tobj; tobj = tobj->next) {
    if ((item0 > 0) && (GET_OBJ_VNUM(tobj) == item0)) {
      obj0 = tobj;
      item0 = -1;
    } else if ((item1 > 0) && (GET_OBJ_VNUM(tobj) == item1)) {
      obj1 = tobj;
      item1 = -1;
    } else if ((item2 > 0) && (GET_OBJ_VNUM(tobj) == item2)) {
      obj2 = tobj;
      item2 = -1;
    }
  }
  if ((item0 > 0) || (item1 > 0) || (item2 > 0)) {
    if (verbose) {
      switch (number(0, 2)) {
      case 0:
	send_to_char("A wart sprouts on your nose.\r\n", ch);
	break;
      case 1:
	send_to_char("Your hair falls out in clumps.\r\n", ch);
	break;
      case 2:
	send_to_char("A huge corn develops on your big toe.\r\n", ch);
	break;
      }
    }
    return (FALSE);
  }
  if (extract) {
    if (item0 < 0) {
      obj_from_char(obj0);
      extract_obj(obj0);
    }
    if (item1 < 0) {
      obj_from_char(obj1);
      extract_obj(obj1);
    }
    if (item2 < 0) {
      obj_from_char(obj2);
      extract_obj(obj2);
    }
  }
  if (verbose) {
    send_to_char("A puff of smoke rises from your pack.\r\n", ch);
    act("A puff of smoke rises from $n's pack.", TRUE, ch, NULL, NULL, TO_ROOM);
  }
  return (TRUE);
}




/*
  Every spell that does damage comes through here.  This calculates the
  amount of damage, adds in any modifiers, determines what the saves are,
  tests for save and calls damage();
*/


void mag_damage(int level, struct char_data * ch, struct char_data * victim,
		     int spellnum, int savetype)
{
  int is_mage = 0, is_cleric = 0;
  int dam = 0;

  if (victim == NULL || ch == NULL)
    return;

  is_mage = (GET_CLASS(ch) == CLASS_MAGIC_USER);
  is_cleric = (GET_CLASS(ch) == CLASS_CLERIC);

  switch (spellnum) {
    /* Mostly mages */
  case SPELL_MAGIC_MISSILE:
  case SPELL_CHILL_TOUCH:	/* chill touch also has an affect */
    if (is_mage)
      dam = dice(1, 8) + 1;
    else
      dam = dice(1, 6) + 1;
    break;
  case SPELL_BURNING_HANDS:
    if (is_mage)
      dam = dice(3, 8) + 3;
    else
      dam = dice(3, 6) + 3;
    break;
  case SPELL_SHOCKING_GRASP:
    if (is_mage)
      dam = dice(5, 8) + 5;
    else
      dam = dice(5, 6) + 5;
    break;
  case SPELL_LIGHTNING_BOLT:
    if (is_mage)
      dam = dice(7, 8) + 7;
    else
      dam = dice(7, 6) + 7;
    break;
  case SPELL_COLOR_SPRAY:
    if (is_mage)
      dam = dice(9, 8) + 9;
    else
      dam = dice(9, 6) + 9;
    break;
  case SPELL_FIREBALL:
    if (is_mage)
      dam = dice(11, 8) + 11;
    else
      dam = dice(11, 6) + 11;
    break;

    /* Mostly clerics */
  case SPELL_DISPEL_EVIL:
    dam = dice(6, 8) + 6;
    if (IS_EVIL(ch)) {
      victim = ch;
      dam = GET_HIT(ch) - 1;
    } else if (IS_GOOD(victim)) {
      act("The gods protect $N.", FALSE, ch, 0, victim, TO_CHAR);
      dam = 0;
      return;
    }
    break;
  case SPELL_DISPEL_GOOD:
    dam = dice(6, 8) + 6;
    if (IS_GOOD(ch)) {
      victim = ch;
      dam = GET_HIT(ch) - 1;
    } else if (IS_EVIL(victim)) {
      act("The gods protect $N.", FALSE, ch, 0, victim, TO_CHAR);
      dam = 0;
      return;
    }
    break;


  case SPELL_CALL_LIGHTNING:
    dam = dice(7, 8) + 7;
    break;

  case SPELL_HARM:
    dam = dice(8, 8) + 8;
    break;

  case SPELL_ENERGY_DRAIN:
    if (GET_LEVEL(victim) <= 2)
      dam = 100;
    else
      dam = dice(1, 10);
    break;

    /* Area spells */
  case SPELL_EARTHQUAKE:
    dam = dice(2, 8) + level;
    break;

  }				/* switch(spellnum) */


  if (mag_savingthrow(victim, savetype))
    dam >>= 1;
  damage(ch, victim, dam, spellnum);
}


/*
  Every spell that does an affect comes through here.  This determines
  the effect, whether it is added or replacement, whether it is legal or
  not, etc.

  affect_join(vict, aff, add_dur, avg_dur, add_mod, avg_mod)
*/

void mag_affects(int level, struct char_data * ch, struct char_data * victim,
		      int spellnum, int savetype)
{

  struct affected_type af;
  int is_mage = FALSE;
  int is_cleric = FALSE;

  if (victim == NULL || ch == NULL)
    return;

  is_mage = (GET_CLASS(ch) == CLASS_MAGIC_USER);
  is_cleric = (GET_CLASS(ch) == CLASS_CLERIC);

  af.bitvector = 0;
  af.modifier = 0;
  af.location = APPLY_NONE;

  switch (spellnum) {
  case SPELL_CHILL_TOUCH:
    if (mag_savingthrow(victim, savetype))
      af.duration = 1;
    else
      af.duration = 4;
    af.type = SPELL_CHILL_TOUCH;
    af.modifier = -1;
    af.location = APPLY_STR;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);
    send_to_char("You feel your strength wither!\r\n", victim);
    break;
  case SPELL_ARMOR:
    af.type = SPELL_ARMOR;
    af.duration = 24;
    af.modifier = -20;
    af.location = APPLY_AC;

    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);
    send_to_char("You feel someone protecting you.\r\n", victim);
    break;
  case SPELL_BLESS:
    af.type = SPELL_BLESS;
    af.modifier = 2;
    af.duration = 6;
    af.location = APPLY_HITROLL;
    af.bitvector = 0;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);

    af.location = APPLY_SAVING_SPELL;
    af.modifier = -1;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);

    send_to_char("You feel righteous.\r\n", victim);
    break;
  case SPELL_BLINDNESS:
    if (IS_AFFECTED(victim, AFF_BLIND)) {
      send_to_char("Nothing seems to happen.\r\n", ch);
      return;
    }
    if (mag_savingthrow(victim, savetype)) {
      send_to_char("You fail.\r\n", ch);
      return;
    }
    act("$n seems to be blinded!", TRUE, victim, 0, 0, TO_ROOM);
    send_to_char("You have been blinded!\r\n", victim);

    af.type = SPELL_BLINDNESS;
    af.location = APPLY_HITROLL;
    af.modifier = -4;
    af.duration = 2;
    af.bitvector = AFF_BLIND;
    affect_join(victim, &af, FALSE, FALSE, FALSE, FALSE);

    af.location = APPLY_AC;
    af.modifier = 40;
    affect_join(victim, &af, FALSE, FALSE, FALSE, FALSE);
    break;

  case SPELL_CURSE:
    if (mag_savingthrow(victim, savetype))
      return;
    af.type = SPELL_CURSE;
    af.duration = 1 + (GET_LEVEL(ch) >> 1);
    af.modifier = -1;
    af.location = APPLY_HITROLL;
    af.bitvector = AFF_CURSE;
    affect_join(victim, &af, TRUE, FALSE, TRUE, TRUE);

    af.modifier = -1;
    af.location = APPLY_DAMROLL;
    affect_join(victim, &af, TRUE, FALSE, TRUE, TRUE);

    act("$n briefly glows red!", FALSE, victim, 0, 0, TO_ROOM);
    act("You feel very uncomfortable.", FALSE, victim, 0, 0, TO_CHAR);
    break;

  case SPELL_DETECT_ALIGN:
    af.type = SPELL_DETECT_ALIGN;
    af.duration = 12 + level;
    af.bitvector = AFF_DETECT_ALIGN;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);
    send_to_char("Your eyes tingle.\r\n", victim);
    break;

  case SPELL_DETECT_INVIS:
    af.type = SPELL_DETECT_INVIS;
    af.duration = 12 + level;
    af.bitvector = AFF_DETECT_INVIS;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);
    send_to_char("Your eyes tingle.\r\n", victim);
    break;

  case SPELL_DETECT_MAGIC:
    af.type = SPELL_DETECT_MAGIC;
    af.duration = 12 + level;
    af.bitvector = AFF_DETECT_MAGIC;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);
    send_to_char("Your eyes tingle.\r\n", victim);
    break;

  case SPELL_DETECT_POISON:
    if (victim == ch)
      if (IS_AFFECTED(victim, AFF_POISON))
	send_to_char("You can sense poison in your blood.\r\n", ch);
      else
	send_to_char("You feel healthy.\r\n", ch);
    else if (IS_AFFECTED(victim, AFF_POISON))
      act("You sense that $E is poisoned.", FALSE, ch, 0, victim, TO_CHAR);
    else
      act("You sense that $E is healthy.", FALSE, ch, 0, victim, TO_CHAR);
    break;

  case SPELL_INFRAVISION:
    af.type = SPELL_INFRAVISION;
    af.duration = 12 + level;
    af.bitvector = AFF_INFRAVISION;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);
    send_to_char("Your eyes glow red.\r\n", victim);
    act("$n's eyes glow red.", TRUE, victim, 0, 0, TO_ROOM);
    break;

  case SPELL_INVISIBLE:
    if (!victim)
      victim = ch;
    act("$n slowly fades out of existence.", TRUE, victim, 0, 0, TO_NOTVICT);
    send_to_char("You vanish.\r\n", victim);

    af.type = SPELL_INVISIBLE;
    af.duration = 12 + (GET_LEVEL(ch) >> 2);
    af.modifier = -40;
    af.location = APPLY_AC;
    af.bitvector = AFF_INVISIBLE;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);
    break;

  case SPELL_POISON:
    if (!mag_savingthrow(victim, savetype)) {
      af.type = SPELL_POISON;
      af.duration = GET_LEVEL(ch);
      af.modifier = -2;
      af.location = APPLY_STR;
      af.bitvector = AFF_POISON;
      affect_join(victim, &af, FALSE, FALSE, FALSE, FALSE);
      send_to_char("You feel very sick.\r\n", victim);
      act("$N gets violently ill!", TRUE, ch, NULL, victim, TO_NOTVICT);
    }
    break;

  case SPELL_PROT_FROM_EVIL:
    af.type = SPELL_PROT_FROM_EVIL;
    af.duration = 24;
    af.bitvector = AFF_PROTECT_EVIL;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);
    send_to_char("You feel invulnerable!\r\n", victim);
    break;

  case SPELL_SANCTUARY:
    act("$n is surrounded by a white aura.", TRUE, victim, 0, 0, TO_ROOM);
    act("You start glowing.", TRUE, victim, 0, 0, TO_CHAR);

    af.type = SPELL_SANCTUARY;
    af.duration = 4;
    af.bitvector = AFF_SANCTUARY;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);
    break;

  case SPELL_SLEEP:
    if (!pk_allowed && !IS_NPC(ch) && !IS_NPC(victim))
      return;
    if (mag_savingthrow(victim, savetype))
      return;

    af.type = SPELL_SLEEP;
    af.duration = 4 + (GET_LEVEL(ch) >> 2);
    af.location = APPLY_NONE;
    af.bitvector = AFF_SLEEP;
    affect_join(victim, &af, FALSE, FALSE, FALSE, FALSE);
    if (GET_POS(victim) > POS_SLEEPING) {
      act("You feel very sleepy...zzzzzz", FALSE, victim, 0, 0, TO_CHAR);
      act("$n goes to sleep.", TRUE, victim, 0, 0, TO_ROOM);
      GET_POS(victim) = POS_SLEEPING;
    }
    break;

  case SPELL_STRENGTH:
    send_to_char("You feel stronger!\r\n", victim);
    af.type = SPELL_STRENGTH;
    af.duration = (GET_LEVEL(ch) >> 1) + 4;
    af.modifier = 1 + (level > 18);
    af.location = APPLY_STR;
    affect_join(victim, &af, TRUE, TRUE, TRUE, FALSE);
    break;

  case SPELL_SENSE_LIFE:
    send_to_char("Your feel your awareness improve.\r\n", ch);
    af.type = SPELL_SENSE_LIFE;
    af.duration = GET_LEVEL(ch);
    af.bitvector = AFF_SENSE_LIFE;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);
    break;

  case SPELL_WATERWALK:
    af.type = SPELL_WATERWALK;
    af.duration = 24;
    af.bitvector = AFF_WATERWALK;
    affect_join(victim, &af, TRUE, FALSE, FALSE, FALSE);
    send_to_char("You feel webbing between your toes.\r\n", victim);
    break;
  }
}


void mag_group_switch(int level, struct char_data * ch, struct char_data * tch,
		           int spellnum, int savetype)
{
  switch (spellnum) {
    case SPELL_GROUP_HEAL:
    mag_points(level, ch, tch, SPELL_HEAL, savetype);
    break;
  case SPELL_GROUP_ARMOR:
    mag_affects(level, ch, tch, SPELL_ARMOR, savetype);
    break;
  case SPELL_GROUP_RECALL:
    spell_recall(level, ch, tch, NULL);
    break;
  }
}

/*
  Every spell that affects the group should run through here
  mag_group_switch contains the switch statement to send us to the right
  magic.

  group spells affect everyone grouped with the caster who is in the room.
*/

void mag_groups(int level, struct char_data * ch, int spellnum, int savetype)
{
  struct char_data *tch, *k;
  struct follow_type *f, *f_next;

  if (ch == NULL)
    return;

  if (!IS_AFFECTED(ch, AFF_GROUP))
    return;
  if (ch->master != NULL)
    k = ch->master;
  else
    k = ch;
  for (f = k->followers; f; f = f_next) {
    f_next = f->next;
    tch = f->follower;
    if (tch->in_room != ch->in_room)
      continue;
    if (!IS_AFFECTED(tch, AFF_GROUP))
      continue;
    mag_group_switch(level, ch, tch, spellnum, savetype);
  }
  if ((k != ch) && IS_AFFECTED(k, AFF_GROUP))
    mag_group_switch(level, ch, k, spellnum, savetype);
  mag_group_switch(level, ch, ch, spellnum, savetype);
}


/*
  mass spells affect every creature in the room except the caster.
*/

void mag_masses(int level, struct char_data * ch, int spellnum, int savetype)
{
  struct char_data *tch, *tch_next;

  for (tch = world[ch->in_room].people; tch; tch = tch_next) {
    tch_next = tch->next_in_room;
    if (tch == ch)
      continue;

    switch (spellnum) {
    }
  }
}


/*
  Every spell that affects an area (room) runs through here.  These are
  generally offensive spells.  This calls mag_damage to do the actual
  damage.

  area spells have limited targets within the room.
*/

void mag_areas(byte level, struct char_data * ch, int spellnum, int savetype)
{
  struct char_data *tch, *next_tch, *m;
  int is_mage = 0, is_cleric = 0;
  int skip;

  if (ch == NULL)
    return;

  is_mage = (GET_CLASS(ch) == CLASS_MAGIC_USER);
  is_cleric = (GET_CLASS(ch) == CLASS_CLERIC);

  if (ch->master != NULL)
    m = ch->master;
  else
    m = ch;

  for (tch = world[ch->in_room].people; tch; tch = next_tch) {
    next_tch = tch->next_in_room;


    /*
     * The skips: 1) immortals && self 2) mobs only hit charmed mobs 3)
     * players can only hit players in CRIMEOK rooms 4) players can only hit
     * charmed mobs in CRIMEOK rooms
     */

    skip = 0;
    if (tch == ch)
      skip = 1;
    if (IS_NPC(ch) && IS_NPC(tch) && !IS_AFFECTED(tch, AFF_CHARM))
      skip = 1;
    if (!IS_NPC(tch) && GET_LEVEL(tch) >= LVL_IMMORT)
      skip = 1;
    if (!IS_NPC(ch) && !IS_NPC(tch))
      skip = 1;
    if (!IS_NPC(ch) && IS_NPC(tch) && IS_AFFECTED(tch, AFF_CHARM))
      skip = 1;

    if (skip)
      continue;

    switch (spellnum) {
    case SPELL_EARTHQUAKE:
      mag_damage(GET_LEVEL(ch), ch, tch, spellnum, 1);
      break;
    default:
      return;
    }
  }
}


/*
  Every spell which summons/gates/conjours a mob comes through here.
*/

static char *mag_summon_msgs[] = {
  "\r\n",
  "$n makes a strange magical gesture; you feel a strong breeze!\r\n",
  "$n animates a corpse!\r\n",
  "$N appears from a cloud of thick blue smoke!\r\n",
  "$N appears from a cloud of thick green smoke!\r\n",
  "$N appears from a cloud of thick red smoke!\r\n",
  "$N disappears in a thick black cloud!\r\n"
  "As $n makes a strange magical gesture, you feel a strong breeze.\r\n",
  "As $n makes a strange magical gesture, you feel a searing heat.\r\n",
  "As $n makes a strange magical gesture, you feel a sudden chill.\r\n",
  "As $n makes a strange magical gesture, you feel the dust swirl.\r\n",
  "$n magically divides!\r\n",
  "$n animates a corpse!\r\n"
};

static char *mag_summon_fail_msgs[] = {
  "\r\n",
  "There are no such creatures.\r\n",
  "Uh oh...\r\n",
  "Oh dear.\r\n",
  "Oh shit!\r\n",
  "The elements resist!\r\n",
  "You failed.\r\n",
  "There is no corpse!\r\n"
};

#define MOB_MONSUM_I		130
#define MOB_MONSUM_II		140
#define MOB_MONSUM_III		150
#define MOB_GATE_I		160
#define MOB_GATE_II		170
#define MOB_GATE_III		180
#define MOB_ELEMENTAL_BASE	110
#define MOB_CLONE		69
#define MOB_ZOMBIE		101
#define MOB_AERIALSERVANT	109


void mag_summons(int level, struct char_data * ch, struct obj_data * obj,
		      int spellnum, int savetype)
{
  struct char_data *mob;
  struct obj_data *tobj, *next_obj;
  int pfail = 0;
  int msg = 0, fmsg = 0;
  int num = 1;
  int a, i;
  int mob_num = 0;
  int handle_corpse = 0;

  if (ch == NULL)
    return;

  switch (spellnum) {
  case SPELL_ANIMATE_DEAD:
    if ((obj == NULL) || (GET_OBJ_TYPE(obj) != ITEM_CONTAINER) ||
	(!GET_OBJ_VAL(obj, 3))) {
      act(mag_summon_fail_msgs[7], FALSE, ch, 0, 0, TO_CHAR);
      return;
    }
    handle_corpse = 1;
    msg = 12;
    mob_num = MOB_ZOMBIE;
    a = number(0, 5);
    if (a)
      mob_num++;
    pfail = 8;
    break;

  default:
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM)) {
    send_to_char("You are too giddy to have any followers!\r\n", ch);
    return;
  }
  if (number(0, 101) < pfail) {
    send_to_char(mag_summon_fail_msgs[fmsg], ch);
    return;
  }
  for (i = 0; i < num; i++) {
    mob = read_mobile(mob_num, VIRTUAL);
    char_to_room(mob, ch->in_room);
    IS_CARRYING_W(mob) = 0;
    IS_CARRYING_N(mob) = 0;
    SET_BIT(AFF_FLAGS(mob), AFF_CHARM);
    add_follower(mob, ch);
    act(mag_summon_msgs[fmsg], FALSE, ch, 0, mob, TO_ROOM);
    if (spellnum == SPELL_CLONE) {
      strcpy(GET_NAME(mob), GET_NAME(ch));
      strcpy(mob->player.short_descr, GET_NAME(ch));
    }
  }
  if (handle_corpse) {
    for (tobj = obj->contains; tobj; tobj = next_obj) {
      next_obj = tobj->next_content;
      obj_from_obj(tobj);
      obj_to_char(tobj, mob);
    }
    extract_obj(obj);
  }
}


void mag_points(int level, struct char_data * ch, struct char_data * victim,
		     int spellnum, int savetype)
{
  int hit = 0;
  int move = 0;

  if (victim == NULL)
    return;

  switch (spellnum) {
  case SPELL_CURE_LIGHT:
    hit = dice(1, 8) + 1 + (level >> 2);
    send_to_char("You feel better.\r\n", victim);
    break;
  case SPELL_CURE_CRITIC:
    hit = dice(3, 8) + 3 + (level >> 2);
    send_to_char("You feel a lot better!\r\n", victim);
    break;
  case SPELL_HEAL:
    hit = 100 + dice(3, 8);
    send_to_char("A warm feeling floods your body.\r\n", victim);
    break;
  }
  GET_HIT(victim) = MIN(GET_MAX_HIT(victim), GET_HIT(victim) + hit);
  GET_MOVE(victim) = MIN(GET_MAX_MOVE(victim), GET_MOVE(victim) + move);
}


void mag_unaffects(int level, struct char_data * ch, struct char_data * victim,
		        int spellnum, int type)
{
  int spell = 0;
  char *to_vict = NULL, *to_room = NULL;

  if (victim == NULL)
    return;

  switch (spellnum) {
  case SPELL_CURE_BLIND:
    spell = SPELL_BLINDNESS;
    to_vict = "Your vision returns!";
    break;
  case SPELL_HEAL:
    spell = SPELL_BLINDNESS;
    break;
  case SPELL_REMOVE_POISON:
    spell = SPELL_POISON;
    to_vict = "A warm feeling runs through your body!";
    to_room = "$n looks better.";
    break;
  case SPELL_REMOVE_CURSE:
    spell = SPELL_CURSE;
    to_vict = "You don't feel so unlucky.";
    break;
  default:
    sprintf(buf, "SYSERR: unknown spellnum %d passed to mag_unaffects", spellnum);
    log(buf);
    return;
    break;
  }

  if (affected_by_spell(victim, spell)) {
    affect_from_char(victim, spell);
    if (to_vict != NULL) {
      send_to_char(to_vict, victim);
      send_to_char("\r\n", victim);
    }
    if (to_room != NULL)
      act(to_room, TRUE, victim, NULL, NULL, TO_ROOM);
  } else if (to_vict != NULL)
    send_to_char("Nothing seems to happen.\r\n", ch);
}


void mag_alter_objs(int level, struct char_data * ch, struct obj_data * obj,
		         int spellnum, int savetype)
{
  char *to_char = NULL;

  if (obj == NULL)
    return;

  switch (spellnum) {
    case SPELL_INVISIBLE:
      if (!IS_OBJ_STAT(obj, ITEM_NOINVIS | ITEM_INVISIBLE)) {
        SET_BIT(obj->obj_flags.extra_flags, ITEM_INVISIBLE);
        to_char = "$p turns invisible.";
      }
      break;
    case SPELL_REMOVE_CURSE:
      if (IS_OBJ_STAT(obj, ITEM_NODROP)) {
        REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);
        to_char = "$p briefly glows blue.";
      }
      break;
  }
  if (to_char == NULL)
    send_to_char("Nothing seems to happen.\r\n", ch);
  else
    act(to_char, TRUE, ch, obj, 0, TO_CHAR);
}


void mag_objects(int level, struct char_data * ch, struct obj_data * obj,
		      int spellnum)
{
  switch (spellnum) {
    case SPELL_CREATE_WATER:
    break;
  }
}



void mag_creations(int level, struct char_data * ch, int spellnum)
{
  struct obj_data *tobj;
  int z;

  if (ch == NULL)
    return;
  level = MAX(MIN(level, LVL_IMPL), 1);

  switch (spellnum) {
  case SPELL_CREATE_FOOD:
    z = 7090;
    break;
  default:
    send_to_char("Spell unimplemented, it would seem.\r\n", ch);
    return;
    break;
  }

  if (!(tobj = read_object(z, VIRTUAL))) {
    send_to_char("I seem to have goofed.\r\n", ch);
    sprintf(buf, "SYSERR: spell_creations, spell %d, obj %d: obj not found",
	    spellnum, z);
    log(buf);
    return;
  }
  obj_to_char(tobj, ch);
  act("$n creates $p.", FALSE, ch, tobj, 0, TO_ROOM);
  act("You create $p.", FALSE, ch, tobj, 0, TO_CHAR);
}
