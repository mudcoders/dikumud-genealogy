#include <stdio.h>
#include <stdlib.h>

main()
{
   char m[200][1024];
   char buf[1024];

   int i, curr, min_pos, list_top = 0;
   int whitespace = 1;

   while (gets(buf)) {
      if (*buf == '#') {
	 if (!whitespace) {
	    list_top++;
	    *m[list_top] = '\0';
	 }
	 whitespace = 1;
	 continue;
      }

      whitespace = 0;
      strcat(m[list_top], buf);
      strcat(m[list_top], "\n");
   }

   for (curr = 0; curr < list_top; curr++) {
      min_pos = curr;
      for (i = curr+1; i <= list_top; i++)
	 if (strcmp(m[i], m[min_pos]) < 0)
	    min_pos = i;
      if (curr != min_pos) {
	 strcpy(buf, m[curr]);
	 strcpy(m[curr], m[min_pos]);
	 strcpy(m[min_pos], buf);
      }
   }

   for (i = 0; i <= list_top; i++) {
      puts(m[i]);
      printf("\n");
   }
}

