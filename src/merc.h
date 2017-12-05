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
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#include "config.h"			/* Configuration / options */

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined( TRADITIONAL )
#define const
#define args( list )                    ( )
#define DECLARE_DO_FUN( fun )           void fun( )
#define DECLARE_SPEC_FUN( fun )         bool fun( )
#define DECLARE_SPELL_FUN( fun )        void fun( )
#define DECLARE_GAME_FUN( fun )         void fun( )
#define DECLARE_OBJ_FUN( fun )		bool fun( )
#else
#define args( list )                    list
#define DECLARE_DO_FUN( fun )           DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )         SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )        SPELL_FUN fun
#define DECLARE_GAME_FUN( fun )         GAME_FUN  fun
#define DECLARE_OBJ_FUN( fun )		OBJ_FUN   fun
#endif



/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if     !defined( FALSE )
#define FALSE    0
#endif

#if     !defined( TRUE )
#define TRUE     1
#endif

#if     defined( _AIX )
#if     !defined( const )
#define const
#endif
typedef int                             bool;
#define unix
#else
typedef unsigned char                   bool;
#endif

/* Bit defines, so you don't have to recalculate/think up
   all the bitvalues every time  -- Maniac -- */

#define		BIT_0		0
#define		BIT_1		1
#define		BIT_2		2
#define		BIT_3		4
#define		BIT_4		8
#define		BIT_5		16
#define		BIT_6		32
#define		BIT_7		64
#define		BIT_8		128
#define		BIT_9		256
#define		BIT_10		512
#define		BIT_11		1024
#define		BIT_12		2048
#define		BIT_13		4096
#define		BIT_14		8192
#define		BIT_15		16384
#define		BIT_16		32768
#define		BIT_17		65536
#define		BIT_18		131072
#define		BIT_19		262144
#define		BIT_20		524288
#define		BIT_21		1048576
#define		BIT_22		2097152
#define		BIT_23		4194304
#define		BIT_24		8388608
#define		BIT_25		16777216
#define		BIT_26		33554432
#define		BIT_27		67108864
#define		BIT_28		134217728
#define		BIT_29		268435456
#define		BIT_30		536870912
#define		BIT_31		1073741824
#define		BIT_32		2147483648

/* As far as I know this is all on 32 bit's machines, though it's best
   not to use the last (BIT_32) bit, it could be the signed/unsigned bit */

/*
 * Structure types.
 */
typedef struct  affect_data             AFFECT_DATA;
typedef struct  area_data               AREA_DATA;
typedef struct  ban_data                BAN_DATA;
typedef struct  char_data               CHAR_DATA;
typedef struct  descriptor_data         DESCRIPTOR_DATA;
typedef struct  exit_data               EXIT_DATA;
typedef struct  extra_descr_data        EXTRA_DESCR_DATA;
typedef struct	room_mana_data		ROOM_MANA_DATA;
typedef struct  help_data               HELP_DATA;
typedef struct  kill_data               KILL_DATA;
typedef struct  mob_index_data          MOB_INDEX_DATA;
typedef struct  note_data               NOTE_DATA;
typedef struct  obj_data                OBJ_DATA;
typedef struct  obj_index_data          OBJ_INDEX_DATA;
typedef struct  pc_data                 PC_DATA;
typedef struct  reset_data              RESET_DATA;
typedef struct  room_index_data         ROOM_INDEX_DATA;
typedef struct  shop_data               SHOP_DATA;
typedef struct  time_info_data          TIME_INFO_DATA;
typedef struct  weather_data            WEATHER_DATA;
typedef struct	disabled_data		DISABLED_DATA;
typedef struct  auction_data            AUCTION_DATA; /* auction data */
typedef	struct	mob_prog_data		MPROG_DATA;		/* MP */
typedef	struct	mob_prog_act_list	MPROG_ACT_LIST;		/* MP */


/*
 * Function types.
 */
typedef void DO_FUN		args( ( CHAR_DATA *ch,
					char *argument ) );
typedef bool SPEC_FUN		args( ( CHAR_DATA *ch ) );
typedef bool OBJ_SPEC_FUN	args( ( OBJ_DATA *obj ) );
typedef void SPELL_FUN		args( ( int sn, int level,
					CHAR_DATA *ch, void *vo ) );
typedef void GAME_FUN		args( ( CHAR_DATA *ch, 
					CHAR_DATA *croupier,
					int	amount,
					int	cheat,
					char *argument ) );

/*
 * String and memory management parameters.
 */
#define MAX_KEY_HASH             1024
#define MAX_STRING_LENGTH        4096
#define MAX_INPUT_LENGTH          180



/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_CHUNKS		   29		/* Used in ssm.c */
#define MAX_SKILL                 166
#define MAX_CLASS                   8
#define MAX_RACE                   41
#define MAX_LEVEL                  54
#define MAX_LANGUAGE		   12
#define L_DIR                      MAX_LEVEL
#define L_SEN                     ( L_DIR - 1 )
#define L_JUN                     ( L_SEN - 1 )
#define L_APP                     ( L_JUN - 1 )
#define LEVEL_IMMORTAL            L_APP
#define LEVEL_HERO                ( LEVEL_IMMORTAL - 1 )
#define L_HER			  LEVEL_HERO

#define PULSE_PER_SECOND            4
#define PULSE_VIOLENCE            (  2 * PULSE_PER_SECOND )
#define PULSE_MOBILE              (  5 * PULSE_PER_SECOND )
#define PULSE_TICK                ( 30 * PULSE_PER_SECOND )
#define PULSE_AREA                ( 60 * PULSE_PER_SECOND )
#define PULSE_AUCTION             ( 20 * PULSE_PER_SECOND ) /* 20 seconds */
/* Save the database - OLC 1.1b */
#define PULSE_DB_DUMP             (1800* PULSE_PER_SECOND ) /* 30 minutes  */
#define MUD_HOUR		  ( PULSE_TICK / PULSE_PER_SECOND )
#define MUD_DAY			  ( 24 * MUD_HOUR )
#define MUD_WEEK		  ( 7 * MUD_DAY )
#define MUD_MONTH		  ( 5 * MUD_WEEK )
#define MUD_YEAR		  ( 10 * MUD_MONTH )
/* hours per day, days per week, weeks per month, etc */
#define HOUR_DAY		  ( MUD_DAY / MUD_HOUR )
#define DAY_WEEK		  ( MUD_WEEK / MUD_DAY )
#define WEEK_MONTH		  ( MUD_MONTH / MUD_WEEK )
#define MONTH_YEAR		  ( MUD_YEAR / MUD_MONTH )
#define DAY_MONTH		  ( DAY_WEEK * WEEK_MONTH )
/* (Used for time_info.day... to name the days of the month) */
/* MUD dates adjusted to make ppl age a little faster
 * MUD_HOUR  =   7.5 seconds
 * MUD_DAY   =   180 seconds (3 minutes)
 * MUD_WEEK  =  1260 seconds (21 minutes)
 * MUD_MONTH =  6300 seconds (1 hour, 45 minutes)
 * MUD_YEAR  = 63000 seconds (17 hours, 30 minutes)
 * So for every 17.5 hours a player spends on-line, he ages 1 year.
 * (Humans start at +- 20, die at +- 60, so approx 40 life years
 *  this means they can be on-line for 29 days non-stop)
 * Canth - 7/6/97
 */

/*
 * Language stuff
 */

#define	COMMON		0
#define HUMAN		1
#define DWARVISH	2
#define ELVISH		3
#define GNOMISH		4
#define GOBLIN		5
#define ORCISH		6
#define OGRE		7
#define DROW		8
#define KOBOLD		9
#define TROLLISH	10
#define HOBBIT		11


/* Obj list stuff */
static char * const attack_table [ ] = 
{
	"hit",
	"slice", "stab", "slash", "whip", "claw",
	"blast", "pound", "crush", "grep", "bite",
	"pierce", "suction", "chop"
};


/*
 * Site ban structure.
 */
struct  ban_data
{
    BAN_DATA *  next;
    char *      name;
    int		level;
};

/*
 * Drunk struct
 */
struct struckdrunk
{
	int	min_drunk_level;
	int	number_of_rep;
	char	*replacement[11];
};

/*
 * Disable struct
 */

struct disabled_data
{
	DISABLED_DATA		*next;		/* pointer to the next one */
	struct cmd_type const	*command;	/* pointer to the command struct */
	char			*disabled_by;	/* name of disabler */
	int			 dislevel;	/* level of disabler */
	int			 uptolevel;	/* level of execution allowed */
};

/*
 * Time and weather stuff.
 */
#define SUN_DARK                    0
#define SUN_RISE                    1
#define SUN_LIGHT                   2
#define SUN_SET                     3

#define SKY_CLOUDLESS               0
#define SKY_CLOUDY                  1
#define SKY_RAINING                 2
#define SKY_LIGHTNING               3

struct  time_info_data
{
    int         hour;
    int         day;
    int		week;
    int         month;
    int         year;
};

struct  weather_data
{
    int         mmhg;
    int         change;
    int         sky;
    int         sunlight;
};


/*
 * Colour stuff by Lope of Loping Through The MUD
 */
#define CLR_CLEAR	"[0m"		/* Resets Colour	*/
#define CLR_RED		"[0;31m"	/* Normal Colours	*/
#define CLR_GREEN	"[0;32m"
#define CLR_YELLOW	"[0;33m"
#define CLR_BLUE	"[0;34m"
#define CLR_MAGENTA	"[0;35m"
#define CLR_CYAN	"[0;36m"
#define CLR_WHITE	"[0;37m"
#define CLR_D_GREY	"[1;30m"  	/* Light Colors		*/
#define CLR_B_RED	"[1;31m"
#define CLR_B_GREEN	"[1;32m"
#define CLR_B_YELLOW	"[1;33m"
#define CLR_B_BLUE	"[1;34m"
#define CLR_B_MAGENTA	"[1;35m"
#define CLR_B_CYAN	"[1;36m"
#define CLR_B_WHITE	"[1;37m"


/*
 * Connected state for a channel.
 */
#define CON_PLAYING                     0
#define CON_GET_NAME                    1
#define CON_GET_OLD_PASSWORD            2
#define CON_CONFIRM_NEW_NAME            3
#define CON_GET_NEW_PASSWORD            4
#define CON_CONFIRM_NEW_PASSWORD        5
#define CON_DISPLAY_RACE                6
#define CON_GET_NEW_RACE                7
#define CON_CONFIRM_NEW_RACE            8
#define CON_GET_NEW_SEX                 9
#define CON_DISPLAY_CLASS              10
#define CON_GET_NEW_CLASS              11
#define CON_CONFIRM_CLASS              12
#define CON_READ_MOTD                  13

/*
 * Classes
 */
#define CLASS_MAGE		0
#define CLASS_CLERIC		1
#define CLASS_THIEF		2
#define CLASS_WARRIOR		3
#define CLASS_PSIONICIST	4
#define CLASS_NECROMANCER	5
#define CLASS_DRUID		6
#define CLASS_RANGER		7
#define CLASS_NEW_CLASS		8

/*
 * Classes again..this time with bits.
 * Used for race/class restrictions - Canth
 */
#define CLS_MAGE		BIT_1
#define CLS_CLERIC		BIT_2
#define CLS_THIEF		BIT_3
#define CLS_WARRIOR		BIT_4
#define CLS_PSIONICIST		BIT_5
#define CLS_NECROMANCER		BIT_6
#define CLS_DRUID		BIT_7
#define CLS_RANGER		BIT_8
#define CLS_NEW_CLASS		BIT_9

/*
 * Clans
 */
/* The clans */
#define CLAN_NONE		0
#define CLAN_MYTHRAN		1
#define CLAN_DRAGON		2

/* Clan levels */
#define CLAN_EXILE		-2
#define CLAN_NOLEVEL		-1
#define CLAN_APPLICANT		0
#define CLAN_INITIATE		1
#define CLAN_LEVEL_1		2
#define CLAN_LEVEL_2		3
#define CLAN_LEVEL_3		4
#define CLAN_LEVEL_4		5
#define CLAN_LEVEL_5		6
#define CLAN_APPLICATOR		7
#define CLAN_RECRUITER		8
#define CLAN_LEADER		9
#define CLAN_GOD		10

/* Mana types */
#define MANA_NONE	BIT_0
#define MANA_WHITE	BIT_1
#define MANA_BLACK	BIT_2
#define MANA_BLUE	BIT_3
#define MANA_RED	BIT_4
#define MANA_GREEN	BIT_5
#define MANA_YELLOW	BIT_6
#define MANA_PURPLE	BIT_7
#define MANA_BROWN	BIT_8
#define MANA_NEW1	BIT_9
#define MANA_NEW2	BIT_10
#define MANA_NEW3	BIT_11
#define MANA_NEW4	BIT_12


