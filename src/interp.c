/***************************************************************************
 *  file: interp.c , Command interpreter module.      Part of DIKUMUD      *
 *  Usage: Procedures interpreting user command                            *
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
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <memory.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "interp.h"
#include "db.h"
#include "utils.h"
#include "limits.h"

extern bool check_social( struct char_data *ch, char *pcomm,
    int length, char *arg );

extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct room_data *world;

struct command_info
{
    char *command_name;             /* Name of this command             */
    DO_FUN *command_pointer;        /* Function that does it            */
    byte minimum_position;          /* Position commander must be in    */
    byte minimum_level;             /* Minimum level needed             */
    ubyte command_number;           /* Passed to function as argument   */
};

/*
 * Note that command number is always non-zero for user commands.
 * Various spec_proc.c functions impose ICKY requirements.
 * Someday I'll NUKE the arg and pass the command name instead.
 *
 * Spec_procs that need fixing:
 *	shop_keeper
 */
struct command_info cmd_info[] =
{
    /*
     * Common movement commands.
     */
    { "north",      do_move,        POSITION_STANDING,  0,  1 },
    { "east",       do_move,        POSITION_STANDING,  0,  2 },
    { "south",      do_move,        POSITION_STANDING,  0,  3 },
    { "west",       do_move,        POSITION_STANDING,  0,  4 },
    { "up",         do_move,        POSITION_STANDING,  0,  5 },
    { "down",       do_move,        POSITION_STANDING,  0,  6 },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "cast",       do_cast,        POSITION_SITTING,   0,  9 },
    { "exits",      do_exits,       POSITION_RESTING,   0,  9 },
    { "get",        do_get,         POSITION_RESTING,   0,  9 },
    { "inventory",  do_inventory,   POSITION_DEAD,      0,  9 },
    { "kill",       do_kill,        POSITION_FIGHTING,  0,  9 },
    { "look",       do_look,        POSITION_RESTING,   0,  9 },
    { "order",      do_order,       POSITION_RESTING,   0,  9 },
    { "rest",       do_rest,        POSITION_RESTING,   0,  9 },
    { "stand",      do_stand,       POSITION_RESTING,   0,  9 },
    { "tell",       do_tell,        POSITION_RESTING,   0,  9 },
    { "wield",      do_wield,       POSITION_RESTING,   0,  9 },
    { "wizhelp",    do_wizhelp,     POSITION_DEAD,      31, 9 },

    /*
     * Informational commands.
     */
    { "brief",      do_brief,       POSITION_DEAD,      0,  9 },
    { "bug",        do_bug,         POSITION_DEAD,      0,  9 },
    { "compact",    do_compact,     POSITION_DEAD,      0,  9 },
    { "credits",    do_credits,     POSITION_DEAD,      0,  9 },
    { "equipment",  do_equipment,   POSITION_DEAD,      0,  9 },
    { "help",       do_help,        POSITION_DEAD,      0,  9 },
    { "idea",       do_idea,        POSITION_DEAD,      0,  9 },
    { "info",       do_info,        POSITION_DEAD,      0,  9 },
/*  { "inventory",  do_inventory,   POSITION_DEAD,      0,  9 },    */
    { "levels",     do_levels,      POSITION_DEAD,      0,  9 },
    { "news",       do_news,        POSITION_DEAD,      0,  9 },
    { "score",      do_score,       POSITION_DEAD,      0,  9 },
    { "story",      do_story,       POSITION_DEAD,      0,  9 },
    { "tick",       do_tick,        POSITION_DEAD,      0,  9 },
    { "time",       do_time,        POSITION_DEAD,      0,  9 },
    { "title",      do_title,       POSITION_DEAD,      0,  9 },
    { "typo",       do_typo,        POSITION_DEAD,      0,  9 },
    { "weather",    do_weather,     POSITION_DEAD,      0,  9 },
    { "who",        do_who,         POSITION_DEAD,      0,  9 },
    { "wizlist",    do_wizlist,     POSITION_DEAD,      0,  9 },

    /*
     * Communication commands.
     */
    { "ask",        do_ask,         POSITION_RESTING,   0,  9 },
    { "emote",      do_emote,       POSITION_RESTING,   0,  9 },
    { ",",          do_emote,       POSITION_RESTING,   0,  9 },    
    { "gtell",      do_grouptell,   POSITION_DEAD,      0,  9 },
    { ";",          do_grouptell,   POSITION_DEAD,      0,  9 },
    { "insult",     do_insult,      POSITION_RESTING,   0,  9 },
