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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "multiclass.c"

AFFECT_DATA *		affect_free;

/*
 * Local functions.
 */
int	spell_duration	args( ( CHAR_DATA *victim, int sn ) );
void 	perm_spell	args( ( CHAR_DATA *victim, int sn ) );
void    affect_modify   args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
void    affect_modify2  args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
bool	is_colcode	args( ( char code ) );
int	mmlvl_mana	args( ( CHAR_DATA *ch, int sn ) );

/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust( CHAR_DATA *ch )
{
    if ( ch->desc && ch->desc->original )
	ch = ch->desc->original;

/*    if ( IS_CODER( ch ) && ch->trust < 100000 )
      return 100000;*/

    if ( ch->trust != 0 )
	return ch->trust;

    if ( IS_NPC( ch ) && ch->level >= LEVEL_HERO )
	return LEVEL_HERO - 1;
    else
	return ch->level;
}



/*
 * Retrieve a character's age.
 */
int get_age( CHAR_DATA *ch )
{
    return 17 + ( ch->played + (int) ( current_time - ch->logon ) ) / 14400;

    /* 14400 assumes 30 second hours, 24 hours a day, 20 day - Kahn */
}



/*
 * Retrieve character's current strength.
 */
int get_curr_str( CHAR_DATA *ch )
{
    int max;

    if ( IS_NPC( ch ) )
	return 13;

    if ( class_table[prime_class(ch)].attr_prime == APPLY_STR )
	max = 25 + (get_race_data(ch->race))->mstr;
    else
	max = 22 + (get_race_data(ch->race))->mstr;

    return URANGE( 3, ch->pcdata->perm_str + ch->pcdata->mod_str, max );
}



/*
 * Retrieve character's current intelligence.
 */
int get_curr_int( CHAR_DATA *ch )
{
    int max;

    if ( IS_NPC( ch ) )
	return 13;

    if ( class_table[prime_class(ch)].attr_prime == APPLY_INT )
	max = 25 + (get_race_data(ch->race))->mint;
    else
	max = 22 + (get_race_data(ch->race))->mint;

    return URANGE( 3, ch->pcdata->perm_int + ch->pcdata->mod_int, max );
}



/*
 * Retrieve character's current wisdom.
 */
int get_curr_wis( CHAR_DATA *ch )
{
    int max;

    if ( IS_NPC( ch ) )
	return 13;

    if ( class_table[prime_class(ch)].attr_prime == APPLY_WIS )
	max = 25 + (get_race_data(ch->race))->mwis;
    else
	max = 22 + (get_race_data(ch->race))->mwis;

    return URANGE( 3, ch->pcdata->perm_wis + ch->pcdata->mod_wis, max );
}



/*
 * Retrieve character's current dexterity.
 */
int get_curr_dex( CHAR_DATA *ch )
{
    int max;

    if ( IS_NPC( ch ) )
	return 13;

    if ( class_table[prime_class(ch)].attr_prime == APPLY_DEX )
	max = 25 + (get_race_data(ch->race))->mdex;
    else
	max = 22 + (get_race_data(ch->race))->mdex;

    return URANGE( 3, ch->pcdata->perm_dex + ch->pcdata->mod_dex, max );
}



/*
 * Retrieve character's current constitution.
 */
int get_curr_con( CHAR_DATA *ch )
{
    int max;

    if ( IS_NPC( ch ) )
	return 13;

    if ( class_table[prime_class(ch)].attr_prime == APPLY_CON )
	max = 25 + (get_race_data(ch->race))->mcon;
    else
	max = 22 + (get_race_data(ch->race))->mcon;

    return URANGE( 3, ch->pcdata->perm_con + ch->pcdata->mod_con, max );
}



/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n( CHAR_DATA *ch )
{
    if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
	return 1000;

    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
	return 0;

    return MAX_WEAR + 2 * get_curr_dex( ch ) + get_curr_str( ch ) * 2;
}



/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w( CHAR_DATA *ch )
{
    int max_weight = 0;

    if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
	return 1000000;

    if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
	return 0;

/* Angi: Make max carry weight less than the weight of your coins. */

    max_weight = ( ( str_app[get_curr_str( ch )].carry ) -
		 ( ch->money.gold/1000 + ch->money.silver/1000 + ch->money.copper/1000 ) );
    if ( max_weight < 0 )
      max_weight = 0;

   return max_weight;
    
/*    return str_app[get_curr_str( ch )].carry; */

}



/*
 * See if a string is one of the names of an object.
 * New is_name by Alander.
 */

bool is_name ( CHAR_DATA *ch, char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH], part[MAX_INPUT_LENGTH];
    char *list, *string;
    bool (*cfun)(const char *astr, const char *bstr);

    /* fix crash on NULL namelist */
    if (namelist == NULL || namelist[0] == '\0')
    	return FALSE;

    /* fixed to prevent is_name on "" returning TRUE */
    if (str[0] == '\0')
	return FALSE;

    string = str;
    if ( ch && ch->desc && ch->desc->original )
      ch = ch->desc->original;
    if ( !ch )
      cfun = &str_cmp;
    else if ( !IS_NPC(ch) && !IS_SET(ch->act, PLR_FULLNAME) )
      cfun = &str_prefix;
    else
      cfun = &str_cmp_ast;
    /* we need ALL parts of string to match part of namelist */
    for ( ; ; )  /* start parsing string */
    {
	str = one_argument(str,part);

	if (part[0] == '\0' )
	    return TRUE;

	/* check to see if this is part of namelist */
	list = namelist;
	for ( ; ; )  /* start parsing namelist */
	{
	    list = one_argument(list,name);
	    if (name[0] == '\0')  /* this name was not found */
		return FALSE;

	    if (!(*cfun)(string,name))
		return TRUE; /* full pattern match */

	    if (!(*cfun)(part,name))
		break;
	}
    }
}

bool is_exact_name(char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];

    if (namelist == NULL)
	return FALSE;

    for ( ; ; )
    {
	namelist = one_argument( namelist, name );
	if ( name[0] == '\0' )
	    return FALSE;
	if ( !str_cmp( str, name ) )
	    return TRUE;
    }
}


/*
 * Apply or remove an affect to a character.
 */
void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
    OBJ_DATA *wield;
    char      buf1 [ MAX_STRING_LENGTH ];
    int       mod;
/* XORPHOX */
    AFFECT_DATA af;
    int sn;
    int psn = -1;
    char buf[MAX_STRING_LENGTH];
    buf[0] = '\0';
