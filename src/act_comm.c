/***************************************************************************
 *  file: act_comm.c , Implementation of commands.         Part of DIKUMUD *
 *  Usage : Communication.                                                 *
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

/* RT for boards */

extern int Board_write_message(int board_type, struct char_data *ch, char *arg);
extern int find_board(struct char_data *ch);
extern void Board_save_board(int board_type);

/* channels is used to check which channels you have on */

void do_channels(struct char_data *ch,  char *argument, int cm)
{
    /* lists all channels that are on or off */
    send_to_char("   channel     status\n\r",ch);
    send_to_char("---------------------\n\r",ch);
 
    send_to_char("gossip         ",ch);
    if (!IS_SET(ch->specials.act,PLR_NOGOSSIP))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);
 
    send_to_char("auction        ",ch);
    if (!IS_SET(ch->specials.act,PLR_NOAUCTION))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);
 
    send_to_char("music          ",ch);
    if (!IS_SET(ch->specials.act,PLR_NOMUSIC))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);
 
    send_to_char("Q/A            ",ch);
    if (!IS_SET(ch->specials.act,PLR_NOQUESTION))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);
 
    send_to_char("quiet mode     ",ch);
    if (IS_SET(ch->specials.act,PLR_QUIET))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);
 
    send_to_char("shouts         ",ch);
    if (IS_SET(ch->specials.act,PLR_DEAF))
        send_to_char("OFF\n\r",ch);
    else
        send_to_char("ON\n\r",ch);
}


/* RT Deaf prevents you from hearing shouts...or making them! */
void do_deaf(struct char_data *ch, char *arugment, int cmd)
{
  if (IS_SET(ch->specials.act,PLR_NOSHOUT))
  {
     send_to_char("The gods have taken away your ability to shout.\n\r",ch);
     return;
  }
  if (IS_SET(ch->specials.act,PLR_DEAF))
  {
    send_to_char("You can now hear shouts again.\n\r",ch);
    REMOVE_BIT(ch->specials.act,PLR_DEAF);
  }
  else 
  {
    send_to_char("From now on, you won't hear shouts.\n\r",ch);
    SET_BIT(ch->specials.act,PLR_DEAF);
  }
}

 
/* RT  Quiet mode stops all communication except socials, says, and god tells */

void do_quiet(struct char_data *ch, char *argument, int cmd)
{
    if (IS_SET(ch->specials.act, PLR_QUIET))
    {
      send_to_char("Quiet mode removed.\n\r",ch);
      REMOVE_BIT(ch->specials.act, PLR_QUIET);
    }
    else
    {
      send_to_char("From now on, you will only hear says and emotes.\n\r",ch);
      SET_BIT(ch->specials.act,PLR_QUIET);
    }
}

void do_say(struct char_data *ch, char *argument, int cmd)
{
    int i;
    char buf[SHORT_STRING_LENGTH];

    for (i = 0; *(argument + i) == ' '; i++);

    if (!*(argument + i))
	send_to_char("Yes, but WHAT do you want to say?\n\r", ch);
    else
    {
	sprintf(buf,"$n says '%s'", argument + i);
	act_all(buf,FALSE,ch,0,0,TO_ROOM);
	sprintf(buf,"You say '%s'", argument + i);
	act_all(buf,FALSE,ch,0,0,TO_CHAR);
    }
}

