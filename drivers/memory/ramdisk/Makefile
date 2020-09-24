# Makefile for ramdisk image

ifeq (${ARCH}, x86)
PROTO_FILE = proto.x86
PROGRAMS = at_wini bios_wini cdprobe dev2name floppy loadramdisk newroot pci sh service sysenv
endif
ifeq (${ARCH}, arm)
PROTO_FILE = proto.arm
PROGRAMS = dev2name loadramdisk newroot sh service sysenv
endif

MKDEP = $(CC) -M -MG
STRIP = ${TOOLCHAIN_PREFIX}strip

# all:	image.c image.s
all:	image.c

clean:
	rm -rf $(PROGRAMS) bintoc image image.c proto.gen

image.c:	bintoc image
	./bintoc -o $@ image

image.s:	image.c
	sed < image.c > $@ 's/^/.data1 /;s/,$$//' || { rm -f $@; false; }

# Note for cross compilation: this executable has to be compiled for the
# host system
bintoc:	bintoc.c
	$(CC) -o $@ bintoc.c

image:	proto.gen mtab rc $(PROGRAMS) ../../../tools/mkfs
	../../../tools/mkfs -t -B 2048 image proto.gen || { rm -f image; false; }

at_wini: ../../at_wini/at_wini
	$(STRIP) ../../$@/$@ -o $@

../../at_wini/at_wini:
	cd ../../at_wini && make

bios_wini: ../../bios_wini/bios_wini
	$(STRIP) ../../$@/$@ -o $@

../../bios_wini/bios_wini:
	cd ../../bios_wini && make

floppy: ../../floppy/floppy
	$(STRIP) ../../$@/$@ -o $@

../../floppy/floppy:
	cd ../../floppy && make

pci: ../../pci/pci
	$(STRIP) ../../$@/$@ -o $@

../../pci/pci:
	cd ../../pci && make

cdprobe:  ../../../commands/simple/cdprobe
	$(STRIP) ../../../commands/simple/$@ -o $@

../../../commands/simple/cdprobe: 
	cd ../../../commands/simple && make cdprobe

dev2name:  ../../../commands/simple/dev2name
	$(STRIP) ../../../commands/simple/$@ -o $@

../../../commands/simple/dev2name: 
	cd ../../../commands/simple && make dev2name

loadramdisk:  ../../../commands/simple/loadramdisk
	$(STRIP) ../../../commands/simple/$@ -o $@

../../../commands/simple/loadramdisk: 
	cd ../../../commands/simple && make loadramdisk

newroot:  ../../../commands/simple/newroot
	$(STRIP) ../../../commands/simple/$@ -o $@

../../../commands/simple/newroot: 
	cd ../../../commands/simple && make newroot

sysenv:  ../../../commands/simple/sysenv
	$(STRIP) ../../../commands/simple/$@ -o $@

../../../commands/simple/sysenv: 
	cd ../../../commands/simple && make sysenv

sh:	../../../commands/ash/sh
	$(STRIP) ../../../commands/ash/$@ -o $@

../../../commands/ash/sh: 
	cd ../../../commands/ash && make sh

service: ../../../servers/rs/service
	$(STRIP) ../../../servers/rs/$@ -o $@

../../../servers/rs/service: 
	cd ../../../servers/rs && make service

depend: 
	$(MKDEP) $(CFLAGS) *.c > .depend

proto.gen: proto.sh $(PROTO_FILE)
	sh -e proto.sh $(PROTO_FILE) >proto.gen

../../../tools/mkfs: ../../../tools/mkfs32.c ../../../tools/mkfs_dep.c
	cd ../../../tools && make mkfs

# Include generated dependencies.
-include .depend
