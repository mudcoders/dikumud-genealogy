/***************************************************************************
 *  File: olc.c                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 ***************************************************************************/

#if defined(macintosh)
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
#include "olc.h"



/*
 * Local functions.
 */
AREA_DATA *get_area_data args((int vnum));


/*
 * Executed from comm.c.  Minimizes compiling when changes are made. 
 */
bool run_olc_editor(DESCRIPTOR_DATA * d)
{
    if (!d || !d->character)
	return FALSE;

    switch ( d->editor)
    {
	case ED_AREA:
	    aedit(d->character, d->incomm);
	    break;
	case ED_ROOM:
	    redit(d->character, d->incomm);
	    break;
	case ED_OBJECT:
	    oedit(d->character, d->incomm);
	    break;
	case ED_MOBILE:
	    medit(d->character, d->incomm);
	    break;
	case ED_MPROG:
	    mpedit(d->character, d->incomm);
	    break;

	default:
	    return FALSE;
    }
    return TRUE;
}



char *olc_ed_name(CHAR_DATA * ch)
{
    static char buf[10];

    buf[0] = '\0';

    if (!ch->desc)
	return &buf[0];

    switch (ch->desc->editor)
    {
	case ED_AREA:
	    sprintf(buf, "AEDIT");
	    break;
	case ED_ROOM:
	    sprintf(buf, "REDIT");
	    break;
	case ED_OBJECT:
	    sprintf(buf, "OEDIT");
	    break;
	case ED_MOBILE:
	    sprintf(buf, "MEDIT");
	    break;
	case ED_MPROG:
	    sprintf(buf, "MPEDIT");
	    break;
	default:
	    sprintf(buf, " ");
	    break;
    }
    return buf;
}



char *olc_ed_vnum(CHAR_DATA * ch)
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    OBJ_INDEX_DATA *pObj;
    MOB_INDEX_DATA *pMob;
    static char buf[10];

    buf[0] = '\0';

    if (!ch->desc)
	return &buf[0];

    switch (ch->desc->editor)
    {
	case ED_AREA:
	    pArea = (AREA_DATA *) ch->desc->pEdit;
	    sprintf(buf, "%d", pArea ? pArea->vnum : 0);
	    break;
	case ED_ROOM:
	    pRoom = ch->in_room;
	    sprintf(buf, "%d", pRoom ? pRoom->vnum : 0);
	    break;
	case ED_OBJECT:
	    pObj = (OBJ_INDEX_DATA *) ch->desc->pEdit;
	    sprintf(buf, "%d", pObj ? pObj->vnum : 0);
	    break;
	case ED_MOBILE:
	    pMob = (MOB_INDEX_DATA *) ch->desc->pEdit;
	    sprintf(buf, "%d", pMob ? pMob->vnum : 0);
	    break;
	case ED_MPROG:
	    pMob = (MOB_INDEX_DATA *) ch->desc->pEdit;
	    sprintf(buf, "%d(%d)", pMob ? pMob->vnum : 0,
		    ch->pcdata->mprog_edit + 1);
	    break;
	default:
	    sprintf(buf, " ");
	    break;
    }

    return buf;
}



/*****************************************************************************
 Name:		show_olc_cmds
 Purpose:	Format up the commands from given table.
 Called by:	show_commands(olc_act.c).
 ****************************************************************************/
void show_olc_cmds(CHAR_DATA * ch, const struct olc_cmd_type *olc_table)
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    int cmd;
    int col;

    buf1[0] = '\0';
    col = 0;
    for (cmd = 0; olc_table[cmd].name[0] != '\0'; cmd++)
    {
	sprintf(buf, "%-15.15s", olc_table[cmd].name);
	strcat(buf1, buf);
	if (++col % 5 == 0)
	    strcat(buf1, "\n\r");
    }

    if (col % 5 != 0)
	strcat(buf1, "\n\r");

    send_to_char(buf1, ch);
    return;
}



/*****************************************************************************
 Name:		show_commands
 Purpose:	Display all olc commands.
 Called by:	olc interpreters.
 ****************************************************************************/
bool show_commands(CHAR_DATA * ch, char *argument)
{
    switch (ch->desc->editor)
    {
	case ED_AREA:
	    show_olc_cmds(ch, aedit_table);
	    break;
	case ED_ROOM:
	    show_olc_cmds(ch, redit_table);
	    break;
	case ED_OBJECT:
	    show_olc_cmds(ch, oedit_table);
	    break;
	case ED_MOBILE:
	    show_olc_cmds(ch, medit_table);
	    break;
	case ED_MPROG:
	    show_olc_cmds(ch, mpedit_table);
	    break;
    }

    return FALSE;
}



/*****************************************************************************
 *                           Interpreter Tables.                             *
 *****************************************************************************/