void do_shout(struct char_data *ch, char *argument, int cmd)
{
    char buf1[SHORT_STRING_LENGTH];
    char buf2[SHORT_STRING_LENGTH];
    struct descriptor_data *i;


    if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOSHOUT))
    {
	send_to_char("You can't shout!!\n\r", ch);
	return;
    }
 
    if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_DEAF))
    {
	send_to_char("Deaf people can't shout.\n\r", ch);
        return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_QUIET))
    {
	send_to_char("You must turn off quiet mode first.\n\r", ch);
	return;
    }

    if (!IS_NPC(ch) && GET_LEVEL(ch)<3){
      send_to_char("Due to misuse, you must be of at least 3rd level to shout.\n\r",ch);
      return;
    }
    
    if (GET_MOVE(ch) < 10)
    {
	send_to_char("You are too exhausted to shout.\n\r", ch);
	return;
    }

    GET_MOVE(ch) -= 10;

    for (; *argument == ' '; argument++);

    if (!(*argument))
	send_to_char("Shout? Yes! Fine! Shout we must, but WHAT??\n\r", ch);
    else
    {
	sprintf(buf1, "$n shouts '%s'", argument);
	sprintf(buf2, "You shout '%s'", argument);
	act_all(buf2, 0, ch, 0, 0, TO_CHAR);

	for (i = descriptor_list; i; i = i->next)
	if (i->character != ch && !i->connected)
	  if (IS_NPC(i->character) ||
	     (
              !IS_SET(i->character->specials.act, PLR_QUIET) &&
              !IS_SET(i->character->specials.act, PLR_DEAF))
 	     )
             act_all(buf1, 0, ch, 0, i->character, TO_VICT);
    }
}


void do_gossip(struct char_data *ch, char *argument, int cmd)

{
    char buf1[SHORT_STRING_LENGTH];
    char buf2[SHORT_STRING_LENGTH];
    struct descriptor_data *i;


    for (; *argument == ' '; argument++);

    if (!(*argument))
        if (IS_SET(ch->specials.act, PLR_NOGOSSIP))
        {
          send_to_char("Gossip channel is now ON.\n\r", ch);
          REMOVE_BIT(ch->specials.act, PLR_NOGOSSIP);
        }
        else
        {
          send_to_char("Gossip channel is now OFF.\n\r", ch);
          SET_BIT(ch->specials.act, PLR_NOGOSSIP); 
        }
    else
    {
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOGOSSIP))
        {
          send_to_char("Gossip channel must be turned on first.\n\r", ch);
          return;
        } 
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_QUIET))
        {
	  send_to_char("You must turn off quiet mode first.\n\r", ch);
	  return;
	}
	if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOCHANNELS))
	{
	  send_to_char("The gods have revoked your channel privileges.\n\r",ch);
          return;
        }
        
        sprintf(buf1, "$n gossips '%s'", argument);
        sprintf(buf2, "You gossip '%s'", argument);
        act_all(buf2, 0, ch, 0, 0, TO_CHAR);

        for (i = descriptor_list; i; i = i->next)
        if (i->character != ch && !i->connected)
	  if (IS_NPC(i->character) ||
           (
            (!IS_SET(i->character->specials.act, PLR_NOGOSSIP)) &&
      	    (!IS_SET(i->character->specials.act, PLR_QUIET))
           ))
            act_all(buf1, 0, ch, 0, i->character, TO_VICT);
    }
}

void do_auction(struct char_data *ch, char *argument, int cmd)
 
{
    char buf1[SHORT_STRING_LENGTH];
    char buf2[SHORT_STRING_LENGTH];
    struct descriptor_data *i;
 
 
    for (; *argument == ' '; argument++);
 
    if (!(*argument))
        if (IS_SET(ch->specials.act, PLR_NOAUCTION))
        {
          send_to_char("Auction channel is now ON.\n\r", ch);
          REMOVE_BIT(ch->specials.act, PLR_NOAUCTION);
        }
        else
        {
          send_to_char("Auction channel is now OFF.\n\r", ch);
          SET_BIT(ch->specials.act, PLR_NOAUCTION);
        }
    else
    {
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOAUCTION))
        {
          send_to_char("Auction channel must be turned on first.\n\r", ch);
          return;
        }
	if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_QUIET))
	{
	  send_to_char("You must turn off quiet mode first.\n\r", ch);
	  return;
	}
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOCHANNELS))
	{
	  send_to_char("The gods have revoked your channel privileges.\n\r",ch);
          return;
	}

        sprintf(buf1, "$n auctions '%s'", argument);
        sprintf(buf2, "You auction '%s'", argument);
        act_all(buf2, 0, ch, 0, 0, TO_CHAR);
 
        for (i = descriptor_list; i; i = i->next)
        if (i->character != ch && !i->connected)
	  if (IS_NPC(i->character) ||
	     (
		(!IS_SET(i->character->specials.act, PLR_NOAUCTION)) &&
		(!IS_SET(i->character->specials.act, PLR_QUIET))
	     ))
             act_all(buf1, 0, ch, 0, i->character, TO_VICT);
    }
}

