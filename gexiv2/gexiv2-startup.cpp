/*
 * gexiv2-startup.cpp
 *
 * Author(s)
 * 	Clint Rogers <clinton@yorba.org>
 *
 * This is free software. See COPYING for details.
 */
 
#include <exiv2/xmp.hpp>
#include "gexiv2-startup.h"

gboolean gexiv2_initialize(void) {
    return Exiv2::XmpParser::initialize();
}
