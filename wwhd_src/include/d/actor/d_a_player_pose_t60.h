/* d_a_player_pose_t60.h (WWHD, true 60 build; 2026-11-24).
 * Decomp game build only. Header only (rule: no new functions or
 * function-local statics in the glued unit d_a_player_main_04.cpp; the unit keeps two call lines).
 *
 * daPy_lk_c::execute calcs the body model (mpCLModel->calc) near its end; everything before the calc reads the joint
 * matrices of the LAST calc and the words execute derives from them after it (eyePos, mHeadTopPos, the hands, the
 * boomerang / hookshot roots, the sword top / root and their old copies): footBgCheck and posMoveFromFootPos (the feet),
 * setNeckAngle / setHatAngle (head, HATA), the procs (eye, head top, hands, sword). In A frame F's execute reads frame
 * F-1's calc. At N > 1 the frame's first step reads the previous tick30 step's calc (= A's), but a later step reads the
 * calc of the step before it (half a frame ahead at N 2), and the angles it derives (neck, hat, waist, the leg IK) feed
 * the frame's calc through jointBeforeCB: the bomb helper's bt route with B switched in at its own origin (btO17n / m,
 * WWHD_T60_AT_ORIGIN): Link's words equal to A up to the switch-in, 40 joint-derived words off from its first frame
 * (mHatPrevPos, mHeadFrontAngleX, mFootMoveDist, the sword's old top, the colliders), the animation frame equal.
 *
 * Form (the process-order read rule frame-start reads / whole-frame form whole form): the frame-start pose view. The frame's first step keeps, at the
 * start of execute, the body model's joint matrices and those derived words as they are (the previous frame's last calc
 * = A's frame start; host bytes, stamped with the step counter); a later step of the frame writes them back at the same
 * place: the whole pre-calc part of every step reads A's frame-start pose. (Kept at the start, not after the tick30
 * calc: no reads at N 1 / dt 1, dt-1 check untouched (review a73548a3), and the switch-in frame after an N 1 frame is covered.) The calc
 * then overwrites the matrices and the post-calc code rewrites the words, so the draw, the draw history, the collision
 * registration and the attention position stay the step's own (nothing is put back). The partial forms that already
 * read the frame start (footBgCheck's d_a_player_footbg_t60.h records / pose, posMoveFromFootPos's first-step chord)
 * read the same pose through it. Guest memory through gabi load / store (the verify harness maps no host pointers).
 * Not in the view: models whose bases execute sets from the CL joints after the calc (katsura, equip / shield, boots):
 * no pre-calc reader of them in execute. dt 1 / N 1: no reads, no writes (the original). NC: WWHD_T60_LINK_POSE_VIEW_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace lkT60Pose {
static const bool s_nc = [] {
    const char* e = getenv("WWHD_T60_LINK_POSE_VIEW_NEGCTRL");
    return e != nullptr && atoi(e) != 0;
}();
/* the words execute derives from the calc (daPy_lk_c / daPy_py_c offsets, d_a_player.h / d_a_player_main.h) */
struct Word { uint32_t off, len; };
static const Word kWords[] = {
    {0x37C, 12},  /* eyePos */
    {0x3D8, 12},  /* mHeadTopPos */
    {0x3E4, 12},  /* mSwordTopPos */
    {0x3F0, 12},  /* mLeftHandPos */
    {0x3FC, 12},  /* mRightHandPos */
    {0x72CC, 12}, /* mSwordRootPos */
    {0x72D8, 12}, /* mOldSwordTopPos */
    {0x72E4, 12}, /* mOldSwordRootPos */
    {0x72F0, 12}, /* mHookshotRootPos */
    {0x72FC, 12}, /* mBoomerangCatchPos */
};
static const uint32_t kWordBytes = 12 * 10;
static const uint32_t kMaxJoints = 64;
struct Keep {
    uint32_t stamp;
    uint32_t mtx, n;  /* the joint matrix block it came from (guest address, joint count) */
    uint8_t mats[kMaxJoints * 0x30];
    uint8_t words[kWordBytes];
    bool valid;
};
static Keep s_keep;
static inline void rd(uint8_t* dst, uint32_t ea, uint32_t n) {
    for (uint32_t i = 0; i < n; i++)
        dst[i] = gabi::load<uint8_t>(ea + i);
}
static inline void wr(uint32_t ea, const uint8_t* src, uint32_t n) {
    for (uint32_t i = 0; i < n; i++)
        gabi::store<uint8_t>(ea + i, src[i]);
}
/* the body model's joint matrix block (J3DModel +0x2C joints: +0x10 matrices, +0x2C count; as the port reads it) */
static inline bool block(uint32_t model, uint32_t* mtx, uint32_t* n) {
    if (model == 0)
        return false;
    const uint32_t jnt = gabi::load<uint32_t>(model + 0x2C);
    if (jnt == 0)
        return false;
    *mtx = gabi::load<uint32_t>(jnt + 0x10);
    *n = gabi::load<uint16_t>(jnt + 0x2C);
    return *mtx != 0 && *n != 0 && *n <= kMaxJoints;
}
/* execute's start (after the get-demo early return), N > 1 only (dt 1: no reads at all, the original):
 * - the frame's first step: its live pose IS the frame start (the previous frame's last calc, A's T(f-1), also after an
 *   N 1 frame: the switch-in frame is covered): keep the matrices and words;
 * - a later step of the frame: write the first step's keep back, so the pre-calc part reads the frame start */
