#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

/*
  This program was written to delete pfiles older than 30 days.
  -Decklarean...

Decklarean looks at you.
Decklarean shrugs.

*/

int done ;

int dayofyear ( int date, char month[])
{
 int total = 0;

 if (!strcmp( month, "JAN" ) || !strcmp( month, "Jan" ) )
  total = 0;
 else if (!strcmp( month, "FEB") || !strcmp( month, "Feb" ) )
  total = 31;
 else if (!strcmp( month, "MAR") || !strcmp( month, "Mar" ) )
  total = 60;
 else if (!strcmp( month, "APR") || !strcmp( month, "Apr" ) )
  total = 91;
 else if (!strcmp( month, "MAY") || !strcmp( month, "May" ) )
  total = 121;
 else if (!strcmp( month, "JUN") || !strcmp( month, "Jun" ) )
  total = 152;
 else if (!strcmp( month, "JUL") || !strcmp( month, "Jul" ) )
  total = 172;
 else if (!strcmp( month, "AUG") || !strcmp( month, "Aug" ) )
  total = 203;
 else if (!strcmp( month, "SEP") || !strcmp( month, "Sep" ) )
  total = 234;
 else if (!strcmp( month, "OCT") || !strcmp( month, "Oct" ) )
  total = 264;
 else if (!strcmp( month, "NOV") || !strcmp( month, "Nov" ) )
  total = 295;
 else if (!strcmp( month, "DEC") || !strcmp( month, "Dec" ) )
  total = 325;
 else
  {
    printf("Error: Oh oh.  The month abrevations isn't in the list: %s\n", 
month);
    system( "rm DIRINFOFILE" );
    exit(1);
  }

 total = total + date;  /*  add on the days of the current month*/

 return total;
} 

struct filedata
 {
  int date;
  char month[4];
  char filename[256];  
  int checked;
  int type;
 };

void getfiledata( FILE *input, struct filedata *thefile )
{
 char garbage[80];
 if ( fscanf( input, "%s", &garbage ) != EOF )
 {
  fscanf( input, "%s", &garbage );
  while ( !strcmp ( garbage, "total" ) )
/*  if ( !strcmp ( garbage, "total" ) ) */
   {
     fscanf( input, "%s", &garbage );
     fscanf( input, "%s", &garbage );
     fscanf( input, "%s", &garbage );
   }
    /* toss out the info that don't matter*/
  fscanf( input, "%s", &garbage );
  fscanf( input, "%s", &garbage );
  fscanf( input, "%s", &garbage );
  fscanf( input, "%s", &thefile->month );
  fscanf( input, "%d", &thefile->date  );
  fscanf( input, "%s", &garbage  );
  fscanf( input, "%s", &thefile->filename );
  if (strstr( thefile->filename, ".gz") )
   thefile->type = 1 ;
  else if (strstr(thefile->filename, ".fng" ) )
   thefile->type = 2 ;
  else if (strstr(thefile->filename, ".cps" ) )
   thefile->type = 3 ;
  else
   thefile->type = 4 ;
 }
 else
  done = 1;
}

