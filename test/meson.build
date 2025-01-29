test_sample_path = join_paths(meson.current_source_dir(), 'data')

test_env = environment()
test_env.set('G_SLICE', 'always-malloc')
test_env.set('TEST_DATA_DIR', test_sample_path)
test_env.prepend('GI_TYPELIB_PATH', typelib_path)
test_env.prepend('LD_LIBRARY_PATH', typelib_path)
test_env.prepend('PYTHONPATH', join_paths(meson.current_source_dir(), 'python'))
test_env.prepend('LC_ALL', 'en_US.UTF-8')

# For windows
test_env.prepend('PATH', typelib_path)

regression_test = executable('gexiv2-regression', 'gexiv2-regression.c',
                             dependencies : [gobject, gio, math],
                             include_directories : include_directories('..'),
                             c_args : [
                               '-DSAMPLE_PATH="@0@"'.format(test_sample_path),
                               '-Wno-deprecated-declarations',
                             ],
                             link_with : gexiv2)

test('regression', regression_test, env : test_env)

if get_option('python3')
  test('python3-gexiv2', python3, args: ['-m', 'unittest', 'gexiv2'], env: test_env)
  test('python3-metadata', python3, args: ['-m', 'unittest', 'test_metadata'], env: test_env)
endif
