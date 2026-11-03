/* jcmainct.c (decompiled WWHD build)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c strip-buffer controller to guest storage and callbacks.
 */
#include "libjpeg/jcmainct.h"
/* 0284E724 */
static void process_data_simple_main(j_compress_ptr cinfo, JSAMPARRAY input_buf,
                                     be<u32> *in_row_ctr, u32 in_rows_avail) {
    my_main_controller *main = static_cast<my_main_controller *>(cinfo->main.get());
    while (main->cur_iMCU_row < cinfo->total_iMCU_rows) {
        if (main->rowgroup_ctr < (u32)cinfo->min_DCT_v_scaled_size)
            cinfo->prep->pre_process_data(cinfo, input_buf, in_row_ctr, in_rows_avail, main->buffer,
                                          &main->rowgroup_ctr, cinfo->min_DCT_v_scaled_size);
        if (main->rowgroup_ctr != (u32)cinfo->min_DCT_v_scaled_size)
            return;
        if (!cinfo->coef->compress_data(cinfo, main->buffer)) {
            if (!main->suspended) {
                --*in_row_ctr;
                main->suspended = 1;
            }
            return;
        }
        if (main->suspended) {
            ++*in_row_ctr;
            main->suspended = 0;
        }
        main->rowgroup_ctr = 0;
        ++main->cur_iMCU_row;
    }
}
/* 0284E840 */
static void start_pass_main(j_compress_ptr cinfo, s32 pass_mode) {
    my_main_controller *main = static_cast<my_main_controller *>(cinfo->main.get());
    if (cinfo->raw_data_in)
        return;
    main->pass_mode = pass_mode;
    main->rowgroup_ctr = 0;
    main->cur_iMCU_row = 0;
    main->suspended = 0;
    if (pass_mode == JBUF_PASS_THRU)
        main->process_data = process_data_simple_main;
    else
        jpeg_error(cinfo, JERR_BAD_BUFFER_MODE);
}
/* 0284E894 */
void jinit_c_main_controller(j_compress_ptr cinfo, boolean need_full_buffer) {
    auto *main = static_cast<my_main_controller *>(
        cinfo->mem->alloc_small(cinfo, JPOOL_IMAGE, sizeof(my_main_controller)));
    boolean raw_data_in = cinfo->raw_data_in;
    cinfo->main = main;
    main->start_pass = start_pass_main;
    if (raw_data_in)
        return;
    if (need_full_buffer) {
        jpeg_error(cinfo, JERR_BAD_BUFFER_MODE);
        return;
    }
    jpeg_component_info *compptr = cinfo->comp_info;
    for (s32 ci = 0; ci < cinfo->num_components; ++ci, ++compptr)
        main->buffer[ci] = cinfo->mem->alloc_sarray(
            cinfo, JPOOL_IMAGE, (u32)compptr->width_in_blocks * (u32)compptr->DCT_h_scaled_size,
            (u32)compptr->v_samp_factor * (u32)compptr->DCT_v_scaled_size);
}

/* WWHD glue
 */
