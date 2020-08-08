# Makefile for the init program (INIT)
SERVER = init

# directories
u = /usr
i = ../../include
s = $i/sys
h = $i/minix
k = $u/src/kernel
l = ../../lib/bin
d = ../bin

# programs, flags, etc.
CC = ${TOOLCHAIN_PREFIX}gcc
MKDEP = $(CC) -M -MG
CFLAGS = -ffreestanding -nostdinc -fno-plt -fno-pie -I$i -D_POSIX_SOURCE -D_MINIX
LDFLAGS = -static -nostartfiles -Wl,--build-id=none -Wl,-Map=$d/$(SERVER).map -Wl,--cref -L$l

OBJ = 	init.o

# build local binary
all build:	$d/$(SERVER)
$d/$(SERVER):	$(OBJ)
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $(OBJ) -lsysutil
	#install -S 192w $@

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
