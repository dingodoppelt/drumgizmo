include $(top_srcdir)/plugingui/Makefile.am.plugingui
include $(top_srcdir)/src/Makefile.am.drumgizmo

#######
# LV2
###
if ENABLE_LV2
lv2plugindir = $(libdir)/lv2/drumgizmo.lv2
lv2plugin_LTLIBRARIES = drumgizmo.la
lv2plugin_DATA = manifest.ttl drumgizmo.ttl
endif

drumgizmo_la_CXXFLAGS = -DLV2 -DLV2_PLUGIN_URI=\"http://drumgizmo.org/lv2\" \
	-I$(top_srcdir)/plugin/plugingizmo \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/include $(SNDFILE_CXXFLAGS) \
	$(PTHREAD_CFLAGS) $(EXPAT_CFLAGS)  \
	$(PLUGIN_GUI_CFLAGS) $(SSEFLAGS) $(ZITA_CPPFLAGS) \
	$(SAMPLERATE_CFLAGS)

drumgizmo_la_SOURCES = \
	$(DRUMGIZMO_SOURCES) \
	$(PLUGIN_GUI_SOURCES) \
	$(top_srcdir)/plugin/plugingizmo/midievent.cc \
	$(top_srcdir)/plugin/plugingizmo/pluginlv2.cc \
	drumgizmo_plugin.cc

drumgizmo_la_LDFLAGS = -module -avoid-version
drumgizmo_la_LIBADD = $(LV2_LIBS) $(PLUGIN_GUI_LIBS) $(DRUMGIZMO_LIBS)


#######
# VST
###
if ENABLE_VST
vstplugindir = $(prefix)/lib/vst
vstplugin_LTLIBRARIES = drumgizmo_vst.la
endif

VST_BASE = ${VST_SOURCE_PATH}
VST_SRC_BASE = ${VST_BASE}/public.sdk/source/vst2.x/

# hack to compile vst sources without -Wall -Werror
nodist_drumgizmo_vst_la_SOURCES = \
	${VST_SRC_BASE}/audioeffectx.cpp \
	${VST_SRC_BASE}/audioeffect.cpp \
	${VST_SRC_BASE}/vstplugmain.cpp

drumgizmo_vst_la_CXXFLAGS = -DVST \
	-I$(top_srcdir)/plugin/plugingizmo \
	-I$(top_srcdir)/src -I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/include $(SNDFILE_CXXFLAGS) \
	$(EXPAT_CFLAGS) $(VST_CPPFLAGS) \
	$(PLUGIN_GUI_CFLAGS) $(SSEFLAGS)\
	$(SAMPLERATE_CFLAGS)

drumgizmo_vst_la_SOURCES = \
	$(DRUMGIZMO_SOURCES) \
	$(PLUGIN_GUI_SOURCES) \
	$(top_srcdir)/plugin/plugingizmo/midievent.cc \
	$(top_srcdir)/plugin/plugingizmo/pluginvst.cc \
	drumgizmo_plugin.cc

drumgizmo_vst_la_LDFLAGS = -shared -module -avoid-version
drumgizmo_vst_la_LIBADD = $(PLUGIN_GUI_LIBS) $(DRUMGIZMO_LIBS)







EXTRA_DIST = \
	$(plugin_DATA)
