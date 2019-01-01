AC_INIT([gexiv2],
        [0.10.10],
        [https://gitlab.gnome.org/GNOME/gexiv2/issues/new],
        [gexiv2],
        [https://wiki.gnome.org/Projects/gexiv2])
AC_PREREQ([2.69])

AC_CONFIG_MACRO_DIR([m4])

# Will install auxiliary build tools (depcomp, install-sh, missing...) under build-aux/
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 -Wall no-define foreign dist-xz no-dist-gzip tar-ustar
                  subdir-objects -Wno-portability])

AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

AM_PROG_AR
LT_PREREQ([2.2])
LT_INIT([disable-static win32-dll])

# This small tricks allows to provide different default compilation flags
# than autoconf, but still let the user overrides our defaults.
CXXFLAGS_COPY="$CXXFLAGS"
AC_PROG_CXX
CXXFLAGS="$CXXFLAGS_COPY"

PKG_PROG_PKG_CONFIG(0.26)

m4_define([gexiv2_major_version], [0])
m4_define([gexiv2_minor_version], [10])
m4_define([gexiv2_micro_version], [10])
m4_define([gexiv2_real_version],
          [gexiv2_major_version.gexiv2_minor_version.gexiv2_micro_version])
m4_define([gexiv2_version], [gexiv2_real_version])
m4_define([gexiv2_gir_version], [gexiv2_major_version.gexiv2_minor_version])
m4_define([gexiv2_gir_version_u], m4_joinall([_], gexiv2_major_version,
          gexiv2_minor_version))

# Set to target (stable) major.minor version
#
# This was necessary because the typelib files don't seem to load if the
# version info has more than one period in it.
GIR_VERSION=gexiv2_gir_version
AC_SUBST(GIR_VERSION)
GIR_VERSION_U=gexiv2_gir_version_u
AC_SUBST(GIR_VERSION_U)

m4_define([glib_required_version], [2.38.0])
m4_define([exiv2_required_version], [0.21])

# This number must be edited whenever a change has been made that may affect libgexiv2's
# external interface.  Please see http://sourceware.org/autobook/autobook/autobook_91.html
# for the version numbering convention that should be used for this and
# http://davidz25.blogspot.se/2011/07/writing-c-library-part-5.html#abi-api-versioning for
# guidelines about when this should change.
GEXIV2_VERSION_INFO=2:0:0
AC_SUBST(GEXIV2_VERSION_INFO)

GEXIV2_MAJOR_VERSION=gexiv2_major_version
GEXIV2_MINOR_VERSION=gexiv2_minor_version
GEXIV2_MICRO_VERSION=gexiv2_micro_version
GEXIV2_VERSION=gexiv2_version
AC_SUBST(GEXIV2_MAJOR_VERSION)
AC_SUBST(GEXIV2_MINOR_VERSION)
AC_SUBST(GEXIV2_MICRO_VERSION)
AC_SUBST(GEXIV2_VERSION)

AX_IS_RELEASE([git-directory])
AX_COMPILER_FLAGS

###################
# Check for glib2 #
###################

GLIB_PKGS="gobject-2.0"
GLIB_REQUIRED_VERSION=glib_required_version
AC_SUBST(GLIB_REQUIRED_VERSION)

PKG_CHECK_MODULES(GLIB,
                  glib-2.0 >= $GLIB_REQUIRED_VERSION $GLIB_PKGS,
                  ,
                  AC_MSG_ERROR([GLib is required. Please install it.]))

###################
# Check for exiv2 #
###################

EXIV2_REQUIRED_VERSION=exiv2_required_version
AC_SUBST(EXIV2_REQUIRED_VERSION)

PKG_CHECK_MODULES(EXIV2,
                  exiv2 >= $EXIV2_REQUIRED_VERSION,
                  ,
                  AC_MSG_ERROR([exiv2 is required. Please install it.]))

##############
# Debug Mode #
##############

AC_MSG_CHECKING([debugging mode])
AC_ARG_ENABLE(debug,
              [  --enable-debug          enable debug mode [[default=no]]],
              if eval "test x$enable_debug = xyes"; then
              DEBUG_CFLAGS="-O0"
              fi)

if test -n "$DEBUG_CFLAGS"; then
    AC_MSG_RESULT([yes])
    if test -z "$CXXFLAGS"; then
        CXXFLAGS="-O0 -g -pipe -nostdlib -shared"
    else
        CXXFLAGS="$DEBUG_CFLAGS $CXXFLAGS"
    fi
else
    AC_MSG_RESULT([no])
    if test -z "$CXXFLAGS"; then
        CXXFLAGS="-O2 -g -pipe -nostdlib -shared"
    fi
fi

#################
# Introspection #
#################

GOBJECT_INTROSPECTION_CHECK([0.6.4])

AC_PATH_PROGS([PYTHON2_PATH],[python2],[:])
AC_SUBST(PYTHON2_PATH)
AM_CONDITIONAL([PYTHON2_PATH], [test "x$PYTHON2_PATH" != "x:"])

AC_PATH_PROGS([PYTHON3_PATH],[python3],[:])
AC_SUBST(PYTHON3_PATH)
AM_CONDITIONAL([PYTHON3_PATH], [test "x$PYTHON3_PATH" != "x:"])

AC_ARG_WITH([python2-girdir],
            AS_HELP_STRING([--with-python2-girdir],
                             [Set the GI override file install dir (no to disable)]),
            [ac_py2girdir=$withval],
            [ac_py2girdir=""])

AC_ARG_WITH([python3-girdir],
            AS_HELP_STRING([--with-python3-girdir],
                             [Set the GI override file install dir (no to disable)]),
            [ac_py3girdir=$withval],
            [ac_py3girdir=""])

enable_python2="no (introspection disabled)"
enable_python3="no (introspection disabled)"
PYTHON_CODE="import gi; print(gi._overridesdir)"
AS_IF([test "x$found_introspection" = "xyes"],
      [
    # Used to determine where Python wants the .gi file to be installed ...
    # if Python is not installed, PYTHON2 and PYTHON3 will be empty
    AS_IF([test "x$ac_py2girdir" = "x"],
    [
        AS_IF([test "x$PYTHON2_PATH" != "x:"],
              [PYTHON2_GIDIR=$($PYTHON2_PATH -c "$PYTHON_CODE")],
              [PYTHON2_GIDIR=""])
    ],
    [test "x$ac_py2girdir" = "xno"],
    [
        PYTHON2_GIRDIR=""
    ],
    [
        PYTHON2_GIDIR=$ac_py2girdir
    ])
    AC_MSG_RESULT([Python2 GObject Introspection directory: $PYTHON2_GIDIR])


    AS_IF([test "x$ac_py3girdir" = "x"],
    [
        AS_IF([test "x$PYTHON3_PATH" != "x:"],
              [PYTHON3_GIDIR=$($PYTHON3_PATH -c "$PYTHON_CODE")],
              [PYTHON3_GIDIR=""])
    ],
    [test "x$ac_py3girdir" = "xno"],
    [
        PYTHON3_GIRDIR=""
    ],
    [
        PYTHON3_GIDIR=$ac_py3girdir
    ])
    AC_MSG_RESULT([Python3 GObject Introspection directory: $PYTHON3_GIDIR])

    AS_IF([test "x$PYTHON2_GIDIR" != "x"],
          [enable_python2=yes],[enable_python2=no])
    AS_IF([test "x$PYTHON3_GIDIR" != "x"],
          [enable_python3=yes],[enable_python3=no])

    AC_SUBST(PYTHON2_GIDIR)
    AC_SUBST(PYTHON3_GIDIR)
    ])
AM_CONDITIONAL(HAS_PYTHON2, test "x$PYTHON2_GIDIR" != "x")
AM_CONDITIONAL(HAS_PYTHON3, test "x$PYTHON3_GIDIR" != "x")

#######################
# Tests and utilities #
#######################

AC_CHECK_TOOL([VALAC], [valac], [:])
AM_CONDITIONAL([HAVE_VALAC], [test "x$VALAC" != "x:"])

###########################
# Check target architecture
###########################

AC_MSG_CHECKING([for target architecture])
case x"$target" in
    xNONE | x)
        target_or_host="$host" ;;
    *)
        target_or_host="$target" ;;
