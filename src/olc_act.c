/***************************************************************************
 *  File: olc_act.c                                                        *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
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
#include <limits.h>	/* OLC 1.1b */
#include "merc.h"
#include "olc.h"



struct olc_help_type
{
    char *command;
    const void *structure;
    char *desc;
};



bool show_version( CHAR_DATA *ch, char *argument )
{
    send_to_char( VERSION, ch );
    send_to_char( "\r\n", ch );
    send_to_char( AUTHOR, ch );
    send_to_char( "\r\n", ch );
    send_to_char( DATE, ch );
    send_to_char( "\r\n", ch );
    send_to_char( CREDITS, ch );
    send_to_char( "\r\n", ch );

    return FALSE;
}    

/*
 * This table contains help commands and a brief description of each.
 * ------------------------------------------------------------------
 */
const struct olc_help_type help_table[] =
{
    {	"area",		area_flags,	 "Area attributes."		},
    {	"room",		room_flags,	 "Room attributes."		},
    {	"sector",	sector_flags,	 "Sector types, terrain."	},
    {	"exit",		exit_flags,	 "Exit types."			},
    {	"type",		type_flags,	 "Types of objects."		},
    {	"extra",	extra_flags,	 "Object attributes."		},
    {	"wear",		wear_flags,	 "Where to wear object."	},
    {	"spec",		spec_table,	 "Available special programs." 	},
    {	"game",		game_table,	 "Availeble gambling programs."	},
    {	"sex",		sex_flags,	 "Sexes."			},
    {	"act",		act_flags,	 "Mobile attributes."		},
    {	"affect",	affect_flags,	 "Mobile affects."		},
    {	"wear-loc",	wear_loc_flags,	 "Where mobile wears object."	},
    {	"spells",	skill_table,	 "Names of current spells." 	},
    {	"weapon",	weapon_flags,	 "Type of weapon." 		},
    {	"container",	container_flags, "Container status."		},
    {	"liquid",	liquid_flags,	 "Types of liquids."		},
    {	"",		0,		 ""				}
};



/*****************************************************************************
 Name:		show_flag_cmds
 Purpose:	Displays settable flags and stats.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_flag_cmds( CHAR_DATA *ch, const struct flag_type *flag_table )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  flag;
    int  col;
 
    buf1[0] = '\0';
    col = 0;
    for (flag = 0; *flag_table[flag].name; flag++)
    {
	if ( flag_table[flag].settable )
	{
	    sprintf( buf, "%-19.18s", flag_table[flag].name );
	    strcat( buf1, buf );
	    if ( ++col % 4 == 0 )
		strcat( buf1, "\r\n" );
	}
    }
 
    if ( col % 4 != 0 )
	strcat( buf1, "\r\n" );

    send_to_char( buf1, ch );
    return;
}



/*****************************************************************************
 Name:		show_skill_cmds
 Purpose:	Displays all skill functions.
 		Does remove those damn immortal commands from the list.
 		Could be improved by:
 		(1) Adding a check for a particular class.
 		(2) Adding a check for a level range.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_skill_cmds( CHAR_DATA *ch, int tar )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH*2 ];
    int  sn;
    int  col;
 
    buf1[0] = '\0';
    col = 0;
    for (sn = 0; sn < MAX_SKILL; sn++)
    {
	if ( !skill_table[sn].name )
	    break;

	if ( !str_cmp( skill_table[sn].name, "reserved" )
	  || skill_table[sn].spell_fun == spell_null )
	    continue;

	if ( tar == -1 || skill_table[sn].target == tar )
	{
	    sprintf( buf, "%-19.18s", skill_table[sn].name );
	    strcat( buf1, buf );
	    if ( ++col % 4 == 0 )
		strcat( buf1, "\r\n" );
	}
    }
 
    if ( col % 4 != 0 )
	strcat( buf1, "\r\n" );

    send_to_char( buf1, ch );
    return;
}



/*****************************************************************************
 Name:		show_spec_cmds
 Purpose:	Displays settable special functions.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_spec_cmds( CHAR_DATA *ch )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  spec;
    int  col;
 
    buf1[0] = '\0';
    col = 0;
    send_to_char( "Preceed special functions with 'spec_'\r\n\r\n", ch );
    for (spec = 0; *spec_table[spec].spec_fun; spec++)
    {
	sprintf( buf, "%-19.18s", &spec_table[spec].spec_name[5] );
	strcat( buf1, buf );
	if ( ++col % 4 == 0 )
	    strcat( buf1, "\r\n" );
    }
 
    if ( col % 4 != 0 )
	strcat( buf1, "\r\n" );

    send_to_char( buf1, ch );
    return;
}

void show_game_cmds( CHAR_DATA *ch )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  game;
    int  col;

    buf1[0] = '\0';
    col = 0;
    send_to_char( "Preceed game functions with 'game_'\r\n\r\n", ch );
    for (game = 0; *game_table[game].game_fun; game++)
    {
        sprintf( buf, "%-19.18s", &game_table[game].game_name[5] );
        strcat( buf1, buf );
        if ( ++col % 4 == 0 )
            strcat( buf1, "\r\n" );
    }

    if ( col % 4 != 0 )
        strcat( buf1, "\r\n" );

    send_to_char( buf1, ch );
    return;
}



/*****************************************************************************
 Name:		show_help
 Purpose:	Displays help for many tables used in OLC.
 Called by:	olc interpreters.
 ****************************************************************************/
bool show_help( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char spell[MAX_INPUT_LENGTH];
    int cnt;

    argument = one_argument( argument, arg );
    one_argument( argument, spell );

    /*
     * Display syntax.
     */
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  ? [command]\r\n\r\n", ch );
	send_to_char( "[command]  [description]\r\n", ch );
	for (cnt = 0; help_table[cnt].command[0] != '\0'; cnt++)
	{
	    sprintf( buf, "%-10.10s -%s\r\n",
	        capitalize( help_table[cnt].command ),
		help_table[cnt].desc );
	    send_to_char( buf, ch );
	}
	return FALSE;
    }

    /*
     * Find the command, show changeable data.
     * ---------------------------------------
     */
    for (cnt = 0; *help_table[cnt].command; cnt++)
    {
        if (  arg[0] == help_table[cnt].command[0]
          && !str_prefix( arg, help_table[cnt].command ) )
	{
	    if ( help_table[cnt].structure == spec_table )
	    {
		show_spec_cmds( ch );
		return FALSE;
	    }
            else if ( help_table[cnt].structure == game_table )
	    {
		show_game_cmds( ch );
		return FALSE;
	    }
	    else
	    if ( help_table[cnt].structure == skill_table )
	    {

		if ( spell[0] == '\0' )
		{
		    send_to_char( "Syntax:  ? spells "
		        "[ignore/attack/defend/self/object/all]\r\n", ch );
		    return FALSE;
		}

		if ( !str_prefix( spell, "all" ) )
		    show_skill_cmds( ch, -1 );
		else if ( !str_prefix( spell, "ignore" ) )
		    show_skill_cmds( ch, TAR_IGNORE );
		else if ( !str_prefix( spell, "attack" ) )
		    show_skill_cmds( ch, TAR_CHAR_OFFENSIVE );
		else if ( !str_prefix( spell, "defend" ) )
		    show_skill_cmds( ch, TAR_CHAR_DEFENSIVE );
		else if ( !str_prefix( spell, "self" ) )
		    show_skill_cmds( ch, TAR_CHAR_SELF );
		else if ( !str_prefix( spell, "object" ) )
		    show_skill_cmds( ch, TAR_OBJ_INV );
		else
		    send_to_char( "Syntax:  ? spell "
		        "[ignore/attack/defend/self/object/all]\r\n", ch );
		    
		return FALSE;
	    }
	    else
	    {
		show_flag_cmds( ch, help_table[cnt].structure );
		return FALSE;
	    }
	}
    }

    show_help( ch, "" );
    return FALSE;
}



bool redit_mlist( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA	*pMobIndex;
    AREA_DATA		*pArea;
    char		buf  [ MAX_STRING_LENGTH   ];
    char		buf1 [ MAX_STRING_LENGTH*2 ];
    char		arg  [ MAX_INPUT_LENGTH    ];
    bool fAll, found;
    int vnum;
    int  col = 0;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  mlist <all/name>\r\n", ch );
	return FALSE;
    }

    pArea = ch->in_room->area;
    buf1[0] = '\0';
    fAll    = !str_cmp( arg, "all" );
    found   = FALSE;

    for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    if ( fAll || is_name( arg, pMobIndex->player_name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %-17.16s",
		    pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
		strcat( buf1, buf );
		if ( ++col % 3 == 0 )
		    strcat( buf1, "\r\n" );
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "Mobile(s) not found in this area.\r\n", ch);
	return FALSE;
    }

    if ( col % 3 != 0 )
	strcat( buf1, "\r\n" );

    send_to_char( buf1, ch );
    return FALSE;
}



bool redit_olist( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA	*pObjIndex;
    AREA_DATA		*pArea;
    char		buf  [ MAX_STRING_LENGTH   ];
    char		buf1 [ MAX_STRING_LENGTH*2 ];
    char		arg  [ MAX_INPUT_LENGTH    ];
    bool fAll, found;
    int vnum;
    int  col = 0;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  olist <all/name/item_type>\r\n", ch );
	return FALSE;
    }

    pArea = ch->in_room->area;
    buf1[0] = '\0';
    fAll    = !str_cmp( arg, "all" );
    found   = FALSE;

    for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if ( ( pObjIndex = get_obj_index( vnum ) ) )
	{
	    if ( fAll || is_name( arg, pObjIndex->name )
	    || flag_value( type_flags, arg ) == pObjIndex->item_type )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %-17.16s",
		    pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
		strcat( buf1, buf );
		if ( ++col % 3 == 0 )
		    strcat( buf1, "\r\n" );
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "Object(s) not found in this area.\r\n", ch);
	return FALSE;
    }

    if ( col % 3 != 0 )
	strcat( buf1, "\r\n" );

    send_to_char( buf1, ch );
    return FALSE;
}



