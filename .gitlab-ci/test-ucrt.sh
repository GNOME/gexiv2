#!/usr/bin/bash -e

MW=mingw-w64-ucrt-x86_64-

pacman -S --needed base-devel \
    ${MW}toolchain \
    ${MW}gcc \
    ${MW}g++ \
    ${MW}gobject-introspection \
    ${MW}gtk-doc \
    ${MW}meson \
    ${MW}ninja \
    ${MW}python-gobject \
    ${MW}vala \
    ${MW}cmake \
    ${MW}pkg-config \

git clone https://github.com/exiv2/exiv2
mkdir exiv2-build
/usr/bin/meson setup exiv2-build exiv2 --prefix "${PWD}"/exiv2-install
/usr/bin/ninja -C exiv2-build install
PKG_CONFIG_PATH=${PWD}/exiv2-install/share/pkgconfig:${PKG_CONFIG_PATH} /usr/bin/meson setup build --buildtype debug
/usr/bin/ninja -C build
/usr/bin/meson test -C build
