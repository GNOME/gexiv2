#!/usr/bin/env python3

import sys
import re

# From https://stackoverflow.com/questions/1175208/elegant-python-function-to-convert-camelcase-to-snake-case
def camel_to_snake(name):
  name = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
  return re.sub('([a-z0-9])([A-Z])', r'\1_\2', name).upper()


HEAD = """
#ifndef GEXIV2ERROR_H
#define GEXIV2ERROR_H

#include <glib.h>

G_BEGIN_DECLS

GQuark gexiv2_error_quark(void) G_GNUC_CONST;

#define GEXIV2_ERROR (gexiv2_error_quark())

typedef enum _GExiv2Error {
"""

TAIL = """
} GExiv2Error;

G_END_DECLS

#endif // GEXIV2ERROR_H
"""

with open(sys.argv[1]) as f:
    state = 0
    print(HEAD)
    for line in f:
        if state == 0:
            if "enum ErrorCode" in line:
                state = 1
            continue

        if state == 1:
            if "//" in line:
                continue
            if "}" in line:
                break
            else:
                line = line.strip().replace(',', '')
                parts = line.split("=", 2)
                if len(parts) == 2:
                    value = int(parts[1].strip())
                else:
                    value = value + 1
                line = parts[0].strip().replace(",", "")
                snake = camel_to_snake(line[3:])
                print(f"    GEXIV2_ERROR_{snake} = {value},")
    print(TAIL)
