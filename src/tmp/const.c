/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#define unix 1
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"



/*
 * Guild table.
 */
const	struct	guild_data	guild_table	[]		=
{
  {	"CLANS",	"Moksha Keirkan Isabo", AT_CYAN,       GUILD_NORMAL },
  {     "1045",         "Decklarean Angi Thalador",      AT_YELLOW,      GUILD_NORMAL },
  {	"CREATION",     "Sherf",         	AT_BLUE, GUILD_NORMAL },
  {	"",		"",			0,		0 }
};

const   struct  quest_data      quest_table     []              =
{
/*{     "name",                          vnum,         qp,     level  }*/
  {     "practice",                      0,           10,      1      },
  {     "bamboo training sword",         2426,        100,     1      },
  {     "tassled leather leggings",      2427,        200,     5      },
  {     "ivory-carved anklets",          2428,        300,     10     },
  {     "glowing bauble",                2429,        100,     11     },
  {     "blazing flameshield",           2430,        200,     15     },
  {     "scaly vambraces",               2431,        300,     20     },
  {     "steel visor",                   2432,        100,     21     },
  {     "taloned gauntlets",             2433,        200,     25     },
  {     "brass scope",                   2434,        300,     30     },
  {     "silver ear clasp",              2435,        100,     31     },
  {     "heavy leather boots",           2436,        200,     35     },
  {     "wreath of illusions",           2437,        300,     40     },
  {     "long hafted spear",             2438,        100,     41     },
  {     "arch sorcerer's robe",          2439,        200,     45     },
  {     "engraved golden breastplate",   2440,        300,     50     },
  {     "ankle-sheathed dagger",         2441,        150,     51     },
  {     "band of winds",                 2442,        250,     55     },
  {     "dark stormcloud",               2443,        350,     60     },
  {     "winter fur cloak",              2444,        150,     61     },
  {     "traveller's cloak",             2445,        250,     65     },
  {     "demon's fiery glare",           2446,        350,     70     },
  {     "golden band of energy",         2447,        150,     71     },
  {     "dragon-carved belt buckle",     2448,        250,     75     },
  {     "elbow-bladed sleeves",          2449,        350,     80     },
  {     "heavy dwarven chainmail",       2450,        150,     81     },
  {     "shield of solidified ash",      2451,        250,     85     },
  {     "platinum platemail leggings",   2452,        350,     90     },
  {     "book of arcane arts",           2453,        150,     91     },
  {     "hovering sphere of light",      2454,        250,     95     },
  {     "helm's earguard",               2455,        350,     100    },
  {     "ivory-carved mask",             2456,        150,     101    },
  {     "",                              0,           0,       0      }
};
  

#if 0
const   struct  race_type       race_table      [MAX_RACE]      =
{
/*  {  "Sml", "Full Name", mstr, mint, mwis, mdex, mcon }, */
    {  "Hum", "Human",  0, 0, 0, 0, 0  },
    {  "Elf", "Elf", 0, 1, 0, 0, -1  },
    {  "Dwa", "Dwarf", 0, -2, 0, 0, 2 },
    {  "Pix", "Pixie", -2, 0, 0, 2, 0 },
    {  "Hlf", "Halfling", 0, 0, -2, 2, 0 },
    {  "Drw", "Drow", -1, 0, 0, 1, 0 },
    {  "Eld", "Elder", -3, 0, 3, 0, 0 },
    {  "Ogr", "Ogre", 3, -3, 0, 0, 0 },
    {  "Liz", "Lizardman", 0, 0, 0, -2, 2 },
    {  "Dem", "Demon", 0, 0, -4, 0, 4 },
    {  "Ghl", "Ghoul", 2, -1, -1, -1, 1 },
    {  "Ill", "Illithid", -3, 3, 2, 0, -2 },
    {  "Min", "Minotaur",  1, 0, 2, -4, 1 },
    {  "Tro", "Troll", 0, -1, -1, -2, 4},
    {  "Shd", "Shadow", 0, -2, 0, 2, 0 },
    {  "Tbx", "Tabaxi", 1, -2, -2, 2, 1 }
};
#endif
/* Karen
const   struct  weapon_data	wield_table	[MAX_CLASS]	=
{
    {   0,   
	{ MAG, CLE, THI, WAR, PSI, DRU, RNG, PAL, BRD,
	  VAM, NEC, WWF, MNK }
    },

    {   0,
	{ TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
	  TRUE, TRUE, TRUE, TRUE, TRUE }
    },


*/

const	struct	class_type	class_table	[MAX_CLASS]	=
{
/*  {   "Who", "Long", PRIME_STAT, START_WEAPON,
	GUILD, PRAC_%, tach0_0, tach0_97, HP_MIN, HP_MAX, GETS_MANA,
      { HUM, ELF, DWA, PIX, HLF, DRW, ELD, OGR, LIZ, 
	DEM, GHL, ILL, MIN, TRO, SHD, TBX },
      { MAG, CLE, THI, WAR, PSI, DRU, RNG, PAL, BRD,
	VAM, NEC, WWF, MNK }
      { hit, slice, stab, slash, whip, claw, blast, pound, crush,
        grep, bite, pierce, suction, chop }
    } */
    {
	"Mag", "Mage",  APPLY_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	3018,  95,  18,  -6,   6,  8, TRUE,
      { TRUE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE, FALSE, FALSE,
	FALSE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE },
      { FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
	TRUE, TRUE, TRUE, TRUE },
/* Can class wield this type of weapon? */
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    },

    {
	"Cle", "Cleric",  APPLY_WIS,  OBJ_VNUM_SCHOOL_MACE,
	3003,  95,  18,  -10,  7,  10, TRUE,
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
	FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, TRUE },
      { TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
	TRUE, FALSE, TRUE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }        
    },

    {
	"Thi",  "Thief", APPLY_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	3028,  85,  18,  -14,  8,  13, FALSE,
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE },
      { TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
	TRUE, TRUE, TRUE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    },

    {
	"War",  "Warrior", APPLY_STR,  OBJ_VNUM_SCHOOL_SWORD,
	3022,  85,  18,  -30,  11, 20, FALSE,
      { TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, TRUE,
	TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, TRUE },
      { TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE,
	TRUE, TRUE, TRUE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    },

    {
        "Psi",  "Psionisist", APPLY_WIS,  OBJ_VNUM_SCHOOL_DAGGER,
        3151,  95,  18,  -4,   6,  9, TRUE,
      { TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE,
	TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE },
      { TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE,
	TRUE, TRUE, TRUE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    },
    
    {
	"Dru", "Druid",  APPLY_WIS,  OBJ_VNUM_SCHOOL_MACE,
	3003,  90,  18,  -9,  7,  10, TRUE,
      { TRUE, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE, FALSE,
	FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE,
	TRUE, TRUE, TRUE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    },

    {
	"Rng",  "Ranger", APPLY_CON,  OBJ_VNUM_SCHOOL_SWORD,
	3022,  90,  18,  -18,  14, 18, TRUE,
      { TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, FALSE, FALSE, TRUE,
	FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE,
	TRUE, TRUE, TRUE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    },

    {
	"Pal",  "Paladin", APPLY_STR,  OBJ_VNUM_SCHOOL_SWORD,
	3022,  90,  18,  -16,  10, 16, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,
	FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE,
	TRUE, FALSE, TRUE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    },

    {
	"Brd",  "Bard", APPLY_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	3028,  90,  18,  -14,  9,  13, TRUE,
      { TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
	TRUE, TRUE, TRUE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    },

    {
	"Vam",  "Vampire", APPLY_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	3028,  90,  20,  -14,  10,  15, TRUE,
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
	TRUE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
	FALSE, TRUE, FALSE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    },

    {
        "Nec", "Necromancer", APPLY_INT, OBJ_VNUM_SCHOOL_DAGGER,
	3018,  90,  18,   -6,   8,  10, TRUE,
      { TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE,
	TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, FALSE },
      { TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE,
	TRUE, FALSE, TRUE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    },

    {
	"Wwf", "Werewolf", APPLY_STR, OBJ_VNUM_SCHOOL_CLUB,
	3022,  85,  18,  -25,  15,  19, FALSE,
      { TRUE, FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, TRUE,
	TRUE, TRUE, FALSE, TRUE, TRUE, FALSE, FALSE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
	FALSE, TRUE, FALSE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    },

    {
	"Mnk", "Monk", APPLY_CON, OBJ_VNUM_SCHOOL_CLUB,
	3022,  90,  19, -22,  12,  16, TRUE,
      { TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
	FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
	TRUE, TRUE, TRUE, FALSE },
      { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE, TRUE }  
    }
};

/*
 * Immort Levels
*/
#define L_HER            LEVEL_HERO

#if 0
/*
 * Titles.
 */
char *	const			title_table [ MAX_CLASS+3 ][ MAX_LEVEL+1][ 2 ] =
{
    {
/* 00 */{ "Man",			"Woman"				},

	{ "Apprentice of Magic",	"Apprentice of Magic"		},
	{ "Spell Student",		"Spell Student"			},
	{ "Scholar of Magic",		"Scholar of Magic"		},
	{ "Delver in Spells",		"Delveress in Spells"		},
	{ "Medium of Magic",		"Medium of Magic"		},

	{ "Scribe of Magic",		"Scribess of Magic"		},
	{ "Seer",			"Seeress"			},
	{ "Sage",			"Sage"				},
	{ "Illusionist",		"Illusionist"			},
/* 10 */{ "Abjurer",			"Abjuress"			},

	{ "Invoker",			"Invoker"			},
	{ "Enchanter",			"Enchantress"			},
	{ "Conjurer",			"Conjuress"			},
	{ "Magician",			"Witch"				},
	{ "Creator",			"Creator"			},

	{ "Savant",			"Savant"			},
	{ "Magus",			"Craftess"			},
	{ "Wizard",			"Wizard"			},
	{ "Warlock",			"War Witch"			},
/* 20 */{ "Sorcerer",			"Sorceress"			},

	{ "Elder Sorcerer",		"Elder Sorceress"		},
	{ "Grand Sorcerer",		"Grand Sorceress"		},
	{ "Great Sorcerer",		"Great Sorceress"		},
	{ "Golem Maker",		"Golem Maker"			},
	{ "Greater Golem Maker",	"Greater Golem Maker"		},

	{ "Maker of Stones",		"Maker of Stones",		},
	{ "Maker of Potions",		"Maker of Potions",		},
	{ "Maker of Scrolls",		"Maker of Scrolls",		},
	{ "Maker of Wands",		"Maker of Wands",		},
/* 30 */{ "Maker of Staves",		"Maker of Staves",		},

	{ "Demon Summoner",		"Demon Summoner"		},
	{ "Greater Demon Summoner",	"Greater Demon Summoner"	},
	{ "Dragon Charmer",		"Dragon Charmer"		},
	{ "Greater Dragon Charmer",	"Greater Dragon Charmer"	},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 40 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 50 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 60 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 70 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 80 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 90 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/*100 */{ "Mage Hero",			"Mage Heroine"			},

	{ "Avatar of Magic",            "Avatar of Magic"               },
	{ "Angel of Magic",		"Angel of Magic"		},
	{ "Demi God of Magic",		"Demi Goddess of Magic"		},
	{ "Immortal of Magic",		"Immortal of Magic"		},
	{ "God of Magic",		"God of Magic"			},

	{ "Deity of Magic",		"Deity of Magic"		},
	{ "Implementor",		"Implementress"			},
	{ "Supremity of Magic",         "Supremity of Magic"            }
    },

    {
	{ "Man",			"Woman"				},

	{ "Believer",			"Believer"			},
	{ "Attendant",			"Attendant"			},
	{ "Acolyte",			"Acolyte"			},
	{ "Novice",			"Novice"			},
	{ "Missionary",			"Missionary"			},

	{ "Adept",			"Adept"				},
	{ "Deacon",			"Deaconess"			},
	{ "Vicar",			"Vicaress"			},
	{ "Priest",			"Priestess"			},
	{ "Minister",			"Lady Minister"			},

	{ "Canon",			"Canon"				},
	{ "Levite",			"Levitess"			},
	{ "Curate",			"Curess"			},
	{ "Monk",			"Nun"				},
	{ "Healer",			"Healess"			},

	{ "Chaplain",			"Chaplain"			},
	{ "Expositor",			"Expositress"			},
	{ "Bishop",			"Bishop"			},
	{ "Arch Bishop",		"Arch Lady of the Church"	},
	{ "Patriarch",			"Matriarch"			},

	{ "Elder Patriarch",		"Elder Matriarch"		},
	{ "Grand Patriarch",		"Grand Matriarch"		},
	{ "Great Patriarch",		"Great Matriarch"		},
	{ "Demon Killer",		"Demon Killer"			},
	{ "Greater Demon Killer",	"Greater Demon Killer"		},

	{ "Cardinal of the Sea",	"Cardinal of the Sea"		},
	{ "Cardinal of the Earth",	"Cardinal of the Earth"		},
	{ "Cardinal of the Air",	"Cardinal of the Air"		},
	{ "Cardinal of the Ether",	"Cardinal of the Ether"		},
	{ "Cardinal of the Heavens",	"Cardinal of the Heavens"	},

	{ "Avatar of an Immortal",	"Avatar of an Immortal"		},
	{ "Avatar of a Deity",		"Avatar of a Deity"		},
	{ "Avatar of a Supremity",	"Avatar of a Supremity"		},
	{ "Avatar of an Implementor",	"Avatar of an Implementor"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
/*100 */{ "Holy Hero",			"Holy Heroine"			},

	{ "Avatar of Divinity",         "Avatar of Divinity"            },
	{ "Angel",			"Angel"				},
	{ "Demi God of Divinity", 	"Demi Goddess of Divinity"      },
	{ "Immortal",			"Immortal"			},
	{ "God",			"Goddess"			},

	{ "Supreme God",                "Supreme Goddess"               },
	{ "Implementor",		"Implementress"			},
	{ "Supremity of Divine Will",   "Supremity of Divine Will"      }
    },

    {
	{ "Man",			"Woman"				},

	{ "Pilferer",			"Pilferess"			},
	{ "Footpad",			"Footpad"			},
	{ "Filcher",			"Filcheress"			},
	{ "Pick-Pocket",		"Pick-Pocket"			},
	{ "Sneak",			"Sneak"				},

	{ "Pincher",			"Pincheress"			},
	{ "Cut-Purse",			"Cut-Purse"			},
	{ "Snatcher",			"Snatcheress"			},
	{ "Sharper",			"Sharpress"			},
	{ "Rogue",			"Rogue"				},

	{ "Robber",			"Robber"			},
	{ "Magsman",			"Magswoman"			},
	{ "Highwayman",			"Highwaywoman"			},
	{ "Burglar",			"Burglaress"			},
	{ "Thief",			"Thief"				},

	{ "Knifer",			"Knifer"			},
	{ "Quick-Blade",		"Quick-Blade"			},
	{ "Killer",			"Murderess"			},
	{ "Brigand",			"Brigand"			},
	{ "Cut-Throat",			"Cut-Throat"			},

	{ "Spy",			"Spy"				},
	{ "Grand Spy",			"Grand Spy"			},
	{ "Master Spy",			"Master Spy"			},
	{ "Assassin",			"Assassin"			},
	{ "Greater Assassin",		"Greater Assassin"		},

	{ "Master of Vision",		"Mistress of Vision"		},
	{ "Master of Hearing",		"Mistress of Hearing"		},
	{ "Master of Smell",		"Mistress of Smell"		},
	{ "Master of Taste",		"Mistress of Taste"		},
	{ "Master of Touch",		"Mistress of Touch"		},

	{ "Crime Lord",			"Crime Mistress"		},
	{ "Infamous Crime Lord",	"Infamous Crime Mistress"	},
	{ "Greater Crime Lord",		"Greater Crime Mistress"	},
	{ "Master Crime Lord",		"Master Crime Mistress"		},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
/*100 */{ "Assassin Hero",		"Assassin Heroine"		},

	{ "Avatar of Death",		"Avatar of Death"		},
	{ "Angel of Death",		"Angel of Death"		},
	{ "Demigod of Assassins",       "Demigoddess of Assassins"      },
	{ "Immortal Assassin",		"Immortal Assassin"		},
	{ "God of Assassins",		"Goddess of Assassins"		},

	{ "Deity of Assassins",		"Deity of Assassins"		},
	{ "Implementor",		"Implementress"			},
	{ "Supreme Master",		"Supreme Mistress"		}
    },

    {
	{ "Man",			"Woman"				},

	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},

	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},

	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},

	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},

	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Paladin",			"Paladin"			},
	{ "Grand Paladin",		"Grand Paladin"			},
	{ "Demon Slayer",		"Demon Slayer"			},

	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},

	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
/*100 */{ "Knight Hero",		"Knight Heroine"		},

        { "War's Avatar",               "Death's Avatar"                },
	{ "Angel of War",		"Angel of War"			},
	{ "Demigod of War",             "Demigoddess of War"            },
	{ "Immortal Warlord",		"Immortal Warlord"		},
	{ "God of War",			"Goddess of War"			},

	{ "Deity of War",		"Deity of War"			},
	{ "Implementor",		"Implementress"			},
	{ "Supremity of War",		"Supremity of War"		}
    },

    {
        { "Man",                        "Woman"                         },

        { "Psychic",                    "Psychic",                      },
        { "Medium",                     "Medium",                       },
        { "Gypsy",                      "Gypsy",                        },
        { "Meditator",                  "Meditator",                    },
        { "Mind Prober",                "Mind Prober",                  },

        { "Soul Searcher",              "Soul Searcher",                },
        { "Astral Voyager",             "Astral Voyager",               },
	{ "Seeker",                     "Seeker",                       },
        { "Empath",                     "Empath",                       },
        { "Mind Reader",                "Mind Reader"                   },

        { "Telepath",                   "Telepath",                     },
        { "Mental Adept",               "Mental Adept",                 },
        { "Spoonbender",                "Spoonbender",                  },
        { "Perceptive",                 "Perceptive",                   },
        { "Clever",                     "Clever",                       },

        { "Wise",                       "Wise",                         },
        { "Genius",                     "Genius",                       },
        { "Oracle",                     "Oracle",                       },
        { "Soothsayer",                 "Soothsayer",                   },
        { "Truthteller",                "Truthteller",                  },

        { "Sage",                       "Sage",                         },
        { "Master Psychic",             "Mistress Psychic",             },
        { "Master Meditator",           "Mistress Meditator",           },
        { "Master Empath",              "Mistress Empath",              },
	{ "Master Clairvoyant",         "Mistress Clairvoyant",         },

        { "Master Mind Reader",         "Mistress Mind Reader",         },
        { "Master Telepath",            "Mistress Telepath",            },
        { "Master Spoonbender",         "Mistress Spoonbender",         },
        { "Grand Master Psychic",       "Grand Mistress Psychic",       },
        { "Grand Master Meditator",     "Grand Mistress Meditator",     },

        { "Grand Master Empath",        "Grand Mistress Empath",        },
        { "Grand Master Clairvoyant",   "Grand Mistress Clairvoyant",   },
        { "Grand Master Mind Reader",   "Grand Mistress Mind Reader",   },
        { "Grand Master Telepath",      "Grand Mistress Telepath",      },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

	{ "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
	{ "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
	{ "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

	{ "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },

        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
        { "Grand Master Spoonbender",   "Grand Mistress Spoonbender",   },
/*100 */{ "Psionicist Hero",            "Psionicist Herione",           },

        { "Avatar of the Mind",         "Avatar of the Mind"            },
        { "Psionicist Angel",           "Psionicist Angel"              },
        { "Demigod of Will",            "Demigoddess of Will"           },
        { "Immortal of Will",           "Immortal of Will"              },
        { "God of Psionics",            "Goddess of Psionics"           },

        { "Deity of Psionics",          "Deity of Psionics"             },
        { "Implementor",                "Implementress"                 },
	{ "Supremity of Will",          "Supremity of Will"             }
    },

    {
	{ "Man",			"Woman"				},

	{ "Believer",			"Believer"			},
	{ "Attendant",			"Attendant"			},
	{ "Acolyte",			"Acolyte"			},
	{ "Novice",			"Novice"			},
	{ "Missionary",			"Missionary"			},

	{ "Adept",			"Adept"				},
	{ "Deacon",			"Deaconess"			},
	{ "Vicar",			"Vicaress"			},
	{ "Priest",			"Priestess"			},
	{ "Minister",			"Lady Minister"			},

	{ "Canon",			"Canon"				},
	{ "Levite",			"Levitess"			},
	{ "Curate",			"Curess"			},
	{ "Monk",			"Nun"				},
	{ "Healer",			"Healess"			},

	{ "Chaplain",			"Chaplain"			},
	{ "Expositor",			"Expositress"			},
	{ "Bishop",			"Bishop"			},
	{ "Arch Bishop",		"Arch Lady of the Church"	},
	{ "Patriarch",			"Matriarch"			},

	{ "Elder Patriarch",		"Elder Matriarch"		},
	{ "Grand Patriarch",		"Grand Matriarch"		},
	{ "Great Patriarch",		"Great Matriarch"		},
	{ "Demon Killer",		"Demon Killer"			},
	{ "Greater Demon Killer",	"Greater Demon Killer"		},

	{ "Cardinal of the Sea",	"Cardinal of the Sea"		},
	{ "Cardinal of the Earth",	"Cardinal of the Earth"		},
	{ "Cardinal of the Air",	"Cardinal of the Air"		},
	{ "Cardinal of the Ether",	"Cardinal of the Ether"		},
	{ "Cardinal of the Heavens",	"Cardinal of the Heavens"	},

	{ "Avatar of an Immortal",	"Avatar of an Immortal"		},
	{ "Avatar of a Deity",		"Avatar of a Deity"		},
	{ "Avatar of a Supremity",	"Avatar of a Supremity"		},
	{ "Avatar of an Implementor",	"Avatar of an Implementor"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},

	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
/*100 */{ "Holy Hero",			"Holy Heroine"			},

	{ "Avatar of Divinity",         "Avatar of Divinity"            },
	{ "Angel",			"Angel"				},
	{ "Demi God of Divinity", 	"Demi Goddess of Divinity"      },
	{ "Immortal",			"Immortal"			},
	{ "God",			"Goddess"			},

	{ "Supreme God",                "Supreme Goddess"               },
	{ "Implementor",		"Implementress"			},
	{ "Supremity of Divine Will",   "Supremity of Divine Will"      }
    },

    {
	{ "Man",			"Woman"				},

	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},

	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},

	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},

	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},

	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Paladin",			"Paladin"			},
	{ "Grand Paladin",		"Grand Paladin"			},
	{ "Demon Slayer",		"Demon Slayer"			},

	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},

	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
/*100 */{ "Knight Hero",		"Knight Heroine"		},

	{ "War's Avatar",               "Death's Avatar"                },
	{ "Angel of War",		"Angel of War"			},
	{ "Demigod of War",             "Demigoddess of War"            },
	{ "Immortal Warlord",		"Immortal Warlord"		},
	{ "God of War",			"Goddess of War"			},

	{ "Deity of War",		"Deity of War"			},
	{ "Implementor",		"Implementress"			},
	{ "Supremity of War",		"Supremity of War"		}
    },

    {
	{ "Man",			"Woman"				},

	{ "Pupil of divinity",		"Pupil of divinity"		},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},

	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},

	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},

	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},

	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Paladin",			"Paladin"			},
	{ "Grand Paladin",		"Grand Paladin"			},
	{ "Demon Slayer",		"Demon Slayer"			},

	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},

	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
