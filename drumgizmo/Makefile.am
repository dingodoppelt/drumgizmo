DISTDIRS = input output

if ENABLE_CLI

include $(top_srcdir)/src/Makefile.am.drumgizmo 

SUBDIRS = input output

bin_PROGRAMS = drumgizmo

drumgizmo_LDADD = $(DRUMGIZMO_LIBS) $(PTHREAD_LIBS) -ldl

drumgizmo_CXXFLAGS = $(SNDFILE_CXXFLAGS) $(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) \
	-I$(top_srcdir)/include -I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DWITH_HUG_MUTEX $(SSEFLAGS) 

drumgizmo_SOURCES = \
	audioinputenginedl.cc \
	audiooutputenginedl.cc \
	drumgizmoc.cc \
	$(DRUMGIZMO_SOURCES) \
	$(top_srcdir)/hugin/hugin.c

EXTRA_DIST = \
	audioinputenginedl.h \
	audiooutputenginedl.h

endif
