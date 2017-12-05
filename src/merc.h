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



/*
 * Accommodate old non-Ansi compilers.
 */
#if defined( TRADITIONAL )
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_MOB_FUN( fun )		bool fun( )
#define DECLARE_OBJ_FUN( fun )		bool fun( )
#define DECLARE_ROOM_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#define DECLARE_GAME_FUN( fun ) 	void fun( )
#define DECLARE_HIT_FUN( fun )          void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_MOB_FUN( fun )		MOB_FUN   fun
#define DECLARE_OBJ_FUN( fun )		OBJ_FUN   fun
#define DECLARE_ROOM_FUN( fun )		ROOM_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#define DECLARE_GAME_FUN( fun )		GAME_FUN  fun
#define DECLARE_HIT_FUN( fun )          HIT_FUN   fun
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
typedef int				unlong;
typedef int				bool;
typedef	unsigned char			u_intc;
#define unix
#else
typedef unsigned long			unlong;
typedef unsigned char			bool;
typedef	unsigned char			u_intc;
#endif


/*
 * Ok here we define strdup so it can no longer be confused
 * with str_dup.  Suggested by erwin@pip.dknet.dk - Kahn.
 */
#define strdup  STRDUP_ERROR__USE_STR_DUP!

#include <limits.h>

/*
 * Bit defines, so you don't have to recalculate/think up
 * all the bitvalues every time  -- Zen --
 */
#define		BV00			(1 <<  0)	/*          1 */
#define		BV01			(1 <<  1)	/*          2 */
#define		BV02			(1 <<  2)	/*          4 */
#define		BV03			(1 <<  3)	/*          8 */
#define		BV04			(1 <<  4)	/*         16 */
#define		BV05			(1 <<  5)	/*         32 */
#define		BV06			(1 <<  6)	/*         64 */
#define		BV07			(1 <<  7)	/*        128 */
#define		BV08			(1 <<  8)	/*        256 */
#define		BV09			(1 <<  9)	/*        512 */
#define		BV10			(1 << 10)	/*       1024 */
#define		BV11			(1 << 11)	/*       2048 */
#define		BV12			(1 << 12)	/*       4096 */
#define		BV13			(1 << 13)	/*       8192 */
#define		BV14			(1 << 14)	/*      16384 */
#define		BV15			(1 << 15)	/*      32768 */
#define		BV16			(1 << 16)	/*      65536 */
#define		BV17			(1 << 17)	/*     131072 */
#define		BV18			(1 << 18)	/*     262144 */
#define		BV19			(1 << 19)	/*     524288 */
#define		BV20			(1 << 20)	/*    1048576 */
#define		BV21			(1 << 21)	/*    2097152 */
#define		BV22			(1 << 22)	/*    4194304 */
#define		BV23			(1 << 23)	/*    8388608 */
#define		BV24			(1 << 24)	/*   16777216 */
#define		BV25			(1 << 25)	/*   33554432 */
#define		BV26			(1 << 26)	/*   67108864 */
#define		BV27			(1 << 27)	/*  134217728 */
#define		BV28			(1 << 28)	/*  268435456 */
#define		BV29			(1 << 29)	/*  536870912 */
#define		BV30			(1 << 30)	/* 1073741824 */

/*  BV31 is the sign bit, we'll leave it alone  - erwin@pip.dknet.dk  */



/*
 * Structure types.
 */
typedef struct	who_data		WHO_DATA;
typedef struct	affect_data		AFFECT_DATA;
typedef struct	area_data		AREA_DATA;
typedef struct	ban_data		BAN_DATA;
typedef struct	char_data		CHAR_DATA;
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
typedef struct  mob_prog_data		MPROG_DATA;
typedef struct  mob_prog_act_list	MPROG_ACT_LIST;
typedef struct  act_prog_data		ACT_PROG_DATA;
typedef struct	soc_index_data		SOC_INDEX_DATA;
typedef struct	class_type		CLASS_TYPE;
typedef struct	clan_data		CLAN_DATA;
typedef struct	hunt_hate_fear		HHF_DATA;
typedef struct	system_data		SYSTEM_DATA;
typedef struct	game_data		GAME_DATA;
typedef struct	history_data		HISTORY_DATA;
typedef struct	alias_data		ALIAS_DATA;

/*
 * Function types.
 */
typedef	void DO_FUN			args( ( CHAR_DATA *ch,
					       char *argument ) );
typedef bool MOB_FUN			args( ( CHAR_DATA *ch ) );
typedef bool OBJ_FUN			args( ( OBJ_DATA *obj,
					       CHAR_DATA *keeper ) );
typedef bool ROOM_FUN			args( ( ROOM_INDEX_DATA *room ) );
typedef void SPELL_FUN			args( ( int sn, int level,
					       CHAR_DATA *ch, void *vo ) );
typedef void GAME_FUN			args( ( CHAR_DATA *ch,
					       CHAR_DATA *croupier,
					       int amount, int cheat,
					       char *argument ) );
typedef bool HIT_FUN			args( ( CHAR_DATA *ch,
					       CHAR_DATA *victim, int hit,
					       int dam ) );


/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 1024
#define MAX_STRING_LENGTH	 4096
#define MAX_INPUT_LENGTH	  160

#define	MAX_WORD_HASH		   27	/* The latin alphabet has 26 letters */


/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_CHUNKS                 33			/* Used in ssm.c */

#define EXP_PER_LEVEL		 2000
#define MAX_SPELL		  156
#define MAX_SKILL		  ( MAX_SPELL + 65 )
#define MAX_GROUP		   38
#define MAX_ATTACK                 17
#define MAX_MULTICLASS		    2
#define MAX_POSE		   17
#define MAX_RACE                   41
#define MAX_LEVEL		   64
#define MAX_ALIAS		   10
#define MAX_VECTOR                  8
#define MAX_HISTORY		   10
#define MAX_LANGUAGE               24
#define L_DIR		           MAX_LEVEL
#define L_SEN		          ( L_DIR - 1 )
#define L_JUN	        	  ( L_SEN - 1 )
#define L_APP                     ( L_JUN - 1 )
#define LEVEL_IMMORTAL		  L_APP
#define LEVEL_HERO		  ( LEVEL_IMMORTAL - 1 )
#define L_HER			  LEVEL_HERO

#define PULSE_PER_SECOND	    4
#define PULSE_VIOLENCE		  (    2 * PULSE_PER_SECOND )
#define PULSE_FAST		  (    5 * PULSE_PER_SECOND )
#define PULSE_TICK		  (   30 * PULSE_PER_SECOND )
#define PULSE_AREA		  (   60 * PULSE_PER_SECOND )
#define PULSE_DB_DUMP             ( 1800 * PULSE_PER_SECOND ) /* 30 minutes */



/*
 * Player character key data struct
 * Stuff for new error trapping of corrupt pfiles.
 */
struct  key_data
{
    char	    key[12];	/* Increase if you make a key > 11 chars */
    int		    string;	/* TRUE for string, FALSE for int        */
    unlong	    deflt;	/* Default value or pointer              */
    void *	    ptrs[7];	/* Increase if you have > 6 parms/line   */
};

struct  nkey_data
{
    char	    key[12];	/* Increase if you make a key > 11 chars */
    int		    string;	/* TRUE for string, FALSE for int        */
    unlong	    deflt;	/* Default value or pointer              */
    void *	    ptrs;	/* Increase if you have > 6 parms/line   */
};
#define MAND		3344556	/* Magic # for manditory field           */
#define SPECIFIED	3344557 /* Key was used already.                 */
#define DEFLT		3344558 /* Use default from fread_char_obj       */



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
 * Note that weather changes according to the month (winter).
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3
#define MOON_RISE                   4
#define MOON_SET                    5

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
    int		windspeed;
    int		winddir;
    int		temperature;

};

/*
 * Colour stuff by Lope of Loping Through The MUD
 */
#define MOD_CLEAR	"[0m"		/* Resets Colour	*/
#define MOD_BOLD	"[1m"
#define MOD_FAINT	"[2m"
#define MOD_UNDERLINE	"[4m"
#define MOD_BLINK	"[5m"
#define MOD_REVERSE	"[7m"

#define FG_BLACK	"[30m"		/* Foreground Colours  	*/
#define FG_RED		"[31m"
#define FG_GREEN	"[32m"
#define FG_YELLOW	"[33m"
#define FG_BLUE		"[34m"
#define FG_MAGENTA	"[35m"
#define FG_CYAN		"[36m"
#define FG_WHITE	"[37m"

#define BG_BLACK	"[40m"		/* Background Colours 	*/
#define BG_RED		"[41m"
#define BG_GREEN	"[42m"
#define BG_YELLOW	"[43m"
#define BG_BLUE		"[44m"
#define BG_MAGENTA	"[45m"
#define BG_CYAN		"[46m"
#define BG_WHITE	"[47m"



/*
 * Connected state for a channel.
 */
#define CON_PLAYING			0
#define CON_GET_NAME			1
#define CON_GET_OLD_PASSWORD		2
#define CON_CONFIRM_NEW_NAME		3
#define CON_GET_NEW_PASSWORD		4
#define CON_CONFIRM_NEW_PASSWORD	5
#define CON_GET_COLOUR                  6
#define CON_DISPLAY_RACE                7
#define CON_GET_NEW_RACE                8
#define CON_CONFIRM_NEW_RACE            9
#define CON_GET_NEW_SEX                10
#define CON_DISPLAY_1ST_CLASS          11
#define CON_GET_1ST_CLASS              12
#define CON_CONFIRM_1ST_CLASS          13
#define CON_DEFAULT_CHOICE             14
#define CON_DISPLAY_2ND_CLASS          15
#define CON_GET_2ND_CLASS              16
#define CON_CONFIRM_2ND_CLASS          17
#define CON_SHOW_MOTD                  18
#define CON_READ_MOTD                  19

#define CON_PASSWD_GET_OLD            -10
#define CON_PASSWD_GET_NEW            -11
#define CON_PASSWD_CONFIRM_NEW        -12
#define CON_RETIRE_GET_PASSWORD       -13
#define CON_RETIRE_CONFIRM            -14

#define CON_AEDITOR                    -1
#define CON_REDITOR                    -2
#define CON_MEDITOR                    -3
#define CON_OEDITOR                    -4
#define CON_MPEDITOR                   -5

#define IS_PLAYING( d )		( d->connected <= CON_PLAYING )


/*
 * Race structures
 */
