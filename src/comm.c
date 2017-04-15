/***************************************************************************
 *  file: comm.c , Communication module.                   Part of DIKUMUD *
 *  Usage: Communication, central game loop.                               *
 *  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
 *  All Rights Reserved                                                    *
 *  Using *any* part of DikuMud without having read license.doc is         *
 *  violating our copyright.                                               *
 *                                                                         *
 *  Copyright (C) 1992, 1993 Michael Chastain, Michael Quan, Mitchell Tse  *
 *  Performance optimization and bug fixes by MERC Industries.             *
 *  You can use our stuff in any way you like whatsoever so long as this   *
 *  copyright notice remains intact.  If you like it please drop a line    *
 *  to mec@garnet.berkeley.edu.                                            *
 *                                                                         *
 *  This is free software and you are benefitting.  We hope that you       *
 *  share your changes too.  What goes around, comes around.               *
 ***************************************************************************/

#include <sys/types.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <malloc.h>
#include <errno.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "utils.h"
#include "interp.h"
#include "handler.h"
#include "db.h"

extern int socket( int domain, int type, int protocol );
extern int setsockopt(int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen);
extern int bind(int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len);
extern int select( int width, fd_set *readfds, fd_set *writefds,
    fd_set *exceptfds, struct timeval *timeout );
extern int listen( int s, int backlog );
extern int getsockname(int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __len);
extern int accept(int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len);
extern int getpeername(int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __len);

extern int gettimeofday( struct timeval *tp, struct timezone *tzp );
extern char * inet_ntoa( struct in_addr *in );

#define DFLT_PORT   4000
#define MAX_NAME_LENGTH 15
#define MAX_HOSTNAME    256
#define OPT_USEC    250000      /* Microseconds per pass    */



extern int errno;

/* externs */

extern char greetings[MAX_STRING_LENGTH];

extern struct room_data *world;   /* In db.c */
extern int top_of_world;            /* In db.c */
extern struct time_info_data time_info;  /* In db.c */
extern char help[];
extern struct ban_t *ban_list;        /* In db.c */

/* local globals */

struct descriptor_data *descriptor_list, *next_to_process;

int god    = 0;      /* all new chars are gods! */
int slow_death = 0;  /* Shut her down, Martha, she's sucking mud */
int diku_shutdown = 0;    /* clean shutdown */
int maxdesc;



void shutdown_request(void);
void logsig(void);
void hupsig(void);

char * get_from_q(struct txt_q *queue);
void game_loop(int s);
int init_socket(int port);
int new_connection(int s);
int new_descriptor(int s);
int process_output(struct descriptor_data *t);
int process_input(struct descriptor_data *t);
void close_socket(struct descriptor_data *d);
void flush_queues(struct descriptor_data *d);
void nonblock(int s);
void parse_name(struct descriptor_data *desc, char *arg);
int number_playing(void);

/* extern functions */

struct char_data *make_char(char *name, struct descriptor_data *desc);
void boot_db(void);
void zone_update(void);
void affect_update( void ); /* In spells.c */
void point_update( void );  /* In limits.c */
void mobile_activity(void);
void string_add(struct descriptor_data *d, char *str);
void perform_violence(void);
void stop_fighting(struct char_data *ch);
void show_string(struct descriptor_data *d, char *input);



