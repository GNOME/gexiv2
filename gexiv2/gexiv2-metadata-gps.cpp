/*
 * gexiv2-metadata-gps.cpp
 *
 * Author(s)
 * 	Mike Gemuende <mike@gemuende.de>
 *
 * This is free software. See COPYING for details.
 */

#include "gexiv2-metadata.h"
#include "gexiv2-metadata-private.h"
#include <string>
#include <cmath>
#include <stdio.h>
#include <glib-object.h>
#include <exiv2/exif.hpp>

G_BEGIN_DECLS

gboolean
gexiv2_metadata_get_gps_longitude (GExiv2Metadata *self, gdouble *longitude)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	g_return_val_if_fail (longitude != NULL, false);
	
	try {
        double num, den, min, sec;
        *longitude = 0.0;

		gchar* longitude_ref = gexiv2_metadata_get_exif_tag_string (self, "Exif.GPSInfo.GPSLongitudeRef");

		if (longitude_ref == NULL || longitude_ref[0] == '\0')
			return false;

		Exiv2::ExifData& exif_data = self->priv->image->exifData();
		Exiv2::ExifKey key ("Exif.GPSInfo.GPSLongitude");
		Exiv2::ExifData::iterator it = exif_data.findKey (key);

		if (it != exif_data.end () && (*it).count() == 3) {
	
			num = (double)((*it).toRational(0).first);
			den = (double)((*it).toRational(0).second);

			if (den == 0)
				return false;
		
			*longitude = num / den;

			num = (double)((*it).toRational(1).first);
			den = (double)((*it).toRational(1).second);

			if (den == 0)
				return false;

			min = num/den;
	
			if (min != -1.0)
				*longitude = *longitude + min / 60.0;

			num = (double)((*it).toRational(2).first);
			den = (double)((*it).toRational(2).second);

			if (den == 0)
				return false;

			sec = num/den;
	
			if (sec != -1.0)
				*longitude = *longitude + sec / 3600.0;

		} else
			return false;

		if (longitude_ref[0] == 'W')
			*longitude *= -1.0;

		return true;
	} catch (Exiv2::Error &e) {
		LOG_ERROR(e);
	}

	return false;
}


gboolean
gexiv2_metadata_get_gps_latitude (GExiv2Metadata *self, gdouble *latitude)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	g_return_val_if_fail (latitude != NULL, false);
	
	try {
        double num, den, min, sec;
        *latitude = 0.0;

		gchar* latitude_ref = gexiv2_metadata_get_exif_tag_string (self, "Exif.GPSInfo.GPSLatitudeRef");

		if (latitude_ref == NULL || latitude_ref[0] == '\0')
			return false;

		Exiv2::ExifData& exif_data = self->priv->image->exifData();
		Exiv2::ExifKey key ("Exif.GPSInfo.GPSLatitude");
		Exiv2::ExifData::iterator it = exif_data.findKey (key);

		if (it != exif_data.end () && (*it).count() == 3) {
	
			num = (double)((*it).toRational(0).first);
			den = (double)((*it).toRational(0).second);

			if (den == 0)
				return false;
		
			*latitude = num / den;

			num = (double)((*it).toRational(1).first);
			den = (double)((*it).toRational(1).second);

			if (den == 0)
				return false;

			min = num/den;
	
			if (min != -1.0)
				*latitude = *latitude + min / 60.0;

			num = (double)((*it).toRational(2).first);
			den = (double)((*it).toRational(2).second);

			if (den == 0)
				return false;

			sec = num/den;
	
			if (sec != -1.0)
				*latitude = *latitude + sec / 3600.0;
		} else
			return false;

		if (latitude_ref[0] == 'W')
			*latitude *= -1.0;

		return true;
		
	} catch (Exiv2::Error &e) {
		LOG_ERROR(e);
	}

	return false;
}


gboolean
gexiv2_metadata_get_gps_altitude (GExiv2Metadata *self, gdouble *altitude)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);
	g_return_val_if_fail (altitude != NULL, false);
	
	try {
		double num, den;
		*altitude = 0.0;
		
		gchar* altitude_ref = gexiv2_metadata_get_exif_tag_string (self, "Exif.GPSInfo.GPSAltitudeRef");

		if (altitude_ref == NULL || altitude_ref[0] == '\0')
			return false;

		Exiv2::ExifData& exif_data = self->priv->image->exifData();
		Exiv2::ExifKey key ("Exif.GPSInfo.GPSAltitude");
		Exiv2::ExifData::iterator it = exif_data.findKey (key);

		if (it != exif_data.end () && (*it).count() == 1) {
			num = (double)((*it).toRational(0).first);
			den = (double)((*it).toRational(0).second);

			if (den == 0)
				return false;
	
			*altitude = num/den;
		
		} else
			return false;

		if (altitude_ref[0] == '1')
			*altitude *= -1.0;

		return true;
		
	} catch (Exiv2::Error &e) {
		LOG_ERROR(e);
	}

	return false;
}


