
/* Written to get the names of imms and there levels from the players
   "*.fng" file.  needs to be in players directory.
        -Decklarean
   God have mercy on your soul.  This is one long hack.
        -Decklarean
*/

#include<stdio.h>
#include<stdlib.h>

int main ( void )
{
 FILE *FngDirInput, *FngInput, *PfileInput;
 FILE *champion, *truststrange,
      *clan1,
      *clan2,
      *clan3,
      *clan4,
      *clan5,
      *clan6,
      *clan7,
      *clan8,
      *clan9,
      *clan10,
      *clan11,
      *level101,
      *level102,
      *level103,
      *level104,
      *level105,
      *level106,
      *level107,
      *level108;
 char fngfile[256];
 char garbage[256];
 char PfileName[256];
 char name[256];
 int  trust;
 int  clan;
 int  level;
 int  J;
 char S;
 int
     total1 =0,
     total2 =0,
     total3 =0,
     total4 =0,
     total5 =0,
     total6 =0,
     total7 =0,
     total8 =0,
     total9 =0,
     total10 =0,
     total11 =0;


 system ( "ls -R */*.fng > FNGDIRINFO" );

 if ( ( FngDirInput = fopen( "FNGDIRINFO", "rt" ) ) == NULL )
 {
  printf ( "Error: Can't open FNGDIRINFO file.\n" );
  fclose(FngDirInput);
  fclose(FngInput);
  fclose(PfileInput);
  exit(1);
 }
 champion = fopen ( "Champion.txt", "wt" );
 clan1 = fopen ( "clan1.tmpee", "wt" );
 fprintf ( clan1, "\n---Dark Servants\n\n" );
 clan2 = fopen ( "clan2.tmpee", "wt" );
 fprintf ( clan2, "\n---Elysium\n\n" );
 clan3 = fopen ( "clan3.tmpee", "wt" );
 fprintf ( clan3, "\n---Vendetta\n\n" );
 clan4 = fopen ( "clan4.tmpee", "wt" );
 fprintf ( clan4, "\n---Mystics\n\n" );
 clan5 = fopen ( "clan5.tmpee", "wt" );
 fprintf ( clan5, "\n---Grim Reapers\n\n" );
 clan6 = fopen ( "clan6.tmpee", "wt" );
 fprintf ( clan6, "\n---Head Honchos\n\n" );
 clan7 = fopen ( "clan7.tmpee", "wt" );
 fprintf ( clan7, "\n---Clan of One\n\n" );
 clan8 = fopen ( "clan8.tmpee", "wt" );
 fprintf ( clan8, "\n---Incarnate\n\n" );
 clan9 = fopen ( "clan9.tmpee", "wt" );
 fprintf ( clan9, "\n---Special Forces\n\n" );
 clan10 = fopen ( "clan10.tmpee", "wt" );
 fprintf ( clan10, "\n---Curators\n\n" );
 clan11 = fopen ( "clan11.tmpee", "wt" );
 fprintf ( clan11, "\n---Merry Pirates\n\n" );

 level101 = fopen ( "level101.tmpee", "wt" );
 fprintf( level101, "\n---Avatar\n\n" );
 level102 = fopen ( "level102.tmpee", "wt" );
 fprintf( level102, "\n---Immortals\n\n" );
 level103 = fopen ( "level103.tmpee", "wt" );
 fprintf( level103, "\n---Builders\n\n" );
 level104 = fopen ( "level104.tmpee", "wt" );
 fprintf( level104, "\n---Gods\n\n" );
 level105 = fopen ( "level105.tmpee", "wt" );
 fprintf( level105, "\n---Dieties\n\n" );
 level106 = fopen ( "level106.tmpee", "wt" );
 fprintf( level106, "\n---Guardians\n\n" );
 level107 = fopen ( "level107.tmpee", "wt" );
 fprintf( level107, "\n---Council\n\n" );
 level108 = fopen ( "level108.tmpee", "wt" );
 fprintf( level108, "\n---Implementors\n\n" );
 truststrange = fopen ( "truststrange.tmpee", "wt" );
 fprintf( truststrange, "\n---Trusted nonimmortals\n\n" );
 pclist = fopen ( "pclist", "wt" );

 printf("Building ImmsLevels.txt, ClanList.txt, Champion.txt\n");
 while ( fscanf( FngDirInput, "%s", &fngfile ) != EOF )
 {
  if ( ( FngInput = fopen( fngfile, "rt" ) ) == NULL )
  {
   printf ( "Error: Unable to open %s.\n", fngfile );
   fclose(FngDirInput);
   fclose(FngInput);
   fclose(PfileInput);
   exit(1);
  }

  fscanf( FngInput, "%s", &garbage ); /*name:*/
  fscanf( FngInput, "%s", &name );    /*player name*/
  fscanf( FngInput, "%s", &garbage ); /*mud*/
  fscanf( FngInput, "%s", &garbage ); /*age*/
  fscanf( FngInput, "%s", &garbage ); /*player age*/
  fscanf( FngInput, "%s", &garbage ); /*level:*/

/*  fscanf( FngInput, "%d", &level   ); *//*player level*/
  
  /*got what we want so close the file*/
  fclose(FngInput);

/*  printf( "%s: %d\n", name, level); */ 

/*  if ( level == 100 )
    fprintf( champion, "%s\n", name);
  if ( level == 106 )
    fprintf( level101, "%s: %d\n", name, level );
  if ( level == 107 )
    fprintf( level102, "%s: %d\n", name, level );
  if ( level == 108 )
    fprintf( level103, "%s: %d\n", name, level );
  if ( level == 109 )
    fprintf( level104, "%s: %d\n", name, level );
  if ( level == 110 )
    fprintf( level105, "%s: %d\n", name, level );
  if ( level == 111 )
    fprintf( level106, "%s: %d\n", name, level );
  if ( level == 112 )
    fprintf( level107, "%s: %d\n", name, level );
  if ( level == 113 )
    fprintf( level108, "%s: %d\n", name, level );
*/
/*  if (strstr(fngfile, "A"))
   strcpy (PfileName, "a");
  if (strstr(fngfile, "B"))
   strcpy (PfileName, "b");
  if (strstr(fngfile, "C"))
   strcpy (PfileName, "c");
  if (strstr(fngfile, "D"))
   strcpy (PfileName, "d");
  if (strstr(fngfile, "E"))
   strcpy (PfileName, "e");
  if (strstr(fngfile, "F"))
   strcpy (PfileName, "f");
  if (strstr(fngfile, "G"))
   strcpy (PfileName, "g");
  if (strstr(fngfile, "H"))
   strcpy (PfileName, "h");
  if (strstr(fngfile, "I"))
   strcpy (PfileName, "i");
  if (strstr(fngfile, "J"))
   strcpy (PfileName, "j");
  if (strstr(fngfile, "K"))
   strcpy (PfileName, "k");
  if (strstr(fngfile, "L"))
   strcpy (PfileName, "l");
  if (strstr(fngfile, "M"))
   strcpy (PfileName, "m");
  if (strstr(fngfile, "N"))
   strcpy (PfileName, "n");
  if (strstr(fngfile, "O"))
   strcpy (PfileName, "o");
  if (strstr(fngfile, "P"))
   strcpy (PfileName, "p");
  if (strstr(fngfile, "Q"))
   strcpy (PfileName, "q");
  if (strstr(fngfile, "R"))
   strcpy (PfileName, "r");
  if (strstr(fngfile, "S"))
   strcpy (PfileName, "s");
  if (strstr(fngfile, "T"))
   strcpy (PfileName, "t");
  if (strstr(fngfile, "U"))
   strcpy (PfileName, "u");
  if (strstr(fngfile, "V"))
   strcpy (PfileName, "v");
  if (strstr(fngfile, "W"))
   strcpy (PfileName, "w");
  if (strstr(fngfile, "X"))
   strcpy (PfileName, "x");
  if (strstr(fngfile, "Y"))
   strcpy (PfileName, "y");
  if (strstr(fngfile, "Z"))
   strcpy (PfileName, "z");
  strcat(PfileName, "/" );
*/

  J = strlen( fngfile );
 
  fngfile[J-1] = ' ';
  fngfile[J-2] = ' ';
  fngfile[J-3] = ' ';
  fngfile[J-4] = '\0';
 
  strcpy (PfileName, fngfile);

/*  strcat(PfileName, name);*/

/*  printf( "??? %s \n", PfileName ); */
 
  if ( ( PfileInput = fopen( PfileName, "rt" ) ) == NULL )
  {
   printf ( "Error: Can't open pfile %s.\n", PfileName );
   fclose(FngDirInput);
   fclose(FngInput);
   fclose(PfileInput);
   exit(1);
  }

  clan = 0; /* default the clan to 0 */

  while (strcmp ( garbage, "Nm" ) )
   fscanf( PfileInput, "%s", &garbage );
  
  fscanf( PfileInput, "%s", &name );

  /* find the clan information */
  while ( strcmp ( garbage , "Clan" ) )
   fscanf( PfileInput, "%s", &garbage );

  /* get the clan */
  fscanf( PfileInput, "%d", &clan );


  while ( strcmp ( garbage, "Lvl" ) )
   fscanf( PfileInput, "%s", &garbage );

  fscanf( PfileInput, "%d", &level );

  while ( strcmp ( garbage, "Trst" ) )
   fscanf( PfileInput, "%s", &garbage );

  fscanf( PfileInput, "%d", &trust );

  if ( level == 106 )
    fprintf( level101, "%s: %d, %d\n", name, level,trust );
  else if ( level == 107 )
    fprintf( level102, "%s: %d, %d\n", name, level, trust );
  else if ( level == 108 )
    fprintf( level103, "%s: %d, %d\n", name, level, trust );
  else if ( level == 109 )
    fprintf( level104, "%s: %d, %d\n", name, level, trust );
  else if ( level == 110 )
    fprintf( level105, "%s: %d, %d\n", name, level, trust );
  else if ( level == 111 )
    fprintf( level106, "%s: %d, %d\n", name, level, trust );
  else if ( level == 112 )
    fprintf( level107, "%s: %d, %d\n", name, level, trust );
  else if ( level == 113 )
    fprintf( level108, "%s: %d, %d\n", name, level, trust );
  else if ( trust != 0 )
    fprintf( truststrange, "%s: %d, %d\n", name, level, trust );


  /* find the clan information */
/*  while ( strcmp ( garbage , "Clan" ) )
   fscanf( PfileInput, "%s", &garbage );
*/
  /* get the clan */
/*  fscanf( PfileInput, "%d", &clan );
*/
  /* got what we want so close the file */
  fclose( PfileInput );
  if ( clan == 1 )
   {
    fprintf( clan1, "%s: %d\n", name, level);
    /*if ( level < 101 )*/ total1++; 
   }
  if ( clan == 2 )
   {
    fprintf( clan2, "%s: %d\n", name, level);
   /* if ( level < 101 )*/ total2++; 
   }
  if ( clan == 3 )
   {
    fprintf( clan3, "%s: %d\n", name, level);
   /* if ( level < 101 )*/ total3++; 
   }
  if ( clan == 4 )
   {
    fprintf( clan4, "%s: %d\n", name, level);
   /* if ( level < 101 )*/ total4++; 
   }
  if ( clan == 5 )
   {
    fprintf( clan5, "%s: %d\n", name, level);
   /* if ( level < 101 )*/ total5++; 
   }
  if ( clan == 6 )
   {
    fprintf( clan6, "%s: %d\n", name, level);
   /*  if ( level < 101 )*/ total6++; 
   }
  if ( clan == 7 )
   {
    fprintf( clan7, "%s: %d\n", name, level);
   /* if ( level < 101 ) */ total7++; 
   }
  if ( clan == 8 )
   {
    fprintf( clan8, "%s: %d\n", name, level);
   /* if ( level < 101 )*/ total8++; 
   }
  if ( clan == 9 )
   {
    fprintf( clan9, "%s: %d\n", name, level);
 /*   if ( level < 101 )*/ total9++; 
   }
  if ( clan == 10 )
   {
    fprintf( clan10, "%s: %d\n", name, level);
  /*  if ( level < 101 )*/ total10++; 
   }
  if ( clan == 11 )
   {
    fprintf( clan11, "%s: %d\n", name, level);
 /*   if ( level < 101 ) */ total11++; 
   }
   
 }

 
 fprintf( clan1, "Total members: %d\n", total1 );
 fprintf( clan2, "Total members: %d\n", total2 );
 fprintf( clan3, "Total members: %d\n", total3 );
 fprintf( clan4, "Total members: %d\n", total4 );
 fprintf( clan5, "Total members: %d\n", total5 );
 fprintf( clan6, "Total members: %d\n", total6 );
 fprintf( clan7, "Total members: %d\n", total7 );
 fprintf( clan8, "Total members: %d\n", total8 );
 fprintf( clan9, "Total members: %d\n", total9 );
 fprintf( clan10, "Total members: %d\n", total10 );
 fprintf( clan11, "Total members: %d\n", total11 );

 fprintf( clan11, "\n\n\n\n****Clan Member Totals****\n" );
 fprintf( clan11, "Total members 1: %d\n", total1 );
 fprintf( clan11, "Total members 2: %d\n", total2 );
 fprintf( clan11, "Total members 3: %d\n", total3 );
 fprintf( clan11, "Total members 4: %d\n", total4 );
 fprintf( clan11, "Total members 5: %d\n", total5 );
 fprintf( clan11, "Total members 6: %d\n", total6 );
 fprintf( clan11, "Total members 7: %d\n", total7 );
 fprintf( clan11, "Total members 8: %d\n", total8 );
 fprintf( clan11, "Total members 9: %d\n", total9 );
 fprintf( clan11, "Total members 10: %d\n", total10 );
 fprintf( clan11, "Total members 11: %d\n", total11 );


 fclose(FngDirInput);
 system( "rm FNGDIRINFO" );
/* printf( " hello \n\n\n\n\n" );*/
 fclose( level101);
 fclose( level102);
 fclose( level103);
 fclose( level104);
 fclose( level105);
 fclose( level106);
 fclose( level107);
 fclose( level108);
 fclose( truststrange) ;
 fclose( champion );
 fclose( clan1);
 fclose( clan2);
 fclose( clan3);
 fclose( clan4);
 fclose( clan5);
 fclose( clan6);
 fclose( clan7);
 fclose( clan8);
 fclose( clan9);
 fclose( clan10);
 fclose( clan11);

 system ( "cat level108.tmpee level107.tmpee > build1.tmpee" );
 system ( "cat build1.tmpee level106.tmpee > build2.tmpee" );
 system ( "cat build2.tmpee level105.tmpee > build3.tmpee" );
 system ( "cat build3.tmpee level104.tmpee > build4.tmpee" );
 system ( "cat build4.tmpee level103.tmpee > build5.tmpee" );
 system ( "cat build5.tmpee level102.tmpee > build6.tmpee" ); 
 system ( "cat build6.tmpee level101.tmpee > build7.tmpee" );
 system ( "cat build7.tmpee truststrange.tmpee > ImmsLevels.txt" );
 system ( "cat clan1.tmpee clan2.tmpee clan3.tmpee > build4.tmpee" );
 system ( "cat build4.tmpee clan4.tmpee clan5.tmpee > build5.tmpee" );
 system ( "cat build5.tmpee clan6.tmpee clan7.tmpee > build6.tmpee" );
 system ( "cat build6.tmpee clan8.tmpee clan9.tmpee > build7.tmpee" );
 system ( "cat build7.tmpee clan10.tmpee clan11.tmpee > ClanList.txt" );
 system ( "rm *.tmpee" ); 
 printf( "Done.\n" );
 return 0;
}
