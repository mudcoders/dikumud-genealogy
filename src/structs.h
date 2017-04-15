/***************************************************************************
 *  file: structs.h , Structures                           Part of DIKUMUD *
 *  Usage: Declarations of central data structures                         *
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

#include <sys/types.h>

typedef signed char		sbyte;
typedef unsigned char		ubyte;
typedef signed short int	sh_int;
typedef unsigned short int	ush_int;
typedef char			bool;
typedef char			byte;

typedef	struct char_data	CHAR_DATA;
typedef	struct obj_data		OBJ_DATA;

#define PULSE_ZONE     240
#define PULSE_MOBILE    16
#define PULSE_VIOLENCE  12
#define WAIT_SEC         4
#define WAIT_ROUND       4

#define MAX_STRING_LENGTH   8192
#define MAX_INPUT_LENGTH     160
#define MAX_MESSAGES          60

#define MESS_ATTACKER 1
#define MESS_VICTIM   2
#define MESS_ROOM     3

#define SECS_PER_REAL_MIN  60
#define SECS_PER_REAL_HOUR (60*SECS_PER_REAL_MIN)
#define SECS_PER_REAL_DAY  (24*SECS_PER_REAL_HOUR)
#define SECS_PER_REAL_YEAR (365*SECS_PER_REAL_DAY)

#define SECS_PER_MUD_HOUR  31
#define SECS_PER_MUD_DAY   (24*SECS_PER_MUD_HOUR)
#define SECS_PER_MUD_MONTH (35*SECS_PER_MUD_DAY)
#define SECS_PER_MUD_YEAR  (17*SECS_PER_MUD_MONTH)

/*
 * Site ban structure.
 */
struct ban_t
{
	char *name;             /* Name of site */
	struct ban_t *next;     /* next in list */
};

/* ======================================================================= */

/* The following defs are for room_data  */

#define NOWHERE    -1    /* nil reference for room-database    */

/* Bitvector For 'room_flags' */

#define DARK           1
#define DEATH          2
#define NO_MOB         4
#define INDOORS        8
#define LAWFULL       16
#define NEUTRAL       32
#define CHAOTIC       64
#define NO_MAGIC     128
#define TUNNEL       256
#define PRIVATE      512
#define SAFE        1024

/* For 'dir_option' */

#define NORTH          0
#define EAST           1
#define SOUTH          2
#define WEST           3
#define UP             4
#define DOWN           5

#define EX_ISDOOR      1
#define EX_CLOSED      2
#define EX_LOCKED      4
#define EX_RSCLOSED    8
#define EX_RSLOCKED   16
#define EX_PICKPROOF  32

/* For 'Sector types' */

#define SECT_INSIDE          0
#define SECT_CITY            1
#define SECT_FIELD           2
#define SECT_FOREST          3
#define SECT_HILLS           4
#define SECT_MOUNTAIN        5
#define SECT_WATER_SWIM      6
#define SECT_WATER_NOSWIM    7
#define SECT_NO_LOW          8
#define SECT_NO_HIGH         9
#define SECT_DESERT         10

struct room_direction_data
{
    char *general_description;       /* When look DIR.                  */ 
    char *keyword;                   /* for open/close                  */  
    sh_int exit_info;                /* Exit info                       */
    sh_int key;                        /* Key's number (-1 for no key)    */
    sh_int to_room;                  /* Where direction leeds (NOWHERE) */
};

/* ========================= Structure for room ========================== */
struct room_data
{
    sh_int number;               /* Rooms number                       */
    sh_int zone;                 /* Room zone (for resetting)          */
    int sector_type;             /* sector type (move/hide)            */
    char *name;                  /* Rooms name 'You are ...'           */
    char *description;           /* Shown when entered                 */
    struct extra_descr_data *ex_description; /* for examine/look       */
    struct room_direction_data *dir_option[6]; /* Directions           */
    sh_int room_flags;           /* DEATH,DARK ... etc                 */ 
    byte light;                  /* Number of lightsources in room     */
    int (*funct)();              /* special procedure                  */
	 
    struct obj_data *contents;   /* List of items in room              */
    struct char_data *people;    /* List of NPC / PC in room           */
};



/* This structure is purely intended to be an easy way to transfer */
/* and return information about time (real or mudwise).            */
struct time_info_data
{
    int hours;
    int day;
    int month;
    int year;
};

/* These data contain information about a players time data */
struct time_data
{
  time_t birth;    /* This represents the characters age                */
  time_t logon;    /* Time of the last logon (used to calculate played) */
  int played;      /* This is the total accumulated time played in secs */
};

/* ======================================================================== */

