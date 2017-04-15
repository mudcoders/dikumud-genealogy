/* ************************************************************************
*   File: castle.c                                      Part of CircleMUD *
*  Usage: Special procedures for King's Castle area                       *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Special procedures for Kings Castle by Pjotr (d90-pem@nada.kth.se)     *
*  Coded by Sapowox (d90-jkr@nada.kth.se)                                 *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "limits.h"


/*   external vars  */
extern struct room_data *world;
extern struct char_data *character_list;
extern struct descriptor_data *descriptor_list;
extern struct index_data *mob_index;
extern struct index_data *obj_index;
extern struct time_info_data time_info;

/* IMPORTANT! 
   The below defined number is the zone number of the Kings Castle. 
   Change it to apply to your chosen zone number. The default zone
   number (On Alex and Alfa) is 80 (That is rooms and mobs have numbers
   in the 8000 series... */

#define Z_KINGS_C 150


/**********************************************************************\
|* Special procedures for Kings Castle by Pjotr (d90-pem@nada.kth.se) *|
|* Coded by Sapowox (d90-jkr@nada.kth.se)                             *|
\**********************************************************************/

#define C_MOB_SPEC(zone,mob) (mob_index[real_mobile(((zone)*100)+(mob))].func)

#define R_MOB(zone, mob) (real_mobile(((zone)*100)+(mob)))
#define R_OBJ(zone, obj) (real_object(((zone)*100)+(obj)))
#define R_ROOM(zone, num) (real_room(((zone)*100)+(num)))

#define CASTLE_ITEM(item) (Z_KINGS_C*100+(item))


int	CastleGuard(struct char_data *ch, int cmd, char *arg) ;
int	James(struct char_data *ch, int cmd, char *arg) ;
int	cleaning(struct char_data *ch, int cmd, char *arg) ;
int	DicknDavid (struct char_data *ch, int cmd, char *arg) ;
int	tim (struct char_data *ch, int cmd, char *arg) ;
int	tom (struct char_data *ch, int cmd, char *arg) ;
int	king_welmar (struct char_data *ch, int cmd, char *arg) ;
int	training_master (struct char_data *ch, int cmd, char *arg) ;
int	peter (struct char_data *ch, int cmd, char *arg) ;
int	jerry (struct char_data *ch, int cmd, char *arg) ;


/* Routine assign_kings_castle */
/* Used to assign function pointers to all mobiles in the Kings Castle */
/* Add a call to it from spec_assign.c! */

void	assign_kings_castle (void)
{
   C_MOB_SPEC(Z_KINGS_C,  1) = king_welmar ;     /* Our dear friend, the King */
   C_MOB_SPEC(Z_KINGS_C,  3) = CastleGuard ;     /* Jim */
   C_MOB_SPEC(Z_KINGS_C,  4) = CastleGuard ;     /* Brian */
   C_MOB_SPEC(Z_KINGS_C,  5) = CastleGuard ;     /* Mick */
   C_MOB_SPEC(Z_KINGS_C,  6) = CastleGuard ;     /* Matt */
   C_MOB_SPEC(Z_KINGS_C,  7) = CastleGuard ;     /* Jochem */
   C_MOB_SPEC(Z_KINGS_C,  8) = CastleGuard ;     /* Anne */
   C_MOB_SPEC(Z_KINGS_C,  9) = CastleGuard ;     /* Andrew */
   C_MOB_SPEC(Z_KINGS_C, 10) = CastleGuard ;     /* Bertram */
   C_MOB_SPEC(Z_KINGS_C, 11) = CastleGuard ;     /* Jeanette */
   C_MOB_SPEC(Z_KINGS_C, 12) = peter ;           /* Peter */
   C_MOB_SPEC(Z_KINGS_C, 13) = training_master ; /* The training master */
   C_MOB_SPEC(Z_KINGS_C, 16) = James ;           /* James the Butler */
   C_MOB_SPEC(Z_KINGS_C, 17) = cleaning ;        /* Ze Cleaning Fomen */
   C_MOB_SPEC(Z_KINGS_C, 20) = tim ;             /* Tim, Tom's twin */
   C_MOB_SPEC(Z_KINGS_C, 21) = tom ;             /* Tom, Tim's twin */
   C_MOB_SPEC(Z_KINGS_C, 24) = DicknDavid ;      /* Dick, guard of the Treasury */
   C_MOB_SPEC(Z_KINGS_C, 25) = DicknDavid ;      /* David, Dicks brother */
   C_MOB_SPEC(Z_KINGS_C, 26) = jerry ;           /* Jerry, the Gambler */
   C_MOB_SPEC(Z_KINGS_C, 27) = CastleGuard ;     /* Michael */
   C_MOB_SPEC(Z_KINGS_C, 28) = CastleGuard ;     /* Hans */
   C_MOB_SPEC(Z_KINGS_C, 29) = CastleGuard ;     /* Boris */
}


