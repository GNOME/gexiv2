/*
 * gexiv2-metadata.h
 *
 * Author(s)
 * 	Mike Gemuende <mike@gemuende.de>
 * 	Jim Nelson <jim@yorba.org>
 *
 * This is free software. See COPYING for details.
 */

#ifndef __GEXIV2_METADATA_H__
#define __GEXIV2_METADATA_H__

#include <glib-object.h>
#include <gio/gio.h>
#include <gexiv2/gexiv2-managed-stream.h>
#include <gexiv2/gexiv2-preview-properties.h>
#include <gexiv2/gexiv2-preview-image.h>

G_BEGIN_DECLS

#define GEXIV2_TYPE_METADATA \
	(gexiv2_metadata_get_type ())
	
#define GEXIV2_METADATA(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), GEXIV2_TYPE_METADATA, GExiv2Metadata))
	
#define GEXIV2_IS_METADATA(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEXIV2_TYPE_METADATA))
	
#define GEXIV2_METADATA_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), GEXIV2_TYPE_METADATA, GExiv2MetadataClass))
	
#define GEXIV2_IS_METADATA_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), GEXIV2_TYPE_METADATA))
	
#define GEXIV2_METADATA_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), GEXIV2_TYPE_METADATA, GExiv2MetadataClass))
	
#define GEXIV2_METADATA_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GEXIV2_TYPE_METADATA, GExiv2MetadataPrivate))


typedef enum {
	GEXIV2_ORIENTATION_MIN			= 0,
	GEXIV2_ORIENTATION_UNSPECIFIED	= 0,
	GEXIV2_ORIENTATION_NORMAL		= 1,
	GEXIV2_ORIENTATION_HFLIP		= 2,
	GEXIV2_ORIENTATION_ROT_180		= 3,
	GEXIV2_ORIENTATION_VFLIP		= 4,
	GEXIV2_ORIENTATION_ROT_90_HFLIP	= 5,
	GEXIV2_ORIENTATION_ROT_90		= 6,
	GEXIV2_ORIENTATION_ROT_90_VFLIP	= 7,
	GEXIV2_ORIENTATION_ROT_270		= 8,
	GEXIV2_ORIENTATION_MAX			= 8
} GExiv2Orientation;


typedef struct _GExiv2Metadata			GExiv2Metadata;
typedef struct _GExiv2MetadataClass		GExiv2MetadataClass;
typedef struct _GExiv2MetadataPrivate	GExiv2MetadataPrivate;

struct _GExiv2Metadata
{
	GObject parent_instance;

	/*< private >*/
	GExiv2MetadataPrivate *priv;
};

struct _GExiv2MetadataClass
{
	GObjectClass parent_class;
};

/* basic functions */

GType 			gexiv2_metadata_get_type			(void);

/**
 * gexiv2_metadata_new:
 *
 * Returns: (transfer full): A fully constructed #GExiv2Metadata ready to be used
 */
GExiv2Metadata* gexiv2_metadata_new					(void);
void			gexiv2_metadata_free				(GExiv2Metadata *self);

/**
 * gexiv2_metadata_open_path:
 * @path: Path to the file you want to open
 *
 * Returns: Boolean success indicator
 */
gboolean		gexiv2_metadata_open_path			(GExiv2Metadata *self, const gchar *path, GError **error);

/**
 * gexiv2_metadata_open_buf:
 * @data: (array length=n_data): A buffer containing the data to be read
 * @n_data: (skip): The length of the buffer
 *
 * Returns: Boolean success indicator
 */
gboolean		gexiv2_metadata_open_buf			(GExiv2Metadata *self, const guint8 *data, glong n_data, GError **error);

gboolean		gexiv2_metadata_open_stream			(GExiv2Metadata *self, ManagedStreamCallbacks* cb, GError **error);

gboolean		gexiv2_metadata_from_app1_segment	(GExiv2Metadata *self, const guint8 *data, glong n_data, GError **error);

gboolean		gexiv2_metadata_save_file			(GExiv2Metadata *self, const gchar *path, GError **error);

gboolean		gexiv2_metadata_save_stream			(GExiv2Metadata *self, ManagedStreamCallbacks* cb, GError **error);

