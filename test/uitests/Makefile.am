noinst_PROGRAMS = resizetest tabwidgettest framewidgettest \
	filebrowsertest benchmarktest

resizetest_LDADD = $(top_srcdir)/plugingui/libdggui.la $(top_srcdir)/src/libdg.la
resizetest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
resizetest_SOURCES = \
	resizetest.cc \
	$(top_srcdir)/hugin/hugin.c

tabwidgettest_LDADD = $(top_srcdir)/plugingui/libdggui.la $(top_srcdir)/src/libdg.la
tabwidgettest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
tabwidgettest_SOURCES = \
	tabwidgettest.cc \
	$(top_srcdir)/hugin/hugin.c

filebrowsertest_LDADD = $(top_srcdir)/plugingui/libdggui.la $(top_srcdir)/src/libdg.la
filebrowsertest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
filebrowsertest_SOURCES = \
	filebrowsertest.cc \
	$(top_srcdir)/hugin/hugin.c

framewidgettest_LDADD = $(top_srcdir)/plugingui/libdggui.la $(top_srcdir)/src/libdg.la
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

benchmarktest_resource_data.cc : $(top_srcdir)/plugingui/rcgen $(RES)
	$(top_srcdir)/plugingui/rcgen $(RES) > benchmarktest_resource_data.cc

benchmarktest_LDADD = \
	$(top_srcdir)/plugingui/libdggui.la $(top_srcdir)/src/libdg.la
benchmarktest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
benchmarktest_SOURCES = \
	benchmarktest.cc \
	benchmarktest_resource_data.cc \
	$(top_srcdir)/hugin/hugin.c
