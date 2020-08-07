# Makefile for Process Manager (PM)
SERVER = pm

# directories
u = /usr
i = ../../include
s = $i/sys
h = $i/minix
l = ../../lib/bin
d = ../bin

# programs, flags, etc.
CC = ${TOOLCHAIN_PREFIX}gcc
MKDEP = $(CC) -M -MG
CFLAGS = -ffreestanding -nostdinc -fno-plt -fno-pie -I$i
LDFLAGS = -static -nostartfiles -Wl,--build-id=none -Wl,-Map=$d/$(SERVER).map -Wl,--cref -L$l

OBJ = 	main.o forkexit.o break.o exec.o time.o timers.o \
	signal.o alloc.o utility.o table.o trace.o getset.o misc.o

# build local binary
all build:	$d/$(SERVER)
$d/$(SERVER):	$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) -lsys -lsysutil -ltimers
	#install -S 256w $@

# install with other servers
install:	/usr/sbin/$(SERVER)
/usr/sbin/$(SERVER):	$(SERVER)
	install -o root -cs $? $@

# clean up local files
clean:
	rm -f $d/$(SERVER) $d/$(SERVER).map *.o *.bak 

depend: 
	$(MKDEP) $(CFLAGS) *.c > .depend

# Include generated dependencies.
-include .depend
