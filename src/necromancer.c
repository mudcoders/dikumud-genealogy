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
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
 * This file holds the spells and skills used by necromancers
 *
 * This code is (C) 1996 The Maniac from Mythran Mud, you may use it
 * if you follow these rules:
 *	-Don't redistribute the code, or code based on this code
 *	-Include the help entries __EXACTLY__ as you find them in this file
 *	    they may not be altered or omitted
 *	-Your mud has to comply with the Envy, Merc and Diku linsences fully.
 *
 * Thanx go to Michael Quan and Mitchell Tse for writing the best muds ever
 * (merc, and envy 1 and 2)
 *
 * Maniac: v942346@si.hhs.nl
 * Snippets: http://www.hhs.nl/~v942346/snippets.html
 ***************************************************************************/

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void spell_rock_flesh( int sn, int level, CHAR_DATA *ch, void *vo )
{
	CHAR_DATA  *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	OBJ_DATA	*wood = NULL;
	OBJ_DATA	*dirt = NULL;
	OBJ_DATA	*obj = NULL;

        /* Check if we have the needed reagents:  blood, bone and wood */
        for ( obj = ch->carrying; obj; obj = obj->next_content )
        {
                if ( obj->pIndexData->vnum == OBJ_VNUM_NEC_DIRT )
                        dirt = obj;
                if ( obj->pIndexData->vnum == OBJ_VNUM_NEC_WOOD )
                        wood = obj;
                if (dirt && wood)
                        break;
        }

        if (!(obj) )     /* Char does not have all reagents */
        {       /* When !obj, the loop was done, and not all was found */
                send_to_char( "You do not have the required reagents.\n\r", ch );
		return;
        }

	extract_obj( wood );
	extract_obj( dirt );


	if ( is_affected( ch, sn ) )
	    return;

	af.type      = sn;
	af.duration  = level;
	af.location  = APPLY_AC;
	af.modifier  = -50;
	af.bitvector = 0;
	affect_to_char( victim, &af );

	send_to_char( "Your flesh turns to rock.\n\r", victim );
	act( "$n's flesh turns to rock.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_summon_dead( int sn, int level, CHAR_DATA *ch, void *vo )
{
	CHAR_DATA *zombie;
	OBJ_DATA *obj = NULL;
	OBJ_DATA *blood = NULL;
	OBJ_DATA *bone = NULL;
	OBJ_DATA *wood = NULL;

	/* Check if we have the needed reagents:  blood, bone and wood */
	for ( obj = ch->carrying; obj; obj = obj->next_content )
	{
		if ( obj->pIndexData->vnum == OBJ_VNUM_NEC_BLOOD )
			blood = obj;
		if ( obj->pIndexData->vnum == OBJ_VNUM_NEC_BONE )
			bone = obj;
		if ( obj->pIndexData->vnum == OBJ_VNUM_NEC_WOOD )
			wood = obj;
		if (blood && bone && wood)
			break;
	}
	if (!obj )
	{
		send_to_char( "You do not have the required reagents.\n\r", ch );
		return;
	}

	/* We have all reagents... now extract them from player */
	extract_obj(blood);
	extract_obj(bone);
	extract_obj(wood);

	zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE ) );
	SET_BIT( zombie->act, ACT_PET);			/* set some     */
	SET_BIT( zombie->affected_by, AFF_CHARM );	/* stuff	*/
	char_to_room (zombie, ch->in_room);		/* get it here	*/
	zombie->level = number_range( 10, 20 );		/* setup level	*/
	zombie->max_hit = (ch->max_hit /3);	/* give it hp	*/
	zombie->hit = zombie->max_hit;		/* give it full health	*/

	send_to_char("You call upon the dark powers and summon a dead warrior.\n\r", ch);
	act( "$n calls upon dark powers to bring back a dead warrior.", ch, NULL, NULL, TO_ROOM );
	add_follower( zombie, ch);
	zombie->leader = ch;
	return;
}

void spell_call_quake( int sn, int level, CHAR_DATA *ch, void *vo )
{
	CHAR_DATA *vch;
	OBJ_DATA *obj = NULL;
	OBJ_DATA *blood = NULL;
	OBJ_DATA *bone = NULL;
	OBJ_DATA *wood = NULL;
	OBJ_DATA *blackmoor = NULL;

	/* Check if we have the needed reagents:  blood, bone and wood */
	for ( obj = ch->carrying; obj; obj = obj->next_content )
	{
		if ( obj->pIndexData->vnum == OBJ_VNUM_NEC_BLOOD )
			blood = obj;
		if ( obj->pIndexData->vnum == OBJ_VNUM_NEC_BONE )
			bone = obj;
		if ( obj->pIndexData->vnum == OBJ_VNUM_NEC_WOOD )
			wood = obj;
		if ( obj->pIndexData->vnum == OBJ_VNUM_NEC_BLACKMOOR )
			blackmoor = obj;

		if (blood && bone && wood && blackmoor)
		break;
	}
	if (!obj )
	{
		send_to_char( "You do not have the required reagents.\n\r", ch );
		return;
	}
        extract_obj(blood);
        extract_obj(bone);
        extract_obj(wood);
	extract_obj(blackmoor);

	send_to_char( "The earth trembles beneath your feet!\n\r", ch );
	act( "$n makes the earth tremble and shiver.", ch, NULL, NULL, TO_ROOM );

	for ( vch = char_list; vch; vch = vch->next )
	{
		if ( vch->deleted || !vch->in_room )
			continue;
		if ( vch->in_room == ch->in_room )
		{
		   if (vch != ch && (IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch)))
			damage( ch, vch, level + dice( 2, 8 ), sn, WEAR_NONE );
		   continue;
		}

		if ( vch->in_room->area == ch->in_room->area )
		send_to_char( "The earth trembles and shivers.\n\r", vch );
	}
	return;
}

