if ENABLE_LV2

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/include $(SNDFILE_CXXFLAGS) \
	 $(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) $(LV2_CFLAGS)
lv2plugindir = $(libdir)/lv2/drumgizmo.lv2
lv2plugin_LTLIBRARIES = drumgizmo.la
lv2plugin_DATA = manifest.ttl drumgizmo.ttl
EXTRA_DIST = $(lv2plugin_DATA)

drumgizmo_la_SOURCES = 
	lv2.cc \
	input_lv2.cc \
	output_lv2.cc \
	$(top_srcdir)/src/audiofile.cc \
	$(top_srcdir)/src/channel.cc \
	$(top_srcdir)/src/channelmixer.cc \
	$(top_srcdir)/src/drumgizmo.cc \
	$(top_srcdir)/src/drumkit.cc \
	$(top_srcdir)/src/drumkitparser.cc \
	$(top_srcdir)/src/events.cc \
	$(top_srcdir)/src/instrument.cc \
	$(top_srcdir)/src/instrumentparser.cc \
	$(top_srcdir)/src/midimapparser.cc \
	$(top_srcdir)/src/midimapper.cc \
	$(top_srcdir)/src/mutex.cc \
	$(top_srcdir)/src/path.cc \
	$(top_srcdir)/src/sample.cc \
	$(top_srcdir)/src/saxparser.cc \
	$(top_srcdir)/src/velocity.cc
	lv2_event.h \
	lv2-saverestore.h

#	$(top_srcdir)/plugingui/gui.cc

drumgizmo_la_LDFLAGS = -module -avoid-version
drumgizmo_la_LIBADD = $(SNDFILE_LIBS) $(EXPAT_LIBS)  $(LV2_LIBS) -lX11

endif
