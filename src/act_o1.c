/***************************************************************************
 *  file: act_o1.c , Implementation of commands.           Part of DIKUMUD *
 *  Usage : Commands mainly moving around objects.                         *
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
#include <string.h>
#include <stdlib.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "utils.h"
#include "interp.h"
#include "handler.h"
#include "db.h"
#include "spells.h"

/* extern variables */

extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern struct room_data *world;
extern struct str_app_type str_app[];
	 
/* extern functions */
char *fname(char *namelist);
int isname(char *arg, char *arg2);
struct obj_data *create_money( int amount );
void do_put(struct char_data *ch, char *argument, int cmd);

/* no looting code */
bool can_loot(struct char_data *ch, struct obj_data *obj)
{
  if (!IS_NPC(ch) && GET_LEVEL(ch) > 31)
    return TRUE;
  if (!obj->owner || obj->owner == NULL)
    return TRUE;
  if (obj->owner == ch)
    return TRUE;
  if (!IS_NPC(obj->owner) && IS_SET(obj->owner->specials.act,PLR_CANLOOT))
    return TRUE;
  
  return FALSE;
}

void do_noloot(struct char_data *ch, char *argument, int cmd)
{
  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->specials.act,PLR_CANLOOT))
  {
    send_to_char("Your corpse is now protected from thievery.\n\r",ch);
    REMOVE_BIT(ch->specials.act,PLR_CANLOOT);
  }
  else
  {
    send_to_char("Your corpse is no longer protected.\n\r",ch);
    SET_BIT(ch->specials.act,PLR_CANLOOT);
  }
}
 

/* procedures related to get */
void get(struct char_data *ch, struct obj_data *obj_object, 
    struct obj_data *sub_object)
{
    char buffer[SHORT_STRING_LENGTH];
    int gold;

    if (sub_object) {
	obj_from_obj(obj_object);
	obj_to_char(obj_object, ch);
	if (sub_object->carried_by == ch) {
	    act("You get $p from $P.", 0, ch, obj_object, sub_object,
		TO_CHAR);
	    act("$n gets $p from $P.", 1, ch, obj_object, sub_object,
		TO_ROOM);
	} else {
	    act("You get $p from $P.", 0, ch, obj_object, sub_object,
		TO_CHAR);

	    act("$n gets $p from $P.", 1, ch, obj_object, sub_object, TO_ROOM);
	}
    } else {
	obj_from_room(obj_object);
	obj_to_char(obj_object, ch);
	act("You get $p.", 0, ch, obj_object, 0, TO_CHAR);
	act("$n gets $p.", 1, ch, obj_object, 0, TO_ROOM);
    }
    if((obj_object->obj_flags.type_flag == ITEM_MONEY) && 
	(obj_object->obj_flags.value[0]>=1))
    {
	obj_from_char(obj_object);
        gold = obj_object->obj_flags.value[0];
	sprintf(buffer,"There were %d coins.\n\r",gold);
	send_to_char(buffer,ch);
	GET_GOLD(ch) += gold; 
	extract_obj(obj_object);
        if (!IS_NPC(ch) && IS_SET(ch->specials.act,PLR_AUTOSPLIT) &&
	    IS_AFFECTED(ch,AFF_GROUP))
        {
          sprintf(buffer,"%d",gold);
	  do_split(ch,buffer,0);
        }
    }
}


