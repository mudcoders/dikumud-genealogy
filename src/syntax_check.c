/**************************************************************************
*  file: syntax_check.c , Check syntax of all files       Part of DIKUMUD *
*  Usage: syntax_check <file prefix> <start room>                         *
*  Heavily modified by Benedict (bls@sector7g.eng.sun.com)                *
*  compile with -DOLD_DIKU if you're using the original diku file format  *
*  compile with -DNO_HEADERS if you don't have structs.h handy            *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef NO_HEADERS
typedef char bool;
#define MAX_OBJ_AFFECT 2
#define MAX_STRING_LENGTH 4096
#define MAX_WEAR 18
#else
#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "db.h"
#endif

#define FALSE 0
#define TRUE !FALSE

#define CREATE(result, type, number)  do {\
        if (!((result) = (type *) calloc ((number), sizeof(type))))\
                { perror("malloc failure"); abort(); } } while(0)

#define RECREATE(result,type,number) do {\
  if (!((result) = (type *) realloc ((result), sizeof(type) * (number))))\
		{ perror("realloc failure"); abort(); } } while(0)

FILE *mob_f, *obj_f, *wld_f, *zon_f;

struct edge {
  bool exists;
  int dest;
};

struct room_graph {
  int vnum;
#ifndef OLD_DIKU
  int zone;
#endif
  struct edge way_out[6];
  bool flag;
  bool processed;
};

struct part_index_data {
  int virtual;
  /* don't need all the other fields */
};

#ifndef OLD_DIKU
struct part_zone_data {
  int number;
  int teleport_target;
};

struct part_zone_data *zones;
int top_of_zont = 0;
#endif

struct room_graph *world;

struct part_index_data *mob_index;
struct part_index_data *obj_index;
struct part_index_data *wld_index;

int top_of_mobt = 0;
int top_of_objt = 0;
int top_of_wldt = 0;
int firstroom;

/* local procedures */
void setup_dir(FILE *fl, int room, int dir, int rnum);
struct part_index_data *generate_indices(FILE *fl, int *top);
char *fread_string(FILE *);

void assume(int faktisk, int nread, char *pname, int place, char *errmsg)
{
  if (nread != faktisk) {
    printf("Fatal error (%s %d): %s\n", pname, place, errmsg);
    exit(1);
  }
}


/* generate index table for object, monster or world file*/
struct part_index_data *generate_indices(FILE *fl, int *top)
{
  int i = 0, nread;
  struct part_index_data *index;
  char buf[82];

  rewind(fl);

  for (;;) {
    if (fgets(buf, 81, fl)) {
      if (*buf == '#') {
        if (i == 0)
          CREATE(index, struct part_index_data, 1);
        else if ((index = (struct part_index_data*) realloc(index,
                 (i + 1) * sizeof(struct part_index_data))) == 0) {
          printf("out of memory creating indices");
          exit(1);
        }
        nread = sscanf(buf, "#%d", &index[i].virtual);
        assume(nread, 1, "room/mob/obj", index[i].virtual, "Next string with E/A/$");
        i++;
      } else if (*buf == '$')  /* EOF */
        break;
    } else {
      printf("Error generating index, probable missing EOF marker.");
      exit(1);
    }
  }
  *top = i - 1;
  return(index);
}

int exist_index(char *type, struct part_index_data *index_list,
                int top, int num, char *pname, int place)

{
  int i, found;

  for (found = FALSE, i = 0; i <= top && !found; i++)
    if (index_list[i].virtual == num)
      found = TRUE;

  if (!found)
    printf("Lack of existence of %s #%d (%s %d)\n", type, num, pname, place);

  return (found);
}


int rnum_from_vnum(struct part_index_data *p, int last, int vnum)
{
  int bot = 0;
  int top = last;
  int current;

  for (;;) {
    current = (top+bot)/2;
    if (p[current].virtual == vnum) return current;
    else if (bot >= top) return -1;
    else if (p[current].virtual > vnum) top = current - 1;
    else                               bot = current + 1;
  }
}



void
init_node(int room_nr, int vnum)
{
  int i;

  for (i = 0; i < 6; i++)
    world[room_nr].way_out[i].exists = FALSE;
  world[room_nr].vnum = vnum;
}


