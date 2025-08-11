pkg = import('pkgconfig')

as_version = meson.project_version().split('.')

soversion = 4
library_minor = as_version[0].to_int() * 100 + as_version[1].to_int()
library_micro = as_version[2].to_int()

libversion = '@0@.@1@.@2@'.format(soversion, library_minor, library_micro)
libversion_arr = libversion.split('.')
darwin_version_major = libversion_arr[0].to_int()
darwin_version_minor = libversion_arr[1].to_int()
darwin_version_micro = libversion_arr[2].to_int()
darwin_versions = [darwin_version_major + darwin_version_minor + 1, '@0@.@1@'.format(darwin_version_major + darwin_version_minor + 1, darwin_version_micro)]

header_folder = join_paths('gexiv2-' + project_api_version, 'gexiv2')
gexiv2_include_dir = join_paths(get_option('includedir'), header_folder)

config = configuration_data()
config.set('GEXIV2_MAJOR_VERSION', as_version[0])
config.set('GEXIV2_MINOR_VERSION', as_version[1])
config.set('GEXIV2_MICRO_VERSION', as_version[2])

version_header = configure_file(input: 'gexiv2-version.h.meson',
                                output: 'gexiv2-version.h',
                                configuration: config,
                                install_dir : gexiv2_include_dir)

gexiv2_enum_headers = ['gexiv2-metadata.h', 'gexiv2-log.h']

gexiv2_headers = gexiv2_enum_headers + ['gexiv2.h',
                  'gexiv2-preview-properties.h',
                  'gexiv2-preview-image.h',
                  'gexiv2-startup.h']

enum_sources = gnome.mkenums('gexiv2-enums',
                             sources : gexiv2_enum_headers,
                             h_template : 'gexiv2-enums.h.template',
                             c_template : 'gexiv2-enums.cpp.template',
                             identifier_prefix : 'gexiv2',
                             symbol_prefix : 'GExiv2',
                             install_header : true,
                             install_dir : gexiv2_include_dir)

install_headers(gexiv2_headers, subdir : header_folder)

gexiv2 = library('gexiv2-' + project_api_version,
                 ['gexiv2-metadata.cpp',
                  'gexiv2-metadata-exif.cpp',
                  'gexiv2-metadata-gps.cpp',
                  'gexiv2-metadata-iptc.cpp',
                  'gexiv2-metadata-xmp.cpp',
                  'gexiv2-preview-properties.cpp',
                  'gexiv2-preview-image.cpp',
                  'gexiv2-log.cpp',
                  'gexiv2-startup.cpp',
                  'gexiv2-log-private.h',
                  'gexiv2-metadata-private.h',
                  'gexiv2-preview-properties-private.h',
                  'gexiv2-preview-image-private.h',
                  'gexiv2-util-private.h',
                  'gexiv2-gio-io.h'] +
                 gexiv2_headers +
                 [version_header, config_h] +
                 enum_sources,
                 include_directories : include_directories('..'),
                 version: libversion,
                 darwin_versions: darwin_versions,
                 dependencies : [gobject, exiv2, gio],
                 vs_module_defs : 'gexiv2.def',
                 install : true)

update_def_script = find_program('update-def-file.sh', dirs: [meson.project_source_root() / 'build-aux'])
custom_target('update-def-file',
  output: 'gexiv2.def',
  input: gexiv2,
  command: [update_def_script, '@INPUT@', '@OUTPUT@'],
  install: false)

libgexiv2 = declare_dependency(
    link_with : gexiv2,
    include_directories : include_directories('..'),
    dependencies : [gobject, exiv2],
)

pc_deps = ['glib-2.0', 'gobject-2.0', 'gio-2.0']

pkg.generate(
    description : 'GObject bindings for exiv2',
    url : project_url,
    libraries : gexiv2,
    version : meson.project_version(),
    subdirs : ['gexiv2-' + project_api_version],
    name : 'GExiv2',
    filebase : 'gexiv2-' + project_api_version,
    requires_private : 'exiv2',
    requires : pc_deps
    )

# set unconditionally
typelib_path = meson.current_build_dir()

vapi = disabler()
if get_option('introspection')
  gir = gnome.generate_gir(gexiv2,
      sources : ['gexiv2-preview-properties.h',
                 'gexiv2-preview-image.h',
                 'gexiv2-startup.h',
                 'gexiv2-metadata.h',
                 'gexiv2-log.h',
                 version_header,
                 enum_sources.get(1)
                 ],
      namespace : 'GExiv2',
      nsversion : project_api_version,
      symbol_prefix : 'gexiv2',
      identifier_prefix : 'GExiv2',
      export_packages : 'gexiv2',
      includes : ['GObject-2.0', 'Gio-2.0'],
      header : 'gexiv2/gexiv2.h',
      install : true)

    if get_option('vapi')
        vapi = gnome.generate_vapi('gexiv2-' + project_api_version,
                               sources : gir.get(0),
                               install : true,
                               packages: pc_deps)
    endif
else
    gir = ['', '']
endif
