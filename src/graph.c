/* ************************************************************************
*   File: graph.c                                       Part of CircleMUD *
*  Usage: various graph algorithms                                        *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/*
 * You can define or not define TRACK_THOUGH_DOORS, depending on whether
 * or not you want track to find paths which lead through closed or
 * hidden doors. A setting of '#if 0' means to not go through the doors
 * while '#if 1' will pass through doors to find the target.
 */
#if 1
#define TRACK_THROUGH_DOORS	1
#endif

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"


/* Externals */
ACMD(do_say);
extern struct char_data *character_list;
extern int top_of_world;
extern const char *dirs[];
extern struct room_data *world;

/* local functions */
void bfs_enqueue(sh_int room, int dir);
void bfs_dequeue(void);
void bfs_clear_queue(void);
int find_first_step(sh_int src, sh_int target);
ACMD(do_track);
void hunt_victim(struct char_data * ch);

struct bfs_queue_struct {
  sh_int room;
  char dir;
  struct bfs_queue_struct *next;
};

static struct bfs_queue_struct *queue_head = 0, *queue_tail = 0;

/* Utility macros */
#define MARK(room) (SET_BIT(ROOM_FLAGS(room), ROOM_BFS_MARK))
#define UNMARK(room) (REMOVE_BIT(ROOM_FLAGS(room), ROOM_BFS_MARK))
#define IS_MARKED(room) (ROOM_FLAGGED(room, ROOM_BFS_MARK))
#define TOROOM(x, y) (world[(x)].dir_option[(y)]->to_room)
#define IS_CLOSED(x, y) (EXIT_FLAGGED(world[(x)].dir_option[(y)], EX_CLOSED))

#ifdef TRACK_THROUGH_DOORS
#define VALID_EDGE(x, y) (world[(x)].dir_option[(y)] && \
			  (TOROOM(x, y) != NOWHERE) &&	\
			  (!ROOM_FLAGGED(TOROOM(x, y), ROOM_NOTRACK)) && \
			  (!IS_MARKED(TOROOM(x, y))))
#else
#define VALID_EDGE(x, y) (world[(x)].dir_option[(y)] && \
			  (TOROOM(x, y) != NOWHERE) &&	\
			  (!IS_CLOSED(x, y)) &&		\
			  (!ROOM_FLAGGED(TOROOM(x, y), ROOM_NOTRACK)) && \
			  (!IS_MARKED(TOROOM(x, y))))
#endif

void bfs_enqueue(sh_int room, int dir)
{
  struct bfs_queue_struct *curr;

  CREATE(curr, struct bfs_queue_struct, 1);
  curr->room = room;
  curr->dir = dir;
  curr->next = 0;

  if (queue_tail) {
    queue_tail->next = curr;
    queue_tail = curr;
  } else
    queue_head = queue_tail = curr;
}


void bfs_dequeue(void)
{
  struct bfs_queue_struct *curr;

  curr = queue_head;

  if (!(queue_head = queue_head->next))
    queue_tail = 0;
  free(curr);
}


void bfs_clear_queue(void)
{
  while (queue_head)
    bfs_dequeue();
}


/* 
 * find_first_step: given a source room and a target room, find the first
 * step on the shortest path from the source to the target.
 *
 * Intended usage: in mobile_activity, give a mob a dir to go if they're
 * tracking another mob or a PC.  Or, a 'track' skill for PCs.
 */
