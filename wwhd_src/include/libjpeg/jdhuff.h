/* jdhuff.c (decompiled WWHD declarations)
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2006-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpeg_dct_types.h"
struct d_derived_tbl {
    be<s32> maxcode[18],valoffset[17]; gptr<JHUFF_TBL> pub;
    be<s32> look_nbits[256]; be<u8> look_sym[256];
};
using bit_buf_type=s32;
struct bitread_perm_state { be<bit_buf_type> get_buffer; be<s32> bits_left; };
struct bitread_working_state {
    gptr<const be<JOCTET>> next_input_byte; be<u32> bytes_in_buffer;
    be<bit_buf_type> get_buffer; be<s32> bits_left;
    gptr<jpeg_decompress_struct> cinfo;
};
struct savable_state {
    be<u32> EOBRUN; be<s32> last_dc_val[4];
    savable_state& operator=(const savable_state& rhs) {
        EOBRUN=u32(rhs.EOBRUN);
        for (u32 i=0;i<4;++i) last_dc_val[i]=s32(rhs.last_dc_val[i]);
        return *this;
    }
};
struct huff_entropy_decoder {
    jpeg_entropy_decoder pub; bitread_perm_state bitstate; savable_state saved;
    be<boolean> insufficient_data; be<u32> restarts_to_go;
    gptr<d_derived_tbl> derived_tbls[4],ac_derived_tbl,dc_derived_tbls[4],ac_derived_tbls[4];
    gptr<d_derived_tbl> dc_cur_tbls[10],ac_cur_tbls[10]; be<s32> coef_limit[10];
};
using huff_entropy_ptr=huff_entropy_decoder*;
WWHD_OFFSET(d_derived_tbl,pub,0x8C);
WWHD_OFFSET(d_derived_tbl,look_nbits,0x90);
WWHD_SIZE(d_derived_tbl,0x590);
WWHD_SIZE(bitread_perm_state,8);
WWHD_SIZE(bitread_working_state,0x14);
WWHD_SIZE(savable_state,0x14);
WWHD_OFFSET(huff_entropy_decoder,saved,0x10);
WWHD_OFFSET(huff_entropy_decoder,insufficient_data,0x24);
WWHD_OFFSET(huff_entropy_decoder,restarts_to_go,0x28);
WWHD_SIZE(huff_entropy_decoder,0xD8);
#define BIT_BUF_SIZE 32
#define HUFF_LOOKAHEAD 8
#define MIN_GET_BITS (BIT_BUF_SIZE-7)
#define BITREAD_STATE_VARS bit_buf_type get_buffer; int bits_left; gabi::Local<bitread_working_state> br_state_storage; bitread_working_state& br_state=*br_state_storage
#define ASSIGN_STATE(dest,src) ((dest)=(src))
static void huff_clear_words(be<s32>* dest,u32 count);
static void jpeg_make_d_derived_tbl (j_decompress_ptr cinfo, boolean isDC, int tblno,
			 gptr<d_derived_tbl>* pdtbl);
static boolean jpeg_fill_bit_buffer (bitread_working_state * state,
		      bit_buf_type get_buffer, int bits_left,
		      int nbits)
/* Load up the bit buffer to a depth of at least nbits */;
static int jpeg_huff_decode (bitread_working_state * state,
		  bit_buf_type get_buffer, int bits_left,
		  d_derived_tbl * htbl, int min_bits);
static boolean process_restart (j_decompress_ptr cinfo);
static boolean decode_mcu_DC_first (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data);
static boolean decode_mcu_AC_first (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data);
static boolean decode_mcu_DC_refine (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data);
static boolean decode_mcu_AC_refine (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data);
static boolean decode_mcu_sub (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data);
static boolean decode_mcu (j_decompress_ptr cinfo, gptr<JBLOCK>* MCU_data);
static void start_pass_huff_decoder (j_decompress_ptr cinfo);
void jinit_huff_decoder (j_decompress_ptr cinfo);
