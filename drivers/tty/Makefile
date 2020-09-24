# Makefile for terminal driver (TTY)
DRIVER = tty

# directories
u = /usr
i = ../../include
l = ../../lib/bin
d = ../bin

# programs, flags, etc.
MAKE = exec make
CC = ${TOOLCHAIN_PREFIX}gcc
MKDEP = $(CC) -M -MG
CFLAGS = -mcpu=cortex-a9 -ffreestanding -nostdinc -fno-plt -fno-pie -I$i
LDFLAGS = -static -nostartfiles -Wl,--build-id=none -Wl,-Map=$d/$(DRIVER).map -Wl,--cref -L$l
LIBS = -lsys -lsysutil -ltimers

OBJ = tty.o pty.o rs232.o

ifeq (${ARCH}, x86)
OBJ += console.o vidcopy.o keyboard.o
endif

# build local binary 
all build:	$d/$(DRIVER)
$d/$(DRIVER):	$(OBJ) 
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LIBS)
	#install -S 256w $(DRIVER)

# install with other drivers
install:	
	cd keymaps && $(MAKE) -$(MAKEFLAGS) install

#/sbin/$(DRIVER):	$(DRIVER)
#	install -o root -cs $? $@

# clean up local files
clean:
	cd keymaps && $(MAKE) -$(MAKEFLAGS) $@
	rm -f $(DRIVER) *.o *.bak 

depend: 
	$(MKDEP) $(CFLAGS) *.c > .depend

# Include generated dependencies.
-include .depend
