/*
 * gexiv2-metadata.h
 *
 * Author(s)
 * 	Mike Gemuende <mike@gemuende.de>
 * 	Jim Nelson <jim@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef GEXIV2_METADATA_H
#define GEXIV2_METADATA_H

#include <glib-object.h>
#include <gio/gio.h>
#include <gexiv2/gexiv2-managed-stream.h>
#include <gexiv2/gexiv2-preview-properties.h>
#include <gexiv2/gexiv2-preview-image.h>

/**
 * SECTION: gexiv2-metadata
 * @title: Generic image metadata handling
 * @short_description: Class to handle the various image metadata standards
 * @include: gexiv2/gexiv2.h
 *
 * #GExiv2Metadata is a generic object that provides everything from simple
 * aggregated accessors to common data such as image comments up to
 * fine-grained access to specific tags of a specific format, be it EXIF, IPTC
 * or XMP
 */

/**
 * SECTION: gexiv2-metadata-xmp
 * @short_description: Functionality specific to the XMP familiy of metadata information
 * @title: Functions related to XMP metadata
 * @include: gexiv2/gexiv2.h
 *
 * While being part of #GExiv2Metadata, these functions are dealing with specific
 * aspects of XMP
 */

/**
 * SECTION: gexiv2-metadata-exif
 * @short_description: Functionality specific to the EXIF familiy of metadata information
 * @title: Functions related to EXIF metadata
 * @include: gexiv2/gexiv2.h
 *
 * While being part of #GExiv2Metadata, these functions are dealing with specific
 * aspects of EXIF
 */

/**
 * SECTION: gexiv2-metadata-iptc
 * @short_description: Functionality specific to the IPTC familiy of metadata information
 * @title: Functions related to IPTC metadata
 * @include: gexiv2/gexiv2.h
 *
 * While being part of #GExiv2Metadata, these functions are dealing with specific
 * aspects of IPTC
 */

/**
 * SECTION: gexiv2-preview-image
 * @title: Preview images
 * @short_description: Class describing preview images (or thumbnails) that
 * are part of the metadata.
 * @see_also: #GExiv2Metadata #GExiv2PreviewProperties
 * @include: gexiv2/gexiv2.h
 *
 * #GExiv2PreviewImage is an accessor to the preview images contained in
 * the image's metadata. This could be anything from a thumbnail to a
 * full-sized camera development of a RAW image.
 *
 * The #GExiv2PreviewImage is obtained by calling gexiv2_metadata_get_preview_image()
 * with an instance of #GExiv2PreviewProperties that are describing the image
 * to be fetched.
 * <informalexample><programlisting>
 * GExiv2PreviewProperties **properties, **it;
 * properties = it = gexiv2_metadata_get_preview_properties(metadata);
 *
 * while (*it) {
 *   preview_image = gexiv2_metadata_get_preview_image(metadata, *it);
 *   it++;
 * }
 * </programlisting></informalexample>
 */

/**
 * SECTION: gexiv2-preview-properties
 * @title: Properties of preview images
 * @short_description: Class describing properties of a preview image
 * @see_also: #GExiv2Metadata #GExiv2PreviewProperties
 *
 * Metadata can contain multiple preview images. #GExiv2PreviewProperties are
 * used to describe the available image without the need of fetching the whole
 * image from the metadata.
 */

/**
 * SECTION: gexiv2-version
 * @title: Library version information
 * @short_description: Utilities for querying the library version
 *
 * Provides macros and functions to query the version of the GExiv2 library during compile- and runtime.
 */

/**
 * SECTION: gexiv2-io
 * @title: Reading and writing metadata
 * @short_description: How to read and write metadata from files and memory
 */

/**
 * SECTION: gexiv2-convenience
 * @title: High-level functions for specific information
 * @short_description: A set of helper functions to simplify accessing specific information
 *
 * The functions in this section will use several individual tags to provide or set
 * the required information while trying to follow the MWG recommendations
 */

G_BEGIN_DECLS

#define GEXIV2_TYPE_METADATA \
	(gexiv2_metadata_get_type ())
	
#define GEXIV2_METADATA(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), GEXIV2_TYPE_METADATA, GExiv2Metadata))
	
#define GEXIV2_IS_METADATA(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEXIV2_TYPE_METADATA))
	
#define GEXIV2_METADATA_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), GEXIV2_TYPE_METADATA, GExiv2MetadataClass))
	
#define GEXIV2_IS_METADATA_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), GEXIV2_TYPE_METADATA))
	
#define GEXIV2_METADATA_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), GEXIV2_TYPE_METADATA, GExiv2MetadataClass))
	

/**
 * GExiv2Orientation:
 * @GEXIV2_ORIENTATION_UNSPECIFIED: The orientation of the image is unknown
 * @GEXIV2_ORIENTATION_NORMAL: The orientation of the image is without any rotation.
 * @GEXIV2_ORIENTATION_HFLIP: The image is flipped on its horizontal axis
 * @GEXIV2_ORIENTATION_ROT_180: The image is rotated by 180 degrees
 * @GEXIV2_ORIENTATION_VFLIP: The Image is flipped on its vertical axis
 * @GEXIV2_ORIENTATION_ROT_90_HFLIP: The image is rotated by 90 degrees clockwise and flipped on its horizontal axis
 * @GEXIV2_ORIENTATION_ROT_90: The image is rotated by 90 degrees clockwise
 * @GEXIV2_ORIENTATION_ROT_90_VFLIP: The image is rotated by 90 degrees clockwise and flipped on its vertical axis
 * @GEXIV2_ORIENTATION_ROT_270: The image is rotated 270 degrees clockwise
 *
 * The orientation of an image is defined as the location of it's x,y origin.  More than rotation,
 * orientation allows for every variation of rotation, flips, and mirroring to be described in
 * 3 bits of data.
 *
 * A handy primer to orientation can be found at
 * <ulink url="http://jpegclub.org/exif_orientation.html"></ulink>
 */
typedef enum {
	/*< private >*/
	GEXIV2_ORIENTATION_MIN			= 0,
	/*< public >*/
	GEXIV2_ORIENTATION_UNSPECIFIED	= 0,
	GEXIV2_ORIENTATION_NORMAL		= 1,
	GEXIV2_ORIENTATION_HFLIP		= 2,
	GEXIV2_ORIENTATION_ROT_180		= 3,
	GEXIV2_ORIENTATION_VFLIP		= 4,
	GEXIV2_ORIENTATION_ROT_90_HFLIP	= 5,
	GEXIV2_ORIENTATION_ROT_90		= 6,
	GEXIV2_ORIENTATION_ROT_90_VFLIP	= 7,
	GEXIV2_ORIENTATION_ROT_270		= 8,
	/*< private >*/
	GEXIV2_ORIENTATION_MAX			= 8
} GExiv2Orientation;

/**
 * GExiv2StructureType:
 * @GEXIV2_STRUCTURE_XA_NONE: Structure is not an array
 * @GEXIV2_STRUCTURE_XA_ALT: A list of alternative values
 * @GEXIV2_STRUCTURE_XA_BAG: An unordered list of values
 * @GEXIV2_STRUCTURE_XA_SEQ: An ordered list of values
 * @GEXIV2_STRUCTURE_XA_LANG: Not supported. For completeness only
 *
 * Used in gexiv2_metadata_set_xmp_tag_struct() to determine the array type
 */
typedef enum {
  GEXIV2_STRUCTURE_XA_NONE = 0,
  GEXIV2_STRUCTURE_XA_ALT  = 20,
  GEXIV2_STRUCTURE_XA_BAG  = 21,
  GEXIV2_STRUCTURE_XA_SEQ  = 22,
  GEXIV2_STRUCTURE_XA_LANG = 23
} GExiv2StructureType;

