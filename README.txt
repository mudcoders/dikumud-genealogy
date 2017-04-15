Merc Release 1.0
Friday 18 December 1992

Kahn	michael@web.berkeley.edu
Hatchet	mitchell@ocf.berkeley.edu
Furey	mec@garnet.berkeley.edu



=== Copyright and Disclaimer

Thank you for choosing Merc Mud!

Merc Mud is a Diku Mud with many enhancements and contributions.
See our credits.txt and wizlist.txt for acknowledgements to all
our contributors.  Maybe someday you'll be in there too!

Merc Mud is Copyright 1992, 1993 by Michael Chastain, Michael
Quan, and Mitchell Tse.  You can use our stuff in any way you
like whatsoever so long as our copyright notices remain intact.
If you like it, please drop a line to mec@garnet.berkeley.edu
or michael@web.berkeley.edu.

This is free software and you are benefitting.  We hope that you
share your changes too.  What goes around, comes around.

Original Diku Mud is copyrighted by the Diku folks, and their
license is in doc_diku/license.doc.



=== What's in This Release

This is our first release, and we didn't write a bunch of docs
for it.  Briefly, here's what's included:

	doc_diku	The original diku docs.
	doc_merc	Our merc docs.
	lib		Tinyworld and auxiliary files.
	log		Game logs.
	save		Character save files.
	src		All the source.

The log and save directories are initially empty.



=== How to Install

First, group with your highest level Unix guru, and have him or her
tank.  Merc is easier to install than other muds, but every site is
different.

(1) Get the release merc_10.tar.Z from one of these fine ftp sites:

	ucbvax.berkeley.edu
	ferkel.ucsb.edu
	wuarchive.wustl.edu

(2) Unpack the release:

	uncompress merc_10.tar.Z
	tar -xvf merc_10.tar

(3) Go into the src directory.  Edit the Makefile to reflect your
    local C compiler name options.  We use "gcc -O -c -Wall".
    Note: the source uses ansi C prototypes.

(4) Run make.  This recompiles everything:

	make -k >&! make.out

(5) Fix all the errors listed in make.out.  These will mostly be
    incompatible library declarations.  Our declarations are for
    a Sun 4 running SunOS 4.1.2 with gcc and a Dec 5900 running
    Ultrix and GCC.  You may have to fix them up for your own system.

(6) Run "merc -g 9000 &" to start the game on port 9000 in god creation
    mode.  Everyone who creates a char will be a god!

(7) Shut the game down when you have enough gods.  :)  Use the
    "startup" script to start the game for normal operation.

(8) Drop us some e-mail at one of these addresses.  Tell us how you
    like it!

	mec@garnet.berkeley.edu
	michael@web.berkeley.edu
	mitchell@ocf.berkeley.edu

(9) Of course you're going to change the title screen, info files,
    and so on.  Don't just erase all the "Merc" references everywhere,
    ok?  Leave us some lines of glory in the title screen and elsewhere.



=== KNOWN PROBLEMS

The classes are not fully balanced, and the game is somewhat unbalanced
in favor of mobs.  :)

There are other minor things, but they're not going to stop you from
running Merc mud and having hundreds of mud-crazed players show up
every night!

If you find something bad, and fix it, or if you add something new and
wonderful to the code or the worlds, we'd appreciate a copy.  We just
gave you 30,000 lines of fast, tight, reliable code and a megabyte of
worlds ... now it's your turn.  Join in and help make Merc the best mud
on the planet!  Of course, we credit all contributions ... look at the
credits we already have.



=== OUR FUTURE PLANS

We're still developing Merc.  We are releasing again on June 1, 1993.
We might release before then, depending on our mood and how many
contributions we get.
