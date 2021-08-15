/*
 * gexiv2-startup.cpp
 *
 * Author(s)
 * 	Clint Rogers <clinton@yorba.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <exiv2/exiv2.hpp>
#include "gexiv2-startup.h"
#include "gexiv2-version.h"

gboolean gexiv2_initialize(void) {
#if EXIV2_TEST_VERSION(0, 27, 4)
    Exiv2::enableBMFF();
#endif

    return Exiv2::XmpParser::initialize();
}

gint gexiv2_get_version(void) {
    return GEXIV2_MAJOR_VERSION * 100 * 100 +
           GEXIV2_MINOR_VERSION * 100 +
           GEXIV2_MICRO_VERSION;
}