/*
 * Race structures
 */
struct  race_type
{
    char *              name;
    int			race_lang;	/* Languages by Maniac and Canth */
    int			start_age;		/* Race ages for Mythran */
    int			start_age_mod[2];	/* by Maniac and Canth */
    int			death_age;		/* AD&D like age rules */
    int			death_age_mod[2];	/* (C) Mythran Mud 1996 */
    int                 race_abilities;
    int                 size;
    int                 str_mod;
    int                 int_mod;
    int                 wis_mod;
    int                 dex_mod;
    int                 con_mod;
    int			not_class;
    char *              dmg_message;
    char *              hate;
};

/* Race size defines by Maniac */
#define SIZE_ANY		0
#define SIZE_MINUTE		1	/* Insect Rat Plant                  */
#define SIZE_SMALL		2	/* Lizard                            */
#define SIZE_PETITE		3	/* Drow (Half)Dwarf Hobbit Kobold    */
#define SIZE_AVERAGE		4	/* Elf                               */
#define SIZE_MEDIUM		5	/* Human Halfelf                     */
#define SIZE_LARGE		6	/* Orc                               */
#define SIZE_HUGE		7	/* Giant ogre                        */
#define SIZE_TITANIC		8	/* Troll Hydra                       */
#define SIZE_GARGANTUAN		9	/* Dragon God                        */

/* Race ability bits */
#define RACE_NO_ABILITIES		BIT_0
#define RACE_PC_AVAIL			BIT_1
#define RACE_WATERBREATH		BIT_2
#define RACE_FLY			BIT_3
#define RACE_SWIM			BIT_4
#define RACE_WATERWALK			BIT_5
#define RACE_PASSDOOR			BIT_6
#define RACE_INFRAVISION		BIT_7
#define RACE_DETECT_ALIGN		BIT_8
#define RACE_DETECT_INVIS		BIT_9
#define RACE_DETECT_HIDDEN		BIT_10
#define RACE_PROTECTION			BIT_11
#define RACE_SANCT			BIT_12
#define RACE_WEAPON_WIELD		BIT_13

/*
 * Descriptor (channel) structure.
 */
struct  descriptor_data
{
    DESCRIPTOR_DATA *   next;
    DESCRIPTOR_DATA *   snoop_by;
    CHAR_DATA *         character;
    CHAR_DATA *         original;
    char *              host;
    int                 descriptor;
    int                 connected;
    bool                fcommand;
    char                inbuf           [ MAX_INPUT_LENGTH*4 ];
    char                incomm          [ MAX_INPUT_LENGTH   ];
    char                inlast          [ MAX_INPUT_LENGTH   ];
    int                 repeat;
    char *              showstr_head;
    char *              showstr_point;
    char *              outbuf;
    void *              pEdit;          /* OLC */
    char **             pString;        /* OLC */
    int                 editor;         /* OLC */
    int                 outsize;
    int                 outtop;
};



/*
 * Attribute bonus structures.
 */
struct  str_app_type
{
    int                 tohit;
    int                 todam;
    int                 carry;
    int                 wield;
};

struct  int_app_type
{
    int                 learn;
};

struct  wis_app_type
{
    int                 practice;
};

struct  dex_app_type
{
    int                 defensive;
};

