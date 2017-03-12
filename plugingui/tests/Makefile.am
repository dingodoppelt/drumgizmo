noinst_PROGRAMS = resizetest tabwidgettest dgwindowtest

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

dgwindowtest_LDADD = \
	$(top_srcdir)/plugingui/libdggui.la \
	$(top_srcdir)/src/libdg.la
dgwindowtest_CXXFLAGS = \
	-I$(top_srcdir)/plugingui \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/hugin
dgwindowtest_SOURCES = \
	dgwindowtest.cc \
	$(top_srcdir)/hugin/hugin.c
