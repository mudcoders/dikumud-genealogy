/* ************************************************************************
*   File: act.obj1.c                                    Part of CircleMUD *
*  Usage: object handling routines -- get/drop and container handling     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"

/* extern variables */
extern struct str_app_type str_app[];
extern struct room_data *world;


void perform_put(struct char_data *ch, struct obj_data *obj,
		 struct obj_data *container)
{
   if (GET_OBJ_WEIGHT(container) + GET_OBJ_WEIGHT(obj) > container->obj_flags.value[0])
      act("$p won't fit in $P.", FALSE, ch, obj, container, TO_CHAR);
   else {
      obj_from_char(obj);
      obj_to_obj(obj, container);
      act("You put $p in $P.", FALSE, ch, obj, container, TO_CHAR);
      act("$n puts $p in $P.", TRUE, ch, obj, container, TO_ROOM);
   }
}

   
/* The following put modes are supported by the code below:

	1) put <object> <container>
	2) put all.<object> <container>
	3) put all <container>

	<container> must be in inventory or on ground.
	all objects to be put into container must be in inventory.
*/

ACMD(do_put)
{
   char	arg1[MAX_INPUT_LENGTH];
   char	arg2[MAX_INPUT_LENGTH];
   struct obj_data *obj;
   struct obj_data *next_obj;
   struct obj_data *container;
   struct char_data *tmp_char;
   int	obj_dotmode, cont_dotmode;

   argument_interpreter(argument, arg1, arg2);
   obj_dotmode = find_all_dots(arg1);
   cont_dotmode = find_all_dots(arg2);

   if (cont_dotmode != FIND_INDIV)
      send_to_char("You can only put things into one container at a time.\n\r", ch);
   else if (!*arg1)
      send_to_char("Put what in what?\n\r", ch);
   else if (!*arg2) {
      sprintf(buf, "What do you want to put %s in?\n\r",
	      ((obj_dotmode != FIND_INDIV) ? "them" : "it"));
      send_to_char(buf, ch);
   } else {
      generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &tmp_char, &container);
      if (!container) {
	 sprintf(buf, "You don't see a %s here.\n\r", arg2);
	 send_to_char(buf, ch);
      } else if (GET_ITEM_TYPE(container) != ITEM_CONTAINER) {
	 act("$p is not a container.", FALSE, ch, container, 0, TO_CHAR);
      } else if (IS_SET(container->obj_flags.value[1], CONT_CLOSED)) {
	 send_to_char("You'd better open it first!\n\r", ch);
      } else {
	 if (obj_dotmode == FIND_ALL) {	    /* "put all <container>" case */
	    /* check and make sure the guy has something first */
	    if (container == ch->carrying && !ch->carrying->next_content)
	       send_to_char("You don't seem to have anything to put in it.\n\r", ch);
	    else for (obj = ch->carrying; obj; obj = next_obj) {
	       next_obj = obj->next_content;
	       if (obj != container)
		  perform_put(ch, obj, container);
	    }
	 } else if (obj_dotmode == FIND_ALLDOT) {  /* "put all.x <cont>" case */
	    if (!(obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
	       sprintf(buf, "You don't seem to have any %ss.\n\r", arg1);
	       send_to_char(buf, ch);
	    } else while (obj) {
	       next_obj = get_obj_in_list_vis(ch, arg1, obj->next_content);
	       if (obj != container)
		  perform_put(ch, obj, container);
	       obj = next_obj;
	    }
	 } else {		    /* "put <thing> <container>" case */
	    if (!(obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
	       sprintf(buf, "You aren't carrying %s %s.\n\r", AN(arg1), arg1);
	       send_to_char(buf, ch);
	    } else if (obj == container)
	       send_to_char("You attempt to fold it into itself, but fail.\n\r", ch);
	    else
	       perform_put(ch, obj, container);
	 }
      }
   }
}



int	can_take_obj(struct char_data *ch, struct obj_data *obj)
{
   if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch)) {
      sprintf(buf, "%s: You can't carry that many items.\n\r", OBJS(obj, ch));
      send_to_char(buf, ch);
      return 0;
   } else if ((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) > CAN_CARRY_W(ch)) {
      sprintf(buf, "%s: You can't carry that much weight.\n\r", OBJS(obj, ch));
      send_to_char(buf, ch);
      return 0;
   } else if (!(CAN_WEAR(obj, ITEM_TAKE))) {
      sprintf(buf, "%s: You can't take that!\n\r", OBJS(obj, ch));
      send_to_char(buf, ch);
      return 0;
   }

   return 1;
}


