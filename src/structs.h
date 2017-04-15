/* ************************************************************************
*   File: structs.h                                     Part of CircleMUD *
*  Usage: header file for central structures and contstants               *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <sys/types.h>

typedef signed char	sbyte;
typedef unsigned char	ubyte;
typedef signed short int	sh_int;
typedef unsigned short int	ush_int;
typedef char	bool;
typedef char	byte;

#define LEVEL_IMPL	34
#define LEVEL_GRGOD	33
#define LEVEL_GOD	32
#define LEVEL_IMMORT	31

#define LEVEL_FREEZE	LEVEL_GRGOD

#define NUM_OF_DIRS	6
#define PULSE_ZONE      40
#define PULSE_MOBILE    40
#define PULSE_VIOLENCE  12
#define WAIT_SEC	4
#define WAIT_ROUND	4

#define SMALL_BUFSIZE	     512
#define LARGE_BUFSIZE	   16384
#define MAX_STRING_LENGTH   8192
#define MAX_INPUT_LENGTH     255
#define MAX_MESSAGES          60
#define MAX_ITEMS            153
#define MAX_NAME_LENGTH	      15
#define MAX_PWD_LENGTH	      10 /* Used in char_file_u *DO*NOT*CHANGE* */
#define HOST_LEN	      30 /* Used in char_file_u *DO*NOT*CHANGE* */

#define MESS_ATTACKER 1
#define MESS_VICTIM   2
#define MESS_ROOM     3

#define SECS_PER_REAL_MIN  60
#define SECS_PER_REAL_HOUR (60*SECS_PER_REAL_MIN)
#define SECS_PER_REAL_DAY  (24*SECS_PER_REAL_HOUR)
#define SECS_PER_REAL_YEAR (365*SECS_PER_REAL_DAY)

#define SECS_PER_MUD_HOUR  75
#define SECS_PER_MUD_DAY   (24*SECS_PER_MUD_HOUR)
#define SECS_PER_MUD_MONTH (35*SECS_PER_MUD_DAY)
#define SECS_PER_MUD_YEAR  (17*SECS_PER_MUD_MONTH)

/* The following defs are for obj_data  */

/* For 'type_flag' */

#define ITEM_LIGHT      1
#define ITEM_SCROLL     2
#define ITEM_WAND       3
#define ITEM_STAFF      4
#define ITEM_WEAPON     5
#define ITEM_FIREWEAPON 6
#define ITEM_MISSILE    7
#define ITEM_TREASURE   8
#define ITEM_ARMOR      9
#define ITEM_POTION    10 
#define ITEM_WORN      11
#define ITEM_OTHER     12
#define ITEM_TRASH     13
#define ITEM_TRAP      14
#define ITEM_CONTAINER 15
#define ITEM_NOTE      16
#define ITEM_DRINKCON  17
#define ITEM_KEY       18
#define ITEM_FOOD      19
#define ITEM_MONEY     20
#define ITEM_PEN       21
#define ITEM_BOAT      22
#define ITEM_FOUNTAIN  23

/* Bitvector For 'wear_flags' */

#define ITEM_TAKE              1 
#define ITEM_WEAR_FINGER       2
#define ITEM_WEAR_NECK         4
#define ITEM_WEAR_BODY         8
#define ITEM_WEAR_HEAD        16
#define ITEM_WEAR_LEGS        32
#define ITEM_WEAR_FEET        64
#define ITEM_WEAR_HANDS      128 
#define ITEM_WEAR_ARMS       256
#define ITEM_WEAR_SHIELD     512
#define ITEM_WEAR_ABOUT     1024 
#define ITEM_WEAR_WAISTE    2048
#define ITEM_WEAR_WRIST     4096
#define ITEM_WIELD          8192
#define ITEM_HOLD          16384
#define ITEM_THROW         32768
/* UNUSED, CHECKS ONLY FOR ITEM_LIGHT #define ITEM_LIGHT_SOURCE  65536 */

/* Bitvector for 'extra_flags' */

