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
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#define unix 1
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
/* local functions */
void ban	args( ( CHAR_DATA *ch, char *argument, char ban_type ) );

/* Conversion of Immortal powers to Immortal skills done by Thelonius */

extern char * mprog_type_to_name  args ( ( int type ) );
extern bool   can_use_cmd args( ( int cmd, CHAR_DATA *ch, int trust) );
extern void   delete_playerlist   args ( ( char *name ) );

/* Text file output by Thal. Need to change the directories once the mud is open */

void do_ideas( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;   

  if ( !( fp = fopen( "/home/storm/EOSBUILD/area/ideas.txt", "r" ) ) )
   return;

  while ( fgets( buf, MAX_STRING_LENGTH, fp ) != NULL )
  {
    send_to_char( AT_WHITE, buf, ch );
  }

  fclose (fp);   

}

void do_typos( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;   

  if ( !( fp = fopen( "/home/storm/EOSBUILD/area/typos.txt", "r" ) ) )
   return;

  while ( fgets( buf, MAX_STRING_LENGTH, fp ) != NULL )
  {
    send_to_char( AT_WHITE, buf, ch );
  }

  fclose (fp);   

}

void do_todo( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;   

  if ( !( fp = fopen( "/home/storm/todo.lst", "r" ) ) )
   return;

  while ( fgets( buf, MAX_STRING_LENGTH, fp ) != NULL )
  {
    send_to_char( AT_WHITE, buf, ch );
  }

  fclose (fp);   

}
void do_changes( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;

  if ( !( fp = fopen( "/home/storm/changes.txt", "r" ) ) )
   return;
  
  while ( fgets( buf, MAX_STRING_LENGTH, fp ) != NULL )
  {
    send_to_char( AT_WHITE, buf, ch );  
  }

  fclose (fp);

}

void do_empower(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  
  argument = one_argument(argument, arg);
  if ( !*arg )
  {
    send_to_char( AT_PINK, "Syntax: empower <victim> [+|-]<cmd> "
                           "[[+|-]cmd [...]]\n\r", ch );
    send_to_char( AT_PINK, "+ will set, - will remove, none will "
                           "toggle.\n\r", ch );
    return;
  }
  if (    !is_name(NULL, "empower", ch->pcdata->empowerments)
       && ch->level != L_IMP                              )
  {
    send_to_char( AT_WHITE, "You are not empowered to empower.\n\r", ch );
    return;
  }  

  if ( !(victim = get_char_world(ch, arg)) )
  {
    send_to_char( AT_PINK, "They arent in the world.\n\r", ch );
    return;
  }
  if ( IS_NPC(victim) )
  {
    send_to_char( AT_PINK, "Not on NPC's.\n\r", ch );
    return;
  }
  for ( argument = one_argument(argument, arg); *arg != '\0';
        argument = one_argument(argument, arg) )
  {
    char *parg = arg;
    char chn;
    int cmd;
    bool has;
    char sarg[MAX_INPUT_LENGTH];
    char *pemp;
    
    if ( *parg == '+' || *parg == '-' )
    {
      chn = *parg;
      parg++;
    }
    else
      chn = ' ';
    for ( cmd = 0; *cmd_table[cmd].name != '\0'; cmd++ )
      if ( !str_prefix(parg, cmd_table[cmd].name) )
        break;
    if ( !*cmd_table[cmd].name )
      sprintf(buf, "%s is not a command.\n\r", cmd_table[cmd].name);
    else
    {
      has = is_name(NULL, cmd_table[cmd].name, victim->pcdata->empowerments);
      switch(chn)
      {
      case '+':
        if ( has )
        {
          sprintf(buf, "They are already empowered with %s.\n\r",
                  cmd_table[cmd].name);
          chn = ' ';
        }
        else if ( is_name(NULL, cmd_table[cmd].name, victim->pcdata->detractments) )
        {
          sprintf(buf, "%s is detracted from them.\n\r", cmd_table[cmd].name);
          chn = ' ';
        }
        break;
      case '-':
        if ( !has )
        {
          sprintf(buf, "They arent empowered with %s.\n\r",
                  cmd_table[cmd].name);
          chn = ' ';
        }
        break;
      default:
        if ( has )
          chn = '-';
        else
          chn = '+';
        break;
      }
      switch(chn)
      {
      case '+':
        if ( !victim->pcdata->empowerments || !*victim->pcdata->empowerments )
          strcpy(buf, cmd_table[cmd].name);
        else
          sprintf(buf, "%s %s", victim->pcdata->empowerments,
                  cmd_table[cmd].name);
        free_string(victim->pcdata->empowerments);
        victim->pcdata->empowerments = str_dup(buf);
        sprintf(buf, "They are now empowered with %s.\n\r",
                cmd_table[cmd].name);
        break;
      case '-':
        buf[0] = '\0';
        for ( pemp = one_argument(victim->pcdata->empowerments, sarg);
              *sarg != '\0'; pemp = one_argument(pemp, sarg) )
          if ( str_cmp(sarg, cmd_table[cmd].name) )
            sprintf(buf+strlen(buf), " %s", sarg);
        free_string(victim->pcdata->empowerments);
        if ( !*buf )
          victim->pcdata->empowerments = str_dup("");
        else
          victim->pcdata->empowerments = str_dup(buf+1);
        sprintf(buf, "They are no longer empowered with %s.\n\r",
                cmd_table[cmd].name);
        break;
      }
      buf[0] = UPPER(buf[0]);
      send_to_char(AT_PINK, buf, ch);
    }
  }
  return;
}

void do_detract(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  
  argument = one_argument(argument, arg);
  if ( !*arg )
  {
    send_to_char( AT_PINK, "Syntax: detract <victim> [+|-]<cmd> "
                           "[[+|-]cmd [...]]\n\r", ch );
    send_to_char( AT_PINK, "+ will set, - will remove, none will "
                           "toggle.\n\r", ch );
    return;
  }

  if (    !is_name(NULL, "detract", ch->pcdata->empowerments)
       && ch->level != L_IMP )
  {
    send_to_char( AT_WHITE, "You are note empowered to detract.\n\r", ch );
    return;
  }

  if ( !(victim = get_char_world(ch, arg)) )
  {
    send_to_char( AT_PINK, "They arent in the world.\n\r", ch );
    return;
  }
  if ( IS_NPC(victim) )
  {
    send_to_char( AT_PINK, "Not on NPC's.\n\r", ch );
    return;
  }
  for ( argument = one_argument(argument, arg); *arg != '\0';
        argument = one_argument(argument, arg) )
  {
    char *parg = arg;
    char chn;
    int cmd;
    bool has;
    char sarg[MAX_INPUT_LENGTH];
    char *pdet;
    
    if ( *parg == '+' || *parg == '-' )
    {
      chn = *parg;
      parg++;
    }
    else
      chn = ' ';
    for ( cmd = 0; *cmd_table[cmd].name != '\0'; cmd++ )
      if ( !str_prefix(parg, cmd_table[cmd].name) )
        break;
    if ( !*cmd_table[cmd].name )
      sprintf(buf, "%s is not a command.\n\r", cmd_table[cmd].name);
    else
    {
      has = is_name(NULL, cmd_table[cmd].name, victim->pcdata->detractments);
      switch(chn)
      {
      case '+':
        if ( has )
        {
          sprintf(buf, "%s is already detracted from them.\n\r",
                  cmd_table[cmd].name);
          chn = ' ';
        }
        else if ( is_name(NULL, cmd_table[cmd].name, victim->pcdata->empowerments) )
        {
          sprintf(buf, "They are empowered with %s.\n\r", cmd_table[cmd].name);
          chn = ' ';
        }
        break;
      case '-':
        if ( !has )
        {
          sprintf(buf, "%s isnt detracted from.\n\r",
                  cmd_table[cmd].name);
          chn = ' ';
        }
        break;
      default:
        if ( has )
          chn = '-';
        else
          chn = '+';
        break;
      }
      switch(chn)
      {
      case '+':
        if ( !victim->pcdata->detractments || !*victim->pcdata->detractments )
          strcpy(buf, cmd_table[cmd].name);
        else
          sprintf(buf, "%s %s", victim->pcdata->detractments,
                  cmd_table[cmd].name);
        free_string(victim->pcdata->detractments);
        victim->pcdata->detractments = str_dup(buf);
        sprintf(buf, "%s is now detracted from them.\n\r",
                cmd_table[cmd].name);
        break;
      case '-':
        buf[0] = '\0';
        for ( pdet = one_argument(victim->pcdata->detractments, sarg);
              *sarg != '\0'; pdet = one_argument(pdet, sarg) )
          if ( str_cmp(sarg, cmd_table[cmd].name) )
            sprintf(buf+strlen(buf), " %s", sarg);
        free_string(victim->pcdata->detractments);
        if ( !*buf )
          victim->pcdata->detractments = str_dup("");
        else
          victim->pcdata->detractments = str_dup(buf+1);
        sprintf(buf, "%s is no longer detracted from them.\n\r",
                cmd_table[cmd].name);
        break;
      }
      buf[0] = UPPER(buf[0]);
      send_to_char(AT_PINK, buf, ch);
    }
  }
  return;
}

void do_load(CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
	send_to_char(AT_BLUE, "Syntax:\n\r",ch);
	send_to_char(AT_BLUE, "  load mob <vnum>\n\r",ch);
	send_to_char(AT_BLUE, "  load obj <vnum> <level>\n\r",ch);
	return;
    }

    if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
    {
	do_mload(ch,argument);
	return;
    }

    if (!str_cmp(arg,"obj"))
    {
	do_oload(ch,argument);
	return;
    }
    /* echo syntax */
    do_load(ch,"");
}

#if 0
/* taken out for lack of use */
void do_vnum(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char *string;

    string = one_argument(argument,arg);
 
    if (arg[0] == '\0')
    {
	send_to_char(AT_BLUE, "Syntax:\n\r",ch);
	send_to_char(AT_BLUE, "  vnum obj <name>\n\r",ch);
	send_to_char(AT_BLUE, "  vnum mob <name>\n\r",ch);
	send_to_char(AT_BLUE, "  vnum skill <skill or spell>\n\r",ch);
	return;
    }

    if (!str_cmp(arg,"obj"))
    {
	do_ofind(ch,string);
 	return;
    }

    if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
    { 
	do_mfind(ch,string);
	return;
    }

    if (!str_cmp(arg,"skill") || !str_cmp(arg,"spell"))
    {
	do_slookup(ch,string);
	return;
    }
    /* do both */
    do_mfind(ch,argument);
    do_ofind(ch,argument);
}
#endif

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
  char		buf[MAX_STRING_LENGTH];
  int		cmd;
  int		trust;
  int		col = 0;

  trust = get_trust( ch );
  for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
  {
    int clr;
    
    if ( cmd_table[cmd].name[0] == '\0' )
      break;

    if ( cmd_table[cmd].level < LEVEL_HERO ||
        !can_use_cmd(cmd, ch, trust) || 
	(ch->level < cmd_table[cmd].level) )
      continue;

    if ( cmd_table[cmd].level == L_DIR )
      clr = AT_RED;
    else if ( cmd_table[cmd].level > L_DIR )
      clr = AT_BLUE;
    else if ( cmd_table[cmd].level == L_SEN )
      clr = AT_LBLUE;
    else
      clr = AT_GREY;
    sprintf( buf, "%-16s", cmd_table[cmd].name );
    if ( ++col % 5 == 0 )
      strcat( buf, "\n\r" );
    send_to_char(clr, buf, ch);
  }
 
  if ( col % 5 != 0 )
    send_to_char(AT_GREY, "\n\r", ch);
  return;
}
#if 0
/* taken out because it was replaced */
void do_bamfin( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC( ch ) )
    {
        if ( longstring( ch, argument ) )
	    return;

	smash_tilde( argument );
	free_string( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );
	send_to_char(AT_GREY, "Ok.\n\r", ch );
    }
    return;
}
#endif

#if 0
/* Taken out because it was replaced */
void do_bamfout( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC( ch ) )
    {
        if ( longstring( ch, argument ) )
	    return;

	smash_tilde( argument );
	free_string( ch->pcdata->bamfout );
	ch->pcdata->bamfout = str_dup( argument );
	send_to_char(C_DEFAULT, "Ok.\n\r", ch );
    }
    return;
}
#endif
void do_slaymes(CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];   

   argument = one_argument(argument,arg);
      
   if (arg[0] == '\0')
     {
        send_to_char(AT_WHITE, "Syntax:\n\r",ch);
        send_to_char(AT_RED,  "  slaymes usee <string>  (What you see.)\n\r",ch);
        send_to_char(AT_RED,  "  slaymes room <string>  (What everybody else in the room see.)\n\r",ch);
        send_to_char(AT_RED,  "  slaymes vict <string>  (What the victim see.)\n\r",ch);
        return;
     }

   if ( !str_cmp( arg,"usee" ) )
     {
       char         buf  [ MAX_STRING_LENGTH ];

       if (argument[0] == '\0')
         {
           sprintf(buf,"Slayusee: %s\n\r", ch->pcdata->slayusee);
           send_to_char(AT_WHITE, buf, ch);
           return;
         }
     
      
       if ( !IS_NPC( ch ) )
         {
           if ( longstring( ch, argument ) )
	     return;

           smash_tilde( argument );
           ch->pcdata->slayusee = str_dup( argument );
           sprintf(buf,"Slayusee now: %s\n\r",argument);
           send_to_char(AT_WHITE, buf, ch);
         }
       return;

     }

   if ( !str_cmp( arg, "room" ) )
     {
        char         buf  [ MAX_STRING_LENGTH ];

        if (argument[0] == '\0')
          {
            sprintf(buf,"Slayroom: %s\n\r", ch->pcdata->slayroom);
            send_to_char(AT_WHITE, buf, ch);
            return;
          }
      
        if ( !IS_NPC( ch ) )
          {
            if ( longstring( ch, argument ) )
	      return;

	    smash_tilde( argument );
            ch->pcdata->slayroom = str_dup( argument );
            sprintf(buf,"Slayroom now: %s\n\r",argument);
            send_to_char(AT_WHITE, buf, ch);
          }
        return;

     }

   if ( !str_cmp( arg, "vict" ) )
     {
       char         buf  [ MAX_STRING_LENGTH ];

       if (argument[0] == '\0')
         {
           sprintf(buf,"Slayvict: %s\n\r", ch->pcdata->slayvict);
           send_to_char(AT_WHITE, buf, ch);
           return;
         }
     
      
       if ( !IS_NPC( ch ) )
         {
           if ( longstring( ch, argument ) )
	     return;

	   smash_tilde( argument );
           ch->pcdata->slayvict = str_dup( argument );
           sprintf(buf,"Slayvict now: %s\n\r",argument);
           send_to_char(AT_WHITE, buf, ch);
         }
       return;

     }

   /* echo syntax */
   do_slaymes(ch,"");
}

void do_trmes(CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   
   argument = one_argument(argument,arg);
      
   if (arg[0] == '\0')
     {
        send_to_char(AT_WHITE, "Syntax:\n\r",ch);
        send_to_char(AT_RED,  
"  transmes to   <string>  (What the room the victim is trans to see.)\n\r",ch);
        send_to_char(AT_RED,  "  transmes from <string>  (What the room the victim came from see.)\n\r",ch);
        send_to_char(AT_RED,  "  transmes vict <string>  (What the victim see.)\n\r",ch);
        return;
     }

   if ( !str_cmp( arg,"to" ) )
     {
       char         buf  [ MAX_STRING_LENGTH ];

       if (argument[0] == '\0')
         {
           sprintf(buf,"Transto: %s\n\r", ch->pcdata->transto);
           send_to_char(AT_WHITE, buf, ch);
           return;
         }
    
       if ( !IS_NPC( ch ) )
         {
           if ( longstring( ch, argument ) )
	     return;

	   smash_tilde( argument );
           free_string( ch->pcdata->transto );
           ch->pcdata->transto = str_dup( argument );
           sprintf(buf,"Transto now: %s\n\r",argument);
           send_to_char(AT_WHITE, buf, ch);
         }
       return;

     }

   if ( !str_cmp( arg, "from" ) )
     {
       char         buf  [ MAX_STRING_LENGTH ];

       if (argument[0] == '\0')
         {
           sprintf(buf,"Transfrom: %s\n\r", ch->pcdata->transfrom);
           send_to_char(AT_WHITE, buf, ch);
           return;
         }
    
       if ( !IS_NPC( ch ) )
         { 
           if ( longstring( ch, argument ) )
	     return;

           smash_tilde( argument );
           free_string( ch->pcdata->transfrom );
           ch->pcdata->transfrom = str_dup( argument );
           sprintf(buf,"Transfrom now: %s\n\r",argument);
           send_to_char(AT_WHITE, buf, ch);
         }
       return;

     }

   if ( !str_cmp( arg, "vict" ) )
     {
       char         buf  [ MAX_STRING_LENGTH ];

       if (argument[0] == '\0')
         {
           sprintf(buf,"Transvict:%s", ch->pcdata->transvict);
           send_to_char(AT_WHITE, buf, ch);
           return;
         }
    
       if ( !IS_NPC( ch ) )
         {
           if ( longstring( ch, argument ) )
	     return;

	   smash_tilde( argument );
           free_string( ch->pcdata->transvict );
           ch->pcdata->transvict = str_dup( argument );
           sprintf(buf,"Transvict now: %s\n\r",argument);
           send_to_char(AT_WHITE, buf, ch);
         }
       return;
     }

   /* echo syntax */
   do_trmes(ch,"");
}

void do_bamf(CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   
   argument = one_argument(argument,arg);
      
   if (arg[0] == '\0')
     {
        send_to_char(AT_WHITE, "Syntax:\n\r",ch);
        send_to_char(AT_RED,   
"  bamfmes out  <string>  (What everybody in the room you came from see.)\n\r",ch);
        send_to_char(AT_RED,   "  bamfmes in   <string>  (What everybody in the room you goto see.)\n\r",ch);
        send_to_char(AT_RED,   "  bamfmes see  <string>  (What you see.)\n\r",ch);
        return;
     }

   if ( !str_cmp( arg,"out" ) )
     {
       char  arg [ MAX_INPUT_LENGTH ];
       char  buf [ MAX_STRING_LENGTH ];
    
       one_argument( argument, arg );
    
       if ( arg[0] == '\0' )
         {
	   sprintf( buf, "Bamfout: %s\n\r", ch->pcdata->bamfout );
	   send_to_char(AT_WHITE, buf, ch );
	   return;
         }

       if ( !IS_NPC( ch ) )
         {
           if ( longstring( ch, argument ) )
	     return;

	   smash_tilde( argument );
	   free_string( ch->pcdata->bamfout );
	   ch->pcdata->bamfout = str_dup( argument );
	   send_to_char(AT_WHITE, "Bamfout set.\n\r", ch );
         }
       return;

     }

   if ( !str_cmp( arg, "in" ) )
     {
       char  arg [ MAX_INPUT_LENGTH ];
       char  buf [ MAX_STRING_LENGTH ];
    
       one_argument( argument, arg );
    
       if ( arg[0] == '\0' )
         {
	   sprintf( buf, "Bamfin: %s\n\r", ch->pcdata->bamfin );
	   send_to_char(AT_WHITE, buf, ch );
	   return;
         }

       if ( !IS_NPC( ch ) )
        {
          if ( longstring( ch, argument ) )
	    return;

	  smash_tilde( argument );
	  free_string( ch->pcdata->bamfin );
	  ch->pcdata->bamfin = str_dup( argument );
	  send_to_char(AT_WHITE, "Bamfin set.\n\r", ch );
        }
      return;

    }

   if ( !str_cmp( arg, "see" ) )
     {
       char  arg [ MAX_INPUT_LENGTH ];
       char  buf [ MAX_STRING_LENGTH ];
    
       one_argument( argument, arg );
    
       if ( arg[0] == '\0' )
         {
	   sprintf( buf, "Bamfusee: %s\n\r", ch->pcdata->bamfusee );
	   send_to_char(AT_WHITE, buf, ch );
	   return;
         }

       if ( !IS_NPC( ch ) )
        {
          if ( longstring( ch, argument ) )
	    return;

	  smash_tilde( argument );
	  free_string( ch->pcdata->bamfusee );
	  ch->pcdata->bamfusee = str_dup( argument );
	  send_to_char(AT_WHITE, "Bamfusee set.\n\r", ch );
        }
      return;

    }

   /* echo syntax */
   do_bamf( ch, "" );
}

void do_deny( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char(AT_RED, "Deny whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char(AT_RED, "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char(AT_RED, "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char(AT_RED, "You failed.\n\r", ch );
	return;
    }

    SET_BIT( victim->act, PLR_DENY );
    send_to_char(AT_RED, "You are denied access!\n\r", victim );
    send_to_char(AT_RED, "OK.\n\r", ch );
    sprintf( buf, "$N has denied %s", victim->name );
    wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    if ( victim->level < 2 )
     victim->level = 2;
    do_quit( victim, "" );

    return;
}



void do_disconnect( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char(AT_RED, "Disconnect whom?\n\r", ch );
	return;
    }
    if ( is_number( arg ) )
    {
      for ( d = descriptor_list; d; d = d->next )
      {
	if ( d->descriptor == atoi(arg) )
	{
	  close_socket( d );
	  send_to_char( AT_RED, "Ok.\n\r",ch);
	  return;
	}
      }
      send_to_char(AT_RED, "Descriptor not found.\n\r",ch);
      return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char(AT_RED, "They aren't here.\n\r", ch );
	return;
    }

    if ( !victim->desc )
    {
	act(AT_RED, "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d == victim->desc )
	{
	    close_socket( d );
	    send_to_char(AT_RED, "Ok.\n\r", ch );
	    return;
	}
    }

    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char(AT_RED, "Descriptor not found!\n\r", ch );
    return;
}



void do_pardon( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ch->level != L_IMP)
    {
     send_to_char(AT_GREY, "Only an IMP may pardon.\n\r", ch);
     return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char(AT_BLUE, 
             "Syntax: pardon <character> <killer|thief|outcast>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char(AT_BLUE, "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char(AT_BLUE, "Not on NPC's.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "killer" ) )
    {
	if ( IS_SET( victim->act, PLR_KILLER ) )
	{
	    REMOVE_BIT( victim->act, PLR_KILLER );
	    send_to_char(AT_BLUE, "Killer flag removed.\n\r",        ch     );
	    send_to_char(AT_BLUE, "You are no longer a KILLER.\n\r", victim );
	    sprintf( buf, "$N removed %s's killer flag.", victim->name );
	    wiznet( buf, ch, NULL, WIZ_GENERAL, 0, 0 );
	}
	return;
    }

    if ( !str_cmp( arg2, "thief" ) )
    {
	if ( IS_SET( victim->act, PLR_THIEF  ) )
	{
	    REMOVE_BIT( victim->act, PLR_THIEF  );
	    send_to_char(AT_BLUE, "Thief flag removed.\n\r",        ch     );
	    send_to_char(AT_BLUE, "You are no longer a THIEF.\n\r", victim );
	    sprintf( buf, "$N removed %s's thief flag.", victim->name );
	    wiznet( buf, ch, NULL, WIZ_GENERAL, 0, 0 );
	}
	return;
    }

    if ( !str_cmp( arg2, "outcast" ) )
    {
        if ( IS_SET( victim->act, PLR_OUTCAST ) )
        {
            REMOVE_BIT( victim->act, PLR_OUTCAST );
            send_to_char(AT_BLUE, "Outcast flag removed.\n\r", ch );
            send_to_char(AT_BLUE, 
                         "You are no longer an OUTCAST.\n\r", victim );
            sprintf( buf, "$N removed %s's outcast flag.", victim->name );
            wiznet ( buf, ch, NULL, WIZ_GENERAL, 0, 0 );
        }
        return;
    }

    send_to_char(AT_BLUE,
        "Syntax: pardon <character> <killer|thief|outcast>.\n\r", ch );
    return;
}



void do_echo( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
	send_to_char(AT_YELLOW, "Echo what?\n\r", ch );
	return;
    }

    strcat( argument, "\n\r" );
    send_to_all_char( argument );

    return;
}



void do_recho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    
    if ( argument[0] == '\0' )
    {
	send_to_char(AT_YELLOW, "Recho what?\n\r", ch );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	    && d->character->in_room == ch->in_room )
	{
	    send_to_char(AT_YELLOW, argument, d->character );
	    send_to_char(AT_YELLOW, "\n\r",   d->character );
	}
    }

    return;
}



ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;

    if ( is_number( arg ) )
	return get_room_index( atoi( arg ) );

    if ( ( victim = get_char_world( ch, arg ) ) )
	return victim->in_room;

    if ( ( obj = get_obj_world( ch, arg ) ) )
	return obj->in_room;

    return NULL;
}



void do_transfer( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    ROOM_INDEX_DATA *location;
    char             arg1 [ MAX_INPUT_LENGTH ];
    char             arg2 [ MAX_INPUT_LENGTH ];	
    char 	     buf [ MAX_STRING_LENGTH ];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char(AT_RED, "Transfer whom (and where)?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
		&& d->character != ch
		&& d->character->in_room
		&& can_see( ch, d->character ) )
	    {
		sprintf( buf, "%s %s", d->character->name, arg2 );
		do_transfer( ch, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( !( location = find_location( ch, arg2 ) ) )
	{
	    send_to_char(AT_RED, "No such location.\n\r", ch );
	    return;
	}

	if ( room_is_private( location ) )
	{
	    send_to_char(AT_RED, "That room is private right now.\n\r", ch );
	    return;
	}
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char(AT_RED, "They aren't here.\n\r", ch );
	return;
    }

    if ( !victim->in_room )
    {
	send_to_char(AT_RED, "They are in limbo.\n\r", ch );
	return;
    }

    if ( location == victim->in_room )
    {
      send_to_char( C_DEFAULT, "They are already there.\n\r",ch);
      return;
    }

    if ( victim->fighting )
	stop_fighting( victim, TRUE );
	sprintf( buf, "$n %s",
            ( ch->pcdata && ch->pcdata->transfrom[0] != '\0' )
            ? ch->pcdata->transfrom : "disappears in a mushroom cloud." );
        act( AT_RED, buf, victim, NULL, NULL, TO_ROOM );

    char_from_room( victim );
    char_to_room( victim, location );
    sprintf( buf, "$n %s",
            ( ch->pcdata && ch->pcdata->transto[0] != '\0' )
            ? ch->pcdata->transto : "arrives from a puff of smoke" );
        act(AT_RED, buf, victim, NULL, NULL, TO_ROOM );

    if ( ch != victim )
    {
        sprintf( buf, "%s %s.", ch->name,
            ( ch->pcdata && ch->pcdata->transvict[0] != '\0' )
            ? ch->pcdata->transvict : "has transferred you" );
	act(AT_RED, buf, ch, NULL, victim, TO_VICT );
    }

    do_look( victim, "auto" );
    send_to_char(AT_RED, "Ok.\n\r", ch );
}



void do_at( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *wch;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    char             arg [ MAX_INPUT_LENGTH ];
    
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char(AT_RED, "At where what?\n\r", ch );
	return;
    }

    if ( !( location = find_location( ch, arg ) ) )
    {
	send_to_char(AT_RED, "No such location.\n\r", ch );
	return;
    }

    if ( room_is_private( location ) && get_trust(ch) < L_IMP )
    {
	send_to_char(AT_RED, "That room is private right now.\n\r", ch );
	return;
    }

    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    break;
	}
    }

    return;
}

