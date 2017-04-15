/* ************************************************************************
*   File: ban.c                                         Part of CircleMUD *
*  Usage: banning/unbanning/checking sites and player names               *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"

struct ban_list_element *ban_list = 0;
extern char	*ban_types[];


void	load_banned (void)
{
   FILE * fl;
   int	i, date;
   char	site_name[BANNED_SITE_LENGTH+1], ban_type[100];
   char	name[MAX_NAME_LENGTH+1];
   struct ban_list_element *next_node;

   ban_list = 0;

   if (!(fl = fopen(BAN_FILE, "r"))) {
      perror("Unable to open banfile");
      return;
   }

   while (fscanf(fl, " %s %s %d %s ", ban_type, site_name, &date, name) == 4) {
      CREATE(next_node, struct ban_list_element, 1);
      strncpy(next_node->site, site_name, BANNED_SITE_LENGTH);
      next_node->site[BANNED_SITE_LENGTH] = '\0';
      strncpy(next_node->name, name, MAX_NAME_LENGTH);
      next_node->name[MAX_NAME_LENGTH] = '\0';
      next_node->date = date;

      for (i = BAN_NOT; i <= BAN_ALL; i++)
	 if (!strcmp(ban_type, ban_types[i]))
	    next_node->type = i;

      next_node->next = ban_list;
      ban_list = next_node;
   }

   fclose(fl);
}


int	isbanned(char *hostname)
{
   int	i;
   struct ban_list_element *banned_node;
   char	*nextchar;

   if (!hostname || !*hostname)
      return(0);

   i = 0;
   for (nextchar = hostname; *nextchar; nextchar++)
      *nextchar = tolower(*nextchar);

   for (banned_node = ban_list; banned_node; banned_node = banned_node->next)
      if (strstr(hostname, banned_node->site)) /* if hostname is a substring */
	 i = MAX(i, banned_node->type);

   return i;
}


void	_write_one_node(FILE *fp, struct ban_list_element *node)
{
   if (node) {
      _write_one_node(fp, node->next);
      fprintf(fp, "%s %s %d %s\n", ban_types[node->type],
          node->site, node->date, node->name);
   }
}



void	write_ban_list(void)
{
   FILE * fl;

   if (!(fl = fopen(BAN_FILE, "w"))) {
      perror("write_ban_list");
      return;
   }

   _write_one_node(fl, ban_list);  /* recursively write from end to start */
   fclose(fl);
   return;
}


ACMD(do_ban)
{
   char	flag[80], site[80], format[50], *nextchar, *timestr;
   int	i;
   struct ban_list_element *ban_node;

   if (IS_NPC(ch)) {
      send_to_char("You Beast!!\n\r", ch);
      return;
   }

   strcpy(buf, "");
   if (!*argument) {
      if (!ban_list) {
	 send_to_char("No sites are banned.\n\r", ch);
	 return;
      }
      strcpy(format, "%-25.25s  %-8.8s  %-10.10s  %-16.16s\n\r");
      sprintf(buf, format,
          "Banned Site Name",
          "Ban Type",
          "Banned On",
          "Banned By");
      send_to_char(buf, ch);
      sprintf(buf, format,
          "---------------------------------",
          "---------------------------------",
          "---------------------------------",
          "---------------------------------");
      send_to_char(buf, ch);

      for (ban_node = ban_list; ban_node; ban_node = ban_node->next) {
	 if (ban_node->date) {
	    timestr = asctime(localtime(&(ban_node->date)));
	    *(timestr + 10) = 0;
	    strcpy(site, timestr);
	 } else
	    strcpy(site, "Unknown");
	 sprintf(buf, format, ban_node->site, ban_types[ban_node->type], site,
	     ban_node->name);
	 send_to_char(buf, ch);
      }
      return;
   }

   half_chop(argument, flag, site);
   if (!*site || !*flag) {
      send_to_char("Usage: ban {all | select | new} site_name\n\r", ch);
      return;
   }

   if (!(!str_cmp(flag, "select") || !str_cmp(flag, "all") || !str_cmp(flag, "new"))) {
      send_to_char("Flag must be ALL, SELECT, or NEW.\n\r", ch);
      return;
   }

   for (ban_node = ban_list; ban_node; ban_node = ban_node->next) {
      if (!str_cmp(ban_node->site, site)) {
	 send_to_char(
	     "That site has already been banned -- unban it to change the ban type.\n\r", ch);
	 return;
      }
   }

   CREATE(ban_node, struct ban_list_element, 1);
   strncpy(ban_node->site, site, BANNED_SITE_LENGTH);
   for (nextchar = ban_node->site; *nextchar; nextchar++)
      *nextchar = tolower(*nextchar);
   ban_node->site[BANNED_SITE_LENGTH] = '\0';
   strncpy(ban_node->name, GET_NAME(ch), MAX_NAME_LENGTH);
   ban_node->name[MAX_NAME_LENGTH] = '\0';
   ban_node->date = time(0);

   for (i = BAN_NEW; i <= BAN_ALL; i++)
      if (!str_cmp(flag, ban_types[i]))
	 ban_node->type = i;

   ban_node->next = ban_list;
   ban_list = ban_node;

   sprintf(buf, "%s has banned %s for %s players.", GET_NAME(ch), site,
       ban_types[ban_node->type]);
   mudlog(buf, NRM, MAX(LEVEL_GOD, GET_INVIS_LEV(ch)), TRUE);
   send_to_char("Site banned.\n\r", ch);
   write_ban_list();
}


