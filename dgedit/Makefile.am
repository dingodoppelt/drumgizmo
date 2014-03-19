SUBDIRS = icons
DISTDIRS = icons

if HAVE_EDITOR

bin_PROGRAMS = dgedit

dgedit_LDADD = $(SNDFILE_LIBS) $(QT_LIBS) $(AO_LIBS) \
	$(shell ../tools/MocList o ) qrc_dgedit.o 

dgedit_CXXFLAGS = $(SNDFILE_CXXFLAGS) $(QT_CFLAGS) $(AO_CFLAGS)
AM_CXXFLAGS = $(QT_CFLAGS)

dgedit_SOURCES = \
	dgedit.cc \
	audioextractor.cc \
	canvas.cc \
	canvastool.cc \
	canvastoollisten.cc \
	canvastoolselections.cc \
	canvastoolthreshold.cc \
	filelist.cc \
	itemeditor.cc \
	mainwindow.cc \
	mipmap.cc \
	samplesorter.cc

EXTRA_DIST = \
	audioextractor.h \
	canvas.h \
	canvastool.h \
	canvastoollisten.h \
	canvastoolselections.h \
	canvastoolthreshold.h \
	filelist.h \
	itemeditor.h \
	mainwindow.h \
	mipmap.h \
	samplesorter.h \
	selection.h \
	dgedit.qrc

dgedit_MOC = $(shell ../tools/MocList cc )

BUILT_SOURCES = $(dgedit_MOC) qrc_dgedit.cc

CLEANFILES = $(BUILT_SOURCES)

qrc_%.cc: %.qrc
	rcc $< > $@

%.moc.cc: %.h
	$(QT_MOC) -o $@ $<

#
# ui files not used in this project...
#%.h: %.ui
#	$(QT_UIC) -o $@ $<
#
#%.cc: %.ui
#	$(QT_UIC) -o $@ -impl $*.h $<

# command for creating .res file from .rc on Win32
%.res: %.rc
	rc $<

endif