/* jpegint.h decoder interfaces (decompiled WWHD build)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 1997-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c public method tables: big-endian guest storage.
 */
#pragma once
#include "libjpeg/jpeglib.h"
struct jpeg_inverse_dct {
    jpeg_method<void(j_decompress_ptr)> start_pass;
    jpeg_method<void(j_decompress_ptr,jpeg_component_info*,be<JCOEF>*,JSAMPARRAY,JDIMENSION)> inverse_DCT[10];
};
struct jpeg_entropy_decoder {
    jpeg_method<void(j_decompress_ptr)> start_pass;
    jpeg_method<boolean(j_decompress_ptr,JBLOCKARRAY)> decode_mcu;
};
struct jpeg_d_post_controller {
    jpeg_method<void(j_decompress_ptr,s32)> start_pass;
    jpeg_method<void(j_decompress_ptr,JSAMPIMAGE,be<JDIMENSION>*,JDIMENSION,JSAMPARRAY,be<JDIMENSION>*,JDIMENSION)> post_process_data;
};
struct jpeg_upsampler {
    jpeg_method<void(j_decompress_ptr)> start_pass;
    jpeg_method<void(j_decompress_ptr,JSAMPIMAGE,be<JDIMENSION>*,JDIMENSION,JSAMPARRAY,be<JDIMENSION>*,JDIMENSION)> upsample;
    be<boolean> need_context_rows;
};
WWHD_SIZE(jpeg_inverse_dct,0x2C);
WWHD_SIZE(jpeg_entropy_decoder,8);
WWHD_OFFSET(jpeg_d_post_controller,post_process_data,4);
WWHD_OFFSET(jpeg_upsampler,need_context_rows,8);
