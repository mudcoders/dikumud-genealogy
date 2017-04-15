/***************************************************************************
 *  file: magic.c , Implementation of spells.              Part of DIKUMUD *
 *  Usage : The actual effect of magic.                                    *
 *  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
 *                                                                         *
 *  Copyright (C) 1992, 1993 Michael Chastain, Michael Quan, Mitchell Tse  *
 *  Performance optimization and bug fixes by MERC Industries.             *
 *  You can use our stuff in any way you like whatsoever so long as this   *
 *  copyright notice remains intact.  If you like it please drop a line    *
 *  to mec@garnet.berkeley.edu.                                            *
 *                                                                         *
 *  This is free software and you are benefitting.  We hope that you       *
 *  share your changes too.  What goes around, comes around.               *
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "utils.h"
#include "spells.h"
#include "handler.h"
#include "limits.h"
#include "interp.h"
#include "db.h"

/* Extern structures */
extern struct room_data *world;
extern struct obj_data  *object_list;
extern struct char_data *character_list;

/* Extern procedures */

bool saves_spell(struct char_data *ch, sh_int spell);

int dice(int number, int size);
void check_killer(struct char_data *ch, struct char_data *victim);
void update_pos( struct char_data *victim );

/* Offensive Spells */

void spell_magic_missile(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
    int dam_each[] = 
	{0,  3,3,4,4,5, 6,6,6,6,6, 7,7,7,7,7, 8,8,8,8,8,
	     9,9,9,9,9, 10,10,10,10,10, 11,11,11,11,11,
	     12,12,12,12,12, 13,13,13,13,13, 14,14,14,14,14};

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  dam = number(dam_each[(int)level]>>1, dam_each[(int)level]<<1);

  if ( saves_spell(victim, SAVING_SPELL) )
    dam >>= 1;

  damage(ch, victim, dam, SPELL_MAGIC_MISSILE);
}

void spell_chill_touch(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  int dam;
    int dam_each[] = 
	{0,  0,0,7,8,9, 9,12,13,13,13, 14,14,14,15,15, 15,16,16,16,17,
	     17,17,18,18,18, 19,19,19,20,20, 20,21,21,21,22,
	     22,22,23,23,23, 24,24,24,25,25, 25,26,26,26,27};

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  dam = number(dam_each[(int)level]>>1, dam_each[(int)level]<<1);

  if ( !saves_spell(victim, SAVING_SPELL) )
  {
    af.type      = SPELL_CHILL_TOUCH;
    af.duration  = 6;
    af.modifier  = -1;
    af.location  = APPLY_STR;
    af.bitvector = 0;
    affect_join(victim, &af, TRUE, FALSE);
  } else {
    dam >>= 1;
  }
  damage(ch, victim, dam, SPELL_CHILL_TOUCH);
}

void spell_burning_hands(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
  int dam_each[] = 
    {0,  0,0,0,0,14, 17,20,23,26,29, 29,29,30,30,31, 31,32,32,33,33,
       34,34,35,35,36, 36,37,37,38,38, 39,39,40,40,41,
       41,42,42,43,43, 44,44,45,45,46, 46,47,47,48,48};

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  dam = number(dam_each[(int) level]>>1, dam_each[(int) level]<<1);

  if ( saves_spell(victim, SAVING_SPELL) )
    dam >>= 1;

  damage(ch, victim, dam, SPELL_BURNING_HANDS);
}

void spell_shocking_grasp(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
    int dam_each[] = 
	{0,  0,0,0,0,0, 0,20,25,29,33, 36,39,39,39,40, 40,41,41,42,42,
	     43,43,44,44,45, 45,46,46,47,47, 48,48,49,49,50,
	     50,51,51,52,52, 53,53,54,54,55, 55,56,56,57,57};

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  dam = number(dam_each[(int) level]>>1, dam_each[(int) level]<<1);

  if ( saves_spell(victim, SAVING_SPELL) )
    dam >>= 1;

  damage(ch, victim, dam, SPELL_SHOCKING_GRASP);
}

void spell_lightning_bolt(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
    int dam_each[] = 
	{0,  0,0,0,0,0, 0,0,0,25,28, 31,34,37,40,40, 41,42,42,43,44,
	     44,45,46,46,47, 48,48,49,50,50, 51,52,52,53,54,
	     54,55,56,56,57, 58,58,59,60,60, 61,62,62,63,64};

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  dam = number(dam_each[(int) level]>>1, dam_each[(int) level]<<1);

  if ( saves_spell(victim, SAVING_SPELL) )
    dam >>= 1;

  damage(ch, victim, dam, SPELL_LIGHTNING_BOLT);
}

void spell_colour_spray(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
    int dam_each[] = 
	{0,  0,0,0,0,0, 0,0,0,0,0, 30,35,40,45,50, 55,55,55,56,57,
	     58,58,59,60,61, 61,62,63,64,64, 65,66,67,67,68,
	     69,70,70,71,72, 73,73,74,75,76, 76,77,78,79,79};

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  dam = number(dam_each[(int) level]-30, dam_each[(int) level]+40);

  if ( saves_spell(victim, SAVING_SPELL) )
    dam >>= 1;

  damage(ch, victim, dam, SPELL_COLOUR_SPRAY);
}

/* Drain XP, MANA, HP - caster gains HP and MANA */
void spell_energy_drain(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam, xp, mana;

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  if ( !saves_spell(victim, SAVING_SPELL) ) {
	GET_ALIGNMENT(ch) = MIN(-1000, GET_ALIGNMENT(ch)-200);
    if (GET_LEVEL(victim) <= 2) {
      damage(ch, victim, 100, SPELL_ENERGY_DRAIN); /* Kill the sucker */
    } else {
	    xp = number(level>>1,level)*1000;
	    gain_exp(victim, -xp);

	    dam = dice(1,10);

	    mana = GET_MANA(victim)>>1;
	    GET_MOVE(victim) >>= 1;
	    GET_MANA(victim) = mana;

	    GET_MANA(ch) += mana>>1;
	    GET_HIT(ch) += dam;

      send_to_char("Your life energy is drained!\n\r", victim);

      damage(ch, victim, dam, SPELL_ENERGY_DRAIN);
      }
  } else {
    damage(ch, victim, 0, SPELL_ENERGY_DRAIN); /* Miss */
  }
}

void spell_fireball(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
    int dam_each[] = 
	{0,  0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,50, 60,70,80,90,100,
	     100,100,101,102,103, 104,105,106,107,108,
	     109,110,111,112,113, 114,115,116,117,118,
	     119,120,121,122,123, 124,125,126,127,128};

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  dam = number(dam_each[(int) level]-40, dam_each[(int) level]+60);

  if ( saves_spell(victim, SAVING_SPELL) )
    dam >>= 1;

  damage(ch, victim, dam, SPELL_FIREBALL);
}

void spell_armor(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim);
  assert((level >= 0) && (level <= 50));

  if (!affected_by_spell(victim, SPELL_ARMOR)) {
    af.type      = SPELL_ARMOR;
    af.duration  = 24;
    af.modifier  = -20;
    af.location  = APPLY_AC;
    af.bitvector = 0;

    affect_to_char(victim, &af);
    send_to_char("You feel someone protecting you.\n\r", victim);
  }
}

void spell_earthquake(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
  struct char_data *tmp_victim, *temp;

  assert( ch && level >= 1 && level <= 50 );

  dam =  dice(2,8)+level;

  send_to_char("The earth trembles beneath your feet!\n\r", ch);
  act("$n makes the earth trembles and shivers.\n\rYou fall, and hit yourself!",
      FALSE, ch, 0, 0, TO_ROOM);


  for(tmp_victim = character_list; tmp_victim; tmp_victim = temp)
  {
    temp = tmp_victim->next;
    if ( (ch->in_room == tmp_victim->in_room) && (ch != tmp_victim) &&
      (IS_NPC(tmp_victim))){
	    damage(ch, tmp_victim, dam, SPELL_EARTHQUAKE);
    } else
      if (world[ch->in_room].zone == world[tmp_victim->in_room].zone)
	send_to_char("The earth trembles and shivers.\n\r", tmp_victim);
  }

}

void spell_dispel_evil(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
  
  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  if (IS_EVIL(ch))
    victim = ch;
  
  if (IS_GOOD(victim)) {
    act("The Gods protect $N.", FALSE, ch, 0, victim, TO_ROOM);
    spell_armor(level, ch, victim, 0);
    return;
  }

  if (IS_NEUTRAL(victim))
    {
      act("$N does not seem to be affected.", FALSE, ch, 0, victim, TO_CHAR);
      return;
    }

  if ((GET_LEVEL(victim) < level) || (victim == ch))
    dam = 100;
  else {
    dam = dice(level,4);

      if ( saves_spell(victim, SAVING_SPELL) )
      dam >>= 1;
    }

  damage(ch, victim, dam, SPELL_DISPEL_EVIL);
}

void spell_call_lightning(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;
  extern struct weather_data weather_info;

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  dam = dice(MIN(level,15), 8);

  if (OUTSIDE(ch) && (weather_info.sky>=SKY_RAINING)) {

    if ( saves_spell(victim, SAVING_SPELL) )
      dam >>= 1;
  
    damage(ch, victim, dam, SPELL_CALL_LIGHTNING);
  }
}

void spell_harm(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int dam;

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  dam = GET_HIT(victim) - dice(1,4);
  
  if (dam < 0)
    dam = 20; /* Kill the suffering bastard */
  else {
    if ( saves_spell(victim, SAVING_SPELL) )
      dam = MAX(50, dam/4);
  }

  damage(ch, victim, dam, SPELL_HARM);
}

void spell_teleport(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
    int to_room;
    extern int top_of_world;      /* ref to the top element of world */

    assert(victim);

    do {
	to_room = number(0, top_of_world);
    } while (IS_SET(world[to_room].room_flags, PRIVATE));

    act("$n slowly fades out of existence.", FALSE, victim,0,0,TO_ROOM);
    char_from_room(victim);
    char_to_room(victim, to_room);
    act("$n slowly fades into existence.", FALSE, victim,0,0,TO_ROOM);

    do_look(victim, "", 0);
}

void spell_bless(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(ch && (victim || obj));
    assert((level >= 0) && (level <= 50));

  if (obj) {
    if ( (5*GET_LEVEL(ch) > GET_OBJ_WEIGHT(obj)) &&
    (GET_POS(ch) != POSITION_FIGHTING) &&
    !IS_OBJ_STAT(obj, ITEM_EVIL)) {
      SET_BIT(obj->obj_flags.extra_flags, ITEM_BLESS);
      act("$p briefly glows.",FALSE,ch,obj,0,TO_CHAR);
    }
  } else {

    if ((GET_POS(victim) != POSITION_FIGHTING) &&
    (!affected_by_spell(victim, SPELL_BLESS))) {
      
      send_to_char("You feel righteous.\n\r", victim);
      af.type      = SPELL_BLESS;
      af.duration  = 6+level;
      af.modifier  = 1;
      af.location  = APPLY_HITROLL;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      
      af.location = APPLY_SAVING_SPELL;
      af.modifier = -1;                 /* Make better */
      affect_to_char(victim, &af);
    }
  }
}

void spell_blindness(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  if (saves_spell(victim, SAVING_SPELL))
    {
      act("$N seems to be unaffected!", FALSE, ch, NULL, victim, TO_CHAR);
      one_hit (victim, ch, TYPE_UNDEFINED);
      return;
    }
  
  if (affected_by_spell(victim, SPELL_BLINDNESS))
    return;

  act("$n seems to be blinded!", TRUE, victim, 0, 0, TO_ROOM);
  send_to_char("You have been blinded!\n\r", victim);

  af.type      = SPELL_BLINDNESS;
  af.location  = APPLY_HITROLL;
  af.modifier  = -4;  /* Make hitroll worse */
  af.duration  = 1+level;
  af.bitvector = AFF_BLIND;
  affect_to_char(victim, &af);


  af.location = APPLY_AC;
  af.modifier = +40; /* Make AC Worse! */
  affect_to_char(victim, &af);
  one_hit (victim, ch, TYPE_UNDEFINED);
}

void spell_create_food(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct obj_data *tmp_obj;

  assert(ch);
  assert((level >= 0) && (level <= 50));

  CREATE(tmp_obj, struct obj_data, 1);
  clear_object(tmp_obj);

  tmp_obj->name = str_dup("mushroom");
  tmp_obj->short_description = str_dup("A Magic Mushroom");
  tmp_obj->description = str_dup("A really delicious looking magic mushroom lies here.");

  tmp_obj->obj_flags.type_flag = ITEM_FOOD;
  tmp_obj->obj_flags.wear_flags = ITEM_TAKE | ITEM_HOLD;
  tmp_obj->obj_flags.value[0] = 5+level;
  tmp_obj->obj_flags.weight = 1;
  tmp_obj->obj_flags.cost = 10;
  tmp_obj->obj_flags.cost_per_day = 1;

  tmp_obj->next = object_list;
  object_list = tmp_obj;

  obj_to_room(tmp_obj,ch->in_room);

  tmp_obj->item_number = -1;

  act("$p suddenly appears.",TRUE,ch,tmp_obj,0,TO_ROOM);
  act("$p suddenly appears.",TRUE,ch,tmp_obj,0,TO_CHAR);
  
}

void spell_create_water(byte level, struct char_data *ch,
	    struct char_data *victim, struct obj_data *obj)
{
  int water;

  extern struct weather_data weather_info;
  void name_to_drinkcon(struct obj_data *obj,int type);
  void name_from_drinkcon(struct obj_data *obj);

  assert(ch && obj);