gboolean		gexiv2_metadata_has_tag				(GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_clear_tag			(GExiv2Metadata *self, const gchar* tag);
void			gexiv2_metadata_clear				(GExiv2Metadata *self);

const gchar*	gexiv2_metadata_get_mime_type		(GExiv2Metadata *self);

gboolean		gexiv2_metadata_get_supports_exif	(GExiv2Metadata *self);
gboolean		gexiv2_metadata_get_supports_xmp	(GExiv2Metadata *self);
gboolean		gexiv2_metadata_get_supports_iptc	(GExiv2Metadata *self);

GExiv2Orientation gexiv2_metadata_get_orientation 	(GExiv2Metadata *self);
void			gexiv2_metadata_set_orientation		(GExiv2Metadata *self, GExiv2Orientation orientation);

gint			gexiv2_metadata_get_pixel_width		(GExiv2Metadata *self);
gint			gexiv2_metadata_get_pixel_height	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_comment:
 *
 * Returns: (transfer full) (allow-none): The photo's comment
 */
gchar*			gexiv2_metadata_get_comment			(GExiv2Metadata *self);
void			gexiv2_metadata_set_comment			(GExiv2Metadata *self, const gchar* comment);
void			gexiv2_metadata_clear_comment		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_tag_string:
 * @tag: The name of the tag who's value you want
 *
 * Returns: (transfer full) (allow-none): The tag's value as a string
 */
gchar*			gexiv2_metadata_get_tag_string		(GExiv2Metadata *self, const gchar* tag);

/**
 * gexiv2_metadata_get_tag_interpreted_string:
 * @tag: The name of the tag who's value you want
 *
 * Returns: (transfer full) (allow-none): The tag's interpreted value as a string
 */
gchar*			gexiv2_metadata_get_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_tag_string		(GExiv2Metadata *self, const gchar* tag, const gchar* value);

glong			gexiv2_metadata_get_tag_long		(GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_tag_long		(GExiv2Metadata *self, const gchar* tag, glong value);


/**
 * gexiv2_metadata_get_tag_multiple:
 * @tag: The name of the tag to get
 *
 * Returns: (transfer full) (allow-none) (array zero-terminated=1): The multiple values of that tag
 */
gchar**			gexiv2_metadata_get_tag_multiple	(GExiv2Metadata *self, const gchar* tag);

/**
 * gexiv2_metadata_set_tag_multiple:
 * @values: (array zero-terminated=1): An array of tags that you want to set
 *
 * Returns: (transfer full): Boolean success value
 */
gboolean		gexiv2_metadata_set_tag_multiple	(GExiv2Metadata *self, const gchar* tag, const gchar** values);

/**
 * gexiv2_metadata_get_exposure_time:
 * @nom: (out): The numerator
 * @den: (out): The denominator
 *
 * Returns the exposure time in seconds (shutter speed, <em>not</em> date-time of exposure) as a
 * rational.  See https://en.wikipedia.org/wiki/Shutter_speed for more information.
 *
 * Returns: (skip): Boolean success value
 */
gboolean		gexiv2_metadata_get_exposure_time	(GExiv2Metadata *self, gint *nom, gint *den);

/**
 * gexiv2_metadata_get_fnumber:
 *
 * See https://en.wikipedia.org/wiki/F-number for more information.
 *
 * Returns: The exposure Fnumber as a gdouble, or -1.0 if tag is not present or invalid.
 */
gdouble			gexiv2_metadata_get_fnumber			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_focal_length:
 *
 * See https://en.wikipedia.org/wiki/Flange_focal_distance for more information.
 *
 * Returns: The focal length as a gdouble, or -1.0 if tag is not present or invalid.
 */
gdouble			gexiv2_metadata_get_focal_length	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_iso_speed:
 *
 * See https://en.wikipedia.org/wiki/Iso_speed for more information.
 *
 * Returns: The ISO speed rating as a gint, or 0 if tag is not present or invalid.
 */
gint			gexiv2_metadata_get_iso_speed		(GExiv2Metadata *self);

/* static functions */

gboolean		gexiv2_metadata_is_xmp_tag				(const gchar* tag);
gboolean		gexiv2_metadata_is_exif_tag				(const gchar* tag);
gboolean		gexiv2_metadata_is_iptc_tag				(const gchar* tag);

/**
 * gexiv2_metadata_get_tag_label:
 * @tag: Name of the tag who's label you want
 *
 * Returns: (transfer none) (allow-none): The tag's label
 */
const gchar*	gexiv2_metadata_get_tag_label		(const gchar *tag);

/**
 * gexiv2_metadata_get_tag_description:
 * @tag: Name of the tag who's label you want
 *
 * Returns: (transfer none) (allow-none): The tag's description
 */
const gchar*	gexiv2_metadata_get_tag_description	(const gchar *tag);

/**
 * gexiv2_metadata_get_tag_type:
 * @tag: Name of the tag who's type you want
 *
 * Returns: (transfer none) (allow-none): The tag's type
 */
const gchar*	gexiv2_metadata_get_tag_type	(const gchar *tag);


/**
 * EXIF functions
 */

gboolean		gexiv2_metadata_has_exif			(GExiv2Metadata *self);
void			gexiv2_metadata_clear_exif			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_exif_tags:
 *
 * Returns: (transfer full) (array zero-terminated=1): A list of the available EXIF tags
 */
gchar**			gexiv2_metadata_get_exif_tags		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_exif_tag_rational:
 * @tag: (in): The tag you want the rational value for
 * @nom: (out): The numerator
 * @den: (out): The denominator
 *
 * Returns: (skip): Boolean success value
 */
gboolean		gexiv2_metadata_get_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint* nom, gint* den);
gboolean		gexiv2_metadata_set_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint nom, gint den);

