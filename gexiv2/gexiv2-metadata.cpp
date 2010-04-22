/*
 * gexiv2-metadata.cpp
 *
 * Author(s)
 * 	Mike Gemuende <mike@gemuende.de>
 *
 * This is free software. See COPYING for details.
 */

#include "gexiv2-metadata.h"
#include "gexiv2-metadata-private.h"
#include "gexiv2-stream-io.h"
#include "gexiv2-managed-stream.h"
#include <string>
#include <glib-object.h>
#include <gio/gio.h>
#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/iptc.hpp>
#include <exiv2/xmp.hpp>


G_BEGIN_DECLS
G_DEFINE_TYPE (GExiv2Metadata, gexiv2_metadata, G_TYPE_OBJECT);

static void gexiv2_metadata_finalize (GObject *object);
static void gexiv2_metadata_set_comment_internal (GExiv2Metadata *self, const gchar *new_comment);

static gboolean gexiv2_metadata_open_internal (GExiv2Metadata *self, Exiv2::Image::AutoPtr image, GError **error);
static gboolean gexiv2_metadata_save_internal (GExiv2Metadata *self, Exiv2::Image::AutoPtr image, GError **error);



static void
gexiv2_metadata_init (GExiv2Metadata *self)
{
	GExiv2MetadataPrivate *priv;
	self->priv = priv = GEXIV2_METADATA_GET_PRIVATE (self);

	/* Initialize members */
	self->priv->comment = NULL;
	self->priv->mime_type = NULL;
	self->priv->pixel_width = -1;
	self->priv->pixel_height = -1;
	
	/* the others are static members and need not to be initialized */
}


static void
gexiv2_metadata_class_init (GExiv2MetadataClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	
	g_type_class_add_private (klass, sizeof	(GExiv2MetadataPrivate));
	
	gobject_class->finalize = gexiv2_metadata_finalize;
}


static void
gexiv2_metadata_finalize (GObject *object)
{
	GExiv2Metadata *self = GEXIV2_METADATA (object);
	
	g_free (self->priv->comment);
	
	G_OBJECT_CLASS (gexiv2_metadata_parent_class)->finalize (object);
}


GExiv2Metadata*
gexiv2_metadata_new ()
{
	GExiv2Metadata *metadata;
	metadata = GEXIV2_METADATA (g_object_new (GEXIV2_TYPE_METADATA, NULL));
	
	return metadata;
}


static void
gexiv2_metadata_set_comment_internal (GExiv2Metadata *self, const gchar *new_comment)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	g_free (self->priv->comment);
	
	self->priv->comment = g_strdup (new_comment);
}


static gboolean
gexiv2_metadata_open_internal (GExiv2Metadata *self, Exiv2::Image::AutoPtr image, GError **error)
{
	if (image.get () == NULL || ! image->good ()) {
		g_set_error_literal (error, g_quark_from_string ("GExiv2"),
							501, "format seems not to be supported");
		return false;
	}
	
	image->readMetadata ();
	
	GExiv2MetadataPrivate* priv = self->priv;
	
	priv->exif_data = image->exifData ();
	priv->iptc_data = image->iptcData ();
	priv->xmp_data = image->xmpData ();
		
	gexiv2_metadata_set_comment_internal (self, image->comment ().c_str ());
	priv->mime_type = g_strdup (image->mimeType ().c_str ());
		
	priv->pixel_width = image->pixelWidth ();
	priv->pixel_height = image->pixelHeight ();
		
	Exiv2::AccessMode mode = image->checkMode (Exiv2::mdExif);
	priv->supports_exif = (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
	
	mode = image->checkMode (Exiv2::mdXmp);
	priv->supports_xmp = (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
	
	mode = image->checkMode (Exiv2::mdIptc);
	priv->supports_iptc = (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite);
	
	return true;
}


gboolean
gexiv2_metadata_open_path (GExiv2Metadata *self, const gchar *path, GError **error)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);

	try {
		Exiv2::BasicIo::AutoPtr file (new Exiv2::FileIo (path));
		
		return gexiv2_metadata_open_internal (self, Exiv2::ImageFactory::open (file), error);
		
	} catch (Exiv2::Error &e) {
		g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
	}
	
	return false;
}


gboolean
gexiv2_metadata_open_buf (GExiv2Metadata *self, const guint8 *data, glong n_data, GError **error)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	try {
	
		return gexiv2_metadata_open_internal (self, Exiv2::ImageFactory::open (data, n_data), error);
		
	} catch (Exiv2::Error &e) {
		g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
	}
	
	return false;
}


