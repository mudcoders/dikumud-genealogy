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

#if defined( sun )
#include <memory.h>
#endif

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


int stat;

/*
 * Local functions.
 */
void	fwrite_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void	fwrite_obj	args( ( CHAR_DATA *ch,  OBJ_DATA  *obj,
			       FILE *fp, int iNest ) );
int	fread_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
int	fread_obj	args( ( CHAR_DATA *ch,  FILE *fp ) );


/* Courtesy of Yaz of 4th Realm */
char *initial( const char *str )
{
    static char strint [ MAX_STRING_LENGTH ];

    strint[0] = LOWER( str[ 0 ] );
    return strint;

}

/*
 * Delete a character's file.
 * Used for retire command for now. Maybe i will make an imm command.
 * Courtesy of Zen :)
 */
 
void delete_char_obj( CHAR_DATA *ch )
{
    char  buf     [ MAX_STRING_LENGTH ];
    char  strremove [ MAX_INPUT_LENGTH  ];

    if ( IS_NPC( ch ) || ch->level < 2 )
	return;

    /* player files parsed directories by Yaz 4th Realm */
#if !defined( macintosh ) && !defined( WIN32 )
    sprintf( strremove, "%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( ch->name ) );
#else
    sprintf( strremove, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
#endif
    if ( remove( strremove ) )
    {
        sprintf( buf, "Delete_char_obj: remove %s: ", ch->name );
	bug( buf, 0 );
	perror( strremove );
    }
    return;
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
#if !defined( macintosh ) && !defined( WIN32 )
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
    int		 pos;

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

	fprintf( fp, "Security    %d\n",   ch->pcdata->security     );
	fprintf( fp, "Clan        %d %d\n",   ch->pcdata->rank,
						ch->pcdata->clan);
	fprintf( fp, "Pglen       %d\n",   ch->pcdata->pagelen     );

        for ( pos = 0; pos < MAX_ALIAS; pos++ )
	{
	    if ( !ch->pcdata->alias[pos]
		||  !ch->pcdata->alias_sub[pos] )
		break;

	    fprintf( fp, "Alias       %d '%s' %s~\n", pos,
	            ch->pcdata->alias[pos], ch->pcdata->alias_sub[pos] );
	}

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
    fprintf( fp, "Values       %d %d %d %d %d\n",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3],
							obj->value[4]   );

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
    extern char      *daPrompt;
           FILE      *fp;
	   CHAR_DATA *ch;
	   char       strsave [ MAX_INPUT_LENGTH ];
	   bool       found;
           char       sorry_player [] =
	     "********************************************************\n\r"
	     "** One or more of the critical fields in your player  **\n\r"
	     "** file were corrupted since you last played.  Please **\n\r"
	     "** contact an administrator or programmer to          **\n\r"
	     "** investigate the recovery of your characters.       **\n\r"
	     "********************************************************\n\r";
           char       sorry_object [] =
	     "********************************************************\n\r"
	     "** One or more of the critical fields in your player  **\n\r"
	     "** file were corrupted leading to the loss of one or  **\n\r"
	     "** more of your possessions.                          **\n\r"
	     "********************************************************\n\r";


    ch					= new_character( TRUE );

    d->character			= ch;
    ch->desc				= d;
    ch->name				= str_dup( name );
    ch->pcdata->prompt                  = str_dup( daPrompt );
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
    ch->pcdata->security		= 0;    /* OLC */
    ch->pcdata->rank			= 0;
    ch->pcdata->clan	                = 0;

    ch->pcdata->switched                = FALSE;

    found = FALSE;
    fclose( fpReserve );

    /* parsed player file directories by Yaz of 4th Realm */
    /* decompress if .gz file exists - Thx Alander */
#if !defined( macintosh ) && !defined( WIN32 )
    sprintf( strsave, "%s%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( name ), ".gz" );
    if ( ( fp = fopen( strsave, "r" ) ) )
    {
        char       buf     [ MAX_STRING_LENGTH ];

	fclose( fp );
	sprintf( buf, "gzip -dfq %s", strsave );
	system( buf );
    }
#endif

#if !defined( macintosh ) && !defined( WIN32 )
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( ch->name ),
	    "/", capitalize( name ) );
#else
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );
#endif
    if ( ( fp = fopen( strsave, "r" ) ) )
    {
	char buf[ MAX_STRING_LENGTH ];
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	found = TRUE;
	for ( ; ; )
	{
	    char *word;
	    char  letter;
	    int   status;

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

	    word = fread_word( fp, &status );

	    if ( !str_cmp( word, "PLAYER" ) )
	    {
	        if ( fread_char ( ch, fp ) )
		{
		    sprintf( buf,
			    "Load_char_obj:  %s section PLAYER corrupt.\n\r",
			    name );
		    bug( buf, 0 );
		    write_to_buffer( d, sorry_player, 0 );

		    /* 
		     * In case you are curious,
		     * it is ok to leave ch alone for close_socket
		     * to free.
		     * We want to now kick the bad character out as
		     * what we are missing are MANDATORY fields.  -Kahn
		     */
		    SET_BIT( ch->act, PLR_DENY );
		    return TRUE;
		}
	    }
	    else if ( !str_cmp( word, "OBJECT" ) )
	    {
	        if ( !fread_obj  ( ch, fp ) )
		{
		    sprintf( buf,
			    "Load_char_obj:  %s section OBJECT corrupt.\n\r",
			    name );
		    bug( buf, 0 );
		    write_to_buffer( d, sorry_object, 0 );
		    return FALSE;
		}
	    }
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		break;
	    }
	} /* for */

	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return found;
}



