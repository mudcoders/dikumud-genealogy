/***************************************************************************
 *  File: olc.h                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 ***************************************************************************/

/*
 * This is a header file for all the OLC files.  Feel free to copy it into
 * merc.h if you wish.  Many of these routines may be handy elsewhere in
 * the code.  - Jason Dinkel
 */



/*
 * The version info.  Please use this info when reporting bugs.
 * It is displayed in the game by typing 'version' while editing.
 * Do not remove these from the code - by request of Jason Dinkel
 */
#define VERSION	"ILAB Online Creation [Beta 1.1]"
#define AUTHOR	"     By Jason(jdinkel@mines.colorado.edu)"
#define DATE	"     (May. 15, 1995)"
#define CREDITS "     Original by Surreality(cxw197@psu.edu) and Locke(locke@lm.com)"


/*
 * New typedefs.
 */
typedef bool OLC_FUN 	args( ( CHAR_DATA * ch, char *argument ) );
#define DECLARE_OLC_FUN( fun )	OLC_FUN    fun


/*
 * Connected states for editor.
 */
#define ED_AREA 1
#define ED_ROOM 2
#define ED_OBJECT 3
#define ED_MOBILE 4
#define ED_MPROG  5


/*
 * Interpreter Prototypes
 */
void aedit 	args( ( CHAR_DATA * ch, char *argument ) );
void redit 	args( ( CHAR_DATA * ch, char *argument ) );
void medit 	args( ( CHAR_DATA * ch, char *argument ) );
void oedit 	args( ( CHAR_DATA * ch, char *argument ) );
void mpedit 	args( ( CHAR_DATA * ch, char *argument ) );


/*
 * OLC Constants
 */
#define MAX_MOB	1		/*
				 * Default maximum number for resetting mobs 
				 */


/*
 * Structure for an OLC editor command.
 */
struct olc_cmd_type
{
    char *const name;
    OLC_FUN *olc_fun;
};


/*
 * Structure for an OLC editor startup command.
 */
struct editor_cmd_type
{
    char *const name;
    DO_FUN *do_fun;
};


/*
 * Utils.
 */
AREA_DATA *get_vnum_area 	args( ( int vnum ) );
AREA_DATA *get_area_data 	args( ( int vnum ) );
int flag_value 			args( ( const struct flag_type * flag_table,
		     			char *argument ) );
char *flag_string 		args( ( const struct flag_type * flag_table,
					int bits ) );
void add_reset 			args( ( ROOM_INDEX_DATA * room,
		     			RESET_DATA * pReset, int index ) );


/*
 * Interpreter Table Prototypes
 */
extern const struct olc_cmd_type aedit_table[];
extern const struct olc_cmd_type redit_table[];
extern const struct olc_cmd_type oedit_table[];
extern const struct olc_cmd_type medit_table[];
extern const struct olc_cmd_type mpedit_table[];


/*
 * General Functions
 */
bool show_commands 	args( ( CHAR_DATA * ch, char *argument ) );
bool show_help 		args( ( CHAR_DATA * ch, char *argument ) );
bool edit_done 		args( ( CHAR_DATA * ch, char *argument ) );
bool show_version 	args( ( CHAR_DATA * ch, char *argument ) );


/*
 * Area Editor Prototypes
 */
DECLARE_OLC_FUN( aedit_show	);
DECLARE_OLC_FUN( aedit_create	);
DECLARE_OLC_FUN( aedit_name	);
DECLARE_OLC_FUN( aedit_file	);
DECLARE_OLC_FUN( aedit_age	);
DECLARE_OLC_FUN( aedit_recall	);
DECLARE_OLC_FUN( aedit_reset	);
DECLARE_OLC_FUN( aedit_security	);
DECLARE_OLC_FUN( aedit_builder	);
DECLARE_OLC_FUN( aedit_vnum	);
DECLARE_OLC_FUN( aedit_lvnum	);
DECLARE_OLC_FUN( aedit_uvnum	);


/*
 * Room Editor Prototypes
 */
DECLARE_OLC_FUN( redit_show	);
DECLARE_OLC_FUN( redit_create	);
DECLARE_OLC_FUN( redit_name	);
DECLARE_OLC_FUN( redit_desc	);
DECLARE_OLC_FUN( redit_ed	);
DECLARE_OLC_FUN( redit_format	);
DECLARE_OLC_FUN( redit_north	);
DECLARE_OLC_FUN( redit_south	);
DECLARE_OLC_FUN( redit_east	);
DECLARE_OLC_FUN( redit_west	);
DECLARE_OLC_FUN( redit_up	);
DECLARE_OLC_FUN( redit_down	);
DECLARE_OLC_FUN( redit_move	);
DECLARE_OLC_FUN( redit_mreset	);
DECLARE_OLC_FUN( redit_oreset	);
DECLARE_OLC_FUN( redit_mlist	);
DECLARE_OLC_FUN( redit_olist	);
DECLARE_OLC_FUN( redit_mshow	);
DECLARE_OLC_FUN( redit_oshow	);


