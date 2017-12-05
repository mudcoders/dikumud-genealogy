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



/*
 * Removes the tildes and carriage returns from a string.
 * Used for player-entered strings that go into disk files.
 */
char *fix_string( const char *str )
{
    static char strfix [4 * MAX_STRING_LENGTH];
    int         i;
    int         o;

    if ( !str )
	return '\0';

    for ( o = i = 0; str[i + o] != '\0'; i++ )
    {
	if ( str[i + o] == '\r' || str[i + o] == '~' )
	    o++;
	strfix[i] = str[i + o];
    }
    strfix[i] = '\0';
    return strfix;
}


void save_area_list( )
{
    FILE      *fp;
    AREA_DATA *pArea;

    if ( !( fp = fopen( "AREA.LST", "w" ) ) )
    {
	bug( "Save_area_list: fopen", 0 );
	perror( "AREA.LST" );
    }
    else
    {
	/* add any help files to be loaded at startup to this section */
	fprintf( fp, "help.are\n" );
	fprintf( fp, "olc.hlp\n"  );

	for ( pArea = area_first; pArea; pArea = pArea->next )
	    fprintf( fp, "%s\n", pArea->filename );

	fprintf( fp, "$\n" );
	fclose( fp );
    }

    return;
}


void save_mobiles( FILE * fp, AREA_DATA * pArea )
{
    MOB_INDEX_DATA *pMobIndex;
    int             vnum;

    fprintf( fp, "#MOBILES\n" );
    for ( vnum = pArea->low_m_vnum; vnum <= pArea->hi_m_vnum; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    if ( pMobIndex->area == pArea )
	    {
		fprintf( fp, "#%d\n",	pMobIndex->vnum );
		fprintf( fp, "%s~\n",	pMobIndex->player_name );
		fprintf( fp, "%s~\n",	pMobIndex->short_descr );
		fprintf( fp, "%s~\n",	fix_string( pMobIndex->long_descr ) );
		fprintf( fp, "%s~\n",	fix_string( pMobIndex->description ) );
		fprintf( fp, "%d ",	pMobIndex->act );
		fprintf( fp, "%d ",	pMobIndex->affected_by );
		fprintf( fp, "%d S\n",	pMobIndex->alignment );
		fprintf( fp, "%d ",	pMobIndex->level );
		fprintf( fp, "%d ",	pMobIndex->hitroll );
		fprintf( fp, "%d ",	pMobIndex->ac );
		fprintf( fp, "%dd%d+%d ", pMobIndex->hitnodice,
			pMobIndex->hitsizedice,
			pMobIndex->hitplus );
		fprintf( fp, "%dd%d+%d\n", pMobIndex->damnodice,
			pMobIndex->damsizedice,
			pMobIndex->damplus );
		fprintf( fp, "%d ", pMobIndex->gold );
		fprintf( fp, "0\n0 " );
		fprintf( fp, "%s~ ", race_table[pMobIndex->race].name );
		fprintf( fp, "%d\n", pMobIndex->sex );

		/*
		 * This is responsible for writing MOBprograms in_file.
		 * By Farix of Oz.
		 */
		if ( pMobIndex->mobprogs )
		{
		    MPROG_DATA *mPtr;

		    for ( mPtr = pMobIndex->mobprogs; mPtr; mPtr = mPtr->next )
		    {
			fprintf( fp, ">%s ", mprog_type_to_name( mPtr->type ) );
			fprintf( fp, "%s~\n", mPtr->arglist );
			fprintf( fp, "%s~\n", fix_string( mPtr->comlist ) );
		    }
		    fprintf( fp, "|\n" );
		}
	    }
	}
    }
    fprintf( fp, "#0\n\n\n" );
    return;
}


