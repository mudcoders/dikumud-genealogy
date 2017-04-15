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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



char *	const	where_name	[] =
{
    "[Light         ] ",
    "[On Finger     ] ",
    "[On Finger     ] ",
    "[Around Neck   ] ",
    "[Around Neck   ] ",
    "[On Body       ] ",
    "[On Head       ] ",
    "[On Legs       ] ",
    "[On Feet       ] ",
    "[On Hands      ] ",
    "[On Arms       ] ",
    "[Off Hand      ] ",
    "[Around Body   ] ",
    "[Around Waist  ] ",
    "[Around Wrist  ] ",
    "[Around Wrist  ] ",
    "[Right Hand    ] ",
    "[Left Hand     ] ",
    "[On Face       ] ",
    "[Left Scabbard ] ",
    "[Right Scabbard] ",
};



/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				    bool fShort ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool	check_blind		args( ( CHAR_DATA *ch ) );

void	evil_eye		args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void 	check_left_arm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void 	check_right_arm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void 	check_left_leg		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void 	check_right_leg		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

void	obj_score		args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );


char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';
    if ( IS_HERO(ch) && obj->points > 0 ) strcat(buf, "(Legendary) " );
    if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )   strcat( buf, "(Invis) "     );
    if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
         && !IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)
         && IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)   )   strcat( buf, "(Blue Aura) "  );
    else if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
         && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)
         && !IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)   )   strcat( buf, "(Red Aura) "  );
    else if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
         && IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)
         && !IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL)
         && IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)   )   strcat( buf, "(Yellow Aura) "  );
    if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC)
         && IS_OBJ_STAT(obj, ITEM_MAGIC)  )   strcat( buf, "(Magical) "   );
    if ( IS_OBJ_STAT(obj, ITEM_GLOW)      )   strcat( buf, "(Glowing) "   );
    if ( IS_OBJ_STAT(obj, ITEM_HUM)       )   strcat( buf, "(Humming) "   );
    if ( IS_OBJ_STAT(obj, ITEM_SHADOWPLANE) &&
	 obj->in_room != NULL &&
	!IS_AFFECTED(ch,AFF_SHADOWPLANE) )    strcat( buf, "(Shadowplane) " );
    if (!IS_OBJ_STAT(obj, ITEM_SHADOWPLANE) &&
	 obj->in_room != NULL &&
	 IS_AFFECTED(ch,AFF_SHADOWPLANE) )    strcat( buf, "(Normal plane) " );

    if ( fShort )
    {
	if ( obj->short_descr != NULL )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( obj->description != NULL )
	    strcat( buf, obj->description );
    }

    return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char buf[MAX_STRING_LENGTH];
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if ( ch->desc == NULL )
	return;

    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
	count++;
    prgpstrShow	= alloc_mem( count * sizeof(char *) );
    prgnShow    = alloc_mem( count * sizeof(int)    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    { 
	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
	    continue;
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );
	    fCombine = FALSE;

	    if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		send_to_char( buf, ch );
	    }
	    else
	    {
		send_to_char( "     ", ch );
	    }
	}
	send_to_char( prgpstrShow[iShow], ch );
	send_to_char( "\n\r", ch );
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    send_to_char( "     ", ch );
	send_to_char( "Nothing.\n\r", ch );
    }

    /*
     * Clean up.
     */
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );

    return;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    char mount[MAX_STRING_LENGTH];

    buf[0] = '\0';
    buf2[0] = '\0';
    buf3[0] = '\0';

    if (!IS_NPC(victim) && victim->pcdata->chobj != NULL )
	return;

    if (victim->mount != NULL && IS_SET(victim->mounted, IS_MOUNT))
	return;

    if ( IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH))
	strcat( buf, "     " );
    else {
    	if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "(Invis) "      );
    	if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "(Hide) "       );
    	if ( IS_AFFECTED(victim, AFF_CHARM)       ) strcat( buf, "(Charmed) "    );
    	if ( IS_AFFECTED(victim, AFF_PASS_DOOR)  ||
         IS_AFFECTED(victim, AFF_ETHEREAL)    )
	 strcat( buf, "(Translucent) ");
    	if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "(Pink Aura) "  );
    	if ( IS_EVIL(victim)
    	&&   IS_AFFECTED(ch, AFF_DETECT_EVIL)     ) strcat( buf, "(Red Aura) "   );
    	if ( IS_AFFECTED(victim, AFF_SANCTUARY)   ) strcat( buf, "(White Aura) " );
    }
    if ( IS_AFFECTED(ch, AFF_SHADOWPLANE) && !IS_AFFECTED(victim, AFF_SHADOWPLANE))
	strcat( buf, "(Normal plane) "     );
    else if ( !IS_AFFECTED(ch, AFF_SHADOWPLANE) && IS_AFFECTED(victim, AFF_SHADOWPLANE))
	strcat( buf, "(Shadowplane) "     );
    /* Vampires can recognise each other - KaVir */
    if ( IS_SET(victim->act, PLR_VAMPIRE) && IS_SET(ch->act, PLR_VAMPIRE)   ) strcat( buf, "(Vampire) " );
    if ( !IS_NPC(ch) && IS_VAMPAFF(ch,VAM_AUSPEX) &&
	 !IS_NPC(victim) && IS_VAMPAFF(victim, VAM_DISGUISED) )
    {
	strcat( buf, "(");
	strcat( buf, victim->name);
	strcat( buf, ") ");
    }

    if ( IS_AFFECTED(victim, AFF_FLAMING) ) 
    {
	if (IS_NPC(victim))
	    sprintf( buf2, "\n\r...%s is engulfed in blazing flames!", victim->short_descr );
	else if (!IS_NPC(victim) && IS_AFFECTED(victim,AFF_POLYMORPH))
	    sprintf( buf2, "\n\r...%s is engulfed in blazing flames!", victim->morph );
	else
	    sprintf( buf2, "\n\r...%s is engulfed in blazing flames!", victim->name );
    }

    if ( !IS_NPC(victim) && IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH)) 
    {
	if (IS_BODY(victim,GAGGED) && IS_BODY(victim,BLINDFOLDED))
	    sprintf( buf3, "...%s is gagged and blindfolded!", victim->morph );
	else if (IS_BODY(victim,GAGGED))
	    sprintf( buf3, "...%s is gagged!", victim->morph );
	else if (IS_BODY(victim,BLINDFOLDED))
	    sprintf( buf3, "...%s is blindfolded!", victim->morph );
    }

    if ( IS_HEAD(victim, LOST_HEAD) && IS_AFFECTED(victim, AFF_POLYMORPH))
    {
	strcat( buf, victim->morph );
	strcat( buf, " is lying here." );
    	strcat( buf, buf2 );
    	strcat( buf, buf3 );
    	strcat( buf, "\n\r" );
    	buf[5] = UPPER(buf[5]);
    	send_to_char( buf, ch );
	return;
    }

    if ( IS_BODY(victim, TIED_UP) ) 
    {
	if (IS_NPC(victim))
	    sprintf( buf3, "\n\r...%s is tied up", victim->short_descr );
	else if (!IS_NPC(victim) && IS_AFFECTED(victim,AFF_POLYMORPH))
	    sprintf( buf3, "\n\r...%s is tied up", victim->morph );
	else
	    sprintf( buf3, "\n\r...%s is tied up", victim->name );
	if (IS_BODY(victim,GAGGED) && IS_BODY(victim,BLINDFOLDED))
	    strcat( buf3, ", gagged and blindfolded!" );
	else if (IS_BODY(victim,GAGGED))
	    strcat( buf3, " and gagged!" );
	else if (IS_BODY(victim,BLINDFOLDED))
	    strcat( buf3, " and blindfolded!" );
	else
	    strcat( buf3, "!" );
    }

    if ( !IS_NPC(victim) && IS_AFFECTED(victim, AFF_POLYMORPH) )
	strcat( buf, victim->morph );
    else if ( victim->position == POS_STANDING && victim->long_descr[0] != '\0' && victim->mount == NULL )
    {
	strcat( buf, victim->long_descr );
	send_to_char( buf, ch );
    	if ( IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD) ) 
	    act( "...$N is surrounded by a crackling shield of lightning.", ch,NULL,victim,TO_CHAR );
    	if ( IS_ITEMAFF(victim, ITEMA_FIRESHIELD) ) 
	    act( "...$N is surrounded by a burning shield of fire.", ch,NULL,victim,TO_CHAR );
    	if ( IS_ITEMAFF(victim, ITEMA_ICESHIELD) ) 
	    act( "...$N is surrounded by a shimmering shield of ice.", ch,NULL,victim,TO_CHAR );
    	if ( IS_ITEMAFF(victim, ITEMA_ACIDSHIELD) ) 
	    act( "...$N is surrounded by a bubbling shield of acid.", ch,NULL,victim,TO_CHAR );
	return;
    }
    else
    	strcat( buf, PERS( victim, ch ) );

    if (victim->mount != NULL && victim->mounted == IS_RIDING)
    {
	if (IS_NPC(victim->mount))
	    sprintf( mount, " is here riding %s", victim->mount->short_descr );
	else
	    sprintf( mount, " is here riding %s", victim->mount->name );
	strcat( buf, mount );
	if (victim->position == POS_FIGHTING)
	{
	    strcat( buf, ", fighting " );
	    if ( victim->fighting == NULL )
	        strcat( buf, "thin air??" );
	    else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	    else if ( victim->in_room == victim->fighting->in_room )
	    {
	        strcat( buf, PERS( victim->fighting, ch ) );
	        strcat( buf, "." );
	    }
	    else
	        strcat( buf, "somone who left??" );
	}
	else strcat( buf, "." );
    }
    else if ( victim->position == POS_STANDING && IS_AFFECTED(victim, AFF_FLYING) )
	strcat( buf, " is hovering here" );
    else if ( victim->position == POS_STANDING && (!IS_NPC(victim) && (IS_VAMPAFF(victim, VAM_FLYING))) )
	strcat( buf, " is hovering here" );
    else
    {
    	switch ( victim->position )
    	{
    	case POS_DEAD:     strcat( buf, " is DEAD!!" );              break;
    	case POS_MORTAL:   strcat( buf, " is mortally wounded." );   break;
    	case POS_INCAP:    strcat( buf, " is incapacitated." );      break;
    	case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
    	case POS_SLEEPING: strcat( buf, " is sleeping here." );      break;
    	case POS_RESTING:  strcat( buf, " is resting here." );       break;
    	case POS_STANDING:
	    if      (!IS_NPC(victim) && victim->stance[0] == STANCE_NORMAL)
		strcat( buf, " is here, crouched in a fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_VIPER)
		strcat( buf, " is here, crouched in a viper fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_CRANE)
		strcat( buf, " is here, crouched in a crane fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_CRAB)
		strcat( buf, " is here, crouched in a crab fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_MONGOOSE)
		strcat( buf, " is here, crouched in a mongoose fighting stance." );
	    else if (!IS_NPC(victim) && victim->stance[0] == STANCE_BULL)
		strcat( buf, " is here, crouched in a bull fighting stance." );
	    else
		strcat( buf, " is here." );
	    break;
    	case POS_FIGHTING:
	    strcat( buf, " is here, fighting " );
	    if ( victim->fighting == NULL )
	        strcat( buf, "thin air??" );
	    else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	    else if ( victim->in_room == victim->fighting->in_room )
	    {
	        strcat( buf, PERS( victim->fighting, ch ) );
	        strcat( buf, "." );
	    }
	    else
	        strcat( buf, "somone who left??" );
	    break;
	}
    }

    strcat( buf, buf2 );
    strcat( buf, buf3 );
    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );

    if ( IS_ITEMAFF(victim, ITEMA_SHOCKSHIELD) ) 
	act( "...$N is surrounded by a crackling shield of lightning.", ch,NULL,victim,TO_CHAR );
    if ( IS_ITEMAFF(victim, ITEMA_FIRESHIELD) ) 
	act( "...$N is surrounded by a burning shield of fire.", ch,NULL,victim,TO_CHAR );
    if ( IS_ITEMAFF(victim, ITEMA_ICESHIELD) ) 
	act( "...$N is surrounded by a shimmering shield of ice.", ch,NULL,victim,TO_CHAR );
    if ( IS_ITEMAFF(victim, ITEMA_ACIDSHIELD) ) 
	act( "...$N is surrounded by a bubbling shield of acid.", ch,NULL,victim,TO_CHAR );
    return;
}



void evil_eye( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf [MAX_INPUT_LENGTH];
    int tabletype;
    if (IS_NPC(victim)) return;
    if (victim == ch) return;
    if (ch->level != 3 || victim->level != 3) return;
    if (!IS_VAMPAFF(victim, VAM_DOMINATE)) return;
    if (victim->powertype != NULL && strlen(victim->powertype) > 1)
    {
	sprintf(buf,"\n\r%s\n\r",victim->powertype);
	send_to_char(buf,ch);
    }
    if (IS_SET(victim->spectype,EYE_SELFACTION) && victim->poweraction != NULL)
	interpret(victim,victim->poweraction);
    if (IS_SET(victim->spectype,EYE_ACTION) && victim->poweraction != NULL)
	interpret(ch,victim->poweraction);
    if (IS_SET(victim->spectype,EYE_SPELL) && victim->specpower > 0)
    {
	tabletype = skill_table[victim->specpower].target;
	(*skill_table[victim->specpower].spell_fun) (victim->specpower,victim->spl[tabletype],victim,ch);
    }
    return;
}


void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int iWear;
    int percent;
    bool found;

    if ( can_see( victim, ch ) )
    {
	act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
	act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
    }

    if (!IS_NPC(ch) && IS_HEAD(victim,LOST_HEAD))
    {
	act( "$N is lying here.", ch, NULL, victim, TO_CHAR );
	return;
    }
    if ( victim->description[0] != '\0' )
    {
	send_to_char( victim->description, ch );
    }
    else
    {
	act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    if ( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

    strcpy( buf, PERS(victim, ch) );

         if ( percent >= 100 ) strcat( buf, " is in perfect health.\n\r"  );
    else if ( percent >=  90 ) strcat( buf, " is slightly scratched.\n\r" );
    else if ( percent >=  80 ) strcat( buf, " has a few bruises.\n\r"     );
    else if ( percent >=  70 ) strcat( buf, " has some cuts.\n\r"         );
    else if ( percent >=  60 ) strcat( buf, " has several wounds.\n\r"    );
    else if ( percent >=  50 ) strcat( buf, " has many nasty wounds.\n\r" );
    else if ( percent >=  40 ) strcat( buf, " is bleeding freely.\n\r"    );
    else if ( percent >=  30 ) strcat( buf, " is covered in blood.\n\r"   );
    else if ( percent >=  20 ) strcat( buf, " is leaking guts.\n\r"       );
    else if ( percent >=  10 ) strcat( buf, " is almost dead.\n\r"        );
    else                       strcat( buf, " is DYING.\n\r"              );

    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );

    if (IS_AFFECTED(victim, AFF_INFRARED) || IS_VAMPAFF(victim, VAM_NIGHTSIGHT)) act("$N's eyes are glowing bright red.",ch,NULL,victim,TO_CHAR);
    if (IS_AFFECTED(victim, AFF_FLYING)) act("$N is hovering in the air.",ch,NULL,victim,TO_CHAR);
    if (IS_VAMPAFF(victim, VAM_FANGS)) act("$N has a pair of long, pointed fangs.",ch,NULL,victim,TO_CHAR);
    if (IS_VAMPAFF(victim, VAM_CLAWS)) act("$N has razer sharp claws protruding from under $S finger nails.",ch,NULL,victim,TO_CHAR);

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		act( "$N is using:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    send_to_char( where_name[iWear], ch );
	    if (IS_NPC(ch) || ch->pcdata->chobj == NULL || ch->pcdata->chobj != obj)
	    {
	    	send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    	send_to_char( "\n\r", ch );
	    }
	    else
		send_to_char( "you\n\r", ch);
	}
    }

    if ( victim != ch
    &&   !IS_NPC(ch)
    &&   !IS_HEAD(victim, LOST_HEAD)
    &&   number_percent( ) < ch->pcdata->learned[gsn_peek] )
    {
	send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }

    return;
}



void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch == ch )
	    continue;

	if ( !IS_NPC(rch)
	&&   IS_SET(rch->act, PLR_WIZINVIS)
	&&   get_trust( ch ) < get_trust( rch ) )
	    continue;

	if ( !IS_NPC(rch) && IS_HEAD(rch,LOST_HEAD))
	    continue;

	if ( !IS_NPC(rch) && IS_EXTRA(rch,EXTRA_OSWITCH))
	    continue;

	if ( can_see( ch, rch ) )
	    show_char_to_char_0( rch, ch );
	else if ( room_is_dark( ch->in_room )
	&&      ( IS_AFFECTED(rch, AFF_INFRARED) || IS_VAMPAFF(rch, VAM_NIGHTSIGHT)) )
	{
	    send_to_char( "You see glowing red eyes watching YOU!\n\r", ch );
	}
    }

    return;
} 



