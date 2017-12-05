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

#if defined( macintosh )
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


/*
 * Local functions.
 */
#define CD CHAR_DATA
void    get_obj         args( ( CHAR_DATA *ch, OBJ_DATA *obj,
			       OBJ_DATA *container ) );
void    wear_obj        args( ( CHAR_DATA *ch, OBJ_DATA *obj,
			       bool fReplace ) );
CD *    find_keeper     args( ( CHAR_DATA *ch, char *argument ) );
int     get_cost        args( ( CHAR_DATA *keeper, OBJ_DATA *obj,
			       bool fBuy ) );
AUCTION_DATA	*	auction;
#undef  CD



int parsebet (const int currentbet, const char *argument)
{

  int newbet = 0;                /* a variable to temporarily hold the new bet */
  char string[MAX_INPUT_LENGTH]; /* a buffer to modify the bet string */
  char *stringptr = string;      /* a pointer we can move around */

  strcpy (string,argument);      /* make a work copy of argument */


  if (*stringptr)               /* check for an empty string */
  {

    if (isdigit (*stringptr)) /* first char is a digit assume e.g. 433k */
      newbet = advatoi (stringptr); /* parse and set newbet to that value */

    else
      if (*stringptr == '+') /* add ?? percent */
      {
        if (strlen (stringptr) == 1) /* only + specified, assume default */
          newbet = (currentbet * 125) / 100; /* default: add 25% */
        else
          newbet = (currentbet * (100 + atoi (++stringptr))) / 100; /* cut off the first char */
      }
      else
        {
        printf ("considering: * x \n\r");
        if ((*stringptr == '*') || (*stringptr == 'x')) /* multiply */
          if (strlen (stringptr) == 1) /* only x specified, assume default */
            newbet = currentbet * 2 ; /* default: twice */
          else /* user specified a number */
            newbet = currentbet * atoi (++stringptr); /* cut off the first char */
        }
  }

  return newbet;        /* return the calculated bet */
}


void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    if ( !CAN_WEAR( obj, ITEM_TAKE ) )
    {
	send_to_char( "You can't take that.\n\r", ch );
	return;
    }

    if ( !( obj->item_type == ITEM_MONEY ) )
    {
	if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
	{
	    act( "$d: you can't carry that many items.",
		ch, NULL, obj->name, TO_CHAR );
	    return;
	}

	if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
	{
	    act( "$d: you can't carry that much weight.",
		ch, NULL, obj->name, TO_CHAR );
	    return;
	}
    }

    if ( container )
    {
	act( "You get $p from $P.", ch, obj, container, TO_CHAR );
	act( "$n gets $p from $P.", ch, obj, container, TO_ROOM );
	obj_from_obj( obj );
    }
    else
    {
	act( "You get $p.", ch, obj, container, TO_CHAR );
	act( "$n gets $p.", ch, obj, container, TO_ROOM );
	obj_from_room( obj );
    }

    if ( IS_SET( obj->extra_flags, ITEM_HIDDEN ) )
	REMOVE_BIT( obj->extra_flags, ITEM_HIDDEN );

    if ( IS_OBJ_STAT( obj, ITEM_HOLY  ) )
    {
	act( "You are burned by HOLY FIRE from $p.  Ouch!", ch, obj, NULL,
	    TO_CHAR );
	act( "$n is burned by HOLY FIRE from $p!", ch, obj, NULL, TO_ROOM );
	damage( ch, ch, 50, gsn_burning_hands, WEAR_NONE );
    }

    if ( obj->item_type == ITEM_MONEY )
    {
	char buf [ MAX_STRING_LENGTH ];
	int  amount;

	amount = obj->value[0];
	ch->gold += amount;

	if ( amount > 1 )
	{
	    sprintf( buf, "You counted %d coins.\n\r", amount );
	    send_to_char( buf, ch );
	}

	extract_obj( obj );
    }
    else
    {
	obj_to_char( obj, ch );
    }

    return;
}


/* Or do_take */
void do_get( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *container;
    char      arg1 [ MAX_INPUT_LENGTH ];
    char      arg2 [ MAX_INPUT_LENGTH ];
    bool      found;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    /* Get type. */
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Get what?\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj' */
	    obj = get_obj_list( ch, arg1, ch->in_room->contents );
	    if ( !obj )
	    {
		act( "I see no $T here.", ch, NULL, arg1, TO_CHAR );
		return;
	    }

	    get_obj( ch, obj, NULL );
	}
	else
	{
	    /* 'get all' or 'get all.obj' */
	    OBJ_DATA *obj_next;

	    found = FALSE;
	    for ( obj = ch->in_room->contents; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;

		if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		    && can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    get_obj( ch, obj, NULL );
		}
	    }

	    if ( !found ) 
	    {
		if ( arg1[3] == '\0' )
		    send_to_char( "I see nothing here.\n\r", ch );
		else
		    act( "I see no $T here.", ch, NULL, &arg1[4], TO_CHAR );
	    }
	}
    }
    else
    {
	/* 'get ... container' */
	if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}

	if ( !( container = get_obj_here( ch, arg2 ) ) )
	{
	    act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	    return;
	}

	switch ( container->item_type )
	{
	default:
	    send_to_char( "That's not a container.\n\r", ch );
	    return;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	    break;

	case ITEM_CORPSE_PC:
	{
	      CHAR_DATA *gch;
	      char      *pd;
	      char       name [ MAX_INPUT_LENGTH ];

	      if ( IS_NPC( ch ) )
	      {
		  send_to_char( "You can't do that.\n\r", ch );
		  return;
	      }

	      pd = container->short_descr;
	      pd = one_argument( pd, name );
	      pd = one_argument( pd, name );
	      pd = one_argument( pd, name );

	      if ( str_cmp( name, ch->name ) && !IS_IMMORTAL( ch ) )
	      {
		  bool fGroup;
		  
		  fGroup = FALSE;
		  for ( gch = char_list; gch; gch = gch->next )
		  {
		      if ( !IS_NPC( gch )
			  && is_same_group( ch, gch )
			  && !str_cmp( name, gch->name ) )
		      {
			  fGroup = TRUE;
			  break;
		      }
		  }
		  if ( !fGroup )
		  {
		      send_to_char( "You can't do that.\n\r", ch );
		      return;
		  }
	      }
	  }
	}

	if ( IS_SET( container->value[1], CONT_CLOSED ) )
	{
	    act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	    return;
	}

	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
	    /* 'get obj container' */
	    obj = get_obj_list( ch, arg1, container->contains );
	    if ( !obj )
	    {
		act( "I see nothing like that in the $T.",
		    ch, NULL, arg2, TO_CHAR );
		return;
	    }
	    get_obj( ch, obj, container );
	}
	else
	{
	    /* 'get all container' or 'get all.obj container' */
	    OBJ_DATA *obj_next;

	    found = FALSE;
	    for ( obj = container->contains; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;

		if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		    && can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    get_obj( ch, obj, container );
		}
	    }

	    if ( !found )
	    {
		if ( arg1[3] == '\0' )
		    act( "I see nothing in the $T.",
			ch, NULL, arg2, TO_CHAR );
		else
		    act( "I see nothing like that in the $T.",
			ch, NULL, arg2, TO_CHAR );
	    }
	}
    }

    return;
}



void do_put( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *container;
    OBJ_DATA *obj;
    char      arg1 [ MAX_INPUT_LENGTH ];
    char      arg2 [ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Put what in what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( !( container = get_obj_here( ch, arg2 ) ) )
    {
	act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
	return;
    }

    if ( container->item_type != ITEM_CONTAINER )
    {
	send_to_char( "That's not a container.\n\r", ch );
	return;
    }

    if ( IS_SET( container->value[1], CONT_CLOSED ) )
    {
	act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
	return;
    }

    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
	/* 'put obj container' */
	if ( !( obj = get_obj_carry( ch, arg1 ) ) )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( obj == container )
	{
	    send_to_char( "You can't fold it into itself.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	if ( get_obj_weight( obj ) + get_obj_weight( container )
	     > container->value[0] )
	{
	    send_to_char( "It won't fit.\n\r", ch );
	    return;
	}

	obj_from_char( obj );
	obj_to_obj( obj, container );
	act( "You put $p in $P.", ch, obj, container, TO_CHAR );
	act( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
    }
    else
    {
	/* 'put all container' or 'put all.obj container' */
	OBJ_DATA *obj_next;

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		&& can_see_obj( ch, obj )
		&& obj->wear_loc == WEAR_NONE
		&& obj != container
		&& can_drop_obj( ch, obj )
		&& get_obj_weight( obj ) + get_obj_weight( container )
		   <= container->value[0] )
	    {
		obj_from_char( obj );
		obj_to_obj( obj, container );
		act( "You put $p in $P.", ch, obj, container, TO_CHAR );
		act( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
	    }
	}
    }

    return;
}



void do_drop( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    bool      found;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Drop what?\n\r", ch );
	return;
    }

    if ( is_number( arg ) )
    {
	/* 'drop NNNN coins' */
	OBJ_DATA *obj_next;
	int       amount;

	amount   = atoi( arg );
	one_argument( argument, arg );

	if ( amount <= 0 || str_prefix( arg, "coins" ) )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}

	if ( ch->gold < amount )
	{
	    send_to_char( "You haven't got that many coins.\n\r", ch );
	    return;
	}

	ch->gold -= amount;

	for ( obj = ch->in_room->contents; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( obj->deleted )
		continue;

	    switch ( obj->pIndexData->vnum )
	    {
	    case OBJ_VNUM_MONEY_ONE:
		amount += 1;
		extract_obj( obj );
		break;

	    case OBJ_VNUM_MONEY_SOME:
		amount += obj->value[0];
		extract_obj( obj );
		break;
	    }
	}

	obj_to_room( create_money( amount ), ch->in_room );
	send_to_char( "OK.\n\r", ch );
	act( "$n drops some gold.", ch, NULL, NULL, TO_ROOM );
	return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
	/* 'drop obj' */
	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	obj_from_char( obj );
	obj_to_room( obj, ch->in_room );
	act( "You drop $p.", ch, obj, NULL, TO_CHAR );
	act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
	if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
	{
		act("$p dissolves into smoke.",ch,obj,NULL,TO_ROOM);
		act("$p dissolves into smoke.",ch,obj,NULL,TO_CHAR);
		extract_obj(obj);
	}
    }
    else
    {
	/* 'drop all' or 'drop all.obj' */
	OBJ_DATA *obj_next;

	found = FALSE;
	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
		&& can_see_obj( ch, obj )
		&& obj->wear_loc == WEAR_NONE
		&& can_drop_obj( ch, obj ) )
	    {
		found = TRUE;
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
		act( "You drop $p.", ch, obj, NULL, TO_CHAR );
		act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
	        if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
		{
			act("$p dissolves into smoke.",ch,obj,NULL,TO_ROOM);
			act("$p dissolves into smoke.",ch,obj,NULL,TO_CHAR);
			extract_obj(obj);
		}
	    }
	}

	if ( !found )
	{
	    if ( arg[3] == '\0' )
		send_to_char( "You are not carrying anything.", ch );
	    else
		act( "You are not carrying any $T.",
		    ch, NULL, &arg[4], TO_CHAR );
	}
    }

    return;
}