int main( int argc, char *argv[] )
{
    int pos;
    int port	= DFLT_PORT;
    char *dir	= DFLT_DIR;
    int control;

    for ( pos = 1; pos < argc && argv[pos][0] == '-' ; pos++ )
    {
	switch (*(argv[pos] + 1))
	{
	case 'g':
	    god = 1;
	    log( "God creation mode selected." );
	    break;

	case 'd':
	    if ( argv[pos][2] != '\0' )
		dir = &argv[pos][2];
	    else if (++pos < argc)
		dir = &argv[pos][0];
	    else
	    {
		fprintf( stderr, "Directory arg expected after -d.\n\r" );
		exit( 1 );
	    }
	    break;

	default:
	    fprintf( stderr, "Unknown option -%c.\n\r", argv[pos][1] );
	    exit( 1 );
	    break;
	}
    }

    if (pos < argc)
    {
	if (!isdigit(argv[pos][0]))
	{
	    fprintf( stderr, "Usage: %s [-g] [-d pathname] [port #]\n",
		argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi(argv[pos]) ) <= 1024 )
	{
	    printf( "Illegal port #\n" );
	    exit( 1 );
	}
    }

    sprintf( log_buf, "[Port %d] [Dir %s].", port, dir );
    log( log_buf );

    if ( chdir( dir ) < 0 )
    {
	perror( dir );
	exit( 1 );
    }

    srandom( time(0) );

    /*
     * Optional memory tuning.
     */
#if defined(sun)
    mallopt( M_MXFAST, 96 );
    mallopt( M_NLBLKS, 1024 );
#endif

    signal( SIGPIPE, SIG_IGN );
    control   = init_socket( port );
    boot_db( );
    game_loop( control );

    while ( descriptor_list )
	close_socket( descriptor_list );
    log( "Normal termination of game." );
    exit( 0 );
    return 0;
}



