bin_PROGRAMS = plugingui

plugingui_LDADD = $(SNDFILE_LIBS) $(PTHREAD_LIBS) $(EXPAT_LIBS) -ldl -lX11

plugingui_CXXFLAGS = $(SNDFILE_CXXFLAGS) $(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/src -DSTANDALONE -DX11 -DUSE_THREAD

include Makefile.am.plugingui
include $(top_srcdir)/src/Makefile.am.drumgizmo

plugingui_SOURCES = \
	$(PLUGIN_GUI_SOURCES)

#	$(DRUMGIZMO_SOURCES)

EXTRA_DIST = \
	guievent.h \
	eventhandler.h \
	globalcontext.h \
	plugingui.h \
	pixelbuffer.h \
	colour.h \
	font.h \
	filelineedit.h \
	led.h \
	window.h \
	widget.h \
	lineedit.h \
	button.h \
	label.h \
	painter.h \
	img_font.h \
	img_back.h \
	checkbox.h \
	slider.h

################
# Test Section #
################

TEST_SOURCE_DEPS = ${EXTRA_DIST}
TEST_SCRIPT_DIR = $(top_srcdir)/tools

include ${TEST_SCRIPT_DIR}/Makefile.am.test

include Makefile.am.test