/* How much light is in the land ? */

#define SUN_DARK    0
#define SUN_RISE    1
#define SUN_LIGHT   2
#define SUN_SET     3

/* And how is the sky ? */

#define SKY_CLOUDLESS   0
#define SKY_CLOUDY  1
#define SKY_RAINING 2
#define SKY_LIGHTNING   3

struct weather_data
{
    int pressure;   /* How is the pressure ( Mb ) */
    int change; /* How fast and what way does it change. */
    int sky;    /* How is the sky. */
    int sunlight;   /* And how much sun. */
};



/* ***********************************************************
*  The following structures are related to descriptor_data   *
*********************************************************** */



struct txt_block
{
    char *text;
    struct txt_block *next;
};

typedef struct txt_q
{
    struct txt_block *head;
    struct txt_block *tail;
} TXT_Q;



/* modes of connectedness */

#define CON_PLAYING			 0
#define CON_GET_NAME			 1
#define CON_GET_OLD_PASSWORD		 2
#define CON_CONFIRM_NEW_NAME		 3
#define CON_GET_NEW_PASSWORD		 4
#define CON_CONFIRM_NEW_PASSWORD	 5
#define CON_GET_NEW_SEX			 6
#define CON_GET_NEW_CLASS		 7
#define CON_READ_MOTD			 8
#define CON_SELECT_MENU			 9
#define CON_RESET_PASSWORD		10
#define CON_CONFIRM_RESET_PASSWORD	11
#define CON_EXDSCR			12

struct snoop_data
{
    struct char_data *snooping; 
    struct char_data *snoop_by;
};

struct descriptor_data
{
    int		descriptor;		/* file descriptor for socket	*/
    char *	name;			/* Copy of the player name	*/
    char	host[50];              	/* hostname			*/
    int		pos;			/* position in player-file	*/
    int		connected;		/* mode of 'connectedness'	*/
    int		wait;			/* wait for how many loops	*/
    char *	showstr_head;		/* for paging through texts	*/
    char *	showstr_point;		/*       -			*/
    bool	newline;		/* prepend newline in output	*/
    char	**str;			/* for the modify-str system	*/
    int		max_str;		/*	-			*/
    char	buf[10*MAX_INPUT_LENGTH];	/* buffer for raw input	*/
    char	last_input[MAX_INPUT_LENGTH];	/* the last input	*/
    TXT_Q	output;			/* queue of strings to send	*/
    TXT_Q	input;			/* queue of unprocessed input	*/
    CHAR_DATA *	character;		/* linked to char		*/
    CHAR_DATA *	original;		/* for switch / return		*/
    struct snoop_data	snoop;		/* to snoop people		*/
    struct descriptor_data *	next;	/* link to next descriptor	*/
    int         tick_wait;      	/* # ticks desired to wait	*/
};

struct msg_type 
{
    char *attacker_msg;  /* message to attacker */
    char *victim_msg;    /* message to victim   */
    char *room_msg;      /* message to room     */
};

struct message_type
{
    struct msg_type die_msg;      /* messages when death            */
    struct msg_type miss_msg;     /* messages when miss             */
    struct msg_type hit_msg;      /* messages when hit              */
    struct msg_type sanctuary_msg;/* messages when hit on sanctuary */
    struct msg_type god_msg;      /* messages when hit on god       */
    struct message_type *next;/* to next messages of this kind.*/
};

struct message_list
{
    int a_type;               /* Attack type				*/
    int number_of_attacks;    /* # messages to chose from		*/
    struct message_type *msg; /* List of messages			*/
};

struct dex_skill_type
{
    sh_int p_pocket;
    sh_int p_locks;
    sh_int traps;
    sh_int sneak;
    sh_int hide;
};

struct dex_app_type
{
    sh_int reaction;
    sh_int miss_att;
    sh_int defensive;
};

struct str_app_type
{
    sh_int tohit;    /* To Hit (THAC0) Bonus/Penalty        */
    sh_int todam;    /* Damage Bonus/Penalty                */
    sh_int carry_w;  /* Maximum weight that can be carrried */
    sh_int wield_w;  /* Maximum weight that can be wielded  */
};

struct wis_app_type
{
    byte bonus;       /* how many bonus skills a player can */
		      /* practice pr. level                 */
};

struct int_app_type
{
    byte learn;       /* how many % a player learns a spell/skill */
};

struct con_app_type
{
    sh_int hitp;
    sh_int shock;
};

/*
 * TO types for act() output.
 */
#define TO_ROOM    0
#define TO_VICT    1
#define TO_NOTVICT 2
#define TO_CHAR    3