/**
 * GExiv2XmpFormatFlags:
 * @GEXIV2_OMIT_PACKET_WRAPPER: Omit the XML packet wrapper.
 * @GEXIV2_READ_ONLY_PACKET: Default is a writeable packet.
 * @GEXIV2_USE_COMPACT_FORMAT: Use a compact form of RDF.
 * @GEXIV2_INCLUDE_THUMBNAIL_PAD: Include a padding allowance for a thumbnail image.
 * @GEXIV2_EXACT_PACKET_LENGTH: The padding parameter is the overall packet length.
 * @GEXIV2_WRITE_ALIAS_COMMENTS: Show aliases as XML comments.
 * @GEXIV2_OMIT_ALL_FORMATTING: Omit all formatting whitespace.
 *
 * Options to control the format of the serialized XMP packet
 * Taken from: exiv2/src/xmp.hpp
 *
 */
typedef enum { /*< flags >*/
  GEXIV2_OMIT_PACKET_WRAPPER   = 0x0010UL,
  GEXIV2_READ_ONLY_PACKET      = 0x0020UL,
  GEXIV2_USE_COMPACT_FORMAT    = 0x0040UL,
  GEXIV2_INCLUDE_THUMBNAIL_PAD = 0x0100UL,
  GEXIV2_EXACT_PACKET_LENGTH   = 0x0200UL,
  GEXIV2_WRITE_ALIAS_COMMENTS  = 0x0400UL,
  GEXIV2_OMIT_ALL_FORMATTING   = 0x0800UL
} GExiv2XmpFormatFlags;

/**
 * GExiv2ByteOrder:
 * @GEXIV2_BYTE_ORDER_LITTLE: Use little-endian byte order
 * @GEXIV2_BYTE_ORDER_BIG: Use big-endian byte order
 *
 * Options to control the byte order of binary EXIF data exports
 */
typedef enum {
  GEXIV2_BYTE_ORDER_LITTLE = 0,
  GEXIV2_BYTE_ORDER_BIG = 1
} GExiv2ByteOrder;

/**
 * GExiv2Metadata:
 *
 * An object holding all the Exiv2 metadata.  Previews, if present, are also available.
 *
 * As gexiv2 is only a wrapper around Exiv2, it's better to read its documentation to understand
 * the full scope of what it offers: <ulink url="http://www.exiv2.org/"></ulink>
 *
 * In particular, rather than providing a getter/setter method pair for every metadata value
 * available for images (of which there are thousands), Exiv2 uses a dotted addressing scheme.
 * For example, to access a photo's EXIF Orientation field, the caller passes to Exiv2
 * "Exif.Photo.Orientation".  These <emphasis>tags</emphasis> (in Exiv2 parlance) are key to using Exiv2 (and
 * therefore gexiv2) to its fullest.
 *
 * A full reference for all supported Exiv2 tags can be found at
 * <ulink url="http://www.exiv2.org/metadata.html"></ulink>
 */
typedef struct _GExiv2Metadata			GExiv2Metadata;
typedef struct _GExiv2MetadataClass		GExiv2MetadataClass;
typedef struct _GExiv2MetadataPrivate	GExiv2MetadataPrivate;

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GExiv2Metadata, g_object_unref)

struct _GExiv2Metadata
{
	GObject parent_instance;

	/*< private >*/
	GExiv2MetadataPrivate *priv;
};

struct _GExiv2MetadataClass
{
	GObjectClass parent_class;
};

/* basic functions */

GType 			gexiv2_metadata_get_type			(void);

/**
 * gexiv2_metadata_new:
 *
 * Returns: (transfer full): A fully constructed #GExiv2Metadata ready to be used
 */
GExiv2Metadata* gexiv2_metadata_new					(void);

/**
 * gexiv2_metadata_free:
 * @self: An instance of #GExiv2Metadata
 *
 * Destroys the #GExiv2Metadata object and frees all associated memory.
 *
 * Deprecated: 0.10.3: Use g_object_unref() instead.
 */
G_DEPRECATED_FOR(g_object_unref)
void			gexiv2_metadata_free				(GExiv2Metadata *self);

/**
 * gexiv2_metadata_open_path:
 * @self: An instance of #GExiv2Metadata
 * @path: Path to the file you want to open
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The file must be an image format supported by Exiv2.
 *
 * Returns: Boolean success indicator
 *
 *
 */
gboolean		gexiv2_metadata_open_path			(GExiv2Metadata *self, const gchar *path, GError **error);

/**
 * gexiv2_metadata_open_buf:
 * @self: An instance of #GExiv2Metadata
 * @data: (array length=n_data): A buffer containing the data to be read
 * @n_data: (skip): The length of the buffer
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The buffer must be an image format supported by Exiv2.
 *
 * Returns: Boolean success indicator
 *
 */
gboolean		gexiv2_metadata_open_buf			(GExiv2Metadata *self, const guint8 *data, glong n_data, GError **error);

/**
 * gexiv2_metadata_open_stream: (skip)
 * @self: An instance of #GExiv2Metadata
 * @cb: A #ManagedStreamCallbacks struct offering stream access.
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The stream must be an image format supported by Exiv2.
 *
 * Returns: Boolean success indicator
 *
 */
gboolean		gexiv2_metadata_open_stream			(GExiv2Metadata *self, ManagedStreamCallbacks* cb, GError **error);

/**
 * gexiv2_metadata_from_stream:
 *
 * This function does not work and will be removed in a future release.
 *
 */
G_DEPRECATED
gboolean		gexiv2_metadata_from_stream			(GExiv2Metadata *self, GInputStream* stream, GError **error);

/**
 * gexiv2_metadata_from_app1_segment:
 * @self: An instance of #GExiv2Metadata
 * @data: (array length=n_data): A buffer containing the data to be read
 * @n_data: (skip): The length of the buffer
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Load only an EXIF buffer, typically stored in a JPEG's APP1 segment.
 *
 * Returns: Boolean success indicator.
 *
 */
gboolean		gexiv2_metadata_from_app1_segment	(GExiv2Metadata *self, const guint8 *data, glong n_data, GError **error);

/**
 * gexiv2_metadata_save_external:
 * @self: An instance of #GExiv2Metadata
 * @path: Path to the file you want to save to.
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Saves the metadata to the specified using an XMP sidecar file.
 *
 * Returns: Boolean success indicator.
 *
 * Since: 0.10.6
 *
 */
gboolean		gexiv2_metadata_save_external			(GExiv2Metadata *self, const gchar *path, GError **error);

/**
 * gexiv2_metadata_save_file:
 * @self: An instance of #GExiv2Metadata
 * @path: Path to the file you want to save to.
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Saves the metadata to the specified file by reading the file into memory,copying this object's
 * metadata into the image, then writing the image back out.
 *
 * Returns: Boolean success indicator.
 *
 */
gboolean		gexiv2_metadata_save_file			(GExiv2Metadata *self, const gchar *path, GError **error);

/**
 * gexiv2_metadata_save_stream: (skip)
 * @self: An instance of #GExiv2Metadata
 * @cb: A #ManagedStreamCallbacks struct offering stream access.
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Saves the metadata to the stream by reading the stream into memory,copying this object's
 * metadata into the image, then writing the image as a stream back out.
 *
 * Returns: Boolean success indicator.
 *
 */
gboolean		gexiv2_metadata_save_stream			(GExiv2Metadata *self, ManagedStreamCallbacks* cb, GError **error);

/**
 * gexiv2_metadata_has_tag:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE if the tag is present.
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_has_tag() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_has_tag)
gboolean		gexiv2_metadata_has_tag				(GExiv2Metadata *self, const gchar* tag);

/**
 * gexiv2_metadata_try_has_tag:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE if the tag is present.
 *
 * Since: 0.14.0
 */
gboolean gexiv2_metadata_try_has_tag(GExiv2Metadata* self, const gchar* tag, GError** error);