/*100 */{ "Knight Hero",		"Knight Heroine"		},

        { "War's Avatar",               "Death's Avatar"                },
	{ "Angel of War",		"Angel of War"			},
	{ "Demigod of War",             "Demigoddess of War"            },
	{ "Immortal Warlord",		"Immortal Warlord"		},
	{ "God of War",			"Goddess of War"			},

	{ "Deity of War",		"Deity of War"			},
	{ "Implementor",		"Implementress"			},
	{ "Supremity of War",		"Supremity of War"		}
    },

    {
	{ "Man",			"Woman"				},

	{ "Pilferer",			"Pilferess"			},
	{ "Footpad",			"Footpad"			},
	{ "Filcher",			"Filcheress"			},
	{ "Pick-Pocket",		"Pick-Pocket"			},
	{ "Sneak",			"Sneak"				},

	{ "Pincher",			"Pincheress"			},
	{ "Cut-Purse",			"Cut-Purse"			},
	{ "Snatcher",			"Snatcheress"			},
	{ "Sharper",			"Sharpress"			},
	{ "Rogue",			"Rogue"				},

	{ "Robber",			"Robber"			},
	{ "Magsman",			"Magswoman"			},
	{ "Highwayman",			"Highwaywoman"			},
	{ "Burglar",			"Burglaress"			},
	{ "Thief",			"Thief"				},

	{ "Knifer",			"Knifer"			},
	{ "Quick-Blade",		"Quick-Blade"			},
	{ "Killer",			"Murderess"			},
	{ "Brigand",			"Brigand"			},
	{ "Cut-Throat",			"Cut-Throat"			},

	{ "Spy",			"Spy"				},
	{ "Grand Spy",			"Grand Spy"			},
	{ "Master Spy",			"Master Spy"			},
	{ "Assassin",			"Assassin"			},
	{ "Greater Assassin",		"Greater Assassin"		},

	{ "Master of Vision",		"Mistress of Vision"		},
	{ "Master of Hearing",		"Mistress of Hearing"		},
	{ "Master of Smell",		"Mistress of Smell"		},
	{ "Master of Taste",		"Mistress of Taste"		},
	{ "Master of Touch",		"Mistress of Touch"		},

	{ "Crime Lord",			"Crime Mistress"		},
	{ "Infamous Crime Lord",	"Infamous Crime Mistress"	},
	{ "Greater Crime Lord",		"Greater Crime Mistress"	},
	{ "Master Crime Lord",		"Master Crime Mistress"		},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
/*100 */{ "Assassin Hero",		"Assassin Heroine"		},

	{ "Avatar of Death",		"Avatar of Death"		},
	{ "Angel of Death",		"Angel of Death"		},
	{ "Demigod of Assassins",       "Demigoddess of Assassins"      },
	{ "Immortal Assassin",		"Immortal Assassin"		},
	{ "God of Assassins",		"Goddess of Assassins"		},

	{ "Deity of Assassins",		"Deity of Assassins"		},
	{ "Implementor",		"Implementress"			},
	{ "Supreme Master",		"Supreme Mistress"		}
    },

    {
	{ "Man",			"Woman"				},

	{ "Pilferer",			"Pilferess"			},
	{ "Footpad",			"Footpad"			},
	{ "Filcher",			"Filcheress"			},
	{ "Pick-Pocket",		"Pick-Pocket"			},
	{ "Sneak",			"Sneak"				},

	{ "Pincher",			"Pincheress"			},
	{ "Cut-Purse",			"Cut-Purse"			},
	{ "Snatcher",			"Snatcheress"			},
	{ "Sharper",			"Sharpress"			},
	{ "Rogue",			"Rogue"				},

	{ "Robber",			"Robber"			},
	{ "Magsman",			"Magswoman"			},
	{ "Highwayman",			"Highwaywoman"			},
	{ "Burglar",			"Burglaress"			},
	{ "Thief",			"Thief"				},

	{ "Knifer",			"Knifer"			},
	{ "Quick-Blade",		"Quick-Blade"			},
	{ "Killer",			"Murderess"			},
	{ "Brigand",			"Brigand"			},
	{ "Cut-Throat",			"Cut-Throat"			},

	{ "Spy",			"Spy"				},
	{ "Grand Spy",			"Grand Spy"			},
	{ "Master Spy",			"Master Spy"			},
	{ "Assassin",			"Assassin"			},
	{ "Greater Assassin",		"Greater Assassin"		},

	{ "Master of Vision",		"Mistress of Vision"		},
	{ "Master of Hearing",		"Mistress of Hearing"		},
	{ "Master of Smell",		"Mistress of Smell"		},
	{ "Master of Taste",		"Mistress of Taste"		},
	{ "Master of Touch",		"Mistress of Touch"		},

	{ "Crime Lord",			"Crime Mistress"		},
	{ "Infamous Crime Lord",	"Infamous Crime Mistress"	},
	{ "Greater Crime Lord",		"Greater Crime Mistress"	},
	{ "Master Crime Lord",		"Master Crime Mistress"		},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},

	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
/*100 */{ "Assassin Hero",		"Assassin Heroine"		},

	{ "Avatar of Death",		"Avatar of Death"		},
	{ "Angel of Death",		"Angel of Death"		},
	{ "Demigod of Assassins",       "Demigoddess of Assassins"      },
	{ "Immortal Assassin",		"Immortal Assassin"		},
	{ "God of Assassins",		"Goddess of Assassins"		},

	{ "Deity of Assassins",		"Deity of Assassins"		},
	{ "Implementor",		"Implementress"			},
	{ "Supreme Master",		"Supreme Mistress"		}
    },
    {
/* 00 */{ "Man",			"Woman"				},

	{ "Apprentice of Magic",	"Apprentice of Magic"		},
	{ "Spell Student",		"Spell Student"			},
	{ "Scholar of Magic",		"Scholar of Magic"		},
	{ "Delver in Spells",		"Delveress in Spells"		},
	{ "Medium of Magic",		"Medium of Magic"		},

	{ "Scribe of Magic",		"Scribess of Magic"		},
	{ "Seer",			"Seeress"			},
	{ "Sage",			"Sage"				},
	{ "Illusionist",		"Illusionist"			},
/* 10 */{ "Abjurer",			"Abjuress"			},

	{ "Invoker",			"Invoker"			},
	{ "Enchanter",			"Enchantress"			},
	{ "Conjurer",			"Conjuress"			},
	{ "Magician",			"Witch"				},
	{ "Creator",			"Creator"			},

	{ "Savant",			"Savant"			},
	{ "Magus",			"Craftess"			},
	{ "Wizard",			"Wizard"			},
	{ "Warlock",			"War Witch"			},
/* 20 */{ "Sorcerer",			"Sorceress"			},

	{ "Elder Sorcerer",		"Elder Sorceress"		},
	{ "Grand Sorcerer",		"Grand Sorceress"		},
	{ "Great Sorcerer",		"Great Sorceress"		},
	{ "Golem Maker",		"Golem Maker"			},
	{ "Greater Golem Maker",	"Greater Golem Maker"		},

	{ "Maker of Stones",		"Maker of Stones",		},
	{ "Maker of Potions",		"Maker of Potions",		},
	{ "Maker of Scrolls",		"Maker of Scrolls",		},
	{ "Maker of Wands",		"Maker of Wands",		},
/* 30 */{ "Maker of Staves",		"Maker of Staves",		},

	{ "Demon Summoner",		"Demon Summoner"		},
	{ "Greater Demon Summoner",	"Greater Demon Summoner"	},
	{ "Dragon Charmer",		"Dragon Charmer"		},
	{ "Greater Dragon Charmer",	"Greater Dragon Charmer"	},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 40 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 50 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 60 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 70 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 80 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/* 90 */{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
/*100 */{ "Mage Hero",			"Mage Heroine"			},

	{ "Avatar of Magic",            "Avatar of Magic"               },
	{ "Angel of Magic",		"Angel of Magic"		},
	{ "Demi God of Magic",		"Demi Goddess of Magic"		},
	{ "Immortal of Magic",		"Immortal of Magic"		},
	{ "God of Magic",		"God of Magic"			},

	{ "Deity of Magic",		"Deity of Magic"		},
	{ "Implementor",		"Implementress"			},
	{ "Supremity of Magic",         "Supremity of Magic"            }
  },

    {
	{ "Man",			"Woman"				},

	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},

	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},

	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},

	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},

	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Paladin",			"Paladin"			},
	{ "Grand Paladin",		"Grand Paladin"			},
	{ "Demon Slayer",		"Demon Slayer"			},

	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},

	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