bool check_blind( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;

    if ( IS_HEAD(ch, LOST_EYE_L) && IS_HEAD(ch, LOST_EYE_R) )
    {
	send_to_char( "You have no eyes to see with!\n\r", ch );
	return FALSE;
    }

    if ( IS_BODY(ch, BLINDFOLDED) )
    {
	send_to_char( "You can't see a thing through the blindfold!\n\r", ch );
	return FALSE;
    }

    if ( IS_AFFECTED(ch, AFF_BLIND) )
    {
	send_to_char( "You can't see a thing!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}



void do_look( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA *obj;
    OBJ_DATA *portal;
    OBJ_DATA *portal_next;
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *location;
    char *pdesc;
    int door;
    bool found;

    if ( ch->desc == NULL )
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "You can't see anything but stars!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC(ch)
    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
    &&   !IS_VAMPAFF(ch, VAM_NIGHTSIGHT)
    &&   !IS_AFFECTED(ch, AFF_SHADOWPLANE)
    &&   !(ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
	&& !IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
    &&   room_is_dark( ch->in_room ) )
    {
	send_to_char( "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
	if (ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
	&& !IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
	    act( "$p",ch,ch->pcdata->chobj->in_obj,NULL,TO_CHAR);
    	else if ( IS_AFFECTED(ch, AFF_SHADOWPLANE) )
	    send_to_char( "The shadow plane\n\r", ch );
	else
	{
	    send_to_char( ch->in_room->name, ch );
	    send_to_char( "\n\r", ch );
	}

	if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT) )
	    do_exits( ch, "auto" );

	if (ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT
	&& !IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
	{
	    act( "You are inside $p.",ch,ch->pcdata->chobj->in_obj,NULL,TO_CHAR);
	    show_list_to_char( ch->pcdata->chobj->in_obj->contains, ch, FALSE, FALSE );
	}
	else if ( (arg1[0] == '\0' || !str_cmp( arg1, "auto" ) ) && IS_AFFECTED(ch, AFF_SHADOWPLANE) )
	    send_to_char( "You are standing in complete darkness.\n\r", ch );
	else if ( ( !IS_NPC(ch) && !IS_SET(ch->act, PLR_BRIEF) ) &&
		( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) ) )
	{
	    send_to_char( ch->in_room->description, ch );
	    if (ch->in_room->blood == 1000)
		sprintf(buf,"You notice that the room is completely drenched in blood.\n\r");
	    else if (ch->in_room->blood > 750)
		sprintf(buf,"You notice that there is a very large amount of blood around the room.\n\r");
	    else if (ch->in_room->blood > 500)
		sprintf(buf,"You notice that there is a large quantity of blood around the room.\n\r");
	    else if (ch->in_room->blood > 250)
		sprintf(buf,"You notice a fair amount of blood on the floor.\n\r");
	    else if (ch->in_room->blood > 100)
		sprintf(buf,"You notice several blood stains on the floor.\n\r");
	    else if (ch->in_room->blood > 50)
		sprintf(buf,"You notice a few blood stains on the floor.\n\r");
	    else if (ch->in_room->blood > 25)
		sprintf(buf,"You notice a couple of blood stains on the floor.\n\r");
	    else if (ch->in_room->blood > 0)
		sprintf(buf,"You notice a few drops of blood on the floor.\n\r");
	    else sprintf(buf,"You notice nothing special in the room.\n\r");
	    ADD_COLOUR(ch, buf, L_RED);
	    if (ch->in_room->blood > 0) send_to_char(buf,ch);
	}

	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->people,   ch );
	return;
    }

    if ( !str_cmp( arg1, "i" ) || !str_cmp( arg1, "in" ) )
    {
	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "That is not a container.\n\r", ch );
	    break;

	case ITEM_PORTAL:
	    pRoomIndex = get_room_index(obj->value[0]);
	    location = ch->in_room;
	    if ( pRoomIndex == NULL )
	    {
		send_to_char( "It doesn't seem to lead anywhere.\n\r", ch );
		return;
	    }
	    if (obj->value[2] == 1 || obj->value[2] == 3)
	    {
		send_to_char( "It seems to be closed.\n\r", ch );
		return;
	    }
	    char_from_room(ch);
	    char_to_room(ch,pRoomIndex);

	    found = FALSE;
	    for ( portal = ch->in_room->contents; portal != NULL; portal = portal_next )
	    {
		portal_next = portal->next_content;
		if ( ( obj->value[0] == portal->value[3]  )
		    && (obj->value[3] == portal->value[0]) )
		{
		    found = TRUE;
		    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
			!IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
		    {
			REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    		do_look(ch,"auto");
			SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
			break;
		    }
		    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
			IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
		    {
			SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    		do_look(ch,"auto");
			REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
			break;
		    }
		    else
		    {
	    		do_look(ch,"auto");
			break;
		    }
	    	}
	    }
	    char_from_room(ch);
	    char_to_room(ch,location);
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "It is empty.\n\r", ch );
		break;
	    }

	    if (obj->value[1] < obj->value[0] / 5)
	    	sprintf( buf, "There is a little %s liquid left in it.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 4)
	    	sprintf( buf, "It contains a small about of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 3)
	    	sprintf( buf, "It's about a third full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 2)
	    	sprintf( buf, "It's about half full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0])
	    	sprintf( buf, "It is almost full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] == obj->value[0])
	    	sprintf( buf, "It's completely full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else
	    	sprintf( buf, "Somehow it is MORE than full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    send_to_char( buf, ch );
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		send_to_char( "It is closed.\n\r", ch );
		break;
	    }

	    act( "$p contains:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    break;
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) != NULL )
    {
	show_char_to_char_1( victim, ch );
	evil_eye(victim,ch);
	return;
    }

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if (!IS_NPC(vch) && !str_cmp(arg1,vch->morph))
	    {
		show_char_to_char_1( vch, ch );
		evil_eye(vch,ch);
		return;
	    }
	    continue;
	}
    }

    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
    {
	obj = get_obj_in_obj(ch,arg1);
	if (obj != NULL)
	{
	    send_to_char( obj->description, ch );
	    send_to_char( "\n\r", ch );
	    return;
	}
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
	    continue;
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if ( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    send_to_char( "\n\r", ch );
	    return;
	}
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
	if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
	    continue;
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if ( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    send_to_char( "\n\r", ch );
	    return;
	}
    }

    pdesc = get_extra_descr( arg1, ch->in_room->extra_descr );
    if ( pdesc != NULL )
    {
	send_to_char( pdesc, ch );
	return;
    }

         if ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
    else
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    /* 'look direction' */
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }
/*
    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );
*/

    if ( pexit->keyword    != NULL
    &&   pexit->keyword[0] != '\0'
    &&   pexit->keyword[0] != ' ' )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
	else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
	{
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
	    if ((pexit = ch->in_room->exit[door]) == NULL) return;
	    if ((pRoomIndex = pexit->to_room) == NULL) return;
	    location = ch->in_room;
	    char_from_room(ch);
	    char_to_room(ch,pRoomIndex);
	    do_look(ch,"auto");
	    char_from_room(ch);
	    char_to_room(ch,location);
	}
	else
	{
	    if ((pexit = ch->in_room->exit[door]) == NULL) return;
	    if ((pRoomIndex = pexit->to_room) == NULL) return;
	    location = ch->in_room;
	    char_from_room(ch);
	    char_to_room(ch,pRoomIndex);
	    do_look(ch,"auto");
	    char_from_room(ch);
	    char_to_room(ch,location);
	}
    }
    else
    {
	if ((pexit = ch->in_room->exit[door]) == NULL) return;
	if ((pRoomIndex = pexit->to_room) == NULL) return;
	location = ch->in_room;
	char_from_room(ch);
	char_to_room(ch,pRoomIndex);
	do_look(ch,"auto");
	char_from_room(ch);
	char_to_room(ch,location);
    }

    return;
}



void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Examine what?\n\r", ch );
	return;
    }

    do_look( ch, arg );

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	     if (obj->condition >= 100) 
	    sprintf( buf, "You notice that %s is in perfect condition.\n\r",obj->short_descr );
	else if (obj->condition >= 75 )
	    sprintf( buf, "You notice that %s is in good condition.\n\r",obj->short_descr );
	else if (obj->condition >= 50 )
	    sprintf( buf, "You notice that %s is in average condition.\n\r",obj->short_descr );
	else if (obj->condition >= 25 )
	    sprintf( buf, "You notice that %s is in poor condition.\n\r",obj->short_descr );
	else
	    sprintf( buf, "You notice that %s is in awful condition.\n\r",obj->short_descr );
	send_to_char(buf,ch);
	switch ( obj->item_type )
	{
	default:
	    break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    send_to_char( "When you look inside, you see:\n\r", ch );
	    sprintf( buf, "in %s", arg );
	    do_look( ch, buf );
	}
    }

    return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
    extern char * const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door;

    buf[0] = '\0';
    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
	return;

    strcpy( buf, fAuto ? "[Exits:" : "Obvious exits:\n\r" );

    found = FALSE;
    for ( door = 0; door <= 5; door++ )
    {
	if ( ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		strcat( buf, " " );
		strcat( buf, dir_name[door] );
	    }
	    else
	    {
		sprintf( buf + strlen(buf), "%-5s - %s\n\r",
		    capitalize( dir_name[door] ),
		    room_is_dark( pexit->to_room )
			?  "Too dark to tell"
			: pexit->to_room->name
		    );
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " none" : "None.\n\r" );

    if ( fAuto )
	strcat( buf, "]\n\r" );

    send_to_char( buf, ch );
    return;
}