bool redit_mshow( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  mshow <vnum>\r\n", ch );
	return FALSE;
    }

    if ( is_number( argument ) )
    {
	value = atoi( argument );
	if ( !( pMob = get_mob_index( value ) ))
	{
	    send_to_char( "REdit:  That mobile does not exist.\r\n", ch );
	    return FALSE;
	}

	ch->desc->pEdit = (void *)pMob;
    }
 
    medit_show( ch, argument );
    ch->desc->pEdit = (void *)ch->in_room;
    return FALSE; 
}



bool redit_oshow( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  oshow <vnum>\r\n", ch );
	return FALSE;
    }

    if ( is_number( argument ) )
    {
	value = atoi( argument );
	if ( !( pObj = get_obj_index( value ) ))
	{
	    send_to_char( "REdit:  That object does not exist.\r\n", ch );
	    return FALSE;
	}

	ch->desc->pEdit = (void *)pObj;
    }
 
    oedit_show( ch, argument );
    ch->desc->pEdit = (void *)ch->in_room;
    return FALSE; 
}



/*****************************************************************************
 Name:		check_range( lower vnum, upper vnum )
 Purpose:	Ensures the range spans only one area.
 Called by:	aedit_vnum(olc_act.c).
 ****************************************************************************/
bool check_range( int lower, int upper )
{
    AREA_DATA *pArea;
    int cnt = 0;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	/*
	 * lower < area < upper
	 */
	if ( ( lower <= pArea->lvnum && upper >= pArea->lvnum )
	||   ( upper >= pArea->uvnum && lower <= pArea->uvnum ) )
	    cnt++;

	if ( cnt > 1 )
	    return FALSE;
    }
    return TRUE;
}



AREA_DATA *get_vnum_area( int vnum )
{
    AREA_DATA *pArea;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
        if ( vnum >= pArea->lvnum
          && vnum <= pArea->uvnum )
            return pArea;
    }

    return 0;
}



/*
 * Area Editor Functions.
 */
bool aedit_show( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char buf  [MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    sprintf( buf, "Name:     [%5d] %s\r\n", pArea->vnum, pArea->name );
    send_to_char( buf, ch );

    sprintf( buf, "Recall:   [%5d] %s\r\n", pArea->recall,
	get_room_index( pArea->recall )
	? get_room_index( pArea->recall )->name : "none" );
    send_to_char( buf, ch );

    sprintf( buf, "File:     %s\r\n", pArea->filename );
    send_to_char( buf, ch );

    sprintf( buf, "Vnums:    [%d-%d]\r\n", pArea->lvnum, pArea->uvnum );
    send_to_char( buf, ch );

    sprintf( buf, "Age:      [%d]\r\n",	pArea->age );
    send_to_char( buf, ch );

    sprintf( buf, "Players:  [%d]\r\n", pArea->nplayer );
    send_to_char( buf, ch );

    sprintf( buf, "Security: [%d]\r\n", pArea->security );
    send_to_char( buf, ch );

    sprintf( buf, "Builders: [%s]\r\n", pArea->builders );
    send_to_char( buf, ch );

    sprintf( buf, "Flags:    [%s]\r\n", flag_string( area_flags, pArea->area_flags ) );
    send_to_char( buf, ch );

    return FALSE;
}



bool aedit_reset( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    reset_area( pArea );
    send_to_char( "Area reset.\r\n", ch );

    return FALSE;
}



bool aedit_create( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    if ( top_area >= INT_MAX )	/* OLC 1.1b */
    {
	send_to_char( "We're out of vnums for new areas.\r\n", ch );
	return FALSE;
    }

    pArea               =   new_area();
    area_last->next     =   pArea;
    area_last		=   pArea;	/* Thanks, Walker. */
    ch->desc->pEdit     =   (void *)pArea;

    SET_BIT( pArea->area_flags, AREA_ADDED );
    send_to_char( "Area Created.\r\n", ch );
    return TRUE;	/* OLC 1.1b */
}



bool aedit_name( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:   name [$name]\r\n", ch );
	return FALSE;
    }

    free_string( pArea->name );
    pArea->name = str_dup( argument );

    send_to_char( "Name set.\r\n", ch );
    return TRUE;
}



bool aedit_file( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char file[MAX_STRING_LENGTH];
    int i, length;

    EDIT_AREA(ch, pArea);

    one_argument( argument, file );	/* Forces Lowercase */

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  filename [$file]\r\n", ch );
	return FALSE;
    }

    /*
     * Simple Syntax Check.
     */
    length = strlen( argument );
    if ( length > 8 )
    {
	send_to_char( "No more than eight characters allowed.\r\n", ch );
	return FALSE;
    }

    /*
     * Allow only letters and numbers.
     */
    for ( i = 0; i < length; i++ )
    {
	if ( !isalnum( file[i] ) )
	{
	    send_to_char( "Only letters and numbers are valid.\r\n", ch );
	    return FALSE;
	}
    }    

    free_string( pArea->filename );
    strcat( file, ".are" );
    pArea->filename = str_dup( file );

    send_to_char( "Filename set.\r\n", ch );
    return TRUE;
}



