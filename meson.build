project(
    'gexiv2',
    ['c', 'cpp'],
    version : '0.14.1',
    license: 'GPL-2.0-or-later',
    meson_version : '>=0.51',
    default_options : [
        'cpp_std=c++17'
    ]
)

project_api_version = '0.10'
message('Project API version: ' + project_api_version)

project_url = 'https://wiki.gnome.org/Projects/gexiv2'

gnome = import('gnome')
pkg = import('pkgconfig')

exiv2 = dependency('exiv2', version : '>= 0.26')
gobject = dependency('gobject-2.0', version : '>= 2.46.0')
gio = dependency('gio-2.0', version : '>= 2.46.0')
cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')
math = cc.find_library('m', required : false)

bmff_test = '''#include <exiv2/exiv2.hpp>
#ifndef EXV_ENABLE_BMFF
#error No BMFF support in exiv2
#endif
'''

bmff_available = cpp.compiles(bmff_test, name : 'BMFF support in exiv2', dependencies : [exiv2])

private_pdata_test = '''#include <exiv2/exiv2.hpp>
#include <iostream>
int main(void) {
  Exiv2::DataBuf buf;
  std::cout << buf.c_data() << std::endl;
}'''
private_pdata = cpp.compiles(private_pdata_test, name : 'DataBuf is sealead in exiv2', dependencies : [exiv2])

build_config = configuration_data ()
build_config.set10('EXIV2_DATABUF_HAS_PRIVATE_PDATA', private_pdata)
config_h = configure_file(
  output: 'config.h',
  configuration: build_config
)

find_program('g-ir-scanner', required: get_option('introspection'))

if get_option('vapi')
  if get_option('introspection') == false
    error('\'vapi\' build option requires \'introspection\' to be enabled.')
  endif
  vapi_available = add_languages('vala', required: true)
endif

subdir('gexiv2')

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
