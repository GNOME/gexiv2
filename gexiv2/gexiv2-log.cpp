/*
 * gexiv2-log.cpp
 *
 * Author(s)
 *  Jim Nelson <jim@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "gexiv2-log-private.h"

G_BEGIN_DECLS

static GExiv2LogHandler installed_handler = nullptr;

static GExiv2LogLevel exiv2_level_to_gexiv2_level(Exiv2::LogMsg::Level level) {
    switch (level) {
        case Exiv2::LogMsg::debug:
            return GEXIV2_LOG_LEVEL_DEBUG;
        
        case Exiv2::LogMsg::info:
            return GEXIV2_LOG_LEVEL_INFO;
        
        case Exiv2::LogMsg::warn:
            return GEXIV2_LOG_LEVEL_WARN;
        
        case Exiv2::LogMsg::error:
            return GEXIV2_LOG_LEVEL_ERROR;
        
        case Exiv2::LogMsg::mute:
        default:
            return GEXIV2_LOG_LEVEL_MUTE;
    }
}

static Exiv2::LogMsg::Level gexiv2_level_to_exiv2_level(GExiv2LogLevel level) {
    switch (level) {
        case GEXIV2_LOG_LEVEL_DEBUG:
            return Exiv2::LogMsg::debug;
        
        case GEXIV2_LOG_LEVEL_INFO:
            return Exiv2::LogMsg::info;
        
        case GEXIV2_LOG_LEVEL_WARN:
            return Exiv2::LogMsg::warn;
        
        case GEXIV2_LOG_LEVEL_ERROR:
            return Exiv2::LogMsg::error;
        
        case GEXIV2_LOG_LEVEL_MUTE:
        default:
            return Exiv2::LogMsg::mute;
    }
}

static void log_handler_converter(int level, const char *msg) {
    if (installed_handler != nullptr)
        installed_handler(exiv2_level_to_gexiv2_level((Exiv2::LogMsg::Level) level), msg);
    else
        Exiv2::LogMsg::defaultHandler(level, msg);
}

static void default_log_handler(GExiv2LogLevel level, const gchar *msg) {
    Exiv2::LogMsg::defaultHandler(gexiv2_level_to_exiv2_level(level), msg);
}

static void glib_log_handler(GExiv2LogLevel level, const gchar *msg_) {
    char *msg = g_strdup(msg_);
    switch (level) {
        case GEXIV2_LOG_LEVEL_DEBUG:
            g_debug("%s", g_strchomp(msg));
        break;
        
        case GEXIV2_LOG_LEVEL_INFO:
            g_message("%s", g_strchomp(msg));
        break;
        
        case GEXIV2_LOG_LEVEL_WARN:
            g_warning("%s", g_strchomp(msg));
        break;
        
        case GEXIV2_LOG_LEVEL_ERROR:
            g_critical("%s", g_strchomp(msg));
        break;
        
        case GEXIV2_LOG_LEVEL_MUTE:
        default:
            // do nothing
        break;
    }

    g_free(msg);
}

GExiv2LogLevel gexiv2_log_get_level(void) {
    return exiv2_level_to_gexiv2_level(Exiv2::LogMsg::level());
}

void gexiv2_log_set_level(GExiv2LogLevel level) {
    Exiv2::LogMsg::setLevel(gexiv2_level_to_exiv2_level(level));
}

GExiv2LogHandler gexiv2_log_get_handler(void) {
    return (installed_handler != nullptr) ? installed_handler : default_log_handler;
}

GExiv2LogHandler gexiv2_log_get_default_handler(void) {
    return default_log_handler;
}

void gexiv2_log_set_handler(GExiv2LogHandler handler) {
    g_return_if_fail(handler != nullptr);

    installed_handler = handler;
    Exiv2::LogMsg::setHandler(log_handler_converter);
}

void gexiv2_log_use_glib_logging(void) {
    gexiv2_log_set_handler(glib_log_handler);
}

bool gexiv2_log_is_handler_installed(void) {
    return (installed_handler != nullptr);
}

G_END_DECLS