bool aedit_age( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char age[MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    one_argument( argument, age );

    if ( !is_number( age ) || age[0] == '\0' )
    {
	send_to_char( "Syntax:  age [#age]\r\n", ch );
	return FALSE;
    }

    pArea->age = atoi( age );

    send_to_char( "Age set.\r\n", ch );
    return TRUE;
}



bool aedit_recall( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char room[MAX_STRING_LENGTH];
    int  value;

    EDIT_AREA(ch, pArea);

    one_argument( argument, room );

    if ( !is_number( argument ) || argument[0] == '\0' )
    {
	send_to_char( "Syntax:  recall [#rvnum]\r\n", ch );
	return FALSE;
    }

    value = atoi( room );

    if ( !get_room_index( value ) )
    {
	send_to_char( "AEdit:  Room vnum does not exist.\r\n", ch );
	return FALSE;
    }

    pArea->recall = value;

    send_to_char( "Recall set.\r\n", ch );
    return TRUE;
}



bool aedit_security( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char sec[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int  value;

    EDIT_AREA(ch, pArea);

    one_argument( argument, sec );

    if ( !is_number( sec ) || sec[0] == '\0' )
    {
	send_to_char( "Syntax:  security [#level]\r\n", ch );
	return FALSE;
    }

    value = atoi( sec );

    if ( value > ch->pcdata->security || value < 0 )
    {
	if ( ch->pcdata->security != 0 )
	{
	    sprintf( buf, "Security is 0-%d.\r\n", ch->pcdata->security );
	    send_to_char( buf, ch );
	}
	else
	    send_to_char( "Security is 0 only.\r\n", ch );
	return FALSE;
    }

    pArea->security = value;

    send_to_char( "Security set.\r\n", ch );
    return TRUE;
}



bool aedit_builder( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char name[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    one_argument( argument, name );

    if ( name[0] == '\0' )
    {
	send_to_char( "Syntax:  builder [$name]  -toggles builder\r\n", ch );
	send_to_char( "Syntax:  builder All      -allows everyone\r\n", ch );
	return FALSE;
    }

    name[0] = UPPER( name[0] );

    if ( strstr( pArea->builders, name ) != '\0' )
    {
	pArea->builders = string_replace( pArea->builders, name, "\0" );
	pArea->builders = string_unpad( pArea->builders );

	if ( pArea->builders[0] == '\0' )
	{
	    free_string( pArea->builders );
	    pArea->builders = str_dup( "None" );
	}
	send_to_char( "Builder removed.\r\n", ch );
	return TRUE;
    }
    else
    {
	buf[0] = '\0';
	if ( strstr( pArea->builders, "None" ) != '\0' )
	{
	    pArea->builders = string_replace( pArea->builders, "None", "\0" );
	    pArea->builders = string_unpad( pArea->builders );
	}

	if (pArea->builders[0] != '\0' )
	{
	    strcat( buf, pArea->builders );
	    strcat( buf, " " );
	}
	strcat( buf, name );
	free_string( pArea->builders );
	pArea->builders = string_proper( str_dup( buf ) );

	send_to_char( "Builder added.\r\n", ch );
	return TRUE;
    }

    return FALSE;
}



bool aedit_vnum( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char lower[MAX_STRING_LENGTH];
    char upper[MAX_STRING_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    argument = one_argument( argument, lower );
    one_argument( argument, upper );

    if ( !is_number( lower ) || lower[0] == '\0'
    || !is_number( upper ) || upper[0] == '\0' )
    {
	send_to_char( "Syntax:  vnum [#lower] [#upper]\r\n", ch );
	return FALSE;
    }

    if ( ( ilower = atoi( lower ) ) > ( iupper = atoi( upper ) ) )
    {
	send_to_char( "AEdit:  Upper must be larger then lower.\r\n", ch );
	return FALSE;
    }
    
    /* OLC 1.1b */
    if ( ilower <= 0 || ilower >= INT_MAX || iupper <= 0 || iupper >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "AEdit: vnum must be between 0 and %d.\r\n", INT_MAX );
	send_to_char( output, ch );
	return FALSE;
    }

    if ( !check_range( ilower, iupper ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\r\n", ch );
	return FALSE;
    }

    if ( get_vnum_area( ilower )
    && get_vnum_area( ilower ) != pArea )
    {
	send_to_char( "AEdit:  Lower vnum already assigned.\r\n", ch );
	return FALSE;
    }

    pArea->lvnum = ilower;
    send_to_char( "Lower vnum set.\r\n", ch );

    if ( get_vnum_area( iupper )
    && get_vnum_area( iupper ) != pArea )
    {
	send_to_char( "AEdit:  Upper vnum already assigned.\r\n", ch );
	return TRUE;	/* The lower value has been set. */
    }

    pArea->uvnum = iupper;
    send_to_char( "Upper vnum set.\r\n", ch );

    return TRUE;
}



bool aedit_lvnum( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char lower[MAX_STRING_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    one_argument( argument, lower );

    if ( !is_number( lower ) || lower[0] == '\0' )
    {
	send_to_char( "Syntax:  lvnum [#lower]\r\n", ch );
	return FALSE;
    }

    if ( ( ilower = atoi( lower ) ) > ( iupper = pArea->uvnum ) )
    {
	send_to_char( "AEdit:  Value must be less than the uvnum.\r\n", ch );
	return FALSE;
    }
    
    /* OLC 1.1b */
    if ( ilower <= 0 || ilower >= INT_MAX || iupper <= 0 || iupper >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "AEdit: vnum must be between 0 and %d.\r\n", INT_MAX );
	send_to_char( output, ch );
	return FALSE;
    }

    if ( !check_range( ilower, iupper ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\r\n", ch );
	return FALSE;
    }

    if ( get_vnum_area( ilower )
    && get_vnum_area( ilower ) != pArea )
    {
	send_to_char( "AEdit:  Lower vnum already assigned.\r\n", ch );
	return FALSE;
    }

    pArea->lvnum = ilower;
    send_to_char( "Lower vnum set.\r\n", ch );
    return TRUE;
}



bool aedit_uvnum( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char upper[MAX_STRING_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    one_argument( argument, upper );

    if ( !is_number( upper ) || upper[0] == '\0' )
    {
	send_to_char( "Syntax:  uvnum [#upper]\r\n", ch );
	return FALSE;
    }

    if ( ( ilower = pArea->lvnum ) > ( iupper = atoi( upper ) ) )
    {
	send_to_char( "AEdit:  Upper must be larger then lower.\r\n", ch );
	return FALSE;
    }
    
    /* OLC 1.1b */
    if ( ilower <= 0 || ilower >= INT_MAX || iupper <= 0 || iupper >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "AEdit: vnum must be between 0 and %d.\r\n", INT_MAX );
	send_to_char( output, ch );
	return FALSE;
    }

    if ( !check_range( ilower, iupper ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\r\n", ch );
	return FALSE;
    }

    if ( get_vnum_area( iupper )
    && get_vnum_area( iupper ) != pArea )
    {
	send_to_char( "AEdit:  Upper vnum already assigned.\r\n", ch );
	return FALSE;
    }

    pArea->uvnum = iupper;
    send_to_char( "Upper vnum set.\r\n", ch );

    return TRUE;
}



/*
 * Room Editor Functions.
 */
bool redit_show( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA	*pRoom;
    char		buf  [MAX_STRING_LENGTH];
    char		buf1 [2*MAX_STRING_LENGTH];
    OBJ_DATA		*obj;
    CHAR_DATA		*rch;
    int			door;
    bool		fcnt;
    
    EDIT_ROOM(ch, pRoom);

    buf1[0] = '\0';
    
    sprintf( buf, "Description:\r\n%s", pRoom->description );
    strcat( buf1, buf );

    sprintf( buf, "Name:       [%s]\r\nArea:       [%5d] %s\r\n",
	    pRoom->name, pRoom->area->vnum, pRoom->area->name );
    strcat( buf1, buf );

    sprintf( buf, "Vnum:       [%5d]\r\nSector:     [%s]\r\n",
	    pRoom->vnum, flag_string( sector_flags, pRoom->sector_type ) );
    strcat( buf1, buf );

    sprintf( buf, "Room flags: [%s]\r\n",
	    flag_string( room_flags, pRoom->room_flags ) );
    strcat( buf1, buf );

    if ( pRoom->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "Desc Kwds:  [" );
	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, " " );
	}
	strcat( buf1, "]\r\n" );
    }

    strcat( buf1, "Characters: [" );
    fcnt = FALSE;
    for ( rch = pRoom->people; rch; rch = rch->next_in_room )
    {
	one_argument( rch->name, buf );
	strcat( buf1, buf );
	strcat( buf1, " " );
	fcnt = TRUE;
    }

    if ( fcnt )
    {
	int end;

	end = strlen(buf1) - 1;
	buf1[end] = ']';
	strcat( buf1, "\r\n" );
    }
    else
	strcat( buf1, "none]\r\n" );

    strcat( buf1, "Objects:    [" );
    fcnt = FALSE;
    for ( obj = pRoom->contents; obj; obj = obj->next_content )
    {
	one_argument( obj->name, buf );
	strcat( buf1, buf );
	strcat( buf1, " " );
	fcnt = TRUE;
    }

    if ( fcnt )
    {
	int end;

	end = strlen(buf1) - 1;
	buf1[end] = ']';
	strcat( buf1, "\r\n" );
    }
    else
	strcat( buf1, "none]\r\n" );

    for ( door = 0; door < MAX_DIR; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = pRoom->exit[door] ) )
	{
	    char word[MAX_INPUT_LENGTH];
	    char reset_state[MAX_STRING_LENGTH];
	    char *state;
	    int i, length;

	    sprintf( buf, "-%-5s to [%5d] Key: [%5d]",
		capitalize(dir_name[door]),
		pexit->to_room ? pexit->to_room->vnum : 0,
		pexit->key );
	    strcat( buf1, buf );

	    /*
	     * Format up the exit info.
	     * Capitalize all flags that are not part of the reset info.
	     */
	    strcpy( reset_state, flag_string( exit_flags, pexit->rs_flags ) );
	    state = flag_string( exit_flags, pexit->exit_info );
	    strcat( buf1, " Exit flags: [" );
	    for (; ;)
	    {
		state = one_argument( state, word );

		if ( word[0] == '\0' )
		{
		    int end;

		    end = strlen(buf1) - 1;
		    buf1[end] = ']';
		    strcat( buf1, "\r\n" );
		    break;
		}

		if ( str_infix( word, reset_state ) )
		{
		    length = strlen(word);
		    for (i = 0; i < length; i++)
			word[i] = toupper(word[i]);
		}
		strcat( buf1, word );
		strcat( buf1, " " );
	    }

	    if ( pexit->keyword && pexit->keyword[0] != '\0' )
	    {
		sprintf( buf, "Kwds: [%s]\r\n", pexit->keyword );
		strcat( buf1, buf );
	    }
	    if ( pexit->description && pexit->description[0] != '\0' )
	    {
		sprintf( buf, "%s", pexit->description );
		strcat( buf1, buf );
	    }
	}
    }

    send_to_char( buf1, ch );
    return FALSE;
}


/* OLC 1.1b */
/*****************************************************************************
 Name:		change_exit
 Purpose:	Command interpreter for changing exits.
 Called by:	redit_<dir>.  This is a local function.
 ****************************************************************************/
bool change_exit( CHAR_DATA *ch, char *argument, int door )
{
    ROOM_INDEX_DATA *pRoom;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char total_arg[MAX_STRING_LENGTH];
    int  rev;
    int  value = 0;

    EDIT_ROOM(ch, pRoom);

    /* Often used data. */
    rev = rev_dir[door];
    
    if ( argument[0] == '\0' )
    {
	do_help( ch, "EXIT" );
	return FALSE;
    }

    /*
     * Now parse the arguments.
     */
    strcpy( total_arg, argument );
    argument = one_argument( argument, command );
    one_argument( argument, arg );

    if ( !str_cmp( command, "delete" ) )
    {	
	if ( !pRoom->exit[door] )
	{
	    send_to_char( "REdit:  Exit does not exist.\r\n", ch );
	    return FALSE;
	}

	/*
	 * Remove To Room Exit.
	 */
	if ( pRoom->exit[door]->to_room->exit[rev] )
	{
	    free_exit( pRoom->exit[door]->to_room->exit[rev] );
	    pRoom->exit[door]->to_room->exit[rev] = NULL;
	}

	/*
	 * Remove this exit.
	 */
	free_exit( pRoom->exit[door] );
	pRoom->exit[door] = NULL;

	send_to_char( "Exit unlinked.\r\n", ch );
	return TRUE;
    }

    /*
     * Create a two-way exit.
     */
    if ( !str_cmp( command, "link" ) )
    {
	EXIT_DATA	*pExit;
	ROOM_INDEX_DATA	*pLinkRoom;

	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax:  [direction] link [vnum]\r\n", ch );
	    return FALSE;
	}

	if ( !( pLinkRoom = get_room_index( atoi(arg) ) ) )
	{
	    send_to_char( "REdit:  Non-existant room.\r\n", ch );
	    return FALSE;
	}

	if ( !IS_BUILDER( ch, pLinkRoom->area ) )
	{
	    send_to_char( "REdit:  Cannot link to that area.\r\n", ch );
	    return FALSE;
	}

	if ( pLinkRoom->exit[rev] )
	{
	    send_to_char( "REdit:  Remote side's exit exists.\r\n", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )		/* No exit.		*/
	    pRoom->exit[door] = new_exit();

	pRoom->exit[door]->to_room = pLinkRoom;	/* Assign data.		*/
	pRoom->exit[door]->vnum = value;

	pExit			= new_exit();	/* No remote exit.	*/

	pExit->to_room		= ch->in_room;	/* Assign data.		*/
	pExit->vnum		= ch->in_room->vnum;

	pLinkRoom->exit[rev]	= pExit;	/* Link exit to room.	*/

	send_to_char( "Two-way link established.\r\n", ch );
	return TRUE;
    }

    /*
     * Create room and make two-way exit.
     */
    if ( !str_cmp( command, "dig" ) )
    {
	char buf[MAX_INPUT_LENGTH];
	
	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax: [direction] dig <vnum>\r\n", ch );
	    return FALSE;
	}
	
	redit_create( ch, arg );		/* Create the room.	*/
	sprintf( buf, "link %s", arg );
	change_exit( ch, buf, door);		/* Create the exits.	*/
	return TRUE;
    }

    /*
     * Create one-way exit.
     */
    if ( !str_cmp( command, "room" ) )
    {
	ROOM_INDEX_DATA *pLinkRoom;

	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax:  [direction] room [vnum]\r\n", ch );
	    return FALSE;
	}

	if ( !( pLinkRoom = get_room_index( atoi( arg ) ) ) )
	{
	    send_to_char( "REdit:  Non-existant room.\r\n", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	    pRoom->exit[door] = new_exit();

	pRoom->exit[door]->to_room = pLinkRoom;
	pRoom->exit[door]->vnum = value;

	send_to_char( "One-way link established.\r\n", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "remove" ) )
    {
	if ( arg[0] == '\0' )
	{
	    send_to_char( "Syntax:  [direction] remove [key/name/desc]\r\n", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	{
	    send_to_char( "REdit:  Exit does not exist.\r\n", ch );
	    return FALSE;
	}

	if ( !str_cmp( argument, "key" ) )
	{
	    pRoom->exit[door]->key = 0;
            send_to_char( "Exit key removed.\r\n", ch );                        
            return TRUE;
	}

	if ( !str_cmp( argument, "name" ) )
	{
	    free_string( pRoom->exit[door]->keyword );
	    pRoom->exit[door]->keyword = &str_empty[0];
            send_to_char( "Exit name removed.\r\n", ch );                        
            return TRUE;
	}

	if ( argument[0] == 'd' && !str_prefix( argument, "description" ) )
	{
	    free_string( pRoom->exit[door]->description );
	    pRoom->exit[door]->description = &str_empty[0];
            send_to_char( "Exit description removed.\r\n", ch );                        
            return TRUE;
	}

	send_to_char( "Syntax:  [direction] remove [key/name/desc]\r\n", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "key" ) )
    {
	OBJ_INDEX_DATA *pObjIndex;

	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax:  [direction] key [vnum]\r\n", ch );
	    return FALSE;
	}

	if ( !( pObjIndex = get_obj_index( atoi( arg ) ) ) )
	{
	    send_to_char( "REdit:  Item does not exist.\r\n", ch );
	    return FALSE;
	}

	if ( pObjIndex->item_type != ITEM_KEY )
	{
	    send_to_char( "REdit:  Item is not a key.\r\n", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	    pRoom->exit[door] = new_exit();

	pRoom->exit[door]->key = pObjIndex->vnum;

	send_to_char( "Exit key set.\r\n", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "name" ) )
    {
	if ( arg[0] == '\0' )
	{
	    send_to_char( "Syntax:  [direction] name [string]\r\n", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	    pRoom->exit[door] = new_exit();

	free_string( pRoom->exit[door]->keyword );
	pRoom->exit[door]->keyword = str_dup( argument );

	send_to_char( "Exit name set.\r\n", ch );
	return TRUE;
    }

    if ( command[0] == 'd' && !str_prefix( command, "description" ) )
    {
	if ( arg[0] == '\0' )
	{
	    if ( !pRoom->exit[door] )
	        pRoom->exit[door] = new_exit();

	    string_append( ch, &pRoom->exit[door]->description );
	    return TRUE;
	}

	send_to_char( "Syntax:  [direction] desc\r\n", ch );
	return FALSE;
    }

    /*
     * Set the exit flags, needs full argument.
     * ----------------------------------------
     */
    if ( ( value = flag_value( exit_flags, total_arg ) ) != NO_FLAG )
    {
	ROOM_INDEX_DATA *pToRoom;

	/*
	 * Create an exit if none exists.
	 */
	if ( !pRoom->exit[door] )
	    pRoom->exit[door] = new_exit();

	/*
	 * Set door bits for this room.
	 */
	TOGGLE_BIT(pRoom->exit[door]->rs_flags, value);
	pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;

	/*
	 * Set door bits of connected room.
	 * Skip one-way exits and non-existant rooms.
	 */
	if ( ( pToRoom = pRoom->exit[door]->to_room ) && pToRoom->exit[rev] )
	{
	    TOGGLE_BIT(pToRoom->exit[rev]->rs_flags, value);
	    pToRoom->exit[rev]->exit_info =  pToRoom->exit[rev]->rs_flags;
	}

	send_to_char( "Exit flag toggled.\r\n", ch );
	return TRUE;
    }

    return FALSE;
}



bool redit_north( CHAR_DATA *ch, char *argument )
{
    if ( change_exit( ch, argument, DIR_NORTH ) )
	return TRUE;

    return FALSE;
}



bool redit_south( CHAR_DATA *ch, char *argument )
{
    if ( change_exit( ch, argument, DIR_SOUTH ) )
	return TRUE;

    return FALSE;
}



bool redit_east( CHAR_DATA *ch, char *argument )
{
    if ( change_exit( ch, argument, DIR_EAST ) )
	return TRUE;

    return FALSE;
}



bool redit_west( CHAR_DATA *ch, char *argument )
{
    if ( change_exit( ch, argument, DIR_WEST ) )
	return TRUE;

    return FALSE;
}



bool redit_up( CHAR_DATA *ch, char *argument )
{
    if ( change_exit( ch, argument, DIR_UP ) )
	return TRUE;

    return FALSE;
}



bool redit_down( CHAR_DATA *ch, char *argument )
{
    if ( change_exit( ch, argument, DIR_DOWN ) )
	return TRUE;

    return FALSE;
}


/* OLC 1.1b */
bool redit_move( CHAR_DATA *ch, char *argument )
{
    interpret( ch, argument );
    return FALSE;
}



bool redit_ed( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;
    EXTRA_DESCR_DATA *ed;
    char command[MAX_INPUT_LENGTH];
    char keyword[MAX_INPUT_LENGTH];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument( argument, command );
    one_argument( argument, keyword );

    if ( command[0] == '\0' || keyword[0] == '\0' )
    {
	send_to_char( "Syntax:  ed add [keyword]\r\n", ch );
	send_to_char( "         ed edit [keyword]\r\n", ch );
	send_to_char( "         ed delete [keyword]\r\n", ch );
	send_to_char( "         ed format [keyword]\r\n", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "add" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed add [keyword]\r\n", ch );
	    return FALSE;
	}

	ed			=   new_extra_descr();
	ed->keyword		=   str_dup( keyword );
	ed->description		=   str_dup( "" );
	ed->next		=   pRoom->extra_descr;
	pRoom->extra_descr	=   ed;

	string_append( ch, &ed->description );

	return TRUE;
    }


    if ( !str_cmp( command, "edit" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed edit [keyword]\r\n", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\r\n", ch );
	    return FALSE;
	}

	string_append( ch, &ed->description );

	return TRUE;
    }


    if ( !str_cmp( command, "delete" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;

	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed delete [keyword]\r\n", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	    ped = ed;
	}

	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\r\n", ch );
	    return FALSE;
	}

	if ( !ped )
	    pRoom->extra_descr = ed->next;
	else
	    ped->next = ed->next;

	free_extra_descr( ed );

	send_to_char( "Extra description deleted.\r\n", ch );
	return TRUE;
    }


    if ( !str_cmp( command, "format" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed format [keyword]\r\n", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\r\n", ch );
	    return FALSE;
	}

	/* OLC 1.1b */
	if ( strlen(ed->description) >= (MAX_STRING_LENGTH - 4) )
	{
	    send_to_char( "String too long to be formatted.\r\n", ch );
	    return FALSE;
	}

	ed->description = format_string( ed->description );

	send_to_char( "Extra description formatted.\r\n", ch );
	return TRUE;
    }

    redit_ed( ch, "" );
    return FALSE;
}



bool redit_create( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    int value;
    int iHash;
    
    EDIT_ROOM(ch, pRoom);

    value = atoi( argument );

    /* OLC 1.1b */
    if ( argument[0] == '\0' || value <= 0 || value >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "Syntax:  create [0 < vnum < %d]\r\n", INT_MAX );
	send_to_char( output, ch );
	return FALSE;
    }

    if ( argument[0] == '\0' || value <= 0 )
    {
	send_to_char( "Syntax:  create [vnum > 0]\r\n", ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );
    if ( !pArea )
    {
	send_to_char( "REdit:  That vnum is not assigned an area.\r\n", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "REdit:  Vnum in an area you cannot build in.\r\n", ch );
	return FALSE;
    }

    if ( get_room_index( value ) )
    {
	send_to_char( "REdit:  Room vnum already exists.\r\n", ch );
	return FALSE;
    }

    pRoom			= new_room_index();
    pRoom->area			= pArea;
    pRoom->vnum			= value;

    if ( value > top_vnum_room )
        top_vnum_room = value;

    iHash			= value % MAX_KEY_HASH;
    pRoom->next			= room_index_hash[iHash];
    room_index_hash[iHash]	= pRoom;
    ch->desc->pEdit		= (void *)pRoom;

    send_to_char( "Room created.\r\n", ch );
    return TRUE;
}



bool redit_name( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [name]\r\n", ch );
	return FALSE;
    }

    free_string( pRoom->name );
    pRoom->name = str_dup( argument );

    send_to_char( "Name set.\r\n", ch );
    return TRUE;
}



bool redit_desc( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pRoom->description );
	return TRUE;
    }

    send_to_char( "Syntax:  desc\r\n", ch );
    return FALSE;
}




