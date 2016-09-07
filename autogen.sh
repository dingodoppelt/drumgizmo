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

# Check for the existence of cppunit.m4. If missing emulate it.
mkdir -p actest
cat << EOF > actest/configure.ac
AC_INIT([actest], [1.0.0])
AM_PATH_CPPUNIT(1.9.6)
EOF
[ -f acinclude.m4 ] && rm acinclude.m4
autoreconf -W error actest 2>/dev/null || echo "AC_DEFUN([AM_PATH_CPPUNIT],[echo 'cppunit hack'])" > acinclude.m4
rm -Rf actest

# Now run autoreconf
${AUTORECONF:-autoreconf} -fiv --warnings=no-unsupported