void new_save_objects( FILE * fp, AREA_DATA * pArea )
{
    EXTRA_DESCR_DATA *pEd;
    OBJ_INDEX_DATA   *pObjIndex;
    AFFECT_DATA      *pAf;
    int               vnum;

    fprintf( fp, "#NEWOBJECTS\n" );
    for ( vnum = pArea->low_o_vnum; vnum <= pArea->hi_o_vnum; vnum++ )
    {
	if ( ( pObjIndex = get_obj_index( vnum ) ) )
	{
	    if ( pObjIndex->area == pArea )
	    {
		fprintf( fp, "#%d\n",	pObjIndex->vnum );
		fprintf( fp, "%s~\n",	pObjIndex->name );
		fprintf( fp, "%s~\n",	pObjIndex->short_descr );
		fprintf( fp, "%s~\n",	fix_string( pObjIndex->description ) );
		fprintf( fp, "~\n" );
		fprintf( fp, "%d ",	pObjIndex->item_type );
		fprintf( fp, "%d ",	pObjIndex->extra_flags );
		fprintf( fp, "%d\n",	pObjIndex->wear_flags );

		switch ( pObjIndex->item_type )
		{
		    default:
			fprintf( fp, "%d~ %d~ %d~ %d~ %d~\n",
				pObjIndex->value[0],
				pObjIndex->value[1],
				pObjIndex->value[2],
				pObjIndex->value[3],
				pObjIndex->value[4] );
			break;

		    case ITEM_PILL:
		    case ITEM_POTION:
		    case ITEM_SCROLL:
			fprintf( fp, "%d~ %s~ %s~ %s~ %s~\n",
				pObjIndex->value[0],
				pObjIndex->value[1] != -1 ?
				skill_table[pObjIndex->value[1]].name : " ",
				pObjIndex->value[2] != -1 ?
				skill_table[pObjIndex->value[2]].name : " ",
				pObjIndex->value[3] != -1 ?
				skill_table[pObjIndex->value[3]].name : " ",
				pObjIndex->value[4] != -1 ?
				skill_table[pObjIndex->value[4]].name : " " );
			break;

		    case ITEM_STAFF:
		    case ITEM_WAND:
			fprintf( fp, "%d~ %d~ %d~ %s~ %d~\n",
				pObjIndex->value[0],
				pObjIndex->value[1],
				pObjIndex->value[2],
				pObjIndex->value[3] != -1 ?
				skill_table[pObjIndex->value[3]].name : " ",
				pObjIndex->value[4] );
			break;
		}
		fprintf( fp, "%d ",	pObjIndex->weight );
		fprintf( fp, "%d 0\n",	pObjIndex->cost );

		for ( pAf = pObjIndex->affected; pAf; pAf = pAf->next )
		    fprintf( fp, "A\n%d %d %d\n",
			    pAf->location,
			    pAf->modifier,
			    pAf->bitvector );

		for ( pEd = pObjIndex->extra_descr; pEd; pEd = pEd->next )
		    fprintf( fp, "E\n%s~\n%s~\n",
			    pEd->keyword,
			    fix_string( pEd->description ) );
	    }
	}
    }
    fprintf( fp, "#0\n\n\n" );
    return;
}


void save_rooms( FILE * fp, AREA_DATA * pArea )
{
    ROOM_INDEX_DATA  *pRoomIndex;
    EXTRA_DESCR_DATA *pEd;
    EXIT_DATA        *pExit;
    int               vnum;
    int               door;

    fprintf( fp, "#ROOMDATA\n" );
    for ( vnum = pArea->low_r_vnum; vnum <= pArea->hi_r_vnum; vnum++ )
    {
	if ( ( pRoomIndex = get_room_index( vnum ) ) )
	{
	    if ( pRoomIndex->area == pArea )
	    {
		fprintf( fp, "#%d\n",	pRoomIndex->vnum );
		fprintf( fp, "%s~\n",	pRoomIndex->name );
		fprintf( fp, "%s~\n",	fix_string( pRoomIndex->description ) );
		fprintf( fp, "0 " );
		fprintf( fp, "%d ",	pRoomIndex->orig_room_flags );
		fprintf( fp, "%d\n",	pRoomIndex->sector_type );

		for ( pEd = pRoomIndex->extra_descr; pEd; pEd = pEd->next )
		    fprintf( fp, "E\n%s~\n%s~\n",
			    pEd->keyword,
			    fix_string( pEd->description ) );

		for ( door = 0; door < MAX_DIR; door++ )
		{
		    if ( ( pExit = pRoomIndex->exit[door] ) )
		    {
			fprintf( fp, "D%d\n", door );
			fprintf( fp, "%s~\n",
				fix_string( pExit->description ) );
			fprintf( fp, "%s~\n", pExit->keyword );
			fprintf( fp, "%d %d %d\n",
				pExit->rs_flags,
				pExit->key,
				pExit->to_room ? pExit->to_room->vnum : 0 );
		    }
		}
		fprintf( fp, "S\n" );
	    }
	}
    }
    fprintf( fp, "#0\n\n\n" );
    return;
}


