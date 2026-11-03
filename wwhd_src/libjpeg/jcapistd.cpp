/* jcapistd.c (decompiled WWHD build)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c standard compression API to guest method tables.
 */
#include "libjpeg/jcapimin.h"
void jinit_compress_master(j_compress_ptr);
/* 0284E424 */
void jpeg_start_compress(j_compress_ptr cinfo, boolean write_all_tables) {
    if (cinfo->global_state != CSTATE_START)
        jpeg_error1(cinfo, JERR_BAD_STATE, cinfo->global_state);
    if (write_all_tables)
        jpeg_suppress_tables(cinfo, 0);
    cinfo->err->reset_error_mgr(cinfo);
    cinfo->dest->init_destination(cinfo);
    jinit_compress_master(cinfo);
    cinfo->master->prepare_for_pass(cinfo);
    boolean raw_data_in = cinfo->raw_data_in;
    cinfo->next_scanline = 0;
    cinfo->global_state = raw_data_in ? CSTATE_RAW_OK : CSTATE_SCANNING;
}
/* 0284E4FC */
JDIMENSION jpeg_write_scanlines(j_compress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION num_lines) {
    if (cinfo->global_state != CSTATE_SCANNING)
        jpeg_error1(cinfo, JERR_BAD_STATE, cinfo->global_state);
    if (cinfo->next_scanline >= cinfo->image_height) {
        jpeg_error_mgr *err = cinfo->err;
        auto emit = err->emit_message.load();
        err->msg_code = JWRN_TOO_MUCH_DATA;
        emit(cinfo, -1);
    }
    jpeg_progress_mgr *progress = cinfo->progress;
    if (progress) {
        auto monitor = progress->progress_monitor.load();
        u32 height = cinfo->image_height;
        u32 next_scanline = cinfo->next_scanline;
        progress->pass_limit = height;
        progress->pass_counter = next_scanline;
        monitor(cinfo);
    }
    jpeg_comp_master *master = cinfo->master;
    if (master->call_pass_startup)
        master->pass_startup(cinfo);
    u32 rows_left = cinfo->image_height - cinfo->next_scanline;
    if (num_lines > rows_left)
        num_lines = rows_left;
    gabi::Local<be<u32>> row_ctr;
    *row_ctr.get() = 0;
    cinfo->main->process_data(cinfo, scanlines, row_ctr.get(), num_lines);
    cinfo->next_scanline += *row_ctr.get();
    return *row_ctr.get();
}

/* WWHD glue
 */
