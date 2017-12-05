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


/*
 * Class table.
 */
const	struct	class_type	class_table	[MAX_CLASS]	=
{
    {
	"Mag", APPLY_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	3018,  95,  18,  6,   6,  8, TRUE
    },

    {
	"Cle", APPLY_WIS,  OBJ_VNUM_SCHOOL_MACE,
	3003,  95,  18,  9,  7,  10, TRUE
    },

    {
	"Thi", APPLY_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	3028,  85,  18,  3,  8,  13, FALSE
    },

    {
	"War", APPLY_STR,  OBJ_VNUM_SCHOOL_SWORD,
	3022,  85,  18,  0,  11, 15, FALSE
    },

    {
        "Psi", APPLY_WIS,  OBJ_VNUM_SCHOOL_DAGGER,
        3151,  95,  18,  9,   6,  9, TRUE
    }

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
 * Titles.
 */
char *	const			title_table [ MAX_CLASS ][ MAX_LEVEL+1 ][ 2 ] =
{
    {
	{ "Man",			"Woman"				},

	{ "Apprentice of Magic",	"Apprentice of Magic"		},
	{ "Spell Student",		"Spell Student"			},
	{ "Scholar of Magic",		"Scholar of Magic"		},
	{ "Delver in Spells",		"Delveress in Spells"		},
	{ "Medium of Magic",		"Medium of Magic"		},

	{ "Scribe of Magic",		"Scribess of Magic"		},
	{ "Seer",			"Seeress"			},
	{ "Sage",			"Sage"				},
	{ "Illusionist",		"Illusionist"			},
	{ "Abjurer",			"Abjuress"			},

	{ "Invoker",			"Invoker"			},
	{ "Enchanter",			"Enchantress"			},
	{ "Conjurer",			"Conjuress"			},
	{ "Magician",			"Witch"				},
	{ "Creator",			"Creator"			},

	{ "Savant",			"Savant"			},
	{ "Magus",			"Craftess"			},
	{ "Wizard",			"Wizard"			},
	{ "Warlock",			"War Witch"			},
	{ "Sorcerer",			"Sorceress"			},

	{ "Elder Sorcerer",		"Elder Sorceress"		},
	{ "Grand Sorcerer",		"Grand Sorceress"		},
	{ "Great Sorcerer",		"Great Sorceress"		},
	{ "Golem Maker",		"Golem Maker"			},
	{ "Greater Golem Maker",	"Greater Golem Maker"		},

	{ "Maker of Stones",		"Maker of Stones",		},
	{ "Maker of Potions",		"Maker of Potions",		},
	{ "Maker of Scrolls",		"Maker of Scrolls",		},
	{ "Maker of Wands",		"Maker of Wands",		},
	{ "Maker of Staves",		"Maker of Staves",		},

	{ "Demon Summoner",		"Demon Summoner"		},
	{ "Greater Demon Summoner",	"Greater Demon Summoner"	},
	{ "Dragon Charmer",		"Dragon Charmer"		},
	{ "Greater Dragon Charmer",	"Greater Dragon Charmer"	},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},

	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Master of all Magic",	"Master of all Magic"		},
	{ "Mage Hero",			"Mage Heroine"			},

	{ "Angel of Magic",		"Angel of Magic"		},
	{ "Deity of Magic",		"Deity of Magic"		},
	{ "Supremity of Magic",		"Supremity of Magic"		},
	{ "Implementor",		"Implementress"			}
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
	{ "Holy Hero",			"Holy Heroine"			},

	{ "Angel",			"Angel"				},
	{ "Deity",			"Deity"				},
	{ "Supreme Master",		"Supreme Mistress"		},
	{ "Implementor",		"Implementress"			}
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
	{ "Assassin Hero",		"Assassin Heroine"		},

	{ "Angel of Death",		"Angel of Death"		},
	{ "Deity of Assassins",		"Deity of Assassins"		},
	{ "Supreme Master",		"Supreme Mistress"		},
	{ "Implementor",		"Implementress"			}
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

	{ "Baron of the Eastern Plains","Baroness of Meteors"		},
	{ "Baron of the Western Mountains","Baroness of Meteors"	},
	{ "Baron of the Northern Reaches","Baroness of Meteors"		},
	{ "Baron of the Southern Seas",	"Baroness of Meteors"		},
	{ "Knight of the Black Rose",	"Baroness of Meteors"		},

	{ "Knight of the Red Rose",	"Baroness of Meteors"		},
	{ "Knight of the White Rose",	"Baroness of Meteors"		},
	{ "Knight of the Silver Moon",	"Baroness of Meteors"		},
	{ "Knight of the Golden Sun",	"Baroness of Meteors"		},
	{ "Squire of the Realm",	"Baroness of Meteors"		},

	{ "Herald of the Realm",	"Baroness of Meteors"		},
	{ "Knight of the Realm",	"Baroness of Meteors"		},
	{ "Lord of the Realm",		"Baroness of Meteors"		},
	{ "Highlord of the Realm",	"Baroness of Meteors"		},
	{ "Knight Hero",		"Knight Heroine"		},

	{ "Angel of War",		"Angel of War"			},
	{ "Deity of War",		"Deity of War"			},
	{ "Supreme Master of War",	"Supreme Mistress of War"	},
	{ "Implementor",		"Implementress"			}
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
        { "Psionicist Hero",            "Psionicist Herione",           },

        { "Psionicist Angel",           "Psionicist Angel"              },
        { "Deity of Psionics",          "Deity of Psionics"             },
        { "Supreme Psionicst",          "Supreme Psionicist"            },
        { "Implementor",                "Implementress"                 }
    }

};

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



