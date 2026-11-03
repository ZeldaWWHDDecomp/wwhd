/* Named cross-module bindings for the JPEG color/quantizer/entropy unit.
 * Decompiled WWHD build.
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modified 2002-2010 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#include "libjpeg/jpeglib.h"

// RPX veneers 028FEAD0/028FEAC0 resolve to coreinit memset/memcpy imports.
/* WWHD glue
ext 028F5914 void huff_clear_words(void* target, u32 words)
ext C000A858 void* arith_clear(void* target, s32 value, u32 bytes)
ext 0284E0C0 void jzero_far(void* target, u32 bytes)
ext 02866A9C void jpeg_fdct_islow(be<s32>* data, JSAMPARRAY sample_data, u32 start_col) callback
ext 0286689C void jpeg_fdct_ifast(be<s32>* data, JSAMPARRAY sample_data, u32 start_col) callback
ext C000A858 void* huff_clear(void* target, s32 value, u32 bytes)
ext C000A848 void* huff_copy(void* target, const void* source, u32 bytes)
ext 02850620 JHUFF_TBL* jpeg_alloc_huff_table(j_common_ptr cinfo)
ext 0284DFDC s32 jround_up(s32 a, s32 b)
ext 0284E048 void jcopy_sample_rows(JSAMPARRAY input_array, s32 source_row, JSAMPARRAY output_array, s32 dest_row, s32 num_rows, u32 num_cols)
*/
