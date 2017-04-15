/* ************************************************************************
*  file: sign.c                                       Part of CircleMud   *
*  Usage: Present a message on a port					  *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
*  All Rights Reserved                                                    *
************************************************************************* */

/* max size of the sign you can put on the port */
#define TXT_BUF_SIZE	4095

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

void	watch(int port, char *text);
void	wave(int sock, char *text);
int	new_connection(int s);
int	init_socket(int port);
int	write_to_descriptor(int desc, char *txt);
void	nonblock(int s);




main(int argc, char **argv)
{
   int	port;
   char	txt[TXT_BUF_SIZE+1], buf[128];
   FILE * fl;

   if (argc != 3) {
      fprintf(stderr, "Usage: %s (<filename> | - ) <port #>\n", argv[0]);
      exit();
   }
   if (!strcmp(argv[1], "-")) {
      fl = stdin;
      puts("Input text (terminate with ^D)");
   } else if (!(fl = fopen(argv[1], "r"))) {
      perror(argv[1]);
      exit();
   }
   for (; ; ) {
      fgets(buf, 81, fl);
      if (feof(fl))
	 break;
      strcat(buf, "\r");
      if (strlen(buf) + strlen(txt) > TXT_BUF_SIZE) {
	 fputs("String too long.\n", stderr);
	 exit();
      }
      strcat(txt, buf);
   }
   if ((port = atoi(argv[2])) <= 1024) {
      fputs("Illegal port #\n", stderr);
      exit();
   }
   watch(port, txt);
}




void	watch(int port, char *text)
{
   int	mother;
   fd_set input_set;

   mother = init_socket(port);

   FD_ZERO(&input_set);
   for (; ; ) {
      FD_SET(mother, &input_set);
      if (select(64, &input_set, 0, 0, 0) < 0) {
	 perror("select");
	 exit();
      }
      if (FD_ISSET(mother, &input_set))
	 wave(mother, text);
   }
}



void	wave(int sock, char *text)
{
   int	s;

   if ((s = new_connection(sock)) < 0)
      return;

   write_to_descriptor(s, text);
   sleep(6);
   close(s);
}



int	new_connection(int s)
{
   struct sockaddr_in isa;
   /* struct sockaddr peer; */
   int	i;
   int	t;
   char	buf[100];

   i = sizeof(isa);
   getsockname(s, (struct sockaddr *)&isa, &i);


   if ((t = accept(s, (struct sockaddr *)(&isa), &i)) < 0) {
      perror("Accept");
      return(-1);
   }
   nonblock(t);

   /*
   i = sizeof(peer);
   if (!getpeername(t, &peer, &i)) {
      *(peer.sa_data + 49) = '\0';
      sprintf(buf, "New connection from addr %s\n", peer.sa_data);
      log(buf);
   }
   */

   return(t);
}




int	init_socket(int port)
{
   int	s;
   char	*opt;
   char	hostname[1024];
   struct sockaddr_in sa;
   struct hostent *hp;

   bzero(&sa, sizeof(struct sockaddr_in ));
   gethostname(hostname, 1023);
   hp = gethostbyname(hostname);
   if (hp == NULL) {
      perror("gethostbyname");
      exit();
   }
   sa.sin_family = hp->h_addrtype;
   sa.sin_port	 = htons(port);
   s = socket(AF_INET, SOCK_STREAM, 0);
   if (s < 0) {
      perror("Init-socket");
      exit();
   }
   if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR,
       (char *) & opt, sizeof (opt)) < 0) {
      perror ("setsockopt REUSEADDR");
      exit ();
   }

#ifdef USE_LINGER
   struct linger ld;
   ld.l_onoff = 1;
   ld.l_linger = 1000;
   if (setsockopt(s, SOL_SOCKET, SO_LINGER, &ld, sizeof(ld)) < 0) {
      perror("setsockopt LINGER");
      exit();
   }
#endif

   if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
      perror("bind");
      close(s);
      exit();
   }
   listen(s, 5);
   return(s);
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
