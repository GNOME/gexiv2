/*
 * gexiv2-metadata.cpp
 *
 * Author(s)
 *  Mike Gemuende <mike@gemuende.de>
 *  Jim Nelson <jim@yorba.org>
 *
 * This is free software. See COPYING for details.
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

#include <exiv2/exiv2.hpp>

namespace {
class GioIo : public Exiv2::BasicIo {
public:
    GioIo (GInputStream *is)
        : BasicIo ()
        , _is (G_INPUT_STREAM (g_object_ref (is)))
        , _seekable(G_IS_SEEKABLE(_is) ? G_SEEKABLE (_is) : NULL) {}

    ~GioIo() { g_clear_object (&_is); _seekable = NULL;}
#if defined(_MSC_VER)
    typedef int64_t seek_offset_t;
#else
    typedef long seek_offset_t;
#endif

    int open() { return 0; }
    int close() { return 0; }
    long write(const Exiv2::byte *data, long wcount) { return 0; }
    long write(BasicIo &src) { return 0; }
    int putb(Exiv2::byte data) { return EOF; }

    Exiv2::DataBuf read(long rcount) {
        Exiv2::DataBuf b{rcount};

        long bytes_read = this->read(b.pData_, rcount);
        if (bytes_read > 0 && bytes_read != rcount) {
            b.reset(std::pair<Exiv2::byte*, long>(b.pData_, bytes_read));
        }

        return b;
    }

    long read(Exiv2::byte *buf, long rcount) {
        GError *error = NULL;
        gssize result = 0;

        result = g_input_stream_read(_is, reinterpret_cast<void *>(buf), rcount, NULL, &error);
        if (error != NULL) {
            throw Exiv2::Error(2);
            return 0;
        }

        return result;
    }

    int getb() {
        Exiv2::byte b;
        return this->read (&b, 1) == 1 ? b : EOF;
    }

    void transfer(Exiv2::BasicIo &src) {
        // TODO: Implement
    }

    int seek(seek_offset_t offset, Exiv2::BasicIo::Position position) {
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
                g_critical ("Failed to seek: %s", error->message);

                g_error_free (error);

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
                g_critical("Failed to seek forward: %s", error->message);

                g_error_free(error);

                return -1;
            }

            return 0;
        }
    }

    Exiv2::byte *mmap(bool writable) {
        return NULL;
    }

    int munmap() {
        return 0;
    }

    long tell() const {
        return -1;
    }

    size_t size() const {
        return -1;
    }

    bool isopen() const {
        return true;
    }

    int error() const {
        return 0;
    }

    bool eof() const {
        return false;
    }

    std::string path() const {
        return "GIO Wrapper";
    }

    Exiv2::BasicIo::AutoPtr temporary() const {
        return Exiv2::BasicIo::AutoPtr(new Exiv2::FileIo("/tmp/exiv2-temp"));
    }


private:
    GInputStream *_is;
    GSeekable *_seekable;


}; // class GioIo

class RwGioIo : public Exiv2::BasicIo {
public:
    RwGioIo (GIOStream *is)
        : BasicIo ()
        , _ios (G_IO_STREAM (g_object_ref (is)))
        , _seekable(G_IS_SEEKABLE(_ios) ? G_SEEKABLE (_ios) : NULL) {}
    ~RwGioIo() { g_clear_object (&_ios);}
#if defined(_MSC_VER)
    typedef int64_t seek_offset_t;
#else
    typedef long seek_offset_t;
#endif

    int open() {
        g_print ("=> Open\n");
        return 0;
    }
    int close() {
        g_print ("=> Close\n");
        return 0;
    }

    long write(const Exiv2::byte *data, long wcount) {
        g_print("=> write %p %ld\n", data, wcount);
        return 0;
    }

    long write(BasicIo &src) {
        g_print("=> write basicio\n");
        return 0; 
    }

    int putb(Exiv2::byte data) {
        g_print("=> putb\n");
        return EOF;
    }

    Exiv2::DataBuf read(long rcount) {
        g_print("=> read\n");
        return Exiv2::DataBuf();
    }

    long read(Exiv2::byte *buf, long rcount) {
        GInputStream *is = g_io_stream_get_input_stream (_ios);
        GError *error = NULL;
        gssize result = 0;

        result = g_input_stream_read(is, reinterpret_cast<void *>(buf), rcount, NULL, &error);
        if (error != NULL) {
            throw Exiv2::Error(2);
            return 0;
        }

        return result;
    }

    int getb() {
        g_print("=> getb\n");
        Exiv2::byte b;
        return this->read (&b, 1) == 1 ? b : EOF;
    }

    void transfer(Exiv2::BasicIo &src) {
        g_print("=> Transfer\n");
    }

    int seek(seek_offset_t offset, Exiv2::BasicIo::Position position) {
        g_print("seek %lld %d\n", offset, (int)position);
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
                g_critical ("Failed to seek: %s", error->message);

                g_error_free (error);

                return -1;
            }

            return 0;
        } else {
            return -1;
        }
    }

    Exiv2::byte *mmap(bool writable) {
        g_print("mmap: %d\n", (int) writable);
        return NULL;
    }

    int munmap() {
        g_print("munmap\n");
        return 0;
    }

    long tell() const {
        g_print("tell\n");
        return -1;
    }

    size_t size() const {
        g_print("size\n");
        return -1;
    }

    bool isopen() const {
        g_print("isopen\n");
        return true;
    }

    int error() const {
        g_print("error\n");
        return 0;
    }

    bool eof() const {
        g_print("eof\n");
        return false;
    }

    std::string path() const {
        g_print ("path\n");
        return "GIO Wrapper";
    }

    Exiv2::BasicIo::AutoPtr temporary() const {
        g_print ("temporary\n");
        return Exiv2::BasicIo::AutoPtr(new Exiv2::FileIo("/tmp/exiv2-temp"));
    }


private:
    GIOStream *_ios;
    GSeekable *_seekable;
};
} // Anonymous namespace

G_BEGIN_DECLS

G_DEFINE_TYPE_WITH_CODE (GExiv2Metadata, gexiv2_metadata, G_TYPE_OBJECT, G_ADD_PRIVATE (GExiv2Metadata));

static void gexiv2_metadata_finalize (GObject *object);
static void gexiv2_metadata_set_comment_internal (GExiv2Metadata *self, const gchar *new_comment);

static gboolean gexiv2_metadata_open_internal (GExiv2Metadata *self, GError **error);
static gboolean gexiv2_metadata_save_internal (GExiv2Metadata *self, Exiv2::Image::AutoPtr image, GError **error);

static void gexiv2_metadata_init (GExiv2Metadata *self) {
    GExiv2MetadataPrivate *priv;
    self->priv = priv = (GExiv2MetadataPrivate *) gexiv2_metadata_get_instance_private(self);
    
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
    
    g_free (self->priv->comment);
    
    self->priv->comment = g_strdup (new_comment);
}

static void gexiv2_metadata_init_internal (GExiv2Metadata *self) {
    GExiv2MetadataPrivate* priv = self->priv;
    
    // must be set prior to calling this function
    g_return_if_fail(priv->image.get() != NULL);
    
    gexiv2_metadata_set_comment_internal (self, priv->image->comment ().c_str ());
    priv->mime_type = g_strdup (priv->image->mimeType ().c_str ());
    
    priv->pixel_width = priv->image->pixelWidth ();
    priv->pixel_height = priv->image->pixelHeight ();
    
    Exiv2::AccessMode mode = priv->image->checkMode (Exiv2::mdExif);
    priv->supports_exif = (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
    
    mode = priv->image->checkMode (Exiv2::mdXmp);
    priv->supports_xmp = (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
    
    mode = priv->image->checkMode (Exiv2::mdIptc);
    priv->supports_iptc = (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
    
    priv->preview_manager = new Exiv2::PreviewManager(*priv->image.get());
    
    Exiv2::PreviewPropertiesList list = priv->preview_manager->getPreviewProperties();
    int count = list.size();
    if (count > 0) {
        priv->preview_properties = g_new(GExiv2PreviewProperties *, count + 1);
        for (int ctr = 0; ctr < count; ctr++)
            priv->preview_properties[ctr] = gexiv2_preview_properties_new(list[ctr]);
        priv->preview_properties[count] = NULL;
    }
}

static gboolean gexiv2_metadata_open_internal (GExiv2Metadata* self, GError** error) {
    if (self->priv->image.get () == NULL || ! self->priv->image->good ()) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"),
            501, "unsupported format");
        return FALSE;
    }
    
    self->priv->image->readMetadata ();
    gexiv2_metadata_init_internal(self);
    
    return TRUE;
}

gboolean gexiv2_metadata_open_path (GExiv2Metadata *self, const gchar *path, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    
    try {
        Exiv2::BasicIo::AutoPtr file (new Exiv2::FileIo (path));
        self->priv->image = Exiv2::ImageFactory::open (file);
        
        return gexiv2_metadata_open_internal (self, error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
    
    return FALSE;
}

gboolean gexiv2_metadata_open_buf (GExiv2Metadata *self, const guint8 *data, glong n_data,
    GError **error) {
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
        Exiv2::BasicIo::AutoPtr stream_ptr (new StreamIo (cb));
        self->priv->image = Exiv2::ImageFactory::open (stream_ptr);
        
        return gexiv2_metadata_open_internal (self, error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
    
    return FALSE;
}

gboolean gexiv2_metadata_from_stream(GExiv2Metadata *self, GInputStream *stream, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);

    try {
        Exiv2::BasicIo::AutoPtr gio_ptr (new GioIo (stream));
        self->priv->image = Exiv2::ImageFactory::open (gio_ptr);

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
gboolean gexiv2_metadata_from_app1_segment(GExiv2Metadata *self, const guint8 *data, glong n_data,
    GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(data != NULL, FALSE);
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
        if (self->priv->image.get() == NULL)
            return FALSE;
        
        Exiv2::ExifParser::decode(self->priv->image->exifData(), data + offset, n_data - offset);
        gexiv2_metadata_init_internal(self);
        
        return TRUE;
    } catch (Exiv2::Error &e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), e.code(), e.what());
        
        return FALSE;
    }
}

static gboolean gexiv2_metadata_save_internal (GExiv2Metadata *self, Exiv2::Image::AutoPtr image,
    GError **error) {
    if (image.get () == NULL || ! image->good ()) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"),
            501, "format seems not to be supported");
        
        return FALSE;
    }
    
    image->readMetadata ();
        
    Exiv2::AccessMode mode = image->checkMode (Exiv2::mdExif);
    if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite) {
        /* For tiff some image data is stored in exif. This should not
           be overwritten. (see libkexiv2/kexiv2.cpp)
         */
        if (image->mimeType () == "image/tiff") {
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
            
            image->setExifData (self->priv->image->exifData());
        } else {
            image->setExifData (self->priv->image->exifData());
        }
    }
        
    mode = image->checkMode (Exiv2::mdXmp);
    if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
        image->setXmpData (self->priv->image->xmpData());
    
    mode = image->checkMode (Exiv2::mdIptc);
    if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
        image->setIptcData (self->priv->image->iptcData());
    
    mode = image->checkMode (Exiv2::mdComment);
    if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
        image->setComment (self->priv->comment);
    
    image->writeMetadata ();
    
    return TRUE;
}

