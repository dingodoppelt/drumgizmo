if ENABLE_CLI
if HAVE_INPUT_MIDIFILE

include $(top_srcdir)/src/Makefile.am.drumgizmo

bin_PROGRAMS = dgreftest

dgreftest_LDADD = $(DRUMGIZMO_LIBS) $(PTHREAD_LIBS) $(JACK_LIBS) \
	$(SMF_LIBS)

dgreftest_LDFLAGS =

dgreftest_CXXFLAGS = $(SNDFILE_CXXFLAGS) $(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) \
	-I$(top_srcdir)/include -I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DWITH_HUG_MUTEX -DWITH_HUG_FILTER \
	$(JACK_CFLAGS) $(SSEFLAGS) $(SMF_CFLAGS) $(FLOAT_STORE)

dgreftest_CFLAGS = -DWITH_HUG_MUTEX -DWITH_HUG_FILTER

dgreftest_SOURCES = \
	dgreftest.cc \
	midiinputengine.cc \
	wavfileoutputengine.cc \
	compareoutputengine.cc \
	$(DRUMGIZMO_SOURCES) \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/hugin/hugin_filter.c

endif # HAVE_INPUT_MIDIFILE

EXTRA_DIST = \
	midiinputengine.h \
	wavfileoutputengine.h \
	compareoutputengine.h

endif # ENABLE_CLI
