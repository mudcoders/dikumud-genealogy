# Wedit Makefile for project comm
SRCDIR=d:\cygwin\home\unknown\eos\eos\src\tmp
CFLAGS=-IC:\db  -g2 
CC=D:\lcc\bin\lcc.exe
LINKER=D:\lcc\bin\lcclnk.exe
OBJS=\
	comm.obj

LIBS=

comm.exe:	$(OBJS) Makefile
	$(LINKER)  -subsystem console -o $(SRCDIR)\lcc\comm.exe $(OBJS) $(LIBS)

# Build comm.c
COMM_C=\
	$(SRCDIR)\merc.h\
	$(SRCDIR)\colors.h\

comm.obj: $(COMM_C) $(SRCDIR)\comm.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\comm.c

link:
	$(LINKER)  -subsystem console -o $(SRCDIR)\lcc\comm.exe $(OBJS) $(LIBS)

