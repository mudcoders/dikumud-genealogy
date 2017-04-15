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
*	EotS 1.3 is copyright 1993-1996 Eric Orvedal and Nathan Axtman         *
*                                                                          *
*	EotS has been brought to you by us, the merry drunk mudders	           *
*	    Kjodo                                                              *
*	    Torann                                                             *
*	    Sledge                                                             *
*	    Nicodemous                                                         *
*	    Tom                                                                *
*	By using this code, you have agreed to follow the terms of the         *
*	blood oath of the carungatang                                          *
*                                                                          *
*              EotS Random object functions                                *
*       I think you will enjoy :)   (Ugh...935 lines [was 3000+ :)] )       *
*                                                                          *
***************************************************************************/


#define unix 1
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void random_affect args(( OBJ_DATA *obj, char *buf, int total ));

OBJ_DATA  *random_object( int level )
{
	OBJ_DATA       *obj;
    OBJ_INDEX_DATA *pObjIndex;
	char           buf[MAX_STRING_LENGTH];
	bool singlename = TRUE;
	bool lowhigh = FALSE;
	char           long_desc[MAX_STRING_LENGTH];
	int  lowdam = 0;
	int  highdam = 0;

	if ((number_percent() + level) > 90 )
		lowhigh = TRUE;

	if ( level > 100 )
		level = 100;

			pObjIndex = get_obj_index(OBJ_VNUM_DUMMY);
			obj = create_object( pObjIndex, level );

          switch( dice( 1, 6 ) )
          {
           case 1:
		   case 2:
		   case 3:
		   case 4:
		   case 5:
			   obj->item_type = ITEM_ARMOR;
            switch( dice( 1, 18 ) )
            {
             case 1: /* finger */
		       obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_FINGER;
			   sprintf( buf, "Ring" );
				break;
             case 2: /* neck */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_NECK;
			   switch( dice( 1,3))
			   {
			   case 1:
				   sprintf( buf, "Necklace" ); break;
			   case 2:
				   sprintf( buf, "Collar" ); break;
			   case 3:
				   sprintf( buf, "Talisman" ); break;
			   } break;
             case 3: /* body */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_BODY;
			   switch( dice( 1,3))
			   {
			   case 1:
				   sprintf( buf, "Armor" ); break;
			   case 2:
				   sprintf( buf, "Shirt" ); break;
			   case 3:
				   sprintf( buf, "Breastplate" ); break;
			   } break;
             case 4: /* head */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_HEAD;
			   switch( dice( 1,3))
			   {
			   case 1:
				   sprintf( buf, "Hat" ); break;
			   case 2:
				   sprintf( buf, "Helmet" ); break;
			   case 3:
				   sprintf( buf, "Helm" ); break;
			   } break;
             case 5: /* legs */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_LEGS;
			   	switch( dice( 1,3))
			   {
			   case 1:
				   sprintf( buf, "Leggings" ); break;
			   case 2:
				   sprintf( buf, "Pants" ); break;
			   case 3:
				   sprintf( buf, "Kilt" ); break;
			   } break;
             case 6: /* feet */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_FEET;
			   switch( dice( 1,2))
			   {
			   case 1:
				   sprintf( buf, "Shoes" ); break;
			   case 2:
				   sprintf( buf, "Boots" ); break;
			   } break;
			 case 7: /* hands */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_HANDS;
			   switch( dice( 1,2))
			   {
			   case 1:
				   sprintf( buf, "Gloves" ); break;
			   case 2:
				   sprintf( buf, "Gauntlets" ); break;
			   } break;
			 case 8: /* arm */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_ARMS;
			   switch( dice( 1,2))
			   {
			   case 1:
				   sprintf( buf, "Sleeves" ); break;
			   case 2:
				   sprintf( buf, "Vambrace" ); break;
			   } break;
			 case 9: /* sheild */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_SHIELD;
			   sprintf( buf, "Shield" );
				 break;
			 case 10: /* about */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_ABOUT;
			   switch( dice( 1,3))
			   {
			   case 1:
				   sprintf( buf, "Robe" ); break;
			   case 2:
				   sprintf( buf, "Cloak" ); break;
			   case 3:
				   sprintf( buf, "Cape" ); break;
			   } break;
			 case 11: /* waist */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_WAIST;
			   switch( dice( 1,3))
			   {
			   case 1:
				   sprintf( buf, "Belt" ); break;
			   case 2:
				   sprintf( buf, "Sash" ); break;
			   case 3:
				   sprintf( buf, "Cord" ); break;
			   } break;
			 case 12: /* wrist */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_WRIST;
			   switch( dice( 1,2))
			   {
			   case 1:
				   sprintf( buf, "Bracer" ); break;
			   case 2:
				   sprintf( buf, "Bracelet" ); break;
			   } break;
			 case 13: /* hold */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_HOLD;
				   sprintf( buf, "Sceptre" );
				 break;
			 case 14: /* orbit */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_ORBIT;
			   switch( dice( 1,2))
			   {
			   case 1:
				   sprintf( buf, "Orb" ); break;
			   case 2:
				   sprintf( buf, "Faerie" ); break;
			   } break;
			 case 15: /* face */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_FACE;
			   switch( dice( 1,2))
			   {
			   case 1:
				   sprintf( buf, "Mask" ); break;
			   case 2:
				   sprintf( buf, "Veil" ); break;
			   } break;
			 case 16: /* contact */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_CONTACT;
			   switch( dice( 1,3))
			   {
			   case 1:
				   sprintf( buf, "Tears" ); break;
			   case 2:
				   sprintf( buf, "Eyes" ); break;
			   case 3:
				   sprintf( buf, "Lenses" ); break;
			   } break;
			 case 17: /* ears */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_EARS;
				   sprintf( buf, "Collar" );
				 break;
			 case 18: /* ankle */
			   obj->wear_flags ^= ITEM_TAKE;
		       obj->wear_flags ^= ITEM_WEAR_ANKLE;
			   switch( dice( 1,2))
			   {
			   case 1:
				   sprintf( buf, "Anklet" ); break;
			   case 2:
				   sprintf( buf, "Stocking" ); break;
			   } break;
            } break;
           case 6:
				   obj->item_type = ITEM_WEAPON;
				   lowdam = number_range( 1, level );
				   highdam = number_range(lowdam, level + 5 );
				   obj->value[1] = lowdam;
				   obj->value[2] = highdam;
				switch( dice( 1, 5 ) )
				{
				case 1: /* bash */
				   obj->wear_flags ^= ITEM_TAKE;
				   obj->wear_flags ^= ITEM_WIELD;
				   obj->value[3] = 8;
				   switch( dice( 1,2))
				   {
				   case 1:
					   sprintf( buf, "Hammer" ); break;
				   case 2:
					   sprintf( buf, "Mace" ); break;
				   } break;
				case 2: /* pierce */
				   obj->wear_flags ^= ITEM_TAKE;
				   obj->wear_flags ^= ITEM_WIELD;
				   obj->value[3] = 11;
				   switch( dice( 1,2))
				   {
				   case 1:
					   sprintf( buf, "Dagger" ); break;
				   case 2:
					   sprintf( buf, "Knife" ); break;
				   } break;
				case 3: /* slash */
				   obj->wear_flags ^= ITEM_TAKE;
				   obj->wear_flags ^= ITEM_WIELD;
				   obj->value[3] = 3;
				   switch( dice( 1,2))
				   {
				   case 1:
					   sprintf( buf, "Sword" ); break;
				   case 2:
					   sprintf( buf, "Long Sword" ); break;
				   } break;
				case 4: /* pierce */
				   obj->wear_flags ^= ITEM_TAKE;
				   obj->wear_flags ^= ITEM_WIELD;
				   obj->value[3] = 13;
				   switch( dice( 1,2))
				   {
				   case 1:
					   sprintf( buf, "Axe" ); break;
				   case 2:
					   sprintf( buf, "Battle Axe" ); break;
				   } break;
				case 5: /* pierce */
				   obj->wear_flags ^= ITEM_TAKE;
				   obj->wear_flags ^= ITEM_WIELD;
				   obj->value[3] = 4;
				   switch( dice( 1,2))
				   {
				   case 1:
					   sprintf( buf, "Whip" ); break;
				   case 2:
					   sprintf( buf, "Bull Whip" ); break;
				   } break;

				} break;
		  } /* big switch */
		if ( lowhigh )
		  switch ( dice( 1, 10 ) )
		  {
		  case 1:
		  case 2:
		  case 3:
		  case 4:
		  case 5: /* no applies */
			  break;
		  case 6:
			  singlename = FALSE;
			  random_affect( obj, buf, 1); break;
		  case 7:
			  singlename = FALSE;
			  random_affect(obj, buf, 2); break;
		  case 8:
			  singlename = FALSE;
			  random_affect(obj, buf, 3); break;
		  case 9:
			  singlename = FALSE;
			  random_affect(obj, buf, 4); break;
		  case 10:
			  singlename = FALSE;
			  random_affect(obj, buf, 5); break;
		  } 
		  if ( singlename )
		  {
			sprintf( long_desc, "A %s has been left here.", buf );
			obj->name        = str_dup( buf );
			obj->short_descr = str_dup( buf );
			obj->description = str_dup( long_desc );
		  }
	obj->level = level;

return obj;
}

