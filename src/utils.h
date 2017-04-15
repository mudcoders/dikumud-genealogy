/* ************************************************************************
*   File: utils.h                                       Part of CircleMUD *
*  Usage: header file: utility macros and prototypes of utility funcs     *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993 by the Trustees of the Johns Hopkins University     *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

/* Linux does not have a SIGBUS */
#ifdef LINUX
#define SIGBUS SIGUNUSED
#endif

extern struct weather_data weather_info;

/* public functions */
char	*str_dup(const char *source);
int	str_cmp(char *arg1, char *arg2);
int	strn_cmp(char *arg1, char *arg2, int n);
void	log(char *str);
void	mudlog(char *str, char type, sbyte level, byte file);
void	log_death_trap(struct char_data *ch);
int	MAX(int a, int b);
int	MIN(int a, int b);
int	number(int from, int to);
int	dice(int number, int size);
void	sprintbit(long vektor, char *names[], char *result);
void	sprinttype(int type, char *names[], char *result);
struct time_info_data age(struct char_data *ch);

/* in magic.c */
bool	circle_follow(struct char_data *ch, struct char_data * victim);

/* defines for fseek */
#ifndef SEEK_SET
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2
#endif

/* defines for mudlog() */

#define OFF	0
#define BRF	1
#define NRM	2
#define CMP	3

#define TRUE  1

#define FALSE 0

#define YESNO(a) ((a) ? "YES" : "NO")
#define ONOFF(a) ((a) ? "ON" : "OFF")

#define LOWER(c) (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))

#define UPPER(c) (((c)>='a'  && (c) <= 'z') ? ((c)+('A'-'a')) : (c) )

/* Functions in utility.c                     */
/* #define MAX(a,b) (((a) > (b)) ? (a) : (b)) */
/* #define MIN(a,b) (((a) < (b)) ? (a) : (b)) */

#define ISNEWL(ch) ((ch) == '\n' || (ch) == '\r') 

#define IF_STR(st) ((st) ? (st) : "\0")

#define CAP(st)  (*(st) = UPPER(*(st)), st)

#define CREATE(result, type, number)  do {\
	if (!((result) = (type *) calloc ((number), sizeof(type))))\
		{ perror("malloc failure"); abort(); } } while(0)

#define RECREATE(result,type,number) do {\
  if (!((result) = (type *) realloc ((result), sizeof(type) * (number))))\
		{ perror("realloc failure"); abort(); } } while(0)

#define IS_SET(flag,bit)  ((flag) & (bit))
#define SET_BIT(var,bit)  ((var) |= (bit))
#define REMOVE_BIT(var,bit)  ((var) &= ~(bit))
#define TOGGLE_BIT(var,bit) ((var) = (var) ^ (bit))

#define IS_NPC(ch)  (IS_SET((ch)->specials2.act, MOB_ISNPC))
#define IS_MOB(ch)  (IS_NPC(ch) && ((ch)->nr >-1))

#define MOB_FLAGS(ch) ((ch)->specials2.act)
#define PLR_FLAGS(ch) ((ch)->specials2.act)
#define PRF_FLAGS(ch) ((ch)->specials2.pref)

#define MOB_FLAGGED(ch, flag) (IS_NPC(ch) && IS_SET(MOB_FLAGS(ch), (flag)))
#define PLR_FLAGGED(ch, flag) (!IS_NPC(ch) && IS_SET(PLR_FLAGS(ch), (flag)))
#define PRF_FLAGGED(ch, flag) (IS_SET(PRF_FLAGS(ch), (flag)))

#define PLR_TOG_CHK(ch,flag) ((TOGGLE_BIT(PLR_FLAGS(ch), (flag))) & (flag))
#define PRF_TOG_CHK(ch,flag) ((TOGGLE_BIT(PRF_FLAGS(ch), (flag))) & (flag))

#define SWITCH(a,b) { (a) ^= (b); \
                      (b) ^= (a); \
                      (a) ^= (b); }

#define IS_AFFECTED(ch,skill) ( IS_SET((ch)->specials.affected_by, (skill)) )

#define IS_DARK(room)  ( !world[room].light && \
                         (IS_SET(world[room].room_flags, DARK) || \
                          ( ( world[room].sector_type != SECT_INSIDE && \
                              world[room].sector_type != SECT_CITY ) && \
                            (weather_info.sunlight == SUN_SET || \
			     weather_info.sunlight == SUN_DARK)) ) )

