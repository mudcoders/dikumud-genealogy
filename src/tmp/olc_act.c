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

#define unix 1
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
#include <sys/stat.h>
#include "merc.h"
#include "olc.h"

extern  int               num_mob_progs;
extern  int               num_trap_progs;

/*
 * External functions.
 */
void 		clan_sort	args( ( CLAN_DATA *pClan ) );
char 	*mprog_type_to_name   	args( ( int type ) );
HELP_DATA 	*get_help      	args( ( char *argument ) );
SOCIAL_DATA	*get_social	args( ( char *argument ) );
NEWBIE_DATA *get_newbie_data    args( ( char *argument ) );

struct olc_help_type
{
    char *command;
    const void *structure;
    char *desc;
};



bool show_version( CHAR_DATA *ch, char *argument )
{
    send_to_char(C_DEFAULT, VERSION, ch );
    send_to_char(C_DEFAULT, "\n\r", ch );
    send_to_char(C_DEFAULT, AUTHOR, ch );
    send_to_char(C_DEFAULT, "\n\r", ch );
    send_to_char(C_DEFAULT, DATE, ch );
    send_to_char(C_DEFAULT, "\n\r", ch );
    send_to_char(C_DEFAULT, CREDITS, ch );
    send_to_char(C_DEFAULT, "\n\r", ch );

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
/* FOR NEW FLAGS */
    {	"race",		anti_race_flags, "Anti race flags."		},

    {   "class",	anti_class_flags,"Anti class flags."		},

    {	"wear",		wear_flags,	 "Where to wear object."	},
    {	"spec",		spec_table,	 "Available special programs." 	},
    {	"sex",		sex_flags,	 "Sexes."			},
    {	"act",		act_flags,	 "Mobile attributes."		},
    {	"affect",	affect_flags,	 "Mobile affects."		},
    {   "affect2",      affect2_flags,   "Mobile affects 2."  		},
    {	"wear-loc",	wear_loc_flags,	 "Where mobile wears object."	},
    {	"spells",	skill_table,	 "Names of current spells." 	},
    {	"weapon",	weapon_flags,	 "Type of weapon." 		},
    {	"container",	container_flags, "Container status."		},
    {	"liquid",	liquid_flags,	 "Types of liquids."		},
    {   "immune",       immune_flags,    "Types of immunities."         },
    {   "mprogs",       mprog_types,     "Types of MobProgs."           },
    {   "oprogs",       oprog_types,     "Types of ObjProgs."           },
    {   "rprogs",       rprog_types,     "Types of RoomProgs."          },
    {   "eprogs",       eprog_types,     "Types of ExitProgs."          },
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
		strcat( buf1, "\n\r" );
	}
    }
 
    if ( col % 4 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char(C_DEFAULT, buf1, ch );
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
    for (sn = 0; skill_table[sn].name[0] != '\0'; sn++)
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
		strcat( buf1, "\n\r" );
	}
    }
 
    if ( col % 4 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char(C_DEFAULT, buf1, ch );
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
    send_to_char(C_DEFAULT, "Preceed special functions with 'spec_'\n\r\n\r", ch );
    for (spec = 0; *spec_table[spec].spec_fun; spec++)
    {
	sprintf( buf, "%-19.18s", &spec_table[spec].spec_name[5] );
	strcat( buf1, buf );
	if ( ++col % 4 == 0 )
	    strcat( buf1, "\n\r" );
    }
 
    if ( col % 4 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char(C_DEFAULT, buf1, ch );
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
	send_to_char(C_DEFAULT, "Syntax:  ? [command]\n\r\n\r", ch );
	send_to_char(C_DEFAULT, "[command]  [description]\n\r", ch );
	for (cnt = 0; help_table[cnt].command[0] != '\0'; cnt++)
	{
	    sprintf( buf, "%-10.10s -%s\n\r",
	        capitalize( help_table[cnt].command ),
		help_table[cnt].desc );
	    send_to_char(C_DEFAULT, buf, ch );
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
	    else
	    if ( help_table[cnt].structure == skill_table )
	    {

		if ( spell[0] == '\0' )
		{
		    send_to_char(C_DEFAULT, "Syntax:  ? spells "
		        "[ignore/attack/defend/self/object/all]\n\r", ch );
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
		    send_to_char(C_DEFAULT, "Syntax:  ? spell "
		        "[ignore/attack/defend/self/object/all]\n\r", ch );
		    
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

bool redit_proglist( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA      *pMobIndex;
    OBJ_INDEX_DATA      *pObjIndex;
    ROOM_INDEX_DATA	*pRoomIndex;
    AREA_DATA           *pArea;
    char                buf  [ MAX_STRING_LENGTH   ];
    char                buf1 [ MAX_STRING_LENGTH*2 ];
    char		buf2 [ MAX_STRING_LENGTH   ];
    char                arg  [ MAX_INPUT_LENGTH    ];
    bool fAll, found;
    int vnum;
    int  col = 0;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char(C_DEFAULT, "Syntax:  proglist <all/obj/mob/room>\n\r", ch );
        return FALSE;
    }

    pArea = ch->in_room->area;
    buf1[0] = '\0';
    fAll    = !str_cmp( arg, "all" );
    found   = FALSE;

    if ( ( fAll ) || ( !str_cmp( arg, "obj" ) ) )
    {
      found = TRUE;
      send_to_char( AT_WHITE, "Objects with programs:\n\r", ch );
      for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
      {
	if ( ( pObjIndex = get_obj_index( vnum ) ) )
 	{
	   if ( pObjIndex->traptypes ) 
	   {
               sprintf( buf, "&z[&R%5d&z] &w%-17.16s",
                    pObjIndex->vnum,
                    capitalize( strip_color(pObjIndex->short_descr) ) );
                strcat( buf1, buf );
                if ( ++col % 3 == 0 )
                    strcat( buf1, "\n\r" );
	   }
	}
      }
      if ( col % 3 != 0 )
        strcat( buf1, "\n\r" );

      send_to_char(C_DEFAULT, buf1, ch );
      buf1[0] = '\0';
      col = 0;
    }

    if ( ( fAll ) || ( !str_cmp( arg, "room" ) ) )
    {
      found = TRUE;
      send_to_char( AT_WHITE, "Rooms with programs:\n\r", ch );
      for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
      {
        if ( ( pRoomIndex = get_room_index( vnum ) ) )
	{
           if ( pRoomIndex->traptypes )
           {
               sprintf( buf, "&z[&R%5d&z] &w%-17.16s",
                    pRoomIndex->vnum,
                    capitalize( strip_color(pRoomIndex->name) ) );
                strcat( buf1, buf );
                if ( ++col % 3 == 0 )
                    strcat( buf1, "\n\r" );
           }
	}
      }
      if ( col % 3 != 0 )
        strcat( buf1, "\n\r" );

      send_to_char(C_DEFAULT, buf1, ch );
      buf1[0] = '\0';
      col = 0;
    }
    if ( ( fAll ) || ( !str_cmp( arg, "mob" ) ) )
    {
      found = TRUE;
      send_to_char( AT_WHITE, "Mobs with programs:\n\r", ch );
      for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
      {
        if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
           if ( pMobIndex->progtypes )
           {
               sprintf( buf, "&z[&R%5d&z] &w%-17.16s",
                    pMobIndex->vnum,
                    capitalize( strip_color(pMobIndex->short_descr) ) );
                strcat( buf1, buf );
                if ( ++col % 3 == 0 )
                    strcat( buf1, "\n\r" );
           }
	}
      }
      if ( col % 3 != 0 )
        strcat( buf1, "\n\r" );

      send_to_char(C_DEFAULT, buf1, ch );
      buf1[0] = '\0';
      col = 0;
    }
   if ( !found )
    {
	if ( !str_cmp( arg, "all" ) )
	{
          send_to_char(C_DEFAULT, "Programs not found in this area.\n\r", ch);
          return FALSE;
	}
	else {
	  sprintf( buf2, "Programs on %ss not found in this area.\n\r", arg );
	  send_to_char( C_DEFAULT, buf2, ch );
	  return FALSE;
	}
    }

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
	send_to_char(C_DEFAULT, "Syntax:  mlist <all/name>\n\r", ch );
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
	    if ( fAll || is_name( ch, arg, pMobIndex->player_name ) 
		|| (pMobIndex->level == atoi(arg)) )
	    {
		found = TRUE;
		sprintf( buf, "&z[&R%5d&z] &w%-17.16s",
		    pMobIndex->vnum,
		    capitalize( strip_color(pMobIndex->short_descr) ) );
		strcat( buf1, buf );
		if ( ++col % 3 == 0 )
		    strcat( buf1, "\n\r" );
	    }
	}
    }

    if ( !found )
    {
	send_to_char(C_DEFAULT, "Mobile(s) not found in this area.\n\r", ch);
	return FALSE;
    }

    if ( col % 3 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char(C_DEFAULT, buf1, ch );
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
	send_to_char(C_DEFAULT, "Syntax:  olist <all/name/item_type/object level>\n\r", ch );
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
	    if ( fAll || is_name( ch, arg, pObjIndex->name )
	    || ( flag_value( type_flags, arg ) == pObjIndex->item_type ) ||
	       ( pObjIndex->level == atoi(arg) ) )
	    {
		found = TRUE;
		sprintf( buf, "&z[&R%5d&z] &w%-17.16s",
		    pObjIndex->vnum, 
		    capitalize( strip_color(pObjIndex->short_descr) ) );
		strcat( buf1, buf );
		if ( ++col % 3 == 0 )
		    strcat( buf1, "\n\r" );
	    }
	}
    }

    if ( !found )
    {
	send_to_char(C_DEFAULT, "Object(s) not found in this area.\n\r", ch);
	return FALSE;
    }

    if ( col % 3 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char(C_DEFAULT, buf1, ch );
    return FALSE;
}

bool redit_rlist( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA	*pRoomIndex;
    AREA_DATA		*pArea;
    char		buf  [ MAX_STRING_LENGTH   ];
    char		buf1 [ MAX_STRING_LENGTH*2 ];
    bool found;
    int vnum;
    int  col = 0;

    pArea = ch->in_room->area;
    buf1[0] = '\0';
    found   = FALSE;

    for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if ( ( pRoomIndex = get_room_index( vnum ) ) )
	{
		found = TRUE;
		sprintf( buf, "&z[&R%5d&z] &w%-17.16s",
		    pRoomIndex->vnum, 
		    capitalize( strip_color(pRoomIndex->name) ) );
		strcat( buf1, buf );
		if ( ++col % 3 == 0 )
		    strcat( buf1, "\n\r" );
	}
    }

    if ( !found )
    {
	send_to_char(C_DEFAULT, "Room(s) not found in this area.\n\r", ch);
	return FALSE;
    }

    if ( col % 3 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char(C_DEFAULT, buf1, ch );
    return FALSE;
}



bool redit_mshow( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  mshow <vnum>\n\r", ch );
	return FALSE;
    }

    if ( is_number( argument ) )
    {
	value = atoi( argument );
	if ( !( pMob = get_mob_index( value ) ))
	{
	    send_to_char(C_DEFAULT, "REdit:  That mobile does not exist.\n\r", ch );
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
	send_to_char(C_DEFAULT, "Syntax:  oshow <vnum>\n\r", ch );
	return FALSE;
    }

    if ( is_number( argument ) )
    {
	value = atoi( argument );
	if ( !( pObj = get_obj_index( value ) ))
	{
	    send_to_char(C_DEFAULT, "REdit:  That object does not exist.\n\r", ch );
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
    struct stat statis;

    EDIT_AREA(ch, pArea);
    sprintf( buf, "                  &W01234567 &B0123456789012345678 &G0123456789\n\r" );
    send_to_char(C_DEFAULT, buf, ch);
    sprintf( buf, "&WName:     &z[&W%5d&z] &w%s\n\r",pArea->vnum,pArea->name );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&WLevel:    &z{&W%d&z-&W%d&z}\n\r", pArea->llevel, pArea->ulevel );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&WRecall:   &z[&W%5d&z]&w %s\n\r", pArea->recall,
	get_room_index( pArea->recall )
	? get_room_index( pArea->recall )->name : "none" );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WSounds:   &w%s\n\r",
pArea->reset_sound? ( str_cmp(pArea->reset_sound, "none")?
pArea->reset_sound: "(no sound)" ) :
	                              "(default: &GYou shiver as a cold breeze blows through the room.&w)" );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WFile:     &C%s\n\r", pArea->filename );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "../../EOSBUILD/area/%s", pArea->filename );
    if ( !stat( buf, &statis ) )
    {
        sprintf( buf, "&WLast modifed on Build: &C%s\r",(char*)ctime(&statis.st_mtime ) );
        send_to_char( C_DEFAULT, buf, ch );
    }
    else
        send_to_char( C_DEFAULT, "&WLast modifed on Build: &C(No such area)\n\r", ch );

    sprintf( buf, "../../EOS/area/%s", pArea->filename );
    if ( !stat( buf, &statis ) )
    {
        sprintf( buf, "&WLast modifed on Storm: &C%s\r",(char*)ctime(&statis.st_mtime ) );
        send_to_char( C_DEFAULT, buf, ch );
    }
    else
        send_to_char( C_DEFAULT, "&WLast modifed on Storm: &C(No such area)\n\r", ch );


    sprintf( buf, "&WVnums:    &z[&W%d-%d&z]\n\r", pArea->lvnum,pArea->uvnum );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WColor:    &z[&W%d&z]\n\r", pArea->def_color );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WAge:      &z[&W%d&z]\n\r",	pArea->age );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WPlayers:  &z[&W%d&z]\n\r", pArea->nplayer );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WSecurity: &z[&W%d&z]\n\r", pArea->security );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WBuilders: &z[&B%s&z]\n\r", pArea->builders );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WFlags:    &z[&R%s&z]\n\r", flag_string( area_flags,pArea->area_flags ) );
    send_to_char(C_DEFAULT, buf, ch );

    return FALSE;
}

bool aedit_links( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoomIndex;
    char buf  [MAX_STRING_LENGTH];
    int vnum;
    int nMatch = 0;
    int DIR;   

    EDIT_AREA(ch, pArea);

    send_to_char( AT_RED, "Rooms that have exits into this area:\n\r", ch );
    for ( vnum = 0; nMatch < top_room; vnum++ )
    {
	if ( ( pRoomIndex = get_room_index( vnum ) ) )
	{
	  nMatch++;
        if ( pRoomIndex->area != pArea )  
          for ( DIR = 0; DIR <= DIR_MAX; DIR++ )
          {
            if ( pRoomIndex->exit[DIR] )
            {
              if (pRoomIndex->exit[DIR]->to_room)
              {
                if (    pRoomIndex->exit[DIR]->to_room->vnum >= pArea->lvnum 
                     && pRoomIndex->exit[DIR]->to_room->vnum <= pArea->uvnum )
                {
           	    sprintf( buf, "&z[&W%5d&z] &w%s\n\r",
               	    pRoomIndex->vnum, pRoomIndex->name);
       	            send_to_char(AT_RED, buf, ch );
                }
              }
              else
               bug( "Room with a exit to room 0: %d", pRoomIndex->vnum );
            }
          }
	}
    }

    send_to_char( AT_RED, "Rooms that have exits leaving this area:\n\r", ch );

    for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if ( ( pRoomIndex = get_room_index( vnum ) ) )
	{
          for ( DIR = 0; DIR <= DIR_MAX; DIR++ )
          {
            if ( pRoomIndex->exit[DIR] )
              if (pRoomIndex->exit[DIR]->to_room)
              if (    pRoomIndex->exit[DIR]->to_room->vnum < pArea->lvnum 
                   || pRoomIndex->exit[DIR]->to_room->vnum > pArea->uvnum )
              {
      	    sprintf( buf, "&z[&W%5d&z] &w%s\n\r",
          	    pRoomIndex->vnum, pRoomIndex->name);
       	    send_to_char(AT_RED, buf, ch );
              }
          }
	}
    }  

    return FALSE;
}

bool aedit_reset( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    reset_area( pArea );
    send_to_char(C_DEFAULT, "Area reset.\n\r", ch );

    return FALSE;
}



bool aedit_create( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    if ( top_area >= INT_MAX )	/* OLC 1.1b */
    {
	send_to_char(C_DEFAULT, "We're out of vnums for new areas.\n\r", ch );
	return FALSE;
    }

    pArea               =   new_area();
    area_last->next     =   pArea;
    area_last		=   pArea;	/* Thanks, Walker. */
    ch->desc->pEdit     =   (void *)pArea;
/*    pArea->recall       =   25001;*/
    SET_BIT( pArea->area_flags, AREA_ADDED );
    SET_BIT( pArea->area_flags, AREA_PROTOTYPE );
    send_to_char(C_DEFAULT, "Area Created.\n\r", ch );
    return TRUE;	/* OLC 1.1b */
}



bool aedit_name( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:   name [$name]\n\r", ch );
	return FALSE;
    }

    free_string( pArea->name );
    pArea->name = str_dup( argument );

    send_to_char(C_DEFAULT, "Name set.\n\r", ch );
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
	send_to_char(C_DEFAULT, "Syntax:  filename [$file]\n\r", ch );
	return FALSE;
    }

    /*
     * Simple Syntax Check.
     */
    length = strlen( argument );
    if ( length > 8 )
    {
	send_to_char(C_DEFAULT, "No more than eight characters allowed.\n\r", ch );
	return FALSE;
    }

    /*
     * Allow only letters and numbers.
     */
    for ( i = 0; i < length; i++ )
    {
	if ( !isalnum( file[i] ) )
	{
	    send_to_char(C_DEFAULT, "Only letters and numbers are valid.\n\r", ch );
	    return FALSE;
	}
    }    

    free_string( pArea->filename );
    strcat( file, ".are" );
    pArea->filename = str_dup( file );

    send_to_char(C_DEFAULT, "Filename set.\n\r", ch );
    return TRUE;
}

bool aedit_llevel ( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char llevel[MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    one_argument( argument, llevel );

    if ( !is_number( llevel ) || llevel[0] == '\0' )
    {
        send_to_char(C_DEFAULT, "Syntax:  llevel [#lowest sugested level]\n\r", ch);
	return FALSE;
    }

    send_to_char( C_DEFAULT, "Lowest sugested level set.\n\r", ch);

    pArea->llevel = atoi ( llevel );
    return TRUE;
}

bool aedit_ulevel ( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char ulevel[MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    one_argument( argument, ulevel );

    if ( !is_number( ulevel ) || ulevel[0] == '\0' )
    {
        send_to_char(C_DEFAULT, "Syntax:  ulevel [#upper most sugested level]\n\r", ch );
        return FALSE;
    }

    pArea->ulevel = atoi ( ulevel );

    send_to_char(C_DEFAULT, "Upper most suggested level set.\n\r", ch );
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
	send_to_char(C_DEFAULT, "Syntax:  age [#age]\n\r", ch );
	return FALSE;
    }

    pArea->age = atoi( age );

    send_to_char(C_DEFAULT, "Age set.\n\r", ch );
    return TRUE;
}


/*
 * The message displayed to players when area is about to reset.
 * -- Altrag
 */
bool aedit_sounds( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:   sounds [$reset_sounds]\n\r", ch );
	return FALSE;
    }


    free_string( pArea->reset_sound );

    if ( !str_cmp( argument, "default" ) )
      pArea->reset_sound = NULL;
    else
      pArea->reset_sound = str_dup( argument );

    send_to_char(C_DEFAULT, "Reset sounds set.\n\r", ch );
    return TRUE;
}



bool aedit_clan_hq( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    
    EDIT_AREA( ch, pArea );
    
    if ( get_trust( ch ) >= L_CON || !str_cmp( ch->guild->name, "CREATION") )
    {
        TOGGLE_BIT( pArea->area_flags, AREA_CLAN_HQ );
	send_to_char(C_DEFAULT, "Ok.\n\r", ch );
	return TRUE;
    }
     else
    {
	send_to_char(C_DEFAULT, "You are &B*&wtoo&B*&w low of trust to do this.\n\r", ch );
	return FALSE;
    }
return TRUE;
}
bool aedit_prototype( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    EDIT_AREA( ch, pArea );

    if ( get_trust( ch ) >= L_CON || !str_cmp( ch->guild->name, "CREATION") )
    {
	pArea->area_flags ^= AREA_PROTOTYPE;
	send_to_char(C_DEFAULT, "Ok.\n\r", ch );
	return TRUE;
    }
     else
    {
	send_to_char(C_DEFAULT, "You are &B*&wtoo&B*&w low of trust to do this.\n\r", ch );
	return FALSE;
    }
return TRUE;
}
bool aedit_noquest( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    
    EDIT_AREA( ch, pArea );
    
    if ( get_trust( ch ) >= L_CON || !str_cmp( ch->guild->name, "CREATION") )
    {   
        TOGGLE_BIT( pArea->area_flags, AREA_NO_QUEST );
        send_to_char(C_DEFAULT, "Ok.\n\r", ch );
        return TRUE;
    }
     else
    {
        send_to_char(C_DEFAULT, "You are &B*&wtoo&B*&w low of trust to do this.\n\r", ch );
        return FALSE;
    }

    return TRUE;

}

bool aedit_mudschool( CHAR_DATA *ch, char *argument )
{

    AREA_DATA *pArea;
        
    EDIT_AREA( ch, pArea );
     
    if ( get_trust( ch ) >= L_CON || !str_cmp( ch->guild->name, "CREATION") )
    {
        TOGGLE_BIT( pArea->area_flags, AREA_MUDSCHOOL );
        send_to_char(C_DEFAULT, "Ok.\n\r", ch );  
        return TRUE;
    }
     else
    {
        send_to_char(C_DEFAULT, "You are &B*&wtoo&B*&w low of trust to do this.\n\r", ch );
        return FALSE;
    }
    
    return TRUE;

}