void random_affect( OBJ_DATA *obj, char *buf, int total )
{
	char           long_desc[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
	char name[MAX_STRING_LENGTH];
	int number;


			for ( number = 0; number < total; number++ )
			{
				switch( dice( 1, 43 ) )
				{
				case 1: /* saving-rod */
					paf = new_affect();
					paf->location = flag_value( apply_flags, "saving-rod" );
					paf->modifier = number_range( -50, 50 );
					paf->type     = skill_lookup("saving-rod");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Magical Rod Defense", buf );
					break;
				case 2: /* saving-petri */
					paf = new_affect();
					paf->location = flag_value( apply_flags, "saving-petri" );
					paf->modifier = number_range( -50, 50 );
					paf->type     = skill_lookup("saving-petri");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Magical Petrification Defense", buf );
					break;
				case 3: /* saving-breath */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"saving-breath");
					paf->modifier = number_range( -50, 50 );
					paf->type     = skill_lookup("saving-breath");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Magical Breath Defense", buf );
					break;
				case 4: /* saving-spell */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"saving-spell");
					paf->modifier = number_range( -50, 50 );
					paf->type     = skill_lookup("saving-spell");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Magical Spell Defense", buf );
					break;
				case 5: /* invis */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"invis");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("invis");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Invisibility", buf );
					break;
				case 6: /* detect-invis */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"detect-invis");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("detect-invis");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Detect Invisible", buf );
					break;
				case 7: /* hide */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"hide");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("hide");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Hiding", buf );
					break;
				case 8: /* sneak */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"sneak");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("sneak");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Sneaking", buf );
					break;
				case 9: /* scry */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"scry");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("scry");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Scrying", buf );
					break;
				case 10: /* detect-hide */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"detect-hide");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("detect-hide");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Detect Hidden", buf );
					break;
				case 11: /* detect-magic */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"detect-magic");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("detect-magic");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Magic Detection", buf );
					break;
				case 12: /* detect-evil */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"detect-evil");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("detect-evil");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Detect Evil", buf );
					break;
				case 13: /* protect */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"protect");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("protect");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Protection", buf );
					break;
				case 14: /* fly */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"fly");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("fly");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Flight", buf );
					break;
				case 15: /* infrared */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"infrared");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("infrared");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Infravision", buf );
					break;
				case 16: /* sanctuary */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"sanctuary");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("sanctuary");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Sanctuary", buf );
					break;
				case 17: /* pass-door */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"pass-door");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("pass-door");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Pass Door", buf );
					break;
				case 18: /* haste */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"haste");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("haste");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Speed", buf );
					break;
				case 19: /* bless */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"bless");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("bless");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Bless", buf );
					break;
				case 20: /* poison */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"poison");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("poison");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Venom", buf );
					break;
				case 21: /* faerie-fire */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"faerie-fire");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("faerie-fire");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Faerie Fire", buf );
					break;
				case 22: /* giant-str */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"giant-str");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("giant-str");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Giant Strength", buf );
					break;
				case 23: /* strength */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"strength");
					paf->modifier = number_range( -10, 10 );
					paf->type     = skill_lookup("strength");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					break;
					sprintf( name, "%s of Strength", buf );
				case 24: /* dexterity */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"dexterity");
					paf->modifier = number_range( -10, 10 );
					paf->type     = skill_lookup("dexterity");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Dexterity", buf );
					break;
				case 25: /* intelligence */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"intelligence");
					paf->modifier = number_range( -10, 10 );
					paf->type     = skill_lookup("intelligence");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Intelligence", buf );
					break;
				case 26: /* wisdom */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"wisdom");
					paf->modifier = number_range( -10, 10 );
					paf->type     = skill_lookup("wisdom");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Wisdom", buf );
					break;
				case 27: /* constitution */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"constitution");
					paf->modifier = number_range( -10, 10 );
					paf->type     = skill_lookup("constitution");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Constitution", buf );
					break;
				case 28: /* sex */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"sex");
					paf->modifier = number_range( -1, 1 );
					paf->type     = skill_lookup("sex");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Change Sex", buf );
					break;
				case 29: /* age */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"age");
					paf->modifier = number_range( -10, 100 );
					paf->type     = skill_lookup("age");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Aging", buf );
					break;
				case 30: /* height */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"height");
					paf->modifier = number_range( -3, 10 );
					paf->type     = skill_lookup("height");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Size", buf );
					break;
				case 31: /* weight */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"weight");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("weight");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Encumbrance", buf );
					break;
				case 32: /* mana */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"mana");
					paf->modifier = number_range( -200, 200 );
					paf->type     = skill_lookup("mana");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Magical Energy", buf );
					break;
				case 33: /* blood */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"blood");
					paf->modifier = number_range( -200, 200 );
					paf->type     = skill_lookup("blood");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Blood", buf );
					break;
				case 34: /* anti-disarm */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"anti-disarm");
					paf->modifier = number_range( -100, 100 );
					paf->type     = skill_lookup("anti-disarm");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Grip", buf );
					break;
				case 35: /* hp */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"hp");
					paf->modifier = number_range( -200, 200 );
					paf->type     = skill_lookup("hp");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Life", buf );
					break;
				case 36: /* move */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"move");
					paf->modifier = number_range( -200, 200 );
					paf->type     = skill_lookup("move");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Endurance", buf );
					break;
				case 37: /* ac */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"ac");
					paf->modifier = number_range( -200, 200 );
					paf->type     = skill_lookup("ac");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Armoring", buf );
					break;
				case 38: /* hitroll */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"hitroll");
					paf->modifier = number_range( -200, 200 );
					paf->type     = skill_lookup("hitroll");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Accuracy", buf );
					break;
				case 39: /* damroll */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"damroll");
					paf->modifier = number_range( -200, 200 );
					paf->type     = skill_lookup("damroll");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of the Brute", buf );
					break;
				case 40: /* saving-para */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"saving-para");
					paf->modifier = number_range( -20, 20 );
					paf->type     = skill_lookup("saving-para");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Magical Paralysis Defense", buf );
					break;
				case 41: /* heighten-senses */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"heighten-senses");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("heighten-senses");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Heighten Senses", buf );
					break;
				case 42: /* combat-mind */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"combat-mind");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("combat-mind");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Combat", buf );
					break;
				case 43: /* bio-acceleration */
					paf = new_affect();
					paf->location = flag_value( apply_flags,"bio-acceleration");
					paf->modifier = number_range( -1, 100 );
					paf->type     = skill_lookup("bio-acceleration");
					if ( paf->type < 0 )
						 paf->type = 0;
					paf->duration = -1;
					paf->bitvector = 0;
					paf->next = obj->affected;
					obj->affected = paf;
					sprintf( name, "%s of Psionic Defense", buf );
					break;
				} /* aff switch */
			}
			sprintf( long_desc, "A %s has been left here.", name );
			obj->name        = str_dup( name );
			obj->short_descr = str_dup( name );
			obj->description = str_dup( long_desc );

return;
}