gboolean gexiv2_metadata_save_external (GExiv2Metadata *self, const gchar *path, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);

    try {
        return gexiv2_metadata_save_internal (self, Exiv2::ImageFactory::create(Exiv2::ImageType::xmp, path), error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }

    return FALSE;
}

gboolean gexiv2_metadata_save_file (GExiv2Metadata *self, const gchar *path, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    
    try {
        return gexiv2_metadata_save_internal (self, Exiv2::ImageFactory::open (path), error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
    
    return FALSE;
}

gboolean gexiv2_metadata_save_stream (GExiv2Metadata *self, ManagedStreamCallbacks* cb, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    
    try {
        Exiv2::BasicIo::AutoPtr stream_ptr (new StreamIo (cb));
        
        return gexiv2_metadata_save_internal (self, Exiv2::ImageFactory::open (stream_ptr), error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
    
    return FALSE;
}

gboolean gexiv2_metadata_to_stream (GExiv2Metadata *self, GIOStream* cb, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    
    try {
        Exiv2::BasicIo::AutoPtr stream_ptr (new RwGioIo (cb));
        
        return gexiv2_metadata_save_internal (self, Exiv2::ImageFactory::open (stream_ptr), error);
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
    
    return FALSE;
}



gboolean gexiv2_metadata_has_tag(GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != NULL, FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, FALSE);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_has_xmp_tag(self, tag);
    
    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_has_exif_tag(self, tag);
    
    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_has_iptc_tag(self, tag);
    
    return FALSE;
}

gboolean gexiv2_metadata_clear_tag(GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != NULL, FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, FALSE);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_clear_xmp_tag(self, tag);
    
    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_clear_exif_tag(self, tag);
    
    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_clear_iptc_tag(self, tag);
    
    return FALSE;
}

void gexiv2_metadata_clear (GExiv2Metadata *self) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    
    if (self->priv->image.get() == NULL)
        return;
    
    gexiv2_metadata_clear_exif (self);
    gexiv2_metadata_clear_xmp (self);
    gexiv2_metadata_clear_iptc (self);
    gexiv2_metadata_clear_comment (self);
    
    self->priv->image->clearMetadata();
}

const gchar* gexiv2_metadata_get_mime_type (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), NULL);
    
    return self->priv->mime_type;
}