void do_goto( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    CHAR_DATA *gch_next; /* goto follow - Deck*/
    CHAR_DATA *pet;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *startlocation;  /* goto follow - Deck*/
    char             arg [ MAX_INPUT_LENGTH ];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char(AT_RED, "Goto where?\n\r", ch );
	return;
    }

    if ( !( location = find_location( ch, arg ) ) )
    {
	send_to_char(AT_RED, "No such location.\n\r", ch );
	return;
    }

    if ( room_is_private( location ) && get_trust(ch) < L_IMP )
    {
	send_to_char(AT_RED, "That room is private right now.\n\r", ch );
	return;
    }
    
    for ( pet = ch->in_room->people; pet; pet = pet->next_in_room )
    {
      if ( IS_NPC( pet ) )
        if ( IS_SET( pet->act, ACT_PET ) && ( pet->master == ch ) )
          break;
    }

    if ( ch->fighting )
	stop_fighting( ch, TRUE );
    if ( pet && pet->fighting )
        stop_fighting( pet, TRUE );

    act(AT_RED, "You $T.", ch, NULL, 
        ( ch->pcdata && ch->pcdata->bamfusee[0] != '\0' )
        ? ch->pcdata->bamfusee : "leave in a swirling mist", TO_CHAR);
	
    if ( !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
	act(AT_RED, "$n $T.", ch, NULL,
	    ( ch->pcdata && ch->pcdata->bamfout[0] != '\0' )
	    ? ch->pcdata->bamfout : "leaves in a swirling mist", TO_ROOM );
	if ( pet )
	  act( AT_RED, "$n leaves in a swirling mist.", pet, NULL, NULL, TO_ROOM );
    }
    else
     {
       for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
       {
           if ( ch != gch )
           if ( get_trust(gch) >= ch->wizinvis )
           {
              sprintf (buf, "%s slightly phased %s.\n\r", ch->name,
                  ( ch->pcdata && ch->pcdata->bamfout[0] != '\0' )
                  ? ch->pcdata->bamfout : "leaves in a swirling mist");
             send_to_char ( AT_RED, buf, gch );
              if ( pet )
                 {
                    sprintf ( buf, "%s leaves in a swirling mist.\n\r",
                      pet->name );
                    send_to_char ( AT_RED, buf, gch );
                 }
           }
       }
     }

    if ( location == ch->in_room )
    {
      send_to_char(C_DEFAULT, "But you are already there!\n\r",ch);
      return;
    }
   
    startlocation = ch->in_room;    
    char_from_room( ch );
    char_to_room( ch, location );
    if ( pet )
    {
      char_from_room( pet );
    }

    if ( !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
	act(AT_RED, "$n $T.", ch, NULL,
	    ( ch->pcdata && ch->pcdata->bamfin[0] != '\0' )
	    ? ch->pcdata->bamfin : "appears in a swirling mist", TO_ROOM );
    }
    else
     {
       for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
       {
           if ( ch != gch )
           if ( get_trust(gch) >= ch->wizinvis )
	           {
              sprintf( buf, "%s slightly phased %s.\n\r", ch->name,
                 ( ch->pcdata && ch->pcdata->bamfin[0] != '\0' )
                 ? ch->pcdata->bamfin : "appears in a swirling mist" );
              send_to_char ( AT_RED, buf, gch );
           }
       }
     }

    do_look( ch, "auto" );
    if ( pet )
      char_to_room( pet, location );
    if ( pet && !IS_SET( ch->act, PLR_WIZINVIS ) )
      act( AT_RED, "$n appears in a swirling mist.", pet, NULL, NULL, TO_ROOM );
    else if ( pet && IS_SET( ch->act, PLR_WIZINVIS ) )
    {
       for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
       {
           if ( get_trust(gch) >= ch->wizinvis )
           {
             sprintf( buf, "%s appears in a swirling mist.\n\r", pet->name);
             send_to_char ( AT_RED, buf, gch );
           }
       }
    }      

    /* goto follow -Decklarean */
    for ( gch = startlocation->people; gch; gch = gch_next )
    {
        gch_next = gch->next_in_room;

        if ( gch->deleted )
           continue;

        if (!IS_IMMORTAL( gch ))
           continue;

        if ( gch->master == ch )
        {
           act(AT_GREY, "You follow $N.", gch, NULL, ch, TO_CHAR );
           do_goto( gch, arg );
	}
    }

/*    send_to_char(C_DEFAULT, "\n\r", ch);
    act(C_DEFAULT, "\n\r", ch, NULL, NULL, TO_ROOM);
  */  
    return;
}



void do_rstat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA        *obj;
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;
    char             buf  [ MAX_STRING_LENGTH ];
    char             arg  [ MAX_INPUT_LENGTH  ];
    int              door;

    one_argument( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( !location )
    {
	send_to_char(AT_RED, "No such location.\n\r", ch );
	return;
    }

    if ( ch->in_room != location && room_is_private( location ) &&
         get_trust(ch) < L_IMP )
    {
	send_to_char(AT_RED, "That room is private right now.\n\r", ch );
	return;
    }


    sprintf( buf, "&WName: [%s&W]\n\rArea: [%s&W]\n\r",
	    location->name,
	    location->area->name );
    send_to_char(AT_RED, buf, ch);

    sprintf( buf,
	    "&WVnum: &R%d  &WSector: &R%d  &WLight: &R%d\n\r",
	    location->vnum,
	    location->sector_type,
	    location->light );
    send_to_char(AT_RED, buf, ch);

    sprintf( buf,
            "&WRoom flags: &W[&G%s&W]\n\r", flag_string( room_flags, location->room_flags ) );
    send_to_char(AT_RED, buf, ch);
     
    sprintf( buf,
	    "&WRoom flags value: &R%d\n\r&WDescription:\n\r&Y%s",
	    location->room_flags,
	    location->description );
    send_to_char(AT_RED, buf, ch);

    if ( location->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char(AT_RED, "&WExtra description keywords:&W [&R", ch);
	for ( ed = location->extra_descr; ed; ed = ed->next )
	{
	    strcpy( buf, ed->keyword );
	    if ( ed->next )
		strcat( buf, " " );
	    send_to_char(AT_RED, buf, ch);
	}
	send_to_char( AT_RED, "&W]\n\r", ch );
    }

    send_to_char(AT_RED, "&WCharacters: [", ch);

    /* Yes, we are reusing the variable rch.  - Kahn */
    for ( rch = location->people; rch; rch = rch->next_in_room )
    {
	strcpy( buf, " " );
	one_argument( rch->name, buf+1 );
	send_to_char(AT_RED, buf, ch);
    }
    
    send_to_char(AT_RED, "&W ]\n\rObjects:    [", ch);
    for ( obj = location->contents; obj; obj = obj->next_content )
    {
	strcpy( buf, " " );
	one_argument( obj->name, buf+1 );
	send_to_char(AT_RED, buf, ch);
    }
    send_to_char(AT_RED, "&W ]\n\r", ch);

    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = location->exit[door] ) )
	{
	    sprintf( buf,
		    "&WDoor: &G%d  &WTo: &G%d  &WKey: &G%d  &WExit flags: &G%d\n\r",
		    door,
		    pexit->to_room ? pexit->to_room->vnum : 0,
		    pexit->key,
		    pexit->exit_info );
	    send_to_char(AT_RED, buf, ch);
	    sprintf( buf,
		    "&WKeyword: [&R%s&W]  Description: &R%s",
		    pexit->keyword,
		    pexit->description[0] != '\0' ? pexit->description
		                                  : "(none)\n\r" );
	    send_to_char(AT_RED, buf, ch);
	}
    }

    return;
}

void do_astat( CHAR_DATA *ch, char *argument ) 
{
    ROOM_INDEX_DATA    *location;
    AREA_DATA          *pArea;
    char        arg    [ MAX_INPUT_LENGTH ];
    char        buf    [MAX_STRING_LENGTH ];

    one_argument( argument, arg );
    
    pArea = ch->in_room->area;

if ( arg[0] != '\0' )
{   
   if ( is_number( arg ) )
   {
      if ( !( pArea = get_area_data( atoi( arg ) ) ) )
      {
         send_to_char( AT_WHITE, "No such area exists.\n\r", ch ); 
         return;
      }
   }
   else
   {
      location = find_location( ch, arg );
      if ( !location )
      {
         send_to_char( AT_RED, "No such location.\n\r", ch );
         return;
      }
      pArea = location->area;
   }
}        

    sprintf( buf, "&WName:     &z[&W%5d&z] &w%s\n\r",pArea->vnum,pArea->name );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&WLevel:    &z{&W%d&z-&W%d&z}\n\r", pArea->llevel, pArea->ulevel );
    send_to_char(C_DEFAULT, buf, ch );
    sprintf( buf, "&WRecall:   &z[&W%5d&z]&w %s\n\r", pArea->recall,
        get_room_index( pArea->recall )
        ? get_room_index( pArea->recall )->name : "none" );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WSounds:   &w%s\n\r", pArea->reset_sound?pArea->reset_sound :
                                      "(default)" );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WFile:     &C%s\n\r", pArea->filename );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WVnums:    &z[&W%d-%d&z]\n\r", pArea->lvnum,pArea->uvnum );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WAge:      &z[&W%d&z]\n\r", pArea->age );
    send_to_char(C_DEFAULT, buf, ch );

/* Angi 
    sprintf( buf, "&WColor:    &z[&W%s&z]\n\r", pArea->color );
    send_to_char(C_DEFAULT, buf, ch );  */

    sprintf( buf, "&WPlayers:  &z[&W%d&z]\n\r", pArea->nplayer );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WSecurity: &z[&W%d&z]\n\r", pArea->security );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WBuilders: &z[&B%s&z]\n\r", pArea->builders );
    send_to_char(C_DEFAULT, buf, ch );

    sprintf( buf, "&WFlags:    &z[&R%s&z]\n\r", flag_string( area_flags,pArea->area_flags ) );
    send_to_char(C_DEFAULT, buf, ch );

    return;

}

void do_ostat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA    *obj;
    AFFECT_DATA *paf;
    char         buf  [ MAX_STRING_LENGTH ];
    char         arg  [ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Ostat what?\n\r", ch );
	return;
    }

    if ( !( obj = get_obj_world( ch, arg ) ) )
    {
	send_to_char(AT_WHITE, "Nothing like that in hell, earth, or heaven.\n\r", ch);
	return;
    }

    sprintf( buf, "Name: %s.\n\r",
	    obj->name );
    send_to_char(AT_RED, buf, ch);

    sprintf( buf, "Vnum: %d.  Type: %s.\n\r",
	    obj->pIndexData->vnum, item_type_name( obj ) );
    send_to_char(AT_RED, buf, ch);

    sprintf( buf, "Short description: %s.\n\rLong description: %s\n\r",
	    obj->short_descr, obj->description );
    send_to_char(AT_RED, buf, ch);

    sprintf( buf, "Wear bits name: %s.\n\r", flag_string( wear_flags,
    	obj->wear_flags ) );
    send_to_char(AT_RED, buf, ch);

    sprintf( buf, "Wear bits: %d.  Extra bits: %s.\n\r",
	    obj->wear_flags, extra_bit_name( obj->extra_flags ) );
    send_to_char(AT_RED, buf, ch);
/* FOR NEW FLAGS */
    sprintf( buf, "AntiClass bits: %s.\n\r",
	    anticlass_bit_name( obj->anti_class_flags ) );
    send_to_char(AT_RED, buf, ch );
/*    sprintf( buf, "AntiClass bits: %s.\n\r",
           flag_string( anti_class_flags, obj->anti_class_flags ) );
    send_to_char(AT_BLUE, buf, ch );*/

    sprintf( buf, "AntiRace bits: %s.\n\r",
	    antirace_bit_name( obj->anti_race_flags ) );
    send_to_char(AT_RED, buf, ch );

    sprintf( buf, "Number: %d/%d.  Weight: %d/%d.\n\r",
	    1,           get_obj_number( obj ),
	    obj->weight, get_obj_weight( obj ) );
    send_to_char(AT_RED, buf, ch);
#ifdef NEW_MONEY
    sprintf( buf, "Gold Cost: %d.  Silver Cost: %d.  Copper Cost: %d.\n\r",
	     obj->cost.gold, obj->cost.silver, obj->cost.copper );
    send_to_char( AT_RED, buf, ch );
    sprintf( buf, "Timer: %d.  Level: %d.\n\r", obj->timer, obj->level );
#else
    sprintf( buf, "Cost: %d.  Timer: %d.  Level: %d.\n\r",
	    obj->cost, obj->timer, obj->level );
#endif
    send_to_char(AT_RED, buf, ch);

    sprintf( buf,
	    "In room: %d.  In object: %s.  Carried by: %s.  Stored by: %s.\n\rWear_loc: %d.\n\r",
	    !obj->in_room    ?        0 : obj->in_room->vnum,
	    !obj->in_obj     ? "(none)" : obj->in_obj->short_descr,
	    !obj->carried_by ? "(none)" : obj->carried_by->name,
	    !obj->stored_by  ? "(none)" : obj->stored_by->name,
	    obj->wear_loc );
    send_to_char(AT_RED, buf, ch);
    
    sprintf( buf, "Values: %d %d %d %d.\n\r",
	    obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
    send_to_char(AT_RED, buf, ch);

    if ( obj->extra_descr || obj->pIndexData->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char(AT_RED, "Extra description keywords: '", ch);

	for ( ed = obj->extra_descr; ed; ed = ed->next )
	{
	    strcpy( buf, ed->keyword );
	    if ( ed->next )
		strcat( buf, " " );
	    send_to_char(AT_RED, buf, ch);
	}

	for ( ed = obj->pIndexData->extra_descr; ed; ed = ed->next )
	{
	    strcpy( buf, ed->keyword );
	    if ( ed->next )
		strcat( buf, " " );
	    send_to_char(AT_RED, buf, ch);
	}

	send_to_char(AT_RED, "'.\n\r", ch);
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d.\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	send_to_char(AT_RED, buf, ch);
    }

    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d.\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	send_to_char(AT_RED, buf, ch);
    }

    return;
}


void do_mstat( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA   *victim;
    AFFECT_DATA *paf;
    char         buf  [ MAX_STRING_LENGTH ];
    char         arg  [ MAX_INPUT_LENGTH  ];
    
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Mstat whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
    }
send_to_char( AT_WHITE,
"Description&w:&z___________________________________________________\n\r",
	      ch );

    sprintf( buf, "&cName&w: &W%s %s\n\r",
	    victim->name, 
	     ( !IS_NPC( victim ) && victim->pcdata->lname )
	    ? victim->pcdata->lname : "" );
    send_to_char(C_DEFAULT, buf, ch);

    if ( victim->hunting != NULL )
    {
       sprintf( buf, "Tracking victim: %s (%s)\n\r",
                IS_NPC(victim->hunting) ? victim->hunting->short_descr
                                        : victim->hunting->name,
                IS_NPC(victim->hunting) ? "MOB" : "PLAYER" );
       send_to_char(C_DEFAULT, buf, ch);
    }

    
    if ( !IS_NPC( victim )  && victim->desc )
    {
      if ( str_cmp( victim->desc->user, "(unknown)" ) )
      {
        sprintf( buf, "&c    Email&w: &W%s&R@&W%s\n\r",
	      victim->desc->user ? victim->desc->user : "(none)",
	      victim->desc->host );
        send_to_char(C_DEFAULT, buf, ch);
      }
      else send_to_char(C_DEFAULT, "   ", ch);
    }
    else send_to_char(C_DEFAULT, "   ", ch);

    sprintf( buf, "&cGuild&w: &W%s\n\r",
            victim->guild ? victim->guild->name : "NONE");
    send_to_char(C_DEFAULT, buf, ch);
    
    if (!IS_NPC( victim ) )
    {
    CLAN_DATA *pClan = get_clan_index( victim->clan );
    sprintf(buf, "&cClan&w: &R%d &w= &W%s.\n\r",
            victim->clan, pClan->name );
    send_to_char(C_DEFAULT, buf, ch);
    }
    if ( IS_NPC( victim ) )
      {
      sprintf( buf, "Vnum&w: &R%d   ", victim->pIndexData->vnum );
      send_to_char( AT_CYAN, buf, ch );
      }
    sprintf( buf, "Sex&w: &W%s   &cRoom&w: &R%d\n\r",
	    victim->sex == SEX_MALE    ? "male"   :
	    victim->sex == SEX_FEMALE  ? "female" : "neutral",
	    !victim->in_room           ?        0 : victim->in_room->vnum );
    send_to_char(AT_CYAN, buf, ch);
    sprintf( buf, "Age&w: &R%d  &cPlayed&w: &R%d  &cTimer&w: &R%d\n\r",
	    get_age( victim ),
	    (int) victim->played,
	    victim->timer );
    send_to_char(AT_CYAN, buf, ch);
    if ( !IS_NPC( victim ) )
    if (!victim->pcdata->switched)	/* OLC */
    {
	sprintf( buf, "Security&w:&R %d\n\r", victim->pcdata->security );
	send_to_char(AT_CYAN, buf, ch);
    }

    sprintf( buf, "Short description&w:&W %s\n\r&cLong  description&w:&W %s",
	    victim->short_descr,
	    victim->long_descr[0] != '\0' ? victim->long_descr
	                                  : "(none)&w.\n\r" );
    send_to_char(AT_CYAN, buf, ch);
    sprintf( buf, "Act&w:&W %s\n\r", act_bit_name( victim->act ) );
    send_to_char(AT_CYAN, buf, ch);
    if ( IS_NPC( victim ) && victim->spec_fun != 0 )
    {
        sprintf( buf, "Mobile has spec function&w:&W %s\n\r",spec_string(victim->spec_fun) );
	send_to_char(AT_CYAN, buf, ch);
    }
    if ( !IS_NPC( victim ) && get_trust( victim ) > L_APP )
    {
         sprintf( buf, "Bamfin&w:  &W%s\n\r", victim->pcdata->bamfin );
	 send_to_char(AT_CYAN, buf, ch);
         sprintf( buf, "Bamfout&w: &W%s\n\r", victim->pcdata->bamfout );
	 send_to_char(AT_CYAN, buf, ch);
    }
    if ( !IS_NPC( victim ) )
    {
    if ( victim->pcdata->empowerments && *victim->pcdata->empowerments )
      {
      sprintf(buf, "Empowerments&w: &W%s\n\r",victim->pcdata->empowerments);
      send_to_char(AT_CYAN, buf, ch );
      }
    if ( victim->pcdata->detractments && *victim->pcdata->detractments )
      {
      sprintf(buf, "Detractments&w: &W%s\n\r",victim->pcdata->detractments);
      send_to_char(AT_CYAN, buf, ch );
      }
    }
send_to_char(AT_WHITE,
"Statistics&w:&z____________________________________________________\n\r",
	ch );
    sprintf( buf, "Lv&w: &R%d ", victim->level );
    send_to_char( AT_CYAN, buf, ch );
    sprintf( buf, "&cClass&w: &W%s ", class_short( victim ) );
    send_to_char( AT_CYAN, buf, ch );
    sprintf( buf, "&cAlign&w: &R%d ", victim->alignment );
    send_to_char( AT_CYAN, buf, ch );
    sprintf( buf, "&cAC&w: &R%d ", GET_AC( victim ) );
    send_to_char( AT_CYAN, buf, ch );
    sprintf( buf, "&cExp&w: &R%d\n\r", victim->exp );
    send_to_char( AT_CYAN, buf, ch);
    
    if ( !IS_NPC( victim ) )
    {
	sprintf( buf, "Charisma&w: &R%d   &cPage Lines&w: &R%d\n\r",
		victim->charisma, victim->pcdata->pagelen );
	send_to_char(AT_CYAN, buf, ch);
    }
    sprintf( buf, "&cStr&w: &R%2d&w/&R%2d&w(&R%2d&w) "
	      "&cInt&w: &R%2d&w/&R%2d&w(&R%2d&w) "
	      "&cWis&w: &R%2d&w/&R%2d&w(&R%2d&w) "
	      "&cDex&w: &R%2d&w/&R%2d&w(&R%2d&w) "
	      "&cCon&w: &R%2d&w/&R%2d&w(&R%2d&w)\n\r", 
	 IS_NPC( victim ) ? 13 : victim->pcdata->perm_str, 
	 get_curr_str( victim ), 
	 IS_NPC( victim ) ? 0 : victim->pcdata->mod_str,
	 IS_NPC( victim ) ? 13 : victim->pcdata->perm_int, 
	 get_curr_int( victim ),
	 IS_NPC( victim ) ? 0 : victim->pcdata->mod_int,
	 IS_NPC( victim ) ? 13 : victim->pcdata->perm_wis, 
	 get_curr_wis( victim ),
	 IS_NPC( victim ) ? 0 : victim->pcdata->mod_wis,
	 IS_NPC( victim ) ? 13 : victim->pcdata->perm_dex, 
	 get_curr_dex( victim ),
	 IS_NPC( victim ) ? 0 : victim->pcdata->mod_dex,
	 IS_NPC( victim ) ? 13 : victim->pcdata->perm_con,
	 get_curr_con( victim ),
	 IS_NPC( victim ) ? 0 : victim->pcdata->mod_con );
    send_to_char(AT_CYAN, buf, ch);
sprintf( buf, "Hp&w: &R%d&w/&R%d &c%s&w: &R%d&w/&R%d &cMove&w: &R%d&w/&R%d &cPractices&w: &R%d\n\r",
	    victim->hit,         MAX_HIT(victim),
	    is_class( victim, CLASS_VAMPIRE ) ? "Blood" : "Mana",
	    MT( victim ),        MT_MAX(victim), 
	    victim->move,        MAX_MOVE(victim),
	    victim->practice );
    send_to_char( AT_CYAN, buf, ch );

   sprintf( buf, 
"Mod Hp&w: &R%d &cMod Blood&w: &R%d &cMod Mana&w: &R%d &cMod Move&w: &R%d\n\r",
            victim->mod_hit, victim->mod_bp, 
	    victim->mod_mana, victim->mod_move );
 send_to_char(AT_CYAN, buf, ch);
    if ( !IS_NPC( victim ) )
    {
	sprintf( buf,
"Thirst&w: &R%d &cFull&w: &R%d &cDrunk&w: &R%d &cSaving throw&w: &R%d\n\r",
		victim->pcdata->condition[COND_THIRST],
		victim->pcdata->condition[COND_FULL  ],
		victim->pcdata->condition[COND_DRUNK ],
		victim->saving_throw );
	send_to_char(AT_CYAN, buf, ch);
    }
#ifdef NEW_MONEY
    sprintf( buf, "Gold&w: &R%d &cSilver&w: &R%d &cCopper&w: &R%d\n\r",
	     victim->money.gold, victim->money.silver, victim->money.copper );
    send_to_char(AT_CYAN, buf, ch);
    if ( !IS_NPC( victim ) )
    {
    sprintf( buf, "Bank Account&w: &R%d&WGold  &R%d&WSilver &R%d&WCopper\n\r",
	 victim->pcdata->bankaccount.gold, victim->pcdata->bankaccount.silver,
	 victim->pcdata->bankaccount.copper );
    send_to_char(AT_CYAN, buf, ch);
    }
#endif
    sprintf( buf, "Hitroll&w: &R%d &cDamroll&w: &R%d &cPosition&w: &R%d &cWimpy&w:&R%d\n\r",
	    GET_HITROLL( victim ), GET_DAMROLL( victim ),
	    victim->position,      victim->wimpy );
    send_to_char(AT_CYAN, buf, ch);
    sprintf( buf, "Carry number&w: &R%d  &cCarry weight&w: &R%d\n\r",
	    victim->carry_number, victim->carry_weight );
    send_to_char(AT_CYAN, buf, ch);
    sprintf( buf, "Fighting&w: &W%s &cMaster&w:&W %s &cLeader&w: &W%s\n\r",
	    victim->fighting    ? victim->fighting->name : "(none)",
	    victim->master      ? victim->master->name   : "(none)",
	    victim->leader      ? victim->leader->name   : "(none)" );
    send_to_char(AT_CYAN, buf, ch);
    sprintf(buf, "Summon timer&w:&R %d &cCombat timer&w:&R %d &cClan timer&w:&R %d\n\r",
	victim->summon_timer,
	victim->combat_timer,
	victim->ctimer);
    send_to_char(AT_CYAN, buf, ch);

    sprintf(buf, "Stunned&w:&R &cTotal&w:&R %d &cCommand&w:&R %d &cMagic&c:&R %d "
	         "&cNon&w-&cMagic&w:&R %d  &cTo&w-&cStun&w:&R %d\n\r",
	    victim->stunned[STUN_TOTAL], victim->stunned[STUN_COMMAND],
	    victim->stunned[STUN_MAGIC], victim->stunned[STUN_NON_MAGIC],
	    victim->stunned[STUN_TO_STUN]);
    send_to_char(AT_CYAN, buf, ch);

