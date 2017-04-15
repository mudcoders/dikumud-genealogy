/***************************************************************************
 *  file: save.c, Database module.                        Part of DIKUMUD  *
 *  Usage: Saving and loading of characters                                *
 *  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
 *                                                                         *
 *  Copyright (C) 1992, 1993 Michael Chastain, Michael Quan, Mitchell Tse  *
 *  Rewritten by MERC Industries, based on crash.c by prometheus           *
 *  (Taquin Ho) and abaddon (Jeff Stile).                                  *
 *  You can use our stuff in any way you like whatsoever so long as this   *
 *  copyright notice remains intact.  If you like it please drop a line    *
 *  to mec@garnet.berkeley.edu.                                            *
 *                                                                         *
 *  This is free software and you are benefitting.  We hope that you       *
 *  share your changes too.  What goes around, comes around.               *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "utils.h"
#include "db.h"
#include "handler.h"
#include "spells.h"

extern struct index_data *obj_index;
extern struct room_data *world;

void obj_store_to_char(struct char_data *ch, struct obj_file_elem *object);
bool put_obj_in_store(struct obj_data *obj, struct char_data *ch,
    FILE *fpsave);
void restore_weight(struct obj_data *obj);
void store_to_char(struct char_file_u *st, struct char_data *ch);
void char_to_store(struct char_data *ch, struct char_file_u *st);



/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes.
 */
void save_char_obj( struct char_data *ch )
{
    struct  char_file_u uchar;
    FILE *  fpsave  = NULL;
    char    strsave[MAX_INPUT_LENGTH];
    int     iWear;

    if ( IS_NPC(ch) || GET_LEVEL(ch) < 2 )
	goto LSuccess;

    sprintf( strsave, "%s/%s", SAVE_DIR, ch->player.name );
    if ( ( fpsave = fopen( strsave, "wb" ) ) == NULL )
	goto LError;

    char_to_store( ch, &uchar );
    if ( ch->in_room < 2 )
	uchar.load_room = world[ch->specials.was_in_room].number;
    else
	uchar.load_room = world[ch->in_room].number;

    if ( fwrite( &uchar, sizeof(uchar), 1, fpsave ) == 0 )
	goto LError;

    if ( !obj_to_store( ch->carrying, ch, fpsave ) )
	goto LError;

    restore_weight( ch->carrying );
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ch->equipment[iWear] )
	{
	    if ( !obj_to_store( ch->equipment[iWear], ch, fpsave ) )
		goto LError;
	    restore_weight( ch->equipment[iWear] );
	}
    }
    goto LSuccess;

 LError:
    sprintf( log_buf, "Save_char_obj: %s", strsave );
    log( log_buf );

 LSuccess:
    if ( fpsave != NULL )
	fclose( fpsave );
    return;
}



/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj( struct descriptor_data *d, char *name )
{
    FILE *  fpsave  = NULL;
    char    strsave[MAX_INPUT_LENGTH];
    struct  char_file_u uchar;
    struct  char_data *ch;

    CREATE( ch, struct char_data, 1 );
    d->character    = ch;
    clear_char( ch );
    ch->desc        = d;

    sprintf( strsave, "%s/%s", SAVE_DIR, name );
    if ( ( fpsave = fopen( strsave, "rb" ) ) == NULL )
	return FALSE;

    if ( fread( &uchar, sizeof(uchar), 1, fpsave ) == 0 )
	goto LError;
    reset_char( ch );
    GET_NAME(ch) = str_dup(name);
    store_to_char( &uchar, ch );
    if ( world[ch->in_room].zone != world[real_room(3001)].zone )
	gain_exp(ch, 0 - MIN(GET_EXP(ch)/2, 10*GET_LEVEL(ch)*GET_LEVEL(ch) ));

    while ( !feof( fpsave ) )
    {
	struct  obj_file_elem   object;

	fread( &object, sizeof(object), 1, fpsave );
	if ( ferror( fpsave ) )
	    goto LError;
	if ( feof( fpsave ) )
	    break;

	obj_store_to_char( ch, &object );
    }
    goto LSuccess;

 LError:
    sprintf( log_buf, "Load_char_obj: %s", strsave );
    log( log_buf );
    if ( fpsave != NULL )
	fclose( fpsave );
    return FALSE;

 LSuccess:
    if ( fpsave != NULL )
	fclose( fpsave );
    return TRUE;
}



