/***************************************************************************
 *  File: fight.c , Combat module.                         Part of DIKUMUD *
 *  Usage: Combat system and messages.                                     *
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
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "utils.h"
#include "handler.h"
#include "interp.h"
#include "db.h"
#include "spells.h"

struct char_data *combat_list       = NULL;
struct char_data *combat_next_dude  = NULL;

extern struct room_data *world;
extern struct message_list fight_messages[MAX_MESSAGES];
extern struct obj_data  *object_list;

int cleric(struct char_data *ch, int cmd, char *arg);
int magic_user(struct char_data *ch, int cmd,char *arg);
int mana_cost(struct char_data *ch, int level, int min_cost);
void cast_dispel_magic( byte level, struct char_data *ch, char *arg, int si, struct char_data *tar_ch, struct obj_data *tar_obj);
void cast_cure_blind( byte level, struct char_data *ch, char *arg, int si, struct char_data *tar_ch, struct obj_data *tar_obj);

void stop_follower(struct char_data *ch);
void hit(struct char_data *ch, struct char_data *victim, int type);
void mob_hit(struct char_data *ch, struct char_data *victim, int type);
bool is_in_safe(struct char_data *ch, struct char_data *victim);
bool is_first_level(struct char_data *ch, struct char_data *victim);
void check_assist(struct char_data *ch);

void dam_message(int dam, struct char_data *ch, struct char_data *victim,
		 int w_type);
void group_gain( struct char_data *ch, struct char_data *victim );
bool check_parry( struct char_data *ch, struct char_data *victim );
bool check_dodge( struct char_data *ch, struct char_data *victim );
void disarm( struct char_data *ch, struct char_data *victim );
void trip( struct char_data *ch, struct char_data *victim );
void kick (struct char_data *ch, struct char_data *victim);
void bash (struct char_data *ch, struct char_data *victim);

/*
 * Control the fights going on.
 * Called periodically by the main loop.
 */
void perform_violence( void )
{
    struct char_data *ch;

    for (ch = combat_list; ch; ch = combat_next_dude )
    {
	combat_next_dude = ch->next_fighting;
	assert( ch->specials.fighting );

	if ( AWAKE(ch) && ch->in_room == ch->specials.fighting->in_room )
        {
	    hit( ch, ch->specials.fighting, TYPE_UNDEFINED );
            if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_GROUP)) 
	      check_assist(ch);
        }
	else
	    stop_fighting( ch );
    }
}
 
/* check to see if group members should help out */
 
void check_assist (struct char_data *ch)
{
  struct char_data *k;
  struct follow_type *f;
 
 
  if (!(k=ch->master))
    k = ch;
 
  if (GET_POS(k) == POSITION_STANDING && IS_AFFECTED(k,AFF_GROUP) &&
      k->in_room == ch->in_room && IS_SET(k->specials.act,PLR_AUTOASSIST))
    do_assist(k,GET_NAME(ch),0);
 
  for (f=k->followers; f; f=f->next)
  {
    if (GET_POS(f->follower) == POSITION_STANDING && ch != f->follower &&
	IS_AFFECTED(f->follower,AFF_GROUP) &&
	f->follower->in_room == ch->in_room &&
	IS_SET(f->follower->specials.act,PLR_AUTOASSIST))
      do_assist(f->follower,GET_NAME(ch),0);
   }
}


/*
 * Do one group of attacks.
 */
void hit( struct char_data *ch, struct char_data *victim, int type )
{
    int     chance;

    /* start attack series, switch out if it is a mobile */

    if (IS_NPC(ch))
    {
      mob_hit(ch,victim,type);
      return;
    }

    one_hit( ch, victim, type );

     /* if is hasted */
    if (IS_AFFECTED(ch,AFF_HASTE)) 
      one_hit(ch, victim, type );

    if (type == SKILL_BACKSTAB) return;
     /* RT second attack for thief */
     if (!IS_NPC(ch) && (GET_CLASS(ch) == CLASS_THIEF))
     {
	chance =  
	  ch->skills[SKILL_SECOND_ATTACK].learned * 2 /5 + GET_LEVEL(ch);
	if ( number(1,100) < chance )
        { 
	    one_hit( ch, victim, type );
	    check_improve(ch,SKILL_SECOND_ATTACK,5,TRUE);
	}
	return;
      }

     if (!IS_NPC(ch) && (GET_CLASS(ch) != CLASS_WARRIOR))
	return;

    /* second attack */
    chance  = ch->skills[SKILL_SECOND_ATTACK].learned * 2/5 + 2 * GET_LEVEL(ch);
    if ( number(1, 100) < chance )
    {
	one_hit( ch, victim, type );
	check_improve(ch,SKILL_SECOND_ATTACK,4,TRUE);
    

      /* Third attack */
      chance  = ch->skills[SKILL_THIRD_ATTACK].learned * 1/5 + GET_LEVEL(ch); 
      if ( number(1, 100) < chance )
      {
	one_hit( ch, victim, type );
	check_improve(ch,SKILL_THIRD_ATTACK,4,TRUE);
      }
    }
    
}


/* handles mob attacks and spells and specials skills as well */

