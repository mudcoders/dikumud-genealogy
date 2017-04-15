/***************************************************************************
 *  file: const.c                                          Part of DIKUMUD *
 *  Usage: For constants used by the game.                                 *
 *  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
 *                                                                         *
 *  Copyright (C) 1992, 1993 Michael Chastain, Michael Quan, Mitchell Tse  *
 *  Performance optimization and bug fixes by MERC Industries.             *
 *  You can use our stuff in any way you like whatsoever so long as this   *
 *  copyright notice remains intact.  If you like it please drop a line    *
 *  to mec@garnet.berkeley.edu.                                            *
 *                                                                         *
 *  This is free software and you are benefitting.  We hope that you       *
 *  share your changes too.  What goes around, comes around.               *
 ***************************************************************************/

#include <stdio.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "limits.h"
#include "db.h"

/* Each spell and skill must have a wear off message -Kahn */

const char *spell_wear_off_msg[] =
{
  "RESERVED DB.C",                                             /* 0 */
  "You feel less protected.",
  "!Teleport!",
  "You feel less righteous.",
  "You feel a cloak of blindness disolve.",
  "!Burning Hands!",                                           /* 5 */
  "!Call Lightning",
  "You feel more self-confident.",
  "!Chill Touch!",
  "!Clone!",
  "!Color Spray!",                                             /* 10 */
  "!Control Weather!",
  "!Create Food!",
  "!Create Water!",
  "!Cure Blind!",
  "!Cure Critic!",                                             /* 15 */
  "!Cure Light!",
  "You feel better.",
  "You sense the red in your vision disappear.",
  "The detect invisible wears off.",
  "The detect magic wears off.",                               /* 20 */
  "The detect poison wears off.",
  "!Dispel Evil!",
  "!Earthquake!",
  "!Enchant Weapon!",
  "!Energy Drain!",                                            /* 25 */
  "!Fireball!",
  "!Harm!",
  "!Heal",
  "You feel yourself exposed.",
  "!Lightning Bolt!",                                          /* 30 */
  "!Locate object!",
  "!Magic Missile!",
  "You feel less sick.",
  "You feel less protected.",
  "!Remove Curse!",                                            /* 35 */
  "The white aura around your body fades.",
  "!Shocking Grasp!",
  "You feel les tired.",
  "You feel weaker.",
  "!Summon!",                                                  /* 40 */
  "!Ventriloquate!",
  "!Word of Recall!",
  "!Remove Poison!",
  "You feel less aware of your suroundings.",
  "",  /* NO MESSAGE FOR SNEAK */                              /* 45 */
  "!Hide!",    /* NOTE:  I moved #45- 52 to another place -Kahn */
  "!Steal!",   /* These spaces serve only as fillers            */
  "!Backstab!",
  "!Pick Lock!",
  "!Kick!",                                                    /* 50 */
  "!Bash!",
  "!Rescue!",
  "!Identify!",
  "!Animate Dead!",
  "You are not as fearful now.",                               /* 55 */
  "You slowly float to the ground.",
  "Your light slowly fizzes into nothing.",
  "You can no longer see into a persons aura.",
  "!Dispel Magic!",
  "!Conjure Elemental!",                                       /* 60 */
  "!Cure Serious!",
  "!Cause Light!",
  "!Cause Critical!",
  "!Cause Serious!",
  "!Flamestrike!",                                             /* 65 */
  "Your skin does not feal as hard as it used to.",
  "Your force shield shimmers then fades away.",
  "You feel stronger.",
  "!Mass Invisibility!",  /* Uses the invisibility message */
  "!Acid Blast!",                                              /* 70 */
  "!Gate!",
  "The pink aura around you fades away.",
  "!Faerie Fog!",
  "!Drown!",
  "!Demon Fire!",                                              /* 75 */
  "!Turn Undead!",
  "You can no longer see in the dark.",
  "!Sandstorm!",
  "!Hands Of Wind!",
  "You feel better now.",                                      /* 80 */
  "!Refresh!",
  "!UNUSED!"
};


const int rev_dir[] =
{
    2,
    3,
    0,
    1,
    5,
    4
};

const int movement_loss[]=
{
    1,  /* Inside     */
    2,  /* City       */
    2,  /* Field      */
    3,  /* Forest     */
    4,  /* Hills      */
    6,  /* Mountains  */
  4,  /* Swimming   */
  1   /* Unswimable */
};

