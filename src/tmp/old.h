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


#include "colors.h"   /* Dave was watching */
/*
 * Accommodate old non-Ansi compilers.
 */
#if defined( TRADITIONAL )
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#endif



/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined( FALSE )
#define FALSE	 0
#endif

#if	!defined( TRUE )
#define TRUE	 1
#endif

#if	defined( _AIX )
#if	!defined( const )
#define const
#endif
typedef int				bool;
#define unix
#else
typedef unsigned char			bool;
#endif



/*
 * Structure types.
 */
typedef struct	affect_data		AFFECT_DATA;
typedef struct	area_data		AREA_DATA;
typedef struct	ban_data		BAN_DATA;
typedef struct	char_data		CHAR_DATA;
typedef struct  editing_data            EDITING_DATA;	/* OLC */
typedef struct	descriptor_data		DESCRIPTOR_DATA;
typedef struct	exit_data		EXIT_DATA;
typedef struct	extra_descr_data	EXTRA_DESCR_DATA;
typedef struct	help_data		HELP_DATA;
typedef struct	kill_data		KILL_DATA;
typedef struct	mob_index_data		MOB_INDEX_DATA;
typedef struct	note_data		NOTE_DATA;
typedef struct	obj_data		OBJ_DATA;
typedef struct	obj_index_data		OBJ_INDEX_DATA;
typedef struct	pc_data			PC_DATA;
typedef struct	reset_data		RESET_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	shop_data		SHOP_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	weather_data		WEATHER_DATA;

/*
 * Function types.
 */
typedef	void DO_FUN                     args( ( CHAR_DATA *ch,
					       char *argument ) );
typedef bool SPEC_FUN                   args( ( CHAR_DATA *ch ) );
typedef void SPELL_FUN                  args( ( int sn, int level,
					       CHAR_DATA *ch, void *vo ) );

/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 1024
#define MAX_STRING_LENGTH	 4096
#define MAX_INPUT_LENGTH	  160



/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_SKILL		  222
#define MAX_CLASS		   10
#define MAX_RACE                   10
#define MAX_CLAN                    4
#define MAX_LEVEL		  104
#define L_DIR		          MAX_LEVEL
#define L_SEN		          ( L_DIR - 1 )
#define L_JUN	        	  ( L_SEN - 1 )
#define L_APP                     ( L_JUN - 1 )
#define LEVEL_IMMORTAL		  L_APP
#define LEVEL_HERO		  ( LEVEL_IMMORTAL - 1 )

#define PULSE_PER_SECOND	  4
#define PULSE_VIOLENCE		  (  3 * PULSE_PER_SECOND )
#define PULSE_MOBILE		  (  4 * PULSE_PER_SECOND )
#define PULSE_TICK		  ( 30 * PULSE_PER_SECOND )
#define PULSE_AREA		  ( 50 * PULSE_PER_SECOND )



/*
 * Site ban structure.
 */
struct	ban_data
{
    BAN_DATA *	next;
    char *	name;
};



/*
 * Time and weather stuff.
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

struct	time_info_data
{
    int		hour;
    int		day;
    int		month;
    int		year;
};

struct	weather_data
{
    int		mmhg;
    int		change;
    int		sky;
    int		sunlight;
};



/*
 * Connected state for a channel.
 */
#define CON_PLAYING			0
#define CON_GET_NAME			1
#define CON_GET_OLD_PASSWORD		2
#define CON_CONFIRM_NEW_NAME		3
#define CON_GET_NEW_PASSWORD		4
#define CON_CONFIRM_NEW_PASSWORD	5
#define CON_GET_NEW_SEX			6
#define CON_GET_NEW_CLASS		7
#define CON_GET_NEW_RACE                8
#define CON_NEW_ISCOLOR	                9
#define CON_READ_MOTD		       10
#define CON_READ_IMOTD                 11
#define CON_SEE_ANSI                   12

/*
 * Connected states for OLC.
 */
#define CON_AEDITOR                     -1
#define CON_REDITOR                     -2
#define CON_MEDITOR                     -3
#define CON_OEDITOR                     -4


/*
 * Online editing structure.
 */
struct  editing_data	/* OLC */
{
    void *              pIndex;
    int                 vnum;
    int                 type;
    int                 command;
    char *              text;
    char **             destination;
};

/*
 * Area flags. OLC
 */
#define         AREA_NONE       0
#define         AREA_CHANGED    1	/* Area has been modified. */
#define         AREA_ADDED      2	/* Area has been added to. */
#define         AREA_LOADING    4	/* Used for counting in db.c */



/*
 * Descriptor (channel) structure.
 */
struct	descriptor_data
{
    DESCRIPTOR_DATA *	next;
    DESCRIPTOR_DATA *	snoop_by;
    CHAR_DATA *		character;
    CHAR_DATA *		original;
    char *		host;
    int		        descriptor;
    int		        connected;
    bool		fcommand;
    char		inbuf		[ MAX_INPUT_LENGTH*4 ];
    char		incomm		[ MAX_INPUT_LENGTH   ];
    char		inlast		[ MAX_INPUT_LENGTH   ];
    int			repeat;
    char *              showstr_head;
    char *              showstr_point;
    char *		outbuf;
    int			outsize;
    int			outtop;
    EDITING_DATA        editing;	/* OLC */
    void *              pEdit;		/* OLC */
    char **             pString;	/* OLC */
};



/*
 * Attribute bonus structures.
 */
struct	str_app_type
{
    int 	        tohit;
    int         	todam;
    int                 carry;
    int         	wield;
};

struct	int_app_type
{
    int         	learn;
};

struct	wis_app_type
{
    int         	practice;
};

struct	dex_app_type
{
    int         	defensive;
};

