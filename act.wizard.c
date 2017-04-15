
/* ************************************************************************
*  file: actwiz.c , Implementation of commands.           Part of DIKUMUD *
*  Usage : Wizard Commands.                                               *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
************************************************************************* */

#include <stdio.h>
#include <strings.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"

/*   external vars  */

extern int log_all;
extern struct room_data *world;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct title_type titles[4][25];
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct int_app_type int_app[26];
extern struct wis_app_type wis_app[26];
extern int restrict;
extern struct ban_t *ban_list;

/* external functs */
char *strdup(char *s);
void set_title(struct char_data *ch);
int str_cmp(char *arg1, char *arg2);
struct time_info_data age(struct char_data *ch);
void sprinttype(int type, char *names[], char *result);
void sprintbit(long vektor, char *names[], char *result);
int mana_limit(struct char_data *ch);
int hit_limit(struct char_data *ch);
int move_limit(struct char_data *ch);
int mana_gain(struct char_data *ch);
int hit_gain(struct char_data *ch);
int move_gain(struct char_data *ch);
void log(char *str);


void do_systats(struct char_data *ch, char *argument, int cmd)
{
	char buf[80];
	extern int avail_descs, maxdesc;

	if(IS_NPC(ch)){
		send_to_char("Monsters don't like computers\n",ch);
		return;
	}
	sprintf(buf,"There are %d avail descriptors, %d max descriptors\n\r",avail_descs,maxdesc);
	send_to_char(buf,ch);
}