send_to_char(AT_WHITE, 
"Immunities&w:&z____________________________________________________\n\r", ch );
    sprintf( buf, "Immune&w:&W %s\n\r", imm_bit_name(victim->imm_flags));
    send_to_char(AT_CYAN, buf, ch);
    sprintf( buf, "Resist&w:&W %s\n\r", imm_bit_name(victim->res_flags));
    send_to_char(AT_CYAN, buf, ch);
    sprintf( buf, "Vuln&w:&W   %s\n\r", imm_bit_name( victim->res_flags));
    send_to_char(AT_CYAN, buf, ch);
send_to_char(AT_WHITE,
"Spells &w&& &WAffects&w:&z______________________________________________\n\r", ch );
    sprintf( buf, "Affected &w(&W1&w):&W %s\n\r",affect_bit_name(victim->affected_by ) ); 
    send_to_char(AT_CYAN, buf, ch);
    sprintf( buf, "Affected &w(&W2&w):&W %s\n\r",affect_bit_name2(victim->affected_by2 ) );
    send_to_char(AT_CYAN, buf, ch);
   
    for ( paf = victim->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
	sprintf( buf,
	  "'&W%s&w': &W%s &cby &R%d &cfor &R%d hours&w; &cbits &W%s&w, &clevel &R%d&w.\n\r",
	  skill_table[(int) paf->type].name,
	  affect_loc_name( paf->location ),
	  paf->modifier,
	  paf->duration,
	  affect_bit_name( paf->bitvector ),
	  paf->level );
	send_to_char(C_DEFAULT, buf, ch);
    }
    for ( paf = victim->affected2; paf; paf = paf->next )
    {
        if ( paf->deleted )
	  continue;
	sprintf( buf,
	  "'&W%s&w': &W%s &cby &R%d &cfor &R%d hours&w; &cbits &W%s&w, &clevel &R%d&w.\n\r",
	  skill_table[(int) paf->type].name,
	  affect_loc_name( paf->location ),
	  paf->modifier,
	  paf->duration,
	  affect_bit_name2( paf->bitvector ),
	  paf->level );
	send_to_char(C_DEFAULT, buf, ch);
    }
    return;
}



void do_mfind( CHAR_DATA *ch, char *argument )
{
           MOB_INDEX_DATA *pMobIndex;
           char            buf  [ MAX_STRING_LENGTH   ];
           char            arg  [ MAX_INPUT_LENGTH    ];
    extern int             top_mob_index;
           int             vnum;
	   int             nMatch;
	   bool            fAll;
	   bool            found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Mfind whom?\n\r", ch );
	return;
    }

    fAll    = !str_cmp( arg, "all" );
    found   = FALSE;
    nMatch  = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) )
	{
	    nMatch++;
	    if ( fAll || is_name( ch, arg, pMobIndex->player_name ) )
	    {
		found = TRUE;
		sprintf( buf, "&z[&W%5d&z] &w%s\n\r",
		    pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
		send_to_char(AT_RED, buf, ch );
	    }
	}
    }

    if ( !found )
    {
	send_to_char(AT_WHITE, "Nothing like that in hell, earth, or heaven.\n\r", ch);
	return;
    }

    return;
}



void do_ofind( CHAR_DATA *ch, char *argument )
{
	   OBJ_INDEX_DATA *pObjIndex;
	   char            buf  [ MAX_STRING_LENGTH   ];
	   char            arg  [ MAX_INPUT_LENGTH    ];
    extern int             top_obj_index;
	   int             vnum;
	   int             nMatch;
	   bool            fAll;
	   bool            found;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Syntax: ofind <name>\n\r", ch );
	send_to_char(AT_WHITE, "        ofind *<type> <sub-type>\n\r", ch );
	send_to_char(AT_WHITE, "&pType being:\n\r", ch );
	send_to_char(AT_WHITE, "  light scroll wand staff weapon\n\r", ch );
	send_to_char(AT_WHITE, "  treasure armor potion noteboard\n\r", ch );
	send_to_char(AT_WHITE, "  furniture trash container\n\r", ch );
	send_to_char(AT_WHITE, "  drink-container blood key food\n\r", ch );
	send_to_char(AT_WHITE, "  money boat fountain\n\r", ch );
	send_to_char(AT_WHITE, "&pSub-Types being:\n\r", ch );
	send_to_char(AT_WHITE, "  armor: a valid wear-loc\n\r", ch );
	send_to_char(AT_WHITE, "  weapon: a valid weapon-type\n\r", ch );
	send_to_char(AT_WHITE, "  invoke: 1 to 5\n\r", ch );
	return;
    }


    fAll    = !str_cmp( arg, "all" );
    found   = FALSE;
    nMatch  = 0;
    if ( arg[0] == '*' )
      {
      extern int flag_lookup args( ( const char *name, const struct flag_type *flag_table ) );
      int type = flag_lookup( (arg+1), type_flags );
      int sub_type = 0;
      char buf2 [MAX_STRING_LENGTH] = {0};

      if ( str_cmp( (arg+1), "invoke" ) )
	{
        if ( type == ITEM_WEAPON || type == ITEM_ARMOR )
	  {
          one_argument( argument, buf2 );
	  if ( type == ITEM_WEAPON )
	    sub_type = flag_lookup( buf2, weapon_flags );
	  else
	    sub_type = flag_lookup( buf2, wear_flags );
	  }
        if ( type == ITEM_ARMOR
	&& sub_type < 1 )
	  {
	  send_to_char(AT_WHITE, "Unknow armor sub-type.\n\r", ch );
	  return;
	  }
        if ( type == ITEM_WEAPON
        && !str_cmp( flag_string( weapon_flags, sub_type ), "none" ) )
	  {
	  send_to_char(AT_WHITE, "Uknown weapon sub-type.\n\r", ch );
	  return;
	  }
	for ( vnum = 0; nMatch < top_obj_index; vnum++ )
	  {
	  if ( ( pObjIndex = get_obj_index( vnum ) ) )
	    {
	    nMatch++;
	    if ( type == ITEM_ARMOR
	    && pObjIndex->item_type == type
	    && CAN_WEAR( pObjIndex, sub_type ) )
	      {
	      found = TRUE;
	      sprintf( buf, "[&W%5d&z] &w%s\n\r",
              pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
	      send_to_char(AT_DGREY, buf, ch );
	      continue;
	      }
	    else if ( type == ITEM_WEAPON
	    && pObjIndex->item_type == type
	    && pObjIndex->value[3] == sub_type )
	      {
	      found = TRUE;
	      sprintf( buf, "[&W%5d&z] &w%s\n\r",
              pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
	      send_to_char(AT_DGREY, buf, ch );
	      continue;
	      }
	    else if ( pObjIndex->item_type == type 
		   && !sub_type )
	      {
	      found = TRUE;
	      sprintf( buf, "[&W%5d&z] &w%s\n\r",
              pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
	      send_to_char(AT_DGREY, buf, ch );
	      }
	    }
	  }
	}
      else
	{
        one_argument( argument, buf2 );
	sub_type = is_number( buf2 ) ? atoi( buf2 ) : 0;
        if ( sub_type < 0 || sub_type > 5 )
	  {
	  send_to_char(AT_WHITE, "Illegal AC_TYPE for search.\n\r", ch );
	  return;
	  }
	for ( vnum = 0; nMatch < top_obj_index; vnum++ )
	  {
	  if ( ( pObjIndex = get_obj_index( vnum ) ) )
	    {
	    nMatch++;
	    if ( pObjIndex->ac_type == sub_type )
	      {
	      found = TRUE;
	      sprintf( buf, "[&W%5d&z] &w%s\n\r",
		pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
	      send_to_char( AT_GREY, buf, ch );
	      }
	    }
	  }
	}
      if ( !found )
	send_to_char(AT_WHITE, "Nothing like that in hell, earth, or heaven.\n\r", ch);
      return;
      } 
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
	if ( ( pObjIndex = get_obj_index( vnum ) ) )
	{
	    nMatch++;
	    if ( fAll || is_name( ch, arg, pObjIndex->name ) )
	    {
		found = TRUE;
		sprintf( buf, "&z[&W%5d&z] &w%s\n\r",
		    pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
		send_to_char(AT_RED, buf, ch );
	    }
	}
    }

    if ( !found )
	send_to_char(AT_WHITE, "Nothing like that in hell, earth, or heaven.\n\r", ch);
    return;
}


void do_mwhere( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       buf  [ MAX_STRING_LENGTH   ];
    char       arg  [ MAX_INPUT_LENGTH    ];
    bool       found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Mwhere whom?\n\r", ch );
	return;
    }

    found   = FALSE;
    for ( victim = char_list; victim; victim = victim->next )
    {
	if ( IS_NPC( victim )
	    && victim->in_room
	    && is_name( ch, arg, victim->name ) )
	{
	    found = TRUE;
	    sprintf( buf, "[%5d] %-28s [%5d] %s\n\r",
		    victim->pIndexData->vnum,
		    victim->short_descr,
		    victim->in_room->vnum,
		    victim->in_room->name );
	    send_to_char(AT_RED, buf, ch);
	}
    }

    if ( !found )
    {
	act(AT_WHITE, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
	return;
    }

    return;
}



void do_reboo( CHAR_DATA *ch, char *argument )
{
    send_to_char(AT_WHITE, "If you want to REBOOT, spell it out.\n\r", ch );
    return;
}



void do_reboot( CHAR_DATA *ch, char *argument )
{
           char       buf [ MAX_STRING_LENGTH ];
    extern bool       merc_down;

 if ( ch )
 {
    sprintf( buf, "Reboot by %s.", ch->name );
    do_echo( ch, buf );
 }
    end_of_game( );

    merc_down = TRUE;
    return;
}



void do_shutdow( CHAR_DATA *ch, char *argument )
{
    send_to_char(AT_WHITE, "If you want to SHUTDOWN, spell it out.\n\r", ch );
    return;
}



void do_shutdown( CHAR_DATA *ch, char *argument )
{
           char       buf [ MAX_STRING_LENGTH ];
    extern bool       merc_down;
/*    if ( !str_cmp( ch->name, "Hannibal" ) )
	sprintf( buf, "Nuclear Meltdown by %s.", ch->name );
    else */

    sprintf( buf, "Shutdown by %s.", ch->name );
    append_file( ch, SHUTDOWN_FILE, buf );
    strcat( buf, "\n\r" );
    do_echo( ch, buf );

    end_of_game( );

    merc_down = TRUE;
    return;
}



void do_snoop( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             arg [ MAX_INPUT_LENGTH ];
    char             buf [ MAX_STRING_LENGTH ];
    
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Snoop whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
    }

    if ( !victim->desc )
    {
	send_to_char(AT_WHITE, "No descriptor to snoop.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char(AT_BLUE, "Cancelling all snoops.\n\r", ch );
	wiznet( "$N stops being such a snoop.",
	       ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust( ch ) );
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->snoop_by == ch->desc )
		d->snoop_by = NULL;
	}
	return;
    }

    if ( victim->desc->snoop_by )
    {
	send_to_char(AT_WHITE, "Busy already.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char(AT_WHITE, "You failed.\n\r", ch );
	return;
    }

    if ( ch->desc )
    {
	for ( d = ch->desc->snoop_by; d; d = d->snoop_by )
	{
	    if ( d->character == victim || d->original == victim )
	    {
		send_to_char(AT_WHITE, "No snoop loops.\n\r", ch );
		return;
	    }
	}
    }

    victim->desc->snoop_by = ch->desc;
    sprintf( buf, "$N starts snooping %s.", victim->name );
    wiznet( buf, ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust( ch ) );
    sprintf( buf, "You successfully snoop %s.\n\r", victim->name );
    send_to_char(AT_BLUE, buf, ch );
    return;
}



void do_switch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    
    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Switch into whom?\n\r", ch );
	return;
    }

    if ( !ch->desc )
	return;
    
    if ( ch->desc->original )
    {
	send_to_char(AT_WHITE, "You are already switched.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
	return;
    }

    /*
     * Pointed out by Da Pub (What Mud)
     */
    if ( !IS_NPC( victim ) )
    {
        send_to_char(AT_WHITE, "You cannot switch into a player!\n\r", ch );
	return;
    }

    if ( victim->desc )
    {
	send_to_char(AT_WHITE, "Character in use.\n\r", ch );
	return;
    }

    sprintf( buf, "$N switches into %s.", victim->short_descr );
    wiznet( buf, ch, NULL, WIZ_SWITCHES, WIZ_SECURE, get_trust( ch ) );
    ch->pcdata->switched  = TRUE;
    ch->desc->character   = victim;
    ch->desc->original    = ch;
    victim->desc          = ch->desc;
    victim->prompt        = ch->prompt;
    victim->deaf          = ch->deaf;
    ch->desc              = NULL;
    sprintf( buf, "You switch into %s successfully.\n\r", victim->short_descr );
    send_to_char(AT_BLUE, buf, victim );
    return;
}



void do_return( CHAR_DATA *ch, char *argument )
{
    char	buf [ MAX_STRING_LENGTH ];
    
    if ( !ch->desc )
	return;

    if ( !ch->desc->original )
    {
	send_to_char(AT_WHITE, "You aren't switched.\n\r", ch );
	return;
    }

    sprintf( buf, "$N returns from %s.", ch->short_descr );
    wiznet( buf, ch->desc->original, NULL, WIZ_SWITCHES, WIZ_SECURE, get_trust( ch ) );
    send_to_char(AT_BLUE, "You return to your original body.\n\r", ch );
    ch->desc->original->pcdata->switched = FALSE;
    ch->desc->character                  = ch->desc->original;
    ch->desc->original                   = NULL;
    ch->desc->character->desc            = ch->desc;
    ch->prompt                           = NULL;
    ch->desc                             = NULL;
    return;
}



void do_mload( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA      *victim;
    MOB_INDEX_DATA *pMobIndex;
    char            arg  [ MAX_INPUT_LENGTH ];
    char            arg1 [ MAX_INPUT_LENGTH ];
    char            buf  [ MAX_STRING_LENGTH ];
    int             num;
    int             vnum;
    int             nMatch = 0;
    int             amt;
    int             count = 0;
    extern int      top_mob_index;
    
    one_argument( argument, arg );
    amt = number_argument( argument, arg1 );
    num = is_number( arg ) ? atoi( arg ) : 0;

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Syntax: mload <vnum>.\n\r", ch );
	return;
    }

    /* so you could mload <name> or <2.name> and it loads the proper one */
    if ( num == 0 )
    {
        for( vnum = 0; nMatch < top_mob_index; vnum++ )
        {
            if( ( pMobIndex = get_mob_index( vnum ) ) )
            {
                nMatch++;
                if( is_name( ch, arg1, pMobIndex->player_name ) )
                {
                    if( ++count == amt )
                    {
                        num = vnum;
                        break;
                    }
                }
            }
        }
    }

    if ( !( pMobIndex = get_mob_index( num ) ) )
    {
	send_to_char(AT_WHITE, "No mob has that vnum.\n\r", ch );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    sprintf( buf, "$N created %s (mobile)", victim->short_descr );
    wiznet( buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust( ch ) );
    sprintf( buf, "You have created %s!\n\r", victim->short_descr );    
    send_to_char(AT_RED, buf, ch );
    act(AT_RED, "$n has created $N!\n\r", ch, NULL, victim, TO_ROOM );
    return;
}



void do_oload( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA       *obj;
    OBJ_INDEX_DATA *pObjIndex;
    char            arg1 [ MAX_INPUT_LENGTH ];
    char            arg2 [ MAX_INPUT_LENGTH ];
    char	    arg3 [ MAX_INPUT_LENGTH ];
    char            arg  [ MAX_INPUT_LENGTH ];
    int             noi    = 1;
    int             in     = 1;
    int             num;
    int             level;
    int             vnum;
    int             count = 0;
    int             amt;
    int             nMatch = 0;
    extern int      top_obj_index;
    
    argument = one_argument( argument, arg1 );
    amt = number_argument( arg1, arg );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg2 );
    noi = is_number( arg3 ) ? atoi( arg3 ) : 1;
    num = is_number( arg1 ) ? atoi( arg1 ) : 0;
 
    if ( arg1[0] == '\0' )
    {
        send_to_char(AT_WHITE, "&RSyntax: &Woload <vnum> [number] [level].\n\r", ch );
        return;
    }
 
    /* so you can oload <name> or <2.name> and it loads the proper one */
    if ( num == 0 )
    { 
        for( vnum = 0; nMatch < top_obj_index; vnum++ )
        {
            if( ( pObjIndex = get_obj_index(vnum) ) )
            {
                nMatch++;
                if ( ( is_name( ch, arg, pObjIndex->name ) ) )
                {
                    if ( ++count == amt )
                    {
                        num = vnum; 
                        break;
                    }
                }
            }
        }
    }

    if ( !( pObjIndex = get_obj_index( num ) ) )
    {
	send_to_char(AT_WHITE, "No object has that vnum.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
    	level = pObjIndex->level;
	/*level = get_trust( ch ); */
    }
    else
    {
	/*
	 * New feature from Alander.
	 */
        if ( !is_number( arg2 ) )
        {
	    send_to_char(AT_WHITE, "&RSyntax: &Woload <vnum> [number] [level].\n\r", ch );
	    return;
        }
        level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
        {
	    send_to_char(AT_WHITE, "Limited to your trust level.\n\r", ch );
	    return;
        }
    }

    for ( in = 1; in <= noi; in++ )
    {
      obj = create_object( pObjIndex, level );
      if ( CAN_WEAR( obj, ITEM_TAKE ) )
      {
  	obj_to_char( obj, ch );
      }
     else
      {
	obj_to_room( obj, ch->in_room );
      }
    }
    obj = create_object( pObjIndex, level );
      
    sprintf( log_buf, "$n has created %d $p%s!\n\r", noi, 
              noi > 1 ? "s" : "" );
	act(AT_RED, log_buf, ch, obj, NULL, TO_ROOM );
    sprintf( log_buf, "$N has created $p (Total: %d) (Object)", noi );
    wiznet( log_buf, ch, obj, WIZ_LOAD, WIZ_SECURE, get_trust( ch ) );
    sprintf( log_buf, "You have created %d %s%s!\n\r", noi,
	     obj->short_descr, noi > 1 ? "s" : "" ); 
    send_to_char(AT_RED, log_buf, ch );
    extract_obj( obj );
    return;
}



void do_purge( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	/* 'purge' */
        OBJ_DATA  *obj_next;
	CHAR_DATA *vnext;

	for ( victim = ch->in_room->people; victim; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( victim->deleted )
	        continue;

	    if ( IS_NPC( victim ) && victim != ch )
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->contents; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->deleted )
	        continue;
	    extract_obj( obj );
	}

	send_to_char(AT_RED, "Ok.\n\r", ch );
	act(AT_RED, "You purge the room!", ch, NULL, NULL, TO_CHAR);
	act(AT_RED, "$n purges the room!", ch, NULL, NULL, TO_ROOM);
	return;
    }


    if ( !( victim = get_char_room( ch, arg ) ) )
    {
    if ( ( obj = get_obj_list( ch, arg, ch->in_room->contents ) ) )
    {
    	act(AT_RED, "You purge $P.\n\r", ch, NULL, obj, TO_CHAR );
    	act(AT_RED, "$n purges $P.\n\r", ch, NULL, obj, TO_ROOM );
    	extract_obj( obj );
    	return;
    }
      if ( !( victim = get_char_world( ch, arg ) ) )
      {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
      }
    }
    
    if ( !IS_NPC( victim ) )
    {
	send_to_char(AT_WHITE, "Not on PC's.\n\r", ch );
	return;
    }
    
    act(AT_RED, "You purge $N.", ch, NULL, victim, TO_CHAR );
    act(AT_RED, "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
    extract_char( victim, TRUE );
    return;
}