struct	con_app_type
{
    int         	hitp;
    int         	shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3



/*
 * Help table types.
 */
struct	help_data
{
    HELP_DATA * 	next;
    int 	        level;
    char *      	keyword;
    char *      	text;
};



/*
 * Shop types.
 */
#define MAX_TRADE	 5

struct	shop_data
{
    SHOP_DATA *	next;			/* Next shop in list		*/
    int 	keeper;			/* Vnum of shop keeper mob	*/
    int 	buy_type [ MAX_TRADE ];	/* Item types shop will buy	*/
    int 	profit_buy;		/* Cost multiplier for buying	*/
    int 	profit_sell;		/* Cost multiplier for selling	*/
    int 	open_hour;		/* First opening hour		*/
    int 	close_hour;		/* First closing hour		*/
};



/*
 * Per-class stuff.
 */
struct	class_type
{
    char 	who_name	[ 4 ];	/* Three-letter name for 'who'	*/
    char        full_name       [ 15 ]; /* Full name of Class           */
    int 	attr_prime;		/* Prime attribute		*/
    int 	weapon;			/* First weapon			*/
    int 	guild;			/* Vnum of guild room		*/
    int 	skill_adept;		/* Maximum skill level		*/
    int 	thac0_00;		/* Thac0 for level  0		*/
    int 	thac0_97;		/* Thac0 for level 97		*/
    int  	hp_min;			/* Min hp gained on leveling	*/
    int	        hp_max;			/* Max hp gained on leveling	*/
    bool	fMana;			/* Class gains mana on level	*/
};

struct  race_type
{
    char        race_name      [ 4 ];
    char        race_full      [ 20 ];
    int         atribmod1;
    int         atribmod2;
    int         modamount1;
    int         modamount2;
};

struct  clan_type
{
    char        clan_name      [ 15 ];
    char        clan_diety     [ 15 ];
    char        lev1           [ 20 ];
    char        lev2           [ 20 ];
    char        lev3           [ 20 ];
    char        lev4           [ 20 ];
    char        lev5           [ 20 ];
    int         cloc;
    int         sac_type;
};

/*
 * Data structure for notes.
 */
struct	note_data
{
    NOTE_DATA *	next;
    char *	sender;
    char *	date;
    char *	to_list;
    char *	subject;
    char *	text;
    time_t      date_stamp;
};



/*
 * An affect.
 */
struct	affect_data
{
    AFFECT_DATA *	next;
    int 		type;
    int 		duration;
    int 		location;
    int 		modifier;
    int			bitvector;
    bool                deleted;
};



/*
 * A kill structure (indexed by level).
 */
struct	kill_data
{
    int                 number;
    int                 killed;
};



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_CITYGUARD	   3060
#define MOB_VNUM_VAMPIRE	   3404
#define MOB_VNUM_ULT               3160

#define MOB_VNUM_AIR_ELEMENTAL     8914
#define MOB_VNUM_EARTH_ELEMENTAL   8915
#define MOB_VNUM_WATER_ELEMENTAL   8916
#define MOB_VNUM_FIRE_ELEMENTAL    8917
#define MOB_VNUM_DUST_ELEMENTAL    8918

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		      1		/* Auto set for mobs	*/
#define ACT_SENTINEL		      2		/* Stays in one room	*/
#define ACT_SCAVENGER		      4		/* Picks up objects	*/
#define ACT_AGGRESSIVE		     32		/* Attacks PC's		*/
#define ACT_STAY_AREA		     64		/* Won't leave area	*/
#define ACT_WIMPY		    128		/* Flees when hurt	*/
#define ACT_PET			    256		/* Auto set for pets	*/
#define ACT_TRAIN		    512		/* Can train PC's	*/
#define ACT_PRACTICE		   1024		/* Can practice PC's	*/
#define ACT_GAMBLE                 2048         /* Runs a gambling game */


/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		      1
#define AFF_INVISIBLE		      2
#define AFF_DETECT_EVIL		      4
#define AFF_DETECT_INVIS	      8
#define AFF_DETECT_MAGIC	     16
#define AFF_DETECT_HIDDEN	     32
#define AFF_HOLD		     64
#define AFF_SANCTUARY		    128
#define AFF_FAERIE_FIRE		    256
#define AFF_INFRARED		    512
#define AFF_CURSE		   1024
#define AFF_WEB                    2048
#define AFF_POISON		   4096
#define AFF_PROTECT		   8192
#define AFF_SNEAK		  32768
#define AFF_HIDE		  65536
#define AFF_SLEEP		 131072
#define AFF_CHARM		 262144
#define AFF_FLYING		 524288
#define AFF_PASS_DOOR		1048576
#define AFF_BLADES              2097152
#define AFF_SUMMONED            4194304
#define AFF_MUTE                8388608
#define AFF_GILLS              16777216
#define AFF_FIRESHIELD         33554432 
#define AFF_SHOCKSHIELD        67108864 
#define AFF_ICESHIELD         134217728
#define AFF_CHAOS             268435456
#define AFF_PEACE             536870912
#define AFF_ANTI_FLEE        1073741824
#define AFF_DISJUNCTION      2147483648
#define AFF_FUMBLE                    1
#define AFF_REGEN                     2
#define AFF_CONFUSED                  4

/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2



/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MONEY_ONE	      2
#define OBJ_VNUM_MONEY_SOME	      3

#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_FINAL_TURD	     16

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22

#define OBJ_VNUM_SCHOOL_MACE	   15001
#define OBJ_VNUM_SCHOOL_DAGGER	   15002
#define OBJ_VNUM_SCHOOL_SWORD	   15003
#define OBJ_VNUM_SCHOOL_VEST	   15004
#define OBJ_VNUM_SCHOOL_SHIELD	   15005
#define OBJ_VNUM_SCHOOL_BANNER     15006
#define OBJ_VNUM_SCHOOL_CLUB       15007

#define OBJ_VNUM_BLACK_POWDER      8903
#define OBJ_VNUM_FLAMEBLADE        8920


/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		      8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_FURNITURE		     12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		     15
#define ITEM_DRINK_CON		     17
#define ITEM_KEY		     18
#define ITEM_FOOD		     19
#define ITEM_MONEY		     20
#define ITEM_BOAT		     22
#define ITEM_CORPSE_NPC		     23
#define ITEM_CORPSE_PC		     24
#define ITEM_FOUNTAIN		     25
#define ITEM_PILL		     26
#define ITEM_LENSE		     27
#define ITEM_BLOOD                   28

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		      1
#define ITEM_HUM		      2
#define ITEM_DARK		      4
#define ITEM_LOCK		      8
#define ITEM_EVIL		     16
#define ITEM_INVIS		     32
#define ITEM_MAGIC		     64
#define ITEM_NODROP		    128
#define ITEM_BLESS		    256
#define ITEM_ANTI_GOOD		    512
#define ITEM_ANTI_EVIL		   1024
#define ITEM_ANTI_NEUTRAL	   2048
#define ITEM_NOREMOVE		   4096
#define ITEM_INVENTORY		   8192
#define ITEM_POISONED             16384
#define ITEM_ANTI_MAGE            32768
#define ITEM_ANTI_CLERIC          65536
#define ITEM_ANTI_THIEF          131072
#define ITEM_ANTI_WARRIOR        262144
#define ITEM_ANTI_PSI            524288
#define ITEM_ANTI_DRUID         1048576
#define ITEM_ANTI_RANGER        2097152
#define ITEM_ANTI_PALADIN       4194304
#define ITEM_ANTI_BARD          8388608
#define ITEM_FLAMING           16777216
/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		      1
#define ITEM_WEAR_FINGER	      2
#define ITEM_WEAR_NECK		      4
#define ITEM_WEAR_BODY		      8
#define ITEM_WEAR_HEAD		     16
#define ITEM_WEAR_LEGS		     32
#define ITEM_WEAR_FEET		     64
#define ITEM_WEAR_HANDS		    128 
#define ITEM_WEAR_ARMS		    256
#define ITEM_WEAR_SHIELD	    512
#define ITEM_WEAR_ABOUT		   1024 
#define ITEM_WEAR_WAIST		   2048
#define ITEM_WEAR_WRIST		   4096
#define ITEM_WIELD		   8192
#define ITEM_HOLD		  16384
#define ITEM_WEAR_ORBIT           32768
#define ITEM_WEAR_FACE            65536
#define ITEM_WEAR_CONTACT        131072



/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13
#define APPLY_MOVE		     14
#define APPLY_GOLD		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	     23
#define APPLY_SAVING_SPELL	     24
#define APPLY_ANTIDISARM             25



/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8



/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		   3001
#define ROOM_VNUM_CHAT		   1200
#define ROOM_VNUM_TEMPLE	   3001
#define ROOM_VNUM_ALTAR		   3054
#define ROOM_VNUM_SCHOOL	   3700
#define ROOM_VNUM_GRAVEYARD_A      3001
#define ROOM_VNUM_PURGATORY_A      3001


/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		      1
#define ROOM_NO_MOB		      4
#define ROOM_INDOORS		      8
#define ROOM_PRIVATE		    512
#define ROOM_SAFE		   1024
#define ROOM_SOLITARY		   2048
#define ROOM_PET_SHOP		   4096
#define ROOM_NO_RECALL		   8192
#define ROOM_CONE_OF_SILENCE      16384
#define ROOM_NO_MAGIC             32768
#define ROOM_NO_PKILL             65536
#define ROOM_NO_ASTRAL_IN        131072
#define ROOM_NO_ASTRAL_OUT       262144
#define ROOM_TELEPORT_AREA       524288
#define ROOM_TELEPORT_WORLD     1048576

/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5
#define MAX_DIR			      6	/* OLC */


/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		      1
#define EX_CLOSED		      2
#define EX_LOCKED		      4
#define EX_BASHED                     8
#define EX_BASHPROOF                 16
#define EX_PICKPROOF		     32
#define EX_PASSPROOF                 64

/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_UNDERWATER  	      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_MAX		     11



/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		     -1
#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		      1
#define WEAR_FINGER_R		      2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		      5
#define WEAR_HEAD		      6
#define WEAR_IN_EYES		      7
#define WEAR_ON_FACE		      8
#define WEAR_ORBIT                    9
#define WEAR_ORBIT_2                 10
#define WEAR_LEGS		     11
#define WEAR_FEET		     12
#define WEAR_HANDS		     13
#define WEAR_ARMS		     14
#define WEAR_SHIELD		     15
#define WEAR_ABOUT		     16
#define WEAR_WAIST		     17
#define WEAR_WRIST_L		     18
#define WEAR_WRIST_R		     19
#define WEAR_WIELD		     20
#define WEAR_WIELD_2		     21
#define WEAR_HOLD		     22
#define MAX_WEAR		     23



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK		      0
#define COND_FULL		      1
#define COND_THIRST		      2



/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_RESTING		      5
#define POS_FIGHTING		      6
#define POS_STANDING		      7



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		      1		/* Don't EVER set.	*/
#define PLR_BOUGHT_PET		      2

#define PLR_AUTOEXIT		      8
#define PLR_AUTOLOOT		     16
#define PLR_AUTOSAC                  32
#define PLR_BLANK		     64
#define PLR_BRIEF		    128
#define PLR_COMBINE		    512
#define PLR_PROMPT		   1024
#define PLR_TELNET_GA		   2048

#define PLR_HOLYLIGHT		   4096
#define PLR_WIZINVIS		   8192

#define	PLR_SILENCE		  32768
#define PLR_NO_EMOTE		  65536
#define PLR_NO_TELL		 262144
#define PLR_LOG			 524288
#define PLR_DENY		1048576
#define PLR_FREEZE		2097152
#define PLR_THIEF		4194304
#define PLR_KILLER		8388608
#define PLR_ANSI               16777216


/*
 * Obsolete bits.
 */
#if 0
#define PLR_AUCTION		      4	/* Obsolete	*/
#define PLR_CHAT		    256	/* Obsolete	*/
#define PLR_NO_SHOUT		 131072	/* Obsolete	*/
#endif



/*
 * Channel bits.
 */
#define	CHANNEL_AUCTION		      1
#define	CHANNEL_CHAT		      2

#define	CHANNEL_IMMTALK		      8
#define	CHANNEL_MUSIC		     16
#define	CHANNEL_QUESTION	     32
#define	CHANNEL_SHOUT		     64
#define	CHANNEL_YELL		    128
#define CHANNEL_CLANTALK            256
#define CHANNEL_CLASSTALK           512



/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
    MOB_INDEX_DATA *	next;
    SPEC_FUN *		spec_fun;
    char *              spec_name;	/* OLC */
    SHOP_DATA *		pShop;
    AREA_DATA *		area;		/* OLC */
    char *		player_name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    int 		vnum;
    int 		count;
    int 		killed;
    int 		sex;
    int 		level;
    int			act;
    int			affected_by;
    int 		alignment;
    int 		hitroll;		/* Unused */
    int 		ac;			/* Unused */
    int 		hitnodice;		/* Unused */
    int 		hitsizedice;		/* Unused */
    int 		hitplus;		/* Unused */
    int 		damnodice;		/* Unused */
    int 		damsizedice;		/* Unused */
    int 		damplus;		/* Unused */
    int			gold;			/* Unused */
};



