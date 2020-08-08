# Makefile for File System (FS)
SERVER = fs

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
CFLAGS = -ffreestanding -nostdinc -fno-plt -fno-pie -I$i $(EXTRA_OPTS)
LDFLAGS = -static -nostartfiles -Wl,--build-id=none -Wl,-Map=$d/$(SERVER).map -Wl,--cref -L$l
LIBS = -lsys -lsysutil -ltimers

OBJ =	main.o open.o read.o write.o pipe.o dmap.o \
	device.o path.o mount.o link.o super.o inode.o \
	cache.o cache2.o filedes.o stadir.o protect.o time.o \
	lock.o misc.o utility.o select.o timers.o table.o

# build local binary 
install all build:	$d/$(SERVER)
$d/$(SERVER):	$(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LIBS)
	#install -S 512w $@

# clean up local files
clean:
	rm -f $d/$(SERVER) $d/$(SERVER).map *.o *.bak 

depend: 
	$(MKDEP) $(CFLAGS) *.c > .depend

# Include generated dependencies.
-include .depend
