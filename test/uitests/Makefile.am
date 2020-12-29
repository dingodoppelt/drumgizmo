noinst_PROGRAMS = resizetest tabwidgettest framewidgettest \
	filebrowsertest benchmarktest powerwidgettest

resizetest_LDADD = \
	$(top_builddir)/dggui/libdggui.la \
	$(top_builddir)/plugingui/libplugingui.la \
	$(top_builddir)/src/libdg.la \
	$(top_builddir)/src/libnls.la
resizetest_CXXFLAGS = \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
resizetest_SOURCES = \
	resizetest.cc \
	$(top_srcdir)/hugin/hugin.c

tabwidgettest_LDADD = \
	$(top_builddir)/dggui/libdggui.la \
	$(top_builddir)/plugingui/libplugingui.la \
	$(top_builddir)/src/libdg.la \
	$(top_builddir)/src/libnls.la
tabwidgettest_CXXFLAGS = \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
tabwidgettest_SOURCES = \
	tabwidgettest.cc \
	$(top_srcdir)/hugin/hugin.c

filebrowsertest_LDADD = \
	$(top_builddir)/dggui/libdggui.la \
	$(top_builddir)/plugingui/libplugingui.la \
	$(top_builddir)/src/libdg.la \
	$(top_builddir)/src/libnls.la
filebrowsertest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
filebrowsertest_SOURCES = \
	filebrowsertest.cc \
	$(top_srcdir)/hugin/hugin.c

framewidgettest_LDADD = \
	$(top_builddir)/dggui/libdggui.la \
	$(top_builddir)/plugingui/libplugingui.la \
	$(top_builddir)/src/libdg.la \
	$(top_builddir)/src/libnls.la
framewidgettest_CXXFLAGS = \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
framewidgettest_SOURCES = \
	framewidgettest.cc \
	$(top_srcdir)/hugin/hugin.c

RES = \
	benchmarktest_resources/image_no_alpha.png \
	benchmarktest_resources/image_full_alpha.png \
	benchmarktest_resources/image_edge_alpha.png \
	benchmarktest_resources/image_inner_alpha.png

rcgen_verbose = $(rcgen_verbose_@AM_V@)
rcgen_verbose_ = $(rcgen_verbose_@AM_DEFAULT_V@)
rcgen_verbose_0 = @echo "  RCGEN    "$@;

benchmarktest_resource_data.cc: $(top_builddir)/dggui/rcgen $(RES)
	$(rcgen_verbose)$(top_builddir)/dggui/rcgen$(EXEEXT) -d $(top_srcdir)/test/uitests -o $@ $(RES)

benchmarktest_LDADD = \
	$(top_builddir)/dggui/libdggui.la \
	$(top_builddir)/src/libdg.la \
	$(top_builddir)/src/libnls.la
benchmarktest_CXXFLAGS = \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
nodist_benchmarktest_SOURCES = \
	benchmarktest_resource_data.cc
benchmarktest_SOURCES = \
	benchmarktest.cc \
	$(top_srcdir)/hugin/hugin.c

BUILT_SOURCES = benchmarktest_resource_data.cc
CLEANFILES = benchmarktest_resource_data.cc


powerwidgettest_LDADD = \
	$(top_builddir)/dggui/libdggui.la \
	$(top_builddir)/plugingui/libplugingui.la \
	$(top_builddir)/src/libdg.la \
	$(top_builddir)/src/libnls.la
powerwidgettest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
powerwidgettest_SOURCES = \
	powerwidgettest.cc \
	$(top_srcdir)/hugin/hugin.c

EXTRA_DIST = $(RES)