bool aedit_color( CHAR_DATA *ch, char *argument )
{

    AREA_DATA *pArea;
    int i = 0;
    
    EDIT_AREA( ch, pArea );
     
    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  color[color number] \n\r", ch );
	send_to_char(C_DEFAULT, "&WType &Rhelp AT_COLOR&W for list of colors.\n\r",ch);
	return FALSE;
    }

    if(!(is_number(argument)))
    {
    	send_to_char(C_DEFAULT, "Color choices must be numeric (1-15).\n\r", ch );
	send_to_char(C_DEFAULT, "&WType &Rhelp AT_COLOR&W for list of colors.\n\r", ch);
	return FALSE;
    }
    i = atoi(argument);
    if( i < 1 || i > 15)
    {
    	send_to_char(C_DEFAULT, "Color choices are from 1-15 only.\n\r", ch );
	send_to_char(C_DEFAULT, "&WType &Rhelp AT_COLOR&W for list of colors.\n\r", ch);
	return FALSE;
    }    
    pArea->def_color = i;

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
	send_to_char(C_DEFAULT, "Syntax:  recall [#rvnum]\n\r", ch );
	return FALSE;
    }

    value = atoi( room );

    if ( !get_room_index( value ) )
    {
	send_to_char(C_DEFAULT, "AEdit:  Room vnum does not exist.\n\r", ch );
	return FALSE;
    }

    pArea->recall = value;

    send_to_char(C_DEFAULT, "Recall set.\n\r", ch );
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
	send_to_char(C_DEFAULT, "Syntax:  security [#level]\n\r", ch );
	return FALSE;
    }

    value = atoi( sec );

    if ( value > ch->pcdata->security || value < 0 )
    {
	if ( ch->pcdata->security != 0 )
	{
	    sprintf( buf, "Security is 0-%d.\n\r", ch->pcdata->security );
	    send_to_char(C_DEFAULT, buf, ch );
	}
	else
	    send_to_char(C_DEFAULT, "Security is 0 only.\n\r", ch );
	return FALSE;
    }

    pArea->security = value;

    send_to_char(C_DEFAULT, "Security set.\n\r", ch );
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
	send_to_char(C_DEFAULT, "Syntax:  builder [$name]  -toggles builder\n\r", ch );
	send_to_char(C_DEFAULT, "Syntax:  builder All      -allows everyone\n\r", ch );
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
	send_to_char(C_DEFAULT, "Builder removed.\n\r", ch );
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

	send_to_char(C_DEFAULT, "Builder added.\n\r", ch );
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
	send_to_char(C_DEFAULT, "Syntax:  vnum [#lower] [#upper]\n\r", ch );
	return FALSE;
    }

    if ( ( ilower = atoi( lower ) ) > ( iupper = atoi( upper ) ) )
    {
	send_to_char(C_DEFAULT, "AEdit:  Upper must be larger then lower.\n\r", ch );
	return FALSE;
    }

    /* OLC 1.1b */
    if ( ilower <= 0 || ilower >= INT_MAX || iupper <= 0 || iupper >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "AEdit: vnum must be between 0 and %d.\n\r", INT_MAX );
	send_to_char(C_DEFAULT, output, ch );
	return FALSE;
    }

    if ( !check_range( ilower, iupper ) )
    {
	send_to_char(C_DEFAULT, "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }

    if ( get_vnum_area( ilower )
    && get_vnum_area( ilower ) != pArea )
    {
	send_to_char(C_DEFAULT, "AEdit:  Lower vnum already assigned.\n\r", ch );
	return FALSE;
    }

    pArea->lvnum = ilower;
    send_to_char(C_DEFAULT, "Lower vnum set.\n\r", ch );

    if ( get_vnum_area( iupper )
    && get_vnum_area( iupper ) != pArea )
    {
	send_to_char(C_DEFAULT, "AEdit:  Upper vnum already assigned.\n\r", ch );
	return TRUE;	/* The lower value has been set. */
    }

    pArea->uvnum = iupper;
    send_to_char(C_DEFAULT, "Upper vnum set.\n\r", ch );

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
	send_to_char(C_DEFAULT, "Syntax:  lvnum [#lower]\n\r", ch );
	return FALSE;
    }

    if ( ( ilower = atoi( lower ) ) > ( iupper = pArea->uvnum ) )
    {
	send_to_char(C_DEFAULT, "AEdit:  Value must be less than the uvnum.\n\r", ch );
	return FALSE;
    }

    /* OLC 1.1b */
    if ( ilower <= 0 || ilower >= INT_MAX || iupper <= 0 || iupper >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "AEdit: vnum must be between 0 and %d.\n\r", INT_MAX );
	send_to_char(C_DEFAULT, output, ch );
	return FALSE;
    }

    if ( !check_range( ilower, iupper ) )
    {
	send_to_char(C_DEFAULT, "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }

    if ( get_vnum_area( ilower )
    && get_vnum_area( ilower ) != pArea )
    {
	send_to_char(C_DEFAULT, "AEdit:  Lower vnum already assigned.\n\r", ch );
	return FALSE;
    }

    pArea->lvnum = ilower;
    send_to_char(C_DEFAULT, "Lower vnum set.\n\r", ch );
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
	send_to_char(C_DEFAULT, "Syntax:  uvnum [#upper]\n\r", ch );
	return FALSE;
    }

    if ( ( ilower = pArea->lvnum ) > ( iupper = atoi( upper ) ) )
    {
	send_to_char(C_DEFAULT, "AEdit:  Upper must be larger then lower.\n\r", ch );
	return FALSE;
    }

    /* OLC 1.1b */
    if ( ilower <= 0 || ilower >= INT_MAX || iupper <= 0 || iupper >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "AEdit: vnum must be between 0 and %d.\n\r", INT_MAX );
	send_to_char(C_DEFAULT, output, ch );
	return FALSE;
    }

    if ( !check_range( ilower, iupper ) )
    {
	send_to_char(C_DEFAULT, "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }

    if ( get_vnum_area( iupper )
    && get_vnum_area( iupper ) != pArea )
    {
	send_to_char(C_DEFAULT, "AEdit:  Upper vnum already assigned.\n\r", ch );
	return FALSE;
    }

    pArea->uvnum = iupper;
    send_to_char(C_DEFAULT, "Upper vnum set.\n\r", ch );

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

    sprintf( buf, "&WDescription:\n\r&Y%s&w", pRoom->description );
    strcat( buf1, buf );

    sprintf( buf, "&WName:       &z[&W%s&z]\n\r&WArea:       &z[&W%5d&z] &w%s&w\n\r",
	    pRoom->name, pRoom->area->vnum, pRoom->area->name );
    strcat( buf1, buf );

    sprintf( buf, "&WVnum:       &z[&W%5d&z]\n\r&WSector:     &z[&G%s&z]\n\r",
	    pRoom->vnum, flag_string( sector_flags, pRoom->sector_type ) );
    strcat( buf1, buf );

    sprintf( buf, "&WRoom flags: &z[&R%s&z]\n\r",
	    flag_string( room_flags, pRoom->room_flags ) );
    strcat( buf1, buf );
    
    if ( pRoom->rd > 0 )
    {
      sprintf( buf, "&WRoom damage amount: &z[&W%d&z]\n\r", pRoom->rd );
      strcat( buf1, buf );
    }

    if ( pRoom->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "&WDesc Kwds:  &z[&W" );
	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, ", " );
	}
	strcat( buf1, "&z]\n\r" );
    }

    strcat( buf1, "&WCharacters: &z[&B" );
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
	buf1[end] = '&';
	strcat( buf1, "z]\n\r" );
    }
    else
	strcat( buf1, "none&z]\n\r" );

    strcat( buf1, "&WObjects:    &z[&C" );
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
	buf1[end] = '&';
	strcat( buf1, "z]&g\n\r" );
    }
    else
	strcat( buf1, "none&z]&g\n\r" );

    for ( door = 0; door < MAX_DIR; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = pRoom->exit[door] ) )
	{
	    char word[MAX_INPUT_LENGTH];
	    char reset_state[MAX_STRING_LENGTH];
	    char *state;
	    int i, length;

	    sprintf( buf, "&W-%-5s to &z[&W%5d&z] &WKey: &z[&W%5d&z]",
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
	    strcat( buf1, " &WExit flags: &z[&R" );
	    for (; ;)
	    {
		state = one_argument( state, word );

		if ( word[0] == '\0' )
		{
		    int end;

		    end = strlen(buf1) - 1;
		    buf1[end] = '&';
   		    strcat( buf1, "z]&g\n\r" );
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
		sprintf( buf, "&WKwds: &z[&W%s&z]&g\n\r", pexit->keyword);
		strcat( buf1, buf );
	    }
	    if ( pexit->description && pexit->description[0] != '\0' )
	    {
		sprintf( buf, "%s", pexit->description );
		strcat( buf1, buf );
	    }
	}
    }

    send_to_char(C_DEFAULT, buf1, ch );
    return FALSE;
}

