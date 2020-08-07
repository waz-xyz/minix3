# Generated from lib/Makefile.in

all: all-gnu

all-gnu: 

makefiles: Makefile
Makedepend: 
	sh generate.sh obj bin

Makefile: Makefile.in Makedepend
	sh generate.sh obj bin
	@echo
	@echo *Attention*
	@echo Makefile is regenerated... rerun command to see changes
	@echo *Attention*
	@echo

all-gnu: makefiles
	mkdir -p obj/ansi
	cd ansi && $(MAKE) $@
	mkdir -p obj/curses
	cd curses && $(MAKE) $@
	mkdir -p obj/dummy
	cd dummy && $(MAKE) $@
	mkdir -p obj/editline
	cd editline && $(MAKE) $@
	mkdir -p obj/ieee_float
	cd ieee_float && $(MAKE) $@
	mkdir -p obj/ip
	cd ip && $(MAKE) $@
	mkdir -p obj/math
	cd math && $(MAKE) $@
	mkdir -p obj/other
	cd other && $(MAKE) $@
	mkdir -p obj/posix
	cd posix && $(MAKE) $@
	mkdir -p obj/regex
	cd regex && $(MAKE) $@
	mkdir -p obj/stdio
	cd stdio && $(MAKE) $@
	mkdir -p obj/syslib
	cd syslib && $(MAKE) $@
	mkdir -p obj/util
	cd util && $(MAKE) $@
	mkdir -p obj/sysutil
	cd sysutil && $(MAKE) $@
	mkdir -p obj/timers
	cd timers && $(MAKE) $@
	mkdir -p obj/zlib-1.2.3
	cd zlib-1.2.3 && $(MAKE) $@
	mkdir -p obj/armv7
	cd armv7 && $(MAKE) $@

clean depend:: makefiles
	cd ansi && $(MAKE) $@
	cd curses && $(MAKE) $@
	cd dummy && $(MAKE) $@
	cd editline && $(MAKE) $@
	cd ieee_float && $(MAKE) $@
	cd ip && $(MAKE) $@
	cd math && $(MAKE) $@
	cd other && $(MAKE) $@
	cd posix && $(MAKE) $@
	cd regex && $(MAKE) $@
	cd stdio && $(MAKE) $@
	cd syslib && $(MAKE) $@
	cd util && $(MAKE) $@
	cd sysutil && $(MAKE) $@
	cd timers && $(MAKE) $@
	cd zlib-1.2.3 && $(MAKE) $@
	cd armv7 && $(MAKE) $@

makefiles: ansi/Makefile
makefiles: curses/Makefile
makefiles: dummy/Makefile
makefiles: editline/Makefile
makefiles: ieee_float/Makefile
makefiles: ip/Makefile
makefiles: math/Makefile
makefiles: other/Makefile
makefiles: posix/Makefile
makefiles: regex/Makefile
makefiles: stdio/Makefile
makefiles: syslib/Makefile
makefiles: util/Makefile
makefiles: sysutil/Makefile
makefiles: timers/Makefile
makefiles: zlib-1.2.3/Makefile
makefiles: armv7/Makefile

ansi/Makefile: ansi/Makefile.in
	cd ansi && sh ../generate.sh ../obj/ansi ../bin && $(MAKE) makefiles
curses/Makefile: curses/Makefile.in
	cd curses && sh ../generate.sh ../obj/curses ../bin && $(MAKE) makefiles
dummy/Makefile: dummy/Makefile.in
	cd dummy && sh ../generate.sh ../obj/dummy ../bin && $(MAKE) makefiles
editline/Makefile: editline/Makefile.in
	cd editline && sh ../generate.sh ../obj/editline ../bin && $(MAKE) makefiles
ieee_float/Makefile: ieee_float/Makefile.in
	cd ieee_float && sh ../generate.sh ../obj/ieee_float ../bin && $(MAKE) makefiles
ip/Makefile: ip/Makefile.in
	cd ip && sh ../generate.sh ../obj/ip ../bin && $(MAKE) makefiles
math/Makefile: math/Makefile.in
	cd math && sh ../generate.sh ../obj/math ../bin && $(MAKE) makefiles
other/Makefile: other/Makefile.in
	cd other && sh ../generate.sh ../obj/other ../bin && $(MAKE) makefiles
posix/Makefile: posix/Makefile.in
	cd posix && sh ../generate.sh ../obj/posix ../bin && $(MAKE) makefiles
regex/Makefile: regex/Makefile.in
	cd regex && sh ../generate.sh ../obj/regex ../bin && $(MAKE) makefiles
stdio/Makefile: stdio/Makefile.in
	cd stdio && sh ../generate.sh ../obj/stdio ../bin && $(MAKE) makefiles
syslib/Makefile: syslib/Makefile.in
	cd syslib && sh ../generate.sh ../obj/syslib ../bin && $(MAKE) makefiles
util/Makefile: util/Makefile.in
	cd util && sh ../generate.sh ../obj/util ../bin && $(MAKE) makefiles
sysutil/Makefile: sysutil/Makefile.in
	cd sysutil && sh ../generate.sh ../obj/sysutil ../bin && $(MAKE) makefiles
timers/Makefile: timers/Makefile.in
	cd timers && sh ../generate.sh ../obj/timers ../bin && $(MAKE) makefiles
zlib-1.2.3/Makefile: zlib-1.2.3/Makefile.in
	cd zlib-1.2.3 && sh ../generate.sh ../obj/zlib-1.2.3 ../bin && $(MAKE) makefiles
armv7/Makefile: armv7/Makefile.in
	cd armv7 && sh ../generate.sh ../obj/armv7 ../bin && $(MAKE) makefiles

clean::
	rm -df obj/*
	rm -f bin/*.a

clean-make:
	find ansi -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find curses -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find dummy -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find editline -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find ieee_float -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find ip -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find math -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find other -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find posix -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find regex -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find stdio -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find syslib -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find util -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find sysutil -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find timers -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find zlib-1.2.3 -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete
	find armv7 -type f \( -name 'Makefile' -or -name 'Makedepend' \) -delete

install:
	cp obj/*.a /usr/lib

include Makedepend
include .depend
