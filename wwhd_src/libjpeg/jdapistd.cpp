/* jdapistd.c (decompiled WWHD build)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c standard decompression API to guest method tables.
 */
#include "libjpeg/jpeglib.h"
void jinit_master_decompress(j_decompress_ptr);
/* 02852930 */
static boolean output_pass_setup(j_decompress_ptr cinfo) {
    if (cinfo->global_state != DSTATE_PRESCAN) {
        cinfo->master->prepare_for_output_pass(cinfo);
        cinfo->output_scanline = 0;
        cinfo->global_state = DSTATE_PRESCAN;
    }
    while (cinfo->master->is_dummy_pass) {
        u32 scanline = cinfo->output_scanline;
        u32 height = cinfo->output_height;
        while (scanline < height) {
            jpeg_progress_mgr *progress = cinfo->progress;
            if (progress) {
                auto monitor = progress->progress_monitor.load();
                progress->pass_counter = scanline;
                progress->pass_limit = height;
                monitor(cinfo);
            }
            u32 last_scanline = cinfo->output_scanline;
            cinfo->main->process_data(cinfo, nullptr, &cinfo->output_scanline, 0);
            scanline = cinfo->output_scanline;
            if (scanline == last_scanline)
                return 0;
            height = cinfo->output_height;
        }
        cinfo->master->finish_output_pass(cinfo);
        cinfo->master->prepare_for_output_pass(cinfo);
        cinfo->output_scanline = 0;
    }
    cinfo->global_state = cinfo->raw_data_out ? DSTATE_RAW_OK : DSTATE_SCANNING;
    return 1;
}
/* 02852A88 */
boolean jpeg_start_decompress(j_decompress_ptr cinfo) {
    if (cinfo->global_state == DSTATE_READY) {
        jinit_master_decompress(cinfo);
        if (cinfo->buffered_image) {
            cinfo->global_state = DSTATE_BUFIMAGE;
            return 1;
        }
        cinfo->global_state = DSTATE_PRELOAD;
    }
    if (cinfo->global_state == DSTATE_PRELOAD) {
        if (cinfo->inputctl->has_multiple_scans) {
            for (;;) {
                jpeg_progress_mgr *progress = cinfo->progress;
                if (progress)
                    progress->progress_monitor(cinfo);
                s32 retcode = cinfo->inputctl->consume_input(cinfo);
                if (retcode == JPEG_SUSPENDED)
                    return 0;
                if (retcode == JPEG_REACHED_EOI)
                    break;
                progress = cinfo->progress;
                if (progress && (retcode == JPEG_ROW_COMPLETED || retcode == JPEG_REACHED_SOS)) {
                    s32 counter = (u32)progress->pass_counter + 1;
                    s32 limit = progress->pass_limit;
                    progress->pass_counter = counter;
                    if (counter >= limit)
                        progress->pass_limit = (u32)limit + (u32)cinfo->total_iMCU_rows;
                }
            }
        }
        cinfo->output_scan_number = cinfo->input_scan_number;
    } else if (cinfo->global_state != DSTATE_PRESCAN)
        jpeg_error1(cinfo, JERR_BAD_STATE, cinfo->global_state);
    return output_pass_setup(cinfo);
}
/* 02852BF8 */
JDIMENSION jpeg_read_scanlines(j_decompress_ptr cinfo, JSAMPARRAY scanlines, u32 max_lines) {
    if (cinfo->global_state != DSTATE_SCANNING)
        jpeg_error1(cinfo, JERR_BAD_STATE, cinfo->global_state);
    u32 scanline = cinfo->output_scanline;
    u32 height = cinfo->output_height;
    if (scanline >= height) {
        jpeg_error_mgr *err = cinfo->err;
        auto emit = err->emit_message.load();
        err->msg_code = JWRN_TOO_MUCH_DATA;
        emit(cinfo, -1);
        return 0;
    }
    jpeg_progress_mgr *progress = cinfo->progress;
    if (progress) {
        auto monitor = progress->progress_monitor.load();
        progress->pass_counter = scanline;
        progress->pass_limit = height;
        monitor(cinfo);
    }
    gabi::Local<be<u32>> row_ctr;
    *row_ctr.get() = 0;
    cinfo->main->process_data(cinfo, scanlines, row_ctr.get(), max_lines);
    cinfo->output_scanline += *row_ctr.get();
    return *row_ctr.get();
}
/* WWHD glue
ext 02856DC0 void jinit_master_decompress(j_decompress_ptr cinfo)
*/