/**
 * gexiv2_metadata_clear_tag:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag
 *
 * Removes the Exiv2 tag from the metadata object.
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE if the tag was present.
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_clear_tag() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_clear_tag)
gboolean		gexiv2_metadata_clear_tag			(GExiv2Metadata *self, const gchar* tag);

/**
 * gexiv2_metadata_try_clear_tag:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Removes the Exiv2 tag from the metadata object.
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE if the tag was present.
 *
 * Since: 0.14.0
 */
gboolean gexiv2_metadata_try_clear_tag(GExiv2Metadata* self, const gchar* tag, GError** error);

/**
 * gexiv2_metadata_clear:
 * @self: An instance of #GExiv2Metadata
 *
 * Removes all tags for all domains (EXIF, IPTC, and XMP).
 */
void			gexiv2_metadata_clear				(GExiv2Metadata *self);

/**
 * gexiv2_metadata_is_exif_tag:
 * @tag: An Exiv2 tag
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE if the Exiv2 tag is for the EXIF domain.
 */
gboolean		gexiv2_metadata_is_exif_tag				(const gchar* tag);

/**
 * gexiv2_metadata_is_iptc_tag:
 * @tag: An Exiv2 tag
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE if the Exiv2 tag is for the IPTC domain.
 */
gboolean		gexiv2_metadata_is_iptc_tag				(const gchar* tag);

/**
 * gexiv2_metadata_is_xmp_tag:
 * @tag: An Exiv2 tag
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE if the Exiv2 tag is for the XMP domain.
 */
gboolean		gexiv2_metadata_is_xmp_tag				(const gchar* tag);

/**
 * gexiv2_metadata_try_get_tag_label:
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: (transfer none) (allow-none): The tag's label
 *
 * Since: 0.12.2
 */
const gchar*	gexiv2_metadata_try_get_tag_label		(const gchar *tag, GError **error);

/**
 * gexiv2_metadata_get_tag_label:
 * @tag: An Exiv2 tag
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: (transfer none) (allow-none): The tag's label
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_tag_label() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_tag_label)
const gchar*	gexiv2_metadata_get_tag_label		(const gchar *tag);

/**
 * gexiv2_metadata_try_get_tag_description:
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: (transfer none) (allow-none): The tag's description
 *
 * Since: 0.12.2
 */
const gchar*	gexiv2_metadata_try_get_tag_description	(const gchar *tag, GError **error);

/**
 * gexiv2_metadata_get_tag_description:
 * @tag: An Exiv2 tag
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: (transfer none) (allow-none): The tag's description
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_tag_description() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_tag_description)
const gchar*	gexiv2_metadata_get_tag_description	(const gchar *tag);

/**
 * gexiv2_metadata_try_get_tag_type:
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The names of the various Exiv2 tag types can be found at Exiv2::TypeId,
 * <ulink url="http://exiv2.org/doc/namespaceExiv2.html#a5153319711f35fe81cbc13f4b852450c"></ulink>
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: (transfer none) (allow-none): The tag's type name.
 *
 * Since: 0.12.2
 */
const gchar*	gexiv2_metadata_try_get_tag_type	(const gchar *tag, GError **error);

/**
 * gexiv2_metadata_get_tag_type:
 * @tag: An Exiv2 tag
 *
 * The names of the various Exiv2 tag types can be found at Exiv2::TypeId,
 * <ulink url="http://exiv2.org/doc/namespaceExiv2.html#a5153319711f35fe81cbc13f4b852450c"></ulink>
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: (transfer none) (allow-none): The tag's type name.
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_tag_type() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_tag_type)
const gchar*	gexiv2_metadata_get_tag_type	(const gchar *tag);

/**
 * gexiv2_metadata_try_tag_supports_multiple_values:
 * @self: An instance of #GExiv2Metadata
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The Exiv2 Tag Reference can be found at <ulink url="https://www.exiv2.org/metadata.html"></ulink>
 *
 * Multiple value tags are Xmp tags of type "XmpAlt", "XmpBag", "XmpSeq" or "LangAlt", or Iptc
 * tags marked as Repeatable (which can be of any Iptc type). There are no multiple value Exif
 * tags.
 *
 * Returns: Whether @tag is capable of storing multiple values or not. If @tag is undefined
 * (i.e. not built-in and not added to @self), then @error is set and %FALSE is returned.
 *
 * Since: 0.14.0
 */
gboolean gexiv2_metadata_try_tag_supports_multiple_values(GExiv2Metadata* self, const gchar* tag, GError** error);

/**
 * gexiv2_metadata_get_supports_exif:
 * @self: An instance of #GExiv2Metadata
 *
 * Query @self whether it supports writing EXIF metadata.
 *
 * Returns: %TRUE if the loaded image type supports writing EXIF metadata.
 */
gboolean		gexiv2_metadata_get_supports_exif	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_supports_iptc:
 * @self: An instance of #GExiv2Metadata
 *
 * Query @self whether it supports writing IPTC metadata.
 *
 * Returns: %TRUE if the loaded image type supports writing IPTC metadata.
 */
gboolean		gexiv2_metadata_get_supports_iptc	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_supports_xmp:
 * @self: An instance of #GExiv2Metadata
 *
 * Query @self whether it supports writing XMP metadata.
 *
 * Returns: %TRUE if the loaded image type supports writing XMP metadata.
 */
gboolean		gexiv2_metadata_get_supports_xmp	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_mime_type:
 * @self: An instance of #GExiv2Metadata
 *
 * Query mime type of currently loaded image.
 *
 * Returns: (transfer none): The MIME type of the loaded image, %NULL if not loaded or unknown.
 */
const gchar*	gexiv2_metadata_get_mime_type		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_pixel_width:
 * @self: An instance of #GExiv2Metadata
 *
 * Get the <emphasis>actual</emphasis> unoriented display width in pixels of the loaded
 * image. May be different than the width reported by various metadata tags,
 * i.e. "Exif.Photo.PixelXDimension".
 *
 * Returns: Pixel width of current image
 */
gint			gexiv2_metadata_get_pixel_width		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_pixel_height:
 * @self: An instance of #GExiv2Metadata
 *
 * Get the <emphasis>actual</emphasis> unoriented display height in pixels of the loaded image.  This may
 * be different than the height reported by various metadata tags, i.e. "Exif.Photo.PixelYDimension".
 *
 * Returns: Pixel height of current image
 */
gint			gexiv2_metadata_get_pixel_height	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_get_tag_string:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Tags that support multiple values are returned as a single string, with elements separated by ", ".
 *
 * Returns: (transfer full) (allow-none): The tag's value as a string
 *
 * Since: 0.12.2
 */
