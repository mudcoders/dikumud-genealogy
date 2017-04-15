/* ************************************************************************
*   File: db.c                                          Part of CircleMUD *
*  Usage: Loading/saving chars, booting/resetting world, internal funcs   *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __DB_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "db.h"
#include "comm.h"
#include "handler.h"
#include "limits.h"
#include "spells.h"
#include "mail.h"
#include "interpreter.h"

/**************************************************************************
*  declarations of most of the 'global' variables                         *
************************************************************************ */

struct room_data *world = 0;         /* array of rooms			*/
int	top_of_world = 0;            /* ref to the top element of world	*/

struct char_data *character_list = 0; /* global linked list of chars	*/
struct index_data *mob_index;		/* index table for mobile file	*/
struct char_data *mob_proto;		/* prototypes for mobs		*/
int	top_of_mobt = 0;		/* top of mobile index table	*/

struct obj_data *object_list = 0;    /* the global linked list of objs	*/
struct index_data *obj_index;		/* index table for object file	*/
struct obj_data *obj_proto;		/* prototypes for objs		*/
int	top_of_objt = 0;		/* top of object index table	*/

struct zone_data *zone_table;	     /* zone table			*/
int	top_of_zone_table = 0;	     /* ref to top element of zone tab	*/
struct message_list fight_messages[MAX_MESSAGES]; /* fighting messages	*/

struct player_index_element *player_table = 0; /* index to player file	*/
FILE *player_fl = 0;			/* file desc of player file	*/
int	top_of_p_table = 0;		/* ref to top of table		*/
int	top_of_p_file = 0;		/* ref of size of p file	*/
long	top_idnum = 0;			/* highest idnum in use		*/

int	no_mail = 0;			/* mail disabled?		*/
int	mini_mud = 0;			/* mini-mud mode?		*/
int	no_rent_check = 0;		/* skip rent check on boot?	*/
long	boot_time = 0;			/* time of mud boot		*/
int	restrict = 0;			/* level of game restriction	*/
sh_int	r_mortal_start_room;		/* rnum of mortal start room	*/
sh_int	r_immort_start_room;		/* rnum of immort start room	*/
sh_int	r_frozen_start_room;		/* rnum of frozen start room	*/

char	*credits = 0;			/* game credits			*/
char	*news = 0;			/* mud news			*/
char	*motd = 0;			/* message of the day - mortals */
char	*imotd = 0;			/* message of the day - immorts */
char	*help = 0;			/* help screen			*/
char	*info = 0;			/* info page			*/
char	*wizlist = 0;			/* list of higher gods		*/
char	*immlist = 0;			/* list of peon gods		*/
char	*background = 0;		/* background story		*/
char	*handbook = 0;			/* handbook for new immortals	*/
char	*policies = 0;			/* policies page		*/

FILE *help_fl = 0;			/* file for help text		*/
struct help_index_element *help_index = 0; /* the help table		*/
int	top_of_helpt;			/* top of help index table	*/

struct time_info_data time_info;	/* the infomation about the time   */
struct weather_data weather_info;	/* the infomation about the weather */


/* local functions */
void	setup_dir(FILE *fl, int room, int dir);
void	index_boot(int mode);
void	load_rooms(FILE *fl);
void	load_mobiles(FILE *mob_f);
void	load_objects(FILE *obj_f);
void	load_zones(FILE *fl);
void	boot_the_shops(FILE *shop_f, char *filename);
void	assign_mobiles(void);
void	assign_objects(void);
void	assign_rooms(void);
void	assign_the_shopkeepers(void);
void	build_player_index(void);
void	char_to_store(struct char_data *ch, struct char_file_u *st);
void	store_to_char(struct char_file_u *st, struct char_data *ch);
int	is_empty(int zone_nr);
void	reset_zone(int zone);
int	file_to_string(char *name, char *buf);
int	file_to_string_alloc(char *name, char **buf);
void	check_start_rooms(void);
void	renum_world(void);
void	renum_zone_table(void);
void	reset_time(void);
void	clear_char(struct char_data *ch);

/* external functions */
extern struct descriptor_data *descriptor_list;
void	load_messages(void);
void	weather_and_time(int mode);
void	assign_command_pointers(void);
void	assign_spell_pointers(void);
void	boot_social_messages(void);
void	update_obj_file(void); /* In objsave.c */
void	sort_commands(void);
void	load_banned(void);
void	Read_Invalid_List(void);
struct help_index_element *build_help_index(FILE *fl, int *num);


/*************************************************************************
*  routines for booting the system                                       *
*********************************************************************** */

/* this is necessary for the autowiz system */
void	reboot_wizlists(void) 
{
   file_to_string_alloc(WIZLIST_FILE, &wizlist);
   file_to_string_alloc(IMMLIST_FILE, &immlist);
}


ACMD(do_reboot)
{
   int	i;

   one_argument(argument, arg);

   if (!str_cmp(arg, "all") || *arg == '*') {
      file_to_string_alloc(NEWS_FILE, &news);
      file_to_string_alloc(CREDITS_FILE, &credits);
      file_to_string_alloc(MOTD_FILE, &motd);
      file_to_string_alloc(IMOTD_FILE, &imotd);
      file_to_string_alloc(HELP_PAGE_FILE, &help);
      file_to_string_alloc(INFO_FILE, &info);
      file_to_string_alloc(WIZLIST_FILE, &wizlist);
      file_to_string_alloc(IMMLIST_FILE, &immlist);
      file_to_string_alloc(POLICIES_FILE, &policies);
      file_to_string_alloc(HANDBOOK_FILE, &handbook);
      file_to_string_alloc(BACKGROUND_FILE, &background);
   } else if (!str_cmp(arg, "wizlist"))
      file_to_string_alloc(WIZLIST_FILE, &wizlist);
   else if (!str_cmp(arg, "immlist"))
      file_to_string_alloc(IMMLIST_FILE, &immlist);
   else if (!str_cmp(arg, "news"))
      file_to_string_alloc(NEWS_FILE, &news);
   else if (!str_cmp(arg, "credits"))
      file_to_string_alloc(CREDITS_FILE, &credits);
   else if (!str_cmp(arg, "motd"))
      file_to_string_alloc(MOTD_FILE, &motd);
   else if (!str_cmp(arg, "imotd"))
      file_to_string_alloc(IMOTD_FILE, &imotd);
   else if (!str_cmp(arg, "help"))
      file_to_string_alloc(HELP_PAGE_FILE, &help);
   else if (!str_cmp(arg, "info"))
      file_to_string_alloc(INFO_FILE, &info);
   else if (!str_cmp(arg, "policy"))
      file_to_string_alloc(POLICIES_FILE, &policies);
   else if (!str_cmp(arg, "handbook"))
      file_to_string_alloc(HANDBOOK_FILE, &handbook);
   else if (!str_cmp(arg, "background"))
      file_to_string_alloc(BACKGROUND_FILE, &background);
   else if (!str_cmp(arg, "xhelp")) {
      if (help_fl)
	 fclose(help_fl);
      if (!(help_fl = fopen(HELP_KWRD_FILE, "r")))
	 return;
      else {
	 for (i = 0; i < top_of_helpt; i++)
	    free(help_index[i].keyword);
	 free(help_index);
	 help_index = build_help_index(help_fl, &top_of_helpt);
      }
   } else {
      send_to_char("Unknown reboot option.\n\r", ch);
      return;
   }

   send_to_char("Okay.\n\r", ch);
}