void mob_hit (struct char_data *ch, struct char_data *victim, int type)
{
  int chance,roll;
  struct char_data *tmp_vict;

  one_hit(ch,victim,type);  /* 1st attack */
  
  if (IS_SET(ch->specials.act,ACT_FAST) || IS_AFFECTED(ch, AFF_HASTE))
    /* 2nd attack if fast */

    one_hit(ch,victim,type);

  /* extra attacks possible for fighter mobs */

  if (IS_SET(ch->specials.act,ACT_WARRIOR))
  {
    chance = 3 * GET_LEVEL(ch);
    roll = number(1,100);
    if (roll < chance)
      one_hit(ch,victim,type);

    roll = number(1,100);
    chance /= 2;
    if (roll < chance)  
      one_hit(ch,victim,type);
  }

  /* Area attack -- BALLS nasty! */
  
  if (IS_SET(ch->specials.act,ACT_AREA_ATTACK))
  {
    for (tmp_vict = world[ch->in_room].people;
         tmp_vict;
         tmp_vict = tmp_vict->next_in_room)
    {
      if (tmp_vict->specials.fighting)
	if (tmp_vict != victim && tmp_vict->specials.fighting == ch)
	  one_hit(ch,tmp_vict,type);
    }
  } 
  /* start special attacks */

  if (ch->specials.stun_time > 0)
  {
    ch->specials.stun_time--;
    return;
  }

  if (IS_SET(ch->specials.act,ACT_WARRIOR) && number(0,1) == 0)
  /* do some warrior stuff */

  switch (number(0,2)) 
  {
    case 0 : disarm(ch,victim);
             break;
    case 1 : bash(ch,victim);
    	     break;
    case 2:  kick (ch,victim);
             break;
  }

  if (IS_SET(ch->specials.act,ACT_THIEF) && number(0,1) == 0)
    switch (number(0,1))
  {
    case 0 : disarm(ch,victim);
             break;
    case 1 : trip(ch,victim);
  	     break;
  }

  if (IS_SET(ch->specials.act,ACT_MAGE) && IS_SET(ch->specials.act,ACT_CLERIC))
  {
    if (number(0,1) == 0 && GET_MANA(ch) > 0)
      return;
    if (affected_by_spell(ch,SPELL_BLINDNESS))
    {
      if (GET_LEVEL(ch) > 3)
      {
	act("$n utters the words, 'judicandus nose'",0,ch,0,0,TO_ROOM);
        cast_cure_blind(GET_LEVEL(ch),ch,"",SPELL_TYPE_SPELL,ch,0); 
        GET_MANA(ch) -= mana_cost(ch,4,5);
      }
      return;
    }

    switch (number(0,1))
    {
      case 0 : cleric(ch,0,"");
	       break;
      case 1 : magic_user(ch,0,"");
 	       break;
    }
    return;
  }

  if (IS_SET(ch->specials.act,ACT_CLERIC) && GET_MANA(ch) > 0 &&
      number(0,1) == 0)
  {
    if (affected_by_spell(ch,SPELL_BLINDNESS))
    {
      if (GET_LEVEL(ch) > 3)
      {
        act("$n utters the words, 'judicandus nose'",0,ch,0,0,TO_ROOM);
        cast_cure_blind(GET_LEVEL(ch),ch,"",SPELL_TYPE_SPELL,ch,0);
        GET_MANA(ch) -= mana_cost(ch,4,5);
       }
      return;
    }

    cleric(ch,0,"");
    return;
  }

  if (IS_SET(ch->specials.act, ACT_MAGE) && GET_MANA(ch) > 0 &&
      number(0,1) == 0)
  {
    if (affected_by_spell(ch,SPELL_BLINDNESS))
    {
      if (GET_LEVEL(ch) > 10)
      {
        act("$n utters the words, 'sin magicum'",0,ch,0,0,TO_ROOM);
	cast_dispel_magic(GET_LEVEL(ch)+10,ch,"",SPELL_TYPE_SPELL,ch,0);
        GET_MANA(ch) = mana_cost(ch,11,15);
      }
      return;
    }
 
    magic_user(ch,0,"");
    return;
  }
}

 /* Hit one guy once. */