GExiv2Orientation gexiv2_metadata_get_orientation (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), GEXIV2_ORIENTATION_UNSPECIFIED);
    g_return_val_if_fail(self->priv->image.get() != NULL, GEXIV2_ORIENTATION_UNSPECIFIED);
    
    if (gexiv2_metadata_has_exif(self)) {
        // Because some camera set a wrong standard exif orientation tag,
        // We need to check makernote tags first!
        if (gexiv2_metadata_has_exif_tag(self, "Exif.MinoltaCs7D.Rotation")) {
            long orientation = gexiv2_metadata_get_exif_tag_long(self, "Exif.MinoltaCs7D.Rotation");
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
            long orientation = gexiv2_metadata_get_exif_tag_long(self, "Exif.MinoltaCs5D.Rotation");
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
        
        GExiv2Orientation orientation = (GExiv2Orientation) gexiv2_metadata_get_exif_tag_long(self,
            "Exif.Image.Orientation");
        if (orientation >= GEXIV2_ORIENTATION_MIN && orientation <= GEXIV2_ORIENTATION_MAX)
            return orientation;
    }
    
    GExiv2Orientation orientation = GEXIV2_ORIENTATION_UNSPECIFIED;
    if (gexiv2_metadata_has_xmp_tag(self, "Xmp.tiff.ImageWidth")) {
        orientation = (GExiv2Orientation) gexiv2_metadata_get_xmp_tag_long(self,
            "Xmp.tiff.ImageWidth");
    }
    
    return (orientation < GEXIV2_ORIENTATION_MIN || orientation > GEXIV2_ORIENTATION_MAX)
        ? GEXIV2_ORIENTATION_UNSPECIFIED : orientation;
}

