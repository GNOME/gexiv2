/*
 * gexiv2-metadata-xmp.cpp
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
#include <string>

G_BEGIN_DECLS

gboolean gexiv2_metadata_has_xmp (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, FALSE);
    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);

    return !(priv->image->xmpData().empty());
}

void gexiv2_metadata_clear_xmp(GExiv2Metadata *self) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    auto* priv = gexiv2_priv(self);

    g_return_if_fail(priv->image.get() != nullptr);

    priv->image->xmpData().clear();
}

gchar *gexiv2_metadata_generate_xmp_packet(GExiv2Metadata *self,
    GExiv2XmpFormatFlags xmp_format_flags, guint32 padding, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), NULL);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, NULL);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    auto const& xmp_data = priv->image->xmpData();

    try {
        std::string packet;
        if (Exiv2::XmpParser::encode(packet, xmp_data, xmp_format_flags, padding) == 0) {
            return g_strdup(packet.c_str());
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return nullptr;
}

gchar *gexiv2_metadata_try_generate_xmp_packet(GExiv2Metadata *self,
    GExiv2XmpFormatFlags xmp_format_flags, guint32 padding, GError **error) {
    return gexiv2_metadata_generate_xmp_packet (self, xmp_format_flags, padding, error);
}

gchar *gexiv2_metadata_get_xmp_packet(GExiv2Metadata *self, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), NULL);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, NULL);

    try {
        return g_strdup(priv->image->xmpPacket().c_str());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return NULL;
}

gchar *gexiv2_metadata_try_get_xmp_packet(GExiv2Metadata *self, GError **error) {
    return gexiv2_metadata_get_xmp_packet (self, error);
}

gboolean gexiv2_metadata_has_xmp_tag(GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);

    Exiv2::XmpData& xmp_data = priv->image->xmpData();

    for (Exiv2::XmpData::iterator it = xmp_data.begin(); it != xmp_data.end(); ++it) {
        if (it->count() > 0 && g_ascii_strcasecmp(tag, it->key().c_str()) == 0)
            return TRUE;
    }
    
    return FALSE;
}

gboolean gexiv2_metadata_clear_xmp_tag(GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);

    Exiv2::XmpData& xmp_data = priv->image->xmpData();

    gboolean erased = FALSE;
    
    Exiv2::XmpData::iterator it = xmp_data.begin();
    while (it != xmp_data.end()) {
        if (it->count() > 0 && g_ascii_strcasecmp(tag, it->key().c_str()) == 0) {
            it = xmp_data.erase(it);
            erased = TRUE;
        } else {
            it++;
        }
    }
    
    return erased;
}

gchar** gexiv2_metadata_get_xmp_tags (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);

    // get a copy of the original XmpData and sort it by key, preserving the original
    Exiv2::XmpData xmp_data = Exiv2::XmpData(priv->image->xmpData());
    detail::sortMetadata(xmp_data);

    GSList* list = nullptr;
    GSList* list_iter = nullptr;
    gchar** data = nullptr;
    gint count = 0;
    
    for (Exiv2::XmpData::iterator it = xmp_data.begin(); it != xmp_data.end(); ++it) {
        if (it->count() > 0) {
            list = g_slist_prepend (list, g_strdup (it->key ().c_str ()));
            count++;
        }
    }

    data = g_new (gchar*, count + 1);
    data[count--] = nullptr;
    for (list_iter = list; list_iter != nullptr; list_iter = list_iter->next) {
        data[count--] = static_cast<gchar*>(list_iter->data);
    }

    g_slist_free (list);

    return data;
}

gchar* gexiv2_metadata_get_xmp_tag_string (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    try {
        Exiv2::XmpData& xmp_data = priv->image->xmpData();

        Exiv2::XmpData::iterator it = xmp_data.findKey(Exiv2::XmpKey(tag));
        while (it != xmp_data.end() && it->count() == 0)
            it++;
        
        if (it != xmp_data.end())
            return g_strdup (it->toString ().c_str ());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return nullptr;
}

gchar* gexiv2_metadata_get_xmp_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    try {
        Exiv2::XmpData& xmp_data = priv->image->xmpData();

        Exiv2::XmpData::iterator it = xmp_data.findKey(Exiv2::XmpKey(tag));
        while (it != xmp_data.end() && it->count() == 0)
            it++;
        
        if (it != xmp_data.end()) {
            std::ostringstream os;
            it->write (os);
            
            return g_strdup (os.str ().c_str ());
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return nullptr;
}

gboolean gexiv2_metadata_set_xmp_tag_struct (GExiv2Metadata *self, const gchar* tag, GExiv2StructureType type, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(tag != NULL, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    Exiv2::XmpTextValue tv("");
    Exiv2::XmpData& xmp_data = priv->image->xmpData();

    switch (type) {
      case GEXIV2_STRUCTURE_XA_NONE:
        tv.read("");  // Clear the value
        tv.setXmpArrayType(Exiv2::XmpValue::xaNone);
        break;
      case GEXIV2_STRUCTURE_XA_ALT:
        tv.read("");
        tv.setXmpArrayType(Exiv2::XmpValue::xaAlt);
        break;
      case GEXIV2_STRUCTURE_XA_BAG:
        tv.read("");
        tv.setXmpArrayType(Exiv2::XmpValue::xaBag);
        break;
      case GEXIV2_STRUCTURE_XA_SEQ:
        tv.read("");
        tv.setXmpArrayType(Exiv2::XmpValue::xaSeq);
        break;
      case GEXIV2_STRUCTURE_XA_LANG:
      default:
        g_set_error_literal(error, g_quark_from_string("GExiv2"), 0, "Invalid structure type.");
        return FALSE;
        break;
    }

    try {
        xmp_data.add(Exiv2::XmpKey(tag), &tv);
        return TRUE;
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return FALSE;
}

gboolean gexiv2_metadata_try_set_xmp_tag_struct (GExiv2Metadata *self, const gchar* tag, GExiv2StructureType type, GError **error) {
    return gexiv2_metadata_set_xmp_tag_struct (self, tag, type, error);
}

gboolean gexiv2_metadata_set_xmp_tag_string (GExiv2Metadata *self, const gchar* tag, 
    const gchar* value, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(tag != NULL, FALSE);
    g_return_val_if_fail(value != NULL, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        priv->image->xmpData()[tag] = value;

        return TRUE;
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return FALSE;
}

glong gexiv2_metadata_get_xmp_tag_long (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), 0);
    g_return_val_if_fail(tag != nullptr, 0);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, 0);
    g_return_val_if_fail(priv->image.get() != nullptr, 0);
    g_return_val_if_fail(error == nullptr || *error == nullptr, 0);

    try {
        Exiv2::XmpData& xmp_data = priv->image->xmpData();

        Exiv2::XmpData::iterator it = xmp_data.findKey(Exiv2::XmpKey(tag));
        while (it != xmp_data.end() && it->count() == 0)
            it++;

        if (it != xmp_data.end())
            return static_cast<glong>(it->toInt64());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return 0;
}

gboolean gexiv2_metadata_set_xmp_tag_long (GExiv2Metadata *self, const gchar* tag, glong value, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(tag != NULL, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        priv->image->xmpData()[tag] = value;

        return TRUE;
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return FALSE;
}

gchar** gexiv2_metadata_get_xmp_tag_multiple(GExiv2Metadata* self, const gchar* tag, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    gchar** array = nullptr; // Return value

    try {
        Exiv2::XmpData& xmp_data = priv->image->xmpData();

        const Exiv2::XmpKey key = Exiv2::XmpKey(tag);
        auto it = xmp_data.findKey(key);

        while (it != xmp_data.end() && it->count() == 0 && it->key() != key.key())
            it++;

        if (it != xmp_data.end()) {
            if (it->typeId() == Exiv2::TypeId::xmpText) {
                // xmpText tags only have single value
                array = g_new(gchar*, 2);
                array[1] = nullptr;

                array[0] = g_strdup(it->toString().c_str());
            } else if (it->typeId() == Exiv2::TypeId::langAlt) {
                // For langAlt types, it->count() returns the number of
                // items but it->toString(i) ONLY returns the default
                // value (if any) minus the "lang=x-default " prefix.
                //
                // Instead use it->toString() and parse the result to
                // create the return array.
                // (Issue #61 - https://gitlab.gnome.org/GNOME/gexiv2/-/issues/61)

                auto num_items = it->count();

                if (!num_items) {
                    // Empty string
                    array = g_new(gchar*, 2);
                    array[1] = nullptr;
                    array[0] = g_strdup("");
                } else {
                    const int SEPARATOR = 2; // ", "
                    const std::string temp = it->toString();
                    std::string::size_type pos1 = 0;
                    std::string::size_type pos2 = temp.find(", lang=", pos1);

                    array = g_new(gchar*, num_items + 1);
                    array[num_items] = nullptr;

                    for (decltype(num_items) i = 0; i < num_items; i++) {
                        array[i] = g_strdup(temp.substr(pos1, pos2 - pos1).c_str());
                        pos1 = pos2 + SEPARATOR;
                        pos2 = temp.find(',', pos1);
                    }
                }
            } else {
                // For Xmp structures, cycle through all elements and
                // add to return array

                auto num_items = it->count();

                array = g_new(gchar*, num_items + 1);
                array[num_items] = nullptr;

                for (decltype(num_items) i = 0; i < num_items; i++)
                    array[i] = g_strdup(it->toString(i).c_str());
            }
            return array;
        }
    } catch (Exiv2::Error& e) {
        if (array) {
            g_strfreev(array);
        }
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    array = g_new(gchar*, 1);
    array[0] = nullptr;

    return array;
}

gchar** gexiv2_metadata_get_xmp_tag_multiple_deprecated (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    try {
        Exiv2::XmpData& xmp_data = priv->image->xmpData();

        Exiv2::XmpKey key = Exiv2::XmpKey(tag);
        Exiv2::XmpData::iterator it = xmp_data.findKey(key);

        while (it != xmp_data.end() && it->count() == 0 && it->key() != key.key())
            it++;

        if (it != xmp_data.end()) {
            auto size = it->count ();
            gchar **array = g_new (gchar*, size + 1);
            array[size] = nullptr;

            for (decltype(size) i = 0; i < size; i++)
                array[i] = g_strdup (it->toString (i).c_str ());

            return array;
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    gchar **array = g_new (gchar*, 1);
    array[0] = nullptr;

    return array;
}

gboolean gexiv2_metadata_set_xmp_tag_multiple (GExiv2Metadata *self, const gchar* tag, 
    const gchar** values, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(values != nullptr, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, FALSE);
    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        Exiv2::XmpData& xmp_data = priv->image->xmpData();

        /* first clear existing tag */
        Exiv2::XmpData::iterator it = xmp_data.findKey(Exiv2::XmpKey(tag));
        while (it != xmp_data.end() && it->count() == 0)
            it++;
        
        if (it != xmp_data.end())
            xmp_data.erase (it);
        
        /* ... and then set the others */
        const gchar **val_it = values;
        while (*val_it != nullptr) {
            xmp_data[tag] = static_cast<const std::string> (*val_it);
            ++val_it;
        }

        return TRUE;
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return FALSE;
}

