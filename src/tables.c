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

CLAN_DATA *             clan_first = NULL;
CLAN_DATA *             clan_last  = NULL;



/*
 * New code for loading classes from file.
 */
bool fread_class( char *filename )
{
    FILE               *fp;
    char               *word;
    struct  class_type *class;
    char                buf  [ MAX_STRING_LENGTH ];
    bool                fMatch;
    int                 stat;
    int                 number = -1;
    int                 tlev   = 0;

    sprintf( buf, "%s%s", CLASS_DIR, filename );
    if ( !( fp = fopen( buf, "r" ) ) )
    {
        perror( buf );
        return FALSE;
    }

    class = alloc_mem ( sizeof( CLASS_TYPE ) );

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
            KEY( "AtrPrm", class->attr_prime, fread_number( fp, &stat ) );
	    break;

	case 'C':
            KEY( "Cla", number, fread_number( fp, &stat ) );

	    if ( number < 0 || number >= MAX_CLASS )
	    {
		sprintf( buf, "Fread_class: bad class  '%s'.", filename );
		bug ( buf, 0 );
		return FALSE;
	    }
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
            KEY( "Guild", class->guild, fread_number( fp, &stat ) );
	    break;

	case 'H':
            KEY( "Hpmin", class->hp_min, fread_number( fp, &stat ) );
            KEY( "Hpmax", class->hp_max, fread_number( fp, &stat ) );
	    break;

	case 'M':
            KEY( "Mana", class->fMana, fread_number( fp, &stat ) );
	    break;

	case 'N':
            SKEY( "Nm", class->name );
	    break;

	case 'S':
            KEY( "SkllAdpt", class->skill_adept, fread_number( fp, &stat ) );

	    if ( !str_cmp( word, "Skll" ) )
	    {
		int sn;
		int value;

		value = fread_number( fp, &stat );
		sn    = skill_lookup( fread_word( fp, &stat ) );
		if ( sn < 0 )
		    bug( "Fread_class: unknown skill.", 0 );
		else
		    skill_table[sn].skill_level[number] = value;
		fMatch = TRUE;
	    }

	    break;

	case 'T':
	    KEY( "Thac0", class->thac0_00, fread_number( fp, &stat ) );
	    KEY( "Thac47", class->thac0_47, fread_number( fp, &stat ) );

	    if ( !str_cmp( word, "Ttle" ) )
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
	    SKEY( "WhoNm", class->who_name );
	    KEY( "Wpn", class->weapon, fread_number( fp, &stat ) );
	    break;
	}

	if ( !fMatch )
	{
            bugf( "load_class_file: no match: %s", word );
	}
    }

    return FALSE;
}

/*
 * Load in all the class files.
 */ 
void load_classes( void )
{
    FILE *fpList;
    char *filename;
    char  fname		[ MAX_STRING_LENGTH ];
    char  classlist	[ MAX_STRING_LENGTH ];
    int   stat;
    
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
          bugf( "Cannot load class file: %s", fname );
        }
    }
    fclose( fpList );

    return;
}

void save_class( int num )
{
    FILE                    *fp;
    const struct class_type *class = class_table[num];
    char                     filename	[ MAX_INPUT_LENGTH ];
    char                     buf  	[ MAX_STRING_LENGTH ];
    int                      level;
    int                      sn;

    sprintf ( filename, "%s.class", class->name );

    sprintf( buf, "%s%s", CLASS_DIR, filename );
    if ( !( fp = fopen( buf, "w" ) ) )
    {
        bugf( "Cannot open: %s for writing", filename );
        return;
    }

    fprintf( fp, "Nm          %s~\n",        class->name	);
    fprintf( fp, "WhoNm       %s~\n",        class->who_name	);
    fprintf( fp, "Cla         %d\n",         num		);
    fprintf( fp, "AtrPrm      %d\n",         class->attr_prime	);
    fprintf( fp, "Wpn         %d\n",         class->weapon	);
    fprintf( fp, "Guild       %d\n",         class->guild	);
    fprintf( fp, "Sklladpt    %d\n",         class->skill_adept	);
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
          fprintf( fp, "Skll        %2d '%s'\n", level, skill_table[sn].name );
    }

    for ( level = 0; level <= MAX_LEVEL; level++ )
        fprintf( fp, "Ttle        %s~ %s~\n",
                title_table[num][level][0], title_table[num][level][1] );
    fprintf( fp, "End\n" );
    fclose( fp );

    return;
}

