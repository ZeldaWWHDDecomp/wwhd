/* jidctint.c (decompiled WWHD declarations)
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modification developed 2002-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpeg_dct_types.h"
void jpeg_idct_islow (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_7x7 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_6x6 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_5x5 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_4x4 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_3x3 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_2x2 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_1x1 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_9x9 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_10x10 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_11x11 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_12x12 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_13x13 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_14x14 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_15x15 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_16x16 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_16x8 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_14x7 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_12x6 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_10x5 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_8x4 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_6x3 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_4x2 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_2x1 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_8x16 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_7x14 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_6x12 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_5x10 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_4x8 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_3x6 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_2x4 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
void jpeg_idct_1x2 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col);