bool redit_rdamage( CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA *pRoom;
  char arg[MAX_STRING_LENGTH];
  int damage;
  
  EDIT_ROOM( ch, pRoom );
  
  if ( argument[0] == '\0' )
  {
    send_to_char( AT_PURPLE, "Syntax: rdamage [ammount]\n\r", ch );
    return FALSE;
  }
  
  argument = one_argument( argument, arg );
  if ( !is_number( arg ) )
  {
    send_to_char( AT_WHITE, "Amount must be the number of hp the room is to damage the CH per update.\n\r", ch );
    return FALSE;
  }
  
  damage = atoi( arg );
  pRoom->rd = damage;
  send_to_char( AT_WHITE, "Ok.\n\r", ch );
  return TRUE;
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
	if ( !pRoom->exit[door] || pRoom->exit[door]->to_room == 0 )
	{
	   send_to_char( C_DEFAULT, "REdit:  Bad Room number 0.. don't delete it :)", ch );
	   return FALSE;
	}
	if ( !pRoom->exit[door] )
	{
	    send_to_char(C_DEFAULT, "REdit:  Exit does not exist.\n\r", ch );
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

	send_to_char(C_DEFAULT, "Exit unlinked.\n\r", ch );
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
	    send_to_char(C_DEFAULT, "Syntax:  [direction] link [vnum]\n\r", ch );
	    return FALSE;
	}

	if ( !( pLinkRoom = get_room_index( atoi(arg) ) ) )
	{
	    send_to_char(C_DEFAULT, "REdit:  Non-existant room.\n\r", ch );
	    return FALSE;
	}

	if ( !IS_BUILDER( ch, pLinkRoom->area ) )
	{
	    send_to_char(C_DEFAULT, "REdit:  Cannot link to that area.\n\r", ch );
	    return FALSE;
	}

	if ( pLinkRoom->exit[rev] )
	{
	    send_to_char(C_DEFAULT, "REdit:  Remote side's exit exists.\n\r", ch );
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

	send_to_char(C_DEFAULT, "Two-way link established.\n\r", ch );
	return TRUE;
    }

    /*
     * Create room and make two-way exit.
     */
    if ( !str_cmp( command, "dig" ) )
    {
	char buf[MAX_INPUT_LENGTH];

	if ( arg[0] != '\0' && !is_number( arg ) )
	{
	    send_to_char(C_DEFAULT, "Syntax: [direction] dig <vnum>\n\r", ch );
	    return FALSE;
	}

/* Decklarean did this..  not pretty but gets the job done
 * for finding blank vnums for this dig command
 */
    if ( arg[0] == '\0' )
    {
      int value;
      AREA_DATA * pArea;
      
      pArea = ch->in_room->area;
      for ( value = pArea->lvnum; pArea->uvnum >= value; value++ )
      {
        if ( !get_room_index( value ) )
         break;
      }
      if ( value > pArea->uvnum )
      {
        send_to_char( C_DEFAULT, "REdit:  No free room vnums in this area.\n\r", ch );
        return FALSE;
      }
      sprintf( arg, "%d", value);
    }

	
	redit_create( ch, arg );		/* Create the room.	*/
	sprintf( buf, "link %s", arg );
	change_exit( ch, buf, door);		/* Create the exits.	*/
        sprintf( buf, "New room vnum: %s", arg );
	send_to_char( AT_WHITE, buf, ch );
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
	    send_to_char(C_DEFAULT, "Syntax:  [direction] room [vnum]\n\r", ch );
	    return FALSE;
	}

	if ( !( pLinkRoom = get_room_index( atoi( arg ) ) ) )
	{
	    send_to_char(C_DEFAULT, "REdit:  Non-existant room.\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	    pRoom->exit[door] = new_exit();

	pRoom->exit[door]->to_room = pLinkRoom;
	pRoom->exit[door]->vnum = value;

	send_to_char(C_DEFAULT, "One-way link established.\n\r", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "remove" ) )
    {
	if ( arg[0] == '\0' )
	{
	    send_to_char(C_DEFAULT, "Syntax:  [direction] remove [key/name/desc]\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	{
	    send_to_char(C_DEFAULT, "REdit:  Exit does not exist.\n\r", ch );
	    return FALSE;
	}

	if ( !str_cmp( argument, "key" ) )
	{
	    pRoom->exit[door]->key = 0;
            send_to_char(C_DEFAULT, "Exit key removed.\n\r", ch );                        
	    return TRUE;
	}

	if ( !str_cmp( argument, "name" ) )
	{
	    free_string( pRoom->exit[door]->keyword );
	    pRoom->exit[door]->keyword = &str_empty[0];
            send_to_char(C_DEFAULT, "Exit name removed.\n\r", ch );                        
            return TRUE;
	}

	if ( argument[0] == 'd' && !str_prefix( argument, "description" ) )
	{
	    free_string( pRoom->exit[door]->description );
	    pRoom->exit[door]->description = &str_empty[0];
            send_to_char(C_DEFAULT, "Exit description removed.\n\r", ch );                        
            return TRUE;
	}

	send_to_char(C_DEFAULT, "Syntax:  [direction] remove [key/name/desc]\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "key" ) )
    {
	OBJ_INDEX_DATA *pObjIndex;

	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char(C_DEFAULT, "Syntax:  [direction] key [vnum]\n\r", ch );
	    return FALSE;
	}

	if ( !( pObjIndex = get_obj_index( atoi( arg ) ) ) )
	{
	    send_to_char(C_DEFAULT, "REdit:  Item does not exist.\n\r", ch );
	    return FALSE;
	}

	if ( pObjIndex->item_type != ITEM_KEY )
	{
	    send_to_char(C_DEFAULT, "REdit:  Item is not a key.\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	    pRoom->exit[door] = new_exit();

	pRoom->exit[door]->key = pObjIndex->vnum;

	send_to_char(C_DEFAULT, "Exit key set.\n\r", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "name" ) )
    {
	if ( arg[0] == '\0' )
	{
	    send_to_char(C_DEFAULT, "Syntax:  [direction] name [string]\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	    pRoom->exit[door] = new_exit();

	free_string( pRoom->exit[door]->keyword );
	pRoom->exit[door]->keyword = str_dup( argument );

	send_to_char(C_DEFAULT, "Exit name set.\n\r", ch );
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

	send_to_char(C_DEFAULT, "Syntax:  [direction] desc\n\r", ch );
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

	send_to_char(C_DEFAULT, "Exit flag toggled.\n\r", ch );
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
    char command  [ MAX_INPUT_LENGTH ];
/*    char keyword  [ MAX_INPUT_LENGTH ]; */
    EDIT_ROOM(ch, pRoom);

    argument = one_argument( argument, command );
/*     one_argument( argument, keyword );   */

    if ( command[0] == '\0' || /*keyword*/ argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  ed add [keyword]\n\r", ch );
	send_to_char(C_DEFAULT, "         ed edit [keyword]\n\r", ch );
	send_to_char(C_DEFAULT, "         ed delete [keyword]\n\r", ch );
	send_to_char(C_DEFAULT, "         ed format [keyword]\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "add" ) )
    {
	if ( /*keyword*/ argument[0] == '\0' )  
	{
	    send_to_char(C_DEFAULT, "Syntax:  ed add [keyword]\n\r", ch );
	    return FALSE;
	}

	ed			=   new_extra_descr();
	ed->keyword		=   str_dup( /*keyword*/ argument );
 /*	ed->description		=   str_dup( "" );*/
	ed->next		=   pRoom->extra_descr;
	pRoom->extra_descr	=   ed;

	string_append( ch, &ed->description );

	return TRUE;
    }


    if ( !str_cmp( command, "edit" ) )
    {
	if ( /*keyword*/ argument[0] == '\0' ) 
	{
	    send_to_char(C_DEFAULT, "Syntax:  ed edit [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( ch, /*keyword*/  argument, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char(C_DEFAULT, "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	string_append( ch, &ed->description );

	return TRUE;
    }


    if ( !str_cmp( command, "delete" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;

	if (/* keyword*/ argument[0] == '\0' )
	{
	    send_to_char(C_DEFAULT, "Syntax:  ed delete [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( ch, /*keyword*/ argument, ed->keyword ) )
		break;
	    ped = ed;
	}

	if ( !ed )
	{
	    send_to_char(C_DEFAULT, "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	if ( !ped )
	    pRoom->extra_descr = ed->next;
	else
	    ped->next = ed->next;

	free_extra_descr( ed );

	send_to_char(C_DEFAULT, "Extra description deleted.\n\r", ch );
	return TRUE;
    }


    if ( !str_cmp( command, "format" ) )
    {
	if ( /*keyword*/ argument[0] == '\0' )
	{
	    send_to_char(C_DEFAULT, "Syntax:  ed format [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( ch,/*keyword*/ argument, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char(C_DEFAULT, "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	/* OLC 1.1b */
	if ( strlen(ed->description) >= (MAX_STRING_LENGTH - 4) )
	{
	    send_to_char(C_DEFAULT, "String too long to be formatted.\n\r", ch );
	    return FALSE;
	}

	ed->description = format_string( ed->description );

	send_to_char(C_DEFAULT, "Extra description formatted.\n\r", ch );
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

/* Decklarean did this */
    if ( argument[0] == '\0' )
    {
      pArea = ch->in_room->area;
      for ( value = pArea->lvnum; pArea->uvnum >= value; value++ )
      {
	if ( !get_room_index( value ) )
	 break;
      }
      if ( value > pArea->uvnum )
      {
	send_to_char( C_DEFAULT, "REdit:  No free room vnums in this area.\n\r", ch );
	return FALSE;
      }
    }



    /* OLC 1.1b */
    if ( /*argument[0] == '\0' ||*/ value <= 0 || value >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "Syntax:  create [0 < vnum < %d]\n\r", INT_MAX );
	send_to_char(C_DEFAULT, output, ch );
	return FALSE;
    }

    if (/* argument[0] == '\0' ||*/ value <= 0 )
    {
	send_to_char(C_DEFAULT, "Syntax:  create [vnum > 0]\n\r", ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );
    if ( !pArea )
    {
	send_to_char(C_DEFAULT, "REdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char(C_DEFAULT, "REdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_room_index( value ) )
    {
	send_to_char(C_DEFAULT, "REdit:  Room vnum already exists.\n\r", ch );
	return FALSE;
    }

    pRoom			= new_room_index();
    pRoom->area			= pArea;
    pRoom->vnum			= value;

/*    if ( value > top_vnum_room )
	top_vnum_room = value;*/

    iHash			= value % MAX_KEY_HASH;
    pRoom->next			= room_index_hash[iHash];
    room_index_hash[iHash]	= pRoom;
    ch->desc->pEdit		= (void *)pRoom;

    send_to_char(C_DEFAULT, "Room created.\n\r", ch );
    return TRUE;
}

bool redit_delet( CHAR_DATA *ch, char *argument )
{
   send_to_char(AT_GREY, "If you want to DELETE the room, spell it out.\n\r", ch );
   return FALSE;
}

bool redit_delete( CHAR_DATA *ch, char *argument )
{
   ROOM_INDEX_DATA   *pRoom;
   ROOM_INDEX_DATA   *prev;
   ROOM_INDEX_DATA   *location;
   ROOM_INDEX_DATA   *pRoomIndex;  	/*used to delete exits*/
   OBJ_DATA  *obj;
   CHAR_DATA *victim;
   OBJ_DATA *obj_next;
   CHAR_DATA *vnext;
   int DIR;				/*used to delete exits*/
   int index;				/*used to delete exits*/

   EDIT_ROOM(ch, pRoom);

   send_to_char(C_DEFAULT, "Deleted.\n\r", ch);

   /* CLEAR THE ROOM */
       location = get_room_index( ROOM_VNUM_LIMBO );

       for ( victim = ch->in_room->people; victim; victim = vnext )
       {
	   vnext = victim->next_in_room;
	   if ( victim->deleted )
	       continue;

	   if ( IS_NPC( victim ) && ch != victim )
	       extract_char( victim, TRUE );
	   else
	   {
	     char_from_room( victim );
	     char_to_room( victim, location );
	     do_look( victim, "auto" );
	   }
       }

       for ( obj = ch->in_room->contents; obj; obj = obj_next )
       {
	   obj_next = obj->next_content;
	   if ( obj->deleted )
	       continue;
	   extract_obj( obj );
       }



   /* begin remove room from index */
   prev = room_index_hash[pRoom->vnum % MAX_KEY_HASH];

   if ( pRoom == prev )
   {
      room_index_hash[pRoom->vnum % MAX_KEY_HASH] = pRoom->next;
   }
   else
   {
      for ( ; prev; prev = prev->next )
      {
	 if ( prev->next == pRoom )
	 {
	    prev->next = pRoom->next;
	    break;
	 }
      }

      if ( !prev )
      {
	 bug( "redit_delete: room %d not found.",
	       pRoom->vnum );
      }
   }
   /* end remove room from index*/

   /* begin remove connecting exits */
   for ( index = 0; index < MAX_KEY_HASH; index++ )
   {
     if (room_index_hash[index])
     {
       for ( pRoomIndex = room_index_hash[index];
             pRoomIndex;
             pRoomIndex = pRoomIndex->next )
       {
         for ( DIR = 0; DIR <= DIR_MAX; DIR++ )
         {
           if ( pRoomIndex->exit[DIR] )
             if ( pRoomIndex->exit[DIR]->to_room == pRoom )
             {
               free_exit( pRoomIndex->exit[DIR] );
               pRoomIndex->exit[DIR] = NULL;
               SET_BIT(pRoomIndex->area->area_flags, AREA_CHANGED);
             }
         }
       }
     }
   }


   /* end of remove connecting exits */

   /* delete room */
   free_room_index( pRoom );

   ch->desc->pEdit = NULL;
   ch->desc->editor = 0;
   return TRUE;

}

bool redit_name( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  name [name]\n\r", ch );
	return FALSE;
    }

    free_string( pRoom->name );
    pRoom->name = str_dup( argument );

    send_to_char(C_DEFAULT, "Name set.\n\r", ch );
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

    send_to_char(C_DEFAULT, "Syntax:  desc\n\r", ch );
    return FALSE;
}




bool redit_format( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    /* OLC 1.1b */
    if ( strlen(pRoom->description) >= (MAX_STRING_LENGTH - 4) )
    {
	send_to_char(C_DEFAULT, "String too long to be formatted.\n\r", ch );
	return FALSE;
    }

    pRoom->description = format_string( pRoom->description );

    send_to_char(C_DEFAULT, "String formatted.\n\r", ch );
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
	send_to_char (C_DEFAULT, "Syntax:  mreset <vnum> <max #>\n\r", ch );
	return FALSE;
    }

    if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
    {
	send_to_char(C_DEFAULT, "REdit: No mobile has that vnum.\n\r", ch );
	return FALSE;
    }

    if ( pMobIndex->area != pRoom->area ) 
/*&& get_trust( ch ) < L_CON ) */
    /* && !IS_CODER( ch )*/ 
    {
	send_to_char(C_DEFAULT, "REdit: No such mobile in this area.\n\r", ch );
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

    sprintf( output, "%s (%d) has been loaded and added to resets.\n\r"
	"There will be a maximum of %d loaded to this room.\n\r",
	capitalize( pMobIndex->short_descr ),
	pMobIndex->vnum,
	pReset->arg2 );
    send_to_char(C_DEFAULT, output, ch );
    act(C_DEFAULT, "$n has created $N!", ch, NULL, newmob, TO_ROOM );
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
    {   WEAR_IN_EYES,   ITEM_WEAR_CONTACT       },
    {   WEAR_ORBIT,     ITEM_WEAR_ORBIT         },
    {   WEAR_ORBIT_2,   ITEM_WEAR_ORBIT         },
    {   WEAR_ON_FACE,   ITEM_WEAR_FACE          },
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
    {   WEAR_WIELD_2,   ITEM_WIELD              },
    {	WEAR_HOLD,	ITEM_HOLD		},
    {   WEAR_ANKLE_L,   ITEM_WEAR_ANKLE         },
    {   WEAR_ANKLE_R,   ITEM_WEAR_ANKLE         },
    {   WEAR_EAR_L,       ITEM_WEAR_EARS         },
    {   WEAR_EAR_R,       ITEM_WEAR_EARS         },
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
	send_to_char (C_DEFAULT, "Syntax:  oreset <vnum> <args>\n\r", ch );
	send_to_char (C_DEFAULT, "        -no_args               = into room\n\r", ch );
	send_to_char (C_DEFAULT, "        -<obj_name>            = into obj\n\r", ch );
	send_to_char (C_DEFAULT, "        -<mob_name> <wear_loc> = into mob\n\r", ch );
	return FALSE;
    }

    if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
    {
	send_to_char(C_DEFAULT, "REdit: No object has that vnum.\n\r", ch );
	return FALSE;
    }

    if ( pObjIndex->area != pRoom->area ) 
	/* && get_trust( ch ) < L_CON ) */
	/* && !IS_CODER( ch ) */ 
    {
	send_to_char(C_DEFAULT, "REdit: No such object in this area.\n\r", ch );
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

	newobj = create_object( pObjIndex, pObjIndex->level ); /* Angi */
/*	newobj = create_object( pObjIndex, number_fuzzy( olevel ) );  */
	obj_to_room( newobj, pRoom );

	sprintf( output, "%s (%d) has been loaded and added to resets.\n\r",
	    capitalize( pObjIndex->short_descr ),
	    pObjIndex->vnum );
	send_to_char(C_DEFAULT, output, ch );
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
#ifdef NEW_MONEY
	newobj->cost.gold = newobj->cost.silver = newobj->cost.copper = 0;
#else
	newobj->cost = 0;
#endif
	obj_to_obj( newobj, to_obj );

	sprintf( output, "%s (%d) has been loaded into "
	    "%s (%d) and added to resets.\n\r",
	    capitalize( newobj->short_descr ),
	    newobj->pIndexData->vnum,
	    to_obj->short_descr,
	    to_obj->pIndexData->vnum );
	send_to_char(C_DEFAULT, output, ch );
    }
    else
    /*
     * Load into mobile's inventory.
     */
    if ( ( to_mob = get_char_room( ch, arg2 ) ) != NULL )
    {
	int	wear_loc;

	/* Find specific reset to load AFTER */
	RESET_DATA *pMob;
	int     reset_loc = 1;
	int     mob_num;
	int     counter = 0;

	mob_num = number_argument( arg2, arg2 );
	for ( pMob = ch->in_room->reset_first; pMob; pMob = pMob->next )
	{
	  ++reset_loc;
	  if ( pMob->arg1 == to_mob->pIndexData->vnum && ++counter == mob_num )
	    break;
	}
	if ( !pMob )
	{
	  send_to_char(C_DEFAULT, "Mobile not reset in this room.\n\r",ch);
	  return FALSE;
	}
	/* Load after all other worn/held items, but before next reset
	 * of any other type. */
	for ( pMob = pMob->next; pMob; pMob = pMob->next )
	{
	  ++reset_loc;
	  if ( pMob->command != 'G' && pMob->command != 'E' )
	    break;
	}
	if ( !pMob )
	  reset_loc = 0;

	/*
	 * Make sure the location on mobile is valid.
	 */
	if ( (wear_loc = flag_value( wear_loc_flags, argument )) == NO_FLAG )
	{
	    send_to_char(C_DEFAULT, "REdit: Invalid wear_loc.  '? wear-loc'\n\r", ch );
	    return FALSE;
	}

	/*
	 * Disallow loading a sword(WEAR_WIELD) into WEAR_HEAD.
	 */
	if ( !IS_SET( pObjIndex->wear_flags, wear_bit(wear_loc) ) )
	{
	    sprintf( output,
	        "%s (%d) has wear flags: [%s]\n\r",
	        capitalize( pObjIndex->short_descr ),
	        pObjIndex->vnum,
		flag_string( wear_flags, pObjIndex->wear_flags ) );
	    send_to_char(C_DEFAULT, output, ch );
	    return FALSE;
	}
	
	/* load it to a spot in the equ if it has a take flag - Deck */
 	if ( !IS_SET( pObjIndex->wear_flags, ITEM_TAKE ) )
	{
 	    send_to_char(C_DEFAULT, "REdit:  Object needs take flag.\n\r", ch );
	    return FALSE;
	}

	/*
	 * Can't load into same position.
	 */
	if ( ( get_eq_char( to_mob, wear_loc ) ) && ( wear_loc != -1 ) )
	{
	    send_to_char(C_DEFAULT, "REdit:  Object already equipped.\n\r", ch );
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

	add_reset( pRoom, pReset, reset_loc );

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
            case ITEM_LENSE:    olevel = number_range( 10, 20 );        break;
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
	    "%s of %s (%d) and added to resets.\n\r",
	    capitalize( pObjIndex->short_descr ),
	    pObjIndex->vnum,
	    flag_string( wear_loc_strings, pReset->arg3 ),
	    to_mob->short_descr,
	    to_mob->pIndexData->vnum );
	send_to_char(C_DEFAULT, output, ch );
    }
    else	/* Display Syntax */
    {
	send_to_char(C_DEFAULT, "REdit:  That mobile isn't here.\n\r", ch );
	return FALSE;
    }

    act(C_DEFAULT, "$n has created $p!", ch, newobj, NULL, TO_ROOM );
    return TRUE;
}

/*
 * Randomize Exits.
 * Added by Altrag.
 */
bool redit_rreset( CHAR_DATA *ch, char *argument )
{
	static const char * dir_name[6] =
	{ "North\0", "East\0", "South\0", "West\0", "Up\0", "Down\0" };

	char arg[MAX_STRING_LENGTH];
	char output[MAX_STRING_LENGTH];
	RESET_DATA *pReset;
	ROOM_INDEX_DATA *pRoom;
	int direc;
	
	EDIT_ROOM(ch, pRoom);

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( C_DEFAULT, "Syntax: rreset <last-door>\n\r", ch );
		return FALSE;
	}
	
	if ( is_number( arg ) )
		direc = atoi(arg);
	else
	{
		for ( direc = 0; direc < 6; direc++ )
			if ( UPPER(arg[0]) == dir_name[direc][0] ) break;
	}

	if ( direc < 0 || direc > 5 )
	{
		send_to_char( C_DEFAULT, "That is not a direction.\n\r", ch );
		return FALSE;
	}
	
	pReset = new_reset_data();
	pReset->command   = 'R';
	pReset->arg1      = pRoom->vnum;
	pReset->arg2      = direc;
	add_reset( pRoom, pReset, 0/* Last slot*/ );

	sprintf( output, "Exits North (0) to %s (%d) randomized.\n\r",
			 dir_name[direc], direc );
	send_to_char( C_DEFAULT, output, ch );
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
		sprintf( buf, "&z[&Wv2&z] &cLight&w: &WInfinite&z[&R-1&z]\n\r" );
            else
		sprintf( buf, "&z[&Wv2&z] &cLight&w:  &z[&R%d&z]\n\r",
			 obj->value[2] );
	    send_to_char(C_DEFAULT, buf, ch );
	    break;

	case ITEM_PORTAL:
	    sprintf( buf, "&z[&Wv0&z] &cDestination&w:&z [&Wvnum &R%d&z]", obj->value[0] );
            send_to_char( AT_BLUE, buf, ch );
            break;
	case ITEM_WAND:
	case ITEM_LENSE:
	case ITEM_STAFF:
	    if (obj->value[1] == -1 )
                sprintf( buf,
                    "&z[&Wv0&z] &cLevel&w:          &z[&R%d&z]\n\r"
                    "&z[&Wv1&z] &cCharges&w:        &z[&WInfinite&w(&R-1&w)&z]\n\r"
                    "&z[&Wv3&z] &cSpell&w:          &z[&W%s&z]\n\r",
                    obj->value[0],
		    obj->value[3] != -1 ? skill_table[obj->value[3]].name
		                    : "none" );
            else        
                sprintf( buf,
        	    "&z[&Wv0&z] &cLevel&w:          &z[&R%d&z]\n\r"
        	    "&z[&Wv1&z] &cCharges Total&w:  &z[&R%d&z]\n\r"
        	    "&z[&Wv2&z] &cCharges Left&w:   &z[&R%d&z]\n\r" 
        	    "&z[&Wv3&z] &cSpell&w:          &z[&W%s&z]\n\r",
        	    obj->value[0],
        	    obj->value[1],
        	    obj->value[2],
        	    obj->value[3] != -1 ? skill_table[obj->value[3]].name
		                    : "none" );
	    send_to_char(AT_BLUE, buf, ch );
	    break;

	case ITEM_SCROLL:
	case ITEM_POTION:
	case ITEM_PILL:
            sprintf( buf,
		"&z[&Wv0&z] &cLevel&w:  &z[&R%d&z]\n\r"
		"&z[&Wv1&z] &cSpell&w:  &z[&W%s&z]\n\r"
		"&z[&Wv2&z] &cSpell&w:  &z[&W%s&z]\n\r"
		"&z[&Wv3&z] &cSpell&w:  &z[&W%s&z]\n\r",
		obj->value[0],
		obj->value[1] != -1 ? skill_table[obj->value[1]].name
		                    : "none",
		obj->value[2] != -1 ? skill_table[obj->value[2]].name
                                    : "none",
		obj->value[3] != -1 ? skill_table[obj->value[3]].name
		                    : "none" );
	    send_to_char(C_DEFAULT, buf, ch );
	    break;

	case ITEM_WEAPON:
            sprintf( buf,
        	/* v1 and v2 not used - Deck
                "&z[&Wv1&z] &cDamage minimum&w: &z[&R%d&z]\n\r"
        	"&z[&Wv2&z] &cDamage maximum&w: &z[&R%d&z]\n\r"*/
		"&z[&Wv3&z] &cType&w:           &z[&W%s&z]\n\r",
		/*obj->value[1], v1 and v2 not used - Deck
		obj->value[2],*/
		flag_string( weapon_flags, obj->value[3] ) );
	    send_to_char(C_DEFAULT, buf, ch );
	    break;

	case ITEM_ARMOR:
	    sprintf( buf,
		"&z[&Wv0&z] &cArmor class&w: &z[&R%d&z]\n\r",
		obj->value[0] );
	    send_to_char(C_DEFAULT, buf, ch );
	    break;

	case ITEM_NOTEBOARD:
	    sprintf( buf,
		"&z[&Wv0&z] &cDecoder item&w:        &z[&R%d&z] [&W%s&z]\n\r"
		"&z[&Wv1&z] &cMinimum read level&w:  &z[&R%d&z]\n\r"
		"&z[&Wv2&z] &cMinimum write level&w: &z[&R%d&z]\n\r",
		obj->value[0], 
		get_obj_index(obj->value[0])
		    ? get_obj_index(obj->value[0])->short_descr
		    : "none",
		obj->value[1],
		obj->value[2] );
	    send_to_char(C_DEFAULT, buf, ch );
	    break;

	case ITEM_CONTAINER:
	    sprintf( buf,
		"&z[&Wv0&z] &cWeight&w: &z[&R%d &Wkg&z]\n\r"
		"&z[&Wv1&z] &cFlags&w:  &z[&W%s&z]\n\r"
		"&z[&Wv2&z] &cKey&w:    &z[&W%s&z] [&R%d&z]\n\r",
		obj->value[0],
		flag_string( container_flags, obj->value[1] ),
                get_obj_index(obj->value[2])
		    ? get_obj_index(obj->value[2])->short_descr
                    : "none", obj->value[2]);
	    send_to_char(C_DEFAULT, buf, ch );
	    break;

	case ITEM_DRINK_CON:
	    sprintf( buf,
	        "&z[&Wv0&z] &cLiquid Total&w: &z[&R%d&z]\n\r"
	        "&z[&Wv1&z] &cLiquid Left&w:  &z[&R%d&z]\n\r"
	        "&z[&Wv2&z] &cLiquid&w:       &z[&W%s&z]\n\r"
	        "&z[&Wv3&z] &cPoisoned&w:     &z[&W%s&z]\n\r",
	        obj->value[0],
	        obj->value[1],
	        flag_string( liquid_flags, obj->value[2] ),
	        obj->value[3] != 0 ? "Yes" : "No" );
	    send_to_char(C_DEFAULT, buf, ch );
	    break;

	case ITEM_BLOOD:
	    sprintf( buf,
		"&z[&Wv0&z] &cBlood Total&w: &z[&R%d&z]\n\r"
	        "&z[&Wv1&z] &cBlood Left&w:  &z[&R%d&z]\n\r", 
	        obj->value[0],
	        obj->value[1] );
	    send_to_char(C_DEFAULT, buf, ch );
	    break;

	case ITEM_FOOD:
	    sprintf( buf,
		"&z[&Wv0&z] &cFood hours&w: &z[&R%d&z]\n\r"
		"&z[&Wv3&z] &cPoisoned&w:   &z[&W%s&z]\n\r",
		obj->value[0],
		obj->value[3] != 0 ? "Yes" : "No" );
	    send_to_char(C_DEFAULT, buf, ch );
	    break;

	case ITEM_MONEY:
#ifdef NEW_MONEY
            sprintf( buf, "&z[&Wv0&z] &cGold&w:   &z[&R%d&z]\n\r"
	                  "&z[&Wv1&z] &cSilver&w: &z[&R%d&z]\n\r",
		    obj->value[0], obj->value[1] );
	    send_to_char(C_DEFAULT, buf, ch );
	    sprintf( buf, "&z[&Wv2&z] &cCopper&w: &z[&R%d&z]\n\r",
		    obj->value[2] );

#else
            sprintf( buf, "&z[&Wv0&z] &cGold&w:   &z[&R%d&z]\n\r",
		     obj->value[0] );
#endif
	    send_to_char(C_DEFAULT, buf, ch );
	    break;
    }

    return;
}



bool set_ac_type ( CHAR_DATA *ch, char *argument )
{    

    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(AT_WHITE, "Syntax:  ac_type [type #]\n\r", ch );
	send_to_char(AT_WHITE, "         type #1 = oload item.\n\r", ch );
	send_to_char(AT_WHITE, "              #2 = mload mob.\n\r", ch );
	send_to_char(AT_WHITE, "              #3 = transfer character.\n\r", ch );
	send_to_char(AT_WHITE, "              #4 = item morph.\n\r", ch );
	send_to_char(AT_WHITE, "              #5 = item cast spell.\n\r", ch );
	return FALSE;
    }
    if ( atoi( argument ) < -1 || atoi( argument ) > 5 )
	{
	set_ac_type( ch, "" );
	return FALSE;
	}
    pObj->ac_type = atoi( argument );

    send_to_char(C_DEFAULT, "Invoke type set.\n\r", ch);
    return TRUE;

}

bool set_ac_vnum ( CHAR_DATA *ch, char *argument )
{    

    OBJ_INDEX_DATA *pObj;
    int value;
    char buf [MAX_STRING_LENGTH];
    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(AT_WHITE, "Syntax:  ac_vnum [ # ]\n\r", ch );
	return FALSE;
    }
    value = atoi( argument );
    switch( pObj->ac_type )
      {
      default: return FALSE;
      case 0: 
	send_to_char( AT_GREY, "Obj ac_type does not require a vnum.\n\r", ch );
	return FALSE;
      case 1:
	if ( !get_obj_index( value ) )
	  {
	  sprintf( buf, "No ojbect with vnum %d exists.\n\r", value );
	  send_to_char( AT_GREY, buf, ch );
	  return FALSE;
	  }
	break;
      case 2:
	if ( !get_mob_index( value ) )
	  {
	  sprintf( buf, "No mobile with vnum %d exists.\n\r", value );
	  send_to_char( AT_GREY, buf, ch );
	  return FALSE;
	  }
	break;
      case 3:
	if ( !get_room_index( value ) )
	  {
	  sprintf( buf, "Room vnum %d does not exist.\n\r", value );
	  send_to_char( AT_GREY, buf, ch );
	  return FALSE;
	  }
	break;
      case 4:
	if ( !get_obj_index( value ) )
	  {
	  sprintf( buf, "No ojbect with vnum %d exists.\n\r", value );
	  send_to_char( AT_GREY, buf, ch );
	  return FALSE;
	  }
      }
    pObj->ac_vnum = value;
    sprintf( buf, "Invoke vnum set to: %d\n\r", value );
    send_to_char(C_DEFAULT, buf, ch);
    return TRUE;

}

bool set_ac_v1 ( CHAR_DATA *ch, char *argument )
{    

    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(AT_WHITE, "Syntax:  ac_v1 [ current charges ]\n\r", ch );
	return FALSE;
    }

    pObj->ac_charge[0] = atoi( argument );

    send_to_char(C_DEFAULT, "Current charge set.\n\r", ch);
    return TRUE;

}

bool set_ac_v2 ( CHAR_DATA *ch, char *argument )
{    

    OBJ_INDEX_DATA *pObj;
    int value;
    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(AT_WHITE, "Syntax:  ac_v2 [ max charges (-1 unlimited) ]\n\r", ch );
	return FALSE;
    }
    value = atoi( argument );
    if ( value < -1 ) value = -1;
    pObj->ac_charge[1] = value;

    send_to_char(C_DEFAULT, "Max Charge set.\n\r", ch);
    return TRUE;

}


bool set_ac_setspell ( CHAR_DATA *ch, char *argument )
{    

    OBJ_INDEX_DATA *pObj;
    int            spn;
    
    EDIT_OBJ(ch, pObj);

    spn = skill_lookup( argument );
    
    if ( argument[0] == '\0'
    || spn == -1 
    || (*skill_table[spn].spell_fun) == (*spell_null) )
    {
	send_to_char(AT_WHITE, "Syntax:  ac_setspell [ valid spell name ]\n\r", ch );
	return FALSE;
    }

    free_string( pObj->ac_spell );
    pObj->ac_spell = str_dup(skill_table[spn].name);

    send_to_char(C_DEFAULT, "Spell set.\n\r", ch);
    return TRUE;

}

bool set_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, int value_num, char *argument)
{
    switch( pObj->item_type )
    {
	default:
	    break;

	case ITEM_PORTAL:
	    switch ( value_num )
	    {
	    default: return FALSE;
	    case 0:
	       send_to_char(C_DEFAULT, "DESTINATION SET.\n\r\n\r", ch );
	       pObj->value[0] = atoi( argument );
	       break;
	    }
	    break;

	case ITEM_LIGHT:
	    switch ( value_num )
	    {
		default:
		    do_help( ch, "ITEM_LIGHT" );
		    return FALSE;
		case 2:
		    send_to_char(C_DEFAULT, "HOURS OF LIGHT SET.\n\r\n\r", ch );
		    pObj->value[2] = atoi( argument );
		    break;
	    }
	    break;

	case ITEM_WAND:
	case ITEM_LENSE:
	case ITEM_STAFF:
	    switch ( value_num )
	    {
		default:
		    do_help( ch, "ITEM_STAFF_WAND" );
		    return FALSE;
		case 0:
		    send_to_char(C_DEFAULT, "SPELL LEVEL SET.\n\r\n\r", ch );
		    pObj->value[0] = atoi( argument );
		    break;
		case 1:
		    send_to_char(C_DEFAULT, "TOTAL NUMBER OF CHARGES SET.\n\r\n\r", ch );
		    pObj->value[1] = atoi( argument );
		    break;
		case 2:
		    send_to_char(C_DEFAULT, "CURRENT NUMBER OF CHARGES SET.\n\r\n\r", ch );
		    pObj->value[2] = atoi( argument );
		    break;
		case 3:
		    send_to_char(C_DEFAULT, "SPELL TYPE SET.\n\r", ch );
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
	            send_to_char(C_DEFAULT, "SPELL LEVEL SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char(C_DEFAULT, "SPELL TYPE 1 SET.\n\r\n\r", ch );
	            pObj->value[1] = skill_lookup( argument );
	            break;
	        case 2:
	            send_to_char(C_DEFAULT, "SPELL TYPE 2 SET.\n\r\n\r", ch );
	            pObj->value[2] = skill_lookup( argument );
	            break;
	        case 3:
	            send_to_char(C_DEFAULT, "SPELL TYPE 3 SET.\n\r\n\r", ch );
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
	      /*  case 1:  not used -Deck
	            send_to_char(C_DEFAULT, "MINIMUM DAMAGE SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char(C_DEFAULT, "MAXIMUM DAMAGE SET.\n\r\n\r", ch );
	            pObj->value[2] = atoi( argument );
	            break; */
	        case 3:
	            send_to_char(C_DEFAULT, "WEAPON TYPE SET.\n\r\n\r", ch );
	            pObj->value[3] = flag_value( weapon_flags, argument );
	            break;
	    }
            break;

	case ITEM_ARMOR:
	    switch( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_ARMOR" );
		    return FALSE;
	        case 0:
		    send_to_char(C_DEFAULT, "ARMOR CLASS SET.\n\r\n\r", ch );
		    pObj->value[0] = atoi( argument );
		    break;
	    }
	    break;

	case ITEM_NOTEBOARD:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_NOTEBOARD" );
		    return FALSE;
		case 0:
		    send_to_char(C_DEFAULT, "DECODER VNUM SET.\n\r\n\r", ch );
		    pObj->value[0] = atoi( argument );
		    break;
		case 1:
		    if ( atoi(argument) > get_trust(ch) )
		    {
		      send_to_char(C_DEFAULT, "Limited by your trust.\n\r",ch);
		      return FALSE;
		    }
		    send_to_char(C_DEFAULT, "MINIMUM READ LEVEL SET.\n\r\n\r",ch);
		    pObj->value[1] = atoi( argument );
		    break;
		case 2:
		    if ( atoi(argument) > get_trust(ch) )
		    {
		      send_to_char(C_DEFAULT, "Limited by your trust.\n\r",ch);
		      return FALSE;
		    }
		    send_to_char(C_DEFAULT, "MINIMUM WRITE LEVEL SET.\n\r\n\r",ch);
		    pObj->value[2] = atoi( argument );
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
	            send_to_char(C_DEFAULT, "WEIGHT CAPACITY SET.\n\r\n\r", ch );
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
	            send_to_char(C_DEFAULT, "CONTAINER TYPE SET.\n\r\n\r", ch );
	            break;
		case 2:
		    if ( atoi(argument) != 0 )
		    {
			if ( !get_obj_index( atoi( argument ) ) )
			{
			    send_to_char(C_DEFAULT, "THERE IS NO SUCH ITEM.\n\r\n\r", ch );
			    return FALSE;
			}

			if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY )
			{
			    send_to_char(C_DEFAULT, "THAT ITEM IS NOT A KEY.\n\r\n\r", ch );
			    return FALSE;
			}
		    }
		    send_to_char(C_DEFAULT, "CONTAINER KEY SET.\n\r\n\r", ch );
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
	            send_to_char(C_DEFAULT, "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
		    break;
	        case 1:
	            send_to_char(C_DEFAULT, "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char(C_DEFAULT, "LIQUID TYPE SET.\n\r\n\r", ch );
	            pObj->value[2] = flag_value( liquid_flags, argument );
	            break;
	        case 3:
	            send_to_char(C_DEFAULT, "POISON VALUE TOGGLED.\n\r\n\r", ch );
	            pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
	            break;
	    }
            break;

	case ITEM_BLOOD:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_DRINK" );
	            return FALSE;
	        case 0:
	            send_to_char(C_DEFAULT, "MAXIMUM AMOUT OF BLOOD SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char(C_DEFAULT, "CURRENT AMOUNT OF BLOOD SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
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
		    send_to_char(C_DEFAULT, "HOURS OF FOOD SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 3:
	            send_to_char(C_DEFAULT, "POISON VALUE TOGGLED.\n\r\n\r", ch );
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
	            send_to_char(C_DEFAULT, "GOLD AMOUNT SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
#ifdef NEW_MONEY
		    pObj->cost.gold = pObj->value[0];
#else
		    pObj->cost = pObj->value[0];
#endif
	            break;
#ifdef NEW_MONEY
		case 1:
		    send_to_char(C_DEFAULT, "SILVER AMOUNT SET.\n\r\n\r", ch );
		    pObj->value[1] = atoi( argument );
		    pObj->cost.silver = pObj->value[1];
		    break;
		case 2:
		    send_to_char(C_DEFAULT, "COPPER AMOUNT SET.\n\r\n\r", ch );
		    pObj->value[2] = atoi( argument );
		    pObj->cost.copper = pObj->value[2];
		    break;
#endif
	    }
            break;
    }

    show_obj_values( ch, pObj );

    return TRUE;
}



bool oedit_show( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    OBJ_INDEX_DATA *pJoinObj;  /* Used to displaying join obj names -Deck */
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int cnt;

    EDIT_OBJ(ch, pObj);

    sprintf( buf, "&cName&w:&z        [&W%s&z]\n\r"
		  "&cArea&w:&z        [&R%5d&z] [&W%s&z]\n\r",
	pObj->name,
	!pObj->area ? -1        : pObj->area->vnum,
	!pObj->area ? "No Area" : pObj->area->name );
    send_to_char(C_DEFAULT, buf, ch );


    sprintf( buf, "&cVnum&w:&z        [&R%5d&z]\n\r"
		  "&cType&w:&z        [&W%s&z]\n\r",
	pObj->vnum,
	flag_string( type_flags, pObj->item_type ) );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&cWear flags&w:&z  [&W%s&z]\n\r",
	flag_string( wear_flags, pObj->wear_flags ) );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&cExtra flags&w:&z [&W%s&z]\n\r",
	flag_string( extra_flags, pObj->extra_flags ) );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&cAnti-Class&w:&z  [&W%s&z]\n\r",
	flag_string( anti_class_flags, pObj->anti_class_flags ) );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&cAnti-Race&w:&z   [&W%s&z]\n\r",
	flag_string( anti_race_flags, pObj->anti_race_flags ) );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&cLevel&w:&z       [&R%d&z]\n\r", pObj->level );
    send_to_char(C_DEFAULT, buf, ch);
#ifdef NEW_MONEY
    sprintf( buf, "&cWeight&w:&z      [&R%d&z]\n\r"
                  "&cGold Cost&w:&z   [&R%d&z]\n\r",
	pObj->weight, pObj->cost.gold );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&cSilver Cost&w:&z [&R%d&z]\n\r"
		  "&cCopper Cost&w:&z [&R%d&z]\n\r",
        pObj->cost.silver, pObj->cost.copper );
#else
    sprintf( buf, "&cWeight&w:&z      [&R%d&z]\n\r"
		  "&cCost&w:&z        [&R%d&z]\n\r",
	pObj->weight, pObj->cost );
#endif
    send_to_char(C_DEFAULT, buf, ch );
    if ( pObj->join )
    {
      pJoinObj = get_obj_index( pObj->join );
      sprintf( buf, "&cJoins to create&w: &z[&R%d&z] "
		    "[&W%s&z]\n\r", pObj->join, 
	     pJoinObj ? pJoinObj->short_descr : "&RError: No such object."  );
      send_to_char( C_DEFAULT, buf, ch );
    }
    if ( pObj->sep_one )
    {
      pJoinObj = get_obj_index( pObj->sep_one );
      sprintf( buf, "&cFirst seperated vnum is&w: &z[&R%d&z] "
		    "[&W%s&z]\n\r", pObj->sep_one, 
	     pJoinObj ? pJoinObj->short_descr : "&RError: No such object."  );
      send_to_char( C_DEFAULT, buf, ch );
    }
    if ( pObj->sep_two )
    {
      pJoinObj = get_obj_index( pObj->sep_two );
      sprintf( buf, "&cSecond seperated vnum is&w:&z [&R%d&z] "
		    "[&W%s&z]\n\r", pObj->sep_two,
	     pJoinObj ? pJoinObj->short_descr : "&RError: No such object."  );
      send_to_char( C_DEFAULT, buf, ch );
    }
    sprintf( buf, "&cInvoke Type&w:&z [&R%d&z]  "
		  "&c Invoke Vnum&w:&z [&R%d&z]\n\r", 
             pObj->ac_type, pObj->ac_vnum );
    send_to_char(C_DEFAULT, buf, ch );
    if ( pObj->ac_charge[1] == -1 )
        sprintf( buf, "&cInvoke is&w:   &WPERMANENT\n\r" );
    else
        sprintf( buf, "&cInvoke charges&w:&z[&R%d&w/&R%d&z]\n\r",
                 pObj->ac_charge[0], pObj->ac_charge[1] );
    send_to_char( C_DEFAULT, buf, ch );
    if ( ( pObj->ac_type == 5 ) && ( pObj->ac_spell != '\0' ) )
        sprintf( buf, "&cInvoke Spell&w:&z[&W%s&z]\n\r",
		 pObj->ac_spell );
    else sprintf( buf,"&cInvoke Spell&w:&z[&W!NONE!&z]\n\r" );
    send_to_char( C_DEFAULT, buf, ch );
    if ( pObj->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char(C_DEFAULT, "&cEx desc kwd&w: ", ch );

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    send_to_char(AT_DGREY, "[", ch );
	    send_to_char(AT_WHITE, ed->keyword, ch );
	    send_to_char(AT_DGREY, "]", ch );
	}

	send_to_char(AT_YELLOW, "\n\r", ch );
    }

    sprintf( buf, "&cShort desc&w:&z  [&W%s&z]\n\r"
		  "&cLong desc&w:\n\r&W     %s\n\r",
	pObj->short_descr, pObj->description );
    send_to_char(C_DEFAULT, buf, ch );

    for ( cnt = 0, paf = pObj->affected; paf; paf = paf->next )
    {
	if ( cnt == 0 )
	{
	send_to_char(C_DEFAULT, "&cNumber Modifier     Affects\n\r", ch );
	send_to_char(C_DEFAULT, "&z------ -------- ---------------\n\r", ch );
	}
	sprintf( buf, "&z[&R%4d&z] [&R%6d&z] &z[&W%-13s&z]\n\r", cnt,
	    paf->modifier,
	    flag_string( apply_flags, paf->location ) );
	send_to_char(C_DEFAULT, buf, ch );
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
    char buf[MAX_STRING_LENGTH];
    char loc[MAX_STRING_LENGTH];
    char mod[MAX_STRING_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument( argument, loc );
    one_argument( argument, mod );

    if ( loc[0] == '\0' || mod[0] == '\0' || !is_number( mod ) )
    {
	send_to_char(C_DEFAULT, "Syntax:  addaffect [location] [#mod]\n\r", ch );
	return FALSE;
    }
    if ( !str_cmp(
	 flag_string( apply_flags, flag_value( apply_flags, loc ) ),
	"none" ) )
	{
	sprintf( buf, "Unknown affect %s.\n\r", loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}

    pAf             =   new_affect();
    pAf->location   =   flag_value( apply_flags, loc );
    pAf->modifier   =   atoi( mod );
    pAf->type       =   -1;
    pAf->duration   =   -1;
/*    pAf->bitvector  =   0; */
    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;

    send_to_char(C_DEFAULT, "Affect added.\n\r", ch);
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
	send_to_char(C_DEFAULT, "Syntax:  delaffect [#affect]\n\r", ch );
	return FALSE;
    }

    value = atoi( affect );

    if ( value < 0 )
    {
	send_to_char(C_DEFAULT, "Only non-negative affect-numbers allowed.\n\r", ch );
	return FALSE;
    }

    if ( !( pAf = pObj->affected ) )
    {
	send_to_char(C_DEFAULT, "OEdit:  Non-existant affect.\n\r", ch );
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
	     send_to_char(C_DEFAULT, "No such affect.\n\r", ch );
	     return FALSE;
	}
    }

    send_to_char(C_DEFAULT, "Affect removed.\n\r", ch);
    return TRUE;
}



bool oedit_name( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  name [string]\n\r", ch );
	return FALSE;
    }

    free_string( pObj->name );
    pObj->name = str_dup( argument );

    send_to_char(C_DEFAULT, "Name set.\n\r", ch);
    return TRUE;
}



bool oedit_short( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  short [string]\n\r", ch );
	return FALSE;
    }

    free_string( pObj->short_descr );
    pObj->short_descr = str_dup( argument );
/*    pObj->short_descr[0] = LOWER( pObj->short_descr[0] );*/

    send_to_char(C_DEFAULT, "Short description set.\n\r", ch);
    return TRUE;
}



bool oedit_long( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  long [string]\n\r", ch );
	return FALSE;
    }

    free_string( pObj->description );
    pObj->description = str_dup( argument );
    pObj->description[0] = UPPER( pObj->description[0] );

    send_to_char(C_DEFAULT, "Long description set.\n\r", ch);
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

/* oedit_join is for setting the vnum of which an object can be joined to */
bool oedit_join( CHAR_DATA *ch, char *argument )
{
   OBJ_INDEX_DATA *pObj;
   char arg[MAX_STRING_LENGTH];
   int value = 0;

   EDIT_OBJ( ch, pObj );

   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' || !is_number( arg ) )
   {
      send_to_char( AT_WHITE, " &pSyntax: ojoin [vnum]\n\r", ch );
      return FALSE;
   }

   value = atoi( arg );

   if ( value < 0 || value > 33000 )
   {
      send_to_char(AT_WHITE, "Invalid vnum.\n\r", ch );
      return FALSE;
   }

   pObj->join = value;
   send_to_char( AT_WHITE, "Ok.\n\r", ch );
   return TRUE;
}

/* oedit_sepone is for setting the first vnum which an object can seperate
 * into.
 */
bool oedit_sepone( CHAR_DATA *ch, char *argument )
{
  OBJ_INDEX_DATA *pObj;
  char arg[MAX_STRING_LENGTH];
  int value = 0;

  EDIT_OBJ( ch, pObj );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\0' || !is_number( arg ) )
  {
     send_to_char( AT_WHITE, " &pSyntax:  osepone [vnum]\n\r", ch );
     return FALSE;
  }

  value = atoi( arg );
  if ( value < 0 || value > 33000 )
  {
    send_to_char( AT_WHITE, "Invalid vnum.\n\r", ch );
    return FALSE;
  }
  pObj->sep_one = value;
  send_to_char( AT_WHITE, "Ok.\n\r", ch );
  return TRUE;
}

/* oedit_septwo is for setting the second vnum which an object splits into */
bool oedit_septwo( CHAR_DATA *ch, char *argument )
{
  OBJ_INDEX_DATA *pObj;
  char arg[MAX_STRING_LENGTH];
  int value = 0;

  EDIT_OBJ( ch, pObj );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\0' || !is_number( arg ) )
  {
     send_to_char( AT_WHITE, " &pSyntax: oseptwo [vnum]\n\r", ch );
     return FALSE;
  }

  value = atoi( arg );
  if ( value < 0 || value > 33000 )
  {
    send_to_char( AT_WHITE, "Invalid vnum.\n\r", ch );
    return FALSE;
  }
  pObj->sep_two = value;
  send_to_char( AT_WHITE, "Ok.\n\r", ch );
  return TRUE;
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
	send_to_char(C_DEFAULT, "Syntax:  weight [number]\n\r", ch );
	return FALSE;
    }

    pObj->weight = atoi( argument );

    send_to_char(C_DEFAULT, "Weight set.\n\r", ch);
    return TRUE;
}



bool oedit_cost( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
#ifdef NEW_MONEY
    char arg  [ MAX_STRING_LENGTH ];
    char arg2 [ MAX_STRING_LENGTH ];
#endif
    EDIT_OBJ(ch, pObj);
#ifdef NEW_MONEY
 
    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );
    
    if( is_number( arg2 ) )
    {
      	if( !str_cmp( arg, "gold" ) )
	{
	  pObj->cost.gold = atoi( arg2 );
	  send_to_char( C_DEFAULT, "Gold cost set.\n\r", ch );
	}
	else if ( !str_cmp( arg, "silver" ) )
	{
	  pObj->cost.silver = atoi( arg2 );
	  send_to_char( C_DEFAULT, "Silver cost set.\n\r", ch );
	}
	else if ( !str_cmp( arg, "copper" ) )
	{
	  pObj->cost.copper = atoi( arg2 );
	  send_to_char( C_DEFAULT, "Copper cost set.\n\r", ch );
	}
  	else
	{
	  send_to_char( C_DEFAULT, "Invalid currency type.\n\r", ch );
	  return FALSE;
	}
    }
    else
    {
  	send_to_char( C_DEFAULT, "Invalid amount.\n\r", ch );
	return FALSE;
    }

#else
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(C_DEFAULT, "Syntax:  cost [number]\n\r", ch );
	return FALSE;
    }

    pObj->cost = atoi( argument );

    send_to_char(C_DEFAULT, "Cost set.\n\r", ch);
#endif
    return TRUE;
}

bool oedit_level( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(C_DEFAULT, "Syntax:  level [number]\n\r", ch );
	return FALSE;
    }

    pObj->level = atoi( argument );

    send_to_char(C_DEFAULT, "Level set.\n\r", ch);
    return TRUE;
}


bool oedit_create( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    int  value;
    int  iHash;

    value = atoi( argument );

    if ( argument[0] == '\0' )
    {
      pArea = ch->in_room->area;
      for ( value = pArea->lvnum; pArea->uvnum >= value; value++ )
      {
	if ( !get_obj_index( value ) )
	 break;
      }
      if ( value > pArea->uvnum )
      {
	send_to_char( C_DEFAULT, "OEdit:  No free object vnums in this area.\n\r", ch );
	return FALSE;
      }
    }

    /* OLC 1.1b */

    if ( /*argument[0] == '\0' ||*/ value <= 0 || value >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "Syntax:  create [0 < vnum < %d]\n\r", INT_MAX );
	send_to_char(C_DEFAULT, output, ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );
    if ( !pArea )
    {
	send_to_char(C_DEFAULT, "OEdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char(C_DEFAULT, "OEdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_obj_index( value ) )
    {
	send_to_char(C_DEFAULT, "OEdit:  Object vnum already exists.\n\r", ch );
	return FALSE;
    }

    pObj			= new_obj_index();
    pObj->vnum			= value;
    pObj->area			= pArea;
    free_string( pObj->ac_spell );
    pObj->ac_spell 		= str_dup(skill_table[0].name);

/*    if ( value > top_vnum_obj )
	top_vnum_obj = value; */

    iHash			= value % MAX_KEY_HASH;
    pObj->next			= obj_index_hash[iHash];
    obj_index_hash[iHash]	= pObj;
    ch->desc->pEdit		= (void *)pObj;

    send_to_char(C_DEFAULT, "Object Created.\n\r", ch );
    return TRUE;
}
bool oedit_delet( CHAR_DATA *ch, char *argument )
{
   send_to_char( AT_GREY, "If you want to DELETE this object, spell it out.\n\r", ch );
   return FALSE;
}
bool oedit_delete( CHAR_DATA *ch, char *argument )
{
   OBJ_INDEX_DATA   *pObj;
   OBJ_INDEX_DATA   *prev;

   EDIT_OBJ(ch, pObj);

   /* begin remove obj from index */
   prev = obj_index_hash[pObj->vnum % MAX_KEY_HASH];

   if ( pObj == prev )
   {
      obj_index_hash[pObj->vnum % MAX_KEY_HASH] = pObj->next;
   }
   else
   {
      for ( ; prev; prev = prev->next )
      {
	 if ( prev->next == pObj )
	 {
	    prev->next = pObj->next;
	    break;
	 }
      }

      if ( !prev )
      {
	 bug( "oedit_delete: obj %d not found.",
	       pObj->vnum );
      }
   }
   /* end remove obj from index*/

   /* delete obj */
   free_obj_index( pObj );

   ch->desc->pEdit = NULL;
   ch->desc->editor = 0;
   send_to_char(C_DEFAULT, "Deleted.\n\r", ch);
   return TRUE;

}

bool oedit_ed( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    EXTRA_DESCR_DATA *ed;
    char command[MAX_INPUT_LENGTH];
/*    char keyword[MAX_INPUT_LENGTH]; */

    EDIT_OBJ(ch, pObj);

    argument = one_argument( argument, command );
/*    one_argument( argument, keyword ); */

    if ( command[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  ed add [keyword]\n\r", ch );
	send_to_char(C_DEFAULT, "         ed delete [keyword]\n\r", ch );
	send_to_char(C_DEFAULT, "         ed edit [keyword]\n\r", ch );
	send_to_char(C_DEFAULT, "         ed format [keyword]\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "add" ) )
    {
	if ( /*keyword*/ argument[0] == '\0' )
	{
	    send_to_char(C_DEFAULT, "Syntax:  ed add [keyword]\n\r", ch );
	    return FALSE;
	}

	ed                  =   new_extra_descr();
	ed->keyword         =   str_dup( /*keyword*/ argument );
	ed->next            =   pObj->extra_descr;
	pObj->extra_descr   =   ed;

	string_append( ch, &ed->description );

	return TRUE;
    }

    if ( !str_cmp( command, "edit" ) )
    {
	if ( /*keyword*/ argument[0] == '\0' )
	{
	    send_to_char(C_DEFAULT, "Syntax:  ed edit [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( ch, /*keyword*/ argument, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char(C_DEFAULT, "OEdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	string_append( ch, &ed->description );

	return TRUE;
    }

    if ( !str_cmp( command, "delete" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;

	if ( /*keyword*/ argument[0] == '\0' )
	{
	    send_to_char(C_DEFAULT, "Syntax:  ed delete [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( ch, /*keyword*/ argument, ed->keyword ) )
		break;
	    ped = ed;
	}

	if ( !ed )
	{
	    send_to_char(C_DEFAULT, "OEdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	if ( !ped )
	    pObj->extra_descr = ed->next;
	else
	    ped->next = ed->next;

	free_extra_descr( ed );

	send_to_char(C_DEFAULT, "Extra description deleted.\n\r", ch );
	return TRUE;
    }


    if ( !str_cmp( command, "format" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;

	if ( /*keyword*/ argument[0] == '\0' )
	{
	    send_to_char(C_DEFAULT, "Syntax:  ed format [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( ch, /*keyword*/ argument, ed->keyword ) )
		break;
	    ped = ed;
	}

	if ( !ed )
	{
                send_to_char(C_DEFAULT, "OEdit:  Extra description keyword not found.\n\r", ch );
                return FALSE;
	}

	/* OLC 1.1b */
	if ( strlen(ed->description) >= (MAX_STRING_LENGTH - 4) )
	{
	    send_to_char(C_DEFAULT, "String too long to be formatted.\n\r", ch );
	    return FALSE;
	}

	ed->description = format_string( ed->description );

	send_to_char(C_DEFAULT, "Extra description formatted.\n\r", ch );
	return TRUE;
    }

    oedit_ed( ch, "" );
    return FALSE;
}


/*
 * Clan Editor Functions.
 */
bool cedit_show( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;
    char buf[MAX_STRING_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;

    EDIT_CLAN(ch, pClan);

    sprintf( buf, "&PName:&Y        [%s&w]\n\r&PDeity:&Y       [%s]\n\r",
	pClan->name, pClan->diety );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&PClan:&Y        [%3d]\n\r",
	pClan->vnum );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&PRecall:&Y      [%5d] %s\n\r", pClan->recall,
	get_room_index( pClan->recall )
	? get_room_index( pClan->recall )->name : "none" );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&PCivil Pkill:&Y [%3s]\n\r",
             IS_SET(pClan->settings, CLAN_CIVIL_PKILL) ? "YES" : "NO" );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&PPkill:&Y       [%3s]\n\r",
 	     IS_SET(pClan->settings, CLAN_PKILL) ? "YES" : "NO" );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&PMembers:&Y     [%8d]\n\r", pClan->members );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&PPkills:&Y      [%8d]\n\r", pClan->pkills  );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&PPkilled:&Y     [%8d]\n\r", pClan->pdeaths );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&PMkills:&Y      [%8d]\n\r", pClan->mkills  );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&PLeader:&Y      [%10s]&Y%s&R%s\n\r",
	     pClan->leader,
             IS_SET(pClan->settings, CLAN_LEADER_INDUCT) ? " Can induct." : "",
             pClan->isleader ? "" : " Position open." );
    send_to_char( C_DEFAULT, buf, ch );
    sprintf( buf, "&PCouncil:&Y     [%10s]&Y%s&R%s\n\r",
	     pClan->first,
             IS_SET(pClan->settings, CLAN_FIRST_INDUCT) ? " Can induct." : "",
	     pClan->isfirst ? "" : " Position open." );
    send_to_char( C_DEFAULT, buf, ch );
    sprintf( buf, "&PCenturion:&Y   [%10s]&Y%s&R%s\n\r", 
	     pClan->second,
             IS_SET(pClan->settings, CLAN_SECOND_INDUCT) ? " Can induct." : "",
             pClan->issecond ? "" : " Position open." );
    send_to_char( C_DEFAULT, buf, ch );
    sprintf( buf, "&PChampion:&Y    [%10s]&Y%s&R%s\n\r",
	     pClan->champ,
             IS_SET(pClan->settings, CLAN_CHAMP_INDUCT) ? " Can induct." : "",
	     pClan->ischamp ? "" : " Position open." );
    send_to_char( C_DEFAULT, buf, ch );  
/*    sprintf( buf,
        "Object:      [%5d], [%5d], [%5d]\n\r",
        pClan->obj_vnum_1, pClan->obj_vnum_2, pClan->obj_vnum_3  ); */
    sprintf ( buf, "&PObjects:\n\r" );
    send_to_char(C_DEFAULT, buf, ch);
    if IS_SET(pClan->settings,CLAN_PKILL)
    {
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_1 )))
      sprintf( buf, "&YLevel 50:&w  [%5d] %s\n\r",
               pClan->obj_vnum_1, pObjIndex->short_descr );
     else
      sprintf( buf, "&YLevel 50:&w  [%5d] &RNo such object!!\n\r",
               pClan->obj_vnum_1);
     send_to_char(C_DEFAULT, buf, ch );
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_2 )))
      sprintf( buf, "&YLevel 75:&w  [%5d] %s\n\r",
               pClan->obj_vnum_2, pObjIndex->short_descr );
     else
      sprintf( buf, "&YLevel 75:&w  [%5d] &RNo such object!!\n\r",
               pClan->obj_vnum_1);
     send_to_char(C_DEFAULT, buf, ch );
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_3 )))
      sprintf( buf, "&YLevel 100:&w [%5d] %s\n\r",
               pClan->obj_vnum_3, pObjIndex->short_descr );
     else
      sprintf( buf, "&YLevel 100:&w [%5d] &RNo such object!!\n\r",
               pClan->obj_vnum_1);
     send_to_char(C_DEFAULT, buf, ch );
    }
    else
    {
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_1 )))
      sprintf( buf, "&YLevel 30:&w  [%5d] %s\n\r",
               pClan->obj_vnum_1, pObjIndex->short_descr );
     else
      sprintf( buf, "&YLevel 30:&w  [%5d] &RNo such object!!\n\r",
               pClan->obj_vnum_1);
     send_to_char(C_DEFAULT, buf, ch );
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_2 )))
      sprintf( buf, "&YLevel 65:&w  [%5d] %s\n\r",
               pClan->obj_vnum_2, pObjIndex->short_descr );
     else
      sprintf( buf, "&YLevel 65:&w  [%5d] &RNo such object!!\n\r",
               pClan->obj_vnum_1);
     send_to_char(C_DEFAULT, buf, ch );
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_3 )))
      sprintf( buf, "&YLevel 100:&w [%5d] %s\n\r",
               pClan->obj_vnum_3, pObjIndex->short_descr );
     else
      sprintf( buf, "&YLevel 100:&w [%5d] &RNo such object!!\n\r",
               pClan->obj_vnum_1);
     send_to_char(C_DEFAULT, buf, ch );
    }

    sprintf( buf, "&PDescription:\n\r%s\n\r", pClan->description );
    send_to_char(C_DEFAULT, buf, ch );
    
    return FALSE;
}



