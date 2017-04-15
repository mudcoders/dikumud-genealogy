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
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#define unix 1
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#if !defined( macintosh )
extern  int     _filbuf	        args( (FILE *) );
#endif

#if !defined( ultrix )
#include <memory.h>
#endif

/*
 * Globals.
 */

NEWBIE_DATA *		newbie_first;
NEWBIE_DATA *		newbie_last;

HELP_DATA *		help_first;
HELP_DATA *		help_last;

SOCIAL_DATA *		social_first;
SOCIAL_DATA *		social_last;

RACE_DATA *		first_race;

SHOP_DATA *		shop_first;
SHOP_DATA *		shop_last;

CHAR_DATA *		char_free;
EXTRA_DESCR_DATA *	extra_descr_free;
NOTE_DATA *		note_free;
OBJ_DATA *		obj_free;
PC_DATA *		pcdata_free;

char                    bug_buf                 [ MAX_INPUT_LENGTH*2 ];
CHAR_DATA *		char_list;
PLAYERLIST_DATA	*	playerlist; /*Decklarean */
char *			help_greeting;
char	                log_buf                 [ MAX_INPUT_LENGTH*2 ];
KILL_DATA	        kill_table              [ MAX_LEVEL          ];
NOTE_DATA *		note_list;
OBJ_DATA *		object_list;
TIME_INFO_DATA		time_info;
WEATHER_DATA		weather_info;
ARENA_DATA		arena;
WAR_DATA		war;
char *                  down_time;
char *                  warning1;
char *                  warning2;
int                     stype;

int			gsn_trip;
int			gsn_dirt_kick;
int			gsn_track;
int			gsn_shield_block;
int			gsn_drain_life;
int			gsn_mental_drain;
int			gsn_turn_evil;
int			gsn_bash_door;
int                     gsn_stun;
int                     gsn_berserk;

int			gsn_backstab;
int			gsn_backstab_2;
int			gsn_palm;
int			gsn_dodge;
int                     gsn_dual;
int                     gsn_grip;
int			gsn_hide;
int			gsn_peek;
int			gsn_pick_lock;
int                     gsn_punch;
int			gsn_jab_punch;
int			gsn_kidney_punch;
int			gsn_cross_punch;
int			gsn_roundhouse_punch;
int			gsn_uppercut_punch;
int			gsn_sneak;
int			gsn_steal;
int			gsn_disarm;
int                     gsn_poison_weapon;

int                     gsn_bash;
int			gsn_enhanced_damage;
int                     gsn_enhanced_two;
int                     gsn_enhanced_hit;
int			gsn_flury;
int			gsn_kick;
int			gsn_high_kick;
int			gsn_spin_kick;
int			gsn_jump_kick;
int                     gsn_circle;
int                     gsn_feed;
int			gsn_parry;
int			gsn_rescue;
int                     gsn_patch;
int			gsn_second_attack;
int			gsn_third_attack;
int                     gsn_fourth_attack;
int                     gsn_fifth_attack;
int                     gsn_sixth_attack; 
int                     gsn_seventh_attack;
int                     gsn_eighth_attack;
int                     gsn_gouge;
int                     gsn_alchemy;
int                     gsn_scribe;
int			gsn_blindness;
int			gsn_charm_person;
int			gsn_curse;
int                     gsn_incinerate;
int			gsn_invis;
int			gsn_mass_invis;
int			gsn_poison;
int			gsn_sleep;
int                     gsn_doomshield;
int                     gsn_unholystrength;
int                     gsn_image;
int                     gsn_prayer;
int                     gsn_soulstrike;
int			gsn_spellcraft;
int                	gsn_scrolls;
int                	gsn_staves;
int                	gsn_wands;
int			gsn_gravebind;
int			gsn_multiburst;
int			gsn_enhanced_claw;
int			gsn_dualclaw;
int			gsn_fastheal;
int			gsn_rage;
int			gsn_bite;
int			gsn_rush;
int			gsn_howlfear;
int			gsn_scent;
int			gsn_frenzy;
int			gsn_reflex;
int			gsn_rake;
int			gsn_headbutt;
int			gsn_flamehand;
int			gsn_frosthand;
int			gsn_chaoshand;
int			gsn_bladepalm;
int			gsn_flykick;
int			gsn_anatomyknow;
int			gsn_blackbelt;
int			gsn_nerve;
int			gsn_ironfist;
int			gsn_globedark;
int			gsn_drowfire;
int			gsn_snatch;
int			gsn_enhanced_three;
int			gsn_retreat;
int			gsn_antidote;
int			gsn_haggle;
int			gsn_dodge_two;
int			gsn_blindfight;
int			gsn_shriek;
int			gsn_spit;
int			gsn_ward_safe;
int			gsn_ward_heal;
int			gsn_throw;   
int                     gsn_shadow_walk;
/*
 * Psionicist gsn's.
 */
int                     gsn_chameleon;
int                     gsn_domination;
int                     gsn_heighten;
int                     gsn_shadow;
int						gsn_lure;
int						gsn_image;
int						gsn_flip;
int						gsn_hallucinate;
int						gsn_plague;
int						gsn_unholystrength;

int                     port;


/*
 * Locals.
 */
MOB_INDEX_DATA *	mob_index_hash	        [ MAX_KEY_HASH       ];
OBJ_INDEX_DATA *	obj_index_hash	        [ MAX_KEY_HASH       ];
ROOM_INDEX_DATA *	room_index_hash         [ MAX_KEY_HASH       ];
char *			string_hash	        [ MAX_KEY_HASH       ];

AREA_DATA *		area_first;
AREA_DATA *		area_last;
CLAN_DATA *             clan_first;

char *			string_space;
char *			top_string;
char			str_empty	        [ 1                  ];


int      		num_mob_progs;
int			num_trap_progs;

int			mobs_in_game;
int			top_affect;
int			top_area;
int			top_ed;
int			top_exit;
int			top_help;
int			top_mob_index;
int			top_obj_index;
int			top_clan;
int			top_social;
int			top_race;
int			top_newbie;
int			top_reset;
int			top_room;
int			top_shop;
int 			top_trap;
int			top_mob_prog;
/*int			top_vnum_room;*/	/* OLC */
/*int			top_vnum_mob; */	/* OLC */
/*int			top_vnum_obj; */  /* OLC */
int 		 	mprog_name_to_type	args ( ( char* name ) );
MPROG_DATA *		mprog_file_read 	args ( ( char* f, MPROG_DATA* mprg, 
						MOB_INDEX_DATA *pMobIndex ) );
void			load_mobprogs           args ( ( FILE* fp ) );
void   			mprog_read_programs     args ( ( FILE* fp,
					        MOB_INDEX_DATA *pMobIndex ) );
void                    load_traps              args ( ( FILE* fp,
							OBJ_INDEX_DATA *pObj,
							ROOM_INDEX_DATA *pRoom,
							EXIT_DATA *pExit ) );
char *			wind_str		args ( ( int str ) );
void                    area_sort               args ( ( AREA_DATA *pArea ) );
void                    clan_sort               args ( ( CLAN_DATA *pClan ) );
void add_playerlist    args ( (CHAR_DATA *ch) );
void update_playerlist args ( ( CHAR_DATA *ch ) );
void load_player_list  args ( ( ) );
void load_race 		args ( ( ) );
/*
 * Memory management.
 * Increase MAX_STRING from 1500000 if you have too.
 * Tune the others only if you understand what you're doing.
 */
#define			MAX_STRING      3500000

#if defined( machintosh )
#define			MAX_PERM_BLOCK  131072
#define			MAX_MEM_LIST    11

void *			rgFreeList              [ MAX_MEM_LIST       ];
const int		rgSizeList              [ MAX_MEM_LIST       ]  =
{
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768-64
};
#else
#define			MAX_PERM_BLOCK  131072
#define			MAX_MEM_LIST    12

void *			rgFreeList              [ MAX_MEM_LIST       ];
const int		rgSizeList              [ MAX_MEM_LIST       ]  =
{
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768, 65536
};
#endif


int			nAllocString;
int			sAllocString;
int			nAllocPerm;
int			sAllocPerm;
int 			MemAllocated;
int 			MemFreed;



/*
 * Semi-locals.
 */
bool			fBootDb;
FILE *			fpArea;
char			strArea                 [ MAX_INPUT_LENGTH   ];
bool 			quest = FALSE;
int			qmin = 0;
int			qmax = 100;


/*
 * Local booting procedures.
 */
void	init_mm		args( ( void ) );

void	load_area       args( ( FILE *fp ) );
void	load_helps      args( ( FILE *fp ) );
void    load_recall     args( ( FILE *fp ) );
void	load_mobiles    args( ( FILE *fp ) );
void	load_objects    args( ( FILE *fp ) );
void	load_resets     args( ( FILE *fp ) );
void	load_rooms      args( ( FILE *fp ) );
void	load_shops      args( ( FILE *fp ) );
void	load_specials   args( ( FILE *fp ) );
void	load_notes      args( ( void ) );
void    load_clans      args( ( void ) );
void    load_down_time  args( ( void ) );
void	fix_exits       args( ( void ) );

void	reset_area      args( ( AREA_DATA * pArea ) );

/*
 * Non-Envy Loading procedures.
 * Put any new loading function in this section.
 */
void	new_load_area	args( ( FILE *fp ) );	/* OLC */
void	new_load_rooms	args( ( FILE *fp ) );	/* OLC 1.1b */
void	load_banlist	args( ( void ) );
void	load_socials	args( ( void ) );       /* Decklarean */
void    load_newbie	args( ( void ) );	/* Angi	      */


/*
 * Big mama top level function.
 */