void check_world(FILE *fl)
{
  int room_nr = 0, zone = 0, dir_nr, virtual_nr, flag, tmp, old_virtual;
  char *temp, chk[50];
  struct extra_descr_data *new_descr;
  int nread;
  char *temp2;

  rewind(fl);

  old_virtual = -1;

  do {
    nread = fscanf(fl, " #%d\n", &virtual_nr);
    assume(nread, 1, "room", virtual_nr, "Can't read room number");
    init_node(room_nr, virtual_nr);

    assume(old_virtual < virtual_nr, TRUE, "room", virtual_nr, "Rooms out of order");
    old_virtual = virtual_nr;

    temp = fread_string(fl);
    if (flag = (*temp != '$')) { /* a new record to be read */
      temp2 = fread_string(fl);

      nread = fscanf(fl, " %d ", &tmp);
      assume(nread, 1, "room", virtual_nr, "Can't read zone number");
      assume(tmp >= 0, TRUE, "room", virtual_nr, "negative zone number");
#ifndef OLD_DIKU
      world[room_nr].zone = tmp;
#endif

#ifndef OLD_DIKU
      nread = fscanf(fl, " %x ", &tmp);
#else
      nread = fscanf(fl, " %d ", &tmp);
#endif
      assume(nread, 1, "room", virtual_nr, "Can't read room flags");

      nread = fscanf(fl, " %d ", &tmp);
      assume(nread, 1, "room", virtual_nr, "Can't read sector type");
      assume(tmp >= 0 && tmp <= 9, TRUE, "room", virtual_nr, "Invalid sector type");

      for (;;) {
        nread = fscanf(fl, " %s \n", chk);
        assume(nread, 1, "room", virtual_nr, "Can't read D/E/S string");

        if (*chk == 'D') { /* direction field */
          setup_dir(fl, virtual_nr, atoi(chk + 1), room_nr);
        } else if (*chk == 'E') { /* extra description field */
          temp2 = fread_string(fl); /* Description */
          temp2 = fread_string(fl); /* Keywords    */
        } else if (*chk == 'S') { /* end of current room */
          break;
        } else {
          assume(FALSE, TRUE, "room", virtual_nr, "MISSING D/E or S");
        }
      }
    }
    room_nr++;
  } while (flag);
}




/* read direction data */
void setup_dir(FILE *fl, int room, int dir, int rnum)
{
  int dest, key, nread;
  char *temp;

  temp = fread_string(fl);
  temp = fread_string(fl);

  nread = fscanf(fl, " %d ", &key);
  assume(nread, 1, "room", room, "Can't read door flag");
  if (key < 0 || key > 2)
    printf("Closability flag is wrong (room %d, direction %d)\n", room, dir);
  nread = fscanf(fl, " %d ", &key);
  assume(nread,1, "room", room, "Key number for exit");
  if (key > 0 &&
      !exist_index("key for exit, key", obj_index, top_of_objt, key, "room", room))
    printf("That's exit %d\n", dir);
  nread = fscanf(fl, " %d ", &dest);
  assume(nread,1, "room", room, "Destination room for exit");
  if (dest != -1)
    exist_index("exit in world", wld_index, top_of_wldt, dest, "room", room);
  if (dir < 0 || dir > 5) {
    printf("Illegal direction number (%d) for exit from room %d.\n", dir, room);
  } else if (world[rnum].way_out[dir].exists) {
    printf("Room %d has more than one exit in direction %d.\n", room, dir);
  } else if (dest == -1) {
    ; /* deliberate (we assume) exit to room -1 */
  } else {
    world[rnum].way_out[dir].exists = TRUE;
    world[rnum].way_out[dir].dest = dest;
  }
}


