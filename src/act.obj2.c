/* ************************************************************************
*   File: act.obj2.c                                    Part of CircleMUD *
*  Usage: eating/drinking and wearing/removing equipment                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"

/* extern variables */
extern struct str_app_type str_app[];
extern struct room_data *world;
extern char	*drinks[];
extern int	drink_aff[][3];


void	weight_change_object(struct obj_data *obj, int weight)
{
   struct obj_data *tmp_obj;
   struct char_data *tmp_ch;

   if (obj->in_room != NOWHERE) {
      GET_OBJ_WEIGHT(obj) += weight;
   } else if ((tmp_ch = obj->carried_by)) {
      obj_from_char(obj);
      GET_OBJ_WEIGHT(obj) += weight;
      obj_to_char(obj, tmp_ch);
   } else if ((tmp_obj = obj->in_obj)) {
      obj_from_obj(obj);
      GET_OBJ_WEIGHT(obj) += weight;
      obj_to_obj(obj, tmp_obj);
   } else {
      log("SYSERR: Unknown attempt to subtract weight from an object.");
   }
}



void	name_from_drinkcon(struct obj_data *obj)
{
   int	i;
   char	*new_name;
   extern struct obj_data *obj_proto;

   for (i = 0; (*((obj->name) + i) != ' ') && (*((obj->name) + i) != '\0'); i++)
      ;

   if (*((obj->name) + i) == ' ') {
      new_name = str_dup((obj->name) + i + 1);
      if (obj->item_number < 0 || obj->name != obj_proto[obj->item_number].name)
	 free(obj->name);
      obj->name = new_name;
   }
}



void	name_to_drinkcon(struct obj_data *obj, int type)
{
   char	*new_name;
   extern struct obj_data *obj_proto;
   extern char	*drinknames[];

   CREATE(new_name, char, strlen(obj->name) + strlen(drinknames[type]) + 2);
   sprintf(new_name, "%s %s", drinknames[type], obj->name);
   if (obj->item_number < 0 || obj->name != obj_proto[obj->item_number].name)
      free(obj->name);
   obj->name = new_name;
}



