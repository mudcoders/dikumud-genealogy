/* ************************************************************************
*   File: spec_assign.c                                 Part of CircleMUD *
*  Usage: Functions to assign function pointers to objs/mobs/rooms        *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include "structs.h"
#include "db.h"
#include "interpreter.h"
#include "utils.h"

extern struct room_data *world;
extern int top_of_world;
extern struct index_data *mob_index;
extern struct index_data *obj_index;

/* ********************************************************************
*  Assignments                                                        *
******************************************************************** */

/* assign special procedures to mobiles */
void	assign_mobiles(void)
{
   SPECIAL(postmaster);
   SPECIAL(cityguard);
   SPECIAL(receptionist);
   SPECIAL(cryogenicist);
   SPECIAL(guild_guard);
   SPECIAL(guild);
   SPECIAL(puff);
   SPECIAL(fido);
   SPECIAL(janitor);
   SPECIAL(mayor);
   SPECIAL(snake);
   SPECIAL(thief);
   SPECIAL(magic_user);
   SPECIAL(brass_dragon);
   void assign_kings_castle(void);

   assign_kings_castle();

   ASSIGNMOB(1, puff);
   ASSIGNMOB(5005, brass_dragon);
   ASSIGNMOB(4061, cityguard);

   ASSIGNMOB(3059, cityguard);
   ASSIGNMOB(3060, cityguard);
   ASSIGNMOB(3067, cityguard);
   ASSIGNMOB(3061, janitor);
   ASSIGNMOB(3062, fido);
   ASSIGNMOB(3066, fido);
   ASSIGNMOB(3068, janitor);

   ASSIGNMOB(3005, receptionist);
   ASSIGNMOB(3095, cryogenicist);
   ASSIGNMOB(3010, postmaster);

   ASSIGNMOB(3020, guild);
   ASSIGNMOB(3021, guild);
   ASSIGNMOB(3022, guild);
   ASSIGNMOB(3023, guild);

   ASSIGNMOB(3024, guild_guard);
   ASSIGNMOB(3025, guild_guard);
   ASSIGNMOB(3026, guild_guard);
   ASSIGNMOB(3027, guild_guard);

   ASSIGNMOB(3143, mayor);

   ASSIGNMOB(7009, magic_user);
   ASSIGNMOB(6999, magic_user);

   /* MORIA */
   ASSIGNMOB(4000, snake);
   ASSIGNMOB(4001, snake);
   ASSIGNMOB(4053, snake);

   ASSIGNMOB(4103, thief);
   ASSIGNMOB(4100, magic_user);
   ASSIGNMOB(4102, snake);

   /* SEWERS */
   ASSIGNMOB(7006, snake);
   ASSIGNMOB(7200, magic_user);
   ASSIGNMOB(7201, magic_user);
   ASSIGNMOB(7240, magic_user);

   /* FOREST */
   ASSIGNMOB(6113, snake);

   ASSIGNMOB(6115, magic_user);
   ASSIGNMOB(6910, magic_user);
   ASSIGNMOB(6112, magic_user);
   ASSIGNMOB(6114, magic_user);
   ASSIGNMOB(6116, magic_user);


   /* ARACHNOS */
   ASSIGNMOB(6312, magic_user);
   ASSIGNMOB(6314, magic_user);
   ASSIGNMOB(6315, magic_user);
   ASSIGNMOB(6309, magic_user);
   ASSIGNMOB(6302, magic_user);

   /* DROW and THALOS */
   ASSIGNMOB(5104, magic_user);
   ASSIGNMOB(5014, magic_user);
   ASSIGNMOB(5010, magic_user);
   ASSIGNMOB(5107, magic_user);
   ASSIGNMOB(5108, magic_user);
   ASSIGNMOB(5200, magic_user);
   ASSIGNMOB(5201, magic_user);
   ASSIGNMOB(5004, magic_user);
   ASSIGNMOB(5103, magic_user);
   ASSIGNMOB(5352, magic_user);

   /* ROME */
   ASSIGNMOB(12018, cityguard);
   ASSIGNMOB(12021, cityguard);
   ASSIGNMOB(12009, magic_user);
   ASSIGNMOB(12025, magic_user);

   ASSIGNMOB(12020, magic_user);
   ASSIGNMOB(12025, magic_user);
   ASSIGNMOB(12030, magic_user);
   ASSIGNMOB(12031, magic_user);
   ASSIGNMOB(12032, magic_user);

   /* DWARVEN KINGDOM */
   ASSIGNMOB(6502, magic_user);
   ASSIGNMOB(6516, magic_user);
   ASSIGNMOB(6509, magic_user);
   ASSIGNMOB(6500, cityguard);

   /* NEW SPARTA */
   ASSIGNMOB(21068, guild_guard);
   ASSIGNMOB(21069, guild_guard);
   ASSIGNMOB(21070, guild_guard);
   ASSIGNMOB(21071, guild_guard);

   ASSIGNMOB(21072, guild);
   ASSIGNMOB(21073, guild);
   ASSIGNMOB(21074, guild);
   ASSIGNMOB(21075, guild);

   ASSIGNMOB(21084, janitor);
   ASSIGNMOB(21085, fido);
   ASSIGNMOB(21057, magic_user);
   ASSIGNMOB(21054, magic_user);
   ASSIGNMOB(21011, magic_user);
   ASSIGNMOB(21003, cityguard);

   ASSIGNMOB(21078, receptionist);
}



/* assign special procedures to objects */
void	assign_objects(void)
{
   SPECIAL(bank);
   SPECIAL(gen_board);

   ASSIGNOBJ(3096, gen_board);  /* social board */
   ASSIGNOBJ(3097, gen_board);  /* freeze board */
   ASSIGNOBJ(3098, gen_board);	/* immortal board */
   ASSIGNOBJ(3099, gen_board);  /* mortal board */

   ASSIGNOBJ(3034, bank);
}



/* assign special procedures to rooms */
void	assign_rooms(void)
{
   extern int dts_are_dumps;
   int i;

   SPECIAL(dump);
   SPECIAL(pet_shops);
   SPECIAL(pray_for_items);

   ASSIGNROOM(3030, dump);
   ASSIGNROOM(3031, pet_shops);
   ASSIGNROOM(21116, pet_shops);

   /*
   ASSIGNROOM(3054, pray_for_items);
   */

   if (dts_are_dumps)
      for (i = 0; i < top_of_world; i++)
         if (IS_SET(world[i].room_flags, DEATH))
            world[i].funct = dump;
}

