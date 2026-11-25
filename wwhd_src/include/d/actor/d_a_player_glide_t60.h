/* d_a_player_glide_t60.h (WWHD, true 60 build; 2026-11-23): the Deku Leaf
 * glide's start frame on the frame grid. Decomp game build only. Header only
 * (rule: no new functions or function-local statics in the glued unit d_a_player_main_07.cpp).
 *
 * daPy_lk_c::procFanGlide waits for the opening animation's frame to pass 2, then starts the glide once (mProcVar0 <= 0:
 * mNormalSpeed 12, speed.y 15, mProcVar0 = 1) and returns: the 30 Hz frame of the start is the start only, the glide
 * proper (speed / turn chases, wind, lean) begins with the next frame. At N 2 the frame passes 2 on the frame's earlier
 * step (2.25; A reads 2.5 on that frame), so the start ran there and the frame's tick30 step ran the glide proper as well:
 * one glide frame early (A 3617 / B 3616 tick30: speed 12 / 11.75, turn 0 / -12, mProcVar2..4 at compare
 * frame 104 0 / -25, 96, -152; the speed slope a frame early at 108 / 109). The start itself stays on the step that sees
 * the frame pass 2 (its position matches A's frame end: the frame's later steps integrate the start's speed); the frame's
 * later steps return after the proc's per-step head, as A's start frame does. dt 1: the original.
 * Two more frame-grid parts of the glide proper (the same route after the start fix: A 3618 turn -45 / B -35, speed.y
 * 2.5 -> 0 in A / 2.5 -> 1 in B at 3622):
 * - the turn speed mProcVar2 (cLib_addCalcAngleS toward the stick's turn) was chased on every step (twice a frame at N 2)
 *   while lk07_share adds dt of it per step: frame-exact chase (LKT60_CHASE_S site 11, the form of the speed chases:
 *   the frame's first step chases the whole frame with the frame-start stick and facing, the later steps hold);
 * - the glide's gravity switch (speed.y < -gravity: gravity -0.5) tests the frame-start speed.y in A (the proc runs before
 *   posMove's integration); on a later step it saw a half-integrated speed and switched a frame early: the test on the
 *   frame's first step only (gravityCheck).
 * - the turn's move (glF7): A adds the frame's turn mProcVar2 to shape_angle / current.angle and posMove moves the frame
 *   along the new heading; per step the turn is shared (lk07_share, the picture) and the first step moved along a
 *   half-turned heading (x 0.013 off at A 3618, 49 units at frame 154). The move's heading current.angle.y takes the
 *   frame's whole turn on the frame's first step and keeps it (moveAngle); shape_angle.y, the picture, keeps the
 *   per-step share; both meet at the frame's end.
 * NC: WWHD_T60_GLIDE_START_NEGCTRL=1 (all four as before: the start frame's later steps run the glide proper, the turn
 * chased per step, the gravity test on every step, the heading shared per step). */
#pragma once
#include "true60.h"
#include "d/actor/d_a_player_t60.h"
#include "d/actor/d_a_player_anm_t60.h"
#include <cstdlib>