/* END */

    mod = paf->modifier;

    if ( fAdd )
    {
	SET_BIT   ( ch->affected_by, paf->bitvector );
    }
    else
    {
	REMOVE_BIT( ch->affected_by, paf->bitvector );
	mod = 0 - mod;
    }

    switch ( paf->location )
    {
    default:
        sprintf( buf1, "Affect_modify: unknown location %d on %s.",
		paf->location, ch->name );
	bug ( buf1, 0 );
	return;

    case APPLY_NONE:						break;
    case APPLY_STR:
	if ( !IS_NPC( ch ) )
	    ch->pcdata->mod_str += mod;                         break;
    case APPLY_DEX:
	if ( !IS_NPC( ch ) )
	    ch->pcdata->mod_dex += mod;                         break;
    case APPLY_INT:
	if ( !IS_NPC( ch ) )
	    ch->pcdata->mod_int += mod;                         break;
    case APPLY_WIS:
	if ( !IS_NPC( ch ) )
	    ch->pcdata->mod_wis += mod;                         break;
    case APPLY_CON:
	if ( !IS_NPC( ch ) )
	    ch->pcdata->mod_con += mod;                         break;
    case APPLY_SEX:           ch->sex                   += mod; break;
    case APPLY_CLASS:						break;
    case APPLY_LEVEL:						break;
    case APPLY_AGE:						break;
    case APPLY_HEIGHT:						break;
    case APPLY_WEIGHT:						break;
    case APPLY_MANA:          ch->mod_mana              += mod; break;
    case APPLY_BP:            ch->mod_bp                += mod; break;
    case APPLY_ANTI_DIS:      ch->antidisarm            += mod; break;
    case APPLY_HIT:           ch->mod_hit               += mod; break;
    case APPLY_MOVE:          ch->mod_move              += mod; break;
    case APPLY_GOLD:						break;
    case APPLY_EXP:						break;
    case APPLY_AC:            ch->armor                 += mod; break;
    case APPLY_HITROLL:       ch->hitroll               += mod; break;
    case APPLY_DAMROLL:       ch->damroll               += mod; break;
    case APPLY_SAVING_PARA:   ch->saving_throw          += mod; break;
    case APPLY_SAVING_ROD:    ch->saving_throw          += mod; break;
    case APPLY_SAVING_PETRI:  ch->saving_throw          += mod; break;
    case APPLY_SAVING_BREATH: ch->saving_throw          += mod; break;
    case APPLY_SAVING_SPELL:  ch->saving_throw          += mod; break;
/* XORPHOX perm spells */
      case APPLY_INVISIBLE:
        psn = skill_lookup("invis");
        strcpy(buf, "$n slowly fades into existence.");
      break;
      case APPLY_DETECT_EVIL:
        psn = skill_lookup("detect evil");
      break;
      case APPLY_DETECT_INVIS:
        psn = skill_lookup("detect invis");
      break;
      case APPLY_DETECT_MAGIC:
        psn = skill_lookup("detect magic");
      break;
      case APPLY_DETECT_HIDDEN:
        psn = skill_lookup("detect hidden");
      break;
      case APPLY_SANCTUARY:
        psn = skill_lookup("sanctuary");
        strcpy(buf, "The white aura around $n's body vanishes.");
      break;
      case APPLY_FAERIE_FIRE:
        psn = skill_lookup("faerie fire");
        strcpy(buf, "$n's outline fades.");
      break;
      case APPLY_INFRARED:
        psn = skill_lookup("infravision");
      break;
      case APPLY_COMBAT_MIND:
        psn = skill_lookup("combat mind" );
      break;
      case APPLY_PROTECT:
        psn = skill_lookup("protection evil");
      break;
      case APPLY_BIO_ACCELERATION:
	psn = skill_lookup("bio-acceleration");
	paf->level = ch->level;
      break;
      case APPLY_HEIGHTEN_SENSES:
        sn = gsn_heighten;
        if(fAdd)
        {
          affect_strip(ch, sn);
          af.type     = sn;
          af.level     = ch->level;          
          af.duration  = mod;
          af.location  = APPLY_NONE;
          af.modifier  = 0;
          af.bitvector = AFF_DETECT_INVIS;
          affect_to_char(ch, &af);
          af.bitvector = AFF_DETECT_HIDDEN;
          affect_to_char(ch, &af);
          af.bitvector = AFF_INFRARED;
          affect_to_char(ch, &af);
          send_to_char(AT_BLUE, "Your senses are heightened.\n\r", ch );
        }
        else if ((IS_AFFECTED(ch, AFF_DETECT_INVIS)) && (IS_AFFECTED(ch, AFF_DETECT_HIDDEN) )
          && (IS_AFFECTED(ch, AFF_INFRARED ) ) )
        {
          affect_strip( ch, sn );
          send_to_char(AT_BLUE, skill_table[sn].msg_off, ch);
          send_to_char(AT_BLUE, "\n\r", ch);
	  if (skill_table[sn].room_msg_off)
           act(AT_BLUE, skill_table[sn].room_msg_off, ch, NULL, NULL, TO_ROOM);
        }
        break;
      case APPLY_SNEAK:
        sn = gsn_sneak;
        if(fAdd)
        {
          affect_strip(ch, sn);
          af.type      = sn;
          af.level     = ch->level;
          af.duration  = mod;
          af.location  = APPLY_NONE;
          af.modifier  = 0;
          af.bitvector = AFF_SNEAK;
          affect_to_char(ch, &af);
          send_to_char(AT_BLUE, "You move silently.\n\r", ch);
        }
        else if(IS_AFFECTED(ch, AFF_SNEAK))
        {
          affect_strip(ch, sn);
          send_to_char(AT_BLUE, skill_table[sn].msg_off, ch);
          send_to_char(AT_BLUE, "\n\r", ch);
          if (skill_table[sn].room_msg_off)
           act(AT_BLUE, skill_table[sn].room_msg_off, ch, NULL, NULL,TO_ROOM);
        }
      break;
      case APPLY_HIDE:
        if(fAdd)
        {
          if(IS_AFFECTED(ch, AFF_HIDE))
          REMOVE_BIT(ch->affected_by, AFF_HIDE);
          SET_BIT(ch->affected_by, AFF_HIDE); 
          act(AT_BLUE, "$n blends into the shadows.\n\r", ch, NULL, NULL, TO_ROOM);
          send_to_char(AT_BLUE, "You blend into the shadows.\n\r", ch);
        }
        else if(IS_AFFECTED(ch, AFF_HIDE))
        {
          REMOVE_BIT(ch->affected_by, AFF_HIDE);
          send_to_char(AT_BLUE, "You fade out of the shadows.\n\r", ch);
        }
      break;
      case APPLY_FLYING:
        psn = skill_lookup("fly");
        strcpy(buf, "$n floats slowly to the ground.");
      break;
      case APPLY_PASS_DOOR:
        psn = skill_lookup("pass door");
      break;
      case APPLY_HASTE:
/*
        psn = skill_lookup("haste");
        strcpy(buf, "$n is no longer moving so quickly.");
*/
      break;
      case APPLY_FIRESHIELD:
        psn = skill_lookup("fireshield");
      /*  if(fAdd)
        {
          if(IS_AFFECTED(ch, AFF_FIRESHIELD))
            break;
          if(IS_AFFECTED(ch, AFF_ICESHIELD))
          {
            affect_strip(ch, (sn = skill_lookup("iceshield")));
            send_to_char(AT_LBLUE, skill_table[sn].msg_off, ch);
            act(AT_LBLUE, "The icy crust about $n's body melts to a puddle.",
             ch, NULL, NULL, TO_ROOM );
          }
        }*/
        strcpy(buf, "The flames about $n's body burn out.");
      break;
      case APPLY_SHOCKSHIELD:
        psn = skill_lookup("shockshield");
        strcpy(buf, "The electricity about $n's body flee's into the ground.");
      break;
      case APPLY_ICESHIELD:
        psn = skill_lookup("iceshield");
       /* if(fAdd)
        {
          if(IS_AFFECTED(ch, AFF_ICESHIELD))
            break;
          if(IS_AFFECTED(ch, AFF_FIRESHIELD))
          {
            affect_strip(ch, skill_lookup("fireshield"));
            send_to_char(AT_RED,
             "The flames aboout your body are doused.\n\r", ch);
            act(AT_RED, "The flames about $n's body steam away to nothing.",
             ch, NULL, NULL, TO_ROOM );
          }
        }*/
        strcpy(buf, "The icy crust about $n's body melts to a puddle.");
      break;
      case APPLY_CHAOS:
        psn = skill_lookup("chaos field");
        strcpy(buf, "The chaos around $n fades away.");
      break;
      case APPLY_SCRY:
        psn = skill_lookup("scry");
      break;
      case APPLY_BLESS:
        psn = skill_lookup("bless");
      break;
      case APPLY_GIANT_STRENGTH:
        psn = skill_lookup("giant strength");
        strcpy(buf, "$n no longer looks so mighty.");
      break;
/* END */
    }

    if(psn != -1)
    {
      if(fAdd && !is_affected(ch, psn))
      {
        obj_cast_spell(psn, paf->level, ch, ch, NULL);
        perm_spell(ch, psn);
      }
      else if(!fAdd && spell_duration(ch, psn) == -1)
      {
        affect_strip(ch, psn);
        if(buf[0] != '\0')
          act(AT_BLUE, buf, ch, NULL, NULL, TO_ROOM);
        if(skill_table[psn].msg_off)
        {
          send_to_char(AT_BLUE, skill_table[psn].msg_off, ch);
          send_to_char(AT_BLUE, "\n\r", ch);
          if(skill_table[psn].room_msg_off)
          {
            act(AT_BLUE, skill_table[psn].room_msg_off, ch, NULL,NULL,TO_ROOM);
          }
        }
      }
    }

    if ( IS_NPC( ch ) )
        return;

    /*
     * Check for weapon wielding.
     * Guard against recursion (for weapons with affects).
     */
    if ( ( wield = get_eq_char( ch, WEAR_WIELD ) )
	&& get_obj_weight( wield ) > str_app[get_curr_str( ch )].wield )
    {
	static int depth;

	if ( depth == 0 )
	{
	    depth++;
	    act(AT_GREY, "You drop $p.", ch, wield, NULL, TO_CHAR );
	    act(AT_GREY, "$n drops $p.", ch, wield, NULL, TO_ROOM );
	    obj_from_char( wield );
	    obj_to_room( wield, ch->in_room );
	    depth--;
	}
    }

    return;
}

void affect_modify2( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
    OBJ_DATA *wield;
    char      buf [ MAX_STRING_LENGTH ];
    int       mod;

    mod = paf->modifier;

    if ( fAdd )
    {
	SET_BIT   ( ch->affected_by2, paf->bitvector );
    }
    else
    {
	REMOVE_BIT( ch->affected_by2, paf->bitvector );
	mod = 0 - mod;
    }

    switch ( paf->location )
    {
    default:
        sprintf( buf, "Affect_modify: unknown location %d on %s.",
		paf->location, ch->name );
	bug ( buf, 0 );
	return;

    case APPLY_NONE:						break;
    case APPLY_STR:
	if ( !IS_NPC( ch ) )
	    ch->pcdata->mod_str += mod;                         break;
    case APPLY_DEX:
	if ( !IS_NPC( ch ) )
	    ch->pcdata->mod_dex += mod;                         break;
    case APPLY_INT:
	if ( !IS_NPC( ch ) )
	    ch->pcdata->mod_int += mod;                         break;
    case APPLY_WIS:
	if ( !IS_NPC( ch ) )
	    ch->pcdata->mod_wis += mod;                         break;
    case APPLY_CON:
	if ( !IS_NPC( ch ) )
	    ch->pcdata->mod_con += mod;                         break;
    case APPLY_SEX:           ch->sex                   += mod; break;
    case APPLY_CLASS:						break;
    case APPLY_LEVEL:						break;
    case APPLY_AGE:						break;
    case APPLY_HEIGHT:						break;
    case APPLY_WEIGHT:						break;
    case APPLY_MANA:          ch->mod_mana              += mod; break;
    case APPLY_BP:            ch->mod_bp                += mod; break;
    case APPLY_ANTI_DIS:      ch->antidisarm            += mod; break;
    case APPLY_HIT:           ch->mod_hit               += mod; break;
    case APPLY_MOVE:          ch->mod_move              += mod; break;
    case APPLY_GOLD:						break;
    case APPLY_EXP:						break;
    case APPLY_AC:            ch->armor                 += mod; break;
    case APPLY_HITROLL:       ch->hitroll               += mod; break;
    case APPLY_DAMROLL:       ch->damroll               += mod; break;
    case APPLY_SAVING_PARA:   ch->saving_throw          += mod; break;
    case APPLY_SAVING_ROD:    ch->saving_throw          += mod; break;
    case APPLY_SAVING_PETRI:  ch->saving_throw          += mod; break;
    case APPLY_SAVING_BREATH: ch->saving_throw          += mod; break;
    case APPLY_SAVING_SPELL:  ch->saving_throw          += mod; break;
    }

    if ( IS_NPC( ch ) )
        return;

    /*
     * Check for weapon wielding.
     * Guard against recursion (for weapons with affects).
     */
    if ( ( wield = get_eq_char( ch, WEAR_WIELD ) )
	&& get_obj_weight( wield ) > str_app[get_curr_str( ch )].wield )
    {
	static int depth;

	if ( depth == 0 )
	{
	    depth++;
	    act(AT_GREY, "You drop $p.", ch, wield, NULL, TO_CHAR );
	    act(AT_GREY, "$n drops $p.", ch, wield, NULL, TO_ROOM );
	    obj_from_char( wield );
	    obj_to_room( wield, ch->in_room );
	    depth--;
	}
    }

    return;
}


