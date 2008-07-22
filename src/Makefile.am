bin_PROGRAMS = drumgizmo

drumgizmo_LDADD = $(JACK_LIBS) $(SNDFILE_LIBS)

drumgizmo_CXXFLAGS = $(JACK_CXXFLAGS) $(SNDFILE_CXXFLAGS)

drumgizmo_SOURCES = \
	audiofile.cc \
	channel.cc \
	drumgizmo.cc \
	drumkitparser.cc \
	event.cc \
	instrument.cc \
	jackclient.cc \
	midimapper.cc \
	sample.cc \
	saxparser.cc \
	velocity.cc

EXTRA_DIST = \
	audiofile.h \
	channel.h \
	drumkit.h \
	drumkitparser.h \
	event.h \
	instrument.h \
	jackclient.h \
	midimapper.h \
	sample.h \
	saxparser.h \
	velocity.h
