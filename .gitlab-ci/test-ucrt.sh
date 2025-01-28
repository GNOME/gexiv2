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
    ${MINGW_PACKAGE_PREFIX}-pkg-config \

git clone https://github.com/exiv2/exiv2
mkdir exiv2-build
meson setup exiv2-build exiv2 --prefix "${PWD}"/exiv2-install
ninja -C exiv2-build install
PKG_CONFIG_PATH=${PWD}/exiv2-install/share/pkgconfig:${PKG_CONFIG_PATH} meson setup build --buildtype debug
ninja -C build
meson test -C build
