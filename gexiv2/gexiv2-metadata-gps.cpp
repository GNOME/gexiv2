/*
 * gexiv2-metadata-gps.cpp
 *
 * Author(s)
 *  Mike Gemuende <mike@gemuende.de>
 *  Jim Nelson <jim@yorba.org>
 *
 * This is free software. See COPYING for details.
 */

#include "gexiv2-metadata.h"
#include "gexiv2-metadata-private.h"
#include <string>
#include <cmath>
#include <stdexcept>
#include <stdio.h>
#include <glib-object.h>
#include <exiv2/exiv2.hpp>

#include <limits>

G_BEGIN_DECLS

static double convert_rational(const Exiv2::Rational& r) {
    if (r.first == 0) {
        return 0.0;
    }

    if (r.second == 0) {
        throw std::invalid_argument("Invalid fraction");
    }

    double num = static_cast<double>(r.first);
    double den = static_cast<double>(r.second);

    return num / den;
}

struct GPointer {
    GPointer(gpointer p) : _p(p) {}
    ~GPointer() { g_free(_p); }
    gpointer reset(gpointer p) { gpointer old = p; std::swap(old, _p); return old; };

    gpointer _p;
private:
    GPointer operator=(const GPointer &other);
    GPointer(const GPointer &other);
};

gboolean gexiv2_metadata_get_gps_longitude (GExiv2Metadata *self, gdouble *longitude) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail (longitude != NULL, FALSE);
    g_return_val_if_fail (self->priv->image.get() != NULL, FALSE);

    try {
        double min, sec;
        *longitude = 0.0;

        gchar* longitude_ref = gexiv2_metadata_get_exif_tag_string (self, "Exif.GPSInfo.GPSLongitudeRef");
        GPointer longitude_ref_guard(longitude_ref);

        if (longitude_ref == NULL || longitude_ref[0] == '\0') {
            return FALSE;
        }

        Exiv2::ExifData& exif_data = self->priv->image->exifData();
        Exiv2::ExifKey key ("Exif.GPSInfo.GPSLongitude");
        Exiv2::ExifData::iterator it = exif_data.findKey (key);

        if (it != exif_data.end () && it->count() == 3) {
            *longitude = convert_rational(it->toRational(0));
            min = convert_rational(it->toRational(1));
            if (min != -1.0) {
                *longitude += min / 60.0;
            }

            sec = convert_rational(it->toRational(2));
            if (sec != -1.0) {
                *longitude += sec / 3600.0;
            }
        } else {
            return FALSE;
        }

        // There's some weird stuff out there in the wild.
        if (longitude_ref[0] == 'S' || longitude_ref[0] == 'W')
            *longitude *= -1.0;

        return TRUE;
    } catch (Exiv2::Error &e) {
        LOG_ERROR(e);
    } catch (std::overflow_error &e) {
        LOG_ERROR(e);
    }

    return FALSE;
}

gboolean gexiv2_metadata_get_gps_latitude (GExiv2Metadata *self, gdouble *latitude) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail (latitude != NULL, FALSE);
    g_return_val_if_fail (self->priv->image.get() != NULL, FALSE);

    try {
        double min, sec;
        *latitude = 0.0;

        gchar* latitude_ref = gexiv2_metadata_get_exif_tag_string (self, "Exif.GPSInfo.GPSLatitudeRef");
        GPointer latitude_ref_guard(latitude_ref);

        if (latitude_ref == NULL || latitude_ref[0] == '\0') {
            return FALSE;
        }

        Exiv2::ExifData& exif_data = self->priv->image->exifData();
        Exiv2::ExifKey key ("Exif.GPSInfo.GPSLatitude");
        Exiv2::ExifData::iterator it = exif_data.findKey (key);

        if (it != exif_data.end () && it->count() == 3) {
            *latitude = convert_rational(it->toRational(0));
            min = convert_rational(it->toRational(1));
            if (min != -1.0) {
                *latitude += min / 60.0;
            }

            sec = convert_rational(it->toRational(2));
            if (sec != -1.0) {
                *latitude += sec / 3600.0;
            }
       } else {
           return FALSE;
       }

        // There's some weird stuff out there in the wild.
        if (latitude_ref[0] == 'S' || latitude_ref[0] == 'W')
            *latitude *= -1.0;

        return TRUE;
    } catch (Exiv2::Error &e) {
        LOG_ERROR(e);
    } catch (std::overflow_error &e) {
        LOG_ERROR(e);
    }

    return FALSE;
}

