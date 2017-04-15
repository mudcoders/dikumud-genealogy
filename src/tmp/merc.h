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

/* To turn on and off certain features off the mud */
#define NEW_MONEY

#include "colors.h"   /* Include the ansi color routines. */

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
typedef struct	arena_data		ARENA_DATA;
typedef struct  	new_clan_data     CLAN_DATA;
typedef struct	ban_data		BAN_DATA;
/*typedef struct  userl_data         USERL_DATA;*/
typedef struct  	gskill_data       GSPELL_DATA;
typedef struct	char_data		CHAR_DATA;
typedef struct  	social_data		SOCIAL_DATA;
typedef struct  	race_data		RACE_DATA;
typedef struct	descriptor_data	DESCRIPTOR_DATA;
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
typedef struct	room_affect_data	ROOM_AFFECT_DATA;
typedef struct	powered_data		POWERED_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	shop_data		SHOP_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	weather_data		WEATHER_DATA;
typedef struct  mob_prog_data           MPROG_DATA;
typedef struct  mob_prog_act_list       MPROG_ACT_LIST;
typedef struct	guild_data		GUILD_DATA;	/* XOR */
typedef struct  quest_data              QUEST_DATA;     /* Angi */
typedef struct  alias_data              ALIAS_DATA;     /* Altrag */
/*typedef struct  phobia_data             PHOBIA_DATA; */   /* Altrag */
typedef struct  trap_data               TRAP_DATA;      /* Altrag */
typedef struct	playerlist_data		PLAYERLIST_DATA; /*Decklarean*/
typedef struct skill_type		SKILL_TYPE;
typedef struct newbie_data		NEWBIE_DATA; 	 /* Angi */

#ifdef NEW_MONEY
typedef struct money_data	 	MONEY_DATA;

#define S_PER_G				10
#define C_PER_S				10
#define C_PER_G				(S_PER_G * C_PER_S)

#define SILVER_PER_GOLD			10
#define COPPER_PER_SILVER		10
#define COPPER_PER_GOLD			(SILVER_PER_GOLD*COPPER_PER_SILVER)
#endif


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
#define	MAX_KEY_HASH		 8192/*1024*/
#define MAX_STRING_LENGTH	 8192
#define MAX_INPUT_LENGTH	  256



/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_SKILL		   334
#define MAX_GSPELL                 2
#define MAX_CLASS		   13
#define MAX_RACE                   16
#define MAX_CLAN                   21 /*max 20 clans + 1 for clan 0*/
#define MAX_LEVEL		   113
#define STUN_MAX                   5
#define L_IMP                      MAX_LEVEL
#define L_CON                     ( L_IMP - 1 )
#define L_DIR		          ( L_CON - 1 )
#define L_SEN		          ( L_DIR - 1 )
#define L_GOD                     ( L_SEN - 1 )
#define L_DEM                     ( L_GOD - 1 )
#define L_JUN	        	  ( L_DEM - 1 )
#define L_APP                     ( L_JUN - 1 )
#define L_CHAMP5		  ( L_APP - 1 )
#define L_CHAMP4		  ( L_CHAMP5 - 1 )
#define L_CHAMP3		  ( L_CHAMP4 - 1 )
#define L_CHAMP2		  ( L_CHAMP3 - 1 )
#define L_CHAMP1		  ( L_CHAMP2 - 1 )
#define LEVEL_HERO		  1100

#define LEVEL_IMMORTAL		  1106
#define LEVEL_MORTAL 		  1105

#define PULSE_PER_SECOND	    6
#define PULSE_VIOLENCE		  (  3 * PULSE_PER_SECOND )
#define PULSE_MOBILE		  (  4 * PULSE_PER_SECOND )
#define PULSE_TICK		  ( 40 * PULSE_PER_SECOND )
#define PULSE_AREA		  ( 80 * PULSE_PER_SECOND )

/* Save the database - OLC 1.1b */
#define PULSE_DB_DUMP		  (1800* PULSE_PER_SECOND ) /* 30 minutes  */

/* Not used -Deck
#define SIZE_TINY                 0
#define SIZE_SMALL                1
#define SIZE_MEDIUM               2
#define SIZE_LARGE                3
#define SIZE_GIANT                4
#define SIZE_HUGE                 5
#define SIZE_GARGANTUAN           6
#define SIZE_TITANIC              7
*/

/*
 *  User list structure
 */

/* is not used -Deck
struct  userl_data
{
    USERL_DATA *  next;
    char *        name;
    int           level;
    char *        user;
    char *        host;
    char *        lastlogin;
    char *        desc;
}; */


#ifdef NEW_MONEY
struct money_data
{
  int           gold;
  int           silver;
  int           copper;
};
#endif

struct arena_data
{
  AREA_DATA *area;	/* Arena area */
  CHAR_DATA *cch;	/* Challenger char */
  CHAR_DATA *och;	/* optional challengee char */
  CHAR_DATA *fch;	/* First char in arena */
  CHAR_DATA *sch;	/* Second char in arena */
  int award;		/* Money in the pot */
  int count;		/* Update ticker */
};


struct wiznet_type
{
    char *      name;
    long        flag;
    int         level;
};

/*
 * Site ban structure.
 */
struct	ban_data
{
    BAN_DATA *	next;
    char	type;
    char *	name;
    char *      user;
};


bool    MOBtrigger;

#define ERROR_PROG        -1
#define IN_FILE_PROG       0
#define ACT_PROG           1
#define SPEECH_PROG        2
#define RAND_PROG          4
#define FIGHT_PROG         8
#define DEATH_PROG        16
#define HITPRCNT_PROG     32
#define ENTRY_PROG        64
#define GREET_PROG       128
#define ALL_GREET_PROG   256
#define GIVE_PROG        512
#define BRIBE_PROG      1024

/*
 * Time and weather stuff.
 */
#define SUN_EVENING	 	    0
#define SUN_MIDNIGHT		    1
#define SUN_DARK		    2
#define SUN_DAWN		    3
#define SUN_MORNING		    4
#define SUN_NOON		    5
#define SUN_AFTERNOON		    6
#define SUN_DUSK		    7

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
 * WIZnet flags
 */
#define WIZ_ON			      1
#define WIZ_TICKS		      2
#define WIZ_LOGINS		      4
#define WIZ_SITES		      8
#define WIZ_LINKS		     16
#define WIZ_DEATHS		     32
#define WIZ_RESETS		     64
#define WIZ_MOBDEATHS		    128
#define WIZ_FLAGS		    256
#define WIZ_PENALTIES		    512
#define WIZ_SACCING		   1024
#define WIZ_LEVELS		   2048
#define WIZ_SECURE		   4096
#define WIZ_SWITCHES		   8192
#define WIZ_SNOOPS		  16384
#define WIZ_RESTORE	          32768
#define WIZ_LOAD		  65536
#define WIZ_NEWBIE		 131072
#define WIZ_PREFIX		 262144
#define WIZ_SPAM		 524288
#define WIZ_GENERAL		1048576
#define WIZ_OLDLOG		2097152


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
#define CON_READ_MOTD			8
#define CON_GET_NEW_RACE	        9
#define CON_CONFIRM_RACE               10
#define CON_CONFIRM_CLASS              11
#define CON_CHECK_AUTHORIZE   	       12
#define CON_GET_2ND_CLASS	       13
#define CON_CONFIRM_2ND_CLASS	       14
#define CON_WANT_MULTI		       15
#define CON_GET_3RD_CLASS	       16
#define CON_CONFIRM_3RD_CLASS	       17
#define CON_WANT_MULTI_2	       18

#define CON_GET_ANSI			105
#define CON_AUTHORIZE_NAME		100
#define CON_AUTHORIZE_NAME1		101
#define CON_AUTHORIZE_NAME2		102
#define CON_AUTHORIZE_NAME3		103
#define CON_AUTHORIZE_LOGOUT		104
#define CON_CHATTING                    200

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
    char *              user;
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
    void *              pEdit;		/* OLC */
    void *              inEdit;         /* Altrag, for nested editors */
    char **             pString;	/* OLC */
    int			editor;		/* OLC */
    int                 editin;         /* Altrag, again for nesting */
    bool		ansi;
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
#define TO_COMBAT           4



/*
 * Help table types.
 */
struct	help_data
{
    HELP_DATA * 	next;
    /*AREA_DATA *		area;*/		/* OLC 1.1b */
    int 	        level;
    char *      	keyword;
    char *      	text;
};


/*
 * Structure for social in the socials list.
 */
struct  social_data
{
    SOCIAL_DATA	*	next;
    char *              name;
    char *              char_no_arg;
    char *  	        others_no_arg;
    char * 	        char_found;
    char * 	        others_found;
    char * 	        vict_found;
    char * 	        char_auto;
    char * 	        others_auto;
};

struct  newbie_data
{
    NEWBIE_DATA *	next;
    char *		keyword;
    char *		answer1;
    char * 		answer2;
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
 *   Player list structer.
 */
struct playerlist_data
{
  PLAYERLIST_DATA * next;
  char * name;
  unsigned char level;
  char * clan_name;
  unsigned char clan_rank;
  char * guild_name;
  unsigned char guild_rank;
};

#define CLASS_MAGE           0
#define CLASS_CLERIC         1
#define CLASS_THIEF          2
#define CLASS_WARRIOR        3
#define CLASS_PSIONICIST     4
#define CLASS_DRUID          5
#define CLASS_RANGER         6
#define CLASS_PALADIN        7
#define CLASS_BARD           8
#define CLASS_VAMPIRE        9
#define CLASS_NECROMANCER   10
#define CLASS_WEREWOLF      11
#define CLASS_MONK	    12

#define RACE_HUMAN           0
#define RACE_ELF             1
#define RACE_DWARF           2
#define RACE_PIXIE           3
#define RACE_HALFLING        4
#define RACE_DROW            5
#define RACE_ELDER           6
#define RACE_OGRE            7
#define RACE_LIZARDMAN       8
#define RACE_DEMON           9
#define RACE_GHOUL          10
#define RACE_ILLITHID       11
#define RACE_MINOTAUR       12
#define RACE_TROLL	    13
#define RACE_SHADOW         14
#define RACE_TABAXI	    15
/*
 * Per-class stuff.
 */


struct	class_type
{
    char 	who_name	[ 4 ];	/* Three-letter name for 'who'	*/
    char        who_long        [ 15 ]; /* Long name of Class           */
    int 	attr_prime;		/* Prime attribute		*/
    int 	weapon;			/* First weapon			*/
    int 	guild;			/* Vnum of guild room		*/
    int 	skill_adept;		/* Maximum skill level		*/
    int 	thac0_00;		/* Thac0 for level  0		*/
    int 	thac0_97;		/* Thac0 for level 47		*/
    int  	hp_min;			/* Min hp gained on leveling	*/
    int	        hp_max;			/* Max hp gained on leveling	*/
    bool	fMana;			/* Class gains mana on level	*/
    bool	races[ MAX_RACE ];	/* Can a race be a class?	*/
    bool	multi[ MAX_CLASS ];	/* Which classes can multiclass */
    bool	objtype[ 14 ];		/* Which classes can wield what types of weapons */
};
#if 0
struct  race_type
{
    char        race_name      [ 4 ];
    char        race_full      [ 20 ];
    int		mstr;
    int		mint;
    int		mwis;
    int		mdex;
    int		mcon;
/*  int         atribmod1;
    int         atribmod2;     OLD VERSION.
    int         modamount1;
    int         modamount2; */
};
#endif

struct  race_data
{
    RACE_DATA * next;
    int		vnum;
    char *     	race_name;
    char *     	race_full;
    int		mstr;
    int		mint;
    int		mwis;
    int		mdex;
    int		mcon;
};

/* not used - Decklarean
struct  nrace_type
{
    char        lname  [20];
    char        sname  [4];
    int         size;
    int         mstr;
    int         mdex;
    int         mint;
    int         mwis;
    int         mcon;
    int         mods [5];
    long        res_flags;
    long        vul_flags;
    long        affects;
    int         hplev;
    int         manalev;
    int         movelev;
};
*/
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
    bool        protected;
    int         on_board;
};



/*
 * An affect.
 */
