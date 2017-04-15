/* ************************************************************************
*   File: utility.c                                     Part of CircleMUD *
*  Usage: various internal functions of a utility nature                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <arpa/telnet.h>
#include <netinet/in.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "screen.h"

extern struct time_data time_info;

int	MIN(int a, int b)
{
   return a < b ? a : b;
}


int	MAX(int a, int b)
{
   return a > b ? a : b;
}


/* creates a random number in interval [from;to] */
int	number(int from, int to)
{
   return((random() % (to - from + 1)) + from);
}



/* simulates dice roll */
int	dice(int number, int size)
{
   int	r;
   int	sum = 0;

   assert(size >= 1);

   for (r = 1; r <= number; r++)
      sum += ((random() % size) + 1);
   return(sum);
}



/* Create a duplicate of a string */
char	*str_dup(const char *source)
{
   char	*new;

   CREATE(new, char, strlen(source) + 1);
   return(strcpy(new, source));
}



/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different or end of both                 */
int	str_cmp(char *arg1, char *arg2)
{
   int	chk, i;

   for (i = 0; *(arg1 + i) || *(arg2 + i); i++)
      if ((chk = LOWER(*(arg1 + i)) - LOWER(*(arg2 + i))))
	 if (chk < 0)
	    return (-1);
	 else
	    return (1);
   return(0);
}



/* returns: 0 if equal, 1 if arg1 > arg2, -1 if arg1 < arg2  */
/* scan 'till found different, end of both, or n reached     */
int	strn_cmp(char *arg1, char *arg2, int n)
{
   int	chk, i;

   for (i = 0; (*(arg1 + i) || *(arg2 + i)) && (n > 0); i++, n--)
      if ((chk = LOWER(*(arg1 + i)) - LOWER(*(arg2 + i))))
	 if (chk < 0)
	    return (-1);
	 else
	    return (1);

   return(0);
}


/* log a death trap hit */
void	log_death_trap(struct char_data *ch)
{
   char	buf[150];
   extern struct room_data *world;

   sprintf(buf, "%s hit death trap #%d (%s)", GET_NAME(ch),
       world[ch->in_room].number, world[ch->in_room].name);
   mudlog(buf, BRF, LEVEL_IMMORT, TRUE);
}


/* writes a string to the log */
void	log(char *str)
{
   long	ct;
   char	*tmstr;

   ct = time(0);
   tmstr = asctime(localtime(&ct));
   *(tmstr + strlen(tmstr) - 1) = '\0';
   fprintf(stderr, "%-19.19s :: %s\n", tmstr, str);
}


/* New PROC: syslog by Fen Jul 3, 1992 */

void mudlog(char *str, char type, sbyte level, byte file)

{
   char	buf[256];
   extern struct descriptor_data *descriptor_list;
   struct descriptor_data *i;
   char *tmp;
   long	ct;  
   char	tp;

   ct  = time(0);
   tmp = asctime(localtime(&ct));

   if (file)
      fprintf(stderr, "%-19.19s :: %s\n", tmp, str);
   if (level<0)
      return;
  
    sprintf(buf, "[ %s ]\n\r", str );

    for (i=descriptor_list; i; i = i->next) 
       if (!i->connected && !PLR_FLAGGED(i->character, PLR_WRITING)) {
          tp = ((PRF_FLAGGED(i->character, PRF_LOG1) ? 1 : 0) +
                (PRF_FLAGGED(i->character, PRF_LOG2) ? 2 : 0));

          if ((GET_LEVEL(i->character)>=level) && (tp >= type)) {
	     send_to_char(CCGRN(i->character, C_NRM), i->character);
             send_to_char(buf, i->character);
	     send_to_char(CCNRM(i->character, C_NRM), i->character);
	  }
       }
   return;
}

/* End of Modification */



void	sprintbit(long vektor, char *names[], char *result)
{
   long	nr;

   *result = '\0';

   if (vektor < 0) {
      strcpy(result, "SPRINTBIT ERROR!");
      return;
   }

   for (nr = 0; vektor; vektor >>= 1) {
      if (IS_SET(1, vektor)) {
	 if (*names[nr] != '\n') {
	    strcat(result, names[nr]);
	    strcat(result, " ");
	 } else
	    strcat(result, "UNDEFINED ");
      }

      if (*names[nr] != '\n')
	 nr++;
   }

   if (!*result)
      strcat(result, "NOBITS");
}



void	sprinttype(int type, char *names[], char *result)
{
   int	nr;

   for (nr = 0; (*names[nr] != '\n'); nr++)
      ;
   if (type < nr)
      strcpy(result, names[type]);
   else
      strcpy(result, "UNDEFINED");
}


/* Calculate the REAL time passed over the last t2-t1 centuries (secs) */
struct time_info_data real_time_passed(time_t t2, time_t t1)
{
   long	secs;
   struct time_info_data now;

   secs = (long) (t2 - t1);

   now.hours = (secs / SECS_PER_REAL_HOUR) % 24;  /* 0..23 hours */
   secs -= SECS_PER_REAL_HOUR * now.hours;

   now.day = (secs / SECS_PER_REAL_DAY);          /* 0..34 days  */
   secs -= SECS_PER_REAL_DAY * now.day;

   now.month = -1;
   now.year  = -1;

   return now;
}



/* Calculate the MUD time passed over the last t2-t1 centuries (secs) */
struct time_info_data mud_time_passed(time_t t2, time_t t1)
{
   long	secs;
   struct time_info_data now;

   secs = (long) (t2 - t1);

   now.hours = (secs / SECS_PER_MUD_HOUR) % 24;  /* 0..23 hours */
   secs -= SECS_PER_MUD_HOUR * now.hours;

   now.day = (secs / SECS_PER_MUD_DAY) % 35;     /* 0..34 days  */
   secs -= SECS_PER_MUD_DAY * now.day;

   now.month = (secs / SECS_PER_MUD_MONTH) % 17; /* 0..16 months */
   secs -= SECS_PER_MUD_MONTH * now.month;

   now.year = (secs / SECS_PER_MUD_YEAR);        /* 0..XX? years */

   return now;
}



struct time_info_data age(struct char_data *ch)
{
   struct time_info_data player_age;

   player_age = mud_time_passed(time(0), ch->player.time.birth);

   player_age.year += 17;   /* All players start at 17 */

   return player_age;
}




/*
** Turn off echoing (specific to telnet client)
*/

void	echo_off(int sock)
{
   char	off_string[] = 
    {
      (char) IAC,
      (char) WILL,
      (char) TELOPT_ECHO,
      (char)  0,
   };

   (void) write(sock, off_string, sizeof(off_string));
}


/*
** Turn on echoing (specific to telnet client)
*/


void	echo_on(int sock)
{
   char	off_string[] = 
    {
      (char) IAC,
      (char) WONT,
      (char) TELOPT_ECHO,
      (char) TELOPT_NAOFFD,
      (char) TELOPT_NAOCRD,
      (char)  0,
   };

   (void) write(sock, off_string, sizeof(off_string));
}