/*
 * Give an affect to a char.
 */
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_new;

    paf_new = new_affect( );

/* this stuff done in new_affect();
   if ( !affect_free )
    {
	paf_new		= alloc_perm( sizeof( *paf_new ) );
    }
    else
    {
	paf_new		= affect_free;
	affect_free	= affect_free->next;
    } */

    *paf_new		= *paf;
    paf_new->deleted    = FALSE;
    paf_new->next	= ch->affected;
    ch->affected	= paf_new;

    affect_modify( ch, paf_new, TRUE );
    return;
}

void affect_to_char2( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_new;

    paf_new = new_affect();

/*    if ( !affect_free )
    {
	paf_new		= alloc_perm( sizeof( *paf_new ) );
    }
    else
    {
	paf_new		= affect_free;
	affect_free	= affect_free->next;
    } */

    *paf_new		= *paf;
    paf_new->deleted    = FALSE;
    paf_new->next	= ch->affected2;
    ch->affected2	= paf_new;

    affect_modify2( ch, paf_new, TRUE );
    return;
}



/*
 * Remove an affect from a char.
 */
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    if ( paf->deleted )
	return;

    if ( !ch->affected )
    {
	bug( "Affect_remove: no affect.", 0 );
	return;
    }

    if ( paf->bitvector == AFF_FLYING ) 
    {
       affect_modify( ch, paf, FALSE );
       check_nofloor( ch ); 
    }
    else
       affect_modify( ch, paf, FALSE );

    paf->deleted = TRUE;

    return;
}

void affect_remove2( CHAR_DATA *ch, AFFECT_DATA *paf )
{
  if ( paf->deleted )
    return;
  
  if ( !ch->affected2 )
  {
    bug( "Affect_remove2: no affect.", 0 );
    return;
  }

  affect_modify2( ch, paf, FALSE );
  paf->deleted = TRUE;

  return;
}

/*
 * Strip all affects of a given sn.
 */
void affect_strip( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;

    for ( paf = ch->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
	if ( paf->type == sn )
	    affect_remove( ch, paf );
    }

    for ( paf = ch->affected2; paf; paf = paf->next )
    {
      if ( paf->deleted )
	continue;
      if ( paf->type == sn )
	affect_remove2( ch, paf );
    }

    return;
}



/*
 * Return true if a char is affected by a spell.
 */
bool is_affected( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;

    for ( paf = ch->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
	if ( paf->type == sn )
	    return TRUE;
    }
    for ( paf = ch->affected2; paf; paf = paf->next )
    {
      if ( paf->deleted )
	continue;
      if ( paf->type == sn )
	return TRUE;
    }
    return FALSE;
}



/*
 * Add or enhance an affect.
 */
void affect_join( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_old;
    bool         found;

    found = FALSE;
    for ( paf_old = ch->affected; paf_old; paf_old = paf_old->next )
    {
        if ( paf_old->deleted )
	    continue;
	if ( paf_old->type == paf->type )
	{
	   if ( paf_old->location == paf->location )
           {
              paf->duration += paf_old->duration;
	      paf->modifier += paf_old->modifier;
	      affect_remove( ch, paf_old );
	      break;
           }
	}
    }

    affect_to_char( ch, paf );
    return;
}

void affect_join2( CHAR_DATA *ch, AFFECT_DATA *paf )
{
  AFFECT_DATA *paf_old;
  bool         found;

  found = FALSE;
  for ( paf_old = ch->affected2; paf_old; paf_old = paf_old->next )
  {
    if ( paf_old->deleted )
      continue;
    if ( paf_old->type == paf->type )
    {
      if ( paf_old->location == paf->location )
      {
        paf->duration += paf_old->duration;
        paf->modifier += paf_old->modifier;
        affect_remove2( ch, paf_old );
        break;
      }
    }
  }

  affect_to_char2( ch, paf );
  return;
}

/*
 * Move a char out of a room.
 */
void char_from_room( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( !ch->in_room )
    {
	bug( "Char_from_room: NULL.", 0 );
	return;
    }

    if ( !IS_NPC( ch ) && ch->in_room->area )
	--ch->in_room->area->nplayer;

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
	&& obj->item_type == ITEM_LIGHT
	&& obj->value[2] != 0
	&& ch->in_room->light > 0 )
	--ch->in_room->light;

    if ( ch == ch->in_room->people )
    {
	ch->in_room->people = ch->next_in_room;
    }
    else
    {
	CHAR_DATA *prev;

	for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
	{
	    if ( prev->next_in_room == ch )
	    {
		prev->next_in_room = ch->next_in_room;
		break;
	    }
	}

	if ( !prev )
	    bug( "Char_from_room: ch not found.", 0 );
    }

    ch->in_room      = NULL;
    ch->next_in_room = NULL;
    return;
}



/*
 * Move a char into a room.
 */
void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    OBJ_DATA *obj;

    if ( !pRoomIndex )
    {
	bug( "Char_to_room: NULL.", 0 );
	pRoomIndex = get_room_index(ROOM_VNUM_LIMBO);
    }

    ch->in_room		= pRoomIndex;
    ch->next_in_room	= pRoomIndex->people;
    pRoomIndex->people	= ch;

    if ( !IS_NPC( ch ) && ch->in_room->area )
	++ch->in_room->area->nplayer;

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
	&& obj->item_type == ITEM_LIGHT
	&& obj->value[2] != 0 )
	++ch->in_room->light;

    return;
}



/*
 * Give an obj to a char.
 */
void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
    obj->next_content	 = ch->carrying;
    ch->carrying	 = obj;
    obj->carried_by	 = ch;
    obj->in_room	 = NULL;
    obj->in_obj		 = NULL;
    obj->stored_by       = NULL;
    ch->carry_number	+= get_obj_number( obj );
    ch->carry_weight	+= get_obj_weight( obj );
}

/*
 * -- Altrag
 */
void obj_to_storage( OBJ_DATA *obj, CHAR_DATA *ch )
{
    if ( IS_NPC( ch ) )
    {
      bug( "obj_to_storage: NPC storage from %d", ch->pIndexData->vnum );
      obj_to_char( obj, ch );
      return;
    }
    obj->next_content    = ch->pcdata->storage;
    ch->pcdata->storage  = obj;
    obj->carried_by      = NULL;
    obj->in_room         = NULL;
    obj->in_obj          = NULL;
    obj->stored_by       = ch;
    ch->pcdata->storcount++;
}

/*
 * Take an obj from its character.
 */
void obj_from_char( OBJ_DATA *obj )
{
    CHAR_DATA *ch;

    if ( !( ch = obj->carried_by ) )
    {
	bug( "Obj_from_char: null ch.", 0 );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
	unequip_char( ch, obj );

    if ( ch->carrying == obj )
    {
	ch->carrying = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = ch->carrying; prev; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( !prev )
	    bug( "Obj_from_char: obj not in list.", 0 );
    }

    obj->carried_by      = NULL;
    obj->next_content	 = NULL;
    ch->carry_number	-= get_obj_number( obj );
    ch->carry_weight	-= get_obj_weight( obj );
    return;
}

/*
 * -- Altrag
 */
void obj_from_storage( OBJ_DATA *obj )
{
  CHAR_DATA *ch;

  if ( !( ch = obj->stored_by ) )
  {
    bug( "obj_from_storage: NULL ch.", 0 );
    return;
  }

  if ( IS_NPC( ch ) )
  {
    bug( "obj_from_storage: NPC storage by %d.", ch->pIndexData->vnum );
    return;
  }

  if ( ch->pcdata->storage == obj )
  {
    ch->pcdata->storage = obj->next_content;
  }
  else
  {
    OBJ_DATA *prev;

    for ( prev = ch->pcdata->storage; prev; prev = prev->next_content )
    {
      if ( prev->next_content == obj )
      {
	prev->next_content = obj->next_content;
	break;
      }
    }

    if ( !prev )
      bug( "Obj_from_storage: obj not in list.", 0 );
  }

  obj->stored_by = NULL;
  obj->next_content = NULL;
  ch->pcdata->storcount--;
  return;
}

/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac( OBJ_DATA *obj, int iWear )
{
    if ( obj->item_type != ITEM_ARMOR )
	return 0;

    switch ( iWear )
    {
    case WEAR_BODY:     return 3 * obj->value[0];
    case WEAR_HEAD:	return 2 * obj->value[0];
    case WEAR_LEGS:	return 2 * obj->value[0];
    case WEAR_FEET:	return     obj->value[0];
    case WEAR_HANDS:	return     obj->value[0];
    case WEAR_ARMS:	return     obj->value[0];
    case WEAR_SHIELD:	return     obj->value[0];
    case WEAR_FINGER_L:	return     obj->value[0];
    case WEAR_FINGER_R: return     obj->value[0];
    case WEAR_NECK_1:	return     obj->value[0];
    case WEAR_NECK_2:	return     obj->value[0];
    case WEAR_ABOUT:	return 2 * obj->value[0];
    case WEAR_WAIST:	return     obj->value[0];
    case WEAR_WRIST_L:	return     obj->value[0];
    case WEAR_WRIST_R:	return     obj->value[0];
    case WEAR_HOLD:	return     obj->value[0];
    case WEAR_ON_FACE:  return     obj->value[0];
    }

    return 0;
}



/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( obj->deleted )
	    continue;
	if ( obj->wear_loc == iWear )
	    return obj;
    }

    return NULL;
}



