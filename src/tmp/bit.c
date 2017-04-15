/***************************************************************************
 *  File: bit.c                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was written by Jason Dinkel and inspired by Russ Taylor,     *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/
/*
 The code below uses a table lookup system that is based on suggestions
 from Russ Taylor.  There are many routines in handler.c that would benefit
 with the use of tables.  You may consider simplifying your code base by
 implementing a system like below with such functions. -Jason Dinkel
*/


#define unix 1
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



struct flag_stat_type
{
    const struct flag_type *structure;
    bool stat;
};



/*****************************************************************************
 Name:		flag_stat_table
 Purpose:	This table catagorizes the tables following the lookup
 		functions below into stats and flags.  Flags can be toggled
 		but stats can only be assigned.  Update this table when a
 		new set of flags is installed.
 ****************************************************************************/
const struct flag_stat_type flag_stat_table[] =
{
/*  {	structure		stat	}, */
    {	area_flags,		FALSE	},
    {   sex_flags,		TRUE	},
    {   exit_flags,		FALSE	},
    {   door_resets,		TRUE	},
    {   room_flags,		FALSE	},
    {   sector_flags,		TRUE	},
    {   type_flags,		TRUE	},
    {   extra_flags,		FALSE	},
    {   anti_race_flags,	FALSE   },
    {   anti_class_flags, 	FALSE   }, 
    {   wear_flags,		FALSE	},
    {   act_flags,		FALSE	},
    {   affect_flags,		FALSE	},
    {   affect2_flags,          FALSE   },
    {   apply_flags,		TRUE	},
    {   wear_loc_flags,		TRUE	},
    {   wear_loc_strings,	TRUE	},
    {   weapon_flags,		TRUE	},
    {   container_flags,	FALSE	},
    {   liquid_flags,		TRUE	},
    {   immune_flags,           FALSE   },
    {   mprog_types,            TRUE    },
    {   oprog_types,            FALSE   },
    {   rprog_types,            FALSE   },
    {   eprog_types,            FALSE   },

    {   0,			0	}
};
    


/*****************************************************************************
 Name:		is_stat( table )
 Purpose:	Returns TRUE if the table is a stat table and FALSE if flag.
 Called by:	flag_value and flag_string.
 Note:		This function is local and used only in bit.c.
 ****************************************************************************/
bool is_stat( const struct flag_type *flag_table )
{
    int flag;

    for (flag = 0; flag_stat_table[flag].structure; flag++)
    {
	if ( flag_stat_table[flag].structure == flag_table
	  && flag_stat_table[flag].stat )
	    return TRUE;
    }
    return FALSE;
}



/*
 * This function is Russ Taylor's creation.  Thanks Russ!
 * All code copyright (C) Russ Taylor, permission to use and/or distribute
 * has NOT been granted.  Use only in this OLC package has been granted.
 */
/*****************************************************************************
 Name:		flag_lookup( flag, table )
 Purpose:	Returns the value of a single, settable flag from the table.
 Called by:	flag_value and flag_string.
 Note:		This function is local and used only in bit.c.
 ****************************************************************************/
int flag_lookup(const char *name, const struct flag_type *flag_table)
{
    int flag;
 
    for (flag = 0; *flag_table[flag].name; flag++)	/* OLC 1.1b */
    {
        if ( !str_cmp( name, flag_table[flag].name )
          && flag_table[flag].settable )
            return flag_table[flag].bit;
    }
 
    return NO_FLAG;
}



/*****************************************************************************
 Name:		flag_value( table, flag )
 Purpose:	Returns the value of the flags entered.  Multi-flags accepted.
 Called by:	olc.c and olc_act.c.
 ****************************************************************************/
int flag_value( const struct flag_type *flag_table, char *argument)
{
    char word[MAX_INPUT_LENGTH];
    int  bit;
    int  marked = 0;
    bool found = FALSE;

    if ( is_stat( flag_table ) )
    {
	one_argument( argument, word );

	if ( ( bit = flag_lookup( word, flag_table ) ) != NO_FLAG )
	    return bit;
	else
	    return NO_FLAG;
    }

    /*
     * Accept multiple flags.
     */
    for (; ;)
    {
        argument = one_argument( argument, word );

        if ( word[0] == '\0' )
	    break;

        if ( ( bit = flag_lookup( word, flag_table ) ) != NO_FLAG )
        {
            SET_BIT( marked, bit );
            found = TRUE;
        }
    }

    if ( found )
	return marked;
    else
	return NO_FLAG;
}



/*****************************************************************************
 Name:		flag_string( table, flags/stat )
 Purpose:	Returns string with name(s) of the flags or stat entered.
 Called by:	act_olc.c, olc.c, and olc_save.c.
 ****************************************************************************/
