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
#include <gexiv2/gexiv2-preview-properties.h>
#include <gexiv2/gexiv2-preview-image.h>

G_BEGIN_DECLS

#define GEXIV2_TYPE_METADATA (gexiv2_metadata_get_type())

G_DECLARE_DERIVABLE_TYPE(GExiv2Metadata, gexiv2_metadata, GEXIV2, METADATA, GObject)

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
 * <http://jpegclub.org/exif_orientation.html>
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
 * Used in [method@Metadata.set_xmp_tag_struct] to determine the array type
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
 * Options to control the byte order of binary EXIF data exports, in [method@Metadata.get_exif_data].
 */
typedef enum {
  GEXIV2_BYTE_ORDER_LITTLE = 0,
  GEXIV2_BYTE_ORDER_BIG = 1
} GExiv2ByteOrder;

/**
 * GExiv2Metadata:
 *
 * An object holding all the Exiv2 metadata. Previews, if present, are also available.
 *
 * It is a generic object that provides everything from simple aggregated accessors
 * to common data such as image comments up to fine-grained access to specific tags of a
 * specfic format, be it EXIF, IPTC or XMP.
 *
 * As gexiv2 is only a wrapper around Exiv2, it's better to read its documentation to understand
 * the full scope of what it offers: <http://www.exiv2.org/>
 *
 * In particular, rather than providing a getter/setter method pair for every metadata value
 * available for images (of which there are thousands), Exiv2 uses a dotted addressing scheme.
 * For example, to access a photo's EXIF Orientation field, the caller passes to Exiv2
 * "Exif.Photo.Orientation".  These *tags* (in Exiv2 parlance) are key to using Exiv2 (and
 * therefore gexiv2) to its fullest.
 *
 * A full reference for all supported Exiv2 tags can be found at <http://www.exiv2.org/metadata.html>
 */

struct _GExiv2MetadataClass
{
	GObjectClass parent_class;
};

/* basic functions */

/**
 * gexiv2_metadata_new:
 *
 * Create an empty Metadata object.
 *
 * To use it, either use the many setter functions to popuplate the data from
 * scratch or use [method@GExiv2.Metadata.open_path] or
 * [method@GExiv2.Metadata.open_buf] to populate it from an existing file.
 *
 * Returns: (transfer full): A fully constructed [class@GExiv2.Metadata] ready to be used
 */
GExiv2Metadata* gexiv2_metadata_new					(void);

/**
 * gexiv2_metadata_open_path:
 * @self: An instance of [class@GExiv2.Metadata]
 * @path: Path to the file you want to open
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Populate metadata from @path.
 *
 * The file must be an image format supported by Exiv2. If called multiple times,
 * current metadata will be replaced by the content of the last file opened.
 *
 * Returns: Boolean success indicator
 *
 *
 */
gboolean		gexiv2_metadata_open_path			(GExiv2Metadata *self, const gchar *path, GError **error);

/**
 * gexiv2_metadata_open_buf:
 * @self: An instance of [class@GExiv2.Metadata]
 * @data: (array length=n_data): A buffer containing the data to be read
 * @n_data: (skip): The length of the buffer
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Populate metadata from a memory buffer.
 *
 * The buffer must be an image format supported by Exiv2. If called multiple times,
 * current metadata will be replaced by the content of the last file opened.
 *
 * When called on an already filled meta-data object (i.e. one that has already
 * been filled by a previous call of [method@GExiv2.Metadata.open_path]) and the
 * opening of the new path fails, the object will not revert to its previous
 * state but be in a similar state after calling [method@GExiv2.Metadata.new].
 *
 * Returns: Boolean success indicator
 *
 */
gboolean		gexiv2_metadata_open_buf			(GExiv2Metadata *self, const guint8 *data, glong n_data, GError **error);

/**
 * gexiv2_metadata_from_stream:
 * @stream:
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Read metadata from a [class@Gio.InputStream]. This function is not very
 * efficient in places since it needs to copy memory to fullfil underlying
 * requirements by exiv2, which expects everything to be memory-mappable.
 *
 */
gboolean		gexiv2_metadata_from_stream			(GExiv2Metadata *self, GInputStream* stream, GError **error);

/**
 * gexiv2_metadata_from_app1_segment:
 * @self: An instance of [class@GExiv2.Metadata]
 * @data: (array length=n_data): A buffer containing the data to be read
 * @n_data: (skip): The length of the buffer
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Load only an EXIF buffer, typically stored in a JPEG's APP1 segment.
 *
 * Returns: Boolean success indicator.
 *
 */
gboolean		gexiv2_metadata_from_app1_segment	(GExiv2Metadata *self, const guint8 *data, glong n_data, GError **error);

/**
 * gexiv2_metadata_save_external:
 * @self: An instance of [class@GExiv2.Metadata]
 * @path: Path to the file you want to save to.
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
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
 * @self: An instance of [class@GExiv2.Metadata]
 * @path: Path to the file you want to save to.
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Saves the metadata to the specified file by reading the file into memory, copying this object's
 * metadata into the image, then writing the image back out.
 *
 * Returns: Boolean success indicator.
 *
 */
gboolean		gexiv2_metadata_save_file			(GExiv2Metadata *self, const gchar *path, GError **error);

/**
 * gexiv2_metadata_as_bytes:
 * @self: An instance of [class@GExiv2.Metadata]
 * @bytes: (allow-none): An image buffer to update the metadata on, nor %NULL
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Saves the metadata to the stream by reading the stream into memory, copying this object's
 * metadata into the image, then writing the image as a stream back out.
 *
 * if @bytes is %NULL, a copy of the internal image with updated metadata will be returned.
 *
 * Returns: (transfer full): A newly allocated GBytes object containing the image with new metadata
 *
 * Since: 0.16.0
 *
 */
GBytes* gexiv2_metadata_as_bytes(GExiv2Metadata* self, GBytes* bytes, GError** error);

/**
 * gexiv2_metadata_has_tag:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 *
 * Check for presence of a tag.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: %TRUE if the tag is present.
 * Since: 0.16.0
 */
gboolean		gexiv2_metadata_has_tag				(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_try_has_tag:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: TRUE if the tag is present.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.has_tag] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_has_tag)
gboolean gexiv2_metadata_try_has_tag(GExiv2Metadata* self, const gchar* tag, GError** error);

