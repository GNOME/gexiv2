/*
 * gexiv2-metadata-iptc.cpp
 *
 * Author(s)
 * 	Mike Gemuende <mike@gemuende.de>
 * 	Jim Nelson <jim@yorba.org>
 *
 * This is free software. See COPYING for details.
 */

#include "gexiv2-metadata.h"
#include "gexiv2-metadata-private.h"
#include <string>
#include <glib-object.h>
#include <exiv2/iptc.hpp>

G_BEGIN_DECLS

void
gexiv2_metadata_set_iptc_tag_string_cut (GExiv2Metadata *self, const gchar* tag, const gchar* value, gsize bytes);


gboolean
gexiv2_metadata_has_iptc (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	return ! (self->priv->iptc_data.empty ());
}


gboolean
gexiv2_metadata_has_iptc_tag(GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail(GEXIV2_IS_METADATA(self), false);
	g_return_val_if_fail(tag != NULL, false);
	
	Exiv2::IptcData &iptc_data = self->priv->iptc_data;
	for (Exiv2::IptcData::iterator it = iptc_data.begin(); it != iptc_data.end(); ++it) {
		if (g_ascii_strcasecmp(tag, it->key().c_str()) == 0)
			return true;
	}
	
	return false;
}


void
gexiv2_metadata_clear_iptc_tag(GExiv2Metadata *self, const gchar* tag)
{
	g_return_if_fail(GEXIV2_IS_METADATA(self));
	g_return_if_fail(tag != NULL);
	
	Exiv2::IptcData &iptc_data = self->priv->iptc_data;
	Exiv2::IptcData::iterator it = iptc_data.begin();
	while (it != iptc_data.end()) {
		if (g_ascii_strcasecmp(tag, it->key().c_str()) == 0)
			it = iptc_data.erase(it);
		else
			it++;
	}
}


void
gexiv2_metadata_clear_iptc (GExiv2Metadata *self)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));

	self->priv->iptc_data.clear ();
}


gchar** gexiv2_metadata_get_iptc_tags (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	GSList *list = NULL;
	GSList *list_iter;
	gchar** data;
	gint count = 0;
	Exiv2::IptcData iptc_data (self->priv->iptc_data);
	
	iptc_data.sortByKey ();
	
	for (Exiv2::IptcData::iterator it = iptc_data.begin(); it != iptc_data.end(); ++it) {        
		list = g_slist_prepend (list, g_strdup (it->key ().c_str ()));
		count = count + 1;
	}
	
	data = g_new (gchar*, count + 1);
	data[count --] = NULL;
	for (list_iter = list; list_iter != NULL; list_iter = list_iter->next)
		data[count--] = static_cast<gchar*>(list_iter->data);

	g_slist_free (list);

	return data;
}


gchar*
gexiv2_metadata_get_iptc_tag_string (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
		
	Exiv2::IptcData& iptc_data = self->priv->iptc_data;
	
	try {
		
		Exiv2::IptcKey key (tag);
		Exiv2::IptcData::iterator it = iptc_data.findKey (key);
		if (it != iptc_data.end ())
			return g_strdup (it->toString ().c_str ());
		
		return g_strdup ("");
	
	} catch (Exiv2::Error& e) {
	}
	
	return NULL;
}


gchar*
gexiv2_metadata_get_iptc_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	Exiv2::IptcData& iptc_data = self->priv->iptc_data;
	
	try {
		
		Exiv2::IptcKey key (tag);
		Exiv2::IptcData::iterator it = iptc_data.findKey (key);
		
		if (it != iptc_data.end ()) {
			std::ostringstream os;
        	it->write (os);
			return g_strdup (os.str ().c_str ());
		}
		
		return g_strdup ("");
		
	} catch (Exiv2::Error& e) {
	}
	
	return NULL;
}


gboolean
gexiv2_metadata_set_iptc_tag_string (GExiv2Metadata *self, const gchar* tag, const gchar* value)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	try {
		
		self->priv->iptc_data[tag] = value;
		
		return true;
		
	} catch (Exiv2::Error& e) {
	}
	
	return false;
}


gchar**
gexiv2_metadata_get_iptc_tag_multiple (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	GSList *list = NULL;
	GSList *list_iter;
	gchar** values;
	gint count = 0;
	Exiv2::IptcData& iptc_data = self->priv->iptc_data;
	
	try {
		
		Exiv2::IptcKey key (tag);
		
		for (Exiv2::IptcData::iterator it = iptc_data.begin(); it != iptc_data.end(); ++it) {
			
			if (key.key () == it->key ()) {
				list = g_slist_prepend (list, g_strdup (it->toString ().c_str ()));
				count ++;
			}
		}
		
		values = g_new (gchar*, count + 1);
		values[count --] = NULL;
		for (list_iter = list; list_iter != NULL; list_iter = list_iter->next)
			values[count--] = static_cast<gchar*>(list_iter->data);
	
		g_slist_free (list);
	
		return values;
		
	} catch (Exiv2::Error& e) {
	}
	
	return NULL;
}


