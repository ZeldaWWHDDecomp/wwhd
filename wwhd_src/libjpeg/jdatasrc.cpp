/* jdatasrc.c (decompiled WWHD build)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * Modified 2009-2010 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c memory source to guest storage and callbacks.
 */
#include "libjpeg/jdatasrc.h"
// HD: the synthetic EOI bytes are in .bss and are written on every refill.
static u8 fake_eoi_buffer[2] = {0, 0};
boolean jpeg_resync_to_restart(j_decompress_ptr, s32);
/* 02852EA4 */
static void init_mem_source(j_decompress_ptr cinfo) {}
/* 02852EA8 */
static boolean fill_mem_input_buffer(j_decompress_ptr cinfo) {
    jpeg_error_mgr *err = cinfo->err;
    auto emit = err->emit_message.load();
    err->msg_code = JWRN_JPEG_EOF;
    emit(cinfo, -1);
    jpeg_source_mgr *src = cinfo->src;
    src->next_input_byte = fake_eoi_buffer;
    fake_eoi_buffer[1] = 0xD9;
    src->bytes_in_buffer = 2;
    fake_eoi_buffer[0] = 0xFF;
    return 1;
}
/* 02852F18 */
static void skip_input_data(j_decompress_ptr cinfo, s32 num_bytes) {
    jpeg_source_mgr *src = cinfo->src;
    if (num_bytes <= 0)
        return;
    u32 bytes_in_buffer = src->bytes_in_buffer;
    while (num_bytes > (s32)bytes_in_buffer) {
        auto fill = src->fill_input_buffer.load();
        num_bytes = (u32)num_bytes - bytes_in_buffer;
        fill(cinfo);
        bytes_in_buffer = src->bytes_in_buffer;
    }
    const be<u8> *next_input_byte = src->next_input_byte;
    src->bytes_in_buffer = bytes_in_buffer - (u32)num_bytes;
    src->next_input_byte = next_input_byte + num_bytes;
}
/* 02852F9C */
static void term_source(j_decompress_ptr cinfo) {}
/* 02852FA0 */
void jpeg_mem_src(j_decompress_ptr cinfo, const be<u8> *inbuffer, u32 insize) {
    if (inbuffer == nullptr || insize == 0)
        jpeg_error(cinfo, JERR_INPUT_EMPTY);
    jpeg_source_mgr *src = cinfo->src;
    if (src == nullptr) {
        src = static_cast<jpeg_source_mgr *>(
            cinfo->mem->alloc_small(cinfo, JPOOL_PERMANENT, sizeof(jpeg_source_mgr)));
        cinfo->src = src;
    }
    src->term_source = term_source;
    src->fill_input_buffer = fill_mem_input_buffer;
    src->resync_to_restart = jpeg_resync_to_restart;
    src->init_source = init_mem_source;
    src->bytes_in_buffer = insize;
    src->next_input_byte = inbuffer;
    src->skip_input_data = skip_input_data;
}
/* WWHD glue
ext 0285611C boolean jpeg_resync_to_restart(j_decompress_ptr cinfo,s32 desired) callback
data fake_eoi_buffer 104B6744 nocheck
*/
