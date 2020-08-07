#!/bin/sh
# called with parameters: 1:objdir 2:bindir

exec  >Makefile
exec 4>Makedepend
touch  .depend

echo "# Generated from $(basename $(pwd))/Makefile.in"

OBJDIR=$1
BINDIR=$2

RECURSIVE_TARGETS="clean depend"

if [ -z "$OBJDIR" ]; then echo "OBJDIR is not set!" > /dev/tty; exit 1; fi
if [ -z "$BINDIR" ]; then echo "BINDIR is not set!" > /dev/tty; exit 1; fi

. ./Makefile.in

#to enable library debugging, enable the next line
#CFLAGS=$CFLAGS" -g"

CFLAGS="$CFLAGS -fno-plt -fno-pie -ffreestanding -nostdinc -I../../include"
echo
echo "all: all-gnu"
echo
echo "all-gnu: "
echo
echo "makefiles: Makefile"
echo "Makedepend: "
echo "	sh $0 $OBJDIR $BINDIR"
echo
echo "Makefile: Makefile.in Makedepend"
echo "	sh $0 $OBJDIR $BINDIR"
echo "	@echo"
echo "	@echo *Attention*"
echo "	@echo Makefile is regenerated... rerun command to see changes"
echo "	@echo *Attention*"
echo "	@echo"
echo
if [ ! -z "$SUBDIRS" ]; then
	echo "all-gnu: makefiles"
	for dir in $SUBDIRS
	do
		if [ $TYPE = "both" -o $TYPE = "gnu" ]; then
			echo "	mkdir -p $OBJDIR/$dir"
		fi
		
		echo "	cd $dir && \$(MAKE) \$@"
	done
	echo
	echo "$RECURSIVE_TARGETS:: makefiles"
	for dir in $SUBDIRS
	do
		#if [ $TYPE = "both" -o $TYPE = "gnu" ]; then
			#echo "	mkdir -p $OBJDIR/$dir"
		#fi
		
		echo "	cd $dir && \$(MAKE) \$@"
	done
	echo
	for dir in $SUBDIRS
	do
		echo "makefiles: $dir/Makefile"
	done
	echo
	for dir in $SUBDIRS
	do
		echo "$dir/Makefile: $dir/Makefile.in"
		echo "	cd $dir && sh ../$0 ../$OBJDIR/$dir ../$BINDIR && \$(MAKE) makefiles"
	done
else

echo "depend:" >&4
echo "	rm .depend" >&4
echo "	touch .depend" >&4

gnuCommands()
{
	dstfile=$1
	srcfile=$2
	dstdir=`dirname $dstfile`
	sedcmd="sed -e '/<built-in>/d' -e '/<command line>/d' -e 's:^\(.\):$dstdir/\1:'"
	
	case $srcfile in
	*.s )
		echo "	${TOOLCHAIN_PREFIX}gcc $CFLAGS -c -o $dstfile $srcfile"
		
		echo "	${TOOLCHAIN_PREFIX}gcc -M -MG $CFLAGS $srcfile >> .depend" >&4
		;;
	*.gs )
		echo "	${TOOLCHAIN_PREFIX}gas -o $dstfile $srcfile"
		
		echo "	mkdep '${TOOLCHAIN_PREFIX}gcc $CFLAGS -E -x assembler-with-cpp -I.' $srcfile | $sedcmd >> .depend-gnu" >&4
		;;
	*.c )
		echo "	${TOOLCHAIN_PREFIX}gcc $CFLAGS -c -o $dstfile $srcfile"
		
		echo "	${TOOLCHAIN_PREFIX}gcc -M -MG $CFLAGS $srcfile >> .depend" >&4
		;;
	#*.mod )
	#	echo "	\$(M2C) -o $dstfile $srcfile"
	#	;;
	#*.fc )
	#	echo "	sh ./FP.COMPILE $srcfile"
	#	;;
	esac
	echo
}

#libraries
for lib in $LIBRARIES
do	
	if [ $TYPE = "both" -o $TYPE = "gnu" ]; then
		echo "all-gnu: $BINDIR/$lib.a"
		eval "FILES=\$${lib}_FILES" 
		echo
		for f in $FILES
		do
			o=`echo $f | sed -e 's/\\..*\$/\.o/'`
			echo "$BINDIR/$lib.a: $OBJDIR/$o"
		done
		echo
		echo "$BINDIR/$lib.a:"
		echo "	${TOOLCHAIN_PREFIX}ar cr $BINDIR/$lib.a \$?"
		echo
		for f in $FILES
		do
			o=`echo $f | sed -e 's/\\..*\$/\.o/'`
			
			echo "$OBJDIR/$o: $f"
			
			gnuCommands "$OBJDIR/$o" $f
		done
		echo
	else
		echo "ACK is no longer supported!" > /dev/tty
	fi
done
echo

#start files
for f in $STARTFILES
do
	o=`echo $f | sed -e 's/\\..*\$/\.o/'`
	
	if [ $TYPE = "both" -o $TYPE = "gnu" ]; then
		echo "all-gnu: $OBJDIR/$o"
		echo
		echo "$OBJDIR/$o: $f"
		gnuCommands $OBJDIR/$o $f
		echo
	else
		echo "ACK is no longer supported!" > /dev/tty
	fi
done

fi # elif of if [ -n "$SUBDIRS" ]

echo
echo "clean::"
if [ $TYPE = "both" -o $TYPE = "gnu" ]; then
	echo "	rm -df $OBJDIR/*"
	if [ $OBJDIR = "obj" ]; then
		echo "	rm -f $BINDIR/*.a"
	fi
else
	echo "ACK is no longer supported!" > /dev/tty
fi

if [ ! -z "$SUBDIRS" ]; then
	echo
	echo "clean-make:"
	for dir in $SUBDIRS
	do
		echo "	find $dir -type f \\( -name 'Makefile' -or -name 'Makedepend' \\) -delete"
	done
fi

if [ $OBJDIR = "obj" ]; then
	echo
	echo "install:"
	echo "	cp $OBJDIR/*.a /usr/lib"
fi

echo
echo "include Makedepend"
echo "include .depend"