#define IS_LIGHT(room)  (!IS_DARK(room))

#define GET_INVIS_LEV(ch) ((ch)->specials.invis_level)

/* Can subject see character "obj"? */
#define CAN_SEE(sub, obj) ( ((sub) == (obj)) || ( \
      ( (GET_LEVEL(sub) >= GET_INVIS_LEV(obj)) && \
      ( (PRF_FLAGGED((sub), PRF_HOLYLIGHT)) ||    \
           ((!IS_AFFECTED((sub), AFF_BLIND)) &&   \
           ((!IS_AFFECTED((obj), AFF_INVISIBLE)) || \
	     IS_AFFECTED((sub), AFF_DETECT_INVISIBLE)) && \
           (IS_LIGHT((sub)->in_room) || IS_AFFECTED((sub), AFF_INFRARED)))))))

#define WIMP_LEVEL(ch) ((ch)->specials2.wimp_level)

#define GET_REQ(i) (i<2  ? "Awful" :(i<4  ? "Bad"     :(i<7  ? "Poor"      :\
(i<10 ? "Average" :(i<14 ? "Fair"    :(i<20 ? "Good"    :(i<24 ? "Very good" :\
        "Superb" )))))))

#define HSHR(ch) ((ch)->player.sex ?					\
	(((ch)->player.sex == 1) ? "his" : "her") : "its")

#define HSSH(ch) ((ch)->player.sex ?					\
	(((ch)->player.sex == 1) ? "he" : "she") : "it")

#define HMHR(ch) ((ch)->player.sex ? 					\
	(((ch)->player.sex == 1) ? "him" : "her") : "it")	

#define AN(string) (strchr("aeiouAEIOU", *string) ? "an" : "a")

#define ANA(obj) (strchr("aeiouyAEIOUY", *(obj)->name) ? "An" : "A")

#define SANA(obj) (strchr("aeiouyAEIOUY", *(obj)->name) ? "an" : "a")

#define GET_POS(ch)     ((ch)->specials.position)

#define GET_COND(ch, i) ((ch)->specials2.conditions[(i)])

#define GET_LOADROOM(ch) ((ch)->specials2.load_room)

#define GET_NAME(ch)    (IS_NPC(ch) ? (ch)->player.short_descr : (ch)->player.name)

#define GET_TITLE(ch)   ((ch)->player.title)

#define GET_LEVEL(ch)   ((ch)->player.level)

#define GET_CLASS(ch)   ((ch)->player.class)

#define GET_HOME(ch)		((ch)->player.hometown)

#define GET_AGE(ch)     (age(ch).year)

#define GET_STR(ch)     ((ch)->tmpabilities.str)

#define GET_ADD(ch)     ((ch)->tmpabilities.str_add)

#define GET_DEX(ch)     ((ch)->tmpabilities.dex)

#define GET_INT(ch)     ((ch)->tmpabilities.intel)

#define GET_WIS(ch)     ((ch)->tmpabilities.wis)

#define GET_CON(ch)     ((ch)->tmpabilities.con)

#define STRENGTH_APPLY_INDEX(ch) \
        ( ((GET_ADD(ch)==0) || (GET_STR(ch) != 18)) ? GET_STR(ch) :\
          (GET_ADD(ch) <= 50) ? 26 :( \
          (GET_ADD(ch) <= 75) ? 27 :( \
          (GET_ADD(ch) <= 90) ? 28 :( \
          (GET_ADD(ch) <= 99) ? 29 :  30 ) ) )                   \
        )

#define GET_AC(ch)      ((ch)->points.armor)

/* The hit_limit, move_limit, and mana_limit functions are gone.  See
   limits.c for details.
*/

#define GET_HIT(ch)	((ch)->points.hit)
#define GET_MAX_HIT(ch)	((ch)->points.max_hit)

#define GET_MOVE(ch)	((ch)->points.move)
#define GET_MAX_MOVE(ch) ((ch)->points.max_move)

#define GET_MANA(ch)	((ch)->points.mana)
#define GET_MAX_MANA(ch) ((ch)->points.max_mana)

#define GET_GOLD(ch)	((ch)->points.gold)
#define GET_BANK_GOLD(ch) ((ch)->points.bank_gold)

#define GET_EXP(ch)	((ch)->points.exp)

