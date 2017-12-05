/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *  Mitchell Tse.                                                          *
 *                                                                         *
 *  EnvyMud 2.2 improvements copyright (C) 1996, 1997 by Michael Quan.     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 ***************************************************************************/

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void string_edit( CHAR_DATA *ch, char **pString )
{
    send_to_char( "{o{bBegin entering your text now (.h = help .s = show .c = clear @ = save){x\n\r", ch );
    send_to_char( "{o{b-----------------------------------------------------------------------{x\n\r", ch );

    if ( !*pString )
        *pString = str_dup( "" );
    else
        **pString = '\0';

    ch->desc->str_editing = pString;

    return;
}


void string_append( CHAR_DATA *ch, char **pString )
{
    send_to_char( "{o{bBegin entering your text now (.h = help .s = show .c = clear @ = save){x\n\r", ch );
    send_to_char( "{o{b-----------------------------------------------------------------------{x\n\r", ch );

    if ( !*pString )
        *pString = str_dup( "" );
    send_to_char( *pString, ch );
    
    if ( *(*pString + strlen( *pString ) - 1) != '\r' )
    send_to_char( "\n\r", ch );

    ch->desc->str_editing = pString;

    return;
}


/*
 * Name:	string_replace
 * Purpose:	Substitutes one string for another.
 * Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 */
char * string_replace( char * orig, char * old, char * new )
{
    char xbuf [ MAX_STRING_LENGTH ];
    int i;

    xbuf[0] = '\0';
    strcpy( xbuf, orig );
    if ( strstr( orig, old ) )
    {
        i = strlen( orig ) - strlen( strstr( orig, old ) );
        xbuf[i] = '\0';
        strcat( xbuf, new );
        strcat( xbuf, &orig[i+strlen( old )] );
        free_string( orig );
    }

    return str_dup( xbuf );
}


/*
 * Name:	string_delete_last_line (Made by Zen)
 * Purpose:	Deletes the last line from a string.
 * Called by:	string_add (string.c)
 */
char * string_delete_last_line( char * old )
{
    char *ptr;
    char  buf [ MAX_STRING_LENGTH ];

    strcpy( buf, old );
    ptr = strrchr( buf, '\n' );
    if ( ptr )
	*ptr = '\0';
    ptr = strrchr( buf, '\n' );
    if ( ptr )
	*(ptr + 2) = '\0';
    else
	buf[0] = '\0';

    free_string( old );

    return ( str_dup( buf ) );
}

/*****************************************************************************
 Name:		string_add
 Purpose:	Interpreter for string editing.
 Called by:	game_loop_xxxx(comm.c).
 ****************************************************************************/
