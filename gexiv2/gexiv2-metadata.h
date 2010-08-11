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

GExiv2Metadata* gexiv2_metadata_new					();
void			gexiv2_metadata_free				(GExiv2Metadata *self);

gboolean		gexiv2_metadata_open_path			(GExiv2Metadata *self, const gchar *path, GError **error);

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

gchar*			gexiv2_metadata_get_date_time		(GExiv2Metadata *self);
void			gexiv2_metadata_set_date_time		(GExiv2Metadata *self, const gchar *datetime);

GExiv2Orientation gexiv2_metadata_get_orientation 	(GExiv2Metadata *self);
void			gexiv2_metadata_set_orientation		(GExiv2Metadata *self, GExiv2Orientation orientation);

gint			gexiv2_metadata_get_pixel_width		(GExiv2Metadata *self);
gint			gexiv2_metadata_get_pixel_height	(GExiv2Metadata *self);

gint			gexiv2_metadata_get_metadata_pixel_width	(GExiv2Metadata *self);
gint			gexiv2_metadata_get_metadata_pixel_height	(GExiv2Metadata *self);
void			gexiv2_metadata_set_metadata_pixel_width	(GExiv2Metadata *self, gint width);
void			gexiv2_metadata_set_metadata_pixel_height	(GExiv2Metadata *self, gint height);

gchar*			gexiv2_metadata_get_comment			(GExiv2Metadata *self);
void			gexiv2_metadata_set_comment			(GExiv2Metadata *self, const gchar* comment);
void			gexiv2_metadata_clear_comment		(GExiv2Metadata *self);

gchar**			gexiv2_metadata_get_keywords		(GExiv2Metadata *self);
void			gexiv2_metadata_set_keywords		(GExiv2Metadata *self, const gchar** keywords);

guint			gexiv2_metadata_get_rating			(GExiv2Metadata *self);
void			gexiv2_metadata_set_rating			(GExiv2Metadata *self, guint rating);

gchar*			gexiv2_metadata_get_tag_string		(GExiv2Metadata *self, const gchar* tag);
gchar*			gexiv2_metadata_get_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_tag_string		(GExiv2Metadata *self, const gchar* tag, const gchar* value);

