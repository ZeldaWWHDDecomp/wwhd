/*
 * jdmerge.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains code for merged upsampling/color conversion.
 *
 * This file combines functions from jdsample.c and jdcolor.c;
 * read those files first to understand what's going on.
 *
 * When the chroma components are to be upsampled by simple replication
 * (ie, box filtering), we can save some work in color conversion by
 * calculating all the output pixels corresponding to a pair of chroma
 * samples at one time.  In the conversion equations
 *	R = Y           + K1 * Cr
 *	G = Y + K2 * Cb + K3 * Cr
 *	B = Y + K4 * Cb
 * only the Y term varies among the group of pixels corresponding to a pair
 * of chroma samples, so the rest of the terms can be calculated just once.
 * At typical sampling ratios, this eliminates half or three-quarters of the
 * multiplications needed for color conversion.
 *
 * This file currently provides implementations for the following cases:
 *	YCbCr => RGB color conversion only.
 *	Sampling ratios of 2h1v or 2h2v.
 *	No scaling needed at upsample time.
 *	Corner-aligned (non-CCIR601) sampling alignment.
 * Other special cases could be added, but in most applications these are
 * the only common cases.  (For uncommon cases we fall back on the more
 * general code in jdsample.c and jdcolor.c.)
 */
#pragma once
#include "libjpeg/jpeg_decoder_support.h"
struct my_merged_upsampler {
 jpeg_upsampler pub;
 jpeg_method<void(j_decompress_ptr,JSAMPIMAGE,JDIMENSION,JSAMPARRAY)> upmethod;
 gptr<be<s32>> Cr_r_tab,Cb_b_tab,Cr_g_tab,Cb_g_tab;
 gptr<be<JSAMPLE>> spare_row;
 be<boolean> spare_full;
 be<JDIMENSION> out_row_width,rows_to_go;
};
struct merged_output_rows { gptr<be<JSAMPLE>> rows[2]; };
WWHD_SIZE(merged_output_rows,8);
using merged_row_pointer=gptr<be<JSAMPLE>>;
using my_merged_upsample_ptr=my_merged_upsampler*;
WWHD_OFFSET(my_merged_upsampler,upmethod,0xC);
WWHD_OFFSET(my_merged_upsampler,Cr_r_tab,0x10);
WWHD_OFFSET(my_merged_upsampler,Cb_b_tab,0x14);
WWHD_OFFSET(my_merged_upsampler,Cr_g_tab,0x18);
WWHD_OFFSET(my_merged_upsampler,Cb_g_tab,0x1C);
WWHD_OFFSET(my_merged_upsampler,spare_row,0x20);
WWHD_OFFSET(my_merged_upsampler,spare_full,0x24);
WWHD_OFFSET(my_merged_upsampler,out_row_width,0x28);
WWHD_OFFSET(my_merged_upsampler,rows_to_go,0x2C);
WWHD_SIZE(my_merged_upsampler,0x30);
#define SCALEBITS 16
#define ONE_HALF ((INT32) 1 << (SCALEBITS-1))
#define FIX(x) ((INT32) ((x) * (1L<<SCALEBITS) + 0.5))
#define SHIFT_TEMPS
#define RIGHT_SHIFT(x,n) ((x) >> (n))
static void copy_merged_rows(JSAMPARRAY input_array,s32 source_row,JSAMPARRAY output_array,s32 dest_row,s32 num_rows,JDIMENSION num_cols);
static void build_ycc_rgb_table (j_decompress_ptr cinfo);
static void start_pass_merged_upsample (j_decompress_ptr cinfo);
static void merged_2v_upsample (j_decompress_ptr cinfo,
		    JSAMPIMAGE input_buf, be<JDIMENSION>* in_row_group_ctr,
		    JDIMENSION in_row_groups_avail,
		    JSAMPARRAY output_buf, be<JDIMENSION>* out_row_ctr,
		    JDIMENSION out_rows_avail);
static void merged_1v_upsample (j_decompress_ptr cinfo,
		    JSAMPIMAGE input_buf, be<JDIMENSION>* in_row_group_ctr,
		    JDIMENSION in_row_groups_avail,
		    JSAMPARRAY output_buf, be<JDIMENSION>* out_row_ctr,
		    JDIMENSION out_rows_avail);
static void h2v1_merged_upsample (j_decompress_ptr cinfo,
		      JSAMPIMAGE input_buf, JDIMENSION in_row_group_ctr,
		      JSAMPARRAY output_buf);
static void h2v2_merged_upsample (j_decompress_ptr cinfo,
		      JSAMPIMAGE input_buf, JDIMENSION in_row_group_ctr,
		      JSAMPARRAY output_buf);
void jinit_merged_upsampler (j_decompress_ptr cinfo);
