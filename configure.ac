# Filename: configure.in

AC_INIT([drumgizmo], m4_esyscmd([cat version.h | cut -d'"' -f2 | xargs echo -n]))dnl"

AC_CONFIG_SRCDIR([src/drumgizmo.cc])
AM_INIT_AUTOMAKE

AC_PROG_CXX
AM_PROG_CC_C_O

LT_INIT

AC_CONFIG_HEADERS(config.h)

need_jack=no

dnl ======================
dnl Init pkg-config
dnl ======================
PKG_PROG_PKG_CONFIG(0.23)

HUGIN_PARM="-DDISABLE_HUGIN"
AC_ARG_WITH(debug,  [  --with-debug            Build with debug support])
if test x$with_debug == xyes; then
    AC_MSG_WARN([*** Building with debug support!])
    CXXFLAGS="$CXXFLAGS -Wall -Werror -g"
    HUGIN_PARM=""
fi 
CXXFLAGS="$CXXFLAGS $HUGIN_PARM"

dnl ===========================
dnl Check for GUI backend
dnl ===========================
AC_CANONICAL_HOST
AC_ARG_ENABLE([gui],
[  --enable-gui[=backend]   Use specified gui backend. Can be win32, x11, pugl or auto [default=auto]],,
        [enable_gui="auto"])

if test "x$enable_gui" = "xyes"; then
  enable_gui="auto"
fi

if test "x$enable_gui" = "xauto"; then
  AC_MSG_RESULT([Auto setting gui based on host: $host_os])
  case $host_os in
    darwin* )
					enable_gui=""					
          ;;
    linux*|*BSD*)
					enable_gui="x11"
          ;;
    mingw*|windows*|winnt|cygwin)
					enable_gui="win32"
          ;;
		*)
          AC_MSG_ERROR([Your platform is not currently supported])
          ;;
  esac
fi

if test "x$enable_gui" = "xx11"; then
  AC_MSG_RESULT([Setting gui backend to X11])

	dnl ======================
  dnl Check for Xlib
  dnl ======================
  PKG_CHECK_MODULES(X11, x11 >= 1.0)

  GUI_CFLAGS="-DX11 $X11_CFLAGS"
  GUI_LIBS="$X11_LIBS"

elif test "x$enable_gui" = "xwin32"; then

  AC_MSG_RESULT([Setting gui backend to Win32])
  GUI_CFLAGS="-DWIN32"
  GUI_LIBS="-lgdi32 -lsetupapi -lws2_32"

elif test "x$enable_gui" = "xpugl"; then

  AC_MSG_RESULT([Setting gui backend to Pugl])
  GUI_CFLAGS="-DPUGL -I../../pugl"
  GUI_LIBS="-lGLU -lGL -lglut"

else 
  AC_MSG_ERROR([*** No GUI backend has been selected ***])
fi

AC_SUBST(GUI_CFLAGS)
AC_SUBST(GUI_LIBS)

AM_CONDITIONAL([ENABLE_PUGL], [test "x$enable_gui" = "xpugl"])

dnl ======================
dnl Compile unit tests
dnl ======================
AC_ARG_WITH(test,  [  --with-test             Build unit tests])
if test x$with_test == xyes; then
    AC_MSG_WARN([*** Building unittests!])
		AM_PATH_CPPUNIT(1.9.6)
		AC_CONFIG_FILES(test/Makefile)
fi 

dnl ======================
dnl Compile LV2 plugin
dnl ======================
AC_ARG_ENABLE([lv2],
[  --enable-lv2   Compile the LV2 plugin [default=no]],,
        [enable_lv2="no"])
if test "x$enable_lv2" = "xyes"; then
	 enable_lv2=yes 
   dnl ======================
   dnl Check for lv2core
   dnl ======================
   PKG_CHECK_MODULES(LV2, lv2 >= 1.0)
else
	 AC_MSG_RESULT([*** LV2 plugin will not be compiled ***])
   enable_lv2=no
fi
AM_CONDITIONAL([ENABLE_LV2], [test "x$enable_lv2" = "xyes"])

dnl ======================
dnl Compile VST plugin
dnl ======================
AC_ARG_WITH(vst_sources,  [  --with-vst-sources         Point this to the vstsdk24 directory])
AC_ARG_ENABLE([vst],
[  --enable-vst   Compile the VST plugin [default=no]],,
        [enable_vst="no"])