#define ITEM_GLOW            (1 << 0)
#define ITEM_HUM             (1 << 1)
#define ITEM_DARK            (1 << 2)
#define ITEM_LOCK            (1 << 3)
#define ITEM_EVIL	     (1 << 4)
#define ITEM_INVISIBLE       (1 << 5)
#define ITEM_MAGIC           (1 << 6)
#define ITEM_NODROP          (1 << 7)
#define ITEM_BLESS           (1 << 8)
#define ITEM_ANTI_GOOD       (1 << 9)  /* not usable by good people    */
#define ITEM_ANTI_EVIL       (1 << 10) /* not usable by evil people    */
#define ITEM_ANTI_NEUTRAL    (1 << 11) /* not usable by neutral people */
#define ITEM_NORENT	     (1 << 12) /* not allowed to rent the item */
#define ITEM_NODONATE	     (1 << 13) /* not allowed to donate the item */
#define ITEM_NOINVIS	     (1 << 14) /* not allowed to cast invis on */

/* Some different kind of liquids */
#define LIQ_WATER      0
#define LIQ_BEER       1
#define LIQ_WINE       2
#define LIQ_ALE        3
#define LIQ_DARKALE    4
#define LIQ_WHISKY     5
#define LIQ_LEMONADE   6
#define LIQ_FIREBRT    7
#define LIQ_LOCALSPC   8
#define LIQ_SLIME      9
#define LIQ_MILK       10
#define LIQ_TEA        11
#define LIQ_COFFE      12
#define LIQ_BLOOD      13
#define LIQ_SALTWATER  14
#define LIQ_CLEARWATER 15

/* for containers  - value[1] */

#define CONT_CLOSEABLE      1
#define CONT_PICKPROOF      2
#define CONT_CLOSED         4
#define CONT_LOCKED         8

struct extra_descr_data {
   char	*keyword;                 /* Keyword in look/examine          */
   char	*description;             /* What to see                      */
   struct extra_descr_data *next; /* Next in list                     */
};

#define MAX_OBJ_AFFECT 2         /* Used in OBJ_FILE_ELEM *DO*NOT*CHANGE* */
#define OBJ_NOTIMER    -7000000

struct obj_flag_data {
   int	value[4];	/* Values of the item (see list)    */
   byte type_flag;	/* Type of item                     */
   int	wear_flags;	/* Where you can wear it            */
   int	extra_flags;	/* If it hums,glows etc             */
   int	weight;		/* Weigt what else                  */
   int	cost;		/* Value when sold (gp.)            */
   int	cost_per_day;	/* Cost to keep pr. real day        */
   int	timer;		/* Timer for object                 */
   long	bitvector;	/* To set chars bits                */
};

/* Used in OBJ_FILE_ELEM *DO*NOT*CHANGE* */
struct obj_affected_type {
   byte location;      /* Which ability to change (APPLY_XXX) */
   sbyte modifier;     /* How much it changes by              */
};

/* ======================== Structure for object ========================= */
struct obj_data {
   sh_int item_number;            /* Where in data-base               */
   sh_int in_room;                /* In what room -1 when conta/carr  */

   struct obj_flag_data obj_flags;/* Object information               */
   struct obj_affected_type affected[MAX_OBJ_AFFECT];  /* Which abilities in PC to change  */

   char	*name;                    /* Title of object :get etc.        */
   char	*description ;            /* When in room                     */
   char	*short_description;       /* when worn/carry/in cont.         */
   char	*action_description;      /* What to write when used          */
   struct extra_descr_data *ex_description; /* extra descriptions     */
   struct char_data *carried_by;  /* Carried by :NULL in room/conta   */

   struct obj_data *in_obj;       /* In what object NULL when none    */
   struct obj_data *contains;     /* Contains objects                 */

   struct obj_data *next_content; /* For 'contains' lists             */
   struct obj_data *next;         /* For the object list              */
};
/* ======================================================================= */

/* The following defs are for room_data  */

#define NOWHERE    -1    /* nil reference for room-database    */

