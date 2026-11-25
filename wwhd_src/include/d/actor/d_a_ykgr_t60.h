/* d_a_ykgr_t60.h (WWHD, true 60 build, 2026-11-19): the Dragon Roost mountain heat haze
 * (d_a_ykgr.cpp, daYkgr_c) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's source keeps thin calls.
 *
 * the whole-frame form rule + addendum, whole form (d/t60_objw.h): the frame reads Link's position (getPosRate: the distance to the
 * path) and chases the haze strength (mAimRate / m_aim_rate / mPosRate: cLib_addCalc2 0.25 / 0.05 per frame) and steps
 * the global alpha (l_HIO.mAlphaStep per frame) in file statics shared by every ykgr. So daYkgrExecute runs whole once
 * per 30 Hz frame on the tick30 step at dt 1 from the frame-start state; the earlier steps change no state. No model:
 * the emitter's strength (YkgrCB.setParam) and alpha change once a frame, as at 30 Hz. The draw places the emitter at
 * the camera eye on every draw (render state, the camera's picture). dt 1: the original. Negative control:
 * T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"
#include <cstdlib>

struct daYkgr_c;
namespace ykgr_t60 {
static inline const t60w::Ops<daYkgr_c>& ops() { return t60objw::Logic<daYkgr_c>::kOps; }
/* follow-up (stair sweep btd, 2026-11-24): the draw places the haze emitter at the camera's eye from the
 * camera's logic state (mEye), while the camera itself is drawn at W (its step history): on the passes after the
 * frame's earlier steps the logic eye held (the camera's frame form) and the emitter jumped 13.7 units on the tick30
 * passes, 4 units from the eye. Form (draw only): the emitter's matrix (mMtx, read by setGlobalRTMatrix only) takes the
 * drawn camera's eye, from the live view matrix the camera draw left in j3dSys (104B45F8: eye = -R^T t); current.pos
 * keeps the logic eye (the next execute's getPosRate reads it, as in A). WWHD_T60_YKGR_EYE=0: off (the negative
 * control). N = 1: the original. */
static inline bool eyeOn() {
    static const bool on = [] {
        const char* e = getenv("WWHD_T60_YKGR_EYE");
        return !(e && e[0] == '0');
    }();
    return on;
}
static inline void eyePicture(void* mtx) {
    if (t60::steps() == 1 || !eyeOn()) return;
    const u32 v = 0x104B45F8u, m = gabi::ea(mtx);
    f32 r[3][4];
    for (u32 i = 0; i < 3; i++)
        for (u32 j = 0; j < 4; j++) r[i][j] = (f32)*gabi::at<be<f32>>(v + (i * 4 + j) * 4);
    for (u32 k = 0; k < 3; k++) {
        const f32 eye = -(r[0][k] * r[0][3] + r[1][k] * r[1][3] + r[2][k] * r[2][3]);
        *gabi::at<be<f32>>(m + (k * 4 + 3) * 4) = eye;
    }
}
}  // namespace ykgr_t60
