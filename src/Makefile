make:
	@echo Make what?  You must clarify which system to compile ENVY.
	@echo
	@echo 'make aix	- AIX'
	@echo 'make amiga	- Amiga with TCP/IP'
	@echo 'make hpux	- HPUX'
	@echo 'make irix	- Irix'
	@echo 'make mip	- MIP'
	@echo 'make next	- NeXT'
	@echo 'make dec-osf	- DEC OSF'
	@echo 'make solaris	- Anything running Solaris'
	@echo 'make standard	- Default compile'
	@echo 'make linux	- For linux 1.2.x'
	@echo 'make linux2	- For linux 2.+'
	@echo 'make sequent	- Sequent machines'
	@echo 'make tektronix	- Tektronix machines'
	@echo
	@echo If your system does not appear in the above list, try 'make all'.
	@echo
	@echo For Windows 95/NT platforms please read README.WIN

all:	standard

clean:
	rm -f *.o envy envy2

aix:
	make -f Makefile.aix

amiga:
	make -f Makefile.ami

hpux:
	make -f Makefile.hp

mip:
	make -f Makefile.mip

next:
	make -f Makefile.nex

dec-osf:
	make -f Makefile.osf

solaris:
	make -f Makefile.sol

standard:
	make -f Makefile.std

std:
	make -f Makefile.std

linux:
	make -f Makefile.std

linux2:
	make -f Makefile.lnx

sequent:
	make -f Makefile.sys

tektronix:
	make -f Makefile.tek
