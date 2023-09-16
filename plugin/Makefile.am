SUBDIRS = vst
noinst_PROGRAMS =


#######
# LV2
###
if ENABLE_LV2
lv2plugindir = @LV2DIR@/drumgizmo.lv2
lv2plugin_LTLIBRARIES = drumgizmo.la
noinst_PROGRAMS += ttlgen
lv2plugin_DATA = manifest.ttl
endif

drumgizmo_la_CXXFLAGS = -DLV2 -DLV2_PLUGIN_URI=\"http://drumgizmo.org/lv2\" \
	$(DEBUG_FLAGS) \
	-fvisibility=hidden \
	-I$(top_srcdir)/zita-resampler/libs \
	$(LV2_CFLAGS) \
	$(SNDFILE_CFLAGS) \
	-I$(top_srcdir)/plugin/plugingizmo \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
if !ENABLE_HEADLESS
drumgizmo_la_CXXFLAGS += -I$(top_srcdir)/plugingui
endif
drumgizmo_la_CFLAGS = -fvisibility=hidden

drumgizmo_la_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/plugin/plugingizmo/midievent.cc \
	$(top_srcdir)/plugin/plugingizmo/pluginlv2.cc \
	drumgizmo_plugin.cc

drumgizmo_la_LDFLAGS = -shared -module -avoid-version \
	-no-undefined -export-symbols $(top_srcdir)/plugin/drumgizmo_lv2.sym

drumgizmo_la_LIBADD = $(LV2_LIBS) \
	$(top_builddir)/src/libdg.la \
	$(top_builddir)/src/libnls.la
if !ENABLE_HEADLESS
drumgizmo_la_LIBADD += $(top_builddir)/plugingui/libplugingui.la
endif
ttlgen_CPPFLAGS = \
	$(DEBUG_FLAGS) \
	$(DL_CFLAGS) -I$(top_srcdir)/plugin/plugingizmo
ttlgen_LDFLAGS = $(DL_LIBS)
ttlgen_SOURCES = \
	$(top_srcdir)/plugin/plugingizmo/ttlgen.cc

if ENABLE_X11
UITYPE=X11UI
endif

if ENABLE_WIN32
UITYPE=WindowsUI
endif

if ENABLE_COCOA
UITYPE=CocoaUI
endif

if ENABLE_PUGL_X11
UITYPE=X11UI
endif

if ENABLE_PUGL_WIN32
UITYPE=WindowsUI
endif

if ENABLE_PUGL_COCOA
UITYPE=CocoaUI
endif

if ENABLE_HEADLESS
UITYPE=noUI
endif

ttlgen_verbose = $(ttlgen_verbose_@AM_V@)
ttlgen_verbose_ = $(ttlgen_verbose_@AM_DEFAULT_V@)
ttlgen_verbose_0 = @echo "  TTLGEN   "$@;

manifest.ttl : ttlgen drumgizmo.la
	$(ttlgen_verbose)./ttlgen .libs/drumgizmo.so manifest.ttl $(UITYPE) > /dev/null


#######
# VST
###
if ENABLE_VST
vstplugindir = $(libdir)/vst
vstplugin_LTLIBRARIES = drumgizmo_vst.la
vstplugin_DATA =
endif

drumgizmo_vst_la_CXXFLAGS = -DVST \
	$(DEBUG_FLAGS) \
	-fvisibility=hidden \
	-I$(top_srcdir)/zita-resampler/libs \
	$(SNDFILE_CFLAGS) \
	-I$(top_srcdir)/plugin/plugingizmo \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin \
	$(VST_CPPFLAGS) -Ivst

drumgizmo_vst_la_CFLAGS = -fvisibility=hidden

drumgizmo_vst_la_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/plugin/plugingizmo/midievent.cc \
	$(top_srcdir)/plugin/plugingizmo/pluginvst.cc \
	drumgizmo_plugin.cc

drumgizmo_vst_la_LDFLAGS = -shared -module -avoid-version \
	-no-undefined -export-symbols $(top_srcdir)/plugin/drumgizmo_vst.sym

drumgizmo_vst_la_LIBADD = \
	vst/libvstsdk.la \
	$(top_builddir)/plugingui/libplugingui.la \
	$(top_builddir)/src/libdg.la \
	$(top_builddir)/src/libnls.la


install-data-hook:
	rm -f $(DESTDIR)/$(lv2plugindir)/drumgizmo.la
	rm -f $(DESTDIR)/$(vstplugindir)/drumgizmo_vst.la


EXTRA_DIST = \
	$(lv2plugin_DATA) \
	$(vstplugin_DATA) \
	drumgizmo_plugin.h \
	drumgizmo_lv2.sym \
	drumgizmo_vst.sym \
	$(top_srcdir)/plugin/plugingizmo/plugin.h \
	$(top_srcdir)/plugin/plugingizmo/midievent.h \
	$(top_srcdir)/plugin/plugingizmo/midnam_lv2.h \
	$(top_srcdir)/plugin/plugingizmo/pluginlv2.h \
	$(top_srcdir)/plugin/plugingizmo/pluginvst.h \
	$(top_srcdir)/plugin/plugingizmo/inline-display.h