/* Accept new connects, relay commands, and call 'heartbeat-functs' */
void game_loop( int control )
{
    fd_set input_set, output_set, exc_set;
    static struct timeval null_time = {0, 0};
    struct timeval last_time, now_time, stall_time;
    char buf[100];
    struct descriptor_data *point, *next_point;
    int pulse = 0, mask;
    char *pcomm;
    bool fStall;

    gettimeofday(&last_time, (struct timezone *) 0);

    maxdesc = control;

    mask = sigmask(SIGUSR1) | sigmask(SIGUSR2) | sigmask(SIGINT) |
	sigmask(SIGPIPE) | sigmask(SIGALRM) | sigmask(SIGTERM) |
	sigmask(SIGURG) | sigmask(SIGXCPU) | sigmask(SIGHUP) |
	sigmask(SIGVTALRM);

#if 0
    mallocmap();
#endif

    /* Main loop */
    while (!diku_shutdown)
    {
	/* Check what's happening out there */
	extern void bzero( );	/* For FD_ZERO ick! */

	FD_ZERO(&input_set);
	FD_ZERO(&output_set);
	FD_ZERO(&exc_set);
	FD_SET(control, &input_set);
	for (point = descriptor_list; point; point = point->next)
	{
	    FD_SET(point->descriptor, &input_set);
	    FD_SET(point->descriptor, &exc_set);
	    FD_SET(point->descriptor, &output_set);
	}

	sigsetmask(mask);

	if (select(maxdesc + 1, &input_set, &output_set, &exc_set, &null_time)
	    < 0)
	{
	    perror("Select poll");
	    exit(1);
	}

	sigsetmask(0);

	/* Respond to whatever might be happening */

	/* New connection? */
	if (FD_ISSET(control, &input_set))
	    if (new_descriptor(control) < 0)
		perror("New connection");

	/* kick out the freaky folks */
	for (point = descriptor_list; point; point = next_point)
	{
	    next_point = point->next;
	    if (FD_ISSET(point->descriptor, &exc_set))
	    {
		FD_CLR(point->descriptor, &input_set);
		FD_CLR(point->descriptor, &output_set);
		if ( point->character )
		    save_char_obj(point->character);
		close_socket(point);
	    }
	}

	/* Read input */
	for (point = descriptor_list; point; point = next_point)
	{
	    next_point = point->next;
	    if (FD_ISSET(point->descriptor, &input_set))
	    {
		if (process_input(point) < 0)
		{
		    if ( point->character )
			save_char_obj( point->character );
		    close_socket(point);
		    continue;
		}
		point->newline = FALSE;
	    }
	    else
		point->newline = TRUE;
	}

	/* process_commands; */
	for (point = descriptor_list; point; point = next_to_process)
	{
	    next_to_process = point->next;

	    if (--(point->wait) <= 0
	    && (pcomm = get_from_q( &point->input )) != NULL )
	    {
		if (point->character && point->connected == CON_PLAYING &&
		    point->character->specials.was_in_room !=   NOWHERE)
		{
		    if (point->character->in_room != NOWHERE)
			char_from_room(point->character);
		    char_to_room(point->character,
			point->character->specials.was_in_room);
		    point->character->specials.was_in_room = NOWHERE;
		    act("$n has returned.",
			TRUE, point->character, 0, 0, TO_ROOM);
		    affect_total(point->character);
		}

		point->wait = 1;
		if (point->character)
		    point->character->specials.timer = 0;

		if (point->str)
		    string_add(point, pcomm);
		else if (point->connected != CON_PLAYING)
		    nanny(point, pcomm);
		else if (point->showstr_point)
		    show_string(point, pcomm);
		else
		    command_interpreter(point->character, pcomm);
		free( pcomm );

		/* Cheesy way to force prompts */
		write_to_q( "", &point->output );
	    }
	}


	for (point = descriptor_list; point; point = next_point)
	{
	    next_point = point->next;

	    if (FD_ISSET(point->descriptor, &output_set) && point->output.head)
	    {
		/* give the people some prompts */
		if (point->str)
		    write_to_q( "] ", &point->output );
		else if (point->connected != CON_PLAYING)
		    ;
		else if (point->showstr_point)
		    write_to_q( "*** Press return ***", &point->output );
		else
		{
		    if ( !IS_SET(point->character->specials.act, PLR_COMPACT) )
			write_to_q( "\n\r", &point->output );
		    sprintf( buf, "<%dhp %dm %dmv> ",
			GET_HIT(point->character),
			GET_MANA(point->character),
			GET_MOVE(point->character) );
		    write_to_q( buf, &point->output );
		}

		if (process_output(point) < 0)
		{
		    if ( point->character )
			save_char_obj( point->character );
		    close_socket(point);
		}
	    }
	}

	/* See if everyone wants clock FAST. */
	fStall = TRUE;
	for (point = descriptor_list; point; point = next_to_process)
	{
	    if ( point->connected != CON_PLAYING )
		continue;
	    if ( point->tick_wait > 0 )
		continue;
	    fStall = TRUE;
	}

	/*
	 * Heartbeat.
	 * All autonomous actions (including fighting and healing)
	 * are subdivisions of the basic pulse at OPT_USEC interval.
	 */

	pulse++;

	if (pulse % PULSE_ZONE == 0)
	    zone_update();

	if (pulse % PULSE_MOBILE == 0)
	    mobile_activity();

	if (pulse % PULSE_VIOLENCE == 0)
	    perform_violence();

	if (pulse % ((SECS_PER_MUD_HOUR*1000000)/OPT_USEC) == 0)
	{
	    weather_and_time(1);
	    affect_update();
	    point_update();
	}



	/*
	 * Synchronize to an OPT_USEC clock.
	 * Sleep( last_time + OPT_USEC - now ).
	 */
	if ( fStall )
	{
	    gettimeofday( &now_time, NULL );
	    stall_time.tv_usec  = last_time.tv_usec - now_time.tv_usec
			    + OPT_USEC;
	    stall_time.tv_sec   = last_time.tv_sec  - now_time.tv_sec;
	    if ( stall_time.tv_usec < 0 )
	    {
		stall_time.tv_usec += 1000000;
		stall_time.tv_sec--;
	    }
	    if ( stall_time.tv_usec >= 1000000 )
	    {
		stall_time.tv_usec -= 1000000;
		stall_time.tv_sec++;
	    }

	    if ( stall_time.tv_sec > 0
	    || ( stall_time.tv_sec == 0 && stall_time.tv_usec > 0 ) )
	    {
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
		{
		    perror( "Select stall" );
		    exit( 0 );
		}
	    }
	}

	gettimeofday( &last_time, NULL );
    }
}



char * get_from_q(struct txt_q *queue)
{
    struct txt_block *tmp;
    char *dest;

    /* Q empty? */
    if (!queue->head)
	return NULL;

    tmp         = queue->head;
    dest        = tmp->text;
    queue->head = tmp->next;

    free( tmp );
    return dest;
}



