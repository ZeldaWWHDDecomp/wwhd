/* d_a_himo2_wrappic_t60.h (WWHD, true 60 build, 2026-11-24): the grappling
 * hook's coil around its target drawn at each step's own share of the frame. Decomp build only.
 * Separate file: the source keeps thin calls.
 *
 * In the coil demo (mDemoState HIMO2_DEMO_COIL, once mWrapOffsetZ is past -145) new_himo2_move adds the frame's wrap
 * step to mWrapCount (r5 = ftoi(mWrapSpeed / f1) + 1, f1 = 20 - (target scale.y - 1) x 17, at least 0.5) once per 30 Hz
 * frame, on the tick30 step (the 30 Hz timer rule). spin_draw (in daHimo2_Draw) winds that many rope segments around the target and
 * puts the hook head after them, so at N 2 the head jumped once a frame (stair sweep rp on intgc2: W 01BE 130 mrad,
 * excess 8.4 units, fracT 0.18, the head still on every other pass while the count grew 2 .. 5 a frame; interp even).
 * The picture: the draw after an earlier step k of N winds mWrapCount + round(r5 x k / N) (the count the frame will
 * reach, at the step's share); the tick30 step's draw winds the count itself (the original). Draw only: the word is put
 * back after the draw. dt 1 (N 1): nothing. Negative control WWHD_T60_HIMO2_WRAPPIC_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include "d/actor/d_a_himo2.h"
#include <cstdlib>

namespace t60himo2wp {
inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_HIMO2_WRAPPIC_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* daHimo2_Draw's scope: the earlier step's wrap count for the draw, the stored one back at the end */
struct Guard {
    himo2_class* a;
    s32 keep;
    bool set;
    explicit Guard(himo2_class* x) : a(x), keep(0), set(false) {
        /* (the draw: dt is not the step's there; steps / step / tick30 are, as himo2_drawDt and t60_draw.h read them) */
        if (t60::steps() < 2 || t60::tick30() || nc())
            return;
        if (a->mDemoState != HIMO2_DEMO_COIL || a->mpTarget == nullptr || !((f32)a->mWrapOffsetZ > -145.0f))
            return;
        const s32 bc = a->mWrapCount;
        if (bc >= 400)
            return;
        f32 f1 = 20.0f - ((f32)a->mpTarget->scale.y - 1.0f) * 17.0f;
        if (f1 < 0.5f)
            f1 = 0.5f;
        const s32 r5 = (s32)((f32)a->mWrapSpeed / f1) + 1;
        const f32 share = (f32)t60::step() / (f32)t60::steps();
        const s32 add = (s32)((f32)r5 * share + 0.5f);
        if (add <= 0)
            return;
        keep = bc;
        set = true;
        a->mWrapCount = bc + add;
    }
    ~Guard() {
        if (set)
            a->mWrapCount = keep;
    }
};
}  // namespace t60himo2wp
