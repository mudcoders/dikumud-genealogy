# Generic Makefile.  Read 'port.txt' in the documentation
# directory for the Makefile that is right for you  - Kahn

CC      = gcc
PROF    =
DEBUG   = -g
NOCRYPT =
O_FLAGS = -O
C_FLAGS = $(O_FLAGS) -Wall $(DEBUG) $(PROF) $(NOCRYPT)
L_FLAGS = $(O_FLAGS)                $(PROF)
LIBS    = -lcrypt

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
          db.o fight.o handler.o interp.o magic.o save.o special.o update.o

envy: $(O_FILES)
	rm -f envy
	$(CC) $(L_FLAGS) -o envy $(O_FILES) $(LIBS)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm -f *.o envy envy2