char *flag_string( const struct flag_type *flag_table, int bits )
{
    static char buf[512];
    int  flag;

    buf[0] = '\0';

    for (flag = 0; *flag_table[flag].name; flag++)	/* OLC 1.1b */
    {
	if ( !is_stat( flag_table ) && IS_SET(bits, flag_table[flag].bit) )
	{
	    strcat( buf, " " );
	    strcat( buf, flag_table[flag].name );
	}
	else
	if ( flag_table[flag].bit == bits )
	{
	    strcat( buf, " " );
	    strcat( buf, flag_table[flag].name );
	    break;
	}
    }
    return (buf[0] != '\0') ? buf+1 : "none";
}



const struct flag_type area_flags[] =
{
    {	"none",			AREA_NONE,		FALSE	},
    {	"changed",		AREA_CHANGED,		FALSE	},
    {	"added",		AREA_ADDED,		FALSE	},
    {	"loading",		AREA_LOADING,		FALSE	},
    {	"verbose",		AREA_VERBOSE,		FALSE	},
    {   "prototype",            AREA_PROTOTYPE,         FALSE   },
    {   "clan_hq",		AREA_CLAN_HQ,		FALSE	},
    {   "noquest",		AREA_NO_QUEST,		FALSE   },
    {   "mudschool",		AREA_MUDSCHOOL,		FALSE   },
    {	"",			0,			0	}
};



const struct flag_type sex_flags[] =
{
    {	"male",			SEX_MALE,		TRUE	},
    {	"female",		SEX_FEMALE,		TRUE	},
    {	"neutral",		SEX_NEUTRAL,		TRUE	},
    {	"",			0,			0	}
};



const struct flag_type exit_flags[] =
{
    {   "door",			EX_ISDOOR,		TRUE    },
    {	"closed",		EX_CLOSED,		TRUE	},
    {	"locked",		EX_LOCKED,		TRUE	},
    {	"bashed",		EX_BASHED,		FALSE	},
    {	"bashproof",		EX_BASHPROOF,		TRUE	},
    {	"pickproof",		EX_PICKPROOF,		TRUE	},
    {	"passproof",		EX_PASSPROOF,		TRUE	},
    {   "random",		EX_RANDOM,              TRUE    },
    {   "magiclock",            EX_MAGICLOCK,           TRUE    },
    {	"",			0,			0	}
};



const struct flag_type door_resets[] =
{
    {	"open and unlocked",	0,		TRUE	},
    {	"closed and unlocked",	1,		TRUE	},
    {	"closed and locked",	2,		TRUE	},
    {	"",			0,		0	}
};



const struct flag_type room_flags[] =
{
    {	"dark",			ROOM_DARK,		TRUE	},
    {	"no_mob",		ROOM_NO_MOB,		TRUE	},
    {	"indoors",		ROOM_INDOORS,		TRUE	},
    {	"private",		ROOM_PRIVATE,		TRUE	},
    {	"safe",			ROOM_SAFE,		TRUE	},
    {	"solitary",		ROOM_SOLITARY,		TRUE	},
    {	"pet_shop",		ROOM_PET_SHOP,		TRUE	},
    {	"no_recall",		ROOM_NO_RECALL,		TRUE	},
    {	"cone_of_silence",	ROOM_CONE_OF_SILENCE,	TRUE	},
    {   "no_in",                ROOM_NO_ASTRAL_IN,      TRUE    },
    {   "no_out",               ROOM_NO_ASTRAL_OUT,     TRUE    },
    {   "tele_area",            ROOM_TELEPORT_AREA,     TRUE    },
    {   "tele_world",           ROOM_TELEPORT_WORLD,    TRUE    },
    {   "no_magic",             ROOM_NO_MAGIC,          TRUE    },
    {   "no_offensive",         ROOM_NO_OFFENSIVE,      TRUE    },
    {   "no_flee",              ROOM_NO_FLEE,           TRUE    },
    {   "silent",               ROOM_SILENT,            TRUE    },
    {   "bank",		        ROOM_BANK,              TRUE    },
    {   "nofloor",              ROOM_NOFLOOR,           TRUE    },
    {   "smithy",               ROOM_SMITHY,            TRUE    },
    {   "noscry",               ROOM_NOSCRY,            TRUE    },
    {   "damage",               ROOM_DAMAGE,            TRUE    },
    {   "pkill",                ROOM_PKILL,             TRUE    },
    {   "no_shadow",		ROOM_NO_SHADOW,		TRUE	},
    {	"",			0,			0	}
};



const struct flag_type sector_flags[] =
{
    {	"inside",	SECT_INSIDE,		TRUE	},
    {	"city",		SECT_CITY,		TRUE	},
    {	"field",	SECT_FIELD,		TRUE	},
    {	"forest",	SECT_FOREST,		TRUE	},
    {	"hills",	SECT_HILLS,		TRUE	},
    {	"mountain",	SECT_MOUNTAIN,		TRUE	},
    {	"swim",		SECT_WATER_SWIM,	TRUE	},
    {	"noswim",	SECT_WATER_NOSWIM,	TRUE	},
    {	"underwater",	SECT_UNDERWATER,	TRUE	},
    {	"air",		SECT_AIR,		TRUE	},
    {	"desert",	SECT_DESERT,		TRUE	},
    {   "badland",      SECT_BADLAND,           TRUE    },
    {	"",		0,			0	}
};