void do_get(struct char_data *ch, char *argument, int cmd)
{
    char arg1[SHORT_STRING_LENGTH];
    char arg2[SHORT_STRING_LENGTH];
    char buffer[SHORT_STRING_LENGTH];
    struct obj_data *sub_object;
    struct obj_data *obj_object;
    struct obj_data *next_obj;
    bool found = FALSE;
    bool fail  = FALSE;
    int type   = 3;
    bool alldot = FALSE;
    char allbuf[SHORT_STRING_LENGTH];

    argument_interpreter(argument, arg1, arg2);

    /* get type */
    if (!*arg1) {
	type = 0;
    }
    if (*arg1 && !*arg2) {
      alldot = FALSE;
      allbuf[0] = '\0';
      if ((str_cmp(arg1, "all") != 0) &&
	  (sscanf(arg1, "all.%s", allbuf) != 0)){
	strcpy(arg1, "all");
	alldot = TRUE;
      }
      if (!str_cmp(arg1,"all")) {
	type = 1;
      } else {
	type = 2;
      }
    }
    if (*arg1 && *arg2) {
      alldot = FALSE;
      allbuf[0] = '\0';
      if ((str_cmp(arg1, "all") != 0) &&
	      (sscanf(arg1, "all.%s", allbuf) != 0)){
	    strcpy(arg1, "all");
	    alldot = TRUE;
	  }
      if (!str_cmp(arg1,"all")) {
	if (!str_cmp(arg2,"all")) {
	  type = 3;
	} else {
	  type = 4;
	}
      } else {
	if (!str_cmp(arg2,"all")) {
	  type = 5;
	} else {
	  type = 6;
	}
      }
    }

    switch (type) {
	/* get */
	case 0:{ 
	    send_to_char("Get what?\n\r", ch); 
	} break;
	/* get all */
	case 1:{ 
	    sub_object = 0;
	    found = FALSE;
	    fail    = FALSE;
	    for(obj_object = world[ch->in_room].contents;
		obj_object;
		obj_object = next_obj) {
		next_obj = obj_object->next_content;

		/* IF all.obj, only get those named "obj" */
		if (alldot && !isname(allbuf,obj_object->name)) {
		  continue;
		}
		
		if (CAN_SEE_OBJ(ch,obj_object)) {
		    if ((IS_CARRYING_N(ch) + 1) <= CAN_CARRY_N(ch)) {
			if ((IS_CARRYING_W(ch) + obj_object->obj_flags.weight)
				<= CAN_CARRY_W(ch)) {
			    if (CAN_WEAR(obj_object,ITEM_TAKE) &&
				can_loot(ch,obj_object)) {
				get(ch,obj_object,sub_object);
				found = TRUE;
			    } else {
				send_to_char("You can't take that\n\r", ch);
				fail = TRUE;
			    }
			} else {
			    sprintf(buffer,
				"%s : You can't carry that much weight.\n\r", 
				fname(obj_object->name));
			    send_to_char(buffer, ch);
			    fail = TRUE;
			}
		    } else {
			sprintf(buffer,
				"%s : You can't carry that many items.\n\r", 
			    fname(obj_object->name));
			send_to_char(buffer, ch);
			fail = TRUE;
		    }
		}
	    }
	    if (found) {
		send_to_char("OK.\n\r", ch);
	    } else {
		if (!fail) send_to_char("You see nothing here.\n\r", ch);
	    }
	} break;
	/* get ??? */
	case 2:{
	    sub_object = 0;
	    found = FALSE;
	    fail    = FALSE;
	    obj_object = get_obj_in_list_vis(ch, arg1, 
		world[ch->in_room].contents);
	    if (obj_object) {
		if ((IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))) {
		    if ((IS_CARRYING_W(ch) + obj_object->obj_flags.weight) < 
			CAN_CARRY_W(ch)) {
			if (CAN_WEAR(obj_object,ITEM_TAKE) &&
			    can_loot(ch,obj_object)) {
			    get(ch,obj_object,sub_object);
			    found = TRUE;
			} else {
			    send_to_char("You can't take that\n\r", ch);
			    fail = TRUE;
			}
		    } else {
			sprintf(buffer,
				"%s : You can't carry that much weight.\n\r", 
			    fname(obj_object->name));
			send_to_char(buffer, ch);
			fail = TRUE;
		    }
		} else {
		    sprintf(buffer,
			"%s : You can't carry that many items.\n\r", 
			fname(obj_object->name));
		    send_to_char(buffer, ch);
		    fail = TRUE;
		}
	    } else {
		sprintf(buffer,"You do not see a %s here.\n\r", arg1);
		send_to_char(buffer, ch);
		fail = TRUE;
	    }
	} break;
	/* get all all */
	case 3:{ 
	    send_to_char("You must be joking?!\n\r", ch);
	} break;
	/* get all ??? */
	case 4:{
	    found = FALSE;
	    fail    = FALSE; 
	    sub_object = get_obj_in_list_vis(ch, arg2, 
		world[ch->in_room].contents);
	    if (!sub_object){
		sub_object = get_obj_in_list_vis(ch, arg2, ch->carrying);
	    }
	    if (sub_object) {
		if (GET_ITEM_TYPE(sub_object) == ITEM_CONTAINER) {
		  if (IS_SET(sub_object->obj_flags.value[1], CONT_CLOSED)){
		    sprintf(buffer,
			"The %s is closed.\n\r",fname(sub_object->name));
		    send_to_char(buffer, ch);
		    return;
		  }
		  if (!can_loot(ch,sub_object))
		  {
		    send_to_char("Corpse looting is not permitted.\n\r",ch);
		    return;
		  }
		  for(obj_object = sub_object->contains;
		      obj_object;
		      obj_object = next_obj) {
		    next_obj = obj_object->next_content;

		    /* IF all.obj, only get those named "obj" */
		    if (alldot && !isname(allbuf,obj_object->name)){
		      continue;
		    }
		    if (CAN_SEE_OBJ(ch,obj_object)) {
		      if ((IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))) {
		    if ((IS_CARRYING_W(ch) + obj_object->obj_flags.weight) < 
			CAN_CARRY_W(ch)) {
		      if (CAN_WEAR(obj_object,ITEM_TAKE)) {
			get(ch,obj_object,sub_object);
			found = TRUE;
		      } else {
			send_to_char("You can't take that\n\r", ch);
			fail = TRUE;
		      }
		    } else {
		      sprintf(buffer,
		      "%s : You can't carry that much weight.\n\r", 
			  fname(obj_object->name));
		      send_to_char(buffer, ch);
		      fail = TRUE;
		    }
		      } else {
		    sprintf(buffer,"%s : You can't carry that many items.\n\r", 
			fname(obj_object->name));
		    send_to_char(buffer, ch);
		    fail = TRUE;
		      }
		    }
		  }
		  if (!found && !fail) {
		    sprintf(buffer,"You do not see anything in the %s.\n\r", 
			fname(sub_object->name));
		    send_to_char(buffer, ch);
		    fail = TRUE;
		  }
		} else {
		  sprintf(buffer,"The %s is not a container.\n\r",
		      fname(sub_object->name));
		  send_to_char(buffer, ch);
		  fail = TRUE;
		}
		  } else { 
		sprintf(buffer,"You do not see or have the %s.\n\r", arg2);
		send_to_char(buffer, ch);
		fail = TRUE;
		  }
	      } break;
	case 5:{ 
	  send_to_char(
	    "You can't take a thing from more than one container.\n\r", ch);
	} break;
	case 6:{
	  found = FALSE;
	  fail  = FALSE;
	  sub_object = get_obj_in_list_vis(ch, arg2, 
			   world[ch->in_room].contents);
	  if (!sub_object){
	    sub_object = get_obj_in_list_vis(ch, arg2, ch->carrying);
	  }
	  if (sub_object) {
	    if (GET_ITEM_TYPE(sub_object) == ITEM_CONTAINER) {
	      if (IS_SET(sub_object->obj_flags.value[1], CONT_CLOSED)){
	    sprintf(buffer,"The %s is closed.\n\r", fname(sub_object->name));
	    send_to_char(buffer, ch);
	    return;
	      }
                  if (!can_loot(ch,sub_object))
                  {
                    send_to_char("Corpse looting is not permitted.\n\r",ch);
                    return;
                  }

	      obj_object = get_obj_in_list_vis(ch, arg1, sub_object->contains);
	      if (obj_object) {
	    if ((IS_CARRYING_N(ch) + 1 < CAN_CARRY_N(ch))) {
	      if ((IS_CARRYING_W(ch) + obj_object->obj_flags.weight) < 
		  CAN_CARRY_W(ch)) {
		if (CAN_WEAR(obj_object,ITEM_TAKE)) {
		  get(ch,obj_object,sub_object);
		  found = TRUE;
		} else {
		  send_to_char("You can't take that\n\r", ch);
		  fail = TRUE;
		}
	      } else {
		sprintf(buffer,"%s : You can't carry that much weight.\n\r", 
		    fname(obj_object->name));
		send_to_char(buffer, ch);
		fail = TRUE;
	      }
	    } else {
	      sprintf(buffer,"%s : You can't carry that many items.\n\r", 
		  fname(obj_object->name));
	      send_to_char(buffer, ch);
	      fail = TRUE;
	    }
	      } else {
	    sprintf(buffer,"The %s does not contain the %s.\n\r", 
		fname(sub_object->name), arg1);
	    send_to_char(buffer, ch);
	    fail = TRUE;
	      }
	    } else {
	      sprintf(buffer,
	      "The %s is not a container.\n\r", fname(sub_object->name));
	      send_to_char(buffer, ch);
	      fail = TRUE;
	    }
	  } else {
	    sprintf(buffer,"You do not see or have the %s.\n\r", arg2);
	    send_to_char(buffer, ch);
	    fail = TRUE;
	  }
	} break;
	}
      }