bool cedit_create( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;
    int  value;

    value = atoi( argument );

    /* OLC 1.1b */
    if ( argument[0] == '\0' || value <= 0 || value >= MAX_CLAN )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "Syntax:  cedit create [1 < vnum < %d]\n\r",
		 MAX_CLAN );
	send_to_char(C_DEFAULT, output, ch );
	return FALSE;
    }


    if ( get_clan_index( value ) )
    {
	send_to_char(C_DEFAULT, "CEdit:  Clan vnum already exists.\n\r", ch );
	return FALSE;
    }

    pClan			= new_clan_index();
    pClan->vnum			= value;
/*    iHash			= value % MAX_KEY_HASH;
    pClan->next			= clan_index_hdata;
    clan_index_hash[iHash]	= pClan;*/
/*    if ( !clan_first )
      clan_first = pClan;
    if ( clan_last )
      clan_last->next = pClan;
    clan_last->next             = pClan;
    clan_last                   = pClan;*/
    clan_sort(pClan);
    ch->desc->pEdit		= (void *)pClan;

    send_to_char(C_DEFAULT, "Clan Created.\n\r", ch );
    return TRUE;
}

bool cedit_members( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(AT_WHITE, "Syntax: &Bmembers [number]\n\r", ch );
	return FALSE;
    }

    pClan->members = atoi( argument );

    send_to_char(C_DEFAULT, "Members set.\n\r", ch);
    return TRUE;
}

