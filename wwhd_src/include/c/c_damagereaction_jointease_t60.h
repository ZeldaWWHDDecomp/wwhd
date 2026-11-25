/* c_damagereaction_jointease_t60.h (WWHD, true 60 build, 2026-11-25). Decomp build
 * only. Separate file: c_damagereaction.cpp keeps thin
 * calls.
 *
 * dr_damage_anime eases the reaction's joint angles (mJntAngle[20], cLib_addCalcAngleS2 scale 2 toward mJntTargetAngle)
 * with a limit that follows the state: 0x3000 while launched, 0x400 otherwise. A's one call of the frame eases from the
 * frame start toward the frame-end state's targets under the frame-end state's limit. At N steps the state can change
 * between the steps: a long route (N 2), Bokoblin 47C5CE68 bounces on the tick30 step of frame 335 (launched on
 * the first step, landed on the tick30 step): the first step eased toward the airborne targets under 0x3000, the tick30
 * step half a frame under 0x400; joint 0 -15231 for A's -15334, joint 12 z -513 for A's -1025, both clamped at the limit
 * on the later frames, so the gap stayed (the eye 4.3 off, Link's lock-on shape 22 off at 336, the room fight's knock-back
 * headings after it). Form: the first step notes the frame-start joint angles and its limit; a tick30 step whose limit
 * differs (the state changed within the frame) puts the frame-start angles back and runs the ease loop as A's one call
 * (dt 1). Frames without a change keep the per-step eases. Host memory keyed by the reaction's guest address.
 * dt 1: never. Negative control WWHD_T60_DRJOINTEASE_NEGCTRL=1. */
#pragma once
#include "true60.h"
#include <cstdlib>
#include <unordered_map>

namespace t60drJoint {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_DRJOINTEASE_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
struct Entry {
    u32 frame;        /* 30 Hz frame tag of the note */
    s16 ang[20][3];   /* mJntAngle at the frame's first ease loop */
    s32 limit;        /* the first step's limit */
};
inline std::unordered_map<u32, Entry> s_entries;

/* before the ease loop. Returns true when this call is to run as A's one call (the caller sets dt 1 for the loop). */
template <class J>
static inline bool begin(u32 key, u32 frame, J* jntAngle, s32 limit) {
    if (t60::dt() == 1.0f || nc())
        return false;
    if (t60::first_step()) {
        Entry& e = s_entries[key];
        e.frame = frame;
        e.limit = limit;
        for (int i = 0; i < 20; i++) {
            e.ang[i][0] = jntAngle[i].x;
            e.ang[i][1] = jntAngle[i].y;
            e.ang[i][2] = jntAngle[i].z;
        }
        return false;
    }
    if (!t60::tick30())
        return false;
    auto it = s_entries.find(key);
    if (it == s_entries.end() || it->second.frame != frame)
        return false;
    const Entry& e = it->second;
    const bool changed = e.limit != limit;
    if (changed) {
        for (int i = 0; i < 20; i++) {
            jntAngle[i].x = e.ang[i][0];
            jntAngle[i].y = e.ang[i][1];
            jntAngle[i].z = e.ang[i][2];
        }
    }
    s_entries.erase(it);
    return changed;
}
} // namespace t60drJoint