void obj_store_to_char(struct char_data *ch, struct obj_file_elem *object)
{
    struct obj_data *obj;
    int j;
    int nr;

    void obj_to_char(struct obj_data *object, struct char_data *ch);

    if ( ( nr = real_object(object->item_number) ) > -1 ) 
    {
	obj = read_object( nr, 0 );
	obj->obj_flags.value[0] = object->value[0];
	obj->obj_flags.value[1] = object->value[1];
	obj->obj_flags.value[2] = object->value[2];
	obj->obj_flags.value[3] = object->value[3];

	obj->obj_flags.extra_flags = object->extra_flags;
	obj->obj_flags.weight      = object->weight;
	obj->obj_flags.timer       = object->timer;
	obj->obj_flags.eq_level    = object->eq_level;
	obj->obj_flags.bitvector   = object->bitvector;

	for(j=0; j<MAX_OBJ_AFFECT; j++)
	    obj->affected[j] = object->affected[j];

	obj_to_char(obj, ch);
    }
}



bool obj_to_store( struct obj_data *obj, struct char_data *ch, FILE *fpsave )
{
    struct obj_data *tmp;

    if ( obj == NULL )
	return TRUE;

    /* Write depth first (so weights come out right) */
    if ( !obj_to_store( obj->contains, ch, fpsave ) )
	return FALSE;
    if ( !obj_to_store( obj->next_content, ch, fpsave) )
	return FALSE;
    if ( !put_obj_in_store( obj, ch, fpsave ) )
	return FALSE;

    /* Adjust container weights of up-linked items */
    for ( tmp = obj->in_obj; tmp; tmp = tmp->in_obj )
	GET_OBJ_WEIGHT(tmp) -= GET_OBJ_WEIGHT(obj);

    return TRUE;
}





/*
 * Write one object to the file.
 */
bool put_obj_in_store(struct obj_data *obj, struct char_data *ch, FILE *fpsave)
{
    int iAffect;
    struct  obj_file_elem   object;

    if ( GET_ITEM_TYPE(obj) == ITEM_KEY )
	return TRUE;

    object.version	= 0;
    object.item_number  = obj_index[obj->item_number].virtual;
    object.value[0]     = obj->obj_flags.value[0];
    object.value[1]     = obj->obj_flags.value[1];
    object.value[2]     = obj->obj_flags.value[2];
    object.value[3]     = obj->obj_flags.value[3];
    object.extra_flags  = obj->obj_flags.extra_flags;
    object.weight       = obj->obj_flags.weight;
    object.timer        = obj->obj_flags.timer;
    object.eq_level	= obj->obj_flags.eq_level;
    object.bitvector    = obj->obj_flags.bitvector;
    for ( iAffect = 0; iAffect < MAX_OBJ_AFFECT; iAffect++ )
	object.affected[iAffect]    = obj->affected[iAffect];
    
    if ( fwrite( &object, sizeof(object), 1, fpsave ) == 0 )
	return FALSE;

    return TRUE;
}



/*
 * Restore container weights after a save.
 */
void restore_weight(struct obj_data *obj)
{
    struct obj_data *tmp;

    if ( obj == NULL )
	return;

    restore_weight( obj->contains );
    restore_weight( obj->next_content );
    for ( tmp = obj->in_obj; tmp; tmp = tmp->in_obj )
	GET_OBJ_WEIGHT( tmp ) += GET_OBJ_WEIGHT( obj );
}



void store_to_char(struct char_file_u *st, struct char_data *ch)
{
    int i;

    strncpy( ch->pwd, st->pwd, 11 );

    GET_SEX(ch) = st->sex;
    GET_CLASS(ch) = st->class;
    GET_LEVEL(ch) = st->level;

    ch->player.short_descr = 0;
    ch->player.long_descr = 0;

    if (*st->title)
    {
	CREATE(ch->player.title, char, strlen(st->title) + 1);
	strcpy(ch->player.title, st->title);
    }
    else
	GET_TITLE(ch) = 0;

    if (*st->description)
    {
	CREATE(ch->player.description, char, 
	    strlen(st->description) + 1);
	strcpy(ch->player.description, st->description);
    }
    else
	ch->player.description = 0;

    ch->player.hometown = st->hometown;

    ch->player.time.birth = st->birth;
    ch->player.time.played = st->played;
    ch->player.time.logon  = time(0);

    for (i = 0; i < MAX_TONGUE; i++)
	ch->player.talks[i] = st->talks[i];

    ch->player.weight = st->weight;
    ch->player.height = st->height;

    ch->abilities = st->abilities;
    ch->tmpabilities = st->abilities;
    ch->points = st->points;
	if (ch->points.max_mana < 100) {
	   ch->points.max_mana = 100;
     } /* if */

    for (i = 0; i < MAX_SKILLS; i++)
	ch->skills[i] = st->skills[i];

    ch->specials.practices    = st->practices;
    ch->specials.alignment    = st->alignment;

    ch->specials.act          = st->act;
    ch->specials.carry_weight = 0;
    ch->specials.carry_items  = 0;
    ch->points.armor          = 100;
    ch->points.hitroll        = 0;
    ch->points.damroll        = 0;

    /* Not used as far as I can see (Michael) */
    for(i = 0; i <= 4; i++)
      ch->specials.apply_saving_throw[i] = st->apply_saving_throw[i];

    for(i = 0; i <= 2; i++)
      GET_COND(ch, i) = st->conditions[i];

    /* Add all spell effects */
    for(i=0; i < MAX_AFFECT; i++) {
	if (st->affected[i].type)
	    affect_to_char(ch, &st->affected[i]);
    }
    ch->in_room = real_room(st->load_room);
    affect_total(ch);
}



