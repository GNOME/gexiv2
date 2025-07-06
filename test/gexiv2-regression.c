/*
 * Copyright (C) 2017 Jens Georg <mail@jensge.org>
 *
 * This library is free software. See COPYING for details
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>

#include <gexiv2/gexiv2.h>

#include <string.h>
#include <math.h>

const char LOREM_IPSUM[] =
"Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet." \
"Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat." \
"Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi." \
"Nam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat." \
"Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis." \
"At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, At accusam aliquyam diam diam dolore dolores duo eirmod eos erat, et nonumy sed tempor et et invidunt justo labore Stet clita ea et gubergren, kasd magna no rebum. sanctus sea sed takimata ut vero voluptua. est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat." \
"Consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus." \
"Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet." \
" Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat." \
"Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi." \
"Nam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat." \
"Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis." \
"Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi." \
"===== EOM =====";

/* Regression test for https://gitlab.gnome.org/GNOME/gexiv2/issues/31 */
static void test_ggo_31(void)
{
    GExiv2Metadata *meta = NULL;
    gboolean result = FALSE;
    GError *error = NULL;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);

    result = gexiv2_metadata_open_path (meta, SAMPLE_PATH "/no-metadata.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    gexiv2_metadata_set_metadata_pixel_height(meta, 123, &error);
    g_assert_no_error(error);
    /* Would abort without fix */

    g_clear_object(&meta);
}

/* Regression test for https://gitlab.gnome.org/GNOME/gexiv2/issues/32 */
static void test_ggo_32 (void)
{
    GExiv2Metadata *meta = NULL;
    gboolean result = FALSE;
    GError *error = NULL;
    gint pixel_height = 0;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);

    result = gexiv2_metadata_open_path (meta, SAMPLE_PATH "/no-metadata.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    gexiv2_metadata_set_tag_long(meta, "Exif.Image.ImageLength", 1234, &error);
    g_assert_no_error(error);

    pixel_height = gexiv2_metadata_get_metadata_pixel_height(meta, &error);
    g_assert_no_error(error);
    g_assert_cmpint(pixel_height, ==, 1234);

    g_clear_object(&meta);
}

/* Regression test for https://gitlab.gnome.org/GNOME/gexiv2/issues/33 */
static void test_ggo_33 (void)
{
    GExiv2Metadata *meta = NULL;
    gboolean result = FALSE;
    GError *error = NULL;
    glong pixels = 0;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);

    result = gexiv2_metadata_open_path (meta, SAMPLE_PATH "/no-metadata.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);


    /* Check all the width tags and check that they have the same value */
    gexiv2_metadata_set_metadata_pixel_width(meta, 1234, &error);
    g_assert_no_error(error);
    pixels = gexiv2_metadata_get_tag_long(meta, "Exif.Photo.PixelXDimension", &error);
    g_assert_no_error(error);
    g_assert_cmpint(pixels, ==, 1234);

    pixels = gexiv2_metadata_get_tag_long(meta, "Exif.Image.ImageWidth", &error);
    g_assert_no_error(error);
    g_assert_cmpint(pixels, ==, 1234);

    pixels = gexiv2_metadata_get_tag_long(meta, "Xmp.tiff.ImageWidth", &error);
    g_assert_no_error(error);
    g_assert_cmpint(pixels, ==, 1234);

    pixels = gexiv2_metadata_get_tag_long(meta, "Xmp.exif.PixelXDimension", &error);
    g_assert_no_error(error);
    g_assert_cmpint(pixels, ==, 1234);


    /* Check all the height tags and check that they have the same value */
    gexiv2_metadata_set_metadata_pixel_height(meta, 4321, &error);
    g_assert_no_error(error);
    pixels = gexiv2_metadata_get_tag_long(meta, "Exif.Photo.PixelYDimension", &error);
    g_assert_no_error(error);
    g_assert_cmpint(pixels, ==, 4321);

    pixels = gexiv2_metadata_get_tag_long(meta, "Exif.Image.ImageLength", &error);
    g_assert_no_error(error);
    g_assert_cmpint(pixels, ==, 4321);

    pixels = gexiv2_metadata_get_tag_long(meta, "Xmp.tiff.ImageLength", &error);
    g_assert_no_error(error);
    g_assert_cmpint(pixels, ==, 4321);

    pixels = gexiv2_metadata_get_tag_long(meta, "Xmp.exif.PixelYDimension", &error);
    g_assert_no_error(error);
    g_assert_cmpint(pixels, ==, 4321);

    g_clear_object(&meta);
}


