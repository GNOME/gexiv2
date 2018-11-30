subdir('xml')
gnome.gtkdoc('gexiv2',
             main_xml : 'gexiv2-docs.xml',
             gobject_typesfile : 'gexiv2.types',
             src_dir : [join_paths(meson.source_root(), 'gexiv2'),
                        join_paths(meson.build_root(), 'gexiv2')],
             dependencies : libgexiv2,
             install : true)
