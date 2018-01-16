#!/bin/bash

BASEPATH=$PWD

GET_OS=`uname -s`;
GET_BIT=`uname -m`;
case $GET_OS in
SunOS)
	MACH_OS="solaris";
	MACH_BIT=`isainfo -b`;
	MACH_OUT="s$MACH_BIT";
	;;
Darwin)
	MACH_OS="macosx";
	if [ "$GET_BIT" == "x86_64" ]; then
		MACH_BIT="64";
	else
		MACH_BIT="32";
	fi;
	MACH_OUT="m$MACH_BIT";
	;;
Linux)
	MACH_OS="linux";
	if [ "$GET_BIT" == "x86_64" ]; then
		MACH_BIT="64";
	else
		MACH_BIT="32";
	fi;
	MACH_OUT="l$MACH_BIT";
	;;
*)
	MACH_OS="unknown";
	MACH_BIT="unknown";
	MACH_OUT="unknown";
	exit;
	;;
esac;
MACH_DESC="${MACH_OS}-${MACH_BIT}bit";



INCPATH="$BASEPATH/inc"
LIBPATH="$BASEPATH/lib"
BINPATH="$BASEPATH/bin"
LIBFROM="$LIBPATH/$MACH_OUT"
BINFROM="$BINPATH/$MACH_OUT"

echo "[35mICORE Set Environment($MACH_DESC)[0m"

if [ -d "$LIBFROM" ]; then
echo "  > [LIBRARY] [33mSet Library files from '$LIBFROM' to '$LIBPATH'[0m ..."
cp $LIBFROM/* $LIBPATH/
else
echo "  > [LIBRARY] [33mLibrary path($LIBFROM) is not exist !! [0m ..."
echo "[35mICORE Set Environment($MACH_DESC) Failed !![0m"
exit;
fi

if [ -d "$BINFROM" ]; then
echo "  > [BINARY]  [33mSet Binary files from '$BINFROM' to '$BINPATH'[0m ..."
cp $BINFROM/* $BINPATH/
else
echo "  > [BINARY] [33mBinary path($BINFROM) is not exist !! [0m ..."
echo "[35mICORE Set Environment($MACH_DESC) Failed !![0m"
exit;
fi

echo "[35mICORE Set Environment($MACH_DESC) Complete !![0m"
