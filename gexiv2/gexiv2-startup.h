/*
 * gexiv2-startup.h
 *
 * Author(s)
 *  Clint Rogers <clinton@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef GEXIV2_STARTUP_H
#define GEXIV2_STARTUP_H

#include <glib-object.h>

/**
 * SECTION:gexiv2-startup
 * @title: Library initialisation
 * @short_description:Functions that need to be called to set up the GExiv2 library.
 */

G_BEGIN_DECLS

/**
 * gexiv2_initialize:
 *
 * Initializes GExiv2.
 *
 * GExiv2 requires initialization before its methods are used.  In particular, this call must be
 * made in a thread-safe fashion.  Best practice is to call from the application's main thread and
 * not to use any Gexiv2 code until it has returned.
 *
 * Returns: %TRUE if initialized.  If %FALSE, GExiv2 should not be used (unable to initialize
 * properly).
 */
gboolean gexiv2_initialize(void);

/**
 * gexiv2_shutdown:
 *
 * Shutdown GExiv2.
 *
 * Shutdown GExiv2. Should be called in the same thread as gexiv2_initialize().
 *
 * Since: 0.16.0
 */
void gexiv2_shutdown(void);

G_END_DECLS
#endif
