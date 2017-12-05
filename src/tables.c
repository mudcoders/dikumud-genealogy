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

#if !defined( macintosh )
extern  int     _filbuf	        args( (FILE *) );
#endif

#if !defined( ultrix ) && !defined( apollo )
#include <memory.h>
#endif



#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
                if ( !str_cmp( word, literal ) )    \
                {                                   \
                    field  = value;                 \
                    fMatch = TRUE;                  \
                    break;                          \
                }

#define SKEY( string, field )                       \
                if ( !str_cmp( word, string ) )     \
                {                                   \
                    free_string( field );           \
                  field = fread_string( fp, &stat); \
                    fMatch = TRUE;                  \
                    break;                          \
                }



/*
 * Globals.
 */

/* The social table. New socials contributed by Katrina and Binky */
SOC_INDEX_DATA *	soc_index_hash	[ MAX_WORD_HASH ];

/* Class table (see class files in area/classes) */
struct  class_type *    class_table     [ MAX_CLASS ];

/* Titles (see class files in area/classes) */ 
char *title_table 			[ MAX_CLASS ][ MAX_LEVEL+1 ][ 2 ];




/*
 * New code for loading classes from file.
 */
bool fread_class( char *filename )
{
    FILE   *fp;
    char   *word;
    char    buf  [ MAX_STRING_LENGTH ];
    bool    fMatch;
    int     stat;
    struct  class_type *class;
    int     number;
    int     tlev = 0;

    sprintf( buf, "%s%s", CLASS_DIR, filename );
    if ( !( fp = fopen( buf, "r" ) ) )
    {
        perror( buf );
        return FALSE;
    }

    word = feof( fp ) ? "End" : fread_word( fp, &stat );
    if ( !str_cmp( word, "Class" ) )
	number = fread_number( fp, &stat );
    else
	number = -1;	/* reports any error in case "Class N" doesn't exist */

    if ( number < 0 || number >= MAX_CLASS )
    {
	sprintf( buf, "Fread_class: bad class  '%s'.", filename );
	bug ( buf, 0 );
	return FALSE;
    }

    class = alloc_mem ( sizeof( CLASS_TYPE ) );

    class->filename = str_dup( filename );

    word = feof( fp ) ? "End" : fread_word( fp, &stat );
    if ( !str_cmp( word, "Name" ) )
	class->who_name = fread_string( fp, &stat );

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp, &stat );
	fMatch = FALSE;

	switch ( UPPER( word[0] ) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
            KEY( "AtrPrm", class->attr_prime,
            	fread_number( fp, &stat ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		fclose( fp );
		class_table[number] = class;
		return TRUE;
	    }
	    break;

	case 'G':
            KEY( "Guild", class->guild,
		fread_number( fp, &stat ) );
	    break;

	case 'H':
            KEY( "Hpmin", class->hp_min,
		fread_number( fp, &stat ) );
            KEY( "Hpmax", class->hp_max,
		fread_number( fp, &stat ) );
	    break;

	case 'M':
            KEY( "Mana", class->fMana, fread_number( fp, &stat ) );
	    break;

	case 'S':
            KEY( "SkillAdept", class->skill_adept,
		fread_number( fp, &stat ) );

	    if ( !str_cmp( word, "Skll" ) )
	    {
		int sn;
		int value;

		sn    = skill_lookup( fread_word( fp, &stat ) );
		value = fread_number( fp, &stat );
		if ( sn < 0 )
		    bug( "Fread_class: unknown skill.", 0 );
		else
		    skill_table[sn].skill_level[number] = value;
		fMatch = TRUE;
	    }

	    break;

	case 'T':
	    KEY( "Thac0", class->thac0_00,
		fread_number( fp, &stat ) );
	    KEY( "Thac47", class->thac0_47,
		fread_number( fp, &stat ) );

	    if ( !str_cmp( word, "Title" ) )
	    {
		if ( tlev <= MAX_LEVEL )
		{
                    title_table[number][tlev][0] = fread_string( fp, &stat );
                    title_table[number][tlev][1] = fread_string( fp, &stat );
		    ++tlev;
		}
		else
		    bug( "Fread_class: too many titles.", 0 );
		fMatch = TRUE;
	    }

	    break;

	case 'W':
	    KEY( "Weapon", class->weapon,
		fread_number( fp, &stat ) );
	    break;
	}

	if ( !fMatch )
	{
            sprintf( buf, "load_class_file: no match: %s", word );
            bug( buf, 0 );
	}
    }

    return FALSE;
}


