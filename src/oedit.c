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

OBJ_INDEX_DATA *        obj_index_hash          [MAX_KEY_HASH];
int                     top_obj_index;


void oedit_create_object(CHAR_DATA *ch, char *argument) 
{
     OBJ_INDEX_DATA *pObjIndex; 

     int  vnum=0;
     int iHash;    
     char vnum_store[MAX_INPUT_LENGTH];
     
     argument = one_argument( argument, vnum_store ); 

     if ((is_number(vnum_store))==FALSE)
     {
        send_to_char("Syntax : oedit create <vnum>\n\r", ch);
        return;
     };
 
     vnum = atoi( vnum_store ); 

     if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )   
     {
        send_to_char( "Error : An item with that vnum already exists.\n\r", ch );
        return;
     };                           

     pObjIndex                       = alloc_perm( sizeof(*pObjIndex) );
     pObjIndex->vnum                 = vnum;
     pObjIndex->name                 = str_dup("ball of prot-plasm");
     pObjIndex->short_descr          = str_dup("ball of prot-plasm");
     pObjIndex->description          = str_dup("ball of prot-plasm.");
     /* Action description            default setting;*/

     pObjIndex->short_descr[0]       = LOWER(pObjIndex->short_descr[0]);
     pObjIndex->description[0]       = UPPER(pObjIndex->description[0]);

     pObjIndex->item_type            = ITEM_TRASH;  
     pObjIndex->extra_flags          = 0;
     pObjIndex->wear_flags           = -1;
     pObjIndex->value[0]             = 0;
     pObjIndex->value[1]             = 0;
     pObjIndex->value[2]             = 0;
     pObjIndex->value[3]             = 0;
     pObjIndex->weight               = 1;
     pObjIndex->cost                 = 1;   /* Unused */      

    iHash                   = vnum % MAX_KEY_HASH;
    pObjIndex->next         = obj_index_hash[iHash];
    obj_index_hash[iHash]   = pObjIndex;
    top_obj_index++;                         

    ch->builder_mode   = MODE_OEDIT;
    ch->builder_vnum   = vnum;  
    oedit_load(ch, vnum);
    send_to_char("Object Editor Mode Started.\n\rObject Created.\n\r", ch);
    return;
};

void oedit_show( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;  

    one_argument( argument, arg );

    if ( ch->builder_mode == MODE_OEDIT && arg[0] == '\0' )
    {
        if ( ( pObjIndex = get_obj_index( ch->builder_vnum ) ) == NULL )
        {
          send_to_char( "OEDIT: No object has that vnum.\n\r", ch );
          return;
        }
        if ( ( obj = get_obj_vnum( ch, pObjIndex->vnum ) ) == NULL )
        {  
          send_to_char( "OEDIT: that item is currently not load in the game.\n\r", ch );
          return;
        }
    }
    else
    {
       if (arg[0] == '\0' )
       {
          send_to_char( "Show which object?\n\r", ch );
          return;
       }
       if ( is_number( arg) == TRUE )
       {
           if ( ( obj = get_obj_vnum( ch, atoi( arg ) ) ) == NULL )
           {
              send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
              return;
           }
       }
       else
       {
           if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
           {
              send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
              return;
           }
       }    
    }

    sprintf( buf, "Name: %s.\n\r",obj->name );
    send_to_char( buf, ch );

    sprintf( buf, "Vnum: %d.  Type: %s.\n\r", obj->pIndexData->vnum,
                                              item_type_name( obj ) );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s.\n\rLong description: %s\n\r",
        obj->short_descr, obj->description );
    send_to_char( buf, ch );

    sprintf( buf, "Wear bits: %d.  Extra bits: %s.\n\r",
        obj->wear_flags, extra_bit_name( obj->extra_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Number: %d/%d.  Weight: %d/%d.\n\r",
        1,           get_obj_number( obj ),
        obj->weight, get_obj_weight( obj ) );
    send_to_char( buf, ch );        

    sprintf( buf, "Cost: %d.  Timer: %d.  Level: %d.\n\r",
        obj->cost, obj->timer, obj->level );
    send_to_char( buf, ch );

    sprintf( buf,
        "In room: %d.  In object: %s.  Carried by: %s.  Wear_loc: %d.\n\r",
        obj->in_room    == NULL    ?        0 : obj->in_room->vnum,
        obj->in_obj     == NULL    ? "(none)" : obj->in_obj->short_descr,
        obj->carried_by == NULL    ? "(none)" : obj->carried_by->name, 
        obj->wear_loc );
    send_to_char( buf, ch );

    sprintf( buf, "Values: %d %d %d %d.\n\r",
        obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
    send_to_char( buf, ch );

    if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
    {
        EXTRA_DESCR_DATA *ed;                  

        send_to_char( "Extra description keywords: '", ch );

        for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
        {
            send_to_char( ed->keyword, ch );
            if ( ed->next != NULL )
                send_to_char( " ", ch );
        }

        for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
        {
            send_to_char( ed->keyword, ch );
            if ( ed->next != NULL )
                send_to_char( " ", ch );
        }

        send_to_char( "'.\n\r", ch );
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
        sprintf( buf, "Affects %s by %d.\n\r",
            affect_loc_name( paf->location ), paf->modifier );
        send_to_char( buf, ch );
    }

    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
        sprintf( buf, "Affects %s by %d.\n\r",   
            affect_loc_name( paf->location ), paf->modifier );
        send_to_char( buf, ch );
    }

    return;                    
}

