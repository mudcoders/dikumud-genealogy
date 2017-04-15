/*
  Security from sites and bad names as well as
  crashproof saving of players items. Contributions
  by Taquin Ho ( prometheus ) and Jeff Stile ( Abaddon )
*/

#include <strings.h>
#include <stdio.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "db.h"

extern struct index_data *obj_index;

void log(char *str);

/*****************************************************************
*  CRASHSAFE                                                     *
*  These functions handle saving inventory in case of a crash    *
*  You must create a directory in the lib directory called crash *
*****************************************************************/

/****************************************************************
*    call here from handler.c: extract_char			*
****************************************************************/
void update_crash(struct char_data *ch)
{
	char fname[MAX_STRING_LENGTH];
	FILE *fl;

	sprintf(fname, "crash/%s.crash", ch->player.name);
	if (!(fl = fopen(fname, "rb")))
		return;
	fclose(fl);
	sprintf(fname, "rm -f crash/%s.crash", ch->player.name);
	system(fname);
}

void obj2_store_to_char(struct char_data *ch, struct obj_file_elem *object)
{
	struct obj_data *obj;
	int j;

	void obj_to_char(struct obj_data *object, struct char_data *ch);

	if (real_object(object->item_number) > -1)
	{
		obj = read_object(object->item_number, VIRTUAL);
		obj->obj_flags.value[0] = object->value[0];
		obj->obj_flags.value[1] = object->value[1];
		obj->obj_flags.value[2] = object->value[2];
		obj->obj_flags.value[3] = object->value[3];

		obj->obj_flags.extra_flags = object->extra_flags;
		obj->obj_flags.weight      = object->weight;
		obj->obj_flags.timer       = object->timer;
		obj->obj_flags.bitvector   = object->bitvector;

		for(j=0; j<MAX_OBJ_AFFECT; j++)
			obj->affected[j] = object->affected[j];

		obj_to_char(obj, ch);
	}
}


/*******************************************************************
*	call here from interpreter when loading character	   *
*******************************************************************/
void load_char_objs2(struct char_data *ch)
{
	FILE *fl;
	char fname[MAX_STRING_LENGTH];
	struct obj_file_elem object;


	sprintf(fname, "oldcrash/%s.crash", ch->player.name);
	if (!(fl = fopen(fname, "rb"))) {
		return;
	}
	log("Loading char equipment from crash");
	while (!feof(fl)) {
		fread(&object, sizeof(object), 1, fl);
			if (ferror(fl)) {
				perror("reading player file -load_char_objs2");
				fclose(fl);
				return;
			}
		if (!feof(fl))
			obj2_store_to_char(ch, &object);
	}

	fclose(fl);

	sprintf(fname, "rm -f oldcrash/%s.crash", ch->player.name);
	system(fname);
}

/*
 * Assumes file is open with write privilege
 */
bool put_obj2_in_store(struct obj_data *obj, struct char_data *ch, FILE *fl)
{
	int j;
	char fname[MAX_STRING_LENGTH];
	struct obj_file_elem object;

	object.item_number = obj_index[obj->item_number].virtual;
	object.value[0] = obj->obj_flags.value[0];
	object.value[1] = obj->obj_flags.value[1];
	object.value[2] = obj->obj_flags.value[2];
	object.value[3] = obj->obj_flags.value[3];
	object.extra_flags = obj->obj_flags.extra_flags;
	object.weight  = obj->obj_flags.weight;
	object.timer  = obj->obj_flags.timer;
	object.bitvector  = obj->obj_flags.bitvector;
	for(j=0; j<MAX_OBJ_AFFECT; j++)
		object.affected[j] = obj->affected[j];

	if (fwrite(&object, sizeof(object), 1, fl) < 1) {
		perror("writing crash data -put_obj2_in_store");
		return FALSE;
	}
	return TRUE;
}

bool obj2_to_store(struct obj_data *obj, struct char_data *ch, FILE *fp)
{
	struct obj_data *tmp;
	bool result;

	if (obj) {
		obj2_to_store(obj->contains, ch, fp);
		obj2_to_store(obj->next_content, ch, fp);

		result = put_obj2_in_store(obj, ch, fp);
		if (!result) {
			return FALSE;
		} /* if */

		for (tmp=obj->in_obj;tmp;tmp=tmp->in_obj)
			GET_OBJ_WEIGHT(tmp) -= GET_OBJ_WEIGHT(obj);
	}
	return TRUE;
}

/**************************************************************
*  required to keep total weight of containers from changing  *
**************************************************************/
void restore_weight(struct obj_data *obj)
{
	struct obj_data *tmp;

	if (obj) {
		restore_weight(obj->contains);
		restore_weight(obj->next_content);
		for (tmp=obj->in_obj;tmp;tmp=tmp->in_obj)
			GET_OBJ_WEIGHT(tmp) += GET_OBJ_WEIGHT(obj);
	}
}


/************************************************************************
*	call here from : do_save, and after loading character 		*
*	to save the characters inventory				*
*									*
*       can also call anytime inventory changes ie:			*
*       do_get, do_drop, buy, sell, steal etc.				*
*									*
************************************************************************/

void save_obj2(struct char_data *ch)
{
	int j;
	char buf[MAX_STRING_LENGTH];
	FILE *fp;

	if (IS_NPC(ch))
		return;

	sprintf(buf, "crash/%s.crash", ch->player.name);
	if (!(fp = fopen(buf, "wb")))
		return;

	if (!obj2_to_store(ch->carrying, ch, fp)) {
		fclose(fp);
		return;
	} /* if */

	restore_weight(ch->carrying);
	for(j=0; j<MAX_WEAR; j++)
		if (ch->equipment[j]) {
			if (!obj2_to_store(ch->equipment[j], ch, fp)) {
				fclose(fp);
				return;
			}
			restore_weight(ch->equipment[j]);
		}

	fclose(fp);
}



/***************************************************************************
*	In comm.c within the game_loop add:
*
*	if (pulse2 == 3) {
*     /comment 3 = 1/2 hr  each +1 increases delay 10min comment/
*		system("rm crash/*.crash");
*     /comment or as used in alex : system("mv crash/* crash/oldcrash"); com/
*		for (point = descriptor_list;point;point = next_point)
*		{
*                 next_point = point->next;
*                 if (!IS_NPC(ch))
*		  save_obj2(ch);
*               }
*		pulse2 = -1;
*	}
*
*	if (pulse >= 2400) {
*		if (pulse2 >= 0)
*			pulse2++;
*		pulse = 0
*		.
*		.
*		.
*
*      This will remove all save files so that a person must log back on
*      within alotted time after a crash to retrieve items.
*      (Ammendment made by Taquin : addition of point, next_point
*                                   or else it does not work....)
****************************************************************************/
