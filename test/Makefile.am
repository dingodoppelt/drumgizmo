# Rules for the test code (use `make check` to execute)
SUBDIRS = dgreftest uitests

if ENABLE_TESTS

TESTS = resource enginetest paintertest resampler configfile audiocache \
	audiocachefile audiocacheidmanager audiocacheeventhandler \
	randomtest atomictest syncedsettingstest imagecachetest \
	semaphoretest drumkitcreatortest bytesizeparsertest notifiertest \
	instrumentparsertest drumkitparsertest

if ENABLE_LV2
TESTS += lv2
endif

check_PROGRAMS = $(TESTS)

resource_CXXFLAGS = -DOUTPUT=\"resource\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
resource_LDFLAGS = $(CPPUNIT_LIBS) $(SNDFILE_LIBS)
resource_SOURCES = \
	$(top_srcdir)/plugingui/resource.cc \
	$(top_srcdir)/plugingui/resource_data.cc \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/src/random.cc \
	test.cc \
	drumkit_creator.cc \
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
	$(top_srcdir)/src/semaphore.cc \
	$(top_srcdir)/src/audiofile.cc \
	$(top_srcdir)/src/random.cc \
	test.cc \
	drumkit_creator.cc \
	audiocachetest.cc

audiocachefile_CXXFLAGS = -DOUTPUT=\"audiocachefile\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN $(PTHREAD_CFLAGS) $(SNDFILE_CFLAGS)
audiocachefile_LDFLAGS = $(PTHREAD_LIBS) $(CPPUNIT_LIBS) $(SNDFILE_LIBS)
audiocachefile_SOURCES = \
	$(top_srcdir)/src/audiocachefile.cc \
	$(top_srcdir)/src/thread.cc \
	$(top_srcdir)/src/semaphore.cc \
	$(top_srcdir)/src/audiofile.cc \
	$(top_srcdir)/src/random.cc \
	test.cc \
	drumkit_creator.cc \
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
	$(top_srcdir)/src/thread.cc \
	$(top_srcdir)/src/semaphore.cc \
	test.cc \
	audiocacheeventhandlertest.cc

enginetest_CXXFLAGS = -DOUTPUT=\"enginetest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include \
	-I$(top_srcdir)/hugin $(PTHREAD_CFLAGS)
enginetest_LDFLAGS = $(CPPUNIT_LIBS) $(PTHREAD_LIBS) $(SNDFILE_LIBS) \
	 $(top_srcdir)/src/libdg.la
enginetest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	test.cc \
	drumkit_creator.cc \
	enginetest.cc

paintertest_CXXFLAGS = -DOUTPUT=\"paintertest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include -I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/hugin $(PTHREAD_CFLAGS)
paintertest_LDFLAGS = $(CPPUNIT_LIBS) $(top_srcdir)/plugingui/libdggui.la
paintertest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	test.cc \
	paintertest.cc

resampler_CXXFLAGS = -DOUTPUT=\"resampler\" $(CPPUNIT_CFLAGS) \
	$(ZITA_CXXFLAGS) $(SAMPLERATE_CFLAGS) \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN
resampler_LDFLAGS = $(ZITA_LIBS) $(SAMPLERATE_LIBS) $(CPPUNIT_LIBS)
resampler_SOURCES = \
	$(top_srcdir)/src/chresampler.cc \
	test.cc \
	resampler.cc

lv2_CXXFLAGS = -DOUTPUT=\"lv2\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src \
	 `pkg-config --cflags serd-0` `pkg-config --cflags lilv-0` \
	-DLV2_PATH=\"@LV2DIR@\"
lv2_LDFLAGS = $(CPPUNIT_LIBS) `pkg-config --libs serd-0` \
	`pkg-config --libs lilv-0` -lcrypto \
	$(SNDFILE_LIBS)
lv2_SOURCES = \
	$(top_srcdir)/src/random.cc \
	test.cc \
	drumkit_creator.cc \
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
	-I$(top_srcdir)/hugin $(PTHREAD_CFLAGS)
memchecker_LDFLAGS = $(CPPUNIT_LIBS) $(PTHREAD_LIBS) \
	$(top_srcdir)/src/libdg.la
memchecker_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	test.cc \
	drumkit_creator.cc \
	memcheckertest.cc

randomtest_CXXFLAGS = -DOUTPUT=\"randomtest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN
randomtest_CFLAGS = -DDISABLE_HUGIN
randomtest_LDFLAGS = $(CPPUNIT_LIBS)
randomtest_SOURCES = \
	$(top_srcdir)/src/random.cc \
	test.cc \
	randomtest.cc

atomictest_CXXFLAGS = -DOUTPUT=\"atomictest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/hugin
atomictest_LDFLAGS = $(CPPUNIT_LIBS)
atomictest_SOURCES = atomictest.cc test.cc

syncedsettingstest_CXXFLAGS = -DOUTPUT=\"syncedsettingstest\" \
	$(CPPUNIT_CFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)/hugin
syncedsettingstest_LDFLAGS = $(CPPUNIT_LIBS)
syncedsettingstest_SOURCES = syncedsettings.cc test.cc

EXTRA_DIST = \
	lv2_test_host.h \
	drumkit_creator.h

imagecachetest_CXXFLAGS = -DOUTPUT=\"imagecachetest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/plugingui  -I$(top_srcdir)/hugin
imagecachetest_LDFLAGS = $(CPPUNIT_LIBS) $(top_srcdir)/plugingui/libdggui.la
imagecachetest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	imagecachetest.cc \
	test.cc

semaphoretest_CXXFLAGS = -DOUTPUT=\"semaphoretest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/hugin $(PTHREAD_CFLAGS)
semaphoretest_LDFLAGS = $(CPPUNIT_LIBS) $(PTHREAD_LIBS)
semaphoretest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/src/semaphore.cc \
	semaphoretest.cc \
	test.cc

drumkitcreatortest_CXXFLAGS = -DOUTPUT=\"drumkitcreatortest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN
drumkitcreatortest_CFLAGS = -DDISABLE_HUGIN
drumkitcreatortest_LDFLAGS = $(CPPUNIT_LIBS) $(SNDFILE_LIBS)
drumkitcreatortest_SOURCES = \
	$(top_srcdir)/src/random.cc \
	test.cc \
	drumkit_creator.cc \
	drumkitcreatortest.cc

bytesizeparsertest_CXXFLAGS = -DOUTPUT=\"bytesizeparsertest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src
bytesizeparsertest_LDFLAGS = $(CPPUNIT_LIBS)
bytesizeparsertest_SOURCES = \
	$(top_srcdir)/src/bytesizeparser.cc \
	bytesizeparsertest.cc \
	test.cc

notifiertest_CXXFLAGS = -DOUTPUT=\"notifiertest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src
notifiertest_LDFLAGS = $(CPPUNIT_LIBS)
notifiertest_SOURCES = \
	notifiertest.cc \
	test.cc

instrumentparsertest_CXXFLAGS = -DOUTPUT=\"instrumentparsertest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include
instrumentparsertest_LDFLAGS = $(CPPUNIT_LIBS) \
	 $(top_srcdir)/src/libdg.la
instrumentparsertest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	instrumentparsertest.cc \
	scopedfile.cc \
	test.cc

drumkitparsertest_CXXFLAGS = -DOUTPUT=\"drumkitparsertest\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include
drumkitparsertest_LDFLAGS = $(CPPUNIT_LIBS) \
	 $(top_srcdir)/src/libdg.la
drumkitparsertest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	drumkitparsertest.cc \
	scopedfile.cc \
	test.cc

endif