/*  { "order",      do_order,       POSITION_RESTING,   0,  9 },    */
    { "pose",       do_pose,        POSITION_RESTING,   0,  9 },
    { "report",     do_report,      POSITION_DEAD,      0,  9 },
    { "say",        do_say,         POSITION_RESTING,   0,  9 },
    { "'",          do_say,         POSITION_RESTING,   0,  9 },
    { "shout",      do_shout,       POSITION_RESTING,   0,  9 },
/*  { "tell",       do_tell,        POSITION_RESTING,   0,  9 },    */
    { "whisper",    do_whisper,     POSITION_RESTING,   0,  9 },

    /*
     * Object manipulation commands.
     */
    { "close",      do_close,       POSITION_RESTING,   0,  9 },
    { "drink",      do_drink,       POSITION_RESTING,   0,  9 },
    { "drop",       do_drop,        POSITION_RESTING,   0,  9 },
    { "eat",        do_eat,         POSITION_RESTING,   0,  9 },
    { "fill",       do_fill,        POSITION_RESTING,   0,  9 },
/*  { "get",        do_get,         POSITION_RESTING,   0,  9 },    */
    { "give",       do_give,        POSITION_RESTING,   0,  9 },
    { "grab",       do_grab,        POSITION_RESTING,   0,  9 },
    { "hold",       do_grab,        POSITION_RESTING,   0,  9 },
    { "lock",       do_lock,        POSITION_RESTING,   0,  9 },
    { "open",       do_open,        POSITION_RESTING,   0,  9 },
    { "pour",       do_pour,        POSITION_RESTING,   0,  9 },
    { "put",        do_put,         POSITION_RESTING,   0,  9 },
    { "quaff",      do_quaff,       POSITION_RESTING,   0,  9 },
    { "read",       do_read,        POSITION_RESTING,   0,  9 },
    { "recite",     do_recite,      POSITION_RESTING,   0,  9 },
    { "remove",     do_remove,      POSITION_RESTING,   0,  9 },
    { "sip",        do_sip,         POSITION_RESTING,   0,  9 },
    { "take",       do_get,         POSITION_RESTING,   0,  9 },
    { "junk",       do_tap,         POSITION_RESTING,   0,  9 },
    { "sacrifice",  do_tap,         POSITION_RESTING,   0,  9 },
    { "tap",        do_tap,         POSITION_RESTING,   0,  9 },
    { "taste",      do_taste,       POSITION_RESTING,   0,  9 },
    { "unlock",     do_unlock,      POSITION_RESTING,   0,  9 },
    { "use",        do_use,         POSITION_RESTING,   0,  9 },
    { "wear",       do_wear,        POSITION_RESTING,   0,  9 },
/*  { "wield",      do_wield,       POSITION_RESTING,   0,  9 },    */

    /*
     * Combat commands.
     */
    { "bash",       do_bash,        POSITION_FIGHTING,  0,  9 },
    { "disarm",     do_disarm,      POSITION_FIGHTING,  0,  9 },
    { "flee",       do_flee,        POSITION_FIGHTING,  0,  9 },
    { "hit",        do_hit,         POSITION_FIGHTING,  0,  9 },
    { "kick",       do_kick,        POSITION_FIGHTING,  0,  9 },
/*  { "kill",       do_kill,        POSITION_FIGHTING,  0,  9 },    */
    { "murder",     do_murder,      POSITION_FIGHTING,  5,  9 },
    { "rescue",     do_rescue,      POSITION_FIGHTING,  0,  9 },
    { "trip",       do_trip,        POSITION_FIGHTING,  0,  9 },

    /*
     * Position commands.
     */
