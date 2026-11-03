/* jdcolor.cpp private layout (decompiled WWHD build)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpegint_color.h"
struct my_color_deconverter {
    jpeg_color_deconverter pub;
    gptr<be<s32>> Cr_r_tab;
    gptr<be<s32>> Cb_b_tab;
    gptr<be<s32>> Cr_g_tab;
    gptr<be<s32>> Cb_g_tab;
};
WWHD_OFFSET(my_color_deconverter, Cr_r_tab, 8);
WWHD_OFFSET(my_color_deconverter, Cb_b_tab, 0xC);
WWHD_OFFSET(my_color_deconverter, Cr_g_tab, 0x10);
WWHD_OFFSET(my_color_deconverter, Cb_g_tab, 0x14);
WWHD_SIZE(my_color_deconverter, 0x18);
void jinit_color_deconverter(j_decompress_ptr cinfo);
