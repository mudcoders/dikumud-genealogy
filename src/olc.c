/* ************************************************************************
*   File: olc.c                                         Part of CircleMUD *
*  Usage: online creation                                                 *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/*
 * PLEASE, FOR THE LOVE OF GOD, DON'T TRY TO USE THIS YET!!!
 *  *** DO *** NOT *** SEND ME MAIL ASKING WHY IT DOESN'T WORK -- IT'S
 *  NOT DONE!!
 */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "olc.h"

/* OLC command format:
 *
 * olc {"." | {<"room"|"mobile"|"object"> <number>}} <arguments>
 * olc {"set"|"show"} <attribute> <arguments>
 */

#define OLC_USAGE "Usage: olc { . | set | show | obj | mob | room} [args]\r\n"

extern struct room_data *world;
extern struct obj_data *obj_proto;
extern struct char_data *mob_proto;

struct char_data *olc_ch;

/* local functions */
void olc_interpreter(void *targ, int mode, char *arg);
void olc_set_show(struct char_data * ch, int olc_mode, char *arg);
void olc_string(char **string, size_t maxlen, char *arg);
int can_modify(struct char_data * ch, int vnum);
ACMD(do_olc);
void olc_bitvector(int *bv, const char **names, char *arg);

const char *olc_modes[] = {
  "set",			/* set OLC characteristics */
  "show",			/* show OLC characteristics */
  ".",				/* repeat last modification command */
  "room",			/* modify a room */
  "mobile",			/* modify a mobile */
  "object",			/* modify an object */
  "\n"
};

const char *olc_commands[] = {
  "copy",
  "name",
  "description",
  "aliases",
  "\n",				/* many more to be added */
};


/* The actual do_olc command for the interpreter.  Determines the target
   entity, checks permissions, and passes control to olc_interpreter */
ACMD(do_olc)
{
  void *olc_targ = NULL;
  char mode_arg[MAX_INPUT_LENGTH];
  room_rnum rnum;
  room_vnum vnum = NOWHERE;
  int olc_mode;

  /* WARNING!  **DO NOT** under any circumstances remove the code below!!!!  */
  if (strcmp(GET_NAME(ch), "Ras")) {
    send_to_char("OLC is not yet complete.  Sorry.\r\n", ch);
    return;
  }
  /* WARNING!  **DO NOT** under any circumstances remove the code above!!!!  */

  /* first, figure out the first (mode) argument */
  half_chop(argument, mode_arg, argument);
  if ((olc_mode = search_block(mode_arg, olc_modes, FALSE)) < 0) {
    sprintf(buf, "Invalid mode '%s'.\r\n", mode_arg);
    send_to_char(buf, ch);
    send_to_char(OLC_USAGE, ch);
    return;
  }
  switch (olc_mode) {
  case OLC_SET:
  case OLC_SHOW:
    olc_set_show(ch, olc_mode, argument);
    return;
  case OLC_REPEAT:
    if (!(olc_mode = GET_LAST_OLC_MODE(ch)) ||
	((olc_targ = GET_LAST_OLC_TARG(ch)) == NULL)) {
      send_to_char("No last OLC operation!\r\n", ch);
      return;
    }
    break;
  case OLC_ROOM:
    if (isdigit(*argument)) {
      /* room specified.  take the numeric argument off */
      argument = one_argument(argument, arg);
      if ((vnum = atoi(arg)) < 0) {
	sprintf(buf, "Invalid room vnum '%s'.\r\n", arg);
	send_to_char(buf, ch);
	return;
      }
      if ((rnum = real_room(vnum)) == NOWHERE) {
	send_to_char("No such room!\r\n", ch);
	return;
      }
    } else {
      rnum = ch->in_room;
      vnum = GET_ROOM_VNUM(IN_ROOM(ch));
      sprintf(buf, "(Using current room %d)\r\n", vnum);
      send_to_char(buf, ch);
    }

/*   if (!ROOM_FLAGGED(rnum, ROOM_OLC))
	 send_to_char("That room is not modifyable.\r\n", ch);
     else
*/
    olc_targ = (void *) &(world[rnum]);
    break;
  case OLC_MOB:
    argument = one_argument(argument, arg);
    if ((vnum = atoi(arg)) < 0) {
      sprintf(buf, "Invalid mob vnum '%s'.\r\n", arg);
      send_to_char(buf, ch);
      return;
    }
    if ((rnum = real_mobile(vnum)) < 0)
      send_to_char("No such mobile vnum.\r\n", ch);
    else
      olc_targ = (void *) &(mob_proto[rnum]);
    break;
  case OLC_OBJ:
    argument = one_argument(argument, arg);
    if ((vnum = atoi(arg)) < 0) {
      sprintf(buf, "Invalid obj vnum '%s'\r\n", arg);
      send_to_char(buf, ch);
      return;
    }
    if ((rnum = real_object(vnum)) < 0) {
      sprintf(buf, "No object with vnum %d.\r\n", vnum);
      send_to_char(buf, ch);
    } else
      olc_targ = (void *) &(obj_proto[rnum]);
    break;
  default:
    send_to_char("Usage: olc {.|set|show|obj|mob|room} [args]\r\n", ch);
    return;
  }

  if (olc_targ == NULL)
    return;

  if (!can_modify(ch, vnum)) {
    send_to_char("You can't modify that.\r\n", ch);
    return;
  }
  GET_LAST_OLC_MODE(ch) = olc_mode;
  GET_LAST_OLC_TARG(ch) = olc_targ;

  olc_ch = ch;
  olc_interpreter(olc_targ, olc_mode, argument);
  /* freshen? */
}