gboolean
gexiv2_metadata_open_stream (GExiv2Metadata *self, ManagedStreamCallbacks* cb, GError **error)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	try {
		Exiv2::BasicIo::AutoPtr stream_ptr (new StreamIo (cb));
		
		return gexiv2_metadata_open_internal (self, Exiv2::ImageFactory::open (stream_ptr), error);
	
	} catch (Exiv2::Error &e) {
		g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
	}
	
	return false;
}


static gboolean
gexiv2_metadata_save_internal (GExiv2Metadata *self, Exiv2::Image::AutoPtr image, GError **error)
{
	if (image.get () == NULL || ! image->good ()) {
		g_set_error_literal (error, g_quark_from_string ("GExiv2"),
							501, "format seems not to be supported");
		return false;
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
            
            image->setExifData (self->priv->exif_data);
		} else {
			image->setExifData (self->priv->exif_data);
		}
	}
		
	mode = image->checkMode (Exiv2::mdXmp);
	if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
		image->setXmpData (self->priv->xmp_data);
	
	mode = image->checkMode (Exiv2::mdIptc);
	if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
		image->setIptcData (self->priv->iptc_data);
	
	mode = image->checkMode (Exiv2::mdComment);
	if (mode == Exiv2::amWrite || mode == Exiv2::amReadWrite)
		image->setComment (self->priv->comment);
	
    image->writeMetadata ();
    
    return true;
}


gboolean
gexiv2_metadata_save_file (GExiv2Metadata *self, const gchar *path, GError **error)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	try {
		return gexiv2_metadata_save_internal (self, Exiv2::ImageFactory::open (path), error);

	} catch (Exiv2::Error e) {
		g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
	}
	
	return false;
}

gboolean
gexiv2_metadata_save_stream (GExiv2Metadata *self, ManagedStreamCallbacks* cb, GError **error)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	try {
		Exiv2::BasicIo::AutoPtr stream_ptr (new StreamIo (cb));
		
		return gexiv2_metadata_save_internal (self, Exiv2::ImageFactory::open (stream_ptr), error);
	
	} catch (Exiv2::Error &e) {
		g_set_error_literal (error, g_quark_from_string ("GExiv2"), e.code (), e.what ());
	}
	
	return false;
}



void
gexiv2_metadata_clear (GExiv2Metadata *self)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	gexiv2_metadata_clear_exif (self);
	gexiv2_metadata_clear_xmp (self);
	gexiv2_metadata_clear_iptc (self);
	gexiv2_metadata_clear_comment (self);
}


const gchar*
gexiv2_metadata_get_mime_type (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	return self->priv->mime_type;
}


gchar*
gexiv2_metadata_get_date_time (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	
	if (!exif_data.empty ()) {
		Exiv2::ExifKey key ("Exif.Photo.DateTimeOriginal");
		Exiv2::ExifData::iterator it = exif_data.findKey (key);
		if (it != exif_data.end ())
			return g_strdup (it->toString ().c_str ());
				
		key = Exiv2::ExifKey ("Exif.Image.DateTime");
		it = exif_data.findKey (key);
		if (it != exif_data.end ())
			return g_strdup (it->toString ().c_str ());
		
		key = Exiv2::ExifKey ("Exif.Photo.DateTimeDigitized");
		it = exif_data.findKey (key);
		if (it != self->priv->exif_data.end ())
			return g_strdup (it->toString ().c_str ());
	}
	
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	if (!xmp_data.empty ()) {
		Exiv2::XmpKey key ("Xmp.exif.DateTimeOriginal");
		Exiv2::XmpData::iterator it = xmp_data.findKey (key);
		if (it != xmp_data.end ())
			return g_strdup (it->toString ().c_str ());
		
		key = Exiv2::XmpKey ("Xmp.xmp.CreateDate");
		it = xmp_data.findKey (key);
		if (it != xmp_data.end ())
			return g_strdup (it->toString ().c_str ());
		
		key = Exiv2::XmpKey ("Xmp.exif.DateTimeDigitized");
		it = xmp_data.findKey (key);
		if (it != xmp_data.end ())
			return g_strdup (it->toString ().c_str ());
		
		key = Exiv2::XmpKey ("Xmp.tiff.DateTime");
		it = xmp_data.findKey (key);
		if (it != xmp_data.end ())
			return g_strdup (it->toString ().c_str ());
		
		key = Exiv2::XmpKey ("Xmp.photoshop.DateCreated");
		it = xmp_data.findKey (key);
		if (it != xmp_data.end ())
			return g_strdup (it->toString ().c_str ());
		
		key = Exiv2::XmpKey ("Xmp.xmp.ModifyDate");
		it = xmp_data.findKey (key);
		if (it != xmp_data.end ())
			return g_strdup (it->toString ().c_str ());
		
		key = Exiv2::XmpKey ("Xmp.xmp.MetadataDate");
		it = xmp_data.findKey (key);
		if (it != xmp_data.end ())
			return g_strdup (it->toString ().c_str ());
	}
	
	/* FIXME: handle IPTC Date */
	
	return NULL;
}


