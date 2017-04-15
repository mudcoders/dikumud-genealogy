/* ************************************************************************
*   File: comm.c                                        Part of CircleMUD *
*  Usage: Communication, socket handling, main(), central game loop       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "limits.h"

#define MAX_HOSTNAME	256
#define OPT_USEC	250000  /* time delay corresponding to 4 passes/sec */

/* externs */
extern int	restrict;
extern int	mini_mud;
extern int	no_rent_check;
extern FILE	*player_fl;
extern int	DFLT_PORT;
extern char	*DFLT_DIR;
extern int	MAX_PLAYERS;
extern int	MAX_DESCRIPTORS_AVAILABLE;

extern struct room_data *world;		/* In db.c */
extern int	top_of_world;		/* In db.c */
extern struct time_info_data time_info;	/* In db.c */
extern char	help[];

/* local globals */
struct descriptor_data *descriptor_list, *next_to_process;
struct txt_block *bufpool = 0;	/* pool of large output buffers */
int	buf_largecount;		/* # of large buffers which exist */
int	buf_overflows;		/* # of overflows of output */
int	buf_switches;		/* # of switches from small to large buf */
int	circle_shutdown = 0;	/* clean shutdown */
int	circle_reboot = 0;	/* reboot the game after a shutdown */
int	no_specials = 0;	/* Suppress ass. of special routines */
int	last_desc = 0;		/* last unique num assigned to a desc. */
int	mother_desc = 0;	/* file desc of the mother connection */
int	maxdesc;		/* highest desc num used */
int	avail_descs;		/* max descriptors available */
int	tics = 0;		/* for extern checkpointing */
extern int	nameserver_is_slow;	/* see config.c */
extern int	auto_save;		/* see config.c */
extern int	autosave_time;		/* see config.c */

/* functions in this file */
int	get_from_q(struct txt_q *queue, char *dest);
void	run_the_game(int port);
void	game_loop(int s);
int	init_socket(int port);
int	new_connection(int s);
int	new_descriptor(int s);
int	process_output(struct descriptor_data *t);
int	process_input(struct descriptor_data *t);
void	close_sockets(int s);
void	close_socket(struct descriptor_data *d);
struct timeval timediff(struct timeval *a, struct timeval *b);
void	flush_queues(struct descriptor_data *d);
void	nonblock(int s);
int	perform_subst(struct descriptor_data *t, char *orig, char *subst);

/* extern fcnts */
void	boot_db(void);
void	zone_update(void);
void	affect_update(void); /* In spells.c */
void	point_update(void);  /* In limits.c */
void	mobile_activity(void);
void	string_add(struct descriptor_data *d, char *str);
void	perform_violence(void);
void	show_string(struct descriptor_data *d, char *input);
void	check_reboot(void);
int	isbanned(char *hostname);
void	weather_and_time(int mode);



/* *********************************************************************
*  main game loop and related stuff				       *
********************************************************************* */

int	main(int argc, char **argv)
{
   int	port;
   char	buf[512];
   int	pos = 1;
   char	*dir;

   port = DFLT_PORT;
   dir = DFLT_DIR;

   while ((pos < argc) && (*(argv[pos]) == '-')) {
      switch (*(argv[pos] + 1)) {
      case 'd':
	 if (*(argv[pos] + 2))
	    dir = argv[pos] + 2;
	 else if (++pos < argc)
	    dir = argv[pos];
	 else {
	    log("Directory arg expected after option -d.");
	    exit(0);
	 }
	 break;
      case 'm':
	 mini_mud = 1;
	 no_rent_check = 1;
	 log("Running in minimized mode & with no rent check.");
	 break;
      case 'q':
	 no_rent_check = 1;
	 log("Quick boot mode -- rent check supressed.");
	 break;
      case 'r':
	 restrict = 1;
	 log("Restricting game -- no new players allowed.");
	 break;
      case 's':
	 no_specials = 1;
	 log("Suppressing assignment of special routines.");
	 break;
      default:
	 sprintf(buf, "SYSERR: Unknown option -%c in argument string.", *(argv[pos] + 1));
	 log(buf);
	 break;
      }
      pos++;
   }

   if (pos < argc)
      if (!isdigit(*argv[pos])) {
	 fprintf(stderr, "Usage: %s [-m] [-q] [-r] [-s] [-d pathname] [ port # ]\n", argv[0]);
	 exit(0);
      }
      else if ((port = atoi(argv[pos])) <= 1024) {
	 printf("Illegal port #\n");
	 exit(0);
      }

   sprintf(buf, "Running game on port %d.", port);
   log(buf);

   if (chdir(dir) < 0) {
      perror("Fatal error changing to data directory");
      exit(0);
   }

   sprintf(buf, "Using %s as data directory.", dir);
   log(buf);

   srandom(time(0));
   run_the_game(port);
   return(0);
}





