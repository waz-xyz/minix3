# Makefile for driver library

# Directories
u = /usr
i = ../../include
s = $i/sys
b = $i/ibm
m = $i/minix
l = ../../lib/bin
d = ../bin

# Programs, flags, etc.
CC = ${TOOLCHAIN_PREFIX}gcc
CFLAGS = -mcpu=cortex-a9 -ffreestanding -nostdinc -fno-plt -fno-pie -I$i
# LDFLAGS = -static -nostartfiles -Wl,--build-id=none -Wl,-Map=$d/$(SERVER).map -Wl,--cref -L$l
# LIBS = -lsysutil -lsys

OBJECTS = driver.o drvlib.o

all build install: $(OBJECTS)
	cp *.o $d
	
# $(CC) -c $@ $(LDFLAGS) $(OBJ) $(LIBS)

clean:
	rm -f $(OBJECTS)

depend: 
	$(MKDEP) $(CFLAGS) *.c > .depend

# Include generated dependencies.
-include .depend
