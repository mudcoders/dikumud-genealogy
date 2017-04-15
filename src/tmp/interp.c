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
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#define unix 1
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
bool    check_alias     args( ( CHAR_DATA *ch, char *command,
			       char *argument ) );
void    add_alias   args( ( CHAR_DATA *ch, ALIAS_DATA *pAl,
                                char *old, char *new ) );
bool	can_use_cmd	args( ( int cmd, CHAR_DATA *ch, int trust ) );

/*
 * Command logging types.
 */
#define LOG_NORMAL              0
#define LOG_ALWAYS              1
#define LOG_NEVER               2
/*
 * Altrag was here.
 */
#define LOG_BUILD               3


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
struct	cmd_type	cmd_table	[ ] =
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

/* XOR */
    { "push",		do_push,	POS_STANDING,	 0,  LOG_NORMAL },
    { "drag",		do_drag,	POS_STANDING,	 0,  LOG_NORMAL },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "accept",		do_accept,	POS_STANDING,	 0,  LOG_NORMAL },
    { "buy",		do_buy,		POS_STANDING,	 0,  LOG_NORMAL	},
    { "cast",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "challenge",	do_challenge,	POS_STANDING,	 0,  LOG_NORMAL },
    { "deposit",	do_deposit,     POS_RESTING,     0, LOG_NORMAL  },
    { "dual",           do_dual,        POS_RESTING,     0,  LOG_NORMAL },
    { "exits",		do_exits,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "get",		do_get,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "palm",		do_palm,	POS_RESTING,	 0,  LOG_NORMAL },
    { "inventory",	do_inventory,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "invoke",         do_invoke,      POS_RESTING,     0,  LOG_NORMAL },
    { "kill",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "look",		do_look,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "order",		do_order,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "rest",		do_rest,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "repair",         do_repair,      POS_RESTING,     0,  LOG_NORMAL },
    { "sing",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "stand",		do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "tell",		do_tell,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "track",          do_track,       POS_STANDING,    0,  LOG_NORMAL },
    { "wield",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "wizhelp",	do_wizhelp,	POS_DEAD,        0,  LOG_NORMAL	},

    { "mpasound",       do_mpasound,    POS_DEAD,        0,  LOG_NORMAL },
    { "mpjunk",         do_mpjunk,      POS_DEAD,        0,  LOG_NORMAL },
    { "mpecho",         do_mpecho,      POS_DEAD,        0,  LOG_NORMAL },
    { "mpechoat",       do_mpechoat,    POS_DEAD,        0,  LOG_NORMAL },
    { "mpechoaround",   do_mpechoaround,POS_DEAD,        0,  LOG_NORMAL },
    { "mpkill",         do_mpkill      ,POS_DEAD,        0,  LOG_NORMAL },
    { "mpmload",        do_mpmload     ,POS_DEAD,        0,  LOG_NORMAL },
    { "mpoload",        do_mpoload     ,POS_DEAD,        0,  LOG_NORMAL },
    { "mppurge",        do_mppurge     ,POS_DEAD,        0,  LOG_NORMAL },
    { "mpgoto",         do_mpgoto      ,POS_DEAD,        0,  LOG_NORMAL },
    { "mpat",           do_mpat        ,POS_DEAD,        0,  LOG_NORMAL },
    { "mptransfer",     do_mptransfer  ,POS_DEAD,        0,  LOG_NORMAL },
    { "mpforce",        do_mpforce     ,POS_DEAD,        0,  LOG_NORMAL },
    { "mpteleport",     do_mpteleport,  POS_DEAD,        0,  LOG_NORMAL },

    /*
     * Informational commands.
     */
    { "affected",       do_affectedby,  POS_DEAD,        0,  LOG_NORMAL },
    { "raffected",	do_raffect,	POS_DEAD,	 0,  LOG_NORMAL },
    { "areas",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "bug",		do_bug,		POS_DEAD,	 0,  LOG_NORMAL	},
    { "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL },
    { "compare",	do_compare,	POS_RESTING,	 0,  LOG_NORMAL },
    { "consider",	do_consider,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "enter",          do_enter,       POS_STANDING,    0,  LOG_NORMAL },
    { "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "examine",	do_examine,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "finger",         do_finger,      POS_DEAD,        0,  LOG_ALWAYS },
    { "forge",		do_forge,	POS_STANDING,   30,  LOG_ALWAYS },
    { "help",		do_help,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "idea",		do_idea,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "report",		do_report,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "pagelength",     do_pagelen,     POS_DEAD,        0,  LOG_NORMAL },
    { "read",		do_look,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "score",		do_score,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "scent",		do_scent,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "slist",          do_slist,       POS_DEAD,        0,  LOG_NORMAL },
    { "socials",	do_socials,	POS_DEAD,	 0,  LOG_NORMAL },
    { "time",		do_time,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "typo",		do_typo,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "weather",	do_weather,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "worth",          do_worth,       POS_DEAD,        0,  LOG_NORMAL },
    { "who",		do_who,		POS_DEAD,	 0,  LOG_NORMAL	},
    { "wizlist",        do_immlist,     POS_DEAD,        0,  LOG_NORMAL },
    { "champlist",	do_champlist,	POS_DEAD,	 0,  LOG_NORMAL },
    { "newbie",		do_newbie,	POS_DEAD,        0,  LOG_NORMAL },

    /*
     * Clan commands.
     */
    { "induct",        do_induct,       POS_STANDING,    0,  LOG_ALWAYS },
    { "outcast",       do_outcast,      POS_STANDING,    0,  LOG_ALWAYS },
    { "setlev",        do_setlev,       POS_STANDING,    0,  LOG_ALWAYS },
    { "cinfo",         do_cinfo,        POS_DEAD,        0,  LOG_NORMAL },
 /* { "assasinate",    do_assasinate,   POS_STANDING,   30,  LOG_ALWAYS },*/
    { "farsight",      do_farsight,     POS_SLEEPING,   30,  LOG_NORMAL },
    { "cquest",        do_clanquest,    POS_DEAD,        0,  LOG_NORMAL },

    /*
     * Guild commands
     */
    { "guild",		do_guild,	POS_DEAD,	 0,  LOG_ALWAYS },
    { "unguild",	do_unguild,	POS_DEAD,	 0,  LOG_ALWAYS	},
    { "setrank",	do_setrank,	POS_DEAD,	 0,  LOG_ALWAYS	},
    { "gdt",		do_gdt,		POS_SLEEPING,	 0,  LOG_NORMAL },
    { "guildtalk",	do_gdt,		POS_SLEEPING,	 0,  LOG_NORMAL },
    { "guilds",         do_guilds,      POS_DEAD,        0,  LOG_NORMAL },
    /*
     * Configuration commands.
     */
    { "alias",          do_alias,       POS_DEAD,        0,  LOG_NORMAL },
    { "auto",           do_auto,        POS_DEAD,        0,  LOG_NORMAL },
    { "autoexit",       do_autoexit,    POS_DEAD,        0,  LOG_NORMAL },
    { "autoloot",       do_autoloot,    POS_DEAD,        0,  LOG_NORMAL },
    { "autocoins",      do_autocoins,   POS_DEAD,        0,  LOG_NORMAL },
    { "autosac",        do_autosac,     POS_DEAD,        0,  LOG_NORMAL },
    { "blank",          do_blank,       POS_DEAD,        0,  LOG_NORMAL },
    { "brief",          do_brief,       POS_DEAD,        0,  LOG_NORMAL },
    { "channels",	do_channels,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "combine",        do_combine,     POS_DEAD,        0,  LOG_NORMAL },
    { "autosplit",      do_autosplit,   POS_DEAD,        0,  LOG_NORMAL },
    { "ansi",           do_ansi,    	POS_DEAD,        0,  LOG_NORMAL },
    { "fullname",       do_fullname,    POS_DEAD,        0,  LOG_NORMAL },
    { "combat",         do_combat,      POS_DEAD,        0,  LOG_NORMAL },
    { "telnetga",       do_telnetga,    POS_DEAD,        0,  LOG_NORMAL },
    { "config",		do_config,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "description",	do_description,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "email",		do_email,	POS_DEAD,	 0,  LOG_NORMAL },
    { "password",	do_password,	POS_DEAD,	 0,  LOG_NEVER	},
    { "plan",		do_plan,	POS_DEAD,	 0,  LOG_NORMAL },
    { "pr",     	do_practice,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "prompt",         do_prompt,      POS_DEAD,        0,  LOG_NORMAL },
    { "title",		do_title,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "wimpy",		do_wimpy,	POS_DEAD,	 0,  LOG_NORMAL	},
/*    { "qjoin",		do_questflag,   POS_DEAD, 0,LOG_NORMAL },*/
    /*
     * Communication commands.
     */
    { "answer",		do_answer,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "auction",	do_auction,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "bid",            do_bid,         POS_RESTING,     0,  LOG_NORMAL },
    { "chat",		do_chat,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { ".",		do_chat,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "emote",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL	},
    { ",",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "gtell",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL	},
    { ";",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "music",		do_music,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "note",		do_note,	POS_SLEEPING,	 0,  LOG_NORMAL	}, 
#if 0
    { "pose",		do_pose,	POS_RESTING,	 0,  LOG_NORMAL	},
#endif
    { "quest",          do_quest,       POS_RESTING,     0,  LOG_NORMAL },
    { "question",	do_question,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "reply",		do_reply,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "say",		do_say,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "'",		do_say,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "shout",		do_shout,	POS_RESTING,	 3,  LOG_NORMAL	},
    { "yell",		do_yell,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "clantalk",       do_clan,        POS_DEAD,        0,  LOG_NORMAL },
    { "clans",          do_clans,       POS_DEAD,        0,  LOG_NORMAL },
    { "class",          do_class,       POS_DEAD,        0,  LOG_NORMAL },
    { ">",              do_class,       POS_DEAD,        0,  LOG_NORMAL },
    { "ooc",		do_ooc,		POS_DEAD,	 0,  LOG_NORMAL },
    { "]",		do_ooc,		POS_DEAD,	 0,  LOG_NORMAL },
    { "guard",          do_guard,       POS_DEAD,    L_CON,  LOG_NORMAL },
    { "[",		do_guard,	POS_DEAD,    L_CON,  LOG_NORMAL },
    { "conference",     do_conference,  POS_DEAD,    L_IMP,  LOG_ALWAYS },
    { "info",		do_info,	POS_DEAD,    L_DIR,  LOG_ALWAYS },

/*    { "vent",           do_vent,        POS_DEAD,        0,  LOG_NORMAL },
    { "/",              do_vent,        POS_DEAD,        0,  LOG_NORMAL }, */
    { "clear_alias",    do_clear,       POS_DEAD,        0,  LOG_NORMAL },

    /*
     * Object manipulation commands.
     */
    { "brandish",	do_brandish,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "close",		do_close,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "donate",		do_donate,	POS_RESTING,	 0,  LOG_NORMAL },
    { "drink",		do_drink,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "drop",		do_drop,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "eat",		do_eat,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "fill",		do_fill,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "give",		do_give,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "hold",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "join",           do_join,        POS_RESTING,     0,  LOG_NORMAL },
    { "list",		do_list,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "lock",		do_lock,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "open",		do_open,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "patch",          do_patch,       POS_STANDING,    26, LOG_NORMAL },
    { "pick",		do_pick,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "put",		do_put,		POS_RESTING,	 0,  LOG_NORMAL },
    { "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "recall",         do_lowrecall,   POS_STANDING,    0,  LOG_NORMAL },
    { "recite",		do_recite,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "remove",		do_remove,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "retrieve",       do_retrieve,    POS_STANDING,    2,  LOG_NORMAL },
    { "sell",		do_sell,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "take",		do_get,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "sacrifice",	do_sacrifice,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "separate",       do_separate,    POS_RESTING,     0,  LOG_NORMAL },
    { "store",          do_store,       POS_STANDING,    2,  LOG_NORMAL },
    { "unlock",		do_unlock,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "value",		do_value,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "wear",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "zap",		do_zap,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "stare",          do_stare,       POS_RESTING,     0,  LOG_NORMAL },
    { "alchemy",        do_alchemy,     POS_RESTING,     0,  LOG_ALWAYS },
    { "antidote",	do_antidote,	POS_RESTING,	 0,  LOG_NORMAL },
    { "scribe",         do_scribe,      POS_RESTING,     0,  LOG_ALWAYS },
    { "gravebind",	do_gravebind,	POS_RESTING,	 0,  LOG_NORMAL },
    { "flamehand",	do_flamehand,	POS_RESTING,	 0,  LOG_NORMAL },
    { "frosthand",	do_frosthand,	POS_RESTING,	 0,  LOG_NORMAL },
    { "chaoshand",	do_chaoshand,	POS_RESTING,	 0,  LOG_NORMAL },
    /*
     * Combat commands.
     */
    { "backstab",	do_backstab,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "bladepalm",	do_bladepalm,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "bs",		do_backstab,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "df",		do_drowfire,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "disarm",		do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "flee",		do_flee,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "retreat",	do_retreat,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "flury",		do_flury,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "kick",		do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "high kick",	do_high_kick,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "jump kick",	do_jump_kick,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "spin kick",	do_spin_kick,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "feed",           do_feed,        POS_FIGHTING,    0,  LOG_NORMAL },
    { "flying kick",	do_flyingkick,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "punch",          do_punch,       POS_FIGHTING,    0,  LOG_NORMAL },
    { "jab punch",      do_jab_punch,   POS_FIGHTING,    0,  LOG_NORMAL },
    { "kidney punch",   do_kidney_punch,POS_FIGHTING,    0,  LOG_NORMAL },
    { "cross punch",    do_cross_punch, POS_FIGHTING,    0,  LOG_NORMAL },
    { "roundhouse punch",do_roundhouse_punch,POS_FIGHTING,0, LOG_NORMAL },
    { "uppercut punch", do_uppercut_punch,POS_FIGHTING,  0,  LOG_NORMAL },
    { "murde",		do_murde,	POS_FIGHTING,	 5,  LOG_NORMAL	},
    { "murder",		do_murder,	POS_FIGHTING,	 5,  LOG_ALWAYS	},
    { "nerve",		do_nerve,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "rescue",		do_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "throw",	        do_throw,	POS_STANDING,    0,  LOG_NORMAL }, 
    { "trip",		do_trip,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "drain life",	do_drain_life,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "mental drain",   do_mental_drain,POS_FIGHTING,    0,  LOG_NORMAL },
    { "shriek",		do_shriek,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "snatch",		do_snatch,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "spit",		do_spit,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "stun",           do_stun,        POS_FIGHTING,    0,  LOG_NORMAL },
    { "berserk",        do_berserk,     POS_FIGHTING,    0,  LOG_NORMAL },
    { "soulstrike",     do_soulstrike,  POS_FIGHTING,    0,  LOG_NORMAL },
    { "multiburst",	do_multiburst,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "bite",		do_bite,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "howl of fear",	do_howl_of_fear,POS_FIGHTING,	 0,  LOG_NORMAL },
    { "frenzy",		do_frenzy,	POS_FIGHTING,	 0,  LOG_NORMAL },

    /*
     * Miscellaneous commands.
     */
    { "account",        do_account,     POS_RESTING,     0,  LOG_NORMAL },
    { "adrenaline rush",do_rush,	POS_RESTING,	 0,  LOG_NORMAL },
    { "afk",		do_afk,		POS_DEAD,	 0,  LOG_NORMAL },
    { "afkmes",         do_afkmes,      POS_DEAD,        0,  LOG_NORMAL }, 
    { "bash door",      do_bash,        POS_STANDING,    0,  LOG_NORMAL },
    { "chameleon power",do_chameleon,   POS_STANDING,    0,  LOG_NORMAL },
    { "follow",		do_follow,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "group",		do_group,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "globe",		do_globedarkness, POS_STANDING,  0,  LOG_NORMAL },
    { "heighten senses",do_heighten,    POS_STANDING,    0,  LOG_NORMAL },
    { "enhance reflexes",do_reflex,	POS_STANDING,	 0,  LOG_NORMAL },
    { "hide",		do_hide,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "iron grip",      do_irongrip,    POS_STANDING,    0,  LOG_NORMAL },
    { "ironfist",	do_ironfist,	POS_STANDING,	 0,  LOG_NORMAL },
    { "poison weapon",	do_poison_weapon,POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "practice",	do_practice,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "qui",		do_qui,		POS_DEAD,	 0,  LOG_NORMAL	},
    { "quit",		do_quit,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "rage",		do_rage,	POS_DEAD,	 0,  LOG_NORMAL },
    { "remote",		do_remote,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "<",		do_remote,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "rent",		do_rent,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "save",		do_save,	POS_DEAD,	 1,  LOG_NORMAL	},
    { "shadow",         do_shadow_walk, POS_STANDING,    0,  LOG_NORMAL },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "sneak",		do_sneak,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "spells",         do_spells,      POS_SLEEPING,    L_IMP, LOG_NORMAL},
    { "split",		do_split,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "steal",		do_steal,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "train",		do_train,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "visible",	do_visible,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "ward",		do_ward,    POS_STANDING, L_CHAMP1,  LOG_ALWAYS },
    { "where",		do_where,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "withdraw",       do_withdraw,    POS_RESTING,     0,  LOG_NORMAL },
    { "pray",           do_pray,        POS_RESTING,     0,  LOG_NORMAL },
    { "delet",		do_delet,	POS_STANDING,    0,  LOG_NORMAL },
    { "delete",		do_delete,	POS_STANDING,    0,  LOG_ALWAYS },
    { "pkill",		do_pkill,	POS_DEAD,	 0,  LOG_NORMAL },

    /*
     * Artifactor commands
     */
    { "indestructable", do_indestructable, POS_STANDING, 0,  LOG_NORMAL },
    { "remake",		do_remake,  	POS_STANDING,	 0,  LOG_NORMAL },

    /*
     * Immortal commands.
     */

/*    { "vnum",           do_vnum,        POS_DEAD,  L_DIR,  LOG_NORMAL }, */
/*    { "load",           do_load,        POS_DEAD,  L_DIR,  LOG_NORMAL }, */

    { "advance",        do_advance,     POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "allow",          do_allow,       POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "astat",          do_astat,       POS_DEAD,  L_JUN,  LOG_NORMAL },
    { "astrip",         do_astrip,      POS_DEAD,  L_JUN,  LOG_NORMAL },
    { "at",             do_at,          POS_DEAD,  L_APP,  LOG_NORMAL },
    { "authorize",      do_authorize,   POS_DEAD,  L_APP,  LOG_ALWAYS },
    { "bamfmes",        do_bamf,        POS_DEAD,  L_APP,  LOG_NORMAL },
    { "banlist",	do_banlist,     POS_DEAD,  L_GOD,  LOG_NORMAL },
    { "bodybag",        do_bodybag,     POS_DEAD,  L_APP,  LOG_ALWAYS },
    { "changes",	do_changes,	POS_DEAD,  L_DIR,  LOG_NORMAL },
    { "clandesc",       do_clandesc,    POS_DEAD,  L_SEN,  LOG_NORMAL },
    { "cloak",          do_cloak,       POS_DEAD,  L_DIR,  LOG_NORMAL },
    { "clone",		do_clone,       POS_DEAD,  L_SEN,  LOG_NORMAL },
    { "countcommands",  do_countcommands,POS_DEAD, L_IMP,  LOG_NORMAL },
    { "cset",           do_cset,        POS_DEAD,  L_DIR,  LOG_BUILD  },
    { "deny",           do_deny,        POS_DEAD,  L_GOD,  LOG_ALWAYS },
    { "desc_check",	do_desc_check,	POS_DEAD,  L_IMP,  LOG_NORMAL },
    { "desc_clean",     do_descript_clean, POS_DEAD, L_IMP,LOG_NORMAL },
    { "detract",	do_detract,	POS_DEAD,  L_IMP,  LOG_ALWAYS },
    { "disconnect",     do_disconnect,  POS_DEAD,  L_GOD,  LOG_ALWAYS },
    { "echo",           do_echo,        POS_DEAD,  L_JUN,  LOG_ALWAYS },
    { "empower",	do_empower,     POS_DEAD,  L_IMP,  LOG_ALWAYS },
    { "force",          do_force,       POS_DEAD,  L_DEM,  LOG_ALWAYS },
    { "freeze",         do_freeze,      POS_DEAD,  L_GOD,  LOG_ALWAYS },
    { "goto",           do_goto,        POS_DEAD,  L_APP,  LOG_NORMAL },
    { "hero",           do_hero,        POS_DEAD,    100,    LOG_NORMAL },
    { "hlist",          do_hlist,       POS_DEAD,    L_APP,  LOG_NORMAL },
    { "holylight",      do_holylight,   POS_DEAD,  L_APP,  LOG_NORMAL },
#if defined ( HOTREBOOT )
    { "hotreboo",      	do_hotreboo,    POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "hotreboot",      do_hotreboot,   POS_DEAD,  L_DIR,  LOG_ALWAYS },
#endif
    { "ideas",          do_ideas,       POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "imc",		do_imc,         POS_DEAD,  L_APP,  LOG_NORMAL },
    { "immtalk",        do_immtalk,     POS_DEAD,  L_APP,  LOG_NORMAL },
    { "log",            do_log,         POS_DEAD,  L_CON,  LOG_ALWAYS },
    { "memory",         do_memory,      POS_DEAD,  L_GOD,  LOG_NORMAL },
    { "mfind",          do_mfind,       POS_DEAD,  L_JUN,  LOG_NORMAL },
    { "mload",          do_mload,       POS_DEAD,  L_DEM,  LOG_BUILD  },
    { "mpstat",         do_mpstat,      POS_DEAD,  L_APP,  LOG_NORMAL },
    { "mpcommands",     do_mpcommands,  POS_DEAD,  L_APP,  LOG_NORMAL },
    { "mset",           do_mset,        POS_DEAD,  L_DIR,  LOG_BUILD  },
    { "mstat",          do_mstat,       POS_DEAD,  L_JUN,  LOG_NORMAL },
    { "mwhere",         do_mwhere,      POS_DEAD,  L_JUN,  LOG_NORMAL },
    { "newban",		do_newban,      POS_DEAD,  L_GOD,  LOG_ALWAYS },
    { "newcorpse",	do_newcorpse,   POS_DEAD,  L_GOD,  LOG_ALWAYS },
    { "newlock",        do_newlock,     POS_DEAD,  L_IMP,  LOG_ALWAYS },
    { "noemote",        do_noemote,     POS_DEAD,  L_GOD,  LOG_NORMAL },
    { "notell",         do_notell,      POS_DEAD,  L_GOD,  LOG_NORMAL },
    { "nukerep",	do_nukerep,	POS_DEAD,  L_GOD,  LOG_NORMAL },
    { "numlock",        do_numlock,     POS_DEAD,  L_CON,  LOG_ALWAYS },
    { "ofind",          do_ofind,       POS_DEAD,  L_JUN,  LOG_NORMAL },
    { "oload",          do_oload,       POS_DEAD,  L_JUN,  LOG_BUILD  },
    { "opstat",		do_opstat,	POS_DEAD,    L_APP,  LOG_NORMAL },
    { "oset",           do_oset,        POS_DEAD,  L_DIR,  LOG_BUILD  },
    { "ostat",          do_ostat,       POS_DEAD,  L_JUN,  LOG_NORMAL },
    { "owhere",         do_owhere,      POS_DEAD,  L_JUN,  LOG_NORMAL },
    { "pardon",         do_pardon,      POS_DEAD,  L_IMP,  LOG_ALWAYS },
    { "peace",          do_peace,       POS_DEAD,  L_DEM,  LOG_NORMAL },
    { "permban",	do_permban,	    POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "plist",	        do_playerlist,  POS_DEAD,    L_APP,  LOG_NORMAL },
    { "pload",          do_pload,       POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "purge",          do_purge,       POS_DEAD,  L_DEM,  LOG_NORMAL },
    { "qecho",          do_qecho,       POS_DEAD,  L_GOD,  LOG_ALWAYS },
    { "qset",           do_qset,        POS_DEAD,  L_GOD,  LOG_ALWAYS },
    { "racelist",	do_racelist,	POS_DEAD,  L_APP,  LOG_NORMAL },
    { "rebuild",        do_rebuild,	    POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "reboo",          do_reboo,       POS_DEAD,  L_DIR,  LOG_NORMAL },
    { "reboot",         do_reboot,      POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "recho",          do_recho,       POS_DEAD,  L_JUN,  LOG_ALWAYS },
    { "restore",        do_restore,     POS_DEAD,  L_GOD,  LOG_ALWAYS },
    { "restrict",       do_restrict,    POS_DEAD,  L_GOD,  LOG_NORMAL },
    { "rpstat",		do_rpstat,	POS_DEAD,  L_APP,  LOG_NORMAL },
    { "rset",           do_rset,        POS_DEAD,  L_DIR,  LOG_BUILD  },
    { "rstat",          do_rstat,       POS_DEAD,  L_JUN,  LOG_NORMAL },
    { "seize",		do_seize,       POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "shutdow",        do_shutdow,     POS_DEAD,  L_CON,  LOG_NORMAL },
    { "shutdown",       do_shutdown,    POS_DEAD,  L_CON,  LOG_ALWAYS },
    { "silence",        do_silence,     POS_DEAD,  L_GOD,  LOG_NORMAL },
    { "sla",            do_sla,         POS_DEAD,  L_GOD,  LOG_NORMAL },
    { "slay",           do_slay,        POS_DEAD,  L_GOD,  LOG_ALWAYS },
    { "slaymes",        do_slaymes,     POS_DEAD,  L_GOD,  LOG_NORMAL },
    { "slookup",        do_slookup,     POS_DEAD,  L_DIR,  LOG_NORMAL },
    { "snoop",          do_snoop,       POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "sset",           do_sset,        POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "sstat",          do_sstat,       POS_DEAD,  L_DIR,  LOG_NORMAL },
    { "sstime",         do_sstime,      POS_DEAD,  L_CON,  LOG_ALWAYS },
    { "tempban",	do_tempban,     POS_DEAD,  L_GOD,  LOG_ALWAYS },
    { "todo",           do_todo,        POS_DEAD,  L_DIR,  LOG_ALWAYS },
    { "transfer",       do_transfer,    POS_DEAD,  L_JUN,  LOG_ALWAYS },
    { "transmes",       do_trmes,       POS_DEAD,  L_JUN,  LOG_NORMAL },
    { "trust",          do_trust,       POS_DEAD,  L_IMP,  LOG_ALWAYS },
    { "typos",          do_typos,       POS_DEAD,  L_SEN,  LOG_ALWAYS },
    { "users",          do_users,       POS_DEAD,  L_DIR,  LOG_NORMAL },
    { "vused",          do_vused,       POS_DEAD,  L_DIR,  LOG_NORMAL },
    { "whotype",	do_whotype,	POS_DEAD,    L_CON,  LOG_NORMAL },
    { "wizify",         do_wizify,      POS_DEAD,  L_CON,  LOG_ALWAYS },
    { "wizlock",        do_wizlock,     POS_DEAD,  L_IMP,  LOG_ALWAYS },
    { "wizinvis",       do_invis,       POS_DEAD,  L_DIR,  LOG_NORMAL },
    { "wiznet",		do_wiznet,      POS_DEAD,  L_JUN,  LOG_NORMAL },
    { "wrlist",         do_wrlist,      POS_DEAD,  L_JUN,  LOG_NORMAL },
    { ":",              do_immtalk,     POS_DEAD,  L_APP,  LOG_NORMAL },
    { "=",              do_imc,         POS_DEAD,  L_APP,  LOG_NORMAL },
    { "-",              do_hero,        POS_DEAD,    100,    LOG_NORMAL },

 /*{ "fighting",        do_fighting,    POS_DEAD,  L_GOD,  LOG_NORMAL },*/
/*    { "switch",       do_switch,      POS_DEAD,  L_DIR,  LOG_ALWAYS }, 
    { "return",       do_return,      POS_DEAD,  L_JUN,  LOG_NORMAL }, */
/*{ "pwhere",           do_pwhere,      POS_DEAD,    105,  LOG_NORMAL },*/
/*    { "bamfin",         do_bamfin,      POS_DEAD,  L_APP,  LOG_NORMAL },
    { "bamfout",        do_bamfout,     POS_DEAD,  L_APP,  LOG_NORMAL }, */

    /*
     * Here because they interefeared with something
     */
    { "gorge",          do_gorge,       POS_STANDING,    0,  LOG_NORMAL },
    { "gouge",          do_gouge,      POS_FIGHTING, 0,      LOG_NORMAL },
    { "circle",         do_circle,     POS_FIGHTING, 0,      LOG_NORMAL },
    { "rake",           do_rake,       POS_FIGHTING, 0,      LOG_NORMAL },
    { "headbutt",	do_headbutt,	POS_FIGHTING,	 0,  LOG_NORMAL },
    { "transport",      do_transport,  POS_RESTING,      0,  LOG_NORMAL },
/*    { "immlist",	do_immlist,	POS_DEAD, 	 0,  LOG_NORMAL },*/

    /*
     * OLC 1.1b
     */
    { "aedit",		do_aedit,	POS_DEAD, L_IMP,  LOG_BUILD  },
    { "cedit",          do_cedit,       POS_DEAD, L_CON,  LOG_BUILD  },
    { "hedit",          do_hedit,       POS_DEAD, L_APP,  LOG_BUILD  },
    { "redit",		do_redit,	POS_DEAD, L_CON,  LOG_BUILD  },
    { "oedit",		do_oedit,	POS_DEAD, L_IMP,  LOG_BUILD  },
    { "medit",		do_medit,	POS_DEAD, L_IMP,  LOG_BUILD  },
    { "asave",          do_asave,	POS_DEAD, L_APP,  LOG_NORMAL },
    { "alist",		do_alist,	POS_DEAD, L_CON,  LOG_NORMAL },
    { "resets",		do_resets,	POS_DEAD, L_CON,  LOG_NORMAL },
    { "mreset",		do_mreset,	POS_DEAD, L_CON,  LOG_NORMAL },

    { "sedit",		do_sedit,	POS_DEAD, L_CON,  LOG_BUILD  },
    { "spedit",		do_spedit,	POS_DEAD, L_IMP,  LOG_ALWAYS },
    { "rename_obj",	do_rename_obj,	POS_DEAD, L_APP,  LOG_ALWAYS },
    { "race_edit",	do_race_edit,	POS_DEAD, L_IMP,  LOG_ALWAYS },
    { "nedit",		do_nedit,	POS_DEAD, L_CON,  LOG_BUILD  }, 
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
/*
const	struct	social_type	social_table [ ] =
{

    {   "dammit",
        "You curse like a drunken sailor for how the fates have treated you.",
        "$n slams $s weapon on the ground and yells, 'Dammit Dammit DAMMIT!'",
        "You throw your hands up in disgust and let the verbal barbs fly at $N!",
        "$n curses $N to eternity while throwing a fit.",
        "$n curses you out like never before, $e must be pretty ticked off.",
        "You curse your own stupidity! Bah!",
        "$n curses $mself!"
    },       

    {   "oral",
        "Answer: &YORAL SEX!! &PAnd lots of it!",
        "$n yells, &C'Answer: ORAL SEX!! And lots of it!'",
        "Answer: &YORAL SEX!! &PAnd lots of it!", 
        "$n yells, &C'Answer: ORAL SEX!! And lots of it!'",  
        "$n yells, &C'Answer: ORAL SEX!! And lots of it!'",         
        "You remind yourself of the answer, &YORAL SEX!",
        "$n yells, &C'Answer: ORAL SEX!! And lots of it!'"
    },

    {   "autostrip",
        "You drool in ecstasy as Mel Gibson strips naked in front of you!",
        "$n drools in ecstasy as Mel Gibson peels off all his clothes!",
        "You grab $N and start to rip $S clothes off!",
        "$n grabs $N and starts to rip $S clothes off!",
        "$n grabs you and attempts to rip off all your clothes!",
        "You suddenly feel adventurous and begin to rip your clothes off!",
        "$n moans as $e begins to take off all $s clothing..."
    },   

    {
        "pounce",
        "You crouch low to the ground, looking for someone to pounce on...",
        "$n pounces up and down like a mad man!",
        "You pounce onto $N's lap playfully.",
        "$n pounces onto $N's lap playfully.",
        "$n crouches low to the ground, looking for someone to pounce on...",
        "You attempt to pounce on yourself. Not too swift...",
        "$n attempts to pounce on $mself! Not too swift..."
    },

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
        "blah",
        "You say 'BLAH!'",
        "$n says 'BLAH!'",
        "You tell $M that everything is just blah.",
        "$n tells $N that everything is just blah.",
        "$n tells you that everything is just blah.",
        "You scream 'BLAH BLAH BLAH!'",
        "$n screams 'BLAH BLAH BLAH!'"
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
	"doh",
	"You say, 'Doh!!' and hit your forehead.  What an idiot you are!",
	"$n hits $mself in the forehead and says, 'Doh!!!'",
	"You say, 'Doh!!' and hit your forehead.  What an idiot you are!",
	"$n hits $mself in the forehead and says, 'Doh!!!'",
	"$n hits $mself in the forehead and says, 'Doh!!!'",
	"You hit yourself in the forehead and say, 'Doh!!!'",
	"$n hits $mself in the forehead and says, 'Doh!!!'"    
    },
*/    
    /*
     * This one's for Baka, Penn, and Onethumb!
     */
  /*  {
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
        "frag",
        "You say, 'FRAG FRAG FRAG FRAG!!!'",
        "$n says, 'FRAG FRAG FRAG FRAG!!!'",
        "You glare at $N and say, 'FRAG FRAG FRAG FRAG!!!'",
        "$n glares at $N and says, 'FRAG FRAG FRAG FRAG!!!'",
        "$n glares at you and says, 'FRAG FRAG FRAG FRAG!!!'",
        "You mutter angrily to yourself, 'FRAG FRAG FRAG FRAG!!!'",
        "$n mutters angrily to $mself, 'FRAG FRAG FRAG FRAG!!!'"
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
	"fruff",
	"You turn into a fruff monster and look for someone to fruff.",
	"$n turns into a fruff monster and looks around for a fruff victim.",
	"You fruff $N until $E starts to giggle out loud.",
	"$n fruffs you and you giggle out loud!",
	"You find no one and fruff yourself.",
	"Finding no one, $n fruffs $mself."
    },
    {
	"gasp",
	"You gasp in astonishment.",
	"$n gasps in astonishment.",
	"You gasp as you realize what $e did.",
	"$n gasps as $e realizes what $N did.",
	"$n gasps as $e realizes what you did.",
	"You look at yourself and gasp!",
	"$n takes one look at $mself and gasps in astonishment!"
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
	"$n grimaces is disgust.",
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
        "lapdance",
        "You look around for someone to lapdance.",
        "$n looks around for someone to lapdance.",
        "You lapdance $M making $M pop into a sweat!",
        "$n lapdances $N till they beg for mercy!",
        "$n lapdances you slowly to the beat of Latin music!",
        "You lapdance yourself.  Kinky!",
        "$n lapdances $mself. You wonder, 'Are they really that lonely?'",
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
        "liege",
        "You kneel down on the ground, looking for someone to liege.",
        "$n kneels to the ground, looking for someone to liege.",
        "You kneel before $M and say, 'My liege!'",
        "$n kneels before $N and says, 'My liege!'",
        "$n kneels before you and says, 'My liege!'",
        "You liege yourself, how silly!",
        "$n lieges $mself, how strange."
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
        "mock",
        "You mock nothing in general.",
        "$n mocks nothing in general.",
        "You look at $M, smile, and say 'I mock you.'",
        "$n mocks $N mercilessly.",
        "$n looks at you, smiles, and says 'I mock you.'",
        "You mock yourself alot.",
        "$n mocks $mself just cause $e can."
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
	"pet",
	"Pat whom?",
	NULL,
	"You pat $N on $S head.",
	"$n pats $N on $S head.",
	"$n pats you on your head.",
	"You pat yourself on your head.  Aren't you proud?",
	"$n pats $mself on the head.  $e is proud of $mself!"
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
        "pounce",
        "You crouch low to the ground, looking for someone to pounce on...",
        "$n pounces up and down like a mad man!",
        "You pounce onto $N's lap playfully.",
        "$n pounces onto $N's lap playfully.",
        "$n crouches low to the ground, looking for someone to pounce on...",
        "You attempt to pounce on yourself. Not too swift...",
        "$n attempts to pounce on $mself! Not too swift..."
    },

    {
	"ponder",
	"You ponder the question.",
	"$n sits down and thinks deeply.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
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
        "roflmao",
        "You roll on the floor, laughing your ass off!",
        "$n rolls on the floor, laughing $s ass off!",
        "You roll on the floor, laughing your ass off at $M!",
        "$n rolls on the floor, laughing $s ass off at $N!",
        "$n rolls on the floor, laughing $s ass off at YOU!",
        "You roll on the floor, laughing your ass off at yourself!",
        "$n rolls on the floor, laughing $s ass off at $mself...Strange."
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
	"shrug",
	"You shrug.",
	"$n shrugs helplessly.",
	"You shrug in response to $s question.",
	"$n shrugs in response to $N's question.",
	"$n shrugs in respopnse to your question.",
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
        "slap",
        "You look around for someone to slap.",
        "$n looks around for someone to slap.",
        "You slap $M.",
        "$n slaps $N.",
        "$n slaps you.",
        "You slap yourself across the face.",
        "$n slaps $mself across the face."
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
	"sneeze",
	"Gesundheit!",
	"$n sneezes.",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
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
	"",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL
    }
};

*/

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

    if ( ch->wait > 0 )
      return;

    /*
     * Strip leading spaces.
     */
    while ( isspace( *argument ) )
	argument++;
    if ( argument[0] == '\0' )
	return;

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_FREEZE ) )
    {
	send_to_char(AT_LBLUE, "You're totally frozen!\n\r", ch );
	return;
    }

    /*
     * Implement stun.
     */
    if ( IS_STUNNED( ch, STUN_COMMAND ) ||
	 IS_STUNNED( ch, STUN_TOTAL ) )
    {
      send_to_char(AT_LBLUE, "You're too stunned to do anything!\n\r", ch );
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
	    && can_use_cmd(cmd, ch, trust) )
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


	/*
	 * Builder Logs.
	 * Added by Altrag.
	 */
	if ( cmd_table[cmd].log == LOG_BUILD )
	{
	sprintf( log_buf, "%s: %s", ch->name, logline );
	log_string( log_buf, CHANNEL_BUILD , get_trust( ch ) );
	wiznet( log_buf, ch, NULL, WIZ_SECURE, 0, get_trust( ch ) );
	}

    if ( ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_LOG ) )
	|| fLogAll
	|| cmd_table[cmd].log == LOG_ALWAYS )
    {
      sprintf( log_buf, "%s: %s", ch->name, logline );
      log_string( log_buf, ( ch->level == L_IMP ? CHANNEL_NONE : CHANNEL_GOD ),
         ( ( ch->level > 100 ) ? get_trust( ch ) : -1 ) );
      wiznet( log_buf, ch, NULL, WIZ_SECURE, 0, get_trust( ch ) );
    }

    if ( ch->desc && ch->desc->snoop_by )
    {
        sprintf( log_buf, "%s%%", ch->name );
	write_to_buffer( ch->desc->snoop_by, log_buf,    0 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    /*
     *  Hiding stuff...  - Decklarean
     */
    if ( IS_AFFECTED(ch,AFF_HIDE) )
     {     /*Things you can do while hiding*/
	if (            str_cmp( cmd_table[cmd].name, "pick"     ) &&
			str_cmp( cmd_table[cmd].name, "chameleon power") &&
			str_cmp( cmd_table[cmd].name, "save"     ) &&
			str_cmp( cmd_table[cmd].name, "visible"  ) &&
			str_cmp( cmd_table[cmd].name, "inventory") &&
			str_cmp( cmd_table[cmd].name, "equipment") &&
			str_cmp( cmd_table[cmd].name, "where"    ) &&
			str_cmp( cmd_table[cmd].name, "who"      ) &&
			str_cmp( cmd_table[cmd].name, "open"     ) &&
			str_cmp( cmd_table[cmd].name, "hide"     ) &&
			str_cmp( cmd_table[cmd].name, "sleep"    ) &&
			str_cmp( cmd_table[cmd].name, "stand"    ) &&
			str_cmp( cmd_table[cmd].name, "rest"     ) &&
			str_cmp( cmd_table[cmd].name, "peek"     ) &&
			str_cmp( cmd_table[cmd].name, "shadow"   ) &&
			str_cmp( cmd_table[cmd].name, "sneak"    ) &&
			str_cmp( cmd_table[cmd].name, "worth"    ) &&
			str_cmp( cmd_table[cmd].name, "affected" ) &&
			str_cmp( cmd_table[cmd].name, "look"     ) &&
                        str_cmp( cmd_table[cmd].name, "tell"     ) &&
			str_cmp( cmd_table[cmd].name, "score"    )    )
        {
 /*If you are sneaking you can move these directions while hiding*/
 	   if ( IS_AFFECTED(ch,AFF_SNEAK) &&
		( !str_cmp( cmd_table[cmd].name, "north" ) ||
	          !str_cmp( cmd_table[cmd].name, "east"  ) ||
           	  !str_cmp( cmd_table[cmd].name, "south" ) ||
		  !str_cmp( cmd_table[cmd].name, "west"  ) ||
		  !str_cmp( cmd_table[cmd].name, "up"    ) ||
		  !str_cmp( cmd_table[cmd].name, "down"  )    ) )
	   { 
              /* check and see if they are chameleon and remove it if they
		 are trying to move*/
	      if ( is_affected(ch, gsn_chameleon) )
	      {
                 send_to_char(AT_BLUE, "You return to your normal coloration.\n\r", ch );
  	         affect_strip ( ch, gsn_chameleon );
              }
 	      /* If they aren't hidding remove the hide bit */	
              if ( !is_affected(ch, gsn_hide ) )
 	         REMOVE_BIT( ch->affected_by, AFF_HIDE );
           }
           else
           {  /* remove all the hidding stuff */
              if ( is_affected(ch, gsn_chameleon) )
	      {
                 send_to_char(AT_BLUE, "You return to your normal coloration.\n\r", ch );
                 affect_strip ( ch, gsn_chameleon);
              }
	      if ( is_affected(ch, gsn_hide) )
              {
		 send_to_char(AT_BLUE, "You stop hiding.\n\r", ch);
                 affect_strip ( ch, gsn_hide );
	      }
 	      REMOVE_BIT( ch->affected_by, AFF_HIDE );
	   }
	}
     }


    if ( IS_AFFECTED2(ch,AFF_BERSERK) && trust < LEVEL_IMMORTAL)
    {
      if ( !found || (str_cmp( cmd_table[cmd].name, "flee" ) &&
		      str_cmp( cmd_table[cmd].name, "look" ) &&
		      str_cmp( cmd_table[cmd].name, "get"  ) &&
		      str_cmp( cmd_table[cmd].name, "wield") &&
		      str_cmp( cmd_table[cmd].name, "dual" ) &&
		      str_cmp( cmd_table[cmd].name, "score")) )
      {
	send_to_char( AT_BLOOD, "You cannot do that in such a rage!\n\r",ch);
	return;
      }
    }

    if ( !found )
    {
	/*
	 * Look for command in socials table.
	 */
	if ( IS_NPC(ch) || !check_alias( ch, command, argument ) )
	if ( !check_social( ch, command, argument ) )
	    send_to_char(C_DEFAULT, "Huh?\n\r", ch );
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
	    send_to_char(AT_RED, "Lie still; you are DEAD.\n\r", ch );
	    break;

	case POS_MORTAL:
	case POS_INCAP:
	    send_to_char(AT_RED, "You are hurt far too bad for that.\n\r", ch );
	    break;

	case POS_STUNNED:
	    send_to_char(AT_WHITE, "You are too stunned to do that.\n\r",    ch );
	    break;

	case POS_SLEEPING:
	    send_to_char(AT_BLUE, "In your dreams, or what?\n\r",           ch );
	    break;

	case POS_RESTING:
	    send_to_char(AT_BLUE, "Nah... You feel too relaxed...\n\r",     ch );
	    break;

	case POS_FIGHTING:
	    send_to_char(AT_BLOOD, "No way!  You are still fighting!\n\r",   ch );
	    break;

	}
	return;
    }
/*
    if ( IS_NPC(ch) )
    {
      char combuf [MAX_STRING_LENGTH];
      char filname[MAX_INPUT_LENGTH];
      extern int port;

      sprintf(combuf, "%s: [%d] %s %s\n\r", ctime( &current_time), ch->pIndexData->vnum, command, argument);
      sprintf(filname, "comlog%d.txt", port );
      append_file( ch, filname, combuf );
    }
*/
    /*
     * Dispatch the command.
     */
    if ( IS_SET( ch->act, PLR_AFK ) )  /* if player was afk, remove afk */
       {
       REMOVE_BIT( ch->act, PLR_AFK );
       send_to_char( AT_YELLOW, "You are no longer AFK.\n\r", ch );
       }
    if ( !strcmp( cmd_table[cmd].name, "sing" )
    && prime_class(ch) != CLASS_BARD )
	{
	send_to_char( C_DEFAULT, "What do you think you are, a BARD???\n\r", ch );
	return;
	}
    else if ( !strcmp( cmd_table[cmd].name, "cast" )
	 && prime_class(ch) == CLASS_BARD )
	{
	send_to_char( C_DEFAULT, "You're a bard, SING!\n\r", ch );
	return;
	}

/* Here checks if ch is fighting in the arena **/

    if ( ch == arena.fch || ch == arena.sch ) 
       arena_master( ch, cmd_table[cmd].name, argument ); 

/** Do arena_master so we can look at arg and send info over channel then we come
    back and process command **/


    (*cmd_table[cmd].do_fun) ( ch, argument );
    tail_chain( );
    return;
}