/* body of the booting system */
void	boot_db(void)
{
   int	i;
   extern int	no_specials;

   log("Boot db -- BEGIN.");

   log("Resetting the game time:");
   reset_time();

   log("Reading news, credits, help, bground, info & motds.");
   file_to_string_alloc(NEWS_FILE, &news);
   file_to_string_alloc(CREDITS_FILE, &credits);
   file_to_string_alloc(MOTD_FILE, &motd);
   file_to_string_alloc(IMOTD_FILE, &imotd);
   file_to_string_alloc(HELP_PAGE_FILE, &help);
   file_to_string_alloc(INFO_FILE, &info);
   file_to_string_alloc(WIZLIST_FILE, &wizlist);
   file_to_string_alloc(IMMLIST_FILE, &immlist);
   file_to_string_alloc(POLICIES_FILE, &policies);
   file_to_string_alloc(HANDBOOK_FILE, &handbook);
   file_to_string_alloc(BACKGROUND_FILE, &background);

   log("Opening help file.");
   if (!(help_fl = fopen(HELP_KWRD_FILE, "r")))
      log("   Could not open help file.");
   else
      help_index = build_help_index(help_fl, &top_of_helpt);

   log("Loading zone table.");
   index_boot(DB_BOOT_ZON);

   log("Loading rooms.");
   index_boot(DB_BOOT_WLD);

   log("Renumbering rooms.");
   renum_world();

   log("Checking start rooms.");
   check_start_rooms();

   log("Loading mobs and generating index.");
   index_boot(DB_BOOT_MOB);

   log("Loading objs and generating index.");
   index_boot(DB_BOOT_OBJ);

   log("Renumbering zone table.");
   renum_zone_table();

   log("Generating player index.");
   build_player_index();

   log("Loading fight messages.");
   load_messages();

   log("Loading social messages.");
   boot_social_messages();

   if (!no_specials) {
      log("Loading shops.");
      index_boot(DB_BOOT_SHP);
   }

   log("Assigning function pointers:");

   if (!no_specials) {
      log("   Shopkeepers.");
      assign_the_shopkeepers();
      log("   Mobiles.");
      assign_mobiles();
      log("   Objects.");
      assign_objects();
      log("   Rooms.");
      assign_rooms();
   }

   log("   Commands.");
   assign_command_pointers();
   log("   Spells.");
   assign_spell_pointers();

   log("Sorting command list.");
   sort_commands();

   log("Booting mail system.");
   if (!scan_file()) {
      log("    Mail boot failed -- Mail system disabled");
      no_mail = 1;
   }

   log("Reading banned site and invalid-name list.");
   load_banned();
   Read_Invalid_List();

   if (!no_rent_check) {
      log("Deleting timed-out crash and rent files:");
      update_obj_file();
      log("Done.");
   }

   for (i = 0; i <= top_of_zone_table; i++) {
      sprintf(buf2, "Resetting %s (rooms %d-%d).",
          zone_table[i].name,
          (i ? (zone_table[i - 1].top + 1) : 0),
          zone_table[i].top);
      log(buf2);
      reset_zone(i);
   }

   reset_q.head = reset_q.tail = 0;

   boot_time = time(0);

   log("Boot db -- DONE.");
}


/* reset the time in the game from file */
void	reset_time(void)
{
   long	beginning_of_time = 650336715;

   struct time_info_data mud_time_passed(time_t t2, time_t t1);

   time_info = mud_time_passed(time(0), beginning_of_time);


   switch (time_info.hours) {
   case 0 :
   case 1 :
   case 2 :
   case 3 :
   case 4 :
      weather_info.sunlight = SUN_DARK;
      break;
   case 5 :
      weather_info.sunlight = SUN_RISE;
      break;
   case 6 :
   case 7 :
   case 8 :
   case 9 :
   case 10 :
   case 11 :
   case 12 :
   case 13 :
   case 14 :
   case 15 :
   case 16 :
   case 17 :
   case 18 :
   case 19 :
   case 20 :
      weather_info.sunlight = SUN_LIGHT;
      break;
   case 21 :
      weather_info.sunlight = SUN_SET;
      break;
   case 22 :
   case 23 :
   default :
      weather_info.sunlight = SUN_DARK;
      break;
   }

   sprintf(buf, "   Current Gametime: %dH %dD %dM %dY.",
       time_info.hours, time_info.day,
       time_info.month, time_info.year);
   log(buf);

   weather_info.pressure = 960;
   if ((time_info.month >= 7) && (time_info.month <= 12))
      weather_info.pressure += dice(1, 50);
   else
      weather_info.pressure += dice(1, 80);

   weather_info.change = 0;

   if (weather_info.pressure <= 980)
      weather_info.sky = SKY_LIGHTNING;
   else if (weather_info.pressure <= 1000)
      weather_info.sky = SKY_RAINING;
   else if (weather_info.pressure <= 1020)
      weather_info.sky = SKY_CLOUDY;
   else
      weather_info.sky = SKY_CLOUDLESS;
}




/* generate index table for the player file */
void	build_player_index(void)
{
   int	nr = -1, i;
   long	size, recs;
   struct char_file_u dummy;

   if (!(player_fl = fopen(PLAYER_FILE, "r+b"))) {
      perror("Error opening playerfile");
      exit(1);
   }

   fseek(player_fl, 0L, SEEK_END);
   size = ftell(player_fl);
   rewind(player_fl);
   if (size % sizeof(struct char_file_u))
      fprintf(stderr, "WARNING:  PLAYERFILE IS PROBABLY CORRUPT!\n");
   recs = size / sizeof(struct char_file_u);
   if (recs) {
      sprintf(buf, "   %ld players in database.", recs);
      log(buf);
      CREATE(player_table, struct player_index_element, recs);
   } else {
      player_table = 0;
      top_of_p_file = top_of_p_table = -1;
      return;
   }

   for (; !feof(player_fl); ) {
      fread(&dummy, sizeof(struct char_file_u), 1, player_fl);
      if (!feof(player_fl))   /* new record */ {
	 nr++;
	 CREATE(player_table[nr].name, char, strlen(dummy.name) + 1);
	 for (i = 0;
	      (*(player_table[nr].name + i) = LOWER(*(dummy.name + i))); i++)
	    ;
	 top_idnum = MAX(top_idnum, dummy.specials2.idnum);
      }
   }

   top_of_p_file = top_of_p_table = nr;
}


/* function to count how many hash-mark delimited records exist in a file */
int	count_hash_records(FILE *fl)
{
   char	buf[120];
   int	count = 0;

   while (fgets(buf, 120, fl))
      if (*buf == '#')
	 count++;

   return (count - 1);
}



void	index_boot(int mode)
{
   char	*index_filename, *prefix;
   FILE * index, *db_file;
   int	rec_count = 0;

   switch (mode) {
   case DB_BOOT_WLD	: prefix = WLD_PREFIX; break;
   case DB_BOOT_MOB	: prefix = MOB_PREFIX; break;
   case DB_BOOT_OBJ	: prefix = OBJ_PREFIX; break;
   case DB_BOOT_ZON	: prefix = ZON_PREFIX; break;
   case DB_BOOT_SHP	: prefix = SHP_PREFIX; break;
   default:
      log("SYSERR: Unknown subcommand to index_boot!");
      exit(1);
      break;
   }

   if (mini_mud)
      index_filename = MINDEX_FILE;
   else
      index_filename = INDEX_FILE;

   sprintf(buf2, "%s/%s", prefix, index_filename);

   if (!(index = fopen(buf2, "r"))) {
      sprintf(buf1, "Error opening index file '%s'", buf2);
      perror(buf1);
      exit(1);
   }

   /* first, count the number of records in the file so we can malloc */
   if (mode != DB_BOOT_SHP) {
      fscanf(index, "%s\n", buf1);
      while (*buf1 != '$') {
	 sprintf(buf2, "%s/%s", prefix, buf1);
	 if (!(db_file = fopen(buf2, "r"))) {
	    perror(buf2);
	    exit(1);
	 } else {
	    if (mode == DB_BOOT_ZON)
	       rec_count++;
	    else
	       rec_count += count_hash_records(db_file);
	 }
	 fclose(db_file);
	 fscanf(index, "%s\n", buf1);
      }

      if (!rec_count) {
	 log("SYSERR: boot error - 0 records counted");
	 exit(1);
      }

      rec_count++;

      switch (mode) {
      case DB_BOOT_WLD :
	 CREATE(world, struct room_data, rec_count);
	 break;
      case DB_BOOT_MOB :
	 CREATE(mob_proto, struct char_data, rec_count);
	 CREATE(mob_index, struct index_data, rec_count);
	 break;
      case DB_BOOT_OBJ :
	 CREATE(obj_proto, struct obj_data, rec_count);
	 CREATE(obj_index, struct index_data, rec_count);
	 break;
      case DB_BOOT_ZON :
	 CREATE(zone_table, struct zone_data, rec_count);
	 break;
      }
   }

   rewind(index);
   fscanf(index, "%s\n", buf1);
   while (*buf1 != '$') {
      sprintf(buf2, "%s/%s", prefix, buf1);
      if (!(db_file = fopen(buf2, "r"))) {
	 perror(buf2);
	 exit(1);
      }

      switch (mode) {
      case DB_BOOT_WLD	: load_rooms(db_file); break;
      case DB_BOOT_OBJ	: load_objects(db_file); break;
      case DB_BOOT_MOB	: load_mobiles(db_file); break;
      case DB_BOOT_ZON	: load_zones(db_file); break;
      case DB_BOOT_SHP	: boot_the_shops(db_file, buf2); break;
      }

      fclose(db_file);
      fscanf(index, "%s\n", buf1);
   }
}


