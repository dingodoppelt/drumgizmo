bin_PROGRAMS = drumgizmo

drumgizmo_LDADD = $(JACK_LIBS) $(SNDFILE_LIBS)

drumgizmo_CXXFLAGS = $(JACK_CXXFLAGS) $(SNDFILE_CXXFLAGS)

drumgizmo_SOURCES = \
	drumgizmo.cc \
	event.cc \
	jackclient.cc \
	sample.cc

EXTRA_DIST = \
	jackclient.h \
	event.h \
	sample.h
