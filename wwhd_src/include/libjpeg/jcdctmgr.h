/* jcdctmgr.cpp private layout (decompiled WWHD build)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpeglib.h"
struct fdct_controller {
    jpeg_forward_dct pub;
    jpeg_method<void(be<s32>*, JSAMPARRAY, u32)> do_dct[10];
    gptr<be<s32>> divisors[4];
};
WWHD_OFFSET(fdct_controller, do_dct, 0x2C);
WWHD_OFFSET(fdct_controller, divisors, 0x54);
WWHD_SIZE(fdct_controller, 0x64);
void jpeg_fdct_islow(be<s32>* data, JSAMPARRAY sample_data, u32 start_col);
void jpeg_fdct_ifast(be<s32>* data, JSAMPARRAY sample_data, u32 start_col);
void jinit_forward_dct(j_compress_ptr cinfo);

struct fdct_workspace { be<s32> data[64]; };
