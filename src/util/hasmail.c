/* ************************************************************************
*  file:  readmail.c                                  Part of CircleMud   *
*  Usage: read mail in a player mail file without removing it from file   *
*  Written by Jeremy Elson                                                *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
*  All Rights Reserved                                                    *
*  Copyright (C) 1993 The Trustees of The Johns Hopkins University        *
**************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "../mail.h"

#define log(x)

/* defines for fseek */
#ifndef SEEK_SET
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2
#endif

char	MAIL_FILE[100];
mail_index_type		*mail_index = 0; /* list of recs in the mail file  */
position_list_type 	*free_list = 0;  /* list of free positions in file */
long	file_end_pos = 0; /* length of file */



mail_index_type *find_char_in_index(char *searchee)
{
   mail_index_type * temp_rec;

   if (!*searchee) {
      log("Mail system -- non fatal error 1");
      return 0;
   }

   for (temp_rec = mail_index; 
       (temp_rec && strcmp(temp_rec->recipient, searchee)); 
       temp_rec = temp_rec->next)
      ;

   return temp_rec;
}





void	read_from_file(void *buf, int size, long filepos)
{
   FILE * mail_file;

   if (!(mail_file = fopen(MAIL_FILE, "r"))) {
      perror("Error opening mail file for read");
      exit(1);
   }


   if (filepos % BLOCK_SIZE) {
      log("Mail system -- fatal error #2!!!");
      return;
   }

   fseek(mail_file, filepos, SEEK_SET);
   fread(buf, size, 1, mail_file);
   fclose(mail_file);
   return;
}




void	index_mail(char *name_to_index, long pos)
{
   mail_index_type     * new_index;
   position_list_type * new_position;

   if (!*name_to_index) {
      log("Mail system -- non-fatal error 2");
      return;
   }

   if (!(new_index = find_char_in_index(name_to_index))) {
      /* name not already in index.. add it */
      new_index = (mail_index_type * )malloc(sizeof(mail_index_type));
      strncpy(new_index->recipient, name_to_index, NAME_SIZE);
      new_index->recipient[strlen(name_to_index)] = '\0';
      new_index->list_start = 0;

      /* add to front of list */
      new_index->next = mail_index;
      mail_index = new_index;
   }

   /* now, add this position to front of position list */
   new_position = (position_list_type * )malloc(sizeof(position_list_type));
   new_position->position = pos;
   new_position->next = new_index->list_start;
   new_index->list_start = new_position;
}


/* SCAN_FILE */
/* scan_file is called once during boot-up.  It scans through the mail file
   and indexes all entries currently in the mail file. */
int	scan_file(void)
{
   FILE 		   * mail_file;
   header_block_type  next_block;
   int	total_messages = 0, block_num = 0;
   char	buf[100];

   if (!(mail_file = fopen(MAIL_FILE, "r"))) {
      perror("Error opening mail file for read");
      exit(1);
   }

   while (fread(&next_block, sizeof(header_block_type), 1, mail_file)) {
      if (next_block.block_type == HEADER_BLOCK) {
	 index_mail(next_block.to, block_num * BLOCK_SIZE);
	 total_messages++;
      }
      block_num++;
   }

   file_end_pos = ftell(mail_file);
   sprintf(buf, "   %ld bytes read.", file_end_pos);
   log(buf);
   if (file_end_pos % BLOCK_SIZE) {
      log("Error booting mail system -- Mail file corrupt!");
      log("Mail disabled!");
      return 0;
   }
   sprintf(buf, "   Mail file read -- %d messages.", total_messages);
   log(buf);
   return 1;
} /* end of scan_file */


/* HAS_MAIL */
/* a simple little function which tells you if the guy has mail or not */
int	has_mail(char *recipient)
{
   if (find_char_in_index(recipient))
      return 1;
   return 0;
}




int	main(int argc, char *argv[])
{
   char	searchee[NAME_SIZE+1], *ptr;

   if (argc != 3) {
      fprintf(stderr, "Usage: %s <filename> <plrname>\n", argv[0]);
      exit(1);
   }

   strcpy(MAIL_FILE, argv[1]);
   scan_file();
   strcpy(searchee, argv[2]);
   if (!has_mail(searchee)) {
      exit(1);
   }

   exit(0);

}