/*100 */{ "Knight Hero",		"Knight Heroine"		},

        { "War's Avatar",               "Death's Avatar"                },
	{ "Angel of War",		"Angel of War"			},
	{ "Demigod of War",             "Demigoddess of War"            },
	{ "Immortal Warlord",		"Immortal Warlord"		},
	{ "God of War",			"Goddess of War"			},

	{ "Deity of War",		"Deity of War"			},
	{ "Implementor",		"Implementress"			},
	{ "Supremity of War",		"Supremity of War"		}
    },

    {
	{ "Man",			"Woman"				},

	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},

	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},

	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},

	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},

	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Paladin",			"Paladin"			},
	{ "Grand Paladin",		"Grand Paladin"			},
	{ "Demon Slayer",		"Demon Slayer"			},

	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},

	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
/*100 */{ "Knight Hero",		"Knight Heroine"		},

        { "War's Avatar",               "Death's Avatar"                },
	{ "Angel of War",		"Angel of War"			},
	{ "Demigod of War",             "Demigoddess of War"            },
	{ "Immortal Warlord",		"Immortal Warlord"		},
	{ "God of War",			"Goddess of War"			},

	{ "Deity of War",		"Deity of War"			},
	{ "Implementor",		"Implementress"			},
	{ "Supremity of War",		"Supremity of War"		}
    },

    {
	{ "Man",			"Woman"				},

	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},

	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},

	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},

	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},

	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Paladin",			"Paladin"			},
	{ "Grand Paladin",		"Grand Paladin"			},
	{ "Demon Slayer",		"Demon Slayer"			},

	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},

	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
/*100 */{ "Knight Hero",		"Knight Heroine"		},

        { "War's Avatar",               "Death's Avatar"                },
	{ "Angel of War",		"Angel of War"			},
	{ "Demigod of War",             "Demigoddess of War"            },
	{ "Immortal Warlord",		"Immortal Warlord"		},
	{ "God of War",			"Goddess of War"			},

	{ "Deity of War",		"Deity of War"			},
	{ "Implementor",		"Implementress"			},
	{ "Supremity of War",		"Supremity of War"		}
    },

    {
	{ "Man",			"Woman"				},

	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},

	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},

	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},

	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},

	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Paladin",			"Paladin"			},
	{ "Grand Paladin",		"Grand Paladin"			},
	{ "Demon Slayer",		"Demon Slayer"			},

	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},

	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
/*100 */{ "Knight Hero",		"Knight Heroine"		},

        { "War's Avatar",               "Death's Avatar"                },
	{ "Angel of War",		"Angel of War"			},
	{ "Demigod of War",             "Demigoddess of War"            },
	{ "Immortal Warlord",		"Immortal Warlord"		},
	{ "God of War",			"Goddess of War"			},

	{ "Deity of War",		"Deity of War"			},
	{ "Implementor",		"Implementress"			},
	{ "Supremity of War",		"Supremity of War"		}
    },

    {
	{ "Man",			"Woman"				},

	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},

	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},

	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},

	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},

	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Paladin",			"Paladin"			},
	{ "Grand Paladin",		"Grand Paladin"			},
	{ "Demon Slayer",		"Demon Slayer"			},

	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},

	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
/*100 */{ "Knight Hero",		"Knight Heroine"		},

        { "War's Avatar",               "Death's Avatar"                },
	{ "Angel of War",		"Angel of War"			},
	{ "Demigod of War",             "Demigoddess of War"            },
	{ "Immortal Warlord",		"Immortal Warlord"		},
	{ "God of War",			"Goddess of War"			},

	{ "Deity of War",		"Deity of War"			},
	{ "Implementor",		"Implementress"			},
	{ "Supremity of War",		"Supremity of War"		}
    }

};
#endif

/* 
 * Wiznet table and prototype for future flag setting
 */
const   struct wiznet_type      wiznet_table    []              =
{
   {    "on",           WIZ_ON,      	LEVEL_HERO },
   {    "prefix",	WIZ_PREFIX,  	LEVEL_HERO },
   {    "ticks",        WIZ_TICKS,   	LEVEL_IMMORTAL },
   {    "general",      WIZ_GENERAL,   	LEVEL_HERO },
   {    "logins",       WIZ_LOGINS,  	LEVEL_HERO },
   {    "sites",        WIZ_SITES,   	LEVEL_IMMORTAL },
   {    "links",        WIZ_LINKS,   	LEVEL_IMMORTAL },
   {	"newbies",	WIZ_NEWBIE,  	LEVEL_HERO },
   {	"spam",		WIZ_SPAM,    	LEVEL_IMMORTAL },
   {    "deaths",       WIZ_DEATHS,  	LEVEL_HERO },
   {    "resets",       WIZ_RESETS,  	LEVEL_IMMORTAL },
   {    "mobdeaths",    WIZ_MOBDEATHS,  LEVEL_IMMORTAL },
   {    "flags",	WIZ_FLAGS,	LEVEL_IMMORTAL },
   {	"penalties",	WIZ_PENALTIES,	L_GOD },
   {	"saccing",	WIZ_SACCING,	L_CON },
   {	"levels",	WIZ_LEVELS,	LEVEL_HERO },
   {	"load",		WIZ_LOAD,	L_CON },
   {	"restore",	WIZ_RESTORE,	L_CON },
   {	"snoops",	WIZ_SNOOPS,	L_CON },
   {	"switches",	WIZ_SWITCHES,	L_SEN },
   {	"secure",	WIZ_SECURE,	L_CON },
   {	"oldlog",	WIZ_OLDLOG,	L_DIR },
   {	NULL,		0,		0  }
};


/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[31]		=
{
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  7 },
    {  0,  0, 100,  8 },
    {  0,  0, 100,  9 },
    {  0,  0, 115, 10 }, /* 10  */
    {  0,  0, 115, 11 },
    {  0,  0, 140, 12 },
    {  0,  0, 140, 13 }, /* 13  */
    {  0,  1, 170, 14 },
    {  1,  1, 170, 15 }, /* 15  */
    {  1,  2, 195, 16 },
    {  2,  3, 220, 22 },
    {  2,  4, 250, 25 }, /* 18  */
    {  3,  5, 400, 30 },
    {  3,  6, 500, 35 }, /* 20  */
    {  4,  7, 600, 40 },
    {  5,  7, 700, 45 },
    {  6,  8, 800, 50 },
    {  8, 10, 900, 55 },
    { 10, 12, 999, 60 },  /* 25   */
    { 12, 14, 999, 60 },  
    { 14, 16, 999, 60 },  
    { 16, 18, 999, 60 },  /* 28 */
    { 18, 20, 999, 60 },  
    { 20, 22, 999, 60 }  /* 30   */

};



const	struct	int_app_type	int_app		[31]		=
{
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 85 },
    { 99 },	/* 25 */
    { 99 },	
    { 99 },	
    { 99 },	
    { 99 },	
    { 99 }	/* 30 */
};



const	struct	wis_app_type	wis_app		[31]		=
{
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 2 },
    { 2 },	/* 10 */
    { 2 },
    { 2 },
    { 2 },
    { 2 },
    { 3 },	/* 15 */
    { 3 },
    { 4 },
    { 4 },	/* 18 */
    { 5 },
    { 5 },	/* 20 */
    { 6 },
    { 7 },
    { 7 },
    { 7 },
    { 8 },	/* 25 */
    { 8 },	
    { 8 },	
    { 9 },	
    { 9 },	
    { 10 }	/* 30 */
};



const	struct	dex_app_type	dex_app		[31]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 65 },
    { - 75 },
    { - 90 },
    { -L_SEN },
    { -120 },    /* 25 */
    { -140 },    
    { -160 },    
    { -180 },    
    { -200 },    
    { -220 }    /* 30 */
};



const	struct	con_app_type	con_app		[31]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 },    /* 25 */
    {  8, 99 },    
    {  9, 99 },    
    {  9, 99 },    
    {  10, 99 },    
    {  10, 99 }    /* 30 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[LIQ_MAX]	=
{
    { "water",			"clear",	{  0, 0, 10 }	},  /*  0 */
    { "beer",			"amber",	{  3, 2,  5 }	},
    { "wine",			"rose",		{  4, 2,  5 }	},
    { "ale",			"brown",	{  2, 2,  5 }	},
    { "dark ale",		"dark",		{  1, 2,  5 }	},

    { "whisky",			"golden",	{  8, 1,  4 }	},  /*  5 */
    { "lemonade",		"pink",		{  0, 1,  8 }	},
    { "firebreather",		"boiling",	{ 10, 0,  0 }	},
    { "local specialty",	"everclear",	{  3, 3,  3 }	},
    { "slime mold juice",	"green",	{  0, 4, -8 }	},

    { "milk",			"white",	{  0, 3,  6 }	},  /* 10 */
    { "tea",			"tan",		{  0, 1,  6 }	},
    { "coffee",			"black",	{  0, 1,  6 }	},
    { "blood",			"red",		{  0, 2, -1 }	},
    { "salt water",		"clear",	{  0, 1, -2 }	},

    { "cola",			"cherry",	{  0, 1,  5 }	}   /* 15 */
};


