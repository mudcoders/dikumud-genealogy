/* ************************************************************************
*   File: modify.c                                      Part of CircleMUD *
*  Usage: Run-time modification of game variables                         *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
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
#include "mail.h"

#define REBOOT_AT    10  /* 0-23, time of optional reboot if -e lib/reboot */


#define TP_MOB    0
#define TP_OBJ	  1
#define TP_ERROR  2


void	show_string(struct descriptor_data *d, char *input);



char	*string_fields[] = 
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
int	length[] = 
{
   15,
   60,
   256,
   240,
   60
};



char	*skill_fields[] = 
{
   "learned",
   "affected",
   "duration",
   "recognize",
   "\n"
};



int	max_value[] = 
{
   255,
   255,
   10000,
   1
};


/* ************************************************************************
*  modification of malloc'ed strings                                      *
************************************************************************ */

/* Add user input to the 'current' string (as defined by d->str) */
void	string_add(struct descriptor_data *d, char *str)
{
   char	*scan;
   int	terminator = 0;
   extern char	*MENU;

   /* determine if this is the terminal string, and truncate if so */
   for (scan = str; *scan; scan++)
      if ((terminator = (*scan == '@'))) {
	 *scan = '\0';
	 break;
      }

   if (!(*d->str)) {
      if (strlen(str) > d->max_str) {
	 send_to_char("String too long - Truncated.\n\r",
	     d->character);
	 *(str + d->max_str) = '\0';
	 terminator = 1;
      }
      CREATE(*d->str, char, strlen(str) + 3);
      strcpy(*d->str, str);
   } else {
      if (strlen(str) + strlen(*d->str) > d->max_str) {
	 send_to_char("String too long. Last line skipped.\n\r",
	     d->character);
	 terminator = 1;
      } else	 {
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
	 store_mail(d->name, d->character->player.name, *d->str);
	 free(*d->str);
	 free(d->str);
	 free(d->name);
	 d->name = 0;
	 SEND_TO_Q("Message sent!\n\r", d);
	 if (!IS_NPC(d->character))
	    REMOVE_BIT(PLR_FLAGS(d->character), PLR_MAILING | PLR_WRITING);
      }
      d->str = 0;
      if (d->connected == CON_EXDSCR) {
	 SEND_TO_Q(MENU, d);
	 d->connected = CON_SLCT;
      }
      if (!d->connected && d->character && !IS_NPC(d->character))
	 REMOVE_BIT(PLR_FLAGS(d->character), PLR_WRITING);
   } else
      strcat(*d->str, "\n\r");
}


/* interpret an argument for do_string */
void	quad_arg(char *arg, int *type, char *name, int *field, char *string)
{
   char	buf[MAX_STRING_LENGTH];

   /* determine type */
   arg = one_argument(arg, buf);
   if (is_abbrev(buf, "char"))
      *type = TP_MOB;
   else if (is_abbrev(buf, "obj"))
      *type = TP_OBJ;
   else {
      *type = TP_ERROR;
      return;
   }

   /* find name */
   arg = one_argument(arg, name);

   /* field name and number */
   arg = one_argument(arg, buf);
   if (!(*field = old_search_block(buf, 0, strlen(buf), string_fields, 0)))
      return;

   /* string */
   for (; isspace(*arg); arg++)
      ;
   for (; (*string = *arg); arg++, string++)
      ;

   return;
}