const char *dirs[] =
{
    "north",
    "east",
    "south",
    "west",
    "up",
    "down",
    "\n"
};

const char *weekdays[7] =
{
    "the Day of the Moon",
    "the Day of the Bull",
    "the Day of the Deception",
    "the Day of Thunder",
    "the Day of Freedom",
    "the day of the Great Gods",
    "the Day of the Sun"
};

const char *month_name[17] =
{
    "Month of Winter",           /* 0 */
    "Month of the Winter Wolf",
    "Month of the Frost Giant",
    "Month of the Old Forces",
    "Month of the Grand Struggle",
    "Month of the Spring",
    "Month of Nature",
    "Month of Futility",
    "Month of the Dragon",
    "Month of the Sun",
    "Month of the Heat",
    "Month of the Battle",
    "Month of the Dark Shades",
    "Month of the Shadows",
    "Month of the Long Shadows",
    "Month of the Ancient Darkness",
    "Month of the Great Evil"
};

const int sharp[] = {
   0,
   0,
   0,
   1,    /* Slashing */
   0,
   0,
   0,
   0,    /* Bludgeon */
   0,
   0,
   0,
   0
};  /* Pierce   */

const char *where[] =
{
    "<used as light>      ",
    "<worn on finger>     ",
    "<worn on finger>     ",
    "<worn around neck>   ",
    "<worn around neck>   ",
    "<worn on body>       ",
    "<worn on head>       ",
    "<worn on legs>       ",
    "<worn on feet>       ",
    "<worn on hands>      ",
    "<worn on arms>       ",
    "<worn as shield>     ",
    "<worn about body>    ",
    "<worn about waist>   ",
    "<worn around wrist>  ",
    "<worn around wrist>  ",
    "<wielded>            ",
    "<held>               "
};

const char *drinks[]=
{
    "water",
    "beer",
    "wine",
    "ale",
    "dark ale",
    "whisky",
    "lemonade",
    "firebreather",
    "local speciality",
    "slime mold juice",
    "milk",
    "tea",
    "coffee",
    "blood",
    "salt water",
    "coca cola"
};

const char *drinknames[]=
{
    "water",
    "beer",
    "wine",
    "ale",
    "ale",
    "whisky",
    "lemonade",
    "firebreather",
    "local",
    "juice",
    "milk",
    "tea",
    "coffee",
    "blood",
    "salt",
    "cola"
};

const int drink_aff[][3] =
{
    { 0,1,10 },  /* Water    */
    { 3,2,5 },   /* beer     */
    { 5,2,5 },   /* wine     */
    { 2,2,5 },   /* ale      */
    { 1,2,5 },   /* ale      */
    { 6,1,4 },   /* Whiskey  */
    { 0,1,8 },   /* lemonade */
    { 10,0,0 },  /* firebr   */
    { 3,3,3 },   /* local    */
    { 0,4,-8 },  /* juice    */
    { 0,3,6 },
    { 0,1,6 },
    { 0,1,6 },
    { 0,2,-1 },
    { 0,1,-2 },
    { 0,1,5 }
};

const char *color_liquid[]=
{
    "clear",
    "brown",
    "clear",
    "brown",
    "dark",
    "golden",
    "red",
    "green",
    "clear",
    "light green",
    "white",
    "brown",
    "black",
    "red",
    "clear",
    "black"
};

const char *fullness[] =
{
    "less than half ",
    "about half ",
    "more than half ",
    ""
};

#define	K	* 1000
#define M	K K
const int exp_table[36+1] =
{
	    0,
	    1  ,   450  ,   900  ,     2 K,     5 K,	/*  5 */
	   10 K,    20 K,    40 K,    80 K,   160 K,	/* 10 */
	  320 K,   480 K,   640 K,   960 K,  1280 K,	/* 15 */
	 1600 K,  2240 K,  2880 K,  3520 K,  4800 K,	/* 20 */
	    6 M,     8 M,    10 M,    12 M,    15 M,	/* 25 */
	   20 M,    25 M,    30 M,    35 M,    40 M,	/* 30 */
	   50 M,   200 M,   300 M,   400 M,   500 M,	/* 35 */
	0x7FFFFFFF
};
#undef	M
#undef	K

