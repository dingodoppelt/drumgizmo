
jackmidisources = \
	jackmidi.cc

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

AM_CPPFLAGS = -I$(top_srcdir)/include
libjackmidi_la_LDFLAGS =
libjackmidi_la_LIBADD =
libjackmidi_la_SOURCES = $(jackmidibuildsources)