/* load the rooms */
void	load_rooms(FILE *fl)
{
   static int	room_nr = 0, zone = 0, virtual_nr, flag, tmp;
   char	*temp, chk[50];
   struct extra_descr_data *new_descr;

   do {
      fscanf(fl, " #%d\n", &virtual_nr);

      sprintf(buf2, "room #%d", virtual_nr);

      temp = fread_string(fl, buf2);
      if ((flag = (*temp != '$'))) {	/* a new record to be read */
	 world[room_nr].number = virtual_nr;
	 world[room_nr].name = temp;
	 world[room_nr].description = fread_string(fl, buf2);

	 if (top_of_zone_table >= 0) {
	    fscanf(fl, " %*d ");

	    /* OBS: Assumes ordering of input rooms */

	    if (world[room_nr].number <= (zone ? zone_table[zone-1].top : -1)) {
	       fprintf(stderr, "Room nr %d is below zone %d.\n",
	           room_nr, zone);
	       exit(0);
	    }
	    while (world[room_nr].number > zone_table[zone].top)
	       if (++zone > top_of_zone_table) {
		  fprintf(stderr, "Room %d is outside of any zone.\n",
		      virtual_nr);
		  exit(0);
	       }
	    world[room_nr].zone = zone;
	 }
	 fscanf(fl, " %d ", &tmp);
	 world[room_nr].room_flags = tmp;
	 fscanf(fl, " %d ", &tmp);
	 world[room_nr].sector_type = tmp;

	 world[room_nr].funct = 0;
	 world[room_nr].contents = 0;
	 world[room_nr].people = 0;
	 world[room_nr].light = 0; /* Zero light sources */

	 for (tmp = 0; tmp <= 5; tmp++)
	    world[room_nr].dir_option[tmp] = 0;

	 world[room_nr].ex_description = 0;

	 for (; ; ) {
	    fscanf(fl, " %s \n", chk);

	    if (*chk == 'D')  /* direction field */
	       setup_dir(fl, room_nr, atoi(chk + 1));
	    else if (*chk == 'E')  /* extra description field */ {
	       CREATE(new_descr, struct extra_descr_data, 1);
	       new_descr->keyword = fread_string(fl, buf2);
	       new_descr->description = fread_string(fl, buf2);
	       new_descr->next = world[room_nr].ex_description;
	       world[room_nr].ex_description = new_descr;
	    } else if (*chk == 'S')	/* end of current room */
	       break;
	 }

	 room_nr++;
      }
   } while (flag);

   free(temp);	/* cleanup the area containing the terminal $  */

   top_of_world = room_nr - 1;
}




/* read direction data */
void	setup_dir(FILE *fl, int room, int dir)
{
   int	tmp;

   sprintf(buf2, "Room #%d, direction D%d", world[room].number, dir);

   CREATE(world[room].dir_option[dir], struct room_direction_data , 1);

   world[room].dir_option[dir]->general_description = 
       fread_string(fl, buf2);
   world[room].dir_option[dir]->keyword = fread_string(fl, buf2);

   fscanf(fl, " %d ", &tmp);
   if (tmp == 1)
      world[room].dir_option[dir]->exit_info = EX_ISDOOR;
   else if (tmp == 2)
      world[room].dir_option[dir]->exit_info = EX_ISDOOR | EX_PICKPROOF;
   else
      world[room].dir_option[dir]->exit_info = 0;

   fscanf(fl, " %d ", &tmp);
   world[room].dir_option[dir]->key = tmp;

   fscanf(fl, " %d ", &tmp);
   world[room].dir_option[dir]->to_room = tmp;
}


void	check_start_rooms(void)
{
   extern sh_int mortal_start_room;
   extern sh_int immort_start_room;
   extern sh_int frozen_start_room;

   if ((r_mortal_start_room = real_room(mortal_start_room)) < 0) {
      log("SYSERR:  Mortal start room does not exist.  Change in config.c.");
      exit(1);
   }

   if ((r_immort_start_room = real_room(immort_start_room)) < 0) {
      if (!mini_mud)
         log("SYSERR:  Warning: Immort start room does not exist.  Change in config.c.");
      r_immort_start_room = r_mortal_start_room;
   }

   if ((r_frozen_start_room = real_room(frozen_start_room)) < 0) {
      if (!mini_mud)
         log("SYSERR:  Warning: Frozen start room does not exist.  Change in config.c.");
      r_frozen_start_room = r_mortal_start_room;
   }
}



void	renum_world(void)
{
   register int	room, door;

   for (room = 0; room <= top_of_world; room++)
      for (door = 0; door <= 5; door++)
	 if (world[room].dir_option[door])
	    if (world[room].dir_option[door]->to_room != NOWHERE)
	       world[room].dir_option[door]->to_room = 
	           real_room(world[room].dir_option[door]->to_room);
}


void	renum_zone_table(void)
{
   int	zone, comm, a, b;

   for (zone = 0; zone <= top_of_zone_table; zone++)
      for (comm = 0; zone_table[zone].cmd[comm].command != 'S'; comm++) {
	 a = b = 0;
	 switch (zone_table[zone].cmd[comm].command) {
	 case 'M':
	    a = zone_table[zone].cmd[comm].arg1 = 
	        real_mobile(zone_table[zone].cmd[comm].arg1);
	    b = zone_table[zone].cmd[comm].arg3 = 
	        real_room(zone_table[zone].cmd[comm].arg3);
	    break;
	 case 'O':
	    a = zone_table[zone].cmd[comm].arg1 = 
	        real_object(zone_table[zone].cmd[comm].arg1);
	    if (zone_table[zone].cmd[comm].arg3 != NOWHERE)
	       b = zone_table[zone].cmd[comm].arg3 = 
	           real_room(zone_table[zone].cmd[comm].arg3);
	    break;
	 case 'G':
	    a = zone_table[zone].cmd[comm].arg1 = 
	        real_object(zone_table[zone].cmd[comm].arg1);
	    break;
	 case 'E':
	    a = zone_table[zone].cmd[comm].arg1 = 
	        real_object(zone_table[zone].cmd[comm].arg1);
	    break;
	 case 'P':
	    a = zone_table[zone].cmd[comm].arg1 = 
	        real_object(zone_table[zone].cmd[comm].arg1);
	    b = zone_table[zone].cmd[comm].arg3 = 
	        real_object(zone_table[zone].cmd[comm].arg3);
	    break;
	 case 'D':
	    a = zone_table[zone].cmd[comm].arg1 = 
	        real_room(zone_table[zone].cmd[comm].arg1);
	    break;
	 }
	 if (a < 0 || b < 0) {
	    if (!mini_mud)
	       fprintf(stderr, "Invalid vnum in zone reset cmd: zone #%d, cmd %d .. command disabled.\n",
	            			zone_table[zone].number, comm + 1);
	    zone_table[zone].cmd[comm].command = '*';
	 }
      }
}



/* load the zone table and command tables */
void	load_zones(FILE *fl)
{
   static int	zon = 0;
   int	cmd_no = 0, expand, tmp;
   char	*check, buf[81];

   for (; ; ) {
      fscanf(fl, " #%d\n", &tmp);
      sprintf(buf2, "beginning of zone #%d", tmp);
      check = fread_string(fl, buf2);

      if (*check == '$')
	 break;		/* end of file */

      zone_table[zon].number = tmp;
      zone_table[zon].name = check;
      fscanf(fl, " %d ", &zone_table[zon].top);
      fscanf(fl, " %d ", &zone_table[zon].lifespan);
      fscanf(fl, " %d ", &zone_table[zon].reset_mode);

      /* read the command table */

      cmd_no = 0;

      for (expand = 1; ; ) {
	 if (expand)
	    if (!cmd_no)
	       CREATE(zone_table[zon].cmd, struct reset_com, 1);
	    else if (!(zone_table[zon].cmd = 
	        (struct reset_com *) realloc(zone_table[zon].cmd, 
	        (cmd_no + 1) * sizeof(struct reset_com)))) {
	       perror("reset command load");
	       exit(0);
	    }

	 expand = 1;

	 fscanf(fl, " "); /* skip blanks */
	 fscanf(fl, "%c", 
	     &zone_table[zon].cmd[cmd_no].command);

	 if (zone_table[zon].cmd[cmd_no].command == 'S')
	    break;

	 if (zone_table[zon].cmd[cmd_no].command == '*') {
	    expand = 0;
	    fgets(buf, 80, fl); /* skip command */
	    continue;
	 }

	 fscanf(fl, " %d %d %d", 
	     &tmp,
	     &zone_table[zon].cmd[cmd_no].arg1,
	     &zone_table[zon].cmd[cmd_no].arg2);

	 zone_table[zon].cmd[cmd_no].if_flag = tmp;

	 if (zone_table[zon].cmd[cmd_no].command == 'M' || 
	     zone_table[zon].cmd[cmd_no].command == 'O' || 
	     zone_table[zon].cmd[cmd_no].command == 'E' || 
	     zone_table[zon].cmd[cmd_no].command == 'P' || 
	     zone_table[zon].cmd[cmd_no].command == 'D')
	    fscanf(fl, " %d", &zone_table[zon].cmd[cmd_no].arg3);

	 fgets(buf, 80, fl);	/* read comment */

	 cmd_no++;
      }
      zon++;
   }
   top_of_zone_table = zon - 1;
   free(check);
}



