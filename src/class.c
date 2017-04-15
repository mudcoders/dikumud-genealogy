/* ************************************************************************
*   File: class.c                                       Part of CircleMUD *
*  Usage: Source file for class-specific code                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/*
 * This file attempts to concentrate most of the code which must be changed
 * in order for new classes to be added.  If you're adding a new class,
 * you should go through this entire file from beginning to end and add
 * the appropriate new special cases for your new class.
 */



#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "db.h"
#include "utils.h"
#include "spells.h"
#include "interpreter.h"


/* Names first */

const char *class_abbrevs[] = {
  "Mu",
  "Cl",
  "Th",
  "Wa",
  "\n"
};


const char *pc_class_types[] = {
  "Magic User",
  "Cleric",
  "Thief",
  "Warrior",
  "\n"
};


/* The menu for choosing a class in interpreter.c: */
const char *class_menu =
"\r\n"
"Select a class:\r\n"
"  [C]leric\r\n"
"  [T]hief\r\n"
"  [W]arrior\r\n"
"  [M]agic-user\r\n";



/*
 * The code to interpret a class letter -- used in interpreter.c when a
 * new character is selecting a class and by 'set class' in act.wizard.c.
 */

int parse_class(char arg)
{
  arg = LOWER(arg);

  switch (arg) {
  case 'm':
    return CLASS_MAGIC_USER;
    break;
  case 'c':
    return CLASS_CLERIC;
    break;
  case 'w':
    return CLASS_WARRIOR;
    break;
  case 't':
    return CLASS_THIEF;
    break;
  default:
    return CLASS_UNDEFINED;
    break;
  }
}

/*
 * bitvectors (i.e., powers of two) for each class, mainly for use in
 * do_who and do_users.  Add new classes at the end so that all classes
 * use sequential powers of two (1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4,
 * 1 << 5, etc.
 */

long find_class_bitvector(char arg)
{
  arg = LOWER(arg);

  switch (arg) {
    case 'm':
      return (1 << 0);
      break;
    case 'c':
      return (1 << 1);
      break;
    case 't':
      return (1 << 2);
      break;
    case 'w':
      return (1 << 3);
      break;
    default:
      return 0;
      break;
  }
}


/*
 * These are definitions which control the guildmasters for each class.
 *
 * The first field (top line) controls the highest percentage skill level
 * a character of the class is allowed to attain in any skill.  (After
 * this level, attempts to practice will say "You are already learned in
 * this area."
 * 
 * The second line controls the maximum percent gain in learnedness a
 * character is allowed per practice -- in other words, if the random
 * die throw comes out higher than this number, the gain will only be
 * this number instead.
 *
 * The third line controls the minimu percent gain in learnedness a
 * character is allowed per practice -- in other words, if the random
 * die throw comes out below this number, the gain will be set up to
 * this number.
 * 
 * The fourth line simply sets whether the character knows 'spells'
 * or 'skills'.  This does not affect anything except the message given
 * to the character when trying to practice (i.e. "You know of the
 * following spells" vs. "You know of the following skills"
 */

#define SPELL	0
#define SKILL	1

/* #define LEARNED_LEVEL	0  % known which is considered "learned" */
/* #define MAX_PER_PRAC		1  max percent gain in skill per practice */
/* #define MIN_PER_PRAC		2  min percent gain in skill per practice */
/* #define PRAC_TYPE		3  should it say 'spell' or 'skill'?	*/

int prac_params[4][NUM_CLASSES] = {
  /* MAG	CLE	THE	WAR */
  {95,		95,	85,	80},		/* learned level */
  {100,		100,	12,	12},		/* max per prac */
  {25,		25,	0,	0,},		/* min per pac */
  {SPELL,	SPELL,	SKILL,	SKILL}		/* prac name */
};


/*
 * ...And the appropriate rooms for each guildmaster/guildguard; controls
 * which types of people the various guildguards let through.  i.e., the
 * first line shows that from room 3017, only MAGIC_USERS are allowed
 * to go south.
 */
int guild_info[][3] = {

/* Midgaard */
  {CLASS_MAGIC_USER,	3017,	SCMD_SOUTH},
  {CLASS_CLERIC,	3004,	SCMD_NORTH},
  {CLASS_THIEF,		3027,	SCMD_EAST},
  {CLASS_WARRIOR,	3021,	SCMD_EAST},

/* Brass Dragon */
  {-999 /* all */ ,	5065,	SCMD_WEST},

/* this must go last -- add new guards above! */
{-1, -1, -1}};




/* THAC0 for classes and levels.  (To Hit Armor Class 0) */

