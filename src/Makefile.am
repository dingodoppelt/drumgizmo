noinst_LTLIBRARIES = libdg.la

libdg_la_CPPFLAGS = \
	-I$(top_srcdir)/include -I$(top_srcdir)/hugin \
	$(SSEFLAGS) \
	$(ZITA_CPPFLAGS) $(SNDFILE_CFLAGS) $(EXPAT_CFLAGS) \
	$(PTHREAD_CFLAGS)

libdg_la_LIBADD = \
	$(ZITA_LIBS) $(SNDFILE_LIBS) $(EXPAT_LIBS) \
	$(PTHREAD_LIBS)

# If you add a file here, remember to add it to plugin/Makefile.mingw32.in
nodist_libdg_la_SOURCES = \
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
	drumgizmo.cc \
	drumkit.cc \
	drumkitloader.cc \
	drumkitparser.cc \
	events.cc \
	inputprocessor.cc \
	instrument.cc \
	instrumentparser.cc \
	latencyfilter.cc \
	midimapparser.cc \
	midimapper.cc \
	path.cc \
	powerlist.cc \
	random.cc \
	sample.cc \
	semaphore.cc \
	saxparser.cc \
	staminafilter.cc \
	thread.cc \
	versionstr.cc

EXTRA_DIST = \
	$(nodist_libdg_la_SOURCES) \
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
	beatmapper.h \
	bytesizeparser.h \
	channel.h \
	channelmixer.h \
	configfile.h \
	configparser.h \
	cpp11fix.h \
	drumgizmo.h \
	drumkit.h \
	drumkitloader.h \
	drumkitparser.h \
	events.h \
	inputfilter.h \
	inputprocessor.h \
	instrument.h \
	instrumentparser.h \
	latencyfilter.h \
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
	saxparser.h \
	semaphore.h \
	settings.h \
	staminafilter.h \
	syncedsettings.h \
	thread.h \
	versionstr.h