void do_score( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char ss1[MAX_STRING_LENGTH];
    char ss2[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int hr;
    int dr;

    if (!IS_NPC(ch) && (IS_EXTRA(ch,EXTRA_OSWITCH) || IS_HEAD(ch,LOST_HEAD)))
	{obj_score(ch,ch->pcdata->chobj);return;}
    sprintf( buf,
	"You are %s%s, %d years old (%d hours).\n\r",
	ch->name,
	IS_NPC(ch) ? "" : ch->pcdata->title,
	get_age(ch),
	(get_age(ch) - 17) * 2 );
    send_to_char( buf, ch );

    if (!IS_NPC(ch))
    {
    	sprintf( buf, "This character was created %s\n\r", ch->createtime);
	send_to_char( buf, ch );
    }

    if ( get_trust( ch ) != ch->level )
    {
	sprintf( buf, "You are trusted at level %d.\n\r",
	    get_trust( ch ) );
	send_to_char( buf, ch );
    }

    sprintf( buf,
	"You have %d/%d hit, %d/%d mana, %d/%d movement, %d primal energy.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->practice );
    send_to_char( buf, ch );

    sprintf( buf,
	"You are carrying %d/%d items with weight %d/%d kg.\n\r",
	ch->carry_number, can_carry_n(ch),
	ch->carry_weight, can_carry_w(ch) );
    send_to_char( buf, ch );

    sprintf( buf,
	"Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d.\n\r",
	get_curr_str(ch),
	get_curr_int(ch),
	get_curr_wis(ch),
	get_curr_dex(ch),
	get_curr_con(ch) );
    send_to_char( buf, ch );

    sprintf( buf,
	"You have scored %d exp, and have %d gold coins.\n\r",
	ch->exp,  ch->gold );
    send_to_char( buf, ch );

    sprintf( buf,
	"Autoexit: %s.  Autoloot: %s.  Autosac: %s.\n\r",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT)) ? "yes" : "no",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOLOOT)) ? "yes" : "no",
	(!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOSAC) ) ? "yes" : "no" );
    send_to_char( buf, ch );
    
    sprintf( buf, "Wimpy set to %d hit points.\n\r", ch->wimpy );
    send_to_char( buf, ch );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
	send_to_char( "You are drunk.\n\r",   ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
	send_to_char( "You are thirsty.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL]   ==  0 )
	send_to_char( "You are hungry.\n\r",  ch );

    switch ( ch->position )
    {
    case POS_DEAD:     
	send_to_char( "You are DEAD!!\n\r",		ch );
	break;
    case POS_MORTAL:
	send_to_char( "You are mortally wounded.\n\r",	ch );
	break;
    case POS_INCAP:
	send_to_char( "You are incapacitated.\n\r",	ch );
	break;
    case POS_STUNNED:
	send_to_char( "You are stunned.\n\r",		ch );
	break;
    case POS_SLEEPING:
	send_to_char( "You are sleeping.\n\r",		ch );
	break;
    case POS_RESTING:
	send_to_char( "You are resting.\n\r",		ch );
	break;
    case POS_STANDING:
	send_to_char( "You are standing.\n\r",		ch );
	break;
    case POS_FIGHTING:
	send_to_char( "You are fighting.\n\r",		ch );
	break;
    }

    if ( ch->level >= 0 )
    {
	sprintf( buf, "AC: %d.  ", GET_AC(ch) );
	send_to_char( buf, ch );
    }

    send_to_char( "You are ", ch );
         if ( GET_AC(ch) >=  101 ) send_to_char( "naked!\n\r", ch );
    else if ( GET_AC(ch) >=   80 ) send_to_char( "barely clothed.\n\r",   ch );
    else if ( GET_AC(ch) >=   60 ) send_to_char( "wearing clothes.\n\r",  ch );
    else if ( GET_AC(ch) >=   40 ) send_to_char( "slightly armored.\n\r", ch );
    else if ( GET_AC(ch) >=   20 ) send_to_char( "somewhat armored.\n\r", ch );
    else if ( GET_AC(ch) >=    0 ) send_to_char( "armored.\n\r",          ch );
    else if ( GET_AC(ch) >=  -50 ) send_to_char( "well armored.\n\r",     ch );
    else if ( GET_AC(ch) >= -100 ) send_to_char( "strongly armored.\n\r", ch );
    else if ( GET_AC(ch) >= -150 ) send_to_char( "heavily armored.\n\r",  ch );
    else if ( GET_AC(ch) >= -200 ) send_to_char( "superbly armored.\n\r", ch );
    else if ( GET_AC(ch) >= -249 ) send_to_char( "divinely armored.\n\r", ch );
    else                           send_to_char( "ultimately armored!\n\r",ch );

    hr = GET_HITROLL(ch); dr = GET_DAMROLL(ch);
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_VAMPIRE))
    	{ hr += ch->pcdata->wolf; dr += ch->pcdata->wolf; }
    sprintf( buf, "Hitroll: %d.  Damroll: %d.  ", hr, dr );
    send_to_char( buf, ch );
    
    if ( !IS_NPC(ch) && IS_SET(ch->act,PLR_VAMPIRE) )
    {
	sprintf( buf, "Blood: %d.\n\r", ch->pcdata->condition[COND_THIRST] );
	send_to_char( buf, ch );
    }
    else if (ch->level >= 0)
	send_to_char( "\n\r", ch );

    if ( !IS_NPC(ch) && IS_SET(ch->act,PLR_VAMPIRE) )
    {
	sprintf( buf, "Beast: %d.  ", ch->beast );
	send_to_char( buf, ch );
	if      (ch->beast <  0 ) send_to_char("You are a cheat!\n\r",ch);
	else if (ch->beast == 0 ) send_to_char("You have attained Golconda!\n\r",ch);
	else if (ch->beast <= 5 ) send_to_char("You have almost reached Golconda!\n\r",ch);
	else if (ch->beast <= 10) send_to_char("You are nearing Golconda!\n\r",ch);
	else if (ch->beast <= 15) send_to_char("You have great control over your beast.\n\r",ch);
	else if (ch->beast <= 20) send_to_char("Your beast has little influence over your actions.\n\r",ch);
	else if (ch->beast <= 30) send_to_char("You are in control of your beast.\n\r",ch);
	else if (ch->beast <= 40) send_to_char("You are able to hold back the beast.\n\r",ch);
	else if (ch->beast <= 60) send_to_char("You are constantly struggling for control of your beast.\n\r",ch);
	else if (ch->beast <= 75) send_to_char("Your beast has great control over your actions.\n\r",ch);
	else if (ch->beast <= 90) send_to_char("The power of the beast overwhelms you.\n\r",ch);
	else if (ch->beast <= 99) send_to_char("You have almost lost your battle with the beast!\n\r",ch);
	else                      send_to_char("The beast has taken over!\n\r",ch);
    }

    if ( ch->level >= 0 )
    {
	sprintf( buf, "Alignment: %d.  ", ch->alignment );
	send_to_char( buf, ch );
    }

    send_to_char( "You are ", ch );
         if ( ch->alignment >  900 ) send_to_char( "angelic.\n\r", ch );
    else if ( ch->alignment >  700 ) send_to_char( "saintly.\n\r", ch );
    else if ( ch->alignment >  350 ) send_to_char( "good.\n\r",    ch );
    else if ( ch->alignment >  100 ) send_to_char( "kind.\n\r",    ch );
    else if ( ch->alignment > -100 ) send_to_char( "neutral.\n\r", ch );
    else if ( ch->alignment > -350 ) send_to_char( "mean.\n\r",    ch );
    else if ( ch->alignment > -700 ) send_to_char( "evil.\n\r",    ch );
    else if ( ch->alignment > -900 ) send_to_char( "demonic.\n\r", ch );
    else                             send_to_char( "satanic.\n\r", ch );
    
    if ( !IS_NPC(ch) && ch->level >= 0 )
    {
	sprintf( buf, "Status: %d.  ", ch->race );
	send_to_char( buf, ch );
    }

    if (!IS_NPC(ch)) send_to_char( "You are ", ch );

         if (!IS_NPC(ch) && ch->level == 1 ) send_to_char( "a Mortal.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 2 ) send_to_char( "a Mortal.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 4 ) send_to_char( "a Builder.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 5 ) send_to_char( "a Quest Maker.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 6 ) send_to_char( "an Enforcer.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 7 ) send_to_char( "a Judge.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 8 ) send_to_char( "a High Judge.\n\r", ch);
    else if (!IS_NPC(ch) && ch->level == 9 ) send_to_char( "an Implementor.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race <= 0 ) send_to_char( "an Avatar.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race <= 4 ) send_to_char( "an Immortal.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race <= 9 ) send_to_char( "a Godling.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race <= 14) send_to_char( "a Demigod.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race <= 19) send_to_char( "a Lesser God.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race <= 24) send_to_char( "a Greater God.\n\r", ch);
    else if (!IS_NPC(ch) && ch->race >= 25) send_to_char( "a Supreme God.\n\r", ch);
    else if (!IS_NPC(ch)) send_to_char( "a Bugged Character!\n\r", ch);

    if ( !IS_NPC(ch) )
    {
	if      (ch->pkill  == 0) sprintf(ss1,"no players");
	else if (ch->pkill  == 1) sprintf(ss1,"%d player",ch->pkill);
	else                      sprintf(ss1,"%d players",ch->pkill);
	if      (ch->pdeath == 0) sprintf(ss2,"no players");
	else if (ch->pdeath == 1) sprintf(ss2,"%d player",ch->pdeath);
	else                      sprintf(ss2,"%d players",ch->pdeath);
	sprintf( buf, "You have killed %s and have been killed by %s.\n\r", ss1, ss2 );
	send_to_char( buf, ch );
	if      (ch->mkill  == 0) sprintf(ss1,"no mobs");
	else if (ch->mkill  == 1) sprintf(ss1,"%d mob",ch->mkill);
	else                      sprintf(ss1,"%d mobs",ch->mkill);
	if      (ch->mdeath == 0) sprintf(ss2,"no mobs");
	else if (ch->mdeath == 1) sprintf(ss2,"%d mob",ch->mdeath);
	else                      sprintf(ss2,"%d mobs",ch->mdeath);
	sprintf( buf, "You have killed %s and have been killed by %s.\n\r", ss1, ss2 );
	send_to_char( buf, ch );
    }

    if ( !IS_NPC(ch) && ch->pcdata->quest > 0)
    {
	if (ch->pcdata->quest == 1)
	    sprintf( buf, "You have a single quest point.\n\r" );
	else
	    sprintf( buf, "You have %d quest points.\n\r", ch->pcdata->quest );
	send_to_char( buf, ch );
    }

    if (IS_AFFECTED(ch,AFF_HIDE)) send_to_char( "You are keeping yourself hidden from those around you.\n\r", ch );

    if ( !IS_NPC(ch) && IS_SET(ch->act,PLR_VAMPIRE) && ch->pcdata->wolf > 0)
    {
	sprintf(buf,"The beast is in control of your actions:  Affects Hitroll and Damroll by +%d.\n\r",ch->pcdata->wolf);
	send_to_char( buf, ch );
    }

    if ( ch->affected != NULL )
    {
	send_to_char( "You are affected by:\n\r", ch );
	for ( paf = ch->affected; paf != NULL; paf = paf->next )
	{
	    sprintf( buf, "Spell: '%s'", skill_table[paf->type].name );
	    send_to_char( buf, ch );

	    if ( ch->level >= 0 )
	    {
		sprintf( buf,
		    " modifies %s by %d for %d hours",
		    affect_loc_name( paf->location ),
		    paf->modifier,
		    paf->duration );
		send_to_char( buf, ch );
	    }

	    send_to_char( ".\n\r", ch );
	}
    }

    return;
}



