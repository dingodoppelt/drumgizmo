# Rules for the test code (use `make check` to execute)
SUBDIRS = dgreftest uitests

if ENABLE_TESTS

TESTS = resource enginetest paintertest configfile audiocache \
	audiocachefile audiocacheidmanager audiocacheeventhandler \
	randomtest atomictest syncedsettingstest imagecachetest \
	semaphoretest drumkitcreatortest bytesizeparsertest notifiertest \
	dgxmlparsertest domloadertest configparsertest midimapparsertest \
	eventsdstest powermaptest midimappertest

if WITH_NLS
TESTS += translationtest
endif

EXTRA_DIST = \
	drumkit_creator.h \
	lv2_test_host.h \
	scopedfile.h \
	locale/da.mo

if ENABLE_LV2
TESTS += lv2
endif

noinst_PROGRAMS = $(TESTS)
check_PROGRAMS = $(TESTS)

resource_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"resource\" \
	$(SNDFILE_CFLAGS) \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/hugin
resource_LDFLAGS = $(SNDFILE_LIBS)
nodist_resource_SOURCES = \
	$(top_builddir)/plugingui/resource_data.cc
resource_SOURCES = \
	$(top_srcdir)/dggui/resource.cc \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/src/random.cc \
	uunit/uunit.cc \
	drumkit_creator.cc \
	resource_test.cc

audiocache_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"audiocache\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN \
	$(PTHREAD_CFLAGS) \
	$(SNDFILE_CFLAGS)  \
	-Wno-deprecated-declarations
audiocache_LDFLAGS = $(PTHREAD_LIBS) $(SNDFILE_LIBS)
audiocache_SOURCES = \
	$(top_srcdir)/src/audiocache.cc \
	$(top_srcdir)/src/audiocacheeventhandler.cc \
	$(top_srcdir)/src/audiocachefile.cc \
	$(top_srcdir)/src/audiocacheidmanager.cc \
	$(top_srcdir)/src/thread.cc \
	$(top_srcdir)/src/sem.cc \
	$(top_srcdir)/src/audiofile.cc \
	$(top_srcdir)/src/random.cc \
	uunit/uunit.cc \
	drumkit_creator.cc \
	audiocachetest.cc

audiocachefile_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"audiocachefile\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN \
	$(PTHREAD_CFLAGS) \
	$(SNDFILE_CFLAGS) \
	-Wno-deprecated-declarations
audiocachefile_LDFLAGS = $(PTHREAD_LIBS) $(SNDFILE_LIBS)
audiocachefile_SOURCES = \
	$(top_srcdir)/src/audiocachefile.cc \
	$(top_srcdir)/src/thread.cc \
	$(top_srcdir)/src/sem.cc \
	$(top_srcdir)/src/audiofile.cc \
	$(top_srcdir)/src/random.cc \
	uunit/uunit.cc \
	drumkit_creator.cc \
	audiocachefiletest.cc

audiocacheidmanager_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"audiocacheidmanager\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN \
	$(SNDFILE_CFLAGS)
audiocacheidmanager_LDFLAGS = $(SNDFILE_LIBS)
audiocacheidmanager_SOURCES = \
	$(top_srcdir)/src/audiocacheidmanager.cc \
	uunit/uunit.cc \
	audiocacheidmanagertest.cc

audiocacheeventhandler_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"audiocacheeventhandler\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN \
	$(PTHREAD_CFLAGS) \
	$(SNDFILE_CFLAGS) \
	-Wno-deprecated-declarations
audiocacheeventhandler_LDFLAGS = $(PTHREAD_LIBS) $(SNDFILE_LIBS)
audiocacheeventhandler_SOURCES = \
	$(top_srcdir)/src/audiocacheeventhandler.cc \
	$(top_srcdir)/src/audiocacheidmanager.cc \
	$(top_srcdir)/src/audiocachefile.cc \
	$(top_srcdir)/src/thread.cc \
	$(top_srcdir)/src/sem.cc \
	uunit/uunit.cc \
	audiocacheeventhandlertest.cc

enginetest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"enginetest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin $(PTHREAD_CFLAGS) \
	$(SNDFILE_CFLAGS)
enginetest_LDFLAGS = $(PTHREAD_LIBS) $(SNDFILE_LIBS) \
	 $(top_builddir)/src/libdg.la
enginetest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	uunit/uunit.cc \
	drumkit_creator.cc \
	enginetest.cc

paintertest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"paintertest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/hugin \
	$(PTHREAD_CFLAGS)
paintertest_LDFLAGS = \
	$(top_builddir)/dggui/libdggui.la
paintertest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	uunit/uunit.cc \
	paintertest.cc \
	$(top_builddir)/plugingui/resource_data.cc

lv2_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"lv2\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	`pkg-config --cflags serd-0` `pkg-config --cflags lilv-0` \
	-DLV2_PATH=\"@LV2DIR@\" $(SNDFILE_CFLAGS)
lv2_LDFLAGS = `pkg-config --libs serd-0` \
	`pkg-config --libs lilv-0` -lcrypto \
	$(SNDFILE_LIBS)
lv2_SOURCES = \
	$(top_srcdir)/src/random.cc \
	uunit/uunit.cc \
	drumkit_creator.cc \
	lv2_test_host.cc \
	lv2.cc

configfile_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"configfile\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/hugin \
	-I$(top_srcdir)/src
configfile_LDFLAGS =
configfile_SOURCES = \
	$(top_srcdir)/src/configfile.cc \
	$(top_srcdir)/src/directory.cc \
	$(top_srcdir)/hugin/hugin.c \
	uunit/uunit.cc \
	configtest.cc

