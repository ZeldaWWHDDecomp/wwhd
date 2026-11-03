/* jquant2.cpp private layouts (decompiled WWHD build)
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpegint_color.h"
using quant_histptr = be<u16>*;
using quant_hist1d = be<u16>[32];
using quant_hist2d = quant_hist1d*;
using quant_hist3d = gptr<quant_hist1d>*;
struct twopass_cquantizer {
    jpeg_color_quantizer pub;
    gptr<gptr<be<JSAMPLE>>> sv_colormap;
    be<s32> desired;
    gptr<gptr<quant_hist1d>> histogram;
    be<boolean> needs_zeroed;
    gptr<be<s16>> fserrors;
    be<boolean> on_odd_row;
    gptr<be<s32>> error_limiter;
};
struct quant_box {
    be<s32> c0min, c0max;
    be<s32> c1min, c1max;
    be<s32> c2min, c2max;
    be<s32> volume;
    be<s32> colorcount;
};
using quant_boxptr = quant_box*;
WWHD_SIZE(quant_box, 0x20);
WWHD_OFFSET(twopass_cquantizer, histogram, 0x18);
WWHD_OFFSET(twopass_cquantizer, fserrors, 0x20);
WWHD_OFFSET(twopass_cquantizer, error_limiter, 0x28);
WWHD_SIZE(twopass_cquantizer, 0x2C);
inline void jpeg_quant2_trace(j_decompress_ptr cinfo, s32 code, s32 p0) {
    jpeg_error_mgr* err = cinfo->err;
    auto emit_message = err->emit_message.load();
    err->msg_parm[0] = p0;
    err->msg_code = code;
    emit_message(cinfo, 1);
}
void jinit_2pass_quantizer(j_decompress_ptr cinfo);

struct inverse_cmap_workspace {
    be<JSAMPLE> colorlist[256];
    be<JSAMPLE> bestcolor[128];
};
