/***************************************************************************
 *  file: db.c , Database module.                          Part of DIKUMUD *
 *  Usage: Loading/Saving chars, booting world, resetting etc.             *
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

#include <stdlib.h>
#include <stdio.h>
extern int _filbuf(FILE *);
#include <string.h>
#include <memory.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "utils.h"
#include "db.h"
#include "handler.h"
#include "limits.h"
#include "spells.h"

#define NEW_ZONE_SYSTEM

/*
 * Sizes.
 */
#define MAX_ZONE      96
#define MAX_INDEX   1024
#define MAX_ROOM    3072
#define	MAX_RESET   4096

/**************************************************************************
*  declarations of most of the 'global' variables                         *
************************************************************************ */

struct room_data world_array[MAX_ROOM]; /* array of rooms                  */
struct room_data *world = world_array;
#define world   world_array
int top_of_world = 0;                 /* ref to the top element of world */
struct obj_data  *object_list = 0;    /* the global linked list of obj's */
struct char_data *character_list = 0; /* global l-list of chars          */
struct ban_t *ban_list=0;             /* list of banned site--sigh       */

struct zone_data zone_table_array[MAX_ZONE];
struct zone_data *zone_table = zone_table_array;
#define zone_table  zone_table_array
		      /* table of reset data             */
int top_of_zone_table = 0;
struct message_list fight_messages[MAX_MESSAGES]; /* fighting messages   */

char greetings[MAX_STRING_LENGTH];    /* the greeting screen             */
char credits[MAX_STRING_LENGTH];      /* the Credits List                */
char news[MAX_STRING_LENGTH];         /* the news                        */
char motd[MAX_STRING_LENGTH];         /* the messages of today           */
char story[MAX_STRING_LENGTH];        /* the game story                  */
char help[MAX_STRING_LENGTH];         /* the main help page              */
char info[MAX_STRING_LENGTH];         /* the info text                   */
char wizlist[MAX_STRING_LENGTH];      /* the wizlist                     */



FILE *mob_f,                          /* file containing mob prototypes  */
     *obj_f,                          /* obj prototypes                  */
     *help_fl;                        /* file for help texts (HELP <kwd>)*/

struct index_data mob_index_array[MAX_INDEX];
struct index_data *mob_index = mob_index_array;
#define mob_index   mob_index_array
		      /* index table for mobile file     */
struct index_data obj_index_array[MAX_INDEX];
struct index_data *obj_index = obj_index_array;
		      /* index table for object file     */
#define obj_index   obj_index_array
struct help_index_element *help_index = 0;

int top_of_mobt = 0;                  /* top of mobile index table       */
int top_of_objt = 0;                  /* top of object index table       */
int top_of_helpt = 0;                 /* top of help index table         */

struct time_info_data time_info;    /* the infomation about the time   */
struct weather_data weather_info;   /* the infomation about the weather */



/* local procedures */
void boot_zones(void);
void setup_dir(FILE *fl, int room, int dir);

void boot_world(void);
struct index_data *generate_indices(FILE *fl, int *top,
	struct index_data *index);
int is_empty(int zone_nr);
void reset_zone(int zone);
int file_to_string(char *name, char *buf);
void renum_world(void);
void renum_zone_table(void);
void reset_time(void);
void clear_char(struct char_data *ch);

/* external refs */
extern struct descriptor_data *descriptor_list;
void load_messages(void);
void weather_and_time ( int mode );
void assign_spell_pointers ( void );
int dice(int number, int size);
int number(int from, int to);
void boot_social_messages(void);
void boot_pose_messages(void);
struct help_index_element *build_help_index(FILE *fl, int *num);


/*************************************************************************
*  routines for booting the system                                       *
*********************************************************************** */

