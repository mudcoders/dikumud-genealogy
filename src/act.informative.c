/* ************************************************************************
*   File: act.informative.c                             Part of CircleMUD *
*  Usage: Player-level commands of an informative nature                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"
#include "screen.h"

/* extern variables */
extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern struct char_data *character_list;
extern struct obj_data *object_list;
extern struct title_type titles[4][35];
extern struct command_info cmd_info[];

extern char	*credits;
extern char	*news;
extern char	*info;
extern char	*wizlist;
extern char	*immlist;
extern char	*policies;
extern char	*handbook;
extern char	*dirs[];
extern char	*where[];
extern char	*color_liquid[];
extern char	*fullness[];
extern char	*connected_types[];
extern char	*command[];
extern char	*class_abbrevs[];
extern char	*room_bits[];

/* intern functions & vars*/
int	num_of_cmds;
void	list_obj_to_char(struct obj_data *list, struct char_data *ch, int mode, bool show);


/* Procedures related to 'look' */
void	argument_split_2(char *argument, char *first_arg, char *second_arg)
{
   int	look_at, found, begin;
   found = begin = 0;

   /* Find first non blank */
   for ( ; *(argument + begin ) == ' ' ; begin++)
      ;

   /* Find length of first word */
   for (look_at = 0; *(argument + begin + look_at) > ' ' ; look_at++)

      /* Make all letters lower case, AND copy them to first_arg */
      *(first_arg + look_at) = LOWER(*(argument + begin + look_at));
   *(first_arg + look_at) = '\0';
   begin += look_at;

   /* Find first non blank */
   for ( ; *(argument + begin ) == ' ' ; begin++)
      ;

   /* Find length of second word */
   for ( look_at = 0; *(argument + begin + look_at) > ' ' ; look_at++)

      /* Make all letters lower case, AND copy them to second_arg */
      *(second_arg + look_at) = LOWER(*(argument + begin + look_at));
   *(second_arg + look_at) = '\0';
   begin += look_at;
}



char	*find_ex_description(char *word, struct extra_descr_data *list)
{
   struct extra_descr_data *i;

   for (i = list; i; i = i->next)
      if (isname(word, i->keyword))
	 return(i->description);

   return(0);
}


void	show_obj_to_char(struct obj_data *object, struct char_data *ch, int mode)
{
   bool found;

   *buf = '\0';
   if ((mode == 0) && object->description)
      strcpy(buf, object->description);
   else if (object->short_description && ((mode == 1) || 
       (mode == 2) || (mode == 3) || (mode == 4)))
      strcpy(buf, object->short_description);
   else if (mode == 5) {
      if (object->obj_flags.type_flag == ITEM_NOTE) {
	 if (object->action_description) {
	    strcpy(buf, "There is something written upon it:\n\r\n\r");
	    strcat(buf, object->action_description);
	    page_string(ch->desc, buf, 1);
	 } else
	    act("It's blank.", FALSE, ch, 0, 0, TO_CHAR);
	 return;
      } else if ((object->obj_flags.type_flag != ITEM_DRINKCON)) {
	 strcpy(buf, "You see nothing special..");
      } else /* ITEM_TYPE == ITEM_DRINKCON||FOUNTAIN */
	 strcpy(buf, "It looks like a drink container.");
   }

   if (mode != 3) {
      found = FALSE;
      if (IS_OBJ_STAT(object, ITEM_INVISIBLE)) {
	 strcat(buf, "(invisible)");
	 found = TRUE;
      }
      if (IS_OBJ_STAT(object, ITEM_EVIL) && IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
	 strcat(buf, "..It glows red!");
	 found = TRUE;
      }
      if (IS_OBJ_STAT(object, ITEM_MAGIC) && IS_AFFECTED(ch, AFF_DETECT_MAGIC)) {
	 strcat(buf, "..It glows blue!");
	 found = TRUE;
      }
      if (IS_OBJ_STAT(object, ITEM_GLOW)) {
	 strcat(buf, "..It has a soft glowing aura!");
	 found = TRUE;
      }
      if (IS_OBJ_STAT(object, ITEM_HUM)) {
	 strcat(buf, "..It emits a faint humming sound!");
	 found = TRUE;
      }
   }

   strcat(buf, "\n\r");
   page_string(ch->desc, buf, 1);
}


void	list_obj_to_char(struct obj_data *list, struct char_data *ch, int mode, 
bool show)
{
   struct obj_data *i;
   bool found;

   found = FALSE;
   for ( i = list ; i ; i = i->next_content ) {
      if (CAN_SEE_OBJ(ch, i)) {
	 show_obj_to_char(i, ch, mode);
	 found = TRUE;
      }
   }
   if ((!found) && (show))
      send_to_char(" Nothing.\n\r", ch);
}


void	diag_char_to_char(struct char_data *i, struct char_data *ch)
{
   int	percent;

   if (GET_MAX_HIT(i) > 0)
      percent = (100 * GET_HIT(i)) / GET_MAX_HIT(i);
   else
      percent = -1; /* How could MAX_HIT be < 1?? */

   strcpy(buf, GET_NAME(i));
   CAP(buf);

   if (percent >= 100)
      strcat(buf, " is in excellent condition.\n\r");
   else if (percent >= 90)
      strcat(buf, " has a few scratches.\n\r");
   else if (percent >= 75)
      strcat(buf, " has some small wounds and bruises.\n\r");
   else if (percent >= 50)
      strcat(buf, " has quite a few wounds.\n\r");
   else if (percent >= 30)
      strcat(buf, " has some big nasty wounds and scratches.\n\r");
   else if (percent >= 15)
      strcat(buf, " looks pretty hurt.\n\r");
   else if (percent >= 0)
      strcat(buf, " is in awful condition.\n\r");
   else
      strcat(buf, " is bleeding awfully from big wounds.\n\r");

   send_to_char(buf, ch);
}