void	get_check_money(struct char_data *ch, struct obj_data *obj)
{
   if ((GET_ITEM_TYPE(obj) == ITEM_MONEY) && (obj->obj_flags.value[0] > 0)) {
      obj_from_char(obj);
      if (obj->obj_flags.value[0] > 1) {
	 sprintf(buf, "There were %d coins.\n\r", obj->obj_flags.value[0]);
	 send_to_char(buf, ch);
      }
      GET_GOLD(ch) += obj->obj_flags.value[0];
      extract_obj(obj);
   }
}


void	perform_get_from_container(struct char_data *ch, struct obj_data *obj,
				   struct obj_data *cont, int mode)
{
   if (mode == FIND_OBJ_INV || can_take_obj(ch, obj)) {
      if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
	 sprintf(buf, "%s: You can't hold any more items.\n\r", OBJS(obj, ch));
      else {
         obj_from_obj(obj);
         obj_to_char(obj, ch);
	 act("You get $p from $P.", FALSE, ch, obj, cont, TO_CHAR);
	 act("$n gets $p from $P.", TRUE, ch, obj, cont, TO_ROOM);
	 get_check_money(ch, obj);
      }
   }
}


void	get_from_container(struct char_data *ch, struct obj_data *cont,
			   char *arg, int mode)
{
   struct obj_data *obj, *next_obj;
   int obj_dotmode, found = 0;

   obj_dotmode = find_all_dots(arg);

   if (IS_SET(cont->obj_flags.value[1], CONT_CLOSED))
      act("The $p is closed.", FALSE, ch, cont, 0, TO_CHAR);
   else if (obj_dotmode == FIND_ALL) {
      for (obj = cont->contains; obj; obj = next_obj) {
	 next_obj = obj->next_content;
	 if (CAN_SEE_OBJ(ch, obj)) {
	    found = 1;
	    perform_get_from_container(ch, obj, cont, mode);
	 }
      }
      if (!found)
	 act("$p seems to be empty.", FALSE, ch, cont, 0, TO_CHAR);
   } else if (obj_dotmode == FIND_ALLDOT) {
      if (!*arg) {
	 send_to_char("Get all of what?\n\r", ch);
	 return;
      }
      obj = get_obj_in_list_vis(ch, arg, cont->contains);
      while (obj) {
         next_obj = get_obj_in_list_vis(ch, arg, obj->next_content);
	 if (CAN_SEE_OBJ(ch, obj)) {
	    found = 1;
	    perform_get_from_container(ch, obj, cont, mode);
	 }
	 obj = next_obj;
      }
      if (!found) {
	 sprintf(buf, "You can't find any %ss in $p.", arg);
	 act(buf, FALSE, ch, cont, 0, TO_CHAR);
      }
   } else {
      if (!(obj = get_obj_in_list_vis(ch, arg, cont->contains))) {
	 sprintf(buf, "There doesn't seem to be %s %s in $p.", AN(arg), arg);
	 act(buf, FALSE, ch, cont, 0, TO_CHAR);
      } else
	 perform_get_from_container(ch, obj, cont, mode);
   }
}


int	perform_get_from_room(struct char_data *ch, struct obj_data *obj)
{
   if (can_take_obj(ch, obj)) {
      obj_from_room(obj);
      obj_to_char(obj, ch);
      act("You get $p.", FALSE, ch, obj, 0, TO_CHAR);
      act("$n gets $p.", TRUE, ch, obj, 0, TO_ROOM);
      get_check_money(ch, obj);
      return 1;
   }

   return 0;
}


