
/* ************************************************************************
*  file:  boards.h                                      Part of JediMud   *
*  Usage: improved boards, header file                                    *
*  Written by Jeremy Elson                                                *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
*  All Rights Reserved                                                    *
*  Copyright (C) 1993 The Trustees of The Johns Hopkins University        *
************************************************************************* */



#define NUM_OF_BOARDS	        4	
#define MAX_BOARD_MESSAGES 	40      /* arbitrary */
#define INDEX_SIZE	   ((NUM_OF_BOARDS*MAX_BOARD_MESSAGES) + 5)
#define MAX_MESSAGE_LENGTH 4096		/* arbitrary */

struct board_msginfo {
	int slot_num;     /* pos of message in "master index" */
	char *heading;    /* pointer to message's heading */
	int level;        /* level of poster */
	int heading_len;  /* size of header (for file write) */
	int message_len;  /* size of message text (for file write) */
};

struct board_info_type {
	long vnum;	   /* vnum of this board */
	int read_lvl;	   /* min level to read messages on this board */
	int write_lvl;     /* min level to write messages on this board */
	int remove_lvl;    /* min level to remove messages from this board */
	int clan_num;	   /* clan number or 0 for all clans. */
	char filename[50]; /* file to save this board to */
	int rnum;	   /* rnum of this board */
};

#define VNUM(i) (board_info[i].vnum)
#define READ_LVL(i) (board_info[i].read_lvl)
#define WRITE_LVL(i) (board_info[i].write_lvl)
#define REMOVE_LVL(i) (board_info[i].remove_lvl)
#define CLAN_BOARD(i) (board_info[i].clan_num)
#define FILENAME(i) (board_info[i].filename)
#define RNUM(i) (board_info[i].rnum)

#define NEW_MSG_INDEX(i) (msg_index[i][num_of_msgs[i]])
#define MSG_HEADING(i, j) (msg_index[i][j].heading)
#define MSG_SLOTNUM(i, j) (msg_index[i][j].slot_num)
#define MSG_LEVEL(i, j) (msg_index[i][j].level)