const char *title_table[4][36][2] =
{
    {
	{ "the Man",			"the Woman"			},

	{ "the Apprentice of Magic",	"the Apprentice of Magic"	},
	{ "the Spell Student",		"the Spell Student"		},
	{ "the Scholar of Magic",	"the Scholar of Magic"		},
	{ "the Delver in Spells",	"the Delveress in Spells"	},
	{ "the Medium of Magic",	"the Medium of Magic"		},

	{ "the Scribe of Magic",	"the Scribess of Magic"		},
	{ "the Seer",			"the Seeress"			},
	{ "the Sage",			"the Sage"			},
	{ "the Illusionist",		"the Illusionist"		},
	{ "the Abjurer",		"the Abjuress"			},

	{ "the Invoker",		"the Invoker"			},
	{ "the Enchanter",		"the Enchantress"		},
	{ "the Conjurer",		"the Conjuress"			},
	{ "the Magician",		"the Witch"			},
	{ "the Creator",		"the Creator"			},

	{ "the Savant",			"the Savant"			},
	{ "the Magus",			"the Craftess"			},
	{ "the Wizard",			"the Wizard"			},
	{ "the Warlock",			"the War Witch"		},
	{ "the Sorcerer",			"the Sorceress"		},

	{ "the Elder Sorcerer",		"the Elder Sorceress"		},
	{ "the Grand Sorcerer",		"the Grand Sorceress"		},
	{ "the Great Sorcerer",		"the Great Sorceress"		},
	{ "the Golem Maker",		"the Golem Maker"		},
	{ "the Greater Golem Maker",	"the Greater Golem Maker"	},

	{ "the Demon Summoner",		"the Demon Summoner"		},
	{ "the Greater Demon Summoner",	"the Greater Demon Summoner"	},
	{ "the Dragon Charmer",		"the Dragon Charmer"		},
	{ "the Greater Dragon Charmer",	"the Greater Dragon Charmer"	},
	{ "the Master of all Magic",	"the Master of all Magic"	},

	{ "the Mage Hero",		"the Mage Heroine"		},
	{ "the Immortal Warlock",	"the Immortal Enchantress"	},
	{ "the Deity of Magic",		"the Deity of Magic"		},
	{ "the Supremity of Magic",	"the Supremity of Magic"	},
	{ "the Implementor",		"the Implementress"		}
    },

    {
	{ "the Man",			"the Woman"			},

	{ "the Believer",		"the Believer"			},
	{ "the Attendant",		"the Attendant"			},
	{ "the Acolyte",		"the Acolyte"			},
	{ "the Novice",			"the Novice"			},
	{ "the Missionary",		"the Missionary"		},

	{ "the Adept",			"the Adept"			},
	{ "the Deacon",			"the Deaconess"			},
	{ "the Vicar",			"the Vicaress"			},
	{ "the Priest",			"the Priestess"			},
	{ "the Minister",		"the Lady Minister"		},

	{ "the Canon",			"the Canon"			},
	{ "the Levite",			"the Levitess"			},
	{ "the Curate",			"the Curess"			},
	{ "the Monk",			"the Nun"			},
	{ "the Healer",			"the Healess"			},

	{ "the Chaplain",		"the Chaplain"			},
	{ "the Expositor",		"the Expositress"		},
	{ "the Bishop",			"the Bishop"			},
	{ "the Arch Bishop",		"the Arch Lady of the Church"	},
	{ "the Patriarch",		"the Matriarch"			},

	{ "the Elder Patriarch",	"the Elder Matriarch"		},
	{ "the Grand Patriarch",	"the Grand Matriarch"		},
	{ "the Great Patriarch",	"the Great Matriarch"		},
	{ "the Demon Killer",		"the Demon Killer"		},
	{ "the Greater Demon Killer",	"the Greater Demon Killer"	},

	{ "the Avatar of an Immortal",	"the Avatar of an Immortal"	},
	{ "the Avatar of a Deity",	"the Avatar of a Deity"		},
	{ "the Avatar of a Supremity",	"the Avatar of a Supremity"	},
	{ "the Avatar of Implementors",	"the Avatar of Implementors"	},
	{ "the Master of all Divinity",	"the Mistress of all Divinity"	},

	{ "the Cardinal Hero",		"the Cardinal Heroine"		},
	{ "the Immortal Cardinal",	"the Immortal Cardinal"		},
	{ "the Deity",			"the Deity"			},
	{ "the Supreme Master",		"the Supreme Mistress"		},
	{ "the Implementor",		"the Implementress"		}
    },

    {
	{ "the Man",			"the Woman"			},

	{ "the Pilferer",		"the Pilferess"			},
	{ "the Footpad",		"the Footpad"			},
	{ "the Filcher",		"the Filcheress"		},
	{ "the Pick-Pocket",		"the Pick-Pocket"		},
	{ "the Sneak",			"the Sneak"			},

	{ "the Pincher",		"the Pincheress"		},
	{ "the Cut-Purse",		"the Cut-Purse"			},
	{ "the Snatcher",		"the Snatcheress"		},
	{ "the Sharper",		"the Sharpress"			},
	{ "the Rogue",			"the Rogue"			},

	{ "the Robber",			"the Robber"			},
	{ "the Magsman",		"the Magswoman"			},
	{ "the Highwayman",		"the Highwaywoman"		},
	{ "the Burglar",		"the Burglaress"		},
	{ "the Thief",			"the Thief"			},

	{ "the Knifer",			"the Knifer"			},
	{ "the Quick-Blade",		"the Quick-Blade"		},
	{ "the Killer",			"the Murderess"			},
	{ "the Brigand",		"the Brigand"			},
	{ "the Cut-Throat",		"the Cut-Throat"		},

	{ "the Spy",			"the Spy"			},
	{ "the Grand Spy",		"the Grand Spy"			},
	{ "the Master Spy",		"the Master Spy"		},
	{ "the Assassin",		"the Assassin"			},
	{ "the Greater Assassin",	"the Greater Assassin"		},

	{ "the Crimelord",		"the Crime Mistress"		},
	{ "the Infamous Crimelord",	"the Infamous Crime Mistress"	},
	{ "the Greater Crimelord",	"the Greater Crime Mistress"	},
	{ "the Master Crimelord",	"the Master Crime Mistress"	},
	{ "the Godfather",		"the Godmother"			},

	{ "the Assassin Hero",		"the Assassin Heroine"		},
	{ "the Immortal Assasin",	"the Immortal Assasin"		},
	{ "the Deity of Thieves",	"the Deity of Thieves"		},
	{ "the Supreme Master",		"the Supreme Mistress"		},
	{ "the Implementor",		"the Implementress"		}
    },

    {
	{ "the Man",			"the Woman"			},

	{ "the Swordpupil",		"the Swordpupil"		},
	{ "the Recruit",		"the Recruit"			},
	{ "the Sentry",			"the Sentress"			},
	{ "the Fighter",		"the Fighter"			},
	{ "the Soldier",		"the Soldier"			},

	{ "the Warrior",		"the Warrior"			},
	{ "the Veteran",		"the Veteran"			},
	{ "the Swordsman",		"the Swordswoman"		},
	{ "the Fencer",			"the Fenceress"			},
	{ "the Combatant",		"the Combatess"			},

	{ "the Hero",			"the Heroine"			},
	{ "the Myrmidon",		"the Myrmidon"			},
	{ "the Swashbuckler",		"the Swashbuckleress"		},
	{ "the Mercenary",		"the Mercenaress"		},
	{ "the Swordmaster",		"the Swordmistress"		},

	{ "the Lieutenant",		"the Lieutenant"		},
	{ "the Champion",		"the Lady Champion"		},
	{ "the Dragoon",		"the Lady Dragoon"		},
	{ "the Cavalier",		"the Lady Cavalier"		},
	{ "the Knight",			"the Lady Knight"		},

	{ "the Grand Knight",		"the Grand Knight"		},
	{ "the Master Knight",		"the Master Knight"		},
	{ "the Paladin",		"the Paladin"			},
	{ "the Grand Paladin",		"the Grand Paladin"		},
	{ "the Demon Slayer",		"the Demon Slayer"		},

	{ "the Greater Demon Slayer",	"the Greater Demon Slayer"	},
	{ "the Dragon Slayer",		"the Dragon Slayer"		},
	{ "the Greater Dragon Slayer",	"the Greater Dragon Slayer"	},
	{ "the Underlord",		"the Underlord"			},
	{ "the Overlord",		"the Overlord"			},

	{ "the Knight Hero",		"the Knight Heroine"		},
	{ "the Immortal Warlord",	"the Immortal Lady of War"	},
	{ "the Deity of War",		"the Deity of War"		},
	{ "the Supreme Master of War",	"the Supreme Mistress of War"	},
	{ "the Implementor",		"the Implementress"		}
    }
};


