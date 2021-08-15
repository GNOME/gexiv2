/*
 * gexiv2-metadata.cpp
 *
 * Author(s)
 *  Mike Gemuende <mike@gemuende.de>
 *  Jim Nelson <jim@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "gexiv2-metadata.h"
#include "gexiv2-metadata-private.h"
#include "gexiv2-stream-io.h"
#include "gexiv2-managed-stream.h"
#include "gexiv2-preview-properties.h"
#include "gexiv2-preview-properties-private.h"
#include "gexiv2-preview-image.h"
#include "gexiv2-preview-image-private.h"
#include "gexiv2-log.h"
#include "gexiv2-log-private.h"
#include <string>
#include <cmath>
#include <glib-object.h>
#include <gio/gio.h>

#ifdef G_OS_WIN32
#include <glib/gwin32.h>
#endif

#include <exiv2/exiv2.hpp>

#if EXIV2_TEST_VERSION(0,27,99)
using image_ptr = Exiv2::Image::UniquePtr;
#else
using image_ptr = Exiv2::Image::AutoPtr;
#endif

namespace {
class GioIo : public Exiv2::BasicIo {
public:
    GioIo (GInputStream *is)
        : BasicIo ()
        , _is (G_INPUT_STREAM (g_object_ref (is)))
        , _seekable(G_IS_SEEKABLE(_is) ? G_SEEKABLE (_is) : NULL)
        , _error{nullptr}
        , _eof{false}
        {}
#if EXIV2_TEST_VERSION(0,27,99)
    using size_type = long;
#else
    using size_type = long;
#endif

    size_type _size;

    ~GioIo() { g_clear_object (&_is); g_clear_error (&_error); _seekable = NULL;}
#if defined(_MSC_VER)
    typedef int64_t seek_offset_t;
#else
    typedef long seek_offset_t;
#endif

#if EXIV2_TEST_VERSION(0,27,99)
    using ptr_type = Exiv2::BasicIo::UniquePtr;
#else
    using ptr_type = Exiv2::BasicIo::AutoPtr;
#endif

    int open() override {
        if (_seekable == nullptr)
            return 0;

        if (_size >= 0)
            return 0;

        auto position = tell();
        seek (0, Exiv2::BasicIo::end);
        _size = tell();
        seek (position, Exiv2::BasicIo::beg);

        return 0;
    }

    int close() override { return 0; }

    // Writing is not supported
    size_type write(const Exiv2::byte* /*data*/, size_type /*wcount*/) override { return 0; }
    size_type write(BasicIo& /*src*/) override { return 0; }
    int putb(Exiv2::byte /*data*/) override { return EOF; }

    Exiv2::DataBuf read(size_type rcount) override {
        Exiv2::DataBuf b{rcount};

        auto bytes_read = this->read(b.pData_, rcount);
        if (bytes_read > 0 && bytes_read != rcount) {
            b.reset({b.pData_, bytes_read});
        }

        return b;
    }

    size_type read(Exiv2::byte* buf, size_type rcount) override {
        GError *error = NULL;
        gssize result = 0;

        result = g_input_stream_read(_is, reinterpret_cast<void *>(buf), rcount, NULL, &error);
        if (error != NULL) {
            g_critical ("Error reading from stream: %d %s", error->code, error->message);
            g_clear_error (&_error);
            _error = error;

#if EXIV2_TEST_VERSION(0,27,0)
            throw Exiv2::Error(Exiv2::ErrorCode::kerFailedToReadImageData);
#else
            throw Exiv2::Error(2);
#endif
            return 0;
        }

        if (result == 0) {
            _eof = true;
        } else {
            _eof = false;
        }

        return result;
    }

    int getb() override {
        Exiv2::byte b;
        return this->read (&b, 1) == 1 ? b : EOF;
    }

    void transfer(Exiv2::BasicIo& /*src*/) override {
        // Does not seem necessary for Read-only support
    }

    int seek(seek_offset_t offset, Exiv2::BasicIo::Position position) override {
        if (_seekable != NULL && g_seekable_can_seek (_seekable)) {
            GSeekType t = G_SEEK_SET;
            switch (position) {
                case Exiv2::BasicIo::cur:
                    t = G_SEEK_CUR;
                    break;
                case Exiv2::BasicIo::beg:
                    t = G_SEEK_SET;
                    break;
                case Exiv2::BasicIo::end:
                    t = G_SEEK_END;
                    break;
                default:
                    g_assert_not_reached ();
                    break;
            }

            GError *error = NULL;
            g_seekable_seek (_seekable, offset, t, NULL, &error);
            if (error != NULL) {
                g_clear_error(&_error);
                g_critical ("Failed to seek: %s", error->message);
                _error = error;

                return -1;
            }

            return 0;
        } else {
            // Can only seek forward here...
            if (position != Exiv2::BasicIo::cur) {
                return -1;
            }

            GError *error = NULL;
            g_input_stream_skip (_is, offset, NULL, &error);
            if (error != NULL) {
                g_clear_error(&_error);
                _error = error;
                g_critical("Failed to seek forward: %s", error->message);

                return -1;
            }

            return 0;
        }
    }

    Exiv2::byte* mmap(bool /*writable*/) override { return nullptr; }

    int munmap() override { return 0; }

    long tell() const override {
        if (_seekable != nullptr && g_seekable_can_seek (_seekable)) {
            return static_cast<long>(g_seekable_tell (_seekable));
        } else {
            return -1;
        }
    }

    size_t size() const override { return static_cast<size_t>(_size); }

    bool isopen() const override { return true; }

    int error() const override { return _error == nullptr ? 0 : 1; }

    bool eof() const override { return _eof; }

    std::string path() const override { return "GIO Wrapper"; }

#ifdef EXV_UNICODE_PATH
    std::wstring wpath() const override {
        std::string p = path();
        std::wstring w(p.length(), L' ');
        std::copy(p.begin(), p.end(), w.begin());
        return w;
    }
#endif

#if EXIV2_TEST_VERSION(0,27,99)
    Exiv2::BasicIo::UniquePtr temporary() const {
        return Exiv2::BasicIo::UniquePtr(nullptr);
    }
#else
    Exiv2::BasicIo::AutoPtr temporary() const {
        return Exiv2::BasicIo::AutoPtr(nullptr);
    }
#endif


private:
    GInputStream *_is;
    GSeekable *_seekable;
    GError *_error;
    bool _eof;
}; // class GioIo
} // Anonymous namespace