/*************************************************************************
*  procedures for resetting, both play-time and boot-time	 	 *
*********************************************************************** */



int	vnum_mobile(char *searchname, struct char_data *ch)
{
   int	nr, found = 0;

   for (nr = 0; nr <= top_of_mobt; nr++) {
      if (isname(searchname, mob_proto[nr].player.name)) {
	 sprintf(buf, "%3d. [%5d] %-60.60s\n\r", ++found,
	     mob_index[nr].virtual,
	     mob_proto[nr].player.short_descr);
	 send_to_char(buf, ch);
      }
   }

   return(found);
}



int	vnum_object(char *searchname, struct char_data *ch)
{
   int	nr, found = 0;

   for (nr = 0; nr <= top_of_objt; nr++) {
      if (isname(searchname, obj_proto[nr].name)) {
	 sprintf(buf, "%3d. [%5d] %-60.60s\n\r", ++found,
	     obj_index[nr].virtual,
	     obj_proto[nr].short_description);
	 send_to_char(buf, ch);
      }
   }
   return(found);
}


/* create a new mobile from a prototype */
struct char_data *read_mobile(int nr, int type)
{
   int	i;
   struct char_data *mob;

   if (type == VIRTUAL) {
      if ((i = real_mobile(nr)) < 0) {
	 sprintf(buf, "Mobile (V) %d does not exist in database.", nr);
	 return(0);
      }
   } else
      i = nr;

   CREATE(mob, struct char_data, 1);

   *mob = mob_proto[i];

   if (!mob->points.max_hit) {
      mob->points.max_hit = dice(mob->points.hit, mob->points.mana) + 
          mob->points.move;
   } else
      mob->points.max_hit = number(mob->points.hit, mob->points.mana);

   mob->points.hit = mob->points.max_hit;
   mob->points.mana = mob->points.max_mana;
   mob->points.move = mob->points.max_move;

   mob->player.time.birth = time(0);
   mob->player.time.played = 0;
   mob->player.time.logon  = time(0);

   /* insert in list */
   mob->next = character_list;
   character_list = mob;

   mob_index[i].number++;

   return mob;
}


void	load_mobiles(FILE *mob_f)
{
   static int	i = 0;
   int	nr, j;
   long	tmp, tmp2, tmp3, tmp4, tmp5, tmp6;
   char	chk[10], *tmpptr;
   char	letter;


   if (!fscanf(mob_f, "%s\n", chk)) {
      perror("load_mobiles");
      exit(1);
   }

   for (; ; ) {
      if (*chk == '#') {
	 sscanf(chk, "#%d\n", &nr);
	 if (nr >= 99999) 
	    break;

	 mob_index[i].virtual = nr;
	 mob_index[i].number  = 0;
	 mob_index[i].func    = 0;

	 clear_char(mob_proto + i);

	 sprintf(buf2, "mob vnum %d", nr);

	 /***** String data *** */
	 mob_proto[i].player.name = fread_string(mob_f, buf2);
	 tmpptr = mob_proto[i].player.short_descr = fread_string(mob_f, buf2);
	 if (tmpptr && *tmpptr)
	    if (!str_cmp(fname(tmpptr), "a") || 
	        !str_cmp(fname(tmpptr), "an") || 
	        !str_cmp(fname(tmpptr), "the"))
	       *tmpptr = tolower(*tmpptr);
	 mob_proto[i].player.long_descr = fread_string(mob_f, buf2);
	 mob_proto[i].player.description = fread_string(mob_f, buf2);
	 mob_proto[i].player.title = 0;

	 /* *** Numeric data *** */
	 fscanf(mob_f, "%d ", &tmp);
	 MOB_FLAGS(mob_proto + i) = tmp;
	 SET_BIT(MOB_FLAGS(mob_proto + i), MOB_ISNPC);

	 fscanf(mob_f, " %d %d %c \n", &tmp, &tmp2, &letter);
	 mob_proto[i].specials.affected_by = tmp;
	 GET_ALIGNMENT(mob_proto + i) = tmp2;

	 if (letter == 'S') {
	    /* The new easy monsters */
	    mob_proto[i].abilities.str   = 11;
	    mob_proto[i].abilities.intel = 11;
	    mob_proto[i].abilities.wis   = 11;
	    mob_proto[i].abilities.dex   = 11;
	    mob_proto[i].abilities.con   = 11;

	    fscanf(mob_f, " %d %d %d", &tmp, &tmp2, &tmp3);
	    GET_LEVEL(mob_proto + i) = tmp;
	    mob_proto[i].points.hitroll = 20 - tmp2;
	    mob_proto[i].points.armor = 10 * tmp3;

	    fscanf(mob_f, " %dd%d+%d ", &tmp, &tmp2, &tmp3);
	    mob_proto[i].points.max_hit = 0;
	    mob_proto[i].points.hit = tmp;
	    mob_proto[i].points.mana = tmp2;
	    mob_proto[i].points.move = tmp3;

	    mob_proto[i].points.max_mana = 10;
	    mob_proto[i].points.max_move = 50;

	    fscanf(mob_f, " %dd%d+%d \n", &tmp, &tmp2, &tmp3);
	    mob_proto[i].specials.damnodice = tmp;
	    mob_proto[i].specials.damsizedice = tmp2;
	    mob_proto[i].points.damroll = tmp3;

	    fscanf(mob_f, " %d %d \n", &tmp, &tmp2);
	    GET_GOLD(mob_proto + i) = tmp;
	    GET_EXP(mob_proto + i) = tmp2;

	    fscanf(mob_f, " %d %d %d \n", &tmp, &tmp2, &tmp3);
	    mob_proto[i].specials.position = tmp;
	    mob_proto[i].specials.default_pos = tmp2;
	    mob_proto[i].player.sex = tmp3;

	    mob_proto[i].player.class = 0;

	    mob_proto[i].player.weight = 200;
	    mob_proto[i].player.height = 198;

	    for (j = 0; j < 3; j++)
	       GET_COND(mob_proto + i, j) = -1;

	    for (j = 0; j < 5; j++)
	       mob_proto[i].specials2.apply_saving_throw[j] = 
	           MAX(20 - GET_LEVEL(mob_proto + i), 2);
	 } else {  /* The old monsters are down below here */
	    fscanf(mob_f, " %d %d %d %d %d \n ", &tmp, &tmp2, &tmp3, &tmp4, &tmp5);
	    mob_proto[i].abilities.str = tmp;
	    mob_proto[i].abilities.intel = tmp2;
	    mob_proto[i].abilities.wis = tmp3;
	    mob_proto[i].abilities.dex = tmp4;
	    mob_proto[i].abilities.con = tmp5;

	    fscanf(mob_f, " %d %d ", &tmp, &tmp2);

	    mob_proto[i].points.max_hit = 1;
	    mob_proto[i].points.hit = tmp;
	    mob_proto[i].points.mana = tmp2;

	    fscanf(mob_f, " %d ", &tmp);
	    mob_proto[i].points.armor = 10 * tmp;

	    fscanf(mob_f, " %d %d %d %d \n ", &tmp, &tmp2, &tmp3, &tmp4);
	    mob_proto[i].points.max_mana = tmp;
	    mob_proto[i].points.max_move = tmp2;
	    mob_proto[i].points.gold = tmp3;
	    GET_EXP(mob_proto + i) = tmp4;

	    fscanf(mob_f, " %d %d %d %d %d %d ", &tmp, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6);
	    mob_proto[i].specials.position = tmp;
	    mob_proto[i].specials.default_pos = tmp2;
	    mob_proto[i].player.sex = tmp3;
	    mob_proto[i].player.class = tmp4;
	    GET_LEVEL(mob_proto + i) = tmp5;

	    fscanf(mob_f, " %d %d \n", &tmp, &tmp2);
	    mob_proto[i].player.weight = tmp;
	    mob_proto[i].player.height = tmp2;

	    for (j = 0; j < 3; j++) {
	       fscanf(mob_f, " %d ", &tmp);
	       GET_COND(mob_proto + i, j) = tmp;
	    }

	    fscanf(mob_f, " \n ");

	    for (j = 0; j < 5; j++) {
	       fscanf(mob_f, " %d ", &tmp);
	       mob_proto[i].specials2.apply_saving_throw[j] = tmp;
	    }

	    fscanf(mob_f, " \n ");

	    /* Set the damage as some standard 1d4 */
	    mob_proto[i].points.damroll = 0;
	    mob_proto[i].specials.damnodice = 1;
	    mob_proto[i].specials.damsizedice = 6;

	    /* Calculate THAC0 as a formular of Level */
	    mob_proto[i].points.hitroll = MAX(1, GET_LEVEL(mob_proto + i) - 3);
	 }

	 mob_proto[i].tmpabilities = mob_proto[i].abilities;

	 for (j = 0; j < MAX_WEAR; j++) /* Initialisering Ok */
	    mob_proto[i].equipment[j] = 0;

	 mob_proto[i].nr = i;
	 mob_proto[i].desc = 0;

	 if (!fscanf(mob_f, "%s\n", chk)) {
	    sprintf(buf2, "SYSERR: Format error in mob file near mob #%d", nr);
	    log(buf2);
	    exit(1);
	 }
	 i++;
      } else if (*chk == '$') /* EOF */
	 break;
      else {
         sprintf(buf2, "SYSERR: Format error in mob file near mob #%d", nr);
	 log(buf2);
	 exit(1);
      }
   }
   top_of_mobt = i - 1;
}


