#!/bin/bash

# ---------------------------------------------------------------------------------------------------------------------
# stop on error

set -e

# ---------------------------------------------------------------------------------------------------------------------
# stop on error

TARGETDIR=$PWD/builds/drumgizmo

DOWNLOADS=$PWD/downloads

PKG_CONFIG_VERSION=0.28
LIBOGG_VERSION=1.3.3
LIBVORBIS_VERSION=1.3.6
FLAC_VERSION=1.3.2
LIBSNDFILE_VERSION=1.0.28
ZITA_RESAMPLER_VERSION=1.6.2

DRUMGIZMO_VERSION=0.9.20

# ---------------------------------------------------------------------------------------------------------------------
# function to remove old stuff

cleanup()
{

rm -rf $TARGETDIR
rm -rf flac-*
rm -rf libogg-*
rm -rf libsndfile-*
rm -rf libvorbis-*
rm -rf pkg-config-*
exit 0

}

# ---------------------------------------------------------------------------------------------------------------------
# setup build vars

export CC=clang
export CXX=clang++

export PREFIX=${TARGETDIR}
export PATH=${PREFIX}/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig

export CFLAGS="-O3 -mtune=generic -msse -msse2 -fvisibility=hidden"
export CFLAGS="${CFLAGS} -fPIC -DPIC -DNDEBUG -I${PREFIX}/include"
export CXXFLAGS="${CFLAGS} -fvisibility-inlines-hidden -stdlib=libc++"

export LDFLAGS="-Wl,-dead_strip -Wl,-dead_strip_dylibs"
export LDFLAGS="${LDFLAGS} -L${PREFIX}/lib"

# ---------------------------------------------------------------------------------------------------------------------
echo ==== Package pkgconfig ====

if [ ! -f ${DOWNLOADS}/pkg-config-${PKG_CONFIG_VERSION}.tar.gz ]; then
  pushd ${DOWNLOADS}
  curl -O https://pkg-config.freedesktop.org/releases/pkg-config-${PKG_CONFIG_VERSION}.tar.gz || \
  curl -k -O https://drumgizmo.org/mirror/pkg-config-${PKG_CONFIG_VERSION}.tar.gz
  popd
fi

if [ ! -d pkg-config-${PKG_CONFIG_VERSION} ]; then
  tar -xf ${DOWNLOADS}/pkg-config-${PKG_CONFIG_VERSION}.tar.gz
fi

if [ ! -f pkg-config-${PKG_CONFIG_VERSION}/build-done ]; then
  cd pkg-config-${PKG_CONFIG_VERSION}
  ./configure --enable-indirect-deps --with-internal-glib --with-pc-path=$PKG_CONFIG_PATH --prefix=${PREFIX}
  make ${MAKE_ARGS}
  make install
  touch build-done
  cd ..
fi

# ---------------------------------------------------------------------------------------------------------------------
echo ==== Package libogg ====

if [ ! -f ${DOWNLOADS}/libogg-${LIBOGG_VERSION}.tar.gz ]; then
  pushd ${DOWNLOADS}
  curl -O http://ftp.osuosl.org/pub/xiph/releases/ogg/libogg-${LIBOGG_VERSION}.tar.gz || \
  curl -k -O https://drumgizmo.org/mirror/libogg-${LIBOGG_VERSION}.tar.gz
  popd
fi

if [ ! -d libogg-${LIBOGG_VERSION} ]; then
  tar -xf ${DOWNLOADS}/libogg-${LIBOGG_VERSION}.tar.gz
fi

if [ ! -f libogg-${LIBOGG_VERSION}/build-done ]; then
  cd libogg-${LIBOGG_VERSION}
  ./configure --enable-static --disable-shared --prefix=${PREFIX}
  make ${MAKE_ARGS}
  make install
  touch build-done
  cd ..
fi

# ---------------------------------------------------------------------------------------------------------------------
echo ==== Package libvorbis ====

