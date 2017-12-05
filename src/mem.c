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
 *  GreedMud 0.88 improvements copyright (C) 1997, 1998 by Vasco Costa.    *
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



/*
 * Globals
 */
AREA_DATA *		area_free;
EXTRA_DESCR_DATA *	extra_descr_free;
EXIT_DATA *		exit_free;
ROOM_INDEX_DATA *	room_index_free;
OBJ_INDEX_DATA *	obj_index_free;
SHOP_DATA *		shop_free;
GAME_DATA *		game_free;
MOB_INDEX_DATA *	mob_index_free;
HHF_DATA *		hhf_free;
RESET_DATA *		reset_free;
HELP_DATA *		help_free;

HELP_DATA *		help_last;



RESET_DATA *new_reset_data( void )
{
    RESET_DATA *pReset;

    if ( !reset_free )
    {
	pReset = alloc_perm( sizeof( *pReset ) );
	top_reset++;
    }
    else
    {
	pReset		= reset_free;
	reset_free	= reset_free->next;
    }

    pReset->next	= NULL;
    pReset->prev	= NULL;
    pReset->command	= 'X';
    pReset->rs_vnum 	= 0;
    pReset->loc 	= 0;
    pReset->percent	= 100;

    return pReset;
}


void free_reset_data( RESET_DATA *pReset )
{
    pReset->next	= reset_free;
    reset_free		= pReset;
    return;
}


AREA_DATA *new_area( void )
{
    AREA_DATA *pArea;
    char       buf[ MAX_INPUT_LENGTH ];

    if ( !area_free )
    {
	pArea = alloc_perm( sizeof( *pArea ) );
	top_area++;
    }
    else
    {
	pArea = area_free;
	area_free = area_free->next;
    }

    pArea->next		= NULL;
    pArea->name		= str_dup( "New area" );
    pArea->recall	= ROOM_VNUM_TEMPLE;
    pArea->area_flags	= AREA_ADDED;
    pArea->security	= 1;
    pArea->builders	= str_dup( "None" );
    pArea->lvnum	= 0;
    pArea->uvnum	= 0;
    pArea->age		= 0;
    pArea->nplayer	= 0;
    pArea->vnum		= top_area - 1;

    sprintf( buf, "area%d.are", pArea->vnum );
    pArea->filename 	= str_dup( buf );

    return pArea;
}


void free_area( AREA_DATA *pArea )
{
    free_string( pArea->name );
    free_string( pArea->filename );
    free_string( pArea->builders );

    pArea->next	= area_free->next;
    area_free	= pArea;
    return;
}


EXIT_DATA *new_exit( void )
{
    EXIT_DATA *pExit;

    if ( !exit_free )
    {
	pExit = alloc_perm( sizeof( *pExit ) );
	top_exit++;
    }
    else
    {
	pExit = exit_free;
	exit_free = exit_free->next;
    }

    pExit->to_room	= NULL;
    pExit->next		= NULL;
    pExit->vnum		= 0;
    pExit->exit_info	= 0;
    pExit->key		= 0;
    pExit->keyword	= &str_empty[0];;
    pExit->description	= &str_empty[0];;
    pExit->rs_flags	= 0;

    return pExit;
}


void free_exit( EXIT_DATA *pExit )
{
    free_string( pExit->keyword );
    free_string( pExit->description );

    pExit->next	= exit_free;
    exit_free	= pExit;
    return;
}


void free_extra_descr( EXTRA_DESCR_DATA *pExtra )
{
    free_string( pExtra->keyword );
    free_string( pExtra->description );

    pExtra->next	= extra_descr_free;
    extra_descr_free	= pExtra;
    return;
}


ROOM_INDEX_DATA *new_room_index( void )
{
    ROOM_INDEX_DATA *pRoom;
    int              door;

    if ( !room_index_free )
    {
	pRoom = alloc_perm( sizeof( *pRoom ) );
	top_room++;
    }
    else
    {
	pRoom = room_index_free;
	room_index_free = room_index_free->next;
    }

    pRoom->next		= NULL;
    pRoom->people	= NULL;
    pRoom->contents	= NULL;
    pRoom->extra_descr	= NULL;
    pRoom->area		= NULL;

    for ( door = 0; door < MAX_DIR; door++ )
	pRoom->exit[door] = NULL;

    pRoom->name		= &str_empty[0];
    pRoom->description	= &str_empty[0];
    pRoom->vnum		= 0;
    pRoom->room_flags	= 0;
    pRoom->light	= 0;
    pRoom->sector_type	= 0;

    return pRoom;
}


void free_room_index( ROOM_INDEX_DATA *pRoom )
{
    EXTRA_DESCR_DATA *pExtra;
    RESET_DATA       *pReset;
    int               door;

    free_string( pRoom->name );
    free_string( pRoom->description );

    for ( door = 0; door < MAX_DIR; door++ )
	if ( pRoom->exit[door] )
	    free_exit( pRoom->exit[door] );

    for ( pExtra = pRoom->extra_descr; pExtra; pExtra = pExtra->next )
	free_extra_descr( pExtra );

    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
	free_reset_data( pReset );

    pRoom->next		= room_index_free;
    room_index_free	= pRoom;
    return;
}


void free_affect( AFFECT_DATA *pAf )
{
    pAf->next	= affect_free;
    affect_free	= pAf;
    return;
}


