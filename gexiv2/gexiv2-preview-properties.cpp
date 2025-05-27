/*
 * gexiv2-preview-properties.cpp
 *
 * Author(s)
 * Jim Nelson <jim@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "gexiv2-preview-properties.h"
#include "gexiv2-preview-properties-private.h"
#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

using GExiv2PreviewPropertiesPrivate = struct _GExiv2PreviewPropertiesPrivate;

struct _GExiv2PreviewProperties {
    GObject parent_instance;

    GExiv2PreviewPropertiesPrivate* priv;
};
G_DEFINE_TYPE_WITH_PRIVATE(GExiv2PreviewProperties, gexiv2_preview_properties, G_TYPE_OBJECT);

static void gexiv2_preview_properties_finalize (GObject *object);

static void gexiv2_preview_properties_init (GExiv2PreviewProperties *self) {
    self->priv = (GExiv2PreviewPropertiesPrivate *) gexiv2_preview_properties_get_instance_private(self);

    self->priv->props = nullptr;
}

static void gexiv2_preview_properties_class_init (GExiv2PreviewPropertiesClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    
    gobject_class->finalize = gexiv2_preview_properties_finalize;
}

static void gexiv2_preview_properties_finalize (GObject *object) {
    GExiv2PreviewProperties *self = GEXIV2_PREVIEW_PROPERTIES (object);
    
    delete self->priv->props;
    
    G_OBJECT_CLASS (gexiv2_preview_properties_parent_class)->finalize (object);
}

GExiv2PreviewProperties* gexiv2_preview_properties_new (Exiv2::PreviewProperties &props) {
    GExiv2PreviewProperties* self = 
      GEXIV2_PREVIEW_PROPERTIES(g_object_new (GEXIV2_TYPE_PREVIEW_PROPERTIES, NULL));
    
    self->priv->props = new Exiv2::PreviewProperties(props);
    
    return self;
}

#define ACCESSOR(self, field, fail) \
    g_return_val_if_fail(GEXIV2_IS_PREVIEW_PROPERTIES(self), fail); \
    \
    return self->priv->props->field;

const gchar * gexiv2_preview_properties_get_mime_type (GExiv2PreviewProperties *self) {
    ACCESSOR(self, mimeType_.c_str(), NULL);
}

const gchar * gexiv2_preview_properties_get_extension (GExiv2PreviewProperties *self) {
    ACCESSOR(self, extension_.c_str(), NULL);
}

guint32 gexiv2_preview_properties_get_size (GExiv2PreviewProperties *self) {
    ACCESSOR(self, size_, 0);
}

guint32 gexiv2_preview_properties_get_width (GExiv2PreviewProperties *self) {
    ACCESSOR(self, width_, 0);
}

guint32 gexiv2_preview_properties_get_height (GExiv2PreviewProperties *self) {
    ACCESSOR(self, height_, 0);
}

Exiv2::PreviewProperties* gexiv2_preview_properties_get_impl(GExiv2PreviewProperties* self) {
    g_return_val_if_fail(GEXIV2_IS_PREVIEW_PROPERTIES(self), nullptr);

    return self->priv->props;
}
G_END_DECLS
