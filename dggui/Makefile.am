noinst_PROGRAMS = rcgen
noinst_LTLIBRARIES = libdggui.la

libdggui_la_CPPFLAGS = \
	$(DEBUG_FLAGS) \
	-I$(top_srcdir) \
	$(GUI_CPPFLAGS) \
	-I$(top_srcdir)/hugin \
	-I$(top_srcdir)/src \
	-DWITH_HUG_MUTEX $(PTHREAD_CFLAGS) \
	-DLODEPNG_NO_COMPILE_ENCODER \
	-DLODEPNG_NO_COMPILE_DISK \
	-DLODEPNG_NO_COMPILE_ANCILLARY_CHUNKS \
	-DLODEPNG_NO_COMPILE_ERROR_TEXT \
	-DLODEPNG_NO_COMPILE_CPP

libdggui_la_CFLAGS =

libdggui_la_LIBTOOLFLAGS=--tag=CC

libdggui_la_LIBADD = \
	$(GUI_LIBS) $(PTHREAD_LIBS)

# If you add a file here, remember to add it to plugin/Makefile.mingw32.in
GUI_SRC = \
	button.cc \
	button_base.cc \
	checkbox.cc \
	colour.cc \
	combobox.cc \
	dialog.cc \
	eventhandler.cc \
	font.cc \
	frame.cc \
	helpbutton.cc \
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
	painter.cc \
	pixelbuffer.cc \
	powerbutton.cc \
	progressbar.cc \
	rc_data.cc \
	resource.cc \
	scrollbar.cc \
	slider.cc \
	stackedwidget.cc \
	tabbutton.cc \
	tabwidget.cc \
	textedit.cc \
	texture.cc \
	texturedbox.cc \
	toggle.cc \
	tooltip.cc \
	uitranslation.cc \
	utf8.cc \
	verticalline.cc \
	widget.cc \
	window.cc

GUI_HDR = \
	button.h \
	button_base.h \
	canvas.h \
	checkbox.h \
	colour.h \
	combobox.h \
	dialog.h \
	drawable.h \
	eventhandler.h \
	font.h \
	frame.h \
	guievent.h \
	helpbutton.h \
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
	nativewindow_cocoa.h \
	nativewindow_cocoa.mm \
	nativewindow_pugl.h \
	nativewindow_pugl.cc \
	nativewindow_win32.h \
	nativewindow_win32.cc \
	nativewindow_x11.h \
	nativewindow_x11.cc \
	painter.h \
	pixelbuffer.h \
	powerbutton.h \
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
	toggle.h \
	tooltip.h \
	uitranslation.h \
	utf8.h \
	verticalline.h \
	widget.h \
	window.h

libdggui_la_SOURCES = \
	$(GUI_SRC) \
	lodepng/lodepng.cpp

nodist_libdggui_la_SOURCES =

if ENABLE_X11
nodist_libdggui_la_SOURCES += \
	nativewindow_x11.cc
endif

if ENABLE_WIN32
nodist_libdggui_la_SOURCES += \
	nativewindow_win32.cc
endif

if ENABLE_COCOA
nodist_libdggui_la_SOURCES += \
	nativewindow_cocoa.mm

libdggui_la_OBJCXXFLAGS = \
	-fblocks
endif

if ENABLE_PUGL_X11
nodist_libdggui_la_SOURCES += \
	nativewindow_pugl.cc \
	$(top_srcdir)/pugl/pugl/pugl_x11.c

libdggui_la_CPPFLAGS += \
	-I$(top_srcdir)/pugl

libdggui_la_CFLAGS += \
	-std=c99
endif

if ENABLE_PUGL_WIN32
nodist_libdggui_la_SOURCES += \
	nativewindow_pugl.cc \
	$(top_srcdir)/pugl/pugl/pugl_win.cpp

libdggui_la_CPPFLAGS += \
	-I$(top_srcdir)/pugl
endif

if ENABLE_PUGL_COCOA
nodist_libdggui_la_SOURCES += \
	nativewindow_pugl.cc \
	$(top_srcdir)/pugl/pugl/pugl_osx.m

libdggui_la_CPPFLAGS += \
	-I$(top_srcdir)/pugl
endif

rcgen_LDFLAGS = -static
rcgen_CXXFLAGS = -I$(top_srcdir)/getoptpp
rcgen_SOURCES = \
	rcgentool.cc

EXTRA_DIST = \
	$(RES) \
	$(GUI_HDR)
