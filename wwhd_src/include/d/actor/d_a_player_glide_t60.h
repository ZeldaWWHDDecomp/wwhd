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
#include <cstdlib>

namespace lkT60Glide {
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
