PROF =
C_FLAGS = -O -Wall $(PROF)
L_FLAGS = -O $(PROF)
LIBS = -lcrypt

O_FILES = act_comm.o act_info.o act_move.o act_o1.o act_o2.o act_off.o \
          act_oth.o act_soc.o act_wiz.o board.o changes.o comm.o       \
          const.o db.o fight.o fount.o handler.o interp.o limits.o     \
          magic.o mob_act.o modify.o nanny.o save.o shop.o             \
          spec_ass.o spec_pro.o spells.o utility.o weather.o

merc: $(O_FILES)
	rm -f merc
	gcc $(L_FLAGS) -o merc $(O_FILES) $(LIBS)

.c.o:
	gcc -c $(C_FLAGS) $<
