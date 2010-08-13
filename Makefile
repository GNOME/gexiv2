PKGNAME = gexiv2
VERSION = 0.1.91+trunk

LIBRARY = lib$(PKGNAME)
LIBRARY_BIN = $(LIBRARY).la

INSTALL_PROGRAM = install
INSTALL_DATA = install -m 644

# defaults that may be overridden by configure.mk
PREFIX=/usr/local
BUILD_RELEASE=1
BUILD_DIR=gexiv2
LIB=lib

UNAME := $(shell uname)
SYSTEM := $(UNAME:MINGW32_%=MinGW)

ifeq "$(SYSTEM)" "Linux"
  LINUX = 1
endif

ifeq "$(SYSTEM)" "MinGW"
  WINDOWS = 1
endif

ifeq "$(SYSTEM)" "Darwin"
  MAC = 1
endif

-include configure.mk

SRC_FILES = \
	gexiv2-metadata.cpp \
	gexiv2-metadata-exif.cpp \
	gexiv2-metadata-gps.cpp \
	gexiv2-metadata-iptc.cpp \
	gexiv2-metadata-xmp.cpp \
	gexiv2-stream-io.cpp \
	gexiv2-preview-properties.cpp \
	gexiv2-preview-image.cpp

HEADER_FILES = \
	gexiv2.h \
	gexiv2-metadata.h \
	gexiv2-managed-stream.h \
	gexiv2-metadata-private.h \
	gexiv2-stream-io.h \
	gexiv2-preview-properties.h \
	gexiv2-preview-properties-private.h \
	gexiv2-preview-image.h \
	gexiv2-preview-image-private.h

INSTALLED_HEADER_FILES = \
	gexiv2.h \
	gexiv2-metadata.h \
	gexiv2-managed-stream.h \
	gexiv2-preview-properties.h \
	gexiv2-preview-image.h

EXT_PKGS = \
	gobject-2.0 \
	glib-2.0 \
	exiv2

EXT_PKG_VERSIONS = \
	exiv2 >= 0.19

VAPI_INPUT = \
	gexiv2.deps \
	gexiv2.excludes \
	gexiv2.files \
	gexiv2.metadata \
	gexiv2.namespace

EXPANDED_SRC_FILES = $(foreach src,$(SRC_FILES),gexiv2/$(src))
EXPANDED_OBJ_FILES = $(foreach src,$(SRC_FILES),$(BUILD_DIR)/$(src:.cpp=.o))
EXPANDED_LO_FILES = $(foreach src,$(SRC_FILES),$(BUILD_DIR)/$(src:.cpp=.lo))
EXPANDED_HEADER_FILES = $(foreach hdr,$(HEADER_FILES),gexiv2/$(hdr))
EXPANDED_INSTALLED_HEADER_FILES = $(foreach hdr,$(INSTALLED_HEADER_FILES),gexiv2/$(hdr))
EXPANDED_VAPI_INPUT = $(foreach src,$(VAPI_INPUT),vapi/$(src))
DESTDIR_HEADER_FILES = $(foreach hdr,$(INSTALLED_HEADER_FILES),$(DESTDIR)$(PREFIX)/include/$(PKGNAME)/$(hdr))

VAPI_FILE = $(PKGNAME).vapi
VAPI_GENERATED_FILES = vapi/$(PKGNAME).gi

PC_INPUT = $(PKGNAME).m4
PC_FILE = $(PKGNAME).pc

DIST_FILES = Makefile configure $(EXPANDED_SRC_FILES) $(EXPANDED_HEADER_FILES) $(EXPANDED_VAPI_INPUT) \
	$(VAPI_FILE) $(PC_INPUT) AUTHORS COPYING INSTALLING MAINTAINERS NEWS README THANKS

DIST_TAR = $(LIBRARY)-$(VERSION).tar
DIST_TAR_BZ2 = $(DIST_TAR).bz2
DIST_TAR_GZ = $(DIST_TAR).gz

EXT_PKGS_CFLAGS = `pkg-config --cflags $(EXT_PKGS)`
EXT_PKGS_LDFLAGS = `pkg-config --libs $(EXT_PKGS)`

# setting CFLAGS in configure.mk overrides build type
ifndef CFLAGS
ifdef BUILD_DEBUG
CFLAGS = -O0 -g -pipe -fPIC -nostdlib
else
CFLAGS = -O2 -g -pipe -fPIC -nostdlib
endif
endif

