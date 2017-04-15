/*
 * Instructions :
 *   1. Add ATM to the object file. (Obj # must be same as in ATMisPresent()
 *   2. Add to the zone file. (wherever you want to have one)
 *   3. Add do_deposit, do_withdraw, and do_balance to interpreter.c
 *   4. Create an empty file in the lib directory called : "atm.accounts"
 *  Note : I added it to zone already, Swiftest.
*** Object definition ***
#3097
machine atm~
an ATM machine~
An ATM machine is here.~
~
13 0 0
0 0 0 0
0 0 0
E
machine~
Welcome to the First Diku Bank.   You may DEPOSIT <value>,
WITHDRAW <value>, or get your account BALANCE.
Have a nice day.
~
E
atm~
Welcome to the First Diku Bank.   You may DEPOSIT <value>,
WITHDRAW <value>, or get your account BALANCE.
Have a nice day.
~
***** End of definition *****
*/

#include <strings.h>
#include <stdio.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"

#define ATM_FILE "atm.accounts"    /* File for bank */

typedef struct ATMtAccount {
   char name[20];
   long balance;
} ATMtAccount;

extern struct room_data *world;
extern int log_all;
int real_object(int i);
void log(char *str);

void ATMwithdraw(struct char_data *ch, char *arg, int cmd);
void ATMdeposit(struct char_data *ch, char *arg, int cmd);
void ATMbalance(struct char_data *ch, char *arg, int cmd);

/**********************************************************************/

int ATMisPresent(struct char_data *ch)
{
   struct obj_data *tmp;
   bool found = FALSE;

   for (tmp = world[ch->in_room].contents;
        tmp != NULL && !found;
        tmp = tmp->next_content) {
      if ((tmp->item_number == real_object(3097)) && (CAN_SEE_OBJ(ch, tmp))) {
         found = TRUE;
      } /* if */
   } /* for */

   return found;

} /* ATMisPresent */

/*******************************************************************/

void do_deposit(struct char_data *ch, char *argument, int cmd)
{
   if (ATMisPresent(ch)) {
      ATMdeposit(ch, argument, cmd);
   }
   else {
      send_to_char("There's no ATM here!\n\r", ch);
   } /* if */

} /* do_deposit */

/*******************************************************************/

void do_balance(struct char_data *ch, char *argument, int cmd)
{
   if (ATMisPresent(ch)) {
      ATMbalance(ch, argument, cmd);
   }
   else {
      send_to_char("There's no ATM here!\n\r", ch);
   } /* if */

} /* do_balance */

/*******************************************************************/

void do_withdraw(struct char_data *ch, char *argument, int cmd)
{
   if (ATMisPresent(ch)) {
      ATMwithdraw(ch, argument, cmd);
   }
   else {
      send_to_char("There's no ATM here!\n\r", ch);
   } /* if */

} /* do_withdraw */

/*****************************************************************/

int ATMgetUser(char *name, ATMtAccount *acc)
{
   FILE *fp;
   int offset = 0;


   if ((fp = fopen(ATM_FILE, "r")) == NULL) {
      log("Error opening ATM_FILE!");
      exit(0);
   } /* if */

   while (!feof(fp)) {
      fread(acc, sizeof(ATMtAccount), 1, fp);
      if ((str_cmp(acc->name, name) == 0) && (!feof(fp))) {
         /* Found the player, return this record */
         offset = ftell(fp) - (int)sizeof(ATMtAccount);
	 fclose(fp);
         return offset;
      } /* if */
   } /* while */

   /*
    * If the person is not in the bank file, then create an entry.
    */
   strcpy(acc->name, name);
   acc->balance = 0;

   fseek(fp, 0, 2);   /* Seek to end of file */
   offset = ftell(fp);
   fwrite(acc, sizeof(ATMtAccount), 1, fp);
   fclose(fp);

   return offset;

} /* ATMgetUser */

/*****************************************************************/

void ATMputUser(int offset, ATMtAccount *acc)
{
   FILE *fp;
   bool found = FALSE;
   ATMtAccount temp;


   if ((fp = fopen(ATM_FILE, "r+")) == NULL) {
      log("Error opening ATM_FILE!");
      exit(0);
   } /* if */

   fseek(fp, offset, 0);
   fwrite(acc, sizeof(ATMtAccount), 1, fp);
   fclose(fp);

   return;

} /* ATMputUser */