#define SLOT(s) s
const	struct	skill_type	skill_table	[ MAX_SKILL ]	=
{

/*
 * Magic spells.
 */

/*
    {
	"Name",
	{MAG,CLE,THI,WAR,PSY,DRU,RAN,PAL,BAR,VAM,
	 NEC,WWF,MNK,PRO,PRO,PRO}
	function,		target type,		position,
	gsn,			min mana,	wait,
	"damage name",		"off name",	"room see spell ends", 
	dispelable, SLOT(slot)
    },
*/

  {
    "reserved",
    { 999, 999, 999, 999, 999, 999, 999, 999, 999, 999,
      999, 999, 999, 999, 999, 999 },
    0,			TAR_IGNORE,		POS_STANDING,
    NULL,			0,	 0,
    "",			"", "", FALSE, SLOT(0)
  },

  {
    "acid blast",
    { 20, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			20,	12,
    "&gacid blast&X",		"!Acid Blast!", "", FALSE, SLOT(0)
  },

  {
    "animate dead",
    { L_APP, 33, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 67,
      24, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_animate,  	TAR_OBJ_INV,		POS_STANDING,
    NULL,			100,	24,
    "",			"!Animate Dead!", "", FALSE, SLOT(0)
  },

  {
    "armor",
    {     13,     7, L_APP, L_APP, L_APP, 11, 15, 8, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			5,	12,
    "",
    "You feel less protected.",
    "$n looks less protected.",
    TRUE, SLOT(1)
  },

  {
    "astral walk",
    { 26, 32, L_APP, L_APP,  32, 42, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_astral,		TAR_IGNORE,		POS_STANDING,
    NULL,			50,	12,
    "",			"!Astral Walk!", "", FALSE, SLOT(0)
  },

/*
    "barkskin"
    { L_APP, L_APP, L_APP, L_APP, 18, L_APP, 15, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP},
      spell_barkskin,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
      NULL,                     15,     12,
      "",               "Your skin turns back to it's original texture."
      FALSE,   SLOT(1)
     },
*/
 
  {
    "bless",
    { L_APP, 5, L_APP, L_APP, L_APP, 14, L_APP, 6, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_bless,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			5,	12,
    "",
    "You feel less righteous.",
    "$n looks less righteous.",
    FALSE, SLOT(2)
  },

  {
    "blindness",
    { 18,  28, L_APP, L_APP, L_APP, 60, L_APP, L_APP, L_APP, L_APP,
      55, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_blindness,		5,	12,
    "",
    "You can see again.",
    "$n is no longer blinded",
    TRUE, SLOT(3)
  },

  {
    "bloodbath",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 47,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_blood_bath,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,	        	100,	12,
    "",			"!Blood Bath!", "", FALSE, SLOT(0)
  },

  {
    "burning hands",
    { 9, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      5, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			15,	12,
    "&rbu&Yrni&Wng &rh&Oa&Yn&Wds&X","!Burning Hands!", "", FALSE, SLOT(0)
  },

  {
    "call lightning",
    { L_APP, L_APP, L_APP, L_APP, L_APP, 21, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
    NULL,			15,	12,
    "&Blightning bolt&X",	"!Call Lightning!", "", FALSE, SLOT(0)
  },

  {
    "cause critical",
    { L_APP, 19, L_APP, L_APP, L_APP, L_APP, L_APP, 24, L_APP, L_APP, 
      18, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			20,	12,
    "spell",		"!Cause Critical!", "", FALSE, SLOT(0)
  },

  {
    "cause light",
    { L_APP,     5, L_APP, L_APP, L_APP, 3, L_APP, 8, L_APP, L_APP, 
      3, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			15,	12,
    "&wspell&X",		"!Cause Light!", "", FALSE, SLOT(0)
  },

  {
    "cause serious",
    { L_APP,     13, L_APP, L_APP, L_APP, 21, L_APP, 17, L_APP, L_APP,
      10, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			17,	12,
    "&wspell&X",		"!Cause Serious!", "", FALSE, SLOT(0)
  },

  {
    "change sex",
    { 40, L_APP, L_APP, L_APP, L_APP, 42, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
    NULL,			15,	12,
    "",
    "Your body feels familiar again.",
    "$n looks more like $mself again.",
    TRUE, SLOT(4)
  },

  {
    "charm person",
    {  24, L_APP, L_APP, L_APP, L_APP, 50, 29, L_APP, L_APP, 19,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
    &gsn_charm_person,	5,	12,
    "",
    "You feel more self-confident.",
    "$n regains $s free will.",
    TRUE, SLOT(5)
  },

  {
    "chill touch",
    { 5, L_APP, L_APP, L_APP, L_APP, L_APP, 7, L_APP, L_APP, 7,
      8, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			15,	12,
    "&Cch&wil&Cli&wng &Cto&wu&Cch&X",
    "You feel less cold.",
    "$n looks warmer.", TRUE, SLOT(6)
  },

  {
    "colour spray",
    { 22, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			15,	12,
    "&rc&go&cl&Yo&Gu&Rr &rsp&gr&Ba&Gy&X","!Colour Spray!", "",
FALSE, SLOT(0)
  },

  {
    "continual light",
    { 6, 7, L_APP, L_APP, L_APP, 13, L_APP, 11, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_continual_light,      TAR_IGNORE,             POS_STANDING,
    NULL,			7,	12,
    "",			"!Continual Light!", "", FALSE, SLOT(0)
  },

  {
    "control weather",
    { L_APP, L_APP, L_APP, L_APP, L_APP, 21, L_APP, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_control_weather,      TAR_IGNORE,             POS_STANDING,
    NULL,			25,	12,
    "",			"!Control Weather!", "", FALSE, SLOT(0)
  },

  {
    "create food",
    { L_APP, 12, L_APP, L_APP, L_APP, 26, L_APP, 37, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_create_food,	TAR_IGNORE,		POS_STANDING,
    NULL,			5,	12,
    "",			"!Create Food!", "", FALSE, SLOT(0)
  },

  {
    "create spring",
    { L_APP, L_APP, L_APP, L_APP, L_APP, 20, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_create_spring,	TAR_IGNORE,		POS_STANDING,
    NULL,			20,	12,
    "",			"!Create Spring!", "", FALSE, SLOT(0)
  },

  {
    "create water",
    { L_APP, 7, L_APP, L_APP, L_APP, 13, L_APP, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
    NULL,			5,	12,
    "",			"!Create Water!", "", FALSE, SLOT(0)
  },

  {
    "cure blindness",
    { L_APP, 30, L_APP, L_APP, L_APP, 63, L_APP, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
    NULL,			5,	12,
    "",			"!Cure Blindness!", "", FALSE, SLOT(0)
  },

  {
    "cure critical",
    { L_APP, 17, L_APP, L_APP, L_APP, L_APP, L_APP, 37, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
    NULL,			20,	12,
    "",			"!Cure Critical!", "", FALSE, SLOT(0)
  },

  {
    "cure light",
    { L_APP,     5, L_APP, L_APP, L_APP, 7, 7, 6, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
    NULL,			10,	12,
    "",			"!Cure Light!", "", FALSE, SLOT(0)
  },

  {
    "cure poison",
    { L_APP, 17, L_APP, L_APP, L_APP, 19, L_APP, L_APP, L_APP, L_APP, 
      36, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			5,	12,
    "",			"!Cure Poison!", "", FALSE, SLOT(0)
  },

  {
    "cure serious",
    { L_APP, 13, L_APP, L_APP, L_APP, 17, 19, 15, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
    NULL,			15,	12,
    "",			"!Cure Serious!", "", FALSE, SLOT(0)
  },

  {
    "curse",
    { L_APP, 22, L_APP, L_APP, L_APP, 28, L_APP, L_APP, L_APP, 38, 
      34, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_curse,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_curse,		20,	12,
    "curse",  "The curse wears off.", "$n becomes pure again.",
    TRUE, SLOT(7)
  },

  {
   "detect evil",
    { L_APP,     6, L_APP, L_APP, L_APP, 6, 6, 6, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			5,	12,
    "",			"The red in your vision disappears.", 
    "$n's vision is restored to normal.", TRUE, SLOT(8)
  },

 {
    "detect hidden",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 39, L_APP, L_APP, 23, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			5,	12,
    "",	"You feel less aware of your suroundings.", 
    "$n feels less aware of $s surroundings.", TRUE, SLOT(9)
  },

  {
    "detect invis",
    {    21,   25,    L_APP, L_APP, L_APP, 24, L_APP, L_APP, L_APP, L_APP, 
      15, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			5,	12,
    "",	"You no longer see invisible objects.", 
    "$n's vision returns to normal.", TRUE, SLOT(10)
  },

  {
    "detect magic",
    {     6,     9, L_APP, L_APP, L_APP, 6, L_APP, L_APP, L_APP, 10, 
      6, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			5,	12,
    "",			"The detect magic wears off.", 
    "$n's vision returns to normal.", TRUE, SLOT(11)
  },

  {
    "detect poison",
    { L_APP, 11, 17, L_APP, L_APP, 11, 27, 11, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
    NULL,			5,	12,
    "",			"!Detect Poison!", "", TRUE, SLOT(12)
  },

  {
    "dispel evil",
    { L_APP, 17, L_APP, L_APP, L_APP, L_APP, 41, 19, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			15,	12,
    "dispel &zevil&X",		"!Dispel Evil!", "", FALSE, SLOT(0)
  },

  {
    "dispel magic",
    { 46, 51, L_APP, L_APP, L_APP, 62, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, 62, L_APP, L_APP },
    spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			15,	16,
    "",			"!Dispel Magic!", "", FALSE, SLOT(0)
  },

  {
    "earthquake",
    { L_APP, 17, L_APP, L_APP, L_APP, 17, L_APP, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
    NULL,			15,	12,
    "&Oearthquake&X",		"!Earthquake!", "", FALSE, SLOT(0)
  },

  {
    "enchant weapon",
    { 50, L_APP, L_APP, L_APP, L_APP, 50, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, 57, L_APP, L_APP },
    spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
    NULL,			100,	24,
    "",			"!Enchant Weapon!", "", FALSE, SLOT(0)
  },

  {
    "energy drain",
    { 23, L_APP, L_APP, L_APP, 24, L_APP, L_APP, L_APP, L_APP, 23,
      33, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			35,	12,
    "&zene&wrgy d&Wrain&X",	"!Energy Drain!", "", FALSE, SLOT(0)
  },

  {
    "faerie fire",
    {L_APP, 31, L_APP, L_APP, L_APP, 31, 47, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			5,	12,
    "faerie fire",
    "The pink aura around you fades away.",
    "$n's outline fades.",
    TRUE, SLOT(13)
  },

  {
    "faerie fog",
    {  L_APP, L_APP, L_APP, L_APP, L_APP, 41, 57, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
    NULL,			12,	12,
    "faerie fog",		"!Faerie Fog!", "", FALSE, SLOT(0)
  },

  {
    "fireball",
    { 21, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			15,	12,
    "&Rfireball&X",		"!Fireball!", "", FALSE, SLOT(0)
  },

  {
    "fireshield",
    { 65,  23, L_APP, L_APP, L_APP, 34, 51, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_fireshield,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			75,	12,
    "",
    "The flames engulfing your body burn out.",
    "The flames about $n's body burn out.",
    TRUE, SLOT(14)
  },

  {
    "flamestrike",
    { L_APP, 13, L_APP, L_APP, L_APP, 19, 24, 14, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			20,	12,
    "&rflame&Rstrike",		"!Flamestrike!", "", FALSE, SLOT(0)
  },

  {
    "fly",
    {     9,    12, L_APP, L_APP, L_APP, 14, 14, 12, L_APP, 6,
      14, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			10,	18,
    "",
    "You slowly float to the ground.",
    "$n falls to the ground!",
    TRUE, SLOT(15)
  },

  {
    "gate",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_gate,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
    NULL,			50,	12,
    "",			"!Gate!", "", FALSE, SLOT(0)
  },

  {
    "giant strength",
    { 7, L_APP, L_APP, L_APP, L_APP, 13, 13, 34, L_APP, 15,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			20,	12,
    "",
    "You feel weaker.",
    "$n no longer looks so mighty.", TRUE, SLOT(16)
  },

{
    "eternal intellect",
    { 16, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_eternal_intellect,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       20,     12,
    "",
    "You mind surges with a superior prowess.",
    "$n looks a bit duller.", TRUE, SLOT(16)
  },

  {
    "goodberry",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_goodberry,	TAR_OBJ_INV,	        POS_STANDING,
    NULL,			25,	8,
    "",			"!GOODBERRY!", "", FALSE, SLOT(0)
  },

  {
    "harm",
    { L_APP, 25, L_APP, L_APP, L_APP, 41, 69, 30, L_APP, L_APP, 
      22, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			35,	12,
    "&zharm &wspell&X",		"!Harm!", "", FALSE, SLOT(0)
  },

  {
    "heal",
    { L_APP, 24, L_APP, L_APP, L_APP, 49, L_APP, 36, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
    NULL,			50,	12,
    "",			"!Heal!", "", FALSE, SLOT(0)
  },

  {
    "iceshield",
    { L_APP, 53, L_APP, L_APP, L_APP, 73, L_APP, L_APP, L_APP, 63,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_iceshield,	TAR_CHAR_SELF,  	POS_STANDING,
    NULL,			150,	12,
    "",
    "The icy crust about your body melts away.",
    "The icy crust about $n's body melts to a puddle.",
    TRUE, SLOT(17)
  },

  {
    "icestorm",
    {  31, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_icestorm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			25,	8,
    "&Cicy blast&X",	"!ICESTORM!", "", FALSE, SLOT(0)
  },

  {
    "identify",
    {   10,   10,   25, L_APP,    13, 10, 17, 17, L_APP, 16, 
      11, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_identify,		TAR_OBJ_INV,		POS_STANDING,
    NULL,			12,	24,
    "",			"!Identify!", "", FALSE, SLOT(0)
  },

  {
    "vibrate",
    { L_APP, L_APP, L_APP, L_APP, 30, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_vibrate, 	       TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			75,	0,
    "",
    "Your body stops vibrating.",
    "$n body stops vibrating.", FALSE, SLOT(18)
  },

  {
    "infravision",
    {   6,   9, L_APP, L_APP, L_APP, 9, 9, 9, L_APP, 9,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			5,	18,
    "",			"You no longer see in the dark.", "",
TRUE, SLOT(19)
  },

  {
    "incinerate",
    { 14, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      25, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_incinerate,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
    &gsn_incinerate,	30,	12,
    "&rflames&X","The flames have been extinguished.", 
    "The flames around $n have been extinguished.",TRUE, SLOT(20)
  },

  {
    "invis",
    {  24, L_APP,   L_APP, L_APP, L_APP, 30, 24, L_APP, L_APP, 26,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_invis,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
    &gsn_invis,		5,	12,
    "",
    "You are no longer invisible.",
    "$n fades into existance.",
    TRUE, SLOT(21)
  },

  {
    "know alignment",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
    NULL,			9,	12,
    "",			"!Know Alignment!", "", FALSE, SLOT(0)
  },

  {
    "lightning bolt",
    { 17, L_APP, L_APP, L_APP, L_APP, 18, 43, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			15,	12,
    "&Blightning bolt&X",	"!Lightning Bolt!", "", FALSE, SLOT(0)
  },

  {
    "locate object",
    {     29,    29, L_APP, L_APP, 85, 36, L_APP, L_APP, L_APP, L_APP, 
      36, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_locate_object,	TAR_IGNORE,		POS_STANDING,
    NULL,			20,	18,
    "",			"!Locate Object!", "", FALSE, SLOT(0)
  },

  {
    "magic missile",
    { 3, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			15,	12,
    "&Pmag&pic mis&Psile",	"!Magic Missile!", "", FALSE, SLOT(0)
  },
 
  {
    "mana",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_mana,           TAR_CHAR_DEFENSIVE,      POS_STANDING,
    NULL,                  0, 0,
    "",                    "!Mana!", "", FALSE, SLOT(0)
  },

  {
    "mass invis",
    { 35, L_APP, L_APP, L_APP, L_APP, 35, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
    &gsn_mass_invis,	20,	24,
    "",
    "!Mass Invis!",
    "$n fades into existance.", TRUE, SLOT(22)
  },

  {
    "pass door",
    {    28, L_APP, L_APP, L_APP, L_APP, 27, 48, L_APP, L_APP, L_APP,
      50, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			20,	12,
    "",
    "You feel solid again.",
    "$n becomes soild.", TRUE, SLOT(23)
  },

  {
    "aura of peace",
    { L_APP, 70, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_aura,     	TAR_CHAR_SELF,	POS_STANDING,
    NULL,			100,	12,
    "",
    "The peace aura around your body fades.",
    "The peace aura around $n fades.", FALSE, SLOT(24)
  },

  {
    "permenancy",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_permenancy,	TAR_OBJ_INV,		POS_STANDING,
    NULL,			200,	24,
    "",			"!Permenancy!", "", FALSE, SLOT(0)
  },

  {
    "poison",
    { L_APP, L_APP, 35, L_APP, L_APP, 19, 29, L_APP, L_APP, 9,
      30, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_poison,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
    &gsn_poison,		10,	12,
    "&Gp&go&Gi&gs&Go&gn",
    "You feel less sick.",
    "$n looks better.", TRUE, SLOT(25)
  },

  {
    "portal",
    { 37, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      67, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_portal,		TAR_IGNORE,     	POS_STANDING,
    NULL,   		100,	12,
    "",     		"", "", FALSE, SLOT(0)
  },

  {
    "protection evil",
    { L_APP,  L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_protection,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			5,	12,
    "",
    "You feel less protected.",
    "$n looks less protected.",
    TRUE, SLOT(26)
  },

  {
    "refresh",
    { L_APP,  3, L_APP, L_APP, L_APP, 4, L_APP, 5, L_APP, L_APP,
      3, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			12,	18,
    "refresh",		"!Refresh!", "", FALSE, SLOT(0)
  },

  {
    "remove curse",
    { L_APP, 22, L_APP, L_APP, L_APP, 21, L_APP, 26, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_remove_curse,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			5,	12,
    "",			"!Remove Curse!", "", FALSE, SLOT(0)
  },

  {
    "sanctuary",
    { L_APP, 20, L_APP, L_APP, L_APP, 29, L_APP, 27, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			75,	12,
    "",
    "The white aura around your body fades.",
    "The white aura around $n's body vanishes.", TRUE, SLOT(27)
  },

  {
    "scry",
    {    13, 13, L_APP, L_APP, 9, 24, 31, 27, L_APP, 19,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_scry,		TAR_CHAR_SELF,  	POS_STANDING,
    NULL,			35,	20,
    "",			"Your vision returns to normal.", 
    "$n's vision returns to normal.", TRUE, SLOT(0)
  },

  {
    "shield",
    { 13, L_APP, L_APP, L_APP, L_APP, 16, 21, L_APP, L_APP, 16,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			12,	18,
    "",
    "Your force shield shimmers then fades away.",
    "The shield protecting $n shimmers then fades away.",
    TRUE, SLOT(28)
  },

  {
    "shocking grasp",
    { 10, L_APP, L_APP, L_APP, L_APP, 10, 14, L_APP, L_APP, 12,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			15,	12,
    "&Ysh&Bo&Yck&Bin&Yg g&Bra&Ysp",	"!Shocking Grasp!", "",
FALSE, SLOT(0)
  },

  {
    "shockshield",
    { 79, L_APP, L_APP, L_APP, L_APP, 61, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_shockshield,	TAR_CHAR_SELF,  	POS_STANDING,
    NULL,			100,	12,
    "",
    "The electricity flows into the ground.",
    "The electricity about $n's body flows into the ground.",
    TRUE, SLOT(29)
  },

  {
    "sleep",
    {    23, L_APP, L_APP, L_APP, L_APP, 41, 52, L_APP, L_APP, 46,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
    &gsn_sleep,		15,	12,
    "",			"You feel less tired.", 
    "$n looks much more refreshed.", TRUE, SLOT(30)
  },

  {
    "spell bind",
    { 46, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_spell_bind,	TAR_OBJ_INV,		POS_STANDING,
    NULL,			100,	24,
    "",			"!Spell Bind!", "", FALSE, SLOT(0)
  },

  {
    "stone skin",
    { 27, L_APP, L_APP, L_APP, L_APP, 35, 41, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			12,	18,
    "",
    "Your skin feels soft again.",
    "$n's skin regains its normal texture.",
    TRUE, SLOT(31)
  },

  {
    "summon",
    {  45, 45, L_APP, L_APP, 61, 52, L_APP, L_APP, L_APP, 61,
      45, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_summon,		TAR_IGNORE,		POS_STANDING,
    NULL,			50,	12,
    "",			"!Summon!", "", FALSE, SLOT(0)
  },

  {
    "teleport",
    { 90, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
    NULL,	 		35,	12,
    "",			"!Teleport!", "", FALSE, SLOT(0)
  },

  {
    "ventriloquate",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
    NULL,			5,	12,
    "",			"!Ventriloquate!", "", FALSE, SLOT(0)
  },

  {
    "weaken",
    { 17, L_APP, L_APP, L_APP, L_APP, L_APP, 29, L_APP, L_APP, 29,
      39, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			20,	12,
    "spell",
    "You feel stronger.",
    "$n looks stronger.",
    TRUE, SLOT(32)
  },

  {
    "word of recall",
    { 21, 16, L_APP, L_APP, 23, 32, 51, 54, L_APP, 53, 
      25, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
    NULL,			5,	12,
    "",			"!Word of Recall!", "", FALSE, SLOT(0)
  },

/*
 * Dragon breath
 */
  {
    "acid breath",
    { 63, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			50,	12,
    "&gblast of &Gacid&X",	"!Acid Breath!", "", FALSE, SLOT(0)
  },

  {
    "fire breath",
    { 64, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			50,	12,
    "&Rblast of &rflame&X",	"!Fire Breath!", "", FALSE, SLOT(0)
  },

  {
    "frost breath",
    { 61, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 
       L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			50,	12,
    "&Cblast of &bfrost",	"!Frost Breath!", "", FALSE, SLOT(0)
  },

  {
    "gas breath",
    { 65, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
    NULL,			50,	12,
    "&zblast of &wgas",		"!Gas Breath!", "", FALSE, SLOT(0)
  },

  {
    "lightning breath",	
    { 62, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			50,	12,
    "&cblast of &Blightning",	"!Lightning Breath!", "", FALSE, SLOT(0)
  },
/*
  {MAG,THI,CLE,WAR,PSY,DRU,RAN,PAL,BAR,VAM,
   NEC,WWF,PRO,PRO,PRO,PRO}
*/
/* XOR */
  {
    "summon swarm",
    { L_APP, 80, L_APP, L_APP, L_APP, 60, 70, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_summon_swarm,	TAR_CHAR_SELF,		POS_FIGHTING,
    NULL,			50,	12,
    "",			"!Summon Insect!", "", FALSE, SLOT(0)
  },

  {
    "summon pack",
    { L_APP, L_APP, L_APP, L_APP, L_APP, 70, 80, L_APP, L_APP, L_APP,
      L_APP, 58, L_APP, L_APP, L_APP, L_APP },
    spell_summon_pack,	TAR_CHAR_SELF,		POS_FIGHTING,
    NULL,			50,	12,
    "",			"!Summon Pack!", "", FALSE, SLOT(0)
  },

  {
    "summon demon",
    { 90, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 90,
      95, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_summon_demon,	TAR_CHAR_SELF,		POS_FIGHTING,
    NULL,			50,	12,
    "",			"!Summon Demon!", "", FALSE, SLOT(0)
  },

/*
 * Fighter and thief skills.
 */
  {
    "backstab",
    { L_APP, L_APP, 9, L_APP, L_APP, L_APP, L_APP, L_APP, 42, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_STANDING,
    &gsn_backstab,		0,	24,
    "backstab",		"!Backstab!", "", FALSE, SLOT(0)
  },

  {
    "double backstab",
    { L_APP, L_APP, 85, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_STANDING,
    &gsn_backstab_2,		0,	24,
    "backstab",		"!Double Backstab!", "", FALSE, SLOT(0)
  },
  
  {
    "palm",
    { L_APP, L_APP, 20, L_APP, L_APP, L_APP, L_APP, L_APP, 40, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,			TAR_IGNORE, POS_RESTING,
    &gsn_palm,			0, 0,
    "", "!palm", "", FALSE, SLOT(0)
  },

  {
    "bash",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
    &gsn_bash,		0,	24,
    "bash",			"!Bash!", "", FALSE, SLOT(0)
  },

  {
    "disarm",
    { L_APP, L_APP,    11, 9, L_APP, L_APP, 12, 14, 15, L_APP,
      L_APP, 30, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_disarm,		0,	24,
    "",			"!Disarm!", "", FALSE, SLOT(0)
  },

  {
    "dodge",
    { 29, 26,     4, 5, 9, 25, 7, 11, 7, 14,
      29, 5, 4, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_dodge,		0,	 0,
    "",			"!Dodge!", "", FALSE, SLOT(0)
  },

  {
    "dual",
    { L_APP, L_APP, 21,  14, L_APP, L_APP, 27, 21, 25, 37,
      L_APP, L_APP, 22, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,	POS_RESTING,
    &gsn_dual,		0,	 0,
    "",			"!DUAL!", "", FALSE, SLOT(0)
  },

  {
    "eighth attack",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_eighth_attack,	0,	 0,
    "",			"!eighth Attack!", "", FALSE, SLOT(0)
  },

  {
    "enhanced damage",
    { L_APP, 49, 14, 3, L_APP, 57, 13, 11, 26, 23,
      L_APP, 13, 12, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_enhanced_damage,	0,	 0,
    "",			"!Enhanced Damage!", "", FALSE, SLOT(0)
  },

  {
    "enhanced damage two",
    { L_APP, L_APP, L_APP, 20, L_APP, L_APP,   42, 38, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_enhanced_two,	0,	 0,
    "",			"!Enhanced Damage Two!", "", FALSE, SLOT(0)
  },

  {
    "enhanced hit",
    { L_APP, L_APP, 17, 15, 26, L_APP, 17, 16,    15, 31,
      L_APP, 17, 20, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_enhanced_hit,	0,	 0,
    "",			"!Enhanced Hit!", "", FALSE, SLOT(0)
  },
  
  {
    "feed",			
    { L_APP, L_APP, L_APP,  L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 3, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_feed,		0,	 12,
    "bite",			"!Feed!", "", FALSE, SLOT(0)
  },

  {
    "fifth attack",
    { L_APP, L_APP, L_APP, 29, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_fifth_attack,	0,	 0,
    "",			"!Fifth Attack!", "", FALSE, SLOT(0)
  },

  {
    "fourth attack",
    { L_APP, L_APP, L_APP, 23, L_APP, L_APP, 34, 29, 45, 40, 
      L_APP, 50, 40, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_fourth_attack,	0,	 0,
    "",			"!FOURTH Attack!", "", FALSE, SLOT(0)
  },

  {
    "hide",
    { L_APP, L_APP,     4, L_APP, L_APP, L_APP, 7, L_APP, 5, 8,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_RESTING,
    &gsn_hide,		0,	12,
    "",
    "You are no longer so difficult to see.",
    "$n has stopped hiding.", FALSE, SLOT(33)
  },

  {
    "iron grip",
    { L_APP, L_APP, L_APP,  19, L_APP, L_APP, L_APP, 27, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,   	 	POS_STANDING,
    &gsn_grip,		0,	 8,
    "",
    "You loosen your hold on your weapon.",
    "$n has loosened $s hold on $s weapon.", FALSE, SLOT(34)
  },

  {
    "flury",
    { L_APP, L_APP,  L_APP,  44, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_flury,		0,	 50,
    "flury",			"!flury!", "", FALSE, SLOT(0)
  },

  {
    "kick",
    { L_APP, L_APP,  50,  10, L_APP, L_APP, 23, L_APP, L_APP, L_APP,
      L_APP, L_APP, 6, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_kick,		0,	 8,
    "kick",			"!Kick!", "", FALSE, SLOT(0)
  },

  {
    "high kick",
    { L_APP, L_APP, L_APP,  35, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_high_kick,		0,	 10,
    "high kick",  "!High Kick!", "", FALSE, SLOT(0)
  },

  {
    "jump kick",
    { L_APP, L_APP, L_APP,  50, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_jump_kick,		0,	 20,
    "jump kick",  "!Jump Kick!", "", FALSE, SLOT(0)
  },

  {
    "spin kick",
    { L_APP, L_APP, L_APP,  60, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_spin_kick,		0,	 25,
    "spin kick",  "!Spin Kick!", "", FALSE, SLOT(0)
  },

  {
    "parry",
    { L_APP, 27, 5, 6, L_APP, 27, 9, 7, 7, 15,
      L_APP, 5, 7, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_parry,		0,	 0,
    "",			"!Parry!", "", FALSE, SLOT(0)
  },

  {
    "peek",
    { L_APP, L_APP, 23, L_APP, L_APP, L_APP, L_APP, L_APP, 29, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_STANDING,
    &gsn_peek,		0,	 0,
    "",			"!Peek!", "", FALSE, SLOT(0)
  },

  {
    "pick lock",
    { L_APP, L_APP,  5, L_APP, L_APP, L_APP, L_APP, L_APP, 9, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_STANDING,
    &gsn_pick_lock,		0,	12,
    "",			"!Pick!", "", FALSE, SLOT(0)
  },

  {
    "poison weapon",
    { L_APP, L_APP, 43, L_APP, L_APP, L_APP, L_APP, L_APP, 72, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_OBJ_INV,		POS_STANDING,
    &gsn_poison_weapon,	0,	12,
    "poisonous concoction",	"!Poison Weapon!", "", FALSE, SLOT(0)
  },

  {
    "punch",
    { L_APP, L_APP, L_APP,  31, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 17, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_punch,		0,	 10,
    "punch",		"!punch!", "", FALSE, SLOT(0)
  },

  {
    "jab punch",
    { L_APP, L_APP, L_APP,  35, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_jab_punch,		0,	 10,
    "jab",		"!jab!", "", FALSE, SLOT(0)
  },

  {
    "kidney punch",
    { L_APP, L_APP, L_APP,  40, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_kidney_punch,		0,	 10,
    "kindey punch",	"!kidney punch!", "", FALSE, SLOT(0)
  },

  {
    "cross punch",
    { L_APP, L_APP, L_APP,  45, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_cross_punch,		0,	 10,
    "cross punch",	"!cross punch!", "", FALSE, SLOT(0)
  },

  {
    "roundhouse punch",
    { L_APP, L_APP, L_APP,  50, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_roundhouse_punch,		0,	 18,
    "roundhouse",	"!roundhouse punch!", "", FALSE, SLOT(0)
  },

  {
    "uppercut punch",
    { L_APP, L_APP, L_APP,  55, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_uppercut_punch,		0,	 18,
    "uppercut",	"!uppercut punch!", "", FALSE, SLOT(0)
  },

  {
    "rescue",
    { L_APP, 19, L_APP, 7, L_APP, 31, 11, 11, L_APP, L_APP,
      L_APP, L_APP, 35, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_rescue,		0,	12,
    "",			"!Rescue!", "", FALSE, SLOT(0)
  },

  {
    "second attack",
    { 12, 17, 11, 3, 16, 16, 12, 8, 21, 19,
      18, 6, 10, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_second_attack,	0,	 0,
    "",			"!Second Attack!", "", FALSE, SLOT(0)
  },

  {
    "seventh attack",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_seventh_attack,	0,	 0,
    "",			"!seventh Attack!", "", FALSE, SLOT(0)
  },

  {
    "sixth attack",
    { L_APP, L_APP, L_APP, 70, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_sixth_attack,	0,	 0,
    "",			"!sixth Attack!", "", FALSE, SLOT(0)
  },

  {
    "sneak",
    { L_APP, L_APP,  22, L_APP, L_APP, L_APP, 33, L_APP, 23, 29,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_STANDING,
    &gsn_sneak,		0,	12,
    "",			NULL, "", FALSE, SLOT(35)
  },

  {
    "steal",
    { L_APP, L_APP, 13, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_STANDING,
    &gsn_steal,		0,	24,
    "",			"!Steal!", "", FALSE, SLOT(0)
  },

  {
    "third attack",
    { L_APP, 31, 22, 9, 60, 50, 24, 19, 33, 31, 
      60, 25, 25, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_third_attack,	0,	 0,
    "",			"!Third Attack!", "", FALSE, SLOT(0)
  },

/*
 *  Spells for mega1.are from Glop/Erkenbrand.
 */
  {
    "general purpose",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   0,      12,
    "general purpose ammo", "!General Purpose Ammo!", "", FALSE, SLOT(0)
  },

  {
    "high explosive",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   0,      12,
    "high explosive ammo",  "!High Explosive Ammo!", "", FALSE, SLOT(0)
  },


    /*
     * Psionicist spell/skill definitions
     * Psi class conceived, created, and coded by Thelonius (EnvyMud)
     */

  {
    "adrenaline control",
    { L_APP, L_APP, L_APP, L_APP, 17, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 30, L_APP, L_APP, L_APP },
    spell_adrenaline_control,       TAR_CHAR_SELF,  POS_STANDING,
    NULL,                           6,      12,
    "",
    "The adrenaline rush wears off.",
    "$n has calmed down.", FALSE, SLOT(36)
  },

  {
    "agitation",
    { L_APP, L_APP, L_APP, L_APP,    6, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_agitation,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   10,     12,
    "agitation",            "!Agitation!", "", FALSE, SLOT(0)
  },

  {
    "aura sight",
    { L_APP, L_APP, L_APP, L_APP,   14, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_aura_sight,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   9,     12,
    "",                     "!Aura Sight!", "", FALSE, SLOT(0)
  },

  {
    "awe",
    { L_APP, L_APP, L_APP, L_APP,   21, L_APP, L_APP, L_APP, L_APP, L_APP,
      85, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_awe,              TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
    NULL,                   35,     12,
    "",                     "!Awe!", "", FALSE, SLOT(0)
  },

  {
    "ballistic attack",
    { L_APP, L_APP, L_APP, L_APP,    3, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_ballistic_attack, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   5,     12,
    "ballistic attack",     "!Ballistic Attack!", "", FALSE, SLOT(0)
  },

  {
    "biofeedback",
    { L_APP, L_APP, L_APP, L_APP,   18, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_biofeedback,      TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   75,     12,
    "",
    "Your biofeedback is no longer effective.",
    "$n's body shivers and relaxes.",
    FALSE, SLOT(37)
  },

  {
    "cell adjustment",
    { L_APP, L_APP, L_APP, L_APP,   11, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 24, L_APP, L_APP, L_APP },
    spell_cell_adjustment,  TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   8,     12,
    "",                     "!Cell Adjustment!", "", FALSE, SLOT(0)
  },

  {
    "chameleon power",
    { L_APP, L_APP, L_APP, L_APP,   6, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_chameleon,         0,     12,
    "",                     "!Chameleon Power!", "", FALSE, SLOT(38)
  },

  {
    "chaos field",
    { L_APP,  L_APP, L_APP, L_APP, 67, L_APP, L_APP, L_APP, L_APP, L_APP,
      75, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_chaosfield,	TAR_CHAR_SELF,  	POS_STANDING,
    NULL,			150,	12,
    "",
    "The chaos flees from the order that surrounds you.",
    "The chaos flees from the order around $n.", FALSE, SLOT(39)
  },

  {
    "combat mind",
    { L_APP, L_APP, L_APP, L_APP,  15, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_combat_mind,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   15,     12,
    "",                     "Your battle sense has faded.",
    "$n's sense of battle has diminished.", FALSE, SLOT(40)
  },

  {
    "complete healing",
    { L_APP, 70, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_complete_healing, TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   200,    12,
    "",                     "!Complete Healing!", "", FALSE, SLOT(0)
  },

  {
    "control flames",
    { L_APP, L_APP, L_APP, L_APP, 11, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_control_flames,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   15,     12,
    "tongue of &Rflame&X",      "!Control Flames!", "", FALSE, SLOT(0)
  },

  {
    "create sound",
    { L_APP, L_APP, L_APP, L_APP, 3, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_create_sound,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   5,     12,
    "",                     "!Create Sound!", "", FALSE, SLOT(0)
  },

  {
    "death field",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 68,
      70, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_death_field,      TAR_IGNORE,             POS_FIGHTING,
    NULL,                   200,    18,
    "&zfield of death&X",       "!Death Field!", "", FALSE, SLOT(0)
  },

  {
    "detonate",
    { L_APP, L_APP, L_APP, L_APP,   27, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_detonate,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   35,     24,
    "&zdet&Rona&rtion",           "!Detonate!", "", FALSE, SLOT(0)
  },

  {
    "disintegrate",
    { L_APP, L_APP, L_APP, L_APP,   97, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_disintegrate,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   150,    18,
    "disintegration",       "!Disintegrate!", "", FALSE, SLOT(0)
  },

  {
    "displacement",
    { L_APP, L_APP, L_APP, L_APP,    9, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_displacement,     TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   10,     12,
    "",
    "You are no longer displaced.",
    "$n's image fades as he appears in another spot.", FALSE, SLOT(41)
  },

  {
    "domination",
    { L_APP, L_APP, L_APP, L_APP,   16, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_domination,       TAR_CHAR_OFFENSIVE,     POS_STANDING,
    &gsn_domination,        5,     12,
    "",
    "You regain control of your body.",
    "$n regains control of $m body.", FALSE, SLOT(42)
  },

  {
    "ectoplasmic form",
    { L_APP, L_APP, L_APP, L_APP,   19, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_ectoplasmic_form, TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   20,     12,
    "",
    "You feel solid again.",
    "$n looks solid again.",
    FALSE, SLOT(43)
  },

  {
    "ego whip",
    { L_APP, L_APP, L_APP, L_APP,   13, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_ego_whip,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   20,     12,
    "",
    "You feel more confident.",
    "$n looks more confident.", FALSE, SLOT(44)
  },

  {
    "energy containment",
    { L_APP, L_APP, L_APP, L_APP,   10, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_energy_containment,       TAR_CHAR_SELF,  POS_STANDING,
    NULL,                           10,     12,
    "",
    "You no longer absorb energy.",
    "$n no longer absorbs energy.", FALSE, SLOT(45)
  },

  {
    "enhance armor",
    { L_APP, 60, L_APP, L_APP, L_APP, 60, 43, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_enhance_armor,    TAR_OBJ_INV,    POS_STANDING,
    NULL,                   100,    24,
    "",                     "!Enhance Armor!", "", FALSE, SLOT(0)
  },

  {
    "enhanced strength",
    { L_APP, L_APP, L_APP, L_APP,    7, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 19, L_APP, L_APP, L_APP },
    spell_enhanced_strength,        TAR_CHAR_SELF,  POS_STANDING,
    NULL,                           20,     12,
    "",
    "You no longer feel so HUGE.",
    "$n doesn't look so huge.",
     FALSE, SLOT(46)
  },

  {
    "flesh armor",
    { L_APP, L_APP, L_APP, L_APP,   11, L_APP, L_APP, L_APP, L_APP, L_APP,
      43, L_APP, 21, L_APP, L_APP, L_APP },
    spell_flesh_armor,      TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   15,     12,
    "",
    "Your skin returns to normal.",
    "$n's skin returns to its normal texture.", FALSE, SLOT(47)
  },

  {
    "heighten senses",
    { L_APP, L_APP, 10, L_APP,      3, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 23, 14, L_APP, L_APP, L_APP },
    spell_null,             TAR_CHAR_SELF,          POS_STANDING,
    &gsn_heighten,          0,      0,
    "",                     "Your senses return to normal.", 
    "$n's senses return to normal.", FALSE, SLOT(48)
  },

  {
    "inertial barrier",
    { L_APP, L_APP, L_APP, L_APP,   19, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_inertial_barrier, TAR_IGNORE,             POS_STANDING,
    NULL,                   40,     24,
    "",
    "Your inertial barrier dissipates.",
    "$n's inertial barrier dissipates.", FALSE, SLOT(49)
  },

  {
    "inflict pain",
    { L_APP, L_APP, L_APP, L_APP,    5, L_APP, L_APP, L_APP, L_APP, L_APP,
      9, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_inflict_pain,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   10,     12,
    "&wmind&Wpower&X",            "!Inflict Pain!", "", FALSE, SLOT(0)
  },

  {
    "intellect fortress",
    { L_APP, L_APP, L_APP, L_APP,   12, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_intellect_fortress,       TAR_IGNORE,     POS_STANDING,
    NULL,                           25,     24,
    "",
    "Your intellectual fortress crumbles.",
    "$n's intellectual fortress crumbles.",
    FALSE, SLOT(50)
  },

  {
    "lend health",
    { L_APP, L_APP, L_APP, L_APP,   10, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_lend_health,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   10,     12,
    "",                     "!Lend Health!", "", FALSE, SLOT(0)
  },

  {
    "levitation",
    { L_APP, L_APP, L_APP, L_APP,    3, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 16, L_APP, L_APP, L_APP },
    spell_levitation,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   10,     18,
    "",
    "You slowly float to the ground.",
    "$n slowly floats to the ground.",
     FALSE, SLOT(51)
  },

  {
    "mental barrier",
    { L_APP, L_APP, L_APP, L_APP,    6, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 48, L_APP, L_APP, L_APP },
    spell_mental_barrier,   TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   8,     12,
    "",                     "Your mental barrier breaks down.", 
    "$n's mental barrier diminishes.", FALSE, SLOT(52)
  },

  {
    "mind thrust",
    { L_APP, L_APP, L_APP, L_APP,    3, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_mind_thrust,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   8,     12,
    "&wmind &Wthrust&X",          "!Mind Thrust!", "", FALSE, SLOT(0)
  },

  {
    "project force",
    { L_APP, L_APP, L_APP, L_APP,    9, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_project_force,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   18,     12,
    "&wprojected force&X",      "!Project Force!", "", FALSE, SLOT(0)
  },

  {
    "psionic blast",
    { L_APP, L_APP, L_APP, L_APP,   17, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_psionic_blast,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   25,     12,
    "&wpsionic blast&X",        "!Psionic Blast!", "", FALSE, SLOT(0)
  },

  {
    "psychic crush",
    { L_APP, L_APP, L_APP, L_APP,    8, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_psychic_crush,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   15,     18,
    "&wpsychic crush&X",        "!Psychic Crush!", "", FALSE, SLOT(0)
  },

  {
    "psychic drain",
    { L_APP, L_APP, L_APP, L_APP,    4, L_APP, L_APP, L_APP, L_APP, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_psychic_drain,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   20,     12,
    "",
    "You no longer feel drained.",
    "$n looks stronger.",
     FALSE, SLOT(53)
  },

  {
    "psychic healing",
    { L_APP, L_APP, L_APP, L_APP,   5, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 15, L_APP, L_APP, L_APP },
    spell_psychic_healing,  TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   20,      12,
    "",                     "!Psychic Healing!", "", FALSE, SLOT(0)
  },

  {
    "shadow form",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_shadow,            0,     12,
    "",                     "You no longer move in the shadows.", 
    "$n returns from the shadows.", FALSE, SLOT(54)
  },

  {
    "share strength",
    { L_APP, L_APP, L_APP, L_APP,   11, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_share_strength,   TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   8,     12,
    "",                 "You no longer share strength with another.", 
    "$n stops sharing $m strength.", FALSE, SLOT(55)
  },

  {
    "thought shield",
    { L_APP, L_APP, L_APP, L_APP,    3, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 11, L_APP, L_APP, L_APP },
    spell_thought_shield,   TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   5,     12,
    "",                     "You no longer feel so protected.", 
    "$n doesn't feel very protected.", FALSE, SLOT(56)
  },

  {
    "ultrablast",
    { L_APP, L_APP, L_APP, L_APP,   25, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_ultrablast,       TAR_IGNORE,             POS_FIGHTING,
    NULL,                   75,     24,
    "&zultr&wabl&Wast&X",		"!Ultrablast!" , "", FALSE, 
SLOT(0)
  },

  {
    "cancellation",
    { L_APP, 56, L_APP, L_APP, L_APP, 72, L_APP, L_APP, L_APP, L_APP,
      60, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_cancellation,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
    NULL, 			15,	16,
    "",                     "!Cancellation!", "", FALSE, SLOT(0)
  },

  {
    "throw",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null, TAR_CHAR_OFFENSIVE, POS_STANDING,
    &gsn_throw,   0,  0,
    "throw",   "!Throw!", "", FALSE, SLOT(0)
  },


/*
 * Turn undead and mental block by Altrag
 */
  {
    "turn undead",
    { L_APP, 58, L_APP, L_APP, L_APP, L_APP, L_APP, 71, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_turn_undead, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,              100,     12,
    "",                "!Turn Undead!", "", FALSE, SLOT(0)
  },

  {
    "mental block",
    { L_APP, L_APP, L_APP, L_APP, 27, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 100, L_APP, L_APP, L_APP },
    spell_mental_block, TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,               25,     8,
    "",                 "Your mind feels more vulnerable.", 
    "$n's mind becomes more vulnerable.", FALSE, SLOT(57)
  },

/*
 * Patch by Altrag
 */
  {
    "patch",
    { L_APP, L_APP, L_APP, 26, L_APP, L_APP, 68, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,     TAR_IGNORE, POS_STANDING,
    &gsn_patch,     0, 0,
    "",             "!Patch!", "", FALSE, SLOT(0)
  },

  {
    "flamesphere",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    gspell_flamesphere, TAR_GROUP_OFFENSIVE, POS_STANDING,
    NULL,           14, 20,
    "flamesphere",  "!Flamesphere!", "", TRUE, SLOT(58)
  },

  {
    "bash door",
    { L_APP, L_APP, L_APP,   18, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_STANDING,
    &gsn_bash_door,		0,	24,
    "bash",			"!Bash Door!", "", FALSE, SLOT(0)
  },

  {
    "mental drain",
    { L_APP, L_APP, L_APP, L_APP,  30, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_mental_drain,	0,	12,
    "mental drain",		"!Mental Drain!",	"", FALSE, SLOT(0)
  },

  {
    "drain life",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 10,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_drain_life,	0,	12,
    "&zlife &wdrain",		"!Drain Life!",	"", FALSE, SLOT(0)
  },

  {
    "holy strength",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 30, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_holy_strength,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			100,	12,
    "",			"Your divine strength fades.",	
    "The divine strength surrounding $n fades.", FALSE, SLOT(59)
  },

  {
    "curse of nature",
    { L_APP, L_APP, L_APP, L_APP, L_APP, 30, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_curse_of_nature,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			100,	12,
    "",		"You feel in sync with nature once again.",	
    "$n feels in sync with nature once again.",TRUE, SLOT(60)
  },

  {
    "detect good",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 22, L_APP, L_APP, 15,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_detect_good,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			5,	12,
    "",			"The white in your vision disappears.",	
    "$n's vision returns to normal.", TRUE, SLOT(61)
  },

  {
    "protection good",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_protection_good,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			5,	12,
    "",			"You feel less protected.",	
    "$n looks less protected.", TRUE, SLOT(62)
  },

  {
    "shield block",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_shield_block,	0,	 0,
    "",			"!Shield Block!",	"", FALSE, SLOT(0)
  },

  {
    "enchantment",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 30, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_enchanted_song,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			100,	12,
    "",			"The peace leaves you.",	
    "The peace within $n disappears.", FALSE, SLOT(0)
  },

  {
    "turn evil",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_turn_evil,		0,	12,
    "&Wdivine power&X",		"!Turn Evil!",	"", FALSE, SLOT(0)
  },

  {
    "mass shield",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    gspell_mass_shield,     TAR_GROUP_ALL,    POS_STANDING,
    NULL,              80,  12,
    "",     "!Mass Shield!", "", TRUE, SLOT(63)
  },

  {
    "gouge",
    { L_APP, L_APP, 20, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_gouge,             0,      12,
    "gouge",                "!Gouge!", "", FALSE, SLOT(0)
  },

  {
    "holy sword", 
    { L_APP, 52, L_APP, L_APP, L_APP, L_APP, L_APP, 56, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_holysword,         TAR_OBJ_INV,        POS_STANDING,
    NULL,			100,	24,
    "",			"!Holy Sword!", "", FALSE, SLOT(0)
  },

  {
    "summon angel",
    { L_APP, 80, L_APP, L_APP, L_APP, L_APP, L_APP, 86, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_summon_angel,	TAR_CHAR_SELF,		POS_FIGHTING,
    NULL,			100,	12,
    "",			"!Summon Angel!", "", FALSE, SLOT(0)
  },

  {
    "holy fires",
    { L_APP, 32, L_APP, L_APP, L_APP, L_APP, L_APP, 40, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_holy_fires,	TAR_CHAR_OFFENSIVE,      POS_FIGHTING,
    NULL,			50,	12,
    "&Who&Rly fl&rames&X",		"!Holy Fires!", "", FALSE, SLOT(0)
  },

  {
    "true sight",
    {    51,   55,   L_APP, L_APP, 36, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_truesight,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			59,	12,
    "",			"Your vision is no longer so true.", 
    "$n's vision returns to normal.", TRUE, SLOT(64)
  },

  {
    "blade barrier",
    { L_APP,  L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_bladebarrier,	TAR_CHAR_SELF,  	POS_STANDING,
    NULL,			150,	12,
    "",			"The phantom blades dissipate into the air.", 
    "The phantom blades surrounding $n disappear.", TRUE, SLOT(65)
  },
    
  { 
    "alchemy",
    { L_APP, 75, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,              TAR_IGNORE,             POS_STANDING,
    &gsn_alchemy,             0,  50,
    "",                      "!ALCHEMY!", "", FALSE, SLOT(0)
  },      
    
  { 
    "scribe",
    { 60, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      72, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,              TAR_IGNORE,             POS_STANDING,
    &gsn_scribe,             0,  50,
    "",                      "!SCRIBE!", "", FALSE, SLOT(0)
  },
    
  {
    "flame blade", 
    { L_APP, 62, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_flame_blade,       TAR_OBJ_INV,        POS_STANDING,
    NULL,			100,	24,
    "",			"!Flame Blade!", "", FALSE, SLOT(0)
  },

  {
    "chaos blade", 
    { L_APP, L_APP, L_APP, L_APP, 60, L_APP, L_APP, L_APP, L_APP, L_APP,
      90, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_chaos_blade,       TAR_OBJ_INV,        POS_STANDING,
    NULL,			100,	24,
    "",			"!Chaos Blade!", "", FALSE, SLOT(0)
  },

  {
    "frost blade", 
    { L_APP, 82, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_frost_blade,       TAR_OBJ_INV,        POS_STANDING,
    NULL,			100,	24,
    "",			"!Frost Blade!", "", FALSE, SLOT(0)
  },

  {
    "stun",
    { L_APP, L_APP, L_APP, 43, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,           TAR_IGNORE,           POS_FIGHTING,
    &gsn_stun,            0, 0,
    "", "!Stun!", "", FALSE, SLOT(0)
  },

  {
    "berserk",
    { L_APP, L_APP, L_APP, 76, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,       TAR_IGNORE,       POS_FIGHTING,
    &gsn_berserk,     0, 0,
    "",        "You feel more sane.", 
    "$n has regained $s sanity.", FALSE, SLOT(0)
  },

  {
    "web", 
    { 12, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_web,         TAR_CHAR_OFFENSIVE,        POS_FIGHTING,      
    NULL,			25,	14,
    "",			"The webs melt away.", 
    "The webs entangling $n melt away.", TRUE, SLOT(66)
  },

  {
    "entangle", 
    { L_APP, L_APP, L_APP, L_APP, L_APP, 19, 32, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_entangle,          TAR_CHAR_OFFENSIVE,        POS_FIGHTING,
    NULL,			25,	14,
    "",			"The vines release their grasp.", 
    "The vines unwrap themselves from $n.", TRUE, SLOT(67)
  },

  {
    "dark blessing",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 26,
      61, L_APP, L_APP, L_APP, L_APP, L_APP },
	 spell_darkbless,       TAR_CHAR_DEFENSIVE,        POS_STANDING,
    NULL,			50,	24,
    "",			"The sweet caress of oblivion has lifted.", 
    "The sweet caress of oblivion rises from $n.", FALSE, SLOT(68)
  },
/* 250 */
  {
    "bio-acceleration", 
    { L_APP, L_APP, L_APP, L_APP, 29, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_bio_acceleration,  TAR_CHAR_SELF,        POS_STANDING,
    NULL,			50,	24,
    "",			"Your body structure returns to normal.",     
    "$n's body returns to a normal form.", FALSE, SLOT(69)
  },

  {
    "confusion", 
    { 57, L_APP, L_APP, L_APP, 58, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_confusion,       TAR_CHAR_OFFENSIVE,        POS_FIGHTING,
    NULL,			50,	24,
    "",		"You become more sure of your surroundings.", 
    "$n looks less confused.", TRUE, SLOT(70)
  },

  {
    "mind probe", 
    { L_APP, L_APP, L_APP, L_APP, 43, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_mind_probe,       TAR_CHAR_DEFENSIVE,        POS_STANDING,
    NULL,			50,	24,
    "",			"!MIND PROBE!", "", FALSE, SLOT(0)
  },

  {
    "haste",
    { 75, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_haste,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			30,	12,
    "",			"You feel yourself slow down",
    "$n slows down.", TRUE, SLOT(71)
  },

  {
    "trip",
    { L_APP, L_APP, 35, L_APP, L_APP, L_APP, L_APP, L_APP, 55, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,             TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_trip,              0,      12,
    "trip",                "!Trip!", "", FALSE, SLOT(0)
  },

  {
    "dirt kick",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,             TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_dirt_kick,		0,      12,
    "dirt kick",		"!Dirt Kick!", "", FALSE, SLOT(0)
  },

  {
    "chain lightning",
    { 42, L_APP, L_APP, L_APP, L_APP, 57, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_chain_lightning,	TAR_IGNORE,		POS_FIGHTING,
    NULL,			50,	22,
    "&Belectrical blast&X",   	"!CHAIN LIGHTNING!", "", FALSE, SLOT(0)
  },

  {
    "meteor swarm",
    { 72, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_meteor_swarm,	TAR_IGNORE,		POS_FIGHTING,
    NULL,			150,	25,
    "&rm&Oe&Yt&Oe&ro&Yr&rs",       	"!SWARM!", "", TRUE, SLOT(72)
  },

  {
    "psychic quake",
    { L_APP, L_APP, L_APP, L_APP, 79, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_psychic_quake,	TAR_IGNORE,		POS_FIGHTING,
    NULL,			150,	25,
    "&wmental shockwave&X",   	"!PSY-QUAKE!", "", FALSE, SLOT(0)
  },

  {
    "fumble", 
    { 37, L_APP, L_APP, L_APP, 38, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_fumble,       TAR_CHAR_OFFENSIVE,        POS_STANDING,
    NULL,			50,	24,
    "",			"You feel more sure of hand.", 
    "$n looks more confident.", TRUE, SLOT(73)
  },

  {
    "dancing lights",
    { 41, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_dancing_lights,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			50,	12,
    "dancing lights","The dancing lights about your body fade away.", 
    "Dancing lights about $n fade away.", FALSE, SLOT(74)
  },

  {
    "shadow walk",
    { L_APP, L_APP, 71, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_shadow_walk,       0,     22,
    "",                     "!SHADOW WALK!", "", FALSE, SLOT(0)
  },

  {
    "circle",
    { L_APP, L_APP, 80, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_circle,		0,	 12,
    "sneak attack",		"!CIRCLE!", "", FALSE, SLOT(0)
  },

  {
    "summon shadow",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 69,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_summon_shadow,	TAR_CHAR_SELF,		POS_FIGHTING,
    NULL,			100,	12,
    "",			"!Summon Shadow!", "", FALSE, SLOT(0)
  },

  {
    "summon beast",
    { L_APP, L_APP, L_APP, L_APP, L_APP, 47, 62, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_summon_beast,	TAR_CHAR_SELF,		POS_FIGHTING,
    NULL,			100,	12,
    "",			"!Summon Beast!", "", FALSE, SLOT(0)
  },

  {
    "summon treant",
    { L_APP, L_APP, L_APP, L_APP, L_APP, 76, 97, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_summon_trent,	TAR_CHAR_SELF,		POS_FIGHTING,
    NULL,			100,	12,
    "",			"!Summon Trent!", "", FALSE, SLOT(0)
  },

  {
    "shatter",
    { 79, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_shatter,  	TAR_CHAR_OFFENSIVE,		POS_FIGHTING,
    NULL,		        150,	32,
    "",			"!shatter!", "", FALSE, SLOT(0)
  },

  {
    "molecular unbinding",
    { L_APP, L_APP, L_APP, L_APP, 66, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_molecular_unbind,	TAR_CHAR_OFFENSIVE,		POS_FIGHTING,
    NULL,			100,	32,
    "",			"!shatter!", "", FALSE, SLOT(0)
  },

  {
    "phase shift",
    { L_APP, L_APP, L_APP, L_APP, 70, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_phase_shift,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			100,	12,
    "",			"You shift back into the physical plane.", 
    "$n's body shifts back into the existing world.", FALSE, SLOT(75)
  },

  {
    "disrupt",
    { L_APP, L_APP, L_APP, L_APP, 92, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_disrupt,        TAR_CHAR_OFFENSIVE,       POS_FIGHTING,
    NULL,                 125,     12,
    "disruption",         "You feel less disrupted.", 
    "$n looks more stable.", TRUE, SLOT(76)
  },

  {
    "soulstrike",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 73, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,           TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    &gsn_soulstrike,      0,      24,
    "soul",        "!Soulstrike!",   "", FALSE, SLOT(0)
  },

  {
    "healing hands",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 48, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_healing_hands,      TAR_CHAR_DEFENSIVE,    POS_STANDING,
    NULL,              40,     12,
    "",         "!Healing Hands!",    "", FALSE, SLOT(0)
  },

  {
    "pray",
    { L_APP, 23, L_APP, L_APP, L_APP, L_APP, L_APP, 27, L_APP, L_APP,
      L_APP, L_APP, 43, L_APP, L_APP, L_APP },
    spell_null,           TAR_CHAR_SELF,    POS_RESTING,
    &gsn_prayer,       0, 12,
    "",   "Thalador's blessing leaves your soul.",   
    "Thalador's blessing rises from $n's soul.", FALSE, SLOT(77)
  },
  {
    "spellcraft",
    { 25, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      60, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		  TAR_IGNORE,	POS_FIGHTING,
    &gsn_spellcraft,	0, 0,
    "",    "!Spellcraft!", "", FALSE, SLOT(0)
  },
  {
    "multiburst",
    { 80, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_multiburst,	0, 8,
    "",		"!Multiburst!", "", FALSE, SLOT(0)
  },
/* Necromancer spells by Hannibal. */
  {
    "hex",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      60, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_hex,		 TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,	 	75, 12,
    "", "The hex placed upon your soul wears off.", 
    "The hex placed upon $n's soul vanishes.", TRUE, SLOT(78)
  },
  {
    "dark ritual",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      10, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_dark_ritual,  TAR_IGNORE,		POS_STANDING,
    NULL,		5, 12,
    "", 		"!Dark Ritual!", "", FALSE, SLOT(0)
  },
  {
    "gravebind",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      10, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_OBJ_INV,		POS_RESTING,
    &gsn_gravebind,		0, 0,
    "", 		"!Gravebind!", "", FALSE, SLOT(0)
  },
  {
    "field of decay",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      80, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_field_of_decay,	TAR_CHAR_SELF, POS_RESTING,
    NULL,			300, 12,
    "",		"The black haze enveloping your body dissipates.", "",
TRUE, SLOT(79)
  },
  {
    "stench of decay",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      25, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_stench_of_decay,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    NULL,			15, 12,
    "&zstench of decay&X", "!Stench of Decay!", "", FALSE, SLOT(0)
  },
/* Werewolf starts here -- Hannibal */
  {
    "enhanced claws",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 3, L_APP, L_APP, L_APP, L_APP },
    spell_null,			TAR_IGNORE, POS_FIGHTING,
    &gsn_enhanced_claw,		0, 0,
    "", "!Enhanced Claws!", "", FALSE, SLOT(0)
  },
  {
    "dual claws",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 9, L_APP, L_APP, L_APP, L_APP },
    spell_null,			TAR_IGNORE, POS_FIGHTING,
    &gsn_dualclaw,		0, 0,
    "", "!Dual Claws!", "", FALSE, SLOT(0)
  },
  {
    "fast healing",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 21, L_APP, L_APP, L_APP, L_APP },
    spell_null,			TAR_IGNORE, POS_SLEEPING,
    &gsn_fastheal,		0, 0,
    "", "!Fast Healing!", "", FALSE, SLOT(0)
  },
  {
    "rage",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 9, L_APP, L_APP, L_APP, L_APP },
    spell_null,			TAR_IGNORE,	POS_RESTING,
    &gsn_rage,			0, 24,
    "", "You calm down as the rage leaves you.",
    "$n calms down as the rage leaves $m.", FALSE, SLOT(80)
  },
  {
    "bite",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 9, L_APP, L_APP, L_APP, L_APP },
    spell_null,			TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_bite,			0, 12,
    "vicious bite", "!Bite!", "", FALSE, SLOT(0)
  }, 
  {
    "adrenaline rush",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 27, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,	POS_RESTING,
    &gsn_rush,			0, 12,
    "",
    "You feel the rush of adrenaline wash away.", 
    "$n has calmed down.", FALSE, SLOT(81)
  },

  {
    "howl of fear",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 60, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_howlfear,		0, 24,
    "", "You shake off the terror and gather your senses.",
    "$n doesn't look as terrified anymore.", FALSE, SLOT(82)
  },

  {
    "scent",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 60, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_RESTING,
    &gsn_scent,			0, 0,
    "", "!Scent!", "", FALSE, SLOT(0)
  },
  {
    "frenzy",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 75, L_APP, L_APP, L_APP, L_APP },
    spell_null,       TAR_IGNORE,       POS_FIGHTING,
    &gsn_frenzy,     0, 0,
    "",        "You feel more sane.",
    "$n looks more sane.", FALSE, SLOT(82)
  },
  {
    "enhanced reflexes",
    { L_APP, L_APP, 70, 79, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 70, 76, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_SELF,	POS_STANDING,
    &gsn_reflex,	0, 36,
    "", "Your reflexes slow down.", "$n slows down.", FALSE, SLOT(84)
  },
  {
    "rake eyes",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, 55, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
    &gsn_rake,		0, 36,
    "eye rake", "!rake!", "", FALSE, SLOT(0)
  },

  {
    "song of sanctuary",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 41, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_sanctuary,    TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       75,     12,
    "",                 "The white aura around your body fades.", 
    "The white aura surrounding $n fades away.", TRUE, SLOT(85)
  },

  {
    "missiles",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 3, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_magic_missile,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       15,     12,
    "&Pmag&pic mis&Psile",	"!Magic Missile!", "", FALSE, SLOT(0)
  },

   {
    "voices",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 3, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_ventriloquate,        TAR_IGNORE,             POS_STANDING,
    NULL,                       5,      12,
    "",                 "!Ventriloquate!", "", FALSE, SLOT(0)
  },

  {
    "song of evil sight",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },   
    spell_detect_evil,  TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       5,      12,
    "",                 "The red in your vision disappears.", 
    "$n's vision returns to normal.", TRUE, SLOT(86)
  },

  {
    "icy touch",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 7, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_chill_touch,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       15,     12,
    "&Cch&wil&Cli&wng &Cto&wu&Cch&X",
    "You feel less cold.", "$n looks less cold.", TRUE, SLOT(87)
  },

   {
    "song of alignment",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_know_alignment,       TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
    NULL,                       9,      12,
    "",                 "!Know Alignment!", "", FALSE, SLOT(0)
  },

  {
    "night vision",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 9, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_infravision,  TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       5,      18,
    "",                 "You no longer see in the dark.", 
    "$n's vision returns to normal.", TRUE, SLOT(88)
  },

  {
    "protection",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 10, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_armor,                TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       5,      12,
    "",                 "You feel less protected.", 
    "$n looks less protected.", TRUE, SLOT(89)
  },

  {
    "fiery touch",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 10, L_APP, 
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_burning_hands,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       15,     12,
    "&rbu&Yrni&Wng &rh&Oa&Yn&Wds&X","!Burning Hands!", "", FALSE, SLOT(0)
  },

  {
    "soothing",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 10, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_cure_light,   TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
    NULL,                       10,     12,
    "",                 "!Cure Light!", "", FALSE, SLOT(0)
  },

  {
    "song of identify",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },   
    spell_identify,             TAR_OBJ_INV,            POS_STANDING,
    NULL,                       12,     24,
    "",                 "!Identify!", "", FALSE, SLOT(0)
  },

  {
    "sight",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 13, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_detect_magic, TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       5,      12,
    "",                 "The detect magic wears off.", 
    "$n's vision returns to normal.", TRUE, SLOT(90)
  },

  {
    "flight",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 17, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_fly,          TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       10,     18,
    "",                 "You slowly float to the ground.", 
    "$n slowly floats to the ground.", TRUE, SLOT(91)
  },

   {
    "shocking",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 15, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_shocking_grasp,       TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       15,     12,
    "&Ysh&Bo&Yck&Bin&Yg g&Bra&Ysp",	"!Shocking Grasp!", "",
FALSE, SLOT(0)
  },
 
  {
    "enticement",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 21, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_charm_person, TAR_CHAR_OFFENSIVE,     POS_STANDING,
    &gsn_charm_person,  5,      12,
    "",                 "You feel more self-confident.", 
    "$n looks more confident.", TRUE, SLOT(92)
  },

  {
    "vision",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 21, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_scry,         TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       35,     20,
    "",                 "Your vision returns to normal.", 
    "$n's vision returns to normal.", TRUE, SLOT(93)
  },

  {
    "giants",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 7, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_giant_strength,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       20,     12,
    "",                 "You feel weaker.", 
    "$n looks much weaker.", TRUE, SLOT(94)
  },

  {
    "seeing",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 28, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_detect_invis, TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       5,      12,
    "",                 "You no longer see invisible objects.", 
    "$n's vision returns to normal.", TRUE, SLOT(95)
  },

  {
    "flames",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 23, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_flamestrike,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       20,     12,
    "&rflame&Rstrike",		"!Flamestrike!", "", FALSE, SLOT(0)
  },

  {
    "location",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 29, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_locate_object,        TAR_IGNORE,             POS_STANDING,
    NULL,                       20,     18,
    "",                 "!Locate Object!", "", FALSE, SLOT(0)
  },

  {
    "invisibility",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 31, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_invis,                TAR_CHAR_DEFENSIVE,     POS_STANDING,
    &gsn_invis,         5,      12,
    "",                 "You are no longer invisible.", 
    "$n appears from thin air.", TRUE, SLOT(96)
  },

  {
    "homeward bound",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 31, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_word_of_recall,       TAR_CHAR_SELF,          POS_RESTING,
    NULL,                       5,      12,
    "",                 "!Word of Recall!", "", FALSE, SLOT(0)
  },

  {
    "poison sight",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 32, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_detect_poison,        TAR_OBJ_INV,            POS_STANDING,
    NULL,                       5,      12,
    "",                 "!Detect Poison!", "", TRUE, SLOT(97)
  },

  {
    "shielding",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 19, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_shield,               TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                       12,     18,
    "",                 "Your force shield shimmers then fades away.", 
    "The force shield surrounding $n shimmers and fades away.", FALSE, SLOT(98)
  },

  {
    "boosting",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 25, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_adrenaline_control,       TAR_CHAR_SELF,  POS_STANDING,
    NULL,                           6,      12,
    "",                             "The adrenaline rush wears off.", 
    "$n doesn't seem so pumped up anymore.", FALSE, SLOT(99)
  },

  {
    "song of acid blast",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 35, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_acid_blast,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       20,     12,
    "&gacid blast&X",		"!Acid Blast!", "", FALSE, SLOT(0)
  },

  {
    "song of colour spray",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 34, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_colour_spray, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                       15,     12,
    "&rc&go&cl&Yo&Gu&Rr &rsp&gr&Ba&Gy&X","!Colour Spray!", "",
FALSE, SLOT(0)
  },

  {
    "song of hidden sight",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 43, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_detect_hidden,        TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       5,      12,
    "",                 "You feel less aware of your suroundings.", 
    "$n's vision returns to normal.", TRUE, SLOT(100)
  },

  {
    "song of cursing",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 35, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_curse,                TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    &gsn_curse,                 20,     12,
    "curse",            "The curse wears off.", 
    "The curse within $n vanishes.", TRUE, SLOT(101)
  },

  {
    "song of earthquake",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 41, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_earthquake,           TAR_IGNORE,             POS_FIGHTING,
    NULL,                       15,     12,
    "&Oearthquake&X",		"!Earthquake!", "", FALSE, SLOT(0)
  },

  {
    "song of faerie fire",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 34, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_faerie_fire,          TAR_CHAR_OFFENSIVE,    POS_FIGHTING,
    NULL,                       5,      12,
    "faerie fire",       "The pink aura around you fades away.", 
    "The pink aura enveloping $n slowly fades away.", TRUE, SLOT(102)
  },

  {
    "song of astral travel",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 54, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_astral,               TAR_IGNORE,            POS_STANDING,
    NULL,                       50,     12,
    "",                  "!Astral Walk!", "", FALSE, SLOT(0)
  },

  {
    "song of the ghosts",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 57, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_pass_door,            TAR_CHAR_SELF,         POS_STANDING,
    NULL,                       20,     12,
    "",                  "You feel solid again.", 
    "$n's body appears more solid.", TRUE, SLOT(103)
  },

  {
    "song of revealing",
   { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 59, L_APP,
     L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
   spell_faerie_fog,            TAR_IGNORE,           POS_STANDING,
   NULL,                        12,     12,
   "faerie fog",         "!Faerie Fog!", "", FALSE, SLOT(0)
  },

  {
   "song of stone",
   { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 49, L_APP,
     L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
   spell_stone_skin,            TAR_CHAR_SELF,        POS_STANDING,
   NULL,                        12,     18,
   "",                   "Your skin feels soft again.", 
   "$n's skin takes on a softer texture.", TRUE,  SLOT(104)
  },

  {
   "lullaby",
   { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 64, L_APP,
     L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
   spell_sleep,                 TAR_CHAR_OFFENSIVE,   POS_STANDING,
   &gsn_sleep,                  15,     12,
   "",                   "You feel less tired.",        
   "$n looks more energetic.", TRUE,  SLOT(105)
  },

  {
   "song of fiery death",
   { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 58, L_APP,
     L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_fireball,             TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
    NULL,                       15,     12,
    "&Rfireball&X",		"!Fireball!", "", FALSE, SLOT(0)
  },

  {
   "song of cancellation",
   { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 86, L_APP,
     L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
   spell_cancellation,          TAR_CHAR_DEFENSIVE,   POS_FIGHTING,
   NULL,                        15,     16,
   "",                   "!Cancellation!",              "", FALSE, SLOT(0)
  },

  {
   "song of sight",
   { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 90, L_APP,
     L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
   spell_truesight,             TAR_CHAR_SELF,        POS_STANDING,
   NULL,                        59,     12,  
   "",                  "Your vision is no longer so true.", 
   "$n's vision returns to normal.", TRUE, SLOT(106)
  },

  {
   "song of fireshield",
   { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 71, L_APP,
     L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
   spell_fireshield,            TAR_CHAR_DEFENSIVE,   POS_STANDING,
   NULL,                        75,     12,
   "",               "The flames engulfing you body burn out.", 
   "The flames surrounding $n suddenly burn out.", TRUE, SLOT(107)
  },

  {
   "song of portal",
   { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 91, L_APP,
     L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
   spell_portal,                TAR_IGNORE,           POS_STANDING,
   NULL,                        100,    12,
   "",               "",                            "", FALSE, SLOT(0)
  },

  {
   "song of healing",
   { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 47, L_APP,
     L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
   spell_heal,                  TAR_CHAR_DEFENSIVE,   POS_FIGHTING,
   NULL,                        50,     12,
   "",              "!Heal!",                       "", FALSE, SLOT(0)
  },
  {
    "track",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,    30, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,         TAR_IGNORE,             POS_STANDING,
    &gsn_track,         0,       0,
    "",                 "!Track!",              "", FALSE, SLOT(0)
  }, 
  {
    "headbutt",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_headbutt,	0,	0,
    "headbutt",			"!Headbutt!",		"", FALSE, SLOT(0)
  },
  {
    "pass plant",
    { L_APP, L_APP, L_APP, L_APP, L_APP,    80, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_pass_plant,               TAR_IGNORE,             POS_STANDING,
    NULL,                       50,     12,
    "",                 "!PASS PLANT!", "", FALSE, SLOT(0)
  },

  {
    "flamehand",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 35, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_SELF,		POS_STANDING,
    &gsn_flamehand,	0,	0,
    "",	"The &rflames &won your hands flicker and vanish.",
    "The &rflames &won $n's hands flicker and vanish.", FALSE, SLOT(108)
  },
  {
    "frosthand",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 60, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_SELF,		POS_STANDING,
    &gsn_frosthand,	0,	0,
    "",	"The &Cfrost&w and &Cice&w melt from your hands.",
    "The &Cfrost&w and &Cice&w melt from $n's hands.", FALSE, SLOT(109)
  },
  {
    "chaoshand",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 85, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_SELF,		POS_STANDING,
    &gsn_chaoshand,	0,	0,
    "",	"Your hands stop flickering, and order overtakes the &Ychaos&w.",
    "$n's hands stop flickering, and order overtakes the &Ychaos&w.",
    FALSE, SLOT(110)
  },
  {
    "bladepalm",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 55, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_bladepalm,	0,	18,
    "blade palm",	"!Blade Palm!",		"", FALSE, SLOT(0)
  },
  {
    "flying kick",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 50, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_flykick,	0,	12,
    "flying kick",	"!Flying Kick!", "", FALSE, SLOT(0)
  },
  {
    "anatomy knowledge",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 75, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_STANDING,
    &gsn_anatomyknow,	0,	0,
    "",			"!Anatomy Knowledge!",		"", FALSE, SLOT(0)
  },
  {
    "blackbelt",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 13, L_APP, L_APP, L_APP },
    spell_null,         TAR_IGNORE,             POS_FIGHTING,
    &gsn_blackbelt,     0,      0,
    "",        "!Blackbelt!", "", FALSE, SLOT(0)
  },
  {
    "nerve",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 95, L_APP, L_APP, L_APP },
    spell_null,         TAR_CHAR_OFFENSIVE,             POS_FIGHTING,
    &gsn_nerve,     0,      24,
    "",        "You can feel your arms again.", 
    "Feeling returns to $n's arms.", FALSE, SLOT(111)
  },
  {
    "soulbind",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      100, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_soul_bind,         TAR_CHAR_OFFENSIVE,             POS_FIGHTING,
    NULL,     200,      18,
    "",        "!SOULBIND!", "", FALSE, SLOT(0)
  },
  {
    "iron skin",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 65, L_APP, L_APP, L_APP },
    spell_iron_skin,      TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   50,     12,
    "",
    "Your skin returns to normal.",
    "$n's skin returns to its normal texture.", TRUE, SLOT(112)
  },
  {
    "chi shield",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 45, L_APP, L_APP, L_APP },
    spell_chi_shield,      TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   75,     12,
    "", "Your chi shield dissipates.", 
    "The chi shield surrounding $n dissipates.", FALSE, SLOT(113)
  },
  {
    "ironfist",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, 25, L_APP, L_APP, L_APP },
    spell_null,      TAR_CHAR_SELF,          POS_STANDING,
    &gsn_ironfist,     0,     12,
    "", "Your fists are flesh once again.", 
    "$n's fists return to a normal structure.", FALSE, SLOT(114)
  },
  {
    "Globe of Darkness",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_globedark,		50,	0,
    "&zYou raise your hand and the globe dissipates.",	
    "You are no longer able to sustain your &zglobe &cin &w:&W",
    "The globe of darkness about $n dissipates.", TRUE, SLOT(0)
  },
  {
    "drow fire",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_drowfire,	0,	12,
    "drow fire",
    "The purple aura around you fades away.",
    "$n's outline fades away.",
    TRUE, SLOT(0)
  },

  {
    "snatch",
    { L_APP, L_APP, 45, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_snatch,		0,	12,
    "",			"!Steal!", "", FALSE, SLOT(0)
  },

  {
    "enhanced damage three",
    { L_APP, L_APP, L_APP, 55, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_enhanced_three,	0,	 0,
    "",			"!Enhanced Damage Two!", "", FALSE, SLOT(0)
  },

  {
    "retreat",
    { L_APP, L_APP, 25, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_retreat,	0,	 0,
    "",			"!Run Away:P!", "", FALSE, SLOT(0)
  },


  {
    "antidote",
    { L_APP, L_APP, 57, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_antidote,	0,	 0,
    "",			"", "", FALSE, SLOT(0)
  },
  {
    "haggle",
    { L_APP, L_APP, 8, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_haggle,	0,	 0,
    "",			"", "", FALSE, SLOT(0)
  },
  {
    "blind fighting",
    { L_APP, L_APP, 55, 45, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_blindfight,	0,	 0,
    "",			"", "", FALSE, SLOT(0)
  },
  {
    "enhanced dodge",
    { L_APP, L_APP, 20, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_dodge_two,	0,	 0,
    "",			"", "", FALSE, SLOT(0)
  },

 {
    "ethereal mist", 
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 39,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },   
    spell_pass_door,    TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       20,     12,
    "",                 
    "You feel solid again.",
    "$n's body becomes more solid.", TRUE, SLOT(115)
  },
{
    "mist form",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 42,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_mist_form,  TAR_CHAR_SELF,          POS_STANDING,
    NULL,                       100,    12,
    "",                 "A strange feeling comes over you as you solidify.",
    "$n's body returns to it's normal structure.",
    TRUE, SLOT(116)
  },
  {
    "golden aura",
    { L_APP, 60, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_golden_aura,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			75,	12,
    "",
    "The golden aura around your body vanishes.",
    "The golden aura around $n's body vanishes.", TRUE, SLOT(117)
  },
  {
    "shriek",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 43, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
    &gsn_shriek,	0,	 16,
    "shriek",		"", "", FALSE, SLOT(0)
  },
  {
    "acidic spit",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,		TAR_IGNORE,		POS_FIGHTING,
    &gsn_spit,	0,	0,
    "&gacidic &Gspit",	"The corosive spit finally evaporates.",
    "The acidic spit burning $n evaporates.", FALSE, SLOT(118)
  },
  {
    "ward of safety", 
    { L_APP, 101, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,       TAR_IGNORE,        POS_STANDING,
    &gsn_ward_safe,			100,	12,
    "You remove the ward of safety from the room.",
    "You are no longer able to sustain your &zward of safety &cin &w:&W",
    "The wards of safety crumble.", FALSE, SLOT(0)
  },
  {
    "ward of healing", 
    { L_APP, 101, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_null,       TAR_IGNORE,        POS_STANDING,
    &gsn_ward_heal,			200,	12,
    "You remove the ward of healing from the room.",
    "You are no longer able to sustain your &Wward of healing &cin &w:&W",
    "The wards of healing crumble.", FALSE, SLOT(0)
  },

  {
    "thunder strike",
    { L_APP, L_APP, L_APP, L_APP, L_APP, 70, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_thunder_strike,		TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,			15,	12,
    "thunder strike",		"!thunder strike!", "", SLOT(0)
  },

  {
    "inspiration",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 50, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_inspiration,		TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,			15,	12,
    "",		"The inspiration you once felt vanishes.", 
    "The inspiration within $n disappears.", FALSE, SLOT(0)
  },

  {
    "warcry",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, 60, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
     spell_war_cry,		TAR_CHAR_OFFENSIVE, POS_FIGHTING,
     NULL,			15,	12,
     "war cry",			"!War cry!", "", SLOT(0)
  },  

  {
    "group healing",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
     spell_group_healing,       TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
     NULL,                       50,     12,
     "",                 "!Group Heal!", "", FALSE, SLOT(0)  
  },

  {
    "chant",
    { L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
     spell_chant,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
     NULL,			50,	12,
     "",		"!Chant!", "", FALSE, SLOT(0)
  },

  {
    "aid",
    { L_APP, 36, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_aid,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
    NULL,			30,	12,
    "",
    "You feel divine aid disipate.",
    "$n looks less righteous.",
    FALSE, SLOT(199)
  },

  {
    "bark skin",
    { L_APP, L_APP, L_APP, L_APP, L_APP, 40, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_bark_skin,	TAR_CHAR_SELF,		POS_STANDING,
    NULL,			8,	18,
    "",
    "Your skin returns to normal.",
    "$n's skin returns to normal.",
    TRUE, SLOT(200)
  },

  {
    "blur",
    { 10, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_blur,     TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   15,     12,
    "",
    "You are no longer blured.",
    "$n's blured form focused suddenly.", FALSE, SLOT(201)
  },

  {
    "firewall",
    { 50, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_firewall,	TAR_IGNORE,		POS_FIGHTING,
    NULL,			55,	22,
    "&Rroaring flames&X",   	"!FIRWALL!", "", FALSE, SLOT(0)
  },

  {
    "draw strength", 
    { L_APP, L_APP, L_APP, L_APP, 82, L_APP, L_APP, L_APP, L_APP, L_APP,
      L_APP, L_APP, L_APP, L_APP, L_APP, L_APP },
    spell_draw_strength,  TAR_CHAR_SELF,        POS_STANDING,
    NULL,			80,	40,
    "",		"Strength returns to your body.",     
    "$n's doesn't look so week anymore.", FALSE, SLOT(202)
  },

{
    "scrolls",
    { 3, 5, 15, 20, 8, 7, 18, 18, 12, 17, 7, 20, 18, L_APP, L_APP, L_APP },
    spell_null,           TAR_IGNORE,   POS_STANDING,
    &gsn_scrolls,    0, 0,
    "&Wfailure",    "!Scrolls!", "", FALSE, SLOT(0)
  },

{
    "wands",
    { 3, 5, 15, 20, 8, 7, 18, 18, 12, 17, 7, 20, 18, L_APP, L_APP, L_APP },
    spell_null,           TAR_IGNORE,   POS_STANDING,
    &gsn_wands,    0, 0,
    "&Wfailure",    "!Wands!", "", FALSE, SLOT(0)
  },

{
    "staves",
    { 3, 5, 15, 20, 8, 7, 18, 18, 12, 17, 7, 20, 18, L_APP, L_APP, L_APP },
    spell_null,           TAR_IGNORE,   POS_STANDING,
    &gsn_staves,    0, 0,
    "&Wfailure",    "!Staves!", "", FALSE, SLOT(0)
  },

/*
 * Place all new spells/skills BEFORE this one.  It is used as an index marker
 * in the same way that theres a blank entry at the end of the command table.
 * (in interp.c)
 * -- Altrag
 */
  {
    "",
    {999,999,999,999,999,999,999,999,999,999,
     999,999,999,999,999,999},
    spell_null,     0, 0,
    NULL, 0, 0,
    "", "", "", SLOT(0)
  }
};

const struct gskill_type gskill_table [MAX_GSPELL] =
{
  /*
   * The globals for group spells..
   * -- Altrag
   */
/*{wait,SLOT(slot),{MAG,CLE,THI,WAR,PSI,DRU,RAN,PAL,BAR,VAM,
		    NEC,WWF,MNK,PRO,PRO,PRO},*/
  { 3, SLOT(221), {0,0,0,0,0,0,0,0,0,1,0} },
  { 2, SLOT(227), {2,1,0,0,2,1,0,0,0,0,0} },
};