void oedit_load( CHAR_DATA *ch, int argument )
{
   OBJ_INDEX_DATA *pObjIndex;
   OBJ_DATA *obj;

    if ( ( pObjIndex = get_obj_index( argument ) ) == NULL )   
    {
        send_to_char("OEDIT : ERROR it is not possible to create this item. \n\r",ch);
        return;
    }

    obj = create_object( pObjIndex, 1 ); /* chage this to the mortal level :) */
    if ( CAN_WEAR(obj, ITEM_TAKE) )
    {
        obj_to_char( obj, ch );
    }                  
    else
    {
        obj_to_room( obj, ch->in_room );
        act( "$n has created $p!", ch, obj, NULL, TO_ROOM );
    }
    return;
};

void oedit_load_object( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex; 
    int vnum;

    argument = one_argument( argument, arg ); 

    if (!is_number(arg))
    {
       send_to_char("Syntax : oedit load <vnum>\n\r", ch);
       return;
    };

    vnum = atoi( arg );

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
      send_to_char( "OEDIT : No object with that vnum exists.\n\r", ch); 
      return;
    };           

    oedit_load(ch, vnum);
    send_to_char("Object Loaded.\n\r", ch);
    return;
};

void oedit_weight( CHAR_DATA *ch, char *argument )
{
   char arg1 [MAX_INPUT_LENGTH];
   int  value;
   OBJ_DATA *obj; 
   OBJ_INDEX_DATA *pObjIndex;  

   argument = one_argument( argument, arg1 );
   value    = atoi( arg1 );

   if ( ch->builder_mode != MODE_OEDIT )
   {
      send_to_char("OEDIT: But you are not editing an object?\n\r", ch);
      return;
   }
   if ( is_number( arg1 ) )
   {
      if ( ( pObjIndex = get_obj_index( ch->builder_vnum ) ) == NULL )
      {
        send_to_char( "OEDIT: No object has that vnum.\n\r", ch );
        return;
      }
      if ( ( obj = get_obj_vnum( ch, ch->builder_vnum/* pObjIndex->vnum */ ) ) == NULL )
      {  
        send_to_char( "OEDIT: that item is currently not load in the game.\n\r", ch );
        return;
      }
      obj->weight       = value;
      pObjIndex->weight = value;
      send_to_char("Weight Changed.\n\r", ch);
      return;
   }
   else
   {
      send_to_char("Syntax : oedit weight <value>\n\r", ch);
      return;
   }   
   return;
};

void oedit_timmer( CHAR_DATA *ch, char *argument )
{
   char arg1 [MAX_INPUT_LENGTH];
   int  value;
   OBJ_DATA *obj; 
   OBJ_INDEX_DATA *pObjIndex;  

   argument = one_argument( argument, arg1 );
   value    = atoi( arg1 );

   if ( ch->builder_mode != MODE_OEDIT )
   {
      send_to_char("OEDIT: But you are not editing an object?\n\r", ch);
      return;
   }
   if ( is_number( arg1 ) )
   {
      if ( ( pObjIndex = get_obj_index( ch->builder_vnum ) ) == NULL )
      {
        send_to_char( "OEDIT: No object has that vnum.\n\r", ch );
        return;
      }
      if ( ( obj = get_obj_vnum( ch, pObjIndex->vnum ) ) == NULL )
      {  
        send_to_char( "OEDIT: that item is currently not load in the game.\n\r", ch );
        return;
      }
      obj->timer        = value;
      pObjIndex->count  = value;
      send_to_char("Timmer Changed.\n\r", ch);
      return;
   }
   else
   {
      send_to_char("Syntax : oedit timmer <value>\n\r", ch);
      return;
   }   
   return;
};

