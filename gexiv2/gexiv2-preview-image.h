/*
 * gexiv2-preview-image.h
 *
 * Author(s)
 * 	Jim Nelson <jim@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef GEXIV2_PREVIEW_IMAGE_H
#define GEXIV2_PREVIEW_IMAGE_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GEXIV2_TYPE_PREVIEW_IMAGE \
    (gexiv2_preview_image_get_type())
	
#define GEXIV2_PREVIEW_IMAGE(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), GEXIV2_TYPE_PREVIEW_IMAGE, GExiv2PreviewImage))
	
#define GEXIV2_IS_PREVIEW_IMAGE(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEXIV2_TYPE_PREVIEW_IMAGE))
	
#define GEXIV2_PREVIEW_IMAGE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), GEXIV2_TYPE_PREVIEW_IMAGE, GExiv2PreviewImageClass))
	
#define GEXIV2_IS_PREVIEW_IMAGE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), GEXIV2_TYPE_PREVIEW_IMAGE))
	
#define GEXIV2_PREVIEW_IMAGE_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), GEXIV2_TYPE_PREVIEW_IMAGE, GExiv2PreviewImageClass))
	


typedef struct _GExiv2PreviewImage			GExiv2PreviewImage;
typedef struct _GExiv2PreviewImageClass		GExiv2PreviewImageClass;
typedef struct _GExiv2PreviewImagePrivate	GExiv2PreviewImagePrivate;

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GExiv2PreviewImage, g_object_unref)

/**
 * GExiv2PreviewImage:
 *
 * An accessor to the preview images contained in the image's metadata.
 *
 * This could be anything from a thumbnail to a full-sized camera development
 * of a RAW image.
 *
 * The `GExiv2PreviewImage` is obtained by calling gexiv2_metadata_get_preview_image()
 * with an instance of #GExiv2PreviewProperties that are describing the image
 * to be fetched.
 *
 * ```c
 * GExiv2PreviewProperties **properties, **it;
 * properties = it = gexiv2_metadata_get_preview_properties(metadata);
 *
 * while (*it) {
 *   preview_image = gexiv2_metadata_get_preview_image(metadata, *it);
 *   it++;
 * }
 * ```
 *
 */

struct _GExiv2PreviewImage
{
	GObject parent_instance;

	/*< private >*/
	GExiv2PreviewImagePrivate *priv;
};

struct _GExiv2PreviewImageClass
{
	GObjectClass parent_class;
};

/* basic functions */

GType 			gexiv2_preview_image_get_type			(void);

/* preview image properties */

/**
 * gexiv2_preview_image_get_data:
 * @self: An instance of [class@GExiv2.PreviewImage]
 * @size: (out) (skip): The size of the buffer holding the data
 *
 * Get a pointer to the image data of the preview image. The returned data is
 * owned by the preview image and must not be freed.
 *
 * Returns: (transfer none) (array length=size): The raw image data
 */
const guint8*	gexiv2_preview_image_get_data			(GExiv2PreviewImage *self, guint32 *size);

/**
 * gexiv2_preview_image_get_mime_type:
 * @self: An instance of [class@GExiv2.PreviewImage]
 *
 * Get the MIME type associated with the preview image.
 *
 * Returns: (transfer none): The preview image's MIME type.
 */
const gchar*	gexiv2_preview_image_get_mime_type		(GExiv2PreviewImage *self);

/**
 * gexiv2_preview_image_get_extension:
 * @self: An instance of [class@GExiv2.PreviewImage]
 *
 * Get the file extension commonly associated with the preview image
 *
 * Returns: (transfer none): The preview image's recommended file extension.
 */
const gchar*	gexiv2_preview_image_get_extension		(GExiv2PreviewImage *self);

/**
 * gexiv2_preview_image_get_width:
 * @self: An instance of [class@GExiv2.PreviewImage]
 *
 * Get the width in pixels
 *
 * Returns: The preview image's display width in pixels.
 */
guint32			gexiv2_preview_image_get_width			(GExiv2PreviewImage *self);

/**
 * gexiv2_preview_image_get_height:
 * @self: An instance of [class@GExiv2.PreviewImage]
 *
 * Get the height in pixels
 *
 * Returns: The preview image's display height in pixels.
 */
guint32			gexiv2_preview_image_get_height			(GExiv2PreviewImage *self);

/**
 * gexiv2_preview_image_write_file:
 * @self: An instance of [class@GExiv2.PreviewImage]
 * @path: (in): The file path to write the preview image to.
 *
 * Write the preview image to a file in @path
 *
 * Returns: The number of bytes written to the file.
 *
 * Deprecated: 0.14.0: Use [method@GExiv2.PreviewImage.try_write_file] instead.
 */
G_DEPRECATED_FOR(gexiv2_preview_image_try_write_file)
glong			gexiv2_preview_image_write_file			(GExiv2PreviewImage *self, const gchar *path);

/**
 * gexiv2_preview_image_try_write_file:
 * @self: An instance of [class@GExiv2.PreviewImage]
 * @path: (in): The file path to write the preview image to.
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Write the preview image to a file in @path
 *
 * Returns: The number of bytes written to the file.
 *
 * Since: 0.14.0
 */
glong gexiv2_preview_image_try_write_file(GExiv2PreviewImage* self, const gchar* path, GError** error);

G_END_DECLS

#endif /* GEXIV2_PREVIEW_IMAGE_H */

