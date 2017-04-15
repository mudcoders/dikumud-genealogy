/***************************************************************************
 *  file: shop.c , Shop module.                            Part of DIKUMUD *
 *  Usage: Procedures handling shops and shopkeepers.                      *
 *  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
 *                                                                         *
 *  Copyright (C) 1992, 1993 Michael Chastain, Michael Quan, Mitchell Tse  *
 *  Performance optimization and bug fixes by MERC Industries.             *
 *  You can use our stuff in any way you like whatsoever so long as this   *
 *  copyright notice remains intact.  If you like it please drop a line    *
 *  to mec@garnet.berkeley.edu.                                            *
 *                                                                         *
 *  This is free software and you are benefitting.  We hope that you       *
 *  share your changes too.  What goes around, comes around.               *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "mob.h"
#include "obj.h"
#include "handler.h"
#include "db.h"
#include "interp.h"
#include "utils.h"

#define MAX_SHOP    64
#define MAX_TRADE    5

extern struct str_app_type str_app[];
extern struct index_data *mob_index;

struct shop_data
{
    int type[MAX_TRADE];    /* Types of things shop will buy.       */
    float profit_buy;       /* Factor to multiply cost with.        */
    float profit_sell;      /* Factor to multiply cost with.        */
    char *no_such_item1;    /* Message if keeper hasn't got an item */
    char *no_such_item2;    /* Message if player hasn't got an item */
    char *missing_cash1;    /* Message if keeper hasn't got cash    */
    char *missing_cash2;    /* Message if player hasn't got cash    */
    char *do_not_buy;       /* If keeper doesn't buy such things.   */
    char *message_buy;      /* Message when player buys item        */
    char *message_sell;     /* Message when player sells item       */
    int keeper;             /* The mob who owns the shop (virtual)  */
    int in_room;            /* Where is the shop?                   */
    int open1, open2;       /* When does the shop open?             */
    int close1, close2;     /* When does the shop close?            */
};



extern struct room_data *world;
extern struct time_info_data time_info;

struct shop_data shop_index[MAX_SHOP];
int max_shop;



/*
 * See if a shop keeper wants to trade.
 */
int is_ok( struct char_data *keeper, struct char_data *ch, int shop_nr )
{
    char buf[240];

    /*
     * Undesirables.
     */
    if ( IS_SET(ch->specials.affected_by, AFF_KILLER) )
    {
	do_say( keeper, "Go away before I call the guards!!", 0 );
	sprintf( buf, "%s the KILLER is over here!\n\r", GET_NAME(ch) );
	do_shout( keeper, buf, 0 );
	return FALSE;
    }

    if ( IS_SET(ch->specials.affected_by, AFF_THIEF) )
    {
	do_say( keeper, "Thieves are not welcome!", 0 );
	sprintf( buf, "%s the THIEF is over here!\n\r", GET_NAME(ch) );
	do_shout( keeper, buf, 0 );
	return FALSE;
    }

    /*
     * Shop hours.
     */
    if ( time_info.hours < shop_index[shop_nr].open1 )
    {
	do_say( keeper, "Come back later!", 0 );
	return FALSE;
    }
    
    else if ( time_info.hours <= shop_index[shop_nr].close1 )
	;

    else if ( time_info.hours < shop_index[shop_nr].open2 )
    {
	do_say( keeper, "Come back later!", 0 );
	return FALSE;
    }

    else if ( time_info.hours <= shop_index[shop_nr].close2 )
	;

    else
    {
	do_say( keeper, "Sorry, come back tomorrow.", 0 );
	return FALSE;
    }

    /*
     * Invisible people.
     */
    if ( !CAN_SEE( keeper, ch ) )
    {
	do_say( keeper, "I don't trade with someone I can't see!", 0 );
	return FALSE;
    }

    return TRUE;
}



/*
 * See if a shop will buy an item.
 */