if test "x$enable_vst" = "xyes"; then
	 enable_vst=yes
	 VST_CPPFLAGS="-I$with_vst_sources -D__int64='long long int'"
	 VST_SOURCE_PATH="$with_vst_sources"
	 AC_SUBST(VST_SOURCE_PATH)
	 AC_SUBST(VST_CPPFLAGS)
   AC_CONFIG_FILES(vst/Makefile.mingw32)
else
	 AC_MSG_RESULT([*** VST plugin will not be compiled ***])
   enable_vst=no
fi
AM_CONDITIONAL([ENABLE_VST], [test "x$enable_vst" = "xyes"])


dnl ======================
dnl Compile AU plugin
dnl ======================
#AC_ARG_ENABLE([au],
#[  --enable-au   Compile the audio units plugin [default=no]],,
#        [enable_au="no"])
#if test "x$enable_au" = "xyes"; then
#	 enable_au=yes 
#else
#	 AC_MSG_RESULT([*** Audio Units plugin will not be compiled ***])
#   enable_au=no
#fi
#AM_CONDITIONAL([ENABLE_AU], [test "x$enable_au" = "xyes"])

dnl ======================
dnl Compile cli client
dnl ======================
AC_ARG_ENABLE([cli],
[  --enable-cli   Compile the command line interface [default=yes]],,
        [enable_cli="yes"])
if test "x$enable_cli" = "xyes"; then
	 enable_cli=yes 

   dnl ======================
   dnl Check for getopt
   dnl ======================
   AC_CHECK_HEADERS(getopt.h)
   
   dnl ***
   dnl *** Input plugins
   dnl ***
   
   dnl *** Dummy
   AC_ARG_ENABLE([input_dummy],
   [  --disable-input-dummy       Disable input dummy plugin [default=enabled]],,
           [enable_input_dummy="yes"])
   if test "x$enable_input_dummy" = "xyes"; then
   	 have_input_dummy=yes 
   else
   	 AC_MSG_RESULT([*** input dummy plugin disabled per user request ***])
      have_input_dummy=no
   fi
   
   dnl *** Test
   AC_ARG_ENABLE([input_test],
   [  --disable-input-test        Disable input test plugin [default=enabled]],,
           [enable_input_test="yes"])
   if test "x$enable_input_test" = "xyes"; then
   	 have_input_test=yes 
   else
   	 AC_MSG_RESULT([*** input test plugin disabled per user request ***])
      have_input_test=no
   fi
   
   dnl *** Jackmidi
   AC_ARG_ENABLE([input_jackmidi],
   [  --disable-input-jackmidi    Disable input jackmidi plugin [default=enabled]],,
           [enable_input_jackmidi="yes"])
   if test "x$enable_input_jackmidi" = "xyes"; then
   	 have_input_jackmidi=yes 
		 need_jack=yes
   else
   	 AC_MSG_RESULT([*** input jackmidi plugin disabled per user request ***])
      have_input_jackmidi=no
   fi
   
   dnl *** Midifile
   AC_ARG_ENABLE([input_midifile],
   [  --disable-input-midifile    Disable input midifile plugin [default=enabled]],,
           [enable_input_midifile="yes"])
   if test "x$enable_input_midifile" = "xyes"; then
   	 have_input_midifile=yes
   	 dnl ======================
   	 dnl Check for libsmf
   	 dnl ======================
   	 PKG_CHECK_MODULES(SMF, smf >= 1.2)
   else
   	 AC_MSG_RESULT([*** input midifile plugin disabled per user request ***])
      have_input_midifile=no
   fi   
   
   INPUT_PLUGINS="midifile jackmidi dummy test"
   AC_SUBST(INPUT_PLUGINS)
   
   
   dnl ***
   dnl *** Output plugins
   dnl ***
   
   dnl *** dummy
   AC_ARG_ENABLE([output_dummy],
   [  --disable-output-dummy      Disable output dummy plugin [default=enabled]],,
           [enable_output_dummy="yes"])
   if test "x$enable_output_dummy" = "xyes"; then
   	 have_output_dummy=yes 
   else
   	 AC_MSG_RESULT([*** output dummy plugin disabled per user request ***])
      have_output_dummy=no
   fi
   
   dnl *** Jackaudio
   AC_ARG_ENABLE([output_jackaudio],
   [  --disable-output-jackaudio  Disable output jack plugin [default=enabled]],,
           [enable_output_jackaudio="yes"])
   if test "x$enable_output_jackaudio" = "xyes"; then
   	 have_output_jackaudio=yes 
		 need_jack=yes
   else
   	 AC_MSG_RESULT([*** output jack plugin disabled per user request ***])
      have_output_jackaudio=no
   fi

   dnl *** alsa
   AC_ARG_ENABLE([output_alsa],
   [  --disable-output-alsa       Disable output alsa plugin [default=enabled]],,
           [enable_output_alsa="yes"])
   if test "x$enable_output_alsa" = "xyes"; then
   	 have_output_alsa=yes 
   	 dnl ======================
   	 dnl Check for alsa library
   	 dnl ======================
   	 PKG_CHECK_MODULES(ALSA, alsa >= 1.0.18)
   else
   	 AC_MSG_RESULT([*** output alsa plugin disabled per user request ***])
      have_output_alsa=no
   fi
   
   dnl *** wavfile
   AC_ARG_ENABLE([output_wavfile],
   [  --disable-output-wavfile    Disable output wavfile plugin [default=enabled]],,
           [enable_output_wavfile="yes"])
   if test "x$enable_output_wavfile" = "xyes"; then
   	 have_output_wavfile=yes 
   else
   	 AC_MSG_RESULT([*** output wavfile plugin disabled per user request ***])
      have_output_wavfile=no
   fi
   
   OUTPUT_PLUGINS="dummy alsa wavfile jackaudio"
   AC_SUBST(OUTPUT_PLUGINS)
   
   dnl
   dnl Setup plugin paths
   dnl
   dgplugindir=${libdir}/drumgizmo
   AC_SUBST([dgplugindir])
   
   INPUT_PLUGIN_DIR=${dgplugindir}/input
   OUTPUT_PLUGIN_DIR=${dgplugindir}/output
   AC_SUBST([INPUT_PLUGIN_DIR])
   AC_SUBST([OUTPUT_PLUGIN_DIR])
   
   AC_DEFINE_UNQUOTED(INPUT_PLUGIN_DIR, "${prefix}/lib/drumgizmo/input",
     [Input plugin dir])
   AC_DEFINE_UNQUOTED(OUTPUT_PLUGIN_DIR, "${prefix}/lib/drumgizmo/output",
     [Output plugin dir])
