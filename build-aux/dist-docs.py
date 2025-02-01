#!/usr/bin/env python3

import os
import shutil

references = {
        'docs/reference/GExiv2': 'docs/gexiv2-0.16'
    }

sourceroot = os.environ.get('MESON_SOURCE_ROOT')
buildroot = os.environ.get('MESON_BUILD_ROOT')
distroot = os.environ.get('MESON_DIST_ROOT')

for src,dest in references.items():
    src_path = os.path.join(buildroot, src)
    if os.path.isdir(src_path):
        dst_path = os.path.join(distroot, dest)
        shutil.copytree(src_path, dst_path)