void
gexiv2_metadata_set_date_time (GExiv2Metadata *self, const gchar* datetime)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	exif_data["Exif.Image.DateTime"] = datetime;
	exif_data["Exif.Photo.DateTimeOriginal"] = datetime;
	/* FIXME: Maybe make set of Digitized-Time optional */
	exif_data["Exif.Photo.DateTimeDigitized"] = datetime;
	
	xmp_data["Xmp.exif.DateTimeOriginal"] = datetime;
	/* FIXME: Maybe make set of Digitized-Time optional */
	xmp_data["Xmp.exif.DateTimeDigitized"] = datetime;
	xmp_data["Xmp.xmp.CreateDate"] = datetime;
	xmp_data["Xmp.tiff.DateTime"] = datetime;
	xmp_data["Xmp.photoshop.DateCreated"] = datetime;
	xmp_data["Xmp.xmp.ModifyDate"] = datetime;
	xmp_data["Xmp.xmp.MetadataDate"] = datetime;
	
	/* handle IPTC data */
}


GExiv2Orientation
gexiv2_metadata_get_orientation (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), GEXIV2_ORIENTATION_UNSPECIFIED);
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	
	if (!exif_data.empty ()) {
	
		/* Because some camera set a wrong standard exif orientation tag,
		   We need to check makernote tags first!
		 */
		Exiv2::ExifKey minolta_key1 ("Exif.MinoltaCs7D.Rotation");
		Exiv2::ExifData::iterator it = exif_data.findKey (minolta_key1);
		if (it != exif_data.end ()) {
			switch (it->toLong ()) {
				case 76:
					return GEXIV2_ORIENTATION_ROT_90;
				case 82:
					return GEXIV2_ORIENTATION_ROT_270;
				default:
					return GEXIV2_ORIENTATION_NORMAL;
			}
		}
		
		Exiv2::ExifKey minolta_key2 ("Exif.MinoltaCs5D.Rotation");
		it = exif_data.findKey (minolta_key2);
		if (it != exif_data.end ()) {
			switch (it->toLong ()) {
				case 76:
					return GEXIV2_ORIENTATION_ROT_90;
				case 82:
					return GEXIV2_ORIENTATION_ROT_270;
				default:
					return GEXIV2_ORIENTATION_NORMAL;
			}
		}
	
		Exiv2::ExifKey std_key ("Exif.Image.Orientation");
		it = self->priv->exif_data.findKey (std_key);
		if (it != exif_data.end ())
			return (GExiv2Orientation)it->toLong ();
	}
	
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	if (!xmp_data.empty ()) {
		Exiv2::XmpKey tiff_width_key ("Xmp.tiff.ImageWidth");
		Exiv2::XmpData::iterator it = xmp_data.findKey (tiff_width_key);
		if (it != xmp_data.end ())
			return (GExiv2Orientation)(it->toLong ());
	}
	
	return GEXIV2_ORIENTATION_UNSPECIFIED;
}


gboolean
gexiv2_metadata_get_supports_exif (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	return self->priv->supports_exif;
}


gboolean
gexiv2_metadata_get_supports_xmp (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	return self->priv->supports_xmp;
}


gboolean
gexiv2_metadata_get_supports_iptc (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	return self->priv->supports_iptc;
}


void
gexiv2_metadata_set_orientation (GExiv2Metadata *self, GExiv2Orientation orientation)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	g_return_if_fail (orientation > GEXIV2_ORIENTATION_ROT_270);
	g_return_if_fail (orientation < GEXIV2_ORIENTATION_UNSPECIFIED);	
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	exif_data["Exif.Image.Orientation"] = static_cast<uint16_t> (orientation);
	xmp_data["Xmp.tiff.ImageWidth"] = static_cast<uint16_t> (orientation);
	
	Exiv2::ExifKey minolta_key1 ("Exif.MinoltaCs7D.Rotation");
	Exiv2::ExifData::iterator it = exif_data.findKey (minolta_key1);
	if (it != exif_data.end ())
		exif_data.erase (it);
	
	Exiv2::ExifKey minolta_key2 ("Exif.MinoltaCs5D.Rotation");
	it = exif_data.findKey (minolta_key2);
	if (it != exif_data.end ())
		exif_data.erase (it);
}


