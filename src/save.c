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

#if !defined( macintosh )
extern	int	_filbuf		args( (FILE *) );
#endif

#if defined( sun )
int     system          args( ( const char *string ) );
#endif


/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[ MAX_NEST ];



/*
 * Local functions.
 */
void	fwrite_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void	fwrite_obj	args( ( CHAR_DATA *ch,  OBJ_DATA  *obj,
			       FILE *fp, int iNest ) );
void	fread_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void	fread_obj	args( ( CHAR_DATA *ch,  FILE *fp ) );


/* Courtesy of Yaz of 4th Realm */
char *initial( const char *str )
{
    static char strint [ MAX_STRING_LENGTH ];

    strint[0] = LOWER( str[ 0 ] );
    return strint;

}

/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj( CHAR_DATA *ch )
{
    FILE *fp;
    char  buf     [ MAX_STRING_LENGTH ];
    char  strsave [ MAX_INPUT_LENGTH  ];

    if ( IS_NPC( ch ) || ch->level < 2 )
	return;

    if ( ch->desc && ch->desc->original )
	ch = ch->desc->original;

    ch->save_time = current_time;
    fclose( fpReserve );

    /* player files parsed directories by Yaz 4th Realm */
#if !defined( macintosh ) && !defined( MSDOS )
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( ch->name ) );
#else
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
#endif
    if ( !( fp = fopen( strsave, "w" ) ) )
    {
        sprintf( buf, "Save_char_obj: fopen %s: ", ch->name );
	bug( buf, 0 );
	perror( strsave );
    }
    else
    {
	fwrite_char( ch, fp );
	if ( ch->carrying )
	    fwrite_obj( ch, ch->carrying, fp, 0 );
	fprintf( fp, "#END\n" );
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
    AFFECT_DATA *paf;
    int          sn;

    fprintf( fp, "#%s\n", IS_NPC( ch ) ? "MOB" : "PLAYER"	);

    fprintf( fp, "Nm          %s~\n",	ch->name		);
    fprintf( fp, "ShtDsc      %s~\n",	ch->short_descr		);
    fprintf( fp, "LngDsc      %s~\n",	ch->long_descr		);
    fprintf( fp, "Dscr        %s~\n",	ch->description		);
    fprintf( fp, "Prmpt       %s~\n",	ch->pcdata->prompt	);
    fprintf( fp, "Sx          %d\n",	ch->sex			);
    fprintf( fp, "Cla         %d\n",	ch->class		);

    fprintf( fp, "Race        %s~\n",	race_table[ ch->race ].name );

    fprintf( fp, "Lvl         %d\n",	ch->level		);
    fprintf( fp, "Trst        %d\n",	ch->trust		);
    fprintf( fp, "Playd       %d\n",
	ch->played + (int) ( current_time - ch->logon )		);
    fprintf( fp, "Note        %ld\n",   (unsigned long)ch->last_note );
    fprintf( fp, "Room        %d\n",
	    (  ch->in_room == get_room_index( ROOM_VNUM_LIMBO )
	     && ch->was_in_room )
	    ? ch->was_in_room->vnum
	    : ch->in_room->vnum );

    fprintf( fp, "HpMnMv      %d %d %d %d %d %d\n",
	ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );
    fprintf( fp, "Gold        %d\n",	ch->gold		);
    fprintf( fp, "Exp         %d\n",	ch->exp			);
    fprintf( fp, "Act         %d\n",    ch->act			);
    fprintf( fp, "AffdBy      %d\n",	ch->affected_by		);
    /* Bug fix from Alander */
    fprintf( fp, "Pos         %d\n",
	    ch->position == POS_FIGHTING ? POS_STANDING : ch->position );

    fprintf( fp, "Prac        %d\n",	ch->practice		);
    fprintf( fp, "SavThr      %d\n",	ch->saving_throw	);
    fprintf( fp, "Align       %d\n",	ch->alignment		);
    fprintf( fp, "Hit         %d\n",	ch->hitroll		);
    fprintf( fp, "Dam         %d\n",	ch->damroll		);
    fprintf( fp, "Armr        %d\n",	ch->armor		);
    fprintf( fp, "Wimp        %d\n",	ch->wimpy		);
    fprintf( fp, "Deaf        %d\n",	ch->deaf		);

    if ( IS_NPC( ch ) )
    {
	fprintf( fp, "Vnum        %d\n",	ch->pIndexData->vnum	);
    }
    else
    {
	fprintf( fp, "Paswd       %s~\n",	ch->pcdata->pwd		);
	fprintf( fp, "Bmfin       %s~\n",	ch->pcdata->bamfin	);
	fprintf( fp, "Bmfout      %s~\n",	ch->pcdata->bamfout	);
	fprintf( fp, "Immskll     %s~\n",	ch->pcdata->immskll	);
	fprintf( fp, "Ttle        %s~\n",	ch->pcdata->title	);
	fprintf( fp, "AtrPrm      %d %d %d %d %d\n",
		ch->pcdata->perm_str,
		ch->pcdata->perm_int,
		ch->pcdata->perm_wis,
		ch->pcdata->perm_dex,
		ch->pcdata->perm_con );

	fprintf( fp, "AtrMd       %d %d %d %d %d\n",
		ch->pcdata->mod_str, 
		ch->pcdata->mod_int, 
		ch->pcdata->mod_wis,
		ch->pcdata->mod_dex, 
		ch->pcdata->mod_con );

	fprintf( fp, "Cond        %d %d %d\n",
		ch->pcdata->condition[0],
		ch->pcdata->condition[1],
		ch->pcdata->condition[2] );

	fprintf( fp, "Pglen       %d\n",   ch->pcdata->pagelen     );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name && ch->pcdata->learned[sn] > 0 )
	    {
		fprintf( fp, "Skll        %d '%s'\n",
		    ch->pcdata->learned[sn], skill_table[sn].name );
	    }
	}
    }

    for ( paf = ch->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;

	fprintf( fp, "Afft       %18s~ %3d %3d %3d %10d\n",
		skill_table[ paf->type ].name,
		paf->duration,
		paf->modifier,
		paf->location,
		paf->bitvector );
    }

    fprintf( fp, "End\n\n" );
    return;
}



