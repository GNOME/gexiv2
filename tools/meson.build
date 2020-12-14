if vapi_available
  executable('gexiv2-dump',
             'gexiv2-dump.vala',
             include_directories : include_directories('..'),
             dependencies : [gobject, vapi, gio],
             vala_args: ['--disable-since-check'],
             link_with : gexiv2)
endif