/*  { "rest",       do_rest,        POSITION_RESTING,   0,  9 },    */
    { "sit",        do_sit,         POSITION_RESTING,   0,  9 },
    { "sleep",      do_sleep,       POSITION_SLEEPING,  0,  9 },
/*  { "stand",      do_stand,       POSITION_RESTING,   0,  9 },    */
    { "wake",       do_wake,        POSITION_SLEEPING,  0,  9 },

    /*
     * Miscellaneous commands.
     */
    { "backstab",   do_backstab,    POSITION_STANDING,  0,  9 },
    { "bs",         do_backstab,    POSITION_STANDING,  0,  9 },
/*  { "cast",       do_cast,        POSITION_SITTING,   0,  9 },    */
    { "consider",   do_consider,    POSITION_RESTING,   0,  9 },
    { "enter",      do_enter,       POSITION_STANDING,  0,  9 },
    { "examine",    do_examine,     POSITION_RESTING,   0,  9 },
/*  { "exits",      do_exits,       POSITION_RESTING,   0,  9 },    */
    { "follow",     do_follow,      POSITION_RESTING,   0,  9 },
    { "group",      do_group,       POSITION_RESTING,   0,  9 },
    { "hide",       do_hide,        POSITION_RESTING,   0,  9 },
    { "leave",      do_leave,       POSITION_STANDING,  0,  9 },
/*  { "look",       do_look,        POSITION_RESTING,   0,  9 },    */
    { "pick",       do_pick,        POSITION_STANDING,  0,  9 },
    { "qui",        do_qui,         POSITION_DEAD,      0,  9 },
    { "quit",       do_quit,        POSITION_DEAD,      0,  9 },
    { "recall",     do_recall,      POSITION_FIGHTING,  0,  9 },
    { "/",          do_recall,      POSITION_FIGHTING,  0,  9 },
    { "return",     do_return,      POSITION_DEAD,      0,  9 },
    { "save",       do_save,        POSITION_DEAD,      0,  9 },
    { "sneak",      do_sneak,       POSITION_STANDING,  1,  9 },
    { "split",      do_split,       POSITION_RESTING,   0,  9 },
    { "steal",      do_steal,       POSITION_STANDING,  1,  9 },
    { "where",      do_where,       POSITION_RESTING,   0,  9 },
    { "wimpy",      do_wimpy,       POSITION_DEAD,      0,  9 },
    { "write",      do_write,       POSITION_STANDING,  0,  9 },

    /*
     * Special procedure commands.
     */
    { "buy",        do_not_here,    POSITION_STANDING,  0,  56 },
    { "sell",       do_not_here,    POSITION_STANDING,  0,  57 },
    { "value",      do_not_here,    POSITION_STANDING,  0,  58 },
    { "list",       do_not_here,    POSITION_STANDING,  0,  59 },
    { "practice",   do_practice,    POSITION_RESTING,   1,  164 },
    { "practise",   do_practice,    POSITION_RESTING,   1,  164 },
    { "train",      do_not_here,    POSITION_RESTING,   1,  165 },

    /*
     * Immortal commands.
     */
    { "advance",    do_advance,     POSITION_DEAD,      35, 9 },

    { "allow",      do_allow,       POSITION_DEAD,      34, 9 },
    { "ban",        do_ban,         POSITION_DEAD,      34, 9 },
    { "disconnect", do_disconnect,  POSITION_DEAD,      34, 9 },
    { "freeze",     do_freeze,      POSITION_DEAD,      34, 9 },
    { "log",        do_log,         POSITION_DEAD,      34, 9 },
    { "purge",      do_purge,       POSITION_DEAD,      34, 9 },
    { "reroll",     do_reroll,      POSITION_DEAD,      34, 9 },
    { "reset",      do_reroll,      POSITION_DEAD,      34, 9 },
    { "set",        do_set,         POSITION_DEAD,      34, 9 },
    { "shutdow",    do_shutdow,     POSITION_DEAD,      34, 9 },
    { "shutdown",   do_shutdown,    POSITION_DEAD,      34, 9 },
    { "sockets",    do_sockets,     POSITION_DEAD,      34, 9 },
    { "string",     do_string,      POSITION_DEAD,      34, 9 },
    { "wizlock",    do_wizlock,     POSITION_DEAD,      34, 9 },

    { "force",      do_force,       POSITION_DEAD,      33, 9 },
    { "load",       do_load,        POSITION_DEAD,      33, 9 },
    { "noemote",    do_noemote,     POSITION_DEAD,      33, 9 },
    { "nosocial",   do_noemote,     POSITION_DEAD,      33, 9 },
    { "noshout",    do_noshout,     POSITION_DEAD,      33, 9 },
    { "notell",     do_notell,      POSITION_DEAD,      33, 9 },
    { "pardon",     do_pardon,      POSITION_DEAD,      33, 9 },
    { "restore",    do_restore,     POSITION_DEAD,      33, 9 },
    { "teleport",   do_teleport,    POSITION_DEAD,      33, 9 },
    { "trans",      do_trans,       POSITION_DEAD,      33, 9 },

    { "at",         do_at,          POSITION_DEAD,      32, 9 },
    { "echo",       do_echo,        POSITION_DEAD,      32, 9 },
    { "goto",       do_goto,        POSITION_DEAD,      32, 9 },
    { "snoop",      do_snoop,       POSITION_DEAD,      32, 9 },
    { "stat",       do_stat,        POSITION_DEAD,      32, 9 },
    { "switch",     do_switch,      POSITION_DEAD,      32, 9 },
    { "invis",      do_wizinvis,    POSITION_DEAD,      32, 9 },
    { "wizinvis",   do_wizinvis,    POSITION_DEAD,      32, 9 },

    { "holylite",   do_holylite,    POSITION_DEAD,      31, 9 },
    { "immortal",   do_wiz,         POSITION_DEAD,      31, 9 },
    { ":",          do_wiz,         POSITION_DEAD,      31, 9 },
