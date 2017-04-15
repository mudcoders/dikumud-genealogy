/* ************************************************************************
*   File: act.comm.c                                    Part of CircleMUD *
*  Usage: Player-level communication commands                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************* ********************************************** */

#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "screen.h"

/* extern variables */
extern struct room_data *world;
extern struct descriptor_data *descriptor_list;


ACMD(do_say)
{
   int	i;

   for (i = 0; *(argument + i) == ' '; i++)
      ;

   if (!*(argument + i))
      send_to_char("Yes, but WHAT do you want to say?\n\r", ch);
   else {
      sprintf(buf, "$n says '%s'", argument + i);
      act(buf, FALSE, ch, 0, 0, TO_ROOM);
      if (!PRF_FLAGGED(ch, PRF_NOREPEAT)) {
	 sprintf(buf, "You say '%s'\n\r", argument + i);
	 send_to_char(buf, ch);
      } else
	 send_to_char("Ok.\n\r", ch);
   }
}


ACMD(do_gsay)
{
   int	i;
   struct char_data *k;
   struct follow_type *f;

   for (i = 0; *(argument + i) == ' '; i++)
      ;

   if (!IS_AFFECTED(ch, AFF_GROUP)) {
      send_to_char("But you are not the member of a group!\n\r", ch);
      return;
   }

   if (!*(argument + i))
      send_to_char("Yes, but WHAT do you want to group-say?\n\r", ch);
   else {
      if (ch->master)
	 k = ch->master;
      else
	 k = ch;
      sprintf(buf, "%s group-says '%s'\n\r", GET_NAME(ch), argument + i);
      if (IS_AFFECTED(k, AFF_GROUP) && (k != ch))
	 send_to_char(buf, k);
      for (f = k->followers; f; f = f->next)
	 if (IS_AFFECTED(f->follower, AFF_GROUP) && (f->follower != ch))
	    send_to_char(buf, f->follower);
      if (!PRF_FLAGGED(ch, PRF_NOREPEAT)) {
	 sprintf(buf, "You group-say '%s'\n\r", argument + i);
	 send_to_char(buf, ch);
      } else
	 send_to_char("Ok.\n\r", ch);
   }
}



ACMD(do_tell)
{
   struct char_data *vict;

   half_chop(argument, buf, buf2);

   if (!*buf || !*buf2)
      send_to_char("Who do you wish to tell what??\n\r", ch);
   else if (!(vict = get_char_vis(ch, buf)))
      send_to_char("No-one by that name here..\n\r", ch);
   else if (ch == vict)
      send_to_char("You try to tell yourself something.\n\r", ch);
   else if (PRF_FLAGGED(ch, PRF_NOTELL))
      send_to_char("You can't tell other people while you have notell on.\n\r", ch);
   else if (!IS_NPC(vict) && !vict->desc) /* linkless */
      act("$E's linkless at the moment.", FALSE, ch, 0, vict, TO_CHAR);
   else if (PLR_FLAGGED(vict, PLR_WRITING))
      act("$E's writing a message right now, try again later.",
          FALSE, ch, 0, vict, TO_CHAR);
   else if ((GET_POS(vict) == POSITION_SLEEPING) || (PRF_FLAGGED(vict, PRF_NOTELL)))
      act("$E can't hear you.", FALSE, ch, 0, vict, TO_CHAR);
   else {
      sprintf(buf, "%s tells you '%s'%s\n\r", GET_NAME(ch), buf2,
	      CCNRM(vict, C_NRM));
      CAP(buf);
      send_to_char(CCRED(vict, C_NRM), vict);
      send_to_char(buf, vict);
      if (!PRF_FLAGGED(ch, PRF_NOREPEAT)) {
	 sprintf(buf, "%sYou tell %s '%s'%s\n\r", CCRED(ch, C_CMP),
		 GET_NAME(vict), buf2, CCNRM(ch, C_CMP));
	 send_to_char(buf, ch);
      } else
	 send_to_char("Ok.\n\r", ch);
   }
}



ACMD(do_whisper)
{
   struct char_data *vict;

   half_chop(argument, buf, buf2);

   if (!*buf || !*buf2)
      send_to_char("Who do you want to whisper to.. and what??\n\r", ch);
   else if (!(vict = get_char_room_vis(ch, buf)))
      send_to_char("No-one by that name here..\n\r", ch);
   else if (vict == ch) {
      act("$n whispers quietly to $mself.", FALSE, ch, 0, 0, TO_ROOM);
      send_to_char("You can't seem to get your mouth close enough to your ear...\n\r", ch);
   } else {
      sprintf(buf, "$n whispers to you, '%s'", buf2);
      act(buf, FALSE, ch, 0, vict, TO_VICT);
      send_to_char("Ok.\n\r", ch);
      act("$n whispers something to $N.", FALSE, ch, 0, vict, TO_NOTVICT);
   }
}