/* Bitvector For 'room_flags' */

#define DARK           (1 << 0)
#define DEATH          (1 << 1)
#define NO_MOB         (1 << 2)
#define INDOORS        (1 << 3)
#define LAWFULL        (1 << 4)
#define NEUTRAL        (1 << 5)
#define CHAOTIC        (1 << 6)
#define NO_MAGIC       (1 << 7)
#define TUNNEL         (1 << 8)
#define PRIVATE        (1 << 9)
#define GODROOM        (1 << 10)
#define BFS_MARK       (1 << 11)

#define BFS_ERROR	   -1
#define BFS_ALREADY_THERE  -2
#define BFS_NO_PATH	   -3

/* For 'dir_option' */

#define NORTH          0
#define EAST           1
#define SOUTH          2
#define WEST           3
#define UP             4
#define DOWN           5

#define EX_ISDOOR		1
#define EX_CLOSED		2
#define EX_LOCKED		4
#define EX_RSCLOSED		8
#define EX_RSLOCKED		16
#define EX_PICKPROOF		32

/* For 'Sector types' */

#define SECT_INSIDE          0
#define SECT_CITY            1
#define SECT_FIELD           2
#define SECT_FOREST          3
#define SECT_HILLS           4
#define SECT_MOUNTAIN        5
#define SECT_WATER_SWIM      6
#define SECT_WATER_NOSWIM    7

struct room_direction_data {
   char	*general_description;       /* When look DIR.                  */

   char	*keyword;                   /* for open/close                  */

   sh_int exit_info;                /* Exit info                       */
   sh_int key;		            /* Key's number (-1 for no key)    */
   sh_int to_room;                  /* Where direction leeds (NOWHERE) */
};

/* ========================= Structure for room ========================== */
struct room_data {
   sh_int number;               /* Rooms number                       */
   sh_int zone;                 /* Room zone (for resetting)          */
   int	sector_type;            /* sector type (move/hide)            */
   char	*name;                  /* Rooms name 'You are ...'           */
   char	*description;           /* Shown when entered                 */
   struct extra_descr_data *ex_description; /* for examine/look       */
   struct room_direction_data *dir_option[NUM_OF_DIRS]; /* Directions */
   sh_int room_flags;           /* DEATH,DARK ... etc                 */

   byte light;                  /* Number of lightsources in room     */
   int	(*funct)();             /* special procedure                  */

   struct obj_data *contents;   /* List of items in room              */
   struct char_data *people;    /* List of NPC / PC in room           */
};
/* ======================================================================== */

/* The following defs and structures are related to char_data   */

/* For 'equipment' */

#define WEAR_LIGHT      0
#define WEAR_FINGER_R   1
#define WEAR_FINGER_L   2
#define WEAR_NECK_1     3
#define WEAR_NECK_2     4
#define WEAR_BODY       5
#define WEAR_HEAD       6
#define WEAR_LEGS       7
#define WEAR_FEET       8
#define WEAR_HANDS      9
#define WEAR_ARMS      10
#define WEAR_SHIELD    11
#define WEAR_ABOUT     12
#define WEAR_WAISTE    13
#define WEAR_WRIST_R   14
#define WEAR_WRIST_L   15
#define WIELD          16
#define HOLD           17


/* For 'char_payer_data' */

#define MAX_TOUNGE  3     /* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
#define MAX_SKILLS  128   /* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
#define MAX_WEAR    18
#define MAX_AFFECT  32    /* Used in CHAR_FILE_U *DO*NOT*CHANGE* */

/* Predifined  conditions */
#define DRUNK        0
#define FULL         1
#define THIRST       2

/* Bitvector for 'affected_by' */
#define AFF_BLIND             (1 << 0)
#define AFF_INVISIBLE         (1 << 1)
#define AFF_DETECT_EVIL       (1 << 2)
#define AFF_DETECT_INVISIBLE  (1 << 3)
#define AFF_DETECT_MAGIC      (1 << 4)
#define AFF_SENSE_LIFE        (1 << 5)
#define AFF_HOLD              (1 << 6)
#define AFF_SANCTUARY         (1 << 7)

