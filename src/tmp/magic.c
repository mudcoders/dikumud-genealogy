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


/*
 * External functions.
 */
bool    is_safe     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	set_fighting args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/*
 * Local functions.
 */
void	say_spell	args( ( CHAR_DATA *ch, int sn ) );
int     blood_count     args( ( OBJ_DATA *list, int amount ) );
void    magic_mob       args( ( CHAR_DATA *ch, OBJ_DATA *obj, int vnum ) );
int     slot_lookup     args( ( int slot ) );
int	sc_dam		args( ( CHAR_DATA *ch, int dam ) );
/*
 * "Permament sn's": slot loading for objects -- Altrag
 */
int slot_lookup( int slot )
{
  int sn;

  for ( sn = 0; skill_table[sn].name[0] != '\0'; sn++ )
    if ( skill_table[sn].slot == slot )
      return sn;

  bug( "Slot_lookup: no such slot #%d", slot );
  return 0;
}

/*
 * Replacement for MAX_SKILL -- Altrag
 */
bool is_sn( int sn )
{
  int cnt;

  for ( cnt = 0; skill_table[cnt].name[0] != '\0'; cnt++ )
    if ( cnt == sn )
      return TRUE;

  return FALSE;
}

void magic_mob ( CHAR_DATA *ch, OBJ_DATA *obj, int vnum )
{
   CHAR_DATA      *victim;
   CHAR_DATA      *zombie;
   MOB_INDEX_DATA *ZombIndex;
   MOB_INDEX_DATA *pMobIndex;
   char           *name;
   char            buf [MAX_STRING_LENGTH];

    if ( !( pMobIndex = get_mob_index( vnum ) ) )
    {
         send_to_char(AT_BLUE, "Nothing happens.\n\r", ch);
         return;
    }
    ZombIndex = get_mob_index( 3 );
    victim = create_mobile( pMobIndex );
    zombie = create_mobile( ZombIndex );
    name = victim->short_descr;
    sprintf( buf, zombie->short_descr, name );
    free_string( zombie->short_descr );
    zombie->short_descr = str_dup(buf);
    sprintf( buf, zombie->long_descr, name );
    free_string( zombie->long_descr );
    zombie->long_descr = str_dup(buf);
    victim->perm_hit /= 2;
    victim->hit = MAX_HIT(victim);
    zombie->mod_hit = victim->mod_hit;
    zombie->perm_hit = victim->perm_hit;
    zombie->hit = victim->hit;
    zombie->level = victim->level;
    SET_BIT( zombie->act, ACT_UNDEAD );
    SET_BIT( zombie->act, ACT_PET );
    SET_BIT( zombie->affected_by, AFF_CHARM );
    char_to_room( zombie, ch->in_room );
    add_follower( zombie, ch );
    update_pos( zombie );
    act( AT_BLUE, "$n passes $s hands over $p, $E slowly rises to serve $S new master.", ch, obj, zombie, TO_ROOM );
    act( AT_BLUE, "You animate $p, it rises to serve you.", ch, obj, NULL, TO_CHAR );
    char_to_room( victim, ch->in_room );
    extract_char ( victim, TRUE );
    return;
}
int blood_count( OBJ_DATA *list, int amount )
{
    OBJ_DATA   *obj;
    int         count;
    OBJ_DATA   *obj_next;


    count = 0;
    for ( obj = list; obj; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( obj->deleted )
	   continue;
	if ( ( obj->item_type == ITEM_BLOOD ) && ( count != amount ) )
	   {
	     count++;
             extract_obj( obj );
	   }
    }

    return count;
}

void update_skpell( CHAR_DATA *ch, int sn )
{

  int xp = 0;
  char buf[MAX_STRING_LENGTH];
  int adept;

  if ( IS_NPC( ch ) )
    return;

  adept = IS_NPC( ch ) ? 100 :
	  class_table[prime_class( ch )].skill_adept;

  if ( ch->pcdata->learned[sn] <= 0
    || ch->pcdata->learned[sn] >= adept )
      return;

  ch->pcdata->learned[sn] += ( get_curr_wis( ch ) / 5 );

  if ( ch->pcdata->learned[sn] > adept )
    ch->pcdata->learned[sn] = adept;

   xp = ch->level / 5;
   if ( xp < 2 )
     xp = 2;
   xp = number_range( xp * 2, ( xp * xp ) / 3 );
   if (!(ch->fighting))
   {
     sprintf( buf, "You gain %d experience for your success with %s.\n\r",
      	      xp, skill_table[sn].name );
     send_to_char( C_DEFAULT, buf, ch );
   }
  gain_exp( ch, xp );

   return;
}

int skill_lookup( const char *name )
{
    int sn;

    for ( sn = 0; skill_table[sn].name[0] != '\0'; sn++ )
    {
	if ( !skill_table[sn].name )
	    break;
	if ( LOWER( name[0] ) == LOWER( skill_table[sn].name[0] )
	    && !str_prefix( name, skill_table[sn].name ) )
	    return sn;
    }

    return -1;
}



/*
 * Utter mystical words for an sn.
 */
void say_spell( CHAR_DATA *ch, int sn )
{
                        CHAR_DATA *rch;
			char      *pName;
			char       buf       [ MAX_STRING_LENGTH ];
			char       buf2      [ MAX_STRING_LENGTH ];
			int        iSyl;
			int        length;

	       	 struct syl_type
	         {
		        char *	   old;
		        char *	   new;
		 };

    static const struct syl_type   syl_table [ ] =
    {
	{ " ",		" "		},
	{ "ar",		"abra"		},
	{ "au",		"kada"		},
	{ "bless",	"fido"		},
	{ "blind",	"nose"		},
	{ "bur",	"mosa"		},
	{ "cu",		"judi"		},
	{ "de",		"oculo"		},
	{ "en",		"unso"		},
	{ "light",	"dies"		},
	{ "lo",		"hi"		},
	{ "mor",	"zak"		},
	{ "move",	"sido"		},
	{ "ness",	"lacri"		},
	{ "ning",	"illa"		},
	{ "per",	"duda"		},
	{ "ra",		"gru"		},
	{ "re",		"candus"	},
	{ "son",	"sabru"		},
	{ "tect",	"infra"		},
	{ "tri",	"cula"		},
	{ "ven",	"nofo"		},
	{ "a", "a" }, { "b", "b" }, { "c", "q" }, { "d", "e" },
	{ "e", "z" }, { "f", "y" }, { "g", "o" }, { "h", "p" },
	{ "i", "u" }, { "j", "y" }, { "k", "t" }, { "l", "r" },
	{ "m", "w" }, { "n", "i" }, { "o", "a" }, { "p", "s" },
	{ "q", "d" }, { "r", "f" }, { "s", "g" }, { "t", "h" },
	{ "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
	{ "y", "l" }, { "z", "k" },
	{ "", "" }
    };

    buf[0]	= '\0';
    for ( pName = skill_table[sn].name; *pName != '\0'; pName += length )
    {
	for ( iSyl = 0;
	     ( length = strlen( syl_table[iSyl].old ) ) != 0;
	     iSyl++ )
	{
	    if ( !str_prefix( syl_table[iSyl].old, pName ) )
	    {
		strcat( buf, syl_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    sprintf( buf2, "$n %s the words, '%s'.",
	     prime_class(ch) != CLASS_BARD ?
	     "utters" : "sings", buf );
    sprintf( buf,  "$n %s the words, '%s'.",
	     prime_class(ch) != CLASS_BARD ?
	     "utters" : "sings", skill_table[sn].name );

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( rch != ch )
	    act(AT_BLUE,
	    is_class( rch, prime_class(ch) )
	    ? buf
	    : buf2, ch, NULL, rch, TO_VICT );
    }

    return;
}



/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */
bool saves_spell( int level, CHAR_DATA *victim )
{
    int save;
    int base = 20;
    int savebase;

    if ( IS_NPC( victim ) )
        base += 30;
    savebase = 0 - victim->saving_throw / 2;
    if ( victim->race == RACE_DWARF )
	savebase += 25 * savebase / 100;
    if ( !IS_NPC( victim ) )
        savebase /= 6;
    else
        savebase /= 2;
    save = base + ( victim->level - level ) + savebase;
    save = URANGE( 5, save, ( victim->race == RACE_DWARF ) ? 95 : 90 );
    return number_percent( ) < save;
}



/*
 * The kludgy global is for spells who want more stuff from command line.
 */
char *target_name;

void do_acspell ( CHAR_DATA *ch, OBJ_DATA *pObj, char *argument )
{
    void      *vo;
    OBJ_DATA  *obj = NULL;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    int        sn;
    int        spec;

    spec = skill_lookup( "astral walk" );
    target_name = one_argument( argument, arg1 );
    one_argument( target_name, arg2 );

    if ( IS_NPC( ch ) )
      if ( IS_SET( ch->affected_by, AFF_CHARM ) )
        return;

    if ( ( sn = skill_lookup( arg1 ) ) < 0)
    {
	send_to_char(AT_BLUE, "You can't do that.\n\r", ch );
	return;
    }

    if ( ( sn == spec )  && ( is_name( ch, arg2, ch->name ) ) )
       {
         send_to_char( AT_BLUE, "You are already in the same room as yourself.\n\r", ch );
         return;
       }

    /*
     * Locate targets.
     */
    victim	= NULL;
    obj		= NULL;
    vo		= NULL;

    switch ( skill_table[sn].target )
    {
    default:
	bug( "Do_cast: bad target for sn %d.", sn );
	return;

    case TAR_GROUP_OFFENSIVE:
    case TAR_GROUP_DEFENSIVE:
    case TAR_GROUP_ALL:
    case TAR_GROUP_OBJ:
    case TAR_GROUP_IGNORE:
	group_cast( sn, URANGE( 1, ch->level, LEVEL_HERO ), ch, arg2 );
	return;

    case TAR_IGNORE:
	break;

    case TAR_CHAR_OFFENSIVE:
    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_OFFENSIVE ) )
      {
       send_to_char( AT_BLUE, "You failed.\n\r", ch );
       return;
      }
    	if ( arg2[0] == '\0' )
	{
	    if ( !( victim = ch->fighting ) )
	    {
		send_to_char(AT_BLUE, "Cast the spell on whom?\n\r", ch );
		return;
	    }
	}
	else
	{
	    if ( !( victim = get_char_room( ch, arg2 ) ) )
	    {
		send_to_char(AT_BLUE, "They aren't here.\n\r", ch );
		return;
	    }
	}
    if ( IS_AFFECTED(victim, AFF_PEACE) )
    {
      send_to_char(AT_WHITE, "A wave of peace overcomes you.\n\r", ch);
      return;
    }
    if ( IS_AFFECTED( ch, AFF_PEACE) )
    {
	    affect_strip( ch, skill_lookup("aura of peace") );
	    REMOVE_BIT( ch->affected_by, AFF_PEACE );
    }
	if ( is_safe(ch, victim) )
	{
	  send_to_char(AT_BLUE,"You failed.\n\r",ch);
	  return;
	}
	vo = (void *) victim;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( arg2[0] == '\0' )
	{
	    victim = ch;
	}
	else
	{
	    if ( !( victim = get_char_room( ch, arg2 ) ) )
	    {
		send_to_char(AT_BLUE, "They aren't here.\n\r", ch );
		return;
	    }
	}

	vo = (void *) victim;
	break;

    case TAR_CHAR_SELF:
	if ( arg2[0] != '\0' && !is_name( ch, arg2, ch->name ) )
	{
	    send_to_char(AT_BLUE, "You cannot cast this spell on another.\n\r", ch );
	    return;
	}

	vo = (void *) ch;
	break;

    case TAR_OBJ_INV:
	if ( arg2[0] == '\0' )
	{
	    send_to_char(AT_BLUE, "What should the spell be cast upon?\n\r", ch );
	    return;
	}

	if ( !( obj = get_obj_carry( ch, arg2 ) ) )
	{
	    send_to_char(AT_BLUE, "You are not carrying that.\n\r", ch );
	    return;
	}

	vo = (void *) obj;
	break;
    }



    WAIT_STATE( ch, skill_table[sn].beats );

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_MAGIC ) )
      {
       send_to_char( AT_BLUE, "You failed.\n\r", ch );
       return;
      }
      if ( !IS_NPC( ch ) )
      update_skpell( ch, sn );
    (*skill_table[sn].spell_fun) ( sn, URANGE( 1, ch->level, LEVEL_HERO ),
				   ch, vo );

    if ( vo )
    {
      oprog_invoke_trigger( pObj, ch, vo );
      if ( skill_table[sn].target == TAR_OBJ_INV )
	oprog_cast_sn_trigger( obj, ch, sn, vo );
      rprog_cast_sn_trigger( ch->in_room, ch, sn, vo );
    }

    if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
	&& victim->master != ch && victim != ch && IS_AWAKE( victim ) )

    {
	CHAR_DATA *vch;

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted )
	        continue;
	    if ( victim == vch && !victim->fighting )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	}
    }

    return;
}

void do_cast( CHAR_DATA *ch, char *argument )
{
    void      *vo;
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    int        mana;
    int        sn;
    bool	IS_DIVINE;
/*    int        spec; */
/*    char       buf [ MAX_STRING_LENGTH ];*/

/*    spec = skill_lookup( "astral walk" ); */

    IS_DIVINE = FALSE;

    target_name = one_argument( argument, arg1 );

    if ( arg1[0] != '\0' )
     if ( !str_prefix( arg1, "divine" ) && ch->level >= LEVEL_IMMORTAL )
     {
       IS_DIVINE = TRUE;
       target_name = one_argument( target_name, arg1 );
     }

    one_argument( target_name, arg2 );

    if ( arg1[0] == '\0' )
    {
	if ( prime_class(ch) != CLASS_BARD )
		send_to_char(AT_BLUE, "Cast which what where?\n\r", ch );
	else
		send_to_char(AT_BLUE, "Sing what song?\n\r", ch );
	return;
    }

    if ( IS_NPC( ch ) )
      if ( IS_SET( ch->affected_by, AFF_CHARM ) )
        return;

    if ( !IS_NPC( ch ) )
    if ( ( sn = skill_lookup( arg1 ) ) < 0
	|| !can_use_skpell( ch, sn ) )
    {
	send_to_char(AT_BLUE, "You can't do that.\n\r", ch );
	return;
    }

    if ( IS_NPC( ch ) )
     if ( ( sn = skill_lookup( arg1 ) ) < 0 )
       return;

    if ( ch->position < skill_table[sn].minimum_position )
    {
	send_to_char(AT_BLUE, "You can't concentrate enough.\n\r", ch );
	return;
    }
/*
    if ( ch->level < LEVEL_IMMORTAL && ch->class == CLASS_VAMPIRE )
     if ( !IS_SET( ch->in_room->room_flags, ROOM_INDOORS ) )
     {
      if ( time_info.hour > 6 && time_info.hour < 18 )
      {
	int      chance;

	chance = 15;
	if (number_percent ( )< chance)
	{
	 send_to_char(AT_RED, "The sun blocks your magic!\n\r", ch );
	 return;
	}
      }
    }
*/
    if ( IS_STUNNED( ch, STUN_MAGIC ) )
    {
      if ( prime_class(ch) != CLASS_BARD )
      send_to_char(AT_LBLUE, "You're too stunned to cast spells.\n\r", ch );
      else
      send_to_char(AT_LBLUE, "You're too stunned to sing songs.\n\r", ch );
      return;
    }

    if ( !IS_NPC( ch ) && ( !( ch->level > LEVEL_MORTAL ) ) )
    {
    mana = SPELL_COST( ch, sn );
    if ( ch->race == RACE_ELF || ch->race == RACE_ELDER )
       mana -= mana / 4;
    }
    else
    mana = 0;
    /*
     * Locate targets.
     */
    victim	= NULL;
    obj		= NULL;
    vo		= NULL;

    switch ( skill_table[sn].target )
    {
    default:
	bug( "Do_cast: bad target for sn %d.", sn );
	return;

    case TAR_GROUP_OFFENSIVE:
    case TAR_GROUP_DEFENSIVE:
    case TAR_GROUP_ALL:
    case TAR_GROUP_OBJ:
    case TAR_GROUP_IGNORE:
	group_cast( sn, URANGE( 1, ch->level, LEVEL_HERO ), ch, arg2 );
	return;

    case TAR_IGNORE:
	break;

    case TAR_CHAR_OFFENSIVE:
	if ( IS_SET( ch->in_room->room_flags, ROOM_NO_OFFENSIVE ) )
	{
		send_to_char( AT_BLUE, "You failed.\n\r", ch );
		return;
	}

	if ( arg2[0] == '\0' )
	{
	    if ( !( victim = ch->fighting ) )
	    {
		if ( prime_class(ch) != CLASS_BARD )
		send_to_char(AT_BLUE, "Cast the spell on whom?\n\r", ch );
		else
		send_to_char(AT_BLUE, "Sing the song to whom?\n\r", ch );
		return;
	    }
	}
	else
	{
	    if ( !( victim = get_char_room( ch, arg2 ) ) )
	    {
		send_to_char(AT_BLUE, "They aren't here.\n\r", ch );
		return;
	    }
	}
    if ( IS_AFFECTED(victim, AFF_PEACE) )
    {
      send_to_char(AT_WHITE, "A wave of peace overcomes you.\n\r", ch);
      return;
    }
    if ( IS_AFFECTED( ch, AFF_PEACE) )
    {
	    affect_strip( ch, skill_lookup("aura of peace") );
	    REMOVE_BIT( ch->affected_by, AFF_PEACE );
    }

	if (is_safe(ch, victim ) )
	{
	  send_to_char( AT_BLUE, "You failed.\n\r",ch);
	  return;
	}

	vo = (void *) victim;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( arg2[0] == '\0' )
	{
	    victim = ch;
	}
	else
	{
	    if ( !( victim = get_char_room( ch, arg2 ) ) )
	    {
		send_to_char(AT_BLUE, "They aren't here.\n\r", ch );
		return;
	    }
	}

	vo = (void *) victim;
	break;

    case TAR_CHAR_SELF:
	if ( arg2[0] != '\0' && !is_name( ch, arg2, ch->name ) )
	{
	    if ( prime_class(ch) != CLASS_BARD )
	    send_to_char(AT_BLUE, "You cannot cast this spell on another.\n\r", ch );
	    else
	    send_to_char(AT_BLUE, "You cannot sing this song for others.\n\r", ch );
	    return;
	}

	vo = (void *) ch;
	break;

    case TAR_OBJ_INV:
	if ( arg2[0] == '\0' )
	{
	    send_to_char(AT_BLUE, "What should the spell be cast upon?\n\r", ch );
	    return;
	}

        if ( !(obj = get_obj_here( ch, arg2 ) ) )
        {
	  send_to_char( AT_BLUE, "You can't find that.\n\r", ch );
          return;
        }
	vo = (void *) obj;
	break;
    }
    if ( !IS_NPC( ch ) )
    if ( !is_class( ch, CLASS_VAMPIRE ) && ch->mana < mana )
       {
   	send_to_char(AT_BLUE, "You don't have enough mana.\n\r", ch );
	return;
       }
    else
       if ( ( ch->bp < mana ) && ( is_class( ch, CLASS_VAMPIRE ) ) )
       {
   	send_to_char(AT_RED, "You are to starved to cast, you must feed.\n\r", ch );
	return;
       }


    if ( str_cmp( skill_table[sn].name, "ventriloquate" ) )
	say_spell( ch, sn );

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_MAGIC ) )
      {
       send_to_char( AT_BLUE, "You failed.\n\r", ch );
       return;
      }
    if ( ( IS_SET( ch->in_room->room_flags, ROOM_NO_OFFENSIVE ) ) && ( skill_table[sn].target == TAR_CHAR_OFFENSIVE ) )
      {
       send_to_char( AT_BLUE, "You failed.\n\r", ch );
       return;
      }
    WAIT_STATE( ch, skill_table[sn].beats );

    if ( !IS_NPC( ch ) )
    if ( number_percent( ) > ch->pcdata->learned[sn] )
    {
	send_to_char(AT_BLUE, "You lost your concentration.\n\r", ch );
  if ( is_class( ch, CLASS_VAMPIRE ) ) {
    ch->bp -= mana / 2;
  } else {
    ch->mana -= mana / 2;
  }
    }
    else
    {
      if ( is_class( ch, CLASS_VAMPIRE ) ) {
        ch->bp -= mana;
      } else {
        ch->mana -= mana;
      }
	if ( ( IS_AFFECTED2( ch, AFF_CONFUSED ) )
	    && number_percent( ) < 10 )
	{
	   act(AT_YELLOW, "$n looks around confused at what's going on.", ch, NULL, NULL, TO_ROOM );
	   send_to_char( AT_YELLOW, "You become confused and botch the spell.\n\r", ch );
	   return;
	}
	update_skpell( ch, sn );
	(*skill_table[sn].spell_fun) ( sn,
				      IS_DIVINE ?
				      URANGE( 1, ch->level, LEVEL_HERO )*3 :
				      URANGE( 1, ch->level, LEVEL_HERO ) ,
				      ch, vo );
    }

    if ( IS_NPC( ch ) )
     (*skill_table[sn].spell_fun) ( sn,
				      IS_DIVINE ?
				      URANGE( 1, ch->level, LEVEL_HERO )*3 :
				      URANGE( 1, ch->level, LEVEL_HERO ) ,
            			      ch, vo );

    if ( vo )
    {
      if ( skill_table[sn].target == TAR_OBJ_INV )
	oprog_cast_sn_trigger( obj, ch, sn, vo );
      rprog_cast_sn_trigger( ch->in_room, ch, sn, vo );
    }

    if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
	&& victim->master != ch && victim != ch && IS_AWAKE( victim ) )
    {
	CHAR_DATA *vch;

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted )
	        continue;
	    if ( victim == vch && !victim->fighting )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	}
    }

    return;
}



/*
 * Cast spells at targets using a magical object.
 */
void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim,
		    OBJ_DATA *obj )
{
    void *vo;

    if ( sn <= 0 )
	return;

    if ( !is_sn(sn) || skill_table[sn].spell_fun == 0 )
    {
	bug( "Obj_cast_spell: bad sn %d.", sn );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_MAGIC ) )
      {
       send_to_char( AT_BLUE, "The magic of the item fizzles.\n\r", ch );
       return;
      }

    switch ( skill_table[sn].target )
    {
    default:
	bug( "Obj_cast_spell: bad target for sn %d.", sn );
	return;

    case TAR_GROUP_OFFENSIVE:
    case TAR_GROUP_DEFENSIVE:
    case TAR_GROUP_ALL:
    case TAR_GROUP_OBJ:
    case TAR_GROUP_IGNORE:
	group_cast( sn, URANGE( 1, level, LEVEL_HERO ), ch,
		    victim ? (void *)victim : (void *)obj );
	return;

    case TAR_IGNORE:
	vo = NULL;
	break;

    case TAR_CHAR_OFFENSIVE:
    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_OFFENSIVE ) )
      {
       send_to_char( AT_BLUE, "The magic of the item fizzles.\n\r", ch );
       return;
      }
	if ( !victim )
	    victim = ch->fighting;
	if ( !victim || ( !IS_NPC( victim ) && ch != victim ) )
	{
	    send_to_char(AT_BLUE, "You can't do that.\n\r", ch );
	    return;
	}
	if ( ( ( ch->clan == 0 ) || ( ch->clan == 0 ) ) && ( !IS_NPC( victim ) ) )
 	   return;
    if ( IS_AFFECTED(victim, AFF_PEACE) )
    {
      send_to_char(AT_WHITE, "A wave of peace overcomes you.\n\r", ch);
      return;
    }
    if ( IS_AFFECTED( ch, AFF_PEACE) )
    {
	    affect_strip( ch, skill_lookup("aura of peace") );
	    REMOVE_BIT( ch->affected_by, AFF_PEACE );
    }
    if ( ( ( ch->level - 9 > victim->level )
      || ( ch->level + 9 < victim->level ) )
      && ( !IS_NPC(victim) ) )
    {
	send_to_char(AT_WHITE, "That is not in the pkill range... valid range is +/- 8 levels.\n\r", ch );
	return;
    }
	vo = (void *) victim;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( !victim )
	    victim = ch;
	vo = (void *) victim;
	break;

    case TAR_CHAR_SELF:
	vo = (void *) ch;
	break;

    case TAR_OBJ_INV:
	if ( !obj )
	{
	    send_to_char(AT_BLUE, "You can't do that.\n\r", ch );
	    return;
	}
	vo = (void *) obj;
	break;
    }
/*    target_name = "";*/
    (*skill_table[sn].spell_fun) ( sn, level, ch, vo );

    if ( vo )
    {
      if ( skill_table[sn].target == TAR_OBJ_INV )
	oprog_cast_sn_trigger( obj, ch, sn, vo );
      rprog_cast_sn_trigger( ch->in_room, ch, sn, vo );
    }

    if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
	&& victim->master != ch && ch != victim )
    {
	CHAR_DATA *vch;

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
	    if ( vch->deleted )
	        continue;
	    if ( victim == vch && !victim->fighting )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	}
    }

    return;
}



/*
 * Spell functions.
 */
void spell_acid_blast( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = dice( level, 8 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}

void spell_animate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA      *obj = (OBJ_DATA *) vo;
    OBJ_DATA      *obj_next;


    if ( obj->item_type != ITEM_CORPSE_NPC )
    {
      send_to_char(AT_BLUE, "You cannot animate that.\n\r", ch );
      return;
    }
 obj_next = obj->next;
 if (obj->deleted)
    return;
 magic_mob( ch, obj, obj->ac_vnum );
 extract_obj(obj);
 return;
}



void spell_armor( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 24;
    af.location  = APPLY_AC;
    af.modifier  = -25;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "You feel someone protecting you.\n\r", victim );
    return;
}

