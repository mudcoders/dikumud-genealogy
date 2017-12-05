/* Item wear locations */

const char * str_wear_type [] =
{
	"take",		/* 1 */
	"finger",	/* 2 */
	"neck",		/* 4 */
	"body",		/* 8 */
	"head",		/* 16 */
	"legs",		/* 32 */
	"feet",		/* 64 */
	"hands",	/* 128 */
	"arms",		/* 256 */
	"shield",	/* 512 */
	"about",	/* 1024 */
	"waist",	/* 2048 */
	"wrist",	/* 4096 */
	"wield",	/* 8192 */
	"hold"		/* 16384 */
};

const char * str_item_type [] =
{
	"light",		/* 1 */
	"scroll",
	"wand",
	"staff",
	"weapon",		/* 5 */
	"unknown",		/* unused 6 */
	"unknown",		/* unused 7 */
	"treasure",		/* 8 */
	"armor",
	"potion",
	"unknown"		/* unused 11 */
	"furniture",		/* 12 */
	"trash",
	"unknown",		/* unused 14 */
	"container",
	"unknown",		/* unused 16 */
	"drink-container",
	"key",
	"food",
	"money",		/* 20 */
	"unknown",		/* unused 21 */
	"boat",
	"NPC corpse",
	"PC corpse",
	"fountain",
	"pill",
	"jukebox"		/* 27 */
}

const char * str_mob_act [] =
{
	"is_npc",		/* 1 */
	"sentinel",		/* 2 */
	"scavenger",		/* 4 */
	"is_healer",		/* 8 */
	"unknown",		/* unused 16 */
	"aggresive",		/* 32 */
	"stay_area",		/* 64 */
	"wimpy",		/* 128 */
	"pet",			/* 256 */
	"train",		/* 512 */
	"practice",		/* 1024 */
	"gamble",		/* 2048 */
	"banker",		/* 4096 */
	"teacher",		/* 8092 */
	"unknown",		/* 16384 */
	"unknown",		/* 32768 */
	"unknown",		/* 65536 */
	"**MOVED**"		/* 131072 */	/* Don't ever set !! */
}

const char * str_obj_extra [] =
{
	"glowing",		/* 1 */
	"humming",		/* 2 */
	"dark",			/* 4 */
	"locked",
	"evil",
	"invisible",		/* 32 */
	"no_drop",
	"blessed",
	"anti_good",		/* 256 */
	"anti_evil",
	"anti_neutral",
	"no_remove",
	"inventory",		/* 8092 */
	"poisoned",
	"vampire_bane",
	"holy",			/* 65536 */
	"blade_thirst",
	"melt_drop",
	"unknown",		/* 524288 */
	"no_purge",		/* 1048576 */
	"hidden"		/* 2097152 */
}

const char * str_cont_lock [] =
{
	"closeable",		/* 1 */
	"pickproof",		/* 2 */
	"closed",		/* 4 */
	"locked"		/* 8 */
}

const char * str_room_flag [] =
{
	"dark",			/* 1 */
	"unknown",		/* 2 */
	"no_mob",		/* 4 */
	"indoors",		/* 8 */
	"underground",		/* 16 */
	"arena",		/* 32 */
	"unknown",		/* 64 */
	"unknown",		/* 128 */
	"unknown",		/* 256 */
	"private",		/* 512 */
	"safe",			/* 1024 */
	"solitary",		/* 2048 */
	"pet_shop",		/* 4096 */
	"no_recall",		/* 8092 */
	"cone_of_silence"	/* 16384 */
}

const char * str_exit_flag [] =
{
	"is_door",		/* 1 */
	"closed",		/* 2 */
	"locked",		/* 4 */
	"bashed",		/* 8 */
	"bashproof",		/* 16 */
	"pickproof",		/* 32 */
	"passproof"		/* 64 */
}

const char * str_sector_type [] =
{
	"inside",		/* 0 */	/* is this needed */
	"city",			/* 1 */
	"field",
	"forest",
	"hills",
	"mountain",		/* 5 */
	"water_swim",
	"water_noswim",
	"underwater",
	"air",
	"desert",		/* 10 */
	"**MAX**"		/* 11 */
}