/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest )
{
    AFFECT_DATA      *paf;
    EXTRA_DESCR_DATA *ed;

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if ( obj->next_content )
	fwrite_obj( ch, obj->next_content, fp, iNest );

    /*
     * Castrate storage characters.
     */
    if ( ch->level < obj->level
	|| obj->item_type == ITEM_KEY
	|| obj->deleted )
	return;

    fprintf( fp, "#OBJECT\n" );
    fprintf( fp, "Nest         %d\n",	iNest			     );
    fprintf( fp, "Name         %s~\n",	obj->name		     );
    fprintf( fp, "ShortDescr   %s~\n",	obj->short_descr	     );
    fprintf( fp, "Description  %s~\n",	obj->description	     );
    fprintf( fp, "Vnum         %d\n",	obj->pIndexData->vnum	     );
    fprintf( fp, "ExtraFlags   %d\n",	obj->extra_flags	     );
    fprintf( fp, "WearFlags    %d\n",	obj->wear_flags		     );
    fprintf( fp, "WearLoc      %d\n",	obj->wear_loc		     );
    fprintf( fp, "ItemType     %d\n",	obj->item_type		     );
    fprintf( fp, "Weight       %d\n",	obj->weight		     );
    fprintf( fp, "Level        %d\n",	obj->level		     );
    fprintf( fp, "Timer        %d\n",	obj->timer		     );
    fprintf( fp, "Cost         %d\n",	obj->cost		     );
    fprintf( fp, "Values       %d %d %d %d\n",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3]   );

    switch ( obj->item_type )
    {
    case ITEM_POTION:
    case ITEM_SCROLL:
	if ( obj->value[1] > 0 )
	{
	    fprintf( fp, "Spell 1      '%s'\n", 
		skill_table[obj->value[1]].name );
	}

	if ( obj->value[2] > 0 )
	{
	    fprintf( fp, "Spell 2      '%s'\n", 
		skill_table[obj->value[2]].name );
	}

	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3      '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;

    case ITEM_PILL:
    case ITEM_STAFF:
    case ITEM_WAND:
	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3      '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
	fprintf( fp, "Affect       %d %d %d %d %d\n",
		paf->type,
		paf->duration,
		paf->modifier,
		paf->location,
		paf->bitvector );
    }

    for ( ed = obj->extra_descr; ed; ed = ed->next )
    {
	fprintf( fp, "ExtraDescr   %s~ %s~\n",
		ed->keyword, ed->description );
    }

    fprintf( fp, "End\n\n" );

    if ( obj->contains )
	fwrite_obj( ch, obj->contains, fp, iNest + 1 );

    tail_chain();
    return;
}