void spell_astral( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
    CHAR_DATA *pet;

    if ( !( victim = get_char_world( ch, target_name ) )
	|| IS_SET( victim->in_room->room_flags, ROOM_SAFE      )
	|| IS_SET( victim->in_room->room_flags, ROOM_PRIVATE   )
	|| IS_SET( victim->in_room->room_flags, ROOM_SOLITARY  )
	|| IS_SET( victim->in_room->room_flags, ROOM_NO_ASTRAL_IN )
	|| IS_SET( ch->in_room->room_flags, ROOM_NO_ASTRAL_OUT )
	|| IS_SET( victim->in_room->area->area_flags, AREA_PROTOTYPE )
	|| IS_SET( victim->act, ACT_NOASTRAL )
	|| IS_ARENA(ch)
	|| victim->in_room->area == arena.area
	|| IS_AFFECTED( victim, AFF_NOASTRAL ) )
    {
	send_to_char(AT_BLUE, "You failed.\n\r", ch );
	return;
    }

    for ( pet = ch->in_room->people; pet; pet = pet->next_in_room )
    {
      if ( IS_NPC( pet ) )
        if ( IS_SET( pet->act, ACT_PET ) && ( pet->master == ch ) )
          break;
    }

    act(AT_BLUE, "$n vanishes in a flash of blinding light.", ch, NULL, NULL, TO_ROOM );
    if ( ch != victim )
    {
     if ( pet )
     {
       act( AT_BLUE, "$n vanishes in a flash of blinding light.", pet, NULL, NULL, TO_ROOM );
       char_from_room( pet );
     }
     char_from_room( ch );
     char_to_room( ch, victim->in_room );
    }
    act(AT_BLUE, "$n appears in a flash of blinding light.", ch, NULL,
NULL, TO_ROOM );
    do_look( ch, "auto" );
    if ( pet )
    {
      char_to_room( pet, victim->in_room );
      act( AT_BLUE, "$n appears in a flash of blinding light.", pet, NULL, NULL, TO_ROOM );
    }
    return;
}

void spell_pass_plant ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim ;
    CHAR_DATA *pet;

    if ( IS_AFFECTED( ch, AFF_ANTI_FLEE ) )
    {
      send_to_char( AT_WHITE, "You cannot walk through the plants in your condition!\n\r", ch );
      return;
    }

    if (    ch->in_room->sector_type == SECT_INSIDE
         || ch->in_room->sector_type == SECT_CITY
         || ch->in_room->sector_type == SECT_AIR
       )
    {
	send_to_char(AT_GREEN, "There are no plants here.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_NO_SHADOW ) )
    {
        send_to_char(AT_GREEN, "The plants do not respond to your call.\n\r", ch);
        return;
    }

    if ( !( victim = get_char_world( ch, target_name ) )
        || victim->in_room->area != ch->in_room->area
        || IS_SET( victim->in_room->room_flags, ROOM_PRIVATE   )
	|| IS_SET( victim->in_room->room_flags, ROOM_SOLITARY  )
        || victim->in_room->sector_type == SECT_INSIDE
        || victim->in_room->sector_type == SECT_CITY
        || victim->in_room->sector_type == SECT_AIR
        || IS_AFFECTED( victim, AFF_NOASTRAL )  )
    {
        send_to_char(AT_GREEN, "The plants do not reach there.\n\r", ch );
        return;
    }

    for ( pet = ch->in_room->people; pet; pet = pet->next_in_room )
    {
      if ( IS_NPC( pet ) )
        if ( IS_SET( pet->act, ACT_PET ) && ( pet->master == ch ) )
          break;
    }


    if ( ch != victim )
    {
     act(AT_GREEN, "$n melts into the surrounding plant life.", ch, NULL, NULL, TO_ROOM );
     char_from_room( ch );
     char_to_room( ch, victim->in_room );
     act(AT_GREEN, "$n steps out of the surrounding plant life.", ch, NULL, NULL, TO_ROOM );

     if ( pet )
     {
      act( AT_BLUE, "$n melts into the surrounding plant life.", pet, NULL, NULL, TO_ROOM );
      char_from_room( pet );
      char_to_room( pet, victim->in_room );
      act( AT_BLUE, "$n steps out of the surrounding plant life.", pet, NULL, NULL, TO_ROOM );
     }
    }

    do_look( ch, "auto" );
    return;
}


void spell_aura( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_PEACE) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PEACE;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "You feel a wave of peace flow lightly over your body.\n\r", victim );
    act(AT_BLUE, "$n looks very peaceful.", victim, NULL, NULL, TO_ROOM );
    return;
}


void spell_bless( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 6 + level;
    af.location  = APPLY_HITROLL;
    af.modifier  = level / 8;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 0 - level / 8;
    affect_to_char( victim, &af );
    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "You feel righteous.\n\r", victim );
    return;
}

void spell_darkbless( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 20 + level;
    af.location  = APPLY_DAMROLL;
    af.modifier  = level / 6;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_HIT;
    af.modifier  = level * 3;
    affect_to_char( victim, &af );
    if ( ch != victim )
	send_to_char(AT_BLUE, "You call forth the hand of oblivion.\n\r", ch );
    send_to_char(AT_BLUE, "The hand of oblivion rests upon you.\n\r", victim );
    return;
}

void spell_aid( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 8 + level;
    af.location  = APPLY_HITROLL;
    af.modifier  = level / 6;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 0 - level / 6;
    affect_to_char( victim, &af );

    af.location  = APPLY_HIT;
    af.modifier  = level * 3;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char(AT_BLUE, "You grant them divine aid.\n\r", ch );
    send_to_char(AT_BLUE, "You feel divine aid reashure you.\n\r", victim );
    return;
}


void spell_bio_acceleration( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = 20 + level;
    af.location  = APPLY_HIT;
    af.modifier  = number_fuzzy ( level * 4 );
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_MOVE;
    af.modifier  = level * 2;
    affect_to_char( victim, &af );

    send_to_char( AT_BLUE, "You greatly enhance your bio-functions.\n\r", ch );
    act(AT_BLUE, "$n's body shudders briefly.", ch, NULL, NULL, TO_ROOM);
    return;
}

/*Decklarean*/
void spell_draw_strength( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = 20 + level;
    af.location  = APPLY_MANA;
    af.modifier  = number_fuzzy ( level * 4 );
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_HIT;
    af.modifier  = -af.modifier;
    affect_to_char( victim, &af );

    /* They still have hitpoints over there max hit points
       get ride of them */
    if ( MAX_HIT(victim) < victim->hit )
     victim->hit = MAX_HIT(victim);

    send_to_char( AT_BLUE, "You draw from your physical strength and increase your energy reserve.\n\r", ch );
    act(AT_BLUE, "$n's body weakens.", ch, NULL, NULL, TO_ROOM);
    return;
}


/*
void spell_barkskin( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED( victim, sn))
    return;
     af.type       = sn;
     af.duration   = 35;
     af.location   = APPLY_AC;
     af.modifier   = -30;
     af.bitvector  = 0;
     affect_to_char( victim, &af);

     af.location = APPLY_SAVING_SPELL;
     af.modifier   = 0 - LEVEL / 12;
     affect_to_char( victim, &af);
     if ( ch != victim )
     send_to_char( "Ok.\n\r", ch );
      send_to_char( "Your skin turns into a bark-like texture.\n\r",
victim, );
     return;
}
*/

void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED( victim, AFF_BLIND ) || saves_spell( level, victim ) )
    {
	send_to_char(AT_BLUE, "You have failed.\n\r", ch );
	return;
    }

    af.type      = sn;
    af.level	 = level;
    af.duration  = 5;
    af.location  = APPLY_HITROLL;
    af.modifier  = -10;
    af.bitvector = AFF_BLIND;
    affect_to_char( victim, &af );

    act(AT_WHITE, "$N is blinded!", ch, NULL, victim, TO_CHAR    );
    send_to_char(AT_WHITE, "You are blinded!\n\r", victim );
    act(AT_WHITE, "$N is blinded!", ch, NULL, victim, TO_NOTVICT );
    return;
}

void spell_blood_bath( int sn, int level, CHAR_DATA *ch, void *vo )
{
     CHAR_DATA  *victim;

    if ( blood_count( ch->in_room->contents, 5 ) < 2 )
       {
        send_to_char( AT_RED, "There is not enough blood in the room.\n\r", ch );
	return;
       }
    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if ( victim->deleted )
	    continue;

	if ( IS_NPC( victim ) )
	    continue;

	victim->hit = UMIN( victim->hit + 250, MAX_HIT(victim) );
        update_pos( victim );
        act( AT_RED, "You bath $N in the life giving fluid.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n baths $N in blood.", ch, NULL, victim, TO_ROOM );
	act( AT_RED, "$n baths you in blood.", ch, NULL, victim, TO_VICT );
    }
    send_to_char( AT_RED, "The blood bath is over.\n\r", ch );
    return;
}


void spell_burning_hands( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	 0,
	 0,  0,  0,  0,	14,	17, 20, 23, 26, 29,
	29, 29, 30, 30,	31,	31, 32, 32, 33, 33,
	34, 34, 35, 35,	36,	36, 37, 37, 38, 38,
	39, 39, 40, 40,	41,	41, 42, 42, 43, 43,
	44, 44, 45, 45,	46,	46, 47, 47, 48, 48,
	48, 48, 49, 49,	49,	49, 50, 50, 50, 51,
	51, 51, 52, 52,	52,	53, 53, 53, 54, 54,
	54, 54, 54, 54,	55,	55, 55, 55, 55, 55,
	56, 56, 56, 56,	56,	57, 57, 58, 59, 60
    };
                 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_call_lightning( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    int        dam;

    if ( !IS_OUTSIDE( ch ) )
    {
	send_to_char(AT_WHITE, "You must be out of doors.\n\r", ch );
	return;
    }

    if ( weather_info.sky < SKY_RAINING )
    {
	send_to_char(AT_WHITE, "You need bad weather.\n\r", ch );
	return;
    }

    dam = dice( level / 2, 8 );
    send_to_char(AT_WHITE, "Lightning slashes out of the sky to strike your foes!\n\r", ch );
    act(AT_WHITE, "$n calls lightning from the sky to strike $s foes!",
	ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch; vch = vch->next )
    {
	if ( vch->deleted || !vch->in_room )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch
		&& ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
		{
		dam = sc_dam( ch, dam );
		damage( ch, vch, saves_spell( level, vch ) ? dam/2 : dam, sn );
		}
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area
	    && IS_OUTSIDE( vch )
	    && IS_AWAKE( vch ) )
	    send_to_char(AT_LBLUE, "Lightning flashes in the sky.\n\r", vch );
    }

    return;
}



void spell_cause_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int dam = dice( 1, 8 ) + level / 3;
    dam = sc_dam( ch, dam );
    damage( ch, (CHAR_DATA *) vo, dam, sn );
    return;
}



void spell_cause_critical( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int dam = dice( 3, 8 ) + level - 6;
    dam = sc_dam( ch, dam );
    damage( ch, (CHAR_DATA *) vo, dam, sn );
    return;
}



void spell_cause_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int dam = dice( 2, 8 ) + level / 2;
    dam = sc_dam( ch, dam );
    damage( ch, (CHAR_DATA *) vo, dam, sn );
    return;
}



void spell_change_sex( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    af.type      = sn;
    af.level	 = level;
    af.duration  = 10 * level;
    af.location  = APPLY_SEX;
    do
    {
	af.modifier  = number_range( 0, 2 ) - victim->sex;
    }
    while ( af.modifier == 0 );
    af.bitvector = 0;
    affect_to_char( victim, &af );
    if ( ch != victim )
	send_to_char(AT_WHITE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "You feel different.\n\r", victim );
    return;
}

void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim == ch )
    {
	send_to_char(AT_BLUE, "You like yourself even better!\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
       return;
    if (   IS_AFFECTED( victim, AFF_CHARM )
	|| IS_AFFECTED( ch,     AFF_CHARM )
	|| level < victim->level
	|| saves_spell( level, victim ) )
	return;

    if(IS_SIMM(victim, IMM_CHARM))
      return;

    if ( victim->master )
	stop_follower( victim );
    add_follower( victim, ch );
    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 6 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "Ok.\n\r", ch );
    act(AT_BLUE, "Isn't $n just so nice?", ch, NULL, victim, TO_VICT );
    return;
}



void spell_chill_touch( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA  *victim       = (CHAR_DATA *) vo;
		 AFFECT_DATA af;
    static const int         dam_each [ ] =
    {
	 0,
	 0,  0,  6,  7,  8,	 9, 12, 13, 13, 13,
	14, 14, 14, 15, 15,	15, 16, 16, 16, 17,
	17, 17, 18, 18, 18,	19, 19, 19, 20, 20,
	20, 21, 21, 21, 22,	22, 22, 23, 23, 23,
	24, 24, 24, 25, 25,	25, 26, 26, 26, 27,
	27, 27, 27, 28, 28,	28, 29, 29, 29, 30,
	30, 30, 31, 31, 31,	32, 32, 33, 33, 33,
	34, 34, 34, 35, 35,	35, 36, 36, 36, 37,
	37, 37, 37, 37, 38,	38, 38, 38, 39, 39,
	39, 39, 39, 40, 40,	40, 41, 41, 42, 43
    };
		 int         dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    dam = sc_dam( ch, dam );
    if ( !saves_spell( level, victim ) )
    {
	af.type      = sn;
	af.level     = level;
	af.duration  = 6;
	af.location  = APPLY_STR;
	af.modifier  = -1;
	af.bitvector = 0;
	affect_join( victim, &af );
    }
    else
    {
	dam /= 2;
    }

    damage( ch, victim, dam, sn );
    return;
}



void spell_colour_spray( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	 0,
	 0,  0,  0,  0,  0,	 0,  0,  0,  0,  0,
	30, 35, 40, 45, 50,	55, 55, 55, 56, 57,
	58, 58, 59, 60, 61,	61, 62, 63, 64, 64,
	65, 66, 67, 67, 68,	69, 70, 70, 71, 72,
	73, 73, 74, 75, 76,	76, 77, 78, 79, 79,
        79, 80, 80, 81, 81,	82, 82, 83, 83, 84,
        84, 85, 85, 86, 86,	87, 87, 88, 88, 90,
	90, 91, 91, 92, 92,	93, 93, 94, 94, 95,
	95, 96, 96, 97, 97,	98, 98, 99, 99, 100,
	100,101,102,102,103,	104,105,106,107,120
    };

    int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn );
    return;
}



void spell_continual_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *light;

    light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), 0 );
    obj_to_room( light, ch->in_room );

    act(AT_BLUE, "You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR );
    act(AT_BLUE, "$n twiddles $s thumbs and $p appears.",   ch, light, NULL, TO_ROOM );
    return;
}



void spell_control_weather( int sn, int level, CHAR_DATA *ch, void *vo )
{
    if ( !str_cmp( target_name, "better" ) )
	weather_info.change += dice( level / 3, 4 );
    else if ( !str_cmp( target_name, "worse" ) )
	weather_info.change -= dice( level / 3, 4 );
    else
	send_to_char (AT_BLUE, "Do you want it to get better or worse?\n\r", ch );

    send_to_char(AT_BLUE, "Ok.\n\r", ch );
    return;
}



void spell_create_food( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *mushroom;

    mushroom = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), 0 );
    mushroom->value[0] = 5 + level;
    obj_to_room( mushroom, ch->in_room );

    act(AT_ORANGE, "$p suddenly appears.", ch, mushroom, NULL, TO_CHAR );
    act(AT_ORANGE, "$p suddenly appears.", ch, mushroom, NULL, TO_ROOM );
    return;
}



void spell_create_spring( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *spring;

    spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), 0 );
    spring->timer = level;
    obj_to_room( spring, ch->in_room );

    act(AT_BLUE, "$p flows from the ground.", ch, spring, NULL, TO_CHAR );
    act(AT_BLUE, "$p flows from the ground.", ch, spring, NULL, TO_ROOM );
    return;
}



void spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj   = (OBJ_DATA *) vo;
    int       water;

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char(AT_BLUE, "It is unable to hold water.\n\r", ch );
	return;
    }

    if ( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
    {
	send_to_char(AT_BLUE, "It contains some other liquid.\n\r", ch );
	return;
    }

    water = UMIN( level * ( weather_info.sky >= SKY_RAINING ? 4 : 2 ),
		 obj->value[0] - obj->value[1] );

    if ( water > 0 )
    {
	obj->value[2] = LIQ_WATER;
	obj->value[1] += water;
	if ( !is_name( NULL, "water", obj->name ) )
	{
	    char buf [ MAX_STRING_LENGTH ];

	    sprintf( buf, "%s water", obj->name );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	}
	act(AT_BLUE, "$p is filled.", ch, obj, NULL, TO_CHAR );
    }

    return;
}



void spell_cure_blindness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !is_affected( victim, gsn_blindness ) )
	return;

    affect_strip( victim, gsn_blindness );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_WHITE, "Your vision returns!\n\r", victim );
    return;
}



void spell_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    heal = dice( 3, 8 ) + level - 6;
    victim->hit = UMIN( victim->hit + heal, MAX_HIT(victim)) ;
    update_pos( victim );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "You feel better!\n\r", victim );
    return;
}



void spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    heal = dice( 1, 8 ) + level / 3;
    victim->hit = UMIN( victim->hit + heal, MAX_HIT(victim));
    update_pos( victim );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "You feel better!\n\r", victim );
    return;
}



void spell_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( !is_affected( victim, gsn_poison ) )
        return;

    affect_strip( victim, gsn_poison );

    send_to_char(AT_GREEN, "Ok.\n\r",                                    ch     );
    send_to_char(AT_GREEN, "A warm feeling runs through your body.\n\r", victim );
    act(AT_GREEN, "$N looks better.", ch, NULL, victim, TO_NOTVICT );

    return;
}



void spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        heal;

    heal = dice( 2, 8 ) + level / 2 ;
    victim->hit = UMIN( victim->hit + heal,  MAX_HIT(victim));
    update_pos( victim );
    send_to_char(AT_BLUE, "You feel better!\n\r", victim );
    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    return;
}



void spell_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_CURSE ) || saves_spell( level, victim ) )
    {
	send_to_char(AT_RED, "You have failed.\n\r", ch );
	return;
    }

    af.type      = sn;
    af.level	 = level;
    af.duration  = 4 * level;
    af.location  = APPLY_HITROLL;
    af.modifier  = -1;
    af.bitvector = AFF_CURSE;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 1;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char(AT_RED, "Ok.\n\r", ch );
    send_to_char(AT_RED, "You feel unclean.\n\r", victim );
    return;
}



void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_DETECT_EVIL ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_EVIL;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "Your eyes tingle.\n\r", victim );
    return;
}



void spell_detect_hidden( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_DETECT_HIDDEN ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "Your awareness improves.\n\r", victim );
    return;
}



void spell_detect_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_DETECT_INVIS ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_INVIS;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "Your eyes tingle.\n\r", victim );
    return;
}

void spell_truesight( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED2( victim, AFF_TRUESIGHT ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level / 8;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_TRUESIGHT;
    affect_to_char2( victim, &af );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "Your eyes tingle.\n\r", victim );
    return;
}



void spell_detect_magic( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_DETECT_MAGIC ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_MAGIC;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "Your eyes tingle.\n\r", victim );
    return;
}



void spell_detect_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
    {
	if ( obj->value[3] != 0 )
	    send_to_char(AT_GREEN, "You smell poisonous fumes.\n\r", ch );
	else
	    send_to_char(AT_GREEN, "It looks very delicious.\n\r", ch );
    }
    else
    {
	send_to_char(AT_GREEN, "It looks very delicious.\n\r", ch );
    }

    return;
}


/* Dispel Magic recoded by Thelonius for EnvyMud
look for the improved version below
void spell_dispel_magic ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA   *victim = (CHAR_DATA *) vo;
    AFFECT_DATA *paf;

    if ( (victim == ch) || !IS_NPC( ch )
	&& ( !IS_NPC( victim )
	    || ( IS_AFFECTED( victim, AFF_CHARM )
		&&   is_same_group( ch, victim->master ) ) ) )
    {
	for ( paf = victim->affected; paf; paf = paf->next )
	{
	    if ( paf->deleted )
	        continue;

	    affect_remove( victim, paf );

	    }
	}

	if ( victim == ch )
	{
	    act(AT_BLUE, "You have removed all magic effects from yourself.",
			ch, NULL, NULL, TO_CHAR );
	    act(AT_BLUE, "$n has removed all magic effects from $mself.",
			ch, NULL, NULL, TO_ROOM );
	}
	else
	{
	    act(AT_BLUE, "You have removed all magic effects from $N.",
			ch, NULL, victim, TO_CHAR );
	    act(AT_BLUE, "$n has removed all magic effects from you.",
			ch, NULL, victim, TO_VICT );
	    act(AT_BLUE, "$n has removed all magic effects from $N.",
			ch, NULL, victim, TO_NOTVICT );
	}
	return;
    }
    else
    {
	for ( paf = victim->affected; paf; paf = paf->next )
	{
	    if ( paf->deleted )
	        continue;
	    if ( !saves_spell( level, victim ) )
	    {
		send_to_char(AT_BLUE, skill_table[paf->type].msg_off, victim );
		send_to_char(AT_BLUE, "\n\r", victim );
		act(AT_BLUE, "$n is no longer affected by '$t'.",
			victim, skill_table[paf->type].name, NULL, TO_ROOM );
		affect_strip( victim, paf->type );
		break;
	    }
	}
	if ( IS_AFFECTED( victim, AFF_SANCTUARY)
	    && !saves_spell( level, victim ) )
	{
	    REMOVE_BIT( victim->affected_by, AFF_SANCTUARY );
	    send_to_char(AT_GREY, "The white aura around your body fades.\n\r",
								victim );
	    act(AT_GREY, "The white aura around $n's body fades.",
			victim, NULL, NULL, TO_ROOM );
	}
	if ( IS_AFFECTED( victim, AFF_FIRESHIELD)
	    && !saves_spell( level, victim ) )
	{
	    REMOVE_BIT( victim->affected_by, AFF_FIRESHIELD );
	    send_to_char(AT_RED, "The flames about your body have been doused.\n\r",
								victim );
	    act(AT_RED, "The flames about $n's body burn out.",
			victim, NULL, NULL, TO_ROOM );
	}
	if ( IS_AFFECTED( victim, AFF_SHOCKSHIELD)
	    && !saves_spell( level, victim ) )
	{
	    REMOVE_BIT( victim->affected_by, AFF_SHOCKSHIELD );
	    send_to_char(AT_BLUE, "The electricity about your body flee's into the ground..\n\r",
								victim );
	    act(AT_BLUE, "The electricity about $n's body flee's into the ground.",
			victim, NULL, NULL, TO_ROOM );
	}
	if ( IS_AFFECTED( victim, AFF_ICESHIELD)
	    && !saves_spell( level, victim ) )
	{
	    REMOVE_BIT( victim->affected_by, AFF_ICESHIELD );
	    send_to_char(AT_LBLUE, "The icy crust about your body melts to a puddle..\n\r",
								victim );
	    act(AT_LBLUE, "The icy crust about $n's body melts to a puddle.",
			victim, NULL, NULL, TO_ROOM );
	}
	if ( !victim->fighting || !is_same_group( ch, victim->fighting ) )
	  multi_hit( victim, ch, TYPE_UNDEFINED );
    }
    return;
}
*/

void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    if ( !IS_NPC( ch ) && IS_EVIL( ch ) )
    {
	send_to_char(AT_RED, "You are too EVIL to cast this.\n\r", ch );
	return;
    }

    if ( IS_GOOD( victim ) )
    {
	act(AT_BLUE, "God protects $N.", ch, NULL, victim, TO_ROOM );
	return;
    }

    if ( IS_NEUTRAL( victim ) )
    {
	act(AT_BLUE, "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
	return;
    }

    dam = dice( level, 4 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_earthquake( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;

    send_to_char(AT_ORANGE, "The earth trembles beneath your feet!\n\r", ch );
    act(AT_ORANGE, "$n makes the earth tremble and shiver.", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch; vch = vch->next )
    {
        if ( vch->deleted || !vch->in_room )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch )
			                     :  IS_NPC( vch ) ) )
		damage( ch, vch, level + dice( 2, 8 ), sn );
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char(AT_ORANGE, "The earth trembles and shivers.\n\r", vch );
    }

    return;
}

void spell_chain_lightning( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;

    send_to_char(AT_BLUE, "Bolts of electricity arc from your hands!\n\r", ch );
    act(AT_BLUE, "Electrical energy bursts from $n's hands.", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch; vch = vch->next )
    {
	if ( vch->deleted || !vch->in_room )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch )
					     :  IS_NPC( vch ) ) )
		damage( ch, vch, level + dice( level, 6 ), sn );
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char(AT_BLUE, "The air fills with static.\n\r", vch );
    }

    return;
}