struct  race_type
{
    char *              name;
    int                 race_abilities;
    int                 size;
    int                 str_mod;
    int                 int_mod;
    int                 wis_mod;
    int                 dex_mod;
    int                 con_mod;
    int                 hp_gain;
    int                 mana_gain;
    int                 move_gain;
    int                 thirst_mod;
    int                 hunger_mod;
    char *              dmg_message;
    char *              hate;
    int			parts;
    int			resistant;
    int			immune;
    int			susceptible;
    int			language;
};

/* Race size defines by Maniac */
#define SIZE_ANY		      0
#define SIZE_MINUTE		      1
#define SIZE_SMALL		      2
#define SIZE_PETITE		      3
#define SIZE_AVERAGE		      4
#define SIZE_MEDIUM		      5
#define SIZE_LARGE		      6
#define SIZE_HUGE		      7
#define SIZE_TITANIC		      8
#define SIZE_GARGANTUAN		      9

/* Race ability bits */
#define RACE_NO_ABILITIES	      0
#define RACE_PC_AVAIL		   BV00
#define RACE_WATERBREATH	   BV01
#define RACE_FLY		   BV02
#define RACE_SWIM		   BV03
#define RACE_WATERWALK		   BV04
#define RACE_PASSDOOR		   BV05
#define RACE_INFRAVISION	   BV06
#define RACE_DETECT_ALIGN	   BV07
#define RACE_DETECT_INVIS	   BV08
#define RACE_DETECT_HIDDEN	   BV09
#define RACE_PROTECTION		   BV10
#define RACE_SANCT		   BV11
#define RACE_WEAPON_WIELD	   BV12
#define RACE_MUTE                  BV13

/* Language bits, to be implemented, took most of these off SMAUG */
#define LANG_UNKNOWN                  0  /* Anything else                    */
#define LANG_COMMON		   BV00  /* Human base language              */
#define LANG_ELVEN		   BV01  /* Elven base language              */
#define LANG_DWARVEN		   BV02  /* Dwarven base language            */
#define LANG_PIXIE		   BV03  /* Pixie|Faerie base language       */
#define LANG_OGRE		   BV04  /* Ogre base language               */
#define LANG_ORCISH                BV05  /* Orc base language                */
#define LANG_TROLLESE              BV06  /* Troll base language              */
#define LANG_RODENT		   BV07  /* Small mammals                    */
#define LANG_INSECTOID		   BV08  /* Insects                          */
#define LANG_MAMMAL		   BV09  /* Larger mammals                   */
#define LANG_REPTILE		   BV10  /* Small reptiles                   */
#define LANG_DRAGON		   BV11  /* Large reptiles, Dragons          */
#define LANG_SPIRITUAL		   BV12  /* Necromancers or undeads|spectres */
#define LANG_MAGICAL		   BV13  /* Spells maybe?  Magical creatures */
#define LANG_GOBLIN		   BV14  /* Goblin base language             */
#define LANG_GOD		   BV15  /* Clerics possibly?  God creatures */
#define LANG_HALFLING		   BV16  /* Halfling base language           */
#define LANG_GITH		   BV17  /* Gith Language                    */
#define LANG_DROW		   BV18  /* Drow base language               */
#define LANG_KOBOLD		   BV19  /* Kobold base language             */
#define LANG_GNOMISH		   BV20  /* Gnome base language              */
#define LANG_PLANT		   BV21  /* Plant language                   */
#define LANG_FISH		   BV22  /* Fish language                    */
#define LANG_ANIMAL		   BV23  /* Animal language                  */
#define LANG_CLAN		   BV24  /* Clan language                    */


/*
 * Drunkeness communication structure.
 */
struct  struckdrunk
{
    int                 min_drunk_level;
    int                 number_of_rep;
    char               *replacement[11];
};


/*
 * History data structure.
 */
struct history_data
{
    HISTORY_DATA *	next;
    HISTORY_DATA *	prev;
    char *		comm;
};


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
    char *		user;
    unsigned int        descriptor;
    int		        connected;
    bool		fcommand;
    char		inbuf		[ MAX_INPUT_LENGTH*4 ];
    char		incomm		[ MAX_INPUT_LENGTH   ];
    HISTORY_DATA *	infirst;
    HISTORY_DATA *	inlast;
    int			histsize;
    int			repeat;
    char *              showstr_head;
    char *              showstr_point;
    char *		outbuf;
    int			outsize;
    int			outtop;
    void *              olc_editing;
    char **             str_editing;
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
    AREA_DATA *		area;
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



struct	game_data
{
    GAME_DATA *	next;			/* Next game in list		*/
    int 	croupier;		/* Vnum of croupier mob		*/
    GAME_FUN *	game_fun;		/* Game fun run by mob		*/
    int		bankroll;		/* Amount of gold in the bank	*/
    int		max_wait;		/* Number of pulse to decision	*/
    bool	cheat;			/* True if mob cheats		*/
};


#define CLASS_MAGE          0
#define CLASS_CLERIC        1
#define CLASS_THIEF         2
#define CLASS_WARRIOR       3
#define CLASS_PSIONICIST    4

/*
 * Per-class stuff.
 */
struct	class_type
{
    CLASS_TYPE *next;			    /* Next class in list	   */
    char *	name;			    /* Full class name		   */
    char *	who_name;		    /* Three-letter name for 'who' */
    int 	attr_prime;		    /* Prime attribute		   */
    int 	weapon;			    /* First weapon		   */
    int 	guild;			    /* Vnum of guild room	   */
    int 	max_adept;		    /* Maximum skill level	   */
    int 	thac0_00;		    /* Thac0 for level  0	   */
    int 	thac0_47;		    /* Thac0 for level 47	   */
    int  	hp_min;			    /* Min hp gained on leveling   */
    int	        hp_max;			    /* Max hp gained on leveling   */
    bool	fMana;			    /* Class gains mana on level   */
    int 	skill_level  [ MAX_SKILL ]; /* Level needed by class	   */
    int 	skill_adept  [ MAX_SKILL ]; /* Max % attainable by class   */
    int 	skill_rating [ MAX_SKILL ]; /* Difficulty level by class   */

    char *	title  [ MAX_LEVEL+1 ][2];  /* The title table             */
    char *	pose   [ MAX_POSE    ][2];  /* The pose table              */
};


#define CLAN_PLAIN                    0
#define CLAN_NOKILL                   1
#define CLAN_ORDER                    2
#define CLAN_GUILD                    3

#define	RANK_EXILED		      1		/* Exiled from a clan   */
#define	RANK_CLANSMAN		      2		/* Foot soldier         */
#define	RANK_CLANHERO		      3		/* Knight               */
#define	RANK_SUBCHIEF		      4		/* Knight Lord          */
#define	RANK_CHIEFTAIN		      5		/* Second in command    */
#define	RANK_OVERLORD		      6		/* Head clan leader     */

