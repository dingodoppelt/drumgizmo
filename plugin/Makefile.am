SUBDIRS = vst

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
	-I$(top_srcdir)/plugin/plugingizmo $(DRUMGIZMO_CPPFLAGS) \
	$(PLUGIN_GUI_CPPFLAGS)

drumgizmo_la_SOURCES = \
	$(DRUMGIZMO_SOURCES) \
	$(PLUGIN_GUI_SOURCES) \
	$(top_srcdir)/plugin/plugingizmo/midievent.cc \
	$(top_srcdir)/plugin/plugingizmo/pluginlv2.cc \
	drumgizmo_plugin.cc

drumgizmo_la_LDFLAGS = -shared -module -avoid-version
drumgizmo_la_LIBADD = $(LV2_LIBS) $(PLUGIN_GUI_LIBS) $(DRUMGIZMO_LIBS)


#######
# VST
###
if ENABLE_VST
vstplugindir = $(libdir)/vst
vstplugin_LTLIBRARIES = drumgizmo_vst.la
vstplugin_DATA =
endif

drumgizmo_vst_la_CXXFLAGS = -DVST \
	-I$(top_srcdir)/plugin/plugingizmo $(DRUMGIZMO_CPPFLAGS) \
	$(PLUGIN_GUI_CPPFLAGS) $(VST_CPPFLAGS) -Ivst

drumgizmo_vst_la_SOURCES = \
	$(DRUMGIZMO_SOURCES) \
	$(PLUGIN_GUI_SOURCES) \
	$(top_srcdir)/plugin/plugingizmo/midievent.cc \
	$(top_srcdir)/plugin/plugingizmo/pluginvst.cc \
	drumgizmo_plugin.cc

drumgizmo_vst_la_LDFLAGS = -shared -module -avoid-version
drumgizmo_vst_la_LIBADD = vst/libvstsdk.la $(PLUGIN_GUI_LIBS) $(DRUMGIZMO_LIBS)


install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/lv2/drumgizmo.lv2/drumgizmo.la
	rm -f $(DESTDIR)$(libdir)/vst/drumgizmo_vst.la


EXTRA_DIST = \
	$(lv2plugin_DATA) \
	$(vstplugin_DATA) \
	drumgizmo_plugin.h \
	$(top_srcdir)/plugin/plugingizmo/plugin.h \
	$(top_srcdir)/plugin/plugingizmo/midievent.h \
	$(top_srcdir)/plugin/plugingizmo/pluginlv2.h \
	$(top_srcdir)/plugin/plugingizmo/pluginvst.h