void write_to_q(char *txt, struct txt_q *queue)
{
    struct txt_block *new;

    CREATE(new, struct txt_block, 1);
    CREATE(new->text, char, strlen(txt) + 1);

    strcpy(new->text, txt);

    /* Q empty? */
    if (!queue->head)
    {
	new->next = NULL;
	queue->head = queue->tail = new;
    }
    else
    {
	queue->tail->next = new;
	queue->tail = new;
	new->next = NULL;
    }
}



/* Empty the queues before closing connection */
void flush_queues(struct descriptor_data *d)
{
    char *pbuf;

    while ( ( pbuf = get_from_q( &d->input ) ) != NULL )
	free( pbuf );

    while ( ( pbuf = get_from_q( &d->output ) ) != NULL )
	free( pbuf );
}



int init_socket(int port)
{
    int s;
    char *opt;
    char hostname[MAX_HOSTNAME+1];
    struct sockaddr_in sa;
    struct hostent *hp;

    gethostname(hostname, MAX_HOSTNAME);
    hp = gethostbyname(hostname);
    if (hp == NULL)
    {
	perror("gethostbyname");
	exit(1);
    }

    sa.sin_family      = hp->h_addrtype;
    sa.sin_port        = htons(port);
    sa.sin_addr.s_addr = 0;
    sa.sin_zero[0]     = 0;
    sa.sin_zero[1]     = 0;
    sa.sin_zero[2]     = 0;
    sa.sin_zero[3]     = 0;
    sa.sin_zero[4]     = 0;
    sa.sin_zero[5]     = 0;
    sa.sin_zero[6]     = 0;
    sa.sin_zero[7]     = 0;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
	perror("Init-socket");
	exit(1);
    }
    if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR,
	(char *) &opt, sizeof (opt)) < 0)
    {
	perror ("setsockopt SO_REUSEADDR");
	exit (1);
    }

#if defined(SO_DONTLINGER)
    {
	struct linger ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if (setsockopt(s, SOL_SOCKET, SO_DONTLINGER, &ld, sizeof(ld)) < 0)
	{
	    perror("setsockopt SO_DONTLINGER");
	    exit( 1 );
	}
    }
#endif

    if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) < 0)
    {
	perror("bind");
	close(s);
	exit(1);
    }
    listen(s, 3);
    return(s);
}



int new_connection(int s)
{
    struct sockaddr_in isa;
    /* struct sockaddr peer; */
    int i;
    int t;

    i = sizeof(isa);
    getsockname(s, (struct sockaddr *) &isa, &i);

    if ((t = accept(s, (struct sockaddr *) &isa, &i)) < 0)
    {
	perror("Accept");
	return(-1);
    }
    nonblock(t);

    return(t);
}



int number_playing(void)
{
    struct descriptor_data *d;

    int i;

    for ( i = 0, d = descriptor_list ; d ; d = d->next )
	i++;

    return(i);
}



