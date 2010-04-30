/*
 * gexiv2-preview-properties.h
 *
 * Author(s)
 * 	Jim Nelson <jim@yorba.org>
 *
 * This is free software. See COPYING for details.
 */

#ifndef __GEXIV2_PREVIEW_PROPERTIES_H__
#define __GEXIV2_PREVIEW_PROPERTIES_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GEXIV2_TYPE_PREVIEW_PROPERTIES \
	(gexiv2_preview_properties_get_type ())
	
#define GEXIV2_PREVIEW_PROPERTIES(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), GEXIV2_TYPE_PREVIEW_PROPERTIES, GExiv2PreviewProperties))
	
#define GEXIV2_IS_PREVIEW_PROPERTIES(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEXIV2_TYPE_PREVIEW_PROPERTIES))
	
#define GEXIV2_PREVIEW_PROPERTIES_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), GEXIV2_TYPE_PREVIEW_PROPERTIES, GExiv2PreviewPropertiesClass))
	
#define GEXIV2_IS_PREVIEW_PROPERTIES_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), GEXIV2_TYPE_PREVIEW_PROPERTIES))
	
#define GEXIV2_PREVIEW_PROPERTIES_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), GEXIV2_TYPE_PREVIEW_PROPERTIES, GExiv2PreviewPropertiesClass))
	
#define GEXIV2_PREVIEW_PROPERTIES_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GEXIV2_TYPE_PREVIEW_PROPERTIES, GExiv2PreviewPropertiesPrivate))


typedef struct _GExiv2PreviewProperties			GExiv2PreviewProperties;
typedef struct _GExiv2PreviewPropertiesClass	GExiv2PreviewPropertiesClass;
typedef struct _GExiv2PreviewPropertiesPrivate	GExiv2PreviewPropertiesPrivate;

struct _GExiv2PreviewProperties
{
	GObject parent_instance;

	/*< private >*/
	GExiv2PreviewPropertiesPrivate *priv;
};

struct _GExiv2PreviewPropertiesClass
{
	GObjectClass parent_class;
};

/* basic functions */

GType 			gexiv2_preview_properties_get_type			(void);


/* preview properties */

const gchar*	gexiv2_preview_properties_get_mime_type		(GExiv2PreviewProperties *self);
const gchar*	gexiv2_preview_properties_get_extension		(GExiv2PreviewProperties *self);
guint32			gexiv2_preview_properties_get_size			(GExiv2PreviewProperties *self);
guint32			gexiv2_preview_properties_get_width			(GExiv2PreviewProperties *self);
guint32			gexiv2_preview_properties_get_height		(GExiv2PreviewProperties *self);


G_END_DECLS

#endif /* __GEXIV2_PREVIEW_PROPERTIES_H__ */