char *	const	day_name	[] =
{
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *	const	month_name	[] =
{
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
	"It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\rGod Wars started up at %s\rThe system time is %s\r",

	(time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
	day, suf,
	month_name[time_info.month],
	str_boot_time,
	(char *) ctime( &current_time )
	);

    send_to_char( buf, ch );
    return;
}



void do_weather( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    static char * const sky_look[4] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You can't see the weather indoors.\n\r", ch );
	return;
    }

    sprintf( buf, "The sky is %s and %s.\n\r",
	sky_look[weather_info.sky],
	weather_info.change >= 0
	? "a warm southerly breeze blows"
	: "a cold northern gust blows"
	);
    send_to_char( buf, ch );
    return;
}



void do_help( CHAR_DATA *ch, char *argument )
{
    char argall[MAX_INPUT_LENGTH];
    char argone[MAX_INPUT_LENGTH];
    HELP_DATA *pHelp;

    if ( argument[0] == '\0' )
	argument = "summary";

    /*
     * Tricky argument handling so 'help a b' doesn't match a.
     */
    argall[0] = '\0';
    while ( argument[0] != '\0' )
    {
	argument = one_argument( argument, argone );
	if ( argall[0] != '\0' )
	    strcat( argall, " " );
	strcat( argall, argone );
    }

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
	if ( pHelp->level > get_trust( ch ) )
	    continue;

	if ( is_name( argall, pHelp->keyword ) )
	{
	    if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) )
	    {
		send_to_char( pHelp->keyword, ch );
		send_to_char( "\n\r", ch );
	    }

	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if ( pHelp->text[0] == '.' )
		send_to_char( pHelp->text+1, ch );
	    else
		send_to_char( pHelp->text  , ch );
	    return;
	}
    }

    send_to_char( "No help on that word.\n\r", ch );
    return;
}



/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char kav[MAX_STRING_LENGTH];
    char openb[5];
    char closeb[5];
    DESCRIPTOR_DATA *d;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    int iClass;
    bool rgfClass[MAX_CLASS];
    bool fClassRestrict;
    bool fImmortalOnly;
 
    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    fClassRestrict = FALSE;
    fImmortalOnly  = FALSE;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	rgfClass[iClass] = FALSE;

    /*
     * Parse arguments.
     */
    nNumber = 0;
    for ( ;; )
    {
	char arg[MAX_STRING_LENGTH];

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	    break;

	if ( is_number( arg ) )
	{
	    send_to_char("Enter 'Avatar' for level 3's, or 'God' for level 4's and 5's.\n\r",ch);
	    return;
	}
	else
	{
	    /*
	     * Look for classes to turn on.
	     */
	    arg[3]    = '\0';
	    if (   !str_cmp( arg, "imm" ) || !str_cmp( arg, "immortal" )
		|| !str_cmp( arg, "ava" ) || !str_cmp( arg, "avatar"   ) )
	    {
		fClassRestrict = TRUE;
	    }
	    else if ( !str_cmp( arg, "god" ) || !str_cmp( arg, "imp" ) )
	    {
		fImmortalOnly = TRUE;
	    }
	    else
	    {
		send_to_char("Enter 'Avatar' for level 3's, or 'God' for level 4's and 5's.\n\r",ch);
		return;
	    }
	}
    }

    /*
     * Now show matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *wch;
	char const *class;

	/*
	 * Check for match against restrictions.
	 * Don't use trust as that exposes trusted mortals.
	 */
	if ( d->connected != CON_PLAYING || (!can_see( ch, d->character ) && (!IS_SET(ch->act, PLR_WATCHER)) )  )
	    continue;

	wch   = ( d->original != NULL ) ? d->original : d->character;
	if ( wch->level < iLevelLower
	||   wch->level > iLevelUpper
	|| ( fImmortalOnly  && wch->level <  LEVEL_IMMORTAL )
	|| ( fClassRestrict && wch->level != LEVEL_HERO     ) )
	    continue;

	nMatch++;

	/*
	 * Figure out what to print for class.
	 */
	class = class_table[wch->class].who_name;

        if ((IS_HEAD(wch,LOST_HEAD) || IS_EXTRA(wch,EXTRA_OSWITCH)) && wch->pcdata->chobj != NULL)
	{
	    if (wch->pcdata->chobj->pIndexData->vnum == 12)
	    	class = "A Head       ";
	    else if (wch->pcdata->chobj->pIndexData->vnum == 30005)
	    	class = "A Brain      ";
	    else
	    	class = "An Object    ";
	} else
	switch ( wch->level )
	{
	default: break;
	case MAX_LEVEL - 0:
		if (!str_cmp(wch->name,"KaVir")) class = "The Coder    ";
		else class = "Implementor  "; break;
	case MAX_LEVEL - 1: class = "High Judge   "; break;
	case MAX_LEVEL - 2: class = "Judge        "; break;
	case MAX_LEVEL - 3: class = "Enforcer     "; break;
	case MAX_LEVEL - 4: class = "Quest Maker  "; break;
	case MAX_LEVEL - 5: class = "Builder      "; break;
	case MAX_LEVEL - 6:
		     if (wch->race <= 0 ) class = "Avatar       ";
		else if (wch->race <= 4 ) class = "Immortal     ";
		else if (wch->race <= 9 ) class = "Godling      ";
		else if (wch->race <= 14) class = "Demigod      ";
		else if (wch->race <= 19) class = "Lesser God   ";
		else if (wch->race <= 24) class = "Greater God  ";
		else class = "Supreme God  ";
		break;
	case MAX_LEVEL - 7: class = "Mortal       "; break;
	case MAX_LEVEL - 8: class = "Mortal       "; break;
	case MAX_LEVEL - 9: class = "Mortal       "; break;
	}
	/*
	 * Format it up.
	 */
	if ( IS_SET(wch->act, PLR_VAMPIRE) )
	{ strcpy( openb, "<" ); strcpy( closeb, ">" ); }
	else
	{ strcpy( openb, "[" ); strcpy( closeb, "]" ); }
	if ( IS_SET(ch->act, PLR_VAMPIRE) && wch->vampgen == 1 )
	    sprintf( kav, ". %sMaster Vampire%s", openb, closeb );
	else if ( IS_SET(ch->act, PLR_VAMPIRE) &&
		wch->clan != NULL && strlen(wch->clan) > 1 )
	{
	    if (wch->vampgen == 2)
	    	sprintf( kav, ". %sFounder of %s%s", openb, wch->clan, closeb );
	    else if (IS_EXTRA(wch, EXTRA_PRINCE) && wch->sex == SEX_FEMALE)
	    	sprintf( kav, ". %s%s Princess%s", openb, wch->clan, closeb );
	    else if (IS_EXTRA(wch, EXTRA_PRINCE))
	    	sprintf( kav, ". %s%s Prince%s", openb, wch->clan, closeb );
	    else
	    	sprintf( kav, ". %s%s%s", openb, wch->clan, closeb );
	}
	else
	    strcpy( kav, "." );
	sprintf( buf + strlen(buf), "%s %s%s%s\n\r",
	    class,
	    wch->name,
	    wch->pcdata->title,
	    kav );
    }
    if (nMatch == 1)
       sprintf( buf2, "You are the only visible player connected!\n\r");
    else
       sprintf( buf2, "There are a total of %d visible players connected.\n\r", nMatch);
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    send_to_char( buf, ch );
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    send_to_char( buf2, ch );
    send_to_char( "--------------------------------------------------------------------------------\n\r", ch );
    return;
}



