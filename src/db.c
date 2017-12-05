/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright5 (C) 1992, 1993 by Michael         *
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
#include <stdarg.h>
#include "merc.h"

#if !defined( macintosh )
extern  int     _filbuf	        args( (FILE *) );
#endif

#if !defined( ultrix ) && !defined( apollo )
#include <memory.h>
#endif



/*
 * Globals.
 */
SYSTEM_DATA		sysdata;		/* for mud configuration */

HELP_DATA *		help_first;
HELP_DATA *		help_last;

SHOP_DATA *		shop_first;
SHOP_DATA *		shop_last;

GAME_DATA *		game_first;
GAME_DATA *		game_last;

CHAR_DATA *		char_free;
EXTRA_DESCR_DATA *	extra_descr_free;
AFFECT_DATA *		affect_free;
NOTE_DATA *		note_free;
OBJ_DATA *		obj_free;
PC_DATA *		pcdata_free;

char                    bug_buf                 [ MAX_INPUT_LENGTH*2 ];
CHAR_DATA *		char_list;
char *			help_greeting;
char	                log_buf                 [ MAX_INPUT_LENGTH*2 ];
KILL_DATA	        kill_table              [ MAX_LEVEL          ];
NOTE_DATA *		note_list;
OBJ_DATA *		object_list;
TIME_INFO_DATA		time_info;
WEATHER_DATA		weather_info;
time_t                  down_time;
time_t                  warning1;
time_t                  warning2;
bool                    Reboot;

int			gsn_backstab;
int                     gsn_berserk;		/* by Thelonius */
int                     gsn_breathe_water;	/* by Thelonius */
int                     gsn_burning_hands;
int                     gsn_circle;		/* by Thelonius */
int			gsn_disarm;
int			gsn_dodge;
int			gsn_hide;
int			gsn_peek;
int			gsn_pick_lock;
int                     gsn_poison_weapon;	/* by Thelonius */
int                     gsn_scrolls;		/* by Binky / Thelonius */
int                     gsn_snare;		/* by Binky / Thelonius */
int			gsn_sneak;
int                     gsn_staves;		/* by Binky / Thelonius */
int			gsn_steal;
int                     gsn_untangle;		/* by Thelonius */
int                     gsn_wands;		/* by Binky / Thelonius */

int                     gsn_bash;
int                     gsn_dual;		/* by Thelonius */
int			gsn_enhanced_damage;
int			gsn_kick;
int			gsn_parry;
int			gsn_rescue;
int			gsn_second_attack;
int			gsn_third_attack;

int			gsn_blindness;
int			gsn_charm_person;
int			gsn_curse;
int			gsn_invis;
int			gsn_mass_invis;
int			gsn_mute;		/* by Thelonius */
int			gsn_poison;
int			gsn_sleep;
int			gsn_turn_undead;


/*
 * Psionicist gsn's (by Thelonius).
 */
int                     gsn_chameleon;
int                     gsn_domination;
int                     gsn_heighten;
int                     gsn_shadow;


int                     gsn_stake;

/*
 * New gsn's (by Zen & other ppl).
 */
int                     gsn_scan;
int                     gsn_shield_block;
int                     gsn_fast_healing;
int                     gsn_fourth_attack;
int			gsn_brew;
int			gsn_scribe;
int                     gsn_dirt;
int                     gsn_meditate;		/* by Zen */
int			gsn_swim;		/* by Zen */
int			gsn_mass_vortex_lift;	/* by Zen */
int			gsn_track;
int			gsn_whirlwind;
int			gsn_plague;

int			gsn_hit;
int			gsn_slash;
int			gsn_pierce;
int			gsn_whip;
int			gsn_explode;
int			gsn_pound;
int			gsn_suction;
int			gsn_shot;
int			gsn_mount;
int			gsn_flame_shield;
int			gsn_frost_shield;
int			gsn_shock_shield;

/*
 * Race gsn's (by Kahn).
 */
int                     gsn_vampiric_bite;


/*
 * Locals.
 */
MOB_INDEX_DATA *	mob_index_hash	        [ MAX_KEY_HASH       ];
OBJ_INDEX_DATA *	obj_index_hash	        [ MAX_KEY_HASH       ];
ROOM_INDEX_DATA *	room_index_hash         [ MAX_KEY_HASH       ];

AREA_DATA *		area_first;
AREA_DATA *		area_last;

extern char		str_empty	        [ 1                  ];

int			top_affect;
int			top_area;
int			top_ed;
int			top_exit;
int			top_help;
int			top_mob_index;
int			top_obj_index;
int			top_reset;
int			top_room;
int			top_shop;
int			top_game;
int                     top_vnum_room;
int                     top_vnum_mob;
int                     top_vnum_obj;

/*
 * Memory management.
 * Increase MAX_STRING from 1500000 if you have too.
 * Tune the others only if you understand what you're doing.
 * MAX_STRING is now in ssm.c
 */
extern int		MAX_STRING;

#if defined( macintosh )
#define			MAX_PERM_BLOCK  131072
#define			MAX_MEM_LIST    11

void *			rgFreeList              [ MAX_MEM_LIST       ];
const int		rgSizeList              [ MAX_MEM_LIST       ]  =
{
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768-64
};
#else
#define			MAX_PERM_BLOCK  131072
#define			MAX_MEM_LIST    14

void *			rgFreeList              [ MAX_MEM_LIST       ];
const int		rgSizeList              [ MAX_MEM_LIST       ]  =
{
    4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768-64
};
#endif

extern int		nAllocString;
extern int		sAllocString;
extern int		nOverFlowString;
extern int		sOverFlowString;
extern bool		Full;
int			nAllocPerm;
int			sAllocPerm;


/*
 * Semi-locals.
 */
bool			fBootDb;
FILE *			fpArea;
char			strArea			[ MAX_INPUT_LENGTH   ];

void    init_string_space ( void );
void    boot_done( );
char *  daPrompt;

/*
 * Local booting procedures.
 */
void	init_mm		  args( ( void ) );

void	load_area_file    args( ( void ) );
void	load_area         args( ( FILE *fp ) );
void	load_helps        args( ( FILE *fp ) );
void    load_recall       args( ( FILE *fp ) );
void	load_mobiles      args( ( FILE *fp ) );
void	load_objects      args( ( FILE *fp ) );
void	load_resets       args( ( FILE *fp ) );
void	load_rooms        args( ( FILE *fp ) );
void	load_shops        args( ( FILE *fp ) );
void	load_specials     args( ( FILE *fp ) );
void	load_games	  args( ( FILE *fp ) );
void	load_notes	  args( ( void ) );
void	load_ban          args( ( void ) );
void    load_sysdata      args( ( void ) );
void    save_sysdata      args( ( void ) );
void    load_down_time    args( ( void ) );
void	fix_exits         args( ( void ) );

void	reset_area        args( ( AREA_DATA *pArea ) );

/*
 * MOBprogram locals
 */
int 		mprog_name_to_type	args ( ( char* name ) );
MPROG_DATA *	mprog_file_read 	args ( ( const char* f, MPROG_DATA* mprg, 
						MOB_INDEX_DATA *pMobIndex ) );
void		load_mobprogs           args ( ( FILE* fp ) );
void   		mprog_read_programs     args ( ( FILE* fp,
					        MOB_INDEX_DATA *pMobIndex ) );
/*
 * Non-Envy Loading procedures.
 * Put any new loading function in this section.
 */
void    new_load_area		args( ( FILE *fp ) );	/* OLC */
void    new_load_objects	args( ( FILE *fp ) );	/* new object form */
void    new_load_rooms		args( ( FILE *fp ) );	/* OLC 1.1b */


/*
 * Big mama top level function.
 */
void boot_db( void )
{
    /*
     * Init some data space stuff.
     */
    {
        init_string_space( );
	fBootDb		= TRUE;
    }

    fprintf( stderr, "[*****] BOOT: ---------------------------[ Boot Log ]---------------------------" );

    /*
     * Init random number generator.
     */
    log_string( "Initializing random number generator" );
    {
	init_mm( );
    }

    /*
     * Set time and weather.
     */
    log_string( "Setting time and weather" );
    {
	long lhour, lday, lmonth;

	lhour		= ( current_time - 650336715 )
			   / ( PULSE_TICK / PULSE_PER_SECOND );
	time_info.hour  = lhour  % 24;
	lday		= lhour  / 24;
	time_info.day	= lday   % 35;
	lmonth		= lday   / 35;
	time_info.month	= lmonth % 17;
	time_info.year	= lmonth / 17;

	     if ( time_info.hour <  5 ) weather_info.sunlight = MOON_SET;
	else if ( time_info.hour <  6 ) weather_info.sunlight = SUN_RISE;
	else if ( time_info.hour < 19 ) weather_info.sunlight = SUN_LIGHT;
	else if ( time_info.hour < 20 ) weather_info.sunlight = SUN_SET;
	else if ( time_info.hour < 24 ) weather_info.sunlight = SUN_DARK;
	else                            weather_info.sunlight = MOON_RISE;

	weather_info.change	= 0;
	weather_info.mmhg	= 960;
	if ( time_info.month >= 7 && time_info.month <=12 )
	    weather_info.mmhg += number_range( 1, 50 );
	else
	    weather_info.mmhg += number_range( 1, 80 );

	     if ( weather_info.mmhg <=  980 ) weather_info.sky = SKY_LIGHTNING;
	else if ( weather_info.mmhg <= 1000 ) weather_info.sky = SKY_RAINING;
	else if ( weather_info.mmhg <= 1020 ) weather_info.sky = SKY_CLOUDY;
	else                                  weather_info.sky = SKY_CLOUDLESS;

    }

    /*
     * Assign gsn's for skills which have them.
     * Init skill_table class levels.
     */
    log_string( "Assigning gsn's" );
    {
	int sn;
	int class;

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].pgsn )
		*skill_table[sn].pgsn = sn;

	    for ( class = 0; class < MAX_CLASS; class++ )
		skill_table[sn].skill_level[class] = L_APP;
	}
    }

    load_sysdata( );

    log_string( "Loading socials" );
    load_socials( );

    log_string( "Loading classes" );
    load_classes( );

    /*
     * Read in all the area files.
     */
    log_string( "Reading in area files..." );
    {
	FILE      *fpList;
	char       strsave [ MAX_INPUT_LENGTH ];

	sprintf( strsave, "%s%s", AREA_DIR, AREA_LIST );

	if ( !( fpList = fopen( strsave, "r" ) ) )
	{
	    perror( AREA_LIST );
	    exit( 1 );
	}

	for ( ; ; )
	{
	    int stat;

	    strcpy( strArea, fread_word( fpList, &stat ) );
	    if ( strArea[0] == '$' )
		break;

	    load_area_file( );

	}
	fclose( fpList );
    }

    /*
     * Fix up exits.
     * Declare db booting over.
     * Reset all areas once.
     * Load up the notes file.
     */
    {
	log_string( "Fixing exits" );
	fix_exits( );
	fBootDb  = FALSE;
	daPrompt = str_dup( "<%hhp %mm %vmv> " );
	boot_done( );
	log_string( "Reseting areas" );
	area_update( );
	load_notes( );
	load_ban( );
	load_clans( );
	load_down_time( );
	MOBtrigger = TRUE;
    }

    return;
}



/*
 * Snarf an 'area' header line.
 */