gint
gexiv2_metadata_get_pixel_width (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), -1);
	
	return self->priv->pixel_width;
}


gint
gexiv2_metadata_get_pixel_height (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), -1);
	
	return self->priv->pixel_height;
}


gint
gexiv2_metadata_get_metadata_pixel_width (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), -1);
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	
	if (!exif_data.empty ()) {
		Exiv2::ExifKey pixel_dim_key ("Exif.Photo.PixelXDimension");
		Exiv2::ExifData::iterator it = exif_data.findKey (pixel_dim_key);
		if (it != exif_data.end ())
			return it->toLong ();
	
		Exiv2::ExifKey image_width_key ("Exif.Image.ImageWidth");
		it = self->priv->exif_data.findKey (image_width_key);
		if (it != exif_data.end ())
			return it->toLong ();
	}
	
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	if (!exif_data.empty ()) {
		Exiv2::XmpKey tiff_width_key ("Xmp.tiff.ImageWidth");
		Exiv2::XmpData::iterator it = xmp_data.findKey (tiff_width_key);
		if (it != xmp_data.end ())
			return it->toLong ();
	
		Exiv2::XmpKey xmp_exif_key ("Xmp.exif.PixelXDimension");
		it = xmp_data.findKey (xmp_exif_key);
		if (it != xmp_data.end ())
			return it->toLong ();
	}
	
	return -1;
}


gint
gexiv2_metadata_get_metadata_pixel_height (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), -1);
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	
	if (!exif_data.empty ()) {
		Exiv2::ExifKey pixel_dim_key ("Exif.Photo.PixelYDimension");
		Exiv2::ExifData::iterator it = exif_data.findKey (pixel_dim_key);
		if (it != exif_data.end ())
			return it->toLong ();
	
		Exiv2::ExifKey image_height_key ("Exif.Image.ImageHeight");
		it = exif_data.findKey (image_height_key);
		if (it != exif_data.end ())
			return it->toLong ();
	}
	
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	if (!exif_data.empty ()) {
		Exiv2::XmpKey tiff_height_key ("Xmp.tiff.ImageHeight");
		Exiv2::XmpData::iterator it = xmp_data.findKey (tiff_height_key);
		if (it != xmp_data.end ())
			return it->toLong ();
	
		Exiv2::XmpKey xmp_exif_key ("Xmp.exif.PixelYDimension");
		it = xmp_data.findKey (xmp_exif_key);
		if (it != xmp_data.end ())
			return it->toLong ();
	}
	
	return -1;
}


void
gexiv2_metadata_set_metadata_pixel_width (GExiv2Metadata *self, gint width)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	exif_data["Exif.Photo.PixelXDimension"] = static_cast<uint32_t>(width);
	exif_data["Exif.Image.ImageWidth"] = static_cast<uint32_t>(width);
	xmp_data["Xmp.tiff.ImageWidth"] = static_cast<uint32_t>(width);
	xmp_data["Xmp.exif.PixelYDimension"] = static_cast<uint32_t>(width);
}


void
gexiv2_metadata_set_metadata_pixel_height (GExiv2Metadata *self, gint height)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	exif_data ["Exif.Photo.PixelXDimension"] = static_cast<uint32_t>(height);
	exif_data ["Exif.Image.ImageHeight"] = static_cast<uint32_t>(height);
	xmp_data ["Xmp.tiff.ImageHeight"] = static_cast<uint32_t>(height);
	xmp_data ["Xmp.exif.PixelXDimension"] = static_cast<uint32_t>(height);
}


gchar*
gexiv2_metadata_get_comment (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
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
	
	str = gexiv2_metadata_get_exif_tag_interpreted_string (self, "Iptc.Application2.Caption");
	if (str != NULL && *str != '\0')
		return str;
	else
		g_free (str);
	
	return NULL;
}


