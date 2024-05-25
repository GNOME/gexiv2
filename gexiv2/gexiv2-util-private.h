// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: Copyright (C) 2024 Jens Georg
#pragma once

#include <exception>
#include <exiv2/exiv2.hpp>
#include <gio/gio.h>

inline void operator<<(GError** error, Exiv2::Error& ex) {
    g_set_error_literal(error, g_quark_from_string("GExiv2"), static_cast<int>(ex.code()), ex.what());
}

inline void operator<<(GError** error, std::exception& ex) {
    g_set_error_literal(error, g_quark_from_string("GExiv2"), 404, ex.what());
}