const struct flag_type type_flags[] =
{
    {	"light",		ITEM_LIGHT,		TRUE	},
    {	"scroll",		ITEM_SCROLL,		TRUE	},
    {	"wand",			ITEM_WAND,		TRUE	},
    {	"staff",		ITEM_STAFF,		TRUE	},
    {	"weapon",		ITEM_WEAPON,		TRUE	},
    {	"treasure",		ITEM_TREASURE,		TRUE	},
    {	"armor",		ITEM_ARMOR,		TRUE	},
    {	"potion",		ITEM_POTION,		TRUE	},
    {   "noteboard",            ITEM_NOTEBOARD,         TRUE    },
    {	"furniture",		ITEM_FURNITURE,		TRUE	},
    {	"trash",		ITEM_TRASH,		TRUE	},
    {	"container",		ITEM_CONTAINER,		TRUE	},
    {	"drink-container",	ITEM_DRINK_CON,		TRUE	},
    {   "blood",                ITEM_BLOOD,             TRUE    },
    {	"key",			ITEM_KEY,		TRUE	},
    {	"food",			ITEM_FOOD,		TRUE	},
    {	"money",		ITEM_MONEY,		TRUE	},
    {	"boat",			ITEM_BOAT,		TRUE	},
    {	"npc corpse",		ITEM_CORPSE_NPC,	TRUE	},
    {	"pc corpse",		ITEM_CORPSE_PC,		FALSE	},
    {	"fountain",		ITEM_FOUNTAIN,		TRUE	},
    {	"pill",			ITEM_PILL,		TRUE	},
    {   "contact",              ITEM_LENSE,             TRUE    },
    {   "portal",               ITEM_PORTAL,            TRUE    },
    {   "doll",                 ITEM_VODOO,             TRUE    },
    {   "berry",                ITEM_BERRY,             TRUE    },
    {	"",			0,			0	}
};


const struct flag_type extra_flags[] =
{
    {	"glow",			ITEM_GLOW,		TRUE	},
    {	"hum",			ITEM_HUM,		TRUE	},
    {	"dark",			ITEM_DARK,		TRUE	},
    {	"lock",			ITEM_LOCK,		TRUE	},
    {	"evil",			ITEM_EVIL,		TRUE	},
    {	"invis",		ITEM_INVIS,		TRUE	},
    {	"magic",		ITEM_MAGIC,		TRUE	},
    {	"nodrop",		ITEM_NODROP,		TRUE	},
    {	"bless",		ITEM_BLESS,		TRUE	},
    {	"anti-good",		ITEM_ANTI_GOOD,		TRUE	},
    {	"anti-evil",		ITEM_ANTI_EVIL,		TRUE	},
    {	"anti-neutral",		ITEM_ANTI_NEUTRAL,	TRUE	},
    {	"noremove",		ITEM_NOREMOVE,		TRUE	},
    {	"inventory",		ITEM_INVENTORY,		TRUE	},
    {	"poisoned",		ITEM_POISONED,		TRUE	},
    {   "dwarven",		ITEM_DWARVEN,		TRUE	},
    {   "nolocate",             ITEM_NO_LOCATE,	        TRUE    },
    {   "nodamage",             ITEM_NO_DAMAGE,         TRUE    },
    {   "flame",                ITEM_FLAME,             TRUE    },
    {   "chaos",                ITEM_CHAOS,             TRUE    },
    {   "patched",              ITEM_PATCHED,           TRUE    },
    {	"",			0,			0	}
};
/* FOR NEW FLAGS */
const struct flag_type anti_class_flags[] =
{
    {   "anti-mage",            ITEM_ANTI_MAGE,         TRUE    },
    {   "anti-paladin",         ITEM_ANTI_PALADIN,      TRUE    },
    {   "anti-cleric",          ITEM_ANTI_CLERIC,       TRUE    },
    {   "anti-thief",           ITEM_ANTI_THIEF,        TRUE    },
    {   "anti-warrior",         ITEM_ANTI_WARRIOR,      TRUE    },
    {   "anti-psi",             ITEM_ANTI_PSI,          TRUE    },
    {   "anti-druid",           ITEM_ANTI_DRUID,        TRUE    },
    {   "anti-ranger",          ITEM_ANTI_RANGER,       TRUE    },
    {   "anti-bard",            ITEM_ANTI_BARD,         TRUE    },
    {   "anti-vamp",            ITEM_ANTI_VAMP,         TRUE    },
    {   "anti-necro",           ITEM_ANTI_NECRO,        TRUE    },
    {   "anti-werewolf",        ITEM_ANTI_WWF,     	TRUE    },
    {   "anti-monk",		ITEM_ANTI_MONK,		TRUE	},
    {	"",			0,			0	}
};
const struct flag_type anti_race_flags[] =
{
    {   "anti-human",		ITEM_ANTI_HUMAN,	TRUE	},
    {	"anti-elf",		ITEM_ANTI_ELF,		TRUE	},
    {	"anti-dwarf",		ITEM_ANTI_DWARF,	TRUE	},
    {	"anti-pixie",		ITEM_ANTI_PIXIE,	TRUE	},
    {	"anti-halfling",	ITEM_ANTI_HALFLING,	TRUE	},
    {	"anti-drow",		ITEM_ANTI_DROW,		TRUE	},
    {	"anti-elder",		ITEM_ANTI_ELDER,	TRUE	},
    {	"anti-ogre",		ITEM_ANTI_OGRE,		TRUE	},
    {	"anti-lizardman",	ITEM_ANTI_LIZARDMAN,	TRUE	},
    {	"anti-demon",		ITEM_ANTI_DEMON,	TRUE	},
    {	"anti-ghoul",		ITEM_ANTI_GHOUL,	TRUE	},
    {	"anti-illithid",	ITEM_ANTI_ILLITHID,	TRUE	},
    {	"anti-minotaur",	ITEM_ANTI_MINOTAUR,	TRUE	},
    {   "anti-troll",           ITEM_ANTI_TROLL,        TRUE    },
    {   "anti-shadow",          ITEM_ANTI_SHADOW,       TRUE    },
    {   "anti-tabaxi",		ITEM_ANTI_TABAXI,	TRUE	},
    {	"",			0,			0	}
};