bool redit_format( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    /* OLC 1.1b */
    if ( strlen(pRoom->description) >= (MAX_STRING_LENGTH - 4) )
    {
	send_to_char( "String too long to be formatted.\r\n", ch );
	return FALSE;
    }

    pRoom->description = format_string( pRoom->description );

    send_to_char( "String formatted.\r\n", ch );
    return TRUE;
}



bool redit_mreset( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA	*pRoom;
    MOB_INDEX_DATA	*pMobIndex;
    CHAR_DATA		*newmob;
    char		arg [ MAX_INPUT_LENGTH ];

    RESET_DATA		*pReset;
    char		output [ MAX_STRING_LENGTH ];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
	send_to_char ( "Syntax:  mreset <vnum> <max #>\r\n", ch );
	return FALSE;
    }

    if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
    {
	send_to_char( "REdit: No mobile has that vnum.\r\n", ch );
	return FALSE;
    }

    if ( pMobIndex->area != pRoom->area )
    {
	send_to_char( "REdit: No such mobile in this area.\r\n", ch );
	return FALSE;
    }

    /*
     * Create the mobile reset.
     */
    pReset = new_reset_data();
    pReset->command	= 'M';
    pReset->arg1	= pMobIndex->vnum;
    pReset->arg2	= is_number( argument ) ? atoi( argument ) : MAX_MOB;
    pReset->arg3	= pRoom->vnum;
    add_reset( pRoom, pReset, 0/* Last slot*/ );

    /*
     * Create the mobile.
     */
    newmob = create_mobile( pMobIndex );
    char_to_room( newmob, pRoom );

    sprintf( output, "%s (%d) has been loaded and added to resets.\r\n"
	"There will be a maximum of %d loaded to this room.\r\n",
	capitalize( pMobIndex->short_descr ),
	pMobIndex->vnum,
	pReset->arg2 );
    send_to_char( output, ch );
    act( "$n has created $N!", ch, NULL, newmob, TO_ROOM );
    return TRUE;
}