/* body of the booting system */
void boot_db(void)
{
    int i;

    reset_time();

    log( "Reading aux files." );
    file_to_string(GREETINGS_FILE, greetings);
    file_to_string(NEWS_FILE, news);
    file_to_string(CREDITS_FILE, credits);
    file_to_string(MOTD_FILE, motd);
    strcat( motd, "\n\rPress RETURN to continue: " );
    file_to_string(STORY_FILE, story);
    file_to_string(HELP_PAGE_FILE, help);
    file_to_string(INFO_FILE, info);
    file_to_string(WIZLIST_FILE, wizlist);

    log("Opening mobile, object and help files.");
    if (!(mob_f = fopen(MOB_FILE, "r")))
    {
	perror( MOB_FILE );
	exit( 1 );
    }

    if (!(obj_f = fopen(OBJ_FILE, "r")))
    {
	perror( OBJ_FILE );
	exit( 1 );
    }

    if (!(help_fl = fopen(HELP_KWRD_FILE, "r")))
    {
	perror( HELP_KWRD_FILE );
	exit( 1 );
    }
    help_index = build_help_index(help_fl, &top_of_helpt);


    log( "Loading tinyworld." );
    boot_zones();
    boot_world();
    renum_world();
    generate_indices(mob_f, &top_of_mobt, mob_index);
    generate_indices(obj_f, &top_of_objt, obj_index);
    renum_zone_table();

    log("Loading messages.");
    load_messages();
    boot_social_messages();
    boot_pose_messages();

    log( "Assigning function pointers." );
    assign_mobiles();
    assign_objects();
    assign_rooms();
    assign_spell_pointers();

    fprintf( stderr, "\n[ Room  Room]\t{Level}\t  Author\tZone\n" );
    for (i = 0; i <= top_of_zone_table; i++)
    {
	fprintf(stderr, "[%5d %5d]\t%s.\n",
	    (i ? (zone_table[i - 1].top + 1) : 0),
	    zone_table[i].top,
	    zone_table[i].name);
	reset_zone(i);
    }
    fprintf( stderr, "\n" );
}


/* reset the time in the game from file */
void reset_time(void)
{
    long beginning_of_time = 650336715;

    struct time_info_data mud_time_passed(time_t t2, time_t t1);

    time_info = mud_time_passed(time(0), beginning_of_time);

    switch(time_info.hours){
	case 0 :
	case 1 :
	case 2 :
	case 3 :
	case 4 : 
	{
	    weather_info.sunlight = SUN_DARK;
	    break;
	}
	case 5 :
	{
	    weather_info.sunlight = SUN_RISE;
	    break;
	}
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
	{
	    weather_info.sunlight = SUN_LIGHT;
	    break;
	}
	case 21 :
	{
	    weather_info.sunlight = SUN_SET;
	    break;
	}
	case 22 :
	case 23 :
	default :
	{
	    weather_info.sunlight = SUN_DARK;
	    break;
	}
    }

    sprintf(log_buf, "Current Gametime: %dH %dD %dM %dY.",
	    time_info.hours, time_info.day,
	    time_info.month, time_info.year);
    log(log_buf);

    weather_info.pressure = 960;
    if ((time_info.month>=7)&&(time_info.month<=12))
	weather_info.pressure += dice(1,50);
    else
	weather_info.pressure += dice(1,80);

    weather_info.change = 0;

    if (weather_info.pressure<=980)
	weather_info.sky = SKY_LIGHTNING;
    else if (weather_info.pressure<=1000)
	weather_info.sky = SKY_RAINING;
    else if (weather_info.pressure<=1020)
	weather_info.sky = SKY_CLOUDY;
    else weather_info.sky = SKY_CLOUDLESS;
}



/* generate index table for object or monster file */
struct index_data *generate_indices(FILE *fl, int *top,
	struct index_data *index)
{
    int i = 0;
    char buf[82];

    rewind(fl);

    for (;;)
    {
	if (fgets(buf, 81, fl))
	{
	    if (*buf == '#')
	    {
		/* allocate new cell */
		if ( i >= MAX_INDEX )
		{
		    perror( "Too many indexes" );
		    exit( 1 );
		}
		
		sscanf(buf, "#%d", &index[i].virtual);
		index[i].pos = ftell(fl);
		index[i].number = 0;
		index[i].func = 0;
		i++;
	    }
	    else 
		if (*buf == '$')    /* EOF */
		    break;
	}
	else
	{
	    perror("generate indices");
	    exit( 1 );
	}
    }
    *top = i - 2;
    return(index);
}