const struct olc_cmd_type aedit_table[] =
{
/*  {   command          function                }, */
    {	"show", 	aedit_show	},
    {	"age", 		aedit_age	},
    {	"builders", 	aedit_builder	},
    {	"commands", 	show_commands	},
    {	"create", 	aedit_create	},
    {	"filename", 	aedit_file	},
    {	"name", 	aedit_name	},
    {	"recall", 	aedit_recall	},
    {	"reset", 	aedit_reset	},
    {	"security", 	aedit_security	},
    {	"vnum", 	aedit_vnum	},
    {	"lvnum", 	aedit_lvnum	},
    {	"uvnum", 	aedit_uvnum	},

    {	"?", 		show_help	},
    {	"version", 	show_version	},
    {	"done", 	edit_done	},

    {	"", 		0,		}
};



const struct olc_cmd_type redit_table[] =
{
/*  {   command          function                }, */
    {	"north", 	redit_north	},
    {	"south", 	redit_south	},
    {	"east", 	redit_east	},
    {	"west", 	redit_west	},
    {	"up", 		redit_up	},
    {	"down", 	redit_down	},
    {	"walk", 	redit_move	},

    {	"commands", 	show_commands	},
    {	"show", 	redit_show	},
    {	"create", 	redit_create	},
    {	"desc", 	redit_desc	},
    {	"ed", 		redit_ed	},
    {	"format", 	redit_format	},
    {	"name", 	redit_name	},

  /*
   * New reset commands. 
   */
    {	"mreset", 	redit_mreset	},
    {	"oreset", 	redit_oreset	},
    {	"mlist", 	redit_mlist	},
    {	"olist", 	redit_olist	},
    {	"mshow", 	redit_mshow	},
    {	"oshow", 	redit_oshow	},

    {	"?", 		show_help	},
    {	"version", 	show_version	},
    {	"done", 	edit_done	},

    {	"", 		0,		}
};



const struct olc_cmd_type oedit_table[] =
{
/*  {   command          function                }, */
    {	"show", 	oedit_show	},
    {	"commands", 	show_commands	},
    {	"addaffect", 	oedit_addaffect	},
    {	"cost", 	oedit_cost	},
    {	"create", 	oedit_create	},
    {	"copy", 	oedit_copy	},
    {	"delaffect", 	oedit_delaffect	},
    {	"ed", 		oedit_ed	},
    {	"long", 	oedit_long	},
    {	"name", 	oedit_name	},
    {	"short", 	oedit_short	},
    {	"v0", 		oedit_value0	},
    {	"v1", 		oedit_value1	},
    {	"v2", 		oedit_value2	},
    {	"v3", 		oedit_value3	},
    {	"v4", 		oedit_value4	},
    {	"weight", 	oedit_weight	},

    {	"?", 		show_help	},
    {	"version", 	show_version	},
    {	"done", 	edit_done	},

    {	"", 		0,		}
};



const struct olc_cmd_type medit_table[] =
{
/*  {   command          function                }, */
    {	"alignment", 	medit_align	},
    {	"commands", 	show_commands	},
    {	"create", 	medit_create	},
    {	"copy", 	medit_copy	},
    {	"desc", 	medit_desc	},
    {	"level", 	medit_level	},
    {	"long", 	medit_long	},
    {	"name", 	medit_name	},
    {	"shop", 	medit_shop	},
    {	"short", 	medit_short	},
    {	"show", 	medit_show	},
    {	"spec", 	medit_spec	},

    {	"?", 		show_help	},
    {	"version", 	show_version	},
    {	"done", 	edit_done	},

    {	"", 		0,		}
};

const struct olc_cmd_type mpedit_table[] =
{
/*  {   command          function                }, */
    {	"add", 		mpedit_add	},
    {	"commands", 	show_commands	},
    {	"copy", 	mpedit_copy	},
    {	"create", 	mpedit_create	},
    {	"delete", 	mpedit_delete	},
    {	"program", 	mpedit_program	},
    {	"show", 	mpedit_show	},
    {	"trigger", 	mpedit_trigger	},

    {	"?", 		show_help	},
    {	"version", 	show_version	},
    {	"done", 	edit_done	},

    {	"", 		0,		}
};

/*****************************************************************************
 *                          End Interpreter Tables.                          *
 *****************************************************************************/



/*****************************************************************************
 Name:		get_area_data
 Purpose:	Returns pointer to area with given vnum.
 Called by:	do_aedit(olc.c).
 ****************************************************************************/
AREA_DATA *get_area_data(int vnum)
{
    AREA_DATA *pArea;

    for (pArea = area_first; pArea; pArea = pArea->next)
    {
	if (pArea->vnum == vnum)
	    return pArea;
    }

    return 0;
}



/*****************************************************************************
 Name:		edit_done
 Purpose:	Resets builder information on completion.
 Called by:	aedit, redit, oedit, medit(olc.c)
 ****************************************************************************/