/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj( DESCRIPTOR_DATA *d, char *name )
{
           FILE      *fp;
    static PC_DATA    pcdata_zero;
	   CHAR_DATA *ch;
#if !defined( MSDOS )
	   char       buf     [ MAX_STRING_LENGTH ];
#endif
	   char       strsave [ MAX_INPUT_LENGTH ];
	   bool       found;

    if ( !char_free )
    {
	ch				= alloc_perm( sizeof( *ch ) );
    }
    else
    {
	ch				= char_free;
	char_free			= char_free->next;
    }
    clear_char( ch );

    if ( !pcdata_free )
    {
	ch->pcdata			= alloc_perm( sizeof( *ch->pcdata ) );
    }
    else
    {
	ch->pcdata			= pcdata_free;
	pcdata_free			= pcdata_free->next;
    }
    *ch->pcdata				= pcdata_zero;

    d->character			= ch;
    ch->desc				= d;
    ch->name				= str_dup( name );
    ch->pcdata->prompt                  = str_dup( "<%hhp %mm %vmv> " );
    ch->last_note                       = 0;
    ch->act				= PLR_BLANK
					| PLR_COMBINE
					| PLR_PROMPT;
    ch->pcdata->pwd			= str_dup( "" );
    ch->pcdata->bamfin			= str_dup( "" );
    ch->pcdata->bamfout			= str_dup( "" );
    ch->pcdata->immskll			= str_dup( "" );
    ch->pcdata->title			= str_dup( "" );
    ch->pcdata->perm_str		= 13;
    ch->pcdata->perm_int		= 13; 
    ch->pcdata->perm_wis		= 13;
    ch->pcdata->perm_dex		= 13;
    ch->pcdata->perm_con		= 13;
    ch->pcdata->condition[COND_THIRST]	= 48;
    ch->pcdata->condition[COND_FULL]	= 48;
    ch->pcdata->pagelen                 = 20;

    ch->pcdata->switched                = FALSE;

    found = FALSE;
    fclose( fpReserve );

    /* parsed player file directories by Yaz of 4th Realm */
    /* decompress if .gz file exists - Thx Alander */
#if !defined( macintosh ) && !defined( MSDOS )
    sprintf( strsave, "%s%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( name ), ".gz" );
    if ( ( fp = fopen( strsave, "r" ) ) )
    {
	fclose( fp );
	sprintf( buf, "gzip -dfq %s", strsave );
	system( buf );
    }
#endif

#if !defined( macintosh ) && !defined( MSDOS )
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( name ) );
#else
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );
#endif
    if ( ( fp = fopen( strsave, "r" ) ) )
    {
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	found = TRUE;
	for ( ; ; )
	{
	    char  letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_char_obj: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	         if ( !str_cmp( word, "PLAYER" ) ) fread_char ( ch, fp );
	    else if ( !str_cmp( word, "OBJECT" ) ) fread_obj  ( ch, fp );
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		break;
	    }
	}
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return found;
}



/*
 * Read in a char.
 */

#if defined( KEY )
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

