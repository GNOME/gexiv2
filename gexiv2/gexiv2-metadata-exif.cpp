/*
 * gexiv2-metadata-exif.cpp
 *
 * Author(s)
 *  Mike Gemuende <mike@gemuende.de>
 *  Jim Nelson <jim@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// config.h needs to be the first include
// clang-format off
#include <config.h>
// clang-format on

#include "gexiv2-metadata-private.h"
#include "gexiv2-metadata.h"
#include "gexiv2-util-private.h"

#include <exiv2/exiv2.hpp>
#include <glib-object.h>
#include <iostream>
#include <string>

G_BEGIN_DECLS

gboolean gexiv2_metadata_has_exif (GExiv2Metadata *self) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, FALSE);
    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);

    return !(priv->image->exifData().empty());
}

gboolean gexiv2_metadata_has_exif_tag(GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);

    Exiv2::ExifData& exif_data = priv->image->exifData();
    for (Exiv2::ExifData::iterator it = exif_data.begin(); it != exif_data.end(); ++it) {
        if (it->count() > 0 && g_ascii_strcasecmp(tag, it->key().c_str()) == 0)
            return TRUE;
    }
    
    return FALSE;
}

gboolean gexiv2_metadata_clear_exif_tag(GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);

    Exiv2::ExifData& exif_data = priv->image->exifData();

    gboolean erased = FALSE;

    Exiv2::ExifData::iterator it = exif_data.begin();
    while (it != exif_data.end()) {
        if (it->count() > 0 && g_ascii_strcasecmp(tag, it->key().c_str()) == 0) {
            it = exif_data.erase(it);
            erased = TRUE;
        } else {
            it++;
        }
    }
    
    return erased;
}

void gexiv2_metadata_clear_exif (GExiv2Metadata *self) {
    g_return_if_fail (GEXIV2_IS_METADATA (self));
    auto* priv = gexiv2_priv(self);

    g_return_if_fail(priv->image.get() != nullptr);

    priv->image->exifData().clear();
}

gchar** gexiv2_metadata_get_exif_tags(GExiv2Metadata* self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);

    // get a copy of the ExifData and sort it by tags, preserving sort of original
    Exiv2::ExifData exif_data = Exiv2::ExifData(priv->image->exifData());
    exif_data.sortByKey();
    // FIXME: Use detail::sortMetadata(exif_data);
    // Something is not right in ExifData that makes std::sort fail

    GSList* list = nullptr;
    GSList* list_iter = nullptr;
    gchar** data = nullptr;
    gint count = 0;
    
    for (Exiv2::ExifData::iterator it = exif_data.begin(); it != exif_data.end(); ++it) {
        if (it->count() > 0) {
            list = g_slist_prepend (list, g_strdup (it->key ().c_str ()));
            count++;
        }
    }
    
    data = g_new (gchar*, count + 1);
    data[count--] = nullptr;
    for (list_iter = list; list_iter != nullptr; list_iter = list_iter->next)
        data[count--] = static_cast<gchar*>(list_iter->data);

    g_slist_free (list);

    return data;
}

gchar* gexiv2_metadata_get_exif_tag_string (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    try {
        Exiv2::ExifData& exif_data = priv->image->exifData();

        Exiv2::ExifData::iterator it = exif_data.findKey(Exiv2::ExifKey(tag));
        while (it != exif_data.end() && it->count() == 0)
            it++;
        
        if (it != exif_data.end())
            return g_strdup (it->toString ().c_str ());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return nullptr;
}

gchar** gexiv2_metadata_get_exif_tag_multiple(GExiv2Metadata* self, const gchar* tag, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    gchar** array = nullptr;

    try {
        Exiv2::ExifData& exif_data = priv->image->exifData();
        auto it = exif_data.findKey(Exiv2::ExifKey(tag));

        while (it != exif_data.end() && it->count() == 0)
            it++;

        if (it != exif_data.end()) {
            array = g_new(gchar*, 2);
            array[0] = g_strdup(it->toString().c_str());
            array[1] = nullptr;

            return array;
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    array = g_new(gchar*, 1);
    array[0] = nullptr;

    return array;
}

gboolean gexiv2_metadata_set_exif_tag_multiple(GExiv2Metadata* self,
                                               const gchar* tag,
                                               const gchar** values,
                                               GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(values != nullptr, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, FALSE);
    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    try {
        Exiv2::ExifData& exif_data = priv->image->exifData();

        auto it = exif_data.findKey(Exiv2::ExifKey(tag));

        if (it != exif_data.end())
            exif_data.erase(it);

        if (values[0]) {
            // Exif tags only store one value, so find the last non NULL one
            const gchar** val_it = values;
            while (*val_it != nullptr) {
                ++val_it;
            }

            --val_it;

            exif_data[tag] = static_cast<const std::string>(*val_it);
        }
        return TRUE;
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return FALSE;
}

gchar* gexiv2_metadata_get_exif_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    try {
        Exiv2::ExifData& exif_data = priv->image->exifData();

        Exiv2::ExifData::iterator it = exif_data.findKey(Exiv2::ExifKey(tag));
        while (it != exif_data.end() && it->count() == 0)
            it++;
        
        if (it != exif_data.end()) {
            std::ostringstream os;
            auto value = it->getValue();
            const auto* comment_value = dynamic_cast<const Exiv2::CommentValue*>(value.get());
            if (comment_value == nullptr) {
                it->write(os, &exif_data);
            } else {
                os << comment_value->comment();
            }

            return g_strdup (os.str ().c_str ());
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return nullptr;
}

gboolean gexiv2_metadata_set_exif_tag_string (GExiv2Metadata *self, const gchar* tag, const gchar* value, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(tag != NULL, FALSE);
    g_return_val_if_fail(value != NULL, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        priv->image->exifData()[tag] = value;

        return TRUE;
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return FALSE;
}

glong gexiv2_metadata_get_exif_tag_long (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), 0);
    g_return_val_if_fail(tag != nullptr, 0);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, 0);
    g_return_val_if_fail(priv->image.get() != nullptr, 0);
    g_return_val_if_fail(error == nullptr || *error == nullptr, 0);

    try {
        Exiv2::ExifData& exif_data = priv->image->exifData();

        Exiv2::ExifData::iterator it = exif_data.findKey(Exiv2::ExifKey(tag));
        while (it != exif_data.end() && it->count() == 0)
            it++;
        if (it != exif_data.end())
            return static_cast<glong>(it->toInt64());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return 0;
}

gboolean gexiv2_metadata_set_exif_tag_long (GExiv2Metadata *self, const gchar* tag, glong value, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(tag != NULL, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        priv->image->exifData()[tag] = static_cast<int32_t>(value);

        return TRUE;
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return FALSE;
}

gboolean gexiv2_metadata_get_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint* nom,
    gint* den, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(nom != nullptr, FALSE);
    g_return_val_if_fail(den != nullptr, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, FALSE);
    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        Exiv2::ExifData& exif_data = priv->image->exifData();

        Exiv2::ExifData::iterator it = exif_data.findKey(Exiv2::ExifKey(tag));
        while (it != exif_data.end() && it->count() == 0)
            it++;
        
        if (it != exif_data.end()) {
            Exiv2::Rational r = it->toRational();
            *nom = r.first;
            *den = r.second;
            
            return TRUE;
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return FALSE;
}

gboolean gexiv2_metadata_set_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint nom,
    gint den, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(tag != NULL, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        Exiv2::Rational r;
        r.first = nom;
        r.second = den;
        priv->image->exifData()[tag] = r;

        return TRUE;
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return FALSE;
}

gboolean gexiv2_metadata_try_get_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint* nom,
    gint* den, GError **error) {
    return gexiv2_metadata_get_exif_tag_rational(self, tag, nom, den, error);
}

gboolean gexiv2_metadata_try_set_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint nom,
    gint den, GError **error) {
    return gexiv2_metadata_set_exif_tag_rational(self, tag, nom, den, error);
}

/**
 * gexiv2_metadata_get_exif_tag_rational_as_double:
 * @self: An instance of #GExiv2Metadata
 * @tag: Name of the tag to fetch
 * @def: Default value that is returned in error case
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * A convenience wrapper around gexiv2_metadata_get_exif_tag_rational() that
 * will convert the fraction into a floating point number.
 *
 * Returns: 0.0 if the nominator of the fraction is 0.0, @def if the fraction
 * would be a division by zero or the tag could not be read, the fraction as a
 * floating point value otherwise.
 */
