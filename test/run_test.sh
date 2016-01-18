#!/bin/bash
# This is a script to faciliate running single tests.
#
# Usage: ./run_test.sh <test_1> <test_2> ... <test_n>
# If no test string is passed then all tests are run.

test_dir=$(dirname $0)
cd $test_dir

if [[ $# == 0 ]]
then
	echo "======================"
	echo "All tests are now run."
	echo "======================"
	make -C .. check
else
	for TST in "$@"
	do
		echo "========================="
		echo "The $TST test is now run."
		echo "========================="
		rm -f $TST*.o; make $TST && (./$TST; RES=$?; echo; echo "Result: $RES"; cat result_$TST.xml)
	done
fi