else
	 AC_MSG_RESULT([*** Command line interface disabled per user request ***])
   enable_cli=no
fi
AM_CONDITIONAL([ENABLE_CLI], [test "x$enable_cli" == "xyes"])
AM_CONDITIONAL([HAVE_INPUT_DUMMY], [test "x$have_input_dummy" = "xyes"])
AM_CONDITIONAL([HAVE_INPUT_TEST], [test "x$have_input_test" = "xyes"])
AM_CONDITIONAL([HAVE_INPUT_JACKMIDI], [test "x$have_input_jackmidi" = "xyes"])
AM_CONDITIONAL([HAVE_INPUT_MIDIFILE], [test "x$have_input_midifile" = "xyes"])
AM_CONDITIONAL([HAVE_OUTPUT_DUMMY], [test "x$have_output_dummy" = "xyes"])
AM_CONDITIONAL([HAVE_OUTPUT_ALSA], [test "x$have_output_alsa" = "xyes"])
AM_CONDITIONAL([HAVE_OUTPUT_WAVFILE], [test "x$have_output_wavfile" = "xyes"])
AM_CONDITIONAL([HAVE_OUTPUT_JACKAUDIO], [test "x$have_output_jackaudio" = "xyes"])


dnl ======================
dnl Check for sndfile
dnl ======================
PKG_CHECK_MODULES(SNDFILE, sndfile >= 1.0.20)


AC_ARG_ENABLE([resampler],
[  --enable-resampler[=lib]   Enable resampler using either 'zita' or 'src' (libsamplerate). Use 'auto' for autodetect [default=auto]],,
        [enable_resampler="auto"])
if test "x$enable_resampler" = "xyes"; then
  enable_resampler="auto"
fi

