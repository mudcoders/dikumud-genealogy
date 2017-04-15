/* ************************************************************************
*   File: scheck.c                                      Part of CircleMUD *
*  Usage: Circle world/mob/obj file syntax checker			  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define __DB_C__

#define log(msg) puts(msg)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "../structs.h"
#include "../db.h"
#include "../utils.h"

/**************************************************************************
*  declarations of most of the 'global' variables                         *
************************************************************************ */

struct room_data *world = NULL;		/* array of rooms		*/
int	top_of_world = 0;		/* ref to top element of world	*/

struct char_data *character_list = NULL; /* global linked list of chars	*/
struct index_data *mob_index;		/* index table for mobile file	*/
struct char_data *mob_proto;		/* prototypes for mobs		*/
int	top_of_mobt = 0;		/* top of mobile index table	*/

struct obj_data *object_list = NULL;    /* global linked list of objs	*/
struct index_data *obj_index;		/* index table for object file	*/
struct obj_data *obj_proto;		/* prototypes for objs		*/
int	top_of_objt = 0;		/* top of object index table	*/

struct zone_data *zone_table;		/* zone table			*/
int	top_of_zone_table = 0;		/* top element of zone tab	*/
struct message_list fight_messages[MAX_MESSAGES]; /* fighting messages	*/

struct player_index_element *player_table = NULL; /* index to plr file	*/
FILE *player_fl = NULL;			/* file desc of player file	*/
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

char	*credits = NULL;		/* game credits			*/
char	*news = NULL;			/* mud news			*/
char	*motd = NULL;			/* message of the day - mortals */
char	*imotd = NULL;			/* message of the day - immorts */
char	*help = NULL;			/* help screen			*/
char	*info = NULL;			/* info page			*/
char	*wizlist = NULL;		/* list of higher gods		*/
char	*immlist = NULL;		/* list of peon gods		*/
char	*background = NULL;		/* background story		*/
char	*handbook = NULL;		/* handbook for new immortals	*/
char	*policies = NULL;		/* policies page		*/

FILE *help_fl = NULL;			/* file for help text		*/
struct help_index_element *help_index = 0; /* the help table		*/
int	top_of_helpt;			/* top of help index table	*/

struct time_info_data time_info;	/* the infomation about the time    */
struct weather_data weather_info;	/* the infomation about the weather */
struct player_special_data dummy_mob;	/* dummy spec area for mobs	*/
struct reset_q_type reset_q;		/* queue of zones to be reset	*/

/* local functions */
void	setup_dir(FILE *fl, int room, int dir);
void	index_boot(int mode);
void	discrete_load(FILE *fl, int mode);
void	parse_room(FILE *fl, int virtual_nr);
void	parse_mobile(FILE *mob_f, int nr);
char	*parse_object(FILE *obj_f, int nr);
void	load_zones(FILE *fl);
void	assign_mobiles(void);
void	assign_objects(void);
void	assign_rooms(void);
void	assign_the_shopkeepers(void);
void	build_player_index(void);
void	char_to_store(struct char_data *ch, struct char_file_u *st);
void	store_to_char(struct char_file_u *st, struct char_data *ch);
int	is_empty(int zone_nr);
int	file_to_string(char *name, char *buf);
int	file_to_string_alloc(char *name, char **buf);
void	check_start_rooms(void);
void	renum_world(void);
void	renum_zone_table(void);
void	reset_time(void);
void	clear_char(struct char_data *ch);

/* external functions */
void	load_messages(void);
void	weather_and_time(int mode);
void	mag_assign_spells(void);
void	boot_social_messages(void);
void	update_obj_file(void); /* In objsave.c */
void	sort_commands(void);
void	load_banned(void);
void	Read_Invalid_List(void);
struct help_index_element *build_help_index(FILE *fl, int *num);


/*************************************************************************
*  routines for booting the system                                       *
*********************************************************************** */

/* body of the booting system */
void	main()
{
   int	i;

   log("Boot db -- BEGIN.");

   log("Loading zone table.");
   index_boot(DB_BOOT_ZON);

   log("Loading rooms.");
   index_boot(DB_BOOT_WLD);

   log("Renumbering rooms.");
   renum_world();

   log("Loading mobs and generating index.");
   index_boot(DB_BOOT_MOB);

   log("Loading objs and generating index.");
   index_boot(DB_BOOT_OBJ);

   log("Renumbering zone table.");
   renum_zone_table();

   log("Boot db -- DONE.");
}



