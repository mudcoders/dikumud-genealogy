#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void spell_imprint( int sn, int level, CHAR_DATA *ch, void *vo );

void do_brew ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int sn;

    if ( !IS_NPC( ch )                                                  
	&& ch->level < skill_table[gsn_brew].skill_level[ch->class] )
    {                                          
	send_to_char( "You do not know how to brew potions.\r\n", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Brew what spell?\r\n", ch );
	return;
    }

    /* Do we have a vial to brew potions? */
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->item_type == ITEM_POTION && obj->wear_loc == WEAR_HOLD )
	    break;
    }

    /* Interesting ... Most scrolls/potions in the mud have no hold
       flag; so, the problem with players running around making scrolls 
       with 3 heals or 3 gas breath from pre-existing scrolls has been 
       severely reduced. Still, I like the idea of 80% success rate for  
       first spell imprint, 25% for 2nd, and 10% for 3rd. I don't like the
       idea of a scroll with 3 ultrablast spells; although, I have limited
       its applicability when I reduced the spell->level to 1/3 and 1/4 of 
       ch->level for scrolls and potions respectively. --- JH */


    /* I will just then make two items, an empty vial and a parchment available
       in midgaard shops with holdable flags and -1 for each of the 3 spell
       slots. Need to update the midgaard.are files --- JH */

    if ( !obj )
    {
	send_to_char( "You are not holding a vial.\r\n", ch );
	return;
    }

    if ( ( sn = skill_lookup(arg) )  < 0)
    {
	send_to_char( "You don't know any spells by that name.\r\n", ch );
	return;
    }

    /* preventing potions of gas breath, acid blast, etc.; doesn't make sense
       when you quaff a gas breath potion, and then the mobs in the room are
       hurt. Those TAR_IGNORE spells are a mixed blessing. - JH */

/* Well I (maniac) think you should be able to do so... so I comment it out..
  
    if ( (skill_table[sn].target != TAR_CHAR_DEFENSIVE) && 
         (skill_table[sn].target != TAR_CHAR_SELF)              ) 
    {
	send_to_char( "You cannot brew that spell.\r\n", ch );
	return;
    }
*** So much for commenting it out... */

    act( "$n begins preparing a potion.", ch, obj, NULL, TO_ROOM );
    WAIT_STATE( ch, skill_table[gsn_brew].beats );

    /* Check the skill percentage, fcn(wis,int,skill) */
    if ( ch->level < L_SEN)
    {
	 if ( !IS_NPC(ch) 
            && ( number_percent( ) > ch->pcdata->learned[gsn_brew] ||
                 number_percent( ) > ((get_curr_int(ch)-13)*5 + 
                                      (get_curr_wis(ch)-13)*3) ))
 	{
	   act( "$p explodes violently!", ch, obj, NULL, TO_CHAR );
	   act( "$p explodes violently!", ch, obj, NULL, TO_ROOM );
           spell_acid_blast(skill_lookup("acid blast"), LEVEL_HERO - 1, ch, ch);
	   extract_obj( obj );
	   return;
	}
    }

    /* took this outside of imprint codes, so I can make do_brew differs from
       do_scribe; basically, setting potion level and spell level --- JH */

    obj->level = ch->level/2;
    obj->value[0] = ch->level/4;
    spell_imprint(sn, ch->level, ch, obj); 

}

void do_scribe ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int sn;

    if ( !IS_NPC( ch )                                                  
	&& ch->level < skill_table[gsn_scribe].skill_level[ch->class] )
    {                                          
	send_to_char( "You do not know how to scribe scrolls.\r\n", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Scribe what spell?\r\n", ch );
	return;
    }

    /* Do we have a parchment to scribe spells? */
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
	if ( obj->item_type == ITEM_SCROLL && obj->wear_loc == WEAR_HOLD )
	    break;
    }
    if ( !obj )
    {
	send_to_char( "You are not holding a parchment.\r\n", ch );
	return;
    }


    if ( ( sn = skill_lookup(arg) )  < 0)
    {
	send_to_char( "You don't know any spells by that name.\r\n", ch );
	return;
    }
    
    act( "$n begins writing a scroll.", ch, obj, NULL, TO_ROOM );
    WAIT_STATE( ch, skill_table[gsn_scribe].beats );

    /* Check the skill percentage, fcn(int,wis,skill) */
    if ( ch->level < L_SEN )
    {
	if ( !IS_NPC(ch) 
	   && ( number_percent( ) > ch->pcdata->learned[gsn_brew] ||
		number_percent( ) >	((get_curr_int(ch)-13)*5 + 
					(get_curr_wis(ch)-13)*3) ))
	{
	   act( "$p bursts in flames!", ch, obj, NULL, TO_CHAR );
	   act( "$p bursts in flames!", ch, obj, NULL, TO_ROOM );
           spell_fireball(skill_lookup("fireball"), LEVEL_HERO - 1, ch, ch); 
	   extract_obj( obj );
	   return;
	}
    }

    /* basically, making scrolls more potent than potions; also, scrolls
       are not limited in the choice of spells, i.e. scroll of enchant weapon
       has no analogs in potion forms --- JH */

    obj->level = ch->level*2/3;
    obj->value[0] = ch->level/3;
    spell_imprint(sn, ch->level, ch, obj); 

}

