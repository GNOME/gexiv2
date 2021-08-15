test_sample_path = join_paths(meson.current_source_dir(), 'data')
python_module_path = join_paths(meson.current_build_dir(), 'python')

test_env = environment()
test_env.set('G_SLICE', 'always-malloc')
test_env.set('TEST_DATA_DIR', test_sample_path)
test_env.prepend('GI_TYPELIB_PATH', typelib_path)
test_env.prepend('LD_LIBRARY_PATH', typelib_path)
test_env.prepend('PYTHONPATH', python_module_path)

regression_test = executable('gexiv2-regression', 'gexiv2-regression.c',
                             dependencies : [gobject, gio, math],
                             include_directories : include_directories('..'),
                             c_args : [
                               '-DSAMPLE_PATH="@0@"'.format(test_sample_path),
                               '-Wno-deprecated-declarations',
                             ],
                             link_with : gexiv2)

test('regression', regression_test, env : test_env)

subdir('python')

python3_test_conf = configuration_data()
python3_test_conf.set('PYTHON3_PATH', python3.path())
python3_test = configure_file(input: 'python3-test.in',
                              output : 'python3-test',
                              configuration: python3_test_conf)
test('python3', find_program(python3_test), env : test_env)