/* OLC interpreter command; called by do_olc */
void olc_interpreter(void *targ, int mode, char *arg)
{
  int error = 0, command;
  char command_string[MAX_INPUT_LENGTH];
  struct char_data *olc_mob = NULL;
  struct room_data *olc_room = NULL;
  struct obj_data *olc_obj = NULL;

  half_chop(arg, command_string, arg);
  if ((command = search_block(command_string, olc_commands, FALSE)) < 0) {
    sprintf(buf, "Invalid OLC command '%s'.\r\n", command_string);
    send_to_char(buf, olc_ch);
    return;
  }
  switch (mode) {
  case OLC_ROOM:
    olc_room = (struct room_data *) targ;
    break;
  case OLC_MOB:
    olc_mob = (struct char_data *) targ;
    break;
  case OLC_OBJ:
    olc_obj = (struct obj_data *) targ;
    break;
  default:
    log("SYSERR: Invalid OLC mode %d passed to interp.", mode);
    return;
  }


  switch (command) {
  case OLC_COPY:
    switch (mode) {
    case OLC_ROOM:
      break;
    case OLC_MOB:
      break;
    case OLC_OBJ:
      break;
    default:
      error = 1;
      break;
    }
    break;
  case OLC_NAME:
    switch (mode) {
    case OLC_ROOM:
      olc_string(&(olc_room->name), MAX_ROOM_NAME, arg);
      break;
    case OLC_MOB:
      olc_string(&olc_mob->player.short_descr, MAX_MOB_NAME, arg);
      break;
    case OLC_OBJ:
      olc_string(&olc_obj->short_description, MAX_OBJ_NAME, arg);
      break;
    default:
      error = 1;
      break;
    }
    break;

  case OLC_DESC:
    switch (mode) {
    case OLC_ROOM:
      olc_string(&olc_room->description, MAX_ROOM_DESC, arg);
      break;
    case OLC_MOB:
      olc_string(&olc_mob->player.long_descr, MAX_MOB_DESC, arg);
      break;
    case OLC_OBJ:
      olc_string(&olc_obj->description, MAX_OBJ_DESC, arg);
      break;
    default:
      error = 1;
      break;
    }
    break;

  case OLC_ALIASES:
    switch (mode) {
    case OLC_ROOM:
      break;
    case OLC_MOB:
      break;
    case OLC_OBJ:
      break;
    default:
      error = 1;
      break;
    }

  }
}


/* can_modify: determine if a particular char can modify a vnum */
int can_modify(struct char_data * ch, int vnum)
{
  return (1);
}


/* generic fn for modifying a string */
void olc_string(char **string, size_t maxlen, char *arg)
{
  skip_spaces(&arg);

  if (!*arg) {
    sprintf(buf, "Enter new string (max of %d characters); "
	    "use '@' on a new line when done.\r\n", (int) maxlen);
    send_to_char(buf, olc_ch);
    **string = '\0';
    string_write(olc_ch->desc, string, maxlen, 0, NULL);
  } else {
    if (strlen(arg) > maxlen) {
      sprintf(buf, "String too long (cannot be more than %d chars).\r\n",
	      (int) maxlen);
      send_to_char(buf, olc_ch);
    } else {
      if (*string != NULL)
	free(*string);
      *string = str_dup(arg);
      send_to_char(OK, olc_ch);
    }
  }
}


/* generic fn for modifying a bitvector */
void olc_bitvector(int *bv, const char **names, char *arg)
{
  int newbv, flagnum, remove = 0;
  char *this_name;

  skip_spaces(&arg);

  if (!*arg) {
    send_to_char("Flag list or flag modifiers required.\r\n", olc_ch);
    return;
  }
  /* determine if this is 'absolute' or 'relative' mode */
  if (*arg == '+' || *arg == '-')
    newbv = *bv;
  else
    newbv = 0;

  while (*arg) {
    arg = one_argument(arg, buf);	/* get next argument */

    /* change to upper-case */
    for (this_name = buf; *this_name; this_name++)
      CAP(this_name);

    /* determine if this is an add or a subtract */
    if (*buf == '+' || *buf == '-') {
      this_name = buf + 1;
      if (*buf == '-')
	remove = TRUE;
      else
	remove = FALSE;
    } else {
      this_name = buf;
      remove = FALSE;
    }

    /* figure out which one we're dealing with */
    if ((flagnum = search_block(this_name, names, TRUE)) < 0) {
      sprintf(buf2, "Unknown flag: %s\r\n", this_name);
      send_to_char(buf2, olc_ch);
    } else {
      if (remove)
	REMOVE_BIT(newbv, (1 << flagnum));
      else
	SET_BIT(newbv, (1 << flagnum));
    }
  }

  *bv = newbv;
  sprintbit(newbv, names, buf);
  sprintf(buf2, "Flags now set to: %s\r\n", buf);
  send_to_char(buf2, olc_ch);
}

void olc_set_show(struct char_data * ch, int olc_mode, char *arg)
{
}
