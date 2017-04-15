/* ************************************************************************
*   File: interpreter.c                                 Part of CircleMUD *
*  Usage: parse user commands, search for specials, call ACMD functions   *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "structs.h"
#include "comm.h"
#include "interpreter.h"
#include "db.h"
#include "utils.h"
#include "limits.h"
#include "spells.h"
#include "handler.h"
#include "mail.h"
#include "screen.h"

#define COMMANDO(number,min_pos,pointer,min_level,subcommand) {      \
	cmd_info[(number)].command_pointer = (pointer);   \
	cmd_info[(number)].minimum_position = (min_pos);  \
	cmd_info[(number)].minimum_level = (min_level); \
	cmd_info[(number)].subcmd = (subcommand); }


extern const struct title_type titles[4][35];
extern char	*motd;
extern char	*imotd;
extern char	*background;
extern char	*MENU;
extern char	*WELC_MESSG;
extern char	*START_MESSG;
extern struct char_data *character_list;
extern struct player_index_element *player_table;
extern int	top_of_p_table;
extern int	restrict;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct room_data *world;
struct command_info cmd_info[MAX_CMD_LIST];

/* external functions */
void	echo_on(int sock);
void	echo_off(int sock);
void	set_title(struct char_data *ch);
void	do_start(struct char_data *ch);
void	init_char(struct char_data *ch);
int	create_entry(char *name);
int	special(struct char_data *ch, int cmd, char *arg);
int	isbanned(char *hostname);
int	Valid_Name(char *newname);


/* prototypes for all do_x functions. */
ACMD(do_move);
ACMD(do_look);
ACMD(do_read);
ACMD(do_say);
ACMD(do_exit);
ACMD(do_snoop);
ACMD(do_insult);
ACMD(do_quit);
ACMD(do_help);
ACMD(do_who);
ACMD(do_emote);
ACMD(do_echo);
ACMD(do_trans);
ACMD(do_kill);
ACMD(do_stand);
ACMD(do_sit);
ACMD(do_rest);
ACMD(do_sleep);
ACMD(do_wake);
ACMD(do_force);
ACMD(do_get);
ACMD(do_drop);
ACMD(do_score);
ACMD(do_inventory);
ACMD(do_equipment);
ACMD(do_not_here);
ACMD(do_tell);
ACMD(do_wear);
ACMD(do_wield);
ACMD(do_grab);
ACMD(do_remove);
ACMD(do_put);
ACMD(do_shutdown);
ACMD(do_save);
ACMD(do_hit);
ACMD(do_string);
ACMD(do_give);
ACMD(do_stat);
ACMD(do_skillset);
ACMD(do_set);
ACMD(do_time);
ACMD(do_weather);
ACMD(do_load);
ACMD(do_vstat);
ACMD(do_purge);
ACMD(do_whisper);
ACMD(do_cast);
ACMD(do_at);
ACMD(do_goto);
ACMD(do_ask);
ACMD(do_drink);
ACMD(do_eat);
ACMD(do_pour);
ACMD(do_order);
ACMD(do_follow);
ACMD(do_rent);
ACMD(do_offer);
ACMD(do_advance);
ACMD(do_close);
ACMD(do_open);
ACMD(do_lock);
ACMD(do_unlock);
ACMD(do_exits);
ACMD(do_enter);
ACMD(do_leave);
ACMD(do_write);
ACMD(do_flee);
ACMD(do_sneak);
ACMD(do_hide);
ACMD(do_backstab);
ACMD(do_pick);
ACMD(do_steal);
ACMD(do_bash);
ACMD(do_rescue);
ACMD(do_kick);
ACMD(do_examine);
ACMD(do_info);
ACMD(do_users);
ACMD(do_where);
ACMD(do_levels);
ACMD(do_consider);
ACMD(do_group);
ACMD(do_restore);
ACMD(do_return);
ACMD(do_switch);
ACMD(do_quaff);
ACMD(do_recite);
ACMD(do_use);
ACMD(do_credits);
ACMD(do_display);
ACMD(do_poofset);
ACMD(do_teleport);
ACMD(do_gecho);
ACMD(do_wiznet);
ACMD(do_invis);
ACMD(do_wimpy);
ACMD(do_wizlock);
ACMD(do_dc);
ACMD(do_gsay);
ACMD(do_title);
ACMD(do_visible);
ACMD(do_assist);
ACMD(do_split);
ACMD(do_toggle);
ACMD(do_send);
ACMD(do_vnum);
ACMD(do_action);
ACMD(do_practice);
ACMD(do_uptime);
ACMD(do_commands);
ACMD(do_ban);
ACMD(do_unban);
ACMD(do_date);
ACMD(do_zreset);
ACMD(do_gen_write);
ACMD(do_gen_ps);
ACMD(do_gen_tog);
ACMD(do_gen_com);
ACMD(do_wizutil);
ACMD(do_color);
ACMD(do_syslog);
ACMD(do_show);
ACMD(do_ungroup);
ACMD(do_report);
ACMD(do_page);
ACMD(do_diagnose);
ACMD(do_qsay);
ACMD(do_reboot);
ACMD(do_last);
ACMD(do_track);

char	*command[] = 
{
   "north", 		/* 1 */
   "east",
   "south",
   "west",
   "up",
   "down",
   "enter",
   "exits",
   "kiss",
   "get",
   "drink", 		/* 11 */
   "eat",
   "wear",
   "wield",
   "look",
   "score",
   "say",
   "shout",
   "tell",
   "inventory",
   "qui", 		/* 21 */
   "bounce",
   "smile",
   "dance",
   "kill",
   "cackle",
   "laugh",
   "giggle",
   "shake",
   "puke",
   "growl", 		/* 31 */
   "scream",
   "insult",
   "comfort",
   "nod",
   "sigh",
   "sulk",
   "help",
   "who",
   "emote",
   "echo", 		/* 41 */
   "stand",
   "sit",
   "rest",
   "sleep",
   "wake",
   "force",
   "transfer",
   "hug",
   "snuggle",
   "cuddle", 		/* 51 */
   "nuzzle",
   "cry",
   "news",
   "equipment",
   "buy",
   "sell",
   "value",
   "list",
   "drop",
   "goto", 		/* 61 */
   "weather",
   "read",
   "pour",
   "grab",
   "remove",
   "put",
   "shutdow",
   "save",
   "hit",
   "string", 		/* 71 */
   "give",
   "quit",
   "stat",
   "skillset",
   "time",
   "load",
   "purge",
   "shutdown",
   "idea",
   "typo", 		/* 81 */
   "bug",
   "whisper",
   "cast",
   "at",
   "ask",
   "order",
   "sip",
   "taste",
   "snoop",
   "follow", 		/* 91 */
   "rent",
   "offer",
   "poke",
   "advance",
   "accuse",
   "grin",
   "bow",
   "open",
   "close",
   "lock", 		/* 101 */
   "unlock",
   "leave",
   "applaud",
   "blush",
   "burp",
   "chuckle",
   "clap",
   "cough",
   "curtsey",
   "fart", 		/* 111 */
   "flip",
   "fondle",
   "frown",
   "gasp",
   "glare",
   "groan",
   "grope",
   "hiccup",
   "lick",
   "love", 		/* 121 */
   "moan",
   "nibble",
   "pout",
   "purr",
   "ruffle",
   "shiver",
   "shrug",
   "sing",
   "slap",
   "smirk", 		/* 131 */
   "snap",
   "sneeze",
   "snicker",
   "sniff",
   "snore",
   "spit",
   "squeeze",
   "stare",
   "strut",
   "thank", 		/* 141 */
   "twiddle",
   "wave",
   "whistle",
   "wiggle",
   "wink",
   "yawn",
   "snowball",
   "write",
   "hold",
   "flee", 		/* 151 */
   "sneak",
   "hide",
   "backstab",
   "pick",
   "steal",
   "bash",
   "rescue",
   "kick",
   "french",
   "comb", 		/* 161 */
   "massage",
   "tickle",
   "practice",
   "pat",
   "examine",
   "take",
   "info",
   "'",
   "practise",
   "curse", 		/* 171 */
   "use",
   "where",
   "levels",
   "reroll",
   "pray",
   ":",
   "beg",
   "bleed",
   "cringe",
   "daydream", 		/* 181 */
   "fume",
   "grovel",
   "hop",
   "nudge",
   "peer",
   "point",
   "ponder",
   "punch",
   "snarl",
   "spank", 		/* 191 */
   "steam",
   "tackle",
   "taunt",
   "think",
   "whine",
   "worship",
   "yodel",
   "brief",
   "wizlist",
   "consider", 		/* 201 */
   "group",
   "restore",
   "return",
   "switch",
   "quaff",
   "recite",
   "users",
   "immlist",
   "noshout",
   "wizhelp", 		/* 211 */
   "credits",
   "compact",
   "display",
   "poofin",    
   "poofout",   
   "teleport",
   "gecho",
   "wiznet",
   "holylight",
   "invis", 		/* 221 */
   "wimpy",
   "set",
   "ungroup",
   "notell",
   "wizlock",
   "junk",
   "gsay",
   "pardon",
   "murder",
   "title", 		/* 231 */
   ";", /* WIZNET */
   "balance",
   "deposit",
   "withdraw",
   "visible",
   "quest",
   "freeze",
   "dc",
   "nosummon",
   "assist", 		/* 241 */
   "split",
   "gtell",
   "brb",
   "norepeat",
   "toggle",
   "mail",
   "check",
   "receive",
   "send", /* not mail related :-) */
   "holler", 		/* 251 */
   "vnum",
   "fill",
   "uptime",
   "commands",
   "socials",
   "ban",
   "unban",
   "roll",
   "flirt",
   "tango", 		/* 261 */
   "embrace",
   "stroke",
   "greet",
   "drool",
   "date",
   "zreset",
   "color",
   "show",
   "prompt",
   "handbook", 		/* 271 */
   "policy",
   "nohassle",
   "nogossip",
   "noauction",
   "nograts",
   "roomflags",
   "mute",
   "nowiz",
   "notitle",
   "thaw", 		/* 281 */
   "unaffect",
   "cls",
   "clear",
   "version",
   "gossip",
   "auction",
   "grats",
   "donate",
   "page",
   "report", 		/* 291 */
   "diagnose",
   "qsay",
   "reboot",
   "syslog",
   "last",
   "slowns",
   "track",
   "whoami",
   "vstat",
   "\n"
};