/*
 * One character (PC or NPC).
 */
struct	char_data
{
    CHAR_DATA *		next;
    CHAR_DATA *		next_in_room;
    CHAR_DATA *		master;
    CHAR_DATA *		leader;
    CHAR_DATA *		fighting;
    CHAR_DATA *		reply;
    SPEC_FUN *		spec_fun;
    MOB_INDEX_DATA *	pIndexData;
    DESCRIPTOR_DATA *	desc;
    AFFECT_DATA *	affected;
    AFFECT_DATA *       affected2;
    NOTE_DATA *		pnote;
    OBJ_DATA *		carrying;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	was_in_room;
    PC_DATA *		pcdata;
    char *		name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    char *              prompt;
    int 		sex;
    int 		class;
    int 		race;
    int                 disarm;
    int                 returnp;
    int                 clan;
    int                 clevel;
    int 		level;
    int  		trust;
    bool                wizbit;
    int			played;
    time_t		logon;
    time_t		save_time;
    time_t              last_note;
    int 		timer;
    int 		wait;
    int 		hit;
    int 		max_hit;
    int 		mana;
    int 		max_mana;
    int 		move;
    int 		max_move;
    int			bp;
    int			max_bp;
    int			gold;
    int			exp;
    int			act;
    int			affected_by;
    int                 affected_by2;
    int 		position;
    int 		practice;
    int 		carry_weight;
    int 		carry_number;
    int 		saving_throw;
    int 		alignment;
    int 		hitroll;
    int 		damroll;
    int 		armor;
    int 		wimpy;
    int 		deaf;
    bool                deleted;
};



/*
 * Data which only PC's have.
 */
struct	pc_data
{
    PC_DATA *		next;
    char *		pwd;
    char *		bamfin;
    char *		bamfout;
    char *		title;
    int 		perm_str;
    int 		perm_int;
    int 		perm_wis;
    int 		perm_dex;
    int 		perm_con;
    int 		mod_str;
    int 		mod_int;
    int 		mod_wis;
    int 		mod_dex;
    int 		mod_con;
    int 		condition	[ 3 ];
    int                 pagelen;
    int 		learned		[ MAX_SKILL ];
    int 		security;	/* OLC */ /* Builder security */
    bool                switched;
};

/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_MAX		16

struct	liq_type
{
    char               *liq_name;
    char               *liq_color;
    int                 liq_affect [ 3 ];
};



/*
 * Extra description data for a room or object.
 */
struct	extra_descr_data
{
    EXTRA_DESCR_DATA *next;	   /* Next in list                     */
    char             *keyword;     /* Keyword in look/examine          */
    char             *description; /* What to see                      */
    bool              deleted;
};



/*
 * Prototype for an object.
 */
struct	obj_index_data
{
    OBJ_INDEX_DATA *	next;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    AREA_DATA *		area;	/* OLC */
    char *		name;
    char *		short_descr;
    char *		description;
    int 		vnum;
    int 		item_type;
    int 		extra_flags;
    int 		wear_flags;
    int 		count;
    int 		weight;
    int			cost;			/* Unused */
    int			value	[ 4 ];
    int                 level;
};



/*
 * One object.
 */