bool cedit_pkills( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(AT_WHITE, "Syntax: &Bpkills [number]\n\r", ch );
	return FALSE;
    }

    pClan->pkills = atoi( argument );

    send_to_char(C_DEFAULT, "Pkills set.\n\r", ch);
    return TRUE;
}

bool cedit_pkilled( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(AT_WHITE, "Syntax: &Bpkilled [number]\n\r", ch );
	return FALSE;
    }

    pClan->pdeaths = atoi( argument );

    send_to_char(C_DEFAULT, "PKilled set.\n\r", ch);
    return TRUE;
}

bool cedit_mkills( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(AT_WHITE, "Syntax: &Bmkills [number]\n\r", ch );
	return FALSE;
    }

    pClan->mkills = atoi( argument );

    send_to_char(C_DEFAULT, "Mkills set.\n\r", ch);
    return TRUE;
}

bool cedit_name( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  name [string]\n\r", ch );
	return FALSE;
    }

    free_string( pClan->name );
    pClan->name = str_dup( argument );

    send_to_char(C_DEFAULT, "Name set.\n\r", ch);
    return TRUE;
}

bool cedit_diety( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  deity [string]\n\r", ch );
	return FALSE;
    }

    free_string( pClan->diety );
    pClan->diety = str_dup( argument );

    send_to_char(C_DEFAULT, "Deity set.\n\r", ch);
    return TRUE;
}

bool cedit_civil( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    TOGGLE_BIT (pClan->settings, CLAN_CIVIL_PKILL);

    if IS_SET( pClan->settings, CLAN_CIVIL_PKILL)
	send_to_char(C_DEFAULT, "Clan switched to Civil Pkill.\n\r", ch);
    else
	send_to_char(C_DEFAULT, "Clan switched to No Civil Pkill.\n\r", ch);

    return TRUE;
}


bool cedit_pkill( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    TOGGLE_BIT (pClan->settings, CLAN_PKILL);

    if IS_SET( pClan->settings, CLAN_PKILL)
	send_to_char(C_DEFAULT, "Clan switched to Pkill.\n\r", ch);
    else
	send_to_char(C_DEFAULT, "Clan switched to Peace.\n\r", ch);

    return TRUE;
}

bool cedit_object( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;
    char number[MAX_INPUT_LENGTH];
    char onum[MAX_INPUT_LENGTH];
    int  value;
    int  vnum;

    argument = one_argument( argument, number );
    argument = one_argument( argument, onum );

    EDIT_CLAN(ch, pClan);

    if ( number[0] == '\0' )
    {
     send_to_char( C_DEFAULT, "Syntax: object <1/2/3> <vnum>\n\r", ch );
     return FALSE;
    }

    value = atoi( number );
    vnum  = atoi( onum );

    if ( ( value < 1 ) || ( value > 3 ) )
       return FALSE;

    switch ( value )
    {
     case 1:
	pClan->obj_vnum_1 = vnum;
	break;
     case 2:
	pClan->obj_vnum_2 = vnum;
	break;
     case 3:
	pClan->obj_vnum_3 = vnum;
	break;
     default:
	break;
    }

    send_to_char( C_DEFAULT, "Object vnum set.\n\r", ch );
    return TRUE;
}

bool cedit_power( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;
    char number[MAX_INPUT_LENGTH];
    char onum[MAX_INPUT_LENGTH];
    int  value;

    argument = one_argument( argument, number );
    argument = one_argument( argument, onum );

    EDIT_CLAN(ch, pClan);

    if ( number[0] == '\0' )
    {
     send_to_char( C_DEFAULT, "Syntax: power <1/2/3/4> <name>\n\r", ch );
     send_to_char(AT_WHITE, "Valid Levels are as follows\n\r", ch );
     send_to_char(AT_WHITE, "       1 -> Centurion of Clan.\n\r", ch );
     send_to_char(AT_WHITE, "       2 -> Council of Clan.\n\r", ch );
     send_to_char(AT_WHITE, "       3 -> Leader of Clan.\n\r", ch );
     send_to_char(AT_WHITE, "       4 -> Champion of Clan.\n\r", ch );
     send_to_char(AT_WHITE, "To clear a position, in the <name> field type: none", ch );
     return FALSE;
    }

    value = atoi( number );

    if ( ( value < 1 ) || ( value > 4 ) )
       return FALSE;

    switch ( value )
    {
     case 4:
        free_string( pClan->champ );
	if (!str_cmp(onum, "none" ))
	{
	 pClan->champ = str_dup( "none" );
	 pClan->ischamp = FALSE;
	}
	else
	{
	 pClan->champ = str_dup( onum );
	 pClan->ischamp = TRUE;
	}
	break;
     case 3:
        free_string( pClan->leader );
	if (!str_cmp(onum, "none" ))
	{
	 pClan->leader = str_dup( "none" );
	 pClan->isleader = FALSE;
	}
	else
	{
	 pClan->leader = str_dup( onum );
	 pClan->isleader = TRUE;
	}
	break;
     case 2:
        free_string( pClan->first );
	if (!str_cmp(onum, "none" ))
	{
	 pClan->first = str_dup( "none" );
	 pClan->isfirst = FALSE;
	}
	else
	{
	 pClan->first = str_dup( onum );
	 pClan->isfirst = TRUE;
	}
	break;
     case 1:
        free_string( pClan->second );
	if (!str_cmp(onum, "none" ))
	{
	 pClan->second = str_dup( "none" );
	 pClan->issecond = FALSE;
	}
	else
	{
	 pClan->second = str_dup( onum );
	 pClan->issecond = TRUE;
	}
	break;
     default:
	break;
    }

    send_to_char( C_DEFAULT, "Power seat set.\n\r", ch );
    return TRUE;
}

bool cedit_induct ( CHAR_DATA *ch, char *argument )
{
     CLAN_DATA *pClan;
     int       value;

     EDIT_CLAN(ch, pClan);

     value = atoi(argument);

     if ( ( argument[0] == '\0' ) || ( ( value < 1 ) || ( value > 4 ) ) )
     {
      send_to_char(C_DEFAULT, "Sets which clan position can induct.", ch );
      send_to_char(C_DEFAULT, "Syntax: induct <1/2/3/4>\n\r", ch );
      send_to_char(AT_WHITE, "Valid positions are as follows\n\r", ch );
      send_to_char(AT_WHITE, "       1 -> Centurion of Clan.\n\r", ch );
      send_to_char(AT_WHITE, "       2 -> Council of Clan.\n\r", ch );
      send_to_char(AT_WHITE, "       3 -> Leader of Clan.\n\r", ch );
      send_to_char(AT_WHITE, "       4 -> Champion of Clan.\n\r", ch );
      return FALSE;
     }

     switch ( value )
     {
      case 4: TOGGLE_BIT(pClan->settings, CLAN_CHAMP_INDUCT  ); break;
      case 3: TOGGLE_BIT(pClan->settings, CLAN_LEADER_INDUCT ); break;
      case 2: TOGGLE_BIT(pClan->settings, CLAN_FIRST_INDUCT  ); break;
      case 1: TOGGLE_BIT(pClan->settings, CLAN_SECOND_INDUCT ); break;
      default: break;
     }

     return TRUE;
}

bool cedit_clist( CHAR_DATA *ch, char *argument )
{
  return TRUE;
}

bool cedit_desc( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pClan->description );
	return TRUE;
    }

    send_to_char(C_DEFAULT, "Syntax:  desc    - line edit\n\r", ch );
    return FALSE;
}

void do_clandesc ( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;

   if ( (pClan=get_clan_index(ch->clan)) != NULL )
   {
    string_append( ch, &pClan->description );
    save_clans( );
   }
}

bool cedit_recall( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;
    char room[MAX_STRING_LENGTH];
    int  value;

    EDIT_CLAN(ch, pClan);

    one_argument( argument, room );

    if ( !is_number( argument ) || argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  recall [#rvnum]\n\r", ch );
	return FALSE;
    }

    value = atoi( room );

    if ( !get_room_index( value ) )
    {
	send_to_char(C_DEFAULT, "CEdit:  Room vnum does not exist.\n\r", ch );
	return FALSE;
    }

    pClan->recall = value;

    send_to_char(C_DEFAULT, "Recall set.\n\r", ch );
    return TRUE;
}
  
/*
 * Mobile Editor Functions.
 */
bool medit_show( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    char buf[MAX_STRING_LENGTH];

    EDIT_MOB(ch, pMob);

    sprintf( buf, "&cName&w:          &z[&W%s&z]\n\r"
		  "&cArea:          &z[&R%5d&z]&W%s\n\r",
	pMob->player_name,
	!pMob->area ? -1        : pMob->area->vnum,
	!pMob->area ? "No Area" : pMob->area->name );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&cAct&w:&z           [&W%s&z]\n\r",
	flag_string( act_flags, pMob->act ) );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&cVnum&w:&z          [&R%5d&z]\n\r"
		  "&cSex&w:&z           [&W%s&z]\n\r",
	pMob->vnum,
	pMob->sex == SEX_MALE    ? "male"   :
	pMob->sex == SEX_FEMALE  ? "female" : "neutral" );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf,
	"&cLevel&w:&z         [&R%2d&z]\n\r"
	"&cAlign&w:&z         [&R%4d&z]\n\r",
	pMob->level,       pMob->alignment );
    send_to_char(C_DEFAULT, buf, ch );
#ifdef NEW_MONEY
   sprintf( buf,
        "&cHit Points&w:&z    [&R%5d&z]\n\r"
        "&cGold&w:&z          [&R%7d&z]\n\r"
	"&cSilver&w:&z	       [&R%7d&z]\n\r"
	"&cCopper&w:&z	       [&R%7d&z]\n\r",
        pMob->hitnodice, pMob->money.gold, pMob->money.silver, pMob->money.copper );
    send_to_char(C_DEFAULT, buf, ch);
#else
    sprintf( buf,
        "&cHit Points&w:&z    [&R%5d&z]\n\r"
	"&cGold&w:&z          [&R%d&z]\n\r",
        pMob->hitnodice, pMob->gold );
    send_to_char(C_DEFAULT, buf, ch);
#endif
    sprintf( buf, "&cAffected by&w:&z   [&W%s&z]\n\r",
	flag_string( affect_flags, pMob->affected_by ) );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&cAffected by 2&w:&z [&W%s&z]\n\r",
        flag_string( affect2_flags, pMob->affected_by2 ) );
    send_to_char(C_DEFAULT, buf, ch );

    if ( pMob->spec_fun )
    {
	sprintf( buf, "&cSpec fun&w:&z      [&W%s&z]\n\r",
	spec_string(pMob->spec_fun ) );
	send_to_char(C_DEFAULT, buf, ch );
    }

    sprintf( buf, "&cImmunities&w:&z    [&W%s&z]\n\r",
	flag_string( immune_flags, pMob->imm_flags ) );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&cShort descr&w:&z   [&W%s&z]\n\r"
		  "&cLong descr&w:\n\r    &W%s",
	pMob->short_descr,
	pMob->long_descr );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&cDescription&w:\n\r&W%s", pMob->description );
    send_to_char(C_DEFAULT, buf, ch );

    if ( pMob->pShop )
    {
	SHOP_DATA *pShop;
	int iTrade;

	pShop = pMob->pShop;

	sprintf( buf,
	  "&cShop data for&z [&R%5d&z]&w:\n\r"
	  " &cMarkup for purchaser&w:&z [&R%d%%&z]\n\r"
	  " &cMarkdown for seller&w:&z  [&R%d%%&z]\n\r",
	    pShop->keeper, pShop->profit_buy, pShop->profit_sell );
	send_to_char(C_DEFAULT, buf, ch );
	sprintf( buf, "  &cHours&w: &z[&R%d&z] &cto &z[&R%d&z]&c.\n\r",
	    pShop->open_hour, pShop->close_hour );
	send_to_char(C_DEFAULT, buf, ch );

	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	{
	    if ( pShop->buy_type[iTrade] != 0 )
	    {
		if ( iTrade == 0 ) {
		    send_to_char(C_DEFAULT, " &c Number Trades Type\n\r",ch );
		    send_to_char(C_DEFAULT, " &z ------ -----------\n\r",ch );
		}
		sprintf( buf, "    &z[&R%4d&z] [&W%s&z]\n\r", iTrade,
		    flag_string( type_flags, pShop->buy_type[iTrade] ) );
		send_to_char(C_DEFAULT, buf, ch );
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

    if ( argument[0] == '\0' )
    {
      pArea = ch->in_room->area;
      for ( value = pArea->lvnum; pArea->uvnum >= value; value++ )
      {
        if ( !get_mob_index( value ) )
         break;
      }
      if ( value > pArea->uvnum )
      {
	send_to_char( C_DEFAULT, "MEdit:  No free mob vnums in this area.\n\r", ch );
	return FALSE;
      }
    }



    /* OLC 1.1b */
    if ( /*argument[0] == '\0' ||*/ value <= 0 || value >= INT_MAX )
    {
	char output[MAX_STRING_LENGTH];

	sprintf( output, "Syntax:  create [0 < vnum < %d]\n\r", INT_MAX );
	send_to_char(C_DEFAULT, output, ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );

    if ( !pArea )
    {
	send_to_char(C_DEFAULT, "MEdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char(C_DEFAULT, "MEdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_mob_index( value ) )
    {
	send_to_char(C_DEFAULT, "MEdit:  Mobile vnum already exists.\n\r", ch );
	return FALSE;
    }

    pMob			= new_mob_index();
    pMob->vnum			= value;
    pMob->area			= pArea;

/*    if ( value > top_vnum_mob )
	top_vnum_mob = value;        */

/*    pMob->act			= ACT_IS_NPC; */
    iHash			= value % MAX_KEY_HASH;
    pMob->next			= mob_index_hash[iHash];
    mob_index_hash[iHash]	= pMob;
    ch->desc->pEdit		= (void *)pMob;

    send_to_char(C_DEFAULT, "Mobile Created.\n\r", ch );
    return TRUE;
}

bool medit_delet( CHAR_DATA *ch, char *argument )
{
   send_to_char( AT_GREY, "If you want to DELETE this mob, spell it out.\n\r", ch );
   return FALSE;
}

bool medit_delete( CHAR_DATA *ch, char *argument )
{
   MOB_INDEX_DATA   *pMob;
   MOB_INDEX_DATA   *prev;

   EDIT_MOB(ch, pMob);

   /* begin remove mob from index */
   prev = mob_index_hash[pMob->vnum % MAX_KEY_HASH];

   if ( pMob == prev )
   {
      mob_index_hash[pMob->vnum % MAX_KEY_HASH] = pMob->next;
   }
   else
   {
      for ( ; prev; prev = prev->next )
      {
	 if ( prev->next == pMob )
	 {
	    prev->next = pMob->next;
	    break;
	 }
      }

      if ( !prev )
      {
	 bug( "medit_delete: mob %d not found.",
	       pMob->vnum );
      }
   }
   /* end remove mob from index*/

   /* delete mob */
   free_mob_index( pMob );

   ch->desc->pEdit = NULL;
   ch->desc->editor = 0;
   send_to_char(C_DEFAULT, "Deleted.\n\r", ch);
   return TRUE;

}

bool medit_class( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob );

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax: class [mobs class]\n\r", ch );
	return FALSE;
    }

    if ( ( atoi(argument) < 0 ) || ( atoi(argument) > MAX_CLASS ) )
    {
      pMob->class = (atoi(argument));
      return TRUE;
    }

    send_to_char(C_DEFAULT, "Incorrect class number.\n\r", ch );
    return FALSE;
}


bool medit_spec( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  spec [special function]\n\r", ch );
	return FALSE;
    }


    if ( !str_cmp( argument, "none" ) )
    {
        pMob->spec_fun = NULL;

        send_to_char(C_DEFAULT, "Spec removed.\n\r", ch);
        return TRUE;
    }

    if ( spec_lookup( argument ) )
    {
	pMob->spec_fun = spec_lookup( argument );
	send_to_char(C_DEFAULT, "Spec set.\n\r", ch);
	return TRUE;
    }

    send_to_char(C_DEFAULT, "MEdit: No such special function.\n\r", ch );
    return FALSE;
}



bool medit_align( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(C_DEFAULT, "Syntax:  alignment [number]\n\r", ch );
	return FALSE;
    }

    pMob->alignment = atoi( argument );

    send_to_char(C_DEFAULT, "Alignment set.\n\r", ch);
    return TRUE;
}



bool medit_level( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(C_DEFAULT, "Syntax:  level [number]\n\r", ch );
	return FALSE;
    }

    pMob->level = atoi( argument );

    send_to_char(C_DEFAULT, "Level set.\n\r", ch);
    return TRUE;
}

bool medit_gold( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(C_DEFAULT, "Syntax:  gold [amount]\n\r", ch );
	return FALSE;
    }
#ifdef NEW_MONEY
    pMob->money.gold = atoi( argument );
#else
    pMob->gold = atoi( argument );
#endif

    send_to_char(C_DEFAULT, "Gold coins set.\n\r", ch);
    return TRUE;
}

#ifdef NEW_MONEY
bool medit_silver( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        send_to_char(C_DEFAULT, "Syntax:  silver [amount]\n\r", ch );
        return FALSE;
    }

    pMob->money.silver = atoi( argument );

    send_to_char(C_DEFAULT, "Silver coins set.\n\r", ch);
    return TRUE;
}

bool medit_copper( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        send_to_char(C_DEFAULT, "Syntax:  copper [amount]\n\r", ch );
        return FALSE;
    }

    pMob->money.copper = atoi( argument );

    send_to_char(C_DEFAULT, "Copper coins set.\n\r", ch);
    return TRUE;
}
#endif

bool medit_hitpoint( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char(C_DEFAULT, "Syntax:  hp [amount]\n\r", ch );
	return FALSE;
    }

    pMob->hitnodice = atoi( argument );

    send_to_char(C_DEFAULT, "Hit points set.\n\r", ch);
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

    send_to_char(C_DEFAULT, "Syntax:  desc    - line edit\n\r", ch );
    return FALSE;
}