struct  con_app_type
{
    int                 hitp;
    int                 shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM             0
#define TO_NOTVICT          1
#define TO_VICT             2
#define TO_CHAR             3



/*
 * Help table types.
 */
struct  help_data
{
    HELP_DATA *         next;
    AREA_DATA *		area;
    int                 level;
    char *              keyword;
    char *              text;
};



/*
 * Shop types.
 */
#define MAX_TRADE        5

struct  shop_data
{
    SHOP_DATA * next;                   /* Next shop in list            */
    int         keeper;                 /* Vnum of shop keeper mob      */
    int         buy_type [ MAX_TRADE ]; /* Item types shop will buy     */
    int         profit_buy;             /* Cost multiplier for buying   */
    int         profit_sell;            /* Cost multiplier for selling  */
    int         open_hour;              /* First opening hour           */
    int         close_hour;             /* First closing hour           */
};



/*
 * Per-class stuff.
 */
struct  class_type
{
    char        who_name        [ 4 ];  /* Three-letter name for 'who'  */
    int         attr_prime;             /* Prime attribute              */
    int         weapon;                 /* First weapon                 */
    int         guild;                  /* Vnum of guild room           */
    int         skill_adept;            /* Maximum skill level          */
    int         thac0_00;               /* Thac0 for level  0           */
    int         thac0_47;               /* Thac0 for level 47           */
    int         hp_min;                 /* Min hp gained on leveling    */
    int         hp_max;                 /* Max hp gained on leveling    */
    bool        fMana;                  /* Class gains mana on level    */
};

struct clan_type
{
	int	vnum;			/* Clan number			*/
	char *	name;			/* Clan name			*/
	char *	who_name;		/* CLan who name		*/
	int	pit;			/* Clan donation pit obj vnum	*/
	int	pitroom;		/* Clan don pit room vnum	*/
	int	rooms		[ 3 ];	/* Clan rooms			*/
	int	recall;			/* Clan recall room		*/
};

struct mana_type
{
	int	type;
	char *	name;
	int	opposite;
};

/*
 * Data structure for notes.
 */
struct  note_data
{
    NOTE_DATA * next;
    char *      sender;
    char *      date;
    char *      to_list;
    char *      subject;
    char *      text;
    time_t      date_stamp;
};



/*
 * An affect.
 */
struct  affect_data
{
    AFFECT_DATA *       next;
    int                 type;
    int                 duration;
    int                 location;
    int                 modifier;
    int                 bitvector;
    int			level;
    bool                deleted;
};



/*
 * A kill structure (indexed by level).
 */
struct  kill_data
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
#define MOB_VNUM_CITYGUARD         3060
#define MOB_VNUM_VAMPIRE           3404
#define MOB_VNUM_SECRETARY         3142
#define MOB_VNUM_MIDGAARD_MAYOR    3143
#define MOB_VNUM_ZOMBIE            3601
#define MOB_VNUM_DOGGY             3062

#define MOB_VNUM_AIR_ELEMENTAL     8914
#define MOB_VNUM_EARTH_ELEMENTAL   8915
#define MOB_VNUM_WATER_ELEMENTAL   8916
#define MOB_VNUM_FIRE_ELEMENTAL    8917
#define MOB_VNUM_DUST_ELEMENTAL    8918

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		BIT_1		/* Auto set for mobs    */
#define ACT_SENTINEL		BIT_2		/* Stays in one room    */
#define ACT_SCAVENGER		BIT_3		/* Picks up objects     */
#define ACT_IS_HEALER		BIT_4		/* Healers by MANIAC    */

#define ACT_AGGRESSIVE		BIT_6		/* Attacks PC's         */
#define ACT_STAY_AREA		BIT_7		/* Won't leave area     */
#define ACT_WIMPY		BIT_8		/* Flees when hurt      */
#define ACT_PET			BIT_9		/* Auto set for pets    */
#define ACT_TRAIN		BIT_10		/* Can train PC's       */
#define ACT_PRACTICE		BIT_11		/* Can practice PC's    */

#define ACT_BANKER		BIT_13		/* Is a banker		*/
#define ACT_TEACHER		BIT_14		/* Teacher of languages */

#define ACT_MOVED		BIT_18		/* 131072 Dont ever set */

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		BIT_1
#define AFF_INVISIBLE		BIT_2
#define AFF_DETECT_EVIL		BIT_3
#define AFF_DETECT_INVIS	BIT_4
#define AFF_DETECT_MAGIC	BIT_5
#define AFF_DETECT_HIDDEN	BIT_6
#define AFF_HOLD		BIT_7
#define AFF_SANCTUARY		BIT_8
#define AFF_FAERIE_FIRE		BIT_9
#define AFF_INFRARED		BIT_10
#define AFF_CURSE		BIT_11
#define AFF_CHANGE_SEX		BIT_12
#define AFF_POISON		BIT_13
#define AFF_PROTECT		BIT_14
#define AFF_POLYMORPH		BIT_15
#define AFF_SNEAK		BIT_16
#define AFF_HIDE		BIT_17
#define AFF_SLEEP		BIT_18
#define AFF_CHARM		BIT_19 
#define AFF_FLYING		BIT_20
#define AFF_PASS_DOOR		BIT_21
#define AFF_WATERWALK		BIT_22
#define AFF_SUMMONED		BIT_23
#define AFF_MUTE		BIT_24
#define AFF_GILLS		BIT_25
#define AFF_VAMP_BITE		BIT_26
#define AFF_GHOUL		BIT_27
#define AFF_FLAMING		BIT_28
#define AFF_PLAGUE		BIT_29

/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL                   0
#define SEX_MALE                      1
#define SEX_FEMALE                    2



/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MONEY_ONE            2
#define OBJ_VNUM_MONEY_SOME           3

#define OBJ_VNUM_CORPSE_NPC          10
#define OBJ_VNUM_CORPSE_PC           11
#define OBJ_VNUM_SEVERED_HEAD        12
#define OBJ_VNUM_TORN_HEART          13
#define OBJ_VNUM_SLICED_ARM          14
#define OBJ_VNUM_SLICED_LEG          15
#define OBJ_VNUM_FINAL_TURD          16

#define OBJ_VNUM_MUSHROOM            20
#define OBJ_VNUM_LIGHT_BALL          21
#define OBJ_VNUM_SPRING              22

#define OBJ_VNUM_SCHOOL_MACE       3700
#define OBJ_VNUM_SCHOOL_DAGGER     3701
#define OBJ_VNUM_SCHOOL_SWORD      3702
#define OBJ_VNUM_SCHOOL_VEST       3703
#define OBJ_VNUM_SCHOOL_SHIELD     3704
#define OBJ_VNUM_SCHOOL_BANNER     3716
#define OBJ_VNUM_SCHOOL_CLUB       3717

#define OBJ_VNUM_BLACK_POWDER      8903
#define OBJ_VNUM_FLAMEBLADE        8920
#define OBJ_VNUM_STAKE             3811
#define OBJ_VNUM_LICENSE           3011

#define OBJ_VNUM_MITHRIL           1312

/* Necromantic special items by Maniac */
#define OBJ_VNUM_NEC_DIRT	28
#define OBJ_VNUM_NEC_WOOD	27
#define OBJ_VNUM_NEC_BLOOD	25
#define OBJ_VNUM_NEC_BONE	26
#define OBJ_VNUM_NEC_HOOD	24
#define OBJ_VNUM_NEC_BLACKMOOR	29

/* Was 60 and 61 */
#define OBJ_VNUM_DIAMOND_RING	3371	/* Wedding rings for Marriage code */
#define OBJ_VNUM_WEDDING_BAND	3372	/* Canth (canth@xs4all.nl) of Mythran */

#define OBJ_VNUM_PORTAL		62	/* Default portal for portal spell */

#define OBJ_VNUM_BDAY_1		70	/* Birthday presents - Canth */
#define OBJ_VNUM_BDAY_2		71
#define OBJ_VNUM_BDAY_3		72
#define OBJ_VNUM_BDAY_4		73
#define OBJ_VNUM_BDAY_5		74

/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT                    1
#define ITEM_SCROLL                   2
#define ITEM_WAND                     3
#define ITEM_STAFF                    4
#define ITEM_WEAPON                   5

#define ITEM_TREASURE                 8
#define ITEM_ARMOR                    9
#define ITEM_POTION                  10

#define ITEM_FURNITURE               12
#define ITEM_TRASH                   13

#define ITEM_CONTAINER               15

#define ITEM_DRINK_CON               17
#define ITEM_KEY                     18
#define ITEM_FOOD                    19
#define ITEM_MONEY                   20
#define ITEM_SPECIAL                 21
#define ITEM_BOAT                    22
#define ITEM_CORPSE_NPC              23
#define ITEM_CORPSE_PC               24
#define ITEM_FOUNTAIN                25
#define ITEM_PILL                    26
#define ITEM_JUKEBOX                 27
#define ITEM_PORTAL		     28



/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		BIT_1
#define ITEM_HUM		BIT_2
#define ITEM_DARK		BIT_3
#define ITEM_LOCK		BIT_4
#define ITEM_EVIL		BIT_5
#define ITEM_INVIS		BIT_6
#define ITEM_MAGIC		BIT_7
#define ITEM_NODROP		BIT_8
#define ITEM_BLESS		BIT_9
#define ITEM_ANTI_GOOD		BIT_10
#define ITEM_ANTI_EVIL		BIT_11
#define ITEM_ANTI_NEUTRAL	BIT_12
#define ITEM_NOREMOVE		BIT_13
#define ITEM_INVENTORY		BIT_14
#define ITEM_POISONED		BIT_15
#define ITEM_VAMPIRE_BANE	BIT_16
#define ITEM_HOLY		BIT_17
#define ITEM_BLADE_THIRST	BIT_18		/* Item is aff. blade thirst */
#define ITEM_MELT_DROP		BIT_19		/* Item melts when dropped... */
#define ITEM_BURN_PROOF		BIT_20		/* Item is burn proof */
#define ITEM_NOPURGE		BIT_21		/* Can't be purged */
#define ITEM_HIDDEN		BIT_22		/* Item has been hidden */


/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		BIT_1
#define ITEM_WEAR_FINGER	BIT_2
#define ITEM_WEAR_NECK		BIT_3
#define ITEM_WEAR_BODY		BIT_4
#define ITEM_WEAR_HEAD		BIT_5
#define ITEM_WEAR_LEGS		BIT_6
#define ITEM_WEAR_FEET		BIT_7
#define ITEM_WEAR_HANDS		BIT_8
#define ITEM_WEAR_ARMS		BIT_9
#define ITEM_WEAR_SHIELD	BIT_10
#define ITEM_WEAR_ABOUT		BIT_11
#define ITEM_WEAR_WAIST		BIT_12
#define ITEM_WEAR_WRIST		BIT_13
#define ITEM_WIELD		BIT_14
#define ITEM_HOLD		BIT_15
#define ITEM_FLOATING		BIT_16		/* Not yet implemented */
#define ITEM_WEAR_BELT		BIT_17		/* Not yet implemented */
#define ITEM_WEAR_SHOULDER	BIT_18		/* Not yet implemented */

/* furniture flags */				/* Not yet implemented */
#define STAND_AT		BIT_1
#define STAND_ON		BIT_2
#define STAND_IN		BIT_3 
#define SIT_AT			BIT_4
#define SIT_ON			BIT_5
#define SIT_IN			BIT_6
#define REST_AT			BIT_7
#define REST_ON			BIT_8
#define REST_IN			BIT_9
#define SLEEP_AT		BIT_10
#define SLEEP_ON		BIT_11
#define SLEEP_IN		BIT_12
#define PUT_AT			BIT_13
#define PUT_ON			BIT_14
#define PUT_IN			BIT_15
#define PUT_INSIDE		BIT_16

#define FURNITURE_UNUSED	-1
#define FURNITURE_NONE		BIT_0
#define ST_AT			BIT_1	/*	Stand at	*/
#define ST_ON			BIT_2	/*	Stand on	*/
#define ST_IN			BIT_3	/*	Stand in	*/
#define SI_AT			BIT_4	/*	Sit at		*/
#define SI_ON			BIT_5	/*	Sit on		*/
#define SI_IN			BIT_6	/*	Sit in		*/
#define RE_AT			BIT_7	/*	Rest at		*/
#define RE_ON			BIT_8	/*	Rest on		*/
#define RE_IN			BIT_9	/*	Rest in		*/
#define SL_AT			BIT_10	/*	Sleep at	*/
#define SL_ON			BIT_11	/*	Sleep on	*/
#define SL_IN			BIT_12	/*	Sleep in	*/
#define PT_AT			BIT_13	/*	Put at		*/
#define PT_ON			BIT_14	/*	Put on		*/
#define PT_IN			BIT_15	/*	Put in		*/
#define PT_INSIDE		BIT_16	/*	Put inside	*/

/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE                    0
#define APPLY_STR                     1
#define APPLY_DEX                     2
#define APPLY_INT                     3
#define APPLY_WIS                     4
#define APPLY_CON                     5
#define APPLY_SEX                     6
#define APPLY_CLASS                   7
#define APPLY_LEVEL                   8
#define APPLY_AGE                     9
#define APPLY_HEIGHT                 10
#define APPLY_WEIGHT                 11
#define APPLY_MANA                   12
#define APPLY_HIT                    13
#define APPLY_MOVE                   14
#define APPLY_GOLD                   15
#define APPLY_EXP                    16
#define APPLY_AC                     17
#define APPLY_HITROLL                18
#define APPLY_DAMROLL                19
#define APPLY_SAVING_PARA            20
#define APPLY_SAVING_ROD             21
#define APPLY_SAVING_PETRI           22
#define APPLY_SAVING_BREATH          23
#define APPLY_SAVING_SPELL           24
#define APPLY_RACE                   25


/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE                BIT_1
#define CONT_PICKPROOF                BIT_2
#define CONT_CLOSED                   BIT_3
#define CONT_LOCKED                   BIT_4



/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO               2
#define ROOM_VNUM_CHAT             1200
#define ROOM_VNUM_TEMPLE           3001
#define ROOM_VNUM_ALTAR            3054
#define ROOM_VNUM_SCHOOL           3700
#define ROOM_VNUM_GRAVEYARD_A       427
#define ROOM_VNUM_PURGATORY_A       401


/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		BIT_1

#define ROOM_NO_MOB		BIT_3
#define ROOM_INDOORS		BIT_4
#define ROOM_UNDERGROUND	BIT_5
#define ROOM_ARENA		BIT_6

#define ROOM_PRIVATE		BIT_10
#define ROOM_SAFE		BIT_11
#define ROOM_SOLITARY		BIT_12
#define ROOM_PET_SHOP		BIT_13
#define ROOM_NO_RECALL		BIT_14
#define ROOM_CONE_OF_SILENCE	BIT_15


/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH                     0
#define DIR_EAST                      1
#define DIR_SOUTH                     2
#define DIR_WEST                      3
#define DIR_UP                        4
#define DIR_DOWN                      5



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		BIT_1
#define EX_CLOSED		BIT_2
#define EX_LOCKED		BIT_3
#define EX_BASHED		BIT_4
#define EX_BASHPROOF		BIT_5
#define EX_PICKPROOF		BIT_6
#define EX_PASSPROOF		BIT_7

/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE                   0
#define SECT_CITY                     1
#define SECT_FIELD                    2
#define SECT_FOREST                   3
#define SECT_HILLS                    4
#define SECT_MOUNTAIN                 5
#define SECT_WATER_SWIM               6
#define SECT_WATER_NOSWIM             7
#define SECT_UNDERWATER               8
#define SECT_AIR                      9
#define SECT_DESERT                  10
#define SECT_MAX                     11



/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE                    -1
#define WEAR_LIGHT                    0
#define WEAR_FINGER_L                 1
#define WEAR_FINGER_R                 2
#define WEAR_NECK_1                   3
#define WEAR_NECK_2                   4
#define WEAR_BODY                     5
#define WEAR_HEAD                     6
#define WEAR_LEGS                     7
#define WEAR_FEET                     8
#define WEAR_HANDS                    9
#define WEAR_ARMS                    10
#define WEAR_SHIELD                  11
#define WEAR_ABOUT                   12
#define WEAR_WAIST                   13
#define WEAR_WRIST_L                 14
#define WEAR_WRIST_R                 15
#define WEAR_WIELD                   16
#define WEAR_HOLD                    17
#define WEAR_WIELD_2                 18         /* by Thelonius */
#define	WEAR_FLOATING_1              19		/* not implemented */
#define WEAR_FLOATING_2	             20
#define WEAR_SHOULDER_L              21		/* Not implemented */
#define WEAR_SHOULDER_R              22
#define WEAR_BELT_1                  23		/* Not implemented */
#define WEAR_BELT_2                  24
#define WEAR_BELT_3                  25
#define WEAR_BELT_4                  26
#define WEAR_BELT_5                  27
#define MAX_WEAR                     28

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK                    0
#define COND_FULL                     1
#define COND_THIRST                   2



/*
 * Positions.
 */
#define POS_DEAD                      0
#define POS_MORTAL                    1
#define POS_INCAP                     2
#define POS_STUNNED                   3
#define POS_SLEEPING                  4
#define POS_RESTING                   5
#define POS_FIGHTING                  6
#define POS_STANDING                  7



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		BIT_1	/* Don't EVER set.      */
#define PLR_BOUGHT_PET		BIT_2
#define PLR_REGISTER		BIT_3	/* Registering for Pkill */
#define PLR_AUTOEXIT		BIT_4
#define PLR_AUTOLOOT		BIT_5
#define PLR_AUTOSAC		BIT_6
#define PLR_BLANK		BIT_7
#define PLR_BRIEF		BIT_8
#define PLR_QUESTOR		BIT_9
#define PLR_COMBINE		BIT_10
#define PLR_PROMPT		BIT_11
#define PLR_TELNET_GA		BIT_12
#define PLR_HOLYLIGHT		BIT_13
#define PLR_WIZINVIS		BIT_14
#define PLR_WIZBIT		BIT_15
#define PLR_SILENCE		BIT_16
#define PLR_NO_EMOTE		BIT_17
#define PLR_MOVED		BIT_18	/* Has to be same as ACT_MOVED */
#define PLR_NO_TELL		BIT_19
#define PLR_LOG			BIT_20
#define PLR_DENY		BIT_21
#define PLR_FREEZE		BIT_22
#define PLR_THIEF		BIT_23
#define PLR_KILLER		BIT_24
#define PLR_AUTOGOLD		BIT_25
#define PLR_AFK			BIT_26
#define PLR_COLOUR		BIT_27

/* WIZnet flags */
#define WIZ_ON			BIT_1	/* On / Off switch */
#define WIZ_TICKS		BIT_2	/* Tick's */
#define WIZ_LOGINS		BIT_3	/* Login / Logoff */
#define WIZ_DEATHS		BIT_4	/* Plr death's */
#define WIZ_RESETS		BIT_5	/* Area reset's */
#define WIZ_FLAGS		BIT_6	/* killer and thief flags */
#define WIZ_SACCING		BIT_7	/* Item saccing ??? */
#define WIZ_LEVELS		BIT_8	/* Level's advancing */
#define WIZ_SECURE		BIT_9	/* log -> screen */
#define WIZ_SWITCHES		BIT_10	/* Switch */
#define WIZ_SNOOPS		BIT_11	/* Snoops */
#define WIZ_RESTORE		BIT_12	/* Restores */
#define WIZ_LOAD		BIT_13	/* Loading of mobs, obj's */
#define WIZ_NEWBIE		BIT_14	/* Newbie's */
#define WIZ_SPAM		BIT_15	/* Spamming ??? */
#define WIZ_CHAT		BIT_16	/* Wiznet chatting */
#define WIZ_DEBUG		BIT_17	/* Nice way to get debug info */


/*
 * Obsolete bits.
 */
#if 0
#define PLR_CHAT                    256 /* Obsolete     */
#define ACT_GAMBLE		   2048 /* Obsolete, maniac, by game_lookup */
#endif



/*
 * Channel bits.
 */
#define CHANNEL_AUCTION		BIT_1
#define CHANNEL_CHAT		BIT_2

#define CHANNEL_IMMTALK		BIT_4
#define CHANNEL_MUSIC		BIT_5
#define CHANNEL_QUESTION	BIT_6
#define CHANNEL_SHOUT		BIT_7
#define CHANNEL_YELL		BIT_8
#define CHANNEL_NEWS		BIT_9
#define CHANNEL_CLAN		BIT_10
#define CHANNEL_WIZNEWS		BIT_11

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct  mob_index_data
{
    MOB_INDEX_DATA *    next;
    SPEC_FUN *          spec_fun;
    GAME_FUN *		game_fun;
    SHOP_DATA *         pShop;
    AREA_DATA *		area;			/* OLC */
    MPROG_DATA *	mobprogs;		/* MP */
    char *              player_name;
    char *              short_descr;
    char *              long_descr;
    char *              description;
    int                 vnum;
    int                 count;
    int                 killed;
    int                 sex;
    int                 level;
    int                 act;
    int                 affected_by;
    int                 alignment;
    int			progtypes;		/* MP */
    int                 hitroll;                /* Unused */
    int                 ac;                     /* Unused */
    int                 hitnodice;              /* Unused */
    int                 hitsizedice;            /* Unused */
    int                 hitplus;                /* Unused */
    int                 damnodice;              /* Unused */
    int                 damsizedice;            /* Unused */
    int                 damplus;                /* Unused */
    int                 gold;                   /* Unused */
    int                 race;
};



/*
 * One character (PC or NPC).
 */
struct  char_data
{
    CHAR_DATA *         next;
    CHAR_DATA *         next_in_room;
    CHAR_DATA *         master;
    CHAR_DATA *         leader;
    CHAR_DATA *         fighting;
    CHAR_DATA *         reply;
    CHAR_DATA *         hunting;
    CHAR_DATA *		questgiver;		/* Vassago, quest */
    CHAR_DATA *		riding;			/* Maniac, unimplemented */
    SPEC_FUN *          spec_fun;
    GAME_FUN *		game_fun;		/* Maniac, dice games */
    MOB_INDEX_DATA *    pIndexData;
    DESCRIPTOR_DATA *   desc;
    AFFECT_DATA *       affected;
    NOTE_DATA *         pnote;
    OBJ_DATA *          carrying;
    OBJ_DATA *		on;			/* Maniac, stand on objects */
    ROOM_INDEX_DATA *   in_room;
    ROOM_INDEX_DATA *   was_in_room;
    PC_DATA *           pcdata;
    MPROG_ACT_LIST *    mpact;			/* MP */
    char *              name;
    char *              short_descr;
    char *              long_descr;
    char *              description;
    int                 sex;
    int                 class;
    int                 race;
    int                 level;
    int                 trust;
    int                 played;
    int			current_age;		/* Current age */
    int			death_age;		/* Death age */
    int                 mpactnum;		/* MP */
    time_t              logon;
    time_t              save_time;
    time_t              last_note;
    int                 timer;
    int                 wait;
    int                 hit;
    int                 max_hit;
    int                 mana;
    int                 max_mana;
    int                 move;
    int                 max_move;
    int                 gold;
    int                 exp;
/* WHY is the queststuff CHAR_DATA and not PC_DATA??????
 * Canth (canth@xs4all.nl)
 * To be fixed... */
    int			questpoints;		/* Vassago */
    int			nextquest;		/* Vassago */
    int			countdown;		/* Vassago */
    int			questobj;		/* Vassago */
    int			questmob;		/* Vassago */
    int                 act;
    int                 affected_by;
    int                 position;
    int                 practice;
    int                 carry_weight;
    int                 carry_number;
    int                 saving_throw;
    int                 alignment;
    int                 hitroll;
    int                 damroll;
    int                 armor;
    int                 wimpy;
    int                 deaf;
    int			fur_pos;		/* Maniac, furniture pos. */
    bool                deleted;
};



/*
 * Data which only PC's have.
 */
struct  pc_data
{
    PC_DATA *           next;
    char *              pwd;
    char *              bamfin;
    char *              bamfout;
    char *              immskll;
    char *              title;
    char *              prompt;
    char *		who_text;	/* Text in who listing - Canth */
    char *		spouse;		/* Name of spouse if wed - Canth */
    int                 perm_str;
    int                 perm_int;
    int                 perm_wis;
    int                 perm_dex;
    int                 perm_con;
    int                 mod_str;
    int                 mod_int;
    int                 mod_wis;
    int                 mod_dex;
    int                 mod_con;
    int                 condition       [ 3 ];
    int                 pagelen;
    int                 learned         [ MAX_SKILL ];		/* Maniac */
    int			language	[ MAX_LANGUAGE ];	/* Maniac */
    int			speaking;				/* Maniac */
    int			learn;					/* Maniac */
    int			last_age;				/* Canth */
    int			balance;				/* Maniac */
    int			shares;					/* Maniac */
    int			security;				/* OLC */
    int			clan;					/* Maniac */
    int			clanlevel;				/* Maniac */
    int			wiznet;					/* Maniac */
    int			oldclass;	/* remort, old classes	   Maniac */
    int			mprog_edit;	/* Mprog currently editing - Walker */
    bool                switched;
    bool		confirm_delete;				/* Maniac */
};

/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_BLOOD	13		/* Maniac, handy for vamp checks */
#define LIQ_MAX         16

struct  liq_type
{
    char               *liq_name;
    char               *liq_color;
    int                 liq_affect [ 3 ];
};



/*
 * Extra description data for a room or object.
 */
struct  extra_descr_data
{
    EXTRA_DESCR_DATA *next;        /* Next in list                     */
    char             *keyword;     /* Keyword in look/examine          */
    char             *description; /* What to see                      */
    bool              deleted;
};

/*
 * Room mana data for room mana... new feature by Maniac for
 * Mythran Mud v3.1.5 (C) 1996 The Maniac
 */
struct	room_mana_data
{
	ROOM_MANA_DATA	*next;		/* Next in list for this room	*/
	int		type;		/* Type of mana			*/
	int		amount;		/* Amount of mana		*/
};


/*
 * Prototype for an object.
 */
struct  obj_index_data
{
    OBJ_INDEX_DATA *    next;
    EXTRA_DESCR_DATA *  extra_descr;
    AFFECT_DATA *       affected;
    AREA_DATA *		area;
    OBJ_SPEC_FUN *	obj_spec_fun;		/* Maniac, unimplemented */
    char *              name;
    char *              short_descr;
    char *              description;
    int                 vnum;
    int                 item_type;
    int                 extra_flags;
    int                 wear_flags;
    int                 count;
    int                 weight;
    int                 cost;                   /* Unused */
    int                 value   [ 4 ];
};



/*
 * One object.
 */
struct  obj_data
{
    OBJ_DATA *          next;
    OBJ_DATA *          next_content;
    OBJ_DATA *          contains;
    OBJ_DATA *          in_obj;
    CHAR_DATA *         carried_by;
    EXTRA_DESCR_DATA *  extra_descr;
    AFFECT_DATA *       affected;
    OBJ_INDEX_DATA *    pIndexData;
    ROOM_INDEX_DATA *   in_room;
    OBJ_SPEC_FUN *	obj_spec_fun;		/* Maniac - Unimplemented */
    char *              name;
    char *              short_descr;
    char *              description;
    int                 item_type;
    int                 extra_flags;
    int                 wear_flags;
    int                 wear_loc;
    int                 weight;
    int                 cost;
    int                 level;
    int                 timer;
    int                 value   [ 4 ];
    bool                deleted;
};



/*
 * Exit data.
 */
struct  exit_data
{
    ROOM_INDEX_DATA *   to_room;
    EXIT_DATA *         next;           /* OLC */
    int                 rs_flags;       /* OLC */
    int                 orig_door;      /* OLC */
    int                 vnum;
    int                 exit_info;
    int                 key;
    char *              keyword;
    char *              description;
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
struct  reset_data
{
    RESET_DATA *        next;
    char                command;
    int                 arg1;
    int                 arg2;
    int                 arg3;
};

struct  area_data
{
    AREA_DATA *         next;
    char *              name;
    int                 recall;
    int                 age;
    int                 nplayer;
    char *              filename;       /* OLC */
    char *              builders;       /* OLC - Listing of builders */
    int                 security;       /* OLC - Value 0-infinity  */
    int                 lvnum;          /* OLC - Lower vnum */
    int                 uvnum;          /* OLC - Upper vnum */
    int                 vnum;           /* OLC - Area vnum  */
    int                 area_flags;     /* OLC */
};


/*
 * Room type.
 */
struct  room_index_data
{
    ROOM_INDEX_DATA *   next;
    CHAR_DATA *         people;
    OBJ_DATA *          contents;
    EXTRA_DESCR_DATA *  extra_descr;
    AREA_DATA *         area;
    EXIT_DATA *         exit    [ 6 ];
    RESET_DATA *        reset_first;    /* OLC */
    RESET_DATA *        reset_last;     /* OLC */
    ROOM_MANA_DATA *	mana;		/* Maniac, room mana struct */
    char *              name;
    char *              description;
    int                 vnum;
    int                 room_flags;
    int                 light;
    int                 sector_type;
};

/*
 * Auction information
 */
struct  auction_data
{
    OBJ_DATA  * item;   /* a pointer to the item */
    CHAR_DATA * seller; /* a pointer to the seller - which may NOT quit */
    CHAR_DATA * buyer;  /* a pointer to the buyer - which may NOT quit */
    int		bet;    /* last bet - or 0 if noone has bet anything */
    int		going;  /* 1,2, sold */
    int		pulse;  /* how many pulses (.25 sec) until another call-out ? */
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
#define TAR_IGNORE                  0
#define TAR_CHAR_OFFENSIVE          1
#define TAR_CHAR_DEFENSIVE          2
#define TAR_CHAR_SELF               3
#define TAR_OBJ_INV                 4

#define TARGET_CHAR                 0
#define TARGET_OBJ                  1
#define TARGET_ROOM                 2
#define TARGET_NONE                 3


/* Language type */
struct	lang_type
{
	char	*name;			/* Language name */
};

/*
 * Skills include spells as a particular case.
 */
struct  skill_type
{
    char *      name;                      /* Name of skill              */
    int         skill_level [ MAX_CLASS ]; /* Level needed by class      */
    SPELL_FUN * spell_fun;                 /* Spell pointer (for spells) */
    int         target;                    /* Legal targets              */
    int         minimum_position;          /* Position for caster / user */
    int *       pgsn;                      /* Pointer to associated gsn  */
    int         min_mana;                  /* Minimum mana used          */
    int         beats;                     /* Waiting time after use     */
    char *      noun_damage;               /* Damage message             */
    char *      msg_off;                   /* Wear off message           */
};

/*
 * These are skill_lookup return values for common skills and spells.
 */
extern  int     gsn_backstab;
extern  int     gsn_berserk;                    /* by Thelonius */
extern  int     gsn_bladethirst;                /* by Maniac    */
extern  int     gsn_circle;                     /* by Thelonius */
extern  int     gsn_breathe_water;              /* by Thelonius */
extern  int     gsn_burning_hands;
extern  int     gsn_disarm;
extern  int     gsn_dodge;
extern  int     gsn_hide;
extern	int	gsn_hunt;			/* by Maniac	*/
extern	int	gsn_brew;
extern	int	gsn_scribe;
extern	int	gsn_setkill;			/* by Maniac	*/
extern  int     gsn_peek;
extern  int     gsn_pick_lock;
extern  int     gsn_poison_weapon;              /* by Thelonius */
extern  int     gsn_scrolls;                    /* by Binky / Thelonius */
extern  int     gsn_snare;                      /* by Binky / Thelonius */
extern  int     gsn_sneak;
extern  int     gsn_staves;                     /* by Binky / Thelonius */
extern  int     gsn_steal;
extern  int     gsn_untangle;                   /* by Thelonius */
extern  int     gsn_wands;                      /* by Binky / Thelonius */

extern  int     gsn_bash;  
extern  int     gsn_dual;                       /* by Thelonius */
extern  int     gsn_enhanced_damage;
extern  int     gsn_kick;
extern  int     gsn_parry;
extern  int     gsn_rescue;
extern  int     gsn_second_attack;
extern  int     gsn_third_attack;

extern  int     gsn_blindness;
extern  int     gsn_charm_person;
extern  int     gsn_curse;
extern  int     gsn_invis;
extern  int     gsn_mass_invis;
extern  int     gsn_mute;                       /* by Thelonius */
extern  int     gsn_poison;
extern  int     gsn_plague;                     /* by Maniac    */
extern  int     gsn_sleep;
extern  int     gsn_turn_undead;
extern	int	gsn_lore;


/*
 * Psionicist gsn's (by Thelonius).
 */
extern  int     gsn_chameleon;
extern  int     gsn_domination;
extern  int     gsn_heighten;
extern  int     gsn_shadow;


extern  int     gsn_stake;

/*
 * Race gsn's (by Kahn).
 */
extern  int     gsn_vampiric_bite;

/*
 * Utility macros.
 */
#define UMIN( a, b )            ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define UMAX( a, b )            ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define URANGE( a, b, c )       ( ( b ) < ( a ) ? ( a )                       \
						: ( ( b ) > ( c ) ? ( c )     \
								  : ( b ) ) )
#define LOWER( c )              ( ( c ) >= 'A' && ( c ) <= 'Z'                \
						? ( c ) + 'a' - 'A' : ( c ) )
#define UPPER( c )              ( ( c ) >= 'a' && ( c ) <= 'z'                \
						? ( c ) + 'A' - 'a' : ( c ) )