all: $(LIBRARY_BIN) $(PC_FILE)

clean:
	rm -f $(EXPANDED_OBJ_FILES)
	rm -f $(EXPANDED_LO_FILES)
	rm -rf .libs
	rm -rf $(BUILD_DIR)/.libs
	rm -rf $(LIBRARY)-$(VERSION)
	rm -f $(LIBRARY_BIN)
	rm -f $(VAPI_GENERATED_FILES)
	rm -f $(PC_FILE)

dist: $(DIST_FILES)
	mkdir -p $(LIBRARY)-$(VERSION)
	cp --parents $(DIST_FILES) $(LIBRARY)-$(VERSION)
	tar --bzip2 -cvf $(DIST_TAR_BZ2) $(LIBRARY)-$(VERSION)
	tar --gzip -cvf $(DIST_TAR_GZ) $(LIBRARY)-$(VERSION)
	rm -rf $(LIBRARY)-$(VERSION)

distclean: clean
	rm -f configure.mk

vapi: $(VAPI_FILE)
	@

.PHONY: install
install:
	@mkdir -p $(DESTDIR)$(PREFIX)/$(LIB)
	libtool --mode=install $(INSTALL_PROGRAM) $(LIBRARY).la $(DESTDIR)$(PREFIX)/$(LIB)
	@mkdir -p $(DESTDIR)$(PREFIX)/include/$(PKGNAME)
	$(INSTALL_DATA) $(EXPANDED_INSTALLED_HEADER_FILES) $(DESTDIR)$(PREFIX)/include/$(PKGNAME)
	@mkdir -p $(DESTDIR)$(PREFIX)/$(LIB)/pkgconfig
	$(INSTALL_DATA) $(PC_FILE) $(DESTDIR)$(PREFIX)/$(LIB)/pkgconfig
	@mkdir -p $(DESTDIR)$(PREFIX)/share/vala/vapi
	$(INSTALL_DATA) $(VAPI_FILE) $(DESTDIR)$(PREFIX)/share/vala/vapi

install-vapi:
	@mkdir -p $(DESTDIR)$(PREFIX)/share/vala/vapi
	$(INSTALL_DATA) $(VAPI_FILE) $(DESTDIR)$(PREFIX)/share/vala/vapi

uninstall:
	libtool --mode=uninstall rm -f $(DESTDIR)$(PREFIX)/$(LIB)/$(LIBRARY).la
	rm -rf $(DESTDIR)$(PREFIX)/include/$(PKGNAME)
	rm -f $(DESTDIR)$(PREFIX)/$(LIB)/pkgconfig/$(PKGNAME).pc
	rm -f $(DESTDIR)$(PREFIX)/share/vala/vapi/$(PKGNAME).vapi

$(VAPI_FILE): $(EXPANDED_VAPI_INPUT) $(DESTDIR_HEADER_FILES) Makefile $(CONFIG_IN)
	@pkg-config --exists --print-errors $(PKGNAME)
	vala-gen-introspect $(PKGNAME) vapi
	vapigen --library=$(PKGNAME) --metadata=vapi/$(PKGNAME).metadata vapi/$(PKGNAME).gi

$(PC_FILE) : $(PC_INPUT) Makefile $(CONFIG_IN)
	m4 '--define=_VERSION_=$(VERSION)' '--define=_PREFIX_=$(PREFIX)' $< > $@

$(EXPANDED_OBJ_FILES): $(BUILD_DIR)/%.o: gexiv2/%.cpp $(EXPANDED_HEADER_FILES) $(CONFIG_IN) Makefile
	@pkg-config --print-errors --exists '$(EXT_PKG_VERSIONS)'
	@mkdir -p $(BUILD_DIR)
	libtool --mode=compile --tag=CC $(CXX) -c $(EXT_PKGS_CFLAGS) $(CFLAGS) -I. -o $@ $<

$(LIBRARY_BIN): $(EXPANDED_OBJ_FILES)
	libtool --mode=link --tag=CC $(CXX) -rpath $(PREFIX)/$(LIB) $(EXPANDED_LO_FILES) $(EXT_PKGS_LDFLAGS) $(CFLAGS) $(LDFLAGS) -o $(LIBRARY_BIN)