/**
 * gexiv2_metadata_clear_tag:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Removes the Exiv2 tag from the metadata object.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: %TRUE if the tag was present.
 *
 * Since: 0.16.0
 */
gboolean		gexiv2_metadata_clear_tag			(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_try_clear_tag:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Removes the Exiv2 tag from the metadata object.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: TRUE if the tag was present.
 *
 * Since: 0.14.0
 * Deprecated: 0.14.0: Use [method@Metadata.clear_tag] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_clear_tag)
gboolean gexiv2_metadata_try_clear_tag(GExiv2Metadata* self, const gchar* tag, GError** error);

/**
 * gexiv2_metadata_clear:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Removes all tags for all domains (EXIF, IPTC, and XMP).
 */
void			gexiv2_metadata_clear				(GExiv2Metadata *self);

/**
 * gexiv2_metadata_is_exif_tag:
 * @tag: An Exiv2 tag
 *
 * Check whether @tag is from the EXIF domain of tags.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: TRUE if the Exiv2 tag is for the EXIF domain.
 */
gboolean		gexiv2_metadata_is_exif_tag				(const gchar* tag);

/**
 * gexiv2_metadata_is_iptc_tag:
 * @tag: An Exiv2 tag
 *
 * Check whether @tag is from the IPTC domain of tags.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: TRUE if the Exiv2 tag is for the IPTC domain.
 */
gboolean		gexiv2_metadata_is_iptc_tag				(const gchar* tag);

/**
 * gexiv2_metadata_is_xmp_tag:
 * @tag: An Exiv2 tag
 *
 * Check whether @tag is from the XMP domain of tags.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: TRUE if the Exiv2 tag is for the XMP domain.
 */
gboolean		gexiv2_metadata_is_xmp_tag				(const gchar* tag);

/**
 * gexiv2_metadata_try_get_tag_label:
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Get Exiv2's label for this tag.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: (transfer none) (allow-none): The tag's label
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [func@Metadata.get_tag_label] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_tag_label)
const gchar*	gexiv2_metadata_try_get_tag_label		(const gchar *tag, GError **error);

/**
 * gexiv2_metadata_get_tag_label:
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Get Exiv2's label for this tag.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: (transfer none) (allow-none): The tag's label
 *
 */
const gchar*	gexiv2_metadata_get_tag_label		(const gchar *tag, GError **error);

/**
 * gexiv2_metadata_try_get_tag_description:
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Get Exiv2's description for a tag.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: (transfer none) (allow-none): The tag's description
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [func@Metadata.get_tag_description] instead.
 *
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_tag_description)
const gchar*	gexiv2_metadata_try_get_tag_description	(const gchar *tag, GError **error);

/**
 * gexiv2_metadata_get_tag_description:
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Get Exiv2's description for a tag.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: (transfer none) (allow-none): The tag's description
 *
 * Since: 0.16.0
 */
const gchar*	gexiv2_metadata_get_tag_description	(const gchar *tag, GError **error);

/**
 * gexiv2_metadata_try_get_tag_type:
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Get Exiv2's type name of a tag.
 *
 * The names of the various Exiv2 tag types can be found at Exiv2::TypeId,
 * <http://exiv2.org/doc/namespaceExiv2.html#a5153319711f35fe81cbc13f4b852450c>
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: (transfer none) (allow-none): The tag's type name.
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [func@Metadata.get_tag_type] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_tag_type)
const gchar*	gexiv2_metadata_try_get_tag_type	(const gchar *tag, GError **error);

/**
 * gexiv2_metadata_get_tag_type:
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Get Exiv2's type name of a tag.
 *
 * The names of the various Exiv2 tag types can be found at Exiv2::TypeId,
 * <http://exiv2.org/doc/namespaceExiv2.html#a5153319711f35fe81cbc13f4b852450c>
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: (transfer none) (allow-none): The tag's type name.
 * Since: 0.16.0
 */
const gchar*	gexiv2_metadata_get_tag_type	(const gchar *tag, GError **error);

/**
 * gexiv2_metadata_try_tag_supports_multiple_values:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Check whether a tag supports multiple values.
 *
 * Multiple value tags are Xmp tags of type "XmpAlt", "XmpBag", "XmpSeq" or "LangAlt", or Iptc
 * tags marked as Repeatable (which can be of any Iptc type). There are no multiple value Exif
 * tags.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: Whether @tag is capable of storing multiple values or not. If @tag is undefined
 * (i.e. not built-in and not added to @self), then @error is set and %FALSE is returned.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0. Use [method@GExiv2.Metadata.tag_supports_multiple_values] instead.
 */
gboolean gexiv2_metadata_try_tag_supports_multiple_values(GExiv2Metadata* self, const gchar* tag, GError** error);

/**
 * gexiv2_metadata_tag_supports_multiple_values:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: An Exiv2 tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Check whether a tag supports multiple values.
 *
 * Multiple value tags are Xmp tags of type "XmpAlt", "XmpBag", "XmpSeq" or "LangAlt", or Iptc
 * tags marked as Repeatable (which can be of any Iptc type). There are no multiple value Exif
 * tags.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: Whether @tag is capable of storing multiple values or not. If @tag is undefined
 * (i.e. not built-in and not added to @self), then @error is set and %FALSE is returned.
 *
 * Since: 0.16.0
 */
gboolean gexiv2_metadata_tag_supports_multiple_values(GExiv2Metadata* self, const gchar* tag, GError** error);

/**
 * gexiv2_metadata_get_supports_exif:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Query whether the currently loaded image supports writing of EXIF metadata.
 *
 * Returns: %TRUE if the loaded image type supports writing EXIF metadata.
 */
gboolean		gexiv2_metadata_get_supports_exif	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_supports_iptc:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Query whether the currently loaded image supports writing of IPTC metadata.
 *
 * Returns: %TRUE if the loaded image type supports writing IPTC metadata.
 */
gboolean		gexiv2_metadata_get_supports_iptc	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_supports_xmp:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Query whether the currently loaded image supports writing of XMP metadata.
 *
 * Returns: %TRUE if the loaded image type supports writing XMP metadata.
 */
gboolean		gexiv2_metadata_get_supports_xmp	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_mime_type:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Query mime type of currently loaded image.
 *
 * Returns: (transfer none): The MIME type of the loaded image, %NULL if not loaded or unknown.
 */
