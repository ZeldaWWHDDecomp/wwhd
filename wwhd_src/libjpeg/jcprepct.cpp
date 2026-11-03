/* jcprepct.c (decompiled WWHD build)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c preprocessing controller to guest storage and callbacks.
 */
#include "libjpeg/jcprepct.h"
#include "libjpeg/jutils.h"
/* 02850F24 */
static void start_pass_prep(j_compress_ptr cinfo, s32 pass_mode) {
    auto *prep = static_cast<my_prep_controller *>(cinfo->prep.get());
    if (pass_mode != JBUF_PASS_THRU)
        jpeg_error(cinfo, JERR_BAD_BUFFER_MODE);
    u32 height = cinfo->image_height;
    prep->next_buf_row = 0;
    prep->rows_to_go = height;
    prep->this_row_group = 0;
    prep->next_buf_stop = (u32)cinfo->max_v_samp_factor * 2;
}
/* 02850F9C */
static void expand_bottom_edge(JSAMPARRAY image_data, u32 num_cols, s32 input_rows,
                               s32 output_rows) {
    for (s32 row = input_rows; row < output_rows; ++row)
        jcopy_sample_rows(image_data, (u32)input_rows - 1, image_data, row, 1, num_cols);
}
/* 02851004 */
static void pre_process_data(j_compress_ptr cinfo, JSAMPARRAY input_buf, be<u32> *in_row_ctr,
                             u32 in_rows_avail, JSAMPIMAGE output_buf, be<u32> *out_row_group_ctr,
                             u32 out_row_groups_avail) {
    auto *prep = static_cast<my_prep_controller *>(cinfo->prep.get());
    while (*in_row_ctr < in_rows_avail && *out_row_group_ctr < out_row_groups_avail) {
        u32 inrows = in_rows_avail - *in_row_ctr;
        u32 numrows = (u32)cinfo->max_v_samp_factor - (u32)prep->next_buf_row;
        if (numrows > inrows)
            numrows = inrows;
        cinfo->cconvert->color_convert(cinfo, input_buf + *in_row_ctr, prep->color_buf,
                                       prep->next_buf_row, numrows);
        *in_row_ctr += numrows;
        prep->next_buf_row += numrows;
        prep->rows_to_go -= numrows;
        if (prep->rows_to_go == 0 && prep->next_buf_row < cinfo->max_v_samp_factor) {
            for (s32 ci = 0; ci < cinfo->num_components; ++ci)
                expand_bottom_edge(prep->color_buf[ci], cinfo->image_width, prep->next_buf_row,
                                   cinfo->max_v_samp_factor);
            prep->next_buf_row = cinfo->max_v_samp_factor;
        }
        if (prep->next_buf_row == cinfo->max_v_samp_factor) {
            cinfo->downsample->downsample(cinfo, prep->color_buf, 0, output_buf,
                                          *out_row_group_ctr);
            prep->next_buf_row = 0;
            ++*out_row_group_ctr;
        }
        if (prep->rows_to_go == 0 && *out_row_group_ctr < out_row_groups_avail) {
            jpeg_component_info *comp = cinfo->comp_info;
            for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp) {
                s32 numrows = ppc_divw((u32)comp->v_samp_factor * (u32)comp->DCT_v_scaled_size,
                                       cinfo->min_DCT_v_scaled_size);
                expand_bottom_edge(
                    output_buf[ci], (u32)comp->width_in_blocks * (u32)comp->DCT_h_scaled_size,
                    *out_row_group_ctr * (u32)numrows, out_row_groups_avail * (u32)numrows);
            }
            *out_row_group_ctr = out_row_groups_avail;
            break;
        }
    }
}
/* 028511F4 */
static void pre_process_context(j_compress_ptr cinfo, JSAMPARRAY input_buf, be<u32> *in_row_ctr,
                                u32 in_rows_avail, JSAMPIMAGE output_buf,
                                be<u32> *out_row_group_ctr, u32 out_row_groups_avail) {
    auto *prep = static_cast<my_prep_controller *>(cinfo->prep.get());
    s32 buf_height = (u32)cinfo->max_v_samp_factor * 3;
    while (*out_row_group_ctr < out_row_groups_avail) {
        if (*in_row_ctr < in_rows_avail) {
            u32 inrows = in_rows_avail - *in_row_ctr;
            u32 numrows = (u32)prep->next_buf_stop - (u32)prep->next_buf_row;
            if (numrows > inrows)
                numrows = inrows;
            cinfo->cconvert->color_convert(cinfo, input_buf + *in_row_ctr, prep->color_buf,
                                           prep->next_buf_row, numrows);
            if (prep->rows_to_go == cinfo->image_height) {
                for (s32 ci = 0; ci < cinfo->num_components; ++ci)
                    for (s32 row = 1; row <= cinfo->max_v_samp_factor; ++row)
                        jcopy_sample_rows(prep->color_buf[ci], 0, prep->color_buf[ci], -row, 1,
                                          cinfo->image_width);
            }
            *in_row_ctr += numrows;
            prep->next_buf_row += numrows;
            prep->rows_to_go -= numrows;
        } else {
            if (prep->rows_to_go != 0)
                break;
            if (prep->next_buf_row < prep->next_buf_stop) {
                for (s32 ci = 0; ci < cinfo->num_components; ++ci)
                    expand_bottom_edge(prep->color_buf[ci], cinfo->image_width, prep->next_buf_row,
                                       prep->next_buf_stop);
                prep->next_buf_row = prep->next_buf_stop;
            }
        }
        if (prep->next_buf_row == prep->next_buf_stop) {
            cinfo->downsample->downsample(cinfo, prep->color_buf, prep->this_row_group, output_buf,
                                          *out_row_group_ctr);
            ++*out_row_group_ctr;
            prep->this_row_group += cinfo->max_v_samp_factor;
            if (prep->this_row_group >= buf_height)
                prep->this_row_group = 0;
            if (prep->next_buf_row >= buf_height)
                prep->next_buf_row = 0;
            prep->next_buf_stop = (u32)prep->next_buf_row + (u32)cinfo->max_v_samp_factor;
        }
    }
}
/* 02851428 */
static void create_context_buffer(j_compress_ptr cinfo) {
    auto *prep = static_cast<my_prep_controller *>(cinfo->prep.get());
    s32 rgroup_height = cinfo->max_v_samp_factor;
    auto *fake_buffer = static_cast<JSAMPARRAY>(cinfo->mem->alloc_small(
        cinfo, JPOOL_IMAGE, (u32)cinfo->num_components * 5 * (u32)rgroup_height * 4));
    jpeg_component_info *comp = cinfo->comp_info;
    for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp) {
        auto *true_buffer = cinfo->mem->alloc_sarray(
            cinfo, JPOOL_IMAGE,
            ppc_divw((u32)comp->width_in_blocks * (u32)cinfo->min_DCT_h_scaled_size *
                         (u32)cinfo->max_h_samp_factor,
                     comp->h_samp_factor),
            (u32)rgroup_height * 3);
        jpeg_copy_bytes(fake_buffer + rgroup_height, true_buffer, (u32)rgroup_height * 12);
        for (s32 i = 0; i < rgroup_height; ++i) {
            fake_buffer[i] = true_buffer[(u32)rgroup_height * 2 + (u32)i];
            fake_buffer[(u32)rgroup_height * 4 + (u32)i] = true_buffer[i];
        }
        prep->color_buf[ci] = fake_buffer + rgroup_height;
        fake_buffer += (u32)rgroup_height * 5;
    }
}
/* 02851554 */
void jinit_c_prep_controller(j_compress_ptr cinfo, boolean need_full_buffer) {
    if (need_full_buffer)
        jpeg_error(cinfo, JERR_BAD_BUFFER_MODE);
    auto *prep = static_cast<my_prep_controller *>(
        cinfo->mem->alloc_small(cinfo, JPOOL_IMAGE, sizeof(my_prep_controller)));
    boolean need_context_rows = cinfo->downsample->need_context_rows;
    cinfo->prep = prep;
    prep->start_pass = start_pass_prep;
    if (need_context_rows) {
        prep->pre_process_data = pre_process_context;
        create_context_buffer(cinfo);
    } else {
        prep->pre_process_data = pre_process_data;
        jpeg_component_info *comp = cinfo->comp_info;
        for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++comp)
            prep->color_buf[ci] = cinfo->mem->alloc_sarray(
                cinfo, JPOOL_IMAGE,
                ppc_divw((u32)comp->width_in_blocks * (u32)cinfo->min_DCT_h_scaled_size *
                             (u32)cinfo->max_h_samp_factor,
                         comp->h_samp_factor),
                cinfo->max_v_samp_factor);
    }
}
/* WWHD glue
 */
