noinst_PROGRAMS = resizetest tabwidgettest framewidgettest \
	filebrowsertest benchmarktest powerwidgettest

resizetest_LDADD = \
	$(top_builddir)/plugingui/libdggui.la \
	$(top_builddir)/src/libdg.la
resizetest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
resizetest_SOURCES = \
	resizetest.cc \
	$(top_srcdir)/hugin/hugin.c

tabwidgettest_LDADD = \
	$(top_builddir)/plugingui/libdggui.la \
	$(top_builddir)/src/libdg.la
tabwidgettest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
tabwidgettest_SOURCES = \
	tabwidgettest.cc \
	$(top_srcdir)/hugin/hugin.c

filebrowsertest_LDADD = \
	$(top_builddir)/plugingui/libdggui.la \
	$(top_builddir)/src/libdg.la
filebrowsertest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
filebrowsertest_SOURCES = \
	filebrowsertest.cc \
	$(top_srcdir)/hugin/hugin.c

framewidgettest_LDADD = \
	$(top_builddir)/plugingui/libdggui.la \
	$(top_builddir)/src/libdg.la
framewidgettest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
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

benchmarktest_resource_data.cc: $(top_builddir)/plugingui/rcgen $(RES)
	$(rcgen_verbose)$(top_builddir)/plugingui/rcgen -d $(top_srcdir)/test/uitests $(RES) > $(top_srcdir)/test/uitests/benchmarktest_resource_data.cc

benchmarktest_LDADD = \
	$(top_builddir)/plugingui/libdggui.la \
	$(top_builddir)/src/libdg.la
benchmarktest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
benchmarktest_SOURCES = \
	benchmarktest.cc \
	benchmarktest_resource_data.cc \
	$(top_srcdir)/hugin/hugin.c

powerwidgettest_LDADD = \
	$(top_builddir)/plugingui/libdggui.la \
	$(top_builddir)/src/libdg.la
powerwidgettest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
powerwidgettest_SOURCES = \
	powerwidgettest.cc \
	$(top_srcdir)/hugin/hugin.c

EXTRA_DIST = $(RES)
