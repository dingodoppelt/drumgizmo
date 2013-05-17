
alsasources = \
	alsa.cc

if HAVE_OUTPUT_ALSA

alsaltlibs = libalsa.la
alsabuildsources = $(alsasources)

else

alsaltlibs =
alsabuildsources =

endif

EXTRA_DIST = $(alsasources)

lib_LTLIBRARIES = $(alsaltlibs)

libdir = $(OUTPUT_PLUGIN_DIR)

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/include $(ALSA_CFLAGS)
libalsa_la_LDFLAGS = $(ALSA_LIBS)
libalsa_la_LIBADD =
libalsa_la_SOURCES = $(alsabuildsources)
