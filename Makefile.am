AUTOMAKE_OPTIONS = gnu
SUBDIRS = tools src plugin drumgizmo
DISTDIRS = tools src plugin drumgizmo
if !ENABLE_HEADLESS
SUBDIRS += dggui plugingui test man
DISTDIRS += dggui plugingui test man
endif

EXTRA_DIST = \
	version.h \
\
	hugin/hugin.h \
	hugin/hugin.hpp \
	hugin/hugin.c \
	hugin/hugin_util.h \
	hugin/hugin_syslog.h \
	hugin/hugin_syslog.c \
	hugin/hugin_filter.h \
	hugin/hugin_filter.c \
\
	getoptpp/getoptpp.hpp \
\
\
	pugixml/src/pugixml.hpp \
	pugixml/src/pugiconfig.hpp \
	pugixml/src/pugixml.cpp \
\
	zita-resampler/libs/resampler.cc \
	zita-resampler/libs/resampler-table.cc \
	zita-resampler/libs/zita-resampler/resampler-table.h \
	zita-resampler/libs/zita-resampler/resampler.h
if !ENABLE_HEADLESS
EXTRA_DIST += dggui/lodepng/lodepng.h test/uunit/uunit.h test/uunit/uunit.cc
endif
