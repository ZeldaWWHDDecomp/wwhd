/* jcparam.c (decompiled WWHD build)
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modified 2003-2008 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c parameter selection to guest storage and callbacks.
 */
#include "libjpeg/jcparam.h"
#include "libjpeg/jutils.h"
static const u32 std_luminance_quant_tbl[64] = {
    16, 11, 10, 16, 24,  40,  51,  61,  12, 12, 14, 19, 26,  58,  60,  55,
    14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
    18, 22, 37, 56, 68,  109, 103, 77,  24, 35, 55, 64, 81,  104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};
static const u32 std_chrominance_quant_tbl[64] = {
    17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26, 56, 99, 99, 99,
    99, 99, 47, 66, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99};
static const u8 bits_dc_luminance[17] = {0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
static const u8 val_dc_luminance[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
static const u8 bits_dc_chrominance[17] = {0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0};
static const u8 val_dc_chrominance[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
static const u8 bits_ac_luminance[17] = {0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d};
static const u8 val_ac_luminance[162] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61,
    0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52,
    0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25,
    0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45,
    0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64,
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83,
    0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
    0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3,
    0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8,
    0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};
static const u8 bits_ac_chrominance[17] = {0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77};
static const u8 val_ac_chrominance[162] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61,
    0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33,
    0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18,
    0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44,
    0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63,
    0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca,
    0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};
/* 0285065C */
void jpeg_add_quant_table(j_compress_ptr cinfo, s32 which_tbl, const be<u32> *basic_table,
                          s32 scale_factor, boolean force_baseline) {
    if (cinfo->global_state != CSTATE_START)
        jpeg_error1(cinfo, JERR_BAD_STATE, cinfo->global_state);
    if ((u32)which_tbl >= NUM_QUANT_TBLS)
        jpeg_error1(cinfo, JERR_DQT_INDEX, which_tbl);
    gptr<JQUANT_TBL> *qtblptr = &cinfo->quant_tbl_ptrs[which_tbl];
    if (*qtblptr == nullptr)
        *qtblptr = jpeg_alloc_quant_table(cinfo);
    JQUANT_TBL *table = *qtblptr;
    for (s32 i = 0; i < DCTSIZE2; ++i) {
        s32 temp = (s32)((u32)basic_table[i] * (u32)scale_factor + 50) / 100;
        if (temp < 1)
            temp = 1;
        if (temp > 32767)
            temp = 32767;
        if (force_baseline && temp > 255)
            temp = 255;
        table->quantval[i] = temp;
    }
    table->sent_table = 0;
}
/* 02850790 */
void jpeg_set_linear_quality(j_compress_ptr cinfo, s32 scale_factor, boolean force_baseline) {
    jpeg_add_quant_table(cinfo, 0, std_luminance_quant_tbl, scale_factor, force_baseline);
    jpeg_add_quant_table(cinfo, 1, std_chrominance_quant_tbl, scale_factor, force_baseline);
}
/* 02850804 */
s32 jpeg_quality_scaling(s32 quality) {
    if (quality <= 0)
        return 5000;
    if (quality > 100)
        return 0;
    return quality < 50 ? 5000 / quality : 200 - quality * 2;
}
/* 02850844 */
void jpeg_set_quality(j_compress_ptr cinfo, s32 quality, boolean force_baseline) {
    jpeg_set_linear_quality(cinfo, jpeg_quality_scaling(quality), force_baseline);
}
/* 02850878 */
static void add_huff_table(j_compress_ptr cinfo, gptr<JHUFF_TBL> *htblptr, const be<u8> *bits,
                           const be<u8> *val) {
    if (*htblptr == nullptr)
        *htblptr = jpeg_alloc_huff_table(cinfo);
    jpeg_copy_bytes((*htblptr)->bits, bits, 17);
    s32 nsymbols = 0;
    for (s32 len = 1; len <= 16; ++len)
        nsymbols += (u32)bits[len];
    if (nsymbols < 1 || nsymbols > 256)
        jpeg_error(cinfo, JERR_BAD_HUFF_TABLE);
    jpeg_copy_bytes((*htblptr)->huffval, val, nsymbols);
    (*htblptr)->sent_table = 0;
}
/* 02850934 */
static void std_huff_tables(j_compress_ptr cinfo) {
    add_huff_table(cinfo, &cinfo->dc_huff_tbl_ptrs[0], bits_dc_luminance, val_dc_luminance);
    add_huff_table(cinfo, &cinfo->ac_huff_tbl_ptrs[0], bits_ac_luminance, val_ac_luminance);
    add_huff_table(cinfo, &cinfo->dc_huff_tbl_ptrs[1], bits_dc_chrominance, val_dc_chrominance);
    add_huff_table(cinfo, &cinfo->ac_huff_tbl_ptrs[1], bits_ac_chrominance, val_ac_chrominance);
}
static inline void set_component(jpeg_component_info *comp, s32 id, s32 hsamp, s32 vsamp, s32 quant,
                                 s32 dctbl, s32 actbl) {
    comp->component_id = id;
    comp->h_samp_factor = hsamp;
    comp->v_samp_factor = vsamp;
    comp->quant_tbl_no = quant;
    comp->dc_tbl_no = dctbl;
    comp->ac_tbl_no = actbl;
}
/* 028509C8 */
void jpeg_set_colorspace(j_compress_ptr cinfo, s32 colorspace) {
    if (cinfo->global_state != CSTATE_START)
        jpeg_error1(cinfo, JERR_BAD_STATE, cinfo->global_state);
    cinfo->jpeg_color_space = colorspace;
    cinfo->write_Adobe_marker = 0;
    cinfo->write_JFIF_header = 0;
    switch (colorspace) {
    case JCS_GRAYSCALE: {
        jpeg_component_info *comp = cinfo->comp_info;
        cinfo->write_JFIF_header = 1;
        cinfo->num_components = 1;
        set_component(comp, 1, 1, 1, 0, 0, 0);
        break;
    }
    case JCS_RGB: {
        jpeg_component_info *comp = cinfo->comp_info;
        cinfo->write_Adobe_marker = 1;
        cinfo->num_components = 3;
        set_component(comp, 82, 1, 1, 0, 0, 0);
        set_component(comp + 1, 71, 1, 1, 0, 0, 0);
        set_component(comp + 2, 66, 1, 1, 0, 0, 0);
        break;
    }
    case JCS_YCbCr: {
        jpeg_component_info *comp = cinfo->comp_info;
        cinfo->write_JFIF_header = 1;
        cinfo->num_components = 3;
        set_component(comp, 1, 2, 2, 0, 0, 0);
        set_component(comp + 1, 2, 1, 1, 1, 1, 1);
        set_component(comp + 2, 3, 1, 1, 1, 1, 1);
        break;
    }
    case JCS_CMYK: {
        jpeg_component_info *comp = cinfo->comp_info;
        cinfo->write_Adobe_marker = 1;
        cinfo->num_components = 4;
        set_component(comp, 67, 1, 1, 0, 0, 0);
        set_component(comp + 1, 77, 1, 1, 0, 0, 0);
        set_component(comp + 2, 89, 1, 1, 0, 0, 0);
        set_component(comp + 3, 75, 1, 1, 0, 0, 0);
        break;
    }
    case JCS_YCCK: {
        jpeg_component_info *comp = cinfo->comp_info;
        cinfo->write_Adobe_marker = 1;
        cinfo->num_components = 4;
        set_component(comp, 1, 2, 2, 0, 0, 0);
        set_component(comp + 1, 2, 1, 1, 1, 1, 1);
        set_component(comp + 2, 3, 1, 1, 1, 1, 1);
        set_component(comp + 3, 4, 2, 2, 0, 0, 0);
        break;
    }
    case JCS_UNKNOWN: {
        cinfo->num_components = cinfo->input_components;
        if (cinfo->num_components < 1 || cinfo->num_components > MAX_COMPONENTS)
            jpeg_error2(cinfo, JERR_COMPONENT_COUNT, cinfo->num_components, MAX_COMPONENTS);
        for (s32 ci = 0; ci < cinfo->num_components; ++ci) {
            jpeg_component_info *comp = cinfo->comp_info;
            set_component(comp + ci, ci, 1, 1, 0, 0, 0);
        }
        break;
    }
    default:
        jpeg_error(cinfo, JERR_BAD_J_COLORSPACE);
    }
}
/* 02850D84 */
void jpeg_default_colorspace(j_compress_ptr cinfo) {
    s32 color_space = cinfo->in_color_space;
    if (color_space == JCS_RGB)
        color_space = JCS_YCbCr;
    if ((u32)color_space <= JCS_YCCK)
        jpeg_set_colorspace(cinfo, color_space);
    else
        jpeg_error(cinfo, JERR_BAD_IN_COLORSPACE);
}
/* 02850DF0 */
void jpeg_set_defaults(j_compress_ptr cinfo) {
    if (cinfo->global_state != CSTATE_START)
        jpeg_error1(cinfo, JERR_BAD_STATE, cinfo->global_state);
    if (cinfo->comp_info == nullptr)
        cinfo->comp_info = static_cast<jpeg_component_info *>(cinfo->mem->alloc_small(
            cinfo, JPOOL_PERMANENT, MAX_COMPONENTS * sizeof(jpeg_component_info)));
    cinfo->scale_denom = 1;
    cinfo->data_precision = 8;
    cinfo->scale_num = 1;
    jpeg_set_quality(cinfo, 75, 1);
    std_huff_tables(cinfo);
    for (s32 i = 0; i < NUM_ARITH_TBLS; ++i) {
        cinfo->arith_dc_L[i] = 0;
        cinfo->arith_dc_U[i] = 1;
        cinfo->arith_ac_K[i] = 5;
    }
    s32 precision = cinfo->data_precision;
    cinfo->arith_code = 0;
    cinfo->optimize_coding = 0;
    cinfo->num_scans = 0;
    cinfo->scan_info = nullptr;
    cinfo->raw_data_in = 0;
    if (precision > 8)
        cinfo->optimize_coding = 1;
    cinfo->CCIR601_sampling = 0;
    cinfo->JFIF_major_version = 1;
    cinfo->X_density = 1;
    cinfo->do_fancy_downsampling = 1;
    cinfo->density_unit = 0;
    cinfo->Y_density = 1;
    cinfo->restart_interval = 0;
    cinfo->smoothing_factor = 0;
    cinfo->JFIF_minor_version = 1;
    cinfo->restart_in_rows = 0;
    cinfo->dct_method = JDCT_ISLOW;
    jpeg_default_colorspace(cinfo);
}
/* WWHD glue
data std_luminance_quant_tbl 1017BCB8
data std_chrominance_quant_tbl 1017BDB8
data val_dc_luminance 1017BEB8
data val_dc_chrominance 1017BEC4
data bits_dc_luminance 1017BED0
data bits_dc_chrominance 1017BEE4
data bits_ac_luminance 1017BEF8
data bits_ac_chrominance 1017BF0C
data val_ac_luminance 1017BF20
data val_ac_chrominance 1017BFC4
*/