// -----------------------------------------------------------------------------
// Misc internal helper functions

// Port of the NaturalCollate.vala collation key generation to C++
// Simplified to assume that all XMP keys are ASCII and not UTF-8
// Original source:
// https://gitlab.gnome.org/GNOME/shotwell/-/blob/master/src/NaturalCollate.vala
std::string detail::collate_key(const std::string& str) {
    constexpr char SUPERDIGIT = ':';
    constexpr char COLLATION_SENTINAL[] = "\x01\x01\x01";
    constexpr char NUM_SENTINEL = 0x2;

    std::stringstream in{str};
    std::stringstream out{};

    while (not in.eof()) {
        // As long as there are no digits, we put them from input to output
        while (not std::isdigit(in.peek()) && not in.eof()) {
            out << static_cast<char>(in.get());
        }

        if (not in.eof()) {

            // We read the number (integer only)...
            uint64_t number;
            in >> number;

            std::string to_append(std::to_string(number).length(), SUPERDIGIT);

            // ... and append it together with its length in : to the output
            out << COLLATION_SENTINAL << NUM_SENTINEL << to_append << number;
        }
    }

    // Add a sentinal for good measure (no idea, follows the original code)
    out << NUM_SENTINEL;

    return out.str();
}

G_BEGIN_DECLS

G_DEFINE_TYPE_WITH_CODE (GExiv2Metadata, gexiv2_metadata, G_TYPE_OBJECT, G_ADD_PRIVATE (GExiv2Metadata));

static void gexiv2_metadata_finalize (GObject *object);
static void gexiv2_metadata_set_comment_internal (GExiv2Metadata *self, const gchar *new_comment);

static gboolean gexiv2_metadata_open_internal (GExiv2Metadata *self, GError **error);
static gboolean gexiv2_metadata_save_internal (GExiv2Metadata *self, image_ptr image, GError **error);

static void gexiv2_metadata_init (GExiv2Metadata *self) {
    self->priv = (GExiv2MetadataPrivate*) gexiv2_metadata_get_instance_private(self);

    /* Initialize members */
    self->priv->comment = NULL;
    self->priv->mime_type = NULL;
    self->priv->preview_manager = NULL;
    self->priv->preview_properties = NULL;
    self->priv->pixel_width = -1;
    self->priv->pixel_height = -1;
    
    /* the others are static members and need not to be initialized */
    
    /*  install GLib logging in place of Exiv2's default (where everything is dumped to stderr)
        but only if the user hasn't beaten us to the punch
        
        if user wants old behavior they should code this:
            gexiv2_log_set_handler(gexiv2_log_get_default_handler());
    */
    if (!gexiv2_log_is_handler_installed())
        gexiv2_log_use_glib_logging();
}

static void gexiv2_metadata_class_init (GExiv2MetadataClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    
    gobject_class->finalize = gexiv2_metadata_finalize;
}


static void gexiv2_metadata_finalize (GObject *object) {
    GExiv2Metadata *self = GEXIV2_METADATA (object);
    
    g_free (self->priv->comment);
    g_free (self->priv->mime_type);
    delete self->priv->preview_manager;
    
    if (self->priv->preview_properties != NULL) {
        int ctr = 0;
        while (self->priv->preview_properties[ctr] != NULL)
            gexiv2_preview_properties_free(self->priv->preview_properties[ctr++]);
        
        g_free (self->priv->preview_properties);
    }
    
    if (self->priv->image.get() != NULL)
        self->priv->image.reset();
    
    G_OBJECT_CLASS (gexiv2_metadata_parent_class)->finalize (object);
}

GExiv2Metadata* gexiv2_metadata_new (void) {
    return GEXIV2_METADATA (g_object_new (GEXIV2_TYPE_METADATA, NULL));
}

void gexiv2_metadata_free (GExiv2Metadata *self) {
    g_return_if_fail(GEXIV2_IS_METADATA(self));
    
    g_object_unref(self);
}

static void gexiv2_metadata_set_comment_internal (GExiv2Metadata* self, const gchar* new_comment) {
    g_return_if_fail (GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv != nullptr);

    g_free (self->priv->comment);

    self->priv->comment = g_strdup (new_comment);
}

static void gexiv2_metadata_init_internal(GExiv2Metadata* self, GError** error) {
    g_return_if_fail(GEXIV2_IS_METADATA(self));
    g_return_if_fail(self->priv != nullptr);
    g_return_if_fail(self->priv->image.get() != nullptr);
    g_return_if_fail(error == nullptr || *error == nullptr);

    GExiv2MetadataPrivate* priv = self->priv;
    try {

        gexiv2_metadata_set_comment_internal(self, priv->image->comment().c_str());
        priv->mime_type = g_strdup(priv->image->mimeType().c_str());

        priv->pixel_width = priv->image->pixelWidth();
        priv->pixel_height = priv->image->pixelHeight();

        Exiv2::AccessMode mode = priv->image->checkMode(Exiv2::mdExif);
        priv->supports_exif = (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);

        mode = priv->image->checkMode(Exiv2::mdXmp);
        priv->supports_xmp = (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);

        mode = priv->image->checkMode(Exiv2::mdIptc);
        priv->supports_iptc = (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);

        priv->preview_manager = new Exiv2::PreviewManager(*priv->image.get());

        Exiv2::PreviewPropertiesList list = priv->preview_manager->getPreviewProperties();
        int count = list.size();
        if (count > 0) {
            priv->preview_properties = g_new(GExiv2PreviewProperties*, count + 1);
            for (int ctr = 0; ctr < count; ctr++)
                priv->preview_properties[ctr] = gexiv2_preview_properties_new(list[ctr]);
            priv->preview_properties[count] = nullptr;
        }
    } catch (Exiv2::Error& e) {
        if (priv->mime_type)
            g_free(priv->mime_type);

        if (priv->preview_manager)
            delete priv->preview_manager;

        g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());
    }
}

static gboolean gexiv2_metadata_open_internal (GExiv2Metadata* self, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(self->priv != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    if (self->priv->image.get() == nullptr || !self->priv->image->good()) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"),
            501, "unsupported format");
        return FALSE;
    }

    try {
        self->priv->image->readMetadata();
        gexiv2_metadata_init_internal(self, error);

        return !(error && *error);
    } catch (Exiv2::Error& e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());
    }

    return FALSE;
}