#define GET_HEIGHT(ch)	((ch)->player.height)

#define GET_WEIGHT(ch)	((ch)->player.weight)

#define GET_SEX(ch)	((ch)->player.sex)

#define GET_HITROLL(ch) ((ch)->points.hitroll)

#define GET_DAMROLL(ch) ((ch)->points.damroll)

#define SPELLS_TO_LEARN(ch) ((ch)->specials2.spells_to_learn)

#define GET_IDNUM(ch) (IS_NPC(ch) ? -1 : (ch)->specials2.idnum)

#define AWAKE(ch) (GET_POS(ch) > POSITION_SLEEPING)

#define GET_SKILL(ch, i) ((ch)->skills ? (((ch)->skills)[i]) : 80)

#define SET_SKILL(ch, i, pct) { if ((ch)->skills) (ch)->skills[i] = pct; }

#define WAIT_STATE(ch, cycle)  (((ch)->desc) ? (ch)->desc->wait = (cycle) : 0)

#define CHECK_WAIT(ch) (((ch)->desc) ? ((ch)->desc->wait > 1) : 0)


/* Object And Carry related macros */

#define CAN_SEE_OBJ(sub, obj)                                    \
	(( (( !IS_SET((obj)->obj_flags.extra_flags, ITEM_INVISIBLE) ||   \
	     IS_AFFECTED((sub),AFF_DETECT_INVISIBLE) ) &&               \
	     !IS_AFFECTED((sub),AFF_BLIND)) && IS_LIGHT(sub->in_room) ) \
	     || PRF_FLAGGED((sub), PRF_HOLYLIGHT))


#define GET_ITEM_TYPE(obj) ((obj)->obj_flags.type_flag)

#define CAN_WEAR(obj, part) (IS_SET((obj)->obj_flags.wear_flags,part))

#define GET_OBJ_WEIGHT(obj) ((obj)->obj_flags.weight)

#define CAN_CARRY_W(ch) (str_app[STRENGTH_APPLY_INDEX(ch)].carry_w)

#define CAN_CARRY_N(ch) (5+GET_DEX(ch)/2+GET_LEVEL(ch)/2)

#define IS_CARRYING_W(ch) ((ch)->specials.carry_weight)

#define IS_CARRYING_N(ch) ((ch)->specials.carry_items)

#define CAN_CARRY_OBJ(ch,obj)  \
   (((IS_CARRYING_W(ch) + GET_OBJ_WEIGHT(obj)) <= CAN_CARRY_W(ch)) &&   \
    ((IS_CARRYING_N(ch) + 1) <= CAN_CARRY_N(ch)))

#define CAN_GET_OBJ(ch, obj)   \
   (CAN_WEAR((obj), ITEM_TAKE) && CAN_CARRY_OBJ((ch),(obj)) &&          \
    CAN_SEE_OBJ((ch),(obj)))

#define IS_OBJ_STAT(obj,stat) (IS_SET((obj)->obj_flags.extra_flags,stat))



/* char name/short_desc(for mobs) or someone?  */

#define PERS(ch, vict)   (CAN_SEE(vict, ch) ? GET_NAME(ch) : "someone")

#define OBJS(obj, vict) (CAN_SEE_OBJ((vict), (obj)) ? \
	(obj)->short_description  : "something")

#define OBJN(obj, vict) (CAN_SEE_OBJ((vict), (obj)) ? \
	fname((obj)->name) : "something")

#define OUTSIDE(ch) (!IS_SET(world[(ch)->in_room].room_flags,INDOORS))

#define EXIT(ch, door)  (world[(ch)->in_room].dir_option[door])

#define CAN_GO(ch, door) (EXIT(ch,door)  &&  (EXIT(ch,door)->to_room != NOWHERE) \
                          && !IS_SET(EXIT(ch, door)->exit_info, EX_CLOSED))

#define GET_ALIGNMENT(ch) ((ch)->specials2.alignment)

#define IS_GOOD(ch)    (GET_ALIGNMENT(ch) >= 350)
#define IS_EVIL(ch)    (GET_ALIGNMENT(ch) <= -350)
#define IS_NEUTRAL(ch) (!IS_GOOD(ch) && !IS_EVIL(ch))

#define CLASS_ABBR(ch) (IS_NPC(ch) ? "--" : class_abbrevs[(int)GET_CLASS(ch)])

