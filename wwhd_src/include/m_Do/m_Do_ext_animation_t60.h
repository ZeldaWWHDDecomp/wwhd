#pragma once
/* true60 (pair, boomerang throw: Link's hands at frames 113 / 115-116, the boomerang's release
 * point and its wall hit): the frame-start anchor of Link's old-frame blend, called around daPy_lk_c::execute's
 * mpCLModel->calc() (only Link uses mDoExt_MtxCalcOldFrame; included by d_a_player_main_04.cpp only).
 *
 * The calc (guest) blends this step's animation pose P with the pose in the old-frame buffers (weight mOldFrameRate)
 * and writes the result back into them. the rule (m_Do_ext_animation.cpp, s_oldMorfInit: counter / remain in
 * whole frames, the frame's rate rho split as rho^(1/N) per step) composes to A's one calc only for a still pose:
 * against a moving animation (the throw's arm swing) the chain over P_1 .. P_N ends off A's rho P0 + (1 - rho) P_N by
 * the earlier steps' share of the pose change (the left hand 6.4 units at 115 under procWait_init's morph at the
 * release, 1.7 at 113 under the throw's upper morph; 0 at 114 / 117 where no morph runs; the boomerang starts from
 * the hand and its flight is shifted with it).
 * Form (anchor): every step of the frame blends against the frame's START pose (the buffers as the frame's first calc
 * found them, restored before each later calc) with the weight rho^(k/N) for its k-th step, k = 1 .. N. The last step's
 * calc is A's calc (rho against A's old pose with the same P_N), moving or still; k = 1 is the rule's rho^(1/N) and
 * a still pose gives the same in-between poses as before. Outside the calc mOldFrameRate keeps rho^(1/N) (Link's
 * logic tests read it). A later step whose buffers were changed between calcs by other code (not the previous calc's
 * result) drops the anchor for the rest of the frame (the per-step form). dt 1: no-ops. Host memory only (a copy of
 * the 0x2A joints' J3DTransformInfo / Quaternion buffers). */
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "true60.h"
#include "m_Do/m_Do_ext_animation.h"

namespace t60OldAnchor {
static const uint32_t kTransSize = 0x540, kQuatSize = 0x2A0; /* new J3DTransformInfo[0x2A], new Quaternion[0x2A] */
struct State {
    uint32_t stamp, ea, ti, q;
    bool valid;
    uint8_t start[kTransSize + kQuatSize], post[kTransSize + kQuatSize];
};
inline State& state() {
    static State s;
    return s;
}
/* A's calc rate for this frame, as the original's decOldFrameMorfCounter computes it from the remaining weights
 * (m_Do_ext_animation.cpp oldMorfStepRate's rho) */
inline f32 frameRho(mDoExtAnimation::mDoExt_MtxCalcOldFrame* self) {
    f32 one = 1.0f, zero = 0.0f, old = self->mOldFrameMorfRemainOld, value = self->mOldFrameMorfRemain;
    f32 rho = zero;
    if (old > zero) {
        f32 ratio = gabi::fsubs_ppc(old, value) / old;
        rho = gabi::fsubs_ppc(one, ratio);
    }
    if (!(self->mOldFrameMorfCounter > zero) && !(value > zero))
        rho = zero;
    return rho;
}
inline void copyOut(uint8_t* dst, uint32_t ti, uint32_t q) {
    memcpy(dst, gabi::at<uint8_t>(ti), kTransSize);
    memcpy(dst + kTransSize, gabi::at<uint8_t>(q), kQuatSize);
}
inline bool same(const uint8_t* src, uint32_t ti, uint32_t q) {
    return !memcmp(src, gabi::at<uint8_t>(ti), kTransSize) && !memcmp(src + kTransSize, gabi::at<uint8_t>(q), kQuatSize);
}
}  // namespace t60OldAnchor

/* (nest pair frame 34; the first-step input rule): the calc weight at a change between the
 * stepped and the held form. Link's tick30 dec leaves mOldFrameRate for the NEXT frame's first calc: rho^(1/N) at
 * dt < 1 (oldMorfStepRate), rho at dt 1. A frame whose form differs from the previous one's took the other frame's
 * weight: the first held frame (moving-BG hold, 30 Hz execute at dt 1; 34) blended with rho^(1/2) against A's
 * rho (Link's pose part-way to the old frame: his co cylinder 0.47 off, the nest's push 4.5765 / 0.0593 against
 * 4.6000 / 0.0380), and the first stepped frame after a hold its first step with rho for rho^(1/N). The weight is put
 * to the frame's form from the remaining weights (frameRho, the dec's own formula): rho for a 30 Hz execute at
 * N > 1, rho^(1/N) on a first step. Same value as before whenever the form did not change. N = 1: never.
 * Negative control WWHD_T60_OLDRATE_NEGCTRL=1 (game build): the weight as left. */