bool medit_long( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  long [string]\n\r", ch );
	return FALSE;
    }

    free_string( pMob->long_descr );
    strcat( argument, "\n\r" );
    pMob->long_descr = str_dup( argument );
    pMob->long_descr[0] = UPPER( pMob->long_descr[0]  );

    send_to_char(C_DEFAULT, "Long description set.\n\r", ch);
    return TRUE;
}



bool medit_short( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  short [string]\n\r", ch );
	return FALSE;
    }

    free_string( pMob->short_descr );
    pMob->short_descr = str_dup( argument );

    send_to_char(C_DEFAULT, "Short description set.\n\r", ch);
    return TRUE;
}



bool medit_name( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  name [string]\n\r", ch );
	return FALSE;
    }

    free_string( pMob->player_name );
    pMob->player_name = str_dup( argument );

    send_to_char(C_DEFAULT, "Name set.\n\r", ch);
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
	send_to_char(C_DEFAULT, "Syntax:  shop hours [#opening] [#closing]\n\r", ch );
	send_to_char(C_DEFAULT, "         shop profit [#buying%] [#selling%]\n\r", ch );
	send_to_char(C_DEFAULT, "         shop type [#0-4] [item type]\n\r", ch );
	send_to_char(C_DEFAULT, "         shop delete [#0-4]\n\r", ch );
	send_to_char(C_DEFAULT, "         shop remove\n\r", ch );
	return FALSE;
    }


    if ( !str_cmp( command, "hours" ) )
    {
	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' || !is_number( argument ) )
	{
	    send_to_char(C_DEFAULT, "Syntax:  shop hours [#opening] [#closing]\n\r", ch );
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

	send_to_char(C_DEFAULT, "Shop hours set.\n\r", ch);
	return TRUE;
    }


    if ( !str_cmp( command, "profit" ) )
    {
	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' || !is_number( argument ) )
	{
	    send_to_char(C_DEFAULT, "Syntax:  shop profit [#buying%] [#selling%]\n\r", ch );
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

	send_to_char(C_DEFAULT, "Shop profit set.\n\r", ch);
	return TRUE;
    }


    if ( !str_cmp( command, "type" ) )
    {
	char buf[MAX_INPUT_LENGTH];
	int value;

	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' )
	{
	    send_to_char(C_DEFAULT, "Syntax:  shop type [#0-4] [item type]\n\r", ch );
	    return FALSE;
	}

	if ( atoi( arg1 ) >= MAX_TRADE )
	{
	    sprintf( buf, "REdit:  May buy %d items max.\n\r", MAX_TRADE );
	    send_to_char(C_DEFAULT, buf, ch );
	    return FALSE;
	}

	if ( ( value = flag_value( type_flags, argument ) ) == NO_FLAG )
	{
	    send_to_char(C_DEFAULT, "REdit:  That type of item is not known.\n\r", ch );
	    return FALSE;
	}

	if ( !pMob->pShop )
	{
	    pMob->pShop         = new_shop();
	    pMob->pShop->keeper = pMob->vnum;
	    shop_last->next     = pMob->pShop;
	}

	pMob->pShop->buy_type[atoi( arg1 )] = value;

	send_to_char(C_DEFAULT, "Shop type set.\n\r", ch);
	return TRUE;
    }


    if ( !str_cmp( command, "delete" ) )
    {
	if ( arg1[0] == '\0' || !is_number( arg1 ) )
	{
	    send_to_char(C_DEFAULT, "Syntax:  shop delete [#0-4]\n\r", ch );
	    return FALSE;
	}

	if ( !pMob->pShop )
	{
	    send_to_char(C_DEFAULT, "REdit:  Non-existant shop.\n\r", ch );
	    return FALSE;
	}

	if ( atoi( arg1 ) >= MAX_TRADE )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "REdit:  May buy %d items max.\n\r", MAX_TRADE );
	    send_to_char(C_DEFAULT, buf, ch);
	    return FALSE;
	}

	pMob->pShop->buy_type[atoi( arg1 )] = 0;
	send_to_char(C_DEFAULT, "Shop type deleted.\n\r", ch );
	return TRUE;
    }

    else if ( !str_cmp( command, "remove" ) )
    {
        SHOP_DATA *pShop;
	SHOP_DATA *pPrev;

	if ( !pMob->pShop )
	{
	    send_to_char(C_DEFAULT, "REdit:  No shop to remove.\n\r", ch );
	    return FALSE;
	}

	for ( pShop = shop_first, pPrev = NULL; pShop; pPrev = pShop,
	      pShop = pShop->next )
	{
	    if ( pShop == pMob->pShop )
	      break;
	}

	if ( pPrev == NULL )
	  shop_first = shop_first->next;
	else
	  pPrev->next = pShop->next;

	free_shop( pShop );
	pMob->pShop = NULL;
	send_to_char( C_DEFAULT, "Shop removed.\n\r", ch );
	return TRUE;
    }

    medit_shop( ch, "" );
    return FALSE;
}

bool medit_immune(CHAR_DATA *ch, char *argument)
{
  MOB_INDEX_DATA *pMob;
  int value;

  EDIT_MOB(ch, pMob);

  if ( ( value = flag_value( immune_flags, argument ) ) != NO_FLAG )
  {
    TOGGLE_BIT(pMob->imm_flags, value);
    send_to_char(C_DEFAULT, "Immune toggled.\n\r", ch);
    return TRUE;
  }
  send_to_char(C_DEFAULT, "Immune not found.\n\r", ch);
  return FALSE;
}


/*
 * MobProg editor functions.
 * -- Altrag
 */
bool medit_mplist( CHAR_DATA *ch, char *argument )
{
  MPROG_DATA *pMProg;
  MOB_INDEX_DATA *pMob;
  char buf[MAX_STRING_LENGTH];
  int value = 0;

  EDIT_MOB(ch, pMob);

  for ( pMProg = pMob->mobprogs; pMProg; pMProg = pMProg->next, value++ )
  {
    sprintf( buf, "[%2d] (%14s)  %s\n\r", value,
	    mprog_type_to_name( pMProg->type ), pMProg->arglist );
    send_to_char(C_DEFAULT, buf, ch );
  }
  return FALSE;
}

bool oedit_oplist( CHAR_DATA *ch, char *argument )
{
  TRAP_DATA *pTrap;
  OBJ_INDEX_DATA *pObj;
  char buf[MAX_STRING_LENGTH];
  int value = 0;

  EDIT_OBJ( ch, pObj );

  for ( pTrap = pObj->traps; pTrap; pTrap = pTrap->next_here, value++ )
  {
    sprintf(buf, "[%2d] (%13s)  %s\n\r", value,
	    flag_string(oprog_types, pTrap->type), pTrap->arglist);
    send_to_char(C_DEFAULT, buf, ch);
  }
  return FALSE;
}

bool redit_rplist( CHAR_DATA *ch, char *argument )
{
  TRAP_DATA *pTrap;
  ROOM_INDEX_DATA *pRoom;
  char buf[MAX_STRING_LENGTH];
  int value = 0;

  EDIT_ROOM(ch, pRoom);

  for ( pTrap = pRoom->traps; pTrap; pTrap = pTrap->next_here, value++ )
  {
    sprintf(buf, "[%2d] (%12s)  %s\n\r", value,
	    flag_string(rprog_types, pTrap->type), pTrap->arglist);
    send_to_char(C_DEFAULT, buf, ch );
  }
  return FALSE;
}

bool redit_eplist( CHAR_DATA *ch, char *argument )
{
  TRAP_DATA *pTrap;
  ROOM_INDEX_DATA *pRoom;
  int dir;
  EXIT_DATA *pExit = NULL;
  char buf[MAX_STRING_LENGTH];
  int value = 0;

  EDIT_ROOM(ch, pRoom);

  for ( dir = 0; dir < 6; dir++ )
    if ( !str_prefix( argument, dir_name[dir] ) &&
	(pExit = pRoom->exit[dir]) )
      break;
  if ( dir == 6 )
  {
    send_to_char(C_DEFAULT, "Exit does not exist in this room.\n\r",ch);
    return FALSE;
  }
  for ( pTrap = pExit->traps; pTrap; pTrap = pTrap->next_here, value++ )
  {
    sprintf(buf, "[%2d] (%11s)  %s\n\r", value,
	    flag_string(eprog_types, pTrap->type), pTrap->arglist);
    send_to_char(C_DEFAULT, buf, ch);
  }
  return FALSE;
}

bool medit_mpremove( CHAR_DATA *ch, char *argument )
{
  MPROG_DATA *pMProg;
  MPROG_DATA *pMPrev;
  MOB_INDEX_DATA *pMob;
  int value = 0;
  int vnum;

  if ( !is_number( argument ) )
  {
    send_to_char( C_DEFAULT, "Syntax:  mpremove #\n\r", ch );
    return FALSE;
  }

  vnum = atoi( argument );

  EDIT_MOB(ch, pMob);


 for ( pMProg = pMob->mobprogs, pMPrev = NULL; value < vnum;
        pMPrev = pMProg, pMProg = pMProg->next, value++ )
  {
    if ( !pMProg )
    {
      send_to_char( C_DEFAULT, "No such MobProg.\n\r", ch );
      return FALSE;
    }
  } 

 /* so we don't crash deleting nonexisting progs */
  if ( !pMProg )
  {
    send_to_char( C_DEFAULT, "No such MobProg.\n\r", ch );
    return FALSE;
  }

  if ( pMPrev == NULL )
    pMob->mobprogs = pMob->mobprogs->next;
  else
    pMPrev->next = pMProg->next;

  free_mprog_data( pMProg );

  num_mob_progs--;
  send_to_char(C_DEFAULT, "Ok.\n\r", ch );
  return TRUE;
}

bool oedit_opremove( CHAR_DATA *ch, char *argument )
{
  TRAP_DATA *pTrap;
  TRAP_DATA *pPrev;
  OBJ_INDEX_DATA *pObj;
  int value = 0;
  int vnum;

  if ( !is_number( argument ) )
  {
    send_to_char(C_DEFAULT, "Syntax:  opremove #\n\r", ch );
    return FALSE;
  }

  vnum = atoi( argument );

  EDIT_OBJ( ch, pObj );

  for ( pTrap = pObj->traps, pPrev = NULL; value < vnum;
        pPrev = pTrap, pTrap = pTrap->next_here, value++ )
  {
    if ( !pTrap )
    {
      send_to_char(C_DEFAULT, "No such ObjProg.\n\r", ch );
      return FALSE;
    }
  }
    /* So we don't crash when deleteing nonexisting progs */
    if ( !pTrap )
    {
      send_to_char(C_DEFAULT, "No such ObjProg.\n\r", ch );
      return FALSE;
    }

  if ( !pPrev )
    pObj->traps = pObj->traps->next_here;
  else
    pPrev->next_here = pTrap->next_here;

  if ( pTrap == trap_list )
    trap_list = pTrap->next;
  else
  {
    for ( pPrev = trap_list; pPrev; pPrev = pPrev->next )
      if ( pPrev->next == pTrap )
	break;
    if ( pPrev )
      pPrev->next = pTrap->next;
  }

  free_trap_data( pTrap );

  num_trap_progs--;
  send_to_char(C_DEFAULT, "Ok.\n\r", ch);
  return TRUE;
}

bool redit_rpremove( CHAR_DATA *ch, char *argument )
{
  TRAP_DATA *pTrap;
  TRAP_DATA *pPrev;
  ROOM_INDEX_DATA *pRoom;
  int value = 0;
  int vnum;

  if ( !is_number( argument ) )
  {
    send_to_char(C_DEFAULT, "Syntax:  rpremove #", ch);
    return FALSE;
  }

  vnum = atoi( argument );

  EDIT_ROOM(ch, pRoom);

  for ( pTrap = pRoom->traps, pPrev = NULL; value < vnum;
	pPrev = pTrap, pTrap = pTrap->next_here, value++ )
  {
    if ( !pTrap )
    {
      send_to_char(C_DEFAULT, "No such RoomProg.\n\r", ch);
      return FALSE;
    }
  }

    /* So we don't crash deleting nonexisting progs */
    if ( !pTrap )
    {
      send_to_char(C_DEFAULT, "No such RoomProg.\n\r", ch);
      return FALSE;
    }

  if ( !pPrev )
    pRoom->traps = pRoom->traps->next_here;
  else
    pPrev->next_here = pTrap->next_here;

  if ( pTrap == trap_list )
    trap_list = pTrap->next;
  else
  {
    for ( pPrev = trap_list; pPrev; pPrev = pPrev->next )
      if ( pPrev->next == pTrap )
	break;
    if ( pPrev )
      pPrev->next = pTrap->next;
  }
  free_trap_data( pTrap );
  num_trap_progs--;
  send_to_char(C_DEFAULT, "Ok.\n\r",ch);
  return TRUE;
}

bool redit_epremove( CHAR_DATA *ch, char *argument )
{
  TRAP_DATA *pTrap;
  TRAP_DATA *pPrev;
  ROOM_INDEX_DATA *pRoom;
  int dir;
  EXIT_DATA *pExit = NULL;
  char arg[MAX_INPUT_LENGTH];
  int value = 0;
  int vnum;

  argument = one_argument(argument, arg);
  if ( arg[0] == '\0' || !is_number( argument ) )
  {
    send_to_char(C_DEFAULT, "Syntax:  epremove <direction> #\n\r",ch);
    return FALSE;
  }

  vnum = atoi(argument);

  EDIT_ROOM(ch, pRoom);

  for ( dir = 0; dir < 6; dir++ )
    if ( !str_prefix(arg, dir_name[dir]) && (pExit = pRoom->exit[dir]) )
      break;
  if ( dir == 6 )
  {
    send_to_char(C_DEFAULT, "Exit does not exist in this room.\n\r", ch);
    return FALSE;
  }

  for ( pTrap = pExit->traps, pPrev = NULL; value < vnum;
        pPrev = pTrap, pTrap = pTrap->next_here, value++ )
  {
    if ( !pPrev )
    {
      send_to_char(C_DEFAULT, "No such ExitProg.\n\r", ch);
      return FALSE;
    }
  }
   /* so we don't crash deleting nonexisting progs */
    if ( !pPrev )
    {
      send_to_char(C_DEFAULT, "No such ExitProg.\n\r", ch);
      return FALSE;
    }

  if ( !pPrev )
    pExit->traps = pExit->traps->next_here;
  else
    pPrev->next_here = pTrap->next_here;

  if ( pTrap == trap_list )
    trap_list = pTrap->next;
  else
  {
    for ( pPrev = trap_list; pPrev; pPrev = pPrev->next )
      if ( pPrev->next == pTrap )
	break;
    if ( pPrev )
      pPrev->next = pTrap->next;
  }
  free_trap_data( pTrap );
  num_trap_progs--;
  send_to_char(C_DEFAULT, "Ok.\n\r", ch);
  return TRUE;
}

bool mpedit_show( CHAR_DATA *ch, char *argument )
{
  MPROG_DATA *pMProg;
  MOB_INDEX_DATA *pMob;
  char buf[MAX_STRING_LENGTH];

  EDIT_MPROG(ch, pMProg);
  pMob = (MOB_INDEX_DATA *)ch->desc->inEdit;

  sprintf(buf, "Mobile: [%5d] %s\n\r", pMob->vnum, pMob->player_name );
  send_to_char( C_DEFAULT, buf, ch );

  sprintf(buf, "MobProg type: %s\n\r", mprog_type_to_name( pMProg->type ) );
  send_to_char(C_DEFAULT, buf, ch );

  sprintf(buf, "Arguments: %s\n\r", pMProg->arglist );
  send_to_char(C_DEFAULT, buf, ch );

  sprintf(buf, "Commands:\n\r%s", pMProg->comlist );
  send_to_char(C_DEFAULT, buf, ch );

  return TRUE;
}

bool tedit_show( CHAR_DATA *ch, char *argument )
{
  TRAP_DATA *pTrap;
  char buf[MAX_STRING_LENGTH];

  EDIT_TRAP( ch, pTrap );

  if ( pTrap->on_obj )
  {
    sprintf(buf, "Object: [%5d] %s\n\r", pTrap->on_obj->vnum,
	    pTrap->on_obj->short_descr);
    send_to_char(C_DEFAULT, buf, ch);
  }

  if ( pTrap->in_room )
  {
    sprintf(buf, "Room: [%5d] %s\n\r", pTrap->in_room->vnum,
	    pTrap->in_room->name);
    send_to_char(C_DEFAULT, buf, ch);
  }

  if ( pTrap->on_exit )
  {
    int dir;
    EXIT_DATA *pExit;
    TRAP_DATA *trap;

    for ( dir = 0; dir < 6; dir++ )
      if ( (pExit = ch->in_room->exit[dir]) )
      {
	for ( trap = pExit->traps; trap; trap = trap->next_here )
	  if ( trap == pTrap )
	    break;
	if ( trap )
	  break;
      }

    sprintf(buf, "Exit: [%5s] %s\n\r", (dir == 6 ? "none" : dir_name[dir]),
	   (dir == 6 ? "Not found in room" : pExit->description));
    send_to_char(C_DEFAULT, buf, ch);
  }

  switch(ch->desc->editor)
  {
  case ED_OPROG:
    sprintf(buf, "ObjProg type: %s\n\r", flag_string(oprog_types,pTrap->type));
    break;
  case ED_RPROG:
    sprintf(buf, "RoomProg type: %s\n\r",flag_string(rprog_types,pTrap->type));
    break;
  case ED_EPROG:
    sprintf(buf, "ExitProg type: %s\n\r",flag_string(eprog_types,pTrap->type));
    break;
  default:
    bug("Tedit_show: Invalid editor %d",ch->desc->editor);
    sprintf(buf, "Unknown TrapProg type\n\r");
    break;
  }
  send_to_char(C_DEFAULT, buf, ch);

/*  sprintf(buf, "Disarmable: %s\n\r", (pTrap->disarmable ? "Yes" : "No"));
  send_to_char(C_DEFAULT, buf, ch);*/

  sprintf(buf, "Arguments: %s\n\r", pTrap->arglist);
  send_to_char(C_DEFAULT, buf, ch);

  sprintf(buf, "Commands:\n\r%s", pTrap->comlist);
  send_to_char(C_DEFAULT, buf, ch);

  return TRUE;
}

bool mpedit_create( CHAR_DATA *ch, char *argument )
{
  MPROG_DATA *pMProg;
  MPROG_DATA *pMLast;
  MOB_INDEX_DATA *pMob;

  EDIT_MOB(ch, pMob);

  pMProg = new_mprog_data( );

  if ( !pMob->mobprogs )
    pMob->mobprogs = pMProg;
  else
  {
    /* No purpose except to find end of list. -- Altrag */
    for ( pMLast = pMob->mobprogs; pMLast->next; pMLast = pMLast->next );
    pMLast->next = pMProg;
  }

  SET_BIT( pMob->progtypes, 1 );

  ch->desc->inEdit = (void *)ch->desc->pEdit;
  ch->desc->pEdit  = (void *)pMProg;
  num_mob_progs++;

  send_to_char(C_DEFAULT, "MobProg created.\n\r", ch );
  return TRUE;
}

bool tedit_create( CHAR_DATA *ch, char *argument )
{
  TRAP_DATA *pTrap;
  TRAP_DATA *pLast;
  TRAP_DATA **pFirst;
  OBJ_INDEX_DATA *pObj = NULL;
  ROOM_INDEX_DATA *pRoom = NULL;
  EXIT_DATA *pExit = NULL;

  switch(ch->desc->editor)
  {
    int dir;
    char arg[MAX_STRING_LENGTH];
  case ED_OPROG:
    pExit = NULL;
    EDIT_OBJ(ch, pObj);
    pFirst = &pObj->traps;
    SET_BIT(pObj->traptypes, 1);
    break;
  case ED_RPROG:
    pExit = NULL;
    EDIT_ROOM(ch, pRoom);
    pFirst = &pRoom->traps;
    SET_BIT(pRoom->traptypes, 1);
    break;
  case ED_EPROG:
    pExit = NULL;
    EDIT_ROOM(ch, pRoom);
    argument = one_argument(argument, arg);
    for ( dir = 0; dir < 6; dir++ )
      if ( !str_prefix(arg, dir_name[dir]) && (pExit = pRoom->exit[dir]) )
	break;
    if ( dir == 6 )
    {
      bug("Tedit_create: No exit",0);
      return FALSE;
    }
    pRoom = NULL;
    pFirst = &pExit->traps;
    SET_BIT(pExit->traptypes, 1);
    break;
  default:
    pExit = NULL;
    bug("Tedit_create: Invalid editor %d", ch->desc->editor);
    return FALSE;
  }

  pTrap = new_trap_data( );
  pTrap->on_obj = pObj;
  pTrap->in_room = pRoom;
  pTrap->on_exit = pExit;

  if ( !trap_list )
    trap_list = pTrap;
  else
  {
    for ( pLast = trap_list; pLast->next; pLast = pLast->next );
    pLast->next = pTrap;
  }


  if ( !*pFirst )
    *pFirst = pTrap;
  else
  {
    for ( pLast = *pFirst; pLast->next_here; pLast = pLast->next_here );
    pLast->next_here = pTrap;
  }

  if ( ch->desc->editor == ED_OPROG )
    ch->desc->inEdit = (void *)ch->desc->pEdit;
  ch->desc->pEdit = (void *)pTrap;

  switch(ch->desc->editor)
  {
  case ED_OPROG:
    send_to_char(C_DEFAULT, "ObjProg created\n\r",ch);
    break;
  case ED_RPROG:
    send_to_char(C_DEFAULT, "RoomProg created\n\r",ch);
    break;
  case ED_EPROG:
    send_to_char(C_DEFAULT, "ExitProg created\n\r",ch);
    break;
  }
  num_trap_progs++;
  return TRUE;
}

bool mpedit_arglist( CHAR_DATA *ch, char *argument )
{
  MPROG_DATA *pMProg;
  int prc = 0;

  EDIT_MPROG(ch, pMProg);

  if ( argument[0] == '\0' )
  {
    send_to_char( C_DEFAULT, "Syntax:  arglist [string]\n\r", ch );
    return FALSE;
  }

  prc = ( is_number( argument ) ? atoi( argument ) : 0 );
  if ( pMProg->type == RAND_PROG && prc > 95 )
  {
    send_to_char( C_DEFAULT, "You can't set the percentage that high on a rand_prog.\n\r", ch );
    return FALSE;
  }
  free_string(pMProg->arglist);
  pMProg->arglist = str_dup(argument);

  send_to_char( C_DEFAULT, "Arglist set.\n\r", ch );
  return TRUE;
}

