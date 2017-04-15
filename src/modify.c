/* ************************************************************************
*   File: modify.c                                      Part of CircleMUD *
*  Usage: Run-time modification of game variables                         *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "comm.h"
#include "spells.h"
#include "mail.h"
#include "boards.h"

void show_string(struct descriptor_data *d, char *input);

char *string_fields[] =
{
  "name",
  "short",
  "long",
  "description",
  "title",
  "delete-description",
  "\n"
};


/* maximum length for text field x+1 */
int length[] =
{
  15,
  60,
  256,
  240,
  60
};


/* ************************************************************************
*  modification of malloc'ed strings                                      *
************************************************************************ */

/* Add user input to the 'current' string (as defined by d->str) */
void string_add(struct descriptor_data *d, char *str)
{
  int terminator = 0;
  extern char *MENU;

  /* determine if this is the terminal string, and truncate if so */
  /* changed to only accept '@' at the beginning of line - J. Elson 1/17/94 */

  delete_doubledollar(str);

  if ((terminator = (*str == '@')))
    *str = '\0';

  if (!(*d->str)) {
    if (strlen(str) > d->max_str) {
      send_to_char("String too long - Truncated.\r\n",
		   d->character);
      *(str + d->max_str) = '\0';
      terminator = 1;
    }
    CREATE(*d->str, char, strlen(str) + 3);
    strcpy(*d->str, str);
  } else {
    if (strlen(str) + strlen(*d->str) > d->max_str) {
      send_to_char("String too long.  Last line skipped.\r\n", d->character);
      terminator = 1;
    } else {
      if (!(*d->str = (char *) realloc(*d->str, strlen(*d->str) +
				       strlen(str) + 3))) {
	perror("string_add");
	exit(1);
      }
      strcat(*d->str, str);
    }
  }

  if (terminator) {
    if (!d->connected && (PLR_FLAGGED(d->character, PLR_MAILING))) {
      store_mail(d->mail_to, GET_IDNUM(d->character), *d->str);
      d->mail_to = 0;
      free(*d->str);
      free(d->str);
      SEND_TO_Q("Message sent!\r\n", d);
      if (!IS_NPC(d->character))
	REMOVE_BIT(PLR_FLAGS(d->character), PLR_MAILING | PLR_WRITING);
    }
    d->str = NULL;

    if (d->mail_to >= BOARD_MAGIC) {
      Board_save_board(d->mail_to - BOARD_MAGIC);
      d->mail_to = 0;
    }
    if (d->connected == CON_EXDESC) {
      SEND_TO_Q(MENU, d);
      d->connected = CON_MENU;
    }
    if (!d->connected && d->character && !IS_NPC(d->character))
      REMOVE_BIT(PLR_FLAGS(d->character), PLR_WRITING);
  } else
    strcat(*d->str, "\r\n");
}



/* **********************************************************************
*  Modification of character skills                                     *
********************************************************************** */

ACMD(do_skillset)
{
  extern char *spells[];
  struct char_data *vict;
  char name[100], buf2[100], buf[100], help[MAX_STRING_LENGTH];
  int skill, value, i, qend;

  argument = one_argument(argument, name);

  if (!*name) {			/* no arguments. print an informative text */
    send_to_char("Syntax: skillset <name> '<skill>' <value>\r\n", ch);
    strcpy(help, "Skill being one of the following:\n\r");
    for (i = 0; *spells[i] != '\n'; i++) {
      if (*spells[i] == '!')
	continue;
      sprintf(help + strlen(help), "%18s", spells[i]);
      if (i % 4 == 3) {
	strcat(help, "\r\n");
	send_to_char(help, ch);
	*help = '\0';
      }
    }
    if (*help)
      send_to_char(help, ch);
    send_to_char("\n\r", ch);
    return;
  }
  if (!(vict = get_char_vis(ch, name))) {
    send_to_char(NOPERSON, ch);
    return;
  }
  skip_spaces(&argument);

  /* If there is no chars in argument */
  if (!*argument) {
    send_to_char("Skill name expected.\n\r", ch);
    return;
  }
  if (*argument != '\'') {
    send_to_char("Skill must be enclosed in: ''\n\r", ch);
    return;
  }
  /* Locate the last quote && lowercase the magic words (if any) */

  for (qend = 1; *(argument + qend) && (*(argument + qend) != '\''); qend++)
    *(argument + qend) = LOWER(*(argument + qend));

  if (*(argument + qend) != '\'') {
    send_to_char("Skill must be enclosed in: ''\n\r", ch);
    return;
  }
  strcpy(help, (argument + 1));
  help[qend - 1] = '\0';
  if ((skill = find_skill_num(help)) <= 0) {
    send_to_char("Unrecognized skill.\n\r", ch);
    return;
  }
  argument += qend + 1;		/* skip to next parameter */
  argument = one_argument(argument, buf);

  if (!*buf) {
    send_to_char("Learned value expected.\n\r", ch);
    return;
  }
  value = atoi(buf);
  if (value < 0) {
    send_to_char("Minimum value for learned is 0.\n\r", ch);
    return;
  }
  if (value > 100) {
    send_to_char("Max value for learned is 100.\n\r", ch);
    return;
  }
  if (IS_NPC(vict)) {
    send_to_char("You can't set NPC skills.\n\r", ch);
    return;
  }
  sprintf(buf2, "%s changed %s's %s to %d.", GET_NAME(ch), GET_NAME(vict),
	  spells[skill], value);
  mudlog(buf2, BRF, -1, TRUE);

  SET_SKILL(vict, skill, value);

  sprintf(buf2, "You change %s's %s to %d.\n\r", GET_NAME(vict),
	  spells[skill], value);
  send_to_char(buf2, ch);
}


