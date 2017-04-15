/***************************************************************************
 *  file: spells.h , Implementation of magic spells.       Part of DIKUMUD *
 *  Usage : Spells                                                         *
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

#define MAX_BUF_LENGTH              240

/*
 * Spell numbers are well known.
 * They appear in tinyworld files attached to magical items.
 * Spells and skills share the same address space (for practicing)
 *   and also for special damage messages as attack types.
 */
#define TYPE_UNDEFINED               -1
#define SPELL_RESERVED_DBC            0  /* SKILL NUMBER ZERO */
#define SPELL_ARMOR                   1
#define SPELL_TELEPORT                2
#define SPELL_BLESS                   3
#define SPELL_BLINDNESS               4
#define SPELL_BURNING_HANDS           5
#define SPELL_CALL_LIGHTNING          6
#define SPELL_CHARM_PERSON            7
#define SPELL_CHILL_TOUCH             8
#define SPELL_CLONE                   9
#define SPELL_COLOUR_SPRAY           10
#define SPELL_CONTROL_WEATHER        11
#define SPELL_CREATE_FOOD            12
#define SPELL_CREATE_WATER           13
#define SPELL_CURE_BLIND             14
#define SPELL_CURE_CRITIC            15
#define SPELL_CURE_LIGHT             16
#define SPELL_CURSE                  17
#define SPELL_DETECT_EVIL            18
#define SPELL_DETECT_INVISIBLE       19
#define SPELL_DETECT_MAGIC           20
#define SPELL_DETECT_POISON          21
#define SPELL_DISPEL_EVIL            22
#define SPELL_EARTHQUAKE             23
#define SPELL_ENCHANT_WEAPON         24
#define SPELL_ENERGY_DRAIN           25
#define SPELL_FIREBALL               26
#define SPELL_HARM                   27
#define SPELL_HEAL                   28
#define SPELL_INVISIBLE              29
#define SPELL_LIGHTNING_BOLT         30
#define SPELL_LOCATE_OBJECT          31
#define SPELL_MAGIC_MISSILE          32
#define SPELL_POISON                 33
#define SPELL_PROTECT_FROM_EVIL      34
#define SPELL_REMOVE_CURSE           35
#define SPELL_SANCTUARY              36
#define SPELL_SHOCKING_GRASP         37
#define SPELL_SLEEP                  38
#define SPELL_STRENGTH               39
#define SPELL_SUMMON                 40
#define SPELL_VENTRILOQUATE          41
#define SPELL_WORD_OF_RECALL         42
#define SPELL_REMOVE_POISON          43
#define SPELL_SENSE_LIFE             44



/*
 * Some skills.
 */
#define SKILL_SNEAK                  45
#define SKILL_HIDE                   46
#define SKILL_STEAL                  47
#define SKILL_BACKSTAB               48
#define SKILL_PICK_LOCK              49
#define SKILL_KICK                   50
#define SKILL_BASH                   51
#define SKILL_RESCUE                 52

/*
 * More spells.
 */
#define SPELL_IDENTIFY               53
#define SPELL_ANIMATE_DEAD           54
#define SPELL_FEAR                   55
#define SPELL_FLY                    56
#define SPELL_CONT_LIGHT             57
#define SPELL_KNOW_ALIGNMENT         58
#define SPELL_DISPEL_MAGIC           59
#define SPELL_CONJURE_ELEMENTAL      60
#define SPELL_CURE_SERIOUS           61
#define SPELL_CAUSE_LIGHT            62
#define SPELL_CAUSE_CRITICAL         63
#define SPELL_CAUSE_SERIOUS          64
#define SPELL_FLAMESTRIKE            65
#define SPELL_STONE_SKIN             66
#define SPELL_SHIELD                 67
#define SPELL_WEAKEN                 68
#define SPELL_MASS_INVISIBILITY      69
#define SPELL_ACID_BLAST             70
#define SPELL_GATE                   71
#define SPELL_FAERIE_FIRE            72
#define SPELL_FAERIE_FOG             73
#define SPELL_DROWN                  74
#define SPELL_DEMONFIRE              75
#define SPELL_TURN_UNDEAD            76
#define SPELL_INFRAVISION            77
#define SPELL_SANDSTORM              78
#define SPELL_HANDS_OF_WIND          79
#define SPELL_PLAGUE                 80
#define SPELL_REFRESH                81
#define SPELL_CURE_DISEASE           82
#define SPELL_CALM		     83
#define SPELL_CHAIN_LIGHTNING	     84
#define SPELL_HASTE		     85
#define SPELL_FRENZY		     86
#define MAX_SPL_LIST                 87