gboolean gexiv2_metadata_get_gps_altitude (GExiv2Metadata *self, gdouble *altitude) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(altitude != NULL, FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, FALSE);

    try {
        *altitude = 0.0;

        gchar* altitude_ref = gexiv2_metadata_get_exif_tag_string (self, "Exif.GPSInfo.GPSAltitudeRef");
        GPointer altitude_ref_guard(altitude_ref);

        if (altitude_ref == NULL || altitude_ref[0] == '\0') {
            return FALSE;
        }

        Exiv2::ExifData& exif_data = self->priv->image->exifData();
        Exiv2::ExifKey key ("Exif.GPSInfo.GPSAltitude");
        Exiv2::ExifData::iterator it = exif_data.findKey (key);

        if (it != exif_data.end () && it->count() == 1) {
            *altitude = convert_rational(it->toRational(0));
        } else {
            return FALSE;
        }

        if (altitude_ref[0] == '1')
            *altitude *= -1.0;

        return TRUE;
    } catch (Exiv2::Error &e) {
        LOG_ERROR(e);
    } catch (std::overflow_error &e) {
        LOG_ERROR(e);
    }

    return FALSE;
}

gboolean gexiv2_metadata_get_gps_info (GExiv2Metadata *self, gdouble *longitude, gdouble *latitude,
    gdouble *altitude) {
    gboolean result = FALSE;

    if (!gexiv2_metadata_get_gps_longitude (self, longitude)) {
        *longitude = 0.0;
    } else {
        result = TRUE;
    }

    if (!gexiv2_metadata_get_gps_latitude (self, latitude)) {
        *latitude = 0.0;
    } else {
        result = TRUE;
    }

    if (!gexiv2_metadata_get_gps_altitude (self, altitude)) {
        *altitude = 0.0;
    } else {
        result = TRUE;
    }

    return result;
}


gboolean gexiv2_metadata_set_gps_info (GExiv2Metadata *self, gdouble longitude, gdouble latitude, 
    gdouble altitude) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    g_return_val_if_fail(self->priv->image.get() != NULL, FALSE);

    try {
        gexiv2_metadata_delete_gps_info (self);
        
        Exiv2::ExifData& exif_data = self->priv->image->exifData();
        
        gchar buffer [100];
        gint deg, min, sec;
        gdouble remainder, whole;
        
        const gint denom = 1000000;
        
        /* set GPS Info Version */
        exif_data ["Exif.GPSInfo.GPSVersionID"] = "2 0 0 0";
        
        /* set GPS Info format */
        exif_data ["Exif.GPSInfo.GPSMapDatum"] = "WGS-84";
        
        /* set altitude */
        if (altitude >= 0)
            exif_data ["Exif.GPSInfo.GPSAltitudeRef"] = "0";
        else
            exif_data ["Exif.GPSInfo.GPSAltitudeRef"] = "1";

        auto frac = Exiv2::floatToRationalCast(static_cast<float>(fabs(altitude)));

        exif_data ["Exif.GPSInfo.GPSAltitude"] = frac;
        
        /* set latitude */
        if (latitude < 0)
            exif_data ["Exif.GPSInfo.GPSLatitudeRef"] = "S";
        else
            exif_data ["Exif.GPSInfo.GPSLatitudeRef"] = "N";
        
        remainder = modf (fabs (latitude), &whole);
        deg = (gint) floor (whole);
        
        remainder = modf (fabs (remainder * 60), &whole);
        min = (gint) floor (whole);
        sec = (gint) floor (remainder * 60 * denom);
        
        snprintf (buffer, 100, "%d/1 %d/1 %d/%d", deg, min, sec, denom);
        exif_data ["Exif.GPSInfo.GPSLatitude"] = buffer;
        
        /* set longitude */
        if (longitude < 0)
            exif_data ["Exif.GPSInfo.GPSLongitudeRef"] = "W";
        else
            exif_data ["Exif.GPSInfo.GPSLongitudeRef"] = "E";
        
        remainder = modf (fabs (longitude), &whole);
        deg = (gint) floor (whole);
        
        remainder = modf (fabs (remainder * 60), &whole);
        min = (gint) floor (whole);
        sec = (gint) floor (remainder * 60 * denom);
        
        snprintf (buffer, 100, "%d/1 %d/1 %d/%d", deg, min, sec, denom);
        exif_data ["Exif.GPSInfo.GPSLongitude"] = buffer;
        
        return TRUE;
    } catch (Exiv2::Error &e) {
        LOG_ERROR(e);
    }

    return FALSE;
}

void gexiv2_metadata_delete_gps_info (GExiv2Metadata *self) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    g_return_if_fail(self->priv->image.get() != NULL);
    
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