void load_area_file( )
{
    int		stat;
    char	buf [ MAX_STRING_LENGTH ];

    if ( !fBootDb )
    {
        bug( "Load_area: can't load area if not booting!", 0 );
        return;
    }

    if ( strArea[0] == '-' )
    {
	fpArea = stdin;
	strcpy ( buf, &strArea[1] );
	strcpy ( strArea, buf );
    }
    else
    {
	char strsave [ MAX_INPUT_LENGTH ];

	sprintf( strsave, "%s%s", AREA_DIR, strArea );

	if ( !( fpArea = fopen( strsave, "r" ) ) )
	{
	    perror( strArea );
	    exit( 1 );
	}
    }

    for ( ; ; )
    {
	char *word;

	if ( fread_letter( fpArea ) != '#' )
	{
	    bug( "Load_area: # not found.", 0 );
	    exit( 1 );
	}

	word = fread_word( fpArea, &stat );

	if ( word[0] == '$'               )
	    break;
	else if ( !str_cmp( word, "AREA"     ) )
	    load_area    ( fpArea );
	else if ( !str_cmp( word, "HELPS"    ) ) 
	    load_helps   ( fpArea );
	else if ( !str_cmp( word, "RECALL"   ) )
	    load_recall  ( fpArea );
	else if ( !str_cmp( word, "MOBILES"  ) )
	    load_mobiles ( fpArea );
	else if ( !str_cmp( word, "MOBPROGS" ) ) 
	    load_mobprogs( fpArea );
	else if ( !str_cmp( word, "OBJECTS"  ) )
	    load_objects ( fpArea );
	else if ( !str_cmp( word, "RESETS"   ) )
	    load_resets  ( fpArea );
	else if ( !str_cmp( word, "ROOMS"    ) )
	    load_rooms   ( fpArea );
	else if ( !str_cmp( word, "SHOPS"    ) )
	    load_shops   ( fpArea );
	else if ( !str_cmp( word, "SPECIALS" ) )
	    load_specials( fpArea );
	else if ( !str_cmp( word, "GAMES"    ) )
	    load_games( fpArea );
	else if ( !str_cmp( word, "AREADATA" ) )
	    new_load_area( fpArea );
	else if ( !str_cmp( word, "ROOMDATA" ) )
	    new_load_rooms( fpArea );
	else if ( !str_cmp( word, "NEWOBJECTS" ) )
	    new_load_objects( fpArea );

	else
	{
	    bug( "Load_area: bad section name.", 0 );
	    exit( 1 );
	}
    }

    if ( fpArea != stdin )
	fclose( fpArea );
    fpArea = NULL;

    if ( area_last )
    {
	fprintf( stderr, "%-14s: Rooms: %5d - %-5d Objs: %5d - %-5d Mobs: %5d - %d\n",
		area_last->filename,
		area_last->low_r_vnum, area_last->hi_r_vnum,
		area_last->low_o_vnum, area_last->hi_o_vnum,
		area_last->low_m_vnum, area_last->hi_m_vnum );
    }
    else
	fprintf( stderr, "(%s)\n", strArea );

    return;
}



/*
 * Snarf an 'area' header line.
 */
void load_area( FILE *fp )
{
    AREA_DATA *pArea;
    char       author [ MAX_STRING_LENGTH ];
    char       range  [ MAX_STRING_LENGTH ];
    char       name   [ MAX_STRING_LENGTH ];
    char       arg1   [ MAX_STRING_LENGTH ];
    char       arg2   [ MAX_STRING_LENGTH ];
    char       buf    [ MAX_STRING_LENGTH ];
    char      *ptr;

    pArea               = alloc_perm( sizeof( *pArea ) );       /*
    pArea->reset_first  = NULL;                                  * OLC-Removed
    pArea->reset_last   = NULL;                                  */

    /*
     * Ugly hack to convert well behaved areas to new format.  -Zen
     */
    temp_fread_string( fp, buf );

    while ( ( ptr = strchr( buf, '{' ) ) )
	*ptr = '"';

    while ( ( ptr = strchr( buf, '}' ) ) )
	*ptr = '"';

    ptr = &buf[0];

    ptr = one_argument( ptr, range	);
    ptr = one_argument( ptr, author	);

    strcpy( name, ptr );

    ptr = &range[0];

    ptr = one_argument( ptr, arg1	);
    ptr = one_argument( ptr, arg2	);

    if ( is_number( arg1 ) && is_number( arg2 ) )
    {
	pArea->llv		= atoi( arg1 );
	pArea->ulv		= atoi( arg2 );
    }
    else
    if ( !str_cmp( arg1, "All" ) )
    {
	pArea->llv		= 0;
	pArea->ulv		= MAX_LEVEL;
    }
    else
    {
	pArea->llv		= 0;
	pArea->ulv		= 0;
    }

    pArea->name		= str_dup( name );
    pArea->author	= str_dup( capitalize( author ) );
    pArea->resetmsg	= str_dup( "You hear the patter of little feet." );

    pArea->recall       = ROOM_VNUM_TEMPLE;
    pArea->area_flags   = AREA_LOADING;
    pArea->security     = 1;
    pArea->builders     = str_dup( "None" );
    pArea->lvnum        = 0;
    pArea->uvnum        = 0;
    pArea->vnum         = top_area;
    pArea->filename     = str_dup( strArea );
    pArea->age		= 15;
    pArea->nplayer	= 0;

    pArea->low_r_vnum	= 0;
    pArea->hi_r_vnum	= 0;
    pArea->low_o_vnum	= 0;
    pArea->hi_o_vnum	= 0;
    pArea->low_m_vnum	= 0;
    pArea->hi_m_vnum	= 0;
    
    if ( !area_first )
	area_first = pArea;
    if (  area_last  )
    {
	area_last->next = pArea;
        REMOVE_BIT( area_last->area_flags, AREA_LOADING );
    }
    area_last	= pArea;
    pArea->next	= NULL;

    top_area++;
    return;
}



/* 
 * OLC
 * Snarf an 'area' header line.   Check this format.  MUCH better.  Add fields
 * too.
 *
 * #AREAFILE
 * Name   -Newbie School~
 * Author Locke~
 * Levels 1 54
 * Recall 3001
 * Reset You hear hitting a pencil on paper.~
 * End
 *
 * The first character of the name ( '-' or '*' ) signs, that the area
 * is taken from the original (-) or has been added by wizards (*).
 * 
 * -Sektor
 */