const struct flag_type wear_flags[] =
{
    {	"take",			ITEM_TAKE,		TRUE	},
    {	"finger",		ITEM_WEAR_FINGER,	TRUE	},
    {	"neck",			ITEM_WEAR_NECK,		TRUE	},
    {	"body",			ITEM_WEAR_BODY,		TRUE	},
    {	"head",			ITEM_WEAR_HEAD,		TRUE	},
    {	"legs",			ITEM_WEAR_LEGS,		TRUE	},
    {	"feet",			ITEM_WEAR_FEET,		TRUE	},
    {	"hands",		ITEM_WEAR_HANDS,	TRUE	},
    {	"arms",			ITEM_WEAR_ARMS,		TRUE	},
    {	"shield",		ITEM_WEAR_SHIELD,	TRUE	},
    {	"about",		ITEM_WEAR_ABOUT,	TRUE	},
    {	"waist",		ITEM_WEAR_WAIST,	TRUE	},
    {	"wrist",		ITEM_WEAR_WRIST,	TRUE	},
    {	"wield",		ITEM_WIELD,		TRUE	},
    {	"hold",			ITEM_HOLD,		TRUE	},
    {   "lense",                ITEM_WEAR_CONTACT,      TRUE    },
    {   "orbit",                ITEM_WEAR_ORBIT,        TRUE    },
    {   "mask",                 ITEM_WEAR_FACE,         TRUE    },
    {   "ears",                 ITEM_WEAR_EARS,         TRUE    },
    {   "ankle",                ITEM_WEAR_ANKLE,        TRUE    },
    {	"",			0,			0	}
};



const struct flag_type act_flags[] =
{
    {	"npc",			ACT_IS_NPC,		FALSE	},
    {	"sentinel",		ACT_SENTINEL,		TRUE	},
    {	"scavenger",		ACT_SCAVENGER,		TRUE	},
    {	"aggressive",		ACT_AGGRESSIVE,		TRUE	},
    {	"stay_area",		ACT_STAY_AREA,		TRUE	},
    {	"wimpy",		ACT_WIMPY,		TRUE	},
    {	"pet",			ACT_PET,		TRUE	},
    {	"train",		ACT_TRAIN,		TRUE	},
    {	"practice",		ACT_PRACTICE,		TRUE	},
    {	"gamble",		ACT_GAMBLE,		TRUE	},
    {   "undead",               ACT_UNDEAD,             TRUE    },
    {   "track",                ACT_TRACK,              TRUE    },
    {   "quest",                ACT_QUESTMASTER,        TRUE    },
    {	"postman",		ACT_POSTMAN,		TRUE	},
    {	"nopush",		ACT_NOPUSH,		TRUE	},
    {	"nodrag",		ACT_NODRAG,		TRUE	},
    {   "noshadow",		ACT_NOSHADOW,		TRUE    },
    {   "noastral",		ACT_NOASTRAL,		TRUE	},
    {   "newbie",		ACT_NEWBIE,		TRUE	},
    {	"",			0,			0	}
};



