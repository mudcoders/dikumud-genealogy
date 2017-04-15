/*****************************************************************************
 * Clan routines for clan funtions.. I hope to move the ones created prior   *
 * to this file in here eventually as well.                                  *
 * -- Altrag Dalosein, Lord of the Dragons                                   *
 *****************************************************************************/

#define unix 1
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"


/*
 * Illuminati bestow command, for deity only.
 */
void do_bestow( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  if ( ch->clan != 1 || ch->clev < 5 )
  {
    send_to_char(C_DEFAULT, "Huh?\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg );

  if ( !( victim = get_char_room( ch, arg ) ) || IS_NPC(victim) )
  {
    send_to_char(C_DEFAULT, "They aren't here.\n\r", ch );
    return;
  }

  if ( victim->clan != ch->clan )
  {
    send_to_char(C_DEFAULT, "They aren't in your clan!\n\r", ch );
    return;
  }

  if ( !IS_SET( victim->act, PLR_CSKILL ) )
  {
    SET_BIT( victim->act, PLR_CSKILL );
    act(AT_PINK, "$N bestowed with the Transmute skill.", ch, NULL, victim,
	TO_CHAR );
    send_to_char( AT_PINK, "You have been given the Transmute skill.\n\r",
		  victim );
  }
  else
  {
    REMOVE_BIT( victim->act, PLR_CSKILL );
    act(AT_PINK, "Transmute denied from $N.", ch, NULL, victim, TO_CHAR );
    send_to_char( AT_PINK, "You have been denied the Transmute skill.\n\r",
		  victim );
  }
}


/*
 * Illuminati transmute skill, must be given to a member by the deity
 */
void do_transmute( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj;
  int chance;

  if ( ch->clan != 1 || !IS_SET(ch->act, PLR_CSKILL)
      || IS_NPC(ch) )
  {
    send_to_char(C_DEFAULT, "Huh?\n\r", ch );
    return;
  }

  if ( !( obj = get_obj_carry( ch, argument ) ) )
  {
    send_to_char(C_DEFAULT, "You do not have that item.\n\r", ch );
    return;
  }

  if ( obj->level > 0 )
    chance = (ch->level * 75) / obj->level;
  else
    chance = 85;

  if ( number_percent( ) < chance )
  {
    obj->extra_flags |= ITEM_NO_DAMAGE;
    act(AT_PINK, "$p transmuted.", ch, obj, NULL, TO_CHAR );
    return;
  }

  act(AT_PINK, "You failed!  $p exploded in your face!", ch, obj, NULL,
      TO_CHAR);
  extract_obj(obj);
  return;
}


/*
 * ??? Doomshield skill
 */
void do_doomshield( CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA af;

  if ( ch->clan != -1 )
  {
    send_to_char( C_DEFAULT, "Huh?\n\r", ch );
    return;
  }

  if ( IS_AFFECTED2( ch, AFF_DOOMSHIELD ) )
    return;

  af.type = gsn_doomshield;
  af.level = ch->level;
  af.duration = 5;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = AFF_DOOMSHIELD;
  affect_to_char2(ch, &af);
  act( AT_PURPLE, "$n is surrounded with the protection of Decklarean", ch, NULL,
       NULL, TO_ROOM );
  send_to_char(AT_PURPLE, "You are surrounded with the protection of Decklarean\n\r",
	       ch );
  return;
}

void do_unholystrength( CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA af;

  if ( ch->clan != 8 )
  {
    send_to_char(C_DEFAULT, "Huh?\n\r",ch);
    return;
  }

  if ( ch->fighting || ch->ctimer || IS_AFFECTED2(ch, AFF_UNHOLYSTRENGTH ) )
  {
    send_to_char(C_DEFAULT, "You failed.\n\r",ch);
    return;
  }

  af.type = gsn_unholystrength;
  af.level = ch->level;
  af.duration = -1;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = AFF_UNHOLYSTRENGTH;
  affect_to_char2(ch, &af);
/*  ch->ctimer = 17;*/

  send_to_char(AT_DGREEN, "You feel the strength of the Unholy Plague run through you.\n\r",ch);
  return;
}

void do_image( CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA af;

  if ( ch->clan != 3 )
  {
    send_to_char( C_DEFAULT, "Huh?\n\r",ch);
    return;
  }

  if ( ch->ctimer || ch->combat_timer )
  {
    send_to_char(AT_BLUE, "You can't right now.\n\r",ch);
    return;
  }

  if ( IS_AFFECTED2(ch, AFF_IMAGE) )
    return;

  af.location = APPLY_NONE;
  af.modifier = number_range( 1, 5 );
  af.duration = number_fuzzy(3);
  af.bitvector = AFF_IMAGE;
  af.type = gsn_image;
  af.level = ch->level;
  ch->ctimer = af.duration + 10;
  affect_to_char2( ch, &af );
  send_to_char(AT_CYAN, "You are surrounded by images of the Talisman.\n\r",
	       ch);
  act(AT_CYAN, "$n suddenly splits into many images.",ch,NULL,NULL,TO_ROOM);
  return;
}
/* END */