const char *item_types[] =
{
    "UNDEFINED",
    "LIGHT",
    "SCROLL",
    "WAND",
    "STAFF",
    "WEAPON",
    "FIRE WEAPON",
    "MISSILE",
    "TREASURE",
    "ARMOR",
    "POTION",
    "WORN",
    "OTHER",
    "TRASH",
    "TRAP",
    "CONTAINER",
    "NOTE",
    "LIQUID CONTAINER",
    "KEY",
    "FOOD",
    "MONEY",
    "PEN",
    "BOAT",
    "\n"
};

const char *wear_bits[] =
{
    "TAKE",
    "FINGER",
    "NECK",
    "BODY",
    "HEAD",
    "LEGS",
    "FEET",
    "HANDS",
    "ARMS",
    "SHIELD",
    "ABOUT",
    "WAISTE",
    "WRIST",
    "WIELD",
    "HOLD",
    "THROW",
    "LIGHT-SOURCE",
    "\n"
};

const char *extra_bits[] =
{
    "GLOW",
    "HUM",
    "DARK",
    "LOCK",
    "EVIL",
    "INVISIBLE",
    "MAGIC",
    "NODROP",
    "BLESS",
    "ANTI-GOOD",
    "ANTI-EVIL",
    "ANTI-NEUTRAL",
    "\n"
};