void save_specials( FILE * fp, AREA_DATA * pArea )
{
    MOB_INDEX_DATA *pMobIndex;
    int             vnum;

    fprintf( fp, "#SPECIALS\n" );

    for ( vnum = pArea->low_m_vnum; vnum <= pArea->hi_m_vnum; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    if ( pMobIndex->area == pArea && pMobIndex->spec_fun )
	    {
		fprintf( fp, "M %d %s\n",
			pMobIndex->vnum,
			spec_string( pMobIndex->spec_fun ) );
	    }
	}
    }

    fprintf( fp, "S\n\n\n" );
    return;
}


/* New formating thanks to Rac */
void vsave_specials( FILE * fp, AREA_DATA * pArea )
{
    MOB_INDEX_DATA *pMobIndex;
    int             vnum;

    fprintf( fp, "#SPECIALS\n" );

    for ( vnum = pArea->low_m_vnum; vnum <= pArea->hi_m_vnum; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    if ( pMobIndex->area == pArea && pMobIndex->spec_fun )
	    {
		fprintf( fp, "M %5d %-30s \t* %s\n",
			pMobIndex->vnum,
			spec_string( pMobIndex->spec_fun ),
			pMobIndex->short_descr );
	    }
	}
    }

    fprintf( fp, "S\n\n\n" );
    return;
}


/* Written by: Maniac */
void save_games( FILE *fp, AREA_DATA *pArea )
{
    MOB_INDEX_DATA *pMobIndex;
    GAME_DATA      *pGameIndex;
    int             vnum;

    fprintf( fp, "#GAMES\n" );

    for ( vnum = pArea->low_m_vnum; vnum <= pArea->hi_m_vnum; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    if ( pMobIndex->area == pArea && pMobIndex->pGame )
	    {
		pGameIndex = pMobIndex->pGame;

		fprintf( fp, "M %d %s %d %d %d\n",
			pGameIndex->croupier,
			game_string( pGameIndex->game_fun ),
			pGameIndex->bankroll,
			pGameIndex->max_wait,
			pGameIndex->cheat );
	    }
	}
    }

    fprintf( fp, "S\n\n\n" );
    return;
}


/* Written by: Maniac */
void vsave_games( FILE *fp, AREA_DATA *pArea )
{
    MOB_INDEX_DATA *pMobIndex;
    GAME_DATA      *pGameIndex;
    int             vnum;

    fprintf( fp, "#GAMES\n" );

    for ( vnum = pArea->low_m_vnum; vnum <= pArea->hi_m_vnum; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    if ( pMobIndex->area == pArea && pMobIndex->pGame )
	    {
		pGameIndex = pMobIndex->pGame;

		fprintf( fp, "M %5d %-30s %10d %3d %1d\t* %s\n",
			pGameIndex->croupier,
			game_string( pGameIndex->game_fun ),
			pGameIndex->bankroll,
			pGameIndex->max_wait,
			pGameIndex->cheat,
			pMobIndex->short_descr );
	    }
	}
    }

    fprintf( fp, "S\n\n\n" );
    return;
}


/* New format thanks to Rac */
void save_resets( FILE * fp, AREA_DATA * pArea )
{
    ROOM_INDEX_DATA *pRoomIndex;
    MOB_INDEX_DATA  *pLastMob = NULL;
    OBJ_INDEX_DATA  *pLastObj;
    RESET_DATA      *pReset;
    char             buf	[ MAX_STRING_LENGTH ];
    int              vnum;

    fprintf( fp, "#RESETS\n" );

    for ( vnum = pArea->low_r_vnum; vnum <= pArea->hi_r_vnum; vnum++ )
    {
	if ( ( pRoomIndex = get_room_index( vnum ) ) )
	{
	    if ( pRoomIndex->area == pArea )
	    {
		for ( pReset = pRoomIndex->reset_first; pReset; pReset = pReset->next )
		{
		    switch ( pReset->command )
		    {
			default:
			    bug( "Save_resets: bad command %c.", pReset->command );
			    break;

			case 'M':
			    pLastMob = get_mob_index( pReset->arg1 );
			    fprintf( fp, "M 0 %d %d %d\n",
				    pReset->arg1,
				    pReset->arg2,
				    pReset->arg3 );
			    break;

			case 'O':
			    pLastObj = get_obj_index( pReset->arg1 );
			    fprintf( fp, "O 0 %d 0 %d\n",
				    pReset->arg1,
				    pReset->arg3 );
			    break;

			case 'P':
			    pLastObj = get_obj_index( pReset->arg1 );
			    fprintf( fp, "P 0 %d 0 %d\n",
				    pReset->arg1,
				    pReset->arg3 );
			    break;

			case 'G':
			    fprintf( fp, "G 0 %d 0\n", pReset->arg1 );
			    if ( !pLastMob )
			    {
				sprintf( buf,
					"Save_resets: !NO_MOB! in [%s]", pArea->filename );
				bug( buf, 0 );
			    }
			    break;

			case 'E':
			    fprintf( fp, "E 0 %d 0 %d\n",
				    pReset->arg1,
				    pReset->arg3 );
			    if ( !pLastMob )
			    {
				sprintf( buf,
					"Save_resets: !NO_MOB! in [%s]", pArea->filename );
				bug( buf, 0 );
			    }
			    break;

			case 'D':
			    break;

			case 'R':
			    fprintf( fp, "R 0 %d %d\n",
				    pReset->arg1,
				    pReset->arg2 );
			    break;
		    }
		}
	    }
	}
    }
    fprintf( fp, "S\n\n\n" );
    return;
}


