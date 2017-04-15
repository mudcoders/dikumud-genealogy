/* ************************************************************************
*   File: objsave.c                                     Part of CircleMUD *
*  Usage: loading/saving player objects for rent and crash-save           *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#include "structs.h"
#include "comm.h"
#include "handler.h"
#include "db.h"
#include "interpreter.h"
#include "utils.h"
#include "spells.h"

/* these factors should be unique integers */
#define RENT_FACTOR 	1
#define CRYO_FACTOR 	4

extern struct str_app_type str_app[];
extern struct room_data *world;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct descriptor_data *descriptor_list;
extern struct player_index_element *player_table;
extern int	top_of_p_table;
extern int	min_rent_cost;

/* Extern functions */
ACMD(do_tell);
SPECIAL(receptionist);
SPECIAL(cryogenicist);

FILE *fd;

int	Crash_get_filename(char *orig_name, char *filename)
{
   char	*ptr, name[30];

   if (!*orig_name)
      return 0;

   strcpy(name, orig_name);
   for (ptr = name; *ptr; ptr++)
      *ptr = tolower(*ptr);

   switch (tolower(*name)) {
   case 'a': case 'b': case 'c': case 'd': case 'e':
      sprintf(filename, "plrobjs/A-E/%s.objs", name); break;
   case 'f': case 'g': case 'h': case 'i': case 'j':
      sprintf(filename, "plrobjs/F-J/%s.objs", name); break;
   case 'k': case 'l': case 'm': case 'n': case 'o':
      sprintf(filename, "plrobjs/K-O/%s.objs", name); break;
   case 'p': case 'q': case 'r': case 's': case 't':
      sprintf(filename, "plrobjs/P-T/%s.objs", name); break;
   case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
      sprintf(filename, "plrobjs/U-Z/%s.objs", name); break;
   default:
      sprintf(filename, "plrobjs/ZZZ/%s.objs", name); break;
   }

   return 1;
}



int	Crash_delete_file(char *name)
{
   char	filename[50];
   FILE * fl;

   if (!Crash_get_filename(name, filename))
      return 0;
   if (!(fl = fopen(filename, "rb"))) {
      if (errno != ENOENT) { /* if it fails but NOT because of no file */
         sprintf(buf1, "SYSERR: deleting crash file %s (1)", filename);
         perror(buf1);
      }
      return 0;
   }
   fclose(fl);

   if (unlink(filename) < 0) {
      if (errno != ENOENT) { /* if it fails, NOT because of no file */
         sprintf(buf1, "SYSERR: deleting crash file %s (2)", filename);
         perror(buf1);
      }
   }

   return(1);
}


int	Crash_delete_crashfile(struct char_data *ch)
{
   char	fname[MAX_INPUT_LENGTH];
   struct rent_info rent;
   FILE * fl;

   if (!Crash_get_filename(GET_NAME(ch), fname))
      return 0;
   if (!(fl = fopen(fname, "rb"))) {
      if (errno != ENOENT) { /* if it fails, NOT because of no file */
         sprintf(buf1, "SYSERR: checking for crash file %s (3)", fname);
         perror(buf1);
      }
      return 0;
   }

   if (!feof(fl))
      fread(&rent, sizeof(struct rent_info), 1, fl);
   fclose(fl);

   if (rent.rentcode == RENT_CRASH)
      Crash_delete_file(GET_NAME(ch));

   return 1;
}