struct  clan_data
{
    CLAN_DATA *	next;           /* next clan in list                    */
    char *	filename;       /* Clan filename                        */
    char *	who_name;       /* Clan who name                        */
    char *	name;           /* Clan name                            */
    char *	motto;          /* Clan motto                           */
    char *	description;    /* A brief description of the clan      */
    char *	overlord;       /* Head clan leader                     */
    char *	chieftain;      /* Second in command                    */
    int		pkills;         /* Number of pkills on behalf of clan   */
    int		pdeaths;        /* Number of pkills against clan        */
    int		mkills;         /* Number of mkills on behalf of clan   */
    int		mdeaths;        /* Number of clan deaths due to mobs    */
    int		illegal_pk;     /* Number of illegal pk's by clan       */
    int		score;          /* Overall score                        */
    int		clan_type;      /* See clan type defines                */
    int		subchiefs;      /* Number of subchiefs                  */
    int		clanheros;      /* Number of clanheros                  */
    int		members;        /* Number of clan members               */
    int		clanobj1;       /* Vnum of first clan obj (ring)        */
    int		clanobj2;       /* Vnum of second clan obj (shield)     */
    int		clanobj3;       /* Vnum of third clan obj (weapon)      */
    int		recall;         /* Vnum of clan's recall room           */
    int		donation;       /* Vnum of clan's donation pit          */
    CLASS_TYPE *class;          /* For guilds                           */
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
 * For do_who output.
 */
struct	who_data
{
    WHO_DATA *	prev;
    WHO_DATA *	next;
    char *	text;
};



/*
 * For alias output.
 */
struct	alias_data
{
    ALIAS_DATA * next;
    char *	 cmd;
    char *	 subst;
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
    u_intc		bitvector [ MAX_VECTOR ];
    int			level;
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



/*
 * Resistant Immune Susceptible flags.
 */
#define RIS_NONE		      0
#define RIS_FIRE		   BV00
#define RIS_COLD		   BV01
#define RIS_ELECTRICITY		   BV02
#define RIS_ENERGY		   BV03
#define RIS_ACID		   BV04
#define RIS_POISON		   BV05
#define RIS_CHARM		   BV06
#define RIS_MENTAL		   BV07
#define RIS_WHITE_MANA		   BV08
#define RIS_BLACK_MANA		   BV09
#define RIS_DISEASE		   BV10
#define RIS_DROWNING		   BV11
#define RIS_LIGHT		   BV12
#define RIS_SOUND		   BV13
#define RIS_MAGIC		   BV14
#define RIS_NONMAGIC		   BV15
#define RIS_SILVER		   BV16
#define RIS_IRON		   BV17
#define RIS_WOOD		   BV18
#define RIS_WEAPON		   BV19
#define RIS_BASH		   BV20
#define RIS_PIERCE		   BV21
#define RIS_SLASH		   BV22



/*
 * Damage types.
 */
#define DAM_NONE                      0		/*  weapon damage types */
#define DAM_BASH		      1
#define DAM_PIERCE		      2
#define DAM_SLASH		      3

#define DAM_OTHER		   1000		/* magical damage types */
#define DAM_FIRE		   1001
#define DAM_COLD		   1002
#define DAM_ELECTRICITY		   1003
#define DAM_ACID		   1004
#define DAM_POISON		   1005
#define DAM_CHARM		   1006
#define DAM_MENTAL                 1007
#define DAM_ENERGY                 1008
#define DAM_WHITE_MANA		   1009
#define DAM_BLACK_MANA		   1010
#define DAM_DISEASE                1011
#define DAM_DROWNING               1012
#define DAM_LIGHT                  1013
#define DAM_SOUND		   1014
#define DAM_HARM		   1015


/*
 * Return values for check_ris().
 */
#define IS_NORMAL               0
#define IS_RESISTANT            1
#define IS_IMMUNE               2
#define IS_SUSCEPTIBLE          3



/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000

struct attack_type
{
     char *	name;
     int *	wpn_gsn;
     int	dam_type;
     HIT_FUN *	hit_fun;
};



/*
 * Mana types.
 */
#define MANA_ANY                     -1
#define MANA_NONE                     0
#define MANA_EARTH                 BV00
#define MANA_AIR                   BV01
#define MANA_FIRE                  BV02
#define MANA_WATER                 BV03



/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4



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
#define MOB_VNUM_SECRETARY         3142
#define MOB_VNUM_MIDGAARD_MAYOR    3143

#define MOB_VNUM_AIR_ELEMENTAL     8914
#define MOB_VNUM_EARTH_ELEMENTAL   8915
#define MOB_VNUM_WATER_ELEMENTAL   8916
#define MOB_VNUM_FIRE_ELEMENTAL    8917
#define MOB_VNUM_DUST_ELEMENTAL    8918

#define MOB_VNUM_CLONE		   3060


/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		   BV00		/* Auto set for mobs	*/
#define ACT_SENTINEL		   BV01		/* Stays in one room	*/
#define ACT_SCAVENGER		   BV02		/* Picks up objects	*/
#define ACT_AGGRESSIVE		   BV05		/* Attacks PC's		*/
#define ACT_STAY_AREA		   BV06		/* Won't leave area	*/
#define ACT_WIMPY		   BV07		/* Flees when hurt	*/
#define ACT_PET			   BV08		/* Auto set for pets	*/
#define ACT_TRAIN		   BV09		/* Can train PC's	*/
#define ACT_PRACTICE		   BV10		/* Can practice PC's	*/
#define ACT_MOBINVIS		   BV13         /* Player invis mobs    */
#define ACT_MOVED                  BV17         /* Dont ever set!       */
#define ACT_MOUNTABLE              BV29         /* Can be mounted       */

/*
 * Body parts.
 */
#define PART_NONE		      0
#define PART_HEAD		   BV00
#define PART_ARMS		   BV01
#define PART_LEGS		   BV02
#define PART_HEART		   BV03
#define PART_BRAINS		   BV04
#define PART_GUTS		   BV05
#define PART_HANDS		   BV06
#define PART_FEET		   BV07
#define PART_FINGERS		   BV08
#define PART_EAR		   BV09
#define PART_EYE		   BV10
#define PART_LONG_TONGUE	   BV11
#define PART_EYESTALKS		   BV12
#define PART_TENTACLES		   BV13
#define PART_FINS		   BV14
#define PART_WINGS		   BV15
#define PART_TAIL		   BV16

/*
 * Body parts used in combat.
 */
#define PART_CLAWS		   BV17
#define PART_FANGS		   BV18
#define PART_HORNS		   BV19
#define PART_SCALES		   BV20
#define PART_TUSKS		   BV21

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		      0
#define AFF_INVISIBLE		      1
#define AFF_DETECT_EVIL		      2
#define AFF_DETECT_INVIS	      3
#define AFF_DETECT_MAGIC	      4
#define AFF_DETECT_HIDDEN	      5
#define AFF_HOLD		      6
#define AFF_SANCTUARY		      7
#define AFF_FAERIE_FIRE		      8
#define AFF_INFRARED		      9
#define AFF_CURSE		     10
#define AFF_CHANGE_SEX		     11
#define AFF_POISON		     12
#define AFF_PROTECT_EVIL	     13
#define AFF_POLYMORPH		     14
#define AFF_SNEAK		     15
#define AFF_HIDE		     16
#define AFF_SLEEP		     17
#define AFF_CHARM		     18
#define AFF_FLYING		     19
#define AFF_PASS_DOOR		     20
#define AFF_WATERWALK                21
#define AFF_SUMMONED                 22
#define AFF_MUTE                     23
#define AFF_GILLS                    24
#define AFF_VAMP_BITE                25
#define AFF_GHOUL                    26
#define AFF_FLAMING                  27
#define AFF_DETECT_GOOD              28
#define AFF_PROTECT_GOOD             29
#define AFF_PLAGUE		     30
/*
 * no more bits left... now i need a DEC Alpha with 64bit integers :-)
 * or to add a new field, affect_by2...					Zen
 */

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
#define OBJ_VNUM_GUTS		     17
#define OBJ_VNUM_BRAINS		     18

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22

#define OBJ_VNUM_DONATION_PIT	   3010
#define OBJ_VNUM_SCHOOL_MACE	   3700
#define OBJ_VNUM_SCHOOL_DAGGER	   3701
#define OBJ_VNUM_SCHOOL_SWORD	   3702
#define OBJ_VNUM_SCHOOL_VEST	   3703
#define OBJ_VNUM_SCHOOL_SHIELD	   3704
#define OBJ_VNUM_SCHOOL_BANNER     3716
#define OBJ_VNUM_SCHOOL_CLUB       3717

#define OBJ_VNUM_BLACK_POWDER      8903
#define OBJ_VNUM_FLAMEBLADE        8920
#define OBJ_VNUM_STAKE             3811
#define OBJ_VNUM_LICENSE           3011
#define OBJ_VNUM_CLAN_CARD         3381
#define OBJ_VNUM_PORTAL              23

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
#define ITEM_PORTAL		     28
#define ITEM_WARP_STONE		     29
#define ITEM_CLOTHING		     30
#define ITEM_RANGED_WEAPON           31
#define ITEM_AMMO                    32
#define ITEM_GEM                     33


/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		   BV00
#define ITEM_HUM		   BV01
#define ITEM_DARK		   BV02
#define ITEM_LOCK		   BV03
#define ITEM_EVIL		   BV04
#define ITEM_INVIS		   BV05
#define ITEM_MAGIC		   BV06
#define ITEM_NODROP		   BV07
#define ITEM_BLESS		   BV08
#define ITEM_ANTI_GOOD		   BV09
#define ITEM_ANTI_EVIL		   BV10
#define ITEM_ANTI_NEUTRAL	   BV11
#define ITEM_NOREMOVE		   BV12
#define ITEM_INVENTORY		   BV13
#define ITEM_POISONED              BV14
#define ITEM_VAMPIRE_BANE          BV15
#define ITEM_HOLY                  BV16
#define ITEM_VIS_DEATH             BV17


/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		   BV00
#define ITEM_WEAR_FINGER	   BV01
#define ITEM_WEAR_NECK		   BV02
#define ITEM_WEAR_BODY		   BV03
#define ITEM_WEAR_HEAD		   BV04
#define ITEM_WEAR_LEGS		   BV05
#define ITEM_WEAR_FEET		   BV06
#define ITEM_WEAR_HANDS		   BV07
#define ITEM_WEAR_ARMS		   BV08
#define ITEM_WEAR_SHIELD	   BV09
#define ITEM_WEAR_ABOUT		   BV10
#define ITEM_WEAR_WAIST		   BV11
#define ITEM_WEAR_WRIST		   BV12
#define ITEM_WIELD		   BV13
#define ITEM_HOLD		   BV14
#define ITEM_MISSILE_WIELD	   BV15



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
#define APPLY_RACE                   25
#define APPLY_RESISTANT		     26
#define APPLY_IMMUNE		     27
#define APPLY_SUSCEPTIBLE	     28


/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		   BV00
#define CONT_PICKPROOF		   BV01
#define CONT_CLOSED		   BV02
#define CONT_LOCKED		   BV03

/*
 * Values for portals (value[1]).
 * Used in #OBJECTS.
 */
#define PORTAL_CLOSEABLE	   BV00
#define PORTAL_PICKPROOF	   BV01
#define PORTAL_CLOSED		   BV02
#define PORTAL_LOCKED		   BV03

/*
 * Values for portals (value[3]).
 * Used in #OBJECTS.
 */
#define PORTAL_NO_CURSED	   BV00
#define PORTAL_GO_WITH		   BV01
#define PORTAL_RANDOM		   BV02
#define PORTAL_BUGGY		   BV03

/*
 * Values for ranged weapons (value[3]).
 * Used in #OBJECTS.
 */
#define RNG_BOW			      0
#define RNG_CROSSBOW		      1
#define RNG_CATAPULT		      2


/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_CHAT		   1200
#define ROOM_VNUM_TEMPLE	   3001
#define ROOM_VNUM_ALTAR		   3054
#define ROOM_VNUM_SCHOOL	   3700
#define ROOM_VNUM_GRAVEYARD_A       427
#define ROOM_VNUM_PURGATORY_A       401


/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		   BV00
#define ROOM_NO_MOB		   BV02
#define ROOM_INDOORS		   BV03
#define ROOM_UNDERGROUND           BV04
#define ROOM_PRIVATE		   BV09
#define ROOM_SAFE		   BV10
#define ROOM_SOLITARY		   BV11
#define ROOM_PET_SHOP		   BV12
#define ROOM_NO_RECALL		   BV13
#define ROOM_CONE_OF_SILENCE       BV14
#define ROOM_ARENA		   BV15

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



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		   BV00
#define EX_CLOSED		   BV01
#define EX_LOCKED		   BV02
#define EX_BASHED                  BV03
#define EX_BASHPROOF               BV04
#define EX_PICKPROOF		   BV05
#define EX_PASSPROOF               BV06
#define EX_EAT_KEY                 BV07

/* to be implemented */
#define EX_JAMMED                  BV08
#define EX_SECRET                  BV09


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
#define SECT_DUNNO                   11
#define SECT_ICELAND                 12
#define SECT_MAX		     13



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
#define WEAR_LEGS		      7
#define WEAR_FEET		      8
#define WEAR_HANDS		      9
#define WEAR_ARMS		     10
#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_WIELD		     16
#define WEAR_HOLD		     17
#define WEAR_WIELD_2                 18		/* by Thelonius */
#define WEAR_MISSILE_WIELD           19
#define MAX_WEAR		     20



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
 * MUD ACT bits for sysdata.
 */
#define MUD_AUTOSAVE_DB		   BV00
#define MUD_NONEWS    		   BV02
#define MUD_VERBOSE_LOGS	   BV03
#define MUD_AUTOPARDON		   BV04



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		   BV00		/* Don't EVER set.	 */
#define PLR_BOUGHT_PET		   BV01
#define PLR_REGISTER               BV02         /* Registering for Pkill */
#define PLR_AUTOEXIT		   BV03
#define PLR_AUTOLOOT		   BV04
#define PLR_AUTOSAC		   BV05
#define PLR_BLANK		   BV06
#define PLR_BRIEF		   BV07
#define PLR_COMBINE		   BV09
#define PLR_PROMPT		   BV10
#define PLR_TELNET_GA		   BV11

#define PLR_HOLYLIGHT		   BV12
#define PLR_WIZINVIS		   BV13
#define PLR_WIZBIT		   BV14
#define	PLR_SILENCE		   BV15
#define PLR_NO_EMOTE		   BV16
#define PLR_MOVED		   BV17
#define PLR_NO_TELL		   BV18
#define PLR_LOG			   BV19
#define PLR_DENY		   BV20
#define PLR_FREEZE		   BV21
#define PLR_THIEF		   BV22
#define PLR_KILLER		   BV23
#define PLR_AUTOGOLD		   BV24
#define PLR_AFK			   BV25
#define PLR_COLOUR		   BV26
#define PLR_EDIT_INFO		   BV27
#define PLR_PAGER		   BV28

/* WIZNET flags */
#define WIZ_ON			   BV00		/* On|Off switch	*/
#define WIZ_TICKS		   BV01		/* Tick's.		*/
#define WIZ_LOGINS		   BV02   	/* Login|Logoff. 	*/
#define WIZ_DEATHS		   BV03   	/* Plr death's. 	*/
#define WIZ_RESETS		   BV04   	/* Area reset's. 	*/
#define WIZ_FLAGS		   BV05   	/* KILLER & THIEF flag. */
#define WIZ_SACCING		   BV06   	/* Item saccing. 	*/
#define WIZ_LEVELS		   BV07   	/* Level's advancing. 	*/
#define WIZ_SECURE		   BV08   	/* log -> screen. 	*/
#define WIZ_SWITCHES		   BV09  	/* Switch. 		*/
#define WIZ_SNOOPS		   BV10  	/* Snoops. 		*/
#define WIZ_RESTORE		   BV11  	/* Restores. 		*/
#define WIZ_LOAD		   BV12  	/* oload, mload. 	*/
#define WIZ_NEWBIE		   BV13  	/* Newbie's. 		*/
#define WIZ_SPAM		   BV14		/* Spamming.	 	*/
#define WIZ_CHAT		   BV15		/* Wiznet chatting. 	*/
#define WIZ_DEBUG		   BV16		/* Debug info. 		*/

/*
 * Obsolete bits.
 */
#if 0
#define PLR_CHAT		   BV08		/* Obsolete		*/
#define ACT_GAMBLE                 BV11         /* Runs a gambling game	*/
#endif



/*
 * Channel bits.
 */
#define	CHANNEL_AUCTION		   BV00
#define	CHANNEL_CHAT		   BV01

#define	CHANNEL_IMMTALK		   BV03
#define	CHANNEL_MUSIC		   BV04
#define	CHANNEL_QUESTION	   BV05
#define	CHANNEL_SHOUT		   BV06
#define	CHANNEL_YELL		   BV07
#define	CHANNEL_GRATS		   BV08
#define	CHANNEL_CLANTALK	   BV09
#define	CHANNEL_NEWS    	   BV10



/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
    MOB_INDEX_DATA *	next;
    MOB_FUN *		spec_fun;
    GAME_DATA *		pGame;
    SHOP_DATA *		pShop;
    AREA_DATA *         area;
    MPROG_DATA *        mobprogs;
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
    u_intc		affected_by [ MAX_VECTOR ];
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
    int			race;
    int			progtypes;
    int			resistant;
    int			immune;
    int			susceptible;
    int			speaks;
    int			speaking;
};



struct hunt_hate_fear
{
    HHF_DATA *		next;
    char *		name;
    CHAR_DATA *		who;
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
    CHAR_DATA *		riding;
    CHAR_DATA *		rider;
    MOB_FUN *		spec_fun;
    MOB_INDEX_DATA *	pIndexData;
    DESCRIPTOR_DATA *	desc;
    AFFECT_DATA *	affected;
    NOTE_DATA *		pnote;
    OBJ_DATA *		carrying;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	was_in_room;
    PC_DATA *		pcdata;
    char *		name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    int 		sex;
    CLASS_TYPE *	class		[ MAX_MULTICLASS ];
    int 		race;
    int 		level;
    int  		trust;
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
    int			gold;
    int			exp;
    int			act;
    u_intc		affected_by	[ MAX_VECTOR ];
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
    bool		deleted;
    MPROG_ACT_LIST *	mpact;
    int			mpactnum;
    HHF_DATA *		hunting;
    HHF_DATA *		hating;
    HHF_DATA *		fearing;
    int			resistant;
    int			immune;
    int			susceptible;
    int			speaks;
    int			speaking;
};


/*
 * Data which only PC's have.
 */
struct	pc_data
{
    PC_DATA *		next;
    char *		pwd;
    char *		pwdnew;
    char *		bamfin;
    char *		bamfout;
    char *              immskll;
    char *		title;
    char *              prompt;
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
    int			perm_hit;
    int			perm_mana;
    int			perm_move;
    int 		condition	[ 3 ];
    int                 pagelen;
    int 		learned		[ MAX_SKILL ];
    bool                switched;
    ALIAS_DATA *	alias_list;
    int                 security;
    int                 mprog_edit;
    CLAN_DATA *		clan;
    int			rank;
    int			pkills;
    int			pdeaths;
    int			mkills;
    int			mdeaths;
    int			illegal_pk;
    int			wiznet;
    int			points;
};

/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_MAX		27

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
    OBJ_FUN *		spec_fun;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
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
    int 		level;
    int			value	[ 5 ];
    AREA_DATA *         area;			/* OLC */
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
    OBJ_FUN *		spec_fun;
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
    int			value	[ 5 ];
    bool                deleted;
};