/*
 * Read in a char.
 */

int fread_char( CHAR_DATA *ch, FILE *fp )
{
    char        *word;
    char        buf [ MAX_STRING_LENGTH ];
    AFFECT_DATA *paf;
    int         sn;
    int         i;
    int         j;
    int         error_count = 0;
    int         status;
    int         status1;
    char        *p;
    int         tmpi;
    int         num_keys;
    int         last_key = 0;

    char        def_prompt [] = "{c<%hhp %mm %vmv>{x ";
    char        def_sdesc  [] = "Your short description was corrupted.";
    char        def_ldesc  [] = "Your long description was corrupted.";
    char        def_desc   [] = "Your description was corrupted.";
    char        def_title  [] = "Your title was corrupted.";

    struct key_data key_tab [] = {
      { "ShtDsc", TRUE,  (int) &def_sdesc,	{ &ch->short_descr,   NULL } },
      { "LngDsc", TRUE,  (int) &def_ldesc,	{ &ch->long_descr,    NULL } },
      { "Dscr",   TRUE,  (int) &def_desc,	{ &ch->description,   NULL } },
      { "Prmpt",  TRUE,  (int) &def_prompt,	{ &ch->pcdata->prompt,NULL } },
      { "Sx",     FALSE, SEX_MALE,		{ &ch->sex,           NULL } },
      { "Cla",    FALSE, MAND,			{ &ch->class,         NULL } },
      { "Lvl",    FALSE, MAND,			{ &ch->level,         NULL } },
      { "Trst",   FALSE, 0,			{ &ch->trust,         NULL } },
      { "Playd",  FALSE, 0,			{ &ch->played,        NULL } },
      { "Note",   FALSE, 0,			{ &ch->last_note,     NULL } },
      { "HpMnMv", FALSE, MAND,			{ &ch->hit,
						  &ch->max_hit,
						  &ch->mana,
						  &ch->max_mana,
						  &ch->move,
						  &ch->max_move,      NULL } },
      { "Gold",   FALSE, 0,			{ &ch->gold,          NULL } },
      { "Exp",    FALSE, MAND,			{ &ch->exp,           NULL } },
      { "Act",    FALSE, DEFLT,			{ &ch->act,           NULL } },
      { "AffdBy", FALSE, 0,			{ &ch->affected_by,   NULL } },
      { "Pos",    FALSE, POS_STANDING, 		{ &ch->position,      NULL } },
      { "Prac",   FALSE, MAND,			{ &ch->practice,      NULL } },
      { "SavThr", FALSE, MAND,			{ &ch->saving_throw,  NULL } },
      { "Align",  FALSE, 0,			{ &ch->alignment,     NULL } },
      { "Hit",    FALSE, MAND,			{ &ch->hitroll,       NULL } },
      { "Dam",    FALSE, MAND,			{ &ch->damroll,       NULL } },
      { "Armr",   FALSE, MAND,			{ &ch->armor,         NULL } },
      { "Wimp",   FALSE, 10,			{ &ch->wimpy,         NULL } },
      { "Deaf",   FALSE, 0,			{ &ch->deaf,          NULL } },
      { "Paswd",  TRUE,  MAND,			{ &ch->pcdata->pwd,   NULL } },
      { "Bmfin",  TRUE,  DEFLT,			{ &ch->pcdata->bamfin,
						                      NULL } },
      { "Bmfout", TRUE,  DEFLT,			{ &ch->pcdata->bamfout,
						                      NULL } },
      { "Immskll",TRUE,  DEFLT,			{ &ch->pcdata->immskll,
						                      NULL } },
      { "Ttle",   TRUE,  (int) &def_title,	{ &ch->pcdata->title, NULL } },
      { "AtrPrm", FALSE, MAND,			{ &ch->pcdata->perm_str,
						  &ch->pcdata->perm_int,
						  &ch->pcdata->perm_wis,
						  &ch->pcdata->perm_dex,
						  &ch->pcdata->perm_con,
						                      NULL } },
      { "AtrMd",  FALSE, MAND,			{ &ch->pcdata->mod_str,
						  &ch->pcdata->mod_int,
						  &ch->pcdata->mod_wis,
						  &ch->pcdata->mod_dex,
						  &ch->pcdata->mod_con,
						                      NULL } },
      { "Cond",   FALSE, DEFLT,			{ &ch->pcdata->condition [0],
						  &ch->pcdata->condition [1],
						  &ch->pcdata->condition [2],
						                      NULL } },
      { "Security",   FALSE, DEFLT,             { &ch->pcdata->security,
								      NULL } },
      { "Clan",   FALSE, DEFLT,			{ &ch->pcdata->rank,
						  &ch->pcdata->clan,
						                      NULL } },
      { "Pglen",  FALSE, 20,			{ &ch->pcdata->pagelen,
						                      NULL } },
      { "\0",     FALSE, 0                                                 } };

    for ( num_keys = 0; *key_tab [num_keys].key; )
        num_keys++;

    for ( ; !feof (fp) ; )
    {

        word = fread_word( fp, &status );

        if ( !word )
	{
            bug( "fread_char:  Error reading key.  EOF?", 0 );
            fread_to_eol( fp );
            break;
	}

                /* This little diddy searches for the keyword
                   from the last keyword found */

        for ( i = last_key;
              i < last_key + num_keys &&
                str_cmp (key_tab [i % num_keys].key, word); )
            i++;

        i = i % num_keys;

        if ( !str_cmp (key_tab [i].key, word) )
            last_key = i;
        else
            i = num_keys;

        if ( *key_tab [i].key )         /* Key entry found in key_tab */
	{
            if ( key_tab [i].string == SPECIFIED )
                bug ("Key already specified.", 0);

                        /* Entry is a string */

            else
	      if ( key_tab [i].string )
	      {
                  if ( ( p = fread_string( fp, &status ) ) && !status )
		  {
		      free_string ( *(char **)key_tab [i].ptrs [0] );
		      *(char **)key_tab [i].ptrs [0] = p;
		  }
	      }

                        /* Entry is an integer */
            else
                for ( j = 0; key_tab [i].ptrs [j]; j++ )
		{
                    tmpi = fread_number ( fp, &status );
                    if ( !status )
                        *(int *)key_tab [i].ptrs [j] = tmpi;
		}

            if ( status )
	    {
                fread_to_eol( fp );
                continue;
	    }
	    else
                key_tab [i].string = SPECIFIED;
	}

        else if ( *word == '*' || !str_cmp( word, "Nm" ) )
            fread_to_eol( fp );

        else if ( !str_cmp( word, "End" ) )
            break;

        else if ( !str_cmp( word, "Room" ) )
	  {
	      ch->in_room = get_room_index( fread_number( fp, &status ) );
	      if ( !ch->in_room )
                  ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
	  }

	else if ( !str_cmp( word, "Race" ) )
	  {
	      i  = race_lookup( fread_string( fp, &status ) );

	      if ( status )
		  bug( "Fread_char: Unknown Race.", 0 );
	      else
		  ch->race = i;
	  }

	else if ( !str_cmp( word, "Alias" ) )
	  {

	      i  = fread_number( fp, &status );

	      if ( status  )
	      {
		  bug( "Fread_char: Error reading alias.", 0 );
		  fread_to_eol( fp );
		  continue;
	      }

	      if ( i >= MAX_ALIAS )
	      {
		  bug( "Fread_char: too many aliases.", 0 );
		  fread_to_eol( fp );
		  continue;
	      }

	      ch->pcdata->alias[i] = str_dup( fread_word( fp, &status1 ) );
	      ch->pcdata->alias_sub[i] = fread_string( fp, &status );
	  }

        else if ( !str_cmp( word, "Skll" ) )
	  {
              i  = fread_number( fp, &status );
	      sn = skill_lookup( fread_word( fp, &status1 ) );
	      
	      if ( status || status1 )
	      {
		  bug( "Fread_char: Error reading skill.", 0 );
		  fread_to_eol( fp );
		  continue;
	      }

	      if ( sn < 0 )
                  bug( "Fread_char: unknown skill.", 0 );
	      else
                  ch->pcdata->learned[sn] = i;
	  }

	else if ( !str_cmp ( word, "Afft" ) )
	  {

	      int status;

	      paf                 = new_affect();

	      paf->type           = affect_lookup( fread_string( fp,
								&status ) );
	      paf->duration       = fread_number( fp, &status );
	      paf->modifier       = fread_number( fp, &status );
	      paf->location       = fread_number( fp, &status );
	      paf->bitvector      = fread_number( fp, &status );
	      paf->deleted        = FALSE;
	      paf->next           = ch->affected;
	      ch->affected        = paf;
	  }

        else
	{
	    sprintf( buf, "fread_char: Unknown key '%s' in pfile.", word );
	    bug( buf, 0 );
	    fread_to_eol( fp );
	}
	
    }

                /* Require all manditory fields, set defaults */

    for ( i = 0; *key_tab [i].key; i++ )
    {

        if ( key_tab [i].string == SPECIFIED ||
             key_tab [i].deflt == DEFLT )
            continue;

        if ( key_tab [i].deflt == MAND )
	{
            sprintf( buf, "Manditory field '%s' missing from pfile.",
                          key_tab [i].key );
            bug( buf, 0 );
            error_count++;
            continue;
	}

               /* This if/else sets default strings and numbers */

        if ( key_tab [i].string && key_tab [i].deflt )
	{
	    free_string( *(char **)key_tab [i].ptrs [0] );
            *(char **)key_tab [i].ptrs [0] =
	      str_dup( (char *)key_tab [i].deflt );
	}
        else
            for ( j = 0; key_tab [i].ptrs [j]; j++ )
	        *(int *)key_tab [i].ptrs [j] = key_tab [i].deflt;
    }

                /* Fixups */

    if ( ch->pcdata->title && isalnum ( *ch->pcdata->title ) )
    {
        sprintf( buf, " %s", ch->pcdata->title );
        free_string( ch->pcdata->title );
        ch->pcdata->title = str_dup( buf );
    }

    return error_count;

}