int	Crash_clean_file(char *name)
{
   char	fname[MAX_STRING_LENGTH], filetype[20];
   struct rent_info rent;
   extern int rent_file_timeout, crash_file_timeout;
   FILE * fl;

   if (!Crash_get_filename(name, fname))
      return 0;
   /* open for write so that permission problems will be flagged now,
      at boot time. */
   if (!(fl = fopen(fname, "r+b"))) {
      if (errno != ENOENT) { /* if it fails, NOT because of no file */
         sprintf(buf1, "SYSERR: OPENING OBJECT FILE %s (4)", fname);
         perror(buf1);
      }
      return 0;
   }

   if (!feof(fl))
      fread(&rent, sizeof(struct rent_info), 1, fl);
   fclose(fl);

   if ( (rent.rentcode == RENT_CRASH) || 
       (rent.rentcode == RENT_FORCED) || (rent.rentcode == RENT_TIMEDOUT) )  {
      if (rent.time < time(0) - (crash_file_timeout * SECS_PER_REAL_DAY)) {
	 Crash_delete_file(name);
	 switch (rent.rentcode) {
	 case RENT_CRASH   : strcpy(filetype, "crash"); break;
	 case RENT_FORCED  : strcpy(filetype, "forced rent"); break;
	 case RENT_TIMEDOUT: strcpy(filetype, "idlesave"); break;
	 default: strcpy(filetype, "UNKNOWN!"); break;
	 }
	 sprintf(buf, "    Deleting %s's %s file.", name, filetype);
	 log(buf);
	 return 1;
      }

      /* Must retrieve rented items w/in 30 days */
   } else if (rent.rentcode == RENT_RENTED)
      if (rent.time < time(0) - (rent_file_timeout * SECS_PER_REAL_DAY)) {
	 Crash_delete_file(name);
	 sprintf(buf, "    Deleting %s's rent file.", name);
	 log(buf);
	 return 1;
      }
   return(0);
}



void	update_obj_file(void) 
{
   int	i;

   for (i = 0; i <= top_of_p_table; i++)
      Crash_clean_file((player_table + i)->name);
   return;
}


void	Crash_obj2char(struct char_data *ch, struct obj_file_elem *object) 
{
   struct obj_data *obj;
   int	j;

   if (real_object(object->item_number) > -1) {
      obj = read_object(object->item_number, VIRTUAL);
      obj->obj_flags.value[0] = object->value[0];
      obj->obj_flags.value[1] = object->value[1];
      obj->obj_flags.value[2] = object->value[2];
      obj->obj_flags.value[3] = object->value[3];
      obj->obj_flags.extra_flags = object->extra_flags;
      obj->obj_flags.weight = object->weight;
      obj->obj_flags.timer = object->timer;
      obj->obj_flags.bitvector = object->bitvector;

      for (j = 0; j < MAX_OBJ_AFFECT; j++)
	 obj->affected[j] = object->affected[j];

      obj_to_char(obj, ch);
   }
}