/*
 * Exit data.
 */
struct	exit_data
{
    ROOM_INDEX_DATA *	to_room;
    EXIT_DATA *         next;
    int 		vnum;
    int 		exit_info;
    int 		key;
    char *		keyword;
    char *		description;
    int                 rs_flags;
    int                 orig_door;
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
    RESET_DATA *	prev;
    ROOM_INDEX_DATA *	room;
    char		command;
    int 		rs_vnum;
    int 		loc;
    int 		percent;
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
    int                 recall;
    int 		age;
    int 		nplayer;
    char *              filename;		/* OLC                     */
    char *              builders;		/* OLC Listing of builders */
    int                 security;		/* OLC Value 0-infinity    */
    int                 lvnum;			/* OLC Lower vnum          */
    int                 uvnum;			/* OLC Upper vnum          */
    int                 vnum;			/* OLC Area vnum           */
    int                 area_flags;		/* OLC                     */
    int                 low_r_vnum;
    int                 hi_r_vnum;
    int                 low_o_vnum;
    int                 hi_o_vnum;
    int                 low_m_vnum;
    int                 hi_m_vnum;
    int                 llv;
    int                 ulv;
    char *              author;
    char *              resetmsg;
    char *		note;
};



/*
 * System information and flags.
 */
struct	system_data
{
    int			max_players;		/* Maximum players this boot */
    int			all_time_max;		/* Maximum players ever      */
    char *		time_of_max;		/* Time of max players ever  */
    int			act;			/* Mud act flags             */
};


/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    RESET_DATA *        reset_first;
    RESET_DATA *        reset_last;
    ROOM_FUN *		spec_fun;
    CHAR_DATA *		people;
    OBJ_DATA *		contents;
    EXTRA_DESCR_DATA *	extra_descr;
    AREA_DATA *		area;
    EXIT_DATA *		exit	[ 6 ];
    char *		name;
    char *		description;
    int 		vnum;
    int 		room_flags;
    int 		orig_room_flags;
    int 		light;
    int 		sector_type;
    int			heal_rate;
    int			mana_rate;
};


/*
 * Skills include spells as a particular case.
 */
struct	skill_type
{
    char *	name;			    /* Name of skill		  */
    SPELL_FUN *	spell_fun;		    /* Spell pointer (for spells) */
    int 	target;			    /* Legal targets		  */
    int 	minimum_position;	    /* Position for caster / user */
    int *	pgsn;			    /* Pointer to associated gsn  */
    int 	min_mana;		    /* Minimum mana used	  */
    int 	beats;			    /* Waiting time after use	  */
    char *	noun_damage;		    /* Damage message		  */
    char *	msg_off;		    /* Wear off message		  */
    int 	mana_type;		    /* Mana type (for spells)	  */
    char *	teachers;
};


struct lang_type
{
     char *		name;
     int		value;
};



/* Mob program structures */
struct	act_prog_data
{
    ACT_PROG_DATA *  next;
    void *           vo;
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
    MPROG_DATA *    next;
    int             type;
    char *          arglist;
    char *          comlist;
};

bool    MOBtrigger;

#define ERROR_PROG		     -1
#define IN_FILE_PROG		      0
#define ACT_PROG		   BV00
#define SPEECH_PROG		   BV01
#define RAND_PROG		   BV02
#define FIGHT_PROG		   BV03
#define DEATH_PROG		   BV04
#define HITPRCNT_PROG		   BV05
#define ENTRY_PROG		   BV06
#define GREET_PROG		   BV07
#define ALL_GREET_PROG		   BV08
#define GIVE_PROG		   BV09
#define BRIBE_PROG		   BV10



/*
 * These are skill_lookup return values for common skills and spells.
 */
extern  int	gsn_backstab;
extern  int     gsn_berserk;			/* by Thelonius */
extern  int     gsn_circle;			/* by Thelonius */
extern  int     gsn_breathe_water;		/* by Thelonius */
extern  int     gsn_burning_hands;
extern  int	gsn_disarm;
extern  int	gsn_dodge;
extern  int	gsn_hide;
extern  int	gsn_peek;
extern  int	gsn_pick_lock;
extern  int     gsn_poison_weapon;		/* by Thelonius */
extern  int	gsn_scrolls;			/* by Binky / Thelonius */
extern  int	gsn_snare;			/* by Binky / Thelonius */
extern  int	gsn_sneak;
extern  int	gsn_staves;			/* by Binky / Thelonius */
extern  int	gsn_steal;
extern  int	gsn_untangle;			/* by Thelonius */
extern  int	gsn_wands;			/* by Binky / Thelonius */

extern  int     gsn_bash;
extern  int     gsn_dual;			/* by Thelonius */
extern	int	gsn_enhanced_damage;
extern	int	gsn_kick;
extern	int	gsn_punch;			/* by Zen */
extern	int	gsn_parry;
extern	int	gsn_rescue;
extern	int	gsn_second_attack;
extern	int	gsn_third_attack;

extern	int	gsn_blindness;
extern	int	gsn_charm_person;
extern	int	gsn_curse;
extern	int	gsn_invis;
extern	int	gsn_mass_invis;
extern  int     gsn_mute;			/* by Thelonius */
extern	int	gsn_poison;
extern	int	gsn_sleep;
extern	int	gsn_turn_undead;


/*
 * Psionicist gsn's (by Thelonius).
 */
extern  int     gsn_chameleon;
extern  int     gsn_domination;
extern  int     gsn_heighten;
extern  int     gsn_shadow;