/* also in act_info.c table where_name */
const char * str_wear_loc [] =
{
	"none",			/* -1 */
	"light",		/* 0 */
	"finger_l",		/* 1 */
	"finger_r",		/* 2 */
        "neck_1",		/* 3 */
	"neck_2",
	"body",			/* 5 */
	"head",
	"legs",
	"feet",
	"hands",
	"arms",			/* 10 */
	"shield",
	"about",
	"waist",
	"wrist_l",
	"wrist_r",		/* 15 */
	"wield",
	"hold",
	"wield_2",
	"**MAX_WEAR**"		/* 19 */
};

const char * str_char_pos [] =
{
	"dead",			/* 0 */
	"mortally wounded",	/* 1 */
	"incapacitated",
	"stunned",
	"sleeping",
	"resting",		/* 5 */
	"fighting",
	"standing"		/* 7 */
}

const char * str_play_act [] =
{
	"is_npc"		/* 1 */		/* Don't EVER set.	*/
	"bought_pet",		/* 2 */
	"register",		/* 4 */
	"autoexit",		/* 8 */
	"autoloot",		/* 16 */
	"autosac",		/* 32 */
	"blank",		/* 64 */
	"brief",		/* 128 */
	"combine",		/* 256 */
	"prompt",		/* 512 */
	"telnet_ga",		/* 1024 */
	"holylight",		/* 2048 */
	"wizinvis",		/* 4096 */
	"holylight",		/* 8192 */
	"wizbit",		/* 16384 */
	"silence",		/* 32768 */
	"no_emote",		/* 65536 */
	"moved",		/* 131072 */	/* Don't ever set	*/
	"no_tell",		/* 262144 */
	"log",			/* 524288 */
	"deny",			/* 1048576 */
	"freeze",		/* 2097152 */
	"thief",		/* 4194304 */
	"killer",		/* 8388608 */
	"autogold",		/* 16777216 */
	"afk"			/* 33554432 */
}

const char * str_chan_name [] =
{
	"auction",		/* 1 */
	"chat",			/* 2 */
	"unknown",		/* 4 */
	"immtalk",		/* 8 */
	"music",		/* 16 */
	"question",		/* 32 */
	"shout",		/* 64 */
	"yell",			/* 128 */
	"news",			/* 256 */
	"wiznews"		/* 512 */
}

const char * str_class_name [] =
{
	"mage",			/* 0 */
	"cleric",		/* 1 */
	"thief",		/* 2 */
	"warrior",		/* 3 */
	"psionicist",		/* 4 */
	"necromancer",		/* 5 */
	"druid",		/* 6 */
	"ranger",		/* 7 */
	"NEW_CLASS"		/* 8 */
}

/* no use for this one ??? also in const.c race_table field 1 */
const char * str_race_name [] =
{
	"human",
	"elf",
	"half-elf",
	"drow",
	"dwarf",
	"half-dwarf",
	"hobbit",
	"giant",
	"ogre",
	"orc",
	"kobold",
	"minotaur",
	"troll",
	"hobgoblin",
	"insect",
	"dragon",
	"animal",
	"GOD",
	"undead",
	"harpy",
	"bear",
	"githyanki",
	"elemental",
	"bat",
	"plant",
	"rat",
	"vampire",
	"werewolf",
	"goblin",
	"faerie",
	"arachnid",
	"mindflayer",
	"object",
	"mist",
	"snake",
	"worm",
	"fish",
	"hydra",
	"lizard",
	"gnome",
	"halfkobold"
}

/* day_name and month_name are from act_info.c */
char *  const   day_name        [] =
{
	"the Moon",
	"the Bull",
	"Deception",
	"Thunder",
	"Freedom",
	"the Great Gods",
	"the Sun"
};

char *  const   month_name      [] =
{
	"Winter",
	"the Winter Wolf",
	"the Frost Giant",
	"the Old Forces",
	"the Grand Struggle",
	"the Spring",
	"Nature",
	"Futility",
	"the Dragon",
	"the Sun",
	"the Heat",
	"the Battle",
	"the Dark Shades",
	"the Shadows",
	"the Long Shadows",
	"the Ancient Darkness",
	"the Great Evil"
};
















