static void test_bgo_792239(void)
{
    GExiv2Metadata *meta = NULL;
    gdouble fnumber = -1.0;
    gboolean result = FALSE;
    GError *error = NULL;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);
    result = gexiv2_metadata_open_path(meta, SAMPLE_PATH "/no-fnumber.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);
    fnumber = gexiv2_metadata_get_fnumber(meta, &error);
    g_assert_no_error(error);
    g_assert_cmpfloat(fnumber, !=, -1.0);

    g_clear_object(&meta);
}

/* Check that gexiv2 correctly handles ratios with 0/0 as used by
 * some cameras
 */
static void test_bgo_775249(void)
{
    GExiv2Metadata *meta = NULL;
    gboolean result = FALSE;
    gdouble lon = 0.0, lat = 0.0, alt = 0.0;
    GError *error = NULL;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);
    result = gexiv2_metadata_open_path(meta, SAMPLE_PATH "/CaorVN.jpeg", &error);
    g_assert_no_error(error);
    g_assert_true(result);
    g_assert_true(gexiv2_metadata_get_gps_info(meta, &lon, &lat, &alt, &error));
    g_assert_no_error(error);

    g_assert_cmpfloat_with_epsilon(lon, -1.508425, 0.0001);

    /* Just check if it fits in there; The main issue is that it's not 0 */
    g_assert_cmpfloat(lat, >=, 48.631806);
    g_assert_cmpfloat(lat, <=, 48.631807);
    g_assert_cmpfloat_with_epsilon(alt, -0.926000, 0.0001);

    g_clear_object(&meta);
}

static const char test_bgo_730136_artist_data[] = {
  0xc0, 0xeb, 0xe5, 0xea, 0xf1, 0xe0, 0xed, 0xe4,
  0xf0,  ' ', 0xca, 0xee, 0xf8, 0xe5, 0xeb, 0xe5,
  0xe2, 0x00
};

