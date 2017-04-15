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
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <sys/stat.h>
#include "merc.h"



bool	can_use_cmd	args( ( int cmd, CHAR_DATA *ch, int level ) );


char *	const	where_name	[] =
{
    "<used as light>     ",
    "<worn on finger>    ",
    "<worn on finger>    ",
    "<worn around neck>  ",
    "<worn around neck>  ",
    "<worn on body>      ",
    "<worn on head>      ",
    "<worn in eyes>      ",
    "<worn on face>      ",
    "<orbiting>          ",
    "<orbiting>          ",
    "<worn on legs>      ",
    "<worn on feet>      ",
    "<worn on hands>     ",
    "<worn on arms>      ",
    "<worn as shield>    ",
    "<worn about body>   ",
    "<worn about waist>  ",
    "<worn around wrist> ",
    "<worn around wrist> ",
    "<wielded>           ",
    "<dual wielded>      ",
    "<held>              ",
    "<worn on ear>       ",
    "<worn on ear>       ",
    "<worn around ankle> ",
    "<worn around ankle> ",
	"<triple wielded>    "
};



/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				       bool fShort ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				       bool fShort, bool fShowNothing ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );

/* void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch )
); */

void show_char_to_char_1     args( ( CHAR_DATA *victim, CHAR_DATA *ch, char *argument ) );

void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
void    do_scry_exits           args( ( CHAR_DATA *ch, ROOM_INDEX_DATA  *scryer ) );


char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf [ MAX_STRING_LENGTH ];

    buf[0] = '\0';
    if ( IS_OBJ_STAT( obj, ITEM_INVIS)     )   strcat( buf, "(Invis) "     );
    if ( IS_AFFECTED( ch, AFF_DETECT_EVIL  )
	&& IS_OBJ_STAT( obj, ITEM_EVIL )   )   strcat( buf, "&r(Red Aura)&w "  );
    if ( IS_AFFECTED( ch, AFF_DETECT_MAGIC )
	&& IS_OBJ_STAT( obj, ITEM_MAGIC )  )   strcat( buf, "&Y(Magical)&w "   );
    if ( IS_OBJ_STAT( obj, ITEM_GLOW )     )   strcat( buf, "&W(Glowing)&B "   );
    if ( IS_OBJ_STAT( obj, ITEM_HUM )      )   strcat( buf, "(Humming) "   );
    if ( IS_OBJ_STAT( obj, ITEM_POISONED ) )   strcat( buf, "&G(Poisoned)&w "  );
    if ( IS_OBJ_STAT( obj, ITEM_FLAME )    )   strcat( buf, "&r(Burning)&w " );
    if ( IS_OBJ_STAT( obj, ITEM_CHAOS )    )   strcat( buf, "&Y(Chaotic)&w " );
    if ( IS_OBJ_STAT( obj, ITEM_ICY   )    )   strcat( buf, "&B(Frosty)&w " );
    if ( fShort )
    {
	if ( obj->short_descr )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( obj->description )
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
    OBJ_DATA  *obj;
    int		figchance = 0;
    char       buf [ MAX_STRING_LENGTH ];
    char     **prgpstrShow;
    char      *pstrShow;
    int       *prgnShow;
    int       *prgnType;
    int        nShow;
    int        iShow;
    int        count;
    bool       fCombine;

    if ( !ch->desc )
	return;

    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj; obj = obj->next_content )
    {
	if ( obj->deleted )
	    continue;
	count++;
    }

    prgpstrShow	= alloc_mem( count * sizeof( char * ) );
    prgnShow    = alloc_mem( count * sizeof( int )    );
    prgnType    = alloc_mem( count * sizeof( int )    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj; obj = obj->next_content )
    { 
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );
	    fCombine = FALSE;

	    if ( IS_NPC( ch ) || IS_SET( ch->act, PLR_COMBINE ) )
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
		prgnType    [nShow] = obj->item_type;
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
	if ( IS_NPC( ch ) || IS_SET( ch->act, PLR_COMBINE ) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		switch( prgnType[iShow] )
		{
		default:
		     sprintf( buf, "(%2d) ", prgnShow[iShow] );
		     send_to_char(AT_GREEN, buf, ch );
		     break;
		case ITEM_LIGHT:
		     sprintf( buf, "(%2d) ", prgnShow[iShow] );
		     send_to_char(AT_WHITE, buf, ch );
		     break;
		case ITEM_FOOD:
		case ITEM_BERRY:
		case ITEM_PILL:
		     sprintf( buf, "(%2d) ", prgnShow[iShow] );
		     send_to_char(AT_ORANGE, buf, ch );
		     break;
		case ITEM_FOUNTAIN:
		case ITEM_DRINK_CON:
		case ITEM_POTION:
		     sprintf( buf, "(%2d) ", prgnShow[iShow] );
		     send_to_char(AT_BLUE, buf, ch );
		     break;
		case ITEM_MONEY:
		     sprintf( buf, "(%2d) ", prgnShow[iShow] );
		     send_to_char(AT_YELLOW, buf, ch );
		     break;
		case ITEM_BLOOD:     
		     sprintf( buf, "(%2d) ", prgnShow[iShow] );
		     send_to_char(AT_RED, buf, ch );
		     break;
		}   
	    }
	    else
	    {
		send_to_char(C_DEFAULT, "     ", ch );
	    }
	}
		switch( prgnType[iShow] )
                {
		default:
	             send_to_char(AT_GREEN, prgpstrShow[iShow], ch );
	             send_to_char(C_DEFAULT, "\n\r", ch );
		     break;
		case ITEM_LIGHT:
	             send_to_char(AT_WHITE, prgpstrShow[iShow], ch );
	             send_to_char(C_DEFAULT, "\n\r", ch );
		     break;
		case ITEM_FOOD:
		case ITEM_PILL:
	             send_to_char(AT_ORANGE, prgpstrShow[iShow], ch );
	             send_to_char(C_DEFAULT, "\n\r", ch );
		     break;
		case ITEM_FOUNTAIN:
		case ITEM_DRINK_CON:
		case ITEM_POTION:
	             send_to_char(AT_BLUE, prgpstrShow[iShow], ch );
	             send_to_char(C_DEFAULT, "\n\r", ch );
		     break;
		case ITEM_MONEY:
	             send_to_char(AT_YELLOW, prgpstrShow[iShow], ch );
	             send_to_char(C_DEFAULT, "\n\r", ch );
		     break;
		case ITEM_BLOOD:     
	             send_to_char(AT_RED, prgpstrShow[iShow], ch );
	             send_to_char(C_DEFAULT, "\n\r", ch );
		     break;
		}   
	free_string( prgpstrShow[iShow] );
    }

   while( figchance != 3 )
   {
    if ( IS_AFFECTED2( ch, AFF_HALLUCINATING) && number_percent() > 55 )
    {
     OBJ_INDEX_DATA	*figment;
     char		figbuf[MAX_STRING_LENGTH];
     
     figment = rand_figment_obj( ch );

    if ( fShowNothing )
     sprintf( figbuf, "     %s\n\r", figment->short_descr );
    else
     sprintf( figbuf, "     %s\n\r", figment->description );

     send_to_char( AT_GREEN, figbuf, ch );
     nShow += 1;
    }
    figchance += 1;
   }


    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC( ch ) || IS_SET( ch->act, PLR_COMBINE ) )
	    send_to_char(C_DEFAULT, "     ", ch );
	send_to_char(AT_DGREEN, "Nothing.\n\r", ch );
    }

    /*
     * Clean up.
     */
    free_mem( prgpstrShow, count * sizeof( char * ) );
    free_mem( prgnShow,    count * sizeof( int )    );
    free_mem( prgnType,    count * sizeof( int )    );
    return;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf [ MAX_STRING_LENGTH ];
    char buf2 [MAX_STRING_LENGTH ];
    
    buf[0] = '\0';
    buf2[0] = '\0';
    if (!victim->desc && !IS_NPC(victim))
        strcat( buf, "(Link-dead) " );
    if ( !IS_NPC( victim ) )
      {
      if ( IS_SET( victim->act, PLR_AFK ) )
	 strcat( buf, "&Y<AFK>&P " );
      if ( IS_SET( victim->act, PLR_PKILLER ) )
	 strcat( buf, "&W[&RPK&W]&P " );

      if ( IS_SET( victim->act, PLR_WIZINVIS ) )
	 {
	 sprintf( buf2, "%s %d%s", "&w(Wizinvis", victim->wizinvis,
	 	  ")&P " );
	 strcat( buf, buf2 );
	 }
      if ( IS_SET( victim->act, PLR_CLOAKED ) )
         {
         sprintf( buf2, "%s %d%s", "&w(Cloaked", victim->cloaked,
                  ")&P " );
         strcat( buf, buf2 );
         }

      if ( IS_SET( victim->act, PLR_QUEST ) )
         strcat( buf, "<QUESTING> " );
      if ( IS_SET( victim->act, PLR_QUESTOR ) )
         strcat( buf, "<QUESTING> " );
      }
    if ( victim->desc && victim->desc->editor != 0 && get_trust(ch) > LEVEL_IMMORTAL)
      strcat( buf, "&R<&BEDITING&R>&P " );
    if ( IS_AFFECTED( victim, AFF_INVISIBLE )   )
                                                strcat( buf, "(Invis) "      );
    if ( IS_AFFECTED( victim, AFF_HIDE )        )
                                                strcat( buf, "(Hide) "       );
    if ( IS_AFFECTED( victim, AFF_CHARM ) 
    && ( !IS_SET( victim->act, UNDEAD_TYPE( victim ) ) ) )
                                                strcat( buf, "(Charmed) "    );
    if ( IS_AFFECTED( victim, AFF_PEACE )       )
                                                strcat( buf, "(Peaceful) "    );
    if ( IS_AFFECTED( victim, AFF_PASS_DOOR )   )
                                                strcat( buf, "(Translucent) ");
    if ( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
                                                strcat( buf, "(Pink Aura) "  );
    if ( is_affected( victim, gsn_drowfire ) ) 
	strcat( buf, "&p(Purple Aura)&X " );
    if ( IS_EVIL( victim ) && IS_AFFECTED( ch, AFF_DETECT_EVIL ) )
        strcat( buf, "&r(Red Aura)&X "   );
    if ( IS_GOOD( victim ) && IS_AFFECTED( ch, AFF_DETECT_GOOD ) )
        strcat( buf, "&B(Blue Aura)&X "  );
    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_KILLER )  )
						strcat( buf, "(KILLER) "     );
    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_OUTCAST ) )
                                                strcat( buf, "(OUTCAST) "    );
    if ( !IS_NPC( victim ) && IS_SET( victim->act, PLR_THIEF  )  )
						strcat( buf, "(THIEF) "      );
    if ( IS_NPC(victim ) && IS_SET( victim->act, UNDEAD_TYPE( victim ) ) )
                                                strcat( buf, "(Undead) "     );
    if (IS_NPC(victim) && ch->questmob && victim == ch->questmob )
       strcat( buf, "[TARGET] ");
    if ( victim->position == POS_STANDING && victim->long_descr[0] != '\0' )
    {
	strcat( buf, victim->long_descr );
	if ( !IS_NPC( victim ) )
	  if ( victim->pcdata->lname )
	    {
	      if ( victim->pcdata->lname[0] != '\0' ) 
	        strcat( buf, " " );
	      strcat( buf, victim->pcdata->lname );
	    }
	send_to_char(AT_PINK, buf, ch );
    }
    else
    {
      strcat( buf, PERS( victim, ch ) );
      if ( !IS_NPC( victim ) )
        if ( victim->pcdata->lname )
        {
          if ( victim->pcdata->lname[0] != '\0' )
          strcat( buf, " " );
          strcat( buf, victim->pcdata->lname );
        }
      if ( !IS_NPC( victim ) && !IS_SET( ch->act, PLR_BRIEF ) )
	strcat( buf, victim->pcdata->title );

      switch ( victim->position )
      {
	case POS_DEAD:     strcat( buf, " is DEAD!!"              ); break;
	case POS_MORTAL:   strcat( buf, " is mortally wounded."   ); break;
	case POS_INCAP:    strcat( buf, " is incapacitated."      ); break;
	case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
	case POS_SLEEPING: strcat( buf, " is sleeping here."      ); break;
	case POS_RESTING:  strcat( buf, " is resting here."       ); break;
	case POS_STANDING: strcat( buf, " is here."               ); break;
	case POS_FIGHTING:
	  strcat( buf, " is here, fighting " );
	  if ( !victim->fighting )
	    strcat( buf, "thin air??" );
	  else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	  else if ( victim->in_room == victim->fighting->in_room )
	  {
	    strcat( buf, PERS( victim->fighting, ch ) );
	    strcat( buf, "." );
	  }
	  else
	    strcat( buf, "someone who left??" );
	  break;
      }

      strcat( buf, "\n\r" );
      buf[0] = UPPER( buf[0] );
      send_to_char(AT_PINK, buf, ch );
    }
    buf2[0] = '\0';
    if ( IS_AFFECTED( victim, AFF_SANCTUARY ))
    {
      strcat( buf2, "    " );
      strcat( buf2, PERS( victim, ch ) );
      if ( !IS_NPC( victim ) )
        if ( victim->pcdata->lname )
          {
          if ( victim->pcdata->lname[0] != '\0' )
            strcat( buf2, " " );
            strcat( buf2, victim->pcdata->lname );
          }
      strcat( buf2, "&W's body is veiled in a glowing white mist.\n\r" );
      buf2[4] = UPPER( buf2[4] );
      send_to_char ( AT_WHITE, buf2, ch );
      buf2[0] = '\0';
    }
    if ( IS_AFFECTED2( victim, AFF_GOLDEN ))
    {
      strcat( buf2, "    " );
      strcat( buf2, PERS( victim, ch ) );
      if ( !IS_NPC( victim ) )
        if ( victim->pcdata->lname )
          {
          if ( victim->pcdata->lname[0] != '\0' )
            strcat( buf2, " " );
            strcat( buf2, victim->pcdata->lname );
          }
      strcat( buf2, "&Y's body radiates a crisp golden aura.\n\r" );
      buf2[4] = UPPER( buf2[4] );
      send_to_char( AT_YELLOW, buf2, ch );
      buf2[0] = '\0';
    }
    if ( IS_AFFECTED2( victim, AFF_FIELD ) )
    {
      strcat( buf2, "    " );
      strcat( buf2, PERS( victim, ch ) );
      if ( !IS_NPC( victim ) )
	if ( victim->pcdata->lname )
	  {
          if ( victim->pcdata->lname[0] != '\0' )
	    strcat( buf2, " " );
	    strcat( buf2, victim->pcdata->lname );
	  }
      strcat( buf2, "&z's body is enveloped in a black haze.\n\r" );
      buf2[4] = UPPER( buf2[4] );
      send_to_char( AT_DGREY, buf2, ch );
      buf2[0] = '\0';
    }
    if ( IS_AFFECTED( victim, AFF_FIRESHIELD ))
    {
      strcat( buf2, "    " );
      strcat( buf2, PERS( victim, ch ) );
      if ( !IS_NPC( victim ) )
        if ( victim->pcdata->lname )
          {
          if ( victim->pcdata->lname[0] != '\0' )
            strcat( buf2, " " );
            strcat( buf2, victim->pcdata->lname );
          }
      strcat( buf2, "&R's body is burning with unfelt heat.\n\r" );
      buf2[4] = UPPER( buf2[4] );
      send_to_char ( AT_RED, buf2, ch );
      buf2[0] = '\0';
    }
    if ( IS_AFFECTED( victim, AFF_SHOCKSHIELD ))
    {
      strcat( buf2, "    " );
      strcat( buf2, PERS( victim, ch ) );
      if ( !IS_NPC( victim ) )
        if ( victim->pcdata->lname )
          {
          if ( victim->pcdata->lname[0] != '\0' )
            strcat( buf2, " " );
            strcat( buf2, victim->pcdata->lname );
          }
      strcat( buf2, "&B's body is sparking with electricity.\n\r" );
      buf2[4] = UPPER( buf2[4] );
      send_to_char ( AT_BLUE, buf2, ch );
      buf2[0] = '\0';
    }
    if ( IS_AFFECTED( victim, AFF_ICESHIELD ))
    {
      strcat( buf2, "    " );
      strcat( buf2, PERS( victim, ch ) );
      if ( !IS_NPC( victim ) )
        if ( victim->pcdata->lname )
          {
          if ( victim->pcdata->lname[0] != '\0' )
            strcat( buf2, " " );
            strcat( buf2, victim->pcdata->lname );
          }
      strcat( buf2, "&C's body is covered in frost and ice.\n\r" );
      buf2[4] = UPPER( buf2[4] );
      send_to_char ( AT_LBLUE, buf2, ch );
      buf2[0] = '\0';
    }
    if ( IS_AFFECTED( victim, AFF_CHAOS ))
    {
      strcat( buf2, "    " );
      strcat( buf2, PERS( victim, ch ) );
      if ( !IS_NPC( victim ) )
        if ( victim->pcdata->lname )
          {
          if ( victim->pcdata->lname[0] != '\0' )
            strcat( buf2, " " );
            strcat( buf2, victim->pcdata->lname );
          }
      strcat( buf2, "&Y's body shimmers randomly with raw chaos.\n\r" );
      buf2[4] = UPPER( buf2[4] );
      send_to_char ( AT_YELLOW, buf2, ch );
      buf2[0] = '\0';
    }
    if ( IS_AFFECTED( victim, AFF_VIBRATING ))
    {
      strcat( buf2, "    " );
      strcat( buf2, PERS( victim, ch ) );
      if ( !IS_NPC( victim ) )
        if ( victim->pcdata->lname )
          {
          if ( victim->pcdata->lname[0] != '\0' )
            strcat( buf2, " " );
            strcat( buf2, victim->pcdata->lname );
          }
      strcat( buf2, "&w's body is vibrating rapidly.\n\r" );
      buf2[4] = UPPER( buf2[4] );
      send_to_char ( C_DEFAULT, buf2, ch );
      buf2[0] = '\0';
    }
    if ( IS_AFFECTED2( victim, AFF_BLADE ) )
    {
      strcat( buf2, "    " );
      strcat( buf2, PERS( victim, ch ) );
      if ( !IS_NPC( victim ) )
        if ( victim->pcdata->lname )
          {
          if ( victim->pcdata->lname[0] != '\0' )
            strcat( buf2, " " );
            strcat( buf2, victim->pcdata->lname );
          }
      strcat( buf2, "&w is surrounded by thousands of spinning blades.\n\r" );
      buf2[4] = UPPER( buf2[4] );
      send_to_char( AT_GREY, buf2, ch );
      buf2[0] = '\0';
    }
    if ( IS_AFFECTED2( victim, AFF_DANCING ) )
    {
      strcat( buf2, "&w    " );
      strcat( buf2, PERS( victim, ch ) );
      if ( !IS_NPC( victim ) )
        if ( victim->pcdata->lname )
          {
          if ( victim->pcdata->lname[0] != '\0' )
            strcat( buf2, "&w " );
            strcat( buf2, victim->pcdata->lname );
          }
      strcat( buf2, " &.is s&.urrounde&.d by &.thou&.sand&.s of &.danci&.ng l&.igh&.ts&w.\n\r" );
      buf2[4] = UPPER( buf2[4] );
      send_to_char( AT_GREY, buf2, ch );
      buf2[0] = '\0';
    }
    return;
}


