noinst_LTLIBRARIES = libdg.la

libdg_la_CPPFLAGS = \
	-I$(top_srcdir)/hugin -I$(top_srcdir)/pugixml/src \
	$(SSEFLAGS) -D_USE_MATH_DEFINES -Wno-unused-private-field \
	-I$(top_srcdir)/zita-resampler/libs $(SNDFILE_CFLAGS) $(PTHREAD_CFLAGS)

libdg_la_LIBADD = \
	$(SNDFILE_LIBS) $(PTHREAD_LIBS)

# If you add a file here, remember to add it to plugin/Makefile.mingw32.in
nodist_libdg_la_SOURCES = \
	$(top_srcdir)/pugixml/src/pugixml.cpp \
	$(top_srcdir)/zita-resampler/libs/resampler.cc \
	$(top_srcdir)/zita-resampler/libs/resampler-table.cc \
	audiocachefile.cc \
	audiocache.cc \
	audiocacheeventhandler.cc \
	audiocacheidmanager.cc \
	audioinputenginemidi.cc \
	audiofile.cc \
	bytesizeparser.cc \
	channel.cc \
	channelmixer.cc \
	configfile.cc \
	configparser.cc \
	directory.cc \
	domloader.cc \
	dgxmlparser.cc \
	drumgizmo.cc \
	drumkit.cc \
	drumkitloader.cc \
	events.cc \
	inputprocessor.cc \
	instrument.cc \
	latencyfilter.cc \
	midimapparser.cc \
	midimapper.cc \
	path.cc \
	powerlist.cc \
	random.cc \
	sample.cc \
	sample_selection.cc \
	sem.cc \
	staminafilter.cc \
	thread.cc \
	velocityfilter.cc \
	versionstr.cc

EXTRA_DIST = \
	$(nodist_libdg_la_SOURCES) \
	DGDOM.h \
	atomic.h \
	audio.h \
	audiocache.h \
	audiocacheeventhandler.h \
	audiocachefile.h \
	audiocacheidmanager.h \
	audiofile.h \
	audioinputengine.h \
	audioinputenginemidi.h \
	audiooutputengine.h \
	audiotypes.h \
	bytesizeparser.h \
	channel.h \
	channelmixer.h \
	configfile.h \
	configparser.h \
	directory.h \
	cpp11fix.h \
	dgxmlparser.h \
	domloader.h \
	drumgizmo.h \
	drumkit.h \
	drumkitloader.h \
	event.h \
	events.h \
	grid.h \
	inputfilter.h \
	inputprocessor.h \
	instrument.h \
	latencyfilter.h \
	logger.h \
	midimapparser.h \
	midimapper.h \
	nolocale.h \
	notifier.h \
	path.h \
	platform.h \
	powerlist.h \
	random.h \
	rangemap.h \
	sample.h \
	sample_selection.h \
	sem.h \
	settings.h \
	staminafilter.h \
	syncedsettings.h \
	thread.h \
	velocityfilter.h \
	versionstr.h