gdouble gexiv2_metadata_get_exif_tag_rational_as_double(GExiv2Metadata* self,
                                                        const gchar* tag,
                                                        gdouble def,
                                                        GError** error) {
    gint nom = 0;
    gint den = 0;
    gboolean result = FALSE;

    result = gexiv2_metadata_get_exif_tag_rational(self, tag, &nom, &den, error);
    if (error && *error)
        return def;

    if (!result)
        return def;
    
    if (nom == 0.0) {
        return 0.0;
    }

    return (den != 0.0) ? (double) nom / (double) den : def;
}

const gchar* gexiv2_metadata_get_exif_tag_label (const gchar* tag, GError **error) {
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        Exiv2::ExifKey key(tag);
        return g_intern_string(key.tagLabel().c_str());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return NULL;
}

const gchar* gexiv2_metadata_get_exif_tag_description (const gchar* tag, GError **error) {
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        Exiv2::ExifKey key(tag);
        return g_intern_string(key.tagDesc().c_str());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return NULL;
}

const gchar* gexiv2_metadata_get_exif_tag_type (const gchar* tag, GError **error) {
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        Exiv2::ExifKey key(tag);
        return Exiv2::TypeInfo::typeName(key.defaultTypeId());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return NULL;
}

gboolean gexiv2_metadata_exif_tag_supports_multiple_values (const gchar* tag, GError **error) {
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    try {
    	// Exif does not support multiple values, but still check if @tag is valid
        const Exiv2::ExifKey key(tag);
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return FALSE;
}

GBytes* gexiv2_metadata_get_exif_tag_raw (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    try {
        Exiv2::ExifData& exif_data = priv->image->exifData();

        Exiv2::ExifData::iterator it = exif_data.findKey(Exiv2::ExifKey(tag));
        while (it != exif_data.end() && it->count() == 0)
            it++;

        if (it != exif_data.end()) {
            auto size = it->size();
            if( size > 0 ) {
                gpointer data = g_malloc(size);
                it->copy((Exiv2::byte*)data, Exiv2::invalidByteOrder);
                return g_bytes_new_take(data, size);
            }
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return nullptr;
}

GBytes * gexiv2_metadata_get_exif_data (GExiv2Metadata *self,
                                        GExiv2ByteOrder byte_order,
                                        GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    try {
        Exiv2::ExifData& exif_data = priv->image->exifData();

        if (exif_data.empty()) {
            return nullptr;
        }

        Exiv2::Blob blob;

        Exiv2::ExifParser::encode(blob,
                                  byte_order == GEXIV2_BYTE_ORDER_LITTLE ?
                                      Exiv2::littleEndian :
                                      Exiv2::bigEndian,
                                  exif_data);

        if (blob.size() <= 0) {
            return nullptr;
        }

        gpointer data = g_malloc0(blob.size());
        memcpy(data, blob.data(), blob.size());

        return g_bytes_new_take (data, blob.size());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return nullptr;
}

G_END_DECLS