gboolean gexiv2_metadata_get_supports_exif (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, FALSE);
    
    return self->priv->supports_exif;
}

gboolean gexiv2_metadata_get_supports_xmp (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, FALSE);
    
    return self->priv->supports_xmp;
}

gboolean gexiv2_metadata_get_supports_iptc (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, FALSE);
    
    return self->priv->supports_iptc;
}

void gexiv2_metadata_set_orientation (GExiv2Metadata *self, GExiv2Orientation orientation) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv->image.get() != NULL);
    g_return_if_fail(orientation <= GEXIV2_ORIENTATION_ROT_270);
    g_return_if_fail(orientation >= GEXIV2_ORIENTATION_UNSPECIFIED);
    
    Exiv2::ExifData& exif_data = self->priv->image->exifData();
    Exiv2::XmpData& xmp_data = self->priv->image->xmpData();
    
    exif_data["Exif.Image.Orientation"] = static_cast<uint16_t> (orientation);
    xmp_data["Xmp.tiff.Orientation"] = static_cast<uint16_t> (orientation);
    
    gexiv2_metadata_clear_exif_tag(self, "Exif.MinoltaCs7D.Rotation");
    gexiv2_metadata_clear_exif_tag(self, "Exif.MinoltaCs5D.Rotation");
}