void one_hit( struct char_data *ch, struct char_data *victim, int type )
{
    struct obj_data *wielded;
    int w_type;
    int victim_ac, calc_thaco;
    int dam;
    byte diceroll;

    extern int thaco[4][36];
    extern byte backstab_mult[];
    extern struct str_app_type str_app[];
    extern struct dex_app_type dex_app[];

    /*
     * Can't beat a dead char!
     */
    if ( GET_POS(victim) == POSITION_DEAD )
	return;

    /*
     * This happens when people flee et cetera during multi attacks.
     */
    if ( ch->in_room != victim->in_room )
	return;
      
    /*
     * Figure out the type of damage message.
     */
    wielded = ch->equipment[WIELD];
    w_type  = TYPE_HIT;
    if ( wielded && wielded->obj_flags.type_flag == ITEM_WEAPON )
    {
	switch ( wielded->obj_flags.value[3] )
	{
	case 0: case 1: case 2:             w_type = TYPE_WHIP;     break;
	case 3:                             w_type = TYPE_SLASH;    break;
	case 4: case 5: case 6:             w_type = TYPE_CRUSH;    break;
	case 7:                             w_type = TYPE_BLUDGEON; break;
	case 8: case 9: case 10: case 11:   w_type = TYPE_PIERCE;   break;
	}
    }
    if ( type == SKILL_BACKSTAB )
	w_type  = SKILL_BACKSTAB;

    /*
     * Calculate to-hit-armor-class-0 versus armor class.
     * thaco for monsters is set in hitroll.
     */
    if ( !IS_NPC(ch) )
	calc_thaco  = thaco[(int) GET_CLASS(ch)-1][(int) GET_LEVEL(ch)];
    else
	calc_thaco  = 20;
    
    calc_thaco  -= str_app[STRENGTH_APPLY_INDEX(ch)].tohit;
    calc_thaco  -= GET_HITROLL(ch);

    victim_ac   = GET_AC(victim)/10;
    if ( AWAKE(victim) )
	victim_ac += dex_app[GET_DEX(victim)].defensive;
    victim_ac   = MAX( -10, victim_ac );

    /*
     * The moment of excitement!
     */
    diceroll    = number(1, 20);

    if ( diceroll < 20 && AWAKE(victim)
    &&  (diceroll == 1 || diceroll < calc_thaco - victim_ac) )
    {
	/* Miss. */
	damage( ch, victim, 0, w_type );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( wielded )
	dam = dice( wielded->obj_flags.value[1], wielded->obj_flags.value[2] );
    else if ( IS_NPC(ch) )
	dam = dice( ch->specials.damnodice, ch->specials.damsizedice );
    else
	dam = number( 0, 2 );

    /*
     * Bonuses.
     */
    dam += str_app[STRENGTH_APPLY_INDEX(ch)].todam;
    dam += GET_DAMROLL(ch);

    /*
     * Bonus for hitting weak opponents.
     * Bonus for backstabbing.
     */
    if ( GET_POS(victim) < POSITION_FIGHTING )
	dam *= 1 + ( 2 * ( POSITION_FIGHTING - GET_POS(victim) ) ) / 3;

    if ( type == SKILL_BACKSTAB )
      {
	dam *= backstab_mult[(int) GET_LEVEL(ch)];
	type = TYPE_UNDEFINED;    /* Prevents multi backstabbing/combat */
	                          /*                            -Kahn   */
      }

    /* RT code for modifying damage by class */
    if (!IS_NPC(ch))
    {
       
	switch (GET_CLASS(ch))
	{
	  case CLASS_MAGIC_USER:  dam = (dam * 7) / 10; break;
	  case CLASS_CLERIC:      dam = (dam * 8) / 10; break;
	  case CLASS_THIEF:	  break;  /* no change */
  	  case CLASS_WARRIOR:     dam = (dam * 12) / 10; break; 
	  default:  break; 
         }
    }

    if ( dam < 1 )
	dam = 1;

    damage( ch, victim, dam, w_type );
    return;
}



/*
 * Inflict damage from a hit.
 */
void damage( struct char_data *ch, struct char_data *victim,
	    int dam, int attacktype )
{
    struct message_type *messages;
    int i, j, nr, max_hit;
    struct obj_data *corpse;

    int hit_limit(struct char_data *ch);

    if ( GET_POS(victim) == POSITION_DEAD )
	return;

    /*
     * Can't hurt god, but he likes to see the messages.
     */
    if ( GET_LEVEL(victim) >= 32 && !IS_NPC(victim) )
	dam = 0;

    /* code to prevent pkilling */

    if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_KILLER))
	dam = 0;


    /* shortcut code for invulnerable shopkeepers */

    if (IS_NPC(victim) &&
        IS_SET(victim->specials.act,ACT_NOWEAPON) &&
	IS_SET(victim->specials.act,ACT_NOMAGIC))
      dam = 0;

    /*
     * Certain attacks are forbidden.
     */
    if ( victim != ch )
    {
	if ( is_in_safe( ch, victim ) )
	    return;
	if ( is_first_level( ch, victim ) )
	    return;
	check_killer( ch, victim );
    }

    /*
     * An eye for an eye, a tooth for a tooth, a life for a life.
     */
    if ( GET_POS(victim) > POSITION_STUNNED && ch != victim )
    {
	if ( !victim->specials.fighting )
	    set_fighting( victim, ch );
	GET_POS(victim) = POSITION_FIGHTING;
    }

    if ( GET_POS(ch) > POSITION_STUNNED && ch != victim )
    {
	if ( !ch->specials.fighting )
	    set_fighting( ch, victim );

	/*
	 * If victim is charmed, ch might attack victim's master.
	 */
	if ( IS_NPC(ch) && IS_NPC(victim) && IS_AFFECTED(victim, AFF_CHARM)
	&& victim->master != NULL && victim->master->in_room == ch->in_room
	&& number(0,9) == 0 )
	{
	    if ( ch->specials.fighting )
		stop_fighting(ch);
	    hit( ch, victim->master, TYPE_UNDEFINED );
	    return;
	}
    }

    /*
     * More charm stuff.
     */
    if ( victim->master == ch )
	stop_follower( victim );

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
	stop_follower( ch );
	    
    /*
     * Inviso attacks.  Not.
     */
    if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
    {
	act( "$n slowly fades into existence.", FALSE, ch, 0, 0, TO_ROOM );

	if ( affected_by_spell( ch, SPELL_INVISIBLE ) )
	    affect_from_char( ch, SPELL_INVISIBLE );

	REMOVE_BIT( ch->specials.affected_by, AFF_INVISIBLE );
	ch->specials.wizInvis = FALSE;
    }

    /*
     * Damage modifiers.
     */
    if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
	dam /= 2;

    if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL) && GET_ALIGNMENT(ch) < -350 )
	dam /= 2;

    /* RT  Damage reduction */
    if (dam > 30) dam = 30 + (dam - 30)/2;
    if (dam > 75) dam = 75 + (dam - 75)/2;

    if ( dam < 0 )
	dam = 0;

    /*
     * Check for parry, mob disarm, and trip.
     * Print a suitable damage message.
     */
    if ( attacktype >= TYPE_HIT && attacktype < TYPE_SUFFERING )
    {
	if ( check_parry( ch, victim ) )
	    return;
	if ( check_dodge( ch, victim ) )
	    return;
	if ( ch->equipment[WIELD] == NULL )
	    dam_message(dam, ch, victim, TYPE_HIT);
	else
	    dam_message(dam, ch, victim, attacktype);
    }
    else
    {
	for ( i = 0; i < MAX_MESSAGES; i++ )
	{
	    if ( fight_messages[i].a_type != attacktype )
		continue;

	    nr  = dice( 1, fight_messages[i].number_of_attacks );
	    j   = 1;
	    for ( messages = fight_messages[i].msg; j < nr && messages; j++ )
		messages = messages->next;

	    if ( !IS_NPC(victim) && GET_LEVEL(victim) > 31)
	    {
		act( messages->god_msg.attacker_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR );
		act( messages->god_msg.victim_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_VICT );
		act( messages->god_msg.room_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT );
	    }
	    else if ( dam == 0 )
	    {
		act( messages->miss_msg.attacker_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR );
		act( messages->miss_msg.victim_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_VICT );
		act( messages->miss_msg.room_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT );
	    }
	    else if ( GET_POS(victim) == POSITION_DEAD )
	    {
		act( messages->die_msg.attacker_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR );
		act( messages->die_msg.victim_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_VICT );
		act( messages->die_msg.room_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT );
	    }
	    else
	    {
		act( messages->hit_msg.attacker_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_CHAR );
		act( messages->hit_msg.victim_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_VICT );
		act( messages->hit_msg.room_msg,
		    FALSE, ch, ch->equipment[WIELD], victim, TO_NOTVICT );
	    }
	}
    }

    /*
     * Hurt the victim.
     * Reward the perpetrator.
     * Life is hard.
     */
    GET_HIT(victim) -= dam;
    update_pos( victim );
    if ( ch != victim )
	gain_exp( ch, GET_LEVEL(victim) * dam );

    /*
     * Inform the victim of his new state.
     * Use send_to_char, because act() doesn't send to DEAD people.
     */
    switch( GET_POS(victim) )
    {
    case POSITION_MORTALLYW:
	act( "$n is mortally wounded, and will die soon, if not aided.",
	    TRUE, victim, 0, 0, TO_ROOM );
	send_to_char( 
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
	break;

    case POSITION_INCAP:
	act( "$n is incapacitated and will slowly die, if not aided.",
	    TRUE, victim, 0, 0, TO_ROOM );
	send_to_char(
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
	break;

    case POSITION_STUNNED:
	act( "$n is stunned, but will probably recover.",
	    TRUE, victim, 0, 0, TO_ROOM);
	send_to_char("You are stunned, but will probably recover.\n\r",
	    victim );
	break;

    case POSITION_DEAD:
	act( "$n is DEAD!!", TRUE, victim, 0, 0, TO_ROOM );
	send_to_char( "You have been KILLED!!\n\r\n\r", victim );
	break;

    default:
	max_hit = hit_limit( victim );
	if ( dam > max_hit / 5 )
	    send_to_char( "That really did HURT!\n\r", victim );

	/*
	 * Wimp out?
	 */
	if ( GET_HIT(victim) < max_hit/5 )
	{
	    send_to_char( "You wish you would stop BLEEDING so much!\n\r",
		victim );
	    if ( IS_NPC(victim) )
	    {
		if ( IS_SET(victim->specials.act, ACT_WIMPY) )
		{
		    do_flee( victim, "", 0 );
		    return;
		}
	    }
	    else
	    {
		if ( IS_SET(victim->specials.act, PLR_WIMPY) )
		{
		    do_flee( victim, "", 0 );
		    return;
		}
	    }
	}
	break;
    }

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	do_flee( victim, "", 0 );
	if ( !victim->specials.fighting )
	{
	    act( "$n is rescued by divine forces.",
		FALSE, victim, 0, 0, TO_ROOM );
	    victim->specials.was_in_room = victim->in_room;
	    char_from_room(victim);
	    char_to_room(victim, 0);
	}
	return;
    }

    /*
     * Sleep spells.
     */
    if ( !AWAKE(victim) )
    {
	if ( victim->specials.fighting )
	    stop_fighting( victim );
    }

    /*
     * Payoff for killing things.
     */
    if ( GET_POS(victim) == POSITION_DEAD )
    {
	if ( IS_NPC(victim) || victim->desc != NULL )
	    group_gain( ch, victim );

	if ( !IS_NPC(victim) )
	{
	    sprintf( log_buf, "%s killed by %s at %d",
		GET_NAME(victim),
		(IS_NPC(ch) ? ch->player.short_descr : GET_NAME(ch)),
		world[victim->in_room].number );
	    log( log_buf );

	    if ( GET_EXP(victim) > exp_table[(int) GET_LEVEL(victim)] )
	    {
		GET_EXP(victim) = ( GET_EXP(victim)
		                + exp_table[(int) GET_LEVEL(victim)] ) / 2;
	    }
	}
	raw_kill( victim );

        corpse = get_obj_in_list_vis(ch, "corpse",world[ch->in_room].contents);
 
	/* RT new auto commands */
 	
        if (corpse && IS_SET(ch->specials.act,PLR_AUTOLOOT) && 
            corpse->contains && !IS_NPC(ch))  
  	  do_get( ch, "all corpse", 0 );	
 
        if (corpse && IS_SET(ch->specials.act,PLR_AUTOGOLD) && !IS_NPC(ch) &&
            corpse->contains &&!IS_SET(ch->specials.act,PLR_AUTOLOOT))
	  do_get( ch, "gold corpse", 0);
 
	if (corpse && IS_SET(ch->specials.act,PLR_AUTOSAC) && !IS_NPC(ch))
	{
	  if (IS_SET(ch->specials.act,PLR_AUTOLOOT) && corpse->contains)
	    /* return because the corpse was not empty */
	    return;
	  do_tap( ch, "corpse", 0);
 	}
	   
	return;
    }
 
    return;
}