gchar*			gexiv2_metadata_try_get_tag_string	(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_try_set_tag_string:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @value: The value to set or replace the existing value
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * If a tag supports multiple values, then @value is added to any existing values. For single
 * tags, @value replaces the value.
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE on success
 *
 * Since: 0.12.2
 */
gboolean		gexiv2_metadata_try_set_tag_string	(GExiv2Metadata *self, const gchar* tag, const gchar* value, GError **error);

/**
 * gexiv2_metadata_get_tag_string:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Tags that support multiple values are returned as a single string, with elements separated by ", ".
 *
 * In case of error, a GLib warning will be logged. Use instead
 * gexiv2_metadata_try_get_tag_string() if you want to avoid this and
 * control if and how the error is outputted.
 *
 * Returns: (transfer full) (allow-none): The tag's value as a string
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_tag_string() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_tag_string)
gchar*			gexiv2_metadata_get_tag_string		(GExiv2Metadata *self, const gchar* tag);

/**
 * gexiv2_metadata_set_tag_string:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @value: The value to set or replace the existing value
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * If a tag supports multiple values, then @value is added to any existing values. For single
 * value tags, @value replaces the value.
 *
 * In case of error, a GLib warning will be logged. Use instead
 * gexiv2_metadata_try_set_tag_string() if you want to avoid this and
 * control if and how the error is outputted.
 *
 * Returns: TRUE on success
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_set_tag_string() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_tag_string)
gboolean		gexiv2_metadata_set_tag_string		(GExiv2Metadata *self, const gchar* tag, const gchar* value);

/**
 * gexiv2_metadata_try_set_xmp_tag_struct:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @type: The GExiv2StructureType specifying the type of structure
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE on success
 *
 * Since: 0.12.2
 */
gboolean gexiv2_metadata_try_set_xmp_tag_struct (GExiv2Metadata *self, const gchar* tag, GExiv2StructureType type, GError **error);

/**
 * gexiv2_metadata_set_xmp_tag_struct:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @type: The GExiv2StructureType specifying the type of structure
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE on success
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_set_xmp_tag_struct() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_xmp_tag_struct)
gboolean gexiv2_metadata_set_xmp_tag_struct (GExiv2Metadata *self, const gchar* tag, GExiv2StructureType type);

/**
 * gexiv2_metadata_try_get_tag_interpreted_string:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * An interpreted string is one fit for user display.  It may display units or use formatting
 * appropriate to the type of data the tag holds.
 *
 * Tags that support multiple values are returned as a single string, with elements separated by ", ".
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: (transfer full) (allow-none): The tag's interpreted value as a string
 *
 * Since: 0.12.2
 */
gchar*			gexiv2_metadata_try_get_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_get_tag_interpreted_string:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 *
 * An interpreted string is one fit for user display.  It may display units or use formatting
 * appropriate to the type of data the tag holds.
 *
 * Tags that support multiple values are returned as a single string, with elements separated by ", ".
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: (transfer full) (allow-none): The tag's interpreted value as a string
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_tag_interpreted_string() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_tag_interpreted_string)
gchar*			gexiv2_metadata_get_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag);

/**
 * gexiv2_metadata_try_get_tag_long:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: The tag's value as a glong
 *
 * Since: 0.12.2
 */
glong			gexiv2_metadata_try_get_tag_long	(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_get_tag_long:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: The tag's value as a glong
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_tag_long() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_tag_long)
glong			gexiv2_metadata_get_tag_long		(GExiv2Metadata *self, const gchar* tag);

/**
 * gexiv2_metadata_try_set_tag_long:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @value: The value to set or replace the existing value
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE on success
 *
 * Since: 0.12.2
 */
gboolean		gexiv2_metadata_try_set_tag_long	(GExiv2Metadata *self, const gchar* tag, glong value, GError **error);

/**
 * gexiv2_metadata_set_tag_long:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @value: The value to set or replace the existing value
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: TRUE on success
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_set_tag_long() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_tag_long)
gboolean		gexiv2_metadata_set_tag_long		(GExiv2Metadata *self, const gchar* tag, glong value);

/**
 * gexiv2_metadata_try_get_tag_multiple:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Returns: (transfer full) (allow-none) (array zero-terminated=1): The multiple string values of
 * @tag.  Returns %NULL if parameters are %NULL or @tag does not begin with recognised type of
 * metadata ("Exif.", "Xmp." or "Iptc.").  For a well formed @tag, returns array[0] = %NULL if @tag
 * is undefined or is not set in the current metadata.
 *
 * Since: 0.12.2
 */
gchar**			gexiv2_metadata_try_get_tag_multiple	(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_try_set_tag_multiple:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @values: (array zero-terminated=1): An array of values to set or replace the existing value(s)
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * All previous @tag values are erased. For multiple value tags, each of the non %NULL
 * entries in @values is stored. For single value tags, only the last non %NULL value
 * is assigned.
 *
 * Returns: Boolean success value
 *
 * Since: 0.12.2
 */
gboolean		gexiv2_metadata_try_set_tag_multiple	(GExiv2Metadata *self, const gchar* tag, const gchar** values, GError **error);

/**
 * gexiv2_metadata_get_tag_multiple:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * In case of error, a GLib warning will be logged. Use instead
 * gexiv2_metadata_try_get_tag_multiple() if you want to avoid this and
 * control if and how the error is outputted.
 *
 * Returns: (transfer full) (allow-none) (array zero-terminated=1): The multiple string values of
 * the tag.  Returns %NULL if parameters are %NULL or @tag does not begin with recognised type of
 * metadata ("Exif.", "Xmp." or "Iptc.").  For a well formed @tag, returns array[0] = %NULL if @tag
 * is undefined or is not set in the current metadata.
 * (Note: <ulink url="https://gitlab.gnome.org/GNOME/gexiv2/-/issues/61">xmpText/langAlt bug</ulink>
 *  is fixed in gexiv2_metadata_try_get_tag_multiple())
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_tag_multiple() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_tag_multiple)
gchar**			gexiv2_metadata_get_tag_multiple	(GExiv2Metadata *self, const gchar* tag);

/**
 * gexiv2_metadata_set_tag_multiple:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @values: (array zero-terminated=1): An array of values to set or replace the existing value(s)
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * All previous @tag values are erased. For multiple value tags, each of the non %NULL
 * entries in @values is stored. For single value tags, only the last non %NULL value
 * is assigned.
 *
 * In case of error, a GLib warning will be logged. Use instead
 * gexiv2_metadata_try_set_tag_multiple() if you want to avoid this and
 * control if and how the error is outputted.
 *
 * Returns: Boolean success value
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_set_tag_multiple() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_tag_multiple)
gboolean		gexiv2_metadata_set_tag_multiple	(GExiv2Metadata *self, const gchar* tag, const gchar** values);

/**
 * gexiv2_metadata_try_get_tag_raw:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Tags that support multiple values may be returned as a single byte array, with records separated
 * by 4x INFORMATION SEPARATOR FOUR (ASCII 0x1c)
 *
 * Returns: (transfer full) (allow-none): The tag's raw value as a byte array
 *
 * Since: 0.12.2
 */
GBytes*			gexiv2_metadata_try_get_tag_raw		(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_get_tag_raw:
 * @self: An instance of #GExiv2Metadata
 * @tag: Exiv2 tag name
 *
 * The Exiv2 Tag Reference can be found at <ulink url="http://exiv2.org/metadata.html"></ulink>
 *
 * Tags that support multiple values may bereturned as a single byte array, with records separated
 * by 4x INFORMATION SEPARATOR FOUR (ASCII 0x1c)
 *
 * Returns: (transfer full) (allow-none): The tag's raw value as a byte array
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_tag_raw() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_tag_raw)
GBytes*			gexiv2_metadata_get_tag_raw			(GExiv2Metadata *self, const gchar* tag);

/*
 * EXIF functions
 */

/**
 * gexiv2_metadata_has_exif:
 * @self: An instance of #GExiv2Metadata
 *
 * Returns: TRUE if EXIF metadata is present in the loaded image
 */
gboolean		gexiv2_metadata_has_exif			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_clear_exif:
 * @self: An instance of #GExiv2Metadata
 *
 * Clears all EXIF metadata from the loaded image.
 */
void			gexiv2_metadata_clear_exif			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_exif_tags:
 * @self: An instance of #GExiv2Metadata
 *
 * Query @self for a list of available EXIF tags
 *
 * Returns: (transfer full) (array zero-terminated=1): A unique list of the available EXIF tags in the
 * loaded image
 */
gchar**			gexiv2_metadata_get_exif_tags		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_get_exif_tag_rational:
 * @self: An instance of #GExiv2Metadata
 * @tag: (in): The tag you want the rational value for
 * @nom: (out): The numerator
 * @den: (out): The denominator
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Fetch EXIF @tag represented by a fraction. @nom will contain the numerator,
 * @den the denominator of the fraction on successful return.
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.12.2
 */
gboolean		gexiv2_metadata_try_get_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint* nom, gint* den, GError **error);