bool tedit_arglist( CHAR_DATA *ch, char *argument )
{
  TRAP_DATA *pTrap;

  EDIT_TRAP(ch, pTrap);

  if ( argument[0] == '\0' )
  {
    send_to_char(C_DEFAULT, "Syntax:  arglist [string]\n\r",ch);
    return FALSE;
  }
  free_string(pTrap->arglist);
  pTrap->arglist = str_dup(argument);

  send_to_char(C_DEFAULT, "Arglist set.\n\r",ch);
  return TRUE;
}

bool mpedit_comlist( CHAR_DATA *ch, char *argument )
{
  MPROG_DATA *pMProg;

  EDIT_MPROG(ch, pMProg);

  if ( argument[0] == '\0' )
  {
    string_append( ch, &pMProg->comlist );
    return TRUE;
  }

  send_to_char( C_DEFAULT, "Syntax:  comlist    - line edit\n\r", ch );
  return FALSE;
}

bool tedit_comlist( CHAR_DATA *ch, char *argument )
{
  TRAP_DATA *pTrap;

  EDIT_TRAP(ch, pTrap);

  if ( argument[0] == '\0' )
  {
    string_append( ch, &pTrap->comlist );
    return TRUE;
  }

  send_to_char( C_DEFAULT, "Syntax:  comlist    - line edit\n\r", ch );
  return FALSE;
}

bool tedit_disarmable( CHAR_DATA *ch, char *argument )
{
  TRAP_DATA *pTrap;

  EDIT_TRAP(ch, pTrap);

  pTrap->disarmable = !pTrap->disarmable;

  if ( pTrap->disarmable )
    send_to_char(C_DEFAULT, "Trap is now disarmable.\n\r",ch);
  else
    send_to_char(C_DEFAULT, "Trap is no longer disarmable.\n\r",ch);
  return TRUE;
}

/* Decklarean */

bool sedit_show(CHAR_DATA *ch, char *argument)
{
  SOCIAL_DATA *pSocial;
  char buf[MAX_STRING_LENGTH];

  EDIT_SOCIAL(ch, pSocial);

  sprintf( buf, "&YKeyword: &W%s\n\r", pSocial->name );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf, 
           "&Y(1) Char_no_arg:    &B<social>          &wYou see.\n\r&W%s\n\r",
           pSocial->char_no_arg );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf,
           "&Y(2) Others_no_arg:  &B<social>          &wRoom see.\n\r&W%s\n\r",
           pSocial->others_no_arg );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf,
           "&Y(3) Char_found:     &B<social> <victim> &wYou see.\n\r&W%s\n\r",
           pSocial->char_found );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf,
           "&Y(4) Others_found:   &B<social> <victim> &wRoom see.\n\r&W%s\n\r",
	   pSocial->others_found );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf,
           "&Y(5) Victim_found:   &B<social> <victim> &wVictim see.\n\r&W%s\n\r",
           pSocial->vict_found );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf,
           "&Y(6) Char_auto:      &B<social> self     &wYou see.\n\r&W%s\n\r", 
           pSocial->char_auto );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf,
           "&Y(7) Others_auto:    &B<social> self     &wRoom see\n\r&W%s\n\r",
           pSocial->others_auto );
  send_to_char( C_DEFAULT, buf, ch );

  return FALSE;
}

bool sedit_name(CHAR_DATA *ch, char *argument)
{
  SOCIAL_DATA *pSocial;
  EDIT_SOCIAL(ch, pSocial);

  if(argument[0] == '\0')
  {
    send_to_char(C_DEFAULT, "Syntax:  keyword [string]\n\r", ch);
    return FALSE;
  }

  if( get_social(argument) != NULL )
  {
    send_to_char(C_DEFAULT, "Keyword already taken.\n\r", ch);
    return FALSE;
  }

  free_string( pSocial->name);
  pSocial->name = str_dup(argument);

  send_to_char(C_DEFAULT, "Keyword set.\n\r", ch);
  return TRUE;
}

bool sedit_char_no_arg( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA *pSocial;

    EDIT_SOCIAL(ch, pSocial);

    if ( argument[0] == '\0' )
    {
        send_to_char(C_DEFAULT, "Syntax:   char_no_arg [string]\n\r", ch );
        return FALSE;
    }

    free_string( pSocial->char_no_arg );
    if (str_cmp(argument, "clear" ))
     pSocial->char_no_arg = str_dup( argument );
    else
     pSocial->char_no_arg = &str_empty[0];
    send_to_char(C_DEFAULT, "Set.\n\r", ch );
    return TRUE;
}

bool sedit_others_no_arg( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA *pSocial;

    EDIT_SOCIAL(ch, pSocial);

    if ( argument[0] == '\0' )
    {
        send_to_char(C_DEFAULT, "Syntax:   others_no_arg [string]\n\r", ch );
        return FALSE;
    }

    free_string( pSocial->others_no_arg );
    if (str_cmp(argument, "clear" ))
      pSocial->others_no_arg = str_dup( argument );
    else pSocial->others_no_arg = &str_empty[0];
      send_to_char(C_DEFAULT, "Set.\n\r", ch );
    return TRUE;
}

bool sedit_char_found( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA *pSocial;

    EDIT_SOCIAL(ch, pSocial);

    if ( argument[0] == '\0' )
    {
        send_to_char(C_DEFAULT, "Syntax:   char_found [string]\n\r", ch );
        return FALSE;
    }

    free_string( pSocial->char_found );
if (str_cmp(argument, "clear" ))
    pSocial->char_found = str_dup( argument );
else pSocial->char_found = &str_empty[0];
    send_to_char(C_DEFAULT, "Set.\n\r", ch );
    return TRUE;
}

bool sedit_others_found( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA *pSocial;

    EDIT_SOCIAL(ch, pSocial);

    if ( argument[0] == '\0' )
    {
        send_to_char(C_DEFAULT, "Syntax:   others_found [string]\n\r", ch );
        return FALSE;
    }

    free_string( pSocial->others_found );
if (str_cmp(argument, "clear" ))
    pSocial->others_found = str_dup( argument );
else pSocial->others_found = &str_empty[0];
    send_to_char(C_DEFAULT, "Set.\n\r", ch );
    return TRUE;
}

bool sedit_vict_found( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA *pSocial;

    EDIT_SOCIAL(ch, pSocial);

    if ( argument[0] == '\0' )
    {
        send_to_char(C_DEFAULT, "Syntax:   vict_found [string]\n\r", ch );
        return FALSE;
    }

    free_string( pSocial->vict_found );
if (str_cmp(argument, "clear" ))
    pSocial->vict_found = str_dup( argument );
else pSocial->vict_found = &str_empty[0];
    send_to_char(C_DEFAULT, "Set.\n\r", ch );
    return TRUE;
}

bool sedit_char_auto( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA *pSocial;

    EDIT_SOCIAL(ch, pSocial);

    if ( argument[0] == '\0' )
    {
        send_to_char(C_DEFAULT, "Syntax:   char_auto [string]\n\r", ch );
        return FALSE;
    }

    free_string( pSocial->char_auto );
if (str_cmp(argument, "clear" ))
    pSocial->char_auto = str_dup( argument );
else pSocial->char_auto = &str_empty[0];
    send_to_char(C_DEFAULT, "Set.\n\r", ch );
    return TRUE;
}

bool sedit_others_auto( CHAR_DATA *ch, char *argument )
{
    SOCIAL_DATA *pSocial;

    EDIT_SOCIAL(ch, pSocial);

    if ( argument[0] == '\0' )
    {
        send_to_char(C_DEFAULT, "Syntax:   others_auto [string]\n\r", ch );
        return FALSE;
    }

    free_string( pSocial->others_auto );
if (str_cmp(argument, "clear" ))
    pSocial->others_auto = str_dup( argument );
else pSocial->others_auto = &str_empty[0];
    send_to_char(C_DEFAULT, "Set.\n\r", ch );
    return TRUE;
}

bool sedit_delete(CHAR_DATA *ch, char *argument)
{
  SOCIAL_DATA *pSocial;
  SOCIAL_DATA *pMark;

  EDIT_SOCIAL(ch, pSocial);

  if(argument[0] != '\0')
  {
    send_to_char(C_DEFAULT, "Type delete by itself.\n\r", ch);
    return FALSE;
  }

  if(pSocial == social_first)
  {
    social_first = pSocial->next;
    free_social_index( pSocial );

    ch->desc->pEdit = NULL;
    ch->desc->editor = 0;
    send_to_char(C_DEFAULT, "Deleted.\n\r", ch);
    return TRUE;
  }

  for(pMark = social_first;pMark;pMark = pMark->next)
  {
    if(pSocial == pMark->next)
    {
      pMark->next = pSocial->next;
      free_social_index( pSocial );

      ch->desc->pEdit = NULL;
      ch->desc->editor = 0;
      send_to_char(C_DEFAULT, "Deleted.\n\r", ch);
      return TRUE;
    }
  }
  return FALSE;
}

bool rename_show(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA  *pObj;

  RENAME_OBJ( ch, pObj );

  send_to_char( AT_WHITE, "REMAKE AN ITEM: Type done when finished.\n\r\n\r", ch );
  sprintf( buf, "&Y1) &WKeyword(s):        &zSyntax: &R1 <new keyword>\n\r&B[&G%s&B]\n\r", pObj->name );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf, "&Y2) &WShort:             &zSyntax: &R2 <Looks in inv or equiped.>\n\r&B[&G%s&B]\n\r",
                 pObj->short_descr );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf, "&Y3) &WLong:              &zSyntax: &R3 <Looks when on the ground or looked at.>\n\r&B[&G%s&B]\n\r",
	         pObj->description );
  send_to_char( C_DEFAULT, buf, ch );

  return FALSE;
}

bool rename_keyword( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *pObj;

    RENAME_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  keyword [string]\n\r", ch );
	return FALSE;
    }

    free_string( pObj->name );
    pObj->name = str_dup( argument );

    send_to_char(C_DEFAULT, "Keyword set.\n\r", ch);
    return TRUE;
}

bool rename_short( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *pObj;

    RENAME_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  short [string]\n\r", ch );
	return FALSE;
    }

    free_string( pObj->short_descr );
    pObj->short_descr = str_dup( argument );

    send_to_char(C_DEFAULT, "Short description set.\n\r", ch);
    return TRUE;
}

bool rename_long( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *pObj;

    RENAME_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Syntax:  long [string]\n\r", ch );
	return FALSE;
    }
        
    free_string( pObj->description );
    pObj->description = str_dup( argument );
    pObj->description[0] = UPPER( pObj->description[0] );

    send_to_char(C_DEFAULT, "Long description set.\n\r", ch);
    return TRUE;
}


/* XOR */
bool hedit_show(CHAR_DATA *ch, char *argument)
{
  HELP_DATA *pHelp;
  char buf[MAX_STRING_LENGTH];

  EDIT_HELP(ch, pHelp);
  if(pHelp == NULL)
  {
    send_to_char(C_DEFAULT, "bug 1", ch);
    return FALSE;
  }
  sprintf(buf, "Keyword(s):    [%s]\n\r",
   pHelp->keyword ? pHelp->keyword : "none");
  send_to_char(C_DEFAULT, buf, ch);

  sprintf(buf, "Level:         [%d]\n\r", pHelp->level);
  send_to_char(C_DEFAULT, buf, ch);

  sprintf(buf, "Description:\n\r%s\n\r",
   pHelp->text ? pHelp->text : "none.");
  send_to_char(C_DEFAULT, buf, ch);
  return FALSE;
}

bool hedit_desc(CHAR_DATA *ch, char *argument)
{
  HELP_DATA *pHelp;
  EDIT_HELP(ch, pHelp);

  if(argument[0] == '\0')
  {
    string_append( ch, &pHelp->text);
    return TRUE;
  }
  send_to_char(C_DEFAULT, "Syntax:  desc    - line edit\n\r", ch);
  return FALSE;
}

bool hedit_level(CHAR_DATA *ch, char *argument)
{
  HELP_DATA *pHelp;
  EDIT_HELP(ch, pHelp);
  
  if(argument[0] == '\0' || !is_number(argument))
  {
    send_to_char(C_DEFAULT, "Syntax:  level [number]\n\r", ch );
    return FALSE;
  }

  pHelp->level = atoi(argument);

  send_to_char(C_DEFAULT, "Level set.\n\r", ch);
  return TRUE;
}

bool hedit_name(CHAR_DATA *ch, char *argument)
{
  HELP_DATA *pHelp;
  EDIT_HELP(ch, pHelp);

  if(argument[0] == '\0')
  {
    send_to_char(C_DEFAULT, "Syntax:  keyword [string]\n\r", ch);
    return FALSE;
  }

  if( get_help(argument) != NULL )
  {
    send_to_char(C_DEFAULT, "Keyword already taken.\n\r", ch);
    return FALSE;
  }

  free_string( pHelp->keyword);

  pHelp->keyword = str_dup(argument);

  send_to_char(C_DEFAULT, "Keyword(s) set.\n\r", ch);
  return TRUE;
}

bool edit_delet(CHAR_DATA *ch, char *argument)
{
  send_to_char(C_DEFAULT, "If you want to delete, spell it out.\n\r", ch);
  return FALSE;
}

bool hedit_delete(CHAR_DATA *ch, char *argument)
{
  HELP_DATA *pHelp;
  HELP_DATA *pMark;
  EDIT_HELP(ch, pHelp);

  if(argument[0] != '\0')
  {
    send_to_char(C_DEFAULT, "Type delete by itself.\n\r", ch);
    return FALSE;
  }

  if(pHelp == help_first)
  {
     help_first = pHelp->next;
     free_string( pHelp->keyword );
     free_string( pHelp->text );
     free_mem( pHelp, sizeof( *pHelp ) );
     top_help--;
     ch->desc->pEdit = NULL;
     ch->desc->editor = 0;
     send_to_char(C_DEFAULT, "Deleted.\n\r", ch);
     return TRUE;
  }

  for(pMark = help_first;pMark;pMark = pMark->next)
  {
    if(pHelp == pMark->next)
    {
      pMark->next = pHelp->next;
/*      pHelp->next = help_free;
      help_free = pHelp;*/
      free_string( pHelp->keyword );
      free_string( pHelp->text );
      free_mem( pHelp, sizeof( *pHelp ) );
      top_help--;
      ch->desc->pEdit = NULL;
      ch->desc->editor = 0;
      send_to_char(C_DEFAULT, "Deleted.\n\r", ch);
      return TRUE;
    }
  }
  return FALSE;
}