void get_from_room(struct char_data *ch, char *arg)
{
   struct obj_data *obj, *next_obj;
   int	dotmode, found = 0;

   dotmode = find_all_dots(arg);

   if (dotmode == FIND_ALL) {
      for (obj = world[ch->in_room].contents; obj; obj = next_obj) {
	 next_obj = obj->next_content;
	 if (CAN_SEE_OBJ(ch, obj)) {
	    found = 1;
	    perform_get_from_room(ch, obj);
	 }
      }
      if (!found)
	 send_to_char("There doesn't seem to be anything here.\n\r", ch);
   } else if (dotmode == FIND_ALLDOT) {
      if (!*arg) {
	 send_to_char("Get all of what?\n\r", ch);
	 return;
      }
      if (!(obj = get_obj_in_list_vis(ch, arg, world[ch->in_room].contents))) {
	 sprintf(buf, "You don't see any %ss here.\n\r", arg);
	 send_to_char(buf, ch);
      } else while (obj) {
	 next_obj = get_obj_in_list_vis(ch, arg, obj->next_content);
	 perform_get_from_room(ch, obj);
	 obj = next_obj;
      }
   } else {
      if (!(obj = get_obj_in_list_vis(ch, arg, world[ch->in_room].contents))) {
	 sprintf(buf, "You don't see %s %s here.\n\r", AN(arg), arg);
	 send_to_char(buf, ch);
      } else
	 perform_get_from_room(ch, obj);
   }
}



ACMD(do_get)
{
   char	arg1[MAX_INPUT_LENGTH];
   char	arg2[MAX_INPUT_LENGTH];

   int	cont_dotmode, found = 0, mode;
   struct obj_data *cont, *next_cont;
   struct char_data *tmp_char;

   if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch)) 
      send_to_char("Your arms are already full!\n\r", ch);
   else {
      argument_interpreter(argument, arg1, arg2);
      if (!*arg1)
         send_to_char("Get what?\n\r", ch);
      else {
         if (!*arg2)
            get_from_room(ch, arg1);
         else {
	    cont_dotmode = find_all_dots(arg2);
	    if (cont_dotmode == FIND_ALL) { /* use all in inv. and on ground */
	       for(cont = ch->carrying; cont; cont = cont->next_content)
		  if (GET_ITEM_TYPE(cont) == ITEM_CONTAINER) {
		     found = 1;
		     get_from_container(ch, cont, arg1, FIND_OBJ_INV);
		  }
	       for(cont = world[ch->in_room].contents; cont; cont = cont->next_content)
		  if (CAN_SEE_OBJ(ch, cont) && GET_ITEM_TYPE(cont) == ITEM_CONTAINER) {
		     found = 1;
		     get_from_container(ch, cont, arg1, FIND_OBJ_ROOM);
		  }
	       if (!found)
		  send_to_char("You can't seem to find any containers.\n\r", ch);
	    } else if (cont_dotmode == FIND_ALLDOT) {
	       if (!*arg2) {
		  send_to_char("Get from all of what?\n\r", ch);
		  return;
	       }
	       cont = get_obj_in_list_vis(ch, arg2, ch->carrying);
	       while (cont) {
		  found = 1;
		  next_cont = get_obj_in_list_vis(ch, arg2, cont->next_content);
		  if (GET_ITEM_TYPE(cont) != ITEM_CONTAINER)
		     act("$p is not a container.", FALSE, ch, cont, 0, TO_CHAR);
		  else
		     get_from_container(ch, cont, arg1, FIND_OBJ_INV);
		  cont = next_cont;
	       }
	       cont = get_obj_in_list_vis(ch, arg2, world[ch->in_room].contents);
	       while (cont) {
		  found = 1;
		  next_cont = get_obj_in_list_vis(ch, arg2, cont->next_content);
		  if (GET_ITEM_TYPE(cont) != ITEM_CONTAINER)
		     act("$p is not a container.", FALSE, ch, cont, 0, TO_CHAR);
		  else
		      get_from_container(ch, cont, arg1, FIND_OBJ_ROOM);
		  cont = next_cont;
	       }
	       if (!found) {
		  sprintf(buf, "You can't seem to find any %ss here.\n\r", arg2);
		  send_to_char(buf, ch);
	       }
	    } else { /* get <items> <container> (no all or all.x) */
	       mode = generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM, ch, &tmp_char, &cont);
	       if (!cont) {
		  sprintf(buf, "You don't have %s %s.\n\r", AN(arg2), arg2);
		  send_to_char(buf, ch);
	       } else if (GET_ITEM_TYPE(cont) != ITEM_CONTAINER)
	          act("$p is not a container.", FALSE, ch, cont, 0, TO_CHAR);
	       else
		  get_from_container(ch, cont, arg1, mode);
	    }
	 }
      }
   }
}


