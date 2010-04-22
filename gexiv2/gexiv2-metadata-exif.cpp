/*
 * gexiv2-metadata-exif.cpp
 *
 * Author(s)
 * 	Mike Gemuende <mike@gemuende.de>
 *
 * This is free software. See COPYING for details.
 */
 
#include "gexiv2-metadata.h"
#include "gexiv2-metadata-private.h"
#include <string>
#include <glib-object.h>
#include <exiv2/exif.hpp>

G_BEGIN_DECLS


gboolean
gexiv2_metadata_has_exif (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	return ! (self->priv->exif_data.empty ());
}


void
gexiv2_metadata_clear_exif (GExiv2Metadata *self)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	self->priv->exif_data.clear ();
}


gchar**
gexiv2_metadata_get_exif_tags (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	GSList *list = NULL;
	GSList *list_iter;
	gchar** data;
	gint count = 0;
	Exiv2::ExifData exif_data (self->priv->exif_data);
	
	exif_data.sortByKey ();
	
	for (Exiv2::ExifData::iterator it = exif_data.begin(); it != exif_data.end(); ++it) {        
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
gexiv2_metadata_get_exif_tag_string (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	
	try {
		
		Exiv2::ExifKey key (tag);
		Exiv2::ExifData::iterator it = exif_data.findKey (key);
		
		if (it != exif_data.end ())
			return g_strdup (it->toString ().c_str ());
		
		return g_strdup ("");
		
	} catch (Exiv2::Error& e) {
	}
	
	return NULL;
}


gchar*
gexiv2_metadata_get_exif_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	
	try {
		
		Exiv2::ExifKey key (tag);
		Exiv2::ExifData::iterator it = exif_data.findKey (key);
		
		if (it != exif_data.end ()) {
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
gexiv2_metadata_set_exif_tag_string (GExiv2Metadata *self, const gchar* tag, const gchar* value)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	try {
		
		self->priv->exif_data[tag] = value;
	
		return true;
		
	} catch (Exiv2::Error& e) {
	}
	
	return false;
}


glong
gexiv2_metadata_get_exif_tag_long (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), 0);
	
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	
	try {
		
		Exiv2::ExifKey key (tag);
		Exiv2::ExifData::iterator it = exif_data.findKey (key);
		if (it != exif_data.end ())
			return it->toLong ();
		
		return 0;
		
	} catch (Exiv2::Error& e) {
	}
	
	return 0;
}


gboolean
gexiv2_metadata_set_exif_tag_long (GExiv2Metadata *self, const gchar* tag, glong value)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	try {
		
		self->priv->exif_data[tag] = static_cast<int32_t>(value);
	
		return true;
		
	} catch (Exiv2::Error& e) {
	}
	
	return false;
}


gboolean
gexiv2_metadata_get_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint* nom, gint* den)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
		
	Exiv2::ExifData& exif_data = self->priv->exif_data;
	
	try {
		
		Exiv2::ExifKey key (tag);
		Exiv2::ExifData::iterator it = exif_data.findKey (key);
		if (it != exif_data.end ()) {
			Exiv2::Rational r = it->toRational ();
			*nom = r.first;
			*den = r.second;
			return true;
		}
		
		return false;
		
	} catch (Exiv2::Error& e) {
	}
	
	return false;
}


gboolean
gexiv2_metadata_set_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint nom, gint den)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	try {
		Exiv2::Rational r;
		r.first = nom;
		r.second = den;
		self->priv->exif_data[tag] = r;
	
		return true;
		
	} catch (Exiv2::Error& e) {
	}
	
	return false;
}


const gchar*
gexiv2_metadata_get_exif_tag_label (const gchar* tag)
{
	try {
		
		Exiv2::ExifKey key (tag);
		return Exiv2::ExifTags::tagLabel (key.tag (), key.ifdId ());
		
	} catch (Exiv2::Error& e) {
	}
	
	return "";
}


const gchar*
gexiv2_metadata_get_exif_tag_description (const gchar* tag)
{
	try {
		
		Exiv2::ExifKey key (tag);
		return Exiv2::ExifTags::tagDesc (key.tag (), key.ifdId ());
		
	} catch (Exiv2::Error& e) {
	}
	
	return "";
}


G_END_DECLS