#ifdef EXV_UNICODE_PATH
static std::wstring convert_path(const char* path, GError** error) {
    wchar_t* wfile{nullptr};
    wfile = reinterpret_cast<wchar_t*>(g_utf8_to_utf16(path, -1, nullptr, nullptr, error));
    // Error is set by g_utf8_to_utf16()
    if (wfile == nullptr) {
        return {};
    }

    std::wstring file{wfile};
    g_free(wfile);

    return file;
}
#else
static std::string convert_path(const char* path, GError** error) {
#ifdef G_OS_WIN32
    std::string file;
    char* local_path = g_win32_locale_filename_from_utf8(path);
    if (local_path == nullptr) {
        char* msg = g_strdup_printf("Failed to convert \"%s\" to locale \"%s\"", path, g_win32_getlocale());
        g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_INVALID_FILENAME, msg);

        g_free(msg);

        return FALSE;
    }
    file = local_path;
    g_free(local_path);

    return file;
#else
    g_return_val_if_fail(error != nullptr && *error == nullptr, std::string());

    return std::string{path};
#endif
}
#endif

gboolean gexiv2_metadata_open_path(GExiv2Metadata* self, const gchar* path, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    try {
        GError* inner_error = nullptr;

        auto converted_path = convert_path(path, &inner_error);
        if (inner_error != nullptr) {
            g_propagate_error(error, inner_error);

            return FALSE;
        }
        self->priv->image = Exiv2::ImageFactory::open(converted_path);

        return gexiv2_metadata_open_internal (self, error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
#ifdef EXV_UNICODE_PATH
    catch (Exiv2::WError &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
#endif

    return FALSE;
}

gboolean gexiv2_metadata_open_buf(GExiv2Metadata* self, const guint8* data, glong n_data, GError** error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    
    try {
        self->priv->image = Exiv2::ImageFactory::open (data, n_data);
        
        return gexiv2_metadata_open_internal (self, error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), 501, "unsupported format");
    }
    
    return FALSE;
}

gboolean gexiv2_metadata_open_stream (GExiv2Metadata *self, ManagedStreamCallbacks* cb, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    
    try {
        StreamIo::ptr_type stream_ptr{new StreamIo (cb)};
#if EXIV2_TEST_VERSION(0,27,99)
        self->priv->image = Exiv2::ImageFactory::open (std::move(stream_ptr));
#else
        self->priv->image = Exiv2::ImageFactory::open (stream_ptr);
#endif
        
        return gexiv2_metadata_open_internal (self, error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
    
    return FALSE;
}

gboolean gexiv2_metadata_from_stream(GExiv2Metadata *self, GInputStream *stream, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);

    try {
        GioIo::ptr_type gio_ptr{new GioIo (stream)};
#if EXIV2_TEST_VERSION(0,27,99)
        self->priv->image = Exiv2::ImageFactory::open (std::move(gio_ptr));
#else
        self->priv->image = Exiv2::ImageFactory::open (gio_ptr);
#endif

        return gexiv2_metadata_open_internal (self, error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code(), e.what());
    }

    return FALSE;
}

// Exiv2 does not today offer a clean way to decode a buffer with only the JFIF APP1 segment,
// where EXIF lives.  This is a common situation when reading EXIF metadata straight from a
// camera (i.e. via gPhoto) where accessing the entire JPEG image is inconvenient.
//
// This method of directly decoding the buffer from offset 10 (where the EXIF data looks like
// a TIFF image) comes from this ticket: http://dev.exiv2.org/issues/show/465  When this ticket
// is closed this code can revert to a direct image decode.  To be more flexible for other
// file formats that encode EXIF via other means (such as PNG), search for the TIFF header and
// decode from there.
gboolean gexiv2_metadata_from_app1_segment(GExiv2Metadata* self, const guint8* data, glong n_data, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(data != nullptr, FALSE);
    g_return_val_if_fail(n_data > 0, FALSE);
    
    int offset = 0;
    while (offset < n_data - 1) {
        if ((data[offset] == 0x4D && data[offset + 1] == 0x4D)
            || (data[offset] == 0x49 && data[offset + 1] == 0x49))
            break;
        
        offset++;
    }
    
    if (offset >= n_data - 1) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), 501, "unsupported format");\
        
        return FALSE;
    }
    
    try {
        self->priv->image = Exiv2::ImageFactory::create(Exiv2::ImageType::jpeg);
        if (self->priv->image.get() == nullptr)
            return FALSE;
        
        Exiv2::ExifParser::decode(self->priv->image->exifData(), data + offset, n_data - offset);
        gexiv2_metadata_init_internal(self, error);
        if (error && *error) {
            // Cleanup
            delete self->priv->image.release();
            return FALSE;
        }

        return TRUE;
    } catch (Exiv2::Error &e) {
        delete self->priv->image.release();
        g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());
    }
    return FALSE;
}

static gboolean gexiv2_metadata_save_internal (GExiv2Metadata *self, image_ptr image, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(self->priv != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    if (image.get() == nullptr || !image->good()) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"),
            501, "format seems not to be supported");
        
        return FALSE;
    }

    try {
        image->readMetadata();

        Exiv2::AccessMode mode = image->checkMode(Exiv2::mdExif);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite) {
            /* For tiff some image data is stored in exif. This should not
               be overwritten. (see libkexiv2/kexiv2.cpp)
             */
            if (image->mimeType() == "image/tiff") {
                // FIXME
                /*Exiv2::ExifData new_exif_data = self->priv->exif_data;
                Exiv2::ExifData old_exif_data = image->getExifData ();

                new_exif_data["Exif.Image.ImageWidth"];
                new_exif_data["Exif.Image.ImageLength"];
                new_exif_data["Exif.Image.BitsPerSample"];
                new_exif_data["Exif.Image.Compression"];
                new_exif_data["Exif.Image.PhotometricInterpretation"];
                new_exif_data["Exif.Image.FillOrder"];
                new_exif_data["Exif.Image.SamplesPerPixel"];
                new_exif_data["Exif.Image.StripOffsets"];
                new_exif_data["Exif.Image.RowsPerStrip"];
                new_exif_data["Exif.Image.StripByteCounts"];
                new_exif_data["Exif.Image.XResolution"];
                new_exif_data["Exif.Image.YResolution"];
                new_exif_data["Exif.Image.PlanarConfiguration"];
                new_exif_data["Exif.Image.ResolutionUnit"];
                */

                image->setExifData(self->priv->image->exifData());
            } else {
                image->setExifData(self->priv->image->exifData());
            }
        }

        mode = image->checkMode(Exiv2::mdXmp);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
            image->setXmpData(self->priv->image->xmpData());

        mode = image->checkMode(Exiv2::mdIptc);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
            image->setIptcData(self->priv->image->iptcData());

        mode = image->checkMode(Exiv2::mdComment);
        if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
            image->setComment(self->priv->comment);

        image->writeMetadata();

        return TRUE;
    } catch (Exiv2::Error& e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());
    }
    return FALSE;
}

