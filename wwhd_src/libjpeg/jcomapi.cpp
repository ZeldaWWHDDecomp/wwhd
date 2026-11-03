/* jcomapi.c (decompiled WWHD build)
 * Copyright (C) 1994-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 * Adapted IJG8c common API to Cafe guest layouts and callback storage.
 */
#include "libjpeg/jcomapi.h"
/* 02850514 */
void jpeg_abort(j_common_ptr cinfo) {
    if (cinfo->mem != nullptr) {
        cinfo->mem->free_pool(cinfo, JPOOL_IMAGE);
        if (cinfo->is_decompressor) {
            cinfo->global_state = DSTATE_START;
            static_cast<j_decompress_ptr>(cinfo)->marker_list = nullptr;
        } else
            cinfo->global_state = CSTATE_START;
    }
}
/* 02850594 */
void jpeg_destroy(j_common_ptr cinfo) {
    if (cinfo->mem != nullptr)
        cinfo->mem->self_destruct(cinfo);
    cinfo->mem = nullptr;
    cinfo->global_state = 0;
}
/* 028505E4 */
JQUANT_TBL *jpeg_alloc_quant_table(j_common_ptr cinfo) {
    JQUANT_TBL *table = static_cast<JQUANT_TBL *>(
        cinfo->mem->alloc_small(cinfo, JPOOL_PERMANENT, sizeof(JQUANT_TBL)));
    table->sent_table = 0;
    return table;
}
/* 02850620 */
JHUFF_TBL *jpeg_alloc_huff_table(j_common_ptr cinfo) {
    JHUFF_TBL *table = static_cast<JHUFF_TBL *>(
        cinfo->mem->alloc_small(cinfo, JPOOL_PERMANENT, sizeof(JHUFF_TBL)));
    table->sent_table = 0;
    return table;
}
/* WWHD glue
 */