void do_advance( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    int	       classes;
    int        level;
    int        iLevel;
    char       msgbuf [ MAX_STRING_LENGTH ];
       
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char(AT_WHITE, "Syntax: advance <char> <level>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	send_to_char(AT_WHITE, "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	return;
    }
    
    level = atoi( arg2 );
    classes = number_classes( victim );
    if ( level < 1 || level > MAX_LEVEL )
    {
	char buf [ MAX_STRING_LENGTH ];

	sprintf( buf, "Advance within range 1 to %d.\n\r", MAX_LEVEL );
	send_to_char(AT_WHITE, buf, ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char(AT_WHITE, "Limited to your trust level.\n\r", ch );
	return;
    }

    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     */
   if ( level < LEVEL_IMMORTAL )
   {
    if ( level <= victim->level )
    {
	int sn;

        if ( victim->level > LEVEL_MORTAL )
          do_help( victim, "demm_mortal" );
	
	send_to_char(AT_RED, "Lowering a player's level!\n\r", ch );
	sprintf( msgbuf, "$N has demoted %s to level %d!", victim->name,
	         level );
        wiznet( msgbuf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	send_to_char(AT_RED, "**** ARGHHHHHHHHHHHHHHH ****\n\r",    victim );
	victim->level    = 1;
	victim->exp      = classes == 1 ? 1000 : classes * 2000;
	victim->perm_hit = 10;
	victim->perm_mana = 100;
	victim->perm_bp   = 20;
	victim->perm_move = 100;
	for ( sn = 0; skill_table[sn].name[0] != '\0'; sn++ )
	  victim->pcdata->learned[sn] = 0;
	victim->practice = 0;
	victim->hit      = MAX_HIT(victim);
	victim->mana     = MAX_MANA(victim);
	victim->bp       = MAX_BP(victim);
	victim->move     = MAX_MOVE(victim);
	advance_level( victim );
    }
    else
    {
	send_to_char(AT_RED, "Raising a player's level!\n\r", ch );
	sprintf( msgbuf, "$N has promoted %s to level %d!", 
	         victim->name, level );
	wiznet( msgbuf, ch, NULL, WIZ_LEVELS, WIZ_SECURE, 0 );
	send_to_char(AT_RED, "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim );
    }

    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
	send_to_char(AT_RED, "You raise a level!!  ", victim );
	victim->level += 1;
	advance_level( victim );
    }
   }
    if ( ( level > 100 ) && ( level > victim->level ) )
    {
      victim->level = level;
      sprintf( log_buf, "advm_%d", level );
      do_help( victim, log_buf );
    }
    if ( ( level > 100 ) && ( level < victim->level ) )
    {
      victim->level = level;
      sprintf( log_buf, "demm_%d", level );
      do_help( victim, log_buf );
    }
    if ( victim->level <= LEVEL_HERO )
	victim->exp = classes == 1 ? 1000 * victim->level
				   : 2000 * classes * victim->level;
    else if ( victim->level <= L_CHAMP3 )
	{
	int base_xp = classes == 1 ? 100000 : 200000;
	int mod = 1;
	switch( victim->level )
	  {
	  case L_CHAMP1: mod = 4; break;
	  case L_CHAMP2: mod = 10; break;
	  case L_CHAMP3: mod = 20; break;
	  }
	victim->exp = base_xp * classes * mod;
	}
    else
	victim->exp = 25000000 + victim->level;
    return;
}



void do_trust( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    int        level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char(AT_WHITE, "Syntax: trust <char> <level>.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char(AT_WHITE, "That player is not here.\n\r", ch );
	return;
    }

    level = atoi( arg2 );

    if ( level < 0 || level > MAX_LEVEL )
    {
	char buf [ MAX_STRING_LENGTH ];

	sprintf( buf, "Trust within range 0 to %d.\n\r", MAX_LEVEL );
	send_to_char(AT_WHITE, buf, ch );
	return;
    }

    if ( level >= get_trust( ch ) )
    {
	send_to_char(AT_WHITE, "Limited to your trust.\n\r", ch );
	return;
    }

    victim->trust = level;
    return;
}



void do_restore( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Restore whom?\n\r", ch );
	return;
	 }

	 /* Restore All feature coded by Katrina*/
	 if ( !str_cmp( arg, "all" ) )
	 {
		 for ( victim = char_list; victim; victim = victim->next )
	{
   	  if ( victim->deleted )
	    continue;
	  if ( IS_NPC(victim) )
	   continue;

	  victim->hit  = MAX_HIT(victim);
	  victim->mana = MAX_MANA(victim);
	  victim->bp   = MAX_BP(victim);
	  victim->move = MAX_MOVE(victim);
	  if ( IS_AFFECTED( victim, AFF_BLIND ) )
	     {
	     AFFECT_DATA *paf;
	     for ( paf = victim->affected; paf; paf = paf->next )
	        {
		if ( paf->deleted )
		   continue;
		if ( paf->bitvector == AFF_BLIND )
		   {
		   affect_remove( victim, paf );
		   break;
		   }
	        }
	     }
	  if ( IS_AFFECTED( victim, AFF_POISON ) )
	     {
	     AFFECT_DATA *paf;
	     for ( paf = victim->affected; paf; paf = paf->next )
		{
		if ( paf->deleted )
		   continue;
		if ( paf->bitvector == AFF_POISON )
		   {
		   affect_remove( victim, paf );
		   break;
		   }
		}
	     }
	  update_pos( victim );
	  if(victim && victim->in_room)
	   act(AT_BLUE, "$n has restored you.", ch, NULL, victim, TO_VICT);
	}
       wiznet( "$N has restored everyone!", 
               ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust( ch ) );	  
       send_to_char(AT_BLUE, "Everybody has now been restored.\r\n", ch);
    }
    else
    {
	if ( !( victim = get_char_world( ch, arg ) ) )
	  {
		 send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
		 return;
	  }

	victim->hit  = MAX_HIT(victim);
	victim->mana = MAX_MANA(victim);
	victim->bp   = MAX_BP(victim);
	victim->move = MAX_MOVE(victim);
	if ( IS_AFFECTED( victim, AFF_BLIND ) )
	   {
	   AFFECT_DATA *paf;
	   for ( paf = victim->affected; paf; paf = paf->next )
		{
		if ( paf->deleted )
		   continue;
		if ( paf->bitvector == AFF_BLIND )
		   {
		   affect_remove( victim, paf );
		   break;
		   }
		}
	   }
	if ( IS_AFFECTED( victim, AFF_POISON ) )
	   {
	   AFFECT_DATA *paf;
	   for ( paf = victim->affected; paf; paf = paf->next )
		{
		if ( paf->deleted )
		   continue;
		if ( paf->bitvector == AFF_POISON )
		   {
		   affect_remove( victim, paf );
		   break;
		   }
		}
	   }	
	update_pos( victim );
	act(AT_BLUE, "$n has restored you.", ch, NULL, victim, TO_VICT );
	sprintf( buf, "$N restored %s", victim->name );
	wiznet( buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust( ch ) );
	sprintf( buf, "You restore %s successfully.\n\r", victim->name );
	send_to_char(AT_BLUE, buf, ch );
    }

    return;
}



void do_freeze( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Freeze whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char(AT_WHITE, "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_FREEZE ) )
    {
	REMOVE_BIT( victim->act, PLR_FREEZE );
	sprintf( buf, "$N removes %s's freeze bit.", victim->name );
	wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
        sprintf( buf, "FREEZE removed from %s.\n\r", victim->name );
	send_to_char(AT_LBLUE, buf, ch );
	send_to_char(AT_LBLUE, "You can play again.\n\r", victim );
    }
    else
    {
	SET_BIT(    victim->act, PLR_FREEZE );
	sprintf( buf, "$N freezes %s.", victim->name );
	wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
        sprintf( buf, "FREEZE set on %s.\n\r", victim->name );
	send_to_char(AT_LBLUE, buf, ch );
	send_to_char(AT_LBLUE, "You can't do ANYthing!\n\r", victim );
    }

    save_char_obj( victim, FALSE );

    return;
}



void do_log( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_RED, "Log whom?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	if ( fLogAll )
	{
	    fLogAll = FALSE;
	    send_to_char(AT_RED, "Log ALL off.\n\r", ch );
	}
	else
	{
	    fLogAll = TRUE;
	    send_to_char(AT_RED, "Log ALL on.\n\r",  ch );
	}
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	return;
    }

    /*
     * No level check, gods can log anyone.
     */
    if ( IS_SET( victim->act, PLR_LOG ) )
    {
	REMOVE_BIT( victim->act, PLR_LOG );
	send_to_char(AT_WHITE, "LOG removed.\n\r", ch );
    }
    else
    {
	SET_BIT(    victim->act, PLR_LOG );
	send_to_char(AT_WHITE, "LOG set.\n\r",     ch );
    }

    return;
}



void do_noemote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Noemote whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char(AT_WHITE, "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_NO_EMOTE ) )
    {
	REMOVE_BIT( victim->act, PLR_NO_EMOTE );
        sprintf(buf,"$N restores emotes to %s.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
	send_to_char(AT_RED, "NO_EMOTE removed.\n\r",    ch     );
	send_to_char(AT_RED, "You can emote again.\n\r", victim );
    }
    else
    {
	SET_BIT(    victim->act, PLR_NO_EMOTE );
        sprintf(buf,"$N revokes %s's emotes.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
	send_to_char(AT_RED, "You can't emote!\n\r",    victim );
	send_to_char(AT_RED, "NO_EMOTE set.\n\r",       ch     );
    }

    return;
}



void do_notell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Notell whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char(AT_WHITE, "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_NO_TELL ) )
    {
	REMOVE_BIT( victim->act, PLR_NO_TELL );
        sprintf(buf,"$N restores tells to %s.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
        send_to_char(AT_RED, "NO_TELL removed.\n\r",    ch );
	send_to_char(AT_RED, "You can tell again.\n\r", victim );
    }
    else
    {
	SET_BIT(    victim->act, PLR_NO_TELL );
        sprintf(buf,"$N revokes %s's tells.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
	send_to_char(AT_RED, "NO_TELL set.\n\r",        ch     );
	send_to_char(AT_RED, "You can't tell!\n\r",     victim );
    }

    return;
}



void do_silence( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    char       buf [ MAX_STRING_LENGTH ];
    
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Silence whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char(AT_WHITE, "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET( victim->act, PLR_SILENCE ) )
    {
	REMOVE_BIT( victim->act, PLR_SILENCE );
        sprintf( buf, "%s has unsilenced %s.", ch->name, victim->name );
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
	send_to_char(AT_RED, "You can use channels again.\n\r", victim );
	send_to_char(AT_RED, "SILENCE removed.\n\r",            ch     );
    }
    else
    {
	SET_BIT(    victim->act, PLR_SILENCE );
        sprintf( buf, "%s has silenced %s!", ch->name, victim->name );
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
	send_to_char(AT_RED, "You can't use channels!\n\r",     victim );
	send_to_char(AT_RED, "SILENCE set.\n\r",                ch     );
    }

    return;
}

/*
void do_fighting( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if (d->connected == CON_PLAYING)
	{
		CHAR_DATA *victim;

		victim = d->original ? d->original : d->character;

		sprintf(buf, "%s is fighting: %s\n\r", victim->name,
		    victim->fighting ? victim ->fighting->name : "(none)" );
		send_to_char(AT_WHITE, buf, ch);
	}
   }
}
*/	    
void do_peace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       buf [ MAX_STRING_LENGTH ];

    /* Yes, we are reusing rch.  -Kahn */
    if ( argument[0] == '\0' )
    {
      for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
      {
	if ( rch->fighting )
	    stop_fighting( rch, TRUE );
      }
   
      send_to_char(AT_BLUE, "You bring peace to the room.\n\r", ch );
      return;
    }

    if ( !str_cmp( argument, "world" ) )
    {
      for ( rch = char_list; rch; rch = rch->next )
      {
        if ( ch->desc == NULL || ch->desc->connected != CON_PLAYING )
         continue;

        if ( rch->fighting )
        {
          sprintf( buf, "%s has declared World Peace.\n\r", ch->name );
          send_to_char( AT_BLUE, buf, rch );
          stop_fighting( rch, TRUE );
        }
      }

      send_to_char( AT_BLUE, "You have declared World Peace.\n\r", ch );
      return;
    }                                                                                                                              
    do_help( ch, "peace" );
    return;
}



BAN_DATA *		ban_free;
BAN_DATA *		ban_list;
void do_permban( CHAR_DATA *ch, char *argument )
{
    ban( ch, argument, 'P' );
    return;
}
void do_tempban( CHAR_DATA *ch, char *argument )
{
    ban( ch, argument, 'T' );
    return;
}
void do_newban( CHAR_DATA *ch, char *argument )
{
    ban( ch, argument, 'N' );
    return;
}

void ban( CHAR_DATA *ch, char *argument, char ban_type )
{
    BAN_DATA  *pban;
/*    BAN_DATA  *to_ban;*/
    char       buf [ MAX_STRING_LENGTH ];
    char       arg [ MAX_INPUT_LENGTH  ];
    char       arg1[ MAX_INPUT_LENGTH  ];

    if ( IS_NPC( ch ) )
	return;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

    if ( arg[0] == '\0' )
    {
	sprintf( buf, "%s banned sites and users:\n\r&R",
		 ban_type == 'P' ? "Perm" : ban_type == 'T' ? "Temp"
							    : "Newban" );
	send_to_char(AT_BLOOD, buf, ch );
	for ( pban = ban_list; pban; pban = pban->next )
	{
	    if ( pban->type == ban_type )
		{
                if ( pban->user )
                  sprintf( buf, "-> &R%s&W@&R%s\n\r", pban->user, pban->name );
                else
                  sprintf( buf, "-> &R%s\n\r", pban->name );
                send_to_char(AT_WHITE, buf, ch );
		}
	}
	buf[0] = '\0';
/*	sprintf( buf, "Syntax: %s [username@]site.to.ban\n\r",
		 ban_type == 'P' ? "Permban" : ban_type == 'T'
				 ? "Tempban" : "Newban" ); */
	sprintf( buf, "Syntax: %s site.to.ban [username]\n\r",
		 ban_type == 'P' ? "Permban" : ban_type == 'T'
				 ? "Tempban" : "Newban" ); 
	send_to_char(C_DEFAULT, buf, ch );
	send_to_char(C_DEFAULT, 
	"User being the optional name of a specific person to ban.\n\r", ch );
/*	send_to_char(C_DEFAULT, 
	"You MUST use the @ symbol if you specify a username.\n\r", ch );*/
	return;
    }
/*
    parse_ban( argument, to_ban ); 
    for ( pban = ban_list; pban; pban = pban->next )
	{
	if ( ( to_ban->name == pban->name && !to_ban->user )
	|| ( to_ban->user && pban->user 
			  && to_ban->user == pban->user
			  && to_ban->name == pban->name ) )
	  {
	  if ( to_ban->user )
	    send_to_char( AT_RED, "The user from that site is already banned.\n\r", ch );
	  else
	    send_to_char( AT_RED, "That site is already banned.\n\r", ch );
	  }
	}
*/	    
    for ( pban = ban_list; pban; pban = pban->next )
    {
	if ( ( !str_cmp( arg, pban->name ) && arg1[0] == '\0' ) 
	  || ( arg1[0] != '\0' && pban->user && !str_cmp( arg1, pban->user ) 
	  && !str_cmp( arg, pban->name ) ) )
	{
	    send_to_char(AT_RED, "That site is already banned!\n\r", ch );
	    return;
	}
    }

    if ( !ban_free )
    {
	pban		= alloc_perm( sizeof( *pban ) );
    }
    else
    {
	pban		= ban_free;
	ban_free	= ban_free->next;
    }
    pban->type	= ban_type;
    parse_ban( argument, pban );
    pban->name	= str_dup( arg );
    if ( arg1[0] != '\0' )
      pban->user = str_dup( arg1 );
    pban->next	= ban_list;
    ban_list	= pban;
    save_banlist( ban_list );
    send_to_char(AT_RED, "Ok.\n\r", ch );
    return;
}
void do_banlist( CHAR_DATA *ch, char *argument )
{
	BAN_DATA *pban;
	char buf [ MAX_INPUT_LENGTH ];
	sprintf( buf, "Banned sites and users:\n\r" );
	send_to_char(AT_BLOOD, buf, ch );
	for ( pban = ban_list; pban; pban = pban->next )
	{
                if ( pban->user )
                  sprintf( buf, "Ban type: &R%c&W -> &R%s&W@&R%s\n\r",
                           pban->type, pban->user, pban->name );
                else
                  sprintf( buf, "Ban type: &R%c&W -> &R%s\n\r",
                           pban->type, pban->name );
                send_to_char(AT_GREY, buf, ch );
	}
	return;
}


void do_allow( CHAR_DATA *ch, char *argument )
{
    BAN_DATA  *prev;
    BAN_DATA  *curr;
    char       arg [ MAX_INPUT_LENGTH ];
    char     arg1 [ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

    if ( arg[0] == '\0' )
    {
	do_banlist( ch, "" );
	send_to_char(AT_RED, "Remove which site or user from the ban list?\n\r", ch );
	send_to_char(C_DEFAULT, "Syntax: Allow banned.site [username]\n\r", ch );
	send_to_char(C_DEFAULT, "Username being the optional banned user (if applicable).\n\r", ch );
	return;
    }

    prev = NULL;
    for ( curr = ban_list; curr; prev = curr, curr = curr->next )
    {
	if ( ( !str_cmp( arg, curr->name ) && !curr->user && 
	   arg1[0] == '\0' ) || 
	( curr->user && !str_cmp( arg, curr->name ) &&
	    !str_cmp( arg1, curr->user ) ) )
	{
	    if ( curr->type == 'P' && get_trust( ch ) < L_IMP )
		{
		send_to_char( AT_RED, "You cannot remove a perm ban.\n\r", ch );
		return;
		}
	    if ( !prev )
		ban_list   = ban_list->next;
	    else
		prev->next = curr->next;

	    free_string( curr->name );
	    if ( curr->user )
	      free_string( curr->user );
/*	    curr->next	= ban_free;
	    ban_free	= curr;*/
	    free_mem( curr, sizeof( *curr ) );
	    save_banlist( ban_list );
	    send_to_char(AT_RED, "Site removed.\n\r", ch );
	    return;
	}
    }

    send_to_char(AT_RED, "That site or user is not banned.\n\r", ch );
    return;
}



void do_wizlock( CHAR_DATA *ch, char *argument )
{
    extern bool       wizlock;

    wizlock = !wizlock;

    if ( wizlock )
	send_to_char(AT_BLUE, "Game wizlocked.\n\r", ch );
    else
	send_to_char(AT_BLUE, "Game un-wizlocked.\n\r", ch );

    return;
}



void do_slookup( CHAR_DATA *ch, char *argument )
{
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg  [ MAX_INPUT_LENGTH ];
    int        sn;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Slookup what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	for ( sn = 0; skill_table[sn].name[0] != '\0'; sn++ )
	{
	    if ( !skill_table[sn].name )
		break;
	    sprintf( buf, "Slot: %4d Sn: %4d Skill/spell: '%s'\n\r",
		    skill_table[sn].slot, sn, skill_table[sn].name );
	    send_to_char(AT_WHITE, buf, ch);
	}
    }
    else
    {
        int ccount;

	if ( ( sn = skill_lookup( arg ) ) < 0 )
	{
	    send_to_char(AT_WHITE, "No such skill or spell.\n\r", ch );
	    return;
	}

	sprintf( buf, "Slot: %4d Sn: %4d Skill/spell: '%s'\n\r",
		skill_table[sn].slot, sn, skill_table[sn].name );
	send_to_char(AT_WHITE, buf, ch );
	for ( ccount = 0; ccount < MAX_CLASS; ccount++ )
	{
	  sprintf( buf, "%s: %3d   ", class_table[ccount].who_name,
		  skill_table[sn].skill_level[ccount] );
	  send_to_char(AT_WHITE, buf, ch );
	}
	send_to_char(AT_WHITE, "\n\r", ch );
    }

    return;
}



void do_sset( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg1  [ MAX_INPUT_LENGTH ];
    char       arg2  [ MAX_INPUT_LENGTH ];
    char       arg3  [ MAX_INPUT_LENGTH ];
    int        value;
    int        sn;
    bool       fAll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Syntax: sset <victim> <skill> <value>\n\r",	ch );
	send_to_char(AT_WHITE, "or:     sset <victim> all     <value>\n\r",	ch );
	send_to_char(AT_WHITE, "Skill being any skill or spell.\n\r",		ch );
	return;
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char(AT_WHITE, "No such skill or spell.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char(AT_WHITE, "Value must be numeric.\n\r", ch );
	return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
	send_to_char(AT_WHITE, "Value range is 0 to 100.\n\r", ch );
	return;
    }

    if ( fAll )
    {
        if ( get_trust( ch ) < L_DIR )
	{
	    send_to_char(AT_WHITE, "Only Imps may sset all.\n\r", ch );
	    return;
	}
	for ( sn = 0; skill_table[sn].name[0] != '\0'; sn++ )
	{
	    if ( skill_table[sn].name )
	        if ( can_use_skpell( victim, sn ) )
		    victim->pcdata->learned[sn]	= value;
	}
    }
    else
    {
	victim->pcdata->learned[sn] = value;
    }

    return;
}

void do_cset( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA *pClan;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    char       arg3 [ MAX_INPUT_LENGTH  ];
    int        value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char(AT_WHITE, "Syntax: &Bcset <victim> <field>  <value>\n\r", ch );
        send_to_char(AT_WHITE, "\n\r",                                         ch );
        send_to_char(AT_WHITE, "&pField being one of:\n\r",                    ch );
        send_to_char(AT_WHITE, "  clan clvl\n\r",                              ch );
        return;
    }


    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
      send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
      return;
    }

    value = is_number( arg3 ) ? atoi( arg3 ) : -1;


    if (    !str_cmp( arg2, "clvl" )
         && (    is_name(NULL, "cset", ch->pcdata->empowerments)
              || ch->level == L_IMP                          ) )
    {
       if IS_NPC( victim)
       {
          send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
          return;
       }
       victim->clev = value;
       send_to_char(AT_WHITE, "Ok.\n\r", ch );
       return;
    }
    else if ( !str_cmp( arg2, "clvl" ) )
    {
       send_to_char(AT_WHITE, "You are not empowered to set one's clan level.\n\r", ch );
       return;
    }

    argument = one_argument( argument, arg3 );

    for ( pClan = clan_first->next; pClan; pClan = pClan->next )
    {
        if ( !str_cmp( arg3, strip_color( pClan->name ) ) )
        {
           value = pClan->vnum;
           break;
        }
    }

    if (    !str_cmp( arg2, "clan" ) 
         && (    is_name(NULL, "cset", ch->pcdata->empowerments)
              || ch->level == L_IMP                          ) )
    {
       if IS_NPC( victim )
       {
          send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
          return;
       }
       if ( !get_clan_index(value) )
       {
          send_to_char(AT_WHITE, "Invalid clan.\n\r", ch );
          return;
       }
       victim->clan = value;
       send_to_char(AT_WHITE, "Ok.\n\r", ch );
       return;
    }
    else if ( !str_cmp( arg2, "clan" ) )
    {
       send_to_char(AT_WHITE, "You are not empowered to set one's clan.\n\r", ch);
       return;
    }

 return;
}

void do_mset( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob = NULL;
    CHAR_DATA *victim;
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    char       arg3 [ MAX_INPUT_LENGTH  ];
    int        value;
    int        max;
    bool       p = FALSE;
#ifdef NEW_MONEY
    char       arg4 [ MAX_STRING_LENGTH ];
#endif
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( !str_cmp( arg2, "bank" ) )
    {
       argument = one_argument( argument, arg3 );
       argument = one_argument( argument, arg4 );
    }
    else
       strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Syntax: mset <victim> <field>  <value>\n\r",	 ch );
	send_to_char(AT_WHITE, "or:     mset <victim> <string> <value>\n\r",	 ch );
	send_to_char(AT_WHITE, "or:     mset <victim> class <slot> <value>\n\r", ch );
	send_to_char(AT_WHITE, "\n\r",						 ch );
	send_to_char(AT_WHITE, "&pField being one of:\n\r",			 ch );
	send_to_char(AT_WHITE, "  str int wis dex con level gold silver\n\r",    ch );
	send_to_char(AT_WHITE, "  copper hp mana blood move practice align\n\r", ch );
        send_to_char(AT_WHITE, "  mhp mmana mblood mmove\n\r", 			 ch );
	send_to_char(AT_WHITE, "  thirst drunk full security affected_by2\n\r",	 ch );
	send_to_char(AT_WHITE, "  affected_by act mstr mint mwis\n\r",  	 ch );
	send_to_char(AT_WHITE, "  mdex mcon bank carryn carryw save race\n\r",   ch );
	send_to_char(AT_WHITE, "  lname sex salign\n\r",			 ch );
	send_to_char(AT_WHITE, "&pString being one of:\n\r",			 ch );
	send_to_char(AT_WHITE, "  name short long title spec\n\r",               ch );
	send_to_char(AT_WHITE, "&pSlot being 1-2 class slot.\n\r", ch );
	send_to_char(AT_WHITE, "  example: mset self class 1 warrior\n\r", ch );
	return;
    }

/*    if ( !is_number( arg1 ) )*/
      if ( !( victim = get_char_room( ch, arg1 ) ) )
      {
	send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	return;
      }
/*    else
      if ( !(pMob = get_mob_index( atoi( arg1  )) ) )
      {
         send_to_char(AT_WHITE, "Nothing like that in hell, earth, or heaven.\n\r", ch );
         return;
      }
      else 
      {
         p = TRUE;
         victim = get_char_world( ch, pMob->player_name );
      }*/
     
    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    /*
     * Set something.
     */
     if ( !str_cmp( arg2, "qtime1" ) )
	{
	victim->nextquest = value;
	return;
	}
     if ( !str_cmp( arg2, "qtime2" ) )
	{
	victim->countdown = value;
	return;
	}
    if ( !str_cmp( arg2, "sex" ) )
    {
      if ( IS_NPC( victim ) )
	return;
      if ( !str_cmp( arg3, "male" ) )
	value = 1;
      else if ( !str_cmp( arg3, "female" ) )
	value = 2;
      else if ( !str_cmp( arg3, "neutral" ) )
	value = 0;
      else
	{
	send_to_char( AT_WHITE, "Valid sexes are Male, Female & Neutral.\n\r", ch );
	return;
	}
      victim->sex = value;
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      return;
    }
    if ( !str_cmp( arg2, "salign" ) )
    {
      if ( IS_NPC(victim) )
      {
        send_to_char( AT_WHITE, "Not on NPC's.\n\r", ch );
        return;
      }
      if ( !*arg3 )
      {
        send_to_char( AT_WHITE, "Set salign to what?\n\r", ch );
        return;
      }
      if ( !str_prefix(arg3, "good") || !str_prefix(arg3, "neutral") ||
	   !str_prefix(arg3, "evil") )
        victim->start_align = UPPER(*arg3);
      else
      {
        send_to_char( AT_WHITE, "Invalid salign.\n\r"
            "Salign is Good/Neutral/Evil.\n\r", ch );
        return;
      }
      send_to_char( AT_WHITE, "Salign set.\n\r", ch );
      return;
    }
    if ( !str_cmp( arg2, "lname" ) )
    {
      if ( IS_NPC( victim ) )
        return;
      if ( victim->pcdata->lname )
        free_string( victim->pcdata->lname );
      if ( !str_cmp( "none", arg3 ) )
      {
	victim->pcdata->lname = NULL;
        send_to_char( AT_WHITE, "Cleared.\n\r", ch );
        return;
      }
      victim->pcdata->lname = str_dup( arg3 );
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      return;
    }
    
    if ( !str_cmp( arg2, "carryn" ) )
    {
      victim->carry_number = value;
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      return;
    }
    
    if ( !str_cmp( arg2, "carryw" ) )
    {
      victim->carry_weight = value;
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      return;
    }
    
    if ( !str_cmp( arg2, "bank" ) )
    {
      if ( IS_NPC( victim ) )
        return;
#ifdef NEW_MONEY

      if ( is_number( arg4 ) )
      {
        if ( !str_cmp( arg3, "gold" ) )
          victim->pcdata->bankaccount.gold = atoi( arg4 );
	else if ( !str_cmp( arg3, "silver" ) )
	  victim->pcdata->bankaccount.silver = atoi( arg4 );
	else if ( !str_cmp( arg3, "copper" ) )
	  victim->pcdata->bankaccount.copper = atoi( arg4 );
	else
	{
	  send_to_char( AT_WHITE, "Invalid currency type.\n\r", ch );
	  send_to_char( AT_WHITE, "&WSyntax: mset <victim> <bank> <currency_type> <amount>\n\r",
		   ch );
	  return;
	}
      }
      else
      {
	send_to_char( AT_WHITE, "Invalid amount.\n\r", ch );
	send_to_char( AT_WHITE, "&WSyntax: mset &R<victim> <bank> <currency_type> <amount>\n\r", 
                   ch );
	return;
      }
#else
      victim->pcdata->bankaccount = value;
#endif
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      return;
    }

    if ( !str_cmp( arg2, "save" ) )
    {
      if ( IS_NPC( victim ) )
        return;
      victim->saving_throw = value;
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      return;
    }
    if ( !str_cmp( arg2, "mstr" ) )
    {
      if ( IS_NPC( victim ) )
      {
         send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
         return;
      }
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      victim->pcdata->mod_str = value;
      return;
    }
    
    if ( !str_cmp( arg2, "mint" ) )
    {
      if ( IS_NPC( victim ) )
      {
        send_to_char( AT_WHITE, "Not on NPC's.\n\r", ch );
        return;
      }
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      victim->pcdata->mod_int = value;
      return;
    }
    
    if ( !str_cmp( arg2, "mwis" ) )
    {
      if ( IS_NPC( victim ) )
      {
	send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
        return;
      }
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      victim->pcdata->mod_wis = value;
      return;
    }
    
    if ( !str_cmp( arg2, "mdex" ) )
    {
      if ( IS_NPC( victim ) )
      {
        send_to_char(AT_WHITE, "Not on NPCS's\n\r", ch );
        return;
      }
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      victim->pcdata->mod_dex = value;
      return;
    }

    if ( !str_cmp( arg2, "mcon" ) )
    {
      if (IS_NPC( victim ) )
      {
        send_to_char( AT_WHITE, "Not on NPC's\n\r", ch );
        return;
      }
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      victim->pcdata->mod_con = value;
      return;
    }
    
    if ( !str_cmp( arg2, "str" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[prime_class(victim)].attr_prime == APPLY_STR )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Strength range is 3 to %d.\n\r", max );
	    send_to_char(AT_WHITE, buf, ch );
	    return;
	}

	victim->pcdata->perm_str = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "int" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[prime_class(victim)].attr_prime == APPLY_INT )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Intelligence range is 3 to %d.\n\r", max );
	    send_to_char(AT_WHITE, buf, ch );
	    return;
	}

	victim->pcdata->perm_int = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "wis" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[prime_class(victim)].attr_prime == APPLY_WIS )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Wisdom range is 3 to %d.\n\r", max );
	    send_to_char(AT_WHITE, buf, ch );
	    return;
	}

	victim->pcdata->perm_wis = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "dex" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[prime_class(victim)].attr_prime == APPLY_DEX )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Dexterity range is 3 to %d.\n\r", max );
	    send_to_char(AT_WHITE, buf, ch );
	    return;
	}

	victim->pcdata->perm_dex = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "con" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( class_table[prime_class(victim)].attr_prime == APPLY_CON )
	    max = 25;
	else
	    max = 18;

	if ( value < 3 || value > max )
	{
	    sprintf( buf, "Constitution range is 3 to %d.\n\r", max );
	    send_to_char(AT_WHITE, buf, ch );
	    return;
	}

	victim->pcdata->perm_con = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }
    