void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch, char *argument ) 
{
    OBJ_DATA *obj;
    char      buf  [ MAX_STRING_LENGTH ];
    int       iWear;
    int       percent;
    bool      found;

    if ( ( can_see( victim, ch ) ) && ( argument[0] == '\0' ) )  
    {
	act(AT_GREY, "$n looks at you.", ch, NULL, victim, TO_VICT    );
	act(AT_GREY, "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
    }

/* Put the check here for argument = storage.. display their storage 
   and then return w/o showing char stuff   --Angi */

    if ( ( !str_cmp( argument, "storage" ) ) && ( get_trust( ch ) > L_DIR ) )
    {
      if ( IS_NPC( victim ) )
      {
         send_to_char( AT_WHITE, "NPC's do not have items in storage.\n\r", ch );
      }
      else
      {
	 sprintf( buf, "%s's storage box contains:\n\r", victim->name );  
         send_to_char( AT_WHITE, buf, ch );
   	 show_list_to_char( victim->pcdata->storage, ch, TRUE, TRUE ); 
      }
      return;
    }

    if ( ( argument[0] != '\0' ) && ( get_trust( ch ) > L_DIR ) )
      if ( !( obj = get_obj_here( victim, argument ) ) )
        {    
	  sprintf( buf, "%s is not carrying that item.\n\r", victim->name );
          send_to_char(AT_WHITE, buf, ch );
          return;    
	}
        else
        {
          switch ( obj->item_type )
          {
            default:
            send_to_char(AT_DGREEN, "That is not a container.\n\r", ch );
            break;

            case ITEM_DRINK_CON:
            if ( obj->value[1] <= 0 )
            {
                send_to_char(AT_BLUE, "It is empty.\n\r", ch );
                oprog_look_in_trigger( obj, ch );
                break;
            }

            sprintf( buf, "%s %s has is %s full of a %s liquid.\n\r",
	 	capitalize(obj->short_descr), victim->name,
                obj->value[1] <     obj->value[0] / 4
                    ? "less than" :
                obj->value[1] < 3 * obj->value[0] / 4
                    ? "about"     : "more than",
                liq_table[obj->value[2]].liq_color
                );

            send_to_char(AT_BLUE, buf, ch );  
            oprog_look_in_trigger( obj, ch );
            break;

        case ITEM_CONTAINER:
        case ITEM_CORPSE_NPC:
        case ITEM_CORPSE_PC:
/*            if ( IS_SET( obj->value[1], CONT_CLOSED ) )   
  	    {
                send_to_char(AT_GREEN, "It is closed.\n\r", ch );
                break;
            } <so doesn't matter if its closed, look anyways> */

	    sprintf( buf, "%s %s has contains:\n\r",
 		capitalize(obj->short_descr), victim->name );
	    send_to_char( AT_WHITE, buf, ch );
/*            act(AT_WHITE, "$p contains:", ch, obj, NULL, TO_CHAR ); */
            show_list_to_char( obj->contains, ch, TRUE, TRUE );
            oprog_look_in_trigger( obj, ch );
            break; 
        }
      return;
      }

    if ( victim->description[0] != '\0' )
    {
	send_to_char(AT_GREEN, victim->description, ch );
    }
    else
    {
	act(AT_GREY, "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    if ( MAX_HIT(victim) > 0 )
	percent = ( 100 * victim->hit ) / MAX_HIT(victim);
    else
	percent = -1;

    strcpy( buf, PERS( victim, ch ) );

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

    buf[0] = UPPER( buf[0] );

         if ( percent >= 100 ) send_to_char( AT_WHITE, buf, ch );
    else if ( percent >=  90 ) send_to_char( AT_WHITE, buf, ch );
    else if ( percent >=  80 ) send_to_char( AT_WHITE, buf, ch );
    else if ( percent >=  70 ) send_to_char( AT_BLUE,  buf, ch );
    else if ( percent >=  60 ) send_to_char( AT_BLUE,  buf, ch );
    else if ( percent >=  50 ) send_to_char( AT_BLUE,  buf, ch );
    else if ( percent >=  40 ) send_to_char( AT_RED,   buf, ch );
    else if ( percent >=  30 ) send_to_char( AT_RED,   buf, ch );
    else if ( percent >=  20 ) send_to_char( AT_RED,   buf, ch );
    else if ( percent >=  10 ) send_to_char( AT_BLOOD, buf, ch );
    else                       send_to_char( (AT_BLOOD+AT_BLINK),  buf, ch );

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) )
	    && can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char(AT_GREY, "\n\r", ch );
		act(AT_WHITE, "$N is using:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    send_to_char(AT_BLUE, where_name[iWear], ch );
	    send_to_char(AT_CYAN, format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char(AT_CYAN, "\n\r", ch );
	}
	else if ( !IS_NPC( victim )
	     && is_class( victim, CLASS_WEREWOLF )
	     && ( ( iWear == WEAR_WIELD && !get_eq_char( victim, iWear ) )
	     || ( iWear == WEAR_WIELD_2 && !get_eq_char( victim, iWear )
             && !get_eq_char( victim, WEAR_HOLD )
	     && !get_eq_char( victim, WEAR_SHIELD )
	     && victim->pcdata->learned[gsn_dualclaw] > 0 ) ) )
		{
		send_to_char(AT_BLUE, where_name[iWear], ch );
		sprintf( buf, "%s%s%s&ca Werewolf Claw",
		is_affected( victim, gsn_flamehand ) ? "&r(Burning) " : "",
		is_affected( victim, gsn_chaoshand ) ? "&Y(Chaotic) " : "",
		is_affected( victim, gsn_frosthand ) ? "&B(Frosty) " : "" );
		send_to_char(AT_CYAN, buf, ch );
		send_to_char(AT_CYAN, "\n\r", ch );
		found = TRUE;
		}
	else if ( !IS_NPC( victim )
	     && is_class( victim, CLASS_MONK )
	     && !is_class( victim, CLASS_WEREWOLF ) 
	     && ( ( iWear == WEAR_WIELD && !get_eq_char( victim, iWear ) )
	     || ( iWear == WEAR_WIELD_2 && !get_eq_char( victim, iWear )
		&& !get_eq_char( victim, WEAR_HOLD )
		&& !get_eq_char( victim, WEAR_SHIELD ) 
		&& victim->pcdata->learned[gsn_blackbelt] > 0 ) ) )
		{
		send_to_char(AT_BLUE, where_name[iWear], ch );
		sprintf( buf, "%s%s%s&c%s's Fist", 
		is_affected( victim, gsn_flamehand ) ? "&r(Burning) " : "",
		is_affected( victim, gsn_chaoshand ) ? "&Y(Chaotic) " : "",
		is_affected( victim, gsn_frosthand ) ? "&B(Frosty) " : "",
		victim->name );
		send_to_char(AT_CYAN, buf, ch );
		send_to_char(AT_CYAN, "\n\r", ch );
		found = TRUE;
		}
    }

    if ( victim != ch
	&& !IS_NPC( ch ) 
	&& ( number_percent( ) < ch->pcdata->learned[gsn_peek]  || ( ch->race == RACE_HALFLING ) ) )
    {
       if ( ( victim->level > L_CON ) && ( !IS_IMMORTAL( ch ) ) )
        send_to_char( AT_WHITE, "\n\rYou cannot peek into an Immortal's inventory.\n\r", ch );
      else
      {
	send_to_char(AT_WHITE, "\n\rYou peek at the inventory:\n\r", ch );
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
	update_skpell( ch, gsn_peek );
      } 
    }

    return;
}



void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;
    CHAR_DATA	*rch2;
    int		figchance = 0;

    for ( rch = list; rch; rch = rch->next_in_room )
    {
        if ( rch->deleted || rch == ch )
	    continue;
	
	if ( !(rch->desc) 
         && !IS_NPC(rch)
         && get_trust( ch ) < L_APP )
	    continue;

	if ( !IS_NPC( rch )
	    && IS_SET( rch->act, PLR_WIZINVIS )
	    && get_trust( ch ) < rch->wizinvis )
	    continue;

	if ( can_see( ch, rch ) )
	{

         if ( IS_AFFECTED2( ch, AFF_HALLUCINATING)  && number_percent() > 55 )
         {
           rch2 = rch;
           rch = rand_figment( ch );
	    show_char_to_char_0( rch, ch );
          rch = rch2;
         }
         else
	    show_char_to_char_0( rch, ch );

	}
	else if ( room_is_dark( ch->in_room )
		 && IS_AFFECTED( rch, AFF_INFRARED ) )
	{
	    send_to_char(AT_RED, "You see glowing red eyes watching YOU!\n\r", ch );
	}
    }

   while( figchance != 5 )
   {
    if ( IS_AFFECTED2( ch, AFF_HALLUCINATING) && number_percent() > 55 )
    {
     MOB_INDEX_DATA	*figment;
     char		figbuf[MAX_STRING_LENGTH];
     
     figment = rand_figment_mob( ch );

     sprintf( figbuf, "%s", figment->long_descr );
     send_to_char( AT_PINK, figbuf, ch );
    }
    figchance += 1;
   }
    return;
} 


bool check_blind( CHAR_DATA *ch )
{
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )
	return TRUE;

    if ( IS_AFFECTED( ch, AFF_BLIND ) && ch->race != RACE_ILLITHID )
    {
	send_to_char(AT_WHITE, "You can't see a thing!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}



void do_look( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    char       buf  [ MAX_STRING_LENGTH ];
    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       arg2 [ MAX_INPUT_LENGTH  ];
    char      *pdesc;
    ROOM_INDEX_DATA *portroom;
    int        door;
    extern OBJ_DATA *auc_obj;
#ifdef NEW_MONEY
    extern MONEY_DATA auc_cost;
#else
    extern int auc_cost;
#endif
    if ( !IS_NPC( ch ) && !ch->desc ) 
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char(AT_CYAN, "You can't see anything but stars!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char(AT_CYAN, "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }
    if ( is_raffected( ch->in_room, gsn_globedark )
    && !IS_SET( ch->act, PLR_HOLYLIGHT )
    && ch->race != RACE_DROW )
        {
	send_to_char(AT_DGREY, "It's completely and utterly black!\n\r", ch );
	return;
	}
    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC( ch )
	&& !IS_SET( ch->act, PLR_HOLYLIGHT )
	&& room_is_dark( ch->in_room ) 
        && ( ch->race != RACE_ELF   )
        && ( ch->race != RACE_DWARF )
        && ( ch->race != RACE_DROW  )
        && ( ch->race != RACE_DEMON ) 
	&& ( ch->race != RACE_TABAXI ) )
    {
	send_to_char(AT_DGREY, "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
	send_to_char(AT_WHITE, ch->in_room->name, ch );
	send_to_char(AT_WHITE, "\n\r", ch );

        /* This is where the exits thing used to be (Ne was here) 
	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOEXIT ) )
	    do_exits( ch, "auto" );
        */  

	if ( arg1[0] == '\0'
	    || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_BRIEF ) ) )
/* Thalador room color change */
	    send_to_char(ch->in_room->area->def_color, ch->in_room->description, ch );   

	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOEXIT ) )
	    do_exits( ch, "auto" );

	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->people,   ch );
	return;
    }

    if ( !str_prefix( arg1, "in" ) )
    {
	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char(AT_DGREEN, "Look in what?\n\r", ch );
	    return;
	}

	if ( !( obj = get_obj_here( ch, arg2 ) ) )
	{
	    if ( !str_prefix( arg2, "auction" ) )
	    {
	      int objcount = 1;
	      char buf[MAX_INPUT_LENGTH];

	      if ( !auc_obj )
	      {
		send_to_char(C_DEFAULT, "There is no object being auctioned.\n\r",ch);
		return;
	      }
	      obj_to_char( auc_obj, ch );
	      for ( obj = ch->carrying; obj; obj = obj->next )
	      {
		if ( obj == auc_obj )
		  break;
		objcount++;
	      }
	      sprintf(buf, "in %d.%s", objcount, auc_obj->name );
	      do_look(ch, buf );
	      obj_from_char(auc_obj);
	      return;
	    }
	      
	    send_to_char(AT_DGREEN, "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char(AT_DGREEN, "That is not a container.\n\r", ch );
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char(AT_BLUE, "It is empty.\n\r", ch );
		oprog_look_in_trigger( obj, ch );
		break;
	    }

	    sprintf( buf, "It's %s full of a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about"     : "more than",
		liq_table[obj->value[2]].liq_color
		);

	    send_to_char(AT_BLUE, buf, ch );
	    oprog_look_in_trigger( obj, ch );
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET( obj->value[1], CONT_CLOSED ) )
	    {
		send_to_char(AT_GREEN, "It is closed.\n\r", ch );
		break;
	    }

	    act(AT_WHITE, "$p contains:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    oprog_look_in_trigger( obj, ch );
	    break;
	case ITEM_PORTAL:
	    if ( !( portroom = get_room_index( obj->value[0] ) ) )
	     { 
	      act(AT_WHITE, "You cannot see anything through $p", ch, obj, NULL, TO_CHAR );
	      break;
	     }
	act(AT_GREEN, "You look into $p and see...", ch, obj, NULL, TO_CHAR );
	act(AT_GREEN, "$n looks into $p.", ch, obj, NULL, TO_ROOM);
	send_to_char(AT_WHITE, portroom->name, ch );
	send_to_char(AT_WHITE, "\n\r", ch );

	if ( arg1[0] == '\0'
	    || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_BRIEF ) ) )
	    send_to_char(AT_CYAN, portroom->description, ch );
	do_scry_exits( ch, portroom );
	show_list_to_char( portroom->contents, ch, FALSE, FALSE );
	show_char_to_char( portroom->people,   ch );
	    oprog_look_in_trigger( obj, ch );
	break;
	     
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) )
    {

/*	show_char_to_char_1( victim, ch );  Changed arguments so you
        can look into char's storage (look victim storage) --Angi */

        show_char_to_char_1( victim, ch, arg2 ); 
	return;
    }

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( ch, arg1, obj->extra_descr );
	    if ( pdesc )
	    {
		send_to_char(AT_BLUE, pdesc, ch );
		oprog_look_trigger( obj, ch );
		return;
	    }

	    pdesc = get_extra_descr( ch, arg1, obj->pIndexData->extra_descr );
	    if ( pdesc )
	    {
		send_to_char(AT_BLUE, pdesc, ch );
		oprog_look_trigger( obj, ch );
		return;
	    }
	}

	if ( is_name(ch, arg1, obj->name ) )
	{
	    send_to_char(AT_GREEN, obj->description, ch );
	    send_to_char(AT_GREEN, "\n\r", ch );
	    oprog_look_trigger( obj, ch );
	    return;
	}
    }

    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( ch, arg1, obj->extra_descr );
	    if ( pdesc )
	    {
		send_to_char(AT_GREEN, pdesc, ch );
		oprog_look_trigger( obj, ch );
		return;
	    }

	    pdesc = get_extra_descr( ch, arg1, obj->pIndexData->extra_descr );
	    if ( pdesc )
	    {
		send_to_char(AT_GREEN, pdesc, ch );
		oprog_look_trigger( obj, ch );
		return;
	    }
	}

	if ( is_name(ch, arg1, obj->name ) )
	{
	    send_to_char(AT_GREEN, obj->description, ch );
	    send_to_char(AT_GREEN, "\n\r", ch );
	    oprog_look_trigger( obj, ch );
	    return;
	}
    }

    pdesc = get_extra_descr( ch, arg1, ch->in_room->extra_descr );
    if ( pdesc )
    {
	send_to_char(AT_WHITE, pdesc, ch );
	return;
    }

         if ( !str_prefix( arg1, "north" ) ) door = 0;
    else if ( !str_prefix( arg1, "east"  ) ) door = 1;
    else if ( !str_prefix( arg1, "south" ) ) door = 2;
    else if ( !str_prefix( arg1, "west"  ) ) door = 3;
    else if ( !str_prefix( arg1, "up"    ) ) door = 4;
    else if ( !str_prefix( arg1, "down"  ) ) door = 5;
    else if ( !str_prefix( arg1, "auction" ) )
    {
      char buf[MAX_STRING_LENGTH];

      if ( !auc_obj )
      {
        send_to_char( C_DEFAULT, "There is no object being auctioned.\n\r",ch);
        return;
      }
      sprintf( buf, "Object: %s\n\r", auc_obj->short_descr );
      send_to_char( AT_WHITE, buf, ch );
      sprintf( buf, "Type: %s   Level: %d\n\r",
               item_type_name( auc_obj ), auc_obj->level );
      send_to_char( AT_WHITE, buf, ch );
#ifdef NEW_MONEY      
      sprintf( buf, "Value: %s  Price: %s\n\r", money_string( &auc_obj->cost ),
		money_string( &auc_cost ) );
#else
      sprintf( buf, "Value: %d   Price: %d\n\r", auc_obj->cost, auc_cost );
#endif
      send_to_char( AT_WHITE, buf, ch );
      return;
    }
    else if ( !str_prefix( arg1, "arena" ) && !IS_ARENA(ch) )
    {
      char buf[MAX_STRING_LENGTH];
      
      if ( !arena.cch && !(arena.fch || arena.sch) )
      {
        send_to_char( C_DEFAULT, "There is no challenge being offered.\n\r",
                      ch );
        return;
      }
      if ( arena.cch )
      {
        sprintf(buf, "Challenger: %s, a level %d %s.\n\r",
                arena.cch->name, arena.cch->level,
                class_long( arena.cch ) );
	sprintf(buf+strlen(buf), "CHALLENGING\n\r" );
	if ( arena.och )
        	sprintf(buf+strlen(buf), 
		"Challenged: %s, a level %d %s.\n\r",
		arena.och->name, arena.och->level,
		class_long( arena.och ) );
	else
		sprintf(buf+strlen(buf), "Challenged: ANYONE\n\r" );
        sprintf(buf+strlen(buf), "Award is %d coins.\n\r", arena.award);
      }
      else
      {
        int fp, sp;
        
        fp = (arena.fch->hit*100)/MAX_HIT(arena.fch);
        sp = (arena.sch->hit*100)/MAX_HIT(arena.sch);
        sprintf(buf, "Challenger: %s, a level %d %s.\n\r",
                arena.fch->name, arena.fch->level,
                class_long( arena.fch ) );
	sprintf(buf+strlen(buf), "FIGHTING\n\r" );
       	sprintf(buf+strlen(buf), "Challenged: %s, a level %d %s.\n\r",
		arena.sch->name, arena.sch->level,
		class_long( arena.sch ) );
	sprintf(buf+strlen(buf), "Award is %d coins.\n\r", arena.award );
        send_to_char(AT_WHITE, buf, ch);
        if ( fp > sp )
          sprintf(buf, "%s appears to be winning at the moment.\n\r",
                  arena.fch->name);
        else if ( sp > fp )
          sprintf(buf, "%s appears to be winning at the moment.\n\r",
                  arena.sch->name);
        else
          strcpy(buf, "They appear to be evenly matched at the moment.\n\r");
      }
      send_to_char(AT_WHITE, buf, ch);
      return;
    }
    else
    {
      send_to_char(AT_GREY, "You do not see that here.\n\r", ch );
      return;
    }

    /* 'look direction' */
    if ( !( pexit = ch->in_room->exit[door] ) || !pexit->to_room )
    {
	send_to_char(AT_GREY, "Nothing special there.\n\r", ch );
	return;
    }

    if ( pexit->description && pexit->description[0] != '\0' )
	send_to_char(AT_GREY, pexit->description, ch );
    else
	send_to_char(AT_GREY, "Nothing special there.\n\r", ch );
    if ( ( IS_AFFECTED( ch, AFF_SCRY ) ) /*&& ( pexit->to_room->description[0] != '\0' )*/ )
    {
        ROOM_INDEX_DATA *rid;
	if ( IS_SET( ch->in_room->room_flags, ROOM_NO_MAGIC ) 
	   || IS_SET( pexit->to_room->room_flags, ROOM_NOSCRY ) )
	  {
	   send_to_char( AT_BLUE, "You failed.\n\r", ch );
	   return;
	  }
	act( AT_BLUE, "You scry to the $T.", ch, NULL, dir_name[door], TO_CHAR );
/*	send_to_char(AT_WHITE, pexit->to_room->name, ch );
	send_to_char(AT_WHITE, "\n\r", ch );

	if ( arg1[0] == '\0'
	    || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_BRIEF ) ) )
	    send_to_char(AT_YELLOW, pexit->to_room->description, ch );
	do_scry_exits( ch, pexit->to_room );
	show_list_to_char( pexit->to_room->contents, ch, FALSE, FALSE );
	show_char_to_char( pexit->to_room->people,   ch );*/
	rid = ch->in_room;
	ch->in_room = pexit->to_room;
	do_look( ch, "" );
	ch->in_room = rid;
	eprog_scry_trigger( pexit, ch->in_room, ch );
	return;
    }
    if (   pexit->keyword
	&& pexit->keyword[0] != '\0'
	&& pexit->keyword[0] != ' ' )
    {
      if ( IS_SET( pexit->exit_info, EX_BASHED ) )
	act(AT_GREY, "The $d has been bashed from its hinges.",
	    ch, NULL, pexit->keyword, TO_CHAR );
      else if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
	act(AT_GREY, "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
      else if ( IS_SET( pexit->exit_info, EX_ISDOOR ) )
	act(AT_GREY, "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
    }
    eprog_look_trigger( pexit, ch->in_room, ch );
    return;
}



void do_examine( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char      buf [ MAX_STRING_LENGTH ];
    char      arg [ MAX_INPUT_LENGTH  ];
    char      msg [ MAX_INPUT_LENGTH  ];
    int brk;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_BLUE, "Examine what?\n\r", ch );
	return;
    }

    do_look( ch, arg );

    if ( ( obj = get_obj_here( ch, arg ) ) )
    {
	switch ( obj->item_type )
	{
	default:
	    break;

	case ITEM_WEAPON:
	case ITEM_ARMOR:
#ifdef NEW_MONEY

	    if ( (obj->pIndexData->cost.gold > 0)   ||
		 (obj->pIndexData->cost.silver > 0) ||
		 (obj->pIndexData->cost.copper > 0) )
	      brk = ( ( ( obj->cost.gold*C_PER_G + obj->cost.silver*S_PER_G +
		          obj->cost.copper ) * 100 ) /
		      ( obj->pIndexData->cost.gold*C_PER_G +
		        obj->pIndexData->cost.silver*S_PER_G +
			obj->pIndexData->cost.copper ) );
/*
	    if ( (obj->pIndexData->cost.gold + (obj->pIndexData->cost.silver/SILVER_PER_GOLD) +
		 (obj->pIndexData->cost.copper/COPPER_PER_GOLD) ) > 0 )
		brk = ( (obj->cost.gold + (obj->cost.silver/SILVER_PER_GOLD) +
		        (obj->cost.copper/COPPER_PER_GOLD) ) * 100 ) / ( obj->pIndexData->cost.gold +
			(obj->pIndexData->cost.silver/SILVER_PER_GOLD) +
			(obj->pIndexData->cost.copper/COPPER_PER_GOLD) );
*/
#else
	    if ( obj->pIndexData->cost > 0 )
	      brk = (obj->cost * 100) / obj->pIndexData->cost;
#endif
	    else
	      brk = 101;
	         if ( brk ==  0 ) strcpy( msg, "is utterly destroyed!" );
	    else if ( brk <= 10 ) strcpy( msg, "is almost useless." );
	    else if ( brk <= 20 ) strcpy( msg, "should be replaced soon." );
	    else if ( brk <= 30 ) strcpy( msg, "is in pretty bad shape." );
	    else if ( brk <= 40 ) strcpy( msg, "has seen better days." );
	    else if ( brk <= 50 ) strcpy( msg, "could use some repairs." );
	    else if ( brk <= 60 ) strcpy( msg, "is in average condition." );
	    else if ( brk <= 70 ) strcpy( msg, "has the odd dent." );
	    else if ( brk <= 80 ) strcpy( msg, "needs a bit of polishing." );
	    else if ( brk <= 90 ) strcpy( msg, "looks almost new." );
	    else if ( brk <=100 ) strcpy( msg, "is in perfect condition." );
	    else                  strcpy( msg, "looks almost indestructable!");
	    act(AT_WHITE,"Looking closer, you see that $p $T",ch,obj,msg,
		TO_CHAR);
	    break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	case ITEM_PORTAL:
	    send_to_char(AT_WHITE, "When you look inside, you see:\n\r", ch );
	    sprintf( buf, "in %s", arg );
	    do_look( ch, buf );
	}
    }
    
    return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_scry_exits( CHAR_DATA *ch, ROOM_INDEX_DATA  *scryer )
{
           EXIT_DATA       *pexit;
    extern char *    const  dir_name [ ];
           char             buf      [ MAX_STRING_LENGTH ];
           int              door;
           bool             found;
           bool             fAuto;
    
    fAuto = TRUE;
    strcpy( buf, "&z[&RExits&w:" );
    found = FALSE;
    for ( door = 0; door <= 5; door++ )
    {
	if ( ( pexit = scryer->exit[door] )
	    && pexit->to_room
	    && !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		strcat( buf, "&W " );
		strcat( buf, dir_name[door] );
	    }
        }
    }
    if ( !found )
	strcat( buf, "&WNone" );

    if ( fAuto )
	strcat( buf, "&z]\n\r" );

    send_to_char(AT_WHITE, buf, ch );
    return;
}