const char *room_bits[] =
{
    "DARK",
    "DEATH",
    "NO_MOB",
    "INDOORS",
    "LAWFULL",
    "NEUTRAL",
    "CHAOTOC",
    "NO_MAGIC",
    "TUNNEL",
    "PRIVATE",
	"SAFE",
    "\n"
};

const char *exit_bits[] =
{
    "IS-DOOR",
    "CLOSED",
    "LOCKED",
    "\n"
};

const char *sector_types[] =
{
    "Inside",
    "City",
    "Field",
    "Forest",
    "Hills",
    "Mountains",
    "Water Swim",
    "Water NoSwim",
    "\n"
};

const char *equipment_types[] =
{
    "Special",
    "Worn on right finger",
    "Worn on left finger",
    "First worn around Neck",
    "Second worn around Neck",
    "Worn on body",
    "Worn on head",
    "Worn on legs",
    "Worn on feet",
    "Worn on hands",
    "Worn on arms",
    "Worn as shield",
    "Worn about body",
    "Worn around waiste",
    "Worn around right wrist",
    "Worn around left wrist",
    "Wielded",
    "Held",
    "\n"
};

/* Should be in exact correlation as the AFF types -Kahn */

const char *affected_bits[] =
{
    "BLIND",
    "INVISIBLE",
    "DETECT-EVIL",
    "DETECT-INVISIBLE",
    "DETECT-MAGIC",
    "SENCE-LIFE",
    "HOLD",
    "SANCTUARY",
    "GROUP",
    "UNUSED",
    "CURSE",
    "FLAMING-HANDS",
    "POISON",
    "PROTECT-EVIL",
    "PARALYSIS",
    "MORDENS-SWORD",
    "FLAMING-SWORD",
    "SLEEP",
    "DODGE",
    "SNEAK",
    "HIDE",
    "FEAR",
    "CHARM",
    "FOLLOW",
    "WIMPY",
    "INFARED",
    "THIEF",
    "KILLER",
    "FLYING",
    "PLAGUE",
    "\n"
};

/* Should be in exact correlation as the APPLY types -Kahn */

const char *apply_types[] =
{
    "NONE",
    "STR",
    "DEX",
    "INT",
    "WIS",
    "CON",
    "SEX",
    "CLASS",
    "LEVEL",
    "AGE",
    "CHAR_WEIGHT",
    "CHAR_HEIGHT",
    "MANA",
    "HIT_POINTS",
    "MOVE",
    "GOLD",
    "EXP",
    "ARMOR",
    "HITROLL",
    "DAMROLL",
    "SAVING_PARA",
    "SAVING_ROD",
    "SAVING_PETRI",
    "SAVING_BREATH",
    "SAVING_SPELL",
    "\n"
};

