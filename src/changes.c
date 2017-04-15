/***************************************************************************
 *  file: changes.c , Implementation of new commands.      Part of DIKUMUD *
 *  Usage : New commands while new datastructures are developed.           *
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
#include <ctype.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "utils.h"
#include "interp.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"

/*   external vars  */

extern struct room_data *world;
extern struct descriptor_data *descriptor_list;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct int_app_type int_app[26];
extern bool wizlock;

/* external functs */

void set_title(struct char_data *ch);
int str_cmp(char *arg1, char *arg2);
char *skip_spaces(char *string);
struct time_info_data age(struct char_data *ch);
int mana_limit(struct char_data *ch);
int hit_limit(struct char_data *ch);
int move_limit(struct char_data *ch);
int mana_gain(struct char_data *ch);
int hit_gain(struct char_data *ch);
int move_gain(struct char_data *ch);

/* To be moved to moved to act.wizard.c */

void do_noemote(struct char_data *ch, char *argument, int cmd)
{
    struct char_data *vict;
    struct obj_data *dummy;
    char buf[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
	return;

    one_argument(argument, buf);

    if (!*buf)
	send_to_char("Noemote who?\n\t", ch);

    else if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
	send_to_char("Couldn't find any such creature.\n\r", ch);
    else if (IS_NPC(vict))
	send_to_char("Can't do that to a beast.\n\r", ch);
    else if (GET_LEVEL(vict) >= GET_LEVEL(ch))
	act("$E might object to that ... better not.",
	0, ch, 0, vict, TO_CHAR);
    else if (IS_SET(vict->specials.act, PLR_NOEMOTE))
    {
	send_to_char("You can emote again.\n\r", vict);
	send_to_char("NOEMOTE removed.\n\r", ch);
	REMOVE_BIT(vict->specials.act, PLR_NOEMOTE);
    }
    else
    {
	send_to_char("The gods take away your ability to emote!\n\r", vict);
	send_to_char("NOEMOTE set.\n\r", ch);
	SET_BIT(vict->specials.act, PLR_NOEMOTE);
    }
}


void do_notell(struct char_data *ch, char *argument, int cmd)
{
    struct char_data *vict;
    struct obj_data *dummy;
    char buf[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
	return;

    one_argument(argument, buf);

    if (!*buf)
	if (IS_SET(ch->specials.act, PLR_NOTELL))
	{
	    send_to_char("You can now hear tells again.\n\r", ch);
	    REMOVE_BIT(ch->specials.act, PLR_NOTELL);
	}
	else
	{
	    send_to_char("From now on, you can't use tell.\n\r", ch);
	    SET_BIT(ch->specials.act, PLR_NOTELL);
	}
    else if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
	send_to_char("Couldn't find any such creature.\n\r", ch);
    else if (IS_NPC(vict))
	send_to_char("Can't do that to a beast.\n\r", ch);
    else if (GET_LEVEL(vict) >= GET_LEVEL(ch))
	act("$E might object to that.. better not.", 0, ch, 0, vict, TO_CHAR);
    else if (IS_SET(vict->specials.act, PLR_NOTELL))
    {
	send_to_char("You can use telepatic communication again.\n\r", vict);
	send_to_char("NOTELL removed.\n\r", ch);
	REMOVE_BIT(vict->specials.act, PLR_NOTELL);
    }
    else
    {
	send_to_char(
	"The gods take away your ability to use telepatic communication!\n\r",
	    vict);
	send_to_char("NOTELL set.\n\r", ch);
	SET_BIT(vict->specials.act, PLR_NOTELL);
    }
}


void do_freeze(struct char_data *ch, char *argument, int cmd)
{
    struct char_data *vict;
    struct obj_data *dummy;
    char buf[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
	return;

    one_argument(argument, buf);

    if (!*buf)
	send_to_char("Freeze who?\n\r", ch);

    else if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
	send_to_char("Couldn't find any such creature.\n\r", ch);
    else if (IS_NPC(vict))
	send_to_char("Can't do that to a beast.\n\r", ch);
    else if (GET_LEVEL(vict) >= GET_LEVEL(ch))
	act("$E might object to that.. better not.", 0, ch, 0, vict, TO_CHAR);
    else if (IS_SET(vict->specials.act, PLR_FREEZE))
    {
	send_to_char("You now can do things again.\n\r", vict);
	send_to_char("FREEZE removed.\n\r", ch);
	REMOVE_BIT(vict->specials.act, PLR_FREEZE);
    }
    else
    {
	send_to_char("The gods take away your ability to ...\n\r", vict);
	send_to_char("FREEZE set.\n\r", ch);
	SET_BIT(vict->specials.act, PLR_FREEZE);
    }
}


void do_log(struct char_data *ch, char *argument, int cmd)
{
    struct char_data *vict;
    struct obj_data *dummy;
    char buf[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
	return;

    one_argument(argument, buf);

    if (!*buf)
	send_to_char("Log who?\n\r", ch);

    else if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
	send_to_char("Couldn't find any such creature.\n\r", ch);
    else if (IS_NPC(vict))
	send_to_char("Can't do that to a beast.\n\r", ch);
    else if (GET_LEVEL(vict) >= GET_LEVEL(ch))
	act("$E might object to that.. better not.", 0, ch, 0, vict, TO_CHAR);
    else if (IS_SET(vict->specials.act, PLR_LOG))
    {
	send_to_char("LOG removed.\n\r", ch);
	REMOVE_BIT(vict->specials.act, PLR_LOG);
    }
    else
    {
	send_to_char("LOG set.\n\r", ch);
	SET_BIT(vict->specials.act, PLR_LOG);
    }
}

void do_wizlock(struct char_data *ch, char *argument, int cmd)
{
    wizlock = !wizlock;

    if ( wizlock ) {
	sprintf(log_buf,"Game has been wizlocked by %s.",GET_NAME(ch));
	log(log_buf);
	send_to_char("Game wizlocked.\n\r", ch);
    } else {
	sprintf(log_buf,"Game has been un-wizlocked by %s.",GET_NAME(ch));
	log(log_buf);
	send_to_char("Game un-wizlocked.\n\r", ch);
    }
}




/* This routine is used by 34+ level ONLY to set 
   specific char/npc variables, including skills */

void do_set(struct char_data *ch, char *argument, int cmd)
{
    /* from spell_parser.c */
    char *spells[]= {
	"armor","teleport","bless","blindness","burning hands","call lightning",
	"charm person","chill touch","clone","colour spray","control weather",
	"create food","create water","cure blind","cure critic","cure light",
	"curse","detect evil","detect invisibility","detect magic",
	"detect poison","dispel evil","earthquake","enchant weapon",
	"energy drain","fireball","harm","heal","invisibility",
	"lightning bolt","locate object","magic missile","poison",
	"protection from evil","remove curse","sanctuary","shocking grasp",
	"sleep","strength","summon","ventriloquate","word of recall",
	"remove poison","sense life",
	"sneak","hide","steal","backstab","pick lock",
	"kick","bash","rescue","\n"
    };
    char *values[] = {
	"age","sex","class","level","height","weight","str","stradd",
	"int","wis","dex","con","gold","exp","mana","hit","move",
	"sessions","alignment","thirst","drunk","full","\n"
    };
    struct char_data *vict;
    char name[100], buf2[100], buf[100], help[MAX_STRING_LENGTH];
    int skill, value, i, qend;

    argument = one_argument(argument, name);
    if (!*name) /* no arguments. print an informative text */
    {
	send_to_char(
	    "Syntax:\n\rset <name> skill '<skill>' <value> <value>\n\r", ch);
	send_to_char(
	    "or:\n\rset <name> value <field> <value>\n\r", ch);

	strcpy(help, "Skill being one of the following:\n\r");
	for (i = 1; *spells[i] != '\n'; i++)
	{
	    sprintf(help + strlen(help), "%18s", spells[i]);
	    if (!(i % 4))
	    {
		strcat(help, "\n\r");
		send_to_char(help, ch);
		*help = '\0';
	    }
	}
	if (*help)
	    send_to_char(help, ch);

	strcpy(help, "\n\rField being one of the following:\n\r");
	for (i = 1; *values[i] != '\n'; i++)
	{
	    sprintf(help + strlen(help), "%18s", values[i]);
	    if (!(i % 4))
	    {
		strcat(help, "\n\r");
		send_to_char(help, ch);
		*help = '\0';
	    }
	}
	if (*help)
	    send_to_char(help, ch);
	send_to_char("\n\r", ch);
	return;
    }
    if (!(vict = get_char_vis(ch, name)))
    {
	send_to_char("No living thing by that name.\n\r", ch);
	return;
    }
    argument = one_argument(argument, buf);
    if (!*buf)
    {
	send_to_char("'skill' or 'value' expected.\n\r", ch);
	return;
    }
    if (str_cmp(buf,"skill") && str_cmp(buf,"s")
    && str_cmp(buf,"value") && str_cmp(buf,"v"))
    {
	send_to_char("'skill' or 'value' expected.\n\r", ch);
	return;
    }
    if (!str_cmp(buf,"skill") || !str_cmp(buf,"s"))
    /* This is a skill */
    {
	argument = skip_spaces(argument);

	/* If there is no chars in argument */
	if (!(*argument)) {
	    send_to_char("Skill name expected.\n\r", ch);
	    return;
	}

	if (*argument != '\'') {
	    send_to_char("Skill must be enclosed in: ''\n\r",ch);
	    return;
	}

	/* Locate the last quote && lowercase the magic words (if any) */

	for (qend=1; *(argument+qend) && (*(argument+qend) != '\'') ; qend++)
	    *(argument+qend) = LOWER(*(argument+qend));

	if (*(argument+qend) != '\'') {
	    send_to_char("Skill must be enclosed in: ''\n\r",ch);
	    return;
	}

	if ((skill = old_search_block(argument, 1, qend-1, spells, 0)) < 0)
	{
	    send_to_char("Unrecognized skill.\n\r", ch);
	    return;
	}
	skill--;
	argument += qend+1; /* skip to next parameter */
	argument = one_argument(argument,buf);
	if (!*buf)
	{
	    send_to_char("Learned value expected.\n\r", ch);
	    return;
	}
	value = atoi(buf);
	if (value < 0)
	{
	    send_to_char("Minimum value for learned is 0.\n\r", ch);
	    return;
	}
	if (value > 100)
	{
	    send_to_char("Max value for learned is 100.\n\r", ch);
	    return;
	}
	argument = one_argument(argument,buf);
	if (!*buf)
	{
	    send_to_char("Learned value expected.\n\r", ch);
	    return;
	}
	if (str_cmp(buf,"y") && str_cmp(buf,"n"))
	{
	    send_to_char("Recognice value must be 'y' or 'n'", ch);
	    return;
	}
	sprintf(buf2,
	    "%s changes %s's %s to %d,%s.",GET_NAME(ch),GET_NAME(vict),
	    spells[skill],value,buf);
	vict->skills[skill].learned = value;
	vict->skills[skill].recognise = (!str_cmp(buf,"y"));
    } else {
	/* it is another value */
	argument = one_argument(argument,buf);
	if (!*buf)
	{
	    send_to_char("Field name expected.\n\r", ch);
	    return;
	}
	if ((skill = old_search_block(buf, 0, strlen(buf), values, 1)) < 0)
	{
	    send_to_char(
		"No such field is known. Try 'set' for list.\n\r", ch);
	    return;
	}
	skill--;
	argument = one_argument(argument,buf);
	if (!*buf)
	{
	    send_to_char("Value for field expected.\n\r", ch);
	    return;
	}
	sprintf(buf2,
		"%s sets %s's %s to %s.",
		GET_NAME(ch),GET_NAME(vict),values[skill],buf);
	switch (skill) {
	    case 0: /* age */
	    {
		value = atoi(buf);
		if ((value < 16) || (value > 79))
		{
		    send_to_char("Age must be more than 16 years\n\r", ch);
		    send_to_char("and less than 80 years.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set age of victim */
		vict->player.time.birth = 
		    time(0) - (long)value*(long)SECS_PER_MUD_YEAR;
	    };
	    break;
	    case 1: /* sex */
	    {
		if (str_cmp(buf,"m") && str_cmp(buf,"f") && str_cmp(buf,"n"))
		{
		    send_to_char("Sex must be 'm','f' or 'n'.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set sex of victim */
		switch(*buf) {
		    case 'm':vict->player.sex = SEX_MALE;   break;
		    case 'f':vict->player.sex = SEX_FEMALE; break;
		    case 'n':vict->player.sex = SEX_NEUTRAL;break;
		}
	    }
	    break;
	    case 2: /* class */
	    {
		if (str_cmp(buf,"m") && str_cmp(buf,"c") && 
		    str_cmp(buf,"w") && str_cmp(buf,"t"))
		{
		    send_to_char("Class must be 'm','c','w' or 't'.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set class of victim */
		switch(*buf) {
		    case 'm':vict->player.class = CLASS_MAGIC_USER; break;
		    case 'c':vict->player.class = CLASS_CLERIC;     break;
		    case 'w':vict->player.class = CLASS_WARRIOR;    break;
		    case 't':vict->player.class = CLASS_THIEF;      break;
		}
	    }
	    break;
	    case 3: /* level */
	    {
		value = atoi(buf);
		if ((value < 0) || (value > 31))
		{
		    send_to_char(
			"Level must be between 0 and 31.\n\r", ch );
		    return;
		}
		log(buf2);
		/* set level of victim */
		vict->player.level = value;
	    }
	    break;
	    case 4: /* height */
	    {
		value = atoi(buf);
		if ((value < 100) || (value > 250))
		{
		    send_to_char("Height must be more than 100 cm\n\r", ch);
		    send_to_char("and less than 251 cm.\n\r", ch); 
		    return;
		}
		log(buf2);
		/* set hieght of victim */
		vict->player.height = value;
	    }       
	    break;
	    case 5: /* weight */
	    {
		value = atoi(buf);
		if ((value < 100) || (value > 250))
		{
		    send_to_char("Weight must be more than 100 pound\n\r", ch);
		    send_to_char("and less than 251 pounds.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set weight of victim */
		vict->player.weight = value;
	    }
	    break;
	    case 6: /* str */
	    {
		value = atoi(buf);
		if ((value <= 0) || (value > 18))
		{
		    send_to_char("Strength must be more than 0\n\r", ch);
		    send_to_char("and less than 19.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original strength of victim */
		vict->tmpabilities.str = value;
		vict->abilities.str = value;
	    }
	    break;
	    case 7: /* stradd */
	    {
		send_to_char( "Strength addition not supported.\n\r", ch );
	    }
	    break;
	    case 8: /* int */
	    {
		value = atoi(buf);
		if ((value <= 0) || (value > 18))
		{
		    send_to_char("Intelligence must be more than 0\n\r", ch);
		    send_to_char("and less than 19.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original INT of victim */
		vict->tmpabilities.intel = value;
		vict->abilities.intel = value;
	    }
	    break;
	    case 9: /* wis */
	    {
		value = atoi(buf);
		if ((value <= 0) || (value > 18))
		{
		    send_to_char("Wisdom must be more than 0\n\r", ch);
		    send_to_char("and less than 19.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original WIS of victim */
		vict->tmpabilities.wis = value;
		vict->abilities.wis = value;
	    }
	    break;
	    case 10: /* dex */
	    {
		value = atoi(buf);
		if ((value <= 0) || (value > 18))
		{
		    send_to_char("Dexterity must be more than 0\n\r", ch);
		    send_to_char("and less than 19.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original DEX of victim */
		vict->tmpabilities.dex = value;
		vict->abilities.dex = value;
	    }
	    break;
	    case 11: /* con */
	    {
		value = atoi(buf);
		if ((value <= 0) || (value > 18))
		{
		    send_to_char("Constitution must be more than 0\n\r", ch);
		    send_to_char("and less than 19.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original CON of victim */
		vict->tmpabilities.con = value;
		vict->abilities.con = value;
	    }
	    break;
	    case 12: /* gold */
	    {
		value = atoi(buf);
		log(buf2);
		/* set original gold of victim */
		vict->points.gold = value;
	    }
	    break;
	    case 13: /* exp */
	    {
		value = atoi(buf);
		if ((value <= 0) || (value > 90000000))
		{
		    send_to_char(
			"Experience-points must be more than 0\n\r", ch);
		    send_to_char("and less than 90000001.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original exp of victim */
		vict->points.exp = value;
	    }
	    break;
	    case 14: /* mana */
	    {
		value = atoi(buf);
		if ((value <= -100) || (value > 9994))
		{
		    send_to_char("Mana-points must be more than -100\n\r", ch);
		    send_to_char("and less than 9995.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original mana of victim */
		vict->points.max_mana = value;
	    }
	    break;
	    case 15: /* hit */
	    {
		value = atoi(buf);
		if ((value <= -10) || (value > 30000))
		{
		    send_to_char("Hit-points must be more than -10\n\r", ch);
		    send_to_char("and less than 30001.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original hit of victim */
		vict->points.max_hit = value;
	    }
	    break;
	    case 16: /* move */
	    {
		value = atoi(buf);
		if ((value <= -100) || (value > 9917))
		{
		    send_to_char("Move-points must be more than -100\n\r", ch);
		    send_to_char("and less than 9918.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original move of victim */
		vict->points.max_move = value;
	    }
	    break;
	    case 17: /* sessions */
	    {
		value = atoi(buf);
		if ((value < 0) || (value > 100))
		{
		    send_to_char("Sessions must be more than 0\n\r", ch);
		    send_to_char("and less than 101.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original sessions of victim */
		vict->specials.practices = value;
	    }
	    break;
	    case 18: /* alignment */
	    {
		value = atoi(buf);
		if ((value < -1000) || (value > 1000))
		{
		    send_to_char("Alignment must be more than -1000\n\r", ch);
		    send_to_char("and less than 1000.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original alignment of victim */
		vict->specials.alignment = value;
	    }
	    break;
	    case 19: /* thirst */
	    {
		value = atoi(buf);
		if ((value < -1) || (value > 100))
		{
		    send_to_char("Thirst must be more than -2\n\r", ch);
		    send_to_char("and less than 101.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original thirst of victim */
		vict->specials.conditions[THIRST] = value;
	    }
	    break;
	    case 20: /* drunk */
	    {
		value = atoi(buf);
		if ((value < -1) || (value > 100))
		{
		    send_to_char("Drunk must be more than -2\n\r", ch);
		    send_to_char("and less than 101.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original drunk of victim */
		vict->specials.conditions[DRUNK] = value;
	    }
	    break;
	    case 21: /* full */
	    {
		value = atoi(buf);
		if ((value < -1) || (value > 100))
		{
		    send_to_char("Full must be more than -2\n\r", ch);
		    send_to_char("and less than 101.\n\r", ch);
		    return;
		}
		log(buf2);
		/* set original full of victim */
		vict->specials.conditions[FULL] = value;
	    }
	    break;
	}
    }
    send_to_char("Ok.\n\r", ch);
}

void do_wiz(struct char_data *ch, char *argument, int cmd)
{
    char buf1[MAX_STRING_LENGTH];
    struct descriptor_data *i;

    for (; *argument == ' '; argument++);

    if (!(*argument))
	send_to_char(
	    "What do you want to tell all gods and immortals?\n\r", ch);
    else
    {
	sprintf(buf1, "$n: %s", argument);
	act(buf1, FALSE, ch, 0, 0, TO_CHAR);

	for (i = descriptor_list; i; i = i->next)
	if (i->character != ch && !i->connected && 
		GET_LEVEL(i->character) > 30)
		act(buf1, FALSE, ch, 0, i->character, TO_VICT);
    }
}

void do_tap(struct char_data *ch, char *argument, int cmd)
{
    struct obj_data *obj;
    char name[100];
    char buf[MAX_INPUT_LENGTH];
    int xp;

    one_argument (argument, name);

    if (!*name || !str_cmp(name, GET_NAME(ch)) )
    {
	act( "$n offers $mself to $m god, who graciously declines.",
	    FALSE, ch, 0, 0, TO_ROOM);
	act( "Your god appreciates your offer and may accept it later.",
	    FALSE, ch, 0, 0, TO_CHAR);
	return;
    }

    obj = get_obj_in_list_vis( ch, name, world[ch->in_room].contents );
    if ( obj == NULL )
    {
	act( "You can't find that object.",
	    FALSE, ch, 0, 0, TO_CHAR);
	return;
    }

    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	act( "$p is not an acceptable sacrifice.",
	    FALSE, ch, obj, 0, TO_CHAR );
	return;
    }

    if ( GET_ITEM_TYPE(obj) != ITEM_CONTAINER || obj->obj_flags.value[3] != 1 )
    {
	act( "$n sacrifices $p to $s god.", FALSE, ch, obj, 0, TO_ROOM );
	act( "You get one gold coin for your sacrifice.",
	    FALSE, ch, obj, 0, TO_CHAR );
	GET_GOLD(ch) += 1;
	extract_obj(obj);
    }
    else if ( obj->obj_flags.cost_per_day != 100000 )
    {
	act( "Such a sacrifice would be very unwise.",
	    FALSE, ch, obj, 0, TO_CHAR );
	return;
    }
    else
    {
	xp = 10 * GET_LEVEL(ch);
	sprintf( buf,
	    "You get %d experience points for your sacrifice.", xp );
	act( "$n sacrifices $p to $s god.", FALSE, ch, obj, 0, TO_ROOM);
	act( buf, FALSE, ch, obj, 0, TO_CHAR );
	gain_exp(ch, xp);
	extract_obj(obj);
    }
}

void do_trip(struct char_data *ch, char *argument, int cmd)
{
    struct char_data *victim;
    char name[256];
    byte percent;

    /* Mobs auto-trip in fight.c */
    if ( IS_NPC(ch) )
        return;

    if ((GET_CLASS(ch) != CLASS_THIEF) && GET_LEVEL(ch)<33){
        send_to_char("You better leave all the agile fighting to thieves.\n\r",
		     ch);
        return;
    }

    one_argument(argument, name);

    victim = get_char_room_vis( ch, name );
    if ( victim == NULL )
        victim = ch->specials.fighting;

    if ( victim == NULL )
    {
        send_to_char( "Trip whom?\n\r", ch );
        return;
    }

    if (victim == ch) {
        send_to_char("Aren't we funny today...\n\r", ch);
        return;
    }

     /* 101% is a complete failure */
    percent=number(1,101) + (GET_LEVEL(victim) - GET_LEVEL(ch));

    if (percent > ch->skills[SKILL_TRIP].learned * 2 / 3 ) {
        damage(ch, victim, 0, SKILL_TRIP);
    } else {
      act( "$n trips you and you go down!",
	  FALSE, ch, NULL, victim, TO_VICT );
      act( "You trip $N and $N goes down!",
	  FALSE, ch, NULL, victim, TO_CHAR );
      act( "$n trips $N and $N goes down!",
	  FALSE, ch, NULL, victim, TO_NOTVICT );
      damage(ch, victim, 1, SKILL_TRIP);
      WAIT_STATE(ch, PULSE_VIOLENCE*2);
      WAIT_STATE(victim, PULSE_VIOLENCE*3);
      GET_POS(victim) = POSITION_SITTING;
    }

}

void do_disarm( struct char_data *ch, char *argument, int cmd )
{
    struct char_data *victim;
    char name[256];
    int percent;
    struct obj_data *obj;

    /* Mobs auto-disarm in fight.c */
    if ( IS_NPC(ch) )
	return;

    if ( GET_CLASS(ch) != CLASS_WARRIOR && GET_CLASS(ch) != CLASS_THIEF
    && GET_LEVEL(ch) < 33 )
    {
	send_to_char( "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if ( ch->equipment[WIELD] == NULL )
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    one_argument( argument, name );
    victim = get_char_room_vis( ch, name );
    if ( victim == NULL )
	victim = ch->specials.fighting;

    if ( victim == NULL )
    {
	send_to_char( "Disarm whom?\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	act( "$n disarms $mself!", FALSE, ch, NULL, victim, TO_NOTVICT );
	send_to_char( "You disarm yourself!\n\r", ch );
	obj = unequip_char( ch, WIELD );
	obj_to_room( obj, ch->in_room );
	return;
    }

    if ( victim->equipment[WIELD] == NULL )
    {
	send_to_char( "Your opponent is not wielding a weapon!\n\r", ch );
	return;
    }

    percent = number( 1, 100 ) + GET_LEVEL(victim) - GET_LEVEL(ch);
    if ( percent < ch->skills[SKILL_DISARM].learned * 2 / 3 )
    {
	disarm( ch, victim );
	one_hit( victim, ch, TYPE_UNDEFINED );
	WAIT_STATE( ch, 3 * PULSE_VIOLENCE );
    }
    else
    {
	one_hit( victim, ch, TYPE_UNDEFINED );
	WAIT_STATE( ch, PULSE_VIOLENCE*2 );
    }
}

void do_title(struct char_data *ch, char *argument, int cmd)
{
  char buf[100];

  if (!*argument){
    send_to_char("Change your title to what?\n\r", ch);
    return;
  }
    
  for (; isspace(*argument); argument++);

  if (strlen(argument)>40){
    send_to_char("Title field too big.  40 characters max.\n\r", ch);
    return;
  }
  
  ch->player.title = str_dup(argument);
  sprintf(buf,"Your title is now: %s\n\r", argument);
  send_to_char(buf,ch);
  
}

void do_split(struct char_data *ch, char *argument, int cmd)
{
  int amount;
  char buf[256], number[10];
  int no_members, share, extra;
  struct char_data *k;
  struct follow_type *f;


  if (!*argument){
    send_to_char("Split what?\n\r", ch);
    return;
  }

  one_argument (argument, number);
  
  if (strlen(number)>7){
    send_to_char("Number field too big.\n\r", ch);
    return;
  }

  amount = atoi(number);

  if ( amount < 0 )
  {
    send_to_char( "Your group wouldn't like that!\n\r", ch );
    return;
  }

  if ( amount == 0 )
  {
    send_to_char(
    "You hand out zero coins to everyone, but no one notices.\n\r", ch );
    return;
  }

  if (GET_GOLD(ch)<amount)
  {
    send_to_char( "You don't have that much gold!\n\r", ch );
    return;
  }
  
  if (ch->master)
    k = ch->master;
  else
    k = ch;

  if ( !IS_AFFECTED(k, AFF_GROUP) || k->in_room != ch->in_room )
  {
    send_to_char("You must be grouped to split your money!\n\r", ch);
    return;
  }

  no_members = 1;
  for (f=k->followers; f; f=f->next)
  {
    if (IS_AFFECTED(f->follower, AFF_GROUP) &&
    (f->follower->in_room == ch->in_room))
      no_members++;
  }

  share = amount / no_members;
  extra = amount % no_members;
  GET_GOLD(ch) -= amount;

  if ( k==ch )
  {
      sprintf( buf, "You split %d gold coins.  ", amount );
      extra = amount - ( no_members * share );
  }
  else 
  {
      sprintf( buf, "%s splits %d gold coins.  ", GET_NAME(ch), amount );
  }
  send_to_char( buf, k );
  sprintf( buf, "Your share is %d gold coins.\n\r", share + extra );
  send_to_char( buf, k );
  GET_GOLD(k) += share + extra;

  for (f=k->followers; f; f=f->next) {
    if (IS_AFFECTED(f->follower, AFF_GROUP) &&
    (f->follower->in_room == ch->in_room)) {
      if (f->follower==ch)
	sprintf( buf, "You split %d gold coins.  ", amount );
      else 
	sprintf( buf, "%s splits %d gold coins.  ", GET_NAME(ch), amount );
      send_to_char( buf, f->follower );
      sprintf( buf, "Your share is %d gold coins.\n\r", share );
      send_to_char( buf, f->follower );
      GET_GOLD(f->follower) += share;
      } 
  }
}

void do_grouptell(struct char_data *ch, char *argument, int cmd)
{
  char buf[256];
  struct char_data *k;
  struct follow_type *f;

  if (!*argument){
    send_to_char("Tell your group what?\n\r", ch);
    return;
  }

  for (; isspace(*argument); argument++);

  if (IS_SET(ch->specials.act, PLR_NOTELL))
    {
      send_to_char("Your message didn't get through!!\n\r", ch);
      return;
    }

  if (!IS_AFFECTED(ch, AFF_GROUP))
    {
      send_to_char("You don't have a group to talk to!\n\r", ch);
      return;
    }

  if (!(k=ch->master))
    k = ch;

  if (IS_AFFECTED(k, AFF_GROUP)) {
    sprintf(buf,"%s tells the group, '%s'.\n\r",GET_NAME(ch),argument);
    send_to_char(buf, k);
  }

  for(f=k->followers; f; f=f->next) {
    if (IS_AFFECTED(f->follower, AFF_GROUP)){
      sprintf(buf,"%s tells the group, '%s'.\n\r",GET_NAME(ch),argument);
      send_to_char(buf,f->follower);
    }
  }
}

void do_report(struct char_data *ch, char *argument, int cmd)
{
  char buf[256];

  sprintf(buf, "$n says, 'My stats are: [%d/%dhp %d/%dm %d/%dmv]'.",
      GET_HIT(ch),
      GET_MAX_HIT(ch),
      GET_MANA(ch),
      GET_MAX_MANA(ch),
      GET_MOVE(ch),
      GET_MAX_MOVE(ch));
  act(buf, FALSE, ch, 0, 0, TO_CHAR);
  act(buf, FALSE, ch, 0, 0, TO_ROOM);
}

bool CAN_SEE( struct char_data *sub, struct char_data *obj )
{
    if (obj == sub)
      return TRUE;
    
    if ( obj->specials.wizInvis )
    {
	if ( IS_NPC(sub) || GET_LEVEL(sub) < GET_LEVEL(obj) )
	    return FALSE;
    }

    if ( sub->specials.holyLite )
	return TRUE;

    if ( IS_AFFECTED(sub, AFF_BLIND) )
	return FALSE;

    if ( !IS_LIGHT(sub->in_room) && !IS_AFFECTED(sub, AFF_INFRARED) )
	return FALSE;

    if ( ( IS_AFFECTED( obj, AFF_SNEAK ) || ( IS_AFFECTED( obj, AFF_HIDE ) ) )
	&& IS_NPC(sub) )
    {
	if ( number( 1, 100 ) < GET_LEVEL(sub) * 3 / 2 )
	    return FALSE;
    }

    if ( !IS_AFFECTED( obj, AFF_INVISIBLE ) )
	return TRUE;

    if ( IS_AFFECTED( sub, AFF_DETECT_INVISIBLE ) )
	return TRUE;

    return FALSE;
}



bool CAN_SEE_OBJ( struct char_data *sub, struct obj_data *obj )
{
    if ( sub->specials.holyLite )
	return TRUE;

    if ( IS_AFFECTED( sub, AFF_BLIND ) )
	return FALSE;

    if ( !IS_LIGHT(sub->in_room) && !IS_AFFECTED(sub, AFF_INFRARED) )
	return FALSE;

    if ( !IS_SET(obj->obj_flags.extra_flags, ITEM_INVISIBLE) )
	return TRUE;

    if ( IS_AFFECTED(sub, AFF_DETECT_INVISIBLE) )
	return TRUE;

    return FALSE;
}



bool check_blind( struct char_data *ch )
{
    if ( IS_AFFECTED(ch, AFF_BLIND) )
    {
	send_to_char( "You can't see a damn thing!\n\r", ch );
	return TRUE;
    }

    return FALSE;
}



/*
 * Given a mob, determine what level its eq is.
 * Wormhole to shopping_buy.
 */
int map_eq_level( struct char_data *mob )
{
    if ( mob_index[mob->nr].func == shop_keeper )
	return 1000;
    if ( GET_LEVEL(mob) <= 5 )
	return 0;
    else if ( GET_LEVEL(mob) <= 30 )
	return GET_LEVEL(mob) - 5;
    else
	return 25;
}



void do_tick( struct char_data *ch, char *argument, int cmd )
{
    int	ntick;
    char buf[256];

    if ( IS_NPC(ch) )
    {
	send_to_char( "Monsters don't wait for anything.\n\r", ch );
	return;
    }

    if ( ch->desc == NULL )
	return;

    while ( *argument == ' ' )
	argument++;

    if ( *argument == '\0' )
	ntick = 1;
    else
	ntick = atoi( argument );

    if ( ntick == 1 )
	sprintf( buf, "$n is waiting for one tick." );
    else
	sprintf( buf, "$n is waiting for %d ticks.", ntick );

    act(buf, TRUE, ch, 0, 0, TO_CHAR);
    act(buf, TRUE, ch, 0, 0, TO_ROOM);

    ch->desc->tick_wait	= ntick;
}