  if (GET_ITEM_TYPE(obj) == ITEM_DRINKCON) {
    if ((obj->obj_flags.value[2] != LIQ_WATER)
    && (obj->obj_flags.value[1] != 0)) {
      
      name_from_drinkcon(obj);
      obj->obj_flags.value[2] = LIQ_SLIME;
      name_to_drinkcon(obj, LIQ_SLIME);
      
    } else {

      water = 2*level * ((weather_info.sky >= SKY_RAINING) ? 2 : 1);
      
      /* Calculate water it can contain, or water created */
      water = MIN(obj->obj_flags.value[0]-obj->obj_flags.value[1], water);
      
      if (water > 0) {
    obj->obj_flags.value[2] = LIQ_WATER;
    obj->obj_flags.value[1] += water;



    name_from_drinkcon(obj);
    name_to_drinkcon(obj, LIQ_WATER);
    act("$p is filled.", FALSE, ch,obj,0,TO_CHAR);
      }
    }
  }
}

void spell_cure_blind(byte level, struct char_data *ch,
	      struct char_data *victim, struct obj_data *obj)
{
  
  assert(victim);
  assert((level >= 0) && (level <= 50));

  if (affected_by_spell(victim, SPELL_BLINDNESS)) {
    affect_from_char(victim, SPELL_BLINDNESS);

    send_to_char("Your vision returns!\n\r", victim);
  }
}

void spell_cure_critic(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int healpoints;

  assert(victim);
  assert((level >= 0) && (level <= 50));

  healpoints = dice(3,8)-6+level;

  if ( (healpoints + GET_HIT(victim)) > hit_limit(victim) )
    GET_HIT(victim) = hit_limit(victim);
  else
    GET_HIT(victim) += healpoints;

  send_to_char("You feel better!\n\r", victim);

  update_pos(victim);
}

void spell_cure_light(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int healpoints;

  assert(ch && victim);
  assert((level >= 0) && (level <= 50));

  healpoints = dice(1,8)+(level/3);

  if ( (healpoints+GET_HIT(victim)) > hit_limit(victim) )
    GET_HIT(victim) = hit_limit(victim);
  else
    GET_HIT(victim) += healpoints;

  update_pos( victim );

  send_to_char("You feel better!\n\r", victim);
}

void spell_curse(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( victim, ch );

  if (obj) {
    SET_BIT(obj->obj_flags.extra_flags, ITEM_EVIL);
    SET_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);

    /* LOWER ATTACK DICE BY -1 */
     if(obj->obj_flags.type_flag == ITEM_WEAPON)  {
       if (obj->obj_flags.value[2] > 1) {
     obj->obj_flags.value[2]--;
     act("$p glows red.", FALSE, ch, obj, 0, TO_CHAR);
       }
       else {
     send_to_char("Nothing happens.\n\r", ch);
       }
     }
  } else {
    if (saves_spell(victim, SAVING_SPELL))
      {
	act("$N seems to be unaffected!", FALSE, ch, NULL, victim, TO_CHAR);
	one_hit( victim, ch, TYPE_UNDEFINED);
	return;
      }
    
    if (affected_by_spell(victim, SPELL_CURSE))
      return;

    af.type      = SPELL_CURSE;
    af.duration  = 5*level;
    af.modifier  = -1;
    af.location  = APPLY_HITROLL;
    af.bitvector = AFF_CURSE;
    affect_to_char(victim, &af);

    af.location = APPLY_SAVING_PARA;
    af.modifier = 1; /* Make worse */
    affect_to_char(victim, &af);

    act("$n briefly reveals a red aura!", FALSE, victim, 0, 0, TO_ROOM);
    act("You feel very uncomfortable.",FALSE,victim,0,0,TO_CHAR);
    one_hit( victim, ch, TYPE_UNDEFINED);
  }
}

void spell_detect_evil(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim);
  assert((level >= 0) && (level <= 50));

  if ( affected_by_spell(victim, SPELL_DETECT_EVIL) )
    return;

  af.type      = SPELL_DETECT_EVIL;
  af.duration  = level*5;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = AFF_DETECT_EVIL;

  affect_to_char(victim, &af);

  send_to_char("Your eyes tingle.\n\r", victim);
}

void spell_detect_invisibility(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim);
  assert((level >= 0) && (level <= 50));

  if ( IS_AFFECTED(victim,AFF_DETECT_INVISIBLE) )
    return;

  if ( affected_by_spell(victim, SPELL_DETECT_INVISIBLE) )
    return;

  af.type      = SPELL_DETECT_INVISIBLE;
  af.duration  = level*5;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = AFF_DETECT_INVISIBLE;

  affect_to_char(victim, &af);

  send_to_char("Your eyes tingle.\n\r", victim);
}

void spell_detect_magic(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim);
  assert((level >= 0) && (level <= 50));

  if ( affected_by_spell(victim, SPELL_DETECT_MAGIC) )
    return;

  af.type      = SPELL_DETECT_MAGIC;
  af.duration  = level*5;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = AFF_DETECT_MAGIC;

  affect_to_char(victim, &af);
  send_to_char("Your eyes tingle.\n\r", victim);
}

void spell_detect_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
    assert(ch && (victim || obj));

  if (victim) {
    if (victim == ch)
      if (IS_AFFECTED(victim, AFF_POISON))
	send_to_char("You can sense poison in your blood.\n\r", ch);
      else
	send_to_char("You feel healthy.\n\r", ch);
    else
      if (IS_AFFECTED(victim, AFF_POISON)) {
	act("You sense that $E is poisoned.",FALSE,ch,0,victim,TO_CHAR);
      } else {
	act("You sense that $E is healthy.",FALSE,ch,0,victim,TO_CHAR);
      }
  } else { /* It's an object */
    if ((obj->obj_flags.type_flag == ITEM_DRINKCON) ||
	(obj->obj_flags.type_flag == ITEM_FOOD)) {
      if (obj->obj_flags.value[3])
	act("Poisonous fumes are revealed.",FALSE, ch, 0, 0, TO_CHAR);
      else
	send_to_char("It looks very delicious.\n\r", ch);
    }
  }
}

void spell_enchant_weapon(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int i;

  assert(ch && obj);
#if MAX_OBJ_AFFECT < 2
    error!!!
#endif


  if ((GET_ITEM_TYPE(obj) == ITEM_WEAPON) &&
      !IS_SET(obj->obj_flags.extra_flags, ITEM_MAGIC)) {

    for (i=0; i < MAX_OBJ_AFFECT; i++)
      if (obj->affected[i].location != APPLY_NONE)
    return;

    SET_BIT(obj->obj_flags.extra_flags, ITEM_MAGIC);
      
    obj->affected[0].location = APPLY_HITROLL;
    obj->affected[0].modifier = 1 +(level >= 18)+ (level >=20)+ (level>=25)+
      (level >=30)+ (level >=35);

    obj->affected[1].location = APPLY_DAMROLL;
    obj->affected[1].modifier = 1+(level >= 20)+ (level >=25)+ (level >=30)+
      (level >=35);

    if (IS_GOOD(ch)) {
      SET_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_EVIL);
      act("$p glows blue.",FALSE,ch,obj,0,TO_CHAR);
    } else if (IS_EVIL(ch)) {
      SET_BIT(obj->obj_flags.extra_flags, ITEM_ANTI_GOOD);
      act("$p glows red.",FALSE,ch,obj,0,TO_CHAR);
    } else {
      act("$p glows yellow.",FALSE,ch,obj,0,TO_CHAR);
	}
    }
}

void spell_heal(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  assert(victim);

  spell_cure_blind(level, ch, victim, obj);

  GET_HIT(victim) += 100;

  if (GET_HIT(victim) >= hit_limit(victim))
    GET_HIT(victim) = hit_limit(victim)-dice(1,4);

  update_pos( victim );

  send_to_char("A warm feeling fills your body.\n\r", victim);
}

void spell_invisibility(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert((ch && obj) || victim);

  if (obj) {
    if (CAN_WEAR(obj,ITEM_TAKE)) {
      if ( !IS_SET(obj->obj_flags.extra_flags, ITEM_INVISIBLE)){
    act("$p turns invisible.",FALSE,ch,obj,0,TO_CHAR);
    act("$p turns invisible.",TRUE,ch,obj,0,TO_ROOM);
    SET_BIT(obj->obj_flags.extra_flags, ITEM_INVISIBLE);
      }
    } else {
      act("You fail in making $p invisible.",FALSE,ch,obj,0,TO_ROOM);
    }
  }
  else {              /* Then it is a PC | NPC */
    if (!affected_by_spell(victim, SPELL_INVISIBLE)) {

      act("$n slowly fade out of existence.", TRUE, victim,0,0,TO_ROOM);
      send_to_char("You vanish.\n\r", victim);

      af.type      = SPELL_INVISIBLE;
      af.duration  = 24;
      af.modifier  = -40;
      af.location  = APPLY_AC;
      af.bitvector = AFF_INVISIBLE;
      affect_to_char(victim, &af);
    }
  }
}

void spell_locate_object(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct obj_data *i;
  char name[256];
  char buf[MAX_STRING_LENGTH];
  int j;

  assert(ch);

  strcpy(name, fname(obj->name));

  j=level;

  for (i = object_list; i && (j>0); i = i->next)
    if (isname(name, i->name)) {
      if(i->carried_by) {
	sprintf(buf,"%s carried by %s.\n\r",
	  i->short_description,PERS(i->carried_by,ch));
	send_to_char(buf,ch);
      } else if (i->in_obj) {
	sprintf(buf,"%s in %s.\n\r",i->short_description,
	  i->in_obj->short_description);
	send_to_char(buf,ch);
      } else {
	sprintf(buf,"%s in %s.\n\r",i->short_description,
	(i->in_room == NOWHERE ? "Used but uncertain." : world[i->in_room].name));
	send_to_char(buf,ch);
    j--;
      }
    }

  if(j==0)
    send_to_char("You are very confused.\n\r",ch);
  if(j==level)
    send_to_char("No such object.\n\r",ch);
}

void spell_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert( ch && ( victim || obj ) && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  if (victim) {
    check_killer(ch,victim);
    if(!saves_spell(victim, SAVING_PARA))
      {
	af.type = SPELL_POISON;
	af.duration = level*2;
	af.modifier = -2;
	af.location = APPLY_STR;
	af.bitvector = AFF_POISON;

	affect_join(victim, &af, FALSE, FALSE);

	send_to_char("You feel very sick.\n\r", victim);
      }
    else
      act("$N seems to be unaffected!", FALSE, ch, NULL, victim, TO_CHAR);
    
    if (!( ch == victim ))
      one_hit(victim,ch,TYPE_UNDEFINED);
  } else { /* Object poison */
    if ((obj->obj_flags.type_flag == ITEM_DRINKCON) ||
	(obj->obj_flags.type_flag == ITEM_FOOD)) {
      obj->obj_flags.value[3] = 1;
    }
  }
}

void spell_protection_from_evil(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

    assert(victim);

  if ( IS_AFFECTED(victim,AFF_PROTECT_EVIL) )
    return;

  if (!affected_by_spell(victim, SPELL_PROTECT_FROM_EVIL) ) {
    af.type      = SPELL_PROTECT_FROM_EVIL;
    af.duration  = 24;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_PROTECT_EVIL;
    affect_to_char(victim, &af);
    send_to_char("You have a righteous, protected feeling!\n\r", victim);
    }
}

void spell_remove_curse(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{

  assert(ch && (victim || obj));

  if (obj) {

    if ( IS_SET(obj->obj_flags.extra_flags, ITEM_EVIL) ||
    IS_SET(obj->obj_flags.extra_flags, ITEM_NODROP)) {
      act("$p briefly glows blue.", TRUE, ch, obj, 0, TO_CHAR);

      REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_EVIL);
      REMOVE_BIT(obj->obj_flags.extra_flags, ITEM_NODROP);
    }
  } else {      /* Then it is a PC | NPC */
    if (affected_by_spell(victim, SPELL_CURSE) ) {
      act("$n briefly glows red, then blue.",FALSE,victim,0,0,TO_ROOM);
      act("You feel better.",FALSE,victim,0,0,TO_CHAR);
      affect_from_char(victim, SPELL_CURSE);
    }
  }
}

void spell_remove_poison(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{

  assert(ch && (victim || obj));

  if (victim) {
    if(affected_by_spell(victim,SPELL_POISON)) {
      affect_from_char(victim,SPELL_POISON);
      act("A warm feeling runs through your body.",FALSE,victim,0,0,TO_CHAR);
      act("$N looks better.",FALSE,ch,0,victim,TO_ROOM);
    }
  } else {
    if ((obj->obj_flags.type_flag == ITEM_DRINKCON) ||
	(obj->obj_flags.type_flag == ITEM_FOOD)) {
      obj->obj_flags.value[3] = 0;
      act("The $p steams briefly.",FALSE,ch,obj,0,TO_CHAR);
    }
  }
}

void spell_sanctuary(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  if ( IS_AFFECTED(victim,AFF_SANCTUARY) )
  {
    act("$N is already sanctified.",TRUE,ch,0,victim,TO_CHAR);
    return;
  }

  if (!affected_by_spell(victim, SPELL_SANCTUARY))
  {
    act("$n is surrounded by a white aura.",TRUE,victim,0,0,TO_ROOM);
    act("You start glowing.",TRUE,victim,0,0,TO_CHAR);

    af.type      = SPELL_SANCTUARY;
    af.duration  = (level<32) ? 3 : level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_SANCTUARY;
    affect_to_char(victim, &af);
  }
}

void spell_sleep(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;
  char buf[80];
  
  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );
  
  /* You can't sleep someone higher level than you*/

  if (GET_LEVEL(ch)<GET_LEVEL(victim)){
    sprintf(buf,"%s laughs in your face at your feeble attempt.\n\r",
	GET_NAME(victim));
    send_to_char(buf,ch);
    sprintf(buf,"%s tries to sleep you, but fails horribly.\n\r",
	GET_NAME(ch));
    send_to_char(buf,ch);
    one_hit(victim,ch,TYPE_UNDEFINED);    
    return;
  }

  if ( !saves_spell(victim, SAVING_SPELL) )
    {
      af.type      = SPELL_SLEEP;
      af.duration  = 4+level;
      af.modifier  = 0;
      af.location  = APPLY_NONE;
      af.bitvector = AFF_SLEEP;
      affect_join(victim, &af, FALSE, FALSE);

      if (GET_POS(victim)>POSITION_SLEEPING)
	{
	  act("You feel very sleepy ..... zzzzzz",FALSE,victim,0,0,TO_CHAR);
	  act("$n go to sleep.",TRUE,victim,0,0,TO_ROOM);
	  GET_POS(victim)=POSITION_SLEEPING;
	}
      
      return;
    }
  else
    act("$N does not look sleepy!", FALSE, ch, NULL, victim, TO_CHAR);
      
  one_hit(victim,ch,TYPE_UNDEFINED);
}