/* db stuff *********************************************** */


/* One_Word is like one_argument, execpt that words in quotes "" are */
/* regarded as ONE word                                              */

char *one_word(char *argument, char *first_arg)
{
  int found, begin, look_at;

  found = begin = 0;

  do {
    for (; isspace(*(argument + begin)); begin++);

    if (*(argument + begin) == '\"') {	/* is it a quote */

      begin++;

      for (look_at = 0; (*(argument + begin + look_at) >= ' ') &&
	   (*(argument + begin + look_at) != '\"'); look_at++)
	*(first_arg + look_at) = LOWER(*(argument + begin + look_at));

      if (*(argument + begin + look_at) == '\"')
	begin++;

    } else {

      for (look_at = 0; *(argument + begin + look_at) > ' '; look_at++)
	*(first_arg + look_at) = LOWER(*(argument + begin + look_at));

    }

    *(first_arg + look_at) = '\0';
    begin += look_at;
  } while (fill_word(first_arg));

  return (argument + begin);
}


struct help_index_element *build_help_index(FILE * fl, int *num)
{
  int nr = -1, issorted, i;
  struct help_index_element *list = 0, mem;
  char buf[128], tmp[128], *scan;
  long pos;
  int count_hash_records(FILE * fl);

  i = count_hash_records(fl) * 5;
  rewind(fl);
  CREATE(list, struct help_index_element, i);

  for (;;) {
    pos = ftell(fl);
    fgets(buf, 128, fl);
    *(buf + strlen(buf) - 1) = '\0';
    scan = buf;
    for (;;) {
      /* extract the keywords */
      scan = one_word(scan, tmp);

      if (!*tmp)
	break;

      nr++;

      list[nr].pos = pos;
      CREATE(list[nr].keyword, char, strlen(tmp) + 1);
      strcpy(list[nr].keyword, tmp);
    }
    /* skip the text */
    do
      fgets(buf, 128, fl);
    while (*buf != '#');
    if (*(buf + 1) == '~')
      break;
  }
  /* we might as well sort the stuff */
  do {
    issorted = 1;
    for (i = 0; i < nr; i++)
      if (str_cmp(list[i].keyword, list[i + 1].keyword) > 0) {
	mem = list[i];
	list[i] = list[i + 1];
	list[i + 1] = mem;
	issorted = 0;
      }
  } while (!issorted);

  *num = nr;
  return (list);
}



void page_string(struct descriptor_data *d, char *str, int keep_internal)
{
  if (!d)
    return;

  if (keep_internal) {
    CREATE(d->showstr_head, char, strlen(str) + 1);
    strcpy(d->showstr_head, str);
    d->showstr_point = d->showstr_head;
  } else
    d->showstr_point = str;

  show_string(d, "");
}



void show_string(struct descriptor_data *d, char *input)
{
  char buffer[MAX_STRING_LENGTH], buf[MAX_INPUT_LENGTH];
  register char *scan, *chk;
  int lines = 0, toggle = 1;

  one_argument(input, buf);

  if (*buf) {
    if (d->showstr_head) {
      free(d->showstr_head);
      d->showstr_head = 0;
    }
    d->showstr_point = 0;
    return;
  }
  /* show a chunk */
  for (scan = buffer;; scan++, d->showstr_point++) {
    if ((((*scan = *d->showstr_point) == '\n') || (*scan == '\r')) &&
	((toggle = -toggle) < 0))
      lines++;
    else if (!*scan || (lines >= 22)) {
      if (lines >= 22) {
	/* We need to make sure that if we're breaking the input here, we
	 * must set showstr_point to the right place and null terminate the
	 * character after the last '\n' or '\r' so we don't lose it.
	 * -- Michael Buselli
	 */
	d->showstr_point++;
	*(++scan) = '\0';
      }
      SEND_TO_Q(buffer, d);

      /* see if this is the end (or near the end) of the string */
      for (chk = d->showstr_point; isspace(*chk); chk++);
      if (!*chk) {
	if (d->showstr_head) {
	  free(d->showstr_head);
	  d->showstr_head = 0;
	}
	d->showstr_point = 0;
      }
      return;
    }
  }
}