struct	affect_data
{
    AFFECT_DATA *	next;
    int 		type;
    int			level;
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

/* Bitvector values.. replaces the old A,B,C system. */
#define BV00    0x00000001
#define BV01    0x00000002
#define BV02    0x00000004
#define BV03    0x00000008
#define BV04    0x00000010
#define BV05    0x00000020
#define BV06    0x00000040
#define BV07    0x00000080
#define BV08    0x00000100
#define BV09    0x00000200
#define BV10    0x00000400
#define BV11    0x00000800
#define BV12    0x00001000
#define BV13    0x00002000
#define BV14    0x00004000
#define BV15    0x00008000
#define BV16    0x00010000
#define BV17    0x00020000
#define BV18    0x00040000
#define BV19    0x00080000
#define BV20    0x00100000
#define BV21    0x00200000
#define BV22    0x00400000
#define BV23    0x00800000
#define BV24    0x01000000
#define BV25    0x02000000
#define BV26    0x04000000
#define BV27    0x08000000
#define BV28    0x10000000
#define BV29    0x20000000
#define BV30    0x40000000
#define BV31	0x80000000

/* RT ASCII conversions -- used so we can have letters in this file */

#define A		  	1
#define B			2
#define C			4
#define D			8
#define E			16
#define F			32
#define G			64
#define H			128

#define I			256
#define J			512
#define K		        1024
#define L		 	2048
#define M			4096
#define N		 	8192
#define O			16384
#define P			32768

#define Q			65536
#define R			131072
#define S			262144
#define T			524288
#define U			1048576
#define V			2097152
#define W			4194304
#define X			8388608

#define Y			16777216
#define Z			33554432
#define aa			67108864 	/* doubled due to conflicts */
#define bb			134217728
#define cc			268435456
#define dd			536870912
#define ee			1073741824

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
#define MOB_VNUM_CITYGUARD	   127
#define MOB_VNUM_DEMON1 	   4
#define MOB_VNUM_DEMON2            4
#define MOB_VNUM_SUPERMOB          7
#define MOB_VNUM_ULT               3160

#define MOB_VNUM_AIR_ELEMENTAL     8914
#define MOB_VNUM_EARTH_ELEMENTAL   8915
#define MOB_VNUM_WATER_ELEMENTAL   8916
#define MOB_VNUM_FIRE_ELEMENTAL    8917
#define MOB_VNUM_DUST_ELEMENTAL    8918

/* XOR */
#define MOB_VNUM_DEMON		   80
#define MOB_VNUM_INSECTS	   81
#define MOB_VNUM_WOLFS		   82

/*ELVIS*/
#define MOB_VNUM_ANGEL             83
#define MOB_VNUM_SHADOW            84
#define MOB_VNUM_BEAST             85
#define MOB_VNUM_TRENT             86

/* BILL GATES */
#define GUILD_NORMAL		0
#define GUILD_PKILL		BV00
#define GUILD_CHAOS		BV01

/* CLANS */
#define CLAN_PKILL              BV00
#define CLAN_CIVIL_PKILL	BV01
#define CLAN_CHAMP_INDUCT	BV02
#define CLAN_LEADER_INDUCT	BV03
#define CLAN_FIRST_INDUCT	BV04
#define CLAN_SECOND_INDUCT	BV05

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		BV00		/* Auto set for mobs */
#define ACT_SENTINEL		BV01		/* Stays in one room	*/
#define ACT_SCAVENGER		BV02		/* Picks up objects	*/

#define ACT_AGGRESSIVE		BV05		/* Attacks PC's		*/
#define ACT_STAY_AREA		BV06		/* Won't leave area	*/
#define ACT_WIMPY		BV07		/* Flees when hurt	*/
#define ACT_PET			BV08		/* Auto set for pets	*/
#define ACT_TRAIN		BV09		/* Can train PC's	*/
#define ACT_PRACTICE		BV10		/* Can practice PC's	*/
#define ACT_GAMBLE              BV11            /* Runs a gambling game */
#define ACT_PROTOTYPE           BV12            /* Prototype flag       */
#define ACT_UNDEAD              BV13            /* Can be turned        */
#define ACT_TRACK               BV14            /* Track players        */
#define ACT_QUESTMASTER		BV15            /* Autoquest giver      */
#define ACT_POSTMAN		BV16		/* Hello Mr. Postman!   */
#define ACT_NODRAG		BV17		/* No drag mob		*/
#define ACT_NOPUSH		BV18		/* No push mob		*/
#define ACT_NOSHADOW		BV19		/* No shadow mob	*/
#define ACT_NOASTRAL		BV20		/* No astral mob	*/
#define ACT_NEWBIE		BV21		/* Newbie Helper	*/

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		BV00
#define AFF_INVISIBLE		BV01
#define AFF_DETECT_EVIL		BV02
#define AFF_DETECT_INVIS	BV03
#define AFF_DETECT_MAGIC	BV04
#define AFF_DETECT_HIDDEN	BV05
#define AFF_HASTE		BV06
#define AFF_SANCTUARY		BV07
#define AFF_FAERIE_FIRE		BV08
#define AFF_INFRARED		BV09
#define AFF_CURSE		BV10
#define AFF_FLAMING             BV11
#define AFF_POISON		BV12
#define AFF_PROTECT		BV13
#define AFF_VIBRATING		BV14		/* Unused	*/
#define AFF_SNEAK		BV15
#define AFF_HIDE		BV16
#define AFF_SLEEP		BV17
#define AFF_CHARM		BV18
#define AFF_FLYING		BV19
#define AFF_PASS_DOOR		BV20
#define AFF_STUN                BV21
#define AFF_SUMMONED            BV22
#define AFF_MUTE                BV23
#define AFF_PEACE              	BV24
#define AFF_FIRESHIELD         	BV25
#define AFF_SHOCKSHIELD        	BV26
#define AFF_ICESHIELD         	BV27
#define AFF_CHAOS             	BV28
#define AFF_SCRY              	BV29
#define AFF_ANTI_FLEE        	BV30
#define AFF_DISJUNCTION      	BV31

#define AFF_POLYMORPH		1
#define CODER			2
#define AFF_NOASTRAL            4
#define AFF_TRUESIGHT          16
#define AFF_BLADE              32
#define AFF_DETECT_GOOD        64
#define AFF_PROTECTION_GOOD    128
#define AFF_BERSERK            256
#define AFF_CONFUSED          1024
#define AFF_FUMBLE            2048
#define AFF_DANCING           4096
#define AFF_PHASED           16384
#define AFF_FIELD	     32768
#define AFF_RAGE	     65536
#define AFF_RUSH	    131072
#define AFF_INERTIAL	    262144
#define AFF_GOLDEN	    524288
#define AFF_PLOADED     1073741824

/* damage classes */
#define DAM_NONE                0
#define DAM_BASH                1
#define DAM_PIERCE              2
#define DAM_SLASH               3
#define DAM_FIRE                4
#define DAM_COLD                5
#define DAM_LIGHTNING           6
#define DAM_ACID                7
#define DAM_POISON              8
#define DAM_NEGATIVE            9
#define DAM_HOLY                10
#define DAM_ENERGY              11
#define DAM_MENTAL              12
#define DAM_DISEASE             13
#define DAM_DROWNING            14
#define DAM_LIGHT		15
#define DAM_OTHER               16
#define DAM_HARM		17
#define DAM_CHARM		18
#define DAM_SOUND		19

/* return values for check_imm */
#define IS_NORMAL		0
#define IS_IMMUNE		1
#define IS_RESISTANT		2
#define IS_VULNERABLE		3

/* IMM bits for mobs */
#define IMM_SUMMON              (A)
#define IMM_CHARM               (B)
#define IMM_MAGIC               (C)
#define IMM_WEAPON              (D)
#define IMM_BASH                (E)
#define IMM_PIERCE              (F)
#define IMM_SLASH               (G)
#define IMM_FIRE                (H)
#define IMM_COLD                (I)
#define IMM_LIGHTNING           (J)
#define IMM_ACID                (K)
#define IMM_POISON              (L)
#define IMM_NEGATIVE            (M)
#define IMM_HOLY                (N)
#define IMM_ENERGY              (O)
#define IMM_MENTAL              (P)
#define IMM_DISEASE             (Q)
#define IMM_DROWNING            (R)
#define IMM_LIGHT		(S)
#define IMM_SOUND		(T)
#define IMM_WOOD                (X)
#define IMM_SILVER              (Y)
#define IMM_IRON                (Z)
#define IMM_NERVE		(aa)

/* RES bits for mobs */
#define RES_SUMMON		(A)
#define RES_CHARM		(B)
#define RES_MAGIC               (C)
#define RES_WEAPON              (D)
#define RES_BASH                (E)
#define RES_PIERCE              (F)
#define RES_SLASH               (G)
#define RES_FIRE                (H)
#define RES_COLD                (I)
#define RES_LIGHTNING           (J)
#define RES_ACID                (K)
#define RES_POISON              (L)
#define RES_NEGATIVE            (M)
#define RES_HOLY                (N)
#define RES_ENERGY              (O)
#define RES_MENTAL              (P)
#define RES_DISEASE             (Q)
#define RES_DROWNING            (R)
#define RES_LIGHT		(S)
#define RES_SOUND		(T)
#define RES_WOOD                (X)
#define RES_SILVER              (Y)
#define RES_IRON                (Z)

/* VULN bits for mobs */
#define VULN_SUMMON		(A)
#define VULN_CHARM		(B)
#define VULN_MAGIC              (C)
#define VULN_WEAPON             (D)
#define VULN_BASH               (E)
#define VULN_PIERCE             (F)
#define VULN_SLASH              (G)
#define VULN_FIRE               (H)
#define VULN_COLD               (I)
#define VULN_LIGHTNING          (J)
#define VULN_ACID               (K)
#define VULN_POISON             (L)
#define VULN_NEGATIVE           (M)
#define VULN_HOLY               (N)
#define VULN_ENERGY             (O)
#define VULN_MENTAL             (P)
#define VULN_DISEASE            (Q)
#define VULN_DROWNING           (R)
#define VULN_LIGHT		(S)
#define VULN_SOUND		(T)
#define VULN_WOOD               (X)
#define VULN_SILVER             (Y)
#define VULN_IRON		(Z)


/* weapon class */
#define WEAPON_EXOTIC		0
#define WEAPON_SWORD		1
#define WEAPON_DAGGER		2
#define WEAPON_SPEAR		3
#define WEAPON_MACE		4
#define WEAPON_AXE		5
#define WEAPON_FLAIL		6
#define WEAPON_WHIP		7
#define WEAPON_POLEARM		8

/* weapon types */
#define WEAPON_FLAMING		(A)
#define WEAPON_FROST		(B)
#define WEAPON_VAMPIRIC		(C)
#define WEAPON_SHARP		(D)
#define WEAPON_VORPAL		(E)
#define WEAPON_TWO_HANDS	(F)
#define WEAPON_SHOCKING		(G)
#define WEAPON_POISON		(H)

/* gate flags */
#define GATE_NORMAL_EXIT	(A)
#define GATE_NOCURSE		(B)
#define GATE_GOWITH		(C)
#define GATE_BUGGY		(D)
#define GATE_RANDOM		(E)


/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000


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

#define OBJ_VNUM_BLOOD		      9
#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_FINAL_TURD	     16
#define OBJ_VNUM_PORTAL              17
#define OBJ_VNUM_DOLL                18
#define OBJ_VNUM_BERRY               19

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22
#define OBJ_VNUM_SOULGEM	     23
#define OBJ_VNUM_PARCHMENT        25050
#define OBJ_VNUM_QUILL            25051
#define OBJ_VNUM_FLASK            25052
#define OBJ_VNUM_CAULDRON         25053
#define OBJ_VNUM_MFIRE            25054
#define OBJ_VNUM_MINK             25055
/* Added for use in Ravenwood - Angi */
#define OBJ_VNUM_RWPARCHMENT	   1339
#define OBJ_VNUM_RWQUILL	   1341
#define OBJ_VNUM_RWFLASK	   1343
#define OBJ_VNUM_RWCAULDRON	   1344
#define OBJ_VNUM_RWFIRE	   	   1345
#define OBJ_VNUM_RWINK		   1346

#define OBJ_VNUM_PAPER		  25062
#define OBJ_VNUM_LETTER		  25063
#define OBJ_VNUM_SMOKEBOMB	  25064
#define OBJ_VNUM_TO_FORGE_A	  25065 /* armor */
#define OBJ_VNUM_TO_FORGE_W	  25066 /* weapon */
#define OBJ_VNUM_WARD_PHYS	  25067
#define OBJ_VNUM_SMITHY_HAMMER	    713
#define OBJ_VNUM_SCHOOL_MACE	   3001
#define OBJ_VNUM_SCHOOL_DAGGER	   138
#define OBJ_VNUM_SCHOOL_SWORD	   3003
#define OBJ_VNUM_SCHOOL_VEST	   136
#define OBJ_VNUM_SCHOOL_SHIELD	   3005
#define OBJ_VNUM_SCHOOL_BANNER     144
#define OBJ_VNUM_SCHOOL_CLUB       3007
/* 15001 - 15007 */

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
#define ITEM_NOTEBOARD               11
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
#define ITEM_LENSE                   27
#define ITEM_BLOOD                   28
#define ITEM_PORTAL                  29
#define ITEM_VODOO		     30
#define ITEM_BERRY                   31
//#define ITEM_POTION_BAG		     32

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		      1		/*	1	*/
#define ITEM_HUM		      2
#define ITEM_DARK		      4
#define ITEM_LOCK		      8
#define ITEM_EVIL		     16
#define ITEM_INVIS		     32
#define ITEM_MAGIC		     64
#define ITEM_NODROP		    128
#define ITEM_BLESS		    256
#define ITEM_ANTI_GOOD		    512		/*	10	*/
#define ITEM_ANTI_EVIL		   1024
#define ITEM_ANTI_NEUTRAL	   2048
#define ITEM_NOREMOVE		   4096
#define ITEM_INVENTORY		   8192
#define ITEM_POISONED             16384
#define ITEM_DWARVEN              32768
#define ITEM_FREE_FLAG_2          65536
#define ITEM_FREE_FLAG_3          131072
#define ITEM_FREE_FLAG_4          262144
#define ITEM_FREE_FLAG_5          524288		/*	20	*/
#define ITEM_FREE_FLAG_6         1048576
#define ITEM_FREE_FLAG_7         2097152
#define ITEM_FREE_FLAG_8         4194304
#define ITEM_FREE_FLAG_9         8388608
#define ITEM_FREE_FLAG_10       16777216
#define ITEM_FLAME              33554432
#define ITEM_CHAOS              67108864
#define ITEM_NO_LOCATE		(cc)
#define ITEM_NO_DAMAGE          (dd)
#define ITEM_PATCHED            (ee)
#define ITEM_ICY                (bb)

/* ANTI_CLASS flags, FOR NEW FLAGS */
#define ITEM_ANTI_MAGE			1
#define ITEM_ANTI_CLERIC         	2
#define ITEM_ANTI_THIEF          	4
#define ITEM_ANTI_WARRIOR        	8
#define ITEM_ANTI_PSI                  16
#define ITEM_ANTI_DRUID                32
#define ITEM_ANTI_RANGER               64
#define ITEM_ANTI_PALADIN             128
#define ITEM_ANTI_BARD                256
#define ITEM_ANTI_VAMP                512
#define ITEM_ANTI_NECRO		     1024
#define ITEM_ANTI_WWF		     2048
#define ITEM_ANTI_MONK		     4096
/* ANTI_RACE flags, FOR NEW FLAGS */

#define ITEM_ANTI_HUMAN			1
#define ITEM_ANTI_ELF			2
#define ITEM_ANTI_DWARF			4
#define ITEM_ANTI_PIXIE			8
#define ITEM_ANTI_HALFLING	       16
#define ITEM_ANTI_DROW		       32
#define ITEM_ANTI_ELDER		       64
#define ITEM_ANTI_OGRE		      128
#define ITEM_ANTI_LIZARDMAN	      256
#define ITEM_ANTI_DEMON		      512
#define ITEM_ANTI_GHOUL		     1024
#define ITEM_ANTI_ILLITHID	     2048
#define ITEM_ANTI_MINOTAUR	     4096
#define ITEM_ANTI_TROLL              8192
#define ITEM_ANTI_SHADOW            16384
#define ITEM_ANTI_TABAXI	    32768

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
#define ITEM_PROTOTYPE           262144
#define ITEM_WEAR_EARS           524288
#define ITEM_WEAR_ANKLE         1048576


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
#define APPLY_BP                     25
#define APPLY_ANTI_DIS               26

/* X */
#define PERM_SPELL_BEGIN	100
#define APPLY_INVISIBLE		(PERM_SPELL_BEGIN + 0)
#define APPLY_DETECT_EVIL	(PERM_SPELL_BEGIN + 1)
#define APPLY_DETECT_INVIS	(PERM_SPELL_BEGIN + 2)
#define APPLY_DETECT_MAGIC	(PERM_SPELL_BEGIN + 3)
#define APPLY_DETECT_HIDDEN	(PERM_SPELL_BEGIN + 4)
#define APPLY_SANCTUARY		(PERM_SPELL_BEGIN + 5)

#define APPLY_INFRARED		(PERM_SPELL_BEGIN + 6)
#define APPLY_PROTECT		(PERM_SPELL_BEGIN + 7)
#define APPLY_SNEAK		(PERM_SPELL_BEGIN + 8)
#define APPLY_HIDE		(PERM_SPELL_BEGIN + 9)
#define APPLY_FLYING		(PERM_SPELL_BEGIN + 10)

#define APPLY_PASS_DOOR		(PERM_SPELL_BEGIN + 11)
#define APPLY_HASTE		(PERM_SPELL_BEGIN + 12)

#define APPLY_FIRESHIELD        (PERM_SPELL_BEGIN + 13)
#define APPLY_SHOCKSHIELD       (PERM_SPELL_BEGIN + 14)

#define APPLY_ICESHIELD         (PERM_SPELL_BEGIN + 15)
#define APPLY_CHAOS             (PERM_SPELL_BEGIN + 16)
#define APPLY_SCRY              (PERM_SPELL_BEGIN + 17)
#define APPLY_FAERIE_FIRE	(PERM_SPELL_BEGIN + 19)
#define APPLY_POISON		(PERM_SPELL_BEGIN + 20)
#define APPLY_BLESS		(PERM_SPELL_BEGIN + 21)
#define APPLY_GIANT_STRENGTH	(PERM_SPELL_BEGIN + 22)
#define APPLY_COMBAT_MIND       (PERM_SPELL_BEGIN + 23)
#define APPLY_HEIGHTEN_SENSES   (PERM_SPELL_BEGIN + 24)
#define APPLY_BIO_ACCELERATION  (PERM_SPELL_BEGIN + 25)
/* END */

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
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_CHAT		      1
#define ROOM_VNUM_TEMPLE	   25000
#define ROOM_VNUM_ALTAR		   25000
#define ROOM_VNUM_JEZ		   25001
#define ROOM_VNUM_LAIK		   1301
#define ROOM_VNUM_SCHOOL	   800   /* Used to be 101 - Angi */
#define ROOM_VNUM_RW_SCHOOL	   1251
#define ROOM_VNUM_GRAVEYARD_A      25000
#define ROOM_VNUM_MORGUE           25130
#define ROOM_VNUM_DON_1		   25129
#define ROOM_VNUM_DON_2		   25127
#define ROOM_VNUM_DON_3		   25126
#define ROOM_VNUM_DON_4		   25128
#define ROOM_VNUM_HELL		      8
#define ROOM_VNUM_RJAIL		   1497
#define ROOM_VNUM_ARTIFACTOR	   25097
#define ROOM_VNUM_SMITHY	713
#define ROOM_ARENA_VNUM		7350
#define ROOM_ARENA_ENTER_F	7368
#define ROOM_ARENA_ENTER_S	7369
#define ROOM_ARENA_HALL_SHAME	7370


/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_NONE		      0
#define ROOM_DARK		      1
#define ROOM_NO_MOB		      4
#define ROOM_INDOORS		      8
#define ROOM_NO_SHADOW	             16
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
#define ROOM_NO_OFFENSIVE       2097152
#define ROOM_NO_FLEE            4194304
#define ROOM_SILENT             8388608
#define ROOM_BANK		16777216
#define ROOM_NOFLOOR            33554432
#define ROOM_SMITHY             67108864
#define ROOM_NOSCRY		134217728
#define ROOM_DAMAGE             268435456
#define ROOM_PKILL              536870912
#define ROOM_MARK              1073741824

/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_MAX			      5

#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5


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
#define EX_RANDOM                   128
#define EX_MAGICLOCK		    256

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
#define SECT_BADLAND                 11
#define SECT_MAX		     12



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
#define WEAR_EAR_R                   23
#define WEAR_EAR_L		     24
#define WEAR_ANKLE_L                 25
#define WEAR_ANKLE_R                 26
#define MAX_WEAR		     27



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
 *  Maxes for conditions.
 */
#define MAX_FULL		     50
#define MAX_THIRST		     50
#define MAX_DRUNK		     50


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
#define POS_MEDITATING                8


/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		BV00		/* Don't EVER set.	*/
#define PLR_BOUGHT_PET		BV01
#define PLR_AFK			BV02
#define PLR_AUTOEXIT		BV03
#define PLR_AUTOLOOT		BV04
#define PLR_AUTOSAC             BV05
#define PLR_BLANK		BV06
#define PLR_BRIEF		BV07
#define PLR_FULLNAME            BV08
#define PLR_COMBINE		BV09
#define PLR_PROMPT		BV10
#define PLR_TELNET_GA		BV11
#define PLR_HOLYLIGHT		BV12
#define PLR_WIZINVIS		BV13
#define PLR_QUEST		BV14
#define	PLR_SILENCE		BV15
#define PLR_NO_EMOTE		BV16
#define PLR_CLOAKED		BV17
#define PLR_NO_TELL		BV18
#define PLR_LOG			BV19
#define PLR_DENY		BV20
#define PLR_FREEZE		BV21
#define PLR_THIEF		BV22
#define PLR_KILLER		BV23
#define PLR_ANSI                BV24
#define PLR_AUTOCOINS           BV25
#define PLR_AUTOSPLIT           BV26
#define PLR_UNDEAD              BV27
#define PLR_QUESTOR             BV28
#define PLR_COMBAT              BV29
#define PLR_OUTCAST             BV30
#define PLR_PKILLER		BV31

/*
 * Obsolete bits.
 */
#if 0
#define PLR_AUCTION		      4	/* Obsolete	*/
#define PLR_CHAT		    256	/* Obsolete	*/
#define PLR_NO_SHOUT		 131072	/* Obsolete	*/
#endif


#define STUN_TOTAL            0   /* Commands and combat halted. Normal stun */
#define STUN_COMMAND          1   /* Commands halted. Combat goes through */
#define STUN_MAGIC            2   /* Can't cast spells */
#define STUN_NON_MAGIC        3   /* No weapon attacks */
#define STUN_TO_STUN          4   /* Requested. Stop continuous stunning */


/*
 * Channel bits.
 */
#define CHANNEL_NONE                  0
#define	CHANNEL_AUCTION		      1
#define	CHANNEL_CHAT		      2
#define CHANNEL_OOC		      4
#define	CHANNEL_IMMTALK	              8
#define	CHANNEL_MUSIC		     16
#define	CHANNEL_QUESTION	     32
#define	CHANNEL_SHOUT		     64
#define	CHANNEL_YELL		    128
#define CHANNEL_CLAN                256
#define CHANNEL_CLASS               512
#define CHANNEL_HERO               1024

/*
 * Log Channels
 * Added by Altrag.
 */
#define CHANNEL_LOG		   2048
#define CHANNEL_BUILD		   4096
#define CHANNEL_GOD                8192
#define CHANNEL_GUARDIAN          16384
#define CHANNEL_GUILD		  32768
#define CHANNEL_CODER             65536
#define CHANNEL_INFO		 131072
#define CHANNEL_CHALLENGE        262144


/* Master Channels
 * Added by Decklarean
 */
#define CHANNEL_GUILD_MASTER     524288
#define CHANNEL_CLASS_MASTER    1048576
#define CHANNEL_CLAN_MASTER     2097152

/* IMC channel.. -- Altrag */
#define CHANNEL_IMC		4194304

/* Vent and Arena --Angi */
#define CHANNEL_VENT            8388608
#define CHANNEL_ARENA 	       16777216

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
    MOB_INDEX_DATA *	next;
    SPEC_FUN *		spec_fun;
    SHOP_DATA *		pShop;
    AREA_DATA *		area;			/* OLC */
    MPROG_DATA *        mobprogs;
    char *		player_name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    int 		vnum;
    int			progtypes;
    int 		count;
    int 		killed;
    short 		sex;
    short		class;
    int		 	level;
    int			act;
    long		affected_by;
    long		affected_by2;
    long		imm_flags;	/* XOR */
    long		res_flags;
    long		vul_flags;	/* XOR */
    int 		alignment;
    int 		hitroll;		/* Unused */
    int 		ac;			/* Unused */
    int 		hitnodice;		/* Unused */
    int 		hitsizedice;		/* Unused */
    int 		hitplus;		/* Unused */
    int 		damnodice;		/* Unused */
    int 		damsizedice;		/* Unused */
    int 		damplus;		/* Unused */
#ifdef NEW_MONEY
    MONEY_DATA		money;
#else
    int			gold;			/* Unused */
#endif
};