/* New format thanks to Rac */
void vsave_resets( FILE * fp, AREA_DATA * pArea )
{
    ROOM_INDEX_DATA *pRoomIndex;
    MOB_INDEX_DATA  *pLastMob = NULL;
    OBJ_INDEX_DATA  *pLastObj;
    RESET_DATA      *pReset;
    char             buf	[ MAX_STRING_LENGTH ];
    int              vnum;

    fprintf( fp, "#RESETS\n" );

    for ( vnum = pArea->low_r_vnum; vnum <= pArea->hi_r_vnum; vnum++ )
    {
	if ( ( pRoomIndex = get_room_index( vnum ) ) )
	{
	    if ( pRoomIndex->area == pArea )
	    {
		for ( pReset = pRoomIndex->reset_first; pReset; pReset = pReset->next )
		{
		    switch ( pReset->command )
		    {
			default:
			    bug( "Save_resets: bad command %c.", pReset->command );
			    break;

			case 'M':
			    pLastMob = get_mob_index( pReset->arg1 );
			    fprintf( fp, "M 0 %5d %2d %5d \t* %s -> %s\n",
				    pReset->arg1,
				    pReset->arg2,
				    pReset->arg3,
				    pLastMob->short_descr,
				    pRoomIndex->name );
			    break;

			case 'O':
			    pLastObj = get_obj_index( pReset->arg1 );
			    fprintf( fp, "O 0 %5d  0 %5d \t* %s -> %s\n",
				    pReset->arg1,
				    pReset->arg3,
				    capitalize( pLastObj->short_descr ),
				    pRoomIndex->name );
			    break;

			case 'P':
			    pLastObj = get_obj_index( pReset->arg1 );
			    fprintf( fp, "P 0 %5d  0 %5d \t* %s inside %s\n",
				    pReset->arg1,
				    pReset->arg3,
				    capitalize( get_obj_index( pReset->arg1 )->short_descr ),
				    pLastObj ? pLastObj->short_descr : "!NO_OBJ!" );

			    if ( !pLastObj )	/*
						 * Thanks Rac! 
						 */
			    {
				sprintf( buf, "Save_resets: P with !NO_OBJ! in [%s]", pArea->filename );
				bug( buf, 0 );
			    }

			    break;

			case 'G':
			    pLastObj = get_obj_index( pReset->arg1 );
			    fprintf( fp, "G 0 %5d  0      \t*   %s\n",
				    pReset->arg1,
				    capitalize( pLastObj->short_descr ) );

			    if ( !pLastMob )
			    {
				sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->filename );
				bug( buf, 0 );
			    }
			    break;

			case 'E':
			    fprintf( fp, "E 0 %5d  0 %5d \t*   %s %s\n",
				    pReset->arg1,
				    pReset->arg3,
				    capitalize( get_obj_index( pReset->arg1 )->short_descr ),
				    flag_string( wear_loc_strings, pReset->arg3 ) );
			    if ( !pLastMob )
			    {
				sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->filename );
				bug( buf, 0 );
			    }
			    break;

			case 'D':
			    break;

			case 'R':
			    fprintf( fp, "R 0 %5d %2d      \t* Randomize %s\n",
				    pReset->arg1,
				    pReset->arg2,
				    pRoomIndex->name );
			    break;
		    }
		}
	    }
	}
    }
    fprintf( fp, "S\n\n\n" );
    return;
}


