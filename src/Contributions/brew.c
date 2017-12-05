brew.c by Jason Huang (huangjac@netcom.com) 

I. Introduction:	

The following is my version of the brew/scribe code. I have modified and 
made enhancements to Todd's original code; however, the underlying 
architecure is the same. If you wish to use this code, you must keep the 
credit header intact. 

The code is written specifically for Envy 1.0 with Jason's olc port. I
believe the get_item_type fcn is defined only in the olc code. However,
anyone using merc 2.x derivatives such as Rom 2.x, Isle 1.x, and Merc 2.x
should be able to adapt the following code with very slight changes. 

If you have problems implementing this code, send e-mail to Jason Huang at
huangjac@netcom.com.


II. Basics:

The syntax for using brew and scribe spell is simply brew <spell_name> and
scribe <spell_name> respectively. You must be holding a potion or a 
scroll in order for the skill work. I have noticed that the potions and 
scrolls in almost all envy areas have no holdable flags. Therefore, this 
allows me to avoid the whole mess of creating ITEM_VIAL and ITEM_PAPER to 
duplicate the function of ITEM_POTION and ITEM_SCROLL. Instead I just 
create two new items in the midgaard's area files, a potion with a 
holdable flag and a scroll with a holdable flag.

You can charge up to 3 spells with brew and scribe skills. However, to
prevent players from running around with a potion of heal, heal, heal, or
a scroll of gas breath, gas breath, gas breath, I have made it
successively more difficult to add new spells, the success percentage rate
is 80%, 25%, 10% respectively for adding the first, the second, and the third
spell. 

I have made the brew/scribe a fcn of skill, char_int, and char_wis. 
Whenever a character fails to brew a potion, the vial explodes and the
character is damaged with acid blast; similarly, when players fails to
scribe a scroll, the parchment bursts in flame and damages them with a
fireball. In all the cases, spell failure means the potion/scroll is
extracted. 


III. Code:

in act_obj.c:

/* Original Code by Todd Lair.                                        */
/* Improvements and Modification by Jason Huang (huangjac@netcom.com).*/
/* Permission to use this code is granted provided this header is     */
/* retained and unaltered.                                            */

void do_brew ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int sn;

    if ( !IS_NPC( ch )                                                  
	&& ch->level < skill_table[gsn_brew].skill_level[ch->class] )
    {                                          
	send_to_char( "You do not know how to brew potions.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Brew what spell?\n\r", ch );
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
	send_to_char( "You are not holding a vial.\n\r", ch );
	return;
    }

    if ( ( sn = skill_lookup(arg) )  < 0)
    {
	send_to_char( "You don't know any spells by that name.\n\r", ch );
	return;
    }

    /* preventing potions of gas breath, acid blast, etc.; doesn't make sense
       when you quaff a gas breath potion, and then the mobs in the room are
       hurt. Those TAR_IGNORE spells are a mixed blessing. - JH */
  
    if ( (skill_table[sn].target != TAR_CHAR_DEFENSIVE) && 
         (skill_table[sn].target != TAR_CHAR_SELF)              ) 
    {
	send_to_char( "You cannot brew that spell.\n\r", ch );
	return;
    }

    act( "$n begins preparing a potion.", ch, obj, NULL, TO_ROOM );
    WAIT_STATE( ch, skill_table[gsn_brew].beats );

    /* Check the skill percentage, fcn(wis,int,skill) */
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
	send_to_char( "You do not know how to scribe scrolls.\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Scribe what spell?\n\r", ch );
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
	send_to_char( "You are not holding a parchment.\n\r", ch );
	return;
    }


    if ( ( sn = skill_lookup(arg) )  < 0)
    {
	send_to_char( "You don't know any spells by that name.\n\r", ch );
	return;
    }
    
    act( "$n begins writing a scroll.", ch, obj, NULL, TO_ROOM );
    WAIT_STATE( ch, skill_table[gsn_scribe].beats );

    /* Check the skill percentage, fcn(int,wis,skill) */
    if ( !IS_NPC(ch) 
         && ( number_percent( ) > ch->pcdata->learned[gsn_brew] ||
              number_percent( ) > ((get_curr_int(ch)-13)*5 + 
                                   (get_curr_wis(ch)-13)*3) ))
    {
	act( "$p bursts in flames!", ch, obj, NULL, TO_CHAR );
	act( "$p bursts in flames!", ch, obj, NULL, TO_ROOM );
        spell_fireball(skill_lookup("fireball"), LEVEL_HERO - 1, ch, ch); 
	extract_obj( obj );
	return;
    }

    /* basically, making scrolls more potent than potions; also, scrolls
       are not limited in the choice of spells, i.e. scroll of enchant weapon
       has no analogs in potion forms --- JH */

    obj->level = ch->level*2/3;
    obj->value[0] = ch->level/3;
    spell_imprint(sn, ch->level, ch, obj); 

}