bool edit_done(CHAR_DATA * ch, char *argument)
{
    ch->desc->pEdit = NULL;
    ch->desc->editor = 0;
    return FALSE;
}



/*****************************************************************************
 *                              Interpreters.                                *
 *****************************************************************************/


/*
 * Area Interpreter, called by do_aedit. 
 */
void aedit(CHAR_DATA * ch, char *argument)
{
    AREA_DATA *pArea = edit_area( ch );
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int cmd;
    int value;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    if ( !is_builder( ch, pArea ) )
    {
	send_to_char("AEdit:  Insufficient security to modify area.\n\r", ch);
	interpret(ch, arg);
	return;
    }

    if (command[0] == '\0')
    {
	aedit_show(ch, argument);
	return;
    }

    /*
     * Search Table and Dispatch Command. 
     */
    for (cmd = 0; *aedit_table[cmd].name; cmd++)
    {
	if (!str_prefix(command, aedit_table[cmd].name))
	{
	    if ((*aedit_table[cmd].olc_fun) (ch, argument))
		SET_BIT(pArea->area_flags, AREA_CHANGED);
	    return;
	}
    }

    /*
     * Take care of flags. 
     */
    if ((value = flag_value(area_flags, arg)) != NO_FLAG)
    {
	TOGGLE_BIT(pArea->area_flags, value);

	send_to_char("Flag toggled.\n\r", ch);
	return;
    }

    /*
     * Default to Standard Interpreter. 
     */
    interpret(ch, arg);
    return;
}



/*
 * Room Interpreter, called by do_redit. 
 */