void do_drop(struct char_data *ch, char *argument, int cmd)
{
    char arg[SHORT_STRING_LENGTH];
    int amount;
    char buffer[SHORT_STRING_LENGTH];
    struct obj_data *tmp_object;
    struct obj_data *next_obj;
    bool test = FALSE;

    argument=one_argument(argument, arg);
    if(is_number(arg))
    {
      if(strlen(arg)>7){
	send_to_char("Number field too big.\n\r", ch);
	return;
      }
      amount = atoi(arg);
      argument=one_argument(argument,arg);
      if (str_cmp("coins",arg) && str_cmp("coin",arg) &&
         str_cmp("gold",arg))
	{
	  send_to_char("Sorry, you can't do that (yet)...\n\r",ch);
	  return;
	}
      if(amount<0)
	{
	  send_to_char("Sorry, you can't do that!\n\r",ch);
	  return;
	}
      if(GET_GOLD(ch)<amount)
	{
	  send_to_char("You haven't got that many coins!\n\r",ch);
	  return;
	}
      send_to_char("OK.\n\r",ch);
      if(amount==0)
	return;
	
      act("$n drops some gold.", FALSE, ch, 0, 0, TO_ROOM);
      tmp_object = create_money(amount);
      obj_to_room(tmp_object,ch->in_room);
      GET_GOLD(ch)-=amount;
      return;
    }

    if (*arg) {
	if (!str_cmp(arg,"all")) {
	    for(tmp_object = ch->carrying;
		tmp_object;
		tmp_object = next_obj) {
		next_obj = tmp_object->next_content;
		if (! IS_SET(tmp_object->obj_flags.extra_flags, ITEM_NODROP)) {
		    if (CAN_SEE_OBJ(ch, tmp_object)) {
			sprintf(buffer,
			    "You drop the %s.\n\r", fname(tmp_object->name));
			send_to_char(buffer, ch);
		    } else {
			send_to_char("You drop something.\n\r", ch);
		    }
		    act("$n drops $p.", 1, ch, tmp_object, 0, TO_ROOM);
		    obj_from_char(tmp_object);
		    obj_to_room(tmp_object,ch->in_room);
		    test = TRUE;
		} else {
		    if (CAN_SEE_OBJ(ch, tmp_object)) {
			sprintf(buffer,
			    "You can't drop the %s, it must be CURSED!\n\r",
			    fname(tmp_object->name));
			send_to_char(buffer, ch);
			test = TRUE;
		    }
		}
	    }
	    if (!test) {
		send_to_char("You do not seem to have anything.\n\r", ch);
	    }
     } else {
	    tmp_object = get_obj_in_list_vis(ch, arg, ch->carrying);
	    if (tmp_object) {
		if (! IS_SET(tmp_object->obj_flags.extra_flags, ITEM_NODROP)) {
		    sprintf(buffer,
			"You drop the %s.\n\r", fname(tmp_object->name));
		    send_to_char(buffer, ch);
		    act("$n drops $p.", 1, ch, tmp_object, 0, TO_ROOM);
		    obj_from_char(tmp_object);
		    obj_to_room(tmp_object,ch->in_room);
		} else {
		    send_to_char(
			"You can't drop it, it must be CURSED!\n\r", ch);
		}
	    } else {
		send_to_char("You do not have that item.\n\r", ch);
	    }
	}
    } else {
	send_to_char("Drop what?\n\r", ch);
    }
}