/* RT version of chain
void spell_chain_lightning(int sn, int level, CHAR_DATA *ch, void *vo)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *tmp_vict,*last_vict,*next_vict;
    bool found;
    int dam;

    * first strike *

    act(AT_BLUE, "A lightning bolt leaps from $n's hand and arcs to $N.",
        ch,NULL,victim,TO_ROOM);
    act(AT_BLUE, "A lightning bolt leaps from your hand and arcs to $N.",
	ch,NULL,victim,TO_CHAR);
    act(AT_BLUE, "A lightning bolt leaps from $n's hand and hits you!",
	ch,NULL,victim,TO_VICT);

    dam = level + dice(level,6);
    if (saves_spell(level,victim))
	dam /= 3;
    damage(ch,victim,dam,sn);*,DAM_LIGHTNING);*
    last_vict = victim;
    level -= 4;		* decrement damage *

    * new targets *
    while (level > 0)
    {
	found = FALSE;
	for (tmp_vict = ch->in_room->people;
	     tmp_vict != NULL;
	     tmp_vict = next_vict)
	{
if(tmp_vect->deleted) continue;
	  next_vict = tmp_vict->next_in_room;
	  if (*!is_safe_spell(ch,tmp_vict,TRUE) &&* tmp_vict != last_vict)
	  {
	    found = TRUE;
	    last_vict = tmp_vict;
	    act(AT_BLUE, "The bolt arcs to $n!",tmp_vict,NULL,NULL,TO_ROOM);
	    act(AT_BLUE, "The bolt hits you!",tmp_vict,NULL,NULL,TO_CHAR);
	    dam = level + dice(level,6);
	    if (saves_spell(level,tmp_vict))
		dam /= 3;
	    damage(ch,tmp_vict,dam,sn);*,DAM_LIGHTNING);*
	    level -= 4;  * decrement damage *
	  }
	}   * end target searching loop *

	if (!found) * no target found, hit the caster *
	{
	  if (ch == NULL)
     	    return;

	  if (last_vict == ch) * no double hits *
	  {
	    act(AT_BLUE,
	     "The bolt seems to have fizzled out.",ch,NULL,NULL,TO_ROOM);
	    act(AT_BLUE, "The bolt grounds out through your body.",
		ch,NULL,NULL,TO_CHAR);
	    return;
	  }

	  last_vict = ch;
	  act(AT_BLUE, "The bolt arcs to $n...whoops!",ch,NULL,NULL,TO_ROOM);
	  send_to_char(AT_BLUE, "You are struck by your own lightning!\n\r",ch);
	  dam = level + dice(level,6);
	  if (saves_spell(level,ch))
	    dam /= 3;
	  damage(ch,ch,dam,sn);*,DAM_LIGHTNING);*
	  level -= 4;  * decrement damage *
	  if (ch == NULL)
	    return;
	}
    * now go back and find more targets *
    }
}
*/

void spell_meteor_swarm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
   AFFECT_DATA af;

    send_to_char(AT_RED, "Flaming meteors fly forth from your outstreched hands!\n\r", ch );
    act(AT_RED, "Hundreds of flaming meteors fly forth from $n's hands.", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch; vch = vch->next )
    {
        if ( vch->deleted || !vch->in_room )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch )
			                     :  IS_NPC( vch ) ) )
		damage( ch, vch, level + dice( level, 8 ), sn );
            if ( vch != ch )
            {
                af.type      = sn;
                af.level     = level;
                af.duration  = level / 8;
                af.location  = APPLY_NONE;
                af.modifier  = 0;
                af.bitvector = AFF_FLAMING;
                affect_join( vch, &af );
	        send_to_char(AT_RED, "You body bursts into flame!\n\r", vch);
	     }
	    continue;
	}

    }

    return;
}


void spell_enchant_weapon( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    if ( obj->item_type != ITEM_WEAPON
	|| IS_OBJ_STAT( obj, ITEM_MAGIC )
	|| ( obj->affected && !IS_OBJ_STAT( obj, ITEM_DWARVEN ) ) )
    {
	send_to_char(AT_BLUE, "That item cannot be enchanted.\n\r", ch );
	return;
    }

    if ( !affect_free )
    {
	paf		= alloc_perm( sizeof( *paf ) );
    }
    else
    {
	paf		= affect_free;
	affect_free	= affect_free->next;
    }

    paf->type		= sn;
    paf->duration	= -1;
    paf->location	= APPLY_HITROLL;
    paf->modifier	= 1 + (level >= 18) + (level >= 25) + (level >= 45) + (level >= 65) +(level >= 90);
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;

    if ( !affect_free )
    {
	paf		= alloc_perm( sizeof( *paf ) );
    }
    else
    {
	paf		= affect_free;
	affect_free	= affect_free->next;
    }

    paf->type		= sn;
    paf->duration	= -1;
    paf->location	= APPLY_DAMROLL;
    paf->modifier	= 1 + (level >= 18) + (level >= 25) + (level >= 45) + (level >= 65) +(level >= 90);;
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;

    if ( IS_GOOD( ch ) )
    {
	SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL);
	act(AT_BLUE, "$p glows.",   ch, obj, NULL, TO_CHAR );
    }
    else if ( IS_EVIL( ch ) )
    {
	SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
	act(AT_RED, "$p glows",    ch, obj, NULL, TO_CHAR );
    }
    else
    {
	SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
	SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
	act(AT_YELLOW, "$p glows.", ch, obj, NULL, TO_CHAR );
    }

    send_to_char(AT_BLUE, "Ok.\n\r", ch );
    return;
}

void spell_flame_blade( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;

    if ( obj->item_type != ITEM_WEAPON
	|| IS_OBJ_STAT( obj, ITEM_MAGIC )
	|| IS_OBJ_STAT( obj, ITEM_FLAME )
	|| ( obj->affected && !IS_OBJ_STAT( obj, ITEM_DWARVEN ) ) )
    {
	send_to_char(AT_RED, "That item cannot be enchanted.\n\r", ch );
	return;
    }
    SET_BIT( obj->extra_flags, ITEM_MAGIC);
    SET_BIT( obj->extra_flags, ITEM_FLAME );
    send_to_char(AT_RED, "Ok.\n\r", ch );
    return;
}

void spell_chaos_blade( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;

    if ( obj->item_type != ITEM_WEAPON
	|| IS_OBJ_STAT( obj, ITEM_MAGIC )
	|| IS_OBJ_STAT( obj, ITEM_CHAOS )
	|| ( obj->affected && !IS_OBJ_STAT( obj, ITEM_DWARVEN ) ) )
    {
	send_to_char(AT_YELLOW, "That item cannot be enchanted.\n\r", ch );
	return;
    }
    SET_BIT( obj->extra_flags, ITEM_MAGIC);
    SET_BIT( obj->extra_flags, ITEM_CHAOS );
    send_to_char(AT_YELLOW, "Ok.\n\r", ch );
    return;
}

void spell_frost_blade( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;

    if ( obj->item_type != ITEM_WEAPON
	|| IS_OBJ_STAT( obj, ITEM_MAGIC )
	|| IS_OBJ_STAT( obj, ITEM_ICY )
	|| ( obj->affected && !IS_OBJ_STAT( obj, ITEM_DWARVEN ) ) )
    {
	send_to_char(AT_LBLUE, "That item cannot be enchanted.\n\r", ch );
	return;
    }
    SET_BIT( obj->extra_flags, ITEM_MAGIC);
    SET_BIT( obj->extra_flags, ITEM_ICY );
    send_to_char(AT_LBLUE, "Ok.\n\r", ch );
    return;
}
void spell_holysword( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    if ( obj->item_type != ITEM_WEAPON
	|| IS_OBJ_STAT( obj, ITEM_MAGIC )
	|| ( obj->affected && !IS_OBJ_STAT( obj, ITEM_DWARVEN ) ) )
    {
	send_to_char(AT_BLUE, "That item cannot be consecrated.\n\r", ch );
	return;
    }

    if ( !affect_free )
    {
	paf		= alloc_perm( sizeof( *paf ) );
    }
    else
    {
	paf		= affect_free;
	affect_free	= affect_free->next;
    }

    paf->type		= sn;
    paf->duration	= -1;
    paf->location	= APPLY_HITROLL;
    paf->modifier	= 6 + (level >= 18) + (level >= 25) + (level >= 40) + (level >= 60) +(level >= 90);
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;

    if ( !affect_free )
    {
	paf		= alloc_perm( sizeof( *paf ));
    }
    else
    {
	paf		= affect_free;
	affect_free	= affect_free->next;
    }

    paf->type		= sn;
    paf->duration	= -1;
    paf->location	= APPLY_DAMROLL;
    paf->modifier	= 6 + (level >= 18) + (level >= 25) + (level >= 45) + (level >= 65) +(level >= 90);;
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;

    if ( IS_GOOD( ch ) )
    {
	SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL);
	act(AT_BLUE, "$p glows.",   ch, obj, NULL, TO_CHAR );
    }
    else if ( IS_EVIL( ch ) )
    {
	SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
	act(AT_RED, "$p glows",    ch, obj, NULL, TO_CHAR );
    }
    else
    {
	SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
	SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
	act(AT_YELLOW, "$p glows.", ch, obj, NULL, TO_CHAR );
    }
    SET_BIT( obj->anti_class_flags, ITEM_ANTI_WARRIOR );
    SET_BIT( obj->anti_class_flags, ITEM_ANTI_MAGE );
    SET_BIT( obj->anti_class_flags, ITEM_ANTI_BARD );
    SET_BIT( obj->anti_class_flags, ITEM_ANTI_RANGER );
    SET_BIT( obj->anti_class_flags, ITEM_ANTI_VAMP );
    SET_BIT( obj->anti_class_flags, ITEM_ANTI_DRUID );
    SET_BIT( obj->anti_class_flags, ITEM_ANTI_PSI );
    SET_BIT( obj->anti_class_flags, ITEM_ANTI_THIEF );
    SET_BIT( obj->anti_class_flags, ITEM_ANTI_NECRO );
    SET_BIT( obj->anti_class_flags, ITEM_ANTI_WWF );
    SET_BIT( obj->anti_class_flags, ITEM_ANTI_MONK );
    send_to_char(AT_BLUE, "Ok.\n\r", ch );
    return;
}


/*
 * Drain XP, MANA, HP.
 * Caster gains HP.
 */
void spell_energy_drain( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    if ( saves_spell( level, victim ) )
	return;

    /* ch->alignment = UMAX(-1000, ch->alignment - 200); */
    if ( victim->level <= 2 )
    {
	dam		 = ch->hit + 1;
    }
    else
    {
	victim->mana	/= 2;
	victim->move	/= 2;
	dam		 = dice( 4, level );
	if ( ( ch->hit + dam ) > ( MAX_HIT(ch) + 200 ) )
	    ch->hit = ( MAX_HIT(ch) + 200 );
	 else
	  ch->hit		+= dam;
    }
    dam = sc_dam( ch, dam );
    damage( ch, victim, dam, sn );

    return;
}

void spell_psychic_quake( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;

    send_to_char(AT_YELLOW, "You let the chaos free from your mind!\n\r", ch );
    act(AT_YELLOW, "$n's face becomes blank and concentrated.", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch; vch = vch->next )
    {
        if ( vch->deleted || !vch->in_room )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch )
			                     :  IS_NPC( vch ) ) )
		if (vch != ch )
		    spell_energy_drain(sn, level/2, ch, vch);
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char(AT_BLUE, "A wave of chaos brushes your mind.\n\r", vch );
    }

    return;
}

void spell_farsight( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;
   ROOM_INDEX_DATA *blah;

    if ( !( victim = get_char_world( ch, target_name ) )
	|| IS_SET( victim->in_room->room_flags, ROOM_SAFE      )
	|| IS_SET( victim->in_room->room_flags, ROOM_PRIVATE   )
	|| IS_SET( victim->in_room->room_flags, ROOM_SOLITARY  )
	|| IS_SET( victim->in_room->room_flags, ROOM_NO_ASTRAL_IN )
	|| IS_SET( ch->in_room->room_flags, ROOM_NO_ASTRAL_OUT ) )
    {
	send_to_char(AT_BLUE, "You failed.\n\r", ch );
	return;
    }

    blah = ch->in_room;
    if ( ch != victim )
    {
     char_from_room( ch );
     char_to_room( ch, victim->in_room );
    }
    do_look( ch, "auto" );
    if (ch != victim )
    {
      char_from_room( ch );
      char_to_room( ch, blah );
     }
    return;
}


void spell_fireball( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,  30,	 35,  40,  45,  50,  55,
	 60,  65,  70,  75,  80,	 82,  84,  86,  88,  90,
	 92,  94,  96,  98, 100,	102, 104, 106, 108, 110,
	112, 114, 116, 118, 120,	122, 124, 126, 128, 130,
	132, 134, 136, 138, 140,	142, 144, 146, 148, 150,
	152, 154, 156, 158, 160,	162, 164, 166, 168, 170,
	172, 174, 176, 178, 180,	182, 184, 186, 188, 190,
	192, 194, 196, 198, 200,	202, 204, 206, 208, 210,
	215, 220, 225, 230, 235,	240, 245, 250, 255, 260
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	= number_range( dam_each[level] / 2, dam_each[level] * 7 );
    dam = sc_dam( ch, dam );
    if ( IS_AFFECTED( victim, AFF_FIRESHIELD ) )
       dam /= 6;
    if ( IS_AFFECTED( victim, AFF_ICESHIELD ) )
       dam += dam/2;
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}

void spell_molecular_unbind( int sn, int level, CHAR_DATA *ch, void *vo )
{
   CHAR_DATA *victim       = (CHAR_DATA *) vo;
   OBJ_DATA  *obj_lose;
   OBJ_DATA  *obj_next;

   if(saves_spell ( level, victim ))
     {
       send_to_char(AT_BLUE, "You failed.\n\r", ch );
       return;
     }

	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
            char *msg;

	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted )
	        continue;
	    if ( obj_lose->wear_loc == WEAR_NONE )
	        continue;
	    if ( IS_SET( obj_lose->extra_flags, ITEM_NO_DAMAGE ) )
	        continue;
       	    switch ( obj_lose->item_type )
	    {
	    default:
	      msg = "Your $p gets ruined!";
	      extract_obj( obj_lose );
	      break;
	    case ITEM_DRINK_CON:
	    case ITEM_POTION:
	    case ITEM_CONTAINER:
	    case ITEM_LIGHT:
	      msg = "Your $p shatters!";
	      extract_obj( obj_lose );
	      break;
            case ITEM_WEAPON:
	    case ITEM_ARMOR:
              {
              OBJ_DATA       *pObj;
              OBJ_INDEX_DATA *pObjIndex;
              char           *name;
              char           buf[MAX_STRING_LENGTH];

              	    pObjIndex = get_obj_index(4);
              	    pObj = create_object(pObjIndex, obj_lose->level);
              	    name = obj_lose->short_descr;
              	    sprintf(buf, pObj->description, name);
              	    free_string(pObj->description);
              	    pObj->description = str_dup(buf);
              	    pObj->weight = obj_lose->weight;
              	    pObj->timer = obj_lose->level;
              	    msg = "$p has been destroyed!";
              	    extract_obj( obj_lose );
              	    obj_to_room ( pObj, victim->in_room );
                 	  break;

	    act(AT_YELLOW, msg, victim, obj_lose, NULL, TO_CHAR );
  	}
       }
      }
    return;
}

void spell_shatter( int sn, int level, CHAR_DATA *ch, void *vo )
{
   CHAR_DATA *victim       = (CHAR_DATA *) vo;
   OBJ_DATA  *obj_lose;
   OBJ_DATA  *obj_next;

   if(saves_spell ( level, victim ))
     {
       send_to_char(AT_BLUE, "You failed.\n\r", ch );
       return;
     }

	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
            char *msg;

	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted )
	        continue;
	    if ( obj_lose->wear_loc == WEAR_NONE )
	        continue;
	    if ( IS_SET( obj_lose->extra_flags, ITEM_NO_DAMAGE ) )
	        continue;
       	    switch ( obj_lose->item_type )
	    {
	    default:
	      msg = "Your $p gets ruined!";
	      extract_obj( obj_lose );
	      break;
	    case ITEM_DRINK_CON:
	    case ITEM_POTION:
	    case ITEM_CONTAINER:
	    case ITEM_LIGHT:
	      msg = "Your $p shatters!";
	      extract_obj( obj_lose );
	      break;
            case ITEM_WEAPON:
	    case ITEM_ARMOR:
              {
              OBJ_DATA       *pObj;
              OBJ_INDEX_DATA *pObjIndex;
              char           *name;
              char           buf[MAX_STRING_LENGTH];

              	    pObjIndex = get_obj_index(4);
              	    pObj = create_object(pObjIndex, obj_lose->level);
              	    name = obj_lose->short_descr;
              	    sprintf(buf, pObj->description, name);
              	    free_string(pObj->description);
              	    pObj->description = str_dup(buf);
              	    pObj->weight = obj_lose->weight;
              	    pObj->timer = obj_lose->level;
              	    msg = "$p has been destroyed!";
              	    extract_obj( obj_lose );
              	    obj_to_room ( pObj, victim->in_room );
                 	  break;

      	    act(AT_YELLOW, msg, victim, obj_lose, NULL, TO_CHAR );
     	}
     }
    }
    return;
}

void spell_fireshield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_FIRESHIELD ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_FIRESHIELD;
    affect_to_char( victim, &af );

    send_to_char(AT_RED, "Your body is engulfed by unfelt flame.\n\r", victim );
    act(AT_RED, "$n's body is engulfed in flames.", victim, NULL, NULL, TO_ROOM );
    return;
}


void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = dice( 6, level/2 );
    dam = sc_dam( ch, dam );
    if ( IS_AFFECTED( victim, AFF_FIRESHIELD ) )
       dam /= 2;
    if ( IS_AFFECTED( victim, AFF_ICESHIELD ) )
       dam += dam/4;
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_faerie_fire( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_AC;
    af.modifier  = 2 * level;
    af.bitvector = AFF_FAERIE_FIRE;
    affect_to_char( victim, &af );

    af.location  = APPLY_HITROLL;
    af.modifier  = 0 - level/10;
    affect_to_char( victim, &af );

    send_to_char(AT_PINK, "You are surrounded by a pink outline.\n\r", victim );
    act(AT_PINK, "$n is surrounded by a pink outline.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_faerie_fog( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *ich;

    send_to_char(AT_PURPLE, "You conjure a cloud of purple smoke.\n\r", ch );
    act(AT_PURPLE, "$n conjures a cloud of purple smoke.", ch, NULL, NULL, TO_ROOM );

    for ( ich = ch->in_room->people; ich; ich = ich->next_in_room )
    {
	if ( !IS_NPC( ich ) && IS_SET( ich->act, PLR_WIZINVIS ) )
	    continue;

	if ( ich == ch || saves_spell( level, ich ) )
	    continue;

	affect_strip ( ich, gsn_invis			);
	affect_strip ( ich, gsn_mass_invis		);
	affect_strip ( ich, gsn_sneak			);
	affect_strip ( ich, gsn_shadow			);
        affect_strip ( ich, skill_lookup("phase shift") );
        affect_strip ( ich, skill_lookup("mist form")   );
        affect_strip ( ich, gsn_hide                    );
        affect_strip ( ich, gsn_chameleon               );
	REMOVE_BIT   ( ich->affected_by, AFF_HIDE	);
	REMOVE_BIT   ( ich->affected_by, AFF_INVISIBLE	);
	REMOVE_BIT   ( ich->affected_by, AFF_SNEAK	);
	REMOVE_BIT   ( ich->affected_by2, AFF_PHASED    );

	act(AT_PURPLE, "$n is revealed!", ich, NULL, NULL, TO_ROOM );
	send_to_char(AT_PURPLE, "You are revealed!\n\r", ich );
    }

    return;
}



void spell_fly( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_FLYING ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level + 3;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_FLYING;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "Your feet rise off the ground.\n\r", victim );
    act(AT_BLUE, "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_gate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *gch;
    int        npccount  = 0;
    int        pccount   = 0;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if ( IS_NPC( gch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
	    npccount++;
	if ( !IS_NPC( gch ) ||
	    ( IS_NPC( gch ) && IS_AFFECTED( gch, AFF_CHARM ) ) )
	    pccount++;
    }

    if ( npccount > pccount )
    {
	do_say( ch, "There are too many of us here!  One must die!" );
        return;
    }

    do_say( ch, "Come brothers!  Join me in this glorious bloodbath!" );
    char_to_room( create_mobile( get_mob_index( MOB_VNUM_DEMON1 ) ),
		 ch->in_room );
    return;
}



/*
 * Spell for mega1.are from Glop/Erkenbrand.
 */
void spell_general_purpose( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = number_range( 25, 100 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_giant_strength( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = 1 + (level >= 18) + (level >= 25);
    af.bitvector = 0;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "You feel stronger.\n\r", victim );
    return;
}
void spell_eternal_intellect( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type      = sn;
    af.level     = level;
    af.duration  = level/2;
    af.location  = APPLY_INT;
    af.modifier  = 1 + (level >= 18) + (level >= 25);
    af.bitvector = 0;
    affect_to_char( victim, &af );

    if ( ch != victim )
        send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "You feel an unsurpased intelligence.\n\r", victim );
    return;
}
void spell_golden_aura( int sn, int level, CHAR_DATA *ch, void *vo )
{
   CHAR_DATA *victim = (CHAR_DATA *)vo;
   AFFECT_DATA af;
   if ( IS_AFFECTED2( victim, AFF_GOLDEN ) )
	return;
   if ( !IS_NPC( ch )
   && !can_use_skpell( ch, sn ) )
	{
	send_to_char(AT_BLUE, "Nothing happens.\n\r", ch );
	return;
	}
   af.type	 = sn;
   af.level	 = level;
   af.duration	 = number_fuzzy( level / 8 );
   af.location	 = APPLY_NONE;
   af.modifier	 = 0;
   af.bitvector	 = AFF_GOLDEN;
   affect_to_char2( victim, &af );
   send_to_char( AT_YELLOW, "You are surrounded by a golden aura.\n\r", victim );
   act(AT_YELLOW, "$n is surrounded by a golden aura.", victim, NULL, NULL, TO_ROOM );
   return;
}

void spell_goodberry( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
   OBJ_DATA     *berry;

    if ( obj->item_type != ITEM_FOOD
	|| IS_OBJ_STAT( obj, ITEM_MAGIC ) )
    {
	send_to_char(AT_BLUE, "You can do nothing to that item.\n\r", ch );
	return;
    }

    act(AT_BLUE, "You pass your hand over $p slowly.", ch, obj, NULL, TO_CHAR );
    act(AT_BLUE, "$n has created a goodberry.", ch, NULL, NULL, TO_ROOM );
    berry = create_object( get_obj_index( OBJ_VNUM_BERRY ), 0 );
    berry->timer = ch->level;
    berry->value[0] = ch->level * 3;
    berry->value[1] = ch->level * 8;
    extract_obj( obj );
    obj_to_char( berry, ch );
    return;
}


void spell_harm( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;

    dam = UMAX(  20, victim->hit - dice( 1,4 ) );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam = UMIN( 50, dam / 4 );
    dam = UMIN( 175, dam );
    damage( ch, victim, dam, sn );
    return;
}



void spell_heal( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    victim->hit = UMIN( (victim->hit + (ch->level*5)), MAX_HIT(victim) );
    update_pos( victim );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "A warm feeling fills your body.\n\r", victim );
    return;
}



/*
 * Spell for mega1.are from Glop/Erkenbrand.
 */
void spell_high_explosive( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = number_range( 30, 120 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}

void spell_iceshield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_ICESHIELD ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 3 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_ICESHIELD;
    affect_to_char( victim, &af );

    send_to_char(AT_LBLUE, "An Icy crust forms about your body.\n\r", victim );
    act(AT_LBLUE, "An icy crust forms about $n's body.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_icestorm( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
                 int     dam;

    dam = dice( level, 10 );
    dam = sc_dam( ch, dam );
    if ( IS_AFFECTED( victim, AFF_ICESHIELD ) )
       dam /= 2;
    if ( IS_AFFECTED( victim, AFF_FIRESHIELD ) )
       dam += dam/4;
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}
void spell_holy_fires( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim       = (CHAR_DATA *) vo;
    int     dam;

    dam = dice( level, 10 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}

void spell_identify( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    char         buf [ MAX_STRING_LENGTH ];
    int          spn;

    sprintf( buf,
	    "Object '%s' is type %s, extra flags %s %s %s.\n\r",
	    obj->name,
	    item_type_name( obj ),
	    obj->extra_flags ? extra_bit_name( obj->extra_flags ) : "",
            obj->anti_race_flags ? antirace_bit_name(obj->anti_race_flags) : "",
            obj->anti_class_flags ? anticlass_bit_name(obj->anti_class_flags) : "");
    send_to_char(AT_CYAN, buf, ch );
#ifdef NEW_MONEY
    sprintf( buf, "Weight : %d, level : %d.\n\r", obj->weight, obj->level );
    send_to_char( AT_CYAN, buf, ch );
    sprintf( buf, "Gold Value: %d  Silver Value: %d  Copper Value: %d\n\r",
	    obj->cost.gold, obj->cost.silver, obj->cost.copper );
#else
    sprintf( buf,
	    "Weight : %d, value : %d, level : %d.\n\r",
	    obj->weight,
	    obj->cost,
	    obj->level );
#endif
    send_to_char(AT_CYAN, buf, ch );

    switch ( obj->item_type )
    {
    case ITEM_PILL:
    case ITEM_SCROLL:
    case ITEM_POTION:
	sprintf( buf, "Level %d spells of:", obj->value[0] );
	send_to_char(AT_CYAN, buf, ch );

	if ( is_sn(obj->value[1]) )
	{
	    send_to_char(AT_CYAN, " '", ch );
	    send_to_char(AT_WHITE, skill_table[obj->value[1]].name, ch );
	    send_to_char(AT_CYAN, "'", ch );
	}

	if ( is_sn(obj->value[2]) )
	{
	    send_to_char(AT_CYAN, " '", ch );
	    send_to_char(AT_WHITE, skill_table[obj->value[2]].name, ch );
	    send_to_char(AT_CYAN, "'", ch );
	}

	if ( is_sn(obj->value[3]) )
	{
	    send_to_char(AT_CYAN, " '", ch );
	    send_to_char(AT_WHITE, skill_table[obj->value[3]].name, ch );
	    send_to_char(AT_CYAN, "'", ch );
	}

	send_to_char(AT_CYAN, ".\n\r", ch );
	break;

    case ITEM_WAND:
    case ITEM_LENSE:
    case ITEM_STAFF:
	if (!(obj->value[1] == -1 ) )
	    sprintf( buf, "Has %d(%d) charges of level %d",
		   obj->value[1], obj->value[2], obj->value[0] );
	else
	    sprintf( buf, "Has unlimited charges of level %d", obj->value[0] );

	send_to_char(AT_CYAN, buf, ch );

	if ( is_sn(obj->value[3]) )
	{
	    send_to_char(AT_CYAN, " '", ch );
	    send_to_char(AT_WHITE, skill_table[obj->value[3]].name, ch );
	    send_to_char(AT_CYAN, "'", ch );
	}

	send_to_char(AT_CYAN, ".\n\r", ch );
	break;

    case ITEM_WEAPON:
	sprintf( buf, "Damage is %d to %d (average %d).\n\r",
		obj->value[1], obj->value[2],
		( obj->value[1] + obj->value[2] ) / 2 );
	send_to_char(AT_RED, buf, ch );
	break;

    case ITEM_ARMOR:
	sprintf( buf, "Armor class is %d.\n\r", obj->value[0] );
	send_to_char(AT_CYAN, buf, ch );
	break;
    }
    if ( obj->ac_type != 0 )
    {
      switch( obj->ac_type )
      {
       default:  send_to_char(AT_CYAN, "Invoke Type Unknown.\n\r", ch ); break;
       case 1 :
         {
           if ( obj->ac_charge[1] != -1 )
              sprintf( buf, "Object creation invoke, with [%d/%d] charges.\n\r",
                    obj->ac_charge[0], obj->ac_charge[1] );
           else
              sprintf( buf, "Object creation invoke, with unlimited charges.\n\r" );
           send_to_char(AT_CYAN, buf, ch );
           break;
         }
       case 2 :
         {
           if ( obj->ac_charge[1] != -1 )
              sprintf( buf, "Monster creation invoke, with [%d/%d] charges.\n\r",
                    obj->ac_charge[0], obj->ac_charge[1] );
           else
              sprintf( buf, "Monster creation invoke, with unlimited charges.\n\r" );
           send_to_char(AT_CYAN, buf, ch );
           break;
         }
       case 3 :
         {
           if ( obj->ac_charge[1] != -1 )
              sprintf( buf, "Transfer invoke, with [%d/%d] charges.\n\r",
                    obj->ac_charge[0], obj->ac_charge[1] );
           else
              sprintf( buf, "Transfer invoke, with unlimited charges.\n\r" );
           send_to_char(AT_CYAN, buf, ch );
           break;
         }
       case 4 :
         {
           if ( obj->ac_charge[1] != -1 )
              sprintf( buf, "Object morph invoke, with [%d/%d] charges.\n\r",
                    obj->ac_charge[0], obj->ac_charge[1] );
           else
              sprintf( buf, "Object morph invoke, with unlimited charges.\n\r" );
           send_to_char(AT_CYAN, buf, ch );
           break;
         }
       case 5 :
         {
           if ( obj->ac_charge[1] != -1 )
              sprintf( buf, "Spell invoke, has [%d/%d] charges of ",
                    obj->ac_charge[0], obj->ac_charge[1] );
           else
              sprintf( buf, "Spell invoke, with unlimited charges of " );
           send_to_char(AT_CYAN, buf, ch );
	   spn = skill_lookup( obj->ac_spell );
	   if ( is_sn(spn) )
	   {
	    send_to_char(AT_CYAN, " '", ch );
	    send_to_char(AT_WHITE, spn ? obj->ac_spell : "(none)", ch );
	    send_to_char(AT_CYAN, "'\n\r", ch );
	   }
	   break;
         }
      }
    }
    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d.\n\r",
		    affect_loc_name( paf->location ), paf->modifier );
	    send_to_char(AT_BLUE, buf, ch );
	}
    }

    for ( paf = obj->affected; paf; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d.\n\r",
		    affect_loc_name( paf->location ), paf->modifier );
	    send_to_char(AT_BLUE, buf, ch );
	}
    }

    return;
}