void string_add( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    /*
     * Thanks to James Seng
     */
    smash_tilde( argument );

    if ( *argument == '.' )
    {
        char arg1 [ MAX_INPUT_LENGTH ];
        char arg2 [ MAX_INPUT_LENGTH ];
        char arg3 [ MAX_INPUT_LENGTH ];

        argument = one_argument( argument, arg1 );
        argument = first_arg( argument, arg2, FALSE );
        argument = first_arg( argument, arg3, FALSE );

        if ( !str_cmp( arg1, ".b" ) )
        {
            send_to_char( "Last line of string deleted.\n\r", ch );
            *ch->desc->str_editing = string_delete_last_line( *ch->desc->str_editing );
            return;
        }

        if ( !str_cmp( arg1, ".c" ) )
        {
            send_to_char( "String cleared.\n\r", ch );
            **ch->desc->str_editing = '\0';
            return;
        }

        if ( !str_cmp( arg1, ".s" ) )
        {
            send_to_char( "String so far:\n\r{o{y", ch );
            send_to_char( *ch->desc->str_editing, ch );
            send_to_char( "{x", ch );
            return;
        }

        if ( !str_cmp( arg1, ".r" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char(
                    "usage:  .r \"old string\" \"new string\"\n\r", ch );
                return;
            }

            *ch->desc->str_editing =
                string_replace( *ch->desc->str_editing, arg2, arg3 );
            sprintf( buf, "'%s' replaced with '%s'.\n\r", arg2, arg3 );
            send_to_char( buf, ch );
            return;
        }

        if ( !str_cmp( arg1, ".f" ) )
        {
            *ch->desc->str_editing = format_string( *ch->desc->str_editing );
            send_to_char( "String formatted.\n\r", ch );
            return;
        }
        
        if ( !str_cmp( arg1, ".h" ) )
        {
            send_to_char( "{o{wSedit help (commands on blank line):{x\n\r", ch );
            send_to_char( "{o{y.r {w'{yold{w' '{ynew{w'{x     Replace a substring (requires '', \"\").{x\n\r", ch );
            send_to_char( "{o{y.h            {x     Get help (this info).{x\n\r", ch );
            send_to_char( "{o{y.s            {x     Show string so far.{x\n\r", ch );
            send_to_char( "{o{y.f            {x     Word wrap string.{x\n\r", ch );
            send_to_char( "{o{y.c            {x     Clear string so far.{x\n\r", ch );
            send_to_char( "{o{y.b            {x     Delete last line.{x\n\r", ch );
            send_to_char( "{o{y@             {x     End string.{x\n\r", ch );
            return;
        }
            

        send_to_char( "SEdit:  Invalid dot command.\n\r", ch );
        return;
    }

    if ( *argument == '@' )
    {
        ch->desc->str_editing = NULL;
        return;
    }

    /*
     * Truncate strings to MAX_STRING_LENGTH.
     */
    if ( strlen( buf ) + strlen( argument ) >= ( MAX_STRING_LENGTH - 4 ) )
    {
        send_to_char( "String too long, last line skipped.\n\r", ch );

	/* Force character out of editing mode. */
        ch->desc->str_editing = NULL;
        return;
    }

    strcpy( buf, *ch->desc->str_editing );
    strcat( buf, argument );
    strcat( buf, "\n\r" );
    free_string( *ch->desc->str_editing );
    *ch->desc->str_editing = str_dup( buf );
    return;
}



/*
 *  Thanks to Kalgen for the new procedure (no more bug!)
 *  Original wordwrap() written by Surreality.
 */
/*****************************************************************************
 Name:		format_string
 Purpose:	Special string formating and word-wrapping.
 Called by:	string_add (string.c) (many) olc_act.c
 ****************************************************************************/