void fread_area( FILE *fp, AREA_DATA *pArea )
{
    char        *word;
    char        buf [ MAX_STRING_LENGTH ];
    int         i;
    int         j;
    int         error_count = 0;
    int         status;
    char        *p;
    int         tmpi;
    int         num_keys;
    int         last_key = 0;

    char	def_author [] = "Envy";
    char	def_reset  [] = "You hear the patter of little feet.";

    struct key_data key_tab [] = { 
      { "Name",     TRUE,  MAND,		{ &pArea->name,       NULL } },
      { "Author",   TRUE,  (unlong) &def_author,{ &pArea->author,     NULL } },
      { "Levels",   FALSE, 9999,		{ &pArea->llv,
						  &pArea->ulv,        NULL } },
      { "Security", FALSE, MAND,		{ &pArea->security,   NULL } },
      { "VNUMs",    FALSE, MAND,		{ &pArea->lvnum,
						  &pArea->uvnum,      NULL } },
      { "Builders", TRUE,  MAND,		{ &pArea->builders,   NULL } },
      { "Recall",   FALSE, ROOM_VNUM_TEMPLE,	{ &pArea->recall,     NULL } },
      { "Reset",    TRUE,  (unlong) &def_reset,	{ &pArea->resetmsg,   NULL } },
      { "\0",       FALSE, 0                                               } };


    for ( num_keys = 0; *key_tab [num_keys].key; )
        num_keys++;

    for ( ; !feof (fp) ; )
    {

        word = fread_word( fp, &status );

        if ( !word )
	{
            bug( "fread_area:  Error reading key.  EOF?", 0 );
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

        else if ( !str_cmp( word, "End" ) )
            break;

        else
	{
	    sprintf( buf, "fread_area: Unknown key '%s' in file.", word );
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
            sprintf( buf, "Manditory field '%s' missing from file.",
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

    if ( pArea->llv == 9999 )
	pArea->llv = 0;

    if ( pArea->ulv == 9999 )
	pArea->ulv = MAX_LEVEL;

    return;
}

void new_load_area( FILE *fp )
{
    AREA_DATA *pArea;

    pArea		= alloc_perm( sizeof( *pArea ) );
    pArea->recall       = ROOM_VNUM_TEMPLE;
    pArea->area_flags   = AREA_LOADING;
    pArea->security     = 1;
    pArea->builders     = str_dup( "None" );
    pArea->lvnum        = 0;
    pArea->uvnum        = 0;
    pArea->vnum         = top_area;
    pArea->filename     = str_dup( strArea );
    pArea->age		= 15;
    pArea->nplayer	= 0;

    pArea->low_r_vnum	= 0;
    pArea->hi_r_vnum	= 0;
    pArea->low_o_vnum	= 0;
    pArea->hi_o_vnum	= 0;
    pArea->low_m_vnum	= 0;
    pArea->hi_m_vnum	= 0;

    fread_area( fp, pArea );

    if ( !area_first )
	area_first = pArea;
    if (  area_last  )
    {
	area_last->next = pArea;
        REMOVE_BIT( area_last->area_flags, AREA_LOADING );
    }
    area_last	= pArea;
    pArea->next	= NULL;

    top_area++;
    return;
}


/*
 * Sets vnum range for area using OLC protection features.
 */
void assign_area_vnum( int vnum )
{
    if ( area_last->lvnum == 0 || area_last->uvnum == 0 )
        area_last->lvnum = area_last->uvnum = vnum;
    if ( vnum != URANGE( area_last->lvnum, vnum, area_last->uvnum ) )
        if ( vnum < area_last->lvnum )
            area_last->lvnum = vnum;
        else
            area_last->uvnum = vnum;
    return;
}



/*
 * Snarf a help section.
 */
void load_helps( FILE *fp )
{
    HELP_DATA *pHelp;
    char      *keyword;
    int        level;
    int        stat;

    for ( ; ; )
    {
        level           = fread_number( fp, &stat );
	keyword         = fread_string( fp, &stat );
	if ( keyword[0] == '$' )
	    break;
	pHelp		= alloc_perm( sizeof( *pHelp ) );
	pHelp->level	= level;
	pHelp->keyword	= keyword;
	pHelp->text	= fread_string( fp, &stat );
        pHelp->area     = area_last ? area_last : NULL;

	if ( !str_cmp( pHelp->keyword, "greeting" ) )
	    help_greeting = pHelp->text;

	if ( !help_first )
	    help_first = pHelp;
	if (  help_last  )
	    help_last->next = pHelp;

	help_last	= pHelp;
	pHelp->next	= NULL;
	top_help++;
    }

    return;
}



/*
 * Snarf a recall point.
 */
void load_recall( FILE *fp )
{
    AREA_DATA *pArea;
    char       buf [ MAX_STRING_LENGTH ];
    int        stat;

    pArea         = area_last;
    pArea->recall = fread_number( fp, &stat );

    if ( pArea->recall < 1 )
    {
        sprintf( buf, "Load_recall:  %s invalid recall point", pArea->name );
	bug( buf, 0 );
	pArea->recall = ROOM_VNUM_TEMPLE;
    }

    return;

}



/*
 * Snarf a mob section.
 */
void load_mobiles( FILE *fp )
{
    MOB_INDEX_DATA *pMobIndex;

    if ( !area_last )				/* OLC */
    {
        bug( "Load_mobiles: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
	char *race;
	char  letter;
	int   vnum;
	int   iHash;
	int   stat;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_mobiles: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp, &stat );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_mob_index( vnum ) )
	{
	    bug( "Load_mobiles: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pMobIndex			= alloc_perm( sizeof( *pMobIndex ) );
	pMobIndex->vnum			= vnum;

	if ( !area_last->low_m_vnum )
	    area_last->low_m_vnum       = vnum;
	if ( vnum > area_last->hi_m_vnum )
	    area_last->hi_m_vnum        = vnum;

        pMobIndex->area                 = area_last;
	pMobIndex->player_name		= fread_string( fp, &stat );
	pMobIndex->short_descr		= fread_string( fp, &stat );
	pMobIndex->long_descr		= fread_string( fp, &stat );
	pMobIndex->description		= fread_string( fp, &stat );

	pMobIndex->long_descr[0]	= UPPER( pMobIndex->long_descr[0]  );
	pMobIndex->description[0]	= UPPER( pMobIndex->description[0] );

	pMobIndex->act			= fread_number( fp, &stat ) | ACT_IS_NPC;
	pMobIndex->affected_by		= fread_number( fp, &stat );
	pMobIndex->pShop		= NULL;
	pMobIndex->alignment		= fread_number( fp, &stat );
	letter				= fread_letter( fp );
	pMobIndex->level		= fread_number( fp, &stat );
        /* Took number_fuzzy out of above line to store mob levels
           at a constant due to OLC resaving areas  --Stoked */

	/*
	 * The unused stuff is for imps who want to use the old-style
	 * stats-in-files method.
	 */
	pMobIndex->hitroll	= fread_number( fp, &stat );	/* Unused */
	pMobIndex->ac		= fread_number( fp, &stat );	/* Unused */
	pMobIndex->hitnodice	= fread_number( fp, &stat );	/* Unused */
	/* 'd'		*/	  fread_letter( fp );		/* Unused */
	pMobIndex->hitsizedice	= fread_number( fp, &stat );	/* Unused */
	/* '+'		*/	  fread_letter( fp );		/* Unused */
	pMobIndex->hitplus	= fread_number( fp, &stat );	/* Unused */
	pMobIndex->damnodice	= fread_number( fp, &stat );	/* Unused */
	/* 'd'		*/	  fread_letter( fp );		/* Unused */
	pMobIndex->damsizedice	= fread_number( fp, &stat );	/* Unused */
	/* '+'		*/	  fread_letter( fp );		/* Unused */
	pMobIndex->damplus	= fread_number( fp, &stat );	/* Unused */
	pMobIndex->gold		= fread_number( fp, &stat );	/* Unused */
	/* xp           */	  fread_number( fp, &stat );	/* Unused */
	/* position	*/	  fread_number( fp, &stat );	/* Unused */
	race			= fread_string( fp, &stat );
	pMobIndex->sex		= fread_number( fp, &stat );

	pMobIndex->race = race_lookup( race );
	if ( pMobIndex->race < 0 )
	{
	    bug( "Load_mobiles: vnum %d bad race.", vnum );
	    pMobIndex->race = 0;
	}

	if ( letter != 'S' )
	{
	    bug( "Load_mobiles: vnum %d non-S.", vnum );
	    exit( 1 );
	}

        letter = fread_letter( fp );
        if ( letter == '>' )
        {
          ungetc( letter, fp );
          mprog_read_programs( fp, pMobIndex );
        }
        else ungetc( letter,fp );

	iHash			= vnum % MAX_KEY_HASH;
	pMobIndex->next		= mob_index_hash[iHash];
	mob_index_hash[iHash]	= pMobIndex;
	top_mob_index++;
        top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;
        assign_area_vnum( vnum );
	kill_table[URANGE( 0, pMobIndex->level, MAX_LEVEL-1 )].number++;
    }

    return;
}



/*
 * Snarf an obj section.
 */
void load_objects( FILE *fp )
{
    OBJ_INDEX_DATA *pObjIndex;

    if ( !area_last )
    {
        bug( "Load_objects: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
        char *value [ 5 ];
	char  letter;
	int   vnum;
	int   iHash;
	int   stat;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_objects: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp, &stat );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_obj_index( vnum ) )
	{
	    bug( "Load_objects: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pObjIndex			= alloc_perm( sizeof( *pObjIndex ) );
	pObjIndex->vnum			= vnum;

	if ( !area_last->low_o_vnum )
	    area_last->low_o_vnum       = vnum;
	if ( vnum > area_last->hi_o_vnum )
	    area_last->hi_o_vnum        = vnum;

        pObjIndex->area                 = area_last;
	pObjIndex->name			= fread_string( fp, &stat );
	pObjIndex->short_descr		= fread_string( fp, &stat );
	pObjIndex->description		= fread_string( fp, &stat );
	/* Action description */	  fread_string( fp, &stat );

	pObjIndex->short_descr[0]	= LOWER( pObjIndex->short_descr[0] );
	pObjIndex->description[0]	= UPPER( pObjIndex->description[0] );

	pObjIndex->item_type		= fread_number( fp, &stat );
	pObjIndex->extra_flags		= fread_number( fp, &stat );
	pObjIndex->wear_flags		= fread_number( fp, &stat );
	value[0]		        = fread_string( fp, &stat );
	value[1]		        = fread_string( fp, &stat );
	value[2]		        = fread_string( fp, &stat );
	value[3]		        = fread_string( fp, &stat );
	value[4]		        = &str_empty[0];
	pObjIndex->weight		= fread_number( fp, &stat );
	pObjIndex->cost			= fread_number( fp, &stat );
	/* Cost per day */		  fread_number( fp, &stat );   /* Unused */

	/*
	 * Check here for the redundancy of invisible light sources - Kahn
	 */
	if ( pObjIndex->item_type == ITEM_LIGHT
	    && IS_SET( pObjIndex->extra_flags, ITEM_INVIS ) )
	{
	    bug( "Vnum %d : light source with ITEM_INVIS set", vnum );
	    REMOVE_BIT( pObjIndex->extra_flags, ITEM_INVIS );
	}
	    
	for ( ; ; )
	{
	    char letter;

	    letter = fread_letter( fp );

	    if ( letter == 'A' )
	    {
		AFFECT_DATA *paf;

		paf			= alloc_perm( sizeof( *paf ) );
		paf->type		= -1;
		paf->duration		= -1;
		paf->location		= fread_number( fp, &stat );
		paf->modifier		= fread_number( fp, &stat );
		paf->bitvector		= 0;
		paf->next		= pObjIndex->affected;
		pObjIndex->affected	= paf;
		top_affect++;
	    }

	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof( *ed ) );
		ed->keyword		= fread_string( fp, &stat );
		ed->description		= fread_string( fp, &stat );
		ed->next		= pObjIndex->extra_descr;
		pObjIndex->extra_descr	= ed;
		top_ed++;
	    }

	    else
	    {
		ungetc( letter, fp );
		break;
	    }
	}

	/*
	 * Translate character strings *value[] into integers:  sn's for
	 * items with spells, or straight conversion for other items.
	 * - Thelonius
	 */
	switch ( pObjIndex->item_type )
	{
	default:
	    pObjIndex->value[0] = atoi( value[0] );
	    pObjIndex->value[1] = atoi( value[1] );
	    pObjIndex->value[2] = atoi( value[2] );
	    pObjIndex->value[3] = atoi( value[3] );
	    pObjIndex->value[4] = atoi( value[4] );
	    break;

	case ITEM_PILL:
	case ITEM_POTION:
	case ITEM_SCROLL:
	    pObjIndex->value[0] = atoi( value[0] );
	    pObjIndex->value[1] = skill_lookup( value[1] );
	    pObjIndex->value[2] = skill_lookup( value[2] );
	    pObjIndex->value[3] = skill_lookup( value[3] );
	    pObjIndex->value[4] = skill_lookup( value[4] );
	    break;

	case ITEM_STAFF:
	case ITEM_WAND:
	    pObjIndex->value[0] = atoi( value[0] );
	    pObjIndex->value[1] = atoi( value[1] );
	    pObjIndex->value[2] = atoi( value[2] );
	    pObjIndex->value[3] = skill_lookup( value[3] );
	    pObjIndex->value[4] = atoi( value[4] );
	    break;
	}

	iHash			= vnum % MAX_KEY_HASH;
	pObjIndex->next		= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObjIndex;
	top_obj_index++;
        top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;
        assign_area_vnum( vnum );				   /* OLC */
    }

    return;
}


/*
 * Snarf an obj section. This supports the new obj format with value[4]...
 * So much work just to support a new value! This was why i ported OLC!
 * This way OLC acts as a file converter for your old Envy areas.
 * - Zen
 */
void new_load_objects( FILE *fp )
{
    OBJ_INDEX_DATA *pObjIndex;

    if ( !area_last )
    {
        bug( "Load_objects: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
        char *value [ 5 ];
	char  letter;
	int   vnum;
	int   iHash;
	int   stat;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_objects: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp, &stat );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_obj_index( vnum ) )
	{
	    bug( "Load_objects: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pObjIndex			= alloc_perm( sizeof( *pObjIndex ) );
	pObjIndex->vnum			= vnum;

	if ( !area_last->low_o_vnum )
	    area_last->low_o_vnum       = vnum;
	if ( vnum > area_last->hi_o_vnum )
	    area_last->hi_o_vnum        = vnum;

        pObjIndex->area                 = area_last;		/* OLC */
	pObjIndex->name			= fread_string( fp, &stat );
	pObjIndex->short_descr		= fread_string( fp, &stat );
	pObjIndex->description		= fread_string( fp, &stat );
	/* Action description */	  fread_string( fp, &stat );

	pObjIndex->short_descr[0]	= LOWER( pObjIndex->short_descr[0] );
	pObjIndex->description[0]	= UPPER( pObjIndex->description[0] );

	pObjIndex->item_type		= fread_number( fp, &stat );
	pObjIndex->extra_flags		= fread_number( fp, &stat );
	pObjIndex->wear_flags		= fread_number( fp, &stat );
	value[0]		        = fread_string( fp, &stat );
	value[1]		        = fread_string( fp, &stat );
	value[2]		        = fread_string( fp, &stat );
	value[3]		        = fread_string( fp, &stat );
	value[4]		        = fread_string( fp, &stat );
	pObjIndex->weight		= fread_number( fp, &stat );
	pObjIndex->cost			= fread_number( fp, &stat );
	/* Cost per day */		  fread_number( fp, &stat ); /*Unused*/

	/*
	 * Check here for the redundancy of invisible light sources - Kahn
	 */
	if ( pObjIndex->item_type == ITEM_LIGHT
	    && IS_SET( pObjIndex->extra_flags, ITEM_INVIS ) )
	{
	    bug( "Vnum %d : light source with ITEM_INVIS set", vnum );
	    REMOVE_BIT( pObjIndex->extra_flags, ITEM_INVIS );
	}
	    
	for ( ; ; )
	{
	    char letter;

	    letter = fread_letter( fp );

	    if ( letter == 'A' )
	    {
		AFFECT_DATA *paf;

		paf			= alloc_perm( sizeof( *paf ) );
		paf->type		= -1;
		paf->duration		= -1;
		paf->location		= fread_number( fp, &stat );
		paf->modifier		= fread_number( fp, &stat );
		paf->bitvector		= fread_number( fp, &stat );
		paf->next		= pObjIndex->affected;
		pObjIndex->affected	= paf;
		top_affect++;
	    }

	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof( *ed ) );
		ed->keyword		= fread_string( fp, &stat );
		ed->description		= fread_string( fp, &stat );
		ed->next		= pObjIndex->extra_descr;
		pObjIndex->extra_descr	= ed;
		top_ed++;
	    }

	    else
	    {
		ungetc( letter, fp );
		break;
	    }
	}

	/*
	 * Translate character strings *value[] into integers:  sn's for
	 * items with spells, or straight conversion for other items.
	 * - Thelonius
	 */
	switch ( pObjIndex->item_type )
	{
	default:
	    pObjIndex->value[0] = atoi( value[0] );
	    pObjIndex->value[1] = atoi( value[1] );
	    pObjIndex->value[2] = atoi( value[2] );
	    pObjIndex->value[3] = atoi( value[3] );
	    pObjIndex->value[4] = atoi( value[4] );
	    break;

	case ITEM_PILL:
	case ITEM_POTION:
	case ITEM_SCROLL:
	    pObjIndex->value[0] = atoi( value[0] );
	    pObjIndex->value[1] = skill_lookup( value[1] );
	    pObjIndex->value[2] = skill_lookup( value[2] );
	    pObjIndex->value[3] = skill_lookup( value[3] );
	    pObjIndex->value[4] = skill_lookup( value[4] );
	    break;

	case ITEM_STAFF:
	case ITEM_WAND:
	    pObjIndex->value[0] = atoi( value[0] );
	    pObjIndex->value[1] = atoi( value[1] );
	    pObjIndex->value[2] = atoi( value[2] );
	    pObjIndex->value[3] = skill_lookup( value[3] );
	    pObjIndex->value[4] = atoi( value[4] );
	    break;
	}

	iHash			= vnum % MAX_KEY_HASH;
	pObjIndex->next		= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObjIndex;
	top_obj_index++;
        top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;
        assign_area_vnum( vnum );
    }

    return;
}


/*
 * Adds a reset to a room.  OLC
 * Similar to add_reset in olc.c
 */
void new_reset( ROOM_INDEX_DATA *pR, RESET_DATA *pReset )
{
    RESET_DATA *pr;

    if ( !pR )
       return;

    pr = pR->reset_last;

    if ( !pr )
    {
        pR->reset_first = pReset;
        pR->reset_last  = pReset;
    }
    else
    {
        pR->reset_last->next = pReset;
        pR->reset_last       = pReset;
        pR->reset_last->next = NULL;
    }

    top_reset++;
    return;
}


/*
 * Snarf a reset section.
 */
void load_resets( FILE *fp )
{
    RESET_DATA *pReset;
    int         iLastRoom = 0;
    int         iLastObj  = 0;
    int		 stat;

    if ( !area_last )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	EXIT_DATA       *pexit;
	ROOM_INDEX_DATA *pRoomIndex;
	char             letter;

	if ( ( letter = fread_letter( fp ) ) == 'S' )
	    break;

	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	pReset		= alloc_perm( sizeof( *pReset ) );
	pReset->command	= letter;
	/* if_flag */	  fread_number( fp, &stat );
	pReset->arg1	= fread_number( fp, &stat );
	pReset->arg2	= fread_number( fp, &stat );
	pReset->arg3	= ( letter == 'G' || letter == 'R' )
			    ? 0 : fread_number( fp, &stat );
			  fread_to_eol( fp );

	/*
	 * Validate parameters.
	 * We're calling the index functions for the side effect.
	 */
	switch ( letter )
	{
	default:
	    bug( "Load_resets: bad command '%c'.", letter );
	    exit( 1 );
	    break;

        case 'M':
            get_mob_index  ( pReset->arg1 );
            if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
            {
                new_reset( pRoomIndex, pReset );
                iLastRoom = pReset->arg3;
            }
            break;

        case 'O':
            get_obj_index  ( pReset->arg1 );
            if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
            {
                new_reset( pRoomIndex, pReset );
                iLastObj = pReset->arg3;
            }
            break;

        case 'P':
            get_obj_index  ( pReset->arg1 );
            if ( ( pRoomIndex = get_room_index ( iLastObj ) ) )
            {
                new_reset( pRoomIndex, pReset );
            }
            break;

        case 'G':
        case 'E':
            get_obj_index  ( pReset->arg1 );
            if ( ( pRoomIndex = get_room_index ( iLastRoom ) ) )
            {
                new_reset( pRoomIndex, pReset );
                iLastObj = iLastRoom;
            }
            break;

        case 'D':
            pRoomIndex = get_room_index( pReset->arg1 );

            if (   pReset->arg2 < 0
                || pReset->arg2 >= MAX_DIR
                || !pRoomIndex
                || !( pexit = pRoomIndex->exit[pReset->arg2] )
                || !IS_SET( pexit->rs_flags, EX_ISDOOR ) )
            {
                bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
                exit( 1 );
            }

            switch ( pReset->arg3 )
            {
                default:
                    bug( "Load_resets: 'D': bad 'locks': %d." , pReset->arg3);
                case 0:
                    break;
                case 1: SET_BIT( pexit->rs_flags, EX_CLOSED );
                    break;
                case 2: SET_BIT( pexit->rs_flags, EX_CLOSED | EX_LOCKED );
                    break;
            }
            break;

        case 'R':
            if ( pReset->arg2 < 0 || pReset->arg2 > MAX_DIR ) /* Last Door. */
            {
                bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
                exit( 1 );
            }

            if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) )
                new_reset( pRoomIndex, pReset );

            break;
	}
    }

    return;
}