void do_exits( CHAR_DATA *ch, char *argument )
{
           EXIT_DATA       *pexit;
    extern char *    const  dir_name [ ];
           char             buf      [ MAX_STRING_LENGTH ];
           int              door;
           bool             found;
           bool             fAuto;

    buf[0] = '\0';
    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
	return;

    strcpy( buf, fAuto ? "&z[&RExits&w:" : "&cObvious exits&w:\n\r" );

    found = FALSE;
    for ( door = 0; door <= 5; door++ )
    {
	if ( ( pexit = ch->in_room->exit[door] )
	    && pexit->to_room
	    && !IS_SET( pexit->exit_info, EX_CLOSED ) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		strcat( buf, "&W " );
		strcat( buf, dir_name[door] );
	    }
	    else
	    {
		sprintf( buf + strlen( buf ), "&W%-5s&w - &W%s\n\r",
		    capitalize( dir_name[door] ),
		    room_is_dark( pexit->to_room )
			?  "&zToo dark to tell"
			: pexit->to_room->name
		    );
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? "&W none" : "&WNone.\n\r" );

    if ( fAuto )
	strcat( buf, "&z]\n\r" );

    send_to_char(AT_WHITE, buf, ch );
    return;
}



void do_score( CHAR_DATA *ch, char *argument )
{
    char         buf  [ MAX_STRING_LENGTH ];
    char         buf1 [ MAX_STRING_LENGTH ];

    buf1[0] = '\0';
    if (IS_NPC(ch))
      return;
    if ( !str_cmp( argument, "msg" ) && get_trust( ch ) >= LEVEL_IMMORTAL )
    {
	send_to_char( AT_WHITE, "Your personalized messages are as follows:\n\r", ch );
        sprintf( buf, "Bamfin&r: %s &w%s.\n\r", ch->name, ch->pcdata->bamfin );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Bamfout&r: %s &w%s.\n\r", ch->name, ch->pcdata->bamfout );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Bamfusee&r: You &w%s.\n\r", ch->pcdata->bamfusee );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Transto&r: Victim &w%s.\n\r", ch->pcdata->transto );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Transfrom&r: Victim &w%s.\n\r", ch->pcdata->transfrom );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Transvict&r: %s &w%s.\n\r", ch->name, ch->pcdata->transvict );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Slayusee&r: You &w%s.\n\r", ch->pcdata->slayusee );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Slayroom&r: %s &w%s.\n\r", ch->name, ch->pcdata->slayroom );
        send_to_char( AT_RED, buf, ch );
        sprintf( buf, "Slayvict&r: %s &w%s.\n\r", ch->name, ch->pcdata->slayvict );
        send_to_char( AT_RED, buf, ch );
	return;
    }
	/* booger */
	send_to_char( AT_WHITE, "==================================================================\n\r",
		ch );
    sprintf( buf,
	    "You are &W%s%s.\n\r",
	    ch->name,
	    IS_NPC( ch ) ? "" : ch->pcdata->title );
    send_to_char( AT_CYAN, buf, ch );
    if ( !IS_NPC(ch) )
    {
      if ( ch->pcdata->empowerments && *ch->pcdata->empowerments )
        sprintf( buf, "You have been empowered with %s.\n\r",
                 ch->pcdata->empowerments );
      if ( ch->pcdata->detractments && *ch->pcdata->detractments )
        sprintf( buf, "You have been detracted from %s.\n\r",
                 ch->pcdata->detractments );
    }
    sprintf( buf,
            "Level &W%d&c, %d years old (&W%d hours&c).\n\r",
	    ch->level,
	    get_age( ch ),
	    (get_age( ch ) - 17) * 4 );
    send_to_char( AT_CYAN, buf, ch );
	send_to_char( AT_WHITE, "==================================================================\n\r",
		ch );
    sprintf( buf, "&cYou are a &W%s&c &r%s&c.\n\r",
             (get_race_data(ch->race))->race_full, class_long( ch ) );
    send_to_char( AT_CYAN, buf, ch );
    if ( ch->clan )
    {
        CLAN_DATA *clan;
        
        clan = get_clan_index( ch->clan );
        sprintf( buf, "You belong to the clan %s.\n\r", clan->name );
	if ( ch->ctimer )
	  sprintf( buf + strlen(buf), "Clan skill timer: %d.\n\r", ch->ctimer );
        send_to_char( AT_WHITE, buf, ch );
        if ( ch->cquestpnts > 0 )
        {
          sprintf( buf, "You currently have %d Clan Quest Points.\n\r", ch->cquestpnts );
          send_to_char( AT_WHITE, buf, ch );
        }
    }
    
    sprintf ( buf, " %d/%d", ch->hit, MAX_HIT(ch) );
    send_to_char( AT_WHITE, buf, ch );
    send_to_char( AT_CYAN, " HP, ", ch );
    if ( !is_class( ch, CLASS_VAMPIRE ) )
       {
         sprintf ( buf, "%d/%d", ch->mana, MAX_MANA(ch) );
         send_to_char( AT_BLUE, buf, ch );
         send_to_char( AT_CYAN, " mana, ", ch );
       }
    else
       {
         sprintf ( buf, "%d/%d", ch->bp, MAX_BP(ch) );
         send_to_char( AT_RED, buf, ch );
         send_to_char( AT_CYAN, " blood, ", ch );
       }
    sprintf ( buf, "%d/%d", ch->move, MAX_MOVE(ch));
    send_to_char( AT_GREEN, buf, ch );
    send_to_char( AT_CYAN, " movement, ", ch );
    sprintf ( buf, "%d", ch->practice );
    send_to_char( AT_WHITE, buf, ch );
    send_to_char( AT_CYAN, " practices.\n\r", ch );

    sprintf( buf,
	    "&BYou are carrying &W%d/%d &Bitems with weight &W%d/%d &Bkg.\n\r",
	    ch->carry_number, can_carry_n( ch ),
	    ch->carry_weight, can_carry_w( ch ) );
    send_to_char( AT_BLUE, buf, ch );
	send_to_char( AT_WHITE, "==================================================================\n\r",
		ch );
	send_to_char( AT_WHITE, "&rAttributes:\n\r", ch );
    sprintf( buf,
	"&BStr: &W%d&p(&r%d&p)&B  Int: &W%d&p(&r%d&p)&B  Wis: &W%d&p(&r%d&p)\n\r",
	IS_NPC(ch) ? 13: ch->pcdata->perm_str, IS_NPC(ch) ? 13: get_curr_str( ch ),
	IS_NPC(ch) ? 13: ch->pcdata->perm_int, IS_NPC(ch) ? 13: get_curr_int( ch ),
	IS_NPC(ch) ? 13: ch->pcdata->perm_wis, IS_NPC(ch) ? 13: get_curr_wis( ch ));
    send_to_char( AT_PINK, buf, ch );
    sprintf( buf,
    "&BDex: &W%d&p(&r%d&p)&B  Con: &W%d&p(&r%d&p)&B  Cha: &W%d\n\r",
	IS_NPC(ch) ? 13: ch->pcdata->perm_dex, IS_NPC(ch) ? 13: get_curr_dex( ch ),
	IS_NPC(ch) ? 13: ch->pcdata->perm_con, IS_NPC(ch) ? 13: get_curr_con( ch ),
	IS_NPC(ch) ? 20: ch->charisma );
    send_to_char( AT_PINK, buf, ch );
    send_to_char( AT_CYAN, "You have scored ", ch );
#ifdef NEW_MONEY
    sprintf( buf, "&W%d &cexperience points.\n\r", ch->exp );
    send_to_char( AT_WHITE, buf, ch );
    sprintf( buf, "&cYou have accumulated: &W%d &Ygold, &W%d &wsilver, &cand &W%d &Ocopper &ccoins.\n\r",
	    ch->money.gold, ch->money.silver, ch->money.copper );
    send_to_char( AT_WHITE, buf, ch );
#else
    sprintf( buf, "%d ", ch->exp );
    send_to_char( AT_WHITE, buf, ch );
    send_to_char( AT_CYAN, "exp, and have accumulated ", ch );
    sprintf( buf, "%d ", ch->gold );
    send_to_char( AT_YELLOW, buf, ch );
    send_to_char( AT_CYAN, "gold coins.\n\r", ch );
#endif
    sprintf( buf,
    "Autoexit: %s.  Autoloot: %s.  Autosac: %s.  Autocoins: %s. Autosplit: %s.\n\r",
	    ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOEXIT ) ) ? "&Ryes&c"
	                                                         : "&Bno&c",
	    ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOLOOT ) ) ? "&Ryes&c"
	                                                         : "&Bno&c",
	    ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOSAC  ) ) ? "&Ryes&c"
	                                                         : "&Bno&c",
	    ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOCOINS ) ) ? "&Ryes&c"
	                                                          : "&Bno&c",
	    ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOSPLIT ) ) ? "&Ryes&c"
	                                                          : "&Bno&c" );
    send_to_char( AT_CYAN, buf, ch );    
    sprintf( buf, "Wimpy set to %d hit points.\n\r", ch->wimpy );
    send_to_char( AT_CYAN, buf, ch );
    if ( !IS_NPC( ch ) )
    {
	sprintf( buf, "Page pausing set to %d lines of text.\n\r",
		ch->pcdata->pagelen );
	send_to_char( AT_CYAN, buf, ch );
    }

    if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_DRUNK]   > 10 )
	send_to_char( AT_GREY, "You are drunk.\n\r", ch );
    if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_THIRST] ==  0
	&& ch->level >= LEVEL_IMMORTAL )
	send_to_char( AT_BLUE, "You are dying of thirst.\n\r", ch );
    if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL]   ==  0
	&& ch->level >= LEVEL_IMMORTAL )
	send_to_char( AT_ORANGE, "You are starving.\n\r", ch  );

    switch ( ch->position )
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
	sprintf( buf, "AC: %d.  ", GET_AC( ch ) );
	send_to_char( AT_CYAN, buf, ch );
    }

    send_to_char( AT_GREEN, "You are ", ch );
         if ( GET_AC( ch ) >=  101 ) send_to_char( AT_GREEN, "WORSE than naked!\n\r", ch );
    else if ( GET_AC( ch ) >=   20 ) send_to_char( AT_GREEN, 
	"naked.\n\r"           , ch );
    else if ( GET_AC( ch ) >=    0 ) send_to_char( AT_GREEN, "wearing "
	"clothes.\n\r" , ch );
    else if ( GET_AC( ch ) >= - 50 ) send_to_char( AT_GREEN, "slightly "
	"armored.\n\r", ch );
    else if ( GET_AC( ch ) >= -100 ) send_to_char( AT_GREEN, "somewhat "
	"armored.\n\r", ch );
    else if ( GET_AC( ch ) >= -250 ) send_to_char( AT_GREEN, 
	"armored.\n\r"         , ch );
    else if ( GET_AC( ch ) >= -500 ) send_to_char( AT_GREEN, "well "
	"armored.\n\r"    , ch );
    else if ( GET_AC( ch ) >= -750 ) send_to_char( AT_GREEN, "strongly "
	"armored.\n\r", ch );
    else if ( GET_AC( ch ) >= -1000 ) send_to_char( AT_GREEN, "heavily "
	"armored.\n\r" , ch );
    else if ( GET_AC( ch ) >= -1200 ) send_to_char( AT_GREEN, "superbly "
	"armored.\n\r", ch );
    else if ( GET_AC( ch ) >= -1400 ) send_to_char( AT_GREEN, "divinely "
	"armored.\n\r", ch );
    else                           send_to_char( AT_GREEN, "invincible!\n\r", ch );

    if ( ch->level >= 12 )
    {
	sprintf( buf, "Hitroll: " );
	send_to_char(AT_BLOOD, buf, ch );
	sprintf( buf, "%d", GET_HITROLL( ch ) );
	send_to_char(AT_RED, buf, ch);
	sprintf( buf, "  Damroll: " );
	send_to_char( AT_BLOOD, buf, ch );
	sprintf( buf, "%d.\n\r", GET_DAMROLL( ch ) );
	send_to_char( AT_RED, buf, ch );
    }
    
    if ( ch->level >=50)
    {
       sprintf( buf, "Saving-Throw: &R%d&X.\n\r", 
       ( ch->race != RACE_DWARF ) ? ch->saving_throw
		  : ch->saving_throw + ch->saving_throw * 25 / 100 );
       send_to_char( AT_CYAN, buf, ch );
    }
    
    if ( ch->level >= 8 )
    {
	sprintf( buf, "Alignment: %d.  ", ch->alignment );
	send_to_char( AT_CYAN, buf, ch );
    }

    send_to_char( AT_CYAN, "You are ", ch );
         if ( ch->alignment >  900 ) send_to_char( AT_BLUE, "angelic.\n\r",ch );
    else if ( ch->alignment >  700 ) send_to_char( AT_BLUE, "saintly.\n\r",ch );
    else if ( ch->alignment >  350 ) send_to_char( AT_BLUE, "good.\n\r"   ,ch );
    else if ( ch->alignment >  100 ) send_to_char( AT_BLUE, "kind.\n\r"   ,ch );
    else if ( ch->alignment > -100 ) send_to_char( AT_YELLOW, "neutral.\n\r",ch );
    else if ( ch->alignment > -350 ) send_to_char( AT_RED, "mean.\n\r"    ,ch);
    else if ( ch->alignment > -700 ) send_to_char( AT_RED, "evil.\n\r"    ,ch);
    else if ( ch->alignment > -900 ) send_to_char( AT_RED, "demonic.\n\r" ,ch);
    else                             send_to_char( AT_RED, "satanic.\n\r" ,ch);
  
    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
    {
      sprintf( buf, "WizInvis level: %d   WizInvis is %s\n\r",
                      ch->wizinvis,
                      IS_SET( ch->act, PLR_WIZINVIS ) ? "ON" : "OFF" );
      send_to_char( AT_YELLOW, buf, ch );
      sprintf( buf, "Cloaked level: %d   Cloaked is %s\n\r",
                      ch->cloaked,
                      IS_SET( ch->act, PLR_CLOAKED ) ? "ON" : "OFF" );
      send_to_char( AT_YELLOW, buf, ch );
    }
    if ( get_trust( ch ) != ch->level )
    {
	sprintf( buf, "You have been granted the powers of a level &R%d&W.\n\r",
		get_trust( ch ) );
        send_to_char( AT_WHITE, buf, ch );
    }
    
    if (!IS_NPC( ch ))
    sprintf( buf, "&RAfk Message: &r%s &W%s.\n\r", ch->name, ch->pcdata->afkchar );
    send_to_char( AT_WHITE, buf, ch );
/*    if ( ch->affected )
    {
	for ( paf = ch->affected; paf; paf = paf->next )
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

    if ( ch->affected2 )
    {
	for ( paf = ch->affected2; paf; paf = paf->next )
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
    }*/


    return;
}

void do_affectedby( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf;
    char         buf  [ MAX_STRING_LENGTH ];
    char         buf1 [ MAX_STRING_LENGTH ];
    bool printed = FALSE;

    buf1[0] = '\0';

  if ( IS_NPC( ch ) )
    return;
    if ( !ch->affected && !ch->affected2 ) 
      { send_to_char( AT_CYAN, "You are not affected by anything\n\r", ch); } 
    if ( ch->affected )
    {

	for ( paf = ch->affected; paf; paf = paf->next )
	{
		    if ( paf->deleted )
	        continue;

	    if ( !printed )
	    {
		send_to_char( AT_CYAN, "You are affected by:\n\r", ch );
		printed = TRUE;
	    }

	    sprintf( buf, "&BSpell&W: '&G%s&W'", skill_table[paf->type].name );
            send_to_char( AT_WHITE, buf, ch );
	    if ( ch->level >= 20 )
	    {
		sprintf( buf,
			" &Wmodifies &G%s&W by %d for %d hours",
			affect_loc_name( paf->location ),
			paf->modifier,
			paf->duration );
		send_to_char(AT_WHITE, buf, ch );
	    }

	    send_to_char( AT_WHITE, ".\n\r", ch );
	}
    }
    if ( ch->affected2 )
    {
	for ( paf = ch->affected2; paf; paf = paf->next )
	{
		    if ( paf->deleted )
		        continue;

	    if ( !printed )
	    {
		send_to_char( AT_CYAN, "You are affected by:\n\r", ch );
		printed = TRUE;
	    }

	    sprintf( buf, "&BSpell&W: '&G%s&W'", skill_table[paf->type].name );
            send_to_char( AT_WHITE, buf, ch );
	    if ( ch->level >= 20 )
	    {
		sprintf( buf,
			" &Wmodifies &G%s&W by %d for %d hours",
			affect_loc_name( paf->location ),
			paf->modifier,
			paf->duration );
		send_to_char(AT_WHITE, buf, ch );
	    }

	    send_to_char( AT_WHITE, ".\n\r", ch );
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
           char   buf           [ MAX_STRING_LENGTH ];
    extern char   str_boot_time[];
    extern time_t exe_comp_time;
    extern char * exe_file;
           char  *suf;
           int    day;
    struct stat   statis;

    day     = time_info.day + 1;
         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
	    "It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r",
	    ( time_info.hour % 12 == 0 ) ? 12 : time_info.hour % 12,
	    time_info.hour >= 12 ? "pm" : "am",
	    day_name[day % 7],
	    day, suf,
	    month_name[time_info.month] );
    send_to_char(AT_YELLOW, buf, ch );
    sprintf( buf,
	    "The storm awoke at %s\rThe system time is %s\r",
	    str_boot_time,
	    (char *) ctime( &current_time ));
    send_to_char(AT_RED, buf, ch );
if IS_IMMORTAL( ch )
{

    sprintf( buf, "Running copy compiled at %s\r", (char *) ctime(&exe_comp_time) );
    send_to_char(AT_RED, buf, ch );

    if ( !stat( exe_file, &statis ) )
    {
      sprintf( buf, "Storm compiled at %s\r", (char*)ctime(&statis.st_mtime ) );
      send_to_char( AT_RED, buf, ch );
    }
    else
    {
      if IS_IMMORTAL( ch )
       send_to_char( AT_RED, "No executable avable!!\r\n", ch );
    }
}
    return;
}



void do_weather( CHAR_DATA *ch, char *argument )
{
           char         buf     [ MAX_STRING_LENGTH ];
    static char * const sky_look[ 4 ] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if ( !IS_OUTSIDE( ch ) )
    {
	send_to_char(AT_BLUE, "You can't see the weather indoors.\n\r", ch );
	return;
    }

    sprintf( buf, "The sky is %s and %s.\n\r",
	    sky_look[weather_info.sky],
	    weather_info.change >= 0
	    ? "a warm southerly breeze blows"
	    : "a cold northern gust blows" );
    send_to_char(AT_BLUE, buf, ch );
    return;
}



void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;

    if ( argument[0] == '\0' )
	argument = "summary";

    for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
    {
	if ( pHelp->level > get_trust( ch ) )
	    continue;

	if ( is_name(ch, argument, pHelp->keyword ) )
	{
	    if ( pHelp->level >= 0 && str_cmp( argument, "imotd" ) 
	       && str_prefix( "advm_", argument ) 
	       && str_prefix( "demm_", argument ) )
	    {
		send_to_char(AT_GREY, pHelp->keyword, ch );
		send_to_char(AT_GREY, "\n\r", ch );
	    }

	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if ( pHelp->text[0] == '.' )
		send_to_char(AT_GREY, pHelp->text+1, ch );
	    else
		send_to_char(AT_GREY, pHelp->text  , ch );
	    return;
	}
    }

    send_to_char(AT_WHITE, "No help on that word.\n\r", ch );
    return;
}



/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char             arg1[ MAX_STRING_LENGTH ];
    char             buf      [ MAX_STRING_LENGTH*3 ];
    char             buf2     [ MAX_STRING_LENGTH   ];
    CLAN_DATA *      tClan;
    int              iClass;
    int              iLevelLower;
    int              iLevelUpper;
    int              nNumber;
    int              nMatch;
    bool             rgfClass [ MAX_CLASS ];
    bool             fClassRestrict;
    bool 	     fClanRestrict;
/*    bool	     fGuildRestrict; */
    bool             fImmortalOnly;
    bool             fHeroOnly;
    bool	     fRaceRestrict;
    bool	     fNameRestrict;
    bool         lng = FALSE;
    int		     iRace;
    int	   	     iClan;
    bool	     rgfClan [ MAX_CLAN ];
/*    int	 	     iGuild; 
    bool	     rgfGuild [ MAX_CLAN ];  */
    bool	     rgfRace [ MAX_RACE ];
    int              num_of_imm = 0;
    int              noclass[MAX_CLASS];
    int			remorts = 0;
	int			incrs;
 
    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = L_IMP; /*Used to be Max_level */
    fClassRestrict = FALSE;
    fImmortalOnly  = FALSE;
    fHeroOnly      = FALSE;
    fRaceRestrict  = FALSE;
    fNameRestrict  = FALSE;
    fClanRestrict  = FALSE;
/*    fGuildRestrict = FALSE;  */
    tClan          = NULL;