void	show_char_to_char(struct char_data *i, struct char_data *ch, int mode)
{
   int	j, found;
   struct obj_data *tmp_obj;

   if (mode == 0) {

      if (IS_AFFECTED(i, AFF_HIDE) || !CAN_SEE(ch, i)) {
	 if (IS_AFFECTED(ch, AFF_SENSE_LIFE))
	    send_to_char("You sense a hidden life form in the room.\n\r", ch);
	 return;
      }

      if (!(i->player.long_descr) || (GET_POS(i) != i->specials.default_pos)) {
	 /* A player char or a mobile without long descr, or not in default pos. */
	 if (!IS_NPC(i))
	    sprintf(buf, "%s %s", i->player.name, GET_TITLE(i));
	 else {
	    strcpy(buf, i->player.short_descr);
	    CAP(buf);
	 }

	 if (IS_AFFECTED(i, AFF_INVISIBLE))
	    strcat(buf, " (invisible)");

	 if (!IS_NPC(i) && !i->desc)
	    strcat(buf, " (linkless)");

	 if (PLR_FLAGGED(i, PLR_WRITING))
	    strcat(buf, " (writing)");

	 switch (GET_POS(i)) {
	 case POSITION_STUNNED  :
	    strcat(buf, " is lying here, stunned.");
	    break;
	 case POSITION_INCAP    :
	    strcat(buf, " is lying here, incapacitated.");
	    break;
	 case POSITION_MORTALLYW:
	    strcat(buf, " is lying here, mortally wounded.");
	    break;
	 case POSITION_DEAD     :
	    strcat(buf, " is lying here, dead.");
	    break;
	 case POSITION_STANDING :
	    strcat(buf, " is standing here.");
	    break;
	 case POSITION_SITTING  :
	    strcat(buf, " is sitting here.");
	    break;
	 case POSITION_RESTING  :
	    strcat(buf, " is resting here.");
	    break;
	 case POSITION_SLEEPING :
	    strcat(buf, " is sleeping here.");
	    break;
	 case POSITION_FIGHTING :
	    if (i->specials.fighting) {
	       strcat(buf, " is here, fighting ");
	       if (i->specials.fighting == ch)
		  strcat(buf, "YOU!");
	       else {
		  if (i->in_room == i->specials.fighting->in_room)
		     strcat(buf, GET_NAME(i->specials.fighting));
		  else
		     strcat(buf, "someone who has already left");
		  strcat(buf, ".");
	       }
	    } else /* NIL fighting pointer */
	       strcat(buf, " is here struggling with thin air.");
	    break;
	 default :
	    strcat(buf, " is floating here.");
	    break;
	 }
	 if (IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
	    if (IS_EVIL(i))
	       strcat(buf, " (Red Aura)");
	 }

	 strcat(buf, "\n\r");
	 send_to_char(buf, ch);
      } else { /* npc with long */
	 if (IS_AFFECTED(i, AFF_INVISIBLE))
	    strcpy(buf, "*");
	 else
	    *buf = '\0';

	 if (IS_AFFECTED(ch, AFF_DETECT_EVIL)) {
	    if (IS_EVIL(i))
	       strcat(buf, " (Red Aura)");
	 }

	 strcat(buf, i->player.long_descr);

	 send_to_char(buf, ch);
      }

      if (IS_AFFECTED(i, AFF_SANCTUARY))
	 act("$n glows with a bright light!", FALSE, i, 0, ch, TO_VICT);

   } else if (mode == 1) {

      if (i->player.description)
	 send_to_char(i->player.description, ch);
      else {
	 act("You see nothing special about $m.", FALSE, i, 0, ch, TO_VICT);
      }

      /* Show a character to another */

      diag_char_to_char(i, ch);

      found = FALSE;
      for (j = 0; j < MAX_WEAR; j++) {
	 if (i->equipment[j]) {
	    if (CAN_SEE_OBJ(ch, i->equipment[j])) {
	       found = TRUE;
	    }
	 }
      }
      if (found) {
	 act("\n\r$n is using:", FALSE, i, 0, ch, TO_VICT);
	 for (j = 0; j < MAX_WEAR; j++) {
	    if (i->equipment[j]) {
	       if (CAN_SEE_OBJ(ch, i->equipment[j])) {
		  send_to_char(where[j], ch);
		  show_obj_to_char(i->equipment[j], ch, 1);
	       }
	    }
	 }
      }
      if (((GET_CLASS(ch) == CLASS_THIEF) || GET_LEVEL(ch) >= LEVEL_IMMORT) && (ch != i)) {
	 found = FALSE;
	 send_to_char("\n\rYou attempt to peek at the inventory:\n\r", ch);
	 for (tmp_obj = i->carrying; tmp_obj; tmp_obj = tmp_obj->next_content) {
	    if (CAN_SEE_OBJ(ch, tmp_obj) && (number(0, 20) < GET_LEVEL(ch))) {
	       show_obj_to_char(tmp_obj, ch, 1);
	       found = TRUE;
	    }
	 }
	 if (!found)
	    send_to_char("You can't see anything.\n\r", ch);
      }

   } else if (mode == 2) {

      /* Lists inventory */
      act("$n is carrying:", FALSE, i, 0, ch, TO_VICT);
      list_obj_to_char(i->carrying, ch, 1, TRUE);
   }
}



void	list_char_to_char(struct char_data *list, struct char_data *ch, 
int mode)
{
   struct char_data *i;

   for (i = list; i; i = i->next_in_room)
      if (ch != i) {
         if ((CAN_SEE(ch, i) && 
            (IS_AFFECTED(ch, AFF_SENSE_LIFE) || !IS_AFFECTED(i, AFF_HIDE))))
	       show_char_to_char(i, ch, 0);
          else if ((IS_DARK(ch->in_room)) && (IS_AFFECTED(i, AFF_INFRARED)))
	     send_to_char("You see a pair of glowing red eyes looking your way.\n\r", ch);
      }
}