/* create a new object from a prototype */
struct obj_data *read_object(int nr, int type)
{
   struct obj_data *obj;
   int	i;

   if (nr < 0) {
      log("SYSERR: trying to create obj with negative num!");
      return 0;
   }

   if (type == VIRTUAL) {
      if ((i = real_object(nr)) < 0) {
	 sprintf(buf, "Object (V) %d does not exist in database.", nr);
	 return(0);
      }
   } else
      i = nr;

   CREATE(obj, struct obj_data, 1);
   *obj = obj_proto[i];

   obj->next = object_list;
   object_list = obj;

   obj_index[i].number++;

   return (obj);
}



/* read all objects from obj file; generate index and prototypes */
void	load_objects(FILE *obj_f)
{
   static int	i = 0;
   int	tmp, tmp2, tmp3, tmp4, j, nr;
   char	chk[50], *tmpptr;
   struct extra_descr_data *new_descr;

   if (!fscanf(obj_f, "%s\n", chk)) {
      perror("load_objects");
      exit(1);
   }

   for (; ; ) {
      if (*chk == '#') {
	 sscanf(chk, "#%d\n", &nr);
	 if (nr >= 99999) 
	    break;

	 obj_index[i].virtual = nr;
	 obj_index[i].number  = 0;
	 obj_index[i].func    = 0;

	 clear_object(obj_proto + i);

	 sprintf(buf2, "object #%d", nr);

	 /* *** string data *** */

	 tmpptr = obj_proto[i].name = fread_string(obj_f, buf2);
	 if (!tmpptr) {
	    fprintf(stderr, "format error at or near %s\n", buf2);
	    exit(1);
	 }

	 tmpptr = obj_proto[i].short_description = fread_string(obj_f, buf2);
	 if (*tmpptr)
	    if (!str_cmp(fname(tmpptr), "a") || 
	        !str_cmp(fname(tmpptr), "an") || 
	        !str_cmp(fname(tmpptr), "the"))
	       *tmpptr = tolower(*tmpptr);
	 tmpptr = obj_proto[i].description = fread_string(obj_f, buf2);
	 if (tmpptr && *tmpptr)
	    *tmpptr = toupper(*tmpptr);
	 obj_proto[i].action_description = fread_string(obj_f, buf2);

	 /* *** numeric data *** */

	 fscanf(obj_f, " %d %d %d", &tmp, &tmp2, &tmp3);
	 obj_proto[i].obj_flags.type_flag = tmp;
	 obj_proto[i].obj_flags.extra_flags = tmp2;
	 obj_proto[i].obj_flags.wear_flags = tmp3;

	 fscanf(obj_f, " %d %d %d %d", &tmp, &tmp2, &tmp3, &tmp4);
	 obj_proto[i].obj_flags.value[0] = tmp;
	 obj_proto[i].obj_flags.value[1] = tmp2;
	 obj_proto[i].obj_flags.value[2] = tmp3;
	 obj_proto[i].obj_flags.value[3] = tmp4;

	 fscanf(obj_f, " %d %d %d", &tmp, &tmp2, &tmp3);
	 obj_proto[i].obj_flags.weight = tmp;
	 obj_proto[i].obj_flags.cost = tmp2;
	 obj_proto[i].obj_flags.cost_per_day = tmp3;

	 /* *** extra descriptions *** */

	 obj_proto[i].ex_description = 0;

	 sprintf(buf2, "%s - extra desc. section", buf2);

	 while (fscanf(obj_f, " %s \n", chk), *chk == 'E') {
	    CREATE(new_descr, struct extra_descr_data, 1);
	    new_descr->keyword = fread_string(obj_f, buf2);
	    new_descr->description = fread_string(obj_f, buf2);
	    new_descr->next = obj_proto[i].ex_description;
	    obj_proto[i].ex_description = new_descr;
	 }

	 for (j = 0 ; (j < MAX_OBJ_AFFECT) && (*chk == 'A') ; j++) {
	    fscanf(obj_f, " %d %d ", &tmp, &tmp2);
	    obj_proto[i].affected[j].location = tmp;
	    obj_proto[i].affected[j].modifier = tmp2;
	    fscanf(obj_f, " %s \n", chk);
	 }

	 for (; (j < MAX_OBJ_AFFECT); j++) {
	    obj_proto[i].affected[j].location = APPLY_NONE;
	    obj_proto[i].affected[j].modifier = 0;
	 }

	 obj_proto[i].in_room = NOWHERE;
	 obj_proto[i].next_content = 0;
	 obj_proto[i].carried_by = 0;
	 obj_proto[i].in_obj = 0;
	 obj_proto[i].contains = 0;
	 obj_proto[i].item_number = i;

	 i++;
      } else if (*chk == '$') /* EOF */
	 break;
      else {
         sprintf(buf2, "Format error in obj file at or near obj #%d", nr);
         log(buf2);
         exit(1);
      }
   }
   top_of_objt = i - 1;
}



#define ZO_DEAD  999

/* update zone ages, queue for reset if necessary, and dequeue when possible */
void	zone_update(void)
{
   int	i;
   struct reset_q_element *update_u, *temp;
   static int timer = 0;
   char buf[128];

   /* jelson 10/22/92 */
   if (((++timer * PULSE_ZONE) / 4) >= 60) { /* 4 comes from 4 passes/sec */
      /* one minute has passed */
      /* NOT accurate unless PULSE_ZONE is a multiple of 4 or a factor of 60 */

      timer = 0;

      /* since one minute has passed, increment zone ages */
      for (i = 0; i <= top_of_zone_table; i++) {
         if (zone_table[i].age < zone_table[i].lifespan &&
	     zone_table[i].reset_mode)
	       (zone_table[i].age)++;

         if (zone_table[i].age >= zone_table[i].lifespan &&
	     zone_table[i].age < ZO_DEAD && zone_table[i].reset_mode) {
                /* enqueue zone */
 
	        CREATE(update_u, struct reset_q_element, 1);

		update_u->zone_to_reset = i;
		update_u->next = 0;

		if (!reset_q.head)
		   reset_q.head = reset_q.tail = update_u;
		else {
		   reset_q.tail->next = update_u;
		   reset_q.tail = update_u;
		}

		zone_table[i].age = ZO_DEAD;
	 }
      }
   }

   /* dequeue zones (if possible) and reset */

   for (update_u = reset_q.head; update_u; update_u = update_u->next)
      if (zone_table[update_u->zone_to_reset].reset_mode == 2 || 
          is_empty(update_u->zone_to_reset)) {
	 reset_zone(update_u->zone_to_reset);
	 sprintf(buf, "Auto zone reset: %s",
		 zone_table[update_u->zone_to_reset].name);
	 mudlog(buf, CMP, LEVEL_GOD, FALSE);
	 /* dequeue */
	 if (update_u == reset_q.head)
	    reset_q.head = reset_q.head->next;
	 else {
	    for (temp = reset_q.head; temp->next != update_u;
		 temp = temp->next) ;

	    if (!update_u->next)
	       reset_q.tail = temp;

	    temp->next = update_u->next;
	 }

	 free(update_u);
	 break;
      }
}