void redit(CHAR_DATA * ch, char *argument)
{
    ROOM_INDEX_DATA *pRoom;
    AREA_DATA *pArea;
    char arg[MAX_STRING_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;
    int value;

    pRoom = edit_room(ch);
    pArea = pRoom->area;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    if (!is_builder(ch, pArea))
    {
	send_to_char("Insufficient security to modify room.\n\r", ch);
	interpret(ch, arg);
	return;
    }

    if (command[0] == '\0')
    {
	redit_show(ch, argument);
	return;
    }

    /*
     * Search Table and Dispatch Command. 
     */
    for (cmd = 0; *redit_table[cmd].name; cmd++)
    {
	if (!str_prefix(command, redit_table[cmd].name))
	{
	    if ((*redit_table[cmd].olc_fun) (ch, argument))
		SET_BIT(pArea->area_flags, AREA_CHANGED);
	    return;
	}
    }

    /*
     * Take care of flags. 
     */
    if ((value = flag_value(room_flags, arg)) != NO_FLAG)
    {
	TOGGLE_BIT(pRoom->orig_room_flags, value);

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Room flag toggled.\n\r", ch);
	return;
    }

    if ((value = flag_value(sector_flags, arg)) != NO_FLAG)
    {
	pRoom->sector_type = value;

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Sector type set.\n\r", ch);
	return;
    }

    /*
     * Default to Standard Interpreter. 
     */
    interpret(ch, arg);
    return;
}



/*
 * Object Interpreter, called by do_oedit. 
 */
void oedit(CHAR_DATA * ch, char *argument)
{
    AREA_DATA *pArea;
    OBJ_INDEX_DATA *pObj;
    char arg[MAX_STRING_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;
    int value;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    pObj = edit_obj(ch);
    pArea = pObj->area;

    if (!is_builder(ch, pArea))
    {
	send_to_char("Insufficient security to modify area.\n\r", ch);
	interpret(ch, arg);
	return;
    }

    if (command[0] == '\0')
    {
	oedit_show(ch, argument);
	return;
    }

    /*
     * Search Table and Dispatch Command. 
     */
    for (cmd = 0; *oedit_table[cmd].name; cmd++)
    {
	if (!str_prefix(command, oedit_table[cmd].name))
	{
	    if ((*oedit_table[cmd].olc_fun) (ch, argument))
		SET_BIT(pArea->area_flags, AREA_CHANGED);
	    return;
	}
    }

    /*
     * Take care of flags. 
     */
    if ((value = flag_value(type_flags, arg)) != NO_FLAG)
    {
	pObj->item_type = value;

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Type set.\n\r", ch);

	/*
	 * Clear the values.
	 */
	pObj->value[0] = 0;
	pObj->value[1] = 0;
	pObj->value[2] = 0;
	pObj->value[3] = 0;

	return;
    }

    if ((value = flag_value(extra_flags, arg)) != NO_FLAG)
    {
	TOGGLE_BIT(pObj->extra_flags, value);

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Extra flag toggled.\n\r", ch);
	return;
    }

    if ((value = flag_value(wear_flags, arg)) != NO_FLAG)
    {
	TOGGLE_BIT(pObj->wear_flags, value);

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Wear flag toggled.\n\r", ch);
	return;
    }

    /*
     * Default to Standard Interpreter. 
     */
    interpret(ch, arg);
    return;
}



/*
 * Mobile Interpreter, called by do_medit. 
 */
void medit(CHAR_DATA * ch, char *argument)
{
    AREA_DATA *pArea;
    MOB_INDEX_DATA *pMob;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_STRING_LENGTH];
    int cmd;
    int value;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    pMob = edit_mob(ch);
    pArea = pMob->area;

    if (!is_builder(ch, pArea))
    {
	send_to_char("Insufficient security to modify area.\n\r", ch);
	interpret(ch, arg);
	return;
    }

    if (command[0] == '\0')
    {
	medit_show(ch, argument);
	return;
    }

    /*
     * Search Table and Dispatch Command. 
     */
    for (cmd = 0; *medit_table[cmd].name; cmd++)
    {
	if (!str_prefix(command, medit_table[cmd].name))
	{
	    if ((*medit_table[cmd].olc_fun) (ch, argument))
		SET_BIT(pArea->area_flags, AREA_CHANGED);
	    return;
	}
    }

    /*
     * Take care of flags. 
     */
    if ((value = flag_value(sex_flags, arg)) != NO_FLAG)
    {
	pMob->sex = value;

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Sex set.\n\r", ch);
	return;
    }

    if ((value = race_full_lookup( arg ) ) != NO_FLAG)
    {
	pMob->race = value;

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Race set.\n\r", ch);
	return;
    }

    if ((value = flag_value(act_flags, arg)) != NO_FLAG)
    {
	TOGGLE_BIT(pMob->act, value);

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Act flag toggled.\n\r", ch);
	return;
    }


    if ((value = flag_value(affect_flags, arg)) != NO_FLAG)
    {
	TOGGLE_BIT(pMob->affected_by, value);

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Affect flag toggled.\n\r", ch);
	return;
    }

    /*
     * Default to Standard Interpreter. 
     */
    interpret(ch, arg);
    return;
}


/*
 * MobProg Interpreter, called by do_mpedit. 
 */
void mpedit(CHAR_DATA * ch, char *argument)
{
    AREA_DATA *pArea;
    MOB_INDEX_DATA *pMob;
    MPROG_DATA *pMobProg;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_STRING_LENGTH];
    int cmd;
    int value;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    pMob = edit_mob(ch);
    pArea = pMob->area;
    pMobProg = edit_mprog(ch, pMob);

    if (!is_builder(ch, pArea))
    {
	send_to_char("Insufficient security to modify area.\n\r", ch);
	interpret(ch, arg);
	return;
    }

    if (command[0] == '\0')
    {
	mpedit_show(ch, "");
	return;
    }

    /*
     * Set mobprog type 
     */
    if ((value = flag_value(mprog_type_flags, arg)) != NO_FLAG)
    {
	if (pMobProg->type)
	    REMOVE_BIT(pMob->progtypes, pMobProg->type);
	pMobProg->type = value;
	SET_BIT(pMob->progtypes, pMobProg->type);

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("MOBProg type set.\n\r", ch);
	return;
    }

    /*
     * Search Table and Dispatch Command. 
     */
    for (cmd = 0; mpedit_table[cmd].name[0] != '\0'; cmd++)
    {
	if (!str_prefix(command, mpedit_table[cmd].name))
	{
	    if ((*mpedit_table[cmd].olc_fun) (ch, argument))
	    {
		SET_BIT(pArea->area_flags, AREA_CHANGED);
		return;
	    }
	    else
		return;
	}
    }

    /*
     * Default to Standard Interpreter. 
     */
    interpret(ch, arg);
    return;
}


void do_aedit(CHAR_DATA * ch, char *argument)
{
    AREA_DATA *pArea;
    char command[MAX_INPUT_LENGTH];

    argument = one_argument(argument, command);
    pArea = ch->in_room->area;

    if (command[0] == 'r' && !str_prefix(command, "reset"))
    {
	if (ch->desc->editor == ED_AREA)
	    reset_area((AREA_DATA *) ch->desc->pEdit);
	else
	    reset_area(pArea);
	send_to_char("Area reset.\n\r", ch);
	return;
    }

    if (command[0] == 'c' && !str_prefix(command, "create"))
    {
	if (aedit_create(ch, argument))
	{
	    ch->desc->editor = ED_AREA;
	    pArea = (AREA_DATA *) ch->desc->pEdit;
	    SET_BIT(pArea->area_flags, AREA_CHANGED);
	    aedit_show(ch, "");
	}
	return;
    }

    if (is_number(command))
    {
	if (!(pArea = get_area_data(atoi(command))))
	{
	    send_to_char("No such area vnum exists.\n\r", ch);
	    return;
	}
    }

    /*
     * Builder defaults to editing current area.
     */
    ch->desc->pEdit = (void *) pArea;
    ch->desc->editor = ED_AREA;
    aedit_show(ch, "");
    return;
}



/*
 * Entry point for editing room_index_data. 
 */
