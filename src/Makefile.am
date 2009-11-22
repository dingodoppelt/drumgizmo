bin_PROGRAMS = drumgizmo

drumgizmo_LDADD = $(JACK_LIBS) $(SNDFILE_LIBS) $(SMF_LIBS)

drumgizmo_CXXFLAGS = $(JACK_CXXFLAGS) $(SNDFILE_CXXFLAGS) $(SMF_CFLAGS)

drumgizmo_SOURCES = \
	audiofile.cc \
	beatmapper.cc \
	channel.cc \
	drumgizmo.cc \
	drumkitparser.cc \
	event.cc \
	instrument.cc \
	jackclient.cc \
	midimapper.cc \
	midiplayer.cc \
	sample.cc \
	saxparser.cc \
	velocity.cc

EXTRA_DIST = \
	audiofile.h \
	beatmapper.h \
	channel.h \
	drumkit.h \
	drumkitparser.h \
	event.h \
	instrument.h \
	jackclient.h \
	midimapper.h \
	midiplayer.h \
	sample.h \
	saxparser.h \
	velocity.h