struct	obj_data
{
    OBJ_DATA *		next;
    OBJ_DATA *		next_content;
    OBJ_DATA *		contains;
    OBJ_DATA *		in_obj;
    CHAR_DATA *		carried_by;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    OBJ_INDEX_DATA *	pIndexData;
    ROOM_INDEX_DATA *	in_room;
    char *		name;
    char *		short_descr;
    char *		description;
    int 		item_type;
    int 		extra_flags;
    int 		wear_flags;
    int 		wear_loc;
    int 		weight;
    int			cost;
    int 		level;
    int 		timer;
    int			value	[ 4 ];
    bool                deleted;
};



/*
 * Exit data.
 */
struct	exit_data
{
    ROOM_INDEX_DATA *	to_room;
    EXIT_DATA *		next;	/* OLC */
    int 		vnum;
    int 		exit_info;
    int 		key;
    char *		keyword;
    char *		description;
    int			rs_flags;	/* OLC */
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile 
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */


/*
 * Area-reset definition.
 */
struct	reset_data
{
    RESET_DATA *	next;
    char		command;
    int 		arg1;
    int 		arg2;
    int 		arg3;
};



/*
 * Area definition.
 */
struct	area_data
{
    AREA_DATA *		next;
    RESET_DATA *	reset_first;
    RESET_DATA *	reset_last;
    char *		name;
    char *		filename;	/* OLC */
    char *		builders;	/* OLC */ /* Listing of */
    int			security;	/* OLC */ /* Value 1-9  */
    int			lvnum;		/* OLC */ /* Lower vnum */
    int			uvnum;		/* OLC */ /* Upper vnum */
    int			vnum;		/* OLC */ /* Area vnum  */
    int			area_flags;	/* OLC */
    int 		recall;
    int 		age;
    int 		nplayer;
};



/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    CHAR_DATA *		people;
    OBJ_DATA *		contents;
    EXTRA_DESCR_DATA *	extra_descr;
    AREA_DATA *		area;
    EXIT_DATA *		exit	[ 6 ];
    char *		name;
    char *		description;
    int 		vnum;
    int 		room_flags;
    int 		light;
    int 		sector_type;
};



/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED             -1
#define TYPE_HIT                 1000



/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4



/*
 * Skills include spells as a particular case.
 */
struct	skill_type
{
    char *	name;			   /* Name of skill		 */
    int 	skill_level [ MAX_CLASS ]; /* Level needed by class	 */
    SPELL_FUN *	spell_fun;		   /* Spell pointer (for spells) */
    int 	target;			   /* Legal targets		 */
    int 	minimum_position;	   /* Position for caster / user */
    int *	pgsn;			   /* Pointer to associated gsn	 */
    int 	min_mana;		   /* Minimum mana used		 */
    int 	beats;			   /* Waiting time after use	 */
    char *	noun_damage;		   /* Damage message		 */
    char *	msg_off;		   /* Wear off message		 */
};



/*
 * These are skill_lookup return values for common skills and spells.
 */
extern  int	gsn_backstab;
extern  int	gsn_dodge;
extern  int	gsn_hide;
extern  int	gsn_peek;
extern  int	gsn_pick_lock;
extern  int	gsn_sneak;
extern  int	gsn_steal;
extern  int	gsn_disarm;
extern  int     gsn_poison_weapon;

extern  int     gsn_bash;  
extern  int     gsn_dualwield;
extern	int	gsn_enhanced_damage;
extern  int     gsn_enhanced_damage2;
extern	int	gsn_enhanced_hit;
extern	int	gsn_kick;
extern	int	gsn_lore;
extern	int	gsn_parry;
extern	int	gsn_rescue;
extern	int	gsn_second_attack;
extern	int	gsn_third_attack;

extern	int	gsn_blindness;
extern	int	gsn_charm_person;
extern	int	gsn_curse;
extern	int	gsn_invis;
extern	int	gsn_mass_invis;
extern	int	gsn_poison;
extern	int	gsn_sleep;

extern  int     gsn_advance;
extern  int     gsn_allow;
extern  int     gsn_at;
extern  int     gsn_bamfin;
extern  int     gsn_bamfout;
extern  int     gsn_ban;
extern  int     gsn_deny;
extern  int     gsn_disconnect;
extern  int     gsn_echo;
extern  int     gsn_feed;
extern  int     gsn_force;
extern  int     gsn_freeze;
extern  int     gsn_goto;
extern  int     gsn_gouge;
extern  int     gsn_holylight;
extern  int     gsn_immtalk;
extern  int     gsn_irongrip;
extern  int     gsn_log;
extern  int     gsn_memory;
extern  int     gsn_mfind;
extern  int     gsn_mload;
extern  int     gsn_mset;
extern  int     gsn_mstat;
extern  int     gsn_mwhere;
extern  int     gsn_newlock;
extern  int     gsn_noemote;
extern  int     gsn_notell;
extern  int     gsn_numlock;
extern  int     gsn_ofind;
extern  int     gsn_oload;
extern  int     gsn_oset;
extern  int     gsn_ostat;
extern  int     gsn_owhere;
extern  int     gsn_pardon;
extern  int     gsn_peace;
extern  int     gsn_purge;
extern  int     gsn_reboot;
extern  int     gsn_recho;
extern  int     gsn_restore;
extern  int     gsn_return;
extern  int     gsn_rset;
extern  int     gsn_rstat;
extern  int     gsn_shutdown;
extern  int     gsn_silence;
extern  int     gsn_slay;
extern  int     gsn_slookup;
extern  int     gsn_snoop;
extern  int     gsn_sset;
extern  int     gsn_sstime;
extern  int     gsn_switch;
extern  int     gsn_transfer;
extern  int     gsn_trust;
extern  int     gsn_users;
extern  int     gsn_wizhelp;
extern  int     gsn_wizify;
extern  int     gsn_wizinvis;
extern  int     gsn_wizlock;

/*
 * Psionicist gsn's.
 */
extern  int     gsn_chameleon;
extern  int     gsn_domination;
extern  int     gsn_heighten;
extern  int     gsn_shadow;

/*
 * Utility macros.
 */
#define UMIN( a, b )		( ( a ) < ( b ) ? ( a ) : ( b ) )
#define UMAX( a, b )		( ( a ) > ( b ) ? ( a ) : ( b ) )
#define URANGE( a, b, c )	( ( b ) < ( a ) ? ( a )                       \
				                : ( ( b ) > ( c ) ? ( c )     \
						                  : ( b ) ) )
#define LOWER( c )		( ( c ) >= 'A' && ( c ) <= 'Z'                \
				                ? ( c ) + 'a' - 'A' : ( c ) )
#define UPPER( c )		( ( c ) >= 'a' && ( c ) <= 'z'                \
				                ? ( c ) + 'A' - 'a' : ( c ) )
#define IS_SET( flag, bit )	( ( flag ) &   ( bit ) )
#define SET_BIT( var, bit )	( ( var )  |=  ( bit ) )
#define REMOVE_BIT( var, bit )	( ( var )  &= ~( bit ) )
#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))	/* OLC */
#define STR(dat, field)         (( (dat)->field != NULL                    \
                                             ? (dat)->field                \
                                             : (dat)->pIndexData->field ))	/* OLC */


/*
 * Character macros.
 */
#define IS_NPC( ch )		( IS_SET( ( ch )->act, ACT_IS_NPC ) )
#define IS_IMMORTAL( ch )	( get_trust( ch ) >= LEVEL_IMMORTAL )
#define IS_HERO( ch )		( get_trust( ch ) >= LEVEL_HERO     )
#define IS_AFFECTED( ch, sn )	( IS_SET( ( ch )->affected_by, ( sn ) ) )
#define IS_AFFECTED2( ch, sn )  ( IS_SET( ( ch )->affected_by2, ( sn ) ) )