void spell_imprint( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int       sp_slot, i, mana;
    char      buf[ MAX_STRING_LENGTH ];

      if (skill_table[sn].spell_fun == spell_null )
      {
	send_to_char("That is not a spell.\r\n",ch);
	return;
      }

    /* counting the number of spells contained within */

    for (sp_slot = i = 1; i < 4; i++) 
	if (obj->value[i] != -1)
	    sp_slot++;

    if (sp_slot > 3)
    {
	act ("$p cannot contain any more spells.", ch, obj, NULL, TO_CHAR);
	return;
    }

   /* scribe/brew costs 4 times the normal mana required to cast the spell */

    mana = 4 * MANA_COST(ch, sn);
	    
    if ( !IS_NPC(ch) && ch->mana < mana )
    {
	send_to_char( "You don't have enough mana.\r\n", ch );
	return;
    }
      

    if ((number_percent() > ch->pcdata->learned[sn]) && (ch->level < L_SEN))
    {
	send_to_char( "You lost your concentration.\r\n", ch );
	ch->mana -= mana / 2;
	return;
    }

    /* executing the imprinting process */
    ch->mana -= mana;
    obj->value[sp_slot] = sn;

    /* Making it successively harder to pack more spells into potions or 
       scrolls - JH */ 

    if (ch->level < L_SEN )
    {
    switch( sp_slot )
    {
   
    default:
	bug( "sp_slot has more than %d spells.", sp_slot );
	return;

    case 1:
        if ( number_percent() > 80 )
        { 
          sprintf(buf, "The magic enchantment has failed --- the %s vanishes.\r\n", item_type_name(obj) );
	  send_to_char( buf, ch );
	  extract_obj( obj );
	  return;
	}     
	break;
    case 2:
        if ( number_percent() > 25 )
        { 
          sprintf(buf, "The magic enchantment has failed --- the %s vanishes.\r\n", item_type_name(obj) );
	  send_to_char( buf, ch );
	  extract_obj( obj );
	  return;
	}     
	break;

    case 3:
        if ( number_percent() > 10 )
        { 
          sprintf(buf, "The magic enchantment has failed --- the %s vanishes.\r\n", item_type_name(obj) );
	  send_to_char( buf, ch );
	  extract_obj( obj );
	  return;
	}     
	break;
    }
    } /* If level < L_SEN */ 
  

    /* labeling the item */

    free_string (obj->short_descr);
    sprintf ( buf, "a %s of ", item_type_name(obj) ); 
    for (i = 1; i <= sp_slot ; i++)
      if (obj->value[i] != -1)
      {
	strcat (buf, skill_table[obj->value[i]].name);
        (i != sp_slot ) ? strcat (buf, ", ") : strcat (buf, "") ; 
      }
    obj->short_descr = str_dup(buf);
	
    sprintf( buf, "%s %s", obj->name, item_type_name(obj) );
    free_string( obj->name );
    obj->name = str_dup( buf );        

    sprintf(buf, "You have imbued a new spell to the %s.\r\n", item_type_name(obj) );
    send_to_char( buf, ch );

    return;
}

/* Study skill... converted from Rom by Maniac */
void do_study( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    OBJ_DATA *scroll;

    argument = one_argument(argument, arg1);

    if (( scroll = get_obj_carry(ch,arg1)) == NULL)
    {
        send_to_char("You don't have that scroll.\r\n",ch);
        return;
    }

    if ( scroll->item_type != ITEM_SCROLL )
    {
        send_to_char("You can only study scrolls.\r\n",ch);
        return;
    }

    if ( ch->level < scroll->level )
    {
        send_to_char("You are not a high enough level to use this scroll.\r\n", ch );
        return;
    }

    if (( skill_table[scroll->value[1]].skill_level[ch->class] >= 72 )
        && ( !IS_IMMORTAL(ch) ))
    {
        send_to_char("Your class may not learn that spell.\r\n",ch);
        return;
    }

    if ( skill_table[scroll->value[1]].skill_level[ch->class] > ch->level )
    {
        send_to_char("This spell is beyond your grasp. Perhaps in a few levels...\r\n", ch );
        return;
    }

    if ( ch->pcdata->learned[scroll->value[1]] > 0 )
    {
        send_to_char("You know that spell already!\r\n",ch);
        return;
    }

    act("$n studies $p.",ch,scroll,NULL,TO_ROOM);
    act("You study $p.",ch,scroll,NULL,TO_CHAR);

    if (number_percent() >= (20 + ch->pcdata->learned[gsn_scrolls]) * 4/5)
    {
        send_to_char("You mess up and the scroll vanishes!\r\n",ch);
        act("$n screams in anger.",ch,NULL,NULL,TO_ROOM);
/*      check_improve(ch,gsn_scrolls,FALSE,2);		*/
    }

    else
    {
        act("You learn the spell!",ch,NULL,NULL,TO_CHAR);
        act("$n learned the spell!",ch,NULL,NULL,TO_ROOM);
        ch->pcdata->learned[scroll->value[1]] = 5;
/*      ch->pcdata->points += 3; 			*/
/*      check_improve(ch,gsn_scrolls,TRUE,2);		*/
    }
    extract_obj(scroll);
    return;
}

