VST_BASE = vstsdk2.4
VST_SRC_BASE = ${VST_BASE}/public.sdk/source/vst2.x/
VST_SRC = \
	${VST_SRC_BASE}/audioeffectx.cpp \
	${VST_SRC_BASE}/audioeffect.cpp \
	${VST_SRC_BASE}/vstplugmain.cpp

DG_BASE = ../
DG_SRC = \
	${DG_BASE}/src/audiofile.cc \
	${DG_BASE}/src/channel.cc \
	${DG_BASE}/src/channelmixer.cc \
	${DG_BASE}/src/drumgizmo.cc \
	${DG_BASE}/src/drumkit.cc \
	${DG_BASE}/src/drumkitparser.cc \
	${DG_BASE}/src/events.cc \
	${DG_BASE}/src/instrument.cc \
	${DG_BASE}/src/instrumentparser.cc \
	${DG_BASE}/src/midimapparser.cc \
	${DG_BASE}/src/midimapper.cc \
	${DG_BASE}/src/mutex.cc \
	${DG_BASE}/src/path.cc \
	${DG_BASE}/src/sample.cc \
	${DG_BASE}/src/saxparser.cc \
	${DG_BASE}/src/velocity.cc
DG_FLAGS = -I../include -I../src

EXPAT_BASE = expat
EXPAT_FLAGS = -I${EXPAT_BASE}/Source/lib -L${EXPAT_BASE}/Bin -lexpat

SNDFILE_BASE = libsndfile
SNDFILE_FLAGS = -I${SNDFILE_BASE}\include -L${SNDFILE_BASE}/bin -lsndfile-1 -static

SRC = \
	drumgizmo_vst.cc \
	input_vst.cc \
	output_vst.cc

all:
	g++ -g -Wall -DWIN32 ${DG_FLAGS} ${EXPAT_FLAGS} ${SNDFILE_FLAGS} -I${VST_BASE} ${DG_SRC} ${VST_SRC} ${SRC} -shared -o drumgizmo_vst.dll -Wl,--out-implib,libdrumgizmo_vst.a

clean:
	rm -f drumgizmo_vst.dll libdrumgizmo_vst.a