#define IS_GOOD( ch )		( ch->alignment >=  350 )
#define IS_EVIL( ch )		( ch->alignment <= -350 )
#define IS_NEUTRAL( ch )	( !IS_GOOD( ch ) && !IS_EVIL( ch ) )

#define IS_AWAKE( ch )		( ch->position > POS_SLEEPING )
#define PC(ch,field)            ((ch)->pcdata->field)	/* OLC */
#define GET_PC(ch,field,nopc)   ((ch->pcdata != NULL ? ch->pcdata->field    \
                                                     : (nopc)) )	/* OLC */
#define GET_AC( ch )		( ( ch )->armor				     \
				    + ( IS_AWAKE( ch )			     \
				    ? dex_app[get_curr_dex( ch )].defensive  \
				    : 0 ) )
#define GET_HITROLL( ch )      	( ( ch )->hitroll                            \
				 + str_app[get_curr_str( ch )].tohit )
#define GET_DAMROLL( ch )      	( ( ch )->damroll                            \
				 + str_app[get_curr_str( ch )].todam )

#define IS_OUTSIDE( ch )       	( !IS_SET(				     \
				    ( ch )->in_room->room_flags,       	     \
				    ROOM_INDOORS ) )

#define WAIT_STATE( ch, pulse ) ( ( ch )->wait = UMAX( ( ch )->wait,         \
						      ( pulse ) ) )

#define MANA_COST( ch, sn )     ( IS_NPC( ch ) ? 0 : UMAX (                  \
				skill_table[sn].min_mana,                    \
				100 / ( 2 + ch->level -                      \
				skill_table[sn].skill_level[ch->class] ) ) )

#define IS_SWITCHED( ch )       ( ch->pcdata->switched )

/*
 * Object macros.
 */
#define CAN_WEAR( obj, part )	( IS_SET( ( obj)->wear_flags,  ( part ) ) )
#define IS_OBJ_STAT( obj, stat )( IS_SET( ( obj)->extra_flags, ( stat ) ) )



/*
 * Description macros.
 */
#define PERS( ch, looker )	( can_see( looker, ( ch ) ) ?		     \
				( IS_NPC( ch ) ? ( ch )->short_descr	     \
				               : ( ch )->name ) : "someone" )


/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
    int 		position;
    int 		level;
    int 		log;
};



/*
 * Structure for a social in the socials table.
 */
struct	social_type
{
    char * const	name;
    char * const	char_no_arg;
    char * const	others_no_arg;
    char * const	char_found;
    char * const	others_found;
    char * const	vict_found;
    char * const	char_auto;
    char * const	others_auto;
};



/*
 * Global constants.
 */
extern	const	struct	str_app_type	str_app		[ 26 ];
extern	const	struct	int_app_type	int_app		[ 26 ];
extern	const	struct	wis_app_type	wis_app		[ 26 ];
extern	const	struct	dex_app_type	dex_app		[ 26 ];
extern	const	struct	con_app_type	con_app		[ 26 ];

extern	const	struct	class_type	class_table	[ MAX_CLASS   ];
extern  const   struct  race_type       race_table      [ MAX_RACE    ];
extern  const   struct  clan_type       clan_table      [ MAX_CLAN    ];
extern	const	struct	cmd_type	cmd_table	[ ];
extern	const	struct	liq_type	liq_table	[ LIQ_MAX     ];
extern	const	struct	skill_type	skill_table	[ MAX_SKILL   ];
extern	const	struct	social_type	social_table	[ ];
extern	char *	const			title_table	[ MAX_CLASS   ]
							[ MAX_LEVEL+1 ]
							[ 2 ];
extern	char *	const	dir_name        [];	/* OLC */
extern	const	int	rev_dir         [];	/* OLC */



/*
 * Global variables.
 */
extern		HELP_DATA	  *	help_first;
extern		SHOP_DATA	  *	shop_first;

extern		BAN_DATA	  *	ban_list;
extern		CHAR_DATA	  *	char_list;
extern		DESCRIPTOR_DATA   *	descriptor_list;
extern		NOTE_DATA	  *	note_list;
extern		OBJ_DATA	  *	object_list;

extern		AFFECT_DATA	  *	affect_free;
extern		BAN_DATA	  *	ban_free;
extern		CHAR_DATA	  *	char_free;
extern		DESCRIPTOR_DATA	  *	descriptor_free;
extern		EXTRA_DESCR_DATA  *	extra_descr_free;
extern		NOTE_DATA	  *	note_free;
extern		OBJ_DATA	  *	obj_free;
extern		PC_DATA		  *	pcdata_free;

extern		char			bug_buf		[ ];
extern		time_t			current_time;
extern		bool			fLogAll;
extern		FILE *			fpReserve;
extern		KILL_DATA		kill_table	[ ];
extern		char			log_buf		[ ];
extern		TIME_INFO_DATA		time_info;
extern		WEATHER_DATA		weather_info;
extern          char              *     down_time;
extern          char              *     warning1;
extern          char              *     warning2;

/*
 * online creation externals  OLC
 */
extern          AREA_DATA *             area_first;
extern          AREA_DATA *             area_last;
extern  	SHOP_DATA *             shop_last;

extern          int                     top_affect;
extern          int                     top_area;
extern          int                     top_ed;
extern          int                     top_exit;
extern          int                     top_help;
extern          int                     top_mob_index;
extern          int                     top_obj_index;
extern          int                     top_reset;
extern          int                     top_room;
extern          int                     top_shop;

extern          int                     top_vnum_mob;
extern          int                     top_vnum_obj;
extern          int                     top_vnum_room;

extern          char                    str_empty       [1];

extern  MOB_INDEX_DATA *        mob_index_hash  [MAX_KEY_HASH];
extern  OBJ_INDEX_DATA *        obj_index_hash  [MAX_KEY_HASH];
extern  ROOM_INDEX_DATA *       room_index_hash [MAX_KEY_HASH];