ACMD(do_drink)
{
   struct obj_data *temp;
   struct affected_type af;
   int	amount, weight;
   int	on_ground = 0;

   one_argument(argument, arg);

   if (!*arg) {
      send_to_char("Drink from what?\n\r", ch);
      return;
   }

   if (!(temp = get_obj_in_list_vis(ch, arg, ch->carrying))) {
      if (!(temp = get_obj_in_list_vis(ch, arg, world[ch->in_room].contents))) {
	 act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
	 return;
      } else
	 on_ground = 1;
   }

   if ((GET_ITEM_TYPE(temp) != ITEM_DRINKCON) && 
       (GET_ITEM_TYPE(temp) != ITEM_FOUNTAIN)) {
      send_to_char("You can't drink from that!\n\r", ch);
      return;
   }

   if (on_ground && (GET_ITEM_TYPE(temp) == ITEM_DRINKCON)) {
      send_to_char("You have to be holding that to drink from it.\n\r", ch);
      return;
   }

   if ((GET_COND(ch, DRUNK) > 10) && (GET_COND(ch, THIRST) > 0)) {
      /* The pig is drunk */
      send_to_char("You can't seem to get close enough to your mouth.\n\r", ch);
      act("$n tries to drink but misses $s mouth!", TRUE, ch, 0, 0, TO_ROOM);
      return;
   }

   if ((GET_COND(ch, FULL) > 20) && (GET_COND(ch, THIRST) > 0)) {
      send_to_char("Your stomach can't contain anymore!\n\r", ch);
      return;
   }

   if (!temp->obj_flags.value[1]) {
      send_to_char("It's empty.\n\r", ch);
      return;
   }

   if (subcmd == SCMD_DRINK) {
      if (temp->obj_flags.type_flag != ITEM_FOUNTAIN) {
         sprintf(buf, "$n drinks %s from $p.", drinks[temp->obj_flags.value[2]]);
         act(buf, TRUE, ch, temp, 0, TO_ROOM);
      }

      sprintf(buf, "You drink the %s.\n\r", drinks[temp->obj_flags.value[2]]);
      send_to_char(buf, ch);

      if (drink_aff[temp->obj_flags.value[2]][DRUNK] > 0 )
         amount = (25 - GET_COND(ch, THIRST)) / drink_aff[temp->obj_flags.value[2]][DRUNK];
      else
         amount = number(3, 10);

   } else {
      act("$n sips from the $o.", TRUE, ch, temp, 0, TO_ROOM);
      sprintf(buf, "It tastes like %s.\n\r", drinks[temp->obj_flags.value[2]]);
      send_to_char(buf, ch);
      amount = 1;
   }

   amount = MIN(amount, temp->obj_flags.value[1]);

   /* You can't subtract more than the object weighs */
   weight = MIN(amount, temp->obj_flags.weight);

   weight_change_object(temp, -weight);  /* Subtract amount */

   gain_condition(ch, DRUNK,
      (int)((int)drink_aff[temp->obj_flags.value[2]][DRUNK]*amount) / 4);

   gain_condition(ch, FULL,
      (int)((int)drink_aff[temp->obj_flags.value[2]][FULL]*amount) / 4);

   gain_condition(ch, THIRST,
      (int)((int)drink_aff[temp->obj_flags.value[2]][THIRST]*amount) / 4);

   if (GET_COND(ch, DRUNK) > 10)
      send_to_char("You feel drunk.\n\r", ch);

   if (GET_COND(ch, THIRST) > 20)
      send_to_char("You don't feel thirsty any more.\n\r", ch);

   if (GET_COND(ch, FULL) > 20)
      send_to_char("You are full.\n\r", ch);

   if (temp->obj_flags.value[3]) { /* The shit was poisoned ! */
      send_to_char("Oops, it tasted rather strange!\n\r", ch);
      act("$n chokes and utters some strange sounds.", TRUE, ch, 0, 0, TO_ROOM);

      af.type = SPELL_POISON;
      af.duration = amount * 3;
      af.modifier = 0;
      af.location = APPLY_NONE;
      af.bitvector = AFF_POISON;
      affect_join(ch, &af, FALSE, FALSE);
   }

   /* empty the container, and no longer poison. */
   temp->obj_flags.value[1] -= amount;
   if (!temp->obj_flags.value[1]) {  /* The last bit */
      temp->obj_flags.value[2] = 0;
      temp->obj_flags.value[3] = 0;
      name_from_drinkcon(temp);
   }

   return;
}