const gchar*	gexiv2_metadata_get_mime_type		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_pixel_width:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Get the *actual* unoriented display width in pixels of the loaded
 * image. May be different than the width reported by various metadata tags,
 * i.e. `Exif.Photo.PixelXDimension`.
 *
 * Returns: Pixel width of current image
 */
gint			gexiv2_metadata_get_pixel_width		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_pixel_height:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Get the *actual* unoriented display height in pixels of the loaded image.  This may
 * be different than the height reported by various metadata tags, i.e. `Exif.Photo.PixelYDimension`.
 *
 * Returns: Pixel height of current image
 */
gint			gexiv2_metadata_get_pixel_height	(GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_get_tag_string:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Get a string representationf a tag.
 *
 * Tags that support multiple values are returned as a single string, with elements separated by ", ".
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: (transfer full) (allow-none): The tag's value as a string
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.get_tag_string] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_tag_string)
gchar*			gexiv2_metadata_try_get_tag_string	(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_try_set_tag_string:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @value: The value to set or replace the existing value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Set the value of a tag using a string.
 *
 * If a tag supports multiple values, then @value is added to any existing values. For single
 * tags, @value replaces the value.
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: TRUE on success
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.set_tag_string] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_set_tag_string)
gboolean		gexiv2_metadata_try_set_tag_string	(GExiv2Metadata *self, const gchar* tag, const gchar* value, GError **error);

/**
 * gexiv2_metadata_get_tag_string:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Tags that support multiple values are returned as a single string, with elements separated by ", ".
 *
 * In case of error, a GLib warning will be logged. Use instead
 * [method@Metadata.try_get_tag_string] if you want to avoid this and
 * control if and how the error is outputted.
 *
 * Returns: (transfer full) (allow-none): The tag's value as a string
 *
 * Since: 0.16.0
 */
gchar*			gexiv2_metadata_get_tag_string		(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_set_tag_string:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @value: The value to set or replace the existing value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * If a tag supports multiple values, then @value is added to any existing values. For single
 * value tags, @value replaces the value.
 *
 * In case of error, a GLib warning will be logged. Use instead
 * [method@Metadata.try_set_tag_string] if you want to avoid this and
 * control if and how the error is outputted.
 *
 * Returns: TRUE on success
 *
 * Since: 0.16.0
 */
gboolean		gexiv2_metadata_set_tag_string		(GExiv2Metadata *self, const gchar* tag, const gchar* value, GError **error);

/**
 * gexiv2_metadata_try_set_xmp_tag_struct:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @type: The GExiv2StructureType specifying the type of structure
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: TRUE on success
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.set_xmp_tag_struct] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_set_xmp_tag_struct)
gboolean gexiv2_metadata_try_set_xmp_tag_struct (GExiv2Metadata *self, const gchar* tag, GExiv2StructureType type, GError **error);

/**
 * gexiv2_metadata_set_xmp_tag_struct:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @type: The GExiv2StructureType specifying the type of structure
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: TRUE on success
 * Since: 0.16.0
 */
gboolean gexiv2_metadata_set_xmp_tag_struct (GExiv2Metadata *self, const gchar* tag, GExiv2StructureType type, GError **error);

/**
 * gexiv2_metadata_try_get_tag_interpreted_string:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * An interpreted string is one fit for user display.  It may display units or use formatting
 * appropriate to the type of data the tag holds.
 *
 * Tags that support multiple values are returned as a single string, with elements separated by ", ".
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: (transfer full) (allow-none): The tag's interpreted value as a string
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.get_tag_interpreted_string] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_tag_interpreted_string)
gchar*			gexiv2_metadata_try_get_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_get_tag_interpreted_string:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * An interpreted string is one fit for user display.  It may display units or use formatting
 * appropriate to the type of data the tag holds.
 *
 * Tags that support multiple values are returned as a single string, with elements separated by ", ".
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: (transfer full) (allow-none): The tag's interpreted value as a string
 * Since: 0.16.0
 */
gchar*			gexiv2_metadata_get_tag_interpreted_string (GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_try_get_tag_long:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: The tag's value as a glong
 *
 * Since: 0.12.2
 * Deprecated: 0.12.2: Use [method@Metadata.get_tag_long] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_tag_long)
glong			gexiv2_metadata_try_get_tag_long	(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_get_tag_long:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: The tag's value as a glong
 * Since: 0.16.0
 */
glong			gexiv2_metadata_get_tag_long		(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_try_set_tag_long:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @value: The value to set or replace the existing value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: TRUE on success
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.set_tag_long] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_set_tag_long)
gboolean		gexiv2_metadata_try_set_tag_long	(GExiv2Metadata *self, const gchar* tag, glong value, GError **error);

/**
 * gexiv2_metadata_set_tag_long:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @value: The value to set or replace the existing value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: TRUE on success
 * Since: 0.16.0
 */
gboolean		gexiv2_metadata_set_tag_long		(GExiv2Metadata *self, const gchar* tag, glong value, GError **error);

/**
 * gexiv2_metadata_try_get_tag_multiple:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Returns: (transfer full) (allow-none) (array zero-terminated=1): The multiple string values of
 * @tag.  Returns %NULL if parameters are %NULL or @tag does not begin with recognised type of
 * metadata ("Exif.", "Xmp." or "Iptc.").  For a well formed @tag, returns array[0] = %NULL if @tag
 * is undefined or is not set in the current metadata.
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.get_tag_multiple] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_tag_multiple)
gchar**			gexiv2_metadata_try_get_tag_multiple	(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_try_set_tag_multiple:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @values: (array zero-terminated=1): An array of values to set or replace the existing value(s)
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * All previous @tag values are erased. For multiple value tags, each of the non %NULL
 * entries in @values is stored. For single value tags, only the last non %NULL value
 * is assigned.
 *
 * Returns: Boolean success value
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.set_tag_multiple] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_set_tag_multiple)
gboolean		gexiv2_metadata_try_set_tag_multiple	(GExiv2Metadata *self, const gchar* tag, const gchar** values, GError **error);

/**
 * gexiv2_metadata_get_tag_multiple:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * In case of error, a GLib warning will be logged. Use instead
 * [method@Metadata.try_get_tag_multiple] if you want to avoid this and
 * control if and how the error is outputted.
 *
 * Returns: (transfer full) (allow-none) (array zero-terminated=1): The multiple string values of
 * the tag.  Returns %NULL if parameters are %NULL or @tag does not begin with recognised type of
 * metadata ("Exif.", "Xmp." or "Iptc.").  For a well formed @tag, returns array[0] = %NULL if @tag
 * is undefined or is not set in the current metadata.
 *
 * Since: 0.16.0
 */
gchar**			gexiv2_metadata_get_tag_multiple	(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_set_tag_multiple:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @values: (array zero-terminated=1): An array of values to set or replace the existing value(s)
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * All previous @tag values are erased. For multiple value tags, each of the non %NULL
 * entries in @values is stored. For single value tags, only the last non %NULL value
 * is assigned.
 *
 * Returns: Boolean success value
 *
 */
gboolean		gexiv2_metadata_set_tag_multiple	(GExiv2Metadata *self, const gchar* tag, const gchar** values, GError **error);

/**
 * gexiv2_metadata_try_get_tag_raw:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Tags that support multiple values may be returned as a single byte array, with records separated
 * by 4x INFORMATION SEPARATOR FOUR (ASCII 0x1c)
 *
 * Returns: (transfer full) (allow-none): The tag's raw value as a byte array
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.get_tag_raw] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_tag_raw)
GBytes*			gexiv2_metadata_try_get_tag_raw		(GExiv2Metadata *self, const gchar* tag, GError **error);

/**
 * gexiv2_metadata_get_tag_raw:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: Exiv2 tag name
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The Exiv2 Tag Reference can be found at <http://exiv2.org/metadata.html>
 *
 * Tags that support multiple values may bereturned as a single byte array, with records separated
 * by 4x INFORMATION SEPARATOR FOUR (ASCII 0x1c)
 *
 * Returns: (transfer full) (allow-none): The tag's raw value as a byte array
 * Since: 0.16.0
 */
GBytes*			gexiv2_metadata_get_tag_raw			(GExiv2Metadata *self, const gchar* tag, GError **error);

/*
 * EXIF functions
 */

/**
 * gexiv2_metadata_has_exif:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Whether the metadata contains EXIF data.
 *
 * Returns: TRUE if EXIF metadata is present in the loaded image
 */
gboolean		gexiv2_metadata_has_exif			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_clear_exif:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Clears all EXIF metadata from the loaded image.
 */
void			gexiv2_metadata_clear_exif			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_exif_tags:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Query @self for a list of available EXIF tags
 *
 * Returns: (transfer full) (array zero-terminated=1): A unique list of the available EXIF tags in the
 * loaded image
 */
gchar**			gexiv2_metadata_get_exif_tags		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_get_exif_tag_rational:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: (in): The tag you want the rational value for
 * @nom: (out): The numerator
 * @den: (out): The denominator
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Fetch EXIF @tag represented by a fraction. @nom will contain the numerator,
 * @den the denominator of the fraction on successful return.
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.get_exif_tag_rational] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_exif_tag_rational)
gboolean		gexiv2_metadata_try_get_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint* nom, gint* den, GError **error);

