bin_PROGRAMS = dgedit

dgedit_LDADD = $(SNDFILE_LIBS) $(QT_LIBS) $(shell ../tools/MocList o )

dgedit_CXXFLAGS = $(SNDFILE_CXXFLAGS) $(QT_CFLAGS)
CXXFLAGS += $(QT_CFLAGS)

dgedit_SOURCES = $(shell ../tools/MocList cc ) \
	dgedit.cc \
	audioextractor.cc \
	canvas.cc \
	filelist.cc \
	itemeditor.cc \
	mainwindow.cc \
	samplesorter.cc

EXTRA_DIST = \
	audioextractor.h \
	canvas.h \
	filelist.h \
	itemeditor.h \
	mainwindow.h \
	samplesorter.h

dgedit_MOC = $(shell ../tools/MocList cc )

BUILT_SOURCES = $(dgedit_MOC)

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
