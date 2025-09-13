project(
    'gexiv2',
    ['c', 'cpp'],
    version : '0.16.0',
    license: 'GPL-2.0-or-later',
    meson_version : '>=1.4',
    default_options : [
        'cpp_std=c++17'
    ]
)

project_api_version = '0.16'
message('Project API version: ' + project_api_version)

project_url = 'https://wiki.gnome.org/Projects/gexiv2'

gnome = import('gnome')
pkg = import('pkgconfig')

exiv2 = dependency('exiv2', version : '>= 0.28')

gobject = dependency('gobject-2.0', version : '>= 2.46.0')
gio = dependency('gio-2.0', version : '>= 2.46.0')
cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')
math = cc.find_library('m', required : false)

build_config = configuration_data ()
config_h = configure_file(
  output: 'config.h',
  configuration: build_config
)


find_program('g-ir-scanner', required: get_option('introspection'))

if get_option('vapi')
  if get_option('introspection') == false
    error('\'vapi\' build option requires \'introspection\' to be enabled.')
  endif
  vapi_available = add_languages('vala', required: true, native: false)
endif

subdir('gexiv2')

gidocgen_dep = dependency('gi-docgen', version: '>= 2021.1',
                          fallback: ['gi-docgen', 'dummy_dep'],
                          native: true,
                          required: get_option('gtk_doc') and get_option('introspection'),
                          disabler: true
                      )

if get_option('gtk_doc')
  subdir('docs')
endif

if get_option('python3')
  if get_option('introspection') == false
    error('Build option \'python3\' requires \'introspection\' to be enabled.')
  endif
  python3 = import('python').find_installation('python3', modules: 'gi')
  python3.install_sources('GExiv2.py', subdir: 'gi/overrides')
else
  python3 = disabler()
endif

if get_option('tests')
  subdir('test')
endif

if get_option('tools')
  subdir('tools')
endif

if not meson.is_subproject()
  meson.add_dist_script('build-aux/dist-docs.py')
endif