const char *pc_class_types[] =
{
    "UNDEFINED",
    "Magic User",
    "Cleric",
    "Thief",
    "Warrior",
    "\n"
};

const char *npc_class_types[] =
{
    "Normal",
    "Undead",
    "\n"
};

const char *action_bits[] =
{
    "SPEC",
    "SENTINEL",
    "SCAVENGER",
    "ISNPC",
    "NICE-THIEF",
    "AGGRESSIVE",
    "STAY-ZONE",
    "WIMPY",
    "AGGRESSIVE_EVIL",
    "AGGRESSIVE_GOOD",
    "AGGRESSIVE_NEUTRAL",
    "\n"
};


const char *player_bits[] =
{
    "BRIEF",
    "NOSHOUT",
    "COMPACT",
    "DONTSET",
    "NOTELL",
    "NOEMOTE",
    "LOG",
    "FREEZE",
    "\n"
};


const char *position_types[] =
{
    "Dead",
    "Mortally wounded",
    "Incapasitated",
    "Stunned",
    "Sleeping",
    "Resting",
    "Sitting",
    "Fighting",
    "Standing",
    "\n"
};

const char *connected_types[] =
{
    "Playing",
    "Get name",
    "Confirm name",
    "Read Password",
    "Get new password",
    "Confirm new password",
    "Get sex",
    "Read messages of today",
    "Read Menu",
    "Get extra description",
    "Get class",
    "\n"
};

/* [class], [level] (all) */
const int thaco[4][36] =
{
    {
	100,20,20,19,19,18,18,17,17,16,16,15,15,14,14,13,13,12,12,
	11,11,10,10, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3
    },
    {
	100,20,20,19,18,17,17,16,15,14,14,13,12,11,11,10, 9, 8, 8,
	 7, 6, 5, 5, 4, 3, 2, 2, 1, 0,-1,-1,-2,-3,-4,-4,-5
    },
    {
	100,20,20,19,18,18,17,16,16,15,14,14,13,12,12,11,10,10, 9,
	 8, 8, 7, 6, 6, 5, 4, 4, 3, 2, 2, 1, 0, 0,-1,-2,-2
    },
    {
	100,20,19,18,17,16,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3,
	 2, 1, 0,-1,-2,-3,-4,-5,-6,-7,-8,-9,-9,-9,-9,-9,-9
    }
};

/* [ch] strength apply (all) */
const struct str_app_type str_app[26] =
{
    { -5,-4,   0,  0 },  /* 0  */
    { -5,-4,   3,  1 },  /* 1  */
    { -3,-2,   3,  2 },
    { -3,-1,  10,  3 },  /* 3  */
    { -2,-1,  25,  4 },
    { -2,-1,  55,  5 },  /* 5  */
    { -1, 0,  80,  6 },
    { -1, 0,  90,  7 },
    {  0, 0, 100,  8 },
    {  0, 0, 100,  9 },
    {  0, 0, 115, 10 }, /* 10  */
    {  0, 0, 115, 11 },
    {  0, 0, 140, 12 },
    {  0, 0, 140, 13 }, /* 13  */
    {  0, 1, 170, 14 },
    {  1, 1, 170, 15 }, /* 15  */
    {  1, 2, 195, 16 },
    {  2, 3, 220, 22 },
    {  2, 4, 250, 30 }, /* 18  */
    {  3, 7, 640, 40 },
    {  3, 8, 700, 40 }, /* 20  */
    {  4, 9, 810, 40 },
    {  4,10, 970, 40 },
    {  5,11,1130, 40 },
    {  6,12,1440, 40 },
    {  7,14,1750, 99 } /* 25            */
};