#define IS_SET( flag, bit )     ( ( flag ) &   ( bit ) )
#define SET_BIT( var, bit )     ( ( var )  |=  ( bit ) )
#define REMOVE_BIT( var, bit )  ( ( var )  &= ~( bit ) )
#define TOGGLE_BIT( var, bit )  ( ( var )  ^=  ( bit ) )
#define IS_QUESTOR(ch)		( IS_SET((ch)->act, PLR_QUESTOR))
#define MOB_GAME_CHEAT(mob)	( mob->pIndexData->ac )
#define WCHAN(ch, chan)         ( IS_SET(ch->pcdata->wiznet, chan) )
#define PLAYED(ch)		( ch->played + (int) ( current_time - ch->logon ) )




/*
 * Character macros.
 */
#define IS_NPC( ch )            ( IS_SET( ( ch )->act, ACT_IS_NPC ) )
#define IS_PC( ch )		( !IS_SET( ( ch )->act, ACT_IS_NPC ) )
#define IS_IMMORTAL( ch )       ( get_trust( ch ) >= LEVEL_IMMORTAL )
#define IS_HERO( ch )           ( get_trust( ch ) >= LEVEL_HERO     )
#define IS_AFFECTED( ch, sn )   ( IS_SET( ( ch )->affected_by, ( sn ) ) )