/**
 * gexiv2_metadata_try_set_exif_tag_rational:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: (in): The Exiv2 tag
 * @nom: Rational numerator
 * @den: Rational denominator
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Set EXIF @tag represented by a fraction, with @nom being the numerator,
 * @den the denominator of the fraction.
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.set_exif_tag_rational] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_set_exif_tag_rational)
gboolean		gexiv2_metadata_try_set_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint nom, gint den, GError **error);

/**
 * gexiv2_metadata_get_exif_tag_rational:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: (in): The tag you want the rational value for
 * @nom: (out): The numerator
 * @den: (out): The denominator
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Fetch EXIF @tag represented by a fraction. @nom will contain the numerator,
 * @den the denominator of the fraction on successful return.
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.16.0
 */
gboolean		gexiv2_metadata_get_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint* nom, gint* den, GError **error);

/**
 * gexiv2_metadata_set_exif_tag_rational:
 * @self: An instance of [class@GExiv2.Metadata]
 * @tag: (in): The Exiv2 tag
 * @nom: Rational numerator
 * @den: Rational denominator
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Set EXIF @tag represented by a fraction, with @nom being the numerator,
 * @den the denominator of the fraction.
 *
 * Returns: (skip): Boolean success value
 * Since: 0.16.0
 */
gboolean		gexiv2_metadata_set_exif_tag_rational (GExiv2Metadata *self, const gchar* tag, gint nom, gint den, GError **error);

/**
 * gexiv2_metadata_get_exif_thumbnail:
 * @self: An instance of [class@GExiv2.Metadata]
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
 * @self: An instance of [class@GExiv2.Metadata]
 * @path: (in): Path of image file
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Sets or replaces the EXIF thumbnail with the image in the file
 *
 * Returns: Boolean success value
 *
 */
gboolean		gexiv2_metadata_set_exif_thumbnail_from_file (GExiv2Metadata *self, const gchar *path, GError **error);

/**
 * gexiv2_metadata_set_exif_thumbnail_from_buffer:
 * @self: An instance of [class@GExiv2.Metadata]
 * @buffer: (array length=size): A buffer containing thumbnail data
 * @size: (skip): Size of the thumbnail's buffer
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Sets or replaces the EXIF thumbnail with the data in @buffer.
 * Since: 0.16.0
 */
void			gexiv2_metadata_set_exif_thumbnail_from_buffer (GExiv2Metadata *self, const guint8 *buffer, gint size, GError **error);

/**
 * gexiv2_metadata_try_set_exif_thumbnail_from_buffer:
 * @self: An instance of [class@GExiv2.Metadata]
 * @buffer: (array length=size): A buffer containing thumbnail data
 * @size: (skip): Size of the thumbnail's buffer
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Sets or replaces the EXIF thumbnail with the data in @buffer.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.set_exif_thumbnail_from_buffer] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_set_exif_thumbnail_from_buffer)
void gexiv2_metadata_try_set_exif_thumbnail_from_buffer(GExiv2Metadata* self,
                                                        const guint8* buffer,
                                                        gint size,
                                                        GError** error);

/**
 * gexiv2_metadata_erase_exif_thumbnail:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Removes the EXIF thumbnail from the loaded image.
 *
 */
