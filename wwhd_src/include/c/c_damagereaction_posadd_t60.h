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
/* (2026-11-25; long2 seg 3 at N 2, Bokoblin 47C5CE68 knocked back at speed.x
 * -90 into a wall at frames 226-237): A moves the enemy once a frame (dr_damage_set) and checks once (dr_body_bg_check:
 * mAcch.CrrPos, old.pos the frame start -> pos the frame start + the frame's move): the wall slides it along the whole
 * move. At N steps each step checked its own part: the first step's wall correction stayed in's frame
 * start (x0) and the tick30 check ran from the first step's place (old.pos = the first step's end): the slide ended
 * elsewhere (0.21 at 227, 1.08 at 236, 0.31 when it lands), the enemy stood 0.31 off, and Link's first Co push contact
 * with it at 287 differed (A 1.78 / 1.50, B 1.99 / 1.67): Link 0.27 off, 2.07 at the 675 damage. Form (the enemy twin of
 * Link's / speed-ramp position-gap (3) BG note): on the earlier steps of a frame this form runs, the body check's xz corrections
 * are noted (kx, kz); when an earlier step's body check ran, the tick30 step writes the frame start without them + A's
 * move (A's place before its check) and its body check runs on A's segment: old.pos = the first step's old.pos at its
 * check (the frame start), put back after the call; when an earlier step's check changed y (it landed), pos.y gets A's
 * pre-check state too: the height (the frame-start height + N x the first step's fall: the frame-start speed's whole
 * fall), speed.y at the check (the frame-start speed less the frame's whole gravity, x 0.25) and the Acch flags the
 * previous frame's check left (the first step's landing had set the ground bits: 0x20F0 for A's 0x2010). Also without an xz correction: the landing frame 236 (the first
 * step's check met the ground, no xz change) checked from the landed place and slid 1.03 off A's z. A frame whose
 * earlier steps made no body check: as before. Host memory in the entry; dt 1: never. Negative control
 * WWHD_T60_DRWALLSEG_NEGCTRL=1. */
static inline bool wallSegNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_DRWALLSEG_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return true;
#endif
}
struct Entry;
static inline bool wallSegWanted(const Entry& e);
struct Entry {
    u32 frame;       /* 30 Hz frame tag of the record */
    double x0, z0;   /* the frame-start xz plus the corrections between the steps */
    f32 wx, wz;      /* the xz this form left after the last step's adds */
    f32 pbx, pbz;    /* the frame's full pushback vector (first step, before x dt) */
    /* */
    double kx, kz;   /* the earlier steps' body BG check xz corrections (CrrPos) of this frame */
    f32 o0[3];       /* the first step's old.pos at its body BG check (A's segment start) */
    f32 oSave[3];    /* the tick30 step's own old.pos, put back after its check */
    f32 pre[3];      /* xyz before this step's check */
    double ky;       /* the earlier steps' body check y corrections (a landing) */
    f32 fy1;         /* the first step's fall before its check (pos.y - old.pos.y) */
    f32 v0;          /* the frame-start speed.y (first step, before the move) */
    f32 sq1;         /* the first step's speed.y at its check (x 0.25) */
    u32 fl0;         /* the Acch flags at the first step's check (the previous frame's check) */
    bool o0Valid;
    bool segOn;      /* the tick30 check has A's segment in place */
    bool t30Done;    /* the tick30 write ran (the entry stays for the tick30 check) */
};
inline std::unordered_map<u32, Entry> s_entries;

/* before the step's forward add: x / z the position now, pbx / pbz the full pushback vector of this step (before x dt).
 * Returns the entry when the form runs this step. */
static inline Entry* begin(u32 key, u32 frame, f32 x, f32 z, f32 pbx, f32 pbz, f32 vy) {
    if (t60::dt() == 1.0f || nc())
        return nullptr;
    if (t60::first_step()) {
        Entry& e = s_entries[key];
        e.frame = frame;
        e.x0 = x, e.z0 = z;
        e.pbx = pbx, e.pbz = pbz;
        e.v0 = vy;
        e.kx = e.kz = 0.0;
        e.ky = 0.0;
        e.o0Valid = e.segOn = e.t30Done = false;
        return &e;
    }
    auto it = s_entries.find(key);
    if (it == s_entries.end() || it->second.frame != frame || it->second.t30Done)
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
        const bool seg = wallSegWanted(*e); /* the earlier steps' BG corrections left out */
        f32 ax = gabi::fadds_ppc((f32)(seg ? e->x0 - e->kx : e->x0), fx);
        f32 az = gabi::fadds_ppc((f32)(seg ? e->z0 - e->kz : e->z0), fz);
        if (push) {
            ax = gabi::fadds_ppc(ax, heldX);
            az = gabi::fadds_ppc(az, heldZ);
        }
        x = ax, z = az;
        e->t30Done = true; /* kept for the tick30 body BG check (wallSegBegin / wallSegEnd) */
        return;
    }
    e->wx = x, e->wz = z;
}
static inline bool wallSegWanted(const Entry& e) {
    return !wallSegNc() && e.o0Valid;
}
/* dr_body_bg_check, before CrrPos (old.pos / pos with the pivot taken out): the first step notes A's segment start; an
 * earlier step notes xz; the tick30 step of a frame with earlier corrections puts A's segment start in old.pos */