/* Init sockets, run game, and cleanup sockets */
void	run_the_game(int port)
{
   int	s;

   void	signal_setup(void);

   descriptor_list = NULL;

   log("Signal trapping.");
   signal_setup();

   log("Opening mother connection.");
   mother_desc = s = init_socket(port);

   boot_db();

   log("Entering game loop.");

   game_loop(s);

   close_sockets(s);
   fclose(player_fl);

   if (circle_reboot) {
      log("Rebooting.");
      exit(52);            /* what's so great about HHGTTG, anyhow? */
   }

   log("Normal termination of game.");
}






/* Accept new connects, relay commands, and call 'heartbeat-functs' */
void	game_loop(int s)
{
   fd_set input_set, output_set, exc_set;
   struct timeval last_time, now, timespent, timeout, null_time;
   static struct timeval opt_time;
   char	comm[MAX_INPUT_LENGTH];
   char	prompt[MAX_INPUT_LENGTH];
   struct descriptor_data *point, *next_point;
   int	pulse = 0, mins_since_crashsave = 0, mask;
   int	sockets_connected, sockets_playing;
   bool disp;
   char	buf[100];

   null_time.tv_sec = 0;
   null_time.tv_usec = 0;

   opt_time.tv_usec = OPT_USEC;  /* Init time values */
   opt_time.tv_sec = 0;
   gettimeofday(&last_time, (struct timezone *) 0);

   maxdesc = s;


#if defined (OPEN_MAX)
   avail_descs = OPEN_MAX - 8;
#elif defined (USE_TABLE_SIZE)
   {
      int retval;

      retval = setdtablesize(64);
      if (retval == -1)
        log("SYSERR: unable to set table size");
      else {
         sprintf(buf, "%s %d\n", "dtablesize set to: ", retval);
         log(buf);
      }
      avail_descs = getdtablesize() - 8;
   }
#else
   avail_descs = MAX_DESCRIPTORS_AVAILABLE;
#endif

   avail_descs = MIN(avail_descs, MAX_PLAYERS);

   mask = sigmask(SIGUSR1) | sigmask(SIGUSR2) | sigmask(SIGINT) | 
       sigmask(SIGPIPE) | sigmask(SIGALRM) | sigmask(SIGTERM) | 
       sigmask(SIGURG) | sigmask(SIGXCPU) | sigmask(SIGHUP) | 	
       sigmask(SIGSEGV) | sigmask(SIGBUS);

   /* Main loop */
   while (!circle_shutdown) {
      /* Check what's happening out there */
      FD_ZERO(&input_set);
      FD_ZERO(&output_set);
      FD_ZERO(&exc_set);
      FD_SET(s, &input_set);
      for (point = descriptor_list; point; point = point->next) {
	 FD_SET(point->descriptor, &input_set);
	 FD_SET(point->descriptor, &exc_set);
	 FD_SET(point->descriptor, &output_set);
      }

      /* check out the time */
      gettimeofday(&now, (struct timezone *) 0);
      timespent = timediff(&now, &last_time);
      timeout = timediff(&opt_time, &timespent);
      last_time.tv_sec = now.tv_sec + timeout.tv_sec;
      last_time.tv_usec = now.tv_usec + timeout.tv_usec;
      if (last_time.tv_usec >= 1000000) {
	 last_time.tv_usec -= 1000000;
	 last_time.tv_sec++;
      }

      sigsetmask(mask);

      if (select(maxdesc + 1, &input_set, &output_set, &exc_set, &null_time)
           < 0) {
	 perror("Select poll");
	 return;
      }

      if (select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &timeout) < 0) {
	 perror("Select sleep");
	 exit(1);
      }

      sigsetmask(0);

      /* Respond to whatever might be happening */

      /* New connection? */
      if (FD_ISSET(s, &input_set))
	 if (new_descriptor(s) < 0)
	    perror("New connection");

      /* kick out the freaky folks */
      for (point = descriptor_list; point; point = next_point) {
	 next_point = point->next;
	 if (FD_ISSET(point->descriptor, &exc_set)) {
	    FD_CLR(point->descriptor, &input_set);
	    FD_CLR(point->descriptor, &output_set);
	    close_socket(point);
	 }
      }

      for (point = descriptor_list; point; point = next_point) {
	 next_point = point->next;
	 if (FD_ISSET(point->descriptor, &input_set))
	    if (process_input(point) < 0)
	       close_socket(point);
      }

      /* process_commands; */
      for (point = descriptor_list; point; point = next_to_process) {
	 next_to_process = point->next;

	 if ((--(point->wait) <= 0) && get_from_q(&point->input, comm)) {
	    if (point->character && point->connected == CON_PLYNG && 
	        point->character->specials.was_in_room != NOWHERE) {
	       if (point->character->in_room != NOWHERE)
		  char_from_room(point->character);
	       char_to_room(point->character, 
	           point->character->specials.was_in_room);
	       point->character->specials.was_in_room = NOWHERE;
	       act("$n has returned.", 	TRUE, point->character, 0, 0, TO_ROOM);
	    }

	    point->wait = 1;
	    if (point->character)
	       point->character->specials.timer = 0;
	    point->prompt_mode = 1;

	    if (point->str)
	       string_add(point, comm);
	    else if (!point->connected)
	       if (point->showstr_point)
		  show_string(point, comm);
	       else
		  command_interpreter(point->character, comm);
	    else
	       nanny(point, comm);
	 }
      }


      for (point = descriptor_list; point; point = next_point) {
	 next_point = point->next;
	 if (FD_ISSET(point->descriptor, &output_set) && *(point->output))
	    if (process_output(point) < 0)
	       close_socket(point);
	    else
	       point->prompt_mode = 1;
      }

      /* kick out the Phreaky Pholks II  -JE */
      for (point = descriptor_list; point; point = next_to_process) {
	 next_to_process = point->next;
	 if (STATE(point) == CON_CLOSE) 
	    close_socket(point);
      }

      /* give the people some prompts */
      for (point = descriptor_list; point; point = point->next)
	 if (point->prompt_mode) {
	    if (point->str) {
	       write_to_descriptor(point->descriptor, "] ");
	    } else if (!point->connected) {
	       if (point->showstr_point) {
		  write_to_descriptor(point->descriptor, "*** Press return to continue, q to quit ***");
	       } else {
		  if (GET_INVIS_LEV(point->character))
		     sprintf(prompt, "i%d> ", GET_INVIS_LEV(point->character));
		  else {
		     strcpy(prompt, "|");
		     disp = FALSE;

		     if ((PRF_FLAGGED(point->character, PRF_DISPHP)) || 
		         ((PRF_FLAGGED(point->character, PRF_DISPAUTO) && 
		         ((GET_HIT(point->character) / (float)GET_MAX_HIT(point->character)) <
		         0.30)))) {
			sprintf(prompt, "%s %dH", prompt, GET_HIT(point->character));
			disp = TRUE;
		     }

		     if ((PRF_FLAGGED(point->character, PRF_DISPMANA)) || 
		         ((PRF_FLAGGED(point->character, PRF_DISPAUTO) && 
		         ((GET_MANA(point->character) / (float)GET_MAX_MANA(point->character)) <
		         0.30)))) {
			sprintf(prompt, "%s %dM", prompt, GET_MANA(point->character));
			disp = TRUE;
		     }

		     if ((PRF_FLAGGED(point->character, PRF_DISPMOVE)) || 
		         ((PRF_FLAGGED(point->character, PRF_DISPAUTO) && 
		         ((GET_MOVE(point->character) / (float)GET_MAX_MOVE(point->character)) <
		         0.30)))) {
			sprintf(prompt, "%s %dV", prompt, GET_MOVE(point->character));
			disp = TRUE;
		     }

		     if (disp)
			strcat(prompt, " > ");
		     else
			strcpy(prompt, "> ");
		  }

		  write_to_descriptor(point->descriptor, prompt);
	       }
	    }
	    point->prompt_mode = 0;
	 }


      /* handle heartbeat stuff */
      /* Note: pulse now changes every 1/4 sec  */

      pulse++;

      if (!(pulse % PULSE_ZONE))	zone_update();
      if (!(pulse % PULSE_MOBILE))	mobile_activity();
      if (!(pulse % PULSE_VIOLENCE))	perform_violence();

      if (!(pulse % (SECS_PER_MUD_HOUR * 4))) {
	 weather_and_time(1);
	 affect_update();
	 point_update();
	 fflush(player_fl);
      }

      if (auto_save)
         if (!(pulse % (60 * 4))) /* one minute */
	    if (++mins_since_crashsave >= autosave_time) {
	       mins_since_crashsave = 0;
	       Crash_save_all();
	    }

      if (!(pulse % 1200)) {
	 sockets_connected = sockets_playing = 0;

	 for (point = descriptor_list; point; point = next_point) {
	    next_point = point->next;
	    sockets_connected++;
	    if (!point->connected)
	       sockets_playing++;
	 }

	 sprintf(buf, "nusage: %-3d sockets connected, %-3d sockets playing",
	     sockets_connected,
	     sockets_playing);
	 log(buf);

#ifdef RUSAGE
	  {
	    struct rusage rusagedata;

	    getrusage(0, &rusagedata);
	    sprintf(buf, "rusage: %d %d %d %d %d %d %d",
	        rusagedata.ru_utime.tv_sec,
	        rusagedata.ru_stime.tv_sec,
	        rusagedata.ru_maxrss,
	        rusagedata.ru_ixrss,
	        rusagedata.ru_ismrss,
	        rusagedata.ru_idrss,
	        rusagedata.ru_isrss);
	    log(buf);
	 }
#endif

      }

      if (pulse >= 2400) {
	 pulse = 0;
	 check_reboot();
      }

      tics++;        /* tics since last checkpoint signal */
   }
}


