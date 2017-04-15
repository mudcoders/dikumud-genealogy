/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	EotS 1.3 is copyright 1993-1996 Eric Orvedal and		*
*       Nathan Axtman							*
*	EotS has been brought to you by us, the merry drunk mudders	*
*	    Kjodo							*
*	    Torann							*
*	    Sledge				    			*
*	    Nicodemous						   	*
*	    Tom					   			*
*	By using this code, you have agreed to follow the terms of the	*
*	blood oath of the carungatang			   		*
*                                                                       *
*              FUBAR WAR CODE                         *
*       I think you will enjoy :)   (Ugh...480 lines [more now :)] )    *
*                                                                       *
***************************************************************************/




#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void do_declare(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  char arg4[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *d;
  CLAN_DATA    *pClan = NULL;
  CLAN_DATA	   *dClan= NULL;
  int num = 0;

  war.team_red = 0;
  war.team_blue = 0;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);
  argument = one_argument(argument, arg4);
  if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
  {
    send_to_char(C_DEFAULT, "Syntax: declare <type> <min_level> <max_level>\n\r", ch);
    return;
  }

  if (atoi(arg1) < 1 || atoi(arg1) > 3)
  {
    send_to_char(C_DEFAULT, "The type either has to be 1, 2, or 3.\n\r",
ch);
    return;
  }

  if (atoi(arg2) <= 0 || atoi(arg2) > 113)
  {
    send_to_char(C_DEFAULT, "Level must be between 1 and 113.\n\r", ch);
    return;
  }

  if (atoi(arg3) <= 0 || atoi(arg3) > 113)
  {
    send_to_char(C_DEFAULT, "Level must be between 1 and 113.\n\r", ch);
    return;
  }

  if (atoi(arg3) < atoi(arg2))
  {
    send_to_char(C_DEFAULT, "Max level must be greater than the min level.\n\r", ch);
    return;
  }

  if (war.iswar == TRUE)
  {
    send_to_char(C_DEFAULT, "There is already a war going!\n\r", ch);
    return;
  }

    if (atoi(arg1) == 3 && arg4[0] == '\0')
   {
	   send_to_char(C_DEFAULT, "You must supply a clan to challenge for clan war.\n\r", ch);
	   send_to_char(C_DEFAULT, "Use clans to find the appropriate clan's number.\n\r", ch);
	   return;
   }
  if (atoi(arg1) == 3)
  {
    num = is_number( arg4 ) ? atoi( arg4 ) : -1; 

    if ( num == -1 )
    {
     send_to_char( AT_WHITE, "No such clan exists, please try again.\n\r", ch );
     send_to_char( AT_WHITE, "Type clans to see available clan names.\n\r", ch );
     return;
    }

    if (!(pClan = get_clan_index(num)))
    {
     send_to_char( AT_WHITE, "Illegal clan number, please try again.\n\r", ch );
     send_to_char( AT_WHITE, "Type clans to see available clan numbers.\n\r", ch );
     return;
    }

	if (!(dClan = get_clan_index(ch->clan)))
	{
		send_to_char(C_DEFAULT, "You must be in a clan to make a clan war challenge.\n\r", ch);
		return;
	}

  }
  
  war.iswar = TRUE;
  war.wartype = atoi(arg1);
  war.min_level = atoi(arg2);
  war.max_level = atoi(arg3);
  if (war.wartype == 3)
  {
	  war.clan_chlng = ch->clan;
	  war.clan_accpt = num;
  }
if(war.wartype != 3)
  sprintf(buf, "&c%s &Rwar started for levels &Y%d &Rto &Y%d&R.  Type"
    "'WAR' to kill or be killed", war.wartype == 1 ? "Single" : "Team",
    war.min_level, war.max_level);
