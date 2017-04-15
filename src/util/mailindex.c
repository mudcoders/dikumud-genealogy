/* ************************************************************************
*  file:  mailindex.c                                 Part of CircleMud   *
*  Usage: list all pieces of mail in a player mail file                   *
*  Written by Jeremy Elson                                                *
*  All Rights Reserved                                                    *
*  Copyright (C) 1993 The Trustees of The Johns Hopkins University        *
************************************************************************* */


#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "../mail.h"

#define log(x) puts(x)

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


void	push_free_list(long pos)
{
   position_list_type * new_pos;

   new_pos = (position_list_type * )malloc(sizeof(position_list_type));
   new_pos->position = pos;
   new_pos->next = free_list;
   free_list = new_pos;
}



long	pop_free_list(void)
{
   position_list_type * old_pos;
   long	return_value;

   if ((old_pos = free_list) != 0) {
      return_value = free_list->position;
      free_list = old_pos->next;
      free(old_pos);
      return return_value;
   } else
      return file_end_pos;
}




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

   if (!(mail_file = fopen(MAIL_FILE, "r+b"))) {
      perror("read_from_file (mail.c)");
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
      log("Mail file non-existant... creating new file.");
      mail_file = fopen(MAIL_FILE, "w");
      fclose(mail_file);
      return 1;
   }

   while (fread(&next_block, sizeof(header_block_type), 1, mail_file)) {
      if (next_block.block_type == HEADER_BLOCK) {
	 index_mail(next_block.to, block_num * BLOCK_SIZE);
	 total_messages++;
      } else if (next_block.block_type == DELETED_BLOCK)
	 push_free_list(block_num * BLOCK_SIZE);
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



int	main(int argc, char *argv[])
{
   char	searchee[NAME_SIZE+1], *ptr;
   mail_index_type * i1;
   position_list_type * i2;
   header_block_type header;

   if (argc != 2) {
      fprintf(stderr, "%s <filename>\n", argv[0]);
      exit(1);
   }

   strcpy(MAIL_FILE, argv[1]);
   scan_file();

   for (i1 = mail_index; i1; i1 = i1->next) {
      printf("%s\n", i1->recipient);
      for (i2 = i1->list_start; i2; i2 = i2->next) {
	 read_from_file(&header, BLOCK_SIZE, i2->position);
	 printf("\t%s\n", header.from);
      }
   }

   exit(0);
}


