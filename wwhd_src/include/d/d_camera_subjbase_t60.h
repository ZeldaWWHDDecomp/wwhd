/* d_camera_subjbase_t60.h (WWHD, true 60 build; 2026-11-25). Decomp build only.
 * Separate file (no new statics in the glued unit).
 *
 * The subject (first-person aim) camera's steady view yaw is Link's facing minus the aim base m3BA (subjectCamera's
 * else branch: angU = shape_angle.y - m3BA), and m3BA is written by the player's CalcSubjectAngle call (Link's
 * setBodyAngleToCamera) as Link's facing before that call's turn. In A that call runs once a frame, so m3BA is the
 * frame start's facing and the camera's yaw offset is the frame's whole aim turn. At N steps every converted Link step
 * calls it, so on the tick30 step (and in the camera's whole run) m3BA was the facing before the last
 * step only: the view took 1/N of the frame's turn (boomerang aim route bmA at N 2: view yaw U at frame 77 B
 * -18021 vs A -18100 under the right stick; camera 17.3 @96 with Link exact). Form: the frame's first player call
 * records its base (per camera body); the camera's steady branch measures the offset from it on every step of that
 * frame (the frame's turn so far on the earlier steps, the whole turn on the tick30 step: A's). The player's own call is
 * unchanged (it still turns from its own facing each step). NC WWHD_T60_SUBJBASE_NEGCTRL=1: the live m3BA, as before.
 * N 1: never consulted. */
#pragma once
#include <cstdint>
#include <cstdlib>
#include "true60.h"
#include "d/d_camera_key_t60.h"
#include "d/d_camera_run_t60.h"

namespace subject_t60 {
struct Base {
    uint32_t stamp; /* t60_step_counter of the frame's first player call */
    int16_t base;   /* m3BA it wrote */
    bool valid;
};
inline camera_key_t60::PerCam<Base> g_base;
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_SUBJBASE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* the camera body's slot (Link's call runs outside camera_execute: looked up, not claimed); -1: none */
static inline int slotOf(uint32_t camEa) {
    for (int i = 0; i < camera_key_t60::kCams; i++)
        if (camera_key_t60::g_ea[i] == camEa)
            return i;
    return -1;
}
/* CalcSubjectAngle after it set m3BA: the player's call on the frame's first step records it */
static inline void noteBase(uint32_t camEa, bool cameraCall, int16_t base) {
    if (t60::steps() <= 1 || cameraCall || !t60::first_step() || nc())
        return;
    const int s = slotOf(camEa);
    if (s < 0)
        return;
    const int saved = camera_key_t60::g_cur;
    camera_key_t60::g_cur = s;
    g_base() = Base{t60_step_counter(), base, true};
    camera_key_t60::g_cur = saved;
}
/* subjectCamera's steady branch: the frame's base when the frame's first player call recorded one */
static inline bool frameBase(uint32_t camEa, int16_t* out) {
    if (t60::steps() <= 1 || nc())
        return false;
    const int s = slotOf(camEa);
    if (s < 0)
        return false;
    const int saved = camera_key_t60::g_cur;
    camera_key_t60::g_cur = s;
    const Base b = g_base();
    camera_key_t60::g_cur = saved;
    if (!b.valid || t60_step_counter() - b.stamp >= (uint32_t)t60::steps())
        return false;
    *out = b.base;
    return true;
}
/* (2026-11-25; the rope route at N 4): the pitch share
 * m388 (SubjectWork +0x10, guest obj +0x38C) is an accumulator; every player call (Link's setBodyAngleToCamera, each
 * converted step) adds pitch x k x dt to it, the camera's call its own share. In the follow-whole form the frame-start
 * copy is taken at the camera's step-1 Run (after Link's step-1 call) and the tick30 restore puts it back before Link's
 * tick30 call: the player's adds of steps 2 .. N-1 were dropped (none at N 2; at N 4 half a frame of the stick's pitch:
 * rp frame-end pitch V B -114 / -291 / -502 vs A -229 / -584 / -1008 from the hook aim's first frame, camera 315.4 @119).
 * Form: the player's m388 change on a call made after this frame's frame-start copy and before the tick30 step is
 * summed (per camera body) and added back (clamped to +-1 as the call clamps) right after the restore. The camera's own
 * calls and the tick30 step's calls are not carried (the restore takes the picture steps' camera share back; the whole
 * run makes the frame's). NC WWHD_T60_SUBJCARRY_NEGCTRL=1: dropped, as before. N 1: never. */
struct Carry {
    uint32_t stamp; /* the frame-start copy's stamp the sum belongs to */
    float d388;
};
inline camera_key_t60::PerCam<Carry> g_carry;
static inline bool carryNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_SUBJCARRY_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
/* CalcSubjectAngle end: the player's call's m388 change (pre / post); snapStamp / snapValid: this camera's frame-start
 * copy (camera_run_t60::g_runFrameStart, read by the caller in the camera's slot) */
static inline void noteCarry(uint32_t camEa, bool cameraCall, float pre, float post) {
    if (t60::steps() <= 1 || cameraCall || t60::tick30() || carryNc() || pre == post)
        return;
    const int s = slotOf(camEa);
    if (s < 0)
        return;
    const int saved = camera_key_t60::g_cur;
    camera_key_t60::g_cur = s;
    const camera_run_t60::RunFrameStart& F = camera_run_t60::g_runFrameStart();
    if (F.valid && t60_step_counter() - F.stamp < (uint32_t)(t60::steps() - 1)) {
        Carry& c = g_carry();
        if (c.stamp != F.stamp)
            c = Carry{F.stamp, 0.0f};
        c.d388 += post - pre;
    }
    camera_key_t60::g_cur = saved;
}
/* right after a whole-form restore (current camera slot = this camera's): the carried player share back on m388 */
static inline void applyCarry(uint32_t objEa) {
    if (t60::steps() <= 1 || carryNc())
        return;
    Carry& c = g_carry();
    const camera_run_t60::RunFrameStart& F = camera_run_t60::g_runFrameStart();
    if (!F.valid || c.stamp != F.stamp)
        return;
    float v = gabi::load<float>(objEa + 0x38C) + c.d388;
    if (v > 1.0f)
        v = 1.0f;
    else if (v < -1.0f)
        v = -1.0f;
    gabi::store<float>(objEa + 0x38C, v);
    c = Carry{0xFFFFFFFFu, 0.0f};
}
} // namespace subject_t60