void fread_char( CHAR_DATA *ch, FILE *fp )
{
    char *word;
    char *race;
    char  buf [ MAX_STRING_LENGTH ];
    bool  fMatch;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER( word[0] ) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Act",		ch->act,		fread_number( fp ) );
	    KEY( "AffdBy",	ch->affected_by,	fread_number( fp ) );
	    KEY( "Align",	ch->alignment,		fread_number( fp ) );
	    KEY( "Armr",	ch->armor,		fread_number( fp ) );

	    if ( !str_prefix( word, "Afft" ) )
	    {
		AFFECT_DATA *paf;

		if ( !affect_free )
		{
		    paf		= alloc_perm( sizeof( *paf ) );
		}
		else
		{
		    paf		= affect_free;
		    affect_free	= affect_free->next;
		}


		if ( !str_cmp( word, "Aff" ) )
		    paf->type	= fread_number( fp );
		else
		    paf->type   = affect_lookup( fread_string( fp ) );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		paf->deleted    = FALSE;
		paf->next	= ch->affected;
		ch->affected	= paf;
		fMatch = TRUE;

	    }

	    if ( !str_cmp( word, "AtrMd"  ) )
	    {
		ch->pcdata->mod_str  = fread_number( fp );
		ch->pcdata->mod_int  = fread_number( fp );
		ch->pcdata->mod_wis  = fread_number( fp );
		ch->pcdata->mod_dex  = fread_number( fp );
		ch->pcdata->mod_con  = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "AtrPrm" ) )
	    {
		ch->pcdata->perm_str = fread_number( fp );
		ch->pcdata->perm_int = fread_number( fp );
		ch->pcdata->perm_wis = fread_number( fp );
		ch->pcdata->perm_dex = fread_number( fp );
		ch->pcdata->perm_con = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'B':
	    KEY( "Bmfin",	ch->pcdata->bamfin,	fread_string( fp ) );
	    KEY( "Bmfout",	ch->pcdata->bamfout,	fread_string( fp ) );
	    break;

	case 'C':
	    KEY( "Cla", 	ch->class,		fread_number( fp ) );

	    if ( !str_cmp( word, "Cond" ) )
	    {
		ch->pcdata->condition[0] = fread_number( fp );
		ch->pcdata->condition[1] = fread_number( fp );
		ch->pcdata->condition[2] = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'D':
	    KEY( "Dam", 	ch->damroll,		fread_number( fp ) );
	    KEY( "Deaf",	ch->deaf,		fread_number( fp ) );
	    KEY( "Dscr",	ch->description,	fread_string( fp ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
		return;
	    KEY( "Exp",		ch->exp,		fread_number( fp ) );
	    break;

	case 'G':
	    KEY( "Gold",	ch->gold,		fread_number( fp ) );
	    break;

	case 'H':
	    KEY( "Hit", 	ch->hitroll,		fread_number( fp ) );

	    if ( !str_cmp( word, "HpMnMv" ) )
	    {
		ch->hit		= fread_number( fp );
		ch->max_hit	= fread_number( fp );
		ch->mana	= fread_number( fp );
		ch->max_mana	= fread_number( fp );
		ch->move	= fread_number( fp );
		ch->max_move	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'I':
	    KEY ( "Immskll",    ch->pcdata->immskll,    fread_string( fp ) );

	case 'L':
	    KEY( "Lvl", 	ch->level,		fread_number( fp ) );
	    KEY( "LngDsc",	ch->long_descr,		fread_string( fp ) );
	    break;

	case 'N':
	    if ( !str_cmp( word, "Nm" ) )
	    {
		/*
		 * Name already set externally.
		 */
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }
	    KEY( "Note",        ch->last_note,          fread_number( fp ) );
	    break;

	case 'P':
	    KEY( "Pglen",       ch->pcdata->pagelen,    fread_number( fp ) );
	    KEY( "Paswd",	ch->pcdata->pwd,	fread_string( fp ) );
	    KEY( "Playd",	ch->played,		fread_number( fp ) );
	    KEY( "Pos", 	ch->position,		fread_number( fp ) );
	    KEY( "Prac",	ch->practice,		fread_number( fp ) );
	    KEY( "Prmpt",	ch->pcdata->prompt,	fread_string( fp ) );
	    break;

	case 'R':
	    /*
	     * This line is left to maintain compatibility stock Envy 1.0
	     */
	    KEY( "Rce",         ch->race,               fread_number( fp ) );

	    if ( !str_cmp( word, "Race" ) )
	    {
		race     = fread_string( fp );
		ch->race = race_lookup( race );
		fMatch   = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Room" ) )
	    {
		ch->in_room = get_room_index( fread_number( fp ) );
		if ( !ch->in_room )
		    ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
		fMatch = TRUE;
		break;
	    }
	      
	    break;

	case 'S':
	    KEY( "SavThr",	ch->saving_throw,	fread_number( fp ) );
	    KEY( "Sx",		ch->sex,		fread_number( fp ) );
	    KEY( "ShtDsc",	ch->short_descr,	fread_string( fp ) );

	    if ( !str_cmp( word, "Skll" ) )
	    {
		int sn;
		int value;

		value = fread_number( fp );
		sn    = skill_lookup( fread_word( fp ) );
		if ( sn < 0 )
		    bug( "Fread_char: unknown skill.", 0 );
		else
		    ch->pcdata->learned[sn] = value;
		fMatch = TRUE;
	    }

	    break;

	case 'T':
	    KEY( "Trst",	ch->trust,		fread_number( fp ) );

	    if ( !str_cmp( word, "Ttle" ) )
	    {
		ch->pcdata->title = fread_string( fp );
		if (   isalpha( ch->pcdata->title[0] )
		    || isdigit( ch->pcdata->title[0] ) )
		{
		    sprintf( buf, " %s", ch->pcdata->title );
		    free_string( ch->pcdata->title );
		    ch->pcdata->title = str_dup( buf );
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'V':
	    if ( !str_cmp( word, "Vnum" ) )
	    {
		ch->pIndexData = get_mob_index( fread_number( fp ) );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "Wimp",	ch->wimpy,		fread_number( fp ) );

	    if ( !str_cmp( word, "Wizbt" ) )
	    {
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }
	    break;
	}

	/* Make sure old chars have this field - Kahn */
	if ( !ch->pcdata->pagelen )
	    ch->pcdata->pagelen = 20;
	if ( !ch->pcdata->prompt || ch->pcdata->prompt == '\0' )
	    ch->pcdata->prompt = str_dup ( "<%hhp %mm %vmv> " );

	/* Make sure old chars do not have pagelen > 60 - Kahn */
	if ( ch->pcdata->pagelen > 60 )
	    ch->pcdata->pagelen = 60;
	if ( ch->pcdata->pagelen < 19 )
	    ch->pcdata->pagelen = 19;

	if ( !fMatch )
	{
	    bug( "Fread_char: no match.", 0 );
	    fread_to_eol( fp );
	}
    }
}



void fread_obj( CHAR_DATA *ch, FILE *fp )
{
    static OBJ_DATA  obj_zero;
           OBJ_DATA *obj;
           char     *word;
           int       iNest;
           bool      fMatch;
           bool      fNest;
           bool      fVnum;

    if ( !obj_free )
    {
	obj		= alloc_perm( sizeof( *obj ) );
    }
    else
    {
	obj		= obj_free;
	obj_free	= obj_free->next;
    }

    *obj		= obj_zero;
    obj->name		= str_dup( "" );
    obj->short_descr	= str_dup( "" );
    obj->description	= str_dup( "" );
    obj->deleted        = FALSE;

    fNest		= FALSE;
    fVnum		= TRUE;
    iNest		= 0;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER( word[0] ) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    if ( !str_cmp( word, "Affect" ) )
	    {
		AFFECT_DATA *paf;

		if ( !affect_free )
		{
		    paf		= alloc_perm( sizeof( *paf ) );
		}
		else
		{
		    paf		= affect_free;
		    affect_free	= affect_free->next;
		}

		paf->type	= fread_number( fp );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		paf->next	= obj->affected;
		obj->affected	= paf;
		fMatch		= TRUE;
		break;
	    }
	    break;

	case 'C':
	    KEY( "Cost",	obj->cost,		fread_number( fp ) );
	    break;

	case 'D':
	    KEY( "Description",	obj->description,	fread_string( fp ) );
	    break;

	case 'E':
	    KEY( "ExtraFlags",	obj->extra_flags,	fread_number( fp ) );

	    if ( !str_cmp( word, "ExtraDescr" ) )
	    {
		EXTRA_DESCR_DATA *ed;

		if ( !extra_descr_free )
		{
		    ed			= alloc_perm( sizeof( *ed ) );
		}
		else
		{
		    ed			= extra_descr_free;
		    extra_descr_free	= extra_descr_free->next;
		}

		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= obj->extra_descr;
		obj->extra_descr	= ed;
		fMatch = TRUE;
	    }

	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !fNest || !fVnum )
		{
		    bug( "Fread_obj: incomplete object.", 0 );
		    free_string( obj->name        );
		    free_string( obj->description );
		    free_string( obj->short_descr );
		    obj->next = obj_free;
		    obj_free  = obj;
		    return;
		}
		else
		{
		    obj->next	= object_list;
		    object_list	= obj;
		    obj->pIndexData->count++;
		    if ( iNest == 0 || !rgObjNest[iNest] )
			obj_to_char( obj, ch );
		    else
			obj_to_obj( obj, rgObjNest[iNest-1] );
		    return;
		}
	    }
	    break;

	case 'I':
	    KEY( "ItemType",	obj->item_type,		fread_number( fp ) );
	    break;

	case 'L':
	    KEY( "Level",	obj->level,		fread_number( fp ) );
	    break;

	case 'N':
	    KEY( "Name",	obj->name,		fread_string( fp ) );

	    if ( !str_cmp( word, "Nest" ) )
	    {
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		{
		    bug( "Fread_obj: bad nest %d.", iNest );
		}
		else
		{
		    rgObjNest[iNest] = obj;
		    fNest = TRUE;
		}
		fMatch = TRUE;
	    }
	    break;

	case 'S':
	    KEY( "ShortDescr",	obj->short_descr,	fread_string( fp ) );

	    if ( !str_cmp( word, "Spell" ) )
	    {
		int iValue;
		int sn;

		iValue = fread_number( fp );
		sn     = skill_lookup( fread_word( fp ) );
		if ( iValue < 0 || iValue > 3 )
		{
		    bug( "Fread_obj: bad iValue %d.", iValue );
		}
		else if ( sn < 0 )
		{
		    bug( "Fread_obj: unknown skill.", 0 );
		}
		else
		{
		    obj->value[iValue] = sn;
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'T':
	    KEY( "Timer",	obj->timer,		fread_number( fp ) );
	    break;

	case 'V':
	    if ( !str_cmp( word, "Values" ) )
	    {
		obj->value[0]	= fread_number( fp );
		obj->value[1]	= fread_number( fp );
		obj->value[2]	= fread_number( fp );
		obj->value[3]	= fread_number( fp );
		fMatch		= TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Vnum" ) )
	    {
		int vnum;

		vnum = fread_number( fp );
		if ( !( obj->pIndexData = get_obj_index( vnum ) ) )
		    bug( "Fread_obj: bad vnum %d.", vnum );
		else
		    fVnum = TRUE;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "WearFlags",	obj->wear_flags,	fread_number( fp ) );
	    KEY( "WearLoc",	obj->wear_loc,		fread_number( fp ) );
	    KEY( "Weight",	obj->weight,		fread_number( fp ) );
	    break;

	}

	if ( !fMatch )
	{
	    bug( "Fread_obj: no match.", 0 );
	    fread_to_eol( fp );
	}
    }
}
