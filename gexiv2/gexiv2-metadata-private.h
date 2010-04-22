/*
 * gexiv2-metadata-private.h
 *
 * Author(s)
 * 	Mike Gemuende <mike@gemuende.de>
 *
 * This is free software. See COPYING for details.
 */

#ifndef __GEXIV2_METADATA_PRIVATE_H__
#define __GEXIV2_METADATA_PRIVATE_H__

#include "gexiv2-metadata.h"
#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/iptc.hpp>
#include <exiv2/xmp.hpp>

G_BEGIN_DECLS


struct _GExiv2MetadataPrivate
{
	Exiv2::ExifData exif_data;
	Exiv2::IptcData iptc_data;
	Exiv2::XmpData xmp_data;
	gchar* comment;
	gchar* mime_type;
	gint pixel_width;
	gint pixel_height;
	gboolean supports_exif;
	gboolean supports_xmp;
	gboolean supports_iptc;	
};

G_END_DECLS

#endif /* __GEXIV2_METADATA_PRIVATE_H__ */