#define ZCMD zone_table[zone].cmd[cmd_no]

/* execute the reset command table of a given zone */
void	reset_zone(int zone)
{
   int	cmd_no, last_cmd = 1;
   char	buf[256];
   struct char_data *mob;
   struct obj_data *obj, *obj_to;

   for (cmd_no = 0; ; cmd_no++) {
      if (ZCMD.command == 'S')
	 break;

      if (last_cmd || !ZCMD.if_flag)
	 switch (ZCMD.command) {
	 case '*': /* ignore command */
	    break;

	 case 'M': /* read a mobile */
	    if (mob_index[ZCMD.arg1].number < ZCMD.arg2) {
	       mob = read_mobile(ZCMD.arg1, REAL);
	       char_to_room(mob, ZCMD.arg3);
	       last_cmd = 1;
	    } else
	       last_cmd = 0;
	    break;

	 case 'O': /* read an object */
	    if (obj_index[ZCMD.arg1].number < ZCMD.arg2)
	       if (ZCMD.arg3 >= 0) {
		  if (!get_obj_in_list_num(ZCMD.arg1, world[ZCMD.arg3].contents)) {
		     obj = read_object(ZCMD.arg1, REAL);
		     obj_to_room(obj, ZCMD.arg3);
		     last_cmd = 1;
		  } else
		     last_cmd = 0;
	       }
	       else {
		  obj = read_object(ZCMD.arg1, REAL);
		  obj->in_room = NOWHERE;
		  last_cmd = 1;
	       }
	    else
	       last_cmd = 0;
	    break;

	 case 'P': /* object to object */
	    if (obj_index[ZCMD.arg1].number < ZCMD.arg2) {
	       obj = read_object(ZCMD.arg1, REAL);
	       if (!(obj_to = get_obj_num(ZCMD.arg3))) {
		  log("SYSERR: error in zone file: target obj not found.");
		  sprintf(buf, "SYSERR:   Offending cmd: \"P %d %d %d\" in zone #%d (cmd %d)",
		       			  obj_index[ZCMD.arg1].virtual, ZCMD.arg2,
		      obj_index[ZCMD.arg3].virtual,
		      zone_table[zone].number, cmd_no + 1);
		  log(buf);
		  last_cmd = 0;
		  break;
	       }
	       obj_to_obj(obj, obj_to);
	       last_cmd = 1;
	    } else
	       last_cmd = 0;
	    break;

	 case 'G': /* obj_to_char */
	    if (!mob) {
	       log("SYSERR: error in zone file: attempt to give obj to non-existant mob.");
	       sprintf(buf, "SYSERR:   Offending cmd: \"G %d %d\" in zone #%d (cmd %d)",
	           mob_index[ZCMD.arg1].virtual, ZCMD.arg2,
	           zone_table[zone].number, cmd_no + 1);
	       log(buf);
	       last_cmd = 0;
	       break;
	    }
	    if (obj_index[ZCMD.arg1].number < ZCMD.arg2) {
	       obj = read_object(ZCMD.arg1, REAL);
	       obj_to_char(obj, mob);
	       last_cmd = 1;
	    } else
	       last_cmd = 0;
	    break;

	 case 'E': /* object to equipment list */
	    if (!mob) {
	       log("SYSERR: error in zone file: trying to equip non-existant mob");
	       sprintf(buf, "SYSERR:   Offending cmd: \"E %d %d %d\" in zone #%d (cmd %d)",
	           obj_index[ZCMD.arg1].virtual, ZCMD.arg2,
	           ZCMD.arg3, zone_table[zone].number, cmd_no + 1);
	       log(buf);
	       last_cmd = 0;
	       break;
	    }
	    if (obj_index[ZCMD.arg1].number < ZCMD.arg2) {
	       if (ZCMD.arg3 < 0 || ZCMD.arg3 >= MAX_WEAR) {
		   log("SYSERR: error in zone file: invalid equipment pos number");
		   sprintf(buf, "SYSERR:   Offending cmd: \"E %d %d %d\" in zone #%d (cmd %d)",
			   obj_index[ZCMD.arg1].virtual, ZCMD.arg2,
			   ZCMD.arg3, zone_table[zone].number, cmd_no + 1);
		   log(buf);
		   last_cmd = 0;
	       } else {
		   obj = read_object(ZCMD.arg1, REAL);
		   equip_char(mob, obj, ZCMD.arg3);
		   last_cmd = 1;
	       }
	    } else
	       last_cmd = 0;
	    break;

	 case 'D': /* set state of door */
	    switch (ZCMD.arg3) {
	    case 0:
	       REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
	           EX_LOCKED);
	       REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
	           EX_CLOSED);
	       break;
	    case 1:
	       SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
	           EX_CLOSED);
	       REMOVE_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
	           EX_LOCKED);
	       break;
	    case 2:
	       SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
	           EX_LOCKED);
	       SET_BIT(world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
	           EX_CLOSED);
	       break;
	    }
	    last_cmd = 1;
	    break;

	 default:
	    sprintf(buf, "SYSERR:  Unknown cmd in reset table; zone %d cmd %d.\n\r",
	        zone_table[zone].number, cmd_no + 1);
	    mudlog(buf, NRM, LEVEL_GRGOD, TRUE);
	    break;
	 }
      else
	 last_cmd = 0;

   }

   zone_table[zone].age = 0;
}



/* for use in reset_zone; return TRUE if zone 'nr' is free of PC's  */
int	is_empty(int zone_nr)
{
   struct descriptor_data *i;

   for (i = descriptor_list; i; i = i->next)
      if (!i->connected)
	 if (world[i->character->in_room].zone == zone_nr)
	    return(0);

   return(1);
}





/*************************************************************************
*  stuff related to the save/load player system				 *
*********************************************************************** */

/* Load a char, TRUE if loaded, FALSE if not */
int	load_char(char *name, struct char_file_u *char_element)
{
   int	player_i;

   int	find_name(char *name);

   if ((player_i = find_name(name)) >= 0) {
      fseek(player_fl, (long) (player_i * sizeof(struct char_file_u)), SEEK_SET);
      fread(char_element, sizeof(struct char_file_u), 1, player_fl);
      return(player_i);
   } else
      return(-1);
}




/* copy data from the file structure to a char struct */
void	store_to_char(struct char_file_u *st, struct char_data *ch)
{
   int	i;

   GET_SEX(ch) = st->sex;
   GET_CLASS(ch) = st->class;
   GET_LEVEL(ch) = st->level;

   ch->player.short_descr = 0;
   ch->player.long_descr = 0;

   if (*st->title) {
      CREATE(ch->player.title, char, strlen(st->title) + 1);
      strcpy(ch->player.title, st->title);
   } else
      GET_TITLE(ch) = 0;

   if (*st->description) {
      CREATE(ch->player.description, char, 
          strlen(st->description) + 1);
      strcpy(ch->player.description, st->description);
   } else
      ch->player.description = 0;

   ch->player.hometown = st->hometown;

   ch->player.time.birth = st->birth;
   ch->player.time.played = st->played;
   ch->player.time.logon  = time(0);

   for (i = 0; i < MAX_TOUNGE; i++)
      ch->player.talks[i] = st->talks[i];

   ch->player.weight = st->weight;
   ch->player.height = st->height;

   ch->abilities = st->abilities;
   ch->tmpabilities = st->abilities;
   ch->points = st->points;
   ch->specials2 = st->specials2;
   if (ch->points.max_mana < 100) {
      ch->points.max_mana = 100;
   }

   /* New dynamic skill system: only PCs have a skill array allocated. */
   CREATE(ch->skills, byte, MAX_SKILLS);
   for (i = 0; i < MAX_SKILLS; i++)
      SET_SKILL(ch, i, st->skills[i]);

   ch->specials.carry_weight = 0;
   ch->specials.carry_items  = 0;
   ch->points.armor          = 100;
   ch->points.hitroll        = 0;
   ch->points.damroll        = 0;

   CREATE(ch->player.name, char, strlen(st->name) + 1);
   strcpy(ch->player.name, st->name);

   /* Add all spell effects */
   for (i = 0; i < MAX_AFFECT; i++) {
      if (st->affected[i].type)
	 affect_to_char(ch, &st->affected[i]);
   }

   ch->in_room = GET_LOADROOM(ch);

   affect_total(ch);

   /* If you're not poisioned and you've been away for more than
      an hour, we'll set your HMV back to full */

   if (!IS_AFFECTED(ch, AFF_POISON) && 
       (((long) (time(0) - st->last_logon)) >= SECS_PER_REAL_HOUR)) {
      GET_HIT(ch) = GET_MAX_HIT(ch);
      GET_MOVE(ch) = GET_MAX_MOVE(ch);
      GET_MANA(ch) = GET_MAX_MANA(ch);
   }

} /* store_to_char */





