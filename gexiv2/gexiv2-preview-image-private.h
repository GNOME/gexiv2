/*
 * gexiv2-preview-image-private.h
 *
 * Author(s)
 * Jim Nelson <jim@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef GEXIV2_PREVIEW_IMAGE_PRIVATE_H
#define GEXIV2_PREVIEW_IMAGE_PRIVATE_H

#include <gexiv2/gexiv2-preview-image.h>
#include <exiv2/exiv2.hpp>

G_BEGIN_DECLS

struct _GExiv2PreviewImagePrivate
{
    Exiv2::PreviewImage *image;
    gchar *mime_type;
    gchar *extension;
};

G_GNUC_INTERNAL GExiv2PreviewImage* gexiv2_preview_image_new(Exiv2::PreviewManager* manager,
                                                             const Exiv2::PreviewProperties& props,
                                                             GError** error);

G_END_DECLS

#endif /* GEXIV2_PREVIEW_IMAGE_PRIVATE_H */
