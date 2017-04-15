CC      = gcc
PROF    =
NOCRYPT =
C_FLAGS = -O -Wall $(PROF) $(NOCRYPT)
L_FLAGS = -O $(PROF)
LIBS    = -lcrypt

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
          db.o db2.o fight.o handler.o healer.o interp.o magic.o save.o \
          skills.o special.o update.o

merc: $(O_FILES)
	rm -f merc
	$(CC) $(L_FLAGS) -o merc $(O_FILES) $(LIBS)

.c.o: merc.h
	$(CC) -c -g $(C_FLAGS) $<
