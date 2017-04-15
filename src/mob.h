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



/* For 'char_player_data' */

#define MAX_TONGUE  3     /* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
#define MAX_SKILLS  400   /* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
#define MAX_WEAR    18
#define MAX_AFFECT  25    /* Used in CHAR_FILE_U *DO*NOT*CHANGE* */

#define DRUNK        0
#define FULL         1
#define THIRST       2

/* Bitvector for 'affected_by' */
#define AFF_BLIND             1
#define AFF_INVISIBLE         2
#define AFF_DETECT_EVIL       4
#define AFF_DETECT_INVISIBLE  8
#define AFF_DETECT_MAGIC      16
#define AFF_SENSE_LIFE        32
#define AFF_HOLD              64
#define AFF_SANCTUARY         128
#define AFF_GROUP             256
#define AFF_CURSE             1024
#define AFF_FLAMING           2048
#define AFF_POISON            4096
#define AFF_PROTECT_EVIL      8192
#define AFF_PARALYSIS         16384
#define AFF_MORDEN_SWORD      32768
#define AFF_FLAMING_SWORD     65536
#define AFF_SLEEP             131072
#define AFF_DODGE             262144
#define AFF_SNEAK             524288
#define AFF_HIDE              1048576
#define AFF_FEAR              2097152
#define AFF_CHARM             4194304
#define AFF_FOLLOW            8388608
#define AFF_WIMPY            16777216
#define AFF_INFRARED         33554432
#define AFF_THIEF            67108864
#define AFF_KILLER          134217728
#define AFF_FLYING          268435456
#define AFF_PLAGUE          536870912

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
#define ACT_SPEC         1     /* special routine to be called if exist   */
#define ACT_SENTINEL     2     /* this mobile not to be moved             */
#define ACT_SCAVENGER    4     /* pick up stuff lying around              */
#define ACT_ISNPC        8     /* This bit is set for use with IS_NPC()   */
#define ACT_NICE_THIEF  16     /* Set if a thief should NOT be killed     */
#define ACT_AGGRESSIVE  32     /* Set if automatic attack on NPC's        */
#define ACT_STAY_ZONE   64     /* MOB Must stay inside its own zone       */
#define ACT_WIMPY      128     /* MOB Will flee when injured, and if      */
			       /* aggressive only attack sleeping players */
#define ACT_2ND_ATTACK 256     /* Set this mobile to have 2nd attack      */
#define ACT_3RD_ATTACK 512     /* Set this mobile to have 3rd attack      */
#define ACT_4TH_ATTACK 1024    /* Set this mobile to have 4th attack      */
			       /* Each attack bit must be set to get up   */
			       /* 4 attacks                               */
/*
 * For ACT_AGGRESSIVE_XXX, you must also set ACT_AGGRESSIVE
 * These switches can be combined, if none are selected, then
 * the mobile will attack any alignment (same as if all 3 were set)
 */
#define ACT_AGGR_EVIL       2048    /* Auto attack evil PC's    */
#define ACT_AGGR_GOOD       4096    /* Auto attack good PC's    */
#define ACT_AGGR_NEUT       8192    /* Auto attack neutral PC's */
#define ACT_AGGR_ALL        (ACT_AGGR_EVIL|ACT_AGGR_GOOD|ACT_AGGR_NEUT)
#define ACT_UNDEAD             16384    /* The undead flag              */
    
/* For players : specials.act */
#define PLR_BRIEF        1
#define PLR_NOSHOUT      2
#define PLR_COMPACT      4
#define PLR_DONTSET      8 /* Dont EVER set */
#define PLR_NOTELL      16
#define PLR_NOEMOTE     32
#define PLR_LOG         64 /* log activities of this player. */
#define PLR_FREEZE     128 /* No commands available */



struct char_player_data
{
    char *name;         /* PC / NPC s name (kill ...  )         */
    char *short_descr;  /* for 'actions'                        */
    char *long_descr;   /* for 'look'.. Only here for testing   */
    char *description;  /* Extra descriptions                   */
    char *title;        /* PC / NPC s title                     */
    byte sex;           /* PC / NPC s sex                       */
    byte class;         /* PC s class or NPC alignment          */
    byte level;         /* PC / NPC s level                     */
    int hometown;       /* PC s Hometown (zone)                 */
    bool talks[MAX_TONGUE]; /* PC s Tounges 0 for NPC           */
    struct time_data time; /* PC s AGE in days                  */
    struct time_data tmptime; /* PC s AGE in days, modified     */
    ubyte weight;       /* PC / NPC s weight                    */
    ubyte height;       /* PC / NPC s height                    */
};


/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
struct char_ability_data
{
    sbyte str; 
    sbyte unused;
    sbyte intel;
    sbyte wis; 
    sbyte dex; 
    sbyte con; 
};