void do_redit(CHAR_DATA * ch, char *argument)
{
    ROOM_INDEX_DATA *pRoom;
    char command[MAX_INPUT_LENGTH];

    argument = one_argument(argument, command);
    pRoom = ch->in_room;

    if (command[0] == 'r' && !str_prefix(command, "reset"))
    {
	reset_room(pRoom);
	send_to_char("Room reset.\n\r", ch);
	return;
    }

    if (command[0] == 'c' && !str_prefix(command, "create"))
    {
	if (redit_create(ch, argument))
	{
	    char_from_room(ch);
	    char_to_room(ch, ch->desc->pEdit);
	    SET_BIT(pRoom->area->area_flags, AREA_CHANGED);
	}
    }

    /*
     * Builder defaults to editing current room.
     */
    ch->desc->editor = ED_ROOM;
    redit_show(ch, "");
    return;
}



/*
 * Entry point for editing obj_index_data. 
 */
void do_oedit(CHAR_DATA * ch, char *argument)
{
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    char command[MAX_INPUT_LENGTH];

    argument = one_argument(argument, command);

    if (is_number(command))
    {
	if ( !(pObj = get_obj_index(atoi(command))) )
	{
	    send_to_char("That vnum does not exist.\n\r", ch);
	    return;
	}

	ch->desc->pEdit = (void *) pObj;
	ch->desc->editor = ED_OBJECT;
	oedit_show(ch, "");
	return;
    }

    if (command[0] == 'c' && !str_prefix(command, "create"))
    {
	if (oedit_create(ch, argument))
	{
	    pArea = get_vnum_area(atoi(argument));
	    SET_BIT(pArea->area_flags, AREA_CHANGED);
	    ch->desc->editor = ED_OBJECT;
	    oedit_show(ch, "");
	}
	return;
    }

    send_to_char("Syntax: oedit <vnum>.\n\r", ch);
    return;
}



/*
 * Entry point for editing mob_index_data. 
 */
void do_medit(CHAR_DATA * ch, char *argument)
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    char command[MAX_INPUT_LENGTH];

    argument = one_argument(argument, command);
    if (is_number(command))
    {
	if (!(pMob = get_mob_index(atoi(command))))
	{
	    send_to_char("That vnum does not exist.\n\r", ch);
	    return;
	}

	ch->desc->pEdit = (void *) pMob;
	ch->desc->editor = ED_MOBILE;
	medit_show(ch, "");
	return;
    }

    if (command[0] == 'c' && !str_prefix(command, "create"))
    {
	if (medit_create(ch, argument))
	{
	    pArea = get_vnum_area(atoi(argument));
	    SET_BIT(pArea->area_flags, AREA_CHANGED);
	    ch->desc->editor = ED_MOBILE;
	    medit_show(ch, "");
	}
	return;
    }

    send_to_char("Syntax: medit <vnum>.\n\r", ch);
    return;
}



/*
 * Entry point for editing mob_prog_data. 
 */
void do_mpedit(CHAR_DATA * ch, char *argument)
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    MPROG_DATA *mprg, *cprg;
    int value;
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (ch->in_room && !is_builder(ch, ch->in_room->area))
    {
	send_to_char("Insufficient security to edit mobprog.\n\r", ch);
	return;
    }

    if (is_number(arg1))
    {
	value = atoi(arg1);
	if (!(pMob = get_mob_index(value)))
	{
	    send_to_char("That vnum does not exist.\n\r", ch);
	    return;
	}

	if (arg2[0] == '\0' || atoi(arg2) == 0)
	{
	    if (!pMob->mobprogs)
	    {
		send_to_char("Mobile has no mobprogs.", ch);
		send_to_char("  Use create.\n\r", ch);
		return;
	    }
	    else
	    {
		send_to_char("Editing first mobprog.\n\r", ch);
		value = 1;
	    }
	}
	else if ((value = atoi(arg2)) > mprog_count(pMob))
	{
	    send_to_char("Mobile does not have that many mobprogs.\n\r", ch);
	    return;
	}

	ch->desc->pEdit = (void *) pMob;
	ch->pcdata->mprog_edit = value - 1;
	ch->desc->editor = ED_MPROG;
	return;
    }
    else
    {
	if (!str_cmp(arg1, "create"))
	{
	    value = atoi(arg2);
	    if (arg2[0] == '\0' || value == 0)
	    {
		send_to_char("Syntax:  edit mobprog create vnum [svnum]\n\r",
			     ch);
		return;
	    }

	    pArea = get_vnum_area(value);

	    if (mpedit_create(ch, arg2))
	    {
		SET_BIT(pArea->area_flags, AREA_CHANGED);
		ch->desc->editor = ED_MPROG;
	    }
	    else
		return;

	    /*
	     * See if another argument was supplied, meaning COPY 
	     */
	    if (is_number(argument))
	    {
		MOB_INDEX_DATA *cMob, *pMob;

		/*
		 * Check destination mobile 
		 */
		if (!(pMob = get_mob_index(value)))
		{
		    send_to_char("No destination mob exists.\n\r", ch);
		    return;
		}

		value = atoi(argument);
		if (!(cMob = get_mob_index(value)))
		{
		    send_to_char("No such source mob exists.\n\r", ch);
		    return;
		}

		/*
		 * Start copying 
		 */
		mprg = pMob->mobprogs;	/*
					 * allocated by create 
					 */

		for (cprg = cMob->mobprogs; cprg;
		     cprg = cprg->next, mprg = mprg->next)
		{
		    mprg->type = cprg->type;
		    mprg->arglist = str_dup(cprg->arglist);
		    mprg->comlist = str_dup(cprg->comlist);
		    if (cprg->next)
			mprg->next = (MPROG_DATA *) alloc_perm(sizeof(
							    MPROG_DATA));
		    else
			mprg->next = NULL;
		}

		send_to_char("MOBProg copied.\n\r", ch);
	    }
	    return;
	}
    }

    send_to_char("Syntax: mpedit <vnum>.\n\r", ch);
    return;
}



