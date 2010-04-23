prefix=_PREFIX_
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: GExiv2
Description: GObject bindings for exiv2
Requires: exiv2 glib-2.0 gobject-2.0
Version: _VERSION_
Libs: -L${libdir} -lgexiv2
Cflags: -I${includedir}/gexiv2 -I${libdir}/gexiv2/include

