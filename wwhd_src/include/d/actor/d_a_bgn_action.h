#pragma once
#include "d/actor/d_a_bgn.h"

// HD: private views for the mutable pose arrays and rope arrays. The original TU owns
// their constructors and storage; this split source adds no static definitions.
namespace bgn_action {
struct Words { /* 0x00 */
    be<u32> x; /* 0x04 */
    be<u32> y; /* 0x08 */
    be<u32> z;
};
WWHD_SIZE(Words, 0xC);
inline Words *words(cXyz *p) { return gabi::at<Words>(gabi::ea(p)); }
struct Line {      /* 0x00 */
    be<u32> mpPos; /* 0x04 */
    be<u32> mpSize;
};
WWHD_SIZE(Line, 8);
inline csXyz *dance_pause_1() { return gabi::at<csXyz>(0x10461A5C); }
inline csXyz *dance_pause_2() { return gabi::at<csXyz>(0x10461A8C); }
inline csXyz *dance_pause_3() { return gabi::at<csXyz>(0x10461ABC); }
inline csXyz *dance_pause_4() { return gabi::at<csXyz>(0x10461AEC); }
inline csXyz *punch_lr1_d() { return gabi::at<csXyz>(0x10461B1C); }
inline csXyz *punch_lr12_d() { return gabi::at<csXyz>(0x10461B4C); }
inline csXyz *punch_lr2_d() { return gabi::at<csXyz>(0x10461B7C); }
inline csXyz *punch_r1_d() { return gabi::at<csXyz>(0x10461BAC); }
inline csXyz *punch_r2_d() { return gabi::at<csXyz>(0x10461BDC); }
inline csXyz *punch_l1_d() { return gabi::at<csXyz>(0x10461C0C); }
inline csXyz *punch_l2_d() { return gabi::at<csXyz>(0x10461C3C); }
enum : u32 {
    JA_SE_CM_BGN_D_KAZEKIRI = 0x5973,
    JA_SE_CM_BGN_D_DANCE = 0x516F,
    JA_SE_CV_BGN_D_ATTACK = 0x4971,
    JA_SE_CM_BGN_D_PUNCH_FIRE = 0x5971
};
} // namespace bgn_action
s16 cM_rad2s(f32);
void mDoAud_monsSeStart(u32, cXyz *, u32, u32, s32);