/*****************************************************************/


void ATMbalance(struct char_data *ch, char *arg, int cmd)
{
   ATMtAccount acc;
   char buf[MAX_INPUT_LENGTH];

   (void)ATMgetUser(ch->player.name, &acc);
   if (acc.balance == 0) {
      sprintf(buf, "Your balance is a big fat zero!\n\r");
   }
   else if (acc.balance != 1) {
      sprintf(buf,"Your balance is %ld coins.\n\r", acc.balance);
   }
   else {
      sprintf(buf,"Your balance is %ld coin.\n\r", acc.balance);
   } /* if */

   send_to_char (buf, ch);

} /* ATMbalance */

/*****************************************************************/

void ATMdeposit(struct char_data *ch, char *arg, int cmd)
{
   ATMtAccount acc;
   char buf[MAX_INPUT_LENGTH];
   char number[MAX_INPUT_LENGTH];
   char logit[MAX_INPUT_LENGTH];
   long amount;
   int offset;

   offset = ATMgetUser(ch->player.name, &acc);

   if(log_all){
	   sprintf(logit,"WIZINFO: (%s) deposit %s",GET_NAME(ch),arg);
	   log(logit);
   }

   one_argument(arg, number);
   if (!*number) {
      send_to_char("Usage :: DEPOSIT <value>\n\r", ch);
      return;
   } /* if */

   if (strlen(number)>7){ /* Was getting overflow...fix--Swiftest */
      send_to_char("Usage :: DEPOSIT <value>\n\r",ch);
      return;
   }

   if (!(amount = atol(number))) {
      send_to_char("Usage :: DEPOSIT <value>\n\r", ch);
      return;
   } /* if */

   if (amount < 0) {
      send_to_char("The amount to deposit must be non-negative.\n\r",ch);
      return;
   } /* if */

   if (amount > GET_GOLD(ch)) {
      send_to_char("You don't have that much gold.\n\r", ch);
   }
   else {
      acc.balance += amount;
      GET_GOLD(ch) -= amount;
      ATMputUser(offset, &acc);

      if (amount == 1) {
         sprintf(buf, "You deposit 1 coin.\n\r");
      }
      else if (amount == 0) {
         sprintf(buf, "A most unproductive effort.\n\r");
      }
      else {
         sprintf(buf, "You deposit %ld coins.\n\r", amount);
      } /* if */

      send_to_char(buf, ch);
   } /* if */

} /* ATMdeposit */

/*****************************************************************/

void ATMwithdraw(struct char_data *ch, char *arg, int cmd)
{
   ATMtAccount acc;
   char buf[MAX_INPUT_LENGTH];
   char number[MAX_INPUT_LENGTH];
   char logit[MAX_INPUT_LENGTH];
   long amount;
   int offset;

   offset = ATMgetUser(ch->player.name, &acc);

   if(log_all){
	   sprintf(logit,"WIZINFO: (%s) withdraw %s",GET_NAME(ch),arg);
	   log(logit);
   }

   one_argument(arg, number);
   if (!*number) {
      send_to_char("Usage :: WITHDRAW <value>\n\r", ch);
      return;
   } /* if */

   if (strlen(number)>7){ /* Was getting overflow error--Swiftest */
      send_to_char("Usage :: WITHDRAW <value>\n\r",ch);
      return;
   } /* if */

   if (!(amount = atol(number))) {
      send_to_char("Usage :: WITHDRAW <value>\n\r", ch);
      return;
   } /* if */

   if (amount < 0) {
      send_to_char("The amount to withdraw must be non-negative.\n\r",ch);
      return;
   } /* if */

   if (amount > acc.balance) {
      send_to_char("You don't have that much gold in your account.\n\r", ch);
   }
   else {
      acc.balance -= amount;
      GET_GOLD(ch) += amount;
      ATMputUser(offset, &acc);

      if (amount == 1) {
         sprintf(buf, "You withdraw 1 coin.\n\r");
      }
      else if (amount == 0) {
         sprintf(buf, "A most unproductive effort.\n\r");
      }
      else {
         sprintf(buf, "You withdraw %ld coins.\n\r", amount);
      } /* if */

      send_to_char(buf, ch);
   } /* if */

} /* ATMwithdraw */

/*****************************************************************/