void save_shops( FILE * fp, AREA_DATA * pArea )
{
    MOB_INDEX_DATA *pMobIndex;
    SHOP_DATA      *pShopIndex;
    int             iTrade;
    int             vnum;

    fprintf( fp, "#SHOPS\n" );

    for ( vnum = pArea->low_m_vnum; vnum <= pArea->hi_m_vnum; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    if ( pMobIndex->area == pArea && pMobIndex->pShop )
	    {
		pShopIndex = pMobIndex->pShop;

		fprintf( fp, "%d ", pShopIndex->keeper );
		for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
		{
		    if ( pShopIndex->buy_type[iTrade] != 0 )
			fprintf( fp, "%d ", pShopIndex->buy_type[iTrade] );
		    else
			fprintf( fp, "0 " );
		}
		fprintf( fp, "%d %d ", pShopIndex->profit_buy, pShopIndex->profit_sell );
		fprintf( fp, "%d %d\n", pShopIndex->open_hour, pShopIndex->close_hour );
	    }
	}
    }

    fprintf( fp, "0\n\n\n" );
    return;
}


/* New format thanks to Rac */
void vsave_shops( FILE * fp, AREA_DATA * pArea )
{
    MOB_INDEX_DATA *pMobIndex;
    SHOP_DATA      *pShopIndex;
    int             iTrade;
    int             vnum;

    fprintf( fp, "#SHOPS\n" );

    for ( vnum = pArea->low_m_vnum; vnum <= pArea->hi_m_vnum; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    if ( pMobIndex->area == pArea && pMobIndex->pShop )
	    {
		pShopIndex = pMobIndex->pShop;

		fprintf( fp, "%d ", pShopIndex->keeper );
		for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
		{
		    if ( pShopIndex->buy_type[iTrade] != 0 )
			fprintf( fp, "%2d ", pShopIndex->buy_type[iTrade] );
		    else
			fprintf( fp, " 0 " );
		}
		fprintf( fp, "%3d %3d ", pShopIndex->profit_buy, pShopIndex->profit_sell );
		fprintf( fp, "%2d %2d ", pShopIndex->open_hour, pShopIndex->close_hour );
		fprintf( fp, "\t* %s\n", get_mob_index( pShopIndex->keeper )->short_descr );
	    }
	}
    }

    fprintf( fp, "0\n\n\n" );
    return;
}


/* Written by: Walker <nkrendel@evans.Denver.Colorado.EDU> */
void save_helps( FILE * fp, AREA_DATA * pArea )
{
    HELP_DATA *pHelp;
    bool       found = FALSE;

    for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
    {
	if ( pHelp->area && pHelp->area == pArea )
	{
	    if ( !found )
	    {
		fprintf( fp, "#HELPS\n\n" );
		found = TRUE;
	    }
	    fprintf( fp, "%d %s~\n%s~\n\n",
		    pHelp->level,
		    all_capitalize( pHelp->keyword ),
		    fix_string( pHelp->text ) );
	}
    }

    if ( found )
	fprintf( fp, "0 $~\n\n" );

    return;
}


