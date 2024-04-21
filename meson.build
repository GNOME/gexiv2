project(
    'gexiv2',
    ['c', 'cpp'],
    version : '0.15.0',
    license: 'GPL-2.0-or-later',
    meson_version : '>=0.51',
    default_options : [
        'cpp_std=c++17'
    ]
)

project_api_version = '0.12'
message('Project API version: ' + project_api_version)

project_url = 'https://wiki.gnome.org/Projects/gexiv2'

gnome = import('gnome')
pkg = import('pkgconfig')

exiv2 = dependency('exiv2', version : '>= 0.27')

# when building for 0.26 or 0.27 and with clang's libc++, reenable std::auto_ptr
# and remove deprecation warnings for that
if exiv2.version().version_compare('< 0.28')
  add_project_arguments('-D_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR', language: 'cpp')
  add_project_arguments('-D_LIBCPP_DISABLE_DEPRECATION_WARNINGS', language: 'cpp')
endif
gobject = dependency('gobject-2.0', version : '>= 2.46.0')
gio = dependency('gio-2.0', version : '>= 2.46.0')
cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')
math = cc.find_library('m', required : false)
bmff_test = '''#define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
#include <exiv2/exiv2.hpp>
#ifndef EXV_ENABLE_BMFF
#error No BMFF support in exiv2
#endif
'''

bmff_available = cpp.compiles(bmff_test, name : 'BMFF support in exiv2', dependencies : [exiv2])

private_pdata_test = '''#define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
#include <exiv2/exiv2.hpp>
#include <iostream>
int main(void) {
  Exiv2::DataBuf buf;
  std::cout << buf.c_data() << std::endl;

  return 0;
}'''
private_pdata = cpp.compiles(private_pdata_test, name : 'DataBuf is sealead in exiv2', dependencies : [exiv2])

has_any_error_test = '''#define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
#include <exiv2/exiv2.hpp>
#include <iostream>
int main(void) {
  Exiv2::AnyError *e;

  return 0;
}'''
has_any_error = cpp.compiles(has_any_error_test, name : 'AnyError exists in exiv2', dependencies : [exiv2])

xmpdatum_has_to_long_test = '''#define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
#include <exiv2/exiv2.hpp>
#include <iostream>
int main(void) {
  Exiv2::XmpKey k{"Xmp.audio.Compressor"};
  Exiv2::Xmpdatum d{k, nullptr};
  d.toLong();

  return 0;
}'''
xmpdatum_has_to_long = cpp.compiles(xmpdatum_has_to_long_test, name: 'Xmpdatum has toLong() in exiv2', dependencies : [exiv2])

exifdatum_has_to_long_test = '''#define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
#include <exiv2/exiv2.hpp>
#include <iostream>
int main(void) {
  Exiv2::ExifKey k{"Exif.Image.OECF"};
  Exiv2::Exifdatum d{k, nullptr};
  d.toLong();

  return 0;
}'''
exifdatum_has_to_long = cpp.compiles(exifdatum_has_to_long_test, name: 'Exifdatum has toLong() in exiv2', dependencies : [exiv2])

build_config = configuration_data ()
build_config.set('EXIV2_DATABUF_HAS_PRIVATE_PDATA', private_pdata)
build_config.set('EXIV2_HAS_ANY_ERROR', has_any_error)
build_config.set('EXIV2_XMPDATUM_HAS_TO_LONG', xmpdatum_has_to_long)
build_config.set('EXIV2_EXIFDATUM_HAS_TO_LONG', exifdatum_has_to_long)
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
