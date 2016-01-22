DISTDIRS = input output

if ENABLE_CLI

include $(top_srcdir)/src/Makefile.am.drumgizmo 

SUBDIRS = input output

bin_PROGRAMS = drumgizmo

drumgizmo_LDADD = $(DRUMGIZMO_LIBS) $(PTHREAD_LIBS) -ldl $(JACK_LIBS)

drumgizmo_LDFLAGS = 

drumgizmo_CXXFLAGS = $(SNDFILE_CXXFLAGS) $(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) \
	-I$(top_srcdir)/include -I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DWITH_HUG_MUTEX -DWITH_HUG_FILTER \
	$(JACK_CFLAGS) $(SSEFLAGS) \
	-DINPUT_PLUGIN_DIR=\"${INPUT_PLUGIN_DIR}\" \
	-DOUTPUT_PLUGIN_DIR=\"${OUTPUT_PLUGIN_DIR}\"

drumgizmo_CFLAGS = -DWITH_HUG_MUTEX -DWITH_HUG_FILTER

drumgizmo_SOURCES = \
	drumgizmoc.cc \
	jackclient.cc \
	enginefactory.cc \
	$(DRUMGIZMO_SOURCES) \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/hugin/hugin_filter.c

if HAVE_INPUT_DUMMY
drumgizmo_SOURCES += input/inputdummy.cc
drumgizmo_CXXFLAGS += -DHAVE_INPUT_DUMMY
endif # HAVE_INPUT_DUMMY

if HAVE_INPUT_MIDIFILE
drumgizmo_CXXFLAGS += $(SMF_CFLAGS)
drumgizmo_LDADD += $(SMF_LIBS)
drumgizmo_SOURCES += input/midifile.cc
drumgizmo_CXXFLAGS += -DHAVE_INPUT_MIDIFILE
endif # HAVE_INPUT_MIDIFILE

if HAVE_OUTPUT_DUMMY
drumgizmo_SOURCES += output/outputdummy.cc
drumgizmo_CXXFLAGS += -DHAVE_OUTPUT_DUMMY
endif # HAVE_OUTPUT_DUMMY

if HAVE_OUTPUT_WAVFILE
drumgizmo_SOURCES += output/wavfile.cc
drumgizmo_CXXFLAGS += -DHAVE_OUTPUT_WAVFILE
endif # HAVE_OUTPUT_WAVFILE

if HAVE_OUTPUT_ALSA
drumgizmo_CXXFLAGS += $(ALSA_CFLAGS)
drumgizmo_LDFLAGS += $(ALSA_LIBS)
drumgizmo_SOURCES += output/alsa.cc
drumgizmo_CXXFLAGS += -DHAVE_OUTPUT_ALSA
endif # HAVE_OUTPUT_ALSA

EXTRA_DIST = \
	drumgizmoc.h \
	jackclient.h \
	enginefactory.h

endif # ENABLE_CLI
