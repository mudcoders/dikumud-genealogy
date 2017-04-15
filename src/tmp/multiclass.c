/***********************************
 * Do NOT delete this file!        *
 * -- Hannibal			   *
 ***********************************/

bool can_use_skpell( CHAR_DATA *ch, int sn )
{
  int iClass = 0;
  if ( IS_NPC( ch ) )
      return TRUE;
  for ( iClass = 0; ch->class[iClass] != -1; iClass++ )
    {
    if ( ch->level >= skill_table[sn].skill_level[ch->class[iClass]] )
      return TRUE;
    }
  return FALSE;
}
bool has_spells( CHAR_DATA *ch )
{
  int iClass;
  if ( IS_NPC( ch ) )
     return FALSE;
  for ( iClass = 0; ch->class[iClass] != -1; iClass++ )
     if ( class_table[iClass].fMana )
	return TRUE;
  return FALSE;
}
bool is_class( CHAR_DATA *ch, int class )
{
  int iClass; 
  if ( IS_NPC( ch ) )
    return FALSE;

/*  if( ch->class[0] == class )
     return TRUE;  */

  for ( iClass = 0; ch->class[iClass] != -1; iClass++ )
    {
    if ( ch->class[iClass] == class )
	return TRUE;
    }

  return FALSE;
}
int prime_class( CHAR_DATA *ch )
{
  return ch->class[0];
}
int number_classes( CHAR_DATA *ch )
{
  int iClass;
  if ( IS_NPC( ch ) )
     return 0;
   for ( iClass = 0; ch->class[iClass] != -1; iClass++ )
    ; 
  return iClass;
}
char *class_long( CHAR_DATA *ch )
{
  static char buf [ 512 ];
  int iClass;
  buf[0] = '\0';
  if ( IS_NPC( ch ) )
    return "Mobile";
  for ( iClass = 0; ch->class[iClass] != -1 ; iClass++ )
	{
	strcat( buf, "/" );
	strcat( buf, class_table[ch->class[iClass]].who_long );
	}
  return buf+1;
}
char *class_short( CHAR_DATA *ch )
{
  static char buf [ 512 ];
  int iClass;
  buf[0] = '\0';
  if ( IS_NPC( ch ) )
    return "Mob";
  for ( iClass = 0; ch->class[iClass] != -1 ; iClass++ )
	{
	strcat( buf, "/" );
	strcat( buf, class_table[ch->class[iClass]].who_name );
	}
  return buf+1;
}
char *class_numbers( CHAR_DATA *ch, bool pSave )
{
  static char buf[ 512 ];
  char buf2[ 10 ];
  int iClass;
  buf[0] = '\0';
  if ( IS_NPC( ch ) )
    return "0";
  for ( iClass = 0; ch->class[ iClass ] != -1; iClass++ )
	{
	strcat( buf, " " );
	sprintf( buf2, "%d", ch->class[iClass] );
	strcat( buf, buf2 );
	}
  if ( pSave )
    strcat( buf, " -1" );
  return buf+1;
}
bool gets_zapped( CHAR_DATA *ch, OBJ_DATA *obj )
{
if ( !IS_NPC( ch ) )
if ( ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL   ) && IS_EVIL   ( ch ) )
|| ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD   ) && IS_GOOD   ( ch ) )
|| ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL( ch ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_MAGE   ) && is_class( ch, CLASS_MAGE ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_CLERIC ) && is_class( ch, CLASS_CLERIC ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_THIEF  ) && is_class( ch, CLASS_THIEF ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_WARRIOR) && is_class( ch, CLASS_WARRIOR ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_PSI    ) && is_class( ch, CLASS_PSIONICIST ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_DRUID  ) && is_class( ch, CLASS_DRUID ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_RANGER ) && is_class( ch, CLASS_RANGER ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_PALADIN) && is_class( ch, CLASS_PALADIN ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_BARD   ) && is_class( ch, CLASS_BARD ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_VAMP   ) && is_class( ch, CLASS_VAMPIRE ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_NECRO  ) && is_class( ch, CLASS_NECROMANCER ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_WWF    ) && is_class( ch, CLASS_WEREWOLF ) )
|| ( IS_ANTI_CLASS( obj, ITEM_ANTI_MONK   ) && is_class( ch, CLASS_MONK ) )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_HUMAN     ) && ch->race == RACE_HUMAN )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_ELF       ) && ch->race == RACE_ELF )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_DWARF     ) && ch->race == RACE_DWARF )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_PIXIE     ) && ch->race == RACE_PIXIE )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_HALFLING  ) && ch->race == RACE_HALFLING )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_DROW      ) && ch->race == RACE_DROW )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_ELDER     ) && ch->race == RACE_ELDER )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_OGRE      ) && ch->race == RACE_OGRE )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_LIZARDMAN ) && ch->race == RACE_LIZARDMAN )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_DEMON     ) && ch->race == RACE_DEMON )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_GHOUL     ) && ch->race == RACE_GHOUL )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_ILLITHID  ) && ch->race == RACE_ILLITHID )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_MINOTAUR  ) && ch->race == RACE_MINOTAUR )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_SHADOW    ) && ch->race == RACE_SHADOW )
|| ( IS_ANTI_RACE( obj, ITEM_ANTI_TABAXI    ) && ch->race == RACE_TABAXI ) )
  return TRUE;
return FALSE;
}