send_to_char(C_DEFAULT ,
"\n\r&W==================================================================\n\r",ch );
send_to_char(C_DEFAULT,"&W===                  &wEntropy &zof the &wSoul                       ===\n\r",ch);
send_to_char(C_DEFAULT,"&W==================================================================\n\r \n\r",ch);

    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
        noclass[iClass] = 0;
    for ( iClan = 0; iClan < MAX_CLAN; iClan++ )
        rgfClan[iClan] = FALSE;
	    for ( iRace = 0; iRace < MAX_RACE; iRace++ )
        rgfRace[iRace] = FALSE;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	rgfClass[iClass] = FALSE;

    /*
     * Parse arguments.
     */
    nNumber = 0;
    for ( ;; )
    {
	char arg [ MAX_STRING_LENGTH ];

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	    break;
	strcpy( arg1, arg );

	if ( is_number( arg ) )
	{
	    switch ( ++nNumber )
	    {
	    case 1: iLevelLower = atoi( arg ); break;
	    case 2: iLevelUpper = atoi( arg ); break;
	    default:
		send_to_char(AT_DGREEN, "Only two level numbers allowed.\n\r", ch );
		return;
	    }
	}
	else
	{
	    int iClass;

	    if ( strlen( arg ) < 3 )
	    {
		send_to_char(AT_DGREEN, "Classes must be longer than that.\n\r", ch );
		return;
	    }
	    
	    if ( strlen( arg ) > 3 )
	      lng = TRUE;

	    /*
	     * Look for classes to turn on.
	     */
	    arg[3]    = '\0';

	    if ( !str_cmp( arg, "imm" ) )
	    {
		fImmortalOnly = TRUE;
	    }
	    else
            if ( !str_cmp( arg, "chm" ) )
            {
               fHeroOnly = TRUE;
            }
            else
	    {
		fClassRestrict = TRUE;
		for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		{
		    if ( ( !str_cmp( arg, class_table[iClass].who_name ) 
			&& !lng )
		    || ( !str_cmp( arg1, class_table[iClass].who_long ) 
			&& lng ) )
		    {
			rgfClass[iClass] = TRUE;
			break;
		    }
		}

                if ( iClass == MAX_CLASS )
                {
                fClassRestrict = FALSE;
                fRaceRestrict = TRUE;
                for ( iRace = 0; iRace < MAX_RACE; iRace++ )
                {
                    if ( ( !str_cmp( arg, (get_race_data(iRace))->race_name ) && !lng )
                      || ( !str_cmp( arg1, (get_race_data(iRace))->race_full ) ) )
                    {
                      rgfRace[iRace] = TRUE;
                      break;
                    }
                }
		
		if ( iRace == MAX_RACE )
		{
		fRaceRestrict = FALSE;
		fClanRestrict = TRUE;
		for ( iClan = 0; iClan < MAX_CLAN; iClan++ )
		{
   		  tClan = get_clan_index( iClan );
   		  if ( !(tClan) )
   		    continue;
   		    
		  if ( !str_cmp( arg1, strip_color(tClan->name) ) )
		  {
		    rgfClan[iClan] = TRUE;
		    break;
		  }
		}  
		if ( iClan >= MAX_CLAN )
		{	
		  fClanRestrict = FALSE;
		  fNameRestrict = TRUE;
		} 

/*		if ( iClan >= MAX_CLAN )
		{ 
		  fClanRestrict = FALSE;   
		  fGuildRestrict = TRUE;
		  for ( iGuild = 0; guild_table[iGuild].name[0] != '\0'; iGuild++ )
		  {
		     if ( !str_cmp( arg1, guild_table[iGuild].name ) )
		     {
		       rgfGuild[iGuild] = TRUE;
		       break;
		     }
		  }
		
		if ( guild_table[iGuild].name[0] == '\0' )
		{
		  fGuildRestrict = FALSE;
		  fNameRestrict = TRUE;
		} 
 		} */
		}
		}
		
	    }
	}
    }

    /*
     * Now show matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
    rgfClan[0] = FALSE;
    if ( fNameRestrict )
      send_to_char( C_DEFAULT,
"\n\r\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"
"/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\n\r", ch );  
    for ( d = descriptor_list; d; d = d->next )
    {
	CHAR_DATA       *wch;
	CLAN_DATA       *pClan;
	char      const *class;
        char      const *race;
        char             clan[MAX_STRING_LENGTH];
        
	wch   = ( d->original ) ? d->original : d->character;

	/*
	 * Check for match against restrictions.
	 * Don't use trust as that exposes trusted mortals.
	 */
	if ( d->connected != CON_PLAYING || !can_see( ch, wch ) )
	    continue;

	if (   wch->level < iLevelLower
	    || wch->level > iLevelUpper
	    || ( fImmortalOnly  && wch->level < LEVEL_IMMORTAL )
            || ( fHeroOnly      && wch->level < LEVEL_HERO )
            || ( fHeroOnly      && wch->level > L_CHAMP5 )
	    || ( fClassRestrict && !rgfClass[prime_class(wch)] )
	    || ( fRaceRestrict  && !rgfRace[wch->race] ) 
	    || ( fNameRestrict  && str_prefix( arg1, wch->name ) ) 
/*	    || ( fGuildRestrict && str_cmp( wch->guild->name,
guild_table[iGuild].name ) )   */
	    || ( fClanRestrict  && !rgfClan[wch->clan] ) )
	    continue;

	nMatch++;

        if ( wch->level >= LEVEL_IMMORTAL )
          num_of_imm++;

        noclass[prime_class(wch)]++;
        
	/*
	 * Figure out what to print for class.
	 */
	class = class_short( wch );
	if ( wch->level >= LEVEL_HERO )
	    switch ( wch->level )
	      {
	      default: break;
              case L_IMP: class = wch->sex == 2 ? "&PSTORM LADY&W"
						: "&RSTORM LORD&W"; break;
              case L_CON: class = "CODER"; break;
	      case L_DIR: class = "COUNCIL"; break;
              case L_GOD: class = "GUARDIAN"; break;
	      case L_SEN: class = wch->sex == 2 ? "GODDESS"
						: "GOD"; break;
              case L_DEM: class = "BUILDER";break;
              case L_JUN: class = "IMMORTAL"; break;
	      case L_APP: class = "AVATAR"; break;
	      case L_CHAMP5:
	      case L_CHAMP4:
	      case L_CHAMP3:
	      case L_CHAMP2:
	      case L_CHAMP1:
	      case LEVEL_HERO:
		switch ( prime_class( wch ) )
		{
		default: class = "CHAMPION"; break;
		case 0:  class = "&BARCH MAGUS&W"; break;
		case 1:  class = wch->sex == 2 ? "&CMATRIARCH&W"
				  	       : "&CPATRIARCH&W"; break;
		case 2:  class = "&zBLACKGUARD&W"; break;
		case 3:  class = "&wKNIGHT&W"; break;
		case 4:  class = "&GMINDBENDER&W"; break;
		case 5:  class = "&gHIEROPHANT&W"; break;
		case 6:  class = wch->sex == 2 ? "&gLADYRANGER&W"
					       : "&gLORDRANGER&W"; break;
		case 7:  class = "&wCRUSADER&W"; break;
		case 8:  class = "&PLOREMASTER&W"; break;
		case 9:  class = "&rKINDRED&W"; break;
		case 10: class = "&cLICHMASTER&W "; break;
                case 11: class = "&OLYCANTHROPE&W"; break;
		case 12: class = "&cSENSEI"; break;

		}
	      }
	if ( wch->level >= L_APP 
	&& wch->pcdata->whotype 
	&& str_cmp( wch->pcdata->whotype, "!!!!!!!!!!!!" ) )
		class = wch->pcdata->whotype;
	/*
	 * Figure out what to print for race.
	 */
	race = (get_race_data(wch->race))->race_name;
        /* Clan Stuff */
        if (wch->clan != 0)
          {
            pClan = get_clan_index(wch->clan);
           if ( IS_SET(pClan->settings, CLAN_PKILL) )
              switch ( wch->clev )
              {
               default:
                  sprintf( clan, "-<%s>-", pClan->name ); break;
               case 0:
                  sprintf( clan, "-<%s>-", pClan->name ); break;
               case 1:
                  sprintf( clan, "-<Centurion of %s>-", pClan->name ); break;
               case 2:
                  sprintf( clan, "-<Council of %s>-", pClan->name ); break;
               case 3:
                  sprintf( clan, "-<Leader of %s>-", pClan->name ); break;
               case 4:
                  sprintf( clan, "-<Champion of %s>-", pClan->name ); break;
               case 5:
                  sprintf( clan, "-<Deity of %s>-", pClan->name ); break;
              }
           else
              switch ( wch->clev )
              {
               default:
                  sprintf( clan, "(%s&C)", pClan->name ); break;
               case 0:
                  sprintf( clan, "(%s&C)", pClan->name ); break;
               case 1:
                  sprintf( clan, "(Centurion of %s&C)", pClan->name ); break;
               case 2:
                  sprintf( clan, "(Council of %s&C)", pClan->name ); break;
               case 3:
                  sprintf( clan, "(Leader of %s&C)", pClan->name ); break;
               case 4:
                  sprintf( clan, "(Champion of %s&C)", pClan->name ); break;
               case 5:
                  sprintf( clan, "(Deity of %s&C)", pClan->name ); break;
              }
 	}
 	/*
	 * Format it up.
	 */
	if ( wch->level < (LEVEL_HERO + 1) )
	  {
	  sprintf( buf + strlen( buf ), "[%-11s %s %d] ",
	  	   class, race, wch->level );
	  }
	else if ( wch->level > LEVEL_HERO
	     && wch->pcdata->whotype
	     && !strcmp( class, wch->pcdata->whotype ) )
		{
		int len = strlen_wo_col(wch->pcdata->whotype);
		int len2 = len;
		len = 6 - ( len/2 + ( len % 2 != 0 ) );
		len2 = ( len2 %2 == 0 ) ? len - 1 : len;
	        sprintf( buf + strlen(buf), "[    %*s%-s%*s&W   ] ", 
			 len, "", class, len2, "" );
		}
	else
		{
		int len = strlen_wo_col((char *)class);
		int len2 = len;
		len = 9 - ( len/2 + ( len % 2 != 0 ) );
		len2 = ( len2 %2 == 0 ) ? len - 1 : len;
	        sprintf( buf + strlen(buf), "[    %*s%-s%*s&W   ] ", 
			 len, "", class, len2, "" );
		}
        send_to_char( AT_WHITE, buf, ch );
		if ( ( IS_SET( wch->act, PLR_SILENCE ) ) && ( ch->level >= L_DIR ) ) {
			send_to_char( AT_WHITE, "&z[&rSILENCED&z] ", ch );
		}
        if ( IS_SET( wch->act, PLR_QUEST ) )
        {
          send_to_char( AT_RED, "<QUEST> ", ch );
        }
        if ( IS_SET( wch->act, PLR_QUESTOR ) )
          send_to_char( AT_BLUE, "&z(&YQ&z) ", ch );

        if(wch->guild != NULL)
        {
          buf[0] = '\0';
          sprintf(buf+strlen(buf), "[%s", wch->guild->name);
          send_to_char(wch->guild->color, buf, ch);
          buf[0] = '\0';
	  switch(wch->guild_rank)
          {
	    case 0: sprintf(buf+strlen(buf), "] ");		break;
	    case 1: sprintf(buf+strlen(buf), " %s] ", wch->sex == SEX_FEMALE ?
			    "Lady" : "Lord" );	break;
	    case 2: sprintf(buf+strlen(buf), " High %s] ",
			    wch->sex == SEX_FEMALE ? "Lady" : "Lord" );	break;
	    case 3: sprintf(buf+strlen(buf), " Over%s] ",
			    wch->sex == SEX_FEMALE ? "lady" : "lord" );	break;
            case 4: sprintf(buf+strlen(buf), " Deity] " ); break;
            default: sprintf(buf+strlen(buf), "Bug] ");		break;
          }
          send_to_char(wch->guild->color, buf, ch);
        }
        if ( IS_SET( wch->act, PLR_AFK ) )
           send_to_char( AT_YELLOW, "&z[&PA&z] ", ch );
	buf[0] = '\0';
	if ( IS_SET( wch->act, PLR_PKILLER ) )
	  send_to_char( C_DEFAULT, "&z[&RPK&z] ", ch );
	buf[0] = '\0';

	if ( IS_SET( wch->act, PLR_WIZINVIS ) )
	  {
	  sprintf( buf, "%s %d%s", "&w[&zWizin", wch->wizinvis, "&w] ");
	  send_to_char( AT_WHITE, buf, ch );
	  }
        if ( IS_SET( wch->act, PLR_CLOAKED ) )
          {
          sprintf( buf, "%s %d%s", "&w[&zCloak", wch->cloaked, "&w] ");
          send_to_char( AT_WHITE, buf, ch );
          }

	if ( wch->desc && wch->desc->editor != 0 )
	  {
	  if ( wch->desc->editor == 13 ) /* forging eq */
	    send_to_char( AT_DGREY, "&z[&OForging&z] ", ch );
	  else
	    send_to_char( AT_WHITE, "&z[&CB&z] ", ch );
	  }

	if ( IS_SET( wch->act, PLR_THIEF ) )
	  send_to_char( AT_DGREY, "[&GT&z] ", ch );

	/* booger */
	incrs = wch->incarnations;
	if ( IS_SET( wch->act2, PLR_REMORT ) )
	{
		for ( remorts = 0; remorts < incrs ; remorts++ )
			send_to_char( AT_RED, "*", ch );
	}

	buf[0] = '\0';

	if ( wch->pcdata->lname )
	  sprintf( buf + strlen( buf ), "%s%s%s%s ",
	  wch->name, ( wch->pcdata->lname[0] != '\0' ) ? " " : "", 
/*	  wch->name, ( *wch->pcdata->lname != 39 ) ? " " : "",  */
	  wch->pcdata->lname, wch->pcdata->title );
	else
	  sprintf( buf + strlen( buf ), "%s%s ", 
	    wch->name, wch->pcdata->title );
        send_to_char( AT_WHITE, buf, ch);
	buf[0] = '\0';
        if (wch->clan != 0)
          sprintf( buf + strlen( buf ), "%s\n\r", clan );
        else sprintf( buf, "\n\r" );
        if (wch->clan != 0)
          {
            pClan=get_clan_index(wch->clan);
            if IS_SET(pClan->settings,CLAN_PKILL)
              send_to_char( AT_RED, buf, ch );
	    else
	      send_to_char( AT_LBLUE, buf, ch );
	  }
	else
	  send_to_char(C_DEFAULT, buf, ch );
	buf[0] = '\0';
	if ( fNameRestrict && !str_cmp( arg1, wch->name ) )
	   break;
       
    }

  if ( nMatch > 0 )

  {
    if ( fNameRestrict )
      send_to_char( C_DEFAULT,
"\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/"
"\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\n\r", ch );
/*    sprintf( buf2, "Mag&g[&G%d&g]&G Cle&g[&G%d&g]&G Thi&g[&G%d&g]&G "
      "War&g[&G%d&g]&G Psi&g[&G%d&g]&G Dru&g[&G%d&g]&G Rng&g[&G%d&g]&G "
      "Pal&g[&G%d&g]&G Brd&g[&G%d&g]&G Vam&g[&G%d&g]&G Nec&g[&G%d&g]&G\n\r",
      noclass[0], noclass[1], noclass[2], noclass[3], noclass[4], 
      noclass[5], noclass[6], noclass[7], noclass[8], noclass[9],
      noclass[10] );  
    strcat( buf, buf2 );
    send_to_char( AT_GREEN, buf, ch );
    buf[0]=buf2[0]='\0';
sprintf( buf, "Wwf&g[&G%d&g] &GMnk&g[&G%d&g] ", noclass[11], noclass[12] ); */
    send_to_char( AT_WHITE, buf, ch );
    buf[0]=buf2[0]='\0';

send_to_char(C_DEFAULT ,
"\n\r&W==================================================================\n\r",ch );
send_to_char(C_DEFAULT,
"&z======     &z[&PA&z]&Wfk &z[&RPK&z]&Willer &z[&GT&z]&Whief &z[&CB&z]&Wuilding &z[&YQ&z]&Wuesting    &z======\n\r",ch);
send_to_char(C_DEFAULT,"&W==================================================================\n\r \n\r",ch);

    sprintf( buf, "&O[&z%d&O]&z &OImm&zo&Ortals.\n\r", num_of_imm );
    sprintf( buf2, "&z[&O%d&z]&O &zTot&Oa&zl Player%s.\n\r",
	    nMatch, nMatch == 1 ? "" : "s" );
    strcat( buf, buf2 );
    send_to_char(AT_GREEN, buf, ch );
  	if ( doubleexp() == TRUE) {
		sprintf( buf2, "&BExperience Boost &Xis &YON!\n\r" );
		send_to_char(AT_GREEN, buf2, ch );  
	}
  }
 else
  send_to_char( AT_GREEN, "There is no class/race/clan/person by that name in the game.\n\r", ch );
  return;
}

/* Outdated by my changes to normal who ( besides, i'm too lazy to type
   whoi or whois :)   -  TRI
   Contributed by Kaneda 
void do_whois( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char             buf  [ MAX_STRING_LENGTH  ];
    char             name [ MAX_INPUT_LENGTH   ];
    bool             found;
    argument = one_argument( argument, name );
    found = FALSE;
    if( name[0] == '\0' )
    {
	send_to_char(AT_DGREEN, "Usage:  whois <name>\n\r", ch );
	return;
    }

    name[0] = UPPER( name[0] );

    buf[0] = '\0';
    for( d = descriptor_list ; d ; d = d->next )
    {
	CHAR_DATA       *wch;
	char      const *class;
        char      const *race;
	char             clan[MAX_STRING_LENGTH];
	CLAN_DATA       *pClan;
	
	wch = ( d->original ) ? d->original : d->character; 

	if( d->connected != CON_PLAYING || !can_see( ch, wch ) )
	    continue;
  
	if( str_prefix( name, wch->name ) )
	    continue;

	found = TRUE;
	class = class_table[ wch->class ].who_name;
	if( wch->level >= LEVEL_HERO )
	    switch( wch->level )
	    {
              case L_IMP: class = "STORM LORD"; break;
	      case L_CON: class = " COUNCIL  "; break;
	      case L_DIR: class = " GUARDIAN "; break;
	      case L_SEN: class = " CREATOR  "; break;
	      case L_GOD: 
	             if ( wch->sex == 2 )
	                class = " GODDESS  ";
	             else
	                class = "   GOD    ";
	             break;
	      case L_DEM: class = "  DEITY   ";break;
	      case L_JUN: class = "  SAINT   "; break;
	      case L_APP: class = "  AVATAR  "; break;
	      case LEVEL_HERO: class = " CHAMPION "; break;
	    }
        race = (get_race_data(wch->race))->race_name;    
         Clan Stuff 
        if (wch->clan != 0)
          {
            pClan = get_clan_index(wch->clan);
           if IS_SET(pClan->settings, CLAN_PKILL)
              switch ( wch->clev )
              {
               default:
                  sprintf( clan, "-<%s>-", pClan->name ); break;
               case 0:
                  sprintf( clan, "-<%s>-", pClan->name ); break;
               case 1:
                  sprintf( clan, "-<Centurion of %s>-", pClan->name ); break;
               case 2:
                  sprintf( clan, "-<Council of %s>-", pClan->name ); break;
               case 3:
                  sprintf( clan, "-<Leader of %s>-", pClan->name ); break;
               case 4:
                  sprintf( clan, "-<Champion of %s>-", pClan->name ); break;
               case 5:
                  sprintf( clan, "-<Deity of %s>-", pClan->name ); break;
              }
           else
              switch ( wch->clev )
              {
               default:
                  sprintf( clan, "(%s)", pClan->name ); break;
               case 0:
                  sprintf( clan, "(%s)", pClan->name ); break;
               case 1:
                  sprintf( clan, "(Centurion of %s)", pClan->name ); break;
               case 2:
                  sprintf( clan, "(Council of %s)", pClan->name ); break;
               case 3:
                  sprintf( clan, "(Leader of %s)", pClan->name ); break;
               case 4:
                  sprintf( clan, "(Champion of %s)", pClan->name ); break;
               case 5:
                  sprintf( clan, "(Deity of %s)", pClan->name ); break;
              }
 	}
	*
	 * Format it up.
	 *
	if ( wch->level < LEVEL_HERO )
	  sprintf( buf + strlen( buf ), "[%2d %s %s] ",
	   wch->level, race, class );
	else
	  sprintf( buf + strlen( buf ), "[%s] ", class);
        send_to_char( AT_WHITE, buf, ch );

        if(wch->guild != NULL)
        {
          buf[0] = '\0';
          sprintf(buf+strlen(buf), "[%s", wch->guild->name);
          send_to_char(wch->guild->color, buf, ch);
          buf[0] = '\0';
	  switch(wch->guild_rank)
          {
	    case 0: sprintf(buf+strlen(buf), "] ");		break;
	    case 1: sprintf(buf+strlen(buf), " Lord] ");	break;
	    case 2: sprintf(buf+strlen(buf), " High Lord] ");	break;
	    case 3: sprintf(buf+strlen(buf), " Overlord] ");	break;
            cast 4: sprintf(buf+strlen(buf), " Diety] ");       break;
            default: sprintf(buf+strlen(buf), "Bug] ");		break;
          }
          send_to_char(wch->guild->color, buf, ch);
        }
	buf[0] = '\0';
	if ( wch->pcdata->lname )
	  sprintf( buf + strlen( buf ), "%s%s%s%s. ",
	    wch->name, ( wch->pcdata->lname[0] != '\0' ) ? " " : "", 
	    wch->pcdata->lname, wch->pcdata->title );
	else
  	  sprintf( buf + strlen( buf ), "%s%s. ", 
	    wch->name, wch->pcdata->title );
        send_to_char(AT_GREEN, buf, ch);
	buf[0] = '\0';
        if (wch->clan != 0)
          sprintf( buf + strlen( buf ), "%s\n\r",  clan );
        else sprintf( buf, "\n\r" );
        send_to_char( AT_RED, buf, ch );
	buf[0] = '\0';
      }

    if ( !found )
        send_to_char(AT_DGREEN, "No one matches the given criteria.\n\r", ch );
    return;
}
*/