/* modification of malloc'ed strings in chars/objects */
ACMD(do_string)
{
   char	name[MAX_STRING_LENGTH], string[MAX_STRING_LENGTH];
   int	field, type;
   struct char_data *mob;
   struct obj_data *obj;
   struct extra_descr_data *ed, *tmp;

   if (IS_NPC(ch))
      return;

   quad_arg(arg, &type, name, &field, string);

   if (type == TP_ERROR) {
      send_to_char("Usage: string ('obj'|'char') <name> <field> [<string>]", ch);
      return;
   }

   if (!field) {
      send_to_char("No field by that name. Try 'help string'.\n\r", ch);
      return;
   }

   if (type == TP_MOB) {
      /* locate the beast */
      if (!(mob = get_char_vis(ch, name))) {
	 send_to_char("I don't know anyone by that name...\n\r", ch);
	 return;
      }

      if (IS_NPC(mob)) {
	 send_to_char("Sorry, string is disabled for mobs.\n\r", ch);
	 return;
      }

      switch (field) {
      case 1:
	 if (!IS_NPC(mob) && GET_LEVEL(ch) < LEVEL_IMPL) {
	    send_to_char("You can't change that field for players.", ch);
	    return;
	 }
	 ch->desc->str = &(mob->player.name);
	 if (!IS_NPC(mob))
	    send_to_char("WARNING: You have changed the name of a player.\n\r", ch);
	 break;
      case 2:
	 if (!IS_NPC(mob)) {
	    send_to_char("That field is for monsters only.\n\r", ch);
	    return;
	 }
	 ch->desc->str = &mob->player.short_descr;
	 break;
      case 3:
	 if (!IS_NPC(mob)) {
	    send_to_char("That field is for monsters only.\n\r", ch);
	    return;
	 }
	 ch->desc->str = &mob->player.long_descr;
	 break;
      case 4:
	 ch->desc->str = &mob->player.description;
	 break;
      case 5:
	 if (IS_NPC(mob)) {
	    send_to_char("Monsters have no titles.\n\r", ch);
	    return;
	 }
	 ch->desc->str = &mob->player.title;
	 break;
      default:
	 send_to_char("That field is undefined for monsters.\n\r", ch);
	 return;
	 break;
      }
   } else /* type == TP_OBJ */	 {
      send_to_char("Sorry, string has been disabled for objects.\n\r", ch);
      return;

      /* locate the object */
      if (!(obj = get_obj_vis(ch, name))) {
	 send_to_char("Can't find such a thing here..\n\r", ch);
	 return;
      }

      switch (field) {
      case 1:
	 ch->desc->str = &obj->name;
	 break;
      case 2:
	 ch->desc->str = &obj->short_description;
	 break;
      case 3:
	 ch->desc->str = &obj->description;
	 break;
      case 4:
	 if (!*string) {
	    send_to_char("You have to supply a keyword.\n\r", ch);
	    return;
	 }
	 /* try to locate extra description */
	 for (ed = obj->ex_description; ; ed = ed->next)
	    if (!ed) /* the field was not found. create a new one. */ {
	       CREATE(ed , struct extra_descr_data, 1);
	       ed->next = obj->ex_description;
	       obj->ex_description = ed;
	       CREATE(ed->keyword, char, strlen(string) + 1);
	       strcpy(ed->keyword, string);
	       ed->description = 0;
	       ch->desc->str = &ed->description;
	       send_to_char("New field.\n\r", ch);
	       break;
	    }
	    else if (!str_cmp(ed->keyword, string)) /* the field exists */ {
	       free(ed->description);
	       ed->description = 0;
	       ch->desc->str = &ed->description;
	       send_to_char("Modifying description.\n\r", ch);
	       break;
	    }
	 ch->desc->max_str = MAX_STRING_LENGTH;
	 return; /* the stndrd (see below) procedure does not apply here */
	 break;
      case 6:
	 if (!*string) {
	    send_to_char("You must supply a field name.\n\r", ch);
	    return;
	 }
	 /* try to locate field */
	 for (ed = obj->ex_description; ; ed = ed->next)
	    if (!ed) {
	       send_to_char("No field with that keyword.\n\r", ch);
	       return;
	    }
	    else if (!str_cmp(ed->keyword, string)) {
	       free(ed->keyword);
	       if (ed->description)
		  free(ed->description);

	       /* delete the entry in the desr list */
	       if (ed == obj->ex_description)
		  obj->ex_description = ed->next;
	       else {
		  for (tmp = obj->ex_description; tmp->next != ed; 
		      tmp = tmp->next)
		     ;
		  tmp->next = ed->next;
	       }
	       free(ed);

	       send_to_char("Field deleted.\n\r", ch);
	       return;
	    }
	 break;
      default:
	 send_to_char(
	     "That field is undefined for objects.\n\r", ch);
	 return;
	 break;
      }
   }

   if (*ch->desc->str) {
      free(*ch->desc->str);
   }

   if (*string)   /* there was a string in the argument array */ {
      if (strlen(string) > length[field - 1]) {
	 send_to_char("String too long - truncated.\n\r", ch);
	 *(string + length[field - 1]) = '\0';
      }
      CREATE(*ch->desc->str, char, strlen(string) + 1);
      strcpy(*ch->desc->str, string);
      ch->desc->str = 0;
      send_to_char("Ok.\n\r", ch);
   } else /* there was no string. enter string mode */	 {
      send_to_char("Enter string. terminate with '@'.\n\r", ch);
      *ch->desc->str = 0;
      ch->desc->max_str = length[field - 1];
   }
}





