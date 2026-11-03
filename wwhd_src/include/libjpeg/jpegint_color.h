/* jpegint.h color interfaces (decompiled WWHD build)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 1997-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c method tables use 32-bit big-endian guest entries.
 */
#pragma once
#include "libjpeg/jpeglib.h"
struct jpeg_color_deconverter {
 jpeg_method<void(j_decompress_ptr)> start_pass;
 jpeg_method<void(j_decompress_ptr,JSAMPIMAGE,JDIMENSION,JSAMPARRAY,s32)> color_convert;
};
struct jpeg_color_quantizer {
 jpeg_method<void(j_decompress_ptr,boolean)> start_pass;
 jpeg_method<void(j_decompress_ptr,JSAMPARRAY,JSAMPARRAY,s32)> color_quantize;
 jpeg_method<void(j_decompress_ptr)> finish_pass,new_color_map;
};
WWHD_SIZE(jpeg_color_deconverter,8);
WWHD_OFFSET(jpeg_color_deconverter,color_convert,4);
WWHD_SIZE(jpeg_color_quantizer,0x10);
WWHD_OFFSET(jpeg_color_quantizer,color_quantize,4);
WWHD_OFFSET(jpeg_color_quantizer,finish_pass,8);
WWHD_OFFSET(jpeg_color_quantizer,new_color_map,0xC);
