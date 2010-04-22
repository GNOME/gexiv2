/*
 * gexiv2-metadata-xmp.cpp
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
#include <exiv2/xmp.hpp>

G_BEGIN_DECLS


gboolean
gexiv2_metadata_has_xmp (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	return ! (self->priv->xmp_data.empty ());
}


void
gexiv2_metadata_clear_xmp	(GExiv2Metadata *self)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	self->priv->xmp_data.clear ();
}


gchar**
gexiv2_metadata_get_xmp_tags (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	GSList *list = NULL;
	GSList *list_iter;
	gchar** data;
	gint count = 0;
	Exiv2::XmpData xmp_data (self->priv->xmp_data);
	
	xmp_data.sortByKey ();
	
	for (Exiv2::XmpData::iterator it = xmp_data.begin(); it != xmp_data.end(); ++it) {        
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
gexiv2_metadata_get_xmp_tag_string (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	try {
		
		Exiv2::XmpKey key (tag);
		Exiv2::XmpData::iterator it = xmp_data.findKey (key);
		if (it != xmp_data.end ())
			return g_strdup (it->toString ().c_str ());
		
		return g_strdup ("");
		
	} catch (Exiv2::Error& e) {
	}
	
	return NULL;
}


gchar*
gexiv2_metadata_get_xmp_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	try {
		
		Exiv2::XmpKey key (tag);
		Exiv2::XmpData::iterator it = xmp_data.findKey (key);
		
		if (it != xmp_data.end ()) {
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
gexiv2_metadata_set_xmp_tag_string (GExiv2Metadata *self, const gchar* tag, const gchar* value)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	try {
		
		self->priv->xmp_data[tag] = value;
	
		return true;
		
	} catch (Exiv2::Error& e) {
	}
	
	return false;
}


glong
gexiv2_metadata_get_xmp_tag_long (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), 0);
	
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	try {
		
		Exiv2::XmpKey key (tag);
		Exiv2::XmpData::iterator it = xmp_data.findKey (key);
		if (it != xmp_data.end ())
			return it->toLong ();
		
		return 0;
		
	} catch (Exiv2::Error& e) {
	}
	
	return 0;
}


gboolean
gexiv2_metadata_set_xmp_tag_long (GExiv2Metadata *self, const gchar* tag, glong value)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	try {
		
		self->priv->xmp_data[tag] = value;
	
		return true;
		
	} catch (Exiv2::Error& e) {
	}
	
	return false;
}


gchar**
gexiv2_metadata_get_xmp_tag_multiple (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	try {
	
		Exiv2::XmpKey key (tag);
		Exiv2::XmpData::iterator it = xmp_data.findKey (key);
		
		gchar **array = NULL;
		
		if (it != xmp_data.end ()) {
			
			int size = it->count ();
			array = g_new (gchar*, size + 1);
			array[size] = NULL;
			
			for (int i = 0; i < it->count (); i++)
				array[i] = g_strdup (it->toString (i).c_str ());
			
			return array;
		}
		
		array = g_new (gchar*, 1);
		array[0] = NULL;
		return array;
		
	} catch (Exiv2::Error& e) {
	}
	
	return NULL;
}


gboolean
gexiv2_metadata_set_xmp_tag_multiple (GExiv2Metadata *self, const gchar* tag, const gchar** values)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	
	Exiv2::XmpData& xmp_data = self->priv->xmp_data;
	
	try {
		
		/* first clear existing tag */
		Exiv2::XmpKey key (tag);
		Exiv2::XmpData::iterator xmp_it = xmp_data.findKey (key);
		if (xmp_it != xmp_data.end ())
			xmp_data.erase (xmp_it);
		
		/* ... and then set the others */
		const gchar **val_it = values;
		while (*val_it != NULL) {
			
			xmp_data[tag] = static_cast<const std::string> (*val_it);
			++ val_it;
		}
	 
	 	return true;
	 
	} catch (Exiv2::Error& e) {
	}
	
	return false;
}


const gchar*
gexiv2_metadata_get_xmp_tag_label (const gchar* tag)
{
	try {
		
		Exiv2::XmpKey key (tag);
		return Exiv2::XmpProperties::propertyTitle (key);
		
	} catch (Exiv2::Error& e) {
	}
	
	return "";
}


const gchar*
gexiv2_metadata_get_xmp_tag_description (const gchar* tag)
{
	try {
		
		Exiv2::XmpKey key (tag);
		return Exiv2::XmpProperties::propertyDesc (key);
		
	} catch (Exiv2::Error& e) {
	}
	
	return "";
}


G_END_DECLS
