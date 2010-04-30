/*
 * gexiv2-metadata-xmp.cpp
 *
 * Author(s)
 * 	Mike Gemuende <mike@gemuende.de>
 *  Jim Nelson <jim@yorba.org>
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
	
	return !(self->priv->image->xmpData().empty());
}


void
gexiv2_metadata_clear_xmp	(GExiv2Metadata *self)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	self->priv->image->xmpData().clear();
}


gboolean
gexiv2_metadata_has_xmp_tag(GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail(GEXIV2_IS_METADATA(self), false);
	g_return_val_if_fail(tag != NULL, false);
	
	Exiv2::XmpData &xmp_data = self->priv->image->xmpData();
	
	for (Exiv2::XmpData::iterator it = xmp_data.begin(); it != xmp_data.end(); ++it) {
		if (g_ascii_strcasecmp(tag, it->key().c_str()) == 0)
			return true;
	}
	
	return false;
}


gboolean
gexiv2_metadata_clear_xmp_tag(GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail(GEXIV2_IS_METADATA(self), false);
	g_return_val_if_fail(tag != NULL, false);
	
	Exiv2::XmpData &xmp_data = self->priv->image->xmpData();
	
	gboolean erased = false;
	
	Exiv2::XmpData::iterator it = xmp_data.begin();
	while (it != xmp_data.end()) {
		if (g_ascii_strcasecmp(tag, it->key().c_str()) == 0) {
			it = xmp_data.erase(it);
			erased = true;
		} else {
			it++;
		}
	}
	
	return erased;
}


gchar**
gexiv2_metadata_get_xmp_tags (GExiv2Metadata *self)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	
	// get a copy of the original XmpData and sort it by key, preserving the original
	Exiv2::XmpData xmp_data = Exiv2::XmpData(self->priv->image->xmpData());
	xmp_data.sortByKey ();
	
	GSList *list = NULL;
	GSList *list_iter;
	gchar** data;
	gint count = 0;
	
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
	g_return_val_if_fail (tag != NULL, NULL);
	
	Exiv2::XmpData& xmp_data = self->priv->image->xmpData();
	
	try {
		
		Exiv2::XmpData::iterator it = xmp_data.findKey(Exiv2::XmpKey(tag));
		
		if (it != xmp_data.end ())
			return g_strdup (it->toString ().c_str ());
	} catch (Exiv2::Error& e) {
		LOG_ERROR(e);
	}
	
	return NULL;
}


gchar*
gexiv2_metadata_get_xmp_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	g_return_val_if_fail(tag != NULL, NULL);
	
	Exiv2::XmpData& xmp_data = self->priv->image->xmpData();
	
	try {
		Exiv2::XmpData::iterator it = xmp_data.findKey(Exiv2::XmpKey(tag));
		if (it != xmp_data.end ()) {
			std::ostringstream os;
        	it->write (os);
        	
			return g_strdup (os.str ().c_str ());
		}
	} catch (Exiv2::Error& e) {
		LOG_ERROR(e);
	}
	
	return NULL;
}


gboolean
gexiv2_metadata_set_xmp_tag_string (GExiv2Metadata *self, const gchar* tag, const gchar* value)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	g_return_val_if_fail(tag != NULL, false);
	g_return_val_if_fail(value != NULL, false);
	
	try {
		self->priv->image->xmpData()[tag] = value;
		
		return true;
	} catch (Exiv2::Error& e) {
		LOG_ERROR(e);
	}
	
	return false;
}


glong
gexiv2_metadata_get_xmp_tag_long (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), 0);
	g_return_val_if_fail(tag != NULL, 0);
	
	Exiv2::XmpData& xmp_data = self->priv->image->xmpData();
	
	try {
		Exiv2::XmpData::iterator it = xmp_data.findKey(Exiv2::XmpKey(tag));
		if (it != xmp_data.end ())
			return it->toLong ();
	} catch (Exiv2::Error& e) {
		LOG_ERROR(e);
	}
	
	return 0;
}


gboolean
gexiv2_metadata_set_xmp_tag_long (GExiv2Metadata *self, const gchar* tag, glong value)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	g_return_val_if_fail(tag != NULL, false);
	
	try {
		self->priv->image->xmpData()[tag] = value;
		
		return true;
	} catch (Exiv2::Error& e) {
		LOG_ERROR(e);
	}
	
	return false;
}


gchar**
gexiv2_metadata_get_xmp_tag_multiple (GExiv2Metadata *self, const gchar* tag)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), NULL);
	g_return_val_if_fail(tag != NULL, NULL);
	
	Exiv2::XmpData& xmp_data = self->priv->image->xmpData();
	
	try {
		Exiv2::XmpData::iterator it = xmp_data.findKey(Exiv2::XmpKey(tag));
		if (it != xmp_data.end ()) {
			int size = it->count ();
			gchar **array = g_new (gchar*, size + 1);
			array[size] = NULL;
			
			for (int i = 0; i < it->count (); i++)
				array[i] = g_strdup (it->toString (i).c_str ());
			
			return array;
		}
	} catch (Exiv2::Error& e) {
		LOG_ERROR(e);
	}
	
	gchar **array = g_new (gchar*, 1);
	array[0] = NULL;
	
	return array;
}


gboolean
gexiv2_metadata_set_xmp_tag_multiple (GExiv2Metadata *self, const gchar* tag, const gchar** values)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	g_return_val_if_fail(tag != NULL, false);
	g_return_val_if_fail(values != NULL, false);
	
	Exiv2::XmpData& xmp_data = self->priv->image->xmpData();
	
	try {
		/* first clear existing tag */
		Exiv2::XmpData::iterator xmp_it = xmp_data.findKey(Exiv2::XmpKey(tag));
		if (xmp_it != xmp_data.end ())
			xmp_data.erase (xmp_it);
		
		/* ... and then set the others */
		const gchar **val_it = values;
		while (*val_it != NULL) {
			xmp_data[tag] = static_cast<const std::string> (*val_it);
			++val_it;
		}
		
		return true;
	} catch (Exiv2::Error& e) {
		LOG_ERROR(e);
	}
	
	return false;
}


const gchar*
gexiv2_metadata_get_xmp_tag_label (const gchar* tag)
{
	g_return_val_if_fail(tag != NULL, NULL);
	
	try {
		return Exiv2::XmpProperties::propertyTitle(Exiv2::XmpKey(tag));
	} catch (Exiv2::Error& e) {
		LOG_ERROR(e);
	}
	
	return NULL;
}


const gchar*
gexiv2_metadata_get_xmp_tag_description (const gchar* tag)
{
	g_return_val_if_fail(tag != NULL, NULL);
	
	try {
		return Exiv2::XmpProperties::propertyDesc(Exiv2::XmpKey(tag));
	} catch (Exiv2::Error& e) {
		LOG_ERROR(e);
	}
	
	return NULL;
}


G_END_DECLS