gboolean
gexiv2_metadata_set_iptc_tag_multiple (GExiv2Metadata *self, const gchar* tag, const gchar** values)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	try {
		Exiv2::IptcData& iptc_data = self->priv->iptc_data;
		
		/* first clear all existing ... */
		Exiv2::IptcKey iptc_key (tag);
		Exiv2::IptcData::iterator iptc_it = iptc_data.begin();		
		while (iptc_it != iptc_data.end()) {
			if (iptc_key.key () == iptc_it->key ())
				iptc_it = iptc_data.erase (iptc_it);
			else
				++ iptc_it;
		}
		
		/* ... and then set the others */
		Exiv2::Value::AutoPtr iptc_value = Exiv2::Value::create(Exiv2::string);
			
		const gchar **it = values;
		while (*it != NULL) {
			iptc_value->read (static_cast<const std::string>(*it));
			iptc_data.add (iptc_key, iptc_value.get());
			
			++ it;
		}
		
		return true;
	} catch (Exiv2::Error& e) {
	}
	
	return false;
}


gchar**
gexiv2_metadata_get_iptc_keywords (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_multiple (self, "Iptc.Application2.Keywords");
}


gchar*
gexiv2_metadata_get_iptc_headline (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.Headline");
}


gchar*
gexiv2_metadata_get_iptc_caption (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.Caption");
}


gchar*
gexiv2_metadata_get_iptc_byline (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.Byline");
}


gchar*
gexiv2_metadata_get_iptc_byline_title (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.BylineTitle");
}


gchar*
gexiv2_metadata_get_iptc_copyright (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.Copyright");
}


gchar*
gexiv2_metadata_get_iptc_contact (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.Contact");
}


gchar*
gexiv2_metadata_get_iptc_city (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.City");
}


gchar*
gexiv2_metadata_get_iptc_sublocation (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.SubLocation");
}


gchar*
gexiv2_metadata_get_iptc_province_state (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.ProvinceState");
}


gchar*
gexiv2_metadata_get_iptc_country_code (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.CountryCode");
}


gchar*
gexiv2_metadata_get_iptc_country_name (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.CountryName");
}


gchar*
gexiv2_metadata_get_iptc_writer (GExiv2Metadata *self)
{
	return gexiv2_metadata_get_iptc_tag_string (self, "Iptc.Application2.Writer");
}


void
gexiv2_metadata_set_iptc_tag_string_cut (GExiv2Metadata *self, const gchar* tag, const gchar* value, gsize bytes)
{
	gchar* cut_value = g_strndup (value, bytes);
	gexiv2_metadata_set_iptc_tag_string (self, tag, cut_value);
	g_free (cut_value);
}


void
gexiv2_metadata_set_iptc_byline (GExiv2Metadata *self, const gchar* value)
{
	gexiv2_metadata_set_iptc_tag_string_cut (self, "Iptc.Application2.Byline", value, 32);
}


void
gexiv2_metadata_set_iptc_byline_title (GExiv2Metadata *self, const gchar* value)
{
	gexiv2_metadata_set_iptc_tag_string_cut (self, "Iptc.Application2.BylineTitle", value, 32);
}


void
gexiv2_metadata_set_iptc_copyright (GExiv2Metadata *self, const gchar* value)
{
	gexiv2_metadata_set_iptc_tag_string_cut (self, "Iptc.Application2.Copyright", value, 128);
}


void
gexiv2_metadata_set_iptc_contact (GExiv2Metadata *self, const gchar* value)
{
	gexiv2_metadata_set_iptc_tag_string_cut (self, "Iptc.Application2.Contact", value, 128);
}


void
gexiv2_metadata_set_iptc_city (GExiv2Metadata *self, const gchar* value)
{
	gexiv2_metadata_set_iptc_tag_string_cut (self, "Iptc.Application2.City", value, 32);
}


void
gexiv2_metadata_set_iptc_sublocation (GExiv2Metadata *self, const gchar* value)
{
	gexiv2_metadata_set_iptc_tag_string_cut (self, "Iptc.Application2.SubLocation", value, 32);
}


void
gexiv2_metadata_set_iptc_province_state (GExiv2Metadata *self, const gchar* value)
{
	gexiv2_metadata_set_iptc_tag_string_cut (self, "Iptc.Application2.ProvinceState", value, 32);
}


void
gexiv2_metadata_set_iptc_country_code (GExiv2Metadata *self, const gchar* value)
{
	gexiv2_metadata_set_iptc_tag_string_cut (self, "Iptc.Application2.CountryCode", value, 3);
}


void
gexiv2_metadata_set_iptc_country_name (GExiv2Metadata *self, const gchar* value)
{
	gexiv2_metadata_set_iptc_tag_string_cut (self, "Iptc.Application2.CountryName", value, 32);
}


void
gexiv2_metadata_set_iptc_writer (GExiv2Metadata *self, const gchar* value)
{
	gexiv2_metadata_set_iptc_tag_string_cut (self, "Iptc.Application2.Writer", value, 32);
}


const gchar*
gexiv2_metadata_get_iptc_tag_label (const gchar* tag)
{
	try {
		
		Exiv2::IptcKey key (tag);
		return Exiv2::IptcDataSets::dataSetTitle (key.tag (), key.record ());
		
	} catch (Exiv2::Error& e) {
	}
	
	return "";
}


const gchar*
gexiv2_metadata_get_iptc_tag_description (const gchar* tag)
{
	try {
		
		Exiv2::IptcKey key (tag);
		return Exiv2::IptcDataSets::dataSetDesc (key.tag (), key.record ());
		
	} catch (Exiv2::Error& e) {
	}
	
	return "";
}


G_END_DECLS