/* CEND: search for me when you're looking for the end of the cmd list! :) */

char	*fill[] = 
{
   "in",
   "from",
   "with",
   "the",
   "on",
   "at",
   "to",
   "\n"
};


int	search_block(char *arg, char **list, bool exact)
{
   register int	i, l;

   /* Make into lower case, and get length of string */
   for (l = 0; *(arg + l); l++)
      *(arg + l) = LOWER(*(arg + l));

   if (exact) {
      for (i = 0; **(list + i) != '\n'; i++)
	 if (!strcmp(arg, *(list + i)))
	    return(i);
   } else {
      if (!l)
	 l = 1; /* Avoid "" to match the first available string */
      for (i = 0; **(list + i) != '\n'; i++)
	 if (!strncmp(arg, *(list + i), l))
	    return(i);
   }

   return(-1);
}


int	old_search_block(char *argument, int begin, int length, char **list, int mode)
{
   int	guess, found, search;

   /* If the word contain 0 letters, then a match is already found */
   found = (length < 1);

   guess = 0;

   /* Search for a match */

   if (mode)
      while ( NOT found AND * (list[guess]) != '\n' ) {
	 found = (length == strlen(list[guess]));
	 for (search = 0; (search < length AND found); search++)
	    found = (*(argument + begin + search) == *(list[guess] + search));
	 guess++;
      }
   else {
      while ( NOT found AND * (list[guess]) != '\n' ) {
	 found = 1;
	 for (search = 0; (search < length AND found); search++)
	    found = (*(argument + begin + search) == *(list[guess] + search));
	 guess++;
      }
   }

   return ( found ? guess : -1 );
}


void	command_interpreter(struct char_data *ch, char *argument)
{
   int	look_at, cmd, begin;
   extern int	no_specials;

   REMOVE_BIT(ch->specials.affected_by, AFF_HIDE);

   /* Find first non blank */
   for (begin = 0 ; (*(argument + begin ) == ' ' ) ; begin++)  
      ;

   /* Find length of first word */
   for (look_at = 0; *(argument + begin + look_at ) > ' ' ; look_at++)

      /* Make all letters lower case AND find length */
      *(argument + begin + look_at) = LOWER(*(argument + begin + look_at));

   cmd = old_search_block(argument, begin, look_at, command, 0);

   if (!cmd)
      return;

   if (PLR_FLAGGED(ch, PLR_FROZEN) && GET_LEVEL(ch) < LEVEL_IMPL) {
      send_to_char("You try, but the mind-numbing cold prevents you...\n\r", ch);
      return;
   }

   if (cmd > 0 && GET_LEVEL(ch) < cmd_info[cmd].minimum_level) {
      send_to_char("Huh?!?\n\r", ch);
      return;
   }

   if (cmd > 0 && (cmd_info[cmd].command_pointer != 0)) {
      if (GET_POS(ch) < cmd_info[cmd].minimum_position)
	 switch (GET_POS(ch)) {
	 case POSITION_DEAD:
	    send_to_char("Lie still; you are DEAD!!! :-( \n\r", ch);
	    break;
	 case POSITION_INCAP:
	 case POSITION_MORTALLYW:
	    send_to_char("You are in a pretty bad shape, unable to do anything!\n\r", ch);
	    break;
	 case POSITION_STUNNED:
	    send_to_char("All you can do right now is think about the stars!\n\r", ch);
	    break;
	 case POSITION_SLEEPING:
	    send_to_char("In your dreams, or what?\n\r", ch);
	    break;
	 case POSITION_RESTING:
	    send_to_char("Nah... You feel too relaxed to do that..\n\r", ch);
	    break;
	 case POSITION_SITTING:
	    send_to_char("Maybe you should get on your feet first?\n\r", ch);
	    break;
	 case POSITION_FIGHTING:
	    send_to_char("No way!  You're fighting for your life!\n\r", ch);
	    break;
	 }
      else {
	 if (!no_specials && special(ch, cmd, argument + begin + look_at))
	    return;

	 ((*cmd_info[cmd].command_pointer)
	     (ch, argument + begin + look_at, cmd, cmd_info[cmd].subcmd));
      }

      return;
   }

   if (cmd > 0 && (cmd_info[cmd].command_pointer == 0))
      send_to_char("Sorry, but that command has yet to be implemented...\n\r", ch);
   else
      send_to_char("Huh?!?\n\r", ch);
}


void	argument_interpreter(char *argument, char *first_arg, char *second_arg )
{
   int	look_at, found, begin;

   found = begin = 0;

   do {
      /* Find first non blank */
      for ( ; *(argument + begin ) == ' ' ; begin++) 
	 ;

      /* Find length of first word */
      for ( look_at = 0; *(argument + begin + look_at) > ' ' ; look_at++)

	 /* Make all letters lower case, AND copy them to first_arg */
	 *(first_arg + look_at) = LOWER(*(argument + begin + look_at));

      *(first_arg + look_at) = '\0';
      begin += look_at;
   } while (fill_word(first_arg));

   do {
      /* Find first non blank */
      for ( ; *(argument + begin ) == ' ' ; begin++)
	 ;

      /* Find length of first word */
      for ( look_at = 0; *(argument + begin + look_at) > ' ' ; look_at++)

	 /* Make all letters lower case, AND copy them to second_arg */
	 *(second_arg + look_at) = LOWER(*(argument + begin + look_at));

      *(second_arg + look_at) = '\0';
      begin += look_at;
   } while (fill_word(second_arg));
}


