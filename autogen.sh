#!/bin/sh -e

# Check for the existence of cppunit.m4. If missing emulate it.
mkdir -p actest
cat << EOF > actest/configure.ac
AC_INIT([actest], [1.0.0])
AM_PATH_CPPUNIT(1.9.6)
EOF
[ -f acinclude.m4 ] && rm acinclude.m4
autoreconf actest 2>/dev/null || echo "AC_DEFUN([AM_PATH_CPPUNIT],[])" > acinclude.m4
rm -Rf actest

# Now run autoreconf
${AUTORECONF:-autoreconf} -fiv