void spell_strength(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  assert(victim);

  act("You feel stronger.", FALSE, victim,0,0,TO_CHAR);

  af.type      = SPELL_STRENGTH;
  af.duration  = level;
  af.modifier  = 1+(level>18)+(level>25);

  af.location  = APPLY_STR;
  af.bitvector = 0;

  affect_join(victim, &af, TRUE, FALSE);
}



void spell_ventriloquate(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
    /* Actual spell resides in cast_ventriloquate */
}



void spell_word_of_recall(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  int loc_nr,location;

  void do_look(struct char_data *ch, char *argument, int cmd);

    assert(victim);

  if (IS_NPC(victim))
    return;

  /*  loc_nr = GET_HOME(ch); */
  loc_nr = 3001;
  location = real_room( loc_nr );
  if ( location == -1 )
  {
    send_to_char("You are completely lost.\n\r", victim);
    return;
  }

  /* a location has been found. */

  act("$n disappears.", TRUE, victim, 0, 0, TO_ROOM);
  char_from_room(victim);
  char_to_room(victim, location);
  act("$n appears in the middle of the room.", TRUE, victim, 0, 0, TO_ROOM);
  do_look(victim, "",15);

}


void spell_summon(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  sh_int target;

  assert(ch && victim);
  if (IS_SET(world[victim->in_room].room_flags, SAFE)){

    send_to_char("That person is in a safe area!\n\r",ch);
    return;
  }

  if (GET_LEVEL(victim) > MIN(31,level+3)) {
    send_to_char("You failed.\n\r",ch);
    return;
  }

  if ((IS_NPC(victim) && saves_spell(victim, SAVING_SPELL)) ||
      IS_SET(world[victim->in_room].room_flags,PRIVATE)) {
    send_to_char("You failed.\n\r", ch);
    return;
  }

  act("$n disappears suddenly.",TRUE,victim,0,0,TO_ROOM);

  target = ch->in_room;
  char_from_room(victim);
  char_to_room(victim,target);

  act("$n arrives suddenly.",TRUE,victim,0,0,TO_ROOM);
  act("$n has summoned you!",FALSE,ch,0,victim,TO_VICT);
  do_look(victim,"",15);
}


void spell_charm_person(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

  void add_follower(struct char_data *ch, struct char_data *leader);
  void stop_follower(struct char_data *ch);

  assert( victim && ch && level >= 1 && level <= 50 );

  check_killer( ch, victim );

  /* By testing for IS_AFFECTED we avoid ei. Mordenkainens sword to be */
  /* able to be "recharmed" with duration                              */

  if (victim == ch) {
    send_to_char("You like yourself even better!\n\r", ch);
    return;
  }

  if (!IS_AFFECTED(victim, AFF_CHARM) && !IS_AFFECTED(ch, AFF_CHARM) &&
      (level >= GET_LEVEL(victim))) {
    if (circle_follow(victim, ch)) {
      send_to_char("Sorry, following in circles can not be allowed.\n\r", ch);
      return;
    }

    if (saves_spell(victim, SAVING_PARA))
      {
	act("$N doesnt seem to like you!", FALSE, ch, NULL, victim, TO_CHAR);
	one_hit(victim,ch,TYPE_UNDEFINED);
	return;
      }

    if (victim->master)
      stop_follower(victim);

    add_follower(victim, ch);

    af.type      = SPELL_CHARM_PERSON;

    if (GET_INT(victim))
      af.duration  = 24*18/GET_INT(victim);
    else
      af.duration  = 24*18;

    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char(victim, &af);

    act("Isn't $n just such a nice fellow?",FALSE,ch,0,victim,TO_VICT);
  }
}



void spell_sense_life(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  struct affected_type af;

    assert(victim);

  if (!affected_by_spell(victim, SPELL_SENSE_LIFE)) {
    send_to_char("Your feel your awareness improve.\n\r", ch);

    af.type      = SPELL_SENSE_LIFE;
    af.duration  = 5*level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_SENSE_LIFE;
    affect_to_char(victim, &af);
  }
}

void spell_identify(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
  char buf[256], buf2[256];
  int i;
  bool found;

  struct time_info_data age(struct char_data *ch);

  /* Spell Names */
  extern char *spells[];

  /* For Objects */
  extern char *item_types[];
  extern char *extra_bits[];
  extern char *apply_types[];
  extern char *affected_bits[];

  assert(obj || victim);

  if (obj) {
    send_to_char("You feel informed:\n\r", ch);

    sprintf(buf, "Object '%s', Item type: ", obj->name);
    sprinttype(GET_ITEM_TYPE(obj),item_types,buf2);
    strcat(buf,buf2); strcat(buf,"\n\r");
    send_to_char(buf, ch);

    if (obj->obj_flags.bitvector) {
      send_to_char("Item will give you following abilities:  ", ch);
      sprintbit(obj->obj_flags.bitvector,affected_bits,buf);
      strcat(buf,"\n\r");
      send_to_char(buf, ch);
    }

    send_to_char("Item is: ", ch);
    sprintbit(obj->obj_flags.extra_flags,extra_bits,buf);
    strcat(buf,"\n\r");
    send_to_char(buf,ch);
    
    sprintf(buf,"Weight: %d, Value: %d\n\r",
	obj->obj_flags.weight, obj->obj_flags.cost);
    send_to_char(buf, ch);

    switch (GET_ITEM_TYPE(obj)) {

    case ITEM_SCROLL : 
    case ITEM_POTION :
      sprintf(buf, "Level %d spells of:\n\r",   obj->obj_flags.value[0]);
      send_to_char(buf, ch);
      if (obj->obj_flags.value[1] >= 1) {
    sprinttype(obj->obj_flags.value[1]-1,spells,buf);
    strcat(buf,"\n\r");
    send_to_char(buf, ch);
      }
      if (obj->obj_flags.value[2] >= 1) {
    sprinttype(obj->obj_flags.value[2]-1,spells,buf);
    strcat(buf,"\n\r");
    send_to_char(buf, ch);
      }
      if (obj->obj_flags.value[3] >= 1) {
    sprinttype(obj->obj_flags.value[3]-1,spells,buf);
    strcat(buf,"\n\r");
    send_to_char(buf, ch);
      }
      break;

    case ITEM_WAND : 
    case ITEM_STAFF : 
      sprintf(buf, "Has %d charges, with %d charges left.\n\r",
	  obj->obj_flags.value[1],
	  obj->obj_flags.value[2]);
      send_to_char(buf, ch);
      
      sprintf(buf, "Level %d spell of:\n\r",    obj->obj_flags.value[0]);
      send_to_char(buf, ch);

      if (obj->obj_flags.value[3] >= 1) {
    sprinttype(obj->obj_flags.value[3]-1,spells,buf);
    strcat(buf,"\n\r");
    send_to_char(buf, ch);
      }
      break;
      
    case ITEM_WEAPON :
      sprintf(buf, "Damage Dice is '%dD%d'\n\r",
	  obj->obj_flags.value[1],
	  obj->obj_flags.value[2]);
      send_to_char(buf, ch);
      break;

    case ITEM_ARMOR :
      sprintf(buf, "AC-apply is %d\n\r",
	  obj->obj_flags.value[0]);
      send_to_char(buf, ch);
      break;

    }

    found = FALSE;

    for (i=0;i<MAX_OBJ_AFFECT;i++) {
      if ((obj->affected[i].location != APPLY_NONE) &&
      (obj->affected[i].modifier != 0)) {
    if (!found) {
      send_to_char("Can affect you as:\n\r", ch);
      found = TRUE;
    }

    sprinttype(obj->affected[i].location,apply_types,buf2);
    sprintf(buf,"    Affects : %s By %d\n\r", buf2,obj->affected[i].modifier);
    send_to_char(buf, ch);
      }
    }

  } else {       /* victim */

    if (!IS_NPC(victim)) {
      sprintf(buf,"%d Years,  %d Months,  %d Days,  %d Hours old.\n\r",
	  age(victim).year, age(victim).month,
	  age(victim).day, age(victim).hours);
      send_to_char(buf,ch);

      sprintf(buf,"Height %dcm  Weight %dpounds \n\r",
	  GET_HEIGHT(victim), GET_WEIGHT(victim));
      send_to_char(buf,ch);


  sprintf(buf,"Str %d,  Int %d,  Wis %d,  Dex %d,  Con %d\n\r",
  GET_STR(victim),
  GET_INT(victim),
  GET_WIS(victim),
  GET_DEX(victim),
  GET_CON(victim) );
  send_to_char(buf,ch);


    } else {
      send_to_char("You learn nothing new.\n\r", ch);
    }
  }
}


/* ***************************************************************************
 *                     NPC spells..                                          *
 * ************************************************************************* */

void spell_fire_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
    int dam;
    int hpch;
    struct obj_data *burn;

    assert( victim && ch && level >= 1 && level <= 50 );

    check_killer( ch, victim );

    hpch = GET_HIT(ch);
    if(hpch<10) hpch=10;

    dam = number((hpch/8)+1,(hpch/4));

    if ( saves_spell(victim, SAVING_BREATH) )
	dam >>= 1;

    damage(ch, victim, dam, SPELL_FIRE_BREATH);

    /* And now for the damage on inventory */

    if(number(0,50)<GET_LEVEL(ch))
    {
      if (!saves_spell(victim, SAVING_BREATH) )
	{
	  for(burn=victim->carrying ; 
	  burn && !(((burn->obj_flags.type_flag==ITEM_SCROLL) || 
		 (burn->obj_flags.type_flag==ITEM_WAND) ||
		 (burn->obj_flags.type_flag==ITEM_STAFF) ||
		 (burn->obj_flags.type_flag==ITEM_NOTE)) &&
		(number(0,2)==0)) ;
	  burn=burn->next_content);
	  if(burn)
	{
	  act("$o burns",0,victim,burn,0,TO_CHAR);
	  extract_obj(burn);
	}
	}
    }
}


void spell_frost_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
    int dam;
    int hpch;
    struct obj_data *frozen;

    assert( victim && ch && level >= 1 && level <= 50 );

    check_killer( ch, victim );

    hpch = GET_HIT(ch);
    if(hpch<10) hpch=10;

    dam = number((hpch/8)+1,(hpch/4));

    if ( saves_spell(victim, SAVING_BREATH) )
	dam >>= 1;

    damage(ch, victim, dam, SPELL_FROST_BREATH);

    /* And now for the damage on inventory */

    if(number(0,50)<GET_LEVEL(ch))
    {
      if (!saves_spell(victim, SAVING_BREATH) )
	{
	  for(frozen=victim->carrying ; 
	  frozen && !(((frozen->obj_flags.type_flag==ITEM_DRINKCON) || 
		   (frozen->obj_flags.type_flag==ITEM_FOOD) ||
		   (frozen->obj_flags.type_flag==ITEM_POTION)) &&
		  (number(0,2)==0)) ;
	  frozen=frozen->next_content); 
	  if(frozen)
	{
	  act("$o breaks.",0,victim,frozen,0,TO_CHAR);
	  extract_obj(frozen);
	}
	}
    }
}


void spell_acid_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
    int dam;
    int hpch;
    int damaged;

    int apply_ac(struct char_data *ch, int eq_pos);
    
    assert( victim && ch && level >= 1 && level <= 50 );

    check_killer( ch, victim );

    hpch = GET_HIT(ch);
    if(hpch<10) hpch=10;

    dam = number((hpch/8)+1,(hpch/4));

    if ( saves_spell(victim, SAVING_BREATH) )
	dam >>= 1;

    damage(ch, victim, dam, SPELL_ACID_BREATH);

    /* And now for the damage on equipment */

    if(number(0,50)<GET_LEVEL(ch))
    {
      if (!saves_spell(victim, SAVING_BREATH) )
	{
	  for(damaged = 0; damaged<MAX_WEAR &&
	  !((victim->equipment[damaged]) &&
	    (victim->equipment[damaged]->obj_flags.type_flag!=ITEM_ARMOR) &&
	    (victim->equipment[damaged]->obj_flags.value[0]>0) && 
	    (number(0,2)==0)) ; damaged++);  
	  if(damaged<MAX_WEAR)
	{
	  act("$o is damaged.",0,victim,victim->equipment[damaged],0,TO_CHAR);
	  GET_AC(victim)-=apply_ac(victim,damaged);
	  victim->equipment[damaged]->obj_flags.value[0]-=number(1,7);
	  GET_AC(victim)+=apply_ac(victim,damaged);
	  victim->equipment[damaged]->obj_flags.cost = 0;
	}
	}
    }
      }


void spell_gas_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
    int dam;
    int hpch;

    assert( victim && ch && level >= 1 && level <= 50 );

    check_killer( ch, victim );

    hpch = GET_HIT(ch);
    if(hpch<10) hpch=10;

    dam = number((hpch/10)+1,(hpch/6));

    if ( saves_spell(victim, SAVING_BREATH) )
	dam >>= 1;

    damage(ch, victim, dam, SPELL_GAS_BREATH);

}