template <class V, class S, class F>
static inline void wallSegBegin(u32 key, u32 frame, V& oldPos, V& pos, S& speedY, F& flags) {
    const f32 x = pos.x, z = pos.z;
    if (t60::dt() == 1.0f || nc() || wallSegNc())
        return;
    auto it = s_entries.find(key);
    if (it == s_entries.end() || it->second.frame != frame)
        return;
    Entry& e = it->second;
    e.segOn = false;
    if (t60::tick30()) {
        if (!e.t30Done || !wallSegWanted(e))
            return;
        e.oSave[0] = oldPos.x, e.oSave[1] = oldPos.y, e.oSave[2] = oldPos.z;
        oldPos.x = e.o0[0], oldPos.y = e.o0[1], oldPos.z = e.o0[2];
        if (e.ky != 0.0) { /* an earlier step's check landed: A's pre-check state */
            /* the height: the frame start + the frame's fall (the frame-start speed's whole fall) */
            pos.y = (f32)((double)e.o0[1] + (double)e.fy1 * (double)t60::steps());
            /* speed.y at the check (x 0.25): the frame-start speed less the frame's whole gravity (the first step took
             * dt of it), as A's one decrement */
            const f32 v0q = gabi::fmuls_ppc(e.v0, 0.25f);
            speedY = gabi::fsubs_ppc(v0q, gabi::fmuls_ppc(gabi::fsubs_ppc(v0q, e.sq1), (f32)t60::steps()));
            /* the Acch's flags as the previous frame's check left them (A's check starts from those) */
            flags = e.fl0;
        }
        e.segOn = true;
        return;
    }
    if (t60::first_step()) {
        e.o0[0] = oldPos.x, e.o0[1] = oldPos.y, e.o0[2] = oldPos.z;
        e.o0Valid = true;
        e.fy1 = (f32)pos.y - e.o0[1];
        e.sq1 = speedY;
        e.fl0 = flags;
    }
    e.pre[0] = x, e.pre[1] = z, e.pre[2] = pos.y;
}
/* after CrrPos: an earlier step's xz correction noted; the tick30 step's own old.pos back, the entry dropped */
template <class V>
static inline void wallSegEnd(u32 key, u32 frame, V& oldPos, V& pos) {
    const f32 x = pos.x, z = pos.z;
    if (t60::dt() == 1.0f || nc() || wallSegNc())
        return;
    auto it = s_entries.find(key);
    if (it == s_entries.end() || it->second.frame != frame)
        return;
    Entry& e = it->second;
    if (t60::tick30()) {
        if (e.segOn) {
            oldPos.x = e.oSave[0], oldPos.y = e.oSave[1], oldPos.z = e.oSave[2];
            e.segOn = false;
        }
        if (e.t30Done)
            s_entries.erase(it);
        return;
    }
    if (e.t30Done)
        return;
    e.kx += (double)x - (double)e.pre[0];
    e.kz += (double)z - (double)e.pre[1];
    e.ky += (double)(f32)pos.y - (double)e.pre[2];
}
/* (2026-11-25, Bokoblin B4B4 on bq / bk3 / bkL at N 2, frame 6): the fall term's tick30 form
 * (c_damagereaction_fall_t60.h) returns the share (y0 + term) - y that caller adds to y: two roundings
 * where A's one fmadds(speed.y, 0.25, y) gives y0 + term (the product by 0.25 is exact): the pre-ground-check height (the
 * Acch's copies of it) 1 ulp off. The caller now writes y0 + term itself when that form's frame-start path is taken.
 * Negative control WWHD_T60_DRFALLX_NEGCTRL=1: the share add, as before. */
inline bool fallNc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_DRFALLX_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
} // namespace t60drPos