else
  sprintf(buf, "&Y%s &Rchallenges &Y%s&rto clan war. Levels &Y%d &Rto &Y%d&R. Type"
    "'WAR' to kill or be killed", dClan->name, pClan->name, war.min_level, war.max_level);
 
  info( buf, 0, 0 );
  war.timeleft = 5;
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (!IS_NPC(d->character))
    {
      if (IS_SET(d->character->act2, PLR_WAR))
        REMOVE_BIT(d->character->act2, PLR_WAR);
    }
  }

 }

void do_war(CHAR_DATA *ch, char *argument)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA *location;
  location = get_room_index(9);
  argument = one_argument(argument, arg);


  if (war.iswar != TRUE) 
  {
    send_to_char(C_DEFAULT, "There is no war going!\n\r", ch);
    return;
  }

  if (!str_cmp( arg, "reset"))
    {
	  if (war.timeleft < 1 && ch->level < 108)
	  {
		  send_to_char(C_DEFAULT, "You cannot reset a running war.\n\r", ch);
		  return;
	  }
      sprintf(buf, "War reset by an admin.");
      info(buf, 0, 0);
      war.iswar		= FALSE;
      war.timeleft	= 0;
      war.count		= 0;
      war.min_level = 0;
      war.max_level = 0;
      war.wartype	= 0;
	  war.inwar		= 0;
      for( d = descriptor_list; d != NULL; d = d->next )
	{
	  if (IS_SET(d->character->act2, PLR_WAR))
	    {
	      char_from_room(d->character);
	      char_to_room(d->character, (get_room_index(ROOM_VNUM_TEMPLE)));
	      do_look(d->character, "auto");
	    }
	}
      return;
    }

  if (ch->level < war.min_level || ch->level > war.max_level)
  {
    send_to_char(C_DEFAULT, "Sorry, you can't join this war.\n\r", ch);
    return;
  }

  if (IS_SET(ch->act2, PLR_WAR))
  {
    send_to_char(C_DEFAULT, "I don't think so.\n\r", ch);
    return;
  }

  if (war.wartype == 1)
  {
/*    if ((location = get_room_index(SINGLE_WAR_WAITING_ROOM)) == NULL)
    {
      send_to_char(C_DEFAULT, "Arena is not yet completed, sorry.\n\r", ch);
      return;
    }
    else
    { */
      act(C_DEFAULT, "$n has joined the war!", ch, NULL, NULL, TO_ROOM); 
      char_from_room(ch);
      char_to_room(ch, location);
      SET_BIT(ch->act2, PLR_WAR);
      sprintf(buf, "&c%s &R(&cLevel &Y%d&R) joins the war!", ch->name, ch->level);
      info(buf, 0, 0);
      act(C_DEFAULT, "$n arrives to get $s &Rass&w whipped!", ch, NULL, NULL, TO_ROOM);
      war.inwar++;
      do_look(ch, "auto");
      return;
/*    } */
  }
  if(war.wartype == 2)
  {
	  if( war.team_red <= war.team_blue )
	  {
      act(C_DEFAULT, "$n has joined the &Rred team&c!", ch, NULL, NULL, TO_ROOM); 
      char_from_room(ch);
      char_to_room(ch, location);
      SET_BIT(ch->act2, PLR_WAR);
	  SET_BIT(ch->act2, TEAM_RED);
      sprintf(buf, "&c%s &R(&cLevel &Y%d&R) joins the &Rred team&c!", ch->name, ch->level);
      info(buf, 0, 0);
      act(C_DEFAULT, "$n arrives to get $s &Rass&w whipped!", ch, NULL, NULL, TO_ROOM);
      war.inwar++;
	  war.team_red++;
      do_look(ch, "auto");
      return;
	  }
	  else
	  {
		  act(C_DEFAULT, "$n has joined the &Bblue team&c!", ch, NULL, NULL, TO_ROOM); 
      char_from_room(ch);
      char_to_room(ch, location);
      SET_BIT(ch->act2, PLR_WAR);
	  SET_BIT(ch->act2, TEAM_BLUE);
      sprintf(buf, "&c%s &R(&cLevel &Y%d&R) joins the &Bblue team&c!", ch->name, ch->level);
      info(buf, 0, 0);
      act(C_DEFAULT, "$n arrives to get $s &Rass&w whipped!", ch, NULL, NULL, TO_ROOM);
      war.inwar++;
	  war.team_blue++;
      do_look(ch, "auto");
	  return;
	  }
  }
  if(war.wartype == 3)
  {
	  if( ch->clan == war.clan_chlng )
	  {
      act(C_DEFAULT, "$n has joined the &Rred team&c!", ch, NULL, NULL, TO_ROOM); 
      char_from_room(ch);
      char_to_room(ch, location);
      SET_BIT(ch->act2, PLR_WAR);
	  SET_BIT(ch->act2, TEAM_RED);
      sprintf(buf, "&c%s &R(&cLevel &Y%d&R) joins the &Rred team&c!", ch->name, ch->level);
      info(buf, 0, 0);
      act(C_DEFAULT, "$n arrives to get $s &Rass&w whipped!", ch, NULL, NULL, TO_ROOM);
      war.inwar++;
	  war.team_red++;
      do_look(ch, "auto");
      return;
	  }
	  else if( ch->clan == war.clan_accpt )
	  {
		  act(C_DEFAULT, "$n has joined the &Bblue team&c!", ch, NULL, NULL, TO_ROOM); 
      char_from_room(ch);
      char_to_room(ch, location);
      SET_BIT(ch->act2, PLR_WAR);
	  SET_BIT(ch->act2, TEAM_BLUE);
      sprintf(buf, "&c%s &R(&cLevel &Y%d&R) joins the &Bblue team&c!", ch->name, ch->level);
      info(buf, 0, 0);
      act(C_DEFAULT, "$n arrives to get $s &Rass&w whipped!", ch, NULL, NULL, TO_ROOM);
      war.inwar++;
	  war.team_blue++;
      do_look(ch, "auto");
	  return;
	  }
	  else
	  {
  	  send_to_char(C_DEFAULT, "You cannot join this war. You are not in the appropriate clan.\n\r", ch );
	  return;
	  }
  }
}