extern  int     gsn_stake;

/*
 * New gsn's (by Zen & other ppl).
 */
extern  int     gsn_scan;
extern  int     gsn_shield_block;
extern  int     gsn_fast_healing;
extern  int     gsn_fourth_attack;
extern  int     gsn_fifth_attack;
extern	int	gsn_brew;
extern	int	gsn_scribe;
extern	int	gsn_dirt;
extern  int     gsn_meditate;			/* by Zen */
extern  int     gsn_swim;			/* by Zen */
extern  int     gsn_mass_vortex_lift;		/* by Zen */
extern	int	gsn_track;
extern	int	gsn_whirlwind;
extern	int	gsn_plague;
extern	int	gsn_mount;

extern	int	gsn_hit;
extern	int	gsn_slash;
extern	int	gsn_pierce;
extern	int	gsn_whip;
extern	int	gsn_claw;
extern	int	gsn_explode;
extern	int	gsn_pound;
extern	int	gsn_suction;
extern	int	gsn_shot;

extern	int	gsn_flame_shield;
extern	int	gsn_frost_shield;
extern	int	gsn_shock_shield;

/*
 * Race gsn's (by Kahn).
 */
extern  int     gsn_vampiric_bite;

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

#define BMASK( bit )		( 1 << ( ( bit ) % CHAR_BIT ) )
#define BSLOT( bit )		(        ( bit ) / CHAR_BIT )

#define is_set( var, bit )	( ( var )[BSLOT( bit )] &   BMASK( bit ) )
#define set_bit( var, bit )	( ( var )[BSLOT( bit )] |=  BMASK( bit ) )
#define remove_bit( var, bit )	( ( var )[BSLOT( bit )] &= ~BMASK( bit ) )
#define toggle_bit( var, bit )	( ( var )[BSLOT( bit )] ^=  BMASK( bit ) )


/*
 * Character macros.
 */
#define IS_NPC( ch )		( IS_SET( ( ch )->act, ACT_IS_NPC ) )
#define IS_IMMORTAL( ch )	( get_trust( ch ) >= LEVEL_IMMORTAL )
#define IS_HERO( ch )		( get_trust( ch ) >= LEVEL_HERO     )
#define IS_AFFECTED( ch, sn )	( is_set( ( ch )->affected_by, ( sn ) ) )

#define IS_GOOD( ch )		( ch->alignment >=  350 )
#define IS_EVIL( ch )		( ch->alignment <= -350 )
#define IS_NEUTRAL( ch )	( !IS_GOOD( ch ) && !IS_EVIL( ch ) )

#define IS_AWAKE( ch )		( ch->position > POS_SLEEPING )
#define GET_AC( ch )		( ( ch )->armor				     \
				    + ( IS_AWAKE( ch )			     \
				    ? dex_app[get_curr_dex( ch )].defensive  \
				    : 0 ) )

#define IS_OUTSIDE( ch )       	( !IS_SET(				     \
				    ( ch )->in_room->room_flags,       	     \
				    ROOM_INDOORS ) )

#define WAIT_STATE( ch, pulse ) ( ( ch )->wait = UMAX( ( ch )->wait,         \
						      ( pulse ) ) )

#define MANA_COST( ch, sn )     ( IS_NPC( ch ) ? 0 : UMAX (                  \
				skill_table[sn].min_mana,                    \
				100 / ( 2 + UMAX ( 0, ch->level -            \
				ch->class[0]->skill_level[sn] ) ) ) )

#define IS_SWITCHED( ch )       ( ch->pcdata->switched )

#define	WCHAN( ch, chan )	( IS_SET( ch->pcdata->wiznet, chan ) )

#define CHECK_RES( ch, bit )	( IS_SET( ch->resistant                      \
				    | race_table[ch->race].resistant, bit ) )
#define CHECK_IMM( ch, bit )	( IS_SET( ch->immune                         \
				    | race_table[ch->race].immune, bit ) )
#define CHECK_SUS( ch, bit )	( IS_SET( ch->susceptible                    \
				    | race_table[ch->race].susceptible, bit ) )


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
    bool		show;
};



/*
 * Structure for a social in the socials table.
 */
struct	soc_index_data
{
    SOC_INDEX_DATA *	next;
    char *		name;
    char *		char_no_arg;
    char *		others_no_arg;
    char *		char_found;
    char *		others_found;
    char *		vict_found;
    char *		char_auto;
    char *		others_auto;
    bool		deleted;
};



/*
 * Global constants.
 */
extern	const	struct	str_app_type	str_app		[ 26 ];
extern	const	struct	int_app_type	int_app		[ 26 ];
extern	const	struct	wis_app_type	wis_app		[ 26 ];
extern	const	struct	dex_app_type	dex_app		[ 26 ];
extern	const	struct	con_app_type	con_app		[ 26 ];

extern	const	struct	cmd_type	cmd_table	[ ];
extern	const	struct	liq_type	liq_table	[ LIQ_MAX      ];
extern		struct	skill_type	skill_table	[ MAX_SKILL    ];
extern	const	struct	lang_type	lang_table	[ MAX_LANGUAGE ];

extern  const   struct  race_type       race_table      [ MAX_RACE     ];
extern  const   struct  attack_type     attack_table    [ ];
extern  const   struct  struckdrunk     drunk           [ ];



/*
 * Global variables.
 */
extern		HELP_DATA	  *	help_first;
extern		SHOP_DATA	  *	shop_first;
extern		GAME_DATA	  *	game_first;
extern		CLAN_DATA	  *	clan_first;
extern		CLASS_TYPE	  *	class_first;

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
extern          time_t                  down_time;
extern          time_t                  warning1;
extern          time_t                  warning2;
extern          bool                    Reboot;
extern		SYSTEM_DATA		sysdata;
extern		int			num_descriptors;
extern		char			strAsshole	[ ];
extern		char *			daPrompt;