const struct flag_type affect_flags[] =
{
    {	"blind",		AFF_BLIND,		TRUE	},
    {	"invisible",		AFF_INVISIBLE,		TRUE	},
    {	"detect-evil",		AFF_DETECT_EVIL,	TRUE	},
    {	"detect-invis",		AFF_DETECT_INVIS,	TRUE	},
    {	"detect-magic",		AFF_DETECT_MAGIC,	TRUE	},
    {	"detect-hidden",	AFF_DETECT_HIDDEN,	TRUE	},
    {	"haste",		AFF_HASTE,		TRUE	},
    {	"sanctuary",		AFF_SANCTUARY,		TRUE	},
    {   "fireshield",           AFF_FIRESHIELD,         TRUE    },
    {   "shockshield",          AFF_SHOCKSHIELD,        TRUE    },
    {   "iceshield",            AFF_ICESHIELD,          TRUE    },
    {   "chaos-field",          AFF_CHAOS,              TRUE    },
    {	"vibrating",		AFF_VIBRATING,		TRUE	},
    {	"faerie-fire",		AFF_FAERIE_FIRE,	TRUE	},
    {	"infrared",		AFF_INFRARED,		TRUE	},
    {	"curse",		AFF_CURSE,		TRUE	},
    {	"flaming",		AFF_FLAMING,		FALSE	},
    {	"poison",		AFF_POISON,		TRUE	},
    {	"protect",		AFF_PROTECT,		TRUE	},
    {	"sneak",		AFF_SNEAK,		TRUE	},
    {	"hide",			AFF_HIDE,		TRUE	},
    {	"sleep",		AFF_SLEEP,		TRUE	},
    {	"charm",		AFF_CHARM,		TRUE	},
    {	"flying",		AFF_FLYING,		TRUE	},
    {	"pass-door",		AFF_PASS_DOOR,		TRUE	},
    {	"stunned",		AFF_STUN,		TRUE	},
    {	"summoned",		AFF_SUMMONED,		TRUE	},
    {	"mute",			AFF_MUTE,		TRUE	},
    {	"aura-of-peace",	AFF_PEACE,		TRUE	},
 /*   {   "no-charm",             AFF_NO_CHARM,           TRUE    }, */
    {	"",			0,			0	}
};

const struct flag_type affect2_flags [] =
{
    {   "mental-block",         AFF_NOASTRAL,           TRUE    },
    {   "true-sight",           AFF_TRUESIGHT,          TRUE    },
    {   "blade-barrier",        AFF_BLADE,              TRUE    },
    {   "detect-good",          AFF_DETECT_GOOD,        TRUE    },
    {   "protect-good",         AFF_PROTECTION_GOOD,    TRUE    },
    {   "berserk",              AFF_BERSERK,            FALSE   },
    {   "field-decay",		AFF_FIELD,		TRUE    },
    {   "rage",			AFF_RAGE,		TRUE	},
    {   "adrenaline-rush",	AFF_RUSH,		TRUE	},
    {   "inertial",		AFF_INERTIAL,		TRUE	},
    {   "golden-aura",		AFF_GOLDEN,		FALSE	},
    {   "",                     0,                      0       }
};


/*
 * Used when adding an affect to tell where it goes.
 * See addaffect and delaffect in act_olc.c
 */
const struct flag_type apply_flags[] =
{
    {	"none",			APPLY_NONE,		TRUE	},
    {	"strength",		APPLY_STR,		TRUE	},
    {	"dexterity",		APPLY_DEX,		TRUE	},
    {	"intelligence",		APPLY_INT,		TRUE	},
    {	"wisdom",		APPLY_WIS,		TRUE	},
    {	"constitution",		APPLY_CON,		TRUE	},
    {	"sex",			APPLY_SEX,		TRUE	},
    {	"class",		APPLY_CLASS,		TRUE	},
    {	"level",		APPLY_LEVEL,		TRUE	},
    {	"age",			APPLY_AGE,		TRUE	},
    {	"height",		APPLY_HEIGHT,		TRUE	},
    {	"weight",		APPLY_WEIGHT,		TRUE	},
    {	"mana",			APPLY_MANA,		TRUE	},
    {   "blood",                APPLY_BP,               TRUE    },
    {   "anti-disarm",          APPLY_ANTI_DIS,         TRUE    },
    {	"hp",			APPLY_HIT,		TRUE	},
    {	"move",			APPLY_MOVE,		TRUE	},
    {	"gold",			APPLY_GOLD,		TRUE	},
    {	"experience",		APPLY_EXP,		TRUE	},
    {	"ac",			APPLY_AC,		TRUE	},
    {	"hitroll",		APPLY_HITROLL,		TRUE	},
    {	"damroll",		APPLY_DAMROLL,		TRUE	},
    {	"saving-para",		APPLY_SAVING_PARA,	TRUE	},
    {	"saving-rod",		APPLY_SAVING_ROD,	TRUE	},
    {	"saving-petri",		APPLY_SAVING_PETRI,	TRUE	},
    {	"saving-breath",	APPLY_SAVING_BREATH,	TRUE	},
    {	"saving-spell",		APPLY_SAVING_SPELL,	TRUE	},
    {	"invis",		APPLY_INVISIBLE,	TRUE	},
    {	"detect-invis",		APPLY_DETECT_INVIS,	TRUE	},
    {	"hide",	        	APPLY_HIDE,     	TRUE	},
    {	"sneak",		APPLY_SNEAK,		TRUE	},
    {	"scry",	        	APPLY_SCRY,		TRUE	},
    {	"detect-hide",	 	APPLY_DETECT_HIDDEN,	TRUE	},
    {	"detect-magic",		APPLY_DETECT_MAGIC,	TRUE	},
    {	"detect-evil",		APPLY_DETECT_EVIL,	TRUE	},
    {	"protect",		APPLY_PROTECT,		TRUE	},
    {	"fly",   		APPLY_FLYING,		TRUE	},
    {	"infrared",		APPLY_INFRARED,		TRUE	},
    {	"sanctuary",		APPLY_SANCTUARY,	TRUE	},
    {	"pass-door",		APPLY_PASS_DOOR,	TRUE	},
    {	"haste",		APPLY_HASTE,		TRUE	},
    {	"bless",		APPLY_BLESS,		TRUE	},
    {	"poison",		APPLY_POISON,		TRUE	},
    {	"faerie-fire",		APPLY_FAERIE_FIRE,	TRUE	},
    {	"giant-str",		APPLY_GIANT_STRENGTH,	TRUE	},
    {   "heighten-senses",      APPLY_HEIGHTEN_SENSES,  TRUE    },
    {   "combat-mind",		APPLY_COMBAT_MIND,      TRUE    },
    {   "bio-acceleration",	APPLY_BIO_ACCELERATION, TRUE    },
    {	"",			0,			0	}
};