in magic.c:

void spell_imprint( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int       sp_slot, i, mana;
    char      buf[ MAX_STRING_LENGTH ];

      if (skill_table[sn].spell_fun == spell_null )
      {
	send_to_char("That is not a spell.\n\r",ch);
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
	send_to_char( "You don't have enough mana.\n\r", ch );
	return;
    }
      

    if ( number_percent( ) > ch->pcdata->learned[sn] )
    {
	send_to_char( "You lost your concentration.\n\r", ch );
	ch->mana -= mana / 2;
	return;
    }

    /* executing the imprinting process */
    ch->mana -= mana;
    obj->value[sp_slot] = sn;

    /* Making it successively harder to pack more spells into potions or 
       scrolls - JH */ 

    switch( sp_slot )
    {
   
    default:
	bug( "sp_slot has more than %d spells.", sp_slot );
	return;

    case 1:
        if ( number_percent() > 80 )
        { 
          sprintf(buf, "The magic enchantment has failed --- the %s vanishes.\n\r", item_type_name(obj->item_type) );
	  send_to_char( buf, ch );
	  extract_obj( obj );
	  return;
	}     
	break;
    case 2:
        if ( number_percent() > 25 )
        { 
          sprintf(buf, "The magic enchantment has failed --- the %s vanishes.\n\r", item_type_name(obj->item_type) );
	  send_to_char( buf, ch );
	  extract_obj( obj );
	  return;
	}     
	break;

    case 3:
        if ( number_percent() > 10 )
        { 
          sprintf(buf, "The magic enchantment has failed --- the %s vanishes.\n\r", item_type_name(obj->item_type) );
	  send_to_char( buf, ch );
	  extract_obj( obj );
	  return;
	}     
	break;
    } 
  

    /* labeling the item */

    free_string (obj->short_descr);
    sprintf ( buf, "a %s of ", item_type_name(obj->item_type) ); 
    for (i = 1; i <= sp_slot ; i++)
      if (obj->value[i] != -1)
      {
	strcat (buf, skill_table[obj->value[i]].name);
        (i != sp_slot ) ? strcat (buf, ", ") : strcat (buf, "") ; 
      }
    obj->short_descr = str_dup(buf);
	
    sprintf( buf, "%s %s", obj->name, item_type_name(obj->item_type) );
    free_string( obj->name );
    obj->name = str_dup( buf );        

    sprintf(buf, "You have imbued a new spell to the %s.\n\r", item_type_name(obj->item_type) );
    send_to_char( buf, ch );

    return;
}


in const.c:

/* I haven't bother assigning brew/scribe to various classes, so you should
   do this on your own */


    {
	"brew",			{ L_APP , L_APP,     1, L_APP, L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_brew,		0,	 24,
	"",			"!Brew!"
    },

    {
	"scribe",		{ L_APP, L_APP,     1, L_APP, L_APP },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_scribe,		0,	 24,
	"",			"!Scribe!"
    },
:

in merc.h:

extern	int	gsn_brew;
extern	int	gsn_scribe;
DECLARE_DO_FUN(	do_brew		);
DECLARE_DO_FUN(	do_scribe	);
DECLARE_DO_FUN(	do_imprint	);

in db.c:

int	gsn_brew;
int	gsn_scribe;


also to midgaard.are file:

Add the #3043 (an empty vial) and #3044 ( a parchment) to your area file;
make sure they have the holdable flag, 1|16384.

#3042
scroll recall~
a scroll of recall~
A scroll has carelessly been left here.~
~
2 64 1
12~ word of recall~ ~ ~
4 2000 100
E
scroll~
The scroll has a formula of 'Word of Recall' written upon it.
~
#3043
parchment~
a clean parchment~
A parchment used for writing spells has been left here.~
~
2 64 1|16384
15~ ~ ~ ~
5 2000 100
#3044
vial~
an empty vial~
A vial suitable for preparing potions has been left here.~
~
10 64 1|16384
15~ ~ ~ ~
5 2000 100
#3045
jacket scale mail~
a scale mail jacket~
A scale mail jacket is lying on the ground.~
~
9 0 9
4~ 0~ 0~ 0~
40 1000 300