/* function to count how many hash-mark delimited records exist in a file */
int	count_hash_records(FILE *fl)
{
   char	buf[128];
   int	count = 0;

   while (fgets(buf, 128, fl))
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

   rewind(index);
   fscanf(index, "%s\n", buf1);
   while (*buf1 != '$') {
      sprintf(buf2, "%s/%s", prefix, buf1);
      if (!(db_file = fopen(buf2, "r"))) {
	 perror(buf2);
	 exit(1);
      }

      switch (mode) {
      case DB_BOOT_WLD	: 
      case DB_BOOT_OBJ	: 
      case DB_BOOT_MOB	: discrete_load(db_file, mode); break;
      case DB_BOOT_ZON	: load_zones(db_file); break;
      }

      fclose(db_file);
      fscanf(index, "%s\n", buf1);
   }
}


void	discrete_load(FILE *fl, int mode)
{
   int	nr = -1, last = 0;
   char	line[256];

   char *modes[] = { "world", "mob", "obj" };

   for (; ; ) {
      if (mode != DB_BOOT_OBJ || nr < 0)
	 if (!get_line(fl, line)) {
	    fprintf(stderr, "Format error after %s #%d\n", modes[mode], nr);
	    exit(1);
	 }

      if (*line == '$')
	 return;

      if (*line == '#') {
	 last = nr;
	 if (sscanf(line, "#%d", &nr) != 1) {
	    fprintf(stderr, "Format error after %s #%d\n", modes[mode], last);
	    exit(1);
	 }

	 if (nr >= 99999) 
	    return;
	 else switch (mode) {
	    case DB_BOOT_WLD:	parse_room(fl, nr); break;
	    case DB_BOOT_MOB:	parse_mobile(fl, nr); break;
	    case DB_BOOT_OBJ:	strcpy(line, parse_object(fl, nr)); break;
	 }
      } else {
         fprintf(stderr, "Format error in %s file near %s #%d\n",
		 modes[mode], modes[mode], nr);
	 fprintf(stderr, "Offending line: '%s'\n", line);
	 exit(1);
      }
   }
}



/* load the rooms */
void	parse_room(FILE *fl, int virtual_nr)
{
   static int	room_nr = 0, zone = 0;
   int	t[10], i;
   char	line[256];
   struct extra_descr_data *new_descr;

   sprintf(buf2, "room #%d", virtual_nr);

   if (virtual_nr <= (zone ? zone_table[zone-1].top : -1)) {
      fprintf(stderr, "Room #%d is below zone %d.\n", virtual_nr, zone);
      exit(1);
   }

   while (virtual_nr > zone_table[zone].top)
      if (++zone > top_of_zone_table) {
	 fprintf(stderr, "Room %d is outside of any zone.\n", virtual_nr);
	 exit(1);
      }

   world[room_nr].zone = zone;
   world[room_nr].number = virtual_nr;
   world[room_nr].name = fread_string(fl, buf2);
   world[room_nr].description = fread_string(fl, buf2);

   if (!get_line(fl, line) || sscanf(line, " %d %d %d ", t, t+1, t+2) != 3) {
      fprintf(stderr, "Format error in room #%d\n", virtual_nr);
      exit(1);
   }

   /* t[0] is the zone number; ignored with the zone-file system */
   world[room_nr].room_flags = t[1];
   world[room_nr].sector_type = t[2];

   world[room_nr].func = NULL;
   world[room_nr].contents = NULL;
   world[room_nr].people = NULL;
   world[room_nr].light = 0; /* Zero light sources */

   for (i = 0; i < NUM_OF_DIRS; i++)
      world[room_nr].dir_option[i] = NULL;

   world[room_nr].ex_description = NULL;

   sprintf(buf, "Format error in room #%d (expecting D/E/S)", virtual_nr);

   for (; ; ) {
      if (!get_line(fl, line)) {
	 fprintf(stderr, "%s\n", buf);
	 exit(1);
      }

      switch(*line) {
      case 'D':
	 setup_dir(fl, room_nr, atoi(line + 1));
	 break;
      case 'E':
	 CREATE(new_descr, struct extra_descr_data, 1);
	 new_descr->keyword = fread_string(fl, buf2);
	 new_descr->description = fread_string(fl, buf2);
	 new_descr->next = world[room_nr].ex_description;
	 world[room_nr].ex_description = new_descr;
	 break;
      case 'S': /* end of room */
	 top_of_world = room_nr++;
	 return;
	 break;
      default:
	 fprintf(stderr, "%s\n", buf);
	 exit(1);
	 break;
      }
   }
}



