/* jcprepct.h (decompiled WWHD declarations)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c preprocessing controller to guest storage and callbacks.
 */
#pragma once
#include "libjpeg/jpeglib.h"
struct my_prep_controller : jpeg_c_prep_controller {
    gptr<gptr<be<u8>>> color_buf[MAX_COMPONENTS];
    be<u32> rows_to_go;
    be<s32> next_buf_row;
    be<s32> this_row_group;
    be<s32> next_buf_stop;
};
WWHD_OFFSET(my_prep_controller, color_buf, 0x8);
WWHD_OFFSET(my_prep_controller, rows_to_go, 0x30);
WWHD_OFFSET(my_prep_controller, next_buf_row, 0x34);
WWHD_OFFSET(my_prep_controller, this_row_group, 0x38);
WWHD_OFFSET(my_prep_controller, next_buf_stop, 0x3C);
WWHD_SIZE(my_prep_controller, 0x40);
static void start_pass_prep(j_compress_ptr, s32);
static void expand_bottom_edge(JSAMPARRAY, u32, s32, s32);
static void pre_process_data(j_compress_ptr, JSAMPARRAY, be<u32> *, u32, JSAMPIMAGE, be<u32> *,
                             u32);
static void pre_process_context(j_compress_ptr, JSAMPARRAY, be<u32> *, u32, JSAMPIMAGE, be<u32> *,
                                u32);
static void create_context_buffer(j_compress_ptr);