/*
 * 83 to 299 reserved for more spells.
 * Now some new skills.
 */
#define SKILL_TRIP                   300
#define SKILL_DODGE                  301
#define SKILL_SECOND_ATTACK          302
#define SKILL_DISARM                 303
#define SKILL_THIRD_ATTACK           304
#define SKILL_PARRY                  305

#define SKILL_RECALL                 800

/*
 * Only for dragon breaths, not char abilities.
 */
#define SPELL_FIRE_BREATH            900
#define SPELL_GAS_BREATH             901
#define SPELL_FROST_BREATH           902
#define SPELL_ACID_BREATH            903
#define SPELL_LIGHTNING_BREATH       904


/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_HIT                     1000
#define TYPE_BLUDGEON                (TYPE_HIT +  1)
#define TYPE_PIERCE                  (TYPE_HIT +  2)
#define TYPE_SLASH                   (TYPE_HIT +  3)
#define TYPE_WHIP                    (TYPE_HIT +  4)
#define TYPE_CLAW                    (TYPE_HIT +  5)
#define TYPE_BITE                    (TYPE_HIT +  6)
#define TYPE_STING                   (TYPE_HIT +  7)
#define TYPE_CRUSH                   (TYPE_HIT +  8)
#define TYPE_SUFFERING               (TYPE_HIT +  9)



/*
 * Saving throw types.
 */
#define SAVING_PARA   0
#define SAVING_ROD    1
#define SAVING_PETRI  2
#define SAVING_BREATH 3
#define SAVING_SPELL  4



/*
 *  Target types:
 *
 *  bit 0 : IGNORE TARGET
 *  bit 1 : PC/NPC in room
 *  bit 2 : PC/NPC in world
 *  bit 3 : Object held
 *  bit 4 : Object in inventory
 *  bit 5 : Object in room
 *  bit 6 : Object in world
 *  bit 7 : If fighting, and no argument, select tar_char as self
 *  bit 8 : If fighting, and no argument, select tar_char as victim (fighting)
 *  bit 9 : If no argument, select self, if argument check that it IS self.
 */
#define TAR_IGNORE        1
#define TAR_CHAR_ROOM     2
#define TAR_CHAR_WORLD    4
#define TAR_FIGHT_SELF    8
#define TAR_FIGHT_VICT   16
#define TAR_SELF_ONLY    32 /* Only a check, use with ei. TAR_CHAR_ROOM */
#define TAR_SELF_NONO    64 /* Only a check, use with ei. TAR_CHAR_ROOM */
#define TAR_OBJ_INV     128
#define TAR_OBJ_ROOM    256
#define TAR_OBJ_WORLD   512
#define TAR_OBJ_EQUIP  1024
#define TAR_SELF_DEF   2048  /* default to char if no arg */

typedef	void	SPELL_FUN	( byte level, struct char_data *ch,
				  char *arg, int type,
				  struct char_data *tar_ch,
				  struct obj_data *tar_obj );

struct spell_info_type
{
    SPELL_FUN *	spell_pointer;		/* Function to call		*/
    byte	minimum_position;	/* Position for caster		*/
    ubyte	min_usesmana;		/* Mana used			*/
    byte	beats;			/* Waiting time after spell	*/
    byte	min_level_cleric;	/* Level required for cleric	*/
    byte	min_level_magic;	/* Level required for mage	*/
    sh_int	targets;		/* Legal targets		*/
};

#define SPELL_TYPE_SPELL   0
#define SPELL_TYPE_POTION  1
#define SPELL_TYPE_WAND    2
#define SPELL_TYPE_STAFF   3
#define SPELL_TYPE_SCROLL  4


/*
 * Attack types with grammar.
 */
struct attack_hit_type
{
  char *singular;
  char *plural;
};