void save_classes( void )
{
    int class;

    for ( class = 0; class < MAX_CLASS; class++ )
      save_class( class );

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
void save_socials( void )
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
SOC_INDEX_DATA *new_social( void )
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

    if ( !islower( social->name[0] ) )
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
    free_string( social->name          );
    free_string( social->char_no_arg   );
    free_string( social->others_no_arg );
    free_string( social->char_found    );
    free_string( social->others_found  );
    free_string( social->vict_found    );
    free_string( social->char_auto     );
    free_string( social->others_auto   );

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

    social = new_social( );

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
	    SKEY( "CharFound", social->char_found  );
	    SKEY( "CharAuto",  social->char_auto   );
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
	    SKEY( "OthersFound", social->others_found  );
	    SKEY( "OthersAuto",	 social->others_auto   );
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

void load_socials( void )
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



/*
 * Get pointer to clan structure from clan name. (SMAUG)
 */
CLAN_DATA *get_clan( char *name )
{
    CLAN_DATA *clan;
    
    for ( clan = clan_first; clan; clan = clan->next )
       if ( !str_cmp( name, clan->name ) )
         return clan;
    return NULL;
}

/*
 * New code for loading clans from file.
 */
bool fread_clan( CLAN_DATA *clan, FILE *fp )
{
    char      *word;
    bool       fMatch;
    int        stat;

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
            SKEY( "Chieftain",   clan->chieftain );
            KEY( "Class",        clan->class,     fread_number( fp, &stat ) );
            KEY( "ClanHeros",    clan->clanheros, fread_number( fp, &stat ) );
            KEY( "ClanType",     clan->clan_type, fread_number( fp, &stat ) );
            KEY( "ClanObjOne",   clan->clanobj1,  fread_number( fp, &stat ) );
            KEY( "ClanObjTwo",   clan->clanobj2,  fread_number( fp, &stat ) );
            KEY( "ClanObjThree", clan->clanobj3,  fread_number( fp, &stat ) );
	    break;

	case 'D':
            SKEY( "Desc",    clan->description );
            KEY( "Donation", clan->donation,  fread_number( fp, &stat ) );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
		return TRUE;
	    break;

	case 'I':
            KEY( "IllegalPK",   clan->illegal_pk,  fread_number( fp, &stat ) );
	    break;

	case 'M':
            KEY( "Members",     clan->members, fread_number( fp, &stat ) );
            KEY( "MKills",      clan->mkills,  fread_number( fp, &stat ) );
            KEY( "MDeaths",     clan->mdeaths, fread_number( fp, &stat ) );
            SKEY( "Motto",      clan->motto );
	    break;

	case 'N':
            SKEY( "Name",      clan->name    );
	    break;

	case 'O':
            SKEY( "Overlord", clan->overlord );
	    break;

	case 'P':
            KEY( "PKills",  clan->pkills,  fread_number( fp, &stat ) );
            KEY( "PDeaths", clan->pdeaths, fread_number( fp, &stat ) );
	    break;

	case 'R':
            KEY( "Recall",  clan->recall,  fread_number( fp, &stat ) );
	    break;

	case 'S':
            KEY( "Score",     clan->score,     fread_number( fp, &stat ) );
            KEY( "Subchiefs", clan->subchiefs, fread_number( fp, &stat ) );
	    break;

	case 'W':
            SKEY( "WhoName",   clan->who_name    );
	    break;

	}

	if ( !fMatch )
	{
            bugf( "Load_clan_file: no match: %s", word );
	}
    }

    return FALSE;
}

bool load_clan_file( char *filename )
{
    CLAN_DATA *clan;
    FILE      *fp;
    int        stat;
    char       buf [ MAX_STRING_LENGTH ];

    sprintf( buf, "%s%s", CLAN_DIR, filename );
    if ( !( fp = fopen( buf, "r" ) ) )
    {
        perror( buf );
        return FALSE;
    }

    clan = alloc_mem ( sizeof( CLAN_DATA ) );
    clan->filename = str_dup( filename );

    for ( ; ; )
    {
	char *word;
	char letter;

	letter = fread_letter( fp );
	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	if ( letter != '#' )
	{
	    bug( "Load_clan_file: # not found.", 0 );
	    free_mem( clan, sizeof( CLAN_DATA ) );
	    break;
	}

	word = fread_word( fp, &stat );

	if ( !str_cmp( word, "CLAN" ) )
	{
	    fread_clan( clan, fp );

	    if ( !clan_first )
		clan_first	= clan;
	    else
		clan_last->next	= clan;
	    clan->next		= NULL;
	    clan_last		= clan;

	    break;
	}
	else if ( !str_cmp( word, "END"  ) )				break;
	else
	{
	    bugf( "Load_clan_file: bad section: %s.", word );
	    free_mem( clan, sizeof( CLAN_DATA ) );
	    break;
	}
    }
    fclose( fp );

    return TRUE;
}

/*
 * Load in all the clan files.
 */ 
void load_clans( void )
{
    FILE *fpList;
    char *filename;
    char  fname		[ MAX_STRING_LENGTH ];
    char  clanslist	[ MAX_STRING_LENGTH ];
    int   stat;
    
    clan_first  = NULL;
    clan_last   = NULL;

    log_string( "Loading clans..." );

    sprintf( clanslist, "%s%s", CLAN_DIR, CLANS_LIST );
    if ( !( fpList = fopen( clanslist, "r" ) ) )
    {
        perror( clanslist );
        exit( 1 );
    }

    for ( ; ; )
    {
        filename = feof( fpList ) ? "$" : fread_word( fpList, &stat );
	strcpy( fname, filename );
        if ( fname[0] == '$' )
          break;

        if ( !load_clan_file( fname ) )
        {
          bugf( "Cannot load clan file: %s", fname );
        }
    }
    fclose( fpList );

    return;
}