void war_update(void)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  ROOM_INDEX_DATA *random;
  
  if (war.timeleft > 0)
  {
    sprintf(buf, "%d tick%s left to join the war.", war.timeleft, war.timeleft == 1 ? "" : "s");
    info(buf, 0, 0);
    sprintf(buf, "%d %s %s fighting in the war, so far.", war.inwar, war.inwar == 1 ? "person" : "people", war.inwar == 1 ? "is" : "are");
    info(buf, 0, 0);
    sprintf(buf, "Type of war: &Y%d &R- &Y%d&R, &C%s war&R.&w", war.min_level, war.max_level, war.wartype == 1 ? "Single" : "Team");
    info(buf, 0, 0);
    war.timeleft--;
    return;
  }

  if (war.timeleft == 0 && war.iswar == TRUE && war.count == 0)
  {  
    if (war.inwar == 0 || war.inwar == 1)
    {
      sprintf(buf, "Not enough people for war.  War reset.");
      info(buf, 0, 0);
      war.iswar = FALSE;
      war.timeleft = 0;
      war.count = 0;
      war.min_level = 0;
      war.max_level = 0;
      war.wartype = 0;
      for(d = descriptor_list; d != NULL; d = d->next)
      {
        if (IS_SET(d->character->act2, PLR_WAR))
        {
          char_from_room(d->character);
          char_to_room(d->character, (get_room_index(ROOM_VNUM_TEMPLE)));
          do_look(d->character, "auto");
        }
      }
    }
    else
    {
      sprintf(buf, "The battle begins! %d players are fighting!", war.inwar);
      info(buf, 0, 0);
      war.count = 20;
      for(d = descriptor_list; d != NULL; d = d->next)
      {
        if (IS_SET(d->character->act2, PLR_WAR))
        {
          random = get_room_index(number_range(7350, 7365));
          char_from_room(d->character);
          char_to_room(d->character, random);
          do_look(d->character, "auto");
        }
      }
    }
  }
  return;
}
