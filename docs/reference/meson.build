entities = configuration_data()
entities.set('VERSION', meson.project_version())

gidocgen = find_program('gi-docgen', required: true)

gexiv2_toml = configure_file (
    input: 'gexiv2.toml.in',
    output: 'gexiv2.toml',
    configuration: entities
)

docs_dir = join_paths(get_option('prefix'), get_option('datadir')) / 'doc/gexiv2-' + project_api_version + '/reference'

custom_target(
    'gexiv2-doc',
    input: gir[0],
    output: 'GExiv2',
    command : [
        gidocgen,
        'generate',
        '--quiet',
        '--add-include-path=@0@'.format(meson.current_build_dir() / '../gexiv2'),
        '--config', gexiv2_toml,
        '--output-dir=@OUTPUT@',
        '--no-namespace-dir',
        '--content-dir=@0@'.format(meson.current_source_dir()),
        '@INPUT@',
    ],
    depend_files : gexiv2_toml,
    build_by_default: true,
    install: true,
    install_dir : docs_dir,
)