/* load the zone table and command tables */
void check_zones(FILE *fl)
{
  int line_no;
  int nread, tmp1, tmp2, tmp3, tmp4;
  int bottom;
  int zon = 0, cmd_no = 0, ch;
  int prob;
  char *check, buf[81];
  char cmd_type;
  int index;

  rewind(fl);
  line_no = 1;

  for (;;) {
    nread = fscanf(fl, " #%d\n", &bottom);
    assume(nread, 1, "zone file line", line_no++, "Zone number not found");

    check = fread_string(fl);
    line_no++;

    if (*check == '$')
      break;    /* end of file */

    /* alloc a new zone */

    nread = fscanf(fl, " %d ", &zon);
    assume(nread, 1, "zone file line", line_no, "Can't read last room in zone");

    nread = fscanf(fl, " %d ", &zon);
    assume(nread, 1, "zone file line", line_no, "Can't read life span");

    nread = fscanf(fl, " %d ", &zon);
    assume(nread, 1, "zone file line", line_no, "Can't read reset mode");
/*
#ifndef OLD_DIKU
    nread = fscanf(fl, " %d ", &zon);
    assume(nread, 1, "zone file line", line_no, "Can't read teleport target");
    if (zon != -1)
      exist_index("teleport target, room", wld_index, top_of_wldt, zon,
                  "zone line number", line_no);
    if (zones == 0)
      CREATE(zones, struct part_zone_data, 1);
    else
      RECREATE(zones, struct part_zone_data, top_of_zont+1);
    zones[top_of_zont].number = bottom;
    zones[top_of_zont].teleport_target = rnum_from_vnum(wld_index, top_of_wldt, zon);
    top_of_zont++;
    nread = fscanf(fl, " %x ", &zon);
    assume(nread, 1, "zone file line", line_no, "Can't read zone flags");
#endif
*/
    line_no++;

    /* read the command table */

    cmd_no = 0;

    for (;;) {
      fscanf(fl, " "); /* skip blanks */
      nread = fscanf(fl, "%c", &cmd_type);
      assume(nread, 1, "zone file line", line_no, "Command type M/*/O/G/E/S missing");

      if (cmd_type == 'S') {
        line_no++;
        break;
      }

      if (cmd_type == '*') {
        fgets(buf, 80, fl); /* skip command */
        line_no++;
        continue;
      }
/*
#ifndef OLD_DIKU
      nread = fscanf(fl, " %d", &prob);
      assume(nread, 1, "zone file line", line_no, "Can't read probability");
      if (prob < 0 || prob > 100) {
        printf("Warning: zone file line %d: Probability out of range on %c command",
                line_no, cmd_type);
      }
#endif
*/
      nread = fscanf(fl, " %d %d %d", &tmp1, &tmp2, &tmp3);
      if (nread != 3) {
        sprintf(buf, "Three numbers after %c command missing -- we have %d %d %d",
                cmd_type, tmp1, tmp2, tmp3);
        assume(TRUE, FALSE, "zone file line", line_no, buf);
      }

      if (cmd_type == 'M' || cmd_type == 'O' ||
          cmd_type == 'D' || cmd_type == 'P' ||
          cmd_type == 'E'
#ifndef OLD_DIKU
          || cmd_type == 'R'
#endif
        ) {
        nread = fscanf(fl, " %d", &tmp4);
        if (nread != 1) {
          sprintf(buf, "Fourth number after %c command missing -- we have %d %d %d",
                  cmd_type, tmp1, tmp2, tmp3);
          assume(TRUE, FALSE, "zone file line", line_no, buf);
        }
      }

      switch (cmd_type) {
      case 'M':
        exist_index("M mob", mob_index, top_of_mobt, tmp2, "zone line", line_no);
        exist_index("M wld", wld_index, top_of_wldt, tmp4, "zone line", line_no);
        if (tmp3 <= 0) printf("non-positive limit on %d", line_no);
        break;
      case 'O':
        exist_index("O obj", obj_index, top_of_objt, tmp2, "zone line", line_no);
        exist_index("O wld", wld_index, top_of_wldt, tmp4, "zone line", line_no);
        if (tmp3 <= 0) printf("non-positive limit on %d", line_no);
        break;
      case 'G':
        exist_index("G obj", obj_index, top_of_objt, tmp2, "zone line", line_no);
        if (tmp3 <= 0) printf("non-positive limit on %d", line_no);
        break;
      case 'E':
        exist_index("E obj", obj_index, top_of_objt, tmp2, "zone line", line_no);
        if (tmp4 < 0 || tmp4 >= MAX_WEAR)
          printf("Error line %d: illegal wear position on E command", line_no);
        if (tmp3 <= 0) printf("non-positive limit on %d", line_no);
        break;
      case 'P':
        exist_index("P obj", obj_index, top_of_objt, tmp2, "zone line", line_no);
        exist_index("P obj", obj_index, top_of_objt, tmp4, "zone line", line_no);
        if (tmp3 <= 0) printf("non-positive limit on %d", line_no);
        break;
      case 'D':
        exist_index("D wld", wld_index, top_of_wldt, tmp2, "zone line", line_no);
        if ( (index = rnum_from_vnum(wld_index, top_of_wldt, tmp2)) == -1)
          printf("No room #%d -- zone trouble\n", tmp2);
        else if (tmp3 < 0 || tmp3 > 5)
          printf("Illegal direction (%d) for D command (line %d)\n", tmp3, line_no);
        else if (!world[index].way_out[tmp3].exists)
          printf("D cmd: no exit %d from room %d (line %d)\n", tmp3, tmp2, line_no);
        break;
#ifndef OLD_DIKU
      case 'R':
        exist_index("R obj", obj_index, top_of_objt, tmp2, "zone line", line_no);
        exist_index("R wld", wld_index, top_of_wldt, tmp4, "zone line", line_no);
        break;
#endif
      case '*':
        break;
      default:
        printf("Error: zone file line %d: illegal command %c", line_no, cmd_type);
        break;
      }

      fgets(buf, 80, fl);  /* read comment */
      line_no++;
    }
  }
}

