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
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *  Mitchell Tse.                                                          *
 *                                                                         *
 *  EnvyMud 2.2 improvements copyright (C) 1996, 1997 by Michael Quan.     *
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

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"



/*
 * Clan table (by Zen).
 */
const	struct	clan_type	clan_table	[MAX_CLAN]	=
{
    /*	name,			clan_vest,		castle	*/
    {	"none",			OBJ_VNUM_SCHOOL_VEST,	3001	},
    {	"Shining Clan",		3383,			3001	},
    {	"Klowns Clan",		3383,			3001	},
    {	"Berserkers Clan",	3383,			3001	},
    {	"Krushers Clan",	3383,			3001	},
    {	"White Dragon Clan",	3383,			3001	}
};


#define CLASS_MAGE       0
#define CLASS_CLERIC     1
#define CLASS_THIEF      2
#define CLASS_WARRIOR    3
#define CLASS_PSIONICIST 4

/*
 * Immort Levels
 */
#define L_HER            LEVEL_HERO


/* 
 * Race types
 */
const   struct    race_type       race_table      [ MAX_RACE ]    =
{
    { "Human",     RACE_PC_AVAIL | RACE_WEAPON_WIELD,      3, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, "punch", "Githyanki Vampire Werewolf Mindflayer"      },
    { "Elf",       RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN      |
	           RACE_WEAPON_WIELD,                      2, 0, 1, 0, 1, -1,
	0, 4, 0, 0, 0, "punch",
	"Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold"                                                                    },
    { "Halfelf",   RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	                                                   3, 0, 0, 0, 1, 0,
	0, 2, 0, 0, 0, "punch",
	"Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin"},
    { "Drow",      RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN      |
	           RACE_WEAPON_WIELD,                      2, 0, 0, 1, 1, 0,
	0, 4, 0, 0, 0, "punch",
	"Elf Halfelf Hobbit Githyanki Vampire Werewolf"             },
    { "Dwarf",     RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN      |
	           RACE_WEAPON_WIELD,                      2, 0, 0, 0, -1, 1,
	0, 0, 0, 0, 1, "punch",
	"Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold"                                                          },
    { "Halfdwarf", RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	                                                   2, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, "punch",
	"Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin"                                                                     },
    { "Hobbit",    RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN      |
	           RACE_WEAPON_WIELD,                      2, 0, 0, 0, 1, -1,
	0, 0, 0, 0, 0, "punch",
	"Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold"                                                          },
    { "Gnome",     RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	                                                   2, -1, 0, 1, 1, -1,
	0, 4, 0, 0, 0, "punch",
	"Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin"},
    { "Halfkobold", RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	                                                  2, -2, -1, -2, 3, -2,
	0, 0, 0, 0, 0, "punch", "Ogre Orc Giant Troll Hobgoblin"             },
    { "Giant",     RACE_WEAPON_WIELD,                      6, 2, -1, 0, -1, 1,
	3, -5, -2, 2, 4, "fist",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome"          },
    { "Ogre",      RACE_WEAPON_WIELD,                      6, 1, -1, 0, -1, 1,
	2, -3, -1, 0, 2, "fist",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome"          },
    { "Orc",       RACE_INFRAVISION | RACE_WEAPON_WIELD,   4, 1, -1, 0, 0, 0,
	2, -1, -1, 0, 1, "punch",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome"          },
    { "Kobold",    RACE_INFRAVISION | RACE_WEAPON_WIELD,   2, -1, -1, 0, 1, 0,
	0, 0, 0, 0, 0, "punch",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome Halfkobold"},
    { "Minotaur",  RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD, 5, 2, 0, 0, -1, 1,
	3, 0, -1, 0, 3, "fist",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome"          },
    { "Troll",     RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	                                                   7, 2, -1, 0, 0, 1,
	10, 0, 0, 0, 8, "fist",
	"Human Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome"    },
    { "Hobgoblin", RACE_INFRAVISION | RACE_WEAPON_WIELD,   3, 1, 0, -1, 0, 1,
	0, 0, 0, 0, 0, "punch",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome"          },
    { "Insect",    RACE_MUTE,                              0, 0, 0, 0, 0, -1,
	0, 0, 5, 0, 0, "bite", ""                                            },
    { "Dragon",    RACE_FLY | RACE_INFRAVISION | RACE_DETECT_ALIGN |
	           RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	                                                   9, 2, 2, 1, -3, 2,
	15, 0, -10, 0, 10, "claw", ""                                        },
    { "Animal",    RACE_DETECT_HIDDEN | RACE_MUTE,         2, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, "bite", "Kobold Halfkobold"                           },
    { "God",       RACE_WATERBREATH | RACE_FLY | RACE_SWIM | RACE_WATERWALK   |
	           RACE_PASSDOOR | RACE_INFRAVISION | RACE_DETECT_ALIGN       |
		   RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_PROTECTION   |
		   RACE_SANCT | RACE_WEAPON_WIELD,         8, 3, 3, 3, 3, 3,
	20, 0, 10, 0, 0, "smite", ""                                         },
    { "Undead",    RACE_PASSDOOR | RACE_INFRAVISION | RACE_DETECT_ALIGN       |
	           RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	                                                   3, 1, 0, 0, -2, 1,
	0, 0, 0, -1, -1, "touch",
	"Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Goblin Faerie Gnome"                                  },
    { "Harpy",     RACE_FLY | RACE_DETECT_INVIS | RACE_MUTE,
	                                                   3, 0, 0, 0, 2, 0,
	0, 0, 6, 0, 0, "claw",
	"Human Elf Halfelf Dwarf Halfdwarf Hobbit Gnome"                     },
    { "Bear",      RACE_SWIM | RACE_DETECT_HIDDEN | RACE_MUTE,
	                                                   3, 1, 0, 0, -1, 1,
	0, 0, 0, 0, 3, "swipe", ""                                           },
    { "Githyanki", RACE_WEAPON_WIELD,                      3, 0, 1, 0, 0, 0,
	0, 4, 0, 0, 0, "punch", "Mindflayer"                                 },
    { "Elemental", RACE_MUTE,                              4, 1, 0, 0, 0, 1,
	0, 0, 0, -1, -1, "punch", ""                                         },
    { "Bat",       RACE_FLY | RACE_INFRAVISION | RACE_MUTE,
	                                                   1, -1, 0, 0, 2, -1,
	0, 0, 4, 0, 0, "bite", ""                                            },
    { "Plant",     RACE_MUTE,                              1, 0, 0, 0, -1, 1,
	0, 0, 0, -1, 0, "swipe", ""                                          },
    { "Rat",       RACE_PASSDOOR | RACE_MUTE,              0, -1, 0, 0, 2, -1,
	0, 0, 0, 0, 0, "bite", ""                                            },
    { "Vampire",   RACE_FLY | RACE_PASSDOOR | RACE_INFRAVISION                |
	           RACE_DETECT_ALIGN | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN |
		   RACE_WEAPON_WIELD,                      3, 1, 1, 0, 1, 2,
	10, 3, 3, -1, 10, "claw",
	"Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Werewolf Goblin Faerie Gnome"                         },
    { "Werewolf",  RACE_INFRAVISION | RACE_DETECT_ALIGN | RACE_DETECT_INVIS   |
	           RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD, 3, 2, 0, 1, 2, 3,
	10, 0, 5, 0, 5, "claw",
	"Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Vampire Goblin Faerie Gnome"                          },
    { "Goblin",    RACE_INFRAVISION | RACE_WEAPON_WIELD,   2, -1, -1, -1, 1, 0,
	0, 0, 0, 0, 0, "punch",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome"          },
    { "Faerie",    RACE_FLY | RACE_INFRAVISION | RACE_DETECT_INVIS            |
	           RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD, 1, -2, 1, 1, 1, -1,
	0, 0, 7, 0, 0, "punch", ""                                           },
    { "Arachnid",  RACE_NO_ABILITIES | RACE_WEAPON_WIELD | RACE_MUTE,
	                                                   2, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, "bite", ""                                            },
    { "Mindflayer",RACE_INFRAVISION | RACE_WEAPON_WIELD,   3, 1, 2, 1, -1, 0,
	0, 6, 0, 0, 0, "punch", "Githyanki"                                  },
    { "Object",    RACE_WATERBREATH | RACE_MUTE,           3, 3, 0, 0, 0, 3,
	0, 0, 0, -1, -1, "swing", ""                                         },
    { "Mist",      RACE_FLY | RACE_PASSDOOR | RACE_MUTE,   2, -1, 0, 0, 3, 0,
	0, 0, 20, -1, -1, "gas", ""                                          },
    { "Snake",     RACE_MUTE,                              1, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, "bite", ""                                            },
    { "Worm",      RACE_PASSDOOR | RACE_MUTE,              0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, "slime", ""                                           },
    { "Fish",      RACE_WATERBREATH | RACE_SWIM | RACE_MUTE,
	                                                   1, 0, 0, 0, 2, 0,
	0, 0, 0, -1, 0, "slap", ""                                           },
    { "Hydra",     RACE_DETECT_HIDDEN | RACE_MUTE,         8, 2, 0, 0, -1, 2,
	0, 0, 0, 0, 0, "bite", ""                                            },
    { "Lizard",    RACE_MUTE,                              1, -1, 0, 0, 1, 0,
	0, 0, 0, 0, 0, "lash", ""                                            }
};


/*
 * Drunkeness string changes.
 */
const   struct  struckdrunk     drunk           [  ]            =
{
    { 3, 10,
	{ "a", "a", "a", "A", "aa", "ah", "Ah", "ao", "aw", "oa", "ahhhh" }  },
    { 8, 5,
	{ "b", "b", "b", "B", "B", "vb" }                                    },
    { 3, 5,
	{ "c", "c", "C", "ch", "sj", "zj" }                                  },
    { 5, 2,
	{ "d", "d", "D" }                                                    },
    { 3, 3,
	{ "e", "e", "eh", "E" }                                              },
    { 4, 5,
	{ "f", "f", "ff", "fff", "fFf", "F" }                                },
    { 8, 3,
	{ "g", "g", "G", "jeh" }                                             },
    { 9, 6,
	{ "h", "h", "hh", "hhh", "Hhh", "HhH", "H" }                         },
    { 7, 6,
	{ "i", "i", "Iii", "ii", "iI", "Ii", "I" }                           },
    { 9, 5,
	{ "j", "j", "jj", "Jj", "jJ", "J" }                                  },
    { 7, 3,
	{ "k", "k", "K", "kah" }                                             },
    { 3, 2, 
	{ "l", "l", "L" }                                                    },
    { 5, 8,
	{ "m", "m", "mm", "mmm", "mmmm", "mmmmm", "MmM", "mM", "M" }         },
    { 6, 6,
	{ "n", "n", "nn", "Nn", "nnn", "nNn", "N" }                          },
    { 3, 6,
	{ "o", "o", "ooo", "ao", "aOoo", "Ooo", "ooOo" }                     },
    { 3, 2,
	{ "p", "p", "P" }                                                    },
    { 5, 5,
	{ "q", "q", "Q", "ku", "ququ", "kukeleku" }                          },
    { 4, 2,
	{ "r", "r", "R" }                                                    },
    { 2, 5,
	{ "s", "ss", "zzZzssZ", "ZSssS", "sSzzsss", "sSss" }                 },
    { 5, 2,
	{ "t", "t", "T" }                                                    },
    { 3, 6,
	{ "u", "u", "uh", "Uh", "Uhuhhuh", "uhU", "uhhu" }                   },
    { 4, 2,
	{ "v", "v", "V" }                                                    },
    { 4, 2,
	{ "w", "w", "W" }                                                    },
    { 5, 6,
	{ "x", "x", "X", "ks", "iks", "kz", "xz" }                           },
    { 3, 2,
	{ "y", "y", "Y" }                                                    },
    { 2, 8,
	{ "z", "z", "ZzzZz", "Zzz", "Zsszzsz", "szz", "sZZz", "ZSz", "zZ"}   }
};
      
/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[26]		=
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
    { 10, 12, 999, 60 }  /* 25   */
};



const	struct	int_app_type	int_app		[26]		=
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
    { 99 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
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
    { 8 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
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
    { -105 },
    { -120 }    /* 25 */
};



const	struct	con_app_type	con_app		[26]		=
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
    {  8, 99 }    /* 25 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[LIQ_MAX]	=
{
/*    name                      colour       drunk, food, thirst */
    { "water",			"clear",	{  0, 0, 10 }	},  /*  0 */
    { "beer",			"amber",	{  3, 2,  5 }	},
    { "wine",			"rose",		{  5, 2,  5 }	},
    { "ale",			"brown",	{  2, 2,  5 }	},
    { "dark ale",		"dark",		{  1, 2,  5 }	},

    { "whisky",			"golden",	{  6, 1,  4 }	},  /*  5 */
    { "lemonade",		"pink",		{  0, 1,  8 }	},
    { "firebreather",		"boiling",	{ 10, 0,  0 }	},
    { "local specialty",	"everclear",	{  3, 3,  3 }	},
    { "slime mold juice",	"green",	{  0, 4, -8 }	},

    { "milk",			"white",	{  0, 3,  6 }	},  /* 10 */
    { "tea",			"tan",		{  0, 1,  6 }	},
    { "coffee",			"black",	{  0, 1,  6 }	},
    { "blood",			"red",		{  0, 2, -1 }	},
    { "salt water",		"clear",	{  0, 1, -2 }	},

    { "cola",			"cherry",	{  0, 1,  5 }	},   /* 15 */
    { "white wine",		"golden",	{  5, 2,  5 }	},
    { "root beer",		"brown",	{  0, 3,  6 }	},
    { "champagne",		"golden",	{  5, 2,  5 }	},
    { "vodka",			"clear",	{  7, 1,  4 }	},

    { "absinth",		"green",	{ 10, 0,  0 }	},   /* 20 */
    { "brandy",			"golden",	{  5, 1,  4 }	},
    { "schnapps",		"clear",	{  6, 1,  4 }	},
    { "orange juice",		"orange",	{  0, 2,  8 }	},
    { "sherry",			"red",		{  3, 2,  4 }	},

    { "rum",			"amber",	{  8, 1,  4 }	},   /* 25 */
    { "port",			"red",		{  3, 3,  4 }	}

};



struct	skill_type	skill_table	[MAX_SKILL]	=
{

/*
 * Magic spells.
 */

    {
	"reserved",		{},
	0,			TAR_IGNORE,		POS_STANDING,
	NULL,			0,	 0,
	"",			""
    },

    {
	"acid blast",		{},
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"acid blast",		"!Acid Blast!"
    },

    {
	"armor",		{},
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected."
    },

    {
	"bless",		{},
	spell_bless,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less righteous."
    },

    {
	"blindness",		{},
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,		5,	12,
	"",			"You can see again."
    },

    {
	"breathe water",	{},
	spell_breathe_water,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_breathe_water, 	 5,	12,
	"pained lungs",		"You can no longer breathe underwater."
    },

    {
	"burning hands",	{},
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_burning_hands,     15,	12,
	"burning hands",	"!Burning Hands!"
    },

    {
	"call lightning",	{},
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			15,	12,
	"lightning bolt",	"!Call Lightning!"
    },

    {
	"cause critical",	{},
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"invocation",		"!Cause Critical!"
    },

    {
	"cause light",		{},
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"invocation",		"!Cause Light!"
    },

    {
	"cause serious",	{},
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			17,	12,
	"invocation",		"!Cause Serious!"
    },

    {
	"change sex",		{},
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	0,
	"",			"Your body feels familiar again."
    },

    {
	"charm person",		{},
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	5,	12,
	"",			"You feel more self-confident."
    },

    {
	"chill touch",		{},
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"chilling touch",	"You feel less cold."
    },

    {
	"colour spray",		{},
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"colour spray",		"!Colour Spray!"
    },

    {
        "cone of silence",	{},
        spell_cone_of_silence,	TAR_IGNORE,		POS_FIGHTING,
        NULL,			35,     12,
        "",                     "!Cone of Silence!"
    },

    {
	"continual light",	{},
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,			7,	12,
	"",			"!Continual Light!"
    },

    {
	"control weather",	{},
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,			25,	12,
	"",			"!Control Weather!"
    },

    {
	"create food",		{},
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Create Food!"
    },

    {
	"create spring",	{},
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	12,
	"",			"!Create Spring!"
    },

    {
	"create water",		{},
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Create Water!"
    },

    {
	"cure blindness",	{},
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			5,	12,
	"",			"!Cure Blindness!"
    },

    {
	"cure critical",	{},
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"",			"!Cure Critical!"
    },

    {
	"cure light",		{},
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			10,	12,
	"",			"!Cure Light!"
    },

    {
	"cure poison",		{},
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"!Cure Poison!"
    },

    {
	"cure serious",		{},
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"",			"!Cure Serious!"
    },

    {
	"curse",		{},
	spell_curse,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_curse,		20,	12,
	"curse",		"The curse wears off."
    },

    {
	"destroy cursed",	{},
	spell_destroy_cursed,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	12,
	"",			"!destroy cursed!"
    },

    {
	"detect evil",		{},
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"The red in your vision disappears."
    },

    {
	"detect hidden",	{},
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less aware of your surroundings."
    },

    {
	"detect invis",		{},
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You no longer see invisible objects."
    },

    {
	"detect magic",		{},
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING, 
	NULL,			5,	12,
	"",			"The detect magic wears off."
    },

    {
	"detect poison",	{},
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Detect Poison!"
    },

    {
	"dispel evil",		{},
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"holy fire",		"!Dispel Evil!"
    },

    {
	"dispel magic",		{},
	spell_dispel_magic,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	16,
	"",			"!Dispel Magic!"
    },

    {
	"earthquake",		{},
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			15,	12,
	"earthquake",		"!Earthquake!"
    },

    {
	"enchant weapon",	{},
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			100,	24,
	"",			"!Enchant Weapon!"
    },

    {
	"energy drain",		{},
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			35,	12,
	"energy drain",		"!Energy Drain!"
    },

    {
	"exorcise",		{},
	spell_exorcise, 	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			35,	12,
	"",		        "!Exorcise!"
    },

    {
	"faerie fire",		{},
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			5,	12,
	"faerie fire",		"The pink aura around you fades away."
    },

    {
	"faerie fog",		{},
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,			12,	12,
	"faerie fog",		"!Faerie Fog!"
    },

    {
	"fireball",		{},
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"fireball",		"!Fireball!"
    },

    {
	"flamestrike",		{},
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"flamestrike",		"!Flamestrike!"
    },

    {
	"flaming shield",	{},
	spell_flaming,  	TAR_CHAR_SELF,  	POS_STANDING,
	NULL,			100,	60,
	"flaming shield",	"The flaming shield around you dies out."
    },

    {
	"fly",			{},
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			10,	18,
	"",			"You slowly float to the ground."
    },

    {
	"gate",			{},
	spell_gate,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"",			"!Gate!"
    },

    {
	"giant strength",	{},
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	12,
	"",			"You feel weaker."
    },

    {
	"harm",			{},
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			35,	12,
	"divine power",		"!Harm!"
    },

    {
	"heal",			{},
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"",			"!Heal!"
    },

    {
	"identify",		{},
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			12,	24,
	"",			"!Identify!"
    },

    {
	"infravision",		{},
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	18,
	"",			"You no longer see in the dark."
    },

    {
	"invis",		{},
	spell_invis,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_invis,		5,	12,
	"",			"You are no longer invisible."
    },

    {
	"know alignment",	{},
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			9,	12,
	"",			"!Know Alignment!"
    },

    {
	"lightning bolt",	{},
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"lightning bolt",	"!Lightning Bolt!"
    },

    {
	"locate object",	{},
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	18,
	"",			"!Locate Object!"
    },

    {
	"magic missile",	{},
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"magic missile",	"!Magic Missile!"
    },

    {
	"mass heal",		{},
	spell_mass_heal,	TAR_IGNORE,		POS_STANDING,
	NULL,           	50,	24,
	"",			"!Mass Heal!"
    },

    {
	"mass invis",		{},
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	20,	24,
	"",			"You are no longer invisible."
    },

    {
        "mute",			{},
        spell_mute,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
        &gsn_mute,		20,     12,
        "",                     "You are no longer muted."
    },

    {
	"pass door",		{},
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			20,	12,
	"",			"You feel solid again."
    },

    {
	"poison",		{},
	spell_poison,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_poison,		10,	12,
	"burning blood",	"You feel less sick."
    },

    {
	"polymorph other",	{},
	spell_polymorph_other,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			20,	0,
	"",			"Your body feels familiar again." 
    },

    {
	"protection evil",	{},
	spell_protection_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected."
    },

    {
	"protection good",	{},
	spell_protection_good,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected."
    },

    {                                  
        "recharge item",        {},
        spell_recharge_item,    TAR_OBJ_INV,            POS_STANDING,
        NULL,                   25,     12,
        "blunder",              "!Recharge Item!"
    }, 

    {
	"refresh",		{},
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			12,	18,
	"refresh",		"!Refresh!"
    },

    {
        "remove alignment",	{},
        spell_remove_alignment,	TAR_OBJ_INV,		POS_STANDING,
        NULL,			100,	12,
        "",                     "!Remove Alignment!"
    },

    {
	"remove curse",		{},
	spell_remove_curse,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"!Remove Curse!"
    },

    {
        "remove silence",	{},
        spell_remove_silence,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
        NULL,			15,     12,
        "",                     "!Remove Silence!"
    },

    {
	"sanctuary",		{},
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			75,	12,
	"",			"The white aura around your body fades."
    },

    {
	"shield",		{},
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			12,	18,
	"",			"Your force shield shimmers then fades away."
    },

    {
	"shocking grasp",	{},
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"shocking grasp",	"!Shocking Grasp!"
    },

    {
	"sleep",		{},
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,		15,	12,
	"",			"You feel less tired."
    },

    {
	"stone skin",		{},
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			12,	18,
	"",			"Your skin feels soft again."
    },

    {
	"summon",		{},
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,			50,	12,
	"",			"!Summon!"
    },

    {
	"teleport",		{},
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 		35,	12,
	"",			"!Teleport!"
    },

    {
	"turn undead",		{},
	spell_turn_undead,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_turn_undead,	10,	12,
	"divine exorcism",	"!Turn undead!"
    },

    {
	"ventriloquate",	{},
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Ventriloquate!"
    },

    {
	"weaken",		{},
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"weakening spell",	"You feel stronger."
    },

    {
	"word of recall",	{},
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	NULL,			5,	12,
	"",			"!Word of Recall!"
    },

/*
 * Dragon breath
 */
    {
	"acid breath",		{},
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of acid",	"!Acid Breath!"
    },

    {
	"fire breath",		{},
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of flame",	"!Fire Breath!"
    },

    {
	"frost breath",		{},
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of frost",	"!Frost Breath!"
    },

    {
	"gas breath",		{},
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			50,	12,
	"breath of gas",		"!Gas Breath!"
    },

    {
	"lightning breath",	{},
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of lightning",	"!Lightning Breath!"
    },

/*
 * Fighter and thief skills, as well as magic item skills.
 */
    {
	"backstab",		{},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_backstab,		 0,	24,
	"vicious backstab",	"!Backstab!"
    },

    {
	"bash door", 	 	{},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_bash,		 0,	24,
	"powerful bash",	"!Bash Door!"
    },

    {
        "berserk",              {},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_berserk,            0,      12,
        "",                     "The bloody haze lifts."
    },

    {
        "circle",		{},
        spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_circle,		 0,	24,
        "sneak attack",		"!Circle!"
    },

    {
	"disarm",		{},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_disarm,		 0,	24,
	"",			"!Disarm!"
    },

    {
	"dodge",		{},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dodge,		 0,	 0,
	"",			"!Dodge!"
    },

    {
        "dual",         	{},
        spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_dual,      	 0,	 0,
        "",                     "!Dual!"
    },

    {
	"enhanced damage",	{},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_enhanced_damage,	0,	 0,
	"",			"!Enhanced Damage!"
    },

    {
	"hide",			{},
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		 0,	12,
	"",			"!Hide!"
    },

    {
	"kick",			{},
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_kick,		 0,	 8,
	"mighty kick",		"!Kick!"
    },

    {
	"parry",		{},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_parry,		 0,	 0,
	"",			"!Parry!"
    },

    {
	"peek",			{},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		 0,	 0,
	"",			"!Peek!"
    },

    {
	"pick lock",		{},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		 0,	12,
	"",			"!Pick!"
    },

    {
	"poison weapon",	{},
	spell_null,		TAR_OBJ_INV,		POS_STANDING,
	&gsn_poison_weapon,	 0,	12,
	"poisonous concoction",	"!Poison Weapon!"
    },

    {
	"rescue",		{},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_rescue,		 0,	12,
	"",			"!Rescue!"
    },

    {
	"scrolls",      	{},
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_scrolls,	 	 0,	0,
	"blazing scroll",	"!Scrolls!"
    },

    {
	"second attack",	{},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_second_attack,	 0,	 0,
	"",			"!Second Attack!"
    },

    {
	"snare",		{},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_snare,		 0,	12,
	"",			"You are no longer ensnared."
    },

    {
	"sneak",		{},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		 0,	12,
	"",			NULL
    },

    {
	"staves",       	{},
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_staves,	 	 0,	0,
	"shattered staff",	"!Staves!"
    },

    {
	"steal",		{},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		 0,	24,
	"",			"!Steal!"
    },

    {
	"third attack",		{},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_third_attack,	 0,	 0,
	"",			"!Third Attack!"
    },

    {
        "untangle",		{},
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_untangle,		 0,	24,
        "",                     "!Untangle!"
    },

    {
	"wands",        	{},
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_wands,	 	 0,	0,
	"exploding wand",	"!Wands!"
    },

/*
 *  Spells for mega1.are from Glop/Erkenbrand.
 */
    {
        "general purpose",      {},
	spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   0,      12,
	"general purpose ammo", "!General Purpose Ammo!"
    },

    {
        "high explosive",       {},
	spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   0,      12,
	"high explosive ammo",  "!High Explosive Ammo!"
    },


    /*
     * Psionicist spell/skill definitions
     * Psi class conceived, created, and coded by Thelonius (EnvyMud)
     */

    {
        "adrenaline control",   {},
        spell_adrenaline_control,       TAR_CHAR_SELF,  POS_STANDING,
        NULL,                           6,      12,
        "",                             "The adrenaline rush wears off."
    },

    {
        "agitation",            {},
        spell_agitation,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   10,     12,
        "molecular agitation",  "!Agitation!"
    },

    {
        "aura sight",           {},
        spell_aura_sight,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   9,     12,
        "",                     "!Aura Sight!"
    },

    {
        "awe",                  {},
        spell_awe,              TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
        NULL,                   35,     12,
        "",                     "!Awe!"
    },

    {
        "ballistic attack",     {},
        spell_ballistic_attack, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   5,     12,
        "ballistic attack",     "!Ballistic Attack!"
    },

    {
        "biofeedback",          {},
        spell_biofeedback,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   75,     12,
        "",                     "Your biofeedback is no longer effective."
    },

    {
        "cell adjustment",      {},
        spell_cell_adjustment,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   8,     12,
        "",                     "!Cell Adjustment!"
    },

    {
        "chameleon power",      {},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_chameleon,         0,     12,
        "",                     "!Chameleon Power!"
    },

    {
        "combat mind",          {},
        spell_combat_mind,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   15,     12,
        "",                     "Your battle sense has faded."
    },

    {
        "complete healing",     {},
        spell_complete_healing, TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   100,    12,
        "",                     "!Complete Healing!"
    },

    {
        "control flames",       {},
        spell_control_flames,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   15,     12,
        "tongue of flame",      "!Control Flames!"
    },

    {
        "create sound",         {},
        spell_create_sound,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   5,     12,
        "",                     "!Create Sound!"
    },

    {
        "death field",          {},
        spell_death_field,      TAR_IGNORE,             POS_FIGHTING,
        NULL,                   200,    18,
        "field of death",       "!Death Field!"
    },

    {
        "detonate",             {},
        spell_detonate,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   35,     24,
        "detonation",           "!Detonate!"
    },

    {
        "disintegrate",         {},
        spell_disintegrate,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   150,    18,
        "disintegration",       "!Disintegrate!"
    },

    {
        "displacement",         {},
        spell_displacement,     TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   10,     12,
        "",                     "You are no longer displaced."
    },

    {
        "domination",           {},
        spell_domination,       TAR_CHAR_OFFENSIVE,     POS_STANDING,
        &gsn_domination,        5,     12,
        "",                     "You regain control of your body."
    },

    {
        "ectoplasmic form",     {},
        spell_ectoplasmic_form, TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   20,     12,
        "",                     "You feel solid again."
    },

    {
        "ego whip",             {},
        spell_ego_whip,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   20,     12,
        "",                     "You feel more confident."
    },

    {
        "energy containment",   {},
        spell_energy_containment,       TAR_CHAR_SELF,  POS_STANDING,
        NULL,                           10,     12,
        "",                             "You no longer absorb energy."
    },

    {
        "enhance armor",        {},
        spell_enhance_armor,    TAR_OBJ_INV,    POS_STANDING,
        NULL,                   100,    24,
        "",                     "!Enhance Armor!"
    },

    {
        "enhanced strength",    {},
        spell_enhanced_strength,        TAR_CHAR_SELF,  POS_STANDING,
        NULL,                           20,     12,
        "",                             "You no longer feel so HUGE."
    },

    {
        "flesh armor",          {},
        spell_flesh_armor,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   15,     12,
        "",                     "Your skin returns to normal."
    },

    {
        "heighten senses",      {},
        spell_null,             TAR_CHAR_SELF,          POS_STANDING,
        &gsn_heighten,          0,      0,
        "",                     "Your senses return to normal."
    },

    {
        "inertial barrier",     {},
        spell_inertial_barrier, TAR_IGNORE,             POS_STANDING,
        NULL,                   40,     24,
        "",                     "Your inertial barrier dissipates."
    },

    {
        "inflict pain",         {},
        spell_inflict_pain,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   10,     12,
        "mindpower",            "!Inflict Pain!"
    },

    {
        "intellect fortress",   {},
        spell_intellect_fortress,       TAR_IGNORE,     POS_STANDING,
        NULL,                           25,     24,
        "",                     "Your intellectual fortress crumbles."
    },

    {
        "lend health",          {},
        spell_lend_health,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   10,     12,
        "",                     "!Lend Health!"
    },

    {
        "levitation",           {},
        spell_levitation,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   10,     18,
        "",                     "You slowly float to the ground."
    },

    {
        "mental barrier",       {},
        spell_mental_barrier,   TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   8,     12,
        "",                     "Your mental barrier breaks down."
    },

    {
        "mind thrust",          {},
        spell_mind_thrust,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   8,     12,
        "mind thrust",          "!Mind Thrust!"
    },

    {
        "project force",        {},
        spell_project_force,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   18,     12,
        "projected force",      "!Project Force!"
    },

    {
        "psionic blast",        {},
        spell_psionic_blast,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   25,     12,
        "psionic blast",        "!Psionic Blast!"
    },

    {
        "psychic crush",        {},
        spell_psychic_crush,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   15,     18,
        "psychic crush",        "!Psychic Crush!"
    },

    {
        "psychic drain",        {},
        spell_psychic_drain,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   20,     12,
        "",                     "You no longer feel drained."
    },

    {
        "psychic healing",      {},
        spell_psychic_healing,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   20,      12,
        "",                     "!Psychic Healing!"
    },

    {
        "shadow form",          {},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_shadow,            0,     12,
        "",                     "You no longer move in the shadows."
    },

    {
        "share strength",       {},
        spell_share_strength,   TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   8,     12,
        "",                     "You no longer share strength with another."
    },

    {
        "thought shield",       {},
        spell_thought_shield,   TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   5,     12,
        "",                     "You no longer feel so protected."
    },

    {
        "ultrablast",           {},
        spell_ultrablast,       TAR_IGNORE,             POS_FIGHTING,
        NULL,                   75,     24,
        "ultrablast",           "!Ultrablast!"
    },

    {
        "stake",                {},
        spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_stake,             0,       8,
        "carefully aimed stake","!Stake!"
    },

	/* New abilities added by Zen made by other ppl */
    {
        "scan",			{},
        spell_null,		TAR_IGNORE,     POS_STANDING,
        &gsn_scan,		0,       24,
        "",			"!Scan!"
    },

    {
        "shield block",         {},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_shield_block,      0,      0,
        "",                     "!Shield Block!"
    },

    {
        "fast healing",         {},
        spell_null,             TAR_IGNORE,             POS_DEAD,
        &gsn_fast_healing,      0,      0,
        "",                     "!Fast Healing!"
    },

   {
        "fourth attack",        {},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_fourth_attack,     0,      0,
        "",                     "!Fourth Attack!"
    },

    {
	"brew",			{},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_brew,		0,	 24,
	"blunder",		"!Brew!"
    },

    {
	"scribe",		{},
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_scribe,		0,	 24,
	"blunder",		"!Scribe!"
    },

	/*
	 * spells & skills here by Zen :)
	 */
    {
        "dirt kicking",		{},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dirt,              0,      24,
        "kicked dirt",          "You rub the dirt out of your eyes."
    },

    {
        "swim",                 {},
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_swim,		 0,	 0,
	"",			"!Swim!"
    },

    {
        "meditate",         	{},
        spell_null,             TAR_IGNORE,             POS_DEAD,
        &gsn_meditate,      	0,      0,
        "",			"!Meditate!"
    },

    {
        "meteor swarm",		{},
        spell_meteor_swarm,	TAR_IGNORE,     POS_FIGHTING,
        NULL,			20,       12,
        "meteor swarm",		"!Meteor Swarm!"
    },

    {   
	"chain lightning",	{},
	spell_chain_lightning,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	12,
	"lightning",		"!Chain Lightning!"
    }, 

    {
	"scry",			{},
	spell_scry, 		TAR_IGNORE,		POS_STANDING,
	NULL,			15,	12,
	"",			"!Scry!"
    },
    
    {
	"vortex lift",		{},
	spell_vortex_lift,	TAR_IGNORE,		POS_STANDING,
	NULL,			50,	12,
	"",			"!Vortex Lift!"
    },
    
    {
	"mass vortex lift",	{},
	spell_mass_vortex_lift,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_vortex_lift,	150,	12,
	"",			"!Mass Vortex Lift!"
    },

    {
	"home sick",		{},
	spell_home_sick,	TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,			0,	2,
	"",			"!Home Sick!"
    },

    {
	"detect good",		{},
	spell_detect_good,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"The gold in your vision disappears."
    },

    {
	"dispel good",		{},
	spell_dispel_good,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"unholy fire",		"!Dispel Good!"
    },

    {
        "portal",               {},
        spell_portal,           TAR_IGNORE,             POS_STANDING,
        NULL,                   100,     24,
        "",                     "!Portal!",
    },

    {
        "nexus",                {},
        spell_nexus,            TAR_IGNORE,             POS_STANDING,
        NULL,                   150,   36,
        "",                     "!Nexus!"
    },

    {
	"create buffet",	{},
	spell_create_buffet,	TAR_IGNORE,		POS_STANDING,
	NULL,			33,	12,
	"",			"!Create Buffet!"
    },

	  /* Race ability spells */
    {
        "vampiric bite",        {},
        spell_vampiric_bite,    TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
        &gsn_vampiric_bite,     0,     0,
        "vampiric bite",        "You feel well fed."
    }

};