/* load the rooms */
void boot_world(void)
{
    FILE *fl;
    int room_nr = 0, zone = 0, virtual_nr, flag, tmp;
    char *temp, chk[50];
    struct extra_descr_data *new_descr;

    character_list = 0;
    object_list = 0;
    
    if (!(fl = fopen(WORLD_FILE, "r")))
    {
	perror("fopen");
	log("boot_world: could not open world file.");
	exit( 1 );
    }

    do
    {
	fscanf(fl, " #%d\n", &virtual_nr);

	temp = fread_string(fl);
	if ( ( flag = (*temp != '$') ) != 0 )  /* a new record to be read */
	{
	    if (room_nr >= MAX_ROOM)
	    {
		perror( "Too many rooms" );
		exit( 1 );
	    }
	    world[room_nr].number = virtual_nr;
	    world[room_nr].name = temp;
	    world[room_nr].description = fread_string(fl);

	    if (top_of_zone_table >= 0)
	    {
		fscanf(fl, " %*d ");

		/* OBS: Assumes ordering of input rooms */

		if (world[room_nr].number <=
		    (zone ? zone_table[zone-1].top : -1))
		{
		    fprintf(stderr, "Room nr %d is below zone %d.\n",
			room_nr, zone);
		    exit( 1 );
		}
		while (world[room_nr].number > zone_table[zone].top)
		    if (++zone > top_of_zone_table)
		    {
			fprintf(stderr, "Room %d is outside of any zone.\n",
			    virtual_nr);
			exit( 1 );
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

	    for (;;)
	    {
		fscanf(fl, " %s \n", chk);

		if (*chk == 'D')  /* direction field */
		    setup_dir(fl, room_nr, atoi(chk + 1));
		else if (*chk == 'E')  /* extra description field */
		{
		    CREATE(new_descr, struct extra_descr_data, 1);
		    new_descr->keyword = fread_string(fl);
		    new_descr->description = fread_string(fl);
		    new_descr->next = world[room_nr].ex_description;
		    world[room_nr].ex_description = new_descr;
		}
		else if (*chk == 'S')   /* end of current room */
		    break;
	    }
			
	    room_nr++;
	}
    }
    while (flag);

    free(temp); /* cleanup the area containing the terminal $  */

    fclose(fl);
    top_of_world = --room_nr;
}



/* read direction data */
void setup_dir(FILE *fl, int room, int dir)
{
    int tmp;

    CREATE(world[room].dir_option[dir], 
	struct room_direction_data, 1);

    world[room].dir_option[dir]->general_description =
	fread_string(fl);
    world[room].dir_option[dir]->keyword = fread_string(fl);

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




void renum_world(void)
{
    register int room, door;

    for (room = 0; room <= top_of_world; room++)
	for (door = 0; door <= 5; door++)
	    if (world[room].dir_option[door])
	 if (world[room].dir_option[door]->to_room != NOWHERE)
	     world[room].dir_option[door]->to_room =
		 real_room(world[room].dir_option[door]->to_room);
}


#ifdef NEW_ZONE_SYSTEM

void renum_zone_table(void)
{
    int zone, comm;

    for (zone = 0; zone <= top_of_zone_table; zone++)
	for (comm = 0; zone_table[zone].cmd[comm].command != 'S'; comm++)
	    switch(zone_table[zone].cmd[comm].command)
	    {
		case 'M':
		    zone_table[zone].cmd[comm].arg1 =
			real_mobile(zone_table[zone].cmd[comm].arg1);
		    zone_table[zone].cmd[comm].arg3 = 
			real_room(zone_table[zone].cmd[comm].arg3);
		break;
		case 'O':
		    zone_table[zone].cmd[comm].arg1 = 
			real_object(zone_table[zone].cmd[comm].arg1);
		    if (zone_table[zone].cmd[comm].arg3 != NOWHERE)
			zone_table[zone].cmd[comm].arg3 =
			real_room(zone_table[zone].cmd[comm].arg3);
		break;
		case 'G':
		    zone_table[zone].cmd[comm].arg1 =
			real_object(zone_table[zone].cmd[comm].arg1);
		break;
		case 'E':
		    zone_table[zone].cmd[comm].arg1 =
			real_object(zone_table[zone].cmd[comm].arg1);
		break;
		case 'P':
		    zone_table[zone].cmd[comm].arg1 =
			real_object(zone_table[zone].cmd[comm].arg1);
		    zone_table[zone].cmd[comm].arg3 =
			real_object(zone_table[zone].cmd[comm].arg3);
		break;                  
		case 'D':
		    zone_table[zone].cmd[comm].arg1 =
			real_room(zone_table[zone].cmd[comm].arg1);
		break;
	    }
}

#endif



#ifdef NEW_ZONE_SYSTEM

/* load the zone table and command tables */
void boot_zones(void)
{
    FILE *fl;
    int zon = 0, tmp;
    char *check, buf[81];
    static struct reset_com reset_tab[MAX_RESET];
    static int reset_top = 0;

    if (!(fl = fopen(ZONE_FILE, "r")))
    {
	perror( ZONE_FILE );
	exit( 1 );
    }

    for (;;)
    {
	fscanf(fl, " #%*d\n");
	check = fread_string(fl);

	if (*check == '$')
	    break;      /* end of file */

	/* alloc a new zone */

	if ( zon >= MAX_ZONE )
	{
	    perror( "Too many zones" );
	    exit( 1 );
	}

	zone_table[zon].name = check;
	fscanf(fl, " %d ", &zone_table[zon].top);
	fscanf(fl, " %d ", &zone_table[zon].lifespan);
	fscanf(fl, " %d ", &zone_table[zon].reset_mode);

	/* read the command table */
	zone_table[zon].cmd = &reset_tab[reset_top];

	for ( ;; )
	{
	    if ( reset_top >= MAX_RESET )
	    {
		perror( "Too many zone resets" );
		exit( 1 );
	    }

	    fscanf(fl, " "); /* skip blanks */
	    fscanf(fl, "%c", 
		&reset_tab[reset_top].command);
	    
	    if (reset_tab[reset_top].command == 'S')
	    {
		reset_top++;
		break;
	    }

	    if (reset_tab[reset_top].command == '*')
	    {

		fgets(buf, 80, fl); /* skip command */
		continue;
	    }

	    fscanf(fl, " %d %d %d", 
		&tmp,
		&reset_tab[reset_top].arg1,
		&reset_tab[reset_top].arg2);

	    reset_tab[reset_top].if_flag = tmp;

	    if (reset_tab[reset_top].command == 'M' ||
		reset_tab[reset_top].command == 'O' ||
		reset_tab[reset_top].command == 'E' ||
		reset_tab[reset_top].command == 'P' ||
		reset_tab[reset_top].command == 'D')
		fscanf(fl, " %d", &reset_tab[reset_top].arg3);

	    fgets(buf, 80, fl); /* read comment */
	    reset_top++;
	}
	zon++;
    }
    top_of_zone_table = --zon;
    free(check);
    fclose(fl);
}



#endif

/*************************************************************************
*  procedures for resetting, both play-time and boot-time        *
*********************************************************************** */


/* read a mobile from MOB_FILE */
struct char_data *read_mobile(int nr, int type)
{
    int i;
    long tmp, tmp2, tmp3;
    struct char_data *mob;
    char buf[100];
    char letter;

    i = nr;
    if (type == VIRTUAL)
	if ((nr = real_mobile(nr)) < 0)
    {
	sprintf(buf, "Mobile (V) %d does not exist in database.", i);
	return(0);
    }

    fseek(mob_f, mob_index[nr].pos, 0);

    CREATE(mob, struct char_data, 1);
    clear_char(mob);

    /***** String data *** */
	
    mob->player.name = fread_string(mob_f);
    mob->player.short_descr = fread_string(mob_f);
    mob->player.long_descr = fread_string(mob_f);
    mob->player.description = fread_string(mob_f);
    mob->player.title = 0;

    /* *** Numeric data *** */

    fscanf(mob_f, "%d ", &tmp);
    mob->specials.act = tmp;
    SET_BIT(mob->specials.act, ACT_ISNPC);

    fscanf(mob_f, " %d ", &tmp);
    mob->specials.affected_by = tmp;

    fscanf(mob_f, " %d ", &tmp);
    mob->specials.alignment = tmp;

    fscanf(mob_f, " %c \n", &letter);

    if (letter == 'S') {
	/* The new easy monsters */
	mob->abilities.str   = 11;
	mob->abilities.intel = 11; 
	mob->abilities.wis   = 11;
	mob->abilities.dex   = 11;
	mob->abilities.con   = 11;

	fscanf(mob_f, " %d ", &tmp);
	GET_LEVEL(mob) = tmp;
	
	fscanf(mob_f, " %d ", &tmp);
	mob->points.hitroll = 20-tmp;
	
	fscanf(mob_f, " %d ", &tmp);
	mob->points.armor = 10*tmp;

	fscanf(mob_f, " %dd%d+%d ", &tmp, &tmp2, &tmp3);
	mob->points.max_hit = dice(tmp, tmp2)+tmp3;
	mob->points.hit = mob->points.max_hit;

	fscanf(mob_f, " %dd%d+%d \n", &tmp, &tmp2, &tmp3);
	mob->points.damroll = tmp3;
	mob->specials.damnodice = tmp;
	mob->specials.damsizedice = tmp2;

	mob->points.mana = 100;
	mob->points.max_mana = 100;

	mob->points.move = 82;
	mob->points.max_move = 82;

	fscanf(mob_f, " %d ", &tmp);
	mob->points.gold = tmp;

	fscanf(mob_f, " %d \n", &tmp);
	GET_EXP(mob) = tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->specials.position = tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->specials.default_pos = tmp;

	fscanf(mob_f, " %d \n", &tmp);
	mob->player.sex = tmp;

	mob->player.class = 0;

	mob->player.time.birth = time(0);
	mob->player.time.played = 0;
	mob->player.time.logon  = time(0);
	mob->player.weight = 200;
	mob->player.height = 198;

	for (i = 0; i < 3; i++)
	    GET_COND(mob, i) = -1;

	for (i = 0; i < 5; i++)
	    mob->specials.apply_saving_throw[i] = MAX(20-GET_LEVEL(mob), 2);

    } else {  /* The old monsters are down below here */

	fscanf(mob_f, " %d ", &tmp);
	mob->abilities.str = tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->abilities.intel = tmp; 

	fscanf(mob_f, " %d ", &tmp);
	mob->abilities.wis = tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->abilities.dex = tmp;

	fscanf(mob_f, " %d \n", &tmp);
	mob->abilities.con = tmp;

	fscanf(mob_f, " %d ", &tmp);
	fscanf(mob_f, " %d ", &tmp2);

	mob->points.max_hit = number(tmp, tmp2);
	mob->points.hit = mob->points.max_hit;

	fscanf(mob_f, " %d ", &tmp);
	mob->points.armor = 10*tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->points.mana = tmp;
	mob->points.max_mana = tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->points.move = tmp;     
	mob->points.max_move = tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->points.gold = tmp;

	fscanf(mob_f, " %d \n", &tmp);
	GET_EXP(mob) = tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->specials.position = tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->specials.default_pos = tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->player.sex = tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->player.class = tmp;

	fscanf(mob_f, " %d ", &tmp);
	GET_LEVEL(mob) = tmp;

	fscanf(mob_f, " %d ", &tmp);
	mob->player.time.birth = time(0);
	mob->player.time.played = 0;
	mob->player.time.logon  = time(0);

	fscanf(mob_f, " %d ", &tmp);
	mob->player.weight = tmp;

	fscanf(mob_f, " %d \n", &tmp);
	mob->player.height = tmp;

	for (i = 0; i < 3; i++)
	{
	    fscanf(mob_f, " %d ", &tmp);
	    GET_COND(mob, i) = tmp;
	}
	fscanf(mob_f, " \n ");

	for (i = 0; i < 5; i++)
	{
	    fscanf(mob_f, " %d ", &tmp);
	    mob->specials.apply_saving_throw[i] = tmp;
	}

	fscanf(mob_f, " \n ");

	/* Set the damage as some standard 1d4 */
	mob->points.damroll = 0;
	mob->specials.damnodice = 1;
	mob->specials.damsizedice = 6;

	/* Calculate THAC0 as a formula of Level */
	mob->points.hitroll = MAX(1, GET_LEVEL(mob)-3);
    }

    mob->tmpabilities = mob->abilities;

    for (i = 0; i < MAX_WEAR; i++) /* Initialisering Ok */
	mob->equipment[i] = 0;

    mob->nr = nr;

    mob->desc = 0;


    /* insert in list */

    mob->next = character_list;
    character_list = mob;

    mob_index[nr].number++;

    return(mob);
}


/* read an object from OBJ_FILE */
struct obj_data *read_object(int nr, int eq_level)
{
    struct obj_data *obj;
    int tmp, i;

    char chk[MAX_INPUT_LENGTH];
    struct extra_descr_data *new_descr;

    fseek(obj_f, obj_index[nr].pos, 0);
    CREATE(obj, struct obj_data, 1);
    clear_object(obj);

    /* *** string data *** */

    obj->name			= fread_string(obj_f);
    obj->short_description	= fread_string(obj_f);
    obj->description		= fread_string(obj_f);
    obj->action_description	= fread_string(obj_f);

    /* *** numeric data *** */

    fscanf(obj_f, " %d ", &tmp);   obj->obj_flags.type_flag	= tmp;
    fscanf(obj_f, " %d ", &tmp);   obj->obj_flags.extra_flags	= tmp;
    fscanf(obj_f, " %d ", &tmp);   obj->obj_flags.wear_flags	= tmp;
    fscanf(obj_f, " %d ", &tmp);   obj->obj_flags.value[0]	= tmp;
    fscanf(obj_f, " %d ", &tmp);   obj->obj_flags.value[1]	= tmp;
    fscanf(obj_f, " %d ", &tmp);   obj->obj_flags.value[2]	= tmp;
    fscanf(obj_f, " %d ", &tmp);   obj->obj_flags.value[3]	= tmp;
    fscanf(obj_f, " %d ", &tmp);   obj->obj_flags.weight	= tmp;
    fscanf(obj_f, " %d \n", &tmp); obj->obj_flags.cost		= tmp;
    fscanf(obj_f, " %d \n", &tmp); obj->obj_flags.cost_per_day	= tmp;

    /* *** extra descriptions *** */

    obj->ex_description = 0;
    while (fscanf(obj_f, " %s \n", chk), *chk == 'E')
    {
	CREATE(new_descr, struct extra_descr_data, 1);
	new_descr->keyword	= fread_string(obj_f);
	new_descr->description	= fread_string(obj_f);
	new_descr->next		= obj->ex_description;
	obj->ex_description	= new_descr;
    }

    for( i = 0 ; (i < MAX_OBJ_AFFECT) && (*chk == 'A') ; i++)
    {
	fscanf(obj_f, " %d ", &tmp);   obj->affected[i].location	= tmp;
	fscanf(obj_f, " %d \n", &tmp); obj->affected[i].modifier	= tmp;
	fscanf(obj_f, " %s \n", chk);
    }

    for (;(i < MAX_OBJ_AFFECT);i++)
    {
	obj->affected[i].location	= APPLY_NONE;
	obj->affected[i].modifier	= 0;
    }

    obj->in_room	= NOWHERE;
    obj->next_content	= 0;
    obj->carried_by	= 0;
    obj->in_obj		= 0;
    obj->contains	= 0;
    obj->item_number	= nr;  
    obj->obj_flags.eq_level	= eq_level;

    obj->next		= object_list;
    object_list		= obj;
    obj_index[nr].number++;

    return (obj);  
}




void zone_update(void)
{
    int i;

    for ( i = 0; i <= top_of_zone_table; i++ )
    {
	if ( zone_table[i].reset_mode == 0 )
	    continue;

	if ( zone_table[i].age < zone_table[i].lifespan )
	{
	    zone_table[i].age++;
	    continue;
	}

	if ( zone_table[i].reset_mode == 1 && !is_empty(i) )
	    continue;

	reset_zone( i );
    }
}



#define ZCMD zone_table[zone].cmd[cmd_no]

/* execute the reset command table of a given zone */
void reset_zone(int zone)
{
    int cmd_no, last_cmd = 1;
    struct char_data *mob = NULL;
    struct obj_data *obj, *obj_to;

    for (cmd_no = 0;;cmd_no++)
    {
	if (ZCMD.command == 'S')
	    break;

	if ( last_cmd || ZCMD.if_flag == 0 )
	    switch(ZCMD.command)
	{
	    case 'M': /* read a mobile */
		if (mob_index[ZCMD.arg1].number < 
		    ZCMD.arg2)
		{
		    mob = read_mobile(ZCMD.arg1, REAL);
		    char_to_room(mob, ZCMD.arg3);
		    last_cmd = 1;
		}
		else
		    last_cmd = 0;
	    break;

	    case 'O': /* read an object */
		if (obj_index[ZCMD.arg1].number < ZCMD.arg2)
		if (ZCMD.arg3 >= 0)
		{
		    if (!get_obj_in_list_num(
			ZCMD.arg1,world[ZCMD.arg3].contents))
		    {
			obj = read_object(ZCMD.arg1, 0);
			obj_to_room(obj, ZCMD.arg3);
			last_cmd = 1;
		    }
		    else
			last_cmd = 0;
		}
		else
		{
		    obj = read_object(ZCMD.arg1, 0);
		    obj->in_room = NOWHERE;
		    last_cmd = 1;
		}
		else
		    last_cmd = 0;
	    break;

	    case 'P': /* object to object */
		if (obj_index[ZCMD.arg1].number < ZCMD.arg2)
		{
		    obj_to = get_obj_num(ZCMD.arg3);
		    obj = read_object(ZCMD.arg1, obj_to->obj_flags.eq_level);
		    obj_to_obj(obj, obj_to);
		    last_cmd = 1;
		}
		else
		    last_cmd = 0;
	    break;

	    case 'G': /* obj_to_char */
		if ( mob == NULL )
		{
		    log( "Null mob in G" );
		    last_cmd = 0;
		    break;
		}
		if (obj_index[ZCMD.arg1].number < ZCMD.arg2)
		{       
		    obj = read_object(ZCMD.arg1, map_eq_level(mob) );
		    obj_to_char(obj, mob);
		    last_cmd = 1;
		}
		else
		    last_cmd = 0;
	    break;

	    case 'E': /* object to equipment list */
		if ( mob == NULL )
		{
		    log( "Null mob in E" );
		    last_cmd = 0;
		    break;
		}
		if (obj_index[ZCMD.arg1].number < ZCMD.arg2)
		{       
		    obj = read_object(ZCMD.arg1, map_eq_level(mob) );
		    equip_char(mob, obj, ZCMD.arg3);
		    last_cmd = 1;
		}
		else
		    last_cmd = 0;
	    break;

	    case 'D': /* set state of door */
		switch (ZCMD.arg3)
		{
		    case 0:
			REMOVE_BIT(
			    world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
			    EX_LOCKED);
			REMOVE_BIT(
			    world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
			    EX_CLOSED);
		    break;
		    case 1:
			SET_BIT(
			    world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
			    EX_CLOSED);
			REMOVE_BIT(
			    world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
			    EX_LOCKED);
		    break;
		    case 2:
			SET_BIT(
			    world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
			    EX_LOCKED);
			SET_BIT(
			    world[ZCMD.arg1].dir_option[ZCMD.arg2]->exit_info,
			    EX_CLOSED);
		    break;
		}
		last_cmd = 1;
	    break;

	    default:
		sprintf(log_buf, "Undef command: zone %d cmd %d.",
		    zone, cmd_no);
		log(log_buf);
		exit( 1 );
	    break;
	}
	else
	    last_cmd = 0;

    }

    zone_table[zone].age = 0;
}

#undef ZCMD



/* for use in reset_zone; return TRUE if zone 'nr' is free of PC's  */
int is_empty(int zone_nr)
{
    struct descriptor_data *i;

    for (i = descriptor_list; i; i = i->next)
	if (!i->connected)
	    if (world[i->character->in_room].zone == zone_nr)
		return(0);

    return(1);
}



/************************************************************************
*  procs of a (more or less) general utility nature         *
********************************************************************** */


/* read and allocate space for a '~'-terminated string from a given file */
char *fread_string(FILE *fl)
{
    char    buf[MAX_STRING_LENGTH];
    char *  pAlloc;
    char *  pBufLast;

    for ( pBufLast = buf; pBufLast < &buf[sizeof(buf)-2]; )
    {
	switch( *pBufLast = getc( fl ) )
	{
	default:
	    pBufLast++;
	    break;

	case EOF:
	    perror( "fread_string: EOF" );
	    exit( 1 );
	    break;

	case '\n':
	    while ( pBufLast > buf && isspace(pBufLast[-1]) )
		pBufLast--;
	    *pBufLast++ = '\n';
	    *pBufLast++ = '\r';
	    break;

	case '~':
	    getc( fl );
	    if ( pBufLast == buf )
		pAlloc  = "";
	    else
	    {
		*pBufLast++ = '\0';
		CREATE( pAlloc, char, pBufLast-buf );
		memcpy( pAlloc, buf, pBufLast-buf );
	    }
	    return pAlloc;
	}
    }

    perror( "fread_string: string too long" );
    exit( 1 );
    return( NULL );
}



/* release memory allocated for a char struct */
void free_char( CHAR_DATA *ch )
{
    int iWear;
    struct affected_type *af;

    FREE( GET_NAME(ch)           );
    FREE( ch->player.title       );
    FREE( ch->player.short_descr );
    FREE( ch->player.long_descr  );
    FREE( ch->player.description );

    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ch->equipment[iWear] )
	    obj_to_char( unequip_char( ch, iWear ), ch );
    }

    while ( ch->carrying )
	extract_obj( ch->carrying );

    for ( af = ch->affected; af; af = af->next )
	affect_remove( ch, af );

    FREE( ch );
}



/* release memory allocated for an obj struct */
void free_obj(struct obj_data *obj)
{
    struct extra_descr_data *this, *next_one;

    FREE( obj->name               );
    FREE( obj->description        );
    FREE( obj->short_description  );
    FREE( obj->action_description );

    for ( this = obj->ex_description; this != NULL; this = next_one )
    {
	next_one = this->next;
	FREE( this->keyword );
	FREE( this->description );
	FREE( this );
    }

    FREE(obj);
}



/* read contents of a text file, and place in buf */
int file_to_string(char *name, char *buf)
{
    FILE *fl;
    char tmp[100];

    *buf = '\0';

    if (!(fl = fopen(name, "r")))
    {
	perror("file-to-string");
	*buf = '\0';
	return(-1);
    }

    do
    {
	fgets(tmp, 99, fl);

	if (!feof(fl))
	{
	    if (strlen(buf) + strlen(tmp) + 2 > MAX_STRING_LENGTH)
	    {
		log("fl->strng: string too big (db.c, file_to_string)");
		*buf = '\0';
		return(-1);
	    }

	    strcat(buf, tmp);
	    *(buf + strlen(buf) + 1) = '\0';
	    *(buf + strlen(buf)) = '\r';
	}
    }
    while (!feof(fl));

    fclose(fl);

    return(0);
}



/* clear some of the the working variables of a char */
void reset_char(struct char_data *ch)
{
    int i;

    for (i = 0; i < MAX_WEAR; i++)  /* Intializing  */
	ch->equipment[i] = 0;

    ch->followers = 0;
    ch->master = 0;

    ch->carrying = 0;
    ch->next = 0;
    ch->next_fighting = 0;
    ch->next_in_room = 0;
    ch->specials.fighting = 0;
    ch->specials.position = POSITION_STANDING;
    ch->specials.default_pos = POSITION_STANDING;
    ch->specials.carry_weight = 0;
    ch->specials.carry_items = 0;
    GET_AC(ch) = 100;
    ch->specials.wizInvis = FALSE;
    ch->specials.holyLite = FALSE;

    if (GET_HIT(ch) <= 0)
	GET_HIT(ch) = 1;
    if (GET_MOVE(ch) <= 0)
	GET_MOVE(ch) = 1;
    if (GET_MANA(ch) <= 0)
	GET_MANA(ch) = 1;
}



/*
 * Clear but do not de-alloc.
 */
void clear_char(struct char_data *ch)
{
    memset((char *)ch, (char)'\0', (int)sizeof(struct char_data));

    ch->in_room = NOWHERE;
    ch->specials.was_in_room = NOWHERE;
    ch->specials.position = POSITION_STANDING;
    ch->specials.default_pos = POSITION_STANDING;
	ch->specials.wizInvis = FALSE;
	ch->specials.holyLite = FALSE;

    GET_AC(ch) = 100; /* Basic Armor */
    if (ch->points.max_mana < 100) {
      ch->points.max_mana = 100;
    } /* if */
}


void clear_object(struct obj_data *obj)
{
    memset((char *)obj, (char)'\0', (int)sizeof(struct obj_data));

    obj->item_number = -1;
    obj->in_room      = NOWHERE;
}




/* initialize a new character only if class is set */
void init_char(struct char_data *ch)
{
    int i;
    extern int god;

    /*
     * Boot with -g to make gods.
     */
    if ( god )
    {
	GET_EXP(ch) = 90000000;
	GET_LEVEL(ch) = 35;
    }

    set_title(ch);

    ch->player.short_descr = 0;
    ch->player.long_descr = 0;
    ch->player.description = 0;

    ch->player.hometown = number(1,4);

    ch->player.time.birth = time(0);
    ch->player.time.played = 0;
    ch->player.time.logon = time(0);

    for (i = 0; i < MAX_TONGUE; i++)
     ch->player.talks[i] = 0;

    GET_STR(ch) = 9;
    GET_INT(ch) = 9;
    GET_WIS(ch) = 9;
    GET_DEX(ch) = 9;
    GET_CON(ch) = 9;

    /* make favors for sex */
    if (ch->player.sex == SEX_MALE) {
	ch->player.weight = number(120,180);
	ch->player.height = number(160,200);
    } else {
	ch->player.weight = number(100,160);
	ch->player.height = number(150,180);
    }
    
	ch->points.max_mana = 100;
    ch->points.mana = GET_MAX_MANA(ch);
    ch->points.hit = GET_MAX_HIT(ch);
    ch->points.move = GET_MAX_MOVE(ch);
    ch->points.armor = 100;

    for (i = 0; i < MAX_SKILLS; i++)
    {
	if (GET_LEVEL(ch) <35) {
	    ch->skills[i].learned = 0;
	    ch->skills[i].recognise = FALSE;
	}   else {
	    ch->skills[i].learned = 100;
	    ch->skills[i].recognise = FALSE;
	}
    }

    ch->specials.affected_by = 0;
    ch->specials.practices   = 0;

    for (i = 0; i < 5; i++)
	ch->specials.apply_saving_throw[i] = 0;

    for (i = 0; i < 3; i++)
	GET_COND(ch, i) = (GET_LEVEL(ch) == 35 ? -1 : 35);

    reset_char(ch);
}



/* returns the real number of the room with given virtual number */
int real_room(int virtual)
{
    int bot, top, mid;

    bot = 0;
    top = top_of_world;

    /* perform binary search on world-table */
    for (;;)
    {
	mid = (bot + top) / 2;

	if ((world + mid)->number == virtual)
	    return(mid);
	if (bot >= top)
	{
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
int real_mobile(int virtual)
{
    int bot, top, mid;

    bot = 0;
    top = top_of_mobt;

    /* perform binary search on mob-table */
    for (;;)
    {
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
int real_object(int virtual)
{
    int bot, top, mid;

    bot = 0;
    top = top_of_objt;

    /* perform binary search on obj-table */
    for (;;)
    {
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