/*
 * Load in all the class files.
 */ 
void load_classes()
{
    FILE *fpList;
    char *filename;
    char  fname		[ MAX_STRING_LENGTH ];
    char  classlist	[ MAX_STRING_LENGTH ];
    char  buf		[ MAX_STRING_LENGTH ];
    int   stat;
    int   sn;
    int   class;
    
    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( !skill_table[sn].name )
	    break;

	for ( class = 0; class < MAX_CLASS; class++ )
	    skill_table[sn].skill_level[class] = L_APP;	
    }

    sprintf( classlist, "%s%s", CLASS_DIR, CLASS_LIST );
    if ( !( fpList = fopen( classlist, "r" ) ) )
    {
        perror( classlist );
        exit( 1 );
    }

    for ( ; ; )
    {
        filename = feof( fpList ) ? "$" : fread_word( fpList, &stat );
	strcpy( fname, filename );
        if ( fname[0] == '$' )
          break;

        if ( !fread_class( fname ) )
        {
          sprintf( buf, "Cannot load class file: %s", fname );
          bug( buf, 0 );
        }
    }
    fclose( fpList );

    return;
}


void fwrite_class( int num )
{
    FILE                    *fp;
    const struct class_type *class = class_table[num];
    char                     buf	[ MAX_STRING_LENGTH ];
    char                     filename	[ MAX_INPUT_LENGTH ];
    int                      level;
    int                      sn;

    strcpy ( filename, class->filename );

    if ( !( fp = fopen( filename, "w" ) ) )
    {
        sprintf( buf, "Cannot open: %s for writing", filename );
        bug( buf, 0 );
        return;
    }

    fprintf( fp, "Name        %s~\n",        class->who_name	);
    fprintf( fp, "Class       %d\n",         num		);
    fprintf( fp, "Attrprime   %d\n",         class->attr_prime	);
    fprintf( fp, "Weapon      %d\n",         class->weapon	);
    fprintf( fp, "Guild       %d\n",         class->guild	);
    fprintf( fp, "Skilladept  %d\n",         class->skill_adept	);
    fprintf( fp, "Thac0       %d\n",         class->thac0_00	);
    fprintf( fp, "Thac47      %d\n",         class->thac0_47	);
    fprintf( fp, "Hpmin       %d\n",         class->hp_min	);
    fprintf( fp, "Hpmax       %d\n",         class->hp_max	);
    fprintf( fp, "Mana        %d\n",         class->fMana	);

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( !skill_table[sn].name )
	    break;

        if ( ( level = skill_table[sn].skill_level[num] ) < LEVEL_IMMORTAL )
          fprintf( fp, "Skll        '%s' %d\n", skill_table[sn].name, level );
    }

    for ( level = 0; level <= MAX_LEVEL; level++ )
        fprintf( fp, "Title\n%s~\n%s~\n",
                title_table[num][level][0], title_table[num][level][1] );
    fprintf( fp, "End\n" );
    fclose( fp );

    return;
}


void save_classes()
{
    int class;

    for ( class = 0; class < MAX_CLASS; class++ )
      fwrite_class( class );

    return;
}



/*
 * Add a social to the social index table                       - Thoric
 * Hashed and insert sorted (SMAUG code)
 */