glong			gexiv2_metadata_get_tag_long		(GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_tag_long		(GExiv2Metadata *self, const gchar* tag, glong value);

gchar**			gexiv2_metadata_get_tag_multiple	(GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_tag_multiple	(GExiv2Metadata *self, const gchar* tag, const gchar** values);

/* TODO: not yet implemented
gchar*			gexiv2_metadata_get_creator			(GExiv2Metadata *self);
void			gexiv2_metadata_set_creator			(GExiv2Metadata *self, const gchar* creator);

gchar*			gexiv2_metadata_get_copyright		(GExiv2Metadata *self);
void			gexiv2_metadata_set_copyright		(GExiv2Metadata *self, const gchar* creator);
*/

gboolean		gexiv2_metadata_get_exposure_time	(GExiv2Metadata *self, gint *nom, gint *den);
gdouble			gexiv2_metadata_get_fnumber			(GExiv2Metadata *self);
gdouble			gexiv2_metadata_get_focal_length	(GExiv2Metadata *self);
gint			gexiv2_metadata_get_iso_speed		(GExiv2Metadata *self);
gchar*			gexiv2_metadata_get_camera_model	(GExiv2Metadata *self);

/* static functions */

const gchar*	gexiv2_metadata_get_tag_label		(const gchar *tag);
const gchar*	gexiv2_metadata_get_tag_description	(const gchar *tag);


/* exif functions */

gboolean		gexiv2_metadata_has_exif			(GExiv2Metadata *self);
gboolean		gexiv2_metadata_clear_exif_tag		(GExiv2Metadata *self, const gchar* tag);
void			gexiv2_metadata_clear_exif			(GExiv2Metadata *self);

gboolean		gexiv2_metadata_has_exif_tag		(GExiv2Metadata *self, const gchar* tag);
gchar**			gexiv2_metadata_get_exif_tags		(GExiv2Metadata *self);

gchar*			gexiv2_metadata_get_exif_tag_string	(GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_exif_tag_string	(GExiv2Metadata *self, const gchar* tag, const gchar* value);

gchar*			gexiv2_metadata_get_exif_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag);

glong			gexiv2_metadata_get_exif_tag_long	(GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_exif_tag_long	(GExiv2Metadata *self, const gchar* tag, glong value);

gboolean		gexiv2_metadata_get_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint* nom, gint* den);
gboolean		gexiv2_metadata_set_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint nom, gint den);

gboolean		gexiv2_metadata_get_exif_thumbnail (GExiv2Metadata *self, guint8** buffer, gint* size);
gboolean		gexiv2_metadata_set_exif_thumbnail_from_file (GExiv2Metadata *self, const gchar *path, GError **error);
void			gexiv2_metadata_set_exif_thumbnail_from_buffer (GExiv2Metadata *self, const guint8 *buffer, gint size);
void			gexiv2_metadata_erase_exif_thumbnail (GExiv2Metadata *self);

/* static exif functions */

const gchar*	gexiv2_metadata_get_exif_tag_label	(const gchar* tag);
const gchar*	gexiv2_metadata_get_exif_tag_description (const gchar* tag);



/* xmp functions */

gboolean		gexiv2_metadata_has_xmp				(GExiv2Metadata *self);
gboolean		gexiv2_metadata_clear_xmp_tag		(GExiv2Metadata *self, const gchar* tag);
void			gexiv2_metadata_clear_xmp			(GExiv2Metadata *self);

gchar*			gexiv2_metadata_get_xmp_packet		(GExiv2Metadata *self);

gboolean		gexiv2_metadata_has_xmp_tag			(GExiv2Metadata *self, const gchar* tag);
gchar**			gexiv2_metadata_get_xmp_tags		(GExiv2Metadata *self);

gchar*			gexiv2_metadata_get_xmp_tag_string	(GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_xmp_tag_string	(GExiv2Metadata *self, const gchar* tag, const gchar* value);

gchar*			gexiv2_metadata_get_xmp_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag);

glong			gexiv2_metadata_get_xmp_tag_long	(GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_xmp_tag_long	(GExiv2Metadata *self, const gchar* tag, glong value);

gchar**			gexiv2_metadata_get_xmp_tag_multiple (GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_xmp_tag_multiple (GExiv2Metadata *self, const gchar* tag, const gchar** values);

/* static xmp functions */

gboolean		gexiv2_metadata_is_xmp_tag				(const gchar* tag);
gboolean		gexiv2_metadata_is_exif_tag				(const gchar* tag);
gboolean		gexiv2_metadata_is_iptc_tag				(const gchar* tag);
const gchar*	gexiv2_metadata_get_xmp_tag_label		(const gchar* tag);
const gchar*	gexiv2_metadata_get_xmp_tag_description	(const gchar* tag);



/* iptc functions */

gboolean		gexiv2_metadata_has_iptc			(GExiv2Metadata *self);
gboolean		gexiv2_metadata_clear_iptc_tag		(GExiv2Metadata *self, const gchar* tag);
void			gexiv2_metadata_clear_iptc			(GExiv2Metadata *self);

gboolean		gexiv2_metadata_has_iptc_tag		(GExiv2Metadata *self, const gchar* tag);
gchar**			gexiv2_metadata_get_iptc_tags		(GExiv2Metadata *self);

gchar*			gexiv2_metadata_get_iptc_tag_string	(GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_iptc_tag_string	(GExiv2Metadata *self, const gchar* tag, const gchar* value);

gchar*			gexiv2_metadata_get_iptc_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag);

gchar**			gexiv2_metadata_get_iptc_tag_multiple	(GExiv2Metadata *self, const gchar* tag);
gboolean		gexiv2_metadata_set_iptc_tag_multiple	(GExiv2Metadata *self, const gchar* tag, const gchar** values);

gchar**			gexiv2_metadata_get_iptc_keywords	(GExiv2Metadata *self);

gchar*			gexiv2_metadata_get_iptc_headline	(GExiv2Metadata *self);
/* TODO: set headline */

gchar*			gexiv2_metadata_get_iptc_caption	(GExiv2Metadata *self);
/* TODO: set caption */

gchar*			gexiv2_metadata_get_iptc_byline			(GExiv2Metadata *self);
gchar*			gexiv2_metadata_get_iptc_byline_title	(GExiv2Metadata *self);
gchar*			gexiv2_metadata_get_iptc_copyright		(GExiv2Metadata *self);
gchar*			gexiv2_metadata_get_iptc_contact		(GExiv2Metadata *self);
gchar*			gexiv2_metadata_get_iptc_city			(GExiv2Metadata *self);
gchar*			gexiv2_metadata_get_iptc_sublocation	(GExiv2Metadata *self);
gchar*			gexiv2_metadata_get_iptc_province_state	(GExiv2Metadata *self);
gchar*			gexiv2_metadata_get_iptc_country_code	(GExiv2Metadata *self);
gchar*			gexiv2_metadata_get_iptc_country_name	(GExiv2Metadata *self);
gchar*			gexiv2_metadata_get_iptc_writer			(GExiv2Metadata *self);

void			gexiv2_metadata_set_iptc_byline			(GExiv2Metadata *self, const gchar* value);
void			gexiv2_metadata_set_iptc_byline_title	(GExiv2Metadata *self, const gchar* value);
void			gexiv2_metadata_set_iptc_copyright		(GExiv2Metadata *self, const gchar* value);
void			gexiv2_metadata_set_iptc_contact		(GExiv2Metadata *self, const gchar* value);
void			gexiv2_metadata_set_iptc_city			(GExiv2Metadata *self, const gchar* value);
void			gexiv2_metadata_set_iptc_sublocation	(GExiv2Metadata *self, const gchar* value);
void			gexiv2_metadata_set_iptc_province_state	(GExiv2Metadata *self, const gchar* value);
void			gexiv2_metadata_set_iptc_country_code	(GExiv2Metadata *self, const gchar* value);
void			gexiv2_metadata_set_iptc_country_name	(GExiv2Metadata *self, const gchar* value);
void			gexiv2_metadata_set_iptc_writer			(GExiv2Metadata *self, const gchar* value);


/* gps functions */

void			gexiv2_metadata_delete_gps_info			(GExiv2Metadata *self);

gboolean		gexiv2_metadata_get_gps_longitude			(GExiv2Metadata *self, gdouble *longitude);
gboolean		gexiv2_metadata_get_gps_latitude			(GExiv2Metadata *self, gdouble *latitude);
gboolean		gexiv2_metadata_get_gps_altitude			(GExiv2Metadata *self, gdouble *altitude);

gboolean		gexiv2_metadata_get_gps_info				(GExiv2Metadata *self, gdouble *longitude, gdouble *latitude, gdouble *altitude);

gboolean		gexiv2_metadata_set_gps_info				(GExiv2Metadata *self, gdouble longitude, gdouble latitude, gdouble altitude);


/* static iptc functions */

const gchar*	gexiv2_metadata_get_iptc_tag_label			(const gchar* tag);
const gchar*	gexiv2_metadata_get_iptc_tag_description	(const gchar* tag);


/* preview manager */

GExiv2PreviewProperties** gexiv2_metadata_get_preview_properties (GExiv2Metadata *self);
GExiv2PreviewImage* gexiv2_metadata_get_preview_image		(GExiv2Metadata *self, GExiv2PreviewProperties *props);

G_END_DECLS

#endif /* __GEXIV2_METADATA_H__ */