void display_resets(CHAR_DATA * ch)
{
    ROOM_INDEX_DATA *pRoom;
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pMob = NULL;
    char buf[MAX_STRING_LENGTH];
    char final[MAX_STRING_LENGTH];
    int iReset = 0;

    pRoom = edit_room(ch);
    final[0] = '\0';

    send_to_char("{c No.  Loads    Description       Location         Vnum", ch);
    send_to_char("    Max    Description{x\n\r{c==== ======== =============", ch);
    send_to_char(" =================== ======== ===== ==============={x\n\r", ch);

    for (pReset = pRoom->reset_first; pReset; pReset = pReset->next)
    {
	OBJ_INDEX_DATA *pObj;
	MOB_INDEX_DATA *pMobIndex;
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_INDEX_DATA *pObjToIndex;
	ROOM_INDEX_DATA *pRoomIndex;

	final[0] = '\0';
	sprintf(final, "{c[%2d] {x", ++iReset);

	switch (pReset->command)
	{
	    default:
		sprintf(buf, "Bad reset command: %c.", pReset->command);
		strcat(final, buf);
		break;

	    case 'M':
		if (!(pMobIndex = get_mob_index(pReset->arg1)))
		{
		    sprintf(buf, "Load Mobile - Bad Mob %d\n\r", pReset->arg1);
		    strcat(final, buf);
		    continue;
		}

		if (!(pRoomIndex = get_room_index(pReset->arg3)))
		{
		    sprintf(buf, "Load Mobile - Bad Room %d\n\r", pReset->arg3);
		    strcat(final, buf);
		    continue;
		}

		pMob = pMobIndex;
		sprintf(buf, "{BM[%5d] %-13.13s in room             R[%5d] [%3d] %-15.15s{x\n\r",
			pReset->arg1, pMob->short_descr, pReset->arg3,
			pReset->arg2, pRoomIndex->name);
		strcat(final, buf);

		/*
		 * Check for pet shop.
		 * -------------------
		 */
		{
		    ROOM_INDEX_DATA *pRoomIndexPrev;

		    pRoomIndexPrev = get_room_index(pRoomIndex->vnum - 1);
		    if (pRoomIndexPrev
		    && IS_SET(pRoomIndexPrev->orig_room_flags, ROOM_PET_SHOP))
			final[5] = 'P';
		}

		break;

	    case 'O':
		if (!(pObjIndex = get_obj_index(pReset->arg1)))
		{
		    sprintf(buf, "Load Object - Bad Object %d\n\r",
			    pReset->arg1);
		    strcat(final, buf);
		    continue;
		}

		pObj = pObjIndex;

		if (!(pRoomIndex = get_room_index(pReset->arg3)))
		{
		    sprintf(buf, "Load Object - Bad Room %d\n\r", pReset->arg3);
		    strcat(final, buf);
		    continue;
		}

		sprintf(buf, "{GO[%5d] %-13.13s in room             R[%5d]       %-15.15s{x\n\r",
			pReset->arg1, pObj->short_descr,
			pReset->arg3, pRoomIndex->name);
		strcat(final, buf);

		break;

	    case 'P':
		if (!(pObjIndex = get_obj_index(pReset->arg1)))
		{
		    sprintf(buf, "Put Object - Bad Object %d\n\r",
			    pReset->arg1);
		    strcat(final, buf);
		    continue;
		}

		pObj = pObjIndex;

		if (!(pObjToIndex = get_obj_index(pReset->arg3)))
		{
		    sprintf(buf, "Put Object - Bad To Object %d\n\r",
			    pReset->arg3);
		    strcat(final, buf);
		    continue;
		}

		sprintf(buf,
			"{gO[%5d] %-13.13s inside              O[%5d]       %-15.15s{x\n\r",
			pReset->arg1,
			pObj->short_descr,
			pReset->arg3,
			pObjToIndex->short_descr);
		strcat(final, buf);

		break;

	    case 'G':
	    case 'E':
		if (!(pObjIndex = get_obj_index(pReset->arg1)))
		{
		    sprintf(buf, "Give/Equip Object - Bad Object %d\n\r",
			    pReset->arg1);
		    strcat(final, buf);
		    continue;
		}

		pObj = pObjIndex;

		if (!pMob)
		{
		    sprintf(buf, "Give/Equip Object - No Previous Mobile\n\r");
		    strcat(final, buf);
		    break;
		}

		if (pMob->pShop)
		{
		    sprintf(buf,
			    "{bO[%5d] %-13.13s in the inventory of S[%5d]       %-15.15s{x\n\r",
			    pReset->arg1,
			    pObj->short_descr,
			    pMob->vnum,
			    pMob->short_descr);
		}
		else
		    sprintf(buf,
		    "{bO[%5d] %-13.13s %-19.19s M[%5d]       %-15.15s{x\n\r",
			    pReset->arg1,
			    pObj->short_descr,
			    (pReset->command == 'G') ?
			    flag_string(wear_loc_strings, WEAR_NONE)
			    : flag_string(wear_loc_strings, pReset->arg3),
			    pMob->vnum,
			    pMob->short_descr);
		strcat(final, buf);

		break;

		/*
		 * Doors are set in rs_flags don't need to be displayed.
		 * If you want to display them then uncomment the new_reset
		 * line in the case 'D' in load_resets in db.c and here.
		 *
		 case 'D':
		 pRoomIndex = get_room_index( pReset->arg1 );
		 sprintf( buf, "{MR[%5d] %s door of %-19.19s reset to %s{x\n\r",
		 pReset->arg1,
		 capitalize( dir_name[ pReset->arg2 ] ),
		 pRoomIndex->name,
		 flag_string( door_resets, pReset->arg3 ) );
		 strcat( final, buf );
		 
		 break;
		 *
		 * End Doors Comment.
		 */
	    case 'R':
		if (!(pRoomIndex = get_room_index(pReset->arg1)))
		{
		    sprintf(buf, "Randomize Exits - Bad Room %d\n\r",
			    pReset->arg1);
		    strcat(final, buf);
		    continue;
		}

		sprintf(buf, "{rR[%5d] Exits are randomized in %s{x\n\r",
			pReset->arg1, pRoomIndex->name);
		strcat(final, buf);

		break;
	}
	send_to_char(final, ch);
    }

    return;
}