gboolean gexiv2_metadata_save_external (GExiv2Metadata *self, const gchar *path, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);

    try {
        GError* inner_error = nullptr;

        auto local_path = convert_path(path, &inner_error);
        if (inner_error != nullptr) {
            g_propagate_error(error, inner_error);

            return FALSE;
        }

        return gexiv2_metadata_save_internal(self,
                                             Exiv2::ImageFactory::create(Exiv2::ImageType::xmp, local_path),
                                             error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
#ifdef EXV_UNICODE_PATH
    catch (Exiv2::WError &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
#endif

    return FALSE;
}

gboolean gexiv2_metadata_save_file (GExiv2Metadata *self, const gchar *path, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    
    try {
        GError* inner_error = nullptr;

        auto local_path = convert_path(path, &inner_error);
        if (inner_error != nullptr) {
            g_propagate_error(error, inner_error);

            return FALSE;
        }

        return gexiv2_metadata_save_internal(self, Exiv2::ImageFactory::open(local_path), error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
#ifdef EXV_UNICODE_PATH
    catch (Exiv2::WError &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
#endif
    
    return FALSE;
}

gboolean gexiv2_metadata_save_stream (GExiv2Metadata *self, ManagedStreamCallbacks* cb, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    
    try {
        StreamIo::ptr_type stream_ptr{new StreamIo (cb)};
        
#if EXIV2_TEST_VERSION(0,27,99)
        return gexiv2_metadata_save_internal (self, Exiv2::ImageFactory::open (std::move(stream_ptr)), error);
#else
        return gexiv2_metadata_save_internal (self, Exiv2::ImageFactory::open (stream_ptr), error);
#endif
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
    
    return FALSE;
}

gboolean gexiv2_metadata_has_tag(GExiv2Metadata *self, const gchar* tag) {
    GError* error = nullptr;
    gboolean value = FALSE;

    value = gexiv2_metadata_try_has_tag(self, tag, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gboolean gexiv2_metadata_try_has_tag(GExiv2Metadata* self, const gchar* tag, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_has_xmp_tag(self, tag);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_has_exif_tag(self, tag);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_has_iptc_tag(self, tag);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return FALSE;
}

gboolean gexiv2_metadata_clear_tag(GExiv2Metadata *self, const gchar* tag) {
    GError* error = nullptr;
    gboolean value = FALSE;

    value = gexiv2_metadata_try_clear_tag(self, tag, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gboolean gexiv2_metadata_try_clear_tag(GExiv2Metadata* self, const gchar* tag, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_clear_xmp_tag(self, tag);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_clear_exif_tag(self, tag);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_clear_iptc_tag(self, tag);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return FALSE;
}

void gexiv2_metadata_clear (GExiv2Metadata *self) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv != nullptr);
    g_return_if_fail(self->priv->image.get() != nullptr);

    gexiv2_metadata_clear_exif (self);
    gexiv2_metadata_clear_xmp (self);
    gexiv2_metadata_clear_iptc (self);

    gexiv2_metadata_clear_comment (self);

    self->priv->image->clearMetadata();
}

const gchar* gexiv2_metadata_get_mime_type (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(self->priv != nullptr, nullptr);

    return self->priv->mime_type;
}

GExiv2Orientation gexiv2_metadata_get_orientation (GExiv2Metadata *self) {
    GError* error = nullptr;
    GExiv2Orientation value = GEXIV2_ORIENTATION_UNSPECIFIED;

    value = gexiv2_metadata_try_get_orientation(self, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

GExiv2Orientation gexiv2_metadata_try_get_orientation(GExiv2Metadata* self, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), GEXIV2_ORIENTATION_UNSPECIFIED);
    g_return_val_if_fail(self->priv->image.get() != nullptr, GEXIV2_ORIENTATION_UNSPECIFIED);
    g_return_val_if_fail(error == nullptr || *error == nullptr, GEXIV2_ORIENTATION_UNSPECIFIED);

    if (gexiv2_metadata_has_exif(self)) {
        // Because some cameras set a wrong standard exif orientation tag,
        // We need to check makernote tags first!
        if (gexiv2_metadata_has_exif_tag(self, "Exif.MinoltaCs7D.Rotation")) {
            long orientation = gexiv2_metadata_get_exif_tag_long(self, "Exif.MinoltaCs7D.Rotation", error);
            if (error && *error) {
                return GEXIV2_ORIENTATION_UNSPECIFIED;
            }
            switch (orientation) {
                case 76:
                    return GEXIV2_ORIENTATION_ROT_90;
                case 82:
                    return GEXIV2_ORIENTATION_ROT_270;
                case 72:
                    return GEXIV2_ORIENTATION_UNSPECIFIED;
                default:
                    g_debug ("Unknown Minolta rotation value %ld, ignoring", orientation);
            }
        }

        if (gexiv2_metadata_has_exif_tag(self, "Exif.MinoltaCs5D.Rotation")) {
            long orientation = gexiv2_metadata_get_exif_tag_long(self, "Exif.MinoltaCs5D.Rotation", error);
            if (error && *error) {
                return GEXIV2_ORIENTATION_UNSPECIFIED;
            }

            switch (orientation) {
                case 76:
                    return GEXIV2_ORIENTATION_ROT_90;
                case 82:
                    return GEXIV2_ORIENTATION_ROT_270;
                case 72:
                    return GEXIV2_ORIENTATION_UNSPECIFIED;
                default:
                    g_debug ("Unknown Minolta rotation value %ld, ignoring", orientation);
            }
        }

        GExiv2Orientation orientation =
            (GExiv2Orientation) gexiv2_metadata_get_exif_tag_long(self, "Exif.Image.Orientation", error);
        if (error && *error) {
            return GEXIV2_ORIENTATION_UNSPECIFIED;
        }

        if (orientation >= GEXIV2_ORIENTATION_MIN && orientation <= GEXIV2_ORIENTATION_MAX)
            return orientation;
    }
    
    GExiv2Orientation orientation = GEXIV2_ORIENTATION_UNSPECIFIED;
    if (gexiv2_metadata_has_xmp_tag(self, "Xmp.tiff.ImageWidth")) {
        orientation = (GExiv2Orientation) gexiv2_metadata_get_xmp_tag_long(self, "Xmp.tiff.ImageWidth", error);
        if (error && *error) {
            return GEXIV2_ORIENTATION_UNSPECIFIED;
        }
    }
    
    return (orientation < GEXIV2_ORIENTATION_MIN || orientation > GEXIV2_ORIENTATION_MAX)
        ? GEXIV2_ORIENTATION_UNSPECIFIED : orientation;
}

gboolean gexiv2_metadata_get_supports_exif (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(self->priv != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);

    return self->priv->supports_exif;
}

gboolean gexiv2_metadata_get_supports_xmp (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(self->priv != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);

    return self->priv->supports_xmp;
}

gboolean gexiv2_metadata_get_supports_iptc (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(self->priv != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);

    return self->priv->supports_iptc;
}

void gexiv2_metadata_set_orientation (GExiv2Metadata *self, GExiv2Orientation orientation) {
    GError* error = nullptr;

    gexiv2_metadata_try_set_orientation(self, orientation, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }
}

void gexiv2_metadata_try_set_orientation(GExiv2Metadata* self, GExiv2Orientation orientation, GError** error) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv->image.get() != nullptr);
    g_return_if_fail(orientation <= GEXIV2_ORIENTATION_ROT_270);
    g_return_if_fail(orientation >= GEXIV2_ORIENTATION_UNSPECIFIED);
    g_return_if_fail(error == nullptr || *error == nullptr);

    try {
        Exiv2::ExifData& exif_data = self->priv->image->exifData();
        Exiv2::XmpData& xmp_data = self->priv->image->xmpData();

        exif_data["Exif.Image.Orientation"] = static_cast<uint16_t>(orientation);
        xmp_data["Xmp.tiff.Orientation"] = static_cast<uint16_t>(orientation);

        gexiv2_metadata_clear_exif_tag(self, "Exif.MinoltaCs7D.Rotation");
        gexiv2_metadata_clear_exif_tag(self, "Exif.MinoltaCs5D.Rotation");
    } catch (Exiv2::Error& e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());
    }
}

gint gexiv2_metadata_get_pixel_width (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), -1);
    g_return_val_if_fail(self->priv != nullptr, -1);
    g_return_val_if_fail(self->priv->image.get() != nullptr, -1);

    return self->priv->pixel_width;
}

gint gexiv2_metadata_get_pixel_height (GExiv2Metadata *self) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), -1);
    g_return_val_if_fail(self->priv != nullptr, -1);
    g_return_val_if_fail(self->priv->image.get() != nullptr, -1);

    return self->priv->pixel_height;
}