ACMD(do_ask)
{
   struct char_data *vict;

   half_chop(argument, buf, buf2);

   if (!*buf || !*buf2)
      send_to_char("Who do you want to ask something.. and what??\n\r", ch);
   else if (!(vict = get_char_room_vis(ch, buf)))
      send_to_char("No-one by that name here..\n\r", ch);
   else if (vict == ch) {
      act("$n quietly asks $mself a question.", FALSE, ch, 0, 0, TO_ROOM);
      send_to_char("You think about it for a while...\n\r", ch);
   } else {
      sprintf(buf, "$n asks you '%s'", buf2);
      act(buf, FALSE, ch, 0, vict, TO_VICT);
      send_to_char("Ok.\n\r", ch);
      act("$n asks $N a question.", FALSE, ch, 0, vict, TO_NOTVICT);
   }
}



#define MAX_NOTE_LENGTH 1000      /* arbitrary */

ACMD(do_write)
{
   struct obj_data *paper = 0, *pen = 0;
   char	*papername, *penname;

   papername = buf1;
   penname = buf2;

   argument_interpreter(argument, papername, penname);

   if (!ch->desc)
      return;

   if (!*papername)  /* nothing was delivered */ {
      send_to_char("Write?  With what?  ON what?  What are you trying to do?!?\n\r", ch);
      return;
   }
   if (*penname) /* there were two arguments */ {
      if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying))) {
	 sprintf(buf, "You have no %s.\n\r", papername);
	 send_to_char(buf, ch);
	 return;
      }
      if (!(pen = get_obj_in_list_vis(ch, penname, ch->carrying))) {
	 sprintf(buf, "You have no %s.\n\r", papername);
	 send_to_char(buf, ch);
	 return;
      }
   } else /* there was one arg.let's see what we can find */	 {
      if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying))) {
	 sprintf(buf, "There is no %s in your inventory.\n\r", papername);
	 send_to_char(buf, ch);
	 return;
      }
      if (paper->obj_flags.type_flag == ITEM_PEN)  /* oops, a pen.. */ {
	 pen = paper;
	 paper = 0;
      } else if (paper->obj_flags.type_flag != ITEM_NOTE) {
	 send_to_char("That thing has nothing to do with writing.\n\r", ch);
	 return;
      }

      /* one object was found. Now for the other one. */
      if (!ch->equipment[HOLD]) {
	 sprintf(buf, "You can't write with a %s alone.\n\r", papername);
	 send_to_char(buf, ch);
	 return;
      }
      if (!CAN_SEE_OBJ(ch, ch->equipment[HOLD])) {
	 send_to_char("The stuff in your hand is invisible!  Yeech!!\n\r", ch);
	 return;
      }

      if (pen)
	 paper = ch->equipment[HOLD];
      else
	 pen = ch->equipment[HOLD];
   }

   /* ok.. now let's see what kind of stuff we've found */
   if (pen->obj_flags.type_flag != ITEM_PEN) {
      act("$p is no good for writing with.", FALSE, ch, pen, 0, TO_CHAR);
   } else if (paper->obj_flags.type_flag != ITEM_NOTE) {
      act("You can't write on $p.", FALSE, ch, paper, 0, TO_CHAR);
   } else if (paper->action_description)
      send_to_char("There's something written on it already.\n\r", ch);
   else {
      /* we can write - hooray! */
      send_to_char("Ok.. go ahead and write.. end the note with a @.\n\r", ch);
      act("$n begins to jot down a note.", TRUE, ch, 0, 0, TO_ROOM);
      ch->desc->str = &paper->action_description;
      ch->desc->max_str = MAX_NOTE_LENGTH;
   }
}



ACMD(do_page)
{
   struct descriptor_data *d;
   struct char_data *vict;

   if (IS_NPC(ch)) {
      send_to_char("Monsters can't page.. go away.\n\r", ch);
      return;
   }

   if (!*argument) {
      send_to_char("Whom do you wish to page?\n\r", ch);
      return;
   }
   half_chop(argument, buf, buf2);
   if (!str_cmp(buf, "all")) {
      if (GET_LEVEL(ch) > LEVEL_GOD) {
	 sprintf(buf, "\007\007*%s* %s\n\r", GET_NAME(ch), buf2);
	 for (d = descriptor_list; d; d = d->next)
	    if (!d->connected)
	       SEND_TO_Q(buf, d);
      } else
	 send_to_char("You will never be godly enough to do that!\n\r", ch);
      return;
   }

   if ((vict = get_char_vis(ch, buf))) {
      sprintf(buf, "\007\007*%s* %s\n\r", GET_NAME(ch), buf2);
      send_to_char(buf, vict);
      if (PRF_FLAGGED(ch, PRF_NOREPEAT))
	 send_to_char("Ok.\n\r", ch);
      else
	 send_to_char(buf, ch);
      return;
   } else
      send_to_char("There is no such person in the game!\n\r", ch);
}


/**********************************************************************
 * generalized communication func, originally by Fred C. Merkel (Torg) *
  *********************************************************************/