/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( struct char_data *ch, struct char_data *victim )
{
    /*
     * No attack in safe room anyways.
     */
    if (IS_SET( world[ch->in_room].room_flags, SAFE) )
	return;
    
    /* RT  Gods cannot be killers past 31 */
    if (GET_LEVEL(ch) > 31)
       return;
    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if ( IS_NPC(victim) )
	return;
    if ( IS_SET(victim->specials.affected_by, AFF_KILLER) )
	return;
    if ( IS_SET(victim->specials.affected_by, AFF_THIEF) )
	return;

    /*
     * NPC's are cool of course (as long as not charmed).
     * Hitting yourself is cool too (bleeding).
     * And current killers stay as they are.
     */
    if ( IS_NPC(ch) || ch == victim )
	return;

    if ( IS_SET(ch->specials.affected_by, AFF_KILLER) )
	return;

    send_to_char( "*** You are now a KILLER!! ***\n\r", ch );
    SET_BIT(ch->specials.affected_by, AFF_KILLER);
    do_quit(ch,0,0);
    return;
}

/*
 * Check for parry.
 */
bool check_parry( struct char_data *ch, struct char_data *victim )
{
    int percent;
    int chance;

    if ( victim->equipment[WIELD] == NULL )
	return FALSE;

    if ( ch->equipment[WIELD] == NULL && number ( 1, 101 ) >= 50 )
        return FALSE;

    if ( IS_NPC(victim) && IS_SET(victim->specials.act,ACT_WARRIOR))
	chance	= MIN( 40, 2 * GET_LEVEL(victim) );
    else
	chance	= victim->skills[SKILL_PARRY].learned / 2;

    chance = chance + (GET_LEVEL(ch) - GET_LEVEL(victim))/2;
    percent = number(1, 101);
    if ( percent >= chance )
	return FALSE;

    act( "$n parries your attack.", FALSE, victim, NULL, ch, TO_VICT );
    act( "You parry $N's attack.",  FALSE, victim, NULL, ch, TO_CHAR );
    if (!IS_NPC(ch))
      check_improve(ch,SKILL_PARRY,4,TRUE);
    return TRUE;
}

