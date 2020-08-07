# Makefile for kernel

# Directories
i = ../include
l = ../lib/bin
s = system

# Programs, flags, etc.
CC =	${TOOLCHAIN_PREFIX}gcc
CPP =	${TOOLCHAIN_PREFIX}cpp
LD =	$(CC)
MKDEP = $(CC) -M -MG
CFLAGS = -ffreestanding -nostdinc -fno-plt -fno-pie -I$i
LDFLAGS = -static -nostartfiles -nostdlib -T kernel.ld -Wl,--build-id=none -Wl,-Map=kernel.map -Wl,--cref -Wl,--gc-sections -L$l

HEAD =	mpx.o
OBJS =	klib.o table.o kprintf.o proc.o \
	system.o utility.o debug.o \
	serial.o
ifeq (${ARCH}, x86)
OBJS += main_x86.o exception_x86.o protect.o interrupts_x86.o clock_x86.o start_x86.o
endif
ifeq (${ARCH}, arm)
OBJS += main_armv7.o exception_armv7.o interrupts_armv7.o clock_armv7.o start_armv7.o
endif
SYSTEM = system.a
LIBS = -lgcc -lc -ltimers

# What to make.
all: build 
kernel build install: $(HEAD) $(OBJS) 
	cd system && $(MAKE) -$(MAKEFLAGS) $@
	$(LD) $(LDFLAGS) -o kernel.elf $(HEAD) $(OBJS) $(SYSTEM) $(LIBS)
	#install -S 0 kernel.elf

clean:
	cd system && $(MAKE) -$(MAKEFLAGS) $@
	rm -f *.a *.o *~ *.bak kernel.elf

depend:
	cd system && $(MAKE) -$(MAKEFLAGS) $@
	$(MKDEP) $(CFLAGS) *.c *.S > .depend

# How to build it
.S.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

# Include generated dependencies.
-include .depend