void do_give( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Give what to whom?\n\r", ch );
	return;
    }

    if ( is_number( arg1 ) )
    {
	/* 'give NNNN coins victim' */
	char buf[ MAX_STRING_LENGTH ];
	int  amount;

	amount   = atoi( arg1 );
	if ( amount <= 0 || str_prefix( arg2, "coins" ) )
	{
	    send_to_char( "Sorry, you can't do that.\n\r", ch );
	    return;
	}

	one_argument( argument, arg2 );

	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Give what to whom?\n\r", ch );
	    return;
	}

	if ( !( victim = get_char_room( ch, arg2 ) ) )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( ch->gold < amount )
	{
	    send_to_char( "You haven't got that much gold.\n\r", ch );
	    return;
	}

	ch->gold     -= amount;
	victim->gold += amount;
	act( "You give $N some gold.",  ch, NULL, victim, TO_CHAR    );

	sprintf( buf, "$n gives you %d gold.", amount );
	act( buf, ch, NULL, victim, TO_VICT    );

	act( "$n gives $N some gold.",  ch, NULL, victim, TO_NOTVICT );
	mprog_bribe_trigger( victim, ch, amount );
	return;
    }

    if ( !( obj = get_obj_carry( ch, arg1 ) ) )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	send_to_char( "You must remove it first.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_HOLY )
	&& victim->race == race_lookup( "Vampire" ) )
    {
	act( "$N refuses the $p.", ch, obj, victim, TO_CHAR );
	act( "$n tries to give $N a $p but $E refuses.",
	    ch, obj, victim, TO_ROOM );
	act( "You refuse to accept the $p from $n.",
	    ch, obj, victim, TO_VICT );
	return;
    }

    if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
    {
	act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
    {
	act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
	act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
	return;
    }

    obj_from_char( obj );
    obj_to_char( obj, victim );
    act( "You give $p to $N.", ch, obj, victim, TO_CHAR    );
    act( "$n gives you $p.",   ch, obj, victim, TO_VICT    );
    act( "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT );
    mprog_give_trigger( victim, ch, obj );
    return;
}




void do_fill( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *fountain;
    char      arg [ MAX_INPUT_LENGTH ];
    bool      found;

    if ( !check_blind( ch ) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Fill what?\n\r", ch );
	return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    found = FALSE;
    for ( fountain = ch->in_room->contents; fountain;
	fountain = fountain->next_content )
    {
	if ( fountain->item_type == ITEM_FOUNTAIN )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
    {
	send_to_char( "There is no fountain here!\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "You can't fill that.\n\r", ch );
	return;
    }

    if ( obj->value[1] != 0 && obj->value[2] != 0 )
    {
	send_to_char( "There is already another liquid in it.\n\r", ch );
	return;
    }

    if ( obj->value[1] >= obj->value[0] )
    {
	send_to_char( "Your container is full.\n\r", ch );
	return;
    }

    act( "You fill $p.", ch, obj, NULL, TO_CHAR );
    obj->value[2] = 0;
    obj->value[1] = obj->value[0];
    return;
}



void do_drink( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];
    int       amount;
    int       liquid;

    if ( !check_blind( ch ) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	{
	    if ( obj->item_type == ITEM_FOUNTAIN )
		break;
	}

	if ( !obj )
	{
	    send_to_char( "Drink what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( !( obj = get_obj_here( ch, arg ) ) )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_DRUNK] > 10 )
    {
	send_to_char( "You fail to reach your mouth.  *Hic*\n\r", ch );
	return;
    }

    switch ( obj->item_type )
    {
    default:
	send_to_char( "You can't drink from that.\n\r", ch );
	break;

    case ITEM_FOUNTAIN:
	if ( ch->race == race_lookup( "vampire" ) )
	{
	    send_to_char( "You can't drink from that.\n\r", ch );
	    break;
	}

	if ( !IS_NPC( ch ) )
	    ch->pcdata->condition[COND_THIRST] = 48;
	send_to_char( "You drink from the fountain.\n\r", ch );
	send_to_char( "You are not thirsty.\n\r", ch );
	act( "$n drinks from the fountain.", ch, NULL, NULL, TO_ROOM );
	break;

    case ITEM_DRINK_CON:
	if ( obj->value[1] <= 0 )
	{
	    send_to_char( "It is already empty.\n\r", ch );
	    return;
	}

	if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->value[2] = 0;
	}

	act( "You drink $T from $p.",
	    ch, obj, liq_table[liquid].liq_name, TO_CHAR );
	act( "$n drinks $T from $p.",
	    ch, obj, liq_table[liquid].liq_name, TO_ROOM );

	amount = number_range( 3, 10 );
	amount = UMIN( amount, obj->value[1] );
	
	gain_condition( ch, COND_DRUNK,
	    amount * liq_table[liquid].liq_affect[COND_DRUNK  ] );
	if ( ch->race != race_lookup( "vampire" ) )
	{
	    gain_condition( ch, COND_FULL,
			 amount * liq_table[liquid].liq_affect[COND_FULL   ] );
	    gain_condition( ch, COND_THIRST,
			 amount * liq_table[liquid].liq_affect[COND_THIRST ] );
	}
	else
	    /* If blood */
	    if ( liquid == LIQ_BLOOD )
	    {
		gain_condition( ch, COND_FULL, amount * 2 );
		gain_condition( ch, COND_THIRST, amount );
	    }

	if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_DRUNK ] > 10 )
	    send_to_char( "You feel drunk.\n\r", ch );
	if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL  ] > 40 )
	    send_to_char( "You are full.\n\r", ch );
	if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_THIRST] > 40 )
	    send_to_char( "You do not feel thirsty.\n\r", ch );
	
	if ( obj->value[3] != 0
	    && ( ch->race != race_lookup( "Vampire" )
		&& ch->race != race_lookup( "Undead" ) ) )
	{
	    /* The shit was poisoned ! */
	    AFFECT_DATA af;

	    send_to_char( "You choke and gag.\n\r", ch );
	    act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
	    af.type      = gsn_poison;
	    af.duration  = 3 * amount;
	    af.location  = APPLY_STR;
	    af.modifier  = -2;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	
	obj->value[1] -= amount;
	if ( obj->value[1] <= 0 )
	{
	    send_to_char( "The empty container vanishes.\n\r", ch );
	    extract_obj( obj );
	}
	break;
    }

    return;
}



void do_eat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];

    if ( !check_blind( ch ) )
	return;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Eat what?\n\r", ch );
	return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( !IS_IMMORTAL( ch ) )
    {
	if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL )
	{
	    send_to_char( "That's not edible.\n\r", ch );
	    return;
	}

	if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL] > 40 )
	{   
	    send_to_char( "You are too full to eat more.\n\r", ch );
	    return;
	}
    }

    act( "You eat $p.", ch, obj, NULL, TO_CHAR );
    act( "$n eats $p.", ch, obj, NULL, TO_ROOM );

    switch ( obj->item_type )
    {

    case ITEM_FOOD:
	if ( !IS_NPC( ch ) )
	{
	    int condition;

	    condition = ch->pcdata->condition[COND_FULL];
	    if ( ch->race != race_lookup( "vampire" ) )
		gain_condition( ch, COND_FULL, obj->value[0] );
	    if ( ch->pcdata->condition[COND_FULL] > 40 )
		send_to_char( "You are full.\n\r", ch );
	    else if ( condition == 0 && ch->pcdata->condition[COND_FULL] > 0 )
		send_to_char( "You are no longer hungry.\n\r", ch );
	}

	if ( obj->value[3] != 0
	    && ( ch->race != race_lookup( "Vampire" )
		&& ch->race != race_lookup( "Undead" ) ) )
	{
	    /* The shit was poisoned! */
	    AFFECT_DATA af;

	    act( "$n chokes and gags.", ch, 0, 0, TO_ROOM );
	    send_to_char( "You choke and gag.\n\r", ch );

	    af.type      = gsn_poison;
	    af.duration  = 2 * obj->value[0];
	    af.location  = APPLY_STR;
	    af.modifier  = -2;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	break;

    case ITEM_PILL:
	if ( obj->level > ch->level )
	    act( "$p is too high level for you.", ch, obj, NULL, TO_CHAR );
	else
	{
	    obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
	    obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
	    obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
	}
	break;
    }

    if ( !IS_NPC( ch )
	|| ( IS_NPC( ch ) && is_affected( ch, gsn_charm_person ) ) )
	extract_obj( obj );
    return;
}



/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;

    if ( !( obj = get_eq_char( ch, iWear ) ) )
	return TRUE;

    if ( !fReplace )
	return FALSE;

    if (( IS_SET( obj->extra_flags, ITEM_NOREMOVE ) && (!IS_IMMORTAL(ch)) ))
    {
	act( "You can't remove $p.", ch, obj, NULL, TO_CHAR );
	return FALSE;
    }

    unequip_char( ch, obj );
    act( "$n stops using $p.", ch, obj, NULL, TO_ROOM );
    act( "You stop using $p.", ch, obj, NULL, TO_CHAR );
    return TRUE;
}