void do_question(struct char_data *ch, char *argument, int cmd)
 
{
    char buf1[SHORT_STRING_LENGTH];
    char buf2[SHORT_STRING_LENGTH];
    struct descriptor_data *i;
 
 
    for (; *argument == ' '; argument++);
 
    if (!(*argument))
        if (IS_SET(ch->specials.act, PLR_NOQUESTION))
        {
          send_to_char("Q/A channel is now ON.\n\r", ch);
          REMOVE_BIT(ch->specials.act, PLR_NOQUESTION);
        }
        else 
        {
          send_to_char("Q/A channel is now OFF.\n\r", ch);
          SET_BIT(ch->specials.act, PLR_NOQUESTION);
        }
    else
    {
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOQUESTION))
        {
          send_to_char("Q/A channel must be turned on first.\n\r", ch);
          return;
        }
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r", ch);
          return;
        }
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel privileges.\n\r",ch);
          return;
        }

        sprintf(buf1, "$n questions '%s'", argument);
        sprintf(buf2, "You question '%s'", argument);
        act_all(buf2, 0, ch, 0, 0, TO_CHAR);

        for (i = descriptor_list; i; i = i->next)
        if (i->character != ch && !i->connected)
          if (IS_NPC(i->character) ||
             (
                (!IS_SET(i->character->specials.act, PLR_NOQUESTION)) &&
                (!IS_SET(i->character->specials.act, PLR_QUIET))
             ))
             act_all(buf1, 0, ch, 0, i->character, TO_VICT);
 
    }
}

void do_answer(struct char_data *ch, char *argument, int cmd)
 
{
    char buf1[SHORT_STRING_LENGTH];
    char buf2[SHORT_STRING_LENGTH];
    struct descriptor_data *i;
 
 
    for (; *argument == ' '; argument++);
 
    if (!(*argument))
        if (IS_SET(ch->specials.act, PLR_NOQUESTION))
        {
          send_to_char("Q/A channel is now ON.\n\r", ch);
          REMOVE_BIT(ch->specials.act, PLR_NOQUESTION);
        }
        else 
        {
          send_to_char("Q/A channel is now OFF.\n\r", ch);
          SET_BIT(ch->specials.act, PLR_NOQUESTION);
        }
    else
    {
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOQUESTION))
        {
          send_to_char("Q/A channel must be turned on first.\n\r", ch);
          return;
        }
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r", ch);
          return;
        }
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel privileges.\n\r",ch);
          return;
        }

        sprintf(buf1, "$n answers '%s'", argument);
        sprintf(buf2, "You answer '%s'", argument);
        act_all(buf2, 0, ch, 0, 0, TO_CHAR);

        for (i = descriptor_list; i; i = i->next)
        if (i->character != ch && !i->connected)
          if (IS_NPC(i->character) ||
             (
                (!IS_SET(i->character->specials.act, PLR_NOQUESTION)) &&
                (!IS_SET(i->character->specials.act, PLR_QUIET))
             ))
             act_all(buf1, 0, ch, 0, i->character, TO_VICT);
 
    }
}

void do_music(struct char_data *ch, char *argument, int cmd)
 
{
    char buf1[SHORT_STRING_LENGTH];
    char buf2[SHORT_STRING_LENGTH];
    struct descriptor_data *i;
 
 
    for (; *argument == ' '; argument++);
 
    if (!(*argument))
        if (IS_SET(ch->specials.act, PLR_NOMUSIC))
        {
          send_to_char("Music channel is now ON.\n\r", ch);
	  REMOVE_BIT(ch->specials.act, PLR_NOMUSIC);
        }
	else
        {
	  send_to_char("Music channel is now OFF.\n\r",ch);
          SET_BIT(ch->specials.act, PLR_NOMUSIC);
        }
    else
    {
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOMUSIC))
        {
          send_to_char("Music channel must be turned on first.\n\r", ch);
          return;
        }
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r", ch);
          return;
        }
        if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel privileges.\n\r",ch);
          return;
        }

        sprintf(buf1, "$n MUSIC: '%s'", argument);
        sprintf(buf2, "You MUSIC:  '%s'", argument);
        act_all(buf2, 0, ch, 0, 0, TO_CHAR);

        for (i = descriptor_list; i; i = i->next)
        if (i->character != ch && !i->connected)
          if (IS_NPC(i->character) ||
             (
                (!IS_SET(i->character->specials.act, PLR_NOMUSIC)) &&
                (!IS_SET(i->character->specials.act, PLR_QUIET))
             ))
             act_all(buf1, 0, ch, 0, i->character, TO_VICT); 
    }
}