/*  { "wizhelp",    do_wizhelp,     POSITION_DEAD,      31, 9 },    */

    /*
     * End of list.
     */
    { "",           do_not_here,    POSITION_DEAD,      0,  9 }
};



char *fill[]=
{
    "in", "from", "with", "the", "on", "at", "to", "\n"
};



void command_interpreter( struct char_data *ch, char *pcomm )
{
    int look_at;
    int cmd;

    /*
     * No hiding.
     */
    REMOVE_BIT( ch->specials.affected_by, AFF_HIDE );

    /*
     * Log players.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->specials.act, PLR_LOG) )
    {
	sprintf( log_buf, "Log %s: %s", GET_NAME(ch), pcomm );
	log( log_buf );
    }

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->specials.act, PLR_FREEZE) )
    {
	send_to_char( "You're totally frozen!\n\r", ch );
	return;
    }

    /*
     * Strip initial spaces and parse command word.
     * Translate to lower case.
     */
    while ( *pcomm == ' ' )
	pcomm++;
    
    for ( look_at = 0; pcomm[look_at] > ' '; look_at++ )
	pcomm[look_at]  = LOWER(pcomm[look_at]);

    if ( look_at == 0 )
	return;
    
    /*
     * Look for command in command table.
     */
    for ( cmd = 0; cmd < sizeof(cmd_info)/sizeof(cmd_info[0]); cmd++ )
    {
	if ( GET_LEVEL(ch) < cmd_info[cmd].minimum_level )
	    continue;
	if ( cmd_info[cmd].command_pointer == NULL )
	    continue;
	if ( memcmp( pcomm, cmd_info[cmd].command_name, look_at ) == 0 )
	    goto LCmdFound;
    }

    /*
     * Look for command in socials table.
     */
    if ( check_social( ch, pcomm, look_at, &pcomm[look_at] ) )
	return;

    /*
     * Unknown command (or char too low level).
     */
    send_to_char( "Huh?\n\r", ch );
    return;

 LCmdFound:
    /*
     * Character not in position for command?
     */
    if ( GET_POS(ch) < cmd_info[cmd].minimum_position )
    {
	switch( GET_POS(ch) )
	{
	case POSITION_DEAD:
	    send_to_char( "Lie still; you are DEAD.\n\r", ch );
	    break;
	case POSITION_INCAP:
	case POSITION_MORTALLYW:
	    send_to_char( "You are hurt far too bad for that.\n\r", ch );
	    break;
	case POSITION_STUNNED:
	    send_to_char( "You are too stunned to do that.\n\r", ch );
	    break;
	case POSITION_SLEEPING:
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    break;
	case POSITION_RESTING:
	    send_to_char( "Nah... You feel too relaxed...\n\r", ch);
	    break;
	case POSITION_SITTING:
	    send_to_char( "Maybe you should stand up first?\n\r", ch);
	    break;
	case POSITION_FIGHTING:
	    send_to_char( "No way!  You are still fighting!\n\r", ch);
	    break;
	}
	return;
    }

    /*
     * We're gonna execute it.
     * First look for usable special procedure.
     */
    if ( special( ch, cmd_info[cmd].command_number, &pcomm[look_at] ) )
	return;
      
    /*
     * Normal dispatch.
     */
    (*cmd_info[cmd].command_pointer)
	(ch, &pcomm[look_at], cmd_info[cmd].command_number);

    /*
     * This call is here to prevent gcc from tail-chaining the
     * previous call, which screws up the debugger call stack.
     * -- Furey
     */
    number( 0, 0 );
    return;
}



