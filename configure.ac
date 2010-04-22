#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(gexiv2, 0.0.1, mike@gemuende.de)
AC_CONFIG_SRCDIR([gexiv2/gexiv2-metadata-iptc.cpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES(EXIV2, exiv2 >= 0.18)
PKG_CHECK_MODULES(GLIB, glib-2.0)
PKG_CHECK_MODULES(GOBJECT, gobject-2.0)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 gexiv2/Makefile
                 gexiv2/gexiv2.pc])
AC_OUTPUT
