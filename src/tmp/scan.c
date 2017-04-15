#define unix 1
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"  

int scan_room (CHAR_DATA *ch, const ROOM_INDEX_DATA *room,char *buf)
{
    CHAR_DATA *target = room->people;
    int number_found = 0;

    while (target != NULL) /* repeat as long more peple in the room */
    {
        if (can_see(ch,target)) /* show only if the character can see the
target */
        {
            strcat (buf, " - ");
            strcat (buf, IS_NPC(target) ? target->short_descr :
target->name);
            strcat (buf, "\n\r");
            number_found++;
        }
        target = target->next_in_room;
    }

    return number_found;
}

void do_scan (CHAR_DATA *ch, char *argument)
{
    EXIT_DATA * pexit;
    ROOM_INDEX_DATA * room;
    extern char * const dir_name[];
    char buf[MAX_STRING_LENGTH];
    int dir;
    int distance;

    sprintf (buf, "Right here you see:\n\r");
    if (scan_room(ch,ch->in_room,buf) == 0)
        strcat (buf, "Noone\n\r");
    send_to_char (AT_BLUE, buf,ch);

    for (dir = 0; dir < 6; dir++) /* look in every direction */
    {
        room = ch->in_room; /* starting point */

        for (distance = 1 ; distance < 4; distance++)
        {
            pexit = room->exit[dir]; /* find the door to the next room */
            if ((pexit == NULL) || (pexit->to_room == NULL) ||
               (IS_SET(pexit->exit_info, EX_CLOSED)))
                break; /* exit not there OR points to nothing OR is closed
*/

            /* char can see the room */
            sprintf (buf, "%d %s from here you see:\n\r", distance,
dir_name[dir]);
            if (scan_room(ch,pexit->to_room,buf)) /* if there is something
there */
                send_to_char (AT_WHITE,buf,ch);

            room = pexit->to_room; /* go to the next room */
        } /* for distance */
    } /* for dir */
}
