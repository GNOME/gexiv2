/*
 * gexiv2-metadata-iptc.cpp
 *
 * Author(s)
 *  Mike Gemuende <mike@gemuende.de>
 *  Jim Nelson <jim@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "gexiv2-metadata.h"
#include "gexiv2-metadata-private.h"
#include "gexiv2-util-private.h"

#include <string>
#include <glib-object.h>
#include <exiv2/exiv2.hpp>

G_BEGIN_DECLS

gboolean gexiv2_metadata_has_iptc (GExiv2Metadata *self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, FALSE);
    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);

    return !(priv->image->iptcData().empty());
}

gboolean gexiv2_metadata_has_iptc_tag(GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);

    Exiv2::IptcData& iptc_data = priv->image->iptcData();

    for (Exiv2::IptcData::iterator it = iptc_data.begin(); it != iptc_data.end(); ++it) {
        if (it->count() > 0 && g_ascii_strcasecmp(tag, it->key().c_str()) == 0)
            return TRUE;
    }
    
    return FALSE;
}

gboolean gexiv2_metadata_clear_iptc_tag(GExiv2Metadata *self, const gchar* tag) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);

    Exiv2::IptcData& iptc_data = priv->image->iptcData();

    gboolean erased = FALSE;

    Exiv2::IptcData::iterator it = iptc_data.begin();
    while (it != iptc_data.end()) {
        if (it->count() > 0 && g_ascii_strcasecmp(tag, it->key().c_str()) == 0) {
            it = iptc_data.erase(it);
            erased = TRUE;
        } else {
            it++;
        }
    }
    
    return erased;
}

void gexiv2_metadata_clear_iptc (GExiv2Metadata *self) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    auto* priv = gexiv2_priv(self);

    g_return_if_fail(priv->image.get() != nullptr);

    priv->image->iptcData().clear();
}

gchar** gexiv2_metadata_get_iptc_tags(GExiv2Metadata* self) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);

    // get a copy of the IptcData and sort it by key, preserving the original
    Exiv2::IptcData iptc_data = Exiv2::IptcData(priv->image->iptcData());
    detail::sortMetadata(iptc_data);

    GSList* list = nullptr;
    GSList* list_iter = nullptr;
    gchar** data = nullptr;
    gint count = 0;
    gchar* previous_tag = nullptr; // From previous iteration

    for (const auto& it : iptc_data) {
        if (it.count() > 0 && (previous_tag == nullptr || it.key() != previous_tag)) {
            list = g_slist_prepend(list, g_strdup(it.key().c_str()));
            count++;
            previous_tag = static_cast<gchar*>(list->data);
        }
    }

    data = g_new(gchar*, count + 1);
    data[count--] = nullptr;
    for (list_iter = list; list_iter != nullptr; list_iter = list_iter->next)
        data[count--] = static_cast<gchar*>(list_iter->data);

    g_slist_free(list);

    return data;
}

gchar* gexiv2_metadata_get_iptc_tag_string (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    try {
        const auto& iptc_data = priv->image->iptcData();
        const Exiv2::IptcKey key(tag);
        auto it = iptc_data.findKey(key);

        while (it != iptc_data.end() && it->count() == 0)
            it++;

        if (it != iptc_data.end()) {
        	std::ostringstream os;

        	// Iptc allows Repeatable tags (multi-value) and Non-Repeatable tags
        	// (single value). Repeatable tags are not grouped together, but exist as
        	// separate entries with the same tag name.
        	if (!Exiv2::IptcDataSets::dataSetRepeatable(key.tag(), key.record())) {
                return g_strdup (it->toString ().c_str ());
        	}
            const gchar* SEPARATOR = ", ";
            gboolean add_separator = FALSE;

            for (; it != iptc_data.end(); ++it) {
                if (it->key() == tag) {
                    if (add_separator == TRUE) {
                    	os << SEPARATOR;
                    }
                    os << it->toString();
                    add_separator = TRUE;
                }
            }
            return g_strdup (os.str().c_str());
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return nullptr;
}

gchar* gexiv2_metadata_get_iptc_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    try {
        const auto& iptc_data = priv->image->iptcData();
        const Exiv2::IptcKey key(tag);
        auto it = iptc_data.findKey(key);

        while (it != iptc_data.end() && it->count() == 0)
            it++;

        if (it != iptc_data.end()) {
        	std::ostringstream os;

        	// Iptc allows Repeatable tags (multi-value) and Non-Repeatable tags
        	// (single value). Repeatable tags are not grouped together, but exist as
        	// separate entries with the same tag name.
        	if (!Exiv2::IptcDataSets::dataSetRepeatable(key.tag(), key.record())) {
                it->write (os);
                return g_strdup (os.str().c_str());
        	}
            const gchar* SEPARATOR = ", ";
            gboolean add_separator = FALSE;

            for (; it != iptc_data.end(); ++it) {
                if (it->key() == tag) {
                    if (add_separator == TRUE) {
                    	os << SEPARATOR;
                    }
                    it->write(os);
                    add_separator = TRUE;
                }
            }
            return g_strdup (os.str().c_str());
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return nullptr;
}

gboolean gexiv2_metadata_set_iptc_tag_string (GExiv2Metadata *self, const gchar* tag,
    const gchar* value, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(value != nullptr, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, FALSE);
    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        const Exiv2::IptcKey key(tag);
        auto& iptc_data = priv->image->iptcData();

        // Iptc allows Repeatable tags (multi-value) and Non-Repeatable tags
    	// (single value). Repeatable tags are not grouped together, but exist as
    	// separate entries with the same tag name.
        if (!Exiv2::IptcDataSets::dataSetRepeatable(key.tag(), key.record())) {
            iptc_data[tag] = value;
            return TRUE;
        }

        // Repeatable values can be any type
        GError* internal_error = nullptr;
        const gchar* type = gexiv2_metadata_get_iptc_tag_type(tag, &internal_error);
        if (internal_error != nullptr) {
            g_propagate_error(error, internal_error);
            return FALSE;
        }

        // FIXME: Can type ever be nullptr without internal_error being non-null?
        if (type == nullptr)
            throw Exiv2::Error(Exiv2::ErrorCode::kerInvalidKey, tag);

        auto v = Exiv2::Value::create(Exiv2::TypeInfo::typeId(static_cast<const std::string>(type)));

        if (v->read(static_cast<const std::string>(value)) != 0 || iptc_data.add(key,v.get()) != 0)
         	return FALSE;

        return TRUE;
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return FALSE;
}

gchar** gexiv2_metadata_get_iptc_tag_multiple (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA(self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    GSList* list = nullptr;
    GSList* list_iter = nullptr;
    gchar** values = nullptr;
    gint count = 0;
    
    try {
        Exiv2::IptcData& iptc_data = priv->image->iptcData();
        Exiv2::IptcKey key (tag);
        for (Exiv2::IptcData::iterator it = iptc_data.begin(); it != iptc_data.end(); ++it) {
            if (it->count() > 0 && key.key () == it->key ()) {
                list = g_slist_prepend (list, g_strdup (it->toString ().c_str ()));
                count++;
            }
        }
        
        values = g_new (gchar*, count + 1);
        values[count--] = nullptr;
        for (list_iter = list; list_iter != NULL; list_iter = list_iter->next)
            values[count--] = static_cast<gchar*>(list_iter->data);
        
        g_slist_free (list);
        
        return values;
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    g_slist_free_full (list, g_free);

    return nullptr;
}

gboolean gexiv2_metadata_set_iptc_tag_multiple (GExiv2Metadata *self, const gchar* tag,
    const gchar** values, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(values != nullptr, FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, FALSE);
    g_return_val_if_fail(priv->image.get() != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    if (values[0] == nullptr)
        return TRUE;

    try {
        auto& iptc_data = priv->image->iptcData();

        // Iptc allows Repeatable tags (multi-value) and Non-Repeatable tags
    	// (single value). Repeatable tags are not grouped together, but exist as
    	// separate entries with the same tag name.

        /* first clear all existing ... */
        const Exiv2::IptcKey iptc_key(tag);
        auto iptc_it = iptc_data.begin();
        while (iptc_it != iptc_data.end()) {
            if (iptc_it->count() > 0 && iptc_key.key () == iptc_it->key ())
                iptc_it = iptc_data.erase (iptc_it);
            else
                ++iptc_it;
        }
        
        if (!Exiv2::IptcDataSets::dataSetRepeatable(iptc_key.tag(), iptc_key.record())) {
        	// Skip to last value and assign
            const gchar** val_it = values;
            while (*val_it != nullptr) {
                val_it++;
            }

            --val_it;
            iptc_data[tag] = static_cast<const std::string>(*val_it);

            return TRUE;
        }

        // Repeatable values can be any type
        GError* internal_error = nullptr;
        const gchar* type = gexiv2_metadata_get_iptc_tag_type(tag, &internal_error);
        if (internal_error != nullptr) {
            g_propagate_error(error, internal_error);
            return FALSE;
        }

        // FIXME: Can type ever be nullptr without error being non-null?
        if (type == nullptr)
            throw Exiv2::Error(Exiv2::ErrorCode::kerInvalidKey, tag);

        auto v = Exiv2::Value::create(Exiv2::TypeInfo::typeId(static_cast<const std::string>(type)));

        // Add each value separately
        const gchar** val_it = values;
        while (*val_it != nullptr) {

            if (v->read(static_cast<const std::string>(*val_it)) != 0 || iptc_data.add(iptc_key,v.get()) != 0) {
            	return FALSE;
            }
            val_it++;
        }

        return TRUE;
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return FALSE;
}

