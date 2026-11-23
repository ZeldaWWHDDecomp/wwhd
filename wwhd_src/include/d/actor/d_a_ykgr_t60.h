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

struct daYkgr_c;
namespace ykgr_t60 {
static inline const t60w::Ops<daYkgr_c>& ops() { return t60objw::Logic<daYkgr_c>::kOps; }
}  // namespace ykgr_t60