const	struct	skill_type	skill_table	[MAX_SKILL]	=
{

/*
 * Magic spells.
 */

    {
	"reserved",		{    99,    99,    99,    99,    99 },
	0,			TAR_IGNORE,		POS_STANDING,
	NULL,			0,	 0,
	"",			""
    },

    {
	"acid blast",		{    20, L_APP, L_APP, L_APP, L_APP },
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"acid blast",		"!Acid Blast!"
    },

    {
	"armor",		{     5,     1, L_APP, L_APP, L_APP },
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected."
    },

    {
	"bless",		{ L_APP,     5, L_APP, L_APP, L_APP },
	spell_bless,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less righteous."
    },

    {
	"blindness",		{     8,     5, L_APP, L_APP, L_APP },
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,		5,	12,
	"",			"You can see again."
    },

    {
	"breathe water",	{ L_APP, L_APP, L_APP, L_APP, L_APP },
	spell_breathe_water,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_breathe_water, 	 5,	12,
	"pained lungs",		"You can no longer breathe underwater."
    },

    {
	"burning hands",	{     5, L_APP, L_APP, L_APP, L_APP },
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_burning_hands,     15,	12,
	"burning hands",	"!Burning Hands!"
    },

    {
	"call lightning",	{ L_APP,    12, L_APP, L_APP, L_APP },
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			15,	12,
	"lightning bolt",	"!Call Lightning!"
    },

    {
	"cause critical",	{ L_APP,     9, L_APP, L_APP, L_APP },
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"invocation",		"!Cause Critical!"
    },

    {
	"cause light",		{ L_APP,     1, L_APP, L_APP, L_APP },
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"invocation",		"!Cause Light!"
    },

    {
	"cause serious",	{ L_APP,     5, L_APP, L_APP, L_APP },
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			17,	12,
	"invocation",		"!Cause Serious!"
    },

    {
	"change sex",		{ L_APP, L_APP, L_APP, L_APP, L_APP },
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	0,
	"",			"Your body feels familiar again."
    },

    {
	"charm person",		{    14, L_APP, L_APP, L_APP, L_APP },
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	5,	12,
	"",			"You feel more self-confident."
    },

    {
	"chill touch",		{     3, L_APP, L_APP, L_APP, L_APP },
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"chilling touch",	"You feel less cold."
    },

    {
	"colour spray",		{    11, L_APP, L_APP, L_APP, L_APP },
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"colour spray",		"!Colour Spray!"
    },

    {
        "cone of silence",	{    22, L_APP, L_APP, L_APP, L_APP },
        spell_cone_of_silence,	TAR_IGNORE,		POS_FIGHTING,
        NULL,			35,     12,
        "",                     "!Cone of Silence!"
    },

    {
	"continual light",	{     4,     2, L_APP, L_APP, L_APP },
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,			7,	12,
	"",			"!Continual Light!"
    },

    {
	"control weather",	{     10,   13, L_APP, L_APP, L_APP },
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,			25,	12,
	"",			"!Control Weather!"
    },

    {
	"create food",		{ L_APP,     3, L_APP, L_APP, L_APP },
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Create Food!"
    },

    {
	"create spring",	{    10, L_APP, L_APP, L_APP, L_APP },
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	12,
	"",			"!Create Spring!"
    },

    {
	"create water",		{ L_APP,     2, L_APP, L_APP, L_APP },
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Create Water!"
    },

    {
	"cure blindness",	{ L_APP,     4, L_APP, L_APP, L_APP },
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			5,	12,
	"",			"!Cure Blindness!"
    },

    {
	"cure critical",	{ L_APP,     9, L_APP, L_APP, L_APP },
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"",			"!Cure Critical!"
    },

    {
	"cure light",		{ L_APP,     1, L_APP, L_APP, L_APP },
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			10,	12,
	"",			"!Cure Light!"
    },

    {
	"cure poison",		{ L_APP,     9, L_APP, L_APP, L_APP },
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"!Cure Poison!"
    },

    {
	"cure serious",		{ L_APP,     5, L_APP, L_APP, L_APP },
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"",			"!Cure Serious!"
    },

    {
	"curse",		{    12,    12, L_APP, L_APP, L_APP },
	spell_curse,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_curse,		20,	12,
	"curse",		"The curse wears off."
    },

    {
	"destroy cursed",	{ L_APP,    20, L_APP, L_APP, L_APP },
	spell_destroy_cursed,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	12,
	"",			"!destroy cursed!"
    },

    {
	"detect evil",		{ L_APP,     4, L_APP, L_APP, L_APP },
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"The red in your vision disappears."
    },

    {
	"detect hidden",	{ L_APP,     7, L_APP, L_APP, L_APP },
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less aware of your surroundings."
    },

    {
	"detect invis",		{     2,     5, L_APP, L_APP, L_APP },
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You no longer see invisible objects."
    },

    {
	"detect magic",		{     2,     3, L_APP, L_APP, L_APP },
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING, 
	NULL,			5,	12,
	"",			"The detect magic wears off."
    },

    {
	"detect poison",	{ L_APP,     5, L_APP, L_APP, L_APP },
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Detect Poison!"
    },

    {
	"dispel evil",		{ L_APP,    10, L_APP, L_APP, L_APP },
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"holy fire",		"!Dispel Evil!"
    },

    {
	"dispel magic",		{    26,    31, L_APP, L_APP, L_APP },
	spell_dispel_magic,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	16,
	"",			"!Dispel Magic!"
    },

    {
	"earthquake",		{ L_APP,     7, L_APP, L_APP, L_APP },
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			15,	12,
	"earthquake",		"!Earthquake!"
    },

    {
	"enchant weapon",	{    12, L_APP, L_APP, L_APP, L_APP },
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			100,	24,
	"",			"!Enchant Weapon!"
    },

    {
	"energy drain",		{    13, L_APP, L_APP, L_APP,    14 },
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			35,	12,
	"energy drain",		"!Energy Drain!"
    },

    {
	"exorcise",		{ L_APP,    40, L_APP, L_APP, L_APP },
	spell_exorcise, 	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			35,	12,
	"",		        "!Exorcise!"
    },

    {
	"faerie fire",		{     4,     2, L_APP, L_APP, L_APP },
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			5,	12,
	"faerie fire",		"The pink aura around you fades away."
    },

    {
	"faerie fog",		{    10,    14, L_APP, L_APP, L_APP },
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,			12,	12,
	"faerie fog",		"!Faerie Fog!"
    },

    {
	"fireball",		{    15, L_APP, L_APP, L_APP, L_APP },
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"fireball",		"!Fireball!"
    },

    {
	"flamestrike",		{ L_APP,    13, L_APP, L_APP, L_APP },
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"flamestrike",		"!Flamestrike!"
    },

    {
	"flaming shield",	{ L_APP,    35, L_APP, L_APP, L_APP },
	spell_flaming,  	TAR_CHAR_SELF,  	POS_STANDING,
	NULL,			100,	60,
	"flaming shield",	"The flaming shield around you dies out."
    },

    {
	"fly",			{     7,    12, L_APP, L_APP, L_APP },
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			10,	18,
	"",			"You slowly float to the ground."
    },

    {
	"gate",			{ L_APP, L_APP, L_APP, L_APP, L_APP },
	spell_gate,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"",			"!Gate!"
    },

    {
	"giant strength",	{     7, L_APP, L_APP, L_APP, L_APP },
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	12,
	"",			"You feel weaker."
    },

    {
	"harm",			{ L_APP,    15, L_APP, L_APP, L_APP },
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			35,	12,
	"divine power",		"!Harm!"
    },

    {
	"heal",			{ L_APP,    14, L_APP, L_APP, L_APP },
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"",			"!Heal!"
    },

    {
	"identify",		{    10,    10, L_APP, L_APP,    13 },
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			12,	24,
	"",			"!Identify!"
    },

    {
	"infravision",		{     6,     9, L_APP, L_APP, L_APP },
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	18,
	"",			"You no longer see in the dark."
    },

    {
	"invis",		{  4, L_APP, L_APP, L_APP,       14 },
	spell_invis,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_invis,		5,	12,
	"",			"You are no longer invisible."
    },

    {
	"know alignment",	{     8,     5, L_APP, L_APP, L_APP },
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			9,	12,
	"",			"!Know Alignment!"
    },

    {
	"lightning bolt",	{     9, L_APP, L_APP, L_APP, L_APP },
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"lightning bolt",	"!Lightning Bolt!"
    },

    {
	"locate object",	{     6,    10, L_APP, L_APP, L_APP },
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	18,
	"",			"!Locate Object!"
    },

    {
	"magic missile",	{     1, L_APP, L_APP, L_APP, L_APP },
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"magic missile",	"!Magic Missile!"
    },

    {
	"mass heal",		{ L_APP,    20, L_APP, L_APP, L_APP },
	spell_mass_heal,	TAR_IGNORE,		POS_STANDING,
	NULL,           	50,	24,
	"",			"!Mass Heal!"
    },

    {
	"mass invis",		{    15,    17, L_APP, L_APP, L_APP },
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	20,	24,
	"",			"You are no longer invisible."
    },

    {
        "mute",			{ L_APP,    18, L_APP, L_APP, L_APP },
        spell_mute,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
        &gsn_mute,		20,     12,
        "",                     "You are no longer muted."
    },

    {
	"pass door",		{    18, L_APP, L_APP, L_APP, L_APP },
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			20,	12,
	"",			"You feel solid again."
    },

    {
	"poison",		{ L_APP,     8, L_APP, L_APP, L_APP },
	spell_poison,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_poison,		10,	12,
	"burning blood",	"You feel less sick."
    },

    {
	"polymorph other",	{ L_APP, L_APP, L_APP, L_APP, L_APP },
	spell_polymorph_other,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			20,	0,
	"",			"Your body feels familiar again." 
    },

    {
	"protection evil",	{ L_APP,     6, L_APP, L_APP, L_APP },
	spell_protection_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected."
    },

    {
	"protection good",	{ L_APP,     6, L_APP, L_APP, L_APP },
	spell_protection_good,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected."
    },

    {                                  
        "recharge item",        {    11, L_APP, L_APP, L_APP, L_APP },
        spell_recharge_item,    TAR_OBJ_INV,            POS_STANDING,
        NULL,                   25,     12,
        "blunder",              "!Recharge Item!"
    }, 

    {
	"refresh",		{     5,     3, L_APP, L_APP, L_APP },
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			12,	18,
	"refresh",		"!Refresh!"
    },

    {
        "remove alignment",	{    16, L_APP, L_APP, L_APP, L_APP },
        spell_remove_alignment,	TAR_OBJ_INV,		POS_STANDING,
        NULL,			100,	12,
        "",                     "!Remove Alignment!"
    },

    {
	"remove curse",		{ L_APP,    12, L_APP, L_APP, L_APP },
	spell_remove_curse,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"!Remove Curse!"
    },

    {
        "remove silence",	{ L_APP,    16, L_APP, L_APP, L_APP },
        spell_remove_silence,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
        NULL,			15,     12,
        "",                     "!Remove Silence!"
    },

    {
	"sanctuary",		{ L_APP,    13, L_APP, L_APP, L_APP },
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			75,	12,
	"",			"The white aura around your body fades."
    },

    {
	"shield",		{    13, L_APP, L_APP, L_APP, L_APP },
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			12,	18,
	"",			"Your force shield shimmers then fades away."
    },

    {
	"shocking grasp",	{     7, L_APP, L_APP, L_APP, L_APP },
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"shocking grasp",	"!Shocking Grasp!"
    },

    {
	"sleep",		{    14, L_APP, L_APP, L_APP, L_APP },
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,		15,	12,
	"",			"You feel less tired."
    },

    {
	"stone skin",		{    17, L_APP, L_APP, L_APP, L_APP },
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			12,	18,
	"",			"Your skin feels soft again."
    },

    {
	"summon",		{ L_APP,     8, L_APP, L_APP,    12 },
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,			50,	12,
	"",			"!Summon!"
    },

    {
	"teleport",		{     8, L_APP, L_APP, L_APP,     7 },
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 		35,	12,
	"",			"!Teleport!"
    },

    {
	"turn undead",		{ L_APP,    1, L_APP, L_APP, L_APP },
	spell_turn_undead,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_turn_undead,	10,	12,
	"divine exorcism",	"!Turn undead!"
    },

    {
	"ventriloquate",	{     1, L_APP, L_APP, L_APP, L_APP },
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Ventriloquate!"
    },

    {
	"weaken",		{     7, L_APP, L_APP, L_APP, L_APP },
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"weakening spell",	"You feel stronger."
    },

    {
	"word of recall",	{ L_APP, L_APP, L_APP, L_APP, L_APP },
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	NULL,			5,	12,
	"",			"!Word of Recall!"
    },

/*
 * Dragon breath
 */
    {
	"acid breath",		{    33, L_APP, L_APP, L_APP, L_APP },
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of acid",	"!Acid Breath!"
    },

    {
	"fire breath",		{    34, L_APP, L_APP, L_APP, L_APP },
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of flame",	"!Fire Breath!"
    },

    {
	"frost breath",		{    31, L_APP, L_APP, L_APP, L_APP },
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of frost",	"!Frost Breath!"
    },

    {
	"gas breath",		{    35, L_APP, L_APP, L_APP, L_APP },
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			50,	12,
	"breath of gas",		"!Gas Breath!"
    },

    {
	"lightning breath",	{    32, L_APP, L_APP, L_APP, L_APP },
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of lightning",	"!Lightning Breath!"
    },

/*
 * Fighter and thief skills, as well as magic item skills.
 */
    {
	"backstab",		{ L_APP, L_APP,     1, L_APP, L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_backstab,		 0,	24,
	"vicious backstab",	"!Backstab!"
    },

    {
	"bash door", 	 	{ L_APP, L_APP, L_APP,     8, L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_bash,		 0,	24,
	"powerful bash",	"!Bash Door!"
    },

    {
        "berserk",              { L_APP, L_APP, L_APP,    12, L_APP },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_berserk,            0,      12,
        "",                     "The bloody haze lifts."
    },

    {
        "circle",		{ L_APP, L_APP,    18, L_APP, L_APP },
        spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_circle,		 0,	24,
        "sneak attack",		"!Circle!"
    },

    {
	"disarm",		{ L_APP, L_APP,    10, L_APP, L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_disarm,		 0,	24,
	"",			"!Disarm!"
    },

    {
	"dodge",		{ L_APP, L_APP,     1, L_APP, L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dodge,		 0,	 0,
	"",			"!Dodge!"
    },

    {
        "dual",         	{ L_APP, L_APP, L_APP,    15, L_APP },
        spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_dual,      	 0,	 0,
        "",                     "!Dual!"
    },

    {
	"enhanced damage",	{ L_APP, L_APP, L_APP,     1, L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_enhanced_damage,	0,	 0,
	"",			"!Enhanced Damage!"
    },

    {
	"hide",			{ L_APP, L_APP,     1, L_APP, L_APP },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		 0,	12,
	"",			"!Hide!"
    },

    {
	"kick",			{ L_APP, L_APP, L_APP,     1, L_APP },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_kick,		 0,	 8,
	"mighty kick",		"!Kick!"
    },

    {
	"parry",		{ L_APP, L_APP, L_APP,     1, L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_parry,		 0,	 0,
	"",			"!Parry!"
    },

    {
	"peek",			{ L_APP, L_APP,     1, L_APP, L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		 0,	 0,
	"",			"!Peek!"
    },

    {
	"pick lock",		{ L_APP, L_APP,     1, L_APP, L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		 0,	12,
	"",			"!Pick!"
    },

    {
	"poison weapon",	{ L_APP, L_APP,    13, L_APP, L_APP },
	spell_null,		TAR_OBJ_INV,		POS_STANDING,
	&gsn_poison_weapon,	 0,	12,
	"poisonous concoction",	"!Poison Weapon!"
    },

    {
	"rescue",		{ L_APP, L_APP, L_APP,     1, L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_rescue,		 0,	12,
	"",			"!Rescue!"
    },

    {
	"scrolls",      	{     3,     5,     7,    10,     5 },
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_scrolls,	 	 0,	0,
	"blazing scroll",	"!Scrolls!"
    },

    {
	"second attack",	{ L_APP, L_APP,     1,     1, L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_second_attack,	 0,	 0,
	"",			"!Second Attack!"
    },

    {
	"snare",		{ L_APP, L_APP,     8, L_APP, L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_snare,		 0,	12,
	"",			"You are no longer ensnared."
    },

    {
	"sneak",		{ L_APP, L_APP,     1, L_APP, L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		 0,	12,
	"",			NULL
    },

    {
	"staves",       	{     3,     7,    13,    20,     9 },
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_staves,	 	 0,	0,
	"shattered staff",	"!Staves!"
    },

    {
	"steal",		{ L_APP, L_APP,     1, L_APP, L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		 0,	24,
	"",			"!Steal!"
    },

    {
	"third attack",		{ L_APP, L_APP,    20,     1, L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_third_attack,	 0,	 0,
	"",			"!Third Attack!"
    },

    {
        "untangle",		{ L_APP, L_APP,    11, L_APP, L_APP },
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_untangle,		 0,	24,
        "",                     "!Untangle!"
    },

    {
	"wands",        	{     3,     5,     7,    10,     5 },
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_wands,	 	 0,	0,
	"exploding wand",	"!Wands!"
    },

/*
 *  Spells for mega1.are from Glop/Erkenbrand.
 */
    {
        "general purpose",      { L_APP, L_APP, L_APP, L_APP, L_APP },
	spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   0,      12,
	"general purpose ammo", "!General Purpose Ammo!"
    },

    {
        "high explosive",       { L_APP, L_APP, L_APP, L_APP, L_APP },
	spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   0,      12,
	"high explosive ammo",  "!High Explosive Ammo!"
    },


    /*
     * Psionicist spell/skill definitions
     * Psi class conceived, created, and coded by Thelonius (EnvyMud)
     */

    {
        "adrenaline control",   { L_APP, L_APP, L_APP, L_APP,    10 },
        spell_adrenaline_control,       TAR_CHAR_SELF,  POS_STANDING,
        NULL,                           6,      12,
        "",                             "The adrenaline rush wears off."
    },

    {
        "agitation",            { L_APP, L_APP, L_APP, L_APP,     6 },
        spell_agitation,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   10,     12,
        "molecular agitation",  "!Agitation!"
    },

    {
        "aura sight",           { L_APP, L_APP, L_APP, L_APP,    14 },
        spell_aura_sight,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   9,     12,
        "",                     "!Aura Sight!"
    },

    {
        "awe",                  { L_APP, L_APP, L_APP, L_APP,    21 },
        spell_awe,              TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
        NULL,                   35,     12,
        "",                     "!Awe!"
    },

    {
        "ballistic attack",     { L_APP, L_APP, L_APP, L_APP,     1 },
        spell_ballistic_attack, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   5,     12,
        "ballistic attack",     "!Ballistic Attack!"
    },

    {
        "biofeedback",          { L_APP, L_APP, L_APP, L_APP,    18 },
        spell_biofeedback,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   75,     12,
        "",                     "Your biofeedback is no longer effective."
    },

    {
        "cell adjustment",      { L_APP, L_APP, L_APP, L_APP,    11 },
        spell_cell_adjustment,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   8,     12,
        "",                     "!Cell Adjustment!"
    },

    {
        "chameleon power",      { L_APP, L_APP, L_APP, L_APP,     6 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_chameleon,         0,     12,
        "",                     "!Chameleon Power!"
    },

    {
        "combat mind",          { L_APP, L_APP, L_APP, L_APP,    15 },
        spell_combat_mind,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   15,     12,
        "",                     "Your battle sense has faded."
    },

    {
        "complete healing",     { L_APP, L_APP, L_APP, L_APP,    28 },
        spell_complete_healing, TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   100,    12,
        "",                     "!Complete Healing!"
    },

    {
        "control flames",       { L_APP, L_APP, L_APP, L_APP,     8 },
        spell_control_flames,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   15,     12,
        "tongue of flame",      "!Control Flames!"
    },

    {
        "create sound",         { L_APP, L_APP, L_APP, L_APP,     2 },
        spell_create_sound,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   5,     12,
        "",                     "!Create Sound!"
    },

    {
        "death field",          { L_APP, L_APP, L_APP, L_APP,    30 },
        spell_death_field,      TAR_IGNORE,             POS_FIGHTING,
        NULL,                   200,    18,
        "field of death",       "!Death Field!"
    },

    {
        "detonate",             { L_APP, L_APP, L_APP, L_APP,    20 },
        spell_detonate,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   35,     24,
        "detonation",           "!Detonate!"
    },

    {
        "disintegrate",         { L_APP, L_APP, L_APP, L_APP,    27 },
        spell_disintegrate,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   150,    18,
        "disintegration",       "!Disintegrate!"
    },

    {
        "displacement",         { L_APP, L_APP, L_APP, L_APP,     9 },
        spell_displacement,     TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   10,     12,
        "",                     "You are no longer displaced."
    },

    {
        "domination",           { L_APP, L_APP, L_APP, L_APP,    16 },
        spell_domination,       TAR_CHAR_OFFENSIVE,     POS_STANDING,
        &gsn_domination,        5,     12,
        "",                     "You regain control of your body."
    },

    {
        "ectoplasmic form",     { L_APP, L_APP, L_APP, L_APP,    19 },
        spell_ectoplasmic_form, TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   20,     12,
        "",                     "You feel solid again."
    },

    {
        "ego whip",             { L_APP, L_APP, L_APP, L_APP,    13 },
        spell_ego_whip,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   20,     12,
        "",                     "You feel more confident."
    },

    {
        "energy containment",   { L_APP, L_APP, L_APP, L_APP,    10 },
        spell_energy_containment,       TAR_CHAR_SELF,  POS_STANDING,
        NULL,                           10,     12,
        "",                             "You no longer absorb energy."
    },

    {
        "enhance armor",        { L_APP, L_APP, L_APP, L_APP,    15 },
        spell_enhance_armor,    TAR_OBJ_INV,    POS_STANDING,
        NULL,                   100,    24,
        "",                     "!Enhance Armor!"
    },

    {
        "enhanced strength",    { L_APP, L_APP, L_APP, L_APP,     7 },
        spell_enhanced_strength,        TAR_CHAR_SELF,  POS_STANDING,
        NULL,                           20,     12,
        "",                             "You no longer feel so HUGE."
    },

    {
        "flesh armor",          { L_APP, L_APP, L_APP, L_APP,    11 },
        spell_flesh_armor,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   15,     12,
        "",                     "Your skin returns to normal."
    },

    {
        "heighten senses",      { L_APP, L_APP, L_APP, L_APP,     3 },
        spell_null,             TAR_CHAR_SELF,          POS_STANDING,
        &gsn_heighten,          0,      0,
        "",                     "Your senses return to normal."
    },

    {
        "inertial barrier",     { L_APP, L_APP, L_APP, L_APP,    19 },
        spell_inertial_barrier, TAR_IGNORE,             POS_STANDING,
        NULL,                   40,     24,
        "",                     "Your inertial barrier dissipates."
    },

    {
        "inflict pain",         { L_APP, L_APP, L_APP, L_APP,     5 },
        spell_inflict_pain,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   10,     12,
        "mindpower",            "!Inflict Pain!"
    },

    {
        "intellect fortress",   { L_APP, L_APP, L_APP, L_APP,    12 },
        spell_intellect_fortress,       TAR_IGNORE,     POS_STANDING,
        NULL,                           25,     24,
        "",                     "Your intellectual fortress crumbles."
    },

    {
        "lend health",          { L_APP, L_APP, L_APP, L_APP,    10 },
        spell_lend_health,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   10,     12,
        "",                     "!Lend Health!"
    },

    {
        "levitation",           { L_APP, L_APP, L_APP, L_APP,     3 },
        spell_levitation,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   10,     18,
        "",                     "You slowly float to the ground."
    },

    {
        "mental barrier",       { L_APP, L_APP, L_APP, L_APP,     6 },
        spell_mental_barrier,   TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   8,     12,
        "",                     "Your mental barrier breaks down."
    },

    {
        "mind thrust",          { L_APP, L_APP, L_APP, L_APP,     2 },
        spell_mind_thrust,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   8,     12,
        "mind thrust",          "!Mind Thrust!"
    },

    {
        "project force",        { L_APP, L_APP, L_APP, L_APP,     9 },
        spell_project_force,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   18,     12,
        "projected force",      "!Project Force!"
    },

    {
        "psionic blast",        { L_APP, L_APP, L_APP, L_APP,    17 },
        spell_psionic_blast,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   25,     12,
        "psionic blast",        "!Psionic Blast!"
    },

    {
        "psychic crush",        { L_APP, L_APP, L_APP, L_APP,     8 },
        spell_psychic_crush,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   15,     18,
        "psychic crush",        "!Psychic Crush!"
    },

    {
        "psychic drain",        { L_APP, L_APP, L_APP, L_APP,     4 },
        spell_psychic_drain,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   20,     12,
        "",                     "You no longer feel drained."
    },

    {
        "psychic healing",      { L_APP, L_APP, L_APP, L_APP,     5 },
        spell_psychic_healing,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   20,      12,
        "",                     "!Psychic Healing!"
    },

    {
        "shadow form",          { L_APP, L_APP, L_APP, L_APP,     2 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_shadow,            0,     12,
        "",                     "You no longer move in the shadows."
    },

    {
        "share strength",       { L_APP, L_APP, L_APP, L_APP,    11 },
        spell_share_strength,   TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   8,     12,
        "",                     "You no longer share strength with another."
    },

    {
        "thought shield",       { L_APP, L_APP, L_APP, L_APP,     1 },
        spell_thought_shield,   TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   5,     12,
        "",                     "You no longer feel so protected."
    },

    {
        "ultrablast",           { L_APP, L_APP, L_APP, L_APP,    25 },
        spell_ultrablast,       TAR_IGNORE,             POS_FIGHTING,
        NULL,                   75,     24,
        "ultrablast",           "!Ultrablast!"
    },

    {
        "stake",                {     1,     1,     1,     1,     1 },
        spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_stake,             0,       8,
        "carefully aimed stake","!Stake!"
    },

	/* New abilities added by Zen made by other ppl */
    {
        "scan",			{ L_APP, L_APP,    30, L_APP, L_APP },
        spell_null,		TAR_IGNORE,     POS_STANDING,
        &gsn_scan,		0,       24,
        "",			"!Scan!"
    },

    {
        "shield block",         {    37,    37,    17,     7,    37 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_shield_block,      0,      0,
        "",                     "!Shield Block!"
    },

    {
        "fast healing",         {    37,    37,    27,    20,    37 },
        spell_null,             TAR_IGNORE,             POS_DEAD,
        &gsn_fast_healing,      0,      0,
        "",                     "!Fast Healing!"
    },

   {
        "fourth attack",        { L_APP, L_APP, L_APP,    30,  L_APP },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_fourth_attack,     0,      0,
        "",                     "!Fourth Attack!"
    },

    {
	"brew",			{    27,    31, L_APP, L_APP,  L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_brew,		0,	 24,
	"blunder",		"!Brew!"
    },

    {
	"scribe",		{    28,    32, L_APP, L_APP,  L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_scribe,		0,	 24,
	"blunder",		"!Scribe!"
    },

	/*
	 * spells & skills here by Zen :)
	 */
    {
        "dirt kicking",		{ L_APP, L_APP,    20,    25,  L_APP },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dirt,              0,      24,
        "kicked dirt",          "You rub the dirt out of your eyes."
    },

    {
        "swim",                  {   15,    15,    15,    15,     15 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_swim,		 0,	 0,
	"",			"!Swim!"
    },

    {
        "meditate",         	{    20,    37, L_APP, L_APP,    20 },
        spell_null,             TAR_IGNORE,             POS_DEAD,
        &gsn_meditate,      	0,      0,
        "",			"!Meditate!"
    },

    {
        "meteor swarm",		{    25, L_APP, L_APP, L_APP, L_APP },
        spell_meteor_swarm,	TAR_IGNORE,     POS_FIGHTING,
        NULL,			20,       12,
        "meteor swarm",		"!Meteor Swarm!"
    },

    {   
	"chain lightning",	{    16, L_APP, L_APP, L_APP, L_APP },
	spell_chain_lightning,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	12,
	"lightning",		"!Chain Lightning!"
    }, 

    {
	"scry",			{    35,    35, L_APP, L_APP,    35 },
	spell_scry, 		TAR_IGNORE,		POS_STANDING,
	NULL,			15,	12,
	"",			"!Scry!"
    },
    
    {
	"vortex lift",		{    27, L_APP, L_APP, L_APP,    27 },
	spell_vortex_lift,	TAR_IGNORE,		POS_STANDING,
	NULL,			50,	12,
	"",			"!Vortex Lift!"
    },
    
    {
	"mass vortex lift",	{ L_APP,    40, L_APP, L_APP, L_APP },
	spell_mass_vortex_lift,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_vortex_lift,	150,	12,
	"",			"!Mass Vortex Lift!"
    },

    {
	"home sick",		{ L_APP, L_APP, L_APP, L_APP, L_APP },
	spell_home_sick,	TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,			0,	2,
	"",			"!Home Sick!"
    },

    {
	"detect good",		{ L_APP,     4, L_APP, L_APP, L_APP },
	spell_detect_good,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"The gold in your vision disappears."
    },

    {
	"dispel good",		{ L_APP,    10, L_APP, L_APP, L_APP },
	spell_dispel_good,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"unholy fire",		"!Dispel Good!"
    },

    {
        "portal",               {    35,    30, L_APP, L_APP,    40 },
        spell_portal,           TAR_IGNORE,             POS_STANDING,
        NULL,                   100,     24,
        "",                     "!Portal!",
    },

    {
        "nexus",                {    40,    35, L_APP, L_APP,    45 },
        spell_nexus,            TAR_IGNORE,             POS_STANDING,
        NULL,                   150,   36,
        "",                     "!Nexus!"
    },

	  /* Race ability spells */
    {
        "vampiric bite",        { L_APP, L_APP, L_APP, L_APP ,L_APP },
        spell_vampiric_bite,    TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
        &gsn_vampiric_bite,     0,     0,
        "vampiric bite",        "You feel well fed."
    }

};