void do_inventory( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *portal;
    OBJ_DATA *portal_next;
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *location;
    bool found;

    if (!IS_NPC(ch) && IS_HEAD(ch,LOST_HEAD))
        {send_to_char( "You are not a container.\n\r", ch ); return;}
    else if (!IS_NPC(ch) && IS_EXTRA(ch,EXTRA_OSWITCH))
    {
        if ( !IS_NPC(ch) && (obj = ch->pcdata->chobj) == NULL)
	    {send_to_char( "You are not a container.\n\r", ch ); return;}
	switch ( obj->item_type )
	{
	default:
	    send_to_char( "You are not a container.\n\r", ch );
	    break;

	case ITEM_PORTAL:
	    pRoomIndex = get_room_index(obj->value[0]);
	    location = ch->in_room;
	    if ( pRoomIndex == NULL )
	    {
		send_to_char( "You don't seem to lead anywhere.\n\r", ch );
		return;
	    }
	    char_from_room(ch);
	    char_to_room(ch,pRoomIndex);

	    found = FALSE;
	    for ( portal = ch->in_room->contents; portal != NULL; portal = portal_next )
	    {
		portal_next = portal->next_content;
		if ( ( obj->value[0] == portal->value[3]  )
		    && (obj->value[3] == portal->value[0]) )
		{
		    found = TRUE;
		    if (IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
			!IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
		    {
			REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    		do_look(ch,"auto");
			SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
			break;
		    }
		    else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
			IS_SET(portal->extra_flags, ITEM_SHADOWPLANE) )
		    {
			SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
	    		do_look(ch,"auto");
			REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
			break;
		    }
		    else
		    {
	    		do_look(ch,"auto");
			break;
		    }
	    	}
	    }
	    char_from_room(ch);
	    char_to_room(ch,location);
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "You are empty.\n\r", ch );
		break;
	    }
	    if (obj->value[1] < obj->value[0] / 5)
	    	sprintf( buf, "There is a little %s liquid left in you.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 4)
	    	sprintf( buf, "You contain a small about of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 3)
	    	sprintf( buf, "You're about a third full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0] / 2)
	    	sprintf( buf, "You're about half full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] < obj->value[0])
	    	sprintf( buf, "You are almost full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else if (obj->value[1] == obj->value[0])
	    	sprintf( buf, "You're completely full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    else
	    	sprintf( buf, "Somehow you are MORE than full of %s liquid.\n\r",liq_table[obj->value[2]].liq_color);
	    send_to_char( buf, ch );
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    act( "$p contain:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    break;
	}
	return;
    }
    send_to_char( "You are carrying:\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    send_to_char( "You are using:\n\r", ch );
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	    continue;

	send_to_char( where_name[iWear], ch );
	if ( can_see_obj( ch, obj ) )
	{
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
	else
	{
	    send_to_char( "something.\n\r", ch );
	}
	found = TRUE;
    }

    if ( !found )
	send_to_char( "Nothing.\n\r", ch );

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	for ( obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content )
	{
	    if ( obj2->wear_loc != WEAR_NONE
	    &&   can_see_obj( ch, obj2 )
	    &&   obj1->item_type == obj2->item_type
	    && ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if ( obj2 == NULL )
	{
	    send_to_char( "You aren't wearing anything comparable.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
    }
	    
    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = "You can't compare $p and $P.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0];
	    value2 = obj2->value[0];
	    break;

	case ITEM_WEAPON:
	    value1 = obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if ( msg == NULL )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
    do_help( ch, "diku" );
    return;
}



void do_wizlist( CHAR_DATA *ch, char *argument )
{
    do_help( ch, "wizlist" );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Players near you:\n\r", ch );
	found = FALSE;
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    && ( victim = d->character ) != NULL
	    &&   !IS_NPC(victim)
	    &&   victim->in_room != NULL
	    &&   victim->in_room->area == ch->in_room->area
	    &&   victim->pcdata->chobj == NULL
	    &&   can_see( ch, victim ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    victim->name, victim->in_room->name );
		send_to_char( buf, ch );
	    }
	}
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = char_list; victim != NULL; victim = victim->next )
	{
	    if ( victim->in_room != NULL
	    &&   victim->in_room->area == ch->in_room->area
	    &&   !IS_AFFECTED(victim, AFF_HIDE)
	    &&   !IS_AFFECTED(victim, AFF_SNEAK)
	    &&   can_see( ch, victim )
	    &&   is_name( arg, victim->name ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    PERS(victim, ch), victim->in_room->name );
		send_to_char( buf, ch );
		break;
	    }
	}
	if ( !found )
	    act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;
    int overall;

    one_argument( argument, arg );
    overall = 0;

    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    act( "You examine $N closely, looking for $S weaknesses.", ch, NULL, victim, TO_CHAR );
    act( "$n examine $N closely, looking for $S weaknesses.", ch, NULL, victim, TO_NOTVICT );
    act( "$n examines you closely, looking for your weaknesses.", ch, NULL, victim, TO_VICT );

    if (!IS_NPC(victim)) do_skill(ch,victim->name);

    diff = victim->level - ch->level + victim->hitroll - ch->hitroll;
         if ( diff <= -35 ) {msg = "You are FAR more skilled than $M."; overall = overall + 3;}
    else if ( diff <= -15 ) {msg = "$E is not as skilled as you are."; overall = overall + 2;}
    else if ( diff <=  -5 ) {msg = "$E doesn't seem quite as skilled as you."; overall = overall + 1;}
    else if ( diff <=   5 ) {msg = "You are about as skilled as $M.";}
    else if ( diff <=  15 ) {msg = "$E is slightly more skilled than you are."; overall = overall - 1;}
    else if ( diff <=  35 ) {msg = "$E seems more skilled than you are."; overall = overall -2;}
    else                    {msg = "$E is FAR more skilled than you."; overall = overall - 3;}
    act( msg, ch, NULL, victim, TO_CHAR );

    diff = victim->level - ch->level + victim->damroll - ch->damroll;
         if ( diff <= -35 ) {msg = "You are FAR more powerful than $M."; overall = overall + 3;}
    else if ( diff <= -15 ) {msg = "$E is not as powerful as you are."; overall = overall + 2;}
    else if ( diff <=  -5 ) {msg = "$E doesn't seem quite as powerful as you."; overall = overall + 1;}
    else if ( diff <=   5 ) {msg = "You are about as powerful as $M.";}
    else if ( diff <=  15 ) {msg = "$E is slightly more powerful than you are."; overall = overall - 1;}
    else if ( diff <=  35 ) {msg = "$E seems more powerful than you are."; overall = overall -2;}
    else                    {msg = "$E is FAR more powerful than you."; overall = overall - 3;}
    act( msg, ch, NULL, victim, TO_CHAR );

    diff = ch->hit * 100 / victim->hit;
         if ( diff <=  10 ) {msg = "$E is currently FAR healthier than you are."; overall = overall - 3;}
    else if ( diff <=  50 ) {msg = "$E is currently much healthier than you are."; overall = overall - 2;}
    else if ( diff <=  75 ) {msg = "$E is currently slightly healthier than you are."; overall = overall - 1;}
    else if ( diff <= 125 ) {msg = "$E is currently about as healthy as you are.";}
    else if ( diff <= 200 ) {msg = "You are currently slightly healthier than $M."; overall = overall + 1;}
    else if ( diff <= 500 ) {msg = "You are currently much healthier than $M."; overall = overall + 2;}
    else                    {msg = "You are currently FAR healthier than $M."; overall = overall + 3;}
    act( msg, ch, NULL, victim, TO_CHAR );

    diff = victim->armor - ch->armor;
         if ( diff <= -100) {msg = "$E is FAR better armoured than you."; overall = overall - 3;}
    else if ( diff <= -50 ) {msg = "$E looks much better armoured than you."; overall = overall - 2;}
    else if ( diff <= -25 ) {msg = "$E looks better armoured than you."; overall = overall - 1;}
    else if ( diff <=  25 ) {msg = "$E seems about as well armoured as you.";}
    else if ( diff <=  50 ) {msg = "You are better armoured than $M."; overall = overall + 1;}
    else if ( diff <=  100) {msg = "You are much better armoured than $M."; overall = overall + 2;}
    else                    {msg = "You are FAR better armoured than $M."; overall = overall + 3;}
    act( msg, ch, NULL, victim, TO_CHAR );

    diff = overall;
         if ( diff <= -11 ) msg = "Conclusion: $E would kill you in seconds.";
    else if ( diff <=  -7 ) msg = "Conclusion: You would need a lot of luck to beat $M.";
    else if ( diff <=  -3 ) msg = "Conclusion: You would need some luck to beat $N.";
    else if ( diff <=   2 ) msg = "Conclusion: It would be a very close fight.";
    else if ( diff <=   6 ) msg = "Conclusion: You shouldn't have a lot of trouble defeating $M.";
    else if ( diff <=  10 ) msg = "Conclusion: $N is no match for you.  You can easily beat $M.";
    else                    msg = "Conclusion: $E wouldn't last more than a few seconds against you.";
    act( msg, ch, NULL, victim, TO_CHAR );

    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( isalpha(title[0]) || isdigit(title[0]) )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    if ( strlen(argument) > 50 )
	argument[50] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}



void do_description( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] != '\0' )
    {
	buf[0] = '\0';
	smash_tilde( argument );
	if ( argument[0] == '+' )
	{
	    if ( ch->description != NULL )
		strcat( buf, ch->description );
	    argument++;
	    while ( isspace(*argument) )
		argument++;
	}

	if ( strlen(buf) + strlen(argument) >= MAX_STRING_LENGTH - 2 )
	{
	    send_to_char( "Description too long.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }

    send_to_char( "Your description is:\n\r", ch );
    send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
    return;
}



void do_report( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    char buf[MAX_STRING_LENGTH];
    char hit_str[MAX_INPUT_LENGTH];
    char mana_str[MAX_INPUT_LENGTH];
    char move_str[MAX_INPUT_LENGTH];
    char mhit_str[MAX_INPUT_LENGTH];
    char mmana_str[MAX_INPUT_LENGTH];
    char mmove_str[MAX_INPUT_LENGTH];
    char exp_str[MAX_INPUT_LENGTH];
    sprintf(hit_str, "%d", ch->hit);
    COL_SCALE(hit_str, ch, ch->hit, ch->max_hit);
    sprintf(mana_str, "%d", ch->mana);
    COL_SCALE(mana_str, ch, ch->mana, ch->max_mana);
    sprintf(move_str, "%d", ch->move);
    COL_SCALE(move_str, ch, ch->move, ch->max_move);
    sprintf(exp_str, "%d", ch->exp);
    COL_SCALE(exp_str, ch, ch->exp, 1000);
    sprintf(mhit_str, "%d", ch->max_hit);
    ADD_COLOUR(ch, mhit_str, L_CYAN);
    sprintf(mmana_str, "%d", ch->max_mana);
    ADD_COLOUR(ch, mmana_str, L_CYAN);
    sprintf(mmove_str, "%d", ch->max_move);
    ADD_COLOUR(ch, mmove_str, L_CYAN);
    sprintf( buf,
	"You report: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
	hit_str,  mhit_str,
	mana_str, mmana_str,
	move_str, mmove_str,
	exp_str   );

    send_to_char( buf, ch );

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch == NULL ) continue;
	if ( vch == ch ) continue;
	if ( vch->in_room == NULL ) continue;
	if ( vch->in_room != ch->in_room ) continue;
    	sprintf(hit_str, "%d", ch->hit);
    	COL_SCALE(hit_str, vch, ch->hit, ch->max_hit);
    	sprintf(mana_str, "%d", ch->mana);
    	COL_SCALE(mana_str, vch, ch->mana, ch->max_mana);
    	sprintf(move_str, "%d", ch->move);
    	COL_SCALE(move_str, vch, ch->move, ch->max_move);
    	sprintf(exp_str, "%d", ch->exp);
    	COL_SCALE(exp_str, vch, ch->exp, 1000);
    	sprintf(mhit_str, "%d", ch->max_hit);
    	ADD_COLOUR(vch, mhit_str, L_CYAN);
    	sprintf(mmana_str, "%d", ch->max_mana);
    	ADD_COLOUR(vch, mmana_str, L_CYAN);
    	sprintf(mmove_str, "%d", ch->max_move);
    	ADD_COLOUR(vch, mmove_str, L_CYAN);
    	if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_POLYMORPH))
    	    sprintf( buf, "%s reports: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
	    ch->morph,
	    hit_str,  mhit_str,
	    mana_str, mmana_str,
	    move_str, mmove_str,
	    exp_str   );
        else
    	    sprintf( buf, "%s reports: %s/%s hp %s/%s mana %s/%s mv %s xp.\n\r",
	    IS_NPC(ch) ? capitalize(ch->short_descr) : ch->name,
	    hit_str,  mhit_str,
	    mana_str, mmana_str,
	    move_str, mmove_str,
	    exp_str   );
	buf[0] = UPPER(buf[0]);
    	send_to_char( buf, vch );
    }
    return;
}



void do_practice( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	int col;

	col    = 0;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    if ( ch->level < skill_table[sn].skill_level[ch->class] )
		continue;
	    sprintf( buf, "%18s %3d%%  ",
		skill_table[sn].name, ch->pcdata->learned[sn] );
	    send_to_char( buf, ch );
	    if ( ++col % 3 == 0 )
		send_to_char( "\n\r", ch );
	}

	if ( col % 3 != 0 )
	    send_to_char( "\n\r", ch );

	sprintf( buf, "You have %d exp left.\n\r", ch->exp );
	send_to_char( buf, ch );
    }
    else
    {
	int adept;

	if ( !IS_AWAKE(ch) )
	{
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    return;
	}

	if ( ch->exp <= 0 )
	{
	    send_to_char( "You have no exp left.\n\r", ch );
	    return;
	}

	if ( ( sn = skill_lookup( argument ) ) < 0
	|| ( !IS_NPC(ch)
	&&   ch->level < skill_table[sn].skill_level[ch->class] ) )
	{
	    send_to_char( "You can't practice that.\n\r", ch );
	    return;
	}

	adept = IS_NPC(ch) ? 100 : class_table[ch->class].skill_adept;

	if ( ch->pcdata->learned[sn] >= adept )
	{
	    sprintf( buf, "You are already an adept of %s.\n\r",
		skill_table[sn].name );
	    send_to_char( buf, ch );
	}
	else if ( ch->pcdata->learned[sn] > 0 &&
		(ch->pcdata->learned[sn]/2) > ch->exp )
	{
	    sprintf( buf, "You need %d exp to increase %s any more.\n\r",
		(ch->pcdata->learned[sn]/2),skill_table[sn].name );
	    send_to_char( buf, ch );
	}
	else if ( ch->pcdata->learned[sn] == 0 && ch->exp < 500 )
	{
	    sprintf( buf, "You need 500 exp to increase %s.\n\r",
		skill_table[sn].name );
	    send_to_char( buf, ch );
	}
	else
	{
	    if (ch->pcdata->learned[sn] == 0)
	    {
	    	ch->exp -= 500;
	    	ch->pcdata->learned[sn] += get_curr_int(ch);
	    }
	    else
	    {
	    	ch->exp -= (ch->pcdata->learned[sn]/2);
	    	ch->pcdata->learned[sn] += int_app[get_curr_int(ch)].learn;
	    }
	    if ( ch->pcdata->learned[sn] < adept )
	    {
		act( "You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
	    }
	    else
	    {
		ch->pcdata->learned[sn] = adept;
		act( "You are now an adept of $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
	    }
	}
    }
    return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if ( wimpy > ch->max_hit )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
    send_to_char( buf, ch );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: password <old> <new>.\n\r", ch );
	return;
    }

    if ( strcmp( arg1, ch->pcdata->pwd ) &&
         strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }

    if ( strlen(arg2) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    if (!IS_EXTRA(ch,EXTRA_NEWPASS)) SET_BIT(ch->extra,EXTRA_NEWPASS);
    save_char_obj( ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_socials( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;

    col = 0;

    for ( iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++ )
    {
	sprintf( buf, "%-12s", social_table[iSocial].name );
	send_to_char( buf, ch );
	if ( ++col % 6 == 0 )
	    send_to_char( "\n\r", ch );
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );

    return;
}



void do_spells( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;
    int col;

    col = 0;
    for ( sn = 0; sn < MAX_SKILL && skill_table[sn].name != NULL; sn++ )
    {
	sprintf( buf, "%-12s", skill_table[sn].name );
	send_to_char( buf, ch );
	if ( ++col % 6 == 0 )
	    send_to_char( "\n\r", ch );
    }

    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}



/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
 
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level == 0
        &&   cmd_table[cmd].level <= get_trust( ch ) )
	{
	    sprintf( buf, "%-12s", cmd_table[cmd].name );
	    send_to_char( buf, ch );
	    if ( ++col % 6 == 0 )
		send_to_char( "\n\r", ch );
	}
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}



void do_channels( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
	{
	    send_to_char( "You are silenced.\n\r", ch );
	    return;
	}

	send_to_char( "Channels:", ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_AUCTION)
	    ? " +AUCTION"
	    : " -auction",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_CHAT)
	    ? " +CHAT"
	    : " -chat",
	    ch );

#if 0
	send_to_char( !IS_SET(ch->deaf, CHANNEL_HACKER)
	    ? " +HACKER"
	    : " -hacker",
	    ch );
#endif

	if ( IS_IMMORTAL(ch) )
	{
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_IMMTALK)
		? " +IMMTALK"
		: " -immtalk",
		ch );
	}

	send_to_char( !IS_SET(ch->deaf, CHANNEL_MUSIC)
	    ? " +MUSIC"
	    : " -music",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_QUESTION)
	    ? " +QUESTION"
	    : " -question",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_SHOUT)
	    ? " +SHOUT"
	    : " -shout",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_YELL)
	    ? " +YELL"
	    : " -yell",
	    ch );

	send_to_char( ".\n\r", ch );
    }
    else
    {
	bool fClear;
	int bit;

	     if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
	    send_to_char( "Channels -channel or +channel?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;
        else if ( !str_cmp( arg+1, "chat"     ) ) bit = CHANNEL_CHAT;
#if 0
	else if ( !str_cmp( arg+1, "hacker"   ) ) bit = CHANNEL_HACKER;
#endif
	else if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
	else if ( !str_cmp( arg+1, "music"    ) ) bit = CHANNEL_MUSIC;
	else if ( !str_cmp( arg+1, "question" ) ) bit = CHANNEL_QUESTION;
	else if ( !str_cmp( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
	else if ( !str_cmp( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
	else
	{
	    send_to_char( "Set or clear which channel?\n\r", ch );
	    return;
	}

	if ( fClear )
	    REMOVE_BIT (ch->deaf, bit);
	else
	    SET_BIT    (ch->deaf, bit);

	send_to_char( "Ok.\n\r", ch );
    }

    return;
}



/*
 * Contributed by Grodyn.
 */
void do_config( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "[ Keyword  ] Option\n\r", ch );

	send_to_char(  IS_SET(ch->act, PLR_ANSI)
            ? "[+ANSI     ] You have ansi colour on.\n\r"
	    : "[-ansi     ] You have ansi colour off.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_AUTOEXIT)
            ? "[+AUTOEXIT ] You automatically see exits.\n\r"
	    : "[-autoexit ] You don't automatically see exits.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_AUTOLOOT)
	    ? "[+AUTOLOOT ] You automatically loot corpses.\n\r"
	    : "[-autoloot ] You don't automatically loot corpses.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_AUTOSAC)
	    ? "[+AUTOSAC  ] You automatically sacrifice corpses.\n\r"
	    : "[-autosac  ] You don't automatically sacrifice corpses.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_BLANK)
	    ? "[+BLANK    ] You have a blank line before your prompt.\n\r"
	    : "[-blank    ] You have no blank line before your prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_BRIEF)
	    ? "[+BRIEF    ] You see brief descriptions.\n\r"
	    : "[-brief    ] You see long descriptions.\n\r"
	    , ch );
         
	send_to_char(  IS_SET(ch->act, PLR_COMBINE)
	    ? "[+COMBINE  ] You see object lists in combined format.\n\r"
	    : "[-combine  ] You see object lists in single format.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_PROMPT)
	    ? "[+PROMPT   ] You have a prompt.\n\r"
	    : "[-prompt   ] You don't have a prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_TELNET_GA)
	    ? "[+TELNETGA ] You receive a telnet GA sequence.\n\r"
	    : "[-telnetga ] You don't receive a telnet GA sequence.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_SILENCE)
	    ? "[+SILENCE  ] You are silenced.\n\r"
	    : ""
	    , ch );

	send_to_char( !IS_SET(ch->act, PLR_NO_EMOTE)
	    ? ""
	    : "[-emote    ] You can't emote.\n\r"
	    , ch );

	send_to_char( !IS_SET(ch->act, PLR_NO_TELL)
	    ? ""
	    : "[-tell     ] You can't use 'tell'.\n\r"
	    , ch );
    }
    else
    {
	bool fSet;
	int bit;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char( "Config -option or +option?\n\r", ch );
	    return;
	}

             if ( !str_cmp( arg+1, "ansi"     ) ) bit = PLR_ANSI;
        else if ( !str_cmp( arg+1, "autoexit" ) ) bit = PLR_AUTOEXIT;
	else if ( !str_cmp( arg+1, "autoloot" ) ) bit = PLR_AUTOLOOT;
	else if ( !str_cmp( arg+1, "autosac"  ) ) bit = PLR_AUTOSAC;
	else if ( !str_cmp( arg+1, "blank"    ) ) bit = PLR_BLANK;
	else if ( !str_cmp( arg+1, "brief"    ) ) bit = PLR_BRIEF;
	else if ( !str_cmp( arg+1, "combine"  ) ) bit = PLR_COMBINE;
        else if ( !str_cmp( arg+1, "prompt"   ) ) bit = PLR_PROMPT;
	else if ( !str_cmp( arg+1, "telnetga" ) ) bit = PLR_TELNET_GA;
	else
	{
	    send_to_char( "Config which option?\n\r", ch );
	    return;
	}

	if ( fSet )
	    SET_BIT    (ch->act, bit);
	else
	    REMOVE_BIT (ch->act, bit);

	send_to_char( "Ok.\n\r", ch );
    }

    return;
}

void do_ansi( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_ANSI)) do_config(ch,"-ansi");
    else do_config(ch,"+ansi");
    return;
}