SHOP_DATA *new_shop( void )
{
    SHOP_DATA *pShop;
    int        buy;

    if ( !shop_free )
    {
	pShop = alloc_perm( sizeof( *pShop ) );
	top_shop++;
    }
    else
    {
	pShop = shop_free;
	shop_free = shop_free->next;
    }

    pShop->next = NULL;
    pShop->keeper = 0;

    for ( buy = 0; buy < MAX_TRADE; buy++ )
	pShop->buy_type[buy] = 0;

    pShop->profit_buy	= 100;
    pShop->profit_sell	= 100;
    pShop->open_hour	= 0;
    pShop->close_hour	= 23;

    return pShop;
}


void free_shop( SHOP_DATA *pShop )
{
    pShop->next	= shop_free;
    shop_free	= pShop;
    return;
}


GAME_DATA *new_game( void )
{
    GAME_DATA *pGame;

    if ( !game_free )
    {
	pGame = alloc_perm( sizeof( *pGame ) );
	top_game++;
    }
    else
    {
	pGame		= game_free;
	game_free	= game_free->next;
    }

    pGame->next		= NULL;
    pGame->croupier	= 0;
    pGame->game_fun	= NULL;
    pGame->bankroll	= 5000;
    pGame->max_wait	= 100;
    pGame->cheat	= 0;

    return pGame;
}


void free_game( GAME_DATA *pGame )
{
    pGame->next	= game_free;
    game_free	= pGame;
    return;
}


OBJ_INDEX_DATA *new_obj_index( void )
{
    OBJ_INDEX_DATA *pObj;
    int             value;

    if ( !obj_index_free )
    {
	pObj = alloc_perm( sizeof( *pObj ) );
	top_obj_index++;
    }
    else
    {
	pObj		= obj_index_free;
	obj_index_free	= obj_index_free->next;
    }

    pObj->next		= NULL;
    pObj->spec_fun	= NULL;
    pObj->extra_descr	= NULL;
    pObj->affected	= NULL;
    pObj->area		= NULL;
    pObj->name		= str_dup( "no name" );
    pObj->short_descr	= str_dup( "(no short description)" );
    pObj->description	= str_dup( "(no description)" );
    pObj->vnum		= 0;
    pObj->item_type	= ITEM_TRASH;
    pObj->extra_flags	= 0;
    pObj->wear_flags	= 0;
    pObj->count		= 0;
    pObj->weight	= 0;
    pObj->cost		= 0;

    for ( value = 0; value < 5; value++ )
	pObj->value[value] = 0;

    return pObj;
}


void free_obj_index( OBJ_INDEX_DATA *pObj )
{
    EXTRA_DESCR_DATA *pExtra;
    AFFECT_DATA      *pAf;

    free_string( pObj->name );
    free_string( pObj->short_descr );
    free_string( pObj->description );

    for ( pAf = pObj->affected; pAf; pAf = pAf->next )
	free_affect( pAf );

    for ( pExtra = pObj->extra_descr; pExtra; pExtra = pExtra->next )
	free_extra_descr( pExtra );

    pObj->next		= obj_index_free;
    obj_index_free	= pObj;
    return;
}


MOB_INDEX_DATA *new_mob_index( void )
{
    MOB_INDEX_DATA *pMob;

    if ( !mob_index_free )
    {
	pMob = alloc_perm( sizeof( *pMob ) );
	top_mob_index++;
    }
    else
    {
	pMob		= mob_index_free;
	mob_index_free	= mob_index_free->next;
    }

    pMob->next		= NULL;
    pMob->spec_fun	= NULL;
    pMob->pShop		= NULL;
    pMob->area		= NULL;
    pMob->player_name	= str_dup( "no name" );
    pMob->short_descr	= str_dup( "(no short description)" );
    pMob->long_descr	= str_dup( "(no long description)\n\r" );
    pMob->description	= &str_empty[0];
    pMob->vnum		= 0;
    pMob->count		= 0;
    pMob->killed	= 0;
    pMob->sex		= 0;
    pMob->level		= 0;
    pMob->act		= ACT_IS_NPC;

    vzero( pMob->affected_by );

    pMob->alignment	= 0;
    pMob->hitroll	= 0;
    pMob->ac		= 0;
    pMob->hitnodice	= 0;
    pMob->hitsizedice	= 0;
    pMob->hitplus	= 0;
    pMob->damnodice	= 0;
    pMob->damsizedice	= 0;
    pMob->damplus	= 0;
    pMob->gold		= 0;

    return pMob;
}


void free_mob_index( MOB_INDEX_DATA *pMob )
{
    free_string( pMob->player_name );
    free_string( pMob->short_descr );
    free_string( pMob->long_descr );
    free_string( pMob->description );

    free_shop( pMob->pShop );

    pMob->next		= mob_index_free;
    mob_index_free	= pMob;
    return;
}


HHF_DATA *new_hhf_data( void )
{
    HHF_DATA *pHhf;

    if ( !hhf_free )
    {
	pHhf = alloc_perm( sizeof( *pHhf ) );
	top_hhf++;
    }
    else
    {
	pHhf		= hhf_free;
	hhf_free	= hhf_free->next;
    }

    pHhf->next		= NULL;
    pHhf->name		= NULL;
    pHhf->who		= NULL;

    return pHhf;
}


void free_hhf_data( HHF_DATA *pHhf )
{
    free_string( pHhf->name );

    pHhf->next		= hhf_free;
    hhf_free		= pHhf;
    return;
}