/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN(	do_afk  	);
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN(	do_answer	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN(	do_auction	);
DECLARE_DO_FUN( do_auto         );
DECLARE_DO_FUN( do_autoexit     );
DECLARE_DO_FUN( do_autoloot     );
DECLARE_DO_FUN( do_autogold     );
DECLARE_DO_FUN( do_autosac      );
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN(	do_ban		);
DECLARE_DO_FUN(	do_bash 	);
DECLARE_DO_FUN(	do_beep 	);
DECLARE_DO_FUN(	do_berserk 	);		/* by Thelonius */
DECLARE_DO_FUN(	do_bet  	);		/* by Thelonius */
DECLARE_DO_FUN( do_blank        );
DECLARE_DO_FUN(	do_brandish	);
DECLARE_DO_FUN( do_brief        );
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN( do_chameleon    );		/* by Thelonius */
DECLARE_DO_FUN(	do_channels	);
DECLARE_DO_FUN(	do_chat		);
DECLARE_DO_FUN(	do_circle 	);		/* by Thelonius */
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN( do_colour       );		/* by Lope */
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
DECLARE_DO_FUN(	do_donate	);
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
DECLARE_DO_FUN(	do_fee		);
DECLARE_DO_FUN(	do_feed		);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN( do_heighten     );		/* by Thelonius */
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN(	do_imtlset	);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN(	do_mload	);
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
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
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
DECLARE_DO_FUN(	do_poison_weapon);		/* by Thelonius */
DECLARE_DO_FUN(	do_pose		);
DECLARE_DO_FUN(	do_practice	);
DECLARE_DO_FUN( do_prompt       );
DECLARE_DO_FUN(	do_punch	);		/* by Zen */
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
DECLARE_DO_FUN(	do_register	);
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
DECLARE_DO_FUN( do_shadow       );		/* by Thelonius */
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN(	do_silence	);
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN( do_slist        );
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN(	do_snare	);		/* by Binky / Thelonius */
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN(	do_socials	);
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN( do_spells       );
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN(	do_sstime	);
DECLARE_DO_FUN(	do_stake	);
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
DECLARE_DO_FUN( do_untangle     );		/* by Thelonius */
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN(	do_users	);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN(	do_wear		);
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

DECLARE_DO_FUN( do_mpedit       );

DECLARE_DO_FUN(	do_scan		);
DECLARE_DO_FUN(	do_brew		);
DECLARE_DO_FUN(	do_scribe	);
DECLARE_DO_FUN(	do_delet	);
DECLARE_DO_FUN(	do_delete	);
DECLARE_DO_FUN(	do_enter	);
DECLARE_DO_FUN(	do_retir	);
DECLARE_DO_FUN(	do_retire	);
DECLARE_DO_FUN(	do_alia		);
DECLARE_DO_FUN(	do_alias	);
DECLARE_DO_FUN(	do_unalia	);
DECLARE_DO_FUN(	do_unalias	);
DECLARE_DO_FUN(	do_dirt		);
DECLARE_DO_FUN(	do_grats	);
DECLARE_DO_FUN(	do_wartalk	);
DECLARE_DO_FUN(	do_sober	);
DECLARE_DO_FUN(	do_track	);
DECLARE_DO_FUN(	do_whirlwind	);
DECLARE_DO_FUN(	do_mount	);
DECLARE_DO_FUN(	do_dismount	);

DECLARE_DO_FUN(	do_initiate	);
DECLARE_DO_FUN(	do_exil		);
DECLARE_DO_FUN(	do_exile	);
DECLARE_DO_FUN(	do_promote	);
DECLARE_DO_FUN(	do_demote	);
DECLARE_DO_FUN(	do_crecall	);
DECLARE_DO_FUN(	do_cdonate	);
DECLARE_DO_FUN(	do_leav		);
DECLARE_DO_FUN(	do_leave	);
DECLARE_DO_FUN(	do_claninfo	);
DECLARE_DO_FUN(	do_guilds	);
DECLARE_DO_FUN(	do_clans	);
DECLARE_DO_FUN(	do_orders	);

DECLARE_DO_FUN(	do_showclan	);
DECLARE_DO_FUN(	do_setclan	);
DECLARE_DO_FUN(	do_makeclan	);

DECLARE_DO_FUN(	do_mudconfig	);

DECLARE_DO_FUN(	do_shoot	);
DECLARE_DO_FUN(	do_reload	);
DECLARE_DO_FUN( do_wiznet       );
DECLARE_DO_FUN(	do_astat	);
DECLARE_DO_FUN(	do_history	);
DECLARE_DO_FUN(	do_qhelp	);
DECLARE_DO_FUN(	do_mclone	);
DECLARE_DO_FUN(	do_oclone	);
DECLARE_DO_FUN(	do_speak	);
DECLARE_DO_FUN(	do_ctitle	);
DECLARE_DO_FUN(	do_cpose	);
DECLARE_DO_FUN(	do_cinfo	);
DECLARE_DO_FUN(	do_cslist	);


/*
 * Mobile functions.
 * Defined in spec_mob.c.
 */
DECLARE_MOB_FUN(	spec_breath_any		);
DECLARE_MOB_FUN(	spec_breath_acid	);
DECLARE_MOB_FUN(	spec_breath_fire	);
DECLARE_MOB_FUN(	spec_breath_frost	);
DECLARE_MOB_FUN(	spec_breath_gas		);
DECLARE_MOB_FUN(	spec_breath_lightning	);
DECLARE_MOB_FUN(	spec_cast_adept		);
DECLARE_MOB_FUN(	spec_cast_cleric	);
DECLARE_MOB_FUN(	spec_cast_ghost         );
DECLARE_MOB_FUN(	spec_cast_judge		);
DECLARE_MOB_FUN(	spec_cast_mage		);
DECLARE_MOB_FUN(	spec_cast_psionicist    );
DECLARE_MOB_FUN(	spec_cast_undead	);
DECLARE_MOB_FUN(	spec_executioner	);
DECLARE_MOB_FUN(	spec_fido		);
DECLARE_MOB_FUN(	spec_guard		);
DECLARE_MOB_FUN(	spec_janitor		);
DECLARE_MOB_FUN(	spec_mayor		);
DECLARE_MOB_FUN(	spec_poison		);
DECLARE_MOB_FUN(	spec_repairman		);
DECLARE_MOB_FUN(	spec_thief		);


/*
 * Object functions.
 * Defined in spec_obj.c.
 */
DECLARE_OBJ_FUN(	spec_giggle		);
DECLARE_OBJ_FUN(	spec_soul_moan		);


/*
 * Room functions.
 * Defined in spec_rom.c.
 */
DECLARE_ROOM_FUN(	spec_deathtrap		);


/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(	spell_acid_blast	);
DECLARE_SPELL_FUN(	spell_armor		);
DECLARE_SPELL_FUN(	spell_bless		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_breathe_water	);	/* by Thelonius */
DECLARE_SPELL_FUN(	spell_call_lightning	);
DECLARE_SPELL_FUN(	spell_cause_critical	);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(	spell_change_sex	);
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_chill_touch	);
DECLARE_SPELL_FUN(	spell_colour_spray	);
DECLARE_SPELL_FUN(	spell_cone_of_silence	);	/* by Thelonius */
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
DECLARE_SPELL_FUN(	spell_destroy_cursed    );
DECLARE_SPELL_FUN(	spell_detect_evil	);
DECLARE_SPELL_FUN(	spell_detect_hidden	);
DECLARE_SPELL_FUN(	spell_detect_invis	);
DECLARE_SPELL_FUN(	spell_detect_magic	);
DECLARE_SPELL_FUN(	spell_detect_poison	);
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(	spell_dispel_magic	);	/* by Thelonius */
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_exorcise  	);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(	spell_flaming           );
DECLARE_SPELL_FUN(	spell_fly		);
DECLARE_SPELL_FUN(	spell_gate		);
DECLARE_SPELL_FUN(	spell_general_purpose	);
DECLARE_SPELL_FUN(	spell_giant_strength	);
DECLARE_SPELL_FUN(	spell_harm		);
DECLARE_SPELL_FUN(	spell_heal		);
DECLARE_SPELL_FUN(	spell_high_explosive	);
DECLARE_SPELL_FUN(	spell_identify		);
DECLARE_SPELL_FUN(	spell_infravision	);
DECLARE_SPELL_FUN(	spell_invis		);
DECLARE_SPELL_FUN(	spell_know_alignment	);
DECLARE_SPELL_FUN(	spell_lightning_bolt	);
DECLARE_SPELL_FUN(	spell_locate_object	);
DECLARE_SPELL_FUN(	spell_magic_missile	);
DECLARE_SPELL_FUN(	spell_mass_heal 	);
DECLARE_SPELL_FUN(	spell_mass_invis	);
DECLARE_SPELL_FUN(	spell_mute		);	/* by Thelonius */
DECLARE_SPELL_FUN(	spell_pass_door		);
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(	spell_polymorph_other	);
DECLARE_SPELL_FUN(	spell_protection_evil	);
DECLARE_SPELL_FUN(	spell_recharge_item	);	/* by Thelonius */
DECLARE_SPELL_FUN(	spell_refresh		);
DECLARE_SPELL_FUN(	spell_remove_alignment	);	/* by Thelonius */
DECLARE_SPELL_FUN(	spell_remove_curse	);
DECLARE_SPELL_FUN(	spell_remove_silence	);	/* by Thelonius */
DECLARE_SPELL_FUN(	spell_sanctuary		);
DECLARE_SPELL_FUN(	spell_shocking_grasp	);
DECLARE_SPELL_FUN(	spell_shield		);
DECLARE_SPELL_FUN(	spell_sleep		);
DECLARE_SPELL_FUN(	spell_stone_skin	);
DECLARE_SPELL_FUN(	spell_summon		);
DECLARE_SPELL_FUN(	spell_teleport		);
DECLARE_SPELL_FUN(	spell_turn_undead	);
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(	spell_word_of_recall	);
DECLARE_SPELL_FUN(	spell_acid_breath	);
DECLARE_SPELL_FUN(	spell_fire_breath	);
DECLARE_SPELL_FUN(	spell_frost_breath	);
DECLARE_SPELL_FUN(	spell_gas_breath	);
DECLARE_SPELL_FUN(	spell_lightning_breath	);
DECLARE_SPELL_FUN(	spell_vampiric_bite	);

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
DECLARE_SPELL_FUN(      spell_ego_whip          );
DECLARE_SPELL_FUN(      spell_energy_containment);
DECLARE_SPELL_FUN(      spell_enhance_armor     );
DECLARE_SPELL_FUN(      spell_enhanced_strength );
DECLARE_SPELL_FUN(      spell_flesh_armor       );
DECLARE_SPELL_FUN(      spell_inertial_barrier  );
DECLARE_SPELL_FUN(      spell_inflict_pain      );
DECLARE_SPELL_FUN(      spell_intellect_fortress);
DECLARE_SPELL_FUN(      spell_lend_health       );
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
 * New spell functions, in magic.c (by Zen).
 */
DECLARE_SPELL_FUN(	spell_detect_good	);
DECLARE_SPELL_FUN(	spell_dispel_good	);
DECLARE_SPELL_FUN(      spell_meteor_swarm	);
DECLARE_SPELL_FUN(      spell_chain_lightning	);
DECLARE_SPELL_FUN(      spell_vortex_lift	);
DECLARE_SPELL_FUN(      spell_wizard_eye	);
DECLARE_SPELL_FUN(      spell_mass_vortex_lift	);
DECLARE_SPELL_FUN(      spell_portal		);
DECLARE_SPELL_FUN(      spell_nexus		);
DECLARE_SPELL_FUN(	spell_protection_good	);
DECLARE_SPELL_FUN(	spell_create_buffet	);
DECLARE_SPELL_FUN(	spell_cure_disease	);
DECLARE_SPELL_FUN(	spell_plague		);

DECLARE_SPELL_FUN(	spell_flame_shield	);
DECLARE_SPELL_FUN(	spell_frost_shield	);
DECLARE_SPELL_FUN(	spell_shock_shield	);

DECLARE_SPELL_FUN(	spell_blazeward		);
DECLARE_SPELL_FUN(	spell_blazebane		);
DECLARE_SPELL_FUN(	spell_inner_warmth	);
DECLARE_SPELL_FUN(	spell_winter_mist	);
DECLARE_SPELL_FUN(	spell_ethereal_shield	);
DECLARE_SPELL_FUN(	spell_ethereal_funnel	);
DECLARE_SPELL_FUN(	spell_antimagic_shell	);
DECLARE_SPELL_FUN(	spell_eldritch_sphere	);
DECLARE_SPELL_FUN(	spell_unravel_defense	);
DECLARE_SPELL_FUN(	spell_aquiles_power	);
DECLARE_SPELL_FUN(	spell_demon_skin	);
DECLARE_SPELL_FUN(	spell_swordbait		);
DECLARE_SPELL_FUN(	spell_dragon_skin	);
DECLARE_SPELL_FUN(	spell_razorbait		);

DECLARE_SPELL_FUN(	spell_dragon_wit	);
DECLARE_SPELL_FUN(	spell_sagacity		);
DECLARE_SPELL_FUN(	spell_slink		);
DECLARE_SPELL_FUN(	spell_trollish_vigor	);


/*
 * Weapon functions.
 * Defined in fight.c.
 */
DECLARE_HIT_FUN(        hit_vorpal              );
DECLARE_HIT_FUN(        hit_suck_disarm         );



/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if	defined( _AIX )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

#if     defined( amiga )
#define AmigaTCP
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

#if	defined( macintosh ) || defined( _WIN32 )
#define NOCRYPT
#if	defined( unix )
#undef	unix
#endif
#endif

#if	defined( MIPS_OS )
char *	crypt		args( ( const char *key, const char *salt ) );
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
size_t	fread		args( ( void *ptr, size_t size, size_t nitems,
			       FILE *stream ) );
int	fseek		args( ( FILE *stream, long offset, int ptrname ) );
void	perror		args( ( const char *s ) );
int	ungetc		args( ( int c, FILE *stream ) );
#endif

#if	defined( ultrix )
char *	crypt		args( ( const char *key, const char *salt ) );
#endif

/*
 * Stuff for DEC UNIX on Alpha (OSF3.2C)
 * Fusion
 */
#if defined( _OSF_SOURCE )
char *	crypt           args( ( const char *key, const char *salt ) );
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
#define BACKUP_DIR	"proto.are"	/* Backup player files		*/
#define SYSTEM_DIR	""		/* System directory		*/
#define CLASS_DIR	""		/* New class loading scheme	*/
#define NULL_FILE	""		/* To reserve one stream	*/
#define AREA_DIR	""		/* Area files			*/
#define CLAN_DIR	""		/* Clan files              	*/
#define DICT_DIR	""		/* Dictionary files		*/
#define MOB_DIR		""		/* MOBProg files		*/
#endif

#if defined( unix ) || defined( linux )
#define PLAYER_DIR	"../player/"	/* Player files			*/
#define BACKUP_DIR	"../backup/"	/* Backup player files		*/
#define SYSTEM_DIR	"../sys/"	/* System directory		*/
#define CLASS_DIR	"../classes/"	/* New class loading scheme	*/
#define NULL_FILE	"/dev/null"	/* To reserve one stream	*/
#define AREA_DIR	"../area/"	/* Area files			*/
#define CLAN_DIR	"../clans/"	/* Clan files              	*/
#define DICT_DIR	"../dict/"	/* Dictionary files		*/
#define MOB_DIR		"../mobprogs/"	/* MOBProg files		*/
#endif