int search_block(char *arg, char **list, bool exact)
{
    register int i,l;

    /* Make into lower case, and get length of string */
    for(l=0; *(arg+l); l++)
	*(arg+l)=LOWER(*(arg+l));

    if (exact) {
	for(i=0; **(list+i) != '\n'; i++)
	    if (!strcmp(arg, *(list+i)))
		return(i);
    } else {
	if (!l)
	    l=1; /* Avoid "" to match the first available string */
	for(i=0; **(list+i) != '\n'; i++)
	    if (!strncmp(arg, *(list+i), l))
		return(i);
    }

    return(-1);
}


int old_search_block(char *argument,int begin,int length,char **list,int mode)
{
    int guess, found, search;
	
    /* If the word contain 0 letters, then a match is already found */
    found = (length < 1);

    guess = 0;

    /* Search for a match */

    if(mode)
    while ( !found && *(list[guess]) != '\n' )
    {
	found = (length==strlen(list[guess]));
	for ( search = 0; search < length && found; search++ )
	    found=(*(argument+begin+search)== *(list[guess]+search));
	guess++;
    } else {
	while ( !found && *(list[guess]) != '\n' ) {
	    found=1;
	    for(search=0;( search < length && found );search++)
		found=(*(argument+begin+search)== *(list[guess]+search));
	    guess++;
	}
    }

    return ( found ? guess : -1 ); 
}



void argument_interpreter(char *argument,char *first_arg,char *second_arg )
{
    int look_at, found, begin;

    found = begin = 0;

    do
    {
	/* Find first non blank */
	for ( ;*(argument + begin ) == ' ' ; begin++);

	/* Find length of first word */
	for ( look_at=0; *(argument+begin+look_at)> ' ' ; look_at++)

		/* Make all letters lower case,
		   and copy them to first_arg */
		*(first_arg + look_at) =
		LOWER(*(argument + begin + look_at));

	*(first_arg + look_at)='\0';
	begin += look_at;

    }
    while( fill_word(first_arg));

    do
    {
	/* Find first non blank */
	for ( ;*(argument + begin ) == ' ' ; begin++);

	/* Find length of first word */
	for ( look_at=0; *(argument+begin+look_at)> ' ' ; look_at++)

		/* Make all letters lower case,
		   and copy them to second_arg */
		*(second_arg + look_at) =
		LOWER(*(argument + begin + look_at));

	*(second_arg + look_at)='\0';
	begin += look_at;
    }
    while( fill_word(second_arg));
}



