
midifilesources = \
	midifile.cc \
	midimap.cc

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

INCLUDES = -I$(top_srcdir)/include $(SMF_CFLAGS)
libmidifile_la_LDFLAGS = $(SMF_LIBS)
libmidifile_la_LIBADD =
libmidifile_la_SOURCES = $(midifilebuildsources)