int main ( void )
{
 char DIR;
 char dir[1];
 int choice;
 struct filedata file1, file2, file3, file4;
 FILE *input;
 FILE *fingerfile;
 time_t current_date;
 char cur_month[3];
 int cur_date;
 char garbage[80];
 int  total, j;
 char filename[256];
 char FNG[256];
 char CPS[256];
 char GZ[256];
 char * CN; 

 done = 0;

 system( "date > TMPFILER1" );
 system( "ls -FRlr > TMPFILER2");
 system( "cat TMPFILER1 TMPFILER2 > DIRINFOFILE" );
 system( "rm TMPFILER1" );
 system( "rm TMPFILER2"	);

 if ( (input = fopen( "DIRINFOFILE", "rt" ) ) == NULL )
  {
   printf( "Cannot open file with directory information.\n");
   return 1;
  }

 fscanf( input, "%s", &garbage );
 fscanf( input, "%s", &cur_month );
 fscanf( input, "%d", &cur_date );
 fscanf( input, "%s", &garbage );
 fscanf( input, "%s", &garbage );
 fscanf( input, "%s", &garbage );

 time( &current_date );
 printf("%s\n", ctime(&current_date));
 /* tose out the total at the top of file */
 fscanf( input, "%s", &garbage );
 fscanf( input, "%s", &garbage );

 /*tose out everything in the player directory*/
 while ( strcmp ( garbage , "total" ) )
  fscanf( input, "%s", &garbage );

 /* toss out the total number */
 fscanf( input, "%s", &garbage );

file1.checked = 1;
file2.checked = 1;
file3.checked = 1;
file4.checked = 1;

while (!done)
 {
 if (file1.checked && file2.checked && file3.checked && file4.checked)
  {
   getfiledata( input, &file1);
   file1.checked = 0;
  }
 else
  {
   if (file1.checked && !file2.checked) 
    {
     file1 = file2;
     file2 = file3;
     file3 = file4;
     file4.checked = 1;
    }
   if (file2.checked && !file3.checked)
    {
      file2 = file3;
      file3 = file4;
      file4.checked = 1;
    }
   if (file3.checked && !file4.checked)
    {
      file3 = file4;
      file4.checked = 1;
    }
  }

  /*used for debugging*/
  
  printf( "%s %d %s\n", file1.month, file1.date, file1.filename );
  printf( "%d %d %s\n", dayofyear(cur_date, cur_month),
                        dayofyear(file1.date,file1.month), file1.filename );
  printf( "file type: %d\n", file1.type );

  /*  32 days because that is what they wanted -Deck :> */
  if ((dayofyear(cur_date,cur_month) - dayofyear(file1.date,file1.month)) > 32 )
  {
   printf( "Name:%s\n", file1.filename);
   printf( "File last accesed:%s %d\n", file1.month, file1.date);
   printf( "Days old:%d\n",
           (dayofyear(cur_date, cur_month) - 
            dayofyear(file1.date, file1.month)) );
   printf( "Do you want to remove this file?(1 = Yes / 2 = No / 3 = Quit) " );
   choice = 3;
   scanf("%d", &choice);
   
   if ( choice == 1 )
    {
      strcpy(CPS, "rm ");
      if (strstr(file1.filename, "A"))
       strcat (CPS, "a");
      if (strstr(file1.filename, "B"))
       strcat (CPS, "b");
      if (strstr(file1.filename, "C"))
       strcat (CPS, "c");
      if (strstr(file1.filename, "D"))
       strcat (CPS, "d");
      if (strstr(file1.filename, "E"))
       strcat (CPS, "e");
      if (strstr(file1.filename, "F"))
       strcat (CPS, "f");
      if (strstr(file1.filename, "G"))
       strcat (CPS, "g");
      if (strstr(file1.filename, "H"))
       strcat (CPS, "h");
      if (strstr(file1.filename, "I"))
       strcat (CPS, "i");
      if (strstr(file1.filename, "J"))
       strcat (CPS, "j");
      if (strstr(file1.filename, "K"))
       strcat (CPS, "k");
      if (strstr(file1.filename, "L"))
       strcat (CPS, "l");
      if (strstr(file1.filename, "M"))
       strcat (CPS, "m");
      if (strstr(file1.filename, "N"))
       strcat (CPS, "n");
      if (strstr(file1.filename, "O"))
       strcat (CPS, "o");
      if (strstr(file1.filename, "P"))
       strcat (CPS, "p");
      if (strstr(file1.filename, "Q"))
       strcat (CPS, "q");
      if (strstr(file1.filename, "R"))
       strcat (CPS, "r");
      if (strstr(file1.filename, "S"))
       strcat (CPS, "s");
      if (strstr(file1.filename, "T"))
       strcat (CPS, "t");
      if (strstr(file1.filename, "U"))
       strcat (CPS, "u");
      if (strstr(file1.filename, "V"))
       strcat (CPS, "v");
      if (strstr(file1.filename, "W"))
       strcat (CPS, "w");
      if (strstr(file1.filename, "X"))
       strcat (CPS, "x");
      if (strstr(file1.filename, "Y"))
       strcat (CPS, "y");
      if (strstr(file1.filename, "Z"))
       strcat (CPS, "z");

     strcat(CPS, "/" );
     strcpy(FNG, CPS );   
     strcpy(GZ, CPS );     
/*pfile*/
     strcat(CPS, file1.filename);
     system( CPS );
/*ziped pfile*/
     strcat( GZ, file1.filename);
     strcat( GZ, ".gz");
     system ( GZ );
/*newcorpse file*/
     strcat(CPS, ".cps");
     system( CPS );
/* finger file */
     strcat(FNG, file1.filename);
     strcat(FNG, ".fng");
     system( FNG );
     printf ( "\nDeleted.\n\n" );
   
    
    }
   else if ( choice == 2 )
    {
     printf ( "\nCanceled Delete.\n\n" );
    }
   else
    {
     fclose(input);
     system("rm DIRINFOFILE" );
     exit(0);
    }
  }
   if (file2.checked)
   {
    getfiledata( input, &file2);
    file2.checked = 0;
   }
   if (file3.checked)
   {
    getfiledata( input, &file3);
    file3.checked = 0;
   }
   if (file4.checked)
   {
    getfiledata( input, &file4);
    file4.checked = 0;
   }
   strcpy(CN, file1.filename);
   if (file1.type != 4)
    CN = strtok( CN, "." );
   strcpy(CPS, CN);
   strcat(CPS, ".cps");
   strcpy(FNG, CN);
   strcat(FNG, ".fng");
   strcat(GZ, CN);
   strcat(GZ, ".gz");
   if (!strcmp(file2.filename, CPS) ||
       !strcmp(file2.filename, FNG) ||
       !strcmp(file2.filename, GZ)    )
    file2.checked = 1;
   if (!strcmp(file3.filename, CPS) ||
       !strcmp(file3.filename, FNG) ||
       !strcmp(file3.filename, GZ)    )
    file3.checked = 1;
   if (!strcmp(file4.filename, CPS) ||
       !strcmp(file4.filename, FNG) ||
       !strcmp(file4.filename, GZ)    )
    file4.checked = 1;


  file1.checked = 1;
 }
 fclose( input );
 system( "rm DIRINFOFILE" );
 return 0;
}

