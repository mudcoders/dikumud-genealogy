# Makefile for Linux, based on the Solaris 2.4 for x86 makefile
# Changes contributed by Izuskin Kromosoeto and The Maniac
# Makefile for linux, written by The Maniac
# DEBUG -ggdb
# PROF -pg

CC      = gcc
PROF    =
DEBUG   =
O_FLAGS = -O2 -g3
C_FLAGS = $(O_FLAGS) -Wall $(DEBUG) $(PROF)
L_FLAGS = $(DEBUG) $(PROF) -lcrypt

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
          db.o fight.o handler.o interp.o magic.o save.o special.o update.o \
          healer.o blade.o act_wiz2.o scan.o games.o hunt.o drunk.o quest.o \
          brewscribe.o language.o social-edit.o economy.o clan.o \
          mob_commands.o mob_prog.o wiznet.o \
          necromancer.o bit.o mem.o olc.o olc_act.o olc_save.o string.o

envy: $(O_FILES) merc.h
	rm -f envy2
	$(CC) -o envy2 $(O_FILES) $(L_FLAGS)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm -f *.o envy2 core

proper:
	rm -f envy2 core $(O_FILES) *.out