char *format_string( char *oldstring /*, bool fSpace */)
{
  char *rdesc;
  char  xbuf	[ MAX_STRING_LENGTH ];
  char  xbuf2	[ MAX_STRING_LENGTH ];
  int   i	= 0;
  bool  cap	= TRUE;
  
  xbuf[0] = xbuf2[0] = 0;
  
  i = 0;
  
  if ( strlen( oldstring ) >= ( MAX_STRING_LENGTH - 4 ) )
  {
     bug( "String to format_string() longer than MAX_STRING_LENGTH.", 0 );
     return ( oldstring );
  }

  for ( rdesc = oldstring; *rdesc; rdesc++ )
  {
    if ( *rdesc == '\n' )
    {
      if ( xbuf[i-1] != ' ' )
      {
        xbuf[i] = ' ';
        i++;
      }
    }
    else if ( *rdesc == '\r' ) ;
    else if ( *rdesc == ' ' )
    {
      if ( xbuf[i-1] != ' ' )
      {
        xbuf[i] = ' ';
        i++;
      }
    }
    else if ( *rdesc == ')' )
    {
      if ( xbuf[i-1] == ' ' && xbuf[i-2] == ' ' && 
          ( xbuf[i-3] == '.' || xbuf[i-3] == '?' || xbuf[i-3] == '!' ) )
      {
        xbuf[i-2] = *rdesc;
        xbuf[i-1] = ' ';
        xbuf[i] = ' ';
        i++;
      }
      else
      {
        xbuf[i]=*rdesc;
        i++;
      }
    }
    else if (*rdesc=='.' || *rdesc=='?' || *rdesc=='!') {
      if (xbuf[i-1]==' ' && xbuf[i-2]==' ' && 
          (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!')) {
        xbuf[i-2]=*rdesc;
        if (*(rdesc+1) != '\"')
        {
          xbuf[i-1]=' ';
          xbuf[i]=' ';
          i++;
        }
        else
        {
          xbuf[i-1]='\"';
          xbuf[i]=' ';
          xbuf[i+1]=' ';
          i+=2;
          rdesc++;
        }
      }
      else
      {
        xbuf[ i ] = *rdesc;
        if (*( rdesc + 1 ) != '\"' )
        {
          xbuf[ i+1 ] = ' ';
          xbuf[ i+2 ] = ' ';
          i += 3;
        }
        else
        {
          xbuf[ i+1 ] = '\"';
          xbuf[ i+2 ] = ' ';
          xbuf[ i+3 ] = ' ';
          i += 4;
          rdesc++;
        }
      }
      cap = TRUE;
    }
    else
    {
      xbuf[i] = *rdesc;
      if ( cap )
        {
          cap = FALSE;
          xbuf[i] = UPPER( xbuf[i] );
        }
      i++;
    }
  }
  xbuf[i] = 0;
  strcpy( xbuf2, xbuf );
  
  rdesc = xbuf2;
  
  xbuf[0] = 0;
  
  for ( ; ; )
  {
    for (i = 0; i < 77; i++ )
    {
      if ( !*( rdesc + i ) ) break;
    }
    if ( i < 77 )
    {
      break;
    }
    for ( i = ( xbuf[0] ? 76: 73 ) ; i ; i-- )
      if (*( rdesc + i ) == ' ') break;

    if ( i )
    {
      *( rdesc + i ) = 0;
      strcat( xbuf, rdesc );
      strcat( xbuf, "\n\r" );
      rdesc += i + 1;
      while ( *rdesc == ' ' ) rdesc++;
    }
    else
    {
      bug ( "No spaces", 0 );
      *( rdesc + 75 ) = 0;
      strcat( xbuf, rdesc );
      strcat( xbuf, "-\n\r" );
      rdesc += 76;
    }
  }
  while (*( rdesc + i ) && (*( rdesc + i ) == ' '||
                        *( rdesc + i ) == '\n'||
                        *( rdesc + i ) == '\r' ) )
    i--;
  *( rdesc + i + 1 ) = 0;
  strcat( xbuf, rdesc );
  if ( xbuf[ strlen( xbuf ) - 2 ] != '\n' )
    strcat( xbuf, "\n\r" );

  free_string( oldstring );
  return( str_dup( xbuf ) );
}



/*
 * Used above in string_add.  Because this function does not
 * modify case if fCase is FALSE and because it understands
 * parenthesis, it would probably make a nice replacement
 * for one_argument.
 */
/*****************************************************************************
 Name:		first_arg
 Purpose:	Pick off one argument from a string and return the rest.
 		Understands quates, parenthesis (barring ) ('s) and
 		percentages.
 Called by:	string_add(string.c)
 ****************************************************************************/
char *first_arg( char *argument, char *arg_first, bool fCase )
{
    char cEnd;

    while ( *argument == ' ' )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"'
      || *argument == '%'  || *argument == '(' )
    {
        if ( *argument == '(' )
        {
            cEnd = ')';
            argument++;
        }
        else cEnd = *argument++;
    }

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
    if ( fCase ) *arg_first = LOWER(*argument);
            else *arg_first = *argument;
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( *argument == ' ' )
	argument++;

    return argument;
}



/*
 * Used in olc_act.c for aedit_builders.
 */
char * string_unpad( char * argument )
{
    char buf[MAX_STRING_LENGTH];
    char *s;

    s = argument;

    while ( *s == ' ' )
        s++;

    strcpy( buf, s );
    s = buf;

    if ( *s != '\0' )
    {
        while ( *s != '\0' )
            s++;
        s--;

        while( *s == ' ' )
            s--;
        s++;
        *s = '\0';
    }

    free_string( argument );
    return str_dup( buf );
}



/*
 * Same as capitalize but changes the pointer's data.
 * Used in olc_act.c in aedit_builder.
 */
char * string_proper( char * argument )
{
    char *s;

    s = argument;

    while ( *s != '\0' )
    {
        if ( *s != ' ' )
        {
            *s = UPPER(*s);
            while ( *s != ' ' && *s != '\0' )
                s++;
        }
        else
            s++;
    }

    return argument;
}