/* read direction data */
void	setup_dir(FILE *fl, int room, int dir)
{
   int	t[5];
   char	line[256];

   sprintf(buf2, "room #%d, direction D%d", world[room].number, dir);

   CREATE(world[room].dir_option[dir], struct room_direction_data, 1);
   world[room].dir_option[dir]->general_description = fread_string(fl, buf2);
   world[room].dir_option[dir]->keyword = fread_string(fl, buf2);

   if (!get_line(fl, line)) {
      fprintf(stderr, "Format error, %s\n", buf2);
      exit(1);
   }

   if (sscanf(line, " %d %d %d ", t, t+1, t+2) != 3) {
      fprintf(stderr, "Format error, %s\n", buf2);
      exit(1);
   }

   if (t[0] == 1)
      world[room].dir_option[dir]->exit_info = EX_ISDOOR;
   else if (t[0] == 2)
      world[room].dir_option[dir]->exit_info = EX_ISDOOR | EX_PICKPROOF;
   else
      world[room].dir_option[dir]->exit_info = 0;

   world[room].dir_option[dir]->key = t[1];
   world[room].dir_option[dir]->to_room = t[2];
}


/* resolve all vnums into rnums in the world */
void	renum_world(void)
{
   register int	room, door;

   for (room = 0; room <= top_of_world; room++)
      for (door = 0; door < NUM_OF_DIRS; door++)
	 if (world[room].dir_option[door])
	    if (world[room].dir_option[door]->to_room != NOWHERE)
	       world[room].dir_option[door]->to_room = 
	           real_room(world[room].dir_option[door]->to_room);
}


/* resulve vnums into rnums in the zone reset tables */
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



void	parse_mobile(FILE *mob_f, int nr)
{
   static int	i = 0;
   int	j, t[10];
   char	line[256], *tmpptr, letter;

   mob_index[i].virtual = nr;
   mob_index[i].number  = 0;
   mob_index[i].func    = NULL;

   clear_char(mob_proto + i);

   mob_proto[i].player_specials = &dummy_mob;
   sprintf(buf2, "mob vnum %d", nr);

   /***** String data *** */
   mob_proto[i].player.name = fread_string(mob_f, buf2);
   tmpptr = mob_proto[i].player.short_descr = fread_string(mob_f, buf2);
   mob_proto[i].player.long_descr = fread_string(mob_f, buf2);
   mob_proto[i].player.description = fread_string(mob_f, buf2);
   mob_proto[i].player.title = NULL;

   /* *** Numeric data *** */
   get_line(mob_f, line);
   sscanf(line, "%d %d %d %c", t, t+1, t+2, &letter);
   MOB_FLAGS(mob_proto + i) = t[0];
   SET_BIT(MOB_FLAGS(mob_proto + i), MOB_ISNPC);
   AFF_FLAGS(mob_proto + i) = t[1];
   GET_ALIGNMENT(mob_proto + i) = t[2];

   switch (letter) {
   case 'S': /* Simple monsters */
      mob_proto[i].real_abils.str = 11;
      mob_proto[i].real_abils.intel = 11;
      mob_proto[i].real_abils.wis = 11;
      mob_proto[i].real_abils.dex = 11;
      mob_proto[i].real_abils.con = 11;

      get_line(mob_f, line);
      if (sscanf(line, " %d %d %d %dd%d+%d %dd%d+%d ",
	     t, t+1, t+2, t+3, t+4, t+5, t+6, t+7, t+8) != 9) {
	 fprintf(stderr,"Format error in mob #%d, first line after S flag\n"
			"...expecting line of form '# # # #d#+# #d#+#'\n", nr);
	 exit(1);
      }

      GET_LEVEL(mob_proto + i) = t[0];
      mob_proto[i].points.hitroll = 20 - t[1];
      mob_proto[i].points.armor = 10 * t[2];

      /* max hit = 0 is a flag that H, M, V is xdy+z */
      mob_proto[i].points.max_hit = 0;
      mob_proto[i].points.hit = t[3];
      mob_proto[i].points.mana = t[4];
      mob_proto[i].points.move = t[5];

      mob_proto[i].points.max_mana = 10;
      mob_proto[i].points.max_move = 50;

      mob_proto[i].mob_specials.damnodice = t[6];
      mob_proto[i].mob_specials.damsizedice = t[7];
      mob_proto[i].points.damroll = t[8];

      get_line(mob_f, line);
      sscanf(line, " %d %d ", t, t+1);
      GET_GOLD(mob_proto + i) = t[0];
      GET_EXP(mob_proto + i) = t[1];

      get_line(mob_f, line);
      if (sscanf(line, " %d %d %d %d ", t, t+1, t+2, t+3) == 4)
	 mob_proto[i].mob_specials.attack_type = t[3];
      else
	 mob_proto[i].mob_specials.attack_type = 0;

      mob_proto[i].char_specials.position = t[0];
      mob_proto[i].mob_specials.default_pos = t[1];
      mob_proto[i].player.sex = t[2];

      mob_proto[i].player.class = 0;
      mob_proto[i].player.weight = 200;
      mob_proto[i].player.height = 198;

      break;
   default:
      fprintf(stderr, "Unsupported mob type %c in mob #%d\n", letter, nr);
      exit(1);
      break;
   }

   mob_proto[i].aff_abils = mob_proto[i].real_abils;

   for (j = 0; j < NUM_WEARS; j++)
      mob_proto[i].equipment[j] = NULL;

   mob_proto[i].nr = i;
   mob_proto[i].desc = NULL;

   top_of_mobt = i++;
}