/*
 * What is seen.
 */
const struct flag_type wear_loc_strings[] =
{
    {	"in the inventory",	WEAR_NONE,	TRUE	},
    {	"as a light",		WEAR_LIGHT,	TRUE	},
    {	"on the left finger",	WEAR_FINGER_L,	TRUE	},
    {	"on the right finger",	WEAR_FINGER_R,	TRUE	},
    {	"around the neck (1)",	WEAR_NECK_1,	TRUE	},
    {	"around the neck (2)",	WEAR_NECK_2,	TRUE	},
    {	"on the body",		WEAR_BODY,	TRUE	},
    {	"over the head",	WEAR_HEAD,	TRUE	},
    {   "in the eyes",          WEAR_IN_EYES,   TRUE    },
    {   "on the face",          WEAR_ON_FACE,   TRUE    },
    {   "spinning around(1)",   WEAR_ORBIT,     TRUE    },
    {   "spinning around(2)",   WEAR_ORBIT_2,   TRUE    },
    {	"on the legs",		WEAR_LEGS,	TRUE	},
    {	"on the feet",		WEAR_FEET,	TRUE	},
    {	"on the hands",		WEAR_HANDS,	TRUE	},
    {	"on the arms",		WEAR_ARMS,	TRUE	},
    {	"as a shield",		WEAR_SHIELD,	TRUE	},
    {	"about the shoulders",	WEAR_ABOUT,	TRUE	},
    {	"around the waist",	WEAR_WAIST,	TRUE	},
    {	"on the left wrist",	WEAR_WRIST_L,	TRUE	},
    {	"on the right wrist",	WEAR_WRIST_R,	TRUE	},
    {	"wielded",		WEAR_WIELD,	TRUE	},
    {   "dual wielded",         WEAR_WIELD_2,   TRUE    },
    {	"held in the hands",	WEAR_HOLD,	TRUE	},
    {   "on the left ankle",    WEAR_ANKLE_L,   TRUE    },
    {   "on the right ankle",   WEAR_ANKLE_R,   TRUE    },
    {   "in the left ear",      WEAR_EAR_L,     TRUE    },
    {   "in the right ear",     WEAR_EAR_R,     TRUE    },
    {	"",			0			}
};


/*
 * What is typed.
 * Neck2 should not be settable for loaded mobiles.
 */
