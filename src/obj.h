/***************************************************************************
 *  file: obj.h , Structures                               Part of DIKUMUD *
 *  Usage: Declarations of object data structures                          *
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

#define ITEM_GLOW            1
#define ITEM_HUM             2
#define ITEM_DARK            4
#define ITEM_LOCK            8
#define ITEM_EVIL           16
#define ITEM_INVISIBLE      32
#define ITEM_MAGIC          64
#define ITEM_NODROP        128
#define ITEM_BLESS         256
#define ITEM_ANTI_GOOD     512 /* not usable by good people    */
#define ITEM_ANTI_EVIL    1024 /* not usable by evil people    */
#define ITEM_ANTI_NEUTRAL 2048 /* not usable by neutral people */
#define ITEM_ANTI_WARRIOR 4096 /* not usable by warriors       */
#define ITEM_ANTI_MAGE    8192 /* not usable by mages          */
#define ITEM_ANTI_THIEF  16384 /* not usable by thieves        */
#define ITEM_ANTI_CLERIC 32768 /* not usable by clerics        */
#define ITEM_ANTI_LOW    65536 /* not usable by low-levelers   */

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
#define LIQ_COKE       15

/* for containers  - value[1] */

#define CONT_CLOSEABLE      1
#define CONT_PICKPROOF      2
#define CONT_CLOSED         4
#define CONT_LOCKED         8

struct extra_descr_data
{
    char *keyword;                 /* Keyword in look/examine          */
    char *description;             /* What to see                      */
    struct extra_descr_data *next; /* Next in list                     */
};

#define MAX_OBJ_AFFECT 2         /* Used in OBJ_FILE_ELEM *DO*NOT*CHANGE* */
#define OBJ_NOTIMER    -7000000

struct obj_flag_data
{
    int value[4];       /* Values of the item (see list)    */
    byte type_flag;     /* Type of item                     */
    int wear_flags;     /* Where you can wear it            */
    int extra_flags;    /* If it hums, glows etc            */
    int weight;         /* Weight what else                 */
    int cost;           /* Value when sold (gp.)            */
    int cost_per_day;   /* Cost to keep pr. real day        */
    int eq_level;	/* Min level to use it for eq       */
    int timer;          /* Timer for object                 */
    long bitvector;     /* To set chars bits                */
};

/* Used in OBJ_FILE_ELEM *DO*NOT*CHANGE* */
struct obj_affected_type
{
    byte location;      /* Which ability to change (APPLY_XXX) */
    sbyte modifier;     /* How much it changes by              */
};

/* ======================== Structure for object ========================= */
struct obj_data
{
    sh_int item_number;            /* Where in data-base               */
    sh_int in_room;                /* In what room -1 when conta/carr  */ 
    struct obj_flag_data obj_flags;/* Object information               */
    struct obj_affected_type
	affected[MAX_OBJ_AFFECT];  /* Which abilities in PC to change  */

    char *name;                    /* Title of object :get etc.        */
    char *description ;            /* When in room                     */
    char *short_description;       /* when worn/carry/in cont.         */
    char *action_description;      /* What to write when used          */
    struct extra_descr_data *ex_description; /* extra descriptions     */
    struct char_data *carried_by;  /* Carried by :NULL in room/conta   */

    struct obj_data *in_obj;       /* In what object NULL when none    */
    struct obj_data *contains;     /* Contains objects                 */

    struct obj_data *next_content; /* For 'contains' lists             */
    struct obj_data *next;         /* For the object list              */
};

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

/* ***********************************************************************
*  file element for object file. BEWARE: Changing it will ruin the file  *
*********************************************************************** */

struct obj_file_elem 
{
    int version;
    sh_int item_number;
    int value[4];
    int extra_flags;
    sh_int wear_loc;
    sh_int weight;
    sh_int eq_level;
    sh_int timer;
    long bitvector;
    struct obj_affected_type affected[MAX_OBJ_AFFECT];
    int unused[5];
};
