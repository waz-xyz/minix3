# Makefile for memory driver (MEMORY)
DRIVER = memory

# directories
u = /usr
i = ../../include
l = ../../lib/bin
s = $i/sys
m = $i/minix
b = $i/ibm
d = ../bin

# programs, flags, etc.
MAKE = exec make
CC = ${TOOLCHAIN_PREFIX}gcc
MKDEP = $(CC) -M -MG
CFLAGS = -mcpu=cortex-a9 -ffreestanding -nostdinc -fno-plt -fno-pie -I$i
LDFLAGS = -static -nostartfiles -Wl,--build-id=none -Wl,-Map=$d/$(DRIVER).map -Wl,--cref -L$l
LIBS = -lsys -lsysutil

# imgrd_s.s is the ACK assembler version of the ramdisk. For more portability,
# use the C version imgrd.c. However, the C compiler takes too much memory
# compiling imgrd.c.
#IMGRD=imgrd_s.o
IMGRD=imgrd.o

OBJ = memory.o $(IMGRD)
LIBDRIVER = $d/driver.o


# build local binary
all build:	$d/$(DRIVER)

$d/$(DRIVER):	ramdisk_image $(OBJ) $(LIBDRIVER)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LIBDRIVER) $(LIBS)
	#install -S 8k $(DRIVER)

imgrd.o:        ramdisk/image.c
imgrd_s.o:      ramdisk/image.s

$(LIBDRIVER): 
	cd ../libdriver && $(MAKE) 

ramdisk_image:
	cd ramdisk && make

# install with other drivers
install: /usr/sbin/$(DRIVER)
/usr/sbin/$(DRIVER):	$(DRIVER)
	install -o root -cs $? $@

# clean up local files
clean:
	rm -f $d/$(DRIVER) *.o *.bak 
	cd ramdisk && make clean

depend: 
	$(MKDEP) $(CFLAGS) memory.c ../libdriver/*.c > .depend
	cd ramdisk && make depend

# Include generated dependencies.
-include .depend