int find_first_step(sh_int src, sh_int target)
{
  int curr_dir;
  sh_int curr_room;

  if (src < 0 || src > top_of_world || target < 0 || target > top_of_world) {
    log("Illegal value %d or %d passed to find_first_step. (%s)", src, target, __FILE__);
    return BFS_ERROR;
  }
  if (src == target)
    return BFS_ALREADY_THERE;

  /* clear marks first, some OLC systems will save the mark. */
  for (curr_room = 0; curr_room <= top_of_world; curr_room++)
    UNMARK(curr_room);

  MARK(src);

  /* first, enqueue the first steps, saving which direction we're going. */
  for (curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++)
    if (VALID_EDGE(src, curr_dir)) {
      MARK(TOROOM(src, curr_dir));
      bfs_enqueue(TOROOM(src, curr_dir), curr_dir);
    }

  /* now, do the classic BFS. */
  while (queue_head) {
    if (queue_head->room == target) {
      curr_dir = queue_head->dir;
      bfs_clear_queue();
      return curr_dir;
    } else {
      for (curr_dir = 0; curr_dir < NUM_OF_DIRS; curr_dir++)
	if (VALID_EDGE(queue_head->room, curr_dir)) {
	  MARK(TOROOM(queue_head->room, curr_dir));
	  bfs_enqueue(TOROOM(queue_head->room, curr_dir), queue_head->dir);
	}
      bfs_dequeue();
    }
  }

  return BFS_NO_PATH;
}


/********************************************************
* Functions and Commands which use the above functions. *
********************************************************/

ACMD(do_track)
{
  struct char_data *vict;
  int dir;

  /* The character must have the track skill. */
  if (IS_NPC(ch) || !GET_SKILL(ch, SKILL_TRACK)) {
    send_to_char("You have no idea how.\r\n", ch);
    return;
  }
  one_argument(argument, arg);
  if (!*arg) {
    send_to_char("Whom are you trying to track?\r\n", ch);
    return;
  }
  /* The person can't see the victim. */
  if (!(vict = get_char_vis(ch, arg))) {
    send_to_char("No one is around by that name.\r\n", ch);
    return;
  }
  /* We can't track the victim. */
  if (AFF_FLAGGED(vict, AFF_NOTRACK)) {
    send_to_char("You sense no trail.\r\n", ch);
    return;
  }

  /* 101 is a complete failure, no matter what the proficiency. */
  if (number(0, 101) >= GET_SKILL(ch, SKILL_TRACK)) {
    int tries = 10;
    /* Find a random direction. :) */
    do {
      dir = number(0, NUM_OF_DIRS - 1);
    } while (!CAN_GO(ch, dir) && --tries);
    sprintf(buf, "You sense a trail %s from here!\r\n", dirs[dir]);
    send_to_char(buf, ch);
    return;
  }

  /* They passed the skill check. */
  dir = find_first_step(ch->in_room, vict->in_room);

  switch (dir) {
  case BFS_ERROR:
    send_to_char("Hmm.. something seems to be wrong.\r\n", ch);
    break;
  case BFS_ALREADY_THERE:
    send_to_char("You're already in the same room!!\r\n", ch);
    break;
  case BFS_NO_PATH:
    sprintf(buf, "You can't sense a trail to %s from here.\r\n", HMHR(vict));
    send_to_char(buf, ch);
    break;
  default:	/* Success! */
    sprintf(buf, "You sense a trail %s from here!\r\n", dirs[dir]);
    send_to_char(buf, ch);
    break;
  }
}


void hunt_victim(struct char_data * ch)
{
  int dir;
  byte found;
  struct char_data *tmp;

  if (!ch || !HUNTING(ch) || FIGHTING(ch))
    return;

  /* make sure the char still exists */
  for (found = FALSE, tmp = character_list; tmp && !found; tmp = tmp->next)
    if (HUNTING(ch) == tmp)
      found = TRUE;

  if (!found) {
    do_say(ch, "Damn!  My prey is gone!!", 0, 0);
    HUNTING(ch) = NULL;
    return;
  }
  if ((dir = find_first_step(ch->in_room, HUNTING(ch)->in_room)) < 0) {
    sprintf(buf, "Damn!  I lost %s!", HMHR(HUNTING(ch)));
    do_say(ch, buf, 0, 0);
    HUNTING(ch) = NULL;
  } else {
    perform_move(ch, dir, 1);
    if (ch->in_room == HUNTING(ch)->in_room)
      hit(ch, HUNTING(ch), TYPE_UNDEFINED);
  }
}