#define IS_GOOD( ch )           ( ch->alignment >=  350 )
#define IS_EVIL( ch )           ( ch->alignment <= -350 )
#define IS_NEUTRAL( ch )        ( !IS_GOOD( ch ) && !IS_EVIL( ch ) )

#define IS_AWAKE( ch )          ( ch->position > POS_SLEEPING )
#define GET_AC( ch )            ( ( ch )->armor                              \
				    + ( IS_AWAKE( ch )                       \
				    ? dex_app[get_curr_dex( ch )].defensive  \
				    : 0 ) )

#define IS_OUTSIDE( ch )        ( !IS_SET(                                   \
				    ( ch )->in_room->room_flags,             \
				    ROOM_INDOORS ) )

#define WAIT_STATE( ch, pulse ) ( ( ch )->wait = UMAX( ( ch )->wait,         \
						      ( pulse ) ) )

#define MANA_COST( ch, sn )     ( IS_NPC( ch ) ? 0 : UMAX (                  \
				skill_table[sn].min_mana,                    \
				100 / ( 2 + UMAX ( 0, ch->level -                      \
				skill_table[sn].skill_level[ch->class] ) ) ) )

#define IS_SWITCHED( ch )       ( ch->pcdata->switched )

/*
 * Object macros.
 */
#define CAN_WEAR( obj, part )   ( IS_SET( ( obj)->wear_flags,  ( part ) ) )
#define IS_OBJ_STAT( obj, stat )( IS_SET( ( obj)->extra_flags, ( stat ) ) )



/*
 * Description macros.
 */
#define PERS( ch, looker )      ( can_see( looker, ( ch ) ) ?                \
				( IS_NPC( ch ) ? ( ch )->short_descr         \
					       : ( ch )->name ) : "someone" )



/*
 * Structure for a command in the command lookup table.
 */
struct  cmd_type
{
    char * const        name;
    DO_FUN *            do_fun;
    int                 position;
    int                 level;
    int                 log;
};



/*
 * Structure for a social in the socials table.
 */
struct  social_type
{
    char          * name;
    char          * char_no_arg;
    char          * others_no_arg;
    char          * char_found;
    char          * others_found;
    char          * vict_found;
    char          * char_auto;
    char          * others_auto;
};

struct  mob_prog_act_list
{
    MPROG_ACT_LIST * next;
    char *           buf;
    CHAR_DATA *      ch;
    OBJ_DATA *       obj;
    void *           vo;
};

struct  mob_prog_data
{
    MPROG_DATA *next;
    int         type;
    char *      arglist;
    char *      comlist;
};

bool    MOBtrigger;

#define ERROR_PROG		-1
#define IN_FILE_PROG		BIT_0
#define ACT_PROG		BIT_1
#define SPEECH_PROG		BIT_2
#define RAND_PROG		BIT_3
#define FIGHT_PROG		BIT_4
#define DEATH_PROG		BIT_5
#define HITPRCNT_PROG		BIT_6
#define ENTRY_PROG		BIT_7
#define GREET_PROG		BIT_8
#define ALL_GREET_PROG		BIT_9
#define GIVE_PROG		BIT_10
#define BRIBE_PROG		BIT_11



/*
 * Global constants.
 */
extern  const   struct  str_app_type    str_app         [ 26 ];
extern  const   struct  int_app_type    int_app         [ 26 ];
extern  const   struct  wis_app_type    wis_app         [ 26 ];
extern  const   struct  dex_app_type    dex_app         [ 26 ];
extern  const   struct  con_app_type    con_app         [ 26 ];

extern  const   struct  class_type      class_table     [ MAX_CLASS   ];
extern	const	struct	clan_type	clan_table	[ ];
extern	const	struct	mana_type	mana_table	[ ];
extern  const   struct  cmd_type        cmd_table       [ ];
extern  const   struct  liq_type        liq_table       [ LIQ_MAX     ];
extern  const   struct  skill_type      skill_table     [ MAX_SKILL   ];
extern	const	struct	lang_type	lang_table	[ MAX_LANGUAGE ];
extern          struct  social_type    *social_table;
extern  char *  const                   title_table     [ MAX_CLASS   ]
							[ MAX_LEVEL+1 ]
							[ 2 ];
extern  const   struct  race_type       race_table      [ MAX_RACE ];
extern	int	const			bitvalues	[ 31 ];



/*
 * Global variables.
 */
extern          HELP_DATA         *     help_first;
extern          SHOP_DATA         *     shop_first;

extern          BAN_DATA          *     ban_list;
extern          CHAR_DATA         *     char_list;
extern          DESCRIPTOR_DATA   *     descriptor_list;
extern          NOTE_DATA         *     note_list;
extern          OBJ_DATA          *     object_list;

extern          AFFECT_DATA       *     affect_free;
extern          BAN_DATA          *     ban_free;
extern          CHAR_DATA         *     char_free;
extern          DESCRIPTOR_DATA   *     descriptor_free;
extern          EXTRA_DESCR_DATA  *     extra_descr_free;
extern		ROOM_MANA_DATA    *	room_mana_free;
extern          NOTE_DATA         *     note_free;
extern          OBJ_DATA          *     obj_free;
extern          PC_DATA           *     pcdata_free;
extern		ROOM_INDEX_DATA	  *	room_index_hash	[ MAX_KEY_HASH ];
extern          AUCTION_DATA      *     auction;

extern          char                    bug_buf         [ ];
extern          time_t                  current_time;
extern          bool                    fLogAll;
extern		bool			fLogAllMobs;
extern          FILE *                  fpReserve;
extern          KILL_DATA               kill_table      [ ];
extern          char                    log_buf         [ ];
extern          TIME_INFO_DATA          time_info;
extern          WEATHER_DATA            weather_info;
extern          time_t                  down_time;
extern          time_t                  warning1;
extern          time_t                  warning2;
extern          bool                    Reboot;
extern		int			share_value;
extern		int			pulse_db_dump;


/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN( do_advance      );
DECLARE_DO_FUN( do_afk          );
DECLARE_DO_FUN( do_allow        );
DECLARE_DO_FUN( do_answer       );
DECLARE_DO_FUN( do_areas        );
DECLARE_DO_FUN( do_at           );
DECLARE_DO_FUN( do_auction      );
DECLARE_DO_FUN( do_auto         );
DECLARE_DO_FUN( do_autoexit     );
DECLARE_DO_FUN( do_autoloot     );
DECLARE_DO_FUN( do_autogold     );
DECLARE_DO_FUN( do_autosac      );
DECLARE_DO_FUN( do_backstab     );
DECLARE_DO_FUN( do_bamfin       );
DECLARE_DO_FUN( do_bamfout      );
DECLARE_DO_FUN( do_ban          );
DECLARE_DO_FUN( do_bash         );
DECLARE_DO_FUN( do_beep		);		/* by Canth */
DECLARE_DO_FUN( do_berserk      );              /* by Thelonius */
DECLARE_DO_FUN( do_bet          );              /* by Thelonius */
DECLARE_DO_FUN( do_bladethirst  );              /* by Maniac    */
DECLARE_DO_FUN( do_blank        );
DECLARE_DO_FUN( do_brandish     );
DECLARE_DO_FUN( do_brief        );
DECLARE_DO_FUN( do_bug          );
DECLARE_DO_FUN( do_buy          );
DECLARE_DO_FUN( do_cast         );
DECLARE_DO_FUN( do_cdonate	);
DECLARE_DO_FUN( do_chameleon    );              /* by Thelonius */
DECLARE_DO_FUN( do_channels     );
DECLARE_DO_FUN( do_chat         );
DECLARE_DO_FUN( do_circle       );              /* by Thelonius */
DECLARE_DO_FUN( do_clone	);
DECLARE_DO_FUN( do_close        );
DECLARE_DO_FUN( do_colour	);
DECLARE_DO_FUN( do_combine      );
DECLARE_DO_FUN( do_commands     );
DECLARE_DO_FUN( do_compare      );
DECLARE_DO_FUN( do_config       );
DECLARE_DO_FUN( do_consider     );
DECLARE_DO_FUN( do_credits      );
DECLARE_DO_FUN( do_deny         );
DECLARE_DO_FUN( do_description  );
DECLARE_DO_FUN( do_disarm       );
DECLARE_DO_FUN( do_disconnect   );
DECLARE_DO_FUN( do_donate       );
DECLARE_DO_FUN( do_down         );
DECLARE_DO_FUN( do_drink        );
DECLARE_DO_FUN( do_drop         );
DECLARE_DO_FUN( do_east         );
DECLARE_DO_FUN( do_eat          );
DECLARE_DO_FUN( do_echo         );
DECLARE_DO_FUN( do_emote        );
DECLARE_DO_FUN( do_equipment    );
DECLARE_DO_FUN( do_examine      );
DECLARE_DO_FUN( do_exits        );
DECLARE_DO_FUN( do_fee          );
DECLARE_DO_FUN( do_feed         );
DECLARE_DO_FUN( do_fill         );
DECLARE_DO_FUN( do_flee         );
DECLARE_DO_FUN( do_follow       );
DECLARE_DO_FUN( do_force        );
DECLARE_DO_FUN( do_freeze       );
DECLARE_DO_FUN( do_get          );
DECLARE_DO_FUN( do_give         );
DECLARE_DO_FUN( do_goto         );
DECLARE_DO_FUN( do_group        );
DECLARE_DO_FUN( do_gtell        );
DECLARE_DO_FUN( do_heal         );              /* by Maniac    */
DECLARE_DO_FUN( do_heighten     );              /* by Thelonius */
DECLARE_DO_FUN( do_help         );
DECLARE_DO_FUN( do_hide         );
DECLARE_DO_FUN( do_holylight    );
DECLARE_DO_FUN( do_hunt		);		/* by Maniac	*/
DECLARE_DO_FUN( do_idea         );
DECLARE_DO_FUN( do_immtalk      );
DECLARE_DO_FUN( do_imtlset      );
DECLARE_DO_FUN( do_inventory    );
DECLARE_DO_FUN( do_invis        );
DECLARE_DO_FUN( do_kick         );
DECLARE_DO_FUN( do_kill         );
DECLARE_DO_FUN( do_list         );
DECLARE_DO_FUN( do_lock         );
DECLARE_DO_FUN( do_log          );
DECLARE_DO_FUN( do_look         );
DECLARE_DO_FUN( do_memory       );
DECLARE_DO_FUN( do_mfind        );
DECLARE_DO_FUN( do_mload        );
DECLARE_DO_FUN( do_mset         );
DECLARE_DO_FUN( do_mstat        );
DECLARE_DO_FUN( do_mwhere       );
DECLARE_DO_FUN( do_pwhere	);
DECLARE_DO_FUN( do_murder       );
DECLARE_DO_FUN( do_music        );
DECLARE_DO_FUN( do_newlock      );
DECLARE_DO_FUN( do_noemote      );
DECLARE_DO_FUN( do_north        );
DECLARE_DO_FUN( do_note         );
DECLARE_DO_FUN( do_notell       );
DECLARE_DO_FUN( do_notestat	);
DECLARE_DO_FUN( do_numlock      );
DECLARE_DO_FUN( do_ofind        );
DECLARE_DO_FUN( do_oload        );
DECLARE_DO_FUN( do_open         );
DECLARE_DO_FUN( do_order        );
DECLARE_DO_FUN( do_oset         );
DECLARE_DO_FUN( do_ostat        );
DECLARE_DO_FUN( do_owhere       );
DECLARE_DO_FUN( do_pecho        );
DECLARE_DO_FUN( do_pagelen      );
DECLARE_DO_FUN( do_pardon       );
DECLARE_DO_FUN( do_password     );
DECLARE_DO_FUN( do_peace        );
DECLARE_DO_FUN( do_pick         );
DECLARE_DO_FUN( do_poison_weapon);              /* by Thelonius */
DECLARE_DO_FUN( do_pose         );
DECLARE_DO_FUN( do_practice     );
DECLARE_DO_FUN( do_prompt       );
DECLARE_DO_FUN( do_purge        );
DECLARE_DO_FUN( do_put          );
DECLARE_DO_FUN( do_quaff        );
DECLARE_DO_FUN( do_quest	);
DECLARE_DO_FUN( do_question     );
DECLARE_DO_FUN( do_qui          );
DECLARE_DO_FUN( do_quit         );
DECLARE_DO_FUN( do_reboo        );
DECLARE_DO_FUN( do_reboot       );
DECLARE_DO_FUN( do_recall       );
DECLARE_DO_FUN( do_recho        );
DECLARE_DO_FUN( do_recite       );
DECLARE_DO_FUN( do_register     );
DECLARE_DO_FUN( do_remove       );
DECLARE_DO_FUN( do_rent         );
DECLARE_DO_FUN( do_reply        );
DECLARE_DO_FUN( do_report       );
DECLARE_DO_FUN( do_rescue       );
DECLARE_DO_FUN( do_rest         );
DECLARE_DO_FUN( do_restore      );
DECLARE_DO_FUN( do_return       );
DECLARE_DO_FUN( do_rset         );
DECLARE_DO_FUN( do_rstat        );
DECLARE_DO_FUN( do_sacrifice    );
DECLARE_DO_FUN( do_save         );
DECLARE_DO_FUN( do_say          );
DECLARE_DO_FUN( do_score        );
DECLARE_DO_FUN( do_sell         );
DECLARE_DO_FUN( do_set		);
DECLARE_DO_FUN( do_shadow       );              /* by Thelonius */
DECLARE_DO_FUN( do_shout        );
DECLARE_DO_FUN( do_shutdow      );
DECLARE_DO_FUN( do_shutdown     );
DECLARE_DO_FUN( do_silence      );
DECLARE_DO_FUN( do_sla          );
DECLARE_DO_FUN( do_slay         );
DECLARE_DO_FUN( do_sleep        );
DECLARE_DO_FUN( do_slist        );
DECLARE_DO_FUN( do_slookup      );
DECLARE_DO_FUN( do_snare        );              /* by Binky / Thelonius */
DECLARE_DO_FUN( do_sneak        );
DECLARE_DO_FUN( do_snoop        );
DECLARE_DO_FUN( do_socials      );
DECLARE_DO_FUN( do_south        );
DECLARE_DO_FUN( do_spells       );
DECLARE_DO_FUN( do_split        );
DECLARE_DO_FUN( do_sset         );
DECLARE_DO_FUN( do_sstime       );
DECLARE_DO_FUN( do_stake        );
DECLARE_DO_FUN( do_stand        );
DECLARE_DO_FUN( do_steal        );
DECLARE_DO_FUN( do_switch       );
DECLARE_DO_FUN( do_tell         );
DECLARE_DO_FUN( do_time         );
DECLARE_DO_FUN( do_title        );
DECLARE_DO_FUN( do_train        );
DECLARE_DO_FUN( do_transfer     );
DECLARE_DO_FUN( do_trust        );
DECLARE_DO_FUN( do_typo         );
DECLARE_DO_FUN( do_unlock       );
DECLARE_DO_FUN( do_untangle     );              /* by Thelonius */
DECLARE_DO_FUN( do_up           );
DECLARE_DO_FUN( do_users        );
DECLARE_DO_FUN( do_value        );
DECLARE_DO_FUN( do_visible      );
DECLARE_DO_FUN( do_wake         );
DECLARE_DO_FUN( do_wear         );
DECLARE_DO_FUN( do_weather      );
DECLARE_DO_FUN( do_west         );
DECLARE_DO_FUN( do_where        );
DECLARE_DO_FUN( do_who          );
DECLARE_DO_FUN( do_whois        );
DECLARE_DO_FUN( do_wimpy        );
DECLARE_DO_FUN( do_wizhelp      );
DECLARE_DO_FUN( do_wizify       );
DECLARE_DO_FUN( do_wizlist      );
DECLARE_DO_FUN( do_wizlock      );
DECLARE_DO_FUN( do_yell         );
DECLARE_DO_FUN( do_zap          );

