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
    ${MINGW_PACKAGE_PREFIX}-exiv2 \
    ${MINGW_PACKAGE_PREFIX}-pkg-config

meson setup build --buildtype debug -Dtests=true
ninja -C build
meson test -C build
