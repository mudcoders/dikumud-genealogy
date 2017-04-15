/***************************************************************************
 *  File: mem.c                                                            *
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
#include "merc.h"


/*
 * Globals
 */
extern          int                     top_reset;
extern          int                     top_area;
extern          int                     top_exit;
extern          int                     top_ed;
extern          int                     top_room;

/*AREA_DATA		*	area_free;
EXTRA_DESCR_DATA	*	extra_descr_free;
EXIT_DATA		*	exit_free;
ROOM_INDEX_DATA		*	room_index_free;
SOCIAL_DATA		*	social_index_free;
CLAN_DATA               *       clan_index_free;
OBJ_INDEX_DATA		*	obj_index_free;
SHOP_DATA		*	shop_free;
MOB_INDEX_DATA		*	mob_index_free;
MPROG_DATA              *       mprog_free;
TRAP_DATA               *       trap_free;
RESET_DATA		*	reset_free;
HELP_DATA		*	help_free;*/

HELP_DATA		*	help_last;



/*****************************************************************************
 Name:		new_reset_data
 Purpose:	Creates and clears a reset structure.
 ****************************************************************************/
RESET_DATA *new_reset_data( void )
{
    RESET_DATA *pReset;

/*    if ( !reset_free )
    {*/
	pReset		= alloc_perm( sizeof(*pReset) );
	top_reset++;
/*    }
    else
    {
	pReset		= reset_free;
	reset_free	= reset_free->next;
    }*/

    pReset->next	= NULL;
    pReset->command	= 'X';
    pReset->arg1	= 0;
    pReset->arg2	= 0;
    pReset->arg3	= 0;

    return pReset;
}



/*****************************************************************************
 Name:		free_reset_data
 Purpose:	Clears and deletes a reset structure.
 ****************************************************************************/
void free_reset_data( RESET_DATA *pReset )
{
/*    pReset->next            = reset_free;
    reset_free              = pReset;*/
    top_reset--;
    free_mem( pReset, sizeof( *pReset ) );
    return;
}



/*****************************************************************************
 Name:		new_area
 Purpose:	Creates and clears a new area structure.
 ****************************************************************************/
AREA_DATA *new_area( void )
{
    AREA_DATA *pArea;
    char buf[MAX_INPUT_LENGTH];

/*    if ( !area_free )
    {*/
	pArea		= alloc_perm( sizeof(*pArea) );
	top_area++;
/*    }
    else
    {
	pArea		= area_free;
	area_free	= area_free->next;
    }*/

    pArea->next		= NULL;
    pArea->name		= str_dup( "New area" );
    pArea->recall	= ROOM_VNUM_TEMPLE;
    pArea->age		= 0;
    pArea->nplayer	= 0;
    pArea->reset_sound  = NULL; /*&str_empty[0]; */
    pArea->area_flags	= 0; /*AREA_ADDED;*/
    pArea->security	= 1;
    pArea->builders	= str_dup( "None" );
    pArea->lvnum	= 0;
    pArea->uvnum	= 0;
    pArea->llevel	= 0;
    pArea->ulevel	= 0;
    pArea->windstr	= 0;
    pArea->winddir	= 0;
    pArea->llevel	= 0;
    pArea->ulevel	= 0;
    pArea->def_color	= 6;
    pArea->vnum		= top_area-1;		/* OLC 1.1b */
    sprintf( buf, "area%d.are", pArea->vnum );
    pArea->filename	= str_dup( buf );

    return pArea;
}



/*****************************************************************************
 Name:		free_area
 Purpose:	Clears and deletes an area structure.
 ****************************************************************************/
void free_area( AREA_DATA *pArea )
{
    top_area--;
    free_string( pArea->name );
    free_string( pArea->filename );
    free_string( pArea->builders );
    free_string( pArea->reset_sound );
/*    pArea->next		=   area_free->next;
    area_free		=   pArea;*/
    free_mem( pArea, sizeof( *pArea ) );
    return;
}



EXIT_DATA *new_exit( void )
{
    EXIT_DATA *pExit;

/*    if ( !exit_free )
    {*/
	pExit           =   alloc_perm( sizeof(*pExit) );
	top_exit++;
/*    }
    else
    {
	pExit           =   exit_free;
	exit_free       =   exit_free->next;
    }*/

    pExit->to_room      =   NULL;
    pExit->next         =   NULL;
    pExit->traps	=   NULL;
    pExit->traptypes	=   0;
    pExit->rs_flags     =   0;
    pExit->vnum         =   0;
    pExit->exit_info    =   0;
    pExit->key          =   0;
    pExit->keyword      =   &str_empty[0];
    pExit->description  =   &str_empty[0];

    return pExit;
}