static void test_bgo_730136(void)
{
    GExiv2Metadata *meta = NULL;
    gboolean result = FALSE;
    GError *error = NULL;
    GBytes *raw_tag = NULL;

    meta = gexiv2_metadata_new ();
    g_assert_nonnull (meta);
    result = gexiv2_metadata_open_path (meta, SAMPLE_PATH "/sample-author-badencoding.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    raw_tag = gexiv2_metadata_get_tag_raw(meta, "Exif.Image.Artist", &error);
    g_assert_no_error(error);
    g_assert_nonnull (raw_tag);
    g_assert_cmpmem (g_bytes_get_data(raw_tag, NULL), g_bytes_get_size(raw_tag),
                     test_bgo_730136_artist_data, sizeof(test_bgo_730136_artist_data));
    g_clear_object (&meta);
    g_clear_pointer (&raw_tag, g_bytes_unref);
}

static void test_bgo_790925(void)
{
#if defined(__GNUC__) /* This should include clang */
    g_autoptr(GExiv2Metadata) meta = NULL;
    g_autoptr(GExiv2PreviewImage) image = NULL;
    GExiv2PreviewProperties **props = NULL;
    GError *error = NULL;
    gboolean result = FALSE;

    meta = gexiv2_metadata_new ();
    g_assert_nonnull (meta);

    result = gexiv2_metadata_open_path (meta, SAMPLE_PATH "/original.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    props = gexiv2_metadata_get_preview_properties (meta);
    g_assert_nonnull (props);

    image = gexiv2_metadata_get_preview_image(meta, *props, &error);
    g_assert_no_error(error);
    g_assert_nonnull(image);
#endif
}

static void test_ggo_87(void) {
    GExiv2Metadata* meta = NULL;
    gboolean result = FALSE;
    GError* error = NULL;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);
    result = gexiv2_metadata_open_path(meta, SAMPLE_PATH "/description-with-comma.jpg", &error);

    g_assert_no_error(error);
    g_assert_true(result);

    char** tags = gexiv2_metadata_try_get_tag_multiple(meta, "Xmp.dc.description", &error);
    g_assert_no_error(error);
    g_assert_nonnull(tags);

    g_assert_cmpstr(tags[0], ==, "lang=\"x-default\" Elevator, test");

    g_object_unref(meta);
}

static void test_ggo_xx(void)
{
    GExiv2Metadata *meta = NULL;
    gboolean result = FALSE;
    GError *error = NULL;
    char *tmp_file = NULL;
    char *comment = NULL;
    GFile* src = NULL;
    GFile* dest = NULL;

    meta = gexiv2_metadata_new ();
    g_assert_nonnull (meta);
    result = gexiv2_metadata_open_path (meta, SAMPLE_PATH "/no-metadata.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    gexiv2_metadata_set_comment(meta, LOREM_IPSUM, &error);
    g_assert_no_error(error);
    tmp_file = g_strdup ("lorem-ipsum.jpg"); //g_build_filename (g_get_tmp_dir (), "lorem-ipsum.jpg", NULL);

    src = g_file_new_for_path (SAMPLE_PATH "/no-metadata.jpg");
    dest = g_file_new_for_path (tmp_file);

    result = g_file_copy (src, dest,
                          G_FILE_COPY_OVERWRITE,
                          NULL, NULL, NULL,
                          &error);
    g_assert_no_error(error);
    g_assert_true(result);

    result = gexiv2_metadata_save_file (meta, tmp_file, &error);
    g_assert_no_error(error);
    g_assert_true(result);

    result = gexiv2_metadata_open_path (meta, tmp_file, &error);
    g_assert_no_error(error);
    g_assert_true(result);

    comment = gexiv2_metadata_get_comment(meta, &error);
    g_assert_no_error(error);
    g_assert_nonnull (comment);
    g_assert_cmpstr (LOREM_IPSUM, ==, comment);

    g_object_unref (src);
    g_object_unref (dest);
    g_free (tmp_file);
    g_free (comment);
    g_object_unref(meta);
}

static void test_ggo_27(void)
{
    GError* error = NULL;
    gexiv2_metadata_register_xmp_namespace("http://www.gnome.org/xmp", "gnome", &error);
    g_assert_no_error(error);
}

static void test_ggo_45(void)
{
    GExiv2Metadata *meta = NULL;
    gboolean result = FALSE;
    gdouble lon = 0.0, lat = 0.0, alt = 0.0;
    GError *error = NULL;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);
    result = gexiv2_metadata_open_path(meta, SAMPLE_PATH "/no-metadata.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    alt = 2200.0;
    result = gexiv2_metadata_set_gps_info(meta, lon, lat, alt, &error);
    g_assert_no_error(error);
    g_assert_true(result);

    alt = gexiv2_metadata_get_gps_altitude(meta, &error);
    g_assert_no_error(error);
    g_assert_true(result);
    g_assert_cmpfloat(fabs(alt - 2200.0), <= , 1e-5);

    g_clear_object(&meta);

}

static void test_ggo_58(void) {
    GExiv2Metadata* meta = NULL;
    gboolean result = FALSE;
    GError* error = NULL;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);

    result = gexiv2_metadata_open_path(meta, SAMPLE_PATH "/no-metadata.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    for (int i = 0; i < 20; i++) {
        char* key = g_strdup_printf("Xmp.xmpMM.History[%d]", i);
        result = gexiv2_metadata_try_set_tag_string(meta, key, "value", &error);
        g_free(key);
        g_assert(result);
        g_assert_no_error(error);
    }

    char** tags = gexiv2_metadata_get_xmp_tags(meta);
    for (int i = 0; i < 20; i++) {
        char* key = g_strdup_printf("Xmp.xmpMM.History[%d]", i);
        g_assert_cmpstr(tags[i], ==, key);
        g_free(key);
    }
    g_strfreev(tags);

    g_clear_object(&meta);
}

// Regression test for https://gitlab.gnome.org/GNOME/gexiv2/issues/66
static void test_ggo_66(void) {
    GExiv2Metadata* meta = NULL;
    gboolean result = FALSE;
    GError* error = NULL;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);

    result = gexiv2_metadata_open_path(meta, SAMPLE_PATH "/partial_xml.txt", &error);
    g_assert_nonnull(error);
    g_assert_false(result);

    g_clear_error(&error);
    g_clear_object(&meta);
}

