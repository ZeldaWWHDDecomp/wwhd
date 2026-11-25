/* d_camera_bush_t60.h (WWHD, true 60 build; 2026-11-25). Decomp build only.
 * Separate file: d_camera_bg.cpp keeps a thin call.
 *
 * long3 N 2 seg 3 (the sea 28 warp at t95, Link standing at the hammer spot, style 90 in the follow whole form): camera
 * eye 5.1 off at 3000 with Link exact; the camera yaw then went into Link's camera-relative MOVE heading at 3272 (-8100
 * vs -8092) and the heavy walk ended 2.06 off at 3348. checkGroundInfo's bush test (lineCollisionCheckBush: the eye /
 * attention segment against the CC mass list, the grass and bushes' camera colliders; then GetMassCamTopPos) sets
 * mBushHit / mBushHeight / mBushTopPos, and bumpCheck's CheckBush eases the eye's latitude toward the bush top (A: +0.27,
 * +0.25 ... a frame). The mass list is filled once per 30 Hz frame and emptied after the frame's first CC pass, so only
 * the frame's first step finds it as A's one Run does: B's first step eased (708.13), the tick30 step's run (the whole
 * run from the frame start) found no bush and put the eye back on 707.75, every frame.
 * Form: the frame's first step notes its bush result; the frame's later steps (the whole run on the tick30 step
 * included) take that result instead of their own test (the same frame-start eye and Link: A's input). NC
 * WWHD_T60_CAMBUSH_NEGCTRL=1. dt 1 (N = 1): never. */
#pragma once
#include "true60.h"
#include <cstdlib>

namespace camera_bush_t60 {
struct BushRec {
    uint32_t stamp; /* t60_step_counter of the frame's first step */
    s32 hit;
    f32 height;
    f32 top[3];
    bool valid;
};
inline BushRec g_bushRec;
static inline bool off() {
    static int s = -1;
    if (s < 0) {
        const char* e = getenv("WWHD_T60_CAMBUSH_NEGCTRL");
        s = (e && e[0] == '1') ? 1 : 0;
    }
    return s == 1;
}
/* after checkGroundInfo's bush block (player on the floor): note it on the frame's first step, take the note on later
 * steps; true = the fields were replaced */
static inline bool frame(be<s32>& hit, be<f32>& height, cXyz& top) {
    if (t60::steps() <= 1 || off())
        return false;
    BushRec& r = g_bushRec;
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        r.stamp = now;
        r.hit = hit;
        r.height = height;
        r.top[0] = top.x, r.top[1] = top.y, r.top[2] = top.z;
        r.valid = true;
        return false;
    }
    if (!r.valid || now - r.stamp >= (uint32_t)t60::steps())
        return false;
    hit = r.hit;
    height = r.height;
    top.x = r.top[0], top.y = r.top[1], top.z = r.top[2];
    return true;
}
/* the player is off the floor on this step: no note for the frame's later steps */
static inline void clear() {
    if (t60::first_step())
        g_bushRec.valid = false;
}
} // namespace camera_bush_t60
