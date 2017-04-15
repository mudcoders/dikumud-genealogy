From wildcat@vantek.net  Wed Jun 19 04:28:49 1996
Received: from vtcom3.vantek.net (vtcom3.vantek.net [205.161.186.2]) by soda.CSUA.Berkeley.EDU (8.6.12/8.6.12) with ESMTP id VAA21386 for <michael@CSUA.Berkeley.EDU>; Tue, 18 Jun 1996 21:28:46 -0700
Received: from wildcat ([205.161.186.151]) by vtcom3.vantek.net (8.6.9/8.6.9) with SMTP id WAA11798 for <michael@CSUA.Berkeley.EDU>; Tue, 18 Jun 1996 22:24:20 -0500
Message-ID: <31C77465.2CFF@vantek.net>
Date: Tue, 18 Jun 1996 22:30:45 -0500
From: Bryan Cromartie <wildcat@vantek.net>
Reply-To: wildcat@vantek.net
X-Mailer: Mozilla 3.0B2 (Win95; I)
MIME-Version: 1.0
To: Michael Quan <michael@CSUA.Berkeley.EDU>
Subject: Scan Command contribution for Envy 2.2
Content-Type: text/plain; charset=us-ascii
Content-Transfer-Encoding: 7bit
Status: RO

Greetings,

	My name is Bryan Cromartie in rl. I have not only enjoyed playing Merc
throughout it's original incarnations, but well into its release as
Envy 1 and 2. Given the imminent release of Envy 2.2, I would like to
contribute a custom scan command I created for Glass Dragon Mud. It
handles everything I can thing of and can be easily tweaked if you'd
like to modify it. It has been thoroughly tested with Merc2.2 and Envy
1 & 2.
	It checks carefully in all directions for exits, pcs/npcs, and informs
you of closed doors and distances. It is currently set to scan 4 hexes
away if memory serves me, and may be controlled for distance through
the parse variable.

Respectfully,
Bryan Cromartie aka Martek of Glass Dragon

Scan command as follows ( declare function in merc.h and command entry
in interp.c )

/* Scan command coded by Martek of Glass Dragon */
void do_scan( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;
    ROOM_INDEX_DATA *toRoom;
    int d, parse;
    bool nexit;
    bool seen;
    char buf[MAX_STRING_LENGTH];
    char *const exitname[] =
	{ "North", "East", "South", "West", "Up", "Down" };

    /* Look in all directions and assume we have no exits until we find one */
    nexit = TRUE;
    for (d = 0; d <= 5; d++)
    {
	pRoom = ch->in_room;
        if ( pRoom == NULL
	|| pRoom->exit[d] == NULL
	|| (toRoom = pRoom->exit[d]->to_room) == NULL )
	    continue;
	nexit = FALSE;

        sprintf(buf, "%s:", exitname[d]);
        send_to_char(buf, ch);

        /* We found an exit, so let's look in that direction */
        for (parse = 1; parse <= 4 ; parse++)
        {
            if (pRoom->exit[d] == NULL
	    || (toRoom = pRoom->exit[d]->to_room) == NULL )
                break;

	    /* We can't look through closed doors
	     * We stop here if we find one */
	    if ( IS_SET(pRoom->exit[d]->exit_info, EX_CLOSED) )
	    {
		sprintf( buf, " Closed Door (%d away).\n\r", parse - 1 );
		send_to_char( buf, ch );
		break;
	    }

	    /* If the room is valid we show the player who's inside */
            pRoom = pRoom->exit[d]->to_room;
	    seen = FALSE;
            if ( pRoom != NULL && pRoom->people !=NULL)
            {
		CHAR_DATA *rch;

		for ( rch = pRoom->people; rch != NULL;
		      rch = rch->next_in_room )
		{
		    if ( !can_see( ch, rch ) )
			continue;

		    seen = TRUE;
		    sprintf( buf, " %s,", PERS( rch, ch ) );
		    send_to_char( buf, ch );
		}

	        /* Only show the player who's inside if someone there */
		if ( seen )
		{
		    sprintf( buf, " (%d away).\n\r", parse );
		    send_to_char( buf, ch );
		}
            }
        }
    }

    /* Inform the character of an isolated room */
    if( nexit )
	send_to_char( "This room has no exits!\n\r", ch );

    return;
}
