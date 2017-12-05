/*
 * Mythran Mud 3.1.1
 * Mud Configuration file
 *
 * Various extra options to change the way the mud is run...
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
 * Tax can be turned on by defining TAX, to turn it off comment it.
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
 * ALL_SPEAK_COMMON
 *
 * This let's all characters learn common and speak it by default
 * untill they learn another language and speak that.
 *
 */
#define ALL_SPEAK_COMMON

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
 * Between reboots this value is stored in the file BANK.TXT
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

/*
 * Godnames on sacrificing
 *
 * If SAC_GODNAMES is defined, a player will get a different text each time
 * he sacrifices an item, if it is not defined, it say's 'The gods' instead
 * of something like 'Maniac' or 'Canth' etc.
 *
 */
#define SAC_GODNAMES	TRUE

/*
 * Different rewards for sacrificing...
 *
 * If SAC_VAR_REWARD is defined, players get a random reward for a
 * sacrifice they make, if not, they will just get the usual 1 gp
 *
 */
#define SAC_VAR_REWARD

/*
 * AUTO_WORLD_SAVE
 * Enable or disable automatic saving of all areas (every 30 minutes)
 *
 * If you define AUTO_SAVE_WORLD, the mud will save all area's every 30
 * minutes, this will cause a log of lag for 1-10 seconds (depending on
 * CPU and harddisk speed) This is handy to have in a building port, but
 * very irritating in the playing port. So it's best to have this defined
 * in the building port, and undefined in the running port.
 *
 */
#undef AUTO_WORLD_SAVE