/* Regression test for https://gitlab.gnome.org/GNOME/gexiv2/issues/62 */
static void test_ggo_62(void){
    GExiv2Metadata* meta = NULL;
    gboolean result = FALSE;
    gchar* value = NULL;
    gchar** values = NULL;
    GBytes *raw_value = NULL;
    GError* error = NULL;
    const gchar* IPTC_REPEATABLE_TAG = "Iptc.Application2.Keywords";
    const gchar* IPTC_NON_REPEATABLE_TAG = "Iptc.Application2.Category";

    // Output tags
    const gchar* NR_OUTPUT_SINGLE = "NR 2";
    const gchar* NR_OUTPUT_MULTIPLE[] = {
    		"NR 2",
			NULL
    };

	// == "NR 2"
    const gchar NR_OUTPUT_RAW[] = {
        0x4e, 0x52, 0x20, 0x32
    };

    const gchar* R_OUTPUT_SINGLE = "R 1a, R 1b, R 2";
    const gchar* R_OUTPUT_MULTIPLE[] = {
    		"R 1a",
    		"R 1b",
    		"R 2",
			NULL
    };

    // == "R 1a, R 1b, R 2"
    const gchar R_OUTPUT_RAW[] = {
        0x52, 0x20, 0x31, 0x61, 0x1c, 0x1c, 0x1c, 0x1c,
        0x52, 0x20, 0x31, 0x62, 0x1c, 0x1c, 0x1c, 0x1c,
        0x52, 0x20, 0x32
    };


    //// Setup
    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);

    result = gexiv2_metadata_open_path(meta, SAMPLE_PATH "/no-metadata.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);


    //// Test setting/getting Non-Repeatable Iptc tag
    // Setting
    values = g_new(gchar*, 3);
    values[0] = g_strdup("NR 1a");
    values[1] = g_strdup("NR 1b");
    values[2] = NULL;
    result = gexiv2_metadata_try_set_tag_multiple(meta, IPTC_NON_REPEATABLE_TAG, (const gchar**)values, &error);
    g_assert_no_error(error);
    g_assert_true(result);
    g_strfreev (values);

    result = gexiv2_metadata_try_set_tag_string(meta, IPTC_NON_REPEATABLE_TAG, "NR 2", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    // Getting
    value = gexiv2_metadata_try_get_tag_string(meta, IPTC_NON_REPEATABLE_TAG, &error);
    g_assert_no_error(error);
    g_assert_nonnull(value);
    g_assert_cmpstr(value, ==, NR_OUTPUT_SINGLE);
    g_free (value);

    value = gexiv2_metadata_try_get_tag_interpreted_string(meta, IPTC_NON_REPEATABLE_TAG, &error);
    g_assert_no_error(error);
    g_assert_nonnull(value);
    g_assert_cmpstr(value, ==, NR_OUTPUT_SINGLE);
    g_free (value);

    values = gexiv2_metadata_try_get_tag_multiple(meta, IPTC_NON_REPEATABLE_TAG, &error);
    g_assert_no_error(error);
    g_assert_nonnull(values);
    g_assert_true (g_strv_equal ((const gchar**)values, NR_OUTPUT_MULTIPLE));
    g_assert_null(values[1]);
    g_strfreev (values);

    raw_value = gexiv2_metadata_try_get_tag_raw(meta, IPTC_NON_REPEATABLE_TAG, &error);
    g_assert_nonnull (raw_value);
    g_assert_cmpmem (g_bytes_get_data(raw_value, NULL), g_bytes_get_size(raw_value),
                     NR_OUTPUT_RAW, sizeof(NR_OUTPUT_RAW));
    g_clear_pointer (&raw_value, g_bytes_unref);


    //// Test setting/getting Repeatable Iptc tag
    // Setting
    values = g_new(gchar*, 3);
    values[0] = g_strdup("R 1a");
    values[1] = g_strdup("R 1b");
    values[2] = NULL;
    result = gexiv2_metadata_try_set_tag_multiple(meta, IPTC_REPEATABLE_TAG, (const gchar**)values, &error);
    g_assert_no_error(error);
    g_assert_true(result);
    g_strfreev (values);

    result = gexiv2_metadata_try_set_tag_string(meta, IPTC_REPEATABLE_TAG, "R 2", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    // Getting
    value = gexiv2_metadata_try_get_tag_string(meta, IPTC_REPEATABLE_TAG, &error);
    g_assert_no_error(error);
    g_assert_nonnull(value);
    g_assert_cmpstr(value, ==, R_OUTPUT_SINGLE);
    g_free (value);

    value = gexiv2_metadata_try_get_tag_interpreted_string(meta, IPTC_REPEATABLE_TAG, &error);
    g_assert_no_error(error);
    g_assert_nonnull(value);
    g_assert_cmpstr(value, ==, R_OUTPUT_SINGLE);
    g_free (value);

    values = gexiv2_metadata_try_get_tag_multiple(meta, IPTC_REPEATABLE_TAG, &error);
    g_assert_no_error(error);
    g_assert_nonnull(values);
    g_assert_true (g_strv_equal ((const gchar**)values, R_OUTPUT_MULTIPLE));
    g_strfreev (values);

    raw_value = gexiv2_metadata_try_get_tag_raw(meta, IPTC_REPEATABLE_TAG, &error);
    g_assert_nonnull (raw_value);
    g_assert_cmpmem (g_bytes_get_data(raw_value, NULL), g_bytes_get_size(raw_value),
                     R_OUTPUT_RAW, sizeof(R_OUTPUT_RAW));
    g_clear_pointer (&raw_value, g_bytes_unref);


    //// Cleanup
    g_clear_object(&meta);
}

// Regression test for https://gitlab.gnome.org/GNOME/gexiv2/issues/70
static void test_ggo_70(void) {
    GExiv2Metadata *meta = NULL;
    gboolean result = FALSE;
    GError *error = NULL;
    gchar** values = NULL;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);

    result = gexiv2_metadata_open_path (meta, SAMPLE_PATH "/no-metadata.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    values = g_new (gchar*, 1);
    values[0] = NULL;
    result = gexiv2_metadata_try_set_tag_multiple(meta, "Iptc.Application2.Urgency", (const gchar**)values, &error);
    g_assert_no_error(error);
    g_assert_true(result);

    g_free(values);
    g_clear_object(&meta);
}

static void test_ggo_69(void) {
    GExiv2Metadata* meta = NULL;
    gboolean result = FALSE;
    char* packet = NULL;
    GError* error = NULL;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);
    result = gexiv2_metadata_open_path(meta, SAMPLE_PATH "/no-metadata.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    result = gexiv2_metadata_try_set_tag_string(meta, "Xmp.dc.description", "some description", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    packet = gexiv2_metadata_try_generate_xmp_packet(meta, GEXIV2_OMIT_PACKET_WRAPPER, 0, &error);

    g_assert_nonnull(packet);
    g_assert_no_error(error);
    g_assert(!g_str_has_prefix(packet, "<?xpacket"));

    g_free(packet);
    g_object_unref(meta);
}

static void test_nobug_gps(void) {
    GExiv2Metadata* meta = NULL;
    gboolean result = FALSE;
    GError* error = NULL;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);
    result = gexiv2_metadata_open_path(meta, SAMPLE_PATH "/no-metadata.jpg", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    result = gexiv2_metadata_set_gps_info(meta, -1.0, 2.0, 3.0, &error);
    g_assert_no_error(error);
    g_assert_true(result);

    result = gexiv2_metadata_clear_tag(meta, "Exif.GPSInfo.GPSAltitudeRef", &error);
    g_assert_no_error(error);
    g_assert_true(result);

    gdouble value = gexiv2_metadata_get_gps_longitude(meta, &error);
    g_assert_true(!isnan(value) && !isinf(value));
    g_assert_no_error(error);
    g_assert_cmpfloat_with_epsilon(value, -1.0, 0.001);

    value = gexiv2_metadata_get_gps_latitude(meta, &error);
    g_assert_true(!isnan(value) && !isinf(value));
    g_assert_no_error(error);
    g_assert_cmpfloat_with_epsilon(value, 2.0, 0.001);

    value = gexiv2_metadata_get_gps_altitude(meta, &error);
    g_assert_true(isnan(value));

    gexiv2_metadata_try_get_gps_longitude(meta, &value, &error);
    g_assert_true(!isnan(value) && !isinf(value));
    g_assert_no_error(error);
    g_assert_cmpfloat_with_epsilon(value, -1.0, 0.001);

    gexiv2_metadata_try_get_gps_latitude(meta, &value, &error);
    g_assert_true(!isnan(value) && !isinf(value));
    g_assert_no_error(error);
    g_assert_cmpfloat_with_epsilon(value, 2.0, 0.001);

    gexiv2_metadata_try_get_gps_altitude(meta, &value, &error);
    g_assert_cmpfloat(value, ==, 0.0);

    gdouble lon = 0.0, lat = 0.0, alt = 0.0;


    result = gexiv2_metadata_try_get_gps_info(meta, &lon, &lat, &alt, &error);
    g_assert_no_error(error);
    g_assert_cmpint(meta->parent_instance.ref_count, ==, 1);
    g_object_unref(meta);
}

static void test_ggo_80() {
    GExiv2Metadata* meta = NULL;
    gboolean result = FALSE;
    GError* error = NULL;
    char* comment = NULL;

    meta = gexiv2_metadata_new();
    g_assert_nonnull(meta);
    result = gexiv2_metadata_open_path(meta, SAMPLE_PATH "/encoded-comment.jpg", &error);
    g_assert_no_error(error);

    comment = gexiv2_metadata_get_comment(meta, &error);
    g_assert_cmpstr(comment, ==, "This is a comment, äöüßßßßê漢字, some endcoding perhaps?");
    g_assert_no_error(error);
    g_assert_true(result);

    g_free(comment);
    g_object_unref(meta);
}

int main(int argc, char *argv[static argc + 1])
{
    gexiv2_initialize();

    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/bugs/gnome/775249", test_bgo_775249);
    g_test_add_func("/bugs/gnome/730136", test_bgo_730136);
    g_test_add_func("/bugs/gnome/792239", test_bgo_792239);
    g_test_add_func("/bugs/gnome/790925", test_bgo_790925);
    g_test_add_func("/bugs/gnome/gitlab/31", test_ggo_31);
    g_test_add_func("/bugs/gnome/gitlab/32", test_ggo_32);
    g_test_add_func("/bugs/gnome/gitlab/33", test_ggo_33);
    g_test_add_func("/bugs/gnome/gitlab/27", test_ggo_27);
    g_test_add_func("/bugs/gnome/gitlab/xx", test_ggo_xx);
    g_test_add_func("/bugs/gnome/gitlab/45", test_ggo_45);
    g_test_add_func("/bugs/gnome/gitlab/58", test_ggo_58);
    g_test_add_func("/bugs/gnome/gitlab/62", test_ggo_62);
    g_test_add_func("/bugs/gnome/gitlab/60", test_ggo_66);
    g_test_add_func("/bugs/gnome/gitlab/69", test_ggo_69);
    g_test_add_func("/bugs/gnome/gitlab/70", test_ggo_70);
    g_test_add_func("/bugs/gnome/gitlab/86", test_ggo_80);
    g_test_add_func("/bugs/gnome/gitlab/87", test_ggo_87);
    g_test_add_func("/bugs/gnome/nobug/01", test_nobug_gps);

    int result = g_test_run();

    gexiv2_shutdown();

    return result;
}
