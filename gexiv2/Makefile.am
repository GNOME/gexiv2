lib_LTLIBRARIES = libgexiv2.la

INCLUDES =							\
	$(EXIV2_CFLAGS)					\
	$(GLIB_CFLAGS)					\
	$(GOBJECT_CFLAGS)

CFLAGS = -g

libgexiv2_la_SOURCES =				\
	gexiv2-metadata.cpp		\
	gexiv2-metadata-exif.cpp	\
	gexiv2-metadata-xmp.cpp	\
	gexiv2-metadata-iptc.cpp	\
	gexiv2-metadata-gps.cpp	\
	gexiv2-metadata.h			\
	gexiv2-metadata-private.h \
	gexiv2-stream-io.cpp \
	gexiv2-stream-io.h \
	gexiv2-managed-stream.h

libgexiv2_la_INCLUDES =				\
	gexiv2-metadata.h			\
	gexiv2-metadata-private.h

libgexiv2_la_LIBADD =				\
	$(EXIV2_LIBS)					\
	$(GLIB_LIBS)					\
	$(GOBJECT_LIBS)

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = gexiv2.pc

