/* jdatadst.c (decompiled WWHD build)
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * Modified 2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c memory destination to Cafe caller-owned buffers.
 */
#include "libjpeg/jdatadst.h"
/* 02852D10 */
static void init_mem_destination(j_compress_ptr cinfo) {}
/* 02852D14 */
static boolean empty_mem_output_buffer(j_compress_ptr cinfo) {
    jpeg_error1(cinfo, JERR_OUT_OF_MEMORY, 132);
    return 0;
}
/* 02852D54 */
static void term_mem_destination(j_compress_ptr cinfo) {
    auto *dest = static_cast<my_mem_destination_mgr *>(cinfo->dest.get());
    gptr<be<u8>> *outbuffer = dest->outbuffer;
    be<u8> *buffer = dest->buffer;
    u32 bufsize = dest->bufsize;
    u32 free_in_buffer = dest->free_in_buffer;
    be<u32> *outsize = dest->outsize;
    *outbuffer = buffer;
    *outsize = bufsize - free_in_buffer;
}
/* 02852D7C */
void jpeg_mem_dest(j_compress_ptr cinfo, gptr<be<u8>> *outbuffer, be<u32> *outsize) {
    if (outbuffer == nullptr || outsize == nullptr)
        jpeg_error(cinfo, JERR_BUFFER_SIZE);
    auto *dest = static_cast<my_mem_destination_mgr *>(cinfo->dest.get());
    if (dest == nullptr) {
        dest = static_cast<my_mem_destination_mgr *>(
            cinfo->mem->alloc_small(cinfo, JPOOL_PERMANENT, sizeof(my_mem_destination_mgr)));
        cinfo->dest = dest;
    }
    // HD: callers supply the allocation; this build never grows the destination.
    dest->newbuffer = nullptr;
    be<u8> *initial_buffer = *outbuffer;
    dest->outbuffer = outbuffer;
    dest->outsize = outsize;
    dest->init_destination = init_mem_destination;
    dest->empty_output_buffer = empty_mem_output_buffer;
    dest->term_destination = term_mem_destination;
    if (initial_buffer == nullptr || *outsize == 0)
        jpeg_error1(cinfo, JERR_OUT_OF_MEMORY, 132);
    be<u8> *buffer = *outbuffer;
    u32 size = *outsize;
    dest->buffer = buffer;
    dest->free_in_buffer = size;
    dest->next_output_byte = buffer;
    dest->bufsize = size;
}
/* WWHD glue
 */
