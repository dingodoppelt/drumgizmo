# Rules for the test code (use `make check` to execute)
include $(top_srcdir)/src/Makefile.am.drumgizmo

TESTS = resource engine gui resampler lv2 configfile audiocache \
	audiocachefile audiocacheidmanager audiocacheeventhandler \
	memchecker random atomictest

check_PROGRAMS = $(TESTS)

resource_CXXFLAGS = -DOUTPUT=\"resource\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/hugin
resource_LDFLAGS = $(CPPUNIT_LIBS)
resource_SOURCES = \
	$(top_srcdir)/plugingui/resource.cc \
	$(top_srcdir)/plugingui/resource_data.cc \
	$(top_srcdir)/hugin/hugin.c \
	test.cc \
	resource_test.cc

audiocache_CXXFLAGS = -DOUTPUT=\"audiocache\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN $(PTHREAD_CFLAGS) $(SNDFILE_CFLAGS)
audiocache_LDFLAGS = $(PTHREAD_LIBS) $(CPPUNIT_LIBS) $(SNDFILE_LIBS)
audiocache_SOURCES = \
	$(top_srcdir)/src/audiocache.cc \
	$(top_srcdir)/src/audiocacheeventhandler.cc \
	$(top_srcdir)/src/audiocachefile.cc \
	$(top_srcdir)/src/audiocacheidmanager.cc \
	$(top_srcdir)/src/thread.cc \
	$(top_srcdir)/src/mutex.cc \
	$(top_srcdir)/src/semaphore.cc \
	$(top_srcdir)/src/configuration.cc \
	$(top_srcdir)/src/audiofile.cc \
	test.cc \
	audiocachetest.cc

audiocachefile_CXXFLAGS = -DOUTPUT=\"audiocachefile\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN $(PTHREAD_CFLAGS) $(SNDFILE_CFLAGS)
audiocachefile_LDFLAGS = $(PTHREAD_LIBS) $(CPPUNIT_LIBS) $(SNDFILE_LIBS)
audiocachefile_SOURCES = \
	$(top_srcdir)/src/audiocachefile.cc \
	$(top_srcdir)/src/thread.cc \
	$(top_srcdir)/src/mutex.cc \
	$(top_srcdir)/src/semaphore.cc \
	$(top_srcdir)/src/configuration.cc \
	$(top_srcdir)/src/audiofile.cc \
	test.cc \
	audiocachefiletest.cc

audiocacheidmanager_CXXFLAGS = -DOUTPUT=\"audiocacheidmanager\" \
	$(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN $(SNDFILE_CFLAGS)
audiocacheidmanager_LDFLAGS = $(CPPUNIT_LIBS) $(SNDFILE_LIBS)
audiocacheidmanager_SOURCES = \
	$(top_srcdir)/src/audiocacheidmanager.cc \
	test.cc \
	audiocacheidmanagertest.cc

audiocacheeventhandler_CXXFLAGS = -DOUTPUT=\"audiocacheeventhandler\" \
	$(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN $(PTHREAD_CFLAGS) $(SNDFILE_CFLAGS)
audiocacheeventhandler_LDFLAGS = $(PTHREAD_LIBS) $(CPPUNIT_LIBS) $(SNDFILE_LIBS)
audiocacheeventhandler_SOURCES = \
	$(top_srcdir)/src/audiocacheeventhandler.cc \
	$(top_srcdir)/src/audiocacheidmanager.cc \
	$(top_srcdir)/src/audiocachefile.cc \
	$(top_srcdir)/src/mutex.cc \
	$(top_srcdir)/src/thread.cc \
	$(top_srcdir)/src/semaphore.cc \
	test.cc \
	audiocacheeventhandlertest.cc

engine_CXXFLAGS = -DOUTPUT=\"engine\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN $(PTHREAD_CFLAGS)
engine_CFLAGS = -DDISABLE_HUGIN
engine_LDFLAGS = $(CPPUNIT_LIBS) $(DRUMGIZMO_LIBS) $(PTHREAD_LIBS)
engine_SOURCES = \
	$(DRUMGIZMO_SOURCES) \
	$(top_srcdir)/hugin/hugin.c \
	test.cc \
	engine.cc

gui_CXXFLAGS = -DOUTPUT=\"gui\" $(CPPUNIT_CFLAGS)
gui_LDFLAGS = $(CPPUNIT_LIBS)
gui_SOURCES = \
	test.cc \
	gui.cc

resampler_CXXFLAGS = -DOUTPUT=\"resampler\" $(CPPUNIT_CFLAGS) \
	$(ZITA_CXXFLAGS) $(SAMPLERATE_CFLAGS) \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN
resampler_LDFLAGS = $(ZITA_LIBS) $(SAMPLERATE_LIBS) $(CPPUNIT_LIBS)
resampler_SOURCES = \
	$(top_srcdir)/src/chresampler.cc \
	test.cc \
	resampler.cc

lv2_CXXFLAGS = -DOUTPUT=\"lv2\" $(CPPUNIT_CFLAGS) \
	 `pkg-config --cflags serd-0` `pkg-config --cflags lilv-0` \
	-DLV2_PATH=\"$(libdir)/lv2\"
lv2_LDFLAGS = $(CPPUNIT_LIBS) `pkg-config --libs serd-0` \
	`pkg-config --libs lilv-0` -lcrypto
lv2_SOURCES = \
	test.cc \
	lv2_test_host.cc \
	lv2.cc

configfile_CXXFLAGS = -DOUTPUT=\"configfile\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/hugin
configfile_LDFLAGS = $(CPPUNIT_LIBS)
configfile_SOURCES = \
	$(top_srcdir)/src/configfile.cc \
	$(top_srcdir)/hugin/hugin.c \
	test.cc \
	configtest.cc

memchecker_CXXFLAGS = -DOUTPUT=\"memchecker\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN $(PTHREAD_CFLAGS)
memcheckere_CFLAGS = -DDISABLE_HUGIN
memchecker_LDFLAGS = $(CPPUNIT_LIBS) $(DRUMGIZMO_LIBS) $(PTHREAD_LIBS)
memchecker_SOURCES = \
	$(DRUMGIZMO_SOURCES) \
	test.cc \
	memcheckertest.cc

random_CXXFLAGS = -DOUTPUT=\"random\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN
random_CFLAGS = -DDISABLE_HUGIN
random_LDFLAGS = $(CPPUNIT_LIBS)
random_SOURCES = \
	$(top_srcdir)/src/random.cc \
	test.cc \
	randomtest.cc

atomictest_CXXFLAGS = -DOUTPUT=\"atomictest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/hugin
atomictest_LDFLAGS = $(CPPUNIT_LIBS)
atomictest_SOURCES = atomictest.cc test.cc

EXTRA_DIST = \
	lv2_test_host.h