/* ADDITIONS BY The Maniac */

DECLARE_DO_FUN( do_bank		);	/* Mud economy			*/
DECLARE_DO_FUN( do_update	);	/* Update game functions	*/
DECLARE_DO_FUN( do_count	);	/* Count players on mud		*/
DECLARE_DO_FUN( do_dog          );
DECLARE_DO_FUN( do_iscore       );
DECLARE_DO_FUN( do_lore		);
DECLARE_DO_FUN( do_outfit	);	/* Get initial items... */
DECLARE_DO_FUN( do_setkill	);	/* Set killer/thief flags */
DECLARE_DO_FUN( do_stat		);
DECLARE_DO_FUN( do_smite        );
DECLARE_DO_FUN( do_sstat        );
DECLARE_DO_FUN( do_stat		);
DECLARE_DO_FUN( do_string	);
DECLARE_DO_FUN( do_scan         );
DECLARE_DO_FUN( do_vnum		);
DECLARE_DO_FUN( do_wizpwd	);	/* Edit players' passwords */
DECLARE_DO_FUN( do_brew		);
DECLARE_DO_FUN( do_scribe	);
DECLARE_DO_FUN( do_area_count	);
DECLARE_DO_FUN(	do_hide_obj	);	/* Hiding objects */
DECLARE_DO_FUN( do_search	);	/* Search for hidden items */
DECLARE_DO_FUN( do_lose		);	/* lose your followers */
DECLARE_DO_FUN( do_delet	);	/* delete pfile securety */
DECLARE_DO_FUN( do_delete	);	/* delete playerfile */
DECLARE_DO_FUN( do_study	);	/* Studie spells from scrolls */
DECLARE_DO_FUN(	do_clantalk	);	/* Clan talk */
DECLARE_DO_FUN( do_clanrecall	);	/* Clan Recall shortcut to new recall */
DECLARE_DO_FUN(	do_guild	);	/* Change chars clan options */
DECLARE_DO_FUN(	do_clan		);	/* Clan command, manipulate clans */
DECLARE_DO_FUN( do_wiznet	);	/* Wiznet stuff */
DECLARE_DO_FUN( do_remort	);	/* Remorting */

DECLARE_DO_FUN( do_mpasound     );
DECLARE_DO_FUN( do_mpat         );
DECLARE_DO_FUN( do_mpecho       );
DECLARE_DO_FUN( do_mpechoaround );
DECLARE_DO_FUN( do_mpechoat     );
DECLARE_DO_FUN( do_mpforce      );
DECLARE_DO_FUN( do_mpgoto       );
DECLARE_DO_FUN( do_mpjunk       );
DECLARE_DO_FUN( do_mpkill       );
DECLARE_DO_FUN( do_mpmload      );
DECLARE_DO_FUN( do_mpoload      );
DECLARE_DO_FUN( do_mppurge      );
DECLARE_DO_FUN( do_mpstat       );
DECLARE_DO_FUN( do_mptransfer   );


/* Additions by The Maniac for Languages */

DECLARE_DO_FUN( do_common	);
DECLARE_DO_FUN( do_human	);
DECLARE_DO_FUN( do_dwarvish	);
DECLARE_DO_FUN( do_elvish	);
DECLARE_DO_FUN( do_gnomish	);
DECLARE_DO_FUN( do_goblin	);
DECLARE_DO_FUN( do_orcish	);
DECLARE_DO_FUN( do_ogre		);
DECLARE_DO_FUN( do_drow		);
DECLARE_DO_FUN(	do_kobold	);
DECLARE_DO_FUN( do_trollish	);
DECLARE_DO_FUN( do_hobbit	);
DECLARE_DO_FUN( do_lstat	);	/* language stat */
DECLARE_DO_FUN( do_lset		);	/* Language set */
DECLARE_DO_FUN( do_learn	);	/* learn a language */
DECLARE_DO_FUN( do_speak	);	/* Select a language to speak */

/* Additions by Canth */
DECLARE_DO_FUN( do_sedit	);	/* social editor */
DECLARE_DO_FUN( do_disable	);	/* disabler (in interp.c) */
DECLARE_DO_FUN( do_for		);	/* super AT function */
DECLARE_DO_FUN( do_rename	);	/* rename players */
DECLARE_DO_FUN( do_exlist	);	/* Find exits between areas */
DECLARE_DO_FUN( do_objlist	);	/* Dumps list of objects */
DECLARE_DO_FUN( do_enter	);	/* Entering portqals */

DECLARE_DO_FUN( do_marry	);	/* Marry 2 ppl - Canth */
DECLARE_DO_FUN( do_divorce	);	/* Divorce 2 ppl - Canth */
DECLARE_DO_FUN( do_wed		);	/* Married comm channel - Canth */
DECLARE_DO_FUN( do_gospouse	);	/* Goto spouse - Canth */
DECLARE_DO_FUN( do_getspouse	);	/* Trans spouse - Canth */
DECLARE_DO_FUN( do_rings	);	/* Make rings 4 married ppl - Canth */


/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(      spell_null              );
DECLARE_SPELL_FUN(      spell_acid_blast        );
DECLARE_SPELL_FUN(	spell_age		);	/* Maniac and Canth */
DECLARE_SPELL_FUN(      spell_animate_dead      );
DECLARE_SPELL_FUN(      spell_armor             );
DECLARE_SPELL_FUN(      spell_bless             );
DECLARE_SPELL_FUN(      spell_blindness         );
DECLARE_SPELL_FUN(      spell_burning_hands     );
DECLARE_SPELL_FUN(      spell_breathe_water     );      /* by Thelonius */
DECLARE_SPELL_FUN(      spell_call_lightning    );
DECLARE_SPELL_FUN(      spell_cause_critical    );
DECLARE_SPELL_FUN(      spell_cause_light       );
DECLARE_SPELL_FUN(      spell_cause_serious     );
DECLARE_SPELL_FUN(      spell_change_sex        );
DECLARE_SPELL_FUN(      spell_charm_person      );
DECLARE_SPELL_FUN(      spell_chill_touch       );
DECLARE_SPELL_FUN(      spell_colour_spray      );
DECLARE_SPELL_FUN(      spell_cone_of_silence   );      /* by Thelonius */
DECLARE_SPELL_FUN(      spell_continual_light   );
DECLARE_SPELL_FUN(      spell_control_weather   );
DECLARE_SPELL_FUN(      spell_create_food       );
DECLARE_SPELL_FUN(	spell_create_portal	);	/* by Canth */
DECLARE_SPELL_FUN(      spell_create_spring     );
DECLARE_SPELL_FUN(      spell_create_water      );
DECLARE_SPELL_FUN(      spell_cure_blindness    );
DECLARE_SPELL_FUN(      spell_cure_disease      );      /* by Maniac    */
DECLARE_SPELL_FUN(      spell_cure_critical     );
DECLARE_SPELL_FUN(      spell_cure_light        );
DECLARE_SPELL_FUN(      spell_cure_poison       );
DECLARE_SPELL_FUN(      spell_cure_serious      );
DECLARE_SPELL_FUN(      spell_curse             );
DECLARE_SPELL_FUN(      spell_destroy_cursed    );
DECLARE_SPELL_FUN(      spell_detect_evil       );
DECLARE_SPELL_FUN(      spell_detect_hidden     );
DECLARE_SPELL_FUN(      spell_detect_invis      );
DECLARE_SPELL_FUN(      spell_detect_magic      );
DECLARE_SPELL_FUN(      spell_detect_poison     );
DECLARE_SPELL_FUN(      spell_dispel_evil       );
DECLARE_SPELL_FUN(      spell_dispel_magic      );      /* by Thelonius */
DECLARE_SPELL_FUN(      spell_earthquake        );
DECLARE_SPELL_FUN(      spell_enchant_weapon    );
DECLARE_SPELL_FUN(      spell_energy_drain      );
DECLARE_SPELL_FUN(      spell_exorcise          );
DECLARE_SPELL_FUN(      spell_faerie_fire       );
DECLARE_SPELL_FUN(      spell_faerie_fog        );
DECLARE_SPELL_FUN(      spell_fireball          );
DECLARE_SPELL_FUN(      spell_flamestrike       );
DECLARE_SPELL_FUN(      spell_flaming           );
DECLARE_SPELL_FUN(      spell_fly               );
DECLARE_SPELL_FUN(      spell_gate              );
DECLARE_SPELL_FUN(      spell_general_purpose   );
DECLARE_SPELL_FUN(      spell_giant_strength    );
DECLARE_SPELL_FUN(      spell_harm              );
DECLARE_SPELL_FUN(      spell_heal              );
DECLARE_SPELL_FUN(      spell_high_explosive    );
DECLARE_SPELL_FUN(      spell_identify          );
DECLARE_SPELL_FUN(      spell_infravision       );
DECLARE_SPELL_FUN(      spell_invis             );
DECLARE_SPELL_FUN(      spell_know_alignment    );
DECLARE_SPELL_FUN(      spell_lightning_bolt    );
DECLARE_SPELL_FUN(      spell_locate_object     );
DECLARE_SPELL_FUN(      spell_magic_missile     );
DECLARE_SPELL_FUN(      spell_mass_heal         );
DECLARE_SPELL_FUN(      spell_mass_invis        );
DECLARE_SPELL_FUN(      spell_mute              );      /* by Thelonius */
DECLARE_SPELL_FUN(      spell_pass_door         );
DECLARE_SPELL_FUN(      spell_plague            );	/* by Maniac	*/
DECLARE_SPELL_FUN(      spell_poison            );
DECLARE_SPELL_FUN(      spell_polymorph_other   );
DECLARE_SPELL_FUN(      spell_protection        );
DECLARE_SPELL_FUN(      spell_recharge_item     );      /* by Thelonius */
DECLARE_SPELL_FUN(      spell_refresh           );
DECLARE_SPELL_FUN(	spell_rejuvenate	);	/* Maniac and Canth */
DECLARE_SPELL_FUN(      spell_remove_alignment  );      /* by Thelonius */
DECLARE_SPELL_FUN(      spell_remove_curse      );
DECLARE_SPELL_FUN(      spell_remove_silence    );      /* by Thelonius */
DECLARE_SPELL_FUN(      spell_sanctuary         );
DECLARE_SPELL_FUN(      spell_shocking_grasp    );
DECLARE_SPELL_FUN(      spell_shield            );
DECLARE_SPELL_FUN(      spell_sleep             );
DECLARE_SPELL_FUN(      spell_stone_skin        );
DECLARE_SPELL_FUN(      spell_summon            );
DECLARE_SPELL_FUN(      spell_teleport          );
DECLARE_SPELL_FUN(      spell_turn_undead       );
DECLARE_SPELL_FUN(      spell_ventriloquate     );
DECLARE_SPELL_FUN(      spell_weaken            );
DECLARE_SPELL_FUN(      spell_word_of_recall    );
DECLARE_SPELL_FUN(      spell_acid_breath       );
DECLARE_SPELL_FUN(      spell_fire_breath       );
DECLARE_SPELL_FUN(      spell_frost_breath      );
DECLARE_SPELL_FUN(      spell_gas_breath        );
DECLARE_SPELL_FUN(      spell_lightning_breath  );
DECLARE_SPELL_FUN(      spell_vampiric_bite     );