/* M2g (pot potP4 296, Link z 1.542): the same at a frame of N = 1 after a stepped frame (the pot
 * route's lava damage demo: frames 294 at N 4, 295 at N 1, the old-frame morph of procDemoLavaDamage's init running):
 * the last stepped execute's tick30 dec left rho^(1/4) (0.9036 for A's 0.6667) and the N = 1 frame's one calc blended
 * with it (the root 1.80 off at the record after 295, Link's root move 1.542 at 296). Form: a frame of N = 1 whose
 * previous Link execute ran at dt < 1 takes rho (A's weight); an N = 1 run never has one (dt-1 check, the 30 Hz game).
 * Negative control WWHD_T60_OLDRATE_N1_NEGCTRL=1 (game build). */
inline void mDoExt_t60_oldFrameRateForm(mDoExtAnimation::mDoExt_MtxCalcOldFrame* self) {
    static const bool negctrl = [] { const char* e = getenv("WWHD_T60_OLDRATE_NEGCTRL"); return e && e[0] == '1'; }();
    static const bool negctrlN1 = [] { const char* e = getenv("WWHD_T60_OLDRATE_N1_NEGCTRL"); return e && e[0] == '1'; }();
    static f32 s_prevDt = 1.0f;
    const f32 prevDt = s_prevDt;
    s_prevDt = (f32)t60::dt();
    if (!negctrl && !negctrlN1 && t60::steps() <= 1 && prevDt != 1.0f && (f32)self->mOldFrameRate > 0.0f) {
        const f32 rho = t60OldAnchor::frameRho(self);
        if (rho > 0.0f)
            self->mOldFrameRate = rho;
        return;
    }
    if (negctrl || t60::steps() <= 1 || !((f32)self->mOldFrameRate > 0.0f))
        return;
    const f32 rho = t60OldAnchor::frameRho(self);
    if (!(rho > 0.0f))
        return;
    if (t60::dt() == 1.0f)
        self->mOldFrameRate = rho;
    else if (t60::first_step())
        self->mOldFrameRate = (f32)std::pow((double)rho, 1.0 / (double)t60::steps());
}

/* before Link's model calc */
inline void mDoExt_t60_oldFrameCalcBegin(void* p) {
    using namespace t60OldAnchor;
    if (p != nullptr)
        mDoExt_t60_oldFrameRateForm(static_cast<mDoExtAnimation::mDoExt_MtxCalcOldFrame*>(p));
    if (t60::dt() == 1.0f || p == nullptr)
        return;
    auto* self = static_cast<mDoExtAnimation::mDoExt_MtxCalcOldFrame*>(p);
    State& s = state();
    const uint32_t now = t60_step_counter(), me = gabi::ea(self), ti = self->mOldFrameTransInfo,
                   q = self->mOldFrameQuaternion, n = t60::steps();
    if (ti == 0 || q == 0) {
        s.valid = false;
        return;
    }
    if (t60::first_step()) {
        copyOut(s.start, ti, q);
        s.stamp = now;
        s.ea = me;
        s.ti = ti;
        s.q = q;
        s.valid = true;
        return; /* k = 1: rho^(1/N), the field as it stands */
    }
    if (!s.valid || s.ea != me || s.ti != ti || s.q != q || now - s.stamp >= n || !same(s.post, ti, q)) {
        s.valid = false; /* no first-step record / changed between calcs: the per-step form */
        return;
    }
    if (!((f32)self->mOldFrameRate > 0.0f))
        return; /* no blend on this step */
    const f32 rho = frameRho(self);
    if (!(rho > 0.0f))
        return;
    const uint32_t k = now - s.stamp + 1;
    memcpy(gabi::at<uint8_t>(ti), s.start, kTransSize);
    memcpy(gabi::at<uint8_t>(q), s.start + kTransSize, kQuatSize);
    self->mOldFrameRate = k >= n ? rho : (f32)std::pow((double)rho, (double)k / (double)n);
}

/* after Link's model calc: the calc's result (the next step's check) and the field's rho^(1/N) back */
inline void mDoExt_t60_oldFrameCalcEnd(void* p) {
    using namespace t60OldAnchor;
    if (t60::dt() == 1.0f || p == nullptr)
        return;
    auto* self = static_cast<mDoExtAnimation::mDoExt_MtxCalcOldFrame*>(p);
    State& s = state();
    if (!s.valid || s.ea != gabi::ea(self))
        return;
    copyOut(s.post, s.ti, s.q);
    if ((f32)self->mOldFrameRate > 0.0f) {
        const f32 rho = frameRho(self);
        self->mOldFrameRate = rho > 0.0f ? (f32)std::pow((double)rho, 1.0 / (double)t60::steps()) : 0.0f;
    }
}
