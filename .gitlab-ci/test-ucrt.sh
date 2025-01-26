#!/usr/bin/bash -e

MW=mingw-w64-ucrt-x86_64-

pacman -S --needed base-devel \
    ${MW}toolchain \
    ${MW}gcc \
    ${MW}gobject-introspection \
    ${MW}gtk-doc \
    ${MW}meson \
    ${MW}ninja \
    ${MW}python-gobject \
    ${MW}vala

git clone https://github.com/exiv2/exiv2
mkdir exiv2-build
meson setup exiv2-build exiv2 --prefix "${PWD}"/exiv2-install
ninja -C exiv2-build install
PKG_CONFIG_PATH=${PWD}/exiv2-install/share/pkgconfig:${PKG_CONFIG_PATH} meson setup build --buildtype debug
ninja -C build
meson test -C build