namespace lkT60Glide {
/* (2026-11-25, glide pair: mProcVar6, the glide's in-wind flag, set
 * a frame early at 161, then Link 21.6 / camera 18.2 off at 299-300): procFanGlide's wind test (lk07_windTgHit,
 * main_07) reads his wind cylinder's tg hit. A's frame sees one collision pass, the previous frame end's; the frame's
 * first step reads the same pass, but a later step reads the pass after the first step (his place / the wind's mid
 * frame), and a hit first seen there switched the glide to the wind animation and its push a frame before A. Form:
 * a later step answers as the frame's first step did (a hit held from it, as before; a new hit not: the next frame's
 * first step reads it from the frame end's pass, as A). NC WWHD_T60_GLIDE_WINDHIT_NEGCTRL=1. */
/* (2026-11-25, glide pair frame 300: Link 21.6 / camera 18.2 off):
 * procFanGlide turns his glide speed back (mNormalSpeed = -mNormalSpeed) when his ground check's wall hit faces
 * him. A's frame tests the previous frame end's wall hit once; at N 2 the first step turned it back (as A), and the
 * tick30 step read the wall hit of the first step's own check (still at the wall after a half move) and turned it
 * back again: Link stayed at the wall where A bounced off. Form: the test runs on the frame's first step only (a hit
 * first seen on a later step is the next frame's, from the frame end's check, as A). NC
 * WWHD_T60_GLIDE_WALLBOUNCE_NEGCTRL=1. */
static inline bool wallStep() {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_GLIDE_WALLBOUNCE_NEGCTRL"); return e && e[0] == '1'; }();
    return nc || t60::steps() <= 1 || t60::first_step();
}
static inline bool windHitNc() {
    static const bool v = [] { const char* e = getenv("WWHD_T60_GLIDE_WINDHIT_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
}
static const bool s_nc = [] {
    const char* e = getenv("WWHD_T60_GLIDE_START_NEGCTRL");
    return e != nullptr && atoi(e) != 0;
}();
static bool s_valid = false;
static uint32_t s_tag = 0;
/* the glide starts on this step (procFanGlide's mProcVar0 <= 0 branch) */
static inline void start() {
    if (t60::dt() == 1.0f || s_nc) {
        s_valid = false;
        return;
    }
    s_valid = true;
    s_tag = lkT60_frameTag();
}
/* a later step of the 30 Hz frame the glide started in: the frame is the start only */
static inline bool startFrame() {
    if (t60::dt() == 1.0f || s_nc || !s_valid) {
        return false;
    }
    if (s_tag != lkT60_frameTag()) {
        s_valid = false;
        return false;
    }
    return true;
}
/* the gravity switch's test runs on this step (dt 1, the frame's first step, or the NC) */
static inline bool gravityCheck() { return t60::dt() == 1.0f || s_nc || t60::first_step(); }
static bool s_angleValid = false;
static uint32_t s_angleTag = 0;
static s16 s_angle = 0;
/* the move's heading on this step: `whole` (the frame-start facing + the frame's whole turn) from the frame's first step
 * on, else the shared picture angle `shared` (dt 1, the NC, a glide entered on a later step of the frame) */
static inline s16 moveAngle(s16 shared, s16 whole) {
    if (t60::dt() == 1.0f || s_nc) {
        return shared;
    }
    if (t60::first_step()) {
        s_angleValid = true;
        s_angleTag = lkT60_frameTag();
        s_angle = whole;
        return whole;
    }
    if (s_angleValid && s_angleTag == lkT60_frameTag()) {
        return s_angle;
    }
    return shared;
}
} // namespace lkT60Glide

/* (2026-11-24): procFanSwing's recoil (the swing's frame 22: mNormalSpeed 10 back
 * along the swing, the gust's start) tests the swing animation's frame against 22 once a frame in A (the frame-end
 * frame: A plays before the dispatch). At N 2 the earlier step read 21.5 and the tick30 step 22: the recoil started on
 * the tick30 step and the frame moved half of A's 10 (route: z 5 short from A 3572, pos 10 at compare
 * frame 107). The test reads the frame-end frame (lkT60_endFrame, the frame-end form) on every step of the frame, and the
 * recoil frame's later steps skip the slow-down chase (swingRecoilFrame). dt 1: the original. NC: WWHD_T60_FAN_GUST_NEGCTRL=1. */
namespace lkT60Glide {
static const bool s_gustNc = [] {
    const char* e = getenv("WWHD_T60_FAN_GUST_NEGCTRL");
    return e != nullptr && atoi(e) != 0;
}();
static inline f32 swingFrame(J3DFrameCtrl* fc) { return s_gustNc ? (f32)fc->getFrame() : lkT60_endFrame(fc); }
/* the recoil (mProcVar6 = 1) is A's whole frame: the frame's later steps do not start the slow-down (A: from the next
 * frame on; the chase's per-step form took 0.75 of it on the recoil frame's tick30 step) */
static bool s_recoilValid = false;
static uint32_t s_recoilTag = 0;
static inline void swingRecoil() {
    s_recoilValid = t60::dt() != 1.0f && !s_gustNc;
    s_recoilTag = lkT60_frameTag();
}
static inline bool swingRecoilFrame() {
    return s_recoilValid && t60::dt() != 1.0f && !s_gustNc && s_recoilTag == lkT60_frameTag();
}
} // namespace lkT60Glide
