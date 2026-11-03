/* jmemnobs.c (decompiled WWHD build)
 * Copyright (C) 1992-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Rewritten from WWHD with guest pointers and Cafe heap forwarding.
 */
#include "libjpeg/jmemnobs.h"
static void* MEMAllocFromExpHeapEx(void* heap, u32 size, s32 alignment);
static void MEMFreeToExpHeap(void* heap, void* object);
static s32 MEMGetTotalFreeSizeForExpHeap(void* heap);

/* 02859468 */
void* jpeg_get_small(j_common_ptr cinfo, u32 sizeofobject) {
    // HD: both allocation classes use the application's Cafe heap, aligned to four bytes.
    return MEMAllocFromExpHeapEx(cinfo->allocation_heap.get(), sizeofobject, 4);
}
/* 02859474 */
void jpeg_free_small(j_common_ptr cinfo, void* object, u32 sizeofobject) {
    MEMFreeToExpHeap(cinfo->allocation_heap.get(), object);
}
/* 0285947C */
void* jpeg_get_large(j_common_ptr cinfo, u32 sizeofobject) {
    return MEMAllocFromExpHeapEx(cinfo->allocation_heap.get(), sizeofobject, 4);
}
/* 02859488 */
void jpeg_free_large(j_common_ptr cinfo, void* object, u32 sizeofobject) {
    MEMFreeToExpHeap(cinfo->allocation_heap.get(), object);
}
/* 02859490 */
s32 jpeg_mem_available(j_common_ptr cinfo, s32 min_bytes_needed, s32 max_bytes_needed, s32 already_allocated) {
    // HD: query the Cafe heap instead of promising max_bytes_needed.
    return MEMGetTotalFreeSizeForExpHeap(cinfo->allocation_heap.get());
}
/* 02859498 */
void jpeg_open_backing_store(j_common_ptr cinfo, backing_store_ptr info, s32 total_bytes_needed) {
    jpeg_error_mgr* err = cinfo->err.get();
    auto error_exit = err->error_exit.load();
    err->msg_code = JERR_NO_BACKING_STORE;
    error_exit(cinfo);
}
/* 028594B0 */
s32 jpeg_mem_init(j_common_ptr cinfo) { return 0; }
/* 028594B8 */
void jpeg_mem_term(j_common_ptr cinfo) {}

/* WWHD glue
ext C0009698 void* MEMAllocFromExpHeapEx(void* heap, u32 size, s32 alignment)
ext C0009748 void MEMFreeToExpHeap(void* heap, void* object)
ext C00097D8 s32 MEMGetTotalFreeSizeForExpHeap(void* heap)
*/