/*
 * -- Altrag
 */
struct  gskill_data
{
    int                 sn;
    void *              victim;
    int                 level;
    int                 timer;
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
    CHAR_DATA *         hunting;
    CHAR_DATA *		reply;
    CHAR_DATA *         questgiver;
    CHAR_DATA *         questmob;
    SPEC_FUN *		spec_fun;
    MOB_INDEX_DATA *	pIndexData;
    DESCRIPTOR_DATA *	desc;
    AFFECT_DATA *	affected;
    AFFECT_DATA *	affected2;
    NOTE_DATA *		pnote;
    OBJ_DATA *		carrying;
    OBJ_DATA *          questobj;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	was_in_room;
    POWERED_DATA *	powered;
    PC_DATA *		pcdata;
    MPROG_ACT_LIST *    mpact;
    GSPELL_DATA *       gspell;
/*    PHOBIA_DATA *       phobia;*/
    char *		name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    char *              prompt;
    int                 cquestpnts;    /* for clan quests */
    int                 questpoints;   /* for autoquests  */
    int                 nextquest;
    int                 countdown;
    short 		sex;
/* FOR MULTICLASS BELOW */
    short 		class[ MAX_CLASS ];
/* FOR MULTICLASS ABOVE */
    int                 mpactnum;
    short 		race;
    short               clan;
    char                clev;
    int                 ctimer;
    int                 wizinvis;
    int			cloaked;
    int			level;
    int                 antidisarm;
    int  		trust;
    bool                wizbit;
    int			played;
    time_t		logon;
    time_t		save_time;
    time_t              last_note;
    int 		timer;
    int 		wait;
    int                 race_wait;
    int 		hit;
/*    int 		max_hit; */
    int			perm_hit;
    int			mod_hit;
    int 		mana;
/*    int 		max_mana;*/
    int			perm_mana;
    int			mod_mana;
    int 		move;
/*    int 		max_move; */
    int			perm_move;
    int			mod_move;
    int                 bp;
/*    int                 max_bp;*/
    int			perm_bp;
    int		 	mod_bp;
    int			charisma;
#ifdef NEW_MONEY
    MONEY_DATA		money;
#else
    int			gold;
#endif
    int			exp;
    int			act;
    long		affected_by;
    long		affected_by2;
    long		imm_flags;
    long		res_flags;
    long		vul_flags;
    int 		position;
    int 		practice;
    int 		carry_weight;
    int 		carry_number;
    int 		saving_throw;
    short 		alignment;
    char		start_align;
    int 		hitroll;
    int 		damroll;
    int 		armor;
    int 		wimpy;
    int 		deaf;
    bool                deleted;
    int			combat_timer;	/* XOR */
    int			summon_timer;	/* XOR */
    char		guild_rank;
    GUILD_DATA *	guild;
    int                 stunned[STUN_MAX];
    int			wiznet;
};


