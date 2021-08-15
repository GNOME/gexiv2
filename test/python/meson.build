python_conf = configuration_data()
python_conf.set('PROJECT_API_VERSION', project_api_version)

gexiv2_py_file = configure_file(input: 'gexiv2.py.in',
                                output : 'gexiv2.py',
                                configuration: python_conf)

test_metadata_py_file = configure_file(input: 'test_metadata.py.in',
                                       output : 'test_metadata.py',
                                       configuration: python_conf)