/*
 * Snarf a room section.
 */
void load_rooms( FILE *fp )
{
    ROOM_INDEX_DATA *pRoomIndex;

    if ( !area_last )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	char letter;
	int  vnum;
	int  door;
	int  iHash;
	int  stat;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_rooms: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp, &stat );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_room_index( vnum ) )
	{
	    bug( "Load_rooms: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pRoomIndex			= alloc_perm( sizeof( *pRoomIndex ) );
	pRoomIndex->people		= NULL;
	pRoomIndex->contents		= NULL;
	pRoomIndex->extra_descr		= NULL;
	pRoomIndex->area		= area_last;
	pRoomIndex->vnum		= vnum;
	pRoomIndex->name		= fread_string( fp, &stat );

	if ( !area_last->low_r_vnum )
	    area_last->low_r_vnum       = vnum;
	if ( vnum > area_last->hi_r_vnum )
	    area_last->hi_r_vnum        = vnum;

	pRoomIndex->description		= fread_string( fp, &stat );
	/* Area number */		  fread_number( fp, &stat );  /* Unused */
	pRoomIndex->room_flags		= fread_number( fp, &stat );
	pRoomIndex->orig_room_flags	= pRoomIndex->room_flags;
	pRoomIndex->sector_type		= fread_number( fp, &stat );
	pRoomIndex->light		= 0;
	for ( door = 0; door < MAX_DIR; door++ )
	    pRoomIndex->exit[door] = NULL;

	for ( ; ; )
	{
	    letter = fread_letter( fp );

	    if ( letter == 'S' || letter == 's' )
	    {
		if ( letter == 's' )
		    bug( "Load_rooms: vnum %d has lowercase 's'", vnum );
		break;
	    }

	    if ( letter == 'D' )
	    {
		EXIT_DATA *pexit;
		int        locks;

		door = fread_number( fp, &stat );
		if ( door < 0 || door >= MAX_DIR )
		{
		    bug( "Fread_rooms: vnum %d has bad door number.", vnum );
		    exit( 1 );
		}

		pexit			= alloc_perm( sizeof( *pexit ) );
		pexit->description	= fread_string( fp, &stat );
		pexit->keyword		= fread_string( fp, &stat );
		pexit->exit_info	= 0;
		pexit->rs_flags		= 0;
		locks			= fread_number( fp, &stat );
		pexit->key		= fread_number( fp, &stat );
		pexit->vnum		= fread_number( fp, &stat );

                switch ( locks )	/* OLC exit_info to rs_flags. */
                {
                case 1: pexit->rs_flags  = EX_ISDOOR;                    break;
                case 2: pexit->rs_flags  = EX_ISDOOR | EX_PICKPROOF;     break;
                case 3: pexit->rs_flags  = EX_ISDOOR | EX_BASHPROOF;     break;
                case 4: pexit->rs_flags  = EX_ISDOOR | EX_PICKPROOF
                                         | EX_BASHPROOF;                 break;
                case 5: pexit->rs_flags  = EX_ISDOOR | EX_PASSPROOF;     break;
                case 6: pexit->rs_flags  = EX_ISDOOR | EX_PICKPROOF
                                         | EX_PASSPROOF;                 break;
                case 7: pexit->rs_flags  = EX_ISDOOR | EX_BASHPROOF
                                         | EX_PASSPROOF;                 break;
                case 8: pexit->rs_flags  = EX_ISDOOR | EX_PICKPROOF
                                         | EX_BASHPROOF | EX_PASSPROOF;  break;
                }

		pRoomIndex->exit[door]   = pexit;
		top_exit++;
	    }
	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof( *ed ) );
		ed->keyword		= fread_string( fp, &stat );
		ed->description		= fread_string( fp, &stat );
		ed->next		= pRoomIndex->extra_descr;
		pRoomIndex->extra_descr	= ed;
		top_ed++;
	    }
	    else
	    {
		bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
		exit( 1 );
	    }
	}

	iHash			= vnum % MAX_KEY_HASH;
	pRoomIndex->next	= room_index_hash[iHash];
	room_index_hash[iHash]	= pRoomIndex;
	top_room++;
        top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room; /* OLC */
        assign_area_vnum( vnum );				     /* OLC */
    }

    return;
}


/* OLC 1.1b */
void new_load_rooms( FILE *fp )
{
    ROOM_INDEX_DATA *pRoomIndex;

    if ( !area_last )
    {
        bug( "Load_rooms: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
        char letter;
        int  vnum;
        int  door;
        int  iHash;
        int  stat;

        letter                          = fread_letter( fp );
        if ( letter != '#' )
        {
            bug( "Load_rooms: # not found.", 0 );
            exit( 1 );
        }

        vnum                            = fread_number( fp, &stat );
        if ( vnum == 0 )
            break;

        fBootDb = FALSE;
        if ( get_room_index( vnum ) )
        {
            bug( "Load_rooms: vnum %d duplicated.", vnum );
            exit( 1 );
        }
        fBootDb = TRUE;

        pRoomIndex                      = alloc_perm( sizeof( *pRoomIndex ) );
        pRoomIndex->people              = NULL;
        pRoomIndex->contents            = NULL;
        pRoomIndex->extra_descr         = NULL;
        pRoomIndex->area                = area_last;
        pRoomIndex->vnum                = vnum;

	if ( !area_last->low_r_vnum )
	    area_last->low_r_vnum       = vnum;
	if ( vnum > area_last->hi_r_vnum )
	    area_last->hi_r_vnum        = vnum;

        pRoomIndex->name                = fread_string( fp, &stat );
        pRoomIndex->description         = fread_string( fp, &stat );
        /* Area number */                 fread_number( fp, &stat ); /*Unused*/
        pRoomIndex->room_flags          = fread_number( fp, &stat );
	pRoomIndex->orig_room_flags	= pRoomIndex->room_flags;      /* OLC */
        pRoomIndex->sector_type         = fread_number( fp, &stat );
        pRoomIndex->light               = 0;
        for ( door = 0; door < MAX_DIR; door++ )
            pRoomIndex->exit[door] = NULL;

        for ( ; ; )
        {
            letter = fread_letter( fp );

            if ( letter == 'S' || letter == 's' )
            {
                if ( letter == 's' )
                    bug( "Load_rooms: vnum %d has lowercase 's'", vnum );
                break;
            }

            if ( letter == 'D' )
            {
                EXIT_DATA *pexit;
                int        locks;

                door = fread_number( fp, &stat );
                if ( door < 0 || door >= MAX_DIR )
                {
                    bug( "Fread_rooms: vnum %d has bad door number.", vnum );
                    exit( 1 );
                }

                pexit                   = alloc_perm( sizeof( *pexit ) );
                pexit->description      = fread_string( fp, &stat );
                pexit->keyword          = fread_string( fp, &stat );
                locks                   = fread_number( fp, &stat );
                pexit->exit_info        = locks;
                pexit->rs_flags         = locks;
                pexit->key              = fread_number( fp, &stat );
                pexit->vnum             = fread_number( fp, &stat );


                pRoomIndex->exit[door]          = pexit;
                top_exit++;
            }
            else if ( letter == 'E' )
            {
                EXTRA_DESCR_DATA *ed;

                ed                      = alloc_perm( sizeof( *ed ) );
                ed->keyword             = fread_string( fp, &stat );
                ed->description         = fread_string( fp, &stat );
                ed->next                = pRoomIndex->extra_descr;
                pRoomIndex->extra_descr = ed;
                top_ed++;
            }
            else
            {
                bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
                exit( 1 );
            }
        }

        iHash                   = vnum % MAX_KEY_HASH;
        pRoomIndex->next        = room_index_hash[iHash];
        room_index_hash[iHash]  = pRoomIndex;
        top_room++;
        top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room;
        assign_area_vnum( vnum );
    }

    return;
}


/*
 * Snarf a shop section.
 */
void load_shops( FILE *fp )
{
    SHOP_DATA *pShop;
    int        keeper = 0;

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	int             iTrade;
	int             stat;

	keeper                  = fread_number( fp, &stat );
	if ( keeper == 0 )
	    break;
	pShop			= alloc_perm( sizeof( *pShop ) );
	pShop->keeper		= keeper;
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    pShop->buy_type[iTrade] = fread_number( fp, &stat );
	pShop->profit_buy	= fread_number( fp, &stat );
	pShop->profit_sell	= fread_number( fp, &stat );
	pShop->open_hour	= fread_number( fp, &stat );
	pShop->close_hour	= fread_number( fp, &stat );
				  fread_to_eol( fp );
	pMobIndex		= get_mob_index( pShop->keeper );
	pMobIndex->pShop	= pShop;

	if ( !shop_first )
	    shop_first = pShop;
	if (  shop_last  )
	    shop_last->next = pShop;

	shop_last	= pShop;
	pShop->next	= NULL;
	top_shop++;
    }

    return;
}



/*
 * Snarf spec proc declarations.
 */
void load_specials( FILE *fp )
{
    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	OBJ_INDEX_DATA *pObjIndex;
	char            letter;
	int             stat;

	switch ( letter = fread_letter( fp ) )
	{
	default:
	    bug( "Load_specials: letter '%c' not *MS.", letter );
	    exit( 1 );

	case 'S':
	    return;

	case '*':
	    break;

	case 'M':
	    pMobIndex           = get_mob_index  ( fread_number ( fp, &stat ) );
	    pMobIndex->spec_fun = spec_mob_lookup( fread_word   ( fp, &stat ) );
	    if ( pMobIndex->spec_fun == 0 )
	    {
		bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
		exit( 1 );
	    }
	    break;

	case 'O':
	    pObjIndex           = get_obj_index  ( fread_number ( fp, &stat ) );
	    pObjIndex->spec_fun = spec_obj_lookup( fread_word   ( fp, &stat ) );
	    if ( pObjIndex->spec_fun == 0 )
	    {
		bug( "Load_specials: 'O': vnum %d.", pObjIndex->vnum );
		exit( 1 );
	    }
	    break;
	}

	fread_to_eol( fp );
    }
}



/*
 * Snarf games proc declarations.
 */