/* Routine member_of_staff */
/* Used to see if a character is a member of the castle staff */
/* Used mainly by BANZAI:ng NPC:s */
int	member_of_staff (struct char_data *chChar)
{
   int	ch_num ;

   if (!IS_NPC (chChar))
      return (FALSE) ;

   ch_num = mob_index [chChar->nr].virtual ;
   return (ch_num == CASTLE_ITEM(1) || 
       (ch_num > CASTLE_ITEM (2) && ch_num < CASTLE_ITEM(15)) || 
       (ch_num > CASTLE_ITEM(15) && ch_num < CASTLE_ITEM(18)) || 
       (ch_num > CASTLE_ITEM(18) && ch_num < CASTLE_ITEM(30))) ;
}


/* Function member_of_royal_guard */
/* Returns TRUE if the character is a guard on duty, otherwise FALSE */
/* Used by Peter the captain of the royal guard */
int	member_of_royal_guard (struct char_data *chChar)
{
   int	ch_num ;

   if (!chChar || !IS_NPC (chChar))
      return FALSE ;

   ch_num = mob_index [chChar->nr].virtual ;
   return (ch_num == CASTLE_ITEM(3) || 
       ch_num == CASTLE_ITEM(6) || 
       (ch_num > CASTLE_ITEM (7) && ch_num < CASTLE_ITEM(12)) || 
       (ch_num > CASTLE_ITEM(23) && ch_num < CASTLE_ITEM(26))) ;
}


/* Function find_npc_by_name */
/* Returns a pointer to an npc by the given name */
/* Used by Tim and Tom */
struct char_data *find_npc_by_name (struct char_data *chAtChar, char *pszName,
int iLen)
{
   struct char_data *ch ;

   for (ch = world[chAtChar->in_room].people ; ch ; ch = ch->next_in_room)
      if (IS_NPC(ch))
	 if (!strncmp (pszName, ch->player.short_descr, iLen))
	    return (ch) ;

   return NULL ;
}


/* Function find_guard */
/* Returns the pointer to a guard on duty. */
/* Used by Peter the Captain of the Royal Guard */
struct char_data *find_guard (struct char_data *chAtChar)
{

   struct char_data *ch ;

   for (ch = world[chAtChar->in_room].people ; ch ; ch = ch->next_in_room)
      if (!ch->specials.fighting && member_of_royal_guard (ch))
	 return ch ;

   return NULL ;
}


/* Function get_victim */
/* Returns a pointer to a randomly chosen character in the same room, */
/* fighting someone in the castle staff... */
/* Used by BANZAII-ing characters and King Welmar... */
struct char_data *get_victim (struct char_data *chAtChar)
{

   struct char_data *ch ;
   int	iNum_bad_guys = 0, iVictim ;

   for (ch = world[chAtChar->in_room].people ; ch ; ch = ch->next_in_room)
      if (ch->specials.fighting && 
          member_of_staff (ch->specials.fighting))
	 iNum_bad_guys++;

   if (!iNum_bad_guys)
      return NULL ;

   iVictim = number (0, iNum_bad_guys) ; /* How nice, we give them a chance */
   if (!iVictim)
      return NULL ;

   iNum_bad_guys = 0 ;

   for (ch = world[chAtChar->in_room].people ; ch ; ch = ch->next_in_room)
      if (ch->specials.fighting && 
          member_of_staff (ch->specials.fighting) && 
          ++iNum_bad_guys == iVictim)
	 return ch ;

