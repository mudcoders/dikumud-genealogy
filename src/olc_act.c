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
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
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
#include "olc.h"



void show_version( CHAR_DATA *ch, char *argument )
{
    send_to_char( VERSION, ch );
    send_to_char( "\n\r",  ch );
    send_to_char( AUTHOR,  ch );
    send_to_char( "\n\r",  ch );
    send_to_char( DATE,    ch );
    send_to_char( "\n\r",  ch );
    send_to_char( CREDITS, ch );
    send_to_char( "\n\r",  ch );
    return;
}

/*
 * Added by Zen. Substitutes the C macros they were making a do_switch bug!
 * Return pointers to what is being edited.
 */
bool is_builder( CHAR_DATA *ch, AREA_DATA *area )
{
    CHAR_DATA *rch;

    rch = get_char( ch );
    if ( ( rch->pcdata->security >= area->security
	  || is_name( rch->name, area->builders )
	  || is_name( "All", area->builders ) ) )
	return TRUE;

    return FALSE;
}

void edit_done( CHAR_DATA *ch, char *argument )
{
    ch->desc->olc_editing = NULL;
    ch->desc->connected   = CON_PLAYING;
    return;
}


bool check_range( int lower, int upper )
{
    AREA_DATA *pArea;
    int        cnt = 0;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	if ( ( lower <= pArea->lvnum && upper >= pArea->lvnum )
	    || ( upper >= pArea->uvnum && lower <= pArea->uvnum ) )
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
	if ( vnum >= pArea->lvnum && vnum <= pArea->uvnum )
	    return pArea;

    return 0;
}



const	struct	olc_help_type	help_table	[ ]	=
{
    {	"area", 	area_flags, 	   "area attributes:   e.g. changed" },
    {	"room", 	room_flags,	   "room attributes:   e.g. safe"    },
    {	"sector", 	sector_flags, 	   "sector types:      e.g. desert"  },
    {	"exit", 	exit_flags, 	   "exit types:        e.g. locked"  },
    {	"type", 	type_flags, 	   "types of objects:  e.g. wand"    },
    {	"extra", 	extra_flags, 	   "obj attributes:    e.g. humming" },
    {	"wear", 	wear_flags, 	   "obj wear location: e.g. wield"   },
    {	"wear-loc", 	wear_loc_flags,    "eq wear location:  e.g. body"    },
    {	"sex", 		sex_flags, 	   "sexes:             e.g. male"    },
    {	"weapon", 	weapon_flags, 	   "weapon type:       e.g. slice"   },
    {	"container", 	container_flags,   "container flags:   e.g. closed"  },
    {	"liquid", 	liquid_flags, 	   "liquids types:     e.g. water"   },
    {	"act", 		act_flags, 	   "ACT_ bits:      e.g. scavenger"  },
    {	"affect", 	affect_flags, 	   "AFF_ bits:      e.g. invisible"  },
    {	"spec-mob", 	spec_mob_table,    "mob spec progs: e.g. spec_fido"  },
    {	"mobprogs", 	mprog_type_flags,  "MobProg types:  e.g. act_prog"   },
    {	"game",		game_table,	   "gambling progs: e.g. game_u_l_t" },
    {	"spells", 	skill_table, 	   "spell names:    e.g. blindness"  },
    {	"", 		0, 		   ""				     }
};


void show_flag_cmds( CHAR_DATA *ch, const struct flag_type *flag_table )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  flag;
    int  col;

    buf1[0] = '\0';
    col = 0;
    for ( flag = 0; *flag_table[flag].name; flag++ )
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

    send_to_char( buf1, ch );
    return;
}


void show_skill_cmds( CHAR_DATA *ch, int tar )
{
    char buf  [MAX_STRING_LENGTH];
    char buf1 [MAX_STRING_LENGTH*2];
    int  sn;
    int  col;

    buf1[0] = '\0';
    col = 0;
    for ( sn = 0; sn < MAX_SKILL; sn++ )
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

    send_to_char( buf1, ch );
    return;
}