/*
 * Psi spell_functions, in magic.c (by Thelonius).
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

/* Spell declares for necromancer spells
 * Spells written mostly by Maniac from Mythran Mud
 */
DECLARE_SPELL_FUN(	spell_rock_flesh	);
DECLARE_SPELL_FUN(	spell_summon_dead	);
DECLARE_SPELL_FUN(	spell_call_quake	);

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if     defined( _AIX )
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined( apollo )
int     atoi            args( ( const char *string ) );
void *  calloc          args( ( unsigned nelem, size_t size ) );
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined( hpux )
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined( linux )
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined( macintosh )
#define NOCRYPT
#if     defined( unix )
#undef  unix
#endif
#endif

#if     defined( MIPS_OS )
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined( MSDOS )
#define NOCRYPT
#if     defined( unix )
#undef  unix
#endif
#endif

#if     defined( NeXT )
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined( sequent )
char *  crypt           args( ( const char *key, const char *salt ) );
int     fclose          args( ( FILE *stream ) );
int     fprintf         args( ( FILE *stream, const char *format, ... ) );
int     fread           args( ( void *ptr, int size, int n, FILE *stream ) );
int     fseek           args( ( FILE *stream, long offset, int ptrname ) );
void    perror          args( ( const char *s ) );
int     ungetc          args( ( int c, FILE *stream ) );
#endif

#if     defined( sun )
char *  crypt           args( ( const char *key, const char *salt ) );
int     fclose          args( ( FILE *stream ) );
int     fprintf         args( ( FILE *stream, const char *format, ... ) );
size_t  fread           args( ( void *ptr, size_t size, size_t nitems,
			       FILE *stream ) );
int     fseek           args( ( FILE *stream, long offset, int ptrname ) );
void    perror          args( ( const char *s ) );
int     ungetc          args( ( int c, FILE *stream ) );
#endif

#if     defined( ultrix )
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

/*
 * Stuff for DEC UNIX on Alpha (OSF3.2C)
 * Fusion
 */
#if defined( _OSF_SOURCE )
char *  crypt           args( ( const char *key, const char *salt ) );
int     system          args( ( const char *string ) );
ssize_t read            args( ( int fd, void *buf, size_t nbyte ) );
ssize_t write           args( ( int fd, const void *buf, size_t nbyte ) );
int     close           args( ( int fd ) );
#endif


/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if     defined( NOCRYPT )
#define crypt( s1, s2 ) ( s1 )
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
#define PLAYER_DIR      ""              /* Player files                 */
#define NULL_FILE       "proto.are"     /* To reserve one stream        */
#define MOB_DIR         ""		/* MOBProg files                */
#endif

#if defined( MSDOS )
#define PLAYER_DIR      "../player/"    /* Player files                 */
#define NULL_FILE       "nul"           /* To reserve one stream        */
#define MOB_DIR         ""		/* MOBProg files                */
#endif

#if defined( unix )
#define PLAYER_DIR      "../player/"    /* Player files                 */
#define NULL_FILE       "/dev/null"     /* To reserve one stream        */
#define MOB_DIR         "MOBProgs/"     /* MOBProg files                */
#endif

#if defined( linux )
#define PLAYER_DIR      "../player/"    /* Player files                 */
#define NULL_FILE       "/dev/null"     /* To reserve one stream        */
#define MOB_DIR         "MOBProgs/"     /* MOBProg files                */
#endif

#define AREA_LIST       "AREA.LST"      /* List of areas                */
#define MUSIC_FILE	"MUSIC.TXT"	/* For jukebox			*/
#define BUG_FILE        "BUGS.TXT"      /* For 'bug' and bug( )         */
#define IDEA_FILE       "IDEAS.TXT"     /* For 'idea'                   */
#define TYPO_FILE       "TYPOS.TXT"     /* For 'typo'                   */
#define NOTE_FILE       "NOTES.TXT"     /* For 'notes'                  */
#define SHUTDOWN_FILE   "SHUTDOWN.TXT"  /* For 'shutdown'               */
#define DOWN_TIME_FILE  "TIME.TXT"      /* For automatic shutdown       */
#define BAN_FILE        "BAN.TXT"       /* For banned site save         */
#define BANK_FILE	"BANK.TXT"	/* For Bank Info		*/
#define DISABLED_FILE	"DISABLED.TXT"	/* For disabled commands	*/

/*****************************************************************************
 *                                    OLC                                    *
 *****************************************************************************/

/*
 * This structure is used in special.c to lookup spec funcs and
 * also in olc_act.c to display listings of spec funcs.
 */
struct spec_type
{
    char *      spec_name;
    SPEC_FUN *  spec_fun;
};

struct obj_spec_type
{
	char *		spec_name;
	OBJ_SPEC_FUN *	obj_fun;
};

struct game_type
{
    char *	game_name;
    GAME_FUN *  game_fun;
};

/*
 * This structure is used in bit.c to lookup flags and stats.
 */
struct flag_type
{
    char * name;
    int  bit;
    bool settable;
};

/*
 * Object defined in limbo.are
 * Used in save.c to load objects that don't exist.
 */
#define OBJ_VNUM_DUMMY  1



/*
 * Area flags.
 */
#define         AREA_NONE       BIT_0
#define         AREA_CHANGED    BIT_1       /* Area has been modified. */
#define         AREA_ADDED      BIT_2       /* Area has been added to. */
#define         AREA_LOADING    BIT_3       /* Used for counting in db.c */
#define		AREA_VERBOSE	BIT_4



#define MAX_DIR 6
#define NO_FLAG -99     /* Must not be used in flags or stats. */

/*
 * Interp.c
 */
DECLARE_DO_FUN( do_aedit        );      /* OLC 1.1b */
DECLARE_DO_FUN( do_redit        );      /* OLC 1.1b */
DECLARE_DO_FUN( do_oedit        );      /* OLC 1.1b */
DECLARE_DO_FUN( do_medit        );      /* OLC 1.1b */
DECLARE_DO_FUN( do_mpedit	);	/* OLC 1.1b addition */
DECLARE_DO_FUN( do_asave        );
DECLARE_DO_FUN( do_alist        );
DECLARE_DO_FUN( do_resets       );


/*
 * Global Constants
 */
extern  char *  const   dir_name        [];
extern  const   int     rev_dir         [];
extern  const   struct  spec_type       spec_table      [];
extern	const	struct	obj_spec_type	obj_spec_table	[];
extern	const	struct	game_type	game_table	[];

/*
 * Global variables
 */
extern          AREA_DATA *             area_first;
extern          AREA_DATA *             area_last;
extern          SHOP_DATA *             shop_last;

extern          int                     top_affect;
extern          int                     top_area;
extern          int                     top_ed;
extern          int                     top_exit;
extern          int                     top_help;
extern          int                     top_mob_index;
extern          int                     top_obj_index;
extern          int                     top_reset;
extern          int                     top_room;
extern		int			top_room_mana;
extern          int                     top_shop;

extern          int                     top_vnum_mob;
extern          int                     top_vnum_obj;
extern          int                     top_vnum_room;

extern          char                    str_empty       [1];
extern  MOB_INDEX_DATA *        mob_index_hash  [MAX_KEY_HASH];
extern  OBJ_INDEX_DATA *        obj_index_hash  [MAX_KEY_HASH];
extern  ROOM_INDEX_DATA *       room_index_hash [MAX_KEY_HASH];



/* db.c */
void    reset_area      args( ( AREA_DATA * pArea ) );
void    reset_room      args( ( ROOM_INDEX_DATA *pRoom ) );

/* string.c */
void    string_edit     args( ( CHAR_DATA *ch, char **pString ) );
void    string_append   args( ( CHAR_DATA *ch, char **pString ) );
char *  string_replace  args( ( char * orig, char * old, char * new ) );
void    string_add      args( ( CHAR_DATA *ch, char *argument ) );
char *  format_string   args( ( char *oldstring /*, bool fSpace */ ) );
char *  first_arg       args( ( char *argument, char *arg_first, bool fCase ) );char *  string_unpad    args( ( char * argument ) );
char *  string_proper   args( ( char * argument ) );

/* olc.c */
bool    run_olc_editor  args( ( DESCRIPTOR_DATA *d ) );
char    *olc_ed_name    args( ( CHAR_DATA *ch ) );
char	*olc_ed_vnum	args( ( CHAR_DATA *ch ) );

/* special.c */
char *  spec_string     args( ( SPEC_FUN *fun ) );      /* OLC */

/* games.c */
char *	game_string	args( ( GAME_FUN *fun ) );	/* OLC Maniac */

/* bit.c */
extern const struct flag_type   area_flags[];
extern const struct flag_type   sex_flags[];
extern const struct flag_type   exit_flags[];
extern const struct flag_type   door_resets[];
extern const struct flag_type   room_flags[];
extern const struct flag_type   sector_flags[];
extern const struct flag_type   type_flags[];
extern const struct flag_type   extra_flags[];
extern const struct flag_type   wear_flags[];
extern const struct flag_type   act_flags[];
extern const struct flag_type   affect_flags[];
extern const struct flag_type   apply_flags[];
extern const struct flag_type   wear_loc_strings[];
extern const struct flag_type   wear_loc_flags[];
extern const struct flag_type   weapon_flags[];
extern const struct flag_type   container_flags[];
extern const struct flag_type   liquid_flags[];
extern const struct flag_type	mprog_type_flags[];

/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD      CHAR_DATA
#define MID     MOB_INDEX_DATA
#define OD      OBJ_DATA
#define OID     OBJ_INDEX_DATA
#define RID     ROOM_INDEX_DATA
#define SF      SPEC_FUN
#define OF	OBJ_FUN
#define GF	GAME_FUN

