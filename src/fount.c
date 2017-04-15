/***************************************************************************
 *  Copyright (C) 1992, 1993 Michael Chastain, Michael Quan, Mitchell Tse  *
 *  Performance optimization and bug fixes by MERC Industries.             *
 *  You can use our stuff in any way you like whatsoever so long as this   *
 *  copyright notice remains intact.  If you like it please drop a line    *
 *  to mec@garnet.berkeley.edu.                                            *
 *                                                                         *
 *  This is free software and you are benefitting.  We hope that you       *
 *  share your changes too.  What goes around, comes around.               *
 ***************************************************************************/

#include <strings.h>
#include <stdio.h>
#include <ctype.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "utils.h"

extern struct room_data *world;
int real_object(int i);

/*************************************************************************
* Figures out if a fountain is present in the room                       *
*************************************************************************/

int FOUNTAINisPresent (struct char_data *ch)
{
  struct obj_data *tmp;
  bool found = FALSE;

  for (tmp = world[ch->in_room].contents;
       tmp != NULL && !found;
       tmp = tmp->next_content) {
    if (((tmp->item_number == real_object(5220)) ||
     (tmp->item_number == real_object(3135)) ||
     (tmp->item_number == real_object(1300))) &&
    (CAN_SEE_OBJ(ch, tmp))) {
      found = TRUE;
    }
  }

  return found;

}

/************************************************************************
*  Fill skins and any other drink containers.                           *
************************************************************************/

void do_fill(struct char_data *ch, char *argument, int cmd)
{
  char buf[MAX_STRING_LENGTH];
  struct obj_data *to_obj;
  int amount;
  void name_to_drinkcon(struct obj_data *obj, int type);
  struct obj_data *get_obj_in_list_vis(struct char_data *ch, char *name, \
		       struct obj_data *list);
  
  one_argument(argument, buf);

  if(!*buf) /* No arguments */
    {
      act("What do you want to fill?",FALSE,ch,0,0,TO_CHAR);
      return;
    }
  
  if(!(to_obj = get_obj_in_list_vis(ch,buf,ch->carrying)))
    {
      act("You can't find it!",FALSE,ch,0,0,TO_CHAR);
      return;
    }

  if(FOUNTAINisPresent(ch)) {

    if(to_obj->obj_flags.type_flag!=ITEM_DRINKCON)
      {
    act("You can't pour anything into that.",FALSE,ch,0,0,TO_CHAR);
    return;
      }

    if((to_obj->obj_flags.value[1]!=0)&&
       (to_obj->obj_flags.value[2]!=0))
      {
    act("There is already another liquid in it!",FALSE,ch,0,0,TO_CHAR);
    return;
      }

    if(!(to_obj->obj_flags.value[1]<to_obj->obj_flags.value[0]))
      {
    act("There is no room for more.",FALSE,ch,0,0,TO_CHAR);
    return;
      }

    act ("You fill $p!",TRUE,ch,to_obj,0,TO_CHAR);

    /* First same type liq. */
    to_obj->obj_flags.value[2]=0;
    
    /* Then how much to pour */
    amount= to_obj->obj_flags.value[0]-to_obj->obj_flags.value[1];

    to_obj->obj_flags.value[1]=to_obj->obj_flags.value[0];


    
  } else {
    act("There is no fountain here!",FALSE,ch,0,0,TO_CHAR);
  }
  
  return;
}