/*    if ( !str_cmp( arg2, "clvl" ) && get_trust( ch ) >= L_CON )
    {
       if IS_NPC( victim)
       {
          send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
          return;
       }
       victim->clev = value;
       send_to_char(AT_WHITE, "Ok.\n\r", ch );
       return;
    }
    else if ( !str_cmp( arg2, "clvl" ) && get_trust( ch ) < L_CON )
    {
       send_to_char(AT_WHITE, "You are too low of trust to set one's clan level.\n\r", ch );
       return;
    }
       
    if ( !str_cmp( arg2, "clan" ) && get_trust( ch ) >= L_CON )
    {
       if IS_NPC( victim )
       {
          send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
          return;
       }
*/       /*if ( ( value < 0 ) || ( value > MAX_CLAN ) )*/
/*       if ( !get_clan_index(value) )
       {
          send_to_char(AT_WHITE, "Invalid clan.\n\r", ch );
          return;
       }
       victim->clan = value;
       send_to_char(AT_WHITE, "Ok.\n\r", ch );
       return;
    }
    else if ( !str_cmp( arg2, "clan" ) && get_trust( ch ) < L_CON )
    {
       send_to_char(AT_WHITE, "You are too low of trust to set one's clan.\n\r", ch );
       return;
    }
*/    
    if ( !str_cmp( arg2, "act" ) )
    {   
       if (!IS_NPC( victim ) )
       if ( get_trust( ch ) < L_DIR /*&& !IS_SET( ch->affected_by2, CODER
)*/ )
       {
          send_to_char(AT_WHITE, "You are too low of trust to set ones actflags.\n\r", ch );
          return;
       }
       if (IS_NPC( victim ) )
       {
          if (!str_cmp( arg3, "sentinel" ) || !str_cmp( arg3, "se" ) )
	    value = 2;
          if (!str_cmp( arg3, "scavenger" ) || !str_cmp( arg3, "sc" ) )
            value = 4;
          if (!str_cmp( arg3, "aggressive" ) || !str_cmp( arg3, "ag" ) )
            value = 32;
          if (!str_cmp( arg3, "stayarea" ) || !str_cmp( arg3, "sa" ) )
            value = 64;
          if (!str_cmp( arg3, "wimpy" ) || !str_cmp( arg3, "w" ) )
            value = 128;
          if (!str_cmp( arg3, "pet" ) || !str_cmp( arg3, "pe" ) )
            value = 256;
          if (!str_cmp( arg3, "trainer" ) || !str_cmp( arg3, "t" ) )
            value = 512;
          if (!str_cmp( arg3, "practicer" ) || !str_cmp( arg3, "pr" ) )
            value = 1024;
          if (!str_cmp( arg3, "gambler" ) || !str_cmp( arg3, "g" ) )
            value = 2048;
          if ( (!str_cmp( arg3, "prototype" ) || !str_cmp( arg3, "pro" ) )
            && ( get_trust( ch ) >= L_CON 
		/*|| IS_SET( ch->affected_by2, CODER )*/ ) )
            value = 4096;
       }
       
       if (!IS_NPC( victim ) )
       {
           if (!str_cmp( arg3, "boughtpet" ) || !str_cmp( arg3, "bp" ) )
             value = 2;
           if (!str_cmp( arg3, "autoexit" ) || !str_cmp( arg3, "ae" ) )
             value = 8;
           if (!str_cmp( arg3, "autoloot" ) || !str_cmp( arg3, "al" ) )
             value = 16;
           if (!str_cmp( arg3, "autosac" ) || !str_cmp( arg3, "as" ) )
             value = 32;
           if (!str_cmp( arg3, "blank" ) || !str_cmp( arg3, "bl" ) )
             value = 64;
           if (!str_cmp( arg3, "brief" ) || !str_cmp( arg3, "br" ) )
             value = 128;
           if (!str_cmp( arg3, "combine" ) || !str_cmp( arg3, "c" ) )
	     value = 512;
           if (!str_cmp( arg3, "prompt" ) || !str_cmp( arg3, "p" ) )
             value = 1024;
         if (!str_cmp( arg3, "telnetga" ) || !str_cmp( arg3, "tga" ) )
           value = 2048;
         if (!str_cmp( arg3, "holylight" ) || !str_cmp( arg3, "h" ) )
           value = 4096;
         if (!str_cmp( arg3, "wizinvis" ) || !str_cmp( arg3, "w" ) )
           value = 8192;
         if (!str_cmp( arg3, "cloaked" ) || !str_cmp( arg3, "cl" ) )
           value = BV17;
         if (!str_cmp( arg3, "thief" ) || !str_cmp( arg3, "t" ) )
           value = 4194304;
         if (!str_cmp( arg3, "killer" ) || !str_cmp( arg3, "k" ) )
           value = 8388608;
         if (!str_cmp( arg3, "ansi" ) || !str_cmp( arg3, "a" ) )
           value = 16777216;
         if (!str_cmp( arg3, "autocoins" ) || !str_cmp( arg3, "aco" ) )
           value = 33554432;
	 if (!str_cmp( arg3, "keeptitle" ) || !str_cmp( arg3, "kt" ) )
           value = 67108864;
         if (!str_cmp( arg3, "undead" ) || !str_cmp( arg3, "u" ) )
           value = 134217728;
         if (!str_cmp( arg3, "darkinvis" ) || !str_cmp( arg3, "d" ) )
           value = 268435456;
	 if (!str_cmp( arg3, "pkiller" ) || !str_cmp( arg3, "pk" ) )
	   value = 536870912;
       }
       
       if (IS_NPC( victim ) )
       if ( ( value & ACT_PROTOTYPE ) && ( get_trust( ch ) < L_CON ) )
       {
          send_to_char(AT_WHITE, "You cannot toggle the prototype flag at your current level.\n\r", ch );
          return;
       }
            
     /*  if ( (unsigned) 4294967295 & value ) */
       if ( 0xFFFFFFFF & value )
       {
         if (p) 
         {
           pMob->act ^= value;
           SET_BIT( victim->in_room->area->area_flags, AREA_CHANGED );
         }
          else
         {victim->act ^= value;}
         send_to_char(AT_WHITE, "Ok.\n\r", ch );
         return;
       }
       else
       {
         send_to_char(AT_WHITE, "Invalid bit.", ch );
         return;
       }
    }
    
    if ( !str_cmp( arg2, "affected_by" ) )
    {
       if ( (get_trust( ch ) < L_DIR) && (!IS_NPC( victim )) )
       {
	  send_to_char(AT_WHITE, "You cannot set a player's affected_by from your current level.\n\r", ch );
          return;
       }
       if (p)
       { pMob->affected_by ^= value; }
       else
       { 
       if ( value == 0 ) 
         victim->affected_by = 0;
        else
         victim->affected_by ^= value; 
       }
       send_to_char(AT_WHITE, "Ok.\n\r", ch );
       return;
    }
    
    if ( !str_cmp( arg2, "affected_by2" ) )
    {
       if ( get_trust( ch ) < L_IMP && !IS_NPC( victim ) )
       {
	  send_to_char(AT_WHITE, "You are too low of level to set a player's affected_by2.\n\r", ch );
          return;
       }
       if (p)
       { pMob->affected_by2 ^= value;
         SET_BIT( victim->in_room->area->area_flags, AREA_CHANGED ); }
       else
       { 
       if ( value == 0 )
         victim->affected_by2 = 0;
        else
         victim->affected_by2 ^= value; 
       }
       send_to_char(AT_WHITE, "Ok.\n\r", ch );
       return;
    }

    if ( !str_cmp( arg2, "class" ) )
    {
      char arg4[ MAX_INPUT_LENGTH ];
      char buf [ MAX_STRING_LENGTH ];
      int slot, value, iClass;
      argument = one_argument( argument, arg3 );
      strcpy( arg4, argument );
      slot = ( is_number( arg3 ) ) ? atoi( arg3 ) : -1;
      value = ( is_number( arg4 ) ) ? atoi( arg4 ) : -1;
      if ( value < -1 )
	value = -1;
      if ( slot < 1 || slot > 2 )
	{
	send_to_char( AT_WHITE, "Invalid slot.  Valid slot: 1-2.\n\r", ch );
	return;
	}
      for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	if ( !str_prefix( arg4, class_table[iClass].who_long )
	|| !str_cmp( arg4, class_table[iClass].who_name ) )
	  {
          value = iClass;
	  break;
	  }
	}
      if ( slot == 1 && value == -1 )
	{
	send_to_char( AT_WHITE, "Cannot turn off 1st Class.\n\r", ch );
	return;
	}
      if ( slot == 2 && value == -1 && victim->class[2] != -1 )
	{
	send_to_char( AT_WHITE, "You must turn off 3rd Class before 2nd.\n\r", ch );
	return;
	}
      if ( slot == 3 && victim->class[1] == -1 )
	{
	act( AT_WHITE,
	     "Cannot set 3rd Class for $N.  Player does not have a 2nd Class.",
	     ch, NULL, victim, TO_CHAR );
	return;
	}
      if ( value < -1 || value > MAX_CLASS )
	{
	    sprintf( buf, "Class range is 0 to %d.\n\r", MAX_CLASS-1 );
	    send_to_char(AT_WHITE, buf, ch );
	    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		{
		sprintf( buf, " %2d = %s\n\r", iClass,
			 class_table[iClass].who_long );
		send_to_char( AT_WHITE, buf, ch );
		}
	    send_to_char( AT_WHITE, " -1 = Turns off class.\n\r", ch );
	    return;
	}
	if ( value != -1 && !class_table[value].races[victim->race] )
	  {
	  sprintf( buf, "Player's race (%s) does not allow class %s.\n\r",
	           (get_race_data(victim->race))->race_full,
		   class_table[value].who_long );
	  send_to_char( AT_WHITE, buf, ch );
	  return;
	  }
      if ( value != -1 && is_class( victim, value ) )
	{
	sprintf ( buf, "$N is already %s %s.",
		( victim->class[1] != -1 ) ? "part" : "a",
		class_table[value].who_long );
	act( AT_WHITE, buf, ch, NULL, victim, TO_CHAR );
	return;
	}
	victim->class[slot-1] = value;
	if ( slot == 3 )
	  victim->class[slot] = -1;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "race" ) )
    {
      int iRace;
      RACE_DATA *pRace;
      for ( iRace = 0; iRace < MAX_RACE; iRace++ )
	{
	if ( !str_cmp( arg3, (get_race_data(iRace))->race_full )
	|| !str_cmp( arg3, (get_race_data(iRace))->race_name ) )
	   { 
	   value = iRace; 
	   break; 
	   }
	}
/*
      if ( !str_cmp( arg3, "human" ) || !str_cmp( arg3, "hum" ) )
         value = RACE_HUMAN;
      if ( !str_cmp( arg3, "elf" ) )
         value = RACE_ELF;
      if ( !str_cmp( arg3, "dwarf" ) || !str_cmp( arg3, "dwa" ) )
         value = RACE_DWARF;
      if ( !str_cmp( arg3, "pixie" ) || !str_cmp( arg3, "pix" ) )
         value = RACE_PIXIE;
      if ( !str_cmp( arg3, "halfling" ) || !str_cmp( arg3, "hal" ) )
         value = RACE_HALFLING;
      if ( !str_cmp( arg3, "drow" ) )
         value = RACE_DROW;
      if ( !str_cmp( arg3, "elder" ) || !str_cmp( arg3, "eld" ) )
         value = RACE_ELDER;
      if ( !str_cmp( arg3, "ogre" ) || !str_cmp( arg3, "ogr" ) )
         value = RACE_OGRE;
      if ( !str_cmp( arg3, "lizardman" ) || !str_cmp( arg3, "liz" ) )
         value = RACE_LIZARDMAN;
      if ( !str_cmp( arg3, "demon" ) || !str_cmp( arg3, "dem" ) )
         value = RACE_DEMON;
      if ( !str_cmp( arg3, "ghoul" ) || !str_cmp( arg3, "ghl" ) )
	 value = RACE_GHOUL;
      if ( !str_cmp( arg3, "illithid" ) || !str_cmp( arg3, "ill" ) )
         value = RACE_ILLITHID;
      if ( !str_cmp( arg3, "minotaur" ) || !str_cmp( arg3, "min" ) )
         value = RACE_MINOTAUR;
      if ( !str_cmp( arg3, "troll" ) || !str_cmp( arg3, "tro" ) )
         value = RACE_TROLL;
      if ( !str_cmp( arg3, "shadow" ) || !str_cmp( arg3, "shd" ) )
	 value = RACE_SHADOW;
*/
	if ( value < 0 || value >= MAX_RACE )
	{
	    char buf [ MAX_STRING_LENGTH ];

	    sprintf( buf, "Race range is 0 to %d.\n", MAX_RACE-1 );
	    send_to_char(AT_WHITE, buf, ch );
	    for ( iRace = 0; iRace < MAX_RACE; iRace++ )
		{
		sprintf( buf, " %2d = %s\n\r", iRace,
			 (get_race_data(iRace))->race_full );
		send_to_char( AT_WHITE, buf, ch );
		}
	    return;
	}
	
      pRace = get_race_data(victim->race);
      victim->pcdata->mod_str -= pRace->mstr;
	victim->pcdata->mod_int -= pRace->mint;
	victim->pcdata->mod_wis -= pRace->mwis;
	victim->pcdata->mod_dex -= pRace->mdex;
	victim->pcdata->mod_con -= pRace->mcon;
	if ( victim->race == RACE_GHOUL )
		REMOVE_BIT( victim->act, PLR_UNDEAD );

	victim->race = value;
	if ( victim->race == RACE_GHOUL )
		SET_BIT( victim->act, PLR_UNDEAD );
      pRace = get_race_data(victim->race);
	victim->pcdata->mod_str += pRace->mstr;
	victim->pcdata->mod_int += pRace->mint;
	victim->pcdata->mod_wis += pRace->mwis;
	victim->pcdata->mod_dex += pRace->mdex;
	victim->pcdata->mod_con += pRace->mcon;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on PC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char(AT_WHITE, "Level range is 0 to 50.\n\r", ch );
	    return;
	}
	victim->level = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }
#ifdef NEW_MONEY
    if ( !str_cmp( arg2, "gold" ) )
    {
	victim->money.gold = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "silver" ) )
    {
	victim->money.silver = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "copper" ) )
    {
	victim->money.copper = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }
#else
    if ( !str_cmp( arg2, "gold" ) )
    {
	victim->gold = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }
#endif
    if ( !str_cmp( arg2, "hp" ) )
    {
	if ( value < -10 || value > 30000 )
	{
	    send_to_char(AT_WHITE, "Hp range is -10 to 30,000 hit points.\n\r", ch );
	    return;
	}
	if ( victim->fighting && value < 0 )
	{
	    send_to_char(AT_WHITE, "You cannot set a fighting person's hp below 0.\n\r",
			 ch );
	    return;
	}
	victim->perm_hit = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "mhp" ) )
    {
	victim->mod_hit = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "mana" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char(AT_WHITE, "Mana range is 0 to 30,000 mana points.\n\r", ch );
	    return;
	}
	victim->perm_mana = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "mmana" ) )
    {
	victim->mod_mana = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "blood" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char(AT_WHITE, "Blood range is 0 to 30,000 mana points.\n\r", ch );
	    return;
	}
	victim->perm_bp = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "mblood" ) )
    {
	victim->mod_bp = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "move" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char(AT_WHITE, "Move range is 0 to 30,000 move points.\n\r", ch );
	    return;
	}
	victim->perm_move = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "mmove" ) )
    {
	victim->mod_move = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "practice" ) )
    {
	if ( value < 0 || value > 100 )
	{
	    send_to_char(AT_WHITE, "Practice range is 0 to 100 sessions.\n\r", ch );
	    return;
	}
	victim->practice = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "align" ) )
    {
	if ( value < -1000 || value > 1000 )
	{
	    send_to_char(AT_WHITE, "Alignment range is -1000 to 1000.\n\r", ch );
	    return;
	}
	victim->alignment = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "thirst" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( ( value < 0 || value > 100 )
	    && get_trust( victim ) < LEVEL_IMMORTAL )
	{
	    send_to_char(AT_WHITE, "Thirst range is 0 to 100.\n\r", ch );
	    return;
	}
	else
	    if ( value < -1 || value > 100 )
	    {
		send_to_char(AT_WHITE, "Thirst range is -1 to 100.\n\r", ch );
		return;
	    }

	victim->pcdata->condition[COND_THIRST] = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "drunk" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > 100 )
	{
	    send_to_char(AT_WHITE, "Drunk range is 0 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_DRUNK] = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "full" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( ( value < 0 || value > 100 )
	    && get_trust( victim ) < LEVEL_IMMORTAL )
	{
	    send_to_char(AT_WHITE, "Full range is 0 to 100.\n\r", ch );
	    return;
	}
	else
	    if ( value < -1 || value > 100 )
	    {
		send_to_char(AT_WHITE, "Full range is -1 to 100.\n\r", ch );
		return;
	    }

	victim->pcdata->condition[COND_FULL] = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on PC's.\n\r", ch );
	    return;
	}

	if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->name );
	victim->name = str_dup( arg3 );
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
        if ( !IS_NPC(victim) )
	{
	  send_to_char(AT_WHITE, "Not on PC's.\n\r",ch);
	  return;
	}
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->short_descr );
	victim->short_descr = str_dup( arg3 );
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
        if ( !IS_NPC(victim) )
	{
	  send_to_char(AT_WHITE, "Not on PC's.\n\r",ch);
	  return;
	}
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( victim->long_descr );
	strcat( arg3, "\n\r" );
	victim->long_descr = str_dup( arg3 );
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "title" ) )
    {
	if ( IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	    return;
	}

	set_title( victim, arg3 );
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "spec" ) )
    {
	if ( !IS_NPC( victim ) )
	{
	    send_to_char(AT_WHITE, "Not on PC's.\n\r", ch );
	    return;
	}

	if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
	{
	    send_to_char(AT_WHITE, "No such spec fun.\n\r", ch );
	    return;
	}
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "security" ) )	/* OLC */
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
            return;
        }

	if ( value > ch->pcdata->security || value < 0 )
	{
	    if ( ch->pcdata->security != 0 )
	    {
		sprintf( buf, "Valid security is 0-%d.\n\r",
		    ch->pcdata->security );
		send_to_char(AT_WHITE, buf, ch );
	    }
	    else
	    {
		send_to_char(AT_WHITE, "Valid security is 0 only.\n\r", ch );
	    }
          return;
	}
	victim->pcdata->security = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }
/* XOR */
    if(!str_cmp(arg2, "immune"))
    {
      if(!IS_NPC(victim))
      {
        send_to_char(AT_WHITE, "Not on PC's.\n\r", ch );
        return;
      }
      victim->imm_flags ^= value;
      send_to_char(AT_WHITE, "Ok.\n\r", ch );
      return;
    }
/* END */

    /*
     * Generate usage message.
     */
    do_mset( ch, "" );
    return;
}



