#
# For manually building the distributed VAPI file
#

../gexiv2.vapi: gexiv2.gi
	vapigen --library=gexiv2 --metadatadir=. gexiv2.gi
	mv gexiv2.vapi ..

gexiv2.gi: gexiv2.deps gexiv2.excludes gexiv2.files gexiv2.metadata gexiv2.namespace
	vala-gen-introspect gexiv2 .