#define AFF_GROUP             (1 << 8)
#define AFF_CURSE             (1 << 9)
#define AFF_FLAMING           (1 << 10)
#define AFF_POISON            (1 << 11)
#define AFF_PROTECT_EVIL      (1 << 12)
#define AFF_PARALYSIS         (1 << 13)
#define AFF_MORDEN_SWORD      (1 << 14)
#define AFF_FLAMING_SWORD     (1 << 15)

#define AFF_SLEEP             (1 << 16)
#define AFF_DODGE             (1 << 17)
#define AFF_SNEAK             (1 << 18)
#define AFF_HIDE              (1 << 19)
#define AFF_FEAR              (1 << 20)
#define AFF_CHARM             (1 << 21)
#define AFF_FOLLOW            (1 << 22)
#define AFF_WIMPY	      (1 << 23)
#define AFF_INFRARED          (1 << 24)

/* modifiers to char's abilities */

#define APPLY_NONE              0
#define APPLY_STR               1
#define APPLY_DEX               2
#define APPLY_INT               3
#define APPLY_WIS               4
#define APPLY_CON               5
#define APPLY_SEX               6
#define APPLY_CLASS             7
#define APPLY_LEVEL             8
#define APPLY_AGE               9
#define APPLY_CHAR_WEIGHT      10
#define APPLY_CHAR_HEIGHT      11
#define APPLY_MANA             12
#define APPLY_HIT              13
#define APPLY_MOVE             14
#define APPLY_GOLD             15
#define APPLY_EXP              16
#define APPLY_AC               17
#define APPLY_ARMOR            17
#define APPLY_HITROLL          18
#define APPLY_DAMROLL          19
#define APPLY_SAVING_PARA      20
#define APPLY_SAVING_ROD       21
#define APPLY_SAVING_PETRI     22
#define APPLY_SAVING_BREATH    23
#define APPLY_SAVING_SPELL     24


/* 'class' for PC's */
#define CLASS_MAGIC_USER  1
#define CLASS_CLERIC      2
#define CLASS_THIEF       3
#define CLASS_WARRIOR     4

/* 'class' for NPC's */
#define CLASS_OTHER       0 /* These are not yet used!   */
#define CLASS_UNDEAD      1 /* But may soon be so        */
#define CLASS_HUMANOID    2 /* Idea is to use general    */
#define CLASS_ANIMAL      3 /* monster classes           */
#define CLASS_DRAGON      4 /* Then for example a weapon */
#define CLASS_GIANT       5 /* of dragon slaying is pos. */

/* sex */
#define SEX_NEUTRAL   0
#define SEX_MALE      1
#define SEX_FEMALE    2

/* positions */
#define POSITION_DEAD       0
#define POSITION_MORTALLYW  1
#define POSITION_INCAP      2
#define POSITION_STUNNED    3
#define POSITION_SLEEPING   4
#define POSITION_RESTING    5
#define POSITION_SITTING    6
#define POSITION_FIGHTING   7
#define POSITION_STANDING   8


/* for mobile actions: specials.act */
#define MOB_SPEC         (1 << 0)  /* spec-proc to be called if exist   */
#define MOB_SENTINEL     (1 << 1)  /* this mobile not to be moved       */
#define MOB_SCAVENGER    (1 << 2)  /* pick up stuff lying around        */
#define MOB_ISNPC        (1 << 3)  /* This bit is set for use with IS_NPC()*/
#define MOB_NICE_THIEF   (1 << 4)  /* Set if a thief should NOT be killed  */
#define MOB_AGGRESSIVE   (1 << 5)  /* Set if automatic attack on NPC's     */
#define MOB_STAY_ZONE    (1 << 6)  /* MOB Must stay inside its own zone    */
#define MOB_WIMPY        (1 << 7)  /* MOB Will flee when injured, and if   */
/* aggressive only attack sleeping players */