/* END */
bool forge_show(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA  *pObj;
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next = NULL;
  int cnt, max_stat, max_dam, max_hit, max_hp, max_ac, max_mana;
  int max_saves, max_saveb, max_ad, max_bp;
  FORGE_OBJ( ch, pObj );
  max_stat = ( pObj->level > 100 ) ? 3 : 2;
  if ( pObj->item_type == ITEM_WEAPON )
	max_dam = pObj->level / 2.5;
  else
	max_dam = ( IS_SET( pObj->wear_flags, ITEM_WEAR_BODY ) )
		? pObj->level / 3 : pObj->level / 8;
  max_hit = max_dam * 2 / 3;
  max_hp = max_mana = pObj->level;
  max_bp = max_mana / 3;
  max_saves = max_saveb = 0 - UMAX( 1, pObj->level / 7 );
  max_ad = pObj->level * 0.4;
  max_ac = 0 - ( pObj->level * 3 / 4);
  send_to_char( AT_GREY, "Forging an item; type &RDONE &wonly when finished.\n\r", ch );
  send_to_char( AT_DGREY, "Name and descriptions:\n\r", ch );
  sprintf( buf, "Keywords&w:    &z[&W%s&z]\n\r", pObj->name );
  send_to_char( AT_WHITE, buf, ch );
  sprintf( buf, "Short Desc&w:  &z[&W%s&z]\n\r", pObj->short_descr );
  send_to_char( AT_WHITE, buf, ch );
  sprintf( buf, "Long Desc&w:   &z[&W%s&z]\n\r", pObj->description );
  send_to_char( AT_WHITE, buf, ch );
  if ( pObj->item_type == ITEM_WEAPON )
   {
   sprintf( buf, "Weapon Type&w: &z[&W%s&z]\n\r",
  	    flag_string( weapon_flags, pObj->value[3] ) );
   send_to_char( AT_WHITE, buf, ch );
   }
  send_to_char( AT_DGREY, "Availble stats and affects:\n\r", ch );
  sprintf( buf, "[&WStat 1&w:       &R+%d&z] [&WDamroll&w:      &R+%d&z] [&WHitroll&w:       &R+%d&z]\n\r",
	   max_stat, max_dam, max_hit );
  send_to_char( AT_DGREY, buf, ch );
  sprintf( buf, "[&WHit Points&w: &R+%d&z] [&WMana&w:        &R+%d&z] [&WArmor Class&w:  &R%d&z]",
	   max_hp, max_mana, max_ac );
  send_to_char( AT_DGREY, buf, ch );
  sprintf( buf, "\n\r[&WBlood&w:       &R+%d&z] ", max_bp );
  send_to_char( AT_DGREY, buf, ch ); 
  if ( pObj->level >= 40 )
    {
    sprintf( buf,  "[&WSaving-Spell&w: &R%d&z] [&WSaving-Breath&w: &R%d&z]\n\r",
 	     max_saves, max_saveb );
    send_to_char( AT_DGREY, buf, ch );
    }
  if ( pObj->level >= 45 )
    {
    sprintf( buf, "[&WStat 2&w:        &R+%d&z]", max_stat );
    send_to_char( AT_DGREY, buf, ch );
    }
  if ( pObj->level >= 60 )
    {
    sprintf( buf, " [&WAnti-Disarm&w: &R+%d&z]", max_ad );
    send_to_char( AT_DGREY, buf, ch );
    }
  if ( pObj->level >= 101 )
   {
   sprintf( buf, " [&WStat 3&w:        &R+%d&z]", max_stat );
   send_to_char( AT_DGREY, buf, ch );
   }
  send_to_char( AT_GREY, "\n\r", ch );
  send_to_char( AT_DGREY, "Added stats and affects:\n\r", ch );
  send_to_char( AT_WHITE, "#&w- &z[&W  affect  &z] [&Wmodifier&z]\n\r", ch );
  for ( cnt = 1, paf = pObj->affected; cnt <= pObj->level / 10; cnt++, paf = paf_next )
	{
	if ( cnt == 7 )
	  break;
	if ( paf )
	  {
	  paf_next = paf->next;
	  sprintf( buf, "%d&w- &z[&W%10s&z] [&R%8d&z]\n\r", cnt,
		   flag_string( apply_flags, paf->location ),
		   paf->modifier );
	  }
	else
	  sprintf( buf, "%d&w- &z[      &Wnone&z] [       &R0&z]\n\r", cnt );
	send_to_char( AT_WHITE, buf, ch );
	}
#ifdef NEW_MONEY
  sprintf( buf, "Gold Cost&w:   &z[&R%d&z]\n\r"
	        "Silver Cost&w: &z[&R%d&z]\n\r"
		"Copper Cost&w: &z[&R%d&z]\n\r",
          pObj->cost.gold, pObj->cost.silver, pObj->cost.copper );
#else
  sprintf( buf, "Object Cost&w: &z[&R%d&z]\n\r", pObj->cost );
#endif
  send_to_char( AT_WHITE, buf, ch );
  return FALSE;
}
bool forge_addaffect( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *pObj;
    AFFECT_DATA *pAf;
    char loc[MAX_STRING_LENGTH];
    char mod[MAX_STRING_LENGTH];
    char buf[MAX_INPUT_LENGTH];
  int cnt, max_stat, max_dam, max_hit, max_hp, max_ac, max_mana;
  int max_saves, max_saveb, max_ad, stat_cnt, max_statn, max_bp;
  int cost = 0;
  int Mod = 0;
  bool legal = FALSE;
  FORGE_OBJ( ch, pObj );
  max_statn = 1 + ( pObj->level >= 45 ) + ( pObj->level >= 101 );
  max_stat = ( pObj->level > 100 ) ? 3 : 2;
  if ( pObj->item_type == ITEM_WEAPON )
	max_dam = pObj->level / 2.5;
  else
	max_dam = ( IS_SET( pObj->wear_flags, ITEM_WEAR_BODY ) )
		? pObj->level / 3 : pObj->level / 8;
  max_hit = max_dam * 2 / 3;
  max_hp = max_mana = pObj->level;
  max_bp = max_mana / 3;
  max_saves = max_saveb = 0 - UMAX( 1, pObj->level / 7 );
  max_ad = pObj->level * 0.4;
  max_ac = 0 - ( pObj->level * 3 / 4 );

  argument = one_argument( argument, loc );
  one_argument( argument, mod );

  if ( loc[0] == '\0' || mod[0] == '\0' || !is_number( mod ) )
    {
    send_to_char(C_DEFAULT, "Syntax: # [affect] [modifier]\n\r", ch );
    return FALSE;
    }
  if ( !str_prefix( loc, "strength" ) )
    {
    strcpy( loc, "strength" );
    if ( (Mod=atoi( mod )) > max_stat )
	{
	sprintf( buf, "You may not add more than %d to %s.\n\r",
		 max_stat, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = Mod * 5000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "intelligence" ) )
    {
    strcpy( loc, "intelligence" );
    if ( (Mod=atoi( mod )) > max_stat )
	{
	sprintf( buf, "You may not add more than %d to %s.\n\r", 
		 Mod, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = Mod * 5000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "wisdom" ) )
    {
    strcpy( loc, "wisdom" );
    if ( (Mod=atoi( mod )) > max_stat )
	{
	sprintf( buf, "You may not add more than %d to %s.\n\r", 
		 max_stat, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = Mod * 5000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "dexterity" ) )
    {
    strcpy( loc, "dexterity" );
    if ( (Mod=atoi( mod )) > max_stat )
	{
	sprintf( buf, "You may not add more than %d to %s.\n\r", 
		 max_stat, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = Mod * 5000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "constitution" ) )
    {
    strcpy( loc, "constitution" );
    if ( (Mod=atoi( mod )) > max_stat )
	{
	sprintf( buf, "You may not add more than %d to %s.\n\r", 
		 max_stat, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = Mod * 5000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "damroll" ) )
    {
    strcpy( loc, "damroll" );
    if ( (Mod=atoi( mod )) > max_dam )
	{
	sprintf( buf, "You may not add more than %d %s.\n\r", 
		 max_dam, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = Mod * 10000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "hitroll" ) )
    {
    strcpy( loc, "hitroll" );
    if ( (Mod=atoi( mod )) > max_hit )
	{
	sprintf( buf, "You may not add more than %d %s.\n\r", 
		 max_hit, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = Mod * 10000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "hitpoints" ) || !str_cmp( loc, "hp" ) )
    {
    strcpy( loc, "hp" );
    if ( (Mod=atoi( mod )) > max_hp )
	{
	sprintf( buf, "You may not add more than %d %ss.\n\r", 
		 max_hp, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = Mod * 1000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "blood" ) || !str_cmp( loc, "bp" ) )
    {
    strcpy( loc, "blood" );
    if ( (Mod=atoi( mod )) > max_bp )
	{
	sprintf( buf, "You may not add more than %d %ss.\n\r", 
		 max_bp, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = Mod * 3000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "armorclass" ) || !str_cmp( loc, "ac" ) )
    {
    strcpy( loc, "ac" );
    if ( (Mod=atoi( mod )) < max_ac )
	{
	sprintf( buf, "You may not add more than %d %ss.\n\r", 
		 max_ac, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = abs(Mod) * 1000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "mana" ) )
    {
    strcpy( loc, "mana" );
    if ( (Mod=atoi( mod )) > max_mana )
	{
	sprintf( buf, "You may not add more than %d %s.\n\r", 
		 max_mana, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = Mod * 1000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "anti-disarm" ) )
    {
    strcpy( loc, "anti-disarm" );
    if ( (Mod=atoi( mod )) > max_ad )
	{
	sprintf( buf, "You may not add more than %d %s.\n\r", 
		 max_ad, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = Mod * 3000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "saving-spell" ) )
    {
    strcpy( loc, "saving-spell" );
    if ( (Mod=atoi( mod )) < max_saves )
	{
	sprintf( buf, "You may not add more than %d %s.\n\r", 
		 max_saves, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = abs(Mod) * 10000;
    legal = TRUE;
    }
  if ( !str_prefix( loc, "saving-breath" ) )
    {
    strcpy( loc, "saving-breath" );
    if ( (Mod=atoi( mod )) < max_saveb )
	{
	sprintf( buf, "You may not add more than %d %s.\n\r", 
		 max_saveb, loc );
	send_to_char( AT_GREY, buf, ch );
	return FALSE;
	}
    cost = abs(Mod) * 10000;
    legal = TRUE;
    }
    if ( !legal )
	{
	sprintf( buf, "Unknown affect %s, please choose from the list.\n\r", loc );
	send_to_char(AT_GREY, buf, ch );
	return FALSE;
	}
    cnt = 0;
    stat_cnt = 0;
    for ( pAf = pObj->affected; pAf; pAf = pAf->next )
	{
	cnt++;
	if ( cnt >= pObj->level / 10 || cnt >= 6 )
	  {
	  send_to_char( AT_GREY, "You can no longer add anything to this item.\n\r", ch );
	  return FALSE;
	  }
	if ( pAf->location == flag_value( apply_flags, loc ) )
	  {
	  sprintf( buf, "You have already added %d %s to this item.\n\r",
		   pAf->modifier, loc );
	  send_to_char( AT_GREY, buf, ch );
	  return FALSE;
	  }
        if ( pAf->location == APPLY_STR
	|| pAf->location == APPLY_DEX
	|| pAf->location == APPLY_INT
	|| pAf->location == APPLY_WIS
	|| pAf->location == APPLY_CON )
	  stat_cnt++;
        if ( stat_cnt >= max_statn
	&& (   !str_cmp( loc, "strength" )
	    || !str_cmp( loc, "dexterity" )
	    || !str_cmp( loc, "intelligence" )
	    || !str_cmp( loc, "wisdom" )
	    || !str_cmp( loc, "constitution" ) ) )
	  {
	  send_to_char( AT_GREY, "You have already added the maximum number of stats possible for your experience.\n\r", ch );
	  return FALSE;
	  }
    }	
    pAf             =   new_affect();
    pAf->location   =   flag_value( apply_flags, loc );
    pAf->modifier   =   Mod;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;
    sprintf( buf, "Added %d %s for a cost of %d.\n\r", Mod, loc, cost );
    send_to_char(C_DEFAULT, buf, ch);
#ifdef NEW_MONEY
    pObj->cost.gold += cost;
#else
    pObj->cost += cost;
#endif
    return TRUE;
}
bool forge_type( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *pObj;
  char buf[ MAX_INPUT_LENGTH ];
  FORGE_OBJ(ch, pObj);
  if ( pObj->item_type != ITEM_WEAPON )
    {
    send_to_char( AT_GREY, "You are not forging a weapon.\n\r", ch );
    return FALSE;
    }
  if ( argument[0] == '\0' )
    {
    sprintf( buf, "Legal values:\n\r%s %s %s %s %s %s %s\n\r",
	   weapon_flags[0].name, weapon_flags[1].name, weapon_flags[2].name,
	   weapon_flags[3].name, weapon_flags[4].name, weapon_flags[5].name,
	   weapon_flags[6].name );
    send_to_char( AT_GREY, buf, ch );
    sprintf( buf, "%s %s %s %s %s %s %s\n\r",
	   weapon_flags[7].name, weapon_flags[8].name, weapon_flags[9].name,
	   weapon_flags[10].name, weapon_flags[11].name, weapon_flags[12].name,
	   weapon_flags[13].name );
    send_to_char( AT_GREY, buf, ch );
    return FALSE;
    }
    if ( !str_cmp( 
	 flag_string( weapon_flags, flag_value( weapon_flags, argument ) ), 
	 "none" ) )
	{
	forge_type( ch, "" );
	return FALSE;
	}
    else
	{
	pObj->value[3] = flag_value( weapon_flags, argument );
	send_to_char( AT_GREY, "Weapon type set.\n\r", ch );
	return TRUE;
	}
  return FALSE;
}

void do_rpstat( CHAR_DATA *ch, char *argument )
{
    TRAP_DATA *pTrap; 
    ROOM_INDEX_DATA *location;
    char             buf  [ MAX_STRING_LENGTH ];
    char             arg  [ MAX_INPUT_LENGTH  ];


    one_argument( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( !location )
    {
        send_to_char(AT_RED, "No such location.\n\r", ch );
        return;
    }

    if ( !( location->traptypes) )
    {
        send_to_char(AT_WHITE, "That room has no Programs set.\n\r", ch );
        return;
    }

    sprintf( buf, "&WName: [%s&W]\n\rArea: [%s&W]\n\r",
	     location->name, location->area->name );
    send_to_char( AT_WHITE, buf, ch );
    sprintf( buf, "&WVnum: %d  &WSector: %d  &WLight: %d\n\r",
	     location->vnum, location->sector_type, location->light );
    send_to_char( AT_WHITE, buf, ch );

    for ( pTrap = location->traps; pTrap; pTrap = pTrap->next_here )
    {
        sprintf(buf, ">%s %s\n\r%s\n\r\n\r",
            flag_string(rprog_types, pTrap->type), pTrap->arglist, pTrap->comlist );
        send_to_char(C_DEFAULT, buf, ch );
    }
 
   return;

}

void do_opstat( CHAR_DATA *ch, char *argument )
{
    TRAP_DATA   *pTrap;
    OBJ_DATA    *obj;
    char         buf  [ MAX_STRING_LENGTH ];
    char         arg  [ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char(AT_WHITE, "ObjProg stat what?\n\r", ch );
        return;
    }

    if ( !( obj = get_obj_world( ch, arg ) ) )
    {
        send_to_char(AT_WHITE, "Nothing like that in hell, earth, or heaven.\n\r", ch);
        return;
    }

    if ( !( obj->pIndexData->traptypes ) )
    {
 	send_to_char(AT_WHITE, "That object has no Programs set.\n\r", ch );
	return;
    }

    sprintf( buf, "Name: %s.\n\r",
            obj->name );
    send_to_char(AT_RED, buf, ch);

    sprintf( buf, "Vnum: %d.  Type: %s.\n\r",
            obj->pIndexData->vnum, item_type_name( obj ) );
    send_to_char(AT_RED, buf, ch);

    for ( pTrap = obj->pIndexData->traps; pTrap != NULL; pTrap = pTrap->next_here )
    {
      sprintf( buf, ">%s %s\n\r%s\n\r\n\r",
             flag_string(oprog_types, pTrap->type), 
	     pTrap->arglist, pTrap->comlist );
      send_to_char(C_DEFAULT, buf, ch );
    }

    return;
}


bool mreset_show(CHAR_DATA *ch, char *argument)
{
  ROOM_INDEX_DATA *pRoom;
  RESET_DATA *pReset;
  MOB_INDEX_DATA *pMobIndex;
  char buf[MAX_STRING_LENGTH];
  int count = 0;

  EDIT_ROOM(ch, pRoom);

  sprintf( buf, "Mobile Reset: &z[&B%d&z] &W%s\n\r", pRoom->vnum, pRoom->name );
  send_to_char( AT_YELLOW, buf, ch );
  send_to_char( C_DEFAULT, "&z[ &G#&z] [&RMVnum&z] [&YNum&z] &BMob Short Description\n\r", ch );

  for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
  {
    if ( pReset->command == 'M' )
    {
      pMobIndex = get_mob_index( pReset->arg1 );
      sprintf( buf, "&z[&G%2d&z] [&R%5d&z] [&Y%3d&z] &B%s\n\r", 
               count++, pReset->arg1, pReset->arg2,
               pMobIndex ? pMobIndex->short_descr : "&RMob does not exist!!");
      send_to_char( C_DEFAULT, buf, ch );
    }
  }
  return FALSE;  
}

bool spedit_show( CHAR_DATA *ch, char *argument )
{

  SKILL_TYPE *pSpell;
  char buf[MAX_STRING_LENGTH];

  EDIT_SPELL( ch, pSpell );

sprintf( buf, "Spell Name  [%s]\n\r", pSpell->name );
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "Target      [%s]   What the Target is.\n\r","flag_string(pSpell->target)" );
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "Position    [%s]   What Position you have to be in at least.\n\r", "flag_string(pSpell->minimum_position)" );
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "Mana cost   [%3d]\n\r", pSpell->min_mana );
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "Wait        [%3d]   Wait time after casting.\n\r",pSpell->beats);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "1 What seen when spell does damage to target\n\r[%s]\n\r", pSpell->noun_damage);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "2 What you see spell wears off\n\r[%s]\n\r", pSpell->msg_off);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "3 What room see when spell wears off\n\r[%s]\n\r", pSpell->room_msg_off );
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "dispelable [%s]\n\r", pSpell->dispelable ?  "TRUE" :"FALSE" );
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "Min Level to Cast:\n\r");
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "MAG [%3d]\n\r", pSpell->skill_level[0] );
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "CLE [%3d]\n\r", pSpell->skill_level[1]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "THI [%3d]\n\r", pSpell->skill_level[2]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "WAR [%3d]\n\r", pSpell->skill_level[3]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "PSY [%3d]\n\r", pSpell->skill_level[4]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "DRU [%3d]\n\r", pSpell->skill_level[5]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "RAN [%3d]\n\r", pSpell->skill_level[6]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "PAL [%3d]\n\r", pSpell->skill_level[7]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "BAR [%3d]\n\r", pSpell->skill_level[8]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "VAM [%3d]\n\r", pSpell->skill_level[9]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "NEC [%3d]\n\r", pSpell->skill_level[10]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "WWF [%3d]\n\r", pSpell->skill_level[11]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "MNK [%3d]\n\r", pSpell->skill_level[12]);
send_to_char( C_DEFAULT, buf, ch );
sprintf( buf, "TBX [%3d]\n\r", pSpell->skill_level[13]);
send_to_char( C_DEFAULT, buf, ch );
return FALSE;
}

bool spedit_dispelable( CHAR_DATA *ch, char *argument )
{
  SKILL_TYPE *pSpell;

  EDIT_SPELL( ch, pSpell );

  if ( pSpell->dispelable )
    pSpell->dispelable = FALSE;
  else
    pSpell->dispelable = TRUE;

    if (pSpell->dispelable )
	send_to_char(C_DEFAULT, "Spell switched to dispelable.", ch);
    else
	send_to_char(C_DEFAULT, "Spell switched to non dispelable.", ch);


  return TRUE;
}

bool spedit_name( CHAR_DATA *ch, char *argument )
{
  SKILL_TYPE *pSpell;

  if ( argument[0] == '\0' )
  {
    send_to_char(C_DEFAULT, "Syntax:  name [string]\n\r", ch );
    return FALSE;
  }

  EDIT_SPELL( ch, pSpell );

  free_string( pSpell->name );
  pSpell->name = str_dup( argument );

  send_to_char(C_DEFAULT, "Name set.\n\r", ch);
  return TRUE;
}

bool spedit_wait( CHAR_DATA *ch, char *argument )
{
  SKILL_TYPE *pSpell;

  if ( argument[0] == '\0' || !is_number( argument ) )
  {
    send_to_char(AT_WHITE, "Syntax:  &Bwait [number]\n\r", ch );
    return FALSE;
  }

  EDIT_SPELL( ch, pSpell );

  pSpell->beats = atoi( argument );

  send_to_char(AT_WHITE, "Mana cost set.\n\r", ch);
  return TRUE;
}

bool spedit_damage_msg ( CHAR_DATA *ch, char *argument )
{
  SKILL_TYPE *pSpell;

  if ( argument[0] == '\0' )
  {
    send_to_char(C_DEFAULT, "Syntax:  1 [string]\n\r", ch );
    return FALSE;
  }

  EDIT_SPELL( ch, pSpell );

  free_string( pSpell->noun_damage);
  pSpell->noun_damage = str_dup( argument );

  send_to_char(C_DEFAULT, "Spell damage message set.\n\r", ch);
  return TRUE;

}

bool spedit_spell_ends( CHAR_DATA *ch, char *argument )
{
   SKILL_TYPE *pSpell;

  if ( argument[0] == '\0' )
  {
    send_to_char(C_DEFAULT, "Syntax:  2 [string]\n\r", ch );
    return FALSE;
  }

  EDIT_SPELL( ch, pSpell );

  free_string( pSpell->msg_off);
  pSpell->msg_off = str_dup( argument );

  send_to_char(C_DEFAULT, "Spell wear off message set.\n\r", ch);
  return TRUE;

}

bool spedit_spell_ends_room( CHAR_DATA *ch, char *argument )
{
  SKILL_TYPE *pSpell;

  if ( argument[0] == '\0' )
  {
    send_to_char(C_DEFAULT, "Syntax:  3 [string]\n\r", ch );
    return FALSE;
  }

  EDIT_SPELL( ch, pSpell );

  free_string( pSpell->room_msg_off);
  pSpell->room_msg_off = str_dup( argument );

  send_to_char(C_DEFAULT, "Spell wear off message set.\n\r", ch);
  return TRUE;

}

bool spedit_min( CHAR_DATA *ch, char *argument )
{
  SKILL_TYPE *pSpell;
  int iClass;
  int lng;
  char * class = "\0";

  argument = one_argument( argument, class );

  if ( class[0] == '\0' || argument[0] == '\0' || !is_number( argument ) )
  {
    send_to_char(C_DEFAULT, "Syntax:  min [class] [level]\n\r", ch );
    return FALSE;
  }

      if ( strlen( class ) > 3 )
       lng = TRUE;
     else
       lng = FALSE;


  for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
  {
    if (    ( !str_cmp( class, class_table[iClass].who_name) && !lng )
         || ( !str_cmp( class, class_table[iClass].who_long) &&  lng )
       )
    {

      EDIT_SPELL( ch, pSpell );
 
      pSpell->skill_level[iClass] = atoi( argument );
      return TRUE;
    }
  }
  send_to_char( AT_RED, "Invalide class.\n\r", ch );
  return FALSE;

}

bool spedit_mana( CHAR_DATA *ch, char *argument )
{
  SKILL_TYPE *pSpell;

  if ( argument[0] == '\0' || !is_number( argument ) )
  {
    send_to_char(AT_WHITE, "&BSyntax:  mana [number]\n\r", ch );
    return FALSE;
  }

  EDIT_SPELL( ch, pSpell );

  pSpell->min_mana = atoi( argument );

  send_to_char(AT_WHITE, "Mana cost set.\n\r", ch);
  return TRUE;
}

bool nedit_show( CHAR_DATA *ch, char *argument )
{
  NEWBIE_DATA *pNewbie;
  char buf [ MAX_STRING_LENGTH ];

  EDIT_NEWBIE( ch, pNewbie );

  sprintf( buf, "&CKeyword: &W%s\n\r", pNewbie->keyword );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf, "&C(1) &W%s\n\r", pNewbie->answer1 );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf, "&C(2) &W%s\n\r", pNewbie->answer2 );
  send_to_char( C_DEFAULT, buf, ch );

  return FALSE;
}

bool nedit_keyword( CHAR_DATA *ch, char *argument )
{
  NEWBIE_DATA *pNewbie;

  EDIT_NEWBIE( ch, pNewbie );

  if( argument[0] == '\0' )
  {
    send_to_char( C_DEFAULT, "Syntax: keyword <keyword>\n\r", ch );
    return FALSE;
  }

  if( get_newbie_data( argument ) != NULL )
  {
    send_to_char( C_DEFAULT, "Keyword already exists.\n\r", ch );
    return FALSE;
  }

  free_string( pNewbie->keyword );
  pNewbie->keyword = str_dup( argument );
  send_to_char( C_DEFAULT, "Keyword set.\n\r", ch );

  return TRUE;

}

bool nedit_answer1( CHAR_DATA *ch, char *argument )
{
  NEWBIE_DATA *pNewbie;

  EDIT_NEWBIE( ch, pNewbie );

  if( argument[0] == '\0' )
  {
    send_to_char( C_DEFAULT, "Syntax: answer1 [string]\n\r", ch );
    return FALSE;
  }

  free_string( pNewbie->answer1 );
  pNewbie->answer1 = str_dup( argument );
  send_to_char( C_DEFAULT, "First answer set.\n\r", ch );

  return TRUE;

}

bool nedit_answer2( CHAR_DATA *ch, char *argument )
{
  NEWBIE_DATA *pNewbie;

  EDIT_NEWBIE( ch, pNewbie );

  if( argument[0] == '\0' )
  {
    send_to_char( C_DEFAULT, "Syntax: answer2 [string]\n\r", ch );
    return FALSE;
  }

  free_string( pNewbie->answer2 );
  pNewbie->answer2 = str_dup( argument );
  send_to_char( C_DEFAULT, "Second answer set.\n\r", ch );

  return TRUE;

}

/*
 *  Race editor by Decklarean
 */

bool race_edit_show(CHAR_DATA *ch, char *argument)
{
  RACE_DATA *pRace;
  char buf[MAX_STRING_LENGTH];

  EDIT_RACE(ch, pRace);

  sprintf( buf, "&YFull: &z[&W%2d&z][&W&W%-20s&z]\n\r", pRace->vnum, pRace->race_full );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf, "&YName: &z[&W%3s&z]\n\r", pRace->race_name );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf, "&YMStr: &z[&W%2d&z]\n\r", pRace->mstr );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf, "&YMInt: &z[&W%2d&z]\n\r", pRace->mint );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf, "&YMWis: &z[&W%2d&z]\n\r", pRace->mwis );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf, "&YMDex: &z[&W%2d&z]\n\r", pRace->mdex );
  send_to_char( C_DEFAULT, buf, ch );
  sprintf( buf, "&YMCon: &z[&W%2d&z]\n\r", pRace->mcon );
  send_to_char( C_DEFAULT, buf, ch );

  return FALSE;
}

bool race_edit_mstr( CHAR_DATA *ch, char *argument )
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        send_to_char(C_DEFAULT, "Syntax:  mstr [number]\n\r", ch );
	return FALSE;
    }

    pRace->mstr = atoi( argument );

    send_to_char(C_DEFAULT, "MStr set.\n\r", ch);
    return TRUE;
}

bool race_edit_mint( CHAR_DATA *ch, char *argument )
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        send_to_char(C_DEFAULT, "Syntax:  mint [number]\n\r", ch );
	return FALSE;
    }

    pRace->mint = atoi( argument );

    send_to_char(C_DEFAULT, "MInt set.\n\r", ch);
    return TRUE;
}

bool race_edit_mwis( CHAR_DATA *ch, char *argument )
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        send_to_char(C_DEFAULT, "Syntax:  mwis [number]\n\r", ch );
	return FALSE;
    }

    pRace->mwis = atoi( argument );

    send_to_char(C_DEFAULT, "MWis set.\n\r", ch);
    return TRUE;
}

bool race_edit_mdex( CHAR_DATA *ch, char *argument )
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        send_to_char(C_DEFAULT, "Syntax:  mdex [number]\n\r", ch );
	return FALSE;
    }

    pRace->mdex = atoi( argument );

    send_to_char(C_DEFAULT, "MDex set.\n\r", ch);
    return TRUE;
}

bool race_edit_mcon( CHAR_DATA *ch, char *argument )
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        send_to_char(C_DEFAULT, "Syntax:  mcon [number]\n\r", ch );
	return FALSE;
    }

    pRace->mcon = atoi( argument );

    send_to_char(C_DEFAULT, "MCon set.\n\r", ch);
    return TRUE;
}

bool race_edit_name( CHAR_DATA *ch, char *argument )
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if ( argument[0] == '\0' || strlen( argument ) != 3 )
    {
        send_to_char(C_DEFAULT, "Syntax:  name [string] (string has to be 3 characters long)\n\r", ch );
	return FALSE;
    }

    free_string( pRace->race_name );
    pRace->race_name = str_dup( argument );

    send_to_char(C_DEFAULT, "Name set.\n\r", ch);
    return TRUE;
}

bool race_edit_full( CHAR_DATA *ch, char *argument )
{
    RACE_DATA *pRace;

    EDIT_RACE(ch, pRace);

    if ( argument[0] == '\0' || strlen( argument ) > 20 )
    {
        send_to_char(C_DEFAULT, "Syntax:  full [string] (String can't be more than 20 characters long.)\n\r", ch );
	return FALSE;
    }

    free_string( pRace->race_full );
    pRace->race_full = str_dup( argument );

    send_to_char(C_DEFAULT, "Name set.\n\r", ch);
    return TRUE;
}

bool race_edit_delete(CHAR_DATA *ch, char *argument)
{
  RACE_DATA *pRace;
  RACE_DATA *pMark;

  EDIT_RACE(ch, pRace);

  if(argument[0] != '\0')
  {
    send_to_char(C_DEFAULT, "Type delete by itself.\n\r", ch);
    return FALSE;
  }

  if(pRace == first_race)
  { 
    first_race = pRace->next;
    free_race_data( pRace );

    ch->desc->pEdit = NULL;
    ch->desc->editor = 0;
    send_to_char(C_DEFAULT, "Deleted.\n\r", ch);
    return TRUE;
  }

  for(pMark = first_race;pMark;pMark = pMark->next)
  {
    if(pRace == pMark->next)
    {
      pMark->next = pRace->next;
      free_race_data( pRace );

      ch->desc->pEdit = NULL;
      ch->desc->editor = 0;
      send_to_char(C_DEFAULT, "Deleted.\n\r", ch);
      return TRUE;
    }
  }
  return FALSE;
}