/* act_comm.c */
void    add_follower    args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void    stop_follower   args( ( CHAR_DATA *ch ) );
void    die_follower    args( ( CHAR_DATA *ch, char *name ) );
bool    is_same_group   args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
bool    is_note_to      args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );

/* act_info.c */
void    set_title       args( ( CHAR_DATA *ch, char *title ) );
bool    check_blind     args( ( CHAR_DATA *ch ) );

/* act_move.c */
void    move_char       args( ( CHAR_DATA *ch, int door ) );
void	die_old_age	args( ( CHAR_DATA *ch ) );

/* act_obj.c */
bool    remove_obj      args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void	talk_auction	args( ( char *argument ) );

/* act_wiz.c */
ROOM_INDEX_DATA *find_location args ( ( CHAR_DATA *ch, char *arg ) );


/* comm.c */
void    close_socket     args( ( DESCRIPTOR_DATA *dclose ) );
void    write_to_buffer  args( ( DESCRIPTOR_DATA *d, const char *txt,
				int length ) );
void    send_to_room     args( ( const char *txt, ROOM_INDEX_DATA *room ) );
void    send_to_all_char args( ( const char *text ) );
void    send_to_char     args( ( const char *txt, CHAR_DATA *ch ) );
void    show_string      args( ( DESCRIPTOR_DATA *d, char *input ) );
void    act              args( ( const char *format, CHAR_DATA *ch,
				const void *arg1, const void *arg2,
				int type ) );
void	printf_to_char	args( ( CHAR_DATA *ch, char *fmt, ...) );
int	colour		args( ( char type, CHAR_DATA *ch, char *string ) );
void	colourconv	args( ( char *buffer, const char *txt, CHAR_DATA *ch ) );
void	send_to_char_bw	args( ( const char *txt, CHAR_DATA *ch ) );

/* db.c */
void    boot_db         args( ( void ) );
void    area_update     args( ( void ) );
CD *    create_mobile   args( ( MOB_INDEX_DATA *pMobIndex ) );
void	clone_mobile	args( ( CHAR_DATA *parent, CHAR_DATA *clone ) );
OD *    create_object   args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void	clone_object	args( ( OBJ_DATA *parent, OBJ_DATA *clone ) );
void    clear_char      args( ( CHAR_DATA *ch ) );
void    free_char       args( ( CHAR_DATA *ch ) );
char *  get_extra_descr args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *   get_mob_index   args( ( int vnum ) );
OID *   get_obj_index   args( ( int vnum ) );
RID *   get_room_index  args( ( int vnum ) );
char    fread_letter    args( ( FILE *fp ) );
int     fread_number    args( ( FILE *fp ) );
char *  fread_string    args( ( FILE *fp ) );
void    fread_to_eol    args( ( FILE *fp ) );
char *  fread_word      args( ( FILE *fp ) );
void *  alloc_mem       args( ( int sMem ) );
void *  alloc_perm      args( ( int sMem ) );
void    free_mem        args( ( void *pMem, int sMem ) );
char *  str_dup         args( ( const char *str ) );
void    free_string     args( ( char *pstr ) );
int     number_fuzzy    args( ( int number ) );
int     number_range    args( ( int from, int to ) );
int     number_percent  args( ( void ) );
int     number_door     args( ( void ) );
int     number_bits     args( ( int width ) );
int     number_mm       args( ( void ) );
int     dice            args( ( int number, int size ) );
int     interpolate     args( ( int level, int value_00, int value_32 ) );
void    smash_tilde     args( ( char *str ) );
bool    str_cmp         args( ( const char *astr, const char *bstr ) );
bool    str_prefix      args( ( const char *astr, const char *bstr ) );
bool    str_infix       args( ( const char *astr, const char *bstr ) );
bool    str_suffix      args( ( const char *astr, const char *bstr ) );
char *  capitalize      args( ( const char *str ) );
void    append_file     args( ( CHAR_DATA *ch, char *file, char *str ) );
void    bug             args( ( const char *str, int param ) );
void    log_string      args( ( const char *str ) );
void    tail_chain      args( ( void ) );
void	load_disabled	args( ( void ) );
void	save_disabled	args( ( void ) );

/* fight.c */
void    violence_update args( ( void ) );
void    multi_hit       args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    damage          args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			       int dt, int wpn ) );
void    raw_kill        args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    stop_fighting   args( ( CHAR_DATA *ch, bool fBoth ) );
void    update_pos      args( ( CHAR_DATA *victim ) );
void    check_killer         args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_safe              args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    licensed             args( ( CHAR_DATA *ch ) );
bool    registered           args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	death_cry	args( ( CHAR_DATA *ch ) );

/* handler.c */
int     get_trust       args( ( CHAR_DATA *ch ) );
int     get_age         args( ( CHAR_DATA *ch ) );
int     get_curr_str    args( ( CHAR_DATA *ch ) );
int     get_curr_int    args( ( CHAR_DATA *ch ) );
int     get_curr_wis    args( ( CHAR_DATA *ch ) );
int     get_curr_dex    args( ( CHAR_DATA *ch ) );
int     get_curr_con    args( ( CHAR_DATA *ch ) );
int     get_hitroll     args( ( CHAR_DATA *ch, int wpn ) );
int     get_damroll     args( ( CHAR_DATA *ch, int wpn ) );
int     can_carry_n     args( ( CHAR_DATA *ch ) );
int     can_carry_w     args( ( CHAR_DATA *ch ) );
bool    is_name         args( ( const char *str, char *namelist ) );
void    affect_to_char  args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_remove   args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_modify   args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
void    affect_strip    args( ( CHAR_DATA *ch, int sn ) );
bool    is_affected     args( ( CHAR_DATA *ch, int sn ) );
void    affect_join     args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    char_from_room  args( ( CHAR_DATA *ch ) );
void    char_to_room    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_char     args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    obj_from_char   args( ( OBJ_DATA *obj ) );
int     apply_ac        args( ( OBJ_DATA *obj, int iWear ) );
OD *    get_eq_char     args( ( CHAR_DATA *ch, int iWear ) );
void    equip_char      args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void    unequip_char    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int     count_obj_list  args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void    obj_from_room   args( ( OBJ_DATA *obj ) );
void    obj_to_room     args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_obj      args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void    obj_from_obj    args( ( OBJ_DATA *obj ) );
void    extract_obj     args( ( OBJ_DATA *obj ) );
void    extract_char    args( ( CHAR_DATA *ch, bool fPull ) );
CD *    get_char_room   args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_world  args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_area	args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_type    args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *    get_obj_list    args( ( CHAR_DATA *ch, char *argument,
			       OBJ_DATA *list ) );
OD *    get_obj_carry   args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_wear    args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_here    args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_room	args( (	CHAR_DATA *ch, char *argument )	);
OD *    get_obj_world   args( ( CHAR_DATA *ch, char *argument ) );
OD *    create_money    args( ( int amount ) );
int     get_obj_number  args( ( OBJ_DATA *obj ) );
int     get_obj_weight  args( ( OBJ_DATA *obj ) );
bool    room_is_dark    args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    room_is_private args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    can_see         args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    can_see_obj     args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool    can_drop_obj    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *  item_type_name  args( ( OBJ_DATA *obj ) );
char *  affect_loc_name args( ( int location ) );
char *  affect_bit_name args( ( int vector ) );
char *  extra_bit_name  args( ( int extra_flags ) );
char *	mob_act_name	args( ( int act ) );		/* The Maniac */
char *	plr_act_name	args( ( int act ) );		/* The Maniac */
char *	door_status_name args( ( int status ) );	/* The Maniac */
char *	room_flags_name	args( ( int vector ) );		/* The Maniac */
char *	str_wear_loc	args( ( int vector ) );		/* The Maniac */
char *	obj_eq_loc	args( ( int vector ) );		/* The Maniac */
char *	sector_string	args( ( int sector ) );		/* The Maniac */
char *	race_ab_string	args( ( int bit ) );		/* The Maniac */
char *  pos_string	args( ( CHAR_DATA *victim ) );	/* The Maniac */
char *  fur_pos_string	args( ( int pos ) );		/* The Maniac */
CD   *  get_char        args( ( CHAR_DATA *ch ) );
bool    longstring      args( ( CHAR_DATA *ch, char *argument ) );
bool    authorized      args( ( CHAR_DATA *ch, char *skllnm ) );
void    end_of_game     args( ( void ) );
int     race_lookup     args( ( const char *race ) );	/* Maniac */
int	race_lookup_prefix	args( ( const char *race ) ); /* Maniac */
int     affect_lookup   args( ( const char *race ) );
int	advatoi		args( ( const char *s ) );
int	count_users	args( ( OBJ_DATA *obj ) );

/* clan.c */
int	clan_lookup	args( ( const char * name) );
char *	clan_name	args( ( int clan ) );
int	clan_accept	args( ( CHAR_DATA *ch, int clan ) );
bool	is_same_clan	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_in_clan	args( ( CHAR_DATA *ch ) );
bool	is_clan_member	args( ( CHAR_DATA *ch, int vnum ) );
void	guild		args( ( CHAR_DATA *ch, char * argument) );

/* interp.c */
void    interpret       args( ( CHAR_DATA *ch, char *argument ) );
bool    is_number       args( ( char *arg ) );
int     number_argument args( ( char *argument, char *arg ) );
char *  one_argument    args( ( char *argument, char *arg_first ) );
bool    IS_SWITCHED     args( ( CHAR_DATA *ch ) );

/* magic.c */
int     skill_lookup    args( ( const char *name ) );
bool    saves_spell     args( ( int level, CHAR_DATA *victim ) );
void    obj_cast_spell  args( ( int sn, int level, CHAR_DATA *ch,
			       CHAR_DATA *victim, OBJ_DATA *obj ) );

/* save.c */
void    save_char_obj   args( ( CHAR_DATA *ch ) );
bool    load_char_obj   args( ( DESCRIPTOR_DATA *d, char *name ) );

/* special.c */
SF *    spec_lookup     args( ( const char *name ) );

/* games.c */
GF *	game_lookup	args( ( const char *name ) );

/* update.c */
void    advance_level   args( ( CHAR_DATA *ch ) );
void    demote_level    args( ( CHAR_DATA *ch ) );
void    gain_exp        args( ( CHAR_DATA *ch, int gain ) );
void    gain_condition  args( ( CHAR_DATA *ch, int iCond, int value ) );
void    update_handler  args( ( void ) );
void    ban_update      args( ( void ) );
void	bank_update	args( ( void ) );
void	auction_update	args( ( void ) );

/* hunt.c */
void    hunt_victim     args( ( CHAR_DATA *ch ) );

/* drunk.c */
char	*makedrunk	args( (char *string ,CHAR_DATA *ch) );

/* language.c */
void	do_language	args( ( CHAR_DATA *ch, char *argument, int language) );
int	lang_lookup	args( ( const char *name ) );

/* social-edit.c */
void save_social_table	( );
void load_social_table	( );

/* mob_commands.c */
char *	mprog_type_to_name	args ( ( int type ) );

/* wiznet.c */
void	wiznet		args ( ( CHAR_DATA *ch, int chan, int level, char * string ) );

#ifdef DUNNO_STRSTR
char *  strstr                  args ( (const char *s1, const char *s2 ) );
#endif

void    mprog_wordlist_check    args ( ( char * arg, CHAR_DATA *mob,
                                        CHAR_DATA* actor, OBJ_DATA* object,
                                        void* vo, int type ) );
void    mprog_percent_check     args ( ( CHAR_DATA *mob, CHAR_DATA* actor,
                                        OBJ_DATA* object, void* vo,
                                        int type ) );
void    mprog_act_trigger       args ( ( char* buf, CHAR_DATA* mob,
                                        CHAR_DATA* ch, OBJ_DATA* obj,
                                        void* vo ) );
void    mprog_bribe_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
                                        int amount ) );
void    mprog_entry_trigger     args ( ( CHAR_DATA* mob ) );
void    mprog_give_trigger      args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
                                        OBJ_DATA* obj ) );
void    mprog_greet_trigger     args ( ( CHAR_DATA* mob ) );
void    mprog_fight_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_hitprcnt_trigger  args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_death_trigger     args ( ( CHAR_DATA* mob ) );
void    mprog_random_trigger    args ( ( CHAR_DATA* mob ) );
void    mprog_speech_trigger    args ( ( char* txt, CHAR_DATA* mob ) );


#undef  CD
#undef  MID
#undef  OD
#undef  OID
#undef  RID
#undef  OF
#undef  SF
#undef  GF
