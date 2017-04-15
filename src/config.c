/* ************************************************************************
*   File: config.c                                      Part of CircleMUD *
*  Usage: Configuration of various aspects of CircleMUD operation         *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "structs.h"

#define TRUE	1
#define YES	1
#define FALSE	0
#define NO	0

/*
Below are several constants which you can change to alter certain aspects
of the way CircleMUD acts.  Since this is a .c file, all you have to do
to change one of the constants (assuming you keep your object files around)
is change the constant in this file and type 'make'.  Make will recompile
this file and relink; you don't have to wait for the whole thing to recompile
as you do if you change a header file.

I realize that it would be slightly more efficient to have lots of
#defines strewn about, so that, for example, the autowiz code isn't
compiled at all if you don't want to use autowiz.  However, the actual
code for the various options is quite small, as is the computational time
in checking the option you've selected at run-time, so I've decided the
convenience of having all your options in this one file outweighs the
efficency of doing it the other way.
*/

/****************************************************************************/
/****************************************************************************/


/* GAME PLAY OPTIONS */

/* is playerkilling allowed? */
int pk_allowed = NO;

/* is playerthieving allowed? */
int pt_allowed = NO;

/* minimum level a player must be to shout/holler/gossip/auction */
int level_can_shout = 2;

/* number of movement points it costs to holler */
int holler_move_cost = 20;

/* number of tics (usually 75 seconds) before PC/NPC corpses decompose */
int max_npc_corpse_time = 5;
int max_pc_corpse_time = 10;

/* should items in death traps automatically be junked? */
int dts_are_dumps = YES;

/****************************************************************************/
/****************************************************************************/


/* RENT/CRASHSAVE OPTIONS */

/* maximum number of items players are allowed to rent */
int max_obj_save = 30;

/* receptionist's surcharge on top of item costs */
int min_rent_cost = 100;

/* Should the game automatically save people?  (i.e., save player data
   every 4 kills (on average), and Crash-save as defined below. */
int auto_save = YES;

/* if auto_save (above) is yes, how often (in minutes) should the MUD
   Crash-save people's objects? */
int autosave_time = 10;

/* Lifetime of crashfiles and forced-rent (idlesave) files in days */
int crash_file_timeout = 10;

/* Lifetime of normal rent files in days */
int rent_file_timeout = 30;


/****************************************************************************/
/****************************************************************************/


/* ROOM NUMBERS */

/* virtual number of room that immorts should enter at by default */
sh_int immort_start_room = 1204;

/* virtual number of room that mortals should enter at */
sh_int mortal_start_room = 3001;

/* virtual number of room that frozen players should enter at */
sh_int frozen_start_room = 1202;

/* virtual numbers of donation rooms.  note: you must change code in
   do_drop of act.obj1.c if you change the number of non-NOWHERE
   donation rooms.
*/
sh_int donation_room_1 = 3063;
sh_int donation_room_2 = NOWHERE;
sh_int donation_room_3 = NOWHERE;


/****************************************************************************/
/****************************************************************************/


/* GAME OPERATION OPTIONS */

/* default port the game should run on if no port given on command-line */
int DFLT_PORT = 4000;

/* default directory to use as data directory */
char *DFLT_DIR = "lib";

/* maximum number of players allowed before game starts to turn people away */
int MAX_PLAYERS = 300;

/* maximum number of files your system allows a process to have open at
   once -- not used if you define #TABLE_SIZE in comm.c or if your system
   defines OPEN_MAX.
*/
int MAX_DESCRIPTORS_AVAILABLE = 256;

/* Some nameservers (such as the one here at JHU) are slow and cause the
   game to lag terribly every time someone logs in.  The lag is caused by
   the gethostbyaddr() function -- the function which resolves a numeric
   IP address (such as 128.220.13.30) into an alphabetic name (such as
   circle.cs.jhu.edu).

   The nameserver at JHU can get so bad at times that the incredible lag
   caused by gethostbyaddr() isn't worth the luxury of having names
   instead of numbers for players' sitenames.

   If your nameserver is fast, set the variable below to NO.  If your
   nameserver is slow, of it you would simply prefer to have numbers
   instead of names for some other reason, set the variable to YES.

   You can experiment with the setting of nameserver_is_slow on-line using
   the SLOWNS command from within the MUD.
*/

int nameserver_is_slow = YES;
   

char *MENU = 
"\n\r"
"Welcome to CircleMUD!\n\r"
"0) Exit from CircleMUD.\n\r"
"1) Enter the game.\n\r"
"2) Enter description.\n\r"
"3) Read the background story.\n\r"
"4) Change password.\n\r"
"5) Delete this character.\n\r"
"\n\r"
"   Make your choice: ";



char *GREETINGS =

"\n\r\n\r"
"                             CircleMUD  2.2\n\r"
"\n\r"
"                      Based on DikuMUD (GAMMA 0.0)\n\r"
"                               Created by\n\r"
"                  Hans Henrik Staerfeldt, Katja Nyboe,\n\r"
"           Tom Madsen, Michael Seifert, and Sebastian Hammer\n\r\n\r";

char *WELC_MESSG =
"\n\r"
"Welcome to the land of CircleMUD!  May your visit here be... Interesting."
"\n\r\n\r";

char *START_MESSG =
"Welcome.  This is your new CircleMUD character.  You can now earn gold,\n\r"
"experience, and lots more...\n\r";

/****************************************************************************/
/****************************************************************************/


/* AUTOWIZ OPTIONS */

/* Should the game automatically create a new wizlist/immlist every time
   someone immorts, or is promoted to a higher (or lower) god level? */
int use_autowiz = YES;

/* If yes, what is the lowest level which should be on the wizlist?  (All
   immort levels below the level you specify will go on the immlist instead. */
int min_wizlist_lev = LEVEL_GOD;