void boot_db( void )
{

char buf[MAX_STRING_LENGTH];
int debug = 0;
    /*
     * Init some data space stuff.
     */
    {
	if ( !( string_space = calloc( 1, MAX_STRING ) ) )
	{
	    bug( "Boot_db: can't alloc %d string space.", MAX_STRING );
	    exit( 1 );
	}
	top_string	= string_space;
	fBootDb		= TRUE;
    }

    /*
     * Init random number generator.
     */
    {
	init_mm( );
    }

    /*
     * Set time and weather.
     */
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

	     if ( time_info.hour < 1 ) weather_info.sunlight = SUN_MIDNIGHT;
	else if ( time_info.hour < 5 ) weather_info.sunlight = SUN_DARK;
	else if ( time_info.hour <  6 ) weather_info.sunlight = SUN_DAWN;
	else if ( time_info.hour <  11 ) weather_info.sunlight = SUN_MORNING;
	else if ( time_info.hour < 13 ) weather_info.sunlight = SUN_NOON;
	else if ( time_info.hour < 18 ) weather_info.sunlight = SUN_AFTERNOON;
	else if ( time_info.hour < 19 ) weather_info.sunlight = SUN_DUSK;
	else if ( time_info.hour < 24 ) weather_info.sunlight = SUN_EVENING;
	else                            weather_info.sunlight = SUN_DARK;

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
     */
    {
	int sn;

	for ( sn = 0; skill_table[sn].name[0] != '\0'; sn++ )
	{
	    if ( skill_table[sn].pgsn )
		*skill_table[sn].pgsn = sn;
	}
    }

    /*
     * Read in all the area files.
     */
    {
	FILE *fpList;

	if ( !( fpList = fopen( AREA_LIST, "r" ) ) )
	{
	    perror( AREA_LIST );
	    exit( 1 );
	}
	
	for ( ; ; )
	{
	    fpArea = fpList;
	    strcpy(strArea, AREA_LIST);
	    strcpy( strArea, fread_word( fpList ) );
 
            if ( debug )
            { 
                sprintf(buf, "loading area %s", strArea);
 	        bug(buf, 0);
            }

	    if ( strArea[0] == '$' )
		break;

	    if ( strArea[0] == '-' )
	    {
		fpArea = stdin;
	    }
	    else
	    {
		if ( !( fpArea = fopen( strArea, "r" ) ) )
		{
                    /* Let's see if the area file is zipped */
           /*         sprintf ( strArea, "%s.zip", StrArea );
                    if ( ( fpArea = fopen ( strArea, "r" ) ) )
                    {
		        fclose(fpArea);
  			sprintf( buf, "unzip %s", strArea" );
                        system ( buf );		
                    }
                    else
                    {*/
		       perror( strArea );
		       continue;
		  /*  }*/
		}
	    }

	    for ( ; ; )
	    {
		char *word;

		if ( fread_letter( fpArea ) != '#' )
		{
		    bug( "Boot_db: # not found.", 0 );
		    break;
		}

		word = fread_word( fpArea );

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
		else if ( !str_cmp( word, "AREADATA" ) )	/* OLC */
		    new_load_area( fpArea );
		else if ( !str_cmp( word, "ROOMDATA" ) )	/* OLC 1.1b */
		    new_load_rooms( fpArea );
		else
		{
		    bug( "Boot_db: bad section name.", 0 );
		    break;
		}
	    }
{
char buf[MAX_STRING_LENGTH];
sprintf(buf, "loading %s...", strArea );
log_string( buf, CHANNEL_NONE , -1 );
}
	    if ( fpArea != stdin )
		fclose( fpArea );
	    fpArea = NULL;
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
       fix_exits( );
       load_banlist( ); 
       load_clans( ); 
       load_socials( ); 
       load_newbie( ); 
       load_player_list( ); 
       load_race( ); 
       load_notes( );
       load_down_time( );

       fpArea = NULL;
       strcpy(strArea, "$");
       fBootDb = FALSE;

       area_update( );
}
MOBtrigger = TRUE;
return;
}


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

/* This routine reads in scripts of MOBprograms from a file */

MPROG_DATA* mprog_file_read( char *f, MPROG_DATA *mprg,
			    MOB_INDEX_DATA *pMobIndex )
{

  char        MOBProgfile[ MAX_INPUT_LENGTH ];
  MPROG_DATA *mprg2;
  FILE       *progfile;
  char        letter;
  bool        done = FALSE;

/*  sprintf( MOBProgfile, "%s%s", MOB_DIR, f );
*/
  progfile = fopen( MOBProgfile, "r" );
  if ( !progfile )
  {
     bug( "Mob: %d couldnt open mobprog file", pMobIndex->vnum );
     exit( 1 );
  }
  fpArea = progfile;
  strcpy(strArea, MOBProgfile);

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
    mprg2->type = mprog_name_to_type( fread_word( progfile ) );
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
        mprg2->arglist       = fread_string( progfile );
        mprg2->comlist       = fread_string( progfile );
        num_mob_progs++;
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

/* Snarf a MOBprogram section from the area file.
 */
void load_mobprogs( FILE *fp )
{
  MOB_INDEX_DATA *iMob;
  MPROG_DATA     *original;
  MPROG_DATA     *working;
  char            letter;
  int             value;

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
      value = fread_number( fp );
      if ( ( iMob = get_mob_index( value ) ) == NULL )
      {
	bug( "Load_mobprogs: vnum %d doesnt exist", value );
	exit( 1 );
      }
    
      /* Go to the end of the prog command list if other commands
         exist */

      if ( ( original = iMob->mobprogs ) )
	for ( ; original->next != NULL; original = original->next );

      working = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
      if ( original )
	original->next = working;
      else
	iMob->mobprogs = working;
      working       = mprog_file_read( fread_word( fp ), working, iMob );
      working->next = NULL;
      fread_to_eol( fp );
      break;
    }

  return;

} 

/* This procedure is responsible for reading any in_file MOBprograms.
 */

void mprog_read_programs( FILE *fp, MOB_INDEX_DATA *pMobIndex)
{
  MPROG_DATA *mprg;
  char        letter;
  bool        done = FALSE;

  if ( ( letter = fread_letter( fp ) ) != '>' )
  {
      bug( "Mprog_read_programs: vnum %d MOBPROG char", pMobIndex->vnum );
      exit( 1 );
  }
  pMobIndex->mobprogs = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
  mprg = pMobIndex->mobprogs;

  while ( !done )
  {
    mprg->type = mprog_name_to_type( fread_word( fp ) );
    switch ( mprg->type )
    {
     case ERROR_PROG:
        bug( "Load_mobiles: vnum %d MOBPROG type.", pMobIndex->vnum );
        exit( 1 );
      break;
     case IN_FILE_PROG:
        mprg = mprog_file_read( fread_string( fp ), mprg,pMobIndex );
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
        mprg->arglist        = fread_string( fp );
        fread_to_eol( fp );
        mprg->comlist        = fread_string( fp );
        fread_to_eol( fp );
        num_mob_progs++;
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

void load_traps( FILE *fp, OBJ_INDEX_DATA *pObj, ROOM_INDEX_DATA *pRoom,
	         EXIT_DATA *pExit )
{
  TRAP_DATA *pTrap;
  TRAP_DATA *pFirst;
  int *traptypes;
  char letter;
  bool done = FALSE;

  if ( ( letter = fread_letter( fp ) ) != '>' )
  {
    bug("Load_traps:  No Traps", 0);
    exit(1);
  }

  if ( !trap_list )
  {
    trap_list = (TRAP_DATA *)alloc_perm( sizeof( TRAP_DATA ));
    pFirst = trap_list;
  }
  else
  {
    for( pFirst = trap_list; pFirst->next; pFirst = pFirst->next );
    pFirst->next = (TRAP_DATA *)alloc_perm( sizeof( TRAP_DATA ));
    pFirst = pFirst->next;
  }
  pFirst->next = NULL;
  pFirst->next_here = NULL;
    
  if ( pObj )
  {
    pObj->traps = pFirst;
    traptypes = &pObj->traptypes;
  }
  else if ( pRoom )
  {
    pRoom->traps = pFirst;
    traptypes = &pRoom->traptypes;
  }
  else if ( pExit )
  {
    pExit->traps = pFirst;
    traptypes = &pExit->traptypes;
  }
  else
  {
    bug("Load_traps:  Nothing to load to!", 0);
    exit( 1 );
  }
  pTrap = pFirst;

  while( !done )
  {
    pTrap->type = flag_value( (pObj ? oprog_types : (pRoom ? rprog_types :
			       eprog_types)), fread_word( fp ) );
    switch( pTrap->type )
    {
    case (pObj ? OBJ_TRAP_ERROR : (pRoom ? ROOM_TRAP_ERROR : EXIT_TRAP_ERROR)):
    case NO_FLAG:
      bug( "Load_traps: No flag found.", 0 );
      exit( 1 );
    default:
      pTrap->on_obj = pObj;
      pTrap->in_room = pRoom;
      pTrap->on_exit = pExit;
      *traptypes |= pTrap->type;
      pTrap->arglist = fread_string( fp );
      fread_to_eol( fp );
      pTrap->disarmable = fread_number( fp );
      fread_to_eol( fp );
      pTrap->comlist = fread_string( fp );
      fread_to_eol( fp );
      num_trap_progs++;
      
      switch ( letter = fread_letter( fp ) )
      {
      case '>':
	pTrap->next = alloc_perm( sizeof( TRAP_DATA ));
	pTrap->next_here = pTrap->next;
	pTrap = pTrap->next;
	pTrap->next = NULL;
	pTrap->next_here = NULL;
	break;
      case '|':
	pTrap->next = NULL;
	pTrap->next_here = NULL;
	fread_to_eol( fp );
	done = TRUE;
	break;
      default:
	bug( "Load_traps:  bad TRAP", 0);
	break;
      }
      break;
    }
  }
  return;
}


/*
 * Snarf an 'area' header line.
 */
void load_area( FILE *fp )
{
    AREA_DATA *pArea;

    pArea		= alloc_perm( sizeof( *pArea ) );	/*
    pArea->reset_first	= NULL;					 * OLC-Removed
    pArea->reset_last	= NULL;					 */
    pArea->name		= fread_string( fp );
    pArea->recall       = ROOM_VNUM_TEMPLE;
    pArea->area_flags   = AREA_LOADING;		/* OLC */
    pArea->security     = 1;			/* OLC */
    pArea->builders     = str_dup( "None" );	/* OLC */
    pArea->lvnum        = 0;			/* OLC */
    pArea->uvnum        = 0;			/* OLC */
    pArea->vnum		= top_area;		/* OLC */
    pArea->filename	= str_dup( strArea );	/* OLC */
    pArea->age		= 15;
    pArea->nplayer	= 0;
    pArea->def_color	= 6;  /* Angi - AT_CYAN */

/*    if ( !area_first )
	area_first = pArea;
    if (  area_last  )
    {
	area_last->next = pArea;
	REMOVE_BIT(area_last->area_flags, AREA_LOADING);	* OLC *
    }
    area_last	= pArea;
    pArea->next	= NULL;*/
    area_sort(pArea);

    top_area++;
    return;
}



/*
 * OLC
 * Use these macros to load any new area formats that you choose to
 * support on your MUD.  See the new_load_area format below for
 * a short example.
 */
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
                    field = fread_string( fp );     \
                    fMatch = TRUE;                  \
                    break;                          \
		}



/* OLC
 * Snarf an 'area' header line.   Check this format.  MUCH better.  Add fields
 * too.
 *
 * #AREAFILE
 * Name   { All } Locke    Newbie School~
 * Repop  A teacher pops in the room and says, 'Repop coming!'~
 * Recall 3001
 * End
 */
void new_load_area( FILE *fp )
{
    AREA_DATA *pArea;
    char      *word;
    bool      fMatch;

    pArea               = alloc_perm( sizeof(*pArea) );
    pArea->age          = 15;
    pArea->nplayer      = 0;
    pArea->filename     = str_dup( strArea );
    pArea->vnum         = top_area;
    pArea->name         = str_dup( "New Area" );
    pArea->builders     = str_dup( "" );
    pArea->reset_sound  = NULL;
    pArea->security     = 1;
    pArea->lvnum        = 0;
    pArea->uvnum        = 0;
    pArea->llevel	= 0;
    pArea->ulevel	= 0;
    pArea->area_flags   = 0;
    pArea->recall       = ROOM_VNUM_TEMPLE;
    pArea->def_color 	= 6;  /* Angi - AT_CYAN */

    for ( ; ; )
    {
       word   = feof( fp ) ? "End" : fread_word( fp );
       fMatch = FALSE;

       switch ( UPPER(word[0]) )
       {
	   case 'L':
             if ( !str_cmp( word, "Levels" ) )
             {
                 pArea->llevel = fread_number( fp );
		 pArea->ulevel = fread_number( fp );
	     }
	    break;
           case 'N':
            SKEY( "Name", pArea->name );
            break;
           case 'S':
             KEY( "Security", pArea->security, fread_number( fp ) );
	    SKEY( "Sounds", pArea->reset_sound );
            break;
           case 'V':
            if ( !str_cmp( word, "VNUMs" ) )
            {
                pArea->lvnum = fread_number( fp );
                pArea->uvnum = fread_number( fp );
                /* Set up the arena. */
                if ( pArea->lvnum <= ROOM_ARENA_VNUM &&
                     pArea->uvnum >= ROOM_ARENA_VNUM )
                  arena.area = pArea;
            }
            break;
           case 'F':
            KEY( "Flags", pArea->area_flags, fread_number( fp ) );
            break;
           case 'E':
             if ( !str_cmp( word, "End" ) )
             {
                 fMatch = TRUE;
/*                 if ( area_first == NULL )
                    area_first = pArea;
                 if ( area_last  != NULL )
                    area_last->next = pArea;
                 area_last   = pArea;
                 pArea->next = NULL;*/
		 area_sort(pArea);
                 top_area++;
                 return;
            }
            break;
           case 'B':
            SKEY( "Builders", pArea->builders );
            break;
	   case 'C':
	    KEY( "Color", pArea->def_color, fread_number( fp ) );
	    break;
           case 'R':
             KEY( "Recall", pArea->recall, fread_number( fp ) );
            break;
	}
    }
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

    for ( ; ; )
    {
	pHelp		= alloc_perm( sizeof( *pHelp ) );
/*	pHelp->area	= area_last ? area_last : NULL;	 not used Deck*/ /* OLC 1.1b */
	pHelp->level	= fread_number( fp );
	pHelp->keyword	= fread_string( fp );
	if ( pHelp->keyword[0] == '$' )
	    break;
	pHelp->text	= fread_string( fp );

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

    pArea         = area_last;
    pArea->recall = fread_number( fp );

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
    char letter;
    int num;

    if ( !area_last )	/* OLC */
    {
	bug( "Load_mobiles: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	int  vnum;
	int  iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_mobiles: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
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
	pMobIndex->area			= area_last;		/* OLC */
	pMobIndex->player_name		= fread_string( fp );
	pMobIndex->short_descr		= fread_string( fp );
	pMobIndex->long_descr		= fread_string( fp );
	pMobIndex->description		= fread_string( fp );

	pMobIndex->long_descr[0]	= UPPER( pMobIndex->long_descr[0]  );
	pMobIndex->description[0]	= UPPER( pMobIndex->description[0] );

	pMobIndex->act			= fread_number( fp ) | ACT_IS_NPC;
	pMobIndex->affected_by		= fread_number( fp );
	pMobIndex->affected_by2         = fread_number( fp );
	pMobIndex->pShop		= NULL;
	pMobIndex->alignment		= fread_number( fp );
	letter				= fread_letter( fp );
	pMobIndex->level		= fread_number( fp );

	/*
	 * The unused stuff is for imps who want to use the old-style
	 * stats-in-files method.
	 */
	pMobIndex->hitroll              = fread_number( fp );   /* Unused */
	pMobIndex->ac                   = fread_number( fp );   /* Unused */
	pMobIndex->hitnodice            = fread_number( fp );   /* Unused */
	/* 'd'		*/                fread_letter( fp );   /* Unused */
	pMobIndex->hitsizedice          = fread_number( fp );   /* Unused */
	/* '+'		*/                fread_letter( fp );   /* Unused */
	pMobIndex->hitplus		= fread_number( fp );   /* Unused */
	pMobIndex->damnodice            = fread_number( fp );   /* Unused */
	/* 'd'		*/                fread_letter( fp );   /* Unused */
	pMobIndex->damsizedice          = fread_number( fp );   /* Unused */
	/* '+'		*/                fread_letter( fp );   /* Unused */
	pMobIndex->damplus              = fread_number( fp );   /* Unused */
#ifndef NEW_MONEY 
	pMobIndex->gold                 = fread_number( fp );   /* Unused */

	/* gold		*/		  fread_number( fp );
#else

        pMobIndex->money.gold           = fread_number( fp );
        pMobIndex->money.silver         = fread_number( fp );
        pMobIndex->money.copper         = fread_number( fp );
#endif
	/* xp           */                fread_number( fp );   /* Unused */
	/* position	*/                fread_number( fp );   /* Unused */
	/* start pos	*/                fread_number( fp );   /* Unused */
	pMobIndex->sex			= fread_number( fp );
	pMobIndex->imm_flags            = fread_number( fp );
	pMobIndex->res_flags            = fread_number( fp );
	pMobIndex->vul_flags            = fread_number( fp );

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
/* XORPHOX */
/* This stuff is already in there Xor.. *POKE*
 * Grumble.. now we hafta leave it here or it'll cause errors
 * Sheesh.. -- Altrag
 */
	for ( ; ; ) /* only way so far */
	{
	    letter = fread_letter( fp );

	    if ( letter == 'F' )
	    {
              num = fread_number(fp);
              switch(num)
              {
           /*     case 0:*/ /* affected_by2 */
/*                  pMobIndex->affected_by2	= fread_number(fp);
                break;*/
                case 1: /* imm_flags */
                  pMobIndex->imm_flags		= fread_number(fp);
                break;
                case 2: /* res_flags */
                  pMobIndex->res_flags		= fread_number(fp);
                break;
                case 3: /* vul_flags */
                  pMobIndex->vul_flags		= fread_number(fp);
                break;
              }
	    }
	    else
	    {
		ungetc(letter, fp);
		break;
	    }
	}
/* END */

	iHash			= vnum % MAX_KEY_HASH;
	pMobIndex->next		= mob_index_hash[iHash];
	mob_index_hash[iHash]	= pMobIndex;
	top_mob_index++;
/*	top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob ;*/  /* OLC */
	assign_area_vnum( vnum );				   /* OLC */
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

    if ( !area_last )	/* OLC */
    {
	bug( "Load_objects: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
        char *value [ 4 ];
/*	char *spell;*/
	char  letter;
	int   vnum;
	int   iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_objects: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_obj_index( vnum ) )
	{
	    bug( "Load_objects: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pObjIndex=/*new_obj_index();*/alloc_perm(sizeof(*pObjIndex )); 
	pObjIndex->vnum			= vnum;
        pObjIndex->area			= area_last;		/* OLC */
	pObjIndex->name			= fread_string( fp );
	pObjIndex->short_descr		= fread_string( fp );
	pObjIndex->description		= fread_string( fp );
	/* Action description */	  fread_string( fp );

	pObjIndex->short_descr[0]	= LOWER( pObjIndex->short_descr[0] );
	pObjIndex->description[0]	= UPPER( pObjIndex->description[0] );

	pObjIndex->item_type		= fread_number( fp );
	pObjIndex->extra_flags		= fread_number( fp );
	pObjIndex->anti_class_flags	= fread_number( fp );
	pObjIndex->anti_race_flags	= fread_number( fp );
	pObjIndex->wear_flags		= fread_number( fp );
        pObjIndex->level                = fread_number( fp );
	value[0]		        = fread_string( fp );
	value[1]		        = fread_string( fp );
	value[2]		        = fread_string( fp );
	value[3]		        = fread_string( fp );
	pObjIndex->weight		= fread_number( fp );
#ifdef NEW_MONEY
	pObjIndex->cost.gold		= fread_number( fp );
	pObjIndex->cost.silver		= fread_number( fp );
	pObjIndex->cost.copper		= fread_number( fp );

#else
	pObjIndex->cost			= fread_number( fp );
	/* Obj cost */                    fread_number( fp ); 
#endif
        /* Cost per day */ 		  fread_number( fp );   /* Unused */
	pObjIndex->ac_type              = fread_number( fp );
	pObjIndex->ac_vnum              = fread_number( fp );
	pObjIndex->ac_spell             = fread_string( fp );
	pObjIndex->ac_charge[0]         = fread_number( fp );
	pObjIndex->ac_charge[1]         = fread_number( fp ); 
	pObjIndex->join                 = fread_number( fp );
	pObjIndex->sep_one              = fread_number( fp );
	pObjIndex->sep_two              = fread_number( fp );
	/*
	 * Check here for the redundancy of invisible light sources - Kahn
	 */
	if ( pObjIndex->item_type == ITEM_LIGHT
	    && IS_SET( pObjIndex->extra_flags, ITEM_INVIS ) )
	{
	    bug( "Vnum %d : light source with ITEM_INVIS set", vnum );
	    REMOVE_BIT( pObjIndex->extra_flags, ITEM_INVIS );
	}

	if ( pObjIndex->item_type == ITEM_POTION )
	    SET_BIT( pObjIndex->extra_flags, ITEM_NO_LOCATE );
	    
	for ( ; ; )
	{
	    char letter;

	    letter = fread_letter( fp );

	    if ( letter == 'A' )
	    {
		AFFECT_DATA *paf;

		paf			= new_affect();/*alloc_perm(sizeof( *paf ));*/
		paf->type		= -1;
		paf->level		= pObjIndex->level;
		paf->duration		= -1;
		paf->location		= fread_number( fp );
		paf->modifier		= fread_number( fp );
		paf->bitvector		= 0;
		paf->next		= pObjIndex->affected;
		pObjIndex->affected	= paf;
/*		top_affect++;*/
	    }

	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= new_extra_descr(); /*alloc_perm(sizeof( *ed ));*/
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= pObjIndex->extra_descr;
		pObjIndex->extra_descr	= ed;
/*		top_ed++;*/
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
	    break;

	case ITEM_PILL:
	case ITEM_POTION:
	case ITEM_SCROLL:
	    pObjIndex->value[0] = atoi( value[0] );
	    pObjIndex->value[1] = skill_lookup( value[1] );
	    pObjIndex->value[2] = skill_lookup( value[2] );
	    pObjIndex->value[3] = skill_lookup( value[3] );
	    break;

	case ITEM_PORTAL:
	    pObjIndex->value[0] = atoi( value[0] );
	    break;
	    
	case ITEM_STAFF:
	case ITEM_LENSE:
	case ITEM_WAND:
	    pObjIndex->value[0] = atoi( value[0] );
	    pObjIndex->value[1] = atoi( value[1] );
	    pObjIndex->value[2] = atoi( value[2] );
	    pObjIndex->value[3] = skill_lookup( value[3] );
	    break;
	}

	if ( (letter = fread_letter( fp )) == '>' )
	{
	  ungetc( letter, fp );
	  load_traps( fp, pObjIndex, NULL, NULL );
  	 /* bug ( "Trap on Object %d.", pObjIndex->vnum );*/
	}
	else
	  ungetc( letter, fp );

	iHash			= vnum % MAX_KEY_HASH;
	pObjIndex->next		= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObjIndex;
	top_obj_index++; 
	/*top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;*/  /* OLC */
	assign_area_vnum( vnum );				   /* OLC */
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
 * Snarf a reset section.	Changed for OLC.
 */
void load_resets( FILE *fp )
{
    RESET_DATA	*pReset;
    int 	iLastRoom = 0;
    int 	iLastObj  = 0;

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

	pReset		= new_reset_data(); /*alloc_perm( sizeof( *pReset ));*/
	pReset->command	= letter;
	/* if_flag */	  fread_number( fp );
	pReset->arg1	= fread_number( fp );
	pReset->arg2	= fread_number( fp );
	pReset->arg3	= ( letter == 'G' || letter == 'R' )
			    ? 0 : fread_number( fp );
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
		|| pReset->arg2 > 5
		|| !pRoomIndex
		|| !( pexit = pRoomIndex->exit[pReset->arg2] )
		|| !IS_SET( pexit->rs_flags, EX_ISDOOR ) )
	    {
		bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
		exit( 1 );
	    }

	    switch ( pReset->arg3 )	/* OLC 1.1b */
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
	    if ( pReset->arg2 < 0 || pReset->arg2 > 6 )	/* Last Door. */
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
	bug( "Load_rooms: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	char letter;
	int  vnum;
	int  door;
	int  iHash;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_rooms: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
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
	pRoomIndex->name		= fread_string( fp );
	pRoomIndex->description		= fread_string( fp );
	/* Area number */		  fread_number( fp );   /* Unused */
	pRoomIndex->room_flags		= fread_number( fp );
	pRoomIndex->sector_type		= fread_number( fp );
	pRoomIndex->light		= 0;
	for ( door = 0; door <= 5; door++ )
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
		char       tLetter;

		door = fread_number( fp );
		if ( door < 0 || door > 5 )
		{
		    bug( "Fread_rooms: vnum %d has bad door number.", vnum );
		    exit( 1 );
		}

		pexit			= alloc_perm( sizeof( *pexit ) );
		pexit->description	= fread_string( fp );
		pexit->keyword		= fread_string( fp );
		pexit->exit_info	= 0;
		pexit->rs_flags		= 0;			/* OLC */
		locks			= fread_number( fp );
		pexit->key		= fread_number( fp );
		pexit->vnum		= fread_number( fp );
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
		if ( (tLetter = fread_letter( fp )) == '>' )
		{
		  ungetc( tLetter, fp );
		  load_traps( fp, NULL, NULL, pexit );
	         /* bug ( "Trap on exit in room %d.", pRoomIndex->vnum);*/
		}
		else
		  ungetc( tLetter, fp );
		pRoomIndex->exit[door]   = pexit;
		top_exit++;
	    }
	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof( *ed ) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
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

	if ( (letter = fread_letter( fp )) == '>' )
	{
	  ungetc( letter, fp );
	  load_traps( fp, NULL, pRoomIndex, NULL );
         /* bug ( "Trap on room %d.", pRoomIndex->vnum );*/
	}
	else
	  ungetc( letter, fp );

	iHash			= vnum % MAX_KEY_HASH;
	pRoomIndex->next	= room_index_hash[iHash];
	room_index_hash[iHash]	= pRoomIndex;
	top_room++;
/*	top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room; */ /* OLC */
	assign_area_vnum( vnum );				     /* OLC */

    }

    return;
}



/*****************************************************************************
 Name:		new_load_rooms
 Purpose:	Loads rooms without the anoying case sequence.
 ****************************************************************************/
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

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_rooms: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp );
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
	pRoomIndex->name		= fread_string( fp );
	pRoomIndex->description		= fread_string( fp );
	/* Area number */		  fread_number( fp );   /* Unused */
	pRoomIndex->room_flags		= fread_number( fp );
	pRoomIndex->sector_type		= fread_number( fp );
	pRoomIndex->light		= 0;
	pRoomIndex->rd                  = 0;
	
	for ( door = 0; door <= 5; door++ )
	    pRoomIndex->exit[door] = NULL;

	for ( ; ; )
	{
	    letter = fread_letter( fp );

	    if ( letter == 'R' )
	    {
	      char *word;

	      ungetc( letter, fp );
	      word = fread_word( fp );
	      if ( !str_cmp( word, "Rd" ) )
	      {
		if ( pRoomIndex->rd != 0 )
		  bug( "New_load_rooms: rd already assigned for room #%d; updating.", pRoomIndex->vnum );
		pRoomIndex->rd = fread_number( fp );
	      }
	      else
	      {
		bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
		exit( 1 );
	      }
	      continue;
	    }

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
		char       tLetter;

		door = fread_number( fp );
		if ( door < 0 || door > 5 )
		{
		    bug( "Fread_rooms: vnum %d has bad door number.", vnum );
		    exit( 1 );
		}

		pexit			= alloc_perm( sizeof( *pexit ));
		pexit->description	= fread_string( fp );
		pexit->keyword		= fread_string( fp );
		locks			= fread_number( fp );
		pexit->exit_info	= locks;
		pexit->rs_flags		= locks;
		pexit->key		= fread_number( fp );
		pexit->vnum		= fread_number( fp );

		if ( (tLetter = fread_letter( fp )) == '>' )
		{
		  ungetc( tLetter, fp );
		  load_traps( fp, NULL, NULL, pexit );
                  /*bug ( "Trap on exit in room %d.", pRoomIndex->vnum);*/
		}
		else
		  ungetc( tLetter, fp );

		pRoomIndex->exit[door]		= pexit;
		top_exit++;
	    }
	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof( *ed ));
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
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

	if ( (letter = fread_letter( fp )) == '>' )
	{
	  ungetc( letter, fp );
	  load_traps( fp, NULL, pRoomIndex, NULL );
	  /*bug ( "Trap on room %d.", pRoomIndex->vnum );*/
	}
	else
	  ungetc( letter, fp );

	iHash			= vnum % MAX_KEY_HASH;
	pRoomIndex->next	= room_index_hash[iHash];
	room_index_hash[iHash]	= pRoomIndex;
	top_room++;
/*	top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room; */
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

    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	int iTrade;

	pShop			= alloc_perm( sizeof( *pShop ) );
	pShop->keeper		= fread_number( fp );
	if ( pShop->keeper == 0 )
	    break;
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    pShop->buy_type[iTrade] = fread_number( fp );
	pShop->profit_buy	= fread_number( fp );
	pShop->profit_sell	= fread_number( fp );
	pShop->open_hour	= fread_number( fp );
	pShop->close_hour	= fread_number( fp );
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
	char letter;

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
	    pMobIndex           = get_mob_index ( fread_number ( fp ) );
	    pMobIndex->spec_fun = spec_lookup   ( fread_word   ( fp ) );
	    if ( pMobIndex->spec_fun == 0 )
	    {
		bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
		exit( 1 );
	    }
	    break;
	}

	fread_to_eol( fp );
    }
}



void load_clans( void )
{
    FILE      *fp;
    CLAN_DATA *pClanIndex;
    char letter;

    if ( !( fp = fopen( CLAN_FILE, "r" ) ) )
	return;
    fpArea = fp; 
    strcpy(strArea, CLAN_FILE);
    for ( ; ; )
    {
	int  vnum;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_clans: # not found.", 0 );
	    continue;
	}

	vnum				= fread_number( fp );
	if ( vnum == 999 )
	    break;

	fBootDb = FALSE;
	if ( get_clan_index( vnum ) )
	{
	    bug( "Load_clans: vnum %d duplicated.", vnum );
	    break;
	}
	fBootDb = TRUE;
	pClanIndex			= alloc_perm( sizeof( *pClanIndex ));
	pClanIndex->vnum		= vnum;
	pClanIndex->bankaccount.gold	= fread_number( fp );
	pClanIndex->bankaccount.silver  = fread_number( fp );
	pClanIndex->bankaccount.copper  = fread_number( fp );
	pClanIndex->name		= fread_string( fp );
	pClanIndex->diety               = fread_string( fp );
	pClanIndex->description         = fread_string( fp );
	pClanIndex->champ               = fread_string( fp );

	pClanIndex->leader              = fread_string( fp );
	pClanIndex->first               = fread_string( fp );
	pClanIndex->second              = fread_string( fp );
	pClanIndex->ischamp             = fread_number( fp );
	pClanIndex->isleader            = fread_number( fp );
	pClanIndex->isfirst             = fread_number( fp );
	pClanIndex->issecond            = fread_number( fp );
	pClanIndex->recall		= fread_number( fp );
	pClanIndex->pkills		= fread_number( fp );
	pClanIndex->mkills		= fread_number( fp );
	pClanIndex->members		= fread_number( fp );
	pClanIndex->pdeaths		= fread_number( fp );
	pClanIndex->mdeaths		= fread_number( fp );
	pClanIndex->obj_vnum_1		= fread_number( fp );
	pClanIndex->obj_vnum_2		= fread_number( fp );
	pClanIndex->obj_vnum_3		= fread_number( fp );
        pClanIndex->settings            = fread_number( fp );
/*        if ( !clan_first )
	   clan_first = pClanIndex;
        if (  clan_last  )
        {
	 clan_last->next = pClanIndex;
        }
        clan_last	= pClanIndex;

	pClanIndex->next	= clan_index_data;
	clan_index_data 	= pClanIndex;*/
	clan_sort(pClanIndex);
	top_clan++;
    }
    fclose ( fp );   
 
    return;
}

void social_sort( SOCIAL_DATA *pSocial )
{
  SOCIAL_DATA *fSocial;
  if ( !social_first )
  {
    social_first = pSocial;
    social_last  = pSocial;
    return;
  }

  if ( strncmp( pSocial->name, social_first->name, 256 ) > 0 )
  {
   pSocial->next = social_first->next;
   social_first = pSocial;
   return;
  } 

  for ( fSocial = social_first; fSocial; fSocial = fSocial->next )
  {
    if (    ( strncmp( pSocial->name, fSocial->name, 256 ) < 0 ) )
    {
      if ( fSocial != social_last )
      { 
        pSocial->next = fSocial->next;
        fSocial->next = pSocial;
        return;
       } 
    }
  }

  social_last->next = pSocial;
  social_last = pSocial;
  pSocial->next = NULL;
  return;
}

/* Decklarean */

void load_socials( void )
{
    FILE      *fp;
    SOCIAL_DATA *pSocialIndex;
    char letter;

    if ( !( fp = fopen( SOCIAL_FILE, "r" ) ) )
	return;
    fpArea = fp;
    strcpy(strArea, SOCIAL_FILE);
    for ( ; ; )
    {
	char*  name;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_socials: # not found.", 0 );
	    continue;
	}

	name				= fread_string( fp );
	if ( !str_cmp( name, "END"    ) )
	  break;

/*	fBootDb = TRUE; */
	pSocialIndex		= 	alloc_perm( sizeof( *pSocialIndex ));
	pSocialIndex->name		=	name;
	pSocialIndex->char_no_arg	=	fread_string( fp );
	pSocialIndex->others_no_arg	=	fread_string( fp );
	pSocialIndex->char_found	=	fread_string( fp );
	pSocialIndex->others_found	=	fread_string( fp );
	pSocialIndex->vict_found	=	fread_string( fp );
	pSocialIndex->char_auto		=	fread_string( fp );
	pSocialIndex->others_auto	=	fread_string( fp );
	social_sort(pSocialIndex);
	top_social++;
    }
    fclose ( fp );

    return;
}


/*
 * Snarf notes file.
 */
void load_notes( void )
{
    FILE      *fp;
    NOTE_DATA *pnotelast;

    if ( !( fp = fopen( NOTE_FILE, "r" ) ) )
	return;
    fpArea = fp;
    strcpy(strArea, NOTE_FILE);

    pnotelast = NULL;
    for ( ; ; )
    {
	NOTE_DATA *pnote;
	char       letter;

	do
	{
	    letter = getc( fp );
	    if ( feof(fp) )
	    {
		fclose( fp );
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );

	pnote		  = alloc_perm( sizeof( *pnote ) );

	if ( str_cmp( fread_word( fp ), "sender" ) )
	    break;
	pnote->sender     = fread_string( fp );

	if ( str_cmp( fread_word( fp ), "date" ) )
	    break;
	pnote->date       = fread_string( fp );

	if ( str_cmp( fread_word( fp ), "stamp" ) )
	    break;
	pnote->date_stamp = fread_number( fp );

	if ( str_cmp( fread_word( fp ), "to" ) )
	    break;
	pnote->to_list    = fread_string( fp );

	if ( str_cmp( fread_word( fp ), "subject" ) )
	    break;
	pnote->subject    = fread_string( fp );

	pnote->protected = FALSE;
	pnote->on_board = 0;
	{
	  char letter;

	  letter = fread_letter( fp );
	  ungetc( letter, fp );
	  if ( letter == 'P' && !str_cmp( fread_word( fp ), "protect" ) )
	    pnote->protected  = fread_number( fp );
	  letter = fread_letter( fp );
	  ungetc( letter, fp );
	  if ( letter == 'B' && !str_cmp( fread_word( fp ), "board" ) )
	    pnote->on_board   = fread_number( fp );
	}

	if ( str_cmp( fread_word( fp ), "text" ) )
	    break;
	pnote->text       = fread_string( fp );

	if ( !note_list )
	    note_list           = pnote;
	else
	    pnotelast->next     = pnote;

	pnotelast               = pnote;
    }

    bug( "Load_notes: bad key word.", 0 );
    fclose(fp);
    return;
}


void load_down_time( void )
{
    FILE *fp;

    down_time = str_dup ( "*" );
    warning1  = str_dup ( "*" );
    warning2  = str_dup ( "*" );
    stype     = 1;

    if ( !( fp = fopen( DOWN_TIME_FILE, "r" ) ) )
        return;

    fpArea = fp;
    strcpy(strArea, DOWN_TIME_FILE);
    for ( ; ; )
    {
        char *word;
	char  letter;

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
	
	word = fread_word( fp );

	if ( !str_cmp( word, "DOWNTIME" ) )
	{
	    free_string( down_time );
	    down_time = fread_string( fp );
	}
	if ( !str_cmp( word, "WARNINGA" ) )
	{
	    free_string( warning1 );
	    warning1 = fread_string( fp );
	}
	if ( !str_cmp( word, "WARNINGB" ) )
	{
	    free_string( warning2 );
	    warning2 = fread_string( fp );
	}
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
/*                 char             buf     [ MAX_STRING_LENGTH ];*/
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
	    for ( door = 0; door <= 5; door++ )
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
	    for ( door = 0; door <= 5; door++ )
	    {
		if (   ( pexit     = pRoomIndex->exit[door]       )
		    && ( to_room   = pexit->to_room               )
		    && ( pexit_rev = to_room->exit[rev_dir[door]] )
		    &&   pexit_rev->to_room != pRoomIndex )
		{
/* commented out... too many
		    sprintf( buf, "Fix_exits: %d:%d -> %d:%d -> %d.",
			    pRoomIndex->vnum, door,
			    to_room->vnum,    rev_dir[door],
			    ( !pexit_rev->to_room ) ? 0
			    :  pexit_rev->to_room->vnum );
		    bug( buf, 0 );
*/
		}
	    }
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
		    && pch->in_room->area == pArea 
      		   )
		{
		    if ( pArea->reset_sound == NULL )
		      send_to_char(AT_GREEN, "You shiver as a cold breeze blows through the room.\n\r",
		      pch );
		    else if ( str_cmp( pArea->reset_sound, "none" ) )
                    {
		      send_to_char(AT_GREEN, pArea->reset_sound, pch );
	 	      send_to_char(AT_GREEN, "\n\r", pch );
  		    }
		}
	    }
	}

	/*
	 * Check age and reset.
	 * Note: Mud School resets every 3 minutes (not 15).
	 */
	if ( pArea->nplayer == 0 || pArea->age >= 15 )
	{
	    ROOM_INDEX_DATA *bh_school, *rw_school;

	    reset_area( pArea );
	    pArea->age = number_range( 0, 3 );
	    bh_school = get_room_index( ROOM_VNUM_SCHOOL );
	    rw_school = get_room_index( ROOM_VNUM_RW_SCHOOL );
	    if ( (bh_school && bh_school->area == pArea) ||
	         (rw_school && rw_school->area == pArea) )
		pArea->age = 15 - 3;
	}
    }

    return;
}