void do_autoexit( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_AUTOEXIT)) do_config(ch,"-autoexit");
    else do_config(ch,"+autoexit");
    return;
}

void do_autoloot( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_AUTOLOOT)) do_config(ch,"-autoloot");
    else do_config(ch,"+autoloot");
    return;
}

void do_autosac( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_AUTOSAC)) do_config(ch,"-autosac");
    else do_config(ch,"+autosac");
    return;
}

void do_blank( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_BLANK)) do_config(ch,"-blank");
    else do_config(ch,"+blank");
    return;
}

void do_brief( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) ) return;
    if (IS_SET(ch->act, PLR_BRIEF)) do_config(ch,"-brief");
    else do_config(ch,"+brief");
    return;
}

void do_diagnose( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg  [MAX_INPUT_LENGTH];
    int teeth = 0;
    int ribs = 0;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if (arg == '\0')
    {
	send_to_char("Who do you wish to diagnose?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char("Nobody here by that name.\n\r",ch);
	return;
    }
    act("$n examines $N carefully, diagnosing $S injuries.",ch,NULL,victim,TO_NOTVICT);
    act("$n examines you carefully, diagnosing your injuries.",ch,NULL,victim,TO_VICT);
    act("Your diagnoses of $N reveals the following...",ch,NULL,victim,TO_CHAR);
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    if ( ( victim->loc_hp[0] + victim->loc_hp[1] + victim->loc_hp[2] + 
	   victim->loc_hp[3] + victim->loc_hp[4] + victim->loc_hp[5] +
	   victim->loc_hp[6] ) == 0 )
    {
	act("$N has no apparent injuries.",ch,NULL,victim,TO_CHAR);
	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	return;
    }
    /* Check head */
    if (IS_HEAD(victim,LOST_EYE_L) && IS_HEAD(victim,LOST_EYE_R))
	act("$N has lost both of $S eyes.",ch,NULL,victim,TO_CHAR);
    else if (IS_HEAD(victim,LOST_EYE_L))
	act("$N has lost $S left eye.",ch,NULL,victim,TO_CHAR);
    else if (IS_HEAD(victim,LOST_EYE_R))
	act("$N has lost $S right eye.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,LOST_EAR_L) && IS_HEAD(victim,LOST_EAR_R))
	act("$N has lost both of $S ears.",ch,NULL,victim,TO_CHAR);
    else if (IS_HEAD(victim,LOST_EAR_L))
	act("$N has lost $S left ear.",ch,NULL,victim,TO_CHAR);
    else if (IS_HEAD(victim,LOST_EAR_R))
	act("$N has lost $S right ear.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,LOST_NOSE))
	act("$N has lost $S nose.",ch,NULL,victim,TO_CHAR);
    else if (IS_HEAD(victim,BROKEN_NOSE))
	act("$N has got a broken nose.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,BROKEN_JAW))
	act("$N has got a broken jaw.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,LOST_HEAD))
    {
	act("$N has had $S head cut off.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_HEAD))
	    act("...Blood is spurting from the stump of $S neck.",ch,NULL,victim,TO_CHAR);
    }
    else
    {
	if (IS_BODY(victim,BROKEN_NECK))
	    act("$N has got a broken neck.",ch,NULL,victim,TO_CHAR);
	if (IS_BODY(victim,CUT_THROAT))
	{
	    act("$N has had $S throat cut open.",ch,NULL,victim,TO_CHAR);
	    if (IS_BLEEDING(victim,BLEEDING_THROAT))
		act("...Blood is pouring from the wound.",ch,NULL,victim,TO_CHAR);
	}
    }
    if (IS_HEAD(victim,BROKEN_SKULL))
	act("$N has got a broken skull.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,LOST_TOOTH_1 )) teeth += 1;
    if (IS_HEAD(victim,LOST_TOOTH_2 )) teeth += 2;
    if (IS_HEAD(victim,LOST_TOOTH_4 )) teeth += 4;
    if (IS_HEAD(victim,LOST_TOOTH_8 )) teeth += 8;
    if (IS_HEAD(victim,LOST_TOOTH_16)) teeth += 16;
    if (teeth > 0)
    {
	sprintf(buf,"$N has had %d teeth knocked out.",teeth);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    if (IS_HEAD(victim,LOST_TONGUE))
	act("$N has had $S tongue ripped out.",ch,NULL,victim,TO_CHAR);
    if (IS_HEAD(victim,LOST_HEAD))
    {
    	send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    	return;
    }
    /* Check body */
    if (IS_BODY(victim,BROKEN_RIBS_1 )) ribs += 1;
    if (IS_BODY(victim,BROKEN_RIBS_2 )) ribs += 2;
    if (IS_BODY(victim,BROKEN_RIBS_4 )) ribs += 4;
    if (IS_BODY(victim,BROKEN_RIBS_8 )) ribs += 8;
    if (IS_BODY(victim,BROKEN_RIBS_16)) ribs += 16;
    if (ribs > 0)
    {
	sprintf(buf,"$N has got %d broken ribs.",ribs);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    if (IS_BODY(victim,BROKEN_SPINE))
	act("$N has got a broken spine.",ch,NULL,victim,TO_CHAR);
    /* Check arms */
    check_left_arm(ch,victim);
    check_right_arm(ch,victim);
    check_left_leg(ch,victim);
    check_right_leg(ch,victim);
    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
    return;
}

void check_left_arm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf    [MAX_STRING_LENGTH];
    char finger [10];
    int fingers = 0;

    if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
    {
	act("$N has lost both of $S arms.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_ARM_L) && IS_BLEEDING(victim,BLEEDING_ARM_R))
	    act("...Blood is spurting from both stumps.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_ARM_L))
	    act("...Blood is spurting from the left stump.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_ARM_R))
	    act("...Blood is spurting from the right stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_ARM_L(victim,LOST_ARM))
    {
	act("$N has lost $S left arm.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_ARM_L))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_ARM_L(victim,BROKEN_ARM) && IS_ARM_R(victim,BROKEN_ARM))
	act("$N arms are both broken.",ch,NULL,victim,TO_CHAR);
    else if (IS_ARM_L(victim,BROKEN_ARM))
	act("$N's left arm is broken.",ch,NULL,victim,TO_CHAR);
    if (IS_ARM_L(victim,LOST_HAND) && IS_ARM_R(victim,LOST_HAND))
    {
	act("$N has lost both of $S hands.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_HAND_L) && IS_BLEEDING(victim,BLEEDING_HAND_R))
	    act("...Blood is spurting from both stumps.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_HAND_L))
	    act("...Blood is spurting from the left stump.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_HAND_R))
	    act("...Blood is spurting from the right stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_ARM_L(victim,LOST_HAND))
    {
	act("$N has lost $S left hand.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_HAND_L))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_ARM_L(victim,LOST_FINGER_I)) fingers += 1;
    if (IS_ARM_L(victim,LOST_FINGER_M)) fingers += 1;
    if (IS_ARM_L(victim,LOST_FINGER_R)) fingers += 1;
    if (IS_ARM_L(victim,LOST_FINGER_L)) fingers += 1;
    if (fingers == 1) sprintf(finger,"finger");
    else sprintf(finger,"fingers");
    if (fingers > 0 && IS_ARM_L(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has lost %d %s and $S thumb from $S left hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (fingers > 0)
    {
	sprintf(buf,"$N has lost %d %s from $S left hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_ARM_L(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has lost the thumb from $S left hand.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    fingers = 0;
    if (IS_ARM_L(victim,BROKEN_FINGER_I) && !IS_ARM_L(victim,LOST_FINGER_I)) fingers += 1;
    if (IS_ARM_L(victim,BROKEN_FINGER_M) && !IS_ARM_L(victim,LOST_FINGER_M)) fingers += 1;
    if (IS_ARM_L(victim,BROKEN_FINGER_R) && !IS_ARM_L(victim,LOST_FINGER_R)) fingers += 1;
    if (IS_ARM_L(victim,BROKEN_FINGER_L) && !IS_ARM_L(victim,LOST_FINGER_L)) fingers += 1;
    if (fingers == 1) sprintf(finger,"finger");
    else sprintf(finger,"fingers");
    if (fingers > 0 && IS_ARM_L(victim,BROKEN_THUMB) && !IS_ARM_L(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has broken %d %s and $S thumb on $S left hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (fingers > 0)
    {
	sprintf(buf,"$N has broken %d %s on $S left hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_ARM_L(victim,BROKEN_THUMB) && !IS_ARM_L(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has broken the thumb on $S left hand.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    return;
}

void check_right_arm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf    [MAX_STRING_LENGTH];
    char finger [10];
    int fingers = 0;

    if (IS_ARM_L(victim,LOST_ARM) && IS_ARM_R(victim,LOST_ARM))
	return;
    if (IS_ARM_R(victim,LOST_ARM))
    {
	act("$N has lost $S right arm.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_ARM_R))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (!IS_ARM_L(victim,BROKEN_ARM) && IS_ARM_R(victim,BROKEN_ARM))
	act("$N's right arm is broken.",ch,NULL,victim,TO_CHAR);
    if (IS_ARM_L(victim,LOST_HAND) && IS_ARM_R(victim,LOST_HAND))
	return;
    if (IS_ARM_R(victim,LOST_HAND))
    {
	act("$N has lost $S right hand.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_HAND_R))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_ARM_R(victim,LOST_FINGER_I)) fingers += 1;
    if (IS_ARM_R(victim,LOST_FINGER_M)) fingers += 1;
    if (IS_ARM_R(victim,LOST_FINGER_R)) fingers += 1;
    if (IS_ARM_R(victim,LOST_FINGER_L)) fingers += 1;
    if (fingers == 1) sprintf(finger,"finger");
    else sprintf(finger,"fingers");
    if (fingers > 0 && IS_ARM_R(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has lost %d %s and $S thumb from $S right hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (fingers > 0)
    {
	sprintf(buf,"$N has lost %d %s from $S right hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_ARM_R(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has lost the thumb from $S right hand.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    fingers = 0;
    if (IS_ARM_R(victim,BROKEN_FINGER_I) && !IS_ARM_R(victim,LOST_FINGER_I)) fingers += 1;
    if (IS_ARM_R(victim,BROKEN_FINGER_M) && !IS_ARM_R(victim,LOST_FINGER_M)) fingers += 1;
    if (IS_ARM_R(victim,BROKEN_FINGER_R) && !IS_ARM_R(victim,LOST_FINGER_R)) fingers += 1;
    if (IS_ARM_R(victim,BROKEN_FINGER_L) && !IS_ARM_R(victim,LOST_FINGER_L)) fingers += 1;
    if (fingers == 1) sprintf(finger,"finger");
    else sprintf(finger,"fingers");
    if (fingers > 0 && IS_ARM_R(victim,BROKEN_THUMB) && !IS_ARM_R(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has broken %d %s and $S thumb on $S right hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (fingers > 0)
    {
	sprintf(buf,"$N has broken %d %s on $S right hand.",fingers,finger);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_ARM_R(victim,BROKEN_THUMB) && !IS_ARM_R(victim,LOST_THUMB))
    {
	sprintf(buf,"$N has broken the thumb on $S right hand.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    return;
}

void check_left_leg( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf    [MAX_STRING_LENGTH];
    char toe [10];
    int toes = 0;

    if (IS_LEG_L(victim,LOST_LEG) && IS_LEG_R(victim,LOST_LEG))
    {
	act("$N has lost both of $S legs.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_LEG_L) && IS_BLEEDING(victim,BLEEDING_LEG_R))
	    act("...Blood is spurting from both stumps.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_LEG_L))
	    act("...Blood is spurting from the left stump.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_LEG_R))
	    act("...Blood is spurting from the right stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_LEG_L(victim,LOST_LEG))
    {
	act("$N has lost $S left leg.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_LEG_L))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_LEG_L(victim,BROKEN_LEG) && IS_LEG_R(victim,BROKEN_LEG))
	act("$N legs are both broken.",ch,NULL,victim,TO_CHAR);
    else if (IS_LEG_L(victim,BROKEN_LEG))
	act("$N's left leg is broken.",ch,NULL,victim,TO_CHAR);
    if (IS_LEG_L(victim,LOST_FOOT) && IS_LEG_R(victim,LOST_FOOT))
    {
	act("$N has lost both of $S feet.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_L) && IS_BLEEDING(victim,BLEEDING_FOOT_R))
	    act("...Blood is spurting from both stumps.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_FOOT_L))
	    act("...Blood is spurting from the left stump.",ch,NULL,victim,TO_CHAR);
	else if (IS_BLEEDING(victim,BLEEDING_FOOT_R))
	    act("...Blood is spurting from the right stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_LEG_L(victim,LOST_FOOT))
    {
	act("$N has lost $S left foot.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_L))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_LEG_L(victim,LOST_TOE_A)) toes += 1;
    if (IS_LEG_L(victim,LOST_TOE_B)) toes += 1;
    if (IS_LEG_L(victim,LOST_TOE_C)) toes += 1;
    if (IS_LEG_L(victim,LOST_TOE_D)) toes += 1;
    if (toes == 1) sprintf(toe,"toe");
    else sprintf(toe,"toes");
    if (toes > 0 && IS_LEG_L(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has lost %d %s and $S big toe from $S left foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (toes > 0)
    {
	sprintf(buf,"$N has lost %d %s from $S left foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_LEG_L(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has lost the big toe from $S left foot.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    toes = 0;
    if (IS_LEG_L(victim,BROKEN_TOE_A) && !IS_LEG_L(victim,LOST_TOE_A)) toes += 1;
    if (IS_LEG_L(victim,BROKEN_TOE_B) && !IS_LEG_L(victim,LOST_TOE_B)) toes += 1;
    if (IS_LEG_L(victim,BROKEN_TOE_C) && !IS_LEG_L(victim,LOST_TOE_C)) toes += 1;
    if (IS_LEG_L(victim,BROKEN_TOE_D) && !IS_LEG_L(victim,LOST_TOE_D)) toes += 1;
    if (toes == 1) sprintf(toe,"toe");
    else sprintf(toe,"toes");
    if (toes > 0 && IS_LEG_L(victim,BROKEN_TOE_BIG) && !IS_LEG_L(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has broken %d %s and $S big toe from $S left foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (toes > 0)
    {
	sprintf(buf,"$N has broken %d %s on $S left foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_LEG_L(victim,BROKEN_TOE_BIG) && !IS_LEG_L(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has broken the big toe on $S left foot.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    return;
}

void check_right_leg( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf    [MAX_STRING_LENGTH];
    char toe [10];
    int toes = 0;

    if (IS_LEG_L(victim,LOST_LEG) && IS_LEG_R(victim,LOST_LEG))
	return;
    if (IS_LEG_R(victim,LOST_LEG))
    {
	act("$N has lost $S right leg.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_LEG_R))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (!IS_LEG_L(victim,BROKEN_LEG) && IS_LEG_R(victim,BROKEN_LEG))
	act("$N's right leg is broken.",ch,NULL,victim,TO_CHAR);
    if (IS_LEG_L(victim,LOST_FOOT) && IS_LEG_R(victim,LOST_FOOT))
	return;
    if (IS_LEG_R(victim,LOST_FOOT))
    {
	act("$N has lost $S right foot.",ch,NULL,victim,TO_CHAR);
	if (IS_BLEEDING(victim,BLEEDING_FOOT_R))
	    act("...Blood is spurting from the stump.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (IS_LEG_R(victim,LOST_TOE_A)) toes += 1;
    if (IS_LEG_R(victim,LOST_TOE_B)) toes += 1;
    if (IS_LEG_R(victim,LOST_TOE_C)) toes += 1;
    if (IS_LEG_R(victim,LOST_TOE_D)) toes += 1;
    if (toes == 1) sprintf(toe,"toe");
    else sprintf(toe,"toes");
    if (toes > 0 && IS_LEG_R(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has lost %d %s and $S big toe from $S right foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (toes > 0)
    {
	sprintf(buf,"$N has lost %d %s from $S right foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_LEG_R(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has lost the big toe from $S right foot.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    toes = 0;
    if (IS_LEG_R(victim,BROKEN_TOE_A) && !IS_LEG_R(victim,LOST_TOE_A)) toes += 1;
    if (IS_LEG_R(victim,BROKEN_TOE_B) && !IS_LEG_R(victim,LOST_TOE_B)) toes += 1;
    if (IS_LEG_R(victim,BROKEN_TOE_C) && !IS_LEG_R(victim,LOST_TOE_C)) toes += 1;
    if (IS_LEG_R(victim,BROKEN_TOE_D) && !IS_LEG_R(victim,LOST_TOE_D)) toes += 1;
    if (toes == 1) sprintf(toe,"toe");
    else sprintf(toe,"toes");
    if (toes > 0 && IS_LEG_R(victim,BROKEN_TOE_BIG) && !IS_LEG_R(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has broken %d %s and $S big toe on $S right foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (toes > 0)
    {
	sprintf(buf,"$N has broken %d %s on $S right foot.",toes,toe);
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    else if (IS_LEG_R(victim,BROKEN_TOE_BIG) && !IS_LEG_R(victim,LOST_TOE_BIG))
    {
	sprintf(buf,"$N has broken the big toe on $S right foot.");
	act(buf,ch,NULL,victim,TO_CHAR);
    }
    return;
}

void obj_score( CHAR_DATA *ch, OBJ_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    int itemtype;

    sprintf( buf,"You are %s.\n\r",obj->short_descr);
    send_to_char( buf, ch );

    sprintf( buf,"Type %s, Extra flags %s.\n\r",item_type_name(obj),
	extra_bit_name(obj->extra_flags));
    send_to_char( buf, ch );

    sprintf( buf,"You weigh %d pounds and are worth %d gold coins.\n\r",obj->weight,obj->cost);
    send_to_char( buf, ch );

    if (obj->questmaker != NULL && strlen(obj->questmaker) > 1 &&
	obj->questowner != NULL && strlen(obj->questowner) > 1)
    {
	sprintf( buf, "You were created by %s, and are owned by %s.\n\r", obj->questmaker,obj->questowner );
	send_to_char( buf, ch );
    }
    else if (obj->questmaker != NULL && strlen(obj->questmaker) > 1)
    {
	sprintf( buf, "You were created by %s.\n\r", obj->questmaker );
	send_to_char( buf, ch );
    }
    else if (obj->questowner != NULL && strlen(obj->questowner) > 1)
    {
	sprintf( buf, "You are owned by %s.\n\r", obj->questowner );
	send_to_char( buf, ch );
    }

    switch ( obj->item_type )
    {
    case ITEM_SCROLL: 
    case ITEM_POTION:
	sprintf( buf, "You contain level %d spells of:", obj->value[0] );
	send_to_char( buf, ch );

	if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[1]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[2]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	send_to_char( ".\n\r", ch );
	break;

    case ITEM_QUEST:
	sprintf( buf, "Your quest point value is %d.\n\r", obj->value[0] );
	send_to_char( buf, ch );
	break;

    case ITEM_WAND: 
    case ITEM_STAFF: 
	sprintf( buf, "You have %d(%d) charges of level %d",
	    obj->value[1], obj->value[2], obj->value[0] );
	send_to_char( buf, ch );

	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	send_to_char( ".\n\r", ch );
	break;
      
    case ITEM_WEAPON:
	sprintf( buf, "You inflict %d to %d damage in combat (average %d).\n\r",
	    obj->value[1], obj->value[2],
	    ( obj->value[1] + obj->value[2] ) / 2 );
	send_to_char( buf, ch );

	if (obj->value[0] >= 1000)
	    itemtype = obj->value[0] - ((obj->value[0] / 1000) * 1000);
	else
	    itemtype = obj->value[0];

	if (itemtype > 0)
	{
	    if (obj->level < 10)
		sprintf(buf,"You are a minor spell weapon.\n\r");
	    else if (obj->level < 20)
		sprintf(buf,"You are a lesser spell weapon.\n\r");
	    else if (obj->level < 30)
		sprintf(buf,"You are an average spell weapon.\n\r");
	    else if (obj->level < 40)
		sprintf(buf,"You are a greater spell weapon.\n\r");
	    else if (obj->level < 50)
		sprintf(buf,"You are a major spell weapon.\n\r");
	    else
		sprintf(buf,"You are a supreme spell weapon.\n\r");
	    send_to_char(buf,ch);
	}

	if (itemtype == 1)
	    sprintf (buf, "You are dripping with corrosive acid.\n\r");
	else if (itemtype == 4)
	    sprintf (buf, "You radiate an aura of darkness.\n\r");
	else if (itemtype == 30)
	    sprintf (buf, "You are the bane of all evil.\n\r");
	else if (itemtype == 34)
	    sprintf (buf, "You drink the souls of your victims.\n\r");
	else if (itemtype == 37)
	    sprintf (buf, "You have been tempered in hellfire.\n\r");
	else if (itemtype == 48)
	    sprintf (buf, "You crackle with sparks of lightning.\n\r");
	else if (itemtype == 53)
	    sprintf (buf, "You are dripping with a dark poison.\n\r");
	else if (itemtype > 0)
	    sprintf (buf, "You have been imbued with the power of %s.\n\r",skill_table[itemtype].name);
	if (itemtype > 0)
	    send_to_char( buf, ch );

	if (obj->value[0] >= 1000)
	    itemtype = obj->value[0] / 1000;
	else
	    break;

	if (itemtype == 4 || itemtype == 1)
	    sprintf (buf, "You radiate an aura of darkness.\n\r");
	else if (itemtype == 27 || itemtype == 2)
	    sprintf (buf, "You allow your wielder to see invisible things.\n\r");
	else if (itemtype == 39 || itemtype == 3)
	    sprintf (buf, "You grant your wielder the power of flight.\n\r");
	else if (itemtype == 45 || itemtype == 4)
	    sprintf (buf, "You allow your wielder to see in the dark.\n\r");
	else if (itemtype == 46 || itemtype == 5)
	    sprintf (buf, "You render your wielder invisible to the human eye.\n\r");
	else if (itemtype == 52 || itemtype == 6)
	    sprintf (buf, "You allow your wielder to walk through solid doors.\n\r");
	else if (itemtype == 54 || itemtype == 7)
	    sprintf (buf, "You protect your wielder from evil.\n\r");
	else if (itemtype == 57 || itemtype == 8)
	    sprintf (buf, "You protect your wielder in combat.\n\r");
	else if (itemtype == 9)
	    sprintf (buf, "You allow your wielder to walk in complete silence.\n\r");
	else if (itemtype == 10)
	    sprintf (buf, "You surround your wielder with a shield of lightning.\n\r");
	else if (itemtype == 11)
	    sprintf (buf, "You surround your wielder with a shield of fire.\n\r");
	else if (itemtype == 12)
	    sprintf (buf, "You surround your wielder with a shield of ice.\n\r");
	else if (itemtype == 13)
	    sprintf (buf, "You surround your wielder with a shield of acid.\n\r");
	else
	    sprintf (buf, "You are bugged...please report it.\n\r");
	if (itemtype > 0)
	    send_to_char( buf, ch );
	break;

    case ITEM_ARMOR:
	sprintf( buf, "Your armor class is %d.\n\r", obj->value[0] );
	send_to_char( buf, ch );
	if (obj->value[3] < 1)
	    break;
	if (obj->value[3] == 4 || obj->value[3] == 1)
	    sprintf (buf, "You radiate an aura of darkness.\n\r");
	else if (obj->value[3] == 27 || obj->value[3] == 2)
	    sprintf (buf, "You allow your wearer to see invisible things.\n\r");
	else if (obj->value[3] == 39 || obj->value[3] == 3)
	    sprintf (buf, "You grant your wearer the power of flight.\n\r");
	else if (obj->value[3] == 45 || obj->value[3] == 4)
	    sprintf (buf, "You allow your wearer to see in the dark.\n\r");
	else if (obj->value[3] == 46 || obj->value[3] == 5)
	    sprintf (buf, "You render your wearer invisible to the human eye.\n\r");
	else if (obj->value[3] == 52 || obj->value[3] == 6)
	    sprintf (buf, "You allow your wearer to walk through solid doors.\n\r");
	else if (obj->value[3] == 54 || obj->value[3] == 7)
	    sprintf (buf, "You protect your wearer from evil.\n\r");
	else if (obj->value[3] == 57 || obj->value[3] == 8)
	    sprintf (buf, "You protect your wearer in combat.\n\r");
	else if (obj->value[3] == 9)
	    sprintf (buf, "You allow your wearer to walk in complete silence.\n\r");
	else if (obj->value[3] == 10)
	    sprintf (buf, "You surround your wearer with a shield of lightning.\n\r");
	else if (obj->value[3] == 11)
	    sprintf (buf, "You surround your wearer with a shield of fire.\n\r");
	else if (obj->value[3] == 12)
	    sprintf (buf, "You surround your wearer with a shield of ice.\n\r");
	else if (obj->value[3] == 13)
	    sprintf (buf, "You surround your wearer with a shield of acid.\n\r");
	else
	    sprintf (buf, "You are bugged...please report it.\n\r");
	if (obj->value[3] > 0)
	    send_to_char( buf, ch );
	break;
    }

    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "You affect %s by %d.\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
	}
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "You affect %s by %d.\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
	}
    }

    return;
}