/**
 * gexiv2_metadata_get_exif_thumbnail:
 * @buffer: (out) (array length=size) (transfer full): Where to store the thumbnail data
 * @size: (skip): Size of the thumbnail's buffer
 *
 * Returns: (skip): Boolean success value
 */
gboolean		gexiv2_metadata_get_exif_thumbnail (GExiv2Metadata *self, guint8** buffer, gint* size);
gboolean		gexiv2_metadata_set_exif_thumbnail_from_file (GExiv2Metadata *self, const gchar *path, GError **error);

/**
 * gexiv2_metadata_set_exif_thumbnail_from_buffer:
 * @buffer: (array length=size): A buffer containing thumbnail data
 * @size: (skip): Size of the thumbnail's buffer
 */
void			gexiv2_metadata_set_exif_thumbnail_from_buffer (GExiv2Metadata *self, const guint8 *buffer, gint size);
void			gexiv2_metadata_erase_exif_thumbnail (GExiv2Metadata *self);


/**
 * XMP functions
 */

gboolean		gexiv2_metadata_has_xmp				(GExiv2Metadata *self);
void			gexiv2_metadata_clear_xmp			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_xmp_packet:
 *
 * Returns: (transfer full) (allow-none): The XMP tag's description
 */
gchar*			gexiv2_metadata_get_xmp_packet		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_xmp_tags:
 *
 * Returns: (transfer full) (array zero-terminated=1): A list of the available XMP tags
 */
gchar**			gexiv2_metadata_get_xmp_tags		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_register_xmp_namespace:
 * @name: XMP URI name (should end in /)
 * @prefix: XMP namespace prefix
 *
 * Returns: (skip): Boolean success value
 */
gboolean        gexiv2_metadata_register_xmp_namespace (const gchar* name, const gchar *prefix);

/**
 * gexiv2_metadata_unregister_xmp_namespace:
 * @name: XMP URI name (should end in /)
 *
 * Returns: (skip): Boolean success value
 */
gboolean        gexiv2_metadata_unregister_xmp_namespace (const gchar* name);

void            gexiv2_metadata_unregister_all_xmp_namespaces (void);

/* IPTC functions */

gboolean		gexiv2_metadata_has_iptc			(GExiv2Metadata *self);
void			gexiv2_metadata_clear_iptc			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_iptc_tags:
 *
 * Returns: (transfer full) (array zero-terminated=1): A list of the available IPTC tags
 */
gchar**			gexiv2_metadata_get_iptc_tags		(GExiv2Metadata *self);


/**
 * GPS functions
 */

void			gexiv2_metadata_delete_gps_info			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_gps_longitude:
 * @longitude: (out): Variable to store the longitude value
 *
 * Returns: (skip): Boolean success value
 */
gboolean		gexiv2_metadata_get_gps_longitude			(GExiv2Metadata *self, gdouble *longitude);

/**
 * gexiv2_metadata_get_gps_latitude:
 * @latitude: (out): Variable to store the latitude value
 *
 * Returns: (skip): Boolean success value
 */
gboolean		gexiv2_metadata_get_gps_latitude			(GExiv2Metadata *self, gdouble *latitude);

/**
 * gexiv2_metadata_get_gps_altitude:
 * @altitude: (out): Variable to store the altitude value
 *
 * Returns: (skip): Boolean success value
 */
gboolean		gexiv2_metadata_get_gps_altitude			(GExiv2Metadata *self, gdouble *altitude);

/**
 * gexiv2_metadata_get_gps_info:
 * @longitude: (out): Variable to store the longitude value
 * @latitude: (out): Variable to store the latitude value
 * @altitude: (out): Variable to store the altitude value
 *
 * Returns: (skip): Boolean success value.
 */
gboolean		gexiv2_metadata_get_gps_info				(GExiv2Metadata *self, gdouble *longitude, gdouble *latitude, gdouble *altitude);

gboolean		gexiv2_metadata_set_gps_info				(GExiv2Metadata *self, gdouble longitude, gdouble latitude, gdouble altitude);


/**
 * Preview Manager
 */

/**
 * gexiv2_metadata_get_preview_properties:
 *
 * Returns: (transfer none) (allow-none) (array zero-terminated=1): A #GExiv2PreviewProperties instance
 */
GExiv2PreviewProperties** gexiv2_metadata_get_preview_properties (GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_preview_image:
 * @props: A #GExiv2PreviewProperties instance
 *
 * Returns: (transfer full): A #GExiv2PreviewImage instance
 */
GExiv2PreviewImage* gexiv2_metadata_get_preview_image		(GExiv2Metadata *self, GExiv2PreviewProperties *props);

G_END_DECLS

#endif /* __GEXIV2_METADATA_H__ */