void load_games( FILE *fp )
{
    int        croupier = 0;

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	GAME_DATA      *pGame;
        char            letter;
	int             stat;

	pGame = alloc_perm( sizeof( GAME_DATA ) );

        switch ( letter = fread_letter( fp ) )
        {
        default:
            bug( "Load_games: letter '%c' not *MS.", letter );
            exit( 1 );

        case 'S':
            return;

        case '*':
            break;

        case 'M':
	    croupier			= fread_number( fp, &stat );
	    if ( croupier == 0 )
	    {
		free_mem( pGame, sizeof( GAME_DATA ) );
		return;
	    }
	    pMobIndex			= get_mob_index( croupier );
	    pMobIndex->pGame		= pGame;
	    pGame->croupier		= croupier;
	    pGame->game_fun		= game_lookup( fread_word ( fp, &stat ) );
	    pGame->bankroll		= fread_number( fp, &stat );
	    pGame->max_wait		= fread_number( fp, &stat );
	    pGame->cheat		= fread_number( fp, &stat );

	    if ( pGame->game_fun == 0 )
            {
                bug( "Load_games: 'M': vnum %d.", pMobIndex->vnum );
                exit( 1 );
            }
					  fread_to_eol( fp );
            break;
        }

	if ( !game_first )
	    game_first = pGame;
	if (  game_last  )
	    game_last->next = pGame;

	game_last	= pGame;
	pGame->next	= NULL;
	top_game++;
    }

    return;
}



/*
 * Snarf notes file.
 */
void load_notes( void )
{
    FILE      *fp;
    NOTE_DATA *pnotelast;
    char       strsave [ MAX_INPUT_LENGTH ];

    sprintf( strsave, "%s%s", SYSTEM_DIR, NOTE_FILE );

    if ( !( fp = fopen( strsave, "r" ) ) )
	return;

    pnotelast = NULL;
    for ( ; ; )
    {
	NOTE_DATA *pnote;
	int       letter;
	int        stat;

	do
	{
	    letter = getc( fp );
	    if ( feof( fp ) )
	    {
		fclose( fp );
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );

	pnote		  = alloc_mem( sizeof( *pnote ) );

	if ( str_cmp( fread_word( fp, &stat ), "sender" ) )
	    break;
	pnote->sender     = fread_string( fp, &stat );

	if ( str_cmp( fread_word( fp, &stat ), "date" ) )
	    break;
	pnote->date       = fread_string( fp, &stat );

	if ( str_cmp( fread_word( fp, &stat ), "stamp" ) )
	    break;
	pnote->date_stamp = fread_number( fp, &stat );

	if ( str_cmp( fread_word( fp, &stat ), "to" ) )
	    break;
	pnote->to_list    = fread_string( fp, &stat );

	if ( str_cmp( fread_word( fp, &stat ), "subject" ) )
	    break;
	pnote->subject    = fread_string( fp, &stat );

	if ( str_cmp( fread_word( fp, &stat ), "text" ) )
	    break;
	pnote->text       = fread_string( fp, &stat );

	if ( !note_list )
	    note_list           = pnote;
	else
	    pnotelast->next     = pnote;

	pnotelast               = pnote;
    }

    strcpy( strArea, NOTE_FILE );
    fpArea = fp;
    bug( "Load_notes: bad key word.", 0 );
    exit( 1 );
    return;
}



void load_sysdata( void )
{
    char        *word;
    char        buf [ MAX_STRING_LENGTH ];
    int         i;
    int         j;
    int         error_count = 0;
    int         status;
    char        *p;
    int         tmpi;
    int         num_keys;
    int         last_key = 0;
    FILE       *fp;
    char        strsave [ MAX_INPUT_LENGTH ];

    char	def_time [] = "";

    struct key_data key_tab [] = { 
      { "AllTimeMax", FALSE,  MAND,		{ &sysdata.all_time_max,
                                                                      NULL } },
      { "TimeOfMax",  TRUE,  (unlong) &def_time,{ &sysdata.time_of_max,
                                                                      NULL } },
      { "Act",        FALSE, 0,			{ &sysdata.act,       NULL } },
      { "\0",         FALSE, 0                                             } };


    sprintf( strsave, "%s%s", SYSTEM_DIR, SYSDATA_FILE );

    if ( !( fp = fopen( strsave, "r" ) ) )
	return;


    for ( num_keys = 0; *key_tab [num_keys].key; )
        num_keys++;

    for ( ; !feof (fp) ; )
    {

        word = fread_word( fp, &status );

        if ( !word )
	{
            bug( "fread_area:  Error reading key.  EOF?", 0 );
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

        else if ( !str_cmp( word, "End" ) )
            break;

        else
	{
	    sprintf( buf, "load_sysdata: Unknown key '%s' in file.", word );
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
            sprintf( buf, "Manditory field '%s' missing from sysfile.",
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

    fclose( fp );

    sysdata.max_players			= 0;

    return;
}


void save_sysdata( void )
{
    FILE *fp;
    char  strsave [ MAX_INPUT_LENGTH ];

    fclose( fpReserve );

    sprintf( strsave, "%s%s", SYSTEM_DIR, SYSDATA_FILE );

    if ( !( fp = fopen( strsave, "w" ) ) )
	return;

    fprintf( fp, "AllTimeMax  %d\n",	sysdata.all_time_max	);
    fprintf( fp, "TimeOfMax   %s~\n",	sysdata.time_of_max	);
    fprintf( fp, "Act         %d\n",	sysdata.act		);
    fprintf( fp, "End\n" );

    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


void load_down_time( void )
{
    FILE *fp;
    int   number = 0;
    char  strsave [ MAX_INPUT_LENGTH ];

    down_time = 0;
    warning1  = 0;
    warning2  = 0;
    Reboot    = FALSE;

    sprintf( strsave, "%s%s", SYSTEM_DIR, DOWN_TIME_FILE );

    if ( !( fp = fopen( strsave, "r" ) ) )
	return;

    for ( ; ; )
    {
        char *word;
	int   letter;
	int   stat;

	do
	{
	    letter = getc( fp );
	    if ( feof( fp ) )
	    {
		fclose( fp );
		if ( down_time > 0 && down_time < 301 )
		{
		    down_time  = current_time + 300;
		    warning2   = down_time - 150;
		    warning1   = warning2  - 75;
		}
		else
		    if ( down_time > 0 )
		    {
			down_time += current_time;
			warning2   = down_time - 150;
			warning1   = warning2  - 150;
		    }
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );
	
	word = fread_word( fp, &stat );

	if ( !str_cmp( word, "HOUR" ) )
	{
	    number = fread_number( fp, &stat );
	    if ( number > 0 )
	        down_time += (time_t) ( number * 3600 );
	}
	if ( !str_cmp( word, "MINUTE" ) )
	{
	    number = fread_number( fp, &stat );
	    if ( number > 0 )
	        down_time += (time_t) ( number * 60 );
	}
	if ( !str_cmp( word, "REBOOT" ) )
	{
	    Reboot = TRUE;
	}
    }
}

/*
 * Load up the ban file
 */
void load_ban( void )
{
    BAN_DATA  *pban;
    FILE      *fp;
    char       strsave [ MAX_INPUT_LENGTH ];

    sprintf( strsave, "%s%s", SYSTEM_DIR, BAN_FILE );

    if ( !( fp = fopen( strsave, "r" ) ) )
	return;

    for ( ; ; )
    {
        int    letter;
	int    stat;

	do
	{
	    letter = getc( fp );
	    if ( feof( fp ) )
	    {
		fclose( fp );
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );

	if ( !ban_free )
	{
	    pban     = alloc_perm( sizeof( *pban ) );
	}
	else
	{
	    pban     = ban_free;
	    ban_free = ban_free->next;
	}

	pban->name   = fread_string( fp, &stat );

	pban->next   = ban_list;
	ban_list     = pban;

    }

}

/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad or suspicious reverse exits.
 */
void fix_exits( void )
{
		 EXIT_DATA       *pexit;
		 EXIT_DATA       *pexit_rev;
		 ROOM_INDEX_DATA *pRoomIndex;
		 ROOM_INDEX_DATA *to_room;
                 char             buf     [ MAX_STRING_LENGTH ];
    extern const int              rev_dir [ ];
		 int              iHash;
		 int              door;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex;
	      pRoomIndex  = pRoomIndex->next )
	{
	    bool fexit;

	    fexit = FALSE;
	    for ( door = 0; door < MAX_DIR; door++ )
	    {
		if ( ( pexit = pRoomIndex->exit[door] ) )
		{
		    fexit = TRUE;
		    if ( pexit->vnum <= 0 )
			pexit->to_room = NULL;
		    else
			pexit->to_room = get_room_index( pexit->vnum );
		}
	    }

	    if ( !fexit )
		SET_BIT( pRoomIndex->room_flags, ROOM_NO_MOB );
	}
    }

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex;
	      pRoomIndex  = pRoomIndex->next )
	{
	    for ( door = 0; door < MAX_DIR; door++ )
	    {
		if (   ( pexit     = pRoomIndex->exit[door]       )
		    && ( to_room   = pexit->to_room               )
		    && ( pexit_rev = to_room->exit[rev_dir[door]] )
		    &&   pexit_rev->to_room != pRoomIndex )
		{
		    sprintf( buf, "Fix_exits: %d:%d -> %d:%d -> %d.",
			    pRoomIndex->vnum, door,
			    to_room->vnum,    rev_dir[door],
			    ( !pexit_rev->to_room ) ? 0
			    :  pexit_rev->to_room->vnum );
		    bug( buf, 0 );
		}
	    }
	}
    }

    return;
}


void reset_room( ROOM_INDEX_DATA *pRoom )
{
    RESET_DATA  *pReset;
    CHAR_DATA   *pMob;
    CHAR_DATA   *LastMob = NULL;
    int          iexit;
    int          level = 0;
    bool         last;

    if ( !pRoom )
        return;

    pMob        = NULL;
    last        = TRUE;
    
    for ( iexit = 0; iexit < MAX_DIR; iexit++ )
    {
        EXIT_DATA *pexit;

        if ( !( pexit = pRoom->exit[iexit] )
	    || IS_SET( pexit->exit_info, EX_BASHED ) )
	    continue;	  

	pexit->exit_info = pexit->rs_flags;

	if ( ( pexit->to_room )
	    && ( ( pexit = pexit->to_room->exit[rev_dir[iexit]] ) ) )
	{
	    /* nail the other side */
	    pexit->exit_info = pexit->rs_flags;
	}
    }

    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
    {
        OBJ_DATA        *pObj;
        OBJ_DATA        *LastObj;
        MOB_INDEX_DATA  *pMobIndex;
        OBJ_INDEX_DATA  *pObjIndex;
        OBJ_INDEX_DATA  *pObjToIndex;
        ROOM_INDEX_DATA *pRoomIndex;

        switch ( pReset->command )
        {
        default:
                bug( "Reset_room: bad command %c.", pReset->command );
                break;
        case 'M':
            if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
            {
                bug( "Reset_room: 'M': bad vnum %d.", pReset->arg1 );
                continue;
            }

            if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
            {
                bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
                continue;
            }

            if ( ( pMobIndex->spec_fun == spec_mob_lookup( "spec_cast_ghost" )
                  && ( weather_info.sunlight > SUN_DARK )
                  && ( weather_info.sunlight < MOON_RISE ) ) )
            {
                last = FALSE;
                continue;
            }

            level = URANGE( 0, pMobIndex->level - 2, LEVEL_HERO );

            /* If sentinel, then maximum number indicates in room instead
               of in world. -Kahn */
            /* Midgaard mayor is special case as it has a spec proc which
               moves it.  Dislike such specific coding.  Shrug.  -Kahn */
            if ( IS_SET( pMobIndex->act, ACT_SENTINEL )
                && pMobIndex->vnum != MOB_VNUM_MIDGAARD_MAYOR )
            {
                CHAR_DATA *ch;
                int        count = 0;

                for ( ch = pRoomIndex->people; ch; ch = ch->next_in_room )
                    if ( IS_NPC( ch ) && ch->pIndexData == pMobIndex )
                        count++;
                if ( count >= pReset->arg2 )
                {
                    last = FALSE;
                    break;
                }
            }
            else
                if ( pMobIndex->count >= pReset->arg2 )

                {
                    last = FALSE;
                    break;
                }
            pMob = create_mobile( pMobIndex );

            /*
             * Check for pet shop.
             */
            {
                ROOM_INDEX_DATA *pRoomIndexPrev;

                pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
                if ( pRoomIndexPrev
                    && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
                    SET_BIT( pMob->act, ACT_PET );
            }

            if ( room_is_dark( pRoomIndex ) )
                SET_BIT( pMob->affected_by, AFF_INFRARED );

            LastMob = pMob;
            char_to_room( pMob, pRoomIndex );
            level = URANGE( 0, pMob->level - 2, LEVEL_HERO );
            last  = TRUE;
            break;

        case 'O':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                bug( "Reset_room: 'O': bad vnum %d.", pReset->arg1 );
                continue;
            }

            if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
            {
                bug( "Reset_room: 'O': bad vnum %d.", pReset->arg3 );
                continue;
            }

            if ( pRoom->area->nplayer > 0
              || count_obj_list( pObjIndex, pRoomIndex->contents ) > 0 )
            {
                last = FALSE;
                break;
            }

            pObj = create_object( pObjIndex, number_fuzzy( level ) ); 
            pObj->cost = 0;
            obj_to_room( pObj, pRoom );
            last = TRUE;
            break;

        case 'P':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                bug( "Reset_room: 'P': bad vnum %d.", pReset->arg1 );
                continue;
            }

            if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
            {
                bug( "Reset_room: 'P': bad vnum %d.", pReset->arg3 );
                continue;
            }

            if ( pRoom->area->nplayer > 0
              || !( LastObj = get_obj_type( pObjToIndex ) )
              || count_obj_list( pObjIndex, LastObj->contains ) > 0 )
            {
                last = FALSE;
                break;
            }

            pObj = create_object( pObjIndex, number_fuzzy( LastObj->level ) );
            obj_to_obj( pObj, LastObj );
            last = TRUE;

            /*
             * Ensure that the container gets reset.    OLC 1.1b
             */
            if ( LastObj->item_type == ITEM_CONTAINER )
            {
                LastObj->value[1] = LastObj->pIndexData->value[1];
            }
            else
            {
                    /* THIS SPACE INTENTIONALLY LEFT BLANK */
            }
            break;

        case 'G':
        case 'E':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                bug( "Reset_room: 'E' or 'G': bad vnum %d.", pReset->arg1 );
                continue;
            }

            if ( !last )
                break;

            if ( !LastMob )
            {
                bug( "Reset_room: 'E' or 'G': null mob for vnum %d.",
                    pReset->arg1 );
                last = FALSE;
                break;
            }

            if ( LastMob->pIndexData->pShop )   /* Shop-keeper? */
            {
                int olevel;

                switch ( pObjIndex->item_type )
                {
                default:		olevel = 0;                      break;
                case ITEM_PILL:		olevel = number_range(  0, 10 ); break;
                case ITEM_POTION:	olevel = number_range(  0, 10 ); break;
                case ITEM_SCROLL:	olevel = number_range(  5, 15 ); break;
                case ITEM_WAND:		olevel = number_range( 10, 20 ); break;
                case ITEM_STAFF:	olevel = number_range( 15, 25 ); break;
                case ITEM_ARMOR:	olevel = number_range(  5, 15 ); break;
                case ITEM_FURNITURE:	olevel = number_range(  5, 15 ); break;
                case ITEM_CLOTHING:	olevel = number_range(  5, 15 ); break;
                case ITEM_WEAPON:       if ( pReset->command == 'G' )
                                            olevel = number_range( 5, 15 );
                                        else
                                            olevel = number_fuzzy( level );
                  break;
                }

                pObj = create_object( pObjIndex, olevel );
                if ( pReset->command == 'G' )
                    SET_BIT( pObj->extra_flags, ITEM_INVENTORY );
            }
            else
            {
                pObj = create_object( pObjIndex, number_fuzzy( level ) );
            }
            obj_to_char( pObj, LastMob );
            if ( pReset->command == 'E' )
                equip_char( LastMob, pObj, pReset->arg3 );
            last = TRUE;
            break;

        case 'D':
	    break;

        case 'R':
            break;
        }
    }

    return;
}


/*
 * Repopulate areas periodically.
 */
void area_update( void )
{
    AREA_DATA *pArea;
    char       buf [ MAX_STRING_LENGTH ];

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	CHAR_DATA *pch;

	if ( ++pArea->age < 3 )
	    continue;

	/*
	 * Check for PC's.
	 */
	if ( pArea->nplayer > 0 && pArea->age == 15 - 1 )
	{
	    for ( pch = char_list; pch; pch = pch->next )
	    {
		if ( !IS_NPC( pch )
		    && IS_AWAKE( pch )
		    && pch->in_room
		    && pch->in_room->area == pArea )
		{
		    sprintf( buf, "%s\n\r", pArea->resetmsg );
		    send_to_char( buf,
			pch );
		}
	    }
	}

	/*
	 * Check age and reset.
	 * Note: Mud School resets every 3 minutes (not 15).
	 */
	if ( pArea->nplayer == 0 || pArea->age >= 15 )
	{
	    ROOM_INDEX_DATA *pRoomIndex;

	    sprintf( buf, "%s has just been reset.", pArea->filename );
	    wiznet( NULL, WIZ_RESETS, L_DIR, buf );
	    reset_area( pArea );
	    pArea->age = number_range( 0, 3 );
	    pRoomIndex = get_room_index( ROOM_VNUM_SCHOOL );
	    if ( pRoomIndex && pArea == pRoomIndex->area )
		pArea->age = 15 - 3;
	}
    }

    return;
}



/*
 * OLC
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoom;
    int  vnum;

    for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
        if ( ( pRoom = get_room_index( vnum ) ) )
            reset_room( pRoom );
    }

    return;
}


/*
 * Take a character data from the free list and clean it out.
 */
CHAR_DATA *new_character( bool player )
{
    static PC_DATA    pcdata_zero;
           CHAR_DATA *ch;

    if ( !char_free )
    {
	ch		= alloc_perm( sizeof( CHAR_DATA ) );
    }
    else
    {
	ch		= char_free;
	char_free	= char_free->next;
    }

    clear_char( ch );

    if ( player )
    {
	if ( !pcdata_free )
	{
	    ch->pcdata  = alloc_perm( sizeof( *ch->pcdata ) );
	}
	else
	{
	    ch->pcdata  = pcdata_free;
	    pcdata_free = pcdata_free->next;
	}

	*ch->pcdata = pcdata_zero;
    }

    ch->deleted = FALSE;

    return ch;
}

AFFECT_DATA *new_affect( void )
{
    AFFECT_DATA *paf;

    if ( !affect_free )
        return alloc_perm( sizeof( AFFECT_DATA ) );

    paf		= affect_free;
    affect_free = affect_free->next;

    return paf;
}

OBJ_DATA *new_object( void )
{
    OBJ_DATA *obj;

    if ( !obj_free )
    {
        obj	 = alloc_perm( sizeof( *obj ) );
    }
    else
    {
	obj	 = obj_free;
	obj_free = obj_free->next;
    }

    memset( obj, 0, sizeof( OBJ_DATA ) );

    obj->deleted = FALSE;

    return obj;
}

EXTRA_DESCR_DATA *new_extra_descr( void )
{
    EXTRA_DESCR_DATA *ed;

    if ( !extra_descr_free )
        return alloc_perm( sizeof( EXTRA_DESCR_DATA ) );
    
    ed		     = extra_descr_free;
    extra_descr_free = extra_descr_free->next;

    return ed;

}
	


/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
    CHAR_DATA *mob;

    if ( !pMobIndex )
    {
	bug( "Create_mobile: NULL pMobIndex.", 0 );
	exit( 1 );
    }

    mob			= new_character( FALSE );

    mob->pIndexData     = pMobIndex;

    mob->name		= str_dup( pMobIndex->player_name );
    mob->short_descr	= str_dup( pMobIndex->short_descr );
    mob->long_descr	= str_dup( pMobIndex->long_descr  );
    mob->description	= str_dup( pMobIndex->description );
    mob->spec_fun	= pMobIndex->spec_fun;

    mob->level		= number_fuzzy( pMobIndex->level );
    mob->act		= pMobIndex->act;
    mob->affected_by	= pMobIndex->affected_by;
    mob->alignment	= pMobIndex->alignment;
    mob->sex		= pMobIndex->sex;
    mob->race           = pMobIndex->race;
    mob->resistant	= pMobIndex->resistant;
    mob->immune		= pMobIndex->immune;
    mob->susceptible	= pMobIndex->susceptible;
    mob->gold           = number_fuzzy( 10 )
                        * number_fuzzy( pMobIndex->level )
			* number_fuzzy( pMobIndex->level );

    mob->armor		= interpolate( mob->level, 100, -100 );

    mob->max_hit	= mob->level * 8 + number_range(
				mob->level * mob->level / 4,
				mob->level * mob->level );
    mob->hit		= mob->max_hit;
	    
    /*
     * Insert in list.
     */
    mob->next		= char_list;
    char_list		= mob;
    pMobIndex->count++;
    return mob;
}



