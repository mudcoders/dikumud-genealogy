/* Put in db.c */
/***************/


void load_newbie( void )
{
    FILE      *fp;
    NEWBIE_DATA *pNewbieIndex;
    char letter;

    if ( !( fp = fopen( NEWBIE_FILE, "r" ) ) )
        return;
    fpArea = fp;
    strcpy(strArea, NEWBIE_FILE);
    for ( ; ; )
    {
        char*  keyword;

        letter                          = fread_letter( fp );
        if ( letter != '#' )
        {
            bug( "Load_newbie: # not found.", 0 );
            continue;
        }
        keyword                         = fread_string( fp );
        if ( !str_cmp( name, "END"    ) )
          break;
	
 	pNewbieIndex		=	alloc_perm( sizeof( *pNewbieIndex ));
	pNewbieIndex->keyword		=	keyword;
	pNewbieIndex->answer1		=	fread_string( fp );
	pNewbieIndex->answer2		=	fread_string( fp );

	newbie_sort(pNewbieIndex);
	top_newbie++;

    }
    fclose ( fp );

    return;
}

void newbie_sort( NEWBIE_DATA *pNewbie )
{
  NEWBIE_DATA *fNewbie;

  if ( !newbie_first )
  {
    newbie_first = pNewbie;
    newbie_last  = pNewbie;
    return;
  }

  if ( strncmp( pNewbie->keyword, newbie_first->keyword, 256 ) > 0 )
  {
   pNewbie->next = newbie_first->next;
   newbie_first = pNewbie;
   return;
  }

  for ( fNewbie = newbie_first; fNewbie; fNewbie = fNewbie->next )
  {
    if (    ( strncmp( pNewbie->keyword, fNewbie->keyword, 256 ) < 0 ) )
    {
      if ( fNewbie != newbie_last )
      {
	pNewbie->next = fNewbie->next;
	fNewbie->next = pNewbie;
        return;
       }
    }
  }
  newbie_last->next = pNewbie;
  newbie_last = pNewbie;
  pNewbie->next = NULL;

  return;

}