void spell_lightning_breath(byte level, struct char_data *ch,
  struct char_data *victim, struct obj_data *obj)
{
    int dam;
    int hpch;

    assert( victim && ch && level >= 1 && level <= 50 );

    check_killer( ch, victim );

    hpch = GET_HIT(ch);
    if(hpch<10) hpch=10;

    dam = number((hpch/8)+1,(hpch/4));

    if ( saves_spell(victim, SAVING_BREATH) )
	dam >>= 1;

    damage(ch, victim, dam, SPELL_LIGHTNING_BREATH);

}

/*********************************************************************
*                           New spells                  -Kahn        *
*********************************************************************/


void spell_fear(byte level, struct char_data *ch,
		struct char_data *victim, struct obj_data *obj)
{
    assert(victim && ch);
    assert((level >= 1) && (level <= 50));

    if (saves_spell(victim, SAVING_SPELL)) {
      send_to_char("For a moment you feel compelled to run away, but you fight back the urge.\n\r", victim);
      act("$N doesnt seem to be the yellow bellied slug you thought!",
	  FALSE, ch, NULL, victim, TO_CHAR);
      one_hit(victim, ch, TYPE_UNDEFINED);
      return;
    }
    send_to_char("You suddenly feel very frightened, and you attempt to flee!\n\r", victim);
    do_flee(victim, "", 151);
  }

void spell_refresh(byte level, struct char_data *ch,
		   struct char_data *victim, struct obj_data *obj)
{
    int dam;

    assert(ch && victim);

    dam = dice(level, 4) + level;
    dam = MAX(dam, 20);

    if ((dam + GET_MOVE(victim)) > move_limit(victim))
	GET_MOVE(victim) = move_limit(victim);
    else
	GET_MOVE(victim) += dam;

    send_to_char("You feel less tired.\n\r", victim);

  }

void spell_fly(byte level, struct char_data *ch,
	       struct char_data *victim, struct obj_data *obj)
{
    struct affected_type af;

    assert(ch && victim);

    act("You feel lighter than air!", TRUE, ch, 0, victim, TO_VICT);
    if (victim != ch) {
	act("$N's feet rise off the ground.", TRUE, ch, 0, victim, TO_CHAR);
      } else {
	send_to_char("Your feet rise up off the ground.", ch);
      }
    act("$N's feet rise off the ground.", TRUE, ch, 0, victim, TO_NOTVICT);

    af.type = SPELL_FLY;
    af.duration = GET_LEVEL(ch) + 3;
    af.modifier = 0;
    af.location = 0;
    af.bitvector = AFF_FLYING;
    affect_to_char(victim, &af);
  }

void spell_cont_light(byte level, struct char_data *ch,
		      struct char_data *victim, struct obj_data *obj)
{
/*
   creates a ball of light in the hands.
*/
    struct obj_data *tmp_obj;

    assert(ch);

    CREATE(tmp_obj, struct obj_data, 1);
    clear_object(tmp_obj);

    tmp_obj->name = str_dup("ball light");
    tmp_obj->short_description = str_dup("A bright ball of light");
    tmp_obj->description = str_dup("There is a bright ball of light on the ground here.");

    tmp_obj->obj_flags.type_flag = ITEM_LIGHT;
    tmp_obj->obj_flags.wear_flags = ITEM_TAKE;
    tmp_obj->obj_flags.value[2] = -1;
    tmp_obj->obj_flags.weight = 1;
    tmp_obj->obj_flags.cost = 0;
    tmp_obj->obj_flags.cost_per_day = 1;

    tmp_obj->next = object_list;
    object_list = tmp_obj;

    obj_to_char(tmp_obj, ch);

    tmp_obj->item_number = -1;

    act("$n twiddles $s thumbs and $p suddenly appears.", TRUE, ch, tmp_obj, 0, TO_ROOM);
    act("You twiddle your thumbs and $p suddenly appears.", TRUE, ch, tmp_obj, 0, TO_CHAR);

  }

#if 0
void spell_animate_dead(byte level, struct char_data *ch,
			struct char_data *victim, struct obj_data *corpse)
{

    struct char_data *mob;
    struct obj_data *obj_object, *sub_object, *next_obj, *i;
    struct affected_type af;
    char buf[MAX_STRING_LENGTH];
    int number = 100,          /* Number for Zombie */
    r_num;

/*
 some sort of check for corpse hood
*/
    if ((GET_ITEM_TYPE(corpse) != ITEM_CONTAINER) ||
	(!corpse->obj_flags.value[3])) {
	send_to_char("The magic fails abruptly!\n\r", ch);
	return;
      }
    if ((r_num = real_mobile(number)) < 0) {
	send_to_char("Mobile: Zombie not found.\n\r", ch);
	return;
      }
    mob = read_mobile(r_num, REAL);
    char_to_room(mob, ch->in_room);

/*
  zombie should be charmed and follower ch
*/

    af.type = SPELL_CHARM_PERSON;
    af.duration = MIN(1, GET_LEVEL(ch) / 3);
    af.modifier = 0;
    af.location = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char(mob, &af);
    add_follower(mob, ch);

    GET_EXP(mob) = 100 * GET_LEVEL(ch);
    IS_CARRYING_W(mob) = 0;
    IS_CARRYING_N(mob) = 0;

/*
  take all from corpse, and give to zombie
*/

    for (obj_object = corpse->contains; obj_object; obj_object = next_obj) {
	next_obj = obj_object->next_content;
	obj_from_obj(obj_object);
	obj_to_char(obj_object, mob);
      }

/*
   set up descriptions and such
*/
    
    sprintf(buf, "%s body undead", corpse->name);
    mob->player.name = str_dup(buf);
    sprintf(buf, "The undead body of %s", corpse->short_description);
    mob->player.short_descr = str_dup(buf);
    sprintf(buf, "The undead body of %s slowly staggers around.\n\r", corpse->short_description);
    mob->player.long_descr = str_dup(buf);

/*
  set up hitpoints
*/

    mob->points.max_hit = dice((level + 1), 8);
    mob->points.hit = (int) (mob->points.max_hit / 2);

    GET_LEVEL(mob) = GET_LEVEL(ch);
    mob->player.sex = SEX_NEUTRAL;

    SET_BIT(mob->specials.act, ACT_UNDEAD);

/*
  get rid of corpse
*/
    
    act("With mystic power, $n animates $p.", TRUE, ch,
	corpse, 0, TO_ROOM);
    act("$N slowly rises from the ground.", FALSE, ch, 0, mob, TO_ROOM);

    extract_obj(corpse);
  }
#endif

/*  This should be checked out for real mob creating.  */
  
void spell_know_alignment(byte level, struct char_data *ch,
			  struct char_data *victim, struct obj_data *obj)
{
    int ap;
    char buf[200], name[100];

    assert(victim && ch);

    if (IS_NPC(victim))
	strcpy(name, victim->player.short_descr);
    else
	strcpy(name, GET_NAME(victim));

    ap = GET_ALIGNMENT(victim);

    if (ap > 700)
	sprintf(buf, "%s has an aura as white as the driven snow.\n\r", name);
    else if (ap > 350)
	sprintf(buf, "%s is of excellent moral character.\n\r", name);
    else if (ap > 100)
	sprintf(buf, "%s is often kind and thoughtful.\n\r", name);
    else if (ap > 25)
	sprintf(buf, "%s isn't a bad sort...\n\r", name);
    else if (ap > -25)
	sprintf(buf, "%s doesn't seem to have a firm moral commitment\n\r", name);
    else if (ap > -100)
	sprintf(buf, "%s isn't the worst you've come across\n\r", name);
    else if (ap > -350)
	sprintf(buf, "%s could be a little nicer, but who couldn't?\n\r", name);
    else if (ap > -700)
	sprintf(buf, "%s probably just had a bad childhood\n\r", name);
    else
	sprintf(buf, "I'd rather just not say anything at all about %s\n\r", name);

    send_to_char(buf, ch);

  }

void spell_dispel_magic(byte level, struct char_data *ch,
			struct char_data *victim, struct obj_data *obj)
{
    int yes = 0;

    assert(ch && victim);

/* gets rid of infravision, invisibility, detect, etc */

    if (GET_LEVEL(victim) <= GET_LEVEL(ch))
	yes = TRUE;
    else
	yes = FALSE;

