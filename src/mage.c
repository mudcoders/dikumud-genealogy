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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/*
 * Local functions.
 */

void	adv_spell_damage	args( ( CHAR_DATA *ch, OBJ_DATA *book, 
				OBJ_DATA *page, char *argument) );
void	adv_spell_affect	args( ( CHAR_DATA *ch, OBJ_DATA *book, 
				OBJ_DATA *page, char *argument) );
void	adv_spell_action	args( ( CHAR_DATA *ch, OBJ_DATA *book, 
				OBJ_DATA *page, char *argument) );
void do_chant( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *book;
    OBJ_DATA *page;
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    bool victim_target = FALSE;
    bool object_target = FALSE;
    bool global_target = FALSE; /* Target object/victim may be anywhere */
    int spellno = 1;
    int spellcount = 0;
    int spelltype;
    int sn = 0;
    int level;

    one_argument( argument, arg );

    if ( ( book = get_eq_char(ch, WEAR_WIELD) ) == NULL || 
	book->item_type != ITEM_BOOK)
    {
	if ( ( book = get_eq_char(ch, WEAR_HOLD) ) == NULL || 
	    book->item_type != ITEM_BOOK)
	{
	    send_to_char( "First you must hold a spellbook.\n\r", ch );
	    return;
	}
    }
    if (IS_SET(book->value[1], CONT_CLOSED))
    {
	send_to_char( "First you better open the book.\n\r", ch );
	return;
    }
    if (book->value[2] < 1)
    {
	send_to_char( "There are no spells on the index page!\n\r", ch );
	return;
    }
    if ( ( page = get_page( book, book->value[2] ) ) == NULL )
    {
	send_to_char( "The current page seems to have been torn out!\n\r", ch );
	return;
    }
    spellcount = ((page->value[1] * 10000) + (page->value[2] * 10) + 
	page->value[3]);
    act("You chant the arcane words from $p.",ch,book,NULL,TO_CHAR);
    act("$n chants some arcane words from $p.",ch,book,NULL,TO_ROOM);
    if (IS_SET(page->quest, QUEST_MASTER_RUNE))
    {
	ch->spectype = 0;
	if (IS_SET(page->spectype, ADV_FAILED) || 
	    !IS_SET(page->spectype, ADV_FINISHED) || page->points < 1)
	    send_to_char( "The spell failed.\n\r", ch );
	else if (IS_SET(page->spectype, ADV_DAMAGE))
	    adv_spell_damage(ch,book,page,argument);
	else if (IS_SET(page->spectype, ADV_AFFECT))
	    adv_spell_affect(ch,book,page,argument);
	else if (IS_SET(page->spectype, ADV_ACTION))
	    adv_spell_action(ch,book,page,argument);
	else send_to_char( "The spell failed.\n\r", ch );
	return;
    }
    switch ( spellcount )
    {
    default:
	send_to_char( "Nothing happens.\n\r", ch );
	return;
    case 10022:
	/* FIRE + DESTRUCTION + TARGETING */
	sn = skill_lookup( "fireball" );
	victim_target = TRUE;
	spellno = 2;
	break;
    case 640322:
	/* LIFE + ENHANCEMENT + TARGETING */
	sn = skill_lookup( "heal" );
	victim_target = TRUE;
	spellno = 2;
	break;
    case 1280044:
	/* DEATH + SUMMONING + AREA */
	sn = skill_lookup( "guardian" );
	spellno = 3;
	break;
    case 2565128:
	/* MIND + INFORMATION + OBJECT */
	sn = skill_lookup( "identify" );
	object_target = TRUE;
	global_target = TRUE;
	break;
    }
    if ( arg[0] == '\0' && (victim_target == TRUE || object_target == TRUE))
    {
	send_to_char( "Please specify a target.\n\r", ch );
	return;
    }
    if (victim_target && sn > 0)
    {
	if ( !global_target && ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	else if ( global_target && ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	if (IS_ITEMAFF(victim, ITEMA_REFLECT))
	{
	    send_to_char( "You are unable to focus your spell on them.\n\r", ch );
	    return;
	}
/*
        if (!IS_NPC(victim) && IS_DROW( victim ))
        {
            if (ch == victim )
            {
                send_to_char("You lower your magical resistance....\n\r", ch);
            }
            else if (number_percent ( )  <= victim->drow_magic)
            {
               send_to_char("Your spell does not affect them.\n\r", ch);
               return;
            }
        }
*/
	spelltype = (skill_table[sn].target);
	level = ch->spl[spelltype] * 0.25;
	(*skill_table[sn].spell_fun) ( sn, level, ch, victim );
	if (spellno > 1) (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
	if (spellno > 2) (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
        if (!IS_IMMORTAL( ch ))
		WAIT_STATE(ch, skill_table[sn].beats);
    }
    else if (object_target && sn > 0)
    {
	if ( !global_target && ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "You are not carrying that object.\n\r", ch );
	    return;
	}
	else if ( global_target && ( obj = get_obj_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "You cannot find any object like that.\n\r", ch );
	    return;
	}
	spelltype = (skill_table[sn].target);
	level = ch->spl[spelltype] * 0.25;
	(*skill_table[sn].spell_fun) ( sn, level, ch, obj );
	if (spellno > 1) (*skill_table[sn].spell_fun) ( sn, level, ch, obj );
	if (spellno > 2) (*skill_table[sn].spell_fun) ( sn, level, ch, obj );
	if (!IS_IMMORTAL( ch))	
		WAIT_STATE(ch, skill_table[sn].beats);
    }
    else if (sn > 0)
    {
	spelltype = (skill_table[sn].target);
	if (spelltype == TAR_OBJ_INV)
	{
	    send_to_char( "Nothing happens.\n\r", ch );
	    return;
	}
	level = ch->spl[spelltype] * 0.25;
	(*skill_table[sn].spell_fun) ( sn, level, ch, ch );
	if (spellno > 1) (*skill_table[sn].spell_fun) ( sn, level, ch, ch );
	if (spellno > 2) (*skill_table[sn].spell_fun) ( sn, level, ch, ch );
	if (!IS_IMMORTAL( ch ))
		WAIT_STATE(ch, skill_table[sn].beats);
    }
    else send_to_char( "Nothing happens.\n\r", ch );
    return;
}

OBJ_DATA *get_page( OBJ_DATA *book, int page_num )
{
    OBJ_DATA *page;
    OBJ_DATA *page_next;

    if (page_num < 1) return NULL;
    for ( page = book->contains; page != NULL; page = page_next )
    {
	page_next = page->next_content;
	if (page->value[0] == page_num) return page;
    }
    return NULL;
}

void adv_spell_damage( CHAR_DATA *ch, OBJ_DATA *book, OBJ_DATA *page, char *argument)
{
    char arg [MAX_STRING_LENGTH];
    char buf [MAX_STRING_LENGTH];
    OBJ_DATA *page_next;
    ROOM_INDEX_DATA *old_room;
    char next_par [MAX_INPUT_LENGTH];
    int mana_cost = page->points;
    int min = page->value[1];
    int max = page->value[2];
    int dam;
    int level;
    bool area_affect = FALSE;
    bool victim_target = FALSE;
    bool global_target = FALSE;
    bool next_page = FALSE;
    bool parameter = FALSE;
    bool not_caster = FALSE;
    bool no_players = FALSE;
    bool cast_message = FALSE;
    bool reversed = FALSE;

    if (ch->mana < mana_cost)
    {send_to_char("You have insufficient mana to chant this spell.\n\r",ch);return;}

    if (min < 1 || max < 1)
    {send_to_char("The spell failed.\n\r",ch);return;}

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,arg);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }

    one_argument( argument, arg );

    if (strlen(page->victpoweron) > 0 || str_cmp(page->victpoweron,"(null)"))
    {
	if (strlen(page->victpoweroff) > 0 || str_cmp(page->victpoweroff,"(null)"))
	    cast_message = TRUE;
    }

    if (IS_SET(page->spectype, ADV_AREA_AFFECT  )) area_affect   = TRUE;
    if (IS_SET(page->spectype, ADV_VICTIM_TARGET)) victim_target = TRUE;
    if (IS_SET(page->spectype, ADV_GLOBAL_TARGET)) global_target = TRUE;
    if (IS_SET(page->spectype, ADV_NEXT_PAGE    )) next_page     = TRUE;
    if (IS_SET(page->spectype, ADV_PARAMETER    )) parameter     = TRUE;
    if (IS_SET(page->spectype, ADV_NOT_CASTER   )) not_caster    = TRUE;
    if (IS_SET(page->spectype, ADV_NO_PLAYERS   )) no_players    = TRUE;
    if (IS_SET(page->spectype, ADV_REVERSED     )) reversed      = TRUE;

    if (victim_target)
    {
	CHAR_DATA *victim;

	if ( !global_target && ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	else if ( global_target && ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They are not here.\n\r", ch );
	    return;
	}
	if (reversed)
	    level = ch->spl[BLUE_MAGIC];
	else
	    level = ch->spl[RED_MAGIC];
	if (ch->in_room == victim->in_room)
	    dam = number_range(min,max) + level;
	else
	    dam = number_range(min,max);
	if (ch->spectype < 1000)
	{
	    ch->spectype += dam;
/*	    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_VAMPIRE) && 
		IS_VAMPAFF(victim, VAM_FORTITUDE) && dam > 1 && !reversed)
		dam = number_range(1,dam);*/
	    if (IS_ITEMAFF(victim, ITEMA_RESISTANCE) && dam > 1 && !reversed)
		dam = number_range(1,dam);
	    else if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_DEMON) && !reversed &&
		IS_SET(victim->pcdata->powers[DPOWER_FLAGS], DEM_TOUGH) && dam > 1)
		dam = number_range(1,dam);
/*	    else if (!IS_NPC(victim) && IS_CLASS(victim, CLASS_WEREWOLF) && 
		victim->pcdata->powers[WPOWER_BOAR] > 2 && dam > 1 && !reversed)
		dam *= 0.5;*/
	    if (!IS_NPC(victim) && IS_CLASS(victim,CLASS_DEMON) && victim->in_room != NULL 
		&& victim->in_room->vnum == ROOM_VNUM_HELL && !reversed)
	    { if (dam < 5) dam = 1; else dam *= 0.2; }
	}
	else dam = 0;
	old_room = ch->in_room;
	if (victim->in_room != NULL && victim->in_room != ch->in_room)
	{
	    char_from_room(ch);
	    char_to_room(ch,victim->in_room);
	}
	if (!reversed)
	{
	    if (is_safe(ch,victim))
	    {
		char_from_room(ch);
		char_to_room(ch,old_room);
		return;
	    }
	}
	else if (IS_ITEMAFF(victim, ITEMA_REFLECT))
	{
	    send_to_char( "You are unable to focus your spell on them.\n\r", ch );
	    char_from_room(ch);
	    char_to_room(ch,old_room);
	    return;
	}
	char_from_room(ch);
	char_to_room(ch,old_room);
	if (cast_message)
	{
	    strcpy(buf,page->victpoweron);
	    act2(buf,ch,NULL,victim,TO_CHAR);
	    strcpy(buf,page->victpoweroff);
	    act2(buf,ch,NULL,victim,TO_ROOM);
	}
	if (reversed)
	{
	    victim->hit += dam;
	    if (victim->hit > victim->max_hit) victim->hit = victim->max_hit;
	}
	else
	{
	    adv_damage(ch,victim,dam);
	    sprintf(buf,"Your %s strikes $N incredably hard!",page->chpoweroff);
	    act2(buf,ch,NULL,victim,TO_CHAR);
	    sprintf(buf,"$n's %s strikes $N incredably hard!",page->chpoweroff);
	    act2(buf,ch,NULL,victim,TO_NOTVICT);
	    sprintf(buf,"$n's %s strikes you incredably hard!",page->chpoweroff);
	    act2(buf,ch,NULL,victim,TO_VICT);
	 /*   if (!IS_NPC(victim) && number_percent() <= victim->pcdata->atm)
		act( "$n's spell is unable to penetrate your antimagic shield.", ch, NULL, victim, TO_VICT );
	    else*/
		hurt_person(ch,victim,dam);
	}
	if (!IS_IMMORTAL( ch ))
	{
		WAIT_STATE(ch, 12);
		ch->mana -= mana_cost;
	}
    }
    else if (area_affect)
    {
	CHAR_DATA *vch = ch;
	CHAR_DATA *vch_next;

	if (reversed)
	    level = ch->spl[BLUE_MAGIC];
	else
	    level = ch->spl[RED_MAGIC];
	dam = number_range(min,max) + (level * 0.5);
	if (ch->spectype < 1000)
	{
	    ch->spectype += dam;
	}
	else dam = 0;
	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) && !reversed)
	{
	    send_to_char("You cannot fight in a safe room.\n\r",ch);
	    return;
	}
	if (cast_message)
	{
	    strcpy(buf,page->victpoweron);
	    act2(buf,ch,NULL,vch,TO_CHAR);
	    strcpy(buf,page->victpoweroff);
	    act2(buf,ch,NULL,vch,TO_ROOM);
	}

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next	= vch->next;
	    if ( vch->in_room == NULL ) continue;
	    if ( ch == vch && not_caster ) continue;
	    if ( !IS_NPC(vch) && no_players ) continue;
	    if (!reversed)
	    {
		if (is_safe(ch,vch)) continue;
	    }
	    if (IS_ITEMAFF(vch, ITEMA_REFLECT))
	    {
		send_to_char( "You are unable to focus your spell on them.\n\r", ch );
		continue;
	    }
	    if ( vch->in_room == ch->in_room )
	    {
		if (reversed)
		{
		    vch->hit += dam;
		    if (vch->hit > vch->max_hit) vch->hit = vch->max_hit;
		}
		else
		{
		    adv_damage(ch,vch,dam);
		    sprintf(buf,"Your %s strikes $N incredably hard!",page->chpoweroff);
		    act2(buf,ch,NULL,vch,TO_CHAR);
		    sprintf(buf,"$n's %s strikes $N incredably hard!",page->chpoweroff);
		    act2(buf,ch,NULL,vch,TO_NOTVICT);
		    sprintf(buf,"$n's %s strikes you incredably hard!",page->chpoweroff);
		    act2(buf,ch,NULL,vch,TO_VICT);
/*		    if (!IS_NPC(vch) && number_percent() <= vch->pcdata->atm)
			act( "$n's spell is unable to penetrate your antimagic shield.", ch, NULL, vch, TO_VICT );
		    else if (!IS_NPC(vch) && IS_SET(vch->act, PLR_VAMPIRE) && 
			IS_VAMPAFF(vch, VAM_FORTITUDE) && dam > 1)
			hurt_person(ch,vch,number_range(1,dam));*/
		    if (IS_ITEMAFF(vch, ITEMA_RESISTANCE) && dam > 1)
			hurt_person(ch,vch,number_range(1,dam));
/*		    else if (!IS_NPC(vch) && IS_SET(vch->act, PLR_WEREWOLF) && 
			vch->pcdata->disc[TOTEM_BOAR] > 2 && dam > 1)
			hurt_person(ch,vch,(dam*0.5));*/
		    else hurt_person(ch,vch,dam);
		}
		continue;
	    }
	}
	if (!IS_IMMORTAL(ch))
	{
		WAIT_STATE(ch, 12);
		ch->mana -= mana_cost;
	}
    }
    else {send_to_char("The spell failed.\n\r",ch);return;}
    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	!IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,arg);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }
    return;
}