/* ******************************************************************
*  general utility stuff (for local use)			    *
****************************************************************** */

int	get_from_q(struct txt_q *queue, char *dest)
{
   struct txt_block *tmp;

   /* Q empty? */
   if (!queue->head)
      return(0);

   tmp = queue->head;
   strcpy(dest, queue->head->text);
   queue->head = queue->head->next;

   free(tmp->text);
   free(tmp);

   return(1);
}



void	write_to_output(char *txt, struct descriptor_data *t)
{
   int size;

   size = strlen(txt);

   /* if we're in the overflow state already, ignore this */
   if (t->bufptr < 0)
      return;

   /* if we have enough space, just write to buffer and that's it! */
   if (t->bufspace >= size) {
      strcpy(t->output+t->bufptr, txt);
      t->bufspace -= size;
      t->bufptr += size;
   } else {      /* otherwise, try to switch to a large buffer */
      if (t->large_outbuf || ((size + strlen(t->output)) > LARGE_BUFSIZE)) {
	 /* we're already using large buffer, or even the large buffer
	    in't big enough -- switch to overflow state */
	 t->bufptr = -1;
	 buf_overflows++;
	 return;
      }

      buf_switches++;
      /* if the pool has a buffer in it, grab it */
      if (bufpool) {
	 t->large_outbuf = bufpool;
	 bufpool = bufpool->next;
      } else { /* else create one */
	 CREATE(t->large_outbuf, struct txt_block, 1);
	 CREATE(t->large_outbuf->text, char, LARGE_BUFSIZE);
	 buf_largecount++;
      }

      strcpy(t->large_outbuf->text, t->output);
      t->output = t->large_outbuf->text;
      strcat(t->output, txt);
      t->bufspace = LARGE_BUFSIZE-1 - strlen(t->output);
      t->bufptr = strlen(t->output);
   }
}