/*
 * Equip a char with an obj.
 */
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    AFFECT_DATA *paf;
    char         buf [ MAX_STRING_LENGTH ];

    if ( get_eq_char( ch, iWear ) )
    {
        sprintf( buf, "Equip_char: %s already equipped at %d.",
		ch->name, iWear );
	bug( buf, 0 );
	return;
    }
    if ( gets_zapped( ch, obj ) )
    {
	/*
	 * Thanks to Morgenes for the bug fix here!
	 */
	act(AT_BLUE, "You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR );
	act(AT_BLUE, "$n is zapped by $p and drops it.",  ch, obj, NULL, TO_ROOM );
	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	return;
    }

    ch->armor      	-= apply_ac( obj, iWear );
    obj->wear_loc	 = iWear;

    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
	affect_modify( ch, paf, TRUE );
    for ( paf = obj->affected; paf; paf = paf->next )
	affect_modify( ch, paf, TRUE );

    if ( obj->item_type == ITEM_LIGHT
	&& obj->value[2] != 0
	&& ch->in_room )
	++ch->in_room->light;
    ch->carry_number -= get_obj_number( obj );
    oprog_wear_trigger( obj, ch );
    return;
}



/*
 * Unequip a char with an obj.
 */
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
    AFFECT_DATA *paf;
    char         buf [ MAX_STRING_LENGTH ];

    if ( obj->wear_loc == WEAR_NONE )
    {
        sprintf( buf, "Unequip_char: %s already unequipped with %d.",
		ch->name, obj->pIndexData->vnum );
	bug( buf, 0 );
	return;
    }

    ch->armor		+= apply_ac( obj, obj->wear_loc );
    obj->wear_loc	 = -1;

    for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
	affect_modify( ch, paf, FALSE );
    for ( paf = obj->affected; paf; paf = paf->next )
	affect_modify( ch, paf, FALSE );

    if ( obj->item_type == ITEM_LIGHT
	&& obj->value[2] != 0
	&& ch->in_room
	&& ch->in_room->light > 0 )
	--ch->in_room->light;
    ch->carry_number += get_obj_number( obj );
    return;
}



/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
    OBJ_DATA *obj;
    int       nMatch;

    nMatch = 0;
    for ( obj = list; obj; obj = obj->next_content )
    {
        if ( obj->deleted )
	    continue;
	if ( obj->pIndexData == pObjIndex )
	    nMatch++;
    }

    return nMatch;
}



/*
 * Move an obj out of a room.
 */
void obj_from_room( OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *in_room;

    if ( !( in_room = obj->in_room ) )
    {
	bug( "obj_from_room: NULL.", 0 );
	return;
    }

    if ( obj == in_room->contents )
    {
	in_room->contents = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = in_room->contents; prev; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( !prev )
	{
	    bug( "Obj_from_room: obj not found.", 0 );
	    return;
	}
    }

    obj->in_room      = NULL;
    obj->next_content = NULL;
    return;
}



/*
 * Move an obj into a room.
 */
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
    if ( pRoomIndex->vnum > 32000 )
       pRoomIndex->vnum = 1;
    obj->next_content		= pRoomIndex->contents;
    pRoomIndex->contents	= obj;
    obj->in_room		= pRoomIndex;
    obj->carried_by		= NULL;
    obj->in_obj			= NULL;
    obj->stored_by              = NULL;
    return;
}



/*
 * Move an object into an object.
 */
void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
    if ( obj_to->deleted )
    {
	bug( "Obj_to_obj:  Obj_to already deleted", 0 );
        return;
    }

    obj->next_content		= obj_to->contains;
    obj_to->contains		= obj;
    obj->in_obj			= obj_to;
    obj->in_room		= NULL;
    obj->carried_by		= NULL;
    obj->stored_by              = NULL;

    for ( ; obj_to; obj_to = obj_to->in_obj )
    {
        if ( obj_to->deleted )
            continue;
	if ( obj_to->carried_by )
   	{
            obj_to->carried_by->carry_weight += get_obj_weight( obj );
/*	    obj_to->carried_by->carry_number -= get_obj_number( obj );*/
	    break;
	}
    }
    return;
}



/*
 * Move an object out of an object.
 */
void obj_from_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_from;

    if ( !( obj_from = obj->in_obj ) )
    {
	bug( "Obj_from_obj: null obj_from.", 0 );
	return;
    }

    if ( obj == obj_from->contains )
    {
	obj_from->contains = obj->next_content;
    }
    else
    {
	OBJ_DATA *prev;

	for ( prev = obj_from->contains; prev; prev = prev->next_content )
	{
	    if ( prev->next_content == obj )
	    {
		prev->next_content = obj->next_content;
		break;
	    }
	}

	if ( !prev )
	{
	    bug( "Obj_from_obj: obj not found.", 0 );
	    return;
	}
    }

    obj->next_content = NULL;
    obj->in_obj       = NULL;

    for ( ; obj_from; obj_from = obj_from->in_obj )
    {
        if ( obj_from->deleted )
	    continue;
	if ( obj_from->carried_by )
	{
	/*  obj_from->carried_by->carry_number += get_obj_number( obj );*/
            obj_from->carried_by->carry_weight -= get_obj_weight( obj );
	    break;
	}
    }

    return;
}



/*
 * Extract an obj from the world.
 */
void extract_obj( OBJ_DATA *obj )
{
  OBJ_DATA *obj_content;
  OBJ_DATA *obj_next;
  extern bool      delete_obj;

  if ( obj->deleted )
  {
    bug( "Extract_obj:  Obj already deleted", 0 );
    return;
  }

  if ( obj->in_room    )
    obj_from_room( obj );
  else if ( obj->carried_by )
    obj_from_char( obj );
  else if ( obj->in_obj     )
    obj_from_obj( obj  );
  else if ( obj->stored_by )
    obj_from_storage( obj );

    for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
    {
        obj_next = obj_content->next_content;
	if( obj_content->deleted )
	    continue;
	extract_obj( obj_content );
    }

    obj->deleted = TRUE;

    delete_obj   = TRUE;

    return;
}



/*
 * Extract a char from the world.
 */
void extract_char( CHAR_DATA *ch, bool fPull )
{
           CHAR_DATA *wch;
           OBJ_DATA  *obj;
           OBJ_DATA  *obj_next;
    extern bool       delete_char;
    	   bool       is_arena = IS_ARENA(ch);

    if ( !ch->in_room )
    {
	bug( "Extract_char: NULL.", 0 );
	return;
    }

    if ( fPull )
    {
	char* name;

	if ( IS_NPC ( ch ) )
	    name = ch->short_descr;
	else
	    name = ch->name;

	die_follower( ch, name );
    }

    stop_fighting( ch, TRUE );

    if ( fPull || !is_arena )
      for ( obj = ch->carrying; obj; obj = obj_next )
      {
        obj_next = obj->next_content;
        if ( obj->deleted )
	    continue;
	extract_obj( obj );
      }
    if ( is_arena )
    {
      arena.fch = NULL;
      arena.sch = NULL;
      arena.award = 0;
    }
    
    char_from_room( ch );

    if ( !fPull )
    {
        ROOM_INDEX_DATA *location;

	if ( is_arena && (location = get_room_index(ROOM_ARENA_HALL_SHAME)) )
	  char_to_room(ch, location);
	else if ( !( location = get_room_index( ROOM_VNUM_MORGUE ) ) )
	  {
	    bug( "Purgatory A does not exist!", 0 );
	    char_to_room( ch, get_room_index( ROOM_VNUM_ALTAR ) );
	  }
	else
	    {
	    char buf [ MAX_INPUT_LENGTH ];
	    char_to_room( ch, location );
	    sprintf( buf, "You awaken in the morgue%s",
		     ch->level <= 20 ? ", your battered corpse next to you.\n\r"
				     : ".\n\r" );
	    send_to_char( AT_BLUE, "You awaken in the morgue.\n\r", ch );
	    }
	return;
    }

    if ( IS_NPC( ch ) )
	--ch->pIndexData->count;

    if ( ch->desc && ch->desc->original )
	do_return( ch, "" );

    for ( wch = char_list; wch; wch = wch->next )
    {
	if ( wch->reply == ch )
	    wch->reply = NULL;
	if ( wch->hunting == ch )
	    wch->hunting = NULL;
    }

    ch->deleted = TRUE;

    if ( ch->desc )
	ch->desc->character = NULL;

    if (IS_NPC ( ch ) )
       mobs_in_game--;

    delete_char = TRUE;
    return;
}



/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        number;
    int        count;

    number = number_argument( argument, arg );
    count  = 0;
    if ( !str_cmp( arg, "self" ) )
	return ch;
    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( !can_see( ch, rch ) || !is_name( ch, arg, rch->name ) )
	    continue;
	if ( ++count == number )
	    return rch;
    }

    return NULL;
}




/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    char       arg [ MAX_INPUT_LENGTH ];
    int        number;
    int        count;

    if ( ( wch = get_char_room( ch, argument ) ) )
	return wch;

    number = number_argument( argument, arg );
    count  = 0;
    for ( wch = char_list; wch ; wch = wch->next )
    {
	if ( !can_see( ch, wch ) || !is_name( ch, arg, wch->name ) )
	    continue;
	if ( ++count == number )
	    return wch;
    }

    return NULL;
}
/*
 * Find a PC in the world.
 */
CHAR_DATA *get_pc_world( CHAR_DATA *ch, char *argument )
{
  DESCRIPTOR_DATA *d;
  for ( d = descriptor_list; d; d = d->next )
	{
	if ( d->connected == CON_PLAYING
	&& is_name( ch, argument, d->character->name ) 
	&& can_see( ch, d->character ) )
		return d->character;
	}
  return NULL;
}