ACMD(do_look)
{
   static char	arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
   int	keyword_no;
   int	j, bits, temp;
   bool found;
   struct obj_data *tmp_object, *found_object;
   struct char_data *tmp_char;
   char	*tmp_desc;
   static char	*keywords[] = {
      "north",
      "east",
      "south",
      "west",
      "up",
      "down",
      "in",
      "at",
      "",  /* Look at '' case */
      "\n"          };

   if (!ch->desc)
      return;

   if (GET_POS(ch) < POSITION_SLEEPING)
      send_to_char("You can't see anything but stars!\n\r", ch);
   else if (GET_POS(ch) == POSITION_SLEEPING)
      send_to_char("You can't see anything, you're sleeping!\n\r", ch);
   else if ( IS_AFFECTED(ch, AFF_BLIND) )
      send_to_char("You can't see a damned thing, you're blinded!\n\r", ch);
   else if ( IS_DARK(ch->in_room) && !PRF_FLAGGED(ch, PRF_HOLYLIGHT)) {
      send_to_char("It is pitch black...\n\r", ch);
      list_char_to_char(world[ch->in_room].people, ch, 0);
   } else {
      argument_split_2(argument, arg1, arg2);
      keyword_no = search_block(arg1, keywords, FALSE); /* Partiel Match */

      if ((keyword_no == -1) && *arg1) {
	 keyword_no = 7;
	 strcpy(arg2, arg1); /* Let arg2 become the target object (arg1) */
      }

      found = FALSE;
      tmp_object = 0;
      tmp_char	 = 0;
      tmp_desc	 = 0;

      switch (keyword_no) {
	 /* look <dir> */
      case 0 :
      case 1 :
      case 2 :
      case 3 :
      case 4 :
      case 5 :
	 if (EXIT(ch, keyword_no)) {
	    if (EXIT(ch, keyword_no)->general_description)
	       send_to_char(EXIT(ch, keyword_no)->general_description, ch);
	    else
	       send_to_char("You see nothing special.\n\r", ch);

	    if (IS_SET(EXIT(ch, keyword_no)->exit_info, EX_CLOSED) && 
	        (EXIT(ch, keyword_no)->keyword)) {
	       sprintf(buf, "The %s is closed.\n\r",
	           fname(EXIT(ch, keyword_no)->keyword));
	       send_to_char(buf, ch);
	    } else {
	       if (IS_SET(EXIT(ch, keyword_no)->exit_info, EX_ISDOOR) && 
	           EXIT(ch, keyword_no)->keyword) {
		  sprintf(buf, "The %s is open.\n\r",
		      fname(EXIT(ch, keyword_no)->keyword));
		  send_to_char(buf, ch);
	       }
	    }
	 } else {
	    send_to_char("Nothing special there...\n\r", ch);
	 }
	 break;

	 /* look 'in'	*/
      case 6:
	 if (*arg2) {
	    /* Item carried */

	    bits = generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM | 
	        FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);

	    if (bits) { /* Found something */
	       if ((GET_ITEM_TYPE(tmp_object) == ITEM_DRINKCON) || 
	           (GET_ITEM_TYPE(tmp_object) == ITEM_FOUNTAIN)) {
		  if (tmp_object->obj_flags.value[1] <= 0) {
		     act("It is empty.", FALSE, ch, 0, 0, TO_CHAR);
		  } else {
		     temp = ((tmp_object->obj_flags.value[1] * 3) / tmp_object->obj_flags.value[0]);
		     sprintf(buf, "It's %sfull of a %s liquid.\n\r",
		         fullness[temp], color_liquid[tmp_object->obj_flags.value[2]]);
		     send_to_char(buf, ch);
		  }
	       } else if (GET_ITEM_TYPE(tmp_object) == ITEM_CONTAINER) {
		  if (!IS_SET(tmp_object->obj_flags.value[1], CONT_CLOSED)) {
		     send_to_char(fname(tmp_object->name), ch);
		     switch (bits) {
		     case FIND_OBJ_INV :
			send_to_char(" (carried) : \n\r", ch);
			break;
		     case FIND_OBJ_ROOM :
			send_to_char(" (here) : \n\r", ch);
			break;
		     case FIND_OBJ_EQUIP :
			send_to_char(" (used) : \n\r", ch);
			break;
		     }
		     list_obj_to_char(tmp_object->contains, ch, 2, TRUE);
		  } else
		     send_to_char("It is closed.\n\r", ch);
	       } else {
		  send_to_char("That is not a container.\n\r", ch);
	       }
	    } else { /* wrong argument */
	       send_to_char("You do not see that item here.\n\r", ch);
	    }
	 } else { /* no argument */
	    send_to_char("Look in what?!\n\r", ch);
	 }
	 break;

	 /* look 'at'	*/
      case 7 :
	 if (*arg2) {

	    bits = generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM | 
	        FIND_OBJ_EQUIP | FIND_CHAR_ROOM, ch, &tmp_char, &found_object);

	    if (tmp_char) {
	       show_char_to_char(tmp_char, ch, 1);
	       if (ch != tmp_char) {
		  if (CAN_SEE(tmp_char, ch))
		     act("$n looks at you.", TRUE, ch, 0, tmp_char, TO_VICT);
		  act("$n looks at $N.", TRUE, ch, 0, tmp_char, TO_NOTVICT);
	       }
	       return;
	    }

	    /* Search for Extra Descriptions in room and items */

	    /* Extra description in room?? */
	    if (!found) {
	       tmp_desc = find_ex_description(arg2, 
	           world[ch->in_room].ex_description);
	       if (tmp_desc) {
		  page_string(ch->desc, tmp_desc, 0);
		  return; /* RETURN SINCE IT WAS A ROOM DESCRIPTION */
		  /* Old system was: found = TRUE; */
	       }
	    }

	    /* Search for extra descriptions in items */

	    /* Equipment Used */

	    if (!found) {
	       for (j = 0; j < MAX_WEAR && !found; j++) {
		  if (ch->equipment[j]) {
		     if (CAN_SEE_OBJ(ch, ch->equipment[j])) {
			tmp_desc = find_ex_description(arg2, 
			    ch->equipment[j]->ex_description);
			if (tmp_desc) {
			   page_string(ch->desc, tmp_desc, 1);
			   found = TRUE;
			}
		     }
		  }
	       }
	    }

	    /* In inventory */

	    if (!found) {
	       for (tmp_object = ch->carrying; 
	           tmp_object && !found; 
	           tmp_object = tmp_object->next_content) {
		  if CAN_SEE_OBJ(ch, tmp_object) {
		     tmp_desc = find_ex_description(arg2, 
		         tmp_object->ex_description);
		     if (tmp_desc) {
			page_string(ch->desc, tmp_desc, 1);
			found = TRUE;
		     }
		  }
	       }
	    }

	    /* Object In room */

	    if (!found) {
	       for (tmp_object = world[ch->in_room].contents; 
	           tmp_object && !found; 
	           tmp_object = tmp_object->next_content) {
		  if CAN_SEE_OBJ(ch, tmp_object) {
		     tmp_desc = find_ex_description(arg2, 
		         tmp_object->ex_description);
		     if (tmp_desc) {
			page_string(ch->desc, tmp_desc, 1);
			found = TRUE;
		     }
		  }
	       }
	    }
	    /* wrong argument */

	    if (bits) { /* If an object was found */
	       if (!found)
		  show_obj_to_char(found_object, ch, 5); /* Show no-description */
	       else
		  show_obj_to_char(found_object, ch, 6); /* Find hum, glow etc */
	    } else if (!found) {
	       send_to_char("You do not see that here.\n\r", ch);
	    }
	 } else {
	    /* no argument */

	    send_to_char("Look at what?\n\r", ch);
	 }

	 break;


	 /* look ''		*/
      case 8 :
	 strcpy(buf2, CCCYN(ch, C_CMP));
	 strcat(buf2, world[ch->in_room].name);
	 if (PRF_FLAGGED(ch, PRF_ROOMFLAGS)) {
	    sprintbit((long) world[ch->in_room].room_flags, room_bits, buf);
	    sprintf(buf2, "%s (#%d) [ %s]", buf2, world[ch->in_room].number, buf);
	 }

	 strcat(buf2, CCNRM(ch, C_CMP));
	 strcat(buf2, "\n\r");

	 if (!PRF_FLAGGED(ch, PRF_BRIEF) || (cmd == 15))
	    strcat(buf2, world[ch->in_room].description);
	 send_to_char(buf2, ch);

	 send_to_char(CCGRN(ch, C_CMP), ch);
	 list_obj_to_char(world[ch->in_room].contents, ch, 0, FALSE);
	 send_to_char(CCYEL(ch, C_CMP), ch);
	 list_char_to_char(world[ch->in_room].people, ch, 0);
	 send_to_char(CCNRM(ch, C_CMP), ch);
	 break;

	 /* wrong arg	*/
      case -1 :
	 send_to_char("Sorry, I didn't understand that!\n\r", ch);
	 break;
      }
   }
}