int new_descriptor(int s)
{
    int desc;
    struct descriptor_data *newd;
    int size;
    struct sockaddr_in sock;
    struct hostent *from;
    struct ban_t *tmp;
    char buf [ MAX_STRING_LENGTH ];

    if ((desc = new_connection(s)) < 0)
	return (-1);

    if (desc > maxdesc)
	maxdesc = desc;

    CREATE(newd, struct descriptor_data, 1);

    /* find info */
    size = sizeof(sock);
    if (getpeername(desc, (struct sockaddr *) &sock, &size) < 0)
    {
	perror("getpeername");
	*newd->host = '\0';
    }
    else
    {
      int addr;

  		addr = ntohl( sock.sin_addr.s_addr );

  		sprintf( buf, "%d.%d.%d.%d", ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF, ( addr >>  8 ) & 0xFF, ( addr ) & 0xFF);
  		sprintf( log_buf, "Sock.sinaddr:  %s", buf );

  		log( log_buf );

	strcpy(newd->host, inet_ntoa(&sock.sin_addr));
	from = gethostbyaddr( (char *) &sock.sin_addr,
	    sizeof(sock.sin_addr), AF_INET );
	if ( from )
	{
	    strncpy(newd->host, from->h_name, 49);
	    *(newd->host + 49) = '\0';
	}
    }

#if defined(TWIT_FILTER)
    /*
     * Furey: a site-specific rebuke.
     */
    if ( !strncmp( newd->host, "etch2mac", 8 ) )
    {
	write_to_descriptor( desc,
    "Your site is full of twits.  You are banned for a week.\n\r" );
	close( desc );
	free( newd );
	return 0;
    }
#endif

    /*
     * Swiftest: I added the following to ban sites. I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     */
    for ( tmp = ban_list; tmp; tmp = tmp->next )
    {
	if ( !str_cmp( tmp->name, newd->host ) )
	{
	    write_to_descriptor( desc,
	"Your site has been banned from MERCDikuMud.\n\r" );
	    close( desc );
	    free( newd );
	    return 0;
	}
    }


    /* init desc data */
    newd->descriptor = desc;
    newd->connected  = 1;
    newd->wait = 1;
    *newd->buf = '\0';
    newd->str = 0;
    newd->showstr_head = 0;
    newd->showstr_point = 0;
    newd->newline = FALSE;
    *newd->last_input= '\0';
    newd->output.head = NULL;
    newd->input.head = NULL;
    newd->next = descriptor_list;
    newd->character = 0;
    newd->original = 0;
    newd->snoop.snooping = 0;
    newd->snoop.snoop_by = 0;

    /* prepend to list */

    descriptor_list = newd;

    write_to_q( greetings, &newd->output );
    write_to_q( "By what name do you wish to be known? ", &newd->output );

    return(0);
}



int process_output(struct descriptor_data *t)
{
    char buf[2 * MAX_STRING_LENGTH];
    int ibuf = 0;
    char *pstr;
    int ilen;

    if ( t->newline )
    {
	buf[ibuf++] = '\n';
	buf[ibuf++] = '\r';
    }

    /* Cycle thru output queue */
    while ( ( pstr = get_from_q(&t->output) ) != NULL )
    {
	if(t->snoop.snoop_by)
	{
	    write_to_q("% ",&t->snoop.snoop_by->desc->output);
	    write_to_q(pstr, &t->snoop.snoop_by->desc->output);
	}
	ilen = strlen( pstr );
	if ( ibuf + ilen > sizeof(buf)-10 )
	    ilen = sizeof(buf)-10 - ibuf;
	memcpy( &buf[ibuf], pstr, ilen );
	ibuf += ilen;
	free( pstr );
	if ( ibuf > sizeof(buf)/2 )
	    break;
    }

    buf[ibuf] = '\0';
    return write_to_descriptor( t->descriptor, buf );
}


int write_to_descriptor(int desc, char *txt)
{
    int sofar, thisround, total;

    total = strlen(txt);
    sofar = 0;

    do
    {
	thisround = write(desc, txt + sofar, total - sofar);
	if (thisround < 0)
	{
	    perror("Write to socket");
	    return(-1);
	}
	sofar += thisround;
    }
    while (sofar < total);

    return(0);
}