struct wear_type
{
    int	wear_loc;
    int	wear_bit;
};



const struct wear_type wear_table[] =
{
    {	WEAR_NONE,	ITEM_TAKE		},
    {	WEAR_LIGHT,	ITEM_LIGHT		},
    {	WEAR_FINGER_L,	ITEM_WEAR_FINGER	},
    {	WEAR_FINGER_R,	ITEM_WEAR_FINGER	},
    {	WEAR_NECK_1,	ITEM_WEAR_NECK		},
    {	WEAR_NECK_2,	ITEM_WEAR_NECK		},
    {	WEAR_BODY,	ITEM_WEAR_BODY		},
    {	WEAR_HEAD,	ITEM_WEAR_HEAD		},
    {	WEAR_LEGS,	ITEM_WEAR_LEGS		},
    {	WEAR_FEET,	ITEM_WEAR_FEET		},
    {	WEAR_HANDS,	ITEM_WEAR_HANDS		},
    {	WEAR_ARMS,	ITEM_WEAR_ARMS		},
    {	WEAR_SHIELD,	ITEM_WEAR_SHIELD	},
    {	WEAR_ABOUT,	ITEM_WEAR_ABOUT		},
    {	WEAR_WAIST,	ITEM_WEAR_WAIST		},
    {	WEAR_WRIST_L,	ITEM_WEAR_WRIST		},
    {	WEAR_WRIST_R,	ITEM_WEAR_WRIST		},
    {	WEAR_WIELD,	ITEM_WIELD		},
    {	WEAR_HOLD,	ITEM_HOLD		},
    {	NO_FLAG,	NO_FLAG			}
};



/*****************************************************************************
 Name:		wear_loc
 Purpose:	Returns the location of the bit that matches the count.
 		1 = first match, 2 = second match etc.
 Called by:	oedit_reset(olc_act.c).
 ****************************************************************************/
int wear_loc(int bits, int count)
{
    int flag;
 
    for (flag = 0; wear_table[flag].wear_bit != NO_FLAG; flag++)
    {
        if ( IS_SET(bits, wear_table[flag].wear_bit) && --count < 1)
            return wear_table[flag].wear_loc;
    }
 
    return NO_FLAG;
}



/*****************************************************************************
 Name:		wear_bit
 Purpose:	Converts a wear_loc into a bit.
 Called by:	redit_oreset(olc_act.c).
 ****************************************************************************/
int wear_bit(int loc)
{
    int flag;
 
    for (flag = 0; wear_table[flag].wear_loc != NO_FLAG; flag++)
    {
        if ( loc == wear_table[flag].wear_loc )
            return wear_table[flag].wear_bit;
    }
 
    return 0;
}



bool redit_oreset( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA	*pRoom;
    OBJ_INDEX_DATA	*pObjIndex;
    OBJ_DATA		*newobj;
    OBJ_DATA		*to_obj;
    CHAR_DATA		*to_mob;
    char		arg1 [ MAX_INPUT_LENGTH ];
    char		arg2 [ MAX_INPUT_LENGTH ];
    int			olevel = 0;

    RESET_DATA		*pReset;
    char		output [ MAX_STRING_LENGTH ];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
	send_to_char ( "Syntax:  oreset <vnum> <args>\r\n", ch );
	send_to_char ( "        -no_args               = into room\r\n", ch );
	send_to_char ( "        -<obj_name>            = into obj\r\n", ch );
	send_to_char ( "        -<mob_name> <wear_loc> = into mob\r\n", ch );
	return FALSE;
    }

    if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
    {
	send_to_char( "REdit: No object has that vnum.\r\n", ch );
	return FALSE;
    }

    if ( pObjIndex->area != pRoom->area )
    {
	send_to_char( "REdit: No such object in this area.\r\n", ch );
	return FALSE;
    }

    /*
     * Load into room.
     */
    if ( arg2[0] == '\0' )
    {
	pReset		= new_reset_data();
	pReset->command	= 'O';
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= 0;
	pReset->arg3	= pRoom->vnum;
	add_reset( pRoom, pReset, 0/* Last slot*/ );

	newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
	obj_to_room( newobj, pRoom );

	sprintf( output, "%s (%d) has been loaded and added to resets.\r\n",
	    capitalize( pObjIndex->short_descr ),
	    pObjIndex->vnum );
	send_to_char( output, ch );
    }
    else
    /*
     * Load into object's inventory.
     */
    if ( argument[0] == '\0'
    && ( ( to_obj = get_obj_list( ch, arg2, pRoom->contents ) ) != NULL ) )
    {
	pReset		= new_reset_data();
	pReset->command	= 'P';
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= 0;
	pReset->arg3	= to_obj->pIndexData->vnum;
	add_reset( pRoom, pReset, 0/* Last slot*/ );

	newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
	newobj->cost = 0;
	obj_to_obj( newobj, to_obj );

	sprintf( output, "%s (%d) has been loaded into "
	    "%s (%d) and added to resets.\r\n",
	    capitalize( newobj->short_descr ),
	    newobj->pIndexData->vnum,
	    to_obj->short_descr,
	    to_obj->pIndexData->vnum );
	send_to_char( output, ch );
    }
    else
    /*
     * Load into mobile's inventory.
     */
    if ( ( to_mob = get_char_room( ch, arg2 ) ) != NULL )
    {
	int	wear_loc;

	/*
	 * Make sure the location on mobile is valid.
	 */
	if ( (wear_loc = flag_value( wear_loc_flags, argument )) == NO_FLAG )
	{
	    send_to_char( "REdit: Invalid wear_loc.  '? wear-loc'\r\n", ch );
	    return FALSE;
	}

	/*
	 * Disallow loading a sword(WEAR_WIELD) into WEAR_HEAD.
	 */
	if ( !IS_SET( pObjIndex->wear_flags, wear_bit(wear_loc) ) )
	{
	    sprintf( output,
	        "%s (%d) has wear flags: [%s]\r\n",
	        capitalize( pObjIndex->short_descr ),
	        pObjIndex->vnum,
		flag_string( wear_flags, pObjIndex->wear_flags ) );
	    send_to_char( output, ch );
	    return FALSE;
	}

	/*
	 * Can't load into same position.
	 */
	if ( get_eq_char( to_mob, wear_loc ) )
	{
	    send_to_char( "REdit:  Object already equipped.\r\n", ch );
	    return FALSE;
	}

	pReset		= new_reset_data();
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= wear_loc;
	if ( pReset->arg2 == WEAR_NONE )
	    pReset->command = 'G';
	else
	    pReset->command = 'E';
	pReset->arg3	= wear_loc;

	add_reset( pRoom, pReset, 0/* Last slot*/ );

	olevel  = URANGE( 0, to_mob->level - 2, LEVEL_HERO );
        newobj = create_object( pObjIndex, number_fuzzy( olevel ) );

	if ( to_mob->pIndexData->pShop )	/* Shop-keeper? */
	{
	    switch ( pObjIndex->item_type )
	    {
	    default:		olevel = 0;				break;
	    case ITEM_PILL:	olevel = number_range(  0, 10 );	break;
	    case ITEM_POTION:	olevel = number_range(  0, 10 );	break;
	    case ITEM_SCROLL:	olevel = number_range(  5, 15 );	break;
	    case ITEM_WAND:	olevel = number_range( 10, 20 );	break;
	    case ITEM_STAFF:	olevel = number_range( 15, 25 );	break;
	    case ITEM_ARMOR:	olevel = number_range(  5, 15 );	break;
	    case ITEM_WEAPON:	if ( pReset->command == 'G' )
	    			    olevel = number_range( 5, 15 );
				else
				    olevel = number_fuzzy( olevel );
		break;
	    }

	    newobj = create_object( pObjIndex, olevel );
	    if ( pReset->arg2 == WEAR_NONE )
		SET_BIT( newobj->extra_flags, ITEM_INVENTORY );
	}
	else
	    newobj = create_object( pObjIndex, number_fuzzy( olevel ) );

	obj_to_char( newobj, to_mob );
	if ( pReset->command == 'E' )
	    equip_char( to_mob, newobj, pReset->arg3 );

	sprintf( output, "%s (%d) has been loaded "
	    "%s of %s (%d) and added to resets.\r\n",
	    capitalize( pObjIndex->short_descr ),
	    pObjIndex->vnum,
	    flag_string( wear_loc_strings, pReset->arg3 ),
	    to_mob->short_descr,
	    to_mob->pIndexData->vnum );
	send_to_char( output, ch );
    }
    else	/* Display Syntax */
    {
	send_to_char( "REdit:  That mobile isn't here.\r\n", ch );
	return FALSE;
    }

    act( "$n has created $p!", ch, newobj, NULL, TO_ROOM );
    return TRUE;
}