    if (affected_by_spell(victim, SPELL_INVISIBLE))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_INVISIBLE);
	    send_to_char("You feel exposed.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_DETECT_INVISIBLE))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_DETECT_INVISIBLE);
	    send_to_char("You feel less perceptive.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_DETECT_EVIL))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_DETECT_EVIL);
	    send_to_char("You feel less morally alert.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_DETECT_MAGIC))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_DETECT_MAGIC);
	    send_to_char("You stop noticing the magic in your life.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_SENSE_LIFE))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_SENSE_LIFE);
	    send_to_char("You feel less in touch with living things.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_SANCTUARY))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_SANCTUARY);
	    send_to_char("You don't feel so invulnerable anymore.\n\r", victim);
	    act("The white glow around $n's body fades.", FALSE, victim, 0, 0, TO_ROOM);
      }
    if (IS_AFFECTED(victim, AFF_SANCTUARY))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    REMOVE_BIT(victim->specials.affected_by, AFF_SANCTUARY);
	    send_to_char("You don't feel so invulnerable anymore.\n\r", victim);
	    act("The white glow around $n's body fades.", FALSE, victim, 0, 0, TO_ROOM);
      }
    if (affected_by_spell(victim, SPELL_PROTECT_FROM_EVIL))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_PROTECT_FROM_EVIL);
	    send_to_char("You feel less morally protected.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_INFRAVISION))
      if (yes || !saves_spell(victim, SAVING_SPELL)) {
    affect_from_char(victim, SPELL_INFRAVISION);
    send_to_char("Your sight grows dimmer.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_FAERIE_FIRE))
      if (yes || !saves_spell(victim, SAVING_SPELL)) {
    affect_from_char(victim, SPELL_FAERIE_FIRE);
    send_to_char("Your pink aura vanishes.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_SLEEP))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_SLEEP);
	    send_to_char("You don't feel so tired.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_CHARM_PERSON))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_CHARM_PERSON);
	    send_to_char("You feel less enthused about your master.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_STRENGTH))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_STRENGTH);
	    send_to_char("You don't feel so strong.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_ARMOR))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_ARMOR);
	    send_to_char("You don't feel so well protected.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_DETECT_POISON))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_DETECT_POISON);
	    send_to_char("You don't feel so sensitive to fumes.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_BLESS))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_BLESS);
	    send_to_char("You don't feel so blessed.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_FLY))
	if (yes || !saves_spell(victim, SAVING_SPELL)) {
	    affect_from_char(victim, SPELL_FLY);
	    send_to_char("You don't feel lighter than air anymore.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_BLINDNESS))
      if (yes || !saves_spell(victim, SAVING_SPELL)) {
    affect_from_char(victim, SPELL_BLINDNESS);
    send_to_char("Your vision returns.\n\r", victim);
      }
    if (affected_by_spell(victim, SPELL_POISON)) {
      if (yes || !saves_spell(victim, SAVING_SPELL)) {
    affect_from_char(victim, SPELL_POISON);
      }
    }

  }

void spell_conjure_elemental(byte level, struct char_data *ch,
			  struct char_data *victim, struct obj_data *obj)
{
    struct affected_type af;

    /*
     *   victim, in this case, is the elemental
     *   object could be the sacrificial object
     */

    assert(ch && victim && obj);

    /*
    ** objects:
    **     fire  : red stone
    **     water : pale blue stone
    **     earth : grey stone
    **     air   : clear stone
    */

    act("$n gestures, and a cloud of smoke appears", TRUE, ch, 0, 0, TO_ROOM);
    act("$n gestures, and a cloud of smoke appears", TRUE, ch, 0, 0, TO_CHAR);
    act("$p explodes with a loud BANG!", TRUE, ch, obj, 0, TO_ROOM);
    act("$p explodes with a loud BANG!", TRUE, ch, obj, 0, TO_CHAR);
    obj_from_char(obj);
    extract_obj(obj);
    char_to_room(victim, ch->in_room);
    act("Out of the smoke, $N emerges", TRUE, ch, 0, victim, TO_NOTVICT);

    /* charm them for a while */
    if (victim->master)
	stop_follower(victim);

    add_follower(victim, ch);

    af.type = SPELL_CHARM_PERSON;
    af.duration = 48;
    af.modifier = 0;
    af.location = 0;
    af.bitvector = AFF_CHARM;

    affect_to_char(victim, &af);

  }

void spell_cure_serious(byte level, struct char_data *ch,
			struct char_data *victim, struct obj_data *obj)
{
    int healpoints;

    assert(ch && victim);

    healpoints = dice(2, 8) +(level/2);

    if ((healpoints + GET_HIT(victim)) > hit_limit(victim))
	GET_HIT(victim) = hit_limit(victim);
    else
	GET_HIT(victim) += healpoints;

    update_pos(victim);

    send_to_char("You feel better!\n\r", victim);
  }

void spell_cause_light(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
    int dam;
    assert(ch && victim);

    check_killer(ch, victim);
    
    dam = dice(1, 8) + (level/3);

    damage(ch, victim, dam, SPELL_CAUSE_LIGHT);

  }

void spell_cause_critical(byte level, struct char_data *ch,
			  struct char_data *victim, struct obj_data *obj)
{
    int dam;
    assert(ch && victim);

    check_killer(ch, victim);
    
    dam = dice(3,8)-6+level;

    damage(ch, victim, dam, SPELL_CAUSE_CRITICAL);

  }

void spell_cause_serious(byte level, struct char_data *ch,
			 struct char_data *victim, struct obj_data *obj)
{
    int dam;
    assert(ch && victim);

    check_killer(ch, victim);
    
    dam = dice(2, 8) + (level/2);

    damage(ch, victim, dam, SPELL_CAUSE_SERIOUS);
  }

void spell_flamestrike(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
    int dam;

    assert(victim && ch);

    check_killer (ch, victim);
    
    dam = dice(6, 8);

    if (saves_spell(victim, SAVING_SPELL))
	dam >>= 1;

    damage(ch, victim, dam, SPELL_FLAMESTRIKE);

  }

/*
   magic user spells
*/

void spell_stone_skin(byte level, struct char_data *ch,
		      struct char_data *victim, struct obj_data *obj)
{
    struct affected_type af;

    assert(ch);

    if (!affected_by_spell(ch, SPELL_STONE_SKIN)) {
	act("$n's skin turns grey and granite-like.", TRUE, ch, 0, 0, TO_ROOM);
	act("Your skin turns to a stone-like substance.", TRUE, ch, 0, 0, TO_CHAR);

	af.type = SPELL_STONE_SKIN;
	af.duration = level;
	af.modifier = -40;
	af.location = APPLY_AC;
	af.bitvector = 0;
	affect_to_char(ch, &af);

	}
}

void spell_shield(byte level, struct char_data *ch,
		  struct char_data *victim, struct obj_data *obj)
{
    struct affected_type af;

    assert(victim && ch);

    if (!affected_by_spell(victim, SPELL_SHIELD)) {
	act("$N is surrounded by a strong force shield.", TRUE, ch, 0, victim, TO_NOTVICT);
	if (ch != victim) {
	    act("$N is surrounded by a strong force shield.", TRUE, ch, 0, victim, TO_CHAR);
	    act("You are surrounded by a strong force shield.", TRUE, ch, 0, victim, TO_VICT);
      } else {
	    act("You are surrounded by a strong force shield.", TRUE, ch, 0, victim, TO_VICT);
      }

	af.type = SPELL_SHIELD;
	af.duration = 8 + level;
	af.modifier = -20;
	af.location = APPLY_AC;
	af.bitvector = 0;
	affect_to_char(victim, &af);
      }
  }

void spell_weaken(byte level, struct char_data *ch,
		  struct char_data *victim, struct obj_data *obj)
{
    struct affected_type af;
    float modifier;

    assert(ch && victim);

    check_killer (ch, victim);
    
    if (!affected_by_spell(victim, SPELL_WEAKEN))
      {
	if (!saves_spell(victim, SAVING_SPELL)) {
	  modifier = (77.0 - level) / 100.0;
	  act("You feel weaker.", FALSE, victim, 0, 0, TO_VICT);
	  act("$n seems weaker.", FALSE, victim, 0, 0, TO_ROOM);
	  
	  af.type = SPELL_WEAKEN;
	  af.duration = (int) level / 2;
	  af.modifier = (int) 0 - (victim->abilities.str * modifier);
	  af.location = APPLY_STR;
	  af.bitvector = 0;

	  affect_to_char(victim, &af);
	}
	else
	  {
	    act("$N isn't the wimp you made $Sm out to be!", FALSE,
		ch, NULL, victim, TO_CHAR);
	  }
      }
    
    one_hit(victim, ch, TYPE_UNDEFINED);
  }

void spell_mass_invis(byte level, struct char_data *ch,
		      struct char_data *victim, struct obj_data *obj)
{
    struct char_data *tmp_victim;
    struct affected_type af;

    assert(ch);

    for (tmp_victim = world[ch->in_room].people; tmp_victim;
	 tmp_victim = tmp_victim->next_in_room) {
	if ((ch->in_room == tmp_victim->in_room) && (ch != tmp_victim))
	    if (!affected_by_spell(tmp_victim, SPELL_INVISIBLE)) {

		act("$n slowly fades out of existence.", TRUE, tmp_victim, 0, 0\
, TO_ROOM);
		send_to_char("You vanish.\n\r", tmp_victim);

		af.type = SPELL_INVISIBLE;
		af.duration = 24;
		af.modifier = -40;
		af.location = APPLY_AC;
		af.bitvector = AFF_INVISIBLE;
		affect_to_char(tmp_victim, &af);
	  }
      }
  }

void spell_acid_blast(byte level, struct char_data *ch,
		      struct char_data *victim, struct obj_data *obj)
{

    int dam;

    assert(victim && ch);

    check_killer (ch, victim);
    
    dam = dice(level, 6);

    if (saves_spell(victim, SAVING_SPELL))
	dam >>= 1;

    damage(ch, victim, dam, SPELL_ACID_BLAST);

  }

#if  0
void spell_gate(byte level, struct char_data *ch,
		struct char_data *victim, struct obj_data *obj)
{
    struct affected_type af;
    struct char_data *mob;
    int vnum;

   /* load in a monster of the correct type, determined by
       level of the spell */

    /* really simple to start out with */

    if (level <= 5) {
	vnum = 1100;
      } else if (level <= 7) {
	vnum = 1101;
      } else if (level <= 9) {
	vnum = 1102;
      } else if (level <= 11) {
	vnum = 1103;
      } else if (level <= 13) {
	vnum = 1104;
      } else if (level <= 15) {
	vnum = 1105;
      } else if (level <= 19) {
	vnum = 1106;
      } else {
	vnum = 1107;
      }

    mob = read_mobile(vnum, VIRTUAL);

    char_to_room(mob, ch->in_room);

    act("$n inscribes a glowing pentacle in the air, and $N jumps through.", TRUE, ch, 0, mob, TO_ROOM);
    act("You inscribe a glowing pentacle in the air, and $N jumps through.", TRUE, ch, 0, mob, TO_CHAR);

    add_follower(mob, ch);

    af.type = SPELL_CHARM_PERSON;
    af.duration = (level/2) + dice(1, 5);
    af.modifier = 0;
    af.location = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char(mob, &af);

    if (!number(0, 9)) SET_BIT(mob->specials.act, ACT_AGGRESSIVE);
  }
#endif
     
void spell_faerie_fire(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
    struct affected_type af;

    assert(ch && victim);

    if (affected_by_spell(victim, SPELL_FAERIE_FIRE)) {
	send_to_char("Nothing new seems to happen", ch);
	return;
      }
    act("$n points at $N.", TRUE, ch, 0, victim, TO_ROOM);
    act("You point at $N.", TRUE, ch, 0, victim, TO_CHAR);
    act("$N is surrounded by a pink outline", TRUE, ch, 0, victim, TO_ROOM);
    act("$N is surrounded by a pink outline", TRUE, ch, 0, victim, TO_CHAR);

    af.type = SPELL_FAERIE_FIRE;
    af.duration = level;
    af.modifier = +10;
    af.location = APPLY_ARMOR;
    af.bitvector = 0;

    affect_to_char(victim, &af);

  }

void spell_faerie_fog(byte level, struct char_data *ch,
		      struct char_data *victim, struct obj_data *obj)
{
    struct char_data *tmp_victim;

    assert(ch);

    act("$n snaps $s fingers, and a cloud of purple smoke billows forth",
	TRUE, ch, 0, 0, TO_ROOM);
    act("You snap your fingers, and a cloud of purple smoke billows forth",
	TRUE, ch, 0, 0, TO_CHAR);


    for (tmp_victim = world[ch->in_room].people; tmp_victim;
	 tmp_victim = tmp_victim->next_in_room) {
	if ((ch->in_room == tmp_victim->in_room) && (ch != tmp_victim)) {
	    if (GET_LEVEL(tmp_victim) > 31)
		break;
	    if (IS_AFFECTED(tmp_victim, AFF_INVISIBLE)) {
		if (saves_spell(tmp_victim, SAVING_SPELL)) {
		    REMOVE_BIT(tmp_victim->specials.affected_by, AFF_INVISIBLE);
		    act("$n is briefly revealed, but disappears again.",
			TRUE, tmp_victim, 0, 0, TO_ROOM);
		    act("You are briefly revealed, but disappear again.",
			TRUE, tmp_victim, 0, 0, TO_CHAR);
		    SET_BIT(tmp_victim->specials.affected_by, AFF_INVISIBLE);
	  }
	  }
      }
      }
  }

void spell_drown(byte level, struct char_data *ch,
		 struct char_data *victim, struct obj_data *obj)
{
    int dam;

    check_killer( ch, victim);
    
    if (ch->in_room < 0)
	return;
    dam = dice(level, 3);

    act("$n gestures, and a huge pool of water shoots from the ground!\n\r",
	FALSE, ch, 0, 0, TO_ROOM);

    damage(ch, victim, dam, SPELL_DROWN);
  }

void spell_demonfire(byte level, struct char_data *ch,
		     struct char_data *victim, struct obj_data *obj)
{
    int dam;

    assert(ch && victim && (level > 0));

    check_killer( ch, victim );

    /* needs a description */
    
    dam = dice(level, 8);

    damage(ch, victim, dam, SPELL_DEMONFIRE);
  }

void spell_turn_undead(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
    int dam;

    assert(ch && victim && (level > 0));

    if (!IS_SET(victim->specials.act, ACT_UNDEAD)) {
	act("But $n isn't undead!", FALSE, ch, 0, victim, TO_CHAR);
	return;
      }
    dam = dice(level, 10);
    damage(ch, victim, dam, SPELL_TURN_UNDEAD);
  }

void spell_infravision(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
    struct affected_type af;

    assert(victim && ch);
    if (!IS_AFFECTED(victim, AFF_INFRARED)) {
	if (ch != victim) {
	    send_to_char("Your eyes glow red for a moment.\n\r", victim);
	    act("$n's eyes take on a red hue.\n\r", FALSE, victim, 0, 0, TO_ROOM);
      } else {
	    send_to_char("Your eyes glow red.\n\r", ch);
	    act("$n's eyes glow red.\n\r", FALSE, ch, 0, 0, TO_ROOM);
      }
	af.type = SPELL_INFRAVISION;
	af.duration = 2 * level;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_INFRARED;
	affect_to_char(victim, &af);
      } else {
	send_to_char("Nothing seems to happen.\n\r", ch);
      }
  }

void spell_sandstorm(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)

{
    int dam, chance, dir, room;

    if (world[ch->in_room].sector_type != SECT_DESERT) {
	send_to_char("Nothing seems to happen.\n\r", ch);
	return;
      }

    check_killer (ch, victim);
    
    GET_MOVE(victim) -= (level * 2);
    GET_MOVE(victim) = MAX(GET_MOVE(victim), 0);

    dam = dice(3, level);
    damage(ch, victim, dam, SPELL_SANDSTORM);

    if (!victim || (victim->in_room == NOWHERE)) return;

    chance = (level * 5) - 10;
    if ((chance > number(1, 100)) && world[ch->in_room].dir_option[dir = number(0, 5)]) {
	act("Your sandstorm hurls $N out of the room!", FALSE, ch, 0, victim, TO_CHAR);
	act("$N is hurled out of the room!", FALSE, ch, 0, victim, TO_NOTVICT);
	act("You are hurled out of the room!", FALSE, ch, 0, victim, TO_VICT);

	room = world[ch->in_room].dir_option[dir]->to_room;
	char_from_room(victim);
	char_to_room(victim, room);

	act("$n is hurled into the room!", FALSE, victim, 0, 0, TO_ROOM);
      }
  }
/*
void spell_hands_of_wind(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
    int dir, room;

    if (saves_spell(victim, SAVING_SPELL)) {
	send_to_char("Nothing seems to happen.\n\r", ch);
	return;
      }

    if (GET_LEVEL(victim) > GET_LEVEL(ch)) {
	send_to_char("Nothing seems to happen.\n\r", ch);
	return;
      }

    if ((dir = choose_exit(victim->in_room, ch->in_room, level * 5)) == -1) {
	send_to_char("Nothing seems to happen.\n\r", ch);
	return;
      }

    act("You sense $S aura drawing closer...", FALSE, ch, 0, victim, TO_CHAR);

    act("$n is hurled out of the room by a violent wind!", FALSE, victim, 0, 0, TO_ROOM);
    act("You are hurled out of the room by a violent wind!", FALSE, victim, 0, 0, TO_CHAR);

    room = victim->in_room;
    char_from_room(victim);
    char_to_room(victim, world[room].dir_option[dir]->to_room);

    act("$n is hurled into the room by a violent wind!", FALSE, victim, 0, 0, TO_ROOM);
    do_look(victim, "", 0);
  }
*/  /* have to set up the random choose_exit routine */

void spell_plague(byte level, struct char_data *ch,
		       struct char_data *victim, struct obj_data *obj)
{
    struct affected_type af;

    check_killer (ch, victim);
    
    if (saves_spell(victim, SAVING_SPELL) || (!number(0, 5))) {
	act("$n appears to be unaffected.", FALSE, ch, 0, victim, TO_CHAR);
	one_hit(victim,ch,TYPE_UNDEFINED);
	return;
      }

    af.type = SPELL_PLAGUE;
    af.duration = level / 5;
    af.modifier = -5;
    af.location = APPLY_STR;
    af.bitvector = AFF_PLAGUE;

    affect_join(victim, &af, FALSE, FALSE);

    act("$n chokes and utters some muffled noises.", FALSE, victim, 0, 0, TO_ROOM);
    act("You choke and utter some muffled noises.", FALSE, victim, 0, 0, TO_CHAR);
  }

/*********************************************************************
*  The following are the higher level procedures that handle the     *
*  abilities of objects/people that cast spells.      -Kahn          *
*********************************************************************/

void cast_burning_hands( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj )
{
    switch (type)
    {
    case SPELL_TYPE_SPELL:
	spell_burning_hands(level, ch, victim, 0); 
	break;
	case SPELL_TYPE_SCROLL:
	if (victim)
	  spell_burning_hands(level, ch, victim, 0);
	else if (!tar_obj)
	  spell_burning_hands(level, ch, ch, 0);
	break;
	case SPELL_TYPE_WAND:
	if (victim)
	  spell_burning_hands(level, ch, victim, 0);
	break;
    default : 
	log("Serious screw-up in burning hands!");
	break;
    }
}


void cast_call_lightning( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj )
{
  extern struct weather_data weather_info;

    switch (type) {
	case SPELL_TYPE_SPELL:
	    if (OUTSIDE(ch) && (weather_info.sky>=SKY_RAINING)) {
		spell_call_lightning(level, ch, victim, 0);
	    } else {
		send_to_char("You fail to call upon the lightning from the sky!\\n\r", ch);
	    }
	    break;
      case SPELL_TYPE_POTION:
	    if (OUTSIDE(ch) && (weather_info.sky>=SKY_RAINING)) {
		spell_call_lightning(level, ch, ch, 0);
	    }
	    break;
      case SPELL_TYPE_SCROLL:
	    if (OUTSIDE(ch) && (weather_info.sky>=SKY_RAINING)) {
		if(victim) 
		    spell_call_lightning(level, ch, victim, 0);
		else if(!tar_obj) spell_call_lightning(level, ch, ch, 0);
	    }
	    break;
      case SPELL_TYPE_STAFF:
	    if (OUTSIDE(ch) && (weather_info.sky>=SKY_RAINING))
	    {
		for (victim = world[ch->in_room].people ;
		victim ; victim = victim->next_in_room )
		    if( IS_NPC(victim) )
			spell_call_lightning(level, ch, victim, 0);
	    }
	    break;
      default : 
	 log("Serious screw-up in call lightning!");
	 break;
    }
}


void cast_chill_touch( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_chill_touch(level, ch, victim, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (victim)
      spell_chill_touch(level, ch, victim, 0);
    else if (!tar_obj)
      spell_chill_touch(level, ch, ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (victim)
      spell_chill_touch(level, ch, victim, 0);
    break;          
  default : 
    log("Serious screw-up in chill touch!");
    break;
    }
}


void cast_shocking_grasp( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_shocking_grasp(level, ch, victim, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (victim)
      spell_shocking_grasp(level, ch, victim, 0);
    else if (!tar_obj)
      spell_shocking_grasp(level, ch, ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (victim)
      spell_shocking_grasp(level, ch, victim, 0);
    break;          
  default : 
      log("Serious screw-up in shocking grasp!");
    break;
  }
}


void cast_colour_spray( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_colour_spray(level, ch, victim, 0);
	 break; 
    case SPELL_TYPE_SCROLL:
	 if(victim) 
	    spell_colour_spray(level, ch, victim, 0);
	 else if (!tar_obj)
		spell_colour_spray(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_WAND:
	 if(victim) 
	    spell_colour_spray(level, ch, victim, 0);
	 break;
    default : 
	 log("Serious screw-up in colour spray!");
	 break;
    }
}


void cast_earthquake( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
    case SPELL_TYPE_SCROLL:
    case SPELL_TYPE_STAFF:
	    spell_earthquake(level, ch, 0, 0);
	  break;
    default : 
	 log("Serious screw-up in earthquake!");
	 break;
    }
}


void cast_energy_drain( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_energy_drain(level, ch, victim, 0);
	    break;
    case SPELL_TYPE_POTION:
	 spell_energy_drain(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_SCROLL:
	 if(victim)
		spell_energy_drain(level, ch, victim, 0);
	 else if(!tar_obj)
	    spell_energy_drain(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_WAND:
	 if(victim)
		spell_energy_drain(level, ch, victim, 0);
	 break;
    case SPELL_TYPE_STAFF:
	 for (victim = world[ch->in_room].people ;
	      victim ; victim = victim->next_in_room )
	    if( IS_NPC(victim) )
	       spell_energy_drain(level, ch, victim, 0);
	 break;
    default : 
	 log("Serious screw-up in energy drain!");
	 break;
    }
}


void cast_fireball( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	  spell_fireball(level, ch, victim, 0);
	break;
    case SPELL_TYPE_SCROLL:
	 if(victim)
		spell_fireball(level, ch, victim, 0);
	 else if(!tar_obj)
	    spell_fireball(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_WAND:
	 if(victim)
		spell_fireball(level, ch, victim, 0);
	 break;
    default : 
	 log("Serious screw-up in fireball!");
	 break;

    }
}


void cast_harm( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj )
{
    switch (type) {
    case SPELL_TYPE_SPELL:
	 spell_harm(level, ch, victim, 0);
	 break;
    case SPELL_TYPE_POTION:
	 spell_harm(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_STAFF:
	 for (victim = world[ch->in_room].people ;
	      victim ; victim = victim->next_in_room )
	    if ( IS_NPC(victim) )
	       spell_harm(level, ch, victim, 0);
	 break;
    default : 
	 log("Serious screw-up in harm!");
	 break;

  }
}


void cast_lightning_bolt( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	 spell_lightning_bolt(level, ch, victim, 0);
	 break;
    case SPELL_TYPE_SCROLL:
	 if(victim)
		spell_lightning_bolt(level, ch, victim, 0);
	 else if(!tar_obj)
	    spell_lightning_bolt(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_WAND:
	 if(victim)
		spell_lightning_bolt(level, ch, victim, 0);
	 break;
    default : 
	 log("Serious screw-up in lightning bolt!");
	 break;

  }
}


void cast_magic_missile( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *victim, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
      spell_magic_missile(level, ch, victim, 0);
      break;
    case SPELL_TYPE_SCROLL:
	 if(victim)
		spell_magic_missile(level, ch, victim, 0);
	 else if(!tar_obj)
	    spell_magic_missile(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_WAND:
	 if(victim)
		spell_magic_missile(level, ch, victim, 0);
	 break;
    default : 
	 log("Serious screw-up in magic missile!");
	 break;

  }
}

void cast_armor( byte level, struct char_data *ch, char *arg, int type,
    struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
	case SPELL_TYPE_SPELL:
	    if ( affected_by_spell(tar_ch, SPELL_ARMOR) ){
		send_to_char("Nothing seems to happen.\n\r", ch);
		return;
	    }
	    if (ch != tar_ch)
		act("$N is protected by your deity.", FALSE, ch, 0, tar_ch, TO_CHAR);

	    spell_armor(level,ch,tar_ch,0);
	    break;
	case SPELL_TYPE_POTION:
	    if ( affected_by_spell(ch, SPELL_ARMOR) )
		return;
	    spell_armor(level,ch,ch,0);
	    break;
	case SPELL_TYPE_SCROLL:
	    if (tar_obj) return;
	    if (!tar_ch) tar_ch = ch;
	    if ( affected_by_spell(tar_ch, SPELL_ARMOR) )
		return;
	    spell_armor(level,ch,ch,0);
	    break;
	case SPELL_TYPE_WAND:
	    if (tar_obj) return;
	    if (!tar_ch) tar_ch = ch;
	    if ( affected_by_spell(tar_ch, SPELL_ARMOR) )
		return;
	    spell_armor(level,ch,tar_ch,0);
	    break;
      default : 
	 log("Serious screw-up in armor!");
	 break;
    }
}



void cast_teleport( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SCROLL:
  case SPELL_TYPE_POTION:
  case SPELL_TYPE_SPELL:
    if (!tar_ch)
      tar_ch = ch;
    spell_teleport(level, ch, tar_ch, 0);
    break;
    
  case SPELL_TYPE_WAND:
    if(!tar_ch) tar_ch = ch;
    spell_teleport(level, ch, tar_ch, 0);
    break;

    case SPELL_TYPE_STAFF:
      for (tar_ch = world[ch->in_room].people ; 
	   tar_ch ; tar_ch = tar_ch->next_in_room)
	 if ( IS_NPC(tar_ch) )
	    spell_teleport(level, ch, tar_ch, 0);
      break;
	    
    default : 
      log("Serious screw-up in teleport!");
      break;
    }
}


void cast_bless( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{

  switch (type) {
  case SPELL_TYPE_SPELL:
    if (tar_obj) {        /* It's an object */
      if ( IS_SET(tar_obj->obj_flags.extra_flags, ITEM_BLESS) ) {
    send_to_char("Nothing seems to happen.\n\r", ch);
    return;
      }
      spell_bless(level,ch,0,tar_obj);
      
    } else {              /* Then it is a PC | NPC */
      
      if ( affected_by_spell(tar_ch, SPELL_BLESS) ||
      (GET_POS(tar_ch) == POSITION_FIGHTING)) {
    send_to_char("Nothing seems to happen.\n\r", ch);
    return;
      } 
      spell_bless(level,ch,tar_ch,0);
    }
    break;
  case SPELL_TYPE_POTION:
    if ( affected_by_spell(ch, SPELL_BLESS) ||
    (GET_POS(ch) == POSITION_FIGHTING)) {
    send_to_char("Nothing seems to happen.\n\r", ch);
    return;
      }
    spell_bless(level,ch,ch,0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) {        /* It's an object */
      if ( IS_SET(tar_obj->obj_flags.extra_flags, ITEM_BLESS) )
    {
    send_to_char("Nothing seems to happen.\n\r", ch);
    return;
      }
      spell_bless(level,ch,0,tar_obj);
      
    } else {              /* Then it is a PC | NPC */
      
      if (!tar_ch) tar_ch = ch;
      
      if ( affected_by_spell(tar_ch, SPELL_BLESS) ||
      (GET_POS(tar_ch) == POSITION_FIGHTING))
    {
    send_to_char("Nothing seems to happen.\n\r", ch);
    return;
      }
      spell_bless(level,ch,tar_ch,0);
    }
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) {        /* It's an object */
      if ( IS_SET(tar_obj->obj_flags.extra_flags, ITEM_BLESS) )
    {
    send_to_char("Nothing seems to happen.\n\r", ch);
    return;
      }
      spell_bless(level,ch,0,tar_obj);
      
    } else {              /* Then it is a PC | NPC */
      
      if ( affected_by_spell(tar_ch, SPELL_BLESS) ||
      (GET_POS(tar_ch) == POSITION_FIGHTING))
    {
    send_to_char("Nothing seems to happen.\n\r", ch);
    return;
      }
      spell_bless(level,ch,tar_ch,0);
    }
    break;
    default : 
      log("Serious screw-up in bless!");
    break;
  }
}



void cast_blindness( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{

    if (IS_SET(world[ch->in_room].room_flags,SAFE)){
      send_to_char("You can not blind anyone in a safe area!\n\r", ch);
      return;
    }
    switch (type) {
    case SPELL_TYPE_SPELL:
      if ( IS_AFFECTED(tar_ch, AFF_BLIND) ){
	send_to_char("Nothing seems to happen.\n\r", ch);
	return;
      }
      spell_blindness(level,ch,tar_ch,0);
      break;
    case SPELL_TYPE_POTION:
      if ( IS_AFFECTED(ch, AFF_BLIND) )
	return;
      spell_blindness(level,ch,ch,0);
      break;
    case SPELL_TYPE_SCROLL:
      if (tar_obj) return;
      if (!tar_ch) tar_ch = ch;
      if ( IS_AFFECTED(tar_ch, AFF_BLIND) )
	return;
      spell_blindness(level,ch,tar_ch,0);
      break;
    case SPELL_TYPE_WAND:
      if (tar_obj) return;
      if (!tar_ch) tar_ch = ch;
      if ( IS_AFFECTED(tar_ch, AFF_BLIND) )
	return;
      spell_blindness(level,ch,tar_ch,0);
      break;
    case SPELL_TYPE_STAFF:
      for (tar_ch = world[ch->in_room].people ; 
	   tar_ch ; tar_ch = tar_ch->next_in_room)
	    if ( IS_NPC(tar_ch) ) 
	  if (!(IS_AFFECTED(tar_ch, AFF_BLIND)))
	spell_blindness(level,ch,tar_ch,0);
      break;
      default : 
	log("Serious screw-up in blindness!");
      break;
    }
}

void cast_control_weather( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
    char buffer[MAX_STRING_LENGTH];
    extern struct weather_data weather_info;

  switch (type) {
    case SPELL_TYPE_SPELL:

	    one_argument(arg,buffer);

	    if (str_cmp("better",buffer) && str_cmp("worse",buffer))
	    {
		send_to_char("Do you want it to get better or worse?\n\r",ch);
		return;
	    }

	    if(!str_cmp("better",buffer))
		weather_info.change+=(dice(((level)/3),4));
	    else
		weather_info.change-=(dice(((level)/3),4)); 
	    break;
      default : 
	 log("Serious screw-up in control weather!");
	 break;
    }
}



void cast_create_food( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{

  switch (type) {
    case SPELL_TYPE_SPELL:
	    act("$n magically creates a mushroom.",FALSE, ch, 0, 0, TO_ROOM);
	 spell_create_food(level,ch,0,0);
	    break;
    case SPELL_TYPE_SCROLL:
	 if(tar_obj) return;
	 if(tar_ch) return;
	 spell_create_food(level,ch,0,0);
	    break;
    default : 
	 log("Serious screw-up in create food!");
	 break;
    }
}



void cast_create_water( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    if (tar_obj->obj_flags.type_flag != ITEM_DRINKCON) {
      send_to_char("It is unable to hold water.\n\r", ch);
      return;
    }
    spell_create_water(level,ch,0,tar_obj);
    break;
    default : 
      log("Serious screw-up in create water!");
    break;
  }
}



void cast_cure_blind( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_cure_blind(level,ch,tar_ch,0);
    break;
  case SPELL_TYPE_POTION:
    spell_cure_blind(level,ch,ch,0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_cure_blind(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people ; 
     tar_ch ; tar_ch = tar_ch->next_in_room)

    spell_cure_blind(level,ch,tar_ch,0);
    break;
    default : 
      log("Serious screw-up in cure blind!");
    break;
  }
}



void cast_cure_critic( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_cure_critic(level,ch,tar_ch,0);
    break;
  case SPELL_TYPE_POTION:
    spell_cure_critic(level,ch,ch,0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_cure_critic(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people ; 
     tar_ch ; tar_ch = tar_ch->next_in_room)
      spell_cure_critic(level,ch,tar_ch,0);
    break;
    default : 
      log("Serious screw-up in cure critic!");
    break;

  }
}



void cast_cure_light( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_cure_light(level,ch,tar_ch,0);
    break;
  case SPELL_TYPE_POTION:
    spell_cure_light(level,ch,ch,0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_cure_light(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people ; 
     tar_ch ; tar_ch = tar_ch->next_in_room)
      spell_cure_light(level,ch,tar_ch,0);
    break;
    default : 
      log("Serious screw-up in cure light!");
    break;
  }
}


void cast_curse( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  if (IS_SET(world[ch->in_room].room_flags, SAFE)){
    send_to_char("You can not curse someone in a safe area!\n\r", ch);
    return;
  }
  
  switch (type) {
    case SPELL_TYPE_SPELL:
	    if (tar_obj)   /* It is an object */ 
		spell_curse(level,ch,0,tar_obj);
	    else {              /* Then it is a PC | NPC */
		spell_curse(level,ch,tar_ch,0);
	    }
	    break;
    case SPELL_TYPE_POTION:
	    spell_curse(level,ch,ch,0);
	    break;
    case SPELL_TYPE_SCROLL:
	    if (tar_obj)   /* It is an object */ 
		spell_curse(level,ch,0,tar_obj);
	    else {              /* Then it is a PC | NPC */
		if (!tar_ch) tar_ch = ch;
		spell_curse(level,ch,tar_ch,0);
	    }
	    break;
    case SPELL_TYPE_STAFF:
	 for (tar_ch = world[ch->in_room].people ; 
	      tar_ch ; tar_ch = tar_ch->next_in_room)
	    if ( IS_NPC(tar_ch) )
	       spell_curse(level,ch,tar_ch,0);
	 break;
    default : 
	 log("Serious screw-up in curse!");
	 break;
    }
}


void cast_detect_evil( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    if ( affected_by_spell(tar_ch, SPELL_DETECT_EVIL) ){
      send_to_char("Nothing seems to happen.\n\r", tar_ch);
      return;
    }
    spell_detect_evil(level,ch,tar_ch,0);
    break;
  case SPELL_TYPE_POTION:
    if ( affected_by_spell(ch, SPELL_DETECT_EVIL) )
      return;
    spell_detect_evil(level,ch,ch,0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    if ( affected_by_spell(tar_ch, SPELL_DETECT_EVIL))
      return;
    spell_detect_evil(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people ; 
     tar_ch ; tar_ch = tar_ch->next_in_room)

    if(!(IS_AFFECTED(tar_ch, SPELL_DETECT_EVIL)))
      spell_detect_evil(level,ch,tar_ch,0);
    break;
    default : 
      log("Serious screw-up in detect evil!");
    break;
  }
}



void cast_detect_invisibility( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    if ( affected_by_spell(tar_ch, SPELL_DETECT_INVISIBLE) ){
      send_to_char("Nothing seems to happen.\n\r", tar_ch);
      return;
    }
    spell_detect_invisibility(level,ch,tar_ch,0);
    break;
  case SPELL_TYPE_POTION:
    if ( affected_by_spell(ch, SPELL_DETECT_INVISIBLE) )
      return;
    spell_detect_invisibility(level,ch,ch,0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    if ( affected_by_spell(tar_ch, SPELL_DETECT_INVISIBLE) )
    spell_detect_invisibility(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people ; 
     tar_ch ; tar_ch = tar_ch->next_in_room)

    if(!(IS_AFFECTED(tar_ch, SPELL_DETECT_INVISIBLE)))
      spell_detect_invisibility(level,ch,tar_ch,0);
    break;
    default : 
      log("Serious screw-up in detect invisibility!");
    break;
  }
}



void cast_detect_magic( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    if ( affected_by_spell(tar_ch, SPELL_DETECT_MAGIC) ){
      send_to_char("Nothing seems to happen.\n\r", tar_ch);
      return;
    }
    spell_detect_magic(level,ch,tar_ch,0);
    break;
  case SPELL_TYPE_POTION:
    if ( affected_by_spell(ch, SPELL_DETECT_MAGIC) )
      return;
    spell_detect_magic(level,ch,ch,0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_detect_magic(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people ; 
     tar_ch ; tar_ch = tar_ch->next_in_room)

    if (!(IS_AFFECTED(tar_ch, SPELL_DETECT_MAGIC)))
      spell_detect_magic(level,ch,tar_ch,0);
    break;
    default : 
      log("Serious screw-up in detect magic!");
    break;
  }
}



void cast_detect_poison( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_detect_poison(level, ch, tar_ch,tar_obj);
    break;
  case SPELL_TYPE_POTION:
    spell_detect_poison(level, ch, ch,0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) {
      spell_detect_poison(level, ch, 0, tar_obj);
      return;
    }
    if (!tar_ch) tar_ch = ch;
    spell_detect_poison(level, ch, tar_ch, 0);
    break;
    default : 
      log("Serious screw-up in detect poison!");
    break;
  }
}



void cast_dispel_evil( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_dispel_evil(level, ch, tar_ch,0);
    break;
  case SPELL_TYPE_POTION:
    spell_dispel_evil(level,ch,ch,0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_dispel_evil(level, ch, tar_ch,0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    spell_dispel_evil(level, ch, tar_ch,0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people ; 
     tar_ch ; tar_ch = tar_ch->next_in_room)
      if ( IS_NPC(tar_ch) )
    spell_dispel_evil(level,ch,tar_ch,0);
    break;
    default : 
      log("Serious screw-up in dispel evil!");
    break;
  }
}


void cast_enchant_weapon( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_enchant_weapon(level, ch, 0,tar_obj);
	    break;

    case SPELL_TYPE_SCROLL:
	    if(!tar_obj) return;
	    spell_enchant_weapon(level, ch, 0,tar_obj);
	    break;
    default : 
      log("Serious screw-up in enchant weapon!");
      break;
    }
}


void cast_heal( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    act("$n heals $N.", FALSE, ch, 0, tar_ch, TO_NOTVICT);
	    act("You heal $N.", FALSE, ch, 0, tar_ch, TO_CHAR);
	    spell_heal(level, ch, tar_ch, 0);
	    break;
    case SPELL_TYPE_POTION:
	 spell_heal(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_STAFF:
	 for (tar_ch = world[ch->in_room].people ; 
	      tar_ch ; tar_ch = tar_ch->next_in_room)
       spell_heal(level,ch,tar_ch,0);
	 break;
    default : 
	 log("Serious screw-up in heal!");
	 break;
    }
}


void cast_invisibility( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    if (tar_obj) {
      if ( IS_SET(tar_obj->obj_flags.extra_flags, ITEM_INVISIBLE) )
    send_to_char("Nothing new seems to happen.\n\r", ch);
      else
    spell_invisibility(level, ch, 0, tar_obj);
    } else { /* tar_ch */
      if ( IS_AFFECTED(tar_ch, AFF_INVISIBLE) )
    send_to_char("Nothing new seems to happen.\n\r", ch);
      else
    spell_invisibility(level, ch, tar_ch, 0);
    }
    break;
  case SPELL_TYPE_POTION:
    if (!IS_AFFECTED(ch, AFF_INVISIBLE) )
      spell_invisibility(level, ch, ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) {
      if (!(IS_SET(tar_obj->obj_flags.extra_flags, ITEM_INVISIBLE)) )
    spell_invisibility(level, ch, 0, tar_obj);
    } else { /* tar_ch */
      if (!tar_ch) tar_ch = ch;
      
      if (!( IS_AFFECTED(tar_ch, AFF_INVISIBLE)) )
    spell_invisibility(level, ch, tar_ch, 0);
    }
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) {
      if (!(IS_SET(tar_obj->obj_flags.extra_flags, ITEM_INVISIBLE)) )
    spell_invisibility(level, ch, 0, tar_obj);
    } else { /* tar_ch */
      if (!( IS_AFFECTED(tar_ch, AFF_INVISIBLE)) )
    spell_invisibility(level, ch, tar_ch, 0);
    }
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people ; 
     tar_ch ; tar_ch = tar_ch->next_in_room)

    if (!( IS_AFFECTED(tar_ch, AFF_INVISIBLE)) )
      spell_invisibility(level,ch,tar_ch,0);
    break;
    default : 
      log("Serious screw-up in invisibility!");
    break;
  }
}




void cast_locate_object( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_locate_object(level, ch, 0, tar_obj);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_ch) return;
    spell_locate_object(level, ch, 0, tar_obj);
    break;
    default : 
      log("Serious screw-up in locate object!");
    break;
  }
}


void cast_poison( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  if (IS_SET(world[ch->in_room].room_flags, SAFE)){
    send_to_char("You can not poison someone in a safe area!\n\r",ch);
    return;
  }
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_poison(level, ch, tar_ch, tar_obj);
	    break;
    case SPELL_TYPE_POTION:
	    spell_poison(level, ch, ch, 0);
	    break;
    case SPELL_TYPE_STAFF:
	 for (tar_ch = world[ch->in_room].people ; 
	      tar_ch ; tar_ch = tar_ch->next_in_room)
	    if ( IS_NPC(tar_ch) )
		  spell_poison(level,ch,tar_ch,0);
	 break;
    default : 
	 log("Serious screw-up in poison!");
	 break;
    }
}


void cast_protection_from_evil( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_protection_from_evil(level, ch, tar_ch, 0);
	    break;
    case SPELL_TYPE_POTION:
	 spell_protection_from_evil(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_SCROLL:
	 if(tar_obj) return;
	 if(!tar_ch) tar_ch = ch;
	    spell_protection_from_evil(level, ch, tar_ch, 0);
	    break;
    case SPELL_TYPE_STAFF:
	 for (tar_ch = world[ch->in_room].people ; 
	      tar_ch ; tar_ch = tar_ch->next_in_room)

		  spell_protection_from_evil(level,ch,tar_ch,0);
	 break;
    default : 
	 log("Serious screw-up in protection from evil!");
	 break;
    }
}


void cast_remove_curse( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_remove_curse(level, ch, tar_ch, tar_obj);
	    break;
    case SPELL_TYPE_POTION:
	 spell_remove_curse(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_SCROLL:
	 if(tar_obj) {
		spell_remove_curse(level, ch, 0, tar_obj);
		return;
	    }
	 if(!tar_ch) tar_ch = ch;
	    spell_remove_curse(level, ch, tar_ch, 0);
	    break;
    case SPELL_TYPE_STAFF:
	 for (tar_ch = world[ch->in_room].people ; 
	      tar_ch ; tar_ch = tar_ch->next_in_room)

		  spell_remove_curse(level,ch,tar_ch,0);
	 break;
    default : 
	 log("Serious screw-up in remove curse!");
	 break;
    }
}



void cast_remove_poison( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_remove_poison(level, ch, tar_ch, tar_obj);
	    break;
    case SPELL_TYPE_POTION:
	 spell_remove_poison(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_STAFF:
	 for (tar_ch = world[ch->in_room].people ; 
	      tar_ch ; tar_ch = tar_ch->next_in_room)

		  spell_remove_poison(level,ch,tar_ch,0);
	 break;
    default : 
	 log("Serious screw-up in remove poison!");
	 break;
    }
}



void cast_sanctuary( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_sanctuary(level, ch, tar_ch, 0);
	    break;
    case SPELL_TYPE_POTION:
	 spell_sanctuary(level, ch, ch, 0);
	 break;
    case SPELL_TYPE_SCROLL:
	 if(tar_obj)
		return;
	 if(!tar_ch) tar_ch = ch;
	    spell_sanctuary(level, ch, tar_ch, 0);
	    break;
    case SPELL_TYPE_STAFF:
	 for (tar_ch = world[ch->in_room].people ; 
	      tar_ch ; tar_ch = tar_ch->next_in_room)

		  spell_sanctuary(level,ch,tar_ch,0);
	 break;
    default : 
	 log("Serious screw-up in sanctuary!");
	 break;
    }
}


void cast_sleep( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  if (IS_SET(world[ch->in_room].room_flags, SAFE)){
    send_to_char("You can not sleep someone in a safe area!\n\r", ch);
    return;
  }
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_sleep(level, ch, tar_ch, 0);
	    break;
    case SPELL_TYPE_POTION:
	    spell_sleep(level, ch, ch, 0);
	    break;
    case SPELL_TYPE_SCROLL:
	 if(tar_obj) return;
	 if (!tar_ch) tar_ch = ch;
	 spell_sleep(level, ch, tar_ch, 0);
	 break;
    case SPELL_TYPE_WAND:
	 if(tar_obj) return;
	 spell_sleep(level, ch, tar_ch, 0);
	 break;
    case SPELL_TYPE_STAFF:
	 for (tar_ch = world[ch->in_room].people ; 
	      tar_ch ; tar_ch = tar_ch->next_in_room)
	    if ( IS_NPC(tar_ch) )
		  spell_sleep(level,ch,tar_ch,0);
	 break;
    default : 
	 log("Serious screw-up in sleep!");
	 break;
    }
}


void cast_strength( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_strength(level, ch, tar_ch, 0);
	    break;
    case SPELL_TYPE_POTION:
	    spell_strength(level, ch, ch, 0);
	    break;
    case SPELL_TYPE_SCROLL:
	 if(tar_obj) return;
	 if (!tar_ch) tar_ch = ch;
	 spell_strength(level, ch, tar_ch, 0);
	 break;
    case SPELL_TYPE_STAFF:
	 for (tar_ch = world[ch->in_room].people ; 
	      tar_ch ; tar_ch = tar_ch->next_in_room)

		  spell_strength(level,ch,tar_ch,0);
	 break;
    default : 
	 log("Serious screw-up in strength!");
	 break;
    }
}


void cast_ventriloquate( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
    struct char_data *tmp_ch;
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];

    if (type != SPELL_TYPE_SPELL) {
	log("Attempt to ventriloquate by non-cast-spell.");
	return;
    }
    for(; *arg && (*arg == ' '); arg++);
    if (tar_obj) {
	sprintf(buf1, "The %s says '%s'\n\r", fname(tar_obj->name), arg);
	sprintf(buf2, "Someone makes it sound like the %s says '%s'.\n\r",
	  fname(tar_obj->name), arg);
    }   else {
	sprintf(buf1, "%s says '%s'\n\r", GET_NAME(tar_ch), arg);
	sprintf(buf2, "Someone makes it sound like %s says '%s'\n\r",
	  GET_NAME(tar_ch), arg);
    }

    sprintf(buf3, "Someone says, '%s'\n\r", arg);

    for (tmp_ch = world[ch->in_room].people; tmp_ch;
      tmp_ch = tmp_ch->next_in_room) {

	if ((tmp_ch != ch) && (tmp_ch != tar_ch)) {
	    if ( saves_spell(tmp_ch, SAVING_SPELL) )
		send_to_char(buf2, tmp_ch);
	    else
		send_to_char(buf1, tmp_ch);
	} else {
	    if (tmp_ch == tar_ch)
		send_to_char(buf3, tar_ch);
	}
    }
}



void cast_word_of_recall( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  struct char_data *tar_ch_next;

  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_word_of_recall(level, ch, ch, 0);
	    break;
    case SPELL_TYPE_POTION:
	    spell_word_of_recall(level, ch, ch, 0);
	    break;
    case SPELL_TYPE_SCROLL:
	 if(tar_obj) return;
	 if (!tar_ch) tar_ch = ch;
	 spell_word_of_recall(level, ch, tar_ch, 0);
	 break;
    case SPELL_TYPE_WAND:
	 if(tar_obj) return;
	 spell_word_of_recall(level, ch, tar_ch, 0);
	 break;
    case SPELL_TYPE_STAFF:
	for (tar_ch = world[ch->in_room].people ; 
	    tar_ch ; tar_ch = tar_ch_next)
	{
	    tar_ch_next = tar_ch->next_in_room;
	    if ( !IS_NPC(tar_ch) )
		spell_word_of_recall(level,ch,tar_ch,0);
	}
	break;
    default : 
	 log("Serious screw-up in word of recall!");
	 break;
    }
}



void cast_summon( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_summon(level, ch, tar_ch, 0);
	    break;
      default : 
	 log("Serious screw-up in summon!");
	 break;
    }
}



void cast_charm_person( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
	case SPELL_TYPE_SPELL:
	    spell_charm_person(level, ch, tar_ch, 0);
	    break;
      case SPELL_TYPE_SCROLL:
	 if(!tar_ch) return;
	 spell_charm_person(level, ch, tar_ch, 0);
	 break;
      case SPELL_TYPE_STAFF:
	 for (tar_ch = world[ch->in_room].people ; 
	      tar_ch ; tar_ch = tar_ch->next_in_room)
	    if ( IS_NPC(tar_ch) )
		  spell_charm_person(level,ch,tar_ch,0);
	 break;
      default : 
	 log("Serious screw-up in charm person!");
	 break;
    }
}



void cast_sense_life( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_sense_life(level, ch, ch, 0);
    break;
  case SPELL_TYPE_POTION:
    spell_sense_life(level, ch, ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    spell_sense_life(level, ch, ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people ; 
     tar_ch ; tar_ch = tar_ch->next_in_room)

    spell_sense_life(level,ch,tar_ch,0);
    break;
    default : 
      log("Serious screw-up in sense life!");
    break;
  }
}


void cast_identify( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_identify(level, ch, tar_ch, tar_obj);
    break;
  case SPELL_TYPE_SCROLL:
    spell_identify(level, ch, tar_ch, tar_obj);
    break;
  default:
      log("Serious screw-up in identify!");
    break;
    }
}


void cast_fire_breath( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_fire_breath(level, ch, tar_ch, 0);
	    break;   /* It's a spell.. But people can't cast it! */
      default : 
	 log("Serious screw-up in firebreath!");
	 break;
    }
}

