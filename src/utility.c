/***************************************************************************
 *  file: utility.c, Utility module.                       Part of DIKUMUD *
 *  Usage: Utility procedures                                              *
 *  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
 *                                                                         *
 *  Copyright (C) 1992, 1993 Michael Chastain, Michael Quan, Mitchell Tse  *
 *  Performance optimization and bug fixes by MERC Industries.             *
 *  You can use our stuff in any way you like whatsoever so long as this   *
 *  copyright notice remains intact.  If you like it please drop a line    *
 *  to mec@garnet.berkeley.edu.                                            *
 *                                                                         *
 *  This is free software and you are benefitting.  We hope that you       *
 *  share your changes too.  What goes around, comes around.               *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "utils.h"

extern struct time_data time_info;

char    log_buf[512];



/*
 * Generates a random number.
 */
int number( int from, int to )
{
    if ( from >= to )
	return from;

    return from + random() % (to - from + 1);
}



/*
 * Simulates dice roll.
 */
int dice( int number, int size )
{
    int r;
    int sum = number;

    for ( r = 1; r <= number; r++ )
	sum += random() % size;

    return sum;
}



/*
 * Compare strings, case insensitive.
 */
int str_cmp( char *arg1, char *arg2 )
{
    int check, i;

    for ( i = 0; arg1[i] || arg2[i]; i++ )
    {
	check = LOWER(arg1[i]) - LOWER(arg2[i]);
	if ( check < 0 )
	    return -1;
	if ( check > 0 )
	    return 1;
    }

    return 0;
}



/*
 * Duplicate a string into dynamic memory.
 */
char *str_dup( const char *str )
{
    char *str_new;

    CREATE( str_new, char, strlen(str) + 1 );
    strcpy( str_new, str );
    return str_new;
}



/* writes a string to the log */
void log( char *str )
{
    long ct;
    char *tmstr;

    ct		= time(0);
    tmstr	= asctime(localtime(&ct));
    *(tmstr + strlen(tmstr) - 1) = '\0';
    fprintf(stderr, "%s :: %s\n", tmstr, str);
}



void sprintbit( long vektor, char *names[], char *result )
{
    long nr;

    *result = '\0';

    for ( nr=0; vektor; vektor>>=1 )
    {
	if ( IS_SET(1, vektor) )
	{
	    if ( *names[nr] != '\n' )
		strcat( result, names[nr] );
	    else
		strcat( result, "Undefined" );
	    strcat( result, " " );
	}

	if ( *names[nr] != '\n' )
	  nr++;
    }

    if ( *result == '\0' )
	strcat( result, "NoBits" );
}



void sprinttype( int type, char *names[], char *result )
{
    int nr;

    for ( nr = 0; *names[nr] != '\n'; nr++ )
	;
    if ( type < nr )
	strcpy( result, names[type] );
    else
	strcpy( result, "Undefined" );
}


/* Calculate the MUD time passed over the last t2-t1 centuries (secs) */
struct time_info_data mud_time_passed(time_t t2, time_t t1)
{
    long secs;
    struct time_info_data now;

    secs = (long) (t2 - t1);

    now.hours = (secs/SECS_PER_MUD_HOUR) % 24;  /* 0..23 hours */
    secs -= SECS_PER_MUD_HOUR*now.hours;

    now.day = (secs/SECS_PER_MUD_DAY) % 35;     /* 0..34 days  */
    secs -= SECS_PER_MUD_DAY*now.day;

    now.month = (secs/SECS_PER_MUD_MONTH) % 17; /* 0..16 months */
    secs -= SECS_PER_MUD_MONTH*now.month;

    now.year = (secs/SECS_PER_MUD_YEAR);        /* 0..XX? years */

    return now;
}



struct time_info_data age(struct char_data *ch)
{
    struct time_info_data player_age;

    player_age = mud_time_passed(time(0),ch->player.time.birth);

    player_age.year += 17;   /* All players start at 17 */

    return player_age;
}
