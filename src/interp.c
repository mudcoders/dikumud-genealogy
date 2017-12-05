/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *  Mitchell Tse.                                                          *
 *                                                                         *
 *  EnvyMud 2.2 improvements copyright (C) 1996, 1997 by Michael Quan.     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



bool	check_social	args( ( CHAR_DATA *ch, char *command,
			       char *argument ) );

/*
 * Command logging types.
 */
#define LOG_NORMAL              0
#define LOG_ALWAYS              1
#define LOG_NEVER               2


/*
 * Log-all switch.
 */
bool				fLogAll		    = FALSE;



/*
 * Command table.
 */
const	struct	cmd_type	cmd_table	[ ] =
{
    /*
     * Common movement commands.
     */
    { "north",		do_north,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "east",		do_east,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "south",		do_south,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "west",		do_west,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "up",		do_up,		POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "down",		do_down,	POS_STANDING,	 0,  LOG_NORMAL, 1 },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "buy",		do_buy,		POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "cast",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "exits",		do_exits,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "get",		do_get,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "inventory",	do_inventory,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "immtalk",	do_immtalk,	POS_DEAD,    L_HER,  LOG_NORMAL, 1 },
    { "kill",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "look",		do_look,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "order",		do_order,	POS_RESTING,	 0,  LOG_ALWAYS, 1 },
    { "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "rest",		do_rest,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "shoot",		do_shoot,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "stand",		do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "tell",		do_tell,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "wield",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "wizhelp",	do_wizhelp,	POS_DEAD,    L_HER,  LOG_NORMAL, 1 },

    /*
     * Informational commands.
     */
    { "areas",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "bug",		do_bug,		POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "compare",	do_compare,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "consider",	do_consider,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "examine",	do_examine,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "help",		do_help,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "idea",		do_idea,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "report",		do_report,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "pagelength",     do_pagelen,     POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "read",		do_look,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "score",		do_score,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "slist",          do_slist,       POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "socials",	do_socials,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "time",		do_time,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "typo",		do_typo,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "weather",	do_weather,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "who",		do_who,		POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "whois",		do_whois,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "wizlist",        do_wizlist,     POS_DEAD,        0,  LOG_NORMAL, 1 },

    /*
     * Combat commands.
     */
    { "backstab",	do_backstab,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "bs",		do_backstab,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "berserk",	do_berserk,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "scan",		do_scan,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "circle",		do_circle,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "disarm",		do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "fee",    	do_fee, 	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },    
    { "feed",   	do_feed,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "flee",		do_flee,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "kick",		do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "dirt",		do_dirt,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "murder",		do_murder,	POS_FIGHTING,	 0,  LOG_ALWAYS, 1 },
    { "rescue",		do_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "stake",		do_stake,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "whirlwind",	do_whirlwind,	POS_STANDING,	 0,  LOG_NORMAL, 1 },

    /*
     * Configuration commands.
     */
    { "alia",		do_alia,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "alias",		do_alias,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "auto",           do_auto,        POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autoexit",       do_autoexit,    POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autogold",       do_autogold,    POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autoloot",       do_autoloot,    POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autosac",        do_autosac,     POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "blank",          do_blank,       POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "brief",          do_brief,       POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "channels",	do_channels,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "colour",         do_colour,      POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "combine",        do_combine,     POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "config",		do_config,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "description",	do_description,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "password",	do_password,	POS_DEAD,	 0,  LOG_NEVER,  1 },
    { "prompt",         do_prompt,      POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "title",		do_title,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "unalia",		do_unalia,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "unalias",	do_unalias,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "wimpy",		do_wimpy,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "history",	do_history,	POS_DEAD,	 0,  LOG_NORMAL, 1 },

    /*
     * Communication commands.
     */
    { "answer",		do_answer,	POS_SLEEPING,	 3,  LOG_NORMAL, 1 },
    { "auction",	do_auction,	POS_SLEEPING,	 3,  LOG_NORMAL, 1 },
    { "beep",		do_beep,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "chat",		do_chat,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { ".",		do_chat,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "emote",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { ",",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "gtell",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { ";",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "music",		do_music,	POS_SLEEPING,	 3,  LOG_NORMAL, 1 },
    { "note",		do_note,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "pose",		do_pose,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "question",	do_question,	POS_SLEEPING,	 3,  LOG_NORMAL, 1 },
    { "reply",		do_reply,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "say",		do_say,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "'",		do_say,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "shout",		do_shout,	POS_RESTING,	 3,  LOG_NORMAL, 1 },
    { "yell",		do_yell,	POS_RESTING,	 3,  LOG_NORMAL, 1 },
    { "grats",		do_grats,	POS_SLEEPING,	 3,  LOG_NORMAL, 1 },

    /*
     * Object manipulation commands.
     */
    { "brandish",	do_brandish,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "close",		do_close,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "donate",         do_donate,      POS_RESTING,     0,  LOG_NORMAL, 1 },
    { "drink",		do_drink,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "drop",		do_drop,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "eat",		do_eat,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "fill",		do_fill,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "give",		do_give,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "hold",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "list",		do_list,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "lock",		do_lock,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "open",		do_open,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "pick",		do_pick,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "put",		do_put,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "recite",		do_recite,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "remove",		do_remove,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "reload",		do_reload,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "sell",		do_sell,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "take",		do_get,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "sacrifice",	do_sacrifice,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "unlock",		do_unlock,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "value",		do_value,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "wear",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "zap",		do_zap,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "brew",		do_brew,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "scribe",		do_scribe,	POS_STANDING,	 0,  LOG_NORMAL, 1 },

    /*
     * Miscellaneous commands.
     */
    { "afk",            do_afk,         POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "bash",           do_bash,        POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "bet",            do_bet,         POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "chameleon power",do_chameleon,   POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "enter",		do_enter,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "follow",		do_follow,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "group",		do_group,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "heighten senses",do_heighten,    POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "hide",		do_hide,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "poison weapon",	do_poison_weapon,POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "practice",	do_practice,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "qui",		do_qui,		POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "quit",		do_quit,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "recall",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "/",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "register",	do_register,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "rent",		do_rent,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "retir",		do_retir,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "retire",		do_retire,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "save",		do_save,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "shadow form",    do_shadow,      POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "snare",		do_snare,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "sneak",		do_sneak,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "spells",         do_spells,      POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "split",		do_split,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "steal",		do_steal,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "train",		do_train,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "untangle",	do_untangle,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "visible",	do_visible,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "wager",          do_bet,         POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "where",		do_where,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "track",		do_track,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "mount",		do_mount,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "dismount",	do_dismount,	POS_STANDING,	 0,  LOG_NORMAL, 1 },

    /*
     * Immortal commands.
     */
    { "mudconfig",	do_mudconfig,	POS_DEAD,    L_DIR,  LOG_ALWAYS, 1 },
    { "advance",	do_advance,	POS_DEAD,    L_DIR,  LOG_ALWAYS, 1 },
    { "trust",		do_trust,	POS_DEAD,    L_DIR,  LOG_ALWAYS, 1 },
    { "delet",		do_delet,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "delete",		do_delete,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "allow",		do_allow,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "ban",		do_ban,		POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "deny",		do_deny,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "disconnect",	do_disconnect,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "force",		do_force,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "freeze",		do_freeze,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "imtlset",	do_imtlset,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "log",		do_log,		POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "mset",		do_mset,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "noemote",	do_noemote,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "notell",		do_notell,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "numlock",	do_numlock,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "oload",		do_oload,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "oset",		do_oset,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "pardon",		do_pardon,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "purge",		do_purge,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "reboo",		do_reboo,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "reboot",		do_reboot,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "restore",	do_restore,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "shutdow",	do_shutdow,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "shutdown",	do_shutdown,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "silence",	do_silence,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "sla",		do_sla,		POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "slay",		do_slay,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "sset",		do_sset,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "sstime",		do_sstime,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "transfer",	do_transfer,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "users",		do_users,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "wizify", 	do_wizify,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "wizlock",	do_wizlock,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1 },
    { "setclan",	do_setclan,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "makeclan",	do_makeclan,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },

    { "astat",		do_astat,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "echo",		do_echo,	POS_DEAD,    L_JUN,  LOG_ALWAYS, 1 },
    { "memory",		do_memory,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "mload",		do_mload,	POS_DEAD,    L_JUN,  LOG_ALWAYS, 1 },
    { "mfind",		do_mfind,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "mstat",		do_mstat,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "mwhere",		do_mwhere,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "newlock",	do_newlock,	POS_DEAD,    L_JUN,  LOG_ALWAYS, 1 },
    { "ofind",		do_ofind,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "ostat",		do_ostat,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "owhere",		do_owhere,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "peace",		do_peace,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "recho",		do_recho,	POS_DEAD,    L_JUN,  LOG_ALWAYS, 1 },
    { "return",		do_return,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "rset",		do_rset,	POS_DEAD,    L_JUN,  LOG_ALWAYS, 1 },
    { "rstat",		do_rstat,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "clookup",	do_clookup,	POS_DEAD,    L_APP,  LOG_NORMAL, 1 },
    { "slookup",	do_slookup,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "sober",		do_sober,	POS_DEAD,    L_APP,  LOG_NORMAL, 1 },
    { "snoop",		do_snoop,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "switch",		do_switch,	POS_DEAD,    L_JUN,  LOG_ALWAYS, 1 },
    { "wizinvis",	do_invis,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },

    { "at",		do_at,		POS_DEAD,    L_APP,  LOG_NORMAL, 1 },
    { "bamfin",		do_bamfin,	POS_DEAD,    L_APP,  LOG_NORMAL, 1 },
    { "bamfout",	do_bamfout,	POS_DEAD,    L_APP,  LOG_NORMAL, 1 },
    { "goto",		do_goto,	POS_DEAD,    L_APP,  LOG_NORMAL, 1 },
    { "holylight",	do_holylight,	POS_DEAD,    L_APP,  LOG_NORMAL, 1 },
    { "showclan",	do_showclan,	POS_DEAD,    L_APP,  LOG_NORMAL, 1 },

    { ":",		do_immtalk,	POS_DEAD,    L_HER,  LOG_NORMAL, 1 },
    { "wiznet",		do_wiznet,	POS_DEAD,    L_HER,  LOG_NORMAL, 1 },

    /*
     * Clansman commands.
     */
    { "initiate",	do_initiate,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "exil",		do_exil,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "exile",		do_exile,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "promote",	do_promote,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "demote",		do_demote,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "crecall",	do_crecall,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "cdonate",	do_cdonate,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "leav",		do_leav,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "leave",		do_leave,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "claninfo",	do_claninfo,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "guilds",		do_guilds,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "orders",		do_orders,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "clans",		do_clans,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "wartalk",	do_wartalk,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
  
    /*
     * MOBprogram commands.
     */
    { "mpstat",         do_mpstat,      POS_DEAD,    L_JUN,  LOG_NORMAL, 1 },
    { "mpasound",       do_mpasound,    POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mpjunk",         do_mpjunk,      POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mpecho",         do_mpecho,      POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mpechoat",       do_mpechoat,    POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mpechoaround",   do_mpechoaround,POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mpkill",         do_mpkill      ,POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mpmload",        do_mpmload     ,POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mpoload",        do_mpoload     ,POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mppurge",        do_mppurge     ,POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mpgoto",         do_mpgoto      ,POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mpat",           do_mpat        ,POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mptransfer",     do_mptransfer  ,POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "mpforce",        do_mpforce     ,POS_DEAD,        0,  LOG_NORMAL, 0 },

    { "aedit",          do_aedit,       POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "redit",          do_redit,       POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "oedit",          do_oedit,       POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "medit",          do_medit,       POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "asave",          do_asave,       POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "alist",          do_alist,       POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "resets",         do_resets,      POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },
    { "mpedit",         do_mpedit,      POS_DEAD,    L_SEN,  LOG_NORMAL, 1 },

    /*
     * End of list.
     */
    { "",		0,		POS_DEAD,	 0,  LOG_NORMAL, 1 }
};



/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
    char command [ MAX_INPUT_LENGTH ];
    char logline [ MAX_INPUT_LENGTH ];
    int  cmd;
    int  trust;
    bool found;

    /*
     * Strip leading spaces.
     */
    while ( isspace( *argument ) )
	argument++;
    if ( argument[0] == '\0' )
	return;

    /*
     * No hiding and remove AFK
     */
    REMOVE_BIT( ch->affected_by, AFF_HIDE );
    if ( !IS_NPC( ch ) )
        REMOVE_BIT( ch->act, PLR_AFK );

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_FREEZE ) )
    {
	send_to_char( "You're totally frozen!\n\r", ch );
	return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if ( !isalpha( argument[0] ) && !isdigit( argument[0] ) )
    {
	command[0] = argument[0];
	command[1] = '\0';
	argument++;
	while ( isspace( *argument ) )
	    argument++;
    }
    else
    {
	argument = one_argument( argument, command );
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == cmd_table[cmd].name[0]
	    && !str_prefix( command, cmd_table[cmd].name )
	    && ( cmd_table[cmd].level <= trust ) )

	{
		found = TRUE;
	    break;
	}
    }

    /*
     * Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
	strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

    if ( ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_LOG ) )
	|| fLogAll
	|| cmd_table[cmd].log == LOG_ALWAYS )
    {
	sprintf( log_buf, "Log %s: %s", ch->name, logline );
	log_string( log_buf );
	wiznet( ch, WIZ_SECURE, get_trust( ch ), log_buf );
    }

    if ( ch->desc && ch->desc->snoop_by )
    {
	write_to_buffer( ch->desc->snoop_by, "% ",    2 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    if ( !found )
    {
	/*
	 * Look for command in socials table.
	 */
	if ( !check_social( ch, command, argument ) )
	    send_to_char( "Huh?\n\r", ch );
	return;
    }

    /*
     * Character not in position for command?
     */
    if ( ch->position < cmd_table[cmd].position )
    {
	switch( ch->position )
	{
	case POS_DEAD:
	    send_to_char( "Lie still; you are DEAD.\n\r", ch );
	    break;

	case POS_MORTAL:
	case POS_INCAP:
	    send_to_char( "You are hurt far too bad for that.\n\r", ch );
	    break;

	case POS_STUNNED:
	    send_to_char( "You are too stunned to do that.\n\r",    ch );
	    break;

	case POS_SLEEPING:
	    send_to_char( "In your dreams, or what?\n\r",           ch );
	    break;

	case POS_RESTING:
	    send_to_char( "Nah... You feel too relaxed...\n\r",     ch );
	    break;

	case POS_FIGHTING:
	    send_to_char( "No way!  You are still fighting!\n\r",   ch );
	    break;

	}
	return;
    }

    /*
     * Dispatch the command.
     */
    (*cmd_table[cmd].do_fun) ( ch, argument );

    tail_chain( );
    return;
}



/*
 * Cut & pasted from SMAUG (Zen)
 */
SOC_INDEX_DATA *find_social( char *command )
{
    SOC_INDEX_DATA *social;
    int hash;

    if ( command[0] < 'a' || command[0] > 'z' )
        hash = 0;
    else
        hash = (command[0] - 'a') + 1;

    for ( social = soc_index_hash[hash]; social; social = social->next )
        if ( !str_prefix( command, social->name ) )
            return social;

    return NULL;
}



bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    CHAR_DATA   *victim;
    char         arg [ MAX_INPUT_LENGTH ];
    SOC_INDEX_DATA *soc;

    if ( !( soc = find_social( command ) ) )
	return FALSE;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
    {
	send_to_char( "You are anti-social!\n\r", ch );
	return TRUE;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_char( "Lie still; you are DEAD.\n\r",             ch );
	return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
	send_to_char( "You are hurt far too badly for that.\n\r", ch );
	return TRUE;

    case POS_STUNNED:
	send_to_char( "You are too stunned to do that.\n\r",      ch );
	return TRUE;

    case POS_SLEEPING:
	/*
	 * I just know this is the path to a 12" 'if' statement.  :(
	 * But two players asked for it already!  -- Furey
	 */
	if ( !str_cmp( soc->name, "snore" ) )
	    break;
	send_to_char( "In your dreams, or what?\n\r",             ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
	act( soc->char_no_arg,   ch, NULL, victim, TO_CHAR    );
	act( soc->others_no_arg, ch, NULL, victim, TO_ROOM    );
    }
    else if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r",                    ch );
    }
    else if ( victim == ch )
    {
	act( soc->char_auto,     ch, NULL, victim, TO_CHAR    );
	act( soc->others_auto,   ch, NULL, victim, TO_ROOM    );
    }
    else if ( !get_char_room( ch, arg ) && can_see( ch, victim ) )
    {
	if ( !IS_NPC( victim ) )
        {
	    ROOM_INDEX_DATA *original;
	    char            *ldbase                      = "From far away, ";
	    char             ldmsg [ MAX_STRING_LENGTH ];

	    original = ch->in_room;
	    char_from_room( ch );
	    char_to_room( ch, victim->in_room );

	    strcpy( ldmsg, ldbase );
	    strcat( ldmsg, soc->char_found );
	    act( ldmsg,                       ch, NULL, victim, TO_CHAR    );

	    strcpy( ldmsg, ldbase );
	    strcat( ldmsg, soc->vict_found );
	    act( ldmsg,                       ch, NULL, victim, TO_VICT    );

	    char_from_room( ch );
	    char_to_room( ch, original );
	}
	else
	{
	    send_to_char( "They aren't here.\n\r",                ch );
	}
    }
    else
    {
	act( soc->char_found,    ch, NULL, victim, TO_CHAR    );
	act( soc->vict_found,    ch, NULL, victim, TO_VICT    );
	act( soc->others_found,  ch, NULL, victim, TO_NOTVICT );

	if ( !IS_NPC( ch )
	    && IS_NPC( victim )
	    && !IS_AFFECTED( victim, AFF_CHARM )
	    && IS_AWAKE( victim ) )
	{
	    switch ( number_bits( 4 ) )
	    {
	    case 0:
	        if ( ( victim->level < ch->level )
		    && !( victim->fighting ) )
		    multi_hit( victim, ch, TYPE_UNDEFINED );
		break;

	    case 1: case 2: case 3: case 4:
	    case 5: case 6: case 7: case 8:
		act( soc->char_found,
		    victim, NULL, ch, TO_CHAR    );
		act( soc->vict_found,
		    victim, NULL, ch, TO_VICT    );
		act( soc->others_found,
		    victim, NULL, ch, TO_NOTVICT );
		break;

	    case 9: case 10: case 11: case 12:
		act( "You slap $N.",  victim, NULL, ch, TO_CHAR    );
		act( "$n slaps you.", victim, NULL, ch, TO_VICT    );
		act( "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
		break;
	    }
	}
    }

    return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number( char *arg )
{
    if ( *arg == '\0' )
	return FALSE;

    if ( *arg == '+' || *arg == '-' )
        arg++;

    for ( ; *arg != '\0'; arg++ )
    {
	if ( !isdigit( *arg ) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int   number;
    
    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    }

    strcpy( arg, argument );
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace( *argument ) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER( *argument );
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace( *argument ) )
	argument++;

    return argument;
}


/*
 * Zen's history code; see also read_from_buffer( ) in comm.c
 */
void do_history( CHAR_DATA *ch, char *argument )
{
    HISTORY_DATA *history;
    int           num;
    char          buf [ MAX_STRING_LENGTH ];

    if ( !ch->desc )
	return;

    for ( history = ch->desc->infirst, num = 0; history;
	 history = history->next, num++ )
    {
	sprintf( buf, " {o{y%4d:{x %s\n\r", num, history->comm );
	send_to_char( buf, ch );
    }

    return;
}


/*
 * "alia" command is a trap to the "alias" command.
 */
void do_alia( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to ALIAS, you have to spell it out.\n\r", ch );

    return;
}


/*
 * "unalia" command is a trap to the "unalias" command.
 */
void do_unalia( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to UNALIAS, you have to spell it out.\n\r", ch );

    return;
}


/***************************************************************************
*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@efn.org)				   *
*	    Gabrielle Taylor						   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/*
 * does aliasing and other fun stuff.
 * enhanced by Zen so aliases are able to call other aliases.
 */
void substitute_alias( DESCRIPTOR_DATA *d, char *argument )
{
    CHAR_DATA *ch;
    char      *point;
    char      *rest;
    char       buf  [ MAX_STRING_LENGTH ];
    char       name [ MAX_STRING_LENGTH ];
    char       part [ MAX_STRING_LENGTH ];
    char       arg  [ MAX_STRING_LENGTH ];
    int        alias;
    int        num;
    bool       found;

    ch = ( d->original ? d->original : d->character );

    if ( IS_NPC( ch )
	|| !ch->pcdata->alias[0]
	|| !str_prefix( "alias", argument )
	|| !str_prefix( "unalias", argument ) )
    {
	interpret( d->character, argument );
	return;
    }

    strcpy( buf, argument );

    for ( alias = 0; alias < MAX_ALIAS; alias++ ) /* go through the aliases */
    {
	if ( !ch->pcdata->alias[alias] )
	    break;

	if ( !str_prefix( ch->pcdata->alias[alias], argument ) )
	{
	    point = one_argument( argument, name );
	    if ( !strcmp( ch->pcdata->alias[alias], name ) )
	    {

		strcpy( arg, ch->pcdata->alias_sub[alias] );
		buf[0] = '\0';

		/* What the hell is this??? :) - Zen */
		for ( rest = arg, rest = one_argument( rest, part );
			part[0] != '\0';
			rest = one_argument( rest, part ) )
		{
		    found = FALSE;
		    for ( num = 0; num < MAX_ALIAS; num++ )
		    {
			if ( !ch->pcdata->alias[num] )
			    break;

			if ( !str_prefix( ch->pcdata->alias[num], part )
			     && str_prefix( ch->pcdata->alias[num], name )
			     && !strchr( part, ' ' ) )
			{
			    strcat( buf, ch->pcdata->alias_sub[num] );
			    found = TRUE;
			    break;
			}
		    }

		    if ( strchr ( part, ' ' ) )
			strcat( buf, "'" );
		    if ( !found )
			strcat( buf, part );
		    if ( strchr ( part, ' ' ) )
			strcat( buf, "'" );

		    strcat( buf, " " );
		}		

		strcat( buf, point );
		break;
	    }

	    if ( strlen( buf ) > MAX_INPUT_LENGTH )
	    {
		send_to_char( "Alias substitution too long. Truncated.\r\n",
			     ch );
		buf [ MAX_INPUT_LENGTH - 1 ] = '\0';
	    }
	}
    }

    interpret( d->character, buf );
    return;
}


void do_alias( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    int        pos;

    smash_tilde( argument );

    rch = get_char( ch );

    if ( IS_NPC( rch ) )
	return;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( !rch->pcdata->alias[0] )
	{
	    send_to_char( "You have no aliases defined.\n\r", ch );
	    return;
	}
	send_to_char( "Your current aliases are:\n\r", ch );

	for ( pos = 0; pos < MAX_ALIAS; pos++ )
	{
	    if ( !rch->pcdata->alias[pos]
		|| !rch->pcdata->alias_sub[pos] )
		break;

	    sprintf( buf, "alias '{o{c%s{x' to {o{y%s{x\n\r",
		    rch->pcdata->alias[pos], rch->pcdata->alias_sub[pos] );
	    send_to_char( buf, ch );
	}

	return;
    }

    if ( !str_prefix( "unalias", arg ) || !str_cmp( "alias", arg ) )
    {
	send_to_char( "Sorry, that word is reserved.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	for ( pos = 0; pos < MAX_ALIAS; pos++ )
	{
	    if ( !rch->pcdata->alias[pos]
		|| !rch->pcdata->alias_sub[pos] )
		break;

	    if ( !str_cmp( arg, rch->pcdata->alias[pos] ) )
	    {
		sprintf( buf, "{o{c%s{x aliases to {o{y%s{x.\n\r",
			rch->pcdata->alias[pos], rch->pcdata->alias_sub[pos] );
		send_to_char( buf, ch );
		return;
	    }
	}

	send_to_char( "That alias is not defined.\n\r", ch );
	return;
    }

    if ( !str_prefix( argument, "delete" )
	|| !str_prefix( argument, "retire" ) )
    {
	send_to_char( "That shall not be done!\n\r", ch );
	return;
    }

    for ( pos = 0; pos < MAX_ALIAS; pos++ )
    {
	if ( !rch->pcdata->alias[pos] )
	    break;

	if ( !str_cmp( arg, rch->pcdata->alias[pos] ) ) /* redefine an alias */
	{
	    free_string( rch->pcdata->alias_sub[pos] );
	    rch->pcdata->alias_sub[pos] = str_dup( argument );
	    sprintf( buf, "{o{c%s{x is now realiased to {o{y%s{x.\n\r", arg,
		    argument );
	    send_to_char( buf, ch );
	    return;
	}
    }

    if ( pos >= MAX_ALIAS )
    {
	send_to_char( "Sorry, you have reached the alias limit.\n\r", ch );
	return;
    }

    /* make a new alias */
    rch->pcdata->alias[pos]		= str_dup( arg );
    rch->pcdata->alias_sub[pos]		= str_dup( argument );
    sprintf( buf, "{o{c%s{x is now aliased to {o{y%s{x.\n\r", arg, argument );
    send_to_char( buf, ch );
    return;
}


void do_unalias( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        pos;
    bool       found = FALSE;
 
    rch = get_char( ch );
 
    if ( IS_NPC( rch ) )
	return;
 
    argument = one_argument( argument, arg );

    if ( arg == '\0' )
    {
	send_to_char( "Syntax: unalias <word>.\n\r", ch );
	return;
    }

    for ( pos = 0; pos < MAX_ALIAS; pos++ )
    {
	if ( !rch->pcdata->alias[pos] )
	    break;

	if ( found )
	{
	    rch->pcdata->alias[pos-1]		= rch->pcdata->alias[pos];
	    rch->pcdata->alias_sub[pos-1]	= rch->pcdata->alias_sub[pos];
	    rch->pcdata->alias[pos]		= NULL;
	    rch->pcdata->alias_sub[pos]		= NULL;
	    continue;
	}

	if( !strcmp( arg, rch->pcdata->alias[pos] ) )
	{
	    send_to_char( "Alias removed.\n\r", ch );
	    free_string( rch->pcdata->alias[pos] );
	    free_string( rch->pcdata->alias_sub[pos] );
	    rch->pcdata->alias[pos] = NULL;
	    rch->pcdata->alias_sub[pos] = NULL;
	    found = TRUE;
	}
    }

    if ( !found )
	send_to_char( "No alias of that name to remove.\n\r", ch );

    return;
}