/*
 * Object Editor Prototypes
 */
DECLARE_OLC_FUN( oedit_show		);
DECLARE_OLC_FUN( oedit_create		);
DECLARE_OLC_FUN( oedit_copy		);
DECLARE_OLC_FUN( oedit_name		);
DECLARE_OLC_FUN( oedit_short		);
DECLARE_OLC_FUN( oedit_long		);
DECLARE_OLC_FUN( oedit_addaffect	);
DECLARE_OLC_FUN( oedit_delaffect	);
DECLARE_OLC_FUN( oedit_value0		);
DECLARE_OLC_FUN( oedit_value1		);
DECLARE_OLC_FUN( oedit_value2		);
DECLARE_OLC_FUN( oedit_value3		);
DECLARE_OLC_FUN( oedit_value4		);
DECLARE_OLC_FUN( oedit_weight		);
DECLARE_OLC_FUN( oedit_cost		);
DECLARE_OLC_FUN( oedit_ed		);


/*
 * Mobile Editor Prototypes
 */
DECLARE_OLC_FUN( medit_show	);
DECLARE_OLC_FUN( medit_create	);
DECLARE_OLC_FUN( medit_copy	);
DECLARE_OLC_FUN( medit_name	);
DECLARE_OLC_FUN( medit_short	);
DECLARE_OLC_FUN( medit_long	);
DECLARE_OLC_FUN( medit_shop	);
DECLARE_OLC_FUN( medit_desc	);
DECLARE_OLC_FUN( medit_level	);
DECLARE_OLC_FUN( medit_align	);
DECLARE_OLC_FUN( medit_spec	);


/*
 * MobProg Editor Prototypes
 */
DECLARE_OLC_FUN( mpedit_show	);
DECLARE_OLC_FUN( mpedit_add	);
DECLARE_OLC_FUN( mpedit_delete	);
DECLARE_OLC_FUN( mpedit_create	);
DECLARE_OLC_FUN( mpedit_copy	);
DECLARE_OLC_FUN( mpedit_trigger	);
DECLARE_OLC_FUN( mpedit_program	);


/*
 * Macros
 */

#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))



/*
 * Prototypes
 */

/* mem.c - memory prototypes */
#define ED		EXTRA_DESCR_DATA
RESET_DATA *		new_reset_data 	args( ( void ) );
void 			free_reset_data args( ( RESET_DATA * pReset ) );
AREA_DATA *		new_area 	args( ( void ) );
void 			free_area 	args( ( AREA_DATA * pArea ) );
EXIT_DATA *		new_exit 	args( ( void ) );
void 			free_exit 	args( ( EXIT_DATA * pExit ) );
ED *			new_extra_descr args( ( void ) );
void 			free_extra_descr 	args( ( ED * pExtra ) );
ROOM_INDEX_DATA *	new_room_index 	args( ( void ) );
void 			free_room_index args( ( ROOM_INDEX_DATA * pRoom ) );
AFFECT_DATA *		new_affect 	args( ( void ) );
void 			free_affect 	args( ( AFFECT_DATA * pAf ) );
SHOP_DATA *		new_shop 	args( ( void ) );
void 			free_shop 	args( ( SHOP_DATA * pShop ) );
OBJ_INDEX_DATA *	new_obj_index 	args( ( void ) );
void 			free_obj_index 	args( ( OBJ_INDEX_DATA * pObj ) );
MOB_INDEX_DATA *	new_mob_index 	args( ( void ) );
void 			free_mob_index 	args( ( MOB_INDEX_DATA * pMob ) );
#undef	ED

/* olc.c - miscellaneous prototypes */
int 		mprog_count args( ( MOB_INDEX_DATA * pMob ) );
MPROG_DATA *	edit_mprog args( ( CHAR_DATA * ch, MOB_INDEX_DATA * pMob ) );
void 		show_mprog args( ( CHAR_DATA * ch, MPROG_DATA * pMobProg ) );
void 		delete_mprog args( ( CHAR_DATA * ch, int pnum ) );

/* olc_act.c - miscellaneous prototypes */
MOB_INDEX_DATA *	edit_mob args( ( CHAR_DATA * ch ) );
OBJ_INDEX_DATA *	edit_obj args( ( CHAR_DATA * ch ) );
ROOM_INDEX_DATA *	edit_room args( ( CHAR_DATA * ch ) );
AREA_DATA *		edit_area args( ( CHAR_DATA * ch ) );
bool			is_builder args( ( CHAR_DATA * ch, AREA_DATA * area ) );