static inline void frameStart(uint32_t self) {
    if (s_nc || t60::dt() == 1.0f || t60::steps() <= 1) {
        return;
    }
    const uint32_t clModel = gabi::load<uint32_t>(self + 0x448); /* daPy_lk_c::mpCLModel (read only here: no N 1 read) */
    uint32_t mtx, n;
    if (t60::first_step()) {
        s_keep.valid = false;
        if (!block(clModel, &mtx, &n)) {
            return;
        }
        rd(s_keep.mats, mtx, n * 0x30);
        uint32_t at = 0;
        for (const Word& w : kWords) {
            rd(s_keep.words + at, self + w.off, w.len);
            at += w.len;
        }
        s_keep.mtx = mtx;
        s_keep.n = n;
        s_keep.stamp = t60_step_counter();
        s_keep.valid = true;
        return;
    }
    if (!s_keep.valid || t60_step_counter() - s_keep.stamp >= t60::steps()) {
        return; /* (no keep from this frame's first step: Link entered on a later step, or the first step returned early) */
    }
    if (!block(clModel, &mtx, &n) || mtx != s_keep.mtx || n != s_keep.n) {
        return;
    }
    wr(mtx, s_keep.mats, n * 0x30);
    uint32_t at = 0;
    for (const Word& w : kWords) {
        wr(self + w.off, s_keep.words + at, w.len);
        at += w.len;
    }
}
/* posMoveFromFootPos's raw foot travel (the step without a frame chord: a foot-data reset, an entry): the toe's move
 * since the foot record of the previous step. Under the view the frame's first step measures the whole previous frame
 * (the last record is the previous frame's frame-start pose) and a later step measures nothing (its pose is the frame
 * start again), so the per-step form travel / dt would give 2 x and 0 at N 2: the first step's travel is the frame's
 * (per frame, no / dt) and the frame's later steps take it again. Without the view (NC, dt 1): travel / dt as before.
 * A later step that falls to the raw branch while the first step had a chord (no latch this frame) measures a pose that
 * no longer moves: travel / dt = 0 at N 2 (accepted: the chord path is the normal one). */
static f64 s_footRaw;
static uint32_t s_footRawStamp;
static bool s_footRawValid;
static inline f64 footRaw(f64 travel, f32 dt) {
    if (s_nc || dt == 1.0f) {
        return dt == 1.0f ? travel : travel / (f64)dt;
    }
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        s_footRaw = travel;
        s_footRawStamp = now;
        s_footRawValid = true;
        return travel;
    }
    if (s_footRawValid && now - s_footRawStamp < t60::steps()) {
        return s_footRaw;
    }
    return travel / (f64)dt;
}
/* (2026-11-25, long1 seg 7 frame 7642, the Atorizk exit): the first 30 Hz
 * fallback frame (dt 1) after converted frames took posMoveFromFootPos's travel toe - mFootData.mToePos with the toe
 * the last step stored (the tick30 step's view, half a frame into the previous frame's pose at N 2: travel 14.72 /
 * A 12.52, speedF 13.52 / 12.51). A's stored toe is the previous frame's frame-start one (its one execute): the toe
 * ring's entry of the previous frame's first step. Form: that frame's travel is taken from the ring entry (the frame
 * start of the converted frame right before). NC WWHD_T60_FALLBACK_CHORD_NEGCTRL=1. N steps: not run. */
static inline bool fallbackChordOn() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] {
        const char* e = getenv("WWHD_T60_FALLBACK_CHORD_NEGCTRL");
        return !(e && e[0] == '1');
    }();
    return v;
#else
    return false;
#endif
}
} // namespace lkT60Pose