gint gexiv2_metadata_get_pixel_width (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), -1);
    g_return_val_if_fail(self->priv->image.get() != NULL, -1);
    
    return self->priv->pixel_width;
}

gint gexiv2_metadata_get_pixel_height (GExiv2Metadata *self) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), -1);
    g_return_val_if_fail(self->priv->image.get() != NULL, -1);
    
    return self->priv->pixel_height;
}

gint gexiv2_metadata_get_metadata_pixel_width (GExiv2Metadata *self) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), -1);
    g_return_val_if_fail(self->priv->image.get() != NULL, -1);
    
    if (gexiv2_metadata_has_exif(self)) {
        if (gexiv2_metadata_has_exif_tag(self, "Exif.Photo.PixelXDimension"))
            return gexiv2_metadata_get_exif_tag_long(self, "Exif.Photo.PixelXDimension");
        
        if (gexiv2_metadata_has_exif_tag(self, "Exif.Image.ImageWidth"))
            return gexiv2_metadata_get_exif_tag_long(self, "Exif.Image.ImageWidth");
    }
    
    if (gexiv2_metadata_has_xmp(self)) {
        if (gexiv2_metadata_has_xmp_tag(self, "Xmp.tiff.ImageWidth"))
            return gexiv2_metadata_get_xmp_tag_long(self, "Xmp.tiff.ImageWidth");
        
        if (gexiv2_metadata_has_xmp_tag(self, "Xmp.exif.PixelXDimension"))
            return gexiv2_metadata_get_xmp_tag_long(self, "Xmp.exif.PixelXDimension");
    }
    
    return -1;
}

gint gexiv2_metadata_get_metadata_pixel_height (GExiv2Metadata *self) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), -1);
    g_return_val_if_fail(self->priv->image.get() != NULL, -1);
    
    if (gexiv2_metadata_has_exif(self)) {
        if (gexiv2_metadata_has_exif_tag(self, "Exif.Photo.PixelYDimension"))
            return gexiv2_metadata_get_exif_tag_long(self, "Exif.Photo.PixelYDimension");
        
        if (gexiv2_metadata_has_exif_tag(self, "Exif.Image.ImageLength"))
            return gexiv2_metadata_get_exif_tag_long(self, "Exif.Image.ImageLength");
    }
    
    if (gexiv2_metadata_has_xmp(self)) {
        if (gexiv2_metadata_has_xmp_tag(self, "Xmp.tiff.ImageHeight"))
            return gexiv2_metadata_get_xmp_tag_long(self, "Xmp.tiff.ImageHeight");
            
        if (gexiv2_metadata_has_xmp_tag(self, "Xmp.exif.PixelYDimension"))
            return gexiv2_metadata_get_xmp_tag_long(self, "Xmp.exif.PixelYDimension");
    }
    
    return -1;
}

