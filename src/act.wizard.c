/* ************************************************************************
*   File: act.wizard.c                                  Part of CircleMUD *
*  Usage: Player-level god commands and other goodies                     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"
#include "screen.h"

/*   external vars  */
extern FILE *player_fl;
extern struct room_data *world;
extern struct char_data *character_list;
extern struct obj_data *object_list;
extern struct descriptor_data *descriptor_list;
extern struct title_type titles[4][35];
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct int_app_type int_app[36];
extern struct wis_app_type wis_app[36];
extern struct zone_data *zone_table;
extern int	top_of_zone_table;
extern int	restrict;
extern int	top_of_world;
extern int	top_of_mobt;
extern int	top_of_objt;
extern int	top_of_p_table;

/* for objects */
extern char	*item_types[];
extern char	*wear_bits[];
extern char	*extra_bits[];
extern char	*drinks[];

/* for rooms */
extern char	*dirs[];
extern char	*room_bits[];
extern char	*exit_bits[];
extern char	*sector_types[];

/* for chars */
extern char	*spells[];
extern char	*equipment_types[];
extern char	*affected_bits[];
extern char	*apply_types[];
extern char	*pc_class_types[];
extern char	*npc_class_types[];
extern char	*action_bits[];
extern char	*player_bits[];
extern char	*preference_bits[];
extern char	*position_types[];
extern char	*connected_types[];

/* external functs */
void	reset_zone(int zone);
ACMD(do_look);

ACMD(do_emote)
{
   int	i;

   if (IS_NPC(ch))
      return;

   for (i = 0; *(argument + i) == ' '; i++)
      ;

   if (!*(argument + i))
      send_to_char("Yes.. But what?\n\r", ch);
   else {
      sprintf(buf, "$n %s", argument + i);
      act(buf, FALSE, ch, 0, 0, TO_ROOM);
      if (PRF_FLAGGED(ch, PRF_NOREPEAT))
	 send_to_char("Ok.\n\r", ch);
      else
	 act(buf, FALSE, ch, 0, 0, TO_CHAR);
   }
}


ACMD(do_send)
{
   struct char_data *vict;

   half_chop(argument, arg, buf);

   if (!*arg) {
      send_to_char("Send what to who?\n\r", ch);
      return;
   }

   if (!(vict = get_char_vis(ch, arg))) {
      send_to_char("No such person around.\n\r", ch);
      return;
   }

   send_to_char(buf, vict);
   send_to_char("\n\r", vict);
   if (PRF_FLAGGED(ch, PRF_NOREPEAT))
      send_to_char("Sent.\n\r", ch);
   else {
      sprintf(buf2, "You send '%s' to %s.\n\r", buf, GET_NAME(vict));
      send_to_char(buf2, ch);
   }
}



ACMD(do_echo)
{
   int	i;

   if (IS_NPC(ch))
      return;

   for (i = 0; *(argument + i) == ' '; i++)
      ;

   if (!*(argument + i))
      send_to_char("That must be a mistake...\n\r", ch);
   else {
      sprintf(buf, "%s\n\r", argument + i);
      send_to_room_except(buf, ch->in_room, ch);
      if (PRF_FLAGGED(ch, PRF_NOREPEAT))
	 send_to_char("Ok.\n\r", ch);
      else
	 send_to_char(buf, ch);
   }
}


/* take a string, and return an rnum.. used for goto, at, etc.  -je 4/6/93 */
sh_int find_target_room(struct char_data *ch, char *rawroomstr)
{
   int	tmp;
   sh_int location;
   struct char_data *target_mob;
   struct obj_data *target_obj;
   char	roomstr[MAX_INPUT_LENGTH];

   one_argument(rawroomstr, roomstr);

   if (!*roomstr) {
      send_to_char("You must supply a room number or name.\n\r", ch);
      return NOWHERE;
   }

   if (isdigit(*roomstr) && !strchr(roomstr, '.')) {
      tmp = atoi(roomstr);
      if ((location = real_room(tmp)) < 0) {
	 send_to_char("No room exists with that number.\n\r", ch);
	 return NOWHERE;
      }
   } else if ((target_mob = get_char_vis(ch, roomstr)))
      location = target_mob->in_room;
   else if ((target_obj = get_obj_vis(ch, roomstr))) {
      if (target_obj->in_room != NOWHERE)
	 location = target_obj->in_room;
      else {
	 send_to_char("That object is not available.\n\r", ch);
	 return NOWHERE;
      }
   } else {
      send_to_char("No such creature or object around.\n\r", ch);
      return NOWHERE;
   }

   /* a location has been found. */
   if (IS_SET(world[location].room_flags, GODROOM) && GET_LEVEL(ch) < LEVEL_GRGOD) {
      send_to_char("You are not godly enough to use that room!\n\r", ch);
      return NOWHERE;
   }

   if (IS_SET(world[location].room_flags, PRIVATE) && GET_LEVEL(ch) < LEVEL_GRGOD)
      if (world[location].people && world[location].people->next_in_room) {
	 send_to_char("There's a private conversation going on in that room.\n\r", ch);
	 return NOWHERE;
      }

   return location;
}



ACMD(do_at)
{
   char	command[MAX_INPUT_LENGTH];
   int	location, original_loc;

   if (IS_NPC(ch))
      return;

   half_chop(argument, buf, command);
   if (!*buf) {
      send_to_char("You must supply a room number or a name.\n\r", ch);
      return;
   }

   if ((location = find_target_room(ch, buf)) < 0)
      return;

   /* a location has been found. */
   original_loc = ch->in_room;
   char_from_room(ch);
   char_to_room(ch, location);
   command_interpreter(ch, command);

   /* check if the guy's still there */
   if (ch->in_room == location) {
      char_from_room(ch);
      char_to_room(ch, original_loc);
   }
}


ACMD(do_goto)
{
   sh_int location;

   if (IS_NPC(ch))
      return;

   if ((location = find_target_room(ch, argument)) < 0)
      return;

   if (ch->specials.poofOut)
      sprintf(buf, "$n %s", ch->specials.poofOut);
   else
      strcpy(buf, "$n disappears in a puff of smoke.");

   act(buf, TRUE, ch, 0, 0, TO_ROOM);
   char_from_room(ch);
   char_to_room(ch, location);

   if (ch->specials.poofIn)
      sprintf(buf, "$n %s", ch->specials.poofIn);
   else
      strcpy(buf, "$n appears with an ear-splitting bang.");

   act(buf, TRUE, ch, 0, 0, TO_ROOM);
   do_look(ch, "", 0, 0);
}



ACMD(do_trans)
{
   struct descriptor_data *i;
   struct char_data *victim;

   if (IS_NPC(ch))
      return;

   one_argument(argument, buf);
   if (!*buf)
      send_to_char("Whom do you wish to transfer?\n\r", ch);
   else if (str_cmp("all", buf)) {
      if (!(victim = get_char_vis(ch, buf)))
	 send_to_char("No-one by that name around.\n\r", ch);
      else if (victim == ch)
	 send_to_char("That doesn't make much sense, does it?\n\r", ch);
      else {
	 if ((GET_LEVEL(ch) < GET_LEVEL(victim)) && !IS_NPC(victim)) {
	    send_to_char("Go transfer someone your own size.\n\r", ch);
	    return;
	 }
	 act("$n disappears in a mushroom cloud.", FALSE, victim, 0, 0, TO_ROOM);
	 char_from_room(victim);
	 char_to_room(victim, ch->in_room);
	 act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
	 act("$n has transferred you!", FALSE, ch, 0, victim, TO_VICT);
	 do_look(victim, "", 0, 0);
      }
   } else { /* Trans All */
      for (i = descriptor_list; i; i = i->next)
	 if (!i->connected && i->character && i->character != ch) {
	    victim = i->character;
	    act("$n disappears in a mushroom cloud.", FALSE, victim, 0, 0, TO_ROOM);
	    char_from_room(victim);
	    char_to_room(victim, ch->in_room);
	    act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
	    act("$n has transferred you!", FALSE, ch, 0, victim, TO_VICT);
	    do_look(victim, "", 0, 0);
	 }

      send_to_char("Ok.\n\r", ch);
   }
}



