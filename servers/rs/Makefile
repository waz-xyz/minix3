# Makefile for Reincarnation Server (RS)
SERVER = rs
UTIL = service

# directories
u = /usr
i = ../../include
s = $i/sys
m = $i/minix
b = $i/ibm
l = ../../lib/bin
d = ../bin

# programs, flags, etc.
CC = ${TOOLCHAIN_PREFIX}gcc
MKDEP = $(CC) -M -MG
CFLAGS = -ffreestanding -nostdinc -fno-plt -fno-pie -I$i
LDFLAGS = -static -nostartfiles -Wl,--build-id=none -L$l
UTIL_LIBS = -lsys 
LIBS = -lsys -lsysutil 

UTIL_OBJ = service.o
OBJ = main.o manager.o 

# build local binary
all build:	$d/$(SERVER) $d/$(UTIL)
$d/$(UTIL):	$(UTIL_OBJ)
	$(CC) -o $@ $(LDFLAGS) -Wl,-Map=$d/$(UTIL).map -Wl,--cref $(UTIL_OBJ) $(UTIL_LIBS)
$d/$(SERVER):	$(OBJ)
	$(CC) -o $@ $(LDFLAGS) -Wl,-Map=$d/$(SERVER).map -Wl,--cref $(OBJ) $(LIBS)
	#install -S 16k $@

# install with other servers
install:	/bin/$(UTIL) /usr/sbin/$(SERVER)
/bin/$(UTIL):	$(UTIL)
	install -c $? $@
/usr/sbin/$(SERVER):	$(SERVER)
	install -o root -c $? $@

# clean up local files
clean:
	rm -f $d/$(UTIL) $d/$(SERVER) $d/$(UTIL).map $d/$(SERVER).map *.o *.bak 

depend: 
	$(MKDEP) $(CFLAGS) *.c > .depend

# Include generated dependencies.
-include .depend

