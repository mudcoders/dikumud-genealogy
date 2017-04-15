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
 * God Levels - Check them out in merc.h
*/

#define L_HER                   LEVEL_HERO

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
    { "north",		do_north,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "east",		do_east,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "south",		do_south,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "west",		do_west,	POS_STANDING,	 0,  LOG_NORMAL },
    { "up",		do_up,		POS_STANDING,	 0,  LOG_NORMAL },
    { "down",		do_down,	POS_STANDING,	 0,  LOG_NORMAL },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "buy",		do_buy,		POS_STANDING,	 0,  LOG_NORMAL	},
    { "cast",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "exits",		do_exits,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "get",		do_get,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "inventory",	do_inventory,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "immtalk",	do_immtalk,	POS_DEAD,    L_HER,  LOG_NORMAL	},
    { "kill",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "look",		do_look,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "order",		do_order,	POS_RESTING,	 0,  LOG_ALWAYS	},
    { "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "rest",		do_rest,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "stand",		do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "tell",		do_tell,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "wield",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "wizhelp",	do_wizhelp,	POS_DEAD,    L_HER,  LOG_NORMAL	},

    /*
     * Informational commands.
     */
    { "areas",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "bug",		do_bug,		POS_DEAD,	 0,  LOG_NORMAL	},
    { "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL },
    { "compare",	do_compare,	POS_RESTING,	 0,  LOG_NORMAL },
    { "consider",	do_consider,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "examine",	do_examine,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "help",		do_help,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "idea",		do_idea,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "report",		do_report,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "pagelength",     do_pagelen,     POS_DEAD,        0,  LOG_NORMAL },
    { "read",		do_look,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "score",		do_score,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "slist",          do_slist,       POS_DEAD,        0,  LOG_NORMAL },
    { "socials",	do_socials,	POS_DEAD,	 0,  LOG_NORMAL },
    { "time",		do_time,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "typo",		do_typo,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "weather",	do_weather,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "who",		do_who,		POS_DEAD,	 0,  LOG_NORMAL	},
    { "whois",		do_whois,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "wizlist",        do_wizlist,     POS_DEAD,        0,  LOG_NORMAL },

    /*
     * Combat commands.
     */
    { "backstab",	do_backstab,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "bs",		do_backstab,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "berserk",	do_berserk,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "circle",		do_circle,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "disarm",		do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "fee",    	do_fee, 	POS_FIGHTING,	 0,  LOG_NORMAL	},    
    { "feed",   	do_feed,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "flee",		do_flee,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "kick",		do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "murder",		do_murder,	POS_FIGHTING,	 0,  LOG_ALWAYS	},
    { "rescue",		do_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "stake",		do_stake,	POS_STANDING,	 0,  LOG_NORMAL	},

    /*
     * Configuration commands.
     */
    { "auto",           do_auto,        POS_DEAD,        0,  LOG_NORMAL },
    { "autoexit",       do_autoexit,    POS_DEAD,        0,  LOG_NORMAL },
    { "autogold",       do_autogold,    POS_DEAD,        0,  LOG_NORMAL },
    { "autoloot",       do_autoloot,    POS_DEAD,        0,  LOG_NORMAL },
    { "autosac",        do_autosac,     POS_DEAD,        0,  LOG_NORMAL },
    { "blank",          do_blank,       POS_DEAD,        0,  LOG_NORMAL },
    { "brief",          do_brief,       POS_DEAD,        0,  LOG_NORMAL },
    { "channels",	do_channels,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "combine",        do_combine,     POS_DEAD,        0,  LOG_NORMAL },
    { "config",		do_config,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "description",	do_description,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "password",	do_password,	POS_DEAD,	 0,  LOG_NEVER	},
    { "prompt",         do_prompt,      POS_DEAD,        0,  LOG_NORMAL },
    { "title",		do_title,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "wimpy",		do_wimpy,	POS_DEAD,	 0,  LOG_NORMAL	},

    /*
     * Communication commands.
     */
    { "answer",		do_answer,	POS_SLEEPING,	 3,  LOG_NORMAL },
    { "auction",	do_auction,	POS_SLEEPING,	 3,  LOG_NORMAL	},
    { "beep",		do_beep,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "chat",		do_chat,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { ".",		do_chat,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "emote",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL	},
    { ",",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "gtell",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL	},
    { ";",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "music",		do_music,	POS_SLEEPING,	 3,  LOG_NORMAL },
    { "note",		do_note,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "pose",		do_pose,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "question",	do_question,	POS_SLEEPING,	 3,  LOG_NORMAL },
    { "reply",		do_reply,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "say",		do_say,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "'",		do_say,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "shout",		do_shout,	POS_RESTING,	 3,  LOG_NORMAL	},
    { "yell",		do_yell,	POS_RESTING,	 3,  LOG_NORMAL	},

    /*
     * Object manipulation commands.
     */
    { "brandish",	do_brandish,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "close",		do_close,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "donate",         do_donate,      POS_RESTING,     0,  LOG_NORMAL },
    { "drink",		do_drink,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "drop",		do_drop,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "eat",		do_eat,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "fill",		do_fill,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "give",		do_give,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "hold",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "list",		do_list,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "lock",		do_lock,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "open",		do_open,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "pick",		do_pick,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "put",		do_put,		POS_RESTING,	 0,  LOG_NORMAL },
    { "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "recite",		do_recite,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "remove",		do_remove,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "sell",		do_sell,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "take",		do_get,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "sacrifice",	do_sacrifice,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "unlock",		do_unlock,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "value",		do_value,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "wear",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "zap",		do_zap,		POS_RESTING,	 0,  LOG_NORMAL	},

    /*
     * Miscellaneous commands.
     */
    { "afk",            do_afk,         POS_SLEEPING,    0,  LOG_NORMAL },
    { "bash",           do_bash,        POS_STANDING,    0,  LOG_NORMAL },
    { "bet",            do_bet,         POS_STANDING,    0,  LOG_NORMAL },
    { "wager",          do_bet,         POS_STANDING,    0,  LOG_NORMAL },
    { "chameleon power",do_chameleon,   POS_STANDING,    0,  LOG_NORMAL },
    { "follow",		do_follow,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "group",		do_group,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "heighten senses",do_heighten,    POS_STANDING,    0,  LOG_NORMAL },
    { "hide",		do_hide,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "poison weapon",	do_poison_weapon,POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "practice",	do_practice,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "qui",		do_qui,		POS_DEAD,	 0,  LOG_NORMAL	},
    { "quit",		do_quit,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "recall",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "/",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "register",	do_register,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "rent",		do_rent,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "save",		do_save,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "shadow form",    do_shadow,      POS_STANDING,    0,  LOG_NORMAL },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "snare",		do_snare,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "sneak",		do_sneak,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "spells",         do_spells,      POS_SLEEPING,    0,  LOG_NORMAL },
    { "split",		do_split,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "steal",		do_steal,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "train",		do_train,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "untangle",	do_untangle,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "visible",	do_visible,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "where",		do_where,	POS_RESTING,	 0,  LOG_NORMAL	},

    /*
     * Immortal commands.
     */
    { "advance",	do_advance,	POS_DEAD,    L_DIR,  LOG_ALWAYS	},
    { "trust",		do_trust,	POS_DEAD,    L_DIR,  LOG_ALWAYS },

    { "allow",		do_allow,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "ban",		do_ban,		POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "deny",		do_deny,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "disconnect",	do_disconnect,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "force",		do_force,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "freeze",		do_freeze,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "imtlset",	do_imtlset,	POS_DEAD,    L_SEN,  LOG_ALWAYS },
    { "log",		do_log,		POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "mset",		do_mset,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "noemote",	do_noemote,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "notell",		do_notell,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "numlock",	do_numlock,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "oload",		do_oload,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "oset",		do_oset,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "pardon",		do_pardon,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "purge",		do_purge,	POS_DEAD,    L_SEN,  LOG_NORMAL	},
    { "reboo",		do_reboo,	POS_DEAD,    L_SEN,  LOG_NORMAL	},
    { "reboot",		do_reboot,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "restore",	do_restore,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "shutdow",	do_shutdow,	POS_DEAD,    L_SEN,  LOG_NORMAL	},
    { "shutdown",	do_shutdown,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "silence",	do_silence,	POS_DEAD,    L_SEN,  LOG_ALWAYS },
    { "sla",		do_sla,		POS_DEAD,    L_SEN,  LOG_NORMAL	},
    { "slay",		do_slay,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "sset",		do_sset,	POS_DEAD,    L_SEN,  LOG_ALWAYS },
    { "sstime",		do_sstime,	POS_DEAD,    L_SEN,  LOG_ALWAYS },
    { "transfer",	do_transfer,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "users",		do_users,	POS_DEAD,    L_SEN,  LOG_NORMAL	},
    { "wizify", 	do_wizify,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},
    { "wizlock",	do_wizlock,	POS_DEAD,    L_SEN,  LOG_ALWAYS	},

    { "echo",		do_echo,	POS_DEAD,    L_JUN,  LOG_ALWAYS	},
    { "memory",		do_memory,	POS_DEAD,    L_JUN,  LOG_NORMAL	},
    { "mload",		do_mload,	POS_DEAD,    L_JUN,  LOG_ALWAYS	},
    { "mfind",		do_mfind,	POS_DEAD,    L_JUN,  LOG_NORMAL },
    { "mstat",		do_mstat,	POS_DEAD,    L_JUN,  LOG_NORMAL	},
    { "mwhere",		do_mwhere,	POS_DEAD,    L_JUN,  LOG_NORMAL },
    { "newlock",	do_newlock,	POS_DEAD,    L_JUN,  LOG_ALWAYS	},
    { "ofind",		do_ofind,	POS_DEAD,    L_JUN,  LOG_NORMAL },
    { "ostat",		do_ostat,	POS_DEAD,    L_JUN,  LOG_NORMAL	},
    { "owhere",		do_owhere,	POS_DEAD,    L_JUN,  LOG_NORMAL	},
    { "peace",		do_peace,	POS_DEAD,    L_JUN,  LOG_NORMAL	},
    { "recho",		do_recho,	POS_DEAD,    L_JUN,  LOG_ALWAYS	},
    { "return",		do_return,	POS_DEAD,    L_JUN,  LOG_NORMAL	},
    { "rset",		do_rset,	POS_DEAD,    L_JUN,  LOG_ALWAYS	},
    { "rstat",		do_rstat,	POS_DEAD,    L_JUN,  LOG_NORMAL	},
    { "slookup",	do_slookup,	POS_DEAD,    L_JUN,  LOG_NORMAL },
    { "snoop",		do_snoop,	POS_DEAD,    L_JUN,  LOG_NORMAL	},
    { "switch",		do_switch,	POS_DEAD,    L_JUN,  LOG_ALWAYS	},
    { "wizinvis",	do_invis,	POS_DEAD,    L_JUN,  LOG_NORMAL	},

    { "at",		do_at,		POS_DEAD,    L_APP,  LOG_NORMAL	},
    { "bamfin",		do_bamfin,	POS_DEAD,    L_APP,  LOG_NORMAL	},
    { "bamfout",	do_bamfout,	POS_DEAD,    L_APP,  LOG_NORMAL	},
    { "goto",		do_goto,	POS_DEAD,    L_APP,  LOG_NORMAL	},
    { "holylight",	do_holylight,	POS_DEAD,    L_APP,  LOG_NORMAL	},

    { ":",		do_immtalk,	POS_DEAD,    L_HER,  LOG_NORMAL	},

    /*
     * End of list.
     */
    { "",		0,		POS_DEAD,	 0,  LOG_NORMAL	}
};



/*
 * The social table.
 * Add new socials here.
 * Alphabetical order is not required.
 */

/* New socials contributed by Katrina and Binky */
const	struct	social_type	social_table [ ] =
{
    {
	"accuse",
	"Accuse whom?",
	"$n is in an accusing mood.",
	"You look accusingly at $M.",
	"$n looks accusingly at $N.",
	"$n looks accusingly at you.",
	"You accuse yourself.",
	"$n seems to have a bad conscience."
    },

    {
	"ack",
	"You gasp and say 'ACK!' at your mistake.",
	"$n ACKS at $s big mistake.",
	"You ACK $M.",
	"$n ACKS $N.",
	"$n ACKS you.",
	"You ACK yourself.",
	"$n ACKS $mself.  Must be a bad day."
    },
	
    {
	"addict",
	"You stand and admit to all in the room, 'Hi, I'm $n, and I'm a mud addict.'",
	"$n stands and says, 'Hi, I'm $n, and I'm a mud addict.'",
	"You tell $M that you are addicted to $S love.",
	"$n tells $N that $e is addicted to $S love.",
	"$n tells you that $e is addicted to your love.",
	"You stand and admit to all in the room, 'Hi, I'm $n, and I'm a mud addict.'",
	"$n stands and says, 'Hi, I'm $n, and I'm a mud addict.'",
    },
   
    {
	"agree",
	"You seem to be in an agreeable mood.",
	"$n seems to agree.",
	"You agree with $M.",
	"$n agrees with $N.",
	"$n agrees with you.",
	"Well I hope you would agree with yourself!",
	"$n agrees with $mself, of course."
    },
    
    {
	"airguitar",
	"You sizzle the air with your BITCHIN' guitar playing!",
	"$n air-guitars like Jimi Hendrix!",
	"You sizzle the air with your BITCHIN' guitar playing!",
	"$n air-guitars like Jimi Hendrix!",
	"$n air-guitars like Jimi Hendrix, just for you!",
	"You forget all else as you sizzle the air with your BITCHIN' guitar playing!",
	"$n forgets all else as $e air-guitars like Jimi Hendrix."
    },

    {
	"apologize",
	"You apologize for your behavior.",
	"$n apologizes for $s rude behavior.",
	"You apologize to $M.",
	"$n apologizes to $N.",
	"$n apologizes to you.",
	"You apologize to yourself.",
	"$n apologizes to $mself.  Hmmmm."
    },

    {
	"applaud",
	"Clap, clap, clap.",
	"$n gives a round of applause.",
	"You clap at $S actions.",
	"$n claps at $N's actions.",
	"$n gives you a round of applause.  You MUST'VE done something good!",
	"You applaud at yourself.  Boy, are we conceited!",
	"$n applauds at $mself.  Boy, are we conceited!"
    },

    {
	"banzai",
	"You scream 'BANZAI!!!!' and brandish your sword for battle.",
	"$n screams 'BANZAI!!!!' and charges into the fray.",
	"You scream 'BANZAI!!!!' and drag $M into the fray with you.",
	"$n screams 'BANZAI!!!!' and drags $N into the fray with $m!",
	"$n screams 'BANZAI!!!!, and drags you into the fray with $m!",
	"You scream 'BANZAI!!!!' at yourself, and brandish your weapon for battle.",
	"$n screams 'BANZAI!!!!' at $mself, and charges into the fray."
    },
    
    {
	"bark",
	"Woof!  Woof!",
	"$n barks like a dog.",
	"You bark at $M.",
	"$n barks at $N.",
	"$n barks at you.",
	"You bark at yourself.  Woof!  Woof!",
	"$n barks at $mself.  Woof!  Woof!"
    },

    {
	"bearhug",
	"You hug a grizzly bear.",
	"$n hugs a flea-infested grizzly bear.",
	"You bearhug $M.",
	"$n bearhugs $N.  Some ribs break.",
	"$n bearhugs you.  Wonder what's coming next?",
	"You bearhug yourself.",
	"$n bearhugs $mself."
    },

    {
        "beef",
        "You loudly exclaim 'WHERE'S THE BEEF??????'",
        "$n loudly exclaims 'WHERE'S THE BEEF??????'",
        "You poke $N and exclaim, 'WHERE'S THE BEEF?????'",
        "$n pokes $N, and exclaims, 'WHERE'S THE BEEF????'",
        "$n pokes you and exclaims, 'WHERE'S THE BEEF????'",
        "You poke your fat rolls and exclaim, 'Oh, THERE'S THE BEEF!'",
        "$n pokes his fat rolls, and exclaims, 'Oh, THERE'S THE BEEF!'"
    },
   
    {
	"beer",
	"You down a cold, frosty beer.",
	"$n downs a cold, frosty beer.",
	"You draw a cold, frosty beer for $N.",
	"$n draws a cold, frosty beer for $N.",
	"$n draws a cold, frosty beer for you.",
	"You draw yourself a beer.",
	"$n draws $mself a beer."
    },

    {
	"beg",
	"You beg the gods for mercy.",
	"The gods fall down laughing at $n's request for mercy.",
	"You desperately try to squeeze a few coins from $M.",
	"$n begs $N for a gold piece!",
	"$n begs you for money.",
	"Begging yourself for money doesn't help.",
	"$n begs $mself for money."
    },

    {
	"bkiss",
	"Blow a kiss to whom?",
	"$n blows at $s hand.",
	"You blow a kiss to $M.",
	"$n blows a kiss to $N.  Touching, ain't it?",
	"$n blows a kiss to you.  Not as good as a real one, huh?",
	"You blow a kiss to yourself.",
	"$n blows a kiss to $mself.  Weird."
    },
  
    {
	"bleed",
	"You bleed all over the room!",
	"$n bleeds all over the room!  Get out of $s way!",
	"You bleed all over $M!",
	"$n bleeds all over $N.  Better leave, you may be next!",
	"$n bleeds all over you!  YUCK!",
	"You bleed all over yourself!",
	"$n bleeds all over $mself."
    },

    {
      "blindfold",
      "You pull out a blindfold and grin evilly.",
      "$n pulls out a blindfold and grins evilly.",
      "You a tie a blindfold securely about $S head.",
      "$n ties a blindfold securely about $N's head. Must be dark in there.",
      "$n ties a blindfold securely about your head. You can't see a thing.",
      "You blindfold yourself, not wanting to see what happens next.",
      "$n blindfolds $mself, not wanting to see what happens next."
    },

    {
	"blink",
	"You blink in utter disbelief.",
	"$n blinks in utter disbelief.",
	"You blink at $M in confusion.",
	"$n blinks at $N in confusion.",
	"$n blinks at you in confusion.",
	"You are sooooooooooooo confused",
	"$n blinks at $mself in complete confusion."
    },

    {
	"blush",
	"Your cheeks are burning.",
	"$n blushes.",
	"You get all flustered up seeing $M.",
	"$n blushes as $e sees $N here.",
	"$n blushes as $e sees you here.  Such an effect on people!",
	"You blush at your own folly.",
	"$n blushes as $e notices $s boo-boo."
    },

    {
	"boggle",
	"You boggle at all the loonies around you.",
	"$n boggles at all the loonies around $m.",
	"You boggle at $S ludicrous idea.",
	"$n boggles at $N's ludicrous idea.",
	"$n boggles at your ludicrous idea.",
	"BOGGLE.",
	"$n wonders what BOGGLE means."
    },

    {
	"bonk",
	"BONK.",
	"$n spells 'potato' like Dan Quayle: 'B-O-N-K'.",
	"You bonk $M for being a numbskull.",
	"$n bonks $N.  What a numbskull.",
	"$n bonks you.  BONK BONK BONK!",
	"You bonk yourself.",
	"$n bonks $mself."
    },

    {
	"boogie",
	"You boogie down!",
	"$n gets down and boogies!!",
	"You grab $M and boogie down!",
	"$n grabs $N and they boogie down!",
	"$n grabs you and boogies with you!",
	"You boogie with yourself.  What a great dancer you are!",
	"$n boogies with $mself.  Guess no one will dance with $m."
    },
    
    {
	"bottle",
	"You open up a cold bottle of brew.",
	"$n opens a cold bottle of brew.",
	"You open up a cold bottle of brew for $M.",
	"$n opens a cold bottle of brew for $N.",
	"$n opens a cold bottle of brew for you.",
	"You open a cold bottle of brew for yourself.",
	"$n opens a cold bottle of brew for $mself." 
    },
 
    {
	"bounce",
	"BOIINNNNNNGG!",
	"$n bounces around.",
	"You bounce onto $S lap.",
	"$n bounces onto $N's lap.",
	"$n bounces onto your lap.",
	"You bounce your head like a basketball.",
	"$n plays basketball with $s head."
    },

    {
	"bow",
	"You bow deeply.",
	"$n bows deeply.",
	"You bow before $M.",
	"$n bows before $N.",
	"$n bows before you.",
	"You kiss your toes.",
	"$n folds up like a jack knife and kisses $s own toes."
    },

    {
	"brb",
	"You announce that you will be right back.",
	"$n says in a stern voice, 'I'll be back!'",
	"You announce to $M that you will be right back.",
	"$n says to $N in a stern voice, 'I'll be back!'",
	"$n says to you in a stern voice, 'I'll be right back!'",
	"You mumble to yourself, 'I'll be right back'",
	"$n mumbles to $mself, 'I'll be right back, won't I?'"
    },
    
    {
	"brush",
	"Brush what? Who? Where?",
	NULL,
	"You brush out $S hair for $M.  Very thoughtful.",
	"$n brushes $N's hair for $M.  Looks better now.",
	"$n brushes out your hair.  How nice of $m.",
	"You brush out your hair.  There - much better.",
	"$n brushes out $s hair.  Looks much better now."
    },

    {
	"bully",
	"You growl and demand everyone's lunch money NOW!",
	"$n growls and demands everyone's lunch money.",  
	"You growl and demand $S lunch money NOW!",
	"$n growls and demands $N's lunch money.",
	"$n growls and demands your lunch money.",
	"You bully yourself, and take your own lunch money.",
	"$n bullies $mself, and takes $s lunch money away from $mself???"
    },
 
    {  
	"bungy",
	"You tie a bungy cord to the mud and jump into internet.",
	"$n ties a bungy cord to the mud and jumps into internet.",
	"You tie a bungy cord to $N and throw $M off the mud.",
	"$n ties a bungy cord to $N and throws $M off the mud.",
	"$n ties a bungy cord to you and throws you off the mud.",
	"You tie a bungy cord to yourself and jump off the mud.",
	"$n ties a bungy cord to $mself and jumps off the mud."
    },

    {
	"burp",
	"You burp loudly.",
	"$n burps loudly.",
	"You burp loudly to $M in response.",
	"$n burps loudly in response to $N's remark.",
	"$n burps loudly in response to your remark.",
	"You burp at yourself.",
	"$n burps at $mself.  What a sick sight."
    },

    {
	"bye",
	"You say goodbye to all in the room.",
	"$n says goodbye to everyone in the room.",
	"You say goodbye to $N.",
	"$n says goodbye to $N.",
	"$n says goodbye to you.",
	"You say goodbye to yourself.  Contemplating suicide?",
	"$n says goodbye to $mself.  Is $e contemplating suicide?"
    },
	
    {
	"cackle",
	"You throw back your head and cackle with insane glee!",
	"$n throws back $s head and cackles with insane glee!",
	"You cackle gleefully at $N",
	"$n cackles gleefully at $N.",
	"$n cackles gleefully at you.  Better keep your distance from $m.",
	"You cackle at yourself.  Now, THAT'S strange!",
	"$n is really crazy now!  $e cackles at $mself."
    },

    {
	"cannonball",
	"You cannonball into the pool.   *SPLOOSH*",
	"$n cannonballs into the pool.   *SPLOOSH*",
	"You cannonball into the pool and splash water over $N.",
	"$n cannonballs into the pool and splashes $N.",
	"$n cannonballs into the pool and sends water over you.  You are WET!",
	"You bounce cannonballs off your head to show your strength.",
	"$n bounces cannonballs off $s head.  Can you say 'nutcase'?"
    },

    {
	"caress",
	"Who would you like to caress?",
	"$n is looking for someone to caress with $s gentle touch.",
	"You run your hands across $S body, caressing $M.",
	"$n runs $s hands across $N's body, caressing $M.",
	"$n runs $s hands across your body, caressing you.",
	"You lightly caress yourself- wouldn't you rather do that in private?",
	"$n caresses $mself all over- and in public, no less!"
    },

    {
	"catnap",
	"You curl into a tiny ball and go to sleep.",
	"$n curls $mself into a tiny ball and goes to sleep.",
	"You curl up in $S lap and go to sleep.",
	"$n curls up in $N's lap and goes to sleep.",
	"$n curls up in your lap and goes to sleep.",
	"You curl into a tiny ball and go to sleep.",
	"$n curls $mself into a tiny ball and goes to sleep."
    },
    
    {
	"cheer",
	"ZIS BOOM BAH!  BUGS BUNNY BUGS BUNNY RAH RAH RAH!",
	"$n says 'BUGS BUNNY BUGS BUNNY RAH RAH RAH!'",
	"You cheer loudly: 'Go $N Go!'",
	"$n cheers loudly: 'Go $N Go!'",
	"$n cheers you on!",
	"You cheer yourself up.",
	"$n cheers $mself on."
    },

    {
	"chekov",
	"You speak your best Russian in hopes of finding a nuclear wessel.",
	"$n asks sheepishly, 'Ver are the nuclear wessels?'",
	"You ask $M about the nuclear wessels.",
	"$n asks $N sheepishly, 'Ver are the nuclear wessels?'",
	"$n asks you sheepishly, 'Ver are the nuclear wessels?'",
	"You look around, muttering, 'Ver are the nuclear wessels?'",
	"$n looks around, muttering, 'Ver are the nuclear wessels?'"
    },
    
    {
      "cherish",
      "You cherish the moment.",
      "$n cherishes the moment.",
      "You cherish $M with all your heart.",
      "$n cherishes $N with all $s heart.",
      "$n cherishes you with all $s heart.",
      "You cherish yourself. Getting a bit narcissistic, are you?",
      "$n cherishes $mself narcissistically."
    },

    {
	"chuckle",
	"You chuckle politely.",
	"$n chuckles politely.",
	"You chuckle at $S joke.",
	"$n chuckles at $N's joke.",
	"$n chuckles at your joke.",
	"You chuckle at your own joke, since no one else would.",
	"$n chuckles at $s own joke, since none of you would."
    },

    {
	"clap",
	"You clap your hands together.",
	"$n shows $s approval by clapping $s hands together.",
	"You clap at $S performance.",
	"$n claps at $N's performance.",
	"$n claps at your performance.",
	"You clap at your own performance.",
	"$n claps at $s own performance."
    },

    { 
        "clue",
        "You mumble 'DUM-DUM-DUM-DUM ... ah, a clue!'",
        "$n mumbles 'DUM-DUM-DUM-DUM ... ah, a clue!'",
        "You mumble 'DUM-DUM-DUM-DUM ... ah, a clue!'",
        "$n mumbles 'DUM-DUM-DUM-DUM ... ah, a clue!'",
        "$n mumbles 'DUM-DUM-DUM-DUM ... $N, aha, a clue!'",
        "You mumble to yourself, 'DUM-DUM-DUM-DUM ... ah, a clue!'",
        "$n mumbles to $mself, 'DUM-DUM-DUM-DUM ... ah, a clue!'"
    },
   
    {
	"clueless",
	"You chastise everyone in the room for being clueless.",
	"$n chastises everyone for being clueless.",
	"You chastise $M for being clueless.",
	"$n chastises $N for being clueless.",
	"$n chastises you for being clueless.",
	"You chastise yourself for being clueless.  What a dweeb you are!",
	"$n chastises $mself for being clueless.  What a dweeb!"
    },

    {
      "coffee",
      "You sip from a hot cup of coffee.",
      "$n sips from a hot cup of coffee.",
      "You offer $M a hot cup of strong coffee.",
      "$n offers $N a hot cup of strong coffee.",
      "$n offers you a hot cup of strong coffee.",
      "You quaff a hot cup of strong coffee. You are WIRED.",
      "$n quaffs a hot cup of strong coffee, getting WIRED."
    },

    {
	"comfort",
	"Do you feel uncomfortable?",
	NULL,
	"You comfort $M.",
	"$n comforts $N.",
	"$n comforts you.",
	"You make a vain attempt to comfort yourself.",
	"$n has no one to comfort $m but $mself."
    },

    {
	"cough",
	"You cough to clear your throat and eyes and nose and....",
	"$n coughs loudly.",
	"You cough loudly.  It must be $S fault, $E gave you this cold.",
	"$n coughs loudly, and glares at $N, like it is $S fault.",
	"$n coughs loudly, and glares at you.  Did you give $M that cold?",
	"You cough loudly.  Why don't you take better care of yourself?",
	"$n coughs loudly.  $n should take better care of $mself." 
    },

    { 
	"cower",
	"What are you afraid of?",
	"$n cowers in the corner from claustrophobia.",
	"You cower in the corner at the sight of $M.",
	"$n cowers in the corner at the sight of $N.",
	"$n cowers in the corner at the sight of you.",
	"You cower in the corner at the thought of yourself.  You scaredy cat!",
	"$n cowers in the corner.  What is wrong with $m now?"  
    },
    
    {
	"cringe",
	"You cringe in terror.",
	"$n cringes in terror!",
	"You cringe away from $M.",
	"$n cringes away from $N in mortal terror.",
	"$n cringes away from you.",
	"I beg your pardon?",
	NULL
    },

    {
	"crush",
	"You squint and hold two fingers up, saying 'I'm crushing your heads!'",
	"$n squints and holds two fingers up, saying 'I'm crushing your heads!'",
	"You hold two fingers up at $M and say, 'I'm crushing your head!'",
	"$n holds two fingers up at $N and says, 'I'm crushing your head!'",
	"$n holds two fingers up at you and says, 'I'm crushing your head!'",
	"You crush yourself.  YEEEEOOOUUUUCH!",
	"$n crushes $mself into the ground.  OUCH!"
    },

    {
	"cry",
	"Waaaaah ...",
	"$n bursts into tears.",
	"You cry on $S shoulder.",
	"$n cries on $N's shoulder.",
	"$n cries on your shoulder.",
	"You cry to yourself.",
	"$n sobs quietly to $mself."
    },

    {
	"cuddle",
	"Whom do you feel like cuddling today?",
	NULL,
	"You cuddle $M.",
	"$n cuddles $N.",
	"$n cuddles you.",
	"You must feel very cuddly indeed ... :)",
	"$n cuddles up to $s shadow.  What a sorry sight."
    },

    {
	"curse",
	"You swear loudly for a long time.",
	"$n swears: @*&^%@*&!",
	"You swear at $M.",
	"$n swears at $N.",
	"$n swears at you!  Where are $s manners?",
	"You swear at your own mistakes.",
	"$n starts swearing at $mself.  Why don't you help?"
    },

    {
	"curtsey",
	"You curtsey to your audience.",
	"$n curtseys gracefully.",
	"You curtsey to $M.",
	"$n curtseys gracefully to $N.",
	"$n curtseys gracefully for you.",
	"You curtsey to your audience (yourself).",
	"$n curtseys to $mself, since no one is paying attention to $m."
    },

    {
	"dance",
	"Feels silly, doesn't it?",
	"$n tries to break dance, but nearly breaks $s neck!",
	"You sweep $M into a romantic waltz.",
	"$n sweeps $N into a romantic waltz.",
	"$n sweeps you into a romantic waltz.",
	"You skip and dance around by yourself.",
	"$n dances a pas-de-une."
    },

    {
	"dive",
	"You dive into the ocean.",
	"$n dives into the ocean.",
	"You dive behind $M and hide.",
	"$n dives behind $N and hides.",
	"$n dives behind you and hides.",
	"You take a dive.",
	"$n takes a dive."
    },
    
    {
        "dizzy",
        "You are so dizzy from all this chatter.",
        "$n spins twice and hits the ground, dizzy from all this chatter.",
        "You are dizzy from all of $N's chatter.",
        "$n spins twice and hits the ground, dizzy from all $N's chatter.",
        "$n spins twice and hits the ground, dizzy from all your chatter.",
        "You are dizzy from lack of air.  Don't talk so much!",
        "$n spins twice and falls to the ground from lack of air."
    },

    {
	"doc",
	"You nibble on a carrot and say 'Eh, what's up Doc?'",
	"$n nibbles on a carrot and says 'Eh, what's up Doc?'",
	"You nibble on a carrot and say to $M, 'Eh, what's up Doc?'",
	"$n nibbles on a carrot and says to $N, 'Eh, what's up Doc?'",
	"$n nibbles on a carrot and says to you, 'Eh, what's up Doc?'",
	"You nibble on a carrot and say to yourself, 'Eh, what's up $n?'",
	"$n nibbles on a carrot and says 'Eh, what's up Doc?'" 
    },
    
    {
	"doh",
	"You say, 'Doh!!' and hit your forehead.  What an idiot you are!",
	"$n hits $mself in the forehead and says, 'Doh!!!'",
	"You say, 'Doh!!' and hit your forehead.  What an idiot you are!",
	"$n hits $mself in the forehead and says, 'Doh!!!'",
	"$n hits $mself in the forehead and says, 'Doh!!!'",
	"You hit yourself in the forehead and say, 'Doh!!!'",
	"$n hits $mself in the forehead and says, 'Doh!!!'"    
    },
    
    /*
     * This one's for Baka, Penn, and Onethumb!
     */
    {
	"drool",
	"You drool on yourself.",
	"$n drools on $mself.",
	"You drool all over $N.",
	"$n drools all over $N.",
	"$n drools all over you.",
	"You drool on yourself.",
	"$n drools on $mself."
    },

    {
	"duck",
	"Whew!  That was close!",
	"$n is narrowly missed by a low-flying dragon.",
	"You duck behind $M.  Whew!  That was close!",
	"$n ducks behind $N to avoid the fray.",
	"$n ducks behind you to avoid the fray.",
	"You duck behind yourself.  Oww that hurts!",
	"$n tries to duck behind $mself.  $n needs help getting untied now."
    },
   
    {   
	"embrace",
	"Who do you want to hold?",
	"$n looks around for someone to hold close to $m.",
	"You hold $M in a warm and loving embrace.",
	"$n holds $N in a warm and loving embrace.",
	"$n holds you in a warm and loving embrace.",
	"You hold yourself in a warm and loving embrace.  Feels silly doesn't it?",
	"$n holds $mself in a warm and loving embrace.  $e looks pretty silly."
    },

    {
	"eskimo",
	"Who do you want to eskimo kiss with?",
	"$n is looking for someone to rub noses with.  Any volunteers?",
	"You rub noses with $M.",
	"$n rubs noses with $N.  You didn't know they were eskimos!",
	"$n rubs noses with you.",
	"You ponder the difficulties involved in rubbing noses with yourself.",
	"$n ponders the difficulties involved in rubbing noses with $mself."
    },
    
    {
	"eyebrow",
	"You raise an eyebrow.",
	"$n raises an eyebrow.",
	"You raise an eyebrow at $M.",
	"$n raises an eyebrow at $N.",
	"$n raises an eyebrow at you.",
	"You raise an eyebrow at yourself.  That hurt!",
	"$n raises an eyebrow at $mself.  That must have hurt!"
    },
 
    {
	"faint",
	"You feel dizzy and hit the ground like a board.",
	"$n's eyes roll back in $s head and $e crumples to the ground.",
	"You faint into $S arms.",
	"$n faints into $N's arms.",
	"$n faints into your arms.  How romantic.",
	"You look down at your condition and faint.",
	"$n looks down at $s condition and faints dead away."
    },
   
    {  
	"fakerep",
	"You report: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
	"$n reports: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
	"You report: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
	"$n reports: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
	"$n reports: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
	"You report: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp.",
	"$n reports: 12874/13103 hp 9238/10230 mana 2483/3451 mv 2.31E13 xp."
    },

    {
	"fart",
	"Where are your manners?",
	"$n lets off a real rip-roarer ... a greenish cloud envelops $n!",
	"You fart at $M.  Boy, you are sick.",
	"$n farts in $N's direction.  Better flee before $e turns to you!",
	"$n farts in your direction.  You gasp for air.",
	"You fart at yourself.  You deserve it.",
	"$n farts at $mself.  Better $m than you."
    },

    {
	"flash",
	"You flash your naked body at the gawking crowd.",
	"$n flashes $s naked body at everyone.  Gasp!",
	"You flash your naked body at $M.",
	"$n flashes $s naked body at $N.  Aren't you jealous?",
	"$n flashes $s naked body at you.  Everyone else is jealous.",
	"You flash your naked body at yourself.  How strange.",
	"$n is looking down $s shirt and grinning.  Very wierd!"
    },

    {
	"flex",
	"You flex your bulging muscles at the beach crowd.",
	"$n flexes $s muscles and puts yours to shame!",
	"You flex your bulging muscles at $M.",
	"$n flexes $s muscles at $N.",
	"$n flexes $s muscles at you.  $n must be trying to impress you.",
	"You flex your muscles at yourself.  Wow, you are HUGE!",
	"$n flexes $s muscles at $mself.  How narcissistic!"
    },
    
    {
	"flip",
	"You flip head over heels.",
	"$n flips head over heels.",
	"You flip $M over your shoulder.",
	"$n flips $N over $s shoulder.",
	"$n flips you over $s shoulder.  Hmmmm.",
	"You tumble all over the room.",
	"$n does some nice tumbling and gymnastics."
    },

    {
	"flirt",
	"Wink wink!",
	"$n flirts -- probably needs a date, huh?",
	"You flirt with $M.",
	"$n flirts with $N.",
	"$n wants you to show some interest and is flirting with you.",
	"You flirt with yourself.",
	"$n flirts with $mself.  Hoo boy."
    },
    
    {
	"flutter",
	"You flutter your eyelashes.",
	"$n flutters $s eyelashes.",
	"You flutter your eyelashes at $M.",
	"$n flutters $s eyelashes in $N's direction.",
	"$n looks at you and flutters $s eyelashes.",
	"You flutter your eyelashes at the thought of yourself.",
	"$n flutters $s eyelashes at no one in particular." 
    },
 
    {
	"fondle",
	"Who needs to be fondled?",
	NULL,
	"You fondly fondle $M.",
	"$n fondly fondles $N.",
	"$n fondly fondles you.",
	"You fondly fondle yourself, feels funny doesn't it ?",
	"$n fondly fondles $mself - this is going too far !!"
    },

    {
	"french",
	"Kiss whom?",
	NULL,
	"You give $N a long and passionate kiss.",
	"$n kisses $N passionately.",
	"$n gives you a long and passionate kiss.",
	"You gather yourself in your arms and try to kiss yourself.",
	"$n makes an attempt at kissing $mself."
    },

    {
	"frown",
	"What's bothering you ?",
	"$n frowns.",
	"You frown at what $E did.",
	"$n frowns at what $N did.",
	"$n frowns at what you did.",
	"You frown at yourself.  Poor baby.",
	"$n frowns at $mself.  Poor baby."
    },

    {
	"fume",
	"You grit your teeth and fume with rage.",
	"$n grits $s teeth and fumes with rage.",
	"You stare at $M, fuming.",
	"$n stares at $N, fuming with rage.",
	"$n stares at you, fuming with rage!",
	"That's right - hate yourself!",
	"$n clenches $s fists and stomps $s feet, fuming with anger."
    },

    {
      "gag",
      "You gag in disgust.",
      "$n gags in disgust.",
      "You stuff a sock in $s mouth, gagging $M. Ahhhhhhh...sweet silence!",
      "$n stuffs a sock in $N's mouth, gagging $M. Ahhhhhhh...sweet silence!",
      "$n gags your mouth with a sock. Everyone enjoys a moment of silence.",
      "You gag yourself with a sock before you say something you might regret.",
      "$n gags $mself with a sock before $e says something $e might regret."
    },

    {
        "garth",
        "You will give your weapons away....NOT.",
	"$n yells 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'",
	"You yell 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'",
	"$n yells 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'",
	"$n yells 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'",
	"You yell 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'",
	"$n yells 'WAYNE'S WORLD WAYNE'S WORLD -- PARTY TIME!  EXCELLENT!'"
    },
    
    {
	"gasp",
	"You gasp in astonishment.",
	"$n gasps in astonishment.",
	"You gasp as you realize what $E did.",
	"$n gasps as $e realizes what $N did.",
	"$n gasps as $e realizes what you did.",
	"You look at yourself and gasp!",
	"$n takes one look at $mself and gasps in astonishment!"
    },

    {
	"gawk",
	"You gawk at evryone around you.",
	"$n gawks at everyone in the room.",
	"You gawk at $M.",
	"$n gawks at $N.",
	"$n gawks at you.",
	"You gawk as you think what you must look like to others.",
	"$n is gawking again.  What is on $s mind?" 
    },
 
    {
	"german",
	"You speak your best German in hopes of getting a beer.",
	"$n says 'Du bist das hundchen!'  What the hell?",
	"You speak your best German to $M in hopes of getting a beer.",
	"$n says to $N, 'Du bist das hundchen!'  What the hell?",
	"$n says to you, 'Du bist das hundchen!'  What the hell?",
	"You speak your best German in hopes of getting a beer.",
	"$n says 'Du bist das hundchen!'  What the hell?"
    },
    
    {
	"ghug",
	"GROUP HUG!  GROUP HUG!",
	"$n hugs you all in a big group hug.  How sweet!",
	"GROUP HUG!  GROUP HUG!",
	"$n hugs you all in a big group hug.  How sweet!",
	"$n hugs you all in a big group hug.  How sweet!",
	"GROUP HUG!  GROUP HUG!",
	"$n hugs you all in a big group hug.  How sweet!"
    },
    
    {
	"giggle",
	"You giggle.",
	"$n giggles.",
	"You giggle in $S presence.",
	"$n giggles at $N's actions.",
	"$n giggles at you.  Hope it's not contagious!",
	"You giggle at yourself.  You must be nervous or something.",
	"$n giggles at $mself.  $e must be nervous or something."
    },

    {
	"glare",
	"You glare at nothing in particular.",
	"$n glares around $m.",
	"You glare icily at $M.",
	"$n glares at $N.",
	"$n glares icily at you, you feel cold to your bones.",
	"You glare icily at your feet, they are suddenly very cold.",
	"$n glares at $s feet, what is bothering $m?"
    },

    {
	"goose",
	"You honk like a goose.",
	"$n honks like a goose.",
	"You goose $S luscious bottom.",
	"$n gooses $N's soft behind.",
	"$n squeezes your tush.  Oh my!",
	"You goose yourself.",
	"$n gooses $mself.  Yuck."
    },
    
    {
	"grimace",
	"You contort your face in disgust.",
	"$n grimaces in disgust.",
	"You grimace in disgust at $M.",
	"$n grimaces in disgust at $N.",
	"$n grimaces in disgust at you.",
	"You grimace at yourself in disgust.",
	"$n grimaces at $mself in disgust."
    },

    {
	"grin",
	"You grin evilly.",
	"$n grins evilly.",
	"You grin evilly at $M.",
	"$n grins evilly at $N.",
	"$n grins evilly at you.  Hmmm.  Better keep your distance.",
	"You grin at yourself.  You must be getting very bad thoughts.",
	"$n grins at $mself.  You must wonder what's in $s mind."
    },

    {
	"groan",
	"You groan loudly.",
	"$n groans loudly.",
	"You groan at the sight of $M.",
	"$n groans at the sight of $N.",
	"$n groans at the sight of you.",
	"You groan as you realize what you have done.",
	"$n groans as $e realizes what $e has done."
    },

    {
	"grovel",
	"You grovel in the dirt.",
	"$n grovels in the dirt.",
	"You grovel before $M.",
	"$n grovels in the dirt before $N.",
	"$n grovels in the dirt before you.",
	"That seems a little silly to me.",
	NULL
    },

    {
	"growl",
	"Grrrrrrrrrr ...",
	"$n growls.",
	"Grrrrrrrrrr ... take that, $N!",
	"$n growls at $N.  Better leave the room before the fighting starts.",
	"$n growls at you.  Hey, two can play it that way!",
	"You growl at yourself.  Boy, do you feel bitter!",
	"$n growls at $mself.  This could get interesting..."
    },

    {
	"grumble",
	"You grumble.",
	"$n grumbles.",
	"You grumble to $M.",
	"$n grumbles to $N.",
	"$n grumbles to you.",
	"You grumble under your breath.",
	"$n grumbles under $s breath."
    },

    {
	"grunt",
	"GRNNNHTTTT.",
	"$n grunts like a pig.",
	"GRNNNHTTTT.",
	"$n grunts to $N.  What a pig!",
	"$n grunts to you.  What a pig!",
	"GRNNNHTTTT.",
	"$n grunts to $mself.  What a pig!"
    },

    {
      "halo",
      "An aura of heavenly light appears above your head.",
      "An aura of heavenly light appears above $n's head.",
      "You project an aura of heavenly light above $S head.",
      "$n projects an aura of heavenly light above $N's head.",
      "$n projects an aura of heavenly light above your head.",
      "An aura of heavenly light appears above your head.",
      "An aura of heavenly light appears above $n's head."
    },

    {
	"hand",
	"Kiss whose hand?",
	NULL,
	"You kiss $S hand.",
	"$n kisses $N's hand.  How continental!",
	"$n kisses your hand.  How continental!",
	"You kiss your own hand.",
	"$n kisses $s own hand."
    },

    {
      "handcuff",
      "You pull out a pair of handcuffs.",
      "$n pulls out a pair of handcuffs.",
      "You handcuff $M, putting $M completely at your mercy.",
      "$n handcuffs $N, pretty kinky, huh?",
      "$n handcuffs you. This could get interesting.",
      "You handcuff yourself. Too bad you can't seem to find the key.",
      "$n handcuffs $mself. Too bad $e can't seem to find the key."
    },

    {
	"hangover",
	"You pop a few aspirin and put on your sunglasses.  Ow, your head hurts!",
	"$n holds $s head and says 'Quit breathing so loud!'",
	"Won't $N be quiet?  Your head is gonna split in two if $E keeps talking!",
	"$n complains to $N 'Be quiet!  I have a hangover!",
	"$n complains to you 'Be quiet!  I have a hangover!",
	"You shoosh yourself.  Be quiet!  You have a hangover!",
	"$n shooshes $mself.  Wow, what a hangover that must be!"
    },

    {
	"happy",
	"You smile wide like Cindy Brady.",
	"$n is as happy as a maggot on a piece of rotten meat.",
	"You smile wide at $M like Cindy Brady.",
	"$n is as happy as a maggot on a piece of rotten meat.",
	"$n is as happy as a maggot on a piece of rotten meat.",
	"You smile wide like Cindy Brady.",
	"$n is as happy as a maggot on a piece of rotten meat."
    },

    {   
	"hello",
	"You say hello to everyone in the room.",
	"$n says hello to everyone in the room.",
	"You tell $M how truly glad you are to see $M.",
	"$n tells $N 'Hi!'",
	"$n tells you how truly glad $e is that you are here.",
	"You greet yourself enthusiastically.",
	"$n greets $mself enthusiastically.  How odd."
    },

    {
      "hero",
      "You heroically risk life and limb to save the day.",
      "$n heroically risks life and limb to save the day.",
      "You say to $M, '$N, YOU'RE MY HERO!'",
      "$n says to $N, '$N, YOU'RE MY HERO!'",
      "$n says to you, '$N, YOU'RE MY HERO!'",
      "You try to look heroic.",
      "$n does $s best impression of a hero."
    },

    {
	"highfive",
	"You jump in the air...oops, better get someone else to join you.",
	"$n jumps in the air by $mself.  Is $e a cheerleader, or just daft?",
	"You jump in the air and give $M a big highfive!",
	"$n jumps in the air and gives $N a big highfive!",
	"$n jumps in the air and gives you a big highfive!",
	"You jump in the air and congratulate yourself!",
	"$n jumps in the air and gives $mself a big highfive!  Wonder what $e did?"
    },

    {
      "holdon",
      "You pause for a moment, hesitant.",
      "$n pauses for a moment, somewhat hesitant.",
      "You hold $M closely, hugging $M.",
      "$n holds $N closely, hugging $M.",
      "$n holds you closely, hugging you.",
      "You have to hold yourself up you're so weary. Go to Sleep!",
      "$n has to hold $mself up $e's so tired. $e should go to bed and get some sleep."
    },

    {
	"hop",
	"You hop around like a little kid.",
	"$n hops around like a little kid.",
	"$n hops around like a little kid.",
	"$n hops around like a little kid.",
	"$n hops around like a little kid.",
	"$n hops around like a little kid.",
	"$n hops around like a little kid."
    },

    {
        "hologram",
        "You snap your fingers and create an illusion.",
        "$n leaves south.",
        "You snap your fingers and create an illusion.",
        "$n leaves south.",
        "$n waves at you and leaves south.",
        "You snap your fingers and create an illusion of yourself.",
        "Suddenly, there are 2 $n's standing here!"
    },
    
    {
	"howl",
	"AHOOOOOOOOOOOOooooooooooooo.",
	"$n howls over the smoking corpse.",
	"You howl at $M.",
	"$n howls at $N.  Someone's gonna die!",
	"$n howls at you.  You must be good looking.",
	"You howl at yourself.",
	"$n howls at $mself."
    },
    
    {
	"hmm",
	"You Hmmmm out loud.",
	"$n thinks, 'Hmmmm.'",
	"You gaze thoughtfully at $M and say 'Hmmm.'",
	"$n gazes thoughtfully at $N and says 'Hmmm.'",
	"$n gazes thoughtfully at you and says 'Hmmm.'",
	"You Hmmmm out loud.",
	"$n thinks, 'Hmmmm.'"
    },
 
    {
	"hug",
	"Hug whom?",
	NULL,
	"You hug $M.",
	"$n hugs $N.",
	"$n hugs you.",
	"You hug yourself.",
	"$n hugs $mself in a vain attempt to get friendship."
    },

    {
	"hum",
	"Hmm Hmm Hmm Hmmmmmmm.",
	"$n hums like a bee with a chest cold.",
	"You hum a little ditty for $M.  Hmm Hmm Hmm Hmmmmmm.",
	"$n hums a little ditty for $N.  Hmm Hmm Hmm Hmmmmmm.",
	"$n hums a little ditty for you.  Hmm Hmm Hmm Hmmmmmm.",
	"Hmm Hmm Hmmmmmmm.",
	"$n hums like a bee with a chest cold."
    }, 
 
    {
      "icecube",
      "You wonder who would like an ice cube down their shirt.",
      "With a devious look on $s face, $n hides something behind $s back.",
      "You stuff an ice cube down $S shirt. You laugh as $E hops around.",
      "$n stuffs an ice cube down $N's shirt, better hope that was $s last ice cube.",
      "$n stuffs an ice cube down your shirt. Brrr! $n laughs as you struggle to get it out.",
      "You stick an ice cube down your shirt. Brrr!",
      "$n sticks an icecube down $s shirt, trying to cool off."
    },

    {
	"invite",
	"You speak your best French in hopes of getting lucky.",
	"$n tells you, 'Voulez-vous couche avec moi ce soir?'",  
	"You speak your best French to $M in hopes of getting lucky.",
	"$n tells $N, 'Voulez-vous couche avec moi ce soir?'",
	"$n tells you, 'Voulez-vous couche avec moi ce soir?'",
	"You speak your best French in hopes of getting lucky, with yourself???",
	"$n says to $mself, 'Voulez-vous couche avec moi ce soir?'"
    },
  
    {
      "iwhistle",
      "You whistle innocently, trying to hide your guilt.",
      "$n whistles innocently.",
      "You whistle innocently, avoiding $S question.",
      "$n whistles innocently, a sure sign of guilt.",
      "$n whistles innocently at your question...hmmmmm.",
      "You whistle innocently.",
      "$n whistles innocently, trying to hide $s guilt."
    },

    {
        "jsave",
        "You profess 'Jesus saves!  But Gretsky recovers...he scores!'",
        "$n announces 'Jesus saves!  But Gretsky recovers...he scores!'",
        "You profess 'Jesus saves!  But Gretsky recovers...he scores!'",
        "$n announces 'Jesus saves!  But Gretsky recovers...he scores!'",
        "$n announces to you 'Jesus saves!  But Gretsky recovers...he scores!'",
        "You profess 'Jesus saves!  But Gretsky recovers...he scores!'",
        "$n announces 'Jesus saves!  But Gretsky recovers...he scores!'"
    },
    
    {
	"kiss",
	"Isn't there someone you want to kiss?",
	NULL,
	"You kiss $M.",
	"$n kisses $N.",
	"$n kisses you.",
	"All the lonely people :(",
	NULL
    },

    {
        "lag",
        "You complain about the lag, your hitpoints, your mana...",
        "$n has lost $s link because of the lag.",
        "You complain to $N about the lag, your hitpoints, your mana...",
        "$n has lost $s link because of the lag.",
        "$n has lost $s link because of the lag.",
        "You complain about the lag, your hitpoints, your mana...",
        "$n has lost $s link because of the lag."
    },
    
    {
	"laugh",
	"You laugh.",
	"$n laughs.",
	"You laugh at $N mercilessly.",
	"$n laughs at $N mercilessly.",
	"$n laughs at you mercilessly.  Hmmmmph.",
	"You laugh at yourself.  I would, too.",
	"$n laughs at $mself.  Let's all join in!!!"
    },

    {
	"lick",
	"You lick your lips and smile.",
	"$n licks $s lips and smiles.",
	"You lick $M.",
	"$n licks $N.",
	"$n licks you.",
	"You lick yourself.",
	"$n licks $mself - YUCK."
    },

    {
      "listen",
      "You listen intently.",
      "$n listens intently.",
      "You listen intently to what $E is saying.",
      "$n listens intently to what $N is saying.",
      "$n listens intently to what you are saying.",
      "You pay close attention to what you are saying, trying to avoid saying something you shouldn't.",
      "$n listens intently to $mself. $e must enjoy the sound of $s own voice."
    },

    {
	"love",
	"You love the whole world.",
	"$n loves everybody in the world.",
	"You tell your true feelings to $N.",
	"$n whispers softly to $N.",
	"$n whispers to you sweet words of love.",
	"Well, we already know you love yourself (lucky someone does!)",
	"$n loves $mself, can you believe it ?"
    },

    {
	"maim",
	"Who do you want to maim?",
	"$n is looking for someone to maim.",
	"You maim $M with your dull fingernails.",
	"$n raises $s hand and tries to maim $N to pieces.",
	"$n raises $s hand and paws at you.  You've been maimed!",
	"You maim yourself with your dull fingernails.",
	"$n raises $s hand and maims $mself to pieces."
    },

    {
	"marvelous",
	"You say 'Ah dahling, you look MAHVELLOUS!'",
	"$n says 'Ah dahling, you look MAHVELLOUS!'",
	"You say to $M, 'Ah dahling, you look MAHVELLOUS!'",
	"$n says to $N, 'Ah dahling, you look MAHVELLOUS!'",
	"$n says to you, 'Ah dahling, you look MAHVELLOUS!'",
	"You say 'Ah dahling, I look MAHVELLOUS!'",
	"$n says 'Ah dahling, I look MAHVELLOUS!'"
    },
    
    {
	"massage",
	"Massage what?  Thin air?",
	NULL,
	"You gently massage $N's shoulders.",
	"$n massages $N's shoulders.",
	"$n gently massages your shoulders.  Ahhhhhhhhhh ...",
	"You practice yoga as you try to massage yourself.",
	"$n gives a show on yoga positions, trying to massage $mself."
    },

    {
	"meow",
	"MEOW.",
	"$n meows.  What's $e going to do next, wash $mself with $s tongue?",
	"You meow at $M, hoping $E will give you some milk.",
	"$n meows at $N, hoping $E will give $m some milk. ",
	"$n meows at you.  Maybe $e wants some milk.",
	"You meow like a kitty cat.",
	"$n meows like a kitty cat."
    },
    
    {
	"mmm",
	"You go mmMMmmMMmmMMmm.",
	"$n says, 'mmMMmmMMmmMMmm.'",
	"You go mmMMmmMMmmMMmm.",
	"$n says, 'mmMMmmMMmmMMmm.'",
	"$n thinks of you and says, 'mmMMmmMMmmMMmm.'",
	"You think of yourself, and go mmMMmmMMmmMMmm.",
	"$n thinks of $mself and says, 'mmMMmmMMmmMMmm.'"
    },

    {
	"moan",
	"You start to moan.",
	"$n starts moaning.",
	"You moan for the loss of $M.",
	"$n moans for the loss of $N.",
	"$n moans at the sight of you.  Hmmmm.",
	"You moan at yourself.",
	"$n makes $mself moan."
    },

    {
        "mooch",
        "You beg for money, weapons, coins.",
        "$n says 'Spare change?'",
        "You beg $N for money, weapons, coins.",
        "$n begs you for favors of the insidious type...",
        "$n begs you for favors of the insidious type...",
        "You beg for money, weapons, coins.",
        "$n says 'Spare change?'"
    },
    
    {
	"moocow",
	"You make cow noises.  Mooooooooooooooooooo!",
	"$n Mooooooooooooooooooooooooos like a cow.",
	"You make cow noises at $M.  Mooooooooooooooooooo!",
	"$n Mooooooooooooooooooooooooos like a cow at $N.",
	"$n looks at you and Mooooooooooooooooooooooooos like a cow.",
	"You make cow noises.  Mooooooooooooooooooo!",
	"$n Mooooooooooooooooooooooooos like a cow."
    },
    
    {
	"moon",
	"Gee your butt is big.",
	"$n moons the entire room.",
	"You show your big butt to $M.",
	"$n shows $s big butt to $N.  Find a paddle, quick!",
	"$n shows $s big ugly butt to you.  How do you like it?",
	"You moon yourself.",
	"$n moons $mself.  Ugliest butt you ever saw."
    },
    
    {
	"mosh",
	"You MOSH insanely about the room.",
	"$n MOSHES insanely about the room.",
	"You MOSH $M into a pile of Jello pudding.  Ewww!",
	"$n MOSHES $N into Jello pudding.  Ah, the blood!",
	"$n MOSHES you into the ground.",
	"You MOSH yourself.  Ah the blood!",
	"$n MOSHES and MOSHES and MOSHES and MOSHES..."
    },
    
    {
	"muhaha",
	"You laugh diabolically.  MUHAHAHAHAHAHA!",
	"$n laughs diabolically.  MUHAHAHAHAHAHA!",
	"You laugh at $M diabolically.  MUHAHAHAHAHAHA!",
	"$n laughs at $N diabolically.  MUHAHAHAHAHAHA!",
	"$n laughs at you diabolically.  MUHAHAHAHAHAHA!",
	"Muhaha at yourself??  Wierd.",
	NULL
    },

    {
	"mwalk",
	"You grab your hat and moonwalk across the room.",
	"$n grabs $s black hat and sparkly glove and moonwalks across the room.",      
	"You grab your hat and moonwalk across the room.",
	"$n grabs $s black hat and sparkly glove and moonwalks across the room.", 
	"$n grabs $s black hat and sparkly glove and moonwalks across the room.", 
	"You grab your hat and moonwalk across the room.",
	"$n grabs $s black hat and sparkly glove and moonwalks across the room."
    },

    {
        "nail",
        "You nibble nervously on your nails.",
        "$n nibbles nervously on $s fingernails.",
        "You nibble nervously on your nails.",
        "$n nibbles nervously on $s fingernails.",
        "$n nibbles nervously on your fingernails.  Yuck!",
        "You nibble nervously on your nails.",
        "$n nibbles nervously on $s fingernails."
    },
    
    {
	"nasty",
	"You do the best imitation of the nasty you can.",
	"$n does the nasty solo? -- wow.",
	"You do the nasty with $M.",
	"$n does the nasty with $N.  Find a firehose quick!",
	"$n does the nasty with you.  How do you like it?",
	"You do the nasty with yourself.",
	"$n does the nasty with $mself.  Hoo boy."
    },
    
    {
	"ni",
	"You exclaim proudly, 'I am a knight that goes NI!'",
	"$n exclaims proudly, 'I am a knight that goes NI!'",
	"You exclaim proudly to $M, 'I am a knight that goes NI!'",
	"$n exclaims proudly to $N, 'I am a knight that goes NI!'",
	"$n proudly exclaims to you, 'I am a knight that goes NI!'",
	"You exclaim proudly, 'I am a knight that goes NI!'",
	"$n exclaims proudly, 'I am a knight that goes NI!'"
    },
    
    {
	"nibble",
	"Nibble on whom?",
	NULL,
	"You nibble on $N's ear.",
	"$n nibbles on $N's ear.",
	"$n nibbles on your ear.",
	"You nibble on your OWN ear.",
	"$n nibbles on $s OWN ear."
    },

    {
	"nod",
	"You nod affirmative.",
	"$n nods affirmative.",
	"You nod in recognition to $M.",
	"$n nods in recognition to $N.",
	"$n nods in recognition to you.  You DO know $m, right?",
	"You nod at yourself.  Are you getting senile?",
	"$n nods at $mself.  $e must be getting senile."
    },

    {
      "noogie",
      "You look around, wondering who would like to get a noogie.",
      "$n looks around, wondering who would like to get a noogie.",
      "You put $M in a headlock and give $M a severe noogie.",
      "$n puts $N in a headlock and gives $M a noogie.",
      "$n puts you in a headlock and gives you a severe noogie.",
      "You give yourself a noogie. What a goof!",
      "$n gives $mself a noogie. What a goof!"
    },

    {
	"nose",
	"Gee your nose is big.",
	"$n noses around in your business.",
	"You tweak $S nose.",
	"$n tweaks $N's nose.  Ow!",
	"$n tweaks your nose.  Ow!",
	"You wiggle your nose.",
	"$n wiggles $s nose."
    },

    {
	"nudge",
	"Nudge whom?",
	NULL,
	"You nudge $M.",
	"$n nudges $N.",
	"$n nudges you.",
	"You nudge yourself, for some strange reason.",
	"$n nudges $mself, to keep $mself awake."
    },

    {
	"nuke",
	"Who do you want to nuke?",
	"$n tries to put something in the microwave.",
	"You envelop $M in a mushroom cloud.",
	"$n detonates a nuclear warhead at $N.  Uh oh!",
	"$n envelops you in a mushroom cloud.",
	"You nuke yourself.",
	"$n puts $mself in the microwave."
    },

    {
	"nuzzle",
	"Nuzzle whom?",
	NULL,
	"You nuzzle $S neck softly.",
	"$n softly nuzzles $N's neck.",
	"$n softly nuzzles your neck.",
	"I'm sorry, friend, but that's impossible.",
	NULL
    },

    {
	"ogle",
	"Whom do you want to ogle?",
	NULL,
	"You ogle $M like $E was a piece of meat.",
	"$n ogles $N.  Maybe you should leave them alone for awhile?",
	"$n ogles you.  Guess what $e is thinking about?",
	"You ogle yourself.  You may just be too wierd for this mud.",
	"$n ogles $mself.  Better hope that $e stops there."
    },

    {
	"ohno",
	"Oh no!  You did it again!",
	"Oh no!  $n did it again!",
	"You exclaim to $M, 'Oh no!  I did it again!'",
	"$n exclaims to $N, 'Oh no!  I did it again!'",
	"$n exclaims to you, 'Oh no!  I did it again!'",
	"You exclaim to yourself, 'Oh no!  I did it again!'",
	"$n exclaims to $mself, 'Oh no!  I did it again!'"
    },
    
    {
	"oink",
	"You make pig noises.  OINK!",
	"$n oinks 'OINK OINK OINK!'",
	"You make pig noises at $M.  OINK!",
	"$n oinks at $N: 'OINK OINK OINK!'",
	"$n oinks at you: 'OINK OINK OINK!'",
	"You make pig noises.  OINK!",
	"$n oinks 'OINK OINK OINK!'"
    },
 
    {
        "ooo",
        "You go ooOOooOOooOOoo.",
        "$n says, 'ooOOooOOooOOoo.'",
        "You go ooOOooOOooOOoo.",
        "$n says, 'ooOOooOOooOOoo.'",
        "$n thinks of you and says, 'ooOOooOOooOOoo.'",
        "You go ooOOooOOooOOoo.",
        "$n says, 'ooOOooOOooOOoo.'"
    },
 
    {
	"pat",
	"Pat whom?",
	NULL,
	"You pat $N on $S head.",
	"$n pats $N on $S head.",
	"$n pats you on your head.",
	"You pat yourself on your head.  Aren't you proud?",
	"$n pats $mself on the head.  $e is proud of $mself!"
    },

    {
	"peck",
	"You peck for seeds on the ground.",
	"$n pecks for seeds on the ground.",
	"You give $M a little peck on the cheek.",
	"$n gives $N a small peck on the cheek.",
	"$n gives you a sweet peck on the cheek.",
	"You kiss your own pectoral muscles.",
	"$n pecks $mself on $s pectoral muscles."
    },

    {
	"peer",
	"You peer intently about your surroundings.",
	"$n peers intently about the area, looking for thieves no doubt.",
	"You peer at $M quizzically.",
	"$n peers at $N quizzically.",
	"$n peers at you quizzically.",
	"You peer intently about your surroundings.",
	"$n peers intently about the area, looking for thieves no doubt."
    },

    {
	"phaser",
	"You aim your phasers at everyone in the room.",
	"$n aims a phaser at everyone in the room.  LOOK OUT!",
	"You set your phaser on OWCH and point it at $M.",
	"$n sets $s phaser on OWCH and points it at $N.  Uh oh...",
	"$n sets $s phaser on OWCH and points it at you.  Uh oh...",
	"You aim your phasers at yourself.  Duck!",
	"$n aims $s phasers at $mself.  Maybe $e's depressed?"
    },

    {
	"pinch",
	"You toss a pinch of salt over your shoulder.",
	"$n tosses a pinch of salt over $s shoulder.",
	"You pinch $S rosy cheeks.",
	"$n pinches $N's rosy cheeks.",
	"$n pinches your chubby cheeks.",
	"You need a pinch of salt.",
	"$n needs a pinch of salt."
    },

    {
        "ping",
        "Ping who?",
        NULL,
        "You ping $N.  $N is unreachable.",
        "$n pings $N.  $N is unreachable.",
        "$n pings you.  Is your server dead or alive?",
        "You ping yourself. Ooof!",
        "$n pings $mself. Ooof!"
    },

    {
	"point",
	"Point at whom?",
	NULL,
	"You point at $M accusingly.",
	"$n points at $N accusingly.",
	"$n points at you accusingly.",
	"You point proudly at yourself.",
	"$n points proudly at $mself."
    },

    {
	"poke",
	"Poke whom?",
	NULL,
	"You poke $M in the ribs.",
	"$n pokes $N in the ribs.",
	"$n pokes you in the ribs.",
	"You poke yourself in the ribs, feeling very silly.",
	"$n pokes $mself in the ribs, looking very sheepish."
    },

    {
	"ponder",
	"You ponder the question.",
	"$n sits down and thinks deeply.",
	"$n sits down and thinks deeply.",
	"$n sits down and thinks deeply.",
	"$n sits down and thinks deeply.",
	"$n sits down and thinks deeply.",
	"$n sits down and thinks deeply."
    },

    {
	"possum",
	"You do your best imitation of a corpse.",
	"$n hits the ground... DEAD.",
	"You do your best imitation of a corpse.",
	"$n hits the ground... DEAD.",
	"$n hits the ground... DEAD.",
	"You do your best imitation of a corpse.",
	"$n hits the ground... DEAD."
    },
    
    {
	"potato",
	"You juggle a hot potato in your hands.",
	"$n juggles a hot potato in $s hands.",
	"You juggle a hot potato and pass it onto $M.",
	"$n juggles a hot potato and gives it to $N!",
	"$n juggles a hot potato and gives it to YOU!",
	"You juggle a hot potato in your hands.",
	"$n juggles a hot potato in $s hands."
    },
    
    {
	"poupon",
	"You say 'Pardon me, do you have any Grey Poupon?'.",
	"$n says 'Pardon me, do you have any Grey Poupon?'",
	"You say to $M, 'Pardon me, do you have any Grey Poupon?'.",
	"$n says to $N, 'Pardon me, do you have any Grey Poupon?'",
	"$n says to you, 'Pardon me, do you have any Grey Poupon?'",
	"You say 'Pardon me, do you have any Grey Poupon?'.",
	"$n says 'Pardon me, do you have any Grey Poupon?'"
    },
    
    {
	"pout",
	"Ah, don't take it so hard.",
	"$n pouts.",
	"Ah, don't take it so hard.",
	"$n pouts at $N.  What did $N do?",
	"$n pouts at you.  What did you do to $m?",
	"Ah, don't take it so hard.",
	"$n pouts."
    },

    {
	"pray",
	"You feel righteous, and maybe a little foolish.",
	"$n begs and grovels to the powers that be.",
	"You crawl in the dust before $M.",
	"$n falls down and grovels in the dirt before $N.",
	"$n kisses the dirt at your feet.",
	"Talk about narcissism ...",
	"$n mumbles a prayer to $mself."
    },

    {   
	"pretend",
	"You pretend you are a GOD, and slay everyone in sight!",
	"$n is pretending $e is an implementor again.  *sigh*",
	"You pretend you are a GOD, and demote $M to level 1.",
	"$n pretends $e is a GOD, and says, '$N, you're demoted to level 1!'",
	"$n pretends $e is a GOD, and says, 'You are demoted to level 1!'",
	"You pretend you are an implementor, and you demote yourself to level 1.",
	"$n pretends $e is a GOD, and demotes $mself to level 1."
    },

    {
	"propose",
	"You propose a contract game of bridge.",
	"$n proposes a contract game of bridge.",
	"You propose marriage to $M.",
	"$n gets down on one knee and proposes to $N.",
	"$n says quietly 'Will you marry me?'",
	"Even you wouldn't marry yourself.",
	"$n refuses the proposal made by $mself."
    },
    
    {
	"puke",
	"You puke ... chunks everywhere!",
	"$n pukes.",
	"You puke on $M.",
	"$n pukes on $N.",
	"$n spews vomit and pukes all over your clothing!",
	"You puke on yourself.",
	"$n pukes on $s clothes."
    },

    {
	"punch",
	"Punch whom?",
	NULL,
	"You punch $M playfully.",
	"$n punches $N playfully.",
	"$n punches you playfully.  OUCH!",
	"You punch yourself.  You deserve it.",
	"$n punches $mself.  Why don't you join in?"
    },

    {
	"purr",
	"MMMMEEEEEEEEOOOOOOOOOWWWWWWWWWWWW.",
	"$n purrs contentedly.",
	"You purr contentedly in $S lap.",
	"$n purrs contentedly in $N's lap.",
	"$n purrs contentedly in your lap.",
	"You purr at yourself.",
	"$n purrs at $mself.  Must be a cat thing."
    },

    {
      "push",
      "You push the history-eraser button! BUT, nothing seems to happen.",
      "$n pushes the history-eraser button! BUT, nothing seems to happen.",
      "You push $M away in disgust.",
      "$n pushes $N away in disgust.",
      "$n pushes you away in disgust.",
      "You push yourself to the limit of your abilities.",
      "$n pushes $mself to the limits of $s abilites."
    },

    {
	"raise",
	"You raise your hand in response.",
	"$n raises $s hand in response.",
	"You raise your hand in response.",
	"$n raises $s hand in response.",
	"$n raises $s hand in response to you.",
	"You raise your hand in response.",
	"$n raises $s hand in response."
    },
   
    {
      "reassure",
      "You reassure everyone that everything will be ok.",
      "$n reassures everyone that everything will be ok.",
      "You hug $M reassuringly.",
      "$n hugs $N reassuringly.",
      "$n hugs you reassuringly.",
      "You try to reassure yourself that everything will be ok.",
      "$n tries to reassure $mself that everything will be ok."
    },

    {   
	"renandstimpy",
	"You say, 'Oh Happy Happy, Joy Joy!'",
	"$n exclaims, 'Oh Happy Happy, Joy Joy!'",
	"You exclaim, 'Oh Happy Happy, Joy Joy!' at the mere thought of $M.",
	"$n exclaims, 'Oh Happy Happy, Joy Joy!' as $e sees $N enter the room.",
	"$n exclaims, 'Oh Happy Happy, Joy Joy!' when $e sees you approach.",
	"You exclaim, 'Oh Happy Happy, Joy Joy!' at the thought of yourself.",
	"$n exclaims, 'Oh Happy Happy, Joy Joy!' at the thought of $mself."
    },  

    {
	"rofl",
	"You roll on the floor laughing hysterically.",
	"$n rolls on the floor laughing hysterically.",
	"You laugh your head off at $S remark.",
	"$n rolls on the floor laughing at $N's remark.",
	"$n can't stop laughing at your remark.",
	"You roll on the floor and laugh at yourself.",
	"$n laughs at $mself.  Join in the fun."
    },
    
    {
	"roll",
	"You roll your eyes.",
	"$n rolls $s eyes.",
	"You roll your eyes at $M.",
	"$n rolls $s eyes at $N.",
	"$n rolls $s eyes at you.",
	"You roll your eyes at yourself.",
	"$n rolls $s eyes at $mself."
    },

    {
	"rub",
	"You rub your eyes.  How long have you been at this?",
	"$n rubs $s eyes.  $n must have been playing all day.",
	"You rub your eyes.  Has $N been playing as long as you have?",
	"$n rubs $s eyes.  $n must have been playing all day.",
	"$n rubs $s eyes.  Have you been playing as long as $m?",
	"You rub your eyes.  How long have you been at this?",
	"$n rubs $s eyes.  $n must have been playing all day."
    },

    {
	"ruffle",
	"You've got to ruffle SOMEONE.",
	NULL,
	"You ruffle $N's hair playfully.",
	"$n ruffles $N's hair playfully.",
	"$n ruffles your hair playfully.",
	"You ruffle your hair.",
	"$n ruffles $s hair."
    },

    {
	"russian",
	"You speak Russian.  Yeah, right.  Dream on.",
	"$n says 'Ya horosho stari malenky koshka.'  Huh?",
	"You speak Russian to $M.  Yeah, right.  Dream on.",
	"$n says to $N 'Ya horosho stari malenky koshka.'  Huh?",
	"$n says to you 'Ya horosho stari malenky koshka.'  Huh?",
	"You speak Russian.  Yeah, right.  Dream on.",
	"$n says 'Ya horosho stari malenky koshka.'  Huh?"
    },
    
    {
	"sad",
	"You put on a glum expression.",
	"$n looks particularly glum today.  *sniff*",
	"You give $M your best glum expression.",
	"$n looks at $N glumly.  *sniff*  Poor $n.",
	"$n looks at you glumly.  *sniff*   Poor $n.",
	"You bow your head and twist your toe in the dirt glumly.",
	"$n bows $s head and twists $s toe in the dirt glumly."
    },

    {
	"salute",
	"You salute smartly.",
	"$n salutes smartly.",
	"You salute $M.",
	"$n salutes $N.",
	"$n salutes you.",
	"Huh?",
	NULL
    },

    {
        "scold",
	"You scold everyone for being so naughty!",
	"$n scolds everyone for being so naughty!",
	"You scold $M for being so naughty!",
	"$n scolds $N for being so naughty!",
	"$n scolds you for being so naughty!",
	"You scold yourself for being so naughty!",
	"$n scolds $mself for being so naughty!"
    },

    {
	"scream",
	"ARRRRRRRRRRGH!!!!!",
	"$n screams loudly!",
	"ARRRRRRRRRRGH!!!!!  Yes, it MUST have been $S fault!!!",
	"$n screams loudly at $N.  Better leave before $e blames you, too!!!",
	"$n screams at you!  That's not nice!  *sniff*",
	"You scream at yourself.  Yes, that's ONE way of relieving tension!",
	"$n screams loudly at $mself!  Is there a full moon up?"
    },

    {
	"shake",
	"You shake your head.",
	"$n shakes $s head.",
	"You shake $S hand.",
	"$n shakes $N's hand.",
	"$n shakes your hand.",
	"You are shaken by yourself.",
	"$n shakes and quivers like a bowl full of jelly."
    },

    {
	"shiver",
	"Brrrrrrrrr.",
	"$n shivers uncomfortably.",
	"You shiver at the thought of fighting $M.",
	"$n shivers at the thought of fighting $N.",
	"$n shivers at the suicidal thought of fighting you.",
	"You shiver to yourself?",
	"$n scares $mself to shivers."
    },

    {
	"shrug",
	"You shrug.",
	"$n shrugs helplessly.",
	"You shrug in response to $S question.",
	"$n shrugs in response to $N's question.",
	"$n shrugs in response to your question.",
	"You shrug to yourself.",
	"$n shrugs to $mself.  What a strange person."
    },

    {
	"sigh",
	"You sigh.",
	"$n sighs loudly.",
	"You sigh as you think of $M.",
	"$n sighs at the sight of $N.",
	"$n sighs as $e thinks of you.  Touching, huh?",
	"You sigh at yourself.  You MUST be lonely.",
	"$n sighs at $mself.  What a sorry sight."
    },

    {
	"sing",
	"You raise your clear voice towards the sky.",
	"$n has begun to sing.",
	"You sing a ballad to $M.",
	"$n sings a ballad to $N.",
	"$n sings a ballad to you!  How sweet!",
	"You sing a little ditty to yourself.",
	"$n sings a little ditty to $mself."
    },

    {
	"slobber",
	"You slobber all over the floor.",
	"$n slobbers all over the floor.",
	"You slobber all over $M.",
	"$n slobbers all over $N.",
	"$n slobbers all over you.",
	"You slobber all down your front.",
	"$n slobbers all over $mself."
    },

    {
	"smile",
	"You smile happily.",
	"$n smiles happily.",
	"You smile at $M.",
	"$n beams a smile at $N.",
	"$n smiles at you.",
	"You smile at yourself.",
	"$n smiles at $mself."
    },

    {
	"smirk",
	"You smirk.",
	"$n smirks.",
	"You smirk at $S saying.",
	"$n smirks at $N's saying.",
	"$n smirks at your saying.",
	"You smirk at yourself.  Okay ...",
	"$n smirks at $s own 'wisdom'."
    },

    {
	"smoke",
	"You calmly light a cigarette and take a puff.",
	"$n calmly lights a cigarette and take a puff.",
	"You blow smoke into $S eyes.",
	"$n blows smoke into $N's eyes.",
	"$n blows smoke rings into your eyes.",
	"You call down lightning and SMOKE yourself.",
	"$n calls down lightning and SMOKES $mself."
    },
	  
    {
	"smooch",
	"You are searching for someone to smooch.",
	"$n is looking for someone to smooch.",
	"You give $M a nice, wet smooch.",
	"$n and $N are smooching in the corner.",
	"$n smooches you passionately on the lips.",
	"You smooch yourself.",
	"$n smooches $mself.  Yuck."
    },
    
    {
	"snap",
	"PRONTO ! You snap your fingers.",
	"$n snaps $s fingers.",
	"You snap back at $M.",
	"$n snaps back at $N.",
	"$n snaps back at you!",
	"You snap yourself to attention.",
	"$n snaps $mself to attention."
    },

    {
	"snarl",
	"You grizzle your teeth and look mean.",
	"$n snarls angrily.",
	"You snarl at $M.",
	"$n snarls at $N.",
	"$n snarls at you, for some reason.",
	"You snarl at yourself.",
	"$n snarls at $mself."
    },

    {
	"sneeze",
	"Gesundheit!",
	"$n sneezes.",
	"$n sneezes.",
	"$n sneezes.",
	"$n sneezes.",
	"$n sneezes.",
	"$n sneezes."
    },

    {
	"snicker",
	"You snicker softly.",
	"$n snickers softly.",
	"You snicker with $M about your shared secret.",
	"$n snickers with $N about their shared secret.",
	"$n snickers with you about your shared secret.",
	"You snicker at your own evil thoughts.",
	"$n snickers at $s own evil thoughts."
    },

    {
	"sniff",
	"You sniff sadly. *SNIFF*",
	"$n sniffs sadly.",
	"You sniff sadly at the way $E is treating you.",
	"$n sniffs sadly at the way $N is treating $m.",
	"$n sniffs sadly at the way you are treating $m.",
	"You sniff sadly at your lost opportunities.",
	"$n sniffs sadly at $mself.  Something MUST be bothering $m."
    },

    {
	"snore",
	"Zzzzzzzzzzzzzzzzz.",
	"$n snores loudly.",
	"$n snores loudly.",
	"$n snores loudly.",
	"$n snores loudly.",
	"$n snores loudly.",
	"$n snores loudly."
    },

    {
        "snort",
        "You snort.",
        "$n takes a sniff of some white powder and looks VERY relieved.",
        "You snort.  Maybe $N would like some?",
        "$n takes a sniff of some white powder and looks VERY relieved.",
        "$n takes a sniff of some white powder and looks VERY relieved.  Like some?",
        "You snort.",
        "$n takes a sniff of some white powder and looks VERY relieved."
    },
    
    {
	"snowball",
	"Whom do you want to throw a snowball at?",
	NULL,
	"You throw a snowball in $N's face.",
	"$n throws a snowball at $N.",
	"$n throws a snowball at you.",
	"You throw a snowball at yourself.",
	"$n throws a snowball at $mself."
    },

    {
	"snuggle",
	"Who?",
	NULL,
	"you snuggle $M.",
	"$n snuggles up to $N.",
	"$n snuggles up to you.",
	"You snuggle up, getting ready to sleep.",
	"$n snuggles up, getting ready to sleep."
    },

    {
	"spam",
	"You mutter 'SPAM' quietly to yourself.",
	"$n sings 'SPAM SPAM SPAM SPAM SPAM SPAM....'",
	"You hurl a can of SPAM at $M.",
	"$n hurls a can of SPAM at $N.",
	"$n SPAMS you viciously.",
	"You mutter 'SPAM' in the corner quietly.",
	"$n mutters 'SPAM SPAM SPAM SPAM'.  Join in!"
    },
    
    {
	"spanish",
	"You speak Spanish or at least you think you do.",
	"$n says 'Naces pendejo, mueres pendejo.'  Uh oh.",
	"You speak Spanish or at least you think you do.",
	"$n says 'Naces pendejo, mueres pendejo.'  Uh oh.",
	"$n says 'Naces pendejo, mueres pendejo.'  Uh oh.",
	"You speak Spanish or at least you think you do.",
	"$n says to $mself 'Naces pendejo, mueres pendejo.'  Uh oh."
    },
    
    {
	"spank",
	"Spank whom?",
	NULL,
	"You spank $M playfully.",
	"$n spanks $N playfully.",
	"$n spanks you playfully.  OUCH!",
	"You spank yourself.  Kinky!",
	"$n spanks $mself.  Kinky!"
    },

    {
	"spin",
	"You twirl in a graceful pirouette.",
	"$n twirls in a graceful pirouette.",
	"You spin $M on one finger.",
	"$n spins $N on $s finger.",
	"$n spins you around on $s finger.",
	"You spin yourself around and around and around....",
	"$n spins $mself around and around and around..."
    },
    
    {
	"squeal",
	"You squeal with delight.",
	"$n squeals with delight.",
	"You squeal at $M.",
	"$n squeals at $N.  Wonder why?",
	"$n squeals at you.  You must be doing something good.",
	"You squeal at yourself.",
	"$n squeals at $mself."
    },
    
    {
	"squeeze",
	"Where, what, how, whom?",
	NULL,
	"You squeeze $M fondly.",
	"$n squeezes $N fondly.",
	"$n squeezes you fondly.",
	"You squeeze yourself - try to relax a little!",
	"$n squeezes $mself."
    },

    {
	"squirm",
	"You squirm guiltily.",
	"$n squirms guiltily.  Looks like $e did it.",
	"You squirm in front of $M.",
	"$n squirms in front of $N.",
	"$n squirms in front of you.  You make $m nervous.",
	"You squirm and squirm and squirm....",
	"$n squirms and squirms and squirm....."
    },
    
    {
	"squish",
	"You squish your toes into the sand.",
	"$n squishes $s toes into the sand.",
	"You squish $M between your legs.",
	"$n squishes $N between $s legs.",
	"$n squishes you between $s legs.",
	"You squish yourself.",
	"$n squishes $mself.  OUCH."
    },

    {
	"stare",
	"You stare at the sky.",
	"$n stares at the sky.",
	"You stare dreamily at $N, completely lost in $S eyes..",
	"$n stares dreamily at $N.",
	"$n stares dreamily at you, completely lost in your eyes.",
	"You stare dreamily at yourself - enough narcissism for now.",
	"$n stares dreamily at $mself - NARCISSIST!"
    },

    {
	"stickup",
	"You don a mask and ask for everyone's gold.",
	"$n says 'This is a stickup.  Gimme yer gold, NOW!'",
	"You don a mask and ask for $S gold.",
	"$n says to $N, 'This is a stickup.  Gimme yer gold, NOW!'",
	"$n says to you, 'This is a stickup.  Gimme yer gold, NOW!'",
	"You extort money from yourself.",
	"$n holds $s weapon to $s throat and says 'Ok, me, give me all my money!'"
    },
   
    {
	"stretch",
	"You stretch and relax your sore muscles.",
	"$n stretches luxuriously.  Make you want to, doesn't it.",
	"You stretch and relax your sore muscles.",
	"$n stretches luxuriously.  Make you want to, doesn't it.",
	"$n stretches luxuriously.  Make you want to, doesn't it.",
	"You stretch and relax your sore muscles.",
	"$n stretches luxuriously.  Make you want to, doesn't it."
    },
    
    {
	"strip",
	"You show some of your shoulder as you begin your performance.",
	"$n shows $s bare shoulder and glances seductively around the room.",
	"You show some of your shoulder as you begin your performance.",
	"$n shows $s bare shoulder and glances seductively at $N.",
	"$n shows $s bare shoulder and glances seductively at you.",
	"You show some of your shoulder as you begin your performance.",
	"$n shows $s bare shoulder and glances seductively around the room."
    },

    {
	"strut",
	"Strut your stuff.",
	"$n struts proudly.",
	"You strut to get $S attention.",
	"$n struts, hoping to get $N's attention.",
	"$n struts, hoping to get your attention.",
	"You strut to yourself, lost in your own world.",
	"$n struts to $mself, lost in $s own world."
    },

    {
	"suffer",
	"No xp again?  You suffer at the hands of fate.",
	"$n is suffering.  Looks like $e can't seem to level.",
	"You tell $M how you suffer whenever you're away from $M.",
	"$n tells $N that $e suffers whenever they're apart.",
	"$n tells you that $e suffers whenever you're apart.",
	"No xp again?  You suffer at the hands of fate.",
	"$n is suffering.  Looks like $e can't seem to level."
    },  
    
    {
	"sulk",
	"You sulk.",
	"$n sulks in the corner.",
	"$n sulks in the corner.",
	"$n sulks in the corner.",
	"$n sulks in the corner.",
	"$n sulks in the corner.",
	"$n sulks in the corner."
    },

    {
	"surf",
	"You stoke your soul by catching a smooth, perfect wave.",
	"$n stokes $s soul by catching a smooth, perfect wave.",
	"You stoke your soul by catching a smooth, perfect wave.",
	"$n stokes $s soul by catching a smooth, perfect wave.",
	"$n stokes $s soul by catching a smooth, perfect wave.",
	"You stoke your soul by catching a smooth, perfect wave.",
	"$n stokes $s soul by catching a smooth, perfect wave."
    },
    
    {
      "sweep",
      "You grab a broom and sweep up the mess you made.",
      "$n grabs a broom and sweeps up the mess $e made.",
      "You sweep $M up off $S feet and swirl $M around in happiness.",
      "$n sweeps $N up off $S feet and swirls $M around in happiness.",
      "$n sweeps you off your feet and swirls you around in happiness.",
      "You swirl around in happiness.",
      "$n swirls $mself around.  Pretty silly, huh?"
    },

    {
	"swoon",
	"You swoon in ecstacy.",
	"$n swoons in ecstacy.",
	"You swoon in ecstacy at the thought of $M.",
	"$n swoons in ecstacy at the thought of $N.",
	"$n swoons in ecstacy as $e thinks of you.",
	"You swoon in ecstacy.",
	"$n swoons in ecstacy."
    },

    {
	"tackle",
	"You can't tackle the AIR!",
	NULL,
	"You run over to $M and bring $M down!",
	"$n runs over to $N and tackles $M to the ground!",
	"$n runs over to you and tackles you to the ground!",
	"You wrap your arms around yourself, and throw yourself to the ground.",
	"$n wraps $s arms around $mself and brings $mself down!?"
    },
    
    {
	"tap",
	"You tap your foot impatiently.",
	"$n taps $s foot impatiently.",
	"You tap your foot impatiently.  Will $E ever be ready?",
	"$n taps $s foot impatiently as $e waits for $N.",
	"$n taps $s foot impatiently as $e waits for you.",
	"You tap yourself on the head.  Ouch!",
	"$n taps $mself on the head."
    },

    {
	"tender",
	"You will enjoy it more if you choose someone to kiss.",
	NULL,
	"You give $M a soft, tender kiss.",
	"$n gives $N a soft, tender kiss.",
	"$n gives you a soft, tender kiss.",
	"You'd better not, people may start to talk!",
	NULL
     },
     
    {
	"thank",
	"Thank you too.",
	NULL,
	"You thank $N heartily.",
	"$n thanks $N heartily.",
	"$n thanks you heartily.",
	"You thank yourself since nobody else wants to !",
	"$n thanks $mself since you won't."
    },

    {
	"think",
	"You think about times past and friends forgotten.",
	"$n thinks deeply and is lost in thought.",
	"You think about times past and friends forgotten.",
	"$n thinks deeply and is lost in thought.",
	"$n thinks deeply and is lost in thought.  Maybe $e is thinking of you?",
	"You think about times past and friends forgotten.",
	"$n thinks deeply and is lost in thought."
    },
   
    {
	"throttle",
	"Whom do you want to throttle?",
	NULL,
	"You throttle $M till $E is blue in the face.",
	"$n throttles $N about the neck, until $E passes out.  THUNK!",
	"$n throttles you about the neck until you pass out.  THUNK!",
	"That might hurt!  Better not do it!",
	"$n is getting a crazy look in $s eye again."
    },

    {
	"tickle",
	"Whom do you want to tickle?",
	NULL,
	"You tickle $N - $E starts giggling.",
	"$n tickles $N - $E starts giggling.",
	"$n tickles you - hee hee hee.",
	"You tickle yourself, how funny!",
	"$n tickles $mself."
    },

    {
	"timeout",
	"You take a 'T' and drink some Gatorade (tm).",
	"$n takes a 'T' and drinks some Gatorade (tm).",
	"You take a 'T' and offer $M some Gatorade (tm).",
	"$n takes a 'T' and offers $N some Gatorade (tm).",
	"$n takes a 'T' and offers you some Gatorade (tm).",
	"You take a 'T' and drink some Gatorade (tm).",
	"$n takes a 'T' and drinks some Gatorade (tm)."
    },
    
    {
	"tongue",
	"You stick out your tongue.",
	"$n sticks out $s tongue.",
	"You stick your tongue out at $M.",
	"$n sticks $s tongue out at $N.",
	"$n sticks $s tongue out at you.",
	"You stick out your tongue and touch your nose.",
	"$n sticks out $s tongue and touches $s nose."
    },

    {
	"torture",
	"You have to torture someone!",
	NULL,
	"You torture $M with rusty weapons, Mwaahhhhh!!",
	"$n tortures $N with rusty weapons, $E must have been REAL bad!",
	"$n tortures you with rusty weapons!  What did you DO!?!",
	"You torture yourself with rusty weapons.  Was it good for you?",
	"$n tortures $mself with rusty weapons.  Looks like $e enjoys it!?"
    },

    {   
	"tummy",
	"You rub your tummy and wish you'd bought a pie at the bakery.",
	"$n rubs $s tummy and wishes $e'd bought a pie at the bakery.",
	"You rub your tummy and ask $M for some food.",
	"$n rubs $s tummy and asks $N for some food.",
	"$n rubs $s tummy and asks you for some food.  Please?",
	"You rub your tummy and wish you'd bought a pie at the bakery.",
	"$n rubs $s tummy and wishes $e'd bought a pie at the bakery."
    },

    {
	"tweety",
	"You exclaim 'I TAWT I TAW A PUTTY TAT!!'",
	"$n exclaims 'I TAWT I TAW A PUTTY TAT!!'",
	"You exclaim to $M, 'I TAWT I TAW A PUTTY TAT!!'",
	"$n exclaims to $N, 'I TAWT I TAW A PUTTY TAT!!'",
	"$n exclaims to you, 'I TAWT I TAW A PUTTY TAT!!'",
	"You exclaim to yourself, 'I TAWT I TAW A PUTTY TAT!!'",
	"$n exclaims to $mself, 'I TAWT I TAW A PUTTY TAT!!'"
    },
    
    {
	"twiddle",
	"You patiently twiddle your thumbs.",
	"$n patiently twiddles $s thumbs.",
	"You twiddle $S ears.",
	"$n twiddles $N's ears.",
	"$n twiddles your ears.",
	"You twiddle your ears like Dumbo.",
	"$n twiddles $s own ears like Dumbo."
    },

    {
	"type",
	"You throw up yor handz in dizgust at yur losy typing skils.",
	"$n couldn't type a period if there was only one key on the keyboard.",
	"You throw up yor handz in dizgust at yur losy typing skils.",
	"$n couldn't type a period if there was only one key on the keyboard.",
	"$n couldn't type a period if there was only one key on the keyboard.",
	"You throw up yor handz in dizgust at yur losy typing skils.",
	"$n couldn't type a period if there was only one key on the keyboard."
    },
    
    {
	"wave",
	"You wave.",
	"$n waves happily.",
	"You wave goodbye to $N.",
	"$n waves goodbye to $N.",
	"$n waves goodbye to you.  Have a good journey.",
	"Are you going on adventures as well?",
	"$n waves goodbye to $mself."
    },

    {
	"wedgie",
	"You look around for someone to wedgie.",
	"$n is looking around for someone to wedgie!  Run!",
	"You wedgie $M.  Must be fun! ",
	"$n wedgies $N to the heavens.",
	"$n wedgies you!  Ouch!",
	"You delight in pinning your underwear to the sky.",
	"$n wedgies $mself and revels with glee."
    },

    {
	"whine",
	"You whine like the great whiners of the century.",
	"$n whines 'I want to be a god already.  I need more hitpoints..I...'",
	"You whine to $M like the great whiners of the century.",
	"$n whines to $N 'I want to be an immortal already.  I need more hp...I..'",
	"$n whines to you 'I want to be an immortal already.  I need more hp...I...'",
	"You whine like the great whiners of the century.",
	"$n whines 'I want to be a god already.  I need more hitpoints..I...'"
    },
    
    {
	"whistle",
	"You whistle appreciatively.",
	"$n whistles appreciatively.",
	"You whistle at the sight of $M.",
	"$n whistles at the sight of $N.",
	"$n whistles at the sight of you.",
	"You whistle a little tune to yourself.",
	"$n whistles a little tune to $mself."
    },

    {
	"wiggle",
	"You wiggle your bottom.",
	"$n wiggles $s bottom.",
	"You wiggle your bottom toward $M.",
	"$n wiggles $s bottom toward $N.",
	"$n wiggles $s bottom toward you.",
	"You wiggle about like a fish.",
	"$n wiggles about like a fish."
    },

    {
	"wince",
	"You wince.  Ouch!",
	"$n winces.  Ouch!",
	"You wince at $M.",
	"$n winces at $N.",
	"$n winces at you.",
	"You wince at yourself.  Ouch!",
	"$n winces at $mself.  Ouch!"
    },

    {
	"wink",
	"You wink suggestively.",
	"$n winks suggestively.",
	"You wink suggestively at $N.",
	"$n winks at $N.",
	"$n winks suggestively at you.",
	"You wink at yourself ?? - what are you up to ?",
	"$n winks at $mself - something strange is going on..."
    },

    {
	"wish",
	"You make a wish upon a star for a dream you have.",
	"$n sighs... and makes a wish upon a shooting star.",
	"You wish to be loved by $M.",
	"$n wishes to be loved by $N.",
	"$n wishes to be loved by you.",
	"You wish for more attention.",
	"$n wishes for more attention."
    },

    {
	"worship",
	"You worship the powers that be.",
	"$n worships the powers that be.",
	"You drop to your knees in homage of $M.",
	"$n prostrates $mself before $N.",
	"$n believes you are all powerful.",
	"You worship yourself.",
	"$n worships $mself - ah, the conceitedness of it all."
    },
	
    {
	"wrap",
	"You wrap a present for your love.",
	"$n wraps a present for someone special.",
	"You wrap your legs around $M.",
	"$n wraps $s legs around $N.",
	"$n wraps $s legs around you.  Wonder what's coming next?",
	"You wrap yourself with some paper.",
	"$n wraps $mself with some wrapping paper.  Is it Christmas?"
    },
	
    {
	"yabba",
	"YABBA-DABBA-DOO!",
	"$n hollers 'Hey Wilma -- YABBA DABBA DOO!'",
	"You holler 'Hey $N -- YABBA DABBA DOO!'",
	"$n hollers 'Hey $N -- YABBA DABBA DOO!'",
	"$n hollers 'Hey $N -- YABBA DABBA DOO!'",
	"YABBA-DABBA-DOO!",
	"$n hollers 'Hey Wilma -- YABBA DABBA DOO!'"
    },
   
    {
	"yahoo",
	"You do your best Han Solo impression.",
	"$n exclaims, 'YAHOO!  Great shot, kid, that was one in a million!'",
	"You do your best Han Solo impression.",
	"$n exclaims, 'YAHOO!  Great shot, $N, that was one in a million!'",
	"$n exclaims, 'YAHOO!  Great shot, $N, that was one in a million!'",
	"You do your best Han Solo impression.",
	"$n exclaims, 'YAHOO!  Great shot, $n, that was one in a million!'"
    },
    
    {
	"yawn",
	"You must be tired.",
	"$n yawns.",
	"You must be tired.",
	"$n yawns at $N.  Maybe you should let them go to bed?",
	"$n yawns at you.  Maybe $e wants you to go to bed with $m?",
	"You must be tired.",
	"$n yawns."
    },
	 
    {
	"yeehaw",
	"You mount your white pony (?) and shout 'YEEEEEEEEEEHAW!'",
	"$n mounts $s white pony (?) and shouts 'YEEEEHHHAAAAAAWWWW!'",
	"You mount your white pony (?) and shout 'YEEEEEEEEEEHAW!'",
	"$n mounts $s white pony (?) and shouts 'YEEEEHHHAAAAAAWWWW!'",
	"$n mounts $s white pony (?) and shouts 'YEEEEHHHAAAAAAWWWW!'",
	"You mount your white pony (?) and shout 'YEEEEEEEEEEHAW!'",
	"$n mounts $s white pony (?) and shouts 'YEEEEHHHAAAAAAWWWW!'"
    },

    {
	"",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL
    }
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



bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        cmd;
    bool       found;

    found  = FALSE;
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == social_table[cmd].name[0]
	    && !str_prefix( command, social_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
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
	if ( !str_cmp( social_table[cmd].name, "snore" ) )
	    break;
	send_to_char( "In your dreams, or what?\n\r",             ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
	act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
	act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
    }
    else if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char( "They aren't here.\n\r",                    ch );
    }
    else if ( victim == ch )
    {
	act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR    );
	act( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM    );
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
	    strcat( ldmsg, social_table[cmd].char_found );
	    act( ldmsg,                       ch, NULL, victim, TO_CHAR    );

	    strcpy( ldmsg, ldbase );
	    strcat( ldmsg, social_table[cmd].vict_found );
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
	act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
	act( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT    );
	act( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT );

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
		act( social_table[cmd].char_found,
		    victim, NULL, ch, TO_CHAR    );
		act( social_table[cmd].vict_found,
		    victim, NULL, ch, TO_VICT    );
		act( social_table[cmd].others_found,
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