/**
 * gexiv2_metadata_try_set_exif_tag_rational:
 * @self: An instance of #GExiv2Metadata
 * @tag: (in): The Exiv2 tag
 * @nom: Rational numerator
 * @den: Rational denominator
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Set EXIF @tag represented by a fraction, with @nom being the numerator,
 * @den the denominator of the fraction.
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.12.2
 */
gboolean		gexiv2_metadata_try_set_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint nom, gint den, GError **error);

/**
 * gexiv2_metadata_get_exif_tag_rational:
 * @self: An instance of #GExiv2Metadata
 * @tag: (in): The tag you want the rational value for
 * @nom: (out): The numerator
 * @den: (out): The denominator
 *
 * Fetch EXIF @tag represented by a fraction. @nom will contain the numerator,
 * @den the denominator of the fraction on successful return.
 *
 * Returns: (skip): Boolean success value
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_exif_tag_rational() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_exif_tag_rational)
gboolean		gexiv2_metadata_get_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint* nom, gint* den);

/**
 * gexiv2_metadata_set_exif_tag_rational:
 * @self: An instance of #GExiv2Metadata
 * @tag: (in): The Exiv2 tag
 * @nom: Rational numerator
 * @den: Rational denominator
 *
 * Set EXIF @tag represented by a fraction, with @nom being the numerator,
 * @den the denominator of the fraction.
 *
 * Returns: (skip): Boolean success value
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_set_exif_tag_rational() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_exif_tag_rational)
gboolean		gexiv2_metadata_set_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint nom, gint den);

/**
 * gexiv2_metadata_get_exif_thumbnail:
 * @self: An instance of #GExiv2Metadata
 * @buffer: (out) (array length=size) (transfer full): Where to store the thumbnail data
 * @size: (skip): Size of the thumbnail's buffer
 *
 * Get the thumbnail stored in the EXIF data of @self
 *
 * Returns: (skip): Boolean success value
 */
gboolean		gexiv2_metadata_get_exif_thumbnail (GExiv2Metadata *self, guint8** buffer, gint* size);

/**
 * gexiv2_metadata_set_exif_thumbnail_from_file:
 * @self: An instance of #GExiv2Metadata
 * @path: (in): Path of image file
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Sets or replaces the EXIF thumbnail with the image in the file
 *
 * Returns: Boolean success value
 *
 */
gboolean		gexiv2_metadata_set_exif_thumbnail_from_file (GExiv2Metadata *self, const gchar *path, GError **error);

/**
 * gexiv2_metadata_set_exif_thumbnail_from_buffer:
 * @self: An instance of #GExiv2Metadata
 * @buffer: (array length=size): A buffer containing thumbnail data
 * @size: (skip): Size of the thumbnail's buffer
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_set_exif_thumbnail_from_buffer() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_exif_thumbnail_from_buffer)
void			gexiv2_metadata_set_exif_thumbnail_from_buffer (GExiv2Metadata *self, const guint8 *buffer, gint size);

/**
 * gexiv2_metadata_try_set_exif_thumbnail_from_buffer:
 * @self: An instance of #GExiv2Metadata
 * @buffer: (array length=size): A buffer containing thumbnail data
 * @size: (skip): Size of the thumbnail's buffer
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Since: 0.14.0
 */
void gexiv2_metadata_try_set_exif_thumbnail_from_buffer(GExiv2Metadata* self,
                                                        const guint8* buffer,
                                                        gint size,
                                                        GError** error);

/**
 * gexiv2_metadata_erase_exif_thumbnail:
 * @self: An instance of #GExiv2Metadata
 *
 * Removes the EXIF thumbnail from the loaded image.
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_erase_exif_thumbnail() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_erase_exif_thumbnail)
void			gexiv2_metadata_erase_exif_thumbnail (GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_erase_exif_thumbnail:
 * @self: An instance of #GExiv2Metadata
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Removes the EXIF thumbnail from the loaded image.
 *
 * Since: 0.14.0
 */
void gexiv2_metadata_try_erase_exif_thumbnail(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_get_exif_data:
 * @self: An instance of #GExiv2Metadata
 * @byte_order: Whether to export the data in little or big endian format
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Returns: (transfer full) (allow-none): The content of the EXIF data or %NULL on error
 *
 * Since: 0.12.2
 *
 */
GBytes *        gexiv2_metadata_get_exif_data (GExiv2Metadata *self, GExiv2ByteOrder byte_order, GError **error);

/*
 * XMP functions
 */

/**
 * gexiv2_metadata_has_xmp:
 * @self: An instance of #GExiv2Metadata
 *
 * Returns: TRUE if XMP metadata is present in the loaded image
 */
gboolean		gexiv2_metadata_has_xmp				(GExiv2Metadata *self);

/**
 * gexiv2_metadata_clear_xmp:
 * @self: An instance of #GExiv2Metadata
 *
 * Clears all XMP metadata from the loaded image.
 */
void			gexiv2_metadata_clear_xmp			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_generate_xmp_packet:
 * @self: An instance of #GExiv2Metadata
 * @xmp_format_flags: One of #GExiv2XmpFormatFlags
 * @padding: The padding (FIXME: Add documentation)
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Encode the XMP packet as a %NULL-terminated string.
 *
 * Returns: (transfer full) (allow-none): Encode the XMP packet and return as a %NULL-terminated string.
 *
 * Since: 0.12.2
 */
gchar*		gexiv2_metadata_try_generate_xmp_packet	(GExiv2Metadata *self, GExiv2XmpFormatFlags xmp_format_flags, guint32 padding, GError **error);

/**
 * gexiv2_metadata_generate_xmp_packet:
 * @self: An instance of #GExiv2Metadata
 * @xmp_format_flags: One of #GExiv2XmpFormatFlags
 * @padding: The padding (FIXME: Add documentation)
 *
 * Encode the XMP packet as a %NULL-terminated string.
 *
 * Returns: (transfer full) (allow-none): Encode the XMP packet and return as a %NULL-terminated string.
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_generate_xmp_packet() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_generate_xmp_packet)
gchar*		gexiv2_metadata_generate_xmp_packet	(GExiv2Metadata *self, GExiv2XmpFormatFlags xmp_format_flags, guint32 padding);

/**
 * gexiv2_metadata_try_get_xmp_packet:
 * @self: An instance of #GExiv2Metadata
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Returns: (transfer full) (allow-none): The currently-encoded XMP packet (see gexiv2_metadata_generate_xmp_packet).
 *
 * Since: 0.12.2
 */
gchar*			gexiv2_metadata_try_get_xmp_packet	(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_get_xmp_packet:
 * @self: An instance of #GExiv2Metadata
 *
 * Returns: (transfer full) (allow-none): The currently-encoded XMP packet (see gexiv2_metadata_generate_xmp_packet).
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_xmp_packet() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_xmp_packet)
gchar*			gexiv2_metadata_get_xmp_packet		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_xmp_tags:
 * @self: An instance of #GExiv2Metadata
 *
 * Returns: (transfer full) (array zero-terminated=1): A unique list of the available XMP tags
 */
gchar**			gexiv2_metadata_get_xmp_tags		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_register_xmp_namespace:
 * @name: (in): XMP URI name (should end in /)
 * @prefix: (in): XMP namespace prefix
 *
 * Returns: (skip): Boolean success value
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_register_xmp_namespace() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_register_xmp_namespace)
gboolean        gexiv2_metadata_register_xmp_namespace (const gchar* name, const gchar *prefix);

/**
 * gexiv2_metadata_try_register_xmp_namespace:
 * @name: (in): XMP URI name (should end in /)
 * @prefix: (in): XMP namespace prefix
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.14.0
 */
gboolean gexiv2_metadata_try_register_xmp_namespace(const gchar* name, const gchar* prefix, GError** error);