struct guild_data
{
  char *	name;
  char *	deity;
  int		color;
  int		type;
};

struct quest_data
{
  char *        name;
  int           vnum;
  int           qp;
  int           level;
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

/*
 * Data which only PC's have.
 */
struct	pc_data
{
    PC_DATA *		next;
    ALIAS_DATA *        alias_list;
    char *		pwd;
    char *              afkchar;
    char *		bamfin;
    char *		bamfout;
    char *		bamfusee;
    char *              transto;
    char *              transfrom;
    char *              transvict;
    char *              slayusee;
    char *              slayroom;
    char *              slayvict;
    char *		whotype;
    char *		title;
    char *              lname;
    char *		empowerments;
    char *		detractments;
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
    bool                switched;
    int 		security;	/* OLC - Builder security */
#ifdef NEW_MONEY
    MONEY_DATA		bankaccount;
#else
    int                 bankaccount;
#endif
    OBJ_DATA          * storage;
    int                 storcount;
    int                 corpses;
    char *		plan;
    char *		email;
};

struct  alias_data
{
    ALIAS_DATA *next;
    char       *old;
    char       *new;
};

/* Not used -Deck
#define PHOBIA_FIRE      0
#define PHOBIA_LIGHTNING 1
#define PHOBIA_WATER     2
#define PHOBIA_MAGIC     3
#define PHOBIA_NONMAGIC  4
#define PHOBIA_PSI       5
#define PHOBIA_DRAGON    6
#define PHOBIA_MAX       7

struct  phobia_data
{
    PHOBIA_DATA *next;
    int          type;
    int          duration;
    int          strength;
};
*/

/*
 * heh.. were discussing obj/room progs.. and then these triggers started
 * looking a helluva lot like em.. :).. so what the hell..? :).. main
 * difference between this struct and the mobprog stuff is that this is
 * implemented as traps.  ie.. the disarmable stuff..
 * also, these are a little more global.. :)
 * -- Altrag
 */

/*
 * The object triggers.. quite a few of em.. :)
 */
#define OBJ_TRAP_ERROR           0  /* error! */
#define OBJ_TRAP_GET             A  /* obj is picked up */
#define OBJ_TRAP_DROP            B  /* obj is dropped */
#define OBJ_TRAP_PUT             C  /* obj is put into something */
#define OBJ_TRAP_WEAR            D  /* obj is worn */
#define OBJ_TRAP_LOOK            E  /* obj is looked at/examined */
#define OBJ_TRAP_LOOK_IN         F  /* obj is looked inside (containers) */
#define OBJ_TRAP_INVOKE          G  /* obj is invoked */
#define OBJ_TRAP_USE             H  /* obj is used (recited, zapped, ect) */
#define OBJ_TRAP_CAST            I  /* spell is cast on obj - percent */
#define OBJ_TRAP_CAST_SN         J  /* spell is cast on obj - by slot */
#define OBJ_TRAP_JOIN            K  /* obj is joined with another */
#define OBJ_TRAP_SEPARATE        L  /* obj is separated into two */
#define OBJ_TRAP_BUY             M  /* obj is bought from store */
#define OBJ_TRAP_SELL            N  /* obj is sold to store */
#define OBJ_TRAP_STORE           O  /* obj is stored in storage boxes */
#define OBJ_TRAP_RETRIEVE        P  /* obj is retrieved from storage */
#define OBJ_TRAP_OPEN            Q  /* obj is opened (containers) */
#define OBJ_TRAP_CLOSE           R  /* obj is closed (containers) */
#define OBJ_TRAP_LOCK            S  /* obj is locked (containers) */
#define OBJ_TRAP_UNLOCK          T  /* obj is unlocked (containers) */
#define OBJ_TRAP_PICK            U  /* obj is picked (containers) */
#define OBJ_TRAP_RANDOM          V  /* random trigger */
#define OBJ_TRAP_THROW           W  /* obj is thrown */
#define OBJ_TRAP_GET_FROM        X  /* to allow secondary obj's in get */
#define OBJ_TRAP_GIVE            Y  /* give an obj away */
#define OBJ_TRAP_FILL            Z  /* obj is filled (drink_cons) */

/*
 * Note that entry/exit/pass are only called if the equivilant exit
 * trap for the exit the person went through failed.
 * Pass is only called if the respective enter or exit trap failed.
 */
#define ROOM_TRAP_ERROR          0  /* error! */
#define ROOM_TRAP_ENTER          A  /* someone enters the room */
#define ROOM_TRAP_EXIT           B  /* someone leaves the room */
#define ROOM_TRAP_PASS           C  /* someone enters or leaves */
#define ROOM_TRAP_CAST           D  /* a spell was cast in room - percent */
#define ROOM_TRAP_CAST_SN        E  /* a spell was cast in room - by slot */
#define ROOM_TRAP_SLEEP          F  /* someone sleeps in the room */
#define ROOM_TRAP_WAKE           G  /* someone wakes up in the room */
#define ROOM_TRAP_REST           H  /* someone rests in the room */
#define ROOM_TRAP_DEATH          I  /* someone dies in the room */
#define ROOM_TRAP_TIME           J  /* depends on the time of day */
#define ROOM_TRAP_RANDOM         K  /* random trigger */

/*
 * enter/exit/pass rules are the same as those for room traps.
 * note that look trap is only called if scry trap fails.
 */
#define EXIT_TRAP_ERROR          0  /* error! */
#define EXIT_TRAP_ENTER          A  /* someone enters through the exit */
#define EXIT_TRAP_EXIT           B  /* someone leaves through the exit */
#define EXIT_TRAP_PASS           C  /* someone enters/leaves through exit */
#define EXIT_TRAP_LOOK           D  /* someone looks through exit */
#define EXIT_TRAP_SCRY           E  /* someone scrys through the exit */
#define EXIT_TRAP_OPEN           F  /* someone opens the exit (door) */
#define EXIT_TRAP_CLOSE          G  /* someone closes the exit (door) */
#define EXIT_TRAP_LOCK           H  /* someone locks the exit (door) */
#define EXIT_TRAP_UNLOCK         I  /* someone unlocks the exit (door) */
#define EXIT_TRAP_PICK           J  /* someone picks the exit (locked door) */

struct trap_data
{
    TRAP_DATA *next;
    TRAP_DATA *next_here;
    OBJ_INDEX_DATA *on_obj;
    ROOM_INDEX_DATA *in_room;
    EXIT_DATA *on_exit;
    int type;
    char *arglist;
    char *comlist;
    bool disarmable;
    bool disarmed;
    int disarm_dur;
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
    AREA_DATA *		area;			/* OLC */
    TRAP_DATA *         traps;
    int                 traptypes;
    char *		name;
    char *		short_descr;
    char *		description;
    int 		vnum;
    int 		item_type;
    int 		extra_flags;
/* FOR NEW FLAGS */
    int                 anti_race_flags;
    int                 anti_class_flags;

