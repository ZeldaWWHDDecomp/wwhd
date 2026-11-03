/* jquant1.cpp private layout (decompiled WWHD build)
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpegint_color.h"
using onepass_dither_row = be<s32>[16];
using onepass_dither_matrix = onepass_dither_row[16];
struct onepass_cquantizer {
    jpeg_color_quantizer pub;
    gptr<gptr<be<JSAMPLE>>> sv_colormap;
    be<s32> sv_actual;
    gptr<gptr<be<JSAMPLE>>> colorindex;
    be<boolean> is_padded;
    be<s32> Ncolors[4];
    be<s32> row_index;
    gptr<onepass_dither_row> odither[4];
    gptr<be<s16>> fserrors[4];
    be<boolean> on_odd_row;
};
WWHD_OFFSET(onepass_cquantizer, sv_colormap, 0x10);
WWHD_OFFSET(onepass_cquantizer, Ncolors, 0x20);
WWHD_OFFSET(onepass_cquantizer, odither, 0x34);
WWHD_OFFSET(onepass_cquantizer, fserrors, 0x44);
WWHD_SIZE(onepass_cquantizer, 0x58);
inline void jpeg_quant1_trace1(j_decompress_ptr cinfo, s32 code, s32 p0) {
    jpeg_error_mgr* err = cinfo->err;
    auto emit_message = err->emit_message.load();
    err->msg_parm[0] = p0;
    err->msg_code = code;
    emit_message(cinfo, 1);
}
inline void jpeg_quant1_trace4(j_decompress_ptr cinfo, s32 code, s32 p0, s32 p1, s32 p2, s32 p3) {
    jpeg_error_mgr* err = cinfo->err;
    auto emit_message = err->emit_message.load();
    err->msg_parm[0] = p0;
    err->msg_parm[1] = p1;
    err->msg_parm[2] = p2;
    err->msg_parm[3] = p3;
    err->msg_code = code;
    emit_message(cinfo, 1);
}
void jinit_1pass_quantizer(j_decompress_ptr cinfo);