/*
 * Object Editor Functions.
 */
void show_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];

    switch( obj->item_type )
    {
	default:	/* No values. */
	    break;
            
	case ITEM_LIGHT:
            if ( obj->value[2] == -1 )
		sprintf( buf, "[v2] Light:  Infinite[-1]\r\n" );
            else
		sprintf( buf, "[v2] Light:  [%d]\r\n", obj->value[2] );
	    send_to_char( buf, ch );
	    break;

	case ITEM_WAND:
	case ITEM_STAFF:
            sprintf( buf,
		"[v0] Level:          [%d]\r\n"
		"[v1] Charges Total:  [%d]\r\n"
		"[v2] Charges Left:   [%d]\r\n"
		"[v3] Spell:          %s\r\n",
		obj->value[0],
		obj->value[1],
		obj->value[2],
		obj->value[3] != -1 ? skill_table[obj->value[3]].name
		                    : "none" );
	    send_to_char( buf, ch );
	    break;

	case ITEM_SCROLL:
	case ITEM_POTION:
	case ITEM_PILL:
            sprintf( buf,
		"[v0] Level:  [%d]\r\n"
		"[v1] Spell:  %s\r\n"
		"[v2] Spell:  %s\r\n"
		"[v3] Spell:  %s\r\n",
		obj->value[0],
		obj->value[1] != -1 ? skill_table[obj->value[1]].name
		                    : "none",
		obj->value[2] != -1 ? skill_table[obj->value[2]].name
                                    : "none",
		obj->value[3] != -1 ? skill_table[obj->value[3]].name
		                    : "none" );
	    send_to_char( buf, ch );
	    break;

	case ITEM_WEAPON:
            sprintf( buf,
        	"[v1] Damage minimum: [%d]\r\n"
        	"[v2] Damage maximum: [%d]\r\n"
		"[v3] Type:           %s\r\n",
		obj->value[1],
		obj->value[2],
		flag_string( weapon_flags, obj->value[3] ) );
	    send_to_char( buf, ch );
	    break;

	case ITEM_CONTAINER:
	    sprintf( buf,
		"[v0] Weight: [%d kg]\r\n"
		"[v1] Flags:  [%s]\r\n"
		"[v2] Key:    %s [%d]\r\n",
		obj->value[0],
		flag_string( container_flags, obj->value[1] ),
                get_obj_index(obj->value[2])
                    ? get_obj_index(obj->value[2])->short_descr
                    : "none", obj->value[2]);
	    send_to_char( buf, ch );
	    break;

	case ITEM_DRINK_CON:
	    sprintf( buf,
	        "[v0] Liquid Total: [%d]\r\n"
	        "[v1] Liquid Left:  [%d]\r\n"
	        "[v2] Liquid:       %s\r\n"
	        "[v3] Poisoned:     %s\r\n",
	        obj->value[0],
	        obj->value[1],
	        flag_string( liquid_flags, obj->value[2] ),
	        obj->value[3] != 0 ? "Yes" : "No" );
	    send_to_char( buf, ch );
	    break;

	case ITEM_FOOD:
	    sprintf( buf,
		"[v0] Food hours: [%d]\r\n"
		"[v3] Poisoned:   %s\r\n",
		obj->value[0],
		obj->value[3] != 0 ? "Yes" : "No" );
	    send_to_char( buf, ch );
	    break;

	case ITEM_MONEY:
            sprintf( buf, "[v0] Gold:   [%d]\r\n", obj->value[0] );
	    send_to_char( buf, ch );
	    break;
    }

    return;
}



bool set_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, int value_num, char *argument)
{
    switch( pObj->item_type )
    {
        default:
            break;
            
        case ITEM_LIGHT:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_LIGHT" );
	            return FALSE;
	        case 2:
	            send_to_char( "HOURS OF LIGHT SET.\r\n\r\n", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	    }
            break;

        case ITEM_WAND:
        case ITEM_STAFF:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_STAFF_WAND" );
	            return FALSE;
	        case 0:
	            send_to_char( "SPELL LEVEL SET.\r\n\r\n", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "TOTAL NUMBER OF CHARGES SET.\r\n\r\n", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "CURRENT NUMBER OF CHARGES SET.\r\n\r\n", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	        case 3:
	            send_to_char( "SPELL TYPE SET.\r\n", ch );
	            pObj->value[3] = skill_lookup( argument );
	            break;
	    }
            break;

        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_SCROLL_POTION_PILL" );
	            return FALSE;
	        case 0:
	            send_to_char( "SPELL LEVEL SET.\r\n\r\n", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "SPELL TYPE 1 SET.\r\n\r\n", ch );
	            pObj->value[1] = skill_lookup( argument );
	            break;
	        case 2:
	            send_to_char( "SPELL TYPE 2 SET.\r\n\r\n", ch );
	            pObj->value[2] = skill_lookup( argument );
	            break;
	        case 3:
	            send_to_char( "SPELL TYPE 3 SET.\r\n\r\n", ch );
	            pObj->value[3] = skill_lookup( argument );
	            break;
	    }
            break;

        case ITEM_WEAPON:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_WEAPON" );
	            return FALSE;
	        case 1:
	            send_to_char( "MINIMUM DAMAGE SET.\r\n\r\n", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "MAXIMUM DAMAGE SET.\r\n\r\n", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	        case 3:
	            send_to_char( "WEAPON TYPE SET.\r\n\r\n", ch );
	            pObj->value[3] = flag_value( weapon_flags, argument );
	            break;
	    }
            break;

        case ITEM_CONTAINER:
	    switch ( value_num )
	    {
		int value;
		
		default:
		    do_help( ch, "ITEM_CONTAINER" );
	            return FALSE;
		case 0:
	            send_to_char( "WEIGHT CAPACITY SET.\r\n\r\n", ch );
	            pObj->value[0] = atoi( argument );
	            break;
		case 1:
	            if ( ( value = flag_value( container_flags, argument ) )
	              != NO_FLAG )
	        	TOGGLE_BIT(pObj->value[1], value);
		    else
		    {
			do_help ( ch, "ITEM_CONTAINER" );
			return FALSE;
		    }
	            send_to_char( "CONTAINER TYPE SET.\r\n\r\n", ch );
	            break;
		case 2:
		    if ( atoi(argument) != 0 )
		    {
			if ( !get_obj_index( atoi( argument ) ) )
			{
			    send_to_char( "THERE IS NO SUCH ITEM.\r\n\r\n", ch );
			    return FALSE;
			}

			if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY )
			{
			    send_to_char( "THAT ITEM IS NOT A KEY.\r\n\r\n", ch );
			    return FALSE;
			}
		    }
		    send_to_char( "CONTAINER KEY SET.\r\n\r\n", ch );
		    pObj->value[2] = atoi( argument );
		    break;
	    }
	    break;

	case ITEM_DRINK_CON:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_DRINK" );
	            return FALSE;
	        case 0:
	            send_to_char( "MAXIMUM AMOUT OF LIQUID HOURS SET.\r\n\r\n", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "CURRENT AMOUNT OF LIQUID HOURS SET.\r\n\r\n", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "LIQUID TYPE SET.\r\n\r\n", ch );
	            pObj->value[2] = flag_value( liquid_flags, argument );
	            break;
	        case 3:
	            send_to_char( "POISON VALUE TOGGLED.\r\n\r\n", ch );
	            pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
	            break;
	    }
            break;

	case ITEM_FOOD:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_FOOD" );
	            return FALSE;
	        case 0:
	            send_to_char( "HOURS OF FOOD SET.\r\n\r\n", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 3:
	            send_to_char( "POISON VALUE TOGGLED.\r\n\r\n", ch );
	            pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
	            break;
	    }
            break;

	case ITEM_MONEY:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_MONEY" );
	            return FALSE;
	        case 0:
	            send_to_char( "GOLD AMOUNT SET.\r\n\r\n", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	    }
            break;
    }

    show_obj_values( ch, pObj );

    return TRUE;
}



