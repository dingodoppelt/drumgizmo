
dummysources = \
	dummy.cc

if HAVE_INPUT_DUMMY

dummyltlibs = libdummy.la
dummybuildsources = $(dummysources)

else

dummyltlibs =
dummybuildsources =

endif

EXTRA_DIST = $(dummysources)

lib_LTLIBRARIES = $(dummyltlibs)

libdir = $(INPUT_PLUGIN_DIR)

INCLUDES = -I$(top_srcdir)/include
libdummy_la_LDFLAGS =
libdummy_la_LIBADD =
libdummy_la_SOURCES = $(dummybuildsources)
