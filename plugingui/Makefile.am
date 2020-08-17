SUBDIRS = locale
DISTDIRS = locale

noinst_PROGRAMS = plugingui rcgen
noinst_LTLIBRARIES = libdggui.la

# If you add a file here, remember to add it to plugin/Makefile.mingw32.in
RES = \
	$(top_srcdir)/plugingui/resources/bg.png \
	$(top_srcdir)/plugingui/resources/bypass_button.png \
	$(top_srcdir)/plugingui/resources/font.png \
	$(top_srcdir)/plugingui/resources/fontemboss.png \
	$(top_srcdir)/plugingui/resources/help_button.png \
	$(top_srcdir)/plugingui/resources/knob.png \
	$(top_srcdir)/plugingui/resources/logo.png \
	$(top_srcdir)/plugingui/resources/png_error \
	$(top_srcdir)/plugingui/resources/progress.png \
	$(top_srcdir)/plugingui/resources/pushbutton.png \
	$(top_srcdir)/plugingui/resources/sidebar.png \
	$(top_srcdir)/plugingui/resources/slider.png \
	$(top_srcdir)/plugingui/resources/stddev_horizontal.png \
	$(top_srcdir)/plugingui/resources/stddev_horizontal_disabled.png \
	$(top_srcdir)/plugingui/resources/stddev_vertical.png \
	$(top_srcdir)/plugingui/resources/stddev_vertical_disabled.png \
	$(top_srcdir)/plugingui/resources/switch_back_off.png \
	$(top_srcdir)/plugingui/resources/switch_back_on.png \
	$(top_srcdir)/plugingui/resources/switch_front.png \
	$(top_srcdir)/plugingui/resources/tab.png \
	$(top_srcdir)/plugingui/resources/thinlistbox.png \
	$(top_srcdir)/plugingui/resources/topbar.png \
	$(top_srcdir)/plugingui/resources/toplogo.png \
	$(top_srcdir)/plugingui/resources/vertline.png \
	$(top_srcdir)/plugingui/resources/widget.png \
	$(top_srcdir)/plugingui/../ABOUT \
	$(top_srcdir)/plugingui/../AUTHORS \
	$(top_srcdir)/plugingui/../BUGS \
	$(top_srcdir)/plugingui/../COPYING

if WITH_NLS
$(top_srcdir)/plugingui/locale/drumgizmo.pot: $(GUI_SRC) $(GUI_HDR)
	(cd $(top_srcdir)/plugingui/; $(XGETTEXT) --package-name "$(PACKAGE)" --package-version "$(VERSION)" --copyright-holder="DrumGizmo Team" -k_ -o locale/drumgizmo.pot $(GUI_SRC) )
	grep -v '"POT-Creation-Date:' $(top_srcdir)/plugingui/locale/drumgizmo.pot > $(top_srcdir)/plugingui/locale/drumgizmo.pot.tmp
	mv $(top_srcdir)/plugingui/locale/drumgizmo.pot.tmp $(top_srcdir)/plugingui/locale/drumgizmo.pot

NLS_RES = \
	$(top_builddir)/plugingui/locale/da.mo \
	$(top_builddir)/plugingui/locale/fr.mo

# NOTE: Manually create initial (new) .po file with msginit
$(top_srcdir)/plugingui/locale/da.po \
$(top_srcdir)/plugingui/locale/fr.po: $(top_srcdir)/plugingui/locale/drumgizmo.pot
	$(MSGMERGE) --update $@ $(top_srcdir)/plugingui/locale/drumgizmo.pot

$(top_builddir)/plugingui/locale/da.mo: $(top_srcdir)/plugingui/locale/da.po
	$(MSGFMT) -c -v -o $@ $(top_srcdir)/plugingui/locale/da.po

$(top_builddir)/plugingui/locale/fr.mo: $(top_srcdir)/plugingui/locale/fr.po
	$(MSGFMT) -c -v -o $@ $(top_srcdir)/plugingui/locale/fr.po

endif

rcgen_verbose = $(rcgen_verbose_@AM_V@)
rcgen_verbose_ = $(rcgen_verbose_@AM_DEFAULT_V@)
rcgen_verbose_0 = @echo "  RCGEN    "$@;