void spell_vibrate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_VIBRATING ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_VIBRATING;
    affect_to_char( victim, &af );

    send_to_char(AT_LBLUE, "You set up a complex set of vibrations around your body.\n\r", victim );
    act(AT_LBLUE, "$n's body begins to vibrate.", victim, NULL, NULL, TO_ROOM );
    return;
}


void spell_infravision( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_INFRARED ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = 2 * level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INFRARED;
    affect_to_char( victim, &af );

    send_to_char(AT_RED, "Your eyes glow.\n\r", victim );
    act(AT_RED, "$n's eyes glow.\n\r", ch, NULL, NULL, TO_ROOM );
    return;
}


void spell_incinerate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( saves_spell( level, victim ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_FLAMING;
    affect_join( victim, &af );

    if ( ch != victim )
	send_to_char(AT_RED, "Ok.\n\r", ch );
    send_to_char(AT_RED, "Your body bursts into flames!\n\r", victim );
    return;
}

void spell_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_INVISIBLE ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INVISIBLE;
    affect_to_char( victim, &af );

    send_to_char(AT_GREY, "You fade out of existence.\n\r", victim );
    act(AT_GREY, "$n fades out of existence.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_phase_shift( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED2( victim, AFF_PHASED ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = ch->level/6;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PHASED;
    affect_to_char2( victim, &af );

    send_to_char(AT_GREY, "You phase into another plane.\n\r", victim );
    act(AT_GREY, "$n phases out of reality.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_mist_form( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED2( victim, AFF_PHASED ) )
        return;

    af.type      = sn;
    af.level     = level;
    af.duration  = ch->level/6;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PHASED;
    affect_to_char2( victim, &af );

    send_to_char(AT_GREY, "You seem to feel transparent.\n\r", victim );
    act(AT_GREY, "$n takes on the form of a mist.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_know_alignment( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    char      *msg;
    int        ap;

    ap = victim->alignment;

         if ( ap >  700 ) msg = "$N has an aura as white as the driven snow.";
    else if ( ap >  350 ) msg = "$N is of excellent moral character.";
    else if ( ap >  100 ) msg = "$N is often kind and thoughtful.";
    else if ( ap > -100 ) msg = "$N doesn't have a firm moral commitment.";
    else if ( ap > -350 ) msg = "$N lies to $S friends.";
    else if ( ap > -700 ) msg = "$N's slash DISEMBOWELS you!";
    else msg = "I'd rather just not say anything at all about $N.";

    act(AT_BLUE, msg, ch, NULL, victim, TO_CHAR );
    return;
}



void spell_lightning_bolt( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	 0,
	 0,  0,  0,  0,  0,	 0,  0,  0, 25, 28,
	31, 34, 37, 40, 40,	41, 42, 42, 43, 44,
	44, 45, 46, 46, 47,	48, 48, 49, 50, 50,
	51, 52, 52, 53, 54,	54, 55, 56, 56, 57,
	58, 58, 59, 60, 60,	61, 62, 62, 63, 64,
	70, 72, 74, 76, 78,	80, 82, 84, 86, 88,
	90, 92, 94, 96, 98,	100,102,104,106,108,
	110,112,114,116,118,	120,122,124,126,128,
	130,132,134,136,138,	140,142,144,146,148,
	150,152,154,156,158,	160,162,164,166,170
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    if ( IS_AFFECTED( victim, AFF_SHOCKSHIELD ) )
       dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_locate_object( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    char      buf [ MAX_INPUT_LENGTH ];
    bool      found;

    found = FALSE;
    for ( obj = object_list; obj; obj = obj->next )
    {
	if ( !can_see_obj( ch, obj ) || !is_name( ch, target_name, obj->name ) )
	    continue;

	if ( IS_SET( obj->extra_flags, ITEM_NO_LOCATE) && ( get_trust( ch ) < L_APP ) )
	    continue;

	found = TRUE;

	for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by )
	{
	    sprintf( buf, "%s carried by %s.\n\r",
		    obj->short_descr, PERS( in_obj->carried_by, ch ) );
	}
	else if ( in_obj->stored_by )
	{
	    sprintf( buf, "%s in storage.\n\r",
		    obj->short_descr );
	}
	else
	{
	    sprintf( buf, "%s in %s.\n\r",
		    obj->short_descr, !in_obj->in_room

		    ? "somewhere" : in_obj->in_room->name );
	}

	buf[0] = UPPER( buf[0] );
	send_to_char(AT_BLUE, buf, ch );
    }

    if ( !found )
	send_to_char(AT_WHITE, "Nothing like that in hell, earth, or heaven.\n\r", ch );

    return;
}



void spell_magic_missile( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim      = (CHAR_DATA *) vo;
    static const int       dam_each [ ] =
    {
	 0,
	 3,  3,  4,  4,  5,	 6,  6,  6,  6,  6,
	 7,  7,  7,  7,  7,	 8,  8,  8,  8,  8,
	 9,  9,  9,  9,  9,	10, 10, 10, 10, 10,
	11, 11, 11, 11, 11,	12, 12, 12, 12, 12,
	13, 13, 13, 13, 13,	14, 14, 14, 14, 14,
	15, 15, 15, 15, 15,	16, 16, 16, 16, 16,
	18, 18, 18, 18, 18,	20, 20, 20, 20, 20,
	21, 21, 21, 21, 21,	22, 22, 22, 22, 22,
	24, 24, 24, 24, 24,	26, 26, 26, 26, 26,
	28, 28, 28, 28, 28,	30, 31, 32, 33, 40
    };
		 int       dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}

void spell_mana( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    victim->mana = UMIN( victim->mana + 70, MAX_MANA(victim) );
    update_pos( victim );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "You feel a surge of energy.\n\r", victim );
    return;
}


void spell_mass_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) || IS_AFFECTED( gch, AFF_INVISIBLE ) )
	    continue;

	send_to_char(AT_GREY, "You slowly fade out of existence.\n\r", gch );
	act(AT_GREY, "$n slowly fades out of existence.", gch, NULL, NULL, TO_ROOM );

	af.type      = sn;
        af.level     = level;
	af.duration  = 24;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_INVISIBLE;
	affect_to_char( gch, &af );
    }
    send_to_char(AT_BLUE, "Ok.\n\r", ch );

    return;
}



void spell_null( int sn, int level, CHAR_DATA *ch, void *vo )
{
    send_to_char(AT_WHITE, "That's not a spell!\n\r", ch );
    return;
}



void spell_pass_door( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_PASS_DOOR) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 4 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PASS_DOOR;
    affect_to_char( victim, &af );

    send_to_char(AT_GREY, "You turn translucent.\n\r", victim );
    act(AT_GREY, "$n turns translucent.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_permenancy( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
/*    AFFECT_DATA *paf;*/

    if ( obj->item_type != ITEM_WAND
	&& obj->item_type != ITEM_STAFF
	&& obj->item_type != ITEM_LENSE )
    {
	send_to_char(AT_BLUE, "You cannot make that item permenant.\n\r", ch );
	return;
    }
    obj->value[2] = -1;
    obj->value[1] = -1;
    act(AT_BLUE, "You run your finger up $p, you can feel it's power growing.", ch, obj, NULL, TO_CHAR );
    act(AT_BLUE, "$n slowly runs $s finger up $p.", ch, obj, NULL, TO_ROOM );
    return;
}


void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( saves_spell( level, victim ) || victim->race == RACE_GHOUL )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = AFF_POISON;
    affect_join( victim, &af );

    if ( ch != victim )
	send_to_char(AT_GREEN, "Ok.\n\r", ch );
    send_to_char(AT_GREEN, "You feel very sick.\n\r", victim );
    return;
}
void spell_polymorph( int sn, int level, CHAR_DATA *ch, void *vo )
{
   CHAR_DATA          *victim = (CHAR_DATA *) vo;
  char                buf [MAX_STRING_LENGTH];
    AFFECT_DATA af;

   if ( !(victim = get_char_world( ch, target_name ) )
          || victim == ch
          || saves_spell( level, victim)
          || IS_AFFECTED( ch, AFF_POLYMORPH ) )
      {
         send_to_char( AT_BLUE, "You failed.\n\r", ch );
         return;
      }

    af.type      = sn;
    af.level	 = level;
    af.duration  = level/5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_POLYMORPH;
    affect_to_char2( ch, &af );

    if (!IS_NPC(victim))
      {
       sprintf( buf, "%s %s", victim->name, victim->pcdata->title);
       free_string( ch->long_descr );
       ch->long_descr = str_dup(buf);
      }
    else
      {
       sprintf( buf, "%s", victim->long_descr );
       free_string( ch->long_descr );
       ch->long_descr = str_dup(buf);
      }
    act(AT_BLUE, "$n's form wavers and then resolidifies.", ch, NULL, NULL, TO_ROOM);
    send_to_char(AT_BLUE, "You have succesfully polymorphed.\n\r", ch );
    return;
}
void spell_portal( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA          *victim = (CHAR_DATA *) vo;
    OBJ_DATA           *gate1;
    OBJ_DATA           *gate2;
    int                duration;

    if ( !( victim = get_char_world( ch, target_name ) )
	|| victim == ch
	|| !victim->in_room
	|| IS_SET( victim->in_room->room_flags, ROOM_SAFE      )
	|| IS_SET( victim->in_room->room_flags, ROOM_PRIVATE   )
	|| IS_SET( victim->in_room->room_flags, ROOM_SOLITARY  )
	|| IS_SET( victim->in_room->room_flags, ROOM_NO_ASTRAL_IN  )
	|| IS_SET( ch->in_room->room_flags, ROOM_NO_ASTRAL_OUT )
	|| IS_ARENA(ch)
	|| victim->in_room->area == arena.area
	|| IS_SET( victim->in_room->area->area_flags, AREA_PROTOTYPE ) )
          {
        	send_to_char(AT_BLUE, "You failed.\n\r", ch );
        	return;
          }

    duration = level/10;
    gate1 = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    gate2 = create_object( get_obj_index( OBJ_VNUM_PORTAL ), 0 );
    gate1->timer = duration;
    gate2->timer = duration;
    gate2->value[0] = ch->in_room->vnum;
    gate1->value[0] = victim->in_room->vnum;
    act(AT_BLUE, "A huge shimmering gate rises from the ground.", ch, NULL, NULL, TO_CHAR );
    act(AT_BLUE, "$n utters a few incantations and a gate rises from the ground.", ch, NULL, NULL, TO_ROOM );
    obj_to_room( gate1, ch->in_room );
    act(AT_BLUE, "A huge shimmering gate rises from the ground.", victim, NULL, NULL, TO_CHAR );
    act(AT_BLUE, "A huge shimmering gate rises from the ground.", victim, NULL, NULL, TO_ROOM );
    obj_to_room( gate2, victim->in_room );
    return;
}


void spell_protection( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_PROTECT ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PROTECT;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "You feel protected.\n\r", victim );
    return;
}



void spell_refresh( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    victim->move = UMIN( victim->move + level + 50, MAX_MOVE(victim));

    if ( ch != victim )
	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "You feel less tired.\n\r", victim );
    return;
}


/* Expulsion of ITEM_NOREMOVE addition by Katrina */
void spell_remove_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        iWear, SkNum;
    int        yesno  = FALSE;

    for ( iWear = 0; iWear < MAX_WEAR; iWear ++ )
    {
	if ( !( obj = get_eq_char( victim, iWear ) ) )
	    continue;

        if ( IS_SET( obj->extra_flags, ITEM_NODROP ) )
        {
            REMOVE_BIT( obj->extra_flags, ITEM_NODROP );
            send_to_char( AT_BLUE, "You feel a burden relieved.\n\r", ch );
            yesno = TRUE;
        }
	if ( IS_SET( obj->extra_flags, ITEM_NOREMOVE ) )
	{
	    unequip_char( victim, obj );
	    obj_from_char( obj );
	    obj_to_room( obj, victim->in_room );
	    act(AT_BLUE, "You toss $p to the ground.",  victim, obj, NULL, TO_CHAR );
	    act(AT_BLUE, "$n tosses $p to the ground.", victim, obj, NULL, TO_ROOM );
	    yesno = TRUE;
	}
    }
    SkNum=skill_lookup("incinerate");
    if ( is_affected( victim, SkNum))
    {
        affect_strip( victim, SkNum);
        send_to_char(AT_BLUE, "Your body has been extinguished.\n\r", ch);
        yesno = TRUE;
    }
    SkNum=skill_lookup("curse");
    if ( is_affected( victim, SkNum))
    {
	affect_strip( victim, SkNum);
	send_to_char(AT_BLUE, "You feel better.\n\r", victim );
	yesno = TRUE;
    }

    if ( ch != victim && yesno )
        send_to_char(AT_BLUE, "Ok.\n\r", ch );
    return;
}



