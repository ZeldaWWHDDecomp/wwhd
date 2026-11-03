/* jcapimin.c (decompiled WWHD build)
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * Modified 2003-2010 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c compression API to Cafe allocation and guest method tables.
 */
#include "libjpeg/jcapimin.h"
#include "libjpeg/jutils.h"
void jpeg_zero_words(void *, u32);
void jinit_memory_mgr(j_common_ptr);
/* 0284E0CC */
void jpeg_CreateCompress(j_compress_ptr cinfo, s32 version, u32 structsize) {
    cinfo->mem = nullptr;
    if (version != JPEG_LIB_VERSION)
        jpeg_error2(cinfo, JERR_BAD_LIB_VERSION, JPEG_LIB_VERSION, version);
    if (structsize != sizeof(jpeg_compress_struct))
        jpeg_error2(cinfo, JERR_BAD_STRUCT_SIZE, sizeof(jpeg_compress_struct), structsize);
    jpeg_error_mgr *err = cinfo->err;
    void *client_data = cinfo->client_data;
    void *allocation_heap = cinfo->allocation_heap;
    // HD: preserve the Cafe heap handle together with IJG's application fields.
    jpeg_zero_words(cinfo, sizeof(jpeg_compress_struct) / 4);
    cinfo->err = err;
    cinfo->client_data = client_data;
    cinfo->is_decompressor = 0;
    cinfo->allocation_heap = allocation_heap;
    jinit_memory_mgr(cinfo);
    cinfo->dest = nullptr;
    cinfo->progress = nullptr;
    cinfo->comp_info = nullptr;
    for (s32 i = 0; i < NUM_QUANT_TBLS; ++i) {
        cinfo->q_scale_factor[i] = 100;
        cinfo->quant_tbl_ptrs[i] = nullptr;
    }
    for (s32 i = 0; i < NUM_HUFF_TBLS; ++i) {
        cinfo->dc_huff_tbl_ptrs[i] = nullptr;
        cinfo->ac_huff_tbl_ptrs[i] = nullptr;
    }
    cinfo->script_space = nullptr;
    cinfo->block_size = DCTSIZE;
    cinfo->natural_order = jpeg_natural_order;
    cinfo->lim_Se = DCTSIZE2 - 1;
    cinfo->global_state = CSTATE_START;
    cinfo->input_gamma = 1.0;
}
/* 0284E214 */
void jpeg_destroy_compress(j_compress_ptr cinfo) { jpeg_destroy(cinfo); }
/* 0284E218 */
void jpeg_suppress_tables(j_compress_ptr cinfo, s32 suppress) {
    for (s32 i = 0; i < NUM_QUANT_TBLS; ++i) {
        JQUANT_TBL *qtbl = cinfo->quant_tbl_ptrs[i];
        if (qtbl)
            qtbl->sent_table = suppress;
    }
    for (s32 i = 0; i < NUM_HUFF_TBLS; ++i) {
        JHUFF_TBL *htbl = cinfo->dc_huff_tbl_ptrs[i];
        if (htbl)
            htbl->sent_table = suppress;
        htbl = cinfo->ac_huff_tbl_ptrs[i];
        if (htbl)
            htbl->sent_table = suppress;
    }
}
/* 0284E280 */
void jpeg_finish_compress(j_compress_ptr cinfo) {
    if (cinfo->global_state == CSTATE_SCANNING || cinfo->global_state == CSTATE_RAW_OK) {
        if (cinfo->next_scanline < cinfo->image_height)
            jpeg_error(cinfo, JERR_TOO_LITTLE_DATA);
        cinfo->master->finish_pass(cinfo);
    } else if (cinfo->global_state != CSTATE_WRCOEFS)
        jpeg_error1(cinfo, JERR_BAD_STATE, cinfo->global_state);
    while (!cinfo->master->is_last_pass) {
        cinfo->master->prepare_for_pass(cinfo);
        for (u32 iMCU_row = 0; iMCU_row < cinfo->total_iMCU_rows; ++iMCU_row) {
            if (cinfo->progress != nullptr) {
                jpeg_progress_mgr *progress = cinfo->progress;
                auto monitor = progress->progress_monitor.load();
                progress->pass_counter = iMCU_row;
                progress->pass_limit = cinfo->total_iMCU_rows;
                monitor(cinfo);
            }
            if (!cinfo->coef->compress_data(cinfo, nullptr))
                jpeg_error(cinfo, JERR_CANT_SUSPEND);
        }
        cinfo->master->finish_pass(cinfo);
    }
    cinfo->marker->write_file_trailer(cinfo);
    cinfo->dest->term_destination(cinfo);
    jpeg_abort(cinfo);
}
/* WWHD glue
ext 028F5914 void jpeg_zero_words(void* target,u32 words)
ext 02859318 void jinit_memory_mgr(j_common_ptr cinfo)
*/
