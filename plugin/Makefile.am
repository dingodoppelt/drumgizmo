include $(top_srcdir)/plugingui/Makefile.am.plugingui
include $(top_srcdir)/src/Makefile.am.drumgizmo

plugindir = $(libdir)/lv2/drumgizmo.lv2

if ENABLE_LV2
plugin_LTLIBRARIES = drumgizmo.la
endif

plugin_DATA = manifest.ttl drumgizmo.ttl

EXTRA_DIST = \
	$(plugin_DATA) \
	input_lv2.h \
	output_lv2.h \
	lv2_event.h \
	lv2_gui.h \
	lv2_instance.h

drumgizmo_la_CXXFLAGS = -I$(top_srcdir)/plugin/plugingizmo -I$(top_srcdir)/src \
	-I$(top_srcdir)/plugingui \
	-DLV2 -DX11 -DLV2_PLUGIN_URI=\"http://drumgizmo.org/lv2\" \
	-I$(top_srcdir)/include $(SNDFILE_CXXFLAGS) \
	$(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) $(LV2_CFLAGS) \
	$(PLUGIN_GUI_CFLAGS) $(SSEFLAGS) $(ZITA_CPPFLAGS) \
	-DUSE_THREAD $(SAMPLERATE_CFLAGS)

drumgizmo_la_SOURCES = \
	$(DRUMGIZMO_SOURCES) \
	$(PLUGIN_GUI_SOURCES) \
	$(top_srcdir)/plugin/plugingizmo/midievent.cc \
	$(top_srcdir)/plugin/plugingizmo/pluginlv2.cc \
	drumgizmo_plugin.cc

drumgizmo_la_LDFLAGS = -module -avoid-version
drumgizmo_la_LIBADD = $(LV2_LIBS) $(PLUGIN_GUI_LIBS) $(DRUMGIZMO_LIBS)