void	Crash_listrent(struct char_data *ch, char *name) 
{
   FILE * fl;
   char	fname[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   struct obj_file_elem object;
   struct obj_data *obj;
   struct rent_info rent;

   if (!Crash_get_filename(name, fname))
      return;
   if (!(fl = fopen(fname, "rb"))) {
      sprintf(buf, "%s has no rent file.\n\r", name);
      send_to_char(buf, ch);
      return;
   }
   sprintf(buf, "%s\n\r", fname);
   if (!feof(fl))
      fread(&rent, sizeof(struct rent_info), 1, fl);
   switch (rent.rentcode) {
   case RENT_RENTED  :  strcat(buf, "Rent\n\r"); break;
   case RENT_CRASH   :  strcat(buf, "Crash\n\r"); break;
   case RENT_CRYO    :  strcat(buf, "Cryo\n\r");  break;
   case RENT_TIMEDOUT:
   case RENT_FORCED  : strcat(buf, "TimedOut\n\r"); break;
   default	     : strcat(buf, "Undef\n\r"); break;
   }
   while (!feof(fl)) {
      fread(&object, sizeof(struct obj_file_elem), 1, fl);
      if (ferror(fl)) {
	 fclose(fl);
	 return;
      }
      if (!feof(fl))
	 if (real_object(object.item_number) > -1) {
	    obj = read_object(object.item_number, VIRTUAL);
	    sprintf(buf, "%s [%5d] (%5dau) %-20s\n\r", buf,
	        object.item_number, obj->obj_flags.cost_per_day,
	        obj->short_description);
	    extract_obj(obj);
	 }
   }
   send_to_char(buf, ch);
   fclose(fl);
}



int	Crash_write_rentcode(struct char_data *ch, FILE *fl, struct rent_info *rent)
{
   if (fwrite(rent, sizeof(struct rent_info), 1, fl) < 1) {
      perror("Writing rent code.");
      return 0;
   }
   return 1;
}



int	Crash_load(struct char_data *ch)
/* return values:
	0 - successful load, keep char in rent room.
	1 - load failure or load of crash items -- put char in temple.
	2 - rented equipment lost (no $)
*/
{
   void	Crash_crashsave(struct char_data *ch);

   FILE * fl;
   char	fname[MAX_STRING_LENGTH];
   struct obj_file_elem object;
   struct rent_info rent;
   int	cost, orig_rent_code;
   float	num_of_days;

   if (!Crash_get_filename(GET_NAME(ch), fname))
      return 1;
   if (!(fl = fopen(fname, "r+b"))) {
      if (errno != ENOENT) { /* if it fails, NOT because of no file */
         sprintf(buf1, "SYSERR: READING OBJECT FILE %s (5)", fname);
         perror(buf1);
	 send_to_char("\n\r********************* NOTICE *********************\n\r"
		      "There was a problem loading your objects from disk.\n\r"
                      "Contact a God for assistance.\n\r", ch);
      }
      sprintf(buf, "%s entering game with no equipment.", GET_NAME(ch));
      mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
      return 1;
   }

   if (!feof(fl))
      fread(&rent, sizeof(struct rent_info), 1, fl);

   if (rent.rentcode == RENT_RENTED) {
      num_of_days = (float)(time(0) - rent.time) / SECS_PER_REAL_DAY;
      cost = (int)(rent.net_cost_per_diem * num_of_days);
      if (cost > GET_GOLD(ch) + GET_BANK_GOLD(ch)) {
	 fclose(fl);
	 sprintf(buf, "%s entering game, rented equipment lost (no $).",
	     GET_NAME(ch));
         mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
	 Crash_crashsave(ch);
	 return 2;
      } else {
	 GET_BANK_GOLD(ch) -= MAX(cost - GET_GOLD(ch), 0);
	 GET_GOLD(ch) = MAX(GET_GOLD(ch) - cost, 0);
	 save_char(ch, NOWHERE);
      }
   }

   switch (orig_rent_code = rent.rentcode) {
   case RENT_RENTED:
      sprintf(buf, "%s un-renting and entering game.", GET_NAME(ch));
      mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
      break;
   case RENT_CRASH:
      sprintf(buf, "%s retrieving crash-saved items and entering game.", GET_NAME(ch));
      mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
      break;
   case RENT_CRYO:
      sprintf(buf, "%s un-cryo'ing and entering game.", GET_NAME(ch));
      mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
      break;
   case RENT_FORCED:
   case RENT_TIMEDOUT:
      sprintf(buf, "%s retrieving force-saved items and entering game.", GET_NAME(ch));
      mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
      break;
   default:
      sprintf(buf, "WARNING: %s entering game with undefined rent code.", GET_NAME(ch));
      mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
      break;
   }

   while (!feof(fl)) {
      fread(&object, sizeof(struct obj_file_elem), 1, fl);
      if (ferror(fl)) {
	 perror("Reading crash file: Crash_load.");
	 fclose(fl);
	 return 1;
      }
      if (!feof(fl))
	 Crash_obj2char(ch, &object);
   }

   /* turn this into a crash file by re-writing the control block */
   rent.rentcode = RENT_CRASH;
   rent.time = time(0);
   rewind(fl);
   Crash_write_rentcode(ch, fl, &rent);

   fclose(fl);

   if ((orig_rent_code == RENT_RENTED) || (orig_rent_code == RENT_CRYO))
      return 0;
   else
      return 1;
}




int	Crash_obj2store(struct obj_data *obj, struct char_data *ch, FILE *fl)
{
   int	j;
   struct obj_file_elem object;

   object.item_number = obj_index[obj->item_number].virtual;
   object.value[0] = obj->obj_flags.value[0];
   object.value[1] = obj->obj_flags.value[1];
   object.value[2] = obj->obj_flags.value[2];
   object.value[3] = obj->obj_flags.value[3];
   object.extra_flags = obj->obj_flags.extra_flags;
   object.weight = obj->obj_flags.weight;
   object.timer = obj->obj_flags.timer;
   object.bitvector = obj->obj_flags.bitvector;
   for (j = 0; j < MAX_OBJ_AFFECT; j++)
      object.affected[j] = obj->affected[j];

   if (fwrite(&object, sizeof(struct obj_file_elem), 1, fl) < 1) {
      perror("Writing crash data Crash_obj2store");
      return 0;
   }

   return 1;
}


int	Crash_save(struct obj_data *obj, struct char_data *ch, FILE *fp)
{
   struct obj_data *tmp;
   int	result;

   if (obj) {
      Crash_save(obj->contains, ch, fp);
      Crash_save(obj->next_content, ch, fp);
      result = Crash_obj2store(obj, ch, fp);
      if (!result) {
	 return 0;
      }

      for (tmp = obj->in_obj; tmp; tmp = tmp->in_obj)
	 GET_OBJ_WEIGHT(tmp) -= GET_OBJ_WEIGHT(obj);
   }
   return TRUE;
}


void	Crash_restore_weight(struct obj_data *obj)
{
   if (obj) {
      Crash_restore_weight(obj->contains);
      Crash_restore_weight(obj->next_content);
      if (obj->in_obj)
	 GET_OBJ_WEIGHT(obj->in_obj) += GET_OBJ_WEIGHT(obj);
   }
}



void	Crash_extract_objs(struct obj_data *obj)
{
   if (obj) {
      Crash_extract_objs(obj->contains);
      Crash_extract_objs(obj->next_content);
      extract_obj(obj);
   }
}


int	Crash_is_unrentable(struct obj_data *obj)
{
   if (!obj)
      return 0;
   if (IS_SET(obj->obj_flags.extra_flags, ITEM_NORENT) || 
       obj->obj_flags.cost_per_day < 0 || 
       obj->item_number <= -1 || 
       GET_ITEM_TYPE(obj) == ITEM_KEY )
      return 1;
   return 0;
}


void	Crash_extract_norents(struct obj_data *obj)
{
   if (obj) {
      Crash_extract_norents(obj->contains);
      Crash_extract_norents(obj->next_content);
      if (Crash_is_unrentable(obj))
	 extract_obj(obj);
   }
}


void	Crash_extract_expensive(struct obj_data *obj)
{
   struct obj_data *tobj, *max;

   max = obj;
   for (tobj = obj; tobj; tobj = tobj->next_content)
      if (tobj->obj_flags.cost_per_day > max->obj_flags.cost_per_day)
	 max = tobj;
   extract_obj(max);
}



void	Crash_calculate_rent(struct obj_data *obj, int *cost)
{
   if (obj) {
      *cost += MAX(0, obj->obj_flags.cost_per_day >> 1);
      Crash_calculate_rent(obj->contains, cost);
      Crash_calculate_rent(obj->next_content, cost);
   }
}


void	Crash_crashsave(struct char_data *ch)
{
   char	buf[MAX_INPUT_LENGTH];
   struct rent_info rent;
   int	j;
   FILE * fp;

   if (IS_NPC(ch)) 
      return;

   if (!Crash_get_filename(GET_NAME(ch), buf))
      return;
   if (!(fp = fopen(buf, "wb")))
      return;

   rent.rentcode = RENT_CRASH;
   rent.time = time(0);
   if (!Crash_write_rentcode(ch, fp, &rent)) {
      fclose(fp);
      return;
   }
   if (!Crash_save(ch->carrying, ch, fp)) {
      fclose(fp);
      return;
   }
   Crash_restore_weight(ch->carrying);

   for (j = 0; j < MAX_WEAR; j++)
      if (ch->equipment[j]) {
	 if (!Crash_save(ch->equipment[j], ch, fp)) {
	    fclose(fp);
	    return;
	 }
	 Crash_restore_weight(ch->equipment[j]);
      }

   fclose(fp);
   REMOVE_BIT(PLR_FLAGS(ch), PLR_CRASH);
}


void	Crash_idlesave(struct char_data *ch)
{
   char	buf[MAX_INPUT_LENGTH];
   struct rent_info rent;
   int	j;
   int	cost;
   FILE * fp;

   if (IS_NPC(ch)) 
      return;

   if (!Crash_get_filename(GET_NAME(ch), buf))
      return;
   if (!(fp = fopen(buf, "wb")))
      return;

   for (j = 0; j < MAX_WEAR; j++)
      if (ch->equipment[j])
	 obj_to_char(unequip_char(ch, j), ch);

   Crash_extract_norents(ch->carrying);

   cost = 0;
   Crash_calculate_rent(ch->carrying, &cost);
   cost += cost >> 2;
   while ((cost > GET_GOLD(ch) + GET_BANK_GOLD(ch)) && ch->carrying) {
      Crash_extract_expensive(ch->carrying);
      cost = 0;
      Crash_calculate_rent(ch->carrying, &cost);
      cost += cost >> 2;
   }

   if (!ch->carrying) {
      fclose(fp);
      Crash_delete_file(GET_NAME(ch));
      return;
   }

   rent.net_cost_per_diem = cost;

   rent.rentcode = RENT_TIMEDOUT;
   rent.time = time(0);
   rent.gold = GET_GOLD(ch);
   rent.account = GET_BANK_GOLD(ch);
   if (!Crash_write_rentcode(ch, fp, &rent)) {
      fclose(fp);
      return;
   }

   if (!Crash_save(ch->carrying, ch, fp)) {
      fclose(fp);
      return;
   }
   fclose(fp);

   Crash_extract_objs(ch->carrying);
}


void	Crash_rentsave(struct char_data *ch, int cost)
{
   char	buf[MAX_INPUT_LENGTH];
   struct rent_info rent;
   int	j;
   FILE * fp;

   if (IS_NPC(ch)) 
      return;

   if (!Crash_get_filename(GET_NAME(ch), buf))
      return;
   if (!(fp = fopen(buf, "wb")))
      return;

   for (j = 0; j < MAX_WEAR; j++)
      if (ch->equipment[j])
	 obj_to_char(unequip_char(ch, j), ch);

   Crash_extract_norents(ch->carrying);

   rent.net_cost_per_diem = cost;
   rent.rentcode = RENT_RENTED;
   rent.time = time(0);
   rent.gold = GET_GOLD(ch);
   rent.account = GET_BANK_GOLD(ch);
   if (!Crash_write_rentcode(ch, fp, &rent)) {
      fclose(fp);
      return;
   }

   if (!Crash_save(ch->carrying, ch, fp)) {
      fclose(fp);
      return;
   }
   fclose(fp);

   Crash_extract_objs(ch->carrying);
}


void	Crash_cryosave(struct char_data *ch, int cost)
{
   char	buf[MAX_INPUT_LENGTH];
   struct rent_info rent;
   int	j;
   FILE * fp;

   if (IS_NPC(ch)) 
      return;

   if (!Crash_get_filename(GET_NAME(ch), buf))
      return;
   if (!(fp = fopen(buf, "wb")))
      return;

   for (j = 0; j < MAX_WEAR; j++)
      if (ch->equipment[j])
	 obj_to_char(unequip_char(ch, j), ch);

   Crash_extract_norents(ch->carrying);

   GET_GOLD(ch) = MAX(0, GET_GOLD(ch) - cost);

   rent.rentcode = RENT_CRYO;
   rent.time = time(0);
   rent.gold = GET_GOLD(ch);
   rent.account = GET_BANK_GOLD(ch);
   rent.net_cost_per_diem = 0;
   if (!Crash_write_rentcode(ch, fp, &rent)) {
      fclose(fp);
      return;
   }

   if (!Crash_save(ch->carrying, ch, fp)) {
      fclose(fp);
      return;
   }
   fclose(fp);

   Crash_extract_objs(ch->carrying);
   SET_BIT(PLR_FLAGS(ch), PLR_CRYO);
}


/* ************************************************************************
* Routines used for the "Offer"                                           *
************************************************************************* */

int	Crash_report_unrentables(struct char_data *ch, struct char_data *recep,
struct obj_data *obj)
{
   char	buf[128];
   int	has_norents = 0;

   if (obj) {
      if (Crash_is_unrentable(obj)) {
	 has_norents = 1;
	 sprintf(buf, "$n tells you, 'You cannot store %s.'", OBJS(obj, ch));
	 act(buf, FALSE, recep, 0, ch, TO_VICT);
      }
      has_norents += Crash_report_unrentables(ch, recep, obj->contains);
      has_norents += Crash_report_unrentables(ch, recep, obj->next_content);
   }
   return(has_norents);
}



void	Crash_report_rent(struct char_data *ch, struct char_data *recep, 
struct obj_data *obj, long *cost, long *nitems, int display, int factor)
{
   static char	buf[256];

   if (obj) {
      if (!Crash_is_unrentable(obj)) {
	 (*nitems)++;
	 *cost += MAX(0, (obj->obj_flags.cost_per_day >> 1) * factor);
	 if (display) {
	    sprintf(buf, "$n tells you, '%5d coins for %s..'", 
	        ((obj->obj_flags.cost_per_day >> 1) * factor), OBJS(obj, ch));
	    act(buf, FALSE, recep, 0, ch, TO_VICT);
	 }
      }
      Crash_report_rent(ch, recep, obj->contains, cost, nitems, display, factor);
      Crash_report_rent(ch, recep, obj->next_content, cost, nitems, display, factor);
   }
}



int	Crash_offer_rent(struct char_data *ch, struct char_data *receptionist,
int display, int factor)
{
   extern int	max_obj_save;	/* change in config.c */
   char	buf[MAX_INPUT_LENGTH];
   int	i;
   long	totalcost = 0, numitems = 0, norent = 0, rent_deadline;

   norent = Crash_report_unrentables(ch, receptionist, ch->carrying);
   for (i = 0; i < MAX_WEAR; i++)
      norent += Crash_report_unrentables(ch, receptionist, ch->equipment[i]);

   if (norent)
      return 0;

   totalcost = min_rent_cost * factor;

   Crash_report_rent(ch, receptionist, ch->carrying, &totalcost, &numitems, display, factor);

   for (i = 0; i < MAX_WEAR; i++)
      Crash_report_rent(ch, receptionist, ch->equipment[i], &totalcost, &numitems, display, factor);

   if (!numitems) {
      act("$n tells you, 'But you are not carrying anything!  Just quit!'",
          FALSE, receptionist, 0, ch, TO_VICT);
      return(0);
   }

   if (numitems > max_obj_save) {
      sprintf(buf, "$n tells you, 'Sorry, but I cannot store more than %d items.'",
          max_obj_save);
      act(buf, FALSE, receptionist, 0, ch, TO_VICT);
      return(0);
   }

   if (display) {
      sprintf(buf, "$n tells you, 'Plus, my %d coin fee..'",
          min_rent_cost * factor);
      act(buf, FALSE, receptionist, 0, ch, TO_VICT);
      sprintf(buf, "$n tells you, 'For a total of %d coins%s.'",
          totalcost, (factor == RENT_FACTOR ? " per day" : ""));
      act(buf, FALSE, receptionist, 0, ch, TO_VICT);
      if (totalcost > GET_GOLD(ch)) {
	 act("$n tells you, '...which I see you can't afford.'",
	     FALSE, receptionist, 0, ch, TO_VICT);
	 return(0);
      } else if (factor == RENT_FACTOR) {
	 if (totalcost) {
	    rent_deadline = ((GET_GOLD(ch) + GET_BANK_GOLD(ch)) / (totalcost));
	    sprintf(buf, 
	        "$n tells you, 'You can rent for %d day%s with the gold"
	        " you have\n\r     on hand and in the bank.'\n\r",
	        rent_deadline, (rent_deadline > 1) ? "s" : "");
	 }
	 act(buf, FALSE, receptionist, 0, ch, TO_VICT);
      }
   }

   return(totalcost);
}



int	gen_receptionist(struct char_data *ch, int cmd, char *arg, int mode)
{
   int	cost = 0;
   struct char_data *recep = 0;
   struct char_data *tch;
   sh_int save_room;
   sh_int action_tabel[9] = { 23, 24, 36, 105, 106, 109, 111, 142, 147 };
   long	rent_deadline;

   ACMD(do_action);
   int	number(int from, int to);

   if ((!ch->desc) || IS_NPC(ch))
      return(FALSE);

   for (tch = world[ch->in_room].people; (tch) && (!recep); tch = tch->next_in_room)
      if (IS_MOB(tch) && ((mob_index[tch->nr].func == receptionist || 
          mob_index[tch->nr].func == cryogenicist)))
	 recep = tch;
   if (!recep) {
      log("SYSERR: Fubar'd receptionist.");
      exit(1);
   }

   if ((cmd != 92) && (cmd != 93)) {
      if (!number(0, 30))
	 do_action(recep, "", action_tabel[number(0,8)], 0);
      return(FALSE);
   }
   if (!AWAKE(recep)) {
      send_to_char("She is unable to talk to you...\n\r", ch);
      return(TRUE);
   }
   if (!CAN_SEE(recep, ch)) {
      act("$n says, 'I don't deal with people I can't see!'", FALSE, recep, 0, 0, TO_ROOM);
      return(TRUE);
   }

   if (cmd == 92) { /* Rent  */
      if (!(cost = Crash_offer_rent(ch, recep, FALSE, mode)))
	 return 1;
      if (mode == RENT_FACTOR)
	 sprintf(buf, "$n tells you, 'Rent will cost you %d gold coins per day.'", cost);
      else if (mode == CRYO_FACTOR)
	 sprintf(buf, "$n tells you, 'It will cost you %d gold coins to be frozen.'", cost);
      act(buf, FALSE, recep, 0, ch, TO_VICT);
      if (cost > GET_GOLD(ch)) {
	 act("$n tells you, '...which I see you can't afford.'",
	     FALSE, recep, 0, ch, TO_VICT);
	 return(1);
      }

      if (cost && (mode == RENT_FACTOR)) {
	 rent_deadline = ((GET_GOLD(ch) + GET_BANK_GOLD(ch)) / (cost));
	 sprintf(buf, 
	     "$n tells you, 'You can rent for %d day%s with the gold you have\n\r"
	     "on hand and in the bank.'\n\r",
	     rent_deadline, (rent_deadline > 1) ? "s" : "");
	 act(buf, FALSE, recep, 0, ch, TO_VICT);
      }

      if (mode == RENT_FACTOR) {
	 act("$n stores your belongings and helps you into your private chamber.",
	     FALSE, recep, 0, ch, TO_VICT);
	 Crash_rentsave(ch, cost);
	 sprintf(buf, "%s has rented (%d/day, %d tot.)", GET_NAME(ch),
	     cost, GET_GOLD(ch) + GET_BANK_GOLD(ch));
      } else { /* cryo */
	 act("$n stores your belongings and helps you into your private chamber.\n\r"
	     "A white mist appears in the room, chilling you to the bone...\n\r"
	     "You begin to lose consciousness...",
	     FALSE, recep, 0, ch, TO_VICT);
	 Crash_cryosave(ch, cost);
	 sprintf(buf, "%s has cryo-rented.", GET_NAME(ch));
	 SET_BIT(PLR_FLAGS(ch), PLR_CRYO);
      }

      mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(ch)), TRUE);
      act("$n helps $N into $S private chamber.", FALSE, recep, 0, ch, TO_NOTVICT);
      save_room = ch->in_room;
      extract_char(ch);
      ch->in_room = world[save_room].number;
      save_char(ch, ch->in_room);
   } else {  /* Offer */
      Crash_offer_rent(ch, recep, TRUE, mode);
      act("$N gives $n an offer.", FALSE, ch, 0, recep, TO_ROOM);
   }
   return(TRUE);
}


SPECIAL(receptionist)
{
   return(gen_receptionist(ch, cmd, arg, RENT_FACTOR));
}


SPECIAL(cryogenicist)
{
   return(gen_receptionist(ch, cmd, arg, CRYO_FACTOR));
}


void	Crash_save_all(void)
{
   struct descriptor_data *d;
   for (d = descriptor_list; d; d = d->next) {
      if ((d->connected == CON_PLYNG) && !IS_NPC(d->character)) {
	 if (PLR_FLAGGED(d->character, PLR_CRASH)) {
	    Crash_crashsave(d->character);
	    save_char(d->character, NOWHERE);
	    REMOVE_BIT(PLR_FLAGS(d->character), PLR_CRASH);
	 }
      }
   }
}