void do_oset( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    char       arg3 [ MAX_INPUT_LENGTH ];
    char       arg4 [ MAX_INPUT_LENGTH ];
    int        value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( ( !str_cmp( arg2, "apply" ) ) || ( !str_cmp( arg2, "cost" ) ) )
    {
      argument = one_argument( argument, arg3 );
      argument = one_argument( argument, arg4 );
    }
    else
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char(AT_WHITE, "&pSyntax: oset <object> <field>  <value>\n\r",	ch );
	send_to_char(AT_WHITE, "&por:     oset <object> <string> <value>\n\r",	ch );
	send_to_char(AT_WHITE, "\n\r",						ch );
	send_to_char(AT_WHITE, "&pField being one of:\n\r",			ch );
	send_to_char(AT_WHITE, "  value0 value1 value2 value3\n\r",		ch );
	send_to_char(AT_WHITE, "  anticlass antirace\n\r",			ch );
	send_to_char(AT_WHITE, "  extra wear level weight cost timer\n\r",	ch );
        send_to_char(AT_WHITE, "  ac_type ac_vnum ac_v1 ac_v2\n\r", ch );
	send_to_char(AT_WHITE, "  apply delapply\n\r", ch );
	send_to_char(AT_WHITE, "\n\r",						ch );
	send_to_char(AT_WHITE, "&pString being one of:\n\r",			ch );
	send_to_char(AT_WHITE, "  name short long ed ac_setspell\n\r",			ch );
	return;
    }

    if ( !( obj = get_obj_here( ch, arg1 ) ) )
    {
	send_to_char(AT_WHITE, 
	"Nothing like that in your inventory or the room.\n\r", ch );
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;     
    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
    {
	obj->value[0] = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
    {
	obj->value[1] = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
    {
	obj->value[2] = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
    {
	obj->value[3] = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "delapply" ) )
    {
      AFFECT_DATA *paf;
      AFFECT_DATA *paf_next;
      int cnt = 0;

      if ( value == -1 )
      {
        send_to_char( AT_WHITE, "&pSyntax: oset object delapply [number]\n\r", ch );
        return;
      }
      
      if ( !( paf = obj->affected ) )
      {
          send_to_char( AT_WHITE, "Non-existant apply.\n\r", ch );
          return;
      }

      
      if ( value == 0 )
      {
	paf = obj->affected;
        obj->affected = paf->next;
        free_affect( paf );
      }
      else
      {
        while ( ( paf_next = paf->next ) && ( ++cnt < value ) )
            paf = paf_next;
            
        if ( paf_next )
        {
           paf->next = paf_next->next;
           free_affect( paf_next );
        }
	else
        {
	   send_to_char( AT_WHITE, "No such affect.\n\r", ch );
           return;
        }
      }      
      return;
    }

    if ( !str_cmp( arg2, "apply" ) )
    {
        AFFECT_DATA *paf;
        
        if ( is_number( arg3 ) || arg4[0] == '\0' || !is_number( arg4 ) )
        {
          send_to_char( AT_WHITE, "&pSyntax: oset object apply type modifier\n\r", ch );
          return;
        }

	paf = new_affect();
	paf->location = flag_value( apply_flags, arg3 );
	paf->modifier = atoi( arg4 );
	paf->type     = skill_lookup(arg3);
	if ( paf->type < 0 )
	  paf->type = 0;
	paf->duration = -1;
	paf->bitvector = 0;
	paf->next = obj->affected;
	obj->affected = paf;        
	
	send_to_char( AT_WHITE, "Apply added.\n\r", ch );
	return;
    }
        
        

    if ( !str_cmp( arg2, "extra" ) )
    {
	if (!str_cmp( arg3, "glow" ) || !str_cmp( arg3, "g" ) )
          value = ITEM_GLOW;
        if (!str_cmp( arg3, "hum" ) || !str_cmp( arg3, "h" ) )
          value = ITEM_HUM;
	if (!str_cmp( arg3, "dark" ) || !str_cmp( arg3, "d" ) )
          value = ITEM_DARK;
        if (!strcmp( arg3, "lock" ) || !str_cmp( arg3, "l" ) )
          value = ITEM_LOCK;
	if (!str_cmp( arg3, "evil" ) || !str_cmp( arg3, "e" ) )
          value = ITEM_EVIL;
        if (!str_cmp( arg3, "invis" ) || !str_cmp( arg3, "i" ) )
          value = ITEM_INVIS;
        if (!str_cmp( arg3, "magic" ) || !str_cmp( arg3, "m" ) )
          value = ITEM_MAGIC;
        if (!str_cmp( arg3, "nodrop" ) || !str_cmp( arg3, "nd" ) )
          value = ITEM_NODROP;
        if (!str_cmp( arg3, "antigood" ) || !str_cmp( arg3, "ag" ) )
          value = ITEM_ANTI_GOOD;
        if (!str_cmp( arg3, "antievil" ) || !str_cmp( arg3, "ae" ) )
	  value = ITEM_ANTI_EVIL;
        if (!str_cmp( arg3, "antineutral" ) || !str_cmp( arg3, "an" ) )
          value = ITEM_ANTI_NEUTRAL;
        if (!str_cmp( arg3, "noremove" ) || !str_cmp( arg3, "nr" ) )
          value = ITEM_NOREMOVE;
	if (!str_cmp( arg3, "poisoned" ) || !str_cmp( arg3, "p" ) )
          value = ITEM_POISONED;
        if (!str_cmp( arg3, "flame" ) || !str_cmp( arg3, "f" ) )
	  value = ITEM_FLAME;
        if (!str_cmp( arg3, "chaos" ) || !str_cmp( arg3, "c" ) )
          value = ITEM_CHAOS;
        if (!str_cmp( arg3, "nolocate" ) || !str_cmp( arg3, "nl" ) )
          value = ITEM_NO_LOCATE;
        if (!str_cmp( arg3, "nodamage" ) || !str_cmp( arg3, "n" ) )
          value = ITEM_NO_DAMAGE;
        if (!str_cmp( arg3, "patched" ) || !str_cmp( arg3, "p" ) )
          value = ITEM_PATCHED;
        if (!str_cmp( arg3, "frosty" ) || !str_cmp( arg3, "f" ) )
          value = ITEM_ICY;

	if ( value == 0 || !str_cmp( arg3, "none" ) )
	{
	   obj->extra_flags = 0;
	   send_to_char(AT_WHITE,"Ok.\n\r", ch );
	   return;
	}
/*	if ( 4294967295 & value ) */
	if ( 0xFFFFFFFF & value )
	{
	  if (value == 0)
	    obj->extra_flags = 0;
	   else
	    obj->extra_flags ^= value;
	  send_to_char(AT_WHITE, "Ok.\n\r", ch );
	  return;
	}
	else
	{
	  send_to_char(AT_WHITE, "Invalid bit.\n\r", ch );
	  return;
	}
    }
/* FOR NEW FLAGS */
    if ( !str_cmp( arg2, "anticlass" ) )
    {
	if (!str_cmp( arg3, "mage" ) || !str_cmp( arg3, "mag" ) )
	  value = ITEM_ANTI_MAGE;
        if (!str_cmp( arg3, "cleric" ) || !str_cmp( arg3, "cle" ) )
          value = ITEM_ANTI_CLERIC;
	if (!str_cmp( arg3, "thief" ) || !str_cmp( arg3, "thi" ) )
	  value = ITEM_ANTI_THIEF;
	if (!str_cmp( arg3, "warrior" ) || !str_cmp( arg3, "war" ) )
	  value = ITEM_ANTI_WARRIOR;
	if (!str_cmp( arg3, "psionisist" ) || !str_cmp( arg3, "psi" ) )
	  value = ITEM_ANTI_PSI;
	if (!str_cmp( arg3, "druid" ) || !str_cmp( arg3, "dru" ) )
	  value = ITEM_ANTI_DRUID;
	if (!str_cmp( arg3, "ranger" ) || !str_cmp( arg3, "ran" ) )
	  value = ITEM_ANTI_RANGER;
	if (!str_cmp( arg3, "paladin" ) || !str_cmp( arg3, "pal" ) )
	  value = ITEM_ANTI_PALADIN;
	if (!str_cmp( arg3, "bard" ) || !str_cmp( arg3, "brd" ) )
	  value = ITEM_ANTI_BARD;
	if (!str_cmp( arg3, "vampire" ) || !str_cmp( arg3, "vam" ) )
	  value = ITEM_ANTI_VAMP;
	if (!str_cmp( arg3, "necromancer" ) || !str_cmp( arg3, "nec" ) )
	  value = ITEM_ANTI_NECRO;
	if (!str_cmp( arg3, "werewolf" ) || !str_cmp( arg3, "wwf" ) )
	  value = ITEM_ANTI_WWF;
	if (!str_cmp( arg3, "monk" ) || !str_cmp( arg3, "mnk" ) )
	  value = ITEM_ANTI_MONK;
	if ( value == 0 || !str_cmp( arg3, "none" ) )
	{
	   obj->anti_class_flags = 0;
	   send_to_char(AT_WHITE,"Ok.\n\r", ch );
	   return;
	}
	if ( 0xFFFFFFFF & value )
	{
	  if (value == 0)
	    obj->anti_class_flags = 0;
	   else
	    obj->anti_class_flags ^= value;
	  send_to_char(AT_WHITE, "Ok.\n\r", ch );
	  return;
	}
	else
	{
	  send_to_char(AT_WHITE, "Invalid bit.\n\r", ch );
	  return;
	}
    }
    if ( !str_cmp( arg2, "antirace" ) )
    {
	if (!str_cmp( arg3, "human" ) || !str_cmp( arg3, "hum" ) )
	  value = ITEM_ANTI_HUMAN;
	if (!str_cmp( arg3, "elf" ) )
	  value = ITEM_ANTI_ELF;
	if (!str_cmp( arg3, "dwarf" ) || !str_cmp( arg3, "dwa" ) )
	  value = ITEM_ANTI_DWARF;
	if (!str_cmp( arg3, "pixie" ) || !str_cmp( arg3, "pix" ) )
	  value = ITEM_ANTI_PIXIE;
	if (!str_cmp( arg3, "halfling" ) || !str_cmp( arg3, "hlf" ) )
	  value = ITEM_ANTI_HALFLING;
	if (!str_cmp( arg3, "drow" ) || !str_cmp( arg3, "drw" ) )
	  value = ITEM_ANTI_DROW;
	if (!str_cmp( arg3, "elder" ) || !str_cmp( arg3, "eld" ) )
	  value = ITEM_ANTI_ELDER;
	if (!str_cmp( arg3, "ogre" ) || !str_cmp( arg3, "ogr" ) )
	  value = ITEM_ANTI_OGRE;
	if (!str_cmp( arg3, "lizardman" ) || !str_cmp( arg3, "liz" ) )
	  value = ITEM_ANTI_LIZARDMAN;
	if (!str_cmp( arg3, "demon" ) || !str_cmp( arg3, "dem" ) )
	  value = ITEM_ANTI_DEMON;
	if (!str_cmp( arg3, "ghoul" ) || !str_cmp( arg3, "ghl" ) )
	  value = ITEM_ANTI_GHOUL;
	if (!str_cmp( arg3, "illithid" ) || !str_cmp( arg3, "ill" ) )
	  value = ITEM_ANTI_ILLITHID;
	if (!str_cmp( arg3, "minotaur" ) || !str_cmp( arg3, "min" ) )
	  value = ITEM_ANTI_MINOTAUR;
	if (!str_cmp( arg3, "troll" ) || !str_cmp( arg3, "tro" ) )
	  value = ITEM_ANTI_TROLL;
	if (!str_cmp( arg3, "shadow" ) || !str_cmp( arg3, "shd" ) )
	  value = ITEM_ANTI_SHADOW;
	if (!str_cmp( arg3, "tabaxi" ) || !str_cmp( arg3, "tbx" ) )
	  value = ITEM_ANTI_TABAXI;

	if ( value == 0 || !str_cmp( arg3, "none" ) )
	{
	   obj->anti_race_flags = 0;
	   send_to_char(AT_WHITE,"Ok.\n\r", ch );
	   return;
	}
	if ( 0xFFFFFFFF & value )
	{
	  if (value == 0)
	    obj->anti_race_flags = 0;
	   else
	    obj->anti_race_flags ^= value;
	  send_to_char(AT_WHITE, "Ok.\n\r", ch );
	  return;
	}
	else
	{
	  send_to_char(AT_WHITE, "Invalid bit.\n\r", ch );
	  return;
	}
    }

    if ( !str_cmp( arg2, "wear" ) )
    {
      if (!str_cmp( arg3, "take" ) )
       value = ITEM_TAKE;
      if (!str_cmp( arg3, "finger" ) )
       value = ITEM_WEAR_FINGER;
      if (!str_cmp( arg3, "neck" ) )
       value = ITEM_WEAR_NECK;
      if (!str_cmp( arg3, "body" ) )
       value = ITEM_WEAR_BODY;
      if (!str_cmp( arg3, "head" ) )
       value = ITEM_WEAR_HEAD;
      if (!str_cmp( arg3, "legs" ) )
       value = ITEM_WEAR_LEGS;
      if (!str_cmp( arg3, "feet" ) )
       value = ITEM_WEAR_FEET;
      if (!str_cmp( arg3, "hands" ) )
       value = ITEM_WEAR_HANDS;
      if (!str_cmp( arg3, "arms" ) )
       value = ITEM_WEAR_ARMS;
      if (!str_cmp( arg3, "shield" ) )
       value = ITEM_WEAR_SHIELD;
      if (!str_cmp( arg3, "about" ) )
       value = ITEM_WEAR_ABOUT;
      if (!str_cmp( arg3, "waist" ) )
       value = ITEM_WEAR_WAIST;
      if (!str_cmp( arg3, "wrist" ) )
       value = ITEM_WEAR_WRIST;
      if (!str_cmp( arg3, "wield" ) )
       value = ITEM_WIELD;
      if (!str_cmp( arg3, "hold" ) )
       value = ITEM_HOLD;
      if (!str_cmp( arg3, "orbit" ) )
       value = ITEM_WEAR_ORBIT;
      if (!str_cmp( arg3, "face" ) )
       value = ITEM_WEAR_FACE;
      if (!str_cmp( arg3, "contact" ) )
       value = ITEM_WEAR_CONTACT;
      if (!str_cmp( arg3, "prototype" ) )
       value = ITEM_PROTOTYPE;
      if (!str_cmp( arg3, "ears" ) )
       value = ITEM_WEAR_EARS;
      if (!str_cmp( arg3, "ankle" ) )
       value = ITEM_WEAR_ANKLE;

      if ( value == 0 || !str_cmp( arg3, "none" ) )
       {
         obj->wear_flags = 0;
         send_to_char(AT_WHITE,"Ok.\n\r", ch);
         return;
       }
 
      obj->wear_flags ^= value;
      send_to_char(AT_WHITE, "Ok.\n\r", ch );
      return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
	obj->level = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }
	
    if ( !str_cmp( arg2, "weight" ) )
    {
	obj->weight = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "cost" ) )
    {
#ifdef NEW_MONEY

	if ( is_number( arg4 ) )
	{
	  if ( !str_cmp( arg3, "gold" ) )
	    obj->cost.gold = atoi( arg4 );
	  else if ( !str_cmp( arg3, "silver" ) )
	    obj->cost.silver = atoi( arg4 );
	  else if ( !str_cmp( arg3, "copper" ) )
	    obj->cost.copper = atoi( arg4 );
	  else
	  {
	    send_to_char( AT_WHITE, "Invaild currency type.\n\r", ch );
	    send_to_char( AT_WHITE, "&WSyntax: oset &Robject <cost> <currency_type> <amount>\n\r",
	        	ch );
	    return;
	  }
	}
	else
	{
	    send_to_char( AT_WHITE, "Invalid amount.\n\r", ch );
	    send_to_char( AT_WHITE, "&WSyntax: oset &Robject <cost> <currency_type> <amount>\n\r",
	 		ch );
	    return;
	}
#else
	obj->cost = value;
#endif
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "timer" ) )
    {
	obj->timer = value;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }
	
    if ( !str_cmp( arg2, "name" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->name );
	obj->name = str_dup( arg3 );
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->short_descr );
	obj->short_descr = str_dup( arg3 );
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
        if ( longstring( ch, arg3 ) )
	    return;

	free_string( obj->description );
	obj->description = str_dup( arg3 );
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "ed" ) )
    {
	EXTRA_DESCR_DATA *ed;

	argument = one_argument( argument, arg3 );
	argument = one_argument( argument, arg4 );
	if ( arg4[0] == '\0' )
	{
	    send_to_char(AT_WHITE, "Syntax: oset <object> ed <keyword> <string>\n\r",
		ch );
	    return;
	}

	if ( !extra_descr_free )
	{
	    ed			= alloc_perm( sizeof( *ed ) );
	}
	else
	{
	    ed			= extra_descr_free;
	    extra_descr_free	= extra_descr_free->next;
	}

	ed->keyword		= str_dup( arg3     );
	ed->description		= str_dup( argument );
	ed->deleted             = FALSE;
	ed->next		= obj->extra_descr;
	obj->extra_descr	= ed;
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "ac_type" ) )
    {
	if ( arg3 == '\0' || !is_number( arg3 ) )
	{
	    send_to_char(AT_WHITE, "Syntax: oset ac_type [type #]\n\r", ch );
	    return;
	}

	obj->ac_type = value;

	send_to_char(AT_WHITE, "Invoke type set.\n\r", ch);
	return ;
    }

    if ( !str_cmp( arg2, "ac_vnum" ) )
    {
	if ( arg3 == '\0' || !is_number( arg3 ) )
	{
	    send_to_char(AT_WHITE, "Syntax: oset ac_vnum [ # ]\n\r", ch );
	    return;
	}
	obj->ac_vnum = value;
	send_to_char(AT_WHITE, "Invoke vnum set.\n\r", ch);
	return;
    }

    if ( !str_cmp( arg2, "ac_v1" ) )
    {
       if ( arg3 == '\0' || !is_number( arg3 ) )
       {
	   send_to_char(AT_WHITE, "Syntax: oset ac_v1 [ current charges ]\n\r", ch );
	   return ;
       }

      obj->ac_charge[0] = value;

      send_to_char(AT_WHITE, "Current charge set.\n\r", ch);
      return ;
    }

    if ( !str_cmp( arg2, "ac_v2" ) )
    {
	if ( arg3 == '\0' || !is_number( arg3 ) )
	{
	    send_to_char(AT_WHITE, "Syntax: oset ac_v2 [ max charges (-1 unlimited) ]\n\r", ch );
	    return ;
	}

	obj->ac_charge[1] = value;

	send_to_char(AT_WHITE, "Max Charge set.\n\r", ch);
	return ;
    }


    if ( !str_cmp( arg2, "ac_setspell" ) )
    {
	int            spn;

	spn = skill_lookup( arg3 );

	if ( ( arg3 == '\0' ) || ( spn == -1 ) )
	{
	    send_to_char(AT_WHITE, "Syntax: oset ac_setspell [ valid spell name ]\n\r", ch );
	    return ;
	}

        free_string( obj->ac_spell );
	obj->ac_spell = str_dup(skill_table[spn].name);

	send_to_char(AT_WHITE, "Spell set.\n\r", ch);
	return ;
    }


    /*
     * Generate usage message.
     */
    do_oset( ch, "" );
    return;
}



void do_rset( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    char             arg1 [ MAX_INPUT_LENGTH ];
    char             arg2 [ MAX_INPUT_LENGTH ];
    char             arg3 [ MAX_INPUT_LENGTH ];
    int              value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Syntax: rset <location> <field> value\n\r",	ch );
	send_to_char(AT_WHITE, "\n\r",						ch );
	send_to_char(AT_WHITE, "Field being one of:\n\r",			ch );
	send_to_char(AT_WHITE, "  flags sector\n\r",				ch );
	return;
    }

    if ( !( location = find_location( ch, arg1 ) ) )
    {
	send_to_char(AT_WHITE, "No such location.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char(AT_WHITE, "Value must be numeric.\n\r", ch );
	return;
    }
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "flags" ) )
    {
	location->room_flags	= value;
	return;
    }

    if ( !str_cmp( arg2, "sector" ) )
    {
	location->sector_type	= value;
	return;
    }

    /*
     * Generate usage message.
     */
    do_rset( ch, "" );
    return;
}


char *con_type_name( int connected )
{
  if ( connected == CON_PLAYING ) return "Playing";
  if ( connected == CON_GET_NAME ) return "Get Name";
  if ( connected == CON_GET_OLD_PASSWORD ) return "Old PW";
  if ( connected == CON_CONFIRM_NEW_NAME ) return "Name Conf";
  if ( connected == CON_GET_NEW_PASSWORD ) return "New PW";
  if ( connected == CON_CONFIRM_NEW_PASSWORD ) return "PW Conf";
  if ( connected == CON_GET_NEW_SEX ) return "New Sex";
  if ( connected == CON_GET_NEW_CLASS ) return "New Class";
  if ( connected == CON_GET_2ND_CLASS ) return "2nd Class";
  if ( connected == CON_CONFIRM_2ND_CLASS ) return "2nd Class Conf";
  if ( connected == CON_GET_3RD_CLASS ) return "3rd Class";
  if ( connected == CON_CONFIRM_3RD_CLASS ) return "3rd Class Conf";
  if ( connected == CON_WANT_MULTI ) return "Multi Conf";
  if ( connected == CON_WANT_MULTI_2 ) return "Multi 2 Conf";
  if ( connected == CON_READ_MOTD ) return "Read MOTD";
  if ( connected == CON_GET_NEW_RACE ) return "New Race";
  if ( connected == CON_CONFIRM_RACE ) return "Race Conf";
  if ( connected == CON_CONFIRM_CLASS ) return "Class Conf";
  if ( connected == CON_GET_ANSI ) return "Get ANSI";
  if ( connected == CON_AUTHORIZE_NAME ) return "Authorize";
  if ( connected == CON_AUTHORIZE_NAME1 ) return "Auth One";
  if ( connected == CON_AUTHORIZE_NAME2 ) return "Auth Two";
  if ( connected == CON_AUTHORIZE_NAME3 ) return "Auth Three";
  if ( connected == CON_AUTHORIZE_LOGOUT ) return "Auth Logout";
  if ( connected == CON_CHATTING ) return "Chatting";
  if ( connected == CON_CHECK_AUTHORIZE ) return "Checking Authorize System";
  return "Unknown";
}

void do_users( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char            buf  [ MAX_STRING_LENGTH ];
    int             count;
    char	    arg [ MAX_INPUT_LENGTH ];
    
    argument = one_argument( argument, arg );

    count	= 0;
    buf[0]	= '\0';
    strcpy( buf, "[Desc Connected  ] Name&W!&BUser&W@&GHost&R\n\r" );

    if ( arg[0] == '\0' )
    {
    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->character && can_see( ch, d->character ) )
	{
	    count++;
	    sprintf( buf + strlen(buf), "[%4d %-11s] %s&W!&B%s&W@&G%s&R\n\r",
		    d->descriptor,
		    con_type_name(d->connected),
		    d->original  ? d->original->name  :
		    d->character ? d->character->name : "(none)",
		    d->user ? d->user : "(none)",
		    d->host );
	}
    }
    }
    else   /* To be able to do "user <name>" -Angi */
    {
     for ( d = descriptor_list; d; d = d->next )
     {
      if ( d->character && can_see( ch, d->character ) )
      {
        if ( !str_cmp( arg, d->original ? d->original->name : d->character
             ? d->character->name : "(none)" ) )
        {
            count++;
            sprintf( buf + strlen(buf), "[%4d %-11s] %s&W!&B%s&W@&G%s&R\n\r",
                    d->descriptor,
                    con_type_name(d->connected),
                    d->original  ? d->original->name  :
                    d->character ? d->character->name : "(none)",
                    d->user ? d->user : "(none)",
                    d->host ); 
        }
      }
     }
    }
    sprintf( buf+strlen(buf), "%d user%s\n\r", count, count == 1 ? "" : "s" );
    send_to_char(AT_RED, buf, ch );
    return;
}



