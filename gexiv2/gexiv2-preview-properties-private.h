/*
 * gexiv2-preview-properties-private.h
 *
 * Author(s)
 * Jim Nelson <jim@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef GEXIV2_PREVIEW_PROPERTIES_PRIVATE_H
#define GEXIV2_PREVIEW_PROPERTIES_PRIVATE_H

#include <gexiv2/gexiv2-preview-properties.h>
#include <exiv2/exiv2.hpp>

G_BEGIN_DECLS

struct _GExiv2PreviewPropertiesPrivate
{
  Exiv2::PreviewProperties *props;
};


G_GNUC_INTERNAL GExiv2PreviewProperties* gexiv2_preview_properties_new (Exiv2::PreviewProperties &props);
G_GNUC_INTERNAL void gexiv2_preview_properties_free(GExiv2PreviewProperties *self);

G_END_DECLS

#endif /* GEXIV2_PREVIEW_PROPERTIES_PRIVATE_H */