void	perform_drop_gold(struct char_data *ch, int amount,
			  byte mode, sh_int RDR)
{
   struct obj_data *obj;

   if (amount <= 0)
      send_to_char("Heh heh heh.. we are jolly funny today, eh?\n\r", ch);
   else if (GET_GOLD(ch) < amount)
      send_to_char("You don't have that many coins!\n\r", ch);
   else {
      if (mode != SCMD_JUNK) {
	 obj = create_money(amount);
	 if (mode == SCMD_DONATE) {
	    send_to_char("You throw some gold into the air where it disappears in a puff of smoke!\n\r", ch);
	    act("$n throws some gold into the air where it disappears in a puff of smoke!", FALSE, ch, 0, 0, TO_ROOM);
	    send_to_room("Some gold suddenly appears in mid-air, then drops to the ground!\n\r", RDR);
	    obj_to_room(obj, RDR);
	 } else {
	    send_to_char("You drop some gold.\n\r", ch);
	    act("$n drops some gold.", FALSE, ch, 0, 0, TO_ROOM);
	    obj_to_room(obj, ch->in_room);
	 }
      } else {
	 act("$n drops some gold which disappears in a puff of smoke!", FALSE, ch, 0, 0, TO_ROOM);
	 send_to_char("You drop some gold which disappears in a puff of smoke!\n\r", ch);
      }
      GET_GOLD(ch) -= amount;
   }
}


#define VANISH(mode) ((mode == SCMD_DONATE || mode == SCMD_JUNK) ? \
		      "  It vanishes in a puff of smoke!" : "")

int	perform_drop(struct char_data *ch, struct obj_data *obj,
		     byte mode, char *sname, sh_int RDR)
{
   int	value;

   if (IS_SET(obj->obj_flags.extra_flags, ITEM_NODROP)) {
      sprintf(buf, "You can't %s %s, it must be CURSED!\n\r", sname, OBJS(obj, ch));
      send_to_char(buf, ch);
      return 0;
   }

   if ((mode == SCMD_DONATE) && IS_SET(obj->obj_flags.extra_flags, ITEM_NODONATE)) {
      sprintf(buf, "You can't donate %s.\n\r", OBJS(obj, ch));
      send_to_char(buf, ch);
      return 0;
   }

   sprintf(buf, "You %s %s.%s\n\r", sname, OBJS(obj, ch), VANISH(mode));
   send_to_char(buf, ch);
   sprintf(buf, "$n %ss $p.%s", sname, VANISH(mode));
   act(buf, TRUE, ch, obj, 0, TO_ROOM);

   obj_from_char(obj);

   switch(mode) {
   case SCMD_DROP:
      obj_to_room(obj, ch->in_room);
      return 0;
      break;
   case SCMD_DONATE:
      obj_to_room(obj, RDR);
      send_to_room("Something suddenly appears in a puff a smoke!\n\r", RDR);
      return 0;
      break;
   case SCMD_JUNK:
      value = MAX(1, MIN(200, obj->obj_flags.cost >> 4));
      extract_obj(obj);
      return value;
      break;
   default:
      log("SYSERR: Incorrect argument passed to perform_drop");
      break;
   }

   return 0;
}



