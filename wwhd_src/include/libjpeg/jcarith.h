/* jcarith.cpp private layout (decompiled WWHD build)
 * Developed 1997-2009 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying IJG README file.
 */
#pragma once
#include "libjpeg/jpeglib.h"
#include "libjpeg/jpegint_decoder.h"
struct arith_entropy_encoder {
    jpeg_entropy_encoder pub;
    be<s32> c, a, sc, zc, ct, buffer;
    be<s32> last_dc_val[4];
    be<s32> dc_context[4];
    be<u32> restarts_to_go;
    be<s32> next_restart_num;
    gptr<be<u8>> dc_stats[16];
    gptr<be<u8>> ac_stats[16];
    be<u8> fixed_bin[4];
};
WWHD_SIZE(arith_entropy_encoder, 0xD0);
#ifndef WWHD_JPEG_ARITH_HELPERS
#define WWHD_JPEG_ARITH_HELPERS
void* arith_clear(void* target, s32 value, u32 bytes);
inline void arith_zero(void* target, u32 bytes) { arith_clear(target, 0, bytes); }
inline void arith_warn(j_common_ptr cinfo, s32 code) {
    jpeg_error_mgr* err = cinfo->err;
    auto emit = err->emit_message.load();
    err->msg_code = code;
    emit(cinfo, -1);
}
inline void arith_warn2(j_common_ptr cinfo, s32 code, s32 p0, s32 p1) {
    jpeg_error_mgr* err = cinfo->err;
    auto emit = err->emit_message.load();
    err->msg_parm[0] = p0; err->msg_parm[1] = p1;
    err->msg_code = code;
    emit(cinfo, -1);
}
inline void arith_error4(j_common_ptr cinfo, s32 code, s32 p0, s32 p1, s32 p2, s32 p3) {
    jpeg_error_mgr* err = cinfo->err;
    auto error_exit = err->error_exit.load();
    err->msg_parm[0] = p0; err->msg_parm[1] = p1;
    err->msg_parm[2] = p2; err->msg_parm[3] = p3;
    err->msg_code = code;
    error_exit(cinfo);
}
#endif
void jinit_arith_encoder(j_compress_ptr cinfo);
