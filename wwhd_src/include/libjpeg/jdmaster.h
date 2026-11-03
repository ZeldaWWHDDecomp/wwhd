/*
 * jdmaster.c
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2002-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains master control logic for the JPEG decompressor.
 * These routines are concerned with selecting the modules to be executed
 * and with determining the number of passes and the work to be done in each
 * pass.
 */
#pragma once
#include "libjpeg/jpeg_decoder_support.h"
#define IDCT_SCALING_SUPPORTED
#define QUANT_1PASS_SUPPORTED
#define D_MULTISCAN_FILES_SUPPORTED
struct my_decomp_master {
 jpeg_decomp_master pub;
 be<s32> pass_number;
 be<boolean> using_merged_upsample;
 gptr<jpeg_color_quantizer> quantizer_1pass,quantizer_2pass;
};
using my_master_ptr=my_decomp_master*;
WWHD_OFFSET(my_decomp_master,pass_number,0xC);
WWHD_OFFSET(my_decomp_master,using_merged_upsample,0x10);
WWHD_OFFSET(my_decomp_master,quantizer_1pass,0x14);
WWHD_OFFSET(my_decomp_master,quantizer_2pass,0x18);
WWHD_SIZE(my_decomp_master,0x1C);
static void* master_clear(void* dest,s32 value,u32 size);
static void* master_copy(void* dest,const void* source,u32 size);
#define MEMZERO(dest,size) master_clear(dest,0,size)
#define MEMCOPY(dest,source,size) master_copy(dest,source,size)
void jpeg_core_output_dimensions(j_decompress_ptr cinfo);
void jinit_1pass_quantizer(j_decompress_ptr cinfo);
void jinit_2pass_quantizer(j_decompress_ptr cinfo);
void jinit_color_deconverter(j_decompress_ptr cinfo);
void jinit_inverse_dct(j_decompress_ptr cinfo);
void jinit_arith_decoder(j_decompress_ptr cinfo);
void jinit_huff_decoder(j_decompress_ptr cinfo);
void jinit_d_coef_controller(j_decompress_ptr cinfo,boolean need_full_buffer);
void jinit_d_main_controller(j_decompress_ptr cinfo,boolean need_full_buffer);
s32 jdiv_round_up(s32 a,s32 b);
void jinit_merged_upsampler(j_decompress_ptr cinfo);
void jinit_upsampler(j_decompress_ptr cinfo);
void jinit_d_post_controller(j_decompress_ptr cinfo,boolean need_full_buffer);
static boolean use_merged_upsample (j_decompress_ptr cinfo);
void jpeg_calc_output_dimensions (j_decompress_ptr cinfo);
static void prepare_range_limit_table (j_decompress_ptr cinfo);
static void master_selection (j_decompress_ptr cinfo);
static void prepare_for_output_pass (j_decompress_ptr cinfo);
static void finish_output_pass (j_decompress_ptr cinfo);
void jinit_master_decompress (j_decompress_ptr cinfo);