void save_area( AREA_DATA * pArea )
{
    FILE *fp;

    fclose( fpReserve );
    if ( !( fp = fopen( pArea->filename, "w" ) ) )
    {
	bug( "Open_area: fopen", 0 );
	perror( pArea->filename );
    }

    fprintf( fp, "#AREADATA\n" );

    fprintf( fp, "Name        %s~\n",	pArea->name );
    fprintf( fp, "Builders    %s~\n",	fix_string( pArea->builders ) );
    fprintf( fp, "VNUMs       %d %d\n",	pArea->lvnum, pArea->uvnum );
    fprintf( fp, "Security    %d\n",	pArea->security );
    fprintf( fp, "Recall      %d\n",	pArea->recall );
    fprintf( fp, "End\n\n\n" );

    save_helps( fp, pArea );
    save_mobiles( fp, pArea );
    new_save_objects( fp, pArea );
    save_rooms( fp, pArea );

    if ( IS_SET( pArea->area_flags, AREA_VERBOSE ) )
    {
	vsave_specials( fp, pArea );
	vsave_games( fp, pArea );
	vsave_resets( fp, pArea );
	vsave_shops( fp, pArea );
    }
    else
    {
	save_specials( fp, pArea );
	save_games( fp, pArea );
	save_resets( fp, pArea );
	save_shops( fp, pArea );
    }

    fprintf( fp, "#$\n" );

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


void do_asave( CHAR_DATA * ch, char *argument )
{
    AREA_DATA *pArea;
    char       arg1[ MAX_INPUT_LENGTH ];
    int        value;

    if ( !ch )
    {
	send_to_all_char( "A gentle breeze comes from the east.\n\r" );
	save_area_list( );
	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
	    save_area( pArea );
	    REMOVE_BIT( pArea->area_flags, AREA_CHANGED | AREA_ADDED );
	}
	send_to_all_char( "After the breeze all seems calm.\n\r" );
	return;
    }

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r", ch );
	send_to_char( "  asave <vnum>    - saves a particular area\n\r", ch );
	send_to_char( "  asave list      - saves the AREA.LST file\n\r", ch );
	send_to_char( "  asave area      - saves the area being edited\n\r", ch );
	send_to_char( "  asave changed   - saves all changed zones\n\r", ch );
	send_to_char( "  asave world     - saves the world! (db dump)\n\r", ch );
	send_to_char( "  asave ^ verbose - saves in verbose mode\n\r", ch );
	send_to_char( "\n\r", ch );
	return;
    }

    value = atoi( arg1 );

    if ( !( pArea = get_area_data( value ) ) && is_number( arg1 ) )
    {
	send_to_char( "That area does not exist.\n\r", ch );
	return;
    }

    if ( is_number( arg1 ) )
    {
	if ( !is_builder( ch, pArea ) )
	{
	    send_to_char( "You are not a builder for this area.\n\r", ch );
	    return;
	}

	if ( !str_cmp( "verbose", argument ) )
	    SET_BIT( pArea->area_flags, AREA_VERBOSE );
	save_area( pArea );
	REMOVE_BIT( pArea->area_flags, AREA_VERBOSE );
	return;
    }

    if ( !str_cmp( "world", arg1 ) )
    {
	save_area_list( );
	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
	    /*
	     * Builder must be assigned this area. 
	     */
	    if ( !is_builder( ch, pArea ) )
		continue;

	    if ( !str_cmp( "verbose", argument ) )
		SET_BIT( pArea->area_flags, AREA_VERBOSE );
	    save_area( pArea );
	    REMOVE_BIT( pArea->area_flags, AREA_CHANGED | AREA_ADDED | AREA_VERBOSE );
	}
	send_to_char( "You saved the world.\n\r", ch );
	send_to_all_char( "Database saved.\n\r" );
	return;
    }

    if ( !str_cmp( "changed", arg1 ) )
    {
	char buf[ MAX_INPUT_LENGTH ];

	send_to_char( "Saved zones:\n\r", ch );
	sprintf( buf, "None.\n\r" );

	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
	    if ( !is_builder( ch, pArea ) )
		continue;

	    if ( IS_SET( pArea->area_flags, AREA_CHANGED )
		|| IS_SET( pArea->area_flags, AREA_ADDED ) )
	    {
		if ( !str_cmp( "verbose", argument ) )
		    SET_BIT( pArea->area_flags, AREA_VERBOSE );
		save_area( pArea );
		REMOVE_BIT( pArea->area_flags, AREA_CHANGED | AREA_ADDED | AREA_VERBOSE );
		sprintf( buf, "%24s - '%s'\n\r", pArea->name, pArea->filename );
		send_to_char( buf, ch );
	    }
	}
	if ( !str_cmp( buf, "None.\n\r" ) )
	    send_to_char( buf, ch );
	return;
    }

    if ( !str_cmp( arg1, "list" ) )
    {
	save_area_list( );
	return;
    }

    if ( !str_cmp( arg1, "area" ) )
    {
	switch ( ch->desc->connected )
	{
	    case CON_AEDITOR:
		pArea = ( AREA_DATA * ) ch->desc->olc_editing;
		break;
	    case CON_REDITOR:
		pArea = ch->in_room->area;
		break;
	    case CON_OEDITOR:
		pArea = ( ( OBJ_INDEX_DATA * ) ch->desc->olc_editing )->area;
		break;
	    case CON_MEDITOR:
		pArea = ( ( MOB_INDEX_DATA * ) ch->desc->olc_editing )->area;
		break;
	    default:
		pArea = ch->in_room->area;
		break;
	}

	if ( !is_builder( ch, pArea ) )
	{
	    send_to_char( "You are not a builder for this area.\n\r", ch );
	    return;
	}

	if ( !str_cmp( "verbose", argument ) )
	    SET_BIT( pArea->area_flags, AREA_VERBOSE );
	save_area( pArea );
	REMOVE_BIT( pArea->area_flags, AREA_CHANGED | AREA_ADDED | AREA_VERBOSE );
	send_to_char( "Area saved.\n\r", ch );
	return;
    }

    do_asave( ch, "" );
    return;
}