const gchar* gexiv2_metadata_get_xmp_tag_label (const gchar* tag, GError **error) {
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        return Exiv2::XmpProperties::propertyTitle(Exiv2::XmpKey(tag));
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return NULL;
}

const gchar* gexiv2_metadata_get_xmp_tag_description (const gchar* tag, GError **error) {
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        return Exiv2::XmpProperties::propertyDesc(Exiv2::XmpKey(tag));
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return NULL;
}

const gchar* gexiv2_metadata_get_xmp_tag_type (const gchar* tag, GError **error) {
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        return Exiv2::TypeInfo::typeName(Exiv2::XmpProperties::propertyType(Exiv2::XmpKey(tag)));
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return NULL;
}

gboolean gexiv2_metadata_xmp_tag_supports_multiple_values(GExiv2Metadata* self, const gchar* tag, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, FALSE);
    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    try {
        const Exiv2::XmpKey key(tag); // Check tag is in correct format
        GError* internal_error = nullptr;
        const gchar* type = gexiv2_metadata_get_xmp_tag_type(tag, &internal_error);

        if (internal_error != nullptr) {
            g_propagate_error(error, internal_error);
            return FALSE;
        }

        // Fixme: Can get_xmp_tag_type ever return NULL without an error being set?
        if (type == nullptr)
            throw Exiv2::Error(Exiv2::ErrorCode::kerInvalidKey, tag);

        // If @tag has a valid familyName and groupName, Exiv2 will return
        // "XmpText" even if the tagName has never been added (e.g.
        // "Xmp.dc.TagDoesNotExist").
        // For consistency with the `_supports_multiple_values` Exif and Iptc functions,
        // check if @tag exists - Note: all built-in tags have a label.
        const auto& xmp_data = priv->image->xmpData();

        if (g_ascii_strcasecmp(type, "XmpText") == 0 && gexiv2_metadata_get_xmp_tag_label(tag, error) == nullptr &&
            xmp_data.findKey(key) == xmp_data.end()) {
            throw Exiv2::Error(Exiv2::ErrorCode::kerInvalidKey, tag);
        }

        if (g_ascii_strcasecmp(type, "XmpAlt") == 0 || g_ascii_strcasecmp(type, "XmpBag") == 0 ||
            g_ascii_strcasecmp(type, "XmpSeq") == 0 || g_ascii_strcasecmp(type, "LangAlt") == 0) {
            return TRUE;
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    return FALSE;
}

GBytes* gexiv2_metadata_get_xmp_tag_raw (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    try {
        Exiv2::XmpData& xmp_data = priv->image->xmpData();

        Exiv2::XmpData::iterator it = xmp_data.findKey(Exiv2::XmpKey(tag));
        while (it != xmp_data.end() && it->count() == 0)
            it++;

        if (it != xmp_data.end()) {
            long size = it->size();
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

gboolean gexiv2_metadata_try_register_xmp_namespace (const gchar* name, const gchar* prefix, GError **error) {
    return gexiv2_metadata_register_xmp_namespace(name, prefix, error);
}

gboolean gexiv2_metadata_register_xmp_namespace(const gchar* name, const gchar* prefix, GError** error) {
    g_return_val_if_fail(name != nullptr, FALSE);
    g_return_val_if_fail(prefix != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    try {
        Exiv2::XmpProperties::ns(prefix);
    } catch (Exiv2::Error& e1) {
        // No namespace, OK to register
        try {
            Exiv2::XmpProperties::registerNs(name, prefix);
            return TRUE;
        } catch (Exiv2::Error& e2) {
            error << e2;
        }
    }

    return FALSE;
}

gboolean gexiv2_metadata_try_unregister_xmp_namespace (const gchar* name, GError **error) {
    return gexiv2_metadata_unregister_xmp_namespace(name, error);
}

gboolean gexiv2_metadata_unregister_xmp_namespace(const gchar* name, GError** error) {
    g_return_val_if_fail(name != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    try {
        std::string prefix = Exiv2::XmpProperties::prefix(name);

        if (!prefix.empty()) {
            // Unregister
            Exiv2::XmpProperties::unregisterNs(name);

            try {
                Exiv2::XmpProperties::ns(prefix);
            } catch (Exiv2::Error& e1) {
                // Namespace successfully removed
                return TRUE;
            } catch (std::exception& e) {
                error << e;
            }
        }
    } catch (Exiv2::Error& e2) {
        error << e2;
    } catch (std::exception& e) {
        error << e;
    }
    return FALSE;
}

void gexiv2_metadata_try_unregister_all_xmp_namespaces (GError **error) {
    gexiv2_metadata_unregister_all_xmp_namespaces(error);
}

void gexiv2_metadata_unregister_all_xmp_namespaces(GError** error) {
    g_return_if_fail(error == nullptr || *error == nullptr);

    try {
        Exiv2::XmpProperties::unregisterNs();
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
}

char* gexiv2_metadata_try_get_xmp_namespace_for_tag(const char* tag, GError **error) {
    return gexiv2_metadata_get_xmp_namespace_for_tag(tag, error);
}

char* gexiv2_metadata_get_xmp_namespace_for_tag(const char* tag, GError** error) {
    g_return_val_if_fail(tag != nullptr, nullptr);
    g_return_val_if_fail(strlen(tag) != 0, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    char** list = nullptr;
    char* result = nullptr;

    try {
        list = g_strsplit(tag, ".", 3);
        const char* name = nullptr;
        if (g_strv_length(list) == 0) {
            g_assert_not_reached();
        } else if (g_strv_length(list) == 1) {
            // @tag is in the format "groupName"
            name = list[0];
        } else if (g_strv_length(list) == 3) {
            // Check @tag is in the format "Xmp.groupName.tagName"
            if (g_strcmp0(list[0], "Xmp") != 0 || strlen(list[1]) == 0 || strlen(list[2]) == 0)
                throw Exiv2::Error(Exiv2::ErrorCode::kerInvalidKey, tag);
            name = list[1];
        } else {
            throw Exiv2::Error(Exiv2::ErrorCode::kerInvalidKey, tag);
        }

        auto info = Exiv2::XmpProperties::ns(name);

        result = g_strdup(info.c_str());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    g_clear_pointer(&list, g_strfreev);

    return result;
}

G_END_DECLS
