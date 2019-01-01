project(
    'gexiv2',
    ['c', 'cpp'],
    version : '0.11.0',
    default_options : [
        'cpp_std=c++11'
    ]
)

gnome = import('gnome')
pkg = import('pkgconfig')

exiv2 = dependency('exiv2', version : '>= 0.26')
gobject = dependency('gobject-2.0', version : '>= 2.38.0')

gir = find_program('g-ir-scanner', required: false)
vapigen = find_program('vapigen', required: false)
vala = find_program('valac', required: false)

introspection_available = gir.found() and get_option('introspection')
vapi_available = introspection_available and vapigen.found() and get_option('vapi')

subdir('gexiv2')

if get_option('gtk_doc')
  subdir('docs')
endif

python2_available = false
python3_available = false
python_gi_test_args = ['-c', 'import gi; print(gi._overridesdir)']
python2_girdir = get_option('python2-girdir')
python3_girdir = get_option('python3-girdir')

if introspection_available
  if python2_girdir != 'no'
    python2 = find_program('python2', required : false)
    if python2.found()
      py2gi = run_command(python2, python_gi_test_args)
      if py2gi.returncode() == 0
        python2_available = true
        if python2_girdir == 'auto'
          python2_girdir = py2gi.stdout().strip()
        endif
        install_data('GExiv2.py', install_dir : python2_girdir)
      endif
    endif
  endif

  if python3_girdir != 'no'
    python3 = find_program('python3', required : false)
    if python3.found()
      py3gi = run_command(python3, python_gi_test_args)
      if py3gi.returncode() == 0
        python3_available = true
        if python3_girdir == 'auto'
          python3_girdir = py3gi.stdout().strip()
        endif
        install_data('GExiv2.py', install_dir : python3_girdir)
      endif
    endif
  endif
endif

subdir('test')