/* copy vital data from a players char-structure to the file structure */
void	char_to_store(struct char_data *ch, struct char_file_u *st)
{
   int	i;
   struct affected_type *af;
   struct obj_data *char_eq[MAX_WEAR];

   /* Unaffect everything a character can be affected by */

   for (i = 0; i < MAX_WEAR; i++) {
      if (ch->equipment[i])
	 char_eq[i] = unequip_char(ch, i);
      else
	 char_eq[i] = 0;
   }

   for (af = ch->affected, i = 0; i < MAX_AFFECT; i++) {
      if (af) {
	 st->affected[i] = *af;
	 st->affected[i].next = 0;
	 af = af->next;
      } else {
	 st->affected[i].type = 0;  /* Zero signifies not used */
	 st->affected[i].duration = 0;
	 st->affected[i].modifier = 0;
	 st->affected[i].location = 0;
	 st->affected[i].bitvector = 0;
	 st->affected[i].next = 0;
      }
   }


   /* remove the affections so that the raw values are stored;
      otherwise the effects are doubled when the char logs back in. */

   while (ch->affected)
      affect_remove(ch, ch->affected);

   if ((i >= MAX_AFFECT) && af && af->next)
      log("SYSERR: WARNING: OUT OF STORE ROOM FOR AFFECTED TYPES!!!");

   ch->tmpabilities = ch->abilities;

   st->birth      = ch->player.time.birth;
   st->played     = ch->player.time.played;
   st->played    += (long) (time(0) - ch->player.time.logon);
   st->last_logon = time(0);

   ch->player.time.played = st->played;
   ch->player.time.logon = time(0);

   st->hometown = ch->player.hometown;
   st->weight   = GET_WEIGHT(ch);
   st->height   = GET_HEIGHT(ch);
   st->sex      = GET_SEX(ch);
   st->class    = GET_CLASS(ch);
   st->level    = GET_LEVEL(ch);
   st->abilities = ch->abilities;
   st->points    = ch->points;
   st->specials2 = ch->specials2;

   st->points.armor   = 100;
   st->points.hitroll =  0;
   st->points.damroll =  0;

   if (GET_TITLE(ch))
      strcpy(st->title, GET_TITLE(ch));
   else
      *st->title = '\0';

   if (ch->player.description)
      strcpy(st->description, ch->player.description);
   else
      *st->description = '\0';


   for (i = 0; i < MAX_TOUNGE; i++)
      st->talks[i] = ch->player.talks[i];

   for (i = 0; i < MAX_SKILLS; i++)
      st->skills[i] = GET_SKILL(ch, i);

   strcpy(st->name, GET_NAME(ch));

   /* add spell and eq affections back in now */
   for (i = 0; i < MAX_AFFECT; i++) {
      if (st->affected[i].type)
	 affect_to_char(ch, &st->affected[i]);
   }

   for (i = 0; i < MAX_WEAR; i++) {
      if (char_eq[i])
	 equip_char(ch, char_eq[i], i);
   }

   affect_total(ch);
} /* Char to store */




/* create a new entry in the in-memory index table for the player file */
int	create_entry(char *name)
{
   int	i;

   if (top_of_p_table == -1) {
      CREATE(player_table, struct player_index_element, 1);
      top_of_p_table = 0;
   } else if (!(player_table = (struct player_index_element *)
       realloc(player_table, sizeof(struct player_index_element) * 
       (++top_of_p_table + 1)))) {
      perror("create entry");
      exit(1);
   }

   CREATE(player_table[top_of_p_table].name, char , strlen(name) + 1);

   /* copy lowercase equivalent of name to table field */
   for (i = 0; (*(player_table[top_of_p_table].name + i) = LOWER(*(name + i)));
	i++)
      ;

   return (top_of_p_table);
}




/* write the vital data of a player to the player file */
void	save_char(struct char_data *ch, sh_int load_room)
{
   struct char_file_u st;

   if (IS_NPC(ch) || !ch->desc)
      return;

   char_to_store(ch, &st);

   strncpy(st.host, ch->desc->host, HOST_LEN);
   st.host[HOST_LEN] = '\0';

   if (!PLR_FLAGGED(ch, PLR_LOADROOM))
      st.specials2.load_room = load_room;

   strcpy(st.pwd, ch->desc->pwd);

   fseek(player_fl, ch->desc->pos * sizeof(struct char_file_u), SEEK_SET);
   fwrite(&st, sizeof(struct char_file_u), 1, player_fl);
}


/************************************************************************
*  procs of a (more or less) general utility nature			*
********************************************************************** */


/* read and allocate space for a '~'-terminated string from a given file */
char	*fread_string(FILE *fl, char *error)
{
   char	buf[MAX_STRING_LENGTH], tmp[500];
   char	*rslt;
   register char	*point;
   int	flag;

   bzero(buf, MAX_STRING_LENGTH);

   do {
      if (!fgets(tmp, MAX_STRING_LENGTH, fl)) {
	 fprintf(stderr, "fread_string: format error at or near %s\n", error);
	 exit(0);
      }

      if (strlen(tmp) + strlen(buf) > MAX_STRING_LENGTH) {
	 log("SYSERR: fread_string: string too large (db.c)");
	 exit(0);
      } else
	 strcat(buf, tmp);

      for (point = buf + strlen(buf) - 2; point >= buf && isspace(*point); 
          point--)
	 ;
      if ((flag = (*point == '~')))
	 if (*(buf + strlen(buf) - 3) == '\n') {
	    *(buf + strlen(buf) - 2) = '\r';
	    *(buf + strlen(buf) - 1) = '\0';
	 } 
	 else
	    *(buf + strlen(buf) - 2) = '\0';
      else {
	 *(buf + strlen(buf) + 1) = '\0';
	 *(buf + strlen(buf)) = '\r';
      }
   } while (!flag);

   /* do the allocate boogie  */

   if (strlen(buf) > 0) {
      CREATE(rslt, char, strlen(buf) + 1);
      strcpy(rslt, buf);
   } else
      rslt = 0;
   return(rslt);
}





/* release memory allocated for a char struct */
void	free_char(struct char_data *ch)
{
   int	i;

   if (ch->specials.poofIn) 
      free(ch->specials.poofIn);
   if (ch->specials.poofOut) 
      free(ch->specials.poofOut);

   if (!IS_NPC(ch) || (IS_NPC(ch) && ch->nr == -1)) {
      if (GET_NAME(ch)) 
	 free(GET_NAME(ch));
      if (ch->player.title) 
	 free(ch->player.title);
      if (ch->player.short_descr) 
	 free(ch->player.short_descr);
      if (ch->player.long_descr) 
	 free(ch->player.long_descr);
      if (ch->player.description) 
	 free(ch->player.description);
   } else if ((i = ch->nr) > -1) {
      if (ch->player.name && ch->player.name != mob_proto[i].player.name)
	 free(ch->player.name);
      if (ch->player.title && ch->player.title != mob_proto[i].player.title)
	 free(ch->player.title);
      if (ch->player.short_descr && ch->player.short_descr != mob_proto[i].player.short_descr)
	 free(ch->player.short_descr);
      if (ch->player.long_descr && ch->player.long_descr != mob_proto[i].player.long_descr)
	 free(ch->player.long_descr);
      if (ch->player.description && ch->player.description != mob_proto[i].player.description)
	 free(ch->player.description);
   }

   if (ch->skills) {
      free(ch->skills);
      if (IS_NPC(ch))
	 log("SYSERR: Mob had skills array allocated!");
   }

   while (ch->affected)
      affect_remove(ch, ch->affected);

   free(ch);
}