/* copy vital data from a players char-structure to the file structure */
void char_to_store(struct char_data *ch, struct char_file_u *st)
{
    int i;
    struct affected_type *af;
    struct obj_data *char_eq[MAX_WEAR];

    strncpy( st->pwd, ch->pwd, 11 );

    /* Unaffect everything a character can be affected by */

    for(i=0; i<MAX_WEAR; i++) {
	if (ch->equipment[i])
	    char_eq[i] = unequip_char(ch, i);
	else
	    char_eq[i] = 0;
    }

    for(af = ch->affected, i = 0; i<MAX_AFFECT; i++) {
	if (af &&
		    (!(af->type == SPELL_INVISIBLE &&
		       af->bitvector == AFF_INVISIBLE &&
		       ch->specials.wizInvis))) {
	    st->affected[i] = *af;
	    st->affected[i].next = 0;
	    /* subtract effect of the spell or the effect will be doubled */
	    affect_modify( ch, st->affected[i].location,
			       st->affected[i].modifier,
			       st->affected[i].bitvector, FALSE);                         
	    af = af->next;
	} else {
	    st->affected[i].type = 0;  /* Zero signifies not used */
	    st->affected[i].duration = 0;
	    st->affected[i].modifier = 0;
	    st->affected[i].location = 0;
	    st->affected[i].bitvector = 0;
	    st->affected[i].next = 0;
	}
    }

    if ((i >= MAX_AFFECT) && af && af->next)
	log( "Char_to_store: too many affects." );

    ch->tmpabilities = ch->abilities;

    st->version    = 0;
    st->birth      = ch->player.time.birth;
    st->played     = ch->player.time.played;
    st->played    += (long) (time(0) - ch->player.time.logon);
    st->last_logon = time(0);

    ch->player.time.played = st->played;
    ch->player.time.logon = time(0);

    st->hometown = ch->player.hometown;
    st->weight   = GET_WEIGHT(ch);
    st->height   = GET_HEIGHT(ch);
    st->sex      = GET_SEX(ch);
    st->class    = GET_CLASS(ch);
    st->level    = GET_LEVEL(ch);
    st->abilities = ch->abilities;
    st->points    = ch->points;
    st->alignment = ch->specials.alignment;
    st->practices = ch->specials.practices;
    st->act       = ch->specials.act;

    st->points.armor   = 100;
    st->points.hitroll =  0;
    st->points.damroll =  0;

    if (GET_TITLE(ch))
	strcpy(st->title, GET_TITLE(ch));
    else
	*st->title = '\0';

    if (ch->player.description)
	strcpy(st->description, ch->player.description);
    else
	*st->description = '\0';


    for (i = 0; i < MAX_TONGUE; i++)
	st->talks[i] = ch->player.talks[i];

    for (i = 0; i < MAX_SKILLS; i++)
	st->skills[i] = ch->skills[i];

    strcpy(st->name, GET_NAME(ch) );

    for(i = 0; i <= 4; i++)
      st->apply_saving_throw[i] = ch->specials.apply_saving_throw[i];

    for(i = 0; i <= 2; i++)
      st->conditions[i] = GET_COND(ch, i);

    for(af = ch->affected, i = 0; i<MAX_AFFECT; i++) {
	if (af &&
		    (!(af->type == SPELL_INVISIBLE &&
		       af->bitvector == AFF_INVISIBLE &&
		       ch->specials.wizInvis))) {
	    /* Add effect of the spell or it will be lost */
	    /* When saving without quitting               */
	    affect_modify( ch, st->affected[i].location,
			       st->affected[i].modifier,
			       st->affected[i].bitvector, TRUE);
	    af = af->next;
	}
    }

    for(i=0; i<MAX_WEAR; i++) {
	if (char_eq[i])
	    equip_char(ch, char_eq[i], i);
    }

    affect_total(ch);
}
