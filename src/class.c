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



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

/* New Sparta */
  {CLASS_MAGIC_USER,	21075,	SCMD_NORTH},
  {CLASS_CLERIC,	21019,	SCMD_WEST},
  {CLASS_THIEF,		21014,	SCMD_SOUTH},
  {CLASS_WARRIOR,	21023,	SCMD_SOUTH},

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



/* Names of class/levels and exp required for each level */

const struct title_type titles[NUM_CLASSES][LVL_IMPL + 1] = {
  {{"the Man", "the Woman", 0},
  {"the Apprentice of Magic", "the Apprentice of Magic", 1},
  {"the Spell Student", "the Spell Student", 2500},
  {"the Scholar of Magic", "the Scholar of Magic", 5000},
  {"the Delver in Spells", "the Delveress in Spells", 10000},
  {"the Medium of Magic", "the Medium of Magic", 20000},
  {"the Scribe of Magic", "the Scribess of Magic", 40000},
  {"the Seer", "the Seeress", 60000},
  {"the Sage", "the Sage", 90000},
  {"the Illusionist", "the Illusionist", 135000},
  {"the Abjurer", "the Abjuress", 250000},
  {"the Invoker", "the Invoker", 375000},
  {"the Enchanter", "the Enchantress", 750000},
  {"the Conjurer", "the Conjuress", 1125000},
  {"the Magician", "the Witch", 1500000},
  {"the Creator", "the Creator", 1875000},
  {"the Savant", "the Savant", 2250000},
  {"the Magus", "the Craftess", 2625000},
  {"the Wizard", "the Wizard", 3000000},
  {"the Warlock", "the War Witch", 3375000},
  {"the Sorcerer", "the Sorceress", 3750000},
  {"the Necromancer", "the Necromancress", 4000000},
  {"the Thaumaturge", "the Thaumaturgess", 4300000},
  {"the Student of the Occult", "the Student of the Occult", 4600000},
  {"the Disciple of the Uncanny", "the Disciple of the Uncanny", 4900000},
  {"the Minor Elemental", "the Minor Elementress", 5200000},
  {"the Greater Elemental", "the Greater Elementress", 5500000},
  {"the Crafter of Magics", "the Crafter of Magics", 5950000},
  {"the Shaman", "Shaman", 6400000},
  {"the Keeper of Talismans", "the Keeper of Talismans", 6850000},
  {"the Archmage", "Archwitch", 7400000},
  {"the Immortal Warlock", "the Immortal Enchantress", 8000000},
  {"the Avatar of Magic", "the Empress of Magic", 9000000},
  {"the God of Magic", "the Goddess of Magic", 9500000},
  {"the Implementor", "the Implementress", 10000000}
  },
  {{"the Man", "the Woman", 0},
  {"the Believer", "the Believer", 1},
  {"the Attendant", "the Attendant", 1500},
  {"the Acolyte", "the Acolyte", 3000},
  {"the Novice", "the Novice", 6000},
  {"the Missionary", "the Missionary", 13000},
  {"the Adept", "the Adept", 27500},
  {"the Deacon", "the Deaconess", 55000},
  {"the Vicar", "the Vicaress", 110000},
  {"the Priest", "the Priestess", 225000},
  {"the Minister", "the Lady Minister", 450000},
  {"the Canon", "the Canon", 675000},
  {"the Levite", "the Levitess", 900000},
  {"the Curate", "the Curess", 1125000},
  {"the Monk", "the Nunne", 1350000},
  {"the Healer", "the Healess", 1575000},
  {"the Chaplain", "the Chaplain", 1800000},
  {"the Expositor", "the Expositress", 2100000},
  {"the Bishop", "the Bishop", 2400000},
  {"the Arch Bishop", "the Arch Lady of the Church", 2700000},
  {"the Patriarch", "the Matriarch", 3000000},
  {"the Patriarch (21)", "the Matriarch (21)", 3250000},
  {"the Patriarch (22)", "the Matriarch (22)", 3500000},
  {"the Patriarch (23)", "the Matriarch (23)", 3800000},
  {"the Patriarch (24)", "the Matriarch (24)", 4100000},
  {"the Patriarch (25)", "the Matriarch (25)", 4400000},
  {"the Patriarch (26)", "the Matriarch (26)", 4800000},
  {"the Patriarch (27)", "the Matriarch (27)", 5200000},
  {"the Patriarch (28)", "the Matriarch (28)", 5600000},
  {"the Patriarch (29)", "the Matriarch (29)", 6000000},
  {"the Patriarch (30)", "the Matriarch (30)", 6400000},
  {"the Immortal Cardinal", "the Immortal Priestess", 7000000},
  {"the Inquisitor", "the Inquisitress", 9000000},
  {"the God of good and evil", "the Goddess of good and evil", 9500000},
  {"the Implementor", "the Implementress", 10000000}
  },
  {{"the Man", "the Woman", 0},
  {"the Pilferer", "the Pilferess", 1},
  {"the Footpad", "the Footpad", 1250},
  {"the Filcher", "the Filcheress", 2500},
  {"the Pick-Pocket", "the Pick-Pocket", 5000},
  {"the Sneak", "the Sneak", 10000},
  {"the Pincher", "the Pincheress", 20000},
  {"the Cut-Purse", "the Cut-Purse", 30000},
  {"the Snatcher", "the Snatcheress", 70000},
  {"the Sharper", "the Sharpress", 110000},
  {"the Rogue", "the Rogue", 160000},
  {"the Robber", "the Robber", 220000},
  {"the Magsman", "the Magswoman", 440000},
  {"the Highwayman", "the Highwaywoman", 660000},
  {"the Burglar", "the Burglaress", 880000},
  {"the Thief", "the Thief", 1100000},
  {"the Knifer", "the Knifer", 1500000},
  {"the Quick-Blade", "the Quick-Blade", 2000000},
  {"the Killer", "the Murderess", 2500000},
  {"the Brigand", "the Brigand", 3000000},
  {"the Cut-Throat", "the Cut-Throat", 3500000},
  {"the Cut-Throat (21)", "the Cut-Throat", 3650000},
  {"the Cut-Throat (22)", "the Cut-Throat", 3800000},
  {"the Cut-Throat (23)", "the Cut-Throat", 4100000},
  {"the Cut-Throat (24)", "the Cut-Throat", 4400000},
  {"the Cut-Throat (25)", "the Cut-Throat", 4700000},
  {"the Cut-Throat (26)", "the Cut-Throat", 5100000},
  {"the Cut-Throat (27)", "the Cut-Throat", 5500000},
  {"the Cut-Throat (28)", "the Cut-Throat", 5900000},
  {"the Cut-Throat (29)", "the Cut-Throat", 6300000},
  {"the Cut-Throat (30)", "the Cut-Throat", 6650000},
  {"the Immortal Assasin", "the Immortal Assasin", 7000000},
  {"the Demi God of thieves", "the Demi Goddess of thieves", 9000000},
  {"the God of thieves and tradesmen", "the Goddess of thieves and tradesmen", 9500000},
  {"the Implementor", "the Implementress", 10000000}
  },
  {{"the Man", "the Woman", 0},
  {"the Swordpupil", "the Swordpupil", 1},
  {"the Recruit", "the Recruit", 2000},
  {"the Sentry", "the Sentress", 4000},
  {"the Fighter", "the Fighter", 8000},
  {"the Soldier", "the Soldier", 16000},
  {"the Warrior", "the Warrior", 32000},
  {"the Veteran", "the Veteran", 64000},
  {"the Swordsman", "the Swordswoman", 125000},
  {"the Fencer", "the Fenceress", 250000},
  {"the Combatant", "the Combatess", 500000},
  {"the Hero", "the Heroine", 750000},
  {"the Myrmidon", "the Myrmidon", 1000000},
  {"the Swashbuckler", "the Swashbuckleress", 1250000},
  {"the Mercenary", "the Mercenaress", 1500000},
  {"the Swordmaster", "the Swordmistress", 1850000},
  {"the Lieutenant", "the Lieutenant", 2200000},
  {"the Champion", "the Lady Champion", 2550000},
  {"the Dragoon", "the Lady Dragoon", 2900000},
  {"the Cavalier", "the Cavalier", 3250000},
  {"the Knight", "the Lady Knight", 3600000},
  {"the Knight (21)", "the Lady Knight (21)", 3900000},
  {"the Knight (22)", "the Lady Knight (22)", 4200000},
  {"the Knight (23)", "the Lady Knight (23)", 4500000},
  {"the Knight (24)", "the Lady Knight (24)", 4800000},
  {"the Knight (25)", "the Lady Knight (25)", 5150000},
  {"the Knight (26)", "the Lady Knight (26)", 5500000},
  {"the Knight (27)", "the Lady Knight (27)", 5950000},
  {"the Knight (28)", "the Lady Knight (28)", 6400000},
  {"the Knight (29)", "the Lady Knight (29)", 6850000},
  {"the Knight (30)", "the Lady Knight (30)", 7400000},
  {"the Immortal Warlord", "the Immortal Lady of War", 8000000},
  {"the Extirpator", "the Queen of Destruction", 9000000},
  {"the God of war", "the Goddess of war", 9500000},
  {"the Implementor", "the Implementress", 10000000}
  }

};

