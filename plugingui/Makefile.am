bin_PROGRAMS = plugingui

plugingui_LDADD = $(SNDFILE_LIBS) $(PTHREAD_LIBS) $(EXPAT_LIBS) -ldl -lX11

plugingui_CXXFLAGS = $(SNDFILE_CXXFLAGS) $(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/src -DSTANDALONE -DX11

plugingui_SOURCES = \
	eventhandler.cc \
	globalcontext.cc \
	gui.cc \
	colour.cc \
	window.cc \
	widget.cc \
	lineedit.cc \
	button.cc \
	checkbox.cc \
	combobox.cc \
	label.cc \
	painter.cc

EXTRA_DIST = \
	event.h \
	eventhandler.h \
	globalcontext.h \
	gui.h \
	colour.h \
	window.h \
	widget.h \
	lineedit.h \
	button.h \
	checkbox.h \
	combobox.h \
	label.h \
	painter.h

################
# Test Section #
################

TEST_SOURCE_DEPS = ${EXTRA_DIST}
TEST_SCRIPT_DIR = $(top_srcdir)/tools

include ${TEST_SCRIPT_DIR}/Makefile.am.test

include Makefile.am.test
