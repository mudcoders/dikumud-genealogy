/* ************************************************************************
*   File: db.h                                          Part of CircleMUD *
*  Usage: header file for database handling                               *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/* arbitrary constants used by index_boot() (must be unique) */
#define DB_BOOT_WLD	0
#define DB_BOOT_MOB	1
#define DB_BOOT_OBJ	2
#define DB_BOOT_ZON	3
#define DB_BOOT_SHP	4

/* names of various files and directories */

#define INDEX_FILE	"index"		/* index of world files		*/
#define MINDEX_FILE	"index.mini"	/* ... and for mini-mud-mode	*/
#define WLD_PREFIX	"world/wld"	/* room definitions		*/
#define MOB_PREFIX	"world/mob"	/* monster prototypes		*/
#define OBJ_PREFIX	"world/obj"	/* object prototypes		*/
#define ZON_PREFIX	"world/zon"	/* zon defs & command tables	*/
#define SHP_PREFIX	"world/shp"	/* shop definitions		*/

#define CREDITS_FILE	"text/credits"	/* for the 'credits' command	*/
#define NEWS_FILE	"text/news"	/* for the 'news' command	*/
#define MOTD_FILE	"text/motd"	/* messages of the day / mortal	*/
#define IMOTD_FILE	"text/imotd"	/* messages of the day / immort	*/
#define IDEA_FILE	"text/ideas"	/* for the 'idea'-command	*/
#define TYPO_FILE	"text/typos"	/*         'typo'		*/
#define BUG_FILE	"text/bugs"	/*         'bug'		*/
#define HELP_KWRD_FILE	"text/help_table"/* for HELP <keywrd>		*/
#define HELP_PAGE_FILE	"text/help"	/* for HELP <CR>		*/
#define INFO_FILE	"text/info"	/* for INFO			*/
#define WIZLIST_FILE	"text/wizlist"	/* for WIZLIST			*/
#define IMMLIST_FILE	"text/immlist"	/* for IMMLIST			*/
#define BACKGROUND_FILE	"text/background" /* for the background story	*/
#define POLICIES_FILE	"text/policies"	/* player policies/rules	*/
#define HANDBOOK_FILE	"text/handbook"	/* handbook for new immorts	*/

#define PLAYER_FILE	"misc/players"	/* the player database		*/
#define MESS_FILE	"misc/messages"	/* damage message		*/
#define SOCMESS_FILE	"misc/socials"	/* messgs for social acts	*/
#define MAIL_FILE	"misc/plrmail"	/* for the mudmail system	*/
#define BAN_FILE	"misc/badsites"	/* for the siteban system	*/
#define XNAME_FILE	"misc/xnames"	/* invalid name substrings	*/

/* public procedures in db.c */

void	boot_db(void);
void	char_to_store(struct char_data *ch, struct char_file_u *st);
void	store_to_char(struct char_file_u *st, struct char_data *ch);
int	load_char(char *name, struct char_file_u *char_element);
void	save_char(struct char_data *ch, sh_int load_room);
int	create_entry(char *name);
void	zone_update(void);
void	init_char(struct char_data *ch);
void	clear_char(struct char_data *ch);
void	clear_object(struct obj_data *obj);
void	reset_char(struct char_data *ch);
void	free_char(struct char_data *ch);
void	free_obj(struct obj_data *obj);
int	real_room(int virtual);
char	*fread_string(FILE *fl, char *error);
int	real_object(int virtual);
int	real_mobile(int virtual);
int	vnum_mobile(char *searchname, struct char_data *ch);
int	vnum_object(char *searchname, struct char_data *ch);


#define REAL 0
#define VIRTUAL 1

struct obj_data *read_object(int nr, int type);
struct char_data *read_mobile(int nr, int type);


/* structure for the reset commands */
struct reset_com {
   char	command;   /* current command                      */

   bool if_flag;   /* if TRUE: exe only if preceding exe'd */
   int	arg1;       /*                                      */
   int	arg2;       /* Arguments to the command             */
   int	arg3;       /*                                      */

   /* 
	*  Commands:              *
	*  'M': Read a mobile     *
	*  'O': Read an object    *
	*  'G': Give obj to mob   *
	*  'P': Put obj in obj    *
	*  'G': Obj to char       *
	*  'E': Obj to char equip *
	*  'D': Set state of door *
   */
};



/* zone definition structure. for the 'zone-table'   */
struct zone_data {
   char	*name;		    /* name of this zone                  */
   int	lifespan;           /* how long between resets (minutes)  */
   int	age;                /* current age of this zone (minutes) */
   int	top;                /* upper limit for rooms in this zone */

   int	reset_mode;         /* conditions for reset (see below)   */
   int	number;		    /* virtual number of this zone	  */
   struct reset_com *cmd;   /* command table for reset	          */

   /*
	*  Reset mode:                              *
	*  0: Don't reset, and don't update age.    *
	*  1: Reset if no PC's are located in zone. *
	*  2: Just reset.                           *
   */
};




/* element in monster and object index-tables   */
struct index_data {
   int	virtual;    /* virtual number of this mob/obj           */
   int	number;     /* number of existing units of this mob/obj	*/
   int	(*func)();  /* special procedure for this mob/obj       */
};




/* for queueing zones for update   */
struct reset_q_element {
   int	zone_to_reset;            /* ref to zone_data */
   struct reset_q_element *next;
};



/* structure for the update queue     */
struct reset_q_type {
   struct reset_q_element *head;
   struct reset_q_element *tail;
} reset_q;



struct player_index_element {
   char	*name;
};


struct help_index_element {
   char	*keyword;
   long	pos;
};

/* don't change these */
#define BAN_NOT 	0
#define BAN_NEW 	1
#define BAN_SELECT	2
#define BAN_ALL		3

#define BANNED_SITE_LENGTH    50
struct ban_list_element {
   char	site[BANNED_SITE_LENGTH+1];
   int	type;
   long	date;
   char	name[MAX_NAME_LENGTH+1];
   struct ban_list_element *next;
};


/* global buffering system */

#ifdef __DB_C__
char	buf[MAX_STRING_LENGTH];
char	buf1[MAX_STRING_LENGTH];
char	buf2[MAX_STRING_LENGTH];
char	arg[MAX_STRING_LENGTH];
#else
extern char	buf[MAX_STRING_LENGTH];
extern char	buf1[MAX_STRING_LENGTH];
extern char	buf2[MAX_STRING_LENGTH];
extern char	arg[MAX_STRING_LENGTH];
#endif