gint gexiv2_metadata_get_metadata_pixel_width (GExiv2Metadata *self) {
    GError* error = nullptr;
    gint value = -1;

    value = gexiv2_metadata_try_get_metadata_pixel_width(self, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gint gexiv2_metadata_try_get_metadata_pixel_width(GExiv2Metadata* self, GError** error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), -1);
    g_return_val_if_fail(self->priv->image.get() != nullptr, -1);
    g_return_val_if_fail(error == nullptr || *error == nullptr, -1);

    if (gexiv2_metadata_has_exif(self)) {
        if (gexiv2_metadata_has_exif_tag(self, "Exif.Photo.PixelXDimension"))
            return gexiv2_metadata_get_exif_tag_long(self, "Exif.Photo.PixelXDimension", error);

        if (gexiv2_metadata_has_exif_tag(self, "Exif.Image.ImageWidth"))
            return gexiv2_metadata_get_exif_tag_long(self, "Exif.Image.ImageWidth", error);
    }
    
    if (gexiv2_metadata_has_xmp(self)) {
        if (gexiv2_metadata_has_xmp_tag(self, "Xmp.tiff.ImageWidth"))
            return gexiv2_metadata_get_xmp_tag_long(self, "Xmp.tiff.ImageWidth", error);

        if (gexiv2_metadata_has_xmp_tag(self, "Xmp.exif.PixelXDimension"))
            return gexiv2_metadata_get_xmp_tag_long(self, "Xmp.exif.PixelXDimension", error);
    }
    
    return -1;
}

gint gexiv2_metadata_get_metadata_pixel_height (GExiv2Metadata *self) {
    GError* error = nullptr;
    gint value = -1;

    value = gexiv2_metadata_try_get_metadata_pixel_height(self, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gint gexiv2_metadata_try_get_metadata_pixel_height(GExiv2Metadata* self, GError** error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), -1);
    g_return_val_if_fail(self->priv->image.get() != nullptr, -1);
    g_return_val_if_fail(error == nullptr || *error == nullptr, -1);

    if (gexiv2_metadata_has_exif(self)) {
        if (gexiv2_metadata_has_exif_tag(self, "Exif.Photo.PixelYDimension"))
            return gexiv2_metadata_get_exif_tag_long(self, "Exif.Photo.PixelYDimension", error);

        if (gexiv2_metadata_has_exif_tag(self, "Exif.Image.ImageLength"))
            return gexiv2_metadata_get_exif_tag_long(self, "Exif.Image.ImageLength", error);
    }
    
    if (gexiv2_metadata_has_xmp(self)) {
        if (gexiv2_metadata_has_xmp_tag(self, "Xmp.tiff.ImageHeight"))
            return gexiv2_metadata_get_xmp_tag_long(self, "Xmp.tiff.ImageHeight", error);

        if (gexiv2_metadata_has_xmp_tag(self, "Xmp.exif.PixelYDimension"))
            return gexiv2_metadata_get_xmp_tag_long(self, "Xmp.exif.PixelYDimension", error);
    }
    
    return -1;
}

void gexiv2_metadata_set_metadata_pixel_width (GExiv2Metadata *self, gint width) {
    GError* error = nullptr;

    gexiv2_metadata_try_set_metadata_pixel_width(self, width, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }
}