ACMD(do_eat)
{
   struct obj_data *food;
   struct affected_type af;
   int	amount;

   one_argument(argument, arg);

   if (!*arg) {
      send_to_char("Eat what?\n\r", ch);
      return;
   }

   if (!(food = get_obj_in_list_vis(ch, arg, ch->carrying))) {
      send_to_char("You don't seem to have any.\n\r", ch);
      return;
   }

   if (subcmd == SCMD_TASTE && ((GET_ITEM_TYPE(food) == ITEM_DRINKCON) ||
      (GET_ITEM_TYPE(food) == ITEM_FOUNTAIN))) {
         do_drink(ch, argument, 0, SCMD_SIP);
	 return;
   }

   if ((GET_ITEM_TYPE(food) != ITEM_FOOD) && (GET_LEVEL(ch) < LEVEL_GOD)) {
      send_to_char("You can't eat THAT!\n\r", ch);
      return;
   }

   if (GET_COND(ch, FULL) > 20) { /* Stomach full */
      act("You are too full to eat more!", FALSE, ch, 0, 0, TO_CHAR);
      return;
   }

   if (subcmd == SCMD_EAT) {
      act("You eat the $o.", FALSE, ch, food, 0, TO_CHAR);
      act("$n eats $p.", TRUE, ch, food, 0, TO_ROOM);
   } else {
      act("You nibble a little bit of the $o.", FALSE, ch, food, 0, TO_CHAR);
      act("$n tastes a little bit of $p.", TRUE, ch, food, 0, TO_ROOM);
   }

   amount = (subcmd == SCMD_EAT ? food->obj_flags.value[0] : 1);

   gain_condition(ch, FULL, amount);

   if (GET_COND(ch, FULL) > 20)
      act("You are full.", FALSE, ch, 0, 0, TO_CHAR);

   if (food->obj_flags.value[3] && (GET_LEVEL(ch) < LEVEL_IMMORT)) {
      /* The shit was poisoned ! */
      send_to_char("Oops, that tasted rather strange!\n\r", ch);
      act("$n coughs and utters some strange sounds.", FALSE, ch, 0, 0, TO_ROOM);

      af.type = SPELL_POISON;
      af.duration = amount * 2;
      af.modifier = 0;
      af.location = APPLY_NONE;
      af.bitvector = AFF_POISON;
      affect_join(ch, &af, FALSE, FALSE);
   }

   if (subcmd == SCMD_EAT)
      extract_obj(food);
   else {
      if (!(--food->obj_flags.value[0])) {
	 send_to_char("There's nothing left now.\n\r", ch);
	 extract_obj(food);
      }
   }
}


