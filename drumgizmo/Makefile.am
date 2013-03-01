DISTDIRS = input output

if ENABLE_CLI

include $(top_srcdir)/src/Makefile.am.drumgizmo

SUBDIRS = input output

bin_PROGRAMS = drumgizmo

drumgizmo_LDADD = $(DRUMGIZMO_LIBS) $(PTHREAD_LIBS) -ldl

drumgizmo_CXXFLAGS = $(SNDFILE_CXXFLAGS) $(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) \
	-I$(top_srcdir)/include -I$(top_srcdir)/src -DSSE -msse -msse2 -msse3 \
	-I../hugin -DWITH_HUG_MUTEX

drumgizmo_SOURCES = \
	audioinputenginedl.cc \
	audiooutputenginedl.cc \
	drumgizmoc.cc \
	$(DRUMGIZMO_SOURCES) \
	$(top_srcdir)/hugin/hugin.c

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