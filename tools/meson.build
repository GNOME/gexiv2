executable('gexiv2-tool',
           'gexiv2-tool.vala',
           include_directories : include_directories('..'),
           dependencies : [gobject, vapi, gio],
           vala_args: ['--disable-since-check'],
           c_args : ['-Wno-error=unused-but-set-variable', '-Wno-error=unused-variable'],
           link_with : gexiv2)
