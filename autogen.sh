#!/bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PROJECT=gexiv2

autoreconf -v --force --install -I config -I m4

if test x$NOCONFIGURE = x; then
  echo Running $srcdir/configure $conf_flags "$@" ...
  $srcdir/configure $conf_flags "$@" \
  && echo Now type \`make\' to compile $PROJECT || exit 1
else
  echo Skipping configure process.
fi