void spell_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SANCTUARY;
    affect_to_char( victim, &af );

    send_to_char(AT_WHITE, "You are surrounded by a white aura.\n\r", victim );
    act(AT_WHITE, "$n is surrounded by a white aura.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_web( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    char        buf[MAX_STRING_LENGTH];

    if ( IS_AFFECTED( victim, AFF_ANTI_FLEE ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 10 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_ANTI_FLEE;
    affect_to_char( victim, &af );

    sprintf( buf, "%s lifts his hands and webs entanle you!\n\r", ch->name );
    send_to_char(AT_WHITE, buf, victim );
    act(AT_WHITE, "$n has been imobilized by a plethora of sticky webs.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_confusion( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    char        buf[MAX_STRING_LENGTH];

    if ( IS_AFFECTED2( victim, AFF_CONFUSED ) )
	return;
if ( saves_spell( level, victim ) )
   {
     send_to_char( AT_BLUE, "You failed.\n\r", ch );
     return;
   }

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 10 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_CONFUSED;
    affect_to_char2( victim, &af );

    sprintf( buf, "You feel disorientated.\n\r" );
    send_to_char(AT_WHITE, buf, victim );
    act(AT_WHITE, "$n stares around blankly.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_fumble( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    char        buf[MAX_STRING_LENGTH];

    if ( IS_AFFECTED2( victim, AFF_FUMBLE ) )
	return;
if ( saves_spell( level, victim ) )
   {
     send_to_char( AT_BLUE, "You failed.\n\r", ch );
     return;
   }

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 10 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_FUMBLE;
    affect_to_char2( victim, &af );

    af.location  = APPLY_HITROLL;
    af.modifier  = 0 - level / 5;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    sprintf( buf, "You feel clumsy.\n\r" );
    send_to_char(AT_WHITE, buf, victim );
    act(AT_WHITE, "$n looks very clumsy.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_mind_probe( int sn, int level, CHAR_DATA *ch, void *vo )
{
    AFFECT_DATA *paf;
    CHAR_DATA   *victim;
    bool printed = FALSE;
    char         buf  [ MAX_STRING_LENGTH ];
    char         buf1 [ MAX_STRING_LENGTH ];


    if (!(victim = get_char_room( ch, target_name ) ) )
    {
      send_to_char(AT_BLUE, "You cannot find them.\n\r", ch );
      return;
    }

    if ( victim->level >= LEVEL_IMMORTAL )
    {
      sprintf(buf1, "%s attempted to probe your mind.", ch->name );
      send_to_char(AT_RED, buf1, victim );
      send_to_char(AT_BLUE, "The mind of an immortal is beyond your understanding.", ch );
      return;
    }

    sprintf(buf1, "You send your conciousness into %s's mind.\n\r", victim->name);
    send_to_char(AT_RED, buf1, ch );
    send_to_char(AT_RED, "You feel someone touch your mind.\n\r", victim );
    buf1[0] = '\0';

    if (IS_NPC(victim))
       {
         send_to_char (AT_WHITE, "The mind is to chaotic to merge with.\n\r", ch );
         return;
       }
    sprintf( buf,
	    "You are %s%s.\n\r",
	    victim->name,
	    IS_NPC( victim ) ? "" : victim->pcdata->title );
    send_to_char( AT_CYAN, buf, ch );
    sprintf( buf,
            "Level &C%d&c, %d years old (%d hours).\n\r",
	    victim->level,
	    get_age( victim ),
	    (get_age( victim ) - 17) * 4 );
    send_to_char( AT_CYAN, buf, ch );
    sprintf( buf, "You are a &Y%s&c and have chosen the vocation of a &Y%s&c.\n\r",
             (get_race_data(victim->race))->race_full, class_short( victim ) );
    send_to_char( AT_CYAN, buf, ch );
    if ( victim->clan )
    {
        CLAN_DATA *clan;

        clan = get_clan_index( victim->clan );
        sprintf( buf, "You belong to the clan %s.\n\r", clan->name );
        send_to_char( AT_WHITE, buf, ch );
    }

    if ( get_trust( victim ) != victim->level )
    {
	sprintf( buf, "You have been granted the powers of a level &R%d&W.\n\r",
		get_trust( victim ) );
        send_to_char( AT_WHITE, buf, ch );
    }

    if (!IS_NPC( victim ))
    if ( get_trust( victim ) > LEVEL_IMMORTAL )
    {
        sprintf( buf, "Bamfin&r: &w%s.\n\r", victim->pcdata->bamfin );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Bamfout&r: &w%s.\n\r", victim->pcdata->bamfout );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Bamfusee&r: &w%s.\n\r", ch->pcdata->bamfusee );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Transto&r: &w%s.\n\r", ch->pcdata->transto );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Transfrom&r: &w%s.\n\r", ch->pcdata->transfrom );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Transvict&r: &w%s.\n\r", ch->pcdata->transvict );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Slayusee&r: &w%s.\n\r", ch->pcdata->slayusee );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Slayroom&r: &w%s.\n\r", ch->pcdata->slayroom );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Slayvict&r: &w%s.\n\r", ch->pcdata->slayvict );
        send_to_char( AT_RED, buf, ch );
    }

    send_to_char( AT_CYAN, "You have", ch );
    sprintf ( buf, " %d/%d(%d)", victim->hit,victim->perm_hit,MAX_HIT(victim) );
    send_to_char( AT_YELLOW, buf, ch );
    send_to_char( AT_CYAN, " hit, ", ch );
    sprintf( buf, "%s%d/%d %s",
	     is_class( victim, CLASS_VAMPIRE ) ? "&R" : "&C",
	     MT( ch ), MT_MAX( ch ),
	     is_class( victim, CLASS_VAMPIRE ) ? " blood, " : "mana, " );
/*
    if ( !is_class( victim, CLASS_VAMPIRE ) )
       {
         sprintf ( buf, "%d/%d(%d)",victim->mana,victim->perm_mana,MAX_MANA(victim));
         send_to_char( AT_LBLUE, buf, ch );
         send_to_char( AT_CYAN, " mana, ", ch );
       }
    else
       {
         sprintf ( buf, "%d/%d(%d)", victim->bp,victim->perm_bp, MAX_BP(victim));
         send_to_char( AT_RED, buf, ch );
         send_to_char( AT_CYAN, " blood, ", ch );
       }
*/
    sprintf ( buf, "%d/%d(%d)", victim->move, victim->perm_move, MAX_MOVE(victim));
    send_to_char( AT_GREEN, buf, ch );
    send_to_char( AT_CYAN, " movement, ", ch );
    sprintf ( buf, "%d", victim->practice );
    send_to_char( AT_WHITE, buf, ch );
    send_to_char( AT_CYAN, " practices.\n\r", ch );

    sprintf( buf,
	    "You are carrying %d/%d items with weight %d/%d kg.\n\r",
	    victim->carry_number, can_carry_n( victim ),
	    victim->carry_weight, can_carry_w( victim ) );
    send_to_char( AT_CYAN, buf, ch );

    sprintf( buf,
	"Str: %d&p(&P%d&p)&P  Int: %d&p(&P%d&p)&P  Wis: %d&p(&P%d&p)&P  Dex: %d&p(&P%d&p)&P  Con: %d&p(&P%d&p)&P.\n\r",
	IS_NPC(victim) ? 13: victim->pcdata->perm_str, IS_NPC(victim) ? 13: get_curr_str( victim ),
	IS_NPC(victim) ? 13: victim->pcdata->perm_int, IS_NPC(victim) ? 13: get_curr_int( victim ),
	IS_NPC(victim) ? 13: victim->pcdata->perm_wis, IS_NPC(victim) ? 13: get_curr_wis( victim ),
	IS_NPC(victim) ? 13: victim->pcdata->perm_dex, IS_NPC(victim) ? 13: get_curr_dex( victim ),
	IS_NPC(victim) ? 13: victim->pcdata->perm_con, IS_NPC(victim) ? 13: get_curr_con( victim ) );
    send_to_char( AT_PINK, buf, ch );

    send_to_char( AT_CYAN, "You have scored ", ch );
#ifdef NEW_MONEY
    sprintf( buf, "&W%d &cexperience points.\n\r", victim->exp );
    send_to_char( AT_WHITE, buf, ch );
    sprintf( buf, "&cYou have accumulated &W%d &Ygold, &W%d &wsilver, &cand &W%d &Ocopper &ccoins.\n\r",
	     victim->money.gold, victim->money.silver, victim->money.copper );
    send_to_char( AT_WHITE, buf, ch );
#else
    sprintf( buf, "%d ", victim->exp );
    send_to_char( AT_WHITE, buf, ch );
    send_to_char( AT_CYAN, "exp, and have accumulated ", ch );
    sprintf( buf, "%d ", victim->gold );
    send_to_char( AT_YELLOW, buf, ch );
    send_to_char( AT_CYAN, "gold coins.\n\r", ch );
#endif

    if ( !IS_NPC( victim ) && victim->pcdata->condition[COND_DRUNK]   > 10 )
	send_to_char( AT_GREY, "You are drunk.\n\r", ch );
    if ( !IS_NPC( victim ) && victim->pcdata->condition[COND_THIRST] ==  0
	&& victim->level >= LEVEL_IMMORTAL )
	send_to_char( AT_BLUE, "You are thirsty.\n\r", ch );
    if ( !IS_NPC( victim ) && victim->pcdata->condition[COND_FULL]   ==  0
	&& victim->level >= LEVEL_IMMORTAL )
	send_to_char( AT_ORANGE, "You are hungry.\n\r", ch  );

    switch ( victim->position )
    {
    case POS_DEAD:
	send_to_char( (AT_RED + AT_BLINK), "You are DEAD!!\n\r", ch ); break;
    case POS_MORTAL:
	send_to_char( AT_RED, "You are mortally wounded.\n\r", ch ); break;
    case POS_INCAP:
	send_to_char( AT_RED, "You are incapacitated.\n\r", ch ); break;
    case POS_STUNNED:
	send_to_char( AT_RED, "You are stunned.\n\r", ch ); break;
    case POS_SLEEPING:
	send_to_char( AT_LBLUE, "You are sleeping.\n\r", ch ); break;
    case POS_RESTING:
	send_to_char( AT_LBLUE, "You are resting.\n\r", ch ); break;
    case POS_STANDING:
	send_to_char( AT_GREEN, "You are standing.\n\r", ch ); break;
    case POS_FIGHTING:
	send_to_char( AT_BLOOD, "You are fighting.\n\r", ch ); break;
    }

    if ( ch->level >= 20 )
    {
	sprintf( buf, "AC: %d.  ", GET_AC( victim ) );
	send_to_char( AT_CYAN, buf, ch );
    }

    send_to_char( AT_GREEN, "You are ", ch );
         if ( GET_AC( victim ) >=  101 ) send_to_char( AT_GREEN, "WORSE than naked!\n\r", ch );
    else if ( GET_AC( victim ) >=   20 ) send_to_char( AT_GREEN,
"naked.\n\r"           , ch );
    else if ( GET_AC( victim ) >=    0 ) send_to_char( AT_GREEN, "wearing clothes.\n\r" , ch );
    else if ( GET_AC( victim ) >= - 50 ) send_to_char( AT_GREEN, "slightly armored.\n\r", ch );
    else if ( GET_AC( victim ) >= -100 ) send_to_char( AT_GREEN, "somewhat armored.\n\r", ch );
    else if ( GET_AC( victim ) >= -250 ) send_to_char( AT_GREEN, "armored.\n\r"         , ch );
    else if ( GET_AC( victim ) >= -500 ) send_to_char( AT_GREEN, "well armored.\n\r"    , ch );
    else if ( GET_AC( victim ) >= -750 ) send_to_char( AT_GREEN, "strongly armored.\n\r", ch );
    else if ( GET_AC( victim ) >= -1000 ) send_to_char( AT_GREEN, "heavily armored.\n\r" , ch );
    else if ( GET_AC( victim ) >= -1200 ) send_to_char( AT_GREEN, "superbly armored.\n\r", ch );
    else if ( GET_AC( victim ) >= -1400 ) send_to_char( AT_GREEN, "divinely armored.\n\r", ch );
    else                           send_to_char( AT_GREEN, "invincible!\n\r", ch );

    if ( ch->level >= 12 )
    {
	sprintf( buf, "Hitroll: " );
	send_to_char(AT_BLOOD, buf, ch );
	sprintf( buf, "%d", GET_HITROLL( victim ) );
	send_to_char(AT_RED, buf, ch);
	sprintf( buf, "  Damroll: " );
	send_to_char( AT_BLOOD, buf, ch );
	sprintf( buf, "%d.\n\r", GET_DAMROLL( victim ) );
	send_to_char( AT_RED, buf, ch );
    }

    if ( ch->level >= 8 )
    {
	sprintf( buf, "Alignment: %d.  ", victim->alignment );
	send_to_char( AT_CYAN, buf, ch );
    }

    send_to_char( AT_CYAN, "You are ", ch );
         if ( victim->alignment >  900 ) send_to_char( AT_BLUE, "angelic.\n\r",ch );
    else if ( victim->alignment >  700 ) send_to_char( AT_BLUE, "saintly.\n\r",ch );
    else if ( victim->alignment >  350 ) send_to_char( AT_BLUE, "good.\n\r"   ,ch );
    else if ( victim->alignment >  100 ) send_to_char( AT_BLUE, "kind.\n\r"   ,ch );
    else if ( victim->alignment > -100 ) send_to_char( AT_YELLOW, "neutral.\n\r",ch );
    else if ( victim->alignment > -350 ) send_to_char( AT_RED, "mean.\n\r"    ,ch);
    else if ( victim->alignment > -700 ) send_to_char( AT_RED, "evil.\n\r"    ,ch);
    else if ( victim->alignment > -900 ) send_to_char( AT_RED, "demonic.\n\r" ,ch);
    else                             send_to_char( AT_RED, "satanic.\n\r" ,ch);

    if ( !IS_NPC( victim ) && IS_IMMORTAL( victim ) )
    {
      sprintf( buf, "WizInvis level: %d   WizInvis is %s\n\r",
                      victim->wizinvis,
                      IS_SET( victim->act, PLR_WIZINVIS ) ? "ON" : "OFF" );
      send_to_char( AT_YELLOW, buf, ch );
      sprintf( buf, "Cloaked level: %d   Cloaked is %s\n\r",
                      victim->cloaked,
                      IS_SET( victim->act, PLR_CLOAKED ) ? "ON" : "OFF" );
      send_to_char( AT_YELLOW, buf, ch );
    }
    if ( victim->affected )
    {
	for ( paf = victim->affected; paf; paf = paf->next )
	{
	    if ( paf->deleted )
	        continue;

	    if ( !printed )
	    {
		send_to_char( AT_CYAN, "You are affected by:\n\r", ch );
		printed = TRUE;
	    }

	    sprintf( buf, "Spell: '%s'", skill_table[paf->type].name );
            send_to_char( AT_WHITE, buf, ch );
	    if ( ch->level >= 20 )
	    {
		sprintf( buf,
			" modifies %s by %d for %d hours",
			affect_loc_name( paf->location ),
			paf->modifier,
			paf->duration );
		send_to_char(AT_WHITE, buf, ch );
	    }

	    send_to_char( AT_WHITE, ".\n\r", ch );
	}
    }

    if ( victim->affected2 )
    {
	for ( paf = victim->affected2; paf; paf = paf->next )
	{
	    if ( paf->deleted )
	        continue;

	    if ( !printed )
	    {
		send_to_char( AT_CYAN, "You are affected by:\n\r", ch );
		printed = TRUE;
	    }

	    sprintf( buf, "Spell: '%s'", skill_table[paf->type].name );
            send_to_char( AT_WHITE, buf, ch );
	    if ( ch->level >= 20 )
	    {
		sprintf( buf,
			" modifies %s by %d for %d hours",
			affect_loc_name( paf->location ),
			paf->modifier,
			paf->duration );
		send_to_char(AT_WHITE, buf, ch );
	    }

	    send_to_char( AT_WHITE, ".\n\r", ch );
	}
    }

    send_to_char( AT_RED, "The presence lifts from your mind.\n\r", victim );
    return;
}

void spell_entangle( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    char        buf[MAX_STRING_LENGTH];

    if ( IS_AFFECTED( victim, AFF_ANTI_FLEE ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 10 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_ANTI_FLEE;
    affect_to_char( victim, &af );

    sprintf( buf, "%s calls forth nature to hold you in place.\n\r", ch->name );
    send_to_char(AT_GREEN, buf, victim );
    act(AT_GREEN, "Hundreds of vines reach from the ground to entangle $n.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_scry( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_SCRY ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 4 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SCRY;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "Your vision improves.\n\r", victim );
    return;
}


void spell_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = 8 + level;
    af.location  = APPLY_AC;
    af.modifier  = -30;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char(AT_BLUE, "You are surrounded by a force shield.\n\r", victim );
    act(AT_BLUE, "$n is surrounded by a force shield.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_shocking_grasp( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	 0,
	 0,  0,  0,  0,  0,	 0, 20, 25, 29, 33,
	36, 39, 39, 39, 40,	40, 41, 41, 42, 42,
	43, 43, 44, 44, 45,	45, 46, 46, 47, 47,
	48, 48, 49, 49, 50,	50, 51, 51, 52, 52,
	53, 53, 54, 54, 55,	55, 56, 56, 57, 57,
	58, 58, 59, 60, 61,	62, 63, 64, 65, 66,
	67, 68, 69, 70, 71,	72, 73, 74, 75, 76,
	77, 78, 79, 80, 81,	82, 83, 84, 85, 86,
	87, 88, 89, 90, 91,	92, 93, 94, 95, 96,
	97, 98, 99,100,101,	102,103,104,105,106
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}

void spell_shockshield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_SHOCKSHIELD ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 6 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SHOCKSHIELD;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "Sparks of electricity flow into your body.\n\r", victim );
    act(AT_BLUE, "Bolts of electricity flow from the ground into $n's body.", victim, NULL, NULL, TO_ROOM );
    return;
}


void spell_sleep( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_SLEEP )
	|| level < victim->level
	|| ( saves_spell(level + IS_SRES(victim, RES_MAGIC) ? -5 : 0, victim)
	&& !(get_trust( ch ) > LEVEL_IMMORTAL) ) )
    {
	send_to_char(AT_BLUE, "You failed.\n\r", ch );
	return;
    }

    if(IS_SIMM(victim, IMM_MAGIC))
      return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = 4 + level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SLEEP;
    affect_join( victim, &af );

    if ( IS_AWAKE( victim ) )
    {
	send_to_char(AT_BLUE, "You feel very sleepy ..... zzzzzz.\n\r", victim );
	if ( victim->position == POS_FIGHTING )
	   stop_fighting( victim, TRUE );
	do_sleep( victim, "" );
    }

    return;
}

void spell_spell_bind( int sn, int level, CHAR_DATA *ch, void *vo )
{
    bool Charged = 0;
    OBJ_DATA    *obj = (OBJ_DATA *) vo;


    if ( obj->item_type == ITEM_WAND
	|| obj->item_type == ITEM_STAFF
	|| obj->item_type == ITEM_LENSE )
    {
	if(obj->value[2] < obj->value[1])
	{
	     obj->value[2]=obj->value[1];
	     Charged++;
	}
    }
    else if (obj->ac_type==5 && obj->ac_spell)
    {
	if(obj->ac_charge[0] < obj->ac_charge[1] && obj->ac_charge[1]!=-1)
	{
	     obj->ac_charge[0]++;
	     Charged++;
	}
    }

    else
    {
	send_to_char(AT_BLUE, "You cannot bind magic to that item.\n\r", ch );
	return;
    }

    if (!Charged)
    {
        send_to_char(AT_BLUE, "That item is at full charge.\n\r", ch );
        return;
    }

    act(AT_BLUE, "You slowly pass your hand over $p, it vibrates slowly.", ch, obj, NULL, TO_CHAR );
    act(AT_BLUE, "$n slowly passes $s hand over $p, it vibrates slowly.", ch, obj, NULL, TO_ROOM );
    return;
}

void spell_stone_skin( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_AC;
    af.modifier  = -45;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char(AT_GREY, "Your skin turns to stone.\n\r", victim );
    act(AT_GREY, "$n's skin turns to stone.", victim, NULL, NULL, TO_ROOM );
    return;
}

/*Decklarean*/
void spell_bark_skin( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.location  = APPLY_AC;
    af.modifier  = -10 - level / 4;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char(AT_GREY, "Your skin turns to bark.\n\r", victim );
    act(AT_GREY, "$n's skin turns to bark.", victim, NULL, NULL, TO_ROOM );
    return;
}


void spell_summon( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;

    if ( !( victim = get_char_world( ch, target_name ) )
	|| victim == ch
	|| ch->in_room->area == arena.area
	|| IS_ARENA(victim)
	|| !victim->in_room
	|| IS_SET( victim->in_room->room_flags, ROOM_SAFE      )
	|| IS_SET( victim->in_room->room_flags, ROOM_PRIVATE   )
	|| IS_SET( victim->in_room->room_flags, ROOM_SOLITARY  )
	|| IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL )
        || IS_SET( victim->in_room->room_flags, ROOM_NO_ASTRAL_OUT )
	|| victim->level >= level + 3
	|| victim->fighting
	|| ( IS_NPC( victim ) && saves_spell( level, victim ) )
	|| IS_SET( victim->in_room->area->area_flags, AREA_PROTOTYPE )
	|| IS_AFFECTED( victim, AFF_NOASTRAL ) )
    {
	send_to_char(AT_BLUE, "You failed.\n\r", ch );
	return;
    }

    act(AT_BLUE, "$n disappears suddenly.", victim, NULL, NULL,     TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    act(AT_BLUE, "$n has summoned you!",    ch,     NULL, victim,   TO_VICT );
    act(AT_BLUE, "$n arrives suddenly.",    victim, NULL, NULL,     TO_ROOM );
    send_to_char( AT_BLUE, "You feel a wave of nausia come over you.\n\r", ch );
    ch->position = POS_STUNNED;
    update_pos( ch );
    STUN_CHAR( ch, 3, STUN_COMMAND );
    do_look( victim, "auto" );
    return;
}



void spell_teleport( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA       *victim = (CHAR_DATA *) vo;
    CHAR_DATA *pet;
    ROOM_INDEX_DATA *pRoomIndex;

    if ( !victim->in_room
	|| IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL)
	|| IS_SET( victim->in_room->room_flags, ROOM_NO_ASTRAL_OUT)
	|| IS_SET( victim->in_room->area->area_flags, AREA_PROTOTYPE )
	|| ( !IS_NPC( ch ) && victim->fighting )
	|| ( victim != ch
	    && ( saves_spell( level, victim )
		|| saves_spell( level, victim ) ) ) )
    {
	send_to_char(AT_BLUE, "You failed.\n\r", ch );
	return;
    }

    for ( ; ; )
    {
	pRoomIndex = get_room_index( number_range( 0, 32767 ) );
	if ( pRoomIndex )
	    if (   !IS_SET( pRoomIndex->room_flags, ROOM_PRIVATE  )
		&& !IS_SET( pRoomIndex->room_flags, ROOM_SOLITARY )
		&& !IS_SET( pRoomIndex->room_flags, ROOM_NO_ASTRAL_IN    )
		&& !IS_SET( pRoomIndex->room_flags, ROOM_NO_RECALL)
		&& !IS_SET( pRoomIndex->area->area_flags, AREA_PROTOTYPE ) )
	    break;
    }

    for ( pet = victim->in_room->people; pet; pet = pet->next_in_room )
    {
      if ( IS_NPC( pet ) )
        if ( IS_SET( pet->act, ACT_PET ) && ( pet->master == victim ) )
          break;
    }

    act(AT_BLUE, "$n glimmers briefly, then is gone.", victim, NULL, NULL, TO_ROOM );
    if ( pet )
    {
      act( AT_BLUE, "$n glimmers briefly, then is gone.", pet, NULL, NULL, TO_ROOM );
      char_from_room( pet );
    }
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    act(AT_BLUE, "The air starts to sparkle, then $n appears from nowhere.",   victim, NULL, NULL, TO_ROOM );
    do_look( victim, "auto" );
    if ( pet )
    {
      char_to_room( pet, pRoomIndex );
      act( AT_BLUE, "The air starts to sparkle, then $n appears from nowhere.", pet, NULL, NULL, TO_ROOM );
    }
    return;
}



void spell_ventriloquate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    char       buf1    [ MAX_STRING_LENGTH ];
    char       buf2    [ MAX_STRING_LENGTH ];
    char       speaker [ MAX_INPUT_LENGTH  ];

    target_name = one_argument( target_name, speaker );

    sprintf( buf1, "%s says '%s'.\n\r",              speaker, target_name );
    sprintf( buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name );
    buf1[0] = UPPER( buf1[0] );

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
	if ( !is_name( NULL, speaker, vch->name ) )
	    send_to_char(AT_CYAN, saves_spell( level, vch ) ? buf2 : buf1, vch );
    }

    return;
}



void spell_weaken( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || saves_spell( level, victim ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = level / 2;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char(AT_GREEN, "Ok.\n\r", ch );
    send_to_char(AT_GREEN, "You feel weaker.\n\r", victim );
    return;
}



/*
 * This is for muds that want scrolls of recall.
 */
void spell_word_of_recall( int sn, int level, CHAR_DATA *ch, void *vo )
{
    do_recall( (CHAR_DATA *) vo, "" );
    return;
}



/*
 * NPC spells.
 */
void spell_acid_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( level, victim ) )
    {
	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted )
	        continue;

	    if ( IS_SET( obj_lose->extra_flags, ITEM_NO_DAMAGE ) )
	      continue;

	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    case ITEM_ARMOR:
		if ( obj_lose->value[0] > 0 )
		{
		    act(AT_GREEN, "$p is pitted and etched!",
			victim, obj_lose, NULL, TO_CHAR );
/*		    if ( ( iWear = obj_lose->wear_loc ) != WEAR_NONE )
			victim->armor -= apply_ac( obj_lose, iWear );
		    obj_lose->value[0] -= 1;*/
		    /* 5% -- Altrag */
#ifdef NEW_MONEY
		    obj_lose->cost.gold   = (obj_lose->cost.gold > 0) ?
					    (obj_lose->cost.gold - (obj_lose->cost.gold/20)) : 0;
		    obj_lose->cost.silver = (obj_lose->cost.silver > 0) ?
					    (obj_lose->cost.silver - (obj_lose->cost.silver/20)) : 0;
		    obj_lose->cost.copper = (obj_lose->cost.copper > 0) ?
					    (obj_lose->cost.copper - (obj_lose->cost.copper/20)) : 0;
#else
		    obj_lose->cost     -= (obj_lose->cost / 20);
#endif
/*		    if ( iWear != WEAR_NONE )
			victim->armor += apply_ac( obj_lose, iWear );*/
		}
		break;

	    case ITEM_CONTAINER:
		act(AT_GREEN, "$p fumes and dissolves!",
		    victim, obj_lose, NULL, TO_CHAR );
		extract_obj( obj_lose );
		break;
	    }
	}
    }

    hpch = UMAX( 10, ch->hit );
    dam  = number_range( hpch / 8 + 1, hpch / 4 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_fire_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( level, victim ) )
    {
	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
	    char *msg;

	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted )
	        continue;
	    if ( IS_SET( obj_lose->extra_flags, ITEM_NO_DAMAGE ) )
	      continue;
	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    default:             continue;
	    case ITEM_CONTAINER: msg = "$p ignites and burns!";   break;
	    case ITEM_POTION:    msg = "$p bubbles and boils!";   break;
	    case ITEM_SCROLL:    msg = "$p crackles and burns!";  break;
	    case ITEM_STAFF:     msg = "$p smokes and chars!";    break;
	    case ITEM_WAND:      msg = "$p sparks and sputters!"; break;
	    case ITEM_LENSE:     msg = "$p shrivels and dries!";  break;
	    case ITEM_FOOD:      msg = "$p blackens and crisps!"; break;
	    case ITEM_PILL:      msg = "$p melts and drips!";     break;
	    }

	    act(AT_GREEN, msg, victim, obj_lose, NULL, TO_CHAR );
	    extract_obj( obj_lose );
	}
    }

    hpch = UMAX( 10, ch->hit );
    dam  = number_range( hpch / 8 + 1, hpch / 4 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_frost_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( level, victim ) )
    {
	for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
	{
	    char *msg;

	    obj_next = obj_lose->next_content;
	    if ( obj_lose->deleted )
	        continue;
	    if ( IS_SET( obj_lose->extra_flags, ITEM_NO_DAMAGE ) )
	      continue;
	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    default:            continue;
	    case ITEM_CONTAINER:
	    case ITEM_DRINK_CON:
	    case ITEM_POTION:   msg = "$p freezes and shatters!"; break;
	    }

	    act(AT_WHITE, msg, victim, obj_lose, NULL, TO_CHAR );
	    extract_obj( obj_lose );
	}
    }

    hpch = UMAX( 10, ch->hit );
    dam  = number_range( hpch / 8 + 1, hpch / 4 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int        dam;
    int        hpch;

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
        vch_next = vch->next_in_room;
        if ( vch->deleted )
	    continue;

	if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
	{
	    hpch = UMAX( 10, ch->hit );
	    dam  = number_range( hpch / 8 + 1, hpch / 4 );
	    dam = sc_dam( ch, dam );
	    if ( saves_spell( level, vch ) )
		dam /= 2;
	    spell_poison( gsn_poison, level, ch, vch );
	    damage( ch, vch, dam, sn );
	}
    }
    return;
}



void spell_lightning_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        dam;
    int        hpch;

    hpch = UMAX( 10, ch->hit );
    dam = number_range( hpch / 8 + 1, hpch / 4 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}

/*
 * Code for Psionicist spells/skills by Thelonius
 */
void spell_adrenaline_control ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level - 5;
    af.location	 = APPLY_DEX;
    af.modifier	 = 2;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location	= APPLY_CON;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "You have given yourself an adrenaline rush!\n\r", ch );
    act(AT_BLUE, "$n has given $mself an adrenaline rush!", ch, NULL, NULL,
	TO_ROOM );

    return;
}



void spell_agitation ( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	0,
	 0,  0,  0,  0,  0,      12, 15, 18, 21, 24,
	24, 24, 25, 25, 26,      26, 26, 27, 27, 27,
	28, 28, 28, 29, 29,      29, 30, 30, 30, 31,
	31, 31, 32, 32, 32,      33, 33, 33, 34, 34,
	34, 35, 35, 35, 36,      36, 36, 37, 37, 37,
	38, 39, 40, 41, 42,      43, 44, 45, 46, 47,
	48, 49, 50, 51, 52,      53, 54, 55, 56, 57,
	58, 59, 60, 61, 62,      63, 64, 65, 66, 67,
	68, 69, 70, 71, 72,      73, 74, 75, 76, 77,
	78, 79, 80, 81, 82,      83, 84, 85, 86, 87
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
      dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_aura_sight ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    char      *msg;
    int        ap;

    ap = victim->alignment;

    if ( ap >  700 ) msg = "$N has an aura as white as the driven snow.";
    else if ( ap >  350 ) msg = "$N is of excellent moral character.";
    else if ( ap >  100 ) msg = "$N is often kind and thoughtful.";
    else if ( ap > -100 ) msg = "$N doesn't have a firm moral commitment.";
    else if ( ap > -350 ) msg = "$N lies to $S friends.";
    else if ( ap > -700 ) msg = "Don't bring $N home to meet your family.";
    else msg = "Uh, check please!";

    act(AT_BLUE, msg, ch, NULL, victim, TO_CHAR );
}



void spell_awe ( int sn, int level, CHAR_DATA *ch, void *vo )
  {
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( victim->fighting == ch && !saves_spell( level, victim ) )
    {
	stop_fighting ( victim, TRUE);
	act(AT_BLUE, "$N is in AWE of you!", ch, NULL, victim, TO_CHAR    );
	act(AT_BLUE, "You are in AWE of $n!",ch, NULL, victim, TO_VICT    );
	act(AT_BLUE, "$N is in AWE of $n!",  ch, NULL, victim, TO_NOTVICT );
    }
    return;
}



void spell_ballistic_attack ( int sn, int level, CHAR_DATA *ch, void *vo )
  {
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	 0,
	 3,  4,  4,  5,  6,       6,  6,  7,  7,  7,
	 7,  7,  8,  8,  8,       9,  9,  9, 10, 10,
	10, 11, 11, 11, 12,      12, 12, 13, 13, 13,
	14, 14, 14, 15, 15,      15, 16, 16, 16, 17,
	17, 17, 18, 18, 18,      19, 19, 19, 20, 20,
	21, 22, 23, 24, 25,      26, 27, 28, 29, 30,
	31, 32, 33, 34, 35,      36, 37, 38, 39, 40,
	41, 42, 43, 44, 45,      46, 47, 48, 49, 50,
	51, 52, 53, 54, 55,      56, 57, 58, 59, 60,
	61, 62, 63, 64, 65,      66, 67, 68, 69, 70
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
      dam /= 2;
    act(AT_BLUE, "You chuckle as a stone strikes $N.", ch, NULL, victim,
	TO_CHAR );
    damage( ch, victim, dam, sn);
    return;
}



void spell_biofeedback ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
        return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SANCTUARY;
    affect_to_char( victim, &af );

    send_to_char(AT_WHITE, "You are surrounded by a white aura.\n\r", victim );
    act(AT_WHITE, "$n is surrounded by a white aura.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_cell_adjustment ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int SkNum;

    if ( is_affected( victim, gsn_poison ) )
    {
	affect_strip( victim, gsn_poison );
	send_to_char(AT_BLUE, "A warm feeling runs through your body.\n\r", victim );
	act(AT_BLUE, "$N looks better.", ch, NULL, victim, TO_NOTVICT );
    }

    SkNum = skill_lookup("curse");

    if ( is_affected( victim, SkNum  ) )
    {
	affect_strip( victim, SkNum  );
	send_to_char(AT_BLUE, "You feel better.\n\r", victim );
    }
    send_to_char(AT_BLUE, "Ok.\n\r", ch );
    return;
}

void spell_chaosfield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_CHAOS ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 6 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_CHAOS;
    affect_to_char( victim, &af );

    send_to_char(AT_YELLOW, "You call forth an instance of chaos from the order around you.\n\r", victim );
    act(AT_YELLOW, "$n's body is veiled in an instance or pure chaos.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_bladebarrier( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED2( victim, AFF_BLADE ) )
	return;

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 6 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_BLADE;
    affect_to_char2( victim, &af );

    send_to_char(AT_GREY, "You bring forth thousands of tiny spinning blades about your body.\n\r", victim );
    act(AT_GREY, "$n's body is surrounded by thousands of spinning blades.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_dancing_lights( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED2( victim, AFF_DANCING ) )
	return;

    if ( IS_AFFECTED( victim, AFF_BLIND ) || saves_spell( level, victim ) )
    {
	send_to_char(AT_BLUE, "You have failed.\n\r", ch );
	return;
    }

    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( level / 6 );
    af.location  = APPLY_HITROLL;
    af.modifier  = level/6;
    af.bitvector = AFF_DANCING;
    affect_to_char2( victim, &af );

    act(AT_WHITE, "&.Thou&.sand&.s &.of &.danci&.ng &.ligh&.ts &.surr&.ound &.you&.!&w", victim, NULL, victim, TO_VICT );
    act(AT_GREY, "&W$n's &.body &.is &.surr&.ounded &.by d&.anci&.ng l&.ights.", victim, NULL, NULL, TO_ROOM );
    return;
}


void spell_combat_mind ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
    {
	if ( victim == ch )
	  send_to_char(AT_BLUE, "You already understand battle tactics.\n\r",
		       victim );
	else
	  act(AT_BLUE, "$N already understands battle tactics.",
	      ch, NULL, victim, TO_CHAR );
	return;
    }

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level + 3;
    af.location	 = APPLY_HITROLL;
    af.modifier	 = level / 5;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location	 = APPLY_AC;
    af.modifier	 = - level/2 - 10;
    affect_to_char( victim, &af );

    if ( victim != ch )
	send_to_char(AT_BLUE, "OK.\n\r", ch );
    send_to_char(AT_BLUE, "You gain a keen understanding of battle tactics.\n\r",
		 victim );
    return;
}