void check_mobile(FILE *fl)
{
  int virtual_nr, old_virtual, nread, flag;
  char *temp;
  char bogst;

  int i, skill_nr;
  long tmp, tmp2, tmp3;
  struct char_data *mob;
  char buf[100];
  char chk[10];


  old_virtual = -1;

  rewind(fl);

  do {
    nread = fscanf(fl, " #%d\n", &virtual_nr);
    assume(nread,1, "mobile", virtual_nr, "Reading #xxx");

    if (old_virtual >= virtual_nr)
      printf("Mobs not in order around %d.\n", virtual_nr);

    old_virtual = virtual_nr;

    temp = fread_string(fl);  /* Namelist */
    if (flag = (*temp != '$')) {  /* a new record to be read */

      /***** String data *** */
      /* Name already read mob->player.name = fread_string(fl); */
      temp = fread_string(fl);  /* short description  */
      temp = fread_string(fl);  /*long_description    */
      temp = fread_string(fl);  /* player.description */

      /* *** Numeric data *** */

#ifndef OLD_DIKU
      nread = fscanf(fl, "%s ", buf);
      assume(nread, 1, "mobile", virtual_nr, "ACT error");
      for (i = 0; buf[i]; i++)
        if ( (buf[i] >= '0' && buf[i] <= '9') ||
             (buf[i] >= 'a' && buf[i] <= 'z') ||
             (buf[i] >= 'A' && buf[i] <= 'Z') )
          ;
        else
         printf("Error in affected_by: mob %d (bad digit \"%c\")\n",
                virtual_nr, buf[i]);
      nread = fscanf(fl, "%s", buf);
      assume(nread, 1, "mobile", virtual_nr, "affected_by error");
      for (i = 0; buf[i]; i++)
        if ( (buf[i] >= '0' && buf[i] <= '9') ||
             (buf[i] >= 'a' && buf[i] <= 'z') ||
             (buf[i] >= 'A' && buf[i] <= 'Z') )
          ;
        else
         printf("Error in affected_by: mob %d (bad digit \"%c\")\n",
                virtual_nr, buf[i]);
#else
      nread = fscanf(fl, "%d ", &tmp);
      assume(nread, 1, "mobile", virtual_nr, "ACT error");

      nread = fscanf(fl, "%d", &tmp);
      assume(nread, 1, "mobile", virtual_nr, "affected_by error");
#endif

      nread = fscanf(fl, " %d ", &tmp);
      assume(nread, 1, "mobile", virtual_nr, "Monster Alignment Error");
      if (tmp < -1000 || tmp > 1000) printf("Alignment out of range, mob %d\n", virtual_nr);

      nread = fscanf(fl, " %c \n", &bogst);
      assume(nread, 1, "mobile", virtual_nr, "Simple/Detailed error");

      if (bogst == 'S') {
        nread = fscanf(fl, " %d ", &tmp);
        assume(nread, 1, "mobile", virtual_nr, "Level error");
        if (tmp < 0 || tmp > 35) printf("level out of range, mob %d\n", virtual_nr);

        nread = fscanf(fl, " %d ", &tmp);
        assume(nread, 1, "mobile", virtual_nr, "THAC0 error");

        nread = fscanf(fl, " %d ", &tmp);
        assume(nread, 1, "mobile", virtual_nr, "AC error");

        nread = fscanf(fl, " %dd%d+%d ", &tmp, &tmp2, &tmp3);
        assume(nread, 3, "mobile", virtual_nr, "Hitpoints");
        if (tmp2 <= 0) printf("Hitpoints dice is non-positive, mob %d\n", virtual_nr);

        nread = fscanf(fl, " %dd%d+%d \n", &tmp, &tmp2, &tmp3);
        assume(nread, 3, "mobile", virtual_nr, "Damage error");
        if (tmp2 <= 0) printf("Damage dice is non-positive, mob %d\n", virtual_nr);

        nread = fscanf(fl, " %d ", &tmp);
        assume(nread, 1, "mobile", virtual_nr, "GOLD error");
        if (tmp < 0) printf("Negative gold, mob %d\n", virtual_nr);

        nread = fscanf(fl, " %d \n", &tmp);
        assume(nread, 1, "mobile", virtual_nr, "XP error");
        if (tmp < 0) printf("Negative XP, mob %d\n", virtual_nr);

        nread = fscanf(fl, " %d ", &tmp);
        assume(nread, 1, "mobile", virtual_nr, "POSITION error");
        if (tmp < 4 || tmp > 8 || tmp == 7) printf("Bad position, mob %d\n", virtual_nr);

        nread = fscanf(fl, " %d ", &tmp);
        assume(nread, 1, "mobile", virtual_nr, "DEFAULT POS error");
        if (tmp < 4 || tmp > 8 || tmp == 7) printf("Bad position, mob %d\n", virtual_nr);

        nread = fscanf(fl, " %d \n", &tmp);
        assume(nread, 1, "mobile", virtual_nr, "SEXY error");
        if (tmp < 0 || tmp > 2) printf("Bad sex, mob %d\n", virtual_nr);
      } else {  /* The old monsters are down below here */

        printf("Detailed monsters can't be syntax-checked (yet).\n");
        assume(0, 1, "mobile", virtual_nr, "DETAIL ERROR");

        exit(1);
        /*   ***************************
      fscanf(fl, " %d ", &tmp);
      mob->abilities.str = tmp;

      fscanf(fl, " %d ", &tmp);
      mob->abilities.intel = tmp;

      fscanf(fl, " %d ", &tmp);
      mob->abilities.wis = tmp;

      fscanf(fl, " %d ", &tmp);
      mob->abilities.dex = tmp;

      fscanf(fl, " %d \n", &tmp);
      mob->abilities.con = tmp;

      fscanf(fl, " %d ", &tmp);
      fscanf(fl, " %d ", &tmp2);

      mob->points.max_hit = 0;
      mob->points.hit = mob->points.max_hit;

      fscanf(fl, " %d ", &tmp);
      mob->points.armor = tmp;

      fscanf(fl, " %d ", &tmp);
      mob->points.mana = tmp;
      mob->points.max_mana = tmp;

      fscanf(fl, " %d ", &tmp);
      mob->points.move = tmp;
      mob->points.max_move = tmp;

      fscanf(fl, " %d ", &tmp);
      mob->points.gold = tmp;

      fscanf(fl, " %d \n", &tmp);
      GET_EXP(mob) = tmp;

      fscanf(fl, " %d ", &tmp);
      mob->specials.position = tmp;

      fscanf(fl, " %d ", &tmp);
      mob->specials.default_pos = tmp;

      fscanf(fl, " %d ", &tmp);
      mob->player.sex = tmp;

      fscanf(fl, " %d ", &tmp);
      mob->player.class = tmp;

      fscanf(fl, " %d ", &tmp);
      GET_LEVEL(mob) = tmp;

      fscanf(fl, " %d ", &tmp);

      fscanf(fl, " %d ", &tmp);
      mob->player.weight = tmp;

      fscanf(fl, " %d \n", &tmp);
      mob->player.height = tmp;

      for (i = 0; i < 3; i++)
      {
        fscanf(fl, " %d ", &tmp);
        GET_COND(mob, i) = tmp;
      }
      fscanf(fl, " \n ");

      for (i = 0; i < 5; i++)
      {
        fscanf(fl, " %d ", &tmp);
        mob->specials.apply_saving_throw[i] = tmp;
      }

      fscanf(fl, " \n ");
      mob->points.damroll = 0;
      mob->specials.damnodice = 1;
      mob->specials.damsizedice = 6;

      mob->points.hitroll = 0;
      ************************************* */
      }

    }
  }  while (flag);
}