int	is_number(char *str)
{
   int	look_at;

   if (*str == '\0')
      return(0);

   for (look_at = 0; *(str + look_at) != '\0'; look_at++)
      if ((*(str + look_at) < '0') || (*(str + look_at) > '9'))
	 return(0);
   return(1);
}


/* find the first sub-argument of a string, return pointer to first char in
   primary argument, following the sub-arg */
char	*one_argument(char *argument, char *first_arg)
{
   int	found, begin, look_at;

   found = begin = 0;

   do {
      /* Find first non blank */
      for ( ; isspace(*(argument + begin)); begin++)
	 ;

      /* Find length of first word */
      for (look_at = 0; *(argument + begin + look_at) > ' ' ; look_at++)

	 /* Make all letters lower case, AND copy them to first_arg */
	 *(first_arg + look_at) = LOWER(*(argument + begin + look_at));

      *(first_arg + look_at) = '\0';
      begin += look_at;
   } while (fill_word(first_arg));

   return(argument + begin);
}


int	fill_word(char *argument)
{
   return (search_block(argument, fill, TRUE) >= 0);
}


/* determine if a given string is an abbreviation of another */
int	is_abbrev(char *arg1, char *arg2)
{
   if (!*arg1)
      return(0);

   for (; *arg1; arg1++, arg2++)
      if (LOWER(*arg1) != LOWER(*arg2))
	 return(0);

   return(1);
}



/* return first 'word' plus trailing substring of input string */
void	half_chop(char *string, char *arg1, char *arg2)
{
   for (; isspace(*string); string++)
      ;

   for (; !isspace(*arg1 = *string) && *string; string++, arg1++)
      ;

   *arg1 = '\0';

   for (; isspace(*string); string++)
      ;

   for (; (*arg2 = *string); string++, arg2++)
      ;
}



int	special(struct char_data *ch, int cmd, char *arg)
{
   register struct obj_data *i;
   register struct char_data *k;
   int	j;

   /* special in room? */
   if (world[ch->in_room].funct)
      if ((*world[ch->in_room].funct)(ch, cmd, arg))
	 return(1);

   /* special in equipment list? */
   for (j = 0; j <= (MAX_WEAR - 1); j++)
      if (ch->equipment[j] && ch->equipment[j]->item_number >= 0)
	 if (obj_index[ch->equipment[j]->item_number].func)
	    if ((*obj_index[ch->equipment[j]->item_number].func)
	        (ch, cmd, arg))
	       return(1);

   /* special in inventory? */
   for (i = ch->carrying; i; i = i->next_content)
      if (i->item_number >= 0)
	 if (obj_index[i->item_number].func)
	    if ((*obj_index[i->item_number].func)(ch, cmd, arg))
	       return(1);

   /* special in mobile present? */
   for (k = world[ch->in_room].people; k; k = k->next_in_room)
      if (IS_MOB(k))
	 if (mob_index[k->nr].func)
	    if ((*mob_index[k->nr].func)(ch, cmd, arg))
	       return(1);

   /* special in object present? */
   for (i = world[ch->in_room].contents; i; i = i->next_content)
      if (i->item_number >= 0)
	 if (obj_index[i->item_number].func)
	    if ((*obj_index[i->item_number].func)(ch, cmd, arg))
	       return(1);

   return(0);
}


