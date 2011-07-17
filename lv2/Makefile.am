lv2sources = \
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
	$(top_srcdir)/src/midimapper.cc \
	$(top_srcdir)/src/mutex.cc \
	$(top_srcdir)/src/path.cc \
	$(top_srcdir)/src/sample.cc \
	$(top_srcdir)/src/saxparser.cc \
	$(top_srcdir)/src/thread.cc \
	$(top_srcdir)/src/velocity.cc

if WITH_LV2

lv2ltlibs = liblv2.la
lv2buildsources = $(lv2sources)

else

lv2ltlibs =
lv2buildsources =

endif

EXTRA_DIST = $(lv2sources)

lib_LTLIBRARIES = $(lv2ltlibs)

libdir = $(LV2_PLUGIN_DIR)

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/include $(SNDFILE_CXXFLAGS) \
	 $(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) $(LV2_CFLAGS)
liblv2_la_LDFLAGS = $(SNDFILE_LIBS) $(EXPAT_LIBS)  $(LV2_LIBS)
liblv2_la_LIBADD =
liblv2_la_SOURCES = $(lv2buildsources)
