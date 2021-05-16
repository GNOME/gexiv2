#include "gexiv2-error.h"

GQuark gexiv2_error_quark() {
    static GQuark quark = 0;

    if (quark == 0) {
        quark = g_quark_from_static_string("exiv2-error");
    }

    return quark;
}