int process_input(struct descriptor_data *t)
{
    int sofar, thisround, begin, squelch, i, k, flag;
    char tmp[MAX_INPUT_LENGTH+2], buffer[MAX_INPUT_LENGTH + 60];

    sofar = 0;
    flag = 0;
    begin = strlen(t->buf);

    /* Read in some stuff */
    do
    {
	if ((thisround = read(t->descriptor, t->buf + begin + sofar,
	    MAX_STRING_LENGTH - (begin + sofar) - 1)) > 0){
	    sofar += thisround;
	      }
	else
	    if (thisround < 0)
		if(errno != EWOULDBLOCK)
		    {
		    perror("Read1 - ERROR");
		    return(-1);
		}
		else
		    break;
	    else
	    {
		log("EOF encountered on socket read.");
		return(-1);
	    }
    }
    while (!ISNEWL(*(t->buf + begin + sofar - 1)));

    *(t->buf + begin + sofar) = 0;

    /* if no newline is contained in input, return without proc'ing */
    for (i = begin; !ISNEWL(*(t->buf + i)); i++)
	if (!*(t->buf + i))
	    return(0);

    /* input contains 1 or more newlines; process the stuff */
    for (i = 0, k = 0; *(t->buf + i);)
    {
	if (!ISNEWL(*(t->buf + i)) && !(flag = (k >= (MAX_INPUT_LENGTH - 2))))
	    if(*(t->buf + i) == '\b')    /* backspace */
		if (k)  /* more than one char ? */
		{
		    if (*(tmp + --k) == '$')
			k--;
		    i++;
		}
		else
		    i++;  /* no or just one char.. Skip backsp */
	    else
		if (isascii(*(t->buf + i)) && isprint(*(t->buf + i)))
		{
		    /* trans char, double for '$' (printf)  */
		    if ((*(tmp + k) = *(t->buf + i)) == '$')
			*(tmp + ++k) = '$';
		    k++;
		    i++;
		}
		else
		    i++;
	else
	{
	    *(tmp + k) = 0;
	    if(*tmp == '!')
		strcpy(tmp,t->last_input);
	    else
		strcpy(t->last_input,tmp);

	    write_to_q(tmp, &t->input);

	    if(t->snoop.snoop_by)
		{
		    write_to_q("% ",&t->snoop.snoop_by->desc->output);
		    write_to_q(tmp,&t->snoop.snoop_by->desc->output);
		    write_to_q("\n\r",&t->snoop.snoop_by->desc->output);
		}

	    if (flag)
	    {
		sprintf(buffer,
		    "Line too long. Truncated to:\n\r%s\n\r", tmp);
		if (write_to_descriptor(t->descriptor, buffer) < 0)
		    return(-1);

		/* skip the rest of the line */
		for (; !ISNEWL(*(t->buf + i)); i++);
	    }

	    /* find end of entry */
	    for (; ISNEWL(*(t->buf + i)); i++);

	    /* squelch the entry from the buffer */
	    for (squelch = 0;; squelch++)
		if ((*(t->buf + squelch) =
		    *(t->buf + i + squelch)) == '\0')
		    break;
	    k = 0;
	    i = 0;
	}
    }
    return(1);
}



void close_socket(struct descriptor_data *d)
{
    struct descriptor_data *tmp;

    process_output( d );
    close( d->descriptor );
    if ( d->descriptor == maxdesc )
	--maxdesc;

    /* Forget snooping */
    if (d->snoop.snooping)
	d->snoop.snooping->desc->snoop.snoop_by = 0;

    if (d->snoop.snoop_by)
    {
	send_to_char(
	    "Your victim is no longer among us.\n\r",d->snoop.snoop_by );
	d->snoop.snoop_by->desc->snoop.snooping = 0;
    }

    if (d->character)
    {
	if ( d->connected == CON_PLAYING )
	{
	    act("$n has lost $s link.", TRUE, d->character, 0, 0, TO_ROOM);
	    sprintf(log_buf, "Closing link to: %s.", GET_NAME(d->character));
	    log( log_buf );
	    d->character->desc = 0;
	}
	else
	{
	    sprintf(log_buf, "Losing player: %s.", GET_NAME(d->character) );
	    log( log_buf );
	    free_char( d->character );
	}
    }
    else
    {
	log( "Losing descriptor without char." );
    }

    if (next_to_process == d)
	next_to_process = next_to_process->next;

    if ( d == descriptor_list )
	descriptor_list = descriptor_list->next;
    else
    {
	/* Locate the previous element */
	for (tmp = descriptor_list; (tmp->next != d) && tmp; tmp = tmp->next)
	    ;
	tmp->next = d->next;
    }

    if ( d->showstr_head )
	free( d->showstr_head );

    free( d );
}


void nonblock(int s)
{
    if (fcntl(s, F_SETFL, FNDELAY) == -1)
    {
	perror("Noblock");
	exit(1);
    }
}



