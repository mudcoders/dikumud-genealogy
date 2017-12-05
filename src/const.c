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
	LANG_TROLLISH
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
	RIS_LIGHT,
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
    {  "claw",      &gsn_slash,	   DAM_SLASH,	NULL             },  /*  5   */
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
	"reserved",		{ L_APP },
	0,			TAR_IGNORE,		POS_STANDING,
	NULL,			0,	 0,
	"",			"",
	SCHOOL_NONE,
	MANA_NONE,
    },

    {
	"acid blast",		{ L_APP },
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"acid blast",		"!Acid Blast!",
	SCHOOL_EVOCATION,
	MANA_WATER
    },

    {
	"armor",		{ L_APP },
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected.",
	SCHOOL_CONJURATION,
	MANA_EARTH
    },

    {
	"bless",		{ L_APP },
	spell_bless,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less righteous.",
	SCHOOL_CONJURATION | SCHOOL_SUMMONING,
	MANA_AIR
    },

    {
	"blindness",		{ L_APP },
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,		5,	12,
	"",			"You can see again.",
	SCHOOL_ABJURATION | SCHOOL_NECROMANCY,
	MANA_FIRE
    },

    {
	"breathe water",	{ L_APP },
	spell_breathe_water,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_breathe_water, 	 5,	12,
	"pained lungs",		"You can no longer breathe underwater.",
	SCHOOL_SURVIVAL,
	MANA_WATER
    },

    {
	"burning hands",	{ L_APP },
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_burning_hands,     15,	12,
	"burning hands",	"!Burning Hands!",
	SCHOOL_ALTERATION,
	MANA_FIRE
    },

    {
	"call lightning",	{ L_APP },
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			15,	12,
	"lightning bolt",	"!Call Lightning!",
	SCHOOL_ALTERATION,
	MANA_AIR
    },

    {
	"cause critical",	{ L_APP },
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"invocation",		"!Cause Critical!",
	SCHOOL_NECROMANCY,
	MANA_AIR
    },

    {
	"cause light",		{ L_APP },
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"invocation",		"!Cause Light!",
	SCHOOL_NECROMANCY,
	MANA_AIR
    },

    {
	"cause serious",	{ L_APP },
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			17,	12,
	"invocation",		"!Cause Serious!",
	SCHOOL_NECROMANCY,
	MANA_AIR
    },

    {
	"change sex",		{ L_APP },
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	0,
	"",			"Your body feels familiar again.",
	SCHOOL_ENCHANTMENT | SCHOOL_ALTERATION,
	MANA_ANY
    },

    {
	"charm person",		{ L_APP },
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	5,	12,
	"",			"You feel more self-confident.",
	SCHOOL_ENCHANTMENT | SCHOOL_CHARM,
	MANA_ANY
    },

    {
	"chill touch",		{ L_APP },
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"chilling touch",	"You feel less cold.",
	SCHOOL_NECROMANCY,
	MANA_WATER
    },

    {
	"colour spray",		{ L_APP },
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"colour spray",		"!Colour Spray!",
	SCHOOL_ALTERATION,
	MANA_AIR
    },

    {
        "cone of silence",	{ L_APP },
        spell_cone_of_silence,	TAR_IGNORE,		POS_FIGHTING,
        NULL,			35,     12,
        "",                     "!Cone of Silence!",
        SCHOOL_INVOCATION,
        MANA_EARTH
    },

    {
	"continual light",	{ L_APP },
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,			7,	12,
	"",			"!Continual Light!",
	SCHOOL_ALTERATION,
	MANA_FIRE
    },

    {
	"control weather",	{ L_APP },
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,			25,	12,
	"",			"!Control Weather!",
	SCHOOL_ALTERATION,
	MANA_AIR
    },

    {
	"create food",		{ L_APP },
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Create Food!",
	SCHOOL_CONJURATION,
	MANA_EARTH
    },

    {
	"create spring",	{ L_APP },
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	12,
	"",			"!Create Spring!",
	SCHOOL_ALTERATION,
	MANA_WATER
    },

    {
	"create water",		{ L_APP },
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Create Water!",
	SCHOOL_ALTERATION,
	MANA_WATER
    },

    {
	"cure blindness",	{ L_APP },
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			5,	12,
	"",			"!Cure Blindness!",
	SCHOOL_ABJURATION | SCHOOL_NECROMANCY,
	MANA_WATER
    },

    {
	"cure critical",	{ L_APP },
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"",			"!Cure Critical!",
	SCHOOL_NECROMANCY,
	MANA_EARTH
    },

    {
	"cure disease",		{ L_APP },
	spell_cure_disease,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	12,
	"",			"!Cure Disease!",
	SCHOOL_ABJURATION | SCHOOL_NECROMANCY,
	MANA_WATER
    },

    {
	"cure light",		{ L_APP },
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			10,	12,
	"",			"!Cure Light!",
	SCHOOL_NECROMANCY,
	MANA_EARTH
    },

    {
	"cure poison",		{ L_APP },
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"!Cure Poison!",
	SCHOOL_ABJURATION | SCHOOL_NECROMANCY,
	MANA_WATER
    },

    {
	"cure serious",		{ L_APP },
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"",			"!Cure Serious!",
	SCHOOL_NECROMANCY,
	MANA_EARTH
    },

    {
	"curse",		{ L_APP },
	spell_curse,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_curse,		20,	12,
	"curse",		"The curse wears off.",
	SCHOOL_CONJURATION | SCHOOL_SUMMONING,
	MANA_AIR
    },

    {
	"destroy cursed",	{ L_APP },
	spell_destroy_cursed,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	12,
	"",			"!destroy cursed!",
	SCHOOL_NECROMANCY,
	MANA_EARTH
    },

    {
	"detect evil",		{ L_APP },
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"The red in your vision disappears.",
	SCHOOL_DIVINATION,
	MANA_FIRE
    },

    {
	"detect hidden",	{ L_APP },
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less aware of your surroundings.",
	SCHOOL_DIVINATION,
	MANA_FIRE
    },

    {
	"detect invis",		{ L_APP },
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You no longer see invisible objects.",
	SCHOOL_DIVINATION,
	MANA_FIRE
    },

    {
	"detect magic",		{ L_APP },
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING, 
	NULL,			5,	12,
	"",			"The detect magic wears off.",
	SCHOOL_DIVINATION,
	MANA_FIRE
    },

    {
	"detect poison",	{ L_APP },
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Detect Poison!",
	SCHOOL_DIVINATION,
	MANA_FIRE
    },

    {
	"dispel evil",		{ L_APP },
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"holy fire",		"!Dispel Evil!",
	SCHOOL_ABJURATION,
	MANA_WATER
    },

    {
	"dispel magic",		{ L_APP },
	spell_dispel_magic,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			15,	16,
	"",			"!Dispel Magic!",
	SCHOOL_ABJURATION,
	MANA_EARTH
    },

    {
	"earthquake",		{ L_APP },
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			15,	12,
	"earthquake",		"!Earthquake!",
	SCHOOL_ALTERATION,
	MANA_EARTH
    },

    {
	"enchant weapon",	{ L_APP },
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			100,	24,
	"",			"!Enchant Weapon!",
	SCHOOL_ENCHANTMENT,
	MANA_FIRE
    },

    {
	"energy drain",		{ L_APP },
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			35,	12,
	"energy drain",		"!Energy Drain!",
	SCHOOL_NECROMANCY,
	MANA_FIRE
    },

    {
	"exorcise",		{ L_APP },
	spell_exorcise, 	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			35,	12,
	"",		        "!Exorcise!",
	SCHOOL_ALTERATION | SCHOOL_NECROMANCY,
	MANA_WATER
    },

    {
	"faerie fire",		{ L_APP },
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			5,	12,
	"faerie fire",		"The pink aura around you fades away.",
	SCHOOL_ALTERATION,
	MANA_FIRE
    },

    {
	"faerie fog",		{ L_APP },
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,			12,	12,
	"faerie fog",		"!Faerie Fog!",
	SCHOOL_CONJURATION,
	MANA_AIR
    },

    {
	"fireball",		{ L_APP },
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"fireball",		"!Fireball!",
	SCHOOL_EVOCATION,
	MANA_FIRE
    },

    {
	"flamestrike",		{ L_APP },
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"flamestrike",		"!Flamestrike!",
	SCHOOL_CONJURATION,
	MANA_FIRE
    },

    {
	"fly",			{ L_APP },
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			10,	18,
	"",			"You slowly float to the ground.",
	SCHOOL_ALTERATION,
	MANA_AIR
    },

    {
	"gate",			{ L_APP },
	spell_gate,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"",			"!Gate!",
	SCHOOL_CONJURATION | SCHOOL_SUMMONING,
	MANA_EARTH
    },

    {
	"giant strength",	{ L_APP },
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			20,	12,
	"",			"You feel weaker.",
	SCHOOL_ALTERATION,
	MANA_EARTH
    },

    {
	"harm",			{ L_APP },
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			35,	12,
	"divine power",		"!Harm!",
	SCHOOL_NECROMANCY,
	MANA_EARTH
    },

    {
	"heal",			{ L_APP },
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"",			"!Heal!",
	SCHOOL_NECROMANCY,
	MANA_EARTH
    },

    {
	"identify",		{ L_APP },
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			12,	24,
	"",			"!Identify!",
	SCHOOL_DIVINATION,
	MANA_ANY
    },

    {
	"infravision",		{ L_APP },
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	18,
	"",			"You no longer see in the dark.",
	SCHOOL_ILLUSION,
	MANA_FIRE
    },

    {
	"invis",		{ L_APP },
	spell_invis,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_invis,		5,	12,
	"",			"You are no longer invisible.",
	MANA_AIR
    },

    {
	"know alignment",	{ L_APP },
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			9,	12,
	"",			"!Know Alignment!",
	SCHOOL_DIVINATION,
	MANA_FIRE
    },

    {
	"lightning bolt",	{ L_APP },
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"lightning bolt",	"!Lightning Bolt!",
	SCHOOL_EVOCATION,
	MANA_FIRE
    },

    {
	"locate object",	{ L_APP },
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,			20,	18,
	"",			"!Locate Object!",
	SCHOOL_DIVINATION,
	MANA_ANY
    },

    {
	"magic missile",	{ L_APP },
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"magic missile",	"!Magic Missile!",
	SCHOOL_EVOCATION,
	MANA_FIRE
    },

    {
	"mass heal",		{ L_APP },
	spell_mass_heal,	TAR_IGNORE,		POS_STANDING,
	NULL,           	50,	24,
	"",			"!Mass Heal!",
	SCHOOL_NECROMANCY,
	MANA_EARTH
    },

    {
	"mass invis",		{ L_APP },
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	20,	24,
	"",			"You are no longer invisible.",
	MANA_AIR
    },

    {
        "mute",			{ L_APP },
        spell_mute,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
        &gsn_mute,		20,     12,
        "",                     "You are no longer muted.",
	SCHOOL_ABJURATION | SCHOOL_NECROMANCY,
        MANA_WATER
    },

    {
	"pass door",		{ L_APP },
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			20,	12,
	"",			"You feel solid again.",
	SCHOOL_ALTERATION,
	MANA_AIR
    },

    {
	"plague",		{ L_APP },
	spell_plague,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_plague,		20,	12,
	"sickness",		"Your sores vanish.",
	SCHOOL_NECROMANCY,
	MANA_WATER
    },

    {
	"poison",		{ L_APP },
	spell_poison,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_poison,		10,	12,
	"burning blood",	"You feel less sick.",
	SCHOOL_NECROMANCY,
	MANA_WATER
    },

    {
	"polymorph other",	{ L_APP },
	spell_polymorph_other,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			20,	0,
	"",			"Your body feels familiar again.",
	MANA_ANY
    },

    {
	"protection evil",	{ L_APP },
	spell_protection_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected.",
	SCHOOL_ABJURATION,
	MANA_FIRE
    },

    {
	"protection good",	{ L_APP },
	spell_protection_good,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"You feel less protected.",
	SCHOOL_ABJURATION,
	MANA_FIRE
    },

    {                                  
        "recharge item",        { L_APP },
        spell_recharge_item,    TAR_OBJ_INV,            POS_STANDING,
        NULL,                   25,     12,
        "blunder",              "!Recharge Item!",
        SCHOOL_INVOCATION | SCHOOL_ENCHANTMENT,
        MANA_ANY
    }, 

    {
	"refresh",		{ L_APP },
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			12,	18,
	"refresh",		"!Refresh!",
	SCHOOL_ALTERATION,
	MANA_EARTH
    },

    {
        "remove alignment",	{ L_APP },
        spell_remove_alignment,	TAR_OBJ_INV,		POS_STANDING,
        NULL,			10,	12,
        "",                     "!Remove Alignment!",
	SCHOOL_ABJURATION,
        MANA_WATER
    },

    {
	"remove curse",		{ L_APP },
	spell_remove_curse,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			5,	12,
	"",			"!Remove Curse!",
	SCHOOL_ABJURATION,
	MANA_WATER
    },

    {
        "remove silence",	{ L_APP },
        spell_remove_silence,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
        NULL,			15,     12,
        "",                     "!Remove Silence!",
	SCHOOL_ABJURATION | SCHOOL_NECROMANCY,
        MANA_WATER
    },

    {
	"sanctuary",		{ L_APP },
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			75,	12,
	"",			"The white aura around your body fades.",
	SCHOOL_ABJURATION,
	MANA_WATER
    },

    {
	"shield",		{ L_APP },
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			12,	18,
	"",			"Your force shield shimmers then fades away.",
	SCHOOL_CONJURATION,
	MANA_ANY
    },

    {
	"shocking grasp",	{ L_APP },
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"shocking grasp",	"!Shocking Grasp!",
	SCHOOL_EVOCATION,
	MANA_FIRE
    },

    {
	"sleep",		{ L_APP },
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,		15,	12,
	"",			"You feel less tired.",
	SCHOOL_ENCHANTMENT | SCHOOL_CHARM,
	MANA_EARTH
    },

    {
	"stone skin",		{ L_APP },
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			12,	18,
	"",			"Your skin feels soft again.",
	SCHOOL_ALTERATION,
	MANA_EARTH
    },

    {
	"summon",		{ L_APP },
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,			50,	12,
	"",			"!Summon!",
	SCHOOL_CONJURATION | SCHOOL_SUMMONING,
	MANA_AIR
    },

    {
	"teleport",		{ L_APP },
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 		35,	12,
	"",			"!Teleport!",
	SCHOOL_CONJURATION | SCHOOL_SUMMONING,
	MANA_AIR
    },

    {
	"turn undead",		{ L_APP },
	spell_turn_undead,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_turn_undead,	10,	12,
	"divine exorcism",	"!Turn undead!",
	SCHOOL_NECROMANCY,
	MANA_WATER
    },

    {
	"ventriloquate",	{ L_APP },
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			5,	12,
	"",			"!Ventriloquate!",
	SCHOOL_PHANTASM,
	MANA_AIR
    },

    {
	"weaken",		{ L_APP },
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			20,	12,
	"weakening spell",	"You feel stronger.",
	SCHOOL_NECROMANCY,
	MANA_WATER
    },

    {
	"word of recall",	{ L_APP },
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	NULL,			5,	12,
	"",			"!Word of Recall!",
	SCHOOL_CONJURATION | SCHOOL_SUMMONING,
	MANA_AIR
    },

