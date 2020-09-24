# Makefile for log driver
DRIVER = log

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
LIBS = -lsys -lsysutil

OBJ = log.o diag.o kputc.o
LIBDRIVER = $d/driver.o

# build local binary
all build:	$d/$(DRIVER)
$d/$(DRIVER):	$(OBJ) $(LIBDRIVER)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LIBDRIVER) $(LIBS)
	#install -S 4kb $(DRIVER)

$(LIBDRIVER): 
	cd ../libdriver && $(MAKE) 

# install with other drivers
install:	$d/$(DRIVER)
	install -o root -cs $? /sbin/$(DRIVER) 

# clean up local files
clean:
	rm -f $d/$(DRIVER) *.o *.bak 


depend: 
	$(MKDEP) $(CFLAGS) *.c ../libdriver/*.c > .depend

# Include generated dependencies.
-include .depend
