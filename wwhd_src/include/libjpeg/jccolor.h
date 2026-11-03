/* jccolor.cpp private layout (decompiled WWHD build)
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpeglib.h"
struct my_color_converter {
    jpeg_color_converter pub;
    gptr<be<s32>> rgb_ycc_tab;
};
WWHD_OFFSET(my_color_converter, rgb_ycc_tab, 8);
WWHD_SIZE(my_color_converter, 0xC);
void jinit_color_converter(j_compress_ptr cinfo);
