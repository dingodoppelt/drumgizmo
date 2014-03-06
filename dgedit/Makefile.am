if HAVE_EDITOR

bin_PROGRAMS = dgedit

dgedit_LDADD = $(SNDFILE_LIBS) $(QT_LIBS) $(shell ../tools/MocList o ) \
	$(AO_LIBS)

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
	samplesorter.cc \
	dgedit.qrc

#	dgedit_qrc.cpp $(shell ../tools/MocList cc ) 

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
	selection.h

dgedit_MOC = $(shell ../tools/MocList cc )

dgedit_qrc.cpp: dgedit.qrc
	rcc $< > $@

BUILT_SOURCES = $(dgedit_MOC) dgedit_qrc.cpp

CLEANFILES = $(BUILT_SOURCES)

%.moc.cc: %.h
	$(QT_MOC) -o $@ $<

%.h: %.ui
	$(QT_UIC) -o $@ $<

%.cc: %.ui
	$(QT_UIC) -o $@ -impl $*.h $<

# command for creating .res file from .rc on Win32
%.res: %.rc
	rc $<

endif