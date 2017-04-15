/**************************************************************************
*  File: memory.c                                         Part of DikuMud *
*  Usage: Memory for MOBs.						  *
*  Author : Dionysos, the God of Wine. 					  *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
***************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"

int str_cmp(char *arg1, char *arg2);

/*******************************************************************/
/* FUNCTIONS                                                       */
/*******************************************************************/

void remember (char *name, struct char_data *victim)
{
   MEMtMemoryRec *new, *curr;
   bool present = FALSE;
   
   /* See if the person is already in the list (no duplicates) */

   curr = (MEMtMemoryRec *)victim->specials.memory;
   while (curr != NULL) {
      if (str_cmp(name, curr->name) == 0) {
         present = TRUE;
         break;
      }
      else {
         curr = curr->next;
      }
   } 

   if (!present) {
      CREATE(new, MEMtMemoryRec, 1);
      new->next = victim->specials.memory;
      (void)strcpy(new->name, name);
      victim->specials.memory = new;
   }

} /* remember */

/*******************************************************************/

void forget (char *name, struct char_data *victim) 
{
   MEMtMemoryRec *prev, *curr;

   prev = (MEMtMemoryRec *)victim->specials.memory;
   curr = prev;

   while ((curr != NULL) && (str_cmp(curr->name, name) != 0)) {
      prev = curr;
      curr = curr->next;
   }

   if (curr == NULL) return;
   if (curr == victim->specials.memory) {
      victim->specials.memory = curr->next;
   }
   else {
      prev->next = curr->next;
   }

   free(curr);
} /* forget */

/********************************************************************/

void clearMemory(struct char_data *ch)
{
   MEMtMemoryRec *curr, *prev;

   prev = (MEMtMemoryRec *)ch->specials.memory;
   curr = prev;

   while (prev != NULL) {
      curr = prev->next;
      free(prev);
      prev = curr;
   } 

   ch->specials.memory = NULL;
} /* clearMemory */