/* [dex] skill apply (thieves only) */
const struct dex_skill_type dex_app_skill[26] =
{
    {-99,-99,-90,-99,-60},   /* 0 */
    {-90,-90,-60,-90,-50},   /* 1 */
    {-80,-80,-40,-80,-45},
    {-70,-70,-30,-70,-40},
    {-60,-60,-30,-60,-35},
    {-50,-50,-20,-50,-30},   /* 5 */
    {-40,-40,-20,-40,-25},
    {-30,-30,-15,-30,-20},
    {-20,-20,-15,-20,-15},
    {-15,-10,-10,-20,-10},
    {-10, -5,-10,-15, -5},   /* 10 */
    { -5,  0, -5,-10,  0},
    {  0,  0,  0, -5,  0},
    {  0,  0,  0,  0,  0},
    {  0,  0,  0,  0,  0},
    {  0,  0,  0,  0,  0},   /* 15 */
    {  0,  5,  0,  0,  0},
    {  5, 10,  0,  5,  5},
    { 10, 15,  5, 10, 10},
    { 15, 20, 10, 15, 15},
    { 15, 20, 10, 15, 15},   /* 20 */
    { 20, 25, 10, 15, 20},
    { 20, 25, 15, 20, 20},
    { 25, 25, 15, 20, 20},
    { 25, 30, 15, 25, 25},
    { 25, 30, 15, 25, 25}    /* 25 */
};

/* [level] backstab multiplyer (thieves only) */
const byte backstab_mult[36] =
{
    1,   /* 0 */
    2,   /* 1 */
    2,
    2,
    2,
    3,   /* 5 */
    3,
    3,
    3,
    4,
    4,   /* 10 */
    4,
    4,
    4,
    5,
    5,   /* 15 */
    5,
    5,
    5,
    5,
    6,   /* 20 */
    6,
    6,
    6,
    6,
    6,   /* 25 */
    6,
    7,
    7,
    7,
    7,   /* 30 */
    7,
    7,
    7,
    7,
    10   /* 35 */
};

/* [dex] apply (all) */
struct dex_app_type dex_app[26] =
{
    {-7,-7, 6},   /* 0 */
    {-6,-6, 5},   /* 1 */
    {-4,-4, 5},
    {-3,-3, 4},
    {-2,-2, 3},
    {-1,-1, 2},   /* 5 */
    { 0, 0, 1},
    { 0, 0, 0},
    { 0, 0, 0},
    { 0, 0, 0},
    { 0, 0, 0},   /* 10 */
    { 0, 0, 0},
    { 0, 0, 0},
    { 0, 0, 0},
    { 0, 0, 0},
    { 0, 0,-1},   /* 15 */
    { 1, 1,-2},
    { 2, 2,-3},
    { 2, 2,-4},
    { 3, 3,-4},
    { 3, 3,-4},   /* 20 */
    { 4, 4,-5},
    { 4, 4,-5},
    { 4, 4,-5},
    { 5, 5,-6},
    { 5, 5,-6}    /* 25 */
};

/* [con] apply (all) */
struct con_app_type con_app[26] =
{
    {-4,20},   /* 0 */
    {-3,25},   /* 1 */
    {-2,30},
    {-2,35},
    {-1,40},
    {-1,45},   /* 5 */
    {-1,50},
    { 0,55},
    { 0,60},
    { 0,65},
    { 0,70},   /* 10 */
    { 0,75},
    { 0,80},
    { 0,85},
    { 0,88},
    { 1,90},   /* 15 */
    { 2,95},
    { 2,97},
    { 3,99},
    { 3,99},
    { 4,99},   /* 20 */
    { 5,99},
    { 5,99},
    { 5,99},
    { 6,99},
    { 7,100}   /* 25 */
};

/* [int] apply (all) */
struct int_app_type int_app[26] =
{
    {  3 },
    {  5 },    /* 1 */
    {  7 },
    {  8 },
    {  9 },
    { 10 },   /* 5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },   /* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 30 },
    { 35 },   /* 15 */
    { 40 },
    { 45 },
    { 50 },
    { 53 },
    { 55 },   /* 20 */
    { 56 },
    { 60 },
    { 70 },
    { 80 },
    { 99 },   /* 25 */
};

/* [wis] apply (all) */
struct wis_app_type wis_app[26] =
{
    { 0 },   /* 0 */
    { 0 },   /* 1 */
    { 0 },
    { 0 },
    { 0 },
    { 1 },   /* 5 */
    { 1 },
    { 1 },
    { 1 },
    { 2 },
    { 2 },   /* 10 */
    { 2 },
    { 2 },
    { 2 },
    { 2 },
    { 3 },   /* 15 */
    { 3 },
    { 4 },
    { 5 },   /* 18 */
    { 6 },
    { 6 },   /* 20 */
    { 6 },
    { 6 },
    { 6 },
    { 6 },
    { 6 }  /* 25 */
};