/*
 * For MOB_AGGRESSIVE_XXX, you must also set MOB_AGGRESSIVE.
 * These switches can be combined, if none are selected, then
 * the mobile will attack any alignment (same as if all 3 were set)
 */
#define MOB_AGGRESSIVE_EVIL	(1 << 8) /* auto attack evil PC's only	*/
#define MOB_AGGRESSIVE_GOOD	(1 << 9) /* auto attack good PC's only	*/
#define MOB_AGGRESSIVE_NEUTRAL	(1 << 10) /* auto attack neutral PC's only	*/
#define MOB_MEMORY		(1 << 11) /* remember attackers if struck first */
#define MOB_HELPER		(1 << 12) /* attack chars attacking a PC in room */

/* For players : specials.act */
#define PLR_KILLER	(1 << 0)
#define PLR_THIEF	(1 << 1)
#define PLR_FROZEN	(1 << 2)
#define PLR_DONTSET     (1 << 3)   /* Dont EVER set (ISNPC bit) */
#define PLR_WRITING	(1 << 4)
#define PLR_MAILING	(1 << 5)
#define PLR_CRASH	(1 << 6)
#define PLR_SITEOK	(1 << 7)
#define PLR_NOSHOUT	(1 << 8)
#define PLR_NOTITLE	(1 << 9)
#define PLR_DELETED	(1 << 10)
#define PLR_LOADROOM	(1 << 11)
#define PLR_NOWIZLIST	(1 << 12)
#define PLR_NODELETE	(1 << 13)
#define PLR_INVSTART	(1 << 14)
#define PLR_CRYO	(1 << 15)

/* for players: preference bits */
#define PRF_BRIEF       (1 << 0)
#define PRF_COMPACT     (1 << 1)
#define PRF_DEAF	(1 << 2)
#define PRF_NOTELL      (1 << 3)
#define PRF_DISPHP	(1 << 4)
#define PRF_DISPMANA	(1 << 5)
#define PRF_DISPMOVE	(1 << 6)
#define PRF_DISPAUTO	(1 << 7)
#define PRF_NOHASSLE	(1 << 8)
#define PRF_QUEST	(1 << 9)
#define PRF_SUMMONABLE	(1 << 10)
#define PRF_NOREPEAT	(1 << 11)
#define PRF_HOLYLIGHT	(1 << 12)
#define PRF_COLOR_1	(1 << 13)
#define PRF_COLOR_2	(1 << 14)
#define PRF_NOWIZ	(1 << 15)
#define PRF_LOG1	(1 << 16)
#define PRF_LOG2	(1 << 17)
#define PRF_NOAUCT	(1 << 18)
#define PRF_NOGOSS	(1 << 19)
#define PRF_NOGRATZ	(1 << 20)
#define PRF_ROOMFLAGS	(1 << 21)

struct memory_rec_struct {
   long	id;
   struct memory_rec_struct *next;
};

typedef struct memory_rec_struct memory_rec;

/* This structure is purely intended to be an easy way to transfer */
/* and return information about time (real or mudwise).            */
struct time_info_data {
   byte hours, day, month;
   sh_int year;
};

/* These data contain information about a players time data */
struct time_data {
   time_t birth;    /* This represents the characters age                */
   time_t logon;    /* Time of the last logon (used to calculate played) */
   int	played;     /* This is the total accumulated time played in secs */
};

struct char_player_data {
   char	*name;	       /* PC / NPC s name (kill ...  )         */
   char	*short_descr;  /* for 'actions'                        */
   char	*long_descr;   /* for 'look'.. Only here for testing   */
   char	*description;  /* Extra descriptions                   */
   char	*title;        /* PC / NPC s title                     */
   byte sex;           /* PC / NPC s sex                       */
   byte class;         /* PC s class or NPC alignment          */
   byte level;         /* PC / NPC s level                     */
   int	hometown;      /* PC s Hometown (zone)                 */
   bool talks[MAX_TOUNGE]; /* PC s Tounges 0 for NPC           */
   struct time_data time;  /* PC s AGE in days                 */
   ubyte weight;       /* PC / NPC s weight                    */
   ubyte height;       /* PC / NPC s height                    */
};


