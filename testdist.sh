#!/bin/bash

if [ $# -lt 1 ]
then
		echo "Missing version argument."
		exit 1
fi

VER=$1

make dist && (
		mkdir -p tst
		cd tst
		rm -Rf drumgizmo-$VER
		tar xvzf ../drumgizmo-$VER.tar.gz
		cd drumgizmo-$VER
		./configure --enable-lv2 --enable-cli --prefix=/usr
		make
		DESTDIR=$PWD/tst/install make install
)