void do_tell(struct char_data *ch, char *argument, int cmd)
{
    struct char_data *vict;
    char name[100], message[SHORT_STRING_LENGTH],
	buf[SHORT_STRING_LENGTH];

   if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOTELL))
    {
	send_to_char("Your message didn't get through!!\n\r", ch);
	return;
    }  
    if (!IS_NPC(ch) && IS_SET(ch->specials.act, PLR_QUIET))
    {
	send_to_char("You must turn quiet mode off first.\n\r", ch);
	return;
    }

    half_chop(argument,name,message);

    if(!*name || !*message)
	send_to_char("Who do you wish to tell what??\n\r", ch);
    else if (!(vict = get_char_vis(ch, name)))
	send_to_char("No-one by that name here.\n\r", ch);
    else if (ch == vict)
	send_to_char("You try to tell yourself something.\n\r", ch);
    else if (( GET_POS(vict) == POSITION_SLEEPING && (GET_LEVEL(ch) < 31)) || 
             (!IS_NPC(vict) && (GET_LEVEL(ch) < 31)  && 
              (GET_LEVEL(ch) < GET_LEVEL(vict)) &&
              (IS_SET(vict->specials.act,PLR_NOTELL) ||
	       IS_SET(vict->specials.act,PLR_QUIET)
              )))
    {
	act("$E can't hear you.",FALSE,ch,0,vict,TO_CHAR);
    }
    else if (!vict->desc) /* fixes the tell bug and link death */
    {
      if (IS_NPC(vict))
	send_to_char("No-one by that name here.\n\r", ch);
      else
	act("$N seems to have misplaced $S link -- try again later.",
	     FALSE,ch,0,vict,TO_CHAR);
    }
    else
    {
	sprintf(buf,"%s tells you, '%s'\n\r",
	  (IS_NPC(ch) ? ch->player.short_descr : GET_NAME(ch)), message);
	send_to_char(buf, vict);
	sprintf(buf,"You tell %s, '%s'\n\r",
	    (IS_NPC(vict) ? vict->player.short_descr : GET_NAME(vict)), message);
	send_to_char(buf, ch);
    }
}



void do_whisper(struct char_data *ch, char *argument, int cmd)
{
    struct char_data *vict;
    char name[100], message[SHORT_STRING_LENGTH],
	buf[SHORT_STRING_LENGTH];

    half_chop(argument,name,message);

    if(!*name || !*message)
	send_to_char("Who do you want to whisper to.. and what??\n\r", ch);
    else if (!(vict = get_char_room_vis(ch, name)))
	send_to_char("No-one by that name here..\n\r", ch);
    else if (vict == ch)
    {
	act("$n whispers quietly to $mself.",FALSE,ch,0,0,TO_ROOM);
	send_to_char(
	    "You can't seem to get your mouth close enough to your ear...\n\r",
	     ch);
    }
    else
    {
	sprintf(buf,"$n whispers to you, '%s'",message);
	act(buf, FALSE, ch, 0, vict, TO_VICT);
	sprintf(buf,"You whisper to $N, '%s'",message);
	act(buf, FALSE, ch, 0, vict, TO_CHAR);
	act("$n whispers something to $N.", FALSE, ch, 0, vict, TO_NOTVICT);
    }
}


