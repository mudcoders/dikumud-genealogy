From lerner@scf.usc.edu Thu Apr  7 18:37:15 1994
Received: from chaph.usc.edu by garnet.berkeley.edu (8.6.8/1.33r)
	id SAA14256; Thu, 7 Apr 1994 18:37:15 -0700
Received: from phakt.usc.edu (lerner@phakt.usc.edu [128.125.253.144])
	by chaph.usc.edu (8.6.8.1/8.6.4) with ESMTP
	id SAA28034 for <merc@garnet.berkeley.edu>; Thu, 7 Apr 1994 18:37:13 -0700
Received: (lerner@localhost)
	by phakt.usc.edu (8.6.8.1/8.6.7+ucs)
	id SAA23894 for merc@garnet.berkeley.edu; Thu, 7 Apr 1994 18:37:08 -0700
Date: Thu, 7 Apr 94 18:37:07 PDT
From: Count Zero <lerner@scf.usc.edu>
To: merc@garnet.berkeley.edu
Subject: new warrior skills
Message-ID: <CMM.0.90.2.765769027.lerner@phakt.usc.edu>
Status: RO


Ok here we go  ;)

*** Shield Block

in const.c
==========

    {
        "shield block",         { 37, 37, 37,  7, 37 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_shield_block,      SLOT( 0),        0,      0,
        "",                     "!Shield Block!"
    },

in db.c
=======

sh_int                  gsn_shield_block;

in fight.c
==========
(local functions)

bool    check_shield_block  args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

(in this function add)
	/*
	 * Check for disarm, trip, parry, and dodge. (and shield bloack)
	 */
	if ( dt >= TYPE_HIT )
	{
	    if ( IS_NPC(ch) && number_percent( ) < ch->level / 5 )
		disarm( ch, victim );
	    if ( IS_NPC(ch) && number_percent( ) < ch->level / 2 )
		trip( ch, victim );
	    if ( check_parry( ch, victim ) )
		return;
ADD->       if ( check_shield_block( ch, victim ) )
ADD->            return;
	    if ( check_dodge( ch, victim ) )
		return;
	}

(and finaly include this function)

/*
 * Check for block.
 */
bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;
 
    if ( !IS_AWAKE(victim) )
        return FALSE;
 
    if ( IS_NPC(victim) )
        return FALSE;
       
    else
      {
        if ( get_eq_char( victim, WEAR_SHIELD ) == NULL )
            return FALSE;
        chance  = victim->pcdata->learned[gsn_shield_block] / 2;
      }
 
    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;
 
    act( "You block $n's attack with your shield.",  ch, NULL, victim,
TO_VICT    );
    act( "$N blocks your attack with a shield.", ch, NULL, victim,
TO_CHAR    );
    return TRUE;
  }

in merc.h
=========

(add the gsn)

extern  sh_int  gsn_shield_block;


*** Fast Healing

in const.c
==========

    {
        "fast healing",         { 37, 37, 37, 20, 37 },
        spell_null,             TAR_IGNORE,             POS_DEAD,
        &gsn_fast_healing,      SLOT( 0),        0,      0,
        "",                     "!Fast Healing!"
    },

in db.c
=======

sh_int                  gsn_fast_healing;

in merc.h
=========

(to gsns)

extern  sh_int  gsn_fast_healing;

in update.c
===========

(in this function add)

/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
         int gain;
ADD->    int number;
 
    if ( IS_NPC(ch) )
    {
	gain = ch->level * 3 / 2;
    }
    else
    {
	gain = UMIN( 5, ch->level );
        
ADD->	number = number_percent();
ADD->	if (number < ch->pcdata->learned[gsn_fast_healing])
ADD->        gain += number * gain / 100;
		  
	switch ( ch->position )
	{
	case POS_SLEEPING: gain += get_curr_con(ch);		break;
	case POS_RESTING:  gain += get_curr_con(ch) / 2;	break;
	}
 
	if ( ch->pcdata->condition[COND_FULL]   == 0 )
	    gain /= 2;
 
	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;
 
    }
 
    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;
 
    return UMIN(gain, ch->max_hit - ch->hit);
}

*** Fourth Attack

in const.c
==========

   {
        "fourth attack",         { 37, 37, 37, 25, 37 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_fourth_attack,      SLOT( 0),        0,      0,
        "",                     "!Fourth Attack!"
    },

in db.c
=======

sh_int                  gsn_fourth_attack;


in fight.c
==========
(in this function)



/*
 * Do one group of attacks.
 */

[ a bunch of stuff here, but after third attack....]

/* add all of this */
    chance = IS_NPC(ch) ? ch->level :
ch->pcdata->learned[gsn_fourth_attack]/4; /* <--- change this last number
                                                  to your discretion  */
    if ( number_percent( ) < chance )
      {
        one_hit( ch, victim, dt );
        if ( ch->fighting != victim )
            return;
      }

in merc.h
=========
(to gsns)
 
extern  sh_int  gsn_fourth_attack;

***********
Well thats it, I told you they were easy.
You might want to look at the number() in fast healing
since I pulled that one from ROM code.

- Sandman