void gexiv2_metadata_set_metadata_pixel_width (GExiv2Metadata *self, gint width) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv->image.get() != NULL);
    
    Exiv2::ExifData& exif_data = self->priv->image->exifData();
    Exiv2::XmpData& xmp_data = self->priv->image->xmpData();
    
    exif_data["Exif.Photo.PixelXDimension"] = static_cast<uint32_t>(width);
    exif_data["Exif.Image.ImageWidth"] = static_cast<uint32_t>(width);
    xmp_data["Xmp.tiff.ImageWidth"] = static_cast<uint32_t>(width);
    xmp_data["Xmp.exif.PixelXDimension"] = static_cast<uint32_t>(width);
}

void gexiv2_metadata_set_metadata_pixel_height (GExiv2Metadata *self, gint height) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv->image.get() != NULL);
    
    Exiv2::ExifData& exif_data = self->priv->image->exifData();
    Exiv2::XmpData& xmp_data = self->priv->image->xmpData();
    
    exif_data ["Exif.Photo.PixelYDimension"] = static_cast<uint32_t>(height);
    exif_data ["Exif.Image.ImageLength"] = static_cast<uint32_t>(height);
    xmp_data ["Xmp.tiff.ImageLength"] = static_cast<uint32_t>(height);
    xmp_data ["Xmp.exif.PixelYDimension"] = static_cast<uint32_t>(height);
}

gchar* gexiv2_metadata_get_comment (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), NULL);
    g_return_val_if_fail(self->priv->image.get() != NULL, NULL);
    
    gchar* str = self->priv->comment;
    if (str != NULL && *str != '\0')
        return g_strdup(str);
    
    str = gexiv2_metadata_get_exif_tag_interpreted_string (self, "Exif.Image.ImageDescription");
    if (str != NULL && *str != '\0')
        return str;
    else
        g_free (str);
    
    str = gexiv2_metadata_get_exif_tag_interpreted_string (self, "Exif.Photo.UserComment");
    if (str != NULL && *str != '\0')
        return str;
    else
        g_free (str);
    
    str = gexiv2_metadata_get_exif_tag_interpreted_string (self, "Exif.Image.XPComment");
    if (str != NULL && *str != '\0')
        return str;
    else
        g_free (str);
    
    str = gexiv2_metadata_get_iptc_tag_interpreted_string (self, "Iptc.Application2.Caption");
    if (str != NULL && *str != '\0')
        return str;
    else
        g_free (str);
    
    str = gexiv2_metadata_get_xmp_tag_interpreted_string (self, "Xmp.dc.description");
    if (str != NULL && *str != '\0')
        return str;
    else
        g_free (str);
    
    str = gexiv2_metadata_get_xmp_tag_interpreted_string (self, "Xmp.acdsee.notes");
    if (str != NULL && *str != '\0')
        return str;
    else
        g_free (str);
    
    return NULL;
}

void gexiv2_metadata_set_comment (GExiv2Metadata *self, const gchar* comment) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv->image.get() != NULL);
    g_return_if_fail(comment != NULL);
    
    Exiv2::ExifData& exif_data = self->priv->image->exifData();
    Exiv2::IptcData& iptc_data = self->priv->image->iptcData();
    Exiv2::XmpData& xmp_data = self->priv->image->xmpData();
    
    gexiv2_metadata_set_comment_internal (self, comment);
    exif_data ["Exif.Image.ImageDescription"] = comment;
    exif_data ["Exif.Photo.UserComment"] = comment;
    exif_data ["Exif.Image.XPComment"] = comment;
    iptc_data ["Iptc.Application2.Caption"] = comment;
    xmp_data ["Xmp.dc.description"] = comment;
    /* Do not need to write to acdsee properties, just read from them */ 
    // xmp_data ["Xmp.acdsee.notes"] = comment;
}

void gexiv2_metadata_clear_comment (GExiv2Metadata *self) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv->image.get() != NULL);

    /* don't delete the comment field, merely empty it */
    gexiv2_metadata_set_comment_internal (self, "");
}

gboolean gexiv2_metadata_is_exif_tag(const gchar* tag) {
    g_return_val_if_fail(tag != NULL, FALSE);
    
    return g_ascii_strncasecmp("Exif.", tag, 5) == 0;
}

