/* jpeglib.h (decompiled WWHD build)
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modified 2002-2010 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c declarations: explicit big-endian guest storage and Cafe
 * allocator context.
 */
#pragma once
#include "wwhd.h"
using JDIMENSION = u32;
using boolean = s32;
using JSAMPLE = u8;
using JOCTET = u8;
using JCOEF = s16;
using JSAMPROW = be<JSAMPLE> *;
using JSAMPARRAY = gptr<be<JSAMPLE>> *;
using JSAMPIMAGE = gptr<gptr<be<JSAMPLE>>> *;
using JBLOCK = be<JCOEF>[64];
using JBLOCKROW = JBLOCK *;
using JBLOCKARRAY = gptr<JBLOCK> *;
using JBLOCKIMAGE = gptr<gptr<JBLOCK>> *;
struct jpeg_common_struct;
struct jpeg_compress_struct;
struct jpeg_decompress_struct;
using j_common_ptr = jpeg_common_struct *;
using j_compress_ptr = jpeg_compress_struct *;
using j_decompress_ptr = jpeg_decompress_struct *;
template <class Signature> struct jpeg_callback;
template <class R, class... A> struct jpeg_callback<R(A...)> {
  u32 address;
  R operator()(A... args) const { return gabi::call_ptr<R>(address, args...); }
};
template <class Signature> struct jpeg_method;
template <class R, class... A> struct jpeg_method<R(A...)> {
  be<u32> entry;
  jpeg_callback<R(A...)> load() const { return {entry}; }
  R operator()(A... args) const { return load()(args...); }
  const jpeg_method &operator*() const { return *this; }
  jpeg_method &operator=(std::nullptr_t) {
    entry = 0;
    return *this;
  }
  jpeg_method &operator=(const jpeg_method &rhs) {
    entry = (u32)rhs.entry;
    return *this;
  }
  template <class F> jpeg_method &operator=(F fn) {
    entry = fn ? gabi::guest_fn(reinterpret_cast<const void *>(fn)) : 0;
    return *this;
  }
};
enum {
  DCTSIZE = 8,
  DCTSIZE2 = 64,
  NUM_QUANT_TBLS = 4,
  NUM_HUFF_TBLS = 4,
  NUM_ARITH_TBLS = 16,
  MAX_COMPS_IN_SCAN = 4,
  MAX_COMPONENTS = 10,
  C_MAX_BLOCKS_IN_MCU = 10,
  D_MAX_BLOCKS_IN_MCU = 10,
  JPEG_LIB_VERSION = 80,
  JPOOL_PERMANENT = 0,
  JPOOL_IMAGE = 1,
  JPOOL_NUMPOOLS = 2,
  JPEG_SUSPENDED = 0,
  JPEG_HEADER_OK = 1,
  JPEG_HEADER_TABLES_ONLY = 2,
  JPEG_REACHED_SOS = 1,
  JPEG_REACHED_EOI = 2,
  JPEG_ROW_COMPLETED = 3,
  JPEG_SCAN_COMPLETED = 4
};
enum J_COLOR_SPACE {
  JCS_UNKNOWN,
  JCS_GRAYSCALE,
  JCS_RGB,
  JCS_YCbCr,
  JCS_CMYK,
  JCS_YCCK
};
enum J_DCT_METHOD { JDCT_ISLOW, JDCT_IFAST, JDCT_FLOAT };
enum J_DITHER_MODE { JDITHER_NONE, JDITHER_ORDERED, JDITHER_FS };
enum J_BUF_MODE {
  JBUF_PASS_THRU,
  JBUF_SAVE_SOURCE,
  JBUF_CRANK_DEST,
  JBUF_SAVE_AND_PASS
};
enum {
  CSTATE_START = 100,
  CSTATE_SCANNING,
  CSTATE_RAW_OK,
  CSTATE_WRCOEFS,
  DSTATE_START = 200,
  DSTATE_INHEADER,
  DSTATE_READY,
  DSTATE_PRELOAD,
  DSTATE_PRESCAN,
  DSTATE_SCANNING,
  DSTATE_RAW_OK,
  DSTATE_BUFIMAGE,
  DSTATE_BUFPOST,
  DSTATE_RDCOEFS,
  DSTATE_STOPPING
};
struct jpeg_error_mgr;
struct jpeg_memory_mgr;
struct jpeg_progress_mgr;
struct jpeg_destination_mgr;
struct jpeg_source_mgr;
struct jpeg_component_info;
struct JQUANT_TBL;
struct JHUFF_TBL;
struct jpeg_scan_info;
struct jpeg_marker_struct;
struct jvirt_sarray_control;
struct jvirt_barray_control;
struct jpeg_comp_master;
struct jpeg_c_main_controller;
struct jpeg_c_prep_controller;
struct jpeg_c_coef_controller;
struct jpeg_marker_writer;
struct jpeg_color_converter;
struct jpeg_downsampler;
struct jpeg_forward_dct;
struct jpeg_entropy_encoder;
struct jpeg_decomp_master;
struct jpeg_d_main_controller;
struct jpeg_d_coef_controller;
struct jpeg_d_post_controller;
struct jpeg_input_controller;
struct jpeg_marker_reader;
struct jpeg_entropy_decoder;
struct jpeg_inverse_dct;
struct jpeg_upsampler;
struct jpeg_color_deconverter;
struct jpeg_color_quantizer;
struct jpeg_common_struct {
  /* 0x000 */ gptr<jpeg_error_mgr> err;
  /* 0x004 */ gptr<jpeg_memory_mgr> mem;
  /* 0x008 */ gptr<jpeg_progress_mgr> progress;
  /* 0x00C */ gptr<void> client_data;
  /* 0x010 */ be<s32> is_decompressor;
  /* 0x014 */ be<s32> global_state;
  /* 0x018 */ gptr<void> allocation_heap;
};
WWHD_OFFSET(jpeg_common_struct, err, 0x0);
WWHD_OFFSET(jpeg_common_struct, mem, 0x4);
WWHD_OFFSET(jpeg_common_struct, progress, 0x8);
WWHD_OFFSET(jpeg_common_struct, client_data, 0xC);
WWHD_OFFSET(jpeg_common_struct, is_decompressor, 0x10);
WWHD_OFFSET(jpeg_common_struct, global_state, 0x14);
WWHD_OFFSET(jpeg_common_struct, allocation_heap, 0x18);
WWHD_SIZE(jpeg_common_struct, 0x1C);
// HD: allocation_heap is preserved by creation and used by the Cafe allocation
// adapters.
struct JQUANT_TBL {
  /* 0x000 */ be<u16> quantval[64];
  /* 0x080 */ be<s32> sent_table;
};
WWHD_OFFSET(JQUANT_TBL, quantval, 0x0);
WWHD_OFFSET(JQUANT_TBL, sent_table, 0x80);
WWHD_SIZE(JQUANT_TBL, 0x84);
struct JHUFF_TBL {
  /* 0x000 */ be<u8> bits[17];
  /* 0x011 */ be<u8> huffval[256];
  u8 _alignment[3];
  /* 0x114 */ be<s32> sent_table;
};
WWHD_OFFSET(JHUFF_TBL, bits, 0x0);
WWHD_OFFSET(JHUFF_TBL, huffval, 0x11);
WWHD_OFFSET(JHUFF_TBL, sent_table, 0x114);
WWHD_SIZE(JHUFF_TBL, 0x118);
struct jpeg_component_info {
  /* 0x000 */ be<s32> component_id;
  /* 0x004 */ be<s32> component_index;
  /* 0x008 */ be<s32> h_samp_factor;
  /* 0x00C */ be<s32> v_samp_factor;
  /* 0x010 */ be<s32> quant_tbl_no;
  /* 0x014 */ be<s32> dc_tbl_no;
  /* 0x018 */ be<s32> ac_tbl_no;
  /* 0x01C */ be<u32> width_in_blocks;
  /* 0x020 */ be<u32> height_in_blocks;
  /* 0x024 */ be<s32> DCT_h_scaled_size;
  /* 0x028 */ be<s32> DCT_v_scaled_size;
  /* 0x02C */ be<u32> downsampled_width;
  /* 0x030 */ be<u32> downsampled_height;
  /* 0x034 */ be<s32> component_needed;
  /* 0x038 */ be<s32> MCU_width;
  /* 0x03C */ be<s32> MCU_height;
  /* 0x040 */ be<s32> MCU_blocks;
  /* 0x044 */ be<s32> MCU_sample_width;
  /* 0x048 */ be<s32> last_col_width;
  /* 0x04C */ be<s32> last_row_height;
  /* 0x050 */ gptr<JQUANT_TBL> quant_table;
  /* 0x054 */ gptr<void> dct_table;
};
WWHD_OFFSET(jpeg_component_info, component_id, 0x0);
WWHD_OFFSET(jpeg_component_info, component_index, 0x4);
WWHD_OFFSET(jpeg_component_info, h_samp_factor, 0x8);
WWHD_OFFSET(jpeg_component_info, v_samp_factor, 0xC);
WWHD_OFFSET(jpeg_component_info, quant_tbl_no, 0x10);
WWHD_OFFSET(jpeg_component_info, dc_tbl_no, 0x14);
WWHD_OFFSET(jpeg_component_info, ac_tbl_no, 0x18);
WWHD_OFFSET(jpeg_component_info, width_in_blocks, 0x1C);
WWHD_OFFSET(jpeg_component_info, height_in_blocks, 0x20);
WWHD_OFFSET(jpeg_component_info, DCT_h_scaled_size, 0x24);
WWHD_OFFSET(jpeg_component_info, DCT_v_scaled_size, 0x28);
WWHD_OFFSET(jpeg_component_info, downsampled_width, 0x2C);
WWHD_OFFSET(jpeg_component_info, downsampled_height, 0x30);
WWHD_OFFSET(jpeg_component_info, component_needed, 0x34);
WWHD_OFFSET(jpeg_component_info, MCU_width, 0x38);
WWHD_OFFSET(jpeg_component_info, MCU_height, 0x3C);
WWHD_OFFSET(jpeg_component_info, MCU_blocks, 0x40);
WWHD_OFFSET(jpeg_component_info, MCU_sample_width, 0x44);
WWHD_OFFSET(jpeg_component_info, last_col_width, 0x48);
WWHD_OFFSET(jpeg_component_info, last_row_height, 0x4C);
WWHD_OFFSET(jpeg_component_info, quant_table, 0x50);
WWHD_OFFSET(jpeg_component_info, dct_table, 0x54);
WWHD_SIZE(jpeg_component_info, 0x58);
struct jpeg_scan_info {
  /* 0x000 */ be<s32> comps_in_scan;
  /* 0x004 */ be<s32> component_index[4];
  /* 0x014 */ be<s32> Ss;
  /* 0x018 */ be<s32> Se;
  /* 0x01C */ be<s32> Ah;
  /* 0x020 */ be<s32> Al;
};
WWHD_OFFSET(jpeg_scan_info, comps_in_scan, 0x0);
WWHD_OFFSET(jpeg_scan_info, component_index, 0x4);
WWHD_OFFSET(jpeg_scan_info, Ss, 0x14);
WWHD_OFFSET(jpeg_scan_info, Se, 0x18);
WWHD_OFFSET(jpeg_scan_info, Ah, 0x1C);
WWHD_OFFSET(jpeg_scan_info, Al, 0x20);
WWHD_SIZE(jpeg_scan_info, 0x24);
struct jpeg_compress_struct : jpeg_common_struct {
  /* 0x01C */ gptr<jpeg_destination_mgr> dest;
  /* 0x020 */ be<u32> image_width;
  /* 0x024 */ be<u32> image_height;
  /* 0x028 */ be<s32> input_components;
  /* 0x02C */ be<s32> in_color_space;
  /* 0x030 */ be<f64> input_gamma;
  /* 0x038 */ be<u32> scale_num;
  /* 0x03C */ be<u32> scale_denom;
  /* 0x040 */ be<u32> jpeg_width;
  /* 0x044 */ be<u32> jpeg_height;
  /* 0x048 */ be<s32> data_precision;
  /* 0x04C */ be<s32> num_components;
  /* 0x050 */ be<s32> jpeg_color_space;
  /* 0x054 */ gptr<jpeg_component_info> comp_info;
  /* 0x058 */ gptr<JQUANT_TBL> quant_tbl_ptrs[4];
  /* 0x068 */ be<s32> q_scale_factor[4];
  /* 0x078 */ gptr<JHUFF_TBL> dc_huff_tbl_ptrs[4];
  /* 0x088 */ gptr<JHUFF_TBL> ac_huff_tbl_ptrs[4];
  /* 0x098 */ be<u8> arith_dc_L[16];
  /* 0x0A8 */ be<u8> arith_dc_U[16];
  /* 0x0B8 */ be<u8> arith_ac_K[16];
  /* 0x0C8 */ be<s32> num_scans;
  /* 0x0CC */ gptr<jpeg_scan_info> scan_info;
  /* 0x0D0 */ be<s32> raw_data_in;
  /* 0x0D4 */ be<s32> arith_code;
  /* 0x0D8 */ be<s32> optimize_coding;
  /* 0x0DC */ be<s32> CCIR601_sampling;
  /* 0x0E0 */ be<s32> do_fancy_downsampling;
  /* 0x0E4 */ be<s32> smoothing_factor;
  /* 0x0E8 */ be<s32> dct_method;
  /* 0x0EC */ be<u32> restart_interval;
  /* 0x0F0 */ be<s32> restart_in_rows;
  /* 0x0F4 */ be<s32> write_JFIF_header;
  /* 0x0F8 */ be<u8> JFIF_major_version;
  /* 0x0F9 */ be<u8> JFIF_minor_version;
  /* 0x0FA */ be<u8> density_unit;
  u8 _density_alignment[1];
  /* 0x0FC */ be<u16> X_density;
  /* 0x0FE */ be<u16> Y_density;
  /* 0x100 */ be<s32> write_Adobe_marker;
  /* 0x104 */ be<u32> next_scanline;
  /* 0x108 */ be<s32> progressive_mode;
  /* 0x10C */ be<s32> max_h_samp_factor;
  /* 0x110 */ be<s32> max_v_samp_factor;
  /* 0x114 */ be<s32> min_DCT_h_scaled_size;
  /* 0x118 */ be<s32> min_DCT_v_scaled_size;
  /* 0x11C */ be<u32> total_iMCU_rows;
  /* 0x120 */ be<s32> comps_in_scan;
  /* 0x124 */ gptr<jpeg_component_info> cur_comp_info[4];
  /* 0x134 */ be<u32> MCUs_per_row;
  /* 0x138 */ be<u32> MCU_rows_in_scan;
  /* 0x13C */ be<s32> blocks_in_MCU;
  /* 0x140 */ be<s32> MCU_membership[10];
  /* 0x168 */ be<s32> Ss;
  /* 0x16C */ be<s32> Se;
  /* 0x170 */ be<s32> Ah;
  /* 0x174 */ be<s32> Al;
  /* 0x178 */ be<s32> block_size;
  /* 0x17C */ gptr<const be<s32>> natural_order;
  /* 0x180 */ be<s32> lim_Se;
  /* 0x184 */ gptr<jpeg_comp_master> master;
  /* 0x188 */ gptr<jpeg_c_main_controller> main;
  /* 0x18C */ gptr<jpeg_c_prep_controller> prep;
  /* 0x190 */ gptr<jpeg_c_coef_controller> coef;
  /* 0x194 */ gptr<jpeg_marker_writer> marker;
  /* 0x198 */ gptr<jpeg_color_converter> cconvert;
  /* 0x19C */ gptr<jpeg_downsampler> downsample;
  /* 0x1A0 */ gptr<jpeg_forward_dct> fdct;
  /* 0x1A4 */ gptr<jpeg_entropy_encoder> entropy;
  /* 0x1A8 */ gptr<jpeg_scan_info> script_space;
  /* 0x1AC */ be<s32> script_space_size;
};
WWHD_OFFSET(jpeg_compress_struct, dest, 0x1C);
WWHD_OFFSET(jpeg_compress_struct, image_width, 0x20);
WWHD_OFFSET(jpeg_compress_struct, image_height, 0x24);
WWHD_OFFSET(jpeg_compress_struct, input_components, 0x28);
WWHD_OFFSET(jpeg_compress_struct, in_color_space, 0x2C);
WWHD_OFFSET(jpeg_compress_struct, input_gamma, 0x30);
WWHD_OFFSET(jpeg_compress_struct, scale_num, 0x38);
WWHD_OFFSET(jpeg_compress_struct, scale_denom, 0x3C);
WWHD_OFFSET(jpeg_compress_struct, jpeg_width, 0x40);
WWHD_OFFSET(jpeg_compress_struct, jpeg_height, 0x44);
WWHD_OFFSET(jpeg_compress_struct, data_precision, 0x48);
WWHD_OFFSET(jpeg_compress_struct, num_components, 0x4C);
WWHD_OFFSET(jpeg_compress_struct, jpeg_color_space, 0x50);
WWHD_OFFSET(jpeg_compress_struct, comp_info, 0x54);
WWHD_OFFSET(jpeg_compress_struct, quant_tbl_ptrs, 0x58);
WWHD_OFFSET(jpeg_compress_struct, q_scale_factor, 0x68);
WWHD_OFFSET(jpeg_compress_struct, dc_huff_tbl_ptrs, 0x78);
WWHD_OFFSET(jpeg_compress_struct, ac_huff_tbl_ptrs, 0x88);
WWHD_OFFSET(jpeg_compress_struct, arith_dc_L, 0x98);
WWHD_OFFSET(jpeg_compress_struct, arith_dc_U, 0xA8);
WWHD_OFFSET(jpeg_compress_struct, arith_ac_K, 0xB8);
WWHD_OFFSET(jpeg_compress_struct, num_scans, 0xC8);
WWHD_OFFSET(jpeg_compress_struct, scan_info, 0xCC);
WWHD_OFFSET(jpeg_compress_struct, raw_data_in, 0xD0);
WWHD_OFFSET(jpeg_compress_struct, arith_code, 0xD4);
WWHD_OFFSET(jpeg_compress_struct, optimize_coding, 0xD8);
WWHD_OFFSET(jpeg_compress_struct, CCIR601_sampling, 0xDC);
WWHD_OFFSET(jpeg_compress_struct, do_fancy_downsampling, 0xE0);
WWHD_OFFSET(jpeg_compress_struct, smoothing_factor, 0xE4);
WWHD_OFFSET(jpeg_compress_struct, dct_method, 0xE8);
WWHD_OFFSET(jpeg_compress_struct, restart_interval, 0xEC);
WWHD_OFFSET(jpeg_compress_struct, restart_in_rows, 0xF0);
WWHD_OFFSET(jpeg_compress_struct, write_JFIF_header, 0xF4);
WWHD_OFFSET(jpeg_compress_struct, JFIF_major_version, 0xF8);
WWHD_OFFSET(jpeg_compress_struct, JFIF_minor_version, 0xF9);
WWHD_OFFSET(jpeg_compress_struct, density_unit, 0xFA);
WWHD_OFFSET(jpeg_compress_struct, X_density, 0xFC);
WWHD_OFFSET(jpeg_compress_struct, Y_density, 0xFE);
WWHD_OFFSET(jpeg_compress_struct, write_Adobe_marker, 0x100);
WWHD_OFFSET(jpeg_compress_struct, next_scanline, 0x104);
WWHD_OFFSET(jpeg_compress_struct, progressive_mode, 0x108);
WWHD_OFFSET(jpeg_compress_struct, max_h_samp_factor, 0x10C);
WWHD_OFFSET(jpeg_compress_struct, max_v_samp_factor, 0x110);
WWHD_OFFSET(jpeg_compress_struct, min_DCT_h_scaled_size, 0x114);
WWHD_OFFSET(jpeg_compress_struct, min_DCT_v_scaled_size, 0x118);
WWHD_OFFSET(jpeg_compress_struct, total_iMCU_rows, 0x11C);
WWHD_OFFSET(jpeg_compress_struct, comps_in_scan, 0x120);
WWHD_OFFSET(jpeg_compress_struct, cur_comp_info, 0x124);
WWHD_OFFSET(jpeg_compress_struct, MCUs_per_row, 0x134);
WWHD_OFFSET(jpeg_compress_struct, MCU_rows_in_scan, 0x138);
WWHD_OFFSET(jpeg_compress_struct, blocks_in_MCU, 0x13C);
WWHD_OFFSET(jpeg_compress_struct, MCU_membership, 0x140);
WWHD_OFFSET(jpeg_compress_struct, Ss, 0x168);
WWHD_OFFSET(jpeg_compress_struct, Se, 0x16C);
WWHD_OFFSET(jpeg_compress_struct, Ah, 0x170);
WWHD_OFFSET(jpeg_compress_struct, Al, 0x174);
WWHD_OFFSET(jpeg_compress_struct, block_size, 0x178);
WWHD_OFFSET(jpeg_compress_struct, natural_order, 0x17C);
WWHD_OFFSET(jpeg_compress_struct, lim_Se, 0x180);
WWHD_OFFSET(jpeg_compress_struct, master, 0x184);
WWHD_OFFSET(jpeg_compress_struct, main, 0x188);
WWHD_OFFSET(jpeg_compress_struct, prep, 0x18C);
WWHD_OFFSET(jpeg_compress_struct, coef, 0x190);
WWHD_OFFSET(jpeg_compress_struct, marker, 0x194);
WWHD_OFFSET(jpeg_compress_struct, cconvert, 0x198);
WWHD_OFFSET(jpeg_compress_struct, downsample, 0x19C);
WWHD_OFFSET(jpeg_compress_struct, fdct, 0x1A0);
WWHD_OFFSET(jpeg_compress_struct, entropy, 0x1A4);
WWHD_OFFSET(jpeg_compress_struct, script_space, 0x1A8);
WWHD_OFFSET(jpeg_compress_struct, script_space_size, 0x1AC);
WWHD_SIZE(jpeg_compress_struct, 0x1B0);
struct jpeg_decompress_struct : jpeg_common_struct {
  /* 0x01C */ gptr<jpeg_source_mgr> src;
  /* 0x020 */ be<u32> image_width;
  /* 0x024 */ be<u32> image_height;
  /* 0x028 */ be<s32> num_components;
  /* 0x02C */ be<s32> jpeg_color_space;
  /* 0x030 */ be<s32> out_color_space;
  /* 0x034 */ be<u32> scale_num;
  /* 0x038 */ be<u32> scale_denom;
  u8 _gamma_alignment[4];
  /* 0x040 */ be<f64> output_gamma;
  /* 0x048 */ be<s32> buffered_image;
  /* 0x04C */ be<s32> raw_data_out;
  /* 0x050 */ be<s32> dct_method;
  /* 0x054 */ be<s32> do_fancy_upsampling;
  /* 0x058 */ be<s32> do_block_smoothing;
  /* 0x05C */ be<s32> quantize_colors;
  /* 0x060 */ be<s32> dither_mode;
  /* 0x064 */ be<s32> two_pass_quantize;
  /* 0x068 */ be<s32> desired_number_of_colors;
  /* 0x06C */ be<s32> enable_1pass_quant;
  /* 0x070 */ be<s32> enable_external_quant;
  /* 0x074 */ be<s32> enable_2pass_quant;
  /* 0x078 */ be<u32> output_width;
  /* 0x07C */ be<u32> output_height;
  /* 0x080 */ be<s32> out_color_components;
  /* 0x084 */ be<s32> output_components;
  /* 0x088 */ be<s32> rec_outbuf_height;
  /* 0x08C */ be<s32> actual_number_of_colors;
  /* 0x090 */ gptr<gptr<be<u8>>> colormap;
  /* 0x094 */ be<u32> output_scanline;
  /* 0x098 */ be<s32> input_scan_number;
  /* 0x09C */ be<u32> input_iMCU_row;
  /* 0x0A0 */ be<s32> output_scan_number;
  /* 0x0A4 */ be<u32> output_iMCU_row;
  /* 0x0A8 */ gptr<be<s32>> coef_bits;
  /* 0x0AC */ gptr<JQUANT_TBL> quant_tbl_ptrs[4];
  /* 0x0BC */ gptr<JHUFF_TBL> dc_huff_tbl_ptrs[4];
  /* 0x0CC */ gptr<JHUFF_TBL> ac_huff_tbl_ptrs[4];
  /* 0x0DC */ be<s32> data_precision;
  /* 0x0E0 */ gptr<jpeg_component_info> comp_info;
  /* 0x0E4 */ be<s32> is_baseline;
  /* 0x0E8 */ be<s32> progressive_mode;
  /* 0x0EC */ be<s32> arith_code;
  /* 0x0F0 */ be<u8> arith_dc_L[16];
  /* 0x100 */ be<u8> arith_dc_U[16];
  /* 0x110 */ be<u8> arith_ac_K[16];
  /* 0x120 */ be<u32> restart_interval;
  /* 0x124 */ be<s32> saw_JFIF_marker;
  /* 0x128 */ be<u8> JFIF_major_version;
  /* 0x129 */ be<u8> JFIF_minor_version;
  /* 0x12A */ be<u8> density_unit;
  u8 _density_alignment[1];
  /* 0x12C */ be<u16> X_density;
  /* 0x12E */ be<u16> Y_density;
  /* 0x130 */ be<s32> saw_Adobe_marker;
  /* 0x134 */ be<u8> Adobe_transform;
  u8 _sampling_alignment[3];
  /* 0x138 */ be<s32> CCIR601_sampling;
  /* 0x13C */ gptr<jpeg_marker_struct> marker_list;
  /* 0x140 */ be<s32> max_h_samp_factor;
  /* 0x144 */ be<s32> max_v_samp_factor;
  /* 0x148 */ be<s32> min_DCT_h_scaled_size;
  /* 0x14C */ be<s32> min_DCT_v_scaled_size;
  /* 0x150 */ be<u32> total_iMCU_rows;
  /* 0x154 */ gptr<be<u8>> sample_range_limit;
  /* 0x158 */ be<s32> comps_in_scan;
  /* 0x15C */ gptr<jpeg_component_info> cur_comp_info[4];
  /* 0x16C */ be<u32> MCUs_per_row;
  /* 0x170 */ be<u32> MCU_rows_in_scan;
  /* 0x174 */ be<s32> blocks_in_MCU;
  /* 0x178 */ be<s32> MCU_membership[10];
  /* 0x1A0 */ be<s32> Ss;
  /* 0x1A4 */ be<s32> Se;
  /* 0x1A8 */ be<s32> Ah;
  /* 0x1AC */ be<s32> Al;
  /* 0x1B0 */ be<s32> block_size;
  /* 0x1B4 */ gptr<const be<s32>> natural_order;
  /* 0x1B8 */ be<s32> lim_Se;
  /* 0x1BC */ be<s32> unread_marker;
  /* 0x1C0 */ gptr<jpeg_decomp_master> master;
  /* 0x1C4 */ gptr<jpeg_d_main_controller> main;
  /* 0x1C8 */ gptr<jpeg_d_coef_controller> coef;
  /* 0x1CC */ gptr<jpeg_d_post_controller> post;
  /* 0x1D0 */ gptr<jpeg_input_controller> inputctl;
  /* 0x1D4 */ gptr<jpeg_marker_reader> marker;
  /* 0x1D8 */ gptr<jpeg_entropy_decoder> entropy;
  /* 0x1DC */ gptr<jpeg_inverse_dct> idct;
  /* 0x1E0 */ gptr<jpeg_upsampler> upsample;
  /* 0x1E4 */ gptr<jpeg_color_deconverter> cconvert;
  /* 0x1E8 */ gptr<jpeg_color_quantizer> cquantize;
  u8 _alignment_padding[4];
};
WWHD_OFFSET(jpeg_decompress_struct, src, 0x1C);
WWHD_OFFSET(jpeg_decompress_struct, image_width, 0x20);
WWHD_OFFSET(jpeg_decompress_struct, image_height, 0x24);
WWHD_OFFSET(jpeg_decompress_struct, num_components, 0x28);
WWHD_OFFSET(jpeg_decompress_struct, jpeg_color_space, 0x2C);
WWHD_OFFSET(jpeg_decompress_struct, out_color_space, 0x30);
WWHD_OFFSET(jpeg_decompress_struct, scale_num, 0x34);
WWHD_OFFSET(jpeg_decompress_struct, scale_denom, 0x38);
WWHD_OFFSET(jpeg_decompress_struct, output_gamma, 0x40);
WWHD_OFFSET(jpeg_decompress_struct, buffered_image, 0x48);
WWHD_OFFSET(jpeg_decompress_struct, raw_data_out, 0x4C);
WWHD_OFFSET(jpeg_decompress_struct, dct_method, 0x50);
WWHD_OFFSET(jpeg_decompress_struct, do_fancy_upsampling, 0x54);
WWHD_OFFSET(jpeg_decompress_struct, do_block_smoothing, 0x58);
WWHD_OFFSET(jpeg_decompress_struct, quantize_colors, 0x5C);
WWHD_OFFSET(jpeg_decompress_struct, dither_mode, 0x60);
WWHD_OFFSET(jpeg_decompress_struct, two_pass_quantize, 0x64);
WWHD_OFFSET(jpeg_decompress_struct, desired_number_of_colors, 0x68);
WWHD_OFFSET(jpeg_decompress_struct, enable_1pass_quant, 0x6C);
WWHD_OFFSET(jpeg_decompress_struct, enable_external_quant, 0x70);
WWHD_OFFSET(jpeg_decompress_struct, enable_2pass_quant, 0x74);
WWHD_OFFSET(jpeg_decompress_struct, output_width, 0x78);
WWHD_OFFSET(jpeg_decompress_struct, output_height, 0x7C);
WWHD_OFFSET(jpeg_decompress_struct, out_color_components, 0x80);
WWHD_OFFSET(jpeg_decompress_struct, output_components, 0x84);
WWHD_OFFSET(jpeg_decompress_struct, rec_outbuf_height, 0x88);
WWHD_OFFSET(jpeg_decompress_struct, actual_number_of_colors, 0x8C);
WWHD_OFFSET(jpeg_decompress_struct, colormap, 0x90);
WWHD_OFFSET(jpeg_decompress_struct, output_scanline, 0x94);
WWHD_OFFSET(jpeg_decompress_struct, input_scan_number, 0x98);
WWHD_OFFSET(jpeg_decompress_struct, input_iMCU_row, 0x9C);
WWHD_OFFSET(jpeg_decompress_struct, output_scan_number, 0xA0);
WWHD_OFFSET(jpeg_decompress_struct, output_iMCU_row, 0xA4);
WWHD_OFFSET(jpeg_decompress_struct, coef_bits, 0xA8);
WWHD_OFFSET(jpeg_decompress_struct, quant_tbl_ptrs, 0xAC);
WWHD_OFFSET(jpeg_decompress_struct, dc_huff_tbl_ptrs, 0xBC);
WWHD_OFFSET(jpeg_decompress_struct, ac_huff_tbl_ptrs, 0xCC);
WWHD_OFFSET(jpeg_decompress_struct, data_precision, 0xDC);
WWHD_OFFSET(jpeg_decompress_struct, comp_info, 0xE0);
WWHD_OFFSET(jpeg_decompress_struct, is_baseline, 0xE4);
WWHD_OFFSET(jpeg_decompress_struct, progressive_mode, 0xE8);
WWHD_OFFSET(jpeg_decompress_struct, arith_code, 0xEC);
WWHD_OFFSET(jpeg_decompress_struct, arith_dc_L, 0xF0);
WWHD_OFFSET(jpeg_decompress_struct, arith_dc_U, 0x100);
WWHD_OFFSET(jpeg_decompress_struct, arith_ac_K, 0x110);
WWHD_OFFSET(jpeg_decompress_struct, restart_interval, 0x120);
WWHD_OFFSET(jpeg_decompress_struct, saw_JFIF_marker, 0x124);
WWHD_OFFSET(jpeg_decompress_struct, JFIF_major_version, 0x128);
WWHD_OFFSET(jpeg_decompress_struct, JFIF_minor_version, 0x129);
WWHD_OFFSET(jpeg_decompress_struct, density_unit, 0x12A);
WWHD_OFFSET(jpeg_decompress_struct, X_density, 0x12C);
WWHD_OFFSET(jpeg_decompress_struct, Y_density, 0x12E);
WWHD_OFFSET(jpeg_decompress_struct, saw_Adobe_marker, 0x130);
WWHD_OFFSET(jpeg_decompress_struct, Adobe_transform, 0x134);
WWHD_OFFSET(jpeg_decompress_struct, CCIR601_sampling, 0x138);
WWHD_OFFSET(jpeg_decompress_struct, marker_list, 0x13C);
WWHD_OFFSET(jpeg_decompress_struct, max_h_samp_factor, 0x140);
WWHD_OFFSET(jpeg_decompress_struct, max_v_samp_factor, 0x144);
WWHD_OFFSET(jpeg_decompress_struct, min_DCT_h_scaled_size, 0x148);
WWHD_OFFSET(jpeg_decompress_struct, min_DCT_v_scaled_size, 0x14C);
WWHD_OFFSET(jpeg_decompress_struct, total_iMCU_rows, 0x150);
WWHD_OFFSET(jpeg_decompress_struct, sample_range_limit, 0x154);
WWHD_OFFSET(jpeg_decompress_struct, comps_in_scan, 0x158);
WWHD_OFFSET(jpeg_decompress_struct, cur_comp_info, 0x15C);
WWHD_OFFSET(jpeg_decompress_struct, MCUs_per_row, 0x16C);
WWHD_OFFSET(jpeg_decompress_struct, MCU_rows_in_scan, 0x170);
WWHD_OFFSET(jpeg_decompress_struct, blocks_in_MCU, 0x174);
WWHD_OFFSET(jpeg_decompress_struct, MCU_membership, 0x178);
WWHD_OFFSET(jpeg_decompress_struct, Ss, 0x1A0);
WWHD_OFFSET(jpeg_decompress_struct, Se, 0x1A4);
WWHD_OFFSET(jpeg_decompress_struct, Ah, 0x1A8);
WWHD_OFFSET(jpeg_decompress_struct, Al, 0x1AC);
WWHD_OFFSET(jpeg_decompress_struct, block_size, 0x1B0);
WWHD_OFFSET(jpeg_decompress_struct, natural_order, 0x1B4);
WWHD_OFFSET(jpeg_decompress_struct, lim_Se, 0x1B8);
WWHD_OFFSET(jpeg_decompress_struct, unread_marker, 0x1BC);
WWHD_OFFSET(jpeg_decompress_struct, master, 0x1C0);
WWHD_OFFSET(jpeg_decompress_struct, main, 0x1C4);
WWHD_OFFSET(jpeg_decompress_struct, coef, 0x1C8);
WWHD_OFFSET(jpeg_decompress_struct, post, 0x1CC);
WWHD_OFFSET(jpeg_decompress_struct, inputctl, 0x1D0);
WWHD_OFFSET(jpeg_decompress_struct, marker, 0x1D4);
WWHD_OFFSET(jpeg_decompress_struct, entropy, 0x1D8);
WWHD_OFFSET(jpeg_decompress_struct, idct, 0x1DC);
WWHD_OFFSET(jpeg_decompress_struct, upsample, 0x1E0);
WWHD_OFFSET(jpeg_decompress_struct, cconvert, 0x1E4);
WWHD_OFFSET(jpeg_decompress_struct, cquantize, 0x1E8);
WWHD_SIZE(jpeg_decompress_struct, 0x1F0);
struct jpeg_error_mgr {
  /* 0x000 */ jpeg_method<void(j_common_ptr)> error_exit;
  /* 0x004 */ jpeg_method<void(j_common_ptr, s32)> emit_message;
  /* 0x008 */ jpeg_method<void(j_common_ptr)> output_message;
  /* 0x00C */ jpeg_method<void(j_common_ptr, char *)> format_message;
  /* 0x010 */ jpeg_method<void(j_common_ptr)> reset_error_mgr;
  /* 0x014 */ be<s32> msg_code;
  /* 0x018 */ be<s32> msg_parm[20];
  /* 0x068 */ be<s32> trace_level;
  /* 0x06C */ be<s32> num_warnings;
  /* 0x070 */ gptr<gptr<char>> jpeg_message_table;
  /* 0x074 */ be<s32> last_jpeg_message;
  /* 0x078 */ gptr<gptr<char>> addon_message_table;
  /* 0x07C */ be<s32> first_addon_message;
  /* 0x080 */ be<s32> last_addon_message;
};
WWHD_OFFSET(jpeg_error_mgr, error_exit, 0x0);
WWHD_OFFSET(jpeg_error_mgr, emit_message, 0x4);
WWHD_OFFSET(jpeg_error_mgr, output_message, 0x8);
WWHD_OFFSET(jpeg_error_mgr, format_message, 0xC);
WWHD_OFFSET(jpeg_error_mgr, reset_error_mgr, 0x10);
WWHD_OFFSET(jpeg_error_mgr, msg_code, 0x14);
WWHD_OFFSET(jpeg_error_mgr, msg_parm, 0x18);
WWHD_OFFSET(jpeg_error_mgr, trace_level, 0x68);
WWHD_OFFSET(jpeg_error_mgr, num_warnings, 0x6C);
WWHD_OFFSET(jpeg_error_mgr, jpeg_message_table, 0x70);
WWHD_OFFSET(jpeg_error_mgr, last_jpeg_message, 0x74);
WWHD_OFFSET(jpeg_error_mgr, addon_message_table, 0x78);
WWHD_OFFSET(jpeg_error_mgr, first_addon_message, 0x7C);
WWHD_OFFSET(jpeg_error_mgr, last_addon_message, 0x80);
WWHD_SIZE(jpeg_error_mgr, 0x84);
struct jpeg_progress_mgr {
  /* 0x000 */ jpeg_method<void(j_common_ptr)> progress_monitor;
  /* 0x004 */ be<s32> pass_counter;
  /* 0x008 */ be<s32> pass_limit;
  /* 0x00C */ be<s32> completed_passes;
  /* 0x010 */ be<s32> total_passes;
};
WWHD_OFFSET(jpeg_progress_mgr, progress_monitor, 0x0);
WWHD_OFFSET(jpeg_progress_mgr, pass_counter, 0x4);
WWHD_OFFSET(jpeg_progress_mgr, pass_limit, 0x8);
WWHD_OFFSET(jpeg_progress_mgr, completed_passes, 0xC);
WWHD_OFFSET(jpeg_progress_mgr, total_passes, 0x10);
WWHD_SIZE(jpeg_progress_mgr, 0x14);
struct jpeg_destination_mgr {
  /* 0x000 */ gptr<be<u8>> next_output_byte;
  /* 0x004 */ be<u32> free_in_buffer;
  /* 0x008 */ jpeg_method<void(j_compress_ptr)> init_destination;
  /* 0x00C */ jpeg_method<s32(j_compress_ptr)> empty_output_buffer;
  /* 0x010 */ jpeg_method<void(j_compress_ptr)> term_destination;
};
WWHD_OFFSET(jpeg_destination_mgr, next_output_byte, 0x0);
WWHD_OFFSET(jpeg_destination_mgr, free_in_buffer, 0x4);
WWHD_OFFSET(jpeg_destination_mgr, init_destination, 0x8);
WWHD_OFFSET(jpeg_destination_mgr, empty_output_buffer, 0xC);
WWHD_OFFSET(jpeg_destination_mgr, term_destination, 0x10);
WWHD_SIZE(jpeg_destination_mgr, 0x14);
struct jpeg_source_mgr {
  /* 0x000 */ gptr<const be<u8>> next_input_byte;
  /* 0x004 */ be<u32> bytes_in_buffer;
  /* 0x008 */ jpeg_method<void(j_decompress_ptr)> init_source;
  /* 0x00C */ jpeg_method<s32(j_decompress_ptr)> fill_input_buffer;
  /* 0x010 */ jpeg_method<void(j_decompress_ptr, s32)> skip_input_data;
  /* 0x014 */ jpeg_method<s32(j_decompress_ptr, s32)> resync_to_restart;
  /* 0x018 */ jpeg_method<void(j_decompress_ptr)> term_source;
};
WWHD_OFFSET(jpeg_source_mgr, next_input_byte, 0x0);
WWHD_OFFSET(jpeg_source_mgr, bytes_in_buffer, 0x4);
WWHD_OFFSET(jpeg_source_mgr, init_source, 0x8);
WWHD_OFFSET(jpeg_source_mgr, fill_input_buffer, 0xC);
WWHD_OFFSET(jpeg_source_mgr, skip_input_data, 0x10);
WWHD_OFFSET(jpeg_source_mgr, resync_to_restart, 0x14);
WWHD_OFFSET(jpeg_source_mgr, term_source, 0x18);
WWHD_SIZE(jpeg_source_mgr, 0x1C);
struct jpeg_memory_mgr {
  /* 0x000 */ jpeg_method<void *(j_common_ptr, s32, u32)> alloc_small;
  /* 0x004 */ jpeg_method<void *(j_common_ptr, s32, u32)> alloc_large;
  /* 0x008 */ jpeg_method<JSAMPARRAY(j_common_ptr, s32, u32, u32)> alloc_sarray;
  /* 0x00C */ jpeg_method<JBLOCKARRAY(j_common_ptr, s32, u32, u32)>
      alloc_barray;
  /* 0x010 */ jpeg_method<jvirt_sarray_control *(j_common_ptr, s32, s32, u32,
                                                 u32, u32)>
      request_virt_sarray;
  /* 0x014 */ jpeg_method<jvirt_barray_control *(j_common_ptr, s32, s32, u32,
                                                 u32, u32)>
      request_virt_barray;
  /* 0x018 */ jpeg_method<void(j_common_ptr)> realize_virt_arrays;
  /* 0x01C */ jpeg_method<JSAMPARRAY(j_common_ptr, jvirt_sarray_control *, u32,
                                     u32, s32)>
      access_virt_sarray;
  /* 0x020 */ jpeg_method<JBLOCKARRAY(j_common_ptr, jvirt_barray_control *, u32,
                                      u32, s32)>
      access_virt_barray;
  /* 0x024 */ jpeg_method<void(j_common_ptr, s32)> free_pool;
  /* 0x028 */ jpeg_method<void(j_common_ptr)> self_destruct;
  /* 0x02C */ be<s32> max_memory_to_use;
  /* 0x030 */ be<s32> max_alloc_chunk;
};
WWHD_OFFSET(jpeg_memory_mgr, alloc_small, 0x0);
WWHD_OFFSET(jpeg_memory_mgr, alloc_large, 0x4);
WWHD_OFFSET(jpeg_memory_mgr, alloc_sarray, 0x8);
WWHD_OFFSET(jpeg_memory_mgr, alloc_barray, 0xC);
WWHD_OFFSET(jpeg_memory_mgr, request_virt_sarray, 0x10);
WWHD_OFFSET(jpeg_memory_mgr, request_virt_barray, 0x14);
WWHD_OFFSET(jpeg_memory_mgr, realize_virt_arrays, 0x18);
WWHD_OFFSET(jpeg_memory_mgr, access_virt_sarray, 0x1C);
WWHD_OFFSET(jpeg_memory_mgr, access_virt_barray, 0x20);
WWHD_OFFSET(jpeg_memory_mgr, free_pool, 0x24);
WWHD_OFFSET(jpeg_memory_mgr, self_destruct, 0x28);
WWHD_OFFSET(jpeg_memory_mgr, max_memory_to_use, 0x2C);
WWHD_OFFSET(jpeg_memory_mgr, max_alloc_chunk, 0x30);
WWHD_SIZE(jpeg_memory_mgr, 0x34);
/* Public controller declarations adapted from jpegint.h.
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 1997-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
struct jpeg_comp_master {
  /* 0x000 */ jpeg_method<void(j_compress_ptr)> prepare_for_pass;
  /* 0x004 */ jpeg_method<void(j_compress_ptr)> pass_startup;
  /* 0x008 */ jpeg_method<void(j_compress_ptr)> finish_pass;
  /* 0x00C */ be<s32> call_pass_startup;
  /* 0x010 */ be<s32> is_last_pass;
};
WWHD_OFFSET(jpeg_comp_master, prepare_for_pass, 0x0);
WWHD_OFFSET(jpeg_comp_master, pass_startup, 0x4);
WWHD_OFFSET(jpeg_comp_master, finish_pass, 0x8);
WWHD_OFFSET(jpeg_comp_master, call_pass_startup, 0xC);
WWHD_OFFSET(jpeg_comp_master, is_last_pass, 0x10);
WWHD_SIZE(jpeg_comp_master, 0x14);
struct jpeg_c_main_controller {
  /* 0x000 */ jpeg_method<void(j_compress_ptr, s32)> start_pass;
  /* 0x004 */ jpeg_method<void(j_compress_ptr, JSAMPARRAY, be<u32> *, u32)>
      process_data;
};
WWHD_OFFSET(jpeg_c_main_controller, start_pass, 0x0);
WWHD_OFFSET(jpeg_c_main_controller, process_data, 0x4);
WWHD_SIZE(jpeg_c_main_controller, 0x8);
struct jpeg_c_prep_controller {
  /* 0x000 */ jpeg_method<void(j_compress_ptr, s32)> start_pass;
  /* 0x004 */ jpeg_method<void(j_compress_ptr, JSAMPARRAY, be<u32> *, u32,
                               JSAMPIMAGE, be<u32> *, u32)>
      pre_process_data;
};
WWHD_OFFSET(jpeg_c_prep_controller, start_pass, 0x0);
WWHD_OFFSET(jpeg_c_prep_controller, pre_process_data, 0x4);
WWHD_SIZE(jpeg_c_prep_controller, 0x8);
struct jpeg_c_coef_controller {
  /* 0x000 */ jpeg_method<void(j_compress_ptr, s32)> start_pass;
  /* 0x004 */ jpeg_method<s32(j_compress_ptr, JSAMPIMAGE)> compress_data;
};
WWHD_OFFSET(jpeg_c_coef_controller, start_pass, 0x0);
WWHD_OFFSET(jpeg_c_coef_controller, compress_data, 0x4);
WWHD_SIZE(jpeg_c_coef_controller, 0x8);
struct jpeg_marker_writer {
  /* 0x000 */ jpeg_method<void(j_compress_ptr)> write_file_header;
  /* 0x004 */ jpeg_method<void(j_compress_ptr)> write_frame_header;
  /* 0x008 */ jpeg_method<void(j_compress_ptr)> write_scan_header;
  /* 0x00C */ jpeg_method<void(j_compress_ptr)> write_file_trailer;
  /* 0x010 */ jpeg_method<void(j_compress_ptr)> write_tables_only;
  /* 0x014 */ jpeg_method<void(j_compress_ptr, s32, u32)> write_marker_header;
  /* 0x018 */ jpeg_method<void(j_compress_ptr, s32)> write_marker_byte;
};
WWHD_OFFSET(jpeg_marker_writer, write_file_header, 0x0);
WWHD_OFFSET(jpeg_marker_writer, write_frame_header, 0x4);
WWHD_OFFSET(jpeg_marker_writer, write_scan_header, 0x8);
WWHD_OFFSET(jpeg_marker_writer, write_file_trailer, 0xC);
WWHD_OFFSET(jpeg_marker_writer, write_tables_only, 0x10);
WWHD_OFFSET(jpeg_marker_writer, write_marker_header, 0x14);
WWHD_OFFSET(jpeg_marker_writer, write_marker_byte, 0x18);
WWHD_SIZE(jpeg_marker_writer, 0x1C);
struct jpeg_color_converter {
  /* 0x000 */ jpeg_method<void(j_compress_ptr)> start_pass;
  /* 0x004 */ jpeg_method<void(j_compress_ptr, JSAMPARRAY, JSAMPIMAGE, u32,
                               s32)>
      color_convert;
};
WWHD_OFFSET(jpeg_color_converter, start_pass, 0x0);
WWHD_OFFSET(jpeg_color_converter, color_convert, 0x4);
WWHD_SIZE(jpeg_color_converter, 0x8);
struct jpeg_downsampler {
  /* 0x000 */ jpeg_method<void(j_compress_ptr)> start_pass;
  /* 0x004 */ jpeg_method<void(j_compress_ptr, JSAMPIMAGE, u32, JSAMPIMAGE,
                               u32)>
      downsample;
  /* 0x008 */ be<s32> need_context_rows;
};
WWHD_OFFSET(jpeg_downsampler, start_pass, 0x0);
WWHD_OFFSET(jpeg_downsampler, downsample, 0x4);
WWHD_OFFSET(jpeg_downsampler, need_context_rows, 0x8);
WWHD_SIZE(jpeg_downsampler, 0xC);
struct jpeg_forward_dct {
  /* 0x000 */ jpeg_method<void(j_compress_ptr)> start_pass;
  /* 0x004 */ jpeg_method<void(j_compress_ptr, jpeg_component_info *,
                               JSAMPARRAY, JBLOCKROW, u32, u32, u32)>
      forward_DCT[10];
};
WWHD_OFFSET(jpeg_forward_dct, start_pass, 0x0);
WWHD_OFFSET(jpeg_forward_dct, forward_DCT, 0x4);
WWHD_SIZE(jpeg_forward_dct, 0x2C);
struct jpeg_entropy_encoder {
  /* 0x000 */ jpeg_method<void(j_compress_ptr, s32)> start_pass;
  /* 0x004 */ jpeg_method<s32(j_compress_ptr, gptr<JBLOCK> *)> encode_mcu;
  /* 0x008 */ jpeg_method<void(j_compress_ptr)> finish_pass;
};
WWHD_OFFSET(jpeg_entropy_encoder, start_pass, 0x0);
WWHD_OFFSET(jpeg_entropy_encoder, encode_mcu, 0x4);
WWHD_OFFSET(jpeg_entropy_encoder, finish_pass, 0x8);
WWHD_SIZE(jpeg_entropy_encoder, 0xC);
struct jpeg_decomp_master {
  /* 0x000 */ jpeg_method<void(j_decompress_ptr)> prepare_for_output_pass;
  /* 0x004 */ jpeg_method<void(j_decompress_ptr)> finish_output_pass;
  /* 0x008 */ be<s32> is_dummy_pass;
};
WWHD_OFFSET(jpeg_decomp_master, prepare_for_output_pass, 0x0);
WWHD_OFFSET(jpeg_decomp_master, finish_output_pass, 0x4);
WWHD_OFFSET(jpeg_decomp_master, is_dummy_pass, 0x8);
WWHD_SIZE(jpeg_decomp_master, 0xC);
struct jpeg_input_controller {
  /* 0x000 */ jpeg_method<s32(j_decompress_ptr)> consume_input;
  /* 0x004 */ jpeg_method<void(j_decompress_ptr)> reset_input_controller;
  /* 0x008 */ jpeg_method<void(j_decompress_ptr)> start_input_pass;
  /* 0x00C */ jpeg_method<void(j_decompress_ptr)> finish_input_pass;
  /* 0x010 */ be<s32> has_multiple_scans;
  /* 0x014 */ be<s32> eoi_reached;
};
WWHD_OFFSET(jpeg_input_controller, consume_input, 0x0);
WWHD_OFFSET(jpeg_input_controller, reset_input_controller, 0x4);
WWHD_OFFSET(jpeg_input_controller, start_input_pass, 0x8);
WWHD_OFFSET(jpeg_input_controller, finish_input_pass, 0xC);
WWHD_OFFSET(jpeg_input_controller, has_multiple_scans, 0x10);
WWHD_OFFSET(jpeg_input_controller, eoi_reached, 0x14);
WWHD_SIZE(jpeg_input_controller, 0x18);
struct jpeg_d_main_controller {
  /* 0x000 */ jpeg_method<void(j_decompress_ptr, s32)> start_pass;
  /* 0x004 */ jpeg_method<void(j_decompress_ptr, JSAMPARRAY, be<u32> *, u32)>
      process_data;
};
WWHD_OFFSET(jpeg_d_main_controller, start_pass, 0x0);
WWHD_OFFSET(jpeg_d_main_controller, process_data, 0x4);
WWHD_SIZE(jpeg_d_main_controller, 0x8);
struct jpeg_d_coef_controller {
  /* 0x000 */ jpeg_method<void(j_decompress_ptr)> start_input_pass;
  /* 0x004 */ jpeg_method<s32(j_decompress_ptr)> consume_data;
  /* 0x008 */ jpeg_method<void(j_decompress_ptr)> start_output_pass;
  /* 0x00C */ jpeg_method<s32(j_decompress_ptr, JSAMPIMAGE)> decompress_data;
  /* 0x010 */ gptr<gptr<jvirt_barray_control>> coef_arrays;
};
WWHD_OFFSET(jpeg_d_coef_controller, start_input_pass, 0x0);
WWHD_OFFSET(jpeg_d_coef_controller, consume_data, 0x4);
WWHD_OFFSET(jpeg_d_coef_controller, start_output_pass, 0x8);
WWHD_OFFSET(jpeg_d_coef_controller, decompress_data, 0xC);
WWHD_OFFSET(jpeg_d_coef_controller, coef_arrays, 0x10);
WWHD_SIZE(jpeg_d_coef_controller, 0x14);
struct jpeg_marker_reader {
  /* 0x000 */ jpeg_method<void(j_decompress_ptr)> reset_marker_reader;
  /* 0x004 */ jpeg_method<s32(j_decompress_ptr)> read_markers;
  /* 0x008 */ jpeg_method<s32(j_decompress_ptr)> read_restart_marker;
  /* 0x00C */ be<s32> saw_SOI;
  /* 0x010 */ be<s32> saw_SOF;
  /* 0x014 */ be<s32> next_restart_num;
  /* 0x018 */ be<s32> discarded_bytes;
};
WWHD_OFFSET(jpeg_marker_reader, reset_marker_reader, 0x0);
WWHD_OFFSET(jpeg_marker_reader, read_markers, 0x4);
WWHD_OFFSET(jpeg_marker_reader, read_restart_marker, 0x8);
WWHD_OFFSET(jpeg_marker_reader, saw_SOI, 0xC);
WWHD_OFFSET(jpeg_marker_reader, saw_SOF, 0x10);
WWHD_OFFSET(jpeg_marker_reader, next_restart_num, 0x14);
WWHD_OFFSET(jpeg_marker_reader, discarded_bytes, 0x18);
WWHD_SIZE(jpeg_marker_reader, 0x1C);
/* Error names adapted from jerror.h.
 * Copyright (C) 1994-1997, Thomas G. Lane.
 * Modified 1997-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
enum jpeg_message_code {
  JMSG_NOMESSAGE,
  JERR_BAD_ALIGN_TYPE,
  JERR_BAD_ALLOC_CHUNK,
  JERR_BAD_BUFFER_MODE,
  JERR_BAD_COMPONENT_ID,
  JERR_BAD_CROP_SPEC,
  JERR_BAD_DCT_COEF,
  JERR_BAD_DCTSIZE,
  JERR_BAD_DROP_SAMPLING,
  JERR_BAD_HUFF_TABLE,
  JERR_BAD_IN_COLORSPACE,
  JERR_BAD_J_COLORSPACE,
  JERR_BAD_LENGTH,
  JERR_BAD_LIB_VERSION,
  JERR_BAD_MCU_SIZE,
  JERR_BAD_POOL_ID,
  JERR_BAD_PRECISION,
  JERR_BAD_PROGRESSION,
  JERR_BAD_PROG_SCRIPT,
  JERR_BAD_SAMPLING,
  JERR_BAD_SCAN_SCRIPT,
  JERR_BAD_STATE,
  JERR_BAD_STRUCT_SIZE,
  JERR_BAD_VIRTUAL_ACCESS,
  JERR_BUFFER_SIZE,
  JERR_CANT_SUSPEND,
  JERR_CCIR601_NOTIMPL,
  JERR_COMPONENT_COUNT,
  JERR_CONVERSION_NOTIMPL,
  JERR_DAC_INDEX,
  JERR_DAC_VALUE,
  JERR_DHT_INDEX,
  JERR_DQT_INDEX,
  JERR_EMPTY_IMAGE,
  JERR_EMS_READ,
  JERR_EMS_WRITE,
  JERR_EOI_EXPECTED,
  JERR_FILE_READ,
  JERR_FILE_WRITE,
  JERR_FRACT_SAMPLE_NOTIMPL,
  JERR_HUFF_CLEN_OVERFLOW,
  JERR_HUFF_MISSING_CODE,
  JERR_IMAGE_TOO_BIG,
  JERR_INPUT_EMPTY,
  JERR_INPUT_EOF,
  JERR_MISMATCHED_QUANT_TABLE,
  JERR_MISSING_DATA,
  JERR_MODE_CHANGE,
  JERR_NOTIMPL,
  JERR_NOT_COMPILED,
  JERR_NO_ARITH_TABLE,
  JERR_NO_BACKING_STORE,
  JERR_NO_HUFF_TABLE,
  JERR_NO_IMAGE,
  JERR_NO_QUANT_TABLE,
  JERR_NO_SOI,
  JERR_OUT_OF_MEMORY,
  JERR_QUANT_COMPONENTS,
  JERR_QUANT_FEW_COLORS,
  JERR_QUANT_MANY_COLORS,
  JERR_SOF_DUPLICATE,
  JERR_SOF_NO_SOS,
  JERR_SOF_UNSUPPORTED,
  JERR_SOI_DUPLICATE,
  JERR_SOS_NO_SOF,
  JERR_TFILE_CREATE,
  JERR_TFILE_READ,
  JERR_TFILE_SEEK,
  JERR_TFILE_WRITE,
  JERR_TOO_LITTLE_DATA,
  JERR_UNKNOWN_MARKER,
  JERR_VIRTUAL_BUG,
  JERR_WIDTH_OVERFLOW,
  JERR_XMS_READ,
  JERR_XMS_WRITE,
  JMSG_COPYRIGHT,
  JMSG_VERSION,
  JTRC_16BIT_TABLES,
  JTRC_ADOBE,
  JTRC_APP0,
  JTRC_APP14,
  JTRC_DAC,
  JTRC_DHT,
  JTRC_DQT,
  JTRC_DRI,
  JTRC_EMS_CLOSE,
  JTRC_EMS_OPEN,
  JTRC_EOI,
  JTRC_HUFFBITS,
  JTRC_JFIF,
  JTRC_JFIF_BADTHUMBNAILSIZE,
  JTRC_JFIF_EXTENSION,
  JTRC_JFIF_THUMBNAIL,
  JTRC_MISC_MARKER,
  JTRC_PARMLESS_MARKER,
  JTRC_QUANTVALS,
  JTRC_QUANT_3_NCOLORS,
  JTRC_QUANT_NCOLORS,
  JTRC_QUANT_SELECTED,
  JTRC_RECOVERY_ACTION,
  JTRC_RST,
  JTRC_SMOOTH_NOTIMPL,
  JTRC_SOF,
  JTRC_SOF_COMPONENT,
  JTRC_SOI,
  JTRC_SOS,
  JTRC_SOS_COMPONENT,
  JTRC_SOS_PARAMS,
  JTRC_TFILE_CLOSE,
  JTRC_TFILE_OPEN,
  JTRC_THUMB_JPEG,
  JTRC_THUMB_PALETTE,
  JTRC_THUMB_RGB,
  JTRC_UNKNOWN_IDS,
  JTRC_XMS_CLOSE,
  JTRC_XMS_OPEN,
  JWRN_ADOBE_XFORM,
  JWRN_ARITH_BAD_CODE,
  JWRN_BOGUS_PROGRESSION,
  JWRN_EXTRANEOUS_DATA,
  JWRN_HIT_MARKER,
  JWRN_HUFF_BAD_CODE,
  JWRN_JFIF_MAJOR,
  JWRN_JPEG_EOF,
  JWRN_MUST_RESYNC,
  JWRN_NOT_SEQUENTIAL,
  JWRN_TOO_MUCH_DATA
};
inline void jpeg_error(j_common_ptr cinfo, s32 code) {
  jpeg_error_mgr *err = cinfo->err;
  auto error_exit = err->error_exit.load();
  err->msg_code = code;
  error_exit(cinfo);
}
inline void jpeg_error1(j_common_ptr cinfo, s32 code, s32 p0) {
  jpeg_error_mgr *err = cinfo->err;
  auto error_exit = err->error_exit.load();
  err->msg_parm[0] = p0;
  err->msg_code = code;
  error_exit(cinfo);
}
inline void jpeg_error2(j_common_ptr cinfo, s32 code, s32 p0, s32 p1) {
  jpeg_error_mgr *err = cinfo->err;
  auto error_exit = err->error_exit.load();
  err->msg_parm[0] = p0;
  err->msg_parm[1] = p1;
  err->msg_code = code;
  error_exit(cinfo);
}
void jpeg_abort(j_common_ptr cinfo);
void jpeg_destroy(j_common_ptr cinfo);
JQUANT_TBL *jpeg_alloc_quant_table(j_common_ptr cinfo);
JHUFF_TBL *jpeg_alloc_huff_table(j_common_ptr cinfo);
s32 jdiv_round_up(s32 a, s32 b);
s32 jround_up(s32 a, s32 b);
void jcopy_sample_rows(JSAMPARRAY, s32, JSAMPARRAY, s32, s32, u32);
void jzero_far(void *, u32);
