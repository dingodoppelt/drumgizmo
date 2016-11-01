#!/bin/sh -e

# Check if hugin has been obtained:
if [ ! -f hugin/hugin.c ]
then
		echo "You are missing the hugin submodule."
		echo "Run"
		echo " git submodule init"
		echo " git submodule update"
		echo "to obtain it."
		exit 1
fi

# Now run autoreconf
${AUTORECONF:-autoreconf} -fiv --warnings=no-unsupported
