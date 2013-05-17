
midifilesources = \
	midifile.cc \
	$(top_srcdir)/src/midimapper.cc \
	$(top_srcdir)/src/midimapparser.cc \
	$(top_srcdir)/src/saxparser.cc \
	$(top_srcdir)/hugin/hugin.c


if HAVE_INPUT_MIDIFILE

midifileltlibs = libmidifile.la
midifilebuildsources = $(midifilesources)

else

midifileltlibs =
midifilebuildsources =

endif

EXTRA_DIST = $(midifilesources)

lib_LTLIBRARIES = $(midifileltlibs)

libdir = $(INPUT_PLUGIN_DIR)

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(SMF_CFLAGS) \
	$(EXPAT_CFLAGS) -I$(top_srcdir)/hugin -DWITH_HUG_MUTEX
libmidifile_la_LDFLAGS = $(SMF_LIBS) $(EXPAT_LIBS)
libmidifile_la_LIBADD =
libmidifile_la_SOURCES = $(midifilebuildsources)