/**
 * gexiv2_metadata_unregister_xmp_namespace:
 * @name: (in): XMP URI name (should end in /)
 *
 * Returns: (skip): Boolean success value
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_unregister_xmp_namespace() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_unregister_xmp_namespace)
gboolean        gexiv2_metadata_unregister_xmp_namespace (const gchar* name);

/**
 * gexiv2_metadata_try_unregister_xmp_namespace:
 * @name: (in): XMP URI name (should end in /)
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.14.0
 */
gboolean gexiv2_metadata_try_unregister_xmp_namespace(const gchar* name, GError** error);

/**
 * gexiv2_metadata_unregister_all_xmp_namespaces:
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_unregister_all_xmp_namespaces() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_unregister_all_xmp_namespaces)
void            gexiv2_metadata_unregister_all_xmp_namespaces (void);

/**
 * gexiv2_metadata_try_unregister_all_xmp_namespaces:
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Since: 0.14.0
 */
void gexiv2_metadata_try_unregister_all_xmp_namespaces(GError** error);

/**
 * gexiv2_metadata_get_xmp_namespace_for_tag:
 * @tag: (in): Full tag name (e.g. "Xmp.dc.subject") or XMP namespace identifier (e.g. "dc")
 *
 * Returns: (transfer full): %NULL if there was no namespace registered for the tag, the URI of the namespace otherwise.
 *
 * Since: 0.12.2
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_get_xmp_namespace_for_tag() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_xmp_namespace_for_tag)
char* gexiv2_metadata_get_xmp_namespace_for_tag(const char* tag);

/**
 * gexiv2_metadata_try_get_xmp_namespace_for_tag:
 * @tag: (in): Full tag name (e.g. "Xmp.dc.subject") or XMP namespace identifier (e.g. "dc")
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Returns: (transfer full): %NULL if there was no namespace registered for the tag, the URI of the namespace otherwise.
 *
 * Since: 0.14.0
 */
char* gexiv2_metadata_try_get_xmp_namespace_for_tag(const char* tag, GError** error);

/* IPTC functions */

/**
 * gexiv2_metadata_has_iptc:
 * @self: An instance of #GExiv2Metadata
 *
 * Returns: TRUE if IPTC metadata is present in the loaded image
 */
gboolean		gexiv2_metadata_has_iptc			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_clear_iptc:
 * @self: An instance of #GExiv2Metadata
 *
 * Clears all IPTC metadata from the loaded image.
 */
void			gexiv2_metadata_clear_iptc			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_iptc_tags:
 * @self: An instance of #GExiv2Metadata
 *
 * Query @self for a list of available IPTC tags
 *
 * Returns: (transfer full) (array zero-terminated=1): A unique list of the available IPTC tags
 */
gchar**			gexiv2_metadata_get_iptc_tags		(GExiv2Metadata *self);

/*
 * Composite getters/setters and helpful functions.
 */

/**
 * gexiv2_metadata_get_orientation:
 * @self: An instance of #GExiv2Metadata
 *
 * The EXIF Orientation field
 *
 * Returns: A #GExiv2Orientation value representing the EXIF orientation
 * value.
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_get_orientation() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_orientation)
GExiv2Orientation gexiv2_metadata_get_orientation 	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_get_orientation:
 * @self: An instance of #GExiv2Metadata
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The EXIF Orientation field
 *
 * Returns: A #GExiv2Orientation value representing the EXIF orientation
 * value.
 *
 * Since: 0.14.0
 */
GExiv2Orientation gexiv2_metadata_try_get_orientation(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_set_orientation:
 * @self: An instance of #GExiv2Metadata
 * @orientation: The new #GExiv2Orientation for the image.
 *
 * The orientation must be valid and cannot be #GEXIV2_ORIENTATION_UNSPECIFIED.
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_set_orientation() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_orientation)
void			gexiv2_metadata_set_orientation		(GExiv2Metadata *self, GExiv2Orientation orientation);

/**
 * gexiv2_metadata_try_set_orientation:
 * @self: An instance of #GExiv2Metadata
 * @orientation: The new #GExiv2Orientation for the image.
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * The orientation must be valid and cannot be #GEXIV2_ORIENTATION_UNSPECIFIED.
 *
 * Since: 0.14.0
 */
void gexiv2_metadata_try_set_orientation(GExiv2Metadata* self, GExiv2Orientation orientation, GError** error);

/**
 * gexiv2_metadata_get_metadata_pixel_width:
 * @self: An instance of #GExiv2Metadata
 *
 * Composite accessor to query the pixel with stored in the metadata. This
 * might differ from the width of image that is available through
 * gexiv2_metadata_get_pixel_width()
 *
 * Returns: Width of images in pixels as stored in the metadata
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_get_metadata_pixel_width() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_metadata_pixel_width)
gint gexiv2_metadata_get_metadata_pixel_width (GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_get_metadata_pixel_width:
 * @self: An instance of #GExiv2Metadata
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Composite accessor to query the pixel with stored in the metadata. This
 * might differ from the width of image that is available through
 * gexiv2_metadata_get_pixel_width()
 *
 * Returns: Width of images in pixels as stored in the metadata
 *
 * Since: 0.14.0
 */