ACMD(do_drop)
{
   extern sh_int donation_room_1, donation_room_2, donation_room_3; 
   struct obj_data *obj, *next_obj;
   sh_int RDR = 0;
   byte	mode = SCMD_DROP;
   int	dotmode, amount = 0;
   char	*sname;

   switch (subcmd) {
   case SCMD_JUNK:
      sname = "junk";
      mode = SCMD_JUNK;
      break;
   case SCMD_DONATE:
      sname = "donate";
      mode = SCMD_DONATE;
      switch (number(0, 2)) {
      case 0:
	 mode = SCMD_JUNK;
	 break;
      case 1:
      case 2: RDR = real_room(donation_room_1); break;
/*    case 3: RDR = real_room(donation_room_2); break;
      case 4: RDR = real_room(donation_room_3); break;
*/
      }
      if (RDR == NOWHERE) {
	 send_to_char("Sorry, you can't donate anything right now.\n\r", ch);
	 return;
      }
      break;
   default:
      sname = "drop";
      break;
   }

   argument = one_argument(argument, arg);

   if (!*arg) {
      sprintf(buf, "What do you want to %s?\n\r", sname);
      send_to_char(buf, ch);
      return;
   } else if (is_number(arg)) {
      amount = atoi(arg);
      argument = one_argument(argument, arg);
      if (!str_cmp("coins", arg) || !str_cmp("coin", arg))
         perform_drop_gold(ch, amount, mode, RDR);
      else {
	 /* code to drop multiple items.  anyone want to write it? -je */
	 send_to_char("Sorry, you can't do that (yet)...\n\r", ch);
	 return;
      }
   } else {
      dotmode = find_all_dots(arg);

      /* Can't junk or donate all */
      if ((dotmode == FIND_ALL) && (subcmd == SCMD_JUNK || subcmd == SCMD_DONATE)) {
         if (subcmd == SCMD_JUNK) 
	    send_to_char("Go to the dump if you want to junk EVERYTHING!\n\r", ch);
         else 
	    send_to_char("Go do the donation room if you want to donate EVERYTHING!\n\r", ch);
         return;
      }

      if (dotmode == FIND_ALL) {
         if (!ch->carrying)
	    send_to_char("You don't seem to be carrying anything.\n\r", ch);
	 else
            for (obj = ch->carrying; obj; obj = next_obj) {
               next_obj = obj->next_content;
	       amount += perform_drop(ch, obj, mode, sname, RDR);
	    }
      } else if (dotmode == FIND_ALLDOT) {
	 if (!*arg) {
	    sprintf(buf, "What do you want to %s all of?\n\r", sname);
	    send_to_char(buf, ch);
	    return;
	 }
         if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
            sprintf(buf, "You don't seem to have any %ss.\n\r", arg);
	    send_to_char(buf, ch);
	 }
         while (obj) {
	    next_obj = get_obj_in_list_vis(ch, arg, obj->next_content);
	    amount += perform_drop(ch, obj, mode, sname, RDR);
	    obj = next_obj;
	 }
      } else {
         if (!(obj = get_obj_in_list_vis(ch, arg, ch->carrying))) {
	    sprintf(buf, "You don't seem to have %s %s.\n\r", AN(arg), arg);
	    send_to_char(buf, ch);
	 } else
	    amount += perform_drop(ch, obj, mode, sname, RDR);
      }
   }

   if (amount && (subcmd == SCMD_JUNK)) {
      send_to_char("You have been rewarded by the gods!\n\r", ch);
      act("$n has been rewarded by the gods!", TRUE, ch, 0, 0, TO_ROOM);
      GET_GOLD(ch) += amount;
   }
}


void	perform_give(struct char_data *ch, struct char_data *vict,
		     struct obj_data *obj)
{
   if (IS_SET(obj->obj_flags.extra_flags, ITEM_NODROP)) {
      act("You can't let go of $p!!  Yeech!", FALSE, ch, obj, 0, TO_CHAR);
      return;
   }

   if (IS_CARRYING_N(vict) >= CAN_CARRY_N(vict)) {
      act("$N seems to have $S hands full.", FALSE, ch, 0, vict, TO_CHAR);
      return;
   }

   if (GET_OBJ_WEIGHT(obj) + IS_CARRYING_W(vict) > CAN_CARRY_W(vict)) {
      act("$E can't carry that much weight.", FALSE, ch, 0, vict, TO_CHAR);
      return;
   }