/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
    char buf [ MAX_STRING_LENGTH ];

    if ( ch->level < obj->level )
    {
	sprintf( buf, "You must be level %d to use this object.\n\r",
	    obj->level );
	send_to_char( buf, ch );
	act( "$n tries to use $p, but is too inexperienced.",
	    ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( obj->item_type == ITEM_LIGHT )
    {
	if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
	    return;
	act( "You light $p and hold it.",  ch, obj, NULL, TO_CHAR );
	act( "$n lights $p and holds it.", ch, obj, NULL, TO_ROOM );
	equip_char( ch, obj, WEAR_LIGHT );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
    {
	if ( get_eq_char( ch, WEAR_FINGER_L )
	&&   get_eq_char( ch, WEAR_FINGER_R )
	&&   !remove_obj( ch, WEAR_FINGER_L, fReplace )
	&&   !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
	    return;

	if ( !get_eq_char( ch, WEAR_FINGER_L ) )
	{
	    act( "You wear $p on your left finger.",  ch, obj, NULL, TO_CHAR );
	    act( "$n wears $p on $s left finger.",    ch, obj, NULL, TO_ROOM );
	    equip_char( ch, obj, WEAR_FINGER_L );
	    return;
	}

	if ( !get_eq_char( ch, WEAR_FINGER_R ) )
	{
	    act( "You wear $p on your right finger.", ch, obj, NULL, TO_CHAR );
	    act( "$n wears $p on $s right finger.",   ch, obj, NULL, TO_ROOM );
	    equip_char( ch, obj, WEAR_FINGER_R );
	    return;
	}

	bug( "Wear_obj: no free finger.", 0 );
	send_to_char( "You already wear two rings.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
    {
	if ( get_eq_char( ch, WEAR_NECK_1 )
	&&   get_eq_char( ch, WEAR_NECK_2 )
	&&   !remove_obj( ch, WEAR_NECK_1, fReplace )
	&&   !remove_obj( ch, WEAR_NECK_2, fReplace ) )
	    return;

	if ( !get_eq_char( ch, WEAR_NECK_1 ) )
	{
	    act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
	    act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
	    equip_char( ch, obj, WEAR_NECK_1 );
	    return;
	}

	if ( !get_eq_char( ch, WEAR_NECK_2 ) )
	{
	    act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
	    act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
	    equip_char( ch, obj, WEAR_NECK_2 );
	    return;
	}

	bug( "Wear_obj: no free neck.", 0 );
	send_to_char( "You already wear two neck items.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
    {
	if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
	    return;
	act( "You wear $p on your body.", ch, obj, NULL, TO_CHAR );
	act( "$n wears $p on $s body.",   ch, obj, NULL, TO_ROOM );
	equip_char( ch, obj, WEAR_BODY );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
    {
	if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
	    return;
	act( "You wear $p on your head.", ch, obj, NULL, TO_CHAR );
	act( "$n wears $p on $s head.",   ch, obj, NULL, TO_ROOM );
	equip_char( ch, obj, WEAR_HEAD );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
    {
	if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
	    return;
	act( "You wear $p on your legs.", ch, obj, NULL, TO_CHAR );
	act( "$n wears $p on $s legs.",   ch, obj, NULL, TO_ROOM );
	equip_char( ch, obj, WEAR_LEGS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
    {
	if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
	    return;
	act( "You wear $p on your feet.", ch, obj, NULL, TO_CHAR );
	act( "$n wears $p on $s feet.",   ch, obj, NULL, TO_ROOM );
	equip_char( ch, obj, WEAR_FEET );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
    {
	if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
	    return;
	act( "You wear $p on your hands.", ch, obj, NULL, TO_CHAR );
	act( "$n wears $p on $s hands.",   ch, obj, NULL, TO_ROOM );
	equip_char( ch, obj, WEAR_HANDS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
    {
	if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
	    return;
	act( "You wear $p on your arms.", ch, obj, NULL, TO_CHAR );
	act( "$n wears $p on $s arms.",   ch, obj, NULL, TO_ROOM );
	equip_char( ch, obj, WEAR_ARMS );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
    {
	if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
	    return;
	act( "You wear $p about your body.", ch, obj, NULL, TO_CHAR );
	act( "$n wears $p about $s body.",   ch, obj, NULL, TO_ROOM );
	equip_char( ch, obj, WEAR_ABOUT );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
    {
	if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
	    return;
	act( "You wear $p about your waist.", ch, obj, NULL, TO_CHAR );
	act( "$n wears $p about $s waist.",   ch, obj, NULL, TO_ROOM );
	equip_char( ch, obj, WEAR_WAIST );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
    {
	if ( get_eq_char( ch, WEAR_WRIST_L )
	&&   get_eq_char( ch, WEAR_WRIST_R )
	&&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
	&&   !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
	    return;

	if ( !get_eq_char( ch, WEAR_WRIST_L ) )
	{
	    act( "You wear $p around your left wrist.",
		ch, obj, NULL, TO_CHAR );
	    act( "$n wears $p around $s left wrist.",
		ch, obj, NULL, TO_ROOM );
	    equip_char( ch, obj, WEAR_WRIST_L );
	    return;
	}

	if ( !get_eq_char( ch, WEAR_WRIST_R ) )
	{
	    act( "You wear $p around your right wrist.",
		ch, obj, NULL, TO_CHAR );
	    act( "$n wears $p around $s right wrist.",
		ch, obj, NULL, TO_ROOM );
	    equip_char( ch, obj, WEAR_WRIST_R );
	    return;
	}

	bug( "Wear_obj: no free wrist.", 0 );
	send_to_char( "You already wear two wrist items.\n\r", ch );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
    {
	if ( !remove_obj( ch, WEAR_WIELD_2, fReplace ) )
	    return;
	if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
	    return;
	act( "$n wears $p as a shield.", ch, obj, NULL, TO_ROOM );
	act( "You wear $p as a shield.", ch, obj, NULL, TO_CHAR );
	equip_char( ch, obj, WEAR_SHIELD );
	return;
    }

    if ( CAN_WEAR( obj, ITEM_WIELD ) )
    {
	if( IS_NPC( ch )
	   || ch->level >= skill_table[gsn_dual].skill_level[ch->class] )
	{
	    /*
	     * If you think this looks ugly now, just imagine how
	     * I felt trying to write it!  --- Thelonius (Monk)
	     * Now, it will attempt to wear in 1 if open, then 2 if
	     * open, else replace 2, else replace 1.
	     */
	    if ( get_eq_char( ch, WEAR_WIELD  )
		&& ( ( get_eq_char( ch, WEAR_WIELD_2 )
		      && !remove_obj( ch, WEAR_WIELD_2, fReplace ) )
		    || !remove_obj( ch, WEAR_HOLD,    fReplace )
		    || !remove_obj( ch, WEAR_SHIELD,  fReplace ) )
		&&   !remove_obj( ch, WEAR_WIELD, fReplace ) )
		return;

	    if ( !get_eq_char( ch, WEAR_WIELD ) )
	    {
		OBJ_DATA *other_weapon;
		int       weight = 0;

		if ( ( other_weapon = get_eq_char( ch, WEAR_WIELD_2 ) ) )
		    weight = get_obj_weight( other_weapon );

		if ( weight + get_obj_weight( obj )
		    > str_app[get_curr_str( ch )].wield )
		{
		    send_to_char( "It is too heavy for you to wield.\n\r",
				 ch );
		    return;
		}

		act( "You wield $p.", ch, obj, NULL, TO_CHAR );
		act( "$n wields $p.", ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_WIELD );
		return;
	    }

	    if ( !get_eq_char( ch, WEAR_WIELD_2 ) )
	    {
		OBJ_DATA *primary_weapon;
		int       weight;

		primary_weapon = get_eq_char( ch, WEAR_WIELD );
		weight = get_obj_weight( primary_weapon );

		if ( weight + get_obj_weight( obj )
		    > str_app[get_curr_str( ch )].wield )
		{
		    send_to_char( "It is too heavy for you to wield.\n\r",
				 ch );
		    return;
		}

		act( "You wield $p.", ch, obj, NULL, TO_CHAR );
		act( "$n wields $p.", ch, obj, NULL, TO_ROOM );
		equip_char( ch, obj, WEAR_WIELD_2 );
		return;
	    }
	    bug( "Wear_obj: no free weapon slot.", 0 );
	    send_to_char( "You already wield two weapons.\n\r", ch );
	    return;
	}
	else    /* can only wield one weapon */
	{
	    if ( !remove_obj( ch, WEAR_WIELD, fReplace ) )
		return;

	    if ( get_obj_weight( obj ) > str_app[get_curr_str( ch )].wield )
	    {
		send_to_char( "It is too heavy for you to wield.\n\r", ch );
		return;
	    }

	    act( "You wield $p.", ch, obj, NULL, TO_CHAR );
	    act( "$n wields $p.", ch, obj, NULL, TO_ROOM );
	    equip_char( ch, obj, WEAR_WIELD );
	    return;
	}
    }

    if ( CAN_WEAR( obj, ITEM_HOLD ) )
    {
	if ( !remove_obj( ch, WEAR_WIELD_2, fReplace ) )
	    return;
	if ( !remove_obj( ch, WEAR_HOLD,    fReplace ) )
	    return;
	act( "You hold $p in your hands.", ch, obj, NULL, TO_CHAR );
	act( "$n holds $p in $s hands.",   ch, obj, NULL, TO_ROOM );
	equip_char( ch, obj, WEAR_HOLD );
	return;
    }

    if ( fReplace )
	send_to_char( "You can't wear, wield, or hold that.\n\r", ch );

    return;
}



void do_wear( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch ) && IS_AFFECTED( ch, AFF_GHOUL ) )
    {
	send_to_char(
	   "You may not wear, wield, or hold anything in ghoul form.\n\r",
		     ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Wear, wield, or hold what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	OBJ_DATA *obj_next;

	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( obj->wear_loc != WEAR_NONE || !can_see_obj( ch, obj ) )
		continue;

	    if ( CAN_WEAR( obj, ITEM_WIELD )
		&& !IS_SET( race_table[ ch->race ].race_abilities,
			   RACE_WEAPON_WIELD ) )
		continue;

	    wear_obj( ch, obj, FALSE );
	}
	return;
    }
    else
    {
	if ( !( obj = get_obj_carry( ch, arg ) ) )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( CAN_WEAR( obj, ITEM_WIELD )
	    && !IS_SET( race_table[ ch->race ].race_abilities,
		       RACE_WEAPON_WIELD ) )
	{
	    send_to_char( "You are not able to wield a weapon.\n\r", ch );
	    return;
	}

	wear_obj( ch, obj, TRUE );
    }

    return;
}



void do_remove( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Remove what?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {	/* Remove all - Canth */
	while ( ( obj = get_obj_wear( ch, "" ) ) != NULL)
	/* This while works, but it gives a stupid error during compile
	 * Anyone got a fix for it? - Canth */
	    do_remove( ch, obj->name );
	return;
    }

    if ( !( obj = get_obj_wear( ch, arg ) ) )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    remove_obj( ch, obj->wear_loc, TRUE );
    return;
}



void do_sacrifice( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA	*obj;
    char	arg [ MAX_INPUT_LENGTH ];
    char	godname[15];
    int		gain = 0;

#if defined (SAC_GODNAMES)
    /* Godname idea from CU-mud, code by The Maniac from Mythran */
    static char * god_name_table [ ] =		/* Numbered from 0 !! */
    {
	"Maniac", "Canth", "Phule", "Kahn", "Hatchet",
	"Furey", "Alander", "God", "Someone", "Mythran"
    };

    strcpy(godname, god_name_table[number_range(0,9)]);
#else
    strcpy(godname, "God");
#endif

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !str_cmp( arg, ch->name ) )
    {
	send_to_char(
	    "God appreciates your offer and may accept it later.", ch );
	act( "$n offers $mself to God, who graciously declines.",
	    ch, NULL, NULL, TO_ROOM );
	return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( !obj )
    {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }

    if ( !CAN_WEAR( obj, ITEM_TAKE ) )
    {
	act( "$p is not an acceptable sacrifice.", ch, obj, NULL, TO_CHAR );
	return;
    }
    gain = UMIN(number_range(1, obj->level), ch->level);

#if defined (SAC_VAR_REWARD)
    /* Idea by Bram (Unicorn Mud) Mythran Code by Maniac */
    switch(number_range(1, 50))
    {	/* Approx. 42% chance at something special.. 6% at something real good */
	    default:
	    	sprintf( arg, "%s gives you %d gold coin%s for your sacrifice.\n\r",
		    godname, gain, ((gain == 1) ? "" : "s") );
    		ch->gold += gain;
		break;
	    case 1:
	    	sprintf( arg, "%s gives you 1 permanent move point for your sacrifice!\n\r",
		    godname );
		ch->max_move += 1;
		break;
	    case 2:
	    	sprintf( arg, "%s gives you 1 permanent hit point for your sacrifice!\n\r",
			godname );
		ch->max_hit += 1;
		break;
	    case 3:
		sprintf( arg, "%s gives you 1 permanent mana point for your sacrifice!\n\r",
		    godname );
		ch->max_mana += 1;
		break;
	    case 4:
	    case 5:
	    case 6:
		sprintf(arg, "%s gives you %d move points for yor sacrifice.\n\r",
		    godname, gain );
		ch->move += gain;
		break;
	    case 7:
	    case 8:
	    case 9:
		sprintf( arg, "%s gives you %d hit points for your sacrifice.\n\r",
		    godname, gain );
		ch->hit += gain;
		break;
	    case 10:
	    case 11:
	    case 12:
		sprintf( arg, "%s gives you %d mana points for your sacrifice.\n\r",
		    godname, gain );
		ch->mana +=gain;
		break;
	    case 13:
	    case 14:
	    case 15:
	    	if (IS_NPC(ch))
		{
			sprintf(arg, "%s gives you 1 gp for your sacrifice.\n\r",
			    godname);
			ch->gold += 1;
		}
		else
		{
	    		sprintf(arg, "%s gives you 1 learning session for your sacrifice.\n\r",
			    godname);
			ch->pcdata->learn += 1;
		}
		break;
	    case 16:
	    case 17:
	    case 18:
	    	sprintf(arg, "%s gives you %d experience point%s for your sacrifice.\n\r",
		    godname, gain, (gain == 1) ? "" : "s");
		ch->exp += gain;
		break;
	    case 19:
	    case 20:
	    case 21:
		sprintf( arg, "%s gives you 1 practice session for your sacrifice.\n\r",
		    godname );
		ch->practice += 1;
		break;
    }
#else
	sprintf(arg, "%s gives you 1 gp for your sacrifice.\n\r", godname);
	ch->gold += 1;
#endif

    send_to_char(arg, ch);
    sprintf (arg, "%s: %s(%d).", ch->name,
	obj->short_descr, obj->pIndexData->vnum );
    act( "$n sacrifices $p to God.", ch, obj, NULL, TO_ROOM );
    wiznet (ch, WIZ_SACCING, get_trust(ch), arg );
    extract_obj( obj );
    return;
}



void do_quaff( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      arg [ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Quaff what?\n\r", ch );
	return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	send_to_char( "You do not have that potion.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_POTION )
    {
	send_to_char( "You can quaff only potions.\n\r", ch );
	return;
    }

    act( "You quaff $p.", ch, obj, NULL ,TO_CHAR );
    act( "$n quaffs $p.", ch, obj, NULL, TO_ROOM );

    if ( obj->level > ch->level ) 
	act( "$p is too high level for you.", ch, obj, NULL, TO_CHAR );
    else
    {
	/* obj->value[0] is not used for potions */
	obj_cast_spell( obj->value[1], obj->level, ch, ch, NULL );
	obj_cast_spell( obj->value[2], obj->level, ch, ch, NULL );
	obj_cast_spell( obj->value[3], obj->level, ch, ch, NULL );
    }

    if ( !IS_NPC( ch )
	|| ( IS_NPC( ch ) && is_affected( ch, gsn_charm_person ) ) )
	extract_obj( obj );
    return;
}



void do_recite( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *scroll;
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( !( scroll = get_obj_carry( ch, arg1 ) ) )
    {
	send_to_char( "You do not have that scroll.\n\r", ch );
	return;
    }

    if ( scroll->item_type != ITEM_SCROLL )
    {
	send_to_char( "You can recite only scrolls.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg2[0] == '\0' )
    {
	victim = ch;
	if ( ch->fighting )
	    victim = ch->fighting;
    }
    else
    {
	if ( !( victim = get_char_room ( ch, arg2 ) )
	    && !( obj  = get_obj_here  ( ch, arg2 ) ) )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    if ( IS_AFFECTED( ch, AFF_MUTE )
	|| IS_SET( ch->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
    {
	send_to_char( "You can't seem to break the silence.\n\r", ch );
	return;
    }

    if ( ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
	|| IS_AFFECTED( ch, AFF_CHARM ) )
    {
	act( "You try to recite $p, but you have no free will.",
	    ch, scroll, NULL, TO_CHAR );
	act( "$n tries to recite $p, but has no free will.", 
	    ch, scroll, NULL, TO_ROOM );
	return;
    }

    act( "You recite $p.", ch, scroll, NULL, TO_CHAR );
    act( "$n recites $p.", ch, scroll, NULL, TO_ROOM );

    /* Scrolls skill by Binky for EnvyMud, modified by Thelonius */
    if ( !IS_NPC( ch )
	&& !( number_percent( ) < ch->pcdata->learned[gsn_scrolls] ) )
    {
	switch ( number_bits( 3 ) )
	{
	case 0: 
	case 1:                      
	case 2:
	case 3: 
	    act( "You can't understand $p at all.",
		ch, scroll, NULL, TO_CHAR );
	    act( "$n can't understand $p at all.",
		ch, scroll, NULL, TO_ROOM );
	    return;                    
	case 4:                
	case 5:                      
	case 6:                      
	    send_to_char( "You must have said something incorrectly.\n\r",
			 ch );
	    act( "$n must have said something incorrectly.",
		ch, NULL,   NULL, TO_ROOM );
	    act( "$p blazes brightly, then is gone.",
		ch, scroll, NULL, TO_CHAR );
	    act( "$p blazes brightly and disappears.",
		ch, scroll, NULL, TO_ROOM );
	    extract_obj( scroll );
	    return;
	case 7:
	    act( 
	"You completely botch the recitation, and $p bursts into flames!!", 
		ch, scroll, NULL, TO_CHAR );
	    act( "$p glows and then bursts into flame!", 
		ch, scroll, NULL, TO_ROOM );
	    /*
	     * damage( ) call after extract_obj in case the damage would
	     * have extracted ch.  This is okay because we merely mark
	     * obj->deleted; it still retains all values until list_update.
	     * Sloppy?  Okay, create another integer variable. ---Thelonius
	     */
	    extract_obj( scroll );
	    damage( ch, ch, scroll->level, gsn_scrolls, WEAR_NONE );
	    return;
	}
    }

    if ( scroll->level > ch->level ) 
	act( "$p is too high level for you.", ch, scroll, NULL, TO_CHAR );
    else
    {
	/* scroll->value[0] is not used for scrolls */
	obj_cast_spell( scroll->value[1], scroll->level, ch, victim, obj );
	obj_cast_spell( scroll->value[2], scroll->level, ch, victim, obj );
	obj_cast_spell( scroll->value[3], scroll->level, ch, victim, obj );
    }

    if ( !IS_NPC( ch )
	|| ( IS_NPC( ch ) && is_affected( ch, gsn_charm_person ) ) )
	extract_obj( scroll );
    return;
}



void do_brandish( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *staff;
    CHAR_DATA *vch;
    int        sn;

    if ( !( staff = get_eq_char( ch, WEAR_HOLD ) ) )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( staff->item_type != ITEM_STAFF )
    {
	send_to_char( "You can brandish only with a staff.\n\r", ch );
	return;
    }

    if ( ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
	|| IS_AFFECTED( ch, AFF_CHARM ) )
    {
	act( "You try to brandish $p, but you have no free will.",
	    ch, staff, NULL, TO_CHAR );
	act( "$n tries to brandish $p, but has no free will.", 
	    ch, staff, NULL, TO_ROOM );
	return;
    }

    if ( ( sn = staff->value[3] ) < 0
	|| sn >= MAX_SKILL
	|| skill_table[sn].spell_fun == 0 )
    {
	bug( "Do_brandish: bad sn %d.", sn );
	return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( staff->value[2] > 0 )
    {
	CHAR_DATA *vch_next;

	act( "You brandish $p.",  ch, staff, NULL, TO_CHAR );
	act( "$n brandishes $p.", ch, staff, NULL, TO_ROOM );

	/* Staves skill by Binky for EnvyMud, modified by Thelonius */
	if ( !IS_NPC( ch ) 
	    && !( number_percent( ) < ch->pcdata->learned[gsn_staves] ) )
	{ 
	    switch ( number_bits( 3 ) )
	    {
	    case 0: 
	    case 1:                      
	    case 2:                      
	    case 3: 
		act( "You are unable to invoke the power of $p.",
		    ch, staff, NULL, TO_CHAR );
		act( "$n is unable to invoke the power of $p.",
		    ch, staff, NULL, TO_ROOM );
		return;                    
	    case 4:                
	    case 5:                      
	    case 6:                      
		act( "You summon the power of $p, but it fizzles away.",
		    ch, staff, NULL, TO_CHAR );
		act( "$n summons the power of $p, but it fizzles away.",
		    ch, staff, NULL, TO_ROOM );
		if ( --staff->value[2] <= 0 )
		{
		    act( "$p blazes bright and is gone.",
			ch, staff, NULL, TO_CHAR );
		    act( "$p blazes bright and is gone.",
			ch, staff, NULL, TO_ROOM );
		    extract_obj( staff );
		}
		return;
	    case 7:
		act( "You can't control the power of $p, and it shatters!",
		    ch, staff, NULL, TO_CHAR );
		act( "$p shatters into tiny pieces!", 
		    ch, staff, NULL, TO_ROOM );
		/*
		 * damage( ) call after extract_obj in case the damage would
		 * have extracted ch.  This is okay because we merely mark
		 * obj->deleted; it still retains all values until list_update.
		 * Sloppy?  Okay, create another integer variable. ---Thelonius
		 */
		extract_obj( staff );
		damage( ch, ch, staff->level, gsn_staves, WEAR_NONE );
		return;
	    }
	}

	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next = vch->next_in_room;

	    if ( vch->deleted )
		continue;

	    switch ( skill_table[sn].target )
	    {
	    default:
		bug( "Do_brandish: bad target for sn %d.", sn );
		return;

	    case TAR_IGNORE:
		if ( vch != ch )
		    continue;
		break;

	    case TAR_CHAR_OFFENSIVE:
		if ( IS_NPC( ch ) ? IS_NPC( vch ) : !IS_NPC( vch ) )
		    continue;
		break;
		
	    case TAR_CHAR_DEFENSIVE:
		if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
		    continue;
		break;

	    case TAR_CHAR_SELF:
		if ( vch != ch )
		    continue;
		break;
	    }

	    /* staff->value[0] is not used for staves */
	    obj_cast_spell( staff->value[3], staff->level, ch, vch, NULL );
	}
    }

    if ( !IS_NPC( ch )
	|| ( IS_NPC( ch ) && is_affected( ch, gsn_charm_person ) ) )
	if ( --staff->value[2] <= 0 )
	{
	    act( "$p blazes bright and is gone.", ch, staff, NULL, TO_CHAR );
	    act( "$p blazes bright and is gone.", ch, staff, NULL, TO_ROOM );
	    extract_obj( staff );
	}

    return;
}



void do_zap( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *wand;
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       arg [ MAX_INPUT_LENGTH ];
    int        sn;

    one_argument( argument, arg );
    if ( arg[0] == '\0' && !ch->fighting )
    {
	send_to_char( "Zap whom or what?\n\r", ch );
	return;
    }

    if ( !( wand = get_eq_char( ch, WEAR_HOLD ) ) )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( wand->item_type != ITEM_WAND )
    {
	send_to_char( "You can zap only with a wand.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg[0] == '\0' )
    {
	if ( ch->fighting )
	{
	    victim = ch->fighting;
	}
	else
	{
	    send_to_char( "Zap whom or what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( !( victim = get_char_room ( ch, arg ) )
	    && !( obj  = get_obj_here  ( ch, arg ) ) )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    if ( ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )
	|| IS_AFFECTED( ch, AFF_CHARM ) )
    {
	act( "You try to zap $p, but you have no free will.",
	    ch, wand, NULL, TO_CHAR );
	act( "$n tries to zap $p, but has no free will.", 
	    ch, wand, NULL, TO_ROOM );
	return;
    }

    if ( ( sn = wand->value[3] ) < 0
	|| sn >= MAX_SKILL
	|| skill_table[sn].spell_fun == 0 )
    {
	bug( "Do_zap: bad sn %d.", sn );
	return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( wand->value[2] > 0 )
    {
	if ( victim )
	    if ( victim == ch )
	    {
		act( "You zap yourself with $p.", ch, wand, NULL,   TO_CHAR );
		act( "$n zaps $mself with $p.",   ch, wand, NULL,   TO_ROOM );
	    }
	    else
	    {
		act( "You zap $N with $p.",       ch, wand, victim, TO_CHAR );
		act( "$n zaps $N with $p.",       ch, wand, victim, TO_ROOM );
	    }
	else
	{
	    act( "You zap $P with $p.",           ch, wand, obj,    TO_CHAR );
	    act( "$n zaps $P with $p.",           ch, wand, obj,    TO_ROOM );
	}

	/* Wands skill by Binky for EnvyMud, modified by Thelonius */
	if ( !IS_NPC( ch ) 
	    && !( number_percent( ) < ch->pcdata->learned[gsn_wands] ) )
	{ 
	    switch ( number_bits( 3 ) )
	    {
	    case 0: 
	    case 1:                      
	    case 2:                      
	    case 3: 
		act( "You are unable to invoke the power of $p.",
		    ch, wand, NULL, TO_CHAR );
		act( "$n is unable to invoke the power of $p.",
		    ch, wand, NULL, TO_ROOM );
		return;                    
	    case 4:                
	    case 5:                      
	    case 6:                      
		act( "You summon the power of $p, but it fizzles away.",
		    ch, wand, NULL, TO_CHAR );
		act( "$n summons the power of $p, but it fizzles away.",
		    ch, wand, NULL, TO_ROOM );
		if ( --wand->value[2] <= 0 )
		{
		    act( "$p blazes bright and is gone.",
			ch, wand, NULL, TO_CHAR );
		    act( "$p blazes bright and is gone.",
			ch, wand, NULL, TO_ROOM );
		    extract_obj( wand );
		}
		return;
	    case 7:
		act( "You can't control the power of $p, and it explodes!",
		    ch, wand, NULL, TO_CHAR );
		act( "$p explodes into fragments!", 
		    ch, wand, NULL, TO_ROOM );
		/*
		 * damage( ) call after extract_obj in case the damage would
		 * have extracted ch.  This is okay because we merely mark
		 * obj->deleted; it still retains all values until list_update.
		 * Sloppy?  Okay, create another integer variable. ---Thelonius
		 */
		extract_obj( wand );
		damage( ch, ch, wand->level, gsn_wands, WEAR_NONE );
		return;
	    }
	}

	/* wand->value[0] is not used for wands */
	obj_cast_spell( wand->value[3], wand->level, ch, victim, obj );
    }

    if ( !IS_NPC( ch )
	|| ( IS_NPC( ch ) && is_affected( ch, gsn_charm_person ) ) )
	if ( --wand->value[2] <= 0 )
	{
	    act( "$p explodes into fragments.", ch, wand, NULL, TO_CHAR );
	    act( "$p explodes into fragments.", ch, wand, NULL, TO_ROOM );
	    extract_obj( wand );
	}

    return;
}



void do_steal( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    char       arg [ MAX_INPUT_LENGTH ];
    int        number;
    int        count;
    int        percent;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "That, you cannot do.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Steal what from whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "That's pointless.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    obj = NULL;

    WAIT_STATE( ch, skill_table[gsn_steal].beats );

    /* Modified chances for stealing by Morpheus */
    percent = ch->level; /* Base value */

    percent += number_range( -10, 10 ); /* Luck */

    percent -= victim->level; /* Character level vs victim's */

    if ( !IS_AWAKE( victim ) )
	percent += 25; /* Sleeping characters are easier */

    percent += ch->pcdata->learned[gsn_steal]; /* Character ability */

    if ( IS_AFFECTED( ch, AFF_SNEAK ) )
	percent += 5; /* Quiet characters steal better */

    if ( !can_see( ch, victim ) )
	percent += 10; /* Unseen characters steal better */

    if ( !str_prefix( arg1, "coins" ) || !str_cmp( arg1, "gold" ) )
	percent *= 1.2; /* Gold is fairly easy to steal */
    else
    {
	number = number_argument( arg1, arg );
	count  = 0;
	for ( obj = victim->carrying; obj; obj = obj->next_content )
	{
	    if ( obj->wear_loc == WEAR_NONE
		&& can_see_obj( ch, obj )
		&& is_name( arg, obj->name ) )
	    {
		if ( ++count == number )
		  break;
	    }
	}
	
	if ( !obj )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}

	if ( obj->wear_loc == WEAR_NONE )
	    /* Items in inventory are harder */
	    percent *= .8;
	else
	    percent *= .4;
    }

    if ( (         !IS_NPC( victim )
	  && ( (   !IS_SET( victim->act, PLR_KILLER )
		&& !IS_SET( victim->act, PLR_THIEF )
		&& !IS_SET( victim->act, PLR_REGISTER )
		&& victim->race != race_lookup( "Vampire" ) )
	      ||   ch->level - victim->level < 5 ) )
	|| percent > number_percent( ) )
    {
	/*
	 * Failure.
	 */
	send_to_char( "Oops.\n\r", ch );
	act( "$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT    );
	act( "$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
        switch(number_bits( 2 ))
        {
        case 0:
           sprintf( buf, "%s is a lousy thief!", ch->name );
           break;
        case 1:
           sprintf( buf, "%s couldn't rob %s way out of a paper bag!",
                    ch->name,(ch->sex == 2) ? "her" : "his");
           break;
        case 2:
            sprintf( buf,"%s tried to rob me!",ch->name );
            break;
        case 3:
            sprintf(buf,"Keep your hands out of there, %s!",ch->name);
            break;
        }
	do_shout( victim, buf );
	if ( IS_NPC( victim ) )
	{
	    multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	else
	{
	    if ( !licensed( ch ) )
	    {
		send_to_char(
		     "You are not licensed!  You lose 400 exps.\n\r", ch );
		gain_exp( ch, -400 );
		if ( registered( ch, victim )
		    && ch->level - victim->level < 6 )
		{
		    return;
		}
	    }

	    if (    ch->level - victim->level > 5
		|| !registered( ch, victim ) )
	    {
		log_string( buf );
		if ( !IS_SET( ch->act, PLR_THIEF ) )
		{
		    SET_BIT( ch->act, PLR_THIEF );
		    send_to_char( "*** You are now a THIEF!! ***\n\r", ch );
		    save_char_obj( ch );
		    sprintf( buf, "%s became a THIEF by stealing from %s",
			ch->name, victim->name );
		    wiznet( ch, WIZ_FLAGS, get_trust( ch ), buf );
		}
	    }
	}
	return;
    }

    if (   !str_prefix( arg1, "coins" )
	|| !str_cmp   ( arg1, "gold"  ) )
    {
	int amount;

	amount = victim->gold * number_range( 1, 10 ) / 100;
	if ( amount <= 0 )
	{
	    send_to_char( "You couldn't get any gold.\n\r", ch );
	    return;
	}

	ch->gold     += amount;
	victim->gold -= amount;
	sprintf( buf, "Bingo!  You got %d gold coins.\n\r", amount );
	send_to_char( buf, ch );
	return;
    }

    if ( !can_drop_obj( ch, obj )
	|| IS_SET( obj->extra_flags, ITEM_INVENTORY )
	|| obj->level > ch->level )
    {
	send_to_char( "You can't pry it away.\n\r", ch );
	return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	send_to_char( "You have your hands full.\n\r", ch );
	return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
	send_to_char( "You can't carry that much weight.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	send_to_char( "Very daring, and you got it!\n\r", ch );
	unequip_char( victim, obj );
    }

    obj_from_char( obj );
    obj_to_char( obj, ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}



/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *keeper;
    SHOP_DATA *pShop;
    char       buf [ MAX_STRING_LENGTH ];

    pShop = NULL;
    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
    {
	if ( IS_NPC( keeper ) && ( pShop = keeper->pIndexData->pShop )
	    && ( argument[0] == '\0' || is_name( argument, keeper->name ) ) )
	    break;
    }

    if ( !keeper || !pShop || ( keeper && IS_AFFECTED( keeper, AFF_CHARM ) ) )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return NULL;
    }

    /*
     * Undesirables.
     */
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_KILLER ) )
    {
	do_say( keeper, "Killers are not welcome!" );
	sprintf( buf, "%s the KILLER is over here!\n\r", ch->name );
	do_shout( keeper, buf );
	return NULL;
    }

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_THIEF ) )
    {
	do_say( keeper, "Thieves are not welcome!" );
	sprintf( buf, "%s the THIEF is over here!\n\r", ch->name );
	do_shout( keeper, buf );
	return NULL;
    }

    /*
     * Shop hours.
     */
    if ( time_info.hour < pShop->open_hour )
    {
	do_say( keeper, "Sorry, come back later." );
	return NULL;
    }
    
    if ( time_info.hour > pShop->close_hour )
    {
	do_say( keeper, "Sorry, come back tomorrow." );
	return NULL;
    }

    /*
     * Invisible or hidden people.
     */
    if ( !can_see( keeper, ch ) )
    {
	do_say( keeper, "I don't trade with folks I can't see." );
	return NULL;
    }

    return keeper;
}



int get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
    SHOP_DATA *pShop;
    int        cost;

    if ( !obj || !( pShop = keeper->pIndexData->pShop ) )
	return 0;

    if ( fBuy )
    {
	cost = obj->cost * pShop->profit_buy  / 100;
    }
    else
    {
	OBJ_DATA *obj2;
	int       itype;

	cost = 0;
	for ( itype = 0; itype < MAX_TRADE; itype++ )
	{
	    if ( obj->item_type == pShop->buy_type[itype] )
	    {
		cost = obj->cost * pShop->profit_sell / 100;
		break;
	    }
	}

	for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
	{
	    if ( obj->pIndexData == obj2->pIndexData )
		cost /= 2;
	}
    }

    if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
	cost = cost * obj->value[2] / obj->value[1];

    return cost;
}


/*
 * Buy [amount] <item>
 * by: Erwin S. Andreasen (4u2@aabc.dk)
 * Buy [amount] <item> [shopkeeper]
 * mythran port by:
 * Canth (canth@xs4all.nl)
 */

void do_buy( CHAR_DATA *ch, char *argument )
{
    char arg  [ MAX_INPUT_LENGTH ];
    char arg2 [ MAX_INPUT_LENGTH ];
    char arg3 [ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    one_argument( argument, arg3 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Buy what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_PET_SHOP ) )
    {
	CHAR_DATA       *pet;
	ROOM_INDEX_DATA *pRoomIndexNext;
	ROOM_INDEX_DATA *in_room;
	char             buf [ MAX_STRING_LENGTH ];

	if ( IS_NPC( ch ) )
	    return;

	pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
	if ( !pRoomIndexNext )
	{
	    bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
	    send_to_char( "Sorry, you can't buy that here.\n\r", ch );
	    return;
	}

	in_room     = ch->in_room;
	ch->in_room = pRoomIndexNext;
	pet         = get_char_room( ch, arg );
	ch->in_room = in_room;

	if ( !pet || !IS_SET( pet->act, ACT_PET ) )
	{
	    send_to_char( "Sorry, you can't buy that here.\n\r", ch );
	    return;
	}

	if ( IS_SET( ch->act, PLR_BOUGHT_PET ) )
	{
	    send_to_char( "You already bought one pet this level.\n\r", ch );
	    return;
	}

	if ( ch->gold < 10 * pet->level * pet->level )
	{
	    send_to_char( "You can't afford it.\n\r", ch );
	    return;
	}

	if ( ch->level < pet->level )
	{
	    send_to_char( "You're not ready for this pet.\n\r", ch );
	    return;
	}

	ch->gold -= 10 * pet->level * pet->level;
	pet       = create_mobile( pet->pIndexData );
	SET_BIT( ch->act,          PLR_BOUGHT_PET );
	SET_BIT( pet->act,         ACT_PET        );
	SET_BIT( pet->affected_by, AFF_CHARM      );

	one_argument( argument, arg );

	if ( arg[0] != '\0' )
	{
	    sprintf( buf, "%s %s", pet->name, arg );
	    free_string( pet->name );
	    pet->name = str_dup( buf );
	}

	sprintf( buf, "%sA neck tag says 'I belong to %s'.\n\r",
		pet->description, ch->name );
	free_string( pet->description );
	pet->description = str_dup( buf );

	char_to_room( pet, ch->in_room );
	add_follower( pet, ch );
	send_to_char( "Enjoy your pet.\n\r", ch );
	act( "$n bought $N as a pet.", ch, NULL, pet, TO_ROOM );
	return;
    }
    else
    {
	OBJ_DATA  *obj;
	CHAR_DATA *keeper;
	int        cost;
	int        item_count = 1;	/* buy only one by default */

	if ( is_number ( arg ) )
	{
	    item_count = atoi( arg );
	    strcpy( arg, arg2 );
	    strcpy( arg2, arg3 );
	}

	if ( !( keeper = find_keeper( ch, arg2 ) ) )
	    return;

	obj  = get_obj_carry( keeper, arg );
	cost = get_cost( keeper, obj, TRUE );

	if ( cost <= 0 || !can_see_obj( ch, obj ) )
	{
	    act( "$n tells you 'I don't sell that -- try 'list''.",
		keeper, NULL, ch, TO_VICT );
	    ch->reply = keeper;
	    return;
	}

	if ( !( item_count > 0 ) )
	{
	    send_to_char( "Buy how many? Number must be positive!\n\r", ch );
	    return;
	}

	if ( ch->gold < ( cost * item_count ) )
	{
	    if ( item_count == 1 )
	    {
		act( "$n tells you 'You can't afford to buy $p'.",
		    keeper, obj, ch, TO_VICT );
		ch->reply = keeper;
		return;
	    }
	    else
	    {
		char buf [ MAX_STRING_LENGTH ];	/* Temp buffer */
		if ( ( ch->gold / cost ) > 0 )
		    sprintf ( buf, "$n tells you 'You can only afford %d of those!",
			( ch->gold / cost ) );
		else
		    sprintf ( buf, "$n tells you 'You must be kidding? - you can't even afford one, let alone %d!",
			item_count );
		act( buf, keeper, obj, ch, TO_VICT );
		ch->reply = keeper;
		return;
	    }
	}

	if ( obj->level > ch->level )
	{
	    act( "$n tells you 'You can't use $p yet'.",
		keeper, obj, ch, TO_VICT );
	    ch->reply = keeper;
	    return;
	}

	if ( ch->carry_number + (item_count * get_obj_number( obj ) ) > can_carry_n( ch ) )
	{
	    send_to_char( "You can't carry that many items.\n\r", ch );
	    return;
	}

	if ( ch->carry_weight + ( item_count * get_obj_weight( obj ) ) > can_carry_w( ch ) )
	{
	    send_to_char( "You can't carry that much weight.\n\r", ch );
	    return;
	}

	if ( ( item_count > 1 ) && !IS_SET ( obj->extra_flags, ITEM_INVENTORY ) )
	{
	    act( "$n tells you 'Sorry - $p is something I have only one of'.",
		keeper, obj, ch, TO_CHAR );
	    ch->reply = keeper;
	    return;
	}

	if ( item_count == 1 )
	{
	    act( "You buy $p.", ch, obj, NULL, TO_CHAR );
	    act( "$n buys $p.", ch, obj, NULL, TO_ROOM );
	}
	else
	{
	    char buf [ MAX_STRING_LENGTH ];
	    sprintf ( buf, "$n buys %d * $p.", item_count );
	    act ( buf, ch, obj, NULL, TO_ROOM );
	    sprintf ( buf, "You buy %d * $p.", item_count );
	    act ( buf, ch, obj, NULL, TO_CHAR );
	}

	ch->gold -= cost * item_count;
	keeper->gold += cost * item_count;

	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    for ( ; item_count > 0; item_count-- )
	    {
		obj = create_object( obj->pIndexData, obj->level );
		obj_to_char( obj, ch );
	    }
	else
	{
	    obj_from_char( obj );
	    obj_to_char( obj, ch );
	}
	return;
    }
}



void do_list( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH   ];
    char buf1 [ MAX_STRING_LENGTH*4 ];

    buf1[0] = '\0';

    if ( IS_SET( ch->in_room->room_flags, ROOM_PET_SHOP ) )
    {
	CHAR_DATA       *pet;
	ROOM_INDEX_DATA *pRoomIndexNext;
	bool             found;

	pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
	if ( !pRoomIndexNext )
	{
	    bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	found = FALSE;
	for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
	{
	    if ( IS_SET( pet->act, ACT_PET ) )
	    {
		if ( !found )
		{
		    found = TRUE;
		    strcat( buf1, "Pets for sale:\n\r" );
		}
		sprintf( buf, "[%2d] %8d - %s\n\r",
			pet->level,
			10 * pet->level * pet->level,
			pet->short_descr );
		strcat( buf1, buf );
	    }
	}
	if ( !found )
	    send_to_char( "Sorry, we're out of pets right now.\n\r", ch );

	send_to_char( buf1, ch );
	return;
    }
    else
    {
	OBJ_DATA  *obj;
	CHAR_DATA *keeper;
	char       arg  [ MAX_INPUT_LENGTH ];
	char       arg2 [ MAX_INPUT_LENGTH ];
	int        cost;
	bool       found;

	argument = one_argument( argument, arg );
	one_argument( argument, arg2 );

	if ( !( keeper = find_keeper( ch, arg2 ) ) )
	    return;
	  
	found = FALSE;
	for ( obj = keeper->carrying; obj; obj = obj->next_content )
	{
	    if ( obj->wear_loc != WEAR_NONE
		|| ( cost = get_cost( keeper, obj, TRUE ) ) < 0 )
		continue;

	    if ( can_see_obj( ch, obj )
		&& (   arg[0] == '\0'
		    || !str_cmp( arg, "all" )
		    || is_name( arg, obj->name ) ) )
	    {
		if ( !found )
		{
		    found = TRUE;
		    strcat( buf1, "[Lv Price] Item\n\r" );
		}

		sprintf( buf, "[%2d %5d] %s.\n\r",
			obj->level, cost, capitalize( obj->short_descr ) );
		strcat( buf1, buf );
	    }
	}

	if ( !found )
	{
	    if ( arg[0] == '\0' )
		send_to_char( "You can't buy anything here.\n\r", ch );
	    else
		send_to_char( "You can't buy that here.\n\r", ch );
	    return;
	}

	send_to_char( buf1, ch );
	return;
    }
}



void do_sell( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *keeper;
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg  [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    int        cost;

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Sell what?\n\r", ch );
	return;
    }

    if ( !( keeper = find_keeper( ch, arg2 ) ) )
	return;

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	act( "$n tells you 'You don't have that item'.",
	    keeper, NULL, ch, TO_VICT );
	ch->reply = keeper;
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( !can_see_obj( keeper, obj ) )
    {
	act( "$n tells you 'I can't see that item'.",
	    keeper, NULL, ch, TO_VICT );
	ch->reply = keeper;
	return;
    }
	
    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0
	|| obj->level > LEVEL_HERO )
    {
	act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
	return;
    }

    if ( IS_SET( obj->extra_flags, ITEM_POISONED ) )
    {
	act( "$n tells you 'I won't buy that!  It's poisoned!'",
	    keeper, NULL, ch, TO_VICT );
	ch->reply = keeper;
	return;
    }

    sprintf( buf, "You sell $p for %d gold piece%s.",
	    cost, cost == 1 ? "" : "s" );
    act( buf, ch, obj, NULL, TO_CHAR );
    act( "$n sells $p.", ch, obj, NULL, TO_ROOM );
    ch->gold     += cost;

    if ( obj->item_type == ITEM_TRASH )
    {
	extract_obj( obj );
    }
    else
    {
	obj_from_char( obj );
	obj_to_char( obj, keeper );
    }

    return;
}



void do_value( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *keeper;
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg  [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    int        cost;

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Value what?\n\r", ch );
	return;
    }

    if ( !( keeper = find_keeper( ch, arg2 ) ) )
	return;

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	act( "$n tells you 'You don't have that item'.",
	    keeper, NULL, ch, TO_VICT );
	ch->reply = keeper;
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    if ( !can_see_obj( keeper, obj ) )
    {
	act( "$n tells you 'You are offering me an imaginary object!?!?'.",
	    keeper, NULL, ch, TO_VICT );
	ch->reply = keeper;
	return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
	act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
	return;
    }

    if ( IS_SET( obj->extra_flags, ITEM_POISONED ) )
    {
	act( "$n tells you 'I won't buy that!  It's poisoned!'",
	    keeper, NULL, ch, TO_VICT );
	ch->reply = keeper;
	return;
    }

    sprintf( buf, "$n tells you 'I'll give you %d gold coins for $p'.", cost );
    act( buf, keeper, obj, ch, TO_VICT );
    ch->reply = keeper;

    return;
}

/* Poison weapon by Thelonius for EnvyMud */
void do_poison_weapon( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *pobj;
    OBJ_DATA *wobj;
    char      arg [ MAX_INPUT_LENGTH ];

    /* Don't allow mobs or unskilled pcs to do this */
    if ( IS_NPC( ch )
    || (  !IS_NPC( ch )
	&& ch->level < skill_table[gsn_poison_weapon].skill_level[ch->class] ) )
    {                                          
	send_to_char( "What do you think you are, a thief?\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )                                              
    { send_to_char( "What are you trying to poison?\n\r",    ch ); return; }
    if ( ch->fighting )                                       
    { send_to_char( "While you're fighting?  Nice try.\n\r", ch ); return; }
    if ( !( obj = get_obj_carry( ch, arg ) ) )
    { send_to_char( "You do not have that weapon.\n\r",      ch ); return; }
    if ( obj->item_type != ITEM_WEAPON )
    { send_to_char( "That item is not a weapon.\n\r",        ch ); return; }
    if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )
    { send_to_char( "That weapon is already poisoned.\n\r",  ch ); return; }

    /* Now we have a valid weapon...check to see if we have the powder. */
    for ( pobj = ch->carrying; pobj; pobj = pobj->next_content )
    {
	if ( pobj->pIndexData->vnum == OBJ_VNUM_BLACK_POWDER )
	    break;
    }
    if ( !pobj )
    {
	send_to_char( "You do not have the black poison powder.\n\r", ch );
	return;
    }

    /* Okay, we have the powder...do we have water? */
    for ( wobj = ch->carrying; wobj; wobj = wobj->next_content )
    {
	if ( wobj->item_type == ITEM_DRINK_CON
	    && wobj->value[1]  >  0
	    && wobj->value[2]  == 0 )
	    break;
    }
    if ( !wobj )
    {
	send_to_char( "You have no water to mix with the powder.\n\r", ch );
	return;
    }

    /* Great, we have the ingredients...but is the thief smart enough? */
    if ( !IS_NPC( ch ) && get_curr_wis( ch ) < 19 )
    {
	send_to_char( "You can't quite remember what to do...\n\r", ch );
	return;
    }
    /* And does the thief have steady enough hands? */
    if ( !IS_NPC( ch )
	&& ( get_curr_dex( ch ) < 20
	    || ch->pcdata->condition[COND_DRUNK] > 0 ) )
    {
	send_to_char(
	"Your hands aren't steady enough to properly mix the poison.\n\r",
								ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_poison_weapon].beats );

    /* Check the skill percentage */
    if ( !IS_NPC( ch )
	&& number_percent( ) > ch->pcdata->learned[gsn_poison_weapon] )
    {
	send_to_char( "You failed and spill some on yourself.  Ouch!\n\r",
		     ch );
	damage( ch, ch, ch->level, gsn_poison_weapon, WEAR_NONE );
	act( "$n spills the poison all over!", ch, NULL, NULL, TO_ROOM );
	extract_obj( pobj );
	extract_obj( wobj );
	return;
    }

    /* Well, I'm tired of waiting.  Are you? */
    act( "You mix $p in $P, creating a deadly poison!",
	ch, pobj, wobj, TO_CHAR );
    act( "$n mixes $p in $P, creating a deadly poison!",
	ch, pobj, wobj, TO_ROOM );
    act( "You pour the poison over $p, which glistens wickedly!",
	ch, obj, NULL, TO_CHAR  );
    act( "$n pours the poison over $p, which glistens wickedly!",
	ch, obj, NULL, TO_ROOM  );
    SET_BIT( obj->extra_flags, ITEM_POISONED );
    obj->cost *= ch->level;

    /* Set an object timer.  Dont want proliferation of poisoned weapons */
    obj->timer = 10 + ch->level;

    if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
	obj->timer *= 2;

    if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
	obj->timer *= 2;

    /* WHAT?  All of that, just for that one bit?  How lame. ;) */
    act( "The remainder of the poison eats through $p.",
	ch, wobj, NULL, TO_CHAR );
    act( "The remainder of the poison eats through $p.",
	ch, wobj, NULL, TO_ROOM );
    extract_obj( pobj );
    extract_obj( wobj );

    return;
}

/* Contributed by BoneCrusher of EnvyMud. */
/* Modyfied for clan's by Maniac from Mythran mud */
void do_donate( CHAR_DATA *ch, char *arg )
{
    OBJ_DATA *	container = NULL;
    OBJ_DATA *	obj;
    OBJ_DATA *	obj_next;
    ROOM_INDEX_DATA *	room;
    int		pitvnum = 0;
    int		roomvnum = 0;
    char	arg1 [ MAX_INPUT_LENGTH  ];
    char	arg2 [ MAX_INPUT_LENGTH  ];
    char	buf  [ MAX_STRING_LENGTH ];

    arg = one_argument( arg, arg1 );
    arg = one_argument( arg, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Donate what?\n\r", ch );
	return;
    }

    if( !str_cmp( arg2, "clan" ) )
    {
	pitvnum = clan_table[ch->pcdata->clan].pit;
	roomvnum = clan_table[ch->pcdata->clan].pitroom;

	if( ( room = get_room_index (roomvnum) ) )
	{
	    for (container = room->contents ; container; container = container->next_content )
	    {
		if (container->pIndexData->vnum == pitvnum )
		{
		    break;
		}
	    }
	}
	else
	{
	    sprintf( buf, "Clan %s donation room doesn't exist", clan_name( ch->pcdata->clan ) );
	    log_string( buf );
	}
	if (!container)
	{
	    sprintf( buf, "No clan donation pit for %s", clan_name( ch->pcdata->clan ) );
	    log_string ( buf );
	}
    }
    if (!container)
    {
	if ( ( container = get_obj_world( ch, "donation" ) ) == NULL )
	{
		send_to_char( "The donation pit is missing from the world.\n\r", ch );
		log_string ("Can't find donation pit");
		return;
	}
    }

    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}
	
	if ( get_obj_weight( obj ) + get_obj_weight( container ) 
	  > container->value[0] )
	{
	    send_to_char( "It won't fit.\n\r", ch );
	    return;
	}

	if ( obj->item_type == ITEM_TRASH
	    || obj->item_type == ITEM_FOOD
	    || obj->item_type == ITEM_KEY
	    || obj->item_type == ITEM_PILL )
	{
	    act("You send $p flying to the $P.", ch, obj, container, TO_CHAR );
	    act("$n sends $p flying to the $P.", ch, obj, container, TO_ROOM );
	    extract_obj( obj );
	    return;
	}

	obj_from_char( obj );
	obj_to_obj( obj, container );
	act( "$n sends $p flying to the $P.", ch, obj, container, TO_ROOM );
	act( "You send $p flying to the $P.", ch, obj, container, TO_CHAR );
	send_to_room( "A loud clank is heard from the pit!",
		     container->in_room );
    }
    else
    {
	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
		&& can_see_obj( ch, obj )
		&& obj->wear_loc == WEAR_NONE
		&& obj != container
		&& can_drop_obj( ch, obj )
		&& get_obj_weight( obj ) + get_obj_weight( container )
		<= container->value[0] )
	    {

		if ( obj->item_type == ITEM_TRASH
		    || obj->item_type == ITEM_FOOD
		    || obj->item_type == ITEM_KEY
		    || obj->item_type == ITEM_PILL )
		{
		    act( "You send $p flying to the $P.", ch, obj, container,
			TO_CHAR );
		    act( "$n sends $p flying to the $P.", ch, obj, container,
			TO_ROOM );
		    extract_obj( obj );
		    continue;
		}

		obj_from_char( obj);
		obj_to_obj( obj, container );
		act( "$n sends $p flying to the $P.", ch, obj, container,
		    TO_ROOM );
		act( "You send $p flying to the $P.", ch, obj, container,
		    TO_CHAR );
		send_to_room( "A loud clank is heard from the pit!\n\r",
		     container->in_room );
	    }
	}
    }

    return;

}


void do_cdonate( CHAR_DATA *ch, char *argument )
{
    char *buf;

    if( !is_in_clan( ch ) )
    {
	send_to_char( "Huh?", ch );
	return;
    }

    buf = str_dup( argument );
    buf = strcat( buf, " clan" );
    do_donate( ch, buf );
    return;
}



void do_register( CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *mob;
    int        cost = ch->level * 1000;
    char       buf[MAX_STRING_LENGTH];

    if ( IS_NPC( ch ) )
	return;

    mob = NULL;
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
	if ( IS_NPC( mob ) && mob->pIndexData->vnum == MOB_VNUM_SECRETARY )
	    break;
    }

    if ( !mob )
    {
	send_to_char( "You may not register to kill here.\n\r", ch );
	return;
    }

    if ( ch->level < 16 )
    {
	do_say( mob, "You are too young to participate in pkilling." );
	return;
    }

    if ( ch->level >= LEVEL_HERO )
    {
	do_say( mob, "Heros and Immortals may not register to pkill" );
	return;
    }

    if ( ch->gold < cost )
    {
	sprintf( buf, "You do not have %d gold.", cost );
	do_say( mob, buf );
	return;
    }

    sprintf( buf, "Thank you for your %d gold.", cost );
    do_say( mob, buf );

    ch->gold -= cost;

    SET_BIT( ch->act, PLR_REGISTER );

    sprintf( buf, "%s just registered to PK at %s", ch->name, mob->short_descr );
    wiznet( ch, WIZ_FLAGS, get_trust( ch ), buf );

    return;

}

/* equips a character */
void do_outfit ( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    if (ch->level > 5 || IS_NPC(ch))
    {
        send_to_char("Find it yourself!\n\r",ch);
        return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) == NULL )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0 );
        obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_LIGHT );
    }

    if ( ( obj = get_eq_char( ch, WEAR_BODY ) ) == NULL )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0 );
        obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_BODY );
    }

    if ( ( obj = get_eq_char( ch, WEAR_SHIELD ) ) == NULL )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0 );
        obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_SHIELD );
    }

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
        obj = create_object( get_obj_index(class_table[ch->class].weapon),0);
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_WIELD );
    }

    send_to_char("You have been equipped by the Gods.\n\r",ch);
}

/*
 * Hide objects... by Maniac!
 */
void do_hide_obj(CHAR_DATA *ch, char *argument)
{
    char	arg[MAX_INPUT_LENGTH];
    int		chance;
    OBJ_DATA	*obj;

    one_argument(argument,arg);
    if ( arg[0] == '\0' )
    {
        send_to_char("What do you want to hide?\n\r", ch);
        return;
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }
    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

    obj_from_char( obj );
    obj_to_room( obj, ch->in_room );

    if (IS_SET(obj->extra_flags, ITEM_HIDDEN))	/* no use in hiding it again */
	return;

    chance = number_range(1, 5);
    if (ch->class == CLASS_RANGER)	/* Rangers are better */
	chance += 2;
    if (ch->class == CLASS_THIEF)	/* Thieves as wel, but not so good */
	chance++;
    if (chance > 5)			/* Let's not push it... */
	chance = 5;
    if (ch->level > LEVEL_HERO )	/* Immortals will manage */
	chance = 5;
    switch (chance)			/* Let's see what we've got */
    {
	case 1:
		act("$n is on all fours trying to hide $p.", ch, obj, NULL, TO_ROOM );
		act("You try to hide $p, but fail misarably.", ch, obj, NULL, TO_CHAR );
		break;
	case 2:
		act("$n is on all fours digging in the dirt.", ch, NULL, NULL, TO_ROOM );
		act("You hide $p, but you did it quite obvious.", ch, obj, NULL, TO_CHAR );
		SET_BIT(obj->extra_flags, ITEM_HIDDEN);
		break;
	case 3:
		act( "You fail to hide $p.", ch, obj, NULL, TO_CHAR );
		break;
	case 4:
	case 5:
		act( "You hide $p.", ch, obj, NULL, TO_CHAR );
		SET_BIT(obj->extra_flags, ITEM_HIDDEN);
		break;
    }
}

void do_search(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    char buf[MAX_INPUT_LENGTH];
    char buf2[MAX_INPUT_LENGTH];
    int found = FALSE;
    int	chance;

    buf[0] = '\0';
    buf2[0] = '\0';

    for ( obj = ch->in_room->contents;obj && (!found); obj = obj->next_content )
    {
        if ( IS_SET(obj->extra_flags, ITEM_HIDDEN) )
        {
		chance = number_range( 1, 5);
		if ((ch->class == CLASS_RANGER) || (ch->class == CLASS_THIEF))
			chance++;
		if (chance > 3)
		{
			sprintf(buf, "You find %s.\n\r", obj->short_descr);
			strcat(buf2, buf);
			buf[0] = '\0';
			REMOVE_BIT(obj->extra_flags, ITEM_HIDDEN);
			found = TRUE;
		}
        }
    }
    if (!found)
	strcat(buf2, "You find nothing.\n\t");
    send_to_char(buf2, ch);
    buf2[0] = '\0';
}


void do_lore(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument,arg);
    if ( arg[0] == '\0' )
    {
	send_to_char("What do you want to lore?\n\r", ch);
	return;
    }

    obj = get_obj_carry(ch,arg);
    if ( obj == NULL )
    {
	send_to_char("You aren't carrying that.\n\r", ch);
	return;
    }

    WAIT_STATE(ch, skill_table[gsn_lore].beats);

    if ( !IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[gsn_lore] )
    {
	act("You look at $p, but you can't find out any additional information.",
	    ch,obj,NULL,TO_CHAR);
	act("$n looks at $p but cannot find out anything.", ch, obj, NULL, TO_ROOM);
	return;
    }
    else
    {	
	act("$n studies $p, discovering all of it's hidden powers.",ch,obj,NULL,TO_ROOM);
	if ( ( number_percent( ) * number_percent( ) ) < 40 )
	{
	    obj->cost += ( (ch->pcdata->learned[gsn_lore]) * 0.001 * obj->cost);
	    send_to_char( "Your understanding of the lore behind it increases its worth!\n\r",  ch );
	}

	spell_identify(gsn_lore,( 4 * obj->level )/3,ch,obj); 
	/* check_improve(ch,gsn_lore,TRUE,4); */
    }
}	

/* put an item on auction, or see the stats on the current item or bet */
void do_auction (CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    argument = one_argument (argument, arg1);

	if (IS_NPC(ch)) /* NPC can be extracted at any time and thus can't auction! */
		return;

    if (arg1[0] == '\0')
        if (auction->item != NULL)
        {
            /* show item data here */
            if (auction->bet > 0)
                sprintf (buf, "Current bet on this item is %d gold.\n\r",auction->bet);
            else
                sprintf (buf, "No bets on this item have been received.\n\r");
            send_to_char (buf,ch);
            spell_identify (0, LEVEL_HERO - 1, ch, auction->item); /* uuuh! */
            return;
        }
        else
        {
            send_to_char ("Auction WHAT?\n\r",ch);
            return;
        }

    if (IS_IMMORTAL(ch) && !str_cmp(arg1,"stop"))
    if (auction->item == NULL)
    {
        send_to_char ("There is no auction going on you can stop.\n\r",ch);
        return;
    }
    else /* stop the auction */
    {
        sprintf (buf,"Sale of %s has been stopped by God. Item confiscated.",
                        auction->item->short_descr);
        talk_auction (buf);
        obj_to_char (auction->item, ch);
        auction->item = NULL;
        if (auction->buyer != NULL) /* return money to the buyer */
        {
            auction->buyer->gold += auction->bet;
            send_to_char ("Your money has been returned.\n\r",auction->buyer);
        }
        return;
    }

    if (!str_cmp(arg1,"bet") )
        if (auction->item != NULL)
        {
            int newbet;

            /* make - perhaps - a bet now */
            if (argument[0] == '\0')
            {
                send_to_char ("Bet how much?\n\r",ch);
                return;
            }

            newbet = parsebet (auction->bet, argument);
            printf ("Bet: %d\n\r",newbet);

            if ( ( newbet + 1 ) < ( auction->bet * 1.1 ) )
            {
                send_to_char ("You must at least bid 10% over the current bet.\n\r",ch);
                return;
            }

            if (newbet > ch->gold)
            {
                send_to_char ("You don't have that much money!\n\r",ch);
                return;
            }

            /* the actual bet is OK! */

            /* return the gold to the last buyer, if one exists */
            if (auction->buyer != NULL)
                auction->buyer->gold += auction->bet;

            ch->gold -= newbet; /* substract the gold - important :) */
            auction->buyer = ch;
            auction->bet   = newbet;
            auction->going = 0;
            auction->pulse = PULSE_AUCTION; /* start the auction over again */

            sprintf (buf,"A bet of %d gold has been received on %s.\n\r",newbet,auction->item->short_descr);
            talk_auction (buf);
            return;


        }
        else
        {
            send_to_char ("There isn't anything being auctioned right now.\n\r",ch);
            return;
        }

/* finally... */

    obj = get_obj_list (ch, arg1, ch->carrying); /* does char have the item ? */

    if (obj == NULL)
    {
        send_to_char ("You aren't carrying that.\n\r",ch);
        return;
    }

    if (auction->item == NULL)
    switch (obj->item_type)
    {

    default:
        act ("You cannot auction $Ts.",ch, NULL, item_type_name (obj), TO_CHAR);
        return;

    case ITEM_WEAPON:
    case ITEM_ARMOR:
    case ITEM_STAFF:
    case ITEM_WAND:
    case ITEM_SCROLL:
        obj_from_char (obj);
        auction->item = obj;
        auction->bet = 0;
        auction->buyer = NULL;
        auction->seller = ch;
        auction->pulse = PULSE_AUCTION;
        auction->going = 0;

        sprintf (buf, "A new item has been received: %s.", obj->short_descr);
        talk_auction (buf);

        return;

    } /* switch */
    else
    {
        act ("Try again later - $p is being auctioned right now!",ch,auction->item,NULL,TO_CHAR);
        return;
    }
}
