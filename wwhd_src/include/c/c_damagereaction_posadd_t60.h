/* c_damagereaction_posadd_t60.h (WWHD, true 60 build, t60-pgp 2026-11-24, rule the same
 * day). Decomp build only. Separate file:
 * c_damagereaction.cpp keeps thin calls. The enemy-side twin of (d_a_player_posadd_t60.h).
 *
 * dr_damage_set moves the enemy once a frame in A: x = round(round(x + forward) + push), forward = speed.x / 4 + the
 * pushback (one f32 fmadds), push = the collision push m_cc_move (when the action / mode take it); z the same. At N steps
 * every step adds its share (forward x dt, the speed-ramp position-gap rule (3)'s tick30 rest; the held push x dt, the first-step push rule), each add
 * rounded at the enemy's coordinate (|x| ~ 180: ulp 1.5e-5). The frame's displacement equals A's in value, the position
 * drifts by a few ulps a frame (moF4 Moblin 47C59A80 from frame 6, 0.001 by frame 42): its Co push on Link at frames 43-49
 * then differed by 0.0005 a frame and left Link 0.016 off for the rest of the route.
 * Form: on the frame's first step the frame-start xz and the frame's full pushback vector (A's, before x dt) are recorded;
 * a later step carries any change made to xz since this form's last write (wall / BG corrections between the steps) into
 * the frame start (written-vs-there, exact in double). The earlier steps keep their own adds (the picture). The tick30
 * step writes A's value in A's rounding order from that start: round(round(x0 + fmadds(speed.x, 0.25, pushback.x)) +
 * held push.x) (the push only when the step takes one, as A's condition); z the same; y is not touched (its own forms).
 * A frame whose first step did not move (the block skipped) keeps the per-step adds. Host memory keyed by the reaction's
 * guest address. dt 1: never. Negative control WWHD_T60_DRPOSADD_NEGCTRL=1: the per-step adds only. */
#pragma once
#include "true60.h"
#include <cstdlib>
#include <unordered_map>

namespace t60drPos {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_DRPOSADD_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
struct Entry {
    u32 frame;       /* 30 Hz frame tag of the record */
    double x0, z0;   /* the frame-start xz plus the corrections between the steps */
    f32 wx, wz;      /* the xz this form left after the last step's adds */
    f32 pbx, pbz;    /* the frame's full pushback vector (first step, before x dt) */
};
inline std::unordered_map<u32, Entry> s_entries;

/* before the step's forward add: x / z the position now, pbx / pbz the full pushback vector of this step (before x dt).
 * Returns the entry when the form runs this step. */
static inline Entry* begin(u32 key, u32 frame, f32 x, f32 z, f32 pbx, f32 pbz) {
    if (t60::dt() == 1.0f || nc())
        return nullptr;
    if (t60::first_step()) {
        Entry& e = s_entries[key];
        e.frame = frame;
        e.x0 = x, e.z0 = z;
        e.pbx = pbx, e.pbz = pbz;
        return &e;
    }
    auto it = s_entries.find(key);
    if (it == s_entries.end() || it->second.frame != frame)
        return nullptr;
    Entry& e = it->second;
    e.x0 += (double)x - (double)e.wx;
    e.z0 += (double)z - (double)e.wz;
    return &e;
}
/* after the step's adds (forward, then the push when taken): on the tick30 step x / z become A's frame write; every step
 * notes what it left. speedX / speedZ: the enemy's speed now (the tick30 speed = A's frame speed, the speed-ramp position-gap rule (3));
 * push: whether the step took the push, heldX / heldZ the frame's whole push (A's m_cc_move). */
static inline void end(Entry* e, u32 key, f32 speedX, f32 speedZ, bool push, f32 heldX, f32 heldZ, f32& x, f32& z) {
    if (e == nullptr)
        return;
    if (t60::tick30()) {
        const f32 fx = gabi::fmadds(speedX, 0.25f, e->pbx);
        const f32 fz = gabi::fmadds(speedZ, 0.25f, e->pbz);
        f32 ax = gabi::fadds_ppc((f32)e->x0, fx);
        f32 az = gabi::fadds_ppc((f32)e->z0, fz);
        if (push) {
            ax = gabi::fadds_ppc(ax, heldX);
            az = gabi::fadds_ppc(az, heldZ);
        }
        x = ax, z = az;
        s_entries.erase(key);
        return;
    }
    e->wx = x, e->wz = z;
}
} // namespace t60drPos
