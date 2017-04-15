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
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN(	spec_breath_any		);
DECLARE_SPEC_FUN(	spec_breath_acid	);
DECLARE_SPEC_FUN(	spec_breath_fire	);
DECLARE_SPEC_FUN(	spec_breath_frost	);
DECLARE_SPEC_FUN(	spec_breath_gas		);
DECLARE_SPEC_FUN(	spec_breath_lightning	);
DECLARE_SPEC_FUN(	spec_cast_adept		);
DECLARE_SPEC_FUN(	spec_cast_cleric	);
DECLARE_SPEC_FUN(	spec_cast_judge		);
DECLARE_SPEC_FUN(	spec_cast_mage		);
DECLARE_SPEC_FUN(	spec_cast_undead	);
DECLARE_SPEC_FUN(	spec_fido		);
DECLARE_SPEC_FUN(	spec_guard		);
DECLARE_SPEC_FUN(	spec_janitor		);
DECLARE_SPEC_FUN(	spec_mayor		);
DECLARE_SPEC_FUN(	spec_poison		);
DECLARE_SPEC_FUN(	spec_thief		);
DECLARE_SPEC_FUN(	spec_eater		);
DECLARE_SPEC_FUN(	spec_gremlin_original	);
DECLARE_SPEC_FUN(	spec_gremlin_born	);
DECLARE_SPEC_FUN(	spec_rogue		);



/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN *spec_lookup( const char *name )
{
    if ( !str_cmp( name, "spec_breath_any"	  ) ) return spec_breath_any;
    if ( !str_cmp( name, "spec_breath_acid"	  ) ) return spec_breath_acid;
    if ( !str_cmp( name, "spec_breath_fire"	  ) ) return spec_breath_fire;
    if ( !str_cmp( name, "spec_breath_frost"	  ) ) return spec_breath_frost;
    if ( !str_cmp( name, "spec_breath_gas"	  ) ) return spec_breath_gas;
    if ( !str_cmp( name, "spec_breath_lightning"  ) ) return
							spec_breath_lightning;
    if ( !str_cmp( name, "spec_cast_adept"	  ) ) return spec_cast_adept;
    if ( !str_cmp( name, "spec_cast_cleric"	  ) ) return spec_cast_cleric;
    if ( !str_cmp( name, "spec_cast_judge"	  ) ) return spec_cast_judge;
    if ( !str_cmp( name, "spec_cast_mage"	  ) ) return spec_cast_mage;
    if ( !str_cmp( name, "spec_cast_undead"	  ) ) return spec_cast_undead;
    if ( !str_cmp( name, "spec_fido"		  ) ) return spec_fido;
    if ( !str_cmp( name, "spec_guard"		  ) ) return spec_guard;
    if ( !str_cmp( name, "spec_janitor"		  ) ) return spec_janitor;
    if ( !str_cmp( name, "spec_mayor"		  ) ) return spec_mayor;
    if ( !str_cmp( name, "spec_poison"		  ) ) return spec_poison;
    if ( !str_cmp( name, "spec_thief"		  ) ) return spec_thief;
    if ( !str_cmp( name, "spec_eater"		  ) ) return spec_eater;
    if ( !str_cmp( name, "spec_gremlin_original"  ) ) return spec_gremlin_original;
    if ( !str_cmp( name, "spec_gremlin_born"	  ) ) return spec_gremlin_born;
    if ( !str_cmp( name, "spec_rogue"		  ) ) return spec_rogue;
    return 0;
}



/*
 * Core procedure for dragons.
 */
