/* jcsample.h (decompiled WWHD declarations)
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c downsampling to guest storage and 32-bit Cafe arithmetic.
 */
#pragma once
#include "libjpeg/jpeglib.h"
struct my_downsampler : jpeg_downsampler {
    jpeg_method<void(j_compress_ptr, jpeg_component_info *, JSAMPARRAY, JSAMPARRAY)>
        methods[MAX_COMPONENTS];
    be<s32> rowgroup_height[MAX_COMPONENTS];
    be<u8> h_expand[MAX_COMPONENTS];
    be<u8> v_expand[MAX_COMPONENTS];
};
WWHD_OFFSET(my_downsampler, methods, 0xC);
WWHD_OFFSET(my_downsampler, rowgroup_height, 0x34);
WWHD_OFFSET(my_downsampler, h_expand, 0x5C);
WWHD_OFFSET(my_downsampler, v_expand, 0x66);
WWHD_SIZE(my_downsampler, 0x70);
static void start_pass_downsample(j_compress_ptr);
static void expand_right_edge(JSAMPARRAY, s32, u32, u32);
static void sep_downsample(j_compress_ptr, JSAMPIMAGE, u32, JSAMPIMAGE, u32);
static void int_downsample(j_compress_ptr, jpeg_component_info *, JSAMPARRAY, JSAMPARRAY);
static void fullsize_downsample(j_compress_ptr, jpeg_component_info *, JSAMPARRAY, JSAMPARRAY);
static void h2v1_downsample(j_compress_ptr, jpeg_component_info *, JSAMPARRAY, JSAMPARRAY);
static void h2v2_downsample(j_compress_ptr, jpeg_component_info *, JSAMPARRAY, JSAMPARRAY);
static void h2v2_smooth_downsample(j_compress_ptr, jpeg_component_info *, JSAMPARRAY, JSAMPARRAY);
static void fullsize_smooth_downsample(j_compress_ptr, jpeg_component_info *, JSAMPARRAY,
                                       JSAMPARRAY);