   obj_from_char(obj);
   obj_to_char(obj, vict);
   act("You give $p to $N.", FALSE, ch, obj, vict, TO_CHAR);
   act("$n gives you $p.", FALSE, ch, obj, vict, TO_VICT);
   act("$n gives $p to $N.", TRUE, ch, obj, vict, TO_NOTVICT);
}

/* utility function for give */
struct char_data *give_find_vict(struct char_data *ch, char *arg1)
{
   struct char_data *vict;
   char arg2[MAX_INPUT_LENGTH];

   strcpy(buf, arg1);
   argument_interpreter(buf, arg1, arg2);
   if (!*arg1) {
      send_to_char("Give what to who?\n\r", ch);
      return 0;
   } else if (!*arg2) {
      send_to_char("To who?\n\r", ch);
      return 0;
   } else if (!(vict = get_char_room_vis(ch, arg2))) {
      send_to_char("No-one by that name here.\n\r", ch);
      return 0;
   } else if (vict == ch) {
      send_to_char("What's the point of that?\n\r", ch);
      return 0;
   } else
      return vict;
}


void	perform_give_gold(struct char_data *ch, struct char_data *vict,
			  int amount)
{
   if (amount <= 0) {
      send_to_char("Heh heh heh ... we are jolly funny today, eh?\n\r", ch);
      return;
   }

   if ((GET_GOLD(ch) < amount) && (IS_NPC(ch) || (GET_LEVEL(ch) < LEVEL_GOD))) {
      send_to_char("You haven't got that many coins!\n\r", ch);
      return;
   }

   send_to_char("Ok.\n\r", ch);
   sprintf(buf, "%s gives you %d gold coins.\n\r", PERS(ch, vict), amount);
   send_to_char(buf, vict);
   act("$n gives some gold to $N.", TRUE, ch, 0, vict, TO_NOTVICT);
   if (IS_NPC(ch) || (GET_LEVEL(ch) < LEVEL_GOD))
      GET_GOLD(ch) -= amount;
   GET_GOLD(vict) += amount;
}


ACMD(do_give)
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   int	amount, dotmode;
   struct char_data *vict;
   struct obj_data *obj, *next_obj;

   half_chop(argument, arg1, arg2);

   if (!*arg1)
      send_to_char("Give what to who?\n\r", ch);
   else if (is_number(arg1)) {
      amount = atoi(arg1);
      if (!(vict = give_find_vict(ch, arg2)))
         return;
      if (!str_cmp("coins", arg2) || !str_cmp("coin", arg2))
         perform_give_gold(ch, vict, amount);
      else {
	 /* code to give multiple items.  anyone want to write it? -je */
	 send_to_char("Sorry, you can't do that (yet)...\n\r", ch);
	 return;
      }
   } else {
      if (!(vict = give_find_vict(ch, argument)))
	 return;
      dotmode = find_all_dots(argument);
      if (dotmode == FIND_ALL) {
	 if (!ch->carrying)
	    send_to_char("You don't seem to be holding anything.\n\r", ch);
	 else for (obj = ch->carrying; obj; obj = next_obj) {
	    next_obj = obj->next_content;
	    perform_give(ch, vict, obj);
	 }
      } else if (dotmode == FIND_ALLDOT) {
	 if (!*argument) {
	    send_to_char("All of what?\n\r", ch);
	    return;
	 }
	 if (!(obj = get_obj_in_list_vis(ch, argument, ch->carrying))) {
	    sprintf(buf, "You don't seem to have any %ss.\n\r", argument);
	    send_to_char(buf, ch);
	 } else while (obj) {
	    next_obj = get_obj_in_list_vis(ch, argument, obj->next_content);
	    perform_give(ch, vict, obj);
	    obj = next_obj;
	 }
      } else {
	 if (!(obj = get_obj_in_list_vis(ch, argument, ch->carrying))) {
	    sprintf(buf, "You don't seem to have %s %s.\n\r", AN(argument), argument);
	    send_to_char(buf, ch);
	 } else
	    perform_give(ch, vict, obj);
      }
   }
}

