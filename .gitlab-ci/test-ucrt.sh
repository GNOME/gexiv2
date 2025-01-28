#!/usr/bin/bash -e

pacman --noconfirm -Suy

pacman --noconfirm -S --needed base-devel \
    ${MINGW_PACKAGE_PREFIX}-toolchain \
    ${MINGW_PACKAGE_PREFIX}-gcc \
    ${MINGW_PACKAGE_PREFIX}-gobject-introspection \
    ${MINGW_PACKAGE_PREFIX}-gtk-doc \
    ${MINGW_PACKAGE_PREFIX}-meson \
    ${MINGW_PACKAGE_PREFIX}-ninja \
    ${MINGW_PACKAGE_PREFIX}-python-gobject \
    ${MINGW_PACKAGE_PREFIX}-vala \
    ${MINGW_PACKAGE_PREFIX}-cmake \
    ${MINGW_PACKAGE_PREFIX}-pkg-config

git clone https://github.com/exiv2/exiv2
mkdir exiv2-build

ls -l /ucrt64/bin/cmake 

/ucrt64/bin/cmake --help

/ucrt64/bin/cmake -B exiv2-build -S exiv2 -GNinja \
    -DCMAKE_INSTALL_PREFIX="${PWD}"/exiv2-install \
    -DCMAKE_BUILD_TYPE=Debug \
    -DEXIV2_BUILD_EXIV2_COMMAND=OFF \
    -DEXIV2_BUILD_SAMPLES=OFF \
    -DEXIV2_BUILD_UNIT_TESTS=OFF \
    -DEXIV2_ENABLE_BMFF=ON \
    -DEXIV2_ENABLE_INIH=OFF

ninja -C exiv2-build install
PKG_CONFIG_PATH=${PWD}/exiv2-install/share/pkgconfig:${PKG_CONFIG_PATH} meson setup build --buildtype debug
ninja -C build
meson test -C build