/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type( OBJ_INDEX_DATA *pObjIndex )
{
    OBJ_DATA *obj;

    for ( obj = object_list; obj; obj = obj->next )
    {
        if ( obj->deleted )
	    continue;

	if ( obj->pIndexData == pObjIndex )
	    return obj;
    }

    return NULL;
}


/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = list; obj; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) && is_name( ch, arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->wear_loc == WEAR_NONE
	    && can_see_obj( ch, obj )
	    && is_name( ch, arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}

OBJ_DATA *get_obj_storage( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int number;
  int count;

  if ( IS_NPC( ch ) )
  {
    bug( "get_obj_storage: NPC storage from %d", ch->pIndexData->vnum );
    return NULL;
  }

  number = number_argument( argument, arg );
  count = 0;

  for ( obj = ch->pcdata->storage; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj )
	 && is_name( ch, arg, obj->name ) )
    {
      if ( ++count == number )
	return obj;
    }
  }
  return NULL;
}

/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->wear_loc != WEAR_NONE
	    && can_see_obj( ch, obj )
	    && is_name( ch, arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_here( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    obj = get_obj_list( ch, argument, ch->in_room->contents );
    if ( obj )
	return obj;

    if ( ( obj = get_obj_carry( ch, argument ) ) )
	return obj;

    if ( ( obj = get_obj_wear( ch, argument ) ) )
	return obj;

    return NULL;
}



/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       number;
    int       count;

    if ( ( obj = get_obj_here( ch, argument ) ) )
	return obj;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = object_list; obj; obj = obj->next )
    {
	if ( can_see_obj( ch, obj ) && is_name( ch, arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Create a 'money' obj.
 */

#ifdef NEW_MONEY

OBJ_DATA *create_money( MONEY_DATA *amount )
{
    OBJ_DATA *obj;

    if (  ( ( amount->gold <= 0 ) && ( amount->silver <= 0 ) && ( amount->copper <= 0 ) ) ||
	  ( amount->gold < 0 ) || ( amount->silver < 0 ) || ( amount->copper < 0 )  )
    {
       char buf [ MAX_STRING_LENGTH ];
       sprintf( buf, "Create_money: zero or negative money %d %d %d.", amount->gold, amount->silver, amount->copper );
       bug( buf, 0 );
       amount->copper = 1;
       amount->gold = 0;
       amount->silver = 0;
    }
    if ( ( amount->gold == 1 ) && ( amount->silver <= 0 ) && ( amount->copper <= 0 ) )
        obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE  ), 0 );
    else 
    if ( ( amount->silver == 1 ) && ( amount->gold <= 0 ) && ( amount->copper <= 0 ) )
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE  ), 0 );
    else
    if ( ( amount->copper == 1 ) && ( amount->gold <= 0 ) && ( amount->silver <= 0 ) )
 	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE  ), 0 );
    else
 	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ), 0 );    
    
    obj->value[0]               = amount->gold;
    obj->value[1]		= amount->silver;
    obj->value[2]		= amount->copper;

    return obj;  

}

#else

OBJ_DATA *create_money( int amount )
{
    OBJ_DATA *obj;

    if ( amount <= 0 )
    {
	bug( "Create_money: zero or negative money %d.", amount );
	amount = 1;
    }

    if ( amount == 1 )
    {
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE  ), 0 );
    }
    else
    {
	obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ), 0 );
    }

    obj->value[0]		= amount;
    return obj;
}

#endif

/*
 * Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here.
 */
int get_obj_number( OBJ_DATA *obj )
{
    int number;

    number = 0;
/*    if ( obj->item_type == ITEM_CONTAINER )
        for ( obj = obj->contains; obj; obj = obj->next_content )
	{
	    if ( obj->deleted )
	        continue;
	    number += get_obj_number( obj );
	}
    else */
	number = 1;

    return number;
}



/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight( OBJ_DATA *obj )
{
    int weight;
    

    weight = obj->weight;
    for ( obj = obj->contains; obj; obj = obj->next_content )
    {
	if ( obj->deleted )
	    continue;
	weight += get_obj_weight( obj );
    }

    return weight;
}



/*
 * True if room is dark.
 */
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
    OBJ_DATA *obj;

    if ( pRoomIndex == NULL )
    {
      bug( "pRoomIndex equal to NULL", 0 );
      return TRUE; 
    }

    if ( pRoomIndex->light > 0 )
	return FALSE;

    for ( obj = pRoomIndex->contents; obj; obj = obj->next_content )
    {
	if ( obj->deleted )
	    continue;
	if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
	    return FALSE;
    }

    if ( IS_SET( pRoomIndex->room_flags, ROOM_DARK ) )
	return TRUE;

    if ( pRoomIndex->sector_type == SECT_INSIDE
	|| pRoomIndex->sector_type == SECT_CITY )
	return FALSE;

    if ( weather_info.sunlight == SUN_DUSK
	|| weather_info.sunlight == SUN_DARK )
	return TRUE;

    return FALSE;
}



/*
 * True if room is private.
 */
bool room_is_private( ROOM_INDEX_DATA *pRoomIndex )
{
    CHAR_DATA *rch;
    int        count;

    count = 0;
    for ( rch = pRoomIndex->people; rch; rch = rch->next_in_room )
    {
	if ( rch->deleted )
	    continue;

	count++;
    }

    if ( IS_SET( pRoomIndex->room_flags, ROOM_PRIVATE  ) && count >= 2 )
	return TRUE;

    if ( IS_SET( pRoomIndex->room_flags, ROOM_SOLITARY ) && count >= 1 )
	return TRUE;

    return FALSE;
}



/*
 * True if char can see victim.
 */
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( victim->deleted )
        return FALSE;

    if ( ch == victim )
	return TRUE;
    
/*    if ( !str_cmp(ch->name, "Hannibal") )
	return TRUE; */

    if ( !IS_NPC( victim )
	&& IS_SET( victim->act, PLR_WIZINVIS )
	&& get_trust( ch ) < victim->wizinvis )
	return FALSE;
   
    if ( !IS_NPC( victim )
         && IS_SET( victim->act, PLR_CLOAKED )
         && get_trust( ch ) < victim->cloaked 
         && ( ch->in_room->vnum != victim->in_room->vnum )
       )
         return FALSE;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;
    if ( is_raffected( ch->in_room, gsn_globedark )
    && ch->race != RACE_DROW )
	return FALSE;

    if ( IS_AFFECTED( ch, AFF_BLIND ) 
    && !IS_AFFECTED2( ch, AFF_TRUESIGHT )
    && ch->race != RACE_ILLITHID )
	return FALSE;

    if ( room_is_dark( ch->in_room ) 
      && !IS_AFFECTED( ch, AFF_INFRARED )
      && !IS_AFFECTED2( ch, AFF_TRUESIGHT )
      && ( ch->race != RACE_ELF   )
      && ( ch->race != RACE_DWARF )
      && ( ch->race != RACE_DROW  )
      && ( ch->race != RACE_DEMON ) )
	return FALSE;

    if ( victim->position == POS_DEAD )
        return TRUE;

 if ( IS_AFFECTED2( victim, AFF_PHASED )
      && (!IS_AFFECTED2(ch, AFF_TRUESIGHT )
      || (IS_NPC(ch)
          && ch->level < 50) ))
      return FALSE; 

         
    if ( IS_AFFECTED( victim, AFF_INVISIBLE )
	&& !IS_AFFECTED( ch, AFF_DETECT_INVIS )
	&& !IS_AFFECTED2( ch, AFF_TRUESIGHT )
	&& ( ch->race != RACE_DEMON )
	&& ( !is_class( ch, CLASS_THIEF )
	     && ch->level < 30 ) )
	return FALSE;

    if ( IS_AFFECTED( victim, AFF_HIDE )
	&& !IS_AFFECTED( ch, AFF_DETECT_HIDDEN )
	&& !IS_AFFECTED2( ch, AFF_TRUESIGHT )
	&& !victim->fighting
	&& ch->race != RACE_DWARF
	&& ch->race != RACE_DEMON )
	return FALSE;

    return TRUE;
}



/*
 * True if char can see obj.
 */
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( obj->deleted )
        return FALSE;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( IS_AFFECTED( ch, AFF_BLIND ) 
    && !IS_AFFECTED2( ch, AFF_TRUESIGHT ) 
    && ch->race != RACE_ILLITHID )
	return FALSE;
    if ( is_raffected( ch->in_room, gsn_globedark ) 
    && ch->race != RACE_DROW )
	return FALSE;
    if ( !IS_NPC( ch ) && IS_AFFECTED2( ch, AFF_TRUESIGHT ) )
        return TRUE;

    if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
	return TRUE;

    if ( room_is_dark( ch->in_room ) 
       && !IS_AFFECTED( ch, AFF_INFRARED )
       && !IS_AFFECTED2( ch, AFF_TRUESIGHT )
       && ( ch->race != RACE_ELF )
       && ( ch->race != RACE_DWARF )
       && ( ch->race != RACE_DROW )
       && ( ch->race != RACE_DEMON ) )
	return FALSE;

    if ( IS_SET( obj->extra_flags, ITEM_INVIS )
	&& !IS_AFFECTED( ch, AFF_DETECT_INVIS )
	&& !IS_AFFECTED2( ch, AFF_TRUESIGHT )
	&& ( ch->race != RACE_DEMON ) )
	return FALSE;

    return TRUE;
}



/*
 * True if char can drop obj.
 */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( !IS_SET( obj->extra_flags, ITEM_NODROP ) )
	return TRUE;

    if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL )
	return TRUE;

    return FALSE;
}



/*
 * Return ascii name of an item type.
 */
