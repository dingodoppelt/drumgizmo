bin_PROGRAMS = drumgizmo

drumgizmo_LDADD = $(ALSA_LIBS) $(JACK_LIBS) $(SNDFILE_LIBS) $(SMF_LIBS) \
	$(PTHREAD_CFLAGS)

drumgizmo_CXXFLAGS = $(ALSA_CFLAGS) $(JACK_CXXFLAGS) $(SNDFILE_CXXFLAGS) \
	 $(SMF_CFLAGS) $(PTHREAD_LIBS)

drumgizmo_SOURCES = \
	audioinputengine.cc \
	audioinputenginejackmidi.cc \
	audiooutputengine.cc \
	audiooutputenginealsa.cc \
	audiooutputenginejack.cc \
	audiooutputenginesndfile.cc \
	audiofile.cc \
	cli.cc \
	channel.cc \
	channelmixer.cc \
	drumgizmo.cc \
	drumkitparser.cc \
	event.cc \
	instrument.cc \
	midimapper.cc \
	midiplayer.cc \
	mutex.cc \
	sample.cc \
	saxparser.cc \
	thread.cc \
	velocity.cc

#	beatmapper.cc

EXTRA_DIST = \
	audio.h \
	audioinputengine.h \
	audioinputenginejackmidi.h \
	audiooutputengine.h \
	audiooutputenginealsa.h \
	audiooutputenginejack.h \
	audiooutputenginesndfile.h \
	audiofile.h \
	channel.h \
	channelmixer.h \
	drumgizmo.h \
	drumkit.h \
	drumkitparser.h \
	event.h \
	instrument.h \
	midimapper.h \
	midiplayer.h \
	mutex.h \
	rangemap.h \
	sample.h \
	saxparser.h \
	thread.h \
	velocity.h

#	beatmapper.h

################
# Test Section #
################

TEST_SOURCE_DEPS = ${drumgizmo_SOURCES} ${EXTRA_DIST}
TEST_SCRIPT_DIR = $(top_srcdir)/tools

include ${TEST_SCRIPT_DIR}/Makefile.am.test

include Makefile.am.test