/*****************************************************************************
  Name:		add_reset
  Purpose:	Inserts a new reset in the given index slot.
  Called by:	do_resets(olc.c).
  ****************************************************************************/
void add_reset(ROOM_INDEX_DATA * room, RESET_DATA * pReset, int index)
{
    RESET_DATA *reset;
    int iReset = 0;

    if (!room->reset_first)
    {
	room->reset_first = pReset;
	room->reset_last = pReset;
	pReset->next = NULL;
	return;
    }

    index--;

    if (index == 0)		/*
				 * First slot (1) selected. 
				 */
    {
	pReset->next = room->reset_first;
	room->reset_first = pReset;
	return;
    }

    /*
     * If negative slot( <= 0 selected) then this will find the last.
     */
    for (reset = room->reset_first; reset->next; reset = reset->next)
    {
	if (++iReset == index)
	    break;
    }

    pReset->next = reset->next;
    reset->next = pReset;
    if (!pReset->next)
	room->reset_last = pReset;
    return;
}



void do_resets(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char arg4[MAX_INPUT_LENGTH];
    char arg5[MAX_INPUT_LENGTH];
    RESET_DATA *pReset = NULL;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);
    argument = one_argument(argument, arg4);
    argument = one_argument(argument, arg5);

    /*
     * Display resets in current room.
     * -------------------------------
     */
    if (arg1[0] == '\0')
    {
	if (ch->in_room->reset_first)
	{
	    send_to_char("{cResets: M = mobile, R = room, O = object, {x", ch);
	    send_to_char("{cP = pet, S = shopkeeper{x\n\r", ch);
	    display_resets(ch);
	}
	else
	    send_to_char("There are no resets in this room.\n\r", ch);
    }

    if (!is_builder(ch, ch->in_room->area))
    {
	send_to_char("Invalid security for editing this area.\n\r",
		     ch);
	return;
    }

    /*
     * Take index number and search for commands.
     * ------------------------------------------
     */
    if (is_number(arg1))
    {
	ROOM_INDEX_DATA *pRoom = ch->in_room;

	/*
	 * Delete a reset.
	 * ---------------
	 */
	if (!str_cmp(arg2, "delete"))
	{
	    int insert_loc = atoi(arg1);

	    if (!ch->in_room->reset_first)
	    {
		send_to_char("There are no resets in this area.\n\r", ch);
		return;
	    }

	    if (insert_loc - 1 <= 0)
	    {
		pReset = pRoom->reset_first;
		pRoom->reset_first = pRoom->reset_first->next;
		if (!pRoom->reset_first)
		    pRoom->reset_last = NULL;
	    }
	    else
	    {
		int iReset = 0;
		RESET_DATA *prev = NULL;

		for (pReset = pRoom->reset_first;
		     pReset;
		     pReset = pReset->next)
		{
		    if (++iReset == insert_loc)
			break;
		    prev = pReset;
		}

		if (!pReset)
		{
		    send_to_char("Reset not found.\n\r", ch);
		    return;
		}

		if (prev)
		    prev->next = prev->next->next;
		else
		    pRoom->reset_first = pRoom->reset_first->next;

		for (pRoom->reset_last = pRoom->reset_first;
		     pRoom->reset_last->next;
		     pRoom->reset_last = pRoom->reset_last->next);
	    }

	    free_reset_data(pReset);
	    send_to_char("Reset deleted.\n\r", ch);
	}
	else
	    /*
	     * Add a reset.
	     * ------------
	     */
	    if ((!str_cmp(arg2, "mob") && is_number(arg3))
		|| (!str_cmp(arg2, "obj") && is_number(arg3)))
	{
	    /*
	     * Check for Mobile reset.
	     * -----------------------
	     */
	    if (!str_cmp(arg2, "mob"))
	    {
		pReset = new_reset_data();
		pReset->command = 'M';
		pReset->arg1 = atoi(arg3);
		pReset->arg2 = is_number(arg4) ? atoi(arg4) : 1;	/*
									 * Max #
									 */
		pReset->arg3 = ch->in_room->vnum;
	    }
	    else
		/*
		 * Check for Object reset.
		 * -----------------------
		 */
	    if (!str_cmp(arg2, "obj"))
	    {
		pReset = new_reset_data();
		pReset->arg1 = atoi(arg3);
		/*
		 * Inside another object.
		 * ----------------------
		 */
		if (!str_prefix(arg4, "inside"))
		{
		    pReset->command = 'P';
		    pReset->arg2 = 0;
		    pReset->arg3 = is_number(arg5) ? atoi(arg5) : 1;
		}
		else
		    /*
		     * Inside the room.
		     * ----------------
		     */
		if (!str_cmp(arg4, "room"))
		{
		    /* pReset = new_reset_data(); */
		    pReset->command = 'O';
		    pReset->arg1 = atoi(arg3);
		    pReset->arg2 = 0;
		    pReset->arg3 = ch->in_room->vnum;
		}
		else
		    /*
		     * Into a Mobile's inventory.
		     * --------------------------
		     */
		{
		    if (flag_value(wear_loc_flags, arg4) == NO_FLAG)
		    {
			send_to_char("Resets: '? wear-loc'\n\r", ch);
			return;
		    }
		    /* pReset = new_reset_data(); */
		    pReset->arg3 = flag_value(wear_loc_flags, arg4);
		    if (pReset->arg2 == WEAR_NONE)
			pReset->command = 'G';
		    else
			pReset->command = 'E';
		}
	    }

	    add_reset(ch->in_room, pReset, atoi(arg1));
	    send_to_char("Reset added.\n\r", ch);
	}
	else
	{
	    send_to_char("Syntax: RESET <number> OBJ <vnum> <wearloc>\n\r", ch);
	    send_to_char("        RESET <number> OBJ <vnum> in <vnum>\n\r", ch);
	    send_to_char("        RESET <number> OBJ <vnum> room\n\r", ch);
	    send_to_char("        RESET <number> MOB <vnum> [<max #>]\n\r", ch);
	    send_to_char("        RESET <number> DELETE\n\r", ch);
	}
    }

    return;
}



/*****************************************************************************
  Name:		do_alist
  Purpose:	Normal command to list areas and display area information.
  Called by:	interpreter(interp.c)
  ****************************************************************************/
void do_alist(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char result[MAX_STRING_LENGTH * 2];		/*
						 * May need tweaking. 
						 */
    AREA_DATA *pArea;

    sprintf(result, "{c[%3s] [%-27s] [%-5s/%5s] [%-10s] %3s [%-10s]{x\n\r",
    "Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders");

    for (pArea = area_first; pArea; pArea = pArea->next)
    {
	sprintf(buf, "{c[%3d] {g%-29.29s {c[{x%5d{c/{x%-5d{c] {c%-12.12s [{x%d{c] [{x%-10.10s{c]{x\n\r",
		pArea->vnum,
		&pArea->name[8],
		pArea->lvnum,
		pArea->uvnum,
		pArea->filename,
		pArea->security,
		pArea->builders);
	strcat(result, buf);
    }

    send_to_char(result, ch);
    return;
}
