test_sample_path = join_paths(meson.current_source_dir(), 'data')
python_module_path = join_paths(meson.current_source_dir(), 'python')

test_env = ['G_SLICE=always-malloc',
            'GI_TYPELIB_PATH=@0@'.format(typelib_path),
            'PYTHONPATH=@0@'.format(python_module_path),
            'TEST_DATA_DIR=@0@'.format(test_sample_path),
            'LD_LIBRARY_PATH=@0@'.format(typelib_path),
            ]

regression_test = executable('gexiv2-regression', 'gexiv2-regression.c',
                             dependencies : [gobject, gio],
                             include_directories : include_directories('..'),
                             c_args : '-DSAMPLE_PATH="@0@"'.format(test_sample_path),
                             link_with : gexiv2)

test('regression', regression_test, env : test_env)

if vapi_available
  executable('gexiv2-dump',
	     'gexiv2-dump.vala',
	     include_directories : include_directories('..'),
	     dependencies : [gobject, vapi, gio],
	     link_with : gexiv2)
endif

if python2_available
  python2_test_conf = configuration_data()
  python2_test_conf.set('PYTHON2_PATH', python2.path())
  python2_test = configure_file(input: 'python2-test.in',
                                output : 'python2-test',
                                configuration: python2_test_conf)
  test('python2', find_program(python2_test), env : test_env)
endif

if python3_available
  python3_test_conf = configuration_data()
  python3_test_conf.set('PYTHON3_PATH', python3.path())
  python3_test = configure_file(input: 'python3-test.in',
                                output : 'python3-test',
                                configuration: python3_test_conf)
  test('python3', find_program(python3_test), env : test_env)
endif