void free_trap_data( TRAP_DATA *pTrap )
{
  top_trap--;
  free_string( pTrap->arglist );
  free_string( pTrap->comlist );

  free_mem( pTrap, sizeof( *pTrap ) );
}

void free_exit( EXIT_DATA *pExit )
{

    TRAP_DATA	     *pTrap;
    TRAP_DATA	     *pTrap_next;

    top_exit--;

    free_string( pExit->keyword );
    free_string( pExit->description );

    for ( pTrap = pExit->traps; pTrap; pTrap = pTrap_next )
    {
       pTrap_next = pTrap->next;
       free_trap_data( pTrap );
    }

/*    pExit->next         =   exit_free;
    exit_free           =   pExit;*/
    free_mem( pExit, sizeof( *pExit ) );
    return;
}



EXTRA_DESCR_DATA *new_extra_descr( void )
{
    EXTRA_DESCR_DATA *pExtra;

/*    if ( !extra_descr_free )
    { */
	pExtra              =   alloc_perm( sizeof(*pExtra));
	top_ed++;
/*    }
    else
    {
	pExtra              =   extra_descr_free;
	extra_descr_free    =   extra_descr_free->next;
    }*/

    pExtra->next            =   NULL;
    pExtra->keyword         =   &str_empty[0];
    pExtra->description     =   &str_empty[0];
    pExtra->deleted	    =  	FALSE;

    return pExtra;
}



void free_extra_descr( EXTRA_DESCR_DATA *pExtra )
{
    free_string( pExtra->keyword );
    free_string( pExtra->description );

    top_ed--;
/*    pExtra->next        =   extra_descr_free;
    extra_descr_free    =   pExtra;*/
    free_mem( pExtra, sizeof( *pExtra ) );
    return;
}



ROOM_INDEX_DATA *new_room_index( void )
{
    ROOM_INDEX_DATA *pRoom;
    int door;

/*    if ( !room_index_free )
    {*/
	pRoom           =   alloc_perm( sizeof(*pRoom) );
	top_room++;
/*    }
    else
    {
	pRoom           =   room_index_free;
	room_index_free =   room_index_free->next;
    }*/

    pRoom->next             =   NULL;
    pRoom->people           =   NULL;
    pRoom->contents         =   NULL;
    pRoom->extra_descr      =   NULL;
    pRoom->area             =   NULL;

    for ( door=0; door < MAX_DIR; door++ )
	pRoom->exit[door]   =   NULL;

    pRoom->reset_first	    = 	NULL;
    pRoom->reset_last	    =   NULL;
    pRoom->traps	    =   NULL;
    pRoom->traptypes	    =   0;
    pRoom->name             =   &str_empty[0];
    pRoom->description      =   &str_empty[0];
    pRoom->vnum             =   0;
    pRoom->room_flags       =   0;
    pRoom->light            =   0;
    pRoom->sector_type      =   0;
    pRoom->rd		    =	0;

    return pRoom;
}

void free_social_index( SOCIAL_DATA *pSocial )
{
        free_string(pSocial->name);
        free_string(pSocial->char_no_arg);
        free_string(pSocial->others_no_arg);
        free_string(pSocial->char_found);
        free_string(pSocial->others_found);
        free_string(pSocial->vict_found);    
        free_string(pSocial->char_auto);
        free_string(pSocial->others_auto);    
	top_social--;

        free_mem( pSocial, sizeof( *pSocial ) );

  return;
}  

SOCIAL_DATA *new_social_index ( void )
{
    SOCIAL_DATA *pSocial;

	pSocial  =  alloc_perm( sizeof( *pSocial ));

	pSocial->next		   =       NULL;
	pSocial->name              =       &str_empty[0];
	pSocial->char_no_arg       =       &str_empty[0];
	pSocial->others_no_arg     =       &str_empty[0];
	pSocial->char_found        =       &str_empty[0];
	pSocial->others_found      =       &str_empty[0];
	pSocial->vict_found        =       &str_empty[0];
	pSocial->char_auto         =       &str_empty[0];
	pSocial->others_auto       =       &str_empty[0];
	top_social++;

 return pSocial;
}

NEWBIE_DATA *new_newbie_index( void )
{
  NEWBIE_DATA *pNewbie;

  pNewbie	= alloc_perm( sizeof(*pNewbie) );
  top_newbie++;
  pNewbie->next = NULL;
  pNewbie->keyword = &str_empty[0];
  pNewbie->answer1 = &str_empty[0];
  pNewbie->answer2 = &str_empty[0];

  return pNewbie;

}

