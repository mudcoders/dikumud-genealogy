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
 *  GreedMud 0.88 improvements copyright (C) 1997, 1998 by Vasco Costa.    *
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
 * Race types
 */
const   struct    race_type       race_table      [MAX_RACE]	=
{
    {
	"Human",
	RACE_PC_AVAIL | RACE_WEAPON_WIELD,
	SIZE_MEDIUM,		0, 0, 0, 0, 0,		0, 0, 0,	0, 0,
	"punch",
	"Githyanki Vampire Werewolf Mindflayer",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_NONE,
	RIS_NONE,
	RIS_NONE,
	LANG_COMMON
    },

    {
	"Elf",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN |
		RACE_WEAPON_WIELD,
	SIZE_AVERAGE,		0, 1, 0, 1, -1,		0, 4, 0,	0, 0,
	"punch",
	"Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_CHARM,
	RIS_NONE,
	RIS_IRON,
	LANG_ELVEN
    },

    {
	"Halfelf",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	SIZE_MEDIUM,		0, 0, 0, 1, 0,		0, 2, 0,	0, 0,
	"punch",
	"Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_CHARM | RIS_POISON,
	RIS_NONE,
	RIS_IRON,
	LANG_ELVEN
    },

    {
	"Drow",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN |
		RACE_WEAPON_WIELD,
	SIZE_PETITE,		0, 0, 1, 1, 0,		0, 4, 0,	0, 0,
	"punch",
	"Elf Halfelf Hobbit Githyanki Vampire Werewolf",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_MAGIC,
	RIS_NONE,
	RIS_NONE,
	LANG_DROW
    },

    {
	"Dwarf",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	SIZE_PETITE,		0, 0, 0, -1, 1,		0, 0, 0,	0, 1,
	"punch",
	"Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_POISON | RIS_DISEASE,
	RIS_NONE,
	RIS_DROWNING,
	LANG_DWARVEN
    },

    {
	"Halfdwarf",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	SIZE_PETITE,		0, 0, 0, 0, 1,		0, 0, 0,	0, 0,
	"punch",
	"Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_POISON | RIS_DISEASE,
	RIS_NONE,
	RIS_DROWNING,
	LANG_DWARVEN
    },

    {
	"Hobbit",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN |
		RACE_WEAPON_WIELD,
	SIZE_PETITE,		0, 0, 0, 1, -1,		0, 0, 0,	0, 0,
	"punch",
	"Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_POISON | RIS_DISEASE | RIS_MAGIC,
	RIS_NONE,
	RIS_NONE,
	LANG_HALFLING
    },

    {
	"Gnome",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	SIZE_PETITE,		-1, 0, 1, 1, -1,	0, 4, 0,	0, 0,
	"punch",
	"Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_POISON | RIS_DISEASE | RIS_MAGIC,
	RIS_NONE,
	RIS_NONE,
	LANG_GNOMISH
    },

    {
	"Halfkobold",
	RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	SIZE_PETITE,		-2, -1, -2, 3, -2,	0, 0, 0,	0, 0,
	"punch",
	"Ogre Orc Giant Troll Hobgoblin",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE | PART_TAIL,
	RIS_POISON,
	RIS_NONE,
	RIS_MAGIC,
	LANG_KOBOLD
    },

    {
	"Giant",
	RACE_WEAPON_WIELD,
	SIZE_HUGE,		2, -1, 0, -1, 1,	3, -5, -2,	2, 4,
	"fist",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_FIRE | RIS_COLD,
	RIS_NONE,
	RIS_MENTAL,
	LANG_COMMON
    },

    {
	"Ogre",
	RACE_WEAPON_WIELD,
	SIZE_HUGE,		1, -1, 0, -1, 1,	2, -3, -1,	0, 2,
	"fist",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_NONE,
	RIS_NONE,
	RIS_MENTAL,
	LANG_OGRE
    },

    {
	"Orc",
	RACE_INFRAVISION | RACE_WEAPON_WIELD,
	SIZE_LARGE,		1, -1, 0, 0, 0,		2, -1, -1,	0, 1,
	"punch",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_DISEASE,
	RIS_NONE,
	RIS_LIGHT,
	LANG_ORCISH
    },

    {
	"Kobold",
	RACE_INFRAVISION | RACE_WEAPON_WIELD,
	SIZE_PETITE,		-1, -1, 0, 1, 0,	0, 0, 0,	0, 0,
	"punch",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome Halfkobold",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE | PART_TAIL,
	RIS_POISON,
	RIS_NONE,
	RIS_MAGIC,
	LANG_KOBOLD
    },

    {
	"Minotaur",
	RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	SIZE_LARGE,		2, 0, 0, -1, 1,		3, 0, -1,	0, 3,
	"fist",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE | PART_TAIL,
	RIS_NONE,
	RIS_NONE,
	RIS_NONE,
	LANG_COMMON
    },

    {
	"Troll",
	RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	SIZE_TITANIC,		2, -1, 0, 0, 1,		10, 0, 0,	0, 8,
	"fist",
	"Human Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE | PART_CLAWS | PART_FANGS,
	RIS_CHARM | RIS_BASH,
	RIS_NONE,
	RIS_FIRE | RIS_ACID,
	LANG_TROLLESE
    },

    {
	"Hobgoblin",
	RACE_INFRAVISION | RACE_WEAPON_WIELD,
	SIZE_MEDIUM,		1, 0, -1, 0, 1,		0, 0, 0,	0, 0,
	"punch",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_DISEASE | RIS_POISON,
	RIS_NONE,
	RIS_NONE,
	LANG_COMMON
    },

    {
	"Insect",
	RACE_MUTE,
	SIZE_MINUTE,		0, 0, 0, 0, -1,		0, 0, 5,	0, 0,
	"bite",
	"",
	PART_HEAD | PART_LEGS | PART_EYE,
	RIS_NONE,
	RIS_NONE,
	RIS_NONE,
	LANG_INSECTOID
    },

    {
	"Dragon",
	RACE_FLY | RACE_INFRAVISION | RACE_DETECT_ALIGN | RACE_DETECT_INVIS |
		RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	SIZE_GARGANTUAN,	2, 2, 1, -3, 2,		15, 0, -10,	0, 10,
	"claw",
	"",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE | PART_WINGS | PART_TAIL | PART_CLAWS | PART_FANGS |
		PART_SCALES,
	RIS_FIRE | RIS_BASH | RIS_CHARM,
	RIS_NONE,
	RIS_PIERCE | RIS_COLD,
	LANG_DRAGON
    },

    {
	"Animal",
	RACE_DETECT_HIDDEN | RACE_MUTE,
	SIZE_PETITE,		0, 0, 0, 1, 0,		0, 0, 0,	0, 0,
	"bite",
	"Kobold Halfkobold",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_GUTS | PART_FEET |
		PART_EAR | PART_EYE,
	RIS_NONE,
	RIS_NONE,
	RIS_MAGIC,
	LANG_ANIMAL
    },

    {
	"God",
	RACE_WATERBREATH | RACE_FLY | RACE_SWIM | RACE_WATERWALK |
		RACE_PASSDOOR | RACE_INFRAVISION | RACE_DETECT_ALIGN |
		RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_PROTECTION |
		RACE_SANCT | RACE_WEAPON_WIELD,
	SIZE_GARGANTUAN,	3, 3, 3, 3, 3,		20, 0, 10,	0, 0,
	"smite",
	"",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_MAGIC | RIS_POISON | RIS_MENTAL | RIS_CHARM,
	RIS_DROWNING | RIS_DISEASE,
	RIS_NONE,
	LANG_GOD
    },

    {
	"Undead",
	RACE_PASSDOOR | RACE_INFRAVISION | RACE_DETECT_ALIGN |
	           RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	SIZE_MEDIUM,		1, 0, 0, -2, 1,		0, 0, 0,	-1, -1,
	"touch",
	"Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Goblin Faerie Gnome",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_WEAPON,
	RIS_DISEASE | RIS_POISON,
	RIS_LIGHT | RIS_WHITE_MANA,
	LANG_SPIRITUAL
    },

    {
	"Harpy",
	RACE_FLY | RACE_DETECT_INVIS | RACE_MUTE,
	SIZE_MEDIUM,		0, 0, 0, 2, 0,		0, 0, 6,	0, 0,
	"claw",
	"Human Elf Halfelf Dwarf Halfdwarf Hobbit Gnome",
	PART_HEAD | PART_LEGS | PART_HEART | PART_BRAINS | PART_GUTS |
		PART_FEET | PART_FINGERS | PART_EAR | PART_EYE | PART_WINGS |
		PART_TAIL | PART_CLAWS | PART_FANGS | PART_SCALES,
	RIS_SOUND,
	RIS_NONE,
	RIS_NONE,
	LANG_SPIRITUAL
    },

    {
	"Bear",
	RACE_SWIM | RACE_DETECT_HIDDEN | RACE_MUTE,
	SIZE_LARGE,		1, 0, 0, -1, 1,		0, 0, 0,	0, 3,
	"swipe",
	"",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_EAR | PART_EYE |
		PART_CLAWS | PART_FANGS,
	RIS_BASH | RIS_COLD,
	RIS_NONE,
	RIS_NONE,
	LANG_MAMMAL
    },

    {
	"Githyanki",
	RACE_WEAPON_WIELD,
	SIZE_MEDIUM,		0, 1, 0, 0, 0,		0, 4, 0,	0, 0,
	"punch",
	"Mindflayer",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_NONE,
	RIS_NONE,
	RIS_NONE,
	LANG_GITH
    },

    {
	"Elemental",
	RACE_MUTE,
	SIZE_LARGE,		1, 0, 0, 0, 1,		0, 0, 0,	-1, -1,
	"punch",
	"",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE,
	RIS_NONE,
	RIS_NONE,
	RIS_NONE,
	LANG_MAGICAL
    },

    {
	"Bat",
	RACE_FLY | RACE_INFRAVISION | RACE_MUTE,
	SIZE_MINUTE,		-1, 0, 0, 2, -1,	0, 0, 4,	0, 0,
	"bite",
	"",
	PART_HEAD | PART_LEGS | PART_HEART | PART_BRAINS | PART_GUTS |
		PART_FEET | PART_EAR | PART_EYE | PART_WINGS | PART_CLAWS |
		PART_FANGS,
	RIS_NONE,
	RIS_NONE,
	RIS_NONE,
	LANG_RODENT
    },

    {
	"Plant",
	RACE_MUTE,
	SIZE_MINUTE,		0, 0, 0, -1, 1,		0, 0, 0,	-1, 0,
	"swipe",
	"",
	PART_HEAD | PART_FANGS,
	RIS_DROWNING,
	RIS_NONE,
	RIS_FIRE | RIS_COLD,
	LANG_PLANT
    },

    {
	"Rat",
	RACE_PASSDOOR | RACE_MUTE,
	SIZE_MINUTE,		-1, 0, 0, 2, -1,	0, 0, 0,	0, 0,
	"bite",
	"",
	PART_HEAD | PART_LEGS | PART_HEART | PART_BRAINS | PART_GUTS |
		PART_FEET | PART_EAR | PART_EYE | PART_FANGS,
	RIS_NONE,
	RIS_NONE,
	RIS_NONE,
	LANG_RODENT
    },

    {
	"Vampire",
	RACE_FLY | RACE_PASSDOOR | RACE_INFRAVISION | RACE_DETECT_ALIGN |
		RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	SIZE_MEDIUM,		1, 1, 0, 1, 2,		10, 3, 3,	-1, 10,
	"claw",
	"Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Werewolf Goblin Faerie Gnome",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE | PART_FANGS,
	RIS_DISEASE | RIS_BLACK_MANA,
	RIS_POISON,
	RIS_DROWNING | RIS_WOOD | RIS_LIGHT | RIS_WHITE_MANA,
	LANG_COMMON
    },

    {
	"Werewolf",
	RACE_INFRAVISION | RACE_DETECT_ALIGN | RACE_DETECT_INVIS |
	    RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	SIZE_MEDIUM,		2, 0, 1, 2, 3,		10, 0, 5,	0, 5,
	"claw",
	"Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Vampire Goblin Faerie Gnome",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE | PART_TAIL | PART_CLAWS | PART_FANGS,
	RIS_BASH,
	RIS_NONE,
	RIS_SILVER,
	LANG_SPIRITUAL
    },

    {
	"Goblin",
	RACE_INFRAVISION | RACE_WEAPON_WIELD,
	SIZE_PETITE,		-1, -1, -1, 1, 0,	0, 0, 0,	0, 0,
	"punch",
	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE | PART_TUSKS,
	RIS_DISEASE,
	RIS_NONE,
	RIS_MAGIC,
	LANG_GOBLIN
    },

    {
	"Faerie",
	RACE_FLY | RACE_INFRAVISION | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN |
		RACE_WEAPON_WIELD,
	SIZE_MINUTE,		-2, 1, 1, 1, -1,	0, 0, 7,	0, 0,
	"punch",
	"",
	PART_HEAD | PART_ARMS | PART_LEGS | PART_HEART | PART_BRAINS |
		PART_GUTS | PART_HANDS | PART_FEET | PART_FINGERS | PART_EAR |
		PART_EYE | PART_WINGS,
	RIS_MAGIC,
	RIS_NONE,
	RIS_NONE,
	LANG_PIXIE
    },

    {
	"Arachnid",
	RACE_NO_ABILITIES | RACE_WEAPON_WIELD | RACE_MUTE,
	SIZE_SMALL,		0, 0, 0, 1, 0,		0, 0, 0,	0, 0,
	"bite",
	"",
	PART_HEAD | PART_LEGS | PART_HEART | PART_FEET | PART_EYE |
		PART_FANGS,
	RIS_POISON,
	RIS_NONE,
	RIS_NONE,
	LANG_INSECTOID
    },

    {
	"Mindflayer",
	RACE_INFRAVISION | RACE_WEAPON_WIELD,
	SIZE_MEDIUM,		1, 2, 1, -1, 0,		0, 6, 0,	0, 0,
	"punch",
	"Githyanki",
	PART_LEGS | PART_HEART | PART_GUTS | PART_FEET | PART_TENTACLES,
	RIS_NONE,
	RIS_NONE,
	RIS_NONE,
	LANG_COMMON
    },

    {
	"Object",
	RACE_WATERBREATH | RACE_MUTE,
	SIZE_AVERAGE,		3, 0, 0, 0, 3,		0, 0, 0,	-1, -1,
	"swing",
	"",
	PART_NONE,
	RIS_LIGHT,
	RIS_POISON | RIS_DISEASE | RIS_DROWNING,
	RIS_ENERGY,
	LANG_MAGICAL
    },

    {
	"Mist",
	RACE_FLY | RACE_PASSDOOR | RACE_MUTE,
	SIZE_PETITE,		-1, 0, 0, 3, 0,		0, 0, 20,	-1, -1,
	"gas",
	"",
	PART_NONE,
	RIS_NONE,
	RIS_DISEASE | RIS_POISON,
	RIS_MAGIC,
	LANG_SPIRITUAL
    },

    {
	"Snake",
	RACE_MUTE,
	SIZE_MINUTE,		0, 0, 0, 1, 0,		0, 0, 0,	0, 0,
	"bite",
	"",
	PART_HEAD | PART_HEART | PART_BRAINS | PART_GUTS | PART_EYE |
		PART_TAIL | PART_FANGS | PART_SCALES,
	RIS_POISON,
	RIS_NONE,
	RIS_COLD,
	LANG_REPTILE
    },

    {
	"Worm",
	RACE_PASSDOOR | RACE_MUTE,
	SIZE_MINUTE,		0, 0, 0, 0, 0,		0, 0, 0,	0, 0,
	"slime",
	"",
	PART_HEAD | PART_TAIL,
	RIS_NONE,
	RIS_NONE,
	RIS_BASH,
	LANG_INSECTOID
    },

    {
	"Fish",
	RACE_WATERBREATH | RACE_SWIM | RACE_MUTE,
	SIZE_MINUTE,		0, 0, 0, 2, 0,		0, 0, 0,	-1, 0,
	"slap",
	"",
	PART_HEAD | PART_HEART | PART_BRAINS | PART_GUTS | PART_EYE |
		PART_TAIL | PART_SCALES | PART_FINS,
	RIS_NONE,
	RIS_DROWNING,
	RIS_NONE,
	LANG_FISH
    },

    {
	"Hydra",
	RACE_DETECT_HIDDEN | RACE_MUTE,
	SIZE_TITANIC,		2, 0, 0, -1, 2,		0, 0, 0,	0, 0,
	"bite",
	"",
	PART_HEAD | PART_LEGS | PART_HEART | PART_BRAINS | PART_GUTS |
		PART_FEET | PART_EYE | PART_TAIL | PART_FANGS | PART_SCALES,
	RIS_BASH,
	RIS_NONE,
	RIS_PIERCE,
	LANG_DRAGON
    },

    {
	"Lizard",
	RACE_MUTE,
	SIZE_MINUTE,		-1, 0, 0, 1, 0,		0, 0, 0,	0, 0,
	"lash",
	"",
	PART_HEAD | PART_LEGS | PART_HEART | PART_BRAINS | PART_GUTS |
		PART_FEET | PART_EYE | PART_TAIL | PART_FANGS,
	RIS_POISON,
	RIS_NONE,
	RIS_COLD,
	LANG_REPTILE
    }
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
 * Attack damage type and string.
 */
const	struct	attack_type	attack_table	[MAX_ATTACK]	=
{
    {  "hit",       &gsn_hit,      DAM_BASH,	NULL             },  /*  0   */
    {  "slice",     &gsn_slash,    DAM_SLASH,	NULL             },  /*  1   */
    {  "stab",      &gsn_pierce,   DAM_PIERCE,	NULL             },
    {  "slash",     &gsn_slash,    DAM_SLASH,	NULL             },
    {  "whip",      &gsn_whip,     DAM_SLASH,	NULL             },
    {  "claw",      &gsn_claw,	   DAM_SLASH,	NULL             },  /*  5   */
    {  "blast",     &gsn_explode,  DAM_BASH,	NULL             },
    {  "pound",     &gsn_pound,    DAM_BASH,	NULL             },
    {  "crush",     &gsn_pound,    DAM_BASH,	NULL             },
    {  "grep",      &gsn_pierce,   DAM_PIERCE,	NULL             },
    {  "bite",      &gsn_pierce,   DAM_PIERCE,	NULL             },  /*  10  */
    {  "pierce",    &gsn_pierce,   DAM_PIERCE,	NULL             },
    {  "suction",   &gsn_suction,  DAM_BASH,	hit_suck_disarm  },
    {  "chop",      &gsn_slash,    DAM_SLASH,	NULL             },
    {  "vorpal",    &gsn_slash,    DAM_SLASH,	hit_vorpal       },
    {  "cleave",    &gsn_slash,    DAM_SLASH,	NULL             },  /*  15  */
    {  "wail",      &gsn_hit,      DAM_BASH,	NULL             }
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
	"acid blast",
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"acid blast",		"!Acid Blast!",
	MANA_WATER,		""
    },

    {
	"acid breath",
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of acid",	"!Acid Breath!",
	MANA_WATER,		""
    },

    {
        "adrenaline control",
        spell_adrenaline_control,       TAR_CHAR_SELF,  POS_STANDING,
        NULL,				6,      12,
        "",				"The adrenaline rush wears off.",
        MANA_WATER,			""
    },

    {
        "agitation",
        spell_agitation,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   10,     12,
        "molecular agitation",  "!Agitation!",
        MANA_AIR,		""
    },

    {
	"antimagic shell",
	spell_antimagic_shell,	TAR_CHAR_DEFENSIVE,  	POS_STANDING,
	NULL,			40,	12,
	"",			"The shimmering antimagic shell fades away...",
	MANA_EARTH,		""
    },

    {
	"aquiles power",
	spell_aquiles_power,	TAR_CHAR_DEFENSIVE,  	POS_STANDING,
	NULL,			50,	60,
	"",			"The aquiles power within you dissipates...",
	MANA_EARTH,		""
    },

    {
	"armor",
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected.",
	MANA_EARTH,		""
    },

    {
        "aura sight",
        spell_aura_sight,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   9,     12,
        "",                     "!Aura Sight!",
        MANA_FIRE,		""
    },

    {
        "awe",
        spell_awe,              TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
        NULL,                   35,     12,
        "",                     "!Awe!",
        MANA_WATER,		""
    },

    {
        "ballistic attack",
        spell_ballistic_attack, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   5,     12,
        "ballistic attack",     "!Ballistic Attack!",
        MANA_AIR,		""
    },

    {
        "biofeedback",
        spell_biofeedback,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   75,     12,
        "",                     "Your biofeedback is no longer effective.",
        MANA_WATER,		""
    },

    {
	"blazebane",
	spell_blazebane,	TAR_CHAR_OFFENSIVE,  	POS_STANDING,
	NULL,			70,	15,
	"",			"Your flesh grows less susceptible to fire.",
	MANA_FIRE,		""
    },

    {
	"blazeward",
	spell_blazeward,	TAR_CHAR_DEFENSIVE,  	POS_STANDING,
	NULL,			70,	15,
	"",			"The ward of flames ceases to protect you.",
	MANA_FIRE,		""
    },

    {
	"bless",
	spell_bless,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less righteous.",
	MANA_AIR,		""
    },

    {
	"blindness",
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,		5,	12,
	"",			"You can see again.",
	MANA_FIRE,		""
    },

    {
	"breathe water",
	spell_breathe_water,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_breathe_water, 	 5,	12,
	"pained lungs",		"You can no longer breathe underwater.",
	MANA_WATER,		""
    },

    {
	"burning hands",
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_burning_hands,     15,	12,
	"burning hands",	"!Burning Hands!",
	MANA_FIRE,		""
    },

    {
	"call lightning",
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			15,	12,
	"lightning bolt",	"!Call Lightning!",
	MANA_AIR,		""
    },

    {
	"cause critical",
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"invocation",		"!Cause Critical!",
	MANA_AIR,		""
    },

    {
	"cause light",
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"invocation",		"!Cause Light!",
	MANA_AIR,		""
    },

    {
	"cause serious",
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			17,	12,
	"invocation",		"!Cause Serious!",
	MANA_AIR,		""
    },

    {
        "cell adjustment",
        spell_cell_adjustment,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   8,     12,
        "",                     "!Cell Adjustment!",
        MANA_EARTH,		""
    },

    {   
	"chain lightning",
	spell_chain_lightning,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	12,
	"lightning",		"!Chain Lightning!",
	MANA_FIRE,		""
    }, 

    {
	"change sex",
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	0,
	"",			"Your body feels familiar again.",
	MANA_FIRE,		""
    },

    {
	"charm person",
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	5,	12,
	"",			"You feel more self-confident.",
	MANA_AIR,		""
    },

    {
	"chill touch",
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"chilling touch",	"You feel less cold.",
	MANA_WATER,		""
    },

    {
	"colour spray",
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"colour spray",		"!Colour Spray!",
	MANA_AIR,		""
    },

    {
        "combat mind",
        spell_combat_mind,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   15,     12,
        "",                     "Your battle sense has faded.",
        MANA_EARTH,		""
    },

    {
        "complete healing",
        spell_complete_healing, TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   100,    12,
        "",                     "!Complete Healing!",
        MANA_EARTH,		""
    },

    {
        "cone of silence",
        spell_cone_of_silence,	TAR_IGNORE,		POS_FIGHTING,
        NULL,			35,     12,
        "",                     "!Cone of Silence!",
        MANA_EARTH,		""
    },

    {
	"continual light",
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,			7,	12,
	"",			"!Continual Light!",
	MANA_FIRE,		""
    },

    {
        "control flames",
        spell_control_flames,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   15,     12,
        "tongue of flame",      "!Control Flames!",
        MANA_FIRE,		""
    },

    {
	"control weather",
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,			25,	12,
	"",			"!Control Weather!",
	MANA_AIR,		""
    },

    {
	"create buffet",
	spell_create_buffet,	TAR_IGNORE,		POS_STANDING,
	NULL,			33,	12,
	"",			"!Create Buffet!",
	MANA_EARTH,		""
    },

    {
	"create food",
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Create Food!",
	MANA_EARTH,		""
    },

    {
        "create sound",
        spell_create_sound,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   5,     12,
        "",                     "!Create Sound!",
        MANA_AIR,		""
    },

    {
	"create spring",
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	12,
	"",			"!Create Spring!",
	MANA_WATER,		""
    },

    {
	"create water",
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Create Water!",
	MANA_WATER,		""
    },

    {
	"cure blindness",
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			5,	12,
	"",			"!Cure Blindness!",
	MANA_WATER,		""
    },

    {
	"cure critical",
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"",			"!Cure Critical!",
	MANA_EARTH,		""
    },

    {
	"cure disease",
	spell_cure_disease,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	12,
	"",			"!Cure Disease!",
	MANA_WATER,		""
    },

    {
	"cure light",
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			10,	12,
	"",			"!Cure Light!",
	MANA_EARTH,		""
    },

    {
	"cure poison",
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"!Cure Poison!",
	MANA_WATER,		""
    },

    {
	"cure serious",
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"",			"!Cure Serious!",
	MANA_EARTH,		""
    },

    {
	"curse",
	spell_curse,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_curse,		20,	12,
	"curse",		"The curse wears off.",
	MANA_AIR,		""
    },

    {
        "death field",
        spell_death_field,      TAR_IGNORE,             POS_FIGHTING,
        NULL,                   200,    18,
        "field of death",       "!Death Field!",
        MANA_FIRE,		""
    },

    {
	"demon skin",
	spell_demon_skin,	TAR_CHAR_DEFENSIVE,  	POS_STANDING,
	NULL,			55,	25,
	"",			"Your leathery skin grows thinner...",
	MANA_EARTH,		""
    },

    {
	"destroy cursed",
	spell_destroy_cursed,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	12,
	"",			"!destroy cursed!",
	MANA_EARTH,		""
    },

    {
	"detect evil",
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"The red in your vision disappears.",
	MANA_FIRE,		""
    },

    {
	"detect good",
	spell_detect_good,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"The gold in your vision disappears.",
	MANA_FIRE,		""
    },

    {
	"detect hidden",
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less aware of your surroundings.",
	MANA_FIRE,		""
    },

    {
	"detect invis",
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You no longer see invisible objects.",
	MANA_FIRE,		""
    },

    {
	"detect magic",
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING, 
	NULL,			5,	12,
	"",			"The detect magic wears off.",
	MANA_FIRE,		""
    },

    {
	"detect poison",
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Detect Poison!",
	MANA_FIRE,		""
    },

    {
        "detonate",
        spell_detonate,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   35,     24,
        "detonation",           "!Detonate!",
        MANA_FIRE,		""
    },

    {
        "disintegrate",
        spell_disintegrate,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   150,    18,
        "disintegration",       "!Disintegrate!",
        MANA_FIRE,		""
    },

    {
	"dispel evil",
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"holy fire",		"!Dispel Evil!",
	MANA_WATER,		""
    },

    {
	"dispel good",
	spell_dispel_good,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"unholy fire",		"!Dispel Good!",
	MANA_FIRE,		""
    },

    {
	"dispel magic",
	spell_dispel_magic,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	16,
	"",			"!Dispel Magic!",
	MANA_EARTH,		""
    },

    {
        "displacement",
        spell_displacement,     TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   10,     12,
        "",                     "You are no longer displaced.",
        MANA_AIR,		""
    },

    {
        "domination",
        spell_domination,       TAR_CHAR_OFFENSIVE,     POS_STANDING,
        &gsn_domination,        5,     12,
        "",                     "You regain control of your body.",
        MANA_AIR,		""
    },

    {
	"dragon skin",
	spell_dragon_skin,	TAR_CHAR_DEFENSIVE,  	POS_STANDING,
	NULL,			45,	14,
	"",			"Your flesh sheds its draconian aspects.",
	MANA_EARTH,		""
    },

    {
	"dragon wit",
	spell_dragon_wit,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	12,
	"",			"The dragon wit withdraws from your mind.",
	MANA_EARTH,		""
    },

    {
	"earthquake",
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			15,	12,
	"earthquake",		"!Earthquake!",
	MANA_EARTH,		""
    },

    {
        "ectoplasmic form",
        spell_pass_door,	TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   20,     12,
        "",                     "You feel solid again.",
        MANA_AIR,		""
    },

    {
        "ego whip",
        spell_ego_whip,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   20,     12,
        "",                     "You feel more confident.",
        MANA_WATER,		""
    },

    {
	"eldritch sphere",
	spell_eldritch_sphere,	TAR_CHAR_DEFENSIVE,  	POS_STANDING,
	NULL,			70,	20,
	"",			"The eldritch sphere about you vanishes...",
	MANA_EARTH,		""
    },

    {
	"enchant weapon",
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			100,	24,
	"",			"!Enchant Weapon!",
	MANA_FIRE,		""
    },

    {
        "energy containment",
        spell_energy_containment,       TAR_CHAR_SELF,  POS_STANDING,
        NULL,                           10,     12,
        "",			"You no longer absorb energy.",
        MANA_EARTH,		""
    },

    {
	"energy drain",
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			35,	12,
	"energy drain",		"!Energy Drain!",
	MANA_FIRE,		""
    },

    {
        "enhance armor",
        spell_enhance_armor,    TAR_OBJ_INV,    POS_STANDING,
        NULL,                   100,    24,
        "",                     "!Enhance Armor!",
        MANA_FIRE,		""
    },

    {
        "enhanced strength",
        spell_enhanced_strength,        TAR_CHAR_SELF,  POS_STANDING,
        NULL,                           20,     12,
        "",			"You no longer feel so HUGE.",
        MANA_EARTH,		""
    },

    {
	"ethereal funnel",
	spell_ethereal_funnel,	TAR_CHAR_OFFENSIVE,  	POS_STANDING,
	NULL,			65,	8,
	"",			"The ethereal funnel about you vanishes...",
	MANA_ANY,		""
    },

    {
	"ethereal shield",
	spell_ethereal_shield,	TAR_CHAR_DEFENSIVE,  	POS_STANDING,
	NULL,			50,	60,
	"",			"You return to the mundane energy continuum.",
	MANA_ANY,		""
    },

    {
	"exorcise",
	spell_exorcise, 	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			35,	12,
	"",		        "!Exorcise!",
	MANA_WATER,		""
    },

    {
	"faerie fire",
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			5,	12,
	"faerie fire",		"The pink aura around you fades away.",
	MANA_FIRE,		""
    },

    {
	"faerie fog",
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,			12,	12,
	"faerie fog",		"!Faerie Fog!",
	MANA_AIR,		""
    },

    {
	"fire breath",
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of flame",	"!Fire Breath!",
	MANA_FIRE,		""
    },

    {
	"fireball",
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"fireball",		"!Fireball!",
	MANA_FIRE,		""
    },

    {
	"flamestrike",
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"flamestrike",		"!Flamestrike!",
	MANA_FIRE,		""
    },

    {
	"flaming shield",
	spell_flame_shield,  	TAR_CHAR_SELF,  	POS_STANDING,
	&gsn_flame_shield,	100,	60,
	"flaming shield",	"The flaming shield around you dies out.",
	MANA_FIRE,		""
    },

    {
        "flesh armor",
        spell_flesh_armor,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   15,     12,
        "",                     "Your skin returns to normal.",
        MANA_EARTH,		""
    },

    {
	"fly",
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			10,	18,
	"",			"You slowly float to the ground.",
	MANA_AIR,		""
    },

    {
	"frost breath",
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of frost",	"!Frost Breath!",
	MANA_WATER,		""
    },

    {
	"frost shield",
	spell_frost_shield,  	TAR_CHAR_SELF,  	POS_STANDING,
	&gsn_frost_shield,	100,	60,
	"frost shield",		"The frost shield around you melts.",
	MANA_WATER,		""
    },

    {
	"gas breath",
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			50,	12,
	"breath of gas",		"!Gas Breath!",
	MANA_AIR,		""
    },

    {
	"gate",
	spell_gate,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"",			"!Gate!",
	MANA_EARTH,		""
    },

    {
        "general purpose",
	spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   0,      12,
	"general purpose ammo", "!General Purpose Ammo!",
	MANA_FIRE,		""
    },

    {
	"giant strength",
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	12,
	"",			"You feel weaker.",
	MANA_EARTH,		""
    },

    {
	"harm",
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			35,	12,
	"divine power",		"!Harm!",
	MANA_EARTH,		""
    },

    {
	"heal",
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"",			"!Heal!",
	MANA_EARTH,		""
    },

    {
        "high explosive",
	spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   0,      12,
	"high explosive ammo",  "!High Explosive Ammo!",
	MANA_FIRE,		""
    },

    {
	"identify",
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			12,	24,
	"",			"!Identify!",
	MANA_FIRE,		""
    },

    {
        "inertial barrier",
        spell_inertial_barrier, TAR_IGNORE,             POS_STANDING,
        NULL,                   40,     24,
        "",                     "Your inertial barrier dissipates.",
        MANA_AIR,		""
    },

    {
        "inflict pain",
        spell_inflict_pain,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   10,     12,
        "mindpower",            "!Inflict Pain!",
        MANA_FIRE,		""
    },

    {
	"infravision",
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	18,
	"",			"You no longer see in the dark.",
	MANA_FIRE,		""
    },

    {
	"inner warmth",
	spell_inner_warmth,	TAR_CHAR_DEFENSIVE,  	POS_STANDING,
	NULL,			65,	8,
	"",			"The magical warmth within you subsides.",
	MANA_FIRE,		""
    },

    {
        "intellect fortress",
        spell_intellect_fortress,       TAR_IGNORE,     POS_STANDING,
        NULL,                           25,     24,
        "",                     "Your intellectual fortress crumbles.",
        MANA_EARTH,		""
    },

    {
	"invis",
	spell_invis,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_invis,		5,	12,
	"",			"You are no longer invisible.",
	MANA_AIR,		""
    },

    {
	"know alignment",
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			9,	12,
	"",			"!Know Alignment!",
	MANA_FIRE,		""
    },

    {
        "lend health",
        spell_lend_health,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   10,     12,
        "",                     "!Lend Health!",
        MANA_EARTH,		""
    },

    {
        "levitation",
        spell_fly,		TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   10,     18,
        "",                     "You slowly float to the ground.",
        MANA_AIR,		""
    },

    {
	"lightning bolt",
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"lightning bolt",	"!Lightning Bolt!",
	MANA_FIRE,		""
    },

    {
	"lightning breath",
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of lightning",	"!Lightning Breath!",
	MANA_AIR,		""
    },

    {
	"locate object",
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	18,
	"",			"!Locate Object!",
	MANA_FIRE,		""
    },

    {
	"magic missile",
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"magic missile",	"!Magic Missile!",
	MANA_FIRE,		""
    },

    {
	"mass heal",
	spell_mass_heal,	TAR_IGNORE,		POS_STANDING,
	NULL,           	50,	24,
	"",			"!Mass Heal!",
	MANA_EARTH,		""
    },

    {
	"mass invis",
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	20,	24,
	"",			"You are no longer invisible.",
	MANA_AIR,		""
    },

    {
	"mass vortex lift",
	spell_mass_vortex_lift,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_vortex_lift,	150,	12,
	"",			"!Mass Vortex Lift!",
	MANA_AIR,		""
    },

    {
        "mental barrier",
        spell_mental_barrier,   TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   8,     12,
        "",                     "Your mental barrier breaks down.",
        MANA_EARTH,		""
    },

    {
        "meteor swarm",
        spell_meteor_swarm,	TAR_IGNORE,     POS_FIGHTING,
        NULL,			20,       12,
        "meteor swarm",		"!Meteor Swarm!",
        MANA_AIR,		""
    },

    {
        "mind thrust",
        spell_mind_thrust,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   8,     12,
        "mind thrust",          "!Mind Thrust!",
        MANA_WATER,		""
    },

    {
        "mute",
        spell_mute,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
        &gsn_mute,		20,     12,
        "",                     "You are no longer muted.",
        MANA_WATER,		""
    },

    {
        "nexus",
        spell_nexus,            TAR_IGNORE,             POS_STANDING,
        NULL,                   150,   36,
        "",                     "!Nexus!",
        MANA_AIR,		""
    },

    {
	"pass door",
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			20,	12,
	"",			"You feel solid again.",
	MANA_AIR,		""
    },

    {
	"plague",
	spell_plague,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_plague,		20,	12,
	"sickness",		"Your sores vanish.",
	MANA_WATER,		""
    },

    {
	"poison",
	spell_poison,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_poison,		10,	12,
	"burning blood",	"You feel less sick.",
	MANA_WATER,		""
    },

    {
	"polymorph other",
	spell_polymorph_other,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			20,	0,
	"",			"Your body feels familiar again.",
	MANA_FIRE,		""
    },

    {
        "portal",
        spell_portal,           TAR_IGNORE,             POS_STANDING,
        NULL,                   100,     24,
        "",                     "!Portal!",
        MANA_AIR,		""
    },

    {
        "project force",
        spell_project_force,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   18,     12,
        "projected force",      "!Project Force!",
        MANA_EARTH,		""
    },

    {
	"protection",
	spell_protection_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected.",
	MANA_FIRE,		""
    },

    {
	"protection evil",
	spell_protection_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected.",
	MANA_FIRE,		""
    },

    {
	"protection good",
	spell_protection_good,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected.",
	MANA_FIRE,		""
    },

    {
        "psionic blast",
        spell_psionic_blast,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   25,     12,
        "psionic blast",        "!Psionic Blast!",
        MANA_FIRE,		""
    },

    {
        "psychic crush",
        spell_psychic_crush,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   15,     18,
        "psychic crush",        "!Psychic Crush!",
        MANA_EARTH,		""
    },

    {
        "psychic drain",
        spell_psychic_drain,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   20,     12,
        "",                     "You no longer feel drained.",
        MANA_FIRE,		""
    },

    {
        "psychic healing",
        spell_psychic_healing,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   20,      12,
        "",                     "!Psychic Healing!",
        MANA_EARTH,		""
    },

    {
	"razorbait",
	spell_razorbait,	TAR_CHAR_OFFENSIVE,  	POS_STANDING,
	NULL,			45,	14,
	"",			"You are no longer afraid of stabbing weapons.",
	MANA_FIRE,		""
    },

    {                                  
        "recharge item",
        spell_recharge_item,    TAR_OBJ_INV,            POS_STANDING,
        NULL,                   25,     12,
        "blunder",              "!Recharge Item!",
        MANA_ANY,		""
    }, 

    {
	"refresh",
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			12,	18,
	"refresh",		"!Refresh!",
	MANA_EARTH,		""
    },

    {
        "remove alignment",
        spell_remove_alignment,	TAR_OBJ_INV,		POS_STANDING,
        NULL,			10,	12,
        "",                     "!Remove Alignment!",
        MANA_WATER,		""
    },

    {
	"remove curse",
	spell_remove_curse,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"!Remove Curse!",
	MANA_WATER,		""
    },

    {
        "remove silence",
        spell_remove_silence,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
        NULL,			15,     12,
        "",                     "!Remove Silence!",
        MANA_WATER,		""
    },

    {
	"sagacity",
	spell_sagacity,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			12,	12,
	"",			"You sense the weight of wisdom leave you.",
	MANA_EARTH,		""
    },

    {
	"sanctuary",
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			75,	12,
	"",			"The white aura around your body fades.",
	MANA_WATER,		""
    },

    {
        "share strength",
        spell_share_strength,   TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   8,     12,
        "",                     "You no longer share strength with another.",
        MANA_EARTH,		""
    },

    {
	"shield",
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			12,	18,
	"",			"Your force shield shimmers then fades away.",
	MANA_EARTH,		""
    },

    {
	"shock shield",
	spell_shock_shield,	TAR_CHAR_SELF,  	POS_STANDING,
	&gsn_shock_shield,	100,	60,
	"shock shield",		"The energy suddenly fades away.",
	MANA_AIR,		""
    },

    {
	"shocking grasp",
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"shocking grasp",	"!Shocking Grasp!",
	MANA_FIRE,		""
    },

    {
	"sleep",
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,		15,	12,
	"",			"You feel less tired.",
	MANA_EARTH,		""
    },

    {
	"slink",
	spell_slink,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			12,	12,
	"",			"You suddenly feel less coordinated...",
	MANA_EARTH,		""
    },

    {
	"stone skin",
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			12,	18,
	"",			"Your skin feels soft again.",
	MANA_EARTH,		""
    },

    {
	"summon",
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,			50,	12,
	"",			"!Summon!",
	MANA_AIR,		""
    },

    {
	"swordbait",
	spell_swordbait,	TAR_CHAR_OFFENSIVE,  	POS_STANDING,
	NULL,			55,	25,
	"",			"Your fear of slashing weapons dissipates.",
	MANA_FIRE,		""
    },

    {
	"teleport",
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 		35,	12,
	"",			"!Teleport!",
	MANA_AIR,		""
    },

    {
        "thought shield",
        spell_thought_shield,   TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   5,     12,
        "",                     "You no longer feel so protected.",
        MANA_EARTH,		""
    },

    {
	"trollish vigor",
	spell_trollish_vigor,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			16,	12,
	"",			"The trollish vigor dissipates.",
	MANA_EARTH,		""
    },

    {
	"turn undead",
	spell_turn_undead,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_turn_undead,	10,	12,
	"divine exorcism",	"!Turn undead!",
	MANA_WATER,		""
    },

    {
        "ultrablast",
        spell_ultrablast,       TAR_IGNORE,             POS_FIGHTING,
        NULL,                   75,     24,
        "ultrablast",           "!Ultrablast!",
        MANA_FIRE,		""
    },

    {
	"unravel defense",
	spell_unravel_defense,	TAR_CHAR_OFFENSIVE,  	POS_STANDING,
	NULL,			70,	15,
	"",			"Your defenses are restored to their quality.",
	MANA_FIRE,		""
    },

    {
        "vampiric bite",
        spell_vampiric_bite,    TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
        &gsn_vampiric_bite,     0,     0,
        "vampiric bite",        "You feel well fed.",
	MANA_FIRE,		""
    },

    {
	"ventriloquate",
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Ventriloquate!",
	MANA_AIR,		""
    },

    {
	"vortex lift",
	spell_vortex_lift,	TAR_IGNORE,		POS_STANDING,
	NULL,			60,	12,
	"",			"!Vortex Lift!",
	MANA_AIR,		""
    },
    
    {
	"weaken",
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"weakening spell",	"You feel stronger.",
	MANA_WATER,		""
    },

    {
	"winter mist",
	spell_winter_mist,	TAR_CHAR_OFFENSIVE,  	POS_STANDING,
	NULL,			65,	8,
	"",			"The magical chilling mist about you subsides.",
	MANA_AIR,		""
    },

    {
	"wizard eye",
	spell_wizard_eye, 	TAR_IGNORE,		POS_STANDING,
	NULL,			40,	12,
	"",			"!Wizard Eye!",
	MANA_AIR,		""
    },
    
    {
	"word of recall",
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	NULL,			5,	12,
	"",			"!Word of Recall!",
	MANA_AIR,		""
    },

/*
 * Miscelaneous Skills.
 */
    {
	"backstab",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_backstab,		0,	24,
	"vicious backstab",	"!Backstab!",
	MANA_NONE,		""
    },

    {
	"bash door",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_bash,		0,	24,
	"powerful bash",	"!Bash Door!",
	MANA_NONE,		""
    },

    {
        "berserk",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_berserk,		0,      12,
        "",                     "The bloody haze lifts.",
	MANA_NONE,		""
    },

    {
	"blast weapons",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_explode,		0,	0,
	"",			"!Blast Weapons!",
	MANA_NONE,		""
    },

    {
	"bludgeons",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_pound,		0,	0,
	"",			"!Bludgeons!",
	MANA_NONE,		""
    },

    {
	"brew",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_brew,		0,	 24,
	"blunder",		"!Brew!",
        MANA_NONE,		""
    },

    {
        "chameleon power",
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_chameleon,         0,     12,
        "",                     "!Chameleon Power!",
        MANA_NONE,		""
    },

    {
        "circle",
        spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_circle,		0,	24,
        "sneak attack",		"!Circle!",
	MANA_NONE,		""
    },

    {
	"common",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Common!",
	MANA_NONE,		""
    },

    {
        "dirt kicking",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dirt,              0,      24,
        "kicked dirt",          "You rub the dirt out of your eyes.",
        MANA_NONE,		""
    },

    {
	"disarm",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_disarm,		 0,	24,
	"",			"!Disarm!",
	MANA_NONE,		""
    },

    {
	"dodge",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dodge,		 0,	 0,
	"",			"!Dodge!",
	MANA_NONE,		""
    },

    {
	"drow",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Drow!",
	MANA_NONE,		""
    },

    {
        "dual",
        spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_dual,      	 0,	 0,
        "",                     "!Dual!",
	MANA_NONE,		""
    },

    {
	"dwarven",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Dwarven!",
	MANA_NONE,		""
    },

    {
	"elven",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Elven!",
	MANA_NONE,		""
    },

    {
	"enhanced damage",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_enhanced_damage,	0,	 0,
	"",			"!Enhanced Damage!",
	MANA_NONE,		""
    },

    {
        "fast healing",
        spell_null,             TAR_IGNORE,             POS_DEAD,
        &gsn_fast_healing,      0,      0,
        "",                     "!Fast Healing!",
        MANA_NONE,		""
    },

    {
        "fifth attack",
        spell_null,		TAR_IGNORE,             POS_FIGHTING,
        &gsn_fifth_attack,	0,      0,
        "",			"!Fifth Attack!",
        MANA_NONE,		""
    },

    {
	"flexible arms",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_whip,		0,	0,
	"",			"!Flexible arms!",
	MANA_NONE,		""
    },

    {
        "fourth attack",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_fourth_attack,     0,      0,
        "",                     "!Fourth Attack!",
        MANA_NONE,		""
    },

    {
	"gith",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Gith!",
	MANA_NONE,		""
    },

    {
	"gnomish",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Gnomish!",
	MANA_NONE,		""
    },

    {
	"goblin",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Goblin!",
	MANA_NONE,		""
    },

    {
	"halfling",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Halfling!",
	MANA_NONE,		""
    },

    {
        "heighten senses",
        spell_null,             TAR_CHAR_SELF,          POS_STANDING,
        &gsn_heighten,          0,      0,
        "",                     "Your senses return to normal.",
        MANA_FIRE,		""
    },

    {
	"hide",
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		 0,	12,
	"",			"!Hide!",
	MANA_NONE,		""
    },

    {
	"kick",
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_kick,		 0,	 8,
	"mighty kick",		"!Kick!",
	MANA_NONE,		""
    },

    {
	"kobold",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Kobold!",
	MANA_NONE,		""
    },

    {
	"long blades",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_slash,		0,	0,
	"",			"!Long Blades!",
	MANA_NONE,		""
    },

    {
	"marksmanship",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_shot,		0,	0,
	"",			"!Marksmanship!",
	MANA_NONE,		""
    },

    {
        "meditate",
        spell_null,             TAR_IGNORE,             POS_DEAD,
        &gsn_meditate,      	0,      0,
        "",			"!Meditate!",
        MANA_NONE,		""
    },

    {
        "mount",
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_mount,		0,       5,
        "",			"!Mount!",
        MANA_NONE,		""
    },

    {
	"ogre",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Ogre!",
	MANA_NONE,		""
    },

    {
	"orcish",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Orcish!",
	MANA_NONE,		""
    },

    {
	"parry",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_parry,		 0,	 0,
	"",			"!Parry!",
	MANA_NONE,		""
    },

    {
	"peek",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		 0,	 0,
	"",			"!Peek!",
	MANA_NONE,		""
    },

    {
	"pick lock",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		0,	12,
	"",			"!Pick!",
	MANA_NONE,		""
    },

    {
	"pixie",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Pixie!",
	MANA_NONE,		""
    },

    {
	"poison weapon",
	spell_null,		TAR_OBJ_INV,		POS_STANDING,
	&gsn_poison_weapon,	 0,	12,
	"poisonous concoction",	"!Poison Weapon!",
	MANA_NONE,		""
    },

    {
	"pugilism",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_hit,		0,	0,
	"",			"!Pugilism!",
	MANA_NONE,		""
    },

    {
	"punch",
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_punch,		0,	 8,
	"punch",		"!Punch!",
	MANA_NONE,		""
    },

    {
	"rescue",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_rescue,		 0,	12,
	"",			"!Rescue!",
	MANA_NONE,		""
    },

    {
        "scan",
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_scan,		0,       24,
        "",			"!Scan!",
        MANA_NONE,		""
    },

    {
	"scribe",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_scribe,		0,	 24,
	"blunder",		"!Scribe!",
        MANA_NONE,		""
    },

    {
	"scrolls",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_scrolls,	 	 0,	0,
	"blazing scroll",	"!Scrolls!",
	MANA_NONE,		""
    },

    {
	"second attack",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_second_attack,	 0,	 0,
	"",			"!Second Attack!",
	MANA_NONE,		""
    },

    {
        "shadow form",
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_shadow,            0,     12,
        "",                     "You no longer move in the shadows.",
        MANA_NONE,		""
    },

    {
        "shield block",
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_shield_block,      0,      0,
        "",                     "!Shield Block!",
        MANA_NONE,		""
    },

    {
	"short blades",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_pierce,		0,	0,
	"",			"!Short blades!",
	MANA_NONE,		""
    },

    {
	"snare",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_snare,		 0,	12,
	"",			"You are no longer ensnared.",
	MANA_NONE,		""
    },

    {
	"sneak",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		 0,	12,
	"",			NULL,
	MANA_NONE,		""
    },

    {
        "stake",
        spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_stake,             0,       8,
        "carefully aimed stake","!Stake!",
        MANA_NONE,		""
    },

    {
	"staves",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_staves,	 	 0,	0,
	"shattered staff",	"!Staves!",
	MANA_NONE,		""
    },

    {
	"steal",
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		 0,	24,
	"",			"!Steal!",
	MANA_NONE,		""
    },

    {
        "swim",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_swim,		 0,	 0,
	"",			"!Swim!",
	MANA_NONE,		""
    },

    {
	"talonous arms",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_claw,		0,	0,
	"",			"!Talonous Arms!",
	MANA_NONE,		""
    },

    {
	"third attack",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_third_attack,	 0,	 0,
	"",			"!Third Attack!",
	MANA_NONE,		""
    },

    {
	"tornado suction",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_suction,		0,	0,
	"",			"!Tornado Suction!",
	MANA_NONE,		""
    },

    {
        "track",
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_track,		0,       2,
        "",			"!Track!",
        MANA_NONE,		""
    },

    {
	"trollese",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	NULL,			0,	0,
	"",			"!Trollese!",
	MANA_NONE,		""
    },

    {
        "untangle",
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_untangle,		 0,	24,
        "",                     "!Untangle!",
	MANA_NONE,		""
    },

    {
	"wands",
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_wands,	 	 0,	0,
	"exploding wand",	"!Wands!",
	MANA_NONE,		""
    },

    {
	"whirlwind",
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_whirlwind,		0,	 12,
	"WhirlWind",		"!Whirlwind!",
        MANA_NONE,		""
    }

};