gboolean gexiv2_metadata_is_xmp_tag(const gchar* tag) {
    g_return_val_if_fail(tag != NULL, FALSE);
    
    return g_ascii_strncasecmp("Xmp.", tag, 4) == 0;
}

gboolean gexiv2_metadata_is_iptc_tag(const gchar* tag) {
    g_return_val_if_fail(tag != NULL, FALSE);
    
    return g_ascii_strncasecmp("Iptc.", tag, 5) == 0;
}

gchar* gexiv2_metadata_get_tag_string (GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), NULL);
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(self->priv->image.get() != NULL, NULL);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_string (self, tag);
        
    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_string (self, tag);
        
    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_string (self, tag);
    
    return NULL;
}

gboolean gexiv2_metadata_set_tag_string (GExiv2Metadata *self, const gchar* tag, const gchar* value) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != NULL, FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, FALSE);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_set_xmp_tag_string(self, tag, value);
    
    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_set_exif_tag_string(self, tag, value);
    
    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_set_iptc_tag_string(self, tag, value);
    
    return FALSE;
}

gchar* gexiv2_metadata_get_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), NULL);
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(self->priv->image.get() != NULL, NULL);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_interpreted_string (self, tag);
        
    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_interpreted_string (self, tag);
        
    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_interpreted_string (self, tag);
    
    return NULL;
}

gchar** gexiv2_metadata_get_tag_multiple(GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), NULL);
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(self->priv->image.get() != NULL, NULL);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_multiple(self, tag);
    
    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_multiple(self, tag);
    
    return NULL;
}

gboolean gexiv2_metadata_set_tag_multiple(GExiv2Metadata *self, const gchar* tag, const gchar** values) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != NULL, FALSE);
    g_return_val_if_fail(values != NULL, FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, FALSE);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_set_xmp_tag_multiple(self, tag, values);
    
    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_set_iptc_tag_multiple(self, tag, values);
    
    return FALSE;
}

glong gexiv2_metadata_get_tag_long(GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), 0);
    g_return_val_if_fail(tag != NULL, 0);
    g_return_val_if_fail(self->priv->image.get() != NULL, 0);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_long(self, tag);
    
    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_long(self, tag);
    
    return 0;
}

gboolean gexiv2_metadata_set_tag_long(GExiv2Metadata *self, const gchar* tag, glong value) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != NULL, FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, 0);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_set_xmp_tag_long(self, tag, value);
    
    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_set_exif_tag_long(self, tag, value);
    
    return FALSE;
}

gboolean gexiv2_metadata_get_exposure_time (GExiv2Metadata *self, gint *nom, gint *den) {
    return gexiv2_metadata_get_exif_tag_rational (self, "Exif.Photo.ExposureTime", nom, den);
}

gdouble gexiv2_metadata_get_fnumber (GExiv2Metadata *self) {
    gdouble fnumber = gexiv2_metadata_get_exif_tag_rational_as_double(self, "Exif.Photo.FNumber", -1.0);
    if (fnumber == -1.0) {
        gdouble aperture_value = gexiv2_metadata_get_exif_tag_rational_as_double(self,
                                                                                 "Exif.Photo.ApertureValue",
                                                                                 -1.0);
        if (aperture_value == -1.0)
          return fnumber;

        fnumber = pow (2.0, aperture_value / 2.0);
    }

    return fnumber;
}

gdouble gexiv2_metadata_get_focal_length (GExiv2Metadata *self) {
    return gexiv2_metadata_get_exif_tag_rational_as_double(self, "Exif.Photo.FocalLength", -1.0);
}

gint gexiv2_metadata_get_iso_speed (GExiv2Metadata *self) {
    return (gint) gexiv2_metadata_get_exif_tag_long (self, "Exif.Photo.ISOSpeedRatings");
}

