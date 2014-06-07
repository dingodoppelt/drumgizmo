include $(top_srcdir)/plugingui/Makefile.am.plugingui
include $(top_srcdir)/src/Makefile.am.drumgizmo

VST_BASE = ${VST_SOURCE_PATH}
VST_SRC_BASE = ${VST_BASE}/public.sdk/source/vst2.x/
VST_SRC = \
	${VST_SRC_BASE}/audioeffectx.cpp \
	${VST_SRC_BASE}/audioeffect.cpp \
	${VST_SRC_BASE}/vstplugmain.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/include $(SNDFILE_CXXFLAGS) \
	$(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) $(VST_CPPFLAGS) \
	$(PLUGIN_GUI_CFLAGS) $(SSEFLAGS)\
	-DUSE_THREAD $(SAMPLERATE_CFLAGS)

EXTRA_DIST = \
	Makefile.mingw32 \
	constants.h \
	drumgizmo_vst.cc \
	drumgizmo_vst.h \
	input_vst.cc \
	input_vst.h \
	output_vst.cc \
	output_vst.h

plugindir = $(prefix)/vst
plugin_LTLIBRARIES =

if ENABLE_VST
plugin_LTLIBRARIES += drumgizmo.la
endif

drumgizmo_la_SOURCES = \
  $(VST_SOURCES) \
	$(DRUMGIZMO_SOURCES) \
	$(PLUGIN_GUI_SOURCES) \
	drumgizmo_vst.cc \
	input_vst.cc \
	output_vst.cc

drumgizmo_la_LDFLAGS = -static -static-libgcc -mno-cygwin -module -avoid-version
drumgizmo_la_LIBADD = $(PLUGIN_GUI_LIBS) $(DRUMGIZMO_LIBS)