/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
    char       arg [ MAX_INPUT_LENGTH ];
    char       arg1[ MAX_INPUT_LENGTH ];
    int        trust;
    int        cmd;
    int        llvlr = 0;
    int        hlvlr = L_IMP;

    argument = one_argument( argument, arg );
    if ( is_number( arg ) )
    {
      llvlr = atoi( arg );
      argument = one_argument( argument, arg1 );
      if ( is_number( arg1 ) )
        hlvlr = atoi( arg1 );
    }

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Force whom to do what?\n\r", ch );
	return;
    }

    /*
     * Look for command in command table.
     */
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( argument[0] == cmd_table[cmd].name[0]
	    && !str_prefix( argument, cmd_table[cmd].name )
	    && ( cmd_table[cmd].level > trust ) )
	{
	  send_to_char(AT_WHITE, "You can't even do that yourself!\n\r", ch );
	  return;
	}
    }

    if ( !str_cmp( arg, "all" ) || is_number( arg ) )
    {
	CHAR_DATA *vch;

	for ( vch = char_list; vch; vch = vch->next )
	{
	    if ( vch->deleted )
	        continue;

	    if ( !IS_NPC( vch ) && get_trust( vch ) < get_trust( ch ) 
	      && ( vch->level >= llvlr && vch->level <= hlvlr ) )
	    {
	      if ( !is_number( arg ) || ( is_number( arg ) && is_number( arg1 ) ) )
	      {
		act(AT_RED, "$n forces you to '$t'.", ch, argument, vch, TO_VICT );
		interpret( vch, argument );
		continue;
              }
              if ( !is_number( arg1 ) )
              {
                sprintf( log_buf, "%s %s", arg1, argument );
                act( AT_RED, "$n forces you to '$t'.", ch, log_buf, vch, TO_VICT );
                interpret( vch, log_buf );
              }
              
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( !( victim = get_char_world( ch, arg ) ) )
	{
	    send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char(AT_WHITE, "Aye aye, right away!\n\r", ch );
	    return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
	    send_to_char(AT_WHITE, "Do it yourself!\n\r", ch );
	    return;
	}

	act(AT_RED, "$n forces you to '$t'.", ch, argument, victim, TO_VICT );
	interpret( victim, argument );
    }

    send_to_char(AT_WHITE, "Ok.\n\r", ch );
    return;
}

void do_cloak( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int level;

    if ( IS_NPC(ch) )
        return;

    argument = one_argument( argument, arg );
    if ( arg && arg[0] != '\0' )
    {
        if ( !is_number( arg ) )
        {
           send_to_char( AT_WHITE, "Usage: cloak | cloak <level>\n\r", ch );
           return;
        }
        level = atoi( arg );
        if ( level < 2 || level > get_trust( ch ) )

        {
            send_to_char( AT_WHITE, "Invalid level.\n\r", ch );
            return;
        }
        ch->cloaked = level;
        sprintf( arg, "Cloak level set to %d.\n\r", level );
        send_to_char( AT_WHITE, arg, ch );
        return;
    }

    if ( ch->cloaked < 2 )
      ch->cloaked = ch->level;

    if ( IS_SET(ch->act, PLR_CLOAKED) )
    {
        REMOVE_BIT(ch->act, PLR_CLOAKED);
    act( AT_YELLOW, "$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM );
        send_to_char( AT_WHITE, "You are no longer cloaked.\n\r", ch );
    }
    else
    {
        SET_BIT(ch->act, PLR_CLOAKED);
    act( AT_YELLOW, "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
        send_to_char( AT_WHITE, "You cloak your presence.\n\r", ch );
    }
    return;

}


void do_invis( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int level; 

    if ( IS_NPC(ch) )
        return; 

    argument = one_argument( argument, arg ); 
    if ( arg && arg[0] != '\0' )
    {
        if ( !is_number( arg ) )
        {
           send_to_char( AT_WHITE, "Usage: invis | invis <level>\n\r", ch ); 
           return; 
        }
        level = atoi( arg ); 
        if ( level < 2 || level > get_trust( ch ) )
        {
            send_to_char( AT_WHITE, "Invalid level.\n\r", ch ); 
            return; 
        }
        ch->wizinvis = level; 
        sprintf( arg, "Wizinvis level set to %d.\n\r", level ); 
        send_to_char( AT_WHITE, arg, ch ); 
        return; 
    }

    if ( ch->wizinvis < 2 )
      ch->wizinvis = ch->level; 

    if ( IS_SET(ch->act, PLR_WIZINVIS) )
    {
        REMOVE_BIT(ch->act, PLR_WIZINVIS); 
    act( AT_YELLOW, "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM ); 
        send_to_char( AT_WHITE, "You slowly fade back into existence.\n\r", ch ); 
    }
    else
    {
        SET_BIT(ch->act, PLR_WIZINVIS); 
    act( AT_YELLOW, "$n slowly fades into thin air.", ch, NULL, NULL,
TO_ROOM ); 
        send_to_char( AT_WHITE, "You slowly vanish into thin air.\n\r", 
ch ); 
    }
    return; 
 
 
}



void do_holylight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
	return;

    if ( IS_SET( ch->act, PLR_HOLYLIGHT ) )
    {
	REMOVE_BIT( ch->act, PLR_HOLYLIGHT );
	send_to_char(AT_WHITE, "Holy light mode off.\n\r", ch );
    }
    else
    {
	SET_BIT(    ch->act, PLR_HOLYLIGHT );
	send_to_char(AT_WHITE, "Holy light mode on.\n\r", ch );
    }

    return;
}

/* Wizify and Wizbit sent in by M. B. King */

void do_wizify( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
  
    argument = one_argument( argument, arg1  );
    if ( arg1[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Syntax: wizify <name>\n\r" , ch );
	return;
    }
    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
	send_to_char(AT_WHITE, "They aren't here.\n\r" , ch );
	return;
    }
    if ( IS_NPC( victim ) )
    {
	send_to_char(AT_WHITE, "Not on mobs.\n\r", ch );
	return;
    }
    victim->wizbit = !victim->wizbit;
    if ( victim->wizbit ) 
    {
	act(AT_RED, "$N wizified.",         ch, NULL, victim, TO_CHAR );
	act(AT_RED, "$n has wizified you!", ch, NULL, victim, TO_VICT );
    }
    else
    {
	act(AT_RED, "$N dewizzed.",         ch, NULL, victim, TO_CHAR );
	act(AT_RED, "$n has dewizzed you!", ch, NULL, victim, TO_VICT ); 
    }

    do_save( victim, "");
    return;
}

/* Idea from Talen of Vego's do_where command */

void do_owhere( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    OBJ_DATA  *in_obj;
    char       buf  [ MAX_STRING_LENGTH   ];
    char       arg  [ MAX_INPUT_LENGTH    ];
    int        obj_counter = 1;
    bool       found = FALSE;

    one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	send_to_char(AT_WHITE, "Syntax:  owhere <object>.\n\r", ch );
	return;
    }
    else
    {
	for ( obj = object_list; obj; obj = obj->next )
	{
	    if ( !can_see_obj( ch, obj ) || !is_name( ch, arg, obj->name ) )
	        continue;

	    found = TRUE;

	    for ( in_obj = obj; in_obj->in_obj;
		 in_obj = in_obj->in_obj )
	        ;

	    if ( in_obj->carried_by )
	    {
	        if ( !can_see( ch, in_obj->carried_by ) )
		    continue;
		sprintf( buf, "[%2d] %s carried by %s at [%4d].\n\r",
			obj_counter, obj->short_descr,
			PERS( in_obj->carried_by, ch ),
			in_obj->carried_by->in_room->vnum );
	    }
	    else if ( in_obj->stored_by )
	    {
	      sprintf( buf, "[%2d] %s in %s's storage box.\n\r",
		      obj_counter, obj->short_descr,
		      PERS( in_obj->stored_by, ch ) );
	    }
	    else
	    {
		sprintf( buf, "[%2d] %s in %s at [%4d].\n\r", obj_counter,
			obj->short_descr, ( !in_obj->in_room ) ?
			"somewhere" : in_obj->in_room->name,
			( !in_obj->in_room ) ?
			0 : in_obj->in_room->vnum );
	    }
	    
	    obj_counter++;
	    buf[0] = UPPER( buf[0] );
	    send_to_char(AT_GREEN, buf, ch);
	}

    }

    if ( !found )
	send_to_char(AT_WHITE,
		"Nothing like that in hell, earth, or heaven.\n\r" , ch );

    return;


}

void do_numlock( CHAR_DATA *ch, char *argument )  /*By Globi*/
{
	   char       buf  [ MAX_STRING_LENGTH ];
	   char       arg1 [ MAX_INPUT_LENGTH  ];
    extern int        numlock;
           int        temp;

    argument = one_argument( argument, arg1 );
    temp = atoi( arg1 );

    if ( arg1[0] == '\0' ) /* Prints out the current value */
    {
	sprintf( buf, "Current numlock setting is:  %d.\n\r", numlock );
	send_to_char(AT_RED, buf, ch );
	return;
    }

    if ( ( temp < 0 ) || ( temp > LEVEL_HERO ) )
    {
	sprintf( buf, "Level must be between 0 and %d.\n\r", LEVEL_HERO );
	send_to_char(AT_WHITE, buf, ch );
	return;
    }

    numlock = temp;  /* Only set numlock if arg supplied and within range */

    if ( numlock != 0 )
    {
	sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
	send_to_char(AT_RED, buf, ch );
    }
    else
        send_to_char(AT_RED, "Game now open to all levels.\n\r", ch );

    return;

}

void do_newlock( CHAR_DATA *ch, char *argument )
{
    extern int        numlock;
           char       buf [ MAX_STRING_LENGTH ];

    if ( numlock != 0 && get_trust( ch ) < L_SEN )
    {
	send_to_char(AT_WHITE, "You may not change the current numlock setting\n\r",
		     ch );
	sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
	send_to_char(AT_RED, buf, ch );
	return;
    }

    if ( numlock != 0 )
    {
	sprintf( buf, "Game numlocked to levels %d and below.\n\r", numlock );
	send_to_char(AT_RED, buf, ch );
	send_to_char(AT_RED, "Changing to: ", ch );
    }

    numlock = 1;
    send_to_char(AT_RED, "Game locked to new characters.\n\r", ch );
    wiznet( "$N locks out new characters.", ch, NULL, 0, 0, 0 );
    return;

}

bool sreset = TRUE; /*Is it changable anymore?*/
void do_sstime( CHAR_DATA *ch, char *argument )
{
    extern char      *down_time;
    extern char      *warning1;
    extern char      *warning2;
    extern int        stype;
           char       buf  [ MAX_STRING_LENGTH ];
           char       arg1 [ MAX_INPUT_LENGTH  ];
           char       arg2 [ MAX_INPUT_LENGTH  ];

    if ( !sreset )
    {
      sprintf(buf,"%s locked!\n\r\n\r", (stype == 0 ? "Reboot" : "Shutdown"));
      send_to_char(AT_WHITE+AT_BLINK, buf, ch);
      sprintf(buf,
	      "1st warning:  %s\n\r2nd warning:  %s\n\r   Downtime:  %s\n\r",
	      warning1, warning2, down_time );
      send_to_char(AT_RED, buf, ch);
      sprintf(buf, "Sstime type: %s.\n\r", (stype == 0 ? "reboot" : "shutdown"));
      send_to_char(AT_RED, buf, ch);
      return;
    }

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy ( arg2, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' ||
       ( strlen( arg2 ) != 8 && str_cmp(arg2, "*") &&
	str_cmp(arg1, "type") ) )
	
      {
	send_to_char(AT_WHITE, "Syntax: sstime <field> <value>\n\r",               ch );
	send_to_char(AT_WHITE, "\n\r",                                             ch );
	send_to_char(AT_WHITE, "Field being one of:\n\r",                          ch );
	send_to_char(AT_WHITE, "  downtime  1warning  2warning  type\n\r",         ch );
	send_to_char(AT_WHITE, "\n\r",                                             ch );
	send_to_char(AT_WHITE, "Value being format of:\n\r",                       ch );
	send_to_char(AT_WHITE, "  hh:mm:ss  (military time) or  *  (for off)\n\r", ch );
	send_to_char(AT_WHITE, "\n\r",                                             ch );
	sprintf( buf,
		"1st warning:  %s\n\r2nd warning:  %s\n\r   Downtime:  %s\n\r",
		warning1, warning2, down_time );
	send_to_char(AT_RED, buf,                                                ch );
	sprintf( buf, "Sstime type: %s.\n\r", ( stype == 0 ? "reboot" : "shutdown" ) );
	send_to_char(AT_RED, buf, ch );
	return;
      }

    /* Set something */

    if ( !str_infix( arg1, "downtime" ) )
      {
	free_string( down_time );
	down_time = str_dup( arg2 );
	sprintf( buf, "Downtime is now set to:  %s\n\r", down_time );
	send_to_char(AT_RED, buf, ch );
	return;
      }
    if ( !str_infix( arg1, "1warning" ) )
      {
	free_string( warning1 );
	warning1 = str_dup( arg2 );
	sprintf( buf, "First warning will be given at:  %s\n\r", warning1 );
	send_to_char(AT_RED, buf, ch );
	return;
      }
    if ( !str_infix( arg1, "2warning" ) )
      {
	free_string( warning2 );
	warning2 = str_dup( arg2 );
	sprintf( buf, "Second warning will be given at:  %s\n\r", warning2 );
	send_to_char(AT_RED, buf, ch );
	return;
      }
    if ( !str_infix( arg1, "type" ) )
    {
      if ( is_number( arg2 ) )
        stype = atoi( arg2 );
      else
       {
         if ( !str_cmp( arg2, "reboot" ) )
           stype = 0;
         else if ( !str_cmp( arg2, "shutdown" ) )
           stype = 1;
         else
           {
             send_to_char( AT_RED, "Invalid sstime type.\n\r", ch );
             return;
           }
       }
       sprintf( buf, "SStype set to: %s.\n\r", ( stype == 0 ? "reboot"
          : "shutdown" ) );
       send_to_char( AT_RED, buf, ch );
       return;
    }

    /* Generate usage mesage */

    do_sstime( ch, "" );
    return;

}


void do_qset( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  int value = 0;
  
  if (IS_NPC( ch ) )
    return;
  
  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  strcpy( arg2, argument );
  value = atoi( arg2 );
  
  if ( arg1[0] == '\0' )
  {
    send_to_char( AT_WHITE, "&pSyntax: qset quest\n\r", ch );
    send_to_char( AT_WHITE, "&pSyntax: qset qmin level\n\r", ch );
    send_to_char( AT_WHITE, "&pSyntax: qset qmax level\n\r", ch );
    send_to_char( AT_WHITE, "&pSyntax: qset player\n\r", ch );
    if (quest)
      send_to_char( AT_WHITE, "There is currently a quest.\n\r", ch );
     else
      send_to_char( AT_WHITE, "There is currently no quest.\n\r", ch );
    sprintf( buf, "&R[&B%3d&R]    &pCurrent Qmin\n\r",  qmin );
    send_to_char( AT_WHITE, buf, ch );
    sprintf( buf, "&R[&B%3d&R]         &pCurrent Qmax\n\r", qmax );
    send_to_char( AT_WHITE, buf, ch );
    return;
  }
  
  if ( !str_cmp( arg1, "quest" ) || !str_cmp( arg1, "QUEST" ) )
  {
    if (quest)
      {
        CHAR_DATA *qch;
        
        quest = FALSE;
        qmin = 0;
        qmax = 100;
        send_to_char( AT_WHITE, "Ok, there is no longer a quest running.\n\r", ch );
        sprintf( buf, "The current quest has now ended." );
	do_echo( ch, buf );
	for ( qch = char_list; qch; qch = qch->next )
	{
	 if (!IS_NPC( qch ) )
	 {
	   if ( IS_SET( qch->act, PLR_QUEST ) )
	     qch->act ^= PLR_QUEST;
	 }
        }
        return;
      }
     else
      {
        quest = TRUE;
        send_to_char( AT_WHITE, "Ok, you have started a quest.\n\r", ch );
        sprintf( buf, "%s has just started a quest.", ch->name );
        do_echo( ch, buf );
        return;
      }
  }
  
  if ( !(victim = get_char_world( ch, arg1 ) ) )
  {
  
    if ( arg2[0] == '\0' ) 
    {
      send_to_char( AT_WHITE, "You must include a level.\n\r", ch );
      return;
    }
  
    if (!is_number( arg2 ) )
    {
      send_to_char( AT_WHITE, "Level must be a number.\n\r", ch );
      return;
    }
 
    if ( ( value < 0 ) || ( value > L_IMP ) )
    {
      send_to_char( AT_WHITE, "Invalid level.\n\r", ch );
      return;
    }
  
    if ( !str_cmp( arg1, "qmin" ) || !str_cmp( arg1, "QMIN" ) )
    {
      qmin = value;
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      return;
    }
  
    if ( !str_cmp( arg1, "qmax" ) || !str_cmp( arg1, "QMAX" ) )
    { 
      qmax = value;
      send_to_char( AT_WHITE, "Ok.\n\r", ch );
      return;
    } 
    return;
  }
  else
  {
    if (!IS_NPC( victim ) )
    {
      REMOVE_BIT( victim->act, PLR_QUEST );
      send_to_char( AT_WHITE, "Quest flag removed.\n\r", ch );
      return;
    }
   else
    {
      send_to_char( AT_WHITE, "Not on PCs.\n\r", ch );
      return;
    }
  }
  return;
}  

void do_pload( CHAR_DATA *ch, char *argument )
{
  char buffer[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA d;
  CHAR_DATA *vch = NULL;
  int  location;

  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );
  if ( arg[0] == '\0' )
  {
    send_to_char( AT_WHITE, "Load which player?\n\r", ch );
    return;
  }

  for ( vch = char_list; vch != NULL; vch = vch->next )
  {
    if ( vch->deleted )
      continue;
    if ( !IS_NPC( vch ) && !str_cmp( arg, vch->name ) )
    {
      send_to_char( AT_WHITE, "Character is already playing.\n\r", ch );
      return;
    }
  }

  vch = NULL;

  if ( !load_char_obj( &d, arg ) && str_cmp( arg2, "newname" ) )
  {
    free_char( d.character );
    send_to_char( AT_WHITE, "Player does not exist.\n\r", ch );
    return;
  }
  vch = d.character;
  d.character = NULL;
  vch->desc = NULL;
  vch->next = char_list;
  char_list = vch;
  location  = vch->in_room->vnum;
  char_to_room( vch, ch->in_room );
  {
    char buf[MAX_INPUT_LENGTH];

    strcpy( buf, vch->name );
    free_string( vch->name );
    vch->name = str_dup( capitalize( buf ) );
  }
  SET_BIT( vch->affected_by2, AFF_PLOADED );
  sprintf( buffer, "Player loaded.  Was in room vnum %d.\n\r", location );
  send_to_char( AT_WHITE, buffer , ch );
  return;
}

/* -- Altrag */
void do_sstat( CHAR_DATA *ch, char *argument )
{
  char spbuf[MAX_STRING_LENGTH*2];
  char skbuf[MAX_STRING_LENGTH];
  char wzbuf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int sn;
  int spcol = 0;
  int skcol = 0;
  int wzcol = 0;
  CHAR_DATA *victim;

  argument = one_argument( argument, arg ); 

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char(AT_WHITE, "They aren't here.\n\r", ch );
    return;
  }

  if ( IS_NPC( victim ) )
  {
    send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
    return;
  }

strcpy(spbuf,"\n\r");
strcpy(skbuf,"\n\r");
strcpy(wzbuf,"\n\r");
/*  spbuf[0] = '\n'; spbuf[1] = '\r'; spbuf[2] = '\0';
  skbuf[0] = '\n'; skbuf[1] = '\r'; skbuf[2] = '\0';
  wzbuf[0] = '\n'; wzbuf[1] = '\r'; wzbuf[2] = '\0';*/

  /* Use 1 to skip the reserved sn -- Altrag */
  for ( sn = 1; skill_table[sn].name[0] != '\0'; sn++ )
  {
    if ( victim->pcdata->learned[sn] <= 0 &&
	 !can_use_skpell( ch, sn ) )
      continue;
    sprintf( buf, "%-18s %3d%%  ", skill_table[sn].name,
	     victim->pcdata->learned[sn] );
    if ( skill_table[sn].spell_fun != spell_null )
    {
      strcat( spbuf, buf );
      if ( ++spcol % 3 == 0 )
	strcat( spbuf, "\n\r" );
      continue;
    }

    if ( skill_table[sn].minimum_position == POS_DEAD )
    {
      strcat( wzbuf, buf );
      if ( ++wzcol % 3 == 0 )
	strcat( wzbuf, "\n\r" );
      continue;
    }

    strcat( skbuf, buf );
    if ( ++skcol % 3 == 0 )
      strcat( skbuf, "\n\r" );
  }
  if ( spcol % 3 != 0 )
    strcat( spbuf, "\n\r" );
  if ( skcol % 3 != 0 )
    strcat( skbuf, "\n\r" );
  if ( wzcol % 3 != 0 )
    strcat( wzbuf, "\n\r" );
  send_to_char(AT_PINK,
"-----------------=================[Spells]=================-----------------",
	       ch );
  send_to_char(AT_PINK, spbuf, ch );
  send_to_char(AT_PURPLE,
"-----------------=================[Skills]=================-----------------",
	       ch );
  send_to_char(AT_PURPLE, skbuf, ch );
  send_to_char(AT_GREEN,
"-----------------==============[Wiz Commands]==============-----------------",
	       ch );
  send_to_char(AT_GREEN, wzbuf, ch );
}

void do_qecho( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *qch;

  strcat( argument, "\n\r" );
  
  for ( qch = char_list; qch; qch = qch->next )
  {
   if ( !IS_NPC( qch ) )
    if ( IS_SET( qch->act, PLR_QUEST ) )
    {
      send_to_char( AT_WHITE, argument, qch );
    }
  }
  return;
}


void do_mpcommands( CHAR_DATA *ch, char *argument )
{
  char buf[ MAX_STRING_LENGTH ];
  int cmd;
  int col;
  
  col = 0;
  for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
  {
     if ( !str_prefix( "mp", cmd_table[cmd].name ) )
     {
     if ( str_cmp( cmd_table[cmd].name, "mpcommands" ) )
     {
       sprintf( buf, "%-16s", cmd_table[cmd].name );
       if ( ++col % 5 == 0 )
          strcat( buf, "\n\r" );
       send_to_char(C_DEFAULT, buf, ch);
     }
     }
  }
  
  if ( col % 5 != 0 )
     send_to_char( C_DEFAULT, "\n\r", ch );
     
  return;
}

const char * log_list [] = {"normal", "always", "never", "build"};
void do_restrict( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_STRING_LENGTH];
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  int cmd;
  int lvl;
  bool log = FALSE;
  
  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  
  if ( arg[0] != '\0' && arg1[0] == '\0' )
  {
    char buf[MAX_STRING_LENGTH];

    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
      if ( !str_prefix( cmd_table[cmd].name, arg ) )
	break;
    if ( cmd_table[cmd].name[0] == '\0' || cmd_table[cmd].level > get_trust(ch) )
    {
      do_restrict( ch, "" );
      return;
    }
    sprintf( buf, "Command '%s'.  Level %d.  Logged %s.\n\r",
	     cmd_table[cmd].name, cmd_table[cmd].level,
	     log_list[cmd_table[cmd].log] );
    send_to_char( AT_PURPLE, buf, ch );
    return;
  }

  if ( arg[0] == '\0' || arg1[0] == '\0' || ( !is_number( arg1 ) 
     && ( ( arg2[0] == '\0' ) || !is_number( arg2 ) ) ) )
  {
     send_to_char( AT_PURPLE, "Syntax: restrict [command] [level]\n\r", ch );
     send_to_char( AT_PURPLE, "Syntax: restrict [command] log [logtype]\n\r", ch );
     send_to_char( AT_PURPLE, "Syntax: restrict [command]\n\r\n\r",ch);
     send_to_char( AT_PURPLE, "Logtype being 0-4:\n\r", ch );
     send_to_char( AT_PURPLE, "0 - LOG_NEVER\n\r", ch);
     send_to_char( AT_PURPLE, "1 - LOG_ALWAYS (on God)\n\r", ch);
     send_to_char( AT_PURPLE, "2 - LOG_NORMAL (not logged)\n\r", ch );
     send_to_char( AT_PURPLE, "3 - LOG_BUILD  (logged on build)\n\r", ch );
     return;
  }
  
  if ( arg2[0] != '\0' )
  {
     if ( !str_cmp( "log", arg1 ) )
     {
       lvl = atoi( arg2 );
       log = TRUE;
    }
   else
    lvl = atoi( arg1 );
  }
else
  lvl = atoi( arg1 );
  
  if ( !str_cmp( "all", arg ) )
  {
    int col = 0;
    cmd = 1;
    lvl = 1;
    if ( is_number( arg1 ) )
      cmd = atoi( arg1 );
    if ( is_number( arg2 ) )
      lvl = atoi( arg2 );
      send_to_char( AT_WHITE, "\n\r", ch );
    for ( ; ( cmd <= lvl ) && ( cmd_table[cmd].name[0] != '\0' ); cmd++ )
    {
      if ( get_trust( ch ) >= cmd_table[cmd].level )
      {
        sprintf( log_buf, "%-3d &R%-12s &B(&Y%3d&B)   ", cmd, cmd_table[cmd].name, cmd_table[cmd].level );
        send_to_char( AT_WHITE, log_buf, ch );
      }
      if ( ++col % 3 == 0 )
        send_to_char( AT_WHITE, "\n\r", ch );
    }
    if ( col % 3 != 0 )
      send_to_char( AT_WHITE, "\n\r", ch );
    return;
  }
  
  if ( ( ( lvl < 0 || lvl > L_IMP || lvl > get_trust( ch ) )&& ( arg2[0] == '\0' ) )
   || ( ( arg2[0] != '\0' ) && ( lvl < 0 || lvl > 3 ) ) )
  {
     send_to_char( AT_WHITE, "Invalid level.\n\r", ch );
     return;
  }
  
  for ( cmd = 1; cmd_table[cmd].name != '\0'; cmd++ )
  {
    if ( !str_prefix( arg, cmd_table[cmd].name ) )
    {
      strcpy( arg, cmd_table[cmd].name );
      
      if ( cmd_table[cmd].level > get_trust( ch ) )
      {
        if (log)
        send_to_char( AT_WHITE, "You cannot change the log_type on a command which you do not have.\n\r", ch );
      else
        send_to_char( AT_WHITE, "You cannot restrict a command which you do not have.\n\r", ch );
        return;
      }
      if (log)
        cmd_table[cmd].log = lvl;
      else
      cmd_table[cmd].level = lvl;
      if (log)
        sprintf( log_buf, "%s changing log_type of %s to %d.",
         ch->name, arg, lvl );
      else
        sprintf( log_buf, "%s restricting %s to level %d.",
        ch->name, arg, lvl );
      log_string( log_buf, CHANNEL_GOD, ch->level - 1 );
      if (log)
        sprintf( log_buf, "You change the log_type of %s to %d.\n\r",
        arg, lvl );
      else
        sprintf( log_buf, "You restrict %s to level %d.\n\r",
        arg, lvl );
      send_to_char( AT_WHITE, log_buf, ch );
      break;
    }
    
  }
  if ( cmd_table[cmd].name == '\0' )
  {
     sprintf( log_buf, "There is no %s command.",
       arg );
     send_to_char( AT_WHITE, log_buf, ch );  
  }
  return;
}

void do_wrlist( CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA *room;
  ROOM_INDEX_DATA *in_room;
  MOB_INDEX_DATA *mob;
  OBJ_INDEX_DATA *obj;
  MPROG_DATA *mprog;
  char arg[MAX_STRING_LENGTH];
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  int uvnum;
  int lvnum;
  int MR = 32767;
  int type = -1;
  
  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  uvnum = ( is_number( arg2 ) ) ? atoi( arg2 ) : 0;
  lvnum = ( is_number( arg1 ) ) ? atoi( arg1 ) : 0;  

  if ( !str_cmp( arg, "o" ) )
    type = 2;
  if ( !str_cmp( arg, "m" ) )
    type = 1;
  if ( !str_cmp( arg, "r" ) )
    type = 0;
  if ( !str_cmp( arg, "p" ) )
    type = 3;

  if ( ( uvnum - lvnum ) > 200 )
  {
    send_to_char( AT_WHITE, "That range is too large.\n\r", ch );
    return;
  }
  
  if ( ( ( uvnum == 0 ) && ( lvnum == 0 ) ) || ( arg[0] == '\0' ) 
   || ( type == -1 ) )
  {
    send_to_char( AT_PURPLE, "Syntax: wrlist [type] [lvnum] [uvnum]\n\r", ch );
    return;
  }

  if ( uvnum > MR || uvnum < 1 || lvnum > MR || lvnum < 1 || lvnum > uvnum )
  {
    send_to_char( AT_WHITE, "Invalid level(s).\n\r", ch );
    return;
  }

  in_room = ch->in_room;  
  if ( type == 0 )
  {
    char_from_room( ch );
  }
  for ( MR = lvnum; MR <= uvnum; MR++ )
  {
    if ( type == 0 )
    {
      if ( ( room = get_room_index( MR ) ) )
      {
        sprintf( log_buf, "&R%-5d  &w%-20s\n\r", room->vnum, room->name );
        send_to_char( AT_WHITE, log_buf, ch );
        char_to_room( ch, room );
        do_resets( ch, "" );
        char_from_room( ch );
      }
    }
    if ( type == 2 )
    {
      if ( ( obj = get_obj_index( MR ) ) )
      {
        sprintf( log_buf, "&R%-5d  &w%-20s\n\r",  obj->vnum, obj->name );
        send_to_char( AT_WHITE, log_buf, ch );
      }
    }
    if ( type == 1 )
    {
      if ( ( mob = get_mob_index( MR ) ) )
      {
        sprintf( log_buf, "&R%-5d  &w%-20s\n\r", mob->vnum, mob->player_name );
        send_to_char( AT_WHITE, log_buf, ch );
        if ( mob->mobprogs )
          for ( mprog = mob->mobprogs; mprog; mprog = mprog->next )
          {
            sprintf( log_buf, "  :%s  %s\n\r", mprog_type_to_name( mprog->type ),
              mprog->arglist ? mprog->arglist : "(None)" );
            send_to_char( C_DEFAULT, log_buf, ch );
          }
      }
    }
    if ( type == 3 )
    {
      if ( ( mob = get_mob_index( MR ) ) )
      {
        if ( mob->mobprogs )
        {
          sprintf( log_buf, "%d: %s\n\r", mob->vnum, mob->player_name );
          send_to_char( AT_WHITE, log_buf, ch );
          for ( mprog = mob->mobprogs; mprog; mprog = mprog->next )
          {
            sprintf( log_buf, "%s&R:&B %s\n\r&w%s\n\r",
              mprog_type_to_name( mprog->type ),
              mprog->arglist ? mprog->arglist : "(None)",
              mprog->comlist ? mprog->comlist : "(None)"  );
            send_to_char( C_DEFAULT, log_buf, ch );
          }    
        }
      }
  }  
  }
  if ( type == 0 )
    char_to_room( ch, in_room );
  return;
}

void do_vused( CHAR_DATA *ch, char *argument )
{
  char buf1[MAX_STRING_LENGTH];
  int atvnum;
  int freevnum = 0;
  int bstart = -1;
  int bend;
  int col = 0;

  send_to_char(AT_PINK,"Used VNUMs (ignores vnums assigned but not created).\n\r",ch);
  send_to_char(AT_PINK,"Double check free vnums against assigned vnums in &Balist&P.\n\r",ch);

  for ( atvnum = 1; atvnum <= 32767; atvnum++ )
  {
    if ( get_room_index( atvnum ) || get_mob_index(atvnum) || get_obj_index(atvnum))
    {
      if ( bstart == -1 )
        bstart = atvnum;
    }
   else if ( bstart != -1 )
    {
      bend = ( atvnum - 1 ); 
      sprintf( buf1, "%5d&R-&W%5d  ", bstart, bend );
      send_to_char( AT_WHITE,buf1,ch);
      if ( ++col % 6 == 0 )
	send_to_char( AT_WHITE,"\n\r",ch);
      freevnum++;
      bstart = -1; 
    }
   else
     freevnum++;
  }
  if ( col % 6 != 0 )
    send_to_char( AT_WHITE, "\n\r",ch );
  sprintf( buf1, "There are %d free vnums.\n\r", freevnum );
  send_to_char( AT_WHITE, buf1, ch );
  return;
}

void do_hlist( CHAR_DATA *ch, char *argument )
{
  HELP_DATA *pHelp;
  int uvnum;
  int lvnum;
  int atvnum = 1;
  char arg[MAX_STRING_LENGTH];
  char arg1[MAX_STRING_LENGTH];
  
  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg1 );
  lvnum = is_number( arg ) ? atoi( arg ) : 0;
  uvnum = is_number( arg1 ) ? atoi( arg1 ) : 0;
  
  if ( lvnum == 0 || uvnum == 0 || lvnum > uvnum )
  {
    send_to_char( AT_PURPLE, "Syntax: hlist [firsthelp#] [secondhelp#]\n\r",
       ch );
    return;
  }
  
  for ( pHelp = help_first; pHelp; pHelp = pHelp->next, ++atvnum )
  {
    if ( atvnum >= lvnum && atvnum <= uvnum )
    {
      sprintf( arg, "[&R%-3d&B]  &w%s\n\r", atvnum, pHelp->keyword );
      send_to_char( AT_BLUE, arg, ch );
    }
  }
  
  return;
}
    
void do_astrip( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;

  argument = one_argument( argument, arg );

  if ( !(victim = get_char_room(ch, arg ) ) )
  {
    send_to_char(C_DEFAULT, "They aren't here.\n\r",ch);
    return;
  }

  for( paf = victim->affected; paf; paf = paf_next )
  {
    paf_next = paf->next;
    if ( !paf->deleted )
      affect_remove( victim, paf );
  }

  for( paf = victim->affected2; paf; paf = paf_next )
  {
    paf_next = paf->next;
    if ( !paf->deleted )
      affect_remove2( victim, paf );
  }

  victim->affected_by = 0;
  victim->affected_by2 = 0;

  if ( ch != victim )
    send_to_char(C_DEFAULT, "All your affects have been removed.\n\r", victim);
  send_to_char(C_DEFAULT, "Ok.\n\r",ch);
}

void do_bodybag( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  OBJ_DATA *body = NULL;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int howmany = 0;

  argument = one_argument( argument, arg );

  for ( obj = object_list; obj; obj = obj_next )
  {
    OBJ_DATA *in;
    OBJ_DATA *in_next;

    obj_next = obj->next;
    if ( obj->deleted )
      continue;

    if ( obj->item_type != ITEM_CORPSE_PC )
      continue;
    if ( !obj->in_room )
      continue;
    if ( str_cmp( arg, obj->short_descr + 10 ) )
      continue;

    for ( in = obj->contains; in; in = in_next )
    {
      in_next = in->next_content;

      if ( howmany == 0 )
      {
	char name[MAX_INPUT_LENGTH];

	strcpy(name, obj->short_descr + 10);
	body = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0 );
        sprintf( buf, "bodybag body bag %s", name );
        body->name = str_dup( buf );
	sprintf(buf, "corpse of %s", name );
	body->short_descr = str_dup( buf );
/*	sprintf(buf, "%s'%s bodybag", name,
	       (name[strlen(name)-1] == 's' ? "" : "s") );
	body->short_descr = str_dup(buf); */
	sprintf(buf, "The bodybag of %s.", name);
	body->description = str_dup(buf);
	obj_to_char(body, ch);
      }
      obj_from_obj(in);
      obj_to_obj(in, body);
      howmany++;
    }
    extract_obj(obj);
    body->timer = number_range( 25, 45 );
  }
  return;
}

/* Poor Newbie lost his corpse, let's make him another
 * Bram 5/96
 */