void add_social( SOC_INDEX_DATA *social )
{
    SOC_INDEX_DATA *tmp;
    SOC_INDEX_DATA *prev;
    int             hash;
    int             x;

    if ( !social )
    {
	bug( "Add_social: NULL social", 0 );
	return;
    }

    if ( !social->name )
    {
	bug( "Add_social: NULL social->name", 0 );
	return;
    }

    if ( !social->char_no_arg )
    {
	bug( "Add_social: NULL social->char_no_arg", 0 );
	return;
    }

    /* make sure the name is all lowercase */
    for ( x = 0; social->name[x] != '\0'; x++ )
	social->name[x] = LOWER( social->name[x] );

    if ( social->name[0] < 'a' || social->name[0] > 'z' )
	hash = 0;
    else
	hash = ( social->name[0] - 'a' ) + 1;

    if ( !( prev = tmp = soc_index_hash[hash] ) )
    {
	social->next = soc_index_hash[hash];
	soc_index_hash[hash] = social;
	return;
    }

    for ( ; tmp; tmp = tmp->next )
    {
	if ( !( x = strcmp( social->name, tmp->name ) ) )
	{
	    bug( "Add_social: trying to add duplicate name to bucket %d", hash);
	    free_social( social );
	    return;
	}
	else
	if ( x < 0 )
	{
	    if ( tmp == soc_index_hash[hash] )
	    {
		social->next = soc_index_hash[hash];
		soc_index_hash[hash] = social;
		return;
	    }
	    prev->next = social;
	    social->next = tmp;
	    return;
	}
	prev = tmp;
    }

    /* add to end */
    prev->next = social;
    social->next = NULL;
    return;
}


/*
 * Save the social_table_tables to disk (cut & pasted from SMAUG - Zen)
 */
void save_socials()
{
    FILE           *fpout;
    SOC_INDEX_DATA *social;
    int             x;

    if ( !( fpout = fopen( SOCIAL_FILE, "w" ) ) )
    {
	bug( "Cannot open SOCIALS.TXT for writting", 0 );
	perror( SOCIAL_FILE );
	return;
    }

    for ( x = 0; x < 27; x++ )
    {
	for ( social = soc_index_hash[x]; social; social = social->next )
	{
	    if ( !social->name || social->name[0] == '\0' )
	    {
		bug( "Save_socials: blank social in hash bucket %d", x );
		continue;
	    }
	    fprintf( fpout, "#SOCIAL\n" );
	    fprintf( fpout, "Name        %s~\n",	social->name );
	    if ( social->char_no_arg )
		fprintf( fpout, "CharNoArg   %s~\n",	social->char_no_arg );
	    else
	        bug( "Save_socials: NULL char_no_arg in hash bucket %d", x );
	    if ( social->others_no_arg )
		fprintf( fpout, "OthersNoArg %s~\n",	social->others_no_arg );
	    if ( social->char_found )
		fprintf( fpout, "CharFound   %s~\n",	social->char_found );
	    if ( social->others_found )
		fprintf( fpout, "OthersFound %s~\n",	social->others_found );
	    if ( social->vict_found )
		fprintf( fpout, "VictFound   %s~\n",	social->vict_found );
	    if ( social->char_auto )
		fprintf( fpout, "CharAuto    %s~\n",	social->char_auto );
	    if ( social->others_auto )
		fprintf( fpout, "OthersAuto  %s~\n",	social->others_auto );
	    fprintf( fpout, "End\n\n" );
	}
    }

    fprintf( fpout, "#END\n" );
    fclose( fpout );
    return;
}


/*
 * Clear a new social.
 */
void clear_social( SOC_INDEX_DATA *soc )
{
    static SOC_INDEX_DATA soc_zero;

    *soc			= soc_zero;
    soc->name			= NULL;
    soc->char_no_arg		= NULL;
    soc->others_no_arg		= NULL;
    soc->char_found		= NULL;
    soc->others_found		= NULL;
    soc->vict_found		= NULL;
    soc->char_auto		= NULL;
    soc->others_auto		= NULL;
    return;
}


