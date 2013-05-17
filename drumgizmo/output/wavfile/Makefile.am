
wavfilesources = \
	wavfile.cc

if HAVE_OUTPUT_WAVFILE

wavfileltlibs = libwavfile.la
wavfilebuildsources = $(wavfilesources)

else

wavfileltlibs =
wavfilebuildsources =

endif

EXTRA_DIST = $(wavfilesources)

lib_LTLIBRARIES = $(wavfileltlibs)

libdir = $(OUTPUT_PLUGIN_DIR)

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(SNDFILE_CFLAGS)
libwavfile_la_LDFLAGS = $(SNDFILE_LIBS)
libwavfile_la_LIBADD =
libwavfile_la_SOURCES = $(wavfilebuildsources)