    int 		wear_flags;
    int 		count;
    int 		weight;
#ifdef NEW_MONEY
    MONEY_DATA		cost;
#else
    int			cost;			/* Unused */
#endif
    int                 level;
    int			value	[ 4 ];
    int                 ac_type;
    int                 ac_vnum;
    char *              ac_spell;
    int                 ac_charge [ 2 ];
    int                 join;
    int                 sep_one;
    int                 sep_two;
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
    CHAR_DATA *         stored_by;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    OBJ_INDEX_DATA *	pIndexData;
    ROOM_INDEX_DATA *	in_room;
    char *		name;
    char *		short_descr;
    char *		description;
    int 		item_type;
    int 		extra_flags;
/* FOR NEW FLAGS */
    int			anti_race_flags;
    int			anti_class_flags;
    int 		wear_flags;
    int 		wear_loc;
    int 		weight;
#ifdef NEW_MONEY
    MONEY_DATA		cost;
#else
    int			cost;
#endif
    int 		level;
    int 		timer;
    int			value	[ 4 ];
    int                 ac_type;
    int                 ac_vnum;
    char *              ac_spell;
    int                 ac_charge [ 2 ];
    bool                deleted;
};



/*
 * Exit data.
 */
struct	exit_data
{
    ROOM_INDEX_DATA *	to_room;
    EXIT_DATA *		next;		/* OLC */
    TRAP_DATA *         traps;
    int                 traptypes;
    int			rs_flags;	/* OLC */
    int 		vnum;
    int 		exit_info;
    int 		key;
    char *		keyword;
    char *		description;
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
    char *		name;
    int                 recall;
    int 		age;
    int 		nplayer;
    char *		filename;	/* OLC */
    char *		builders;	/* OLC - Listing of builders */
    int			security;	/* OLC - Value 0-infinity  */
    int			lvnum;		/* OLC - Lower vnum */
    int			uvnum;		/* OLC - Upper vnum */
    int			vnum;		/* OLC - Area vnum  */
    int			area_flags;	/* OLC */
    char *              reset_sound;    /* Altrag */
    int			windstr;
    int			winddir;
    short		llevel;		/* lowest player level suggested */
    short		ulevel;		/* upper player level suggested */
    int			def_color;
};

struct  new_clan_data
{
    CLAN_DATA *         next;
    MONEY_DATA 		bankaccount;
    char *              name;
    char *              diety;
    char *              description;
    char *              leader;
    char *              first;
    char *              second;
    char *              champ;
    bool                isleader;
    bool                isfirst;
    bool                issecond;
    bool                ischamp;
    int                 vnum;
    int                 recall;
    int                 pkills;
    int                 mkills;
    int                 members;
    int                 pdeaths;
    int                 mdeaths;
    int                 obj_vnum_1;
    int                 obj_vnum_2;
    int                 obj_vnum_3;
    int                 settings;
};
/*
 * ROOM AFFECT type
 */
struct	room_affect_data
{
    ROOM_AFFECT_DATA *	next;
    ROOM_INDEX_DATA *	room;
    CHAR_DATA * 	powered_by;
    OBJ_DATA *		material;
    int 		type;
    int 		location;
};
struct	powered_data
{
    POWERED_DATA *	next;
    ROOM_INDEX_DATA *	room;
    ROOM_AFFECT_DATA *	raf;
    int			type;
    int			cost;
};
/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    ROOM_AFFECT_DATA *  rAffect;
    CHAR_DATA *		people;
    OBJ_DATA *		contents;
    EXTRA_DESCR_DATA *	extra_descr;
    AREA_DATA *		area;
    EXIT_DATA *		exit	[ 6 ];
    RESET_DATA *	reset_first;	/* OLC */
    RESET_DATA *	reset_last;	/* OLC */
    TRAP_DATA *         traps;
    int                 traptypes;
    char *		name;
    char *		description;
    int 		vnum;
    int 		room_flags;
    int 		light;
    char 		sector_type;
    int                 rd;    /* TRI ( Room damage ) */
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
#define TAR_GROUP_OFFENSIVE         5
#define TAR_GROUP_DEFENSIVE         6
#define TAR_GROUP_ALL               7
#define TAR_GROUP_OBJ               8
#define TAR_GROUP_IGNORE            9



/*
 * Skills include spells as a particular case.
 */
struct	skill_type
{
    char *	name;			   /* Name of skill		 */
    int 	skill_level [ MAX_CLASS+3 ]; /* Level needed by class */
    SPELL_FUN *	spell_fun;		   /* Spell pointer (for spells) */
    int 	target;			   /* Legal targets		 */
    int 	minimum_position;	   /* Position for caster / user */
    int *	pgsn;			   /* Pointer to associated gsn	 */
    int 	min_mana;		   /* Minimum mana used		 */
    int 	beats;			   /* Waiting time after use	 */
    char *	noun_damage;		   /* Damage message		 */
    char *	msg_off;		   /* Wear off message		 */
    char *      room_msg_off;		   /* Room Wear off message	 */
    bool	dispelable;
    int         slot;                      /* For object loading         */
};

/*
 * -- Altrag
 */
struct gskill_type
{
    int         wait;                      /* Wait for casters in ticks  */
    int         slot;                      /* Matching skill_table sn    */
    int         casters[MAX_CLASS];        /* Casters needed by class    */
};


/*
 * These are skill_lookup return values for common skills and spells.
 */
extern  int	gsn_trip;
extern  int	gsn_dirt_kick;
extern  int	gsn_track;
extern  int	gsn_shield_block;
extern  int	gsn_drain_life;
extern  int	gsn_mental_drain;
extern  int	gsn_turn_evil;
extern  int     gsn_bash_door;
extern  int     gsn_stun;
extern  int     gsn_berserk;

extern  int	gsn_backstab;
extern	int	gsn_backstab_2;
extern	int	gsn_palm;
extern  int	gsn_dodge;
extern  int     gsn_dual;
extern  int	gsn_hide;
extern  int	gsn_peek;
extern  int	gsn_pick_lock;
extern  int     gsn_punch;
extern  int     gsn_jab_punch;
extern  int     gsn_cross_punch;
extern  int     gsn_kidney_punch;
extern  int     gsn_roundhouse_punch;
extern  int     gsn_uppercut_punch;
extern  int	gsn_sneak;
extern  int	gsn_steal;
extern  int	gsn_disarm;
extern  int     gsn_poison_weapon;

extern  int     gsn_bash;
extern	int	gsn_enhanced_damage;
extern  int     gsn_enhanced_two;
extern  int     gsn_enhanced_hit;
extern   int	gsn_flury;
extern	int	gsn_kick;
extern  int	gsn_high_kick;
extern  int	gsn_spin_kick;
extern  int	gsn_jump_kick;
extern  int     gsn_circle;
extern  int     gsn_throw;
extern  int     gsn_feed;
extern	int	gsn_parry;
extern	int	gsn_rescue;
extern  int     gsn_patch;
extern	int	gsn_second_attack;
extern	int	gsn_third_attack;
extern  int     gsn_fourth_attack;
extern  int     gsn_fifth_attack;
extern  int     gsn_sixth_attack;
extern  int     gsn_seventh_attack;
extern  int     gsn_eighth_attack;
extern  int     gsn_gouge;
extern  int     gsn_alchemy;
extern  int     gsn_scribe;
extern	int	gsn_blindness;
extern	int	gsn_charm_person;
extern	int	gsn_curse;
extern	int	gsn_invis;
extern	int	gsn_mass_invis;
extern	int	gsn_poison;
extern	int	gsn_sleep;
extern  int     gsn_prayer;
extern  int     gsn_soulstrike;
extern	int	gsn_scrolls;
extern	int	gsn_staves;
extern	int	gsn_wands;
extern  int	gsn_spellcraft;
extern  int	gsn_gravebind;
extern  int	gsn_multiburst;
extern	int	gsn_enhanced_claw;
extern	int	gsn_dualclaw;
extern	int	gsn_fastheal;
extern	int	gsn_rage;
extern	int	gsn_bite;
extern	int	gsn_rush;
extern	int	gsn_howlfear;
extern	int	gsn_scent;
extern	int	gsn_frenzy;
extern	int	gsn_reflex;
extern	int	gsn_rake;
extern  int     gsn_headbutt;
extern	int	gsn_flamehand;
extern	int	gsn_frosthand;
extern	int	gsn_chaoshand;
extern	int	gsn_bladepalm;
extern	int	gsn_flykick;
extern	int	gsn_anatomyknow;
extern	int	gsn_blackbelt;
extern	int	gsn_nerve;
extern	int	gsn_ironfist;
extern	int	gsn_globedark;
extern	int	gsn_drowfire;
extern	int	gsn_snatch;
extern	int	gsn_enhanced_three;
extern	int	gsn_retreat;
extern	int	gsn_antidote;
extern	int	gsn_haggle;
extern	int	gsn_dodge_two;
extern	int	gsn_blindfight;
extern	int	gsn_shriek;
extern	int	gsn_spit;
extern	int	gsn_ward_safe;
extern	int	gsn_ward_heal;
extern  int     gsn_incinerate;
extern  int     gsn_grip;


/*  do not need gsn for imm commands  -Deck
extern  int     gsn_advance;
extern  int     gsn_allow;
extern  int     gsn_at;
extern  int     gsn_bamfin;
extern  int     gsn_bamfout;
extern  int     gsn_ban;
extern  int     gsn_cset;
extern  int     gsn_deny;
extern  int     gsn_disconnect;
extern  int     gsn_echo;
extern  int 	gsn_fighting;
extern  int     gsn_force;
extern  int     gsn_freeze;
extern  int     gsn_goto;
extern  int     gsn_holylight;
extern  int     gsn_immtalk;
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
*/

extern  int     gsn_shadow_walk;
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
#define TOGGLE_BIT( var, bit )  ( ( var )  ^=  ( bit ) )



/*
 * Character macros.
 */
#define IS_CODER( ch )          ( IS_SET( ( ch )->affected_by2, CODER ) )
#define IS_NPC( ch )		( IS_SET( ( ch )->act, ACT_IS_NPC ) )
#define IS_IMMORTAL( ch )	( get_trust( ch ) >= LEVEL_IMMORTAL )
#define IS_HERO( ch )		( get_trust( ch ) >= LEVEL_HERO     )
#define IS_QUESTOR( ch )        ( IS_SET( ( ch )->act, PLR_QUESTOR ) )

#define IS_AFFECTED( ch, sn )	( IS_SET( ( ch )->affected_by, ( sn ) ) )
#define IS_AFFECTED2(ch, sn)	( IS_SET( ( ch )->affected_by2, (sn)))
#define IS_SIMM(ch, sn)		(IS_SET((ch)->imm_flags, (sn)))
#define IS_SRES(ch, sn)		(IS_SET((ch)->res_flags, (sn)))
#define IS_SVUL(ch, sn)		(IS_SET((ch)->vul_flags, (sn)))

#define IS_GOOD( ch )		( ch->alignment >=  350 )
#define IS_EVIL( ch )		( ch->alignment <= -350 )
#define IS_NEUTRAL( ch )	( !IS_GOOD( ch ) && !IS_EVIL( ch ) )

#define IS_AWAKE( ch )		( ch->position > POS_SLEEPING )
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


#define STUN_CHAR( ch, pulse, type ) ( (ch)->stunned[(type)] =               \
				       UMAX( (ch)->stunned[(type)],          \
					     (pulse) ) )

#define IS_STUNNED( ch, type ) ( (ch)->stunned[(type)] > 0 )
#define MANA_COST( ch, sn )     ( IS_NPC( ch ) ? 0 : UMAX (                  \
				skill_table[sn].min_mana,                    \
				100 / ( 2 + ch->level -                      \
			skill_table[sn].skill_level[prime_class(ch)] ) ) )
#define SPELL_COST( ch, sn )	( is_class( ch, CLASS_VAMPIRE ) \
				? MANA_COST( ch, sn ) / 4 		 \
				: MANA_COST( ch, sn ) )

#define MT( ch )		( is_class( ch, CLASS_VAMPIRE ) ? ch->bp \
				  				: ch->mana )
#define MT_MAX( ch )		( is_class( ch, CLASS_VAMPIRE ) \
				? MAX_BP(ch)			\
				: MAX_MANA(ch) )
#define IS_SWITCHED( ch )       ( ch->pcdata->switched )

#define UNDEAD_TYPE( ch )	( IS_NPC( ch ) ? ACT_UNDEAD : PLR_UNDEAD )

#define MAX_HIT( ch ) 		( (ch)->perm_hit + (ch)->mod_hit )
#define MAX_MANA( ch ) 		( (ch)->perm_mana + (ch)->mod_mana )
#define MAX_BP( ch ) 		( (ch)->perm_bp + (ch)->mod_bp )
#define MAX_MOVE( ch ) 		( (ch)->perm_move + (ch)->mod_move )

#define CAN_FLY( ch )		( (ch->race == RACE_PIXIE) || (ch->race == RACE_ELDER) || \
				  (IS_AFFECTED( ch, AFF_FLYING )) )
/*
 * Object macros.
 */
#define CAN_WEAR( obj, part )	( IS_SET( ( obj)->wear_flags,  ( part ) ) )
#define IS_OBJ_STAT( obj, stat )( IS_SET( ( obj)->extra_flags, ( stat ) ) )
/* FOR NEW FLAGS */
#define IS_ANTI_CLASS( obj, stat )( IS_SET( (obj)->anti_class_flags, ( stat ) ) )
#define IS_ANTI_RACE( obj, stat )(IS_SET( (obj)->anti_race_flags, ( stat ) ) )


/*
 * Description macros.
 */
#define PERS( ch, looker )	( can_see( looker, ( ch ) ) ?		     \
				( IS_NPC( ch ) ? ( ch )->short_descr	     \
				               : ( ch )->name ) : "someone" )



/*
 * Arena macro.
 */
#define IS_ARENA(ch)	(!IS_NPC((ch)) && (ch)->in_room && \
			(ch)->in_room->area == arena.area && \
			((ch) == arena.fch || (ch) == arena.sch))

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
/*
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
*/


/*
 * Global constants.
 */
extern	const	struct	str_app_type	str_app		[ 31 ];
extern	const	struct	int_app_type	int_app		[ 31 ];
extern	const	struct	wis_app_type	wis_app		[ 31 ];
extern	const	struct	dex_app_type	dex_app		[ 31 ];
extern	const	struct	con_app_type	con_app		[ 31 ];

extern	const	struct	class_type	class_table	[ MAX_CLASS   ];
/*extern	const	struct	race_type	race_table	[ MAX_RACE    ];*/
extern  const   struct  wiznet_type     wiznet_table    [ ];
extern  const   struct  guild_data      guild_table     [ ];
extern  const   struct  quest_data      quest_table     [ ];
extern	struct	cmd_type	cmd_table	[ ];
extern	const	struct	liq_type	liq_table	[ LIQ_MAX     ];
extern	const	struct	skill_type	skill_table	[ MAX_SKILL ];
extern  const   struct  gskill_type     gskill_table    [ MAX_GSPELL  ];
/*extern	const	struct	social_type	social_table	[ ];*/
extern	char *	const			title_table	[ MAX_CLASS+3 ]
							[ MAX_LEVEL+1 ]
							[ 2 ];



/*
 * Global variables.
 */

extern		NEWBIE_DATA	  *	newbie_first;
extern		NEWBIE_DATA	  *	newbie_last;   /* Angi */

extern		PLAYERLIST_DATA	  *	playerlist; /* Decklarean */

extern		SOCIAL_DATA	  *	social_first; /* Decklarean */
extern		SOCIAL_DATA	  *	social_last;

extern		RACE_DATA	  *     first_race; /* Decklarean */

extern		HELP_DATA	  *	help_first;
extern		HELP_DATA	  *	help_last;
extern		HELP_DATA	  *	help_free;

extern		SHOP_DATA	  *	shop_first;

extern		BAN_DATA	  *	ban_list;
/*extern          USERL_DATA        *     user_list;*/  /* finger data/mailing */
extern		CHAR_DATA	  *	char_list;
extern		DESCRIPTOR_DATA   *	descriptor_list;
extern		NOTE_DATA	  *	note_list;
extern		OBJ_DATA	  *	object_list;
extern          TRAP_DATA         *     trap_list;  /* Altech trap stuff */

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
extern		ARENA_DATA		arena;
extern          char              *     down_time;
extern          char              *     warning1;
extern          char              *     warning2;
extern          int                     stype;
extern          int			prtv;
extern          bool 			quest;
extern          int			qmin;
extern		int			qmax;
extern          int                     port;

