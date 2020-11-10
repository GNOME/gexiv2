/*
 * gexiv2-metadata-private.h
 *
 * Author(s)
 *  Mike Gemuende <mike@gemuende.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef GEXIV2_METADATA_PRIVATE_H
#define GEXIV2_METADATA_PRIVATE_H

#include <gexiv2/gexiv2-metadata.h>
#include <exiv2/exiv2.hpp>

G_BEGIN_DECLS

struct _GExiv2MetadataPrivate
{
#if EXIV2_TEST_VERSION(0,27,99)
    Exiv2::Image::UniquePtr image;
#else
    Exiv2::Image::AutoPtr image;
#endif
    gchar* comment;
    gchar* mime_type;
    gint pixel_width;
    gint pixel_height;
    gboolean supports_exif;
    gboolean supports_xmp;
    gboolean supports_iptc;
    Exiv2::PreviewManager *preview_manager;
    GExiv2PreviewProperties **preview_properties;
};

#ifndef __GTK_DOC_IGNORE__
#define LOG_ERROR(e) \
    g_warning("%s", e.what());
#endif

/* private EXIF functions */

G_GNUC_INTERNAL gboolean		gexiv2_metadata_has_exif_tag		(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_clear_exif_tag		(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_exif_tag_string	(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_exif_tag_string	(GExiv2Metadata *self, const gchar* tag, const gchar* value);
G_GNUC_INTERNAL gchar**         gexiv2_metadata_get_exif_tag_multiple(GExiv2Metadata* self, const gchar* tag);
G_GNUC_INTERNAL gboolean        gexiv2_metadata_set_exif_tag_multiple(GExiv2Metadata* self, const gchar* tag, const gchar** values);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_exif_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL glong			gexiv2_metadata_get_exif_tag_long	(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_exif_tag_long	(GExiv2Metadata *self, const gchar* tag, glong value);
G_GNUC_INTERNAL gdouble			gexiv2_metadata_get_exif_tag_rational_as_double (GExiv2Metadata *self, const gchar* tag, gdouble def);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_exif_tag_label	(const gchar* tag);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_exif_tag_description (const gchar* tag);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_exif_tag_type (const gchar* tag);
G_GNUC_INTERNAL GBytes*			gexiv2_metadata_get_exif_tag_raw	(GExiv2Metadata *self, const gchar* tag);

/* private XMP functions */

G_GNUC_INTERNAL gboolean		gexiv2_metadata_clear_xmp_tag		(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_has_xmp_tag			(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_xmp_tag_string	(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_xmp_tag_string	(GExiv2Metadata *self, const gchar* tag, const gchar* value);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_xmp_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL glong			gexiv2_metadata_get_xmp_tag_long	(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_xmp_tag_long	(GExiv2Metadata *self, const gchar* tag, glong value);
G_GNUC_INTERNAL gchar**			gexiv2_metadata_get_xmp_tag_multiple (GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_xmp_tag_multiple (GExiv2Metadata *self, const gchar* tag, const gchar** values);

G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_xmp_tag_label		(const gchar* tag);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_xmp_tag_description	(const gchar* tag);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_xmp_tag_type	(const gchar* tag);

G_GNUC_INTERNAL GBytes*			gexiv2_metadata_get_xmp_tag_raw		(GExiv2Metadata *self, const gchar* tag);

/* private IPTC functions */

G_GNUC_INTERNAL gboolean		gexiv2_metadata_clear_iptc_tag		(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_has_iptc_tag		(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_iptc_tag_string	(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_iptc_tag_string	(GExiv2Metadata *self, const gchar* tag, const gchar* value);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_iptc_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gchar**			gexiv2_metadata_get_iptc_tag_multiple	(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_iptc_tag_multiple	(GExiv2Metadata *self, const gchar* tag, const gchar** values);

G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_iptc_tag_label	(const gchar* tag);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_iptc_tag_description	(const gchar* tag);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_iptc_tag_type	(const gchar* tag);

G_GNUC_INTERNAL GBytes*			gexiv2_metadata_get_iptc_tag_raw	(GExiv2Metadata *self, const gchar* tag);

G_END_DECLS

#endif /* GEXIV2_METADATA_PRIVATE_H */