void cast_frost_breath( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_frost_breath(level, ch, tar_ch, 0);
	    break;   /* It's a spell.. But people can't cast it! */
      default : 
	 log("Serious screw-up in frostbreath!");
	 break;
    }
}

void cast_acid_breath( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_acid_breath(level, ch, tar_ch, 0);
	    break;   /* It's a spell.. But people can't cast it! */
      default : 
	 log("Serious screw-up in acidbreath!");
	 break;
    }
}

void cast_gas_breath( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	for (tar_ch = world[ch->in_room].people ; 
		tar_ch ; tar_ch = tar_ch->next_in_room)
	    if ( !IS_NPC(tar_ch) )
		spell_gas_breath(level,ch,tar_ch,0);
	 break;
	    /* THIS ONE HURTS!! */
      default : 
	 log("Serious screw-up in gasbreath!");
	 break;
    }
}

void cast_lightning_breath( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
	    spell_lightning_breath(level, ch, tar_ch, 0);
	    break;   /* It's a spell.. But people can't cast it! */
      default : 
	 log("Serious screw-up in lightningbreath!");
	 break;
    }
}

void cast_fear( byte level, struct char_data *ch, char *arg, int type,
	   struct char_data *tar_ch, struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_fear(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_fear(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_fear(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
     tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch))
    spell_fear(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in fear!");
    break;
  }
}

void cast_refresh( byte level, struct char_data *ch, char *arg, int type,
	  struct char_data *tar_ch, struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_refresh(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_refresh(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_refresh(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)

	spell_refresh(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in refresh!");
    break;
  }
}

void cast_fly( byte level, struct char_data *ch, char *arg, int type,
	  struct char_data *tar_ch, struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_fly(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_fly(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_fly(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)

	spell_fly(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in fly!");
    break;
  }
}

void cast_cont_light( byte level, struct char_data *ch, char *arg, int type,
	     struct char_data *tar_ch, struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_cont_light(level, ch, 0, 0);
    break;
  case SPELL_TYPE_SCROLL:
    spell_cont_light(level, ch, 0, 0);
    break;
  default:
    log("Serious screw-up in cont_light");
    break;
  }
}

void cast_know_alignment(byte level, struct char_data *ch, char *arg, int type,
	     struct char_data *tar_ch, struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_know_alignment(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_know_alignment(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_know_alignment(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      spell_know_alignment(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in know alignment!");
    break;
  }
}

void cast_dispel_magic( byte level, struct char_data *ch, char *arg, int type,
	       struct char_data *tar_ch, struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_dispel_magic(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_dispel_magic(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_dispel_magic(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      spell_dispel_magic(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in dispel magic!");
    break;
  }
}

void cast_conjure_elemental( byte level, struct char_data *ch,
		char *arg, int type,
		struct char_data *tar_ch, struct obj_data *tar_obj)
{
  /* not working yet! */
}

void cast_cure_serious( byte level, struct char_data *ch, char *arg, int type,
	       struct char_data *tar_ch, struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_cure_serious(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_cure_serious(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_cure_serious(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      spell_cure_serious(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in cure serious!");
    break;
  }
}

void cast_cause_light( byte level, struct char_data *ch, char *arg, int type,
	      struct char_data *tar_ch, struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_cause_light(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_cause_light(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_cause_light(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch))
	spell_cause_light(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in cause light!");
    break;
  }
}

void cast_cause_critical( byte level, struct char_data *ch, char *arg,
	     int type, struct char_data *tar_ch,
	     struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_cause_critical(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_cause_critical(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_cause_critical(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch))
	spell_cause_critical(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in cause critical!");
    break;
  }
}

void cast_cause_serious( byte level, struct char_data *ch, char *arg,
	    int type, struct char_data *tar_ch,
	    struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_cause_serious(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_cause_serious(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_cause_serious(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch))
	spell_cause_serious(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in cause serious!");
    break;
  }
}

void cast_flamestrike( byte level, struct char_data *ch, char *arg,
	      int type, struct char_data *tar_ch,
	      struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_flamestrike(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_flamestrike(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_flamestrike(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch))
	spell_flamestrike(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in cause critical!");
    break;
  }
}

void cast_stone_skin( byte level, struct char_data *ch, char *arg,
	     int type, struct char_data *tar_ch,
	     struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_stone_skin(level, ch, 0, 0);
    break;
  case SPELL_TYPE_POTION:
    spell_stone_skin(level, ch, 0, 0);
    break;
  case SPELL_TYPE_SCROLL:
    spell_stone_skin(level, ch, 0, 0);
    break;
  default:
    log("Serious screw-up in stone skin!");
    break;
  }
}

void cast_shield( byte level, struct char_data *ch, char *arg,
	 int type, struct char_data *tar_ch,
	 struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_shield(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_shield(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_shield(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      spell_shield(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in shield!");
    break;
  }
}

void cast_weaken( byte level, struct char_data *ch, char *arg,
	 int type, struct char_data *tar_ch,
	 struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_weaken(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_weaken(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_weaken(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch))
	spell_weaken(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in weaken!");
    break;
  }
}

void cast_mass_invis( byte level, struct char_data *ch, char *arg,
	     int type, struct char_data *tar_ch,
	     struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_mass_invis(level, ch, 0, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    spell_mass_invis(level, ch, 0, 0);
    break;
  case SPELL_TYPE_WAND:
    spell_mass_invis(level, ch, 0, 0);
    break;
  default:
    log("Serious screw-up in mass invis!");
    break;
  }
}

void cast_acid_blast( byte level, struct char_data *ch, char *arg,
	     int type, struct char_data *tar_ch,
	     struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_acid_blast(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_acid_blast(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_acid_blast(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch))
	spell_acid_blast(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in acid blast!");
    break;
  }
}

void cast_gate( byte level, struct char_data *ch, char *arg,
	   int type, struct char_data *tar_ch,
	   struct obj_data *tar_obj)
{
  /* gate needs to be fixed!  */
}

void cast_faerie_fire( byte level, struct char_data *ch, char *arg,
	      int type, struct char_data *tar_ch,
	      struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_faerie_fire(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_faerie_fire(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_faerie_fire(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch))
      spell_faerie_fire(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in faerie fire!");
    break;
  }
}

void cast_faerie_fog( byte level, struct char_data *ch, char *arg,
	     int type, struct char_data *tar_ch,
	     struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_faerie_fog(level, ch, 0, 0);
    break;
  case SPELL_TYPE_SCROLL:
    spell_faerie_fog(level, ch, 0, 0);
    break;
  case SPELL_TYPE_STAFF:
    spell_faerie_fog(level, ch, 0, 0);
    break;
  default:
    log("Serious screw-up in faerie fog!");
    break;
  }
}

void cast_drown( byte level, struct char_data *ch, char *arg,
	int type, struct char_data *tar_ch,
	struct obj_data *tar_obj)
{
  /*  Does not work yet  */
}

void cast_demonfire( byte level, struct char_data *ch, char *arg,
	    int type, struct char_data *tar_ch,
	    struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_demonfire(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_demonfire(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_demonfire(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch))
	spell_demonfire(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in demon fire!");
    break;
  }
}

void cast_turn_undead( byte level, struct char_data *ch, char *arg,
	      int type, struct char_data *tar_ch,
	      struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_turn_undead(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_turn_undead(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_turn_undead(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch) && IS_SET(tar_ch->specials.act, ACT_UNDEAD))
	spell_turn_undead(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in turn undead!");
    break;
  }
}

void cast_infravision( byte level, struct char_data *ch, char *arg,
	      int type, struct char_data *tar_ch,
	      struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_infravision(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_infravision(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)

	spell_infravision(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in infravision!");
    break;
  }
}

void cast_sandstorm( byte level, struct char_data *ch, char *arg,
	    int type, struct char_data *tar_ch,
	    struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_sandstorm(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_sandstorm(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_sandstorm(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch))
	spell_sandstorm(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in sandstorm!");
    break;
  }
}

void cast_hands_of_wind( byte level, struct char_data *ch, char *arg,
	    int type, struct char_data *tar_ch,
	    struct obj_data *tar_obj)
{
  /* have to fix */
}

void cast_plague( byte level, struct char_data *ch, char *arg,
	 int type, struct char_data *tar_ch,
	 struct obj_data *tar_obj)
{
  switch (type) {
  case SPELL_TYPE_SPELL:
    spell_plague(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_SCROLL:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_plague(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_WAND:
    if (tar_obj) return;
    if (!tar_ch) tar_ch = ch;
    spell_plague(level, ch, tar_ch, 0);
    break;
  case SPELL_TYPE_STAFF:
    for (tar_ch = world[ch->in_room].people;
	 tar_ch; tar_ch = tar_ch->next_in_room)
      if (IS_NPC(tar_ch))
	spell_plague(level, ch, tar_ch, 0);
    break;
  default:
    log("Serious screw-up in cause critical!");
    break;
  }
}


#if 0
void cast_animate_dead( byte level, struct char_data *ch, char *arg, int type,
  struct char_data *tar_ch, struct obj_data *tar_obj )
{
  switch (type) {
    case SPELL_TYPE_SPELL:
			spell_animate_dead(level, ch, 0,tar_obj);
			break;

    case SPELL_TYPE_SCROLL:
			if(!tar_obj) return;
			spell_animate_dead(level, ch, 0,tar_obj);
			break;
    default : 
      log("Serious screw-up in Animate Dead!");
      break;
	}
}
#endif
