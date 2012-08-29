PKGNAME = gexiv2
VERSION = 0.4.90
WORD_SIZE:=$(shell getconf LONG_BIT)

# This was necessary because the typelib files don't seem to load if the
# version info has more than one period in it.
GIR_VERSION = 0.4

# This number must be edited whenever a change has been made that may affect libgexiv2's
# external interface.  Please see http://sourceware.org/autobook/autobook/autobook_91.html
# for the version numbering convention that should be used for this.
VERSION_INFO = 1:0:0

LIBRARY = lib$(PKGNAME)
LIBRARY_BIN = $(LIBRARY).la

INSTALL_PROGRAM = install
INSTALL_DATA = install -m 644

# defaults that may be overridden by configure.mk
PREFIX=/usr/local
BUILD_RELEASE=1
BUILD_DIR=gexiv2
LIB=lib

-include configure.mk

# Used to determine where Python wants the .gi file to be installed ... if Python is not installed,
# PYTHON2 and PYTHON3 will be empty
PYTHON="import gi; print(gi._overridesdir)"
PYTHON2=$(shell python2 -c $(PYTHON) 2> /dev/null)
PYTHON3=$(shell python3 -c $(PYTHON) 2> /dev/null)

TYPELIB=`pkg-config gobject-introspection-no-export-1.0 --variable typelibdir`

SRC_FILES = \
	gexiv2-metadata.cpp \
	gexiv2-metadata-exif.cpp \
	gexiv2-metadata-gps.cpp \
	gexiv2-metadata-iptc.cpp \
	gexiv2-metadata-xmp.cpp \
	gexiv2-stream-io.cpp \
	gexiv2-preview-properties.cpp \
	gexiv2-preview-image.cpp \
	gexiv2-log.cpp \
	gexiv2-startup.cpp

HEADER_FILES = \
	gexiv2.h \
	gexiv2-log.h \
	gexiv2-log-private.h \
	gexiv2-metadata.h \
	gexiv2-managed-stream.h \
	gexiv2-metadata-private.h \
	gexiv2-stream-io.h \
	gexiv2-preview-properties.h \
	gexiv2-preview-properties-private.h \
	gexiv2-preview-image.h \
	gexiv2-preview-image-private.h \
	gexiv2-startup.h

INSTALLED_HEADER_FILES = \
	gexiv2.h \
	gexiv2-metadata.h \
	gexiv2-managed-stream.h \
	gexiv2-preview-properties.h \
	gexiv2-preview-image.h \
	gexiv2-log.h \
	gexiv2-startup.h

EXT_PKGS = \
	gobject-2.0 \
	glib-2.0 \
	exiv2

EXT_PKG_VERSIONS = \
	exiv2 >= 0.21

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
ifdef ENABLE_INTROSPECTION
	TYPELIB_FILE = GExiv2-$(GIR_VERSION).typelib
endif

PC_INPUT = $(PKGNAME).m4
PC_FILE = $(PKGNAME).pc

DIST_FILES = Makefile configure $(EXPANDED_SRC_FILES) $(EXPANDED_HEADER_FILES) $(EXPANDED_VAPI_INPUT) \
	$(VAPI_FILE) $(PC_INPUT) AUTHORS COPYING INSTALLING MAINTAINERS NEWS README THANKS

DIST_TAR = $(LIBRARY)-$(VERSION).tar
DIST_TAR_XZ = $(DIST_TAR).xz

EXT_PKGS_CFLAGS = `pkg-config --cflags $(EXT_PKGS)`
EXT_PKGS_LDFLAGS = `pkg-config --libs $(EXT_PKGS)`

# REQUIRED_CFLAGS absolutely get appended to CFLAGS, whatever the
# the value of CFLAGS in the environment.
REQUIRED_CFLAGS=-Wl,-lstdc++

# Because -fPIC can interfere with compilation on 32-bit platforms but 
# is absolutely necessary for AMD64, we check what the target machine's 
# word size is, and set our required flags based upon that.
ifeq "$(WORD_SIZE)" "64"
    REQUIRED_CFLAGS += -fPIC -DPIC
endif
# otherwise, the target is 32-bit, so we don't need to add -fPIC.


# setting CFLAGS in configure.mk overrides build type
ifndef CFLAGS
ifdef BUILD_DEBUG
CFLAGS = -O0 -g -pipe -nostdlib
else
CFLAGS = -O2 -g -pipe -nostdlib
endif
endif

CFLAGS += $(REQUIRED_CFLAGS)

all: $(LIBRARY_BIN) $(PC_FILE) $(TYPELIB_FILE)

%.typelib: %.gir
	g-ir-compiler --includedir=$(BUILD_DIR) -o $@ $<

