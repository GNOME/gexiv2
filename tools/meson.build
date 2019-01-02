if vapi_available
  executable('gexiv2-dump',
             'gexiv2-dump.vala',
             include_directories : include_directories('..'),
             dependencies : [gobject, vapi, gio],
             link_with : gexiv2)

    executable('gexiv2-copy',
               'gexiv2-copy.vala',
               include_directories : include_directories('..'),
               dependencies : [gobject, vapi, gio],
               link_with : gexiv2)
endif