/*
 * Check for dodge.
 */
bool check_dodge( struct char_data *ch, struct char_data *victim )
{
    int percent;
    int chance = 0;

    if ( IS_NPC(victim))
    {
      chance = GET_LEVEL(victim);
      if (IS_SET(victim->specials.act,ACT_THIEF))
	chance *= 2;
      if (IS_SET(victim->specials.act,ACT_FAST))
	chance += 20;
    }
      
    else
        chance  = victim->skills[SKILL_DODGE].learned / 2;

    chance = chance + (GET_LEVEL(victim) - GET_LEVEL(ch))/2;
    percent = number(1,101);
    if ( percent >= chance )
        return FALSE;

    act( "$n dodges your attack.", FALSE, victim, NULL, ch, TO_VICT );
    act( "You dodge $N's attack.", FALSE, victim, NULL, ch, TO_CHAR );
    if (!IS_NPC(ch))
      check_improve(ch,SKILL_DODGE,4,TRUE);
    return TRUE;
}


/*
 * Load fighting messages into memory.
 */
void load_messages(void)
{
    FILE *f1;
    int i,type;
    struct message_type *messages;
    char chk[100];

    if (!(f1 = fopen(MESS_FILE, "r"))){
	perror("read messages");
	exit(0);
    }

    for (i = 0; i < MAX_MESSAGES; i++)
    { 
	fight_messages[i].a_type = 0;
	fight_messages[i].number_of_attacks=0;
	fight_messages[i].msg = 0;
    }

    fscanf(f1, " %s \n", chk);

    while(*chk == 'M')
    {
	fscanf(f1," %d\n", &type);
	for (i = 0; (i < MAX_MESSAGES) && (fight_messages[i].a_type!=type) &&
	    (fight_messages[i].a_type); i++);
	if(i>=MAX_MESSAGES){
	    log("Too many combat messages.");
	    exit(0);
	}

	CREATE(messages,struct message_type,1);
	fight_messages[i].number_of_attacks++;
	fight_messages[i].a_type=type;
	messages->next=fight_messages[i].msg;
	fight_messages[i].msg=messages;

	messages->die_msg.attacker_msg      = fread_string(f1);
	messages->die_msg.victim_msg        = fread_string(f1);
	messages->die_msg.room_msg          = fread_string(f1);
	messages->miss_msg.attacker_msg     = fread_string(f1);
	messages->miss_msg.victim_msg       = fread_string(f1);
	messages->miss_msg.room_msg         = fread_string(f1);
	messages->hit_msg.attacker_msg      = fread_string(f1);
	messages->hit_msg.victim_msg        = fread_string(f1);
	messages->hit_msg.room_msg          = fread_string(f1);
	messages->god_msg.attacker_msg      = fread_string(f1);
	messages->god_msg.victim_msg        = fread_string(f1);
	messages->god_msg.room_msg          = fread_string(f1);
	fscanf(f1, " %s \n", chk);
    }

    fclose(f1);
}



