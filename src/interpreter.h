/* ************************************************************************
*   File: interpreter.h                                 Part of CircleMUD *
*  Usage: header file: public procs, macro defs, subcommand defines       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#define NOT !
#define AND &&
#define OR ||

#define STATE(d) ((d)->connected)

#define MAX_CMD_LIST	350

#define ACMD(c)  \
   void (c)(struct char_data *ch,char *argument,int cmd,int subcmd)
#define CRYPT(a,b) ((char *) crypt((a),(b))) 
#define SPECIAL(c) int (c)(struct char_data *ch, int cmd, char *arg)

#define ASSIGNMOB(mob, fname) { if (real_mobile(mob) >= 0) \
				mob_index[real_mobile(mob)].func = fname; }

#define ASSIGNOBJ(obj, fname) { if (real_object(obj) >= 0) \
				obj_index[real_object(obj)].func = fname; }

#define ASSIGNROOM(room, fname) { if (real_room(room) >= 0) \
				world[real_room(room)].funct = fname; }

void	command_interpreter(struct char_data *ch, char *argument);
int	search_block(char *arg, char **list, bool exact);
int	old_search_block(char *argument, int begin, int length, char **list, int mode);
char	lower( char c );
void	argument_interpreter(char *argument, char *first_arg, char *second_arg);
char	*one_argument(char *argument, char *first_arg);
int	fill_word(char *argument);
void	half_chop(char *string, char *arg1, char *arg2);
void	nanny(struct descriptor_data *d, char *arg);
int	is_abbrev(char *arg1, char *arg2);
int	is_number(char *str);

struct command_info {
   void	(*command_pointer)
   (struct char_data *ch, char * argument, int cmd, int subcmd);
   byte minimum_position;
   sh_int minimum_level;
   sh_int sort_pos;
   int	subcmd;
   byte is_social;
};


#define SCMD_INFO       100
#define SCMD_HANDBOOK   101 
#define SCMD_CREDITS    102
#define SCMD_NEWS       103
#define SCMD_WIZLIST    104
#define SCMD_POLICIES   105
#define SCMD_VERSION    106
#define SCMD_IMMLIST    107
#define SCMD_CLEAR	108
#define SCMD_WHOAMI	109

#define SCMD_TOG_BASE	201
#define SCMD_NOSUMMON   201
#define SCMD_NOHASSLE   202
#define SCMD_BRIEF      203
#define SCMD_COMPACT    204
#define SCMD_NOTELL	205
#define SCMD_NOAUCTION	206
#define SCMD_DEAF	207
#define SCMD_NOGOSSIP	208
#define SCMD_NOGRATZ	209
#define SCMD_NOWIZ	210
#define SCMD_QUEST	211
#define SCMD_ROOMFLAGS	212
#define SCMD_NOREPEAT	213
#define SCMD_HOLYLIGHT	214
#define SCMD_SLOWNS	215

#define SCMD_PARDON     301
#define SCMD_NOTITLE    302
#define SCMD_SQUELCH    303
#define SCMD_FREEZE	304
#define SCMD_THAW	305
#define SCMD_UNAFFECT	306
#define SCMD_REROLL	307

#define SCMD_HOLLER	0
#define SCMD_SHOUT	1
#define SCMD_GOSSIP	2
#define SCMD_AUCTION	3
#define SCMD_GRATZ	4

#define SCMD_SHUTDOWN   1
#define SCMD_QUIT	1

#define SCMD_COMMANDS	0
#define SCMD_SOCIALS	1
#define SCMD_WIZHELP	2

#define SCMD_DROP	0
#define SCMD_JUNK	1
#define SCMD_DONATE	2

#define SCMD_BUG	0
#define SCMD_TYPO	1
#define SCMD_IDEA	2

#define SCMD_GECHO	0
#define SCMD_QECHO	1

#define SCMD_POUR	0
#define SCMD_FILL	1

#define SCMD_POOFIN	0
#define SCMD_POOFOUT	1

#define SCMD_HIT	0
#define SCMD_MURDER	1

#define SCMD_EAT	0
#define SCMD_TASTE	1
#define SCMD_DRINK	2
#define SCMD_SIP	3