/*
 * Take a social data from the free list and clean it out.
 */
SOC_INDEX_DATA *new_social()
{
    SOC_INDEX_DATA *soc;

    soc		= alloc_perm( sizeof( SOC_INDEX_DATA ) );

    clear_social( soc );

    return soc;
}


/*
 * Remove a social from it's hash index                         - Thoric
 * (SMAUG code)
 */
void unlink_social( SOC_INDEX_DATA *social )
{
    SOC_INDEX_DATA *tmp;
    SOC_INDEX_DATA *tmp_next;
    int             hash;

    if ( !social )
    {
        bug( "Unlink_social: NULL social", 0 );
        return;
    }

    if ( social->name[0] < 'a' || social->name[0] > 'z' )
        hash = 0;
    else
        hash = (social->name[0] - 'a') + 1;

    if ( social == ( tmp = soc_index_hash[hash] ) )
    {
        soc_index_hash[hash] = tmp->next;
        return;
    }

    for ( ; tmp; tmp = tmp_next )
    {
        tmp_next = tmp->next;
        if ( social == tmp_next )
        {
            tmp->next = tmp_next->next;
            return;
        }
    }

    return;
}

/*
 * Free a social structure
 */
void free_social( SOC_INDEX_DATA *social )
{
    free_string( social->name );
    free_string( social->char_no_arg );
    free_string( social->others_no_arg );
    free_string( social->char_found );
    free_string( social->others_found );
    free_string( social->vict_found );
    free_string( social->char_auto );
    free_string( social->others_auto );

    free_mem( social, sizeof( SOC_INDEX_DATA ) );
    return;
}


void fread_social( FILE *fp )
{
    char           *word;
    SOC_INDEX_DATA *social;
    char            buf [ MAX_STRING_LENGTH ];
    bool            fMatch;
    int             stat;

    social = new_social();

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp, &stat );
	fMatch = FALSE;

	switch ( UPPER( word[0] ) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'C':
	    SKEY( "CharNoArg", social->char_no_arg );
	    SKEY( "CharFound", social->char_found );
	    SKEY( "CharAuto", social->char_auto );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !social->name )
		{
		    bug( "Fread_social: Name not found", 0 );
		    free_social( social );
		    return;
		}
		if ( !social->char_no_arg )
		{
		    bug( "Fread_social: CharNoArg not found", 0 );
		    free_social( social );
		    return;
		}
		add_social( social );
		return;
	    }
	    break;

	case 'N':
	    SKEY( "Name", social->name );
	    break;

	case 'O':
	    SKEY( "OthersNoArg", social->others_no_arg );
	    SKEY( "OthersFound", social->others_found );
	    SKEY( "OthersAuto",	social->others_auto );
	    break;

	case 'V':
	    SKEY( "VictFound", social->vict_found );
	    break;
	}
	
	if ( !fMatch )
	{
            sprintf( buf, "Fread_social: no match: %s", word );
	    bug( buf, 0 );
	}
    }

    return;
}


void load_socials()
{
    FILE *fp;
    int   stat;

    if ( !( fp = fopen( SOCIAL_FILE, "r" ) ) )
    {
	bug( "Cannot open SOCIALS.TXT", 0 );
	exit( 0 );
    }

    for ( ; ; )
    {
	char letter;
	char *word;

	letter = fread_letter( fp );

	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	if ( letter != '#' )
	{
	    bug( "Load_socials: # not found.", 0 );
	    break;
	}

	word = fread_word( fp, &stat );
	if ( !str_cmp( word, "SOCIAL" ) )
	{
	    fread_social( fp );
	    continue;
	}
	else
	if ( !str_cmp( word, "END" ) )
	    break;
	else
	{
	    bug( "Load_socials: bad section.", 0 );
	    continue;
	}
    }
    
    fclose( fp );
    return;
}
