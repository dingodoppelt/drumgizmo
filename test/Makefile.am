# Rules for the test code (use `make check` to execute)
include $(top_srcdir)/src/Makefile.am.drumgizmo

TESTS = engine gui

check_PROGRAMS = $(TESTS)

engine_CXXFLAGS = -DOUTPUT=\"engine\" $(CPPUNIT_CFLAGS) \
	-I$(top_srcdir)/src -I$(top_srcdir)/include \
	-I$(top_srcdir)/hugin -DDISABLE_HUGIN

engine_CFLAGS = -DDISABLE_HUGIN

engine_LDFLAGS = $(CPPUNIT_LIBS) $(DRUMGIZMO_LIBS) $(PTHREAD_LIBS)

engine_SOURCES = \
	$(DRUMGIZMO_SOURCES) \
	$(top_srcdir)/hugin/hugin.c \
	test.cc \
	engine.cc

gui_CXXFLAGS = -DOUTPUT=\"gui\" $(CPPUNIT_CFLAGS)
gui_LDFLAGS = $(CPPUNIT_LIBS)
gui_SOURCES = \
	test.cc \
	gui.cc