/* psi people shouldn't have this powerful healing */
void spell_complete_healing ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

/*    victim->hit = MAX_HIT(victim);*/
    victim->hit = UMIN( victim->hit + 600, MAX_HIT(victim) );
    update_pos( victim );
    if ( ch != victim )
        send_to_char(AT_BLUE, "Ok.\n\r", ch );
    send_to_char(AT_BLUE, "Ahhhhhh...You feel MUCH better!\n\r", victim );
    send_to_char(AT_BLUE, "Have a nice day.\n\r", victim);
/*
    send_to_char(AT_BLUE, "Ahhhhhh...You are completely healed!\n\r", victim );
*/
    return;
}



void spell_control_flames ( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	 0,
	 0,  0,  0,  0,  0,       0,  0, 16, 20, 24,
	28, 32, 35, 38, 40,      42, 44, 45, 45, 45,
	46, 46, 46, 47, 47,      47, 48, 48, 48, 49,
	49, 49, 50, 50, 50,      51, 51, 51, 52, 52,
	52, 53, 53, 53, 54,      54, 54, 55, 55, 55,
	56, 56, 57, 57, 58,      58, 59, 59, 60, 60,
	62, 63, 63, 63, 64,      64, 64, 65, 65, 65,
	72, 73, 73, 73, 74,      74, 74, 75, 75, 75,
	82, 83, 83, 83, 84,      84, 84, 85, 85, 85,
	92, 93, 93, 93, 94,      94, 94, 95, 95, 95
    };
		 int        dam;

    if ( !get_eq_char( ch, WEAR_LIGHT ) )
    {
	send_to_char(AT_RED, "You must be carrying a light source.\n\r", ch );
	return;
    }

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] * 2 );
    dam = sc_dam( ch, dam );
    if ( IS_AFFECTED( victim, AFF_FIRESHIELD ) )
       dam /= 2;
    if ( IS_AFFECTED( victim, AFF_ICESHIELD ) )
       dam += dam/4;
    if ( saves_spell( level, victim ) )
        dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_create_sound ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    char       buf1    [ MAX_STRING_LENGTH ];
    char       buf2    [ MAX_STRING_LENGTH ];
    char       speaker [ MAX_INPUT_LENGTH  ];

    target_name = one_argument( target_name, speaker );

    sprintf( buf1, "%s says '%s'.\n\r", speaker, target_name );
    sprintf( buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name );
    buf1[0] = UPPER( buf1[0] );

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
	if ( !is_name( NULL, speaker, vch->name ) )
	    send_to_char(AT_RED, saves_spell( level, vch ) ? buf2 : buf1, vch );
    }
    return;
}



void spell_death_field ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
/*    CHAR_DATA *vch_next; */
    int        dam;
    int        hpch;

    if ( !IS_EVIL( ch ) )
    {
	send_to_char(AT_RED, "You are not evil enough to do that!\n\r", ch);
	return;
    }

    send_to_char(AT_DGREY, "A black haze emanates from you!\n\r", ch );
    act (AT_DGREY, "A black haze emanates from $n!", ch, NULL, ch, TO_ROOM );

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
      /*vch = vch_next ) */
    {
/*      vch_next = vch->next_in_room;*/
	if ( vch->deleted )
	  continue;
	if ( IS_NPC( ch ) )
	  continue;
    	if ( ch == vch )
	  continue;

	if ( !IS_NPC( ch ) ? IS_NPC( vch ) : IS_NPC( vch ) )
	{
	    hpch = URANGE( 10, ch->hit, 999 );
	    if ( !saves_spell( level, vch )
		&& (   level <= vch->level + 5
		    && level >= vch->level - 5 ) )
            {
		send_to_char(AT_DGREY, "The haze envelops you!\n\r", vch );
		act(AT_DGREY, "The haze envelops $N!",
		    ch, NULL, vch, TO_NOTVICT );
		dam = 4; /* Enough to compensate for sanct. and prot. */
		vch->hit = 1;
		damage( ch, vch, dam, sn );
		update_pos( vch );
            }
	    else
	    {
    		dam = number_range( hpch / 16 + 1, hpch / 8 );
		dam = sc_dam( ch, dam );
   		damage( ch, vch, dam, sn );
	    }
	}
    }
    return;
}



void spell_detonate ( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
		 int        dam;
    if ( ch->race == RACE_ILLITHID )
	ch->wait /= 2;
    dam	     = dice( level, 13 );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) )
        dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_disintegrate ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA  *obj_lose;
    OBJ_DATA  *obj_next;

    if ( !IS_NPC(victim) )
    {
      send_to_char(AT_BLUE, "You failed.\n\r", ch);
      return;
    }

    if ( number_percent( ) < 2 * level && !saves_spell( level, victim ) )
      for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
      {
	  obj_next = obj_lose->next_content;
	  if ( obj_lose->deleted )
	      continue;

	  if ( number_bits( 2 ) != 0 )
	      continue;

	  act(AT_WHITE, "$p disintegrates!",      victim, obj_lose, NULL, TO_CHAR );
	  act(AT_WHITE, "$n's $p disintegrates!", victim, obj_lose, NULL, TO_ROOM );
	  extract_obj( obj_lose ) ;
      }

    if ( !saves_spell( level, victim ) )
    /*
     * Disintegrate char, do not generate a corpse, do not
     * give experience for kill.  Extract_char will take care
     * of items carried/wielded by victim.  Needless to say,
     * it would be bad to be a target of this spell!
     * --- Thelonius (Monk)
     */
    {
	act(AT_WHITE, "You have DISINTEGRATED $N!",         ch, NULL, victim, TO_CHAR );
	act(AT_WHITE, "You have been DISINTEGRATED by $n!", ch, NULL, victim, TO_VICT );
	act(AT_WHITE, "$n's spell DISINTEGRATES $N!",       ch, NULL, victim, TO_ROOM );

	if ( IS_NPC( victim ) )
	    extract_char( victim, TRUE );
	else
	    extract_char( victim, FALSE );
    }
    return;
}



void spell_disrupt( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  CHAR_DATA *vch;
  AFFECT_DATA af;
  bool negchar = FALSE; /* Is a negative value good..? */
  int val = 0;

  af.type = sn;
  af.duration = (level * 2) / 3;
  af.level = level;
  af.bitvector = 0;

  while ( !val )
  {
    af.location = number_range( 1, 26 );

    switch (af.location)
    {
    case APPLY_STR:
    case APPLY_DEX:
    case APPLY_INT:
    case APPLY_WIS:
    case APPLY_CON:
      val = (level / 34) + 1;
      break;
    case APPLY_MANA:
      val = ((level * 3) / 2) + 1;
      break;
    case APPLY_AC:
      val = ((level * 3) / 2) + 1;
      negchar = TRUE;
      break;
    case APPLY_HITROLL:
    case APPLY_DAMROLL:
      val = (level / 2) + 1;
      break;
    case APPLY_SAVING_PARA:
    case APPLY_SAVING_ROD:
    case APPLY_SAVING_PETRI:
    case APPLY_SAVING_BREATH:
    case APPLY_SAVING_SPELL:
      val = ((level * 2) / 5) + 1;
      negchar = TRUE;
      break;
    case APPLY_BP:
      val = ((level * 3) / 10) + 1;
      break;
    }
  }

  for ( af.modifier = number_range( -val, val ); af.modifier == 0;
        af.modifier = number_range( -val, val ) );

  if ( negchar )
  {
    if ( af.modifier < 0 )
      vch = ch;
    else
      vch = victim;
  }
  else
  {
    if ( af.modifier < 0 )
      vch = victim;
    else
      vch = ch;
  }
  if ( af.location == APPLY_MANA && is_class( vch, CLASS_VAMPIRE ) )
  {
    af.modifier /= 4;
    af.location = APPLY_BP;
  }
  if ( af.location == APPLY_BP && !is_class( vch, CLASS_VAMPIRE ) )
  {
    if ( af.modifier > 0 )
      af.modifier = number_range( 1, (level * 3) / 2 );
    else
      af.modifier = number_range( -((level * 3) / 2), -1 );
    af.location = APPLY_MANA;
  }
  if ( !is_affected( vch, sn ) && (vch != victim ||
				   !saves_spell( level,  victim )) &&
       number_bits( 8 ) == 0 )
  {
    affect_to_char( vch, &af );
    if ( ch == vch )
    {
      act( AT_PINK, "You disrupt yourself!", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "$n disrupts $mself!", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
      act( AT_PURPLE, "You disrupt $N!", ch, NULL, victim, TO_CHAR );
      act( AT_PURPLE, "$n disrupts $N!", ch, NULL, victim, TO_NOTVICT );
      act( AT_PURPLE, "$n disrupts you!", ch, NULL, victim, TO_VICT );
      switch( af.location )
      {
      case APPLY_MANA:
	vch->mana = URANGE( 0, vch->mana, MAX_MANA(vch));
	break;
      case APPLY_BP:
	vch->bp = URANGE( 0, vch->bp, MAX_BP(vch));
	break;
      }
    }
  }

  damage( ch, victim, number_range( (level * 2) / 3, level ), sn );
  return;
}



void spell_displacement ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level - 4;
    af.location	 = APPLY_AC;
    af.modifier	 = 4 - level;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char(AT_GREY, "Your form shimmers, and you appear displaced.\n\r",
		 victim );
    act(AT_GREY, "$N shimmers and appears in a different location.",
	ch, NULL, victim, TO_NOTVICT );
    return;
}



void spell_domination ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim == ch )
    {
	send_to_char(C_DEFAULT, "Dominate yourself?  You're weird.\n\r", ch );
	return;
    }
    if ( !IS_NPC( victim ) )
       return;

    if (   IS_AFFECTED( victim, AFF_CHARM )
	|| IS_AFFECTED( ch,     AFF_CHARM )
	|| level < victim->level
	|| saves_spell( level, victim ) )
	return;

    if ( victim->master )
        stop_follower( victim );
    add_follower( victim, ch );

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = number_fuzzy( level / 4 );
    af.location	 = APPLY_NONE;
    af.modifier	 = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );

    act(AT_BLUE, "Your will dominates $N!", ch, NULL, victim, TO_CHAR );
    act(AT_BLUE, "Your will is dominated by $n!", ch, NULL, victim, TO_VICT );
    return;
}



void spell_ectoplasmic_form ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_PASS_DOOR ) )
        return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = number_fuzzy( level / 4 );
    af.location	 = APPLY_NONE;
    af.modifier	 = 0;
    af.bitvector = AFF_PASS_DOOR;
    affect_to_char( victim, &af );

    send_to_char(AT_GREY, "You turn translucent.\n\r", victim );
    act(AT_GREY, "$n turns translucent.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_ego_whip ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || saves_spell( level, victim ) )
        return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level;
    af.location	 = APPLY_HITROLL;
    af.modifier	 = -2;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location	 = APPLY_SAVING_SPELL;
    af.modifier	 = 2;
    affect_to_char( victim, &af );

    af.location	 = APPLY_AC;
    af.modifier	 = level / 2;
    affect_to_char( victim, &af );

    act(AT_BLUE, "You ridicule $N about $S childhood.", ch, NULL, victim, TO_CHAR    );
    send_to_char(AT_BLUE, "Your ego takes a beating.\n\r", victim );
    act(AT_BLUE, "$N's ego is crushed by $n!",          ch, NULL, victim, TO_NOTVICT );

    return;
}



void spell_energy_containment ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level / 2 + 7;
    af.modifier	 = -level / 3;
    af.location  = APPLY_SAVING_SPELL;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "You can now absorb some forms of energy.\n\r", ch );
    return;
}



void spell_enhance_armor (int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA    *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    if ( obj->item_type != ITEM_ARMOR
	|| IS_OBJ_STAT( obj, ITEM_MAGIC )
	|| obj->affected )
    {
	send_to_char(AT_BLUE, "That item cannot be enhanced.\n\r", ch );
	return;
    }

    if ( !affect_free )
    {
	paf	    = alloc_perm( sizeof( *paf ) );
    }
    else
    {
	paf         = affect_free;
	affect_free = affect_free->next;
    }

    paf->type	   = sn;
    paf->duration  = -1;
    paf->location  = APPLY_AC;
    paf->bitvector = 0;
    paf->next	   = obj->affected;
    obj->affected  = paf;

    if ( number_percent() < ch->pcdata->learned[sn]/2
	+ 3 * ( ch->level - obj->level ) )

    /* Good enhancement */
    {
	paf->modifier   = -level / 5;

	     if ( IS_GOOD( ch ) )
	{
	    SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
	    act(AT_BLUE, "$p glows.",   ch, obj, NULL, TO_CHAR );
	}
	else if ( IS_EVIL( ch ) )
        {
	    SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
	    act(AT_RED, "$p glows.",    ch, obj, NULL, TO_CHAR );
	}
	else
	{
	    SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
	    SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
	    act(AT_YELLOW, "$p glows.", ch, obj, NULL, TO_CHAR );
	}

	send_to_char(AT_BLUE, "Ok.\n\r", ch );
    }
    else
    /* Bad Enhancement ... opps! :) */
    {
	paf->modifier   = level / 8;
#ifdef NEW_MONEY
	obj->cost.gold = obj->cost.silver = obj->cost.copper = 0;
#else
	obj->cost       = 0;
#endif
	SET_BIT( obj->extra_flags, ITEM_NODROP );
	act(AT_DGREY, "$p turns black.", ch, obj, NULL, TO_CHAR );
    }

    return;
}



void spell_enhanced_strength ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level;
    af.location	 = APPLY_STR;
    af.modifier	 = 1 + ( level >= 15 ) + ( level >= 25 );
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "You are HUGE!\n\r", victim );
    return;
}



void spell_flesh_armor ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level;
    af.location	 = APPLY_AC;
    af.modifier	 = -45;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "Your flesh turns to steel.\n\r", victim );
    act(AT_BLUE, "$N's flesh turns to steel.", ch, NULL, victim, TO_NOTVICT);
    return;
}



void spell_inertial_barrier ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) || IS_AFFECTED2( gch, AFF_INERTIAL ) )
	    continue;

	act(AT_BLUE, "An inertial barrier forms around $n.", gch, NULL, NULL,
	    TO_ROOM );
	send_to_char(AT_BLUE, "An inertial barrier forms around you.\n\r", gch );

	af.type	     = sn;
        af.level     = level;
	af.duration  = 24;
	af.modifier  = 0;
	af.location  = APPLY_NONE;
	af.bitvector = AFF_INERTIAL;
	affect_to_char2( gch, &af );
    }
    return;
}



void spell_inflict_pain ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int dam = dice( 2, 10 ) + level / 2;
     dam = sc_dam( ch, dam );
    damage( ch, (CHAR_DATA *) vo, dam, sn );
    return;
}



void spell_intellect_fortress ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *gch;
    AFFECT_DATA af;

    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) || is_affected( gch, sn ) )
	    continue;

	send_to_char(AT_BLUE, "A virtual fortress forms around you.\n\r", gch );
	act(AT_BLUE, "A virtual fortress forms around $N.", gch, NULL, gch, TO_ROOM );

	af.type	     = sn;
        af.level     = level;
	af.duration  = 24;
	af.location  = APPLY_AC;
	af.modifier  = -40;
	af.bitvector = 0;
	affect_to_char( gch, &af );
    }
    return;
}



void spell_lend_health ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int        hpch;

    if ( ch == victim )
    {
	send_to_char(AT_BLUE, "Lend health to yourself?  Easily done.\n\r", ch );
	return;
    }
    hpch = UMIN( 50, MAX_HIT(victim) - victim->hit );
    if ( hpch == 0 )
    {
	act(AT_BLUE, "Nice thought, but $N doesn't need healing.", ch, NULL,
	    victim, TO_CHAR );
	return;
    }
    if ( ch->hit-hpch < 50 )
    {
	send_to_char(AT_BLUE, "You aren't healthy enough yourself!\n\r", ch );
	return;
    }
    victim->hit += hpch;
    ch->hit     -= hpch;
    update_pos( victim );
    update_pos( ch );

    act(AT_BLUE, "You lend some of your health to $N.", ch, NULL, victim, TO_CHAR );
    act(AT_BLUE, "$n lends you some of $s health.",     ch, NULL, victim, TO_VICT );

    return;
}



void spell_levitation ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED( victim, AFF_FLYING ) )
        return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level + 3;
    af.location	 = APPLY_NONE;
    af.modifier	 = 0;
    af.bitvector = AFF_FLYING;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "Your feet rise off the ground.\n\r", victim );
    act(AT_BLUE, "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_mental_barrier ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = 24;
    af.location	 = APPLY_AC;
    af.modifier	 = -20;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "You erect a mental barrier around yourself.\n\r",
		 victim );
    return;
}



void spell_mind_thrust ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int dam = dice( 1, 10 ) + level / 2;
    dam = sc_dam( ch, dam );
    damage( ch, (CHAR_DATA *) vo, dam, sn );
    return;
}



void spell_project_force ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int dam = dice( 4, 6 ) + level;
    damage( ch, (CHAR_DATA *) vo, dam, sn );
    return;
}



void spell_psionic_blast ( int sn, int level, CHAR_DATA *ch, void *vo )
{
                 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	  0,
	  0,   0,   0,   0,   0,        0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,        0,  45,  50,  55,  60,
	 64,  68,  72,  76,  80,       82,  84,  86,  88,  90,
	 92,  94,  96,  98, 100,      102, 104, 106, 108, 100,
	112, 114, 116, 118, 120,      122, 124, 126, 128, 130,
	132, 134, 136, 138, 140,      142, 144, 146, 148, 150,
	152, 154, 156, 158, 160,      162, 164, 166, 168, 170,
	182, 184, 186, 188, 190,      192, 194, 196, 198, 200,
	202, 204, 206, 208, 210,      212, 214, 216, 218, 220,
	222, 224, 226, 228, 230,      232, 234, 236, 238, 240
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	     = number_range( dam_each[level] / 2, dam_each[level] );
    dam = sc_dam( ch, dam );
    if ( saves_spell( level, victim ) && ch->race != RACE_ILLITHID )
        dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_psychic_crush ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    int dam = dice( 3, 5 ) + level;
    damage( ch, (CHAR_DATA *) vo, dam, sn );
    return;
}



void spell_psychic_drain ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || saves_spell( level, victim ) )
        return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level / 2;
    af.location	 = APPLY_STR;
    af.modifier	 = -1 - ( level >= 10 ) - ( level >= 20 ) - ( level >= 30 );
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char(AT_GREEN, "You feel drained.\n\r", victim );
    act(AT_BLUE, "$n appears drained of strength.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_psychic_healing ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    heal = dice( 3, 6 ) + 2 * level / 3 ;
    victim->hit = UMIN( victim->hit + heal, MAX_HIT(victim) );
    update_pos( victim );

    send_to_char(AT_BLUE, "You feel better!\n\r", victim );
    return;
}



void spell_share_strength ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim == ch )
    {
	send_to_char(AT_BLUE, "You can't share strength with yourself.\n\r", ch );
	return;
    }
    if ( is_affected( victim, sn ) )
    {
	act(AT_BLUE, "$N already shares someone's strength.", ch, NULL, victim,
	    TO_CHAR );
	return;
    }
    if ( get_curr_str( ch ) <= 5 )
    {
	send_to_char(AT_BLUE, "You are too weak to share your strength.\n\r", ch );
	return;
    }

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level;
    af.location	 = APPLY_STR;
    af.modifier	 =  1 + ( level >= 20 ) + ( level >= 30 );
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.modifier	 = -1 - ( level >= 20 ) - ( level >= 30 );
    affect_to_char( ch,     &af );

    act(AT_BLUE, "You share your strength with $N.", ch, NULL, victim, TO_CHAR );
    act(AT_BLUE, "$n shares $s strength with you.",  ch, NULL, victim, TO_VICT );
    return;
}



void spell_thought_shield ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
        return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = level;
    af.location	 = APPLY_AC;
    af.modifier	 = -20;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char(AT_BLUE, "You have created a shield around yourself.\n\r", ch );
    return;
}

void spell_ultrablast ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int        dam;
    int        hpch;

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
        vch_next = vch->next_in_room;
	if ( vch->deleted )
	    continue;

	if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
	{
	    hpch = UMAX( 10, ch->hit );
	    dam  = number_range( hpch / 8, hpch / 3 );
	    dam = sc_dam( ch, dam );
	    if ( saves_spell( level, vch ) )
	        dam /= 2;
	    damage( ch, vch, dam, sn );
	}
    }
    return;
}

/* XORPHOX summon mobs */
void spell_summon_swarm(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *mob;
  CHAR_DATA *fch;
  AFFECT_DATA af;
  char buf [ MAX_STRING_LENGTH ];
  int mana;
  if(ch->summon_timer > 0)
  {
    send_to_char(AT_BLUE,
     "You casted the spell, but nothing appears.\n\r", ch);
    return;
  }

  mob = create_mobile(get_mob_index(MOB_VNUM_INSECTS));
  mob->level = URANGE(15, level, 55) - 5;
  mob->perm_hit = mob->level * 20 + dice(1,mob->level);
  mob->hit = MAX_HIT(mob);
  mob->summon_timer = level/5;
  ch->summon_timer = 10;
  char_to_room(mob, ch->in_room);
  act(AT_BLUE, "You summon $N.", ch, NULL, mob, TO_CHAR);
  mana = is_class( ch, CLASS_VAMPIRE ) ? level / 2 : level * 2;
  if ( MT( ch ) < mana )
    {
    sprintf( buf, "%sYou don't have enough %s to bind $N!",
	   (is_class( ch, CLASS_VAMPIRE )) ? "&r" : "&R",
	   (is_class( ch, CLASS_VAMPIRE )) ? "blood" : "mana" );
    act(AT_WHITE, buf, ch, NULL, mob, TO_CHAR );
    extract_char( mob, TRUE );
    return;
    }
    if ( is_class( ch, CLASS_VAMPIRE ) ) {
      ch->bp -= mana;
    } else {
      ch->mana -= mana;
    }
  act(AT_GREEN, "$n summons $N.", ch, NULL, mob, TO_ROOM);

  mob->master = ch;
  mob->leader = ch;
  af.type      = skill_lookup("charm person");
  af.level     = level;
  af.duration  = -1;
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_CHARM;
  affect_to_char(mob, &af);

  if(ch->position == POS_FIGHTING)
  {
    act(AT_BLUE, "$n rescues you!", mob, NULL, ch, TO_VICT    );
    act(AT_BLUE, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT );

    fch = ch->fighting;
    stop_fighting(fch, FALSE );
    stop_fighting( ch, FALSE );
    fch->fighting = mob;
    mob->fighting = fch;
/*    set_fighting(mob, fch);
    set_fighting(fch, mob);*/
  }
  return;
}

void spell_summon_pack(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *mob;
  CHAR_DATA *fch;
  AFFECT_DATA af;
  char buf [ MAX_STRING_LENGTH ];
  int mana;

  if(ch->summon_timer > 0)
  {
    send_to_char(AT_BLUE,
     "You casted the spell, but nothing appears.\n\r", ch);
    return;
  }

  mob = create_mobile(get_mob_index(MOB_VNUM_WOLFS));
  mob->level = URANGE(31, level, 90) - 5;
  mob->perm_hit = mob->level * 20 + dice(1,mob->level);
  mob->hit = MAX_HIT(mob);
  mob->summon_timer = level/8;
  ch->summon_timer = 15;
  char_to_room(mob, ch->in_room);
  act(AT_GREEN, "You summon $N.", ch, NULL, mob, TO_CHAR);
  mana = is_class( ch, CLASS_VAMPIRE ) ? level / 2 : level * 2;
  if ( MT( ch ) < mana )
    {
    sprintf( buf, "%sYou don't have enough %s to bind $N!",
	   is_class( ch, CLASS_VAMPIRE ) ? "&r" : "&R",
	   is_class( ch, CLASS_VAMPIRE ) ? "blood" : "mana" );
    act(AT_WHITE, buf, ch, NULL, mob, TO_CHAR );
    extract_char( mob, TRUE );
    return;
    }
    if ( is_class( ch, CLASS_VAMPIRE ) ) {
      ch->bp -= mana;
    } else {
      ch->mana -= mana;
    }
  act(AT_GREEN, "$N comes to $n aid.", ch, NULL, mob, TO_ROOM);

  mob->master = ch;
  mob->leader = ch;
  af.type      = skill_lookup("charm person");
  af.level     = level;
  af.duration  = -1;
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_CHARM;
  affect_to_char(mob, &af);

  if(ch->position == POS_FIGHTING)
  {
    act(AT_BLUE, "$n rescues you!", mob, NULL, ch, TO_VICT    );
    act(AT_BLUE, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT );

    fch = ch->fighting;
    stop_fighting(fch, FALSE );
    stop_fighting( ch, FALSE );
    set_fighting(mob, fch);
    set_fighting(fch, mob);
  }
  return;
}