bool dragon( CHAR_DATA *ch, char *spell_name )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    if ( ( sn = skill_lookup( spell_name ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



/*
 * Special procedures for mobiles.
 */
bool spec_breath_any( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING )
	return FALSE;

    switch ( number_bits( 3 ) )
    {
    case 0: return spec_breath_fire		( ch );
    case 1:
    case 2: return spec_breath_lightning	( ch );
    case 3: return spec_breath_gas		( ch );
    case 4: return spec_breath_acid		( ch );
    case 5:
    case 6:
    case 7: return spec_breath_frost		( ch );
    }

    return FALSE;
}



bool spec_breath_acid( CHAR_DATA *ch )
{
    return dragon( ch, "acid breath" );
}



bool spec_breath_fire( CHAR_DATA *ch )
{
    return dragon( ch, "fire breath" );
}



bool spec_breath_frost( CHAR_DATA *ch )
{
    return dragon( ch, "frost breath" );
}



bool spec_breath_gas( CHAR_DATA *ch )
{
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if ( ( sn = skill_lookup( "gas breath" ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, NULL );
    return TRUE;
}



bool spec_breath_lightning( CHAR_DATA *ch )
{
    return dragon( ch, "lightning breath" );
}



bool spec_cast_adept( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    switch ( number_bits( 3 ) )
    {
    case 0:
	act( "$n utters the word 'tehctah'.", ch, NULL, NULL, TO_ROOM );
	spell_armor( skill_lookup( "armor" ), ch->level, ch, victim );
	return TRUE;

    case 1:
	act( "$n utters the word 'nhak'.", ch, NULL, NULL, TO_ROOM );
	spell_bless( skill_lookup( "bless" ), ch->level, ch, victim );
	return TRUE;

    case 2:
	act( "$n utters the word 'yeruf'.", ch, NULL, NULL, TO_ROOM );
	spell_cure_blindness( skill_lookup( "cure blindness" ),
	    ch->level, ch, victim );
	return TRUE;

    case 3:
	act( "$n utters the word 'garf'.", ch, NULL, NULL, TO_ROOM );
	spell_cure_light( skill_lookup( "cure light" ),
	    ch->level, ch, victim );
	return TRUE;

    case 4:
	act( "$n utters the words 'rozar'.", ch, NULL, NULL, TO_ROOM );
	spell_cure_poison( skill_lookup( "cure poison" ),
	    ch->level, ch, victim );
	return TRUE;

    case 5:
	act( "$n utters the words 'nadroj'.", ch, NULL, NULL, TO_ROOM );
	spell_refresh( skill_lookup( "refresh" ), ch->level, ch, victim );
	return TRUE;

    }

    return FALSE;
}



bool spec_cast_cleric( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";      break;
	case  1: min_level =  3; spell = "cause serious";  break;
	case  2: min_level =  7; spell = "earthquake";     break;
	case  3: min_level =  9; spell = "cause critical"; break;
	case  4: min_level = 10; spell = "dispel evil";    break;
	case  5: min_level = 12; spell = "curse";          break;
	case  6: min_level = 12; spell = "change sex";     break;
	case  7: min_level = 13; spell = "flamestrike";    break;
	case  8:
	case  9:
	case 10: min_level = 15; spell = "harm";           break;
	default: min_level = 16; spell = "dispel magic";   break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_judge( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    spell = "high explosive";
    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_mage( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";      break;
	case  1: min_level =  3; spell = "chill touch";    break;
	case  2: min_level =  7; spell = "weaken";         break;
	case  3: min_level =  8; spell = "teleport";       break;
	case  4: min_level = 11; spell = "colour spray";   break;
	case  5: min_level = 12; spell = "change sex";     break;
	case  6: min_level = 13; spell = "energy drain";   break;
	case  7:
	case  8:
	case  9: min_level = 15; spell = "fireball";       break;
	default: min_level = 20; spell = "acid blast";     break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_cast_undead( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "curse";          break;
	case  1: min_level =  3; spell = "weaken";         break;
	case  2: min_level =  6; spell = "chill touch";    break;
	case  3: min_level =  9; spell = "blindness";      break;
	case  4: min_level = 12; spell = "poison";         break;
	case  5: min_level = 15; spell = "energy drain";   break;
	case  6: min_level = 18; spell = "harm";           break;
	case  7: min_level = 21; spell = "teleport";       break;
	default: min_level = 24; spell = "gate";           break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim );
    return TRUE;
}



bool spec_fido( CHAR_DATA *ch )
{
    OBJ_DATA *corpse;
    OBJ_DATA *c_next;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( corpse = ch->in_room->contents; corpse != NULL; corpse = c_next )
    {
	c_next = corpse->next_content;
	if ( corpse->item_type != ITEM_CORPSE_NPC )
	    continue;

	act( "$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM );
	for ( obj = corpse->contains; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    obj_from_obj( obj );
	    obj_to_room( obj, ch->in_room );
	}
	if (corpse != NULL) extract_obj( corpse );
	return TRUE;
    }

    return FALSE;
}



bool spec_guard( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    CHAR_DATA *ech;
    int max_evil;
    int rand;

    if ( !IS_AWAKE(ch) || ch->fighting != NULL )
	return FALSE;

    max_evil = 300;
    ech      = NULL;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;

	if ( !IS_NPC(victim) && victim->race > 24 && number_percent() > 95)
	    break;

	if ( victim->fighting != NULL
	&&   victim->fighting != ch
	&&   victim->alignment < max_evil )
	{
	    max_evil = victim->alignment;
	    ech      = victim;
	}
    }

    if ( victim != NULL )
    {
	rand=number_range(1,5);
	if (rand == 1)
	{
	    sprintf( buf, "It is an honour to meet you, %s!",
	        victim->name);
	    do_say( ch, buf );
	}
	else if (rand == 2)
	{
	    act("You bow deeply before $N.",ch,NULL,victim,TO_CHAR);
	    act("$n bows deeply before you.",ch,NULL,victim,TO_VICT);
	    act("$n bows deeply before $N.",ch,NULL,victim,TO_NOTVICT);
	}
	else if (rand == 3)
	{
	    act("You shake $N's hand.",ch,NULL,victim,TO_CHAR);
	    act("$n shakes your hand.",ch,NULL,victim,TO_VICT);
	    act("$n shakes $N's hand.",ch,NULL,victim,TO_NOTVICT);
	    sprintf( buf, "It's a pleasure to see you again, %s!",
	        victim->name);
	    do_say( ch, buf );
	}
	else if (rand == 4)
	{
	    act("You pat $N on the back.",ch,NULL,victim,TO_CHAR);
	    act("$n pats you on the back.",ch,NULL,victim,TO_VICT);
	    act("$n pats $N on the back.",ch,NULL,victim,TO_NOTVICT);
	    sprintf( buf, "Greetings %s!  If you need anything, just say!",
	        victim->name);
	    do_say( ch, buf );
	}
	else
	{
	    act("You beam a smile at $N.",ch,NULL,victim,TO_CHAR);
	    act("$n beams a smile at you.",ch,NULL,victim,TO_VICT);
	    act("$n beams a smile at $N.",ch,NULL,victim,TO_NOTVICT);
	}
	return TRUE;
    }

    if ( ech != NULL )
    {
	if (!IS_NPC(ech) && ech->race >= 25)
	{
	    sprintf( buf, "How DARE you attack %s? You shall DIE!",
	        ech->name);
	    do_say( ch, buf );
	    do_rescue( ch, ech->name );
	    return TRUE;
	}
	act( "$n screams 'PROTECT THE INNOCENT!!  BANZAI!!",
	    ch, NULL, NULL, TO_ROOM );
	multi_hit( ch, ech, TYPE_UNDEFINED );
	return TRUE;
    }

    return FALSE;
}



bool spec_janitor( CHAR_DATA *ch )
{
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( trash = ch->in_room->contents; trash != NULL; trash = trash_next )
    {
	trash_next = trash->next_content;
	if ( !IS_SET( trash->wear_flags, ITEM_TAKE ) )
	    continue;
	if ( trash->item_type == ITEM_DRINK_CON
	||   trash->item_type == ITEM_TRASH
	||   trash->cost < 10 )
	{
	    act( "$n picks up some trash.", ch, NULL, NULL, TO_ROOM );
	    obj_from_room( trash );
	    obj_to_char( trash, ch );
	    return TRUE;
	}
    }

    return FALSE;
}



bool spec_mayor( CHAR_DATA *ch )
{
    static const char open_path[] =
	"W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";

    static const char close_path[] =
	"W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";

    static const char *path;
    static int pos;
    static bool move;

    if ( !move )
    {
	if ( time_info.hour ==  6 )
	{
	    path = open_path;
	    move = TRUE;
	    pos  = 0;
	}

	if ( time_info.hour == 20 )
	{
	    path = close_path;
	    move = TRUE;
	    pos  = 0;
	}
    }

    if ( ch->fighting != NULL )
	return spec_cast_cleric( ch );
    if ( !move || ch->position < POS_SLEEPING )
	return FALSE;

    switch ( path[pos] )
    {
    case '0':
    case '1':
    case '2':
    case '3':
	move_char( ch, path[pos] - '0' );
	break;

    case 'W':
	ch->position = POS_STANDING;
	act( "$n awakens and groans loudly.", ch, NULL, NULL, TO_ROOM );
	break;

    case 'S':
	ch->position = POS_SLEEPING;
	act( "$n lies down and falls asleep.", ch, NULL, NULL, TO_ROOM );
	break;

    case 'a':
	act( "$n says 'Hello Honey!'", ch, NULL, NULL, TO_ROOM );
	break;

    case 'b':
	act( "$n says 'What a view!  I must do something about that dump!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'c':
	act( "$n says 'Vandals!  Youngsters have no respect for anything!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'd':
	act( "$n says 'Good day, citizens!'", ch, NULL, NULL, TO_ROOM );
	break;

    case 'e':
	act( "$n says 'I hereby declare the city of Midgaard open!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'E':
	act( "$n says 'I hereby declare the city of Midgaard closed!'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'O':
	do_unlock( ch, "gate" );
	do_open( ch, "gate" );
	break;

    case 'C':
	do_close( ch, "gate" );
	do_lock( ch, "gate" );
	break;

    case '.' :
	move = FALSE;
	break;
    }

    pos++;
    return FALSE;
}



bool spec_poison( CHAR_DATA *ch )
{
    CHAR_DATA *victim;

    if ( ch->position != POS_FIGHTING
    || ( victim = ch->fighting ) == NULL
    ||   number_percent( ) > 2 * ch->level )
	return FALSE;

    act( "You bite $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n bites $N!",  ch, NULL, victim, TO_NOTVICT );
    act( "$n bites you!", ch, NULL, victim, TO_VICT    );
    spell_poison( gsn_poison, ch->level, ch, victim );
    return TRUE;
}



bool spec_thief( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    int gold;

    if ( ch->position != POS_STANDING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;

	if ( IS_NPC(victim)
	||   (!IS_NPC(victim) && IS_IMMUNE(victim, IMM_STEAL))
	||   victim->level >= LEVEL_IMMORTAL
	||   number_bits( 2 ) != 0
	||   !can_see( ch, victim ) )	/* Thx Glop */
	    continue;

	if ( IS_AWAKE(victim) && number_range( 0, ch->level ) == 0 )
	{
	    act( "You discover $n's hands in your wallet!",
		ch, NULL, victim, TO_VICT );
	    act( "$N discovers $n's hands in $S wallet!",
		ch, NULL, victim, TO_NOTVICT );
	    return TRUE;
	}
	else
	{
	    gold = victim->gold * number_range( 1, 20 ) / 100;
	    ch->gold     += 7 * gold / 8;
	    victim->gold -= gold;
	    return TRUE;
	}
    }

    return FALSE;
}

bool spec_eater( CHAR_DATA *ch )
{
  /* The spec_eater is a hungry bugger who eats players.  If they get 
   * eaten, they get transported to the room with the same vnum as the 
   * mob Example:  A spec_eater dragon with the vnum 31305 would send 
   * anybody eaten to room 31305.
   * KaVir.
   */
   CHAR_DATA       *victim;
   ROOM_INDEX_DATA *pRoomIndex;
   if ( ch->position != POS_FIGHTING )
   {
      return FALSE;
   }
   if (number_percent() > 50 ) return FALSE;
   victim=ch->fighting;
   act( "$n stares at $N hungrily and licks $s lips!",  ch, NULL, victim, TO_NOTVICT    );
   act( "$n stares at you hungrily and licks $s lips!",  ch, NULL, victim, TO_VICT    );
   if (number_percent() > 25 ) return FALSE;
   pRoomIndex = get_room_index(ch->pIndexData->vnum);
   act( "$n opens $s mouth wide and lunges at you!", ch, NULL, victim, TO_VICT    );
   act( "$n swallows you whole!", ch, NULL, victim, TO_VICT    );
   act( "$n opens $s mouth wide and lunges at $N!",  ch, NULL, victim, TO_NOTVICT );
   act( "$n swallows $N whole!",  ch, NULL, victim, TO_NOTVICT );
   char_from_room(victim);
   char_to_room(victim,pRoomIndex);
   do_emote(ch,"burps loudly.");
   do_look(victim,"auto");
   return TRUE;
}

bool spec_gremlin_original( CHAR_DATA *ch )
{
   OBJ_DATA *object;
   OBJ_DATA *object_next;
   OBJ_DATA *obj;

   if ( !IS_AWAKE( ch ) )
      return FALSE;

   if (number_percent() > 25)
   {
	CHAR_DATA *victim;
	CHAR_DATA *v_next;
	char buf  [MAX_INPUT_LENGTH];
	/* Lets make this mob DO things!  KaVir */
	int speech;
	speech = number_range(1,8);
	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
	{
	    v_next = victim->next_in_room;
	    if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
	        break;
	}
	if ( victim != NULL )
	{
	    if (speech == 1) do_say(ch,"Anyone got any food? I'm famished!");
	    if (speech == 2) do_emote(ch,"rubs his tummy hungrily.");
	    if (speech == 3) do_emote(ch,"looks around for any scraps of food.");
	    if (speech == 4)
	    {
		if(!IS_NPC(victim))
		    sprintf(buf,"Excuse me %s, have you got any spare food?",victim->name);
		else
		    sprintf(buf,"I wonder if %s has got any spare food?",victim->short_descr);
		do_say(ch,buf);
	    }

	    if (speech == 5) do_say(ch,"If you're not hungry, drop that pie for me!");
	    if (speech == 6)
	    {
		if(!IS_NPC(victim))
		    sprintf(buf,"Do you have any food, %s?  Pleeeaaase?",victim->name);
		else
		    sprintf(buf,"I bet %s has lots of spare food!",victim->short_descr);
		do_say(ch,buf);
	    }
	    if (speech == 7)
	    {
		if(!IS_NPC(victim))
		    sprintf(buf,"Why won't you give me any food, %s?",victim->name);
		else
		    sprintf(buf,"Why won't you give me any food, %s?",victim->short_descr);
		do_say(ch,buf);
	    }
	    if (speech == 8)
	    {
		if(!IS_NPC(victim))
		    sprintf(buf,"I think i'll follow you around for a while, ok %s?",victim->name);
		else
		    sprintf(buf,"I bet %s has lots of food.",victim->short_descr);
		do_say(ch,buf);
		do_follow(ch,victim->name);
	    }
	}
	/*
        return TRUE;
	*/
   }

   do_drop(ch,"all");
   for ( object = ch->in_room->contents; object; object = object_next )
   {
      object_next = object->next_content;
      if ( !IS_SET(object->wear_flags, ITEM_TAKE) )
         continue;

      if (object == NULL) continue;

      if ( object->item_type == ITEM_FOOD
        || object->item_type == ITEM_TRASH
        || object->item_type == ITEM_CORPSE_NPC )
      {
         act( "$n picks $p up.", ch, object, NULL, TO_ROOM );
         obj_from_room( object );
         obj_to_char( object, ch );
         do_say(ch,"Ah....foooood....goooood!");
         if (object->item_type == ITEM_CORPSE_NPC) do_get(ch,"all corpse");
         act( "$n eats $p.", ch, object, NULL, TO_ROOM );
         if (object != NULL) extract_obj(object);
	 if (number_percent() > 25) return TRUE;
         act( "$n sits down and curls up into a ball.", ch,  NULL, NULL, TO_ROOM );
         act( "You watch in amazement as a cocoon forms around $n.", ch,  NULL, NULL, TO_ROOM );
         obj=create_object(get_obj_index(30008),0);
         obj->value[0] = 30002;
         obj->timer = 1;
         obj_to_room(obj,ch->in_room);
         extract_char(ch,TRUE);
         return TRUE;
      }
   }
   return FALSE;
}

bool spec_gremlin_born( CHAR_DATA *ch )
{
   OBJ_DATA        *object;
   OBJ_DATA        *object_next;
   OBJ_DATA        *obj;
   CHAR_DATA	   *victim;
   EXIT_DATA	   *pexit;
   ROOM_INDEX_DATA *to_room;
   int		   door;

   /* When they first appear, they try to equip themselves as best they can */
   if ( ch->max_move == 100 )
   {
         do_get(ch,"all");
         do_wear(ch,"all");
         do_drop(ch,"all");
   }
   ch->max_move = ch->max_move -1;
   if ( ch->max_mana > 249 )
   {
	 do_say(ch,"I cannot go any further...my task is complete.");
         act( "$n falls to the floor and crumbles into dust.", ch, NULL, NULL, TO_ROOM );
         extract_char(ch,TRUE);
	 return TRUE;
   }
   if ( ch->max_move < 5 )
   {
	 do_say(ch,"Alas, there is not enough food to go around...");
         act( "$n falls to the floor and crumbles into dust.", ch, NULL, NULL, TO_ROOM );
         extract_char(ch,TRUE);
	 return TRUE;
   }
   else if (( ch->max_move < 35) && number_percent() > 95 )
         act( "$n's stomach growls with hunger.", ch, NULL, NULL, TO_ROOM );

   if ( !IS_AWAKE( ch ) )
      return FALSE;

   for ( object = ch->in_room->contents; object; object = object_next )
   {
      object_next = object->next_content;
      if ( !IS_SET(object->wear_flags, ITEM_TAKE) )
         continue;

      if (object == NULL) continue;

      if ( object->item_type == ITEM_FOOD
        || object->item_type == ITEM_TRASH
        || object->item_type == ITEM_CORPSE_NPC )
      {
         act( "$n picks $p up.", ch, object, NULL, TO_ROOM );
         obj_from_room( object );
         obj_to_char( object, ch );
         if (object->item_type == ITEM_CORPSE_NPC) do_get(ch,"all corpse");
         act( "$n eats $p.", ch, object, NULL, TO_ROOM );
	 if (object != NULL) extract_obj(object);
	 if (ch->max_move < 80) ch->max_move = ch->max_move + 20;
         do_wear(ch,"all");
         do_drop(ch,"all");
	 /* Increase the following if gremlins reproduce too fast, and 
	  * reduce it if they reproduce too slowly.  KaVir.
	  */
	 if (ch->max_move > 75 && number_percent() > 25)
	 {
	     if (ch->pIndexData->vnum == 30004)
             	 obj=create_object(get_obj_index(30009),0);
	     else
	     {
             	 obj=create_object(get_obj_index(30008),0);
             	 obj->value[0] = 30003;
             	 if (ch->pIndexData->vnum == 30003)
             	     obj->value[0] = 30004;
             	 obj->timer = number_range(8,10);
	     }
             act( "$n squats down and lays $p!", ch,  obj, NULL, TO_ROOM );
             obj_to_room(obj,ch->in_room);
	     if (number_percent() > 25)
	     	 ch->max_mana = ch->max_mana +50;
	 }
	 return TRUE;
      }
   }

   if ( ch->fighting != NULL )
	return spec_poison( ch );

   /* We'll give him 3 chances to find an open door */
   door = number_range(0,5);
   if (((pexit = ch->in_room->exit[door]) == NULL) || (to_room = pexit->to_room) == NULL)
	door = number_range(0,5);
   if (((pexit = ch->in_room->exit[door]) == NULL) || (to_room = pexit->to_room) == NULL)
	door = number_range(0,5);
   if (!(((pexit = ch->in_room->exit[door]) == NULL) || (to_room = pexit->to_room) == NULL))
   {
   	/* If the door is closed they'll have to open it */
   	if (IS_SET(pexit->exit_info, EX_CLOSED))
   	{
	    if (door == 0) do_open(ch,"north");
	    if (door == 1) do_open(ch,"east");
	    if (door == 2) do_open(ch,"south");
	    if (door == 3) do_open(ch,"west");
	    if (door == 4) do_open(ch,"up");
	    if (door == 5) do_open(ch,"down");
   	}
   	/* Now they know where they are going, they have to move */
   	move_char(ch,door);
/*	return FALSE; */
   }
   /* Now we check the room for someone to kill */
   for (victim = char_list; victim; victim = victim->next)
   {
      if ( victim->in_room != ch->in_room
	 || !victim->in_room
	 || victim == ch
	 || (!IS_NPC(victim) && IS_HERO(victim) && victim->hit < 0)
         || ((victim->name == ch->name) && (ch->max_move > 30))
         || ((victim->level > ch->level) && (ch->level < 12 ))
         || IS_IMMORTAL(victim) )
         continue;

      if (victim->in_room == ch->in_room)
      {
         if (victim->name == ch->name)
	 {
	     do_say(ch,"Sorry brother, but I must eat you or I'll starve.");
             do_kill(ch,"gremlin");
             do_kill(ch,"2.gremlin");
             return TRUE;
	 }
	 else
	 {
             do_kill(ch,victim->name);
             return TRUE;
	 }
         return TRUE;
      }
      continue;
   }
   return FALSE;
}

/* Spec_rogue, coded by Malice. */
bool spec_rogue( CHAR_DATA *ch )
{
/* To add to the life of mobs... they pickup and wear equipment */
    OBJ_DATA *object;
    OBJ_DATA *obj2;
    OBJ_DATA *object_next;

    if ( !IS_AWAKE( ch ) )
	return FALSE;

    for ( object = ch->in_room->contents; object; object = object_next )
    {
        object_next = object->next_content;
        if ( object == NULL )
	    continue;
	    
	if ( !IS_SET( object->wear_flags, ITEM_TAKE ) )
	    continue;
	    
	if ( ( object->item_type != ITEM_DRINK_CON
	    && object->item_type != ITEM_TRASH )
	    &&
	    
	   !(( IS_OBJ_STAT( object, ITEM_ANTI_EVIL   ) && IS_EVIL   ( ch ) ) ||
		 ( IS_OBJ_STAT( object, ITEM_ANTI_GOOD   ) && IS_GOOD   ( ch ) ) ||
		 ( IS_OBJ_STAT( object, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL( ch ) ) ) )
		
	{
	    act( "$n picks up $p and examines it carefully.", ch, object, NULL, TO_ROOM );
	    obj_from_room( object );
	    obj_to_char( object, ch );
		/*Now compare it to what we already have*/
		for ( obj2 = ch->carrying; obj2; obj2 = obj2->next_content )
		{
		    if ( obj2->wear_loc != WEAR_NONE
			&& can_see_obj( ch, obj2 )
			&& object->item_type == obj2->item_type
			&& ( object->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
			break;
		}
		if (!obj2)
		{
		    switch (object->item_type)
		    {
			default:
			    do_say(ch,"Hey, what a find!");
			    break;
			case ITEM_FOOD:
			    do_say(ch, "This looks like a tasty morsel!");
			    do_eat(ch,object->name);
			    break;
			case ITEM_WAND:
			    do_say(ch,"Wow, a magic wand!");
			    wear_obj(ch,object,FALSE);
			    break;
			case ITEM_STAFF:
			    do_say(ch,"Kewl, a magic staff!");
			    wear_obj(ch,object,FALSE);
			    break;
			case ITEM_WEAPON:
			    do_say(ch,"Hey, this looks like a nifty weapon!");
			    wear_obj(ch,object,FALSE);
			    break;
			case ITEM_ARMOR:
			    do_say(ch,"Oooh...a nice piece of armor!");
			    wear_obj(ch,object,FALSE);
			    break;
			case ITEM_POTION:
			    do_say(ch, "Great!  I was feeling a little thirsty!");
			    act( "You quaff $p.", ch, object, NULL ,TO_CHAR );
			    act( "$n quaffs $p.", ch, object, NULL, TO_ROOM );
			    obj_cast_spell( object->value[1], object->level, ch, ch, NULL );
 			    obj_cast_spell( object->value[2], object->level, ch, ch, NULL );
			    obj_cast_spell( object->value[3], object->level, ch, ch, NULL );
			    extract_obj( object );
			    break;
			case ITEM_SCROLL:
			    do_say(ch,"Hmmm I wonder what this says?");
			    act( "You recite $p.", ch, object, NULL, TO_CHAR );
			    act( "$n recites $p.", ch, object, NULL, TO_ROOM );
			    obj_cast_spell( object->value[1], object->level, ch, NULL, object );
			    obj_cast_spell( object->value[2], object->level, ch, NULL, object );
			    obj_cast_spell( object->value[3], object->level, ch, NULL, object );
			    extract_obj( object );
			    break;
		    }
		    return TRUE;
		}
		
	    if ((object->level > obj2->level))
	    {
	    	do_say(ch,"Now THIS looks like an improvement!");
	    	remove_obj(ch,obj2->wear_loc,TRUE);
	    	wear_obj(ch,object,FALSE);
	    }
	    else
	    {
	    	do_say(ch,"I don't want this piece of junk!");
	    	act("You don't like the look of $p.",ch,object,NULL,TO_CHAR);
			do_drop(ch,object->name);
			do_sacrifice(ch,object->name);
		}
	    return TRUE;
	}
    }

    return FALSE;
}