/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN( do_clanquest    );
DECLARE_DO_FUN( do_quest        );
DECLARE_DO_FUN( do_qecho        );
DECLARE_DO_FUN( do_qset         );
DECLARE_DO_FUN( do_questflag    );
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
DECLARE_DO_FUN( do_mpcommands   );
DECLARE_DO_FUN( do_mpteleport   );
DECLARE_DO_FUN( do_mptransfer   );
DECLARE_DO_FUN( do_opstat	);
DECLARE_DO_FUN( do_rpstat	);
DECLARE_DO_FUN( do_accept	);
DECLARE_DO_FUN( do_account      );
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN( do_affectedby   );
DECLARE_DO_FUN(	do_raffect	);
DECLARE_DO_FUN( do_afk		);
DECLARE_DO_FUN( do_afkmes       );
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN( do_ansi         );
DECLARE_DO_FUN(	do_answer	);
DECLARE_DO_FUN( do_antidote	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN( do_assasinate   );
DECLARE_DO_FUN( do_astat        );
DECLARE_DO_FUN( do_astrip       );
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN(	do_auction	);
DECLARE_DO_FUN( do_auto         );
DECLARE_DO_FUN( do_autoexit     );
DECLARE_DO_FUN( do_autocoins    );
DECLARE_DO_FUN( do_autoloot     );
DECLARE_DO_FUN( do_autosac      );
DECLARE_DO_FUN( do_autosplit    );
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN( do_bamf		);
/*DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	); */
DECLARE_DO_FUN(	do_bash 	);
DECLARE_DO_FUN( do_berserk      );
DECLARE_DO_FUN( do_bid          );
DECLARE_DO_FUN( do_blank        );
DECLARE_DO_FUN( do_bodybag      );
DECLARE_DO_FUN(	do_brandish	);
DECLARE_DO_FUN( do_brief        );
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN( do_challenge	);
DECLARE_DO_FUN( do_chameleon    );
DECLARE_DO_FUN( do_champlist 	);
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN( do_changes	);
DECLARE_DO_FUN(	do_channels	);
DECLARE_DO_FUN(	do_chat		);
DECLARE_DO_FUN( do_clan         );
DECLARE_DO_FUN( do_clone	);
DECLARE_DO_FUN( do_cinfo        );
DECLARE_DO_FUN( do_circle       );
DECLARE_DO_FUN( do_clans        );
DECLARE_DO_FUN( do_clandesc     );
DECLARE_DO_FUN( do_class        );
DECLARE_DO_FUN( do_cloak	);
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN( do_combat       );
DECLARE_DO_FUN( do_combine      );
DECLARE_DO_FUN(	do_commands	);
DECLARE_DO_FUN(	do_compare	);
DECLARE_DO_FUN( do_conference   );
DECLARE_DO_FUN(	do_config	);
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN( do_countcommands);
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN( do_cross_punch	);
DECLARE_DO_FUN(	do_cset		);
DECLARE_DO_FUN( do_darkinvis    );
DECLARE_DO_FUN( do_delet	);
DECLARE_DO_FUN( do_delete	);
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN( do_deposit      );
DECLARE_DO_FUN( do_desc_check	);
DECLARE_DO_FUN( do_descript_clean );
DECLARE_DO_FUN(	do_description	);
DECLARE_DO_FUN( do_detract	);
DECLARE_DO_FUN(	do_disarm	);
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN( do_donate	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN( do_dual         );
DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN( do_email	);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN( do_empower	);
DECLARE_DO_FUN( do_enter        );
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN( do_farsight     );
DECLARE_DO_FUN( do_feed         );
DECLARE_DO_FUN( do_fighting	);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN( do_finger       );
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN( do_flury 	);
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN( do_fullname     );
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN( do_gorge        );
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN( do_guard        );
DECLARE_DO_FUN( do_heighten     );
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN( do_hero         );
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN( do_high_kick	);
DECLARE_DO_FUN( do_hlist        );
DECLARE_DO_FUN(	do_holylight	);
#if defined( HOTREBOOT )
DECLARE_DO_FUN(	do_hotreboo	);
DECLARE_DO_FUN(	do_hotreboot	);
#endif
DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN(	do_ideas		);
DECLARE_DO_FUN( do_imc		);
DECLARE_DO_FUN( do_immlist	);
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN( do_indestructable);
DECLARE_DO_FUN( do_induct       );
DECLARE_DO_FUN( do_info		);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN( do_invoke       );
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN( do_irongrip     );
DECLARE_DO_FUN( do_jab_punch	);
DECLARE_DO_FUN( do_join         );
DECLARE_DO_FUN( do_jump_kick	);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN( do_kidney_punch	);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN( do_lowrecall );
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN( do_mental_drain );
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN(	do_mload	);
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN(	do_murde	);
DECLARE_DO_FUN(	do_murder	);
DECLARE_DO_FUN(	do_music	);
DECLARE_DO_FUN( do_newbie	);
DECLARE_DO_FUN( do_newcorpse	);
DECLARE_DO_FUN(	do_newlock	);
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN(	do_note		);
DECLARE_DO_FUN(	do_notell	);
DECLARE_DO_FUN( do_nukerep	);
DECLARE_DO_FUN(	do_numlock	);
DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN( do_olist	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN( do_ooc		);
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN( do_outcast      );
DECLARE_DO_FUN(	do_owhere	);
DECLARE_DO_FUN( do_pagelen      );
DECLARE_DO_FUN(	do_pardon	);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN( do_patch        );
DECLARE_DO_FUN(	do_peace	);
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN( do_pkill	);
DECLARE_DO_FUN( do_plan		);
DECLARE_DO_FUN( do_playerlist   );
DECLARE_DO_FUN( do_pload        );
DECLARE_DO_FUN(	do_poison_weapon);
DECLARE_DO_FUN(	do_pose		);
DECLARE_DO_FUN(	do_practice	);
DECLARE_DO_FUN( do_pray         );
DECLARE_DO_FUN( do_prompt       );
DECLARE_DO_FUN( do_punch        );
DECLARE_DO_FUN(	do_purge	);
DECLARE_DO_FUN(	do_put		);
DECLARE_DO_FUN( do_pwhere       );
DECLARE_DO_FUN(	do_quaff	);
DECLARE_DO_FUN(	do_question	);
DECLARE_DO_FUN(	do_qui		);
DECLARE_DO_FUN(	do_quit		);
DECLARE_DO_FUN(	do_race_edit	);
DECLARE_DO_FUN( do_racelist	);
DECLARE_DO_FUN(	do_reboo	);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN( do_rebuild	);
DECLARE_DO_FUN(	do_recall	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN( do_remake	);
DECLARE_DO_FUN( do_remote       );
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN( do_repair       );
DECLARE_DO_FUN(	do_reply	);
DECLARE_DO_FUN(	do_report	);
DECLARE_DO_FUN(	do_rescue	);
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN( do_restrict     );
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN( do_retreat	);
DECLARE_DO_FUN( do_retrieve     );
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN( do_roundhouse_punch);
DECLARE_DO_FUN(	do_rset		);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN(	do_sacrifice	);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN( do_seize	);
DECLARE_DO_FUN( do_separate     );
DECLARE_DO_FUN( do_setlev       );
DECLARE_DO_FUN( do_shadow       );
DECLARE_DO_FUN( do_shadow_walk  );
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN(	do_silence	);
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN( do_slaymes   	);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN( do_slist        );
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN( do_smash        );
DECLARE_DO_FUN( do_snatch	);
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN(	do_socials	);
DECLARE_DO_FUN( do_soulstrike   );
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN( do_spells       );
DECLARE_DO_FUN( do_spin_kick	);
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN( do_sstat        );
DECLARE_DO_FUN(	do_sstime	);
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN(	do_steal	);
DECLARE_DO_FUN( do_store        );
DECLARE_DO_FUN( do_stun         );
DECLARE_DO_FUN(	do_switch	);
DECLARE_DO_FUN(	do_tell		);
DECLARE_DO_FUN( do_telnetga     );
DECLARE_DO_FUN( do_throw        );
DECLARE_DO_FUN( do_trip		);
DECLARE_DO_FUN(	do_time		);
DECLARE_DO_FUN(	do_title	);
DECLARE_DO_FUN( do_todo         );
DECLARE_DO_FUN( do_track        );
DECLARE_DO_FUN(	do_train	);
DECLARE_DO_FUN(	do_transfer	);
DECLARE_DO_FUN( do_transport    );
DECLARE_DO_FUN(	do_trmes	);
DECLARE_DO_FUN(	do_trust	);
DECLARE_DO_FUN(	do_typo		);
DECLARE_DO_FUN(	do_typos		);
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN( do_uppercut_punch);
DECLARE_DO_FUN(	do_users	);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN( do_vent         );
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN( do_voodo        );
DECLARE_DO_FUN( do_vused        );
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN( do_ward		);
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN(	do_where	);
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN( do_whotype	);
DECLARE_DO_FUN(	do_wimpy	);
DECLARE_DO_FUN( do_withdraw     );
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN( do_wizify       );
/*DECLARE_DO_FUN( do_wizlist      ); */
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN( do_wiznet	);
DECLARE_DO_FUN( do_worth        );
DECLARE_DO_FUN( do_wrlist       );
DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN(	do_zap		);
DECLARE_DO_FUN( do_stare        );
/* XOR */
/*DECLARE_DO_FUN( do_vnum		);*/
DECLARE_DO_FUN( do_load		);
DECLARE_DO_FUN( do_push		);
DECLARE_DO_FUN( do_drag		);
DECLARE_DO_FUN( do_guild	);
DECLARE_DO_FUN( do_guilds       );
DECLARE_DO_FUN( do_unguild	);
DECLARE_DO_FUN( do_setrank	);
DECLARE_DO_FUN( do_gdt		);
DECLARE_DO_FUN( do_authorize	);
DECLARE_DO_FUN( do_drain_life	);
DECLARE_DO_FUN( do_gouge        );
DECLARE_DO_FUN( do_alchemy      );
DECLARE_DO_FUN( do_scribe       );
DECLARE_DO_FUN( do_multiburst	);
/* Necro & Werewolf skills by Hannibal */
DECLARE_DO_FUN( do_gravebind	);
DECLARE_DO_FUN( do_rage		);
DECLARE_DO_FUN( do_bite		);
DECLARE_DO_FUN( do_rush		);
DECLARE_DO_FUN( do_howl_of_fear	);
DECLARE_DO_FUN( do_scent	);
DECLARE_DO_FUN( do_frenzy	);
DECLARE_DO_FUN( do_reflex	);
DECLARE_DO_FUN( do_palm		);
DECLARE_DO_FUN(	do_rake		);
/*
 * Racial Skills Start Here -- Hannibal
 */
DECLARE_DO_FUN( do_headbutt 	);  /* Minotaur */
DECLARE_DO_FUN( do_globedarkness);  /* Drow */
DECLARE_DO_FUN( do_drowfire	);  /* Drow */
DECLARE_DO_FUN( do_forge	);  /* Dwarf */
DECLARE_DO_FUN( do_spit		);  /* Liz. */
/* Monk skills -- Hannibal */
DECLARE_DO_FUN( do_flamehand	);
DECLARE_DO_FUN( do_frosthand	);
DECLARE_DO_FUN( do_chaoshand	);
DECLARE_DO_FUN( do_bladepalm	);
DECLARE_DO_FUN( do_flyingkick	);
DECLARE_DO_FUN( do_nerve	);
DECLARE_DO_FUN( do_ironfist	);
/* Bard skill */
DECLARE_DO_FUN( do_shriek	);
/* Ban Functions -- Hannibal */
DECLARE_DO_FUN(	do_permban	);
DECLARE_DO_FUN(	do_tempban	);
DECLARE_DO_FUN(	do_newban	);
DECLARE_DO_FUN( do_banlist	);
/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(	spell_acid_blast	);
DECLARE_SPELL_FUN(	spell_aid		);
DECLARE_SPELL_FUN(      spell_animate           );
DECLARE_SPELL_FUN(	spell_armor		);
DECLARE_SPELL_FUN(      spell_astral            );
DECLARE_SPELL_FUN(      spell_aura              );
DECLARE_SPELL_FUN(	spell_bark_skin		);
DECLARE_SPELL_FUN(	spell_bless		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(      spell_blood_bath        );
DECLARE_SPELL_FUN(	spell_blur		);
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_call_lightning	);
DECLARE_SPELL_FUN(	spell_cause_critical	);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(	spell_change_sex	);
DECLARE_SPELL_FUN(	spell_chant		);
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_chill_touch	);
DECLARE_SPELL_FUN(	spell_colour_spray	);
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
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(	spell_dispel_magic	);
DECLARE_SPELL_FUN(	spell_draw_strength	);
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(      spell_fireshield        );
DECLARE_SPELL_FUN(	spell_firewall		);
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(	spell_fly		);
DECLARE_SPELL_FUN(	spell_gate		);
DECLARE_SPELL_FUN(	spell_general_purpose	);
DECLARE_SPELL_FUN(	spell_giant_strength	);
DECLARE_SPELL_FUN(      spell_eternal_intellect );
DECLARE_SPELL_FUN(	spell_golden_aura	);
DECLARE_SPELL_FUN(	spell_group_healing	);
DECLARE_SPELL_FUN(	spell_haste		);
DECLARE_SPELL_FUN(      spell_goodberry         );
DECLARE_SPELL_FUN(	spell_harm		);
DECLARE_SPELL_FUN(	spell_heal		);
DECLARE_SPELL_FUN(	spell_high_explosive	);
DECLARE_SPELL_FUN(	spell_iceshield		);
DECLARE_SPELL_FUN(      spell_icestorm          );
DECLARE_SPELL_FUN(	spell_identify		);
DECLARE_SPELL_FUN(      spell_incinerate        );
DECLARE_SPELL_FUN(	spell_infravision	);
DECLARE_SPELL_FUN(	spell_inspiration 	);
DECLARE_SPELL_FUN(	spell_invis		);
DECLARE_SPELL_FUN(	spell_know_alignment	);
DECLARE_SPELL_FUN(	spell_lightning_bolt	);
DECLARE_SPELL_FUN(	spell_locate_object	);
DECLARE_SPELL_FUN(	spell_magic_missile	);
DECLARE_SPELL_FUN(      spell_mana              );
DECLARE_SPELL_FUN(	spell_mass_invis	);
DECLARE_SPELL_FUN(      spell_mental_block      );
DECLARE_SPELL_FUN(	spell_pass_door		);
DECLARE_SPELL_FUN(      spell_permenancy        );
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(      spell_portal            );
DECLARE_SPELL_FUN(	spell_protection	);
DECLARE_SPELL_FUN(	spell_refresh		);
DECLARE_SPELL_FUN(	spell_remove_curse	);
DECLARE_SPELL_FUN(	spell_sanctuary		);
DECLARE_SPELL_FUN(      spell_scry              );
DECLARE_SPELL_FUN(	spell_shocking_grasp	);
DECLARE_SPELL_FUN(      spell_shockshield       );
DECLARE_SPELL_FUN(	spell_shield		);
DECLARE_SPELL_FUN(	spell_sleep		);
DECLARE_SPELL_FUN(      spell_spell_bind        );
DECLARE_SPELL_FUN(	spell_stone_skin	);
DECLARE_SPELL_FUN(	spell_summon		);
DECLARE_SPELL_FUN(	spell_teleport		);
DECLARE_SPELL_FUN(	spell_thunder_strike	);
DECLARE_SPELL_FUN(      spell_turn_undead       );
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(      spell_vibrate           );
DECLARE_SPELL_FUN(	spell_war_cry		);
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(	spell_word_of_recall	);
DECLARE_SPELL_FUN(	spell_acid_breath	);
DECLARE_SPELL_FUN(	spell_fire_breath	);
DECLARE_SPELL_FUN(	spell_frost_breath	);
DECLARE_SPELL_FUN(	spell_gas_breath	);
DECLARE_SPELL_FUN(	spell_lightning_breath	);

DECLARE_SPELL_FUN(	spell_summon_swarm	);	/* XOR */
DECLARE_SPELL_FUN(	spell_summon_pack	);	/* XOR */
DECLARE_SPELL_FUN(	spell_summon_demon	);	/* XOR */
DECLARE_SPELL_FUN(	spell_cancellation	);	/* XOR */
DECLARE_SPELL_FUN(	spell_detect_good	);	/* XOR */
DECLARE_SPELL_FUN(	spell_protection_good	);	/* XOR */
DECLARE_SPELL_FUN(	spell_enchanted_song	);	/* XOR */
DECLARE_SPELL_FUN(	spell_holy_strength	);	/* XOR */
DECLARE_SPELL_FUN(	spell_curse_of_nature	);	/* XOR */
DECLARE_SPELL_FUN(      spell_holysword        );      /* ELVIS */
DECLARE_SPELL_FUN(      spell_summon_angel      );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_holy_fires        );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_truesight         );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_bladebarrier      );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_flame_blade       );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_chaos_blade       );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_frost_blade       );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_web               );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_entangle          );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_darkbless         );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_confusion         );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_bio_acceleration  );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_mind_probe        );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_chain_lightning   );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_meteor_swarm      );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_psychic_quake     );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_fumble            );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_dancing_lights    );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_summon_shadow     );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_summon_beast      );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_summon_trent      );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_shatter           );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_molecular_unbind  );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_phase_shift       );     /* ELVIS */
DECLARE_SPELL_FUN(      spell_healing_hands     );     /* -- Altrag */
DECLARE_SPELL_FUN(      spell_mist_form         );     /* Lacey */
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
DECLARE_SPELL_FUN(      spell_chaosfield        );
DECLARE_SPELL_FUN(      spell_combat_mind       );
DECLARE_SPELL_FUN(      spell_complete_healing  );
DECLARE_SPELL_FUN(      spell_control_flames    );
DECLARE_SPELL_FUN(      spell_create_sound      );
DECLARE_SPELL_FUN(      spell_death_field       );
DECLARE_SPELL_FUN(      spell_detonate          );
DECLARE_SPELL_FUN(      spell_disintegrate      );
DECLARE_SPELL_FUN(      spell_displacement      );
DECLARE_SPELL_FUN(      spell_disrupt           );
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
DECLARE_SPELL_FUN(     gspell_flamesphere       );
DECLARE_SPELL_FUN(     gspell_mass_shield       );
/* Necromancer & Monk spells by Hannibal. */
DECLARE_SPELL_FUN(	spell_hex		);
DECLARE_SPELL_FUN(	spell_dark_ritual	);
DECLARE_SPELL_FUN(	spell_field_of_decay	);
DECLARE_SPELL_FUN(	spell_stench_of_decay	);
DECLARE_SPELL_FUN(	spell_iron_skin		);
DECLARE_SPELL_FUN(	spell_chi_shield	);
DECLARE_SPELL_FUN(    spell_pass_plant 		); /*Deck*/

