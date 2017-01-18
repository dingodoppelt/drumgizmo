#!/bin/bash

if [ $# -lt 1 ]
then
		echo "Missing version argument."
		exit 1
fi

if [ "$VST_BASE" == "" ]
then
		echo "Set the absolute VST_BASE variable."
		exit 1
fi


VER=$1

make dist && (
		mkdir -p tst
		cd tst
		rm -Rf drumgizmo-$VER
		tar xvzf ../drumgizmo-$VER.tar.gz
		cd drumgizmo-$VER
		./configure --enable-lv2 --enable-cli --enable-vst \
		            --with-vst-sources="$VST_BASE" --prefix=/usr \
		            --with-test --with-lv2dir=$PWD/tst/install/lv2
		make
		DESTDIR=$PWD/tst/install make install
		(cd plugin; make install)
		make check
)