void spell_summon_demon(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *mob;
  CHAR_DATA *fch;
  AFFECT_DATA af;
  char buf [ MAX_STRING_LENGTH ];
  int mana;
  if(ch->summon_timer > 0)
  {
    send_to_char(AT_RED,
     "You casted the spell, but nothing appears.\n\r", ch);
    return;
  }

  mob = create_mobile(get_mob_index(MOB_VNUM_DEMON));
  mob->level = URANGE(51, level, 100) - 5;
  mob->perm_hit = mob->level * 20 + dice(1,mob->level);
  mob->hit = MAX_HIT(mob);
  mob->summon_timer = level/10;
  ch->summon_timer = 16;
  char_to_room(mob, ch->in_room);
  act(AT_RED, "You summon $N from the abyss.", ch, NULL, mob, TO_CHAR);
  mana = is_class( ch, CLASS_VAMPIRE ) ? level / 2 : level * 2;
  if ( MT( ch ) < mana )
    {
    sprintf( buf, "%sYou don't have enough %s to bind $N!",
	   is_class( ch, CLASS_VAMPIRE ) ? "&r" : "&R",
	   is_class( ch, CLASS_VAMPIRE ) ? "blood" : "mana" );
    act(AT_WHITE, buf, ch, NULL, mob, TO_CHAR );
    extract_char( mob, TRUE );
    return;
    }
    if ( is_class( ch, CLASS_VAMPIRE ) ) {
      ch->bp -= mana;
    } else {
      ch->mana -= mana;
    }
  act(AT_RED, "$n summons $N from the abyss.", ch, NULL, mob, TO_ROOM);

  mob->master = ch;
  mob->leader = ch;
  af.type      = skill_lookup("charm person");
  af.level     = level;
  af.duration  = -1;
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_CHARM;
  affect_to_char(mob, &af);

  if(ch->position == POS_FIGHTING)
  {
    act(AT_RED, "$n rescues you!", mob, NULL, ch, TO_VICT    );
    act(AT_RED, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT );

    fch = ch->fighting;
    stop_fighting(fch, FALSE );
    stop_fighting( ch, FALSE );
    set_fighting(mob, fch);
    set_fighting(fch, mob);
  }
  return;
}

void spell_summon_angel(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *mob;
  CHAR_DATA *fch;
  AFFECT_DATA af;
  char buf[ MAX_STRING_LENGTH ];
  int mana;
  if(ch->summon_timer > 0)
  {
    send_to_char(AT_WHITE,
     "You casted the spell, but nothing appears.\n\r", ch);
    return;
  }

  mob = create_mobile(get_mob_index(MOB_VNUM_ANGEL));
  mob->level = URANGE(51, level, 100) - 5;
  mob->perm_hit = mob->level * 20 + dice(10,mob->level);
  mob->hit = MAX_HIT(mob);
  mob->summon_timer = level/10;
  ch->summon_timer = 16;
  char_to_room(mob, ch->in_room);
  act(AT_WHITE, "You summon $N from heaven.", ch, NULL, mob, TO_CHAR);
  mana = is_class( ch, CLASS_VAMPIRE ) ? level / 2 : level * 2;
  if ( MT( ch ) < mana )
    {
    sprintf( buf, "%sYou don't have enough %s to bind $N!",
	   is_class( ch, CLASS_VAMPIRE ) ? "&r" : "&R",
	   is_class( ch, CLASS_VAMPIRE ) ? "blood" : "mana" );
    act(AT_WHITE, buf, ch, NULL, mob, TO_CHAR );
    extract_char( mob, TRUE );
    return;
    }
    if ( is_class( ch, CLASS_VAMPIRE ) ) {
      ch->bp -= mana;
    } else {
      ch->mana -= mana;
    }
  act(AT_WHITE, "$n calls forth $N from Heaven.", ch, NULL, mob, TO_ROOM);

  mob->master = ch;
  mob->leader = ch;
  af.type      = skill_lookup("charm person");
  af.level     = level;
  af.duration  = -1;
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_CHARM;
  affect_to_char(mob, &af);

  if(ch->position == POS_FIGHTING)
  {
    act(AT_WHITE, "$n rescues you!", mob, NULL, ch, TO_VICT    );
    act(AT_WHITE, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT );

    fch = ch->fighting;
    stop_fighting(fch, FALSE );
    stop_fighting( ch, FALSE );
    set_fighting(mob, fch);
    set_fighting(fch, mob);
  }
  return;
}

void spell_summon_shadow(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *mob;
  CHAR_DATA *fch;
  AFFECT_DATA af;
  char buf[MAX_STRING_LENGTH];
  int mana;
  if(ch->summon_timer > 0)
  {
    send_to_char(AT_WHITE,
     "You casted the spell, but nothing appears.\n\r", ch);
    return;
  }

  mob = create_mobile(get_mob_index(MOB_VNUM_SHADOW));
  mob->level = URANGE(51, level, 100) - 20;
  mob->perm_hit = mob->level * 20 + dice(10,mob->level);
  mob->hit = MAX_HIT(mob);
  mob->summon_timer = level/10;
  ch->summon_timer = 16;
  char_to_room(mob, ch->in_room);
  act(AT_GREY, "You summon $N from the shadow plane.", ch, NULL, mob, TO_CHAR);
  mana = is_class( ch, CLASS_VAMPIRE ) ? level / 2 : level * 2;
  if ( MT( ch ) < mana )
    {
    sprintf( buf, "%sYou don't have enough %s to bind $N!",
	   is_class( ch, CLASS_VAMPIRE ) ? "&r" : "&R",
	   is_class( ch, CLASS_VAMPIRE ) ? "blood" : "mana" );
    act(AT_WHITE, buf, ch, NULL, mob, TO_CHAR );
    extract_char( mob, TRUE );
    return;
    }
    if ( is_class( ch, CLASS_VAMPIRE ) ) {
      ch->bp -= mana;
    } else {
      ch->mana -= mana;
    }
  act(AT_GREY, "$n calls forth $N from the shadow plane.", ch, NULL, mob, TO_ROOM);

  mob->master = ch;
  mob->leader = ch;
  af.type      = skill_lookup("charm person");
  af.level     = level;
  af.duration  = -1;
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_CHARM;
  affect_to_char(mob, &af);

  if(ch->position == POS_FIGHTING)
  {
    act(AT_WHITE, "$n rescues you!", mob, NULL, ch, TO_VICT    );
    act(AT_WHITE, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT );

    fch = ch->fighting;
    stop_fighting(fch, FALSE );
    stop_fighting( ch, FALSE );
    set_fighting(mob, fch);
    set_fighting(fch, mob);
  }
  return;
}


void spell_summon_trent(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *mob;
  CHAR_DATA *fch;
  AFFECT_DATA af;
  char buf[MAX_STRING_LENGTH];
  int mana;
  if(ch->summon_timer > 0)
  {
    send_to_char(AT_WHITE,
     "You casted the spell, but nothing appears.\n\r", ch);
    return;
  }

  mob = create_mobile(get_mob_index(MOB_VNUM_TRENT));
  mob->level = URANGE(51, level, 100) - 10;
  mob->perm_hit = mob->level * 20 + dice(20,mob->level);
  mob->hit = MAX_HIT(mob);
  mob->summon_timer = level/10;
  ch->summon_timer = 16;
  char_to_room(mob, ch->in_room);
  act(AT_ORANGE, "You summon $N from the plane of nature.", ch, NULL, mob, TO_CHAR);
  mana = is_class( ch, CLASS_VAMPIRE ) ? level / 2 : level * 2;
  if ( MT( ch ) < mana )
    {
    sprintf( buf, "%sYou don't have enough %s to bind $N!",
	   is_class( ch, CLASS_VAMPIRE ) ? "&r" : "&R",
	   is_class( ch, CLASS_VAMPIRE ) ? "blood" : "mana" );
    act(AT_WHITE, buf, ch, NULL, mob, TO_CHAR );
    extract_char( mob, TRUE );
    return;
    }
    if ( is_class( ch, CLASS_VAMPIRE ) ) {
      ch->bp -= mana;
    } else {
      ch->mana -= mana;
    }
  act(AT_ORANGE, "$n calls forth $N from the plane of nature.", ch, NULL, mob, TO_ROOM);

  mob->master = ch;
  mob->leader = ch;
  af.type      = skill_lookup("charm person");
  af.level     = level;
  af.duration  = -1;
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_CHARM;
  affect_to_char(mob, &af);

  if(ch->position == POS_FIGHTING)
  {
    act(AT_WHITE, "$n rescues you!", mob, NULL, ch, TO_VICT    );
    act(AT_WHITE, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT );

    fch = ch->fighting;
    stop_fighting(fch, FALSE );
    stop_fighting( ch, FALSE );
    set_fighting(mob, fch);
    set_fighting(fch, mob);
  }
  return;
}

void spell_summon_beast(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *mob;
  CHAR_DATA *fch;
  AFFECT_DATA af;
  char        buf[MAX_STRING_LENGTH];
  int	      mana;
  char       *beast;

  if(ch->summon_timer > 0)
  {
    send_to_char(AT_WHITE,
     "You casted the spell, but nothing appears.\n\r", ch);
    return;
  }
  switch (number_bits( 4 ) )
  {
    case 0: beast = "horse"; break;
    case 1: beast = "cow"; break;
    case 2: beast = "bear"; break;
    case 3: beast = "lion"; break;
    case 4: beast = "bobcat"; break;
    case 5: beast = "mongoose"; break;
    case 6: beast = "rattle snake"; break;
    case 7: beast = "monkey"; break;
    default: beast = "tigeress"; break;
  }

  mob = create_mobile(get_mob_index(MOB_VNUM_BEAST));
  sprintf(buf, mob->short_descr, beast);
  free_string( mob->short_descr );
  mob->short_descr = str_dup(buf);
  sprintf(buf, mob->long_descr, beast, ch->name);
  free_string( mob->long_descr );
  mob->long_descr = str_dup(buf);
  mob->level = URANGE(51, level, 100) - 20;
  mob->perm_hit = mob->level * 20 + dice(10,mob->level);
  mob->hit = MAX_HIT(mob);
  mob->summon_timer = level/10;
  ch->summon_timer = 16;
  char_to_room(mob, ch->in_room);
  act(AT_GREEN, "You call $N from the forests.", ch, NULL, mob, TO_CHAR);
  mana = is_class( ch, CLASS_VAMPIRE ) ? level / 2 : level * 2;
  if ( MT( ch ) < mana )
    {
    sprintf( buf, "%sYou don't have enough %s to bind $N!",
	   is_class( ch, CLASS_VAMPIRE ) ? "&r" : "&R",
	   is_class( ch, CLASS_VAMPIRE ) ? "blood" : "mana" );
    act(AT_WHITE, buf, ch, NULL, mob, TO_CHAR );
    extract_char( mob, TRUE );
    return;
    }
    if ( is_class( ch, CLASS_VAMPIRE ) ) {
      ch->bp -= mana;
    } else {
      ch->mana -= mana;
    }
  act(AT_GREEN, "$n calls forth $N from the forests.", ch, NULL, mob, TO_ROOM);

  mob->master = ch;
  mob->leader = ch;
  af.type      = skill_lookup("charm person");
  af.level     = level;
  af.duration  = -1;
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_CHARM;
  affect_to_char(mob, &af);

  if(ch->position == POS_FIGHTING)
  {
    act(AT_WHITE, "$n rescues you!", mob, NULL, ch, TO_VICT    );
    act(AT_WHITE, "$n rescues $N!",  mob, NULL, ch, TO_NOTVICT );

    fch = ch->fighting;
    stop_fighting(fch, FALSE );
    stop_fighting( ch, FALSE );
    set_fighting(mob, fch);
    set_fighting(fch, mob);
  }
  return;
}

void perm_spell(CHAR_DATA *victim, int sn)
{
  AFFECT_DATA *af;

  if(is_affected(victim, sn))
  {
    for(af = victim->affected; af != NULL; af = af->next)
    {
      if(af->type == sn)
      {
        af->duration = -1;
      }
    }
  }
  return;
}

int spell_duration(CHAR_DATA *victim, int sn)
{
  AFFECT_DATA *af;

  if(is_affected(victim, sn))
  {
    for(af = victim->affected; af != NULL; af = af->next)
    {
      if(af->type == sn)
      {
        return af->duration;
      }
    }
  }
  return -2;
}
/* RT save for dispels */
/* modified for envy -XOR */
bool saves_dispel(int dis_level, int spell_level, int duration)
{
  int save;

  if(duration == -1)
    spell_level += 5;/* very hard to dispel permanent effects */
  save = 50 + (spell_level - dis_level) * 5;
  save = URANGE( 5, save, 95 );
  return number_percent() < save;
}

/* co-routine for dispel magic and cancellation */
bool check_dispel(int dis_level, CHAR_DATA *victim, int sn)
{
  AFFECT_DATA *af;

  if (is_affected(victim, sn))
  {
    for(af = victim->affected; af != NULL; af = af->next)
    {
      if(af->type == sn)
      {
	if ( !saves_spell(dis_level,victim) )
/*	if(!saves_dispel(dis_level,victim->level,af->duration))*/
        {
          affect_strip(victim,sn);
          if(skill_table[sn].msg_off)
          {
            send_to_char(C_DEFAULT, skill_table[sn].msg_off, victim );
            send_to_char(C_DEFAULT, "\n\r", victim );
            if(skill_table[sn].room_msg_off)
            {
              act(C_DEFAULT, skill_table[sn].room_msg_off,
	      victim, NULL, NULL, TO_ROOM);
            }
          }
	return TRUE;
	}
	else
          af->level--;
      }
    }
  }
  return FALSE;
}

/* Mobs built with spells only have the flag.
 * These function dispels those spells
 *  -Decklarean
 */

void check_dispel_aff( CHAR_DATA *victim, bool * found, int level, const char * spell, long vector )
{
  int sn;
  sn = skill_lookup(spell);
  if(IS_AFFECTED(victim,vector)
   && !saves_spell(level, victim)
/*   && !saves_dispel(level, victim->level,1) */
   && !is_affected(victim,sn) )
  {
    *found = TRUE;
    REMOVE_BIT(victim->affected_by,vector);
    if(skill_table[sn].msg_off)
    {
      act(C_DEFAULT, skill_table[sn].msg_off,
            victim, NULL, NULL, TO_CHAR);
      if(skill_table[sn].room_msg_off)
      {
        act(C_DEFAULT, skill_table[sn].room_msg_off,
            victim, NULL, NULL, TO_ROOM);
      }
    }
    if ( vector == AFF_FLYING )
     check_nofloor( victim );
  }
}


void check_dispel_aff2( CHAR_DATA *victim, bool * found, int level, const char * spell, long vector )
{
  int sn;
  sn = skill_lookup(spell);
  if(IS_AFFECTED2(victim,vector)
   && !saves_dispel(level, victim->level,-1)
   && !is_affected(victim,sn) )
  {
    *found = TRUE;
    REMOVE_BIT(victim->affected_by2,vector);
    if(skill_table[sn].msg_off)
    {
      act(C_DEFAULT, skill_table[sn].msg_off,
            victim, NULL, NULL, TO_CHAR);
      if(skill_table[sn].room_msg_off)
      {
        act(C_DEFAULT, skill_table[sn].room_msg_off,
        victim, NULL, NULL, TO_ROOM);
      }
    }
    if ( vector == AFF_FLYING )
     check_nofloor( victim );
  }
}

bool dispel_flag_only_spells( int level,  CHAR_DATA * victim )
{
bool found;
found = FALSE;

check_dispel_aff ( victim, &found, level, "blindness",AFF_BLIND);
check_dispel_aff ( victim, &found, level, "charm person",AFF_CHARM);
check_dispel_aff ( victim, &found, level, "curse",AFF_CURSE);
check_dispel_aff ( victim, &found, level, "detect evil",AFF_DETECT_EVIL);
check_dispel_aff ( victim, &found, level, "detect hidden",AFF_DETECT_HIDDEN);
check_dispel_aff ( victim, &found, level, "detect invis",AFF_DETECT_INVIS);
check_dispel_aff ( victim, &found, level, "detect magic",AFF_DETECT_MAGIC);
check_dispel_aff ( victim, &found, level, "faerie fire",AFF_FAERIE_FIRE	);
check_dispel_aff ( victim, &found, level, "fireshield",	AFF_FIRESHIELD	);
check_dispel_aff ( victim, &found, level, "flaming",AFF_FLAMING		);
check_dispel_aff ( victim, &found, level, "fly",AFF_FLYING);
check_dispel_aff ( victim, &found, level, "haste",AFF_HASTE);
check_dispel_aff ( victim, &found, level, "iceshield",AFF_ICESHIELD		);
check_dispel_aff ( victim, &found, level, "infravision",AFF_INFRARED		);
check_dispel_aff ( victim, &found, level, "invis",AFF_INVISIBLE		);
check_dispel_aff ( victim, &found, level, "pass door",AFF_PASS_DOOR		);
check_dispel_aff ( victim, &found, level, "protection evil",AFF_PROTECT		);
check_dispel_aff ( victim, &found, level, "sanctuary",AFF_SANCTUARY		);
check_dispel_aff ( victim, &found, level, "shockshield",AFF_SHOCKSHIELD		);
check_dispel_aff ( victim, &found, level, "sleep",AFF_SLEEP);
check_dispel_aff2( victim, &found, level, "detect good",AFF_DETECT_GOOD		);
check_dispel_aff2( victim, &found, level, "field of decay",	AFF_FIELD);
check_dispel_aff2( victim, &found, level, "protection good",AFF_PROTECTION_GOOD	);
check_dispel_aff2( victim, &found, level, "true sight",AFF_TRUESIGHT		);

return found;
}

/* New dispel magic by Decklarean
 * The old way was just to stupid. :>
 * This will dispel all magic spells.
 */

void spell_dispel_magic ( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA *paf;
  bool found;
  if (saves_spell(level, victim))
  {
    send_to_char(AT_RED, "You feel a brief tingling sensation.\n\r",victim);
    send_to_char(AT_RED, "The spell failed.\n\r",ch);
    return;
  }

  found = FALSE;

  /* Check dispel of spells that mobs where built with */
  if (IS_NPC( victim ) )
   found = dispel_flag_only_spells( level, victim );

  /* Check dispel of spells cast */
  for( paf = victim->affected; paf; paf = paf->next )
  {
     if (paf->deleted)
      continue;
     if ( skill_table[paf->type].spell_fun != spell_null
     && skill_table[paf->type].dispelable == TRUE )
     if(check_dispel(level,victim,paf->type))
       found = TRUE;
  }

  for( paf = victim->affected2; paf; paf = paf->next )
  {
     if (paf->deleted)
      continue;
     if ( skill_table[paf->type].spell_fun != spell_null
     && skill_table[paf->type].dispelable == TRUE )
     if(check_dispel(level,victim,paf->type))
       found = TRUE;
  }

  if(found)
  {
    send_to_char(AT_RED, "You feel a brief tingling sensation.\n\r",victim);
    send_to_char(AT_YELLOW,
     "Unraveled magical energy ripple away at your succes.\n\r",ch);
  }
  else
    send_to_char(AT_RED, "The spell failed.\n\r",ch);

}
/* modified by XOR */
/* Rom2 modified for enhanced use */
/*
void spell_dispel_magic ( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  bool found = FALSE;

  if(saves_spell(level, victim))
  {
    send_to_char(C_DEFAULT, "You feel a brief tingling sensation.\n\r",victim);
    send_to_char(C_DEFAULT, "You failed.\n\r", ch);
  }
*/
  /* begin running through the spells */
/*
  if(check_dispel(level,victim,skill_lookup("armor")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("bless")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("blindness")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n is no longer blinded.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("change sex")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n looks more like $mself again.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("charm person")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n regains $s free will.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("chill touch")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n looks warmer.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("curse")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("detect evil")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("detect hidden")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("detect invis")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("detect magic")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("faerie fire")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n's outline fades.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("fly")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n falls to the ground!",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("giant strength")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n no longer looks so mighty.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("infravision")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("invis")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n fades into existance.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("mass invis")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n fades into existance.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("pass door")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("protection")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("sanctuary")))
  {
    found = TRUE;
    act(C_DEFAULT, "The white aura around $n's body vanishes.",
     victim,NULL,NULL,TO_ROOM);
  }

  if(IS_AFFECTED(victim,AFF_SANCTUARY)
   && !saves_dispel(level, victim->level,-1)
   && !is_affected(victim,skill_lookup("sanctuary")))
  {
    found = TRUE;
    REMOVE_BIT(victim->affected_by,AFF_SANCTUARY);
    act(C_DEFAULT, "The white aura around $n's body vanishes.",
     victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("shield")))
  {
    found = TRUE;
    act(C_DEFAULT, "The shield protecting $n vanishes.",
     victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("sleep")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("stone skin")))
  {
    act(C_DEFAULT, "$n's skin regains its normal texture.",
     victim,NULL,NULL,TO_ROOM);
    found = TRUE;
  }

  if(check_dispel(level,victim,skill_lookup("weaken")))
  {
    act(C_DEFAULT, "$n looks stronger.",victim,NULL,NULL,TO_ROOM);
    found = TRUE;
  }

  if(check_dispel(level,victim,skill_lookup("fireshield")))
  {
    found = TRUE;
    act(AT_RED, "The flames about $n's body burn out.",
     victim, NULL, NULL, TO_ROOM );
  }

  if(IS_AFFECTED(victim,AFF_FIRESHIELD)
   && !saves_dispel(level, victim->level,-1)
   && !is_affected(victim,skill_lookup("fireshield")))
  {
    found = TRUE;
    REMOVE_BIT(victim->affected_by,AFF_FIRESHIELD);
    act(AT_RED, "The flames about $n's body burn out.",
     victim, NULL, NULL, TO_ROOM );
  }

  if(check_dispel(level,victim,skill_lookup("shockshield")))
  {
    found = TRUE;
    act(AT_BLUE, "The electricity about $n's body flee's into the ground.",
     victim, NULL, NULL, TO_ROOM );
  }

  if(IS_AFFECTED(victim,AFF_SHOCKSHIELD)
   && !saves_dispel(level, victim->level,-1)
   && !is_affected(victim,skill_lookup("shockshield")))
  {
    found = TRUE;
    REMOVE_BIT(victim->affected_by,AFF_SHOCKSHIELD);
    act(AT_BLUE, "The electricity about $n's body flee's into the ground.",
     victim, NULL, NULL, TO_ROOM );
  }

  if(check_dispel(level,victim,skill_lookup("iceshield")))
  {
    found = TRUE;
    act(AT_LBLUE, "The icy crust about $n's body melts to a puddle.",
     victim, NULL, NULL, TO_ROOM);
  }

  if(IS_AFFECTED(victim,AFF_ICESHIELD)
   && !saves_dispel(level, victim->level,-1)
   && !is_affected(victim,skill_lookup("iceshield")))
  {
    found = TRUE;
    REMOVE_BIT(victim->affected_by,AFF_ICESHIELD);
    act(AT_LBLUE, "The icy crust about $n's body melts to a puddle.",
     victim, NULL, NULL, TO_ROOM );
  }

  if(found)
    send_to_char(C_DEFAULT, "Ok.\n\r",ch);
  else
    send_to_char(C_DEFAULT, "Spell failed.\n\r",ch);
*/
/*
	if ( !victim->fighting || !is_same_group( ch, victim->fighting ) )
	  multi_hit( victim, ch, TYPE_UNDEFINED );
*/
  /*  return;
}*/

/* New cancellation by Decklarean
 * The old way was just to stupid. :>
 * This will dispell all magic spells.
 */
void spell_cancellation(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA *paf;
  bool found;
  if((!IS_NPC(ch) && IS_NPC(victim)
   && !(IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim))
   || (IS_NPC(ch) && !IS_NPC(victim))
   || !is_same_group(ch, victim))
  {
    send_to_char(C_DEFAULT, "You failed, try dispel magic.\n\r",ch);
    return;
  }
  found = FALSE;

  /* Check dispel of spells that mobs where built with */
  if (IS_NPC( victim ) )
   found = dispel_flag_only_spells( level, victim );

  /* Check dispel of spells cast */
  for( paf = victim->affected; paf; paf = paf->next )
  {
    if ( skill_table[paf->type].spell_fun != spell_null
          && skill_table[paf->type].spell_fun != spell_poison
        )
    if( check_dispel(level,victim,paf->type) )
     found = TRUE;
  }

  for( paf = victim->affected2; paf; paf = paf->next )
  {
    if ( skill_table[paf->type].spell_fun != spell_null
          && skill_table[paf->type].spell_fun != spell_poison
        )
    if( check_dispel(level,victim,paf->type) )
     found = TRUE;
  }

  if(found)
    send_to_char(AT_YELLOW,
     "Unraveled magical energy ripple away at your succes.\n\r",ch);
  else
    send_to_char(AT_RED, "The spell failed.\n\r",ch);

}

/*
void spell_cancellation(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  bool found = FALSE;

  level += 2;

  if((!IS_NPC(ch) && IS_NPC(victim)
   && !(IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim))
   || (IS_NPC(ch) && !IS_NPC(victim))
   || !is_same_group(ch, victim))
  {
    send_to_char(C_DEFAULT, "You failed, try dispel magic.\n\r",ch);
    return;
  }
*/
  /* unlike dispel magic, victim gets NO save */

  /* begin running through the spells */