gboolean
gexiv2_metadata_get_gps_info (GExiv2Metadata *self, gdouble *longitude, gdouble *latitude, gdouble *altitude)
{
	if ( ! gexiv2_metadata_get_gps_longitude (self, longitude))
		return false;

	if ( ! gexiv2_metadata_get_gps_latitude (self, latitude))
		return false;
		
	if ( ! gexiv2_metadata_get_gps_altitude (self, altitude))
		*altitude = 0.0;
	
	return true;
}


gboolean
gexiv2_metadata_set_gps_info (GExiv2Metadata *self, gdouble longitude, gdouble latitude, gdouble altitude)
{
	g_return_val_if_fail (GEXIV2_IS_METADATA (self), false);

	try {
		gexiv2_metadata_delete_gps_info (self);
		
		Exiv2::ExifData& exif_data = self->priv->image->exifData();

		gchar buffer [100];
		gint deg, min;

		/* set GPS Info Version */
		exif_data ["Exif.GPSInfo.GPSVersionID"] = "2 0 0 0";

		/* set GPS Info format */
		exif_data ["Exif.GPSInfo.GPSMapDatum"] = "WGS-84";
		
		
		/* set altitude */
		if (altitude >= 0)
			exif_data ["Exif.GPSInfo.GPSAltitudeRef"] = "0";
		else
			exif_data ["Exif.GPSInfo.GPSAltitudeRef"] = "1";
		
		// FIXME:
		exif_data ["Exif.GPSInfo.GPSAltitude"] = "0/1";
		
		/* set latitude */
		if (latitude < 0)
			exif_data ["Exif.GPSInfo.GPSLatitudeRef"] = "S";
		else
			exif_data ["Exif.GPSInfo.GPSLatitudeRef"] = "N";
			
		deg = (gint) floor (fabs (latitude));
		min = (gint) floor ((fabs (latitude) - floor (fabs (latitude))) * 60000000);
		
		snprintf (buffer, 100, "%d/1 %d/1000000 0/1", deg, min);
		exif_data ["Exif.GPSInfo.GPSLatitude"] = buffer;
		
		
		/* set longitude */
		if (longitude < 0)
			exif_data ["Exif.GPSInfo.GPSLongitudeRef"] = "W";
		else
			exif_data ["Exif.GPSInfo.GPSLongitudeRef"] = "E";
		
		deg = (gint) floor (fabs (longitude));
		min = (gint) floor ((fabs (longitude) - floor (fabs (longitude))) * 60000000);
		
		snprintf (buffer, 100, "%d/1 %d/1000000 0/1", deg, min);
		exif_data ["Exif.GPSInfo.GPSLongitude"] = buffer;

		return true;
		
	} catch (Exiv2::Error &e) {
		LOG_ERROR(e);
	}

	return false;
}


void
gexiv2_metadata_delete_gps_info (GExiv2Metadata *self)
{
	g_return_if_fail (GEXIV2_IS_METADATA (self));
	
	try {
		Exiv2::ExifData& exif_data = self->priv->image->exifData();
		
		/* clear in exif data */
		Exiv2::ExifData::iterator exif_it = exif_data.begin();
		while (exif_it != exif_data.end()) {
			if (exif_it->groupName () == "GPSInfo")
				exif_it = exif_data.erase (exif_it);
			else
				++exif_it;
		}
	} catch (Exiv2::Error& e) {
		LOG_ERROR(e);
	}
	
	/* FIXME: two blocks shall ensure to erase in xmp data, if erasing in exif
	 *         fails. Do we need this?
	 */
	try {
		Exiv2::XmpData& xmp_data = self->priv->image->xmpData();
		
		/* clear in xmp data */
		Exiv2::XmpData::iterator xmp_it = xmp_data.begin();
		while (xmp_it != xmp_data.end()) {
			if (xmp_it->tagName ().compare (0, 3, "GPS") == 0)
				xmp_it = xmp_data.erase (xmp_it);
			else
				++ xmp_it;
		}
		
	} catch (Exiv2::Error& e) {
		LOG_ERROR(e);
	}
}


G_END_DECLS