/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, int level )
{
    OBJ_DATA *obj;

    if ( !pObjIndex )
    {
	bug( "Create_object: NULL pObjIndex.", 0 );
	exit( 1 );
    }

    obj			= new_object();

    obj->pIndexData	= pObjIndex;
    obj->in_room	= NULL;
    obj->level		= level;
    obj->wear_loc	= -1;

    obj->name		= str_dup( pObjIndex->name        );
    obj->short_descr	= str_dup( pObjIndex->short_descr );
    obj->description	= str_dup( pObjIndex->description );
    obj->spec_fun	= pObjIndex->spec_fun;
    obj->item_type	= pObjIndex->item_type;
    obj->extra_flags	= pObjIndex->extra_flags;
    obj->wear_flags	= pObjIndex->wear_flags;
    obj->value[0]	= pObjIndex->value[0];
    obj->value[1]	= pObjIndex->value[1];
    obj->value[2]	= pObjIndex->value[2];
    obj->value[3]	= pObjIndex->value[3];
    obj->value[4]	= pObjIndex->value[4];
    obj->weight		= pObjIndex->weight;
    obj->cost		= number_fuzzy( 4 )
			* number_fuzzy( level ) * number_fuzzy( level );
    obj->deleted        = FALSE;

    /*
     * Mess with object properties.
     */
    switch ( obj->item_type )
    {
    default:
	bug( "Read_object: vnum %d bad type.", pObjIndex->vnum );
	break;

    case ITEM_LIGHT:
    case ITEM_TREASURE:
    case ITEM_FURNITURE:
    case ITEM_TRASH:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_KEY:
    case ITEM_FOOD:
    case ITEM_BOAT:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
    case ITEM_FOUNTAIN:
    case ITEM_PORTAL:
    case ITEM_WARP_STONE:
    case ITEM_AMMO:
    case ITEM_GEM:
	break;

    case ITEM_SCROLL:
	obj->value[0]   = number_fuzzy( obj->value[0] );
	break;

    case ITEM_WAND:
    case ITEM_STAFF:
	obj->value[0]   = number_fuzzy( obj->value[0] );
	obj->value[1]	= number_fuzzy( obj->value[1] );
	obj->value[2]	= obj->value[1];
	break;

    case ITEM_WEAPON:
    case ITEM_RANGED_WEAPON:
	obj->value[1]   = number_fuzzy( number_fuzzy( 1 * level / 4 + 2 ) );
	obj->value[2]	= number_fuzzy( number_fuzzy( 3 * level / 4 + 6 ) );
	break;

    case ITEM_ARMOR:
	obj->value[0]   = number_fuzzy( level / 4 + 2 );
	break;

    case ITEM_CLOTHING:
	obj->value[0]   = number_fuzzy( level / 4 + 2 );
	break;

    case ITEM_POTION:
    case ITEM_PILL:
	obj->value[0]   = number_fuzzy( number_fuzzy( obj->value[0] ) );
	break;

    case ITEM_MONEY:
	obj->value[0]   = obj->cost;
	break;
    }

    obj->next		= object_list;
    object_list		= obj;
    pObjIndex->count++;

    return obj;
}



/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
    static CHAR_DATA ch_zero;

    *ch				= ch_zero;
    ch->name			= &str_empty[0];
    ch->short_descr		= &str_empty[0];
    ch->long_descr		= &str_empty[0];
    ch->description		= &str_empty[0];
    ch->last_note               = 0;
    ch->logon			= current_time;
    ch->armor			= 100;
    ch->position		= POS_STANDING;
    ch->level                   = 0;
    ch->race                    = 0;
    ch->practice		= 21;
    ch->hit			= 20;
    ch->max_hit			= 20;
    ch->mana			= 100;
    ch->max_mana		= 100;
    ch->move			= 100;
    ch->max_move		= 100;
    ch->leader                  = NULL;
    ch->master                  = NULL;
    ch->deleted                 = FALSE;
    ch->hunting			= NULL;
    ch->fearing			= NULL;
    ch->hating			= NULL;
    ch->riding			= NULL;
    ch->rider			= NULL;
    ch->resistant		= 0;
    ch->immune			= 0;
    ch->susceptible		= 0;
    return;
}



/*
 * Free a character.
 */