bool oedit_show( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int cnt;

    EDIT_OBJ(ch, pObj);

    sprintf( buf, "Name:        [%s]\r\nArea:        [%5d] %s\r\n",
	pObj->name,
	!pObj->area ? -1        : pObj->area->vnum,
	!pObj->area ? "No Area" : pObj->area->name );
    send_to_char( buf, ch );


    sprintf( buf, "Vnum:        [%5d]\r\nType:        [%s]\r\n",
	pObj->vnum,
	flag_string( type_flags, pObj->item_type ) );
    send_to_char( buf, ch );

    sprintf( buf, "Wear flags:  [%s]\r\n",
	flag_string( wear_flags, pObj->wear_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Extra flags: [%s]\r\n",
	flag_string( extra_flags, pObj->extra_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Weight:      [%d]\r\nCost:        [%d]\r\n",
	pObj->weight, pObj->cost );
    send_to_char( buf, ch );

    if ( pObj->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char( "Ex desc kwd: ", ch );

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    send_to_char( "[", ch );
	    send_to_char( ed->keyword, ch );
	    send_to_char( "]", ch );
	}

	send_to_char( "\r\n", ch );
    }

    sprintf( buf, "Short desc:  %s\r\nLong desc:\r\n     %s\r\n",
	pObj->short_descr, pObj->description );
    send_to_char( buf, ch );

    for ( cnt = 0, paf = pObj->affected; paf; paf = paf->next )
    {
	if ( cnt == 0 )
	{
	    send_to_char( "Number Modifier Affects\r\n", ch );
	    send_to_char( "------ -------- -------\r\n", ch );
	}
	sprintf( buf, "[%4d] %-8d %s\r\n", cnt,
	    paf->modifier,
	    flag_string( apply_flags, paf->location ) );
	send_to_char( buf, ch );
	cnt++;
    }

    show_obj_values( ch, pObj );

    return FALSE;
}


/*
 * Need to issue warning if flag isn't valid.
 */
bool oedit_addaffect( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    char loc[MAX_STRING_LENGTH];
    char mod[MAX_STRING_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument( argument, loc );
    one_argument( argument, mod );

    if ( loc[0] == '\0' || mod[0] == '\0' || !is_number( mod ) )
    {
	send_to_char( "Syntax:  addaffect [location] [#mod]\r\n", ch );
	return FALSE;
    }

    pAf             =   new_affect();
    pAf->location   =   flag_value( apply_flags, loc );
    pAf->modifier   =   atoi( mod );
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;

    send_to_char( "Affect added.\r\n", ch);
    return TRUE;
}



/*
 * My thanks to Hans Hvidsten Birkeland and Noam Krendel(Walker)
 * for really teaching me how to manipulate pointers.
 */
bool oedit_delaffect( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    AFFECT_DATA *pAf_next;
    char affect[MAX_STRING_LENGTH];
    int  value;
    int  cnt = 0;

    EDIT_OBJ(ch, pObj);

    one_argument( argument, affect );

    if ( !is_number( affect ) || affect[0] == '\0' )
    {
	send_to_char( "Syntax:  delaffect [#affect]\r\n", ch );
	return FALSE;
    }

    value = atoi( affect );

    if ( value < 0 )
    {
	send_to_char( "Only non-negative affect-numbers allowed.\r\n", ch );
	return FALSE;
    }

    if ( !( pAf = pObj->affected ) )
    {
	send_to_char( "OEdit:  Non-existant affect.\r\n", ch );
	return FALSE;
    }

    if( value == 0 )	/* First case: Remove first affect */
    {
	pAf = pObj->affected;
	pObj->affected = pAf->next;
	free_affect( pAf );
    }
    else		/* Affect to remove is not the first */
    {
	while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
	     pAf = pAf_next;

	if( pAf_next )		/* See if it's the next affect */
	{
	    pAf->next = pAf_next->next;
	    free_affect( pAf_next );
	}
	else                                 /* Doesn't exist */
	{
	     send_to_char( "No such affect.\r\n", ch );
	     return FALSE;
	}
    }

    send_to_char( "Affect removed.\r\n", ch);
    return TRUE;
}



bool oedit_name( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [string]\r\n", ch );
	return FALSE;
    }

    free_string( pObj->name );
    pObj->name = str_dup( argument );

    send_to_char( "Name set.\r\n", ch);
    return TRUE;
}



bool oedit_short( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  short [string]\r\n", ch );
	return FALSE;
    }

    free_string( pObj->short_descr );
    pObj->short_descr = str_dup( argument );
    pObj->short_descr[0] = LOWER( pObj->short_descr[0] );

    send_to_char( "Short description set.\r\n", ch);
    return TRUE;
}



bool oedit_long( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  long [string]\r\n", ch );
	return FALSE;
    }
        
    free_string( pObj->description );
    pObj->description = str_dup( argument );
    pObj->description[0] = UPPER( pObj->description[0] );

    send_to_char( "Long description set.\r\n", ch);
    return TRUE;
}



bool set_value( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, char *argument, int value )
{
    if ( argument[0] == '\0' )
    {
	set_obj_values( ch, pObj, -1, '\0' );
	return FALSE;
    }

    if ( set_obj_values( ch, pObj, value, argument ) )
	return TRUE;

    return FALSE;
}



/*****************************************************************************
 Name:		oedit_values
 Purpose:	Finds the object and sets its value.
 Called by:	The four valueX functions below.
 ****************************************************************************/
bool oedit_values( CHAR_DATA *ch, char *argument, int value )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( set_value( ch, pObj, argument, value ) )
        return TRUE;

    return FALSE;
}


bool oedit_value0( CHAR_DATA *ch, char *argument )
{
    if ( oedit_values( ch, argument, 0 ) )
        return TRUE;

    return FALSE;
}



bool oedit_value1( CHAR_DATA *ch, char *argument )
{
    if ( oedit_values( ch, argument, 1 ) )
        return TRUE;

    return FALSE;
}



bool oedit_value2( CHAR_DATA *ch, char *argument )
{
    if ( oedit_values( ch, argument, 2 ) )
        return TRUE;

    return FALSE;
}



bool oedit_value3( CHAR_DATA *ch, char *argument )
{
    if ( oedit_values( ch, argument, 3 ) )
        return TRUE;

    return FALSE;
}



bool oedit_weight( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  weight [number]\r\n", ch );
	return FALSE;
    }

    pObj->weight = atoi( argument );

    send_to_char( "Weight set.\r\n", ch);
    return TRUE;
}



bool oedit_cost( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  cost [number]\r\n", ch );
	return FALSE;
    }

    pObj->cost = atoi( argument );

    send_to_char( "Cost set.\r\n", ch);
    return TRUE;
}



bool oedit_create( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    int  value;
    int  iHash;

    value = atoi( argument );

    /* OLC 1.1b */
    if ( argument[0] == '\0' || value <= 0 || value >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "Syntax:  create [0 < vnum < %d]\r\n", INT_MAX );
	send_to_char( output, ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );
    if ( !pArea )
    {
	send_to_char( "OEdit:  That vnum is not assigned an area.\r\n", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "OEdit:  Vnum in an area you cannot build in.\r\n", ch );
	return FALSE;
    }

    if ( get_obj_index( value ) )
    {
	send_to_char( "OEdit:  Object vnum already exists.\r\n", ch );
	return FALSE;
    }
        
    pObj			= new_obj_index();
    pObj->vnum			= value;
    pObj->area			= pArea;
        
    if ( value > top_vnum_obj )
	top_vnum_obj = value;

    iHash			= value % MAX_KEY_HASH;
    pObj->next			= obj_index_hash[iHash];
    obj_index_hash[iHash]	= pObj;
    ch->desc->pEdit		= (void *)pObj;

    send_to_char( "Object Created.\r\n", ch );
    return TRUE;
}



