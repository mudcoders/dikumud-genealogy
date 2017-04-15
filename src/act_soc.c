/***************************************************************************
 *  file: act_soc.c , Implementation of commands.          Part of DIKUMUD *
 *  Usage : Social commands.                                               *
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

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

/* extern functions */

void parse_string(char *input, char *output, struct char_data *ch1,
    struct char_data *ch2, struct char_data *to);
int action(int cmd);
char *fread_action(FILE *fl);

#define MAX_SOCIAL  256

struct social_messg
{
    char *name;
    int hide;
    int min_victim_position; /* Position of victim */

    /* No argument was supplied */
    char *char_no_arg;
    char *others_no_arg;

    /* An argument was there, and a victim was found */
    char *char_found;       /* if NULL, read no further, ignore args */
    char *others_found;
    char *vict_found;

    /* An argument was there, but no victim was found */
    char *not_found;

    /* The victim turned out to be the character */
    char *char_auto;
    char *others_auto;
} soc_mess_list[MAX_SOCIAL];



struct pose_type
{
    int level;          /* minimum level for poser */
    char *poser_msg[4];  /* message to poser        */
    char *room_msg[4];   /* message to room         */
} pose_messages[MAX_MESSAGES];

static int list_top = -1;


char *fread_action(FILE *fl)
{
    char buf[MAX_STRING_LENGTH], *rslt;

    for (;;)
    {
	fgets(buf, MAX_STRING_LENGTH, fl);
	if (feof(fl))
	{
	    log("Fread_action - unexpected EOF.");
	    exit(0);
	}

	if (*buf == '#')
	    return(0);
	{
	    *(buf + strlen(buf) - 1) = '\0';
	    CREATE(rslt, char, strlen(buf) + 1);
	    strcpy(rslt, buf);
	    return(rslt);
	}
    }
}



void boot_social_messages(void)
{
    FILE *fl;
    char tmp[MAX_INPUT_LENGTH];
    int hide, min_pos;

    if (!(fl = fopen(SOCIAL_FILE, "r")))
    {
	perror("boot_social_messages");
	exit(0);
    }

    for (;;)
    {
	fscanf(fl, " %s ", tmp);
	if ( feof(fl) )
	    break;
	fscanf(fl, " %d ", &hide);
	fscanf(fl, " %d \n", &min_pos);

	list_top++;
	if ( list_top >= MAX_SOCIAL )
	{
	    perror( "Too many socials.\n" );
	    exit( 0 );
	}

	/* read the stuff */
	soc_mess_list[list_top].name                = str_dup(tmp);
	soc_mess_list[list_top].hide                = hide;
	soc_mess_list[list_top].min_victim_position = min_pos;
	soc_mess_list[list_top].char_no_arg         = fread_action(fl);
	soc_mess_list[list_top].others_no_arg       = fread_action(fl);
	soc_mess_list[list_top].char_found          = fread_action(fl);

	/* if no char_found, the rest is to be ignored */
	if (!soc_mess_list[list_top].char_found)
	    continue;

	soc_mess_list[list_top].others_found        = fread_action(fl); 
	soc_mess_list[list_top].vict_found          = fread_action(fl);
	soc_mess_list[list_top].not_found           = fread_action(fl);
	soc_mess_list[list_top].char_auto           = fread_action(fl);
	soc_mess_list[list_top].others_auto         = fread_action(fl);
    }
    fclose(fl);
}