#if defined( AmigaTCP )
#define PLAYER_DIR	"envy:player/"	/* Player files			*/
#define BACKUP_DIR	"envy:backup/"	/* Backup player files		*/
#define SYSTEM_DIR	"envy:sys/"	/* System directory		*/
#define CLASS_DIR	"envy:classes/"	/* New class loading scheme	*/
#define NULL_FILE	"nil:"		/* To reserve one stream	*/
#define AREA_DIR	"envy:area/"	/* Area files			*/
#define CLAN_DIR	"envy:clans/"	/* Clan files              	*/
#define DICT_DIR	"envy:dict/"	/* Dictionary files		*/
#define MOB_DIR		"envy:mobprogs/"/* MOBProg files		*/
#endif

#if defined( _WIN32 )
#define PLAYER_DIR	"..\\player\\"	/* Player files			*/
#define BACKUP_DIR	"..\\backup\\"	/* Backup player files		*/
#define SYSTEM_DIR	"..\\sys\\"	/* System directory		*/
#define CLASS_DIR	"..\\classes\\"	/* New class loading scheme	*/
#define NULL_FILE	"nul"		/* To reserve one stream	*/
#define AREA_DIR	"..\\area\\"	/* Area files			*/
#define CLAN_DIR	"..\\clans\\"	/* Clan files              	*/
#define DICT_DIR	"..\\dict\\"	/* Dictionary files		*/
#define MOB_DIR		"..\\mobprogs\\"/* MOBProg files		*/
#endif

#define AREA_LIST	 "AREA.LST"	/* List of areas		*/

#define CLASS_LIST	 "CLASS.LST"	/* List of classes		*/
#define CLANS_LIST	 "CLANS.LST"	/* List of clans		*/
#define LANG_LIST	 "LANG.LST"	/* List of dictionaries.	*/
#define ASSHOLE_LIST     "ASSHOLE.LST"	/* For illegal names            */

#define BUG_FILE	 "BUGS.TXT"	/* For 'bug' and bug( )		*/
#define IDEA_FILE	 "IDEAS.TXT"	/* For 'idea'			*/
#define TYPO_FILE	 "TYPOS.TXT"	/* For 'typo'			*/
#define NOTE_FILE	 "NOTES.TXT"	/* For 'notes'			*/
#define CLAN_FILE	 "CLANS.TXT"	/* For clan code		*/
#define SHUTDOWN_FILE	 "SHUTDOWN.TXT"	/* For 'shutdown'		*/
#define DOWN_TIME_FILE   "TIME.TXT"	/* For automatic shutdown       */
#define BAN_FILE         "BAN.TXT"	/* For banned site save         */
#define SOCIAL_FILE	 "SOCIALS.TXT"	/* For socials			*/
#define SYSDATA_FILE	 "SYSDATA.TXT"	/* For mud configuration	*/
#define ANSI_TITLE_FILE  "MUDTITLE.ANS"	/* For ansi mud title screen    */
#define ASCII_TITLE_FILE "MUDTITLE.ASC"	/* For ascii mud title screen   */


/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define AD	AFFECT_DATA
#define ED	EXTRA_DESCR_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define MF	MOB_FUN
#define OF	OBJ_FUN
#define RF	ROOM_FUN
#define SID	SOC_INDEX_DATA
#define GF      GAME_FUN
#define CLD     CLAN_DATA
#define HHF     HHF_DATA

/* act_comm.c */
void	add_follower	args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	stop_follower	args( ( CHAR_DATA *ch ) );
void	die_follower	args( ( CHAR_DATA *ch, char *name ) );
bool	is_same_group	args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
bool	is_same_clan	args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void	send_ansi_title args( ( DESCRIPTOR_DATA *d ) );
void	send_ascii_title	args( ( DESCRIPTOR_DATA *d ) );
void	news_channel	args( ( CHAR_DATA *ch, const char *string ) );

/* act_info.c */
void	set_title	args( ( CHAR_DATA *ch, char *title ) );
bool	check_blind	args( ( CHAR_DATA *ch ) );
void	show_room_info	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room ) );

/* act_lang.c */
void	load_languages	args( ( void ) );
void	save_languages	args( ( void ) );
bool	knows_language	args( ( CHAR_DATA *ch, int language, CHAR_DATA *cch ) );
char *	translate	args( ( char *str, int ln ) );
char *	lang_bit_name	args( ( int vector ) );


/* act_move.c */
void	move_char	args( ( CHAR_DATA *ch, int door ) );
RID *	get_random_room	args( ( void ) );

/* act_obj.c */
bool	remove_obj	args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );

/* act_wiz.c */
RID *	find_location	args( ( CHAR_DATA *ch, char *arg ) );

/* act_clan.c */
bool			is_clan		args( ( CHAR_DATA *ch ) );

/* comm.c */
void	close_socket	 args( ( DESCRIPTOR_DATA *dclose ) );
void	write_to_buffer	 args( ( DESCRIPTOR_DATA *d, const char *txt,
				int length ) );
void    send_to_room     args( ( const char *txt, ROOM_INDEX_DATA *room ) );
void    send_to_all_char args( ( const char *text ) );
void	send_to_char	 args( ( const char *txt, CHAR_DATA *ch ) );
void	printf_to_char	 args( ( CHAR_DATA *ch, char *fmt, ... ) );
void    show_string      args( ( DESCRIPTOR_DATA *d, char *input ) );
void	act	         args( ( const char *format, CHAR_DATA *ch,
				const void *arg1, const void *arg2,
				int type ) );
/*
 * Colour stuff by Lope of Loping Through The MUD
 */
int	colour		args( ( char type, CHAR_DATA *ch, char *string ) );
void	colourconv	args( ( char *buffer, const char *txt, CHAR_DATA *ch ) );
void	send_to_char_bw	args( ( const char *txt, CHAR_DATA *ch ) );

/* db.c */
void	boot_db		args( ( void ) );
void	area_update	args( ( void ) );
CD *	new_character	args( ( bool player ) );
AD *	new_affect	args( ( void ) );
OD *	new_object	args( ( void ) );
ED *	new_extra_descr args( ( void ) );
CD *	create_mobile	args( ( MOB_INDEX_DATA *pMobIndex ) );
OD *	create_object	args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void	clear_char	args( ( CHAR_DATA *ch ) );
void	free_char	args( ( CHAR_DATA *ch ) );
char *	get_extra_descr	args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *	get_mob_index	args( ( int vnum ) );
OID *	get_obj_index	args( ( int vnum ) );
RID *	get_room_index	args( ( int vnum ) );
int	fread_letter	args( ( FILE *fp ) );
int	fread_number	args( ( FILE *fp, int *status ) );
void	fread_vector	args( ( FILE *fp, u_intc *pvector ) );
char *	fread_string	args( ( FILE *fp, int *status ) );
void	fread_to_eol	args( ( FILE *fp ) );
char *	fread_word	args( ( FILE *fp, int *status ) );
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
char *  all_capitalize  args( ( const char *str ) );
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	bug		args( ( const char *str, int param ) );
void	log_string	args( ( const char *str ) );
void	tail_chain	args( ( void ) );

void	bugf		args( ( char *fmt, ... ) );
void	logf		args( ( char *fmt, ... ) );

void	log_clan	args( ( const char *str ) );
void	temp_fread_string	args( ( FILE *fp, char *str ) );
void	conv_braces	args( ( char *buffer, const char *str ) );
void	unconv_braces	args( ( char *buffer, const char *str ) );
void	save_sysdata	args( ( void ) );

/* fight.c */
void	violence_update	args( ( void ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	damage		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			       int dt, int wpn, int dam_type ) );
void	raw_kill	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    death_cry	args( ( CHAR_DATA *ch ) );
void    stop_hunting    args( ( CHAR_DATA *ch ) );
void    stop_hating     args( ( CHAR_DATA *ch ) );
void    stop_fearing    args( ( CHAR_DATA *ch ) );
void    start_hunting   args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    start_hating    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    start_fearing   args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_hunting      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_hating       args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_fearing      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	check_killer	     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_safe		     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    in_arena             args( ( CHAR_DATA *ch ) );
bool    licensed             args( ( CHAR_DATA *ch ) );
bool    registered           args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/* handler.c */
int	get_trust	args( ( CHAR_DATA *ch ) );
int	get_age		args( ( CHAR_DATA *ch ) );
int	get_curr_str	args( ( CHAR_DATA *ch ) );
int	get_curr_int	args( ( CHAR_DATA *ch ) );
int	get_curr_wis	args( ( CHAR_DATA *ch ) );
int	get_curr_dex	args( ( CHAR_DATA *ch ) );
int	get_curr_con	args( ( CHAR_DATA *ch ) );
int	get_max_str	args( ( CHAR_DATA *ch ) );
int	get_max_int	args( ( CHAR_DATA *ch ) );
int	get_max_wis	args( ( CHAR_DATA *ch ) );
int	get_max_dex	args( ( CHAR_DATA *ch ) );
int	get_max_con	args( ( CHAR_DATA *ch ) );
int     get_hitroll     args( ( CHAR_DATA *ch, int wpn ) );
int     get_damroll     args( ( CHAR_DATA *ch, int wpn ) );
int	can_carry_n	args( ( CHAR_DATA *ch ) );
int	can_carry_w	args( ( CHAR_DATA *ch ) );
bool	is_name		args( ( const char *str, char *namelist ) );
bool	is_name_prefix	args( ( const char *str, char *namelist ) );
void	affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_strip	args( ( CHAR_DATA *ch, int sn ) );
bool	is_affected	args( ( CHAR_DATA *ch, int sn ) );
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
CD *	get_char_area	args( ( CHAR_DATA *ch, char *argument ) );
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
char *	item_type_name	args( ( OBJ_DATA *obj ) );
char *	affect_loc_name	args( ( int location ) );
char *	affect_bit_name	args( ( u_intc *vector ) );
char *	extra_bit_name	args( ( int extra_flags ) );
char *	parts_bit_name	args( ( int vector ) );
char *	ris_bit_name	args( ( int vector ) );
CD   *  get_char        args( ( CHAR_DATA *ch ) );
bool    longstring      args( ( CHAR_DATA *ch, char *argument ) );
bool    authorized      args( ( CHAR_DATA *ch, char *skllnm ) );
void    end_of_game     args( ( void ) );
int     race_lookup     args( ( const char *race ) );
int     affect_lookup   args( ( const char *race ) );
int	advatoi		args( ( const char *s ) );
CLD  *	clan_lookup	args( ( const char *name ) );
int	race_full_lookup args( ( const char *race ) );
int	check_ris	args( ( CHAR_DATA *ch, int dam_type ) );
void	learn		args( ( CHAR_DATA *ch, int sn, bool success ) );