int is_number(char *str)
{
    int look_at;

    if(*str=='\0')
	return(0);

    for(look_at=0;*(str+look_at) != '\0';look_at++)
	if((*(str+look_at)<'0')||(*(str+look_at)>'9'))
	    return(0);
    return(1);
}


/* find the first sub-argument of a string, return pointer to first char in
   primary argument, following the sub-arg                      */
char *one_argument(char *argument, char *first_arg )
{
    int found, begin, look_at;

	found = begin = 0;

	do
	{
		/* Find first non blank */
		for ( ;isspace(*(argument + begin)); begin++);

		/* Find length of first word */
		for (look_at=0; *(argument+begin+look_at) > ' ' ; look_at++)

			/* Make all letters lower case,
			   and copy them to first_arg */
			*(first_arg + look_at) =
			LOWER(*(argument + begin + look_at));

		*(first_arg + look_at)='\0';
	begin += look_at;
    }
	while (fill_word(first_arg));

    return(argument+begin);
}
    


int fill_word(char *argument)
{
    return ( search_block(argument,fill,TRUE) >= 0);
}



/* determine if a given string is an abbreviation of another */
int is_abbrev(char *arg1, char *arg2)
{
    if (!*arg1)
       return(0);

    for (; *arg1; arg1++, arg2++)
       if (LOWER(*arg1) != LOWER(*arg2))
	  return(0);

    return(1);
}




/* return first 'word' plus trailing substring of input string */
void half_chop(char *string, char *arg1, char *arg2)
{
    for (; isspace(*string); string++);

    for (; !isspace(*arg1 = *string) && *string; string++, arg1++);

    *arg1 = '\0';

    for (; isspace(*string); string++);

    for (; ( *arg2 = *string ) != '\0'; string++, arg2++)
	;
}



int special(struct char_data *ch, int cmd, char *arg)
{
    register struct obj_data *i;
    register struct char_data *k;
    int j;

    /* special in room? */
    if (world[ch->in_room].funct)
       if ((*world[ch->in_room].funct)(ch, cmd, arg))
	  return(1);

    /* special in equipment list? */
    for (j = 0; j <= (MAX_WEAR - 1); j++)
       if (ch->equipment[j] && ch->equipment[j]->item_number>=0)
	  if (obj_index[ch->equipment[j]->item_number].func)
	     if ((*obj_index[ch->equipment[j]->item_number].func)
		(ch, cmd, arg))
		return(1);

    /* special in inventory? */
    for (i = ch->carrying; i; i = i->next_content)
	if (i->item_number>=0)
	    if (obj_index[i->item_number].func)
	   if ((*obj_index[i->item_number].func)(ch, cmd, arg))
	      return(1);

    /* special in mobile present? */
    for (k = world[ch->in_room].people; k; k = k->next_in_room)
       if ( IS_MOB(k) )
	  if (mob_index[k->nr].func)
	     if ((*mob_index[k->nr].func)(ch, cmd, arg))
		return(1);

    /* special in object present? */
    for (i = world[ch->in_room].contents; i; i = i->next_content)
       if (i->item_number>=0)
	  if (obj_index[i->item_number].func)
	     if ((*obj_index[i->item_number].func)(ch, cmd, arg))
		return(1);


    return(0);
}



void do_wizhelp(struct char_data *ch, char *argument, int cmd_arg)
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int no;

    if (IS_NPC(ch))
	return;

    send_to_char(
	"The following privileged comands are available:\n\r\n\r", ch);

    buf[0] = '\0';
    for ( cmd = 0, no = 0; cmd_info[cmd].command_name[0] != '\0'; cmd++ )
    {
	if ( cmd_info[cmd].minimum_level <= 30 )
	    continue;
	if ( cmd_info[cmd].minimum_level > GET_LEVEL(ch) )
	    continue;

	sprintf( buf + strlen(buf), "%-10s", cmd_info[cmd].command_name );
	if ( no % 7 == 0 )
	    strcat(buf, "\n\r");
	no++;
    }

    strcat(buf, "\n\r");
    page_string(ch->desc, buf, 1);
}
