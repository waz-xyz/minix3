# Makefile for Data Store Server (DS)
SERVER = ds

# directories
u = /usr
i = ../../include
s = $i/sys
m = $i/minix
b = $i/ibm
k = $u/src/kernel
p = $u/src/servers/pm
f = $u/src/servers/fs
l = ../../lib/bin
d = ../bin

# programs, flags, etc.
CC = ${TOOLCHAIN_PREFIX}gcc
MKDEP = $(CC) -M -MG
CFLAGS = -mcpu=cortex-a9 -ffreestanding -nostdinc -fno-plt -fno-pie -I$i
LDFLAGS = -static -nostartfiles -Wl,--build-id=none -Wl,-Map=$d/$(SERVER).map -Wl,--cref -L$l
LIBS = -lsys -lsysutil 

OBJ = main.o store.o 

# build local binary
all build:	$d/$(SERVER)
$d/$(SERVER):	$(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LIBS)
	#install -S 16k $@

# install with other servers
install: $(SERVER)
	install -o root -c $? /sbin/$(SERVER)
#	install -o root -cs $? $@

# clean up local files
clean:
	rm -f $d/$(SERVER) $d/$(SERVER).map *.o *.bak 

depend: 
	$(MKDEP) $(CFLAGS) *.c > .depend

# Include generated dependencies.
-include .depend
