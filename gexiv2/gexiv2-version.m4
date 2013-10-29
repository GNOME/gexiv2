/*
 * gexiv2-version.h
 *
 * This is free software. See COPYING for details.
 */

#ifndef __GEXIV2_VERSION_H__
#define __GEXIV2_VERSION_H__

#include <glib-object.h>

G_BEGIN_DECLS

`#'define GEXIV2_MAJOR_VERSION _VERSION_MAJOR_
`#'define GEXIV2_MINOR_VERSION _VERSION_MINOR_
`#'define GEXIV2_MICRO_VERSION _VERSION_MICRO_

#define GEXIV2_CHECK_VERSION(major, minor, micro) \
    (GEXIV2_MAJOR_VERSION > (major) || \
     (GEXIV2_MAJOR_VERSION == (major) && GEXIV2_MINOR_VERSION > (minor)) || \
     (GEXIV2_MAJOR_VERSION == (major) && GEXIV2_MINOR_VERSION == (minor) && \
      GEXIV2_MICRO_VERSION >= (micro)))

gint gexiv2_get_version (void);

G_END_DECLS
#endif