void			gexiv2_metadata_erase_exif_thumbnail (GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_try_erase_exif_thumbnail:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Removes the EXIF thumbnail from the loaded image.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.erase_exif_thumbnail] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_erase_exif_thumbnail)
void gexiv2_metadata_try_erase_exif_thumbnail(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_get_exif_data:
 * @self: An instance of [class@GExiv2.Metadata]
 * @byte_order: Whether to export the data in little or big endian format
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Returns a binary blob of the contained EXIF data, if the image contains any.
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
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Whether the metadata contains XMP data.
 *
 * Returns: TRUE if XMP metadata is present in the loaded image
 */
gboolean		gexiv2_metadata_has_xmp				(GExiv2Metadata *self);

/**
 * gexiv2_metadata_clear_xmp:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Clears all XMP metadata from the loaded image.
 */
void			gexiv2_metadata_clear_xmp			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_try_generate_xmp_packet:
 * @self: An instance of [class@GExiv2.Metadata]
 * @xmp_format_flags: One of #GExiv2XmpFormatFlags
 * @padding: The padding before the closing `<?xpacket>` tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Encode the XMP packet as a %NULL-terminated string.
 *
 * Returns: (transfer full) (allow-none): Encode the XMP packet and return as a %NULL-terminated string.
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.try_generate_xmp_packet] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_generate_xmp_packet)
gchar*		gexiv2_metadata_try_generate_xmp_packet	(GExiv2Metadata *self, GExiv2XmpFormatFlags xmp_format_flags, guint32 padding, GError **error);

/**
 * gexiv2_metadata_generate_xmp_packet:
 * @self: An instance of [class@GExiv2.Metadata]
 * @xmp_format_flags: One of #GExiv2XmpFormatFlags
 * @padding: The padding before the closing `<?xpacket>` tag
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Encode the XMP packet as a %NULL-terminated string.
 *
 * Returns: (transfer full) (allow-none): Encode the XMP packet and return as a %NULL-terminated string.
 * Since: 0.16.0
 */
gchar*		gexiv2_metadata_generate_xmp_packet	(GExiv2Metadata *self, GExiv2XmpFormatFlags xmp_format_flags, guint32 padding, GError **error);

/**
 * gexiv2_metadata_try_get_xmp_packet:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Get the currently encoded XMP packet (after having called [method@GExiv2.Metadata.try_generate_xmp_packet])
 *
 * Returns: (transfer full) (allow-none): %NULL if no packet was generated previously, the XMP packet contents otherwise
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.try_get_xmp_packet] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_xmp_packet)
gchar*			gexiv2_metadata_try_get_xmp_packet	(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_get_xmp_packet:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Get the currently encoded XMP packet (after having called [method@GExiv2.Metadata.try_get_xmp_packet])

 * Returns: (transfer full) (allow-none): The currently-encoded XMP packet (see [method@Metadata.generate_xmp_packet]).
 * Since: 0.16.0
 */