/* end of look */




ACMD(do_read)
{
   /* This is just for now - To be changed later.! */
   sprintf(buf1, "at %s", argument);
   do_look(ch, buf1, 15, 0);
}



ACMD(do_examine)
{
   char	name[100], buf[100];
   int	bits;
   struct char_data *tmp_char;
   struct obj_data *tmp_object;

   sprintf(buf, "at %s", argument);
   do_look(ch, buf, 15, 0);

   one_argument(argument, name);

   if (!*name) {
      send_to_char("Examine what?\n\r", ch);
      return;
   }

   bits = generic_find(name, FIND_OBJ_INV | FIND_OBJ_ROOM | 
       FIND_OBJ_EQUIP, ch, &tmp_char, &tmp_object);

   if (tmp_object) {
      if ((GET_ITEM_TYPE(tmp_object) == ITEM_DRINKCON) || 
          (GET_ITEM_TYPE(tmp_object) == ITEM_FOUNTAIN) || 
          (GET_ITEM_TYPE(tmp_object) == ITEM_CONTAINER)) {
	 send_to_char("When you look inside, you see:\n\r", ch);
	 sprintf(buf, "in %s", argument);
	 do_look(ch, buf, 15, 0);
      }
   }
}



ACMD(do_exits)
{
   int	door;
   char	*exits[] = 
    {
      "North",
      "East ",
      "South",
      "West ",
      "Up   ",
      "Down "
   };

   *buf = '\0';

   for (door = 0; door <= 5; door++)
      if (EXIT(ch, door))
	 if (EXIT(ch, door)->to_room != NOWHERE && 
	     !IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED))
	    if (GET_LEVEL(ch) >= LEVEL_IMMORT)
	       sprintf(buf + strlen(buf), "%-5s - [%5d] %s\n\r",
	           exits[door], world[EXIT(ch, door)->to_room].number,
	           world[EXIT(ch, door)->to_room].name);
	    else if (IS_DARK(EXIT(ch, door)->to_room) && !PRF_FLAGGED(ch, PRF_HOLYLIGHT))
	       sprintf(buf + strlen(buf), "%-5s - Too dark to tell\n\r", exits[door]);
	    else
	       sprintf(buf + strlen(buf), "%-5s - %s\n\r", exits[door],
	           world[EXIT(ch, door)->to_room].name);

   send_to_char("Obvious exits:\n\r", ch);

   if (*buf)
      send_to_char(buf, ch);
   else
      send_to_char(" None.\n\r", ch);
}


ACMD(do_score)
{
   struct time_info_data playing_time;
   struct time_info_data real_time_passed(time_t t2, time_t t1);

   sprintf(buf, "You are %d years old.", GET_AGE(ch));

   if ((age(ch).month == 0) && (age(ch).day == 0))
      strcat(buf, "  It's your birthday today.\n\r");
   else
      strcat(buf, "\n\r");

   sprintf(buf, 
       "%sYou have %d(%d) hit, %d(%d) mana and %d(%d) movement points.\n\r",
       buf,
       GET_HIT(ch), GET_MAX_HIT(ch),
       GET_MANA(ch), GET_MAX_MANA(ch),
       GET_MOVE(ch), GET_MAX_MOVE(ch));

   sprintf(buf, "%sYour armor class is %d/10, and your alignment is %d.\n\r",
       buf, GET_AC(ch), GET_ALIGNMENT(ch));

   sprintf(buf, "%sYou have scored %d exp, and have %d gold coins.\n\r",
       buf, GET_EXP(ch), GET_GOLD(ch));

   if (GET_LEVEL(ch) < LEVEL_IMMORT) {
      sprintf(buf, "%sYou need %d exp to reach your next level.\n\r", buf,
          (titles[GET_CLASS(ch)-1][GET_LEVEL(ch)+1].exp) - GET_EXP(ch));
   }

   playing_time = real_time_passed((time(0) - ch->player.time.logon) + 
       ch->player.time.played, 0);
   sprintf(buf, "%sYou have been playing for %d days and %d hours.\n\r",
       buf, playing_time.day, playing_time.hours);

   sprintf(buf, "%sThis ranks you as %s %s (level %d).\n\r", buf,
       GET_NAME(ch), GET_TITLE(ch), GET_LEVEL(ch));

   switch (GET_POS(ch)) {
   case POSITION_DEAD :
      strcat(buf, "You are DEAD!\n\r");
      break;
   case POSITION_MORTALLYW :
      strcat(buf, "You are mortally wounded!  You should seek help!\n\r");
      break;
   case POSITION_INCAP :
      strcat(buf, "You are incapacitated, slowly fading away...\n\r");
      break;
   case POSITION_STUNNED :
      strcat(buf, "You are stunned!  You can't move!\n\r");
      break;
   case POSITION_SLEEPING :
      strcat(buf, "You are sleeping.\n\r");
      break;
   case POSITION_RESTING  :
      strcat(buf, "You are resting.\n\r");
      break;
   case POSITION_SITTING  :
      strcat(buf, "You are sitting.\n\r");
      break;
   case POSITION_FIGHTING :
      if (ch->specials.fighting)
	 sprintf(buf, "%sYou are fighting %s.\n\r", buf, PERS(ch->specials.fighting, ch));
      else
	 strcat(buf, "You are fighting thin air.\n\r");
      break;
   case POSITION_STANDING :
      strcat(buf, "You are standing.\n\r");
      break;
   default :
      strcat(buf, "You are floating.\n\r");
      break;
   }

   if (GET_COND(ch, DRUNK) > 10)
      strcat(buf, "You are intoxicated.\n\r");

   if (GET_COND(ch, FULL) == 0)
      strcat(buf, "You are hungry.\n\r");

   if (GET_COND(ch, THIRST) == 0)
      strcat(buf, "You are thirsty.\n\r");

   if (IS_AFFECTED(ch, AFF_BLIND))
      strcat(buf, "You have been blinded!\n\r");

   if (IS_AFFECTED(ch, AFF_INVISIBLE))
      strcat(buf, "You are invisible.\n\r");

   if (IS_AFFECTED(ch, AFF_DETECT_INVISIBLE))
      strcat(buf, "You are sensitive to the presence of invisible things.\n\r");

   if (IS_AFFECTED(ch, AFF_SANCTUARY))
      strcat(buf, "You are protected by Sanctuary.\n\r");

   if (IS_AFFECTED(ch, AFF_POISON))
      strcat(buf, "You are poisoned!\n\r");

   if (IS_AFFECTED(ch, AFF_CHARM))
      strcat(buf, "You have been charmed!\n\r");

   if (affected_by_spell(ch, SPELL_ARMOR))
      strcat(buf, "You feel protected.\n\r");

   if (PRF_FLAGGED(ch, PRF_SUMMONABLE))
      strcat(buf, "You are summonable by other players.\n\r");

   send_to_char(buf, ch);
}