/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
struct char_ability_data {
   sbyte str;
   sbyte str_add;      /* 000 - 100 if strength 18             */
   sbyte intel;
   sbyte wis;
   sbyte dex;
   sbyte con;
};


/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
struct char_point_data {
   sh_int mana;
   sh_int max_mana;     /* Max move for PC/NPC			   */
   sh_int hit;
   sh_int max_hit;      /* Max hit for PC/NPC                      */
   sh_int move;
   sh_int max_move;     /* Max move for PC/NPC                     */

   sh_int armor;        /* Internal -100..100, external -10..10 AC */
   int	gold;           /* Money carried                           */
   int	bank_gold;	/* Gold the char has in a bank account	   */
   int	exp;            /* The experience of the player            */

   sbyte hitroll;       /* Any bonus or penalty to the hit roll    */
   sbyte damroll;       /* Any bonus or penalty to the damage roll */
};

/* char_special_data's fields are fields which are needed while the game
   is running, but are not stored in the playerfile.  In other words,
   a struct of type char_special_data appears in char_data but NOT
   char_file_u.  If you want to add a piece of data which is automatically
   saved and loaded with the playerfile, add it to char_special2_data.
*/
struct char_special_data {
   struct char_data *fighting; /* Opponent                             */
   struct char_data *hunting;  /* Hunting person..                     */

   long	affected_by;        /* Bitvector for spells/skills affected by */

   byte position;           /* Standing or ...                         */
   byte default_pos;        /* Default position for NPC                */

   int	carry_weight;       /* Carried weight                          */
   byte carry_items;        /* Number of items carried                 */
   int	timer;              /* Timer for update                        */
   sh_int was_in_room;      /* storage of location for linkdead people */

   byte damnodice;          /* The number of damage dice's	       */
   byte damsizedice;        /* The size of the damage dice's           */
   byte last_direction;     /* The last direction the monster went     */
   int	attack_type;        /* The Attack Type Bitvector for NPC's     */

   char	*poofIn;	    /* Description on arrival of a god.	       */
   char	*poofOut; 	    /* Description upon a god's exit.	       */
   sh_int invis_level;	    /* level of invisibility		       */

   memory_rec *memory;	    /* List of attackers to remember	       */
};


struct char_special2_data {
   long	idnum;			/* player's idnum			*/
   sh_int load_room;            /* Which room to place char in		*/
   byte spells_to_learn;	/* How many can you learn yet this level*/
   int	alignment;		/* +-1000 for alignments                */
   long	act;			/* act flag for NPC's; player flag for PC's */
   long	pref;			/* preference flags for PC's.		*/
   int	wimp_level;		/* Below this # of hit points, flee!	*/
   byte freeze_level;		/* Level of god who froze char, if any	*/
   ubyte bad_pws;		/* number of bad password attemps	*/
   sh_int apply_saving_throw[5]; /* Saving throw (Bonuses)		*/
   sbyte conditions[3];         /* Drunk full etc.			*/

   /* spares below for future expansion.  You can change the names from
      'sparen' to something meaningful, but don't change the order.
   */

   ubyte spare0;
   ubyte spare1;
   ubyte spare2;
   ubyte spare3;
   ubyte spare4;
   ubyte spare5;
   ubyte spare6;
   ubyte spare7;
   ubyte spare8;
   ubyte spare9;
   ubyte spare10;
   ubyte spare11;
   long	spare12;
   long	spare13;
   long	spare14;
   long	spare15;
   long	spare16;
   long	spare17;
   long	spare18;
   long	spare19;
   long	spare20;
   long	spare21;
};



/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
struct affected_type {
   sbyte type;           /* The type of spell that caused this      */
   sh_int duration;      /* For how long its effects will last      */
   sbyte modifier;       /* This is added to apropriate ability     */
   byte location;        /* Tells which ability to change(APPLY_XXX)*/
   long	bitvector;       /* Tells which bits to set (AFF_XXX)       */