GExiv2PreviewProperties** gexiv2_metadata_get_preview_properties (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), NULL);
    g_return_val_if_fail(self->priv->image.get() != NULL, NULL);
    
    return self->priv->preview_properties;
}

GExiv2PreviewImage* gexiv2_metadata_get_preview_image (GExiv2Metadata *self,
    GExiv2PreviewProperties *props) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), NULL);
    g_return_val_if_fail(GEXIV2_IS_PREVIEW_PROPERTIES(props), NULL);
    g_return_val_if_fail(self->priv->image.get() != NULL, NULL);
    
    return gexiv2_preview_image_new(self->priv->preview_manager, *props->priv->props);
}

gboolean gexiv2_metadata_get_exif_thumbnail (GExiv2Metadata *self, guint8** buffer, gint *size) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(buffer != NULL, FALSE);
    g_return_val_if_fail(size != NULL, FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, FALSE);
    
    Exiv2::ExifThumb thumb = Exiv2::ExifThumb(self->priv->image->exifData());
    Exiv2::DataBuf data = thumb.copy();
    if (data.pData_ == NULL)
        return FALSE;
    
    *buffer = (guint8*) g_malloc(data.size_);
    memcpy(*buffer, data.pData_, data.size_);
    *size = data.size_;
    
    return TRUE;
}

gboolean gexiv2_metadata_set_exif_thumbnail_from_file (GExiv2Metadata *self, const gchar *path,
    GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(path != NULL && g_utf8_strlen(path, -1) > 0, FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, FALSE);
    
    Exiv2::ExifThumb thumb = Exiv2::ExifThumb(self->priv->image->exifData());
    try {
        thumb.setJpegThumbnail(std::string(path));
        
        return TRUE;
    } catch (Exiv2::Error &e) {
        g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
    }
    
    return FALSE;
}

void gexiv2_metadata_set_exif_thumbnail_from_buffer (GExiv2Metadata *self, const guint8 *buffer,
    gint size) {
    g_return_if_fail(GEXIV2_IS_METADATA(self));
    g_return_if_fail(buffer != NULL);
    g_return_if_fail(size > 0);
    g_return_if_fail(self->priv->image.get() != NULL);
    
    Exiv2::ExifThumb thumb = Exiv2::ExifThumb(self->priv->image->exifData());
    thumb.setJpegThumbnail(buffer, size);
}

void gexiv2_metadata_erase_exif_thumbnail (GExiv2Metadata *self) {
    g_return_if_fail(GEXIV2_IS_METADATA(self));
    g_return_if_fail(self->priv->image.get() != NULL);
    
    Exiv2::ExifThumb thumb = Exiv2::ExifThumb(self->priv->image->exifData());
    thumb.erase();
}

const gchar* gexiv2_metadata_get_tag_label (const gchar *tag) {
    g_return_val_if_fail(tag != NULL, NULL);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_label(tag);
    
    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_label(tag);
    
    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_label(tag);
    
    return NULL;
}

const gchar* gexiv2_metadata_get_tag_description (const gchar *tag) {
    g_return_val_if_fail(tag != NULL, NULL);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_description(tag);
    
    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_description(tag);
    
    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_description(tag);
    
    return NULL;
}

const gchar* gexiv2_metadata_get_tag_type (const gchar *tag) {
    g_return_val_if_fail(tag != NULL, NULL);
    
    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_type(tag);
    
    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_type(tag);
    
    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_type(tag);
    
    return NULL;
}

GBytes* gexiv2_metadata_get_tag_raw(GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), NULL);
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(self->priv->image.get() != NULL, NULL);

    if (gexiv2_metadata_is_xmp_tag(tag))
        return gexiv2_metadata_get_xmp_tag_raw(self, tag);

    if (gexiv2_metadata_is_exif_tag(tag))
        return gexiv2_metadata_get_exif_tag_raw(self, tag);

    if (gexiv2_metadata_is_iptc_tag(tag))
        return gexiv2_metadata_get_iptc_tag_raw(self, tag);

    return NULL;
}

G_END_DECLS