ACMD(do_time)
{
   char	*suf;
   int	weekday, day;
   extern struct time_info_data time_info;
   extern const char	*weekdays[];
   extern const char	*month_name[];

   sprintf(buf, "It is %d o'clock %s, on ",
       ((time_info.hours % 12 == 0) ? 12 : ((time_info.hours) % 12)),
       ((time_info.hours >= 12) ? "pm" : "am") );

   weekday = ((35 * time_info.month) + time_info.day + 1) % 7;/* 35 days in a month */

   strcat(buf, weekdays[weekday]);
   strcat(buf, "\n\r");
   send_to_char(buf, ch);

   day = time_info.day + 1;   /* day in [1..35] */

   if (day == 1)
      suf = "st";
   else if (day == 2)
      suf = "nd";
   else if (day == 3)
      suf = "rd";
   else if (day < 20)
      suf = "th";
   else if ((day % 10) == 1)
      suf = "st";
   else if ((day % 10) == 2)
      suf = "nd";
   else if ((day % 10) == 3)
      suf = "rd";
   else
      suf = "th";

   sprintf(buf, "The %d%s Day of the %s, Year %d.\n\r",
       day, suf, month_name[(int)time_info.month], time_info.year);

   send_to_char(buf, ch);
}


ACMD(do_weather)
{
   static char *sky_look[4] = {
      "cloudless",
      "cloudy",
      "rainy",
      "lit by flashes of lightning" };

   if (OUTSIDE(ch)) {
      sprintf(buf, 
          "The sky is %s and %s.\n\r",
          sky_look[weather_info.sky],
          (weather_info.change >= 0 ? "you feel a warm wind from south" : 
          "your foot tells you bad weather is due"));
      send_to_char(buf, ch);
   } else
      send_to_char("You have no feeling about the weather at all.\n\r", ch);
}


ACMD(do_help)
{
   extern int	top_of_helpt;
   extern struct help_index_element *help_index;
   extern FILE *help_fl;
   extern char	*help;

   int	chk, bot, top, mid, minlen;

   if (!ch->desc)
      return;

   for (; isspace(*argument); argument++)
      ;

   if (*argument) {
      if (!help_index) {
	 send_to_char("No help available.\n\r", ch);
	 return;
      }
      bot = 0;
      top = top_of_helpt;

      for (; ; ) {
	 mid = (bot + top) / 2;
	 minlen = strlen(argument);

	 if (!(chk = strn_cmp(argument, help_index[mid].keyword, minlen))) {

         /* trace backwards to find first matching entry. Thanks Jeff Fink! */
	    while ((mid > 0) &&
	       (!(chk = strn_cmp(argument, help_index[mid-1].keyword, minlen))))
	          mid--;
	    fseek(help_fl, help_index[mid].pos, SEEK_SET);
	    *buf2 = '\0';
	    for (; ; ) {
	       fgets(buf, 80, help_fl);
	       if (*buf == '#')
		  break;
	       strcat(buf2, buf);
	       strcat(buf2, "\r");
	    }
	    page_string(ch->desc, buf2, 1);
	    return;
	 } else if (bot >= top) {
	    send_to_char("There is no help on that word.\n\r", ch);
	    return;
	 } else if (chk > 0)
	    bot = ++mid;
	 else
	    top = --mid;
      }
      return;
   }

   send_to_char(help, ch);
}



#define WHO_FORMAT \
"format: who [minlev[-maxlev]] [-n name] [-c classlist] [-s] [-o] [-q] [-r] [-z]\n\r"

ACMD(do_who)
{
   struct descriptor_data *d;
   struct char_data *tch;
   char	name_search[80];
   char	mode;
   int	low = 0, high = LEVEL_IMPL, i, localwho = 0, questwho = 0;
   int	showclass = 0, short_list = 0, outlaws = 0, num_can_see = 0;
   int	who_room = 0;

   /* skip spaces */
   for (i = 0; *(argument + i) == ' '; i++)
      ;
   strcpy(buf, (argument + i));
   name_search[0] = '\0';

   while (*buf) {
      half_chop(buf, arg, buf1);
      if (isdigit(*arg)) {
	 sscanf(arg, "%d-%d", &low, &high);
	 strcpy(buf, buf1);
      } else if (*arg == '-') {
	 mode = *(arg + 1); /* just in case; we destroy arg in the switch */
	 switch (mode) {
	 case 'o':
	 case 'k':
	    outlaws = 1;
	    strcpy(buf, buf1);
	    break;
	 case 'z':
	    localwho = 1;
	    strcpy(buf, buf1);
	    break;
	 case 's':
	    short_list = 1;
	    strcpy(buf, buf1);
	    break;
	 case 'q':
	    questwho = 1;
	    strcpy(buf, buf1);
	    break;
	 case 'l':
	    half_chop(buf1, arg, buf);
	    sscanf(arg, "%d-%d", &low, &high);
	    break;
	 case 'n':
	    half_chop(buf1, name_search, buf);
	    break;
	 case 'r':
	    who_room = 1;
	    strcpy(buf, buf1);
	    break;
	 case 'c':
	    half_chop(buf1, arg, buf);
	    for (i = 0; i < strlen(arg); i++) {
	       switch (tolower(arg[i])) {
	       case 'm': showclass = showclass |   1; break;
	       case 'c': showclass = showclass |   2; break;
	       case 't': showclass = showclass |   4; break;
	       case 'w': showclass = showclass |   8; break;
	       }
	    }
	    break;
	 default:
	    send_to_char(WHO_FORMAT, ch);
	    return;
	    break;
	 } /* end of switch */

      } else { /* endif */
	 send_to_char(WHO_FORMAT, ch);
	 return;
      }
   } /* end while (parser) */

   send_to_char("Players\n\r-------\n\r", ch);

   for (d = descriptor_list; d; d = d->next) {
      if (d->connected)
	 continue;

      if (d->original)
	 tch = d->original;
      else if (!(tch = d->character))
	 continue;

      if (*name_search && str_cmp(GET_NAME(tch), name_search) && 
          !strstr(GET_TITLE(tch), name_search))
	 continue;
      if (!CAN_SEE(ch, tch) || GET_LEVEL(tch) < low || GET_LEVEL(tch) > high)
	 continue;
      if (outlaws && !PLR_FLAGGED(tch, PLR_KILLER) && 
          !PLR_FLAGGED(tch, PLR_THIEF))
	 continue;
      if (questwho && !PRF_FLAGGED(tch, PRF_QUEST))
	 continue;
      if (localwho && world[ch->in_room].zone != world[tch->in_room].zone)
	 continue;
      if (who_room && (tch->in_room != ch->in_room))
	 continue;
      if (showclass && !(showclass & (1 << (GET_CLASS(tch) - 1))))
	 continue;
      if (short_list) {
	 sprintf(buf, "%s[%2d %s] %-12.12s%s%s",
	     (GET_LEVEL(tch) >= LEVEL_IMMORT ? CCYEL(ch, C_SPR) : ""),
	     GET_LEVEL(tch), CLASS_ABBR(tch), GET_NAME(tch),
	     (GET_LEVEL(tch) >= LEVEL_IMMORT ? CCNRM(ch, C_SPR) : ""),
	     ((!(++num_can_see % 4)) ? "\n\r" : ""));
	 send_to_char(buf, ch);
      } else {
	 num_can_see++;
	 sprintf(buf, "%s[%2d %s] %s %s",
	     (GET_LEVEL(tch) >= LEVEL_IMMORT ? CCYEL(ch, C_SPR) : ""),
	     GET_LEVEL(tch), CLASS_ABBR(tch), GET_NAME(tch),
	     GET_TITLE(tch));

	 if (GET_INVIS_LEV(tch))
	    sprintf(buf, "%s (i%d)", buf, GET_INVIS_LEV(tch));
	 else if (IS_AFFECTED(tch, AFF_INVISIBLE))
	    strcat(buf, " (invis)");

	 if (PLR_FLAGGED(tch, PLR_MAILING))
	    strcat(buf, " (mailing)");
	 else if (PLR_FLAGGED(tch, PLR_WRITING))
	    strcat(buf, " (writing)");

	 if (PRF_FLAGGED(tch, PRF_DEAF))
	    strcat(buf, " (deaf)");
	 if (PRF_FLAGGED(tch, PRF_NOTELL))
	    strcat(buf, " (notell)");
	 if (PRF_FLAGGED(tch, PRF_QUEST))
	    strcat(buf, " (quest)");
	 if (PLR_FLAGGED(tch, PLR_THIEF))
	    strcat(buf, " (THIEF)");
	 if (PLR_FLAGGED(tch, PLR_KILLER))
	    strcat(buf, " (KILLER)");
	 if (GET_LEVEL(tch) >= LEVEL_IMMORT)
	    strcat(buf, CCNRM(ch, C_SPR));
	 strcat(buf, "\n\r");
	 send_to_char(buf, ch);
      } /* endif shortlist */
   } /* end of for */
   if (short_list && (num_can_see % 4))
      send_to_char("\n\r", ch);
   sprintf(buf, "\n\r%d characters displayed.\n\r", num_can_see);
   send_to_char(buf, ch);
}