has_src=no
has_zita=no
if test x$enable_resampler != xno; then
   AC_MSG_WARN([*** Building resample support!])
   AC_DEFINE(WITH_RESAMPLER, [], [Use resample])

  if test x$enable_resampler == xauto || test x$enable_resampler == xsrc; then
    dnl ======================
    dnl Check for libsamplerate
    dnl ======================
    PKG_CHECK_MODULES(SAMPLERATE, samplerate >= 0.1.7, has_src=yes, has_src=no)
  fi

  if test x$enable_resampler == xauto || test x$enable_resampler == xzita; then
    dnl ======================
    dnl Check for the zitaresampler library
    dnl ======================
    AC_LANG_PUSH([C++])
    tmp_CXXFLAGS="$CXXFLAGS"
    tmp_CPPFLAGS="$CPPFLAGS"
    tmp_CFLAGS="$CFLAGS"
    tmp_LDFLAGS="$LDFLAGS"
    tmp_LIBS="$LIBS"
    CXXFLAGS=""
    CPPFLAGS="$ZITA_CPPFLAGS"
    CFLAGS=""
    LDFLAGS="$ZITA_LDFLAGS"
    LIBS="-lzita-resampler -lpthread"
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([#include <zita-resampler/resampler.h>],
        [Resampler r] ) ], has_zita=yes, has_zita=no)
    ZITA_CPPFLAGS="$CXXFLAGS $CPPFLAGS $CFLAGS"
    ZITA_LIBS="$LDFLAGS $LIBS"
    CXXFLAGS="$tmp_CXXFLAGS"
    CPPFLAGS="$tmp_CPPFLAGS"
    CFLAGS="$tmp_CFLAGS"
    LDFLAGS="$tmp_LDFLAGS"
    LIBS="$tmp_LIBS"
    AC_SUBST(ZITA_CPPFLAGS)
    AC_SUBST(ZITA_LIBS)
    AC_LANG_POP([C++])
  fi
fi

if test x$enable_resampler == xauto; then
   if test x$has_zita == xyes; then
	   enable_resampler=zita
   elif test x$has_src == xyes; then
	   enable_resampler=src
   else
	   AC_MSG_ERROR([*** No resampler library present. Either libsamplerate or zita-resampler must be installed.])
   fi
fi

if test x$enable_resampler == xzita; then
	 if test x$has_zita == xyes; then
	   AC_DEFINE(USE_ZITA, [], [zita-resampler is present])
   else
     AC_MSG_ERROR([*** zita-resampler library or headers not found. Set ZITA_LDFLAGS or ZITA_CPPFLAGS to add searchpath.])
   fi
fi

if test x$enable_resampler == xsrc; then
   if test x$has_src == xyes; then
	   AC_DEFINE(USE_SRC, [], [libsamplerate is present])
   else
	   AC_MSG_ERROR([*** Missing libsamplerate.])
   fi
fi

#dnl ======================
#dnl Check for zlib
#dnl ======================
PKG_CHECK_MODULES(ZLIB, zlib >= 1.2.3)

dnl ======================
dnl Check for the pthread library
dnl ======================
tmp_CXXFLAGS="$CXXFLAGS"
tmp_CPPFLAGS="$CPPFLAGS"
tmp_CFLAGS="$CFLAGS"
tmp_LDFLAGS="$LDFLAGS"
tmp_LIBS="$LIBS"
CXXFLAGS=""
CPPFLAGS=""
CFLAGS=""
LDFLAGS=""
LIBS=""
AC_CHECK_HEADER(pthread.h, , AC_MSG_ERROR([*** pthread header file not found!]))
AC_CHECK_LIB(pthread,  pthread_mutex_init, , AC_MSG_ERROR([*** pthread library not found!]))
PTHREAD_CFLAGS="$CXXFLAGS $CPPFLAGS $CFLAGS"
PTHREAD_LIBS="$LDFLAGS $LIBS"
CXXFLAGS="$tmp_CXXFLAGS"
CPPFLAGS="$tmp_CPPFLAGS"
CFLAGS="$tmp_CFLAGS"
LDFLAGS="$tmp_LDFLAGS"
LIBS="$tmp_LIBS"
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_LIBS)