/* OLC
 * Reset one room.  Called by reset_area and olc.
 */
void reset_room( ROOM_INDEX_DATA *pRoom )
{
    RESET_DATA	*pReset;
    CHAR_DATA	*pMob;
    OBJ_DATA	*pObj;
    CHAR_DATA	*LastMob = NULL;
    OBJ_DATA	*LastObj = NULL;
    int iExit;
    int level = 0;
    bool last;

    if ( !pRoom )
	return;

    pMob	= NULL;
    last	= FALSE;
    
    for ( iExit = 0;  iExit < MAX_DIR;  iExit++ )
    {
	EXIT_DATA *pExit;
	if ( ( pExit = pRoom->exit[iExit] )
	  && !IS_SET( pExit->exit_info, EX_BASHED ) )	/* Skip Bashed. */
	{
	    pExit->exit_info = pExit->rs_flags;
	    if ( ( pExit->to_room != NULL )
	      && ( ( pExit = pExit->to_room->exit[rev_dir[iExit]] ) ) )
	    {
		/* nail the other side */
		pExit->exit_info = pExit->rs_flags;
	    }
	}
    }

    for ( pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next )
    {
	MOB_INDEX_DATA	*pMobIndex;
	OBJ_INDEX_DATA	*pObjIndex;
	OBJ_INDEX_DATA	*pObjToIndex;
	ROOM_INDEX_DATA	*pRoomIndex;

	switch ( pReset->command )
	{
	default:
		sprintf(log_buf, "Reset_room: bad command %c in room %d.", 
		pReset->command, pRoom->vnum );
		bug( log_buf, 0 ); 
		break;

	case 'M':
	    if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
	    {
		sprintf(log_buf,  "Reset_room: 'M': bad vnum %d in room %d.", 
		pReset->arg1, pRoom->vnum );
		bug( log_buf, 0 ); 
		continue;
	    }

	    /*
	     * Some hard coding.
	     */
	    if ( ( pMobIndex->spec_fun == spec_lookup( "spec_cast_ghost" ) &&
	         ( weather_info.sunlight != SUN_DARK ) &&
		  !room_is_dark( pRoom ) ) ) continue;

	    if ( pMobIndex->count >= pReset->arg2 )
	    {
		last = FALSE;
		break;
	    }

	    pMob = create_mobile( pMobIndex );

	    /*
	     * Some more hard coding.
	     */
	    if ( room_is_dark( pRoom ) )
		SET_BIT(pMob->affected_by, AFF_INFRARED);

	    /*
	     * Pet shop mobiles get ACT_PET set.
	     */
	    {
		ROOM_INDEX_DATA *pRoomIndexPrev;

		pRoomIndexPrev = get_room_index( pRoom->vnum - 1 );
		if ( pRoomIndexPrev
		    && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
		    SET_BIT( pMob->act, ACT_PET);
	    }

	    char_to_room( pMob, pRoom );

	    LastMob = pMob;
	    level  = URANGE( 0, pMob->level - 2, LEVEL_HERO );
	    last = TRUE;
	    break;

	case 'O':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
		sprintf( log_buf, "Reset_room: 'O': bad vnum %d in room %d.", 
		pReset->arg1, pRoom->vnum );
		bug( log_buf, 0 ); 
		continue;
	    }

	    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
		sprintf( log_buf, "Reset_room: 'O': bad vnum %d in room %d.", 
		pReset->arg3, pRoom->vnum );
		bug( log_buf, 0 ); 
		continue;
	    }

	    if ( pRoom->area->nplayer > 0 
	    ||   count_obj_list( pObjIndex, pRoom->contents ) > 0 )
		break;

	    pObj = create_object( pObjIndex, level);
/*	    pObj->cost = 0;*/
	    obj_to_room( pObj, pRoom );
	    break;

	case 'P':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
		sprintf( log_buf, "Reset_room: 'P': bad vnum %d in room %d.", 
		pReset->arg1, pRoom->vnum );
		bug( log_buf, 0 ); 
		continue;
	    }

	    if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
	    {
		sprintf( log_buf, "Reset_room: 'P': bad vnum %d in room %d.", 
		pReset->arg3, pRoom->vnum );		
		bug( log_buf, 0 ); 
		continue;
	    }

	    if ( pRoom->area->nplayer > 0
	      || !( LastObj = get_obj_type( pObjToIndex ) )
	      || count_obj_list( pObjIndex, LastObj->contains ) > 0 )
		break;

	    pObj = create_object(pObjIndex,level);
	    obj_to_obj( pObj, LastObj );

	    /*
	     * Ensure that the container gets reset.	OLC 1.1b
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
		sprintf( log_buf, "Reset_room: 'E' or 'G': bad vnum %d in room %d.", 
		pReset->arg1, pRoom->vnum );
		bug( log_buf, 0 );
		continue; 
	    }

	    if ( !last )
		break;

	    if ( !LastMob )
	    {
		sprintf( log_buf, "Reset_room: 'E' or 'G': null mob for vnum %d in room %d.",
		    pReset->arg1, pRoom->vnum );
		    bug( log_buf, 0 ); 
		last = FALSE;
		break;
	    }

	    if ( LastMob->pIndexData->pShop )	/* Shop-keeper? */
	    {
		int olevel;

		switch ( pObjIndex->item_type )
		{
		default:                olevel = 0;                      break;
		case ITEM_PILL:         olevel = number_range(  0, 10 ); break;
		case ITEM_POTION:	olevel = number_range(  0, 10 ); break;
		case ITEM_SCROLL:	olevel = number_range(  5, 15 ); break;
		case ITEM_WAND:		olevel = number_range( 10, 20 ); break;
		case ITEM_LENSE:        olevel = number_range( 10, 20 ); break;
		case ITEM_STAFF:	olevel = number_range( 15, 25 ); break;
		case ITEM_ARMOR:	olevel = number_range(  5, 15 ); break;
		case ITEM_WEAPON:	if ( pReset->command == 'G' )
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
  		pObj = create_object( pObjIndex, level);
	    }
            obj_to_char( pObj, LastMob );
	    if ( pReset->command == 'E' )
		equip_char( LastMob, pObj, pReset->arg3 );
	    last = TRUE;
	    break;

	case 'D':
	    break;

	case 'R':
/* OLC 1.1b */
	    if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
	    {
		sprintf( log_buf, "Reset_room: 'R': bad vnum %d in room %d.", 
		pReset->arg1, pRoom->vnum );
		bug( log_buf, 0 ); 
		continue;
	    }

		if ( pRoomIndex->area->builders[0] != '\0' )
			continue;

	    {
		EXIT_DATA *pExit;
		int d0;
		int d1;

		for ( d0 = 0; d0 < pReset->arg2; d0++ )
		{
		    d1                   = number_range( d0, pReset->arg2 );
		    pExit                = pRoomIndex->exit[d0];
		    pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
		    pRoomIndex->exit[d1] = pExit;
		}
	    }
	    break;
	}
    }

    return;
}



