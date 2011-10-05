if ENABLE_CLI

SUBDIRS = input output

bin_PROGRAMS = drumgizmo

drumgizmo_LDADD = $(SNDFILE_LIBS) $(PTHREAD_LIBS) $(EXPAT_LIBS) -ldl

drumgizmo_CXXFLAGS = $(SNDFILE_CXXFLAGS) $(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/src

drumgizmo_SOURCES = \
	audioinputenginedl.cc \
	audiooutputenginedl.cc \
	drumgizmoc.cc \
	$(top_srcdir)/src/audiofile.cc \
	$(top_srcdir)/src/channel.cc \
	$(top_srcdir)/src/channelmixer.cc \
	$(top_srcdir)/src/drumgizmo.cc \
	$(top_srcdir)/src/drumkit.cc \
	$(top_srcdir)/src/drumkitparser.cc \
	$(top_srcdir)/src/events.cc \
	$(top_srcdir)/src/instrument.cc \
	$(top_srcdir)/src/instrumentparser.cc \
	$(top_srcdir)/src/midimapper.cc \
	$(top_srcdir)/src/mutex.cc \
	$(top_srcdir)/src/path.cc \
	$(top_srcdir)/src/sample.cc \
	$(top_srcdir)/src/saxparser.cc \
	$(top_srcdir)/src/thread.cc \
	$(top_srcdir)/src/velocity.cc

EXTRA_DIST = \
	audioinputenginedl.h \
	audiooutputenginedl.h

################
# Test Section #
################

TEST_SOURCE_DEPS = ${drumgizmo_SOURCES} ${EXTRA_DIST}
TEST_SCRIPT_DIR = $(top_srcdir)/tools

include ${TEST_SCRIPT_DIR}/Makefile.am.test

include Makefile.am.test

endif