/* read all objects from obj file; generate index and prototypes */
char	*parse_object(FILE *obj_f, int nr)
{
   static int	i = 0;
   static char	line[256];
   int	t[10], j;
   char	*tmpptr;
   struct extra_descr_data *new_descr;

   obj_index[i].virtual = nr;
   obj_index[i].number  = 0;
   obj_index[i].func    = NULL;

   clear_object(obj_proto + i);
   obj_proto[i].in_room = NOWHERE;
   obj_proto[i].item_number = i;

   sprintf(buf2, "object #%d", nr);

   /* *** string data *** */
   if ((obj_proto[i].name = fread_string(obj_f, buf2)) == NULL) {
      fprintf(stderr, "Null obj name or format error at or near %s\n", buf2);
      exit(1);
   }

   tmpptr = obj_proto[i].short_description = fread_string(obj_f, buf2);
   tmpptr = obj_proto[i].description = fread_string(obj_f, buf2);
   if (tmpptr && *tmpptr)
      *tmpptr = UPPER(*tmpptr);
   obj_proto[i].action_description = fread_string(obj_f, buf2);

   /* *** numeric data *** */
   if (!get_line(obj_f, line) || sscanf(line, " %d %d %d", t, t+1, t+2) != 3) {
      fprintf(stderr, "Format error in first numeric line, %s\n", buf2);
      exit(1);
   }
   obj_proto[i].obj_flags.type_flag = t[0];
   obj_proto[i].obj_flags.extra_flags = t[1];
   obj_proto[i].obj_flags.wear_flags = t[2];

   if (!get_line(obj_f,line) || sscanf(line, "%d %d %d %d", t,t+1,t+2,t+3)!=4){
      fprintf(stderr, "Format error in second numeric line, %s\n", buf2);
      exit(1);
   }
   obj_proto[i].obj_flags.value[0] = t[0];
   obj_proto[i].obj_flags.value[1] = t[1];
   obj_proto[i].obj_flags.value[2] = t[2];
   obj_proto[i].obj_flags.value[3] = t[3];

   if (!get_line(obj_f, line) || sscanf(line, "%d %d %d", t, t+1, t+2) != 3) {
      fprintf(stderr, "Format error in third numeric line, %s\n", buf2);
      exit(1);
   }
   obj_proto[i].obj_flags.weight = t[0];
   obj_proto[i].obj_flags.cost = t[1];
   obj_proto[i].obj_flags.cost_per_day = t[2];

   /* *** extra descriptions and affect fields *** */

   for (j = 0; j < MAX_OBJ_AFFECT; j++) {
      obj_proto[i].affected[j].location = APPLY_NONE;
      obj_proto[i].affected[j].modifier = 0;
   }

   strcat(buf2, ", after numeric constants (expecting E/A/#xxx)");
   j = 0;

   for (; ; ) {
      if (!get_line(obj_f, line)) {
	 fprintf(stderr, "Format error in %s\n", buf2);
	 exit(1);
      }

      switch(*line) {
      case 'E':
	 CREATE(new_descr, struct extra_descr_data, 1);
	 new_descr->keyword = fread_string(obj_f, buf2);
	 new_descr->description = fread_string(obj_f, buf2);
	 new_descr->next = obj_proto[i].ex_description;
	 obj_proto[i].ex_description = new_descr;
	 break;
      case 'A':
	 if (j >= MAX_OBJ_AFFECT) {
	    fprintf(stderr, "Too many A fields (%d max), %s\n", MAX_OBJ_AFFECT, buf2);
	    exit(1);
	 }
	 get_line(obj_f, line);
	 sscanf(line, " %d %d ", t, t+1);
	 obj_proto[i].affected[j].location = t[0];
	 obj_proto[i].affected[j].modifier = t[1];
	 j++;
	 break;
      case '$': case '#':
	 top_of_objt = i++;
	 return line;
	 break;
      default:
	 fprintf(stderr, "Format error in %s\n", buf2);
	 exit(1);
	 break;
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
	       exit(1);
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


/* read and allocate space for a '~'-terminated string from a given file */
char *fread_string(FILE *fl, char *error)
{
   static char buf[MAX_STRING_LENGTH], tmp[512], *rslt;
   register char *point;
   int done=0, length=0, templength=0;

   bzero(buf, MAX_STRING_LENGTH);
   do {
      if (!fgets(tmp, 512, fl)) {
	 fprintf(stderr, "SYSERR: fread_string: format error at or near %s\n",
		 error);
	 exit(1);
      }

      /* If there is a '~', end the string; else an "\r\n" over the '\n'. */
      if ((point = strchr(tmp, '~')) != NULL) {
	 *point = '\0';
	 done = 1;
      } else {
	 point = tmp + strlen(tmp) - 1;
         *(point++) = '\r';
         *(point++) = '\n';
         *point = '\0';
      }

      templength = strlen(tmp);

      if (length + templength >= MAX_STRING_LENGTH) {
	 log("SYSERR: fread_string: string too large (db.c)");
	 exit(1);
      } else {
	 strcat(buf+length, tmp);
	 length += templength;
      }
   } while (!done);

   if (strlen(buf) > 0) {
      rslt = buf;
   } else
      rslt = NULL;

  return rslt;
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




/* clear ALL the working variables of a char; do NOT free any space alloc'ed */
void	clear_char(struct char_data *ch)
{
   bzero((char *)ch, sizeof(struct char_data));

   ch->in_room = NOWHERE;
   GET_WAS_IN(ch) = NOWHERE;
   GET_POS(ch) = POS_STANDING;
   ch->mob_specials.default_pos = POS_STANDING;

   GET_AC(ch) = 100; /* Basic Armor */
   if (ch->points.max_mana < 100)
      ch->points.max_mana = 100;
}


void	clear_object(struct obj_data *obj)
{
   bzero((char *)obj, sizeof(struct obj_data));

   obj->item_number = -1;
   obj->in_room	= NOWHERE;
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

/* get_line reads the next non-blank line off of the input stream.
 * The newline character is removed from the input.  Lines which begin
 * with '*' are considered to be comments.
 */
int	get_line(FILE *fl, char *buf)
{
   char temp[256];

   do {
      fgets(temp, 256, fl);
      if (*temp)
	 temp[strlen(temp) - 1] = '\0';
   } while (!feof(fl) && (*temp == '*' || !*temp));

   if (feof(fl))
      return 0;
   else {
      strcpy(buf, temp);
      return 1;
   }
}