void save_clan_list( void )
{
    FILE      *fp;
    CLAN_DATA *clan;
    char       clanslist	[ MAX_STRING_LENGTH ];

    sprintf( clanslist, "%s%s", CLAN_DIR, CLANS_LIST );
    if ( !( fp = fopen( clanslist, "w" ) ) )
    {
        bug( "Save_clan_list: fopen", 0 );
        perror( clanslist );
	return;
    }

    for ( clan = clan_first; clan; clan = clan->next )
	fprintf( fp, "%s\n", clan->filename );

    fprintf( fp, "$\n" );
    fclose( fp );

    return;
}

/*
 * New code for writing a clan to a file.
 */
void save_clan( CLAN_DATA *clan )
{
    FILE                    *fp;
    char                     filename	[ MAX_INPUT_LENGTH ];
    char                     buf	[ MAX_STRING_LENGTH ];

    if ( !clan->filename )
	return;
    
    strcpy( filename, clan->filename );

    sprintf( buf, "%s%s", CLAN_DIR, filename );
    if ( !( fp = fopen( buf, "w" ) ) )
    {
        bugf( "Cannot open: %s for writing", filename );
        return;
    }

    fprintf( fp, "#CLAN\n" );
    fprintf( fp, "WhoName       %s~\n",        clan->who_name		);
    fprintf( fp, "Name          %s~\n",        clan->name		);
    fprintf( fp, "Motto         %s~\n",        clan->motto		);
    fprintf( fp, "Desc          %s~\n", fix_string( clan->description ) );
    fprintf( fp, "Overlord      %s~\n",        clan->overlord		);
    fprintf( fp, "Chieftain     %s~\n",        clan->chieftain		);
    fprintf( fp, "PKills        %d\n",         clan->pkills		);
    fprintf( fp, "PDeaths       %d\n",         clan->pdeaths		);
    fprintf( fp, "MKills        %d\n",         clan->mkills		);
    fprintf( fp, "MDeaths       %d\n",         clan->mdeaths		);
    fprintf( fp, "IllegalPK     %d\n",         clan->illegal_pk		);
    fprintf( fp, "Score         %d\n",         clan->score		);
    fprintf( fp, "ClanType      %d\n",         clan->clan_type		);
    fprintf( fp, "Clanheros     %d\n",         clan->clanheros		);
    fprintf( fp, "Subchiefs     %d\n",         clan->subchiefs		);
    fprintf( fp, "Members       %d\n",         clan->members		);
    fprintf( fp, "ClanObjOne    %d\n",         clan->clanobj1		);
    fprintf( fp, "ClanObjTwo    %d\n",         clan->clanobj2		);
    fprintf( fp, "ClanObjThree  %d\n",         clan->clanobj3		);
    fprintf( fp, "Recall        %d\n",         clan->recall 		);
    fprintf( fp, "Donation      %d\n",         clan->donation		);
    fprintf( fp, "Class         %d\n",         clan->class  		);

    fprintf( fp, "End\n" );
    fprintf( fp, "#END\n" );
    fclose( fp );

    return;
}



void do_makeclan( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    CLAN_DATA *clan;
    char       filename [ MAX_STRING_LENGTH ];
    char       who_name [ MAX_STRING_LENGTH ];

    rch = get_char( ch );

    if ( !authorized( rch, "makeclan" ) )
        return;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: makeclan <clan name>\n\r", ch );
        return;
    }

    one_argument( argument, who_name );
    who_name[14] = '\0';

    sprintf( filename, "%s.clan", who_name );

    clan = alloc_mem ( sizeof( CLAN_DATA ) );
    if ( !clan_first )
	clan_first		= clan;
    else
	clan_last->next		= clan;
	clan->next		= NULL;
	clan_last		= clan;

    clan->filename		= str_dup( filename );
    clan->who_name		= str_dup( who_name );
    clan->name			= str_dup( argument );
    clan->motto			= str_dup( "" );
    clan->description		= str_dup( "" );
    clan->overlord		= str_dup( "" );
    clan->chieftain		= str_dup( "" );
    clan->clanheros		= 0;
    clan->subchiefs		= 0;
    clan->members		= 0;
    clan->recall		= 3001;
    clan->donation		= 0;
    clan->class			= 0;
    clan->mkills		= 0;
    clan->mdeaths		= 0;
    clan->pkills		= 0;
    clan->pdeaths		= 0;
    clan->illegal_pk		= 0;
    clan->score			= 0;
    clan->clan_type		= 0;

    return;
}