void gexiv2_metadata_try_set_metadata_pixel_width(GExiv2Metadata* self, gint width, GError** error) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv->image.get() != nullptr);
    g_return_if_fail(error == nullptr || *error == nullptr);

    try {
        Exiv2::ExifData& exif_data = self->priv->image->exifData();
        Exiv2::XmpData& xmp_data = self->priv->image->xmpData();

        exif_data["Exif.Photo.PixelXDimension"] = static_cast<uint32_t>(width);
        exif_data["Exif.Image.ImageWidth"] = static_cast<uint32_t>(width);
        xmp_data["Xmp.tiff.ImageWidth"] = static_cast<uint32_t>(width);
        xmp_data["Xmp.exif.PixelXDimension"] = static_cast<uint32_t>(width);
    } catch (Exiv2::Error& e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());
    }
}

void gexiv2_metadata_set_metadata_pixel_height (GExiv2Metadata *self, gint height) {
    GError* error = nullptr;

    gexiv2_metadata_try_set_metadata_pixel_height(self, height, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }
}

void gexiv2_metadata_try_set_metadata_pixel_height(GExiv2Metadata* self, gint height, GError** error) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv->image.get() != NULL);
    g_return_if_fail(error == nullptr || *error == nullptr);

    try {
        Exiv2::ExifData& exif_data = self->priv->image->exifData();
        Exiv2::XmpData& xmp_data = self->priv->image->xmpData();

        exif_data["Exif.Photo.PixelYDimension"] = static_cast<uint32_t>(height);
        exif_data["Exif.Image.ImageLength"] = static_cast<uint32_t>(height);
        xmp_data["Xmp.tiff.ImageLength"] = static_cast<uint32_t>(height);
        xmp_data["Xmp.exif.PixelYDimension"] = static_cast<uint32_t>(height);
    } catch (Exiv2::Error& e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());
    }
}

gchar* gexiv2_metadata_get_comment (GExiv2Metadata *self) {
    GError* error = nullptr;
    gchar* value = nullptr;

    value = gexiv2_metadata_try_get_comment(self, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gchar* gexiv2_metadata_try_get_comment(GExiv2Metadata* self, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(self->priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    gchar* str = self->priv->comment;
    if (str != nullptr && *str != '\0')
        return g_strdup(str);

    str = gexiv2_metadata_get_exif_tag_interpreted_string(self, "Exif.Image.ImageDescription", error);
    if (error && *error) {
        return nullptr;
    }

    if (str != nullptr && *str != '\0')
        return str;
    else
        g_free (str);

    str = gexiv2_metadata_get_exif_tag_interpreted_string(self, "Exif.Photo.UserComment", error);
    if (error && *error) {
        return nullptr;
    }

    if (str != nullptr && *str != '\0')
        return str;
    else
        g_free (str);

    str = gexiv2_metadata_get_exif_tag_interpreted_string(self, "Exif.Image.XPComment", error);
    if (error && *error) {
        return nullptr;
    }

    if (str != nullptr && *str != '\0')
        return str;
    else
        g_free (str);

    str = gexiv2_metadata_get_iptc_tag_interpreted_string(self, "Iptc.Application2.Caption", error);
    if (error && *error) {
        return nullptr;
    }

    if (str != nullptr && *str != '\0')
        return str;
    else
        g_free (str);

    str = gexiv2_metadata_get_xmp_tag_interpreted_string(self, "Xmp.dc.description", error);
    if (error && *error) {
        return nullptr;
    }

    if (str != nullptr && *str != '\0')
        return str;
    else
        g_free (str);

    str = gexiv2_metadata_get_xmp_tag_interpreted_string(self, "Xmp.acdsee.notes", error);
    if (error && *error) {
        return nullptr;
    }

    if (str != nullptr && *str != '\0')
        return str;
    else
        g_free (str);

    return nullptr;
}

void gexiv2_metadata_set_comment (GExiv2Metadata *self, const gchar* comment) {
    GError* error = nullptr;

    gexiv2_metadata_try_set_comment(self, comment, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }
}

void gexiv2_metadata_try_set_comment(GExiv2Metadata* self, const gchar* comment, GError** error) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv->image.get() != nullptr);
    g_return_if_fail(comment != nullptr);
    g_return_if_fail(error == nullptr || *error == nullptr);

    try {
        Exiv2::ExifData& exif_data = self->priv->image->exifData();
        Exiv2::IptcData& iptc_data = self->priv->image->iptcData();
        Exiv2::XmpData& xmp_data = self->priv->image->xmpData();

        gexiv2_metadata_set_comment_internal(self, comment);

        exif_data["Exif.Image.ImageDescription"] = comment;
        exif_data["Exif.Photo.UserComment"] = comment;
        exif_data["Exif.Image.XPComment"] = comment;
        iptc_data["Iptc.Application2.Caption"] = comment;
        xmp_data["Xmp.dc.description"] = comment;
        /* Do not need to write to acdsee properties, just read from them */
        // xmp_data ["Xmp.acdsee.notes"] = comment;
    } catch (Exiv2::Error& e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());
    }
}

void gexiv2_metadata_clear_comment (GExiv2Metadata *self) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv->image.get() != nullptr);

    /* don't delete the comment field, merely empty it */
    gexiv2_metadata_set_comment_internal (self, "");
}

gboolean gexiv2_metadata_is_exif_tag(const gchar* tag) {
    g_return_val_if_fail(tag != nullptr, FALSE);

    return strncmp("Exif.", tag, 4) == 0;
}

gboolean gexiv2_metadata_is_xmp_tag(const gchar* tag) {
    g_return_val_if_fail(tag != nullptr, FALSE);

    return strncmp("Xmp.", tag, 4) == 0;
}

gboolean gexiv2_metadata_is_iptc_tag(const gchar* tag) {
    g_return_val_if_fail(tag != nullptr, FALSE);

    return strncmp("Iptc.", tag, 5) == 0;
}

