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

#include <algorithm>
#include <exiv2/exiv2.hpp>
#include <gexiv2/gexiv2-metadata.h>

// Internal C++ functions, outside of G_BEGIN_DECLS
// FIXME: Do we really need G_BEGIN_DECLS/END_DECLS for internal header?

namespace detail {
G_GNUC_INTERNAL std::string collate_key(const std::string& string);

template<typename T>
G_GNUC_INTERNAL void sortMetadata(T& container) {
    std::sort(container.begin(), container.end(), [](Exiv2::Metadatum& a, Exiv2::Metadatum& b) {
        return collate_key(a.key()) < collate_key(b.key());
    });
}
}; // namespace detail

G_BEGIN_DECLS

struct _GExiv2MetadataPrivate
{
    Exiv2::Image::UniquePtr image;
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
using GExiv2MetadataPrivate = struct _GExiv2MetadataPrivate;

G_GNUC_INTERNAL GExiv2MetadataPrivate* gexiv2_priv(GExiv2Metadata* self);

/* private EXIF functions */

G_GNUC_INTERNAL gboolean		gexiv2_metadata_has_exif_tag		(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_clear_exif_tag		(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_exif_tag_string	(GExiv2Metadata *self, const gchar* tag, GError **error);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_exif_tag_string	(GExiv2Metadata *self, const gchar* tag, const gchar* value, GError **error);
G_GNUC_INTERNAL gchar** gexiv2_metadata_get_exif_tag_multiple(GExiv2Metadata* self, const gchar* tag, GError** error);
G_GNUC_INTERNAL gboolean gexiv2_metadata_set_exif_tag_multiple(GExiv2Metadata* self,
                                                               const gchar* tag,
                                                               const gchar** values,
                                                               GError** error);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_exif_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag, GError **error);
G_GNUC_INTERNAL glong			gexiv2_metadata_get_exif_tag_long	(GExiv2Metadata *self, const gchar* tag, GError **error);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_exif_tag_long	(GExiv2Metadata *self, const gchar* tag, glong value, GError **error);
G_GNUC_INTERNAL gdouble gexiv2_metadata_get_exif_tag_rational_as_double(GExiv2Metadata* self,
                                                                        const gchar* tag,
                                                                        gdouble def,
                                                                        GError** error);

G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_exif_tag_label	(const gchar* tag, GError **error);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_exif_tag_description (const gchar* tag, GError **error);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_exif_tag_type (const gchar* tag, GError **error);
G_GNUC_INTERNAL gboolean        gexiv2_metadata_exif_tag_supports_multiple_values(const gchar* tag, GError** error);

G_GNUC_INTERNAL GBytes*			gexiv2_metadata_get_exif_tag_raw	(GExiv2Metadata *self, const gchar* tag, GError **error);

/* private XMP functions */

G_GNUC_INTERNAL gboolean		gexiv2_metadata_clear_xmp_tag		(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_has_xmp_tag			(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_xmp_tag_string	(GExiv2Metadata *self, const gchar* tag, GError **error);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_xmp_tag_string	(GExiv2Metadata *self, const gchar* tag, const gchar* value, GError **error);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_xmp_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag, GError **error);
G_GNUC_INTERNAL glong			gexiv2_metadata_get_xmp_tag_long	(GExiv2Metadata *self, const gchar* tag, GError **error);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_xmp_tag_long	(GExiv2Metadata *self, const gchar* tag, glong value, GError **error);
G_GNUC_INTERNAL gchar**			gexiv2_metadata_get_xmp_tag_multiple (GExiv2Metadata *self, const gchar* tag, GError **error);
G_GNUC_INTERNAL gchar**			gexiv2_metadata_get_xmp_tag_multiple_deprecated (GExiv2Metadata *self, const gchar* tag, GError **error);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_xmp_tag_multiple (GExiv2Metadata *self, const gchar* tag, const gchar** values, GError **error);

G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_xmp_tag_label		(const gchar* tag, GError **error);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_xmp_tag_description	(const gchar* tag, GError **error);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_xmp_tag_type	(const gchar* tag, GError **error);
G_GNUC_INTERNAL gboolean        gexiv2_metadata_xmp_tag_supports_multiple_values(GExiv2Metadata* self, const gchar* tag, GError** error);

G_GNUC_INTERNAL GBytes*			gexiv2_metadata_get_xmp_tag_raw		(GExiv2Metadata *self, const gchar* tag, GError **error);

/* private IPTC functions */

G_GNUC_INTERNAL gboolean		gexiv2_metadata_clear_iptc_tag		(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_has_iptc_tag		(GExiv2Metadata *self, const gchar* tag);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_iptc_tag_string	(GExiv2Metadata *self, const gchar* tag, GError **error);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_iptc_tag_string	(GExiv2Metadata *self, const gchar* tag, const gchar* value, GError **error);
G_GNUC_INTERNAL gchar*			gexiv2_metadata_get_iptc_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag, GError **error);
G_GNUC_INTERNAL gchar**			gexiv2_metadata_get_iptc_tag_multiple	(GExiv2Metadata *self, const gchar* tag, GError **error);
G_GNUC_INTERNAL gboolean		gexiv2_metadata_set_iptc_tag_multiple	(GExiv2Metadata *self, const gchar* tag, const gchar** values, GError **error);

G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_iptc_tag_label	(const gchar* tag, GError **error);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_iptc_tag_description	(const gchar* tag, GError **error);
G_GNUC_INTERNAL const gchar*	gexiv2_metadata_get_iptc_tag_type	(const gchar* tag, GError **error);
G_GNUC_INTERNAL gboolean        gexiv2_metadata_iptc_tag_supports_multiple_values(const gchar* tag, GError** error);

G_GNUC_INTERNAL GBytes*			gexiv2_metadata_get_iptc_tag_raw	(GExiv2Metadata *self, const gchar* tag, GError **error);

G_END_DECLS

#endif /* GEXIV2_METADATA_PRIVATE_H */