void do_ask(struct char_data *ch, char *argument, int cmd)
{
    struct char_data *vict;
    char name[100], message[SHORT_STRING_LENGTH],
	buf[SHORT_STRING_LENGTH];

    half_chop(argument,name,message);

    if(!*name || !*message)
	send_to_char("Who do you want to ask something, and what??\n\r", ch);
    else if (!(vict = get_char_room_vis(ch, name)))
	send_to_char("No-one by that name here.\n\r", ch);
    else if (vict == ch)
    {
	act("$n quietly asks $mself a question.",FALSE,ch,0,0,TO_ROOM);
	send_to_char("You think about it for a while...\n\r", ch);
    }
    else
    {
	sprintf(buf,"$n asks you, '%s'",message);
	act(buf, FALSE, ch, 0, vict, TO_VICT);
	sprintf(buf,"You ask $N, '%s'",message);
	act(buf, FALSE, ch, 0, vict, TO_CHAR);
	act("$n asks $N a question.",FALSE,ch,0,vict,TO_NOTVICT);
    }
}



#define MAX_NOTE_LENGTH 1000      /* arbitrary */

void do_write(struct char_data *ch, char *argument, int cmd)
{
    struct obj_data *paper = 0, *pen = 0;
    char papername[MAX_INPUT_LENGTH], penname[MAX_INPUT_LENGTH],
	buf[SHORT_STRING_LENGTH];

    int board_type;

    argument_interpreter(argument, papername, penname);

    if (!ch->desc)
	return;
   
    /* RT this lovely code prevents writing in a room with a board..oh well! */
    board_type = (find_board(ch));
    if (board_type != -1);
    {
      Board_write_message(board_type,ch,argument);
      Board_save_board(board_type);
      return;
    }

    if (!*papername)  /* nothing was delivered */
    {   
	send_to_char(
	    "Write? with what? ON what? what are you trying to do??\n\r", ch);
	return;
    }
    if (*penname) /* there were two arguments */
    {
	if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying)))
	{
	    sprintf(buf, "You have no %s.\n\r", papername);
	    send_to_char(buf, ch);
	    return;
	}
	if (!(pen = get_obj_in_list_vis(ch, penname, ch->carrying)))
	{
	    sprintf(buf, "You have no %s.\n\r", papername);
	    send_to_char(buf, ch);
	    return;
	}
    }
    else  /* there was one arg.let's see what we can find */
    {           
	if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying)))
	{
	    sprintf(buf, "There is no %s in your inventory.\n\r", papername);
	    send_to_char(buf, ch);
	    return;
	}
	if (paper->obj_flags.type_flag == ITEM_PEN)  /* oops, a pen.. */
	{
	    pen = paper;
	    paper = 0;
	}
	else if (paper->obj_flags.type_flag != ITEM_NOTE)
	{
	    send_to_char("That thing has nothing to do with writing.\n\r", ch);
	    return;
	}

	/* one object was found. Now for the other one. */
	if (!ch->equipment[HOLD])
	{
	    sprintf(buf, "You can't write with a %s alone.\n\r", papername);
	    send_to_char(buf, ch);
	    return;
	}
	if (!CAN_SEE_OBJ(ch, ch->equipment[HOLD]))
	{
	    send_to_char("The stuff in your hand is invisible! Yeech!!\n\r", ch);
	    return;
	}
	
	if (pen)
	    paper = ch->equipment[HOLD];
	else
	    pen = ch->equipment[HOLD];
    }
	    
    /* ok.. now let's see what kind of stuff we've found */
    if (pen->obj_flags.type_flag != ITEM_PEN)
    {
	act("$p is no good for writing with.",FALSE,ch,pen,0,TO_CHAR);
    }
    else if (paper->obj_flags.type_flag != ITEM_NOTE)
    {
	act("You can't write on $p.", FALSE, ch, paper, 0, TO_CHAR);
    }
    else if (paper->action_description)
	send_to_char("There's something written on it already.\n\r", ch);
    else
    {
	/* we can write - hooray! */
		
	send_to_char("Ok.. go ahead and write.. end the note with a @.\n\r",
	    ch);
	act("$n begins to jot down a note.", TRUE, ch, 0,0,TO_ROOM);
	ch->desc->str = &paper->action_description;
	ch->desc->max_str = MAX_NOTE_LENGTH;
    }
}
