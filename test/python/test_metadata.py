# GExiv2 -- GNOME wrapper for libexiv2 library.
# Copyright (C) 2013 Robert Bruce Park <r@robru.ca>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""Test that GExiv2 is functioning correctly."""

__all__ = [
    'TestMetadata',
    ]


import os
import sys
import errno
import shutil
import unittest
import tempfile
import math

import gi
gi.require_version('GExiv2', '0.16')

import gi.overrides
gi.overrides.__path__.insert(0, os.path.join(os.path.dirname(__file__), '..', '..'))
from gi.repository import GExiv2, GLib
from fractions import Fraction


class TestMetadata(unittest.TestCase):
    maxDiff = None

    DATADIR = os.getenv('TEST_DATA_DIR', default = os.path.join(os.path.dirname(__file__), '..', 'data'))

    def setUp(self):
        self.metadata = GExiv2.Metadata()
        self.metadata.open_path(self.get_input_file())
        self.output_dir = tempfile.mkdtemp()
        self.output_file = 'withgps.jpg'

    def tearDown(self):
        try:
            shutil.rmtree(self.output_dir)
        except Exception as e:
            if e.errno != errno.ENOENT:
                raise

    def get_input_file(self):
        return os.path.join(self.DATADIR, 'original.jpg')

    def get_output_file(self):
        return os.path.join(self.output_dir, self.output_file)

    def test_presence_of_expected_methods(self):
        metadata = GExiv2.Metadata()
        methods = [m for m in dir(metadata) if not m.startswith('_')]
        for method in ['bind_property',
                       'bind_property_full',
                       'chain',
                       'clear',
                       'clear_comment',
                       'clear_exif',
                       'clear_iptc',
                       'clear_tag',
                       'clear_xmp',
                       'compat_control',
                       'connect',
                       'connect_after',
                       'connect_object',
                       'connect_object_after',
                       'delete_gps_info',
                       'disconnect',
                       'disconnect_by_func',
                       'emit',
                       'emit_stop_by_name',
                       'erase_exif_thumbnail',
                       'force_floating',
                       'freeze_notify',
                       'from_app1_segment',
                       'from_stream',
                       'g_type_instance',
                       'generate_xmp_packet',
                       'get_comment',
                       'get_data',
                       'get_exif_data',
                       'get_exif_tag_rational',
                       'get_exif_tags',
                       'get_exif_thumbnail',
                       'get_exposure_time',
                       'get_fnumber',
                       'get_focal_length',
                       'get_gps_altitude',
                       'get_gps_info',
                       'get_gps_latitude',
                       'get_gps_longitude',
                       'get_iptc_tags',
                       'get_iso_speed',
                       'get_metadata_pixel_height',
                       'get_metadata_pixel_width',
                       'get_mime_type',
                       'get_orientation',
                       'get_pixel_height',
                       'get_pixel_width',
                       'get_preview_image',
                       'get_preview_properties',
                       'get_properties',
                       'get_property',
                       'get_qdata',
                       'get_supports_exif',
                       'get_supports_iptc',
                       'get_supports_xmp',
                       'get_tag_description',
                       'get_tag_interpreted_string',
                       'get_tag_label',
                       'get_tag_long',
                       'get_tag_multiple',
                       'get_tag_raw',
                       'get_tag_string',
                       'get_tag_type',
                       'get_xmp_namespace_for_tag',
                       'get_xmp_packet',
                       'get_xmp_tags',
                       'handler_block',
                       'handler_block_by_func',
                       'handler_disconnect',
                       'handler_is_connected',
                       'handler_unblock',
                       'handler_unblock_by_func',
                       'has_exif',
                       'has_iptc',
                       'has_tag',
                       'has_xmp',
                       'interface_find_property',
                       'interface_install_property',
                       'interface_list_properties',
                       'is_exif_tag',
                       'is_floating',
                       'is_iptc_tag',
                       'is_xmp_tag',
                       'new',
                       'notify',
                       'notify_by_pspec',
                       'open_buf',
                       'open_path',
                       'parent_instance',
                       'props',
                       'qdata',
                       'ref',
                       'ref_count',
                       'ref_sink',
                       'register_xmp_namespace',
                       'replace_data',
                       'replace_qdata',
                       'run_dispose',
                       'save_external',
                       'save_file',
                       'set_comment',
                       'set_data',
                       'set_exif_tag_rational',
                       'set_exif_thumbnail_from_buffer',
                       'set_exif_thumbnail_from_file',
                       'set_gps_info',
                       'set_metadata_pixel_height',
                       'set_metadata_pixel_width',
                       'set_orientation',
                       'set_properties',
                       'set_property',
                       'set_tag_long',
                       'set_tag_multiple',
                       'set_tag_string',
                       'set_xmp_tag_struct',
                       'steal_data',
                       'steal_qdata',
                       'stop_emission',
                       'stop_emission_by_name',
                       'thaw_notify',
                       'try_clear_tag',
                       'try_tag_supports_multiple_values',
                       'try_delete_gps_info',
                       'try_erase_exif_thumbnail',
                       'try_generate_xmp_packet',
                       'try_get_comment',
                       'try_get_exif_tag_rational',
                       'try_get_exposure_time',
                       'try_get_fnumber',
                       'try_get_focal_length',
                       'try_get_gps_altitude',
                       'try_get_gps_info',
                       'try_get_gps_latitude',
                       'try_get_gps_longitude',
                       'try_get_iso_speed',
                       'try_get_metadata_pixel_height',
                       'try_get_metadata_pixel_width',
                       'try_get_orientation',
                       'try_get_preview_image',
                       'try_get_tag_description',
                       'try_get_tag_interpreted_string',
                       'try_get_tag_label',
                       'try_get_tag_long',
                       'try_get_tag_multiple',
                       'try_get_tag_raw',
                       'try_get_tag_string',
                       'try_get_tag_type',
                       'try_get_xmp_namespace_for_tag',
                       'try_get_xmp_packet',
                       'try_has_tag',
                       'try_register_xmp_namespace',
                       'try_set_comment',
                       'try_set_exif_tag_rational',
                       'try_set_exif_thumbnail_from_buffer',
                       'try_set_gps_info',
                       'try_set_metadata_pixel_height',
                       'try_set_metadata_pixel_width',
                       'try_set_orientation',
                       'try_set_tag_long',
                       'try_set_tag_multiple',
                       'try_set_tag_string',
                       'try_set_xmp_tag_struct',
                       'try_unregister_all_xmp_namespaces',
                       'try_unregister_xmp_namespace',
                       'try_update_gps_info',
                       'unref',
                       'unregister_all_xmp_namespaces',
                       'unregister_xmp_namespace',
                       'update_gps_info',
                       'watch_closure',
                       'weak_ref']:
            self.assertIn(method, methods)

        preview_properties = GExiv2.PreviewProperties()
        methods = [m for m in dir(preview_properties) if not m.startswith('_')]
        for method in ['bind_property',
                       'bind_property_full',
                       'chain',
                       'connect',
                       'connect_after',
                       'connect_object',
                       'connect_object_after',
                       'disconnect',
                       'disconnect_by_func',
                       'emit',
                       'emit_stop_by_name',
                       'force_floating',
                       'freeze_notify',
                       'g_type_instance',
                       'get_extension',
                       'get_height',
                       'get_mime_type',
                       'get_qdata',
                       'get_size',
                       'get_width',
                       'interface_find_property',
                       'interface_install_property',
                       'interface_list_properties',
                       'notify',
                       'ref_count',
                       'ref_sink',
                       'replace_data',
                       'replace_qdata',
                       'set_property',
                       'steal_data',
                       'steal_qdata',
                       'thaw_notify',
                       'unref',
                       'watch_closure',
                       'weak_ref'
                       ]:
            self.assertIn(method, methods)

        preview_image = GExiv2.PreviewImage()
        methods = [m for m in dir(preview_image) if not m.startswith('_')]
        for method in ['bind_property',
                       'bind_property_full',
                       'chain',
                       'connect',
                       'connect_after',
                       'connect_object',
                       'connect_object_after',
                       'disconnect',
                       'disconnect_by_func',
                       'emit',
                       'emit_stop_by_name',
                       'force_floating',
                       'freeze_notify',
                       'g_type_instance',
                       'get_data',
                       'get_extension',
                       'get_mime_type',
                       'get_height',
                       'get_width',
                       'get_qdata',
                       'interface_find_property',
                       'interface_install_property',
                       'interface_list_properties',
                       'notify',
                       'ref_count',
                       'ref_sink',
                       'replace_data',
                       'replace_qdata',
                       'set_property',
                       'steal_data',
                       'steal_qdata',
                       'thaw_notify',
                       'try_write_file',
                       'unref',
                       'watch_closure',
                       'write_file',
                       'weak_ref'
                       ]:
            self.assertIn(method, methods)

    def test_open_buf(self):
        with open(self.get_input_file(), 'rb') as fd:
            buf = fd.read()
        metadata = GExiv2.Metadata()
        metadata.open_buf(buf)
        self.assertGreaterEqual(len(metadata.get_exif_tags()), 111)

    def test_open_path(self):
        metadata = GExiv2.Metadata()
        metadata.open_path(self.get_input_file())
        self.assertGreaterEqual(len(metadata.get_exif_tags()), 111)

    def test_get_tag_string(self):
        reference_data = dict(
            [('Exif.Image.DateTime', '2012:11:02 09:04:27'),
             ('Exif.Image.ExifTag', '234'),
             ('Exif.Image.ImageDescription', '          '),
             ('Exif.Image.Make', 'NIKON'),
             ('Exif.Image.Model', 'COOLPIX P310'),
             ('Exif.Image.Orientation', '1'),
             ('Exif.Image.ResolutionUnit', '2'),
             ('Exif.Image.Software', 'COOLPIX P310   V1.0'),
             ('Exif.Image.XResolution', '300/1'),
             ('Exif.Image.YCbCrPositioning', '2'),
             ('Exif.Image.YResolution', '300/1'),
             ('Exif.Iop.InteroperabilityIndex', 'R98'),
             ('Exif.Iop.InteroperabilityVersion', '48 49 48 48'),
             ('Exif.MakerNote.ByteOrder', 'II'),
             ('Exif.MakerNote.Offset', '748'),
             ('Exif.Nikon3.0x000a', '7810/1000'),
             ('Exif.Nikon3.0x0026', '257 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0'),
             ('Exif.Nikon3.0x0027', '0 0 0 0 0 0 0 0 0 0 0 0 0 0'),
             ('Exif.Nikon3.0x002d', '512 0 0'),
             ('Exif.Nikon3.0x002e', '1'),
             ('Exif.Nikon3.0x0030', '0'),
             ('Exif.Nikon3.0x0035', '48 49 48 48 0 0 0 0'),
             ('Exif.Nikon3.0x0036', '48 49 48 48 0 0'),
             ('Exif.Nikon3.0x0038', '48 49 48 48 0 0 0 0 0 0'),
             ('Exif.Nikon3.0x003a', '48 49 48 48 0 0 255'),
             ('Exif.Nikon3.0x009b', '0 0'),
             ('Exif.Nikon3.0x009d', '0'),
             ('Exif.Nikon3.0x009f', '56'),
             ('Exif.Nikon3.0x00b2', '        '),
             ('Exif.Nikon3.ActiveDLighting', '0'),
             ('Exif.Nikon3.AuxiliaryLens', 'OFF         '),
             ('Exif.Nikon3.ColorMode', 'COLOR '),
             ('Exif.Nikon3.DigitalZoom', '100/100'),
             ('Exif.Nikon3.FlashSetting', '       '),
             ('Exif.Nikon3.Focus', 'AF-S  '),
             ('Exif.Nikon3.FocusDistance', '0/0'),
             ('Exif.Nikon3.ISOSelection', 'AUTO  '),
             ('Exif.Nikon3.ISOSpeed', '0 0'),
             ('Exif.Nikon3.ImageAdjustment', 'NORMAL       '),
             ('Exif.Nikon3.ImageStabilization', 'VR-ON     '),
             ('Exif.Nikon3.NoiseReduction', 'OFF '),
             ('Exif.Nikon3.Preview', '2480'),
             ('Exif.Nikon3.Quality', 'FINE  '),
             ('Exif.Nikon3.RetouchHistory', '0 0 0 0 0 0 0 0 0 0'),
             ('Exif.Nikon3.Saturation', '0'),
             ('Exif.Nikon3.Saturation', '0'),
             ('Exif.Nikon3.SceneAssist', '                   '),
             ('Exif.Nikon3.SceneMode', '               '),
             ('Exif.Nikon3.Sharpening', 'NORMAL'),
             ('Exif.Nikon3.ToneComp', 'NORMAL  '),
             ('Exif.Nikon3.ToningEffect', '       '),
             ('Exif.Nikon3.Version', '0 2 0 0'),
             ('Exif.Nikon3.WhiteBalance', 'AUTO1       '),
             ('Exif.Nikon3.WhiteBalanceBias', '0'),
             ('Exif.NikonAf.AFAreaMode', '1'),
             ('Exif.NikonAf.AFPoint', '0'),
             ('Exif.NikonAf.AFPointsInFocus', '19'),
             ('Exif.NikonPreview.Compression', '6'),
             ('Exif.NikonPreview.JPEGInterchangeFormat', '2586'),
             ('Exif.NikonPreview.JPEGInterchangeFormatLength', '27981'),
             ('Exif.NikonPreview.ResolutionUnit', '2'),
             ('Exif.NikonPreview.XResolution', '300/1'),
             ('Exif.NikonPreview.YCbCrPositioning', '2'),
             ('Exif.NikonPreview.YResolution', '300/1'),
             ('Exif.Photo.ColorSpace', '1'),
             ('Exif.Photo.ComponentsConfiguration', '1 2 3 0'),
             ('Exif.Photo.CompressedBitsPerPixel', '4/1'),
             ('Exif.Photo.Contrast', '0'),
             ('Exif.Photo.CustomRendered', '0'),
             ('Exif.Photo.DateTimeDigitized', '2012:11:02 09:04:27'),
             ('Exif.Photo.DateTimeOriginal', '2012:11:02 09:04:27'),
             ('Exif.Photo.DigitalZoomRatio', '0/100'),
             ('Exif.Photo.ExifVersion', '48 50 51 48'),
             ('Exif.Photo.ExposureBiasValue', '0/10'),
             ('Exif.Photo.ExposureMode', '0'),
             ('Exif.Photo.ExposureProgram', '2'),
             ('Exif.Photo.ExposureTime', '10/150'),
             ('Exif.Photo.FNumber', '47/10'),
             ('Exif.Photo.FileSource', '3'),
             ('Exif.Photo.Flash', '16'),
             ('Exif.Photo.FlashpixVersion', '48 49 48 48'),
             ('Exif.Photo.FocalLength', '138/10'),
             ('Exif.Photo.FocalLengthIn35mmFilm', '77'),
             ('Exif.Photo.GainControl', '2'),
             ('Exif.Photo.ISOSpeedRatings', '400'),
             ('Exif.Photo.InteroperabilityTag', '31334'),
             ('Exif.Photo.LightSource', '0'),
             ('Exif.Photo.MaxApertureValue', '17/10'),
             ('Exif.Photo.MeteringMode', '5'),
             ('Exif.Photo.PixelXDimension', '4608'),
             ('Exif.Photo.PixelYDimension', '3456'),
             ('Exif.Photo.Saturation', '0'),
             ('Exif.Photo.SceneCaptureType', '0'),
             ('Exif.Photo.SceneType', '1'),
             ('Exif.Photo.SensitivityType', '1'),
             ('Exif.Photo.Sharpness', '0'),
             ('Exif.Photo.SubjectDistanceRange', '0'),
             ('Exif.Photo.WhiteBalance', '0'),
             ('Exif.Thumbnail.Compression', '6'),
             ('Exif.Thumbnail.JPEGInterchangeFormat', '31458'),
             ('Exif.Thumbnail.JPEGInterchangeFormatLength', '4534'),
             ('Exif.Thumbnail.ResolutionUnit', '2'),
             ('Exif.Thumbnail.XResolution', '300/1'),
             ('Exif.Thumbnail.YResolution', '300/1'),
             ])
        
        data = dict([(tag, self.metadata.get_tag_string(tag))
             for tag in self.metadata.get_exif_tags()
             if len(self.metadata.get_tag_string(tag)) < 100])
        
        self.assertEqual(data, data | reference_data)


    def test_get_tag_interpreted_string(self):
        data = dict(
            [(tag, self.metadata.get_tag_interpreted_string(tag))
             for tag in self.metadata.get_exif_tags()
             if len(self.metadata.get_tag_interpreted_string(tag)) < 100]);
        reference_data = dict(
            [('Exif.Image.DateTime', '2012:11:02 09:04:27'),
             ('Exif.Image.ExifTag', '234'),
             ('Exif.Image.ImageDescription', '          '),
             ('Exif.Image.Make', 'NIKON'),
             ('Exif.Image.Model', 'COOLPIX P310'),
             ('Exif.Image.Orientation', 'top, left'),
             ('Exif.Image.ResolutionUnit', 'inch'),
             ('Exif.Image.Software', 'COOLPIX P310   V1.0'),
             ('Exif.Image.XResolution', '300'),
             ('Exif.Image.YCbCrPositioning', 'Co-sited'),
             ('Exif.Image.YResolution', '300'),
             ('Exif.Iop.InteroperabilityIndex', 'R98'),
             ('Exif.Iop.InteroperabilityVersion', '1.00'),
             ('Exif.MakerNote.ByteOrder', 'II'),
             ('Exif.MakerNote.Offset', '748'),
             ('Exif.Nikon3.0x000a', '7810/1000'),
             ('Exif.Nikon3.0x0026', '257 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0'),
             ('Exif.Nikon3.0x0027', '0 0 0 0 0 0 0 0 0 0 0 0 0 0'),
             ('Exif.Nikon3.0x002d', '512 0 0'),
             ('Exif.Nikon3.0x002e', '1'),
             ('Exif.Nikon3.0x0030', '0'),
             ('Exif.Nikon3.0x0035', '48 49 48 48 0 0 0 0'),
             ('Exif.Nikon3.0x0036', '48 49 48 48 0 0'),
             ('Exif.Nikon3.0x0038', '48 49 48 48 0 0 0 0 0 0'),
             ('Exif.Nikon3.0x003a', '48 49 48 48 0 0 255'),
             ('Exif.Nikon3.0x009b', '0 0'),
             ('Exif.Nikon3.0x009d', '0'),
             ('Exif.Nikon3.0x009f', '56'),
             ('Exif.Nikon3.0x00b2', '        '),
             ('Exif.Nikon3.ActiveDLighting', 'Off'),
             ('Exif.Nikon3.AuxiliaryLens', 'OFF         '),
             ('Exif.Nikon3.ColorMode', 'COLOR '),
             ('Exif.Nikon3.DigitalZoom', '1.0x'),
             ('Exif.Nikon3.FlashSetting', '       '),
             ('Exif.Nikon3.Focus', 'Single autofocus'),
             ('Exif.Nikon3.FocusDistance', 'Unknown'),
             ('Exif.Nikon3.ISOSelection', 'AUTO  '),
             ('Exif.Nikon3.ISOSpeed', '0'),
             ('Exif.Nikon3.ImageAdjustment', 'NORMAL       '),
             ('Exif.Nikon3.ImageStabilization', 'VR-ON     '),
             ('Exif.Nikon3.NoiseReduction', 'OFF '),
             ('Exif.Nikon3.Preview', '2480'),
             ('Exif.Nikon3.Quality', 'FINE  '),
             ('Exif.Nikon3.RetouchHistory', 'None'),
             ('Exif.Nikon3.Saturation', '0'),
             ('Exif.Nikon3.Saturation', '0'),
             ('Exif.Nikon3.SceneAssist', '                   '),
             ('Exif.Nikon3.SceneMode', '               '),
             ('Exif.Nikon3.Sharpening', 'NORMAL'),
             ('Exif.Nikon3.ToneComp', 'NORMAL  '),
             ('Exif.Nikon3.ToningEffect', '       '),
             ('Exif.Nikon3.Version', '()'),
             ('Exif.Nikon3.WhiteBalance', 'AUTO1       '),
             ('Exif.Nikon3.WhiteBalanceBias', '0'),
             ('Exif.NikonAf.AFAreaMode', 'Dynamic Area'),
             ('Exif.NikonAf.AFPoint', 'Center'),
             ('Exif.NikonAf.AFPointsInFocus', 'Center, Top, Mid-right'),
             ('Exif.NikonPreview.Compression', 'JPEG (old-style)'),
             ('Exif.NikonPreview.JPEGInterchangeFormat', '2586'),
             ('Exif.NikonPreview.JPEGInterchangeFormatLength', '27981'),
             ('Exif.NikonPreview.ResolutionUnit', 'inch'),
             ('Exif.NikonPreview.XResolution', '300'),
             ('Exif.NikonPreview.YCbCrPositioning', 'Co-sited'),
             ('Exif.NikonPreview.YResolution', '300'),
             ('Exif.Photo.ColorSpace', 'sRGB'),
             ('Exif.Photo.ComponentsConfiguration', 'YCbCr'),
             ('Exif.Photo.CompressedBitsPerPixel', '4'),
             ('Exif.Photo.Contrast', 'Normal'),
             ('Exif.Photo.CustomRendered', 'Normal process'),
             ('Exif.Photo.DateTimeDigitized', '2012:11:02 09:04:27'),
             ('Exif.Photo.DateTimeOriginal', '2012:11:02 09:04:27'),
             ('Exif.Photo.DigitalZoomRatio', '0.0'),
             ('Exif.Photo.ExifVersion', '2.30'),
             ('Exif.Photo.ExposureBiasValue', '0 EV'),
             ('Exif.Photo.ExposureMode', 'Auto'),
             ('Exif.Photo.ExposureProgram', 'Auto'),
             ('Exif.Photo.ExposureTime', '1/15 s'),
             ('Exif.Photo.FNumber', 'F4.7'),
             ('Exif.Photo.FileSource', 'Digital still camera'),
             ('Exif.Photo.Flash', 'No, compulsory'),
             ('Exif.Photo.FlashpixVersion', '1.00'),
             ('Exif.Photo.FocalLength', '13.8 mm'),
             ('Exif.Photo.FocalLengthIn35mmFilm', '77.0 mm'),
             ('Exif.Photo.GainControl', 'High gain up'),
             ('Exif.Photo.ISOSpeedRatings', '400'),
             ('Exif.Photo.InteroperabilityTag', '31334'),
             ('Exif.Photo.LightSource', 'Unknown'),
             ('Exif.Photo.MaxApertureValue', 'F1.8'),
             ('Exif.Photo.MeteringMode', 'Multi-segment'),
             ('Exif.Photo.PixelXDimension', '4608'),
             ('Exif.Photo.PixelYDimension', '3456'),
             ('Exif.Photo.Saturation', 'Normal'),
             ('Exif.Photo.SceneCaptureType', 'Standard'),
             ('Exif.Photo.SceneType', 'Directly photographed'),
             ('Exif.Photo.SensitivityType', '1'),
             ('Exif.Photo.Sharpness', 'Normal'),
             ('Exif.Photo.SubjectDistanceRange', 'Unknown'),
             ('Exif.Photo.WhiteBalance', 'Auto'),
             ('Exif.Thumbnail.Compression', 'JPEG (old-style)'),
             ('Exif.Thumbnail.JPEGInterchangeFormat', '31458'),
             ('Exif.Thumbnail.JPEGInterchangeFormatLength', '4534'),
             ('Exif.Thumbnail.ResolutionUnit', 'inch'),
             ('Exif.Thumbnail.XResolution', '300'),
             ('Exif.Thumbnail.YResolution', '300'),
             ])
        self.assertEqual(data, data | reference_data)

    def test_has_tag(self):
        self.assertTrue(self.metadata.has_tag('Exif.Image.DateTime'))
        self.assertTrue(self.metadata.has_tag('Exif.Thumbnail.XResolution'))
        self.assertFalse(self.metadata.has_tag('Iptc.Application2.City'))
        with self.assertRaises(gi.repository.GLib.GError):
            self.metadata.has_tag('Some.Invalid.Tag')

    def test_clear_tag(self):
        self.assertTrue(self.metadata.has_tag('Exif.Image.DateTime'))
        self.assertTrue(self.metadata.clear_tag('Exif.Image.DateTime'))
        self.assertFalse(self.metadata.has_tag('Exif.Image.DateTime'))

    def test_clear(self):
        self.assertGreaterEqual(len(self.metadata.get_exif_tags()), 111)
        self.assertTrue(self.metadata.has_tag('Exif.Image.DateTime'))
        self.assertIsNone(self.metadata.clear())
        self.assertFalse(self.metadata.has_tag('Exif.Image.DateTime'))
        self.assertEqual(len(self.metadata.get_exif_tags()), 0)

    def test_get_mime_type(self):
        self.assertEqual(self.metadata.get_mime_type(), 'image/jpeg')

    def test_supports(self):
        self.assertTrue(self.metadata.get_supports_exif())
        self.assertTrue(self.metadata.get_supports_iptc())
        self.assertTrue(self.metadata.get_supports_xmp())

    def test_get_orientation(self):
        results = []
        for i in range(9):
            self.metadata.set_tag_string('Exif.Image.Orientation', str(i))
            orient = self.metadata.get_orientation()
            results.append((int(orient), orient))

        self.assertEqual(
            results, [
                (0, GExiv2.Orientation.UNSPECIFIED),
                (1, GExiv2.Orientation.NORMAL),
                (2, GExiv2.Orientation.HFLIP),
                (3, GExiv2.Orientation.ROT_180),
                (4, GExiv2.Orientation.VFLIP),
                (5, GExiv2.Orientation.ROT_90_HFLIP),
                (6, GExiv2.Orientation.ROT_90),
                (7, GExiv2.Orientation.ROT_90_VFLIP),
                (8, GExiv2.Orientation.ROT_270),
                ])

    def test_set_orientation(self):
        rotations = [
            GExiv2.Orientation.UNSPECIFIED,
            GExiv2.Orientation.NORMAL,
            GExiv2.Orientation.HFLIP,
            GExiv2.Orientation.ROT_180,
            GExiv2.Orientation.VFLIP,
            GExiv2.Orientation.ROT_90_HFLIP,
            GExiv2.Orientation.ROT_90,
            GExiv2.Orientation.ROT_90_VFLIP,
            GExiv2.Orientation.ROT_270,
            ]

        results = []
        for i, o in enumerate(rotations):
            self.metadata.set_orientation(o)
            results.append((i, self.metadata.get_tag_string(
                'Exif.Image.Orientation')))

        self.assertEqual(
            results, [
                (0, '0'),
                (1, '1'),
                (2, '2'),
                (3, '3'),
                (4, '4'),
                (5, '5'),
                (6, '6'),
                (7, '7'),
                (8, '8'),
                ])

    def test_get_pixel_width_and_height(self):
        self.assertEqual(self.metadata.get_pixel_width(), 16)
        self.assertEqual(self.metadata.get_pixel_height(), 12)

    def test_get_comment(self):
        self.assertEqual(self.metadata.get_comment().strip(), '')
        self.metadata.set_tag_string('Exif.Image.ImageDescription',
                                     'Lego Dragon, Copenhagen.')
        self.assertEqual(self.metadata.get_comment(),
                         'Lego Dragon, Copenhagen.')

    def test_set_tag_string(self):
        self.assertTrue(
            self.metadata.set_tag_string('Exif.Image.Model', 'FooCam'))
        self.assertEqual(
            self.metadata.get_tag_string('Exif.Image.Model'), 'FooCam')

    def test_get_tag_long(self):
        self.assertEqual(
            self.metadata.get_tag_long('Exif.Image.XResolution'), 300)
        self.assertEqual(
            self.metadata.get_tag_long('Exif.Image.YResolution'), 300)
        self.assertEqual(
            self.metadata.get_tag_long('Exif.Photo.FocalLength'), 13)
        self.assertEqual(
            self.metadata.get_tag_long('Exif.Photo.ISOSpeedRatings'), 400)

    def test_set_tag_long(self):
        self.metadata.set_tag_long('Exif.Photo.FocalLength', 500)
        self.assertEqual(
            self.metadata.get_tag_long('Exif.Photo.FocalLength'), 500)
        self.assertEqual(
            self.metadata.get_tag_string('Exif.Photo.FocalLength'), '500')

    def test_get_tag_multiple(self):
        self.metadata.set_tag_multiple(
            'Xmp.dc.subject', 'some darn picture'.split())
        self.assertEqual(
            self.metadata.get_tag_multiple('Xmp.dc.subject'),
            ['some', 'darn', 'picture'])

    def test_get_exposure_time(self):
        exposure = self.metadata.get_exposure_time()
        if isinstance(exposure, Fraction):
            self.assertEqual(exposure, Fraction(1, 15))
        else:
            self.assertEqual(exposure, (10, 150))

    def test_get_fnumber(self):
        self.assertEqual(self.metadata.get_fnumber(), 4.7)

    def test_get_focal_length(self):
        self.assertEqual(self.metadata.get_focal_length(), 13.8)

    def test_get_iso_speed(self):
        self.assertEqual(self.metadata.get_iso_speed(), 400)

    def test_get_tag_label(self):
        self.assertEqual(
            self.metadata.get_tag_label('Exif.Image.DateTime'),
            'Date and Time')
        self.assertEqual(
            self.metadata.get_tag_label('Exif.Image.Make'),
            'Manufacturer')
        self.assertEqual(
            self.metadata.get_tag_label('Exif.Nikon3.Preview'),
            'Pointer to a preview image')

    def test_get_tag_description(self):
        self.assertEqual(
            self.metadata.get_tag_description('Exif.Image.DateTime'),
            """The date and time of image creation. In Exif standard, it is the \
date and time the file was changed.""")
        self.assertEqual(
            self.metadata.get_tag_description('Exif.Image.Make'),
            """The manufacturer of the recording equipment. This is the \
manufacturer of the DSC, scanner, video digitizer or other equipment that \
generated the image. When the field is left blank, it is treated as unknown.""")

    def test_has_exif(self):
        self.assertTrue(self.metadata.has_exif())
        self.metadata.clear()
        self.assertFalse(self.metadata.has_exif())

    def test_clear_exif(self):
        self.assertTrue('Exif.Image.Make' in self.metadata.get_exif_tags())
        self.metadata.set_tag_string('Xmp.dc.subject', 'Dragons!')
        self.metadata.clear_exif()
        self.assertEqual(
            {tag: self.metadata.get_tag_string(tag)
             for tag in self.metadata.get_exif_tags()
                      + self.metadata.get_xmp_tags()},
            {'Xmp.dc.subject': 'Dragons!'})

    def test_get_exif_tag_rational(self):
        fnumber = self.metadata.get_exif_tag_rational('Exif.Photo.FNumber')
        focal = self.metadata.get_exif_tag_rational('Exif.Photo.FocalLength')
        aperture = self.metadata.get_exif_tag_rational('Exif.Photo.MaxApertureValue')

        if isinstance(fnumber, Fraction):
            self.assertEqual(fnumber, Fraction(47, 10))
            self.assertEqual(focal, Fraction(138, 10))
            self.assertEqual(aperture, Fraction(17, 10))
        else:
            self.assertEqual(fnumber, (47, 10))
            self.assertEqual(focal, (138, 10))
            self.assertEqual(aperture, (17, 10))

    def test_set_exif_tag_rational(self):
        try:
            self.metadata.set_exif_tag_rational(
                'Exif.Photo.FocalLength', 355, 113)
        except TypeError:
            self.metadata.set_exif_tag_rational(
                'Exif.Photo.FocalLength', Fraction(355, 113))
        focal = self.metadata.get_exif_tag_rational('Exif.Photo.FocalLength')
        if isinstance(focal, Fraction):
            self.assertEqual(focal, Fraction(355, 113))
        else:
            self.assertEqual(focal, (355, 113))
        self.assertEqual(self.metadata.get_tag_string('Exif.Photo.FocalLength'), '355/113')

    def test_get_exif_thumbnail(self):
        thumb = self.metadata.get_exif_thumbnail()
        self.assertEqual(len(thumb), 4534)

    def test_set_exif_thumbnail_from_file(self):
        self.metadata.set_exif_thumbnail_from_file(self.get_input_file())
        thumb = self.metadata.get_exif_thumbnail()
        self.assertEqual(len(thumb), 36660)

    def test_set_exif_thumbnail_from_buffer(self):
        with open(self.get_input_file(), 'rb') as fd:
            buf = fd.read()
        self.metadata.set_exif_thumbnail_from_buffer(buf)
        thumb = self.metadata.get_exif_thumbnail()
        self.assertEqual(len(thumb), 36660)

    def test_erase_exif_thumbnail(self):
        self.metadata.erase_exif_thumbnail()
        thumb = self.metadata.get_exif_thumbnail()
        self.assertEqual(thumb, b'')

    def test_has_xmp(self):
        self.assertFalse(self.metadata.has_xmp())
        self.metadata.set_tag_string('Xmp.dc.subject', 'Dragons!')
        self.assertTrue(self.metadata.has_xmp())

    def test_clear_xmp(self):
        self.metadata.set_tag_string('Xmp.dc.subject', 'Dragons!')
        self.metadata.clear_xmp()
        self.assertFalse(self.metadata.has_xmp())
        self.assertTrue(self.metadata.has_exif())

    def test_get_xmp_packet(self):
        self.assertEqual(self.metadata.get_xmp_packet(), '')

    def test_get_xmp_tags(self):
        self.assertEqual(self.metadata.get_xmp_tags(), [])
        self.metadata.set_tag_string('Xmp.dc.subject', 'Dragons!')
        self.assertEqual(self.metadata.get_xmp_tags(), ['Xmp.dc.subject'])

    def test_is_xmp_tag(self):
        self.assertTrue(self.metadata.is_xmp_tag('Xmp.dc.subject'))
        self.assertFalse(self.metadata.is_xmp_tag('Exif.Image.Make'))
        self.assertFalse(self.metadata.is_xmp_tag('Iptc.Application2.City'))

    def test_is_exif_tag(self):
        self.assertFalse(self.metadata.is_exif_tag('Xmp.dc.subject'))
        self.assertTrue(self.metadata.is_exif_tag('Exif.Image.Make'))
        self.assertFalse(self.metadata.is_exif_tag('Iptc.Application2.City'))

    def test_is_iptc_tag(self):
        self.assertFalse(self.metadata.is_iptc_tag('Xmp.dc.subject'))
        self.assertFalse(self.metadata.is_iptc_tag('Exif.Image.Make'))
        self.assertTrue(self.metadata.is_iptc_tag('Iptc.Application2.City'))

    def test_has_iptc(self):
        self.assertFalse(self.metadata.has_iptc())
        self.metadata.set_tag_string('Iptc.Application2.City', 'Copenhagen')
        self.assertTrue(self.metadata.has_iptc())

    def test_clear_iptc(self):
        self.metadata.set_tag_string('Iptc.Application2.City', 'Copenhagen')
        self.metadata.clear_iptc()
        self.assertFalse(self.metadata.has_iptc())
        self.assertTrue(self.metadata.has_exif())

    def test_get_iptc_tags(self):
        self.assertEqual(self.metadata.get_iptc_tags(), [])
        self.metadata.set_tag_string('Iptc.Application2.City', 'Copenhagen')
        self.assertEqual(
            self.metadata.get_iptc_tags(), ['Iptc.Application2.City'])

    def test_try_tag_supports_multiple_values(self):
        # Test different types of built-in/added tags for Exif/Iptc/Xmp
         
        # Add a new xmpText string
        self.metadata.set_tag_string('Xmp.dc.NewXmpTextValue', 'New Value')
        
        ### Test Exif tags
        self.assertFalse(self.metadata.try_tag_supports_multiple_values('Exif.Image.Model'))
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'Exif.Madeup.Orientation')
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'Exif.Image.MadeUp')
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'Exif.MadeUp.MadeUp')

        ### Test Iptc tags
        self.assertTrue(self.metadata.try_tag_supports_multiple_values('Iptc.Application2.Keywords'))
        self.assertFalse(self.metadata.try_tag_supports_multiple_values('Iptc.Envelope.ServiceId'))
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'Iptc.Madeup.Keywords')
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'Iptc.Application2.Madeup')
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'Iptc.Madeup.MadeUp')

        ### Test Xmp tags
        self.assertFalse(self.metadata.try_tag_supports_multiple_values('Xmp.xmpMM.InstanceID'))
        self.assertTrue(self.metadata.try_tag_supports_multiple_values('Xmp.dc.subject'))
        self.assertTrue(self.metadata.try_tag_supports_multiple_values('Xmp.photoshop.TextLayers'))
        self.assertTrue(self.metadata.try_tag_supports_multiple_values('Xmp.xmpRights.UsageTerms'))
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'Xmp.MadeUp.subject')
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'Xmp.dc.MadeUp')
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'Xmp.MadeUp.MadeUp')
        self.assertFalse(self.metadata.try_tag_supports_multiple_values('Xmp.dc.NewXmpTextValue'))

        ### Test Other tags
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values, '')
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'MadeUp.dc.subject')
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'MadeUp.MadeUp.subject')
        self.assertRaises(GLib.GError,self.metadata.try_tag_supports_multiple_values,'MadeUp.MadeUp.MadeUp')

    def test_delete_gps_info(self):
        # Longitude, latitude, altitude
        self.metadata.set_gps_info(-123.35, 48.43, 10)
        self.assertTrue('Exif.GPSInfo.GPSLongitude' in self.metadata.get_exif_tags())
        self.metadata.delete_gps_info()
        self.assertFalse('Exif.GPSInfo.GPSLongitude' in self.metadata.get_exif_tags())

    def test_get_gps_longitude(self):
        # Longitude, latitude, altitude
        self.metadata.set_gps_info(-123.35, 48.43, 10)
        # Should accurate to 9 decimal places despite rounding errors.
        self.assertAlmostEqual(self.metadata.get_gps_longitude(), -123.35, 9)

    def test_get_gps_latitude(self):
        # Longitude, latitude, altitude
        self.metadata.set_gps_info(-123.35, 48.43, 10)
        # Should accurate to 9 decimal places despite rounding errors.
        self.assertAlmostEqual(self.metadata.get_gps_latitude(), 48.43, 9)

    def test_get_gps_altitude(self):
        # Longitude, latitude, altitude
        self.metadata.set_gps_info(-123.35, 48.43, 10)
        # Should accurate to 9 decimal places despite rounding errors.
        self.assertEqual(self.metadata.get_gps_altitude(), 10)

    def test_get_gps_info(self):
        # Longitude, latitude, altitude
        self.metadata.set_gps_info(-123.35, 48.43, 10)
        # Should accurate to 9 decimal places despite rounding errors.
        lon, lat, alt = self.metadata.get_gps_info()
        self.assertAlmostEqual(lon, -123.35, 9)
        self.assertAlmostEqual(lat, 48.43, 9)
        self.assertEqual(alt, 10)

    def test_try_get_gps_info_without_altitude(self):
        self.metadata.set_gps_info(-1.0, 2.0, 3.0)
        self.metadata.clear_tag('Exif.GPSInfo.GPSAltitudeRef')
        lon, lat, alt = self.metadata.try_get_gps_info()
        self.assertAlmostEqual(lon, -1.0, 9)
        self.assertAlmostEqual(lat, 2.0, 9)
        self.assertAlmostEqual(alt, 0.0)

    def test_get_gps_info_without_altitude(self):
        self.metadata.set_gps_info(-2.0, 4.0, 3.0)
        self.metadata.clear_tag('Exif.GPSInfo.GPSAltitudeRef')
        lon, lat, alt = self.metadata.get_gps_info()
        self.assertAlmostEqual(lon, -2.0, 9)
        self.assertAlmostEqual(lat, 4.0, 9)
        self.assertTrue(math.isnan(alt))

    def test_set_gps_info(self):
        # Longitude, latitude, altitude
        self.metadata.set_gps_info(-123.35, 48.43, 10)
        self.assertEqual(
            [(tag, self.metadata.get_tag_string(tag))
             for tag in self.metadata.get_exif_tags()
             if 'GPS' in tag],
            [('Exif.GPSInfo.GPSAltitude', '10/1'),
             ('Exif.GPSInfo.GPSAltitudeRef', '0'),
             ('Exif.GPSInfo.GPSLatitude', '48/1 25/1 47999999/1000000'),
             ('Exif.GPSInfo.GPSLatitudeRef', 'N'),
             ('Exif.GPSInfo.GPSLongitude', '123/1 20/1 59999999/1000000'),
             ('Exif.GPSInfo.GPSLongitudeRef', 'W'),
             ('Exif.GPSInfo.GPSMapDatum', 'WGS-84'),
             ('Exif.GPSInfo.GPSVersionID', '2 0 0 0')])

    def test_bogus_gps_info(self):
        # Longitude, latitude, altitude
        self.metadata.set_gps_info(1.0, 1.0, 1.0)
        bogus_data = '1/0 1/1 1/1'
        self.metadata.set_tag_string('Exif.GPSInfo.GPSLatitude', bogus_data)

        #self.assertAlmostEqual(self.metadata.get_gps_latitude(), 0.0)
        #self.metadata.set_tag_string('Exif.GPSInfo.GPSLongitude', bogus_data)
        #self.assertAlmostEqual(self.metadata.get_gps_longitude(), 0.0)

        #bogus_data = '1/1 1/0 1/1'
        #self.metadata.set_tag_string('Exif.GPSInfo.GPSLatitude', bogus_data)
        #self.assertAlmostEqual(self.metadata.get_gps_latitude(), 1.0)
        #self.metadata.set_tag_string('Exif.GPSInfo.GPSLongitude', bogus_data)
        #self.assertAlmostEqual(self.metadata.get_gps_longitude(), 1.0)

        #bogus_data = '1/1 6/1 1/0'
        #self.metadata.set_tag_string('Exif.GPSInfo.GPSLatitude', bogus_data)
        #self.assertAlmostEqual(self.metadata.get_gps_latitude(), 1.1)
        #self.metadata.set_tag_string('Exif.GPSInfo.GPSLongitude', bogus_data)
        #self.assertAlmostEqual(self.metadata.get_gps_longitude(), 1.1)

        #self.metadata.set_tag_string('Exif.GPSInfo.GPSAltitude', '1/0')
        #self.assertAlmostEqual(self.metadata.get_gps_altitude(), 0.0)

    def test_overwrite_gps_info(self):
        self.metadata.set_tag_string('Exif.GPSInfo.GPSDateStamp', '2019:04:25')
        self.metadata.set_tag_string('Exif.GPSInfo.GPSTimeStamp', '10/1 11/1 50/1')
        self.metadata.set_tag_string('Exif.GPSInfo.GPSVersionID', '2 2 0 0')
        self.metadata.set_gps_info(-123.35, 48.43, 10)
        self.assertEqual(
            [(tag, self.metadata.get_tag_string(tag))
             for tag in self.metadata.get_exif_tags()
             if 'GPS' in tag],
            [('Exif.GPSInfo.GPSAltitude', '10/1'),
             ('Exif.GPSInfo.GPSAltitudeRef', '0'),
             ('Exif.GPSInfo.GPSLatitude', '48/1 25/1 47999999/1000000'),
             ('Exif.GPSInfo.GPSLatitudeRef', 'N'),
             ('Exif.GPSInfo.GPSLongitude', '123/1 20/1 59999999/1000000'),
             ('Exif.GPSInfo.GPSLongitudeRef', 'W'),
             ('Exif.GPSInfo.GPSMapDatum', 'WGS-84'),
             ('Exif.GPSInfo.GPSVersionID', '2 0 0 0')])

    def test_update_gps_info(self):
        self.metadata.set_tag_string('Exif.GPSInfo.GPSDateStamp', '2019:04:25')
        self.metadata.set_tag_string('Exif.GPSInfo.GPSTimeStamp', '10/1 11/1 50/1')
        self.metadata.set_tag_string('Exif.GPSInfo.GPSVersionID', '2 2 0 0')
        self.metadata.update_gps_info(-123.35, 48.43, 10)
        self.assertEqual(
            [(tag, self.metadata.get_tag_string(tag))
             for tag in self.metadata.get_exif_tags()
             if 'GPS' in tag],
            [('Exif.GPSInfo.GPSAltitude', '10/1'),
             ('Exif.GPSInfo.GPSAltitudeRef', '0'),
             ('Exif.GPSInfo.GPSDateStamp', '2019:04:25'),
             ('Exif.GPSInfo.GPSLatitude', '48/1 25/1 47999999/1000000'),
             ('Exif.GPSInfo.GPSLatitudeRef', 'N'),
             ('Exif.GPSInfo.GPSLongitude', '123/1 20/1 59999999/1000000'),
             ('Exif.GPSInfo.GPSLongitudeRef', 'W'),
             ('Exif.GPSInfo.GPSMapDatum', 'WGS-84'),
             ('Exif.GPSInfo.GPSTimeStamp', '10/1 11/1 50/1'),
             ('Exif.GPSInfo.GPSVersionID', '2 2 0 0')])

    def test_get_preview_properties(self):
        previews = self.metadata.get_preview_properties()
        self.assertEqual(len(previews), 2)
        widths = [640, 160]
        heights = [480, 120]
        nbytes = [27981, 4534]
        for preview in previews:
            self.assertIsInstance(preview, GExiv2.PreviewProperties)
            self.assertEqual(preview.get_width(), widths.pop())
            self.assertEqual(preview.get_height(), heights.pop())
            thumb = self.metadata.get_preview_image(preview)
            self.assertEqual(len(thumb.get_data()), nbytes.pop())

    def test_save_file(self):
        shutil.copyfile(self.get_input_file(), self.get_output_file())
        self.metadata.set_gps_info(12.5683371, 55.6760968, 42)
        self.metadata.save_file(self.get_output_file())

        newfile = GExiv2.Metadata()
        newfile.open_path(self.get_output_file())
        self.assertEqual(
            [(tag, newfile.get_tag_string(tag))
             for tag in newfile.get_exif_tags()
             if 'GPSInfo' in tag],
            [(tag, self.metadata.get_tag_string(tag))
             for tag in newfile.get_exif_tags()
             if 'GPSInfo' in tag])

    def test_set_comment(self):
        self.metadata.set_comment('foobar')
        self.assertEqual(self.metadata.get_comment(), 'foobar')
        self.assertEqual(
            self.metadata.get_tag_string('Exif.Image.ImageDescription'),
            'foobar')

    def FIXME_test_clear_comment(self):
        self.metadata.set_tag_string('Exif.Image.ImageDescription',
                                     'Lego Dragon, Copenhagen.')
        self.assertEqual(self.metadata.get_comment(), 'Lego Dragon, Copenhagen.')
        self.metadata.clear_comment()
        self.assertEqual(self.metadata.get_comment().strip(), '')

    def test_try_get_xmp_namespace_for_tag(self):
        self.assertEqual(self.metadata.try_get_xmp_namespace_for_tag('dc'), 'http://purl.org/dc/elements/1.1/')
        self.assertEqual(self.metadata.try_get_xmp_namespace_for_tag('Xmp.dc.subject'), 'http://purl.org/dc/elements/1.1/')
        self.assertEqual(self.metadata.try_get_xmp_namespace_for_tag('Xmp.dc.MadeUp'), 'http://purl.org/dc/elements/1.1/')

        # Invalid "familyName"
        with self.assertRaises(gi.repository.GLib.GError):
            self.metadata.get_xmp_namespace_for_tag('xmp.dc.subject')
        with self.assertRaises(gi.repository.GLib.GError):
            self.metadata.get_xmp_namespace_for_tag('MadeUp.dc.subject')
        with self.assertRaises(gi.repository.GLib.GError):
            self.metadata.get_xmp_namespace_for_tag('.dc.subject')

        # Invalid "groupName"
        with self.assertRaises(gi.repository.GLib.GError):
            self.metadata.get_xmp_namespace_for_tag('MadeUp')
        with self.assertRaises(gi.repository.GLib.GError):
            self.metadata.get_xmp_namespace_for_tag('Xmp.MadeUp.subject')
        with self.assertRaises(gi.repository.GLib.GError):
            self.metadata.get_xmp_namespace_for_tag('Xmp..subject')

        # Missing "tagName"
        with self.assertRaises(gi.repository.GLib.GError):
            self.metadata.get_xmp_namespace_for_tag('Xmp.dc')
        with self.assertRaises(gi.repository.GLib.GError):
            self.metadata.get_xmp_namespace_for_tag('Xmp.dc.')
