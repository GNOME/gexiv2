/*
 * gexiv2-preview-properties.h
 *
 * Author(s)
 * 	Jim Nelson <jim@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef GEXIV2_PREVIEW_PROPERTIES_H
#define GEXIV2_PREVIEW_PROPERTIES_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GEXIV2_TYPE_PREVIEW_PROPERTIES \
	(gexiv2_preview_properties_get_type ())
	
#define GEXIV2_PREVIEW_PROPERTIES(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), GEXIV2_TYPE_PREVIEW_PROPERTIES, GExiv2PreviewProperties))
	
#define GEXIV2_IS_PREVIEW_PROPERTIES(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEXIV2_TYPE_PREVIEW_PROPERTIES))
	
#define GEXIV2_PREVIEW_PROPERTIES_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), GEXIV2_TYPE_PREVIEW_PROPERTIES, GExiv2PreviewPropertiesClass))
	
#define GEXIV2_IS_PREVIEW_PROPERTIES_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), GEXIV2_TYPE_PREVIEW_PROPERTIES))
	
#define GEXIV2_PREVIEW_PROPERTIES_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), GEXIV2_TYPE_PREVIEW_PROPERTIES, GExiv2PreviewPropertiesClass))
	

typedef struct _GExiv2PreviewProperties			GExiv2PreviewProperties;
typedef struct _GExiv2PreviewPropertiesClass	GExiv2PreviewPropertiesClass;
typedef struct _GExiv2PreviewPropertiesPrivate	GExiv2PreviewPropertiesPrivate;

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GExiv2PreviewProperties, g_object_unref)

/**
 * GExiv2PreviewProperties:
 *
 * Properties of a preview image.
 *
 * Metadata can contain multiple preview images. [class@GExiv2.PreviewProperties] are
 * used to describe the available image without the need of fetching the whole
 * image from the metadata.
 */
struct _GExiv2PreviewProperties
{
	GObject parent_instance;

	/*< private >*/
	GExiv2PreviewPropertiesPrivate *priv;
};

struct _GExiv2PreviewPropertiesClass
{
	GObjectClass parent_class;
};

/* basic functions */

GType 			gexiv2_preview_properties_get_type			(void);


/* preview properties */

/**
 * gexiv2_preview_properties_get_mime_type:
 * @self: An instance of [class@GExiv2.PreviewProperties]
 *
 * Get the preview image's content-type.
 *
 * Returns: (transfer none): The preview image's MIME type.
 */
const gchar*	gexiv2_preview_properties_get_mime_type		(GExiv2PreviewProperties *self);

/**
 * gexiv2_preview_properties_get_extension:
 * @self: An instance of [class@GExiv2.PreviewProperties]
 *
 * Get the recommended file extension for the preview image.
 *
 * Returns: (transfer none): The recommended file extension for the preview image.
 */
const gchar*	gexiv2_preview_properties_get_extension		(GExiv2PreviewProperties *self);

/**
 * gexiv2_preview_properties_get_size:
 * @self: An instance of [class@GExiv2.PreviewProperties]
 *
 * Returns the size of the preview image in bytes.
 *
 * Returns: The preview image size in bytes.
 */
guint32			gexiv2_preview_properties_get_size			(GExiv2PreviewProperties *self);

/**
 * gexiv2_preview_properties_get_width:
 * @self: An instance of [class@GExiv2.PreviewProperties]
 *
 * Get the preview image's display width in pixels
 *
 * Returns: The preview image's display width in pixels.
 */
guint32			gexiv2_preview_properties_get_width			(GExiv2PreviewProperties *self);

/**
 * gexiv2_preview_properties_get_height:
 * @self: An instance of [class@GExiv2.PreviewProperties]
 *
 * Get the preview image's display height in pixels.
 *
 * Returns: The preview image's display height in pixels.
 */
guint32			gexiv2_preview_properties_get_height		(GExiv2PreviewProperties *self);


G_END_DECLS

#endif /* GEXIV2_PREVIEW_PROPERTIES_H */