char *item_type_name( OBJ_DATA *obj )
{
    OBJ_DATA *in_obj;
    char      buf [ MAX_STRING_LENGTH ];

    switch ( obj->item_type )
    {
    case ITEM_LIGHT:		return "light";
    case ITEM_SCROLL:		return "scroll";
    case ITEM_WAND:		return "wand";
    case ITEM_STAFF:		return "staff";
    case ITEM_WEAPON:		return "weapon";
    case ITEM_TREASURE:		return "treasure";
    case ITEM_ARMOR:		return "armor";
    case ITEM_POTION:		return "potion";
    case ITEM_NOTEBOARD:	return "noteboard";
    case ITEM_FURNITURE:	return "furniture";
    case ITEM_TRASH:		return "trash";
    case ITEM_CONTAINER:	return "container";
    case ITEM_DRINK_CON:	return "drink container";
    case ITEM_BLOOD:            return "blood";
    case ITEM_KEY:		return "key";
    case ITEM_FOOD:		return "food";
    case ITEM_MONEY:		return "money";
    case ITEM_BOAT:		return "boat";
    case ITEM_CORPSE_NPC:	return "npc corpse";
    case ITEM_CORPSE_PC:	return "pc corpse";
    case ITEM_FOUNTAIN:		return "fountain";
    case ITEM_PILL:		return "pill";
    case ITEM_LENSE:            return "contacts";
    case ITEM_PORTAL:           return "portal";
    case ITEM_VODOO:            return "voodo doll";
    case ITEM_BERRY:            return "goodberry";
    }

    for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
      ;

    if ( in_obj->carried_by )
      sprintf( buf, "Item_type_name: unknown type %d from %s owned by %s.",
	      obj->item_type, obj->name, obj->carried_by->name );
    else
      sprintf( buf,
	      "Item_type_name: unknown type %d from %s owned by (unknown).",
	      obj->item_type, obj->name );

    bug( buf, 0 );
    return "(unknown)";
}



/*
 * Return ascii name of an affect location.
 */
char *affect_loc_name( int location )
{
    switch ( location )
    {
    case APPLY_NONE:		return "none";
    case APPLY_STR:		return "strength";
    case APPLY_DEX:		return "dexterity";
    case APPLY_INT:		return "intelligence";
    case APPLY_WIS:		return "wisdom";
    case APPLY_CON:		return "constitution";
    case APPLY_SEX:		return "sex";
    case APPLY_CLASS:		return "class";
    case APPLY_LEVEL:		return "level";
    case APPLY_AGE:		return "age";
    case APPLY_HEIGHT:          return "height";
    case APPLY_WEIGHT:          return "weight";
    case APPLY_MANA:		return "mana";
    case APPLY_BP:              return "blood";
    case APPLY_ANTI_DIS:        return "anti-disarm";
    case APPLY_HIT:		return "hp";
    case APPLY_MOVE:		return "moves";
    case APPLY_GOLD:		return "gold";
    case APPLY_EXP:		return "experience";
    case APPLY_AC:		return "armor class";
    case APPLY_HITROLL:		return "hit roll";
    case APPLY_DAMROLL:		return "damage roll";
    case APPLY_SAVING_PARA:	return "save vs paralysis";
    case APPLY_SAVING_ROD:	return "save vs rod";
    case APPLY_SAVING_PETRI:	return "save vs petrification";
    case APPLY_SAVING_BREATH:	return "save vs breath";
    case APPLY_SAVING_SPELL:	return "save vs spell";
/* X */
      case APPLY_INVISIBLE:	return "'invisible'";
      case APPLY_DETECT_EVIL:	return "'detect evil'";
      case APPLY_DETECT_INVIS:	return "'detect invis'";
      case APPLY_DETECT_MAGIC:	return "'detect magic'";
      case APPLY_DETECT_HIDDEN:	return "'detect hidden'";
      case APPLY_SANCTUARY:	return "'sanctuary'";
      case APPLY_FAERIE_FIRE:	return "'faerie fire'";
      case APPLY_INFRARED:	return "'infrared'";
      case APPLY_PROTECT:	return "'protection evil'";
      case APPLY_COMBAT_MIND:   return "'combat mind'";
      case APPLY_SNEAK:		return "'sneak'";
      case APPLY_HIDE:		return "'hide'";
      case APPLY_FLYING:	return "'fly'";
      case APPLY_PASS_DOOR:	return "'pass door'";
      case APPLY_HASTE:		return "'haste'";
      case APPLY_FIRESHIELD:    return "'fireshield'";
      case APPLY_SHOCKSHIELD:   return "'shockshield'";
      case APPLY_ICESHIELD:     return "'iceshield'";  
      case APPLY_CHAOS:         return "'chaos field'";
      case APPLY_SCRY:          return "'scry'";
      case APPLY_BLESS:		return "'bless'";
      case APPLY_GIANT_STRENGTH:return "'giant strength'";
      case APPLY_HEIGHTEN_SENSES:      return "'heighten-senses'";
      case APPLY_BIO_ACCELERATION: return "'bio-acceleration'";
/* END */
    }

    bug( "Affect_location_name: unknown location %d.", location );
    return "(unknown)";
}



/*
 * Return ascii name of an affect bit vector.
 */