/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN( do_affectedby   );
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN(	do_answer	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN(	do_auction	);
DECLARE_DO_FUN( do_auto         );
DECLARE_DO_FUN( do_autoexit     );
DECLARE_DO_FUN( do_autoloot     );
DECLARE_DO_FUN( do_autosac      );
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN(	do_ban		);
DECLARE_DO_FUN(	do_bash 	);
DECLARE_DO_FUN( do_blank        );
DECLARE_DO_FUN(	do_brandish	);
DECLARE_DO_FUN( do_brief        );
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN( do_chameleon    );
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN(	do_channels	);
DECLARE_DO_FUN(	do_chat		);
DECLARE_DO_FUN( do_clantalk     );
DECLARE_DO_FUN( do_classtalk     );
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN( do_combine      );
DECLARE_DO_FUN(	do_commands	);
DECLARE_DO_FUN(	do_compare	);
DECLARE_DO_FUN(	do_config	);
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN(	do_description	);
DECLARE_DO_FUN(	do_disarm	);
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN( do_feed         );
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN( do_gouge        );
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN( do_heighten     );
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN( do_irongrip     );
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN(	do_lore		);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN(	do_induct	);
DECLARE_DO_FUN(	do_outcast	);
DECLARE_DO_FUN(	do_levup	);
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN(	do_mload	);
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN(	do_murde	);
DECLARE_DO_FUN(	do_murder	);
DECLARE_DO_FUN(	do_music	);
DECLARE_DO_FUN(	do_newlock	);
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN(	do_note		);
DECLARE_DO_FUN(	do_notell	);
DECLARE_DO_FUN(	do_numlock	);
DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN(	do_owhere	);
DECLARE_DO_FUN( do_pagelen      );
DECLARE_DO_FUN(	do_pardon	);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN(	do_peace	);
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN(	do_poison_weapon);
DECLARE_DO_FUN(	do_pose		);
DECLARE_DO_FUN(	do_practice	);
DECLARE_DO_FUN( do_prompt       );
DECLARE_DO_FUN(	do_purge	);
DECLARE_DO_FUN(	do_put		);
DECLARE_DO_FUN(	do_quaff	);
DECLARE_DO_FUN(	do_question	);
DECLARE_DO_FUN(	do_qui		);
DECLARE_DO_FUN(	do_quit		);
DECLARE_DO_FUN(	do_reboo	);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN(	do_recall	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN(	do_reply	);
DECLARE_DO_FUN(	do_report	);
DECLARE_DO_FUN(	do_rescue	);
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN(	do_rset		);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN(	do_sacrifice	);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN( do_shadow       );
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN(	do_silence	);
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN( do_slist        );
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN(	do_socials	);
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN( do_spells       );
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN(	do_sstime	);
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN(	do_steal	);
DECLARE_DO_FUN(	do_switch	);
DECLARE_DO_FUN(	do_tell		);
DECLARE_DO_FUN(	do_time		);
DECLARE_DO_FUN(	do_title	);
DECLARE_DO_FUN(	do_train	);
DECLARE_DO_FUN(	do_transfer	);
DECLARE_DO_FUN(	do_trust	);
DECLARE_DO_FUN(	do_typo		);
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN(	do_users	);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN( do_dual         );
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN(	do_where	);
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN(	do_whois	);
DECLARE_DO_FUN(	do_wimpy	);
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN( do_wizify       );
DECLARE_DO_FUN( do_wizlist      );
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN(	do_zap		);
DECLARE_DO_FUN(	do_stare	);

/*
 * Online creation commands  OLC
 */
DECLARE_DO_FUN( do_ashow        );
DECLARE_DO_FUN( do_rshow        );
DECLARE_DO_FUN( do_oshow        );
DECLARE_DO_FUN( do_mshow        );

DECLARE_DO_FUN( do_aedit        );
DECLARE_DO_FUN( do_redit        );
DECLARE_DO_FUN( do_oedit        );
DECLARE_DO_FUN( do_medit        );

DECLARE_DO_FUN( do_resets       );

DECLARE_DO_FUN( do_asave        );

DECLARE_DO_FUN( do_arealist     );

/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(	spell_acid_blast	);
DECLARE_SPELL_FUN(	spell_armor		);
DECLARE_SPELL_FUN(      spell_blade_barrier     );
DECLARE_SPELL_FUN(      spell_blades            );
DECLARE_SPELL_FUN(	spell_bless		);
DECLARE_SPELL_FUN(	spell_beserk		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_call_lightning	);
DECLARE_SPELL_FUN(	spell_cause_critical	);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(	spell_change_sex	);
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_chill_touch	);
DECLARE_SPELL_FUN(	spell_colour_spray	);
DECLARE_SPELL_FUN(      spell_confusion         );
DECLARE_SPELL_FUN(	spell_continual_light	);
DECLARE_SPELL_FUN(	spell_control_weather	);
DECLARE_SPELL_FUN(	spell_create_food	);
DECLARE_SPELL_FUN(	spell_create_spring	);
DECLARE_SPELL_FUN(	spell_create_water	);
DECLARE_SPELL_FUN(	spell_cure_blindness	);
DECLARE_SPELL_FUN(	spell_cure_critical	);
DECLARE_SPELL_FUN(	spell_cure_light	);
DECLARE_SPELL_FUN(	spell_cure_poison	);
DECLARE_SPELL_FUN(	spell_cure_serious	);
DECLARE_SPELL_FUN(	spell_curse		);
DECLARE_SPELL_FUN(	spell_detect_evil	);
DECLARE_SPELL_FUN(	spell_detect_hidden	);
DECLARE_SPELL_FUN(	spell_detect_invis	);
DECLARE_SPELL_FUN(	spell_detect_magic	);
DECLARE_SPELL_FUN(	spell_detect_poison	);
DECLARE_SPELL_FUN(      spell_disjunction       );
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(	spell_dispel_magic	);
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_far_lash  	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(      spell_flameblade        );
DECLARE_SPELL_FUN(      spell_fumble            );
DECLARE_SPELL_FUN(	spell_icestorm		);
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(	spell_fly		);
DECLARE_SPELL_FUN(	spell_gate		);
DECLARE_SPELL_FUN(	spell_general_purpose	);
DECLARE_SPELL_FUN(	spell_giant_strength	);
DECLARE_SPELL_FUN(	spell_harm		);
DECLARE_SPELL_FUN(	spell_heal		);
DECLARE_SPELL_FUN(	spell_high_explosive	);
DECLARE_SPELL_FUN(      spell_holy_fire       );
DECLARE_SPELL_FUN(	spell_identify		);
DECLARE_SPELL_FUN(	spell_infravision	);
DECLARE_SPELL_FUN(	spell_invis		);
DECLARE_SPELL_FUN(	spell_know_alignment	);
DECLARE_SPELL_FUN(      spell_know_person       );
DECLARE_SPELL_FUN(	spell_lightning_bolt	);
DECLARE_SPELL_FUN(	spell_locate_object	);
DECLARE_SPELL_FUN(	spell_magic_missile	);
DECLARE_SPELL_FUN(      spell_mana              );
DECLARE_SPELL_FUN(	spell_mass_invis	);
DECLARE_SPELL_FUN(	spell_pass_door		);
DECLARE_SPELL_FUN(      spell_permanency        );
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(      spell_power_bind        );
DECLARE_SPELL_FUN(      spell_power_fear        );
DECLARE_SPELL_FUN(      spell_power_pain        );
DECLARE_SPELL_FUN(      spell_power_slay        );
DECLARE_SPELL_FUN(	spell_protection	);
DECLARE_SPELL_FUN(	spell_refresh		);
DECLARE_SPELL_FUN(      spell_regenerate        );
DECLARE_SPELL_FUN(	spell_remove_curse	);
DECLARE_SPELL_FUN(      spell_return            );
DECLARE_SPELL_FUN(	spell_sanctuary		);
DECLARE_SPELL_FUN(	spell_shocking_grasp	);
DECLARE_SPELL_FUN(	spell_shield		);
DECLARE_SPELL_FUN(	spell_sleep		);
DECLARE_SPELL_FUN(      spell_spell_bind        );
DECLARE_SPELL_FUN(	spell_stone_skin	);
DECLARE_SPELL_FUN(	spell_summon		);
DECLARE_SPELL_FUN(	spell_astral		);
DECLARE_SPELL_FUN(	spell_teleport		);
DECLARE_SPELL_FUN(	spell_teleport_area	);
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(      spell_way_walk          );
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(	spell_web		);
DECLARE_SPELL_FUN(      spell_wizard_mark       );
DECLARE_SPELL_FUN(	spell_word_of_recall	);
DECLARE_SPELL_FUN(	spell_acid_breath	);
DECLARE_SPELL_FUN(	spell_fire_breath	);
DECLARE_SPELL_FUN(	spell_frost_breath	);
DECLARE_SPELL_FUN(	spell_gas_breath	);
DECLARE_SPELL_FUN(	spell_lightning_breath	);
DECLARE_SPELL_FUN(      spell_shockshield       );
DECLARE_SPELL_FUN(      spell_fireshield        );
DECLARE_SPELL_FUN(      spell_iceshield         );
DECLARE_SPELL_FUN(      spell_chaos_field       );
DECLARE_SPELL_FUN(      spell_peace_aura        );
DECLARE_SPELL_FUN(      spell_unholy_fire       );
/*
 * Psi spell_functions, in magic.c.
 */
DECLARE_SPELL_FUN(      spell_adrenaline_control);
DECLARE_SPELL_FUN(      spell_agitation         );
DECLARE_SPELL_FUN(      spell_aura_sight        );
DECLARE_SPELL_FUN(      spell_awe               );
DECLARE_SPELL_FUN(      spell_ballistic_attack  );
DECLARE_SPELL_FUN(      spell_biofeedback       );
DECLARE_SPELL_FUN(      spell_cell_adjustment   );
DECLARE_SPELL_FUN(      spell_combat_mind       );
DECLARE_SPELL_FUN(      spell_complete_healing  );
DECLARE_SPELL_FUN(      spell_control_flames    );
DECLARE_SPELL_FUN(      spell_create_sound      );
DECLARE_SPELL_FUN(      spell_death_field       );
DECLARE_SPELL_FUN(      spell_detonate          );
DECLARE_SPELL_FUN(      spell_disintegrate      );
DECLARE_SPELL_FUN(      spell_displacement      );
DECLARE_SPELL_FUN(      spell_domination        );
DECLARE_SPELL_FUN(      spell_ectoplasmic_form  );
DECLARE_SPELL_FUN(      spell_ego_whip          );
DECLARE_SPELL_FUN(      spell_energy_containment);
DECLARE_SPELL_FUN(      spell_enhance_armor     );
DECLARE_SPELL_FUN(      spell_enhanced_strength );
DECLARE_SPELL_FUN(      spell_flesh_armor       );
DECLARE_SPELL_FUN(      spell_inertial_barrier  );
DECLARE_SPELL_FUN(      spell_inflict_pain      );
DECLARE_SPELL_FUN(      spell_intellect_fortress);
DECLARE_SPELL_FUN(      spell_lend_health       );
DECLARE_SPELL_FUN(      spell_levitation        );
DECLARE_SPELL_FUN(      spell_mental_barrier    );
DECLARE_SPELL_FUN(      spell_mind_thrust       );
DECLARE_SPELL_FUN(      spell_project_force     );
DECLARE_SPELL_FUN(      spell_psionic_blast     );
DECLARE_SPELL_FUN(      spell_psychic_crush     );
DECLARE_SPELL_FUN(      spell_psychic_drain     );
DECLARE_SPELL_FUN(      spell_psychic_healing   );
DECLARE_SPELL_FUN(      spell_share_strength    );
DECLARE_SPELL_FUN(      spell_thought_shield    );
DECLARE_SPELL_FUN(      spell_ultrablast        );


/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if	defined( _AIX )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( apollo )
int	atoi		args( ( const char *string ) );
void *	calloc		args( ( unsigned nelem, size_t size ) );
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( hpux )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( linux )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( macintosh )
#define NOCRYPT
#if	defined( unix )
#undef	unix
#endif
#endif

#if	defined( MIPS_OS )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( MSDOS )
#define NOCRYPT
#if	defined( unix )
#undef	unix
#endif
#endif

#if	defined( NeXT )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if	defined( sequent )
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined( sun )
char *	crypt		args( ( const char *key, const char *salt ) );
int	fclose		args( ( FILE *stream ) );
int	fprintf		args( ( FILE *stream, const char *format, ... ) );
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined( ultrix )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif



/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined( NOCRYPT )
#define crypt( s1, s2 )	( s1 )
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined( macintosh )
#define PLAYER_DIR	""		/* Player files			*/
#define NULL_FILE	"proto.are"	/* To reserve one stream	*/
#endif

#if defined( MSDOS )
#define PLAYER_DIR	""		/* Player files                 */
#define NULL_FILE	"nul"		/* To reserve one stream	*/
#endif

#if defined( unix )
#define PLAYER_DIR	"../player/"	/* Player files			*/
#define NULL_FILE	"/dev/null"	/* To reserve one stream	*/
#endif

#if defined( linux )
#define PLAYER_DIR	"../player/"	/* Player files			*/
#define NULL_FILE	"/dev/null"	/* To reserve one stream	*/
#endif

#define AREA_LIST	"area.lst"	/* List of areas		*/

#define BUG_FILE	"bugs.txt"      /* For 'bug' and bug( )		*/
#define IDEA_FILE	"ideas.txt"	/* For 'idea'			*/
#define TYPO_FILE	"typos.txt"     /* For 'typo'			*/
#define NOTE_FILE	"notes.txt"	/* For 'notes'			*/
#define SHUTDOWN_FILE	"shutdown.txt"	/* For 'shutdown'		*/
#define DOWN_TIME_FILE  "time.txt"      /* For automatic shutdown       */


/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN

/* act_comm.c */
void	add_follower	args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	stop_follower	args( ( CHAR_DATA *ch ) );
void	die_follower	args( ( CHAR_DATA *ch, char *name ) );
bool	is_same_group	args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );

/* act_info.c */
void	set_title	args( ( CHAR_DATA *ch, char *title ) );
bool	check_blind	args( ( CHAR_DATA *ch ) );

/* act_move.c */
void	move_char	args( ( CHAR_DATA *ch, int door ) );

/* act_obj.c */

/* act_wiz.c */
ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg );	/* OLC */

/* comm.c */
void	close_socket	 args( ( DESCRIPTOR_DATA *dclose ) );
void	write_to_buffer	 args( ( DESCRIPTOR_DATA *d, const char *txt,
				int length ) );
void    send_to_all_char args( ( const char *text ) );
void	send_to_char	 args( ( int AType, const char *txt, CHAR_DATA *ch ) );
void    show_string      args( ( int AType, DESCRIPTOR_DATA *d, char *input ) );
void    set_char_color   args( ( int AType, CHAR_DATA *ch ) );
void	act	         args( ( int AType, const char *format, CHAR_DATA *ch,
				const void *arg1, const void *arg2,
				int type ) );

/* db.c */
void	boot_db		args( ( void ) );
void	area_update	args( ( void ) );
CD *	create_mobile	args( ( MOB_INDEX_DATA *pMobIndex ) );
OD *	create_object	args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void	clear_char	args( ( CHAR_DATA *ch ) );
void	free_char	args( ( CHAR_DATA *ch ) );
char *	get_extra_descr	args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *	get_mob_index	args( ( int vnum ) );
OID *	get_obj_index	args( ( int vnum ) );
RID *	get_room_index	args( ( int vnum ) );
char	fread_letter	args( ( FILE *fp ) );
int	fread_number	args( ( FILE *fp ) );
char *	fread_string	args( ( FILE *fp ) );
void	fread_to_eol	args( ( FILE *fp ) );
char *	fread_word	args( ( FILE *fp ) );
void *	alloc_mem	args( ( int sMem ) );
void *	alloc_perm	args( ( int sMem ) );
void	free_mem	args( ( void *pMem, int sMem ) );
char *	str_dup		args( ( const char *str ) );
void	free_string	args( ( char *pstr ) );
int	number_fuzzy	args( ( int number ) );
int	number_range	args( ( int from, int to ) );
int	number_percent	args( ( void ) );
int	number_door	args( ( void ) );
int	number_bits	args( ( int width ) );
int	number_mm	args( ( void ) );
int	dice		args( ( int number, int size ) );
int	interpolate	args( ( int level, int value_00, int value_32 ) );
void	smash_tilde	args( ( char *str ) );
bool	str_cmp		args( ( const char *astr, const char *bstr ) );
bool	str_prefix	args( ( const char *astr, const char *bstr ) );
bool	str_infix	args( ( const char *astr, const char *bstr ) );
bool	str_suffix	args( ( const char *astr, const char *bstr ) );
char *	capitalize	args( ( const char *str ) );
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	bug		args( ( const char *str, int param ) );
void	log_string	args( ( const char *str ) );
void	tail_chain	args( ( void ) );
void	reset_area      args( ( AREA_DATA * pArea ) );	/* OLC */