int trade_with( struct obj_data *item, int shop_nr )
{
    int counter;

    if ( item->obj_flags.cost < 1 )
	return FALSE;

    for ( counter = 0; counter < MAX_TRADE; counter++ )
    {
	if ( GET_ITEM_TYPE(item) == shop_index[shop_nr].type[counter] )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Buy an item from a shop.
 */
void shopping_buy( char *arg, struct char_data *ch,
     struct char_data *keeper, int shop_nr )
{
    char buf[MAX_STRING_LENGTH];
    char argm[100];
    struct obj_data *obj;
    int cost;

    if ( !is_ok( keeper, ch, shop_nr ) )
	return;

    one_argument( arg, argm );
    if ( *argm == '\0' )
    {
	sprintf( buf, "%s What do you want to buy?", GET_NAME(ch) );
	do_tell( keeper, buf, 0 );
	return;
    }

    if ( ( obj = get_obj_in_list_vis( ch, argm, keeper->carrying ) ) == NULL )
    {
	sprintf( buf, shop_index[shop_nr].no_such_item1, GET_NAME(ch) );
	do_tell( keeper, buf, 0 );
	return;
    }

    if ( obj->obj_flags.cost <= 0 )
    {
	extract_obj( obj );
	sprintf( buf, shop_index[shop_nr].no_such_item1, GET_NAME(ch) );
	do_tell( keeper, buf, 0 );
	return;
    }

    cost = (int) (obj->obj_flags.cost * shop_index[shop_nr].profit_buy);

    if ( GET_GOLD(ch) < cost )
    {
	sprintf( buf, shop_index[shop_nr].missing_cash2, GET_NAME(ch) );
	do_tell( keeper, buf, 0 );
	return;
    }
    
    if ( IS_CARRYING_N(ch) + 1 > CAN_CARRY_N(ch) )
    {
	send_to_char( "You can't carry that many items.\n\r", ch );
	return;
    }

    if ( IS_CARRYING_W(ch) + obj->obj_flags.weight > CAN_CARRY_W(ch) )
    {
	send_to_char( "You can't carry that much weight.\n\r", ch );
	return;
    }

    act( "$n buys $p.", FALSE, ch, obj, 0, TO_ROOM );
    sprintf( buf, shop_index[shop_nr].message_buy, GET_NAME(ch), cost );
    do_tell( keeper, buf, 0 );
    sprintf( buf, "You now have %s.\n\r", obj->short_description );
    send_to_char( buf, ch );
    GET_GOLD(ch)     -= cost;
    GET_GOLD(keeper) += cost;

    /* Wormhole to map_eq_level */
    if ( obj->obj_flags.eq_level == 1000 )
	obj = read_object( obj->item_number, 0 );
    else
	obj_from_char( obj );

    obj_to_char( obj, ch );
    return; 
}



/*
 * Sell an item to a shop keeper.
 */
void shopping_sell( char *arg, struct char_data *ch,
     struct char_data *keeper, int shop_nr )
{
    char buf[MAX_STRING_LENGTH];
    char argm[100];
    struct obj_data *obj;
    int cost;

    if ( !is_ok( keeper, ch, shop_nr ) )
	return;

    one_argument( arg, argm );

    if ( *argm == '\0' )
    {
	sprintf( buf, "%s What do you want to sell?", GET_NAME(ch) );
	do_tell( keeper, buf, 0 );
	return;
    }

    if ( ( obj = get_obj_in_list_vis( ch, argm, ch->carrying ) ) == NULL )
    {
	sprintf( buf, shop_index[shop_nr].no_such_item2, GET_NAME(ch) );
	do_tell( keeper, buf, 0 );
	return;
    }

    if ( !trade_with( obj, shop_nr ) || obj->obj_flags.cost < 1 )
    {
	sprintf( buf, shop_index[shop_nr].do_not_buy, GET_NAME(ch) );
	do_tell( keeper, buf, 0 );
	return;
    }

    cost = (int) ( obj->obj_flags.cost * shop_index[shop_nr].profit_sell );
    if ( GET_GOLD(keeper) < cost )
    {
	sprintf( buf, shop_index[shop_nr].missing_cash1, GET_NAME(ch) );
	do_tell( keeper, buf, 0 );
	return;
    }

    act( "$n sells $p.", FALSE, ch, obj, 0, TO_ROOM );
    sprintf( buf, shop_index[shop_nr].message_sell, GET_NAME(ch), cost );
    do_tell( keeper, buf, 0 );
    sprintf( buf, "The shopkeeper now has %s.\n\r", obj->short_description );
    send_to_char( buf,ch );
    GET_GOLD(ch)     += cost;
    GET_GOLD(keeper) -= cost;

    if ( get_obj_in_list( argm, keeper->carrying )
    || GET_ITEM_TYPE(obj) == ITEM_TRASH )
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



/*
 * Value an item.
 */
void shopping_value( char *arg, struct char_data *ch, 
    struct char_data *keeper, int shop_nr )
{
    char buf[MAX_STRING_LENGTH];
    char argm[100];
    struct obj_data *obj;
    int cost;

    if ( !is_ok( keeper, ch, shop_nr ) )
	return;

    one_argument( arg, argm );

    if ( *argm == '\0' )
    {
	sprintf( buf, "%s What do you want to value?", GET_NAME(ch) );
	do_tell( keeper, buf, 0 );
	return;
    }

    if ( ( obj = get_obj_in_list_vis( ch, argm, ch->carrying ) ) == NULL )
    {
	sprintf( buf, shop_index[shop_nr].no_such_item2, GET_NAME(ch) );
	do_tell( keeper, buf, 0 );
	return;
    }

    if ( !trade_with( obj, shop_nr ) || obj->obj_flags.cost < 1 )
    {
	sprintf( buf, shop_index[shop_nr].do_not_buy, GET_NAME(ch) );
	do_tell( keeper, buf, 0 );
	return;
    }

    cost = (int) ( obj->obj_flags.cost * shop_index[shop_nr].profit_sell );
    sprintf( buf, "%s I'll give you %d gold coins for that.",
	GET_NAME(ch), cost );
    do_tell( keeper, buf, 0 );
    return;
}



/*
 * List available items.
 */
void shopping_list( char *arg, struct char_data *ch,
     struct char_data *keeper, int shop_nr )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_INPUT_LENGTH];
    struct obj_data *obj;
    int cost;
    extern char *drinks[];
    int found;

    if ( !is_ok( keeper, ch, shop_nr ) )
	return;

    strcpy( buf, "[Price] Item\n\r" );
    found = FALSE;
    for ( obj = keeper->carrying; obj; obj = obj->next_content )
    {
	if ( !CAN_SEE_OBJ( ch, obj ) || obj->obj_flags.cost <= 0 )
	    continue;

	found = TRUE;

	cost = (int) ( obj->obj_flags.cost * shop_index[shop_nr].profit_buy );
	if ( GET_ITEM_TYPE(obj) == ITEM_DRINKCON && obj->obj_flags.value[1] )
	{
	    sprintf( buf2, "[%5d] %s of %s.\n\r",
		cost, obj->short_description,
		drinks[obj->obj_flags.value[2]] );
	}
	else
	{
	    sprintf( buf2, "[%5d] %s.\n\r",
		cost, obj->short_description );
	}

	buf2[8] = UPPER(buf2[8]);
	strcat( buf, buf2 );
    }

    if ( !found )
	send_to_char( "You can't buy anything here!\n\r", ch );
    else
	send_to_char( buf, ch );

    return;
}



/*
 * Spec proc for shop keepers.
 */
int shop_keeper( struct char_data *ch, int cmd, char *arg )
{
    struct char_data *keeper;
    int shop_nr;

    /*
     * Find a shop keeper in the room.
     */
    for ( keeper = world[ch->in_room].people;
	keeper != NULL;
	keeper = keeper->next_in_room )
    {
	if ( IS_MOB(keeper) && mob_index[keeper->nr].func == shop_keeper )
	    goto LFound1;
    }
    log( "Shop_keeper: keeper not found." );
    return FALSE;

 LFound1:
    for ( shop_nr = 0; shop_nr < max_shop; shop_nr++ )
    {
	if ( shop_index[shop_nr].keeper == keeper->nr )
	    goto LFound2;
    }
    log( "Shop_keeper: shop_nr not found." );
    return FALSE;

 LFound2:
    if ( ch->in_room != shop_index[shop_nr].in_room )
	return FALSE;

    switch ( cmd )
    {
    default: return FALSE;
    case 56: shopping_buy   ( arg, ch, keeper, shop_nr ); break;
    case 57: shopping_sell  ( arg, ch, keeper, shop_nr ); break;
    case 58: shopping_value ( arg, ch, keeper, shop_nr ); break;
    case 59: shopping_list  ( arg, ch, keeper, shop_nr ); break;
    }

    return TRUE;
}



void boot_the_shops()
{
    char *buf;
    int temp;
    int count;
    FILE *fp;

    if ( ( fp = fopen( SHOP_FILE, "r" ) ) == NULL )
    {
	perror( SHOP_FILE );
	exit( 1 );
    }

    max_shop = 0;

    for ( ;; )
    {
	buf = fread_string( fp );
	if ( *buf == '$' )
	    break;
	if ( *buf != '#' )
	    continue;

	if ( max_shop >= MAX_SHOP )
	{
	    perror( "Too many shops.\n" );
	    exit( 1 );
	}

	/*
	 * Ignore "producing" list.
	 */
	for ( count = 0; count < 6; count++ )
	    fscanf( fp, "%d \n", &temp );

	fscanf( fp, "%f \n", &shop_index[max_shop].profit_buy  );
	fscanf( fp, "%f \n", &shop_index[max_shop].profit_sell );
	for( count = 0; count < MAX_TRADE; count++ )
	{
	    fscanf( fp, "%d \n", &shop_index[max_shop].type[count] );
	}

	shop_index[max_shop].no_such_item1	= fread_string( fp );
	shop_index[max_shop].no_such_item2	= fread_string( fp );
	shop_index[max_shop].do_not_buy		= fread_string( fp );
	shop_index[max_shop].missing_cash1	= fread_string( fp );
	shop_index[max_shop].missing_cash2	= fread_string( fp );
	shop_index[max_shop].message_buy	= fread_string( fp );
	shop_index[max_shop].message_sell	= fread_string( fp );

	fscanf( fp, "%d \n", &temp );		/* Temper	*/
	fscanf( fp, "%d \n", &temp );		/* Temper	*/

	fscanf( fp, "%d \n", &temp );
	shop_index[max_shop].keeper	= real_mobile( temp );

	fscanf( fp, "%d \n", &temp );		/* With_whom	*/

	fscanf( fp, "%d \n", &temp );
	shop_index[max_shop].in_room	= real_room( temp );

	fscanf( fp, "%d \n", &shop_index[max_shop].open1   );
	fscanf( fp, "%d \n", &shop_index[max_shop].close1  );
	fscanf( fp, "%d \n", &shop_index[max_shop].open2   );
	fscanf( fp, "%d \n", &shop_index[max_shop].close2  );

	max_shop++;
    }

    fclose( fp );
}



void assign_the_shopkeepers( )
{
    int shop_nr;

    for ( shop_nr = 0; shop_nr < max_shop; shop_nr++ )
	mob_index[shop_index[shop_nr].keeper].func = shop_keeper;

    return;
}
