SUBDIRS = locale
DISTDIRS = locale

noinst_PROGRAMS = plugingui
noinst_LTLIBRARIES = libplugingui.la

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
	$(top_srcdir)/plugingui/../ABOUT.fr \
	$(top_srcdir)/plugingui/../AUTHORS \
	$(top_srcdir)/plugingui/../AUTHORS.fr \
	$(top_srcdir)/plugingui/../BUGS \
	$(top_srcdir)/plugingui/../BUGS.fr \
	$(top_srcdir)/plugingui/../COPYING

if WITH_NLS
$(top_srcdir)/plugingui/locale/drumgizmo.pot: $(GUI_SRC) $(GUI_HDR)
	(cd $(top_srcdir)/plugingui/; $(XGETTEXT) --package-name "$(PACKAGE)" --package-version "$(VERSION)" --copyright-holder="DrumGizmo Team" -k_ -o locale/drumgizmo.pot $(GUI_SRC) $(GUI_HDR) )
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

resource_data.cc: $(top_builddir)/dggui/rcgen$(EXEEXT) $(RES) $(NLS_RES)
	$(rcgen_verbose)$(top_builddir)/dggui/rcgen$(EXEEXT) -s $(top_srcdir)/plugingui/ -s $(top_builddir)/plugingui/ -o $@ $(RES) $(NLS_RES)

CLEANFILES = \
	resource_data.cc \
	$(NLS_RES)

libplugingui_la_CPPFLAGS = \
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

libplugingui_la_CFLAGS =

libplugingui_la_LIBTOOLFLAGS=--tag=CC

libplugingui_la_LIBADD = \
	$(top_builddir)/dggui/libdggui.la \
	$(GUI_LIBS) $(PTHREAD_LIBS)

# If you add a file here, remember to add it to plugin/Makefile.mingw32.in
GUI_SRC = \
	abouttab.cc \
	bleedcontrolframecontent.cc \
	diskstreamingframecontent.cc \
	drumkitframecontent.cc \
	drumkittab.cc \
	filebrowser.cc \
	humaniservisualiser.cc \
	humanizerframecontent.cc \
	maintab.cc \
	mainwindow.cc \
	pluginconfig.cc \
	powerwidget.cc \
	resamplingframecontent.cc \
	sampleselectionframecontent.cc \
	statusframecontent.cc \
	timingframecontent.cc \
	visualizerframecontent.cc \
	voicelimitframecontent.cc

GUI_HDR = \
	abouttab.h \
	bleedcontrolframecontent.h \
	diskstreamingframecontent.h \
	drumkitframecontent.h \
	drumkittab.h \
	filebrowser.h \
	humaniservisualiser.h \
	humanizerframecontent.h \
	labeledcontrol.h \
	maintab.h \
	mainwindow.h \
	pluginconfig.h \
	powerwidget.h \
	resamplingframecontent.h \
	sampleselectionframecontent.h \
	statusframecontent.h \
	timingframecontent.h \
	visualizerframecontent.h \
	voicelimitframecontent.h

libplugingui_la_SOURCES = \
	$(GUI_SRC)

nodist_libplugingui_la_SOURCES = \
	$(top_builddir)/plugingui/resource_data.cc

plugingui_LDADD = \
	libplugingui.la \
	$(top_builddir)/src/libdg.la \
	$(top_builddir)/src/libnls.la

plugingui_CXXFLAGS = \
	$(DEBUG_FLAGS) \
	$(GUI_CPPFLAGS) \
	$(SNDFILE_CXXFLAGS) \
	$(PTHREAD_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin

plugingui_CFLAGS = $(plugingui_CXXFLAGS)

plugingui_SOURCES = \
	testmain.cc \
	$(top_srcdir)/hugin/hugin.c

EXTRA_DIST = \
	$(RES) \
	$(GUI_HDR)