CLAN_DATA *new_clan_index( void )
{
    CLAN_DATA *pClan;

/*    if ( !clan_index_free )
    {*/
	pClan           =   alloc_perm( sizeof(*pClan) );
	top_clan++;
/*    }
    else
    {
	pClan           =   clan_index_free;
	clan_index_free =   clan_index_free->next;
    }*/

    pClan->next             =   NULL;
    pClan->bankaccount.gold   = 0;
    pClan->bankaccount.silver = 0;
    pClan->bankaccount.copper = 0; 
    pClan->name             =   &str_empty[0];
    pClan->diety            =   &str_empty[0];
    pClan->description	    =   &str_empty[0];
    pClan->leader	    =   &str_empty[0];
    pClan->first	    =   &str_empty[0];
    pClan->second	    =   &str_empty[0];
    pClan->champ	    =   &str_empty[0];
    pClan->isleader	    =   FALSE;
    pClan->isfirst	    =	FALSE;
    pClan->issecond	    =   FALSE;
    pClan->ischamp	    =   FALSE;
    pClan->vnum             =   0;
    pClan->recall	    =   ROOM_VNUM_TEMPLE;
    pClan->members          =   0;
    pClan->pkills           =   0;
    pClan->mkills           =   0;
    pClan->mdeaths          =   0;
    pClan->pdeaths          =   0;
    pClan->settings         =   0;
    pClan->obj_vnum_1       =   OBJ_VNUM_DUMMY;
    pClan->obj_vnum_2       =   OBJ_VNUM_DUMMY;
    pClan->obj_vnum_3       =   OBJ_VNUM_DUMMY;
    return pClan;
}



void free_room_index( ROOM_INDEX_DATA *pRoom )
{
    int door;
    EXTRA_DESCR_DATA *pExtra;
    EXTRA_DESCR_DATA *pExtra_next;
    RESET_DATA 	     *pReset;
    RESET_DATA       *pReset_next;
    TRAP_DATA	     *pTrap;
    TRAP_DATA	     *pTrap_next;


    top_room--;
    free_string( pRoom->name );
    free_string( pRoom->description );

    for ( pTrap = pRoom->traps; pTrap; pTrap = pTrap_next )
    {
       pTrap_next = pTrap->next;
       free_trap_data( pTrap );
    }

    for ( door = 0; door < MAX_DIR; door++ )
    {
	if ( pRoom->exit[door] )
	    free_exit( pRoom->exit[door] );
    }

    for ( pExtra = pRoom->extra_descr; pExtra; pExtra = pExtra_next )
    {
	pExtra_next = pExtra->next;
	free_extra_descr( pExtra );
    }

    for ( pReset = pRoom->reset_first; pReset; pReset = pReset_next )
    {
	pReset_next = pReset->next;
	free_reset_data( pReset );
    }

/*    pRoom->next     =   room_index_free;
    room_index_free =   pRoom;*/
    free_mem( pRoom, sizeof( *pRoom ) );
    return;
}



AFFECT_DATA *new_affect( void )
{
    AFFECT_DATA *pAf;

/*    if ( !affect_free )
    {*/
	pAf             =   alloc_perm( sizeof(*pAf) );
	top_affect++;
/*    }
    else
    {
	pAf             =   affect_free;
	affect_free     =   affect_free->next;
    }*/

    pAf->next       =   NULL;
    pAf->type       =   0;
    pAf->level	    =   0;
    pAf->duration   =   0;
    pAf->location   =   0;
    pAf->modifier   =   0;
    pAf->bitvector  =   0;
    pAf->deleted    =   FALSE;

    return pAf;
}


ALIAS_DATA *new_alias( void )
{
  ALIAS_DATA *pAl;

  pAl = alloc_perm( sizeof( *pAl ) );

  pAl->next = NULL;
  pAl->old = &str_empty[0]; /*str_dup( "" );*/
  pAl->new = &str_empty[0]; /*str_dup( "" );*/

  return pAl;
}


void free_alias( ALIAS_DATA *pAl )
{
  free_string( pAl->old );
  free_string( pAl->new );

  free_mem( pAl, sizeof( *pAl ) );

  return;
}


void free_affect( AFFECT_DATA* pAf )
{
/*    pAf->next           = affect_free;
    affect_free         = pAf;*/
    top_affect--;
    free_mem( pAf, sizeof( *pAf ) );
    return;
}