void oedit_select( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

     if (arg[0] == '\0' )
     {
        send_to_char( "OEDIT: Edit which object?\n\r", ch );
        return;
     }

     if ( is_number( arg ) )
     {
         if ( ( obj = get_obj_vnum( ch, atoi( arg ) ) ) == NULL )
         {
            send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch);
            return;
         }
     }
     else
     { 
        if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
        {
            send_to_char( "OEDIT: That object is not currently in the game.\n\r", ch);
            return;
        }
     }
     ch->builder_mode = MODE_OEDIT;
     ch->builder_vnum=  atoi( arg );
     send_to_char("Object Editor Mode Started.\n\rObject Selected.\n\r", ch);
     return;
}

void oedit_affects( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   OBJ_INDEX_DATA *pObjIndex; 

    one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( ch->builder_mode != MODE_OEDIT || arg2[0] == '\0' )
    {
         send_to_char( "OEDIT: But you are not editing an object ?\n\r", ch );
         return;
    }

    if ( ( pObjIndex = get_obj_index( ch->builder_vnum ) ) == NULL )
    {
         send_to_char( "OEDIT: No object has that vnum.\n\r", ch );
         return;
    }
    if ( ( obj = get_obj_vnum( ch, pObjIndex->vnum ) ) == NULL )
    {
         send_to_char( "OEDIT: that item is currently not load in the game.\n\r", ch);
         return;                     
    }

    if (arg1[0] == '\0' )
    {
        send_to_char( "Syntax: oedit affect <field> <value>\n\r", ch );
        send_to_char( "Where Field Is :\n\r", ch );
        send_to_char( "str dex wis con int mana move ac hitroll damroll\n\r", ch );
        send_to_char( "closeable closed locked pickproof\n\r", ch );
        return;               
    }
  
     if ( !str_cmp( arg1 , "str" ) )
     {
         if (!is_number( arg2 )) /* logic statement wrong ??? */
         {
              AFFECT_DATA *paf;

              for ( paf = obj->affected; paf != NULL; paf = paf->next )
              {
                 send_to_char("Object affects: - \n\r", ch);
                 sprintf(buf, "type : %d\n\r", paf->type);
                 send_to_char(buf, ch);
                 sprintf(buf, "duration : %d\n\r", paf->duration);
                 send_to_char(buf, ch);
                 sprintf(buf, "location : %d\n\r", paf->location);
                 send_to_char(buf, ch);
                 sprintf(buf, "modifier : %d\n\r", paf->modifier);
                 send_to_char(buf, ch);
                 return;
              }
/*              paf                     = alloc_perm( sizeof(*paf) );
              paf->type               = -1;
              paf->duration           = -1;
              paf->location           = ;
              paf->modifier           = ;
              paf->bitvector          = 0;
              paf->next               = pObjIndex->affected;
              pObjIndex->affected     = paf;*/
         }
         else
         {
            send_to_char( "Syntax: oedit affect str <value>\n\r", ch );
            return;
         }
     }
     return;
}

/*
 * Return ascii name of extra flags vector.
 
char *extra_bit_name( int extra_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( extra_flags & ITEM_GLOW         ) strcat( buf, " glow"         );
    if ( extra_flags & ITEM_HUM          ) strcat( buf, " hum"          );
    if ( extra_flags & ITEM_DARK         ) strcat( buf, " dark"         );
    if ( extra_flags & ITEM_LOCK         ) strcat( buf, " lock"         );
    if ( extra_flags & ITEM_EVIL         ) strcat( buf, " evil"         );   
    if ( extra_flags & ITEM_INVIS        ) strcat( buf, " invis"        );
    if ( extra_flags & ITEM_MAGIC        ) strcat( buf, " magic"        );
    if ( extra_flags & ITEM_NODROP       ) strcat( buf, " nodrop"       );
    if ( extra_flags & ITEM_BLESS        ) strcat( buf, " bless"        );
    if ( extra_flags & ITEM_ANTI_GOOD    ) strcat( buf, " anti-good"    );
    if ( extra_flags & ITEM_ANTI_EVIL    ) strcat( buf, " anti-evil"    );
    if ( extra_flags & ITEM_ANTI_NEUTRAL ) strcat( buf, " anti-neutral" );
    if ( extra_flags & ITEM_NOREMOVE     ) strcat( buf, " noremove"     );
    if ( extra_flags & ITEM_INVENTORY    ) strcat( buf, " inventory"    );
    return ( buf[0] != '\0' ) ? buf+1 : "none"; 
}
*/