void do_inventory( CHAR_DATA *ch, char *argument )
{
    send_to_char(AT_WHITE, "You are carrying:\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int       iWear;
    bool      found;
    char      buf[ MAX_INPUT_LENGTH ];
    send_to_char(AT_WHITE, "You are using:\n\r", ch );
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( !( obj = get_eq_char( ch, iWear ) ) )
	    {
	    if ( is_class( ch, CLASS_WEREWOLF )
     	    && ( ( iWear == WEAR_WIELD && !get_eq_char( ch, iWear ) )
	    || ( iWear == WEAR_WIELD_2 && !get_eq_char( ch, iWear )
	    && !get_eq_char( ch, WEAR_HOLD )
	    && !get_eq_char( ch, WEAR_SHIELD ) 
	    && ch->pcdata->learned[gsn_dualclaw] > 0 ) ) )
		{
		send_to_char(AT_BLUE, where_name[iWear], ch );
		sprintf( buf, "%s%s%s&ca Werewolf Claw",
		is_affected( ch, gsn_flamehand ) ? "&r(Burning) " : "",
		is_affected( ch, gsn_chaoshand ) ? "&Y(Chaotic) " : "",
		is_affected( ch, gsn_frosthand ) ? "&B(Frosty) " : "" );
		send_to_char(AT_CYAN, buf, ch );
		send_to_char(AT_CYAN, "\n\r", ch );
		found = TRUE;
		}
    	    else if ( is_class( ch, CLASS_MONK )
	     && ( ( iWear == WEAR_WIELD && !get_eq_char( ch, iWear ) )
	     || ( iWear == WEAR_WIELD_2 && !get_eq_char( ch, iWear )
		&& !get_eq_char( ch, WEAR_HOLD )
		&& !get_eq_char( ch, WEAR_SHIELD ) 
		&& ch->pcdata->learned[gsn_blackbelt] > 0 ) ) )
		{
		send_to_char(AT_BLUE, where_name[iWear], ch );
		sprintf( buf, "%s%s%s&cYour Fist", 
		is_affected( ch, gsn_flamehand ) ? "&r(Burning) " : "",
		is_affected( ch, gsn_chaoshand ) ? "&Y(Chaotic) " : "",
		is_affected( ch, gsn_frosthand ) ? "&B(Frosty) " : "" );
		send_to_char(AT_CYAN, buf, ch );
		send_to_char(AT_CYAN, "\n\r", ch );
		found = TRUE;
		}
	    continue;
	    }
	send_to_char(AT_BLUE, where_name[iWear], ch );
	if ( can_see_obj( ch, obj ) )
	{
	    send_to_char(AT_CYAN, format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char(AT_CYAN, "\n\r", ch );
	}
	else
	{
	    send_to_char(AT_RED, "something.\n\r", ch );
	}
	found = TRUE;
    }

    if ( !found )
	send_to_char(AT_RED, "Nothing.\n\r", ch );

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    char     *msg;
    char      arg1 [ MAX_INPUT_LENGTH ];
    char      arg2 [ MAX_INPUT_LENGTH ];
    int       value1;
    int       value2;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Compare what to what?\n\r", ch );
	return;
    }

    if ( !( obj1 = get_obj_carry( ch, arg1 ) ) )
    {
	send_to_char(C_DEFAULT, "You do not have that item.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	for ( obj2 = ch->carrying; obj2; obj2 = obj2->next_content )
	{
	    if ( obj2->wear_loc != WEAR_NONE
		&& can_see_obj( ch, obj2 )
		&& obj1->item_type == obj2->item_type
		&& ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if ( !obj2 )
	{
	    send_to_char(C_DEFAULT, "You aren't wearing anything comparable.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( !( obj2 = get_obj_carry( ch, arg2 ) ) )
	{
	    send_to_char(C_DEFAULT, "You do not have that item.\n\r", ch );
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

    if ( !msg )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act(C_DEFAULT, msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
    do_help( ch, "diku" );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    DESCRIPTOR_DATA *d;
    char             buf [ MAX_STRING_LENGTH ];
    char             arg [ MAX_INPUT_LENGTH  ];
    bool             found;

    if ( !check_blind( ch ) )
        return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(AT_PURPLE, "Players near you:\n\r", ch );
	found = FALSE;
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
		&& ( victim = d->character )
		&& !IS_NPC( victim )
		&& victim->in_room
		&& ( victim->in_room->area == ch->in_room->area
		|| ch->level >= LEVEL_IMMORTAL )
		&& can_see( ch, victim ) )
	    {
		found = TRUE;
		if ( ch->level >= LEVEL_IMMORTAL )
	  	   sprintf( buf, "%-18s [%c][%5d] %s\n\r",
			 victim->name, victim->fighting ? 'F' : 
			 (victim->desc && victim->desc->editor != 0) ? 'E' :
                         (victim->desc && victim->desc->pString) ? 'W' : ' ', 
			 victim->in_room->vnum, victim->in_room->name );
		else
			sprintf( buf, "%-28s %s\n\r",
			victim->name, victim->in_room->name );
		send_to_char(AT_PINK, buf, ch );
	    }
	}
	if ( !found )
	    send_to_char(AT_PINK, "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = char_list; victim; victim = victim->next )
	{      	    
	    if (( (!victim->in_room
		|| IS_AFFECTED( victim, AFF_HIDE ) 
		|| IS_AFFECTED( victim, AFF_SNEAK )
		|| victim->race == RACE_HALFLING ) && ch->level < LEVEL_IMMORTAL) 
		|| (( ch->level >= LEVEL_IMMORTAL ) && (!victim->in_room)) )
	        continue;

	    if (( ( victim->in_room->area == ch->in_room->area) 
	    || ( ch->level >= LEVEL_IMMORTAL ))
	    && can_see( ch, victim )
	    && is_name(ch, arg, victim->name ) )
	    {
		found = TRUE;
		if ( ch->level >= LEVEL_IMMORTAL )
			sprintf( buf, "%-18s [%c][%5d] %s\n\r",
			PERS( victim, ch ), victim->fighting ? 'F' : 
			( victim->desc && victim->desc->editor !=0 ) ? 'E' : ' ',
			victim->in_room->vnum, victim->in_room->name );
		else 
			sprintf( buf, "%-28s %s\n\r",
			PERS( victim, ch ), victim->in_room->name );
		send_to_char(AT_PINK, buf, ch );
		break;
	    }
	  
	}
	if ( !found )
	    act(AT_PINK, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      *msg                      = '\0';
    char      *buf                      = '\0';
    char       arg [ MAX_INPUT_LENGTH ];
    int        diff;
    int        hpdiff;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Consider killing whom?\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
	send_to_char(C_DEFAULT, "They're not here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char(C_DEFAULT, "The gods do not accept this type of sacrifice.\n\r",
		     ch );
	return;
    }

    diff = victim->level - ch->level;
         if ( diff <= -50 ) msg = "$N almost died from your mere gaze!";
    else if ( diff <= -25 ) msg = "$N is a complete wimp."; 
    else if ( diff <= -15 ) msg = "You can kill $N naked and weaponless.";
    else if ( diff <=  -5 ) msg = "$N is no match for you.";
    else if ( diff <=  -2 ) msg = "$N looks like an easy kill.";
    else if ( diff <=   1 ) msg = "The perfect match!";
    else if ( diff <=   4 ) msg = "$N says 'Do you feel lucky, punk?'.";
    else if ( diff <=   9 ) msg = "$N laughs at you mercilessly.";
    else if ( diff <=  12 ) msg = "Oh boy, this is gonna be tough.";
    else if ( diff <=  25 ) msg = "You got to be kidding!";
    else                    msg = "&RDont try it, you WILL die!"; 
    act(C_DEFAULT, msg, ch, NULL, victim, TO_CHAR );

    /* additions by king@tinuviel.cs.wcu.edu */
    hpdiff = ( ch->hit - victim->hit );

    if ( ( ( diff >= 0) && ( hpdiff <= 0 ) )
	|| ( ( diff <= 0 ) && ( hpdiff >= 0 ) ) )
    {
        send_to_char(C_DEFAULT, "Also,", ch );
    }
    else
    {
        send_to_char(C_DEFAULT, "However,", ch );
    }
    
    if ( hpdiff >= 2501 )
        buf = " $E is of very fragile constitution.";
    if ( hpdiff <= 2500 )
        buf = " you are currently much healthier than $E.";
    if ( hpdiff <= 500 )
        buf = " you are currently healthier than $E.";
    if ( hpdiff <= 200 ) 
        buf = " you are currently slightly healthier than $E.";
    if ( hpdiff <= 50 )
        buf = " you are a teensy bit healthier than $E.";
    if ( hpdiff <= 0 )
        buf = " $E is a teensy bit healthier than you.";
    if ( hpdiff <= -50 )
        buf = " $E is slightly healthier than you.";
    if ( hpdiff <= -200 )
        buf = " $E is healthier than you.";
    if ( hpdiff <= -500 )
        buf = " $E is much healthier than you.";
    if ( hpdiff <= -2500 )
        buf = " $E ridicules your hitpoints.";
    if ( hpdiff <= -10000 ) 
        buf = " $E is built like a TANK!.";
             
    act(C_DEFAULT, buf, ch, NULL, victim, TO_CHAR );
    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf [ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch ) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    buf[0] = '\0';
    
    if ( !str_cmp( "none", title ) )
    {
       free_string( ch->pcdata->title );
       ch->pcdata->title = str_dup( " " );
       return;
     }

    if ( isalpha( title[0] ) || isdigit( title[0] ) )
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
    if ( IS_NPC( ch ) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char(C_DEFAULT, "Change your title to what?\n\r", ch );
	return;
    }

    if ( strlen_wo_col( argument ) > 50 )
	{
	send_to_char( C_DEFAULT, "Max title length is 50 excluding color codes.",
		      ch );
	return;
	}
    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char(C_DEFAULT, "Ok.\n\r", ch );
}



void do_description( CHAR_DATA *ch, char *argument )
{
/*    if ( argument[0] != '\0' )
    {
	buf[0] = '\0';
	smash_tilde( argument );
	if ( argument[0] == '+' )
	{
	    if ( ch->description )
		strcat( buf, ch->description );
	    argument++;
	    while ( isspace( *argument ) )
		argument++;
	}

	if ( strlen( buf ) + strlen( argument ) >=  MAX_STRING_LENGTH  - 2 )
	{
	    send_to_char(AT_CYAN, "Description too long.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }*/

    string_append( ch, &ch->description );

/*    send_to_char(AT_CYAN, "Your description is:\n\r", ch );
    send_to_char(AT_CYAN, ch->description ? ch->description : "(None).\n\r", ch );*/
    return;
}



void do_report( CHAR_DATA *ch, char *argument )
{
    char buf [ MAX_INPUT_LENGTH ];

    if ( !is_class( ch, CLASS_VAMPIRE ) )
        sprintf( buf,
	       "You report: %d/%d hp %d/%d mana %d/%d mv %d xp.\n\r",
	       ch->hit,  MAX_HIT(ch),
	       ch->mana, MAX_MANA(ch),
	       ch->move, MAX_MOVE(ch),
	       ch->exp );
    else
        sprintf( buf,
	       "You report: %d/%d hp %d/%d bp %d/%d mv %d xp.\n\r",
	       ch->hit,  MAX_HIT(ch),
	       ch->bp, MAX_BP(ch),
	       ch->move, MAX_MOVE(ch),
	       ch->exp );
	       
    send_to_char(AT_RED, buf, ch );

    if ( !is_class( ch, CLASS_VAMPIRE ) )
         sprintf( buf,
	            "$n reports: %d/%d hp %d/%d mana %d/%d mv %d xp.",
        	    ch->hit,  MAX_HIT(ch),
        	    ch->mana, MAX_MANA(ch),
         	    ch->move, MAX_MOVE(ch),
        	    ch->exp );
    else
         sprintf( buf,
	            "$n reports: %d/%d hp %d/%d bp %d/%d mv %d xp.",
        	    ch->hit,  MAX_HIT(ch),
        	    ch->bp,   MAX_BP(ch),
         	    ch->move, MAX_MOVE(ch),
        	    ch->exp );

    act(AT_RED, buf, ch, NULL, NULL, TO_ROOM );

    return;
}



void do_practice( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH   ];
    char buf1 [ MAX_STRING_LENGTH*2 ];
    int  sn;

    if ( IS_NPC( ch ) )
	return;

    buf1[0] = '\0';


    if ( argument[0] == '\0' )
    {
	CHAR_DATA *mob;
	int        col;

	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
	    if ( mob->deleted )
	        continue;
	    if ( IS_NPC( mob ) && IS_SET( mob->act, ACT_PRACTICE ) )
	        break;
	}

	col    = 0;
	for ( sn = 0; skill_table[sn].name[0] != '\0'; sn++ )
	{
	    if ( !skill_table[sn].name )
		break;
	    if (!can_practice_skpell( ch, sn))
		continue;

	    if ( ( mob ) || ( ch->pcdata->learned[sn] > 0 ) )
	    {
		sprintf( buf, "&W%21s &Y%3d%%",
			skill_table[sn].name, ch->pcdata->learned[sn] );
		strcat( buf1, buf );
		if ( ++col % 3 == 0 )
		    strcat( buf1, "\n\r" );
		else
		    strcat( buf1, " " );
	    }
	}

	if ( col % 3 != 0 )
	    strcat( buf1, "\n\r" );

	sprintf( buf, "You have %d practice sessions left.\n\r",
		ch->practice );
	strcat( buf1, buf );
	send_to_char(C_DEFAULT, buf1, ch );
    }
    else
    {
	CHAR_DATA *mob;
	int        adept;

	if ( !IS_AWAKE( ch ) )
	{
	    send_to_char(C_DEFAULT, "In your dreams, or what?\n\r", ch );
	    return;
	}

	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
	    if ( mob->deleted )
	        continue;
	    if ( IS_NPC( mob ) && IS_SET( mob->act, ACT_PRACTICE ) )
		break;
	}

	if ( !mob )
	{
	    send_to_char(C_DEFAULT, "You can't do that here.\n\r", ch );
	    return;
	}

	if ( ch->practice <= 0 )
	{
	    send_to_char(C_DEFAULT, "You have no practice sessions left.\n\r", ch );
	    return;
	}

	if ( ( sn = skill_lookup( argument ) ) < 0
	    || ( IS_NPC( ch ) ) )
	{
	    send_to_char(C_DEFAULT, "You can't practice that.\n\r", ch );
	    return;
	}

	if (!can_practice_skpell( ch, sn ) )
	{
	send_to_char(C_DEFAULT, "You can't practice that.\n\r", ch );
	return;
	}

/* Practice to approx 50% then use to learn */
        if ( IS_NPC( ch ) || ch->pcdata->learned[sn] > 35 )
        {
            sprintf( buf, "&W%s tells you &c'&CI've already trained you all I can in %s&c'&w.\n\r",
                   capitalize( mob->name ), skill_table[sn].name );
            send_to_char( C_DEFAULT, buf, ch );
            sprintf( buf, "&W%s tells you &c'&CPractice in the real world for more experience&c'&w.\n\r",
                   capitalize( mob->name ) );
            send_to_char( C_DEFAULT, buf, ch );
            return;
        }

	adept = IS_NPC( ch ) ? 100 :
		class_table[prime_class( ch )].skill_adept;

	if ( ch->pcdata->learned[sn] >= adept )
	{
	    sprintf( buf, "You are already an adept of %s.\n\r",
		skill_table[sn].name );
	    send_to_char(C_DEFAULT, buf, ch );
	}
	else
	{
	    ch->practice--;
	    ch->pcdata->learned[sn] += int_app[get_curr_int( ch )].learn; 
/*	    ch->pcdata->learned[sn] += ( ( get_curr_int( ch ) * 4 ) / 3 );*/
	    if ( ch->pcdata->learned[sn] < adept )
	    {
		act(C_DEFAULT, "You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act(C_DEFAULT, "$n practices $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	    else
	    {
		ch->pcdata->learned[sn] = adept;
		act(C_DEFAULT, "You are now an adept of $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act(C_DEFAULT, "$n is now an adept of $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
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
    char buf [ MAX_STRING_LENGTH ];
    char arg [ MAX_INPUT_LENGTH  ];
    int  wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = MAX_HIT(ch) / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	send_to_char(C_DEFAULT, "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if ( wimpy > MAX_HIT(ch) )
    {
	send_to_char(C_DEFAULT, "Such cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
    send_to_char(C_DEFAULT, buf, ch );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char *pArg;
    char *pwdnew;
    char *p;
    char  arg1 [ MAX_INPUT_LENGTH ];
    char  arg2 [ MAX_INPUT_LENGTH ];
    char  cEnd;

    if ( IS_NPC( ch ) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace( *argument ) )
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
    while ( isspace( *argument ) )
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
	send_to_char(C_DEFAULT, "Syntax: password <old> <new>.\n\r", ch );
	return;
    }

    if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char(C_DEFAULT, "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }

    if ( strlen( arg2 ) < 5 )
    {
	send_to_char(C_DEFAULT,
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
	    send_to_char(C_DEFAULT,
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( ch, FALSE );
    send_to_char(C_DEFAULT, "Ok.\n\r", ch );
    return;
}



void do_socials( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
/*    int  iSocial;*/
    int  col;
    SOCIAL_DATA * pSocial;

    buf1[0] = '\0';
    col = 0;
/*    for ( iSocial = 0; social_table[iSocial].name[0] != '\0';iSocial++)*/

    for(pSocial = social_first;pSocial;pSocial = pSocial->next)
    {
	sprintf( buf, "%-12s", pSocial->name );
	strcat( buf1, buf );
	if ( ++col % 6 == 0 )
	    strcat( buf1, "\n\r" );
    }
 
    if ( col % 6 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char(C_DEFAULT, buf1, ch );
    return;
}

void do_newbie( CHAR_DATA *ch, char *argument )
{
  char buf  [ MAX_STRING_LENGTH ];
  char buf1 [ MAX_STRING_LENGTH ];
  int col;
  NEWBIE_DATA *pNewbie;

  buf1[0] = '\0';
  col = 0;

  for( pNewbie = newbie_first; pNewbie; pNewbie = pNewbie->next )
  {
    sprintf( buf, "%-12s", pNewbie->keyword );
    strcat( buf1, buf );
    if( ++col % 6 == 0 )
      strcat( buf1, "\n\r" );
  }

  if( col % 6 != 0 )
    strcat( buf1, "\n\r" );

  send_to_char( C_DEFAULT, buf1, ch );
  return;

}

/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  int  cmd;
  int  col = 0;
  int  trust = get_trust(ch);
 
  for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
  {
    if ( cmd_table[cmd].level < LEVEL_HERO &&
         str_prefix("mp", cmd_table[cmd].name) &&
         can_use_cmd(cmd, ch, trust) )
    {
      sprintf( buf, "%-16s", cmd_table[cmd].name );
      if ( ++col % 5 == 0 )
        strcat( buf, "\n\r" );
      send_to_char(C_DEFAULT, buf, ch);
    }
  }
 
  if ( col % 5 != 0 )
    send_to_char(C_DEFAULT, "\n\r", ch);

  return;
}



void do_channels( CHAR_DATA *ch, char *argument )
{
    char arg [ MAX_INPUT_LENGTH  ];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
	{
	    send_to_char(AT_PURPLE, "You are silenced.\n\r", ch );
	    return;
	}

	send_to_char(AT_PURPLE, "Channels:", ch );

	send_to_char(AT_PINK, !IS_SET( ch->deaf, CHANNEL_AUCTION  )
		     ? " +AUCTION"
		     : " -auction",
		     ch );

	send_to_char(AT_PINK, !IS_SET( ch->deaf, CHANNEL_CHAT     )
		     ? " +CHAT"
		     : " -chat",
		     ch );

        send_to_char(AT_PINK, !IS_SET( ch->deaf, CHANNEL_OOC )
		     ? " +OOC"
		     : " -ooc",
		     ch );

	send_to_char(AT_PINK, !IS_SET( ch->deaf, CHANNEL_CLASS )
		     ? " +CLASS"
		     : " -class",
		     ch );

        send_to_char(AT_PINK, !IS_SET( ch->deaf, CHANNEL_CLAN )
                     ? " +CLAN"
                     : " -clan",
                     ch );
/* 	if( ch->level > 2 )
        send_to_char(AT_WHITE, !IS_SET( ch->deaf, CHANNEL_VENT )
                     ? " +VENT"
                     : " -vent",
                     ch );
*/
	if(IS_HERO(ch)) /* XOR */
	{
	  send_to_char(AT_PINK, !IS_SET(ch->deaf, CHANNEL_HERO)
	   ? " +HERO"
	   : " -hero", ch);
	}

	if ( IS_IMMORTAL( ch ) )
	{
	    send_to_char(AT_PINK, !IS_SET( ch->deaf, CHANNEL_IMMTALK )
			 ? " +IMMTALK"
			 : " -immtalk",
			 ch );
	    send_to_char(AT_PINK, !IS_SET( ch->deaf, CHANNEL_IMC )
	    		 ? " +IMC"
	    		 : " -imc",
	    		 ch );
	}

	send_to_char(AT_PINK, !IS_SET( ch->deaf, CHANNEL_MUSIC    )
		     ? " +MUSIC"
		     : " -music",
		     ch );

	send_to_char(AT_PINK, !IS_SET( ch->deaf, CHANNEL_QUESTION )
		     ? " +QUESTION"
		     : " -question",
		     ch );

	send_to_char(AT_PINK, !IS_SET( ch->deaf, CHANNEL_SHOUT    )
		     ? " +SHOUT"
		     : " -shout",
		     ch );

	send_to_char(AT_PINK, !IS_SET( ch->deaf, CHANNEL_YELL     )
		     ? " +YELL"
		     : " -yell",
		     ch );

        send_to_char( AT_LBLUE, !IS_SET(ch->deaf, CHANNEL_INFO)
                        ? " +INFO" : " -info", ch );

        send_to_char( AT_PURPLE, !IS_SET(ch->deaf, CHANNEL_CHALLENGE)
                        ? " +CHALLENGE" : " -challenge", ch );
  
	send_to_char( AT_RED, !IS_SET(ch->deaf, CHANNEL_ARENA)
			? " +ARENA" : " -arena", ch );


	/*
	 * Log Channel Display.
	 * Added by Altrag.
	 */
	if ( get_trust( ch ) >= L_APP )
	{
                send_to_char(AT_DGREY, !IS_SET( ch->deaf, CHANNEL_LOG )
				 ? " +LOG"
				 : " -log",
				 ch );
	}
	
	if ( get_trust( ch ) >= L_SEN )
	{
                send_to_char(AT_DGREY, !IS_SET( ch->deaf, CHANNEL_BUILD )
				 ? " +BUILD"
				 : " -build",
				 ch );
	}
	
	if ( get_trust( ch ) >= L_DIR )
	{
                send_to_char(AT_DGREY, !IS_SET( ch->deaf, CHANNEL_GOD )
				 ? " +GOD"
				 : " -god",
				 ch );
	}
	
	if ( get_trust( ch ) >= L_IMP )
	{
               send_to_char(AT_DGREY, !IS_SET( ch->deaf, CHANNEL_GUARDIAN )
	       			 ? " +GUARD"
	       			 : " -guard",
	       			 ch );
	}

        /* master channels added by Decklarean */
        if ( get_trust( ch ) >= L_IMP )
        {
               send_to_char(AT_DGREY, !IS_SET( ch->deaf, CHANNEL_CLASS_MASTER )
                                 ? " +CLASSMASTER"
                                 : " -classmaster",
                                 ch );
        }

        if ( get_trust( ch ) >= L_IMP )
        {
               send_to_char(AT_DGREY, !IS_SET( ch->deaf, CHANNEL_CLAN_MASTER )
                                 ? " +CLANMASTER"
                                 : " -clanmaster",
                                 ch );
        }

        if ( get_trust( ch ) >= L_IMP )
        {
               send_to_char(AT_DGREY, !IS_SET( ch->deaf, CHANNEL_GUILD_MASTER )
                                 ? " +GUILDMASTER"
                                 : " -guildmaster",
                                 ch );
        }
		
	send_to_char(AT_PINK, ".\n\r", ch );
	
    }
    else
    {
	int  bit;
	bool fClear;

	     if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
	    send_to_char(AT_PURPLE, "Channels -channel or +channel?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;
	else if ( !str_cmp( arg+1, "ooc"      ) ) bit = CHANNEL_OOC;
	else if ( !str_cmp( arg+1, "class"    ) ) bit = CHANNEL_CLASS;
        else if ( !str_cmp( arg+1, "chat"     ) ) bit = CHANNEL_CHAT;
	else if ( !str_cmp( arg+1, "hero"     ) ) bit = CHANNEL_HERO;
	else if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
	else if ( !str_cmp( arg+1, "imc"      ) ) bit = CHANNEL_IMC;
	else if ( !str_cmp( arg+1, "music"    ) ) bit = CHANNEL_MUSIC;
	else if ( !str_cmp( arg+1, "question" ) ) bit = CHANNEL_QUESTION;
	else if ( !str_cmp( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
	else if ( !str_cmp( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
	else if ( !str_cmp( arg+1, "log"      ) ) bit = CHANNEL_LOG;
	else if ( !str_cmp( arg+1, "build"    ) ) bit = CHANNEL_BUILD;
	else if ( !str_cmp( arg+1, "god"      ) ) bit = CHANNEL_GOD;
	else if ( !str_cmp( arg+1, "guard"    ) ) bit = CHANNEL_GUARDIAN;
	else if ( !str_cmp( arg+1, "info"     ) ) bit = CHANNEL_INFO;
	else if ( !str_cmp( arg+1, "challenge") ) bit = CHANNEL_CHALLENGE;
        else if ( !str_cmp( arg+1, "clan"     ) ) bit = CHANNEL_CLAN;
/*      else if ( !str_cmp( arg+1, "vent"     ) ) bit = CHANNEL_VENT;  */
        else if ( !str_cmp( arg+1,"classmaster")) bit = CHANNEL_CLASS_MASTER;
        else if ( !str_cmp( arg+1,"clanmaster") ) bit = CHANNEL_CLAN_MASTER;
        else if ( !str_cmp( arg+1,"guildmaster")) bit = CHANNEL_GUILD_MASTER;
	else if ( !str_cmp( arg+1,"arena"     ) ) bit = CHANNEL_ARENA;
	else if ( !str_cmp( arg+1, "all"      ) ) bit = ~0;
	else
	{
	    send_to_char(AT_PURPLE, "Set or clear which channel?\n\r", ch );
	    return;
	}

	if ( fClear )
	    REMOVE_BIT ( ch->deaf, bit );
	else
	    SET_BIT    ( ch->deaf, bit );

	send_to_char(AT_PURPLE, "Ok.\n\r", ch );
    }

    return;
}



/*
 * Contributed by Grodyn.
 */
void do_config( CHAR_DATA *ch, char *argument )
{
    char arg [ MAX_INPUT_LENGTH ];

    if ( IS_NPC( ch ) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char(AT_BLOOD, "&w[&Y Keyword   &w]&W Option\n\r", ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_AUTOEXIT  )
            ? "&w[&Y+AUTOEXIT  &w]&W You automatically see exits.\n\r"
	    : "&w[&Y-autoexit  &w]&W You don't automatically see exits.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_AUTOCOINS  )
	    ? "&w[&Y+AUTOCOINS &w]&W You automatically get coins from corpses.\n\r" 
	    : "&w[&Y-autocoins &w]&W You don't automatically get coins from corpses.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_AUTOSPLIT  )
	    ? "&w[&Y+AUTOSPLIT &w]&W You automatically split coins with group members.\n\r"
	    : "&w[&Y-autosplit &w]&W You don't automatically split coins with group members.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_AUTOLOOT  )
	    ? "&w[&Y+AUTOLOOT  &w]&W You automatically loot corpses.\n\r"
	    : "&w[&Y-autoloot  &w]&W You don't automatically loot corpses.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_AUTOSAC   )
	    ? "&w[&Y+AUTOSAC   &w]&W You automatically sacrifice corpses.\n\r"
	    : "&w[&Y-autosac   &w]&W You don't automatically sacrifice corpses.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_ANSI     )
	    ? "&w[&Y+ANSI      &w]&W You have ansi color enabled.\n\r"
	    : "&w[&Y-ansi      &w]&W You have ansi color disabled.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_BLANK     )
	    ? "&w[&Y+BLANK     &w]&W You have a blank line before your prompt.\n\r"
	    : "&w[&Y-blank     &w]&W You have no blank line before your prompt.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_BRIEF     )
	    ? "&w[&Y+BRIEF     &w]&W You see brief descriptions.\n\r"
	    : "&w[&Y-brief     &w]&W You see long descriptions.\n\r"
	    , ch );

	send_to_char(AT_RED, IS_SET( ch->act, PLR_FULLNAME  )
	    ? "&w[&Y+FULLNAME  &w]&W You have name completion off.\n\r"
	    : "&w[&Y-fullname  &w]&W You are using name completion.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_COMBINE   )
	    ? "&w[&Y+COMBINE   &w]&W You see object lists in combined format.\n\r"
	    : "&w[&Y-combine   &w]&W You see object lists in single format.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_PROMPT    )
	    ? "&w[&Y+PROMPT    &w]&W You have a prompt.\n\r"
	    : "&w[&Y-prompt    &w]&W You don't have a prompt.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_TELNET_GA )
	    ? "&w[&Y+TELNETGA  &w]&W You receive a telnet GA sequence.\n\r"
	    : "&w[&Y-telnetga  &w]&W You don't receive a telnet GA sequence.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_COMBAT )
	    ? "&w[&Y+COMBAT    &w]&W You see all combat scroll.\n\r"
	    : "&w[&Y-combat    &w]&W You do not see dodge/parry/miss in combat.\n\r"
	    , ch );

	send_to_char(AT_RED,  IS_SET( ch->act, PLR_SILENCE   )
	    ? "&w[&Y+SILENCE   &w]&W You are silenced.\n\r"
	    : ""
	    , ch );

	send_to_char(AT_RED, !IS_SET( ch->act, PLR_NO_EMOTE  )
	    ? ""
	    : "&w[&Y-emote    &w]&W You can't emote.\n\r"
	    , ch );

	send_to_char(AT_RED, !IS_SET( ch->act, PLR_NO_TELL   )
	    ? ""
	    : "&w[&Y-tell     &w]&W You can't use 'tell'.\n\r"
	    , ch );
    }
    else
    {
	char buf [ MAX_STRING_LENGTH ];
	int  bit;
	bool fSet;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char(AT_BLOOD, "&WConfig &Y-&Woption or &Y+&Woption?\n\r", ch );
	    return;
	}

             if ( !str_cmp( arg+1, "autoexit" ) ) bit = PLR_AUTOEXIT;
	else if ( !str_cmp( arg+1, "autoloot" ) ) bit = PLR_AUTOLOOT;
	else if ( !str_cmp( arg+1, "autocoins") ) bit = PLR_AUTOCOINS;
	else if ( !str_cmp( arg+1, "autosplit") ) bit = PLR_AUTOSPLIT;
	else if ( !str_cmp( arg+1, "autosac"  ) ) bit = PLR_AUTOSAC;
	else if ( !str_cmp( arg+1, "autosac"  ) ) bit = PLR_AUTOCOINS;
	else if ( !str_cmp( arg+1, "blank"    ) ) bit = PLR_BLANK;
	else if ( !str_cmp( arg+1, "brief"    ) ) bit = PLR_BRIEF;
	else if ( !str_cmp( arg+1, "fullname" ) ) bit = PLR_FULLNAME;
	else if ( !str_cmp( arg+1, "combine"  ) ) bit = PLR_COMBINE;
        else if ( !str_cmp( arg+1, "prompt"   ) ) bit = PLR_PROMPT;
	else if ( !str_cmp( arg+1, "telnetga" ) ) bit = PLR_TELNET_GA;
	else if ( !str_cmp( arg+1, "ansi"     ) ) bit = PLR_ANSI;
	else if ( !str_cmp( arg+1, "combat"   ) ) bit = PLR_COMBAT;
	else
	{
	    send_to_char(AT_BLOOD, "Config which option?\n\r", ch );
	    return;
	}

	if ( fSet )
	{
	    SET_BIT    ( ch->act, bit );
	    sprintf( buf, "%s is now &YON&W.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char(AT_WHITE, buf, ch );
	}
	else
	{
	    REMOVE_BIT ( ch->act, bit );
	    sprintf( buf, "%s is now &YOFF&W.\n\r", arg+1 );
	    buf[0] = UPPER( buf[0] );
	    send_to_char(AT_WHITE, buf, ch );
	}

    }

    return;
}

/*
do_immlist replaced this - Decklarean
void do_wizlist ( CHAR_DATA *ch, char *argument )
{

    do_help ( ch, "wizlist" );
    return;

}
*/
void do_spells ( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH ];
    int  sn;
    int  col;

    if ( IS_NPC( ch )
	|| ( !IS_NPC( ch ) && !has_spells( ch ) ) )
    {  
       send_to_char ( AT_BLUE, "You do not know how to cast spells!\n\r", ch );
       return;
    }

    col = 0;
    for ( sn = 0; skill_table[sn].name[0] != '\0'; sn++ )
    {
        if ( !skill_table[sn].name )
	   break;
	if ( !can_use_skpell( ch, sn ) )
	   continue;
	sprintf( buf, "%18s %3dpts ",
		 skill_table[sn].name,
		 ( ch->race == RACE_ELF || ch->race == RACE_ELDER )
		 ? (int)(SPELL_COST( ch, sn ) * .75) 
		 : SPELL_COST( ch, sn ) );
	send_to_char( AT_BLUE, buf, ch );
	if ( ++col % 3 == 0 )
	   send_to_char( AT_BLUE, "\n\r", ch );
    }

    if ( col % 3 != 0 )
      send_to_char( AT_BLUE, "\n\r", ch );

    return;

}

void do_slist ( CHAR_DATA *ch, char *argument )
{
    char buf  [ MAX_STRING_LENGTH ];
    int  sn;
    int  col;
    int  level;
    int  profession;
    int  iClass;
    bool pSpell;
    bool onlyCl = FALSE;
    int  classes = number_classes( ch );
    bool was_printed[ MAX_SKILL ];
    for ( iClass = 0; iClass < MAX_SKILL; iClass++ )
	was_printed[iClass] = FALSE;
    profession = prime_class( ch );
    for ( ;; )
      { char arg [MAX_STRING_LENGTH];
        argument = one_argument ( argument, arg );
        if ( arg[0] == '\0' ) 
          break;
        if (strlen(arg) < 3)
          {
            send_to_char(AT_GREEN, "That is not a class.\n\r", ch );
            return;
          }
        arg[3] = '\0';
        for (iClass = 0; iClass < MAX_CLASS; iClass++)
          {
            if (!str_cmp(arg,class_table[iClass].who_name))
              {
                 profession = iClass;
		 onlyCl = TRUE;
                 break;
              }
           }
         if (iClass == MAX_CLASS)
           {
              send_to_char(AT_GREEN, "That is not a class.\n\r", ch );
              return;
           }     
      }
    if ( IS_NPC( ch ) )
    {  
       send_to_char (AT_BLUE, "You do not need any stinking spells!\n\r", ch );
       return;
    }

    send_to_char( AT_BLUE, "&wLv          &CSpells/Skills\n\r\n\r", ch );

    for ( level = 1; level <= L_CHAMP3; level++ )
    {

      col = 0;
      pSpell = TRUE;

      for ( sn = 0; skill_table[sn].name[0] != '\0'; sn++ )
      {
	if ( !skill_table[sn].name )
	  break;
        if ( ( onlyCl || classes == 1 )
        && skill_table[sn].skill_level[profession] != level )
	  continue;

        if ( classes == 2
        && skill_table[sn].skill_level[profession] != level
        && skill_table[sn].skill_level[ch->class[1]] != level )
	  continue;

        if ( classes == 3
        && skill_table[sn].skill_level[profession] != level
        && skill_table[sn].skill_level[ch->class[1]] != level
        && skill_table[sn].skill_level[ch->class[2]] != level )
	  continue;

	if ( was_printed[sn] )
	  continue;	

	was_printed[sn] = TRUE;

	if ( pSpell )
	{
	  sprintf ( buf, "&w%2d:", level );
	  send_to_char( AT_BLUE, buf, ch );
	  pSpell = FALSE;
	}

	if ( ++col % 4 == 0 )
	  send_to_char( AT_BLUE, "   ", ch );

	sprintf ( buf, "&C%24s", skill_table[sn].name );
	send_to_char( AT_BLUE, buf, ch );
	if ( col % 3 == 0 )
	  send_to_char(AT_BLUE, "\n\r", ch );

      }

      if ( col % 3 != 0 )
	send_to_char( AT_BLUE, "\n\r", ch );

    }

    return;

}

/* bypassing the config command - Kahn */

void do_autoexit ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_AUTOEXIT )
     ? sprintf( buf, "-autoexit" )
     : sprintf( buf, "+autoexit" ) );

    do_config( ch, buf );

    return;

}

void do_autoloot ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_AUTOLOOT )
     ? sprintf( buf, "-autoloot" )
     : sprintf( buf, "+autoloot" ) );

    do_config( ch, buf );

    return;
}

void do_autosac ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_AUTOSAC )
     ? sprintf( buf, "-autosac" )
     : sprintf( buf, "+autosac" ) );

    do_config( ch, buf );

    return;

}

void do_blank ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_BLANK )
     ? sprintf( buf, "-blank" )
     : sprintf( buf, "+blank" ) );

    do_config( ch, buf );

    return;

}

void do_brief ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_BRIEF )
     ? sprintf( buf, "-brief" )
     : sprintf( buf, "+brief" ) ) ; 

    do_config( ch, buf );

    return;

}

void do_combine ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_COMBINE )
     ? sprintf( buf, "-combine" )
     : sprintf( buf, "+combine" ) );

    do_config( ch, buf );

    return;

}