BUILT_SOURCES = \
	resource_data.cc \
	$(NLS_RES)

resource_data.cc: rcgen$(EXEEXT) $(RES) $(NLS_RES)
	$(rcgen_verbose)./rcgen$(EXEEXT) -s $(top_srcdir)/plugingui/ -s $(top_builddir)/plugingui/ -o $@ $(RES) $(NLS_RES)

CLEANFILES = \
	resource_data.cc \
	$(NLS_RES)

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
	abouttab.cc \
	bleedcontrolframecontent.cc \
	button.cc \
	button_base.cc \
	checkbox.cc \
	colour.cc \
	combobox.cc \
	dialog.cc \
	diskstreamingframecontent.cc \
	drumkitframecontent.cc \
	drumkittab.cc \
	eventhandler.cc \
	filebrowser.cc \
	font.cc \
	frame.cc \
	helpbutton.cc \
	humanizerframecontent.cc \
	humaniservisualiser.cc \
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
	powerbutton.cc \
	powerwidget.cc \
	progressbar.cc \
	resamplingframecontent.cc \
	resource.cc \
	sampleselectionframecontent.cc \
	scrollbar.cc \
	slider.cc \
	stackedwidget.cc \
	statusframecontent.cc \
	tabbutton.cc \
	tabwidget.cc \
	textedit.cc \
	texture.cc \
	texturedbox.cc \
	timingframecontent.cc \
	toggle.cc \
	tooltip.cc \
	uitranslation.cc \
	utf8.cc \
	verticalline.cc \
	visualizerframecontent.cc \
	widget.cc \
	window.cc

GUI_HDR = \
	abouttab.h \
	bleedcontrolframecontent.h \
	button.h \
	button_base.h \
	canvas.h \
	checkbox.h \
	colour.h \
	combobox.h \
	dialog.h \
	diskstreamingframecontent.h \
	drawable.h \
	drumkitframecontent.h \
	drumkittab.h \
	eventhandler.h \
	filebrowser.h \
	font.h \
	frame.h \
	guievent.h \
	helpbutton.h \
	humaniservisualiser.h \
	humanizerframecontent.h \
	image.h \
	imagecache.h \
	knob.h \
	label.h \
	labeledcontrol.h \
	layout.h \
	led.h \
	lineedit.h \
	listbox.h \
	listboxbasic.h \
	listboxthin.h \
	maintab.h \
	mainwindow.h \
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
	pluginconfig.h \
	powerbutton.h \
	powerwidget.h \
	progressbar.h \
	resamplingframecontent.h \
	resource.h \
	resource_data.h \
	sampleselectionframecontent.h \
	scrollbar.h \
	slider.h \
	stackedwidget.h \
	statusframecontent.h \
	tabbutton.h \
	tabwidget.h \
	textedit.h \
	texture.h \
	texturedbox.h \
	timingframecontent.h \
	toggle.h \
	tooltip.h \
	uitranslation.h \
	utf8.h \
	verticalline.h \
	visualizerframecontent.h \
	widget.h \
	window.h

libdggui_la_SOURCES = \
	$(GUI_SRC) \
	lodepng/lodepng.cpp

nodist_libdggui_la_SOURCES = \
	$(top_builddir)/plugingui/resource_data.cc

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

plugingui_LDADD = \
	libdggui.la \
	$(top_builddir)/src/libdg.la \
	$(top_builddir)/src/libnls.la

plugingui_CXXFLAGS = \
	$(DEBUG_FLAGS) \
	$(GUI_CPPFLAGS) \
	$(SNDFILE_CXXFLAGS) \
	$(PTHREAD_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin

plugingui_CFLAGS = $(plugingui_CXXFLAGS)

plugingui_SOURCES = \
	testmain.cc \
	$(top_srcdir)/hugin/hugin.c

rcgen_CXXFLAGS = -I$(top_srcdir)/getoptpp
rcgen_SOURCES = \
	rcgen.cc

EXTRA_DIST = \
	$(RES) \
	$(GUI_HDR)