ACMD(do_teleport)
{
   struct char_data *victim;
   sh_int target;

   if (IS_NPC(ch))
      return;

   half_chop(argument, buf, buf2);

   if (!*buf)
      send_to_char("Who do you wish to teleport?\n\r", ch);
   else if (!(victim = get_char_vis(ch, buf)))
      send_to_char("No-one by that name around.\n\r", ch);
   else if (victim == ch)
      send_to_char("Use 'goto' to teleport yourself.\n\r", ch);
   else if (!*buf2)
      send_to_char("Where do you wish to send this person?\n\r", ch);
   else if ((target = find_target_room(ch, buf2)) >= 0) {
      act("$n disappears in a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
      char_from_room(victim);
      char_to_room(victim, target);
      act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
      act("$n has teleported you!", FALSE, ch, 0, (char *)victim, TO_VICT);
      do_look(victim, "", 0, 0);
   }
}



ACMD(do_vnum)
{
   if (IS_NPC(ch)) {
      send_to_char("What would a monster do with a vnum?\n\r", ch);
      return;
   }

   half_chop(argument, buf, buf2);

   if (!*buf || !*buf2 || (!is_abbrev(buf, "mob") && !is_abbrev(buf, "obj"))) {
      send_to_char("Usage: vnum { obj | mob } <name>\n\r", ch);
      return;
   }

   if (is_abbrev(buf, "mob"))
      if (!vnum_mobile(buf2, ch))
	 send_to_char("No mobiles by that name.\n\r", ch);

   if (is_abbrev(buf, "obj"))
      if (!vnum_object(buf2, ch))
	 send_to_char("No objects by that name.\n\r", ch);
}



void	do_stat_room(struct char_data *ch)
{
   struct extra_descr_data *desc;
   struct room_data *rm = &world[ch->in_room];
   int	i, found = 0;
   struct obj_data *j = 0;
   struct char_data *k = 0;

   sprintf(buf, "Room name: %s%s%s\n\r", CCCYN(ch, C_NRM), rm->name,
      CCNRM(ch, C_NRM));
   send_to_char(buf, ch);

   sprinttype(rm->sector_type, sector_types, buf2);
   sprintf(buf, "Zone: [%3d], VNum: [%s%5d%s], RNum: [%5d], Type: %s\n\r",
       rm->zone, CCGRN(ch, C_NRM), rm->number, CCNRM(ch, C_NRM), ch->in_room, buf2);
   send_to_char(buf, ch);

   sprintbit((long) rm->room_flags, room_bits, buf2);
   sprintf(buf, "SpecProc: %s, Flags: %s\n\r", (rm->funct) ? "Exists" : "No",
      buf2);
   send_to_char(buf, ch);

   send_to_char("Description:\n\r", ch);
   if (rm->description)
      send_to_char(rm->description, ch);
   else
      send_to_char("  None.\n\r", ch);

   if (rm->ex_description) {
      sprintf(buf, "Extra descs:%s", CCCYN(ch, C_NRM));
      for (desc = rm->ex_description; desc; desc = desc->next) {
	 strcat(buf, " ");
	 strcat(buf, desc->keyword);
      }
      strcat(buf, CCNRM(ch, C_NRM));
      send_to_char(strcat(buf, "\n\r"), ch);
   }

   sprintf(buf, "Chars present:%s", CCYEL(ch, C_NRM));
   for (found = 0, k = rm->people; k; k = k->next_in_room) {
      if (!CAN_SEE(ch, k))
	 continue;
      sprintf(buf2, "%s %s(%s)", found++ ? "," : "", GET_NAME(k),
	 (!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")));
      strcat(buf, buf2);
      if (strlen(buf) >= 62) {
	 if (k->next_in_room)
	    send_to_char(strcat(buf, ",\n\r"), ch);
	 else
	    send_to_char(strcat(buf, "\n\r"), ch);
	 *buf = found = 0;
      }
   }

   if (*buf)
      send_to_char(strcat(buf, "\n\r"), ch);
   send_to_char(CCNRM(ch, C_NRM), ch);

   if (rm->contents) {
      sprintf(buf, "Contents:%s", CCGRN(ch, C_NRM));
      for (found = 0, j = rm->contents; j; j = j->next_content) {
	 if (!CAN_SEE_OBJ(ch, j))
	    continue;
	 sprintf(buf2, "%s %s", found++ ? "," : "", j->short_description);
	 strcat(buf, buf2);
	 if (strlen(buf) >= 62) {
	    if (j->next_content)
	       send_to_char(strcat(buf, ",\n\r"), ch);
	    else
	       send_to_char(strcat(buf, "\n\r"), ch);
	    *buf = found = 0;
	 }
      }

      if (*buf)
	 send_to_char(strcat(buf, "\n\r"), ch);
      send_to_char(CCNRM(ch, C_NRM), ch);
   }


   for (i = 0; i < NUM_OF_DIRS; i++) {
      if (rm->dir_option[i]) {
	 if (rm->dir_option[i]->to_room == NOWHERE)
	    sprintf(buf1, " %sNONE%s", CCCYN(ch, C_NRM), CCNRM(ch, C_NRM));
	 else
	    sprintf(buf1, "%s%5d%s", CCCYN(ch, C_NRM),
	       world[rm->dir_option[i]->to_room].number, CCNRM(ch, C_NRM));
	 sprintbit(rm->dir_option[i]->exit_info, exit_bits, buf2);
	 sprintf(buf, "Exit %s%-5s%s:  To: [%s], Key: [%5d], Keywrd: %s, Type: %s\n\r ",
	     CCCYN(ch, C_NRM), dirs[i], CCNRM(ch, C_NRM), buf1, rm->dir_option[i]->key,
	     rm->dir_option[i]->keyword ? rm->dir_option[i]->keyword : "None",
	     buf2);
	 send_to_char(buf, ch);
	 if (rm->dir_option[i]->general_description)
	    strcpy(buf, rm->dir_option[i]->general_description);
	 else
	    strcpy(buf, "  No exit description.\n\r");
	 send_to_char(buf, ch);
      }
   }
}



void do_stat_object(struct char_data *ch, struct obj_data *j)
{
   bool found;
   int	i;
   struct obj_data *j2;
   struct extra_descr_data *desc;
   int	virtual;

   virtual = (j->item_number >= 0) ? obj_index[j->item_number].virtual : 0;
   sprintf(buf, "Name: '%s%s%s', Aliases: %s\n\r", CCYEL(ch, C_NRM),
      ((j->short_description) ? j->short_description : "<None>"),
      CCNRM(ch, C_NRM), j->name);
   send_to_char(buf, ch);
   sprinttype(GET_ITEM_TYPE(j), item_types, buf1);
   if (j->item_number >= 0)
      strcpy(buf2, (obj_index[j->item_number].func ? "Exists" : "None"));
   else
      strcpy(buf2, "None");
   sprintf(buf, "VNum: [%s%5d%s], RNum: [%5d], Type: %s, SpecProc: %s\n\r",
      CCGRN(ch, C_NRM), virtual, CCNRM(ch, C_NRM), j->item_number, buf1, buf2);
   send_to_char(buf, ch);
   sprintf(buf, "L-Des: %s\n\r", ((j->description) ? j->description : "None"));
   send_to_char(buf, ch);

   if (j->ex_description) {
      sprintf(buf, "Extra descs:%s", CCCYN(ch, C_NRM));
      for (desc = j->ex_description; desc; desc = desc->next) {
	 strcat(buf, " ");
	 strcat(buf, desc->keyword);
      }
      strcat(buf, CCNRM(ch, C_NRM));
      send_to_char(strcat(buf, "\n\r"), ch);
   }

   send_to_char("Can be worn on: ", ch);
   sprintbit(j->obj_flags.wear_flags, wear_bits, buf);
   strcat(buf, "\n\r");
   send_to_char(buf, ch);

   send_to_char("Set char bits : ", ch);
   sprintbit(j->obj_flags.bitvector, affected_bits, buf);
   strcat(buf, "\n\r");
   send_to_char(buf, ch);

   send_to_char("Extra flags   : ", ch);
   sprintbit(j->obj_flags.extra_flags, extra_bits, buf);
   strcat(buf, "\n\r");
   send_to_char(buf, ch);

   sprintf(buf, "Weight: %d, Value: %d, Cost/day: %d, Timer: %d\n\r",
       j->obj_flags.weight, j->obj_flags.cost,
       j->obj_flags.cost_per_day,  j->obj_flags.timer);
   send_to_char(buf, ch);

   strcpy(buf, "In room: ");
   if (j->in_room == NOWHERE)
      strcat(buf, "Nowhere");
   else {
      sprintf(buf2, "%d", world[j->in_room].number);
      strcat(buf, buf2);
   }
   strcat(buf, ", In object: ");
   strcat(buf, j->in_obj ? j->in_obj->short_description: "None");
   strcat(buf, ", Carried by: ");
   strcat(buf, j->carried_by ? GET_NAME(j->carried_by) : "Nobody");
   strcat(buf, "\n\r");
   send_to_char(buf, ch);

   switch (j->obj_flags.type_flag) {
   case ITEM_LIGHT :
      sprintf(buf, "Color: [%d], Type: [%d], Hours: [%d]",
          j->obj_flags.value[0], j->obj_flags.value[1], j->obj_flags.value[2]);
      break;
   case ITEM_SCROLL :
   case ITEM_POTION :
      sprintf(buf, "Spells: %d, %d, %d, %d", j->obj_flags.value[0],
	  j->obj_flags.value[1], j->obj_flags.value[2], j->obj_flags.value[3]);
      break;
   case ITEM_WAND :
   case ITEM_STAFF :
      sprintf(buf, "Spell: %d, Mana: %d", j->obj_flags.value[0],
	 j->obj_flags.value[1]);
      break;
   case ITEM_FIREWEAPON :
   case ITEM_WEAPON :
      sprintf(buf, "Tohit: %d, Todam: %dd%d, Type: %d", j->obj_flags.value[0],
          j->obj_flags.value[1], j->obj_flags.value[2], j->obj_flags.value[3]);
      break;
   case ITEM_MISSILE :
      sprintf(buf, "Tohit: %d, Todam: %d, Type: %d", j->obj_flags.value[0],
          j->obj_flags.value[1], j->obj_flags.value[3]);
      break;
   case ITEM_ARMOR :
      sprintf(buf, "AC-apply: [%d]", j->obj_flags.value[0]);
      break;
   case ITEM_TRAP :
      sprintf(buf, "Spell: %d, - Hitpoints: %d",
          j->obj_flags.value[0], j->obj_flags.value[1]);
      break;
   case ITEM_CONTAINER :
      sprintf(buf, "Max-contains: %d, Locktype: %d, Corpse: %s",
          j->obj_flags.value[0], j->obj_flags.value[1],
          j->obj_flags.value[3] ? "Yes" : "No");
      break;
   case ITEM_DRINKCON :
   case ITEM_FOUNTAIN :
      sprinttype(j->obj_flags.value[2], drinks, buf2);
      sprintf(buf, "Max-contains: %d, Contains: %d, Poisoned: %s, Liquid: %s",
          j->obj_flags.value[0], j->obj_flags.value[1],
          j->obj_flags.value[3] ? "Yes" : "No", buf2);
      break;
   case ITEM_NOTE :
      sprintf(buf, "Tounge: %d", j->obj_flags.value[0]);
      break;
   case ITEM_KEY :
      sprintf(buf, "Keytype: %d", j->obj_flags.value[0]);
      break;
   case ITEM_FOOD :
      sprintf(buf, "Makes full: %d, Poisoned: %d",
          j->obj_flags.value[0], j->obj_flags.value[3]);
      break;
   default :
      sprintf(buf, "Values 0-3: [%d] [%d] [%d] [%d]",
          j->obj_flags.value[0], j->obj_flags.value[1],
          j->obj_flags.value[2], j->obj_flags.value[3]);
      break;
   }
   send_to_char(buf, ch);

   strcpy(buf, "\n\rEquipment Status: ");
   if (!j->carried_by)
      strcat(buf, "None");
   else {
      found = FALSE;
      for (i = 0; i < MAX_WEAR; i++) {
	 if (j->carried_by->equipment[i] == j) {
	    sprinttype(i, equipment_types, buf2);
	    strcat(buf, buf2);
	    found = TRUE;
	 }
      }
      if (!found)
	 strcat(buf, "Inventory");
   }
   send_to_char(strcat(buf, "\n\r"), ch);

   if (j->contains) {
      sprintf(buf, "Contents:%s", CCGRN(ch, C_NRM));
      for (found = 0, j2 = j->contains; j2; j2 = j2->next_content) {
	 sprintf(buf2, "%s %s", found++ ? "," : "", j2->short_description);
	 strcat(buf, buf2);
	 if (strlen(buf) >= 62) {
	    if (j2->next_content)
	       send_to_char(strcat(buf, ",\n\r"), ch);
	    else
	       send_to_char(strcat(buf, "\n\r"), ch);
	    *buf = found = 0;
	 }
      }

      if (*buf)
	 send_to_char(strcat(buf, "\n\r"), ch);
      send_to_char(CCNRM(ch, C_NRM), ch);
   }

   found = 0;
   send_to_char("Affections:", ch);
   for (i = 0; i < MAX_OBJ_AFFECT; i++)
      if (j->affected[i].modifier) {
	 sprinttype(j->affected[i].location, apply_types, buf2);
	 sprintf(buf, "%s %+d to %s", found++ ? "," : "",
	    j->affected[i].modifier, buf2);
      send_to_char(buf, ch);
   }
   if (!found)
      send_to_char(" None", ch);

   send_to_char("\n\r", ch);
}


void do_stat_character(struct char_data *ch, struct char_data *k)
{
   int	i, i2, found = 0;
   struct obj_data *j;
   struct follow_type *fol;
   struct affected_type *aff;

   switch (k->player.sex) {
   case SEX_NEUTRAL	: strcpy(buf, "NEUTRAL-SEX"); break;
   case SEX_MALE	: strcpy(buf, "MALE"); break;
   case SEX_FEMALE	: strcpy(buf, "FEMALE"); break;
   default		: strcpy(buf, "ILLEGAL-SEX!!"); break;
   }

   sprintf(buf2, " %s '%s'  IDNum: [%5d], In room [%5d]\n\r",
       (!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")),
       GET_NAME(k), GET_IDNUM(k), world[k->in_room].number);
   send_to_char(strcat(buf, buf2), ch);
   if (IS_MOB(k)) {
      sprintf(buf, "Alias: %s, VNum: [%5d], RNum: [%5d]\n\r",
	 k->player.name, mob_index[k->nr].virtual, k->nr);
      send_to_char(buf, ch);
   }

   sprintf(buf, "Title: %s\n\r", (k->player.title ? k->player.title : "<None>"));
   send_to_char(buf, ch);

   sprintf(buf, "L-Des: %s", (k->player.long_descr ? k->player.long_descr : "<None>\n\r"));
   send_to_char(buf, ch);

   if (IS_NPC(k)) {
      strcpy(buf, "Monster Class: ");
      sprinttype(k->player.class, npc_class_types, buf2);
   } else {
      strcpy(buf, "Class: ");
      sprinttype(k->player.class, pc_class_types, buf2);
   }
   strcat(buf, buf2);

   sprintf(buf2, ", Lev: [%s%2d%s], XP: [%s%7d%s], Align: [%4d]\n\r",
      CCYEL(ch, C_NRM), GET_LEVEL(k), CCNRM(ch, C_NRM),
      CCYEL(ch, C_NRM), GET_EXP(k), CCNRM(ch, C_NRM),
      GET_ALIGNMENT(k));
   strcat(buf, buf2);
   send_to_char(buf, ch);

   if (!IS_NPC(k)) {
      strcpy(buf1, (char *)asctime(localtime(&(k->player.time.birth))));
      strcpy(buf2, (char *)asctime(localtime(&(k->player.time.logon))));
      buf1[10] = buf2[10] = '\0';

      sprintf(buf, "Created: [%s], Last Logon: [%s], Played [%dh %dm], Age [%d]\n\r", 
	 buf1, buf2, k->player.time.played / 3600,
	 ((k->player.time.played / 3600) % 60), age(k).year);
      send_to_char(buf, ch);

      sprintf(buf, "Hometown: [%d], Speaks: [%d/%d/%d], (STL[%d]/per[%d]/NSTL[%d])\n\r",
	 k->player.hometown, k->player.talks[0], k->player.talks[1],
	 k->player.talks[2], SPELLS_TO_LEARN(k), int_app[GET_INT(k)].learn,
	 wis_app[GET_WIS(k)].bonus);
      send_to_char(buf, ch);
   }

      sprintf(buf, "Str: [%s%d/%d%s]  Int: [%s%d%s]  Wis: [%s%d%s]  Dex: [%s%d%s]  Con: [%s%d%s]\n\r",
	 CCCYN(ch, C_NRM), GET_STR(k), GET_ADD(k), CCNRM(ch, C_NRM),
	 CCCYN(ch, C_NRM), GET_INT(k), CCNRM(ch, C_NRM),
	 CCCYN(ch, C_NRM), GET_WIS(k), CCNRM(ch, C_NRM),
	 CCCYN(ch, C_NRM), GET_DEX(k), CCNRM(ch, C_NRM),
	 CCCYN(ch, C_NRM), GET_CON(k), CCNRM(ch, C_NRM));
      send_to_char(buf, ch);

   sprintf(buf, "Hit p.:[%s%d/%d+%d%s]  Mana p.:[%s%d/%d+%d%s]  Move p.:[%s%d/%d+%d%s]\n\r",
       CCGRN(ch, C_NRM), GET_HIT(k), GET_MAX_HIT(k), hit_gain(k), CCNRM(ch, C_NRM),
       CCGRN(ch, C_NRM), GET_MANA(k), GET_MAX_MANA(k), mana_gain(k), CCNRM(ch, C_NRM),
       CCGRN(ch, C_NRM), GET_MOVE(k), GET_MAX_MOVE(k), move_gain(k), CCNRM(ch, C_NRM));
   send_to_char(buf, ch);

   sprintf(buf, "Coins: [%9d], Bank: [%9d] (Total: %d)\n\r",
       GET_GOLD(k), GET_BANK_GOLD(k), GET_GOLD(k) + GET_BANK_GOLD(k));
   send_to_char(buf, ch);

   sprintf(buf, "AC: [%d/10], Hitroll: [%2d], Damroll: [%2d], Saving throws: [%d/%d/%d/%d/%d]\n\r",
       GET_AC(k), k->points.hitroll, k->points.damroll,
       k->specials2.apply_saving_throw[0], k->specials2.apply_saving_throw[1],
       k->specials2.apply_saving_throw[2], k->specials2.apply_saving_throw[3],
       k->specials2.apply_saving_throw[4]);
   send_to_char(buf, ch);

   sprinttype(GET_POS(k), position_types, buf2);
   sprintf(buf, "Pos: %s, Fighting: %s", buf2,
       ((k->specials.fighting) ? GET_NAME(k->specials.fighting) : "Nobody") );
   if (k->desc) {
      sprinttype(k->desc->connected, connected_types, buf2);
      strcat(buf, ", Connected: ");
      strcat(buf, buf2);
   }
   send_to_char(strcat(buf, "\n\r"), ch);

   strcpy(buf, "Default position: ");
   sprinttype((k->specials.default_pos), position_types, buf2);
   strcat(buf, buf2);

   sprintf(buf2, ", Idle Timer (in tics) [%d]\n\r", k->specials.timer);
   strcat(buf, buf2);
   send_to_char(buf, ch);

   if (IS_NPC(k)) {
      sprintbit(MOB_FLAGS(k), action_bits, buf2);
      sprintf(buf, "NPC flags: %s%s%s\n\r", CCCYN(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
      send_to_char(buf, ch);
   } else {
      sprintbit(PLR_FLAGS(k), player_bits, buf2);
      sprintf(buf, "PLR: %s%s%s\n\r", CCCYN(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
      send_to_char(buf, ch);
      sprintbit(PRF_FLAGS(k), preference_bits, buf2);
      sprintf(buf, "PRF: %s%s%s\n\r", CCGRN(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
      send_to_char(buf, ch);
   }

   if (IS_MOB(k)) {
      sprintf(buf, "Mob Spec-Proc: %s, NPC Bare Hand Dam: %dd%d\n\r",
	(mob_index[k->nr].func ? "Exists" : "None"),
        k->specials.damnodice, k->specials.damsizedice);
      send_to_char(buf, ch);
   }

   sprintf(buf, "Carried: weight: %d, items: %d; ",
       IS_CARRYING_W(k), IS_CARRYING_N(k));

   for (i = 0, j = k->carrying; j; j = j->next_content, i++)
      ;
   sprintf(buf, "%sItems in: inventory: %d, ", buf, i);

   for (i = 0, i2 = 0; i < MAX_WEAR; i++)
      if (k->equipment[i])
	 i2++;
   sprintf(buf2, "eq: %d\n\r", i2);
   strcat(buf, buf2);
   send_to_char(buf, ch);

   sprintf(buf, "Hunger: %d, Thirst: %d, Drunk: %d\n\r",
       GET_COND(k, FULL), GET_COND(k, THIRST), GET_COND(k, DRUNK));
   send_to_char(buf, ch);

   sprintf(buf, "Master is: %s, Followers are:",
      ((k->master) ? GET_NAME(k->master) : "<none>"));

   for (fol = k->followers; fol; fol = fol->next) {
      sprintf(buf2, "%s %s", found++ ? "," : "", PERS(fol->follower, ch));
      strcat(buf, buf2);
      if (strlen(buf) >= 62) {
	 if (fol->next)
	    send_to_char(strcat(buf, ",\n\r"), ch);
	 else
	    send_to_char(strcat(buf, "\n\r"), ch);
	 *buf = found = 0;
      }
   }

   if (*buf)
      send_to_char(strcat(buf, "\n\r"), ch);

   /* Showing the bitvector */
   sprintbit(k->specials.affected_by, affected_bits, buf2);
   sprintf(buf, "AFF: %s%s%s\n\r", CCYEL(ch, C_NRM), buf2, CCNRM(ch, C_NRM));
   send_to_char(buf, ch);

   /* Routine to show what spells a char is affected by */
   if (k->affected) {
      for (aff = k->affected; aff; aff = aff->next) {
	 *buf2 = '\0';
	 sprintf(buf, "SPL: (%3dhr) %s%-21s%s ", aff->duration+1,
	    CCCYN(ch, C_NRM), spells[aff->type-1], CCNRM(ch, C_NRM));
	 if (aff->modifier) {
	    sprintf(buf2, "%+d to %s", aff->modifier, apply_types[(int)aff->location]);
	    strcat(buf, buf2);
	 }
	 if (aff->bitvector) {
	    if (*buf2)
	       strcat(buf, ", sets ");
	    else
	       strcat(buf, "sets ");
	    sprintbit(aff->bitvector, affected_bits, buf2);
	    strcat(buf, buf2);
	 }
	 send_to_char(strcat(buf, "\n\r"), ch);
      }
   }
}


ACMD(do_stat)
{
   struct char_data *victim = 0;
   struct obj_data *object = 0;
   struct char_file_u tmp_store;

   if (IS_NPC(ch))
      return;

   half_chop(argument, buf1, buf2);

   if (!*buf1) {
      send_to_char("Stats on who or what?\n\r", ch);
      return;
   } else if (is_abbrev(buf1, "room")) {
      do_stat_room(ch);
   } else if (is_abbrev(buf1, "mob")) {
      if (!*buf2) {
	 send_to_char("Stats on which mobile?\n\r", ch);
      } else {
	 if ((victim = get_char_vis(ch, buf2)))
	    do_stat_character(ch, victim);
	 else
	    send_to_char("No such mobile around.\n\r", ch);
      }
   } else if (is_abbrev(buf1, "player")) {
      if (!*buf2) {
	 send_to_char("Stats on which player?\n\r", ch);
      } else {
	 if ((victim = get_player_vis(ch, buf2)))
	    do_stat_character(ch, victim);
	 else
	    send_to_char("No such player around.\n\r", ch);
      }
   } else if (is_abbrev(buf1, "file")) {
      if (!*buf2) {
	 send_to_char("Stats on which player?\n\r", ch);
      } else {
	 CREATE(victim, struct char_data, 1);
	 clear_char(victim);
	 if (load_char(buf2, &tmp_store) > -1) {
	    store_to_char(&tmp_store, victim);
	    if (GET_LEVEL(victim) > GET_LEVEL(ch))
	       send_to_char("Sorry, you can't do that.\n\r", ch);
	    else
	       do_stat_character(ch, victim);
	    free_char(victim);
	 } else {
	    send_to_char("There is no such player.\n\r", ch);
	    free(victim);
	 }
      }
   } else if (is_abbrev(buf1, "object")) {
      if (!*buf2) {
	 send_to_char("Stats on which object?\n\r", ch);
      } else {
	 if ((object = get_obj_vis(ch, buf2)))
	    do_stat_object(ch, object);
	 else
	    send_to_char("No such object around.\n\r", ch);
      }
   } else {
      if ((victim = get_char_vis(ch, buf1)))
	 do_stat_character(ch, victim);
      else if ((object = get_obj_vis(ch, buf1)))
	 do_stat_object(ch, object);
      else
	 send_to_char("Nothing around by that name.\n\r", ch);
   }
}


ACMD(do_shutdown)
{
   extern int	circle_shutdown, circle_reboot;

   if (IS_NPC(ch))
      return;

   if (subcmd != SCMD_SHUTDOWN) {
      send_to_char("If you want to shut something down, say so!\n\r", ch);
      return;
   }

   one_argument(argument, arg);

   if (!*arg) {
      sprintf(buf, "(GC) Shutdown by %s.", GET_NAME(ch));
      send_to_all(buf);
      log(buf);
      circle_shutdown = 1;
   } else if (!str_cmp(arg, "reboot")) {
      sprintf(buf, "(GC) Reboot by %s.", GET_NAME(ch));
      log(buf);
      send_to_all("Rebooting.. come back in a minute or two.");
      system("touch ../.fastboot");
      circle_shutdown = circle_reboot = 1;
   } else if (!str_cmp(arg, "die")) {
      sprintf(buf, "(GC) Shutdown by %s.", GET_NAME(ch));
      send_to_all(buf);
      log(buf);
      system("touch ../.killscript");
      circle_shutdown = 1;
   } else if (!str_cmp(arg, "pause")) {
      sprintf(buf, "(GC) Shutdown by %s.", GET_NAME(ch));
      send_to_all(buf);
      log(buf);
      system("touch ../pause");
      circle_shutdown = 1;
   } else
      send_to_char("Unknown shutdown option.\n\r", ch);
}




ACMD(do_snoop)
{
   struct char_data *victim;

   if (!ch->desc)
      return;

   one_argument(argument, arg);

   if (!*arg) {
      send_to_char("Snoop who?\n\r", ch);
      return;
   }

   if (!(victim = get_char_vis(ch, arg))) {
      send_to_char("No such person around.\n\r", ch);
      return;
   }

   if (!victim->desc) {
      send_to_char("There's no link.. nothing to snoop.\n\r", ch);
      return;
   }

   if (victim == ch) {
      send_to_char("Ok, you just snoop yourself.\n\r", ch);
      if (ch->desc->snoop.snooping) {
	 ch->desc->snoop.snooping->desc->snoop.snoop_by = 0;
	 ch->desc->snoop.snooping = 0;
      }
      return;
   }

   if (victim->desc->snoop.snoop_by) {
      send_to_char("Busy already. \n\r", ch);
      return;
   }

   if (GET_LEVEL(victim) >= GET_LEVEL(ch)) {
      send_to_char("You failed.\n\r", ch);
      return;
   }

   send_to_char("Ok. \n\r", ch);

   if (ch->desc->snoop.snooping)
      ch->desc->snoop.snooping->desc->snoop.snoop_by = 0;

   ch->desc->snoop.snooping = victim;
   victim->desc->snoop.snoop_by = ch;
   return;
}



ACMD(do_switch)
{
   struct char_data *victim;

   if (IS_NPC(ch))
      return;

   one_argument(argument, arg);

   if (!*arg) {
      send_to_char("Switch with who?\n\r", ch);
   } else {
      if (!(victim = get_char(arg)))
	 send_to_char("They aren't here.\n\r", ch);
      else {
	 if (ch == victim) {
	    send_to_char("He he he... We are jolly funny today, eh?\n\r", ch);
	    return;
	 }

	 if (!ch->desc || ch->desc->snoop.snoop_by || ch->desc->snoop.snooping) {
	    send_to_char("Mixing snoop & switch is bad for your health.\n\r", ch);
	    return;
	 }

	 if (victim->desc || (!IS_NPC(victim))) {
	    send_to_char("You can't do that, the body is already in use!\n\r", ch);
	 } else {
	    send_to_char("Ok.\n\r", ch);

	    ch->desc->character = victim;
	    ch->desc->original = ch;

	    victim->desc = ch->desc;
	    ch->desc = 0;
	 }
      }
   }
}



ACMD(do_return)
{
   if (!ch->desc)
      return;

   if (!ch->desc->original) {
      send_to_char("Yeah, right...\n\r", ch);
      return;
   } else {
      send_to_char("You return to your original body.\n\r", ch);

      ch->desc->character = ch->desc->original;
      ch->desc->original = 0;

      ch->desc->character->desc = ch->desc;
      ch->desc = 0;
   }
}



ACMD(do_load)
{
   struct char_data *mob;
   struct obj_data *obj;
   int	number, r_num;

   if (IS_NPC(ch))
      return;

   argument_interpreter(argument, buf, buf2);

   if (!*buf || !*buf2 || !isdigit(*buf2)) {
      send_to_char("Usage: load { obj | mob } <number>\n\r", ch);
      return;
   }

   if ((number = atoi(buf2)) < 0) {
      send_to_char("A NEGATIVE number??\n\r", ch);
      return;
   }
   if (is_abbrev(buf, "mob")) {
      if ((r_num = real_mobile(number)) < 0) {
	 send_to_char("There is no monster with that number.\n\r", ch);
	 return;
      }
      mob = read_mobile(r_num, REAL);
      char_to_room(mob, ch->in_room);

      act("$n makes a quaint, magical gesture with one hand.", TRUE, ch,
          0, 0, TO_ROOM);
      act("$n has created $N!", FALSE, ch, 0, mob, TO_ROOM);
      act("You create $N.", FALSE, ch, 0, mob, TO_CHAR);
   } else if (is_abbrev(buf, "obj")) {
      if ((r_num = real_object(number)) < 0) {
	 send_to_char("There is no object with that number.\n\r", ch);
	 return;
      }
      obj = read_object(r_num, REAL);
      obj_to_room(obj, ch->in_room);
      act("$n makes a strange magical gesture.", TRUE, ch, 0, 0, TO_ROOM);
      act("$n has created $p!", FALSE, ch, obj, 0, TO_ROOM);
      act("You create $p.", FALSE, ch, obj, 0, TO_CHAR);
   } else
      send_to_char("That'll have to be either 'obj' or 'mob'.\n\r", ch);
}



ACMD(do_vstat)
{
   struct char_data *mob;
   struct obj_data *obj;
   int	number, r_num;

   if (IS_NPC(ch))
      return;

   argument_interpreter(argument, buf, buf2);

   if (!*buf || !*buf2 || !isdigit(*buf2)) {
      send_to_char("Usage: vstat { obj | mob } <number>\n\r", ch);
      return;
   }

   if ((number = atoi(buf2)) < 0) {
      send_to_char("A NEGATIVE number??\n\r", ch);
      return;
   }
   if (is_abbrev(buf, "mob")) {
      if ((r_num = real_mobile(number)) < 0) {
	 send_to_char("There is no monster with that number.\n\r", ch);
	 return;
      }
      mob = read_mobile(r_num, REAL);
      do_stat_character(ch, mob);
      extract_char(mob);
   } else if (is_abbrev(buf, "obj")) {
      if ((r_num = real_object(number)) < 0) {
	 send_to_char("There is no object with that number.\n\r", ch);
	 return;
      }
      obj = read_object(r_num, REAL);
      do_stat_object(ch, obj);
      extract_obj(obj);
   } else
      send_to_char("That'll have to be either 'obj' or 'mob'.\n\r", ch);
}





/* clean a room of all mobiles and objects */
ACMD(do_purge)
{
   struct char_data *vict, *next_v;
   struct obj_data *obj, *next_o;

   if (IS_NPC(ch))
      return;

   one_argument(argument, buf);

   if (*buf)  /* argument supplied. destroy single object or char */ {
      if ((vict = get_char_room_vis(ch, buf))) {
	 if (!IS_NPC(vict) && (GET_LEVEL(ch) <= GET_LEVEL(vict))) {
	    send_to_char("Fuuuuuuuuu!\n\r", ch);
	    return;
	 }

	 act("$n disintegrates $N.", FALSE, ch, 0, vict, TO_NOTVICT);

	 if (IS_NPC(vict)) {
	    extract_char(vict);
	 } else {
	    sprintf(buf, "(GC) %s has purged %s.", GET_NAME(ch), GET_NAME(vict));
	    mudlog(buf, BRF, LEVEL_GOD, TRUE);
	    if (vict->desc) {
	       close_socket(vict->desc);
	       vict->desc = 0;
	    }
	    extract_char(vict);
	 }
      } else if ((obj = get_obj_in_list_vis(ch, buf, world[ch->in_room].contents))) {
	 act("$n destroys $p.", FALSE, ch, obj, 0, TO_ROOM);
	 extract_obj(obj);
      } else {
	 send_to_char("I don't know anyone or anything by that name.\n\r", ch);
	 return;
      }

      send_to_char("Ok.\n\r", ch);
   } else { /* no argument. clean out the room */
      if (IS_NPC(ch)) {
	 send_to_char("Don't... You would only kill yourself..\n\r", ch);
	 return;
      }

      act("$n gestures... You are surrounded by scorching flames!", 
          FALSE, ch, 0, 0, TO_ROOM);
      send_to_room("The world seems a little cleaner.\n\r", ch->in_room);

      for (vict = world[ch->in_room].people; vict; vict = next_v) {
	 next_v = vict->next_in_room;
	 if (IS_NPC(vict))
	    extract_char(vict);
      }

      for (obj = world[ch->in_room].contents; obj; obj = next_o) {
	 next_o = obj->next_content;
	 extract_obj(obj);
      }
   }
}



/* Give pointers to the five abilities */
void	roll_abilities(struct char_data *ch)
{
   int	i, j, k, temp;
   ubyte table[5];
   ubyte rools[4];

   for (i = 0; i < 5; table[i++] = 0)
      ;

   for (i = 0; i < 5; i++) {

      for (j = 0; j < 4; j++)
	 rools[j] = number(1, 6);

      temp = rools[0] + rools[1] + rools[2] + rools[3] - 
          MIN(rools[0], MIN(rools[1], MIN(rools[2], rools[3])));

      for (k = 0; k < 5; k++)
	 if (table[k] < temp)
	    SWITCH(temp, table[k]);
   }

   ch->abilities.str_add = 0;

   switch (GET_CLASS(ch)) {
   case CLASS_MAGIC_USER:
      ch->abilities.intel = table[0];
      ch->abilities.wis = table[1];
      ch->abilities.dex = table[2];
      ch->abilities.str = table[3];
      ch->abilities.con = table[4];
      break;
   case CLASS_CLERIC:
      ch->abilities.wis = table[0];
      ch->abilities.intel = table[1];
      ch->abilities.str = table[2];
      ch->abilities.dex = table[3];
      ch->abilities.con = table[4];
      break;
   case CLASS_THIEF:
      ch->abilities.dex = table[0];
      ch->abilities.str = table[1];
      ch->abilities.con = table[2];
      ch->abilities.intel = table[3];
      ch->abilities.wis = table[4];
      break;
   case CLASS_WARRIOR:
      ch->abilities.str = table[0];
      ch->abilities.dex = table[1];
      ch->abilities.con = table[2];
      ch->abilities.wis = table[3];
      ch->abilities.intel = table[4];
      if (ch->abilities.str == 18)
	 ch->abilities.str_add = number(0, 100);
      break;
   }
   ch->tmpabilities = ch->abilities;
}



void	do_start(struct char_data *ch)
{
   void	advance_level(struct char_data *ch);

   GET_LEVEL(ch) = 1;
   GET_EXP(ch) = 1;

   set_title(ch);

   roll_abilities(ch);

   ch->points.max_hit  = 10;  /* These are BASE numbers   */

   switch (GET_CLASS(ch)) {

   case CLASS_MAGIC_USER :
      break;

   case CLASS_CLERIC :
      break;

   case CLASS_THIEF :
      SET_SKILL(ch, SKILL_SNEAK, 10);
      SET_SKILL(ch, SKILL_HIDE, 5);
      SET_SKILL(ch, SKILL_STEAL, 15);
      SET_SKILL(ch, SKILL_BACKSTAB, 10);
      SET_SKILL(ch, SKILL_PICK_LOCK, 10);
      break;

   case CLASS_WARRIOR:
      break;
   }

   advance_level(ch);

   GET_HIT(ch) = GET_MAX_HIT(ch);
   GET_MANA(ch) = GET_MAX_MANA(ch);
   GET_MOVE(ch) = GET_MAX_MOVE(ch);

   GET_COND(ch, THIRST) = 24;
   GET_COND(ch, FULL) = 24;
   GET_COND(ch, DRUNK) = 0;

   ch->player.time.played = 0;
   ch->player.time.logon = time(0);

}


ACMD(do_advance)
{
   struct char_data *victim;
   char	name[100], level[100];
   int	adv, newlevel;

   void	gain_exp(struct char_data *ch, int gain);

   if (IS_NPC(ch))
      return;

   half_chop(argument, name, buf);
   one_argument(buf, level);

   if (*name) {
      if (!(victim = get_char_vis(ch, name))) {
	 send_to_char("That player is not here.\n\r", ch);
	 return;
      }
   } else {
      send_to_char("Advance who?\n\r", ch);
      return;
   }

   if (ch == victim) {
      send_to_char("Maybe that's not such a great idea.\n\r", ch);
      return;
   }

   if (IS_NPC(victim)) {
      send_to_char("NO!  Not on NPC's.\n\r", ch);
      return;
   }

   if (GET_LEVEL(victim) == 0)
      adv = 1;
   else if (!*level) {
      send_to_char("You must supply a level number.\n\r", ch);
      return;
   } else {
      if (!isdigit(*level)) {
	 send_to_char("Second argument must be a positive integer.\n\r", ch);
	 return;
      }
      if ((newlevel = atoi(level)) <= GET_LEVEL(victim)) {
	 do_start(victim);
	 GET_LEVEL(victim) = newlevel;
      }

      adv = newlevel - GET_LEVEL(victim);
   }

   if (((adv + GET_LEVEL(victim)) > 1) && (GET_LEVEL(ch) < LEVEL_IMPL)) {
      send_to_char("Thou art not godly enough.\n\r", ch);
      return;
   }

   if ((adv + GET_LEVEL(victim)) > LEVEL_IMPL) {
      send_to_char("34 is the highest possible level.\n\r", ch);
      return;
   }

   act("$n makes some strange gestures.\n\rA strange feeling comes upon you,"
       "\n\rLike a giant hand, light comes down from\n\rabove, grabbing your"
       "body, that begins\n\rto pulse with colored lights from inside.\n\rYo"
       "ur head seems to be filled with demons\n\rfrom another plane as your"
       " body dissolves\n\rto the elements of time and space itself.\n\rSudde"
       "nly a silent explosion of light snaps\n\ryou back to reality.  You fee"
       "l slightly\n\rdifferent.", FALSE, ch, 0, victim, TO_VICT);

   send_to_char("Ok.\n\r", ch);

   if (GET_LEVEL(victim) == 0) {
      do_start(victim);
   } else {
      if (GET_LEVEL(victim) < LEVEL_IMPL) {
	 sprintf(buf, "(GC) %s has advanced %s to level %d (from %d)",
	     GET_NAME(ch), GET_NAME(victim), newlevel, GET_LEVEL(victim));
	 log(buf);
	 gain_exp_regardless(victim, (titles[GET_CLASS(victim)-1]
	     [GET_LEVEL(victim)+adv].exp) - GET_EXP(victim));
	 save_char(victim, NOWHERE);
      } else {
	 send_to_char("Some idiot just tried to advance your level.\n\r", victim);
	 send_to_char("IMPOSSIBLE!  IDIOTIC!\n\r", ch);
      }
   }
}



ACMD(do_restore)
{
   struct char_data *victim;
   int	i;

   void	update_pos( struct char_data *victim );

   if (IS_NPC(ch))
      return;

   one_argument(argument, buf);
   if (!*buf)
      send_to_char("Whom do you wish to restore?\n\r", ch);
   else if (!(victim = get_char(buf)))
      send_to_char("No-one by that name in the world.\n\r", ch);
   else {
      GET_MANA(victim) = GET_MAX_MANA(victim);
      GET_HIT(victim) = GET_MAX_HIT(victim);
      GET_MOVE(victim) = GET_MAX_MOVE(victim);

      if ((GET_LEVEL(ch) >= LEVEL_GRGOD) && (GET_LEVEL(victim) >= LEVEL_IMMORT)) {
	 for (i = 0; i < MAX_SKILLS; i++)
	    SET_SKILL(victim, i, 100);

	 if (GET_LEVEL(victim) >= LEVEL_GRGOD) {
	    victim->abilities.str_add = 100;
	    victim->abilities.intel = 25;
	    victim->abilities.wis = 25;
	    victim->abilities.dex = 25;
	    victim->abilities.str = 25;
	    victim->abilities.con = 25;
	 }
	 victim->tmpabilities = victim->abilities;
      }
      update_pos(victim);
      send_to_char("Done.\n\r", ch);
      act("You have been fully healed by $N!", FALSE, victim, 0, ch, TO_CHAR);
   }
}



ACMD(do_invis)
{
   int	level;

   if (IS_NPC(ch)) {
      send_to_char("Yeah.. like a mob knows how to bend light.\n\r", ch);
      return;
   }

   one_argument (argument, arg);
   if (!*arg) {
      if (GET_INVIS_LEV(ch) > 0) {
	 GET_INVIS_LEV(ch) = 0;
	 sprintf(buf, "You are now fully visible.\n\r");
      } else {
	 GET_INVIS_LEV(ch) = GET_LEVEL(ch);
	 sprintf(buf, "Your invisibility level is %d.\n\r", GET_LEVEL(ch));
      }
   } else {
      level = atoi(arg);
      if (level > GET_LEVEL(ch)) {
	 send_to_char("You can't go invisible above your own level.\n\r", ch);
	 return;
      } else if (level < 1) {
	 GET_INVIS_LEV(ch) = 0;
	 sprintf(buf, "You are now fully visible.\n\r");
      } else {
	 GET_INVIS_LEV(ch) = level;
	 sprintf(buf, "Your invisibility level is now %d.\n\r", level);
      }
   }
   send_to_char(buf, ch);
}


ACMD(do_gecho)
{
   int	i;
   struct descriptor_data *pt;

   if (IS_NPC(ch))
      return;

   for (i = 0; *(argument + i) == ' '; i++)
      ;

   if (!*(argument + i))
      send_to_char("That must be a mistake...\n\r", ch);
   else {
      sprintf(buf, "%s\n\r", argument + i);
      for (pt = descriptor_list; pt; pt = pt->next)
	 if (!pt->connected && pt->character && pt->character != ch)
	    act(buf, FALSE, ch, 0, pt->character, TO_VICT);
      if (PRF_FLAGGED(ch, PRF_NOREPEAT))
	 send_to_char("Ok.\n\r", ch);
      else
	 send_to_char(buf, ch);
   }
}


ACMD(do_poofset)
{
   char	**msg;
   int	i;

   switch (subcmd) {
   case SCMD_POOFIN : msg = &(ch->specials.poofIn); break;
   case SCMD_POOFOUT: msg = &(ch->specials.poofOut); break;
   default: return; break;
   }

   for (i = 0; *(argument + i) == ' '; i++)
      ;

   if (*msg)
      free(*msg);

   if (!*(argument + i))
      *msg = NULL;
   else
      *msg = str_dup(argument + i);

   send_to_char("Ok.\n\r", ch);
}





ACMD(do_dc)
{
   struct descriptor_data *d;
   int	num_to_dc;

   if (IS_NPC(ch)) {
      send_to_char("Monsters can't cut connections... leave me alone.\n\r", ch);
      return;
   }

   if (!(num_to_dc = atoi(argument))) {
      send_to_char("Usage: DC <connection number> (type USERS for a list)\n\r", ch);
      return;
   }

   for (d = descriptor_list; d && d->desc_num != num_to_dc; d = d->next)
      ;

   if (!d) {
      send_to_char("No such connection.\n\r", ch);
      return;
   }

   if (d->character && GET_LEVEL(d->character) >= GET_LEVEL(ch)) {
      send_to_char("Umm.. maybe that's not such a good idea...\n\r", ch);
      return;
   }

   close_socket(d);
   sprintf(buf, "Connection #%d closed.\n\r", num_to_dc);
   send_to_char(buf, ch);
   sprintf(buf, "(GC) Connection closed by %s.", GET_NAME(ch));
   log(buf);
}



ACMD(do_wizlock)
{
   int	value;
   char *when;

   one_argument(argument, arg);
   if (*arg) {
      value = atoi(arg);
      if (value < 0 || value > LEVEL_IMPL) {
         send_to_char("Invalid wizlock value.\n\r", ch);
         return;
      }
      restrict = value;
      when = "now";
   } else
      when = "currently";

   switch (restrict) {
   case 0 :
      sprintf(buf, "The game is %s completely open.\n\r", when);
      break;
   case 1 :
      sprintf(buf, "The game is %s closed to new players.\n\r", when);
      break;
   default :
      sprintf(buf, "Only level %d and above may enter the game %s.\n\r",
          restrict, when);
      break;
   }
   send_to_char(buf, ch);
}


ACMD(do_date)
{
   long	ct;
   char	*tmstr;

   if (IS_NPC(ch))
      return;

   ct = time(0);
   tmstr = (char *)asctime(localtime(&ct));
   *(tmstr + strlen(tmstr) - 1) = '\0';
   sprintf(buf, "Current machine time: %s\n\r", tmstr);
   send_to_char(buf, ch);
}



ACMD(do_uptime)
{
   char	*tmstr;
   long	uptime;
   int	d, h, m;

   extern long	boot_time;

   if (IS_NPC(ch))
      return;

   tmstr = (char *)asctime(localtime(&boot_time));
   *(tmstr + strlen(tmstr) - 1) = '\0';

   uptime = time(0) - boot_time;
   d = uptime / 86400;
   h = (uptime / 3600) % 24;
   m = (uptime / 60) % 60;

   sprintf(buf, "Up since %s: %d day%s, %d:%02d\n\r", tmstr, d,
       ((d == 1) ? "" : "s"), h, m);

   send_to_char(buf, ch);
}



ACMD(do_last)
{
   struct char_file_u chdata;
   extern char	*class_abbrevs[];

   if (IS_NPC(ch))
      return;

   if (!*argument) {
      send_to_char("For whom do you wish to search?\n\r", ch);
      return;
   }

   one_argument(argument, arg);
   if (load_char(arg, &chdata) < 0) {
      send_to_char("There is no such player.\n\r", ch);
      return;
   }
   if ((chdata.level > GET_LEVEL(ch)) && (GET_LEVEL(ch) < LEVEL_IMPL)) {
      send_to_char("You are not sufficiently godly for that!\n\r", ch);
      return;
   }

   sprintf(buf, "[%5d] [%2d %s] %-12s : %-18s : %-20s\n\r",
       chdata.specials2.idnum, chdata.level, class_abbrevs[(int)chdata.class],
       chdata.name, chdata.host, ctime(&chdata.last_logon));
   send_to_char(buf, ch);
}


ACMD(do_force)
{
   struct descriptor_data *i;
   struct char_data *vict;
   char	name[100], to_force[MAX_INPUT_LENGTH+2];

   if (IS_NPC(ch)) {
      send_to_char("Umm.... no.\n\r", ch );
      return;
   }

   half_chop(argument, name, to_force);

   sprintf(buf1, "%s has forced you to %s.\n\r", GET_NAME(ch), to_force);
   sprintf(buf2, "Someone has forced you to %s.\n\r", to_force);

   if (!*name || !*to_force)
      send_to_char("Whom do you wish to force do what?\n\r", ch);
   else if (str_cmp("all", name) && str_cmp("room", name)) {
      if (!(vict = get_char_vis(ch, name)) || !CAN_SEE(ch, vict))
	 send_to_char("No-one by that name here...\n\r", ch);
      else {
	 if (GET_LEVEL(ch) > GET_LEVEL(vict)) {
	    send_to_char("Ok.\n\r", ch);
	    if (CAN_SEE(vict, ch) && GET_LEVEL(ch) < LEVEL_IMPL)
	       send_to_char(buf1, vict);
	    else if (GET_LEVEL(ch) < LEVEL_IMPL) {
	       send_to_char(buf2, vict);
	    }
	    if (GET_LEVEL(ch) < LEVEL_IMPL) {
	       sprintf(buf, "(GC) %s forced %s to %s", GET_NAME(ch), name, to_force);
	       log(buf);
	    }
	    command_interpreter(vict, to_force);
	 } else
	    send_to_char("No, no, no!\n\r", ch);
      }
   } else if (str_cmp("room", name)) {
      send_to_char("Okay.\n\r", ch);
      if (GET_LEVEL(ch) < LEVEL_IMPL) {
	 sprintf(buf, "(GC) %s forced %s to %s", GET_NAME(ch), name, to_force);
	 log(buf);
      }
      for (i = descriptor_list; i; i = i->next)
	 if (i->character != ch && !i->connected) {
	    vict = i->character;
	    if (GET_LEVEL(ch) > GET_LEVEL(vict)) {
	       if (CAN_SEE(vict, ch) && GET_LEVEL(ch) < LEVEL_IMPL)
		  send_to_char(buf1, vict);
	       else if (GET_LEVEL(ch) < LEVEL_IMPL)
		  send_to_char(buf2, vict);
	       command_interpreter(vict, to_force);
	    }
	 }
   } else {
      send_to_char("Okay.\n\r", ch);
      if (GET_LEVEL(ch) < LEVEL_IMPL) {
	 sprintf(buf, "(GC) %s forced %s to %s", GET_NAME(ch), name, to_force);
	 log(buf);
      }
      for (i = descriptor_list; i; i = i->next)
	 if (i->character != ch && !i->connected && 
	     i->character->in_room == ch->in_room) {
	    vict = i->character;
	    if (GET_LEVEL(ch) > GET_LEVEL(vict)) {
	       if (CAN_SEE(vict, ch) && GET_LEVEL(ch) < LEVEL_IMPL)
		  send_to_char(buf1, vict);
	       else if (GET_LEVEL(ch) < LEVEL_IMPL)
		  send_to_char(buf2, vict);
	       command_interpreter(vict, to_force);
	    }
	 }
   }
}



ACMD(do_wiznet)
{
   struct descriptor_data *d;
   char	emote = FALSE;
   char	any = FALSE;
   int	level = LEVEL_IMMORT;

   if (IS_NPC(ch)) {
      send_to_char("Yeah - like the Gods are interested in listening to mobs.\n\r", ch);
      return;
   }

   for ( ; *argument == ' '; argument++)
      ;

   if (!*argument) {
      send_to_char("Usage: wiznet <text> | #<level> <text> | *<emotetext> |\n\r "
          "       wiznet @<level> *<emotetext> | wiz @ | wiz - | wiz +\n\r", ch);
      return;
   }

   switch (*argument) {
   case '*':
      emote = TRUE;
   case '#':
      one_argument(argument + 1, buf1);
      if (is_number(buf1)) {
	 half_chop(++argument, buf1, argument);
	 level = MAX(atoi(buf1), LEVEL_IMMORT);
	 if (level > GET_LEVEL(ch)) {
	    send_to_char("You can't wizline above your own level.\n\r", ch);
	    return;
	 }
      } else if (emote)
	 argument++;
      break;
   case '@':
      for (d = descriptor_list; d; d = d->next) {
	 if (!d->connected && GET_LEVEL(d->character) >= LEVEL_IMMORT && 
	     !PRF_FLAGGED(d->character, PRF_NOWIZ) && 
	     (CAN_SEE(ch, d->character) || GET_LEVEL(ch) == LEVEL_IMPL) ) {
	    if (!any) {
	       sprintf(buf1, "Gods online:\n\r");
	       any = TRUE;
	    }
	    sprintf(buf1, "%s  %s", buf1, GET_NAME(d->character));
	    if (PLR_FLAGGED(d->character, PLR_WRITING))
	       sprintf(buf1, "%s (Writing)\n\r", buf1);
	    else if (PLR_FLAGGED(d->character, PLR_MAILING))
	       sprintf(buf1, "%s (Writing mail)\n\r", buf1);
	    else
	       sprintf(buf1, "%s\n\r", buf1);

	 }
      }
      any = FALSE;
      for (d = descriptor_list; d; d = d->next) {
	 if (!d->connected && GET_LEVEL(d->character) >= LEVEL_IMMORT && 
	     PRF_FLAGGED(d->character, PRF_NOWIZ) && 
	     CAN_SEE(ch, d->character) ) {
	    if (!any) {
	       sprintf(buf1, "%sGods offline:\n\r", buf1);
	       any = TRUE;
	    }
	    sprintf(buf1, "%s  %s\n\r", buf1, GET_NAME(d->character));
	 }
      }
      send_to_char(buf1, ch);
      return;
      break;
   case '-':
      if (PRF_FLAGGED(ch, PRF_NOWIZ))
	 send_to_char("You are already offline!\n\r", ch);
      else {
	 send_to_char("You will no longer hear the wizline.\n\r", ch);
	 SET_BIT(PRF_FLAGS(ch), PRF_NOWIZ);
      }
      return;
      break;
   case '+':
      if (!PRF_FLAGGED(ch, PRF_NOWIZ))
	 send_to_char("You are already online!\n\r", ch);
      else {
	 send_to_char("You can now hear the wizline again.\n\r", ch);
	 REMOVE_BIT(PRF_FLAGS(ch), PRF_NOWIZ);
      }
      return;
      break;
   case '\\':
      ++argument;
      break;
   default:
      break;
   }
   if (PRF_FLAGGED(ch, PRF_NOWIZ)) {
      send_to_char("You are offline!\n\r", ch);
      return;
   }

   for ( ; *argument == ' '; argument++)
      ;
   if (!*argument) {
      send_to_char("Don't bother the gods like that!\n\r", ch);
      return;
   }

   if (level > LEVEL_IMMORT) {
      sprintf(buf1, "%s: <%d> %s%s\n\r", GET_NAME(ch), level,
	  emote ? "<--- " : "", argument);
      sprintf(buf2, "Someone: <%d> %s%s\n\r", level, emote ? "<--- " : "",
          argument);
   } else {
      sprintf(buf1, "%s: %s%s\n\r", GET_NAME(ch), emote ? "<--- " : "",
          argument);
      sprintf(buf2, "Someone: %s%s\n\r", emote ? "<--- " : "", argument);
   }

   for (d = descriptor_list; d; d = d->next) {
      if ( (!d->connected) && (GET_LEVEL(d->character) >= level) &&
	  (!PRF_FLAGGED(d->character, PRF_NOWIZ)) &&
          (!PLR_FLAGGED(d->character, PLR_WRITING | PLR_MAILING))
           && (d != ch->desc || !(PRF_FLAGGED(d->character, PRF_NOREPEAT)))) {
	 send_to_char(CCCYN(d->character, C_NRM), d->character);
	 if (CAN_SEE(d->character, ch))
	    send_to_char(buf1, d->character);
	 else
	    send_to_char(buf2, d->character);
	 send_to_char(CCNRM(d->character, C_NRM), d->character);
      }
   }

   if (PRF_FLAGGED(ch, PRF_NOREPEAT))
      send_to_char("Ok.\n\r", ch);
}




ACMD(do_zreset)
{
   int	i, j;

   if IS_NPC(ch) {
      send_to_char("Homie don't play that!\n\r", ch);
      return;
   }
   if (!*argument) {
      send_to_char("You must specify a zone.\n\r", ch);
      return;
   }
   one_argument(argument, arg);
   if (*arg == '*') {
      for (i = 0; i <= top_of_zone_table; i++)
	 reset_zone(i);
      send_to_char("Reset world.\n\r", ch);
      return;
   } else if (*arg == '.')
      i = world[ch->in_room].zone;
   else {
      j = atoi(arg);
      for (i = 0; i <= top_of_zone_table; i++)
	 if (zone_table[i].number == j)
	    break;
   }
   if (i >= 0 && i <= top_of_zone_table) {
      reset_zone(i);
      sprintf(buf, "Reset zone %d: %s.\n\r", i, zone_table[i].name);
      send_to_char(buf, ch);
      sprintf(buf, "(GC) %s reset zone %d (%s)", GET_NAME(ch), i, zone_table[i].name);
      mudlog(buf, NRM, MAX(LEVEL_GRGOD, GET_INVIS_LEV(ch)), TRUE);
   } else
      send_to_char("Invalid zone number.\n\r", ch);
}


/* 
  General fn for wizcommands of the sort: cmd <player>
*/

ACMD(do_wizutil)
{
   struct char_data *vict;
   char	name[40];
   long	result;

   if (IS_NPC(ch)) {
      send_to_char("You're just an unfrozen caveman NPC.\n\r", ch);
      return;
   }
   one_argument(argument, name);
   if (!*name) {
      send_to_char("Yes, but for whom?!?\n\r", ch);
      return;
   }
   if (!(vict = get_char_vis(ch, name))) {
      send_to_char("There is no such player.\n\r", ch);
      return;
   }
   if (IS_NPC(vict)) {
      send_to_char("You can't do that to a mob!\n\r", ch);
      return;
   }
   if (GET_LEVEL(vict) > GET_LEVEL(ch)) {
      send_to_char("Hmmm...you'd better not.\n\r", ch);
      return;
   }

   switch (subcmd) {
   case SCMD_PARDON:
      if (!PLR_FLAGGED(vict, PLR_THIEF | PLR_KILLER)) {
	 send_to_char("Your victim is not flagged.\n\r", ch);
	 return;
      }
      REMOVE_BIT(PLR_FLAGS(vict), PLR_THIEF | PLR_KILLER);
      send_to_char("Pardoned.\n\r", ch);
      send_to_char("You have been pardoned by the Gods!\n\r", vict);
      sprintf(buf, "(GC) %s pardoned by %s", GET_NAME(vict), GET_NAME(ch));
      mudlog(buf, BRF, MAX(LEVEL_GOD, GET_INVIS_LEV(ch)), TRUE);
      break;
   case SCMD_NOTITLE:
      result = PLR_TOG_CHK(vict, PLR_NOTITLE);
      sprintf(buf, "(GC) Notitle %s for %s by %s.", ONOFF(result),
          GET_NAME(vict), GET_NAME(ch));
      mudlog(buf, NRM, MAX(LEVEL_GOD, GET_INVIS_LEV(ch)), TRUE);
      strcat(buf, "\n\r");
      send_to_char(buf, ch);
      break;
   case SCMD_SQUELCH:
      result = PLR_TOG_CHK(vict, PLR_NOSHOUT);
      sprintf(buf, "(GC) Squelch %s for %s by %s.", ONOFF(result),
          GET_NAME(vict), GET_NAME(ch));
      mudlog(buf, BRF, MAX(LEVEL_GOD, GET_INVIS_LEV(ch)), TRUE);
      strcat(buf, "\n\r");
      send_to_char(buf, ch);
      break;
   case SCMD_FREEZE:
      if (ch == vict) {
	 send_to_char("Oh, yeah, THAT'S real smart...\n\r", ch);
	 return;
      }
      if (PLR_FLAGGED(vict, PLR_FROZEN)) {
	 send_to_char("Your victim is already pretty cold.\n\r", ch);
	 return;
      }
      SET_BIT(PLR_FLAGS(vict), PLR_FROZEN);
      vict->specials2.freeze_level = GET_LEVEL(ch);
      send_to_char("A bitter wind suddenly rises and drains every erg of heat from your body!\n\rYou feel frozen!\n\r",
                                         vict);
      send_to_char("Frozen.\n\r", ch);
      act("A sudden cold wind conjured from nowhere freezes $n!", FALSE, vict, 0, 0, TO_ROOM);
      sprintf(buf, "(GC) %s frozen by %s.", GET_NAME(vict), GET_NAME(ch));
      mudlog(buf, BRF, MAX(LEVEL_GOD, GET_INVIS_LEV(ch)), TRUE);
      break;
   case SCMD_THAW:
      if (!PLR_FLAGGED(vict, PLR_FROZEN)) {
	 send_to_char("Sorry, your victim is not morbidly encased in ice at the moment.\n\r", ch);
	 return;
      }
      if (vict->specials2.freeze_level > GET_LEVEL(ch)) {
	 sprintf(buf, "Sorry, a level %d God froze %s... you can't unfreeze %s.\n\r",
	     vict->specials2.freeze_level,
	     GET_NAME(vict),
	     HMHR(vict));
	 send_to_char(buf, ch);
	 return;
      }
      sprintf(buf, "(GC) %s un-frozen by %s.", GET_NAME(vict), GET_NAME(ch));
      mudlog(buf, BRF, MAX(LEVEL_GOD, GET_INVIS_LEV(ch)), TRUE);
      REMOVE_BIT(PLR_FLAGS(vict), PLR_FROZEN);
      send_to_char("A fireball suddenly explodes in front of you, melting the ice!\n\rYou feel thawed.\n\r", vict);
      send_to_char("Thawed.\n\r", ch);
      act("A sudden fireball conjured from nowhere thaws $n!", FALSE, vict, 0, 0, TO_ROOM);
      break;
   case SCMD_UNAFFECT:
      if (vict->affected) {
	 while (vict->affected)
	    affect_remove(vict, vict->affected);
	 send_to_char("All spells removed.\n\r", ch);
	 send_to_char("There is a brief flash of light!\n\r"
	     "You feel slightly different.\n\r", vict);
      } else {
	 send_to_char("Your victim does not have any affections!\n\r", ch);
	 return;
      }
      break;
   case SCMD_REROLL:
      send_to_char("Rerolled...\n\r", ch);
      roll_abilities(vict);
      sprintf(buf, "(GC) %s has rerolled %s.", GET_NAME(ch), GET_NAME(vict));
      log(buf);
      break;
   }
   save_char(vict, NOWHERE);
}


/* single zone printing fn used by "show zone" so it's not repeated in the
   code 3 times ... -je, 4/6/93 */

void	print_zone_to_buf(char *bufptr, int zone)
{
   sprintf(bufptr, "%s%3d %-30.30s Age: %3d; Reset: %3d (%1d); Top: %5d\n\r",
       bufptr, 	zone_table[zone].number, zone_table[zone].name,
       zone_table[zone].age, zone_table[zone].lifespan,
       zone_table[zone].reset_mode, zone_table[zone].top);
}


ACMD(do_show)
{
   struct char_file_u vbuf;
   int	i, j, k, l, con;
   char	self = 0;
   struct char_data *vict;
   struct obj_data *obj;
   char	field[40], value[40], birth[80];
   extern char	*class_abbrevs[];
   extern char	*genders[];
   extern int buf_switches, buf_largecount, buf_overflows;

   struct show_struct {
      char	*cmd;
      char	level;
   } fields[] = {
      { "nothing", 	0 },
      { "zones", 	LEVEL_IMMORT },
      { "player", 	LEVEL_GOD },
      { "rent", 	LEVEL_GOD },
      { "stats", 	LEVEL_IMMORT },
      { "unused", 	LEVEL_IMPL },
      { "death", 	LEVEL_GOD },
      { "godrooms", 	LEVEL_GOD },
      { "\n", 0 }
   };   




   if IS_NPC(ch) {
      send_to_char("Oh no!  None of that stuff!\n\r", ch);
      return;
   }
   if (!*argument) {
      strcpy(buf, "Show options:\n\r");
      for (j = 0, i = 1; fields[i].level; i++)
	 if (fields[i].level <= GET_LEVEL(ch))
	    sprintf(buf, "%s%-15s%s", buf, fields[i].cmd, (!(++j % 5) ? "\n\r" : ""));
      strcat(buf, "\n\r");
      send_to_char(buf, ch);
      return;
   }
   half_chop(argument, field, arg);
   half_chop(arg, value, arg);

   for (l = 0; *(fields[l].cmd) != '\n'; l++)
      if (!strncmp(field, fields[l].cmd, strlen(field)))
	 break;

   if (GET_LEVEL(ch) < fields[l].level) {
      send_to_char("You are not godly enough for that!\n\r", ch);
      return;
   }
   if (!strcmp(value, "."))
      self = 1;
   buf[0] = '\0';
   switch (l) {
   case 1: /* zone */
      /* tightened up by JE 4/6/93 */
      if (self)
	 print_zone_to_buf(buf, world[ch->in_room].zone);
      else if (is_number(value)) {
	 for (j = atoi(value), i = 0; zone_table[i].number != j && i <= top_of_zone_table; i++)
	    ;
	 if (i <= top_of_zone_table)
	    print_zone_to_buf(buf, i);
	 else {
	    send_to_char("That is not a valid zone.\n\r", ch);
	    return;
	 }
      } else
	 for (i = 0; i <= top_of_zone_table; i++)
	    print_zone_to_buf(buf, i);
      send_to_char(buf, ch);
      break;
   case 2: /* player */
      if (load_char(value, &vbuf) < 0) {
	 send_to_char("There is no such player.\n\r", ch);
	 return;
      }
      sprintf(buf, "Player: %-12s (%s) [%2d %s]\n\r", vbuf.name,
          genders[(int)vbuf.sex], vbuf.level, class_abbrevs[(int)vbuf.class]);
      sprintf(buf,
          "%sAu: %-8d  Bal: %-8d  Exp: %-8d  Align: %-5d  Lessons: %-3d\n\r",
          buf, vbuf.points.gold, vbuf.points.bank_gold, vbuf.points.exp,
          vbuf.specials2.alignment, vbuf.specials2.spells_to_learn);
      strcpy(birth, ctime(&vbuf.birth));
      sprintf(buf,
          "%sStarted: %-20.16s  Last: %-20.16s  Played: %3dh %2dm\n\r",
          buf, birth, ctime(&vbuf.last_logon), (int)(vbuf.played / 3600),
	  (int)(vbuf.played / 60 % 60));
      send_to_char(buf, ch);
      break;
   case 3:
      Crash_listrent(ch, value);
      break;
   case 4:
      i = 0;
      j = 0;
      k = 0;
      con = 0;
      for (vict = character_list; vict; vict = vict->next) {
	 if (IS_NPC(vict))
	    j++;
	 else if (CAN_SEE(ch, vict)) {
	    i++;
	    if (vict->desc)
	       con++;
	 }
      }
      for (obj = object_list; obj; obj = obj->next)
	 k++;
      sprintf(buf, "Current stats:\n\r");
      sprintf(buf, "%s  %5d players in game  %5d connected\n\r", buf, i, con);
      sprintf(buf, "%s  %5d registered\n\r", buf, top_of_p_table + 1);
      sprintf(buf, "%s  %5d mobiles          %5d prototypes\n\r",
          buf, j, top_of_mobt + 1);
      sprintf(buf, "%s  %5d objects          %5d prototypes\n\r",
          buf, k, top_of_objt + 1);
      sprintf(buf, "%s  %5d rooms            %5d zones\n\r", 
          buf, top_of_world + 1, top_of_zone_table + 1);
      sprintf(buf, "%s  %5d large bufs\n\r", buf, buf_largecount);
      sprintf(buf, "%s  %5d buf switches     %5d overflows\n\r", buf,
	  buf_switches, buf_overflows);
      send_to_char(buf, ch);
      break;
   case 5:
      break;
   case 6:
      strcpy(buf, "Death Traps\n\r-----------\n\r");
      for (i = 0, j = 0; i < top_of_world; i++)
	 if (IS_SET(world[i].room_flags, DEATH))
	    sprintf(buf, "%s%2d: [%5d] %s\n\r", buf, ++j,
	        world[i].number, world[i].name);
      send_to_char(buf, ch);
      break;
   case 7:
#define GOD_ROOMS_ZONE 2
      strcpy(buf, "Godrooms\n\r--------------------------\n\r");
      for (i = 0, j = 0; i < top_of_world; i++)
	 if (world[i].zone == GOD_ROOMS_ZONE)
	    sprintf(buf, "%s%2d: [%5d] %s\n\r", buf, j++, world[i].number,
	        world[i].name);
      send_to_char(buf, ch);
      break;
   default:
      send_to_char("Sorry, I don't understand that.\n\r", ch);
      break;
   }
}


#define PC   1
#define NPC  2
#define BOTH 3

#define MISC	0
#define BINARY	1
#define NUMBER	2

#define SET_OR_REMOVE(flagset, flags) { \
	if (on) SET_BIT(flagset, flags); \
	else if (off) REMOVE_BIT(flagset, flags); }

#define RANGE(low, high) (value = MAX((low), MIN((high), (value))))

ACMD(do_set)
{
   int	i, l;
   struct char_data *vict;
   struct char_data *cbuf;
   struct char_file_u tmp_store;
   char	field[MAX_INPUT_LENGTH], name[MAX_INPUT_LENGTH],
   val_arg[MAX_INPUT_LENGTH];
   int	on = 0, off = 0, value = 0;
   char	is_file = 0, is_mob = 0, is_player = 0;
   int	player_i;

   struct set_struct {
      char	*cmd;
      char	level;
      char	pcnpc;
      char	type;
   } fields[] = 
    {
      { "brief", 	LEVEL_GOD, 	PC, 	BINARY },
      { "invstart", 	LEVEL_GOD, 	PC, 	BINARY },
      { "title", 	LEVEL_GOD, 	PC, 	MISC },
      { "nosummon", 	LEVEL_GRGOD, 	PC, 	BINARY },
      { "maxhit", 	LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "maxmana", 	LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "maxmove", 	LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "hit", 		LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "mana", 	LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "move", 	LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "align", 	LEVEL_GOD, 	BOTH, 	NUMBER },
      { "str", 		LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "stradd", 	LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "int", 		LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "wis", 		LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "dex", 		LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "con", 		LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "sex", 		LEVEL_GRGOD, 	BOTH, 	MISC },
      { "ac", 		LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "gold", 	LEVEL_GOD, 	BOTH, 	NUMBER },
      { "bank", 	LEVEL_GOD, 	PC, 	NUMBER },
      { "exp", 		LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "hitroll", 	LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "damroll", 	LEVEL_GRGOD, 	BOTH, 	NUMBER },
      { "invis", 	LEVEL_IMPL, 	PC, 	NUMBER },
      { "nohassle", 	LEVEL_GRGOD, 	PC, 	BINARY },
      { "frozen", 	LEVEL_FREEZE, 	PC, 	BINARY },
      { "practices", 	LEVEL_GRGOD, 	PC, 	NUMBER },
      { "lessons", 	LEVEL_GRGOD, 	PC, 	NUMBER },
      { "drunk", 	LEVEL_GRGOD, 	BOTH, 	MISC },
      { "hunger", 	LEVEL_GRGOD, 	BOTH, 	MISC },
      { "thirst", 	LEVEL_GRGOD, 	BOTH, 	MISC },
      { "killer", 	LEVEL_GOD, 	PC, 	BINARY },
      { "thief", 	LEVEL_GOD, 	PC, 	BINARY },
      { "level", 	LEVEL_IMPL, 	BOTH, 	NUMBER },
      { "roomflag", 	LEVEL_GRGOD, 	PC, 	BINARY },
      { "room", 	LEVEL_IMPL, 	BOTH, 	NUMBER },
      { "siteok", 	LEVEL_GRGOD, 	PC, 	BINARY },
      { "deleted", 	LEVEL_IMPL, 	PC, 	BINARY },
      { "class", 	LEVEL_GRGOD, 	BOTH, 	MISC },
      { "nowizlist", 	LEVEL_GOD, 	PC, 	BINARY },
      { "quest", 	LEVEL_GOD, 	PC, 	BINARY },
      { "loadroom", 	LEVEL_GRGOD, 	PC, 	MISC },
      { "color", 	LEVEL_GOD, 	PC, 	BINARY },
      { "idnum", 	LEVEL_IMPL, 	PC, 	NUMBER },
      { "passwd", 	LEVEL_IMPL, 	PC, 	MISC },
      { "nodelete", 	LEVEL_GOD, 	PC, 	BINARY },
      { "\n", 0, BOTH, MISC }
   };   


   half_chop(argument, name, buf);
   if (!strcmp(name, "file")) {
      is_file = 1;
      half_chop(buf, name, buf);
   } else if (!str_cmp(name, "player")) {
      is_player = 1;
      half_chop(buf, name, buf);
   } else if (!str_cmp(name, "mob")) {
      is_mob = 1;
      half_chop(buf, name, buf);
   }

   half_chop(buf, field, buf);
   strcpy(val_arg, buf);

   if (!*name || !*field) {
      send_to_char("Usage: set <victim> <field> <value>\n\r", ch);
      return;
   }
   if (IS_NPC(ch)) {
      send_to_char("None of that!\n\r", ch);
      return;
   }
   if (!is_file) {
      if (is_player) {
	 if (!(vict = get_player_vis(ch, name))) {
	    send_to_char("There is no such player.\n\r", ch);
	    return;
	 }
      } else {
	 if (!(vict = get_char_vis(ch, name))) {
	    send_to_char("There is no such creature.\n\r", ch);
	    return;
	 }
      }
   } else if (is_file) {
      CREATE(cbuf, struct char_data, 1);
      clear_char(cbuf);
      if ((player_i = load_char(name, &tmp_store)) > -1) {
	 store_to_char(&tmp_store, cbuf);
	 if (GET_LEVEL(cbuf) >= GET_LEVEL(ch)) {
	    free_char(cbuf);
	    send_to_char("Sorry, you can't do that.\n\r", ch);
	    return;
	 }
	 vict = cbuf;
      } else {
	 free(cbuf);
	 send_to_char("There is no such player.\n\r", ch);
	 return;
      }
   }

   if (GET_LEVEL(ch) != LEVEL_IMPL) {
      if (!IS_NPC(vict) && GET_LEVEL(ch) <= GET_LEVEL(vict) && vict != ch) {
	 send_to_char("Maybe that's not such a great idea...\n\r", ch);
	 return;
      }
   }

   for (l = 0; *(fields[l].cmd) != '\n'; l++)
      if (!strncmp(field, fields[l].cmd, strlen(field)))
	 break;

   if (GET_LEVEL(ch) < fields[l].level) {
      send_to_char("You are not godly enough for that!\n\r", ch);
      return;
   }
   if (IS_NPC(vict) && (!fields[l].pcnpc && NPC)) {
      send_to_char("You can't do that to a beast!\n\r", ch);
      return;
   } else if (!IS_NPC(vict) && (!fields[l].pcnpc && PC)) {
      send_to_char("That can only be done to a beast!\n\r", ch);
      return;
   }

   if (fields[l].type == BINARY) {
      if (!strcmp(val_arg, "on") || !strcmp(val_arg, "yes"))
	 on = 1;
      else if (!strcmp(val_arg, "off") || !strcmp(val_arg, "no"))
	 off = 1;
      if (!(on || off)) {
	 send_to_char("Value must be on or off.\n\r", ch);
	 return;
      }
   } else if (fields[l].type == NUMBER) {
      value = atoi(val_arg);
   }

   strcpy(buf, "Okay.");
   switch (l) {
   case 0:
      SET_OR_REMOVE(PRF_FLAGS(vict), PRF_BRIEF);
      break;
   case 1:
      SET_OR_REMOVE(PLR_FLAGS(vict), PLR_INVSTART);
      break;
   case 2:
      if (GET_TITLE(vict))
	 RECREATE(GET_TITLE(vict), char, strlen(val_arg) + 1);
      else
	 CREATE(GET_TITLE(vict), char, strlen(val_arg) + 1);
      strcpy(GET_TITLE(vict), val_arg);
      sprintf(buf, "%s's title is now: %s", GET_NAME(vict), GET_TITLE(vict));
      break;
   case 3:
      SET_OR_REMOVE(PRF_FLAGS(vict), PRF_SUMMONABLE);
      on = !on; /* so output will be correct */
      break;
   case 4:
      vict->points.max_hit = RANGE(1, 5000);
      affect_total(vict);
      break;
   case 5:
      vict->points.max_mana = RANGE(1, 5000);
      affect_total(vict);
      break;
   case 6:
      vict->points.max_move = RANGE(1, 5000);
      affect_total(vict);
      break;
   case 7:
      vict->points.hit = RANGE(-9, vict->points.max_hit);
      affect_total(vict);
      break;
   case 8:
      vict->points.mana = RANGE(0, vict->points.max_mana);
      affect_total(vict);
      break;
   case 9:
      vict->points.move = RANGE(0, vict->points.max_move);
      affect_total(vict);
      break;
   case 10:
      GET_ALIGNMENT(vict) = RANGE(-1000, 1000);
      affect_total(vict);
      break;
   case 11:
      if (IS_NPC(vict) || GET_LEVEL(vict) >= LEVEL_GRGOD)
	 RANGE(3, 25);
      else
	 RANGE(3, 18);
      vict->abilities.str = value;
      vict->abilities.str_add = 0;
      affect_total(vict);
      break;
   case 12:
      vict->abilities.str_add = RANGE(0, 100);
      if (value > 0)
	 vict->abilities.str = 18;
      affect_total(vict);
      break;
   case 13:
      if (IS_NPC(vict) || GET_LEVEL(vict) >= LEVEL_GRGOD)
	 RANGE(3, 25);
      else
	 RANGE(3, 18);
      vict->abilities.intel = value;
      affect_total(vict);
      break;
   case 14:
      if (IS_NPC(vict) || GET_LEVEL(vict) >= LEVEL_GRGOD)
	 RANGE(3, 25);
      else
	 RANGE(3, 18);
      vict->abilities.wis = value;
      affect_total(vict);
      break;
   case 15:
      if (IS_NPC(vict) || GET_LEVEL(vict) >= LEVEL_GRGOD)
	 RANGE(3, 25);
      else
	 RANGE(3, 18);
      vict->abilities.dex = value;
      affect_total(vict);
      break;
   case 16:
      if (IS_NPC(vict) || GET_LEVEL(vict) >= LEVEL_GRGOD)
	 RANGE(3, 25);
      else
	 RANGE(3, 18);
      vict->abilities.con = value;
      affect_total(vict);
      break;
   case 17:
      if (!str_cmp(val_arg, "male"))
	 vict->player.sex = SEX_MALE;
      else if (!str_cmp(val_arg, "female"))
	 vict->player.sex = SEX_FEMALE;
      else if (!str_cmp(val_arg, "neutral"))
	 vict->player.sex = SEX_NEUTRAL;
      else {
	 send_to_char("Must be 'male', 'female', or 'neutral'.\n\r", ch);
	 return;
      }
      break;
   case 18:
      vict->points.armor = RANGE(-100, 100);
      affect_total(vict);
      break;
   case 19:
      GET_GOLD(vict) = RANGE(0, 100000000);
      break;
   case 20:
      GET_BANK_GOLD(vict) = RANGE(0, 100000000);
      break;
   case 21:
      vict->points.exp = RANGE(0, 50000000);
      break;
   case 22:
      vict->points.hitroll = RANGE(-20, 20);
      affect_total(vict);
      break;
   case 23:
      vict->points.damroll = RANGE(-20, 20);
      affect_total(vict);
      break;
   case 24:
      if (GET_LEVEL(ch) < LEVEL_IMPL && ch != vict) {
	 send_to_char("You aren't godly enough for that!\n\r", ch);
	 return;
      }
      GET_INVIS_LEV(vict) = RANGE(0, GET_LEVEL(vict));
      break;
   case 25:
      if (GET_LEVEL(ch) < LEVEL_IMPL && ch != vict) {
	 send_to_char("You aren't godly enough for that!\n\r", ch);
	 return;
      }
      SET_OR_REMOVE(PRF_FLAGS(vict), PRF_NOHASSLE);
      break;
   case 26:
      if (ch == vict) {
	 send_to_char("Better not -- could be a long winter!\n\r", ch);
	 return;
      }
      SET_OR_REMOVE(PLR_FLAGS(vict), PLR_FROZEN);
      break;
   case 27:
   case 28:
      SPELLS_TO_LEARN(vict) = RANGE(0, 100);
      break;
   case 29:
   case 30:
   case 31:
      if (!str_cmp(val_arg, "off")) {
	 GET_COND(vict, (l - 29)) = (char) -1;
	 sprintf(buf, "%s's %s now off.", GET_NAME(vict),
	     fields[l].cmd);
      } else if (is_number(val_arg)) {
	 value = atoi(val_arg);
	 RANGE(0, 24);
	 GET_COND(vict, (l - 29)) = (char) value;
	 sprintf(buf, "%s's %s set to %d.", GET_NAME(vict),
	     fields[l].cmd, value);
      } else {
	 send_to_char("Must be 'off' or a value from 0 to 24.\n\r", ch);
	 return;
      }
      break;
   case 32:
      SET_OR_REMOVE(PLR_FLAGS(vict), PLR_KILLER);
      break;
   case 33:
      SET_OR_REMOVE(PLR_FLAGS(vict), PLR_THIEF);
      break;
   case 34:
      if (GET_LEVEL(ch) < LEVEL_IMPL && value > GET_LEVEL(ch)) {
	 send_to_char("You cannot raise someone above your own level!\n\r", ch);
	 return;
      }
      vict->player.level = (byte) value;
      break;
   case 35:
      SET_OR_REMOVE(PRF_FLAGS(vict), PRF_ROOMFLAGS);
      break;
   case 36:
      if ((i = real_room(value)) < 0) {
	 send_to_char("No room exists with that number.\n\r", ch);
	 return;
      }
      char_from_room(vict);
      char_to_room(vict, i);
      break;
   case 37:
      SET_OR_REMOVE(PLR_FLAGS(vict), PLR_SITEOK);
      break;
   case 38:
      SET_OR_REMOVE(PLR_FLAGS(vict), PLR_DELETED);
      break;
   case 39:
      /* class */
      break;
   case 40:
      SET_OR_REMOVE(PLR_FLAGS(vict), PLR_NOWIZLIST);
      break;
   case 41:
      SET_OR_REMOVE(PRF_FLAGS(vict), PRF_QUEST);
      break;
   case 42:
      if (!str_cmp(val_arg, "on"))
	 SET_BIT(PLR_FLAGS(vict), PLR_LOADROOM);
      else if (!str_cmp(val_arg, "off"))
	 REMOVE_BIT(PLR_FLAGS(vict), PLR_LOADROOM);
      else {
	 if (real_room(i = atoi(val_arg)) > -1) {
	    GET_LOADROOM(vict) = i;
	    sprintf(buf, "%s will enter at %d.", GET_NAME(vict),
	        GET_LOADROOM(vict));
	 } else
	    sprintf(buf, "That room does not exist!");
      }
      break;
   case 43:
      SET_OR_REMOVE(PRF_FLAGS(vict), (PRF_COLOR_1 | PRF_COLOR_2));
      break;
   case 44:
      if (GET_IDNUM(ch) != 1 || !IS_NPC(vict))
	 return;
      vict->specials2.idnum = value;
      break;
   case 45:
      if (!is_file)
	 return;
      if (GET_IDNUM(ch) > 1)  {
	 send_to_char("Please don't use this command, yet.\n\r", ch);
	 return;
      }
      if (GET_LEVEL(vict) >= LEVEL_GRGOD) {
	 send_to_char("You cannot change that.\n\r", ch);
	 return;
      }
      strncpy(tmp_store.pwd, CRYPT(val_arg, tmp_store.name), MAX_PWD_LENGTH);
      tmp_store.pwd[MAX_PWD_LENGTH] = '\0';
      sprintf(buf, "Password changed to '%s'.", val_arg);
      break;
   case 46:
      SET_OR_REMOVE(PLR_FLAGS(vict), PLR_NODELETE);
      break;
   default:
      sprintf(buf, "Can't set that!");
      break;
   }

   if (fields[l].type == BINARY) {
      sprintf(buf, "%s %s for %s.\n\r", fields[l].cmd, ONOFF(on),
          GET_NAME(vict));
      CAP(buf);
   } else if (fields[l].type == NUMBER) {
      sprintf(buf, "%s's %s set to %d.\n\r", GET_NAME(vict),
          fields[l].cmd, value);
   } else
      strcat(buf, "\n\r");
   send_to_char(buf, ch);

   if (!is_file && !IS_NPC(vict))
      save_char(vict, NOWHERE);

   if (is_file) {
      char_to_store(vict, &tmp_store);
      fseek(player_fl, (player_i) * sizeof(struct char_file_u), SEEK_SET);
      fwrite(&tmp_store, sizeof(struct char_file_u), 1, player_fl);
      free_char(cbuf);
      send_to_char("Saved in file.\n\r", ch);
   }
}


