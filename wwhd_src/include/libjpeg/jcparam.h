/* jcparam.h (decompiled WWHD declarations)
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modified 2003-2008 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c parameter selection to guest storage and callbacks.
 */
#pragma once
#include "libjpeg/jpeglib.h"
void jpeg_add_quant_table(j_compress_ptr, s32, const be<u32> *, s32, boolean);
void jpeg_set_linear_quality(j_compress_ptr, s32, boolean);
s32 jpeg_quality_scaling(s32);
void jpeg_set_quality(j_compress_ptr, s32, boolean);
static void add_huff_table(j_compress_ptr, gptr<JHUFF_TBL> *, const be<u8> *, const be<u8> *);
static void std_huff_tables(j_compress_ptr);
void jpeg_set_colorspace(j_compress_ptr, s32);
void jpeg_default_colorspace(j_compress_ptr);
