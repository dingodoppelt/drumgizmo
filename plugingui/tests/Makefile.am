noinst_PROGRAMS = resizetest tabwidgettest

resizetest_LDADD = $(top_srcdir)/plugingui/libdggui.la
resizetest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
resizetest_SOURCES = \
	resizetest.cc \
	$(top_srcdir)/hugin/hugin.c

tabwidgettest_LDADD = $(top_srcdir)/plugingui/libdggui.la
tabwidgettest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
tabwidgettest_SOURCES = \
	tabwidgettest.cc \
	$(top_srcdir)/hugin/hugin.c