SHOP_DATA *new_shop( void )
{
    SHOP_DATA *pShop;
    int buy;

/*    if ( !shop_free )
    {*/
	pShop           =   alloc_perm( sizeof(*pShop));
	top_shop++;
/*    }
    else
    {
	pShop           =   shop_free;
	shop_free       =   shop_free->next;
    }*/

    pShop->next         =   NULL;
    pShop->keeper       =   0;

    for ( buy=0; buy<MAX_TRADE; buy++ )
	pShop->buy_type[buy]    =   0;

    pShop->profit_buy   =   100;
    pShop->profit_sell  =   100;
    pShop->open_hour    =   0;
    pShop->close_hour   =   23;

    return pShop;
}



void free_shop( SHOP_DATA *pShop )
{
/*    pShop->next = shop_free;
    shop_free   = pShop;*/
    top_shop--;
    free_mem( pShop, sizeof( *pShop ) );
    return;
}

TRAP_DATA *new_trap_data( void )
{
  TRAP_DATA *pTrap;

/*  if ( !trap_free )
  {*/
    pTrap = alloc_perm( sizeof( *pTrap ) );
    top_trap++;
/*  }
  else
  {
    pTrap = trap_free;
    trap_free = trap_free->next;
  }*/

  pTrap->next = NULL;
  pTrap->next_here = NULL;
  pTrap->on_obj = NULL;
  pTrap->in_room = NULL;
  pTrap->on_exit = NULL;
  pTrap->disarmable = FALSE;
  pTrap->disarmed = FALSE;
  pTrap->disarm_dur = 0;
  pTrap->type = 1;
  pTrap->arglist = &str_empty[0]; /*str_dup( "" );*/
  pTrap->comlist = &str_empty[0]; /*str_dup( "" );*/
  return pTrap;
}

OBJ_INDEX_DATA *new_obj_index( void )
{
    OBJ_INDEX_DATA *pObj;
    int value;

/*    if ( !obj_index_free )
    {*/
	pObj           =   alloc_perm( sizeof(*pObj) );
	top_obj_index++;
/*    }
    else
    {
	pObj            =   obj_index_free;
	obj_index_free  =   obj_index_free->next;
    }*/

    pObj->next          =   NULL;
    pObj->extra_descr   =   NULL;
    pObj->affected      =   NULL;
    pObj->area          =   NULL;
    pObj->traps		=   NULL;
    pObj->traptypes	=   0;
    pObj->name          =   str_dup( "no name" );
    pObj->short_descr   =   str_dup( "(no short description)" );
    pObj->description   =   str_dup( "(no description)" );
    pObj->vnum          =   0;
    pObj->item_type     =   ITEM_TRASH;
    pObj->extra_flags   =   0;
    pObj->anti_race_flags = 0;
    pObj->anti_class_flags= 0;
    pObj->wear_flags    =   0;
    pObj->count         =   0;
    pObj->weight        =   0;
#ifdef NEW_MONEY
    pObj->cost.gold	=   0;
    pObj->cost.silver	=   0;
    pObj->cost.copper	=   0;
#else
    pObj->cost          =   0;
#endif
    pObj->level		=   0;
    for ( value=0; value<4; value++ )
	pObj->value[value]  =   0;
    pObj->ac_type	=   0;
    pObj->ac_vnum	=   0;
    pObj->ac_spell	=   &str_empty[0];
    pObj->ac_charge[0]	=   0;
    pObj->ac_charge[1]	=   0;
    pObj->join		=   0;
    pObj->sep_one	=   0;
    pObj->sep_two	=   0;

    return pObj;
}



void free_obj_index( OBJ_INDEX_DATA *pObj )
{
    EXTRA_DESCR_DATA *pExtra;
    EXTRA_DESCR_DATA *pExtra_next;
    AFFECT_DATA      *pAf;
    AFFECT_DATA      *pAf_next;
    TRAP_DATA	     *pTrap;
    TRAP_DATA	     *pTrap_next;

    top_obj_index--;

    free_string( pObj->name );
    free_string( pObj->short_descr );
    free_string( pObj->description );
    free_string( pObj->ac_spell );

    for ( pAf = pObj->affected; pAf; pAf = pAf_next )
    {
	pAf_next = pAf->next;
	free_affect( pAf );
    }

    for ( pExtra = pObj->extra_descr; pExtra; pExtra = pExtra_next )
    {
	pExtra_next = pExtra->next;
	free_extra_descr( pExtra );
    }

    for ( pTrap = pObj->traps; pTrap; pTrap = pTrap_next )
    {
       pTrap_next = pTrap->next;
       free_trap_data( pTrap );
    }

/*    pObj->next              = obj_index_free;
    obj_index_free          = pObj;*/
    free_mem( pObj, sizeof( *pObj ) );
    return;
}