/* read an object from OBJ_FILE */
void check_objects(FILE *fl)
{
  int virtual_nr, old_virtual, nread, flag;
  char *temp;
  struct obj_data *obj;
  int tmp, tmp2, i;
  char chk[256];
  char buf[100];
  struct extra_descr_data *new_descr;

  old_virtual = -1;

  rewind(fl);

  nread = fscanf(fl, " %s \n", chk);
  assume(nread, 1, "object", virtual_nr, "First #xxx number");

  do {
    nread = sscanf(chk, " #%d\n", &virtual_nr);
    assume(nread,1, "object", virtual_nr, "Reading #xxx");

    if (old_virtual >= virtual_nr)
      printf("Objects not in order around %d.\n", virtual_nr);

    old_virtual = virtual_nr;

    temp = fread_string(fl);  /* Namelist */
    if (flag = (*temp != '$')) {  /* a new record to be read */

      /* *** string data *** */

      /* temp = fread_string(fl);  name has been read above */
      temp = fread_string(fl); /* short */
      temp = fread_string(fl); /* descr */
      temp = fread_string(fl); /* action */

      /* *** numeric data *** */

      nread = fscanf(fl, " %d ", &tmp);
      assume(nread, 1, "object", virtual_nr, "Error reading type flag");

#ifndef OLD_DIKU
      nread = fscanf(fl, " %s ", buf);
#else
      nread = fscanf(fl, " %d ", &tmp);
#endif
      assume(nread, 1, "object", virtual_nr, "Extra Flag");

#ifndef OLD_DIKU
      nread = fscanf(fl, " %s ", buf);
#else
      nread = fscanf(fl, " %d ", &tmp);
#endif
      assume(nread, 1, "object", virtual_nr, "wear_flags");

      nread = fscanf(fl, " %d ", &tmp);
      assume(nread, 1, "object", virtual_nr, "value[0]");

      nread = fscanf(fl, " %d ", &tmp);
      assume(nread, 1, "object", virtual_nr, "value[1]");

      nread = fscanf(fl, " %d ", &tmp);
      assume(nread, 1, "object", virtual_nr, "value[2]");

      nread = fscanf(fl, " %d ", &tmp);
      assume(nread, 1, "object", virtual_nr, "value[3]");

      nread = fscanf(fl, " %d ", &tmp);
      assume(nread, 1, "object", virtual_nr, "Weight");
      if (tmp < 0) printf("Negative weight, obj %d\n", virtual_nr);

      nread = fscanf(fl, " %d \n", &tmp);
      assume(nread, 1, "object", virtual_nr, "Cost");

      nread = fscanf(fl, " %d \n", &tmp);
      assume(nread, 1, "object", virtual_nr, "Cost Per Day");
/*

#ifndef OLD_DIKU
      nread = fscanf(fl, " %d ", &tmp);
      assume(nread, 1, "object", virtual_nr, "Base Max");

      nread = fscanf(fl, " %d ", &tmp2);
      assume(nread, 1, "object", virtual_nr, "Additional per hundred players");
      if (tmp <= 0 && tmp2 <= 0) printf("Non-positive limit, obj %d\n", virtual_nr);
#endif
*/

      /* *** extra descriptions *** */

      while (fscanf(fl, " %s \n", chk), *chk == 'E') {
        temp = fread_string(fl);
        temp = fread_string(fl);
      }

      for (i = 0 ; i < MAX_OBJ_AFFECT && *chk == 'A'; i++) {
        nread = fscanf(fl, " %d ", &tmp);
        assume(nread, 1, "object", virtual_nr, "affected location");

        nread = fscanf(fl, " %d \n", &tmp);
        assume(nread, 1, "object", virtual_nr, "Modifier");

        nread = fscanf(fl, " %s \n", chk);
        assume(nread, 1, "object", virtual_nr, "Next string with E/A/$");
      }
    }
  }  while (flag);
}