#define USERS_FORMAT \
"format: users [-l minlevel[-maxlevel]] [-n name] [-h host] [-c classlist] [-o] [-p]\n\r"

ACMD(do_users)
{
   extern char	*connected_types[];
   char	line[200], line2[220], idletime[10], classname[20];
   char	state[30], *timeptr;

   struct char_data *tch;
   char	name_search[80];
   char	host_search[80];
   char	mode, *format;
   int	low = 0, high = LEVEL_IMPL, i;
   int	showclass = 0, outlaws = 0, num_can_see = 0, playing = 0, deadweight = 0;

   struct descriptor_data *d;

   name_search[0] = '\0';
   host_search[0] = '\0';

   strcpy(buf, argument);
   while (*buf) {
      half_chop(buf, arg, buf1);
      if (*arg == '-') {
	 mode = *(arg + 1); /* just in case; we destroy arg in the switch */
	 switch (mode) {
	 case 'o':
	 case 'k':
	    outlaws = 1;
	    playing = 1;
	    strcpy(buf, buf1);
	    break;
	 case 'p':
	    playing = 1;
	    strcpy(buf, buf1);
	    break;
	 case 'd':
	    deadweight = 1;
	    strcpy(buf, buf1);
	    break;
	 case 'l':
	    playing = 1;
	    half_chop(buf1, arg, buf);
	    sscanf(arg, "%d-%d", &low, &high);
	    break;
	 case 'n':
	    playing = 1;
	    half_chop(buf1, name_search, buf);
	    break;
	 case 'h':
	    playing = 1;
	    half_chop(buf1, host_search, buf);
	    break;
	 case 'c':
	    playing = 1;
	    half_chop(buf1, arg, buf);
	    for (i = 0; i < strlen(arg); i++) {
	       switch (tolower(arg[i])) {
	       case 'm': showclass = showclass |   1; break;
	       case 'c': showclass = showclass |   2; break;
	       case 't': showclass = showclass |   4; break;
	       case 'w': showclass = showclass |   8; break;
	       }
	    }
	    break;
	 default:
	    send_to_char(USERS_FORMAT, ch);
	    return;
	    break;
	 } /* end of switch */

      } else { /* endif */
	 send_to_char(USERS_FORMAT, ch);
	 return;
      }
   } /* end while (parser) */
   strcpy(line,
       "Num Class   Name         State          Idl Login@   Site\n\r");
   strcat(line,
       "--- ------- ------------ -------------- --- -------- ------------------------\n\r");
   send_to_char(line, ch);

   one_argument(argument, arg);

   for (d = descriptor_list; d; d = d->next) {
      if (d->connected && playing)
	 continue;
      if (!d->connected && deadweight)
	 continue;
      if (!d->connected) {
	 if (d->original)
	    tch = d->original;
	 else if (!(tch = d->character))
	    continue;

	 if (*host_search && !strstr(d->host, host_search))
	    continue;
	 if (*name_search && str_cmp(GET_NAME(tch), name_search) && 
	     !strstr(GET_TITLE(tch), name_search))
	    continue;
	 if (!CAN_SEE(ch, tch) || GET_LEVEL(tch) < low || GET_LEVEL(tch) > high)
	    continue;
	 if (outlaws && !PLR_FLAGGED(tch, PLR_KILLER) && 
	     !PLR_FLAGGED(tch, PLR_THIEF))
	    continue;
	 if (showclass && !(showclass & (1 << (GET_CLASS(tch) - 1))))
	    continue;
	 if (GET_INVIS_LEV(ch) > GET_LEVEL(ch))
	    continue;

	 if (d->original)
	    sprintf(classname, "[%2d %s]", GET_LEVEL(d->original),
	        CLASS_ABBR(d->original));
	 else
	    sprintf(classname, "[%2d %s]", GET_LEVEL(d->character),
	        CLASS_ABBR(d->character));
      } else
	 strcpy(classname, "   -   ");

      timeptr = asctime(localtime(&d->login_time));
      timeptr += 11;
      *(timeptr + 8) = '\0';

      if (!d->connected && d->original)
	 strcpy(state, "Switched");
      else
	 strcpy(state, connected_types[d->connected]);

      if (d->character && !d->connected && GET_LEVEL(d->character) < LEVEL_GOD)
	 sprintf(idletime, "%3d",
	     d->character->specials.timer * SECS_PER_MUD_HOUR / SECS_PER_REAL_MIN);
      else
	 strcpy(idletime, "");

      format = "%3d %-7s %-12s %-14s %-3s %-8s ";

      if (d->character && d->character->player.name) {
	 if (d->original)
	    sprintf(line, format, d->desc_num, classname,
		    d->original->player.name, state, idletime, timeptr);
	 else
	    sprintf(line, format, d->desc_num, classname,
		    d->character->player.name, state, idletime, timeptr);
      } else
	 sprintf(line, format, d->desc_num, "   -   ", "UNDEFINED",
		 state, idletime, timeptr);

      if (d->host && *d->host)
	 sprintf(line + strlen(line), "[%s]\n\r", d->host);
      else
	 strcat(line, "[Hostname unknown]\n\r");

      if (d->connected) {
	 sprintf(line2, "%s%s%s", CCGRN(ch, C_SPR), line, CCNRM(ch, C_SPR));
	 strcpy(line, line2);
      }

      if (d->connected || (!d->connected && CAN_SEE(ch, d->character))) {
	 send_to_char(line, ch);
	 num_can_see++;
      }
   }

   sprintf(line, "\n\r%d visible sockets connected.\n\r", num_can_see);
   send_to_char(line, ch);
}