u_intc *	vzero	args( ( u_intc *vector ) );
u_intc *	vcopy	args( ( u_intc *dest, const u_intc *src ) );

bool	can_use		args( ( CHAR_DATA *ch, int sn ) );
CLASS_TYPE *skill_class	args( ( CHAR_DATA *ch, int sn ) );
bool	can_prac	args( ( CHAR_DATA *ch, int sn ) );
bool	has_spells	args( ( CHAR_DATA *ch ) );
bool	is_class	args( ( CHAR_DATA *ch, CLASS_TYPE *class ) );
int	number_classes	args( ( CHAR_DATA *ch ) );
char *	class_long	args( ( CHAR_DATA *ch ) );
char *	class_short	args( ( CHAR_DATA *ch ) );
CLASS_TYPE *class_lookup	args( ( const char *name ) );


/* interp.c */
void	interpret	args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number	args( ( char *arg ) );
int	number_argument	args( ( char *argument, char *arg ) );
char *	one_argument	args( ( char *argument, char *arg_first ) );
bool    IS_SWITCHED     args( ( CHAR_DATA *ch ) );
void	strexg		args( ( char *target, char *source, char *c, char *t ) );

/* magic.c */
int	skill_lookup	args( ( const char *name ) );
int	skill_blookup	args( ( const char *name, int first, int last ) );
bool	saves_spell	args( ( int level, CHAR_DATA *victim, int dam_type ) );
void	obj_cast_spell	args( ( int sn, int level, CHAR_DATA *ch,
			       CHAR_DATA *victim, OBJ_DATA *obj ) );

/* mem.c */
HHF *	new_hhf_data	args( ( void ) );
void	free_hhf_data	args( ( HHF_DATA *pHhf ) );


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

/* save.c */
void	backup_char_obj	args( ( CHAR_DATA *ch ) ); /* Zen was here :) */
void	delete_char_obj	args( ( CHAR_DATA *ch ) ); /* Zen was here :) */
void	save_char_obj	args( ( CHAR_DATA *ch ) );
bool	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name ) );

/* spec_mob.c */
MF *	spec_mob_lookup	 args( ( const char *name ) );

/* spec_obj.c */
OF *	spec_obj_lookup	 args( ( const char *name ) );

/* spec_rom.c */
RF *	spec_room_lookup args( ( const char *name ) );

/* tables.c */
void	clear_social	args( ( SOC_INDEX_DATA *soc ) );
void	extract_social	args( ( SOC_INDEX_DATA *soc ) );
SID *	new_social	args( ( void ) );
void	free_social	args( ( SOC_INDEX_DATA *soc ) );

void	load_socials	args( ( void ) );
void	save_socials	args( ( void ) );
void	load_classes	args( ( void ) );
void	save_classes	args( ( void ) );
void	save_clan	args( ( CLAN_DATA *clan ) );
void	save_clan_list  args( ( void ) );
void	load_clans	args( ( void ) );

CLAN_DATA * get_clan		args( ( const char *name ) );

/* track.c */
void    found_prey      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    hunt_victim     args( ( CHAR_DATA *ch) );

/* gamble.c */
GF *    game_lookup     args( ( const char *name ) );

/* update.c */
void	advance_level	args( ( CHAR_DATA *ch ) );
void	demote_level	args( ( CHAR_DATA *ch ) );
void	gain_exp	args( ( CHAR_DATA *ch, int gain ) );
void	gain_condition	args( ( CHAR_DATA *ch, int iCond, int value ) );
void	update_handler	args( ( void ) );
void    ban_update      args( ( void ) );

/* wiznet.c */
void	wiznet		args ( ( CHAR_DATA *ch, int chan,
				int level, const char * string ) );

/* olc_save.c */
char *  mprog_type_to_name      args ( ( int type ) );
char *	fix_string		args ( ( const char *str ) );

#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	MF
#undef	OF
#undef	RF
#undef	SID
#undef	GF
#undef	CLD
#undef	HHF


/***************************************************************************
 *                                                                         *
 *                                    OLC                                  *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * This structure is used in spec_*.c to lookup spec funs and
 * also in olc_act.c to display listings of spec funs.
 */
struct spec_mob_type
{
    char *	spec_name;
    MOB_FUN *	spec_fun;
};

struct spec_obj_type
{
    char *	spec_name;
    OBJ_FUN *	spec_fun;
};

struct spec_room_type
{
    char *	spec_name;
    ROOM_FUN *	spec_fun;
};

struct game_type
{
    char *      game_name;
    GAME_FUN *  game_fun;
};


/* This structure is used in bit.c to lookup flags and stats */
struct flag_type
{
    char *	name;
    int		bit;
    bool	settable;
};


/*
 * Area flags.
 */
#define AREA_NONE	     	      0
#define AREA_CHANGED		   BV00		/* Area has been modified    */
#define AREA_ADDED		   BV01		/* Area has been added to    */
#define AREA_LOADING		   BV02		/* Used for counting in db.c */
#define AREA_VERBOSE		   BV03

#define MAX_DIR			      6
#define NO_FLAG			    -99		/* Must not be used in flags or stats */


/*
 * Command functions.
 * Defined in olc.c
 */
DECLARE_DO_FUN( do_aedit        );
DECLARE_DO_FUN( do_redit        );
DECLARE_DO_FUN( do_oedit        );
DECLARE_DO_FUN( do_medit        );
DECLARE_DO_FUN( do_asave        );
DECLARE_DO_FUN( do_alist        );
DECLARE_DO_FUN( do_resets       );


/*
 * Interpreter prototypes.
 */
void    aedit   args( ( CHAR_DATA * ch, char *argument ) );
void    redit   args( ( CHAR_DATA * ch, char *argument ) );
void    medit   args( ( CHAR_DATA * ch, char *argument ) );
void    oedit   args( ( CHAR_DATA * ch, char *argument ) );
void    mpedit  args( ( CHAR_DATA * ch, char *argument ) );

/*
 * Global Constants
 */
extern  char *  const			dir_name        [ ];
extern  const   int			rev_dir         [ ];
extern  const   struct  spec_mob_type	spec_mob_table	[ ];
extern  const   struct  spec_obj_type	spec_obj_table	[ ];
extern  const   struct  spec_room_type	spec_room_table	[ ];
extern  const   struct  game_type       game_table      [ ];

/*
 * Global variables
 */
extern          AREA_DATA *             area_first;
extern          AREA_DATA *             area_last;
extern          SHOP_DATA *             shop_last;
extern          GAME_DATA *             game_last;

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
extern          int                     top_game;
extern          int                     top_hhf;

extern          int                     top_vnum_mob;
extern          int                     top_vnum_obj;
extern          int                     top_vnum_room;

extern          char                    str_empty       [1];

extern		MOB_INDEX_DATA    *	mob_index_hash	[ MAX_KEY_HASH ];
extern		OBJ_INDEX_DATA    *	obj_index_hash	[ MAX_KEY_HASH ];
extern		ROOM_INDEX_DATA   *	room_index_hash	[ MAX_KEY_HASH ];
extern		SOC_INDEX_DATA    *	soc_index_hash	[ MAX_WORD_HASH ];
extern		ACT_PROG_DATA     *	mob_act_list;

/* db.c */
void    reset_area      args( ( AREA_DATA * pArea ) );
void    reset_room      args( ( ROOM_INDEX_DATA *pRoom ) );
void    do_abort        args( ( void ) );

/* string.c */
void    string_edit     args( ( CHAR_DATA *ch, char **pString ) );
void    string_append   args( ( CHAR_DATA *ch, char **pString ) );
char *  string_replace  args( ( char * orig, char * old, char * new ) );
void    string_add      args( ( CHAR_DATA *ch, char *argument ) );
char *  format_string   args( ( char *oldstring /*, bool fSpace */ ) );
char *  first_arg       args( ( char *argument, char *arg_first, bool fCase ) );
char *  string_unpad    args( ( char * argument ) );
char *  string_proper   args( ( char * argument ) );

/* olc.c */
bool    run_olc_editor  args( ( DESCRIPTOR_DATA *d ) );
char    *olc_ed_name    args( ( CHAR_DATA *ch ) );
char    *olc_ed_vnum    args( ( CHAR_DATA *ch ) );

/* spec_mob.c */
char *  spec_mob_string	 args( ( MOB_FUN *fun ) );

/* spec_obj.c */
char *  spec_obj_string	 args( ( OBJ_FUN *fun ) );

/* spec_room.c */
char *  spec_room_string args( ( ROOM_FUN *fun ) );

/* gamble.c */
char *  game_string     args( ( GAME_FUN *fun ) );

/* bit.c */
extern	const	struct	flag_type	connected_flags		[ ];
extern	const	struct	flag_type	area_flags		[ ];
extern	const	struct	flag_type	sex_flags		[ ];
extern	const	struct	flag_type	size_flags		[ ];
extern	const	struct	flag_type	exit_flags		[ ];
extern	const	struct	flag_type	door_resets		[ ];
extern	const	struct	flag_type	room_flags		[ ];
extern	const	struct	flag_type	sector_flags		[ ];
extern	const	struct	flag_type	type_flags		[ ];
extern	const	struct	flag_type	extra_flags		[ ];
extern	const	struct	flag_type	wear_flags		[ ];
extern	const	struct	flag_type	act_flags		[ ];
extern	const	struct	flag_type	plr_flags		[ ];
extern	const	struct	flag_type	affect_flags		[ ];
extern	const	struct	flag_type	apply_flags		[ ];
extern	const	struct	flag_type	wear_loc_strings	[ ];
extern	const	struct	flag_type	wear_loc_flags		[ ];
extern	const	struct	flag_type	weapon_flags		[ ];
extern	const	struct	flag_type	container_flags		[ ];
extern	const	struct	flag_type	liquid_flags		[ ];
extern	const	struct	flag_type	mprog_type_flags	[ ];
extern	const	struct	flag_type	portal_door_flags	[ ];
extern	const	struct	flag_type	portal_flags		[ ];
extern	const	struct	flag_type	mana_flags		[ ];
extern	const	struct	flag_type	clan_flags		[ ];
extern	const	struct	flag_type	rank_flags		[ ];


/***************************************************************************
 *                                                                         *
 *                                    OLC                                  *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/