/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
struct char_point_data
{
    sh_int mana;         
    sh_int max_mana;     /* Not useable                             */
    sh_int hit;   
    sh_int max_hit;      /* Max hit for NPC                         */
    sh_int move;  
    sh_int max_move;     /* Max move for NPC                        */

    sh_int armor;        /* Internal -100..100, external -10..10 AC */
    int gold;            /* Money carried                           */
    int exp;             /* The experience of the player            */

    sbyte hitroll;       /* Any bonus or penalty to the hit roll    */
    sbyte damroll;       /* Any bonus or penalty to the damage roll */
};


struct char_special_data
{
    CHAR_DATA *fighting;     /* Opponent                                */
    CHAR_DATA *hunting;      /* Hunting person.                         */

    long affected_by;        /* Bitvector for spells/skills affected by */ 

    byte position;           /* Standing or ...                         */
    byte default_pos;        /* Default position for NPC                */
    ubyte act;               /* flags for NPC behavior                  */

    ubyte practices;         /* How many can you learn yet this level   */

    int carry_weight;        /* Carried weight                          */
    byte carry_items;        /* Number of items carried                 */
    int timer;               /* Timer for update                        */
    sh_int was_in_room;      /* storage of location for linkdead people */
    sh_int apply_saving_throw[5]; /* Saving throw (Bonuses)             */
    sbyte conditions[3];      /* Drunk full etc.                        */

    byte damnodice;           /* The number of damage dice's            */
    byte damsizedice;         /* The size of the damage dice's          */
    byte last_direction;      /* The last direction the monster went    */
    int attack_type;          /* The Attack Type Bitvector for NPC's    */
    int alignment;            /* +-1000 for alignments                  */
    bool holyLite;            /* Holy lite mode */
    bool wizInvis;            /* If on, people can't see you. */
    bool unused;
};


/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
struct char_skill_data
{
    byte learned;           /* % chance for success 0 = not learned   */
    bool recognise;         /* If you can recognise the scroll etc.   */
};



/* Used in CHAR_FILE_U *DO*NOT*CHANGE* */
struct affected_type
{
    sbyte type;           /* The type of spell that caused this      */
    sh_int duration;      /* For how long its effects will last      */
    sbyte modifier;       /* This is added to apropriate ability     */
    byte location;        /* Tells which ability to change(APPLY_XXX)*/
    long bitvector;       /* Tells which bits to set (AFF_XXX)       */

    struct affected_type *next;
};



struct follow_type
{
    struct char_data *follower;
    struct follow_type *next;
};




struct char_data
{
  sh_int nr;                            /* monster nr (pos in file)      */
  sh_int in_room;                       /* Location                      */
  char   pwd[11];                       /* Password                      */
  
  struct char_player_data player;       /* Normal data                   */
  struct char_ability_data abilities;   /* Abilities                     */
  struct char_ability_data tmpabilities;/* The abilities we will use     */
  struct char_point_data points;        /* Points                        */
  struct char_special_data specials;    /* Special plaing constants      */
  struct char_skill_data skills[MAX_SKILLS]; /* Skills                   */
  
  struct affected_type *affected;       /* affected by what spells       */
  struct obj_data *equipment[MAX_WEAR]; /* Equipment array               */
  
  struct obj_data *carrying;            /* Head of list                  */
  struct descriptor_data *desc;         /* NULL for mobiles              */
  
  CHAR_DATA *next;                      /* Next anywhere in game         */
  CHAR_DATA *next_in_room;              /* Next in room                  */
  CHAR_DATA *next_fighting;             /* Next fighting                 */
  
  struct follow_type *followers;        /* List of chars followers       */
  struct char_data *master;             /* Who is char following?        */
};




/*
 * File element for player file.
 * BEWARE: changing it will ruin the file.
 */
struct char_file_u
{
    int version;
    byte sex;
    byte class;
    byte level;
    time_t birth;  /* Time of birth of character     */
    int played;    /* Number of secs played in total */

    ubyte weight;
    ubyte height;

    char title[80];
    sh_int hometown;
    char description[240];
    bool talks[MAX_TONGUE];

    sh_int load_room;            /* Which room to place char in           */

    struct char_ability_data abilities;

    struct char_point_data points;

    struct char_skill_data skills[MAX_SKILLS];

    struct affected_type affected[MAX_AFFECT];

    /* specials */

    ubyte practices;
    int alignment;     

    time_t last_logon;  /* Time (in secs) of last logon */
    ubyte act;          /* ACT Flags                    */

    /* char data */
    char name[20];
    char pwd[11];
    sh_int apply_saving_throw[5];
    int conditions[3];
    int unused[100];
};