const struct flag_type wear_loc_flags[] =
{
    {	"none",		WEAR_NONE,	TRUE	},
    {	"light",	WEAR_LIGHT,	TRUE	},
    {	"lfinger",	WEAR_FINGER_L,	TRUE	},
    {	"rfinger",	WEAR_FINGER_R,	TRUE	},
    {	"neck1",	WEAR_NECK_1,	TRUE	},
    {	"neck2",	WEAR_NECK_2,	TRUE	},
    {	"body",		WEAR_BODY,	TRUE	},
    {	"head",		WEAR_HEAD,	TRUE	},
    {   "lense",        WEAR_IN_EYES,   TRUE    },
    {   "mask",         WEAR_ON_FACE,   TRUE    },
    {   "orbit1",       WEAR_ORBIT,     TRUE    },
    {   "orbit2",       WEAR_ORBIT_2,   TRUE    },
    {	"legs",		WEAR_LEGS,	TRUE	},
    {	"feet",		WEAR_FEET,	TRUE	},
    {	"hands",	WEAR_HANDS,	TRUE	},
    {	"arms",		WEAR_ARMS,	TRUE	},
    {	"shield",	WEAR_SHIELD,	TRUE	},
    {	"about",	WEAR_ABOUT,	TRUE	},
    {	"waist",	WEAR_WAIST,	TRUE	},
    {	"lwrist",	WEAR_WRIST_L,	TRUE	},
    {	"rwrist",	WEAR_WRIST_R,	TRUE	},
    {	"wielded",	WEAR_WIELD,	TRUE	},
    {   "dual",         WEAR_WIELD_2,   TRUE    },
    {	"hold",		WEAR_HOLD,	TRUE	},
    {   "lankle",       WEAR_ANKLE_L,   TRUE    },
    {   "rankle",       WEAR_ANKLE_R,   TRUE    },
    {   "lear",         WEAR_EAR_L,     TRUE    },
    {   "rear",         WEAR_EAR_R,     TRUE    },
    {	"",		0,		0	}
};



const struct flag_type weapon_flags[] =
{
    {	"hit",		0,	TRUE	},
    {	"slice",	1,	TRUE	},
    {	"stab",		2,	TRUE	},
    {	"slash",	3,	TRUE	},
    {	"whip",		4,	TRUE	},
    {	"claw",		5,	TRUE	},
    {	"blast",	6,	TRUE	},
    {	"pound",	7,	TRUE	},
    {	"crush",	8,	TRUE	},
    {	"grep",		9,	TRUE	},
    {	"bite",		10,	TRUE	},
    {	"pierce",	11,	TRUE	},
    {	"suction",	12,	TRUE	},
    {	"chop",		13,	TRUE	},
    {	"",		0,	0	}
};


const struct flag_type container_flags[] =
{
    {	"closeable",		1,		TRUE	},
    {	"pickproof",		2,		TRUE	},
    {	"closed",		4,		TRUE	},
    {	"locked",		8,		TRUE	},
    {	"",			0,		0	}
};



const struct flag_type liquid_flags[] =
{
    {	"water",		0,	TRUE	},
    {	"beer",			1,	TRUE	},
    {	"wine",			2,	TRUE	},
    {	"ale",			3,	TRUE	},
    {	"dark-ale",		4,	TRUE	},
    {	"whisky",		5,	TRUE	},
    {	"lemonade",		6,	TRUE	},
    {	"firebreather",		7,	TRUE	},
    {	"local-specialty",	8,	TRUE	},
    {	"slime-mold-juice",	9,	TRUE	},
    {	"milk",			10,	TRUE	},
    {	"tea",			11,	TRUE	},
    {	"coffee",		12,	TRUE	},
    {	"blood",		13,	TRUE	},
    {	"salt-water",		14,	TRUE	},
    {	"cola",			15,	TRUE	},
    {	"",			0,	0	}
};

const struct flag_type immune_flags[] =
{
    {   "summon",    IMM_SUMMON,    TRUE   },
    {   "charm",     IMM_CHARM,     TRUE   },
    {   "magic",     IMM_MAGIC,     TRUE   },
    {   "weapon",    IMM_WEAPON,    TRUE   },
    {   "bash",      IMM_BASH,      TRUE   },
    {   "pierce",    IMM_PIERCE,    TRUE   },
    {   "slash",     IMM_SLASH,     TRUE   },
    {   "fire",      IMM_FIRE,      TRUE   },
    {   "cold",      IMM_COLD,      TRUE   },
    {   "lightning", IMM_LIGHTNING, TRUE   },
    {   "acid",      IMM_ACID,      TRUE   },
    {   "poison",    IMM_POISON,    TRUE   },
    {   "negative",  IMM_NEGATIVE,  TRUE   },
    {   "holy",      IMM_HOLY,      TRUE   },
    {   "energy",    IMM_ENERGY,    TRUE   },
    {   "mental",    IMM_MENTAL,    TRUE   },
    {   "disease",   IMM_DISEASE,   TRUE   },
    {   "drowning",  IMM_DROWNING,  TRUE   },
    {   "light",     IMM_LIGHT,     TRUE   },
    {   "nerve",     IMM_NERVE,	    TRUE   },
    {   "",          0,             0      }
};

const struct flag_type mprog_types[] =
{
    {   "error_prog",     ERROR_PROG,     FALSE  },
    {   "in_file_prog",   IN_FILE_PROG,   FALSE  },
    {   "act_prog",       ACT_PROG,       TRUE   },
    {   "speech_prog",    SPEECH_PROG,    TRUE   },
    {   "rand_prog",      RAND_PROG,      TRUE   },
    {   "fight_prog",     FIGHT_PROG,     TRUE   },
    {   "death_prog",     DEATH_PROG,     TRUE   },
    {   "hitprcnt_prog",  HITPRCNT_PROG,  TRUE   },
    {   "entry_prog",     ENTRY_PROG,     TRUE   },
    {   "greet_prog",     GREET_PROG,     TRUE   },
    {   "all_greet_prog", ALL_GREET_PROG, TRUE   },
    {   "give_prog",      GIVE_PROG,      TRUE   },
    {   "bribe_prog",     BRIBE_PROG,     TRUE   },
    {   "",               0,              0,     }
};

