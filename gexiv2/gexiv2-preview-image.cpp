/*
 * gexiv2-preview-image.cpp
 *
 * Author(s)
 * Jim Nelson <jim@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "gexiv2-preview-image.h"
#include "gexiv2-preview-image-private.h"
#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

G_DEFINE_TYPE_WITH_CODE (GExiv2PreviewImage, gexiv2_preview_image, G_TYPE_OBJECT, G_ADD_PRIVATE(GExiv2PreviewImage));

static void gexiv2_preview_image_finalize (GObject *object);

static void gexiv2_preview_image_init (GExiv2PreviewImage *self) {
    self->priv = (GExiv2PreviewImagePrivate *) gexiv2_preview_image_get_instance_private(self);
    
    self->priv->image = NULL;
    self->priv->mime_type = NULL;
    self->priv->extension = NULL;
}

static void gexiv2_preview_image_class_init (GExiv2PreviewImageClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    
    gobject_class->finalize = gexiv2_preview_image_finalize;
}

static void gexiv2_preview_image_finalize (GObject *object) {
    GExiv2PreviewImage *self = GEXIV2_PREVIEW_IMAGE (object);
    
    delete self->priv->image;
    g_free(self->priv->mime_type);
    g_free(self->priv->extension);
    
    G_OBJECT_CLASS (gexiv2_preview_image_parent_class)->finalize (object);
}

GExiv2PreviewImage* gexiv2_preview_image_new(Exiv2::PreviewManager* manager,
                                             const Exiv2::PreviewProperties& props,
                                             GError** error) {
    g_return_val_if_fail(manager != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    GExiv2PreviewImage* self = GEXIV2_PREVIEW_IMAGE(g_object_new(GEXIV2_TYPE_PREVIEW_IMAGE, nullptr));
    try {
        self->priv->image = new Exiv2::PreviewImage(manager->getPreviewImage(props));
        self->priv->mime_type = g_strdup(self->priv->image->mimeType().c_str());
        self->priv->extension = g_strdup(self->priv->image->extension().c_str());

        return self;
    } catch (Exiv2::Error& e) {
        // Cleanup
        if (self->priv->image)
            delete self->priv->image;
        if (self->priv->mime_type)
            g_free(self->priv->mime_type);
        if (self->priv->extension)
            g_free(self->priv->extension);

        g_object_unref(self);

        g_set_error_literal(error, g_quark_from_string("GExiv2"), static_cast<int>(e.code()), e.what());
    }
    return nullptr;
}

void gexiv2_preview_image_free(GExiv2PreviewImage *self) {
    g_return_if_fail(GEXIV2_IS_PREVIEW_IMAGE(self));
    
    g_object_unref(self);
}

const guint8* gexiv2_preview_image_get_data (GExiv2PreviewImage *self, guint32 *size) {
    g_return_val_if_fail(GEXIV2_IS_PREVIEW_IMAGE(self), nullptr);
    g_return_val_if_fail(self->priv != nullptr, nullptr);
    g_return_val_if_fail(self->priv->image != nullptr, nullptr);
    g_return_val_if_fail(size != nullptr, nullptr);

    *size = self->priv->image->size();
    
    return self->priv->image->pData();
}

const gchar * gexiv2_preview_image_get_mime_type (GExiv2PreviewImage *self) {
    g_return_val_if_fail(GEXIV2_IS_PREVIEW_IMAGE(self), nullptr);
    g_return_val_if_fail(self->priv != nullptr, nullptr);

    return self->priv->mime_type;
}

const gchar *gexiv2_preview_image_get_extension (GExiv2PreviewImage *self) {
    g_return_val_if_fail(GEXIV2_IS_PREVIEW_IMAGE(self), nullptr);
    g_return_val_if_fail(self->priv != nullptr, nullptr);

    return self->priv->extension;
}

guint32 gexiv2_preview_image_get_width (GExiv2PreviewImage *self) {
    g_return_val_if_fail(GEXIV2_IS_PREVIEW_IMAGE(self), 0);
    g_return_val_if_fail(self->priv != nullptr, 0);
    g_return_val_if_fail(self->priv->image != nullptr, 0);

    return self->priv->image->width();
}

guint32 gexiv2_preview_image_get_height (GExiv2PreviewImage *self) {
    g_return_val_if_fail(GEXIV2_IS_PREVIEW_IMAGE(self), 0);
    g_return_val_if_fail(self->priv != nullptr, 0);
    g_return_val_if_fail(self->priv->image != nullptr, 0);

    return self->priv->image->height();
}

glong gexiv2_preview_image_write_file (GExiv2PreviewImage *self, const gchar *path) {
    GError* error = nullptr;
    glong value = -1;

    value = gexiv2_preview_image_try_write_file(self, path, &error);

    if (error) {
        g_warning("%s", error->message);
        g_clear_error(&error);
    }

    return value;
}

glong gexiv2_preview_image_try_write_file(GExiv2PreviewImage* self, const gchar* path, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_PREVIEW_IMAGE(self), -1);
    g_return_val_if_fail(self->priv != nullptr, -1);
    g_return_val_if_fail(self->priv->image != nullptr, -1);
    g_return_val_if_fail(path != nullptr && strlen(path) > 0, -1);

    try {
        return self->priv->image->writeFile(path);
    } catch (Exiv2::Error& e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), static_cast<int>(e.code()), e.what());
    }
    return -1;
}

G_END_DECLS