bool check_social( struct char_data *ch, char *pcomm, int length, char *arg )
{
    char buf[MAX_INPUT_LENGTH];
    struct social_messg *action;
    struct char_data *vict;
    int cmd;

    for ( cmd = 0; cmd <= list_top; cmd++ )
    {
	if ( memcmp( pcomm, soc_mess_list[cmd].name, length ) == 0 )
	    goto LCmdFound;
    }
    return FALSE;

 LCmdFound:
    if ( !IS_NPC(ch) && IS_SET(ch->specials.act, PLR_NOEMOTE) )
    {
	send_to_char( "You are anti-social!\n\r", ch );
	return TRUE;
    }

    switch( GET_POS(ch) )
    {
    case POSITION_DEAD:
	send_to_char( "Lie still; you are DEAD.\n\r", ch );
	return TRUE;

    case POSITION_INCAP:
    case POSITION_MORTALLYW:
	send_to_char( "You are hurt far too bad for that.\n\r", ch );
	return TRUE;

    case POSITION_STUNNED:
	send_to_char( "You are too stunned to do that.\n\r", ch );
	return TRUE;

    case POSITION_SLEEPING:
	send_to_char( "In your dreams, or what?\n\r", ch );
	return TRUE;
    }

    action = &soc_mess_list[cmd];

    if (action->char_found)
	one_argument(arg, buf);
    else
	*buf = '\0';

    if (!*buf)
    {
	send_to_char(action->char_no_arg, ch);
	send_to_char("\n\r", ch);
	act(action->others_no_arg, action->hide, ch, 0, 0, TO_ROOM);
	return TRUE;
    }

    if (!(vict = get_char_room_vis(ch, buf)))
    {
	send_to_char(action->not_found, ch);
	send_to_char("\n\r", ch);
    }
    else if (vict == ch)
    {
	send_to_char(action->char_auto, ch);
	send_to_char("\n\r", ch);
	act(action->others_auto, action->hide, ch, 0, 0, TO_ROOM);
    }
    else if (GET_POS(vict) < action->min_victim_position)
    {
	act("$N is not in a proper position for that.",
	    FALSE,ch,0,vict,TO_CHAR);
    }
    else
    {
	act(action->char_found, 0, ch, 0, vict, TO_CHAR);
	act(action->others_found, action->hide, ch, 0, vict, TO_NOTVICT);
	act(action->vict_found, action->hide, ch, 0, vict, TO_VICT);
    }

    return TRUE;
}



void do_insult(struct char_data *ch, char *argument, int cmd)
{
    char buf[100];
    char arg[MAX_STRING_LENGTH];
    struct char_data *victim;

    one_argument(argument, arg);

    if(*arg) {
	if(!(victim = get_char_room_vis(ch, arg))) {
	    send_to_char("Can't hear you!\n\r", ch);
	} else {
	    if(victim != ch) { 
		sprintf(buf, "You insult %s.\n\r",GET_NAME(victim) );
		send_to_char(buf,ch);

		switch(number(0,2)) {
		    case 0 : {
			if (GET_SEX(ch) == SEX_MALE) {
			    if (GET_SEX(victim) == SEX_MALE)
				act(
				"$n accuses you of fighting like a woman!",
				    FALSE, ch, 0, victim, TO_VICT);
			else
				act("$n says that women can't fight.",
				  FALSE, ch, 0, victim, TO_VICT);
			} else { /* Ch == Woman */
			    if (GET_SEX(victim) == SEX_MALE)
				act(
			"$n accuses you of having the smallest.... (brain?)",
				FALSE, ch, 0, victim, TO_VICT );
			else
				act(
	    "$n tells you that you'd loose a beauty contest against a troll.",
				FALSE, ch, 0, victim, TO_VICT );
			}
		    } break;
		    case 1 : {
			act("$n calls your mother a bitch!",
			FALSE, ch, 0, victim, TO_VICT );
		    } break;
		    default : {
			act("$n tells you to get lost!",
			FALSE,ch,0,victim,TO_VICT);
		    } break;
		} /* end switch */

		act("$n insults $N.", TRUE, ch, 0, victim, TO_NOTVICT);
	    } else { /* ch == victim */
		send_to_char("You feel insulted.\n\r", ch);
	    }
	}
    } else send_to_char("Sure you don't want to insult everybody?\n\r", ch);
}



void boot_pose_messages(void)
{
  FILE *fl;
  int counter;
  int class;

  if (!(fl = fopen(POSEMESS_FILE, "r")))
  {
    perror("boot_pose_messages");
    exit(0);
  }

  for (counter = 0;;counter++)
  {
    fscanf(fl, " %d ", &pose_messages[counter].level);
    if (pose_messages[counter].level < 0)
      break;
	for (class = 0;class < 4;class++)
	{
	    pose_messages[counter].poser_msg[class] = fread_action(fl);
	    pose_messages[counter].room_msg[class] = fread_action(fl);
	}
    }
  
    fclose(fl);
}


void do_pose(struct char_data *ch, char *argument, int cmd)
{
    int to_pose;
    int counter;

    if ((GET_LEVEL(ch) < pose_messages[0].level) || IS_NPC(ch))
    {
	send_to_char("You can't do that.\n\r", ch);
	return;
    }

    for (counter = 0; (pose_messages[counter].level <= GET_LEVEL(ch)) && 
		     (pose_messages[counter].level >= 0); counter++);
    counter--;
  
    to_pose = number(0, counter);
    
    act(pose_messages[to_pose].poser_msg[GET_CLASS(ch)-1],
	0, ch, 0, 0, TO_CHAR);
    act(pose_messages[to_pose].room_msg[GET_CLASS(ch)-1],
   	 0, ch, 0, 0, TO_ROOM);
}