/* **********************************************************************
*  Modification of character skills                                     *
********************************************************************** */


ACMD(do_skillset)
{
   struct char_data *vict;
   char	name[100], num[100], help[MAX_STRING_LENGTH];
   int	skill, field, value, i;
   static char	*skills[] = 
    {
      "search", "frighten", "telepath", "detect-evil",
      "sense-life", "cure", "bless", "remove",
      "poison", "blind", "neutralize", "purify",
      "hide", "cover", "backstab", "detect-invisible",
      "detect-magic", "enchant", "teleport", "create",
      "sanctuary", "resist", "drain", "turn",
      "protect", "light", "charm", "floating",
      "lightning-bolt", "sleep", "wake", "paralysis",
      "recharge", "shield", "fireball", "knock",
      "ventricolism", "double", "invisible", "death-ray",
      "bash", "dodge", "kick", "uppercut",
      "defend", "dirk", "listen", "missile", "detect", "\n"
   };

   send_to_char("This routine is disabled untill it fitts\n\r", ch);
   send_to_char("The new structures (sorry Quinn) ....Bombman\n\r", ch);
   return;

   one_argument(arg, name);
   if (!*name) /* no arguments. print an informative text */ {
      send_to_char("Syntax:\n\rsetskill <name> <skill> <field> <value>\n\r",
          ch);
      strcpy(help, "Skill being one of the following:\n\r\n\r");
      for (i = 1; *skills[i] != '\n'; i++) {
	 sprintf(help + strlen(help), "%18s", skills[i]);
	 if (!(i % 4)) {
	    strcat(help, "\n\r");
	    send_to_char(help, ch);
	    *help = '\0';
	 }
      }
      if (*help)
	 send_to_char(help, ch);
      return;
   }
   if (!(vict = get_char_vis(ch, name))) {
      send_to_char("No living thing by that name.\n\r", ch);
      return;
   }
   one_argument(arg, buf);
   if (!*buf) {
      send_to_char("Skill name expected.\n\r", ch);
      return;
   }
   if ((skill = old_search_block(buf, 0, strlen(buf), skills, 1)) < 0) {
      send_to_char("No such skill is known. Try 'setskill' for list.\n\r", ch);
      return;
   }
   argument_interpreter(arg, buf, num);
   if (!*num || !*buf) {
      send_to_char("Field name or value undefined.\n\r", ch);
      return;
   }
   if ((field = old_search_block(buf, 0, strlen(buf), skill_fields, 0)) < 0) {
      send_to_char("Unrecognized field.\n\r", ch);
      return;
   }
   value = atoi(num);
   if (field == 3) {
      if (value < -1) {
	 send_to_char("Minimum value for that is -1.\n\r", ch);
	 return;
      }
   } else if (value < 0) {
      send_to_char("Minimum value for that is 0.\n\r", ch);
      return;
   }
   if (value > max_value[field - 1]) {
      sprintf(buf, "Max value for that is %d.\n\r", max_value[field - 1]);
      send_to_char(buf, ch);
      return;
   }

   SET_SKILL(vict, skill, value);

   send_to_char("Ok.\n\r", ch);
}




/* db stuff *********************************************** */


/* One_Word is like one_argument, execpt that words in quotes "" are */
/* regarded as ONE word                                              */

