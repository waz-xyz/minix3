# Makefile for inet.
SERVER = inet

# Directories
g = generic
i = ../../include
l = ../../lib/bin
d = ../bin

# Programs, flags, and libraries
CC = ${TOOLCHAIN_PREFIX}gcc
MKDEP = $(CC) -M -MG
CFLAGS = -ffreestanding -nostdinc -fno-plt -fno-pie -I$i -I. -D__minix -D_MINIX $(OPT)
LDFLAGS = -static -nostartfiles -Wl,--build-id=none -Wl,-Map=$d/$(SERVER).map -Wl,--cref -L$l
LIBS = -lsys -lsysutil

OBJ = 	buf.o clock.o inet.o inet_config.o \
	mnx_eth.o mq.o qp.o sr.o stacktrace.o \
	$g/udp.o $g/arp.o $g/eth.o $g/event.o \
	$g/icmp.o $g/io.o $g/ip.o $g/ip_ioctl.o \
	$g/ip_lib.o $g/ip_read.o $g/ip_write.o \
	$g/ipr.o $g/rand256.o $g/tcp.o $g/tcp_lib.o \
	$g/tcp_recv.o $g/tcp_send.o $g/ip_eth.o \
	$g/ip_ps.o $g/psip.o \
	minix3/queryparam.o sha2.o version.o

all:	$d/$(SERVER)

$d/$(SERVER):	$(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LIBS)

install: $d/$(SERVER)
	install -c $? /usr/sbin/inet

clean:
	rm -f $(OBJ) $d/$(SERVER) $d/$(SERVER).map *.bak

depend: 
	$(MKDEP) $(CFLAGS) *.c generic/*.c > .depend

# Include generated dependencies.
-include .depend