ACMD(do_inventory)
{
   send_to_char("You are carrying:\n\r", ch);
   list_obj_to_char(ch->carrying, ch, 1, TRUE);
}


ACMD(do_equipment)
{
   int	j;
   bool found;

   send_to_char("You are using:\n\r", ch);
   found = FALSE;
   for (j = 0; j < MAX_WEAR; j++) {
      if (ch->equipment[j]) {
	 if (CAN_SEE_OBJ(ch, ch->equipment[j])) {
	    send_to_char(where[j], ch);
	    show_obj_to_char(ch->equipment[j], ch, 1);
	    found = TRUE;
	 } else {
	    send_to_char(where[j], ch);
	    send_to_char("Something.\n\r", ch);
	    found = TRUE;
	 }
      }
   }
   if (!found) {
      send_to_char(" Nothing.\n\r", ch);
   }
}


ACMD(do_gen_ps)
{
   extern char	circlemud_version[];

   switch (subcmd) {
   case SCMD_CREDITS : page_string(ch->desc, credits, 0); break;
   case SCMD_NEWS    : page_string(ch->desc, news, 0); break;
   case SCMD_INFO    : page_string(ch->desc, info, 0); break;
   case SCMD_WIZLIST : page_string(ch->desc, wizlist, 0); break;
   case SCMD_IMMLIST : page_string(ch->desc, immlist, 0); break;
   case SCMD_HANDBOOK: page_string(ch->desc, handbook, 0); break;
   case SCMD_POLICIES: page_string(ch->desc, policies, 0); break;
   case SCMD_CLEAR   : send_to_char("\033[H\033[J", ch); break;
   case SCMD_VERSION : send_to_char(circlemud_version, ch); break;
   case SCMD_WHOAMI  : send_to_char(strcat(strcpy(buf, GET_NAME(ch)), "\n\r"), ch); break;
   default: return; break;
   }
}


void perform_mortal_where(struct char_data *ch, char *arg)
{
   register struct char_data *i;
   register struct descriptor_data *d;

   if (!*arg) {
      send_to_char("Players in your Zone\n\r--------------------\n\r", ch);
      for (d = descriptor_list; d; d = d->next)
	 if (!d->connected) {
	    i = (d->original ? d->original : d->character);
            if (i && CAN_SEE(ch, i) && (i->in_room != NOWHERE) &&
	     (world[ch->in_room].zone == world[i->in_room].zone)) {
		sprintf(buf, "%-20s - %s\n\r", GET_NAME(i), world[i->in_room].name);
		send_to_char(buf, ch);
	    }
	 }
   } else { /* print only FIRST char, not all. */
      for (i = character_list; i; i = i->next)
         if (world[i->in_room].zone == world[ch->in_room].zone && CAN_SEE(ch, i) &&
	     (i->in_room != NOWHERE) && isname(arg, i->player.name)) {
	    sprintf(buf, "%-25s - %s\n\r", GET_NAME(i), world[i->in_room].name);
	    send_to_char(buf, ch);
	    return;
	 }
      send_to_char("No-one around by that name.\n\r", ch);
   }
}


void perform_immort_where(struct char_data *ch, char *arg)
{
   register struct char_data *i;
   register struct obj_data *k;
   struct descriptor_data *d;
   int	num = 0, found = 0;

   if (!*arg) {
      send_to_char("Players\n\r-------\n\r", ch);
      for (d = descriptor_list; d; d = d->next)
	 if (!d->connected) {
	    i = (d->original ? d->original : d->character);
            if (i && CAN_SEE(ch, i) && (i->in_room != NOWHERE)) {
	       if (d->original)
		  sprintf(buf, "%-20s - [%5d] %s (in %s)\n\r",
		     GET_NAME(i), world[d->character->in_room].number,
		     world[d->character->in_room].name, GET_NAME(d->character));
	       else
		  sprintf(buf, "%-20s - [%5d] %s\n\r", GET_NAME(i),
		     world[i->in_room].number, world[i->in_room].name);
	       send_to_char(buf, ch);
	    }
	 }
   } else {
      for (i = character_list; i; i = i->next)
         if (CAN_SEE(ch, i) && i->in_room != NOWHERE && isname(arg, i->player.name)) {
	    found = 1;
	    sprintf(buf, "%3d. %-25s - [%5d] %s\n\r", ++num, GET_NAME(i),
	       world[i->in_room].number, world[i->in_room].name);
	    send_to_char(buf, ch);
	 }

      for (num = 0, k = object_list; k; k = k->next)
	 if (CAN_SEE_OBJ(ch, k) && (k->in_room != NOWHERE) && isname(arg, k->name)) {
	    found = 1;
	    sprintf(buf, "%3d. %-25s - [%5d] %s\n\r", ++num,
	       k->short_description, world[k->in_room].number,
	       world[k->in_room].name);
	    send_to_char(buf, ch);
	 }

      if (!found)
         send_to_char("Couldn't find any such thing.\n\r", ch);
   }
}
   


ACMD(do_where)
{
   one_argument(argument, arg);

   if (GET_LEVEL(ch) >= LEVEL_IMMORT)
      perform_immort_where(ch, arg);
   else
      perform_mortal_where(ch, arg);
}



ACMD(do_levels)
{
   int	i;

   /* extern const struct title_type titles[4][25]; */

   if (IS_NPC(ch)) {
      send_to_char("You ain't nothin' but a hound-dog.\n\r", ch);
      return;
   }

   *buf = '\0';

   for (i = 1; i < LEVEL_IMMORT; i++) {
      sprintf(buf + strlen(buf), "[%2d] %8d-%-8d : ",
          i, titles[GET_CLASS(ch) - 1][i].exp,
          titles[GET_CLASS(ch) - 1][i + 1].exp);
      switch (GET_SEX(ch)) {
      case SEX_MALE:
	 strcat(buf, titles[GET_CLASS(ch) - 1][i].title_m);
	 break;
      case SEX_FEMALE:
	 strcat(buf, titles[GET_CLASS(ch) - 1][i].title_f);
	 break;
      default:
	 send_to_char("Oh dear.\n\r", ch);
	 break;
      }
      strcat(buf, "\n\r");
   }
   send_to_char(buf, ch);
}