gint gexiv2_metadata_try_get_metadata_pixel_width(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_get_metadata_pixel_height:
 * @self: An instance of #GExiv2Metadata
 *
 * Composite accessor to query the pixel with stored in the metadata. This
 * might differ from the height of image that is available through
 * gexiv2_metadata_get_pixel_height()
 *
 * Returns: Height of images in pixels as stored in the metadata
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_get_metadata_pixel_height() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_metadata_pixel_height)
gint gexiv2_metadata_get_metadata_pixel_height (GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_get_metadata_pixel_height:
 * @self: An instance of #GExiv2Metadata
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Composite accessor to query the pixel with stored in the metadata. This
 * might differ from the height of image that is available through
 * gexiv2_metadata_get_pixel_height()
 *
 * Returns: Height of images in pixels as stored in the metadata
 *
 * Since: 0.14.0
 */
gint gexiv2_metadata_try_get_metadata_pixel_height(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_set_metadata_pixel_width:
 * @self: An instance of #GExiv2Metadata
 * @width: The width of the image as it should be put into the metadata
 *
 * Composite setter to update the image's metadata with @width
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_set_metadata_pixel_width() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_metadata_pixel_width)
void gexiv2_metadata_set_metadata_pixel_width (GExiv2Metadata *self, gint width);

/**
 * gexiv2_metadata_try_set_metadata_pixel_width:
 * @self: An instance of #GExiv2Metadata
 * @width: The width of the image as it should be put into the metadata
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Composite setter to update the image's metadata with @width
 *
 * Since: 0.14.0
 */
void gexiv2_metadata_try_set_metadata_pixel_width(GExiv2Metadata* self, gint width, GError** error);

/**
 * gexiv2_metadata_set_metadata_pixel_height:
 * @self: An instance of #GExiv2Metadata
 * @height: The width of the image as it should be put into the metadata.
 *
 * Update the image's metadata with @height
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_set_metadata_pixel_height() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_metadata_pixel_height)
void gexiv2_metadata_set_metadata_pixel_height (GExiv2Metadata *self, gint height);

/**
 * gexiv2_metadata_try_set_metadata_pixel_height:
 * @self: An instance of #GExiv2Metadata
 * @height: The width of the image as it should be put into the metadata.
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Update the image's metadata with @height
 *
 * Since: 0.14.0
 */
void gexiv2_metadata_try_set_metadata_pixel_height(GExiv2Metadata* self, gint height, GError** error);

/**
 * gexiv2_metadata_get_comment:
 * @self: An instance of #GExiv2Metadata
 *
 * A composite accessor that uses the first available metadata field from a list of well-known
 * locations to find the photo's comment (or description).
 *
 * MWG guidelines refer to these as <emphasis>Description</emphasis>: a textual description of a resource's content.
 *
 * These fields are:
 *
 * - Exif.Image.ImageDescription  (MWG Guidelines)
 * - Exif.Photo.UserComment
 * - Exif.Image.XPComment
 * - Iptc.Application2.Caption    (MWG Guidelines)
 * - Xmp.dc.description           (MWG Guidelines)
 * - Xmp.acdsee.notes             (Commonly requested, read only)
 *
 * <note>Note that in the EXIF specification Exif.Image.ImageDescription is
 * described  as "the title of the image".  Also, it does not support
 * two-byte character codes for encoding.  However, it's still used here for legacy reasons.
 * </note>
 *
 * For fine-grained control, it's recommended to use Exiv2 tags directly rather than this method,
 * which is more useful for quick or casual use.
 *
 * Returns: (transfer full) (allow-none): The photo's comment field.
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_get_comment() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_comment)
gchar*			gexiv2_metadata_get_comment			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_get_comment:
 * @self: An instance of #GExiv2Metadata
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * A composite accessor that uses the first available metadata field from a list of well-known
 * locations to find the photo's comment (or description).
 *
 * MWG guidelines refer to these as <emphasis>Description</emphasis>: a textual description of a resource's content.
 *
 * These fields are:
 *
 * - Exif.Image.ImageDescription  (MWG Guidelines)
 * - Exif.Photo.UserComment
 * - Exif.Image.XPComment
 * - Iptc.Application2.Caption    (MWG Guidelines)
 * - Xmp.dc.description           (MWG Guidelines)
 * - Xmp.acdsee.notes             (Commonly requested, read only)
 *
 * <note>Note that in the EXIF specification Exif.Image.ImageDescription is
 * described  as "the title of the image".  Also, it does not support
 * two-byte character codes for encoding.  However, it's still used here for legacy reasons.
 * </note>
 *
 * For fine-grained control, it's recommended to use Exiv2 tags directly rather than this method,
 * which is more useful for quick or casual use.
 *
 * Returns: (transfer full) (allow-none): The photo's comment field.
 *
 * Since: 0.14.0
 */
gchar* gexiv2_metadata_try_get_comment(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_set_comment:
 * @self: An instance of #GExiv2Metadata
 * @comment: Comment string to set. Must not be %NULL
 *
 * This is a composite setter that will set a number of fields to the supplied value.  See
 * #gexiv2_metadata_get_comment for more information.
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_set_comment() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_comment)
void			gexiv2_metadata_set_comment			(GExiv2Metadata *self, const gchar* comment);

/**
 * gexiv2_metadata_try_set_comment:
 * @self: An instance of #GExiv2Metadata
 * @comment: Comment string to set. Must not be %NULL
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * This is a composite setter that will set a number of fields to the supplied value.  See
 * #gexiv2_metadata_get_comment for more information.
 *
 * Since: 0.14.0
 */
void gexiv2_metadata_try_set_comment(GExiv2Metadata* self, const gchar* comment, GError** error);

/**
 * gexiv2_metadata_clear_comment:
 * @self: An instance of #GExiv2Metadata
 *
 * This is a composite clear method that will clear a number of fields.  See
 * #gexiv2_metadata_get_comment for more information.
 */
void			gexiv2_metadata_clear_comment		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_exposure_time:
 * @self: An instance of #GExiv2Metadata
 * @nom: (out): The numerator
 * @den: (out): The denominator
 *
 * Returns the exposure time in seconds (shutter speed, <emphasis>not</emphasis> date-time of exposure) as a
 * rational.  See <ulink url="https://en.wikipedia.org/wiki/Shutter_speed"></ulink> for more information.
 *
 * Returns: (skip): Boolean success value
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_get_exposure_time() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_exposure_time)
gboolean		gexiv2_metadata_get_exposure_time	(GExiv2Metadata *self, gint *nom, gint *den);

/**
 * gexiv2_metadata_try_get_exposure_time:
 * @self: An instance of #GExiv2Metadata
 * @nom: (out): The numerator
 * @den: (out): The denominator
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Returns the exposure time in seconds (shutter speed, <emphasis>not</emphasis> date-time of exposure) as a
 * rational.  See <ulink url="https://en.wikipedia.org/wiki/Shutter_speed"></ulink> for more information.
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.14.0
 */
gboolean gexiv2_metadata_try_get_exposure_time(GExiv2Metadata* self, gint* nom, gint* den, GError** error);

/**
 * gexiv2_metadata_get_fnumber:
 * @self: An instance of #GExiv2Metadata
 *
 * See <ulink url="https://en.wikipedia.org/wiki/F-number"></ulink> for more information.
 * If Exif.Photo.FNumber does not exist, it will fall back to calculating the FNumber from
 * Exif.Photo.ApertureValue (if available);
 *
 * Returns: The exposure Fnumber as a gdouble, or -1.0 if tag is not present or invalid.
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_get_fnumber() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_fnumber)
gdouble			gexiv2_metadata_get_fnumber			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_get_fnumber:
 * @self: An instance of #GExiv2Metadata
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * See <ulink url="https://en.wikipedia.org/wiki/F-number"></ulink> for more information.
 * If Exif.Photo.FNumber does not exist, it will fall back to calculating the FNumber from
 * Exif.Photo.ApertureValue (if available);
 *
 * Returns: The exposure Fnumber as a gdouble, or -1.0 if tag is not present or invalid.
 *
 * Since: 0.14.0
 */
gdouble gexiv2_metadata_try_get_fnumber(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_get_focal_length:
 * @self: An instance of #GExiv2Metadata
 *
 * See <ulink url="https://en.wikipedia.org/wiki/Flange_focal_distance"></ulink> for more information.
 *
 * Returns: The focal length as a gdouble, or -1.0 if tag is not present or invalid.
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_get_focal_length() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_focal_length)
gdouble			gexiv2_metadata_get_focal_length	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_get_focal_length:
 * @self: An instance of #GExiv2Metadata
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * See <ulink url="https://en.wikipedia.org/wiki/Flange_focal_distance"></ulink> for more information.
 *
 * Returns: The focal length as a gdouble, or -1.0 if tag is not present or invalid.
 *
 * Since: 0.14.0
 */
gdouble gexiv2_metadata_try_get_focal_length(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_get_iso_speed:
 * @self: An instance of #GExiv2Metadata
 *
 * See <ulink url="https://en.wikipedia.org/wiki/Iso_speed"></ulink> for more information.
 *
 * Returns: The ISO speed rating as a gint, or 0 if tag is not present or invalid.
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_get_iso_speed() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_iso_speed)
gint			gexiv2_metadata_get_iso_speed		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_get_iso_speed:
 * @self: An instance of #GExiv2Metadata
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * See <ulink url="https://en.wikipedia.org/wiki/Iso_speed"></ulink> for more information.
 *
 * Returns: The ISO speed rating as a gint, or 0 if tag is not present or invalid.
 *
 * Since: 0.14.0
 */
gint gexiv2_metadata_try_get_iso_speed(GExiv2Metadata* self, GError** error);

/*
 * GPS functions
 */

/**
 * gexiv2_metadata_try_get_gps_longitude:
 * @self: An instance of #GExiv2Metadata
 * @longitude: (out): Variable to store the longitude value
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Query the longitude stored in the GPS tags of @self
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.12.2
 */
gboolean		gexiv2_metadata_try_get_gps_longitude			(GExiv2Metadata *self, gdouble *longitude, GError **error);

/**
 * gexiv2_metadata_try_get_gps_latitude:
 * @self: An instance of #GExiv2Metadata
 * @latitude: (out): Variable to store the latitude value
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Query the latitude stored in the GPS tags of @self
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.12.2
 */
gboolean		gexiv2_metadata_try_get_gps_latitude			(GExiv2Metadata *self, gdouble *latitude, GError **error);

