/*
 * Mythran Mud 3.1
 * Mud Configuration file
 *
 * Various extra options to change the way the mud is run...
 * Not much here yet, but it's a start...
 *
 * Well the list is growing, default all options are on, only
 * pfile compression is standard off, and there are still some
 * bugs in it... Best is to manually compress the pfiles,
 * the mud does automatic decompression (with .gz files)
 *
 * Maniac
 *
 */

/*
 * Taxes.
 *
 * Players with a lot of money (250.000 +) can be taxed,
 * This is a great way for implementors to reduce the amount
 * of money in the game and reduce the power of the rich players
 * Tax is currently 1 tenth of the amount of money above 250000
 * Tax can be turned on by defining TAX, to turn it of comment it.
 */

#define TAX			/* Taxes are on by default */

/*
 * Initial Language.
 *
 * Since there are now languages in the game, it is possible
 * to let the new players automatically learn one language,
 * this is the language that comes with their race by nature.
 * To enable this feature define AUTOLEARN, comment it to turn it off.
 */

#define AUTOLEARN

/*
 * Force the use of languages.
 *
 * This option makes the say command automatically use the language
 * a person is speaking, and not the usual common language.
 * Other channels still use normal (read 'no') language.
 *
 */

#define FORCE_LANGUAGE

/*
 * Drunk System
 *
 * By defining USE_DRUNK you enable the funny drunk-system.
 * Try for yourself what funny uses it has...
 *
 */

#define USE_DRUNK

/*
 * Bank system money transfer option
 *
 * Defining BANK_TRANSFER allows the players to transfer money from
 * their bank account to someone else's account
 *
 */

#define BANK_TRANSFER

/*
 * Mud Economy System
 * Bank Investments
 *
 * Defining BANK_INVEST allows players to invest their money in a
 * mud fund, the value of the fund may change from time to time
 * making it possible for players to speculate on a random generator
 * (or if you want to call it this way: the economy)
 *
 * Define SHARE_VALUE to be the start/average/default value of a share (100)
 */

#define BANK_INVEST
#define SHARE_VALUE	100

/*
 * Automatic compression of playerfiles
 *
 * By defining AUTO_COMPRESS the player files will automatically
 * be compressed using the command 'gzip --best <pfile>'
 * If you don't have gzip or don't want to compress your pfiles
 * undefine this option NOW.
 * If you are low on disk space leave it untouched.
 *
 */

/* #define AUTO_COMPRESS */

/*
 * Configuring the Mythran Mud Game System
 *
 * There are a few defines for the game system, BET_LIMIT and BET_LIMIT_STR
 * the BET_LIMIT is the maximum amount that can be used in betting games,
 * BET_LIMIT_STR is a string with the amount in it.
 * GAME_CHEAT can be defined or undefined to enable or disable the possibility
 * of cheating mobs, if GAME_CHEAT is defined, it doesn't mean the mobs will
 * cheat, only that it is possible (if they have the cheat bit set)
 *
 */

#define BET_LIMIT	5000
#define BET_LIMIT_STR	"5000"
#define GAME_CHEAT	TRUE