void do_autosplit ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_AUTOSPLIT )
     ? sprintf( buf, "-autosplit" )
     : sprintf( buf, "+autosplit" ) );

    do_config( ch, buf );

    return;

}

void do_ansi ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_ANSI )
     ? sprintf( buf, "-ansi" )
     : sprintf( buf, "+ansi" ) );

    do_config( ch, buf );

    return;

}

void do_fullname ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_FULLNAME )
     ? sprintf( buf, "-fullname" )
     : sprintf( buf, "+fullname" ) );

    do_config( ch, buf );

    return;

}

void do_combat ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_COMBAT )
     ? sprintf( buf, "-combat" )
     : sprintf( buf, "+combat" ) );

    do_config( ch, buf );

    return;

}



void do_telnetga ( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    ( IS_SET ( ch->act, PLR_TELNET_GA )
     ? sprintf( buf, "-telnetga" )
     : sprintf( buf, "+telnetga" ) );

    do_config( ch, buf );

    return;

}

 
void do_pagelen ( CHAR_DATA *ch, char *argument )
{
    char buf [ MAX_STRING_LENGTH ];
    char arg [ MAX_INPUT_LENGTH  ];
    int  lines;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	lines = 20;
    else
	lines = atoi( arg );

    if ( lines < 1 )
    {
	send_to_char(C_DEFAULT,
		"Negative or Zero values for a page pause are not legal.\n\r",
		     ch );
	return;
    }

    if ( lines > 60 )
    {
	send_to_char(C_DEFAULT,
		"I don't know of a screen that is larger than 60 lines!\n\r",
		     ch );
	lines = 60;
    }

    ch->pcdata->pagelen = lines;
    sprintf( buf, "Page pause set to %d lines.\n\r", lines );
    send_to_char(C_DEFAULT, buf, ch );
    return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_prompt( CHAR_DATA *ch, char *argument )
{
   char buf [ MAX_STRING_LENGTH ];

   buf[0] = '\0';
   ch = ( ch->desc->original ? ch->desc->original : ch->desc->character );

   if ( argument[0] == '\0' )
   {
       ( IS_SET ( ch->act, PLR_PROMPT )
	? sprintf( buf, "-prompt" )
	: sprintf( buf, "+prompt" ) );

       do_config( ch, buf );

       return;
   }
   
   if( !strcmp( argument, "all" ) )
      strcat( buf, "<%hhp %mm %vmv> ");
   else
   {
      if ( strlen( argument ) > 50 )
	  argument[50] = '\0';
      smash_tilde( argument );
      if ( strlen( argument ) < 2 )
        sprintf( buf, "%s ", argument );
     else
      strcat( buf, argument );
   }

   free_string( ch->prompt );
   ch->prompt = str_dup( buf );
   send_to_char(C_DEFAULT, "Ok.\n\r", ch );
   return;
} 

void do_auto( CHAR_DATA *ch, char *argument )
{

    do_config( ch, "" );
    return;

}
void do_irongrip( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    
    if ( IS_NPC( ch ) || number_percent( ) > ch->pcdata->learned[gsn_grip] )
    {	
     send_to_char( AT_WHITE, "You failed.\n\r", ch );
     return;
    }
    if ( ch->position == POS_FIGHTING || is_affected( ch, gsn_grip ) )
	return;
    af.type      = gsn_grip;
    af.level     = ch->level;
    af.duration  = ch->level / 6;
    af.location  = APPLY_ANTI_DIS;
    af.modifier  = ch->level - ( ch->level / 4 );
    af.bitvector = 0;
    affect_to_char( ch, &af );
    send_to_char( AT_BLUE, "You grip your weapon tightly.\n\r", ch );

    update_skpell( ch, gsn_grip );

    return;
}

void do_induct( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA  *victim;
    char        arg [MAX_STRING_LENGTH];
    char const *clname;
    char        buf [MAX_STRING_LENGTH];
    CLAN_DATA  *pClan;
    
    buf[0] = '\0';
    one_argument( argument, arg );
    pClan=get_clan_index(ch->clan);

    if ( ( ch->clan == 0 )
        || ( ch->clev < 1 ) )
           return;
    if (  ((ch->clev == 1) && (!IS_SET(pClan->settings,CLAN_SECOND_INDUCT)))
        ||((ch->clev == 2) && (!IS_SET(pClan->settings,CLAN_FIRST_INDUCT) ))
        ||((ch->clev == 3) && (!IS_SET(pClan->settings,CLAN_LEADER_INDUCT)))
        ||((ch->clev == 4) && (!IS_SET(pClan->settings,CLAN_CHAMP_INDUCT) ))
       )
    {
     send_to_char( AT_WHITE, "You can't induct in your current clan position.", ch);
     return;
    }
    if ( ! ( victim = get_char_room( ch, arg ) ) )    
       {
        send_to_char( AT_WHITE, "No such person is in the room.\n\r", ch );
        return;
       }
    if IS_NPC(victim)
       return;
    if ( IS_SET ( victim->act, PLR_OUTCAST ) )
    {
     send_to_char( AT_WHITE, "That person is an outcast.\n\r", ch );
     return;
    }
    if ( victim->clan != 0 )
       return;
    if ( !pClan )
      return;
    pClan->members++;
    clname = pClan->name;
    sprintf( buf + strlen( buf ), "<%s>", clname ); 
    act(AT_RED, "$n has been inducted into $T.", victim, NULL, buf, TO_ROOM);
    act(AT_RED, "You are now a member of $T.", victim, NULL, buf, TO_CHAR);
    act(AT_RED, "You have inducted $N.", ch, NULL, victim, TO_CHAR);
    victim->clan = ch->clan;
    victim->clev = 0;
    return;
}

void do_outcast( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA  *victim;
    char        arg [MAX_STRING_LENGTH];
    char const *clname;
    char        buf [MAX_STRING_LENGTH];
    CLAN_DATA  *pClan;
    
    buf[0] = '\0';
    one_argument( argument, arg );
    if ( ( ch->clan == 0 )
        || ( ch->clev < 2 ) )
           return;
    if ( !( victim = get_char_room( ch, arg ) ) )
       {
        send_to_char( AT_WHITE, "No such person is in the room.\n\r", ch );
        return;
       }
    if IS_NPC(victim)
       return;
    if ( ( victim->clan == 0 ) || ( victim->clan != ch->clan ) )
       return;
    pClan=get_clan_index(ch->clan);
    if ( !pClan )
      return;
    pClan->members--;
    clname = pClan->name;
    sprintf ( buf + strlen( buf ), "<%s>", clname );
    act(AT_RED, "$n has been outcasted from $T.", victim, NULL, buf, TO_ROOM);
    act(AT_RED, "You are no longer a member of $T.", victim, NULL, buf, TO_CHAR);
    act(AT_RED, "You have outcasted $N.", ch, NULL, victim, TO_CHAR);
    victim->clan = 0;
    victim->clev = 0;
    if ( victim->cquestpnts > 0 )     /* delete player's Clan Quest points */
       victim->cquestpnts = 0;
    SET_BIT( victim->act, PLR_OUTCAST );
    return;
}

void do_setlev( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char       arg1 [ MAX_INPUT_LENGTH ];
    char       arg2 [ MAX_INPUT_LENGTH ];
    int        level;
    CLAN_DATA *pClan;
    char const *cltitle;

    cltitle = "NONE"; /* init */    

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ch->clev < 3 )
       return;
    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char(AT_WHITE, "Syntax: &Bsetlev <char> <level>.\n\r", ch );
        send_to_char(AT_WHITE, "Valid Levels are as follows\n\r", ch );
        send_to_char(AT_WHITE, "       0 -> Regular member.\n\r", ch );
        send_to_char(AT_WHITE, "       1 -> Centurion of Clan.\n\r", ch );
        send_to_char(AT_WHITE, "       2 -> Council of Clan.\n\r", ch );
        send_to_char(AT_WHITE, "       3 -> Leader of Clan.\n\r", ch );
        send_to_char(AT_WHITE, "       4 -> Clan Champion.\n\r", ch );
	return;
    }

    if ( !( victim = get_char_room( ch, arg1 ) ) )
    {
	send_to_char(AT_WHITE, "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC( victim ) )
    {
	send_to_char(AT_WHITE, "Not on NPC's.\n\r", ch );
	return;
    }
    if ( (ch->clan != victim->clan) || ( ch->clev < victim->clev ) )
       return;
       
    level = atoi( arg2 );
    if ( level > ch->clev )
      {
        send_to_char(AT_WHITE, "Not above your own level.\n\r", ch);
        return;
      }
    if ( level < 0 || level > 4 )
    {
	send_to_char(AT_WHITE, "Valid Levels are as follows\n\r", ch );
	send_to_char(AT_WHITE, "       0 -> Regular member.\n\r", ch );
	send_to_char(AT_WHITE, "       1 -> Centurion in command.\n\r", ch );
	send_to_char(AT_WHITE, "       2 -> Council in command.\n\r", ch );
	send_to_char(AT_WHITE, "       3 -> Leader of Clan.\n\r", ch );
	send_to_char(AT_WHITE, "       4 -> Clan Champion.\n\r", ch );
	return;
    }
    pClan=get_clan_index(ch->clan);
    /* Lower a player in the Clan */
    switch ( victim->clev )
    {
    case 0: break;
    case 1:
      pClan->issecond=FALSE;
      free_string( pClan->second );
      pClan->second = str_dup( "EMPTY" );
      break;
    case 2:
      pClan->isfirst=FALSE;
      free_string( pClan->first );
      pClan->first= str_dup( "EMPTY" );
      break;
    case 3:
      pClan->isleader=FALSE;
      free_string( pClan->leader );
      pClan->leader=str_dup( "EMPTY" );
      break;
    case 4:
      pClan->ischamp=FALSE;
      free_string( pClan->champ );
      pClan->champ = str_dup( "EMPTY" );
      break;
    default: break;
   }
     switch ( level )
     {
      default: break;
      case 0: break;
      case 1:
        if (pClan->issecond)
          {
            send_to_char(AT_WHITE, "Clan already has a Centurion, defaulting to regular member", ch );
            level = 0;
            break;
          }
        else
          {
            pClan->issecond=TRUE;
            pClan->second = str_dup( victim->name );
            break;
          }            
      case 2:
        if (pClan->isfirst)
          {
            send_to_char(AT_WHITE, "Clan already has a Council, defaulting to regular member", ch );
            level = 0;
            break;
          }
        else
          {
            pClan->isfirst=TRUE;
            pClan->first = str_dup( victim->name );
            break;
          }            
      case 3:
        if (pClan->isleader)
          {
            send_to_char(AT_WHITE, "Clan already has a Leader, defaulting to regular member", ch );
            level = 0;
            break;
          }
        else
          {
            pClan->isleader=TRUE;
            pClan->leader = str_dup( victim->name );
            break;
          }            
      case 4:
        if (pClan->ischamp)
          {
            send_to_char(AT_WHITE, "Clan already has a Champion, defaulting to regular member", ch );
            level = 0;
            break;
          }
        else
          {
            pClan->ischamp=TRUE;
            pClan->champ = str_dup( victim->name );
            break;
          }            
     }
    if ( level <= victim->clev )
    {
     char  buf [MAX_STRING_LENGTH];
     buf[0] = '\0';
     switch( level )
     {
      case 0 :  cltitle = "<"; break;
      case 1 :  cltitle = "<Centurion of"; break;
      case 2 :  cltitle = "<Council of"; break;
      case 3 :  cltitle = "<Leader of"; break;
      case 4 :  cltitle = "<Hero of"; break;
      default:  cltitle = "[bug rep to imm]"; break;
     }
     sprintf( buf + strlen(buf), "%s %s>", cltitle, pClan->name );
     act(AT_BLUE, "You have been lowered to $T.", victim, NULL, buf, TO_CHAR );
     act(AT_BLUE, "Lowering a players clan level.", ch, NULL, NULL, TO_CHAR );
     act(AT_BLUE, "$n is now $T", victim, NULL, buf, TO_ROOM );
     victim->clev = level;
     return;
    }
    else
    {
     char  buf [MAX_STRING_LENGTH];
     
     buf[0] = '\0';
     switch( level )
     {
      case 0 :  cltitle = "<"; break;
      case 1 :  cltitle = "<Centurion of"; break;
      case 2 :  cltitle = "<Council of"; break;
      case 3 :  cltitle = "<Leader of"; break;
      case 4 :  cltitle = "<Hero of"; break;
     }
     sprintf( buf + strlen(buf), "%s %s>", cltitle, pClan->name );
     act(AT_BLUE, "You have been raised to $T.", victim, NULL, buf, TO_CHAR );
     act(AT_BLUE, "Raising a players clan level.", ch, NULL, NULL, TO_CHAR );
     act(AT_BLUE, "$n is now $T", victim, NULL, buf, TO_ROOM );
     victim->clev = level;
     return;
    }
    return;
}

void do_smash ( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA  *obj;
    char       buf [MAX_STRING_LENGTH];
    char       arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char      *name;
    
    if ( ch->clan != 5 )
    {
      send_to_char(C_DEFAULT,"Huh?\n\r",ch);
      return;
    }
    if ( ch->ctimer )
    {
      send_to_char(AT_BLUE, "You failed.\n\r",ch);
      return;
    }
    buf[0]='\0';
    one_argument( argument, arg );
    if ( !(obj = get_obj_carry( ch, arg ) ) )
    {
     send_to_char(AT_WHITE, "You do not have that doll.\n\r", ch );
     return;
    }
    name = obj->name;
    if ( !(victim = get_char_world(ch, name) ) 
	|| victim->in_room->area != ch->in_room->area )
    {
     send_to_char( AT_WHITE, "That person's life cannot be sensed.\n\r", ch );
     return;
    }
    if ( ch->level - victim->level > 8 || victim->level - ch->level > 8 )
    {
      send_to_char(AT_BLUE, "The doll remains undamaged.\n\r",ch);
      return;
    }
    act(AT_RED, "You call down the Dark forces of Retribution on $N.", ch, NULL, victim, TO_CHAR);
    act( AT_RED, "$n smashes $p.", ch, obj, NULL, TO_ROOM );
    if ( !victim->wait )
      act( AT_RED, "You feel a wave of nausia come over you.", victim, NULL, NULL, TO_CHAR );
    extract_obj(obj);
    ch->ctimer = 5;
    if ( victim->wait )
      return;
    STUN_CHAR(victim, 10, STUN_TOTAL);
    victim->position = POS_STUNNED;
    update_pos( victim );
    return;
}

void do_guild(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;

  if(argument[0] == '\0')
  {
    send_to_char(AT_WHITE, "Guild <char>\n\r", ch);
    return;
  }
  if(ch->guild == NULL)
  {
    send_to_char(AT_BLUE, "You must be in a guild to induct someone.\n\r", ch);
    return;
  }
  if(!is_name(NULL, ch->name, ch->guild->deity))
  {
    send_to_char(AT_BLUE, "You are not the deity of this guild.\n\r", ch);
    return;
  }
  if((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char(AT_BLUE, "Player not found.\n\r", ch);
    return;
  }
  if(IS_NPC(victim))
  {
    send_to_char(AT_BLUE, "May guild PCs only.\n\r", ch);
    return;
  }
  if(victim->guild != NULL)
  {
    send_to_char(AT_BLUE, "That person is already guilded.\n\r", ch);
  }
  if ( IS_SET( victim->act, PLR_THIEF ) 
  && !strcmp( ch->guild->name, "MERCENARY" ) )
    {
    send_to_char(AT_BLUE, "That person has a thief flag!\n\r", ch );
    return;
    }
  victim->guild = ch->guild;
  victim->guild_rank = 0;
  act(AT_BLUE, "You guild $N.", ch, NULL, victim, TO_CHAR);
  act(AT_BLUE, "$N guilds you.", victim, NULL, ch, TO_CHAR);
  return;
}

void do_unguild(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;

  if(argument[0] == '\0')
  {
    send_to_char(AT_WHITE, "Unguild <char>\n\r", ch);
    return;
  }
  if(ch->guild == NULL)
  {
    send_to_char(AT_BLUE, "You must be in a guild to unguild someone.\n\r", ch);
    return;
  }
  if(!is_name(NULL, ch->name, ch->guild->deity))
  {
    send_to_char(AT_BLUE, "You are not the deity of this guild.\n\r", ch);
    return;
  }
  if((victim = get_char_world(ch, argument)) == NULL)
  {
    send_to_char(AT_BLUE, "Player not found.\n\r", ch);
    return;
  }
  if(victim->guild != ch->guild)
  {
    send_to_char(AT_BLUE, "That person is not in your guild.\n\r", ch);
    return;
  }
  victim->guild = NULL;
  act(AT_BLUE, "You unguild $N.", ch, NULL, victim, TO_CHAR);
  act(AT_BLUE, "$N unguilds you.", victim, NULL, ch, TO_VICT);
  return;
}

void do_setrank(CHAR_DATA *ch, char *argument)
{
  char	arg1 [MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int value;

  argument = one_argument(argument, arg1);
  value = is_number(argument) ? atoi(argument) : 0;

  if(arg1[0] == '\0')
  {
    send_to_char(AT_WHITE, "Setrank <char> <rank num>\n\r", ch);
    send_to_char(AT_WHITE, " rank num > 0            \n\r", ch);
    return;
  }
  if(ch->guild == NULL)
  {
    send_to_char(AT_BLUE, "You must be in a guild to induct someone.\n\r", ch);
    return;
  }
  if(!is_name(NULL, ch->name, ch->guild->deity))
  {
    send_to_char(AT_BLUE, "You are not the deity of this guild.\n\r", ch);
    return;
  }
  if((victim = get_char_world(ch, arg1)) == NULL)
  {
    send_to_char(AT_BLUE, "Player not found.\n\r", ch);
    return;
  }
  if(victim->guild != ch->guild)
  {
    send_to_char(AT_BLUE, "That person is not in your guild.\n\r", ch);
    return;
  }
  victim->guild_rank = value;
  send_to_char(AT_BLUE, "Ok.\n\r", ch);
  return;
}

void do_guilds( CHAR_DATA *ch, char *argument )
{
    char          buf    [ MAX_STRING_LENGTH ];
    char          result [ MAX_STRING_LENGTH*2 ];
    int cnt = 0;
    sprintf( result, "&z[&R%12s&z] [&R%20s&z]\n\r", "Guild Name","Deities" );

    for ( cnt = 0; guild_table[cnt].name[0] != '\0'; cnt++ )
    {
        sprintf( buf, "&z[&W%*s&z] [&W%*s&z]\n\r", 
                 12 + strlen(guild_table[cnt].name) - strlen_wo_col(guild_table[cnt].name), 
                 guild_table[cnt].name,
                 20 + strlen(guild_table[cnt].deity) - strlen_wo_col(guild_table[cnt].deity),        
                 guild_table[cnt].deity );
        strcat( result, buf );
    }
  
    send_to_char( AT_WHITE, result, ch );
    return;

}
    
void do_racelist( CHAR_DATA *ch, char *argument )
{

    RACE_DATA *pRace;
    char buf[MAX_STRING_LENGTH];


    for ( pRace  = first_race;
          pRace;
          pRace  = pRace->next )
    {
  sprintf( buf,
"&z[&W%2d&z][&W%20s&z][&W%3s&z][&W%2d&z][&W%2d&z][&W%2d&z][&W%2d&z][&W%2d&z]\n\r",
	              pRace->vnum, pRace->race_full, pRace->race_name,
                 pRace->mstr, pRace->mint, pRace->mwis, pRace->mdex,
pRace->mcon );
  send_to_char( C_DEFAULT, buf, ch );

    }

}


void do_clans( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA    *pClan;
    char          buf[MAX_STRING_LENGTH];
    char result [ MAX_STRING_LENGTH*2 ];	/* May need tweaking. */

    if ( clan_first == NULL )
      return;

    sprintf( result, 
"&z[&W%3s&z] [&W%18s&z] [&W%12s&z] [&W%7s&z] [&W%6s&z] [&W%7s&z] [&W%6s&z]\n\r",
       "Num", "Clan Name", "Deity", "Members", "Pkills", "Pkilled", "Mkills"  );

    for ( pClan = clan_first->next; pClan; pClan = pClan->next )
    {
	sprintf( buf, 
"&z[&W%3d&z] [&W%*s&z] [&W%*s&z] [&W%7d&z] [&r%6d&z] [&W%7d&z] [&R%6d&z]\n\r",
	     pClan->vnum,
	     18 + strlen(pClan->name) - strlen_wo_col(pClan->name),
	     pClan->name,
	     12 + strlen(pClan->diety) - strlen_wo_col(pClan->diety),
	     pClan->diety,
	     pClan->members,
	     pClan->pkills,
	     pClan->pdeaths,
	     pClan->mkills );
	     strcat( result, buf );
    }

    send_to_char(AT_WHITE, result, ch );
    return;
}

void do_cinfo( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA    *pClan;
    char          buf[MAX_STRING_LENGTH];
    int           num;
    char          arg1[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    PLAYERLIST_DATA *pPlayer;

    argument = one_argument(argument, arg1);
    
    if (arg1[0] == '\0')
    {
      send_to_char(AT_WHITE, "Syntax:  &Rcinfo <clan number>\n\r", ch );
      send_to_char(AT_WHITE, "Syntax:  &Rcinfo <clan name>\n\r", ch );
      send_to_char(AT_WHITE, "Use the command clans to find a clan name or number.\n\r", ch );
      return;
    }  

    num = is_number( arg1 ) ? atoi( arg1 ) : -1; 
    
    for ( pClan = clan_first->next; pClan; pClan = pClan->next )
    {
        if ( !str_cmp( arg1, strip_color( pClan->name ) ) )
        {
         num = pClan->vnum;
         break;
        }
    }

    if ( num == -1 )
    {
     send_to_char( AT_WHITE, "No such clan exists, please try again.\n\r", ch );
     send_to_char( AT_WHITE, "Type clans to see available clan names.\n\r", ch );
     return;
    }

    if (!(pClan = get_clan_index(num)))
    {
     send_to_char( AT_WHITE, "Illegal clan number, please try again.\n\r", ch );
     send_to_char( AT_WHITE, "Type clans to see available clan numbers.\n\r", ch );
     return;
    }
    
    sprintf( buf, 
"&z------------------&WInformation on &w<&R%s&w>&z-----------------\n\r\n\r", pClan->name );
    send_to_char(AT_WHITE, buf, ch );
    sprintf( buf, "&cDeity&w:       &z[&W%12s&z]\n\r", pClan->diety );
    send_to_char(AT_WHITE, buf, ch );
    sprintf( buf, "&cChampion&w:    &z[&W%12s&z]&c%s&R%s\n\r",
             pClan->champ,
             IS_SET(pClan->settings, CLAN_LEADER_INDUCT) ? " Can induct." : "",
	     pClan->ischamp ? "" : " Position open." );
    send_to_char( AT_WHITE, buf, ch );
    sprintf( buf, "&cLeader&w:      &z[&W%12s&z]&c%s&R%s\n\r", 
             pClan->leader,
             IS_SET(pClan->settings, CLAN_LEADER_INDUCT) ? " Can induct." : "",
	 pClan->isleader ? "" : " Position open." );
    send_to_char( AT_WHITE, buf, ch );
    sprintf( buf, "&cCouncil&w:     &z[&W%12s&z]&c%s&R%s\n\r", 
             pClan->first,
             IS_SET(pClan->settings, CLAN_LEADER_INDUCT) ? " Can induct." : "",
	     pClan->isfirst ? "" : " Position open." );
    send_to_char( AT_WHITE, buf, ch );
    sprintf( buf, "&cCenturion&w:   &z[&W%12s&z]&c%s&R%s\n\r", 
             pClan->second, 
             IS_SET(pClan->settings, CLAN_LEADER_INDUCT) ? " Can induct." : "",
	     pClan->issecond ? "" : " Position open." );
    send_to_char( AT_WHITE, buf, ch );
    sprintf( buf, "&cMembers&w:     &z[&R%12d&z]\n\r", pClan->members );
    send_to_char(AT_WHITE, buf, ch );
    sprintf( buf, "&cCivil Pkill&w: &z[%12s&z]\n\r",
             IS_SET(pClan->settings, CLAN_CIVIL_PKILL) ? "&rYES" : "&CNO" );
    send_to_char(AT_WHITE, buf, ch );
    if IS_SET(pClan->settings, CLAN_PKILL)
    {
     sprintf( buf, "&cPkill&w:       &z[         &rYES&z]\n\r" );
     send_to_char(AT_RED, buf, ch );
     sprintf( buf, "&cPkills&w:      &z[&R%12d&z]\n\r", pClan->pkills );
     send_to_char(AT_WHITE, buf, ch );
     sprintf( buf, "&cPkilled&w:     &z[&R%12d&z]\n\r", pClan->pdeaths );
     send_to_char(AT_WHITE, buf, ch );
     sprintf( buf, "&cMkills&w:      &z[&R%12d&z]\n\r", pClan->mkills );
     send_to_char(AT_WHITE, buf, ch );
     sprintf( buf, "&cEquipment&w:\n\r" );
     send_to_char(AT_WHITE, buf, ch );
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_1 )))
      sprintf( buf, "&cLevel &W50&w: &W%s\n\r", pObjIndex->short_descr ); 
     else
      sprintf( buf, "&cLevel &W50&w:\n\r" ); 
     send_to_char(AT_WHITE, buf, ch );
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_2 )))
      sprintf( buf, "&cLevel &W75&w: &W%s\n\r", pObjIndex->short_descr );
     else
      sprintf( buf, "&cLevel &W75&w:\n\r" );
     send_to_char(AT_WHITE, buf, ch );
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_3 )))
      sprintf( buf, "&cLevel &W100&w: &W%s\n\r", pObjIndex->short_descr );
     else
      sprintf( buf, "&cLevel &W100&w:\n\r" );
     send_to_char(AT_WHITE, buf, ch );
    }
    else
    {
     sprintf( buf, "&cPkill&w:       &z[          &CNO&z]\n\r" );
     send_to_char( AT_LBLUE, buf, ch );
     sprintf( buf, "&cMkills&w:      &z[&R%12d&z]\n\r", pClan->mkills );
     send_to_char(AT_WHITE, buf, ch );
     sprintf( buf, "&cEquipment&w:\n\r" );
     send_to_char(AT_WHITE, buf, ch );
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_1 )))
      sprintf( buf, "&cLevel &W30&w: &W%s\n\r", pObjIndex->short_descr );
     else
      sprintf( buf, "&cLevel &W30&w:\n\r" );
     send_to_char(AT_WHITE, buf, ch );
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_2 )))
      sprintf( buf, "&cLevel &W65&w: &W%s\n\r", pObjIndex->short_descr );
     else
      sprintf( buf, "&cLevel &W65&w:\n\r" ); 
     send_to_char(AT_WHITE, buf, ch );
     if ((pObjIndex = get_obj_index ( pClan->obj_vnum_3 )))
      sprintf( buf, "&cLevel &W100&w: &W%s\n\r", pObjIndex->short_descr );
     else
      sprintf( buf, "&cLevel &W100&w:\n\r" );
     send_to_char(AT_WHITE, buf, ch );
    }   
    sprintf( buf, "&cDescription&w:\n\r&W%s", pClan->description );
    send_to_char( AT_WHITE, buf, ch );

    send_to_char( C_DEFAULT, "&cCurrent Members:\n\r", ch );

    for ( pPlayer = playerlist; pPlayer; pPlayer = pPlayer->next )
    {
       if ( pPlayer->clan_name )
       if ( !str_cmp(pPlayer->clan_name, pClan->name )  )
       {
          sprintf( buf, "%s\n\r", pPlayer->name );
          send_to_char( AT_WHITE, buf, ch );
       }
    }

    return;
}  
    
