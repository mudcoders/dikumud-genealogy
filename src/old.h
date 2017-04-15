/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/


#define OLD_PLR_DEMON		2
#define OLD_PLR_CHAMPION	4
#define OLD_PLR_WOLFMAN		4194304
#define OLD_PLR_WEREWOLF	8388608
#define OLD_PLR_VAMPIRE         67108864

#define OLD_EXTRA_SIRE		1
#define OLD_EXTRA_PRINCE	2
#define OLD_EXTRA_ANARCH	1024
#define OLD_EXTRA_HIGHLANDER	262144	
#define OLD_EXTRA_INCONNU	524288	

/*
 *  Totems for werewolves.
 */
#define OLD_TOTEM_MANTIS      0
#define OLD_TOTEM_BEAR	      1
#define OLD_TOTEM_LYNX	      2
#define OLD_TOTEM_BOAR	      3
#define OLD_TOTEM_OWL	      4
#define OLD_TOTEM_SPIDER      5
#define OLD_TOTEM_WOLF	      6
#define OLD_TOTEM_HAWK	      7
#define OLD_TOTEM_SILVER      10

/*
 * Bits for Demonic Champions.
 */
#define OLD_POWER_CURRENT	      0
#define OLD_POWER_TOTAL		      1

#define OLD_C_POWERS		      1
#define OLD_C_CURRENT		      2
#define OLD_C_HEAD			      3
#define OLD_C_TAIL			      4
#define OLD_O_POWERS		      5

#define OLD_DEM_UNFOLDED		      1

#define OLD_DEM_FANGS		      1
#define OLD_DEM_CLAWS		      2
#define OLD_DEM_HORNS		      4
#define OLD_DEM_HOOVES		      8
#define OLD_DEM_EYES		     16
#define OLD_DEM_WINGS		     32
#define OLD_DEM_MIGHT		     64
#define OLD_DEM_TOUGH		    128
#define OLD_DEM_SPEED		    256
#define OLD_DEM_TRAVEL		    512
#define OLD_DEM_SCRY		   1024
#define OLD_DEM_SHADOWSIGHT		   2048

#define OLD_HEAD_NORMAL		      0
#define OLD_HEAD_WOLF		      1
#define OLD_HEAD_EAGLE		      2
#define OLD_HEAD_LION		      4
#define OLD_HEAD_SNAKE		      8

#define OLD_TAIL_NONE		      0
#define OLD_TAIL_POISONOUS		      1
#define OLD_TAIL_SCORPION		      2

#define OLD_DEM_MOVE		      1 /* Can roll as an object */
#define OLD_DEM_LEAP		      2 /* Can leap out of someone's hands */
#define OLD_DEM_MAGIC		      4 /* Can cast spells as an object */
#define OLD_DEM_LIFESPAN		      8 /* Can change lifespan */

