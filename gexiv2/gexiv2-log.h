/*
 * gexiv2-log.h
 *
 * Author(s)
 * 	Jim Nelson <jim@yorba.org>
 *
 * This is free software. See COPYING for details.
 */

#ifndef __GEXIV2_LOG_H__
#define __GEXIV2_LOG_H__

#include <glib.h>

G_BEGIN_DECLS

typedef enum {
	GEXIV2_LOG_LEVEL_DEBUG	= 0,
	GEXIV2_LOG_LEVEL_INFO	= 1,
	GEXIV2_LOG_LEVEL_WARN	= 2,
	GEXIV2_LOG_LEVEL_ERROR	= 3,
	GEXIV2_LOG_LEVEL_MUTE	= 4
} GExiv2LogLevel;

typedef void (*GExiv2LogHandler)(GExiv2LogLevel level, const gchar *msg);

GExiv2LogLevel		gexiv2_log_get_level(void);
void				gexiv2_log_set_level(GExiv2LogLevel level);
GExiv2LogHandler	gexiv2_log_get_handler(void);
GExiv2LogHandler	gexiv2_log_get_default_handler(void);
void				gexiv2_log_set_handler(GExiv2LogHandler handler);
void				gexiv2_log_use_glib_logging(void);

G_END_DECLS

#endif /* __GEXIV2_LOG_H__ */