/* release memory allocated for an obj struct */
void	free_obj(struct obj_data *obj)
{
   int	nr;
   struct extra_descr_data *this, *next_one;

   if ((nr = obj->item_number) == -1) {
      if (obj->name) 
	 free(obj->name);
      if (obj->description) 
	 free(obj->description);
      if (obj->short_description) 
	 free(obj->short_description);
      if (obj->action_description) 
	 free(obj->action_description);
      if (obj->ex_description)
	 for (this = obj->ex_description; this; this = next_one) {
	    next_one = this->next;
	    if (this->keyword) 
	       free(this->keyword);
	    if (this->description) 
	       free(this->description);
	    free(this);
	 }
   } else {
      if (obj->name && obj->name != obj_proto[nr].name)
	 free(obj->name);
      if (obj->description && obj->description != obj_proto[nr].description)
	 free(obj->description);
      if (obj->short_description && obj->short_description != obj_proto[nr].short_description)
	 free(obj->short_description);
      if (obj->action_description && obj->action_description != obj_proto[nr].action_description)
	 free(obj->action_description);
      if (obj->ex_description && obj->ex_description != obj_proto[nr].ex_description)
	 for (this = obj->ex_description; this; this = next_one) {
	    next_one = this->next;
	    if (this->keyword) 
	       free(this->keyword);
	    if (this->description) 
	       free(this->description);
	    free(this);
	 }
   }

   free(obj);
}





/* read contets of a text file, alloc space, point buf to it */
int	file_to_string_alloc(char *name, char **buf)
{
   char	temp[MAX_STRING_LENGTH];

   if (file_to_string(name, temp) < 0)
      return -1;

   if (*buf)
      free(*buf);

   *buf = str_dup(temp);

   return 0;
}




/* read contents of a text file, and place in buf */
int	file_to_string(char *name, char *buf)
{
   FILE * fl;
   char	tmp[100];

   *buf = '\0';

   if (!(fl = fopen(name, "r"))) {
      sprintf(tmp, "Error reading %s", name);
      perror(tmp);
      *buf = '\0';
      return(-1);
   }

   do {
      fgets(tmp, 99, fl);

      if (!feof(fl)) {
	 if (strlen(buf) + strlen(tmp) + 2 > MAX_STRING_LENGTH) {
	    log("SYSERR: fl->strng: string too big (db.c, file_to_string)");
	    *buf = '\0';
	    return(-1);
	 }

	 strcat(buf, tmp);
	 *(buf + strlen(buf) + 1) = '\0';
	 *(buf + strlen(buf)) = '\r';
      }
   } while (!feof(fl));

   fclose(fl);

   return(0);
}




/* clear some of the the working variables of a char */
void	reset_char(struct char_data *ch)
{
   int	i;

   for (i = 0; i < MAX_WEAR; i++) /* Initialisering */
      ch->equipment[i] = 0;

   ch->followers = 0;
   ch->master = 0;
   /*	ch->in_room = NOWHERE; Used for start in room */
   ch->carrying = 0;
   ch->next = 0;
   ch->next_fighting = 0;
   ch->next_in_room = 0;
   ch->specials.fighting = 0;
   ch->specials.position = POSITION_STANDING;
   ch->specials.default_pos = POSITION_STANDING;
   ch->specials.carry_weight = 0;
   ch->specials.carry_items = 0;

   if (GET_HIT(ch) <= 0)
      GET_HIT(ch) = 1;
   if (GET_MOVE(ch) <= 0)
      GET_MOVE(ch) = 1;
   if (GET_MANA(ch) <= 0)
      GET_MANA(ch) = 1;
}



/* clear ALL the working variables of a char and do NOT free any space alloc'ed*/
void	clear_char(struct char_data *ch)
{
   memset((char *)ch, (char)'\0', (int)sizeof(struct char_data));

   ch->in_room = NOWHERE;
   ch->specials.was_in_room = NOWHERE;
   ch->specials.position = POSITION_STANDING;
   ch->specials.default_pos = POSITION_STANDING;

   GET_AC(ch) = 100; /* Basic Armor */
   if (ch->points.max_mana < 100) {
      ch->points.max_mana = 100;
   }
}


void	clear_object(struct obj_data *obj)
{
   memset((char *)obj, (char)'\0', (int)sizeof(struct obj_data));

   obj->item_number = -1;
   obj->in_room	  = NOWHERE;
}




/* initialize a new character only if class is set */
void	init_char(struct char_data *ch)
{
   int	i;

   /* *** if this is our first player --- he be God *** */

   if (top_of_p_table < 0) {
      GET_EXP(ch) = 7000000;
      GET_LEVEL(ch) = LEVEL_IMPL;

      ch->points.max_hit = 500;
      ch->points.max_mana = 100;
      ch->points.max_move = 82;
   }

   set_title(ch);

   ch->player.short_descr = 0;
   ch->player.long_descr = 0;
   ch->player.description = 0;

   ch->player.hometown = number(1, 4);

   ch->player.time.birth = time(0);
   ch->player.time.played = 0;
   ch->player.time.logon = time(0);

   for (i = 0; i < MAX_TOUNGE; i++)
      ch->player.talks[i] = 0;

   GET_STR(ch) = 9;
   GET_INT(ch) = 9;
   GET_WIS(ch) = 9;
   GET_DEX(ch) = 9;
   GET_CON(ch) = 9;

   /* make favors for sex */
   if (ch->player.sex == SEX_MALE) {
      ch->player.weight = number(120, 180);
      ch->player.height = number(160, 200);
   } else {
      ch->player.weight = number(100, 160);
      ch->player.height = number(150, 180);
   }

   ch->points.max_mana = 100;
   ch->points.mana = GET_MAX_MANA(ch);
   ch->points.hit = GET_MAX_HIT(ch);
   ch->points.max_move = 82;
   ch->points.move = GET_MAX_MOVE(ch);
   ch->points.armor = 100;

   ch->specials2.idnum = ++top_idnum;

   if (!ch->skills)
      CREATE(ch->skills, byte, MAX_SKILLS);

   for (i = 0; i < MAX_SKILLS; i++) {
      if (GET_LEVEL(ch) < LEVEL_IMPL)
	 SET_SKILL(ch, i, 0)
      else
	 SET_SKILL(ch, i, 100);
   }

   ch->specials.affected_by = 0;

   for (i = 0; i < 5; i++)
      ch->specials2.apply_saving_throw[i] = 0;

   for (i = 0; i < 3; i++)
      GET_COND(ch, i) = (GET_LEVEL(ch) == LEVEL_IMPL ? -1 : 24);
}



/* returns the real number of the room with given virtual number */
int	real_room(int virtual)
{
   int	bot, top, mid;

   bot = 0;
   top = top_of_world;

   /* perform binary search on world-table */
   for (; ; ) {
      mid = (bot + top) / 2;

      if ((world + mid)->number == virtual)
	 return(mid);
      if (bot >= top) {
	 if (!mini_mud)
	    fprintf(stderr, "Room %d does not exist in database\n", virtual);
	 return(-1);
      }
      if ((world + mid)->number > virtual)
	 top = mid - 1;
      else
	 bot = mid + 1;
   }
}






/* returns the real number of the monster with given virtual number */
int	real_mobile(int virtual)
{
   int	bot, top, mid;

   bot = 0;
   top = top_of_mobt;

   /* perform binary search on mob-table */
   for (; ; ) {
      mid = (bot + top) / 2;

      if ((mob_index + mid)->virtual == virtual)
	 return(mid);
      if (bot >= top)
	 return(-1);
      if ((mob_index + mid)->virtual > virtual)
	 top = mid - 1;
      else
	 bot = mid + 1;
   }
}






/* returns the real number of the object with given virtual number */
int	real_object(int virtual)
{
   int	bot, top, mid;

   bot = 0;
   top = top_of_objt;

   /* perform binary search on obj-table */
   for (; ; ) {
      mid = (bot + top) / 2;

      if ((obj_index + mid)->virtual == virtual)
	 return(mid);
      if (bot >= top)
	 return(-1);
      if ((obj_index + mid)->virtual > virtual)
	 top = mid - 1;
      else
	 bot = mid + 1;
   }
}

