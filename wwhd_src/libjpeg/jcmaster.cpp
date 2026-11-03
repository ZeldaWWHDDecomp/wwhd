/* jcmaster.c (decompiled WWHD build)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2003-2011 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c master control to the Cafe build without encoder scaling or scan scripts.
 */
#include "libjpeg/jcmaster.h"
#include "libjpeg/jutils.h"
/* 0284F840 */
void jpeg_calc_jpeg_dimensions(j_compress_ptr cinfo) {
    u32 height = cinfo->image_height;
    cinfo->min_DCT_h_scaled_size = DCTSIZE;
    cinfo->jpeg_height = height;
    u32 width = cinfo->image_width;
    cinfo->min_DCT_v_scaled_size = DCTSIZE;
    cinfo->jpeg_width = width;
}
/* 0284F860 */
static void jpeg_calc_trans_dimensions(j_compress_ptr cinfo) {
    if (cinfo->min_DCT_h_scaled_size != cinfo->min_DCT_v_scaled_size)
        jpeg_error2(cinfo, JERR_BAD_DCTSIZE, cinfo->min_DCT_h_scaled_size,
                    cinfo->min_DCT_v_scaled_size);
    cinfo->block_size = cinfo->min_DCT_h_scaled_size;
}
/* 0284F8C4 */
static void initial_setup(j_compress_ptr cinfo, boolean transcode_only) {
    if (transcode_only)
        jpeg_calc_trans_dimensions(cinfo);
    else
        jpeg_calc_jpeg_dimensions(cinfo);
    if ((u32)cinfo->block_size - 1 >= 16)
        jpeg_error2(cinfo, JERR_BAD_DCTSIZE, cinfo->block_size, cinfo->block_size);
    switch ((s32)cinfo->block_size) {
    case 2:
        cinfo->natural_order = jpeg_natural_order2;
        break;
    case 3:
        cinfo->natural_order = jpeg_natural_order3;
        break;
    case 4:
        cinfo->natural_order = jpeg_natural_order4;
        break;
    case 5:
        cinfo->natural_order = jpeg_natural_order5;
        break;
    case 6:
        cinfo->natural_order = jpeg_natural_order6;
        break;
    case 7:
        cinfo->natural_order = jpeg_natural_order7;
        break;
    default:
        cinfo->natural_order = jpeg_natural_order;
    }
    cinfo->lim_Se =
        cinfo->block_size < 8 ? (u32)cinfo->block_size * (u32)cinfo->block_size - 1 : 63;
    if (cinfo->jpeg_height == 0 || cinfo->jpeg_width == 0 || cinfo->num_components <= 0 ||
        cinfo->input_components <= 0)
        jpeg_error(cinfo, JERR_EMPTY_IMAGE);
    if ((s32)cinfo->jpeg_height > 65500 || (s32)cinfo->jpeg_width > 65500)
        jpeg_error1(cinfo, JERR_IMAGE_TOO_BIG, 65500);
    if (cinfo->data_precision != 8)
        jpeg_error1(cinfo, JERR_BAD_PRECISION, cinfo->data_precision);
    if (cinfo->num_components > MAX_COMPONENTS)
        jpeg_error2(cinfo, JERR_COMPONENT_COUNT, cinfo->num_components, MAX_COMPONENTS);
    jpeg_component_info *comp = cinfo->comp_info;
    cinfo->max_h_samp_factor = 1;
    cinfo->max_v_samp_factor = 1;
    for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp) {
        if ((u32)comp->h_samp_factor - 1 >= 4 || (u32)comp->v_samp_factor - 1 >= 4)
            jpeg_error(cinfo, JERR_BAD_SAMPLING);
        if (cinfo->max_h_samp_factor < comp->h_samp_factor)
            cinfo->max_h_samp_factor = comp->h_samp_factor;
        if (cinfo->max_v_samp_factor < comp->v_samp_factor)
            cinfo->max_v_samp_factor = comp->v_samp_factor;
    }
    comp = cinfo->comp_info;
    for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp) {
        comp->component_index = ci;
        // HD: encoder DCT scaling is disabled in this build.
        comp->DCT_h_scaled_size = cinfo->min_DCT_h_scaled_size;
        comp->DCT_v_scaled_size = cinfo->min_DCT_v_scaled_size;
        if (comp->DCT_h_scaled_size > (s32)((u32)comp->DCT_v_scaled_size * 2))
            comp->DCT_h_scaled_size = (u32)comp->DCT_v_scaled_size * 2;
        else if (comp->DCT_v_scaled_size > (s32)((u32)comp->DCT_h_scaled_size * 2))
            comp->DCT_v_scaled_size = (u32)comp->DCT_h_scaled_size * 2;
        comp->width_in_blocks =
            jdiv_round_up((u32)cinfo->jpeg_width * (u32)comp->h_samp_factor,
                          (u32)cinfo->max_h_samp_factor * (u32)cinfo->block_size);
        comp->height_in_blocks =
            jdiv_round_up((u32)cinfo->jpeg_height * (u32)comp->v_samp_factor,
                          (u32)cinfo->max_v_samp_factor * (u32)cinfo->block_size);
        comp->downsampled_width = jdiv_round_up(
            (u32)cinfo->jpeg_width * (u32)comp->h_samp_factor * (u32)comp->DCT_h_scaled_size,
            (u32)cinfo->max_h_samp_factor * (u32)cinfo->block_size);
        comp->downsampled_height = jdiv_round_up(
            (u32)cinfo->jpeg_height * (u32)comp->v_samp_factor * (u32)comp->DCT_v_scaled_size,
            (u32)cinfo->max_v_samp_factor * (u32)cinfo->block_size);
        comp->component_needed = 1;
    }
    cinfo->total_iMCU_rows =
        jdiv_round_up(cinfo->jpeg_height, (u32)cinfo->max_v_samp_factor * (u32)cinfo->block_size);
}
/* 0284FCE8 */
static void select_scan_parameters(j_compress_ptr cinfo) {
    // HD: this encoder only supports a single sequential scan.
    if (cinfo->num_components > MAX_COMPS_IN_SCAN)
        jpeg_error2(cinfo, JERR_COMPONENT_COUNT, cinfo->num_components, MAX_COMPS_IN_SCAN);
    s32 num_components = cinfo->num_components;
    cinfo->comps_in_scan = num_components;
    for (s32 ci = 0; ci < num_components; ++ci)
        cinfo->cur_comp_info[ci] = cinfo->comp_info.get() + ci;
    u32 block_size = cinfo->block_size;
    cinfo->Ah = 0;
    cinfo->Al = 0;
    cinfo->Ss = 0;
    cinfo->Se = block_size * block_size - 1;
}
/* 0284FD9C */
static void per_scan_setup(j_compress_ptr cinfo) {
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
        cinfo->MCUs_per_row = jdiv_round_up(cinfo->jpeg_width,
                                            (u32)cinfo->max_h_samp_factor * (u32)cinfo->block_size);
        cinfo->MCU_rows_in_scan = jdiv_round_up(cinfo->jpeg_height, (u32)cinfo->max_v_samp_factor *
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
            if ((s32)((u32)cinfo->blocks_in_MCU + (u32)mcublks) > C_MAX_BLOCKS_IN_MCU)
                jpeg_error(cinfo, JERR_BAD_MCU_SIZE);
            while (mcublks-- > 0) {
                s32 block = cinfo->blocks_in_MCU;
                cinfo->MCU_membership[block] = ci;
                cinfo->blocks_in_MCU = (u32)block + 1;
            }
        }
    }
    if (cinfo->restart_in_rows > 0) {
        s32 nominal = (u32)cinfo->restart_in_rows * (u32)cinfo->MCUs_per_row;
        cinfo->restart_interval = nominal < 65535 ? nominal : 65535;
    }
}
/* 0284FFC4 */
static void prepare_for_pass(j_compress_ptr cinfo) {
    auto *master = static_cast<my_comp_master *>(cinfo->master.get());
    switch ((s32)master->pass_type) {
    case main_pass:
        select_scan_parameters(cinfo);
        per_scan_setup(cinfo);
        if (!cinfo->raw_data_in) {
            cinfo->cconvert->start_pass(cinfo);
            cinfo->downsample->start_pass(cinfo);
            cinfo->prep->start_pass(cinfo, JBUF_PASS_THRU);
        }
        cinfo->fdct->start_pass(cinfo);
        cinfo->entropy->start_pass(cinfo, cinfo->optimize_coding);
        cinfo->coef->start_pass(cinfo,
                                master->total_passes > 1 ? JBUF_SAVE_AND_PASS : JBUF_PASS_THRU);
        cinfo->main->start_pass(cinfo, JBUF_PASS_THRU);
        master->call_pass_startup = !cinfo->optimize_coding;
        break;
    case huff_opt_pass:
        select_scan_parameters(cinfo);
        per_scan_setup(cinfo);
        if (cinfo->Ss != 0 || cinfo->Ah == 0) {
            cinfo->entropy->start_pass(cinfo, 1);
            cinfo->coef->start_pass(cinfo, JBUF_CRANK_DEST);
            master->call_pass_startup = 0;
            break;
        }
        master->pass_type = output_pass;
        ++master->pass_number;
        [[fallthrough]];
    case output_pass:
        if (!cinfo->optimize_coding) {
            select_scan_parameters(cinfo);
            per_scan_setup(cinfo);
        }
        cinfo->entropy->start_pass(cinfo, 0);
        cinfo->coef->start_pass(cinfo, JBUF_CRANK_DEST);
        if (master->scan_number == 0)
            cinfo->marker->write_frame_header(cinfo);
        cinfo->marker->write_scan_header(cinfo);
        master->call_pass_startup = 0;
        break;
    default:
        jpeg_error(cinfo, JERR_NOT_COMPILED);
    }
    master->is_last_pass = master->pass_number == (s32)((u32)master->total_passes - 1);
    jpeg_progress_mgr *progress = cinfo->progress;
    if (progress) {
        progress->completed_passes = master->pass_number;
        progress->total_passes = master->total_passes;
    }
}
/* 02850290 */
static void pass_startup(j_compress_ptr cinfo) {
    auto write_frame = cinfo->marker->write_frame_header.load();
    cinfo->master->call_pass_startup = 0;
    write_frame(cinfo);
    cinfo->marker->write_scan_header(cinfo);
}
/* 028502E8 */
static void finish_pass_master(j_compress_ptr cinfo) {
    auto *master = static_cast<my_comp_master *>(cinfo->master.get());
    cinfo->entropy->finish_pass(cinfo);
    switch ((s32)master->pass_type) {
    case main_pass:
        master->pass_type = output_pass;
        if (!cinfo->optimize_coding)
            ++master->scan_number;
        break;
    case huff_opt_pass:
        master->pass_type = output_pass;
        break;
    case output_pass:
        if (cinfo->optimize_coding)
            master->pass_type = huff_opt_pass;
        ++master->scan_number;
        break;
    }
    ++master->pass_number;
}
/* 028503B0 */
void jinit_c_master_control(j_compress_ptr cinfo, boolean transcode_only) {
    auto *master = static_cast<my_comp_master *>(
        cinfo->mem->alloc_small(cinfo, JPOOL_IMAGE, sizeof(my_comp_master)));
    master->finish_pass = finish_pass_master;
    master->pass_startup = pass_startup;
    master->prepare_for_pass = prepare_for_pass;
    cinfo->master = master;
    master->is_last_pass = 0;
    initial_setup(cinfo, transcode_only);
    if (cinfo->scan_info != nullptr)
        jpeg_error(cinfo, JERR_NOT_COMPILED);
    else {
        cinfo->progressive_mode = 0;
        cinfo->num_scans = 1;
    }
    if ((cinfo->progressive_mode || cinfo->block_size < DCTSIZE) && !cinfo->arith_code)
        cinfo->optimize_coding = 1;
    s32 pass_type =
        transcode_only ? (cinfo->optimize_coding ? huff_opt_pass : output_pass) : main_pass;
    master->scan_number = 0;
    master->pass_number = 0;
    master->pass_type = pass_type;
    master->total_passes =
        cinfo->optimize_coding ? (u32)cinfo->num_scans * 2 : (u32)cinfo->num_scans;
}
/* WWHD glue
 */