void
do_release(struct char_data *ch,  char *argument, int cmd)
{
	char arg[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	struct descriptor_data *d;
	int sdesc;

	if (!IS_TRUSTED(ch))
		return;

	one_argument(argument, arg);
	sdesc=atoi(arg);
	if(arg==0){
		send_to_char("Illegal descriptor number.\n\r",ch);
		send_to_char("Usage: release <#>\n\r",ch);
		return;
	}
	for(d=descriptor_list;d;d=d->next){
		if (d->descriptor==sdesc){
			close_socket(d);
			sprintf(buf,"Closing socket to descriptor #%d\n\r",sdesc);
			send_to_char(buf,ch);
			return;
		}
	}
	send_to_char("Descriptor not found!\n\r",ch);
}

void do_pardon(struct char_data *ch, char *argument, int cmd)
{
	static char arg[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	struct char_data *victim;

	if (!IS_TRUSTED(ch))
		return;

	one_argument(argument, arg);

	if (!*arg)
	{
		send_to_char("Pardon whom?\n\r", ch);
	}
	else
	{
		if (!(victim = get_char(arg)))
			 send_to_char("They aren't here.\n\r", ch);
		else
		{
			if (ch == victim && GET_LEVEL(ch)!=LV_GOD)
			{
				send_to_char("Gods don't need pardoning.\n\r", ch);
				return;
			}
			if (IS_NPC(victim)) {
				send_to_char("Can't pardon NPCs.\n\r",ch);
				return;
			}
			if (IS_SET(victim->specials.act,PLR_ISTHIEF)){
				send_to_char("Thief flag killed.\n\r",ch);
				REMOVE_BIT(victim->specials.act,PLR_ISTHIEF);
			}
			if (IS_SET(victim->specials.act,PLR_ISKILLER)) {
				send_to_char("Killer flag removed.\n\r",ch);
				REMOVE_BIT(victim->specials.act,PLR_ISKILLER);
			}
			send_to_char("Done.\n\r",ch);
			sprintf(buf,"WIZINFO: (%s) pardon %s",GET_NAME(ch),
			  GET_NAME(victim));
			log(buf);
		}
	}
}

do_emote(struct char_data *ch, char *argument, int cmd)
{
	int i;
	static char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
		return;

	for (i = 0; *(argument + i) == ' '; i++);

	if (!*(argument + i))
		send_to_char("Yes.. But what?\n\r", ch);
	else
	{
		sprintf(buf,"$n %s", argument + i);
		act(buf,FALSE,ch,0,0,TO_ROOM);
		send_to_char("Ok.\n\r", ch);
	}
}



void do_echo(struct char_data *ch, char *argument, int cmd)
{
	int i;
	static char buf[MAX_STRING_LENGTH];

	if (!IS_TRUSTED(ch))
		return;

	for (i = 0; *(argument + i) == ' '; i++);

	if (!*(argument + i))
		send_to_char("That must be a mistake...\n\r", ch);
	else
	{
		sprintf(buf,"%s\n\r", argument + i);
		send_to_room_except(buf, ch->in_room, ch);
		send_to_char("Ok.\n\r", ch);
	}
}



void do_trans(struct char_data *ch, char *argument, int cmd)
{
  struct descriptor_data *i;
	struct char_data *victim;
	char buf[100];
	sh_int target;

	if (!IS_TRUSTED(ch))
		return;

	one_argument(argument,buf);
	if (!*buf)
		send_to_char("Whom do you wish to transfer?\n\r",ch);
	else if (str_cmp("all", buf)) {
		if (!(victim = get_char_vis(ch,buf)))
			send_to_char("No-one by that name around.\n\r",ch);
		else {
			act("$n disappears in a mushroom cloud.", FALSE, victim, 0, 0, TO_ROOM);
			target = ch->in_room;
			char_from_room(victim);
			char_to_room(victim,target);
			act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
			act("$n has transferred you!",FALSE,ch,0,victim,TO_VICT);
			do_look(victim,"",15);
			send_to_char("Ok.\n\r",ch);
		}
	} else { /* Trans All */
    for (i = descriptor_list; i; i = i->next)
			if (i->character != ch && !i->connected) {
				victim = i->character;
				act("$n disappears in a mushroom cloud.", FALSE, victim, 0, 0, TO_ROOM);
				target = ch->in_room;
				char_from_room(victim);
				char_to_room(victim,target);
				act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
				act("$n has transferred you!",FALSE,ch,0,victim,TO_VICT);
				do_look(victim,"",15);
			}

		send_to_char("Ok.\n\r",ch);
	}
}



void do_at(struct char_data *ch, char *argument, int cmd)
{
	char command[MAX_INPUT_LENGTH], loc_str[MAX_INPUT_LENGTH];
	int loc_nr, location, original_loc;
	struct char_data *target_mob;
	struct obj_data *target_obj;
	extern int top_of_world;

	if (!IS_TRUSTED(ch))
		return;

	half_chop(argument, loc_str, command);
	if (!*loc_str)
	{
		send_to_char("You must supply a room number or a name.\n\r", ch);
		return;
	}


	if (isdigit(*loc_str))
	{
		loc_nr = atoi(loc_str);
		for (location = 0; location <= top_of_world; location++)
			if (world[location].number == loc_nr)
				break;
			else if (location == top_of_world)
			{
				send_to_char("No room exists with that number.\n\r", ch);
				return;
			}
	}
	else if (target_mob = get_char_vis(ch, loc_str))
		location = target_mob->in_room;
	else if (target_obj = get_obj_vis(ch, loc_str))
		if (target_obj->in_room != NOWHERE)
			location = target_obj->in_room;
		else
		{
			send_to_char("The object is not available.\n\r", ch);
			return;
		}
	else
	{
		send_to_char("No such creature or object around.\n\r", ch);
		return;
	}

	/* a location has been found. */

	original_loc = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, location);
	command_interpreter(ch, command);

	/* check if the guy's still there */
	for (target_mob = world[location].people; target_mob; target_mob =
		target_mob->next_in_room)
		if (ch == target_mob)
		{
			char_from_room(ch);
			char_to_room(ch, original_loc);
		}
}


#if 0
/* Use new goto */

void do_goto(struct char_data *ch, char *argument, int cmd)
{
	char buf[MAX_INPUT_LENGTH];
	int loc_nr, location, i;
	struct char_data *target_mob, *pers;
	struct obj_data *target_obj;
	extern int top_of_world;

	void do_look(struct char_data *ch, char *argument, int cmd);

	if (IS_NPC(ch))
		return;

	one_argument(argument, buf);
	if (!*buf)
	{
		send_to_char("You must supply a room number or a name.\n\r", ch);
		return;
	}


	if (isdigit(*buf))
	{
		loc_nr = atoi(buf);
		for (location = 0; location <= top_of_world; location++)
			if (world[location].number == loc_nr)
				break;
			else if (location == top_of_world)
			{
				send_to_char("No room exists with that number.\n\r", ch);
				return;
			}
	}
	else if (target_mob = get_char_vis(ch, buf))
		location = target_mob->in_room;
	else if (target_obj = get_obj_vis(ch, buf))
		if (target_obj->in_room != NOWHERE)
			location = target_obj->in_room;
		else
		{
			send_to_char("The object is not available.\n\r", ch);
			return;
		}
	else
	{
		send_to_char("No such creature or object around.\n\r", ch);
		return;
	}

	/* a location has been found. */

	if (IS_SET(world[location].room_flags, PRIVATE))
	{
		for (i = 0, pers = world[location].people; pers; pers =
			pers->next_in_room, i++);
		if (i > 1)
		{
			send_to_char(
				"There's a private conversation going on in that room.\n\r", ch);
			return;
		}
	}

	act("$n disappears in a puff of smoke.", FALSE, ch, 0, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, location);
	act("$n appears with an ear-splitting bang.", FALSE, ch, 0,0,TO_ROOM);
	do_look(ch, "",15);
}
#endif


void do_stat(struct char_data *ch, char *argument, int cmd)
{
	extern char *spells[];
	struct affected_type *aff;
	char arg1[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	struct char_data *victim=0;
	struct room_data *rm=0;
	struct char_data *k=0;
	struct obj_data  *j=0;
	struct obj_data  *j2=0;
	struct extra_descr_data *desc;
	struct follow_type *fol;
	int i, virtual;
	int i2;
	bool found;

	/* for objects */
	extern char *item_types[];
	extern char *wear_bits[];
	extern char *extra_bits[];
	extern char *drinks[];

	/* for rooms */
	extern char *dirs[];
	extern char *room_bits[];
	extern char *exit_bits[];
	extern char *sector_types[];

	/* for chars */
	extern char *equipment_types[];
	extern char *affected_bits[];
	extern char *apply_types[];
	extern char *pc_class_types[];
	extern char *npc_class_types[];
	extern char *action_bits[];
	extern char *player_bits[];
	extern char *position_types[];
	extern char *connected_types[];

	if (!IS_TRUSTED(ch))
		return;

	argument = one_argument(argument, arg1);

	/* no argument */
	if (!*arg1) {
		send_to_char("Stats on who or what?\n\r",ch);
		return;
	} else {
		/* stats on room */
		if (!str_cmp("room", arg1)) {
			rm = &world[ch->in_room];
			sprintf(buf, "Room name: %s, Of zone : %d. V-Number : %d, R-number : %d\n\r",
			        rm->name, rm->zone, rm->number, ch->in_room);
			send_to_char(buf, ch);

			sprinttype(rm->sector_type,sector_types,buf2);
			sprintf(buf, "Sector type : %s", buf2);
			send_to_char(buf, ch);

			strcpy(buf,"Special procedure : ");
			strcat(buf,(rm->funct) ? "Exists\n\r" : "No\n\r");
			send_to_char(buf, ch);

			send_to_char("Room flags: ", ch);
			sprintbit((long) rm->room_flags,room_bits,buf);
			strcat(buf,"\n\r");
			send_to_char(buf,ch);

			send_to_char("Description:\n\r", ch);
			send_to_char(rm->description, ch);

			strcpy(buf, "Extra description keywords(s): ");
			if(rm->ex_description) {
				strcat(buf, "\n\r");
				for (desc = rm->ex_description; desc; desc = desc->next) {
					strcat(buf, desc->keyword);
					strcat(buf, "\n\r");
				}
				strcat(buf, "\n\r");
				send_to_char(buf, ch);
			} else {
				strcat(buf, "None\n\r");
				send_to_char(buf, ch);
			}

			strcpy(buf, "------- Chars present -------\n\r");
			for (k = rm->people; k; k = k->next_in_room)
			{
				if(CAN_SEE(ch,k)){
					strcat(buf, GET_NAME(k));
					strcat(buf,(!IS_NPC(k) ? "(PC)\n\r" : (!IS_MOB(k) ? "(NPC)\n\r" : "(MOB)\n\r")));
				}
			}
			strcat(buf, "\n\r");
			send_to_char(buf, ch);

			strcpy(buf, "--------- Contents ---------\n\r");
			for (j = rm->contents; j; j = j->next_content)
			{
				if(CAN_SEE_OBJ(ch,j)){
					strcat(buf, j->name);
					strcat(buf, "\n\r");
				}
			}
			strcat(buf, "\n\r");
			send_to_char(buf, ch);

			send_to_char("------- Exits defined -------\n\r", ch);
			for (i = 0; i <= 5; i++) {
				if (rm->dir_option[i]) {
					sprintf(buf,"Direction %s . Keyword : %s\n\r",
					        dirs[i], rm->dir_option[i]->keyword);
					send_to_char(buf, ch);
					strcpy(buf, "Description:\n\r  ");
					if(rm->dir_option[i]->general_description)
				  	strcat(buf, rm->dir_option[i]->general_description);
					else
						strcat(buf,"UNDEFINED\n\r");
					send_to_char(buf, ch);
					sprintbit(rm->dir_option[i]->exit_info,exit_bits,buf2);
					sprintf(buf, "Exit flag: %s \n\rKey no: %d\n\rTo room (R-Number): %d\n\r",
					        buf2, rm->dir_option[i]->key,
					        rm->dir_option[i]->to_room);
					send_to_char(buf, ch);
				}
			}
			return;
		}

		/* stat on object */
		if (j = get_obj_vis(ch, arg1)) {
			virtual = (j->item_number >= 0) ? obj_index[j->item_number].virtual : 0;
			sprintf(buf, "Object name: [%s], R-number: [%d], V-number: [%d] Item type: ",
			   j->name, j->item_number, virtual);
			sprinttype(GET_ITEM_TYPE(j),item_types,buf2);
			strcat(buf,buf2); strcat(buf,"\n\r");
			send_to_char(buf, ch);
			sprintf(buf, "Short description: %s\n\rLong description:\n\r%s\n\r",
			        ((j->short_description) ? j->short_description : "None"),
			        ((j->description) ? j->description : "None") );
			send_to_char(buf, ch);
			if(j->ex_description){
				strcpy(buf, "Extra description keyword(s):\n\r----------\n\r");
				for (desc = j->ex_description; desc; desc = desc->next) {
					strcat(buf, desc->keyword);
					strcat(buf, "\n\r");
				}
				strcat(buf, "----------\n\r");
				send_to_char(buf, ch);
			} else {
				strcpy(buf,"Extra description keyword(s): None\n\r");
				send_to_char(buf, ch);
			}

			send_to_char("Can be worn on :", ch);
			sprintbit(j->obj_flags.wear_flags,wear_bits,buf);
			strcat(buf,"\n\r");
			send_to_char(buf, ch);

			send_to_char("Set char bits  :", ch);
			sprintbit(j->obj_flags.bitvector,affected_bits,buf);
			strcat(buf,"\n\r");
			send_to_char(buf, ch);

			send_to_char("Extra flags: ", ch);
			sprintbit(j->obj_flags.extra_flags,extra_bits,buf);
			strcat(buf,"\n\r");
			send_to_char(buf,ch);

			sprintf(buf,"Weight: %d, Value: %d, Cost/day: %d, Timer: %d\n\r",
			       j->obj_flags.weight,j->obj_flags.cost,
			       j->obj_flags.cost_per_day,  j->obj_flags.timer);
			send_to_char(buf, ch);

			strcpy(buf,"In room: ");
			if (j->in_room == NOWHERE)
				strcat(buf,"Nowhere");
			else {
				sprintf(buf2,"%d",world[j->in_room].number);
				strcat(buf,buf2);
			}
			strcat(buf," ,In object: ");
			strcat(buf, (!j->in_obj ? "None" : fname(j->in_obj->name)));
			strcat(buf," ,Carried by:");
			strcat(buf, (!j->carried_by) ? "Nobody" : GET_NAME(j->carried_by));
			strcat(buf,"\n\r");
			send_to_char(buf, ch);

			switch (j->obj_flags.type_flag) {
				case ITEM_LIGHT :
					sprintf(buf, "Colour : [%d]\n\rType : [%d]\n\rHours : [%d]",
						j->obj_flags.value[0],
						j->obj_flags.value[1],
						j->obj_flags.value[2]);
					break;
				case ITEM_SCROLL :
					sprintf(buf, "Spells : %d, %d, %d, %d",
						j->obj_flags.value[0],
				 		j->obj_flags.value[1],
				 		j->obj_flags.value[2],
				 		j->obj_flags.value[3] );
					break;
				case ITEM_WAND :
					sprintf(buf, "Spell : %d\n\rMana : %d",
						j->obj_flags.value[0],
						j->obj_flags.value[1]);
					break;
				case ITEM_STAFF :
					sprintf(buf, "Spell : %d\n\rMana : %d",
						j->obj_flags.value[0],
						j->obj_flags.value[1]);
					break;
				case ITEM_WEAPON :
					sprintf(buf, "Tohit : %d\n\rTodam : %dD%d\n\rType : %d",
						j->obj_flags.value[0],
	    			j->obj_flags.value[1],
	    			j->obj_flags.value[2],
	    			j->obj_flags.value[3]);
					break;
				case ITEM_FIREWEAPON :
					sprintf(buf, "Tohit : %d\n\rTodam : %dD%d\n\rType : %d",
						j->obj_flags.value[0],
	    			j->obj_flags.value[1],
	    			j->obj_flags.value[2],
	    			j->obj_flags.value[3]);
					break;
				case ITEM_MISSILE :
					sprintf(buf, "Tohit : %d\n\rTodam : %d\n\rType : %d",
						j->obj_flags.value[0],
						j->obj_flags.value[1],
						j->obj_flags.value[3]);
					break;
				case ITEM_ARMOR :
					sprintf(buf, "AC-apply : [%d]",
						j->obj_flags.value[0]);
					break;
				case ITEM_POTION :
					sprintf(buf, "Spells : %d, %d, %d, %d",
						j->obj_flags.value[0],
						j->obj_flags.value[1],
						j->obj_flags.value[2],
						j->obj_flags.value[3]);
					break;
				case ITEM_TRAP :
					sprintf(buf, "Spell : %d\n\r- Hitpoints : %d",
						j->obj_flags.value[0],
						j->obj_flags.value[1]);
					break;
				case ITEM_CONTAINER :
					sprintf(buf, "Max-contains : %d\n\rLocktype : %d\n\rCorpse : %s",
						j->obj_flags.value[0],
						j->obj_flags.value[1],
						j->obj_flags.value[3]?"Yes":"No");
					break;
				case ITEM_DRINKCON :
					sprinttype(j->obj_flags.value[2],drinks,buf2);
					sprintf(buf, "Max-contains : %d\n\rContains : %d\n\rPoisoned : %d\n\rLiquid : %s",
						j->obj_flags.value[0],
						j->obj_flags.value[1],
						j->obj_flags.value[3],
						buf2);
					break;
				case ITEM_NOTE :
					sprintf(buf, "Tounge : %d",
						j->obj_flags.value[0]);
					break;
				case ITEM_KEY :
					sprintf(buf, "Keytype : %d",
						j->obj_flags.value[0]);
					break;
				case ITEM_FOOD :
					sprintf(buf, "Makes full : %d\n\rPoisoned : %d",
						j->obj_flags.value[0],
						j->obj_flags.value[3]);
					break;
				default :
					sprintf(buf,"Values 0-3 : [%d] [%d] [%d] [%d]",
						j->obj_flags.value[0],
						j->obj_flags.value[1],
						j->obj_flags.value[2],
						j->obj_flags.value[3]);
					break;
			}
			send_to_char(buf, ch);

			strcpy(buf,"\n\rEquipment Status: ");
			if (!j->carried_by)
				strcat(buf,"NONE");
			else {
				found = FALSE;
				for (i=0;i < MAX_WEAR;i++) {
					if (j->carried_by->equipment[i] == j) {
						sprinttype(i,equipment_types,buf2);
						strcat(buf,buf2);
						found = TRUE;
					}
				}
				if (!found)
					strcat(buf,"Inventory");
			}
			send_to_char(buf, ch);

			strcpy(buf, "\n\rSpecial procedure : ");
			if (j->item_number >= 0)
				strcat(buf, (obj_index[j->item_number].func ? "exists\n\r" : "No\n\r"));
			else
				strcat(buf, "No\n\r");
			send_to_char(buf, ch);

			strcpy(buf, "Contains :\n\r");
			found = FALSE;
			for(j2=j->contains;j2;j2 = j2->next_content) {
				strcat(buf,fname(j2->name));
				strcat(buf,"\n\r");
				found == TRUE;
			}
			if (!found)
				strcpy(buf,"Contains : Nothing\n\r");
			send_to_char(buf, ch);

			send_to_char("Can affect char :\n\r", ch);
			for (i=0;i<MAX_OBJ_AFFECT;i++) {
				sprinttype(j->affected[i].location,apply_types,buf2);
				sprintf(buf,"    Affects : %s By %d\n\r", buf2,j->affected[i].modifier);
				send_to_char(buf, ch);
			}
			return;
		}

		/* mobile in world */
		if (k = get_char_vis(ch, arg1)){

			switch(k->player.sex) {
				case SEX_NEUTRAL :
					strcpy(buf,"NEUTRAL-SEX");
					break;
				case SEX_MALE :
					strcpy(buf,"MALE");
					break;
				case SEX_FEMALE :
					strcpy(buf,"FEMALE");
					break;
				default :
					strcpy(buf,"ILLEGAL-SEX!!");
					break;
			}

			sprintf(buf2, " %s - Name : %s [R-Number%d], In room [%d]\n\r",
			   (!IS_NPC(k) ? "PC" : (!IS_MOB(k) ? "NPC" : "MOB")),
					GET_NAME(k), k->nr, world[k->in_room].number);
			strcat(buf, buf2);
			send_to_char(buf, ch);
			if (IS_MOB(k)) {
				sprintf(buf, "V-Number [%d]\n\r", mob_index[k->nr].virtual);
				send_to_char(buf, ch);
			}

			strcpy(buf,"Short description: ");
			strcat(buf, (k->player.short_descr ? k->player.short_descr : "None"));
			strcat(buf,"\n\r");
			send_to_char(buf,ch);

			strcpy(buf,"Title: ");
			strcat(buf, (k->player.title ? k->player.title : "None"));
			strcat(buf,"\n\r");
			send_to_char(buf,ch);

			send_to_char("Long description: ", ch);
			if (k->player.long_descr)
				send_to_char(k->player.long_descr, ch);
			else
				send_to_char("None", ch);
			send_to_char("\n\r", ch);

			if (IS_NPC(k)) {
				strcpy(buf,"Monster Class: ");
				sprinttype(k->player.class,npc_class_types,buf2);
			} else {
				strcpy(buf,"Class: ");
				sprinttype(k->player.class,pc_class_types,buf2);
			}
			strcat(buf, buf2);

			sprintf(buf2,"   Level [%d] Alignment[%d]\n\r",k->player.level,
			              k->specials.alignment);
			strcat(buf, buf2);
			send_to_char(buf, ch);

			sprintf(buf,"Birth : [%ld]secs, Logon[%ld]secs, Played[%ld]secs\n\r",
			        k->player.time.birth,
			        k->player.time.logon,
			        k->player.time.played);

			send_to_char(buf, ch);

			sprintf(buf,"Age: [%d] Years,  [%d] Months,  [%d] Days,  [%d] Hours\n\r",
			        age(k).year, age(k).month, age(k).day, age(k).hours);
			send_to_char(buf,ch);

			sprintf(buf,"Height [%d]cm  Weight [%d]pounds \n\r", GET_HEIGHT(k), GET_WEIGHT(k));
			send_to_char(buf,ch);

			sprintf(buf,"Hometown[%d], Speaks[%d/%d/%d], (STL[%d]/per[%d]/NSTL[%d])\n\r",
				k->player.hometown,
				k->player.talks[0],
				k->player.talks[1],
				k->player.talks[2],
				k->specials.spells_to_learn,
				int_app[GET_INT(k)].learn,
				wis_app[GET_WIS(k)].bonus);
			send_to_char(buf, ch);

			sprintf(buf,"Str:[%d/%d]  Int:[%d]  Wis:[%d]  Dex:[%d]  Con:[%d]\n\r",
				GET_STR(k), GET_ADD(k),
				GET_INT(k),
				GET_WIS(k),
				GET_DEX(k),
				GET_CON(k) );
			send_to_char(buf,ch);

			sprintf(buf,"Mana p.:[%d/%d+%d]  Hit p.:[%d/%d+%d]  Move p.:[%d/%d+%d]\n\r",
				GET_MANA(k),mana_limit(k),mana_gain(k),
				GET_HIT(k),hit_limit(k),hit_gain(k),
				GET_MOVE(k),move_limit(k),move_gain(k) );
			send_to_char(buf,ch);

			sprintf(buf,"AC:[%d/10], Coins: [%d], Exp: [%d], Hitroll: [%d], Damroll: [%d]\n\r",
				GET_AC(k),
				GET_GOLD(k),
				GET_EXP(k),
				k->points.hitroll,
				k->points.damroll );
			send_to_char(buf,ch);

			sprinttype(GET_POS(k),position_types,buf2);
			sprintf(buf,"Position: %s, Fighting: %s",buf2,
			        ((k->specials.fighting) ? GET_NAME(k->specials.fighting) : "Nobody") );
			if (k->desc) {
				sprinttype(k->desc->connected,connected_types,buf2);
				strcat(buf,", Connected: ");
				strcat(buf,buf2);
			}
			strcat(buf,"\n\r");
			send_to_char(buf, ch);

			strcpy(buf,"Default position: ");
			sprinttype((k->specials.default_pos),position_types,buf2);
			strcat(buf, buf2);
			if (IS_NPC(k))
			{
				strcat(buf,",NPC flags: ");
				sprintbit(k->specials.act,action_bits,buf2);
			}
			else
			{
				strcat(buf,",PC flags: ");
				sprintbit(k->specials.act,player_bits,buf2);
			}

			strcat(buf, buf2);

			sprintf(buf2,",Timer [%d] \n\r", k->specials.timer);
			strcat(buf, buf2);
			send_to_char(buf, ch);

			if (IS_MOB(k)) {
				strcpy(buf, "\n\rMobile Special procedure : ");
				strcat(buf, (mob_index[k->nr].func ? "Exists\n\r" : "None\n\r"));
				send_to_char(buf, ch);
			}

			if (IS_NPC(k)) {
				sprintf(buf, "NPC Bare Hand Damage %dd%d.\n\r",
					k->specials.damnodice, k->specials.damsizedice);
				send_to_char(buf, ch);
			}

			sprintf(buf,"Carried weight: %d   Carried items: %d\n\r",
				IS_CARRYING_W(k),
				IS_CARRYING_N(k) );
			send_to_char(buf,ch);

			for(i=0,j=k->carrying;j;j=j->next_content,i++);
			sprintf(buf,"Items in inventory: %d, ",i);

			for(i=0,i2=0;i<MAX_WEAR;i++)
				if (k->equipment[i]) i2++;
			sprintf(buf2,"Items in equipment: %d\n\r", i2);
			strcat(buf,buf2);
			send_to_char(buf, ch);

			sprintf(buf,"Apply saving throws: [%d] [%d] [%d] [%d] [%d]\n\r",
			        k->specials.apply_saving_throw[0],
			        k->specials.apply_saving_throw[1],
			        k->specials.apply_saving_throw[2],
			        k->specials.apply_saving_throw[3],
			        k->specials.apply_saving_throw[4]);
			send_to_char(buf, ch);

			sprintf(buf, "Thirst: %d, Hunger: %d, Drunk: %d\n\r",
			        k->specials.conditions[THIRST],
			        k->specials.conditions[FULL],
			        k->specials.conditions[DRUNK]);
			send_to_char(buf, ch);

			sprintf(buf, "Master is '%s'\n\r",
				((k->master) ? GET_NAME(k->master) : "NOBODY"));
			send_to_char(buf, ch);
			send_to_char("Followers are:\n\r", ch);
			for(fol=k->followers; fol; fol = fol->next)
				act("    $N", FALSE, ch, 0, fol->follower, TO_CHAR);

			/* Showing the bitvector */
			sprintbit(k->specials.affected_by,affected_bits,buf);
			send_to_char("Affected by: ", ch);
			strcat(buf,"\n\r");
			send_to_char(buf, ch);

			/* Routine to show what spells a char is affected by */
			if (k->affected) {
				send_to_char("\n\rAffecting Spells:\n\r--------------\n\r", ch);
				for(aff = k->affected; aff; aff = aff->next) {
					sprintf(buf, "Spell : '%s'\n\r",spells[aff->type-1]);
					send_to_char(buf, ch);
					sprintf(buf,"     Modifies %s by %d points\n\r",
						apply_types[aff->location], aff->modifier);
					send_to_char(buf, ch);
					sprintf(buf,"     Expires in %3d hours, Bits set ",
						aff->duration);
					send_to_char(buf, ch);
					sprintbit(aff->bitvector,affected_bits,buf);
					strcat(buf,"\n\r");
					send_to_char(buf, ch);
				}
			}
		 	sprintf(buf,"Wizinvis : %s\n\r",
				((k->specials.wizInvis) ? "ON" : "OFF"));
 			send_to_char (buf, ch);
			sprintf(buf,"Holylite : %s\n\r",
				((k->specials.holyLite) ? "ON" : "OFF"));
			send_to_char(buf,ch);

			return;
		} else {
			send_to_char("No mobile or object by that name in the world\n\r", ch);
		}
	}
}



void do_shutdow(struct char_data *ch, char *argument, int cmd)
{
	if(!IS_TRUSTED(ch))
	{
		return;
	}
	send_to_char("If you want to shut something down - say so!\n\r", ch);
}



void do_shutdown(struct char_data *ch, char *argument, int cmd)
{
	extern int diku_shutdown, reboot;
	char buf[100], arg[MAX_INPUT_LENGTH];

	if (IS_NPC(ch))
		return;

	one_argument(argument, arg);

	if (!*arg)
	{
		sprintf(buf, "Shutdown by %s.", GET_NAME(ch) );
		send_to_all(buf);
		log(buf);
		diku_shutdown = 1;
	}
	else if (!str_cmp(arg, "reboot"))
	{
		sprintf(buf, "Reboot by %s.", GET_NAME(ch));
		send_to_all(buf);
		log(buf);
		diku_shutdown = reboot = 1;
	}
	else
		send_to_char("Go shut down someone your own size.\n\r", ch);
}


void do_snoop(struct char_data *ch, char *argument, int cmd)
{
	static char arg[MAX_STRING_LENGTH];
	struct char_data *victim;

	if (!ch->desc)
		return;

	if(IS_NPC(ch)){
		send_to_char("Go away, monsters can't snoop!\n",ch);
		return;
	}
	if(!IS_TRUSTED(ch)) return;

	one_argument(argument, arg);

	if(!*arg)
	{
		send_to_char("Snoop whom ?\n\r",ch);
		return;
	}

	if(!(victim=get_char_vis(ch, arg)))
	{
		send_to_char("No such person around.\n\r",ch);
		return;
	}

	if(!victim->desc)
	{
		send_to_char("There's no link.. nothing to snoop.\n\r",ch);
		return;
	}

	if(victim == ch)
	{
		send_to_char("Ok, you just snoop yourself.\n\r",ch);
		if(ch->desc->snoop.snooping)
			{
				ch->desc->snoop.snooping->desc->snoop.snoop_by = 0;
				ch->desc->snoop.snooping = 0;
			}
		return;
	}

	if(victim->desc->snoop.snoop_by)
	{
		send_to_char("Busy already. \n\r",ch);
		return;
	}

	if(GET_LEVEL(victim)>=GET_LEVEL(ch))
	{
		send_to_char("You failed.\n\r",ch);
		return;
	}

	send_to_char("Ok. \n\r",ch);

	if(ch->desc->snoop.snooping)
		ch->desc->snoop.snooping->desc->snoop.snoop_by = 0;

	ch->desc->snoop.snooping = victim;
	victim->desc->snoop.snoop_by = ch;
	return;
}

void do_switch(struct char_data *ch, char *argument, int cmd)
{
	static char arg[MAX_STRING_LENGTH];
	char buf[70];
	struct char_data *victim;

	if (!IS_TRUSTED(ch))
		return;

	one_argument(argument, arg);

	if (!*arg)
	{
		send_to_char("Switch with whom?\n\r", ch);
	}
	else
	{
		if (!(victim = get_char(arg)))
			 send_to_char("They aren't here.\n\r", ch);
		else
		{
			if (ch == victim)
			{
				send_to_char("He he he... We are jolly funny today, eh?\n\r", ch);
				return;
			}

			if (!ch->desc || ch->desc->snoop.snoop_by || ch->desc->snoop.snooping)
			{
				send_to_char("Mixing snoop & switch is bad for your health.\n\r", ch);
				return;
			}

			if(victim->desc || (!IS_NPC(victim)))
      {
				send_to_char(
           "You can't do that, the body is already in use!\n\r",ch);
			}
			else
			{
				send_to_char("Ok.\n\r", ch);

				ch->desc->character = victim;
				ch->desc->original = ch;

				victim->desc = ch->desc;
				ch->desc = 0;
			}
		}
	}
}



void do_return(struct char_data *ch, char *argument, int cmd)
{
	static char arg[MAX_STRING_LENGTH];
	char buf[70];

	if(!ch->desc)
		return;

	if(!ch->desc->original)
	{
		send_to_char("Yeah, right...!?!\n\r", ch);
		return;
	}
	else
	{
		send_to_char("You return to your original body.\n\r",ch);

		ch->desc->character = ch->desc->original;
		ch->desc->original = 0;

		ch->desc->character->desc = ch->desc;
		ch->desc = 0;
	}
}

#if 0
/* Use new force */

void do_force(struct char_data *ch, char *argument, int cmd)
{
  struct descriptor_data *i;
	struct char_data *vict;
	char name[100], to_force[100],buf[100];

	if (IS_NPC(ch))
		return;

	half_chop(argument, name, to_force);

	if (!*name || !*to_force)
		 send_to_char("Who do you wish to force to do what?\n\r", ch);
	else if (str_cmp("all", name)) {
		if (!(vict = get_char_vis(ch, name)))
			send_to_char("No-one by that name here..\n\r", ch);
		else
		{
			if (GET_LEVEL(ch) < GET_LEVEL(vict))
				/* To prevent lower levels from finding out
				 * if a god is on, we won't respond if the
				 * god is LV_GOD.
				 */
				if(GET_LEVEL(vict)!=LV_GOD){
					send_to_char("Oh no you don't!!\n\r", ch);
				}
			else {
				sprintf(buf, "$n has forced you to '%s'.", to_force);
				act(buf, FALSE, ch, 0, vict, TO_VICT);
				send_to_char("Ok.\n\r", ch);
				command_interpreter(vict, to_force);
			}
		}
	} else { /* force all */
    for (i = descriptor_list; i; i = i->next)
			if (i->character != ch && !i->connected) {
				vict = i->character;
				if (GET_LEVEL(ch) < GET_LEVEL(vict)){
					if(GET_LEVEL(vict)!=24){
						send_to_char("Oh no you don't!!\n\r", ch);
					}
				} else {
					sprintf(buf, "$n has forced you to '%s'.", to_force);
					act(buf, FALSE, ch, 0, vict, TO_VICT);
					command_interpreter(vict, to_force);
				}
			}
			send_to_char("Ok.\n\r", ch);
	}
}
#endif


void do_load(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *mob;
	struct obj_data *obj;
	char type[100], num[100], buf[100];
	int number, r_num;
	char logit[MAX_STRING_LENGTH];

	extern int top_of_mobt;
	extern int top_of_objt;

	if (!IS_TRUSTED(ch))
		return;

	sprintf(logit,"WIZINFO: (%s) load %s",ch->player.name,argument);
	log(logit);
	argument_interpreter(argument, type, num);

	if (!*type || !*num || !isdigit(*num))
	{
		send_to_char("Syntax:\n\rload <'char' | 'obj'> <number>.\n\r", ch);
		return;
	}

	if ((number = atoi(num)) < 0)
	{
		send_to_char("A NEGATIVE number??\n\r", ch);
		return;
	}
	if (is_abbrev(type, "char"))
	{
		if ((r_num = real_mobile(number)) < 0)
		{
			send_to_char("There is no monster with that number.\n\r", ch);
			return;
 		}
		mob = read_mobile(r_num, REAL);
		char_to_room(mob, ch->in_room);

		act("$n makes a quaint, magical gesture with one hand.", TRUE, ch,
			0, 0, TO_ROOM);
		act("$n has created $N!", FALSE, ch, 0, mob, TO_ROOM);
		send_to_char("Done.\n\r", ch);
	}
	else if (is_abbrev(type, "obj"))
	{
		if ((r_num = real_object(number)) < 0)
		{
			send_to_char("There is no object with that number.\n\r", ch);
			return;
		}
		obj = read_object(r_num, REAL);
		obj_to_room(obj, ch->in_room);
		act("$n makes a strange magical gesture.", TRUE, ch, 0, 0, TO_ROOM);
		act("$n has created $p!", FALSE, ch, obj, 0, TO_ROOM);
		send_to_char("Ok.\n\r", ch);
	}
	else
		send_to_char("That'll have to be either 'char' or 'obj'.\n\r", ch);
}





/* clean a room of all mobiles and objects */
void do_purge(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *vict, *next_v;
	struct obj_data *obj, *next_o;

	char name[100], buf[100];

	if (!IS_TRUSTED(ch))
		return;

	one_argument(argument, name);

	if (*name)  /* argument supplied. destroy single object or char */
	{
		if (vict = get_char_room_vis(ch, name))
		{
			if (!IS_NPC(vict) && (GET_LEVEL(ch)<LV_GOD)) {
				send_to_char("Fuuuuuuuuu!\n\r", ch);
				return;
			}

			act("$n disintegrates $N.", FALSE, ch, 0, vict, TO_NOTVICT);

			if (IS_NPC(vict)) {
				extract_char(vict);
			} else {
				if (vict->desc)
				{
					close_socket(vict->desc);
					vict->desc = 0;
					extract_char(vict);
				}
				else
				{
					extract_char(vict);
				}
			}
		}
		else if (obj = get_obj_in_list_vis(ch, name,
			world[ch->in_room].contents))
		{
			act("$n destroys $p.", FALSE, ch, obj, 0, TO_ROOM);
			extract_obj(obj);
		}
		else
		{
			send_to_char("I don't know anyone or anything by that name.\n\r", ch);
			return;
		}

		send_to_char("Ok.\n\r", ch);
	}
	else   /* no argument. clean out the room */
	{
		if (IS_NPC(ch))
		{
			send_to_char("Don't... You would only kill yourself..\n\r", ch);
			return;
		}

		act("$n gestures... You are surrounded by scorching flames!",
			FALSE, ch, 0, 0, TO_ROOM);
		send_to_room("The world seems a little cleaner.\n\r", ch->in_room);

		for (vict = world[ch->in_room].people; vict; vict = next_v)
		{
			next_v = vict->next_in_room;
			if (IS_NPC(vict))
				extract_char(vict);
		}

		for (obj = world[ch->in_room].contents; obj; obj = next_o)
		{
			next_o = obj->next_content;
			extract_obj(obj);
		}
	}
}



/* Give pointers to the five abilities */
void roll_abilities(struct char_data *ch)
{
	int i, j, k, temp;
	ubyte table[5];
	ubyte rools[4];

	for(i=0; i<5; table[i++]=0)  ;

	for(i=0; i<5; i++) {

		for(j=0; j<4; j++)
			rools[j] = number(1,6);

		temp = rools[0]+rools[1]+rools[2]+rools[3] -
		           MIN(rools[0], MIN(rools[1], MIN(rools[2],rools[3])));

		for(k=0; k<5; k++)
			if (table[k] < temp)
				SWITCH(temp, table[k]);
	}

	ch->abilities.str_add = 0;

	switch (GET_CLASS(ch)) {
		case CLASS_MAGIC_USER: {
			ch->abilities.intel = table[0];
			ch->abilities.wis = table[1];
			ch->abilities.dex = table[2];
			ch->abilities.str = table[3];
			ch->abilities.con = table[4];
		}	break;
		case CLASS_CLERIC: {
			ch->abilities.wis = table[0];
			ch->abilities.intel = table[1];
			ch->abilities.str = table[2];
			ch->abilities.dex = table[3];
			ch->abilities.con = table[4];
		} break;
		case CLASS_THIEF: {
			ch->abilities.dex = table[0];
			ch->abilities.str = table[1];
			ch->abilities.con = table[2];
			ch->abilities.intel = table[3];
			ch->abilities.wis = table[4];
		} break;
		case CLASS_WARRIOR: {
			ch->abilities.str = table[0];
			ch->abilities.dex = table[1];
			ch->abilities.con = table[2];
			ch->abilities.wis = table[3];
			ch->abilities.intel = table[4];
			if (ch->abilities.str == 18)
				ch->abilities.str_add = number(0,100);
		} break;
	}
	ch->tmpabilities = ch->abilities;
}



void do_start(struct char_data *ch)
{
	int i, j;
	byte table[5];
	byte rools[4];

	extern struct dex_skill_type dex_app_skill[];
	void advance_level(struct char_data *ch);

	send_to_char("Welcome. This is now your character in Copper DikuMud,\n\rYou can now earn XP, and lots more...\n\r", ch);

	GET_LEVEL(ch) = 1;
	GET_EXP(ch) = 1;

	set_title(ch);

	roll_abilities(ch);

	ch->points.max_hit  = 10;  /* These are BASE numbers   */

	switch (GET_CLASS(ch)) {

		case CLASS_MAGIC_USER : {
		} break;

		case CLASS_CLERIC : {
		} break;

		case CLASS_THIEF : {
			ch->skills[SKILL_SNEAK].learned = 10;
			ch->skills[SKILL_HIDE].learned =  5;
			ch->skills[SKILL_STEAL].learned = 15;
			ch->skills[SKILL_BACKSTAB].learned = 10;
			ch->skills[SKILL_PICK_LOCK].learned = 10;
		} break;

		case CLASS_WARRIOR: {
		} break;
	}

	advance_level(ch);

	GET_HIT(ch) = hit_limit(ch);
	GET_MANA(ch) = mana_limit(ch);
	GET_MOVE(ch) = move_limit(ch);

	GET_COND(ch,THIRST) = 24;
	GET_COND(ch,FULL) = 24;
	GET_COND(ch,DRUNK) = 0;

  ch->player.time.played = 0;
  ch->player.time.logon = time(0);

}


void do_advance(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	char name[100], level[100], buf[300], passwd[100];
	int adv, newlevel;
	int i;

	void gain_exp(struct char_data *ch, int gain);
  void advance_level(struct char_data *ch);

	if (!IS_TRUSTED(ch))
		return;

	half_chop(argument, name, buf);
	argument_interpreter(buf, level, passwd);

	if (*name)
	{
		if (!(victim = get_char_room_vis(ch, name)))
		{
			send_to_char("That player is not here.\n\r", ch);
			return;
		}
	} else {
		send_to_char("Advance whom?\n\r", ch);
		return;
	}

	if (IS_NPC(victim)) {
		send_to_char("NO! Not on NPC's.\n\r", ch);
		return;
	}

	if (GET_LEVEL(victim) == 0)
		adv = 1;
	else if (!*level) {
		send_to_char("You must supply a level number.\n\r", ch);
		return;
	} else {
		if (!isdigit(*level))
		{
			send_to_char("Second argument must be a positive integer.\n\r",ch);
			return;
		}
	}

	if ( GET_LEVEL(ch) < LV_GOD )
	{
		send_to_char("Thou art not godly enough.\n\r", ch);
		return;
	}

	newlevel = atoi(level);

	/* Swiftest--changed so that you can decrease player's level*/

	if (newlevel<1)
	{
		send_to_char("1 is the lowest possible level.\n\r", ch);
		return;
	}

	if (newlevel > LV_GOD)
	{
		send_to_char("Too high...try again.\n\r", ch);
		return;
	}

	if (str_cmp(passwd,"jfkxyzzy") != 0) {
		send_to_char("You try, but can't!\n\r", ch);
		return;
 	} /* if */

/* Lower level:  First reset the player to level 1. Remove all special
abilities (all spells, BASH, STEAL, etc).  Reset spells_to_learn to zero.
Then act as though you're raising a first level character to a higher
level. Note, currently, an implementor can lower another imp.   -Swiftest
*/

	if (newlevel  <= GET_LEVEL(victim))
	{
		send_to_char("Warning: Lowering a player's level!\n\r",ch);


		GET_LEVEL(victim) = 1;
		GET_EXP(victim) = 1;

		set_title(victim);

		victim->points.max_hit  = 10;  /* These are BASE numbers  */
		for(i=1;i<=MAX_SKILLS;i++) { /* Zero them out first */
			victim->skills[i].learned=0;
		}
		victim->specials.spells_to_learn = 0;

		switch (GET_CLASS(victim)) {

			case CLASS_THIEF : {
				victim->skills[SKILL_SNEAK].learned = 10;
				victim->skills[SKILL_HIDE].learned =  5;
				victim->skills[SKILL_STEAL].learned = 15;
				victim->skills[SKILL_BACKSTAB].learned = 10;
				victim->skills[SKILL_PICK_LOCK].learned = 10;
			} break;

		}


		GET_HIT(victim) = hit_limit(victim);
		GET_MANA(victim) = mana_limit(victim);
		GET_MOVE(victim) = move_limit(victim);

		GET_COND(victim,THIRST) = 24;
		GET_COND(victim,FULL) = 24;
		GET_COND(victim,DRUNK) = 0;

		advance_level(victim);
	}

	adv = newlevel - GET_LEVEL(victim);


	send_to_char("You feel generous.\n\r", ch);
  act("$n makes some strange gestures.\n\rA strange feeling comes uppon you,"
			"\n\rLike a giant hand, light comes down from\n\rabove, grabbing your "
			"body, that begins\n\rto pulse with colored lights from inside.\n\rYo"
			"ur head seems to be filled with demons\n\rfrom another plane as your"
			" body dissolves\n\rto the elements of time and space itself.\n\rSudde"
			"nly a silent explosion of light snaps\n\ryou back to reality. You fee"
			"l slightly\n\rdifferent.",FALSE,ch,0,victim,TO_VICT);


	if (GET_LEVEL(victim) == 0) {
		do_start(victim);
	} else {
		if (GET_LEVEL(victim) < LV_GOD) {
			gain_exp_regardless(victim, (titles[GET_CLASS(victim)-1]

				[GET_LEVEL(victim)+adv].exp)-GET_EXP(victim));
			send_to_char("Reminder: Be careful.\n\r", ch);
			if(GET_LEVEL(victim)>=LV_IMMORTAL){
				send_to_char("If you want to be immortal, type HELP on IMMORTAL, POLICY, and TRUSTED!\n\r",victim);
			}
		} else {
			send_to_char("Some idiot just tried to advance your level.\n\r",
				victim);
			send_to_char("IMPOSSIBLE! IDIOTIC!\n\r", ch);
		}
	}
}


void do_reroll(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	char buf[100];
	sh_int target;

	if (!IS_TRUSTED(ch))
		return;

	one_argument(argument,buf);
	if (!*buf)
		send_to_char("Whom do you wich to reroll?\n\r",ch);
	else
		if(!(victim = get_char(buf)))
			send_to_char("No-one by that name in the world.\n\r",ch);
		else {
			send_to_char("Rerolled...\n\r", ch);
			roll_abilities(victim);
		}
}


void do_restore(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *victim;
	char buf[100];
	int i;

	void update_pos( struct char_data *victim );

	if (!IS_TRUSTED(ch))
		return;

	one_argument(argument,buf);
	if (!*buf)
		send_to_char("Who do you wich to restore?\n\r",ch);
	else
		if(!(victim = get_char(buf)))
			send_to_char("No-one by that name in the world.\n\r",ch);
		else {
			GET_MANA(victim) = GET_MAX_MANA(victim);
			GET_HIT(victim) = GET_MAX_HIT(victim);
			GET_MOVE(victim) = GET_MAX_MOVE(victim);

			if(IS_TRUSTED(victim)){
				for (i = 0; i < MAX_SKILLS; i++) {
					victim->skills[i].learned = 100;
					victim->skills[i].recognise = TRUE;
				}

				if (GET_LEVEL(victim) >= LV_LESSERGOD) {
					victim->abilities.str_add = 100;
					victim->abilities.intel = 25;
					victim->abilities.wis = 25;
					victim->abilities.dex = 25;
					victim->abilities.str = 25;
					victim->abilities.con = 25;
				}
				victim->tmpabilities = victim->abilities;

			}
			update_pos( victim );
			send_to_char("Done.\n\r", ch);
			act("You have been fully healed by $N!", FALSE, victim, 0, ch, TO_CHAR);
		}
}




do_noshout(struct char_data *ch, char *argument, int cmd)
{
	struct char_data *vict;
	struct obj_data *dummy;
	char buf[MAX_INPUT_LENGTH];

	if (IS_NPC(ch))
		return;

	one_argument(argument, buf);

	if (!*buf)
		if (IS_SET(ch->specials.act, PLR_NOSHOUT))
		{
			send_to_char("***You can now hear shouts again.\n\r", ch);
			REMOVE_BIT(ch->specials.act, PLR_NOSHOUT);
		}
		else
		{
			send_to_char("***From now on, you won't hear shouts.\n\r", ch);
			SET_BIT(ch->specials.act, PLR_NOSHOUT);
		}
	else if (!generic_find(argument, FIND_CHAR_WORLD, ch, &vict, &dummy))
		send_to_char("Couldn't find any such creature.\n\r", ch);
	else if (IS_NPC(vict))
		send_to_char("Can't do that to a beast.\n\r", ch);
	else if (GET_LEVEL(vict) > GET_LEVEL(ch))
		act("$E might object to that.. better not.", 0, ch, 0, vict, TO_CHAR);
	else if (IS_SET(vict->specials.act, PLR_NOSHOUT))
	{
		send_to_char("***You can shout again.\n\r", vict);
		send_to_char("NOSHOUT removed.\n\r", ch);
		REMOVE_BIT(vict->specials.act, PLR_NOSHOUT);
	}
	else
	{
		send_to_char("***The gods take away your ability to shout!\n\r", vict);
		send_to_char("NOSHOUT set.\n\r", ch);
		SET_BIT(vict->specials.act, PLR_NOSHOUT);
	}
}

/*********************************************************************/
/* New routines by Dionysos.                                         */
/*********************************************************************/

void do_wizinvis(struct char_data *ch, char *argument, int cmd)
{
   struct affected_type af;
   struct affected_type *hjp;

   if(!IS_TRUSTED(ch)){
	return;
   }
   for(hjp = ch->affected;
       ((hjp != NULL) && (hjp->type != SPELL_INVISIBLE));
       hjp = hjp->next);

   if ((hjp != NULL) && (ch->specials.wizInvis)) {
      /* Re-appear */
      hjp->type = SPELL_INVISIBLE;
      hjp->duration  = 0;
      hjp->modifier  = 0;
      hjp->location  = APPLY_NONE;
      hjp->bitvector = AFF_INVISIBLE;
      affect_remove(ch, hjp);

      ch->specials.wizInvis = FALSE;
      send_to_char("You slowly fade back into the world of mortals.\n\r",ch);
      act("Someone utters the words 'izeghy visimon'.\n$n slowly fades into existence.", FALSE, ch, 0, 0, TO_ROOM);
   }
   else {
      /* Disappear */
      if (hjp != NULL) {   /* remove normal invis spell */
	 hjp->type = SPELL_INVISIBLE;
	 hjp->duration = 0;
	 hjp->modifier = 0;
	 hjp->location = APPLY_NONE;
	 hjp->bitvector = AFF_INVISIBLE;
	 affect_remove(ch, hjp);
      }
      af.type = SPELL_INVISIBLE;
      af.duration  = -1;    /* Forever */
      af.modifier  = 0;
      af.location  = APPLY_NONE;
      af.bitvector = AFF_INVISIBLE;
      affect_to_char(ch, &af);

      ch->specials.wizInvis = TRUE;
      send_to_char("You slowly vanish into thin-air.\n\r",ch);
      act("$n utters the words 'inquzky abradyk'.", FALSE, ch, 0, 0, TO_ROOM);
      act("$n slowly vanishes into thin-air.", FALSE, ch, 0, 0, TO_ROOM);
   } /* if */
} /* do_wizinvis */

void do_holylite(struct char_data *ch, char *argument, int cmd)
{
	if (!IS_TRUSTED(ch)) return;

	if (argument[0] != '\0') {
	   send_to_char("HOLYLITE doesn't take any arguments; arg ignored.\n\r",ch);
 	} /* if */

	if (ch->specials.holyLite) {
	   ch->specials.holyLite = FALSE;
	   send_to_char("Holy light mode off.\n\r",ch);
        }
	else {
	   ch->specials.holyLite = TRUE;
	   send_to_char("Holy light mode on.\n\r",ch);
        } /* if */
} /* do_holylite */

void do_notell(struct char_data *ch, char *argument, int cmd)
{
	if (IS_NPC(ch)) return;

	if (argument[0] != '\0') {
	   send_to_char("NOTELL doesn't take any arguments; arg ignored.\n\r",ch);
 	} /* if */

	if (IS_SET(ch->specials.act, PLR_NOTELL)) {
	   REMOVE_BIT(ch->specials.act, PLR_NOTELL);
	   send_to_char("You can now hear tells again.\n\r",ch);
        }
	else {
	   SET_BIT(ch->specials.act, PLR_NOTELL);
	   send_to_char("You will no longer hear tells.\n\r",ch);
        } /* if */
} /* do_notell */


void do_gecho(struct char_data *ch, char *argument, int cmd)
{
	int i;
	static char buf[MAX_STRING_LENGTH];
	struct descriptor_data *pt;


	if (!IS_TRUSTED(ch))
		return;

	for (i = 0; *(argument + i) == ' '; i++);

	if (!*(argument + i))
		send_to_char("That must be a mistake...\n\r", ch);
	else
	{
		sprintf(buf,"%s", argument + i);
                for (pt = descriptor_list; pt; pt = pt->next) {
                   act(buf, FALSE, ch, 0, pt->character, TO_VICT);
                }
		send_to_char("Ok.\n\r", ch);
	}
}

void do_wiznet(struct char_data *ch, char *argument, int cmd)
{
   static char buf[MAX_STRING_LENGTH];
   struct descriptor_data *t;

   if(!IS_TRUSTED(ch)) return;

   for ( ; *argument == ' '; argument++);

   if (!(*argument)) {
      send_to_char("What do you wish to say to the immortals?\n\r",ch);
      return;
   } /* if */

   sprintf(buf, "Hermes delivers the following message to the gods : \n  '%s'\n\r",argument);
   send_to_char(buf, ch);
   sprintf(buf, "Hermes brings you the following message from $n.  It reads\n  '%s'\n\r",argument);

   for (t = descriptor_list; t; t = t->next) {
      if (t->character != ch && !t->connected && IS_TRUSTED(t->character)){
         act(buf, FALSE, ch, 0, t->character, TO_VICT);
      } /* if */
   } /* for */

} /* do_wiznet */

void do_goto(struct char_data *ch, char *argument, int cmd)
{
	char buf[MAX_INPUT_LENGTH];
	char output[256];
	int loc_nr, location, i;
	struct char_data *target_mob, *pers;
	struct obj_data *target_obj;
	extern int top_of_world;


	if (!IS_TRUSTED(ch))
		return;

	one_argument(argument, buf);
	if (!*buf)
	{
		send_to_char("You must supply a room number or a name.\n\r", ch);
		return;
	}


	if (isdigit(*buf))
	{
		loc_nr = atoi(buf);
		for (location = 0; location <= top_of_world; location++)
			if (world[location].number == loc_nr)
				break;
			else if (location == top_of_world)
			{
				send_to_char("No room exists with that number.\n\r", ch);
				return;
			}
	}
	else if (target_mob = get_char_vis(ch, buf))
		location = target_mob->in_room;
	else if (target_obj = get_obj_vis(ch, buf))
		if (target_obj->in_room != NOWHERE)
			location = target_obj->in_room;
		else
		{
			send_to_char("The object is not available.\n\r", ch);
			return;
		}
	else
	{
		send_to_char("No such creature or object around.\n\r", ch);
		return;
	}

	/* a location has been found. */

	if (IS_SET(world[location].room_flags, PRIVATE))
	{
		for (i = 0, pers = world[location].people; pers; pers =
			pers->next_in_room, i++);
		if (i>1 && GET_LEVEL(ch)<LV_GOD )
		{
			send_to_char(
				"There's a private conversation going on in that room.\n\r", ch);
			return;
		}
	}

	strcpy(output, "$n");
	if (ch->specials.poofOut == NULL) {
	   strcat(output, " disappears in a puff of smoke.");
	}
	else {
	   strcat(output, ch->specials.poofOut);
	} /* if */

	act(output, FALSE, ch, 0, 0, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, location);

	strcpy(output, "$n");
	if (ch->specials.poofIn == NULL) {
	   strcat(output, " appears with an ear-splitting bang.");
	}
	else {
	   strcat(output, ch->specials.poofIn);
	} /* if */

	act(output, FALSE, ch, 0,0,TO_ROOM);
	do_look(ch, "",15);
}

void do_force(struct char_data *ch, char *argument, int cmd)
{
  struct descriptor_data *i;
	struct char_data *vict;
	char name[100], to_force[100],buf[100];
        bool local = FALSE;


	if (!IS_TRUSTED(ch))
		return;

	half_chop(argument, name, to_force);

	if (!*name || !*to_force)
		 send_to_char("Who do you wish to force to do what?\n\r", ch);
	else if (str_cmp("all", name) && str_cmp("local", name)) {
		if (!(vict = get_char_vis(ch, name))){
			send_to_char("No-one by that name here..\n\r", ch);
		} else {/*---I added the level thing here..simple &&-randall*/
			if ((GET_LEVEL(ch) < GET_LEVEL(vict)) && (GET_LEVEL(ch) < LV_GOD)) {
				if(GET_LEVEL(vict)!=LV_GOD){
					send_to_char("Oh no you don't!!\n\r", ch);
				}
			} else {
				sprintf(buf, "$n has forced you to '%s'.", to_force);
				act(buf, FALSE, ch, 0, (char *)vict, TO_VICT);
				send_to_char("Ok.\n\r", ch);
				command_interpreter(vict, to_force);
			}
		}
	} else { /* force all/local */
              if (str_cmp("local", name) == 0) {
                 local = TRUE;
              } /* if */
		    for (i = descriptor_list; i; i = i->next)
			if (i->character != ch && !i->connected) {
				vict = i->character;
                                if (local &&
				    (i->character->in_room != ch->in_room)) {
                                   /* not in the room; do nothing */
                                   continue;
                                } /* if */

				if (GET_LEVEL(ch) < GET_LEVEL(vict)){
					if(GET_LEVEL(vict)!=LV_GOD){
						send_to_char("Oh no you don't!!\n\r", ch);
					}
				} else {
					sprintf(buf, "$n has forced you to '%s'.", to_force);
					act(buf, FALSE, ch, 0, (char *)vict, TO_VICT);
					command_interpreter(vict, to_force);
				}
			}
			send_to_char("Ok.\n\r", ch);
	}
}

void do_poofIn(struct char_data *ch, char *argument, int cmd)
{
   char arg[MAX_INPUT_LENGTH];

   if(!IS_TRUSTED(ch)) return;

   one_argument(argument, arg);

   if (ch->specials.poofIn != NULL) {
      free(ch->specials.poofIn);
   } /* if */

   if (!*arg) {
      ch->specials.poofIn = NULL;
   }
   else {
      ch->specials.poofIn = strdup(argument);
   } /* if */
} /* do_poofIn */

void do_poofOut(struct char_data *ch, char *argument, int cmd)
{
   char arg[MAX_INPUT_LENGTH];

   if(!IS_TRUSTED(ch)) return;

   one_argument(argument, arg);

   if (ch->specials.poofOut != NULL) {
      free(ch->specials.poofOut);
   } /* if */

   if (!*arg) {
      ch->specials.poofOut = NULL;
   }
   else {
      ch->specials.poofOut = strdup(argument);
   } /* if */
} /* do_poofOut */

void do_teleport(struct char_data *ch, char *argument, int cmd)
{
   struct descriptor_data *i;
   struct char_data *victim, *target_mob, *pers;
   char person[MAX_INPUT_LENGTH], room[MAX_INPUT_LENGTH];
   sh_int target;
   int loop;
   extern int top_of_world;


   if (!IS_TRUSTED(ch)) return;

   half_chop(argument, person, room);

   if (!*person) {
      send_to_char("Who do you wish to teleport?\n\r", ch);
      return;
   } /* if */

   if (!*room) {
      send_to_char("Where do you wish to send this person?\n\r", ch);
      return;
   } /* if */

   if (!(victim = get_char_vis(ch, person))) {
      send_to_char("No-one by that name around.\n\r", ch);
      return;
   } /* if */

   if (isdigit(*room)) {
      target = atoi(&room[0]);
      for (loop = 0; loop <= top_of_world; loop++) {
         if (world[loop].number == target) {
            target = (sh_int)loop;
            break;
         } else if (loop == top_of_world) {
            send_to_char("No room exists with that number.\n\r", ch);
            return;
         } /* if */
      } /* for */
   } else if (target_mob = get_char_vis(ch, room)) {
      target = target_mob->in_room;
   } else {
      send_to_char("No such target (person) can be found.\n\r", ch);
      return;
   } /* if */

   if (IS_SET(world[target].room_flags, PRIVATE)) {
      for (loop = 0, pers = world[target].people; pers;
           pers = pers->next_in_room, loop++);
      if (loop > 1) {
          send_to_char(
            "There's a private conversation going on in that room\n\r",
            ch);
         return;
      } /* if */
   } /* if */

   act("$n disappears in a puff of smoke.",FALSE,victim, 0,0, TO_ROOM);
   char_from_room(victim);
   char_to_room(victim, target);
   act("$n arrives from a puff of smoke.", FALSE, victim, 0, 0, TO_ROOM);
   act("$n has teleported you!", FALSE, ch, 0, (char *)victim, TO_VICT);
   do_look(victim, "", 15);
   send_to_char("Teleport completed.\n\r", ch);

} /* do_teleport */

void
swap_class(struct char_data *ch, struct char_data *vict, int newclass)
{
	char oldclass[80];
	char sendbuf[80];
	char buf[80];

	sprintf(sendbuf,"Orig class is %d, requested is %d\n\r",
		GET_CLASS(vict),newclass);
	send_to_char(sendbuf,ch);
	if(GET_CLASS(vict)==newclass){
		send_to_char("He/she's already that class!\n\r",ch);
		return;
	}
	switch(GET_CLASS(vict)){
	  case CLASS_THIEF:
		SET_BIT(vict->specials.act,PLR_ISMULTITH);
		strcpy(oldclass,"Thief"); break;
	  case CLASS_MAGIC_USER:
		SET_BIT(vict->specials.act,PLR_ISMULTIMU);
		strcpy(oldclass,"Magic User"); break;
	  case CLASS_CLERIC:
		SET_BIT(vict->specials.act,PLR_ISMULTICL);
		strcpy(oldclass,"Cleric"); break;
	  case CLASS_WARRIOR:
		SET_BIT(vict->specials.act,PLR_ISMULTIWA);
		strcpy(oldclass,"Warrior"); break;
	  default:
		send_to_char("Invalid class!\n\r",ch);
		return;
	}
	GET_CLASS(vict)=newclass;
	switch(GET_CLASS(vict)){
	  case CLASS_WARRIOR:
		sprintf(buf,"Warrior/%s",oldclass);break;
	  case CLASS_THIEF:
		sprintf(buf,"Thief/%s",oldclass);break;
	  case CLASS_CLERIC:
		sprintf(buf,"Cleric/%s",oldclass);break;
	  case CLASS_MAGIC_USER:
		sprintf(buf,"Magic User/%s",oldclass);break;
	  default:
		send_to_char("Serious error in new class!\n\r",ch);
		return;
	}
	sprintf(sendbuf,"You are now a %s!\n\r",buf);
	send_to_char(sendbuf,vict);
	sprintf(sendbuf,"You changed %s to a %s!\n\r",GET_NAME(vict),buf);
	send_to_char(sendbuf,ch);
	sprintf(sendbuf,"New class is %d\n\r",GET_CLASS(ch));
	send_to_char(sendbuf,ch);
}
void do_setstat(struct char_data *ch, char *argument, int cmd)
{
   struct char_data *vict;
   char name[100],buf[MAX_INPUT_LENGTH],class[100],num[100];
   char buf2[MAX_INPUT_LENGTH];
   char setstat_info[MAX_INPUT_LENGTH];
   char logit[MAX_STRING_LENGTH];
   char pwd[100];
   int value;
   int i;


   half_chop(argument, name, buf);
   half_chop(buf, class, buf2);
   argument_interpreter(buf2, num, pwd);

   if (str_cmp(pwd, "xyzzy") != 0) {
      send_to_char("You try, but can't!!!\n\r", ch);
      send_to_char("Syntax : setstat <victim> <field> <value> <passwd>\n\r",ch);
      sprintf(setstat_info,"%s%s%s%s%s",
	"Field can be: alignment, exp, hp, mp, mana, gold, str, add, dex, \n\r",
	" con, int, wis, classmu, classcleric, classthief, classwarrior,\n\r",
	" clearmulticlass, multith, multicl, multimu, multiwa, blue, red,\n\r",
	" notplaying, killer, thief, trusted, untrusted, mortal.\n\r",
	"NOTE: All setstats are logged.\n\r");
      send_to_char(setstat_info,ch);
      return;
   } /* if */

   if((GET_LEVEL(ch)!=LV_GOD)&&!IS_TRUSTED(ch)) return;

   sprintf(logit,"WIZINFO: (%s) setstat %s",ch->player.name,argument);
   log(logit);

   if (!*name || !*buf || !*class || !*num || !isdigit(*num) ) {
      send_to_char("Syntax : setstat <victim> <field> <value> <passwd>\n\r",ch);
      send_to_char("Type setstat alone to see all your options.\n\r",ch);
      return;
   }

   if (!(vict = get_char_vis(ch, name))) {
      send_to_char("No one here by that name...\n\r",ch);
      return;
   }

   if (((GET_LEVEL(vict) >= LV_GOD) && !IS_NPC(vict)) && (ch != vict)) {
      send_to_char("Oh no you don't!!!\n\r", ch);
      return;
   }
   if(GET_LEVEL(ch)<GET_LEVEL(vict)){
      send_to_char("No can do to a higher level.\n\r",ch);
      return;
   }

   for (i = 0; i < strlen(num); i++) {
     if (!isdigit(*num)) {
        send_to_char("Value is not a valid number.\n\r", ch);
        return;
     }
   }

   if ((value = atoi(num)) < 0) {
      if (str_cmp(class, "alignment") != 0) {
         send_to_char("Only alignment can be negative.\n\r", ch);
         return;
      }
   }

   if (str_cmp(class, "exp") == 0)  {
      if (value >= 0 && value <= 2000000) {
         vict->points.exp = value; return;
	 return;
      } else if (value >=0 && (GET_LEVEL(ch)>=LV_GOD)){
		vict->points.exp = value;
		return;
	} else {
         send_to_char("Experience must be >= 0 and <= 2000000.", ch);
         return;
      }
   }
   if (str_cmp(class, "hp")  == 0)  {
      vict->points.max_hit  = value; return;
   }
   if (str_cmp(class, "mp")  == 0) {
      vict->points.max_move  = value; return;
   }
   if (str_cmp(class, "mana") == 0) {
      vict->points.max_mana = value; return;
   }
   if (str_cmp(class, "gold") == 0) {
      vict->points.gold = value; return;
   }
   if (str_cmp(class, "alignment") == 0) {
      if (value >= -1000 && value <= 1000) {
         vict->specials.alignment = value;
	 return;
      }
      else {
         send_to_char("Alignment must be >= -1000 and <= 1000.", ch);
         return;
      }
   }
   if (str_cmp(class, "str") == 0)  {
      if (value < 3 || value > 18) {
         send_to_char("Strength must be >= 3, <= 18.\n\r", ch);
         return;
      }
      vict->abilities.str = value;
      vict->tmpabilities.str = value;
      return;
   }
   if (str_cmp(class, "add")  == 0)  {
      if (value < 0 || value > 100) {
         send_to_char("Strength add must be >= 0, <= 100.\n\r", ch);
         return;
      }
      vict->abilities.str_add = value;
      vict->tmpabilities.str_add = value;
      return;
   }
   if (str_cmp(class, "dex")  == 0) {
      if (value < 3 || value > 18) {
         send_to_char("Dexterity must be >= 3, <= 18.\n\r", ch);
         return;
      }
      vict->abilities.dex = value;
      vict->tmpabilities.dex = value;
      return;
   }
   if (str_cmp(class, "con") == 0) {
      if (value < 3 || value > 18) {
         send_to_char("Mana must be >= 3, <= 18.\n\r", ch);
         return;
      }
      vict->abilities.con = value;
      vict->tmpabilities.con = value;
      return;
   }
   if (str_cmp(class, "int") == 0) {
      if (value < 3 || value > 18) {
         send_to_char("Intelligence must be >= 3, <= 18.\n\r", ch);
         return;
      }
      vict->abilities.intel = value;
      vict->tmpabilities.intel = value;
      return;
   }
   if (str_cmp(class, "wis") == 0) {
      if (value < 3 || value > 18) {
         send_to_char("Wisdom must be >= 3, <= 18.\n\r", ch);
         return;
      }
      vict->abilities.wis = value;
      vict->tmpabilities.wis = value;
      return;
   }

	if(str_cmp(class,"classmu")==0){
		GET_CLASS(vict)=CLASS_MAGIC_USER;
		send_to_char("Victim's new class is MU.\n\r",ch);
		return;
	}
	if(str_cmp(class,"classcleric")==0){
		send_to_char("Victim's new class is Cleric.\n\r",ch);
		GET_CLASS(vict)=CLASS_CLERIC;
		return;
	}
	if(str_cmp(class,"classthief")==0){
		send_to_char("Victim's new class is Thief.\n\r",ch);
		GET_CLASS(vict)=CLASS_THIEF;
		return;
	}
	if(str_cmp(class,"classwarrior")==0){
		send_to_char("Victim's new class is Warrior.\n\r",ch);
		GET_CLASS(vict)=CLASS_WARRIOR;
		return;
	}
	if (str_cmp(class,"clearmulticlass")==0) {
		if(IS_SET(vict->specials.act,PLR_ISMULTITH)){
			REMOVE_BIT(vict->specials.act,PLR_ISMULTITH);
			send_to_char("MultiTH flag gone\n\r",ch);
		}
		if(IS_SET(vict->specials.act,PLR_ISMULTIWA)){
			REMOVE_BIT(vict->specials.act,PLR_ISMULTIWA);
			send_to_char("MultiWA flag gone\n\r",ch);
		}
		if(IS_SET(vict->specials.act,PLR_ISMULTIMU)){
			REMOVE_BIT(vict->specials.act,PLR_ISMULTIMU);
			send_to_char("MultiMU flag gone\n\r",ch);
		}
		if(IS_SET(vict->specials.act,PLR_ISMULTICL)){
			REMOVE_BIT(vict->specials.act,PLR_ISMULTICL);
			send_to_char("MultiCL flag gone\n\r",ch);
		}
		send_to_char("Done.\n\r",ch);
		return;
	}
	if (str_cmp(class,"multith") == 0) {
		swap_class(ch,vict,CLASS_THIEF);
		return;
	}
	if(str_cmp(class,"multicl")==0) {
		swap_class(ch,vict,CLASS_CLERIC);
		return;
	}
	if(str_cmp(class,"multimu")==0){
		swap_class(ch,vict,CLASS_MAGIC_USER);
		return;
	}
	if(str_cmp(class,"multiwa")==0){
		swap_class(ch,vict,CLASS_WARRIOR);
		return;
	}
	if(str_cmp(class,"blue")==0){
		vict->specials.arena=ARENA_BLUE_PLR;
		send_to_char("The victim is now a member of the BLUE team!\n\r",ch);
		send_to_char("**You are now a member of the BLUE team!\n\r",vict);
		return;
	}
	if(str_cmp(class,"red")==0){
		vict->specials.arena=ARENA_RED_PLR;
		send_to_char("The victim is now a member of the RED team!\n\r",ch);
		send_to_char("**You are now a member of the RED team!\n\r",vict);
		return;
	}
	if(str_cmp(class,"notplaying")==0){
		vict->specials.arena=ARENA_NOTPLAYING;
		send_to_char("You cleared the victim's arena flag.\n\r",ch);
		send_to_char("**You do not belong to either Arena team now!\n\r",vict);
		return;
	}
	if(str_cmp(class,"killer")==0){
		SET_BIT(vict->specials.act,PLR_ISKILLER);
		send_to_char("The victim is now a killer!\n\r",ch);
		return;
	}
	if(str_cmp(class,"thief")==0){
		SET_BIT(vict->specials.act,PLR_ISTHIEF);
		send_to_char("The victim is now a thief!\n\r",ch);
		return;
	}
	if(str_cmp(class,"log_all")==0){
		if(GET_LEVEL(ch)!=LV_GOD){
			send_to_char("Only Implementors can do this.\n\r",ch);
			return;
		}
		if(log_all){
			log_all=0;
			send_to_char("Logging turned off.\n\r",ch);
		} else {
			log_all=1;
			send_to_char("Logging is ON.\n\r",ch);
		}
		return;
	}
	if(str_cmp(class,"trusted")==0){
		SET_BIT(vict->specials.act,PLR_ISTRUSTED);
		send_to_char("++++ You are now a trusted player ++++\n\r",vict);
		send_to_char("Done.\n\r",ch);
		return;
	}
	if(str_cmp(class,"untrusted")==0){
		REMOVE_BIT(vict->specials.act,PLR_ISTRUSTED);
		send_to_char("--- You are no longer a trusted player\n\r",vict);
		send_to_char("Done.\n\r",ch);
		return;
	}
	if(str_cmp(class,"mortal")==0){
		for(i=0;i<3;i++){
			vict->specials.conditions[i]=0;
		}
		send_to_char("Hunger, thirst and drunk reset to mortal levels.\n\r",ch);
		return;
	}
   send_to_char ("Invalid field specified.\n\r", ch);
   return;

} /* do_setstat */

void do_wizlock(struct char_data *ch, char *argument, int cmd)
{
   char arg[MAX_INPUT_LENGTH];
   int value;

   if(!IS_TRUSTED(ch)) return;

   one_argument(argument, arg);
   if (!*arg) {
      send_to_char("You must supply an option value.\n\r", ch);
      return;
   }

   value = atoi(arg);

   switch(value) {
      case 0 :
         send_to_char("The game is now open to new players.\n\r", ch);
         restrict = 0;
         break;
      case 1 :
         send_to_char("The game is now closed to new players.\n\r", ch);
         restrict = 1;
         break;
      default :
         send_to_char("Invalid wizlock option...", ch);
         break;
   } /* switch */

} /* do_wizlock */

void
do_ban(struct char_data *ch, char *argument, int cmd)
{
	char name[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	struct ban_t *tmp;
	int count;

	if(!IS_TRUSTED(ch)) return;

	one_argument(argument,name);

	if(!*name){
		/* list the sites */
		send_to_char("------------\n\rBanned sites\n\r------------\n\r",ch);
		if(ban_list==(struct ban_t*)NULL){
			send_to_char("Empty list!\n\r",ch);
			return;
		}
		for(count=0,tmp=ban_list; tmp; count++,tmp=tmp->next){
			sprintf(buf,"%s\n\r",tmp->name);
			send_to_char(buf,ch);
		}
		sprintf(buf,"\n\nThere are %d banned sites.\n\r",count);
		send_to_char(buf,ch);
		return;
	}
	for(tmp=ban_list; tmp; tmp=tmp->next){
		if(str_cmp(name,tmp->name)==0){
			send_to_char("That site is already banned!\n\r",ch);
			return;
		}
	}
	CREATE(tmp,struct ban_t,1);
	CREATE(tmp->name,char,strlen(name)+1);
	strcpy(tmp->name,name);
	tmp->next=ban_list;
	ban_list=tmp;
}

void
do_allow(struct char_data *ch, char *argument, int cmd)
{
	char name[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	struct ban_t *curr, *prev;

	if(!IS_TRUSTED(ch)) return;

	one_argument(argument,name);

	if(!*name){
		send_to_char("Remove which site from the ban list?\n\r",ch);
		return;
	}
	curr=prev=ban_list;
	if(str_cmp(curr->name,name)==0){
		ban_list=ban_list->next;
		free(curr->name);
		free(curr);
		send_to_char("Ok.\n\r",ch);
		return;
	}
	curr=curr->next;
	while(curr){
		if(!strcmp(curr->name,name)){
			if(curr->next){
				prev->next=curr->next;
				free(curr->name);
				free(curr);
				send_to_char("Ok.\n\r",ch);
				return;
			}
			prev->next=(struct ban_t*)NULL;
			free(curr->name);
			free(curr);
			send_to_char("Ok.\n\r",ch);
			return;
		}
		curr=curr->next;
		prev=prev->next;
	}
	send_to_char("Site not found in list!\n\r",ch);
}