gchar* gexiv2_metadata_try_get_tag_string (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(self->priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_string (self, tag, error);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_string (self, tag, error);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_string (self, tag, error);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return nullptr;
}

gboolean gexiv2_metadata_try_set_tag_string (GExiv2Metadata *self, const gchar* tag, const gchar* value, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_set_xmp_tag_string(self, tag, value, error);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_set_exif_tag_string(self, tag, value, error);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_set_iptc_tag_string(self, tag, value, error);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return FALSE;
}

gchar* gexiv2_metadata_get_tag_string (GExiv2Metadata *self, const gchar* tag) {
    gchar  *value;
    GError *error = nullptr;

    g_return_val_if_fail(GEXIV2_IS_METADATA (self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(self->priv->image.get() != nullptr, nullptr);

    value = gexiv2_metadata_try_get_tag_string (self, tag, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gboolean gexiv2_metadata_set_tag_string (GExiv2Metadata *self, const gchar* tag, const gchar* value) {
    gboolean  success;
    GError   *error = nullptr;

    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);

    success = gexiv2_metadata_try_set_tag_string(self, tag, value, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return success;
}

gchar* gexiv2_metadata_try_get_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(self->priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_interpreted_string(self, tag, error);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_interpreted_string(self, tag, error);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_interpreted_string(self, tag, error);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return nullptr;
}

gchar* gexiv2_metadata_get_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag) {
    gchar  *value;
    GError *error = nullptr;

    g_return_val_if_fail(GEXIV2_IS_METADATA (self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(self->priv->image.get() != nullptr, nullptr);

    value = gexiv2_metadata_try_get_tag_interpreted_string(self, tag, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gchar** gexiv2_metadata_try_get_tag_multiple(GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(self->priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_multiple(self, tag, error);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_multiple(self, tag, error);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_multiple(self, tag, error);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return nullptr;
}

gboolean gexiv2_metadata_try_set_tag_multiple(GExiv2Metadata *self, const gchar* tag, const gchar** values, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(values != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_set_xmp_tag_multiple(self, tag, values, error);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_set_exif_tag_multiple(self, tag, values, error);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_set_iptc_tag_multiple(self, tag, values, error);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return FALSE;
}

gchar** gexiv2_metadata_get_tag_multiple(GExiv2Metadata* self, const gchar* tag) {
    gchar** tags = nullptr;
    GError* error = nullptr;

    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(self->priv != nullptr, nullptr);
    g_return_val_if_fail(self->priv->image.get() != nullptr, nullptr);

    if (gexiv2_metadata_is_xmp_tag(tag)) {
        tags = gexiv2_metadata_get_xmp_tag_multiple_deprecated(self, tag, &error);
        if (error) {
            g_warning("%s", error->message);
            g_clear_error(&error);
        }
        return tags;
    }

    if (gexiv2_metadata_is_exif_tag(tag)) {
        tags = gexiv2_metadata_get_exif_tag_multiple(self, tag, &error);
        if (error) {
            g_warning("%s", error->message);
            g_clear_error(&error);
        }
        return tags;
    }

    if (gexiv2_metadata_is_iptc_tag(tag)) {
        tags = gexiv2_metadata_get_iptc_tag_multiple(self, tag, &error);
        if (error) {
            g_warning("%s", error->message);
            g_clear_error(&error);
        }
        return tags;
    }

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(&error, g_quark_from_string("GExiv2"), e.code(), e.what());
    g_warning("%s", error->message);
    g_clear_error(&error);

    return tags;
}

gboolean gexiv2_metadata_set_tag_multiple(GExiv2Metadata *self, const gchar* tag, const gchar** values) {
    GError   *error   = nullptr;
    gboolean  success = FALSE;

    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(values != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);

    success = gexiv2_metadata_try_set_tag_multiple(self, tag, values, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return success;
}

glong gexiv2_metadata_try_get_tag_long(GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), 0);
    g_return_val_if_fail(tag != nullptr, 0);
    g_return_val_if_fail(self->priv->image.get() != nullptr, 0);
    g_return_val_if_fail(error == nullptr || *error == nullptr, 0);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_long(self, tag, error);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_long(self, tag, error);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return 0;
}

glong gexiv2_metadata_get_tag_long(GExiv2Metadata *self, const gchar* tag) {
    GError *error   = nullptr;
    glong   value;

    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);

    value = gexiv2_metadata_try_get_tag_long(self, tag, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gboolean gexiv2_metadata_try_set_tag_long(GExiv2Metadata *self, const gchar* tag, glong value, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, 0);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_set_xmp_tag_long(self, tag, value, error);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_set_exif_tag_long(self, tag, value, error);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return FALSE;
}

gboolean gexiv2_metadata_set_tag_long(GExiv2Metadata *self, const gchar* tag, glong value) {
    GError   *error   = nullptr;
    gboolean  success = FALSE;

    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);

    success = gexiv2_metadata_try_set_tag_long(self, tag, value, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return success;
}

gboolean gexiv2_metadata_get_exposure_time (GExiv2Metadata *self, gint *nom, gint *den) {
    GError* error = nullptr;
    gboolean value = FALSE;

    value = gexiv2_metadata_try_get_exposure_time(self, nom, den, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gboolean gexiv2_metadata_try_get_exposure_time(GExiv2Metadata* self, gint* nom, gint* den, GError** error) {
    return gexiv2_metadata_try_get_exif_tag_rational(self, "Exif.Photo.ExposureTime", nom, den, error);
}

gdouble gexiv2_metadata_get_fnumber (GExiv2Metadata *self) {
    GError* error = nullptr;
    gdouble value = -1.0;

    value = gexiv2_metadata_try_get_fnumber(self, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gdouble gexiv2_metadata_try_get_fnumber(GExiv2Metadata* self, GError** error) {
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    gdouble fnumber = gexiv2_metadata_get_exif_tag_rational_as_double(self, "Exif.Photo.FNumber", -1.0, error);
    if (error && *error)
        return -1.0;

    if (fnumber == -1.0) {
        gdouble aperture_value =
            gexiv2_metadata_get_exif_tag_rational_as_double(self, "Exif.Photo.ApertureValue", -1.0, error);
        if (error && *error)
            return -1.0;

        if (aperture_value == -1.0)
          return fnumber;

        fnumber = pow (2.0, aperture_value / 2.0);
    }

    return fnumber;
}

gdouble gexiv2_metadata_get_focal_length (GExiv2Metadata *self) {
    GError* error = nullptr;
    gdouble value = -1.0;

    value = gexiv2_metadata_try_get_focal_length(self, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gdouble gexiv2_metadata_try_get_focal_length(GExiv2Metadata* self, GError** error) {
    return gexiv2_metadata_get_exif_tag_rational_as_double(self, "Exif.Photo.FocalLength", -1.0, error);
}

gint gexiv2_metadata_get_iso_speed (GExiv2Metadata *self) {
    GError* error = nullptr;
    gdouble value = -1.0;

    value = gexiv2_metadata_try_get_iso_speed(self, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gint gexiv2_metadata_try_get_iso_speed(GExiv2Metadata* self, GError** error) {
    return (gint) gexiv2_metadata_get_exif_tag_long(self, "Exif.Photo.ISOSpeedRatings", error);
}

GExiv2PreviewProperties** gexiv2_metadata_get_preview_properties (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(self->priv != nullptr, nullptr);
    g_return_val_if_fail(self->priv->image.get() != nullptr, nullptr);

    return self->priv->preview_properties;
}

GExiv2PreviewImage* gexiv2_metadata_get_preview_image (GExiv2Metadata *self,
    GExiv2PreviewProperties *props) {

    GExiv2PreviewImage* value = nullptr;
    GError* error = nullptr;

    value = gexiv2_metadata_try_get_preview_image(self, props, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

GExiv2PreviewImage* gexiv2_metadata_try_get_preview_image(GExiv2Metadata* self,
                                                          GExiv2PreviewProperties* props,
                                                          GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(GEXIV2_IS_PREVIEW_PROPERTIES(props), nullptr);
    g_return_val_if_fail(self->priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    return gexiv2_preview_image_new(self->priv->preview_manager, *props->priv->props, error);
}

gboolean gexiv2_metadata_get_exif_thumbnail (GExiv2Metadata *self, guint8** buffer, gint *size) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(buffer != nullptr, FALSE);
    g_return_val_if_fail(size != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);

    Exiv2::ExifThumb thumb = Exiv2::ExifThumb(self->priv->image->exifData());
    Exiv2::DataBuf data = thumb.copy();
    if (data.pData_ == nullptr)
        return FALSE;
    
    *buffer = (guint8*) g_malloc(data.size_);
    memcpy(*buffer, data.pData_, data.size_);
    *size = data.size_;

    return TRUE;
}

gboolean gexiv2_metadata_set_exif_thumbnail_from_file(GExiv2Metadata* self, const gchar* path, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(path != nullptr && g_utf8_strlen(path, -1) > 0, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);

    try {
        Exiv2::ExifThumb thumb = Exiv2::ExifThumb(self->priv->image->exifData());
        thumb.setJpegThumbnail(std::string(path));
        
        return TRUE;
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
    
    return FALSE;
}

void gexiv2_metadata_set_exif_thumbnail_from_buffer (GExiv2Metadata *self, const guint8 *buffer,
    gint size) {
    GError* error = nullptr;

    gexiv2_metadata_try_set_exif_thumbnail_from_buffer(self, buffer, size, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }
}

void gexiv2_metadata_try_set_exif_thumbnail_from_buffer(GExiv2Metadata* self,
                                                        const guint8* buffer,
                                                        gint size,
                                                        GError** error) {
    g_return_if_fail(GEXIV2_IS_METADATA(self));
    g_return_if_fail(buffer != nullptr);
    g_return_if_fail(size > 0);
    g_return_if_fail(self->priv->image.get() != nullptr);
    g_return_if_fail(error == nullptr || *error == nullptr);

    try {
        Exiv2::ExifThumb thumb = Exiv2::ExifThumb(self->priv->image->exifData());
        thumb.setJpegThumbnail(buffer, size);
    } catch (Exiv2::Error& e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());
    }
}

void gexiv2_metadata_erase_exif_thumbnail (GExiv2Metadata *self) {
    GError* error = nullptr;

    gexiv2_metadata_try_erase_exif_thumbnail(self, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }
}

void gexiv2_metadata_try_erase_exif_thumbnail(GExiv2Metadata* self, GError** error) {
    g_return_if_fail(GEXIV2_IS_METADATA(self));
    g_return_if_fail(self->priv->image.get() != nullptr);
    g_return_if_fail(error == nullptr || *error == nullptr);

    try {
        Exiv2::ExifThumb thumb = Exiv2::ExifThumb(self->priv->image->exifData());
        thumb.erase();
    } catch (Exiv2::Error& e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());
    }
}

const gchar* gexiv2_metadata_try_get_tag_label (const gchar *tag, GError **error) {
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_label(tag, error);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_label(tag, error);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_label(tag, error);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return nullptr;
}

const gchar* gexiv2_metadata_get_tag_label (const gchar *tag) {
    const gchar *value;
    GError      *error = nullptr;

    g_return_val_if_fail(tag != nullptr, nullptr);

    value = gexiv2_metadata_try_get_tag_label(tag, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

const gchar* gexiv2_metadata_try_get_tag_description (const gchar *tag, GError **error) {
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_description(tag, error);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_description(tag, error);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_description(tag, error);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return nullptr;
}

const gchar* gexiv2_metadata_get_tag_description (const gchar *tag) {
    const gchar *value;
    GError      *error = nullptr;

    g_return_val_if_fail(tag != nullptr, nullptr);

    value = gexiv2_metadata_try_get_tag_description(tag, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

const gchar* gexiv2_metadata_try_get_tag_type (const gchar *tag, GError **error) {
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_type(tag, error);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_type(tag, error);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_type(tag, error);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return nullptr;
}

const gchar* gexiv2_metadata_get_tag_type (const gchar *tag) {
    const gchar *value;
    GError      *error = nullptr;

    g_return_val_if_fail(tag != nullptr, nullptr);

    value = gexiv2_metadata_try_get_tag_type(tag, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

gboolean gexiv2_metadata_try_tag_supports_multiple_values(GExiv2Metadata* self, const gchar* tag, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(self->priv != nullptr, FALSE);
    g_return_val_if_fail(self->priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    if (gexiv2_metadata_is_iptc_tag(tag) == TRUE)
        return gexiv2_metadata_iptc_tag_supports_multiple_values(tag, error);

    if (gexiv2_metadata_is_xmp_tag(tag) == TRUE)
        return gexiv2_metadata_xmp_tag_supports_multiple_values(self, tag, error);

    if (gexiv2_metadata_is_exif_tag(tag) == TRUE)
        return gexiv2_metadata_exif_tag_supports_multiple_values(tag, error);

    // Invalid tag (Family name)
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return FALSE;
}

GBytes* gexiv2_metadata_try_get_tag_raw(GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(self->priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_raw(self, tag, error);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_raw(self, tag, error);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_raw(self, tag, error);

    // Invalid "familyName"
    Exiv2::Error e(Exiv2::ErrorCode::kerInvalidKey, tag);
    g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());

    return nullptr;
}

GBytes* gexiv2_metadata_get_tag_raw(GExiv2Metadata *self, const gchar* tag) {
    GBytes *value;
    GError *error = nullptr;

    g_return_val_if_fail(GEXIV2_IS_METADATA (self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(self->priv->image.get() != nullptr, nullptr);

    value = gexiv2_metadata_try_get_tag_raw(self, tag, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

G_END_DECLS