ACMD(do_gen_com)
{
   extern int	level_can_shout;
   extern int	holler_move_cost;
   struct descriptor_data *i;
   char	buf3[MAX_STRING_LENGTH];
   char	buf4[MAX_STRING_LENGTH];
   char	name[MAX_NAME_LENGTH+10];
   char	colon[24];

   static int	channels[] = {
      0,
      PRF_DEAF,
      PRF_NOGOSS,
      PRF_NOAUCT,
      PRF_NOGRATZ
   };

   static char	*com_msgs[][4] =  {
      { "You cannot holler!!\n\r",
      "holler",
      "",
      KYEL },
      { "You cannot shout!!\n\r",
      "shout",
      "Turn off your noshout flag first!\n\r",
      KYEL },
      { "You cannot gossip!!\n\r",
      "gossip", 
      "You aren't even on the channel!\n\r",
      KYEL },
      { "You cannot auction!!\n\r", 
      "auction",
      "You aren't even on the channel!\n\r",
      "" },
      { "You cannot congratulate!\n\r",
      "congrat",
      "You aren't even on the channel!\n\r",
      KGRN }
   };  


   if (!ch->desc)
      return;

   if (PLR_FLAGGED(ch, PLR_NOSHOUT)) {
      send_to_char(com_msgs[subcmd][0], ch);
      return;
   }
   if (GET_LEVEL(ch) < level_can_shout) {
      sprintf(buf1, "You must be at least level %d before you can %s.\n\r",
          level_can_shout, com_msgs[subcmd][1]);
      send_to_char(buf1, ch);
      return;
   }

   if (subcmd == SCMD_HOLLER) {
      if (GET_MOVE(ch) < holler_move_cost) {
	 send_to_char("You're too exhausted to holler.\n\r", ch);
	 return;
      } else
	 GET_MOVE(ch) -= holler_move_cost;
   }


   if (PRF_FLAGGED(ch, channels[subcmd])) {
      send_to_char(com_msgs[subcmd][2], ch);
      return;
   }

   for (; *argument == ' '; argument++)
      ;
   if (!(*argument)) {
      sprintf(buf1, "Yes, %s, fine, %s we must, but WHAT???\n\r",
          com_msgs[subcmd][1], com_msgs[subcmd][1]);
      send_to_char(buf1, ch);
      return;
   }

   strcpy(colon, com_msgs[subcmd][3]);
   if (PRF_FLAGGED(ch, PRF_NOREPEAT))
      send_to_char("Ok.\n\r", ch);
   else {
      if (COLOR_LEV(ch) >= C_CMP)
         sprintf(buf1, "%sYou %s, '%s'%s\n\r", colon, com_msgs[subcmd][1],
	     argument, KNRM);
      else
	 sprintf(buf1, "You %s, '%s'\n\r", com_msgs[subcmd][1], argument);
      send_to_char(buf1, ch);
   }

   strcpy(name, GET_NAME(ch));
   *name = UPPER(*name);

   sprintf(buf1, "%s %ss, '%s'\n\r", name, com_msgs[subcmd][1], argument);
   sprintf(buf2, "Someone %ss, '%s'\n\r", com_msgs[subcmd][1], argument);
   sprintf(buf3, "%s%s %ss, '%s'%s\n\r", colon, name, com_msgs[subcmd][1], argument,
       KNRM);
   sprintf(buf4, "%sSomeone %ss, '%s'%s\n\r", colon, com_msgs[subcmd][1], argument,
       KNRM);

   for (i = descriptor_list; i; i = i->next) {
      if (!i->connected && i != ch->desc && 
          !PRF_FLAGGED(i->character, channels[subcmd]) && 
          !PLR_FLAGGED(i->character, PLR_WRITING)) {

	 if (subcmd == SCMD_SHOUT && 
	     ((world[ch->in_room].zone != world[i->character->in_room].zone) || 
	     GET_POS(i->character) < POSITION_RESTING))
	    continue;

	 if (CAN_SEE(i->character, ch)) {
	    if (COLOR_LEV(i->character) >= C_NRM)
	       send_to_char(buf3, i->character);
	    else
	       send_to_char(buf1, i->character);
	 } else {
	    if (COLOR_LEV(i->character) >= C_NRM)
	       send_to_char(buf4, i->character);
	    else
	       send_to_char(buf2, i->character);
	 }
      }
   }
}


ACMD(do_qsay)
{
   struct descriptor_data *i;

   if (!PRF_FLAGGED(ch, PRF_QUEST)) {
      send_to_char("You aren't even part of the quest!\n\r", ch);
      return;
   }

   for (; *argument == ' '; argument++)
      ;
   if (!(*argument))
      send_to_char("Quest-say?  Yes!  Fine!  Quest-say we must, but WHAT??\n\r", ch);
   else {
      if (PRF_FLAGGED(ch, PRF_NOREPEAT))
	 send_to_char("Ok.\n\r", ch);
      else {
	 sprintf(buf1, "You quest-say, '%s'\n\r", argument);
	 send_to_char(buf1, ch);
      }

      sprintf(buf1, "$n quest-says, '%s'", argument);
      for (i = descriptor_list; i; i = i->next)
	 if (!i->connected && i != ch->desc && 
	     PRF_FLAGGED(i->character, PRF_QUEST) && 
	     !PLR_FLAGGED(i->character, PLR_WRITING))
	    act(buf1, 0, ch, 0, i->character, TO_VICT);
   }
}