   struct affected_type *next;
};

struct follow_type {
   struct char_data *follower;
   struct follow_type *next;
};

/* ================== Structure for player/non-player ===================== */
struct char_data {
   sh_int nr;                            /* monster nr (pos in file)      */
   sh_int in_room;                       /* Location                      */

   struct char_player_data player;       /* Normal data                   */
   struct char_ability_data abilities;   /* Abilities                     */
   struct char_ability_data tmpabilities;/* The abilities we will use     */
   struct char_point_data points;        /* Points                        */
   struct char_special_data specials;    /* Special plaing constants      */
   struct char_special2_data specials2;  /* Additional special constants  */
   byte *skills;			 /* dynam. alloc. array of skills */

   struct affected_type *affected;       /* affected by what spells       */
   struct obj_data *equipment[MAX_WEAR]; /* Equipment array               */

   struct obj_data *carrying;            /* Head of list                  */
   struct descriptor_data *desc;         /* NULL for mobiles              */

   struct char_data *next_in_room;     /* For room->people - list         */
   struct char_data *next;             /* For either monster or ppl-list  */
   struct char_data *next_fighting;    /* For fighting list               */

   struct follow_type *followers;        /* List of chars followers       */
   struct char_data *master;             /* Who is char following?        */
};


/* ======================================================================== */

/* How much light is in the land ? */

#define SUN_DARK	0
#define SUN_RISE	1
#define SUN_LIGHT	2
#define SUN_SET		3

/* And how is the sky ? */

#define SKY_CLOUDLESS	0
#define SKY_CLOUDY	1
#define SKY_RAINING	2
#define SKY_LIGHTNING	3

struct weather_data {
   int	pressure;	/* How is the pressure ( Mb ) */
   int	change;	/* How fast and what way does it change. */
   int	sky;	/* How is the sky. */
   int	sunlight;	/* And how much sun. */
};


/* ***********************************************************************
*  file element for player file. BEWARE: Changing it will ruin the file  *
*********************************************************************** */


struct char_file_u {
   byte sex;
   byte class;
   byte level;
   time_t birth;  /* Time of birth of character     */
   int	played;    /* Number of secs played in total */

   ubyte weight;
   ubyte height;

   char	title[80];
   sh_int hometown;
   char	description[240];
   bool talks[MAX_TOUNGE];


   struct char_ability_data abilities;

   struct char_point_data points;

   byte skills[MAX_SKILLS];

   struct affected_type affected[MAX_AFFECT];

   struct char_special2_data specials2;

   time_t last_logon;		/* Time (in secs) of last logon */
   char host[HOST_LEN+1];	/* host of last logon */

   /* char data */
   char	name[20];
   char	pwd[MAX_PWD_LENGTH+1];
};



/* ************************************************************************
*  file element for object file. BEWARE: Changing it will ruin rent files *
************************************************************************ */

struct obj_file_elem {
   sh_int item_number;

   int	value[4];
   int	extra_flags;
   int	weight;
   int	timer;
   long	bitvector;
   struct obj_affected_type affected[MAX_OBJ_AFFECT];
};


#define RENT_UNDEF      0
#define RENT_CRASH      1
#define RENT_RENTED     2
#define RENT_CRYO       3
#define RENT_FORCED     4
#define RENT_TIMEDOUT   5

/* header block for rent files */
struct rent_info {
   int	time;
   int	rentcode;
   int	net_cost_per_diem;
   int	gold;
   int	account;
   int	nitems;
   int	spare0;
   int	spare1;
   int	spare2;
   int	spare3;
   int	spare4;
   int	spare5;
   int	spare6;
   int	spare7;
};


/* ***********************************************************
*  The following structures are related to descriptor_data   *
*********************************************************** */



struct txt_block {
   char	*text;
   struct txt_block *next;
};

struct txt_q {
   struct txt_block *head;
   struct txt_block *tail;
};



/* modes of connectedness */