/*
  if(check_dispel(level,victim,skill_lookup("armor")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("bless")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("blindness")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n is no longer blinded.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("change sex")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n looks more like $mself again.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("charm person")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n regains $s free will.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("chill touch")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n looks warmer.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("curse")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("detect evil")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("detect hidden")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("detect invis")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("detect magic")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("faerie fire")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n's outline fades.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("fly")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n falls to the ground!",victim,NULL,NULL,TO_ROOM);
    check_nofloor( ch );
  }

  if(check_dispel(level,victim,skill_lookup("giant strength")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n no longer looks so mighty.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("infravision")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("invis")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n fades into existance.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("mass invis")))
  {
    found = TRUE;
    act(C_DEFAULT, "$n fades into existance.",victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("pass door")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("protection")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("sanctuary")))
  {
    found = TRUE;
    act(C_DEFAULT, "The white aura around $n's body vanishes.",
     victim,NULL,NULL,TO_ROOM);
  }

  if(IS_AFFECTED(victim,AFF_SANCTUARY)
   && !saves_dispel(level, victim->level,-1)
   && !is_affected(victim,skill_lookup("sanctuary")))
  {
    found = TRUE;
    REMOVE_BIT(victim->affected_by,AFF_SANCTUARY);
    act(C_DEFAULT, "The white aura around $n's body vanishes.",
     victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("shield")))
  {
    found = TRUE;
    act(C_DEFAULT, "The shield protecting $n vanishes.",
     victim,NULL,NULL,TO_ROOM);
  }

  if(check_dispel(level,victim,skill_lookup("sleep")))
    found = TRUE;

  if(check_dispel(level,victim,skill_lookup("stone skin")))
  {
    act(C_DEFAULT, "$n's skin regains its normal texture.",
     victim,NULL,NULL,TO_ROOM);
    found = TRUE;
  }

  if(check_dispel(level,victim,skill_lookup("weaken")))
  {
    act(C_DEFAULT, "$n looks stronger.",victim,NULL,NULL,TO_ROOM);
    found = TRUE;
  }

  if(check_dispel(level,victim,skill_lookup("fireshield")))
  {
    found = TRUE;
    act(AT_RED, "The flames about $n's body burn out.",
     victim, NULL, NULL, TO_ROOM );
  }

  if(IS_AFFECTED(victim,AFF_FIRESHIELD)
   && !saves_dispel(level, victim->level,-1)
   && !is_affected(victim,skill_lookup("fireshield")))
  {
    found = TRUE;
    REMOVE_BIT(victim->affected_by,AFF_FIRESHIELD);
    act(AT_RED, "The flames about $n's body burn out.",
     victim, NULL, NULL, TO_ROOM );
  }

  if(check_dispel(level,victim,skill_lookup("shockshield")))
  {
    found = TRUE;
    act(AT_BLUE, "The electricity about $n's body flee's into the ground.",
     victim, NULL, NULL, TO_ROOM );
  }

  if(IS_AFFECTED(victim,AFF_SHOCKSHIELD)
   && !saves_dispel(level, victim->level,-1)
   && !is_affected(victim,skill_lookup("shockshield")))
  {
    found = TRUE;
    REMOVE_BIT(victim->affected_by,AFF_SHOCKSHIELD);
    act(AT_BLUE, "The electricity about $n's body flee's into the ground.",
     victim, NULL, NULL, TO_ROOM );
  }

  if(check_dispel(level,victim,skill_lookup("iceshield")))
  {
    found = TRUE;
    act(AT_LBLUE, "The icy crust about $n's body melts to a puddle.",
     victim, NULL, NULL, TO_ROOM);
  }

  if(IS_AFFECTED(victim,AFF_ICESHIELD)
   && !saves_dispel(level, victim->level,-1)
   && !is_affected(victim,skill_lookup("iceshield")))
  {
    found = TRUE;
    REMOVE_BIT(victim->affected_by,AFF_ICESHIELD);
    act(AT_LBLUE, "The icy crust about $n's body melts to a puddle.",
     victim, NULL, NULL, TO_ROOM );
  }

  if(found)
    send_to_char(C_DEFAULT, "Ok.\n\r",ch);
  else
    send_to_char(C_DEFAULT, "Spell failed.\n\r",ch);
  return;
}*/

/*
 * Turn undead and mental block by Altrag
 */

void spell_turn_undead( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int chance;

  if ( !IS_NPC(victim) || !IS_SET(victim->act, ACT_UNDEAD))
  {
    send_to_char(C_DEFAULT, "Spell failed.\n\r", ch );
    return;
  }

  chance = (level * (10 + IS_GOOD(ch) ? 15 : IS_EVIL(ch) ? 0 : 10) );
  chance /= victim->level;
  if (number_percent( ) < chance && !saves_spell( level, victim ))
  {
    act(AT_WHITE,"$n has turned $N!",ch,NULL,victim,TO_ROOM);
    act(AT_WHITE,"You have turned $N!",ch,NULL,victim,TO_CHAR);
    raw_kill(ch,victim);
    return;
  }

  send_to_char(C_DEFAULT,"Spell failed.\n\r",ch);
  return;
}

void spell_mental_block( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if ( is_affected(victim,sn) )
    return;

  af.type = sn;
  af.level = level;
  af.duration = number_range( level / 4, level / 2 );
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = AFF_NOASTRAL;

  affect_to_char( victim, &af );

  send_to_char( AT_BLUE, "Your mind feels free of instrusion.\n\r",victim);
  if ( ch != victim )
    send_to_char(AT_BLUE, "Ok.\n\r",ch);
}
/* END */
void spell_protection_good(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *)vo;
  AFFECT_DATA af;

  if ( IS_AFFECTED2(ch, AFF_PROTECTION_GOOD) )
    return;

  af.type = sn;
  af.level = level;
  af.duration = 24;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = AFF_PROTECTION_GOOD;
  affect_to_char2( victim, &af );

  if ( ch != victim )
    send_to_char( AT_BLUE, "Ok.\n\r",ch);
  send_to_char(AT_BLUE, "You feel protected.\n\r",victim);
  return;
}

void spell_detect_good(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *)vo;
  AFFECT_DATA af;

  if ( IS_AFFECTED( victim, AFF_DETECT_GOOD ) )
    return;

  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = AFF_DETECT_GOOD;
  affect_to_char2(ch, &af);

  if ( ch != victim)
    send_to_char(AT_BLUE, "Ok.\n\r",ch);
  send_to_char(AT_BLUE, "Your eyes tingle.\n\r",victim);
  return;
}

void spell_holy_strength(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA  *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if(victim->position == POS_FIGHTING || is_affected(victim, sn))
    return;
  af.type       = sn;
  af.level	= level;
  af.duration   = 6 + level;
  af.location   = APPLY_HITROLL;
  af.modifier   = level / 4;
  af.bitvector  = 0;
  affect_to_char( victim, &af );

  af.location  = APPLY_DAMROLL;
  af.modifier  = level / 4;
  affect_to_char( victim, &af );

  af.location  = APPLY_STR;
  af.modifier  = level / 50;
  affect_to_char( victim, &af );

  if(ch != victim)
    send_to_char(AT_BLUE, "Ok.\n\r", ch );
  send_to_char(AT_BLUE, "The strength of the gods fills you.\n\r", victim);
  return;
}

void spell_curse_of_nature(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA  *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if ( is_affected(victim, sn) )
  {
    send_to_char(AT_RED, "You have failed.\n\r", ch );
    return;
  }

  af.type       = sn;
  af.level	= level;
  af.duration   = 6 + level;
  af.location   = APPLY_HITROLL;
  af.modifier   = 0 - level * 2 / 3;
  af.bitvector  = 0;
  affect_to_char( victim, &af );

  af.location  = APPLY_DAMROLL;
  af.modifier  = 0 - level * 2 / 3;
  affect_to_char( victim, &af );

  af.location  = APPLY_STR;
  af.modifier  = 0 - level / 30;
  affect_to_char( victim, &af );

  if(ch != victim)
    send_to_char(AT_GREEN, "Ok.\n\r", ch );
  send_to_char(AT_GREEN, "The wrath of nature wrecks you.\n\r", victim);
  return;
}

void spell_enchanted_song(int sn, int level, CHAR_DATA *ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *)vo;
  CHAR_DATA *rch = get_char(ch);

  if ( ch == victim )
  {
    act( AT_BLUE, "$n sings an enchanting song.", ch, NULL, NULL, TO_ROOM );
    send_to_char(AT_BLUE, "You sing a song.\n\r", ch );
  }
  act( AT_BLUE, "Your song pacifies $N.", ch, NULL, victim, TO_CHAR );
  act( AT_BLUE, "$n's song pacifies $N.", ch, NULL, victim, TO_NOTVICT );
  act( AT_BLUE, "$n's song slows your reactions.", ch, NULL, victim, TO_VICT );
  STUN_CHAR( victim, 1, STUN_TOTAL );
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( rch->fighting )
	    stop_fighting( rch, TRUE );
    }

  return;
}

/* RT haste spell */

void spell_haste( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || IS_AFFECTED(victim,AFF_HASTE)
/*    ||   IS_SET(victim->off_flags,OFF_FAST)*/)
    {
	if (victim == ch)
	  send_to_char(C_DEFAULT, "You can't move any faster!\n\r",ch);
	else
	  act(C_DEFAULT, "$N is already moving as fast as $e can.",
	      ch,NULL,victim,TO_CHAR);
	return;
    }
    af.type      = sn;
    af.level     = level;
    if (victim == ch)
      af.duration  = level/2;
    else
      af.duration  = level/4;
    af.location  = APPLY_DEX;
    af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
    af.bitvector = AFF_HASTE;
    affect_to_char( victim, &af );
    send_to_char(C_DEFAULT,
     "You feel yourself moving more quickly.\n\r", victim );
    act(C_DEFAULT, "$n is moving more quickly.",victim,NULL,NULL,TO_ROOM);
    if ( ch != victim )
	send_to_char(C_DEFAULT, "Ok.\n\r", ch );
    return;
}


void spell_healing_hands( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *)vo;
  int divisor = 1;
  int heal = 0;

  if ( ch == victim )
  {
    int mana;

    /* Refund mana lost by casting.  Make it seem like a new target type. */
    mana = SPELL_COST( ch, sn );
    if ( is_class( ch, CLASS_VAMPIRE ) ) {
      ch->bp -= mana;
    } else {
      ch->mana -= mana;
    }
    send_to_char(AT_BLUE, "You cannot cast this spell on yourself.\n\r", ch );
    return;
  }

  if ( victim->hit >= MAX_HIT(victim) )
  {
    act(AT_BLUE, "You heal $N.", ch, NULL, victim, TO_CHAR );
    return;
  }

  if ( IS_NEUTRAL( ch ) )
    divisor = 2;
  if ( IS_EVIL( ch ) )
    divisor = 4;
/*  divisos += 2;*/
  heal = (MAX_HIT(victim) - victim->hit) / divisor;

  victim->hit += heal;

  act(AT_BLUE, "You heal $N.", ch, NULL, victim, TO_CHAR);
  act(AT_BLUE, "$n heals you.", ch, NULL, victim, TO_VICT);
  act(AT_BLUE, "$n heals $N.", ch, NULL, victim, TO_NOTVICT);
  return;
}

void spell_hex( int sn, int level, CHAR_DATA *ch, void *vo )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if ( IS_AFFECTED( victim, AFF_BLIND + AFF_CURSE )
  && IS_AFFECTED2( victim, AFF_CONFUSED ) )
	{
	act( AT_DGREY, "$N is already hexed.", ch, NULL, victim, TO_CHAR );
	return;
	}
  if ( saves_spell( level, victim ) )
	{
	act( AT_DGREY, "$N resists the hex.", ch, NULL, victim, TO_CHAR );
	return;
	}

  af.type	= sn;
  af.level	= level;
  af.duration	= ( 2 * level / 3 + 20 ) / 2;
  af.location	= APPLY_HITROLL;
  af.modifier	= -10;
  af.bitvector	= AFF_BLIND;
  affect_to_char( victim, &af );

  af.location	= APPLY_HITROLL;
  af.modifier	= -1;
  af.bitvector  = AFF_CURSE;
  affect_to_char( victim, &af );

  af.location	= APPLY_SAVING_SPELL;
  af.modifier	= 1;
  af.bitvector	= AFF_CURSE;
  affect_to_char( victim, &af );

  af.location	= APPLY_STR;
  af.modifier	= -1 - ( level >= 20 ) - ( level >= 40 ) - ( level >= 60 ) - ( level >= 80 );
  affect_to_char( victim, &af );

  af.location	= APPLY_NONE;
  af.modifier	= 0;
  af.bitvector	= AFF_CONFUSED;
  affect_to_char2( victim, &af );

  send_to_char( AT_DGREY, "You have hexed your opponent.\n\r", ch );
  send_to_char( AT_DGREY, "A hex has been placed upon your soul.\n\r", victim );
  return;
}

void spell_dark_ritual( int sn, int level, CHAR_DATA *ch, void *vo )
{
  OBJ_DATA	*obj;
  int		mana;
  bool		found = FALSE;

  for ( obj = ch->in_room->contents; obj; obj = obj->next )
    {
    if ( obj->deleted )
	continue;
    if ( obj->item_type == ITEM_CORPSE_NPC )
	{
	found = TRUE;
	break;
	}
    }
  if ( found == TRUE )
    {
    mana = UMAX( 30, number_fuzzy( level / 2 ) );
    ch->mana += mana;
    ch->mana = UMIN( MAX_MANA(ch), ch->mana );
    send_to_char( AT_DGREY, "You extract the last of the energy from the corpse.\n\r",
		  ch );
    act( AT_DGREY, "$n saps away the last of the mystical energies from the $p.",
	 ch, obj, NULL, TO_ROOM );
    extract_obj( obj );
    }
  else
    send_to_char( AT_DGREY, "You must have a corpse to sacrifice to perform a dark ritual.\n\r", ch );
  return;
}
void spell_field_of_decay( int sn, int level, CHAR_DATA *ch, void *vo )
{
  AFFECT_DATA af;

  if ( IS_AFFECTED2( ch, AFF_FIELD ) )
	return;
  af.type	= sn;
  af.level	= level;
  af.duration	= number_fuzzy( level / 5 );
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  af.bitvector	= AFF_FIELD;
  affect_to_char2( ch, &af );

  send_to_char( AT_DGREY, "You summon the power of the dead and a black haze envelops you.\n\r", ch );
  act( AT_DGREY, "A black haze emanates from $n's body and envelops $m.",
       ch, NULL, NULL, TO_ROOM );
  return;
}

void spell_stench_of_decay( int sn, int level, CHAR_DATA *ch, void *vo )
{
		 CHAR_DATA *victim       = (CHAR_DATA *) vo;
    static const int        dam_each [ ] =
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,  30,	 35,  40,  45,  50,  55,
	 60,  65,  70,  75,  80,	 82,  84,  86,  88,  90,
	 92,  94,  96,  98, 100,	102, 104, 106, 108, 110,
	112, 114, 116, 118, 120,	122, 124, 126, 128, 130,
	132, 134, 136, 138, 140,	142, 144, 146, 148, 150,
	152, 154, 156, 158, 160,	162, 164, 166, 168, 170,
	172, 174, 176, 178, 180,	182, 184, 186, 188, 190,
	192, 194, 196, 198, 200,	202, 204, 206, 208, 210,
	215, 220, 225, 230, 235,	240, 245, 250, 255, 260
    };
		 int        dam;

    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1 );
    level    = UMAX( 0, level );
    dam	= number_range( dam_each[level] / 2, dam_each[level] * 7 );
    dam = sc_dam( ch, dam );
    if ( IS_AFFECTED2( victim, AFF_FIELD ) )
       dam /= 6;
    if ( saves_spell( level, victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}

void spell_soul_bind( int sn, int level, CHAR_DATA *ch, void *vo )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA  *soulgem;

    if ( !IS_NPC(victim) || saves_spell( level, victim ) )
    {
      send_to_char(AT_BLUE, "You failed.\n\r", ch);
      return;
    }

   soulgem = create_object( get_obj_index( OBJ_VNUM_SOULGEM ), 0 );
   soulgem->ac_vnum = victim->pIndexData->vnum;
   soulgem->level = ch->level;
   soulgem->timer = ch->level / 4;
#ifdef NEW_MONEY
   soulgem->cost.silver = soulgem->cost.copper = 0;
   soulgem->cost.gold = victim->level * 1000;
#else
   soulgem->cost  = victim->level * 1000;
#endif
   soulgem->ac_charge[0] = soulgem->ac_charge[1] = 1;
   obj_to_char( soulgem , ch );

    act(AT_BLUE, "You tear out $Ns soul, binding it to form a Soulgem.", ch, NULL, victim, TO_CHAR);
    act(AT_BLUE, "$n tears out $Ns soul, binding it to form a Soulgem.", ch, NULL, victim, TO_ROOM);
    act(AT_BLUE, "$N screams in agony as it slowly dissipates into nothingness!", ch, NULL, victim, TO_CHAR);
    act(AT_BLUE, "$N screams in agony as it slowly dissipates into nothingness!", ch, NULL, victim, TO_ROOM);
    act(AT_WHITE, "Your SOUL is STOLEN by $n!", ch, NULL, victim, TO_VICT);

    if ( IS_NPC( victim ) )
       extract_char( victim, TRUE );
    else
       extract_char( victim, FALSE );

   return;
}
/* MONK SPELLS */
void spell_iron_skin( int sn, int level, CHAR_DATA *ch, void *vo )
{
  AFFECT_DATA af;
  if ( is_affected( ch, sn ) )
	return;
  send_to_char( AT_GREY, "Your skin turns to iron.\n\r", ch );
  act( AT_GREY, "$n's skin turns to iron.", ch, NULL, NULL, TO_ROOM );
  af.type	 = sn;
  af.level	 = ch->level;
  af.duration	 = ch->level / 6;
  af.location	 = APPLY_AC;
  af.modifier	 = 0 - number_fuzzy( ch->level ) / 2;
  af.bitvector	 = AFF_INERTIAL;
  affect_to_char2( ch, &af );
  return;
}
void spell_chi_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
  AFFECT_DATA af;
  if ( is_affected( ch, sn ) )
	return;
  send_to_char( AT_BLUE, "You tap into your chi and use it to raise a chi shield.\n\r", ch );
  act( AT_BLUE, "$n is surrounded by a chi shield.", ch, NULL, NULL, TO_ROOM );
  af.type	 = sn;
  af.level	 = ch->level * 2;
  af.duration	 = ch->level / 4;
  af.location	 = APPLY_SAVING_SPELL;
  af.modifier	 = 0 - ch->level/4;
  af.bitvector	 = AFF_PROTECT;
  affect_to_char( ch, &af );
  af.location	 = APPLY_SAVING_BREATH;
  af.modifier	 = 0 - ch->level / 4;
  af.bitvector	 = AFF_PROTECTION_GOOD;
  affect_to_char2( ch, &af );
  return;
}

/* Adds + dam to spells for having spellcraft skill */
int sc_dam( CHAR_DATA *ch, int dam )
{
  float mod;
  if ( ch->level < 50 )
	mod = 82.6;		/* x1.15 */
  else if ( ch->level < 60 )
	mod = 73.07;		/* x1.3  */
  else if ( ch->level < 70 )
	mod = 65.51;		/* x1.45 */
  else if ( ch->level < 80 )
	mod = 55.88; 		/* x1.7  */
  else if ( ch->level < 90 )
	mod = 51.35;		/* x1.85 */
  else if ( ch->level < 95 )
	mod = 47.5;		/* x2    */
  else
	mod = 38; 		/* x2.5  */
  if ( !IS_NPC( ch ) && ch->pcdata->learned[gsn_spellcraft] > 0 )
	dam += dam * ch->pcdata->learned[gsn_spellcraft] / mod;
  return dam;
}

void spell_thunder_strike( int sn, int level, CHAR_DATA *ch, void *vo )
{
    AFFECT_DATA af;
    CHAR_DATA *victim = (CHAR_DATA *) vo;


    static const int dam_each [ ] =
    {
      0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0, 150, 155, 160, 165, 170, 175,
    185, 190, 195, 200, 205, 207, 209, 211, 213, 215,
    222, 224, 226, 228, 230, 232, 234, 236, 238, 240,
    247, 249, 251, 253, 255, 257, 259, 261, 263, 265,
    272, 274, 276, 278, 280, 282, 284, 286, 288, 290,
    297, 299, 301, 303, 305, 307, 309, 311, 313, 315,
    322, 324, 326, 328, 330, 332, 334, 336, 338, 340,
    347, 349, 351, 353, 355, 357, 359, 361, 363, 365,
    375, 380, 385, 390, 395, 400, 405, 410, 415, 420
    };

    int        dam;


    level    = UMIN( level, sizeof( dam_each ) / sizeof( dam_each[0] ) - 1);
    level    = UMAX( 0, level );
    dam = number_range( dam_each[level], dam_each[level] * 8 );
    dam = sc_dam( ch, dam );
    if ( IS_AFFECTED( victim, AFF_SHOCKSHIELD) )
	dam /= 2;

    if ( saves_spell( level, victim ) )
	dam /= 2;

    damage( ch, victim, dam, sn );

    if (    IS_AFFECTED( victim, AFF_BLIND )
         && IS_AFFECTED2( victim, AFF_CONFUSED ) )
    {
      return;
    }

    af.type      = sn;
    af.level       = level;
    af.duration  = level / 50;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector  = AFF_BLIND;
    affect_to_char( victim, &af );

  act( AT_YELLOW,
       "A brilliant flash of lightning strikes $N !",
       ch, NULL, victim, TO_CHAR    );

  send_to_char(AT_YELLOW,
       "A flash of lightning makes everything dark!\n\r",
        victim );

  act(AT_YELLOW,
       "A tremendous bolt of lightning has blinded $N !",
        ch, NULL, victim, TO_NOTVICT );

  af.type      = sn;
  af.level     = level;
  af.duration  = 2;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector  = AFF_CONFUSED;
  affect_to_char2( victim, &af );

  STUN_CHAR( ch, 3, STUN_COMMAND );

  act(AT_WHITE,
       "Booming thunder causes momentary confusion to $N!",
        ch,NULL, victim, TO_CHAR );

  send_to_char(AT_WHITE,
       "Loud thunder wracks your ears and leaves you confused in the midst of battle!\n\r",
        victim );

  act(AT_WHITE,
      "$N looks confused!",
       ch, NULL, victim, TO_NOTVICT );

  return;

}

/*
 * Bard spells -- Thanks Arkhane :>
 */
void spell_inspiration( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    char buf [ MAX_STRING_LENGTH ];

    act( AT_YELLOW, "$n sings a song about great battles and brave heros.",
	 ch, NULL, NULL, TO_ROOM );

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {

	if ( ( victim->deleted ) || !is_same_group( ch, victim ) )
	    continue;

	if ( IS_NPC( victim ) )
	    continue;

    	if( is_affected( victim, sn ) )
	{
	  if( victim == ch )
	    send_to_char( AT_WHITE, "You have already been inspired.\n\r", victim );
	  else
	    act( AT_WHITE, "$N has been inspired already.", ch, NULL, victim,
		 TO_CHAR );
/*	  return;  (still want to loop through rest of chars so just continue) */
	  continue;
	}

	af.type 	= sn;
	af.level 	= level;
	af.duration	= level;
	af.location	= APPLY_HITROLL;
	af.modifier	= level / 6;
	af.bitvector	= 0;
	affect_to_char( victim, &af );

	af.location 	= APPLY_DAMROLL;
	af.modifier	= level / 6;
	affect_to_char( victim, &af );

	af.location	= APPLY_SAVING_SPELL;
	af.modifier 	= - level / 2;
	affect_to_char( victim, &af );

	if ( victim != ch)
	{
	  sprintf( buf, "You become inspired by %s's song.\n\r", ch->name );
	  send_to_char( AT_CYAN, buf, victim );
	  sprintf( buf, "%s becomes inspired by your song.\n\r", victim->name );
	  send_to_char( AT_CYAN, buf, ch );
	}
	else
	  send_to_char( AT_CYAN, "You inspire yourself to greater heights.\n\r",
			ch );
    }

    return;

}

void spell_war_cry( int sn, int level, CHAR_DATA *ch, void *vo )
{

    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    act( AT_BLOOD,
	 "$n's cry for the death of $s enemies pierces through the room!",
	 ch, NULL, NULL, TO_ROOM );

    if ( saves_spell( level, victim ) )
    {
	act( AT_WHITE, "$N ignores your cry for blood.",
	     ch, NULL, victim, TO_CHAR );
	return;
    }
    af.type	= sn;
    af.level	= level;
    af.duration = 3;
    af.location = APPLY_HITROLL;
    af.modifier = - level / 20;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location = APPLY_DAMROLL;
    af.modifier = - level / 20;
    affect_to_char( victim, &af );

    if ( ch != victim )
	send_to_char( AT_BLOOD,
		      "Your cry for war sends fear into your opponent!\n\r",
		      ch );
    send_to_char( AT_BLOOD, "A bloodthirsty cry sends shivers down your spine.\n\r",
		      victim );

    return;
}

void spell_group_healing( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    act( AT_WHITE, "$n sings a soothing song.", ch, NULL, NULL, TO_ROOM );

    for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( IS_NPC( victim ) || ( victim->deleted ) ||
	   ( !is_same_group( ch, victim ) ) )
	   continue;

	victim->hit = UMIN( victim->hit + 150, MAX_HIT(victim) + 50 );
	update_pos( victim );

	if( ch != victim )
	   send_to_char( AT_BLUE, "Ok.\n\r", ch );
    }

    send_to_char( AT_BLUE, "You have completed your song.\n\r", ch );
    return;

}

void spell_chant( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    send_to_char(AT_BLUE, "You begin your loud chant of destruction!\n\r", ch );
    act(AT_BLUE, "$n's chant wreaks havoc everywhere!", ch, NULL, NULL, TO_ROOM );

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
	if ( victim->deleted )
             continue;

        if ( victim != ch && ( IS_NPC( ch ) ? !IS_NPC( victim )
                     :  IS_NPC( victim ) ) )
	     damage( ch, victim, 1.5 * level + dice( level, 6 ), sn );
    }

    return;

}

/*Decklarean*/
void spell_blur ( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA  *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;

    af.type	 = sn;
    af.level	 = level;
    af.duration	 = 3 + level;
    af.location	 = APPLY_AC;
    af.modifier	 = 0 - level/2;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = 0 - level / 3;
    affect_to_char( victim, &af );

    send_to_char(AT_GREY, "Your form blurs.\n\r",
		 victim );
    act(AT_GREY, "$N form becomes blurred, shifting and wavering before you.",
	ch, NULL, victim, TO_NOTVICT );
    return;
}

/*Decklarean*/
void spell_firewall( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;

    if (    IS_SET( ch->in_room->room_flags, ROOM_SAFE     )
	 || IS_SET( ch->in_room->room_flags, ROOM_NO_OFFENSIVE ) )
	 send_to_char( AT_BLUE, "The spell fails to ignite.", ch );

    send_to_char(AT_RED, "A wall of fire explodes out in all direction from you!\n\r", ch );
    act(AT_RED, "A wall of fire explodes out from $n!", ch, NULL, NULL, TO_ROOM );

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
	if ( vch->deleted )
	    continue;
	if ( vch != ch )
	    damage( ch, vch, level + dice( level, 20 ), sn );
    }

    return;
}