dnl ======================
dnl Check for eXpat library
dnl ======================
tmp_CXXFLAGS="$CXXFLAGS"
tmp_CPPFLAGS="$CPPFLAGS"
tmp_CFLAGS="$CFLAGS"
tmp_LDFLAGS="$LDFLAGS"
tmp_LIBS="$LIBS"
CXXFLAGS=""
CPPFLAGS=""
CFLAGS="$EXPAT_CFLAGS"
LDFLAGS="$EXPAT_LDFLAGS"
LIBS=""
AC_CHECK_HEADER(expat.h, , AC_MSG_ERROR([*** eXpat header file not found!]))
AC_CHECK_LIB(expat, XML_ParserCreate, , AC_MSG_ERROR([*** eXpat library not found!]))
EXPAT_CFLAGS="$CXXFLAGS $CPPFLAGS $CFLAGS"
EXPAT_LIBS="$LDFLAGS $LIBS"
CXXFLAGS="$tmp_CXXFLAGS"
CPPFLAGS="$tmp_CPPFLAGS"
CFLAGS="$tmp_CFLAGS"
LDFLAGS="$tmp_LDFLAGS"
LIBS="$tmp_LIBS"
AC_SUBST(EXPAT_CFLAGS)
AC_SUBST(EXPAT_LIBS)

dnl ===========================
dnl Check for SSE instructions
dnl ===========================
AC_ARG_ENABLE([sse],
[  --enable-sse[=level]   Enable SSE Level 1, 2, 3 or auto [default=auto]],,
        [enable_sse="auto"])
if test "x$enable_sse" = "xyes"; then
  enable_sse="auto"
fi
#AC_MSG_RESULT([SSE: $enable_sse])
if test "x$enable_sse" = "x3"; then
  AC_MSG_RESULT([Setting sse level 3])
  SSEFLAGS="-DSSE -msse -msse2 -mss3"
elif test "x$enable_sse" = "x2"; then
  AC_MSG_RESULT([Setting sse level 2])
  SSEFLAGS="-DSSE -msse -msse2"
elif test "x$enable_sse" = "x1"; then
  AC_MSG_RESULT([Setting sse level 1])
  SSEFLAGS="-DSSE -msse"
elif test "x$enable_sse" = "xauto"; then
  AC_MSG_RESULT([Auto setting sse level])
  AC_MSG_CHECKING([for sse3 instructions])
  if grep -q "sse3" /proc/cpuinfo
  then
    AC_MSG_RESULT([yes])
    SSEFLAGS="-msse3"
    AC_SUBST(SSEFLAGS)
  else
    AC_MSG_RESULT([no])
  fi
  AC_MSG_CHECKING([for sse2 instructions])
  if grep -q "sse2" /proc/cpuinfo
  then
    AC_MSG_RESULT([yes])
    SSEFLAGS="$SSEFLAGS -msse2"
    AC_SUBST(SSEFLAGS)
  else
    AC_MSG_RESULT([no])
  fi
  AC_MSG_CHECKING([for sse instructions])
  if grep -q "sse" /proc/cpuinfo
  then
    AC_MSG_RESULT([yes])
    SSEFLAGS="$SSEFLAGS -DSSE -msse"
    AC_SUBST(SSEFLAGS)
  else
    AC_MSG_RESULT([no])
  fi
else 
  AC_MSG_RESULT([*** Disabling SSE ***])
fi

if test "x$need_jack" = "xyes"
then
  dnl ======================
  dnl Check for jack
  dnl ======================
  PKG_CHECK_MODULES(JACK, jack >= 0.120.1)
fi

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(LDFLAGS)



#	au/Makefile

AC_CONFIG_FILES(
	Makefile
	tools/Makefile
	src/Makefile
	vst/Makefile
	lv2/Makefile
	plugingui/Makefile
	include/Makefile
	drumgizmo/Makefile
	drumgizmo/input/Makefile
	drumgizmo/input/dummy/Makefile
	drumgizmo/input/test/Makefile
	drumgizmo/input/jackmidi/Makefile
	drumgizmo/input/midifile/Makefile
	drumgizmo/output/Makefile
	drumgizmo/output/dummy/Makefile
	drumgizmo/output/alsa/Makefile
	drumgizmo/output/jackaudio/Makefile
	drumgizmo/output/wavfile/Makefile)

AC_OUTPUT()