void do_putalldot(struct char_data *ch, char *name, char *target, int cmd)
{
	struct obj_data *tmp_object;
	struct obj_data *next_object;
	char buf[SHORT_STRING_LENGTH];
	bool found = FALSE;

	/* If "put all.object bag", get all carried items
	 * named "object", and put each into the bag.
	 */
	for (tmp_object = ch->carrying; tmp_object;
	     tmp_object = next_object) {
	     next_object = tmp_object->next_content;
	   if (isname(name, tmp_object->name) ) {
	      sprintf(buf, "%s %s", name, target);
	      found = TRUE;
	      do_put(ch, buf, cmd);
	}
       }
	if (!found) {
       send_to_char("You don't have one.\n\r", ch);
     } /* if */
}

void do_put(struct char_data *ch, char *argument, int cmd)
{
    char buffer[SHORT_STRING_LENGTH];
    char arg1[SHORT_STRING_LENGTH];
    char arg2[SHORT_STRING_LENGTH];
    struct obj_data *obj_object;
    struct obj_data *sub_object;
    struct char_data *tmp_char;
    int bits;
    char allbuf[SHORT_STRING_LENGTH];

      /* RT code to prevent wear all in room 3001 */
    if (ch->in_room == real_room(3001))
          {
             send_to_char("You may not put away objects in this room.\n\r",ch);
             return;
           }
    argument_interpreter(argument, arg1, arg2);
    if (*arg1) {
	if (*arg2) {
	  allbuf[0] = '\0';
	  if (sscanf(arg1, "all.%s", allbuf) != 0) {
	    do_putalldot(ch, allbuf, arg2, cmd);
	    return;
	  }
	  obj_object = get_obj_in_list_vis(ch, arg1, ch->carrying);
	  if (obj_object) {
	    bits = generic_find(arg2, FIND_OBJ_INV | FIND_OBJ_ROOM,
		    ch, &tmp_char, &sub_object);
	    if (sub_object) {
	      if (GET_ITEM_TYPE(sub_object) == ITEM_CONTAINER) {
	    if (!IS_SET(sub_object->obj_flags.value[1], CONT_CLOSED)) {
	      if (obj_object == sub_object) {
		send_to_char(
			"You attempt to fold it into itself, but fail.\n\r",
			ch);
		return;
	      }
	      if (((sub_object->obj_flags.weight) + 
		   (obj_object->obj_flags.weight)) <
		  (sub_object->obj_flags.value[0])) {
		send_to_char("Ok.\n\r", ch);
		if (bits==FIND_OBJ_INV) {
		  obj_from_char(obj_object);
		  /* make up for above line */
		  IS_CARRYING_W(ch) += GET_OBJ_WEIGHT(obj_object);
		  obj_to_obj(obj_object, sub_object);
		} else {
		  obj_from_char(obj_object);
		  obj_to_obj(obj_object, sub_object);
		}
		
		act("$n puts $p in $P",
			TRUE, ch, obj_object, sub_object, TO_ROOM);
	      } else {
		send_to_char("It won't fit.\n\r", ch);
	      }
	    } else
	      send_to_char("It seems to be closed.\n\r", ch);
	      } else {
	    sprintf(buffer,"The %s is not a container.\n\r",
		fname(sub_object->name));
	    send_to_char(buffer, ch);
	      }
	    } else {
	      sprintf(buffer, "You don't have the %s.\n\r", arg2);
	      send_to_char(buffer, ch);
	    }
	  } else {
	    sprintf(buffer, "You don't have the %s.\n\r", arg1);
	    send_to_char(buffer, ch);
	  }
	} else {
	  sprintf(buffer, "Put %s in what?\n\r", arg1);
	  send_to_char(buffer, ch);
	}
	  } else {
	send_to_char("Put what in what?\n\r",ch);
	  }
}