void	write_to_q(char *txt, struct txt_q *queue)
{
   struct txt_block *new;

   CREATE(new, struct txt_block, 1);
   CREATE(new->text, char, strlen(txt) + 1);

   strcpy(new->text, txt);

   /* Q empty? */
   if (!queue->head) {
      new->next = NULL;
      queue->head = queue->tail = new;
   } else {
      queue->tail->next = new;
      queue->tail = new;
      new->next = NULL;
   }
}




struct timeval timediff(struct timeval *a, struct timeval *b)
{
   struct timeval rslt, tmp;

   tmp = *a;

   if ((rslt.tv_usec = tmp.tv_usec - b->tv_usec) < 0) {
      rslt.tv_usec += 1000000;
      --(tmp.tv_sec);
   }
   if ((rslt.tv_sec = tmp.tv_sec - b->tv_sec) < 0) {
      rslt.tv_usec = 0;
      rslt.tv_sec = 0;
   }
   return(rslt);
}





/* Empty the queues before closing connection */
void	flush_queues(struct descriptor_data *d)
{
   if (d->large_outbuf) {
      d->large_outbuf->next = bufpool;
      bufpool = d->large_outbuf;
   }

   while (get_from_q(&d->input, buf2)) 
      ;
}





/* ******************************************************************
*  socket handling						    *
****************************************************************** */