char	*one_word(char *argument, char *first_arg )
{
   int	found, begin, look_at;

   found = begin = 0;

   do {
      for ( ; isspace(*(argument + begin)); begin++)
	 ;

      if (*(argument + begin) == '\"') {  /* is it a quote */

	 begin++;

	 for (look_at = 0; (*(argument + begin + look_at) >= ' ') && 
	     (*(argument + begin + look_at) != '\"') ; look_at++)
	    *(first_arg + look_at) = LOWER(*(argument + begin + look_at));

	 if (*(argument + begin + look_at) == '\"')
	    begin++;

      } else {

	 for (look_at = 0; *(argument + begin + look_at) > ' ' ; look_at++)
	    *(first_arg + look_at) = LOWER(*(argument + begin + look_at));

      }

      *(first_arg + look_at) = '\0';
      begin += look_at;
   } while (fill_word(first_arg));

   return(argument + begin);
}


struct help_index_element *build_help_index(FILE *fl, int *num)
{
   int	nr = -1, issorted, i;
   struct help_index_element *list = 0, mem;
   char	buf[81], tmp[81], *scan;
   long	pos;

   for (; ; ) {
      pos = ftell(fl);
      fgets(buf, 81, fl);
      *(buf + strlen(buf) - 1) = '\0';
      scan = buf;
      for (; ; ) {
	 /* extract the keywords */
	 scan = one_word(scan, tmp);

	 if (!*tmp)
	    break;

	 if (!list) {
	    CREATE(list, struct help_index_element, 1);
	    nr = 0;
	 } else
	    RECREATE(list, struct help_index_element, ++nr + 1);

	 list[nr].pos = pos;
	 CREATE(list[nr].keyword, char, strlen(tmp) + 1);
	 strcpy(list[nr].keyword, tmp);
      }
      /* skip the text */
      do
	 fgets(buf, 81, fl);
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
   return(list);
}



void	page_string(struct descriptor_data *d, char *str, int keep_internal)
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



void	show_string(struct descriptor_data *d, char *input)
{
   char	buffer[MAX_STRING_LENGTH], buf[MAX_INPUT_LENGTH];
   register char	*scan, *chk;
   int	lines = 0, toggle = 1;

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
   for (scan = buffer; ; scan++, d->showstr_point++)
      if ((((*scan = *d->showstr_point) == '\n') || (*scan == '\r')) && 
          ((toggle = -toggle) < 0))
	 lines++;
      else if (!*scan || (lines >= 22)) {
	 *scan = '\0';
	 SEND_TO_Q(buffer, d);

	 /* see if this is the end (or near the end) of the string */
	 for (chk = d->showstr_point; isspace(*chk); chk++)
	    ;
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



void	check_reboot(void)
{
   long	tc;
   struct tm *t_info;
   char	dummy;
   FILE * boot;

   extern int	circle_shutdown, circle_reboot;

   tc = time(0);
   t_info = localtime(&tc);

   if ((t_info->tm_hour + 1) == REBOOT_AT && t_info->tm_min > 30)
      if ((boot = fopen("./reboot", "r"))) {
	 if (t_info->tm_min > 50) {
	    log("Reboot exists.");
	    fread(&dummy, sizeof(dummy), 1, boot);
	    if (!feof(boot))   /* the file is nonepty */ {
	       log("Reboot is nonempty.");
	       if (system("./reboot")) {
		  log("Reboot script terminated abnormally");
		  send_to_all("The reboot was cancelled.\n\r");
		  system("mv ./reboot reboot.FAILED");
		  fclose(boot);
		  return;
	       } else
		  system("mv ./reboot reboot.SUCCEEDED");
	    }

	    send_to_all("Automatic reboot.  Come back in a little while.\n\r");
	    circle_shutdown = circle_reboot = 1;
	 } else if (t_info->tm_min > 40)
	    send_to_all("ATTENTION: CircleMUD will reboot in 10 minutes.\n\r");
	 else if (t_info->tm_min > 30)
	    send_to_all(
	        "Warning: The game will close and reboot in 20 minutes.\n\r");

	 fclose(boot);
      }
}