   return NULL;
}


/* Function banzaii */
/* Makes a character banzaii on attackers of the castle staff */
/* Used by Guards, Tim, Tom, Dick, David, Peter, Master, King and Guards */
int	banzaii (struct char_data *ch)
{

   struct char_data *chOpponent = NULL ;

   if (!AWAKE (ch) || GET_POS (ch) == POSITION_FIGHTING)
      return FALSE ;

   if ((chOpponent = get_victim (ch))) {
      act ("$n roars: 'Protect the Kingdom of Great King Welmar!  BANZAIIII!!!'",
          FALSE, ch, 0, 0, TO_ROOM) ;
      hit (ch, chOpponent, TYPE_UNDEFINED) ;
      return TRUE ;
   }

   return FALSE ;
}


/* Function do_npc_rescue */
/* Makes ch_hero rescue ch_victim */
/* Used by Tim and Tom */
int	do_npc_rescue (struct char_data *ch_hero, struct char_data *ch_victim)
{

   struct char_data *ch_bad_guy ;

   for (ch_bad_guy = world[ch_hero->in_room].people; 
       ch_bad_guy && (ch_bad_guy->specials.fighting != ch_victim); 
       ch_bad_guy = ch_bad_guy->next_in_room)
      ;
   if (ch_bad_guy) {
      if (ch_bad_guy == ch_hero)
	 return FALSE ; /* NO WAY I'll rescue the one I'm fighting! */
      act ("You bravely rescue $N.\n\r", FALSE, ch_hero, 0, ch_victim, TO_CHAR) ;
      act("You are rescued by $N, your loyal friend!\n\r", 
          FALSE, ch_victim, 0, ch_hero, TO_CHAR);
      act("$n heroically rescues $N.", FALSE, ch_hero, 0, ch_victim, TO_NOTVICT);

      if (ch_bad_guy->specials.fighting)
	 stop_fighting(ch_bad_guy);
      if (ch_hero->specials.fighting)
	 stop_fighting(ch_hero);

      set_fighting(ch_hero, ch_bad_guy);
      set_fighting(ch_bad_guy, ch_hero);
      return TRUE ;
   }
   return FALSE ;
}


/* Procedure to block a person trying to enter a room. */
/* Used by Tim/Tom at Kings bedroom and Dick/David at treasury */
int	block_way(struct char_data *ch, int cmd, char *arg, int iIn_room, 
int iProhibited_direction)
{

   if (cmd != ++iProhibited_direction || (ch->player.short_descr && 
       !strncmp (ch->player.short_descr, "King Welmar", 11)))
      return FALSE ;

   if ((ch->in_room == real_room(iIn_room)) && (cmd == iProhibited_direction)) {
      if (!member_of_staff (ch))
	 act ("The guard roars at $n and pushes $m back.", 
	     FALSE, ch, 0, 0, TO_ROOM) ;
      send_to_char ("The guard roars: 'Entrance is Prohibited!', and pushes you back.\n\r", ch) ;
      return (TRUE) ;
   }

   return FALSE ;
}


/* Routine to check if an object is trash... */
/* Used by James the Butler and the Cleaning Lady */
int	is_trash (struct obj_data *i)
{

   if (IS_SET(i->obj_flags.wear_flags, ITEM_TAKE) && 
       ((i->obj_flags.type_flag == ITEM_DRINKCON) || 
       (i->obj_flags.cost <= 10)))
      return TRUE ;
   else
      return FALSE;
}


