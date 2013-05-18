
jackaudiosources = \
	jackaudio.cc

if HAVE_OUTPUT_JACKAUDIO

jackaudioltlibs = libjackaudio.la
jackaudiobuildsources = $(jackaudiosources)

else

jackaudioltlibs =
jackaudiobuildsources =

endif

EXTRA_DIST = $(jackaudiosources)

lib_LTLIBRARIES = $(jackaudioltlibs)

libdir = $(OUTPUT_PLUGIN_DIR)

AM_CPPFLAGS = -I$(top_srcdir)/include $(JACK_CFLAGS)
libjackaudio_la_LDFLAGS = $(JACK_LIBS)
libjackaudio_la_LIBADD =
libjackaudio_la_SOURCES = $(jackaudiobuildsources)