void adv_spell_affect( CHAR_DATA *ch, OBJ_DATA *book, OBJ_DATA *page, char *argument)
{
    char arg [MAX_STRING_LENGTH];
    char c_m [MAX_INPUT_LENGTH];
    char c_1 [MAX_INPUT_LENGTH];
    char c_2 [MAX_INPUT_LENGTH];
    OBJ_DATA *page_next;
    OBJ_DATA *obj = NULL;
    CHAR_DATA *victim = ch;
    char next_par [MAX_INPUT_LENGTH];
    int mana_cost = page->points;
    int apply_bit = page->value[1];
    int bonuses = page->value[2];
    int affect_bit = page->value[3];
    int sn;
    int level = page->level;
    bool any_affects = FALSE;
    bool area_affect = FALSE;
    bool victim_target = FALSE;
    bool object_target = FALSE;
    bool global_target = FALSE;
    bool next_page = FALSE;
    bool parameter = FALSE;
    bool not_caster = FALSE;
    bool no_players = FALSE;
    bool cast_message = FALSE;
    bool message_one = FALSE;
    bool message_two = FALSE;
    bool is_reversed = FALSE;

    if (ch->mana < mana_cost)
    {send_to_char("You have insufficient mana to chant this spell.\n\r",ch);return;}

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }

    one_argument( argument, arg );

    if (strlen(page->chpoweroff) > 0 && str_cmp(page->chpoweroff,"(null)"))
	{strcpy(c_m,page->chpoweroff);cast_message = TRUE;}
    if (strlen(page->victpoweron) > 0 && str_cmp(page->victpoweron,"(null)"))
	{strcpy(c_1,page->victpoweron);message_one = TRUE;}
    if (strlen(page->victpoweroff) > 0 && str_cmp(page->victpoweroff,"(null)"))
	{strcpy(c_2,page->victpoweroff);message_two = TRUE;}

    if (IS_SET(page->spectype, ADV_AREA_AFFECT  )) area_affect   = TRUE;
    if (IS_SET(page->spectype, ADV_VICTIM_TARGET)) victim_target = TRUE;
    if (IS_SET(page->spectype, ADV_OBJECT_TARGET)) object_target = TRUE;
    if (IS_SET(page->spectype, ADV_GLOBAL_TARGET)) global_target = TRUE;
    if (IS_SET(page->spectype, ADV_NEXT_PAGE    )) next_page     = TRUE;
    if (IS_SET(page->spectype, ADV_PARAMETER    )) parameter     = TRUE;
    if (IS_SET(page->spectype, ADV_NOT_CASTER   )) not_caster    = TRUE;
    if (IS_SET(page->spectype, ADV_NO_PLAYERS   )) no_players    = TRUE;
    if (IS_SET(page->spectype, ADV_REVERSED     )) 
	{is_reversed = TRUE;bonuses = 0 - bonuses;}

    if (victim_target && !area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL ||
	    victim->in_room->area != ch->in_room->area)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && !area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && global_target && !victim_target)
    {
	if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (obj->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }

    if (page->toughness < PURPLE_MAGIC || page->toughness > YELLOW_MAGIC)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    if (page->toughness == PURPLE_MAGIC)
    {
	if ( ( sn = skill_lookup( "purple sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == RED_MAGIC)
    {
	if ( ( sn = skill_lookup( "red sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == BLUE_MAGIC)
    {
	if ( ( sn = skill_lookup( "blue sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == GREEN_MAGIC)
    {
	if ( ( sn = skill_lookup( "green sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (page->toughness == YELLOW_MAGIC)
    {
	if ( ( sn = skill_lookup( "yellow sorcery" ) ) < 0 )
	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else {send_to_char("The spell failed.\n\r",ch);return;}

    if (!victim_target && victim != ch)
	{send_to_char("The spell failed.\n\r",ch);return;}
    if (not_caster && ch == victim)
	{send_to_char("The spell failed.\n\r",ch);return;}
    else if (no_players && !IS_NPC(victim))
	{send_to_char("The spell failed.\n\r",ch);return;}

    if ( is_affected( victim, sn ) )
    {
	send_to_char("They are already affected by a spell of that colour.\n\r",ch);
	return;
    }

    if ( apply_bit == 0 )
    {
	enhance_stat(sn,level,ch,victim,APPLY_NONE,bonuses,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_STR) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_STR,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_DEX) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_DEX,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_INT) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_INT,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_WIS) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_WIS,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_CON) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_CON,bonuses * 0.1,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_MANA) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_MANA,bonuses * 5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_HIT) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_HIT,bonuses * 5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_MOVE) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_MOVE,bonuses * 5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_AC) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_AC,0 - (bonuses * 5),affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_HITROLL) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_HITROLL,bonuses * 0.5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_DAMROLL) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_DAMROLL,bonuses * 0.5,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if ( IS_SET(apply_bit, ADV_SAVING_SPELL) )
    {
	enhance_stat(sn,level,ch,victim,APPLY_SAVING_SPELL,bonuses*0.2,affect_bit);
	affect_bit = 0;
	any_affects = TRUE;
    }
    if (!any_affects) {send_to_char("The spell failed.\n\r",ch);return;}

    if (cast_message) act2(c_m,ch,NULL,victim,TO_CHAR);
    if (message_one)  act2(c_1,ch,NULL,victim,TO_VICT);
    if (message_two)  act2(c_2,ch,NULL,victim,TO_NOTVICT);
    if (!IS_IMMORTAL(ch))
    {
    	WAIT_STATE(ch,12);
    	ch->mana -= mana_cost;
    }

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	!IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }
    return;
}

void adv_spell_action( CHAR_DATA *ch, OBJ_DATA *book, OBJ_DATA *page, char *argument)
{
    char arg1 [MAX_STRING_LENGTH];
    char arg2 [MAX_STRING_LENGTH];
    char c_m [MAX_INPUT_LENGTH];
    char c_1 [MAX_INPUT_LENGTH];
    char c_2 [MAX_INPUT_LENGTH];
    OBJ_DATA *page_next;
    OBJ_DATA *obj = NULL;
    OBJ_DATA *obj2 = NULL;
    CHAR_DATA *victim = NULL;
    CHAR_DATA *victim2 = NULL;
    ROOM_INDEX_DATA *old_room = ch->in_room;
    char next_par [MAX_INPUT_LENGTH];
    int mana_cost = page->points;
    int action_bit = page->value[1];
    int action_type = page->value[2];
    bool area_affect = FALSE;
    bool victim_target = FALSE;
    bool object_target = FALSE;
    bool second_victim = FALSE;
    bool second_object = FALSE;
    bool global_target = FALSE;
    bool next_page = FALSE;
    bool parameter = FALSE;
    bool not_caster = FALSE;
    bool no_players = FALSE;
    bool cast_message = FALSE;
    bool message_one = FALSE;
    bool message_two = FALSE;
    bool is_reversed = FALSE;

    if (ch->mana < mana_cost)
    {send_to_char("You have insufficient mana to chant this spell.\n\r",ch);return;}

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (strlen(page->chpoweroff) > 0 && str_cmp(page->chpoweroff,"(null)"))
	{strcpy(c_m,page->chpoweroff);cast_message = TRUE;}
    if (strlen(page->victpoweron) > 0 && str_cmp(page->victpoweron,"(null)"))
	{strcpy(c_1,page->victpoweron);message_one = TRUE;}
    if (strlen(page->victpoweroff) > 0 && str_cmp(page->victpoweroff,"(null)"))
	{strcpy(c_2,page->victpoweroff);message_two = TRUE;}

    if (IS_SET(page->spectype, ADV_AREA_AFFECT  )) area_affect   = TRUE;
    if (IS_SET(page->spectype, ADV_VICTIM_TARGET)) victim_target = TRUE;
    if (IS_SET(page->spectype, ADV_OBJECT_TARGET)) object_target = TRUE;
    if (IS_SET(page->spectype, ADV_GLOBAL_TARGET)) global_target = TRUE;
    if (IS_SET(page->spectype, ADV_NEXT_PAGE    )) next_page     = TRUE;
    if (IS_SET(page->spectype, ADV_PARAMETER    )) parameter     = TRUE;
    if (IS_SET(page->spectype, ADV_NOT_CASTER   )) not_caster    = TRUE;
    if (IS_SET(page->spectype, ADV_NO_PLAYERS   )) no_players    = TRUE;
    if (IS_SET(page->spectype, ADV_SECOND_VICTIM)) second_victim = TRUE;
    if (IS_SET(page->spectype, ADV_SECOND_OBJECT)) second_object = TRUE;
    if (IS_SET(page->spectype, ADV_REVERSED     )) is_reversed   = TRUE;

    if (victim_target && !area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == ch)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && area_affect && !global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL ||
	    victim->in_room->area != ch->in_room->area)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == ch)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (victim_target && global_target && !object_target)
    {
	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
 	if (!IS_NPC(victim) && !IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == ch)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && !area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && area_affect && !global_target && !victim_target)
    {
	if ( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (object_target && global_target && !victim_target)
    {
	if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (obj->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }

    if ( arg2[0] == '\0' && (second_victim || second_object) )
    {
	send_to_char("Please specify a target.\n\r",ch);
	return;
    }
    else if (second_victim && victim_target && !area_affect && !global_target && !object_target)
    {
	if ( ( victim2 = get_char_room( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim2->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == NULL || IS_NPC(victim) || IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim2) && !IS_IMMUNE(victim2, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == victim2)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_victim && victim_target && area_affect && !global_target && !object_target)
    {
	if ( ( victim2 = get_char_world( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim2->in_room == NULL ||
	    victim2->in_room->area != ch->in_room->area)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == NULL || IS_NPC(victim) || IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim2) && !IS_IMMUNE(victim2, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == victim2)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_victim && victim_target && global_target && !object_target)
    {
	if ( ( victim2 = get_char_world( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim2->in_room == NULL)
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == NULL || IS_NPC(victim) || IS_IMMUNE(victim, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (!IS_NPC(victim2) && !IS_IMMUNE(victim2, IMM_SUMMON))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (victim == victim2)
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_object && object_target && !area_affect && !global_target && !victim_target)
    {
	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_object && object_target && area_affect && !global_target && !victim_target)
    {
	if ( ( obj2 = get_obj_here( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }
    else if (second_object && object_target && global_target && !victim_target)
    {
	if ( ( obj2 = get_obj_world( ch, arg2 ) ) == NULL )
    	{send_to_char("The spell failed.\n\r",ch);return;}
    }

    if (victim != NULL)
    {
	if (victim->level > ch->spl[PURPLE_MAGIC])
	{
	    send_to_char("The spell failed.\n\r",ch);
	    return;
	}
    }

    if (victim2 != NULL)
    {
	if (victim2->level > ch->spl[PURPLE_MAGIC])
	{
	    send_to_char("The spell failed.\n\r",ch);
	    return;
	}
    }

    switch ( action_bit )
    {
    default:
	send_to_char("The spell failed.\n\r",ch);
	return;
    case ACTION_MOVE:
	if (!victim_target && !second_victim && !object_target && !second_object)
	{
	    if (cast_message) act2(c_m,ch,NULL,NULL,TO_CHAR);
	    if (message_one) act2(c_1,ch,NULL,NULL,TO_ROOM);
	    char_from_room(ch);
	    char_to_room(ch,old_room);
	    if (message_two) act2(c_2,ch,NULL,NULL,TO_ROOM);
	}
	else if ( arg1[0] == '\0' )
	{
	    send_to_char("Please specify a target.\n\r",ch);
	    return;
	}
	else if (victim_target && !second_victim && !object_target && !second_object)
	{
	    if (victim == NULL || victim->in_room == NULL ||
		victim->in_room == ch->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (is_reversed)
	    {
		if (victim->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,victim,NULL,NULL,TO_CHAR);
		if (message_one) act2(c_1,victim,NULL,NULL,TO_ROOM);
		char_from_room(victim);
		char_to_room(victim,ch->in_room);
		if (message_two) act2(c_2,victim,NULL,NULL,TO_ROOM);
		do_look(victim,"");
	    }
	    else
	    {
		if (ch->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,ch,NULL,NULL,TO_CHAR);
		if (message_one) act2(c_1,ch,NULL,NULL,TO_ROOM);
		char_from_room(ch);
		char_to_room(ch,victim->in_room);
		if (message_two) act2(c_2,ch,NULL,NULL,TO_ROOM);
		do_look(ch,"");
	    }
	}
	else if (!victim_target && !second_victim && object_target && !second_object)
	{
	    if (obj == NULL || obj->in_room == NULL ||
		obj->in_room == ch->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,ch,obj,NULL,TO_CHAR);
	    if (message_one) act2(c_1,ch,obj,NULL,TO_ROOM);
	    if (is_reversed)
	    {
		obj_from_room(obj);
		obj_to_room(obj,ch->in_room);
	    }
	    else
	    {
		char_from_room(ch);
		char_to_room(ch,obj->in_room);
		do_look(ch,"");
	    }
	    if (message_two) act2(c_2,ch,obj,NULL,TO_ROOM);
	}
	else if (victim_target && second_victim && !object_target && !second_object)
	{
	    if (victim == NULL || victim->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (victim2 == NULL || victim2->in_room == NULL ||
		victim2->in_room == victim->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (is_reversed)
	    {
		if (victim2->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,victim2,NULL,victim,TO_CHAR);
		if (message_one) act2(c_1,victim2,NULL,victim,TO_ROOM);
		char_from_room(victim2);
		char_to_room(victim2,victim->in_room);
		if (message_two) act2(c_2,victim2,NULL,victim,TO_ROOM);
		do_look(victim2,"");
	    }
	    else
	    {
		if (victim->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		if (cast_message) act2(c_m,victim,NULL,victim2,TO_CHAR);
		if (message_one) act2(c_1,victim,NULL,victim2,TO_ROOM);
		char_from_room(victim);
		char_to_room(victim,victim2->in_room);
		if (message_two) act2(c_2,victim,NULL,victim2,TO_ROOM);
		do_look(victim,"");
	    }
	}
	else if (victim_target && !second_victim && !object_target && second_object)
	{
	    if (victim == NULL || victim->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (obj2 == NULL || obj2->in_room == NULL ||
		obj2->in_room == victim->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,victim,NULL,NULL,TO_CHAR);
	    if (message_one) act2(c_1,victim,obj2,NULL,TO_ROOM);
	    if (is_reversed)
	    {
		obj_from_room(obj2);
		obj_to_room(obj2,victim->in_room);
	    }
	    else
	    {
		if (victim->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		char_from_room(victim);
		char_to_room(victim,obj2->in_room);
		do_look(victim,"");
	    }
	    if (message_two) act2(c_2,victim,obj2,NULL,TO_ROOM);
	}
	else if (!victim_target && !second_victim && object_target && second_object)
	{
	    if (obj == NULL || obj->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (obj2 == NULL || obj2->in_room == NULL ||
		obj2->in_room == obj->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,ch,obj,NULL,TO_CHAR);
	    if (is_reversed)
	    {
		old_room = ch->in_room;
		if (message_one) act2(c_1,ch,obj2,NULL,TO_ROOM);
		obj_from_room(obj2);
		obj_to_room(obj2,obj->in_room);
		char_from_room(ch);
		char_to_room(ch,obj->in_room);
		if (message_two) act2(c_2,ch,obj2,NULL,TO_ROOM);
		char_from_room(ch);
		char_to_room(ch,old_room);
	    }
	    else
	    {
		old_room = ch->in_room;
		if (message_one) act2(c_1,ch,obj,NULL,TO_ROOM);
		obj_from_room(obj);
		obj_to_room(obj,obj2->in_room);
		char_from_room(ch);
		char_to_room(ch,obj2->in_room);
		if (message_two) act2(c_2,ch,obj,NULL,TO_ROOM);
		char_from_room(ch);
		char_to_room(ch,old_room);
	    }
	}
	else if (!victim_target && second_victim && object_target && !second_object)
	{
	    if (victim2 == NULL || victim2->in_room == NULL)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (obj == NULL || obj->in_room == NULL ||
		obj->in_room == victim2->in_room)
	    {send_to_char("The spell failed.\n\r",ch);return;}
	    if (cast_message) act2(c_m,victim2,NULL,NULL,TO_CHAR);
	    if (message_one) act2(c_1,victim,obj2,NULL,TO_ROOM);
	    if (is_reversed)
	    {
		if (victim2->position == POS_FIGHTING)
		{send_to_char("The spell failed.\n\r",ch);return;}
		char_from_room(victim2);
		char_to_room(victim2,obj->in_room);
		do_look(victim2,"");
	    }
	    else
	    {
		obj_from_room(obj);
		obj_to_room(obj,victim2->in_room);
	    }
	    if (message_two) act2(c_2,victim2,obj,NULL,TO_ROOM);
	}
	else {send_to_char("The spell failed.\n\r",ch);return;}
	break;
    case ACTION_MOB:
	if ( action_type < 1)
	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_NPC(ch) || ch->pcdata->followers > 4)
	{send_to_char("The spell failed.\n\r",ch);return;}
	if (( victim = create_mobile( get_mob_index( action_type ))) == NULL )
	{send_to_char("The spell failed.\n\r",ch);return;}
	if (cast_message) act2(c_m,ch,NULL,victim,TO_CHAR);
	if (message_one) act2(c_1,ch,NULL,victim,TO_ROOM);
	ch->pcdata->followers++;
	char_to_room( victim, ch->in_room );
	SET_BIT(victim->act, ACT_NOEXP);
	free_string(victim->lord);
	victim->lord = str_dup(ch->name);
	if (victim->level > ch->spl[PURPLE_MAGIC])
	{
	    send_to_char("The spell failed.\n\r",ch);
	    extract_char(victim, TRUE);
	    return;
	}
	break;
    case ACTION_OBJECT:
	if ( action_type < 1)
	{send_to_char("The spell failed.\n\r",ch);return;}
	if (( obj = create_object( get_obj_index( action_type ),0 )) == NULL )
	{send_to_char("The spell failed.\n\r",ch);return;}
	if (cast_message) act2(c_m,ch,obj,NULL,TO_CHAR);
	if (message_one) act2(c_1,ch,obj,NULL,TO_ROOM);
	free_string(obj->questmaker);
	obj->questmaker = str_dup(ch->name);
	obj_to_room( obj, ch->in_room );
	break;
    }
    if (!IS_IMMORTAL( ch ))
    {
    	WAIT_STATE(ch,12);
    	ch->mana -= mana_cost;
    }

    if (IS_SET(page->spectype, ADV_NEXT_PAGE) &&
	!IS_SET(page->spectype, ADV_SPELL_FIRST))
    {
    	if (strlen(page->chpoweroff) < 2 || !str_cmp(page->chpoweroff,"(null)"))
    	{send_to_char("The spell failed.\n\r",ch);return;}
	if (IS_SET(page->spectype, ADV_PARAMETER))
	{
	    if (!str_cmp(page->chpoweron,"(null)"))
		{send_to_char("The spell failed.\n\r",ch);return;}
	    else strcpy(next_par,page->chpoweron);
	}
	else strcpy(next_par,argument);
	if ( page->specpower < page->value[0] )
	{send_to_char( "The spell failed.\n\r", ch );return;}
	if ( ( page_next = get_page( book, page->specpower ) ) == NULL )
	{send_to_char( "The spell failed.\n\r", ch );return;}
    	if (IS_SET(page_next->quest, QUEST_MASTER_RUNE))
    	{
	    if      (IS_SET(page_next->spectype, ADV_DAMAGE))
	    	adv_spell_damage(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_AFFECT))
	    	adv_spell_affect(ch,book,page_next,next_par);
	    else if (IS_SET(page_next->spectype, ADV_ACTION))
	    	adv_spell_action(ch,book,page_next,next_par);
	    else {send_to_char( "The spell failed.\n\r", ch );return;}
    	}
	else {send_to_char( "The spell failed.\n\r", ch );return;}
    }
    return;
}