bool can_use_cmd( int cmd, CHAR_DATA *ch, int trust )
{
  if ( trust <= 0 )
    trust = get_trust(ch);
  if ( !IS_NPC(ch) )
  {
    if ( is_name(NULL, cmd_table[cmd].name, ch->pcdata->empowerments) )
      return TRUE;
    if ( is_name(NULL, cmd_table[cmd].name, ch->pcdata->detractments) )
      return FALSE;
  }
  return (cmd_table[cmd].level <= trust);
}

bool check_alias( CHAR_DATA *ch, char *command, char *argument )
{
   ALIAS_DATA *al;
   char arg[MAX_STRING_LENGTH];
   char newarg[MAX_STRING_LENGTH];
   bool found;
   
   arg[0] = '\0';
   newarg[0] = '\0';
   
/*   if ( argument[0] != '\0' )
     argument = one_argument( argument, arg );*/
   while ( isspace(*argument) )
     argument++;
   strcpy( arg, argument );
   
   if ( IS_NPC( ch ) )
     return FALSE;
     
   if ( !(ch->pcdata->alias_list) )
     return FALSE;
   
   found = FALSE;

   for ( al = ch->pcdata->alias_list; al; al = al->next )
   {
     if ( !str_cmp( al->old, command ) )
     {
/*       if ( arg[0] != '\0' )
       {*/
         strcpy( newarg, al->new );
         strcat( newarg, " " );
         strcat( newarg, arg );
         interpret( ch, newarg );
/*       }
      else
       interpret( ch, al->new );*/
       
       found = TRUE;
       break;
     }
   }
   return found;
}


bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    
/*    int        cmd;*/
    bool       found;
    SOCIAL_DATA *pSocial;

    found  = FALSE;
/*
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == social_table[cmd].name[0]
	    && !str_prefix( command, social_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    } */

  for(pSocial = social_first;pSocial;pSocial = pSocial->next)
  {
    if ( command[0] == pSocial->name[0]
         && !str_prefix( command, pSocial->name) )
    {
	found = TRUE;
	break;
    }
  }

    if ( !found )
	return FALSE;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
    {
	send_to_char(AT_LBLUE, "You are anti-social!\n\r", ch );
	return TRUE;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_char(AT_RED, "Lie still; you are DEAD.\n\r",             ch );
	return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
	send_to_char(AT_RED, "You are hurt far too badly for that.\n\r", ch );
	return TRUE;

    case POS_STUNNED:
	send_to_char(AT_WHITE, "You are too stunned to do that.\n\r",      ch );
	return TRUE;

    case POS_SLEEPING:
	/*
	 * I just know this is the path to a 12" 'if' statement.  :(
	 * But two players asked for it already!  -- Furey
	 */
	if ( !str_cmp( pSocial->name, "snore" ) )
	    break;
	send_to_char(AT_BLUE, "In your dreams, or what?\n\r",             ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
	act(AT_PINK, pSocial->char_no_arg,   ch, NULL, victim, TO_CHAR);
	act(AT_PINK, pSocial->others_no_arg, ch, NULL, victim, TO_ROOM);
    }
    else if ( !( victim = get_char_room( ch, arg ) ) )
    {
	if ( !( victim = get_char_world( ch, arg ) ) )
		send_to_char(AT_WHITE, "They aren't here.\n\r",                    ch );
  	else {  
		if ((ch->in_room->vnum == ROOM_VNUM_HELL) &&
		   (ch->level < LEVEL_IMMORTAL))
		{
		   send_to_char(AT_WHITE, "What do you think this is, a vacation?\n\r" ,ch);
		}
		else
		{
		   sprintf( buf, "From a great distance, %s", pSocial->char_found);
		   act(AT_PINK, buf, ch, NULL, victim, TO_CHAR);
		   sprintf( buf, "From a great distance, %s", pSocial->vict_found);
		   act(AT_PINK, buf, ch, NULL, victim, TO_VICT);
		   sprintf( buf, "From a great distance, %s", pSocial->others_found );
		   act(AT_PINK, buf, ch, NULL, victim, TO_NOTVICT);
	   	}
		
	   }
	
    }
    else if ( victim == ch )
    {
	act(AT_PINK, pSocial->char_auto,     ch, NULL, victim, TO_CHAR);
	act(AT_PINK, pSocial->others_auto,   ch, NULL, victim, TO_ROOM);
    }
    else
    {
	act(AT_PINK, pSocial->char_found,    ch, NULL, victim, TO_CHAR);
	act(AT_PINK, pSocial->vict_found,    ch, NULL, victim, TO_VICT);
	act(AT_PINK, pSocial->others_found,  ch, NULL, victim, TO_NOTVICT);

	if ( !IS_NPC( ch )
	    && IS_NPC( victim )
	    && !IS_AFFECTED( victim, AFF_CHARM )
	    && IS_AWAKE( victim )
	    && ( !victim->pIndexData->mobprogs ) )
	    
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
		act(AT_PINK, pSocial->char_found,
		    victim, NULL, ch, TO_CHAR    );
		act(AT_PINK, pSocial->vict_found,
		    victim, NULL, ch, TO_VICT    );
		act(AT_PINK, pSocial->others_found,
		    victim, NULL, ch, TO_NOTVICT );
		break;

	    case 9: case 10: case 11: case 12:
		act(AT_PINK, "You slap $N.",  victim, NULL, ch, TO_CHAR    );
		act(AT_PINK, "$n slaps you.", victim, NULL, ch, TO_VICT    );
		act(AT_PINK, "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
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

    if ( *argument == '\'' ||
 *argument == '"' )
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

void do_clear( CHAR_DATA *ch, char *argument )
{
  ALIAS_DATA *pA1;
  ALIAS_DATA *pA1_next;

  if( !ch->pcdata->alias_list )
    {
      send_to_char( AT_WHITE, "You have no alias' to clear.\n\r", ch );
      return;
    }
   
   for ( pA1 = ch->pcdata->alias_list; pA1; pA1 = pA1_next )
    {
      pA1_next = pA1->next;
      /*free_string(pA1->old);
      free_string(pA1->new);
      free_mem(pA1, sizeof( *pA1 ));*/
      free_alias(pA1);
    }
   send_to_char( AT_WHITE, "All aliases have been erased.\n\r", ch );
   ch->pcdata->alias_list = NULL;
   return;

}

void do_alias( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_STRING_LENGTH];
  char arg1[MAX_STRING_LENGTH];
  ALIAS_DATA *al;
  ALIAS_DATA *oAl;
  
  smash_tilde( argument );
  argument = one_argument( argument, arg );
  strcpy( arg1, argument );
  
  if ( arg[0] == '\0' )
  {
    if ( !ch->pcdata->alias_list )
    {
      send_to_char( AT_WHITE, "You have no alias' set.\n\r", ch );
      return;
    }
    
    send_to_char( AT_WHITE, "Your currently set alias' are:\n\r", ch );
  
    for ( al = ch->pcdata->alias_list; al; al = al->next )
    {
      sprintf( log_buf, "  %-10s   &B%s&w\n\r", al->old, al->new );
      send_to_char( AT_RED, log_buf, ch );
    }
    send_to_char( AT_WHITE, "\n\r", ch );
    return;
  }
  
  if ( arg1[0] == '\0' && ( !( ch->pcdata->alias_list ) ) )
  {
    send_to_char( AT_WHITE, "You have no command alias' to remove!\n\r", ch );
    return;
  }
  
  if ( arg1[0] != '\0' )
  {
    for ( oAl = ch->pcdata->alias_list; oAl; oAl = oAl->next )
    {
      if ( is_name( NULL, arg1, oAl->old ) || !str_prefix( arg, arg1 ) )
      {
        send_to_char( AT_WHITE, "You cannot alias other alias' into your [new] string.\n\r", ch );
        return;
      }
    }
  }        
  
  for ( al = ch->pcdata->alias_list; al; al = al->next )
    if ( !str_cmp( al->old, arg ) )
      break;
  if ( arg1[0] == '\0' )
  {
    if ( !al )
    {
      send_to_char( C_DEFAULT, "Alias does not exist.\n\r", ch );
      return;
    }
    if ( al == ch->pcdata->alias_list )
      ch->pcdata->alias_list = al->next;
    else
    {
      for ( oAl = ch->pcdata->alias_list; oAl; oAl = oAl->next )
	if ( oAl->next == al )
	  break;
      if ( !oAl )
      {
	sprintf( log_buf, "Do_alias: bad alias - ch (%s) - [%s]", ch->name, arg );
	bug( log_buf, 0 );
	send_to_char(C_DEFAULT, "Alias does not exist.\n\r", ch );
	return;
      }
      oAl->next = al->next;
    }
    /*free_string( al->old );
    free_string( al->new );
    free_mem( al, sizeof( *al ) );*/
    free_alias( al );
    act( AT_WHITE, "Alias '$t' cleared.\n\r", ch, arg, NULL, TO_CHAR );
    return;
  }

  if ( al )
  {
    free_string( al->new );
    al->new = str_dup( arg1 );
    sprintf( log_buf, "Alias '%s' remade.\n\r", arg );
    send_to_char( AT_WHITE, log_buf, ch );  
    return;
  }
  
  /*
  al = alloc_perm( sizeof( *al ) );
  al->old = str_dup( arg );
  al->new = str_dup( arg1 );
  al->next = ch->pcdata->alias_list;
  ch->pcdata->alias_list = al;
  */
  add_alias( ch, al, arg, arg1 );
  sprintf( log_buf, "Alias '%s' added.\n\r", arg );
  send_to_char( AT_WHITE, log_buf, ch );
  return;
}

void add_alias( CHAR_DATA *ch, ALIAS_DATA *pAl, char *old, char *new )
{
  pAl = alloc_perm( sizeof( *pAl ) );
  pAl->old = str_dup( old );
  pAl->new = str_dup( new );
  pAl->next = ch->pcdata->alias_list;  ch->pcdata->alias_list = pAl;
  return;
}

void do_countcommands( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  int scnt;
  int skill[MAX_CLASS];
  int spell[MAX_CLASS];
  int iclass;

  for ( iclass = 0; iclass < MAX_CLASS; iclass++ )
  { 
     skill[iclass] = 0;
     spell[iclass] = 0;
  }

  for ( scnt = 0; skill_table[scnt].name[0] != '\0'; scnt++ )
  {
    for ( iclass = 0; iclass < MAX_CLASS; iclass++ )
    {
       if ( skill_table[scnt].skill_level[iclass] < L_APP )
       {
	  if ( skill_table[scnt].spell_fun == spell_null )
	    skill[iclass]++;
	  else
	    spell[iclass]++;
       }
    }
  }

  sprintf(buf, "Command table size: %d\n\r"
	       "Social table size:  %d\n\r"
	       "GSkill table size:  %d\n\r"
	       "Skill table size:   %d\n\r",
	  /* -1 from each for blank index entry at end. */
	  sizeof(   cmd_table) / sizeof(   cmd_table[0]) - 1,
	  top_social,
/*sizeof(social_table) / sizeof(social_table[0]) -1,*/
	  sizeof(gskill_table) / sizeof(gskill_table[0])    ,
	  scnt); /* Someone wanna explain why theres an error with
		    sizeof(skill_table)?? */
  send_to_char(AT_PINK, buf, ch );
  for ( iclass = 0; iclass < MAX_CLASS; iclass++ )
  {
    sprintf( buf, "%-15s: Skills %2d + Spells %2d = Total %2d\n\r",
		 class_table[iclass].who_long,
		 skill[iclass], spell[iclass], skill[iclass]+spell[iclass] );
    send_to_char(AT_PINK, buf, ch );
  }
  return;
}
void do_afk( CHAR_DATA *ch, char *argument )
{
   if ( IS_NPC( ch ) )
      return;
   send_to_char( AT_YELLOW, "You announce that you are going AFK.\n\r",
		 ch );
   act( AT_YELLOW, "$n announces, 'I am going AFK.'", ch, NULL, NULL,
        TO_ROOM );
   SET_BIT( ch->act, PLR_AFK );
   return;
}

void do_pkill( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if(argument[0] == '\0' )
    {
      send_to_char( AT_WHITE,
        "Syntax: &Rpkill yes&W    To choose to become a PKILLER forever!\n\r", ch );
      send_to_char( AT_WHITE, "This is NOT a reversible choice. Type HELP PK.\n\r", ch );
      return;
    }
    if (!str_cmp( argument, "yes" ) )
    {
      if ( ch->level < 20 )
      {
        send_to_char( AT_WHITE, "You must be at least level 20 to choose the path of PKILL.\n\r", ch );
        return;
      }
      if ( IS_SET( ch->act, PLR_PKILLER ) )
      {
        send_to_char( AT_WHITE, "You have already chosen the path of pkilling.\n\r", ch );
        return;
      }
      send_to_char( AT_RED, 
       "You are now a PKILLER! Beware, the path you have chosen is not one of the weak minded.\n\r", ch);
      act( AT_RED, 
       "$n has made a lifelong decision, to join forces with Pkillers throughout the Storm!",
       ch , NULL, NULL, TO_ROOM );
      SET_BIT( ch->act, PLR_PKILLER );
      do_help( ch, "pk" );
      return;
    }
    else
    {
      send_to_char( AT_WHITE,
        "Syntax: &Rpkill yes&W    To choose to become a PKILLER forever!\n\r", ch );
      send_to_char( AT_WHITE, "This is NOT a reversible choice. Type HELP PK.\n\r", ch );
      return;
    }

}

void arena_master( CHAR_DATA *ch, char *argument, char *arg2 )
{
    char	arg  [ MAX_STRING_LENGTH ];
/*    char	buf  [ MAX_STRING_LENGTH ];  */
    bool	same_room = FALSE;

    one_argument( arg2, arg );

    /* Check if two people are still in arena fighting */
    if ( !arena.fch || !arena.sch )
      return;

    /* Check if fch and sch are in same room */
    if ( arena.fch->in_room == arena.sch->in_room )
      same_room = TRUE;

/*    if ( !str_cmp( "cast", argument ) )
    {

    }

    arena_chann( buf2, 0, 0 ); */

    return;
}