ACMD(do_consider)
{
   struct char_data *victim;
   int	diff;

   one_argument(argument, buf);

   if (!(victim = get_char_room_vis(ch, buf))) {
      send_to_char("Consider killing who?\n\r", ch);
      return;
   }

   if (victim == ch) {
      send_to_char("Easy!  Very easy indeed!\n\r", ch);
      return;
   }

   if (!IS_NPC(victim)) {
      send_to_char("Would you like to borrow a cross and a shovel?\n\r", ch);
      return;
   }

   diff = (GET_LEVEL(victim) - GET_LEVEL(ch));

   if (diff <= -10)
      send_to_char("Now where did that chicken go?\n\r", ch);
   else if (diff <= -5)
      send_to_char("You could do it with a needle!\n\r", ch);
   else if (diff <= -2)
      send_to_char("Easy.\n\r", ch);
   else if (diff <= -1)
      send_to_char("Fairly easy.\n\r", ch);
   else if (diff == 0)
      send_to_char("The perfect match!\n\r", ch);
   else if (diff <= 1)
      send_to_char("You would need some luck!\n\r", ch);
   else if (diff <= 2)
      send_to_char("You would need a lot of luck!\n\r", ch);
   else if (diff <= 3)
      send_to_char("You would need a lot of luck and great equipment!\n\r", ch);
   else if (diff <= 5)
      send_to_char("Do you feel lucky, punk?\n\r", ch);
   else if (diff <= 10)
      send_to_char("Are you mad!?\n\r", ch);
   else if (diff <= 100)
      send_to_char("You ARE mad!\n\r", ch);

}


ACMD(do_toggle)
{
   if (IS_NPC(ch))
      return;
   if (WIMP_LEVEL(ch) == 0)
      strcpy(buf2, "OFF");
   else
      sprintf(buf2, "%-3d", WIMP_LEVEL(ch));

   sprintf(buf,
       "Hit Pnt Display: %-3s    "
       "     Brief Mode: %-3s    "
       " Summon Protect: %-3s\n\r"
       " Move Pnt Disp.: %-3s    "
       "   Compact Mode: %-3s    "
       "       On Quest: %-3s\n\r"
       "   Mana Display: %-3s    "
       "         NoTell: %-3s    "
       "   Repeat Comm.: %-3s\n\r"
       "   Auto Display: %-3s    "
       "           Deaf: %-3s	"
       "     Wimp Level: %-3s\n\r",

       ONOFF(PRF_FLAGGED(ch, PRF_DISPHP)),
       ONOFF(PRF_FLAGGED(ch, PRF_BRIEF)),
       ONOFF(!PRF_FLAGGED(ch, PRF_SUMMONABLE)),
       ONOFF(PRF_FLAGGED(ch, PRF_DISPMOVE)),
       ONOFF(PRF_FLAGGED(ch, PRF_COMPACT)),
       YESNO(PRF_FLAGGED(ch, PRF_QUEST)),
       ONOFF(PRF_FLAGGED(ch, PRF_DISPMANA)),
       ONOFF(PRF_FLAGGED(ch, PRF_NOTELL)),
       YESNO(!PRF_FLAGGED(ch, PRF_NOREPEAT)),
       ONOFF(PRF_FLAGGED(ch, PRF_DISPAUTO)),
       YESNO(PRF_FLAGGED(ch, PRF_DEAF)),
       buf2);

   send_to_char(buf, ch);

}


void	sort_commands(void)
{
   int	a, b, tmp;

   ACMD(do_action);

   num_of_cmds = 1;

   while (num_of_cmds < MAX_CMD_LIST && 
       cmd_info[num_of_cmds].command_pointer) {
      cmd_info[num_of_cmds].sort_pos = num_of_cmds - 1;
      cmd_info[num_of_cmds].is_social = 
          (cmd_info[num_of_cmds].command_pointer == do_action);
      num_of_cmds++;
   }

   num_of_cmds--;
   cmd_info[33].is_social = 1; /* do_insult */

   for (a = 1; a <= num_of_cmds - 1; a++)
      for (b = a + 1; b <= num_of_cmds; b++)
	 if (strcmp(command[cmd_info[a].sort_pos],
	     command[cmd_info[b].sort_pos]) > 0) {
	    tmp = cmd_info[a].sort_pos;
	    cmd_info[a].sort_pos = cmd_info[b].sort_pos;
	    cmd_info[b].sort_pos = tmp;
	 }
}



ACMD(do_commands)
{
   int	no, i, cmd_num;
   int	wizhelp = 0, socials = 0;
   struct char_data *vict;

   one_argument(argument, buf);

   if (*buf) {
      if (!(vict = get_char_vis(ch, buf)) || IS_NPC(vict)) {
	 send_to_char("Who is that?\n\r", ch);
	 return;
      }
      if (GET_LEVEL(ch) < GET_LEVEL(vict)) {
	 send_to_char("Can't determine commands of people above your level.\n\r", ch);
	 return;
      }
   } else
      vict = ch;

   if (subcmd == SCMD_SOCIALS)
      socials = 1;
   else if (subcmd == SCMD_WIZHELP)
      wizhelp = 1;

   sprintf(buf, "The following %s%s are available to %s:\n\r",
       wizhelp ? "privileged " : "",
       socials ? "socials" : "commands",
       vict == ch ? "you" : GET_NAME(vict));

   for (no = 1, cmd_num = 1; cmd_num <= num_of_cmds; cmd_num++) {
      i = cmd_info[cmd_num].sort_pos;
      if (cmd_info[i+1].minimum_level >= 0 && 
          (cmd_info[i+1].minimum_level >= LEVEL_IMMORT) == wizhelp && 
          GET_LEVEL(vict) >= cmd_info[i+1].minimum_level && 
          (wizhelp || socials == cmd_info[i+1].is_social)) {
	 sprintf(buf + strlen(buf), "%-11s", command[i]);
	 if (!(no % 7))
	    strcat(buf, "\n\r");
	 no++;
      }
   }

   strcat(buf, "\n\r");
   send_to_char(buf, ch);
}



ACMD(do_diagnose)
{
   struct char_data *vict;

   one_argument(argument, buf);

   if (*buf) {
      if (!(vict = get_char_room_vis(ch, buf))) {
	 send_to_char("No-one by that name here.\n\r", ch);
	 return;
      } else
	 diag_char_to_char(vict, ch);
   } else {
      if (ch->specials.fighting)
	 diag_char_to_char(ch->specials.fighting, ch);
      else
	 send_to_char("Diagnose who?\n\r", ch);
   }
}


