
jackmidisources = \
	jackmidi.cc \
	$(top_srcdir)/src/midimapper.cc \
	$(top_srcdir)/src/midimapparser.cc \
	$(top_srcdir)/src/saxparser.cc \
	$(top_srcdir)/hugin/hugin.c

if HAVE_INPUT_JACKMIDI

jackmidiltlibs = libjackmidi.la
jackmidibuildsources = $(jackmidisources)

else

jackmidiltlibs =
jackmidibuildsources =

endif

EXTRA_DIST = $(jackmidisources)

lib_LTLIBRARIES = $(jackmidiltlibs)

libdir = $(INPUT_PLUGIN_DIR)

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src -I$(top_srcdir)/hugin
libjackmidi_la_LDFLAGS =
libjackmidi_la_LIBADD =
libjackmidi_la_SOURCES = $(jackmidibuildsources)