char *affect_bit_name( int vector )
{
    static char buf [ 512 ];

    buf[0] = '\0';
    if ( vector & AFF_BLIND         ) strcat( buf, " blind"         );
    if ( vector & AFF_INVISIBLE     ) strcat( buf, " invisible"     );
    if ( vector & AFF_DETECT_EVIL   ) strcat( buf, " detect_evil"   );
    if ( vector & AFF_DETECT_INVIS  ) strcat( buf, " detect_invis"  );
    if ( vector & AFF_DETECT_MAGIC  ) strcat( buf, " detect_magic"  );
    if ( vector & AFF_DETECT_HIDDEN ) strcat( buf, " detect_hidden" );
    if ( vector & AFF_HASTE         ) strcat( buf, " haste"         );
    if ( vector & AFF_SANCTUARY     ) strcat( buf, " sanctuary"     );
    if ( vector & AFF_FIRESHIELD    ) strcat( buf, " fireshield"    );
    if ( vector & AFF_SHOCKSHIELD   ) strcat( buf, " shockshield"   );
    if ( vector & AFF_ICESHIELD     ) strcat( buf, " iceshield"     );
    if ( vector & AFF_CHAOS         ) strcat( buf, " chaos_field"   );
    if ( vector & AFF_FAERIE_FIRE   ) strcat( buf, " faerie_fire"   );
    if ( vector & AFF_INFRARED      ) strcat( buf, " infrared"      );
    if ( vector & AFF_CURSE         ) strcat( buf, " curse"         );
    if ( vector & AFF_FLAMING       ) strcat( buf, " flaming"       );
    if ( vector & AFF_POISON        ) strcat( buf, " poison"        );
    if ( vector & AFF_PROTECT       ) strcat( buf, " protect"       );
    if ( vector & AFF_SLEEP         ) strcat( buf, " sleep"         );
    if ( vector & AFF_SNEAK         ) strcat( buf, " sneak"         );
    if ( vector & AFF_HIDE          ) strcat( buf, " hide"          );
    if ( vector & AFF_CHARM         ) strcat( buf, " charm"         );
    if ( vector & AFF_FLYING        ) strcat( buf, " flying"        );
    if ( vector & AFF_PASS_DOOR     ) strcat( buf, " pass_door"     );
    if ( vector & AFF_PEACE         ) strcat( buf, " peace"         );
    if ( vector & AFF_VIBRATING     ) strcat( buf, " vibrating"     );
    if ( vector & AFF_STUN          ) strcat( buf, " stunned"         );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *affect_bit_name2( int vector )
{
    static char buf [ 512 ];

    buf[0] = '\0';
    if ( vector & AFF_INERTIAL      ) strcat( buf, " inertial"      );
    if ( vector & AFF_POLYMORPH     ) strcat( buf, " polymorph"     );
    if ( vector & AFF_NOASTRAL      ) strcat( buf, " noastral"      );
    if ( vector & AFF_TRUESIGHT     ) strcat( buf, " truesight"     );
    if ( vector & AFF_BLADE         ) strcat( buf, " bladebarrier"  );
    if ( vector & AFF_DETECT_GOOD   ) strcat( buf, " detect_good"   );
    if (vector & AFF_PROTECTION_GOOD) strcat(buf, " protection_good");
    if ( vector & AFF_BERSERK       ) strcat( buf, " berserk"       );
    if ( vector & AFF_FIELD	    ) strcat( buf, " field_of_decay");
    if ( vector & AFF_RAGE	    ) strcat( buf, " rage"	    );
    if ( vector & AFF_RUSH	    ) strcat(buf, " adrenaline_rush");
    if ( vector & AFF_PHASED        ) strcat( buf, " phase_shift"   );
    if ( vector & AFF_GOLDEN	    ) strcat( buf, " golden_aura"   );
	if ( vector & AFF_HALLUCINATING ) strcat( buf, " hallucinate"	);
	if ( vector & AFF_PLAGUE	)     strcat( buf, " plague"		);
	if ( vector & AFF_UNHOLYSTRENGTH) strcat( buf, " unholystrength" );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}



/*
 * Return ascii name of extra flags vector.
 */
char *extra_bit_name( int extra_flags )
{
    static char buf [ 512 ];

    buf[0] = '\0';
    if ( extra_flags & ITEM_PROTOTYPE    ) strcat( buf, " prototype"    );
    if ( extra_flags & ITEM_GLOW         ) strcat( buf, " glow"         );
    if ( extra_flags & ITEM_HUM          ) strcat( buf, " hum"          );
    if ( extra_flags & ITEM_DARK         ) strcat( buf, " dark"         );
    if ( extra_flags & ITEM_LOCK         ) strcat( buf, " lock"         );
    if ( extra_flags & ITEM_EVIL         ) strcat( buf, " evil"         );
    if ( extra_flags & ITEM_INVIS        ) strcat( buf, " invis"        );
    if ( extra_flags & ITEM_MAGIC        ) strcat( buf, " magic"        );
    if ( extra_flags & ITEM_NODROP       ) strcat( buf, " nodrop"       );
    if ( extra_flags & ITEM_BLESS        ) strcat( buf, " bless"        );
    if ( extra_flags & ITEM_ANTI_GOOD    ) strcat( buf, " anti-good"    );
    if ( extra_flags & ITEM_ANTI_EVIL    ) strcat( buf, " anti-evil"    );
    if ( extra_flags & ITEM_ANTI_NEUTRAL ) strcat( buf, " anti-neutral" );
    if ( extra_flags & ITEM_NOREMOVE     ) strcat( buf, " noremove"     );
    if ( extra_flags & ITEM_INVENTORY    ) strcat( buf, " inventory"    );
    if ( extra_flags & ITEM_POISONED     ) strcat( buf, " poisoned"     );
    if ( extra_flags & ITEM_DWARVEN	 ) strcat( buf, " dwarven"	);
    if ( extra_flags & ITEM_FLAME        ) strcat( buf, " burning"      );
    if ( extra_flags & ITEM_CHAOS        ) strcat( buf, " chaotic"      );
    if ( extra_flags & ITEM_NO_DAMAGE    ) strcat( buf, " indestructable");
    if ( extra_flags & ITEM_ICY          ) strcat( buf, " frosty"       );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}
/* FOR NEW FLAGS */
char *anticlass_bit_name( int anticlass )
{
    static char buf [ 512 ];
    buf[0] = '\0';
    if ( anticlass & ITEM_ANTI_MAGE    ) strcat( buf, " anti-mage"    );
    if ( anticlass & ITEM_ANTI_CLERIC  ) strcat( buf, " anti-cleric"  );
    if ( anticlass & ITEM_ANTI_THIEF   ) strcat( buf, " anti-thief"   );
    if ( anticlass & ITEM_ANTI_WARRIOR ) strcat( buf, " anti-warrior" );
    if ( anticlass & ITEM_ANTI_PSI     ) strcat( buf, " anti-psi"     );
    if ( anticlass & ITEM_ANTI_DRUID   ) strcat( buf, " anti-druid"   );
    if ( anticlass & ITEM_ANTI_PALADIN ) strcat( buf, " anti-paladin" );
    if ( anticlass & ITEM_ANTI_RANGER  ) strcat( buf, " anti-ranger"  );
    if ( anticlass & ITEM_ANTI_BARD    ) strcat( buf, " anti-bard"    );
    if ( anticlass & ITEM_ANTI_VAMP    ) strcat( buf, " anti-vampire" );
    if ( anticlass & ITEM_ANTI_NECRO   ) strcat( buf, " anti-necromancer");
    if ( anticlass & ITEM_ANTI_WWF     ) strcat( buf, " anti-werewolf");
    if ( anticlass & ITEM_ANTI_MONK    ) strcat( buf, " anti-monk" );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *antirace_bit_name( int antirace )
{
    static char buf [ 512 ];
    buf[0] = '\0';  
    if ( antirace & ITEM_ANTI_HUMAN     ) strcat( buf, " anti-human"    );
    if ( antirace & ITEM_ANTI_ELF       ) strcat( buf, " anti-elf"      );
    if ( antirace & ITEM_ANTI_DWARF     ) strcat( buf, " anti-dwarf"    );
    if ( antirace & ITEM_ANTI_PIXIE     ) strcat( buf, " anti-pixie"    );
    if ( antirace & ITEM_ANTI_HALFLING  ) strcat( buf, " anti-halfling" );
    if ( antirace & ITEM_ANTI_DROW      ) strcat( buf, " anti-drow"     );
    if ( antirace & ITEM_ANTI_ELDER     ) strcat( buf, " anti-elder"    );
    if ( antirace & ITEM_ANTI_OGRE      ) strcat( buf, " anti-ogre"     );
    if ( antirace & ITEM_ANTI_LIZARDMAN ) strcat( buf, " anti-lizardman");
    if ( antirace & ITEM_ANTI_DEMON     ) strcat( buf, " anti-demon"    );
    if ( antirace & ITEM_ANTI_GHOUL     ) strcat( buf, " anti-ghoul"    );
    if ( antirace & ITEM_ANTI_ILLITHID  ) strcat( buf, " anti-illithid" );
    if ( antirace & ITEM_ANTI_MINOTAUR  ) strcat( buf, " anti-minotaur" );
    if ( antirace & ITEM_ANTI_TROLL     ) strcat( buf, " anti-troll"    );
    if ( antirace & ITEM_ANTI_SHADOW    ) strcat( buf, " anti-shadow"   );
     return ( buf[0] != '\0' ) ? buf+1 : "none";
}


char *act_bit_name( int act )
{
    static char buf [ 512 ];
    
    buf[0] = '\0';
    if ( act & 1 ) 
    {
      strcat( buf, " npc" );
      if ( act & ACT_PROTOTYPE )      strcat( buf, " prototype" );
      if ( act & ACT_SENTINEL )       strcat( buf, " sentinel" );
      if ( act & ACT_SCAVENGER )      strcat( buf, " scavenger" );
      if ( act & ACT_AGGRESSIVE )     strcat( buf, " aggressive" );
      if ( act & ACT_STAY_AREA )      strcat( buf, " stayarea" );
      if ( act & ACT_PET )            strcat( buf, " pet" );
      if ( act & ACT_TRAIN )          strcat( buf, " trainer" );
      if ( act & ACT_PRACTICE )       strcat( buf, " practicer" );
      if ( act & ACT_GAMBLE )         strcat( buf, " gambler" );
      if ( act & ACT_TRACK )          strcat( buf, " track" );
      if ( act & ACT_UNDEAD )	      strcat( buf, " undead" );
      if ( act & ACT_QUESTMASTER )    strcat( buf, " questmaster" );
      if ( act & ACT_POSTMAN )        strcat( buf, " postman" );
      if ( act & ACT_NOPUSH )	      strcat( buf, " nopush" );
      if ( act & ACT_NODRAG )	      strcat( buf, " nodrag" );
      if ( act & ACT_NOSHADOW )	      strcat( buf, " noshadow" );
      if ( act & ACT_NOASTRAL )	      strcat( buf, " noastral" );
      if ( act & ACT_NEWBIE )	      strcat( buf, " newbie" );
    }
    else
    {
      strcat( buf, " pc" );
      if ( act & PLR_AFK ) strcat( buf, " AFK" );
      if ( act & PLR_BOUGHT_PET ) strcat( buf, " boughtpet" );
      if ( act & PLR_AUTOEXIT ) strcat( buf, " autoexit" );
      if ( act & PLR_AUTOLOOT ) strcat( buf, " autoloot" );
      if ( act & PLR_AUTOSAC ) strcat( buf, " autosac" );
      if ( act & PLR_BLANK ) strcat( buf, " blankline" );
      if ( act & PLR_BRIEF ) strcat( buf, " brief" );
      if ( act & PLR_COMBINE ) strcat( buf, " combine" );
      if ( act & PLR_PROMPT ) strcat( buf, " prompt" );
      if ( act & PLR_TELNET_GA ) strcat( buf, " telnetga" );
      if ( act & PLR_HOLYLIGHT ) strcat( buf, " holylight" );
      if ( act & PLR_WIZINVIS ) strcat( buf, " wizinvis" );
      if ( act & PLR_CLOAKED ) strcat( buf, " cloaked" );
      if ( act & PLR_SILENCE ) strcat( buf, " silence" );
      if ( act & PLR_NO_EMOTE ) strcat( buf, " noemote" );
      if ( act & PLR_NO_TELL ) strcat( buf, " notell" );
      if ( act & PLR_LOG ) strcat( buf, " log" );
      if ( act & PLR_FREEZE ) strcat( buf, " freeze" );
      if ( act & PLR_THIEF ) strcat( buf, " thief" );
      if ( act & PLR_KILLER ) strcat( buf, " killer" );
      if ( act & PLR_OUTCAST ) strcat( buf, " outcast" );
      if ( act & PLR_ANSI ) strcat( buf, " ansi" );
      if ( act & PLR_AUTOCOINS ) strcat( buf, " autocoins" );
      if ( act & PLR_AUTOSPLIT ) strcat( buf, " autosplit" );
//      if ( act & PLR_KEEPTITLE ) strcat( buf, " keeptitle" );
      if ( act & PLR_UNDEAD ) strcat( buf, " undead" );
      if ( act & PLR_PKILLER ) strcat( buf, " pkiller" );
    }
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}
        
CHAR_DATA *get_char( CHAR_DATA *ch )
{
    if ( !ch->pcdata )
        return ch->desc->original;
    else
        return ch;
}

bool longstring( CHAR_DATA *ch, char *argument )
{
    if ( strlen( argument) > 60 )
    {
	send_to_char(C_DEFAULT, "No more than 60 characters in this field.\n\r", ch );
	return TRUE;
    }
    else
        return FALSE;
}

void end_of_game( void )
{
    DESCRIPTOR_DATA *d;
    DESCRIPTOR_DATA *d_next;

    save_player_list();

    for ( d = descriptor_list; d; d = d_next )
    {
	d_next = d->next;
	if ( d->connected == CON_PLAYING )
	{
	    if ( d->character->position == POS_FIGHTING )
	      interpret( d->character, "save" );
	    else
	      interpret( d->character, "quit" );
	}
    }

    return;

}

/* XOR */
char *imm_bit_name(int imm_flags)
{
    static char buf[512];

    buf[0] = '\0';

    if (imm_flags & IMM_SUMMON		) strcat(buf, " summon");
    if (imm_flags & IMM_CHARM		) strcat(buf, " charm");
    if (imm_flags & IMM_MAGIC		) strcat(buf, " magic");
    if (imm_flags & IMM_WEAPON		) strcat(buf, " weapon");
    if (imm_flags & IMM_BASH		) strcat(buf, " blunt");
    if (imm_flags & IMM_PIERCE		) strcat(buf, " piercing");
    if (imm_flags & IMM_SLASH		) strcat(buf, " slashing");
    if (imm_flags & IMM_FIRE		) strcat(buf, " fire");
    if (imm_flags & IMM_COLD		) strcat(buf, " cold");
    if (imm_flags & IMM_LIGHTNING	) strcat(buf, " lightning");
    if (imm_flags & IMM_ACID		) strcat(buf, " acid");
    if (imm_flags & IMM_POISON		) strcat(buf, " poison");
    if (imm_flags & IMM_NEGATIVE	) strcat(buf, " negative");
    if (imm_flags & IMM_HOLY		) strcat(buf, " holy");
    if (imm_flags & IMM_ENERGY		) strcat(buf, " energy");
    if (imm_flags & IMM_MENTAL		) strcat(buf, " mental");
    if (imm_flags & IMM_DISEASE		) strcat(buf, " disease");
    if (imm_flags & IMM_LIGHT		) strcat(buf, " light");
    if (imm_flags & IMM_SOUND		) strcat(buf, " light");
    if (imm_flags & VULN_IRON		) strcat(buf, " iron");
    if (imm_flags & VULN_WOOD		) strcat(buf, " wood");
    if (imm_flags & VULN_SILVER		) strcat(buf, " silver");
    if (imm_flags & IMM_NERVE		) strcat(buf, " nerve");  

    return ( buf[0] != '\0' ) ? buf+1 : "none" ; 
}

/* for immunity, vulnerabiltiy, and resistant
   the 'globals' (magic and weapons) may be overriden
   three other cases -- wood, silver, and iron -- are checked in fight.c */

int check_immune(CHAR_DATA *ch, int dam_type)
{
    int immune, def;
    int bit;

    immune = -1;
    def = IS_NORMAL;

    if (dam_type == DAM_NONE)
	return immune;

    if (dam_type <= 3)
    {
	if (IS_SET(ch->imm_flags,IMM_WEAPON))
	    def = IS_IMMUNE;
	else if (IS_SET(ch->res_flags,RES_WEAPON))
	    def = IS_RESISTANT;
	else if (IS_SET(ch->vul_flags,VULN_WEAPON))
	    def = IS_VULNERABLE;
    }
    else /* magical attack */
    {	
	if (IS_SET(ch->imm_flags,IMM_MAGIC))
	    def = IS_IMMUNE;
	else if (IS_SET(ch->res_flags,RES_MAGIC))
	    def = IS_RESISTANT;
	else if (IS_SET(ch->vul_flags,VULN_MAGIC))
	    def = IS_VULNERABLE;
    }

    /* set bits to check -- VULN etc. must ALL be the same or this will fail */
    switch (dam_type)
    {
	case(DAM_BASH):		bit = IMM_BASH;		break;
	case(DAM_PIERCE):	bit = IMM_PIERCE;	break;
	case(DAM_SLASH):	bit = IMM_SLASH;	break;
	case(DAM_FIRE):		bit = IMM_FIRE;		break;
	case(DAM_COLD):		bit = IMM_COLD;		break;
	case(DAM_LIGHTNING):	bit = IMM_LIGHTNING;	break;
	case(DAM_ACID):		bit = IMM_ACID;		break;
	case(DAM_POISON):	bit = IMM_POISON;	break;
	case(DAM_NEGATIVE):	bit = IMM_NEGATIVE;	break;
	case(DAM_HOLY):		bit = IMM_HOLY;		break;
	case(DAM_ENERGY):	bit = IMM_ENERGY;	break;
	case(DAM_MENTAL):	bit = IMM_MENTAL;	break;
	case(DAM_DISEASE):	bit = IMM_DISEASE;	break;
	case(DAM_DROWNING):	bit = IMM_DROWNING;	break;
	case(DAM_LIGHT):	bit = IMM_LIGHT;	break;
	case(DAM_CHARM):	bit = IMM_CHARM;	break;
	case(DAM_SOUND):	bit = IMM_SOUND;	break;
	default:		return def;
    }

    if (IS_SET(ch->imm_flags,bit))
	immune = IS_IMMUNE;
    else if (IS_SET(ch->res_flags,bit) && immune != IS_IMMUNE)
	immune = IS_RESISTANT;
    else if (IS_SET(ch->vul_flags,bit))
    {
	if (immune == IS_IMMUNE)
	    immune = IS_RESISTANT;
	else if (immune == IS_RESISTANT)
	    immune = IS_NORMAL;
	else
	    immune = IS_VULNERABLE;
    }

    if (immune == -1)
	return def;
    else
      	return immune;
}
/* END */

/* 
 * Returns a flag for wiznet 
 */
long wiznet_lookup (const char *name)
{
    int flag;

    for (flag = 0; wiznet_table[flag].name != NULL; flag++)
    {
	if (LOWER(name[0]) == LOWER(wiznet_table[flag].name[0])
	&& !str_prefix(name,wiznet_table[flag].name))
	    return flag;
    }

    return -1;
}
int strlen_wo_col( char *argument )
{
  char *str;
  bool found = FALSE;
  int colfound = 0;
  int ampfound = 0;
  int len;
  for ( str = argument; *str != '\0'; str++ )
    {
    if ( found && is_colcode( *str ) )
	{
	colfound++;
	found = FALSE;
	}
    if ( found && *str == '&' )
	ampfound++;
    if ( *str == '&' )
	found = found ? FALSE : TRUE;
    else
	found = FALSE;
    }
  len = strlen( argument );
  len = len - ampfound - ( colfound * 2 );
  return len;
}
char *strip_color( char *argument )
{
  char *str;
  char new_str [ MAX_INPUT_LENGTH ];
  int i = 0;
  for ( str = argument; *str != '\0'; str++ )
    {
    if ( new_str[ i-1 ] == '&' && is_colcode( *str ) )
	{
	i--;
	continue;
	}
    if ( new_str[ i-1 ] == '&' && *str == '&' )
	continue;
    new_str[i] = *str;
    i++;
    }
  if ( new_str[i] != '\0' )
	new_str[i] = '\0';
//  return strdup( new_str );
  str = str_dup( (char *)new_str );
  return str;
}

bool is_colcode( char code )
{ /* It ain't pretty, but it works :) -- Hannibal */
  if ( code == 'r'
    || code == 'R'
    || code == 'b'
    || code == 'B'
    || code == 'g'
    || code == 'G'
    || code == 'w'
    || code == 'W'
    || code == 'p'
    || code == 'P'
    || code == 'Y'
    || code == 'O'
    || code == 'c'
    || code == 'C'
    || code == 'z' 
    || code == '.' )
	return TRUE;
  return FALSE;
}
int xp_tolvl( CHAR_DATA *ch ) 
{
    int xp_tolvl, base_xp;
    int level = ch->level + 1;
    int classes = number_classes( ch );
	int mod = ch->incarnations +1;

/*   if ( IS_NPC( ch ) || ch->level >= L_CHAMP3 )
	return ch->exp; */
    if ( IS_NPC( ch ) || ch->level > (LEVEL_HERO -1) )
	return ch->exp+1;
    if ( ch->level < LEVEL_HERO )
    xp_tolvl = classes == 1 ? level * 1000 : level * classes * 1500;  /* used to be 2000 - Angi */
     if ( IS_SET( ch->act2, PLR_REMORT ))
    xp_tolvl = classes == 1 ? level * mod * 1000 : level * classes * mod * 1500;
    return xp_tolvl;
}

int mmlvl_mana( CHAR_DATA *ch, int sn )
{
  int level = 1;
  int iClass = 0;
  if ( IS_NPC( ch ) )
      return TRUE;
  for ( iClass = 0; ch->class[iClass] != -1; iClass++ )
    {
    if ( ch->level >= skill_table[sn].skill_level[ch->class[iClass]] )
      level = skill_table[sn].skill_level[ch->class[iClass]];
      return level;
    }
  return 1;
}

CHAR_DATA *rand_figment( CHAR_DATA *ch )
{
         DESCRIPTOR_DATA *d;
         CHAR_DATA	*figment;
         bool		dfig = FALSE;

  	  for ( d = descriptor_list; d; d = d->next )
	  {
	    if ( d->connected == CON_PLAYING
		&& d->character != ch
		&& d->character->in_room
		&& can_see( ch, d->character ) )
            {
             dfig = TRUE;
	     break;
            }
	  }

         if ( !dfig )
          figment = ch;
         else
          figment = d->character;

 return figment;
}

MOB_INDEX_DATA *rand_figment_mob( CHAR_DATA *ch )
{
 MOB_INDEX_DATA *figmentmob;
 int		figmentnum = 0;
 int		lfig = 0;
 int		ufig = 0;


          lfig = ch->in_room->area->lvnum;
          ufig = ch->in_room->area->uvnum;

          while( !(figmentmob = get_mob_index( figmentnum ) ) )
           figmentnum = number_range( lfig, ufig );

 return figmentmob;
}

OBJ_INDEX_DATA *rand_figment_obj( CHAR_DATA *ch )
{
 OBJ_INDEX_DATA *figmentobj;
 int		figmentnum = 0;
 int		lfig = 0;
 int		ufig = 0;


          lfig = ch->in_room->area->lvnum;
          ufig = ch->in_room->area->uvnum;

          while( !(figmentobj = get_obj_index( figmentnum ) ) )
           figmentnum = number_range( lfig, ufig );

 return figmentobj;
}