/* read and allocate space for a '~'-terminated string from a given file */
char *fread_string(FILE *fl)
{
  static char buf[MAX_STRING_LENGTH], tmp[MAX_STRING_LENGTH];
  char *rslt;
  register char *point;
  int flag;

  bzero(buf, MAX_STRING_LENGTH);

  do {
    if (!fgets(tmp, MAX_STRING_LENGTH, fl)) {
      printf("fread_str");
      exit(1);
    }

    if (strlen(tmp) + strlen(buf) > MAX_STRING_LENGTH-1) {
      printf("fread_string: string too large (db.c, fread_string)");
      exit(1);
    } else
      strcat(buf, tmp);

    for (point = buf + strlen(buf) - 2; point >= buf && isspace(*point); point--)
      ;
    if (flag = (*point == '~')) {
      if (*(buf + strlen(buf) - 3) == '\n') {
        *(buf + strlen(buf) - 2) = '\r';
        *(buf + strlen(buf) - 1) = '\0';
      } else {
        *(buf + strlen(buf) -2) = '\0';
      }
    } else {
      *(buf + strlen(buf) + 1) = '\0';
      *(buf + strlen(buf)) = '\r';
    }
  } while (!flag);

  return(buf);
}

void
check_reach(void)
{
  int i, j;
  int dest;
  int first;
  bool changed;

  for (i = 0; i <= top_of_wldt; i++) {
    world[i].flag = world[i].processed = FALSE;
    for (j = 0; j < 6; j++)
      if (world[i].way_out[j].exists)
        world[i].way_out[j].dest =
          rnum_from_vnum(wld_index, top_of_wldt, world[i].way_out[j].dest);
  }
  if ( (first = rnum_from_vnum(wld_index, top_of_wldt, firstroom)) == -1) {
    printf("Your specified first room (%d) is not reachable!\n", firstroom);
    return;
  }

  /* for first pass, flag means "we can get here from firstroom" */
  world[first].flag = TRUE;
  changed = TRUE;
  while (changed) {
    changed = FALSE;
    for (i = 0; i < top_of_wldt; i++) {
      if (world[i].flag && !world[i].processed) {
        world[i].processed = TRUE;
        for (j = 0; j < 6; j++) {
          if (world[i].way_out[j].exists) {
            dest = world[i].way_out[j].dest;
            if (!world[dest].flag) {
              world[dest].flag = TRUE;
              changed = TRUE;
            }
          }
        }
#ifndef NEW_DIKU
        for (j = 0; j < top_of_zont; j++)
          if (world[i].zone == zones[j].number)
            break;
        if (j != top_of_zont) {
          dest = zones[j].teleport_target;
          if (dest != -1 && !world[dest].flag) {
            world[dest].flag = TRUE;
            changed = TRUE;
          }
        }
#endif
      }
    }
  }

  printf("Unreachables:\n");
  for (i = 0; i < top_of_wldt; i++)
    if (!world[i].flag)
      printf("%d\n", world[i].vnum);
  printf("\n");

  for (i = 0; i < top_of_wldt; i++) world[i].flag = world[i].processed = FALSE;

  /* flag now means "we can reach first room from here" */
  world[first].flag = TRUE;
  changed = TRUE;
  while (changed) {
    changed = FALSE;
    for (i = 0; i < top_of_wldt; i++) {
      if (!world[i].flag) {
        for (j = 0; j < 6; j++)
          if (world[i].way_out[j].exists) {
            dest = world[i].way_out[j].dest;
            if (world[dest].flag) {
              world[i].flag = TRUE;
              changed = TRUE;
              break;
            }
          }
        }
#ifndef OLD_DIKU
        if (!world[i].flag) {
          for (j = 0; j < top_of_zont; j++)
            if (zones[j].teleport_target == i)
              break;
          if (j != top_of_zont) {
            dest = zones[j].teleport_target;
            if (world[dest].flag) {
              world[i].flag = TRUE;
              changed = TRUE;
            }
          }
        }
#endif
      }
  }


  printf("Inescapables:\n");
  for (i = 0; i < top_of_wldt; i++)
    if (!world[i].flag)
      printf("%d\n", world[i].vnum);
  printf("\n");
}