/* OLC
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoom;
    int  vnum;

    for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
	if ( ( pRoom = get_room_index(vnum) ) )
	    reset_room(pRoom);
    }

    return;
}



/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
    CHAR_DATA *mob;

    mobs_in_game++;

    if ( !pMobIndex )
    {
	bug( "Create_mobile: NULL pMobIndex.", 0 );
	exit( 1 );
    }

    if ( !char_free )
    {
	mob		= alloc_perm( sizeof( *mob ) );
    }
    else
    {
	mob		= char_free;
	char_free	= char_free->next;
    }

    clear_char( mob );
    mob->pIndexData     = pMobIndex;

    mob->name		= str_dup( pMobIndex->player_name );	/* OLC */
    mob->short_descr	= str_dup( pMobIndex->short_descr );	/* OLC */
    mob->long_descr	= str_dup( pMobIndex->long_descr );	/* OLC */
    mob->description	= str_dup( pMobIndex->description );	/* OLC */
    mob->spec_fun	= pMobIndex->spec_fun;
    mob->prompt         = str_dup( "<%hhp %mm %vmv> " );

    mob->level		= pMobIndex->level;
    mob->act		= pMobIndex->act | ACT_IS_NPC;
    mob->affected_by	= pMobIndex->affected_by;
    mob->affected_by2   = pMobIndex->affected_by2;
    mob->imm_flags	= pMobIndex->imm_flags;
    mob->res_flags      = pMobIndex->res_flags;
    mob->vul_flags      = pMobIndex->vul_flags;

    mob->alignment	= pMobIndex->alignment;
    mob->sex		= pMobIndex->sex;

    mob->armor		= interpolate( mob->level, 100, -100 );