if [ ! -f ${DOWNLOADS}/libvorbis-${LIBVORBIS_VERSION}.tar.gz ]; then
  pushd ${DOWNLOADS}
  curl -O http://ftp.osuosl.org/pub/xiph/releases/vorbis/libvorbis-${LIBVORBIS_VERSION}.tar.gz || \
  curl -k -O https://drumgizmo.org/mirror/libvorbis-${LIBVORBIS_VERSION}.tar.gz
  popd
fi

if [ ! -d libvorbis-${LIBVORBIS_VERSION} ]; then
  tar -xf ${DOWNLOADS}/libvorbis-${LIBVORBIS_VERSION}.tar.gz
fi

if [ ! -f libvorbis-${LIBVORBIS_VERSION}/build-done ]; then
  cd libvorbis-${LIBVORBIS_VERSION}
  ./configure --enable-static --disable-shared --prefix=${PREFIX}
  make ${MAKE_ARGS}
  make install
  touch build-done
  cd ..
fi

# ---------------------------------------------------------------------------------------------------------------------
echo ==== Package flac ====

if [ ! -f ${DOWNLOADS}/flac-${FLAC_VERSION}.tar.xz ]; then
  pushd ${DOWNLOADS}
  curl -O http://ftp.osuosl.org/pub/xiph/releases/flac/flac-${FLAC_VERSION}.tar.xz || \
  curl -k -O https://drumgizmo.org/mirror/flac-${FLAC_VERSION}.tar.xz
  popd
fi
if [ ! -d flac-${FLAC_VERSION} ]; then
  tar -xf ${DOWNLOADS}/flac-${FLAC_VERSION}.tar.xz
fi

if [ ! -f flac-${FLAC_VERSION}/build-done ]; then
  cd flac-${FLAC_VERSION}
  sed -i -e "s/HAVE_CPUID_H/HAVE_CPUID_H_NOT/" src/libFLAC/cpu.c
  chmod +x configure install-sh
  ./configure --enable-static --disable-shared --prefix=${PREFIX} \
    --disable-cpplibs
  echo "exit 0" > missing
  make ${MAKE_ARGS}
  make install
  touch build-done
  cd ..
fi

# ---------------------------------------------------------------------------------------------------------------------
echo ==== Package libsndfile ====

if [ ! -f ${DOWNLOADS}/libsndfile-${LIBSNDFILE_VERSION}.tar.gz ]; then
  pushd ${DOWNLOADS}
  curl -O http://www.mega-nerd.com/libsndfile/files/libsndfile-${LIBSNDFILE_VERSION}.tar.gz || \
  curl -k -O https://drumgizmo.org/mirror/libsndfile-${LIBSNDFILE_VERSION}.tar.gz
  popd
fi

if [ ! -d libsndfile-${LIBSNDFILE_VERSION} ]; then
  tar -xf ${DOWNLOADS}/libsndfile-${LIBSNDFILE_VERSION}.tar.gz
fi

if [ ! -f libsndfile-${LIBSNDFILE_VERSION}/build-done ]; then
  cd libsndfile-${LIBSNDFILE_VERSION}
  ./configure --enable-static --disable-shared --prefix=${PREFIX} \
    --disable-full-suite --disable-sqlite
  make ${MAKE_ARGS}
  make install
  sed -i -e "s|-lsndfile|-lsndfile -lFLAC -lvorbisenc -lvorbis -logg -lm|" ${PREFIX}/lib/pkgconfig/sndfile.pc
  touch build-done
  cd ..
fi

# ---------------------------------------------------------------------------------------------------------------------
echo ==== Package zita-resampler ====

if [ ! -f ${DOWNLOADS}/zita-resampler-${ZITA_RESAMPLER_VERSION}.tar.bz2 ]; then
  pushd ${DOWNLOADS}
  curl -O http://kokkinizita.linuxaudio.org/linuxaudio/downloads/zita-resampler-${ZITA_RESAMPLER_VERSION}.tar.bz2 || \
  curl -k -O https://drumgizmo.org/mirror/zita-resampler-${ZITA_RESAMPLER_VERSION}.tar.bz2
  popd
