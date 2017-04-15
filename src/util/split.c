/* ************************************************************************
*  file:  split.c                                     Part of CircleMud   *
*  Usage: split one large file into multiple smaller ones, with index     *
*  Written by Jeremy Elson                                                *
*  All Rights Reserved                                                    *
*  Copyright (C) 1993 The Trustees of The Johns Hopkins University        *
************************************************************************* */


#define INDEX_NAME "index"
#define BSZ 256
#define MAGIC_CHAR '='

#include <stdio.h>
#include <string.h>

main () 
{
   char	line[BSZ+1];
   FILE * index = 0, *outfile = 0;

   if (!(index = fopen(INDEX_NAME, "w"))) {
      perror("error opening index for write");
      exit();
   }

   while (fgets(line, BSZ, stdin)) {
      if (*line == MAGIC_CHAR) {
	 *(strchr(line, '\n')) = '\0';
	 if (outfile) {
	    fputs("#99999\n$~\n", outfile);
	    fclose(outfile);
	 }
	 if (!(outfile = fopen((line + 1), "w"))) {
	    perror("Error opening output file");
	    exit();
	 }
	 fputs(line + 1, index);
	 fputs("\n", index);
      } else if (outfile)
	 fputs(line, outfile);
   }

   fputs("$\n\r", index);
   fclose(index);
   if (outfile)
      fclose(outfile);
}


