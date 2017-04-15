/***************************************************************************
 *  file: spec_ass.c , Special module.                     Part of DIKUMUD *
 *  Usage: Procedures assigning function pointers.                         *
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

#include <stdio.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "utils.h"
#include "db.h"

typedef int    SPEC_FUN  (struct char_data *ch, int cmd, char *argument);

extern struct room_data *world;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
void boot_the_shops();
void assign_the_shopkeepers();

/* ********************************************************************
*  Assignments                                                        *
******************************************************************** */

/* assign special procedures to mobiles */
void assign_mobiles(void)
{
    SPEC_FUN	cityguard;
    SPEC_FUN	guild;
    SPEC_FUN	guild_guard;
    SPEC_FUN	puff;
    SPEC_FUN	fido;
    SPEC_FUN	janitor;
    SPEC_FUN	mayor;
    SPEC_FUN	snake;
    SPEC_FUN	thief;
    SPEC_FUN	magic_user;
    SPEC_FUN	red_dragon;
    SPEC_FUN	blue_dragon;
    SPEC_FUN	green_dragon;
    SPEC_FUN	black_dragon;
    SPEC_FUN	white_dragon;
    SPEC_FUN	mud_school_adept;
    SPEC_FUN	adept;
    SPEC_FUN	brass_dragon;
    SPEC_FUN	baby_troll;
    SPEC_FUN	Fanatic_Hector;
    SPEC_FUN	Thalos_citizen;
    SPEC_FUN	Executioner;
    SPEC_FUN	MERCling;
    SPEC_FUN	train;

    mob_index[real_mobile(1)].func = puff;

    mob_index[real_mobile(200)].func = mud_school_adept;
    mob_index[real_mobile(207)].func = adept;
    mob_index[real_mobile(208)].func = adept;
    mob_index[real_mobile(214)].func = fido;
    mob_index[real_mobile(218)].func = train;
    mob_index[real_mobile(219)].func = guild;
    mob_index[real_mobile(250)].func = MERCling;

    mob_index[real_mobile(1203)].func = Executioner;
    mob_index[real_mobile(3007)].func = train;
    mob_index[real_mobile(3060)].func = cityguard;
    mob_index[real_mobile(3067)].func = cityguard;
    mob_index[real_mobile(3061)].func = janitor;
    mob_index[real_mobile(3062)].func = fido;
    mob_index[real_mobile(3066)].func = fido;

    mob_index[real_mobile(3020)].func = guild;
    mob_index[real_mobile(3021)].func = guild;
    mob_index[real_mobile(3022)].func = guild;
    mob_index[real_mobile(3023)].func = guild;

    mob_index[real_mobile(3024)].func = guild_guard;
    mob_index[real_mobile(3025)].func = guild_guard;
    mob_index[real_mobile(3026)].func = guild_guard;
    mob_index[real_mobile(3027)].func = guild_guard;

    mob_index[real_mobile(3143)].func = mayor;
      
    /* MIDEN'NIR */
    mob_index[real_mobile(3500)].func = snake;

    /* MORIA */
    mob_index[real_mobile(4000)].func = snake;
    mob_index[real_mobile(4001)].func = snake;
    mob_index[real_mobile(4053)].func = snake;

    mob_index[real_mobile(4103)].func = thief;
    mob_index[real_mobile(4100)].func = magic_user;
    mob_index[real_mobile(4102)].func = snake;

    mob_index[real_mobile(4150)].func = blue_dragon;
    mob_index[real_mobile(4151)].func = red_dragon;
    mob_index[real_mobile(4152)].func = green_dragon;
    mob_index[real_mobile(4153)].func = white_dragon;
    mob_index[real_mobile(4154)].func = black_dragon;
    mob_index[real_mobile(5005)].func = brass_dragon;
    mob_index[real_mobile(5377)].func = white_dragon;
    mob_index[real_mobile(4155)].func = thief;
    mob_index[real_mobile(4157)].func = magic_user;
    mob_index[real_mobile(4158)].func = snake;

    /* mages and mage types */
    mob_index[real_mobile(5200)].func = magic_user;
    mob_index[real_mobile(7200)].func = magic_user;
    mob_index[real_mobile(7201)].func = magic_user;
    mob_index[real_mobile(7202)].func = magic_user;

    /* more dragons */
    mob_index[real_mobile(5050)].func = green_dragon;
    mob_index[real_mobile(5377)].func = white_dragon;
    mob_index[real_mobile(6112)].func = green_dragon;
    mob_index[real_mobile(6302)].func = red_dragon;
    mob_index[real_mobile(6302)].func = green_dragon;
    mob_index[real_mobile(6316)].func = green_dragon;
    mob_index[real_mobile(6317)].func = green_dragon;
    mob_index[real_mobile(350)].func  = green_dragon;
    mob_index[real_mobile(1000)].func = magic_user; /* faerie dragon */
    mob_index[real_mobile(5010)].func = red_dragon; /* draco lich    */

    /* SEWERS */
    mob_index[real_mobile(7006)].func = snake;
    mob_index[real_mobile(7040)].func = red_dragon;

    /* FOREST */
    mob_index[real_mobile(6113)].func = snake;
    mob_index[real_mobile(6114)].func = snake;

    mob_index[real_mobile(6910)].func = magic_user;
    mob_index[real_mobile(3404)].func = magic_user;
    mob_index[real_mobile(2828)].func = baby_troll;

    /* Thalos */
    mob_index[real_mobile(503)].func  = cityguard;
    mob_index[real_mobile(502)].func  = Thalos_citizen;
    mob_index[real_mobile(504)].func  = Thalos_citizen;
    mob_index[real_mobile(506)].func  = Fanatic_Hector;
    mob_index[real_mobile(507)].func  = magic_user;
    
    boot_the_shops();
    assign_the_shopkeepers();
}

/* assign special procedures to objects */
void assign_objects(void)
{
    int board(struct char_data *ch, int cmd, char *arg);

    obj_index[real_object(3099)].func = board;
}



/* assign special procedures to rooms */
void assign_rooms(void)
{
    int pet_shops(struct char_data *ch, int cmd, char *arg);

    world[real_room(3031)].funct = pet_shops;
}