void show_spec_mob_cmds( CHAR_DATA *ch )
{
    char buf  [MAX_STRING_LENGTH];
    char buf1 [MAX_STRING_LENGTH];
    int  spec;
    int  col;

    buf1[0] = '\0';
    col = 0;
    send_to_char( "SPEC MOB FUN's (preceed with spec_):\n\r\n\r", ch );
    for ( spec = 0; *spec_mob_table[spec].spec_fun; spec++ )
    {
	sprintf( buf, "%-19.18s", &spec_mob_table[spec].spec_name[5] );
	strcat( buf1, buf );
	if ( ++col % 4 == 0 )
	    strcat( buf1, "\n\r" );
    }

    if ( col % 4 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}


void show_game_cmds( CHAR_DATA *ch )
{
    char buf  [MAX_STRING_LENGTH];
    char buf1 [MAX_STRING_LENGTH];
    int  game;
    int  col;

    buf1[0] = '\0';
    col = 0;
    send_to_char( "GAME FUN's (preceed with game_):\n\r", ch );
    for ( game = 0; *game_table[game].game_fun; game++ )
    {
	sprintf( buf, "%-19.18s", &game_table[game].game_name[5] );
	strcat( buf1, buf );
	if ( ++col % 4 == 0 )
	    strcat( buf1, "\n\r" );
    }

    if ( col % 4 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}


void show_help( CHAR_DATA *ch, char *argument )
{
    char buf   [ MAX_STRING_LENGTH ];
    char arg   [ MAX_INPUT_LENGTH  ];
    char spell [ MAX_INPUT_LENGTH  ];
    int  cnt;

    argument = one_argument( argument, arg );
    one_argument( argument, spell );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax: ? <command>\n\r",    ch );
	send_to_char( "\n\r",                       ch );
	send_to_char( "Command being one of:\n\r",  ch );

	for ( cnt = 0; help_table[cnt].command[0] != '\0'; cnt++ )
	{
	    sprintf( buf, "  %-10.10s    %s\n\r",
		    capitalize( help_table[cnt].command ),
		    help_table[cnt].desc );
	    send_to_char( buf, ch );
	}
	return;
    }

    for ( cnt = 0; *help_table[cnt].command; cnt++ )
    {
	if ( arg[0] == help_table[cnt].command[0]
	    && !str_prefix( arg, help_table[cnt].command ) )
	{
	    if ( help_table[cnt].structure == spec_mob_table )
	    {
		show_spec_mob_cmds( ch );
		return;
	    }
	    else if ( help_table[cnt].structure == game_table )
	    {
		show_game_cmds( ch );
		return;
	    }
	    else if ( help_table[cnt].structure == skill_table )
	    {
		if ( spell[0] == '\0' )
		{
		    send_to_char( "Syntax: ? spells <option>", ch );
		    send_to_char( "\n\r",                      ch );
		    send_to_char( "Option being one of:\n\r",  ch );
		    send_to_char( "  ignore attack defend self object all\n\r",
							       ch );
		    return;
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
		{
		    send_to_char( "Syntax: ? spells <option>", ch );
		    send_to_char( "\n\r",                      ch );
		    send_to_char( "Option being one of:\n\r",  ch );
		    send_to_char( "  ignore attack defend self object all\n\r",
							       ch );
		}

		return;
	    }
	    else
	    {
		show_flag_cmds( ch, help_table[cnt].structure );
		return;
	    }
	}
    }

    return;
}



bool aedit_create( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    pArea		  =   new_area( );
    area_last->next	  =   pArea;
    area_last		  =   pArea;
    ch->desc->olc_editing =   (void *) pArea;

    SET_BIT( pArea->area_flags, AREA_ADDED );
    send_to_char( "Area created.\n\r", ch );
    return TRUE;
}

void aedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char       arg	[ MAX_STRING_LENGTH ];
    char       arg1	[ MAX_STRING_LENGTH ];
    char       arg2	[ MAX_STRING_LENGTH ];
    char       buf 	[ MAX_STRING_LENGTH ];
    int        value;

    pArea = ( AREA_DATA *) ch->desc->olc_editing;
    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );


    if ( !str_prefix( arg1, "show" ) )
    {
        sprintf( buf, "%d", pArea->vnum );
        do_astat( ch, buf );
        return;
    }


    if ( !str_prefix( arg1, "version" ) )
    {
        show_version( ch, "" );
        return;
    }


    if ( !str_prefix( arg1, "done" ) )
    {
	edit_done( ch, "" );
        return;
    }


    if ( !str_prefix( arg1, "?" ) )
    {
        show_help( ch, arg2 );
        return;
    }


    if ( !is_builder( ch, pArea ) )
    {
        send_to_char( "Insufficient security to modify area.\n\r", ch );
	edit_done( ch, "" );
	return;
    }


    if ( ( value = flag_value( area_flags, arg1 ) ) != NO_FLAG )
    {
        TOGGLE_BIT( pArea->area_flags, value );

        send_to_char( "Flag toggled.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "create" ) )
    {
	aedit_create( ch, "" );
	return;
    }


    if ( !str_prefix( arg1, "name" ) )
    {
        if ( arg2[0] == '\0' )
        {
               send_to_char( "Syntax:   name <name>\n\r", ch );
               return;
        }

        free_string( pArea->name );
        pArea->name = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Name set.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "filename" ) )
    {
        if ( argument[0] == '\0' )
        {
            send_to_char( "Syntax:  filename <filename>\n\r", ch );
            return;
        }

        free_string( pArea->filename );
        pArea->filename = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Filename set.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "age" ) )
    {
        if ( !is_number( arg2 ) || arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  age <age>\n\r", ch );
            return;
        }

        pArea->age = atoi( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Age set.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "recall" ) )
    {
        if ( !is_number( arg2 ) || arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  recall <vnum>\n\r", ch );
            return;
        }

        value = atoi( arg2 );

        if ( !get_room_index( value ) && value != 0 )
        {
            send_to_char( "Room vnum does not exist.\n\r", ch );
            return;
        }

        pArea->recall = value;
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Recall set.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "security" ) )
    {
        if ( !is_number( arg2 ) || arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  security <level>\n\r", ch );
            return;
        }

        value = atoi( arg2 );

	if ( value > ch->pcdata->security || value < 0 )
	{
	    if ( ch->pcdata->security != 0 )
	    {
		sprintf( buf, "Security is 0-%d.\n\r", ch->pcdata->security );
		send_to_char( buf, ch );
	    }
	    else
		send_to_char( "Security is 0 only.\n\r", ch );
	    return;
	}

        pArea->security = value;
        SET_BIT( pArea->area_flags, AREA_CHANGED );

        send_to_char( "Security set.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "builder" ) )
    {
        argument = one_argument( argument, arg2 );

        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  builder <name>\n\r", ch );
            return;
        }

        arg2[0] = UPPER( arg2[0] );

        if ( is_name( arg2, pArea->builders ) )
        {
            pArea->builders = string_replace( pArea->builders, arg2, "\0" );
            pArea->builders = string_unpad( pArea->builders );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Builder removed.\n\r", ch );
            return;
        }
        else
        {
            buf[0] = '\0';
            if ( pArea->builders[0] != '\0' )
            {
                strcat( buf, pArea->builders );
                strcat( buf, " " );
            }
            strcat( buf, arg2 );
            free_string( pArea->builders );
            pArea->builders = string_proper( str_dup( buf ) );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Builder added.\n\r", ch );
            return;
        }
    }


    if ( !str_prefix( arg1, "lvnum" ) )
    {
        if ( !is_number( arg2 ) || arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  lvnum <lower>\n\r", ch );
            return;
        }

        value = atoi( arg2 );

        if ( get_vnum_area( value )
          && get_vnum_area( value ) != pArea )
        {
            send_to_char( "Vnum range already assigned.\n\r", ch );
            return;
        }

        pArea->lvnum = value;
        SET_BIT( pArea->area_flags, AREA_CHANGED );

        send_to_char( "Lower vnum set.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "uvnum" ) )
    {
        if ( !is_number( arg2 ) || arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  uvnum <upper>\n\r", ch );
            return;
        }

        value = atoi( arg2 );

        if ( get_vnum_area( value )
          && get_vnum_area( value ) != pArea )
        {
            send_to_char( "Vnum range already assigned.\n\r", ch );
            return;
        }

        pArea->uvnum = value;
        SET_BIT( pArea->area_flags, AREA_CHANGED );

        send_to_char( "Upper vnum set.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "vnum" ) )
    {
       argument = one_argument( argument, arg1 );
       strcpy( arg2, argument );

       if ( !is_number( arg1 ) || arg1[0] == '\0'
         || !is_number( arg2 ) || arg2[0] == '\0' )
       {
            send_to_char( "Syntax:  vnum <lower> <upper>\n\r", ch );
            return;
        }

        value = atoi( arg1 );

        if ( get_vnum_area( value )
          && get_vnum_area( value ) != pArea )
        {
            send_to_char( "Lower vnum already assigned.\n\r", ch );
            return;
        }

        pArea->lvnum = value;
        send_to_char( "Lower vnum set.\n\r", ch );

        value = atoi( arg2 );

        if ( get_vnum_area( value )
          && get_vnum_area( value ) != pArea )
        {
            send_to_char( "Upper vnum already assigned.\n\r", ch );
            return;
        }

        pArea->uvnum = value;
        send_to_char( "Upper vnum set.\n\r", ch );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }

    if ( !str_prefix( arg1, "range" ) )
    {
       argument = one_argument( argument, arg1 );
       strcpy( arg2, argument );

       if ( !is_number( arg1 ) || arg1[0] == '\0'
         || !is_number( arg2 ) || arg2[0] == '\0' )
       {
            send_to_char( "Syntax:  range <lower> <upper>\n\r", ch );
            return;
        }

        value = atoi( arg1 );
        pArea->llv = value;

        send_to_char( "Lower level set.\n\r", ch );

        value = atoi( arg2 );

        pArea->ulv = value;
        send_to_char( "Upper level set.\n\r", ch );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }

    if ( !str_prefix( arg1, "reset" ) )
    {
        if ( argument[0] == '\0' )
        {
            send_to_char( "Syntax:  reset <string>\n\r", ch );
            return;
        }

        free_string( pArea->resetmsg );
        pArea->resetmsg = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Reset message set.\n\r", ch );
        return;
    }

    if ( !str_prefix( arg1, "author" ) )
    {
        if ( argument[0] == '\0' )
        {
            send_to_char( "Syntax:  author <name>\n\r", ch );
            return;
        }

        free_string( pArea->author );
        pArea->author = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Author name set.\n\r", ch );
        return;
    }

    interpret( ch, arg );
    return;
}



bool redit_create( CHAR_DATA *ch, char *argument )
{
    AREA_DATA        *pArea;
    ROOM_INDEX_DATA  *pRoom;
    int               value;
    int               iHash;

    value = atoi( argument );

    if ( argument[0] == '\0'  || value == 0 )
    {
	send_to_char( "Syntax: create <vnum>\n\r", ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );

    if ( !pArea )
    {
	send_to_char( "That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !is_builder( ch, pArea ) )
    {
	send_to_char( "Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_room_index( value ) )
    {
	send_to_char( "Room vnum already exists.\n\r", ch );
	return FALSE;
    }

    pRoom                   = new_room_index( );
    pRoom->area             = pArea;
    pRoom->vnum             = value;

    if ( value > top_vnum_room ) top_vnum_room = value;
    if ( value > pArea->hi_r_vnum ) pArea->hi_r_vnum = value;

    iHash                   = value % MAX_KEY_HASH;
    pRoom->next             = room_index_hash[iHash];
    room_index_hash[iHash]  = pRoom;
    ch->desc->olc_editing   = (void *)pRoom;

    SET_BIT( pArea->area_flags, AREA_CHANGED );
    send_to_char( "Room created.\n\r", ch );
    return TRUE;
}

void redit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA        *pArea;
    ROOM_INDEX_DATA  *pRoom;
    EXIT_DATA        *pExit;
    EXTRA_DESCR_DATA *ed;
    char              arg	[ MAX_STRING_LENGTH ];
    char              arg1	[ MAX_STRING_LENGTH ];
    char              arg2	[ MAX_STRING_LENGTH ];
    char              buf	[ MAX_STRING_LENGTH ];
    int               value;
    int               door;

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    pRoom = ch->in_room;
    pArea = pRoom->area;


    if ( !str_prefix( arg1, "show" ) )
    {
        sprintf( buf, "%d", pRoom->vnum );
	do_rstat( ch, buf );
        return;
    }


    if ( !str_prefix( arg1, "version" ) )
    {
        show_version( ch, "" );
        return;
    }


    if ( !str_prefix( arg1, "done" ) )
    {
	edit_done( ch, "" );
        return;
    }


    if ( !str_prefix( arg1, "?" ) )
    {
        show_help( ch, arg2 );
        return;
    }


    if ( !is_builder( ch, pArea ) )
    {
        send_to_char( "Insufficient security to modify room.\n\r", ch );
	edit_done( ch, "" );
	return;
    }


    if ( ( value = flag_value( room_flags, arg1 ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pRoom->orig_room_flags, value );

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Room flag toggled.\n\r", ch );
	return;
    }


    if ( ( value = flag_value( sector_flags, arg1 ) ) != NO_FLAG )
    {
	pRoom->sector_type = value;

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Sector type set.\n\r", ch );
	return;
    }


    for ( door = 0; door < MAX_DIR; door++ )
    {
	if ( !str_prefix( arg1, dir_name[door] ) )
	    break;
    }

    if ( door != MAX_DIR && arg2[0] != '\0' )
    {
        argument = one_argument( argument, arg1 );
        strcpy( arg2, argument );

        if ( !str_cmp( arg1, "delete" ) )
        {
            if ( !pRoom->exit[door] )
            {
                send_to_char( "Cannot delete a non-existant exit.\n\r", ch );
                return;
            }

	    if ( pRoom->exit[door]->to_room->exit[rev_dir[door]] )
	    {
		free_exit( pRoom->exit[door]->to_room->exit[rev_dir[door]] );
		pRoom->exit[door]->to_room->exit[rev_dir[door]] = NULL;
	    }

            free_exit( pRoom->exit[door] );
            pRoom->exit[door] = NULL;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit unlinked.\n\r", ch );
            return;
        }

        if ( !str_cmp( arg1, "link" ) )
        {
            if ( arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax:  [direction] link <vnum>\n\r", ch );
                return;
            }

            value = atoi( arg2 );

            if ( !get_room_index( value ) )
            {
                send_to_char( "Cannot link to non-existant room.\n\r", ch );
                return;
            }

            if ( !is_builder( ch, get_room_index( value )->area ) )
            {
              send_to_char( "Remote side not created, not a builder there.\n\r",
			   ch );
              return;
            }

            if ( get_room_index( value )->exit[rev_dir[door]] )
            {
                send_to_char( "Remote side's exit already exists.\n\r", ch );
                return;
            }

            if ( !pRoom->exit[door] )
                pRoom->exit[door] = new_exit( );

            pRoom->exit[door]->to_room = get_room_index( value );
            pRoom->exit[door]->vnum = value;

            pRoom                   = get_room_index( value );
            door                    = rev_dir[door];
            pExit                   = new_exit( );
            pExit->to_room          = ch->in_room;
            pExit->vnum             = ch->in_room->vnum;
            pRoom->exit[door]       = pExit;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit linked.\n\r", ch );
            return;
        }
        
        if ( !str_cmp( arg1, "dig" ) )
        {
            if ( arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax: [direction] dig <vnum>\n\r", ch );
                return;
            }
            sprintf( buf, "create %s", arg2 );
            redit( ch, buf );
            sprintf( buf, "%s link %s", dir_name[door], arg2 );
            redit( ch, buf );
            return;
        }

        if ( !str_cmp( arg1, "room" ) )
        {
            if ( arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax:  [direction] room <vnum>\n\r", ch );
                return;
            }

            if ( !pRoom->exit[door] )
                pRoom->exit[door] = new_exit( );

            value = atoi( arg2 );

            if ( !get_room_index( value ) )
            {
                send_to_char( "Cannot link to non-existant room.\n\r", ch );
                return;
            }

            pRoom->exit[door]->to_room = get_room_index( value );
            pRoom->exit[door]->vnum = value;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit set.\n\r", ch );
            return;
        }


        if ( !str_cmp( arg1, "key" ) )
        {
            if ( arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax:  [direction] key <vnum>\n\r", ch );
                return;
            }

            if ( !pRoom->exit[door] )
                pRoom->exit[door] = new_exit( );

            value = atoi( arg2 );

            if ( !get_obj_index( value ) )
            {
                send_to_char( "Cannot use a non-existant object as a key.\n\r",
			     ch );
                return;
            }

            pRoom->exit[door]->key = value;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit key set.\n\r", ch );
            return;
        }


        if ( !str_cmp( arg1, "name" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  [direction] name <string>\n\r", ch );
                return;
            }

            if ( !pRoom->exit[door] )
                pRoom->exit[door] = new_exit( );

            free_string( pRoom->exit[door]->keyword );
            pRoom->exit[door]->keyword = str_dup( arg2 );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit name set.\n\r", ch );
            return;
        }


        if ( !str_cmp( arg1, "desc" ) || !str_cmp( arg1, "description" ) )
        {
            if ( !pRoom->exit[door] )
                pRoom->exit[door] = new_exit( );

            if ( arg2[0] == '\0' )
            {
                string_append( ch, &pRoom->exit[door]->description );
                SET_BIT( pArea->area_flags, AREA_CHANGED );
                return;
            }

            send_to_char( "Syntax:  [direction] desc    - line edit\n\r", ch );
            return;
        }


        if ( ( value = flag_value( exit_flags, arg1 ) ) != NO_FLAG )
        {
	    ROOM_INDEX_DATA *pToRoom;

            if ( !pRoom->exit[door] )
                pRoom->exit[door] = new_exit( );

            TOGGLE_BIT( pRoom->exit[door]->rs_flags, value );
            pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;

	    if ( ( pToRoom = pRoom->exit[door]->to_room )
		&& pToRoom->exit[rev_dir[door]] )
	    {
		TOGGLE_BIT( pToRoom->exit[rev_dir[door]]->rs_flags, value );
		pToRoom->exit[rev_dir[door]]->exit_info =
					pToRoom->exit[rev_dir[door]]->rs_flags;
	    }

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Exit flag toggled.\n\r", ch );
            return;
        }
    }


    if ( !str_prefix( arg1, "ed" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  ed add    <keyword>\n\r", ch );
            send_to_char( "         ed delete <keyword>\n\r", ch );
            send_to_char( "         ed edit   <keyword>\n\r", ch );
            return;
        }

        argument = one_argument( argument, arg1 );
        strcpy( arg2, argument );

        if ( !str_cmp( arg1, "add" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed add <keyword>\n\r", ch );
                return;
            }

            ed                  =   new_extra_descr();
            ed->keyword         =   str_dup( arg2 );
            ed->description     =   str_dup( "" );
            ed->next            =   pRoom->extra_descr;
            pRoom->extra_descr  =   ed;

            string_append( ch, &ed->description );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "edit" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed edit <keyword>\n\r", ch );
                return;
            }

            for ( ed = pRoom->extra_descr; ed; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
            }

            if ( !ed )
            {
                send_to_char( "Extra description keyword not found.\n\r", ch );
                return;
            }

            string_append( ch, &ed->description );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "delete" ) )
        {
            EXTRA_DESCR_DATA *ped;

            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed delete <keyword>\n\r", ch );
                return;
            }

            ped = NULL;

            for ( ed = pRoom->extra_descr; ed; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
                ped = ed;
            }

            if ( !ed )
            {
                send_to_char( "Extra description keyword not found.\n\r", ch );
                return;
            }

            if ( !ped )
                pRoom->extra_descr = ed->next;
            else
                ped->next = ed->next;

            free_extra_descr( ed );

            send_to_char( "Extra description deleted.\n\r", ch );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }
    }


    if ( !str_prefix( arg1, "create" ) )
    {
	redit_create( ch, arg2 );
	return;
    }


    if ( !str_prefix( arg1, "name" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  name <name>\n\r", ch );
            return;
        }

        free_string( pRoom->name );
        pRoom->name = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Name set.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "description" ) )
    {
        if ( arg2[0] == '\0' )
        {
            string_append( ch, &pRoom->description );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }

        send_to_char( "Syntax:  desc    - line edit\n\r", ch );
        return;
    }

    if ( !str_prefix( arg1, "light" ) )
    {
        if ( arg2[0] == '\0' || !is_number( arg2 ) )
        {
            send_to_char( "Syntax:  light <number>\n\r", ch );
            return;
        }

        value = atoi( arg2 );
        pRoom->light = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Light set.\n\r", ch);
        return;
    }

    interpret( ch, arg );
    return;
}



void show_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *obj )
{
    char buf [ MAX_STRING_LENGTH ];

    buf[0] = '\0';

    switch ( obj->item_type )
    {
    default:								break;
    case ITEM_LIGHT:
	if ( obj->value[2] == -1 )
	    sprintf( buf, "v2{c light {xinfinite{c.  {x-1{c,{x\n\r" );
	else
	    sprintf( buf, "v2{c light {x%d{c.{x\n\r", obj->value[2] );
	break;
    case ITEM_WAND:
    case ITEM_STAFF:
	sprintf( buf,
		"v0{c level         {x%d{c.{x\n\r"
		"v1{c charges total {x%d{c.{x\n\r"
		"v2{c charges left  {x%d{c.{x\n\r"
		"v3{c spell         {x%s{c.{x\n\r",
		obj->value[0],
		obj->value[1],
		obj->value[2],
		obj->value[3] != -1
		  ? skill_table[obj->value[3]].name : "none" );
	break;
    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
	sprintf( buf,
		"v0{c level {x%d{c.{x\n\r"
		"v1{c spell {x%s{c.{x\n\r"
		"v2{c spell {x%s{c.{x\n\r"
		"v3{c spell {x%s{c.{x\n\r"
		"v4{c spell {x%s{c.{x\n\r",
		obj->value[0],
		obj->value[1] != -1 ? skill_table[obj->value[1]].name : "none",
		obj->value[2] != -1 ? skill_table[obj->value[2]].name : "none",
		obj->value[3] != -1 ? skill_table[obj->value[3]].name : "none",
		obj->value[4] != -1 ? skill_table[obj->value[4]].name : "none"
		);
	break;
    case ITEM_WEAPON:
	sprintf( buf,
		"v1{c damage minimum {x%d{c.{x\n\r"
		"v2{c damage maximum {x%d{c.{x\n\r"
		"v3{c type           {x%s{c.{x\n\r",
		obj->value[1],
		obj->value[2],
		flag_string( weapon_flags, obj->value[3] ) );
	break;
    case ITEM_CONTAINER:
	sprintf( buf,
		"v0{c weight {x%d{c kg.{x\n\r"
		"v1{c flags  {x%s{c.{x\n\r"
		"v2{c key    {x%s{c.  {x%d{c.{x\n\r",
		obj->value[0],
		flag_string( container_flags, obj->value[1] ),
		get_obj_index( obj->value[2] )
		  ? get_obj_index( obj->value[2] )->short_descr : "none",
		obj->value[2] );
	break;
    case ITEM_DRINK_CON:
	sprintf( buf,
		"v0{c liquid total {x%d{c.{x\n\r"
		"v1{c liquid left  {x%d{c.{x\n\r"
		"v2{c liquid       {x%s{c.{x\n\r"
		"v3{c poisoned     {x%s{c.{x\n\r",
		obj->value[0],
		obj->value[1],
		flag_string( liquid_flags, obj->value[2] ),
		obj->value[3] != 0 ? "yes" : "no" );
	break;
    case ITEM_FOOD:
	sprintf( buf,
		"v0{c food hours {x%d{c.{x\n\r"
		"v3{c poisoned   {x%s{c.{x\n\r",
		obj->value[0],
		obj->value[3] != 0 ? "yes" : "no" );
	break;
    case ITEM_MONEY:
	sprintf( buf, "v0{c gold {x%d{c.{x\n\r", obj->value[0] );
	break;
    case ITEM_PORTAL:
	sprintf( buf,
		"v0{c charges {x%d{c.{x\n\r"
		"v1{c flags   {x%s{c.{x\n\r"
		"v2{c key     {x%s{c.  {x%d{c.{x\n\r"
		"v3{c flags   {x%s{c.{x\n\r"
		"v4{c destiny {x%d{c.{x\n\r",
		obj->value[0],
		flag_string( portal_door_flags, obj->value[1] ),
		get_obj_index( obj->value[2] )
		  ? get_obj_index( obj->value[2] )->short_descr
		  : "none", obj->value[2],
		flag_string( portal_flags, obj->value[3] ),
		obj->value[4] );
	break;
    case ITEM_GEM:
	sprintf( buf,
		"v0{c flags    {x%s{c.{x\n\r"
		"v1{c mana     {x%d{c.{x\n\r"
		"v2{c max_mana {x%d{c.{x\n\r",
		flag_string( mana_flags, obj->value[0] ),
		obj->value[1],
		obj->value[2] );
	break;
    }

    send_to_char( buf, ch );
    return;
}


bool set_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, int value_num,
		    char *argument )
{
    char buf [ MAX_STRING_LENGTH ];
    int value;

    switch ( pObj->item_type )
    {

    default:								  break;

    case ITEM_LIGHT:
	switch ( value_num )
	{
	default:
	    do_help( ch, "ITEM_LIGHT" );
	    return FALSE;
	case 2:
	    send_to_char( "Hours of light set.\n\r\n\r", ch );
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
	    send_to_char( "Spell level set.\n\r\n\r", ch );
	    pObj->value[0] = atoi( argument );
	    break;
	case 1:
	    send_to_char( "Total number of charges set.\n\r\n\r", ch );
	    pObj->value[1] = atoi( argument );
	    break;
	case 2:
	    send_to_char( "Current number of charges set.\n\r\n\r", ch );
	    pObj->value[2] = atoi( argument );
	    break;
	case 3:
	    send_to_char( "Spell type set.\n\r", ch );
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
	    send_to_char( "Spell level set.\n\r\n\r", ch );
	    pObj->value[0] = atoi( argument );
	    break;
	case 1:
	case 2:
	case 3:
	case 4:
	    sprintf( buf, "Spell type %d set.\n\r\n\r", value_num );
	    send_to_char( buf, ch );
	    pObj->value[value_num] = skill_lookup( argument );
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
	    send_to_char( "Minimum damage set.\n\r\n\r", ch );
	    pObj->value[1] = atoi( argument );
	    break;
	case 2:
	    send_to_char( "Maximum damage set.\n\r\n\r", ch );
	    pObj->value[2] = atoi( argument );
	    break;
	case 3:
	    send_to_char( "Weapon type set.\n\r\n\r", ch );
	    pObj->value[3] = flag_value( weapon_flags, argument );
	    break;
	}
	break;

    case ITEM_CONTAINER:
	switch ( value_num )
	{
	default:
	    do_help( ch, "ITEM_CONTAINER" );
	    return FALSE;
	case 0:
	    send_to_char( "Weight capacity set.\n\r\n\r", ch );
	    pObj->value[0] = atoi( argument );
	    break;
	case 1:
	    if ((value = flag_value( container_flags, argument )) != NO_FLAG)
		TOGGLE_BIT( pObj->value[1], value );
	    else
	    {
		do_help( ch, "ITEM_CONTAINER" );
		return FALSE;
	    }
	    send_to_char( "Container type set.\n\r\n\r", ch );
	    break;
	case 2:
	    if ( atoi( argument ) != 0 )
	    {
		if ( !get_obj_index( atoi( argument ) ) )
		{
		    send_to_char( "There is no such item.\n\r\n\r", ch );
		    return FALSE;
		}
		if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY )
		{
		    send_to_char( "That item is not a key.\n\r\n\r", ch );
		    return FALSE;
		}
	    }
	    send_to_char( "Container key set.\n\r\n\r", ch );
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
	    send_to_char( "Maximum amout of liquid hours set.\n\r\n\r", ch );
	    pObj->value[0] = atoi( argument );
	    break;
	case 1:
	    send_to_char( "Current amount of liquid hours set.\n\r\n\r", ch );
	    pObj->value[1] = atoi( argument );
	    break;
	case 2:
	    send_to_char( "Liquid type set.\n\r\n\r", ch );
	    pObj->value[2] = flag_value( liquid_flags, argument );
	    break;
	case 3:
	    send_to_char( "Poison value toggled.\n\r\n\r", ch );
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
	    send_to_char( "Hours of food set.\n\r\n\r", ch );
	    pObj->value[0] = atoi( argument );
	    break;
	case 3:
	    send_to_char( "Poison value toggled.\n\r\n\r", ch );
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
	    send_to_char( "Gold amount set.\n\r\n\r", ch );
	    pObj->value[0] = atoi( argument );
	    break;
	}
	break;

    case ITEM_PORTAL:
	switch ( value_num )
	{
	default:
	    do_help( ch, "ITEM_PORTAL" );
	    return FALSE;
	case 0:
	    send_to_char( "Number of charges set.\n\r\n\r", ch );
	    pObj->value[0] = atoi( argument );
	    break;
	case 1:
	    if ((value = flag_value( portal_door_flags, argument )) != NO_FLAG)
		TOGGLE_BIT( pObj->value[1], value );
	    else
	    {
		do_help( ch, "ITEM_PORTAL" );
		return FALSE;
	    }
	    send_to_char( "Portal door type set.\n\r\n\r", ch );
	    break;
	case 2:
	    if ( atoi( argument ) )
	    {
		if ( !get_obj_index( atoi( argument ) ) )
		{
		    send_to_char( "There is no such item.\n\r\n\r", ch );
		    return FALSE;
		}
		if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY )
		{
		    send_to_char( "That item is not a key.\n\r\n\r", ch );
		    return FALSE;
		}
	     }
	    send_to_char( "Portal key set.\n\r\n\r", ch );
	    pObj->value[2] = atoi( argument );
	    break;
	case 3:
	    if ((value = flag_value( portal_flags, argument )) != NO_FLAG)
		TOGGLE_BIT( pObj->value[3], value );
	    else
	    {
		do_help( ch, "ITEM_PORTAL" );
		return FALSE;
	    }
	    send_to_char( "Portal type set.\n\r\n\r", ch );
	    break;
	case 4:
	    send_to_char( "Portal destiny room vnum set.\n\r\n\r", ch );
	    pObj->value[4] = atoi( argument );
	    break;
	}
	break;

    case ITEM_GEM:
	switch ( value_num )
	{
	default:
	    do_help( ch, "ITEM_GEM" );
	    return FALSE;
	case 0:
	    if ((value = flag_value( mana_flags, argument )) != NO_FLAG)
		TOGGLE_BIT( pObj->value[0], value );
	    else
	    {
		do_help( ch, "ITEM_GEM" );
		return FALSE;
	    }
	    send_to_char( "Gem mana type flag toggled.\n\r\n\r", ch );
	    break;
	case 1:
	    send_to_char( "Mana amount set.\n\r\n\r", ch );
	    pObj->value[1] = atoi( argument );
	    break;
	case 2:
	    send_to_char( "Max mana amount set.\n\r\n\r", ch );
	    pObj->value[2] = atoi( argument );
	    break;
	}
	break;

    }

    show_obj_values( ch, pObj );

    return TRUE;
}


bool set_value( CHAR_DATA * ch, OBJ_INDEX_DATA * pObj, char *argument,
	       int value )
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


bool oedit_create( CHAR_DATA *ch, char *argument )
{
    AREA_DATA        *pArea;
    OBJ_INDEX_DATA   *pObj;
    int               value;
    int               iHash;

    value = atoi( argument );
    if ( argument[0] == '\0' || value == 0 )
    {
	send_to_char( "Syntax: create <vnum>\n\r", ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );

    if ( !pArea )
    {
	send_to_char( "That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !is_builder( ch, pArea ) )
    {
	send_to_char( "Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_obj_index( value ) )
    {
	send_to_char( "Object vnum already exists.\n\r", ch );
	return FALSE;
    }
        
    pObj                    = new_obj_index( );
    pObj->vnum              = value;
    pObj->area              = pArea;
        
    if ( value > top_vnum_obj ) top_vnum_obj = value;
    if ( value > pArea->hi_o_vnum ) pArea->hi_o_vnum = value;

    iHash                   = value % MAX_KEY_HASH;
    pObj->next              = obj_index_hash[iHash];
    obj_index_hash[iHash]   = pObj;
    ch->desc->olc_editing   = (void *) pObj;

    SET_BIT( pArea->area_flags, AREA_CHANGED );
    send_to_char( "Object created.\n\r", ch );
    return TRUE;
}

void oedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA        *pArea;
    OBJ_INDEX_DATA   *pObj;
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA      *pAf;
    AFFECT_DATA      *pAf_next;
    char              arg	[ MAX_STRING_LENGTH ];
    char              arg1	[ MAX_STRING_LENGTH ];
    char              arg2	[ MAX_STRING_LENGTH ];
    char              buf	[ MAX_STRING_LENGTH ];
    int               value;

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    pObj = ( OBJ_INDEX_DATA * ) ch->desc->olc_editing;
    pArea = pObj->area;


    if ( !str_prefix( arg1, "show" ) )
    {
        sprintf( buf, "%d", pObj->vnum );
        do_oindex( ch, buf );
        return;
    }


    if ( !str_prefix( arg1, "version" ) )
    {
        show_version( ch, "" );
        return;
    }


    if ( !str_prefix( arg1, "done" ) )
    {
	edit_done( ch, "" );
        return;
    }


    if ( !str_prefix( arg1, "?" ) )
    {
        show_help( ch, arg2 );
        return;
    }


    if ( !is_builder( ch, pArea ) )
    {
        send_to_char( "Insufficient security to modify object.\n\r", ch );
	edit_done( ch, "" );
	return;
    }


    if ( !str_prefix( arg1, "addaffect" ) )
    {
	char  arg3 [MAX_STRING_LENGTH];

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
        strcpy( arg3, argument );

        if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0'
	    || !is_number( arg2 ) )
        {
	    send_to_char( "Syntax:  addaffect <location> <mod> <bitvector>\n\r", ch );
            return;
        }

	if ( flag_value( apply_flags, arg1 ) == NO_FLAG )
	{
	    send_to_char( "Affect location doesn't exist!\n\r", ch );
	    return;
	}

	if ( flag_value( affect_flags, arg3 ) == NO_FLAG )
	{
	    send_to_char( "Affect bitvector doesn't exist!\n\r", ch );
	    return;
	}

        pAf             =   new_affect( );
        pAf->location   =   flag_value( apply_flags, arg1 );
        pAf->modifier   =   atoi( arg2 );
        pAf->type       =   -1;
        pAf->duration   =   -1;
        pAf->bitvector  =   flag_value( affect_flags, arg3 );
        pAf->next       =   pObj->affected;
        pObj->affected  =   pAf;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Affect added.\n\r", ch);
        return;
    }


    if ( !str_prefix( arg1, "delaffect" ) )
    {
	int cnt = 0;

        strcpy( arg1, argument );

        if ( arg1[0] == '\0' || !is_number( arg1 ) )
        {
	    send_to_char( "Syntax:  delaffect <affect>\n\r", ch );
            return;
        }

	value = atoi( arg1 );

	if ( value < 0 )
	{
	    send_to_char( "Only non-negative affect-numbers allowed.\n\r", ch );
	    return;
	}

	if ( !( pAf = pObj->affected ) )
	{
	    send_to_char( "Non-existant affect.\n\r", ch );
	    return;
	}

	if ( value == 0 )
	{
	    pAf			= pObj->affected;
	    pObj->affected	= pAf->next;
	    free_affect( pAf );
	}
	else
	{
	    while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
		pAf = pAf_next;

	    if ( pAf_next )
	    {
		pAf->next = pAf_next->next;
		free_affect( pAf_next );
	    }
	    else
	    {
		send_to_char( "No such affect.\n\r", ch );
		return;
	    }
	}

        SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Affect removed.\n\r", ch );
        return;
    }


    if ( ( value = flag_value( type_flags, arg1 ) ) != NO_FLAG )
    {
	pObj->item_type = value;

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Type set.\n\r", ch );

	pObj->value[0] = 0;
	pObj->value[1] = 0;
	pObj->value[2] = 0;
	pObj->value[3] = 0;
	pObj->value[4] = 0;

	return;
    }


    if ( ( value = flag_value( extra_flags, arg1 ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pObj->extra_flags, value );

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Extra flag toggled.\n\r", ch );
	return;
    }


    if ( ( value = flag_value( wear_flags, arg1 ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pObj->wear_flags, value );

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Wear flag toggled.\n\r", ch );
	return;
    }


    if ( !str_prefix( arg1, "name" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  name <string>\n\r", ch );
            return;
        }

        free_string( pObj->name );
        pObj->name = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Name set.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "short" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  short <string>\n\r", ch );
            return;
        }

        free_string( pObj->short_descr );
        pObj->short_descr	= str_dup( arg2 );
	pObj->short_descr[0]	= LOWER( pObj->short_descr[0] );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Short description set.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "long" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  long <string>\n\r", ch );
            return;
        }
        
        free_string( pObj->description );
        pObj->description	= str_dup( arg2 );
	pObj->description[0]	= UPPER( pObj->description[0] );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Long description set.\n\r", ch );
        return;
    }


    if ( !str_cmp( arg1, "value0" ) || !str_cmp( arg1, "v0" ) )
    {
	if ( set_value( ch, pObj, argument, 0 ) )
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }


    if ( !str_cmp( arg1, "value1" ) || !str_cmp( arg1, "v1" ) )
    {
	if ( set_value( ch, pObj, argument, 1 ) )
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }


    if ( !str_cmp( arg1, "value2" ) || !str_cmp( arg1, "v2" ) )
    {
	if ( set_value( ch, pObj, argument, 2 ) )
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }


    if ( !str_cmp( arg1, "value3" ) || !str_cmp( arg1, "v3" ) )
    {
	if ( set_value( ch, pObj, argument, 3 ) )
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }


    if ( !str_cmp( arg1, "value4" ) || !str_cmp( arg1, "v4" ) )
    {
	if ( set_value( ch, pObj, argument, 4 ) )
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }


    if ( !str_prefix( arg1, "weight" ) )
    {
        if ( arg2[0] == '\0' || !is_number( arg2 ) )
        {
            send_to_char( "Syntax:  weight <number>\n\r", ch );
            return;
        }

        value = atoi( arg2 );

	if ( value < 0 )
	{
	    send_to_char( "You may not set negative weight.\n\r", ch );
	    return;
	}

        pObj->weight = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Weight set.\n\r", ch);
        return;
    }


    if ( !str_prefix( arg1, "cost" ) )
    {
        if ( arg2[0] == '\0' || !is_number( arg2 ) )
        {
            send_to_char( "Syntax:  cost <number>\n\r", ch );
            return;
        }

        value = atoi( arg2 );

	if ( value < 0 )
	{
	    send_to_char( "You may not set negative cost.\n\r", ch );
	    return;
	}

        pObj->cost = value;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Cost set.\n\r", ch);
        return;
    }


    if ( !str_prefix( arg1, "create" ) )
    {
	oedit_create( ch, arg2 );
	return;
    }


    if ( !str_prefix( arg1, "ed" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  ed add    <keyword>\n\r", ch );
            send_to_char( "         ed delete <keyword>\n\r", ch );
            send_to_char( "         ed edit   <keyword>\n\r", ch );
            return;
        }

        argument = one_argument( argument, arg1 );
        strcpy( arg2, argument );

        if ( !str_cmp( arg1, "add" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed add <keyword>\n\r", ch );
                return;
            }

            ed                  =   new_extra_descr( );
            ed->keyword         =   str_dup( arg2 );
            ed->next            =   pObj->extra_descr;
            pObj->extra_descr   =   ed;

            string_append( ch, &ed->description );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "edit" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed edit <keyword>\n\r", ch );
                return;
            }

            for ( ed = pObj->extra_descr; ed; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
            }

            if ( !ed )
            {
                send_to_char( "Extra description keyword not found.\n\r", ch );
                return;
            }

            string_append( ch, &ed->description );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }


        if ( !str_cmp( arg1, "delete" ) )
        {
            EXTRA_DESCR_DATA *ped = NULL;

            if ( arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  ed delete <keyword>\n\r", ch );
                return;
            }

            for ( ed = pObj->extra_descr; ed; ed = ed->next )
            {
                if ( is_name( arg2, ed->keyword ) )
                    break;
                ped = ed;
            }

            if ( !ed )
            {
                send_to_char( "Extra description keyword not found.\n\r", ch );
                return;
            }

            if ( !ped )
                pObj->extra_descr = ed->next;
            else
                ped->next = ed->next;

            free_extra_descr( ed );

            send_to_char( "Extra description deleted.\n\r", ch );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }
    }


    interpret( ch, arg );
    return;
}



bool medit_create( CHAR_DATA *ch, char *argument )
{
    AREA_DATA      *pArea;
    MOB_INDEX_DATA *pMob;
    int             value;
    int             iHash;

    value = atoi( argument );
    if ( argument[0] == '\0' || value == 0 )
    {
	send_to_char( "Syntax: create <vnum>\n\r", ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );

    if ( !pArea )
    {
	send_to_char( "That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !is_builder( ch, pArea ) )
    {
	send_to_char( "Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_mob_index( value ) )
    {
	send_to_char( "Mobile vnum already exists.\n\r", ch );
	return FALSE;
    }

    pMob                    = new_mob_index( );
    pMob->vnum              = value;
    pMob->area              = pArea;

    if ( value > top_vnum_mob ) top_vnum_mob = value;        
    if ( value > pArea->hi_m_vnum ) pArea->hi_m_vnum = value;

    pMob->act               = ACT_IS_NPC;
    iHash                   = value % MAX_KEY_HASH;
    pMob->next              = mob_index_hash[iHash];
    mob_index_hash[iHash]   = pMob;
    ch->desc->olc_editing		= (void *) pMob;

    SET_BIT( pArea->area_flags, AREA_CHANGED );
    send_to_char( "Mobile created.\n\r", ch );
    return TRUE;
}


void medit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA      *pArea;
    MOB_INDEX_DATA *pMob;
    char 	    arg		[ MAX_STRING_LENGTH ];
    char            arg1	[ MAX_STRING_LENGTH ];
    char            arg2	[ MAX_STRING_LENGTH ];
    char            buf		[ MAX_STRING_LENGTH ];
    int             value;

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );
    value = atoi( arg2 );

    pMob = ( MOB_INDEX_DATA * ) ch->desc->olc_editing;
    pArea = pMob->area;


    if ( !str_prefix( arg1, "show" ) )
    {
        sprintf( buf, "%d %s", pMob->vnum, arg2 );
        do_mindex( ch, buf );
        return;
    }


    if ( !str_prefix( arg1, "version" ) )
    {
        show_version( ch, "" );
        return;
    }


    if ( !str_prefix( arg1, "done" ) )
    {
	edit_done( ch, "" );
        return;
    }


    if ( !str_prefix( arg1, "?" ) )
    {
        show_help( ch, arg2 );
        return;
    }


    if ( !is_builder( ch, pArea ) )
    {
        send_to_char( "Insufficient security to modify mobile.\n\r", ch );
	edit_done( ch, "" );
	return;
    }


    if ( !str_prefix( arg1, "game" ) )
    {
        argument = one_argument( argument, arg1 );
        strcpy( arg2, argument );

        if ( arg1[0] == '\0' )
        {
	    send_to_char( "Syntax:  game fun      <game function>\n\r", ch );
	    send_to_char( "         game bankroll <num>\n\r",           ch );
	    send_to_char( "         game maxwait  <pulse>\n\r",         ch );
	    send_to_char( "         game cheat    <bool>\n\r",          ch );
	    send_to_char( "         game delete\n\r",                   ch );
            return;
        }


	if ( !str_cmp( arg1, "fun" ) )
        {
            argument = one_argument( argument, arg1 );
            strcpy( arg2, argument );

            if ( arg1[0] == '\0' )
            {
		send_to_char( "Syntax:  game fun <game function>\n\r", ch );
                return;
            }

	    if ( !pMob->pGame )
	    {
		pMob->pGame		= new_game( );
		pMob->pGame->croupier	= pMob->vnum;
		if ( game_last )
		    game_last->next	= pMob->pGame;
		else
		    game_last		= pMob->pGame;
	    }

	    if ( game_lookup( arg1 ) )
	    {
		pMob->pGame->game_fun = game_lookup( arg1 );
		SET_BIT( pArea->area_flags, AREA_CHANGED );
		send_to_char( "Game set.\n\r", ch );
		return;
	    }

	    send_to_char( "No such game function.\n\r", ch );
            return;
        }


	if ( !str_cmp( arg1, "bankroll" ) )
        {
            argument = one_argument( argument, arg1 );
            strcpy( arg2, argument );

            if ( arg1[0] == '\0' || !is_number( arg1 ) )
            {
		send_to_char( "Syntax:  game bankroll <num>\n\r", ch );
                return;
            }

	    if ( !pMob->pGame )
	    {
		pMob->pGame		= new_game( );
		pMob->pGame->croupier	= pMob->vnum;
		if ( game_last )
		    game_last->next	= pMob->pGame;
		else
		    game_last		= pMob->pGame;
	    }

	    pMob->pGame->bankroll	= atoi( arg1 );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
	    send_to_char( "Game bankroll set.\n\r", ch );
            return;
        }


	if ( !str_cmp( arg1, "maxwait" ) )
        {
            argument = one_argument( argument, arg1 );
            strcpy( arg2, argument );

            if ( arg1[0] == '\0' || !is_number( arg1 ) )
            {
		send_to_char( "Syntax:  game maxwait <pulse>\n\r", ch );
                return;
            }

	    if ( !pMob->pGame )
	    {
		pMob->pGame		= new_game( );
		pMob->pGame->croupier	= pMob->vnum;
		if ( game_last )
		    game_last->next	= pMob->pGame;
		else
		    game_last		= pMob->pGame;
	    }

	    pMob->pGame->max_wait = atoi( arg1 );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
	    send_to_char( "Game maxwait pulses set.\n\r", ch );
            return;
        }


	if ( !str_cmp( arg1, "cheat" ) )
        {
            argument = one_argument( argument, arg1 );
            strcpy( arg2, argument );

            if ( arg1[0] == '\0' || !is_number( arg1 ) )
            {
		send_to_char( "Syntax:  game cheat <bool>\n\r", ch );
                return;
            }

	    if ( !pMob->pGame )
	    {
		pMob->pGame		= new_game( );
		pMob->pGame->croupier	= pMob->vnum;
		if ( game_last )
		    game_last->next	= pMob->pGame;
		else
		    game_last		= pMob->pGame;
	    }

	    pMob->pGame->cheat = atoi( arg1 );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
	    send_to_char( "Game cheat set.\n\r", ch );
            return;
        }

        if ( !str_cmp( arg1, "delete" ) )
        {
            if ( !pMob->pGame )
            {
                send_to_char( "Non-existant game.\n\r", ch );
                return;
            }

            free_game( pMob->pGame );
            pMob->pGame = NULL;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Game deleted.\n\r", ch);
            return;
        }

	    send_to_char( "Syntax:  game fun      <game function>\n\r", ch );
	    send_to_char( "         game bankroll <num>\n\r",           ch );
	    send_to_char( "         game maxwait  <pulse>\n\r",         ch );
	    send_to_char( "         game cheat    <bool>\n\r",          ch );
	    send_to_char( "         game delete\n\r",                   ch );
        return;
    }


    if ( !str_prefix( arg1, "shop" ) )
    {
        argument = one_argument( argument, arg1 );
        strcpy( arg2, argument );

        if ( arg1[0] == '\0' )
        {
            send_to_char( "Syntax:  shop hours  <opening> <closing>\n\r", ch );
            send_to_char( "         shop profit <buying>  <selling>\n\r", ch );
            send_to_char( "         shop type   <0-4> <item type>\n\r",   ch );
            send_to_char( "         shop delete\n\r",                     ch );
            return;
        }


        if ( !str_cmp( arg1, "hours" ) )
        {
            argument = one_argument( argument, arg1 );
            strcpy( arg2, argument );

            if ( arg1[0] == '\0' || !is_number( arg1 )
              || arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax:  shop hours <opening> <closing>\n\r",
                ch );
                return;
            }

            if ( !pMob->pShop )
            {
                pMob->pShop         = new_shop( );
                pMob->pShop->keeper = pMob->vnum;
                shop_last->next     = pMob->pShop;
            }

            pMob->pShop->open_hour = atoi( arg1 );
            pMob->pShop->close_hour = atoi( arg2 );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Shop hours set.\n\r", ch);
            return;
        }


        if ( !str_cmp( arg1, "profit" ) )
        {
            argument = one_argument( argument, arg1 );
            strcpy( arg2, argument );

            if ( arg1[0] == '\0' || !is_number( arg1 )
              || arg2[0] == '\0' || !is_number( arg2 ) )
            {
                send_to_char( "Syntax:  shop profit <buying> <selling>\n\r",
			     ch );
                return;
            }

            if ( !pMob->pShop )
            {
                pMob->pShop         = new_shop( );
                pMob->pShop->keeper = pMob->vnum;
                shop_last->next     = pMob->pShop;
            }

            pMob->pShop->profit_buy     = atoi( arg1 );
            pMob->pShop->profit_sell    = atoi( arg2 );

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Shop profit set.\n\r", ch);
            return;
        }


        if ( !str_cmp( arg1, "type" ) )
        {
            argument = one_argument( argument, arg1 );
            strcpy( arg2, argument );

            if ( arg1[0] == '\0' || !is_number( arg1 )
              || arg2[0] == '\0' )
            {
                send_to_char( "Syntax:  shop type <0-4> <item type>\n\r", ch );
                return;
            }

            if ( atoi( arg1 ) >= MAX_TRADE )
            {
		sprintf( buf, "May sell %d items max.\n\r", MAX_TRADE );
		send_to_char( buf, ch );
		return;
            }

	    if ( ( value = flag_value( type_flags, argument ) ) == NO_FLAG )
            {
                send_to_char( "That type of item is not known.\n\r", ch );
                return;
            }

            if ( !pMob->pShop )
            {
                pMob->pShop         = new_shop( );
                pMob->pShop->keeper = pMob->vnum;
                shop_last->next     = pMob->pShop;
            }

            pMob->pShop->buy_type[atoi( arg1 )] = value;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Shop type set.\n\r", ch);
            return;
        }


        if ( !str_cmp( arg1, "delete" ) )
        {
            if ( !pMob->pShop )
            {
                send_to_char( "Cannot delete a shop that is non-existant.\n\r",
			     ch );
                return;
            }

            free_shop( pMob->pShop );
            pMob->pShop = NULL;

            SET_BIT( pArea->area_flags, AREA_CHANGED );
            send_to_char( "Shop deleted.\n\r", ch);
            return;
        }

	send_to_char( "Syntax:  shop hours  <opening> <closing>\n\r", ch );
	send_to_char( "         shop profit <buying>  <selling>\n\r", ch );
	send_to_char( "         shop type   <0-4> <item type>\n\r",   ch );
	send_to_char( "         shop delete\n\r",                     ch );
        return;
    }


    if ( !str_prefix( arg1, "name" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  name <string>\n\r", ch );
            return;
        }

        free_string( pMob->player_name );
        pMob->player_name = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Name set.\n\r", ch);
        return;
    }


    if ( !str_prefix( arg1, "short" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  short <string>\n\r", ch );
            return;
        }

        free_string( pMob->short_descr );
        pMob->short_descr = str_dup( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Short description set.\n\r", ch);
        return;
    }


    if ( !str_prefix( arg1, "long" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  long <string>\n\r", ch );
            return;
        }

        free_string( pMob->long_descr );
        strcat( arg2, "\n\r" );
        pMob->long_descr = str_dup( arg2 );
	pMob->long_descr[0] = UPPER( pMob->long_descr[0] );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Long description set.\n\r", ch);
        return;
    }


    if ( !str_prefix( arg1, "description" ) )
    {
        if ( arg2[0] == '\0' )
        {
            string_append( ch, &pMob->description );
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }

        send_to_char( "Syntax:  desc    - line edit\n\r", ch );
        return;
    }


    if ( ( value = flag_value( sex_flags, arg ) ) != NO_FLAG )
    {
	pMob->sex = value;

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Sex set.\n\r", ch );
	return;
    }

    if ( ( value = race_full_lookup( arg ) ) != NO_FLAG )
    {
	pMob->race = value;

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Race set.\n\r", ch );
	return;
    }

    if ( ( value = flag_value( act_flags, arg ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pMob->act, value );

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Act flag toggled.\n\r", ch );
	return;
    }


    if ( ( value = flag_value( affect_flags, arg ) ) != NO_FLAG )
    {
	TOGGLE_BIT( pMob->affected_by, value );

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Affect flag toggled.\n\r", ch );
	return;
    }


    if ( !str_prefix( arg1, "level" ) )
    {
        if ( arg2[0] == '\0' || !is_number( arg2 ) )
        {
            send_to_char( "Syntax:  level <number>\n\r", ch );
            return;
        }

        pMob->level = atoi( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Level set.\n\r", ch);
        return;
    }

    if ( !str_prefix( arg1, "alignment" ) )
    {
        if ( arg2[0] == '\0' || !is_number( arg2 ) )
        {
            send_to_char( "Syntax:  alignment <number>\n\r", ch );
            return;
        }

        pMob->alignment = atoi( arg2 );

        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Alignment set.\n\r", ch);
        return;
    }

    if ( !str_prefix( arg1, "spec" ) )
    {
        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  spec <special function>\n\r", ch );
            return;
        }

	if ( spec_mob_lookup( arg2 ) )
	{
	    pMob->spec_fun = spec_mob_lookup( argument );
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
	    send_to_char( "Spec set.\n\r", ch);
	    return;
	}

	if ( !str_cmp( arg2, "none" ) )
	{
	    pMob->spec_fun = NULL;

	    SET_BIT( pArea->area_flags, AREA_CHANGED );
	    send_to_char( "Spec removed.\n\r", ch);
	    return;
	}
    }


    if ( !str_prefix( arg1, "create" ) )
    {
	medit_create( ch, arg2 );
	return;
    }

    interpret( ch, arg );
    return;
}



bool mpedit_create( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA      *pArea;
    MPROG_DATA     *mprg;
    int             value;

    value = atoi( argument );
    if ( argument[0] == '\0' || value == 0 )
    {
	send_to_char( "Syntax: create vnum <script vnum>\n\r", ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );

    if ( !pArea )
    {
	send_to_char( "That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !is_builder( ch, pArea ) )
    {
	send_to_char( "Mob vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( !( pMob = get_mob_index( value ) ) )
    {
	send_to_char( "Mobile vnum does not exist!\n\r", ch );
	return FALSE;
    }

    if ( pMob->mobprogs )
    {
	send_to_char( "Mobile already has mob programs!\n\r", ch );
	return FALSE;
    }

    pMob->mobprogs	= ( MPROG_DATA * ) alloc_perm( sizeof( MPROG_DATA ) );
    mprg		= pMob->mobprogs;

    mprg->type			= 0;
    mprg->comlist		= str_dup( "break\n" );
    mprg->arglist		= str_dup( "0" );

    ch->desc->olc_editing		= (void *) pMob;
    ch->pcdata->mprog_edit	= 0;

    send_to_char( "MOBProg created.\n\r", ch );
    SET_BIT( pArea->area_flags, AREA_ADDED );
    return TRUE;
}


int mprog_count( MOB_INDEX_DATA * pMob )
{
    MPROG_DATA *mprg;
    int         count;

    for ( count = 0, mprg = pMob->mobprogs; mprg; mprg = mprg->next, count++ )
	;
    return count;
}

MPROG_DATA *edit_mprog( CHAR_DATA * ch, MOB_INDEX_DATA * pMob )
{
    MPROG_DATA *mprg;
    int         mprog_num;
    int         count = 0;

    mprog_num = ch->pcdata->mprog_edit;
    for ( mprg = pMob->mobprogs; mprg && count < mprog_num; mprg = mprg->next )
	count++;

    return mprg;
}

void show_mprog( CHAR_DATA * ch, MPROG_DATA * pMobProg )
{
    char buf [ MAX_STRING_LENGTH ];

    sprintf( buf, ">%s %s~\n\r",
	    mprog_type_to_name( pMobProg->type ),
	    pMobProg->arglist ? pMobProg->arglist : "NULL" );
    send_to_char( buf, ch );

    sprintf( buf, "%s~\n\r", pMobProg->comlist ? pMobProg->comlist
	    : "NULL\n\r" );
    send_to_char( buf, ch );
}

void delete_mprog( CHAR_DATA * ch, int pnum )
{
    MPROG_DATA     *mprg;
    MPROG_DATA     *mprg_prev;
    MOB_INDEX_DATA *pMob;
    char            buf	[ MAX_INPUT_LENGTH ];
    int             count;

    pMob = ( MOB_INDEX_DATA * ) ch->desc->olc_editing;

    if ( pnum < 0 )
	return;

    if ( pnum == 0 )
    {
	free_string( pMob->mobprogs->arglist );
	free_string( pMob->mobprogs->comlist );
	mprg = pMob->mobprogs->next;
	/*
	 * Here is where we would recycle the memory of pMob->mobprogs...
	 * no such mechanism yet so this actually IS a sort of memory leak
	 * since memory allocated with alloc_perm cannot be freed.  Walker 
	 */
	pMob->mobprogs	= mprg;
    }
    else
    {
	mprg_prev	= pMob->mobprogs;
	mprg		= mprg_prev->next;

	for ( count = 1; mprg && count < pnum; count++ )
	{
	    mprg_prev	= mprg;
	    mprg	= mprg->next;
	}
	if ( mprg )
	{
	    free_string( mprg->arglist );
	    free_string( mprg->comlist );
	    mprg_prev->next = mprg->next;
	    /*
	     * Here is where we would recycle the memory of mprg...
	     * no such mechanism yet so this actually IS a sort of memory leak
	     * since memory allocated with alloc_perm cannot be freed.  Walker
	     */
	}
    }

    sprintf( buf, "MOBProg %d Deleted.\n\r", pnum + 1 );
    send_to_char( buf, ch );
    return;
}


void mpedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA      *pArea;
    MOB_INDEX_DATA *pMob;
    MPROG_DATA     *pMobProg;
    char 	    arg		[ MAX_STRING_LENGTH ];
    char            arg1	[ MAX_STRING_LENGTH ];
    char            arg2	[ MAX_STRING_LENGTH ];
    char            buf		[ MAX_STRING_LENGTH ];
    int             value;
    int             count;

    if ( IS_NPC( ch ) )
    {
	edit_done( ch, "" );
	return;
    }

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    pMob	= ( MOB_INDEX_DATA * ) ch->desc->olc_editing;
    pMobProg	= edit_mprog( ch, pMob );
    count	= mprog_count( pMob );
    pArea	= pMob->area;

    if ( !str_prefix( arg1, "show" ) )
    {
        sprintf( buf, "%s", arg2 );
        mpedit_show( ch, buf );
        return;
    }


    if ( !str_prefix( arg1, "version" ) )
    {
        show_version( ch, "" );
        return;
    }


    if ( !str_prefix( arg1, "done" ) )
    {
	edit_done( ch, "" );
        return;
    }


    if ( !str_prefix( arg1, "?" ) )
    {
        show_help( ch, arg2 );
        return;
    }


    if ( !is_builder( ch, pArea ) )
    {
        send_to_char( "Insufficient security to modify mobprog.\n\r", ch );
	edit_done( ch, "" );
	return;
    }


    if ( !str_prefix( arg1, "add" ) )
    {
	MPROG_DATA *mprg;
	MPROG_DATA *mprg2;

	if ( !pMob->mobprogs )
	    send_to_char( "Mobile doesn't have mobprogs. Use create.\n\r", ch );

	for ( mprg = mprg2 = pMob->mobprogs; mprg;
					mprg2 = mprg, mprg = mprg->next );

	mprg2->next	= ( MPROG_DATA * ) alloc_perm( sizeof( MPROG_DATA ) );
	mprg		= mprg2->next;
	mprg->type	= 0;
	mprg->arglist	= str_dup( "0" );
	mprg->comlist	= str_dup( "break\n" );
	count++;

	ch->pcdata->mprog_edit = count - 1;

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	sprintf( buf, "MOBProg %d Added.\n\r", count );
	send_to_char( buf, ch );
        return;
    }


    if ( !str_prefix( arg1, "delete" ) )
    {
        if ( arg2[0] == '\0' )
	    delete_mprog( ch, ch->pcdata->mprog_edit );
        else if ( is_number( arg2 ) )
        {
	    if ( ( value = atoi( arg2 ) ) > count )
	    {
		send_to_char( "Mobile does not have that many programs.\n\r", ch
		);
		return;
	    }

	    delete_mprog( ch, value - 1 );
        }
	else if ( !str_cmp( arg2, "all" ) )
	{
	    for ( value = count - 1; value >= 0; value-- )
		delete_mprog( ch, value );
	}
	else
	{
	    send_to_char( "Syntax:  delete <pnum>|all\n\r", ch );
	    return;
	}

	count = mprog_count( pMob );
	if ( ch->pcdata->mprog_edit >= count )
	{
	    ch->pcdata->mprog_edit = count - 1;
	    if ( count == 0 )
		edit_done( ch, "" );
	}

        SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Ok.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "copy" ) )
    {
	MOB_INDEX_DATA *cMob;
	MPROG_DATA     *mprg;
	MPROG_DATA     *cprg;
	MPROG_DATA     *mprg_next;

        if ( arg2[0] == '\0' || !is_number( arg2 ) )
        {
	    send_to_char( "Syntax:  copy [vnum]\n\r", ch );
            return;
        }

	value = atoi( arg2 );
	if ( !( cMob = get_mob_index( value ) ) )
	{
	    send_to_char( "No such mobile exists.\n\r", ch );
	    return;
	}

	if ( cMob == pMob )
	{
	    send_to_char( "You can't copy from yourself, sorry.\n\r", ch );
	    return;
	}

	if ( !cMob->mobprogs )
	{
	    send_to_char( "That mobile doesn't have mobprogs!\n\r", ch );
	    return;
	}

	for ( mprg = pMob->mobprogs; mprg; mprg = mprg_next )
	{
	    mprg_next = mprg->next;
	    free_string( mprg->arglist );
	    free_string( mprg->comlist );
	    free_mem( mprg, sizeof( MPROG_DATA ) );
	}

	mprg = pMob->mobprogs = (MPROG_DATA *) alloc_perm( sizeof(MPROG_DATA) );

	for ( cprg = cMob->mobprogs; cprg;
	     cprg = cprg->next, mprg = mprg->next )
	{
	    mprg->type = cprg->type;
	    SET_BIT( pMob->progtypes, cprg->type );
	    mprg->arglist = str_dup( cprg->arglist );
	    mprg->comlist = str_dup( cprg->comlist );
	    if ( cprg->next )
		mprg->next = (MPROG_DATA *) alloc_perm( sizeof( MPROG_DATA ) );
	    else
		mprg->next = NULL;
	}

	ch->pcdata->mprog_edit = mprog_count( pMob ) - 1;

        SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "MOBProg copied.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg1, "trigger" ) )
    {
        if ( arg2[0] == '\0' )
        {
	    send_to_char( "Syntax:  trigger <trigger value(s)>\n\r", ch );
            return;
        }

	free_string( pMobProg->arglist );
	pMobProg->arglist = str_dup( arg2 );
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        return;
    }


    if ( !str_prefix( arg1, "program" ) )
    {
        if ( arg2[0] == '\0' )
        {
	    string_append( ch, &pMobProg->comlist );
	    SET_BIT( pArea->area_flags, AREA_CHANGED );
            return;
        }

	send_to_char( "Syntax:  program\n\r", ch );
        return;
    }


    if ( ( value = flag_value( mprog_type_flags, arg ) ) != NO_FLAG )
    {
	if ( pMobProg->type )
	    REMOVE_BIT( pMob->progtypes, pMobProg->type );
	pMobProg->type = value;
	SET_BIT( pMob->progtypes, pMobProg->type );

	SET_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "MOBProg type set.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg1, "create" ) )
    {
	mpedit_create( ch, argument );
        return;
    }

    interpret( ch, arg );
    return;
}



void do_oindex( CHAR_DATA * ch, char *argument )
{
    char            buf  [ MAX_STRING_LENGTH ];
    char            arg  [ MAX_INPUT_LENGTH ];
    char            arg2 [ MAX_INPUT_LENGTH ];
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA    *paf;
    int             cnt;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Oindex whom?\n\r", ch );
        return;
    }

    if ( !( pObj = get_obj_index( atoi( arg ) ) ) )
    {
	send_to_char( "Invalid obj index VNUM.\n\r", ch );
        return;
    }

    sprintf( buf, "{cName: {x%s{c.{x\n\r{cArea: [%5d] '{x%s{c'.{x\n\r",
	    pObj->name,
	    !pObj->area ? -1 : pObj->area->vnum,
	    !pObj->area ? "No Area" : pObj->area->name );
    send_to_char( buf, ch );


    sprintf( buf, "{cVnum: {x%d{c.{x\n\r{cType: {x%s{c.{x\n\r",
	    pObj->vnum,
	    flag_string( type_flags, pObj->item_type ) );
    send_to_char( buf, ch );

    sprintf( buf, "{cWear flags: {x%s{c.{x\n\r",
	    flag_string( wear_flags, pObj->wear_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "{cExtra flags: {x%s{c.{x\n\r",
	    flag_string( extra_flags, pObj->extra_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "{cWeight: {x%d{c.{x\n\r{cCost: {x%d{c.{x\n\r",
	    pObj->weight, pObj->cost );
    send_to_char( buf, ch );

    if ( pObj->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char( "{cExtra description keywords: {x", ch );

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    send_to_char( " ", ch );
	}

	send_to_char( "{c.{x\n\r", ch );
    }

    sprintf( buf, "{cShort description: {x%s{c.{x\n\r",
	    pObj->short_descr );
    send_to_char( buf, ch );

    sprintf( buf, "{cLong description: {x%s{x\n\r",
	    pObj->description );
    send_to_char( buf, ch );

    for ( cnt = 0, paf = pObj->affected; paf; paf = paf->next )
    {
	if ( cnt == 0 )
	{
	    send_to_char( "\n\r{o{c{B| Number | Modifier | Affects      | Bitvector             {x\n\r", ch );
	    send_to_char( "{o{c{B+--------+----------+--------------+-----------------------{x\n\r", ch );
	}
	sprintf( buf, "{o{c{B|{o %-4d   {x{o{c{B| %-8d | %-12.12s | ",
		cnt,
		paf->modifier,
		flag_string( apply_flags, paf->location ) );
	sprintf( buf + strlen( buf ), "%-22.22s{x\n\r",
		flag_string( affect_flags, paf->bitvector ) );
	send_to_char( buf, ch );
	cnt++;
    }

    send_to_char( "\n\r", ch );
    show_obj_values( ch, pObj );

    return;
}


void do_mindex( CHAR_DATA * ch, char *argument )
{
    char            buf  [ MAX_STRING_LENGTH ];
    char            arg  [ MAX_INPUT_LENGTH ];
    char            arg2 [ MAX_INPUT_LENGTH ];
    MOB_INDEX_DATA *pMob;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mindex whom?\n\r", ch );
        return;
    }

    if ( !( pMob = get_mob_index( atoi( arg ) ) ) )
    {
	send_to_char( "Invalid mob index VNUM.\n\r", ch );
        return;
    }

    sprintf( buf, "{cName: {x%s{c.{x\n\r{cArea: [%5d] '{x%s{c'.{x\n\r",
	    pMob->player_name,
	    !pMob->area ? -1 : pMob->area->vnum,
	    !pMob->area ? "No Area" : pMob->area->name );
    send_to_char( buf, ch );

    sprintf( buf, "{cRace: {x%s{c.{x\n\r", race_table[ pMob->race ].name );
    send_to_char( buf, ch );

    sprintf( buf, "{cVnum: {x%d{c.  Sex: {x%s{c.{x\n\r",
	    pMob->vnum,
	    pMob->sex == SEX_MALE ? "male" :
	    pMob->sex == SEX_FEMALE ? "female" : "neutral" );
    send_to_char( buf, ch );

    sprintf( buf,
	    "{cLevel: {x%2d{c.  Align: {x%4d{c.{x\n\r",
	    pMob->level, pMob->alignment );
    send_to_char( buf, ch );

    sprintf( buf, "{cAffected by: {x%s{c.{x\n\r",
	    flag_string( affect_flags, pMob->affected_by ) );
    send_to_char( buf, ch );

    sprintf( buf, "{cAct: {x%s{c.{x\n\r",
	    flag_string( act_flags, pMob->act ) );
    send_to_char( buf, ch );

    sprintf( buf, "{cShort description: {x%s{c.{x\n\r{cLong description: {x%s",
	    pMob->short_descr,
	    pMob->long_descr );
    send_to_char( buf, ch );

    if ( pMob->spec_fun )
    {
	sprintf( buf, "{cSpec fun: {x%s{c.{x\n\r",
		spec_mob_string( pMob->spec_fun ) );
	send_to_char( buf, ch );
    }

    if ( pMob->pGame )
    {
	sprintf( buf, "{cGame fun: {x%s{c. Bankroll: {x%d{c.  Maxwait: {x%d{c."
		      " Cheat: {x%s{c.{x\n\r",
		game_string( pMob->pGame->game_fun ),
		pMob->pGame->bankroll,
		pMob->pGame->max_wait,
		pMob->pGame->cheat ? "TRUE" : "FALSE" );
	send_to_char( buf, ch );
    }

    sprintf( buf, "{cDescription:{x\n\r%s", pMob->description );
    send_to_char( buf, ch );

    if ( pMob->pShop )
    {
	SHOP_DATA *pShop;
	int iTrade;

	pShop = pMob->pShop;

	sprintf( buf,
		"{cShop data for [{x%5d{c]:{x\n\r  {cMarkup for purchaser:"
		" {x%d%%\n\r  {cMarkdown for seller:  {x%d%%\n\r",
		pShop->keeper, pShop->profit_buy, pShop->profit_sell );
	send_to_char( buf, ch );
	sprintf( buf, "  {cHours: {x%d{c to {x%d.\n\r",
		pShop->open_hour, pShop->close_hour );
	send_to_char( buf, ch );

	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	{
	    if ( pShop->buy_type[iTrade] != 0 )
	    {
		if ( iTrade == 0 )
		{
		    send_to_char( "  {cNumber Trades Type{x\n\r", ch );
		    send_to_char( "  {c------ -----------{x\n\r", ch );
		}
		sprintf( buf, "  {c[{x%4d{c] {x%s\n\r", iTrade,
		  flag_string( type_flags, pShop->buy_type[iTrade] ) );
		send_to_char( buf, ch );
	    }
	}
    }

    return;
}


void mpedit_show( CHAR_DATA *ch, char *argument )
{
    MPROG_DATA     *pMobProg;
    MOB_INDEX_DATA *pMob;
    char            buf [MAX_STRING_LENGTH];

    pMob	= (MOB_INDEX_DATA *) ch->desc->olc_editing;
    pMobProg	= edit_mprog( ch, pMob );

    if ( argument[0] == '\0' )
	show_mprog( ch, pMobProg );
    else
    if ( is_number( argument ) )
    {
	MPROG_DATA *mprg;
	int         prg = atoi( argument );
	int         cnt = mprog_count( pMob );

	if ( prg < 1 || prg > cnt )
	{
	    sprintf( buf, "Valid range is 1 to %d.\n\r", cnt );
	    send_to_char( buf, ch );
	    return;
	}

	for ( mprg = pMob->mobprogs; mprg && prg-- > 1; mprg = mprg->next )
	    ;
	show_mprog( ch, mprg );
    }
    else if ( !str_cmp( argument, "all" ) )
    {
	for ( pMobProg = pMob->mobprogs; pMobProg; pMobProg = pMobProg->next )
	    show_mprog( ch, pMobProg );
	send_to_char( "|\n\r", ch );
    }
    else
	send_to_char( "Syntax: show <all>\n\r", ch );

    return;
}
