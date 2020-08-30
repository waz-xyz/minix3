# Makefile for system library implementation

# Directories
i = ../../include

# Programs, flags, etc.
CC =	${TOOLCHAIN_PREFIX}gcc
CPP =	${TOOLCHAIN_PREFIX}cpp
AR =	${TOOLCHAIN_PREFIX}ar
LD =	$(CC)
MKDEP = $(CC) -M -MG
CFLAGS = -mcpu=cortex-a9 -ffreestanding -nostdinc -fno-plt -fno-pie -I$i
LDFLAGS = -nostdlib

SYSTEM	= ../system.a

# What to make.
all kernel build install: $(SYSTEM)

OBJECTS	= \
	do_unused.o \
	do_fork.o \
	do_exec.o \
	do_newmap.o \
	do_exit.o \
	do_trace.o \
	do_nice.o \
	do_times.o \
	do_setalarm.o \
	do_irqctl.o \
	do_copy.o \
	do_vcopy.o \
	do_umap.o \
	do_memset.o \
	do_privctl.o \
	do_getksig.o \
	do_endksig.o \
	do_kill.o \
	do_sigsend.o \
	do_sigreturn.o \
	do_abort.o \
	do_getinfo.o \
	do_vm.o \
	do_vm_setbuf.o

ifeq (${ARCH}, "x86")
OBJECTS += do_int86.o \
	do_devio.o \
	do_vdevio.o \
	do_sdevio.o \
	do_segctl.o \
	do_iopenable.o
endif

$(SYSTEM):	$(OBJECTS)
	$(AR) cr $@ *.o

clean:
	rm -f $(SYSTEM) *.o *~ *.bak 

depend: 
	$(MKDEP) $(CFLAGS) *.c > .depend

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

# Include generated dependencies.
-include .depend