bool oedit_ed( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    EXTRA_DESCR_DATA *ed;
    char command[MAX_INPUT_LENGTH];
    char keyword[MAX_INPUT_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument( argument, command );
    one_argument( argument, keyword );

    if ( command[0] == '\0' )
    {
	send_to_char( "Syntax:  ed add [keyword]\r\n", ch );
	send_to_char( "         ed delete [keyword]\r\n", ch );
	send_to_char( "         ed edit [keyword]\r\n", ch );
	send_to_char( "         ed format [keyword]\r\n", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "add" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed add [keyword]\r\n", ch );
	    return FALSE;
	}

	ed                  =   new_extra_descr();
	ed->keyword         =   str_dup( keyword );
	ed->next            =   pObj->extra_descr;
	pObj->extra_descr   =   ed;

	string_append( ch, &ed->description );

	return TRUE;
    }

    if ( !str_cmp( command, "edit" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed edit [keyword]\r\n", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char( "OEdit:  Extra description keyword not found.\r\n", ch );
	    return FALSE;
	}

	string_append( ch, &ed->description );

	return TRUE;
    }

    if ( !str_cmp( command, "delete" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;

	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed delete [keyword]\r\n", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	    ped = ed;
	}

	if ( !ed )
	{
	    send_to_char( "OEdit:  Extra description keyword not found.\r\n", ch );
	    return FALSE;
	}

	if ( !ped )
	    pObj->extra_descr = ed->next;
	else
	    ped->next = ed->next;

	free_extra_descr( ed );

	send_to_char( "Extra description deleted.\r\n", ch );
	return TRUE;
    }


    if ( !str_cmp( command, "format" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;

	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed format [keyword]\r\n", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	    ped = ed;
	}

	if ( !ed )
	{
                send_to_char( "OEdit:  Extra description keyword not found.\r\n", ch );
                return FALSE;
	}

	/* OLC 1.1b */
	if ( strlen(ed->description) >= (MAX_STRING_LENGTH - 4) )
	{
	    send_to_char( "String too long to be formatted.\r\n", ch );
	    return FALSE;
	}

	ed->description = format_string( ed->description );

	send_to_char( "Extra description formatted.\r\n", ch );
	return TRUE;
    }

    oedit_ed( ch, "" );
    return FALSE;
}



/*
 * Mobile Editor Functions.
 */
bool medit_show( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    char buf[MAX_STRING_LENGTH];

    EDIT_MOB(ch, pMob);

    sprintf( buf, "Name:        [%s]\r\nArea:        [%5d] %s\r\n",
	pMob->player_name,
	!pMob->area ? -1        : pMob->area->vnum,
	!pMob->area ? "No Area" : pMob->area->name );
    send_to_char( buf, ch );

    sprintf( buf, "Act:         [%s]\r\n",
	flag_string( act_flags, pMob->act ) );
    send_to_char( buf, ch );

    sprintf( buf, "Vnum:        [%5d]\r\nSex:         [%s]\r\n",
	pMob->vnum,
	pMob->sex == SEX_MALE    ? "male"   :
	pMob->sex == SEX_FEMALE  ? "female" : "neutral" );
    send_to_char( buf, ch );

    sprintf( buf,
	"Level:       [%2d]\r\nAlign:       [%4d]\r\n",
	pMob->level,       pMob->alignment );
    send_to_char( buf, ch );

    sprintf( buf, "Affected by: [%s]\r\n",
	flag_string( affect_flags, pMob->affected_by ) );
    send_to_char( buf, ch );

    if ( pMob->spec_fun )
    {
	sprintf( buf, "Spec fun:    [%s]\r\n",  spec_string( pMob->spec_fun ) );
	send_to_char( buf, ch );
    }

    if ( pMob->game_fun )
    {
        sprintf( buf, "Game fun:    [%s]\r\n",  game_string( pMob->game_fun ) );        send_to_char( buf, ch );
    }

    sprintf( buf, "Short descr: %s\r\nLong descr:\r\n%s",
	pMob->short_descr,
	pMob->long_descr );
    send_to_char( buf, ch );

    sprintf( buf, "Description:\r\n%s", pMob->description );
    send_to_char( buf, ch );

    if ( pMob->pShop )
    {
	SHOP_DATA *pShop;
	int iTrade;

	pShop = pMob->pShop;

	sprintf( buf,
	  "Shop data for [%5d]:\r\n"
	  "  Markup for purchaser: %d%%\r\n"
	  "  Markdown for seller:  %d%%\r\n",
	    pShop->keeper, pShop->profit_buy, pShop->profit_sell );
	send_to_char( buf, ch );
	sprintf( buf, "  Hours: %d to %d.\r\n",
	    pShop->open_hour, pShop->close_hour );
	send_to_char( buf, ch );

	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	{
	    if ( pShop->buy_type[iTrade] != 0 )
	    {
		if ( iTrade == 0 ) {
		    send_to_char( "  Number Trades Type\r\n", ch );
		    send_to_char( "  ------ -----------\r\n", ch );
		}
		sprintf( buf, "  [%4d] %s\r\n", iTrade,
		    flag_string( type_flags, pShop->buy_type[iTrade] ) );
		send_to_char( buf, ch );
	    }
	}
    }

    return FALSE;
}



bool medit_create( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    int  value;
    int  iHash;

    value = atoi( argument );

    /* OLC 1.1b */
    if ( argument[0] == '\0' || value <= 0 || value >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "Syntax:  create [0 < vnum < %d]\r\n", INT_MAX );
	send_to_char( output, ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );

    if ( !pArea )
    {
	send_to_char( "MEdit:  That vnum is not assigned an area.\r\n", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "MEdit:  Vnum in an area you cannot build in.\r\n", ch );
	return FALSE;
    }

    if ( get_mob_index( value ) )
    {
	send_to_char( "MEdit:  Mobile vnum already exists.\r\n", ch );
	return FALSE;
    }

    pMob			= new_mob_index();
    pMob->vnum			= value;
    pMob->area			= pArea;
        
    if ( value > top_vnum_mob )
	top_vnum_mob = value;        

    pMob->act			= ACT_IS_NPC;
    iHash			= value % MAX_KEY_HASH;
    pMob->next			= mob_index_hash[iHash];
    mob_index_hash[iHash]	= pMob;
    ch->desc->pEdit		= (void *)pMob;

    send_to_char( "Mobile Created.\r\n", ch );
    return TRUE;
}



bool medit_spec( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  spec [special function]\r\n", ch );
	return FALSE;
    }


    if ( !str_cmp( argument, "none" ) )
    {
        pMob->spec_fun = NULL;

        send_to_char( "Spec removed.\r\n", ch);
        return TRUE;
    }

    if ( spec_lookup( argument ) )
    {
	pMob->spec_fun = spec_lookup( argument );
	send_to_char( "Spec set.\r\n", ch);
	return TRUE;
    }

    send_to_char( "MEdit: No such special function.\r\n", ch );
    return FALSE;
}

bool medit_game( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax:  game [game function]\r\n", ch );
        return FALSE;
    }


    if ( !str_cmp( argument, "none" ) )
    {
        pMob->game_fun = NULL;

        send_to_char( "Game removed.\r\n", ch);
        return TRUE;
    }

    if ( game_lookup( argument ) )
    {
        pMob->game_fun = game_lookup( argument );
        send_to_char( "Game set.\r\n", ch);
        return TRUE;
    }

    send_to_char( "MEdit: No such game function.\r\n", ch );
    return FALSE;
}



bool medit_align( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  alignment [number]\r\n", ch );
	return FALSE;
    }

    pMob->alignment = atoi( argument );

    send_to_char( "Alignment set.\r\n", ch);
    return TRUE;
}



bool medit_level( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  level [number]\r\n", ch );
	return FALSE;
    }

    pMob->level = atoi( argument );

    send_to_char( "Level set.\r\n", ch);
    return TRUE;
}



bool medit_desc( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pMob->description );
	return TRUE;
    }

    send_to_char( "Syntax:  desc    - line edit\r\n", ch );
    return FALSE;
}




bool medit_long( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  long [string]\r\n", ch );
	return FALSE;
    }

    free_string( pMob->long_descr );
    strcat( argument, "\r\n" );
    pMob->long_descr = str_dup( argument );
    pMob->long_descr[0] = UPPER( pMob->long_descr[0]  );

    send_to_char( "Long description set.\r\n", ch);
    return TRUE;
}



bool medit_short( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  short [string]\r\n", ch );
	return FALSE;
    }

    free_string( pMob->short_descr );
    pMob->short_descr = str_dup( argument );

    send_to_char( "Short description set.\r\n", ch);
    return TRUE;
}



bool medit_name( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [string]\r\n", ch );
	return FALSE;
    }

    free_string( pMob->player_name );
    pMob->player_name = str_dup( argument );

    send_to_char( "Name set.\r\n", ch);
    return TRUE;
}




bool medit_shop( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    char command[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );
    argument = one_argument( argument, arg1 );

    EDIT_MOB(ch, pMob);

    if ( command[0] == '\0' )
    {
	send_to_char( "Syntax:  shop hours [#opening] [#closing]\r\n", ch );
	send_to_char( "         shop profit [#buying%] [#selling%]\r\n", ch );
	send_to_char( "         shop type [#0-4] [item type]\r\n", ch );
	send_to_char( "         shop delete [#0-4]\r\n", ch );
	return FALSE;
    }


    if ( !str_cmp( command, "hours" ) )
    {
	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' || !is_number( argument ) )
	{
	    send_to_char( "Syntax:  shop hours [#opening] [#closing]\r\n", ch );
	    return FALSE;
	}

	if ( !pMob->pShop )
	{
	    pMob->pShop         = new_shop();
	    pMob->pShop->keeper = pMob->vnum;
	    shop_last->next     = pMob->pShop;
	}

	pMob->pShop->open_hour = atoi( arg1 );
	pMob->pShop->close_hour = atoi( argument );

	send_to_char( "Shop hours set.\r\n", ch);
	return TRUE;
    }


    if ( !str_cmp( command, "profit" ) )
    {
	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' || !is_number( argument ) )
	{
	    send_to_char( "Syntax:  shop profit [#buying%] [#selling%]\r\n", ch );
	    return FALSE;
	}

	if ( !pMob->pShop )
	{
	    pMob->pShop         = new_shop();
	    pMob->pShop->keeper = pMob->vnum;
	    shop_last->next     = pMob->pShop;
	}

	pMob->pShop->profit_buy     = atoi( arg1 );
	pMob->pShop->profit_sell    = atoi( argument );

	send_to_char( "Shop profit set.\r\n", ch);
	return TRUE;
    }


    if ( !str_cmp( command, "type" ) )
    {
	char buf[MAX_INPUT_LENGTH];
	int value;

	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' )
	{
	    send_to_char( "Syntax:  shop type [#0-4] [item type]\r\n", ch );
	    return FALSE;
	}

	if ( atoi( arg1 ) >= MAX_TRADE )
	{
	    sprintf( buf, "REdit:  May sell %d items max.\r\n", MAX_TRADE );
	    send_to_char( buf, ch );
	    return FALSE;
	}

	if ( ( value = flag_value( type_flags, argument ) ) == NO_FLAG )
	{
	    send_to_char( "REdit:  That type of item is not known.\r\n", ch );
	    return FALSE;
	}

	if ( !pMob->pShop )
	{
	    pMob->pShop         = new_shop();
	    pMob->pShop->keeper = pMob->vnum;
	    shop_last->next     = pMob->pShop;
	}

	pMob->pShop->buy_type[atoi( arg1 )] = value;

	send_to_char( "Shop type set.\r\n", ch);
	return TRUE;
    }


    if ( !str_cmp( command, "delete" ) )
    {
	SHOP_DATA *pShop;
	SHOP_DATA *pShop_next;
	int value;
	int cnt = 0;
	
	if ( arg1[0] == '\0' || !is_number( arg1 ) )
	{
	    send_to_char( "Syntax:  shop delete [#0-4]\r\n", ch );
	    return FALSE;
	}

	value = atoi( argument );
	
	if ( !pMob->pShop )
	{
	    send_to_char( "REdit:  Non-existant shop.\r\n", ch );
	    return FALSE;
	}

	if ( value == 0 )
	{
	    pShop = pMob->pShop;
	    pMob->pShop = pMob->pShop->next;
	    free_shop( pShop );
	}
	else
	for ( pShop = pMob->pShop, cnt = 0; pShop; pShop = pShop_next, cnt++ )
	{
	    pShop_next = pShop->next;
	    if ( cnt+1 == value )
	    {
		pShop->next = pShop_next->next;
		free_shop( pShop_next );
		break;
	    }
	}

	send_to_char( "Shop deleted.\r\n", ch);
	return TRUE;
    }

    medit_shop( ch, "" );
    return FALSE;
}
