
jackmidisources = \
	jackmidi.cc \
	jackclient.cc \
	jackclient.h

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

INCLUDES = -I$(top_srcdir)/include $(JACK_CFLAGS)
libjackmidi_la_LDFLAGS = $(JACK_LIBS)
libjackmidi_la_LIBADD =
libjackmidi_la_SOURCES = $(jackmidibuildsources)