int	init_socket(int port)
{
   int	s;
   char	*opt;
   char	hostname[MAX_HOSTNAME+1];
   struct sockaddr_in sa;
   struct hostent *hp;

   bzero(&sa, sizeof(struct sockaddr_in ));
   gethostname(hostname, MAX_HOSTNAME);

   hp = gethostbyname(hostname);
   if (hp == NULL) {
      perror("gethostbyname");
      exit(1);
   }
   sa.sin_family = hp->h_addrtype;
   sa.sin_port	 = htons(port);

   if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Init-socket");
      exit(1);
   }

   if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof (opt)) < 0) {
      perror ("setsockopt REUSEADDR");
      exit (1);
   }

#ifdef USE_LINGER
   {
      struct linger ld;

      ld.l_onoff = 0;
      ld.l_linger = 1000;
      if (setsockopt(s, SOL_SOCKET, SO_LINGER, &ld, sizeof(ld)) < 0) {
         perror("setsockopt LINGER");
         exit(1);
      }
   }
#endif

   if (bind(s, (struct sockaddr *) & sa, sizeof(sa)) < 0) {
      perror("bind");
      close(s);
      exit(1);
   }
   listen(s, 3);
   return(s);
}





int	new_connection(int s)
{
   struct sockaddr_in isa;
   int	i, t;

   i = sizeof(isa);
   getsockname(s, (struct sockaddr *) & isa, &i);

   if ((t = accept(s, (struct sockaddr *)(&isa), &i)) < 0) {
      perror("Accept");
      return(-1);
   }
   nonblock(t);
   return(t);
}




int	new_descriptor(int s)
{
   int	desc;
   struct descriptor_data *newd, *point, *next_point;
   int	size, sockets_connected, sockets_playing, i;
   struct sockaddr_in sock;
   struct hostent *from;
   extern char *GREETINGS;

   if ((desc = new_connection(s)) < 0)
      return (-1);

   sockets_connected = sockets_playing = 0;

   for (point = descriptor_list; point; point = next_point) {
      next_point = point->next;
      sockets_connected++;
      if (!point->connected)
	 sockets_playing++;
   }

   /*	if ((maxdesc + 1) >= avail_descs) */
   if (sockets_connected >= avail_descs) {
      write_to_descriptor(desc, "Sorry, CircleMUD is full right now... try again later!  :-)\n\r");
      close(desc);
      return(0);
   } else if (desc > maxdesc)
      maxdesc = desc;

   CREATE(newd, struct descriptor_data, 1);

   /* find info */
   size = sizeof(sock);
   if (getpeername(desc, (struct sockaddr *) & sock, &size) < 0) {
      perror("getpeername");
      *newd->host = '\0';
   } else if (nameserver_is_slow ||
      !(from = gethostbyaddr((char *)&sock.sin_addr,
      sizeof(sock.sin_addr), AF_INET))) {
         if (!nameserver_is_slow)
	    perror("gethostbyaddr");
         i = sock.sin_addr.s_addr;
          sprintf(newd->host, "%d.%d.%d.%d", (i & 0x000000FF),
          (i & 0x0000FF00) >> 8, (i & 0x00FF0000) >> 16,
          (i & 0xFF000000) >> 24 );
   } else {
      strncpy(newd->host, from->h_name, 49);
      *(newd->host + 49) = '\0';
   }

   if (isbanned(newd->host) == BAN_ALL) {
      close(desc);
      sprintf(buf2, "Connection attempt denied from [%s]", newd->host);
      mudlog(buf2, CMP, LEVEL_GOD, TRUE);
      free(newd);
      return(0);
   }

/*  Uncomment this if you want new connections logged.  It's usually not
    necessary, and just adds a lot of unnecessary bulk to the logs.

   sprintf(buf2, "New connection from [%s]", newd->host);
   log(buf2);
*/
   /* init desc data */
   newd->descriptor = desc;
   newd->connected = CON_NME;
   newd->bad_pws = 0;
   newd->pos = -1;
   newd->wait = 1;
   newd->prompt_mode = 0;
   *newd->buf = '\0';
   newd->str = 0;
   newd->showstr_head = 0;
   newd->showstr_point = 0;
   *newd->last_input = '\0';
   newd->output = newd->small_outbuf;
   *(newd->output) = '\0';
   newd->bufspace = SMALL_BUFSIZE-1;
   newd->large_outbuf = NULL;
   newd->input.head = NULL;
   newd->next = descriptor_list;
   newd->character = 0;
   newd->original = 0;
   newd->snoop.snooping = 0;
   newd->snoop.snoop_by = 0;
   newd->login_time = time(0);

   if (++last_desc == 1000)
      last_desc = 1;
   newd->desc_num = last_desc;

   /* prepend to list */

   descriptor_list = newd;

   SEND_TO_Q(GREETINGS, newd);
   SEND_TO_Q("By what name do you wish to be known? ", newd);

   return(0);
}