void
gexiv2_metadata_set_comment (GExiv2Metadata *self, const gchar* comment)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	Exiv2::IptcData& iptc_data = self->priv->iptc_data;
	
	gexiv2_metadata_set_comment_internal (self, (comment != NULL) ? comment : "");
	exif_data ["Exif.Image.ImageDescription"] = comment;
	exif_data ["Exif.Photo.UserComment"] = comment;
	exif_data ["Exif.Image.XPComment"] = comment;
	exif_data ["Iptc.Application2.Caption"] = comment;
}


void
gexiv2_metadata_clear_comment (GExiv2Metadata *self)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	/* dont'delete the comment field */
	gexiv2_metadata_set_comment_internal (self, "");
}


gchar**
gexiv2_metadata_get_keywords (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	gchar** keywords = gexiv2_metadata_get_xmp_tag_multiple (self, "Xmp.dc.subject");
	if (keywords != NULL && (*keywords) != NULL)
		return keywords;
	
	keywords = gexiv2_metadata_get_iptc_keywords (self);
	if (keywords != NULL)
		return keywords;
	
	/* return empty string array */
	keywords = g_new (gchar*, 1);
	keywords[0] = NULL;
	return keywords;
}


void
gexiv2_metadata_set_keywords (GExiv2Metadata *self, const gchar** keywords)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	gexiv2_metadata_set_xmp_tag_multiple (self, "Xmp.dc.subject", keywords);
	gexiv2_metadata_set_iptc_tag_multiple (self, "Iptc.Application2.Keywords", keywords);
}


guint
gexiv2_metadata_get_rating (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), 0);
	
	guint rating = gexiv2_metadata_get_xmp_tag_long (self, "Xmp.xmp.Rating");
	
	if (rating != 0)
		return rating;
		
	rating = gexiv2_metadata_get_exif_tag_long (self, "Exif.Image.Rating");
	
	return rating;
}


void
gexiv2_metadata_set_rating (GExiv2Metadata *self, guint rating)
{
	gexiv2_metadata_set_xmp_tag_long (self, "Xmp.xmp.Rating", rating);
	gexiv2_metadata_set_exif_tag_long (self, "Exif.Image.Rating", rating);
}


gchar*
gexiv2_metadata_get_tag_string (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	if (g_ascii_strncasecmp ("Xmp." , tag, 4) == 0)
		return gexiv2_metadata_get_xmp_tag_string (self, tag);
		
	if (g_ascii_strncasecmp ("Exif." , tag, 4) == 0)
		return gexiv2_metadata_get_exif_tag_string (self, tag);
		
	if (g_ascii_strncasecmp ("Iptc." , tag, 5) == 0)
		return gexiv2_metadata_get_iptc_tag_string (self, tag);
	
	return NULL;
}


gchar*
gexiv2_metadata_get_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	if (g_ascii_strncasecmp ("Xmp." , tag, 4) == 0)
		return gexiv2_metadata_get_xmp_tag_interpreted_string (self, tag);
		
	if (g_ascii_strncasecmp ("Exif." , tag, 4) == 0)
		return gexiv2_metadata_get_exif_tag_interpreted_string (self, tag);
		
	if (g_ascii_strncasecmp ("Iptc." , tag, 5) == 0)
		return gexiv2_metadata_get_iptc_tag_interpreted_string (self, tag);
	
	return NULL;
}


gboolean
gexiv2_metadata_get_exposure_time (GExiv2Metadata *self, gint *nom, gint *den)
{
	return gexiv2_metadata_get_exif_tag_rational (self, "	Exif.Photo.ExposureTime", nom, den);
}


gdouble
gexiv2_metadata_get_fnumber (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), -1.0);
	
	gint nom;
	gint den;
	
	if ( ! gexiv2_metadata_get_exif_tag_rational (self, "Exif.Photo.ISOSpeedRatings", &nom, &den))
		return -1.0;
	
	if (den == 0.0)
		return -1.0;
		
	return (nom / den);
}


gdouble
gexiv2_metadata_get_focal_length (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	gint nom;
	gint den;
	
	if ( ! gexiv2_metadata_get_exif_tag_rational (self, "Exif.Photo.FocalLength", &nom, &den))
		return -1.0;
	
	if (den == 0.0)
		return -1.0;
		
	return (nom / den);
}


gint gexiv2_metadata_get_iso_speed (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	return (gint) gexiv2_metadata_get_exif_tag_long (self, "Exif.Photo.ISOSpeedRatings");
}


gchar*
gexiv2_metadata_get_camera_model (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_exif_tag_string (self, "Exif.Image.Model");
}


G_END_DECLS