MOB_INDEX_DATA *new_mob_index( void )
{
    MOB_INDEX_DATA *pMob;

/*    if ( !mob_index_free )
    {*/
	pMob           =   alloc_perm( sizeof(*pMob) );
	top_mob_index++;
/*    }
    else
    {
	pMob            =   mob_index_free;
	mob_index_free  =   mob_index_free->next;
    }*/

    pMob->next          =   NULL;
    pMob->spec_fun      =   NULL;
    pMob->pShop         =   NULL;
    pMob->area          =   NULL;
    pMob->mobprogs	=   NULL;
    pMob->player_name   =   str_dup( "no name" );
    pMob->short_descr   =   str_dup( "(no short description)" );
    pMob->long_descr    =   str_dup( "(no long description)\n\r" );
    pMob->description   =   &str_empty[0];
    pMob->vnum          =   0;
    pMob->progtypes	=   0;
    pMob->count         =   0;
    pMob->killed        =   0;
    pMob->sex           =   0;
    pMob->class		=   0;
    pMob->level         =   0;
    pMob->act           =   ACT_IS_NPC;
    pMob->affected_by   =   0;
    pMob->affected_by2  =   0;
    pMob->imm_flags	=   0;
    pMob->res_flags	=   0;
    pMob->vul_flags	=   0;
    pMob->alignment     =   0;
    pMob->hitroll	=   0;
    pMob->ac	        =   0;
    pMob->hitnodice	=   0;
    pMob->hitsizedice	=   0;
    pMob->hitplus	=   0;
    pMob->damnodice	=   0;
    pMob->damsizedice	=   0;
    pMob->damplus	=   0;
#ifdef NEW_MONEY
    pMob->money.gold	=   0;
    pMob->money.silver	=   0;
    pMob->money.copper	=   0;
#else
    pMob->gold          =   0;
#endif

    return pMob;
}

void free_mprog_data( MPROG_DATA *pMProg )
{
  free_string( pMProg->arglist );
  free_string( pMProg->comlist );

  top_mob_prog--;
/*  pMProg->next = mprog_free;
  mprog_free = pMProg;*/
  free_mem( pMProg, sizeof( *pMProg ) );
}


void free_mob_index( MOB_INDEX_DATA *pMob )
{
    MPROG_DATA	     *pMProg;
    MPROG_DATA	     *pMProg_next;
    SHOP_DATA	     *pShop;
    SHOP_DATA        *pShop_next;

    top_mob_index--;

    free_string( pMob->player_name );
    free_string( pMob->short_descr );
    free_string( pMob->long_descr );
    free_string( pMob->description );

    for ( pMProg = pMob->mobprogs; pMProg; pMProg = pMProg_next )
    {
       pMProg_next = pMProg->next;
       free_mprog_data( pMProg );
    }

    for ( pShop = pMob->pShop; pShop; pShop = pShop_next )
    {
       pShop_next = pShop->next;
       free_shop( pShop );
    }

/*    pMob->next              = mob_index_free;
    mob_index_free          = pMob;*/
    free_mem( pMob, sizeof( *pMob ) );
    return;
}

/* Altrag */
MPROG_DATA *new_mprog_data( void )
{
  MPROG_DATA *pMProg;

/*  if ( !mprog_free )
  {*/
    pMProg = alloc_perm( sizeof(*pMProg) );
    top_mob_prog++;
/*  }
  else
  {
    pMProg = mprog_free;
    mprog_free = mprog_free->next;
  }*/

  pMProg->next     = NULL;
  pMProg->type     = 1;
  pMProg->arglist  = &str_empty[0]; /*str_dup( "" );*/
  pMProg->comlist  = &str_empty[0]; /*str_dup( "" );*/
  return pMProg;
}


RACE_DATA *new_race_data( )
{
   RACE_DATA *pRace;

   top_race++;

   pRace             = alloc_perm( sizeof( RACE_DATA ));
   pRace->vnum       = 0;
   pRace->race_full  = NULL;
   pRace->race_name  = NULL;
   pRace->mstr       = 0;
   pRace->mint       = 0;
   pRace->mwis       = 0;
   pRace->mdex       = 0;
   pRace->mcon       = 0;

   return pRace;
}

void free_race_data( RACE_DATA *pRace )
{

   top_race--;

   free_string( pRace->race_name );
   free_string( pRace->race_full ); 

   free_mem( pRace, sizeof( *pRace ) );

}