int	process_output(struct descriptor_data *t)
{
   static char	i[LARGE_BUFSIZE + 20];

   /* start writing at the 2nd space so we can prepend "% " for snoop */
   if (!t->prompt_mode && !t->connected) {
      strcpy(i+2, "\n\r");
      strcat(i+2, t->output);
   } else
      strcpy(i+2, t->output);

   if (t->bufptr < 0)
      strcat(i+2, "**OVERFLOW**");

   if (!t->connected && !(t->character && !IS_NPC(t->character) && 
      PRF_FLAGGED(t->character, PRF_COMPACT)))
	 strcat(i+2, "\n\r");

   if (write_to_descriptor(t->descriptor, i+2) < 0) 
      return -1;

   if (t->snoop.snoop_by) {
      i[0] = '%';
      i[1] = ' ';
      SEND_TO_Q(i, t->snoop.snoop_by->desc);
   }

   /* if we were using a large buffer, put the large buffer on the buffer
      pool and switch back to the small one */
   if (t->large_outbuf) {
      t->large_outbuf->next = bufpool;
      bufpool = t->large_outbuf;
      t->large_outbuf = NULL;
      t->output = t->small_outbuf;
   }

   /* reset total bufspace back to that of a small buffer */
   t->bufspace = SMALL_BUFSIZE-1;
   t->bufptr = 0;
   *(t->output) = '\0';

   return 1;
}



int	write_to_descriptor(int desc, char *txt)
{
   int	sofar, thisround, total;

   total = strlen(txt);
   sofar = 0;

   do {
      thisround = write(desc, txt + sofar, total - sofar);
      if (thisround < 0) {
	 perror("Write to socket");
	 return(-1);
      }
      sofar += thisround;
   } while (sofar < total);

   return(0);
}





int	process_input(struct descriptor_data *t)
{
   int	sofar, thisround, begin, squelch, i, k, flag, failed_subst = 0;
   char	tmp[MAX_INPUT_LENGTH+2], buffer[MAX_INPUT_LENGTH + 60];

   sofar = 0;
   flag = 0;
   begin = strlen(t->buf);

   /* Read in some stuff */
   do {
      if ((thisround = read(t->descriptor, t->buf + begin + sofar, 
          MAX_STRING_LENGTH - (begin + sofar) - 1)) > 0)
	 sofar += thisround;
      else if (thisround < 0)
	 if (errno != EWOULDBLOCK) {
	    perror("Read1 - ERROR");
	    return(-1);
	 } 
	 else
	    break;
      else {
	 log("EOF encountered on socket read.");
	 return(-1);
      }
   } while (!ISNEWL(*(t->buf + begin + sofar - 1)));

   *(t->buf + begin + sofar) = 0;

   /* if no newline is contained in input, return without proc'ing */
   for (i = begin; !ISNEWL(*(t->buf + i)); i++)
      if (!*(t->buf + i))
	 return(0);

   /* input contains 1 or more newlines; process the stuff */
   for (i = 0, k = 0; *(t->buf + i); ) {
      if (!ISNEWL(*(t->buf + i)) && !(flag = (k >= (MAX_INPUT_LENGTH - 2))))
	 if (*(t->buf + i) == '\b')	 /* backspace */
	    if (k) {  /* more than one char ? */
	       if (*(tmp + --k) == '$')
		  k--;
	       i++;
	    } 
	    else
	       i++;  /* no or just one char.. Skip backsp */
	 else if (isascii(*(t->buf + i)) && isprint(*(t->buf + i))) {
	    /* trans char, double for '$' (printf)	*/
	    if ((*(tmp + k) = *(t->buf + i)) == '$')
	       *(tmp + ++k) = '$';
	    k++;
	    i++;
	 } 
	 else
	    i++;
      else {
	 *(tmp + k) = 0;
	 if (*tmp == '!')
	    strcpy(tmp, t->last_input);
	 else if (*tmp == '^') {
	    if (!(failed_subst = perform_subst(t, t->last_input, tmp)))
	       strcpy(t->last_input, tmp);
	 } else
	    strcpy(t->last_input, tmp);

	 if (!failed_subst)
	    write_to_q(tmp, &t->input);

	 if (t->snoop.snoop_by) {
	    SEND_TO_Q("% ", t->snoop.snoop_by->desc);
	    SEND_TO_Q(tmp, t->snoop.snoop_by->desc);
	    SEND_TO_Q("\n\r", t->snoop.snoop_by->desc);
	 }

	 if (flag) {
	    sprintf(buffer, "Line too long.  Truncated to:\n\r%s\n\r", tmp);
	    if (write_to_descriptor(t->descriptor, buffer) < 0)
	       return(-1);

	    /* skip the rest of the line */
	    for (; !ISNEWL(*(t->buf + i)); i++)
	       ;
	 }

	 /* find end of entry */
	 for (; ISNEWL(*(t->buf + i)); i++)
	    ;

	 /* squelch the entry from the buffer */
	 for (squelch = 0; ; squelch++)
	    if ((*(t->buf + squelch) = *(t->buf + i + squelch)) == '\0')
	       break;
	 k = 0;
	 i = 0;
      }
   }
   return(1);
}