#ifdef NEW_MONEY
    mob->money.gold  	= pMobIndex->money.gold;
    mob->money.silver	= pMobIndex->money.silver;
    mob->money.copper	= pMobIndex->money.copper;
#else
    mob->gold           = pMobIndex->gold;
#endif
    mob->perm_hit	= pMobIndex->hitnodice + 1;
    mob->mod_hit	= 0;
    mob->hit		= MAX_HIT(mob);
    mob->combat_timer	= 0;			/* XOR */
    mob->summon_timer	= -1;			/* XOR */

    mob->start_align = (IS_EVIL(mob) ? 'E' : IS_GOOD(mob) ? 'G' : 'N');
    /*
     * Insert in list.
     */
    mob->next		= char_list;
    char_list		= mob;
    pMobIndex->count++;
    return mob;
}

/* duplicate a mobile exactly -- except inventory */
void clone_mobile(CHAR_DATA *parent, CHAR_DATA *clone)
{
    AFFECT_DATA *paf;

    if ( parent == NULL || clone == NULL || !IS_NPC(parent))
	return;

    mobs_in_game++;    
    /* start fixing values */ 
    clone->name 	= str_dup(parent->name);
    clone->short_descr	= str_dup(parent->short_descr);
    clone->long_descr	= str_dup(parent->long_descr);
    clone->description	= str_dup(parent->description);
    clone->sex		= parent->sex;
    clone->class[0]	= parent->class[0];
    clone->class[1]	= -1;
    clone->race		= parent->race;
    clone->level	= parent->level;
    clone->trust	= 0;
    clone->timer	= parent->timer;
    clone->wait		= parent->wait;
    clone->hit		= parent->hit;
    clone->mod_hit	= parent->mod_hit;
    clone->perm_hit	= parent->perm_hit;
    clone->mana		= parent->mana;
    clone->perm_mana	= parent->perm_mana;
    clone->mod_mana	= parent->mod_mana;
    clone->move		= parent->move;
    clone->perm_move	= parent->perm_move;
    clone->mod_move	= parent->mod_move;
#ifdef NEW_MONEY
    clone->money.gold   = parent->money.gold;
    clone->money.silver	= parent->money.silver;
    clone->money.copper = parent->money.copper;
#else
    clone->gold		= parent->gold;
#endif
    clone->exp		= parent->exp;
    clone->act		= parent->act;
    clone->affected_by	= parent->affected_by;
    clone->position	= parent->position;
    clone->practice	= parent->practice;
    clone->alignment	= parent->alignment;
    clone->hitroll	= parent->hitroll;
    clone->damroll	= parent->damroll;
    clone->wimpy	= parent->wimpy;
    clone->spec_fun	= parent->spec_fun;
    clone->armor	= parent->armor;    

    /* now add the affects */
    for (paf = parent->affected; paf != NULL; paf = paf->next)
        affect_to_char(clone,paf);

}


/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, int level )
{
    static OBJ_DATA  obj_zero;
           OBJ_DATA *obj;

    if ( !pObjIndex )
    {
	bug( "Create_object: NULL pObjIndex.", 0 );
	bug( "Replacing with dummy object.", 0 );
	pObjIndex = get_obj_index( 1 );
	/*
	exit( 1 );
	*/
    }

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
    obj->pIndexData	= pObjIndex;
    obj->in_room	= NULL;
    obj->level		= pObjIndex->level;
    obj->wear_loc	= -1;

    obj->name		= str_dup( pObjIndex->name );		/* OLC */
    obj->short_descr	= str_dup( pObjIndex->short_descr );	/* OLC */
    obj->description	= str_dup( pObjIndex->description );	/* OLC */
    obj->item_type	= pObjIndex->item_type;
    obj->extra_flags	= pObjIndex->extra_flags;
    obj->anti_race_flags = pObjIndex->anti_race_flags;
    obj->anti_class_flags = pObjIndex->anti_class_flags;
    obj->wear_flags	= pObjIndex->wear_flags;
    obj->value[0]	= pObjIndex->value[0];
    obj->value[1]	= pObjIndex->value[1];
    obj->value[2]	= pObjIndex->value[2];
    obj->value[3]	= pObjIndex->value[3];
    obj->weight		= pObjIndex->weight;
#ifdef NEW_MONEY
    obj->cost.gold	= pObjIndex->cost.gold;
    obj->cost.silver	= pObjIndex->cost.silver;
    obj->cost.copper	= pObjIndex->cost.copper;
#else
    obj->cost		= pObjIndex->cost;
#endif
    obj->ac_type        = pObjIndex->ac_type;
    obj->ac_vnum        = pObjIndex->ac_vnum;
    obj->ac_spell       = str_dup(pObjIndex->ac_spell);
    obj->ac_charge[0]   = pObjIndex->ac_charge[0];
    obj->ac_charge[1]   = pObjIndex->ac_charge[1];
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
    case ITEM_CORPSE_PC:
    case ITEM_FOUNTAIN:
    case ITEM_BLOOD:
    case ITEM_VODOO:
    case ITEM_BERRY:
    case ITEM_NOTEBOARD:
	break;

    case ITEM_SCROLL:
	obj->value[0]   = obj->value[0];
	break;

    case ITEM_WAND:
    case ITEM_LENSE:
    case ITEM_STAFF:
	obj->value[0]   = obj->value[0];
	obj->value[1]	= obj->value[1];
	obj->value[2]	= obj->value[1];
	obj->value[3]   = obj->value[3];
	break;
    case ITEM_CORPSE_NPC:
    case ITEM_PORTAL:
        obj->value[0]   = obj->value[0];
        break;
        
    case ITEM_WEAPON:
	obj->value[1]   = number_fuzzy( number_fuzzy( 1 * level / 4 + 2 ) );
	obj->value[2]	= number_fuzzy( number_fuzzy( 3 * level / 4 + 6 ) );
	break;

    case ITEM_ARMOR:
	obj->value[0]   = number_fuzzy( level / 4 + 2 );
	break;

    case ITEM_POTION:
    case ITEM_PILL:
	obj->value[0]   = obj->value[0];
	break;

    case ITEM_MONEY:
#ifdef NEW_MONEY
	obj->value[0]	= obj->cost.gold;
	obj->value[1] 	= obj->cost.silver;
	obj->value[2]	= obj->cost.copper;
#else
	obj->value[0]   = obj->cost;
#endif
	break;
    }

    obj->next		= object_list;
    object_list		= obj;
    pObjIndex->count++;

    return obj;
}


/* duplicate an object exactly -- except contents */
void clone_object(OBJ_DATA *parent, OBJ_DATA *clone)
{
    int i;

    if (parent == NULL || clone == NULL)
	return;

    /* start fixing the object */
    clone->name 	= str_dup(parent->name);
    clone->short_descr 	= str_dup(parent->short_descr);
    clone->description	= str_dup(parent->description);

    if ( parent->pIndexData->vnum != OBJ_VNUM_LETTER )
    {
      if ( parent->extra_descr )
      {
         EXTRA_DESCR_DATA *ed;

         for( ed = parent->extra_descr; ed; ed = ed->next )
         {
	    clone->extra_descr->keyword = str_dup(parent->extra_descr->keyword);
	    clone->extra_descr->description = str_dup(parent->extra_descr->description);
	    clone->extra_descr = clone->extra_descr->next;
         }      
      }
    }

    clone->item_type	= parent->item_type;
    clone->extra_flags	= parent->extra_flags;
    clone->anti_race_flags = parent->anti_race_flags;
    clone->anti_class_flags = parent->anti_class_flags;
    clone->wear_flags	= parent->wear_flags;
    clone->weight	= parent->weight;
#ifdef NEW_MONEY
    clone->cost.gold	= parent->cost.gold;
    clone->cost.silver	= parent->cost.silver;
    clone->cost.copper	= parent->cost.copper;
#else
    clone->cost		= parent->cost;
#endif
    clone->level	= parent->level;
    clone->timer	= parent->timer;

    for (i = 0;  i < 3; i ++)
	clone->value[i]	= parent->value[i];
}


/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
    static CHAR_DATA ch_zero;
    int iclass;

    *ch				= ch_zero;
    ch->name			= &str_empty[0];
    ch->short_descr		= &str_empty[0];
    ch->long_descr		= &str_empty[0];
    ch->description		= &str_empty[0];
    ch->prompt                  = &str_empty[0];
    ch->last_note               = 0;
    ch->logon			= current_time;
    ch->armor			= 100;
    ch->position		= POS_STANDING;
    ch->level                   = 0;
    ch->practice		= 21;
    ch->hit			= 20;
    ch->perm_hit		= 20;
    ch->mod_hit			= 0;
    ch->mana			= 100;
    ch->perm_mana		= 100;
    ch->mod_mana		= 0;
    ch->bp                      = 20;
    ch->perm_bp                 = 20;
    ch->mod_bp			= 0;
    ch->move			= 100;
    ch->perm_move		= 100;
    ch->mod_move		= 0;
    ch->leader                  = NULL;
    ch->master                  = NULL;
    ch->deleted                 = FALSE;
    ch->start_align		= 'N';
    for ( iclass = 0; iclass < MAX_CLASS; iclass++ )
      ch->class[iclass] = -1;
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
    CHAR_DATA   *PeT;

    if ( !IS_NPC(ch) && ch->in_room )
    {
    for ( PeT = ch->in_room->people; PeT; PeT = PeT->next_in_room )
    {
     if ( IS_NPC( PeT ) )
       if( IS_SET( PeT->act, ACT_PET ) && PeT->master == ch )
       {
	 extract_char( PeT, TRUE );
	 break;
       }
    }
    }

    for ( obj = ch->carrying; obj; obj = obj_next )
    {
        obj_next = obj->next_content;
        if ( obj->deleted )
	    continue;
	extract_obj( obj );
    }
