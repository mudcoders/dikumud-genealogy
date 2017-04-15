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
    SPEC_FUN	red_dragon;
    SPEC_FUN	blue_dragon;
    SPEC_FUN	green_dragon;
    SPEC_FUN	black_dragon;
    SPEC_FUN	white_dragon;
    SPEC_FUN	mud_school_adept;
    SPEC_FUN	adept;
    SPEC_FUN	brass_dragon;
    SPEC_FUN	baby_troll;
    SPEC_FUN	Executioner;
    SPEC_FUN	MERCling;
    SPEC_FUN	train;

    mob_index[real_mobile(1)].func = puff;

    mob_index[real_mobile(200)].func = mud_school_adept;
    mob_index[real_mobile(207)].func = adept;
    mob_index[real_mobile(208)].func = adept;
    mob_index[real_mobile(214)].func = fido;
    mob_index[real_mobile(1601)].func = fido;
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

    /* Dwarven Catacombs */


      
    /* MIDEN'NIR */
    mob_index[real_mobile(3500)].func = snake;

    /* MORIA */
    mob_index[real_mobile(4000)].func = snake;
    mob_index[real_mobile(4001)].func = snake;
    mob_index[real_mobile(4053)].func = snake;

    mob_index[real_mobile(4103)].func = thief;
    mob_index[real_mobile(4102)].func = snake;

    mob_index[real_mobile(4150)].func = blue_dragon;
    mob_index[real_mobile(4151)].func = red_dragon;
    mob_index[real_mobile(4152)].func = green_dragon;
    mob_index[real_mobile(4154)].func = black_dragon;
    mob_index[real_mobile(5005)].func = brass_dragon;
    mob_index[real_mobile(4155)].func = thief;
    mob_index[real_mobile(4158)].func = snake;

    /* mages and mage types */

    /* more dragons */
    mob_index[real_mobile(5050)].func = green_dragon;
    mob_index[real_mobile(6112)].func = green_dragon;
    mob_index[real_mobile(6302)].func = red_dragon;
    mob_index[real_mobile(6302)].func = green_dragon;
    mob_index[real_mobile(6316)].func = green_dragon;
    mob_index[real_mobile(6317)].func = green_dragon;
    mob_index[real_mobile(350)].func  = green_dragon;
    mob_index[real_mobile(5010)].func = red_dragon; /* draco lich    */

    /* Arachnos */


    /* SEWERS */
    mob_index[real_mobile(7006)].func = snake;
    mob_index[real_mobile(7040)].func = red_dragon;

    /* FOREST */
    mob_index[real_mobile(6113)].func = snake;
    mob_index[real_mobile(6114)].func = snake;

    mob_index[real_mobile(2828)].func = baby_troll;
  

    /* Drow */


    /* Misc missing procedures */

    mob_index[real_mobile(5017)].func = snake; /* camel */
    mob_index[real_mobile(5003)].func = snake; /* scorpion */
    mob_index[real_mobile(9577)].func = snake; /* hornet */
    mob_index[real_mobile(9570)].func = white_dragon; /* cryohydra */
    mob_index[real_mobile(9574)].func = blue_dragon; /* behir */
    mob_index[real_mobile(9575)].func = red_dragon; /* chimera */
    mob_index[real_mobile(9561)].func = cityguard; /* sultans guard*/
    mob_index[real_mobile(9562)].func = cityguard; /* royal guard */
    mob_index[real_mobile(7203)].func = snake; /* wererat */
    mob_index[real_mobile(7003)].func = snake; /* spider */ 





    /* SHIRE */

    mob_index[real_mobile(9401)].func = snake;
    mob_index[real_mobile(9410)].func = cityguard;
    mob_index[real_mobile(9411)].func = cityguard;
    mob_index[real_mobile(9412)].func = cityguard;
    mob_index[real_mobile(9421)].func = cityguard;
    mob_index[real_mobile(9422)].func = thief;
    mob_index[real_mobile(9432)].func = cityguard;

    /* New Thalos */

    mob_index[real_mobile(9500)].func = guild;
    mob_index[real_mobile(9501)].func = guild;
    mob_index[real_mobile(9502)].func = guild;
    mob_index[real_mobile(9503)].func = guild;
    mob_index[real_mobile(9535)].func = thief;
    mob_index[real_mobile(9556)].func = guild_guard;
    mob_index[real_mobile(9557)].func = guild_guard;
    mob_index[real_mobile(9558)].func = guild_guard;
    mob_index[real_mobile(9559)].func = guild_guard;
    mob_index[real_mobile(9582)].func = train;
    mob_index[real_mobile(9538)].func = thief;
    mob_index[real_mobile(9544)].func = fido;    /* vulture */



    
    boot_the_shops();
    assign_the_shopkeepers();
}

/* assign special procedures to objects */

void assign_objects(void)
{
}


/* assign special procedures to rooms */
void assign_rooms(void)
{
}
