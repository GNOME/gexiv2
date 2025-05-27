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

#define GEXIV2_TYPE_PREVIEW_IMAGE (gexiv2_preview_image_get_type())

G_DECLARE_FINAL_TYPE(GExiv2PreviewImage, gexiv2_preview_image, GEXIV2, PREVIEW_IMAGE, GObject)

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

/* basic functions */

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
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Write the preview image to a file in @path
 *
 * Returns: The number of bytes written to the file.
 *
 * Since: 0.16.0
 */
size_t gexiv2_preview_image_write_file(GExiv2PreviewImage* self, const gchar* path, GError** error);

/**
 * gexiv2_preview_image_try_write_file:
 * @self: An instance of [class@GExiv2.PreviewImage]
 * @path: (in): The file path to write the preview image to.
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Write the preview image to a file in @path
 *
 * Returns: The number of bytes written to the file.
 *
 * Since: 0.14.0
 *
 * Deprecated: 0.16.0: Use [method@GExiv2.PreviewImage.write_file] instead.
 */
G_DEPRECATED_FOR(gexiv2_preview_image_write_file)
glong gexiv2_preview_image_try_write_file(GExiv2PreviewImage* self, const gchar* path, GError** error);

G_END_DECLS

#endif /* GEXIV2_PREVIEW_IMAGE_H */

