
testsources = \
	test.cc

if HAVE_INPUT_TEST

testltlibs = libtest.la
testbuildsources = $(testsources)

else

testltlibs =
testbuildsources =

endif

EXTRA_DIST = $(testsources)

lib_LTLIBRARIES = $(testltlibs)

libdir = $(INPUT_PLUGIN_DIR)

INCLUDES = -I$(top_srcdir)/include
libtest_la_LDFLAGS =
libtest_la_LIBADD =
libtest_la_SOURCES = $(testbuildsources)
