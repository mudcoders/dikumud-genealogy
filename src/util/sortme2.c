#include <stdio.h>
#include <stdlib.h>

#define SZ (1024+2048)
main()
{
   char m[200][SZ];
   char buf[SZ], *a, *b;

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
      for (i = curr+1; i <= list_top; i++) {
	 a = m[i];  if (*a == '"') a++;
	 b = m[min_pos];  if (*b == '"') b++;
	 if (strcmp(a, b) < 0)
	    min_pos = i;
      }
      if (curr != min_pos) {
	 strcpy(buf, m[curr]);
	 strcpy(m[curr], m[min_pos]);
	 strcpy(m[min_pos], buf);
      }
   }

   for (i = 0; i <= list_top; i++) {
      m[i][strlen(m[i])-1] = '\0';
      puts(m[i]);
      printf("#\n");
   }
}

