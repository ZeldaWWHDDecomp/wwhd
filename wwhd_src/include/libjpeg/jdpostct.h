/*
 * jdpostct.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains the decompression postprocessing controller.
 * This controller manages the upsampling, color conversion, and color
 * quantization/reduction steps; specifically, it controls the buffering
 * between upsample/color conversion and color quantization/reduction.
 *
 * If no color quantization/reduction is required, then this module has no
 * work to do, and it just hands off to the upsample/color conversion code.
 * An integrated upsample/convert/quantize process would replace this module
 * entirely.
 */
#pragma once
#include "libjpeg/jpeg_decoder_support.h"
struct my_post_controller {
 jpeg_d_post_controller pub;
 gptr<jvirt_sarray_control> whole_image;
 gptr<gptr<be<JSAMPLE>>> buffer;
 be<JDIMENSION> strip_height,starting_row,next_row;
};
using post_row_counter=be<JDIMENSION>;
using my_post_ptr=my_post_controller*;
WWHD_OFFSET(my_post_controller,whole_image,8);
WWHD_OFFSET(my_post_controller,buffer,0xC);
WWHD_OFFSET(my_post_controller,strip_height,0x10);
WWHD_OFFSET(my_post_controller,starting_row,0x14);
WWHD_OFFSET(my_post_controller,next_row,0x18);
WWHD_SIZE(my_post_controller,0x1C);
static s32 round_post_rows(s32 a,s32 b);
static void copy_post_rows(JSAMPARRAY input_array,s32 source_row,JSAMPARRAY output_array,s32 dest_row,s32 num_rows,JDIMENSION num_cols);
static void post_process_1pass (j_decompress_ptr cinfo,
		    JSAMPIMAGE input_buf, be<JDIMENSION>* in_row_group_ctr,
		    JDIMENSION in_row_groups_avail,
		    JSAMPARRAY output_buf, be<JDIMENSION>* out_row_ctr,
		    JDIMENSION out_rows_avail);
static void post_process_prepass (j_decompress_ptr cinfo,
		      JSAMPIMAGE input_buf, be<JDIMENSION>* in_row_group_ctr,
		      JDIMENSION in_row_groups_avail,
		      JSAMPARRAY output_buf, be<JDIMENSION>* out_row_ctr,
		      JDIMENSION out_rows_avail);
static void post_process_2pass (j_decompress_ptr cinfo,
		    JSAMPIMAGE input_buf, be<JDIMENSION>* in_row_group_ctr,
		    JDIMENSION in_row_groups_avail,
		    JSAMPARRAY output_buf, be<JDIMENSION>* out_row_ctr,
		    JDIMENSION out_rows_avail);
static void start_pass_dpost (j_decompress_ptr cinfo, J_BUF_MODE pass_mode);
void jinit_d_post_controller (j_decompress_ptr cinfo, boolean need_full_buffer);