int	perform_subst(struct descriptor_data *t, char *orig, char *subst)
{
   char new[MAX_INPUT_LENGTH+5];

   char *first, *second, *strpos;

   first = subst+1;
   if (!(second = strchr(first, '^'))) {
      SEND_TO_Q("Invalid substitution.\n\r", t);
      return 1;
   }

   *(second++) = '\0';

   if (!(strpos = strstr(orig, first))) {
      SEND_TO_Q("Invalid substitution.\n\r", t);
      return 1;
   }

   strncpy(new, orig, (strpos-orig));
   new[(strpos-orig)] = '\0';
   strcat(new, second);
   if (((strpos-orig) + strlen(first)) < strlen(orig))
      strcat(new, strpos+strlen(first));
   strcpy(subst, new);

   return 0;
}



void	close_sockets(int s)
{
   log("Closing all sockets.");

   while (descriptor_list)
      close_socket(descriptor_list);

   close(s);
}




void	close_socket(struct descriptor_data *d)
{
   struct descriptor_data *tmp;
   char	buf[100];

   close(d->descriptor);
   flush_queues(d);
   if (d->descriptor == maxdesc)
      --maxdesc;

   /* Forget snooping */
   if (d->snoop.snooping)
      d->snoop.snooping->desc->snoop.snoop_by = 0;

   if (d->snoop.snoop_by) {
      send_to_char("Your victim is no longer among us.\n\r", d->snoop.snoop_by);
      d->snoop.snoop_by->desc->snoop.snooping = 0;
   }

   if (d->character)
      if (d->connected == CON_PLYNG) {
	 save_char(d->character, NOWHERE);
	 act("$n has lost $s link.", TRUE, d->character, 0, 0, TO_ROOM);
	 sprintf(buf, "Closing link to: %s.", GET_NAME(d->character));
	 mudlog(buf, NRM, MAX(LEVEL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
	 d->character->desc = 0;
      }
      else {
	 sprintf(buf, "Losing player: %s.", GET_NAME(d->character));
	 mudlog(buf, CMP, MAX(LEVEL_IMMORT, GET_INVIS_LEV(d->character)), TRUE);
	 free_char(d->character);
      }
   else
      mudlog("Losing descriptor without char.", CMP, LEVEL_IMMORT, TRUE);

   if (next_to_process == d)  /* to avoid crashing the process loop */
      next_to_process = next_to_process->next;

   if (d == descriptor_list) /* this is the head of the list */
      descriptor_list = descriptor_list->next;
   else {  /* This is somewhere inside the list */
      /* Locate the previous element */
      for (tmp = descriptor_list; (tmp->next != d) && tmp; tmp = tmp->next)
	 ;
      tmp->next = d->next;
   }

   if (d->showstr_head)
      free(d->showstr_head);
   free(d);
}


#if defined(SVR4) || defined(LINUX)

void	nonblock(int s)
{
   int	flags;
   flags = fcntl(s, F_GETFL);
   flags |= O_NONBLOCK;
   if (fcntl(s, F_SETFL, flags) < 0) {
      perror("Fatal error executing nonblock (comm.c)");
      exit(1);
   }
}

#else

void	nonblock(int s)
{
   if (fcntl(s, F_SETFL, FNDELAY) == -1) {
      perror("Fatal error executing nonblock (comm.c)");
      exit(1);
   }
}

#endif



/* ****************************************************************
*	Public routines for system-to-player-communication	  *
*******************************************************************/



void	send_to_char(char *messg, struct char_data *ch)
{
   if (ch->desc && messg)
      SEND_TO_Q(messg, ch->desc);
}




void	send_to_all(char *messg)
{
   struct descriptor_data *i;

   if (messg)
      for (i = descriptor_list; i; i = i->next)
	 if (!i->connected)
	    SEND_TO_Q(messg, i);
}


void	send_to_outdoor(char *messg)
{
   struct descriptor_data *i;

   if (messg)
      for (i = descriptor_list; i; i = i->next)
	 if (!i->connected)
	    if (OUTSIDE(i->character))
	       SEND_TO_Q(messg, i);
}


void	send_to_except(char *messg, struct char_data *ch)
{
   struct descriptor_data *i;

   if (messg)
      for (i = descriptor_list; i; i = i->next)
	 if (ch->desc != i && !i->connected)
	    SEND_TO_Q(messg, i);
}



void	send_to_room(char *messg, int room)
{
   struct char_data *i;

   if (messg)
      for (i = world[room].people; i; i = i->next_in_room)
	 if (i->desc)
	    SEND_TO_Q(messg, i->desc);
}




void	send_to_room_except(char *messg, int room, struct char_data *ch)
{
   struct char_data *i;

   if (messg)
      for (i = world[room].people; i; i = i->next_in_room)
	 if (i != ch && i->desc)
	    SEND_TO_Q(messg, i->desc);
}


void	send_to_room_except_two
(char *messg, int room, struct char_data *ch1, struct char_data *ch2)
{
   struct char_data *i;

   if (messg)
      for (i = world[room].people; i; i = i->next_in_room)
	 if (i != ch1 && i != ch2 && i->desc)
	    SEND_TO_Q(messg, i->desc);
}



/* higher-level communication */

void	act(char *str, int hide_invisible, struct char_data *ch,
struct obj_data *obj, void *vict_obj, int type)
{
   register char	*strp, *point, *i;
   struct char_data *to;
   static char	buf[MAX_STRING_LENGTH];

   if (!str)
      return;
   if (!*str)
      return;

   if (type == TO_VICT)
      to = (struct char_data *) vict_obj;
   else if (type == TO_CHAR)
      to = ch;
   else
      to = world[ch->in_room].people;

   for (; to; to = to->next_in_room) {
      if (to->desc && ((to != ch) || (type == TO_CHAR)) &&  
          (CAN_SEE(to, ch) || !hide_invisible || 
          (type == TO_VICT)) && AWAKE(to) && 
          !PLR_FLAGGED(to, PLR_WRITING) && 
          !((type == TO_NOTVICT) && (to == (struct char_data *) vict_obj))) {
	 for (strp = str, point = buf; ; )
	    if (*strp == '$') {
	       switch (*(++strp)) {
	       case 'n':
		  i = PERS(ch, to);
		  break;
	       case 'N':
		  i = PERS((struct char_data *) vict_obj, to);
		  break;
	       case 'm':
		  i = HMHR(ch);
		  break;
	       case 'M':
		  i = HMHR((struct char_data *) vict_obj);
		  break;
	       case 's':
		  i = HSHR(ch);
		  break;
	       case 'S':
		  i = HSHR((struct char_data *) vict_obj);
		  break;
	       case 'e':
		  i = HSSH(ch);
		  break;
	       case 'E':
		  i = HSSH((struct char_data *) vict_obj);
		  break;
	       case 'o':
		  i = OBJN(obj, to);
		  break;
	       case 'O':
		  i = OBJN((struct obj_data *) vict_obj, to);
		  break;
	       case 'p':
		  i = OBJS(obj, to);
		  break;
	       case 'P':
		  i = OBJS((struct obj_data *) vict_obj, to);
		  break;
	       case 'a':
		  i = SANA(obj);
		  break;
	       case 'A':
		  i = SANA((struct obj_data *) vict_obj);
		  break;
	       case 'T':
		  i = (char *) vict_obj;
		  break;
	       case 'F':
		  i = fname((char *) vict_obj);
		  break;
	       case '$':
		  i = "$";
		  break;
	       default:
		  log("SYSERR: Illegal $-code to act():");
		  strcpy(buf1, "SYSERR: ");
		  strcat(buf1, str);
		  log(buf1);
		  break;
	       }
	       while ((*point = *(i++)))
		  ++point;
	       ++strp;
	    }
	    else if (!(*(point++) = *(strp++)))
	       break;

	 *(--point) = '\n';
	 *(++point) = '\r';
	 *(++point) = '\0';

	 SEND_TO_Q(CAP(buf), to->desc);
      }
      if ((type == TO_VICT) || (type == TO_CHAR))
	 return;
   }
}