/* fight.c */
void	violence_update	args( ( void ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	damage		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			       int dt ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
void	raw_kill	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/* handler.c */
int	get_trust	args( ( CHAR_DATA *ch ) );
int	get_age		args( ( CHAR_DATA *ch ) );
int	get_curr_str	args( ( CHAR_DATA *ch ) );
int	get_curr_int	args( ( CHAR_DATA *ch ) );
int	get_curr_wis	args( ( CHAR_DATA *ch ) );
int	get_curr_dex	args( ( CHAR_DATA *ch ) );
int	get_curr_con	args( ( CHAR_DATA *ch ) );
int	can_carry_n	args( ( CHAR_DATA *ch ) );
int	can_carry_w	args( ( CHAR_DATA *ch ) );
bool	is_name		args( ( const char *str, char *namelist ) );
void	affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_to_char2 args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_strip	args( ( CHAR_DATA *ch, int sn ) );
bool	is_affected	args( ( CHAR_DATA *ch, int sn ) );
bool	is_affected2	args( ( CHAR_DATA *ch, int sn ) );
void	affect_join	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	char_from_room	args( ( CHAR_DATA *ch ) );
void	char_to_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_from_char	args( ( OBJ_DATA *obj ) );
int	apply_ac	args( ( OBJ_DATA *obj, int iWear ) );
OD *	get_eq_char	args( ( CHAR_DATA *ch, int iWear ) );
void	equip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	unequip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int	count_obj_list	args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void	obj_from_room	args( ( OBJ_DATA *obj ) );
void	obj_to_room	args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_obj	args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_from_obj	args( ( OBJ_DATA *obj ) );
void	extract_obj	args( ( OBJ_DATA *obj ) );
void	extract_char	args( ( CHAR_DATA *ch, bool fPull ) );
CD *	get_char_room	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_type	args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *	get_obj_list	args( ( CHAR_DATA *ch, char *argument,
			       OBJ_DATA *list ) );
OD *	get_obj_carry	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_wear	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_here	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	create_money	args( ( int amount ) );
int	get_obj_number	args( ( OBJ_DATA *obj ) );
int	get_obj_weight	args( ( OBJ_DATA *obj ) );
bool	room_is_dark	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	room_is_private	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
/* char *	item_type_name	args( ( OBJ_DATA *obj ) ); changed and put in bit.c*/
char *	affect_loc_name	args( ( int location ) );
char *	affect_bit_name	args( ( int vector ) );
/* char *	extra_bit_name	args( ( int extra_flags ) ); */
CD   *  get_char        args( ( CHAR_DATA *ch ) );
bool    longstring      args( ( CHAR_DATA *ch, char *argument ) );
bool    authorized      args( ( CHAR_DATA *ch, int gsn ) );
void    end_of_game     args( ( void ) );

/* interp.c */
void	interpret	args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number	args( ( char *arg ) );
int	number_argument	args( ( char *argument, char *arg ) );
char *	one_argument	args( ( char *argument, char *arg_first ) );
bool    IS_SWITCHED     args( ( CHAR_DATA *ch ) );

/* magic.c */
int	skill_lookup	args( ( const char *name ) );
bool	saves_spell	args( ( int level, CHAR_DATA *victim ) );
void	obj_cast_spell	args( ( int sn, int level, CHAR_DATA *ch,
			       CHAR_DATA *victim, OBJ_DATA *obj ) );

/* save.c */
void	save_char_obj	args( ( CHAR_DATA *ch ) );
bool	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name ) );

/* special.c */
SF *	spec_lookup	args( ( const char *name ) );

/* update.c */
void	advance_level	args( ( CHAR_DATA *ch ) );
void	gain_exp	args( ( CHAR_DATA *ch, int gain ) );
void	gain_condition	args( ( CHAR_DATA *ch, int iCond, int value ) );
void	update_handler	args( ( void ) );

/* string.c */
char *  format_string   args( ( char *oldstring /*, bool fSpace */ ) );	/* OLC */
void	string_edit	args( ( CHAR_DATA *ch, char **pString ) );	/* OLC */
void    string_add      args( ( CHAR_DATA *ch, char *argument ) );	/* OLC */
void    string_append   args( ( CHAR_DATA *ch, char **pString ) );	/* OLC */
char *  first_arg       args( ( char *argument, char *arg_first, bool fCase ) );	/* OLC */
char *	string_replace	args( ( char * orig, char * old, char * new ) );	/* OLC */
char *	string_proper	args( ( char * argument ) );	/* OLC */
char *	string_unpad	args( ( char * argument ) );	/* OLC */
int	arg_count	args( ( char *argument ) );	/* OLC */

/* olc.c */
void    aedit           args( ( CHAR_DATA *ch, char *argument ) );	/* OLC */
void    redit           args( ( CHAR_DATA *ch, char *argument ) );	/* OLC */
void    medit           args( ( CHAR_DATA *ch, char *argument ) );	/* OLC */
void    oedit           args( ( CHAR_DATA *ch, char *argument ) );	/* OLC */
void	add_reset	args( ( AREA_DATA *area, RESET_DATA *pReset, int index ) );	/* OLC */

/* bit.c */
char *	room_bit_name		args( ( int room_flags ) );	/* OLC */
int	room_name_bit		args( ( char *buf ) );		/* OLC */
int	sector_number		args( ( char *argument ) );	/* OLC */
char *	sector_name		args( ( int sect ) );		/* OLC */
int	item_name_type		args( ( char *name ) ); 	/* OLC */
char *	item_type_name		args( ( int item_type ) );	/* OLC */
int	extra_name_bit		args( ( char* buf ) );		/* OLC */
char *	extra_bit_name		args( ( int extra_flags ) ); 	/* OLC */
int	wear_name_bit		args( ( char* buf ) );		/* OLC */
char *	wear_bit_name		args( ( int wear ) );		/* OLC */
int	act_name_bit		args( ( char* buf ) );		/* OLC */
int	affect_name_bit		args( ( char* buf ) );		/* OLC */
int	affect_name_loc		args( ( char* name ) );		/* OLC */
int     wear_name_loc   	args( ( char *buf ) );		/* OLC */
char *	wear_loc_name		args( ( int wearloc ) );	/* OLC */
char *	act_bit_name		args( ( int act ) );		/* OLC */
int	get_weapon_type		args( ( char *arg ) );		/* OLC */
char *	get_type_weapon		args( ( int arg ) );		/* OLC */
int	get_container_flags	args( ( char *arg ) );		/* OLC */
char *	get_flags_container	args( ( int arg ) );		/* OLC */
int	get_liquid_type		args( ( char *arg ) );		/* OLC */
char *	get_type_liquid		args( ( int arg ) );		/* OLC */


#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