DECLARE_SPELL_FUN(    spell_soul_bind 		); /*Malaclypse*/

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
// char *	crypt		args( ( const char *key, const char *salt ) );
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
size_t	fread	args( ( void *ptr, size_t size, size_t n, FILE *stream ) );
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

#if defined( linux )
#define MOB_DIR		"../MOBProgs/"  	/* MOBProg files	*/
#define NULL_FILE       "/dev/null"
#endif

#define AREA_LIST	"area.lst"	/* List of areas		*/
#define BAN_LIST	"../banned.lst" /* List of banned sites & users */
#define BUG_FILE	"bugs.txt"      /* For 'bug' and bug( )		*/
#define IDEA_FILE	"ideas.txt"	/* For 'idea'			*/
#define TYPO_FILE	"typos.txt"     /* For 'typo'			*/
#define NOTE_FILE	"notes.txt"	/* For 'notes'			*/
#define CLAN_FILE       "clan.dat"      /* For 'clans'                  */
#define SOCIAL_FILE	"social.dat"	/* For 'socials'		*/
#define RACE_FILE		"race.dat"		/* For 'races'		*/
#define SHUTDOWN_FILE	"shutdown.txt"	/* For 'shutdown'		*/
#define DOWN_TIME_FILE  "time.txt"      /* For automatic shutdown       */
#define USERLIST_FILE   "users.txt"     /* Userlist -- using identd TRI */
#define AUTH_LIST       "auth.txt"      /* List of who auth who         */
#define PLAYERLIST_FILE "player.lst"    /* Player List 			*/
#define NEWBIE_FILE     "newbie.dat"    /* Newbie help file		*/
#if defined( HOTREBOOT )
#define HOTREBOOT_FILE 	"hotreboot.dat"  /* temporary data file used 	*/
#endif
/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define CID     CLAN_DATA
#define SF	SPEC_FUN
#define BD	BAN_DATA
/* act_comm.c */
void	add_follower	args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	stop_follower	args( ( CHAR_DATA *ch ) );
void	die_follower	args( ( CHAR_DATA *ch, char *name ) );
bool	is_same_group	args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );

/* act_info.c */
void	set_title	args( ( CHAR_DATA *ch, char *title ) );
bool	check_blind	args( ( CHAR_DATA *ch ) );

#ifdef NEW_MONEY
MONEY_DATA *add_money    args( ( MONEY_DATA *a, MONEY_DATA *b ) );
MONEY_DATA *sub_money	 args( ( MONEY_DATA *a, MONEY_DATA *b ) );
MONEY_DATA *take_money   args( ( CHAR_DATA *ch, int amt, char *type, char *verb ) );
MONEY_DATA *spend_money  args( ( MONEY_DATA *a, MONEY_DATA *b ) );
char       *money_string  args( ( MONEY_DATA *money ) );
#endif

/* act_move.c */
void	move_char	args( ( CHAR_DATA *ch, int door, bool Fall ) );

/* act_obj.c */

/* act_wiz.c */
ROOM_INDEX_DATA *	find_location	args( ( CHAR_DATA *ch, char *arg ) );
void wiznet             args( (char *string, CHAR_DATA *ch, OBJ_DATA *obj,
                               long flag, long flag_skip, int min_level ) );

/* comm.c */
void	close_socket	 args( ( DESCRIPTOR_DATA *dclose ) );
void	write_to_buffer	 args( ( DESCRIPTOR_DATA *d, const char *txt,
				int length ) );
void    send_to_all_char args( ( const char *text ) );
void    send_to_al       args( ( int clr, int level, char *text ) );
/* send to above level---^   TRI */
void	send_to_char	 args( ( int AType, const char *txt, CHAR_DATA *ch ) );
void    set_char_color   args( ( int AType, CHAR_DATA *ch ) );
void    show_string      args( ( DESCRIPTOR_DATA *d, char *input ) );
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
char *	get_extra_descr	args( ( CHAR_DATA *CH, char *name, EXTRA_DESCR_DATA *ed ) );
MID *	get_mob_index	args( ( int vnum ) );
OID *	get_obj_index	args( ( int vnum ) );
RID *	get_room_index	args( ( int vnum ) );
CID *   get_clan_index  args( ( int vnum ) );
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
bool	str_cmp_ast	args( ( const char *astr, const char *bstr ) );
bool	str_prefix	args( ( const char *astr, const char *bstr ) );
bool	str_infix	args( ( const char *astr, const char *bstr ) );
bool	str_suffix	args( ( const char *astr, const char *bstr ) );
char *	capitalize	args( ( const char *str ) );
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	info		args( ( const char *str, int param1, int param2 ) );
void	challenge	args( ( const char *str, int param1, int param2 ) );
void	bug		args( ( const char *str, int param ) );
void    logch           args( ( char *l_str, int l_type, int lvl ) );
void	log_string	args( ( char *str, int l_type, int level ) );
void	tail_chain	args( ( void ) );
void    clone_mobile    args( ( CHAR_DATA *parent, CHAR_DATA *clone) );
void    clone_object    args( ( OBJ_DATA *parent, OBJ_DATA *clone ) );
void	parse_ban       args( ( char *argument, BAN_DATA *banned ) );
void    arena_chann	args( ( const char *str, int param1, int param2 ) );

/* fight.c */
void	violence_update	args( ( void ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	damage		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			       int dt ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
void	raw_kill	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
bool    is_safe         args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

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
int	xp_tolvl	args( ( CHAR_DATA *ch ) );
bool	is_name		args( (  CHAR_DATA *ch, char *str, char *namelist) );
bool    is_exact_name   args( ( char *str, char *namelist ) );
void	affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_to_char2	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_remove2  args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_strip	args( ( CHAR_DATA *ch, int sn ) );
bool	is_affected	args( ( CHAR_DATA *ch, int sn ) );
void	affect_join	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_join2    args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	char_from_room	args( ( CHAR_DATA *ch ) );
void	char_to_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    obj_to_storage  args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_from_char	args( ( OBJ_DATA *obj ) );
void    obj_from_storage args(( OBJ_DATA *obj ) );
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
CD *	get_pc_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_type	args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *	get_obj_list	args( ( CHAR_DATA *ch, char *argument,
			       OBJ_DATA *list ) );
OD *	get_obj_carry	args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_storage args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_wear	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_here	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_world	args( ( CHAR_DATA *ch, char *argument ) );

#ifdef NEW_MONEY
OD *  	create_money	args( ( MONEY_DATA *amount ) );
#else
OD *	create_money	args( ( int amount ) );
#endif

int	get_obj_number	args( ( OBJ_DATA *obj ) );
int	get_obj_weight	args( ( OBJ_DATA *obj ) );
bool	room_is_dark	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	room_is_private	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *	item_type_name	args( ( OBJ_DATA *obj ) );
char *	affect_loc_name	args( ( int location ) );
char *	affect_bit_name	args( ( int vector ) );
char * affect_bit_name2 args( ( int vector ) );
char *	extra_bit_name	args( ( int extra_flags ) );
char *	anticlass_bit_name args( ( int anti_class_flags ) );
char *	antirace_bit_name  args( ( int anti_race_flags ) );
char *  act_bit_name    args( ( int act ) );
char *  imm_bit_name    args( ( int ) );	/* XOR */
CD   *  get_char        args( ( CHAR_DATA *ch ) );
bool    longstring      args( ( CHAR_DATA *ch, char *argument ) );
void    end_of_game     args( ( void ) );
long    wiznet_lookup   args( ( const char *name) );
int	strlen_wo_col	args( ( char *argument ) );
char *	strip_color	args( ( char *argument ) );
/* Multiclass stuff -- Hann */
bool	can_use_skpell	args( ( CHAR_DATA *ch, int sn ) );
bool	is_class	args( ( CHAR_DATA *ch, int class ) );
bool	has_spells	args( ( CHAR_DATA *ch ) );
int	prime_class	args( ( CHAR_DATA *ch ) );
int	number_classes	args( ( CHAR_DATA *ch ) );
char *	class_long	args( ( CHAR_DATA *ch ) );
char *	class_numbers	args( ( CHAR_DATA *ch, bool pSave ) );
char *	class_short	args( ( CHAR_DATA *ch ) );
bool	gets_zapped	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
/* Room affect stuff (act_room.c) -- Hann */
void    raffect_to_room args( ( ROOM_INDEX_DATA *room, CHAR_DATA *ch,
			ROOM_AFFECT_DATA *raf ) );
void	raffect_remove	args( ( ROOM_INDEX_DATA *room, CHAR_DATA *ch,
			ROOM_AFFECT_DATA *raf ) );
void	raffect_remall	args( ( CHAR_DATA *ch ) );
bool	is_raffected	args( ( ROOM_INDEX_DATA *room, int sn ) );
void	toggle_raffects	args( ( ROOM_INDEX_DATA *room ) );
void	loc_off_raf	args( ( CHAR_DATA *ch, int type, bool rOff ) );
/* interp.c */
void	interpret	args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number	args( ( char *arg ) );
int	number_argument	args( ( char *argument, char *arg ) );
char *	one_argument	args( ( char *argument, char *arg_first ) );
bool    IS_SWITCHED     args( ( CHAR_DATA *ch ) );
void	arena_master	args( ( CHAR_DATA *ch, char *argument, char *arg2 ) );

/* magic.c */
int     slot_lookup     args( ( int slot ) );
bool    is_sn           args( ( int sn ) );
int	skill_lookup	args( ( const char *name ) );
bool	saves_spell	args( ( int level, CHAR_DATA *victim ) );
void	obj_cast_spell	args( ( int sn, int level, CHAR_DATA *ch,
			       CHAR_DATA *victim, OBJ_DATA *obj ) );
void    update_skpell   args( ( CHAR_DATA *ch, int sn ) );
void    do_acspell      args( ( CHAR_DATA *ch, OBJ_DATA *pObj,
			        char *argument ) );

/* save.c */
bool    pstat            args( ( char *name ) );
void	save_char_obj	args( ( CHAR_DATA *ch, bool leftgame ) );
bool	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name ) );
void    corpse_back     args( ( CHAR_DATA *ch, OBJ_DATA *corpse ) );
void    read_finger	args( ( CHAR_DATA *ch, char *argument ) );
void    save_finger     args( ( CHAR_DATA *ch ) );
void    fwrite_finger   args( ( CHAR_DATA *ch, FILE *fp ) );
void    fread_finger    args( ( CHAR_DATA *ch, FILE *fp ) );
void	save_banlist	args( ( BAN_DATA *ban_list ) );