randomtest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"randomtest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN
randomtest_CFLAGS = -DDISABLE_HUGIN
randomtest_LDFLAGS =
randomtest_SOURCES = \
	$(top_srcdir)/src/random.cc \
	uunit/uunit.cc \
	randomtest.cc

atomictest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"atomictest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
atomictest_LDFLAGS =
atomictest_SOURCES = atomictest.cc uunit/uunit.cc

syncedsettingstest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"syncedsettingstest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
syncedsettingstest_LDFLAGS =
syncedsettingstest_SOURCES = syncedsettings.cc uunit/uunit.cc

imagecachetest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"imagecachetest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/hugin
imagecachetest_LDFLAGS = \
	$(top_builddir)/dggui/libdggui.la \
	$(top_builddir)/plugingui/libplugingui.la
imagecachetest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	imagecachetest.cc \
	uunit/uunit.cc

semaphoretest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"semaphoretest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin \
	$(PTHREAD_CFLAGS) \
	-Wno-deprecated-declarations
semaphoretest_LDFLAGS = $(PTHREAD_LIBS)
semaphoretest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/src/sem.cc \
	semaphoretest.cc \
	uunit/uunit.cc

drumkitcreatortest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"drumkitcreatortest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN \
	$(SNDFILE_CFLAGS)
drumkitcreatortest_CFLAGS = -DDISABLE_HUGIN
drumkitcreatortest_LDFLAGS = $(SNDFILE_LIBS)
drumkitcreatortest_SOURCES = \
	$(top_srcdir)/src/random.cc \
	uunit/uunit.cc \
	drumkit_creator.cc \
	drumkitcreatortest.cc

bytesizeparsertest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"bytesizeparsertest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src
bytesizeparsertest_LDFLAGS =
bytesizeparsertest_SOURCES = \
	$(top_srcdir)/src/bytesizeparser.cc \
	bytesizeparsertest.cc \
	uunit/uunit.cc

notifiertest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"notifiertest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src
notifiertest_LDFLAGS =
notifiertest_SOURCES = \
	notifiertest.cc \
	uunit/uunit.cc

dgxmlparsertest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"dgxmlparsertest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin \
	-I$(top_srcdir)/pugixml/src
dgxmlparsertest_LDFLAGS =
dgxmlparsertest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/src/dgxmlparser.cc \
	$(top_srcdir)/pugixml/src/pugixml.cpp \
	dgxmlparsertest.cc \
	scopedfile.cc \
	uunit/uunit.cc

domloadertest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"domloadertest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src
domloadertest_LDFLAGS = \
	 $(top_builddir)/src/libdg.la
domloadertest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	domloadertest.cc \
	scopedfile.cc \
	uunit/uunit.cc

configparsertest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"configparsertest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin \
	-I$(top_srcdir)/pugixml/src
configparsertest_LDFLAGS =
configparsertest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/src/configparser.cc \
	$(top_srcdir)/pugixml/src/pugixml.cpp \
	configparsertest.cc \
	uunit/uunit.cc

midimapparsertest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"midimapparsertest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin \
	-I$(top_srcdir)/pugixml/src
midimapparsertest_LDFLAGS =
midimapparsertest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/src/midimapparser.cc \
	$(top_srcdir)/pugixml/src/pugixml.cpp \
	scopedfile.cc \
	midimapparsertest.cc \
	uunit/uunit.cc

eventsdstest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"eventsdstest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src
eventsdstest_LDFLAGS =
eventsdstest_SOURCES = \
	$(top_srcdir)/src/events_ds.cc \
	eventsdstest.cc \
	uunit/uunit.cc

powermaptest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"powermaptest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src
powermaptest_LDFLAGS =
powermaptest_SOURCES = \
	$(top_srcdir)/src/powermap.cc \
	powermaptest.cc \
	uunit/uunit.cc

midimappertest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"midimappertest\" \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin \
	-I$(top_srcdir)/pugixml/src
midimappertest_LDFLAGS =
midimappertest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/src/midimapper.cc \
	$(top_srcdir)/pugixml/src/pugixml.cpp \
	scopedfile.cc \
	midimappertest.cc \
	uunit/uunit.cc

RES = \
	$(top_srcdir)/test/locale/da.mo

rcgen_verbose = $(rcgen_verbose_@AM_V@)
rcgen_verbose_ = $(rcgen_verbose_@AM_DEFAULT_V@)
rcgen_verbose_0 = @echo "  RCGEN    "$@;

translationtest_resource_data.cc: $(top_builddir)/dggui/rcgen $(RES)
	$(rcgen_verbose)$(top_builddir)/dggui/rcgen$(EXEEXT) -s $(top_srcdir)/test/ -o $@ $(RES)

translationtest_CXXFLAGS = \
	-I$(top_srcdir)/test/uunit -DOUTPUT=\"translationtest\" \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin \
	-DMO_SRC=\"$(top_srcdir)/test/locale/da.mo\" \
	-Wno-invalid-source-encoding
translationtest_LDFLAGS =
nodist_translationtest_SOURCES = \
	$(top_builddir)/test/translationtest_resource_data.cc
translationtest_SOURCES = \
	$(top_srcdir)/hugin/hugin.c \
	$(top_srcdir)/dggui/resource.cc \
	$(top_srcdir)/src/translation.cc \
	$(top_srcdir)/dggui/uitranslation.cc \
	translationtest.cc \
	uunit/uunit.cc

BUILT_SOURCES = translationtest_resource_data.cc
CLEANFILES = translationtest_resource_data.cc

endif