gchar*			gexiv2_metadata_get_xmp_packet		(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_get_xmp_tags:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Get the XMP data from the image.
 *
 * This could contain multiple XML snippets.
 *
 * Returns: (transfer full) (array zero-terminated=1): A unique list of the available XMP tags
 */
gchar**			gexiv2_metadata_get_xmp_tags		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_register_xmp_namespace:
 * @name: (in): XMP URI name (should end in /)
 * @prefix: (in): XMP namespace prefix
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Register an additional XMP namespace.
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.16.0
 */
gboolean        gexiv2_metadata_register_xmp_namespace (const gchar* name, const gchar *prefix, GError **error);

/**
 * gexiv2_metadata_try_register_xmp_namespace:
 * @name: (in): XMP URI name (should end in /)
 * @prefix: (in): XMP namespace prefix
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Register an additional XMP namespace.
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [func@Metadata.register_xmp_namespace] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_register_xmp_namespace)
gboolean gexiv2_metadata_try_register_xmp_namespace(const gchar* name, const gchar* prefix, GError** error);

/**
 * gexiv2_metadata_unregister_xmp_namespace:
 * @name: (in): XMP URI name (should end in /)
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Unregister a namespace previously registered with
 * [func@Metadata.try_register_xmp_namespace].
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.16.0
 */
gboolean        gexiv2_metadata_unregister_xmp_namespace (const gchar* name, GError **error);

/**
 * gexiv2_metadata_try_unregister_xmp_namespace:
 * @name: (in): XMP URI name (should end in /)
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Unregister a namespace previously registered with
 * [func@Metadata.try_register_xmp_namespace].
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [func@Metadata.try_unregister_xmp_namespace] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_unregister_xmp_namespace)
gboolean gexiv2_metadata_try_unregister_xmp_namespace(const gchar* name, GError** error);

/**
 * gexiv2_metadata_unregister_all_xmp_namespaces:
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Unregister all XMP namespaces that have been previously registered with
 * [func@Metadata.try_register_xmp_namespace].
 * Since: 0.16.0
 */
void            gexiv2_metadata_unregister_all_xmp_namespaces (GError** error);

/**
 * gexiv2_metadata_try_unregister_all_xmp_namespaces:
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Unregister all XMP namespaces that have been previously registered with
 * [func@Metadata.try_register_xmp_namespace].
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [func@Metadata.register_xmp_namespace] instead
 */
G_DEPRECATED_FOR(gexiv2_metadata_unregister_all_xmp_namespaces)
void gexiv2_metadata_try_unregister_all_xmp_namespaces(GError** error);

/**
 * gexiv2_metadata_get_xmp_namespace_for_tag:
 * @tag: (in): Full tag name (e.g. "Xmp.dc.subject") or XMP namespace identifier (e.g. "dc")
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Look up the URI for the namespace for @tag
 *
 * Returns: (transfer full): %NULL if there was no namespace registered for the tag, the URI of the namespace otherwise.
 *
 * Since: 0.16.0
 *
 */
char* gexiv2_metadata_get_xmp_namespace_for_tag(const char* tag, GError **error);

/**
 * gexiv2_metadata_try_get_xmp_namespace_for_tag:
 * @tag: (in): Full tag name (e.g. "Xmp.dc.subject") or XMP namespace identifier (e.g. "dc")
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Look up the URI for the namespace for @tag
 *
 * Returns: (transfer full): %NULL if there was no namespace registered for the tag, the URI of the namespace otherwise.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [func@Metadata.try_get_xmp_namespace_for_tag] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_get_xmp_namespace_for_tag)
char* gexiv2_metadata_try_get_xmp_namespace_for_tag(const char* tag, GError** error);

/* IPTC functions */

/**
 * gexiv2_metadata_has_iptc:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Whether the image has IPTC metadata
 *
 * Returns: TRUE if IPTC metadata is present in the loaded image
 */
gboolean		gexiv2_metadata_has_iptc			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_clear_iptc:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * Clears all IPTC metadata from the loaded image.
 */
void			gexiv2_metadata_clear_iptc			(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_iptc_tags:
 * @self: An instance of [class@GExiv2.Metadata]
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
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The EXIF Orientation field
 *
 * Returns: A [enum@GExiv2.Orientation] value representing the EXIF orientation
 * value.
 *
 * Since: 0.16.0
 */
GExiv2Orientation gexiv2_metadata_get_orientation 	(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_try_get_orientation:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The EXIF Orientation field
 *
 * Returns: A [enum@GExiv2.Orientation] value representing the EXIF orientation
 * value.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.get_orientation] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_orientation)
GExiv2Orientation gexiv2_metadata_try_get_orientation(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_set_orientation:
 * @self: An instance of [class@GExiv2.Metadata]
 * @orientation: The new [enum@GExiv2.Orientation] for the image.
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The orientation must be valid and cannot be [enum@GExiv2.Orientation.UNSPECIFIED].
 *
 * Since: 0.16.0
 */
void			gexiv2_metadata_set_orientation		(GExiv2Metadata *self, GExiv2Orientation orientation, GError **error);

/**
 * gexiv2_metadata_try_set_orientation:
 * @self: An instance of [class@GExiv2.Metadata]
 * @orientation: The new [enum@GExiv2.Orientation] for the image.
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * The orientation must be valid and cannot be [enum@GExiv2.Orientation.UNSPECIFIED.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.set_orientation] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_set_orientation)
void gexiv2_metadata_try_set_orientation(GExiv2Metadata* self, GExiv2Orientation orientation, GError** error);

/**
 * gexiv2_metadata_get_metadata_pixel_width:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Composite accessor to query the pixel with stored in the metadata. This
 * might differ from the width of image that is available through
 * [method@Metadata.get_pixel_width]
 *
 * Returns: Width of images in pixels as stored in the metadata
 *
 * Since: 0.16.0
 */
gint gexiv2_metadata_get_metadata_pixel_width (GExiv2Metadata *self, GError** error);

/**
 * gexiv2_metadata_try_get_metadata_pixel_width:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Composite accessor to query the pixel with stored in the metadata. This
 * might differ from the width of image that is available through
 * [method@Metadata.get_pixel_width]
 *
 * Returns: Width of images in pixels as stored in the metadata
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.get_metadata_pixel_width] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_metadata_pixel_width)
gint gexiv2_metadata_try_get_metadata_pixel_width(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_get_metadata_pixel_height:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Composite accessor to query the pixel with stored in the metadata. This
 * might differ from the height of image that is available through
 * [method@Metadata.get_pixel_height]
 *
 * Returns: Height of images in pixels as stored in the metadata
 * Since: 0.16.0
 */
gint gexiv2_metadata_get_metadata_pixel_height (GExiv2Metadata *self, GError** error);

/**
 * gexiv2_metadata_try_get_metadata_pixel_height:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Composite accessor to query the pixel with stored in the metadata. This
 * might differ from the height of image that is available through
 * [method@Metadata.get_pixel_height]
 *
 * Returns: Height of images in pixels as stored in the metadata
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.get_metadata_pixel_height] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_metadata_pixel_height)
gint gexiv2_metadata_try_get_metadata_pixel_height(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_set_metadata_pixel_width:
 * @self: An instance of [class@GExiv2.Metadata]
 * @width: The width of the image as it should be put into the metadata
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Composite setter to update the image's metadata with @width
 * Since: 0.16.0
 */
void gexiv2_metadata_set_metadata_pixel_width (GExiv2Metadata *self, gint width, GError **error);

/**
 * gexiv2_metadata_try_set_metadata_pixel_width:
 * @self: An instance of [class@GExiv2.Metadata]
 * @width: The width of the image as it should be put into the metadata
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Composite setter to update the image's metadata with @width
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.set_metadata_pixel_width] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_try_set_metadata_pixel_width)
void gexiv2_metadata_try_set_metadata_pixel_width(GExiv2Metadata* self, gint width, GError** error);

/**
 * gexiv2_metadata_set_metadata_pixel_height:
 * @self: An instance of [class@GExiv2.Metadata]
 * @height: The width of the image as it should be put into the metadata.
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Update the image's metadata with @height
 * Since: 0.16.0
 */
void gexiv2_metadata_set_metadata_pixel_height (GExiv2Metadata *self, gint height, GError **error);

/**
 * gexiv2_metadata_try_set_metadata_pixel_height:
 * @self: An instance of [class@GExiv2.Metadata]
 * @height: The width of the image as it should be put into the metadata.
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Update the image's metadata with @height
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.set_metadata_pixel_height] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_set_metadata_pixel_height)
void gexiv2_metadata_try_set_metadata_pixel_height(GExiv2Metadata* self, gint height, GError** error);

/**
 * gexiv2_metadata_get_comment:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * A composite accessor that uses the first available metadata field from a list of well-known
 * locations to find the photo's comment (or description).
 *
 * MWG guidelines refer to these as *Description*: a textual description of a resource's content.
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
 * Since: 0.16.0
 */
gchar*			gexiv2_metadata_get_comment			(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_try_get_comment:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * A composite accessor that uses the first available metadata field from a list of well-known
 * locations to find the photo's comment (or description).
 *
 * MWG guidelines refer to these as *Description*: a textual description of a resource's content.
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
 * Deprecated: 0.16.0: Use [method@Metadata.get_comment] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_comment)
gchar* gexiv2_metadata_try_get_comment(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_set_comment:
 * @self: An instance of [class@GExiv2.Metadata]
 * @comment: Comment string to set. Must not be %NULL
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * This is a composite setter that will set a number of fields to the supplied value.  See
 * [method@Metadata.get_comment] for more information.
 *
 * Since: 0.16.0
 */
void			gexiv2_metadata_set_comment			(GExiv2Metadata *self, const gchar* comment, GError **error);

/**
 * gexiv2_metadata_try_set_comment:
 * @self: An instance of [class@GExiv2.Metadata]
 * @comment: Comment string to set. Must not be %NULL
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * This is a composite setter that will set a number of fields to the supplied value.  See
 * [method@Metadata.get_comment] for more information.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.set_comment] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_set_comment)
void gexiv2_metadata_try_set_comment(GExiv2Metadata* self, const gchar* comment, GError** error);

/**
 * gexiv2_metadata_clear_comment:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * This is a composite clear method that will clear a number of fields.  See
 * [method@Metadata.get_comment]  for more information.
 */
void			gexiv2_metadata_clear_comment		(GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_exposure_time:
 * @self: An instance of [class@GExiv2.Metadata]
 * @nom: (out): The numerator
 * @den: (out): The denominator
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Returns the exposure time in seconds (shutter speed, *not* date-time of exposure) as a
 * rational.  See <https://en.wikipedia.org/wiki/Shutter_speed> for more information.
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.16.0
 */
gboolean		gexiv2_metadata_get_exposure_time	(GExiv2Metadata *self, gint *nom, gint *den, GError **error);

/**
 * gexiv2_metadata_try_get_exposure_time:
 * @self: An instance of [class@GExiv2.Metadata]
 * @nom: (out): The numerator
 * @den: (out): The denominator
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Returns the exposure time in seconds (shutter speed, *not* date-time of exposure) as a
 * rational.  See <https://en.wikipedia.org/wiki/Shutter_speed> for more information.
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.get_exposure_time] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_exposure_time)
gboolean gexiv2_metadata_try_get_exposure_time(GExiv2Metadata* self, gint* nom, gint* den, GError** error);

/**
 * gexiv2_metadata_get_fnumber:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * See <https://en.wikipedia.org/wiki/F-number> for more information.
 * If Exif.Photo.FNumber does not exist, it will fall back to calculating the FNumber from
 * Exif.Photo.ApertureValue (if available)
 *
 * Returns: The exposure Fnumber as a gdouble, or -1.0 if tag is not present or invalid.
 *
 * Since: 0.16.0
 */
gdouble			gexiv2_metadata_get_fnumber			(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_try_get_fnumber:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * See <https://en.wikipedia.org/wiki/F-number> for more information.
 * If Exif.Photo.FNumber does not exist, it will fall back to calculating the FNumber from
 * Exif.Photo.ApertureValue (if available);
 *
 * Returns: The exposure Fnumber as a gdouble, or -1.0 if tag is not present or invalid.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.get_fnumber] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_fnumber)
gdouble gexiv2_metadata_try_get_fnumber(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_get_focal_length:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * See <https://en.wikipedia.org/wiki/Flange_focal_distance> for more information.
 *
 * Returns: The focal length as a gdouble, or -1.0 if tag is not present or invalid.
 *
 * Since: 0.16.0
 */
gdouble			gexiv2_metadata_get_focal_length	(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_try_get_focal_length:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * See <https://en.wikipedia.org/wiki/Flange_focal_distance> for more information.
 *
 * Returns: The focal length as a gdouble, or -1.0 if tag is not present or invalid.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.get_focal_length] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_focal_length)
gdouble gexiv2_metadata_try_get_focal_length(GExiv2Metadata* self, GError** error);

/**
 * gexiv2_metadata_get_iso_speed:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * See <https://en.wikipedia.org/wiki/Iso_speed> for more information.
 *
 * Returns: The ISO speed rating as a gint, or 0 if tag is not present or invalid.
 *
 * Since: 0.16.0
 */
gint			gexiv2_metadata_get_iso_speed		(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_try_get_iso_speed:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * See <https://en.wikipedia.org/wiki/Iso_speed> for more information.
 *
 * Returns: The ISO speed rating as a gint, or 0 if tag is not present or invalid.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.get_iso_speed] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_iso_speed)
gint gexiv2_metadata_try_get_iso_speed(GExiv2Metadata* self, GError** error);

/*
 * GPS functions
 */

/**
 * gexiv2_metadata_try_get_gps_longitude:
 * @self: An instance of [class@GExiv2.Metadata]
 * @longitude: (out): Variable to store the longitude value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Query the longitude stored in the GPS tags of @self
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.get_gps_longitude] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_gps_longitude)
gboolean		gexiv2_metadata_try_get_gps_longitude			(GExiv2Metadata *self, gdouble *longitude, GError **error);

/**
 * gexiv2_metadata_try_get_gps_latitude:
 * @self: An instance of [class@GExiv2.Metadata]
 * @latitude: (out): Variable to store the latitude value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Query the latitude stored in the GPS tags of @self
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.get_gps_altitude] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_gps_altitude)
gboolean		gexiv2_metadata_try_get_gps_latitude			(GExiv2Metadata *self, gdouble *latitude, GError **error);

/**
 * gexiv2_metadata_try_get_gps_altitude:
 * @self: An instance of [class@GExiv2.Metadata]
 * @altitude: (out): Variable to store the altitude value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Convenience function to query the altitude stored in the GPS tags of the
 * image
 *
 * Returns: (skip): Boolean success value
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.get_gps_latitude] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_gps_latitude)
gboolean		gexiv2_metadata_try_get_gps_altitude		(GExiv2Metadata *self, gdouble *altitude, GError **error);

/**
 * gexiv2_metadata_get_gps_longitude:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Query the longitude stored in the GPS tags of @self
 *
 * Returns: The longitude value, or -inf on error, or nan if no altitude value.
 * Since: 0.16.0
 */
gdouble		gexiv2_metadata_get_gps_longitude			(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_get_gps_latitude:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Query the latitude stored in the GPS tags of @self
 *
 * Returns: The latitude or -inf on error, nan if no altitude value is found.
 * Since: 0.16.0
 */
gdouble		gexiv2_metadata_get_gps_latitude			(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_get_gps_altitude:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Convenience function to query the altitude stored in the GPS tags of the
 * image
 *
 * Returns: The altitude value, or -inf on error, or nan if no altitude value.
 * Since: 0.16.0
 */
gdouble		gexiv2_metadata_get_gps_altitude			(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_try_get_gps_info:
 * @self: An instance of [class@GExiv2.Metadata]
 * @longitude: (out): Storage for longitude value
 * @latitude: (out): Storage for latitude value
 * @altitude: (out): Storage for altitude value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Convenience function to query all available GPS information at once.
 *
 * Returns: (skip): Boolean success value.
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.get_gps_info] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_gps_info)
gboolean		gexiv2_metadata_try_get_gps_info			(GExiv2Metadata *self, gdouble *longitude, gdouble *latitude, gdouble *altitude, GError **error);

/**
 * gexiv2_metadata_get_gps_info:
 * @self: An instance of [class@GExiv2.Metadata]
 * @longitude: (out): Storage for longitude value
 * @latitude: (out): Storage for latitude value
 * @altitude: (out): Storage for altitude value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Convenience function to query all available GPS information at once. See the
 * [method@GExiv2.Metadata.get_gps_longitude], [method@GExiv2.Metadata.get_gps_latitude]
 * and [method@GExiv2.Metadata.get_gps_altitude] for possible values of the out
 * parameters.
 *
 * Returns: (skip): Boolean success value. Indicates if any of the queries failed.
 * Since: 0.16.0
 */
gboolean		gexiv2_metadata_get_gps_info				(GExiv2Metadata *self, gdouble *longitude, gdouble *latitude, gdouble *altitude, GError **error);

/**
 * gexiv2_metadata_try_set_gps_info:
 * @self: An instance of [class@GExiv2.Metadata]
 * @longitude: Longitude value to set or replace current value.
 * @latitude: Latitude value to set or replace current value
 * @altitude: Altitude value to set or replace current value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Convenience function to create a new set of simple GPS data. Warning: Will remove any other
 * GPS information that is currently set. See [method@Metadata.update_gps_info] for
 * just modifying the GPS data.
 *
 * Returns: (skip): Boolean success value.
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.set_gps_info] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_set_gps_info)
gboolean		gexiv2_metadata_try_set_gps_info			(GExiv2Metadata *self, gdouble longitude, gdouble latitude, gdouble altitude, GError **error);

/**
 * gexiv2_metadata_set_gps_info:
 * @self: An instance of [class@GExiv2.Metadata]
 * @longitude: Longitude value to set or replace current value
 * @latitude: Latitude value to set or replace current value
 * @altitude: Altitude value to set or replace current value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Convenience function to create a new set of simple GPS data. Warning: Will remove any other
 * GPS information that is currently set. See [method@Metadata.update_gps_info] for
 * just modifying the GPS data.
 *
 * Returns: (skip): Boolean success value.
 * Since: 0.16.0
 */
gboolean		gexiv2_metadata_set_gps_info				(GExiv2Metadata *self, gdouble longitude, gdouble latitude, gdouble altitude, GError **error);

/**
 * gexiv2_metadata_try_update_gps_info:
 * @self: An instance of [class@GExiv2.Metadata]
 * @longitude: Longitude value to set or replace current value
 * @latitude: Latitude value to set or replace current value
 * @altitude: Altitude value to set or replace current value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Convenience function to update longitude, latitude and altitude at once.
 *
 * Returns: (skip): Boolean success value.
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.update_gps_info] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_update_gps_info)
gboolean		gexiv2_metadata_try_update_gps_info			(GExiv2Metadata *self, gdouble longitude, gdouble latitude, gdouble altitude, GError **error);

/**
 * gexiv2_metadata_update_gps_info:
 * @self: An instance of [class@GExiv2.Metadata]
 * @longitude: Longitude value to set or replace current value
 * @latitude: Latitude value to set or replace current value
 * @altitude: Altitude value to set or replace current value
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Convenience function to update longitude, latitude and altitude at once.
 *
 * Returns: (skip): Boolean success value.
 *
 * Since: 0.16.0
 */
gboolean		gexiv2_metadata_update_gps_info				(GExiv2Metadata *self, gdouble longitude, gdouble latitude, gdouble altitude, GError **error);

/**
 * gexiv2_metadata_try_delete_gps_info:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Removes all GPS metadata from the loaded image
 *
 * Since: 0.12.2
 * Deprecated: 0.16.0: Use [method@Metadata.delete_gps_info] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_delete_gps_info)
void			gexiv2_metadata_try_delete_gps_info			(GExiv2Metadata *self, GError **error);

/**
 * gexiv2_metadata_delete_gps_info:
 * @self: An instance of [class@GExiv2.Metadata]
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Removes all GPS metadata from the loaded image
 * Since: 0.16.0
 */
void			gexiv2_metadata_delete_gps_info			(GExiv2Metadata *self, GError **error);

/*
 * Preview Manager
 */

/**
 * gexiv2_metadata_get_preview_properties:
 * @self: An instance of [class@GExiv2.Metadata]
 *
 * An image may have stored one or more previews, often of different qualities, sometimes of
 * different image formats than the containing image.  This call returns the properties of all
 * previews Exiv2 finds within the loaded image.  Use [method@Metadata.get_preview_image] to
 * load a particular preview into memory.
 *
 * Returns: (transfer none) (allow-none) (array zero-terminated=1): An array of
 * #GExiv2PreviewProperties instances, one for each preview present in the loaded image.
 */
GExiv2PreviewProperties** gexiv2_metadata_get_preview_properties (GExiv2Metadata *self);

/**
 * gexiv2_metadata_get_preview_image:
 * @self: An instance of [class@GExiv2.Metadata]
 * @props: A #GExiv2PreviewProperties instance
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Get a preview image from the metadata.
 *
 * Returns: (transfer full): A #GExiv2PreviewImage instance for the particular
 * #GExiv2PreviewProperties.
 *
 * Since: 0.16.0
 */
GExiv2PreviewImage* gexiv2_metadata_get_preview_image		(GExiv2Metadata *self, GExiv2PreviewProperties *props, GError **error);

/**
 * gexiv2_metadata_try_get_preview_image:
 * @self: An instance of [class@GExiv2.Metadata]
 * @props: A #GExiv2PreviewProperties instance
 * @error: (allow-none): A return location for a [struct@GLib.Error] or %NULL
 *
 * Get a preview image from the metadata.
 *
 * Returns: (transfer full): A #GExiv2PreviewImage instance for the particular
 * #GExiv2PreviewProperties.
 *
 * Since: 0.14.0
 * Deprecated: 0.16.0: Use [method@Metadata.get_preview_image] instead.
 */
G_DEPRECATED_FOR(gexiv2_metadata_get_preview_image)
GExiv2PreviewImage* gexiv2_metadata_try_get_preview_image(GExiv2Metadata* self,
                                                          GExiv2PreviewProperties* props,
                                                          GError** error);

G_END_DECLS

#endif /* GEXIV2_METADATA_H */