/*
 * Set position of a victim.
 */
void update_pos( struct char_data *victim )
{
    if ( GET_HIT(victim) > 0 )
    {
    	if ( GET_POS(victim) <= POSITION_STUNNED )
	    GET_POS(victim) = POSITION_STANDING;
	return;
    }

    if ( IS_NPC(victim) || GET_HIT(victim) <= -11 )
    {
	GET_POS(victim) = POSITION_DEAD;
	return;
    }

         if ( GET_HIT(victim) <= -6 ) GET_POS(victim) = POSITION_MORTALLYW;
    else if ( GET_HIT(victim) <= -3 ) GET_POS(victim) = POSITION_INCAP;
    else                              GET_POS(victim) = POSITION_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting(struct char_data *ch, struct char_data *vict)
{
    assert(!ch->specials.fighting);

    ch->next_fighting = combat_list;
    combat_list = ch;

    if(IS_AFFECTED(ch,AFF_SLEEP))
	affect_from_char(ch,SPELL_SLEEP);

    ch->specials.fighting = vict;
    GET_POS(ch) = POSITION_FIGHTING;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting(struct char_data *ch)
{
    struct char_data *tmp;

    assert(ch->specials.fighting);

    if (ch == combat_next_dude)
	combat_next_dude = ch->next_fighting;

    if (combat_list == ch)
       combat_list = ch->next_fighting;
    else
    {
	for (tmp = combat_list; tmp && (tmp->next_fighting != ch); 
	    tmp = tmp->next_fighting)
	    ;
	if (!tmp)
	{
	    log( "Stop_fighting: char not found" );
	    abort();
	}
	tmp->next_fighting = ch->next_fighting;
    }

    ch->next_fighting = 0;
    ch->specials.fighting = 0;
    GET_POS(ch) = POSITION_STANDING;
    update_pos(ch);

    return;
}



#define MAX_NPC_CORPSE_TIME 3
#define MAX_PC_CORPSE_TIME 30

void make_corpse(struct char_data *ch)
{
    struct obj_data *corpse, *o;
    struct obj_data *money; 
    char buf[MAX_STRING_LENGTH];
    int i;

    CREATE(corpse, struct obj_data, 1);
    clear_object(corpse);

    corpse->item_number	= NOWHERE;
    corpse->in_room	= NOWHERE;
    corpse->name	= str_dup("corpse");

    sprintf( buf, "Corpse of %s is lying here.", 
      (IS_NPC(ch) ? ch->player.short_descr : GET_NAME(ch)));
    corpse->description = str_dup(buf);

    sprintf( buf, "Corpse of %s",
      (IS_NPC(ch) ? ch->player.short_descr : GET_NAME(ch)));
    corpse->short_description = str_dup(buf);

    corpse->contains = ch->carrying;
    if ( IS_NPC(ch) && GET_GOLD(ch) > 0 )
    {
	money = create_money(GET_GOLD(ch));
	GET_GOLD(ch)=0;
	obj_to_obj(money, corpse);
    }

    corpse->obj_flags.type_flag    = ITEM_CONTAINER;
    corpse->obj_flags.wear_flags   = ITEM_TAKE;
    corpse->obj_flags.value[0]     = 0; /* You can't store stuff in a corpse */
    corpse->obj_flags.value[3]     = 1; /* corpse identifyer */
    corpse->obj_flags.weight       = GET_WEIGHT(ch)+IS_CARRYING_W(ch);
    corpse->obj_flags.eq_level     = 0;
    if (IS_NPC(ch))
    {
	corpse->obj_flags.cost_per_day = 100000;
	corpse->obj_flags.timer = MAX_NPC_CORPSE_TIME;
    }
    else
    {
	corpse->obj_flags.cost_per_day = 200000;
	corpse->obj_flags.timer = MAX_PC_CORPSE_TIME;
	corpse->owner = ch;
        SET_BIT(corpse->obj_flags.extra_flags,ITEM_NOPURGE);
        REMOVE_BIT(ch->specials.act,PLR_CANLOOT);
    }

    for ( i = 0; i < MAX_WEAR; i++ )
    {
	if ( ch->equipment[i] )
	    obj_to_obj( unequip_char(ch, i), corpse );
    }

    ch->carrying	= 0;
    IS_CARRYING_N(ch)	= 0;
    IS_CARRYING_W(ch)	= 0;

    corpse->next	= object_list;
    object_list		= corpse;

    for ( o = corpse->contains; o; o->in_obj = corpse, o = o->next_content )
	;

    object_list_new_owner( corpse, 0 );
    obj_to_room( corpse, ch->in_room );

    return;
}


/* When ch kills victim */
void change_alignment(struct char_data *ch, struct char_data *victim)
{
    int align;

    align   = GET_ALIGNMENT(ch) - GET_ALIGNMENT(victim);

    if ( align > 650 )
	GET_ALIGNMENT(ch) += (align - 650) / 4;
    else if ( align < -650 )
	GET_ALIGNMENT(ch) += (align + 650) / 4;
    else
	GET_ALIGNMENT(ch) /= 2;

    GET_ALIGNMENT(ch) = MIN( 1000, MAX( -1000, GET_ALIGNMENT(ch) ) );
}



void death_cry(struct char_data *ch)
{
    int door, was_in;
    char *message;

    act( "Your blood freezes as you hear $n's death cry.",
	FALSE, ch, 0, 0, TO_ROOM );

    if ( IS_NPC(ch) )
	message = "You hear something's death cry.";
    else
	message = "You hear someone's death cry.";

    was_in = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
	if (CAN_GO(ch, door))
	{
	    ch->in_room = world[was_in].dir_option[door]->to_room;
	    if ( ch->in_room == was_in )
		continue;
	    act( message, FALSE, ch, 0, 0, TO_ROOM );
	    ch->in_room = was_in;
	}
    }
}



void raw_kill( struct char_data *ch )
{
    if ( ch->specials.fighting )
	stop_fighting( ch );

    death_cry( ch );
    make_corpse( ch );

    if ( IS_NPC(ch) )
    {
	extract_char( ch, TRUE );
	return;
    }

    extract_char( ch, FALSE );
    ch->specials.affected_by    = 0;
    GET_POS(ch)                 = POSITION_RESTING;
    while ( ch->affected )
	affect_remove( ch, ch->affected );
    ch->points.armor = 100; /* RT to fix AC bug */

    if ( GET_HIT(ch) <= 0 )
	GET_HIT(ch) = 1;
    if ( GET_MOVE(ch) <= 0 )
	GET_MOVE(ch) = 1;
    if ( GET_MANA(ch) <= 0 )
	GET_MANA(ch) = 1;
    save_char_obj( ch );
}



void group_gain( struct char_data *ch, struct char_data *victim )
{
    char buf[256];
    int mult;
    int no_members, share;
    int totallevels;
    struct char_data *k;
    struct follow_type *f;

    /*
     * Monsters don't get kill xp's.
     * Dying of mortal wounds doesn't give xp to anyone!
     */
    if ( IS_NPC(ch) || ch == victim )
	return;
    
    if ( ( k = ch->master ) == NULL )
	k = ch;

    no_members  = 0;
    totallevels = 0;

    if ( IS_AFFECTED(k, AFF_GROUP) && k->in_room == ch->in_room )
    {
	no_members  += 1;
	totallevels += GET_LEVEL(k);
    }

    for ( f = k->followers; f; f = f->next )
    {
	if ( IS_AFFECTED(k, AFF_GROUP) && f->follower->in_room == ch->in_room )
	{
	    no_members  += 1;
	    totallevels += GET_LEVEL(f->follower);
	}
    }

    if ( no_members == 0 )
    {
	no_members  = 1;
	totallevels = GET_LEVEL(ch);
    }

    share   = GET_EXP(victim);
    mult = 10;
    if (IS_SET(victim->specials.act,ACT_WARRIOR))
	mult += 1;
    if (IS_SET(victim->specials.act,ACT_MAGE))
        mult += 1;
    if (IS_SET(victim->specials.act,ACT_CLERIC))
        mult += 1;
    if (IS_SET(victim->specials.act,ACT_THIEF))
        mult += 1;
    if (IS_SET(victim->specials.act,ACT_AREA_ATTACK))
        mult += (no_members - 1);
    if (IS_SET(victim->specials.act,ACT_FAST))
        mult += 1;
    if (IS_SET(victim->specials.act,ACT_AGGRESSIVE))
        mult += 1;
    if (IS_SET(victim->specials.act,ACT_WIMPY))
        mult -= 1;

    share = (mult * share) / 10;
 	
    share   += share * (no_members - 1) / 10;

    /*
     * Kludgy loop to get k in at end.
     */
    for ( f = k->followers; ; f = f->next )
    {
	struct char_data *  tmp_ch;
	int                 tmp_share;

	tmp_ch  = (f == NULL) ? k : f->follower;

	if ( tmp_ch->in_room != ch->in_room )
	    goto LContinue;

	if ( !IS_AFFECTED(tmp_ch, AFF_GROUP) && tmp_ch != ch )
	    goto LContinue;
	    
	if ( GET_LEVEL(tmp_ch) - GET_LEVEL(k) >= 9 )
	{
	    act( "You are too high for this group.  You gain no experience.",
		FALSE, tmp_ch, 0, 0, TO_CHAR );
	    goto LContinue;
	}

	if ( GET_LEVEL(tmp_ch) - GET_LEVEL(k) <= -9 )
	{
	    act( "You are too low for this group.  You gain no experience.",
		FALSE, tmp_ch, 0, 0, TO_CHAR );
	    goto LContinue;
	}

	tmp_share   = MIN( 750000, GET_LEVEL(tmp_ch) * share / totallevels );
        if (IS_SET (tmp_ch->specials.affected_by, AFF_KILLER))
          { 
            act( "Murderers receive no rewards.", FALSE, tmp_ch, 0 ,0 ,TO_CHAR);
            goto LContinue;
          }
	sprintf( buf, "You receive %d exps of %d total.\n\r",
	    tmp_share, share );
	send_to_char( buf, tmp_ch );
	gain_exp( tmp_ch, tmp_share );
	change_alignment( tmp_ch, victim );

 LContinue:
	if ( f == NULL )
	    break;
    }
}




void dam_message( int dam, struct char_data *ch, struct char_data *victim,
		 int w_type )
{
    static char *attack_table[] =
    {
	"hit", "pound", "pierce", "slash", "whip", "claw",
	"bite", "sting", "crush"
    };

    char buf1[256], buf2[256], buf3[256];
    char *vs, *vp;
    char *attack;
    char punct;

	 if ( dam ==  0 ) { vs  = "miss";           vp  = "misses";         }
    else if ( dam <=  4 ) { vs  = "hit";            vp  = "hits";           }
    else if ( dam <=  6 ) { vs  = "injure";         vp  = "injures";        }
    else if ( dam <=  8 ) { vs  = "wound";          vp  = "wounds";         }
    else if ( dam <= 11 ) { vs  = "decimate";       vp  = "decimates";      }
    else if ( dam <= 14 ) { vs  = "devastate";      vp  = "devastates";     }
    else if ( dam <= 17 ) { vs  = "maim";           vp  = "maims";          }
    else if ( dam <= 21 ) { vs  = "MUTILATE";       vp  = "MUTILATES";      }
    else if ( dam <= 25 ) { vs  = "DISMEMBER";      vp  = "DISMEMBERS";     }
    else if ( dam <= 29 ) { vs  = "DISEMBOWEL";     vp  = "DISEMBOWELS";    }
    else if ( dam <= 33 ) { vs  = "MASSACRE";       vp  = "MASSACRES";      }
    else if ( dam <= 44 ) { vs  = "*** DEMOLISH ***"; 
                            vp  = "*** DEMOLISHES ***";			    }
    else if ( dam <= 59)  { vs  = "=== OBLITERATE ===";             
   			    vp  = "=== OBLITERATES ===";                    }
    else                  { vs  = ">>> ANNIHILATE <<<";
			    vp  = ">>> ANNIHILATES <<<";                    }

    w_type  -= TYPE_HIT;
    if ( w_type >= sizeof(attack_table)/sizeof(attack_table[0]) )
    {
	log( "Dam_message: bad w_type" );
	w_type  = 0;
    }
    punct   = (dam <= 8) ? '.' : '!';

    if ( w_type == 0 )
    {
	sprintf( buf1, "$n %s $N%c", vp, punct );
	sprintf( buf2, "You %s $N%c", vs, punct );
	sprintf( buf3, "$n %s you%c", vp, punct );
    }
    else
    {
	attack  = attack_table[w_type];
	sprintf( buf1, "$n's %s %s $N%c", attack, vp, punct );
	sprintf( buf2, "Your %s %s $N%c", attack, vp, punct );
	sprintf( buf3, "$n's %s %s you%c", attack, vp, punct );
    }

    act( buf1, FALSE, ch, NULL, victim, TO_NOTVICT );
    act( buf2, FALSE, ch, NULL, victim, TO_CHAR );
    act( buf3, FALSE, ch, NULL, victim, TO_VICT );
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( struct char_data *ch, struct char_data *victim )
{
    struct obj_data *obj;

    if ( victim->equipment[WIELD] == NULL )
	return;

    if ( ch->equipment[WIELD] == NULL && number ( 1, 101 ) >= 50 )
      return;
    if (number ( 1, 101 ) >= 50 + 5 * GET_LEVEL (victim) - 3 * GET_LEVEL(ch))
    {
    act( "$n disarms you and sends your weapon flying!",
	FALSE, ch, NULL, victim, TO_VICT );
    act( "You disarm $N and send $S weapon flying!",
	FALSE, ch, NULL, victim, TO_CHAR );
    act( "$n disarms $N and sends $S weapon flying!",
	FALSE, ch, NULL, victim, TO_NOTVICT );
  
    obj = unequip_char( victim, WIELD );
   /* buggy  obj->owner = victim; */
    obj_to_room( obj, victim->in_room );
    }
    return;
}

void kick(struct char_data *ch, struct char_data *victim)
{
  int percent, skill;

    percent=((10-(GET_AC(victim)/10))<<1) + number(1,101);

    skill = 20 + 2 * GET_LEVEL(ch);
    if (IS_SET(ch->specials.act,ACT_FAST))
      skill += 20;
 
    if (percent > skill) {
        damage(ch, victim, 0, SKILL_KICK);
    } else {
        damage(ch, victim, GET_LEVEL(ch)>>1, SKILL_KICK);
    }
    
}

void bash(struct char_data *ch, struct char_data *victim)
{
  int percent, skill;
  
  percent = number(0,101);
  skill = 20 + 2 * GET_LEVEL(ch);

  if (percent > skill)  {
       damage(ch, victim, 0, SKILL_BASH);
       GET_POS(ch) = POSITION_SITTING;
    } else {
        damage(ch, victim, 1, SKILL_BASH);
        GET_POS(victim) = POSITION_SITTING;
	WAIT_STATE(victim, PULSE_VIOLENCE*2);
        if (IS_NPC(victim))
	  victim->specials.stun_time = 2;
    }
    WAIT_STATE(ch, PULSE_VIOLENCE*2);
    ch->specials.stun_time = 1;

}

/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip( struct char_data *ch, struct char_data *victim )
{
    if (number ( 1, 101 ) >= 40 + 5 * GET_LEVEL (ch) - 3 * GET_LEVEL(victim))
    {
    act( "$n trips you and you go down!",
        FALSE, ch, NULL, victim, TO_VICT );
    act( "You trip $N and $N goes down!",
        FALSE, ch, NULL, victim, TO_CHAR );
    act( "$n trips $N and $N goes down!",
        FALSE, ch, NULL, victim, TO_NOTVICT );

    damage( ch, victim, 1, SKILL_TRIP );
    WAIT_STATE( ch, PULSE_VIOLENCE*2 );
    if (IS_NPC(ch))
      ch->specials.stun_time = 1;

    WAIT_STATE( victim, PULSE_VIOLENCE*3 );
    if (IS_NPC(victim))
      victim->specials.stun_time = 3;
    GET_POS(victim) = POSITION_SITTING;
    }

    return;
}