/*
 * Dragon breath.
 */
    {
	"acid breath",		{ L_APP },
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of acid",	"!Acid Breath!",
	SCHOOL_EVOCATION,
	MANA_WATER
    },

    {
	"fire breath",		{ L_APP },
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of flame",	"!Fire Breath!",
	SCHOOL_EVOCATION,
	MANA_FIRE
    },

    {
	"frost breath",		{ L_APP },
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of frost",	"!Frost Breath!",
	SCHOOL_EVOCATION,
	MANA_WATER
    },

    {
	"gas breath",		{ L_APP },
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			50,	12,
	"breath of gas",		"!Gas Breath!",
	SCHOOL_EVOCATION,
	MANA_AIR
    },

    {
	"lightning breath",	{ L_APP },
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			50,	12,
	"breath of lightning",	"!Lightning Breath!",
	SCHOOL_EVOCATION,
	MANA_ANY
    },

/*
 * Fighter and thief skills, as well as magic item skills.
 */
    {
	"backstab",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_backstab,		 0,	24,
	"vicious backstab",	"!Backstab!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"bash door", 	 	{ L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_bash,		 0,	24,
	"powerful bash",	"!Bash Door!",
	SCHOOL_NONE,
	MANA_NONE
    },

    {
        "berserk",              { L_APP },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_berserk,            0,      12,
        "",                     "The bloody haze lifts.",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
        "circle",		{ L_APP },
        spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_circle,		 0,	24,
        "sneak attack",		"!Circle!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"disarm",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_disarm,		 0,	24,
	"",			"!Disarm!",
	SCHOOL_DEFENSIVE,
	MANA_NONE
    },

    {
	"dodge",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dodge,		 0,	 0,
	"",			"!Dodge!",
	SCHOOL_DEFENSIVE,
	MANA_NONE
    },

    {
        "dual",         	{ L_APP },
        spell_null,		TAR_IGNORE,		POS_FIGHTING,
        &gsn_dual,      	 0,	 0,
        "",                     "!Dual!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"enhanced damage",	{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_enhanced_damage,	0,	 0,
	"",			"!Enhanced Damage!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"hide",			{ L_APP },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		 0,	12,
	"",			"!Hide!",
	SCHOOL_STEALTH,
	MANA_NONE
    },

    {
	"kick",			{ L_APP },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_kick,		 0,	 8,
	"mighty kick",		"!Kick!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"parry",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_parry,		 0,	 0,
	"",			"!Parry!",
	SCHOOL_DEFENSIVE,
	MANA_NONE
    },

    {
	"peek",			{ L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		 0,	 0,
	"",			"!Peek!",
	SCHOOL_STEALTH,
	MANA_NONE
    },

    {
	"pick lock",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		 0,	12,
	"",			"!Pick!",
	SCHOOL_NONE,
	MANA_NONE
    },

    {
	"poison weapon",	{ L_APP },
	spell_null,		TAR_OBJ_INV,		POS_STANDING,
	&gsn_poison_weapon,	 0,	12,
	"poisonous concoction",	"!Poison Weapon!",
	SCHOOL_NONE,
	MANA_NONE
    },

    {
	"rescue",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_rescue,		 0,	12,
	"",			"!Rescue!",
	SCHOOL_DEFENSIVE,
	MANA_NONE
    },

    {
	"scrolls",      	{ L_APP },
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_scrolls,	 	 0,	0,
	"blazing scroll",	"!Scrolls!",
	SCHOOL_NONE,
	MANA_NONE
    },

    {
	"second attack",	{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_second_attack,	 0,	 0,
	"",			"!Second Attack!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"snare",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_snare,		 0,	12,
	"",			"You are no longer ensnared.",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"sneak",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		 0,	12,
	"",			NULL,
	SCHOOL_STEALTH,
	MANA_NONE
    },

    {
	"staves",       	{ L_APP },
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_staves,	 	 0,	0,
	"shattered staff",	"!Staves!",
	SCHOOL_NONE,
	MANA_NONE
    },

    {
	"steal",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		 0,	24,
	"",			"!Steal!",
	SCHOOL_STEALTH,
	MANA_NONE
    },

    {
	"third attack",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_third_attack,	 0,	 0,
	"",			"!Third Attack!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
        "untangle",		{ L_APP },
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_untangle,		 0,	24,
        "",                     "!Untangle!",
	SCHOOL_DEFENSIVE,
	MANA_NONE
    },

    {
	"wands",        	{ L_APP },
	spell_null,     	TAR_IGNORE,     	POS_FIGHTING,
	&gsn_wands,	 	 0,	0,
	"exploding wand",	"!Wands!",
	SCHOOL_NONE,
	MANA_NONE
    },

/*
 *  Spells for mega1.are from Glop/Erkenbrand.
 */
    {
        "general purpose",      { L_APP },
	spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   0,      12,
	"general purpose ammo", "!General Purpose Ammo!",
	SCHOOL_NONE,
	MANA_NONE
    },

    {
        "high explosive",       { L_APP },
	spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   0,      12,
	"high explosive ammo",  "!High Explosive Ammo!",
	SCHOOL_NONE,
	MANA_NONE
    },


    /*
     * Psionicist spell/skill definitions
     * Psi class conceived, created, and coded by Thelonius (EnvyMud)
     */

    {
        "adrenaline control",   { L_APP },
        spell_adrenaline_control,       TAR_CHAR_SELF,  POS_STANDING,
        NULL,                           6,      12,
        "",                             "The adrenaline rush wears off.",
	SCHOOL_ALTERATION,
        MANA_WATER
    },

    {
        "agitation",            { L_APP },
        spell_agitation,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   10,     12,
        "molecular agitation",  "!Agitation!",
        SCHOOL_EVOCATION,
        MANA_AIR
    },

    {
        "aura sight",           { L_APP },
        spell_aura_sight,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   9,     12,
        "",                     "!Aura Sight!",
        SCHOOL_DIVINATION,
        MANA_FIRE
    },

    {
        "awe",                  { L_APP },
        spell_awe,              TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
        NULL,                   35,     12,
        "",                     "!Awe!",
        SCHOOL_PHANTASM,
        MANA_WATER
    },

    {
        "ballistic attack",     { L_APP },
        spell_ballistic_attack, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   5,     12,
        "ballistic attack",     "!Ballistic Attack!",
        SCHOOL_EVOCATION,
        MANA_AIR
    },

    {
        "biofeedback",          { L_APP },
        spell_biofeedback,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   75,     12,
        "",                     "Your biofeedback is no longer effective.",
        SCHOOL_ALTERATION,
        MANA_WATER
    },

    {
        "cell adjustment",      { L_APP },
        spell_cell_adjustment,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   8,     12,
        "",                     "!Cell Adjustment!",
        SCHOOL_ALTERATION,
        MANA_EARTH
    },

    {
        "chameleon power",      { L_APP },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_chameleon,         0,     12,
        "",                     "!Chameleon Power!",
	SCHOOL_STEALTH,
        MANA_NONE
    },

    {
        "combat mind",          { L_APP },
        spell_combat_mind,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   15,     12,
        "",                     "Your battle sense has faded.",
	SCHOOL_EVOCATION | SCHOOL_SUMMONING,
        MANA_EARTH
    },

    {
        "complete healing",     { L_APP },
        spell_complete_healing, TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   100,    12,
        "",                     "!Complete Healing!",
	SCHOOL_NECROMANCY,
        MANA_EARTH
    },

    {
        "control flames",       { L_APP },
        spell_control_flames,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   15,     12,
        "tongue of flame",      "!Control Flames!",
        SCHOOL_EVOCATION,
        MANA_FIRE
    },

    {
        "create sound",         { L_APP },
        spell_create_sound,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   5,     12,
        "",                     "!Create Sound!",
        SCHOOL_PHANTASM,
        MANA_AIR
    },

    {
        "death field",          { L_APP },
        spell_death_field,      TAR_IGNORE,             POS_FIGHTING,
        NULL,                   200,    18,
        "field of death",       "!Death Field!",
        SCHOOL_EVOCATION | SCHOOL_NECROMANCY,
        MANA_FIRE
    },

    {
        "detonate",             { L_APP },
        spell_detonate,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   35,     24,
        "detonation",           "!Detonate!",
        SCHOOL_EVOCATION,
        MANA_FIRE
    },

    {
        "disintegrate",         { L_APP },
        spell_disintegrate,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   150,    18,
        "disintegration",       "!Disintegrate!",
        SCHOOL_EVOCATION | SCHOOL_NECROMANCY,
        MANA_FIRE
    },

    {
        "displacement",         { L_APP },
        spell_displacement,     TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   10,     12,
        "",                     "You are no longer displaced.",
        SCHOOL_ALTERATION,
        MANA_AIR
    },

    {
        "domination",           { L_APP },
        spell_domination,       TAR_CHAR_OFFENSIVE,     POS_STANDING,
        &gsn_domination,        5,     12,
        "",                     "You regain control of your body.",
	SCHOOL_ENCHANTMENT | SCHOOL_CHARM,
        MANA_ANY
    },

    {
        "ectoplasmic form",     { L_APP },
        spell_ectoplasmic_form, TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   20,     12,
        "",                     "You feel solid again.",
	SCHOOL_ALTERATION,
        MANA_AIR
    },

    {
        "ego whip",             { L_APP },
        spell_ego_whip,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   20,     12,
        "",                     "You feel more confident.",
        SCHOOL_PHANTASM,
        MANA_WATER
    },

    {
        "energy containment",   { L_APP },
        spell_energy_containment,       TAR_CHAR_SELF,  POS_STANDING,
        NULL,                           10,     12,
        "",                             "You no longer absorb energy.",
        SCHOOL_ALTERATION,
        MANA_EARTH
    },

    {
        "enhance armor",        { L_APP },
        spell_enhance_armor,    TAR_OBJ_INV,    POS_STANDING,
        NULL,                   100,    24,
        "",                     "!Enhance Armor!",
	SCHOOL_ENCHANTMENT,
        MANA_FIRE
    },

    {
        "enhanced strength",    { L_APP },
        spell_enhanced_strength,        TAR_CHAR_SELF,  POS_STANDING,
        NULL,                           20,     12,
        "",                             "You no longer feel so HUGE.",
	SCHOOL_ALTERATION,
        MANA_EARTH
    },

    {
        "flesh armor",          { L_APP },
        spell_flesh_armor,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   15,     12,
        "",                     "Your skin returns to normal.",
	SCHOOL_ALTERATION,
        MANA_EARTH
    },

    {
        "heighten senses",      { L_APP },
        spell_null,             TAR_CHAR_SELF,          POS_STANDING,
        &gsn_heighten,          0,      0,
        "",                     "Your senses return to normal.",
        SCHOOL_DIVINATION,
        MANA_FIRE
    },

    {
        "inertial barrier",     { L_APP },
        spell_inertial_barrier, TAR_IGNORE,             POS_STANDING,
        NULL,                   40,     24,
        "",                     "Your inertial barrier dissipates.",
        SCHOOL_CONJURATION,
        MANA_AIR
    },

    {
        "inflict pain",         { L_APP },
        spell_inflict_pain,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   10,     12,
        "mindpower",            "!Inflict Pain!",
	SCHOOL_EVOCATION,
        MANA_FIRE
    },

    {
        "intellect fortress",   { L_APP },
        spell_intellect_fortress,       TAR_IGNORE,     POS_STANDING,
        NULL,                           25,     24,
        "",                     "Your intellectual fortress crumbles.",
        SCHOOL_EVOCATION,
        MANA_ANY
    },

    {
        "lend health",          { L_APP },
        spell_lend_health,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   10,     12,
        "",                     "!Lend Health!",
        SCHOOL_NECROMANCY,
        MANA_EARTH
    },

    {
        "levitation",           { L_APP },
        spell_levitation,       TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   10,     18,
        "",                     "You slowly float to the ground.",
        SCHOOL_ALTERATION,
        MANA_AIR
    },

    {
        "mental barrier",       { L_APP },
        spell_mental_barrier,   TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   8,     12,
        "",                     "Your mental barrier breaks down.",
	SCHOOL_CONJURATION,
        MANA_EARTH
    },

    {
        "mind thrust",          { L_APP },
        spell_mind_thrust,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   8,     12,
        "mind thrust",          "!Mind Thrust!",
        SCHOOL_PHANTASM,
        MANA_WATER
    },

    {
        "project force",        { L_APP },
        spell_project_force,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   18,     12,
        "projected force",      "!Project Force!",
        SCHOOL_EVOCATION,
        MANA_EARTH
    },

    {
        "psionic blast",        { L_APP },
        spell_psionic_blast,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   25,     12,
        "psionic blast",        "!Psionic Blast!",
        SCHOOL_EVOCATION,
        MANA_FIRE
    },

    {
        "psychic crush",        { L_APP },
        spell_psychic_crush,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   15,     18,
        "psychic crush",        "!Psychic Crush!",
        SCHOOL_EVOCATION,
        MANA_EARTH
    },

    {
        "psychic drain",        { L_APP },
        spell_psychic_drain,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   20,     12,
        "",                     "You no longer feel drained.",
	SCHOOL_NECROMANCY,
        MANA_FIRE
    },

    {
        "psychic healing",      { L_APP },
        spell_psychic_healing,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   20,      12,
        "",                     "!Psychic Healing!",
	SCHOOL_NECROMANCY,
        MANA_EARTH
    },

    {
        "shadow form",          { L_APP },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_shadow,            0,     12,
        "",                     "You no longer move in the shadows.",
	SCHOOL_STEALTH,
        MANA_NONE
    },

    {
        "share strength",       { L_APP },
        spell_share_strength,   TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   8,     12,
        "",                     "You no longer share strength with another.",
	SCHOOL_CONJURATION,
        MANA_EARTH
    },

    {
        "thought shield",       { L_APP },
        spell_thought_shield,   TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   5,     12,
        "",                     "You no longer feel so protected.",
	SCHOOL_CONJURATION,
        MANA_ANY
    },

    {
        "ultrablast",           { L_APP },
        spell_ultrablast,       TAR_IGNORE,             POS_FIGHTING,
        NULL,                   75,     24,
        "ultrablast",           "!Ultrablast!",
        SCHOOL_EVOCATION,
        MANA_FIRE
    },

    {
        "stake",                { L_APP },
        spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_stake,             0,       8,
        "carefully aimed stake","!Stake!",
	SCHOOL_OFFENSIVE,
        MANA_NONE
    },

/*
 * New abilities added by Zen, made by several other ppl.
 */
    {
        "scan",			{ L_APP },
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_scan,		0,       24,
        "",			"!Scan!",
	SCHOOL_STEALTH,
        MANA_NONE
    },

    {
        "shield block",         { L_APP },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_shield_block,      0,      0,
        "",                     "!Shield Block!",
	SCHOOL_DEFENSIVE,
        MANA_NONE
    },

    {
        "fast healing",         { L_APP },
        spell_null,             TAR_IGNORE,             POS_DEAD,
        &gsn_fast_healing,      0,      0,
        "",                     "!Fast Healing!",
        SCHOOL_NONE,
        MANA_NONE
    },

   {
        "fourth attack",        { L_APP },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_fourth_attack,     0,      0,
        "",                     "!Fourth Attack!",
	SCHOOL_OFFENSIVE,
        MANA_NONE
    },

    {
	"brew",			{ L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_brew,		0,	 24,
	"blunder",		"!Brew!",
        SCHOOL_ENCHANTMENT,
        MANA_NONE
    },

    {
	"scribe",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_scribe,		0,	 24,
	"blunder",		"!Scribe!",
        SCHOOL_ENCHANTMENT,
        MANA_NONE
    },

    {
        "track",		{ L_APP },
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_track,		0,       2,
        "",			"!Track!",
	SCHOOL_SURVIVAL,
        MANA_NONE
    },

    {
	"whirlwind",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_whirlwind,		0,	 12,
	"WhirlWind",		"!Whirlwind!",
	SCHOOL_OFFENSIVE,
        MANA_NONE
    },

    {
        "mount",		{ L_APP },
        spell_null,		TAR_IGNORE,		POS_STANDING,
        &gsn_mount,		0,       5,
        "",			"!Mount!",
        SCHOOL_NONE,
        MANA_NONE
    },

/*
 * Spells & skills here by Zen.
 */
    {
        "dirt kicking",		{ L_APP },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dirt,              0,      24,
        "kicked dirt",          "You rub the dirt out of your eyes.",
	SCHOOL_OFFENSIVE,
        MANA_NONE
    },

    {
        "swim",                 { L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_swim,		 0,	 0,
	"",			"!Swim!",
	SCHOOL_SURVIVAL,
	MANA_NONE
    },

    {
        "meditate",         	{ L_APP },
        spell_null,             TAR_IGNORE,             POS_DEAD,
        &gsn_meditate,      	0,      0,
        "",			"!Meditate!",
        SCHOOL_NONE,
        MANA_NONE
    },

    {
        "meteor swarm",		{ L_APP },
        spell_meteor_swarm,	TAR_IGNORE,     POS_FIGHTING,
        NULL,			20,       12,
        "meteor swarm",		"!Meteor Swarm!",
        SCHOOL_EVOCATION,
        MANA_AIR
    },

    {   
	"chain lightning",	{ L_APP },
	spell_chain_lightning,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			25,	12,
	"lightning",		"!Chain Lightning!",
	SCHOOL_EVOCATION,
	MANA_FIRE
    }, 

    {
	"wizard eye",		{ L_APP },
	spell_wizard_eye, 	TAR_IGNORE,		POS_STANDING,
	NULL,			40,	12,
	"",			"!Wizard Eye!",
	SCHOOL_DIVINATION,
	MANA_AIR
    },
    
    {
	"vortex lift",		{ L_APP },
	spell_vortex_lift,	TAR_IGNORE,		POS_STANDING,
	NULL,			60,	12,
	"",			"!Vortex Lift!",
	SCHOOL_CONJURATION | SCHOOL_SUMMONING,
	MANA_AIR
    },
    
    {
	"mass vortex lift",	{ L_APP },
	spell_mass_vortex_lift,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_vortex_lift,	150,	12,
	"",			"!Mass Vortex Lift!",
	SCHOOL_CONJURATION | SCHOOL_SUMMONING,
	MANA_AIR
    },

    {
	"detect good",		{ L_APP },
	spell_detect_good,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			5,	12,
	"",			"The gold in your vision disappears.",
	SCHOOL_DIVINATION,
	MANA_FIRE
    },

    {
	"dispel good",		{ L_APP },
	spell_dispel_good,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			15,	12,
	"unholy fire",		"!Dispel Good!",
	SCHOOL_ABJURATION,
	MANA_FIRE
    },

    {
        "portal",               { L_APP },
        spell_portal,           TAR_IGNORE,             POS_STANDING,
        NULL,                   100,     24,
        "",                     "!Portal!",
        SCHOOL_CONJURATION | SCHOOL_ENCHANTMENT,
        MANA_AIR
    },

    {
        "nexus",                { L_APP },
        spell_nexus,            TAR_IGNORE,             POS_STANDING,
        NULL,                   150,   36,
        "",                     "!Nexus!",
        SCHOOL_CONJURATION | SCHOOL_ENCHANTMENT,
        MANA_AIR
    },

    {
	"create buffet",	{ L_APP },
	spell_create_buffet,	TAR_IGNORE,		POS_STANDING,
	NULL,			33,	12,
	"",			"!Create Buffet!",
	SCHOOL_CONJURATION,
	MANA_EARTH
    },

    {
	"flaming shield",	{ L_APP },
	spell_flame_shield,  	TAR_CHAR_SELF,  	POS_STANDING,
	&gsn_flame_shield,	100,	60,
	"flaming shield",	"The flaming shield around you dies out.",
	SCHOOL_EVOCATION | SCHOOL_ALTERATION,
	MANA_FIRE
    },

    {
	"frost shield",		{ L_APP },
	spell_frost_shield,  	TAR_CHAR_SELF,  	POS_STANDING,
	&gsn_frost_shield,	100,	60,
	"frost shield",		"The frost shield around you melts and evaporates into nothingness.",
	SCHOOL_EVOCATION | SCHOOL_ALTERATION,
	MANA_WATER
    },

    {
	"shock shield",		{ L_APP },
	spell_shock_shield,  	TAR_CHAR_SELF,  	POS_STANDING,
	&gsn_shock_shield,	100,	60,
	"shock shield",		"The torrents of cascading energy suddenly fade away.",
	SCHOOL_EVOCATION | SCHOOL_ALTERATION,
	MANA_AIR
    },

    {
	"ethereal shield",	{ L_APP },
	spell_ethereal_shield, 	TAR_CHAR_SELF,  	POS_STANDING,
	NULL,			50,	60,
	"",			"You are returned to the mundane energy continuum.",
	SCHOOL_ALTERATION,
	MANA_FIRE
    },

/*
 * Weapon proficiencies.
 * These must all be together and after "pugilism".
 */
    {
	"pugilism",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_hit,		0,	0,
	"",			"!Pugilism!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"long blades",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_slash,		0,	0,
	"",			"!Long Blades!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"short blades",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_pierce,		0,	0,
	"",			"!Short blades!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"flexible arms",	{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_whip,		0,	0,
	"",			"!Flexible arms!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"blast weapons",	{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_explode,		0,	0,
	"",			"!Blast Weapons!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"bludgeons",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_pound,		0,	0,
	"",			"!Bludgeons!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"tornado suction",	{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_suction,		0,	0,
	"",			"!Tornado Suction!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

    {
	"marksmanship",		{ L_APP },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_shot,		0,	0,
	"",			"!Marksmanship!",
	SCHOOL_OFFENSIVE,
	MANA_NONE
    },

	  /* Race ability spells */
    {
        "vampiric bite",        { L_APP },
        spell_vampiric_bite,    TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
        &gsn_vampiric_bite,     0,     0,
        "vampiric bite",        "You feel well fed.",
	SCHOOL_NONE,
	MANA_NONE
    }

};