esac
AC_MSG_RESULT([$target_or_host])

#################
# Check for OS X
#################

AC_MSG_CHECKING([OS X platform])
case "$target_or_host" in
    *-*-darwin*)
        platform_darwin=yes
    ;;
    *)
        platform_darwin=no
    ;;
esac
AC_MSG_RESULT([$platform_darwin])
AM_CONDITIONAL(PLATFORM_DARWIN, test "$platform_darwin" = "yes")


#################
# Check for Win32
#################

AC_MSG_CHECKING([Win32 platform])
case "$target_or_host" in
    *-*-mingw* | *-*-cygwin*)
        platform_win32=yes
    ;;
    *)
        platform_win32=no
    ;;
esac
AC_MSG_RESULT([$platform_win32])
AM_CONDITIONAL(PLATFORM_WIN32, test "$platform_win32" = "yes")

################
# Check for VALA
################

VAPIGEN_CHECK

###########
# gtk-doc #
###########
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])

################
# glib-mkenums #
################
AC_MSG_CHECKING([for glib-mkenums script])
GLIB_MKENUMS=`$PKG_CONFIG --variable=glib_mkenums glib-2.0`
AS_IF([test "x$GLIB_MKENUMS" = "x"],
    [AC_MSG_ERROR([glib-mkenums not listed in glib-2.0 pkg-config file])],
    [AC_SUBST([GLIB_MKENUMS])
     AC_MSG_RESULT([$GLIB_MKENUMS])])

##############
# Generation #
##############

AC_CONFIG_FILES([
Makefile
gexiv2.pc:gexiv2.pc.in
docs/Makefile
docs/reference/Makefile
])
AC_CONFIG_FILES([test/python2-test], [chmod +x test/python2-test])
AC_CONFIG_FILES([test/python3-test], [chmod +x test/python3-test])

AC_OUTPUT

AC_MSG_RESULT([
GExiv2 will be built with prefix $prefix for $host (cross-compiling: $cross_compiling).

Options:
    Introspection:      $found_introspection
    Python2 binding:    $enable_python2
    Python3 binding:    $enable_python3
])

if test "x$enable_tests" = "xyes"; then
AC_MSG_RESULT([To build tests and utiliies, run "make tests".])
fi

