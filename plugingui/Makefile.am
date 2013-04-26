bin_PROGRAMS = plugingui rcgen

include Makefile.am.plugingui
#include $(top_srcdir)/src/Makefile.am.drumgizmo

plugingui_LDADD = $(SNDFILE_LIBS) $(PTHREAD_LIBS) $(EXPAT_LIBS) \
	 -ldl -lX11 $(PNG_LIBS)

plugingui_CXXFLAGS = $(SNDFILE_CXXFLAGS) $(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) \
	-I$(top_srcdir)/include $(PLUGIN_GUI_CFLAGS) -I$(top_srcdir)/src \
	-DSTANDALONE -DX11 -DUSE_THREAD $(PNG_CFLAGS)

plugingui_CFLAGS = $(plugingui_CXXFLAGS)

plugingui_SOURCES = \
	$(PLUGIN_GUI_SOURCES) \
	$(top_srcdir)/src/thread.cc \
	$(top_srcdir)/src/semaphore.cc

rcgen_SOURCES = rcgen.cc

EXTRA_DIST = \
	button.h \
	checkbox.h \
	colour.h \
	directory.h \
  eventhandler.h \
	font.h \
	globalcontext.h \
	guievent.h \
	image.h \
	label.h \
	led.h \
	lineedit.h \
	nativewindow.h \
	nativewindow_win32.h \
	nativewindow_x11.h \
	painter.h \
	pixelbuffer.h \
	plugingui.h \
	slider.h \
	widget.h \
	window.h \
	listbox.h \
	knob.h \
	filebrowser.h \
	combobox.h

################
# Test Section #
################

TEST_SOURCE_DEPS = ${EXTRA_DIST}
TEST_SCRIPT_DIR = $(top_srcdir)/tools

include ${TEST_SCRIPT_DIR}/Makefile.am.test

include Makefile.am.test