/* special.c */
SF *	spec_lookup	args( ( const char *name ) );

/* update.c */
void	advance_level	args( ( CHAR_DATA *ch ) );
void	gain_exp	args( ( CHAR_DATA *ch, int gain ) );
void	gain_condition	args( ( CHAR_DATA *ch, int iCond, int value ) );
void	update_handler	args( ( void ) );

/* mob_prog.c */
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
#ifdef NEW_MONEY
void	mprog_bribe_trigger	args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
					MONEY_DATA *amount ) );
#else
void    mprog_bribe_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
		                        int amount ) );
#endif
void    mprog_entry_trigger     args ( ( CHAR_DATA* mob ) );
void    mprog_give_trigger      args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
                		        OBJ_DATA* obj ) );
void    mprog_greet_trigger     args ( ( CHAR_DATA* mob ) );
void    mprog_fight_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_hitprcnt_trigger  args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_death_trigger     args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void    mprog_random_trigger    args ( ( CHAR_DATA* mob ) );
void    mprog_speech_trigger    args ( ( char* txt, CHAR_DATA* mob ) );

/*
 * Lotsa triggers for ore_progs.. (ore_prog.c)
 * -- Altrag
 */
/*
 * Object triggers
 */
void    oprog_get_trigger       args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_get_from_trigger  args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 OBJ_DATA *secondary ) );
void    oprog_give_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 CHAR_DATA *victim ) );
void    oprog_drop_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_put_trigger       args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 OBJ_DATA *secondary ) );
void    oprog_fill_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 OBJ_DATA *spring ) );
void    oprog_wear_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_look_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_look_in_trigger   args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_invoke_trigger    args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 void *vo ) );
void    oprog_use_trigger       args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 void *vo ) );
void    oprog_cast_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_cast_sn_trigger   args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 int sn, void *vo ) );
void    oprog_join_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 OBJ_DATA *secondary ) );
void    oprog_separate_trigger  args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_buy_trigger       args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 CHAR_DATA *vendor ) );
void    oprog_sell_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 CHAR_DATA *vendor ) );
void    oprog_store_trigger     args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_retrieve_trigger  args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_open_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_close_trigger     args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_lock_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 OBJ_DATA *key ) );
void    oprog_unlock_trigger    args ( ( OBJ_DATA *obj, CHAR_DATA *ch,
					 OBJ_DATA *key ) );
void    oprog_pick_trigger      args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    oprog_random_trigger    args ( ( OBJ_DATA *obj ) );
void    oprog_throw_trigger     args ( ( OBJ_DATA *obj, CHAR_DATA *ch ) );

/*
 * Room triggers
 */
void    rprog_enter_trigger     args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    rprog_exit_trigger      args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    rprog_pass_trigger      args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    rprog_cast_trigger      args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    rprog_cast_sn_trigger   args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, int sn,
					 void *vo ) );
void    rprog_sleep_trigger     args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    rprog_wake_trigger      args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    rprog_rest_trigger      args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    rprog_death_trigger     args ( ( ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    rprog_time_trigger      args ( ( ROOM_INDEX_DATA *room, int hour ) );
void    rprog_random_trigger    args ( ( ROOM_INDEX_DATA *room ) );

/*
 * Exit triggers
 */
void    eprog_enter_trigger     args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_exit_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_pass_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, bool fEnter ) );
void    eprog_look_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_scry_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_open_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_close_trigger     args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );
void    eprog_lock_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, OBJ_DATA *obj ) );
void    eprog_unlock_trigger    args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch, OBJ_DATA *obj ) );
void    eprog_pick_trigger      args ( ( EXIT_DATA *pExit,
					 ROOM_INDEX_DATA *room,
					 CHAR_DATA *ch ) );

/*
 * gr_magic.c
 * -- Altrag
 */
void    check_gcast             args ( ( CHAR_DATA *ch ) );
void    group_cast              args ( ( int sn, int level, CHAR_DATA *ch,
					 char *argument ) );
void    set_gspell              args ( ( CHAR_DATA *ch, GSPELL_DATA *gsp ) );
void    end_gspell              args ( ( CHAR_DATA *ch ) );

/*
 * quest.c
 * --Angi
 */
bool    chance                  args ( ( int num ) );

/*
 * track.c
 */
void    hunt_victim             args ( ( CHAR_DATA *ch ) );
bool    can_go                  args ( ( CHAR_DATA *ch, int dir ) );

/*
 * chatmode.c
 */
void    start_chat_mode         args ( ( DESCRIPTOR_DATA *d ) );
void    chat_interp             args ( ( CHAR_DATA *ch, char *argument ) );

#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
#undef  BD

/*****************************************************************************
 *                                    OLC                                    *
 *****************************************************************************/

/*
 * This structure is used in special.c to lookup spec funcs and
 * also in olc_act.c to display listings of spec funcs.
 */
struct spec_type
{
    char *	spec_name;
    SPEC_FUN *	spec_fun;
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
#define OBJ_VNUM_DUMMY	1



/*
 * Area flags.
 */
#define         AREA_NONE       0
#define         AREA_CHANGED    1	/* Area has been modified. */
#define         AREA_ADDED      2	/* Area has been added to. */
#define         AREA_LOADING    4	/* Used for counting in db.c */
#define		AREA_VERBOSE	8	/* Used for saving in save.c */
#define	 	AREA_PROTOTYPE 16       /* Prototype area(no mortals) */
#define		AREA_CLAN_HQ   32	/* Area is a CLAN HQ */
#define		AREA_NO_QUEST  64	/* No quests allowed */
#define 	AREA_MUDSCHOOL 128	/* Used for mudschool only */

#define MAX_DIR	6
#define NO_FLAG -99	/* Must not be used in flags or stats. */



/*
 * Interp.c
 */
DECLARE_DO_FUN( do_aedit        );	/* OLC 1.1b */
DECLARE_DO_FUN( do_redit        );	/* OLC 1.1b */
DECLARE_DO_FUN( do_oedit        );	/* OLC 1.1b */
DECLARE_DO_FUN( do_medit        );	/* OLC 1.1b */
DECLARE_DO_FUN( do_cedit        );      /* IchiCode 1.1b */
DECLARE_DO_FUN( do_hedit        );      /* XOR 3.14159265359r^2 */
DECLARE_DO_FUN( do_sedit	);	/* Decklarean */
DECLARE_DO_FUN( do_spedit	);	/* Decklarean */
DECLARE_DO_FUN( do_rename_obj	);	/* Decklarean */
DECLARE_DO_FUN( do_race_edit	);	/* Decklarean */
DECLARE_DO_FUN( do_mreset	);	/* Decklarean */
DECLARE_DO_FUN( do_nedit	);	/* Angi */
DECLARE_DO_FUN( do_asave	);
DECLARE_DO_FUN( do_alist	);
DECLARE_DO_FUN( do_resets	);
DECLARE_DO_FUN( do_alias        );
DECLARE_DO_FUN( do_clear        );      /* Angi */




/*
 * Global Constants
 */
extern	char *	const	dir_name        [];
extern	const	int	rev_dir         [];
extern	const	struct	spec_type	spec_table	[];



/*
 * Global variables
 */
extern          AREA_DATA *             area_first;
extern          AREA_DATA *             area_last;
extern          CLAN_DATA *             clan_first;
extern  	SHOP_DATA *             shop_last;

extern          int                     top_affect;
extern          int                     top_area;
extern          int                     top_ed;
extern          int                     top_exit;
extern          int                     top_help;
extern          int                     top_mob_index;
extern		int			mobs_in_game;
extern          int                     top_obj_index;
extern          int                     top_reset;
extern          int                     top_room;
extern          int                     top_shop;
extern          int                     top_clan;
extern		int			top_social;
extern		int			top_race;
extern		int			top_newbie;
extern		int			top_trap;
extern		int			top_mob_prog;

/*extern          int                     top_vnum_mob;*/
/*extern          int                     top_vnum_obj; */
/*extern          int                     top_vnum_room;*/

extern          char                    str_empty       [1];

extern  MOB_INDEX_DATA *        mob_index_hash  [MAX_KEY_HASH];
extern  OBJ_INDEX_DATA *        obj_index_hash  [MAX_KEY_HASH];
extern  ROOM_INDEX_DATA *       room_index_hash [MAX_KEY_HASH];


/* db.c */
void	reset_area      args( ( AREA_DATA * pArea ) );
void	reset_room	args( ( ROOM_INDEX_DATA *pRoom ) );

/* string.c */
void	string_edit	args( ( CHAR_DATA *ch, char **pString ) );
void    string_append   args( ( CHAR_DATA *ch, char **pString ) );
char *	string_replace	args( ( char * orig, char * old, char * new ) );
void    string_add      args( ( CHAR_DATA *ch, char *argument ) );
char *  format_string   args( ( char *oldstring /*, bool fSpace */ ) );
char *  first_arg       args( ( char *argument, char *arg_first, bool fCase ) );
char *	string_unpad	args( ( char * argument ) );
char *	string_proper	args( ( char * argument ) );
char *	all_capitalize	args( ( const char *str ) );	/* OLC 1.1b */
char *  string_delline  args( ( CHAR_DATA *ch, char *argument, char *old ) );
char * string_insline   args( ( CHAR_DATA *ch, char *argument, char *old ) );
/* olc.c */
bool	run_olc_editor	args( ( DESCRIPTOR_DATA *d ) );
char	*olc_ed_name	args( ( CHAR_DATA *ch ) );
char	*olc_ed_vnum	args( ( CHAR_DATA *ch ) );
AREA_DATA  *get_area_data       args( ( int vnum ) );
void 	purge_area	args( ( AREA_DATA * pArea ) ); /* Angi */

/* special.c */
char *	spec_string	args( ( SPEC_FUN *fun ) );	/* OLC */

/* bit.c */
extern const struct flag_type 	area_flags[];
extern const struct flag_type	sex_flags[];
extern const struct flag_type	exit_flags[];
extern const struct flag_type	door_resets[];
extern const struct flag_type	room_flags[];
extern const struct flag_type	sector_flags[];
extern const struct flag_type	type_flags[];
extern const struct flag_type	extra_flags[];
/* FOR NEW FLAGS */
extern const struct flag_type	anti_race_flags[];
extern const struct flag_type	anti_class_flags[];
extern const struct flag_type	wear_flags[];
extern const struct flag_type	act_flags[];
extern const struct flag_type	affect_flags[];
extern const struct flag_type   affect2_flags[];
extern const struct flag_type	apply_flags[];
extern const struct flag_type	wear_loc_strings[];
extern const struct flag_type	wear_loc_flags[];
extern const struct flag_type	weapon_flags[];
extern const struct flag_type	container_flags[];
extern const struct flag_type	liquid_flags[];
extern const struct flag_type   immune_flags[];
extern const struct flag_type   mprog_types[];
extern const struct flag_type   oprog_types[];
extern const struct flag_type   rprog_types[];
extern const struct flag_type   eprog_types[];

/* olc_act.c */
extern int flag_value       args ( ( const struct flag_type *flag_table,
				     char *argument ) );
extern AFFECT_DATA  *new_affect   args ( ( void ) );
extern OBJ_INDEX_DATA    *new_obj_index          args ( ( void ) );
extern void free_affect		  args ( ( AFFECT_DATA *pAf ) );
extern EXTRA_DESCR_DATA *new_extra_descr args( ( void ) );
extern void free_extra_descr	  args ( ( EXTRA_DESCR_DATA *pExtra ) );
extern void free_obj_data	  args ( ( OBJ_DATA *pObj ) );
extern OBJ_DATA *new_obj_data	  args ( ( void ) );

extern RESET_DATA *new_reset_data args ( ( void ) ) ;

extern void check_nofloor         args ( ( CHAR_DATA *ch ) );
extern char *flag_string     args ( ( const struct flag_type *flag_table,
 				      int bits ) );
extern void save_clans            args ( ( ) );
extern void save_social 	  args ( ( ) );
extern void save_race	 	  args ( ( ) );
extern RACE_DATA *new_race_data	  args ( ( void ) );
extern void race_sort		  args ( ( RACE_DATA *pArea ) );
extern RACE_DATA *get_race_data	args ( ( int vnum ) );
extern void save_helps		  args ( ( ) );
extern void wind_update           args ( ( AREA_DATA *pArea ) );
extern void send_to_area          args ( ( AREA_DATA *pArea, char *txt ) );
extern ALIAS_DATA *new_alias      args ( ( ) );
extern void        free_alias     args ( ( ALIAS_DATA *pAl ) );

extern void save_player_list	  args ( ( ) ); /*Decklarean */
extern void save_newbie		  args ( ( ) ); /* Angi */