fi

if [ ! -d zita-resampler-${ZITA_RESAMPLER_VERSION} ]; then
  tar -xf ${DOWNLOADS}/zita-resampler-${ZITA_RESAMPLER_VERSION}.tar.bz2
fi

if [ ! -f zita-resampler-${ZITA_RESAMPLER_VERSION}/build-done ]; then
  cd zita-resampler-${ZITA_RESAMPLER_VERSION}
  if [ ! -f patched ]; then
    patch -p1 -i ../zita-resampler-static-build.patch
    touch patched
  fi
  make ${MAKE_ARGS} -C source
  mkdir ${PREFIX}/include/zita-resampler
  cp source/*.a ${PREFIX}/lib/
  cp source/zita-resampler/*.h ${PREFIX}/include/zita-resampler/
  touch build-done
  cd ..
fi

# ---------------------------------------------------------------------------------------------------------------------
echo ==== Package drumgizmo ====

if [ ! -f ${DOWNLOADS}/drumgizmo-${DRUMGIZMO_VERSION}.tar.gz ]; then
  pushd ${DOWNLOADS}
  curl -O https://www.drumgizmo.org/releases/drumgizmo-${DRUMGIZMO_VERSION}/drumgizmo-${DRUMGIZMO_VERSION}.tar.gz
  popd
fi

if [ ! -d drumgizmo-${DRUMGIZMO_VERSION} ]; then
  tar -xf ${DOWNLOADS}/drumgizmo-${DRUMGIZMO_VERSION}.tar.gz
fi

if [ ! -f drumgizmo-${DRUMGIZMO_VERSION}/build-done ]; then
  PATH=/usr/local/opt/gettext/bin:$PATH
  cd drumgizmo-${DRUMGIZMO_VERSION}
  ./configure --prefix=${PREFIX} \
    --disable-input-midifile \
    --disable-input-jackmidi \
    --disable-output-jackaudio \
    --disable-input-alsamidi \
    --disable-output-alsa \
    --enable-cli \
    --without-debug \
    --with-test \
    --disable-lv2 \
    --enable-vst \
    --with-vst-sources=$HOME/VST3_SDK
  make ${MAKE_ARGS}
  make install
  touch build-done
  cd ..
fi

# ---------------------------------------------------------------------------------------------------------------------

echo Make macOS package:

otool -L ${PREFIX}/lib/vst/drumgizmo_vst.so

if [ ! -f drumgizmo.vst.tar.gz ]; then
  exit "Missing VST package template: drumgizmo.vst.tar.gz "
fi

if [ ! -d ${DRUMGIZMO_VERSION}/drumgizmo.vst ]; then
  mkdir -p ${DRUMGIZMO_VERSION}
  pushd ${DRUMGIZMO_VERSION}
  tar -xvzf ../drumgizmo.vst.tar.gz
  popd
fi

cp -a ${PREFIX}/lib/vst/drumgizmo_vst.so ${DRUMGIZMO_VERSION}/drumgizmo.vst/Contents/MacOS/drumgizmo
sed -e "s/@VERSION@/${DRUMGIZMO_VERSION}/g" -i .bak ${DRUMGIZMO_VERSION}/drumgizmo.vst/Contents/Info.plist
rm -f ${DRUMGIZMO_VERSION}/drumgizmo.vst/Contents/Info.plist.bak

pushd ${DRUMGIZMO_VERSION}
tar cvzf ../drumgizmo_vst-osx-${DRUMGIZMO_VERSION}.tar.gz drumgizmo.vst/
popd

echo Package is now ready in drumgizmo_vst-osx-${DRUMGIZMO_VERSION}.tar.gz
