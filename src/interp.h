/***************************************************************************
 *  file: Interpreter.h , Command interpreter module.      Part of DIKUMUD *
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

void command_interpreter(struct char_data *ch, char *argument);
int search_block(char *arg, char **list, bool exact);
int old_search_block(char *argument,int begin,int length,char **list,int mode);
char lower( char c );
void argument_interpreter(char *argument, char *first_arg, char *second_arg);
char *one_argument(char *argument,char *first_arg);
int fill_word(char *argument);
void half_chop(char *string, char *arg1, char *arg2);
void nanny(struct descriptor_data *d, char *arg);
int is_abbrev(char *arg1, char *arg2);



/*
 * Command functions.
 */
typedef void    DO_FUN  (struct char_data *ch, char *argument, int cmd);

DO_FUN  do_action;
DO_FUN  do_advance;
DO_FUN  do_allow;
DO_FUN  do_answer;
DO_FUN  do_ask;
DO_FUN  do_assist;
DO_FUN  do_at;
DO_FUN  do_auction;
DO_FUN  do_autoassist;
DO_FUN  do_autoexit;
DO_FUN  do_autogold;
DO_FUN  do_autolist;
DO_FUN  do_autoloot;
DO_FUN  do_autosac;
DO_FUN  do_autosplit;
DO_FUN  do_backstab;
DO_FUN  do_ban;
DO_FUN  do_bash;
DO_FUN  do_brief;
DO_FUN  do_bug;
DO_FUN  do_cast;
DO_FUN  do_channels;
DO_FUN  do_close;
DO_FUN  do_commands;
DO_FUN  do_compact;
DO_FUN  do_consider;
DO_FUN  do_credits;
DO_FUN  do_deaf;
DO_FUN  do_disarm;
DO_FUN  do_disconnect;
DO_FUN  do_drink;
DO_FUN  do_drop;
DO_FUN  do_eat;
DO_FUN  do_echo;
DO_FUN  do_emote;
DO_FUN  do_enter;
DO_FUN  do_equipment;
DO_FUN  do_erase;
DO_FUN  do_examine;
DO_FUN  do_exit;
DO_FUN  do_exits;
DO_FUN  do_fill;
DO_FUN  do_flee;
DO_FUN  do_follow;
DO_FUN  do_force;
DO_FUN  do_freeze;
DO_FUN  do_gecho;
DO_FUN  do_get;
DO_FUN  do_give;
DO_FUN  do_gossip;
DO_FUN  do_goto;
DO_FUN  do_grab;
DO_FUN  do_group;
DO_FUN  do_grouptell;
DO_FUN  do_help;
DO_FUN  do_hide;
DO_FUN  do_hit;
DO_FUN  do_holylite;
DO_FUN  do_idea;
DO_FUN  do_imotd;
DO_FUN  do_info;
DO_FUN  do_insult;
DO_FUN  do_inventory;
DO_FUN  do_kick;
DO_FUN  do_kill;
DO_FUN  do_leave;
DO_FUN  do_levels;
DO_FUN  do_load;
DO_FUN  do_lock;
DO_FUN  do_log;
DO_FUN  do_look;
DO_FUN  do_motd;
DO_FUN  do_move;
DO_FUN  do_murder;
DO_FUN  do_music;
DO_FUN  do_newlock;
DO_FUN  do_news;
DO_FUN  do_nochannels;
DO_FUN  do_noemote;
DO_FUN  do_nofollow;
DO_FUN  do_noloot;
DO_FUN  do_noshout;
DO_FUN  do_nosummon;
DO_FUN  do_not_here;
DO_FUN  do_notell;
DO_FUN  do_number;
DO_FUN  do_offer;
DO_FUN  do_open;
DO_FUN  do_order;
DO_FUN  do_pardon;
DO_FUN  do_pick;
DO_FUN  do_pose;
DO_FUN  do_pour;
DO_FUN  do_practice;
DO_FUN  do_purge;
DO_FUN  do_put;
DO_FUN  do_quaff;
DO_FUN  do_question;
DO_FUN  do_qui;
DO_FUN  do_quiet;
DO_FUN  do_quit;
DO_FUN  do_read;
DO_FUN	do_recall;
DO_FUN  do_recite;
DO_FUN  do_remove;
DO_FUN  do_rent;
DO_FUN  do_report;
DO_FUN  do_reroll;
DO_FUN  do_rescue;
DO_FUN  do_rest;
DO_FUN  do_restore;
DO_FUN  do_return;
DO_FUN  do_save;
DO_FUN  do_say;
DO_FUN  do_score;
DO_FUN  do_scroll;
DO_FUN  do_set;
DO_FUN  do_shout;
DO_FUN  do_shutdow;
DO_FUN  do_shutdown;
DO_FUN  do_sip;
DO_FUN  do_sit;
DO_FUN  do_sleep;
DO_FUN  do_sneak;
DO_FUN  do_snoop;
DO_FUN  do_sockets;
DO_FUN  do_split;
DO_FUN  do_stand;
DO_FUN  do_stat;
DO_FUN  do_steal;
DO_FUN  do_story;
DO_FUN  do_string;
DO_FUN  do_switch;
DO_FUN  do_tap;
DO_FUN  do_taste;
DO_FUN  do_teleport;
DO_FUN  do_tell;
DO_FUN	do_tick;
DO_FUN  do_time;
DO_FUN  do_title;
DO_FUN  do_trans;
DO_FUN  do_trip;
DO_FUN  do_typo;
DO_FUN  do_unlock;
DO_FUN  do_use;
DO_FUN  do_visible;
DO_FUN  do_wake;
DO_FUN  do_wear;
DO_FUN  do_weather;
DO_FUN  do_where;
DO_FUN  do_whisper;
DO_FUN  do_who;
DO_FUN  do_wield;
DO_FUN  do_wimpy;
DO_FUN  do_wiz;
DO_FUN  do_wizhelp;
DO_FUN  do_wizinvis;
DO_FUN  do_wizlist;
DO_FUN  do_wizlock;
DO_FUN  do_write;