/* Function fry_victim */
/* Finds a suitabe victim, and cast some _NASTY_ spell on him */
/* Used by King Welmar */
void	fry_victim (struct char_data *ch)
{

   struct char_data *tch ;

   if (ch->points.mana < 10)
      return ;

   /* Find someone suitable to fry ! */

   if (!(tch = get_victim (ch)))
      return ;

   switch (number (0, 8)) {
   case 1:
   case 2:
   case 3:
      act("You raise your hand in a dramatical gesture.", 1, ch, 0, 0, TO_CHAR);
      act("$n raises $s hand in a dramatical gesture.", 1, ch, 0, 0, TO_ROOM);
      cast_colour_spray (GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, tch, 0) ;
      break;
   case 4:
   case 5:
      act("You concentrate and mumble to yourself.", 1, ch, 0, 0, TO_CHAR) ;
      act("$n concentrates, and mumbles to $mself.", 1, ch, 0, 0, TO_ROOM) ;
      cast_harm (GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, tch, 0) ;
      break;
   case 6:
   case 7:
      act ("You look deep into the eyes of $N.", 1, ch, 0, tch, TO_CHAR) ;
      act ("$n looks deep into the eyes of $N.", 1, ch, 0, tch, TO_NOTVICT) ;
      act ("You see an ill-boding flame in the eye of $n.", 
          1, ch, 0, tch, TO_VICT) ;
      cast_fireball (GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, tch, 0) ;
      break ;
   default:
      if (!number (0, 1)) {
	 act ("You utter the words, 'pzar', to heal yourself.", 
	     1, ch, 0, 0, TO_CHAR) ;
	 act ("$n utters the words, 'pzar'", 1, ch, 0, 0, TO_ROOM) ;
	 cast_heal (GET_LEVEL(ch), ch, "", SPELL_TYPE_SPELL, ch, 0) ;
      }
      break;
   }

   ch->points.mana -= 10 ;

   return ;
}