GExiv2-$(GIR_VERSION).gir: $(LIBRARY_BIN)
	g-ir-scanner --verbose                          \
		--namespace=GExiv2                      \
		--nsversion=$(GIR_VERSION)              \
		--symbol-prefix=$(PKGNAME)              \
		--include=GObject-2.0                   \
		--add-include-path=$(BUILD_DIR)         \
		--output $@                             \
		-I $(PKGNAME)                           \
		$(EXPANDED_INSTALLED_HEADER_FILES)      \
		$(EXPANDED_SRC_FILES)                   \
		--library $(LIBRARY_BIN)                \
		--pkg=gobject-2.0

clean:
	rm -f $(EXPANDED_OBJ_FILES)
	rm -f $(EXPANDED_LO_FILES)
	rm -rf .libs
	rm -rf $(BUILD_DIR)/.libs
	rm -rf $(LIBRARY)-$(VERSION)
	rm -f $(LIBRARY_BIN)
	rm -f $(VAPI_GENERATED_FILES)
	rm -f $(PC_FILE)
	rm -f *.gir *.typelib

dist: $(DIST_FILES)
	mkdir -p $(LIBRARY)-$(VERSION)
	cp --parents --preserve $(DIST_FILES) $(LIBRARY)-$(VERSION)
	tar --xz -cvf $(DIST_TAR_XZ) $(LIBRARY)-$(VERSION)
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
ifdef ENABLE_INTROSPECTION
	$(INSTALL_DATA) GExiv2-$(GIR_VERSION).gir $(DESTDIR)$(PREFIX)/share/gir-1.0
	$(INSTALL_DATA) GExiv2-$(GIR_VERSION).typelib $(DESTDIR)$(TYPELIB)
# No introspection, no .gi file for the Python module to call into
ifneq "$(PYTHON2)" ""
	$(INSTALL_DATA) GExiv2.py $(DESTDIR)$(PYTHON2)
endif
ifneq "$(PYTHON3)" ""
	$(INSTALL_DATA) GExiv2.py $(DESTDIR)$(PYTHON3)
endif
endif

install-vapi:
	@mkdir -p $(DESTDIR)$(PREFIX)/share/vala/vapi
	$(INSTALL_DATA) $(VAPI_FILE) $(DESTDIR)$(PREFIX)/share/vala/vapi

uninstall:
	libtool --mode=uninstall rm -f $(DESTDIR)$(PREFIX)/$(LIB)/$(LIBRARY).la
	rm -rf $(DESTDIR)$(PREFIX)/include/$(PKGNAME)
	rm -f $(DESTDIR)$(PREFIX)/$(LIB)/pkgconfig/$(PKGNAME).pc
	rm -f $(DESTDIR)$(PREFIX)/share/vala/vapi/$(PKGNAME).vapi
ifdef ENABLE_INTROSPECTION
	rm -f $(DESTDIR)$(PREFIX)/share/gir-1.0/GExiv2-$(GIR_VERSION).gir
	rm -f $(DESTDIR)$(TYPELIB)/GExiv2-$(GIR_VERSION).typelib
ifneq "$(PYTHON2)" ""
	rm -rf $(DESTDIR)$(PYTHON2)/GExiv2.py
endif
ifneq "$(PYTHON3)" ""
	rm -rf $(DESTDIR)$(PYTHON3)/GExiv2.py
endif
endif

$(VAPI_FILE): $(EXPANDED_VAPI_INPUT) $(DESTDIR_HEADER_FILES) Makefile $(CONFIG_IN)
	@echo NOTE: This version of gexiv2 must be installed to generate a VAPI file.
	@pkg-config --exact-version=$(VERSION) --print-errors $(PKGNAME)
	vala-gen-introspect $(PKGNAME) vapi
	vapigen --library=$(PKGNAME) --metadatadir=vapi vapi/$(PKGNAME).gi

$(PC_FILE): $(PC_INPUT) Makefile $(CONFIG_IN)
	m4 '--define=_VERSION_=$(VERSION)' '--define=_PREFIX_=$(PREFIX)' $< > $@

$(EXPANDED_OBJ_FILES): $(BUILD_DIR)/%.o: gexiv2/%.cpp $(EXPANDED_HEADER_FILES) $(CONFIG_IN) Makefile
	@pkg-config --print-errors --exists '$(EXT_PKG_VERSIONS)'
	@mkdir -p $(BUILD_DIR)
	libtool --mode=compile --tag=CC $(CXX) -c $(EXT_PKGS_CFLAGS) $(CFLAGS) -I. -o $@ $<

$(LIBRARY_BIN): $(EXPANDED_OBJ_FILES)
	libtool --mode=link --tag=CC $(CXX) -rpath $(PREFIX)/$(LIB) $(EXPANDED_LO_FILES) $(EXT_PKGS_LDFLAGS) $(CFLAGS) $(LDFLAGS) -version-info $(VERSION_INFO) -o $(LIBRARY_BIN)

