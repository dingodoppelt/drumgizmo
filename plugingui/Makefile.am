noinst_PROGRAMS = plugingui rcgen
noinst_LTLIBRARIES = libdggui.la

resource_data.cc : rcgen
	(cd resources; ../rcgen * ../../COPYING > ../resource_data.cc)

libdggui_la_CPPFLAGS = \
	$(GUI_CPPFLAGS) \
	-I$(top_srcdir)/hugin \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/include \
	-DWITH_HUG_MUTEX $(PTHREAD_CFLAGS) \
	-DLODEPNG_NO_COMPILE_ENCODER \
	-DLODEPNG_NO_COMPILE_DISK \
	-DLODEPNG_NO_COMPILE_ANCILLARY_CHUNKS \
	-DLODEPNG_NO_COMPILE_ERROR_TEXT \
	-DLODEPNG_NO_COMPILE_CPP

libdggui_la_LIBADD = \
	$(GUI_LIBS) $(PTHREAD_LIBS)

nodist_libdggui_la_SOURCES = \
	button.cc \
	button_base.cc \
	checkbox.cc \
	colour.cc \
	combobox.cc \
	dgwindow.cc \
	dialog.cc \
	directory.cc \
	diskstreamingframecontent.cc \
	drumkitframecontent.cc \
	eventhandler.cc \
	filebrowser.cc \
	font.cc \
	frame.cc \
	humanizerframecontent.cc \
	image.cc \
	imagecache.cc \
	knob.cc \
	label.cc \
	layout.cc \
	led.cc \
	lineedit.cc \
	listbox.cc \
	listboxbasic.cc \
	listboxthin.cc \
	maintab.cc \
	mainwindow.cc \
	painter.cc \
	pixelbuffer.cc \
	pluginconfig.cc \
	plugingui.cc \
	powerbutton.cc \
	progressbar.cc \
	resource.cc \
	resource_data.cc \
	scrollbar.cc \
	slider.cc \
	stackedwidget.cc \
	statusframecontent.cc \
	tabbutton.cc \
	tabwidget.cc \
	textedit.cc \
	texture.cc \
	texturedbox.cc \
	toggle.cc \
	verticalline.cc \
	widget.cc \
	window.cc \
	lodepng/lodepng.cpp

if ENABLE_X11
nodist_libdggui_la_SOURCES += \
	nativewindow_x11.cc
endif

if ENABLE_WIN32
nodist_libdggui_la_SOURCES += \
	nativewindow_win32.cc
endif

if ENABLE_PUGL
nodist_libdggui_la_SOURCES += \
	nativewindow_pugl.cc \
	$(top_srcdir)/pugl/pugl/pugl_x11.c
endif

#if ENABLE_COCOA
#nodist_libdggui_la_SOURCES += \
#	nativewindow_cocoa.m \
#	nativewindow_cocoa.cc
#endif

plugingui_LDADD = libdggui.la $(top_srcdir)/src/libdg.la

plugingui_CXXFLAGS = $(SNDFILE_CXXFLAGS) $(PTHREAD_CFLAGS) $(EXPAT_CFLAGS) \
	-I$(top_srcdir)/include -I$(top_srcdir)/src -I$(top_srcdir)/hugin

plugingui_CFLAGS = $(plugingui_CXXFLAGS)

plugingui_SOURCES = \
	testmain.cc \
	$(top_srcdir)/hugin/hugin.c

rcgen_SOURCES = rcgen.cc

EXTRA_DIST = \
	button.h \
	button_base.h \
	canvas.h \
	checkbox.h \
	colour.h \
	combobox.h \
	dgwindow.h \
	drawable.h \
	dialog.h \
	directory.h \
	eventhandler.h \
	filebrowser.h \
	font.h \
	guievent.h \
	image.h \
	imagecache.h \
	knob.h \
	label.h \
	layout.h \
	led.h \
	lineedit.h \
	listbox.h \
	listboxbasic.h \
	listboxthin.h \
	nativewindow.h \
	nativewindow.h \
	nativewindow_win32.h \
	nativewindow_x11.h \
	painter.h \
	pixelbuffer.h \
	pluginconfig.h \
	plugingui.h \
	progressbar.h \
	resource.h \
	resource_data.h \
	scrollbar.h \
	slider.h \
	stackedwidget.h \
	tabbutton.h \
	tabwidget.h \
	textedit.h \
	texture.h \
	texturedbox.h \
	verticalline.h \
	widget.h \
	window.h \
	lodepng/lodepng.h