void recover( FILE *fp, long fpos )
{
    char        buf[ MAX_STRING_LENGTH ];

    fseek( fp, fpos, 0 );

    while ( !feof (fp) )
    {
        fpos = ftell( fp );

        if ( !fgets( buf, MAX_STRING_LENGTH, fp ) )
            return;

        if ( !strncmp( buf, "#OBJECT", 7 ) ||
             !strncmp( buf, "#END", 4 ) )
	{
            fseek( fp, fpos, 0 );
            return;
	}
    }
}

int fread_obj( CHAR_DATA *ch, FILE *fp )
{
    EXTRA_DESCR_DATA *ed;
    OBJ_DATA         obj;
    OBJ_DATA         *new_obj;
    AFFECT_DATA      *paf;
    char              buf[ MAX_STRING_LENGTH ];
    char             *spell_name = NULL;
    char             *p          = NULL;
    char             *word;
    char             *tmp_ptr;
    bool              fNest;
    bool              fVnum;
    long              fpos;
    int               iNest;
    int               iValue;
    int               status;
    int               sn;
    int               vnum;
    int               num_keys;
    int               last_key   = 0;
    int               i, j, tmpi;

    char              corobj [] = "This object was corrupted.";

    struct key_data key_tab [] =
      {
	{ "Name",        TRUE,  MAND,             { &obj.name,        NULL } },
	{ "ShortDescr",  TRUE,  (int) &corobj,    { &obj.short_descr, NULL } },
	{ "Description", TRUE,  (int) &corobj,    { &obj.description, NULL } },
	{ "ExtraFlags",  FALSE, MAND,             { &obj.extra_flags, NULL } },
	{ "WearFlags",   FALSE, MAND,             { &obj.wear_flags,  NULL } },
	{ "WearLoc",     FALSE, MAND,             { &obj.wear_loc,    NULL } },
	{ "ItemType",    FALSE, MAND,             { &obj.item_type,   NULL } },
	{ "Weight",      FALSE, 10,               { &obj.weight,      NULL } },
	{ "Level",       FALSE, ch->level,        { &obj.level,       NULL } },
	{ "Timer",       FALSE, 0,                { &obj.timer,       NULL } },
	{ "Cost",        FALSE, 300,              { &obj.cost,        NULL } },
	{ "Values",      FALSE, MAND,             { &obj.value [0],
						    &obj.value [1],
						    &obj.value [2],
						    &obj.value [3],
						    &obj.value [4],   NULL } },
	{ "\0",          FALSE, 0                                          } };

    memset( &obj, 0, sizeof( OBJ_DATA ) );

    obj.name        = str_dup( "" );
    obj.short_descr = str_dup( "" );
    obj.description = str_dup( "" );
    obj.deleted     = FALSE;

    fNest           = FALSE;
    fVnum           = TRUE;
    iNest           = 0;

    new_obj = new_object ();

    for ( num_keys = 0; *key_tab [num_keys].key; )
        num_keys++;

    for ( fpos = ftell( fp ) ; !feof( fp ) ; )
    {

        word = fread_word( fp, &status );

        for ( i = last_key;
              i < last_key + num_keys &&
                str_cmp( key_tab [i % num_keys].key, word ); )
            i++;

        i = i % num_keys;

        if ( !str_cmp( key_tab [i].key, word ) )
            last_key = i + 1;
        else
            i = num_keys;

        if ( *key_tab [i].key )         /* Key entry found in key_tab */
	{
            if ( key_tab [i].string == SPECIFIED )
                bug( "Key already specified.", 0 );

                        /* Entry is a string */

            else if ( key_tab [i].string )
	    {
                if ( ( p = fread_string( fp, &status ) ) && !status )
		{
                   free_string ( * (char **) key_tab [i].ptrs [0] );
                   * (char **) key_tab [i].ptrs [0] = p;
		}
	    }

                        /* Entry is an integer */
            else
                for ( j = 0; key_tab [i].ptrs [j]; j++ )
		{
                    tmpi = fread_number( fp, &status );
                    if ( !status )
                        * (int *) key_tab [i].ptrs [j] = tmpi;
		}

            if ( status )
	    {
                fread_to_eol( fp );
                continue;
	    }
	    else
                key_tab [i].string = SPECIFIED;
	}

        else if ( *word == '*' )
            fread_to_eol( fp );

        else if ( !str_cmp( word, "End" ) )
	{
            if ( !fNest || !fVnum )
	    {
                bug( "Fread_obj: incomplete object.", 0 );

		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.description );
		extract_obj( new_obj         );

		return FALSE;
	    }
            break;
	}

        else if ( !str_cmp( word, "Nest" ) )
	{

            iNest = fread_number( fp, &status );

            if ( status )       /* Losing track of nest level is bad */
                iNest = 0;      /* This makes objs go to inventory */

            else if ( iNest < 0 || iNest >= MAX_NEST )
                bug( "Fread_obj: bad nest %d.", iNest );

            else
	    {
                rgObjNest[iNest] = new_obj;
                fNest = TRUE;
	    }
	}

        else if ( !str_cmp( word, "Spell" ) )
	{

            iValue = fread_number( fp, &status );

            if ( !status )
                spell_name = fread_word( fp, &status );

            if ( status )       /* Recover is to skip spell */
	    {
                fread_to_eol( fp );
                continue;
	    }

            sn = skill_lookup( spell_name );

            if ( iValue < 0 || iValue > 3 )
                bug( "Fread_obj: bad iValue %d.", iValue );

            else if ( sn < 0 )
                bug( "Fread_obj: unknown skill.", 0 );

            else
                obj.value [iValue] = sn;
	}

        else if ( !str_cmp( word, "Vnum" ) )
	{

            vnum = fread_number( fp, &status );

            if ( status )               /* Can't live without vnum */
	    {
		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.description );
		extract_obj( new_obj         );
		return FALSE;
	    }

            if ( !( obj.pIndexData = get_obj_index( vnum ) ) )
                bug( "Fread_obj: bad vnum %d.", vnum );
            else
                fVnum = TRUE;
	}

                /* The following keys require extra processing */

        if ( !str_cmp( word, "Affect" ) )
	{
            paf = new_affect ();

	    paf->type       = fread_number( fp, &status );
	    paf->duration   = fread_number( fp, &status );
	    paf->modifier   = fread_number( fp, &status );
	    paf->location   = fread_number( fp, &status );
	    paf->bitvector  = fread_number( fp, &status );

            paf->next = obj.affected;
            obj.affected = paf;
	}

        else if ( !str_cmp( word, "ExtraDescr" ) )
	{
	    tmp_ptr = fread_string( fp, &status );

            if ( !status )
                p = fread_string( fp, &status );

            if ( status )
	    {
		recover    ( fp, fpos        );
		free_string( obj.name        );
		free_string( obj.short_descr );
		free_string( obj.description );
		extract_obj( new_obj         );
		return FALSE;
	    }

            ed = new_extra_descr ();

            ed->keyword     = tmp_ptr;
            ed->description = p;
            ed->next        = obj.extra_descr;
            obj.extra_descr = ed;
	}
    }
                /* Require all manditory fields, set defaults */

    for ( i = 0; *key_tab [i].key; i++ )
    {

        if ( key_tab [i].string == SPECIFIED ||
             key_tab [i].deflt == DEFLT )
            continue;

        if ( key_tab [i].deflt == MAND )
	{
            sprintf( buf, "Manditory obj field '%s' missing from pfile.",
		    key_tab [i].key );
            bug( buf, 0 );

	    recover    ( fp, fpos        );
	    free_string( obj.name        );
	    free_string( obj.short_descr );
	    free_string( obj.description );
	    extract_obj( new_obj         );

	    return FALSE;
	}

                /* This if/else sets default strings and numbers */

        if ( key_tab [i].string && key_tab [i].deflt )
            * (char **) key_tab [i].ptrs [0] =
                        str_dup ( (char *) key_tab [i].deflt );
        else
            for ( j = 0; key_tab [i].ptrs [j]; j++ )
                * (int *) key_tab [i].ptrs [j] = key_tab [i].deflt;
    }

    memcpy( new_obj, &obj, sizeof( OBJ_DATA ) );

    new_obj->next = object_list;
    object_list   = new_obj;

    new_obj->pIndexData->count++;
    if ( iNest == 0 || !rgObjNest[iNest] )
        obj_to_char( new_obj, ch );
    else
        obj_to_obj( new_obj, rgObjNest[iNest-1] );

    return TRUE;
}
