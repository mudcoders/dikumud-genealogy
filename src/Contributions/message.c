>From morpheus@teleport.com Wed Mar 15 12:23:15 1995
Received: from desiree.teleport.com by uclink.berkeley.edu (8.6.9/1.33(web)-OV4)
	id MAA29645; Wed, 15 Mar 1995 12:23:14 -0800
Received: from [204.119.60.144] (ip-pdx3-16.teleport.com [204.119.60.144]) by desiree.teleport.com (8.6.10/8.6.9) with SMTP id MAA26673 for <michael@uclink.berkeley.edu>; Wed, 15 Mar 1995 12:23:08 -0800
Date: Wed, 15 Mar 1995 12:23:08 -0800
Message-Id: <199503152023.MAA26673@desiree.teleport.com>
X-Sender: morpheus@mail.teleport.com (Unverified)
Mime-Version: 1.0
Content-Type: text/plain; charset="us-ascii"
To: michael@uclink.berkeley.edu
From: morpheus@teleport.com (Morpheus)
Subject: message again
Status: R

Like I said in the previous email I wrote please look this over for bugs
I'm sure there are some obvious ones, but as opposed to the obvious
ones it should be pretty clean heh.  So if you could glance it over and tell me
what to fix, and why (if you want to help me learn to code better), that would
be tres cool.  Thanx...

void do_message( CHAR_DATA *ch, char *argument )
{
    char         buf  [ MAX_STRING_LENGTH ];
    char         buf1 [ MAX_STRING_LENGTH ];
    char arg  [ MAX_INPUT_LENGTH ];
    char arg1 [ MAX_INPUT_LENGTH ];

    if ( IS_NPC( ch ) )
        return;

    argument = one_argument( argument, arg  );
    argument = one_argument( argument, arg1 );

if ( arg[0] == '\0' )
    {
        send_to_char( "Format:\n\r", ch );
      send_to_char( "message <message>\n\r", ch);
      send_to_char( "or\n\r",ch );
      send_to_char( "message title <title>\n\r",ch);
        return;
    }

if ( !str_cmp( arg, "title" ) )
{
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Change your title to what?\n\r", ch );
        return;
    }

    if ( strlen( argument ) > 50 )
        argument[50] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "You are now:\n\r", ch );
    buf1[0] = '\0';
    sprintf( buf, "%s %s.\n\r", ch->name,
        IS_NPC( ch ) ? "" : ch->pcdata->title );
    strcat( buf1, buf );
 }
else

    if ( strlen( argument ) > 50 )
        argument[50] = '\0';

    smash_tilde( argument );
    set_message( ch, argument );
    send_to_char( "Your message is:\n\r", ch );
    buf1[0] = '\0';
    sprintf( buf, "%s.\n\r",
            ch->pcdata->message ? ch->message : "(None).\n\r" );
    strcat( buf1, buf );
    return;
}


...

|\/| ___  ___  ___  |  | ___ |  | ____    morpheus@teleport.com
|  | |  | | _/ |__| |--| |__ |  | |___    RL: Dave Lerner
|  | |__| |  \ |    |  | |__ |__| ____|   "One lone-ly beast-ie I be"

WWW Page: http://www.teleport.com/~morpheus/index.html