ACMD(do_unban)
{
   char	site[80];
   struct ban_list_element *ban_node, *prev_node;
   int	found = 0;

   if (IS_NPC(ch)) {
      send_to_char("You are not godly enough for that!\n\r", ch);
   }

   one_argument(argument, site);
   if (!*site) {
      send_to_char("A site to unban might help.\n\r", ch);
      return;
   }

   ban_node = ban_list;
   while (ban_node && !found) {
      if (!str_cmp(ban_node->site, site))
	 found = 1;
      else
	 ban_node = ban_node->next;
   }

   if (!found) {
      send_to_char("That site is not currently banned.\n\r", ch);
      return;
   }

   /* first element in list */
   if (ban_node == ban_list)
      ban_list = ban_list->next;
   else {
      for (prev_node = ban_list; prev_node->next != ban_node; 
          prev_node = prev_node->next)
	 ;

      prev_node->next = ban_node->next;
   }

   send_to_char("Site unbanned.\n\r", ch);
   sprintf(buf, "%s removed the %s-player ban on %s.",
       GET_NAME(ch), ban_types[ban_node->type], ban_node->site);
   mudlog(buf, NRM, MAX(LEVEL_GOD, GET_INVIS_LEV(ch)), TRUE);

   free(ban_node);
   write_ban_list();
}


/**************************************************************************
 *  Code to check for invalid names (i.e., profanity, etc.)		  *
 *  Written by Sharon P. Goza						  *
 **************************************************************************/

typedef char	namestring[MAX_NAME_LENGTH];

namestring *invalid_list = NULL;
int	num_invalid = 0;

int	Valid_Name(char *newname)
{

   int	i;
   char	*cptr;

   char	tempname[MAX_NAME_LENGTH];

   /* Made the list of invalid names and words a file to avoid a
     recompile when an immortal gets promoted or we think of differnt
     permutations, all names in the list should be in caps */

   /* IF THE LIST COULDN'T BE READ IN FOR A REASON, RETURN VALID */
   if (!invalid_list)
      return(1);

   /* CHANGE THE INPUT STRING TO BE LOWERCASE SO WE ONLY HAVE TO COMPARE ONCE */

   for (i = 0, cptr = newname; *cptr && i < MAX_NAME_LENGTH; )
      tempname[i++] = tolower(*cptr++);

   tempname[i] = 0;


   /* Loop through the invalid list and see if the string occurs
       in the desired name */
   for (i = 0; i < num_invalid; i++)
      if (strstr(tempname, invalid_list[i]))
	 return(0);

   return(1);
}


void	Read_Invalid_List(void)
{
   FILE * fp;
   int	i = 0;
   char	string[80];

   if (!(fp = fopen(XNAME_FILE, "r"))) {
      perror("Unable to open invalid name file");
      return;
   }

   /* count how many records */
   while (!feof(fp)) {
      fscanf(fp, "%s", string);
      num_invalid++;
   }
   rewind(fp);

   CREATE(invalid_list, namestring, num_invalid);

   while (!feof(fp)) {
      fscanf(fp, "%s", invalid_list[i++]);
   }

   /* make sure there are no nulls in there */
   for (i = 0; i < num_invalid && *invalid_list[i]; i++)
      num_invalid = i-i;

   fclose(fp);
}