void free_char( CHAR_DATA *ch )
{
    OBJ_DATA    *obj;
    OBJ_DATA    *obj_next;
    AFFECT_DATA *paf;

    /*
     * This places the character in a place we know exists and allows for
     * more controled removal of mud effects and such.  Suggested by Erwin.
     */

    ch->in_room = get_room_index( ROOM_VNUM_LIMBO );

    for ( obj = ch->carrying; obj; obj = obj_next )
    {
        obj_next = obj->next_content;
        if ( obj->deleted )
	    continue;
	extract_obj( obj );
    }

    for ( paf = ch->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
	affect_remove( ch, paf );
    }

    free_string( ch->name               );
    free_string( ch->short_descr	);
    free_string( ch->long_descr		);
    free_string( ch->description	);

    if ( ch->pcdata )
    {
	free_string( ch->pcdata->pwd		);
	free_string( ch->pcdata->bamfin		);
	free_string( ch->pcdata->bamfout	);
	free_string( ch->pcdata->immskll	);
	free_string( ch->pcdata->title		);
	free_string( ch->pcdata->prompt         );
	ch->pcdata->next = pcdata_free;
	pcdata_free      = ch->pcdata;
    }

    ch->next	     = char_free;
    char_free	     = ch;
    return;
}



/*
 * Get an extra description from a list.
 */
char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
    for ( ; ed; ed = ed->next )
    {
	if ( is_name( name, ed->keyword ) )
	    return ed->description;
    }
    return NULL;
}



/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index( int vnum )
{
    MOB_INDEX_DATA *pMobIndex;

    /*
     * There is a possibility of vnum passed is negative.
     * Trapping suggested by Erwin Andreasen  -Kahn
     */

    if ( vnum < 0 )
        return NULL;

    for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH];
	  pMobIndex;
	  pMobIndex  = pMobIndex->next )
    {
	if ( pMobIndex->vnum == vnum )
	    return pMobIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_mob_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index( int vnum )
{
    OBJ_INDEX_DATA *pObjIndex;

    /*
     * There is a possibility of vnum passed is negative.
     * Trapping suggested by Erwin Andreasen  -Kahn
     */

    if ( vnum < 0 )
        return NULL;

    for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH];
	  pObjIndex;
	  pObjIndex  = pObjIndex->next )
    {
	if ( pObjIndex->vnum == vnum )
	    return pObjIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_obj_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index( int vnum )
{
    ROOM_INDEX_DATA *pRoomIndex;

    /*
     * There is a possibility of vnum passed is negative.
     * Trapping suggested by Erwin Andreasen  -Kahn
     */

    if ( vnum < 0 )
        return NULL;

    for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH];
	  pRoomIndex;
	  pRoomIndex  = pRoomIndex->next )
    {
	if ( pRoomIndex->vnum == vnum )
	    return pRoomIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_room_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Read a letter from a file.
 */
char fread_letter( FILE *fp )
{
    char c;

    do
    {
	c = getc( fp );
    }
    while ( isspace( c ) );

    return c;
}



/*
 * Read a number from a file.
 */
int fread_number( FILE *fp, int *status )
{
    int  c;
    bool sign;
    int  number;
    int  stat;

    do
    {
	c = getc( fp );
    }
    while ( isspace( c ) );

    number = 0;

    sign   = FALSE;
    if ( c == '+' )
    {
	c = getc( fp );
    }
    else if ( c == '-' )
    {
	sign = TRUE;
	c = getc( fp );
    }

    if ( !isdigit( c ) )
    {
        *status = 1;
	bug( "Fread_number: bad format.", 0 );
	bug( "   If bad object, check for missing '~' in value[] fields.", 0 );
	return 0;
    }

    while ( isdigit(c) )
    {
	number = number * 10 + c - '0';
	c      = getc( fp );
    }

    if ( sign )
	number = 0 - number;

    if ( c == '|' )
	number += fread_number( fp, &stat );
    else if ( c != ' ' )
	ungetc( c, fp );

    return number;
}



/*
 * Read to end of line (for comments).
 */
void fread_to_eol( FILE *fp )
{
    int c;

    do
    {
	c = getc( fp );
    }
    while ( c != '\n' && c != '\r' && c != EOF );

    do
    {
	c = getc( fp );
    }
    while ( c == '\n' || c == '\r' );

    ungetc( c, fp );
    return;
}



/*
 * Read one word (into static buffer).
 */
char *fread_word( FILE *fp, int *status )
{
    static char  word [ MAX_INPUT_LENGTH ];
           char *pword;
           int   cEnd;

    *status = 0;

    do
    {
	cEnd = getc( fp );
    }
    while ( isspace( cEnd ) );

    if ( cEnd == '\'' || cEnd == '"' )
    {
	pword   = word;
    }
    else
    {
	word[0] = cEnd;
	pword   = word+1;
	cEnd    = ' ';
    }

    for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
    {
	*pword = getc( fp );
	if ( cEnd == ' ' ? isspace( *pword ) : *pword == cEnd )
	{
	    if ( cEnd == ' ' )
		ungetc( *pword, fp );
	    *pword = '\0';
	    return word;
	}
    }

    bug( "Fread_word: word too long.", 0 );
    *status = 1;
    return NULL;
}



/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem( int sMem )
{
    void *pMem;
    int iList;

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
	if ( sMem <= rgSizeList[iList] )
	    break;
    }

    if ( iList == MAX_MEM_LIST )
    {
	bug( "Alloc_mem: size %d too large.", sMem );
	exit( 1 );
    }

    if ( !rgFreeList[iList] )
    {
	pMem		  = alloc_perm( rgSizeList[iList] );
    }
    else
    {
	pMem              = rgFreeList[iList];
	rgFreeList[iList] = * ( (void **) rgFreeList[iList] );
    }

    return pMem;
}



/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, int sMem )
{
    int iList;

    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
	if ( sMem <= rgSizeList[iList] )
	    break;
    }

    if ( iList == MAX_MEM_LIST )
    {
	bug( "Free_mem: size %d too large.", sMem );
	exit( 1 );
    }

    * ( (void **) pMem ) = rgFreeList[iList];
    rgFreeList[iList]  = pMem;

    return;
}



/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm( int sMem )
{
           void *pMem;
    static char *pMemPerm;
    static int   iMemPerm;

    while ( sMem % sizeof( long ) != 0 )
	sMem++;
    if ( sMem > MAX_PERM_BLOCK )
    {
	bug( "Alloc_perm: %d too large.", sMem );
	exit( 1 );
    }

    if ( !pMemPerm || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
	iMemPerm = 0;
	if ( !( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) )
	{
	    perror( "Alloc_perm" );
	    exit( 1 );
	}
    }

    pMem        = pMemPerm + iMemPerm;
    iMemPerm   += sMem;
    nAllocPerm += 1;
    sAllocPerm += sMem;
    return pMem;
}



/*
 * Translate area range values to Envy style ranges.  -Zen
 */
char *range_string( AREA_DATA *pArea )
{
    static char buf [ 512 ];

    if ( !pArea )
	return "none";

    if ( pArea->llv == 0 && pArea->ulv == MAX_LEVEL )
	sprintf( buf, " All " );
    else
	if ( pArea->llv == 0 && pArea->ulv == 0 )
	    sprintf( buf, "None " );
    else
	sprintf( buf, "%2d %2d", pArea->llv, pArea->ulv );

    return buf;
}

void do_areas( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea1;
    AREA_DATA *pArea2;
    char       buf   [ MAX_STRING_LENGTH   ];
    char       buf1  [ MAX_STRING_LENGTH*4 ];
    int        iArea;
    int        iAreaHalf;

    buf1[0] = '\0';

    iAreaHalf = ( top_area + 1 ) / 2;
    pArea1 = area_first;
    pArea2 = area_first;

    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	pArea2 = pArea2->next;

    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
    {
	sprintf( buf, "{{%5.5s} %-7.7s %-22.22s ",
		range_string( pArea1 ), pArea1->author, pArea1->name );
	strcat( buf1, buf );

	if ( pArea2 )
	{
	    sprintf( buf, "{{%5.5s} %-7.7s %-22.22s\n\r",
		range_string( pArea2 ), pArea2->author, pArea2->name );
	}
	else
	    sprintf( buf, "\n\r" );

	strcat( buf1, buf );


	pArea1 = pArea1->next;
	if ( pArea2 )
	    pArea2 = pArea2->next;
    }

    send_to_char( buf1, ch );
    return;
}



void do_memory( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       buf [ MAX_STRING_LENGTH ];

    rch = get_char( ch );
    
    if ( !authorized( rch, "memory" ) )
        return;

    sprintf( buf, "{o{cAffects %5d{x\n\r", top_affect    ); send_to_char( buf, ch );
    sprintf( buf, "{o{cAreas   %5d{x\n\r", top_area      ); send_to_char( buf, ch );
    sprintf( buf, "{o{cExDes   %5d{x\n\r", top_ed        ); send_to_char( buf, ch );
    sprintf( buf, "{o{cExits   %5d{x\n\r", top_exit      ); send_to_char( buf, ch );
    sprintf( buf, "{o{cHelps   %5d{x\n\r", top_help      ); send_to_char( buf, ch );
    sprintf( buf, "{o{cMobs    %5d{x\n\r", top_mob_index ); send_to_char( buf, ch );
    sprintf( buf, "{o{cObjs    %5d{x\n\r", top_obj_index ); send_to_char( buf, ch );
    sprintf( buf, "{o{cResets  %5d{x\n\r", top_reset     ); send_to_char( buf, ch );
    sprintf( buf, "{o{cRooms   %5d{x\n\r", top_room      ); send_to_char( buf, ch );
    sprintf( buf, "{o{cShops   %5d{x\n\r", top_shop      ); send_to_char( buf, ch );

    sprintf( buf, "{o{cPerms   %5d blocks  of %7d bytes.{x\n\r\n\r",
	    nAllocPerm, sAllocPerm );
    send_to_char( buf, ch );

    sprintf( buf, "{o{cShared String Info:{x\n\r\n\r" );
    send_to_char( buf, ch );
    sprintf( buf, "{o{cShared Strings   %5d strings of %7d bytes (max %d).{x\n\r",
	    nAllocString, sAllocString, MAX_STRING );
    send_to_char( buf, ch );
    sprintf( buf, "{o{cOverflow Strings %5d strings of %7d bytes.{x\n\r",
	    nOverFlowString, sOverFlowString );
    send_to_char( buf, ch );
    if ( Full )
    {
	sprintf( buf, "{o{rShared String Heap is full, increase MAX_STRING.{x\n\r" );
	send_to_char( buf, ch );
    }

    return;
}



/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
    switch ( number_bits( 2 ) )
    {
    case 0:  number -= 1; break;
    case 3:  number += 1; break;
    }

    return UMAX( 1, number );
}



/*
 * Generate a random number.
 */
int number_range( int from, int to )
{
    int power;
    int number;

    if ( ( to = to - from + 1 ) <= 1 )
	return from;

    for ( power = 2; power < to; power <<= 1 )
	;

    while ( ( number = number_mm( ) & ( power - 1 ) ) >= to )
	;

    return from + number;
}



/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
    int percent;

    while ( ( percent = number_mm( ) & ( 128-1 ) ) > 99 )
	;

    return 1 + percent;
}



/*
 * Generate a random door.
 */
int number_door( void )
{
    int door;

    while ( ( door = number_mm( ) & ( 8-1 ) ) > 5 )
	;

    return door;
}



int number_bits( int width )
{
    return number_mm( ) & ( ( 1 << width ) - 1 );
}



/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static	int	rgiState[2+55];

void init_mm( )
{
    int *piState;
    int  iState;

    piState	= &rgiState[2];

    piState[-2]	= 55 - 55;
    piState[-1]	= 55 - 24;

    piState[0]	= ( (int) current_time ) & ( ( 1 << 30 ) - 1 );
    piState[1]	= 1;
    for ( iState = 2; iState < 55; iState++ )
    {
	piState[iState] = ( piState[iState-1] + piState[iState-2] )
			& ( ( 1 << 30 ) - 1 );
    }
    return;
}



int number_mm( void )
{
    int *piState;
    int  iState1;
    int  iState2;
    int  iRand;

    piState		= &rgiState[2];
    iState1	 	= piState[-2];
    iState2	 	= piState[-1];
    iRand	 	= ( piState[iState1] + piState[iState2] )
			& ( ( 1 << 30 ) - 1 );
    piState[iState1]	= iRand;
    if ( ++iState1 == 55 )
	iState1 = 0;
    if ( ++iState2 == 55 )
	iState2 = 0;
    piState[-2]		= iState1;
    piState[-1]		= iState2;
    return iRand >> 6;
}



/*
 * Roll some dice.
 */
int dice( int number, int size )
{
    int idice;
    int sum;

    switch ( size )
    {
    case 0: return 0;
    case 1: return number;
    }

    for ( idice = 0, sum = 0; idice < number; idice++ )
	sum += number_range( 1, size );

    return sum;
}