/**
 * gexiv2_metadata_try_get_gps_altitude:
 * @self: An instance of #GExiv2Metadata
 * @altitude: (out): Variable to store the altitude value
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Convenience function to query the altitude stored in the GPS tags of the
 * image
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.12.2
 */
gboolean		gexiv2_metadata_try_get_gps_altitude		(GExiv2Metadata *self, gdouble *altitude, GError **error);

/**
 * gexiv2_metadata_get_gps_longitude:
 * @self: An instance of #GExiv2Metadata
 * @longitude: (out): Variable to store the longitude value
 *
 * Query the longitude stored in the GPS tags of @self
 *
 * Returns: (skip): Boolean success value
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_gps_longitude() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_gps_longitude)
gboolean		gexiv2_metadata_get_gps_longitude			(GExiv2Metadata *self, gdouble *longitude);

/**
 * gexiv2_metadata_get_gps_latitude:
 * @self: An instance of #GExiv2Metadata
 * @latitude: (out): Variable to store the latitude value
 *
 * Query the latitude stored in the GPS tags of @self
 *
 * Returns: (skip): Boolean success value
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_gps_latitude() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_gps_latitude)
gboolean		gexiv2_metadata_get_gps_latitude			(GExiv2Metadata *self, gdouble *latitude);

/**
 * gexiv2_metadata_get_gps_altitude:
 * @self: An instance of #GExiv2Metadata
 * @altitude: (out): Variable to store the altitude value
 *
 * Convenience function to query the altitude stored in the GPS tags of the
 * image
 *
 * Returns: (skip): Boolean success value
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_gps_altitude() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_gps_altitude)
gboolean		gexiv2_metadata_get_gps_altitude			(GExiv2Metadata *self, gdouble *altitude);

/**
 * gexiv2_metadata_try_get_gps_info:
 * @self: An instance of #GExiv2Metadata
 * @longitude: (out): Storage for longitude value
 * @latitude: (out): Storage for latitude value
 * @altitude: (out): Storage for altitude value
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Convenience function to query all available GPS information at once.
 *
 * Returns: (skip): Boolean success value.
 *
 * Since: 0.12.2
 */
gboolean		gexiv2_metadata_try_get_gps_info			(GExiv2Metadata *self, gdouble *longitude, gdouble *latitude, gdouble *altitude, GError **error);

/**
 * gexiv2_metadata_get_gps_info:
 * @self: An instance of #GExiv2Metadata
 * @longitude: (out): Storage for longitude value
 * @latitude: (out): Storage for latitude value
 * @altitude: (out): Storage for altitude value
 *
 * Convenience function to query all available GPS information at once.
 *
 * Returns: (skip): Boolean success value.
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_get_gps_info() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_gps_info)
gboolean		gexiv2_metadata_get_gps_info				(GExiv2Metadata *self, gdouble *longitude, gdouble *latitude, gdouble *altitude);

/**
 * gexiv2_metadata_try_set_gps_info:
 * @self: An instance of #GExiv2Metadata
 * @longitude: Longitude value to set or replace current value
 * @latitude: Latitude value to set or replace current value
 * @altitude: Altitude value to set or replace current value
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Convenience function to create a new set of simple GPS data. Warning: Will remove any other
 * GPS information that is currently set. See gexiv2_metadata_update_gps_info() for
 * just modifying the GPS data.
 *
 * Returns: (skip): Boolean success value.
 *
 * Since: 0.12.2
 */
gboolean		gexiv2_metadata_try_set_gps_info			(GExiv2Metadata *self, gdouble longitude, gdouble latitude, gdouble altitude, GError **error);

/**
 * gexiv2_metadata_set_gps_info:
 * @self: An instance of #GExiv2Metadata
 * @longitude: Longitude value to set or replace current value
 * @latitude: Latitude value to set or replace current value
 * @altitude: Altitude value to set or replace current value
 *
 * Convenience function to create a new set of simple GPS data. Warning: Will remove any other
 * GPS information that is currently set. See gexiv2_metadata_update_gps_info() for
 * just modifying the GPS data.
 *
 * Returns: (skip): Boolean success value.
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_set_gps_info() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_gps_info)
gboolean		gexiv2_metadata_set_gps_info				(GExiv2Metadata *self, gdouble longitude, gdouble latitude, gdouble altitude);

/**
 * gexiv2_metadata_try_update_gps_info:
 * @self: An instance of #GExiv2Metadata
 * @longitude: Longitude value to set or replace current value
 * @latitude: Latitude value to set or replace current value
 * @altitude: Altitude value to set or replace current value
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Convenience function to update longitude, latitude and altitude at once.
 *
 * Returns: (skip): Boolean success value.
 *
 * Since: 0.12.2
 */
gboolean		gexiv2_metadata_try_update_gps_info			(GExiv2Metadata *self, gdouble longitude, gdouble latitude, gdouble altitude, GError **error);

/**
 * gexiv2_metadata_update_gps_info:
 * @self: An instance of #GExiv2Metadata
 * @longitude: Longitude value to set or replace current value
 * @latitude: Latitude value to set or replace current value
 * @altitude: Altitude value to set or replace current value
 *
 * Convenience function to update longitude, latitude and altitude at once.
 *
 * Returns: (skip): Boolean success value.
 *
 * Since: 0.12.1
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_update_gps_info() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_update_gps_info)
gboolean		gexiv2_metadata_update_gps_info				(GExiv2Metadata *self, gdouble longitude, gdouble latitude, gdouble altitude);

/**
 * gexiv2_metadata_try_delete_gps_info:
 * @self: An instance of #GExiv2Metadata
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Removes all GPS metadata from the loaded image
 *
 * Since: 0.12.2
 */
void			gexiv2_metadata_try_delete_gps_info			(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_delete_gps_info:
 * @self: An instance of #GExiv2Metadata
 *
 * Removes all GPS metadata from the loaded image
 *
 * Deprecated: 0.12.2: Use gexiv2_metadata_try_delete_gps_info() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_delete_gps_info)
void			gexiv2_metadata_delete_gps_info			(GExiv2Metadata *self);

/*
 * Preview Manager
 */

/**
 * gexiv2_metadata_get_preview_properties:
 * @self: An instance of #GExiv2Metadata
 *
 * An image may have stored one or more previews, often of different qualities, sometimes of
 * different image formats than the containing image.  This call returns the properties of all
 * previews Exiv2 finds within the loaded image.  Use #gexiv2_metadata_get_preview_image to
 * load a particular preview into memory.
 *
 * Returns: (transfer none) (allow-none) (array zero-terminated=1): An array of
 * #GExiv2PreviewProperties instances, one for each preview present in the loaded image.
 */
GExiv2PreviewProperties** gexiv2_metadata_get_preview_properties (GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_preview_image:
 * @self: An instance of #GExiv2Metadata
 * @props: A #GExiv2PreviewProperties instance
 *
 * Returns: (transfer full): A #GExiv2PreviewImage instance for the particular
 * #GExiv2PreviewProperties.
 *
 * Deprecated: 0.14.0: Use gexiv2_metadata_try_get_preview_image() instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_preview_image)
GExiv2PreviewImage* gexiv2_metadata_get_preview_image		(GExiv2Metadata *self, GExiv2PreviewProperties *props);

/**
 * gexiv2_metadata_try_get_preview_image:
 * @self: An instance of #GExiv2Metadata
 * @props: A #GExiv2PreviewProperties instance
 * @error: (allow-none): A return location for a #GError or %NULL
 *
 * Returns: (transfer full): A #GExiv2PreviewImage instance for the particular
 * #GExiv2PreviewProperties.
 *
 * Since: 0.14.0
 */
GExiv2PreviewImage* gexiv2_metadata_try_get_preview_image(GExiv2Metadata* self,
                                                          GExiv2PreviewProperties* props,
                                                          GError** error);

G_END_DECLS

#endif /* GEXIV2_METADATA_H */