/*bug("obj",0);*/
    for ( paf = ch->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
	affect_remove( ch, paf );
    }
    for ( paf = ch->affected2; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
	affect_remove2( ch, paf );
    }
/*bug("aff",0);*/
    free_string( ch->name               );
    free_string( ch->short_descr	);
    free_string( ch->long_descr		);
    free_string( ch->description	);
    free_string( ch->prompt             );
/*bug("string",0);*/
    if ( ch->pcdata )
    {
        ALIAS_DATA *pAl;
	ALIAS_DATA *pAl_next;

	free_string( ch->pcdata->pwd		);
	free_string( ch->pcdata->bamfin		);
	free_string( ch->pcdata->bamfout	);
        free_string( ch->pcdata->bamfusee   	);
        free_string( ch->pcdata->transto   	);
        free_string( ch->pcdata->transfrom 	);
        free_string( ch->pcdata->transvict 	);
        free_string( ch->pcdata->slayusee  	);
        free_string( ch->pcdata->slayroom  	);
        free_string( ch->pcdata->slayvict  	);
        free_string( ch->pcdata->afkchar        );      
	free_string( ch->pcdata->title		);
        free_string( ch->pcdata->empowerments	);
	free_string( ch->pcdata->detractments 	);
	free_string( ch->pcdata->plan		);
	free_string( ch->pcdata->email		);
	if ( ch->pcdata->lname )
	  free_string(ch->pcdata->lname);
	for ( obj = ch->pcdata->storage; obj; obj = obj_next )
	{
	  obj_next = obj->next_content;
	  if ( obj->deleted )
	    continue;
	  extract_obj( obj );
	}
	for ( pAl = ch->pcdata->alias_list; pAl; pAl = pAl_next )
	{
	  pAl_next = pAl->next;
	  /*free_string(pAl->old);
	  free_string(pAl->new); 
	  free_mem(pAl, sizeof( *pAl )); */
          free_alias( pAl );
	}
/*bug("alias",0);*/
/*	ch->pcdata->next = pcdata_free;
	pcdata_free      = ch->pcdata;*/
	free_mem( ch->pcdata, sizeof( *ch->pcdata ) );
    }

    if ( ch->pnote )
    {
      free_string(ch->pnote->sender);
      free_string(ch->pnote->to_list);
      free_string(ch->pnote->text);
      free_string(ch->pnote->date);
      free_string(ch->pnote->subject);
/*      ch->pnote->next = note_free;
      note_free       = ch->pnote;*/
      free_mem( ch->pnote, sizeof( *ch->pnote ) );
    }
/*bug("pnote",0);*/
    if ( ch->gspell )
      end_gspell( ch );
/*bug("gspell",0);*/
/*    ch->next	     = char_free;
    char_free	     = ch;*/
    free_mem( ch, sizeof( *ch ) );
/*bug("end",0);*/
    return;
}



/*
 * Get an extra description from a list.
 */
char *get_extra_descr( CHAR_DATA *ch, char *name, EXTRA_DESCR_DATA *ed )
{
    for ( ; ed; ed = ed->next )
    {
	if ( is_name( ch, name, ed->keyword ) )
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
    }

    return NULL;
}

CLAN_DATA *get_clan_index( int vnum )
{
    CLAN_DATA *pClanIndex;

    for ( pClanIndex  = clan_first;
	  pClanIndex;
	  pClanIndex  = pClanIndex->next )
    {
	if ( pClanIndex->vnum == vnum )
	    return pClanIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_clan_index: bad clan %d.", vnum );
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

    if ( c == EOF )
    {
      bug( "Fread_letter: EOF", 0 );
      return '\0';
    }
    return c;
}



/*
 * Read a number from a file.
 */
int fread_number( FILE *fp )
{
    char c;
    int  number;
    bool sign;

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

    if ( c == EOF )
    {
      bug( "Fread_number: EOF", 0 );
      return 0;
    }
    if ( !isdigit( c ) )
    {
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
	number += fread_number( fp );
    else if ( c != ' ' )
	ungetc( c, fp );

    return number;
}



/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 * This function takes 40% to 50% of boot-up time.
 */
char *fread_string( FILE *fp )
{
    char *plast;
    char  c;

    plast = top_string + sizeof( char * );
    if ( plast > &string_space [ MAX_STRING - MAX_STRING_LENGTH ] )
    {
	bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
	exit( 1 );
    }

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
	c = getc( fp );
    }
    while ( isspace( c ) );

    if ( ( *plast++ = c ) == '~' || c == EOF )
	return &str_empty[0];

    for ( ;; )
    {
	/*
	 * Back off the char type lookup,
	 *   it was too dirty for portability.
	 *   -- Furey
	 */
	switch ( *plast = getc( fp ) )
	{
	default:
	    plast++;
	    break;

	case EOF:
	    bug( "Fread_string: EOF", 0 );
	    ungetc( '~', fp );
	    break;

	case '\n':
	    plast++;
	    *plast++ = '\r';
	    break;

	case '\r':
	    break;

	case '~':
	    plast++;
	    {
		union
		{
		    char *	pc;
		    char	rgc[sizeof( char * )];
		} u1;
		int ic;
		int iHash;
		char *pHash;
		char *pHashPrev;
		char *pString;

		plast[-1] = '\0';
		iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
		for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
		{
		    for ( ic = 0; ic < sizeof( char * ); ic++ )
			u1.rgc[ic] = pHash[ic];
		    pHashPrev = u1.pc;
		    pHash    += sizeof(char *);

		    if ( top_string[sizeof( char * )] == pHash[0]
			&& !strcmp( top_string+sizeof( char * )+1, pHash+1 ) )
			return pHash;
		}

		if ( fBootDb )
		{
		    pString             = top_string;
		    top_string		= plast;
		    u1.pc		= string_hash[iHash];
		    for ( ic = 0; ic < sizeof( char * ); ic++ )
			pString[ic] = u1.rgc[ic];
		    string_hash[iHash]  = pString;

		    nAllocString += 1;
		    sAllocString += top_string - pString;
		    return pString + sizeof( char * );
		}
		else
		{
		    return str_dup( top_string + sizeof( char * ) );
		}
	    }
	}
    }
}



/*
 * Read to end of line (for comments).
 */
void fread_to_eol( FILE *fp )
{
    char c;

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
char *fread_word( FILE *fp )
{
    static char  word [ MAX_INPUT_LENGTH ];
           char *pword;
           char  cEnd;

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
	if ( *pword == EOF )
	{
	  bug( "Fread_word: EOF", 0 );
	  *pword = '\0';
	  return word;
	}
	if ( cEnd == ' ' ? isspace( *pword ) : *pword == cEnd )
	{
	    if ( cEnd == ' ' )
		ungetc( *pword, fp );
	    *pword = '\0';
	    return word;
	}
    }
    bug( "Fread_word: word too long.", 0 );
    word[MAX_INPUT_LENGTH-1] = '\0';
    {
      char tc;
      
      for ( ; ; )
      {
        tc = getc(fp);
        if ( (cEnd == ' ' ? isspace( tc ) : tc == cEnd) || tc == EOF )
          break;
      }
      if ( cEnd == ' ' && tc != EOF )
        ungetc( tc, fp );
    }
    return word;

}

void *new_mem( int sMem, int isperm)
{
  void *pMem;
  static bool Killit;
  static bool Already;


  isperm=0;
  if (!( pMem = calloc(1, sMem) ) ||
      (((sAllocPerm * 10) / (1024*1024)) > 85 && !Killit))
  {
    if ( !pMem )
    {
      /* Not even enough mem to shutdown decently */
      fprintf( stderr, "New_mem: NULL warning memory reboot.\n" );
      fprintf( stderr, "Memory used: %d\n", sAllocPerm );
      exit( 1 );
    }
    if ( !Already )
    {
      char *strtime;

      strtime = ctime( &current_time );
      strtime[strlen(strtime)-1] = '\0';
      fprintf( stderr, "%s :: New_mem: Out of memory.\n", strtime );
      fprintf( stderr, "Memory used: %d\n", sAllocPerm );
    }
    if ( pMem && !Already )
    {
      char time_buf[MAX_STRING_LENGTH];
      char down_buf[MAX_STRING_LENGTH];
      time_t ntime;
      extern bool sreset;

      Killit = TRUE;
      ntime = current_time + 1;         /*First warning is 1 second from now*/
      strcpy(time_buf, ctime(&ntime));
      strcpy(down_buf, time_buf + 11);
      down_buf[8] = '\0';
      free_string(warning1);
      warning1 = str_dup(down_buf);
      ntime = ntime + 60;               /*Second warning at +61 seconds*/
      strcpy(time_buf, ctime(&ntime));
      strcpy(down_buf, time_buf + 11);
      down_buf[8] = '\0';
      free_string(warning2);
      warning2 = str_dup(down_buf);
      ntime = ntime + 60;               /*Reboot at +121 seconds*/
      strcpy(time_buf, ctime(&ntime));
      strcpy(down_buf, time_buf + 11);
      down_buf[8] = '\0';
      free_string(down_time);
      down_time = str_dup(down_buf);
      stype = 0;                       /*Reboot not shutdown*/
      sreset = FALSE;                  /*SSTime not settable now*/
/*      end_of_game( );*/
    }
    else
    {
      fprintf( stderr, "NULL memory reboot.\n" );
      fprintf( stderr, "Memory used: %d\n", sAllocPerm );
      exit( 1 );                       /*No mem for anything. Straight reboot*/
    }
    Already = TRUE;
  }

  sAllocPerm += sMem;
  nAllocPerm++;
  MemAllocated += sMem;
  return pMem;
}

void dispose( void *pMem, int sMem )
{
  if ( !pMem )
  {
    bug( "Dispose: pMem is null.", 0 );
    return;
  }

  free( pMem );
  sAllocPerm -= sMem;
  nAllocPerm--;
  MemFreed += sMem;
  return;
}

/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem( int sMem )
{
    return new_mem( sMem, 0 );
}

/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, int sMem )
{
  dispose( pMem, sMem );
  return;
}


/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm( int sMem )
{ 
  return new_mem(sMem, 1 );
}

/*
 * Duplicate a string into dynamic memory.
 * Fread_strings are read-only and shared.
 */
char *str_dup( const char *str )
{
    char *str_new;

    if ( str[0] == '\0' )
	return &str_empty[0];

    if ( str >= string_space && str < top_string )
	return (char *) str;

    str_new = alloc_mem( strlen(str) + 1 );
  /*  str_new =(char*)malloc( strlen(str) + 1 ); */ 
/* this is a hack so that we can track memory right
   all the memroy stuff needs to be switched back to an
   envy 1.0 most likely have to ask Altrag
       -Decklarean
*/

/*    sAllocPerm += (strlen(str) + 1);
    nAllocPerm++;
    MemAllocated += (strlen(str) + 1);
*/
    strcpy( str_new, str );
    return str_new;
}



/*
 * Free a string.
 * Null is legal here to simplify callers.
 * Read-only shared strings are not touched.
 */
void free_string( char *pstr )
{
    if (  !pstr
	|| pstr == &str_empty[0]
	|| ( pstr >= string_space && pstr < top_string ) )
	return;

    free_mem( pstr, strlen( pstr ) + 1 );
    return;
}


/*  This is a total mess, the new one is below it.  -Decklarean
void do_areas( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea1;
    AREA_DATA *pArea2;
    char        buf  [ MAX_STRING_LENGTH   ];
    int         iArea = 0;
    int         iAreaHalf;
    int		num;
    
    if ( argument[0] != '\0' )
       num = is_number( argument ) ? atoi( argument ) : 0;
    else
       num = 0;

    for ( pArea1 = area_first; pArea1; pArea1 = pArea1->next )
      if ( !IS_SET( pArea1->area_flags, AREA_PROTOTYPE ) )
	iArea++;

    iAreaHalf = ( iArea + 1 ) / 2;
    pArea1    = area_first;
    pArea2    = area_first;
   
    for ( iArea = 0; pArea2; pArea2 = pArea2->next )
    {
      if ( !IS_SET( pArea2->area_flags, AREA_PROTOTYPE ) )
	iArea++;
      if ( iArea >= iAreaHalf )
	break;
    }

    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
    {
      if ( num == 0 )
      {
        sprintf( buf, "%-39s%-39s\n\r",
	      pArea1->name, ( pArea2 ) ? pArea2->name : "" );
	send_to_char( C_DEFAULT, buf, ch );
      }
      else
      {
	if ( ( pArea1->llevel <= num ) && ( num <= pArea1->ulevel ) )
	{
     	  sprintf( buf, "%-39s\n\r", pArea1->name );  
	  send_to_char( C_DEFAULT, buf, ch );
	}
	if ( ( pArea2->llevel <= num ) && ( num <= pArea2->ulevel ) )
	{
	  sprintf( buf, "%-39s\n\r", pArea2->name );
	  send_to_char( C_DEFAULT, buf, ch );
	}
      }

      for ( pArea1 = pArea1->next; pArea1; pArea1 = pArea1->next )
	if ( !IS_SET( pArea1->area_flags, AREA_PROTOTYPE ) )
	  break;
      if ( pArea2 )
	for ( pArea2 = pArea2->next; pArea2; pArea2 = pArea2->next )
	  if ( !IS_SET( pArea2->area_flags, AREA_PROTOTYPE ) )
	    break;
    }

    return;
}
*/

