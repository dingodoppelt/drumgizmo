EXTRA_DIST = \
	audiofile.cc \
	channel.cc \
	channelmixer.cc \
	configuration.cc \
	drumgizmo.cc \
	drumkit.cc \
	drumkitparser.cc \
	events.cc \
	instrument.cc \
	instrumentparser.cc \
	midimapparser.cc \
	midimapper.cc \
	mutex.cc \
	path.cc \
	sample.cc \
	saxparser.cc \
	velocity.cc \
	\
	audio.h \
	audiofile.h \
	audioinputengine.h \
	audiooutputengine.h \
	beatmapper.h \
	channel.h \
	channelmixer.h \
	configuration.h \
	drumgizmo.h \
	drumkit.h \
	drumkitparser.h \
	events.h \
	instrument.h \
	instrumentparser.h \
	midimapparser.h \
	midimapper.h \
	mutex.h \
	path.h \
	rangemap.h \
	sample.h \
	saxparser.h \
	thread.h \
	velocity.h


################
# Test Section #
################

TEST_SOURCE_DEPS = ${EXTRA_DIST}
TEST_SCRIPT_DIR = $(top_srcdir)/tools

include ${TEST_SCRIPT_DIR}/Makefile.am.test

include Makefile.am.test