void	assign_command_pointers (void)
{
   int	position;

   for (position = 0 ; position < MAX_CMD_LIST; position++)
      cmd_info[position].command_pointer = 0;

   COMMANDO(1  , POSITION_STANDING, do_move     , 0, 0)
   COMMANDO(2  , POSITION_STANDING, do_move     , 0, 0)
   COMMANDO(3  , POSITION_STANDING, do_move     , 0, 0)
   COMMANDO(4  , POSITION_STANDING, do_move     , 0, 0)
   COMMANDO(5  , POSITION_STANDING, do_move     , 0, 0)
   COMMANDO(6  , POSITION_STANDING, do_move     , 0, 0)
   COMMANDO(7  , POSITION_STANDING, do_enter    , 0, 0)
   COMMANDO(8  , POSITION_RESTING , do_exits    , 0, 0)
   COMMANDO(9  , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(10 , POSITION_RESTING , do_get      , 0, 0)
   COMMANDO(11 , POSITION_RESTING , do_drink    , 0, SCMD_DRINK)
   COMMANDO(12 , POSITION_RESTING , do_eat      , 0, SCMD_EAT)
   COMMANDO(13 , POSITION_RESTING , do_wear     , 0, 0)
   COMMANDO(14 , POSITION_RESTING , do_wield    , 0, 0)
   COMMANDO(15 , POSITION_RESTING , do_look     , 0, 0)
   COMMANDO(16 , POSITION_DEAD    , do_score    , 0, 0)
   COMMANDO(17 , POSITION_RESTING , do_say      , 0, 0)
   COMMANDO(18 , POSITION_RESTING , do_gen_com  , 0, SCMD_SHOUT)
   COMMANDO(19 , POSITION_DEAD    , do_tell     , 0, 0)
   COMMANDO(20 , POSITION_DEAD    , do_inventory, 0, 0)
   COMMANDO(21 , POSITION_DEAD    , do_quit     , 0, 0)
   COMMANDO(22 , POSITION_STANDING, do_action   , 0, 0)
   COMMANDO(23 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(24 , POSITION_STANDING, do_action   , 0, 0)
   COMMANDO(25 , POSITION_FIGHTING, do_kill     , 0, 0)
   COMMANDO(26 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(27 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(28 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(29 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(30 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(31 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(32 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(33 , POSITION_RESTING , do_insult   , 0, 0)
   COMMANDO(34 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(35 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(36 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(37 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(38 , POSITION_DEAD    , do_help     , 0, 0)
   COMMANDO(39 , POSITION_DEAD    , do_who      , 0, 0)
   COMMANDO(40 , POSITION_SLEEPING, do_emote    , 1, 0)
   COMMANDO(41 , POSITION_SLEEPING, do_echo     , LEVEL_IMMORT, 0)
   COMMANDO(42 , POSITION_RESTING , do_stand    , 0, 0)
   COMMANDO(43 , POSITION_RESTING , do_sit      , 0, 0)
   COMMANDO(44 , POSITION_RESTING , do_rest     , 0, 0)
   COMMANDO(45 , POSITION_SLEEPING, do_sleep    , 0, 0)
   COMMANDO(46 , POSITION_SLEEPING, do_wake     , 0, 0)
   COMMANDO(47 , POSITION_SLEEPING, do_force    , LEVEL_GOD, 0)
   COMMANDO(48 , POSITION_SLEEPING, do_trans    , LEVEL_GOD, 0)
   COMMANDO(49 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(50 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(51 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(52 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(53 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(54 , POSITION_SLEEPING, do_gen_ps   , 0, SCMD_NEWS)
   COMMANDO(55 , POSITION_SLEEPING, do_equipment, 0, 0)
   COMMANDO(56 , POSITION_STANDING, do_not_here , 0, 0)
   COMMANDO(57 , POSITION_STANDING, do_not_here , 0, 0)
   COMMANDO(58 , POSITION_STANDING, do_not_here , 0, 0)
   COMMANDO(59 , POSITION_STANDING, do_not_here , 0, 0)
   COMMANDO(60 , POSITION_RESTING , do_drop     , 0, SCMD_DROP)
   COMMANDO(61 , POSITION_SLEEPING, do_goto     , LEVEL_IMMORT, 0)
   COMMANDO(62 , POSITION_RESTING , do_weather  , 0, 0)
   COMMANDO(63 , POSITION_RESTING , do_read     , 0, 0)
   COMMANDO(64 , POSITION_STANDING, do_pour     , 0, SCMD_POUR)
   COMMANDO(65 , POSITION_RESTING , do_grab     , 0, 0)
   COMMANDO(66 , POSITION_RESTING , do_remove   , 0, 0)
   COMMANDO(67 , POSITION_RESTING , do_put      , 0, 0)
   COMMANDO(68 , POSITION_DEAD    , do_shutdown , LEVEL_IMPL, 0)
   COMMANDO(69 , POSITION_SLEEPING, do_save     , 0, 0)
   COMMANDO(70 , POSITION_FIGHTING, do_hit      , 0, SCMD_HIT)
   COMMANDO(71 , POSITION_SLEEPING, do_string   , LEVEL_GRGOD, 0)
   COMMANDO(72 , POSITION_RESTING , do_give     , 0, 0)
   COMMANDO(73 , POSITION_DEAD    , do_quit     , 0, SCMD_QUIT)
   COMMANDO(74 , POSITION_DEAD    , do_stat     , LEVEL_IMMORT, 0)
   COMMANDO(75 , POSITION_SLEEPING, do_skillset , LEVEL_GRGOD, 0)
   COMMANDO(76 , POSITION_DEAD    , do_time     , 0, 0)
   COMMANDO(77 , POSITION_DEAD    , do_load     , LEVEL_GOD, 0)
   COMMANDO(78 , POSITION_DEAD    , do_purge    , LEVEL_GOD, 0)
   COMMANDO(79 , POSITION_DEAD    , do_shutdown , LEVEL_IMPL, SCMD_SHUTDOWN)
   COMMANDO(80 , POSITION_DEAD    , do_gen_write, 0, SCMD_IDEA)
   COMMANDO(81 , POSITION_DEAD    , do_gen_write, 0, SCMD_TYPO)
   COMMANDO(82 , POSITION_DEAD    , do_gen_write, 0, SCMD_BUG)
   COMMANDO(83 , POSITION_RESTING , do_whisper  , 0, 0)
   COMMANDO(84 , POSITION_SITTING , do_cast     , 1, 0)
   COMMANDO(85 , POSITION_DEAD    , do_at       , LEVEL_IMMORT, 0)
   COMMANDO(86 , POSITION_RESTING , do_ask      , 0, 0)
   COMMANDO(87 , POSITION_RESTING , do_order    , 1, 0)
   COMMANDO(88 , POSITION_RESTING , do_drink    , 0, SCMD_SIP)
   COMMANDO(89 , POSITION_RESTING , do_eat      , 0, SCMD_TASTE)
   COMMANDO(90 , POSITION_DEAD    , do_snoop    , LEVEL_GOD, 0)
   COMMANDO(91 , POSITION_RESTING , do_follow   , 0, 0)
   COMMANDO(92 , POSITION_STANDING, do_not_here , 1, 0)
   COMMANDO(93 , POSITION_STANDING, do_not_here , 1, 0)
   COMMANDO(94 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(95 , POSITION_DEAD    , do_advance  , LEVEL_IMPL, 0)
   COMMANDO(96 , POSITION_SITTING , do_action   , 0, 0)
   COMMANDO(97 , POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(98 , POSITION_STANDING, do_action   , 0, 0)
   COMMANDO(99 , POSITION_SITTING , do_open     , 0, 0)
   COMMANDO(100, POSITION_SITTING , do_close    , 0, 0)
   COMMANDO(101, POSITION_SITTING , do_lock     , 0, 0)
   COMMANDO(102, POSITION_SITTING , do_unlock   , 0, 0)
   COMMANDO(103, POSITION_STANDING, do_leave    , 0, 0)
   COMMANDO(104, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(105, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(106, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(107, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(108, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(109, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(110, POSITION_STANDING, do_action   , 0, 0)
   COMMANDO(111, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(112, POSITION_STANDING, do_action   , 0, 0)
   COMMANDO(113, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(114, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(115, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(116, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(117, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(118, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(119, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(120, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(121, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(122, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(123, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(124, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(125, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(126, POSITION_STANDING, do_action   , 0, 0)
   COMMANDO(127, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(128, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(129, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(130, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(131, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(132, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(133, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(134, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(135, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(136, POSITION_SLEEPING, do_action   , 0, 0)
   COMMANDO(137, POSITION_STANDING, do_action   , 0, 0)
   COMMANDO(138, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(139, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(140, POSITION_STANDING, do_action   , 0, 0)
   COMMANDO(141, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(142, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(143, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(144, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(145, POSITION_STANDING, do_action   , 0, 0)
   COMMANDO(146, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(147, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(148, POSITION_STANDING, do_action   , LEVEL_IMMORT, 0)
   COMMANDO(149, POSITION_STANDING, do_write    , 1, 0)
   COMMANDO(150, POSITION_RESTING , do_grab     , 1, 0)
   COMMANDO(151, POSITION_FIGHTING, do_flee     , 1, 0)
   COMMANDO(152, POSITION_STANDING, do_sneak    , 1, 0)
   COMMANDO(153, POSITION_RESTING , do_hide     , 1, 0)
   COMMANDO(154, POSITION_STANDING, do_backstab , 1, 0)
   COMMANDO(155, POSITION_STANDING, do_pick     , 1, 0)
   COMMANDO(156, POSITION_STANDING, do_steal    , 1, 0)
   COMMANDO(157, POSITION_FIGHTING, do_bash     , 1, 0)
   COMMANDO(158, POSITION_FIGHTING, do_rescue   , 1, 0)
   COMMANDO(159, POSITION_FIGHTING, do_kick     , 1, 0)
   COMMANDO(160, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(161, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(162, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(163, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(164, POSITION_RESTING , do_practice , 1, 0)
   COMMANDO(165, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(166, POSITION_SITTING , do_examine  , 0, 0)
   COMMANDO(167, POSITION_RESTING , do_get      , 0, 0)
   COMMANDO(168, POSITION_SLEEPING, do_gen_ps   , 0, SCMD_INFO)
   COMMANDO(169, POSITION_RESTING , do_say      , 0, 0)
   COMMANDO(170, POSITION_RESTING , do_practice , 1, 0)
   COMMANDO(171, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(172, POSITION_SITTING , do_use      , 1, 0)
   COMMANDO(173, POSITION_DEAD    , do_where    , 1, 0)
   COMMANDO(174, POSITION_DEAD    , do_levels   , 0, 0)
   COMMANDO(175, POSITION_DEAD    , do_wizutil  , LEVEL_GRGOD, SCMD_REROLL)
   COMMANDO(176, POSITION_SITTING , do_action   , 0, 0)
   COMMANDO(177, POSITION_SLEEPING, do_emote    , 1, 0)
   COMMANDO(178, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(179, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(180, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(181, POSITION_SLEEPING, do_action   , 0, 0)
   COMMANDO(182, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(183, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(184, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(185, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(186, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(187, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(188, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(189, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(190, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(191, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(192, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(193, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(194, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(195, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(196, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(197, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(198, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(199, POSITION_DEAD    , do_gen_tog  , 0, SCMD_BRIEF)
   COMMANDO(200, POSITION_DEAD    , do_gen_ps   , 0, SCMD_WIZLIST)
   COMMANDO(201, POSITION_RESTING , do_consider , 0, 0)
   COMMANDO(202, POSITION_RESTING , do_group    , 1, 0)
   COMMANDO(203, POSITION_DEAD    , do_restore  , LEVEL_GOD, 0)
   COMMANDO(204, POSITION_DEAD    , do_return   , 0, 0)
   COMMANDO(205, POSITION_DEAD    , do_switch   , LEVEL_GRGOD, 0)
   COMMANDO(206, POSITION_RESTING , do_quaff    , 0, 0)
   COMMANDO(207, POSITION_RESTING , do_recite   , 0, 0)
   COMMANDO(208, POSITION_DEAD    , do_users    , LEVEL_IMMORT, 0)
   COMMANDO(209, POSITION_DEAD    , do_gen_ps   , 0, SCMD_IMMLIST)
   COMMANDO(210, POSITION_SLEEPING, do_gen_tog  , 1, SCMD_DEAF)
   COMMANDO(211, POSITION_SLEEPING, do_commands , LEVEL_IMMORT, SCMD_WIZHELP)
   COMMANDO(212, POSITION_DEAD    , do_gen_ps   , 0, SCMD_CREDITS)
   COMMANDO(213, POSITION_DEAD    , do_gen_tog  , 0, SCMD_COMPACT)
   COMMANDO(214, POSITION_DEAD    , do_display  , 0, 0)
   COMMANDO(215, POSITION_DEAD    , do_poofset  , LEVEL_IMMORT, SCMD_POOFIN)
   COMMANDO(216, POSITION_DEAD    , do_poofset  , LEVEL_IMMORT, SCMD_POOFOUT)
   COMMANDO(217, POSITION_DEAD    , do_teleport , LEVEL_GOD, 0)
   COMMANDO(218, POSITION_DEAD    , do_gecho    , LEVEL_GOD, 0)
   COMMANDO(219, POSITION_DEAD    , do_wiznet   , LEVEL_IMMORT, 0)
   COMMANDO(220, POSITION_DEAD    , do_gen_tog  , LEVEL_IMMORT, SCMD_HOLYLIGHT)
   COMMANDO(221, POSITION_DEAD    , do_invis    , LEVEL_IMMORT, 0)
   COMMANDO(222, POSITION_DEAD    , do_wimpy    , 0, 0)
   COMMANDO(223, POSITION_DEAD    , do_set      , LEVEL_GOD, 0)
   COMMANDO(224, POSITION_DEAD    , do_ungroup  , 0, 0)
   COMMANDO(225, POSITION_DEAD    , do_gen_tog  , 1, SCMD_NOTELL)
   COMMANDO(226, POSITION_DEAD    , do_wizlock  , LEVEL_IMPL, 0)
   COMMANDO(227, POSITION_RESTING , do_drop     , 0, SCMD_JUNK)
   COMMANDO(228, POSITION_SLEEPING, do_gsay     , 0, 0)
   COMMANDO(229, POSITION_DEAD    , do_wizutil  , LEVEL_GOD, SCMD_PARDON)
   COMMANDO(230, POSITION_FIGHTING, do_hit      , 0, SCMD_MURDER)
   COMMANDO(231, POSITION_DEAD    , do_title    , 0, 0)
   COMMANDO(232, POSITION_DEAD    , do_wiznet   , LEVEL_IMMORT, 0)
   COMMANDO(233, POSITION_STANDING, do_not_here , 1, 0)
   COMMANDO(234, POSITION_STANDING, do_not_here , 1, 0)
   COMMANDO(235, POSITION_STANDING, do_not_here , 1, 0)
   COMMANDO(236, POSITION_RESTING , do_visible  , 1, 0)
   COMMANDO(237, POSITION_DEAD    , do_gen_tog  , 0, SCMD_QUEST)
   COMMANDO(238, POSITION_DEAD    , do_wizutil  , LEVEL_FREEZE, SCMD_FREEZE)
   COMMANDO(239, POSITION_DEAD    , do_dc       , LEVEL_GOD, 0)
   COMMANDO(240, POSITION_DEAD    , do_gen_tog  , 1, SCMD_NOSUMMON)
   COMMANDO(241, POSITION_FIGHTING, do_assist   , 1, 0)
   COMMANDO(242, POSITION_SITTING , do_split    , 1, 0)
   COMMANDO(243, POSITION_SLEEPING, do_gsay     , 0, 0)
   COMMANDO(244, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(245, POSITION_DEAD    , do_gen_tog  , 0, SCMD_NOREPEAT)
   COMMANDO(246, POSITION_DEAD    , do_toggle   , 0, 0)
   COMMANDO(247, POSITION_STANDING, do_not_here , 1, 0)
   COMMANDO(248, POSITION_STANDING, do_not_here , 1, 0)
   COMMANDO(249, POSITION_STANDING, do_not_here , 1, 0)
   COMMANDO(250, POSITION_SLEEPING, do_send     , LEVEL_GOD, 0)
   COMMANDO(251, POSITION_RESTING , do_gen_com  , 1, SCMD_HOLLER)
   COMMANDO(252, POSITION_DEAD    , do_vnum     , LEVEL_IMMORT, 0)
   COMMANDO(253, POSITION_STANDING, do_pour     , 0, SCMD_FILL)
   COMMANDO(254, POSITION_DEAD    , do_uptime   , LEVEL_IMMORT, 0)
   COMMANDO(255, POSITION_DEAD    , do_commands , 0, SCMD_COMMANDS)
   COMMANDO(256, POSITION_DEAD    , do_commands , 0, SCMD_SOCIALS)
   COMMANDO(257, POSITION_DEAD    , do_ban      , LEVEL_GRGOD, 0)
   COMMANDO(258, POSITION_DEAD    , do_unban    , LEVEL_GRGOD, 0)
   COMMANDO(259, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(260, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(261, POSITION_STANDING, do_action   , 0, 0)
   COMMANDO(262, POSITION_STANDING, do_action   , 0, 0)
   COMMANDO(263, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(264, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(265, POSITION_RESTING , do_action   , 0, 0)
   COMMANDO(266, POSITION_DEAD    , do_date     , LEVEL_IMMORT, 0)
   COMMANDO(267, POSITION_DEAD    , do_zreset   , LEVEL_GRGOD, 0)
   COMMANDO(268, POSITION_DEAD    , do_color    , 0, 0)
   COMMANDO(269, POSITION_DEAD    , do_show     , LEVEL_IMMORT, 0)
   COMMANDO(270, POSITION_DEAD    , do_display  , 0, 0)
   COMMANDO(271, POSITION_DEAD    , do_gen_ps   , LEVEL_IMMORT, SCMD_HANDBOOK)
   COMMANDO(272, POSITION_DEAD    , do_gen_ps   , 0, SCMD_POLICIES)
   COMMANDO(273, POSITION_DEAD    , do_gen_tog  , LEVEL_IMMORT, SCMD_NOHASSLE)
   COMMANDO(274, POSITION_DEAD    , do_gen_tog  , 0, SCMD_NOGOSSIP)
   COMMANDO(275, POSITION_DEAD    , do_gen_tog  , 0, SCMD_NOAUCTION)
   COMMANDO(276, POSITION_DEAD    , do_gen_tog  , 0, SCMD_NOGRATZ)
   COMMANDO(277, POSITION_DEAD    , do_gen_tog  , LEVEL_IMMORT, SCMD_ROOMFLAGS)
   COMMANDO(278, POSITION_DEAD    , do_wizutil  , LEVEL_GOD, SCMD_SQUELCH)
   COMMANDO(279, POSITION_DEAD    , do_gen_tog  , LEVEL_IMMORT, SCMD_NOWIZ)
   COMMANDO(280, POSITION_DEAD    , do_wizutil  , LEVEL_GOD, SCMD_NOTITLE)
   COMMANDO(281, POSITION_DEAD    , do_wizutil  , LEVEL_FREEZE, SCMD_THAW)
   COMMANDO(282, POSITION_DEAD    , do_wizutil  , LEVEL_GOD, SCMD_UNAFFECT)
   COMMANDO(283, POSITION_DEAD    , do_gen_ps   , 0, SCMD_CLEAR)
   COMMANDO(284, POSITION_DEAD    , do_gen_ps   , 0, SCMD_CLEAR)
   COMMANDO(285, POSITION_DEAD    , do_gen_ps   , 0, SCMD_VERSION)
   COMMANDO(286, POSITION_SLEEPING, do_gen_com  , 0, SCMD_GOSSIP)
   COMMANDO(287, POSITION_SLEEPING, do_gen_com  , 0, SCMD_AUCTION)
   COMMANDO(288, POSITION_SLEEPING, do_gen_com  , 0, SCMD_GRATZ)
   COMMANDO(289, POSITION_RESTING , do_drop     , 0, SCMD_DONATE)
   COMMANDO(290, POSITION_DEAD    , do_page     , LEVEL_GOD, 0)
   COMMANDO(291, POSITION_RESTING , do_report   , 0, 0)
   COMMANDO(292, POSITION_RESTING , do_diagnose , 0, 0)
   COMMANDO(293, POSITION_RESTING , do_qsay     , 0, 0)
   COMMANDO(294, POSITION_DEAD    , do_reboot   , LEVEL_IMPL, 0)
   COMMANDO(295, POSITION_DEAD    , do_syslog   , LEVEL_IMMORT, 0)
   COMMANDO(296, POSITION_DEAD    , do_last     , LEVEL_GOD, 0)
   COMMANDO(297, POSITION_DEAD    , do_gen_tog  , LEVEL_IMPL, SCMD_SLOWNS)
   COMMANDO(298, POSITION_STANDING, do_track    , 0, 0)
   COMMANDO(299, POSITION_DEAD    , do_gen_ps   , 0, SCMD_WHOAMI)
   COMMANDO(300, POSITION_DEAD    , do_vstat    , LEVEL_GOD, 0)
}




/* *************************************************************************
*  Stuff for controlling the non-playing sockets (get name, pwd etc)       *
************************************************************************* */


/* locate entry in p_table with entry->name == name. -1 mrks failed search */
int	find_name(char *name)
{
   int	i;

   for (i = 0; i <= top_of_p_table; i++) {
      if (!str_cmp((player_table + i)->name, name))
	 return(i);
   }

   return(-1);
}


int	_parse_name(char *arg, char *name)
{
   int	i;

   /* skip whitespaces */
   for (; isspace(*arg); arg++)
      ;

   for (i = 0; (*name = *arg); arg++, i++, name++)
      if (!isalpha(*arg) || i > 15)
	 return(1);

   if (!i)
      return(1);

   return(0);
}






/* deal with newcomers and other non-playing sockets */
void	nanny(struct descriptor_data *d, char *arg)
{
   char	buf[100];
   int	player_i, load_result;
   char	tmp_name[20];
   struct char_file_u tmp_store;
   struct char_data *tmp_ch;
   struct descriptor_data *k, *next;
   extern struct descriptor_data *descriptor_list;
   extern sh_int r_mortal_start_room;
   extern sh_int r_immort_start_room;
   extern sh_int r_frozen_start_room;
   sh_int load_room;

   ACMD(do_look);
   int	load_char(char *name, struct char_file_u *char_element);


   switch (STATE(d)) {
   case CON_NME:		/* wait for input of name	*/
      if (!d->character) {
	 CREATE(d->character, struct char_data, 1);
	 clear_char(d->character);
	 d->character->desc = d;
      }

      for (; isspace(*arg); arg++)
	 ;

      if (!*arg)
	 close_socket(d);
      else {
         if ((_parse_name(arg, tmp_name)) || fill_word(strcpy(buf,tmp_name)) ||
	     strlen(tmp_name) > MAX_NAME_LENGTH || strlen(tmp_name) < 2 ||
	     !str_cmp(tmp_name, "all") || !str_cmp(tmp_name, "self") ||
	     !str_cmp(tmp_name, "room")) {
	        SEND_TO_Q("Invalid name, please try another.\n\r", d);
	        SEND_TO_Q("Name: ", d);
	        return;
	 }

	 if ((player_i = load_char(tmp_name, &tmp_store)) > -1) {
	    d->pos = player_i;
	    store_to_char(&tmp_store, d->character);

	    if (PLR_FLAGGED(d->character, PLR_DELETED)) {
	       free_char(d->character);
	       CREATE(d->character, struct char_data, 1);
	       clear_char(d->character);
	       d->character->desc = d;
               CREATE(d->character->player.name, char, strlen(tmp_name) + 1);
	       strcpy(d->character->player.name, CAP(tmp_name));
	       sprintf(buf, "Did I get that right, %s (Y/N)? ", tmp_name);
	       SEND_TO_Q(buf, d);
	       STATE(d) = CON_NMECNF;
	    } else {
	       strcpy(d->pwd, tmp_store.pwd);

	       /* undo it just in case they are set */
	       REMOVE_BIT(PLR_FLAGS(d->character),
	          PLR_WRITING | PLR_MAILING | PLR_CRYO);

	       SEND_TO_Q("Password: ", d);
	       echo_off(d->descriptor);

	       STATE(d) = CON_PWDNRM;
	    }
	 } else {
	    /* player unknown gotta make a new */

	    if (!Valid_Name(tmp_name)) {
	       SEND_TO_Q("Invalid name, please try another.\n\r", d);
	       SEND_TO_Q("Name: ", d);
	       return;
	    }

	    CREATE(d->character->player.name, char, strlen(tmp_name) + 1);
	    strcpy(d->character->player.name, CAP(tmp_name));

	    sprintf(buf, "Did I get that right, %s (Y/N)? ", tmp_name);
	    SEND_TO_Q(buf, d);
	    STATE(d) = CON_NMECNF;
	 }
      }
      break;
   case CON_NMECNF:	/* wait for conf. of new name	*/
      /* skip whitespaces */
      for (; isspace(*arg); arg++)
	 ;

      if (*arg == 'y' || *arg == 'Y') {
	 if (isbanned(d->host) >= BAN_NEW) {
	    sprintf(buf, "Request for new char %s denied from [%s] (siteban)",
	        GET_NAME(d->character), d->host);
	    mudlog(buf, NRM, LEVEL_GOD, TRUE);
	    SEND_TO_Q("Sorry, new characters not allowed from your site!\n\r", d);
	    STATE(d) = CON_CLOSE;
	    return;
	 }
	 if (restrict) {
	    SEND_TO_Q("Sorry, new players can't be created at the moment.\n\r", d);
	    sprintf(buf, "Request for new char %s denied from %s (wizlock)",
	        GET_NAME(d->character), d->host);
	    mudlog(buf, NRM, LEVEL_GOD, TRUE);
	    STATE(d) = CON_CLOSE;
	    return;
	 }
	 SEND_TO_Q("New character.\n\r", d);
	 sprintf(buf, "Give me a password for %s: ", GET_NAME(d->character));
	 SEND_TO_Q(buf, d);
	 echo_off(d->descriptor);
	 STATE(d) = CON_PWDGET;
      } else if (*arg == 'n' || *arg == 'N') {
	 SEND_TO_Q("Ok, what IS it, then? ", d);
	 free(GET_NAME(d->character));
	 d->character->player.name = 0;
	 STATE(d) = CON_NME;
      } else { /* Please do Y or N */
	 SEND_TO_Q("Please type Yes or No: ", d);
      }
      break;
   case CON_PWDNRM:	/* get pwd for known player	*/

      /* turn echo back on */
      echo_on(d->descriptor);

      /* skip whitespaces */
      for (; isspace(*arg); arg++) 
	 ;
      if (!*arg)
	 close_socket(d);
      else {
	 if (strncmp(CRYPT(arg, d->pwd), d->pwd, MAX_PWD_LENGTH)) {
	    sprintf(buf, "Bad PW: %s [%s]", GET_NAME(d->character), d->host);
	    mudlog(buf, BRF, LEVEL_GOD, TRUE);
	    d->character->specials2.bad_pws++;
	    save_char(d->character, NOWHERE);
	    if (++(d->bad_pws) >= 3) { /* 3 strikes and you're out. */
	       SEND_TO_Q("Wrong password... disconnecting.\n\r", d);
	       STATE(d) = CON_CLOSE;
	    } else {
	       SEND_TO_Q("Wrong password.\n\rPassword: ", d);
	       echo_off(d->descriptor);
	    }
	    return;
	 }

	 load_result = d->character->specials2.bad_pws;
	 d->character->specials2.bad_pws = 0;
	 save_char(d->character, NOWHERE);

	 if (isbanned(d->host) == BAN_SELECT && 
	     !PLR_FLAGGED(d->character, PLR_SITEOK)) {
	    SEND_TO_Q("Sorry, this char has not been cleared for login from your site!\n\r", d);
	    STATE(d) = CON_CLOSE;
	    sprintf(buf, "Connection attempt for %s denied from %s",
	        GET_NAME(d->character), d->host);
	    mudlog(buf, NRM, LEVEL_GOD, TRUE);
	    return;
	 }

	 if (GET_LEVEL(d->character) < restrict) {
	    SEND_TO_Q("The game is temporarily restricted.. try again later.\n\r", d);
	    STATE(d) = CON_CLOSE;
	    sprintf(buf, "Request for login denied for %s [%s] (wizlock)",
	        GET_NAME(d->character), d->host);
	    mudlog(buf, NRM, LEVEL_GOD, TRUE);
	    return;
	 }

         /* first, check for switched characters */
	 for (tmp_ch = character_list; tmp_ch; tmp_ch = tmp_ch->next)
	    if (IS_NPC(tmp_ch) && tmp_ch->desc && tmp_ch->desc->original &&
		GET_IDNUM(tmp_ch->desc->original) == GET_IDNUM(d->character)) {
		  SEND_TO_Q("Disconnecting.", tmp_ch->desc);
		  free_char(d->character);
		  d->character = tmp_ch->desc->original;
		  d->character->desc = d;
		  tmp_ch->desc->character = 0;
		  tmp_ch->desc->original = 0;
		  STATE(tmp_ch->desc) = CON_CLOSE;
		  d->character->specials.timer = 0;
		  SEND_TO_Q("Reconnecting to unswitched char.", d);
		  REMOVE_BIT(PLR_FLAGS(d->character), PLR_MAILING | PLR_WRITING);
		  STATE(d) = CON_PLYNG;
		  sprintf(buf, "%s [%s] has reconnected.", GET_NAME(d->character), d->host);
                  mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
		  return;
	    }

	 /* now check for linkless and usurpable */
	 for (tmp_ch = character_list; tmp_ch; tmp_ch = tmp_ch->next)
	    if (!IS_NPC(tmp_ch) && 
	        GET_IDNUM(d->character) == GET_IDNUM(tmp_ch)) {
	       if (!tmp_ch->desc) {
		  SEND_TO_Q("Reconnecting.\n\r", d);
		  act("$n has reconnected.", TRUE, tmp_ch, 0, 0, TO_ROOM);
		  sprintf(buf, "%s [%s] has reconnected.", GET_NAME(d->character), d->host);
                  mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
	       } else {
		  sprintf(buf, "%s has re-logged in ... disconnecting old socket.",
		      GET_NAME(tmp_ch));
                  mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(tmp_ch)), TRUE);
		  SEND_TO_Q("This body has been usurped!\n\r", tmp_ch->desc);
		  STATE(tmp_ch->desc) = CON_CLOSE;
		  tmp_ch->desc->character = 0;
		  tmp_ch->desc = 0;
		  SEND_TO_Q("You take over your own body, already in use!\n\r", d);
		  act("$n suddenly keels over in pain, surrounded by a white aura...\n\r"
		      "$n's body has been taken over by a new spirit!",
		      TRUE, tmp_ch, 0, 0, TO_ROOM);
	       }

	       free_char(d->character);
	       tmp_ch->desc = d;
	       d->character = tmp_ch;
	       tmp_ch->specials.timer = 0;
	       REMOVE_BIT(PLR_FLAGS(d->character), PLR_MAILING | PLR_WRITING);
	       STATE(d) = CON_PLYNG;
	       return;
	    }

	 sprintf(buf, "%s [%s] has connected.", GET_NAME(d->character), d->host);
         mudlog(buf, BRF, MAX(LEVEL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
	 if (GET_LEVEL(d->character) >= LEVEL_IMMORT)
	    SEND_TO_Q(imotd, d);
	 else
	    SEND_TO_Q(motd, d);
	 if (load_result) {
	    sprintf(buf, "\n\r\n\r\007\007\007"
	        "%s%d LOGIN FAILURE%s SINCE LAST SUCCESSFUL LOGIN.%s\n\r",
	        CCRED(d->character, C_SPR), load_result,
	        (load_result > 1) ? "S" : "", CCNRM(d->character, C_SPR));
	    SEND_TO_Q(buf, d);
	 }

	 SEND_TO_Q("\n\r\n*** PRESS RETURN: ", d);
	 STATE(d) = CON_RMOTD;
      }
      break;

   case CON_PWDGET:	/* get pwd for new player	*/

      /* skip whitespaces */
      for (; isspace(*arg); arg++)
	 ;

      if (!*arg || strlen(arg) > MAX_PWD_LENGTH || strlen(arg) < 3 || 
          !str_cmp(arg, GET_NAME(d->character))) {
	 SEND_TO_Q("\n\rIllegal password.\n\r", d);
	 SEND_TO_Q("Password: ", d);
	 return;
      }

      strncpy(d->pwd, CRYPT(arg, d->character->player.name), MAX_PWD_LENGTH);
      *(d->pwd + MAX_PWD_LENGTH) = '\0';

      SEND_TO_Q("\n\rPlease retype password: ", d);
      STATE(d) = CON_PWDCNF;
      break;

   case CON_PWDCNF:	/* get confirmation of new pwd	*/

      /* skip whitespaces */
      for (; isspace(*arg); arg++)
	 ;

      if (strncmp(CRYPT(arg, d->pwd), d->pwd, MAX_PWD_LENGTH)) {
	 SEND_TO_Q("\n\rPasswords don't match... start over.\n\r", d);
	 SEND_TO_Q("Password: ", d);
	 STATE(d) = CON_PWDGET;
	 return;
      }

      /* turn echo back on */
      echo_on(d->descriptor);

      SEND_TO_Q("What is your sex (M/F)? ", d);
      STATE(d) = CON_QSEX;
      break;

   case CON_QSEX:	/* query sex of new user	*/

      /* skip whitespaces */
      for (; isspace(*arg); arg++)
	 ;
      switch (*arg) {
      case 'm':
      case 'M':
	 /* sex MALE */
	 d->character->player.sex = SEX_MALE;
	 break;
      case 'f':
      case 'F':
	 /* sex FEMALE */
	 d->character->player.sex = SEX_FEMALE;
	 break;
      default:
	 SEND_TO_Q("That's not a sex..\n\r", d);
	 SEND_TO_Q("What IS your sex? :", d);
	 return;
	 break;
      }

      SEND_TO_Q("\n\rSelect a class:\n\r  [C]leric\n\r  [T]hief\n\r  [W]arrior\n\r  [M]agic-user", d);
      SEND_TO_Q("\n\rClass: ", d);
      STATE(d) = CON_QCLASS;
      break;

   case CON_QCLASS:

      /* skip whitespaces */
      for (; isspace(*arg); arg++)
	 ;

      *arg = LOWER(*arg);

      if (*arg != 'm' && *arg != 'c' && *arg != 'w' && *arg != 't') {
	 SEND_TO_Q("\n\rThat's not a class.\n\rClass: ", d);
	 return;
      }

      switch (*arg) {
      case 'm': GET_CLASS(d->character) = CLASS_MAGIC_USER; break;
      case 'c': GET_CLASS(d->character) = CLASS_CLERIC; break;
      case 'w': GET_CLASS(d->character) = CLASS_WARRIOR; break;
      case 't': GET_CLASS(d->character) = CLASS_THIEF; break;
      }

      init_char(d->character);
      if (d->pos < 0)
         d->pos = create_entry(GET_NAME(d->character));
      save_char(d->character, NOWHERE);
      SEND_TO_Q(motd, d);
      SEND_TO_Q("\n\r\n*** PRESS RETURN: ", d);
      STATE(d) = CON_RMOTD;

      sprintf(buf, "%s [%s] new player.", GET_NAME(d->character), d->host);
      mudlog(buf, NRM, LEVEL_IMMORT, TRUE);
      break;

   case CON_RMOTD:		/* read CR after printing motd	*/
      SEND_TO_Q(MENU, d);
      STATE(d) = CON_SLCT;
      break;

   case CON_SLCT:		/* get selection from main menu	*/

      /* skip whitespaces */
      for (; isspace(*arg); arg++)
	 ;
      switch (*arg) {
      case '0':
	 close_socket(d);
	 break;

      case '1':

         /* this code is to prevent people from multiply logging in */
         for (k = descriptor_list; k; k=next) {
            next = k->next;
            if (!k->connected && k->character &&
               !str_cmp(GET_NAME(k->character), GET_NAME(d->character))){
                  SEND_TO_Q("Your character has been deleted.\n\r",d);
                  STATE(d) = CON_CLOSE;
                  return;
	       }
	 }
	 reset_char(d->character);
	 if ((load_result = Crash_load(d->character)))
	    d->character->in_room = NOWHERE;
	 save_char(d->character, NOWHERE);
	 send_to_char(WELC_MESSG, d->character);
	 d->character->next = character_list;
	 character_list = d->character;
	 if (GET_LEVEL(d->character) >= LEVEL_IMMORT) {
	    if (PLR_FLAGGED(d->character, PLR_LOADROOM)) {
		if ((load_room = real_room(GET_LOADROOM(d->character))) < 0)
		   load_room = r_immort_start_room;
	    } else
	       load_room = r_immort_start_room;
	    if (PLR_FLAGGED(d->character, PLR_INVSTART))
	       GET_INVIS_LEV(d->character) = GET_LEVEL(d->character);
	 } else {
	    if (PLR_FLAGGED(d->character, PLR_FROZEN))
	       load_room = r_frozen_start_room;
	    else {
	       if (d->character->in_room == NOWHERE)
		  load_room = r_mortal_start_room;
	       else if ((load_room = real_room(d->character->in_room)) < 0)
		     load_room = r_mortal_start_room;
	    }
	 }
	 char_to_room(d->character, load_room);
	 act("$n has entered the game.", TRUE, d->character, 0, 0, TO_ROOM);

	 STATE(d) = CON_PLYNG;
	 if (!GET_LEVEL(d->character)) {
	    do_start(d->character);
	    send_to_char(START_MESSG, d->character);
	 }
	 do_look(d->character, "", 0, 0);
	 if (has_mail(GET_NAME(d->character)))
	    send_to_char("You have mail waiting.\n\r", d->character);
	 if (load_result == 2) { /* rented items lost */
	    send_to_char("\n\r\007You could not afford your rent!\n\r"
	        "Your possesions have been donated to the Salvation Army!\n\r", d->character);
	 }
	 d->prompt_mode = 1;
	 break;

      case '2':
	 SEND_TO_Q("Enter the text you'd like others to see when they look at you.\n\r", d);
	 SEND_TO_Q("Terminate with a '@'.\n\r", d);
	 if (d->character->player.description) {
	    SEND_TO_Q("Old description :\n\r", d);
	    SEND_TO_Q(d->character->player.description, d);
	    free(d->character->player.description);
	    d->character->player.description = 0;
	 }
	 d->str = &d->character->player.description;
	 d->max_str = 240;
	 STATE(d) = CON_EXDSCR;
	 break;

      case '3':
	 SEND_TO_Q(background, d);
	 SEND_TO_Q("\n\r\n\r*** PRESS RETURN:", d);
	 STATE(d) = CON_RMOTD;
	 break;

      case '4':
	 SEND_TO_Q("Enter your old password: ", d);
	 echo_off(d->descriptor);
	 STATE(d) = CON_PWDNQO;
	 break;

      case '5':
	SEND_TO_Q("\n\rEnter your password for verification: ", d);
	echo_off(d->descriptor);
	STATE(d) = CON_DELCNF1;
        break;

      default:
	 SEND_TO_Q("\n\rThat's not a menu choice!\n\r", d);
	 SEND_TO_Q(MENU, d);
	 break;
      }

      break;

   case CON_PWDNQO:
      /* skip whitespaces */
      for (; isspace(*arg); arg++) ;
      if (strncmp(CRYPT(arg,d->pwd), d->pwd, MAX_PWD_LENGTH)) {
	 SEND_TO_Q("\n\rIncorrect password.\n\r", d);
	 SEND_TO_Q(MENU, d);
	 STATE(d) = CON_SLCT;
	 echo_on(d->descriptor);
	 return;
      } else {
         SEND_TO_Q("\n\rEnter a new password: ", d);
         STATE(d) = CON_PWDNEW;
	 return;
      }
      break;
   case CON_PWDNEW:

      /* skip whitespaces */
      for (; isspace(*arg); arg++)
	 ;

      if (!*arg || strlen(arg) > MAX_PWD_LENGTH || strlen(arg) < 3 || 
          !str_cmp(arg, GET_NAME(d->character))) {
	 SEND_TO_Q("\n\rIllegal password.\n\r", d);
	 SEND_TO_Q("Password: ", d);
	 return;
      }

      strncpy(d->pwd, CRYPT(arg, d->character->player.name), MAX_PWD_LENGTH);
      *(d->pwd + MAX_PWD_LENGTH) = '\0';

      SEND_TO_Q("\n\rPlease retype password: ", d);
      STATE(d) = CON_PWDNCNF;
      break;

   case CON_PWDNCNF:
      /* skip whitespaces */
      for (; isspace(*arg); arg++)
	 ;

      if (strncmp(CRYPT(arg, d->pwd), d->pwd, MAX_PWD_LENGTH)) {
	 SEND_TO_Q("\n\rPasswords don't match... start over.\n\r", d);
	 SEND_TO_Q("Password: ", d);
	 STATE(d) = CON_PWDNEW;
	 return;
      }

      SEND_TO_Q("\n\rDone.  You must enter the game to make the change final.\n\r", d);
      SEND_TO_Q(MENU, d);
      echo_on(d->descriptor);
      STATE(d) = CON_SLCT;
      break;

   case CON_DELCNF1:
      echo_on(d->descriptor);
      for (; isspace(*arg); arg++);
      if (strncmp(CRYPT(arg,d->pwd), d->pwd, MAX_PWD_LENGTH)) {
	 SEND_TO_Q("\n\rIncorrect password.\n\r", d);
	 SEND_TO_Q(MENU, d);
	 STATE(d) = CON_SLCT;
      } else {
	 SEND_TO_Q("\n\rYOU ARE ABOUT TO DELETE THIS CHARACTER PERMANENTLY.\n\r"
		   "ARE YOU ABSOLUTELY SURE?\n\r\n\r"
		   "Please type \"yes\" to confirm: ", d);
	 STATE(d) = CON_DELCNF2;
      }
      break;

   case CON_DELCNF2:
      if (!strcmp(arg, "yes") || !strcmp(arg, "YES")) {
         if (PLR_FLAGGED(d->character, PLR_FROZEN)) {
            SEND_TO_Q("You try to kill yourself, but the ice stops you.\n\r",d);
            SEND_TO_Q("Character not deleted.\n\r\n\r",d);
            STATE(d) = CON_CLOSE;
            return;
	 }

         if (GET_LEVEL(d->character) < LEVEL_GRGOD)
            SET_BIT(PLR_FLAGS(d->character), PLR_DELETED);
         save_char(d->character, NOWHERE);
         Crash_delete_file(GET_NAME(d->character));
         sprintf(buf, "Character '%s' deleted!\n\rGoodbye.\n\r", GET_NAME(d->character));
         SEND_TO_Q(buf, d);
         sprintf(buf, "%s (lev %d) has self-deleted.", GET_NAME(d->character), GET_LEVEL(d->character));
         mudlog(buf, NRM, LEVEL_GOD, TRUE);
         STATE(d) = CON_CLOSE;
         return;
      } else {
	 SEND_TO_Q("Character not deleted.\n\r\n\r", d);
	 SEND_TO_Q(MENU, d);
	 STATE(d) = CON_SLCT;
      }
      break;

   case CON_CLOSE :
      close_socket(d);
      break;

   default:
      log("SYSERR: Nanny: illegal state of con'ness");
      abort();
      break;
   }
}