/*
 *  New do_areas by Decklarean.
 */
void do_areas( CHAR_DATA *ch, char *argument )
{
  AREA_DATA *pArea;
  
  /* used to display areas */
  char buf[MAX_STRING_LENGTH];
  char buf1[MAX_STRING_LENGTH];
  int col;

  char level[MAX_STRING_LENGTH];
  int ilevel = 0;

  /* get level */
  argument = one_argument( argument, level );

  if ( level[0] != '\0' && is_number( level ) )
     ilevel = atoi( level );

  /* display the area files */
  send_to_char( AT_BLUE, "\n\rTHE WORLD OF THE STORM\n\r", ch );

  buf1[0] = '\0'; 
  col = 0;

  for ( pArea = area_first; pArea; pArea = pArea->next )
  {

    if (    !IS_SET( pArea->area_flags, AREA_PROTOTYPE ) &&
         ( ilevel == 0 || (     ( pArea->llevel <= ilevel ) 
                            && ( pArea->ulevel >= ilevel ) )
         )
       )
    {
       sprintf( buf, "&W%-8.8s &G%-18.18s &B%-10.10s",
                pArea->name,
                &pArea->name[9],
                &pArea->name[28] ); 
       strcat( buf1, buf );
       if ( ++col % 2 == 0 )
        strcat( buf1, "\n\r" );
    } 
  }
  if ( col % 2 != 0 )
   strcat( buf1, "\n\r" );
  if ( col != 0 )
   send_to_char( C_DEFAULT, buf1 , ch );
}                

void do_memory( CHAR_DATA *ch, char *argument )
{
    char       buf [ MAX_STRING_LENGTH ];

    if ( ch->level >= LEVEL_IMMORTAL )
    {
      sprintf( buf, "Affects   %5d\n\r", top_affect    ); send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "Areas     %5d\n\r", top_area      ); send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "ExDes     %5d\n\r", top_ed        ); send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "Exits     %5d\n\r", top_exit      ); send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "Helps     %5d\n\r", top_help      ); send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "Mobs      %5d(%d)\n\r", top_mob_index, mobs_in_game ); send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "Objs      %5d\n\r", top_obj_index ); send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "Resets    %5d\n\r", top_reset     ); send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "Rooms     %5d\n\r", top_room      ); send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "Shops     %5d\n\r", top_shop      ); send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "MobProgs  %5d\n\r", num_mob_progs ); send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "TrapProgs %5d\n\r", num_trap_progs); send_to_char(C_DEFAULT, buf, ch );

      sprintf( buf, "Strings %5d strings of %7d bytes (max %d).\n\r",
	      nAllocString, sAllocString, MAX_STRING );
      send_to_char(C_DEFAULT, buf, ch );

      sprintf( buf, "Perms   %5d blocks of %7d bytes.  Max %7d bytes.\n\r",
	      nAllocPerm, sAllocPerm, 10*1024*1024 );
      send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "%3d%% of max used.\n\r", (sAllocPerm*10) / (1024*1024));
      send_to_char(C_DEFAULT, buf, ch );

      sprintf( buf, "MemAllocated: %d MemFreed: %d Memory being used: %d\n\r", 
               MemAllocated, MemFreed, MemAllocated-MemFreed );
      send_to_char(C_DEFAULT, buf, ch );
      sprintf( buf, "%3d%% of %d.\n\r",
               (((MemAllocated-MemFreed)*10)/(1024*1024)), (10*1024*1024));
      send_to_char(C_DEFAULT, buf, ch );
      return;
    }

    sprintf( buf, "Mem used: %d bytes of %d max.\n\r",
	     sAllocPerm, 10*1024*1024 );
    sprintf( buf + strlen(buf), "%3d%% of max used.\n\r",
	     (sAllocPerm*10) / (1024*1024) );

    send_to_char(C_DEFAULT, buf, ch );
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
    if ( ( to = to - from + 1 ) <= 1 )
	return from;

    return from+(number_mm() % to);
}



/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
    return number_mm() % 100 + 1;
}



/*
 * Generate a random door.
 */
int number_door( void )
{
    return number_mm() % 6;
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
 * Mixture of str_cmp and str_prefix.  An astrisk (*) in astr will cause
 * the equivilant of str_prefix( (astr up to, but not including *), bstr );
 * -- Alty
 */
bool str_cmp_ast( const char *astr, const char *bstr )
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
	if ( *astr == '*' )
	    return FALSE;
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
	send_to_char(C_DEFAULT, "Could not open the file!\n\r", ch );
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
 * Info channel.. -- Alty
 */
void info( const char *str, int param1, int param2 )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  
  strcpy(buf, "[&wINFO&B]&w: &C");
  sprintf(buf+16, str, param1, param2);
  for ( d = descriptor_list; d; d = d->next )
  {
    CHAR_DATA *ch = (d->original ? d->original : d->character);
    
    if ( d->connected == CON_PLAYING && !IS_SET(ch->deaf, CHANNEL_INFO) )
    {
      send_to_char( AT_BLUE, buf, d->character );
      send_to_char( C_DEFAULT, "\n\r", d->character );
    }
  }
}

void arena_chann( const char *str, int param1, int param2 )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  strcpy(buf, "[&cARENA&W]&w: ");
  sprintf(buf+15, str, param1, param2);
  for ( d = descriptor_list; d; d = d->next )
  {
    CHAR_DATA *ch = (d->original ? d->original : d->character);

    if ( d->connected == CON_PLAYING && !IS_SET(ch->deaf, CHANNEL_ARENA) )
    {
      send_to_char( AT_WHITE, buf, d->character );
      send_to_char( C_DEFAULT, "\n\r", d->character );
    }
  }
  return;
}

void challenge( const char *str, int param1, int param2 )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  
  strcpy(buf, "[&cCHALLENGE&W]&w: ");
  sprintf(buf+19, str, param1, param2);
  for ( d = descriptor_list; d; d = d->next )
  {
    CHAR_DATA *ch = (d->original ? d->original : d->character);
    
    if ( d->connected == CON_PLAYING && !IS_SET(ch->deaf, CHANNEL_CHALLENGE) )
    {
      send_to_char( AT_WHITE, buf, d->character );
      send_to_char( C_DEFAULT, "\n\r", d->character );
    }
  }
}

/*
 * Reports a bug.
 */
void bug( const char *str, int param )
{
    FILE *fp;
    char  buf [ MAX_STRING_LENGTH ];

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
		while ( getc( fpArea ) != '\n' && !feof(fpArea) )
		    ;
	    }
	    fseek( fpArea, iChar, 0 );
	}

	sprintf( buf, "[*****] FILE: %s LINE: %d", strArea, iLine );
	log_string( buf, CHANNEL_NONE, -1 );

	if ( ( fp = fopen( "shutdown.txt", "a" ) ) )
	{
	    fprintf( fp, "[*****] %s\n", buf );
	    fclose( fp );
	}
    }

    strcpy( buf, "[*****] BUG: " );
    sprintf( buf + strlen( buf ), str, param );
    log_string( buf, 1 , -1 );

/*    fclose( fpReserve );
    if ( ( fp = fopen( BUG_FILE, "a" ) ) )
    {
	fprintf( fp, "%s\n", buf );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );*/

    return;
}

/*
 * Send logs to imms.
 * Added by Altrag.
 */
void logch( char *l_str, int l_type, int lvl )
{
	DESCRIPTOR_DATA *d;
	int level;
	char log_str[MAX_STRING_LENGTH];
	
	switch ( l_type )
	{
	default:
		strcpy( log_str, "Unknown: " );
		level = L_CON;
		if ( lvl > level )
		  level = lvl;
		break;
	case 1:
		strcpy( log_str, "Coder: " );
		level = 100000;
		break;
	case CHANNEL_LOG:
		strcpy( log_str, "Log: " );
		level = L_DIR;
		if ( lvl > level )
		  level = lvl;
		break;
	case CHANNEL_BUILD:
		strcpy( log_str, "Build: " );
		level = L_DIR;
		if ( lvl > level )
		  level = lvl;
		break;
	case CHANNEL_GOD:
		strcpy( log_str, "God: " );
		level = L_CON;
		if ( lvl > level )
		  level = lvl;
		break;
	}
	strcat( log_str, l_str );
	
	for ( d = descriptor_list; d; d = d->next )
	{
	  if ( d->connected != CON_PLAYING || IS_SET( d->character->deaf, l_type )
	       || get_trust( d->character ) < level ||
	       !IS_SET(d->character->wiznet,WIZ_OLDLOG) )
	    continue;
		send_to_char( AT_PURPLE, log_str, d->character );
		/*
		 * \n\r could have been added earlier,
		 * but need to send a C_DEFAULT line anywayz
		 * Altrag.
		 */
		send_to_char( C_DEFAULT, "\n\r", d->character );
	}
	return;
}

/*
 * Writes a string to the log.
 */
void log_string( char *str, int l_type, int level )
{
    char *strtime;

    strtime                    = ctime( &current_time );
    strtime[strlen( strtime )-1] = '\0';
    fprintf( stderr, "%s :: %s\n", strtime, str );

	/*
	 * The Actual Implementation of the Log Channels.
	 * Added by Altrag.
	 */
    if ( l_type != CHANNEL_NONE )
    	logch( str, l_type, level );
    return;
}

char *wind_str( int str )
{
  int temp = 6;
  
  if ( str> 200 )
   temp = 1;
  else if ( str > 150 )
   temp = 2;
  else if ( str > 100 )
   temp = 3;
  else if ( str > 50 )
   temp = 4;
  else if ( str > 0 )
   temp = 5;
      
  switch( temp )
  {
    case 1: return "impossibly strongly";
    case 2: return "very strongly";
    case 3: return "strongly";
    case 4: return "moderately strongly";
    case 5: return "lightly";
    default: return "wierdly";
  }
  return "";
}


void wind_update( AREA_DATA *pArea )
{
/*  extern const char * dir_wind[];
  char buf[MAX_STRING_LENGTH];
  int change = 0;
  int cdir = 0;
  int  nwd = pArea->winddir;
  int nws = pArea->windstr;
  
  if ( number_percent() <= 50 )
  {
    change = number_range(-50, 50);
    if ( number_bits(2) == 0 )
    {
      if ( change + nws <= 70 )
      {
        nws += change;
      }
     else
      {
        if ( number_percent() < 5 )
        {
          if ( nws + change > 250 )
 	    nws = 250;
          else
	    nws += change;
        }
      }
    }
   else
    { 
      if ( nws - change < 0 )
        nws = 0;
      else
        nws -= change;
    }
  }
    
    if ( number_percent() < 20 )
    {
      cdir = number_fuzzy(0);
    
      if ( ( nwd == 0 ) && ( cdir == -1 ) )
	nwd = 3;
      else if ( ( nwd == 3 ) && ( cdir == 1 ) )
        nwd = 0;
      else
        nwd += cdir;
    }
    
    if ( ( nws <= 0 ) && ( nws != pArea->windstr ) )
    {
      send_to_area( pArea, "The wind dies down.\n\r" );
      pArea->winddir = nwd;
      pArea->windstr = nws;
      return;
    }
        
    if ( ( nwd != pArea->winddir ) && ( nws != pArea->windstr ) )
    {
      sprintf( buf, "The wind shifts directions and begins blowing %s from the %s.\n\r",
         wind_str( nws ), dir_name[rev_dir[nwd]] );
      send_to_area( pArea, buf );
      pArea->winddir = nwd;
      pArea->windstr = nws;
      return;
    }

    if ( ( nwd != pArea->winddir ) && ( nws == pArea->windstr ) )
    {
      sprintf( buf, "The wind begins blowing from the %s at the same strength as before.\n\r",
      dir_name[rev_dir[nwd]] );
      send_to_area( pArea, buf );
      pArea->winddir = nwd;
      return;
    }

    if ( ( nwd == pArea->winddir )  && ( nws != pArea->windstr ) )
    {
      sprintf( buf, "The wind begins blowing %s, still from the %s.\n\r",
        wind_str( nws ), dir_name[rev_dir[nwd]] );
      send_to_area( pArea, buf );
      pArea->windstr = nws;
      return;
    }*/
  return;
}  

/*
void do_aload( CHAR_DATA *ch, char *argument )
{
  
  return;
}
*/
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

void area_sort( AREA_DATA *pArea )
{
  AREA_DATA *fArea;

  if ( !pArea )
  {
    bug( "area_sort: NULL pArea", 0); /* MAJOR probs if you ever see this.. */
    return;
  }

  area_last = pArea;

  if ( !area_first )
  {
    area_first = pArea;
    return;
  }

  for ( fArea = area_first; fArea; fArea = fArea->next )
  {
    if ( pArea->lvnum == fArea->lvnum ||
       ( pArea->lvnum > fArea->lvnum &&
       (!fArea->next || pArea->lvnum < fArea->next->lvnum) ) )
    {
      pArea->next = fArea->next;
      fArea->next = pArea;
      return;
    }
  }
  pArea->next = area_first;
  area_first = pArea;
  return;
}

void clan_sort( CLAN_DATA *pClan )
{
  CLAN_DATA *fClan;

  if ( !clan_first )
  {
    clan_first = pClan;
    return;
  }
  for ( fClan = clan_first; fClan; fClan = fClan->next )
  {
    if ( pClan->vnum == fClan->vnum ||
       ( pClan->vnum > fClan->vnum &&
       (!fClan->next || pClan->vnum < fClan->next->vnum) ) )
    {
      pClan->next = fClan->next;
      fClan->next = pClan;
      return;
    }
  }
  pClan->next = clan_first;
  clan_first = pClan;
  return;
}
BAN_DATA *ban_free;
BAN_DATA *ban_list;