const struct flag_type oprog_types[] =
{
    {   "error_prog",     OBJ_TRAP_ERROR,     FALSE  },
    {   "get_prog",       OBJ_TRAP_GET,       TRUE   },
    {   "get_from_prog",  OBJ_TRAP_GET_FROM,  TRUE   },
    {   "drop_prog",      OBJ_TRAP_DROP,      TRUE   },
    {   "put_prog",       OBJ_TRAP_PUT,       TRUE   },
    {   "give_prog",      OBJ_TRAP_GIVE,      TRUE   },
    {   "fill_prog",      OBJ_TRAP_FILL,      TRUE   },
    {   "wear_prog",      OBJ_TRAP_WEAR,      TRUE   },
    {   "look_prog",      OBJ_TRAP_LOOK,      TRUE   },
    {   "look_in_prog",   OBJ_TRAP_LOOK_IN,   TRUE   },
    {   "invoke_prog",    OBJ_TRAP_INVOKE,    TRUE   },
    {   "use_prog",       OBJ_TRAP_USE,       TRUE   },
    {   "cast_prog",      OBJ_TRAP_CAST,      TRUE   },
    {   "cast_sn_prog",   OBJ_TRAP_CAST_SN,   TRUE   },
    {   "join_prog",      OBJ_TRAP_JOIN,      TRUE   },
    {   "separate_prog",  OBJ_TRAP_SEPARATE,  TRUE   },
    {   "buy_prog",       OBJ_TRAP_BUY,       TRUE   },
    {   "sell_prog",      OBJ_TRAP_SELL,      TRUE   },
    {   "store_prog",     OBJ_TRAP_STORE,     TRUE   },
    {   "retrieve_prog",  OBJ_TRAP_RETRIEVE,  TRUE   },
    {   "open_prog",      OBJ_TRAP_OPEN,      TRUE   },
    {   "close_prog",     OBJ_TRAP_CLOSE,     TRUE   },
    {   "lock_prog",      OBJ_TRAP_LOCK,      TRUE   },
    {   "unlock_prog",    OBJ_TRAP_UNLOCK,    TRUE   },
    {   "pick_prog",      OBJ_TRAP_PICK,      TRUE   },
    {   "throw_prog",     OBJ_TRAP_THROW,     TRUE   },
    {   "rand_prog",      OBJ_TRAP_RANDOM,    TRUE   },
    {   "",               0,                  0      }
};

const struct flag_type rprog_types[] =
{
    {   "error_prog",     ROOM_TRAP_ERROR,    FALSE  },
    {   "enter_prog",     ROOM_TRAP_ENTER,    TRUE   },
    {   "exit_prog",      ROOM_TRAP_EXIT,     TRUE   },
    {   "pass_prog",      ROOM_TRAP_PASS,     TRUE   },
    {   "cast_prog",      ROOM_TRAP_CAST,     TRUE   },
    {   "cast_sn_prog",   ROOM_TRAP_CAST_SN,  TRUE   },
    {   "sleep_prog",     ROOM_TRAP_SLEEP,    TRUE   },
    {   "wake_prog",      ROOM_TRAP_WAKE,     TRUE   },
    {   "rest_prog",      ROOM_TRAP_REST,     TRUE   },
    {   "death_prog",     ROOM_TRAP_DEATH,    TRUE   },
    {   "time_prog",      ROOM_TRAP_TIME,     TRUE   },
    {   "rand_prog",      ROOM_TRAP_RANDOM,   TRUE   },
    {   "",               0,                  0      }
};

const struct flag_type eprog_types[] =
{
    {   "error_prog",     EXIT_TRAP_ERROR,    FALSE  },
    {   "enter_prog",     EXIT_TRAP_ENTER,    TRUE   },
    {   "exit_prog",      EXIT_TRAP_EXIT,     TRUE   },
    {   "pass_prog",      EXIT_TRAP_PASS,     TRUE   },
    {   "look_prog",      EXIT_TRAP_LOOK,     TRUE   },
    {   "scry_prog",      EXIT_TRAP_SCRY,     TRUE   },
    {   "open_prog",      EXIT_TRAP_OPEN,     TRUE   },
    {   "close_prog",     EXIT_TRAP_CLOSE,    TRUE   },
    {   "lock_prog",      EXIT_TRAP_LOCK,     TRUE   },
    {   "unlock_prog",    EXIT_TRAP_UNLOCK,   TRUE   },
    {   "pick_prog",      EXIT_TRAP_PICK,     TRUE   },
    {   "",               0,                  0      }
};