void do_questflag( CHAR_DATA *ch, char *argument )
{
  if (IS_NPC( ch ) )
  {
     send_to_char( AT_WHITE, "Only PCs may do this.\n\r", ch );
     return;
  }
  
  if (quest)
  {
    if ( ch->level < qmin )
    {
      send_to_char( AT_WHITE, "You are too low of level for this quest.\n\r", ch );
      return;
    }
    if ( ch->level > qmax )
    {
      send_to_char( AT_WHITE, "You are too high of level for this quest.\n\r", ch );
      return;
    }
  }
  
  if (!(quest) && !IS_SET( ch->act, PLR_QUESTOR ) )
  {
     send_to_char( AT_WHITE, "There is currently no quest.\n\r", ch );
     return;
  }
  
  if ( IS_SET( ch->act, PLR_QUEST ) && (quest) )
  {
     send_to_char( AT_WHITE, "You must wait for an immortal to remove you from the quest.\n\r", ch );
     return;
  }
  else if ( IS_SET( ch->act, PLR_QUEST ) )
  {
     REMOVE_BIT( ch->act, PLR_QUEST );
     send_to_char( AT_WHITE, "Ok, you are no longer questing.\n\r", ch );
     return;
  }
 else
  {
     SET_BIT( ch->act, PLR_QUEST );
     send_to_char( AT_WHITE, "Ok, you are now questing.\n\r", ch );
     return;
  }
  return;
}

void do_autocoins( CHAR_DATA *ch, char *argument )
/*void do_autogold( CHAR_DATA *ch, char *argument ) */
{
   if ( IS_NPC( ch ) )
   	return;

   if ( IS_SET( ch->act, PLR_AUTOCOINS ) )
   { 
     do_config( ch, "-autocoins" );
     return;
   }
   if ( !IS_SET( ch->act, PLR_AUTOCOINS ) )
   {
     do_config( ch, "+autocoins" );
     return;
   }
   return;
}