/* [class], [level] (all) */
const int thaco[NUM_CLASSES][LVL_IMPL + 1] = {

/* MAGE */
  /* 0                   5                  10                  15	    */
  {100, 20, 20, 20, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16, 15, 15,
  15, 14, 14, 14, 13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 10, 9},
  /* 20                  25                  30		    */

/* CLERIC */
  /* 0                   5                  10                  15	    */
  {100, 20, 20, 20, 18, 18, 18, 16, 16, 16, 14, 14, 14, 12, 12, 12, 10, 10,
  10, 8, 8, 8, 6, 6, 6, 4, 4, 4, 2, 2, 2, 1, 1, 1, 1},
  /* 20             25             30				    */

/* THIEF */
  /* 0                   5                  10                  15	    */
  {100, 20, 20, 19, 19, 18, 18, 17, 17, 16, 16, 15, 15, 14, 13, 13, 12, 12,
  11, 11, 10, 10, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3},
  /* 20              25             30				    */

/* WARRIOR */
  /* 0                   5                  10              15	    */
  {100, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3,
  2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
  /* 20             25             30				    */
};


/*
 * Roll the 6 stats for a character... each stat is made of the sum of
 * the best 3 out of 4 rolls of a 6-sided die.  Each class then decides
 * which priority will be given for the best to worst stats.
 */
void roll_real_abils(struct char_data * ch)
{
  int i, j, k, temp;
  ubyte table[6];
  ubyte rolls[4];

  for (i = 0; i < 6; i++)
    table[i] = 0;

  for (i = 0; i < 6; i++) {

    for (j = 0; j < 4; j++)
      rolls[j] = number(1, 6);

    temp = rolls[0] + rolls[1] + rolls[2] + rolls[3] -
      MIN(rolls[0], MIN(rolls[1], MIN(rolls[2], rolls[3])));

    for (k = 0; k < 6; k++)
      if (table[k] < temp) {
	temp ^= table[k];
	table[k] ^= temp;
	temp ^= table[k];
      }
  }

  ch->real_abils.str_add = 0;

  switch (GET_CLASS(ch)) {
  case CLASS_MAGIC_USER:
    ch->real_abils.intel = table[0];
    ch->real_abils.wis = table[1];
    ch->real_abils.dex = table[2];
    ch->real_abils.str = table[3];
    ch->real_abils.con = table[4];
    ch->real_abils.cha = table[5];
    break;
  case CLASS_CLERIC:
    ch->real_abils.wis = table[0];
    ch->real_abils.intel = table[1];
    ch->real_abils.str = table[2];
    ch->real_abils.dex = table[3];
    ch->real_abils.con = table[4];
    ch->real_abils.cha = table[5];
    break;
  case CLASS_THIEF:
    ch->real_abils.dex = table[0];
    ch->real_abils.str = table[1];
    ch->real_abils.con = table[2];
    ch->real_abils.intel = table[3];
    ch->real_abils.wis = table[4];
    ch->real_abils.cha = table[5];
    break;
  case CLASS_WARRIOR:
    ch->real_abils.str = table[0];
    ch->real_abils.dex = table[1];
    ch->real_abils.con = table[2];
    ch->real_abils.wis = table[3];
    ch->real_abils.intel = table[4];
    ch->real_abils.cha = table[5];
    if (ch->real_abils.str == 18)
      ch->real_abils.str_add = number(0, 100);
    break;
  }
  ch->aff_abils = ch->real_abils;
}


/* Some initializations for characters, including initial skills */
void do_start(struct char_data * ch)
{
  void advance_level(struct char_data * ch);

  GET_LEVEL(ch) = 1;
  GET_EXP(ch) = 1;

  set_title(ch, NULL);
  roll_real_abils(ch);
  ch->points.max_hit = 10;

  switch (GET_CLASS(ch)) {

  case CLASS_MAGIC_USER:
    break;

  case CLASS_CLERIC:
    break;

  case CLASS_THIEF:
    SET_SKILL(ch, SKILL_SNEAK, 10);
    SET_SKILL(ch, SKILL_HIDE, 5);
    SET_SKILL(ch, SKILL_STEAL, 15);
    SET_SKILL(ch, SKILL_BACKSTAB, 10);
    SET_SKILL(ch, SKILL_PICK_LOCK, 10);
    SET_SKILL(ch, SKILL_TRACK, 10);
    break;

  case CLASS_WARRIOR:
    break;
  }

  advance_level(ch);

  GET_HIT(ch) = GET_MAX_HIT(ch);
  GET_MANA(ch) = GET_MAX_MANA(ch);
  GET_MOVE(ch) = GET_MAX_MOVE(ch);

  GET_COND(ch, THIRST) = 24;
  GET_COND(ch, FULL) = 24;
  GET_COND(ch, DRUNK) = 0;

  ch->player.time.played = 0;
  ch->player.time.logon = time(0);
}



/*
 * This function controls the change to maxmove, maxmana, and maxhp for
 * each class every time they gain a level.
 */
void advance_level(struct char_data * ch)
{
  int add_hp = 0, add_mana = 0, add_move = 0, i;

  extern struct wis_app_type wis_app[];
  extern struct con_app_type con_app[];

  add_hp = con_app[GET_CON(ch)].hitp;

  switch (GET_CLASS(ch)) {

  case CLASS_MAGIC_USER:
    add_hp += number(3, 8);
    add_mana = number(GET_LEVEL(ch), (int) (1.5 * GET_LEVEL(ch)));
    add_mana = MIN(add_mana, 10);
    add_move = number(0, 2);
    break;

  case CLASS_CLERIC:
    add_hp += number(5, 10);
    add_mana = number(GET_LEVEL(ch), (int) (1.5 * GET_LEVEL(ch)));
    add_mana = MIN(add_mana, 10);
    add_move = number(0, 2);
    break;

  case CLASS_THIEF:
    add_hp += number(7, 13);
    add_mana = 0;
    add_move = number(1, 3);
    break;

  case CLASS_WARRIOR:
    add_hp += number(10, 15);
    add_mana = 0;
    add_move = number(1, 3);
    break;
  }

  ch->points.max_hit += MAX(1, add_hp);
  ch->points.max_move += MAX(1, add_move);

  if (GET_LEVEL(ch) > 1)
    ch->points.max_mana += add_mana;

  if (GET_CLASS(ch) == CLASS_MAGIC_USER || GET_CLASS(ch) == CLASS_CLERIC)
    GET_PRACTICES(ch) += MAX(2, wis_app[GET_WIS(ch)].bonus);
  else
    GET_PRACTICES(ch) += MIN(2, MAX(1, wis_app[GET_WIS(ch)].bonus));

  if (GET_LEVEL(ch) >= LVL_IMMORT) {
    for (i = 0; i < 3; i++)
      GET_COND(ch, i) = (char) -1;
    SET_BIT(PRF_FLAGS(ch), PRF_HOLYLIGHT);
  }

  save_char(ch, NOWHERE);

  sprintf(buf, "%s advanced to level %d", GET_NAME(ch), GET_LEVEL(ch));
  mudlog(buf, BRF, MAX(LVL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
}


/*
 * This simply calculates the backstab multiplier based on a character's
 * level.  This used to be an array, but was changed to be a function so
 * that it would be easier to add more levels to your MUD.  This doesn't
 * really create a big performance hit because it's not used very often.
 */
int backstab_mult(int level)
{
  if (level <= 0)
    return 1;	  /* level 0 */
  else if (level <= 7)
    return 2;	  /* level 1 - 7 */
  else if (level <= 13)
    return 3;	  /* level 8 - 13 */
  else if (level <= 20)
    return 4;	  /* level 14 - 20 */
  else if (level <= 28)
    return 5;	  /* level 21 - 28 */
  else if (level < LVL_IMMORT)
    return 6;	  /* all remaining mortal levels */
  else
    return 20;	  /* immortals */
}


/*
 * invalid_class is used by handler.c to determine if a piece of equipment is
 * usable by a particular class, based on the ITEM_ANTI_{class} bitvectors.
 */

int invalid_class(struct char_data *ch, struct obj_data *obj) {
  if ((IS_OBJ_STAT(obj, ITEM_ANTI_MAGIC_USER) && IS_MAGIC_USER(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_CLERIC) && IS_CLERIC(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_WARRIOR) && IS_WARRIOR(ch)) ||
      (IS_OBJ_STAT(obj, ITEM_ANTI_THIEF) && IS_THIEF(ch)))
	return 1;
  else
	return 0;
}




/*
 * SPELLS AND SKILLS.  This area defines which spells are assigned to
 * which classes, and the minimum level the character must be to use
 * the spell or skill.
 */
void init_spell_levels(void)
{
  /* MAGES */
  spell_level(SPELL_MAGIC_MISSILE, CLASS_MAGIC_USER, 1);
  spell_level(SPELL_DETECT_INVIS, CLASS_MAGIC_USER, 2);
  spell_level(SPELL_DETECT_MAGIC, CLASS_MAGIC_USER, 2);
  spell_level(SPELL_CHILL_TOUCH, CLASS_MAGIC_USER, 3);
  spell_level(SPELL_INFRAVISION, CLASS_MAGIC_USER, 3);
  spell_level(SPELL_INVISIBLE, CLASS_MAGIC_USER, 4);
  spell_level(SPELL_ARMOR, CLASS_MAGIC_USER, 4);
  spell_level(SPELL_BURNING_HANDS, CLASS_MAGIC_USER, 5);
  spell_level(SPELL_LOCATE_OBJECT, CLASS_MAGIC_USER, 6);
  spell_level(SPELL_STRENGTH, CLASS_MAGIC_USER, 6);
  spell_level(SPELL_SHOCKING_GRASP, CLASS_MAGIC_USER, 7);
  spell_level(SPELL_SLEEP, CLASS_MAGIC_USER, 8);
  spell_level(SPELL_LIGHTNING_BOLT, CLASS_MAGIC_USER, 9);
  spell_level(SPELL_BLINDNESS, CLASS_MAGIC_USER, 9);
  spell_level(SPELL_DETECT_POISON, CLASS_MAGIC_USER, 10);
  spell_level(SPELL_COLOR_SPRAY, CLASS_MAGIC_USER, 11);
  spell_level(SPELL_ENERGY_DRAIN, CLASS_MAGIC_USER, 13);
  spell_level(SPELL_CURSE, CLASS_MAGIC_USER, 14);
  spell_level(SPELL_FIREBALL, CLASS_MAGIC_USER, 15);
  spell_level(SPELL_CHARM, CLASS_MAGIC_USER, 16);
  spell_level(SPELL_ENCHANT_WEAPON, CLASS_MAGIC_USER, 26);


  /* CLERICS */
  spell_level(SPELL_CURE_LIGHT, CLASS_CLERIC, 1);
  spell_level(SPELL_ARMOR, CLASS_CLERIC, 1);
  spell_level(SPELL_CREATE_FOOD, CLASS_CLERIC, 2);
  spell_level(SPELL_CREATE_WATER, CLASS_CLERIC, 2);
  spell_level(SPELL_DETECT_POISON, CLASS_CLERIC, 3);
  spell_level(SPELL_DETECT_ALIGN, CLASS_CLERIC, 4);
  spell_level(SPELL_CURE_BLIND, CLASS_CLERIC, 4);
  spell_level(SPELL_BLESS, CLASS_CLERIC, 5);
  spell_level(SPELL_DETECT_INVIS, CLASS_CLERIC, 6);
  spell_level(SPELL_BLINDNESS, CLASS_CLERIC, 6);
  spell_level(SPELL_INFRAVISION, CLASS_CLERIC, 7);
  spell_level(SPELL_PROT_FROM_EVIL, CLASS_CLERIC, 8);
  spell_level(SPELL_GROUP_ARMOR, CLASS_CLERIC, 9);
  spell_level(SPELL_CURE_CRITIC, CLASS_CLERIC, 9);
  spell_level(SPELL_SUMMON, CLASS_CLERIC, 10);
  spell_level(SPELL_REMOVE_POISON, CLASS_CLERIC, 10);
  spell_level(SPELL_WORD_OF_RECALL, CLASS_CLERIC, 12);
  spell_level(SPELL_EARTHQUAKE, CLASS_CLERIC, 12);
  spell_level(SPELL_DISPEL_EVIL, CLASS_CLERIC, 14);
  spell_level(SPELL_DISPEL_GOOD, CLASS_CLERIC, 14);
  spell_level(SPELL_SANCTUARY, CLASS_CLERIC, 15);
  spell_level(SPELL_CALL_LIGHTNING, CLASS_CLERIC, 15);
  spell_level(SPELL_HEAL, CLASS_CLERIC, 16);
  spell_level(SPELL_CONTROL_WEATHER, CLASS_CLERIC, 17);
  spell_level(SPELL_HARM, CLASS_CLERIC, 19);
  spell_level(SPELL_GROUP_HEAL, CLASS_CLERIC, 22);
  spell_level(SPELL_REMOVE_CURSE, CLASS_CLERIC, 26);


  /* THIEVES */
  spell_level(SKILL_SNEAK, CLASS_THIEF, 1);
  spell_level(SKILL_PICK_LOCK, CLASS_THIEF, 2);
  spell_level(SKILL_BACKSTAB, CLASS_THIEF, 3);
  spell_level(SKILL_STEAL, CLASS_THIEF, 4);
  spell_level(SKILL_HIDE, CLASS_THIEF, 5);
  spell_level(SKILL_TRACK, CLASS_THIEF, 6);


  /* WARRIORS */
  spell_level(SKILL_KICK, CLASS_WARRIOR, 1);
  spell_level(SKILL_RESCUE, CLASS_WARRIOR, 3);
  spell_level(SKILL_TRACK, CLASS_WARRIOR, 9);
  spell_level(SKILL_BASH, CLASS_WARRIOR, 12);
}


/*
 * This is the exp given to implementors -- it must always be greater
 * than the exp required for immortality, plus at least 20,000 or so.
 */
#define EXP_MAX  10000000

/* Function to return the exp required for each class/level */
int level_exp(int class, int level)
{
  if (level > LVL_IMPL || level < 0) {
    log("SYSERR: Requesting exp for invalid level!");
    return 0;
  }

  /*
   * Gods have exp close to EXP_MAX.  This statement should never have to
   * changed, regardless of how many mortal or immortal levels exist.
   */
   if (level > LVL_IMMORT) {
     return EXP_MAX - ((LVL_IMPL-level) * 1000);
   }

  /* Exp required for normal mortals is below */

  switch (class) {

    case CLASS_MAGIC_USER:
    switch (level) {
      case  0: return 0;	break;
      case  1: return 1;	break;
      case  2: return 2500;	break;
      case  3: return 5000;	break;
      case  4: return 10000;	break;
      case  5: return 20000;	break;
      case  6: return 40000;	break;
      case  7: return 60000;	break;
      case  8: return 90000;	break;
      case  9: return 135000;	break;
      case 10: return 250000;	break;
      case 11: return 375000;	break;
      case 12: return 750000;	break;
      case 13: return 1125000;	break;
      case 14: return 1500000;	break;
      case 15: return 1875000;	break;
      case 16: return 2250000;	break;
      case 17: return 2625000;	break;
      case 18: return 3000000;	break;
      case 19: return 3375000;	break;
      case 20: return 3750000;	break;
      case 21: return 4000000;	break;
      case 22: return 4300000;	break;
      case 23: return 4600000;	break;
      case 24: return 4900000;	break;
      case 25: return 5200000;	break;
      case 26: return 5500000;	break;
      case 27: return 5950000;	break;
      case 28: return 6400000;	break;
      case 29: return 6850000;	break;
      case 30: return 7400000;	break;
      /* add new levels here */
      case LVL_IMMORT: return 8000000;	break;
    }
    break;

    case CLASS_CLERIC:
    switch (level) {
      case  0: return 0;	break;
      case  1: return 1;	break;
      case  2: return 1500;	break;
      case  3: return 3000;	break;
      case  4: return 6000;	break;
      case  5: return 13000;	break;
      case  6: return 27500;	break;
      case  7: return 55000;	break;
      case  8: return 110000;	break;
      case  9: return 225000;	break;
      case 10: return 450000;	break;
      case 11: return 675000;	break;
      case 12: return 900000;	break;
      case 13: return 1125000;	break;
      case 14: return 1350000;	break;
      case 15: return 1575000;	break;
      case 16: return 1800000;	break;
      case 17: return 2100000;	break;
      case 18: return 2400000;	break;
      case 19: return 2700000;	break;
      case 20: return 3000000;	break;
      case 21: return 3250000;	break;
      case 22: return 3500000;	break;
      case 23: return 3800000;	break;
      case 24: return 4100000;	break;
      case 25: return 4400000;	break;
      case 26: return 4800000;	break;
      case 27: return 5200000;	break;
      case 28: return 5600000;	break;
      case 29: return 6000000;	break;
      case 30: return 6400000;	break;
      /* add new levels here */
      case LVL_IMMORT: return 7000000;	break;
    }
    break;

    case CLASS_THIEF:
    switch (level) {
      case  0: return 0;	break;
      case  1: return 1;	break;
      case  2: return 1250;	break;
      case  3: return 2500;	break;
      case  4: return 5000;	break;
      case  5: return 10000;	break;
      case  6: return 20000;	break;
      case  7: return 30000;	break;
      case  8: return 70000;	break;
      case  9: return 110000;	break;
      case 10: return 160000;	break;
      case 11: return 220000;	break;
      case 12: return 440000;	break;
      case 13: return 660000;	break;
      case 14: return 880000;	break;
      case 15: return 1100000;	break;
      case 16: return 1500000;	break;
      case 17: return 2000000;	break;
      case 18: return 2500000;	break;
      case 19: return 3000000;	break;
      case 20: return 3500000;	break;
      case 21: return 3650000;	break;
      case 22: return 3800000;	break;
      case 23: return 4100000;	break;
      case 24: return 4400000;	break;
      case 25: return 4700000;	break;
      case 26: return 5100000;	break;
      case 27: return 5500000;	break;
      case 28: return 5900000;	break;
      case 29: return 6300000;	break;
      case 30: return 6650000;	break;
      /* add new levels here */
      case LVL_IMMORT: return 7000000;	break;
    }
    break;

    case CLASS_WARRIOR:
    switch (level) {
      case  0: return 0;	break;
      case  1: return 1;	break;
      case  2: return 2000;	break;
      case  3: return 4000;	break;
      case  4: return 8000;	break;
      case  5: return 16000;	break;
      case  6: return 32000;	break;
      case  7: return 64000;	break;
      case  8: return 125000;	break;
      case  9: return 250000;	break;
      case 10: return 500000;	break;
      case 11: return 750000;	break;
      case 12: return 1000000;	break;
      case 13: return 1250000;	break;
      case 14: return 1500000;	break;
      case 15: return 1850000;	break;
      case 16: return 2200000;	break;
      case 17: return 2550000;	break;
      case 18: return 2900000;	break;
      case 19: return 3250000;	break;
      case 20: return 3600000;	break;
      case 21: return 3900000;	break;
      case 22: return 4200000;	break;
      case 23: return 4500000;	break;
      case 24: return 4800000;	break;
      case 25: return 5150000;	break;
      case 26: return 5500000;	break;
      case 27: return 5950000;	break;
      case 28: return 6400000;	break;
      case 29: return 6850000;	break;
      case 30: return 7400000;	break;
      /* add new levels here */
      case LVL_IMMORT: return 8000000;	break;
    }
    break;
  }

  /*
   * This statement should never be reached if the exp tables in this function
   * are set up properly.  If you see exp of 123456 then the tables above are
   * incomplete -- so, complete them!
   */
  log("SYSERR: XP tables not set up correctly in class.c!");
  return 123456;
}


/* 
 * Default titles of male characters.
 */
char *title_male(int class, int level)
{
  if (level <= 0 || level > LVL_IMPL)
    return "the Man";
  if (level == LVL_IMPL)
    return "the Implementor";

  switch (class) {

    case CLASS_MAGIC_USER:
    switch (level) {
      case  1: return "the Apprentice of Magic"; break;
      case  2: return "the Spell Student"; break;
      case  3: return "the Scholar of Magic"; break;
      case  4: return "the Delver in Spells"; break;
      case  5: return "the Medium of Magic"; break;
      case  6: return "the Scribe of Magic"; break;
      case  7: return "the Seer"; break;
      case  8: return "the Sage"; break;
      case  9: return "the Illusionist"; break;
      case 10: return "the Abjurer"; break;
      case 11: return "the Invoker"; break;
      case 12: return "the Enchanter"; break;
      case 13: return "the Conjurer"; break;
      case 14: return "the Magician"; break;
      case 15: return "the Creator"; break;
      case 16: return "the Savant"; break;
      case 17: return "the Magus"; break;
      case 18: return "the Wizard"; break;
      case 19: return "the Warlock"; break;
      case 20: return "the Sorcerer"; break;
      case 21: return "the Necromancer"; break;
      case 22: return "the Thaumaturge"; break;
      case 23: return "the Student of the Occult"; break;
      case 24: return "the Disciple of the Uncanny"; break;
      case 25: return "the Minor Elemental"; break;
      case 26: return "the Greater Elemental"; break;
      case 27: return "the Crafter of Magics"; break;
      case 28: return "the Shaman"; break;
      case 29: return "the Keeper of Talismans"; break;
      case 30: return "the Archmage"; break;
      case LVL_IMMORT: return "the Immortal Warlock"; break;
      case LVL_GOD: return "the Avatar of Magic"; break;
      case LVL_GRGOD: return "the God of Magic"; break;
      default: return "the Mage"; break;
    }
    break;

    case CLASS_CLERIC:
    switch (level) {
      case  1: return "the Believer"; break;
      case  2: return "the Attendant"; break;
      case  3: return "the Acolyte"; break;
      case  4: return "the Novice"; break;
      case  5: return "the Missionary"; break;
      case  6: return "the Adept"; break;
      case  7: return "the Deacon"; break;
      case  8: return "the Vicar"; break;
      case  9: return "the Priest"; break;
      case 10: return "the Minister"; break;
      case 11: return "the Canon"; break;
      case 12: return "the Levite"; break;
      case 13: return "the Curate"; break;
      case 14: return "the Monk"; break;
      case 15: return "the Healer"; break;
      case 16: return "the Chaplain"; break;
      case 17: return "the Expositor"; break;
      case 18: return "the Bishop"; break;
      case 19: return "the Arch Bishop"; break;
      case 20: return "the Patriarch"; break;
      /* no one ever thought up these titles 21-30 */
      case LVL_IMMORT: return "the Immortal Cardinal"; break;
      case LVL_GOD: return "the Inquisitor"; break;
      case LVL_GRGOD: return "the God of good and evil"; break;
      default: return "the Cleric"; break;
    }
    break;

    case CLASS_THIEF:
    switch (level) {
      case  1: return "the Pilferer"; break;
      case  2: return "the Footpad"; break;
      case  3: return "the Filcher"; break;
      case  4: return "the Pick-Pocket"; break;
      case  5: return "the Sneak"; break;
      case  6: return "the Pincher"; break;
      case  7: return "the Cut-Purse"; break;
      case  8: return "the Snatcher"; break;
      case  9: return "the Sharper"; break;
      case 10: return "the Rogue"; break;
      case 11: return "the Robber"; break;
      case 12: return "the Magsman"; break;
      case 13: return "the Highwayman"; break;
      case 14: return "the Burglar"; break;
      case 15: return "the Thief"; break;
      case 16: return "the Knifer"; break;
      case 17: return "the Quick-Blade"; break;
      case 18: return "the Killer"; break;
      case 19: return "the Brigand"; break;
      case 20: return "the Cut-Throat"; break;
      /* no one ever thought up these titles 21-30 */
      case LVL_IMMORT: return "the Immortal Assasin"; break;
      case LVL_GOD: return "the Demi God of thieves"; break;
      case LVL_GRGOD: return "the God of thieves and tradesmen"; break;
      default: return "the Thief"; break;
    }
    break;

    case CLASS_WARRIOR:
    switch(level) {
      case  1: return "the Swordpupil"; break;
      case  2: return "the Recruit"; break;
      case  3: return "the Sentry"; break;
      case  4: return "the Fighter"; break;
      case  5: return "the Soldier"; break;
      case  6: return "the Warrior"; break;
      case  7: return "the Veteran"; break;
      case  8: return "the Swordsman"; break;
      case  9: return "the Fencer"; break;
      case 10: return "the Combatant"; break;
      case 11: return "the Hero"; break;
      case 12: return "the Myrmidon"; break;
      case 13: return "the Swashbuckler"; break;
      case 14: return "the Mercenary"; break;
      case 15: return "the Swordmaster"; break;
      case 16: return "the Lieutenant"; break;
      case 17: return "the Champion"; break;
      case 18: return "the Dragoon"; break;
      case 19: return "the Cavalier"; break;
      case 20: return "the Knight"; break;
      /* no one ever thought up these titles 21-30 */
      case LVL_IMMORT: return "the Immortal Warlord"; break;
      case LVL_GOD: return "the Extirpator"; break;
      case LVL_GRGOD: return "the God of war"; break;
      default: return "the Warrior"; break;
    }
    break;
  }

  /* Default title for classes which do not have titles defined */
  return "the Classless";
}


/* 
 * Default titles of female characters.
 */
char *title_female(int class, int level)
{
  if (level <= 0 || level > LVL_IMPL)
    return "the Woman";
  if (level == LVL_IMPL)
    return "the Implementress";

  switch (class) {

    case CLASS_MAGIC_USER:
    switch (level) {
      case  1: return "the Apprentice of Magic"; break;
      case  2: return "the Spell Student"; break;
      case  3: return "the Scholar of Magic"; break;
      case  4: return "the Delveress in Spells"; break;
      case  5: return "the Medium of Magic"; break;
      case  6: return "the Scribess of Magic"; break;
      case  7: return "the Seeress"; break;
      case  8: return "the Sage"; break;
      case  9: return "the Illusionist"; break;
      case 10: return "the Abjuress"; break;
      case 11: return "the Invoker"; break;
      case 12: return "the Enchantress"; break;
      case 13: return "the Conjuress"; break;
      case 14: return "the Witch"; break;
      case 15: return "the Creator"; break;
      case 16: return "the Savant"; break;
      case 17: return "the Craftess"; break;
      case 18: return "the Wizard"; break;
      case 19: return "the War Witch"; break;
      case 20: return "the Sorceress"; break;
      case 21: return "the Necromancress"; break;
      case 22: return "the Thaumaturgess"; break;
      case 23: return "the Student of the Occult"; break;
      case 24: return "the Disciple of the Uncanny"; break;
      case 25: return "the Minor Elementress"; break;
      case 26: return "the Greater Elementress"; break;
      case 27: return "the Crafter of Magics"; break;
      case 28: return "Shaman"; break;
      case 29: return "the Keeper of Talismans"; break;
      case 30: return "Archwitch"; break;
      case LVL_IMMORT: return "the Immortal Enchantress"; break;
      case LVL_GOD: return "the Empress of Magic"; break;
      case LVL_GRGOD: return "the Goddess of Magic"; break;
      default: return "the Witch"; break;
    }
    break;

    case CLASS_CLERIC:
    switch (level) {
      case  1: return "the Believer"; break;
      case  2: return "the Attendant"; break;
      case  3: return "the Acolyte"; break;
      case  4: return "the Novice"; break;
      case  5: return "the Missionary"; break;
      case  6: return "the Adept"; break;
      case  7: return "the Deaconess"; break;
      case  8: return "the Vicaress"; break;
      case  9: return "the Priestess"; break;
      case 10: return "the Lady Minister"; break;
      case 11: return "the Canon"; break;
      case 12: return "the Levitess"; break;
      case 13: return "the Curess"; break;
      case 14: return "the Nunne"; break;
      case 15: return "the Healess"; break;
      case 16: return "the Chaplain"; break;
      case 17: return "the Expositress"; break;
      case 18: return "the Bishop"; break;
      case 19: return "the Arch Lady of the Church"; break;
      case 20: return "the Matriarch"; break;
      /* no one ever thought up these titles 21-30 */
      case LVL_IMMORT: return "the Immortal Priestess"; break;
      case LVL_GOD: return "the Inquisitress"; break;
      case LVL_GRGOD: return "the Goddess of good and evil"; break;
      default: return "the Cleric"; break;
    }
    break;

    case CLASS_THIEF:
    switch (level) {
      case  1: return "the Pilferess"; break;
      case  2: return "the Footpad"; break;
      case  3: return "the Filcheress"; break;
      case  4: return "the Pick-Pocket"; break;
      case  5: return "the Sneak"; break;
      case  6: return "the Pincheress"; break;
      case  7: return "the Cut-Purse"; break;
      case  8: return "the Snatcheress"; break;
      case  9: return "the Sharpress"; break;
      case 10: return "the Rogue"; break;
      case 11: return "the Robber"; break;
      case 12: return "the Magswoman"; break;
      case 13: return "the Highwaywoman"; break;
      case 14: return "the Burglaress"; break;
      case 15: return "the Thief"; break;
      case 16: return "the Knifer"; break;
      case 17: return "the Quick-Blade"; break;
      case 18: return "the Murderess"; break;
      case 19: return "the Brigand"; break;
      case 20: return "the Cut-Throat"; break;
      case 34: return "the Implementress"; break;
      /* no one ever thought up these titles 21-30 */
      case LVL_IMMORT: return "the Immortal Assasin"; break;
      case LVL_GOD: return "the Demi Goddess of thieves"; break;
      case LVL_GRGOD: return "the Goddess of thieves and tradesmen"; break;
      default: return "the Thief"; break;
    }
    break;

    case CLASS_WARRIOR:
    switch(level) {
      case  1: return "the Swordpupil"; break;
      case  2: return "the Recruit"; break;
      case  3: return "the Sentress"; break;
      case  4: return "the Fighter"; break;
      case  5: return "the Soldier"; break;
      case  6: return "the Warrior"; break;
      case  7: return "the Veteran"; break;
      case  8: return "the Swordswoman"; break;
      case  9: return "the Fenceress"; break;
      case 10: return "the Combatess"; break;
      case 11: return "the Heroine"; break;
      case 12: return "the Myrmidon"; break;
      case 13: return "the Swashbuckleress"; break;
      case 14: return "the Mercenaress"; break;
      case 15: return "the Swordmistress"; break;
      case 16: return "the Lieutenant"; break;
      case 17: return "the Lady Champion"; break;
      case 18: return "the Lady Dragoon"; break;
      case 19: return "the Cavalier"; break;
      case 20: return "the Lady Knight"; break;
      /* no one ever thought up these titles 21-30 */
      case LVL_IMMORT: return "the Immortal Lady of War"; break;
      case LVL_GOD: return "the Queen of Destruction"; break;
      case LVL_GRGOD: return "the Goddess of war"; break;
      default: return "the Warrior"; break;
    }
    break;
  }

  /* Default title for classes which do not have titles defined */
  return "the Classless";
}