void do_give(struct char_data *ch, char *argument, int cmd)
{
    char obj_name[SHORT_STRING_LENGTH], vict_name[SHORT_STRING_LENGTH], buf[SHORT_STRING_LENGTH];
    char arg[SHORT_STRING_LENGTH];
    int amount;
    struct char_data *vict;
    struct obj_data *obj;

    argument=one_argument(argument,obj_name);
    if(is_number(obj_name))
    {
      if (strlen(obj_name)>7){
	send_to_char("Number field too large.\n\r", ch);
	return;
      }
      amount = atoi(obj_name);
      argument=one_argument(argument, arg);
      if (str_cmp("coins",arg) && str_cmp("coin",arg) && str_cmp("gold", arg))
	{
	  send_to_char("Sorry, you can't do that (yet)...\n\r",ch);
	  return;
	}
      if(amount<0)
	{
	  send_to_char("Sorry, you can't do that!\n\r",ch);
	  return;
	}
      if((GET_GOLD(ch)<amount) && (IS_NPC(ch) || (GET_LEVEL(ch) < 33)))
	{
	  send_to_char("You haven't got that many coins!\n\r",ch);
	  return;
	}
      argument=one_argument(argument, vict_name);
      if(!*vict_name)
	{
	  send_to_char("To who?\n\r",ch);
	  return;
	}
      if (!(vict = get_char_room_vis(ch, vict_name)))
	{
	  send_to_char("To who?\n\r",ch);
	  return;
	}
      send_to_char("Ok.\n\r",ch);
      sprintf(buf,"%s gives you %d gold coins.\n\r",PERS(ch,vict),amount);
      act(buf, 1, ch, 0, vict, TO_VICT);
      act("$n gives some gold to $N.", 1, ch, 0, vict, TO_NOTVICT);
      if (IS_NPC(ch) || (GET_LEVEL(ch) < 33))
	GET_GOLD(ch)-=amount;
      GET_GOLD(vict)+=amount;
      return;
    }

    argument=one_argument(argument, vict_name);


    if (!*obj_name || !*vict_name)
    {
	send_to_char("Give what to who?\n\r", ch);
	return;
    }
    if (!(obj = get_obj_in_list_vis(ch, obj_name, ch->carrying)))
    {
	send_to_char("You do not seem to have anything like that.\n\r",
	   ch);
	return;
    }
    if (IS_SET(obj->obj_flags.extra_flags, ITEM_NODROP))
    {
	send_to_char("You can't let go of it! Yeech!!\n\r", ch);
	return;
    }
    if (!(vict = get_char_room_vis(ch, vict_name)))
    {
	send_to_char("No one by that name around here.\n\r", ch);
	return;
    }

    if ((1+IS_CARRYING_N(vict)) > CAN_CARRY_N(vict))
    {
	act("$N seems to have $S hands full.", 0, ch, 0, vict, TO_CHAR);
	return;
    }
    if (obj->obj_flags.weight + IS_CARRYING_W(vict) > CAN_CARRY_W(vict))
    {
	act("$E can't carry that much weight.", 0, ch, 0, vict, TO_CHAR);
	return;
    }
    obj_from_char(obj);
    obj_to_char(obj, vict);
    act("$n gives $p to $N.", 1, ch, obj, vict, TO_NOTVICT);
    act("$n gives you $p.", 0, ch, obj, vict, TO_VICT);
    send_to_char("Ok.\n\r", ch);
}
