/* ************************************************************************
*   File: signals.c                                     Part of CircleMUD *
*  Usage: Signal trapping and signal handlers                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "structs.h"
#include "utils.h"

extern struct descriptor_data *descriptor_list;
extern int	mother_desc;

void	checkpointing();
void	logsig();
void	hupsig();
/* void	badcrash(); */
void	unrestrict_game();
void	reread_wizlists();

int	graceful_tried = 0;

void	signal_setup(void)
{
   struct itimerval itime;
   struct timeval interval;

   signal(SIGUSR1, reread_wizlists);
   signal(SIGUSR2, unrestrict_game);

   /* just to be on the safe side: */

   signal(SIGHUP, hupsig);
   signal(SIGPIPE, SIG_IGN);
   signal(SIGINT, hupsig);
   signal(SIGALRM, logsig);
   signal(SIGTERM, hupsig);
/*
   signal(SIGSEGV, badcrash);
   signal(SIGBUS, badcrash);
*/
   /* set up the deadlock-protection */

   interval.tv_sec = 900;    /* 15 minutes */
   interval.tv_usec = 0;
   itime.it_interval = interval;
   itime.it_value = interval;
   setitimer(ITIMER_VIRTUAL, &itime, 0);
   signal(SIGVTALRM, checkpointing);
}



void	checkpointing(void)
{
   extern int	tics;

   if (!tics) {
      log("CHECKPOINT shutdown: tics not updated");
      abort();
   } else
      tics = 0;
}


void	reread_wizlists()
{
   void	reboot_wizlists(void);

   signal(SIGUSR1, reread_wizlists);
   mudlog("Rereading wizlists.", CMP, LEVEL_IMMORT, FALSE);
   reboot_wizlists();
}


void	unrestrict_game()
{
   extern int	restrict;
   extern struct ban_list_element *ban_list;
   extern int	num_invalid;

   signal(SIGUSR2, unrestrict_game);
   mudlog("Received SIGUSR2 - unrestricting game (emergent)",
	  BRF, LEVEL_IMMORT, TRUE);
   ban_list = 0;
   restrict = 0;
   num_invalid = 0;
}





/* kick out players etc */
void	hupsig(void)
{
   log("Received SIGHUP, SIGINT, or SIGTERM.  Shutting down...");
   exit(0);   /* something more elegant should perhaps be substituted */
}


void	badcrash(void)
{
   void	close_socket(struct descriptor_data *d);
   struct descriptor_data *desc;

   log("SIGSEGV or SIGBUS received.  Trying to shut down gracefully.");
   if (!graceful_tried) {
      close(mother_desc);
      log("Trying to close all sockets.");
      graceful_tried = 1;
      for (desc = descriptor_list; desc; desc = desc->next)
	 close(desc->descriptor);
   }
   abort();
}


void	logsig(void)
{
   log("Signal received.  Ignoring.");
}