const gchar* gexiv2_metadata_get_iptc_tag_label (const gchar* tag, GError **error) {
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        Exiv2::IptcKey key (tag);
        return Exiv2::IptcDataSets::dataSetTitle (key.tag (), key.record ());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return NULL;
}

const gchar* gexiv2_metadata_get_iptc_tag_description (const gchar* tag, GError **error) {
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        Exiv2::IptcKey key (tag);
        return Exiv2::IptcDataSets::dataSetDesc (key.tag (), key.record ());
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return NULL;
}

const gchar* gexiv2_metadata_get_iptc_tag_type (const gchar* tag, GError **error) {
    g_return_val_if_fail(tag != NULL, NULL);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);
    
    try {
        Exiv2::IptcKey key (tag);
        return Exiv2::TypeInfo::typeName(Exiv2::IptcDataSets::dataSetType(key.tag(), key.record()));
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    return NULL;
}

gboolean gexiv2_metadata_iptc_tag_supports_multiple_values(const gchar* tag, GError** error) {
    g_return_val_if_fail(tag != nullptr, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    try {
        const Exiv2::IptcKey key(tag); // Check to see if @tag is valid
        return (Exiv2::IptcDataSets::dataSetRepeatable(key.tag(), key.record()) ? TRUE : FALSE);
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return FALSE;
}

GBytes* gexiv2_metadata_get_iptc_tag_raw (GExiv2Metadata *self, const gchar* tag, GError **error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), nullptr);
    g_return_val_if_fail(tag != nullptr, nullptr);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv != nullptr, nullptr);
    g_return_val_if_fail(priv->image.get() != nullptr, nullptr);
    g_return_val_if_fail(error == nullptr || *error == nullptr, nullptr);

    try {
        const auto& iptc_data = priv->image->iptcData();
        const Exiv2::IptcKey key(tag);
        auto it = iptc_data.findKey(key);

        while (it != iptc_data.end() && it->count() == 0)
            it++;

        if (it != iptc_data.end()) {
        	// Iptc allows Repeatable tags (multi-value) and Non-Repeatable tags
        	// (single value). Repeatable tags are not grouped together, but exist as
        	// separate entries with the same tag name.
        	if (!Exiv2::IptcDataSets::dataSetRepeatable(key.tag(), key.record())) {
                long size = it->size();
                if( size > 0 ) {
                    gpointer data = g_malloc(size);
                    it->copy(static_cast<Exiv2::byte*>(data), Exiv2::invalidByteOrder);
                    return g_bytes_new_take(data, size);
                }
        	}

        	// Create return string by extracting raw value from Exiv2 and
        	// adding separators (if needed).
            bool add_separator = false;

            // Define record separarator as 4x INFORMATION SEPARATOR FOUR
            constexpr guint8 SEPARATOR[] = {28, 28, 28, 28};
            constexpr int seperator_length = sizeof(SEPARATOR);

            GByteArray* concatenated_raw_arrays = g_byte_array_new();

            for (; it != iptc_data.end(); ++it) {
                auto size = it->size();
            	if (it->key() == tag && size > 0) {
                    if (add_separator) {
                        g_byte_array_append(concatenated_raw_arrays, SEPARATOR, seperator_length);
            		}
                    auto old_size = concatenated_raw_arrays->len;
                    g_byte_array_set_size(concatenated_raw_arrays, old_size + size);
                    it->copy(static_cast<Exiv2::byte*>(concatenated_raw_arrays->data + old_size),
                             Exiv2::invalidByteOrder);
                    add_separator = true;
                }
            }

            return g_byte_array_free_to_bytes(concatenated_raw_arrays);
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return nullptr;
}

G_END_DECLS