/*
 * Simple linear interpolation.
 */
int interpolate( int level, int value_00, int value_47 )
{
    return value_00 + level * ( value_47 - value_00 ) / 47;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde( char *str )
{
    for ( ; *str != '\0'; str++ )
    {
	if ( *str == '~' )
	    *str = '-';
    }

    return;
}



/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp( const char *astr, const char *bstr )
{
    if ( !astr )
    {
	bug( "Str_cmp: null astr.", 0 );
	return TRUE;
    }

    if ( !bstr )
    {
	bug( "Str_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr || *bstr; astr++, bstr++ )
    {
	if ( LOWER( *astr ) != LOWER( *bstr ) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
    if ( !astr )
    {
	bug( "Str_prefix: null astr.", 0 );
	return TRUE;
    }

    if ( !bstr )
    {
	bug( "Str_prefix: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr; astr++, bstr++ )
    {
	if ( LOWER( *astr ) != LOWER( *bstr ) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
    char c0;
    int  sstr1;
    int  sstr2;
    int  ichar;

    if ( ( c0 = LOWER( astr[0] ) ) == '\0' )
	return FALSE;

    sstr1 = strlen( astr );
    sstr2 = strlen( bstr );

    for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
    {
	if ( c0 == LOWER( bstr[ichar] ) && !str_prefix( astr, bstr + ichar ) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;

    sstr1 = strlen( astr );
    sstr2 = strlen( bstr );
    if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
	return FALSE;
    else
	return TRUE;
}



/*
 * Returns an initial-capped string.
 */
char *capitalize( const char *str )
{
    static char strcap [ MAX_STRING_LENGTH ];
           int  i;

    for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = LOWER( str[i] );
    strcap[i] = '\0';
    strcap[0] = UPPER( strcap[0] );
    return strcap;
}



/*
 * Returns an all-caps string.
 */
char *all_capitalize( const char *str )
{
    static char strcap [ MAX_STRING_LENGTH ];
           int  i;

    for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = UPPER( str[i] );
    strcap[i] = '\0';
    return strcap;
}



/*
 * Append a string to a file.
 */
void append_file( CHAR_DATA *ch, char *file, char *str )
{
    FILE *fp;

    if ( IS_NPC( ch ) || str[0] == '\0' )
	return;

    fclose( fpReserve );
    if ( !( fp = fopen( file, "a" ) ) )
    {
	perror( file );
	send_to_char( "Could not open the file!\n\r", ch );
    }
    else
    {
	fprintf( fp, "[%5d] %s: %s\n",
		ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Reports a bug.
 */
void bug( const char *str, int param )
{
    FILE *fp;
    char  buf     [ MAX_STRING_LENGTH ];
    char  strsave [ MAX_INPUT_LENGTH  ];


    if ( fpArea )
    {
	int iLine;
	int iChar;

	if ( fpArea == stdin )
	{
	    iLine = 0;
	}
	else
	{
	    iChar = ftell( fpArea );
	    fseek( fpArea, 0, 0 );
	    for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
	    {
		while ( getc( fpArea ) != '\n' )
		    ;
	    }
	    fseek( fpArea, iChar, 0 );
	}

	sprintf( buf, "[*****] FILE: %s LINE: %d", strArea, iLine );
	log_string( buf );

	if ( ( fp = fopen( "SHUTDOWN.TXT", "a" ) ) )
	{
	    fprintf( fp, "[*****] %s\n", buf );
	    fclose( fp );
	}
    }

    strcpy( buf, "[*****] BUG: " );
    sprintf( buf + strlen( buf ), str, param );
    log_string( buf );

    wiznet( NULL, WIZ_DEBUG, L_DIR, buf );

    fclose( fpReserve );

    sprintf( strsave, "%s%s", SYSTEM_DIR, BUG_FILE );

    if ( ( fp = fopen( strsave, "a" ) ) )
    {
	fprintf( fp, "%s\n", buf );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}



/*
 * Reports a bug.
 * Got it from Erwin S.A. source - Zen
 */
void bugf( char * fmt, ... )
{
    char        buf [ MAX_STRING_LENGTH ];
    va_list     args;

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );

    bug ( buf, 0 );
    return;
}



/*
 * Clan warnings, info, etc.
 */
void log_clan( const char *str )
{
    FILE *fp;
    char  strsave [ MAX_INPUT_LENGTH ];

    fclose( fpReserve );

    sprintf( strsave, "%s%s", SYSTEM_DIR, CLAN_FILE );

    if ( ( fp = fopen( strsave, "a" ) ) )
    {
	fprintf( fp, "%s\n", str );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}


/*
 * Writes a string to the log.
 */
void log_string( const char *str )
{
    char *strtime;

    strtime                    = ctime( &current_time );
    strtime[strlen( strtime )-1] = '\0';
    fprintf( stderr, "%s :: %s\n", strtime, str );
    return;
}



/*
 * Writes a string to the log.
 * Got it from Erwin S.A. source - Zen
 */
void logf( char * fmt, ... )
{
    char        buf [ 2 * MAX_STRING_LENGTH ];
    va_list     args;

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );

    log_string( buf );
    return;
}



/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain( void )
{
    return;
}



/* 
 * MOBProg functions:
 * This routine transfers between alpha and numeric forms of the
 *    mob_prog bitvector types. This allows the use of the words in the
 *    mob/script files.
 */
int mprog_name_to_type ( char *name )
{
   if ( !str_cmp( name, "in_file_prog"   ) )	return IN_FILE_PROG;
   if ( !str_cmp( name, "act_prog"       ) )    return ACT_PROG;
   if ( !str_cmp( name, "speech_prog"    ) )	return SPEECH_PROG;
   if ( !str_cmp( name, "rand_prog"      ) ) 	return RAND_PROG;
   if ( !str_cmp( name, "fight_prog"     ) )	return FIGHT_PROG;
   if ( !str_cmp( name, "hitprcnt_prog"  ) )	return HITPRCNT_PROG;
   if ( !str_cmp( name, "death_prog"     ) )	return DEATH_PROG;
   if ( !str_cmp( name, "entry_prog"     ) )	return ENTRY_PROG;
   if ( !str_cmp( name, "greet_prog"     ) )	return GREET_PROG;
   if ( !str_cmp( name, "all_greet_prog" ) )	return ALL_GREET_PROG;
   if ( !str_cmp( name, "give_prog"      ) ) 	return GIVE_PROG;
   if ( !str_cmp( name, "bribe_prog"     ) )	return BRIBE_PROG;

   return( ERROR_PROG );
}



/*
 * This routine reads in scripts of MOBprograms from a file.
 */
MPROG_DATA* mprog_file_read( const char *f, MPROG_DATA *mprg,
			    MOB_INDEX_DATA *pMobIndex )
{
  char        MOBProgfile[ MAX_INPUT_LENGTH ];
  MPROG_DATA *mprg2;
  FILE       *progfile;
  char        letter;
  bool        done = FALSE;
  int         stat;

  sprintf( MOBProgfile, "%s%s", MOB_DIR, f );

  progfile = fopen( MOBProgfile, "r" );
  if ( !progfile )
  {
     bug( "Mob: %d couldnt open mobprog file", pMobIndex->vnum );
     exit( 1 );
  }

  mprg2 = mprg;
  switch ( letter = fread_letter( progfile ) )
  {
    case '>':
     break;
    case '|':
       bug( "empty mobprog file.", 0 );
       exit( 1 );
     break;
    default:
       bug( "in mobprog file syntax error.", 0 );
       exit( 1 );
     break;
  }

  while ( !done )
  {
    mprg2->type = mprog_name_to_type( fread_word( progfile, &stat ) );
    switch ( mprg2->type )
    {
     case ERROR_PROG:
        bug( "mobprog file type error", 0 );
        exit( 1 );
      break;
     case IN_FILE_PROG:
        bug( "mprog file contains a call to file.", 0 );
        exit( 1 );
      break;
     default:
        pMobIndex->progtypes = pMobIndex->progtypes | mprg2->type;
        mprg2->arglist       = fread_string( progfile, &stat );
        mprg2->comlist       = fread_string( progfile, &stat );
        switch ( letter = fread_letter( progfile ) )
        {
          case '>':
             mprg2->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
             mprg2       = mprg2->next;
             mprg2->next = NULL;
           break;
          case '|':
             done = TRUE;
           break;
          default:
             bug( "in mobprog file syntax error.", 0 );
             exit( 1 );
           break;
        }
      break;
    }
  }
  fclose( progfile );
  return mprg2;
}



void load_mobprogs( FILE *fp )
{
  MOB_INDEX_DATA *iMob;
  MPROG_DATA     *original;
  MPROG_DATA     *working;
  char            letter;
  int             value;
  int             stat;

  for ( ; ; )
    switch ( letter = fread_letter( fp ) )
    {
    default:
      bug( "Load_mobprogs: bad command '%c'.",letter);
      exit(1);
      break;
    case 'S':
    case 's':
      fread_to_eol( fp ); 
      return;
    case '*':
      fread_to_eol( fp ); 
      break;
    case 'M':
    case 'm':
      value = fread_number( fp, &stat );
      if ( !( iMob = get_mob_index( value ) ) )
      {
	bug( "Load_mobprogs: vnum %d doesnt exist", value );
	exit( 1 );
      }
    
      /* Go to the end of the prog command list if other commands exist */

      if ( ( original = iMob->mobprogs ) )
	for ( ; original->next; original = original->next );

      working = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
      if ( original )
	original->next = working;
      else
	iMob->mobprogs = working;
      working       = mprog_file_read( fread_word( fp, &stat ), working, iMob );
      working->next = NULL;
      fread_to_eol( fp );
      break;
    }

  return;

} 



void mprog_read_programs( FILE *fp, MOB_INDEX_DATA *pMobIndex )
{
  MPROG_DATA *mprg;
  char        buf [MAX_STRING_LENGTH];
  char        letter;
  bool        done = FALSE;
  int         stat;

  if ( ( letter = fread_letter( fp ) ) != '>' )
  {
      bug( "Load_mobiles: vnum %d MOBPROG char", pMobIndex->vnum );
      exit( 1 );
  }
  pMobIndex->mobprogs = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
  mprg = pMobIndex->mobprogs;

  while ( !done )
  {
    mprg->type = mprog_name_to_type( fread_word( fp, &stat ) );
    switch ( mprg->type )
    {
     case ERROR_PROG:
        bug( "Load_mobiles: vnum %d MOBPROG type.", pMobIndex->vnum );
        exit( 1 );
      break;
     case IN_FILE_PROG:
	temp_fread_string( fp, buf );
        mprg	 = mprog_file_read( buf, mprg, pMobIndex );
        fread_to_eol( fp );
        switch ( letter = fread_letter( fp ) )
        {
          case '>':
             mprg->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
             mprg       = mprg->next;
             mprg->next = NULL;
           break;
          case '|':
             mprg->next = NULL;
             fread_to_eol( fp );
             done = TRUE;
           break;
          default:
             bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
             exit( 1 );
           break;
        }
      break;
     default:
        pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
        mprg->arglist        = fread_string( fp, &stat );
        fread_to_eol( fp );
        mprg->comlist        = fread_string( fp, &stat );
        fread_to_eol( fp );
        switch ( letter = fread_letter( fp ) )
        {
          case '>':
             mprg->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
             mprg       = mprg->next;
             mprg->next = NULL;
           break;
          case '|':
             mprg->next = NULL;
             fread_to_eol( fp );
             done = TRUE;
           break;
          default:
             bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
             exit( 1 );
           break;
        }
      break;
    }
  }

  return;

}



/*
 * These are here because of ColourUp's problem with {'s
 * You see, in ColourUp '{' is an escape like '\' in C for example:
 *  "{rRed" would print "Red" in red color.
 * So the '{' in area names would vannish unless i replace them
 * with '{{'				(Zen)
 */
void conv_braces( char *buffer, const char *str )
{
    const	char	*point;

    if( str )
    {
	    for( point = str ; *point ; point++ )
	    {
		*buffer = *point;
		if( *point == '{' && *point+1 != '{' )
		    *++buffer = '{';
		*++buffer = '\0';
	    }
	    *buffer = '\0';
    }

    return;
}



/* Not tested yet. May be useful later on :-) (Zen) */
void unconv_braces( char *buffer, const char *str )
{
    const	char	*point;

    if( str )
    {
	    for( point = str ; *point ; point++ )
	    {
		if( *point == '{' && *point+1 == '{' )
		    continue;
		*buffer = *point;
		*++buffer = '\0';
	    }
	    *buffer = '\0';
    }

    return;
}
