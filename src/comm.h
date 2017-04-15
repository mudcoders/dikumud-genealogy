/* ************************************************************************
*   File: comm.h                                        Part of CircleMUD *
*  Usage: header file: prototypes of public communication functions       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/* comm.c */
void	send_to_all(char *messg);
void	send_to_char(char *messg, struct char_data *ch);
void	send_to_except(char *messg, struct char_data *ch);
void	send_to_room(char *messg, int room);
void	send_to_room_except(char *messg, int room, struct char_data *ch);
void	send_to_room_except_two
(char *messg, int room, struct char_data *ch1, struct char_data *ch2);
void	send_to_outdoor(char *messg);
void	perform_to_all(char *messg, struct char_data *ch);
void	close_socket(struct descriptor_data *d);


void	act(char *str, int hide_invisible, struct char_data *ch,
struct obj_data *obj, void *vict_obj, int type);

#define TO_ROOM    0
#define TO_VICT    1
#define TO_NOTVICT 2
#define TO_CHAR    3

int	write_to_descriptor(int desc, char *txt);
void	write_to_q(char *txt, struct txt_q *queue);
void	write_to_output(char *txt, struct descriptor_data *d);
void	page_string(struct descriptor_data *d, char *str, int keep_internal);

/* #define SEND_TO_Q(messg, desc)  write_to_q((messg), &(desc)->output) */
#define SEND_TO_Q(messg, desc)  write_to_output((messg), desc)

#define USING_SMALL(d)	((d)->output == (d)->small_outbuf)
#define USING_LARGE(d)  (!USING_SMALL(d))
