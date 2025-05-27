/*
 * gexiv2-metadata-gps.cpp
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
#include <cmath>
#include <stdexcept>
#include <glib-object.h>
#include <exiv2/exiv2.hpp>

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

gdouble gexiv2_metadata_get_gps_longitude (GExiv2Metadata *self, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    gdouble longitude = -std::numeric_limits<gdouble>::infinity();

    try {
        double min, sec;

        gchar* longitude_ref = gexiv2_metadata_get_exif_tag_string (self, "Exif.GPSInfo.GPSLongitudeRef", error);
        GPointer longitude_ref_guard(longitude_ref);

        if (longitude_ref == NULL || longitude_ref[0] == '\0') {
            return std::numeric_limits<gdouble>::quiet_NaN();
        }

        Exiv2::ExifData& exif_data = priv->image->exifData();
        Exiv2::ExifKey key ("Exif.GPSInfo.GPSLongitude");
        Exiv2::ExifData::iterator it = exif_data.findKey (key);

        if (it != exif_data.end () && it->count() == 3) {
            longitude = convert_rational(it->toRational(0));
            min = convert_rational(it->toRational(1));
            if (min != -1.0) {
                longitude += min / 60.0;
            }

            sec = convert_rational(it->toRational(2));
            if (sec != -1.0) {
                longitude += sec / 3600.0;
            }

            // There's some weird stuff out there in the wild.
            if (longitude_ref[0] == 'S' || longitude_ref[0] == 'W')
                longitude *= -1.0;
        } else {
            g_set_error_literal(error, g_quark_from_string("GExiv2"), 0, "Missing key 'Exif.GPSInfo.GPSLongitude'.");
        }
    } catch (Exiv2::Error &e) {
        error << e;
    } catch (std::invalid_argument &e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), 0, e.what());
    } catch (std::exception& e) {
        error << e;
    }

    return longitude;
}

gdouble gexiv2_metadata_get_gps_latitude (GExiv2Metadata *self, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    gdouble latitude = -std::numeric_limits<gdouble>::infinity();
    try {
        double min, sec;

        gchar* latitude_ref = gexiv2_metadata_get_exif_tag_string (self, "Exif.GPSInfo.GPSLatitudeRef", error);
        GPointer latitude_ref_guard(latitude_ref);

        if (latitude_ref == NULL || latitude_ref[0] == '\0') {
            return std::numeric_limits<gdouble>::quiet_NaN();
        }

        Exiv2::ExifData& exif_data = priv->image->exifData();
        Exiv2::ExifKey key ("Exif.GPSInfo.GPSLatitude");
        Exiv2::ExifData::iterator it = exif_data.findKey (key);

        if (it != exif_data.end () && it->count() == 3) {
            latitude = convert_rational(it->toRational(0));
            min = convert_rational(it->toRational(1));
            if (min != -1.0) {
                latitude += min / 60.0;
            }

            sec = convert_rational(it->toRational(2));
            if (sec != -1.0) {
                latitude += sec / 3600.0;
            }

            // There's some weird stuff out there in the wild.
            if (latitude_ref[0] == 'S' || latitude_ref[0] == 'W')
                latitude *= -1.0;
       } else {
           g_set_error_literal(error, g_quark_from_string("GExiv2"), 0, "Missing key 'Exif.GPSInfo.GPSLatitude'.");
           return latitude;
       }
    } catch (Exiv2::Error &e) {
        error << e;
    } catch (std::invalid_argument &e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), 0, e.what());
    } catch (std::exception& e) {
        error << e;
    }

    return latitude;
}

gdouble gexiv2_metadata_get_gps_altitude(GExiv2Metadata* self, GError** error) {
    g_return_val_if_fail(GEXIV2_IS_METADATA (self), FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    gdouble altitude = -std::numeric_limits<gdouble>::infinity();

    try {
        gchar* altitude_ref = gexiv2_metadata_get_exif_tag_string (self, "Exif.GPSInfo.GPSAltitudeRef", nullptr);
        GPointer altitude_ref_guard(altitude_ref);

        if (altitude_ref == NULL || altitude_ref[0] == '\0') {
            return std::numeric_limits<gdouble>::quiet_NaN();
        }

        Exiv2::ExifData& exif_data = priv->image->exifData();
        Exiv2::ExifKey key ("Exif.GPSInfo.GPSAltitude");
        Exiv2::ExifData::iterator it = exif_data.findKey (key);

        if (it != exif_data.end () && it->count() == 1) {
            altitude = convert_rational(it->toRational(0));
            if (altitude_ref[0] == '1')
                altitude *= -1.0;
        } else {
            g_set_error_literal(error, g_quark_from_string("GExiv2"), 0, "Missing key 'Exif.GPSInfo.GPSAltitude'.");
        }
    } catch (Exiv2::Error &e) {
        error << e;
    } catch (std::invalid_argument &e) {
        g_set_error_literal(error, g_quark_from_string("GExiv2"), 0, e.what());
    } catch (std::exception& e) {
        error << e;
    }

    return altitude;
}

gboolean gexiv2_metadata_try_get_gps_longitude(GExiv2Metadata* self, gdouble* longitude, GError** error) {
    g_return_val_if_fail(longitude != NULL, FALSE);
    auto res = gexiv2_metadata_get_gps_longitude(self, error);
    if (!std::isnan(res) && !std::isinf(res)) {
        *longitude = res;

        return TRUE;
    } else {
        *longitude = 0.0;

        return FALSE;
    }
}

gboolean gexiv2_metadata_try_get_gps_latitude(GExiv2Metadata* self, gdouble* latitude, GError** error) {
    g_return_val_if_fail(latitude != NULL, FALSE);
    auto res = gexiv2_metadata_get_gps_latitude(self, error);
    if (!std::isnan(res) && !std::isinf(res)) {
        *latitude = res;

        return TRUE;
    } else {
        *latitude = 0.0;

        return FALSE;
    }
}

gboolean gexiv2_metadata_try_get_gps_altitude (GExiv2Metadata *self, gdouble *altitude, GError **error) {
    g_return_val_if_fail(altitude != NULL, FALSE);
    auto res = gexiv2_metadata_get_gps_altitude(self, error);
    if (!std::isnan(res) && !std::isinf(res)) {
        *altitude = res;

        return TRUE;
    } else {
        *altitude = 0.0;

        return FALSE;
    }
}

gboolean gexiv2_metadata_get_gps_info (GExiv2Metadata *self, gdouble *longitude, gdouble *latitude,
    gdouble *altitude, GError **error) {
    gboolean result = TRUE;
    GError* inner_error = nullptr;

    *longitude = gexiv2_metadata_get_gps_longitude(self, &inner_error);
    if (inner_error != nullptr) {
        result = FALSE;
        g_clear_error(&inner_error);
    }

    *latitude = gexiv2_metadata_get_gps_latitude(self, &inner_error);
    if (inner_error != nullptr) {
        result = FALSE;
        g_clear_error(&inner_error);
    }

    *altitude = gexiv2_metadata_get_gps_altitude(self, &inner_error);
    if (inner_error != nullptr) {
        result = FALSE;
    }

    if (result == FALSE) {
        auto e = Exiv2::Error(Exiv2::ErrorCode::kerCorruptedMetadata, "GPS Information");
        error << e;
    }

    return result;
}

gboolean gexiv2_metadata_try_get_gps_info(GExiv2Metadata* self,
                                          gdouble* longitude,
                                          gdouble* latitude,
                                          gdouble* altitude,
                                          GError** error) {
    g_return_val_if_fail(longitude != nullptr, FALSE);
    g_return_val_if_fail(latitude != nullptr, FALSE);
    g_return_val_if_fail(altitude != nullptr, FALSE);

    gdouble lon, lat, alt;

    auto res = gexiv2_metadata_get_gps_info(self, &lon, &lat, &alt, error);
    if (std::isinf(lon) || std::isnan(lon)) {
        *longitude = 0.0;
    } else {
        *longitude = lon;
    }
    if (std::isinf(lat) || std::isnan(lat)) {
        *latitude = 0.0;
    } else {
        *latitude = lat;
    }
    if (std::isinf(alt) || std::isnan(alt)) {
        *altitude = 0.0;
    } else {
        *altitude = alt;
    }

    return res;
}

gboolean gexiv2_metadata_set_gps_info (GExiv2Metadata *self, gdouble longitude, gdouble latitude,
    gdouble altitude, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    try {
        gexiv2_metadata_delete_gps_info (self, error);

        if (error && *error)
            return FALSE;

        return gexiv2_metadata_update_gps_info (self, longitude, latitude, altitude, error);
    } catch (Exiv2::Error &e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return FALSE;
}

gboolean gexiv2_metadata_try_set_gps_info (GExiv2Metadata *self, gdouble longitude, gdouble latitude,
    gdouble altitude, GError **error) {
    return gexiv2_metadata_set_gps_info(self, longitude, latitude, altitude, error);
}

gboolean gexiv2_metadata_update_gps_info (GExiv2Metadata *self, gdouble longitude, gdouble latitude,
    gdouble altitude, GError **error) {
    g_return_val_if_fail (GEXIV2_IS_METADATA (self), FALSE);
    auto* priv = gexiv2_priv(self);

    g_return_val_if_fail(priv->image.get() != NULL, FALSE);
    g_return_val_if_fail(error == nullptr || *error == nullptr, FALSE);

    try {
        Exiv2::ExifData& exif_data = priv->image->exifData();

        gchar buffer [100];
        gint deg, min, sec;
        gdouble remainder, whole;
        
        const gint denom = 1000000;
        
        Exiv2::ExifKey key ("Exif.GPSInfo.GPSVersionID");
        Exiv2::ExifData::iterator it = exif_data.findKey (key);
        if (it == exif_data.end()) {
            /* set GPS Info Version */
            exif_data ["Exif.GPSInfo.GPSVersionID"] = "2 0 0 0";
        }

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
        error << e;
    } catch (std::exception& e) {
        error << e;
    }

    return FALSE;
}

