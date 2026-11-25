/* d_a_player_grabpic_t60.h (WWHD, true 60 build, 2026-11-23): the
 * carried item's bounce picture for setGrabItemPos (d_a_player_main_04.cpp, its only includer: the host statics and the
 * exported query live in that one unit). Decomp build only. Separate file
 * (the autoglue takes every free function of a glued .cpp for a decomp function, 2026-11-23):
 * the actor source keeps a thin call. */
#pragma once
#include <cstdint>
#include <cstdlib>
#include "true60.h"

/* the negative control WWHD_T60_GRABPIC_NEGCTRL=1: the picture at the frame-start bounce */
static inline bool lk04_grabPicNegctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_GRABPIC_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}

/* true60 (d/t60_held_pic.h t60hp::grab_dy): the carried item's bounce as a picture on a frame's
 * earlier steps. The logic bounce (above) changes once a frame on the tick30 step, so on the earlier
 * steps the item rides at the frame-start bounce and its height against Link's hands moved on tick30 passes only (stone
 *, Link falling off the ledge: + 0.27 / 0.77 / 1.27 ... per tick30 pass up to the cap 40, nothing between).
 * Picture: on an earlier step the per-step form (0.2 x dt x -speed.y of this step, capped at 40; the chase of 5 x dt;
 * 0 for a worn barrel) runs on a host shadow from the frame-start bounce; its difference to the logic bounce is told to
 * the carried actor's held picture (added to its current.pos.y around its matrix step only, the word put back). No
 * guest word changes. dt 1: never. NC WWHD_T60_GRABPIC_NEGCTRL=1 (the picture at the frame-start bounce). */
#if defined(GABI_GAME_BUILD) && GABI_T60
namespace lk04_grabpic {
static uint32_t s_actor = 0, s_stamp = ~0u;
static float s_val = 0.0f, s_dy = 0.0f;
}  // namespace lk04_grabpic
static inline void lk04_grabPicNote(uint32_t actor, float bounce, bool wear, bool midair, float spdY) {
    using namespace lk04_grabpic;
    const uint32_t now = t60_step_counter();
    if (lk04_grabPicNegctrl()) {
        s_actor = 0;
        return;
    }
    float s = (!t60::first_step() && s_actor == actor && s_stamp + 1 == now) ? s_val : bounce;
    const float dt = t60::dt();
    if (wear) {
        s = 0.0f;
    } else if (midair && spdY < 0.0f) {
        s = s - 0.2f * dt * spdY;
        if (s > 40.0f)
            s = 40.0f;
    } else {
        const float c = 5.0f * dt;
        s = s > 0.0f ? (s - c > 0.0f ? s - c : 0.0f) : (s + c < 0.0f ? s + c : 0.0f);
    }
    s_actor = actor;
    s_stamp = now;
    s_val = s;
    s_dy = s - bounce;
}
/* the picture's height change for `actor` on this step (0: none, another actor, another step) */
extern "C" __attribute__((visibility("default"))) float wwhd_t60_grab_pic_dy(uint32_t actor) {
    using namespace lk04_grabpic;
    return (actor != 0 && actor == s_actor && s_stamp == t60_step_counter() && !t60::tick30()) ? s_dy : 0.0f;
}
#else
static inline void lk04_grabPicNote(uint32_t, float, bool, bool, float) {}
#endif

/* carry placement, worn barrel (frame 248 -> 249): the worn ease (setGrabItemPos: mGrabItemOffsetY +=
 * m35D8, addCalc toward 0 / -115 / -85 by 0.5 with steps 1 .. 20, the part past -85 into m35D8, which lowers Link's body
 * chain in the calc) is a once-a-frame chase with a minimum step: A at 248 moved -86.875 -> -85.875 (half the gap 0.94,
 * the minimum 1: m35D8 -0.875), B's per-step chase 0.549 + 0.5 (m35D8 -0.8258), the next frame's calc 0.049 off. Form:
 * an earlier step holds the ease (mGrabItemOffsetY / m35D8 as the frame started; the worn barrel's place on it is the
 * held picture's), the tick30 step runs it once at dt 1 from the frame-start values (the original arithmetic). dt 1: the
 * original. NC WWHD_T60_GRABEASE_NEGCTRL=1 (the per-step form). */
static inline bool lk04_grabEaseNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_GRABEASE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
static inline bool lk04_grabEaseRun() { return t60::dt() == 1.0f || t60::tick30() || lk04_grabEaseNc(); }
static inline f32 lk04_grabEaseBegin() {
    if (t60::dt() == 1.0f || lk04_grabEaseNc())
        return -1.0f;
    return t60_set_dt(1.0f);
}
static inline void lk04_grabEaseEnd(f32 prev) {
    if (prev >= 0.0f)
        t60_set_dt(prev);
}
