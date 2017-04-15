/*
Here's a little C program to list the names and numbers of the mobs and
objects in your Diku world.  I just needed something to make a quick
reference when I'm writing zone files.

Usage: namegrab myworld.mob or myworld.obj

Nick Borko
nborko@umiami.ir.miami.edu

   Utility to show the names and numbers of mobs and objects in the
   .mob and .obj files

   Program by Nick Borko at the University of Miami
   email nborko@umiami.ir.miami.edu

   For use with DikuMud
*/

#include <stdio.h>

main(int argc, char *argv[])
{
	if(argc>1) {
		FILE *fp;
		if(fp = fopen(argv[1],"r")) {
			char line[256];
			while(!feof(fp)) {
				fgets(line,255,fp);
				if(line[0]=='#') {
					int i;
					for(i=0;line[i];i++)
						if(line[i]=='\n') line[i]=0;
					printf("%s: ",line);
					fgets(line,255,fp);
					fgets(line,255,fp);
					for(i=0;line[i];i++)
						if(line[i]=='~') line[i]=' ';
					printf(line);
				}
			}
			fclose(fp);
		} else printf("Could not open: %s\n\r",argv[1]);
	} else printf("Usage: namegrab <file>\n\r");
}