/*
void do_vsi( CHAR_DATA *ch )
{
  char limbm[MAX_STRING_LENGTH];
  int limb;
  for ( limb = 1; limb <= 2; limb++ )
  {
  switch( ch->arm[limb]->hp )
  {
     case 1:
       sprintf( limbm, "&RYour fingers from your %s hand are missing, and they are %s\n\r",
        ( limb == 1 ) ? "left" : "right", bleedinglev( ch->arm[limb]->bl );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     case 2:
       sprintf( limbm, "&RYour %s hand is missing, and it is %s\n\r", 
        ( limb == 1 ) ? "left" : "right", bleedinglev( ch->arm[limb]->bl );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     case 3:
       sprintf( limbm, "&RYour %s arm is missing past the elbow, and it is %s\n\r", 
        ( limb == 1 ) ? "left" : "right", bleedinglev( ch->arm[limb]->bl );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     case 4:
       sprintf( limbm, "&RYou &B*&RWHOLE&B*&R %s arm is missing!, and it is %s\n\r",
        ( limb == 1 ) ? "left" : "right", bleedinglev( ch->arm[limb]->bl ); 
       send_to_char( AT_WHITE, limbm, ch );
       break;
     default:
       break;
   }
   }
   
   switch( ch->neck->hp )
   {
     case 1:
       sprintf( limbm, "&RYour neck is lightly hurt, and it is %s\n\r",
        bleedinglev( ch->neck->bl ) );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     case 2:
       sprintf( limbm, "&RYour neck is severely hurt, and it is %s\n\r",
        bleedinglev( ch->neck->bl ) );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     case 3:
       sprintf( limbm, "&RYour neck is broken!, and it is %s\n\r",
         bleedinglev( ch->neck->bl ) );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     case 4:
       sprintf( limbm, "&RYour head is missing!, and your neck is %s\n\r",
        bleedinglev( ch->neck->bl ) );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     default:
       break;
   }

   switch( ch->eyes->hp )
   {
     case 1:
       sprintf( limbm, "&ROne of your eyes has been gouged out, and it is %s\n\r",
        bleedinglev( ch->eyes->bl ) );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     case 2:
       sprintf( limbm, "&RBoth of your eyes have been gouged out!, they are %s\n\r",
        bleedinglev( ch->eyes->bl ) );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     default:
       break;
   }
   
   switch( ch->torso->hp )
   {
     case 1:
       sprintf( limbm, "&RYour torso is lightly wounded, and it is %s\n\r",
         bleedinglev( ch->torso->bl ) );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     case 2:
       sprintf( limbm, "&RYour torso is badly wounded, and it is %s\n\r",
         bleedinglev( ch->torso->bl ) );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     case 3:
       sprintf( limbm, "&RYour torso is severely wounded!, and it is %s\n\r",
         bleedinglev( ch->torso->bl ) );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     case 4:
       sprintf( limbm, "&RYour bodyis missing halfway through your torso and down!,and it is %s\n\r",
        bleedintlev( ch->torso->bl ) );
       send_to_char( AT_WHITE, limbm, ch );
       break;
     default:
       break;
   }
   
   for ( limb = 1; limb <= 2; limb++ )
   {
     switch( ch->leg[limb]->hp )
     {
       case 1:
         sprintf( limbm, "&RYour %s foot is missing, and it is %s\n\r",
          ( limb == 1 ) ? "left" : "right", bleedinglev( ch->leg[limb]->bl ) );
         send_to_char( AT_WHITE, limbm, ch );
         break;
       case 2:
         sprintf( limbm, "&RYour %s leg is missing from the knee down, and it is %s\n\r",
          (limb == 1 ) ? "left" : "right", bleedinglev( ch->leg[limb]->bl ) );
         send_to_char( AT_WHITE, limbm, ch );
         break;
       case 3:
         sprintf( limbm, "&RYour whole %s leg is missing!, and it is %s\n\r",
          ( limb == 1 ) ? "left" : "right", bleedinglev( ch->leg[limb]->bl ) );
         send_to_char( AT_WHITE, limbm, ch );
         break;
       default:
         break;
     }
   }
 return;
 }
 */

/*
char *bleedinglev( int blvl )
{
  switch( blvl )
  {
    case 0:
      return "not bleeding.";
    case 1:
      return "bleeding lightly.";
    case 2:
      return "bleeding a lot.";
    case 3: 
      return "bleeding severely!";
    default:
      return "";
  }
  return "";
}
*/

void do_farsight( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *victim;
    char             target_name[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *from_room;
    
    if (IS_NPC(ch))
      return;
    if (ch->clan != 7 )
    {
      send_to_char(C_DEFAULT, "Huh?\n\r", ch );
      return;
    }
    one_argument( argument, target_name );
    
    if ( !( victim = get_char_world( ch, target_name ) )
	|| IS_SET( victim->in_room->room_flags, ROOM_PRIVATE   )
	|| IS_SET( victim->in_room->room_flags, ROOM_SOLITARY  )
	|| IS_SET( victim->in_room->area->area_flags, AREA_PROTOTYPE )
	|| IS_AFFECTED( victim, AFF_NOASTRAL ) )
    {
	send_to_char(AT_BLUE, "You failed.\n\r", ch );
	return;
    }
    from_room = ch->in_room;
    if ( ch != victim )
    {
     char_from_room( ch );
     char_to_room( ch, victim->in_room );
    }
    do_look( ch, "auto" );
    if ( ch != victim )
    {
     char_from_room( ch );
     char_to_room( ch, from_room );
    }
    return;
}

void do_worth( CHAR_DATA *ch, char *argument )
{
  if ( IS_NPC( ch ) )
    return;
  if ( ch->level < L_CHAMP3 )
  {
  sprintf( log_buf, "You have scored a total of %dxp, and are %dxp off from your"
     " next level.\n\r", ch->exp, xp_tolvl( ch ) - ch->exp );
  send_to_char( AT_RED, log_buf, ch );
  }
 else
  {
      send_to_char( AT_RED, "You perceive that you can level no higher by natural means.\n\r", ch );
  }
  sprintf( log_buf, "Your current stats are:\n\r  Str: %d.  Wis: %d.  Int: %d.  Dex: %d.  Con: %d.  Cha: %d.\n\r",
    get_curr_str( ch ), get_curr_wis( ch ), get_curr_int( ch ),
    get_curr_dex( ch ), get_curr_con( ch ), ch->charisma );
  send_to_char( AT_RED, log_buf, ch );

#ifdef NEW_MONEY
  sprintf( log_buf, "You are carrying %d gold, %d silver, and %d copper coins.\n\r", ch->money.gold,
	   ch->money.silver, ch->money.copper );
#else 
  sprintf( log_buf, "You are currently carrying %d coins.\n\r", ch->gold );
#endif

  send_to_char( AT_RED, log_buf, ch );
  return;
}

/*
 * Finger routine written by Dman.
 */
void do_finger( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char      const *class;
  char      const *race;
  
  one_argument( argument, arg );
/*
  if (get_trust(ch) < 215)
  {
   send_to_char(C_DEFAULT,"Finger is temporarily disabled.\n\r",ch);
   return;
  }
*/
  if ( arg[0] == '\0' )
    {
      send_to_char( AT_WHITE, "Finger whom?\n\r", ch );
      return;
    }
          
   if ( !(victim = get_char_world(ch, arg)))
    {
      read_finger( ch, argument );
      return;
    }
                               
  if ( !can_see( ch, victim ) )
    {
      send_to_char( AT_WHITE, "They aren't here.\n\r", ch );
      return;
    }
    
  if  ( IS_NPC( victim ) )
    {
      send_to_char( AT_WHITE, "Not on NPC's.\n\r", ch );
      return;
    } 



  sprintf( buf, "          Finger Info\n\r" );
  send_to_char( AT_WHITE, buf, ch );
                                           
  sprintf( buf, "          ------ ----\n\r\n\r" );
  send_to_char( AT_WHITE, buf, ch );
  
  sprintf( buf, "&CName: &W%-12s\n\r", victim->name);
  send_to_char( AT_WHITE, buf, ch );

  sprintf( buf, "&CMud Age: &W%2d           &CClan: &W%s\n\r",
     get_age( victim ), get_clan_index( victim->clan )->name );
  send_to_char( AT_WHITE, buf, ch );
 
  sprintf( buf, "&CLevel: &W%2d             &CSex: &W%s\n\r",
                victim->level,
                victim->sex == SEX_MALE   ? "male"   :
                victim->sex == SEX_FEMALE ? "female" : "neutral" );
  send_to_char( AT_WHITE, buf, ch );              
                     
  class = class_short( victim );
  race = (get_race_data(victim->race))->race_name;
  sprintf( buf, "&CClass: &W%-10s     &CRace: &W%s\n\r", class,race );
  send_to_char( AT_WHITE, buf, ch );
  sprintf( buf, "&CThief: &W%s\n\r", IS_SET( victim->act, PLR_THIEF ) ? "Yes"
								      : "No" );
  if ( is_class( victim, CLASS_THIEF ) )
	  send_to_char( AT_WHITE, buf, ch );
  sprintf( buf, "&CTitle: &W%s\n\r", 
                victim->pcdata->title );
  send_to_char( AT_WHITE, buf, ch );
                             
  sprintf( buf, "&CEmail: &W%s\n\r", victim->pcdata->email );
  send_to_char( AT_WHITE, buf, ch );
  sprintf( buf, "&CPlan: &W%s.\n\r", victim->pcdata->plan );
  send_to_char( AT_WHITE, buf, ch );
  sprintf( buf, "&CPkills: &W%-15d  &CPkilled: &W%d.\n\r", victim->pkills, victim->pkilled );
  send_to_char( AT_WHITE, buf, ch );
  sprintf( buf, "&CArena Wins: &W%-11d  &CArena Losses: &W%d.\n\r", victim->arenawon, victim->arenalost );
  send_to_char( AT_WHITE, buf, ch );
  sprintf( buf, "&CWar Kills: &W%-12d  &CWar Deaths: &W%-11d &CWar Points &W%d.\n\r",
				victim->warkills, victim->wardeaths, victim->warpts );
  send_to_char( AT_WHITE, buf, ch );
  sprintf( buf, "&CLast on: &W%s\n\r", (char *) ctime( &ch->logon ) );
  send_to_char( AT_WHITE, buf, ch );
  return;
}
      

void do_email( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char         buf  [ MAX_STRING_LENGTH ];

    rch = get_char( ch );


    if ( IS_NPC( ch ) )
     return;
     
    if (argument[0] == '\0')
    {
     sprintf(buf,"Email:%s \n\r", ch->pcdata->email);
     send_to_char( AT_WHITE, buf, ch );
     return;
    }
    
    if ( !IS_NPC( ch ) )
    {
        if ( longstring( ch, argument ) )
	    return;

	smash_tilde( argument );
	free_string( ch->pcdata->email );
	ch->pcdata->email = str_dup( argument );
        sprintf(buf,"Email now:%s \n\r",argument);
        send_to_char(AT_WHITE, buf, ch);
    }
    return;
}

void do_plan( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char         buf  [ MAX_STRING_LENGTH ];

    rch = get_char( ch );


    if ( IS_NPC( ch ) )
     return;
     
    if (argument[0] == '\0')
    {
     sprintf(buf,"Plan:%s \n\r", ch->pcdata->plan);
     send_to_char(AT_WHITE, buf, ch);
     return;
    }
    
    if ( !IS_NPC( ch ) )
    {
        if ( longstring( ch, argument ) )
	    return;

	smash_tilde( argument );
	free_string( ch->pcdata->plan );
	ch->pcdata->plan = str_dup( argument );
        sprintf(buf,"Plan now:%s \n\r",argument);
        send_to_char(AT_WHITE, buf, ch);
    }
    return;
}

void do_afkmes( CHAR_DATA *ch, char *argument )
{
   char buf [ MAX_STRING_LENGTH ];

   if ( !IS_NPC( ch ) )
   {
      if ( argument[0] == '\0' )
      {
        send_to_char( AT_WHITE, "Syntax:\n\r", ch );
        send_to_char( AT_YELLOW, "afkmes <string>\n\r", ch );
        sprintf( buf, "&YAFK Message: &W%s\n\r", ch->pcdata->afkchar );
        send_to_char( AT_WHITE, buf, ch );
        return;
      }

      if ( longstring( ch, argument ) )
        return;
    
      smash_tilde( argument );
      free_string( ch->pcdata->afkchar );
      ch->pcdata->afkchar = str_dup( argument );
      sprintf( buf, "&YAFK Message: &W%s\n\r", argument );
      send_to_char( AT_WHITE, buf, ch );
   } 
   return;
} 

#ifdef NEW_MONEY

/* Money functions, for new money format gold/silver/copper --Angi */

MONEY_DATA *add_money( MONEY_DATA *a,  MONEY_DATA *b )
{
  a->gold   += b->gold;
  a->silver += b->silver;
  a->copper += b->copper;

  return a;
}

MONEY_DATA *sub_money( MONEY_DATA *a, MONEY_DATA *b )
{
  a->gold   -= b->gold;
  a->silver -= b->silver;
  a->copper -= b->copper;

  return a;
}
MONEY_DATA *spend_money( MONEY_DATA *a, MONEY_DATA *b )
{

/* *a is how much money ch has, and *b is the cost of the item.
   Char can already afford, check done before, so just subtract. */

  int tmp_gold, tmp_silver, tmp_copper;

  tmp_gold = b->gold;
  tmp_silver = b->silver;
  tmp_copper = b->copper;

  if ( a->gold <= b->gold )
  {
    b->gold -= a->gold;
    a->gold = 0;
  }
  else
  {
    a->gold -= b->gold;
    b->gold = 0;
  }
  
  if ( a->silver <= b->silver )
  {
    b->silver -= a->silver;
    a->silver = 0;
  }
  else
  {
    a->silver -= b->silver;
    b->silver = 0;
  }

  if ( a->copper <= b->copper )
  {
    b->copper -= a->copper;
    a->copper = 0;
  }
  else
  {
    a->copper -= b->copper;
    b->copper = 0;
  }

  if ( b->gold > 0 )
  {
      if ( a->silver * S_PER_G <= b->gold * C_PER_G )
/*    if ( ( a->silver / SILVER_PER_GOLD ) <= b->gold )*/
    {
      b->gold -= ( a->silver / SILVER_PER_GOLD );
      a->silver %= SILVER_PER_GOLD;
    }
    else
    {
      a->silver -= ( b->gold * SILVER_PER_GOLD );
      b->gold = 0;
    }	
    if ( b->gold > 0 )
    {
        if ( a->copper <= b->gold * C_PER_G )
/*      if ( ( a->copper / COPPER_PER_GOLD ) <= b->gold ) */
      {
	b->gold -= ( a->copper / COPPER_PER_GOLD );
	a->copper %= COPPER_PER_GOLD;
      }
      else
      {
        a->copper -= ( b->gold * COPPER_PER_GOLD );
	b->gold =0;
      }
    }
  }
/* if b->gold != 0 now, then bug( etc.. ) */

  if ( b->silver > 0 )
  {
    if ( ( a->gold * SILVER_PER_GOLD ) <= b->silver )
    {
      b->silver -= ( a->gold * SILVER_PER_GOLD );
      a->gold = 0;
    }
    else
    {
      a->gold -= ( b->silver / SILVER_PER_GOLD ); 
      b->silver %= SILVER_PER_GOLD;
    }
    if ( b->silver > 0 )
    {
        if ( a->copper <= b->silver * S_PER_G )
/*      if ( ( a->copper / COPPER_PER_SILVER ) <= b->silver ) */
      {
        b->silver -= ( a->copper / COPPER_PER_SILVER );
        a->copper %= COPPER_PER_SILVER;
      }
      else
      {
        a->copper -= ( b->silver * COPPER_PER_SILVER );
        b->silver =0;
      }
    }
  }

  if ( b->copper > 0 )
  {
    if ( ( a->silver * COPPER_PER_SILVER ) <= b->copper )
    {
      b->copper -= ( a->silver * COPPER_PER_SILVER );
      a->silver = 0;
    }
    else
    {
      a->silver -= ( b->copper / COPPER_PER_SILVER );
      b->copper %= COPPER_PER_SILVER;
    }
    if ( b->copper > 0 )
    {
      if ( ( a->gold * COPPER_PER_GOLD ) <= b->copper )
      {
        b->copper -= ( a->gold * COPPER_PER_GOLD );
        a->gold = 0;
      }
      else
      {
        a->gold -= ( b->copper / COPPER_PER_GOLD );
        b->copper %= COPPER_PER_GOLD;
      }
    }
  }

  b->gold = tmp_gold;
  b->silver = tmp_silver;
  b->copper = tmp_copper;

  return a;

}

MONEY_DATA *take_money( CHAR_DATA *ch, int amt, char *type, char *verb )
{
  static MONEY_DATA  new_money;

  new_money.gold = new_money.silver = new_money.copper = 0;

  if ( amt <= 0 )
  {
    send_to_char( AT_WHITE, "Sorry, you can't do that.\n\r", ch );
    return NULL;
  }
  
  if ( !str_cmp( type, "gold" ) )
  {
    if ( ch->money.gold < amt )  
       return NULL;
    else
       new_money.gold = amt;
  }
  else
  if ( !str_cmp( type, "silver" ) )
  {
    if ( ch->money.silver < amt )
       return NULL;
    else
       new_money.silver = amt;
  }
  else
  if ( !str_cmp( type, "copper" ) )
  {
    if ( ch->money.copper < amt )
       return NULL;
    else
       new_money.copper = amt;
  }
  else
  {
    send_to_char( AT_WHITE, "There is no such kind of coin.\n\r", ch );
    return NULL;
  }
  
  sub_money( &ch->money, &new_money );

  return &new_money;
}

char *money_string( MONEY_DATA *money )
{
static  char buf  [ MAX_STRING_LENGTH ];
  char *bptr = buf;
  bool gold;
  bool silver;
  bool copper;

  gold   = ( money->gold   > 0 );
  silver = ( money->silver > 0 );
  copper = ( money->copper > 0 );

  if ( gold )
  {
    bptr += sprintf( bptr, "%d gold", money->gold );
    if ( silver != copper )
    {
      strcpy( bptr, " and " );
      bptr += 5;
    }
    else if ( silver || copper )
    {
      strcpy ( bptr, ", " );
      bptr += 2;
    }
    else
    {
      strcpy( bptr, " coins." );
      bptr += 7;
    }
  }
  
  if ( silver )
  {
    bptr += sprintf( bptr, "%d silver", money->silver );
    if ( copper )
    {
      strcpy( bptr, " and " );
      bptr += 5;
    }
    else
    {
      strcpy( bptr, " coins." );
      bptr += 7;
    }
  }
	
  if ( copper )
    bptr += sprintf( bptr, "%d copper coins.", money->copper );

  if ( !gold && !silver && !copper )
  {
    bptr += sprintf( bptr, "0 coins." );
  }
 
  return buf; 
}
#endif     

void do_immlist( CHAR_DATA *ch, char *argument )
{
 PLAYERLIST_DATA * player;
 int level;
 int col;
 char buf[MAX_STRING_LENGTH];
 char buf1[MAX_STRING_LENGTH];
 
 send_to_char( AT_BLUE, "\b\rIMMORTAL STAFF OF THE STORM\n\r" , ch );

 for ( level = MAX_LEVEL; level > LEVEL_MORTAL; level-- )
 {
 buf1[0] = '\0';
   sprintf(buf,"&R\n\r=====================================[%d]=====================================\n\r&W",level);
  strcat( buf1, buf );
  col = 0;
  for ( player = playerlist; player; player = player->next )
   {
     if ( player->level == level )
     {
        sprintf( buf, "%-19s", player->name );
        strcat( buf1, buf );
        if ( ++col % 4 == 0 )
          strcat( buf1, "\n\r" );
     } 
   }
  if ( col % 4 != 0 )
   strcat( buf1, "\n\r" );
  if (col != 0)
   send_to_char( C_DEFAULT, buf1 , ch );
}
}

void do_champlist( CHAR_DATA *ch, char *argument )
{
 PLAYERLIST_DATA * player;
 int level;
 int col;
 char buf[MAX_STRING_LENGTH];
 char buf1[MAX_STRING_LENGTH];

 send_to_char( AT_BLUE, "\n\rCHAMPIONS OF THE STORM\n\r", ch );
 for ( level = LEVEL_MORTAL; level >= LEVEL_HERO; level-- )
 {
 buf1[0] = '\0';
   sprintf(buf,"&R\n\r=====================================[%d]=====================================\n\r&W",level);
  strcat( buf1, buf );
  col = 0;
  for ( player = playerlist; player; player = player->next )
   {
     if ( player->level == level )
     {
        sprintf( buf, "%-19s", player->name );
        strcat( buf1, buf );
        if ( ++col % 4 == 0 )
          strcat( buf1, "\n\r" );
     } 
   }
  if ( col % 4 != 0 )
   strcat( buf1, "\n\r" );
  if ( col != 0 )
   send_to_char( C_DEFAULT, buf1 , ch );
 }
}

void do_playerlist( CHAR_DATA *ch, char *argument )
{
 PLAYERLIST_DATA * player;
 int level;
 int col;
 int counter = 0;
 char buf[MAX_STRING_LENGTH];
 char buf1[MAX_STRING_LENGTH];
 
 send_to_char( C_DEFAULT, "&B\n\rTHE STORM\n\r", ch );
 for ( level = MAX_LEVEL; level; level-- )
 {
 buf1[0] = '\0'; 
 
sprintf(buf,"&R\n\r=====================================[%3d]=====================================\n\r&W",level);
  strcat( buf1, buf );
  col = 0;
  for ( player = playerlist; player; player = player->next )
   {
     if ( player->level == level )
     {
	counter++;
        sprintf( buf, "%-19s", player->name );
        strcat( buf1, buf );
        if ( ++col % 4 == 0 )
          strcat( buf1, "\n\r" );
     } 
   }
  if ( col % 4 != 0 )
   strcat( buf1, "\n\r" );
  if ( col != 0 )
   send_to_char( C_DEFAULT, buf1 , ch );
}
sprintf(buf, "\n\r&WTOTAL: &R%d\n\r", counter);
send_to_char(AT_GREEN, buf, ch);
}

