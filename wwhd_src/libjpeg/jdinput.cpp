/* jdinput.c (decompiled WWHD build)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2002-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c input control to guest storage and Cafe outlined scaling helpers.
 */
#include "libjpeg/jdinput.h"
#include "libjpeg/jutils.h"
/* 02853078 */
void jpeg_core_output_dimensions(j_decompress_ptr cinfo) {
    // HD: the compiler outlines the rounded scale and dimension expressions.
    s32 scaled_size = jpeg_scaled_dct_size(cinfo->block_size, cinfo->scale_num, cinfo->scale_denom);
    cinfo->output_width = jpeg_scaled_dimension(cinfo->image_width, cinfo->block_size, scaled_size);
    u32 height = jpeg_scaled_dimension(cinfo->image_height, cinfo->block_size, scaled_size);
    cinfo->min_DCT_h_scaled_size = scaled_size;
    cinfo->min_DCT_v_scaled_size = scaled_size;
    jpeg_component_info *comp = cinfo->comp_info;
    cinfo->output_height = height;
    for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp) {
        comp->DCT_v_scaled_size = scaled_size;
        comp->DCT_h_scaled_size = scaled_size;
    }
}
/* 0285311C */
static void initial_setup(j_decompress_ptr cinfo) {
    if ((s32)cinfo->image_height > 65500 || (s32)cinfo->image_width > 65500)
        jpeg_error1(cinfo, JERR_IMAGE_TOO_BIG, 65500);
    if (cinfo->data_precision != 8)
        jpeg_error1(cinfo, JERR_BAD_PRECISION, cinfo->data_precision);
    if (cinfo->num_components > MAX_COMPONENTS)
        jpeg_error2(cinfo, JERR_COMPONENT_COUNT, cinfo->num_components, MAX_COMPONENTS);
    cinfo->max_h_samp_factor = 1;
    cinfo->max_v_samp_factor = 1;
    jpeg_component_info *comp = cinfo->comp_info;
    for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp) {
        s32 hsamp = comp->h_samp_factor, vsamp = comp->v_samp_factor;
        if (hsamp <= 0 || hsamp > 4 || vsamp <= 0 || vsamp > 4)
            jpeg_error(cinfo, JERR_BAD_SAMPLING);
        if (cinfo->max_h_samp_factor < comp->h_samp_factor)
            cinfo->max_h_samp_factor = comp->h_samp_factor;
        if (cinfo->max_v_samp_factor < comp->v_samp_factor)
            cinfo->max_v_samp_factor = comp->v_samp_factor;
    }
    if (cinfo->is_baseline || (cinfo->progressive_mode && cinfo->comps_in_scan)) {
        cinfo->block_size = DCTSIZE;
        cinfo->natural_order = jpeg_natural_order;
        cinfo->lim_Se = 63;
    } else {
        s32 spectral_end = cinfo->Se;
        switch (spectral_end) {
        case 0:
            cinfo->block_size = 1;
            cinfo->natural_order = jpeg_natural_order;
            cinfo->lim_Se = 0;
            break;
        case 3:
            cinfo->block_size = 2;
            cinfo->natural_order = jpeg_natural_order2;
            cinfo->lim_Se = 3;
            break;
        case 8:
            cinfo->block_size = 3;
            cinfo->natural_order = jpeg_natural_order3;
            cinfo->lim_Se = 8;
            break;
        case 15:
            cinfo->block_size = 4;
            cinfo->natural_order = jpeg_natural_order4;
            cinfo->lim_Se = 15;
            break;
        case 24:
            cinfo->block_size = 5;
            cinfo->natural_order = jpeg_natural_order5;
            cinfo->lim_Se = 24;
            break;
        case 35:
            cinfo->block_size = 6;
            cinfo->natural_order = jpeg_natural_order6;
            cinfo->lim_Se = 35;
            break;
        case 48:
            cinfo->block_size = 7;
            cinfo->natural_order = jpeg_natural_order7;
            cinfo->lim_Se = 48;
            break;
        case 63:
        case 80:
        case 99:
        case 120:
        case 143:
        case 168:
        case 195:
        case 224:
        case 255: {
            s32 block_size = 8;
            while (block_size * block_size - 1 != spectral_end)
                ++block_size;
            cinfo->block_size = block_size;
            cinfo->natural_order = jpeg_natural_order;
            cinfo->lim_Se = 63;
            break;
        }
        default: {
            jpeg_error_mgr *err = cinfo->err;
            auto error_exit = err->error_exit.load();
            err->msg_parm[0] = cinfo->Ss;
            err->msg_parm[1] = cinfo->Se;
            err->msg_parm[2] = cinfo->Ah;
            err->msg_parm[3] = cinfo->Al;
            err->msg_code = JERR_BAD_PROGRESSION;
            error_exit(cinfo);
            break;
        }
        }
    }
    cinfo->min_DCT_h_scaled_size = cinfo->block_size;
    cinfo->min_DCT_v_scaled_size = cinfo->block_size;
    comp = cinfo->comp_info;
    for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp) {
        comp->DCT_h_scaled_size = cinfo->block_size;
        comp->DCT_v_scaled_size = cinfo->block_size;
        comp->width_in_blocks =
            jdiv_round_up((u32)cinfo->image_width * (u32)comp->h_samp_factor,
                          (u32)cinfo->max_h_samp_factor * (u32)cinfo->block_size);
        comp->height_in_blocks =
            jdiv_round_up((u32)cinfo->image_height * (u32)comp->v_samp_factor,
                          (u32)cinfo->max_v_samp_factor * (u32)cinfo->block_size);
        comp->downsampled_width = jdiv_round_up((u32)cinfo->image_width * (u32)comp->h_samp_factor,
                                                cinfo->max_h_samp_factor);
        comp->downsampled_height = jdiv_round_up(
            (u32)cinfo->image_height * (u32)comp->v_samp_factor, cinfo->max_v_samp_factor);
        comp->component_needed = 1;
        comp->quant_table = nullptr;
    }
    cinfo->total_iMCU_rows =
        jdiv_round_up(cinfo->image_height, (u32)cinfo->max_v_samp_factor * (u32)cinfo->block_size);
    cinfo->inputctl->has_multiple_scans =
        cinfo->comps_in_scan < cinfo->num_components || cinfo->progressive_mode;
}
/* 0285388C */
static void per_scan_setup(j_decompress_ptr cinfo) {
    if (cinfo->comps_in_scan == 1) {
        jpeg_component_info *comp = cinfo->cur_comp_info[0];
        cinfo->MCUs_per_row = comp->width_in_blocks;
        cinfo->MCU_rows_in_scan = comp->height_in_blocks;
        comp->MCU_width = 1;
        comp->MCU_height = 1;
        comp->MCU_blocks = 1;
        comp->MCU_sample_width = comp->DCT_h_scaled_size;
        comp->last_col_width = 1;
        u32 blocks = comp->height_in_blocks;
        u32 vsamp = comp->v_samp_factor;
        s32 tmp = blocks - ppc_divwu(blocks, vsamp) * vsamp;
        if (tmp == 0)
            tmp = vsamp;
        comp->last_row_height = tmp;
        cinfo->blocks_in_MCU = 1;
        cinfo->MCU_membership[0] = 0;
    } else {
        if (cinfo->comps_in_scan <= 0 || cinfo->comps_in_scan > MAX_COMPS_IN_SCAN)
            jpeg_error2(cinfo, JERR_COMPONENT_COUNT, cinfo->comps_in_scan, MAX_COMPS_IN_SCAN);
        cinfo->MCUs_per_row = jdiv_round_up(cinfo->image_width,
                                            (u32)cinfo->max_h_samp_factor * (u32)cinfo->block_size);
        cinfo->MCU_rows_in_scan = jdiv_round_up(cinfo->image_height, (u32)cinfo->max_v_samp_factor *
                                                                         (u32)cinfo->block_size);
        cinfo->blocks_in_MCU = 0;
        for (s32 ci = 0; ci < cinfo->comps_in_scan; ++ci) {
            jpeg_component_info *comp = cinfo->cur_comp_info[ci];
            comp->MCU_width = comp->h_samp_factor;
            comp->MCU_height = comp->v_samp_factor;
            comp->MCU_blocks = (u32)comp->MCU_width * (u32)comp->MCU_height;
            comp->MCU_sample_width = (u32)comp->MCU_width * (u32)comp->DCT_h_scaled_size;
            u32 width = comp->width_in_blocks, mcuw = comp->MCU_width;
            s32 tmp = width - ppc_divwu(width, mcuw) * mcuw;
            if (tmp == 0)
                tmp = mcuw;
            comp->last_col_width = tmp;
            u32 height = comp->height_in_blocks, mcuh = comp->MCU_height;
            tmp = height - ppc_divwu(height, mcuh) * mcuh;
            if (tmp == 0)
                tmp = mcuh;
            comp->last_row_height = tmp;
            s32 mcublks = comp->MCU_blocks;
            if ((s32)((u32)cinfo->blocks_in_MCU + (u32)mcublks) > D_MAX_BLOCKS_IN_MCU)
                jpeg_error(cinfo, JERR_BAD_MCU_SIZE);
            while (mcublks-- > 0) {
                s32 block = cinfo->blocks_in_MCU;
                cinfo->MCU_membership[block] = ci;
                cinfo->blocks_in_MCU = (u32)block + 1;
            }
        }
    }
}
/* 02853A94 */
static void latch_quant_tables(j_decompress_ptr cinfo) {
    for (s32 ci = 0; ci < cinfo->comps_in_scan; ++ci) {
        jpeg_component_info *comp = cinfo->cur_comp_info[ci];
        if (comp->quant_table != nullptr)
            continue;
        s32 qtblno = comp->quant_tbl_no;
        if ((u32)qtblno >= NUM_QUANT_TBLS || cinfo->quant_tbl_ptrs[qtblno] == nullptr)
            jpeg_error1(cinfo, JERR_NO_QUANT_TABLE, qtblno);
        auto *qtbl = static_cast<JQUANT_TBL *>(
            cinfo->mem->alloc_small(cinfo, JPOOL_IMAGE, sizeof(JQUANT_TBL)));
        jpeg_copy_bytes(qtbl, cinfo->quant_tbl_ptrs[qtblno], sizeof(JQUANT_TBL));
        comp->quant_table = qtbl;
    }
}
/* 02853B84 */
static void start_input_pass(j_decompress_ptr cinfo) {
    per_scan_setup(cinfo);
    latch_quant_tables(cinfo);
    cinfo->entropy->start_pass(cinfo);
    cinfo->coef->start_input_pass(cinfo);
    auto consume = cinfo->coef->consume_data.load();
    cinfo->inputctl->consume_input.entry = consume.address;
}
/* 02853BF0 */
static s32 consume_markers(j_decompress_ptr cinfo) {
    auto *inputctl = static_cast<my_input_controller *>(cinfo->inputctl.get());
    if (inputctl->eoi_reached)
        return JPEG_REACHED_EOI;
    for (;;) {
        s32 val = cinfo->marker->read_markers(cinfo);
        if (val == JPEG_REACHED_SOS) {
            if (inputctl->inheaders) {
                if (inputctl->inheaders == 1)
                    initial_setup(cinfo);
                if (cinfo->comps_in_scan == 0) {
                    inputctl->inheaders = 2;
                    continue;
                }
                inputctl->inheaders = 0;
            } else {
                if (!inputctl->has_multiple_scans)
                    jpeg_error(cinfo, JERR_EOI_EXPECTED);
                if (cinfo->comps_in_scan == 0)
                    continue;
                start_input_pass(cinfo);
            }
            return val;
        }
        if (val == JPEG_REACHED_EOI) {
            inputctl->eoi_reached = 1;
            if (inputctl->inheaders) {
                if (cinfo->marker->saw_SOF)
                    jpeg_error(cinfo, JERR_SOF_NO_SOS);
            } else if (cinfo->output_scan_number > cinfo->input_scan_number)
                cinfo->output_scan_number = cinfo->input_scan_number;
        }
        return val;
    }
}
/* 02853D60 */
static void finish_input_pass(j_decompress_ptr cinfo) {
    cinfo->inputctl->consume_input = consume_markers;
}
/* 02853D74 */
static void reset_input_controller(j_decompress_ptr cinfo) {
    auto *inputctl = static_cast<my_input_controller *>(cinfo->inputctl.get());
    jpeg_error_mgr *err = cinfo->err;
    inputctl->consume_input = consume_markers;
    auto reset_error = err->reset_error_mgr.load();
    inputctl->inheaders = 1;
    inputctl->has_multiple_scans = 0;
    inputctl->eoi_reached = 0;
    reset_error(cinfo);
    cinfo->marker->reset_marker_reader(cinfo);
    cinfo->coef_bits = nullptr;
}
/* 02853DF0 */
void jinit_input_controller(j_decompress_ptr cinfo) {
    auto *inputctl = static_cast<my_input_controller *>(
        cinfo->mem->alloc_small(cinfo, JPOOL_PERMANENT, sizeof(my_input_controller)));
    cinfo->inputctl = inputctl;
    inputctl->consume_input = consume_markers;
    inputctl->reset_input_controller = reset_input_controller;
    inputctl->start_input_pass = start_input_pass;
    inputctl->finish_input_pass = finish_input_pass;
    inputctl->has_multiple_scans = 0;
    inputctl->eoi_reached = 0;
    inputctl->inheaders = 1;
}
/* WWHD glue
 */
