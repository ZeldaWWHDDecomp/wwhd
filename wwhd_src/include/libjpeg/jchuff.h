/* jchuff.cpp private layouts (decompiled WWHD build)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2006-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpeglib.h"
struct c_derived_tbl {
    be<u32> ehufco[256];
    be<s8> ehufsi[256];
};
struct huff_encoder_state {
    be<s32> put_buffer, put_bits;
    be<s32> last_dc_val[4];
};
struct huff_entropy_encoder {
    jpeg_entropy_encoder pub;
    huff_encoder_state saved;
    be<u32> restarts_to_go;
    be<s32> next_restart_num;
    gptr<c_derived_tbl> dc_derived_tbls[4], ac_derived_tbls[4];
    gptr<be<s32>> dc_count_ptrs[4], ac_count_ptrs[4];
    be<boolean> gather_statistics;
    gptr<be<JOCTET>> next_output_byte;
    be<u32> free_in_buffer;
    gptr<jpeg_compress_struct> cinfo;
    be<s32> ac_tbl_no;
    be<u32> EOBRUN, BE;
    gptr<be<u8>> bit_buffer;
};
struct huff_encoder_working_state {
    gptr<be<JOCTET>> next_output_byte;
    be<u32> free_in_buffer;
    huff_encoder_state cur;
    gptr<jpeg_compress_struct> cinfo;
};
WWHD_SIZE(c_derived_tbl, 0x500);
WWHD_SIZE(huff_encoder_state, 0x18);
WWHD_SIZE(huff_encoder_working_state, 0x24);
WWHD_OFFSET(huff_entropy_encoder, dc_derived_tbls, 0x2C);
WWHD_OFFSET(huff_entropy_encoder, cinfo, 0x78);
WWHD_SIZE(huff_entropy_encoder, 0x8C);
void* huff_clear(void* target, s32 value, u32 bytes);
void* huff_copy(void* target, const void* source, u32 bytes);
inline void huff_zero(void* target, u32 bytes) { huff_clear(target, 0, bytes); }
void jinit_huff_encoder(j_compress_ptr cinfo);

struct optimal_huffman_workspace {
    be<u8> bits[33];
    be<s32> codesize[257];
    be<s32> others[257];
};
struct huffman_finished_tables {
    be<boolean> did_dc[4], did_ac[4];
};
void huff_clear_words(void* target, u32 words);
