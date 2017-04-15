/***************************************************************************
 *  file: db.h , Database module.                          Part of DIKUMUD *
 *  Usage: Loading/Saving chars booting world.                             *
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


/* data files used by the game system */

#define DFLT_DIR          "../lib"        /* default data directory     */
#define SAVE_DIR          "../save"       /* save directory             */

#define WORLD_FILE        "tinyworld.wld" /* room definitions           */
#define MOB_FILE          "tinyworld.mob" /* monster prototypes         */
#define OBJ_FILE          "tinyworld.obj" /* object prototypes          */
#define ZONE_FILE         "tinyworld.zon" /* zone defs & command tables */
#define SHOP_FILE         "tinyworld.shp" /* shop messages and markups  */
#define GREETINGS_FILE    "greetings.txt" /* initial greetings screen   */
#define CREDITS_FILE      "credits.txt"   /* for the 'credits' command  */
#define NEWS_FILE         "news.txt"      /* for the 'news' command     */
#define MOTD_FILE         "motd.txt"      /* messages of today          */
#define IMM_MOTD_FILE     "imotd.txt"     /* news for immortals         */
#define STORY_FILE        "story.txt"     /* game story                 */
#define TIME_FILE         "time.txt"      /* game calendar information  */
#define IDEA_FILE         "ideas.txt"     /* for the 'idea'-command     */
#define TYPO_FILE         "typos.txt"     /*         'typo'             */
#define BUG_FILE          "bugs.txt"      /*         'bug'              */
#define MESS_FILE         "messages.txt"  /* damage message             */
#define SOCIAL_FILE       "social.txt"    /* messgs for social acts     */
#define HELP_KWRD_FILE    "help_key.txt"  /* for HELP <keywrd>          */
#define HELP_PAGE_FILE    "help.txt"      /* for HELP <CR>              */
#define INFO_FILE         "info.txt"      /* for INFO                   */
#define WIZLIST_FILE      "wizlist.txt"   /* for WIZLIST                */
#define POSEMESS_FILE     "poses.txt"     /* for 'pose'-command         */
#define MOBS_NUMBER	  "mob.list"	  /* for do_number		*/
#define OBJS_NUMBER	  "obj.list"	  /* for do_number		*/ 

/* public procedures in db.c */

void boot_db(void);
int create_entry(char *name);
void zone_update(void);
void init_char(struct char_data *ch);
void clear_char(struct char_data *ch);
void clear_object(struct obj_data *obj);
void reset_char(struct char_data *ch);
void free_char(struct char_data *ch);
int real_room(int virtual);
char *fread_string(FILE *fl);
int real_object(int virtual);
int real_mobile(int virtual);

#define REAL 0
#define VIRTUAL 1

struct obj_data *read_object(int nr, int eq_level);
struct char_data *read_mobile(int nr, int type);




/* structure for the reset commands */
struct reset_com
{
    char command;   /* current command                      */ 
    bool if_flag;   /* if TRUE: exe only if preceding exe'd */
    int arg1;       /*                                      */
    int arg2;       /* Arguments to the command             */
    int arg3;       /*                                      */

    /* 
    *  Commands:              *
    *  'M': Read a mobile     *
    *  'O': Read an object    *
    *  'P': Put obj in obj    *
    *  'G': Obj to char       *
    *  'E': Obj to char equip *
    *  'D': Set state of door *
    */
};



/* zone definition structure. for the 'zone-table'   */
struct zone_data
{
    char *name;             /* name of this zone                  */
    int lifespan;           /* how long between resets (minutes)  */
    int age;                /* current age of this zone (minutes) */
    int empty_age;	    /* age without characters             */
    int top;                /* upper limit for rooms in this zone */

    int reset_mode;         /* conditions for reset (see below)   */
    struct reset_com *cmd;  /* command table for reset             */

    /*
    *  Reset mode:                              *
    *  0: Don't reset, and don't update age.    *
    *  1: Reset if no PC's are located in zone. *
    *  2: Just reset.                           *
    */
};




/* element in monster and object index-tables   */
struct index_data
{
    int virtual;    /* virtual number of this mob/obj           */
    long pos;       /* file position of this field              */
    int number;     /* number of existing units of this mob/obj */
    int (*func)();  /* special procedure for this mob/obj       */
};




struct help_index_element
{
    char *keyword;
    long pos;
};

extern const int exp_table[36+1];
extern const char *title_table[4][36][2];
