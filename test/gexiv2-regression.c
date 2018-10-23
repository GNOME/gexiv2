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

    gexiv2_metadata_set_metadata_pixel_height(meta, 123);
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

    gexiv2_metadata_set_tag_long(meta, "Exif.Image.ImageLength", 1234);

    pixel_height = gexiv2_metadata_get_metadata_pixel_height(meta);
    g_assert_cmpint(pixel_height, ==, 1234);

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
    fnumber = gexiv2_metadata_get_fnumber(meta);
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
    g_assert_true(gexiv2_metadata_get_gps_info(meta, &lon, &lat, &alt));

    g_assert_cmpfloat(lon, ==, -1.508425);

    /* Just check if it fits in there; The main issue is that it's not 0 */
    g_assert_cmpfloat(lat, >=, 48.631806);
    g_assert_cmpfloat(lat, <=, 48.631807);
    g_assert_cmpfloat(alt, ==, -0.926000);

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

    raw_tag = gexiv2_metadata_get_tag_raw (meta, "Exif.Image.Artist");
    g_assert_nonnull (raw_tag);
    g_assert_cmpmem (g_bytes_get_data(raw_tag, NULL), g_bytes_get_size(raw_tag),
                     test_bgo_730136_artist_data, sizeof(test_bgo_730136_artist_data));
    g_clear_object (&meta);
    g_clear_pointer (&raw_tag, g_bytes_unref);
}

static void test_bgo_790925(void)
{
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

    image = gexiv2_metadata_get_preview_image (meta, *props);
    g_assert_nonnull(image);
}

int main(int argc, char *argv[static argc + 1])
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/bugs/gnome/775249", test_bgo_775249);
    g_test_add_func("/bugs/gnome/730136", test_bgo_730136);
    g_test_add_func("/bugs/gnome/792239", test_bgo_792239);
    g_test_add_func("/bugs/gnome/790925", test_bgo_790925);
    g_test_add_func("/bugs/gnome/gitlab/31", test_ggo_31);
    g_test_add_func("/bugs/gnome/gitlab/32", test_ggo_32);

    return g_test_run();
}