ACMD(do_pour)
{
   char	arg1[MAX_INPUT_LENGTH];
   char	arg2[MAX_INPUT_LENGTH];
   struct obj_data *from_obj;
   struct obj_data *to_obj;
   int	amount;

   argument_interpreter(argument, arg1, arg2);

   if (subcmd == SCMD_POUR) {
      if (!*arg1) /* No arguments */ {
	 act("What do you want to pour from?", FALSE, ch, 0, 0, TO_CHAR);
	 return;
      }

      if (!(from_obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
	 act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
	 return;
      }

      if (from_obj->obj_flags.type_flag != ITEM_DRINKCON) {
	 act("You can't pour from that!", FALSE, ch, 0, 0, TO_CHAR);
	 return;
      }
   }

   if (subcmd == SCMD_FILL) {
      if (!*arg1) /* no arguments */ {
	 send_to_char("What do you want to fill?  And what are you filling it from?\n\r", ch);
	 return;
      }

      if (!(to_obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
	 send_to_char("You can't find it!", ch);
	 return;
      }

      if (GET_ITEM_TYPE(to_obj) != ITEM_DRINKCON) {
	 act("You can't fill $p!", FALSE, ch, to_obj, 0, TO_CHAR);
	 return;
      }

      if (!*arg2) /* no 2nd argument */ {
	 act("What do you want to fill $p from?", FALSE, ch, to_obj, 0, TO_CHAR);
	 return;
      }

      if (!(from_obj = get_obj_in_list_vis(ch, arg2, world[ch->in_room].contents))) {
	 sprintf(buf, "There doesn't seem to be any '%s' here.\n\r", arg2);
	 send_to_char(buf, ch);
	 return;
      }

      if (GET_ITEM_TYPE(from_obj) != ITEM_FOUNTAIN) {
	 act("You can't fill something from $p.", FALSE, ch, from_obj, 0, TO_CHAR);
	 return;
      }
   }

   if (from_obj->obj_flags.value[1] == 0) {
      act("The $p is empty.", FALSE, ch, from_obj, 0, TO_CHAR);
      return;
   }

   if (subcmd == SCMD_POUR) /* pour */ {
      if (!*arg2) {
	 act("Where do you want it?  Out or in what?", FALSE, ch, 0, 0, TO_CHAR);
	 return;
      }

      if (!str_cmp(arg2, "out")) {
	 act("$n empties $p.", TRUE, ch, from_obj, 0, TO_ROOM);
	 act("You empty $p.", FALSE, ch, from_obj, 0, TO_CHAR);

	 weight_change_object(from_obj, -from_obj->obj_flags.value[1]); /* Empty */

	 from_obj->obj_flags.value[1] = 0;
	 from_obj->obj_flags.value[2] = 0;
	 from_obj->obj_flags.value[3] = 0;
	 name_from_drinkcon(from_obj);

	 return;
      }

      if (!(to_obj = get_obj_in_list_vis(ch, arg2, ch->carrying))) {
	 act("You can't find it!", FALSE, ch, 0, 0, TO_CHAR);
	 return;
      }

      if ((to_obj->obj_flags.type_flag != ITEM_DRINKCON) && 
          (to_obj->obj_flags.type_flag != ITEM_FOUNTAIN)) {
	 act("You can't pour anything into that.", FALSE, ch, 0, 0, TO_CHAR);
	 return;
      }
   }

   if (to_obj == from_obj) {
      act("A most unproductive effort.", FALSE, ch, 0, 0, TO_CHAR);
      return;
   }

   if ((to_obj->obj_flags.value[1] != 0) && 
       (to_obj->obj_flags.value[2] != from_obj->obj_flags.value[2])) {
      act("There is already another liquid in it!", FALSE, ch, 0, 0, TO_CHAR);
      return;
   }

   if (!(to_obj->obj_flags.value[1] < to_obj->obj_flags.value[0])) {
      act("There is no room for more.", FALSE, ch, 0, 0, TO_CHAR);
      return;
   }

   if (subcmd == SCMD_POUR) {
      sprintf(buf, "You pour the %s into the %s.",
          drinks[from_obj->obj_flags.value[2]], arg2);
      send_to_char(buf, ch);
   }

   if (subcmd == SCMD_FILL) {
      act("You gently fill $p from $P.", FALSE, ch, to_obj, from_obj, TO_CHAR);
      act("$n gently fills $p from $P.", TRUE, ch, to_obj, from_obj, TO_ROOM);
   }

   /* New alias */
   if (to_obj->obj_flags.value[1] == 0)
      name_to_drinkcon(to_obj, from_obj->obj_flags.value[2]);

   /* First same type liq. */
   to_obj->obj_flags.value[2] = from_obj->obj_flags.value[2];

   /* Then how much to pour */
   from_obj->obj_flags.value[1] -= (amount = 
       (to_obj->obj_flags.value[0] - to_obj->obj_flags.value[1]));

   to_obj->obj_flags.value[1] = to_obj->obj_flags.value[0];

   if (from_obj->obj_flags.value[1] < 0) {  /* There was too little */
      to_obj->obj_flags.value[1] += from_obj->obj_flags.value[1];
      amount += from_obj->obj_flags.value[1];
      from_obj->obj_flags.value[1] = 0;
      from_obj->obj_flags.value[2] = 0;
      from_obj->obj_flags.value[3] = 0;
      name_from_drinkcon(from_obj);
   }

   /* Then the poison boogie */
   to_obj->obj_flags.value[3] =
      (to_obj->obj_flags.value[3] || from_obj->obj_flags.value[3]);

   /* And the weight boogie */
   weight_change_object(from_obj, -amount);
   weight_change_object(to_obj, amount);   /* Add weight */

   return;
}



void	wear_message(struct char_data *ch, struct obj_data *obj, int where)
{
   char *wear_messages[][2] = {
      { "$n lights $p and holds it.",
	"You light $p and hold it." },

      { "$n slides $p on to $s right ring finger.",
	"You slide $p on to your right ring finger." },

      { "$n slides $p on to $s left ring finger.",
	"You slide $p on to your left ring finger." },

      { "$n wears $p around $s neck.",
        "You wear $p around your neck." },

      { "$n wears $p around $s neck.",
        "You wear $p around your neck." },

      { "$n wears $p on $s body." ,
	"You wear $p on your body.", },

      { "$n wears $p on $s head.",
	"You wear $p on your head." },

      { "$n puts $p on $s legs.",
	"You put $p on your legs." },

      { "$n wears $p on $s feet.",
	"You wear $p on your feet." },

      { "$n puts $p on $s hands.",
	"You put $p on your hands." },

      { "$n wears $p on $s arms.",
	"You wear $p on your arms." },

      { "$n straps $p around his arm as a shield.",
	"You start to use $p as a shield." },

      { "$n wears $p about $s body." ,
	"You wear $p around your body." },

      { "$n wears $p around $s waist.",
	"You wear $p around your waist." },

      { "$n puts $p on around $s right wrist.",
	"You put $p on around your right wrist." },

      { "$n puts $p on around $s left wrist.",
	"You put $p on around your left wrist." },

      { "$n wields $p.",
        "You wield $p." },

      { "$n grabs $p.",
	"You grab $p." }
   };

   act(wear_messages[where][0], TRUE, ch, obj, 0, TO_ROOM);
   act(wear_messages[where][1], FALSE, ch, obj, 0, TO_CHAR);
}



void	perform_wear(struct char_data *ch, struct obj_data *obj, int where)
{
   int wear_bitvectors[] = {
      ITEM_TAKE, ITEM_WEAR_FINGER, ITEM_WEAR_FINGER, ITEM_WEAR_NECK,
      ITEM_WEAR_NECK, ITEM_WEAR_BODY, ITEM_WEAR_HEAD, ITEM_WEAR_LEGS,
      ITEM_WEAR_FEET, ITEM_WEAR_HANDS, ITEM_WEAR_ARMS, ITEM_WEAR_SHIELD,
      ITEM_WEAR_ABOUT, ITEM_WEAR_WAISTE, ITEM_WEAR_WRIST, ITEM_WEAR_WRIST,
      ITEM_WIELD, ITEM_TAKE };

   char *already_wearing[] = {
      "You're already using a light.\n\r",
      "YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\n\r",
      "You're already wearing something on both of your ring fingers.\n\r",
      "YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\n\r",
      "You can't wear anything else around your neck.\n\r",
      "You're already wearing something on your body.\n\r",
      "You're already wearing something on your head.\n\r",
      "You're already wearing something on your legs.\n\r",
      "You're already wearing something on your feet.\n\r",
      "You're already wearing something on your hands.\n\r",
      "You're already wearing something on your arms.\n\r",
      "You're already using a shield.\n\r",
      "You're already wearing something about your body.\n\r",
      "You already have something around your waist.\n\r",
      "YOU SHOULD NEVER SEE THIS MESSAGE.  PLEASE REPORT.\n\r",
      "You're already wearing something around both of your wrists.\n\r",
      "You're already wielding a weapon.\n\r",
      "You're already holding something.\n\r"
   };

   /* first, make sure that the wear position is valid. */
   if (!CAN_WEAR(obj, wear_bitvectors[where])) {
      act("You can't wear $p there.", FALSE, ch, obj, 0, TO_CHAR);
      return;
   }

   /* for neck, finger, and wrist, try pos 2 if pos 1 is already full */
   if ((where == WEAR_FINGER_R) || (where == WEAR_NECK_1) || (where == WEAR_WRIST_R))
      if (ch->equipment[where])
         where++;

   if (ch->equipment[where]) {
      send_to_char(already_wearing[where], ch);
      return;
   }

   wear_message(ch, obj, where);
   obj_from_char(obj);
   equip_char(ch, obj, where);
}



int find_eq_pos(struct char_data *ch, struct obj_data *obj, char *arg)
{
   int where = -1;

   static char	*keywords[] = {
      "!RESERVED!",
      "finger",
      "!RESERVED!",
      "neck",
      "!RESERVED!",
      "body",
      "head",
      "legs",
      "feet",
      "hands",
      "arms",
      "sheild",
      "about",
      "waist",
      "wrist",
      "!RESERVED!",
      "!RESERVED!",
      "!RESERVED!",
      "\n"
   };

   if (!arg || !*arg) {
      if (CAN_WEAR(obj, ITEM_WEAR_FINGER))	where = WEAR_FINGER_R;
      if (CAN_WEAR(obj, ITEM_WEAR_NECK))	where = WEAR_NECK_1;
      if (CAN_WEAR(obj, ITEM_WEAR_BODY))	where = WEAR_BODY;
      if (CAN_WEAR(obj, ITEM_WEAR_HEAD))	where = WEAR_HEAD;
      if (CAN_WEAR(obj, ITEM_WEAR_LEGS))	where = WEAR_LEGS;
      if (CAN_WEAR(obj, ITEM_WEAR_FEET))	where = WEAR_FEET;
      if (CAN_WEAR(obj, ITEM_WEAR_HANDS))	where = WEAR_HANDS;
      if (CAN_WEAR(obj, ITEM_WEAR_ARMS))	where = WEAR_ARMS;
      if (CAN_WEAR(obj, ITEM_WEAR_SHIELD))	where = WEAR_SHIELD;
      if (CAN_WEAR(obj, ITEM_WEAR_ABOUT))	where = WEAR_ABOUT;
      if (CAN_WEAR(obj, ITEM_WEAR_WAISTE))	where = WEAR_WAISTE;
      if (CAN_WEAR(obj, ITEM_WEAR_WRIST))	where = WEAR_WRIST_R;
   } else {
      if ((where = search_block(arg, keywords, FALSE)) < 0) {
	 sprintf(buf, "'%s'?  What part of your body is THAT?\n\r", arg);
	 send_to_char(buf, ch);
      }
   }

   return where;
}



ACMD(do_wear)
{
   char	arg1[MAX_INPUT_LENGTH];
   char	arg2[MAX_INPUT_LENGTH];
   struct obj_data *obj, *next_obj;
   int	where, dotmode, items_worn = 0;
 
   argument_interpreter(argument, arg1, arg2);

   if (!*arg1) {
      send_to_char("Wear what?\n\r", ch);
      return;
   }

   dotmode = find_all_dots(arg1);

   if (*arg2 && (dotmode != FIND_INDIV)) {
      send_to_char("You can't specify the same body location for more than one item!\n\r", ch);
      return;
   }

   if (dotmode == FIND_ALL) {
      for (obj = ch->carrying; obj; obj = next_obj) {
	 next_obj = obj->next_content;
	 if ((where = find_eq_pos(ch, obj, 0)) >= 0) {
	    items_worn++;
	    perform_wear(ch, obj, where);
	 }
      }
      if (!items_worn)
	 send_to_char("You don't seem to have anything wearable.\n\r", ch);
   } else if (dotmode == FIND_ALLDOT) {
      if (!*arg1) {
	 send_to_char("Wear all of what?\n\r", ch);
	 return;
      }
      if (!(obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
	 sprintf(buf, "You don't seem to have any %ss.\n\r", arg1);
	 send_to_char(buf, ch);
      } else while (obj) {
	 next_obj = get_obj_in_list_vis(ch, arg1, obj->next_content);
	 if ((where = find_eq_pos(ch, obj, 0)) >= 0)
	    perform_wear(ch, obj, where);
	 else
	    act("You can't wear $p.", FALSE, ch, obj, 0, TO_CHAR);
	 obj = next_obj;
      }
   } else {
      if (!(obj = get_obj_in_list_vis(ch, arg1, ch->carrying))) {
	 sprintf(buf, "You don't seem to have %s %s.\n\r", AN(arg1), arg1);
	 send_to_char(buf, ch);
      } else {
	 if ((where = find_eq_pos(ch, obj, arg2)) >= 0)
	    perform_wear(ch, obj, where);
	 else if (!*arg2)
            act("You can't wear $p.", FALSE, ch, obj, 0, TO_CHAR);
      }
   }
}



ACMD(do_wield)
{
   struct obj_data *obj;

   one_argument(argument, arg);

   if (!*arg)
      send_to_char("Wield what?\n\r", ch);
   else if (!(obj = get_obj_in_list(arg, ch->carrying))) {
      sprintf(buf, "You don't seem to have %s %s.\n\r", AN(arg), arg);
      send_to_char(buf, ch);
   } else {
      if (!CAN_WEAR(obj, ITEM_WIELD)) 
	 send_to_char("You can't wield that.\n\r", ch);
      else if (GET_OBJ_WEIGHT(obj) > str_app[STRENGTH_APPLY_INDEX(ch)].wield_w)
	 send_to_char("It's too heavy for you to use.\n\r", ch);
      else
	 perform_wear(ch, obj, WIELD);
   }
}



ACMD(do_grab)
{
   struct obj_data *obj;

   one_argument(argument, arg);

   if (!*arg)
      send_to_char("Hold what?\n\r", ch);
   else if (!(obj = get_obj_in_list(arg, ch->carrying))) {
      sprintf(buf, "You don't seem to have %s %s.\n\r", AN(arg), arg);
      send_to_char(buf, ch);
   } else {
      if (GET_ITEM_TYPE(obj) == ITEM_LIGHT) {
	 perform_wear(ch, obj, WEAR_LIGHT);
         if (obj->obj_flags.value[2])
            world[ch->in_room].light++;
      } else
	 perform_wear(ch, obj, HOLD);
   }
}



void	perform_remove(struct char_data *ch, int pos)
{
   struct obj_data *obj;

   if (!(obj = ch->equipment[pos])) {
      log("Error in perform_remove: bad pos passed.");
      return;
   }

   if (IS_CARRYING_N(ch) >= CAN_CARRY_N(ch))
      act("$p: you can't carry that many items!", FALSE, ch, obj, 0, TO_CHAR);
   else {
      obj_to_char(unequip_char(ch, pos), ch);

      if (obj->obj_flags.type_flag == ITEM_LIGHT)
	 if (obj->obj_flags.value[2])
	    world[ch->in_room].light--;

      act("You stop using $p.", FALSE, ch, obj, 0, TO_CHAR);
      act("$n stops using $p.", TRUE, ch, obj, 0, TO_ROOM);
   }
}



ACMD(do_remove)
{
   struct obj_data *obj;
   int	i, dotmode, found;

   one_argument(argument, arg);

   if (!*arg) {
      send_to_char("Remove what?\n\r", ch);
      return;
   }

   dotmode = find_all_dots(arg);

   if (dotmode == FIND_ALL) {
      found = 0;
      for (i = 0; i < MAX_WEAR; i++)
         if (ch->equipment[i]) {
            perform_remove(ch, i);
	    found = 1;
	 }
      if (!found)
	 send_to_char("You're not using anything.\n\r", ch);
   } else if (dotmode == FIND_ALLDOT) {
      if (!*arg)
         send_to_char("Remove all of what?\n\r", ch);
      else {
         found = 0;
         for (i = 0; i < MAX_WEAR; i++)
            if (ch->equipment[i] && CAN_SEE_OBJ(ch, ch->equipment[i]) &&
	     isname(arg, ch->equipment[i]->name)) {
	       perform_remove(ch, i);
	       found = 1;
	    }
	 if (!found) {
	    sprintf(buf, "You don't seem to be using any %ss.\n\r", arg);
	    send_to_char(buf, ch);
	 }
      }
   } else {
      if (!(obj = get_object_in_equip_vis(ch, arg, ch->equipment, &i))) {
	 sprintf(buf, "You don't seem to be using %s %s.\n\r", AN(arg), arg);
	 send_to_char(buf, ch);
      } else
	 perform_remove(ch, i);
   }
}