gboolean gexiv2_metadata_try_update_gps_info (GExiv2Metadata *self, gdouble longitude, gdouble latitude,gdouble altitude, GError **error)  {
    return gexiv2_metadata_update_gps_info(self, longitude, latitude, altitude, error);
}

void gexiv2_metadata_delete_gps_info (GExiv2Metadata *self, GError **error) {
    g_return_if_fail(GEXIV2_IS_METADATA (self));
    auto* priv = gexiv2_priv(self);

    g_return_if_fail(priv->image.get() != NULL);
    g_return_if_fail(error == nullptr || *error == nullptr);
    
    try {
        Exiv2::ExifData& exif_data = priv->image->exifData();

        /* clear in exif data */
        Exiv2::ExifData::iterator exif_it = exif_data.begin();
        while (exif_it != exif_data.end()) {
            if (exif_it->groupName () == "GPSInfo")
                exif_it = exif_data.erase (exif_it);
            else
                ++exif_it;
        }
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
    
    /* FIXME: two blocks shall ensure to erase in xmp data, if erasing in exif
     *         fails. Do we need this?
     */
    try {
        Exiv2::XmpData& xmp_data = priv->image->xmpData();

        /* clear in xmp data */
        Exiv2::XmpData::iterator xmp_it = xmp_data.begin();
        while (xmp_it != xmp_data.end()) {
            if (xmp_it->tagName ().compare (0, 3, "GPS") == 0)
                xmp_it = xmp_data.erase (xmp_it);
            else
                ++ xmp_it;
        }
        
    } catch (Exiv2::Error& e) {
        error << e;
    } catch (std::exception& e) {
        error << e;
    }
}

void gexiv2_metadata_try_delete_gps_info (GExiv2Metadata *self, GError **error) {
    gexiv2_metadata_delete_gps_info(self, error);
}

G_END_DECLS