/* Function king_welmar */
/* Control the actions and movements of the King. */
/* Used by King Welmar */
int	king_welmar(struct char_data *ch, int cmd, char *arg)
{

   static char	*monolog [] = {
      "$n proclaims 'Primus in regnis Geticis coronam'.",
      "$n proclaims 'regiam gessi, subiique regis'.",
      "$n proclaims 'munus et mores colui sereno'.",
      "$n proclaims 'principe dignos'."       };

   static char	bedroom_path[] = "s33004o1c1S.";

   static char	throne_path[] = "W3o3cG52211rg.";

   static char	monolog_path[] = "ABCDPPPP" ;

   static char	*path;
   static int	index;
   static bool move = FALSE;

   ACMD(do_move);
   ACMD(do_open);
   ACMD(do_lock);
   ACMD(do_unlock);
   ACMD(do_close);

   if (!move) {
      if (time_info.hours == 8 && ch->in_room == R_ROOM (Z_KINGS_C, 51)) {
	 move = TRUE;
	 path = throne_path;
	 index = 0;
      } else if (time_info.hours == 21 && ch->in_room == R_ROOM (Z_KINGS_C, 17)) {
	 move = TRUE;
	 path = bedroom_path;
	 index = 0;
      } else if (time_info.hours == 12 && ch->in_room == R_ROOM (Z_KINGS_C, 17)) {
	 move = TRUE;
	 path = monolog_path;
	 index = 0;
      }

   }

   if (cmd || (GET_POS(ch) < POSITION_SLEEPING) || 
       (GET_POS(ch) == POSITION_SLEEPING && !move))
      return FALSE;

   if (GET_POS(ch) == POSITION_FIGHTING) {
      fry_victim (ch) ;
      return FALSE ;
   } else if (banzaii (ch))
      return FALSE ;

   if (!move)
      return FALSE ;

   switch (path[index]) {
   case '0' :
   case '1' :
   case '2' :
   case '3' :
   case '4' :
   case '5' :
      do_move(ch, "", path[index] - '0' + 1, 0);
      break;
   case 'A':
   case 'B':
   case 'C':
   case 'D':
      act(monolog [path[index]-'A'], FALSE, ch, 0, 0, TO_ROOM);
      break ;
   case 'P':
      break ;
   case 'W' :
      GET_POS(ch) = POSITION_STANDING;
      act("$n awakens and stands up.", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'S' :
      GET_POS(ch) = POSITION_SLEEPING;
      act("$n lies down on $s beautiful bed and instantly falls asleep.", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'r' :
      GET_POS(ch) = POSITION_SITTING;
      act("$n sits down on his great throne.", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 's' :
      GET_POS(ch) = POSITION_STANDING;
      act("$n stands up.", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'G' :
      act("$n says 'Good morning, trusted friends.'", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'g' :
      act("$n says 'Good morning, dear subjects.'", FALSE, ch, 0, 0, TO_ROOM);
      break;

   case 'o' :
      do_unlock(ch, "door", 0, 0);
      do_open(ch, "door", 0, 0);
      break;

   case 'c' :
      do_close(ch, "door", 0, 0);
      do_lock(ch, "door", 0, 0);
      break;

   case '.' :
      move = FALSE;
      break;
   }

   index++;
   return FALSE;
}


/* Function training_master */
/* Acts actions to the training room, if his students are present */
/* Also allowes warrior-class to practice */
/* Used by the Training Master */
int	training_master (struct char_data *ch, int cmd, char *arg)
{

   struct char_data *pupil1, *pupil2, *tch ;

   SPECIAL(guild);

   if (!AWAKE(ch) || (GET_POS(ch) == POSITION_FIGHTING))
      return FALSE ;

   if ((cmd == 164) || (cmd == 170)) {
      if (GET_CLASS(ch) == CLASS_WARRIOR)
	 guild (ch, cmd, arg) ;
      else
	 send_to_char ("The Training Master does not allow your class to practice here.\n\r", ch) ;
      return TRUE ;
   }

   if (cmd)
      return FALSE ;

   if (!banzaii (ch) && !number (0, 2)) {
      if ((pupil1 = find_npc_by_name (ch, "Brian", 5)) && 
          (pupil2 = find_npc_by_name (ch, "Mick", 4)) && 
          (!pupil1->specials.fighting) && (!pupil2->specials.fighting)) {
	 if (number(0, 1)) {
	    tch = pupil1 ;
	    pupil1 = pupil2 ;
	    pupil2 = tch ;
	 }
	 switch (number (0, 7)) {
	 case 0:
	    act ("$n hits $N on $s head with a powerful blow.",
	        FALSE, pupil1, 0, pupil2, TO_NOTVICT) ;
	    act ("You hit $N on $s head with a powerful blow.",
	        FALSE, pupil1, 0, pupil2, TO_CHAR) ;
	    act ("$n hits you on your head with a powerful blow.",
	        FALSE, pupil1, 0, pupil2, TO_VICT) ;
	    break ;
	 case 1:
	    act ("$n hits $N in $s chest with a thrust.",
	        FALSE, pupil1, 0, pupil2, TO_NOTVICT) ;
	    act ("You manage to thrust $N in the chest.",
	        FALSE, pupil1, 0, pupil2, TO_CHAR) ;
	    act ("$n manages to thrust you in your chest.",
	        FALSE, pupil1, 0, pupil2, TO_VICT) ;
	    break ;
	 case 2:
	    send_to_char ("You command your pupils to bow\n\r.", ch) ;
	    act ("$n commands his pupils to bow.", FALSE, ch, 0, 0, TO_ROOM) ;
	    act ("$n bows before $N.",
	        FALSE, pupil1, 0, pupil2, TO_NOTVICT) ;
	    act ("$N bows before $n.",
	        FALSE, pupil1, 0, pupil2, TO_NOTVICT) ;
	    act ("You bow before $N, who returns your gesture.",
	        FALSE, pupil1, 0, pupil2, TO_CHAR) ;
	    act ("You bow before $n, who returns your gesture.",
	        FALSE, pupil1, 0, pupil2, TO_VICT) ;
	    break ;
	 case 3:
	    act ("$N yells at $n, as he fumbles and drops his sword.",
	        FALSE, pupil1, 0, ch, TO_NOTVICT) ;
	    act ("$n quickly picks up his weapon.", FALSE, pupil1, 0, 0, TO_ROOM) ;
	    act ("$N yells at you, as you fumble, losing your weapon.",
	        FALSE, pupil1, 0, ch, TO_CHAR) ;
	    send_to_char ("You quickly pick up your weapon again.", pupil1) ;
	    act ("You yell at $n, as he fumbles, losing his weapon.",
	        FALSE, pupil1, 0, ch, TO_VICT) ;
	    break ;
	 case 4:
	    act ("$N tricks $n, and slashes him across the back.",
	        FALSE, pupil1, 0, pupil2, TO_NOTVICT) ;
	    act ("$N tricks you, and slashes you across your back.",
	        FALSE, pupil1, 0, pupil2, TO_CHAR) ;
	    act ("You trick $n, and quickly slash him across his back.",
	        FALSE, pupil1, 0, pupil2, TO_VICT) ;
	    break ;
	 case 5:
	    act ("$n lunges a blow at $N but $N parries skillfully.",
	        FALSE, pupil1, 0, pupil2, TO_NOTVICT) ;
	    act ("You lunge a blow at $N but $E parries skillfully.",
	        FALSE, pupil1, 0, pupil2, TO_CHAR) ;
	    act ("$n lunges a blow at you, but you skillfully parry it.",
	        FALSE, pupil1, 0, pupil2, TO_VICT) ;
	    break ;
	 case 6:
	    act ("$n clumsily tries to kick $N, but misses.",
	        FALSE, pupil1, 0, pupil2, TO_NOTVICT) ;
	    act ("You clumsily miss $N with your poor excuse for a kick.",
	        FALSE, pupil1, 0, pupil2, TO_CHAR) ;
	    act ("$n fails an unusually clumsy attempt at kicking you.",
	        FALSE, pupil1, 0, pupil2, TO_VICT) ;
	    break ;
	 default:
	    send_to_char ("You show your pupils an advanced technique.", ch) ;
	    act ("$n shows his pupils an advanced technique.",
	        FALSE, ch, 0, 0, TO_ROOM) ;
	    break ;
	 }
      }
   }
   return FALSE ;
}


int	tom (struct char_data *ch, int cmd, char *arg)
{

   struct char_data *king, *tim ;

   ACMD(do_follow);

   if (!AWAKE(ch))
      return FALSE ;

   if ((!cmd) && (king = find_npc_by_name (ch, "King Welmar", 11))) {
      if (!ch->master)
	 do_follow (ch, "King Welmar", 0, 0) ;
      if (king->specials.fighting)
	 do_npc_rescue (ch, king) ;
   }

   if (!cmd)
      if ((tim = find_npc_by_name (ch, "Tim", 3)))
	 if (tim->specials.fighting && 2 * tim->points.hit < ch->points.hit)
	    do_npc_rescue (ch, tim) ;

   if (!cmd && GET_POS(ch) != POSITION_FIGHTING)
      banzaii (ch) ;

   return block_way(ch, cmd, arg, CASTLE_ITEM(49), 1) ;
}


int	tim (struct char_data *ch, int cmd, char *arg)
{

   struct char_data *king, *tom ;

   ACMD(do_follow);

   if (!AWAKE(ch))
      return FALSE ;

   if ((!cmd) && (king = find_npc_by_name (ch, "King Welmar", 11))) {
      if (!ch->master)
	 do_follow (ch, "King Welmar", 0, 0) ;
      if (king->specials.fighting)
	 do_npc_rescue (ch, king) ;
   }

   if (!cmd)
      if ((tom = find_npc_by_name (ch, "Tom", 3)))
	 if (tom->specials.fighting && 2 * tom->points.hit < ch->points.hit)
	    do_npc_rescue (ch, tom) ;

   if (!cmd && GET_POS(ch) != POSITION_FIGHTING)
      banzaii (ch) ;

   return block_way(ch, cmd, arg, CASTLE_ITEM(49), 1) ;
}


/* Routine for James the Butler */
/* Complains if he finds any trash... */
int	James (struct char_data *ch, int cmd, char *arg)
{

   struct obj_data *i;

   if (cmd || !AWAKE(ch) || (GET_POS(ch) == POSITION_FIGHTING))
      return (FALSE) ;

   for (i = world[ch->in_room].contents; i; i = i->next_content)
      if (is_trash (i)) {
	 act ("$n says: 'My oh my!  I ought to fire that lazy cleaning woman!'", 
	     FALSE, ch, 0, 0, TO_ROOM) ;
	 act ("$n picks up a piece of trash.", FALSE, ch, 0, 0, TO_ROOM) ;
	 obj_from_room (i) ;
	 obj_to_char (i, ch) ;
	 return TRUE ;
      }
      else
	 return FALSE ;

   return FALSE;
}


/* Routine for the Cleaning Woman */
/* Picks up any trash she finds... */
int	cleaning(struct char_data *ch, int cmd, char *arg)
{
   struct obj_data *i;

   if (cmd || !AWAKE(ch))
      return(FALSE);

   for (i = world[ch->in_room].contents; i; i = i->next_content) {
      if (is_trash (i))
	 act("$n picks up some trash.", FALSE, ch, 0, 0, TO_ROOM);
      obj_from_room (i);
      obj_to_char (i, ch);
      return TRUE ;
   }

   return FALSE ;
}


/* Routine CastleGuard */
/* Standard routine for ordinary castle guards */
int	CastleGuard (struct char_data *ch, int cmd, char *arg)
{

   if (cmd || !AWAKE(ch) || (GET_POS(ch) == POSITION_FIGHTING))
      return FALSE ;

   return (banzaii (ch)) ;
}


/* Routine DicknDave */
/* Routine for the guards Dick and David */
int	DicknDavid (struct char_data *ch, int cmd, char *arg)
{

   if (!AWAKE(ch))
      return (FALSE) ;

   if (!cmd && GET_POS(ch) != POSITION_FIGHTING)
      banzaii (ch) ;

   return (block_way (ch, cmd, arg, CASTLE_ITEM(36), 1)) ;
}


/* Routine peter */
/* Routine for Captain of the Guards. */
int	peter (struct char_data *ch, int cmd, char *arg)
{

   struct char_data *ch_guard ;

   if (cmd || !AWAKE(ch) || GET_POS(ch) == POSITION_FIGHTING)
      return (FALSE) ;

   if (banzaii (ch))
      return FALSE ;

   if (!(number (0, 3)) && (ch_guard = find_guard (ch)))
      switch (number (0, 5)) {
      case 0:
	 act ("$N comes sharply into attention as $n inspects $M.",
	     FALSE, ch, 0, ch_guard, TO_NOTVICT) ;
	 act ("$N comes sharply into attention as you inspect $M.",
	     FALSE, ch, 0, ch_guard, TO_CHAR) ;
	 act ("You go sharply into attention as $n inspects you.",
	     FALSE, ch, 0, ch_guard, TO_VICT) ;
	 break ;
      case 1:
	 act ("$N looks very small, as $n roars at $M.",
	     FALSE, ch, 0, ch_guard, TO_NOTVICT) ;
	 act ("$N looks very small as you roar at $M.",
	     FALSE, ch, 0, ch_guard, TO_CHAR) ;
	 act ("You feel very small as $N roars at you.",
	     FALSE, ch, 0, ch_guard, TO_VICT) ;
	 break ;
      case 2:
	 act ("$n gives $N some Royal directions.",
	     FALSE, ch, 0, ch_guard, TO_NOTVICT) ;
	 act ("You give $N some Royal directions.",
	     FALSE, ch, 0, ch_guard, TO_CHAR) ;
	 act ("$n gives you some Royal directions.",
	     FALSE, ch, 0, ch_guard, TO_VICT) ;
	 break ;
      case 3:
	 act ("$n looks at you.", FALSE, ch, 0, ch_guard, TO_VICT) ;
	 act ("$n looks at $N.", FALSE, ch, 0, ch_guard, TO_NOTVICT) ;
	 act ("$n growls: 'Those boots need polishing!'",
	     FALSE, ch, 0, ch_guard, TO_ROOM) ;
	 act ("You growl at $N.", FALSE, ch, 0, ch_guard, TO_CHAR) ;
	 break ;
      case 4:
	 act ("$n looks at you.", FALSE, ch, 0, ch_guard, TO_VICT) ;
	 act ("$n looks at $N.", FALSE, ch, 0, ch_guard, TO_NOTVICT) ;
	 act ("$n growls: 'Straighten that collar!'",
	     FALSE, ch, 0, ch_guard, TO_ROOM) ;
	 act ("You growl at $N.", FALSE, ch, 0, ch_guard, TO_CHAR) ;
	 break ;
      default:
	 act ("$n looks at you.", FALSE, ch, 0, ch_guard, TO_VICT) ;
	 act ("$n looks at $N.", FALSE, ch, 0, ch_guard, TO_NOTVICT) ;
	 act ("$n growls: 'That chain mail looks rusty!  CLEAN IT !!!'",
	     FALSE, ch, 0, ch_guard, TO_ROOM) ;
	 act ("You growl at $N.", FALSE, ch, 0, ch_guard, TO_CHAR) ;
	 break ;
      }

   return FALSE ;
}


/* Procedure for Jerry and Michael in x08 of King's Castle.      */
/* Code by Sapowox modified by Pjotr.(Original code from Master) */

int	jerry (struct char_data *ch, int cmd, char *arg)
{

   struct char_data *gambler1, *gambler2, *tch ;

   if (!AWAKE(ch) || (GET_POS(ch) == POSITION_FIGHTING))
      return FALSE ;

   if (cmd)
      return FALSE ;

   if (!banzaii (ch) && !number (0, 2)) {
      if ((gambler1 = ch) && 
          (gambler2 = find_npc_by_name (ch, "Michael", 7)) && 
          (!gambler1->specials.fighting) && (!gambler2->specials.fighting)) {
	 if (number(0, 1)) {
	    tch = gambler1 ;
	    gambler1 = gambler2 ;
	    gambler2 = tch ;
	 }
	 switch (number (0, 5)) {
	 case 0:
	    act ("$n rolls the dice and cheers loudly at the result.",
	        FALSE, gambler1, 0, gambler2, TO_NOTVICT) ;
	    act ("You roll the dice and cheer. GREAT!",
	        FALSE, gambler1, 0, gambler2, TO_CHAR) ;
	    act ("$n cheers loudly as $e rolls the dice.",
	        FALSE, gambler1, 0, gambler2, TO_VICT) ;
	    break ;
	 case 1:
	    act ("$n curses the Goddess of Luck roundly as he sees $N's roll.",
	        FALSE, gambler1, 0, gambler2, TO_NOTVICT) ;
	    act ("You curse the Goddess of Luck as $N rolls.",
	        FALSE, gambler1, 0, gambler2, TO_CHAR) ;
	    act ("$n swears angrily. You are in luck!",
	        FALSE, gambler1, 0, gambler2, TO_VICT) ;
	    break ;
	 case 2:
	    act ("$n sighs loudly and gives $N some gold.",
	        FALSE, gambler1, 0, gambler2, TO_NOTVICT) ;
	    act ("You sigh loudly at the pain of having to give $N some gold.",
	        FALSE, gambler1, 0, gambler2, TO_CHAR) ;
	    act ("$n sighs loudly as $e gives you your rightful win.",
	        FALSE, gambler1, 0, gambler2, TO_VICT) ;
	    break ;
	 case 3:
	    act ("$n smiles remorsefully as $N's roll tops his.",
	        FALSE, gambler1, 0, gambler2, TO_NOTVICT) ;
	    act ("You smile sadly as you see that $N beats you. Again.",
	        FALSE, gambler1, 0, gambler2, TO_CHAR) ;
	    act ("$n smiles remorsefully as your roll tops his.",
	        FALSE, gambler1, 0, gambler2, TO_VICT) ;
	    break ;
	 case 4:
	    act ("$n excitedly follows the dice with his eyes.",
	        FALSE, gambler1, 0, gambler2, TO_NOTVICT) ;
	    act ("You excitedly follow the dice with your eyes.",
	        FALSE, gambler1, 0, gambler2, TO_CHAR) ;
	    act ("$n excitedly follows the dice with his eyes.",
	        FALSE, gambler1, 0, gambler2, TO_VICT) ;
	    break ;
	 default:
	    act ("$n says 'Well, my luck has to change soon', as he shakes the dice.",
	        FALSE, gambler1, 0, gambler2, TO_NOTVICT) ;
	    act ("You say 'Well, my luck has to change soon' and shake the dice.",
	        FALSE, gambler1, 0, gambler2, TO_CHAR) ;
	    act ("$n says 'Well, my luck has to change soon', as he shakes the dice.",
	        FALSE, gambler1, 0, gambler2, TO_VICT) ;
	    break ;
	 }
      }
   }
   return FALSE ;
}