#define CON_PLYNG   0
#define CON_NME     1
#define CON_NMECNF  2
#define CON_PWDNRM  3
#define CON_PWDGET  4
#define CON_PWDCNF  5
#define CON_QSEX    6
#define CON_RMOTD   7
#define CON_SLCT    8
#define CON_EXDSCR  9
#define CON_QCLASS  10
#define CON_LDEAD   11
#define CON_PWDNQO  12
#define CON_PWDNEW  13
#define CON_PWDNCNF 14
#define CON_CLOSE   15
#define CON_DELCNF1 16
#define CON_DELCNF2 17

struct snoop_data {
   struct char_data *snooping;	/* Who is this char snooping		*/
   struct char_data *snoop_by;	/* And who is snooping this char	*/
};

struct descriptor_data {
   int	descriptor;		/* file descriptor for socket		*/
   char	*name;			/* ptr to name for mail system		*/
   char	host[50];		/* hostname				*/
   char	pwd[MAX_PWD_LENGTH+1];	/* password				*/
   byte	bad_pws;		/* number of bad pw attemps this login	*/
   int	pos;			/* position in player-file		*/
   int	connected;		/* mode of 'connectedness'		*/
   int	wait;			/* wait for how many loops		*/
   int	desc_num;		/* unique num assigned to desc		*/
   long	login_time;		/* when the person connected		*/
   char	*showstr_head;		/* for paging through texts		*/
   char	*showstr_point;		/*		-			*/
   char	**str;			/* for the modify-str system		*/
   int	max_str;		/*		-			*/
   int	prompt_mode;		/* control of prompt-printing		*/
   char	buf[MAX_STRING_LENGTH];	/* buffer for raw input			*/
   char	last_input[MAX_INPUT_LENGTH];/* the last input			*/
   char small_outbuf[SMALL_BUFSIZE]; /* standard output bufer		*/
   char *output;		/* ptr to the current output buffer	*/
   int  bufptr;			/* ptr to end of current output		*/
   int	bufspace;		/* space left in the output buffer	*/
   struct txt_block *large_outbuf; /* ptr to large buffer, if we need it */
   struct txt_q input;		/* q of unprocessed input		*/
   struct char_data *character;	/* linked to char			*/
   struct char_data *original;	/* original char if switched		*/
   struct snoop_data snoop;	/* to snoop people			*/
   struct descriptor_data *next; /* link to next descriptor		*/
};

struct msg_type {
   char	*attacker_msg;  /* message to attacker */
   char	*victim_msg;    /* message to victim   */
   char	*room_msg;      /* message to room     */
};

struct message_type {
   struct msg_type die_msg;      /* messages when death			*/
   struct msg_type miss_msg;     /* messages when miss			*/
   struct msg_type hit_msg;      /* messages when hit			*/
   struct msg_type sanctuary_msg;/* messages when hit on sanctuary	*/
   struct msg_type god_msg;      /* messages when hit on god		*/
   struct message_type *next;	/* to next messages of this kind.	*/
};

struct message_list {
   int	a_type;			/* Attack type				*/
   int	number_of_attacks;	/* How many attack messages to chose from. */
   struct message_type *msg;	/* List of messages.			*/
};

struct dex_skill_type {
   sh_int p_pocket;
   sh_int p_locks;
   sh_int traps;
   sh_int sneak;
   sh_int hide;
};

struct dex_app_type {
   sh_int reaction;
   sh_int miss_att;
   sh_int defensive;
};

struct str_app_type {
   sh_int tohit;    /* To Hit (THAC0) Bonus/Penalty        */
   sh_int todam;    /* Damage Bonus/Penalty                */
   sh_int carry_w;  /* Maximum weight that can be carrried */
   sh_int wield_w;  /* Maximum weight that can be wielded  */
};

struct wis_app_type {
   byte bonus;       /* how many bonus skills a player can */
		     /* practice pr. level                 */
};

struct int_app_type {
   byte learn;       /* how many % a player learns a spell/skill */
};

struct con_app_type {
   sh_int hitp;
   sh_int shock;
};