void load_banlist( void )
{
  BAN_DATA *pban;
  FILE *fp;
  char type;
  char *banned;

  if ( !(fp=fopen( BAN_LIST, "r" )) )
    return;
    fpArea = fp;
    strcpy(strArea, BAN_LIST);

  for ( ; ; )
    {
    type = fread_letter( fp );
    if ( type == '$' )
	break;
    else if ( type != 'P'
	   && type != 'T'
	   && type != 'N' )
	{
	log_string( "Unknown ban type, ignoring entry.", -1, -1 );
        banned = fread_string( fp );
	continue;
	}
    if ( !ban_free )
	pban = alloc_perm( sizeof( *pban ) );
    else
	{
	pban	 = ban_free;
	ban_free = ban_free->next;
	}
    pban->type	= type;
    banned	= fread_string( fp );
    parse_ban( banned, pban );
    pban->next	 = ban_list;
    ban_list	 = pban;
    }
    fclose(fp);
    return;
}

void parse_ban( char *argument, BAN_DATA *banned )
{
  char user [ MAX_INPUT_LENGTH ];
  char name [ MAX_INPUT_LENGTH ];
  char *ban, *is_at;
  bool found = FALSE;
  int u, n;

  is_at = strchr( argument, '@' );
  if ( !is_at )
    {
    banned->name = strdup( argument );
    return;
    }
  u = n = 0;
  for ( ban = argument; *ban != '\0'; ban++ )
    {
    if ( *ban == '@' )
	found = TRUE;
    if ( *ban != '@' && !found )
	{
	user[u] = *ban;
	u++;
	}
    else if ( *ban != '@' && found )
	{
	name[n] = *ban;
	n++;
	}
    }
  if ( name[n] != '\0' )
	name[n] = '\0';
  if ( user[u] != '\0' )
	user[u] = '\0';
  banned->name = str_dup( name );
  banned->user = str_dup( user );
  return;
}

void load_player_list( void )
{
    FILE      *fp;
    PLAYERLIST_DATA *pPlayerList;
    char buf[MAX_STRING_LENGTH];
    struct stat   statis;

    /* Check if we have a player list */
    if ( !( fp = fopen( PLAYERLIST_FILE, "r" ) ) )
        return;

    fpArea = fp;
    strcpy(strArea, PLAYERLIST_FILE);
/*	fBootDb = TRUE; */

    /* Load the player list */
    for ( ; ; )
    {
        char*  name;

        pPlayerList             = alloc_perm( sizeof( PLAYERLIST_DATA ));
        pPlayerList->name      	= &str_empty[0];
        pPlayerList->level 	= 0;
        pPlayerList->clan_name 	= NULL; /*&str_empty[0]; */
        pPlayerList->clan_rank 	= 0;
        pPlayerList->guild_name	= NULL; /*&str_empty[0];*/
        pPlayerList->guild_rank	= 0;

        /* Load a character */
	for ( ; ; )
        {
            name   = fread_word( fp );
/*            bug ( name, -1 ); */
            /* Check if we are at the end of the player list */
            if ( !str_cmp( name, "#END"    ) )
            {
               fclose( fp );
               return;
	    }
            else if ( !str_cmp( name, "End" ) )
              break;
	    else if ( !str_cmp( name, "Name" ) )
              pPlayerList->name      	= fread_string( fp );
            else if ( !str_cmp( name, "Lvl" ) )
              pPlayerList->level 	= fread_number( fp );
            else if ( !str_cmp( name, "Clan" ) )       
              pPlayerList->clan_name 	= fread_string( fp );
            else if ( !str_cmp( name, "CRank" ) ) 		
              pPlayerList->clan_rank 	= fread_number( fp );
            else if ( !str_cmp( name, "Guild" ) )		
              pPlayerList->guild_name 	= fread_string( fp );
            else if ( !str_cmp( name, "GRank" ) )		
              pPlayerList->guild_rank 	= fread_number( fp );
 	}

        /* check if the player still exits */
        sprintf( buf, "%s%c/%s.gz", PLAYER_DIR, LOWER(pPlayerList->name[0]),
            capitalize( pPlayerList->name ) );
        if ( stat( buf, &statis ) ) 
        {
          sprintf( buf, "%s%c/%s", PLAYER_DIR, LOWER(pPlayerList->name[0]),
            capitalize( pPlayerList->name ) );
          if ( stat( buf, &statis ) ) 
            continue;
        }

        if ( !playerlist )
        {
          pPlayerList->next = NULL;
          playerlist = pPlayerList;
        }
	else
        {        
          pPlayerList->next = playerlist;
          playerlist = pPlayerList;
        }
   }
   fclose(fp);
}

void save_player_list( )
{
    FILE      *fp;
    PLAYERLIST_DATA *pPlayer;


    if ( ( fp = fopen( PLAYERLIST_FILE, "w" ) ) == NULL ) 
    {
	bug( "Save_Playerlist: fopen", 0 ); 
	perror( "playerlist.lst" ); 
    }

    for ( pPlayer = playerlist;  pPlayer; pPlayer = pPlayer->next )
    {
      fprintf( fp, "Name        %s~\n",   pPlayer->name         );
      fprintf( fp, "Lvl         %d\n" ,   pPlayer->level	);
      if (pPlayer->clan_name)
      fprintf( fp, "Clan        %s~\n" ,   pPlayer->clan_name    );
      if (pPlayer->clan_rank)
      fprintf( fp, "CRank       %d\n" ,   pPlayer->clan_rank    );
      if (pPlayer->guild_name)
      fprintf( fp, "Guild       %s~\n",   pPlayer->guild_name	);   
      if (pPlayer->guild_rank)
      fprintf( fp, "GRank       %d\n" ,   pPlayer->guild_rank   );
      fprintf( fp, "END\n" ); 
    }
    fprintf( fp, "#END\n" ); 
    fclose( fp );
}

void delete_playerlist( char * name )
{
    PLAYERLIST_DATA * pPlayerList;
    PLAYERLIST_DATA * pPrevPlayerList;


    pPlayerList = playerlist;
    if (!str_cmp( name, playerlist->name ) )
    {
      playerlist = playerlist->next;
      free_mem( pPlayerList, sizeof( *pPlayerList ) );
      return;
    }

    for ( pPlayerList = playerlist->next, pPrevPlayerList = playerlist;
          pPlayerList;
          pPrevPlayerList = pPlayerList, pPlayerList = pPlayerList->next)
    {
      if (!str_cmp( name, pPlayerList->name ) )
      {
        pPrevPlayerList->next = pPlayerList->next;
        free_mem( pPlayerList, sizeof( *pPlayerList ) ); 
        return;
      }
    }

}

PLAYERLIST_DATA * name_in_playerlist( char *name )
{
    PLAYERLIST_DATA *pPlayer;

    for ( pPlayer = playerlist; pPlayer; pPlayer = pPlayer->next )
    {
      if (!str_cmp( name, pPlayer->name ) )
       return pPlayer;
    }
   return 0;
}

void update_playerlist( CHAR_DATA *ch )
{
    PLAYERLIST_DATA *pPlayer;

    if ( !(pPlayer = name_in_playerlist( ch->name )))
    {
      add_playerlist(ch);
      return;
    }


      pPlayer->level		= ch->level;

      free_string( pPlayer->clan_name );
      if ( ch->clan )
        pPlayer->clan_name	= str_dup( (get_clan_index(ch->clan))->name );
      else
        pPlayer->clan_name 	= NULL; /*&str_empty[0]; */
      pPlayer->clan_rank	= ch->clev;
      free_string( pPlayer->guild_name );
      if ( ch->guild )
       pPlayer->guild_name	= str_dup( ch->guild->name );
      else
       pPlayer->guild_name	= NULL; /*&str_empty[0]; */
      pPlayer->guild_rank	= ch->guild_rank;

}

void add_playerlist(CHAR_DATA *ch)
{
   PLAYERLIST_DATA *pPlayer;

   if ( (pPlayer = name_in_playerlist( ch->name )))
   {
      update_playerlist(ch);
      return;
   }

    pPlayer             = alloc_perm( sizeof( PLAYERLIST_DATA ));

   
   pPlayer->name 	= str_dup( ch->name );
   pPlayer->level	= ch->level;
   if ( ch->clan )
     pPlayer->clan_name      = str_dup((get_clan_index(ch->clan))->name );
   else 
     pPlayer->clan_name      = NULL; /*&str_empty[0]; */
   pPlayer->clan_rank = ch->clev;
      if ( ch->guild )
       pPlayer->guild_name      = str_dup( ch->guild->name );
      else
       pPlayer->guild_name      = NULL; /*&str_empty[0]; */

   pPlayer->guild_rank	= ch->guild_rank;
   

   if ( !playerlist )
   {
      pPlayer->next = NULL;
      playerlist = pPlayer;
   }
   else
   {        
      pPlayer->next = playerlist;
      playerlist = pPlayer;
   }

}

/* Angi -- Newbie help  */
void newbie_sort( NEWBIE_DATA *pNewbie )
{
  NEWBIE_DATA *fNewbie;

  if ( !newbie_first )
  {
    newbie_first = pNewbie;
    newbie_last  = pNewbie;
    return;
  }

  if ( strncmp( pNewbie->keyword, newbie_first->keyword, 256 ) > 0 )
  {
   pNewbie->next = newbie_first->next;
   newbie_first = pNewbie;
   return;
  }

  for ( fNewbie = newbie_first; fNewbie; fNewbie = fNewbie->next )
  {
    if (    ( strncmp( pNewbie->keyword, fNewbie->keyword, 256 ) < 0 ) )
    {
      if ( fNewbie != newbie_last )
      {
	pNewbie->next = fNewbie->next;
	fNewbie->next = pNewbie;
        return;
       }
    }
  }
  newbie_last->next = pNewbie;
  newbie_last = pNewbie;
  pNewbie->next = NULL;

  return;

}

void load_newbie( void )
{
    FILE      *fp;
    NEWBIE_DATA *pNewbieIndex;
    char letter;

    if ( !( fp = fopen( NEWBIE_FILE, "r" ) ) )
        return;
    fpArea = fp;
    strcpy(strArea, NEWBIE_FILE);
    for ( ; ; )
    {
        char*  keyword;

        letter                          = fread_letter( fp );
        if ( letter != '#' )
        {
            bug( "Load_newbie: # not found.", 0 );
            continue;
        }
        keyword                         = fread_string( fp );
        if ( !str_cmp( keyword, "END"    ) )
          break;
	
 	pNewbieIndex		=	alloc_perm( sizeof( *pNewbieIndex ));
	pNewbieIndex->keyword		=	keyword;
	pNewbieIndex->answer1		=	fread_string( fp );
	pNewbieIndex->answer2		=	fread_string( fp );

	newbie_sort(pNewbieIndex);
	top_newbie++;

    }
    fclose ( fp );

    return;
}



/* Decklarean */

void race_sort( RACE_DATA *pRace )
{
  RACE_DATA *fRace;

  if ( !first_race )
  {
    first_race = pRace;
    return;
  }
  for ( fRace = first_race; fRace; fRace = fRace->next )
  {
    if (    pRace->vnum == fRace->vnum
         || (    pRace->vnum > fRace->vnum
              && (    !fRace->next
                   || pRace->vnum < fRace->next->vnum
                 )
            )
       )
    {
      pRace->next = fRace->next;
      fRace->next = pRace;
      return;
    }
  }
  pRace->next = first_race;
  first_race = pRace;
  return;
}


void load_race( void )
{
    FILE      *fp;
    RACE_DATA *pRace;

    /* Check if we have a race list. */
    if ( !( fp = fopen( RACE_FILE, "r" ) ) )
        return;

    fpArea = fp;
    strcpy(strArea, RACE_FILE);

    /* Load the race list */
    for ( ; ; )
    {
        char*  name;

        pRace = new_race_data( );

        /* Load a character */
	for ( ; ; )
        {
            name   = fread_word( fp );

            /* Check if we are at the end of the race list */
            if ( !str_cmp( name, "#END"    ) )
            {
               fclose( fp );
               return;
	    }
            else if ( !str_cmp( name, "End" ) )
              break;
            else if ( !str_cmp( name, "VNum" ) )       
              pRace->vnum       = fread_number( fp );
            else if ( !str_cmp( name, "Race_Full" ) )
              pRace->race_full  = fread_string( fp );
            else if ( !str_cmp( name, "Race_Name" ) )
              pRace->race_name  = fread_string( fp );
            else if ( !str_cmp( name, "MStr" ) )       
              pRace->mstr       = fread_number( fp );
            else if ( !str_cmp( name, "MInt" ) )       
              pRace->mint       = fread_number( fp );
            else if ( !str_cmp( name, "MWis" ) )       
              pRace->mwis       = fread_number( fp );
            else if ( !str_cmp( name, "MDex" ) )       
              pRace->mdex       = fread_number( fp );
            else if ( !str_cmp( name, "MCon" ) )       
              pRace->mcon       = fread_number( fp );
 	}
        race_sort( pRace );
    }
  fclose(fp);
}


RACE_DATA *get_race_data( int vnum )
{
    RACE_DATA *pRace;

    for ( pRace  = first_race;
          pRace;
          pRace  = pRace->next )
    {
        if ( pRace->vnum == vnum )
            return pRace;
    }

    if ( fBootDb )
    {
        bug( "Get_race_data: bad race %d.", vnum );
    }

    return NULL;
}