void
check_one_way(void)
{
  int i, j, k;
  int other_way, dest;
  static int opposites[] = {2, 3, 0, 1, 5, 4};
  static char *names = "neswud";

  printf("One-ways:\n");
  for (i = 0; i < top_of_wldt; i++)
    for (j = 0; j < 6; j++)
      if (world[i].way_out[j].exists) {
        dest = world[i].way_out[j].dest;
        other_way = opposites[j];
        if (world[dest].way_out[other_way].exists &&
            world[dest].way_out[other_way].dest == i)
          ; /* do nothing */
        else {
          for (k = 0; k < 6; k++)
            if (world[dest].way_out[k].exists &&
                world[dest].way_out[k].dest == i)
              break;
          printf("%d (dir %d(%c))", world[i].vnum, j, names[j]);
          if (k == 6) /* no way back at all */
            printf("\n");
          else
            printf(" (merely non-reciprocal)\n");
        }
      }
}


int main(int argc, char *argv[])
{
  char name[256];

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <basefilename> <firstroom>\n", argv[0]);
    fprintf(stderr, "Your section files must be of the form X.wld, X.obj, X.mob and X.zon.\n");
    fprintf(stderr, "The \"basefilename\" replaces \"X\" in the above.\n\n");
    fprintf(stderr, "\"firstroom\" is the initial room in the section (used for reachability tests).\n");
    fprintf(stderr, "The normal invocation is \"%s lib/tinyworld 3001\"\n", argv[0]);
    fprintf(stderr, "\n\"Unreachable\" rooms are ones that you can't get to from the firstroom.\n");
    fprintf(stderr, "\"Inescapable\" rooms are ones that you can't get to the firstroom from.\n");
    exit(0);
  }

  firstroom = atoi(argv[2]);

  strcpy(name, argv[1]);
  strcat(name, ".wld");

  if ((wld_f = fopen(name, "r")) == 0) {
    fprintf(stderr, "Could not open world file.\n");
    exit(1);
  }

  strcpy(name, argv[1]);
  strcat(name, ".mob");
  if ((mob_f = fopen(name, "r")) == 0) {
    fprintf(stderr, "Could not open mobile file.\n");
    exit(1);
  }

  strcpy(name, argv[1]);
  strcat(name, ".obj");
  if ((obj_f = fopen(name, "r")) == 0) {
    fprintf(stderr, "Could not open object file.\n");
    exit(1);
  }

  strcpy(name, argv[1]);
  strcat(name, ".zon");
  if ((zon_f = fopen(name, "r")) == 0) {
    fprintf(stderr, "Could not open zone file.\n");
    exit(1);
  }

  fprintf(stderr, "Generating world file index.\n");
  wld_index = generate_indices(wld_f, &top_of_wldt);
  CREATE(world, struct room_graph, top_of_wldt);

  fprintf(stderr, "Generating mobile file index.\n");
  mob_index = generate_indices(mob_f, &top_of_mobt);

  fprintf(stderr, "Generating object file index.\n");
  obj_index = generate_indices(obj_f, &top_of_objt);

  fprintf(stderr, "Checking World File.\n");
  check_world(wld_f);

  fprintf(stderr, "Checking Mobile File (only simple mobiles).\n");
  check_mobile(mob_f);

  fprintf(stderr, "Checking Object File.\n");
  check_objects(obj_f);

  fprintf(stderr, "Checking Zone File.\n");
  check_zones(zon_f);

  fclose(zon_f);
  fclose(wld_f);
  fclose(mob_f);
  fclose(obj_f);

  fprintf(stderr, "Checking reachability.\n");
  check_reach();

  fprintf(stderr, "Checking for one-way exits.\n");
  check_one_way();

  printf("Finished all checking.\n");
}
