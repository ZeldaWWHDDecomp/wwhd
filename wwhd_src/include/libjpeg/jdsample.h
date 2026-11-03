/*
 * jdsample.c
 *
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * Modified 2002-2008 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains upsampling routines.
 *
 * Upsampling input data is counted in "row groups".  A row group
 * is defined to be (v_samp_factor * DCT_v_scaled_size / min_DCT_v_scaled_size)
 * sample rows of each component.  Upsampling will normally produce
 * max_v_samp_factor pixel rows from each row group (but this could vary
 * if the upsampler is applying a scale factor of its own).
 *
 * An excellent reference for image resampling is
 *   Digital Image Warping, George Wolberg, 1990.
 *   Pub. by IEEE Computer Society Press, Los Alamitos, CA. ISBN 0-8186-8944-7.
 */
#pragma once
#include "libjpeg/jpeg_decoder_support.h"
struct my_upsampler {
 jpeg_upsampler pub;
 gptr<gptr<be<JSAMPLE>>> color_buf[10];
 jpeg_method<void(j_decompress_ptr,jpeg_component_info*,JSAMPARRAY,gptr<gptr<be<JSAMPLE>>>*)> methods[10];
 be<s32> next_row_out;
 be<JDIMENSION> rows_to_go;
 be<s32> rowgroup_height[10];
 be<u8> h_expand[10],v_expand[10];
};
using my_upsample_ptr=my_upsampler*;
WWHD_OFFSET(my_upsampler,color_buf,0xC);
WWHD_OFFSET(my_upsampler,methods,0x34);
WWHD_OFFSET(my_upsampler,next_row_out,0x5C);
WWHD_OFFSET(my_upsampler,rows_to_go,0x60);
WWHD_OFFSET(my_upsampler,rowgroup_height,0x64);
WWHD_OFFSET(my_upsampler,h_expand,0x8C);
WWHD_OFFSET(my_upsampler,v_expand,0x96);
WWHD_SIZE(my_upsampler,0xA0);
static void copy_upsample_rows(JSAMPARRAY input_array,s32 source_row,JSAMPARRAY output_array,s32 dest_row,s32 num_rows,JDIMENSION num_cols);
static s32 round_upsample_width(s32 a,s32 b);
static void start_pass_upsample (j_decompress_ptr cinfo);
static void sep_upsample (j_decompress_ptr cinfo,
	      JSAMPIMAGE input_buf, be<JDIMENSION>* in_row_group_ctr,
	      JDIMENSION in_row_groups_avail,
	      JSAMPARRAY output_buf, be<JDIMENSION>* out_row_ctr,
	      JDIMENSION out_rows_avail);
static void fullsize_upsample (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		   JSAMPARRAY input_data, gptr<gptr<be<JSAMPLE>>>* output_data_ptr);
static void noop_upsample (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JSAMPARRAY input_data, gptr<gptr<be<JSAMPLE>>>* output_data_ptr);
static void int_upsample (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	      JSAMPARRAY input_data, gptr<gptr<be<JSAMPLE>>>* output_data_ptr);
static void h2v1_upsample (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JSAMPARRAY input_data, gptr<gptr<be<JSAMPLE>>>* output_data_ptr);
static void h2v2_upsample (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JSAMPARRAY input_data, gptr<gptr<be<JSAMPLE>>>* output_data_ptr);
void jinit_upsampler (j_decompress_ptr cinfo);