void do_newcorpse( CHAR_DATA *ch, char *argument )
{
    FILE      *fp;
/*  CHAR_DATA *rch; */
    CHAR_DATA *victim;
    OBJ_DATA  *corpse;
    OBJ_DATA  *obj;
    char       strsave [ MAX_INPUT_LENGTH  ];
    char       buf     [ MAX_STRING_LENGTH ];
    char       arg1    [ MAX_INPUT_LENGTH  ];
    char       arg2    [ MAX_INPUT_LENGTH  ];
    int        c, number;
    int        corpse_cont[1024];
    int        item_level[1024];
    int        checksum1, checksum2;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /* Show usage */
    if ( arg1[0] == '\0' && arg2[0] == '\0' )
    {
       send_to_char( AT_GREY, "Syntax:  newcorpse <playername>\n\r",            ch );
       send_to_char( AT_GREY, "         newcorpse <playername> <corpse #>\n\r", ch );
       send_to_char( AT_GREY, "Author: Bram    Email: bram@ionet.net\n\r",     ch  );
       return;    
    }   
    if ( !( victim = get_pc_world( ch, arg1 ) ) )
    {
	send_to_char( AT_GREY, "They aren't here.\n\r", ch );
	return;
    }
    if ( victim->pcdata->corpses == 0 )
    {
	send_to_char( AT_GREY, "That player has no corpses on file.\n\r", ch );
	return;
    }
    /* Okay, the victim has a corpse file let's get 'em*/


    fclose( fpReserve );
    
    /* player files parsed directories by Yaz 4th Realm */
#if !defined( macintosh ) && !defined( MSDOS )
    sprintf( strsave, "%s%c/%s.cps", PLAYER_DIR, LOWER(victim->name[0]),
              capitalize( victim->name ) );
#else
    sprintf( strsave, "%s%s.cps", PLAYER_DIR, capitalize( victim->name ) );
#endif

    if ( !( fp = fopen( strsave, "r" ) ) )
    {
        sprintf( buf, "New Corpse: fopen %s: ", victim->name );
	bug( buf, 0 );
	sprintf( buf, "No existing corpse file for %s.\n\r",
		 victim->name );
	send_to_char( AT_WHITE, buf, ch );
	perror( strsave );
	return;
    }
    else
    {
        corpse_cont[0] = fread_number( fp );
        item_level[0]  = fread_number( fp );

        for ( c = 1 ; c < corpse_cont[0]+2 ; c++ )
            {    
            corpse_cont[c] = fread_number ( fp );            
            item_level[c]  = fread_number ( fp );        
            }
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
  
    /* Ok now we have the data on all the corpses. now what? */
    
    if ( arg2[0] == '\0' ) /* show 'em */
    {
        sprintf( buf, "%s's corpse:\n\r", victim->name );
        send_to_char( AT_GREY, buf, ch );
        send_to_char( AT_GREY, "Corpse    Contents   Flag\n\r", ch   );
        send_to_char( AT_GREY, "---------------------------\n\r", ch );
        sprintf( buf, "  %d          %3d   ", 1, corpse_cont[0] ); 
        send_to_char( AT_GREY, buf, ch );
        checksum1=0;
        checksum2=0;
        for ( c = 1 ; c < corpse_cont[0]+1; c++ )
            {    
            checksum1 += corpse_cont[c];
            checksum2 += item_level[c];             
            }
        if ( checksum1 == corpse_cont[c]             
             && checksum2 == item_level[c] )         
	    send_to_char( AT_GREY, "Valid\n\r", ch );
	else
	    send_to_char( AT_RED, "Invalid\n\r", ch );          
        return;
    }
    
    if (is_number( arg2 ) )               
    {
	number = atoi( arg2 );
        if ( number != 1 )
        {
            send_to_char( AT_GREY, "Corpse number must be 1\n\r", ch );
            return;
        }
        /* Ok now we've done all the checks, let's try making a corpse */

	corpse        = create_object( get_obj_index( OBJ_VNUM_CORPSE_PC ),
				   0 );
	corpse->timer = -1;

        sprintf( buf, corpse->short_descr, victim->name );
        free_string( corpse->short_descr );
        corpse->short_descr = str_dup( buf );
	
        sprintf( buf, corpse->description, victim->name );
        free_string( corpse->description );
        corpse->description = str_dup( buf );

        for ( c = 1 ; c < corpse_cont[0]+1 ; c++ )
        {    
                  
             obj = create_object( get_obj_index( corpse_cont[c] ),
                                  item_level[c] );           
             obj_to_obj( obj, corpse );
        }
        act( AT_GREY, "You create a $p.", ch, corpse, NULL, TO_CHAR );
        act( AT_GREY, "$n has created a $p!", ch, corpse, NULL, TO_ROOM );
        wiznet( "$N has created a $p.", ch, corpse, 
                  WIZ_LOAD, WIZ_SECURE, get_trust( ch ) );
        obj_to_room( corpse , ch->in_room );
        
    }    
    return; 
}       

void do_wiznet( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    int flag;
    char buf[MAX_STRING_LENGTH];

    rch = get_char( ch );

    if ( argument[0] == '\0' )
    {
      	if (IS_SET(ch->wiznet,WIZ_ON))
      	{
            send_to_char( AT_RED, "Signing off of Wiznet.\n\r",ch);
            REMOVE_BIT(ch->wiznet,WIZ_ON);
      	}
      	else
      	{
            send_to_char( AT_RED, "Welcome to Wiznet!\n\r",ch);
            SET_BIT(ch->wiznet,WIZ_ON);
      	}
      	return;
    }

    if (!str_prefix(argument,"on"))
    {
	send_to_char( AT_RED, "Welcome to Wiznet!\n\r",ch);
	SET_BIT(ch->wiznet,WIZ_ON);
	return;
    }

    if (!str_prefix(argument,"off"))
    {
	send_to_char( AT_RED, "Signing off of Wiznet.\n\r",ch);
	REMOVE_BIT(ch->wiznet,WIZ_ON);
	return;
    }

    /* show wiznet status */
    if (!str_prefix(argument,"status")) 
    {
	buf[0] = '\0';

	if (!IS_SET(ch->wiznet,WIZ_ON))
	    strcat(buf,"off ");

	for (flag = 0; wiznet_table[flag].name != NULL; flag++)
	    if (IS_SET(ch->wiznet,wiznet_table[flag].flag))
	    {
		strcat(buf,wiznet_table[flag].name);
		strcat(buf," ");
	    }

	strcat(buf,"\n\r");

	send_to_char( AT_RED, "Wiznet status:\n\r",ch);
	send_to_char( AT_RED, buf,ch);
	return;
    }

    if (!str_prefix(argument,"show"))
    /* list of all wiznet options */
    {
	buf[0] = '\0';

	for (flag = 0; wiznet_table[flag].name != NULL; flag++)
	{
	    if (wiznet_table[flag].level <= get_trust(ch))
	    {
	    	strcat(buf,wiznet_table[flag].name);
	    	strcat(buf," ");
	    }
	}

	strcat(buf,"\n\r");

	send_to_char(AT_RED, "Wiznet options available to you are:\n\r",ch);
	send_to_char(AT_RED, buf,ch);
	return;
    }

    if ( !str_cmp( argument, "all" ) )
    {
      if (!IS_SET(ch->wiznet,WIZ_ON))   /* turn all available options on */
      {
        for (flag = 0; wiznet_table[flag].name != NULL; flag++)
        {
            if (wiznet_table[flag].level <= get_trust(ch))
              SET_BIT(ch->wiznet,wiznet_table[flag].flag); 
        }   
	send_to_char( AT_RED, "All available options on.\n\r", ch );
      }
      else   /* turn all available options off */
      {
	for (flag = 0; wiznet_table[flag].name != NULL; flag++ )
	{
	    if (IS_SET(ch->wiznet,wiznet_table[flag].flag))
	      REMOVE_BIT(ch->wiznet,wiznet_table[flag].flag);
	}
	send_to_char( AT_RED, "All available options off.\n\r", ch );
      }
      return;
    }

    flag = wiznet_lookup(argument);

    if (flag == -1 || get_trust(ch) < wiznet_table[flag].level)
    {
	send_to_char(AT_RED,"No such option.\n\r",ch);
	return;
    }
   
    if (IS_SET(ch->wiznet,wiznet_table[flag].flag))
    {
	sprintf(buf,"You will no longer see %s on wiznet.\n\r",
	        wiznet_table[flag].name);
	send_to_char(AT_RED,buf,ch);
	REMOVE_BIT(ch->wiznet,wiznet_table[flag].flag);
    	return;
    }
    else
    {
    	sprintf(buf,"You will now see %s on wiznet.\n\r",
		wiznet_table[flag].name);
	send_to_char(AT_RED,buf,ch);
    	SET_BIT(ch->wiznet,wiznet_table[flag].flag);
	return;
    }

}

void wiznet(char *string, CHAR_DATA *ch, OBJ_DATA *obj,
	    long flag, long flag_skip, int min_level) 
{
    DESCRIPTOR_DATA *d;
    bool oldpos;
    
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->connected == CON_PLAYING
	&&  IS_IMMORTAL(d->character) 
	&&  IS_SET(d->character->wiznet,WIZ_ON) 
	&&  (!flag || IS_SET(d->character->wiznet,flag))
	&&  (!flag_skip || !IS_SET(d->character->wiznet,flag_skip))
	&&  get_trust(d->character) >= min_level
	&&  d->character != ch)
        {
	    if (IS_SET(d->character->wiznet,WIZ_PREFIX))
	    {	
	  	send_to_char( AT_RED, "Wiznet> ", d->character);
            }
           oldpos = d->character->position;
           d->character->position = POS_STANDING;
           act( AT_RED, string, d->character, obj, ch, TO_CHAR );
           d->character->position = oldpos;
        }
    }
 
    return;
}

void do_nukerep (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            if( ( victim = d->character ) != NULL )
                if( victim->reply == ch )
                    victim->reply = NULL;
        }
        send_to_char( AT_WHITE, "You nuked all replies to you.\r\n", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( IS_NPC(victim) ) )
    {
        send_to_char( AT_WHITE, "They aren't here.\n\r", ch );
        return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
        act( AT_WHITE, "$N seems to have misplaced $S link...try again later.",
            ch,NULL,victim,TO_CHAR);
        return;
    }
    victim->reply = NULL;

    send_to_char( AT_WHITE, "Reply nuked.\r\n", ch );
    return;
}

void do_seize (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA  *victim;
    OBJ_DATA   *obj;
    char 	arg1 [ MAX_INPUT_LENGTH ];
    char 	arg2 [ MAX_INPUT_LENGTH ];
    char 	arg3 [ MAX_INPUT_LENGTH ];
    char 	buf  [ MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( ( arg1[0] == '\0' ) || ( arg2[0] == '\0' ) )
    {
        send_to_char( AT_WHITE, "Syntax : seize <object> <player>\n\r", ch );
        return;
    }

    if ( !( victim = get_char_world( ch, arg2 ) ) )
    {
        send_to_char( AT_WHITE, "They are not here!\n\r", ch );
        return;
    }

    if ( !( obj = get_obj_list( ch, arg1, victim->carrying ) ) )
    {
        send_to_char( AT_WHITE, "They do not have that item.\n\r", ch );
        return;
    }

    if ( victim->level >= ch->level )
    {
        send_to_char( AT_WHITE, "You Failed.\r\n", ch );
        return;
    }

    if ( obj->wear_loc != WEAR_NONE )
        unequip_char( victim, obj );

    obj_from_char( obj );
    obj_to_char( obj, ch );

    act( AT_WHITE, "You seize $p from $N.", ch, obj, victim, TO_CHAR );
    if ( arg3[0] == '\0' 
    	|| !str_cmp( arg3, "yes" ) || !str_cmp( arg3, "true" ) )		
           act( AT_WHITE, "You no longer own $p.", ch, obj, victim, TO_VICT );

    sprintf( buf, "%s seizes %s from %s.", ch->name, obj->short_descr, victim->name);
    wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, get_trust( ch ) );

    return;
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone)
{
    OBJ_DATA *c_obj, *t_obj;


    for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content)
    {
	    t_obj = create_object(c_obj->pIndexData,0);
	    clone_object(c_obj,t_obj);
	    obj_to_obj(t_obj,clone);
	    recursive_clone(ch,c_obj,t_obj);
    }
}

/* command that is similar to load */
void do_clone(CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char *rest;
    CHAR_DATA *mob;
    OBJ_DATA  *obj;


    rest = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
	send_to_char(AT_WHITE,"Clone what?\n\r",ch);
	return;
    }

    if (!str_prefix(arg,"object"))
    {
	mob = NULL;
	obj = get_obj_here(ch,rest);
	if (obj == NULL)
	{
	    send_to_char(AT_WHITE,"You don't see that here.\n\r",ch);
	    return;
	}
    }
    else if (!str_prefix(arg,"mobile") || !str_prefix(arg,"character"))
    {
	obj = NULL;
	mob = get_char_room(ch,rest);
	if (mob == NULL)
	{
	    send_to_char(AT_WHITE,"You don't see that here.\n\r",ch);
	    return;
	}
    }
    else /* find both */
    {
	mob = get_char_room(ch,argument);
	obj = get_obj_here(ch,argument);
	if (mob == NULL && obj == NULL)
	{
	    send_to_char(AT_WHITE,"You don't see that here.\n\r",ch);
	    return;
	}
    }

    /* clone an object */
    if (obj != NULL)
    {
	OBJ_DATA *clone;

	clone = create_object(obj->pIndexData,0); 
	clone_object(obj,clone);
	if (obj->carried_by != NULL)
	    obj_to_char(clone,ch);
	else
	    obj_to_room(clone,ch->in_room);
 	recursive_clone(ch,obj,clone);

	act(AT_WHITE,"$n has created $p.",ch,clone,NULL,TO_ROOM);
	act(AT_WHITE,"You clone $p.",ch,clone,NULL,TO_CHAR);
	wiznet("$N clones $p.",ch,clone,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
	return;
    }
    else if (mob != NULL)
    {
	CHAR_DATA *clone;
	OBJ_DATA *new_obj;
	char buf[MAX_STRING_LENGTH];

	if (!IS_NPC(mob))
	{
	    send_to_char(AT_WHITE,"You can only clone mobiles.\n\r",ch);
	    return;
	}

	clone = create_mobile(mob->pIndexData);
	clone_mobile(mob,clone); 
	
	for (obj = mob->carrying; obj != NULL; obj = obj->next_content)
	{
		new_obj = create_object(obj->pIndexData,0);
		clone_object(obj,new_obj);
		recursive_clone(ch,obj,new_obj);
		obj_to_char(new_obj,clone);
		new_obj->wear_loc = obj->wear_loc;
	}
	char_to_room(clone,ch->in_room);
        act(AT_WHITE,"$n has created $N.",ch,NULL,clone,TO_ROOM);
        act(AT_WHITE,"You clone $N.",ch,NULL,clone,TO_CHAR);
	sprintf(buf,"$N clones %s.",clone->short_descr);
	wiznet(buf,ch,NULL,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
        return;
    }
}
void do_whotype( CHAR_DATA *ch, char *argument )
{
  char		buf [ MAX_INPUT_LENGTH ];

  if ( IS_NPC( ch ) || ch->level < LEVEL_IMMORTAL )
	return;
  if ( argument[0] == '\0' )
    {
    send_to_char( C_DEFAULT, "Change your whotype to what?\n\r", ch );
    return;
    }
  if ( strlen_wo_col( argument ) > 11 )
    {
    send_to_char( C_DEFAULT, 
   		  "Maximum whotype length is 11 excluding color codes.\n\r",
		  ch );
    return;
    }
  smash_tilde( argument );

  buf[0] = '\0';
  if ( !str_cmp( "default", argument ) )
	{
	free_string( ch->pcdata->whotype );
	ch->pcdata->whotype = str_dup( "!!!!!!!!!!!!" );
	}
  else
	{
	free_string( ch->pcdata->whotype );
	ch->pcdata->whotype = str_dup( argument );
	}
  send_to_char(C_DEFAULT, "Ok.\n\r", ch );
  return;
}

void do_descript_clean( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d, *d_next;
    int number = 0;
    
    for ( d = descriptor_list; d != NULL; d = d_next ) 
    {
        d_next = d->next;
	
	/*took out NULL forseen probs with hanging descriptors*/
	
        if( (d->connected == CON_GET_NAME) || (d->connected == CON_GET_ANSI) )
          {
            close_socket(d);
            number++;
          }
    }
    if (number > 0)
    	send_to_char(AT_WHITE, "Descriptors cleaned!\n\r", ch );
    else
	send_to_char(AT_WHITE, "Descriptors OK.\n\r", ch );
    	
    return;
}

/*
 * To allow clan's to have clan quest points,
 *  seperate from normal quests -Angi      */ 
void do_clanquest( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA   *victim;
    char buf    [ MAX_STRING_LENGTH ]; 
    char buf1   [ MAX_STRING_LENGTH ];
    char arg1   [ MAX_INPUT_LENGTH ];
    char arg2   [ MAX_INPUT_LENGTH ];
    char arg3   [ MAX_INPUT_LENGTH ];
    int  value;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
       send_to_char( AT_WHITE, "&RSyntax: &Wcquest earn  <victim> <amount>\n\r", ch );
       send_to_char( AT_WHITE, "    &Ror: &Wcquest spend <victim> <amount>\n\r", ch );
       return;
    }
  
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    if ( ( ch->clan == 0 ) || ( ch->clev < 5 ) )
       return;

    if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
       send_to_char( AT_WHITE, "They aren't here.\n\r", ch );
       return;
    }      
 
    if IS_NPC( victim )
    {
       send_to_char( AT_WHITE, "Not on NPC's.\n\r", ch );
       return;
    }

    if ( !victim->desc )
    {
       send_to_char( AT_WHITE, "Not on link-dead characters.\n\r", ch );
       return;
    }

    if ( victim == ch )
    {
       send_to_char( AT_WHITE, "You are a deity, you cannot gain clan quest points!\n\r", ch );
       return;
    }

    if ( victim->clan != ch->clan )    
    {
       send_to_char( AT_WHITE, "You cannot set the clan quest points of a person outside your clan.\n\r", ch );
       return;
    }

    if ( value < 0 )
    {
       send_to_char( AT_WHITE, "Amount cannot be negative.\n\r", ch );
       return;
    }
   
    if ( !str_cmp( "earn", arg1 ) )
    {
       /* raise victim's clan qp */
 
       sprintf( buf, "&RYou've earned %d Clan Quest points!! Congratulations!!\n\r", value );
       sprintf( buf1, "&RYou reward %s with %d Clan Quest points.\n\r", victim->name, value );
       send_to_char( AT_RED, buf, victim );
       send_to_char( AT_RED, buf1, ch );
       victim->cquestpnts += value;
       sprintf( buf, "&WYou now have %d Clan Quest points.\n\r", victim->cquestpnts );
       sprintf( buf1, "&W%s now has %d Clan Quest points.\n\r", victim->name, victim->cquestpnts );
       send_to_char( AT_WHITE, buf, victim );
       send_to_char( AT_WHITE, buf1, ch );
       return;
    
    }
    else
       if ( !str_cmp( "spend", arg1 ) )
       {
          /* lower victim's clan qp */

          if ( value > victim->cquestpnts )
          {
             if ( victim->cquestpnts > 0 )
                sprintf( buf, "&W%s only has %d Clan Quest points to spend.\n\r", victim->name, victim->cquestpnts );
             else
                sprintf( buf, "&W%s doesn't have any Clan Quest points to spend.\n\r", victim->name );
             send_to_char( AT_WHITE, buf, ch );
             return;
          }

          sprintf( buf, "&WYou spend %d Clan Quest points.\n\r", value );
          sprintf( buf1, "&WYou lower %s's Clan Quest points by %d.\n\r", victim->name, value );
          send_to_char( AT_WHITE, buf, victim );
          send_to_char( AT_WHITE, buf1, ch );
          victim->cquestpnts -= value;
          sprintf( buf, "&WYou now have %d Clan Quest points.\n\r", victim->cquestpnts );
          sprintf( buf1, "&W%s now has %d Clan Quest points.\n\r", victim->name, victim->cquestpnts );
          send_to_char( AT_WHITE, buf, victim );
          send_to_char( AT_WHITE, buf1, ch );
          return;

       }
       else
       {
          send_to_char( AT_WHITE, "&RSyntax: &Wcquest earn <victim> <amount>\n\r", ch );
          send_to_char( AT_WHITE, "    &Ror: &Wcquest spend <victim> <amount>\n\r", ch );
          return;                  
       }
}

void do_rebuild (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    RACE_DATA *pRace;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    int	       sn;
    int	       level;
    int        iLevel;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
        send_to_char(AT_WHITE, "Syntax: rebuild <char>.\n\r", ch );
        return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
        send_to_char(AT_WHITE, "That player is not here.\n\r", ch);
        return;
    }

    if ( IS_NPC( victim ) )
    {
        send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
        return;
    }

    /* strip them of affects */
    do_astrip( ch, victim->name );

    /* have them remove equipment */
    {
        for ( obj = victim->carrying; obj; obj = obj->next_content )
        {
            if ( obj->wear_loc != WEAR_NONE )
              unequip_char( victim, obj );
        }
    }

    /* reset misc */
    victim->pcdata->condition[COND_THIRST]  =  0;
    victim->pcdata->condition[COND_FULL]    =  0;
    victim->pcdata->condition[COND_DRUNK]    = 0;
    victim->saving_throw = 0;

    /* level one stats */
    victim->perm_hit  = 10;
    victim->mod_hit  = 0;
    victim->perm_mana = 100;
    victim->mod_mana = 0;
    victim->perm_bp   = 20;
    victim->mod_bp = 0;
    victim->perm_move = 100;
    victim->mod_move = 0;
    victim->practice = 21;

    /* set skills and spells to 0 */
    for ( sn = 0; skill_table[sn].name[0] != '\0'; sn++ )
     victim->pcdata->learned[sn] = 0;

    /* set stuff to max for purpose of refiguring hp mana move and pracs */
    victim->pcdata->mod_str = 25;
    victim->pcdata->mod_int = 25;
    victim->pcdata->mod_wis = 25;
    victim->pcdata->mod_dex = 25;
    victim->pcdata->mod_con = 25;

    victim->pcdata->perm_str = 18; 
    victim->pcdata->perm_int = 18; 
    victim->pcdata->perm_wis = 18; 
    victim->pcdata->perm_dex = 18; 
    victim->pcdata->perm_con = 18; 

    level = victim->level;
    victim->level = 1;   

    /* refigure hp mana move and pracs */
    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
	send_to_char(AT_RED, "You raise a level!!  ", victim );
	victim->level += 1;
	advance_level( victim );
    }

    /* reset stats to level 1*/
    pRace = get_race_data(victim->race);
    victim->pcdata->mod_str = pRace->mstr;
    victim->pcdata->mod_int = pRace->mint;
    victim->pcdata->mod_wis = pRace->mwis;
    victim->pcdata->mod_dex = pRace->mdex;
    victim->pcdata->mod_con = pRace->mcon;

    victim->pcdata->perm_str = 13; 
    victim->pcdata->perm_int = 13; 
    victim->pcdata->perm_wis = 13; 
    victim->pcdata->perm_dex = 13; 
    victim->pcdata->perm_con = 13; 

    switch ( class_table[prime_class(victim)].attr_prime )
    {
      case APPLY_STR: victim->pcdata->perm_str = 16; break;
      case APPLY_INT: victim->pcdata->perm_int = 16; break;
      case APPLY_WIS: victim->pcdata->perm_wis = 16; break;
      case APPLY_DEX: victim->pcdata->perm_dex = 16; break;
      case APPLY_CON: victim->pcdata->perm_con = 16; break;
    } 

   /* restore */
   victim->hit      = MAX_HIT(victim);
   victim->mana     = MAX_MANA(victim);
   victim->bp       = MAX_BP(victim);
   victim->move     = MAX_MOVE(victim);

   /* save the character */
   save_char_obj( victim, FALSE );

   /* completed message */
   send_to_char( AT_WHITE, "Ok.\n\r", ch );
}
/*
void do_nuke( CHAR_DATA *ch, char *argument )
{
 CHAR_DATA *vch;
 bool playing = FALSE;
  char arg[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA d;

  argument = one_argument( argument, arg );

 for ( vch = char_list; vch != NULL; vch = vch->next )
 {
   if ( vch->deleted )
     continue;
   if ( !IS_NPC( vch ) && !str_cmp( arg, vch->name ) )
   {
     if ( ch == vch )
     {
       send_to_char( AT_WHITE, "You cannot nuke yourself.\n\r", ch );
       return;
     }
     send_to_char( AT_WHITE, "Character is playing.\n\r", ch );
     playing = TRUE;
     break;
   }
 }

 if (!playing)
 { 
   vch = NULL;

   if ( !load_char_obj( &d, arg ) )
   {
     free_char( d.character );
     send_to_char( AT_WHITE, "Player does not exist.\n\r", ch );
     return;
   }
   vch = d.character;
   d.character = NULL;
   vch->desc = NULL;
   vch->next = char_list;
   char_list = vch;
   char_to_room( vch, ch->in_room );
   {
     char buf[MAX_INPUT_LENGTH];
     strcpy( buf, vch->name );
     free_string( vch->name );
     vch->name = str_dup( capitalize( buf ) );
     act( AT_RED, "$n ghost like form appears in the room.", vch, NULL, NULL, TO_ROOM );
   }
 }

 act( AT_RED, "A rift opens beneath $n, who falls into oblivion.", vch, NULL, NULL, TO_ROOM );
 act( AT_RED, "A rift opens beneath you, you fall into oblivion.", vch, NULL, NULL, TO_VICT );
 
 sprintf(log_buf, "%s%c/%s", PLAYER_DIR, LOWER(vch->name[0]),
     capitalize(vch->name));
 remove(log_buf);
 strcat(log_buf, ".fng");
 remove(log_buf);
 sprintf(log_buf, "%s%c/%s", PLAYER_DIR, LOWER(vch->name[0]),
     capitalize(vch->name));
 strcat(log_buf, ".cps");
 remove(log_buf);

 delete_playerlist( vch->name );

 d = vch->desc;
 extract_char(vch, TRUE);
 if ( d )
  close_socket(d);

 send_to_char( AT_WHITE, "Ok.\n\r", ch );

}

*/
