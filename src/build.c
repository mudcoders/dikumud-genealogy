/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *                      Rotains Builder System                             *
 *                                                                         *
 ***************************************************************************/


#if defined(macintosh)
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
#include "build.h"

const  struct  builder_cmd_type        builder_command_table       [] =
{
    { "redit",        do_redit,    4  },
    { "oedit",        do_oedit,    4  },
    { "medit",        do_medit,    4  },
    { "aedit",        do_aedit,    4  },
/*
 * Dummy Line (End Of Array Marker)
 */

    { "",                      0,    0 }
};

const  struct  builder_cmd_type       oedit_command_table       [] =
{
    { ".",        oedit_show,             4  },
    { "load",     oedit_load_object,      4  },
    { "show",     oedit_show,             4  },
    { "end",      builder_mode_end,       4  },
    { "create",   oedit_create_object,    4  },
    { "edit",     oedit_select,           4  },
    
/* 
 *  Stuff
 */
    { "weight",     oedit_weight,         4  },
    { "timmer",     oedit_timmer,         4  },
    { "affects",    oedit_affects,        4  },

/*
 * Dummy Line (End Of Array Marker)
 */

    { "",                           0,    0 }
};

bool builder_interpret( CHAR_DATA *ch, char *argument )
{ 
  char     command[MAX_INPUT_LENGTH];  
  int      cmd;
  int      trust;
  bool     located;

    /*
     * Builders Only
     */
    if (ch->level < 4)
       return( FALSE );

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
        argument++;
    if ( argument[0] == '\0' )
        return( FALSE );

   /*
     * Implement freeze command.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE) )
    {
        send_to_char( "You're totally frozen!\n\r", ch );
        return( FALSE ); 
    }

   /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
        command[0] = argument[0];
        command[1] = '\0';
        argument++;          
        while ( isspace(*argument) )
            argument++;
    }
    else
    {
        argument = one_argument( argument, command );
    }                                               

   located = FALSE;
   trust = get_trust( ch );
   for ( cmd = 0; builder_command_table[cmd].name[0] != '\0'; cmd++ )
   {
        if ( command[0] == builder_command_table[cmd].name[0]
        &&   !str_prefix( command, builder_command_table[cmd].name ) 
        &&    builder_command_table[cmd].level <= trust )
        {
            located = TRUE;
            break;
        }
   }

   if ( located == FALSE )
   {
      return( FALSE );
   }
   else
   {
     /* 
      * Execute Command
      */
     (*builder_command_table[cmd].do_fun) ( ch, argument );
      return( TRUE );
   }
}

void do_redit( CHAR_DATA *ch, char *argument )
{
  send_to_char("redit works\n\r", ch);
  return;
};

void do_aedit( CHAR_DATA *ch, char *argument )
{
  send_to_char("aedit works\n\r", ch);
  return;
};

void do_medit( CHAR_DATA *ch, char *argument )
{
  send_to_char("medit works\n\r", ch);
  return;
};

void do_oedit( CHAR_DATA *ch, char *argument )
{
   char     command[MAX_INPUT_LENGTH];  
   int      cmd;
   int      trust;
   bool     located;

   argument = one_argument( argument, command );
   located = FALSE;
   trust = get_trust( ch );
   for ( cmd = 0; oedit_command_table[cmd].name[0] != '\0'; cmd++ )
   {
        if ( command[0] == oedit_command_table[cmd].name[0]
        &&   !str_prefix( command, oedit_command_table[cmd].name ) 
        &&    oedit_command_table[cmd].level <= trust )
        {
            located = TRUE;
            break;
        }
   }

   if ( located == TRUE )
   {
     /* 
      * Execute Command
      */
     (*oedit_command_table[cmd].do_fun) ( ch, argument );
      return;
   }
   else
   {
      do_help( ch, "oedit" );
      return;
   }
};

void builder_mode_end( CHAR_DATA *ch, char *argument ) 
{

  if ( ch->builder_mode == MODE_REDIT )
  {
     send_to_char("Room Editor Mode Finished.\n\r", ch);
     ch->builder_mode = MODE_NONE;
     ch->builder_vnum = MODE_NONE;
     return;
  };
  if ( ch->builder_mode == MODE_MEDIT )
  {
     send_to_char("Mobile Editor Mode Finished.\n\r", ch);
     ch->builder_mode = MODE_NONE;
     ch->builder_vnum = MODE_NONE;
     return;
  };
  if ( ch->builder_mode == MODE_OEDIT )
  {
     send_to_char("Object Editor Mode Finished.\n\r", ch);
     ch->builder_mode = MODE_NONE;
     ch->builder_vnum = MODE_NONE;
     return;
  };
  if ( ch->builder_mode == MODE_AEDIT )
  {
     send_to_char("Room Editor Mode Finished.\n\r", ch);
     ch->builder_mode = MODE_NONE;
     ch->builder_vnum = MODE_NONE;
     return;
  };
  if ( ch->builder_mode >= MODE_MAX )
  {
     send_to_char("WARNING: Your builder mode could not been determind!\n\r", ch);
     send_to_char("WARNING: Now resting you builder information to default!\n\r", ch);
     ch->builder_mode = MODE_NONE;
     ch->builder_vnum = MODE_NONE;
     return;
  } 
  send_to_char("You are not in a builder mode!\n\r", ch);  
  return;
};

/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_vnum( CHAR_DATA *ch, int argument )
{
    OBJ_DATA *obj;

    for ( obj = object_list; obj != NULL; obj = obj->next )
        if ( can_see_obj( ch, obj ) && argument == obj->pIndexData->vnum )
                return obj;
    return NULL;
}    