void send_to_char(char *messg, struct char_data *ch)
{

    if (ch->desc && messg)
	write_to_q(messg, &ch->desc->output);
}



void send_to_all(char *messg)
{
    struct descriptor_data *i;

    if (messg)
	for (i = descriptor_list; i; i = i->next)
	    if (!i->connected)
		write_to_q(messg, &i->output);
}


void send_to_outdoor(char *messg)
{
    struct descriptor_data *i;

    if (messg)
	for (i = descriptor_list; i; i = i->next)
	    if (!i->connected)
		if (OUTSIDE(i->character))
		    write_to_q(messg, &i->output);
}


void send_to_room(char *messg, int room)
{
    struct char_data *i;

    if (messg)
	for (i = world[room].people; i; i = i->next_in_room)
	    if (i->desc)
		write_to_q(messg, &i->desc->output);
}



void act(char *str, int hide_invisible, struct char_data *ch,
    struct obj_data *obj, void *vict_obj, int type)
{
    register char *strp, *point, *i = NULL;
    struct char_data *to;
    char buf[MAX_STRING_LENGTH];

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

    for (; to; to = to->next_in_room)
    {
	if (to->desc && ((to != ch) || (type == TO_CHAR)) &&
	    (CAN_SEE(to, ch) || !hide_invisible ||
	    (type == TO_VICT)) && AWAKE(to) &&
	    !((type == TO_NOTVICT) && (to == (struct char_data *) vict_obj)))
	{
	    for (strp = str, point = buf;;)
		if (*strp == '$')
		{
		    switch (*(++strp))
		    {
			case 'n': i = PERS(ch, to); break;
			case 'N': i = PERS((struct char_data *) vict_obj, to);
					break;
			case 'm': i = HMHR(ch); break;
			case 'M': i = HMHR((struct char_data *) vict_obj);
					break;
			case 's': i = HSHR(ch); break;
			case 'S': i = HSHR((struct char_data *) vict_obj);
					break;
			case 'e': i = HSSH(ch); break;
			case 'E': i = HSSH((struct char_data *) vict_obj);
					break;
			case 'o': i = OBJN(obj, to); break;
			case 'O': i = OBJN((struct obj_data *) vict_obj, to);
					break;
			case 'p': i = OBJS(obj, to); break;
			case 'P': i = OBJS((struct obj_data *) vict_obj, to);
					break;
			case 'a': i = SANA(obj); break;
			case 'A': i = SANA((struct obj_data *) vict_obj);
					break;
			case 'T': i = (char *) vict_obj; break;
			case 'F': i = fname((char *) vict_obj); break;
			case '$': i = "$"; break;
			default:
			    log("Illegal $-code to act():");
			    log(str);
			    break;
		    }
		    while ( ( *point = *(i++) ) != '\0' )
			++point;
		    ++strp;
		}
		else if (!(*(point++) = *(strp++)))
		    break;

	    *(--point) = '\n';
	    *(++point) = '\r';
	    *(++point) = '\0';

	    write_to_q(CAP(buf), &to->desc->output);
	}
	if ((type == TO_VICT) || (type == TO_CHAR))
	    return;
    }
}



void night_watchman(void)
{
    long tc;
    struct tm *t_info;

    extern int diku_shutdown;

    void send_to_all(char *messg);

    tc = time(0);
    t_info = localtime(&tc);

    if ((t_info->tm_hour == 8) && (t_info->tm_wday > 0) &&
	(t_info->tm_wday < 6))
    {
	if (t_info->tm_min > 50)
	{
	    log("Leaving the scene for the serious folks.");
	    send_to_all("Closing down. Thank you for flying DikuMUD.\n\r");
	    diku_shutdown = 1;
	}
	else if (t_info->tm_min > 40)
	    send_to_all(
		"ATTENTION: Merc Mud will shut down in 10 minutes.\n\r");
	else if (t_info->tm_min > 30)
	    send_to_all("Warning: The game will close in 20 minutes.\n\r");
    }
}
