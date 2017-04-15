/* this procedure controls all mobile spell casting */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"

/* locals procedures */
CHAR_DATA 	*target_harm	args( (CHAR_DATA *ch) );
CHAR_DATA	*target_aid	args( (CHAR_DATA *ch) );
bool		is_friend	args( (CHAR_DATA *ch, CHAR_DATA *victim) );  

/* returns a target suitable for mayhem */
CHAR_DATA *target_harm(CHAR_DATA *ch)
{
    CHAR_DATA *victim, *vch;
    int count;

    if (ch == NULL || ch->in_room == NULL)
	return NULL;

    count = 0;
    victim = NULL;

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (vch->fighting == ch && can_see(ch,vch))
	{
	    if number_range(0,count) == 0)	
	    	victim = vch; 
	    count++;
	}
    }
    
    return victim;
}


/* finds a friend to help */
CHAR_DATA *target_aid(CHAR_DATA *ch)
{
    CHAR_DATA *friend, *fch;
    int count;

    if (ch == NULL || ch->in_room == NULL)
	return NULL;

    count = 0;
    victim = NULL;

    for (fch = ch->in_room->people; fch != NULL; fch = fch->next_in_room)
    {
	if (is_friend(ch,fch) && can_see(ch,vch))
	{
	    if (number_range(0,count) == 0)
		friend = fch;
	    count++;
	}
    }

    return friend;
}

/* checks to see if a mobile is friendly with a char or NPC */
bool is_friend(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if (ch->fighting != NULL && is_same_group(victim,ch->fighting))
	return FALSE;

    if (is_same_group(ch,victim))
	return TRUE;

    /* check for assist_player */
    if (IS_NPC(ch) && IS_SET(ch->off_flags,ASSIST_PLAYERS) && !IS_NPC(victim))
	return TRUE;

    if (!IS_NPC(victim) || IS_NPC(ch))
	return FALSE;

    /* all cases below are NPC/NPC */

    /* charmees are not friends */
    if (IS_AFFECTED(ch,AFF_CHARM) || IS_AFFECTED(victim,AFF_CHARM))
	return FALSE;

    if (IS_SET(ch->off_flags,ASSIST_ALL))
	return TRUE;

    if (IS_SET(ch->off_flags,ASSIST_VNUM) 
    &&  ch->pIndexData == victim->pIndexData)
	return TRUE;

 
    if (IS_SET(ch->off_flags,ASSIST_RACE) && ch->race == victim->race)
	return TRUE;

    if (IS_SET(ch->off_flags,ASSIST_ALIGN)
    &&  (( IS_GOOD(ch) && IS_GOOD(victim))
    ||   ( IS_EVIL(ch) && IS_EVIL(victim))
    ||   ( IS_NEUTRAL(ch) && IS_NEUTRAL(victim)))
	return TRUE;

    return FALSE;
}


    
	

