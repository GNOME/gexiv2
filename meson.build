project(
    'gexiv2',
    ['c', 'cpp'],
    version : '0.14.0',
    license: 'GPL-2.0-or-later',
    meson_version : '>=0.51',
    default_options : [
        'cpp_std=c++11'
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
python3 = import('python').find_installation('python3', modules: 'gi', required: get_option('python3'))

bmff_test = '''#include <exiv2/exiv2.hpp>
#ifndef EXV_ENABLE_BMFF
#error No BMFF support in exiv2
#endif
'''

bmff_available = cpp.compiles(bmff_test, name : 'BMFF support in exiv2', dependencies : [exiv2])

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

if get_option('python3') == false
    warning('\'python3\' is disabled, any Python tests will not be run.')
else
  if get_option('introspection') == false
    error('Build option \'python3\' requires \'introspection\' to be enabled.')
  endif

  girdir = get_option('python3_girdir')
  if girdir == 'auto'
    python3_output = run_command(python3, ['-c', 'import gi; print(gi._overridesdir)'])
    if python3_output.returncode() != 0
      error('Finding the Python 3 gi/override path: ' + python3_output.stderr())
    endif
    girdir = python3_output.stdout().strip()
  endif
  
  message('Installing \'GExiv2.py\' into \'' + girdir + '\'')
  python3.install_sources('GExiv2.py', subdir : girdir)
endif

subdir('test')

if get_option('tools')
  subdir('tools')
endif
