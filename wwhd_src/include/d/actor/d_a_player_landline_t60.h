/* d_a_player_landline_t60.h (WWHD, true 60 build, 2026-11-23). Decomp build only.
 * Separate file: d_a_player_main_04.cpp keeps thin calls.
 *
 * The landing frame's BG check. A's frame f moves Link once (posMove: old.pos = the frame start, pos = + the frame's
 * speed) and checks once (mAcch.CrrPos). A fast fall triggers the Acch line check (distY > the ground-check offset):
 * the line from old.pos + wall_h to pos + wall_h meets the floor and pos is pulled back to the cross point along the
 * frame's move (xz too), then the ground check puts it on the floor. At N steps each step moves and checks its own
 * part: the step that reaches the floor has a shorter line (no line check, or one that misses) and the ground check
 * snaps it up with the step's full xz move; the later steps (on the ground already) keep the whole rest of the frame's
 * xz move. (VOMIT_JUMP landing, frame 123): A moves 0.669 of the frame's xz (the line meets the floor at
 * wall_h), B's step 0 reaches the floor (ground snap, 0.5 of it), its tick30 step another 0.5: Link 4.24 off from 123 on
 * (strict fail), the 557 damage 2 frames early.
 * Form (the frame's BG check on the tick30 step, for the landing frame only): when the frame started in the air (no
 * ground hit in the previous frame-end check) and an earlier step's check of this frame met the ground, the tick30 step's
 * CrrPos gets A's frame segment: old.pos = the frame-start position (before the first step's posMove) and pos = (the
 * step's xz after its posMove = the frame-start xz + the frame's xz move, the frame-start y + A's frame fall: the frame-
 * start speed.y + gravity once, the limit as posMove's dt 1 path). Its result is A's frame-end position (cross point or
 * ground snap); old.pos is put back after the call. The procedure decisions keep the frame-start ground view
 * (d_a_player_bghit_t60.h). Any other frame, dt 1 (N = 1): the step's own check (the original). Negative control
 * WWHD_T60_LANDLINE_NEGCTRL=1 (game build). Host memory, one player.
 *
 * Second form (2026-11-23): the position-set frame. A procedure body may set Link's
 * position (DEMO_TOOL, grandma frame 1065: y 1 -> 375). A's frame checks once, on the segment old.pos (the frame
 * start, y 1) -> the set position + the frame's fall (372.5): the line check meets a floor at 224, no ground hit (flags
 * 0x12000), speed.y keeps one gravity tick (-2.5). At N steps the first step checks the same segment (half the fall),
 * but the tick30 step starts from the set position (old.pos 375), its short segment finds the ground half a frame early
 * (speed.y 0, flags 0x20E0). Positions agree (DEMO_TOOL puts pos back after the check), speed.y / flags not. Form: when
 * the first step's body moved Link by a set (its pre-posMove position differs from the frame-start old.pos) and the
 * tick30 step's body leaves him at the same set position (bit equal: a set, not a step of a move), the tick30 step's
 * CrrPos gets A's frame segment: old.pos = the frame-start old.pos, pos = the step's xz + the first step's xz move, the
 * set y + A's frame fall; old.pos is put back after the call. Negative control WWHD_T60_POSSET_NEGCTRL=1 (game build);
 * dt 1: never. */
#pragma once
#include "true60.h"
#include "d/d_bg_s.h"
#include "d/actor/d_a_player_posadd_t60.h"
#include <cstdlib>
#include <cstring>

namespace lkT60Land {
struct Rec {
    uint32_t tag;     /* the frame (g_Counter.mCounter0) */
    uint32_t p0[3];   /* current.pos as the frame started (bit copies) */
    f32 sy;           /* A's frame fall speed (the frame-start speed.y + gravity, limited) */
    bool valid;
    bool air;         /* no ground hit in the previous frame-end check */
    bool landed;      /* an earlier step's check of this frame met the ground */
    bool on;          /* the tick30 call has A's segment in place */
    uint32_t old[3];  /* the step's own old.pos, put back after the call */
    /* */
    uint32_t o0[3];   /* the first step's old.pos (A's segment start) */
    f32 d1x, d1z;     /* the first step's posMove xz */
    bool set;         /* the first step's body moved Link by a set (p0 != o0) */
    bool setHold;     /* the tick30 step's body left him at p0 */
    f32 pre[2];       /* xz before an earlier step's CrrPos */
    bool bgNote;
    bool snapped;     /* an earlier step's autoGroundHit snapped Link down onto ground */
    bool wallSegOn;   /* this tick30 check has the wall segment */
    bool water;       /* the frame started in water (the first check's acf WATER_IN) */
    f32 wsv;          /* the step's own speed.y, put back after the check when it kept A's */
    bool snapOn;      /* this step's check has the segment (a non-tick30 step keeps the record) */
    bool airSeg;      /* this tick30 check has the landing frame's segment (air + landed) */
    uint32_t pre3[3]; /* the position before an earlier step's CrrPos (bit copies) */
    bool moved;       /* an earlier step's CrrPos of this frame moved Link */
};
static Rec s_rec;

static inline bool negctrl() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_LANDLINE_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
/* (2026-11-25, long1 seg 0 frame 106): the landing frame's form gives the tick30 step
 * A's segment, and A's check can end in the air (a fall past a ledge lip: an earlier step's half-step check stood on the
 * lip, ground hit, speed.y 0; A's frame line passes it, pushed by the wall only). The tick30 position is then A's, but
 * speed.y kept the earlier step's landing (0, then one half gravity tick: -1.25 against A's -68.84), and the fall
 * restarted from rest (y 70 off from 107). Form: after that check and autoGroundHit, no ground hit (A's frame stays in
 * the air): speed.y = A's frame fall speed (the record's sy, the frame-start speed.y + gravity, limited). A ground hit:
 * as before. Negative control WWHD_T60_FALLSPEEDY_NEGCTRL=1 (game build). */
static inline bool negctrlFall() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_FALLSPEEDY_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
/* (2026-11-25; oq pair + at N 2, frame 244, the swim stroke's
 * dive): A's frame checks once (CrrPos), the Acch line from old.pos (the frame start) to the frame's end; a fast move
 * meets a polygon (LINE_CHECK_HIT) and pulls Link back to the cross point. At N steps the earlier steps' checks met
 * nothing (no correction) and only the tick30 step's line hit, from that step's start: another line (speed.y per step
 * -23 / -20, A's -20 for the frame), another cross point. The position before the check was A's bit for bit, after it
 * 0.059 off (A -6210.1982 / -3314.6428, B -6210.1846 / -3314.5842); the wall circles 115 frames later met another wall
 * at the corner (frame 359: A's speed 18.0 unclamped, B's 15.94 under the side wall's term). Form: on the tick30 step of
 * a frame that started in the air (no ground hit), with the line check on, no position set by the body and no earlier
 * step's check of the frame changing the position (bit equal before / after; no ground hit either: the landing form),
 * the check gets A's segment: old.pos = the frame's first-step old.pos, put back after the call. Any other frame, dt 1:
 * the step's own check. NC WWHD_T60_LINESEG_NEGCTRL=1 (game build). */
static inline bool lineSegOff() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_LINESEG_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return true;
#endif
}
static inline bool negctrlSet() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_POSSET_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
static inline void ld3(uint32_t* d, const cXyz& v) {
    for (int i = 0; i < 3; i++)
        d[i] = gabi::load<uint32_t>(gabi::ea(&v) + 4 * i);
}
static inline void st3(cXyz& v, const uint32_t* s) {
    for (int i = 0; i < 3; i++)
        gabi::store<uint32_t>(gabi::ea(&v) + 4 * i, s[i]);
}

/* the frame's first step, before posMove: the frame-start position and A's frame fall speed (posMove's dt 1 gravity
 * line on the frame-start speed.y) */
static inline bool frameStartStep() { return t60::dt() != 1.0f && t60::first_step() && !negctrl(); }
template <class P>
static inline void frameStart(P* py, uint32_t tag, bool heavy /* checkHeavyStateOn() */) {
    if (!frameStartStep())
        return;
    Rec& r = s_rec;
    r.tag = tag;
    ld3(r.p0, py->current.pos);
    f32 sy;
    if (heavy) {
        const f32 lim = py->maxFallSpeed * 1.5f;
        sy = gabi::fmadds(py->gravity, 2.25f, py->speed.y);
        if (sy < lim)
            sy = lim;
    } else {
        const f32 lim = py->maxFallSpeed;
        sy = py->speed.y + py->gravity;
        if (sy < lim)
            sy = lim;
    }
    r.sy = sy;
    r.valid = true;
    r.landed = false;
    r.snapped = false;
    r.snapOn = false;
    r.on = false;
    r.set = false;
    r.setHold = false;
    r.moved = false;
}

/* (2026-11-24, oq route frame 107, AUTO_JUMP landing on the Kita platform): execute's
 * moving-ground carry (dBgS::MoveBgCrrPos at the execute start when the last check's ground polygon is a moving BG;
 * Kita's dBgS_MoveBGProc_Typical moves Link by the platform's last matrix change, the ground-hit argument unused). A's
 * landing frame starts in the air: the frame-start ground polygon is the floor far below (no moving BG), no carry; the
 * carry starts with the next frame. At N steps an earlier step's check of this frame already put the platform in
 * mAcch.m_gnd, so the tick30 step carried Link with the platform's last move (x +0.023, z -0.002 at oq frame 107) one
 * frame before A, an offset kept from then on. Form: on the tick30 step of the landing frame (the frame started in the
 * air, an earlier step met the ground: LANDLINE's own record) the carry is skipped (A's frame-start view). Any other
 * step / frame, dt 1: the original. Negative control WWHD_T60_LANDCARRY_NEGCTRL=1 (game build). Host memory, one
 * player. */
static inline bool negctrlCarry() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_LANDCARRY_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return false;
#endif
}
static inline bool carrySkip(uint32_t tag) {
    const Rec& r = s_rec;
    if (t60::dt() == 1.0f || negctrl() || negctrlCarry() || !r.valid || r.tag != tag || !t60::tick30() ||
        t60::first_step())
        return false;
    return r.air && r.landed;
}

/* every step, after the body, before posMove (x, y, z: the pre-posMove position's bits): the
 * tick30 step notes whether its body left Link at the first step's set position */
static inline void bodyPos(uint32_t x, uint32_t y, uint32_t z, uint32_t tag) {
    Rec& r = s_rec;
    if (t60::dt() == 1.0f || !r.valid || r.tag != tag || !t60::tick30() || t60::first_step())
        return;
    r.setHold = r.set && x == r.p0[0] && y == r.p0[1] && z == r.p0[2];
}

/* (2026-11-24; the bomb route, frame 135: Link walks back over
 * a 24-unit drop. A's frame checks once on the upper level (CrrPos at the frame start + its move, the wall circles
 * above the ledge face) and autoGroundHit then snaps him down: frame end 16.4 along. B's first step's autoGroundHit
 * snapped him down mid-frame (the per-step ground-check rule), so the tick30 step's CrrPos checked on the lower level, its wall circles
 * met the face of the drop behind him and pushed him 11.9 on (z 10.95 off from then on). Form: when an earlier step of a
 * frame that started on the ground took such a snap, the tick30 step's CrrPos gets A's segment (old.pos = the frame
 * start, pos.y = the frame start + A's frame fall, xz as the step placed it); autoGroundHit after it snaps as A's does.
 * NC WWHD_T60_SNAPSEG_NEGCTRL=1; dt 1 never. */
static inline bool snapSegOff() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_SNAPSEG_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return true;
#endif
}
/* (2026-11-25; pig route frame 91: Link's carry drift): the speed-ramp position-gap (3)
 * wall frame's tick30 check (A's segment start, old.pos = the frame start) also gets A's height, pos.y = the frame start +
 * A's frame fall: Link walks into a wall on the ground, the earlier step's check snapped y back to the floor, so the tick30
 * step's own y add stood at 549.06 where A's input is 547.5, and its speed.y (read by the wall correction) was its own
 * share -1.25 where A's is -2.5 (x 0.03 / z 0.06 off, then the wall term's speed, 1.0 by frame 213; the legs helper's
 * mdc frame 433: the wall circles met the other polygon of a corner). NC WWHD_T60_WALLSEGY_NEGCTRL=1; dt 1 never. */
static inline bool wallSegYOff() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool nc = [] { const char* e = getenv("WWHD_T60_WALLSEGY_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
#else
    return true;
#endif
}
/* before mAcch.CrrPos (acf: the flags before ClrGroundHit, the previous check's result) */
static inline bool begin(uint32_t acf, cXyz& oldPos, cXyz& pos, uint32_t tag) {
    Rec& r = s_rec;
    r.on = false;
    r.airSeg = false;
    r.wallSegOn = false;
    if (t60::dt() == 1.0f || negctrl() || !r.valid || r.tag != tag)
        return false;
    r.bgNote = false;
    if (!t60::tick30()) { /* an earlier step: its BG check's xz move is noted in end */
        if (t60::first_step())
            g_lkT60BgK[0] = g_lkT60BgK[1] = 0.0;
        r.pre[0] = pos.x;
        r.pre[1] = pos.z;
        r.bgNote = true;
        ld3(r.pre3, pos); /* */
    }
    if (t60::first_step()) {
        g_lkT60WallSeg = false;
        g_lkT60WallSegAll = false;
        r.air = !(acf & (uint32_t)dBgS_Acch::GROUND_HIT);
        r.water = (acf & (uint32_t)dBgS_Acch::WATER_IN) != 0; /* (WATER_HIT alone: a water plane found below, e.g. land by the sea) */
        /* the frame-start segment start and whether the body set the position */
        ld3(r.o0, oldPos);
        r.set = !negctrlSet() && (r.o0[0] != r.p0[0] || r.o0[1] != r.p0[1] || r.o0[2] != r.p0[2]);
        f32 p0x, p0z;
        std::memcpy(&p0x, &r.p0[0], 4);
        std::memcpy(&p0z, &r.p0[2], 4);
        r.d1x = pos.x - p0x;
        r.d1z = pos.z - p0z;
        return false;
    }
    if (r.snapped && !r.air && !snapSegOff()) {
        /* a later step of a frame whose earlier step snapped Link down: the check at A's height
         * (the segment from the frame start, y = the frame start + A's frame fall); autoGroundHit snaps after it */
        ld3(r.old, oldPos);
        st3(oldPos, r.p0);
        f32 p0y;
        std::memcpy(&p0y, &r.p0[1], 4);
        pos.y = p0y + r.sy;
        r.on = true;
        r.snapOn = true;
        return true;
    }
    if (!t60::tick30())
        return false;
    /* (widened) a grounded frame without a push; not in / on water: there the frame's y does not follow
     * the gravity fall r.sy (oq frame 246, Link rising out of the water at +6 / frame: A 277.2, the form's 274.2) */
    const bool lkSegAll = g_lkT60WallSegAll && !r.air && !r.water && !r.set; /* (a position-set frame: its own segment) */
    g_lkT60WallSegAll = false;
    if (g_lkT60WallSeg || lkSegAll) {
        /* (speed-ramp position-gap (3) K=0 probe) the wall frame: the tick30 place is A's (frame start + A's move); the check gets A's
         * segment start, old.pos = the frame-start position */
        g_lkT60WallSeg = false;
        if (!(r.air && r.landed)) {
            ld3(r.old, oldPos);
            st3(oldPos, r.p0);
            r.wallSegOn = !r.water; /* (the y / speed.y part: A's gravity fall; not in / on water) */
            if (!wallSegYOff() && !r.water) {
                /* A's height too: the frame start + A's frame fall (on the ground the earlier
                 * step's check snapped y back, so the step's own add stood 0.94 above A's 547.5: the wall circles met
                 * the wall at another height) */
                f32 p0y;
                std::memcpy(&p0y, &r.p0[1], 4);
                pos.y = p0y + r.sy;
            }
            r.on = true;
            return true;
        }
    }
    if (r.air && !r.landed && !r.moved && !r.set && (acf & (uint32_t)dBgS_Acch::LINE_CHECK) && !lineSegOff()) {
        /* the air frame whose earlier checks changed nothing: A's segment start */
        ld3(r.old, oldPos);
        st3(oldPos, r.o0);
        r.on = true;
        return true;
    }
    if (!(r.air && r.landed)) {
        if (!(r.set && r.setHold))
            return false;
        /* the position-set frame: A's segment from the frame-start old.pos to the set position + the frame's move */
        ld3(r.old, oldPos);
        st3(oldPos, r.o0);
        f32 p0y;
        std::memcpy(&p0y, &r.p0[1], 4);
        pos.x = pos.x + r.d1x;
        pos.z = pos.z + r.d1z;
        pos.y = p0y + r.sy;
        r.on = true;
        return true;
    }
    ld3(r.old, oldPos);
    st3(oldPos, r.p0);
    f32 p0y;
    std::memcpy(&p0y, &r.p0[1], 4);
    pos.y = p0y + r.sy; /* A's add: f32(frame-start y + speed.y) */
    r.on = true;
    r.airSeg = true; /* */
    return true;
}
/* after autoGroundHit on the step whose check had the landing frame's segment: A's frame ended in
 * the air (no ground hit) -> A's frame fall speed */
template <class P>
static inline void fallSpeed(P* py, uint32_t flags) {
    Rec& r = s_rec;
    if (!r.airSeg)
        return;
    r.airSeg = false;
    if (negctrlFall() || (flags & (uint32_t)dBgS_Acch::GROUND_HIT))
        return;
    py->speed.y = r.sy;
}
/* around mAcch.CrrPos: the wall-segment check also reads Link's speed (pm_speed: the check's wall
 * correction depends on speed.y; A's speed.y going in is the frame's fall, -2.5 on the ground, B's tick30 step had only
 * its own share after the earlier step's ground hit zeroed it; dc7: A's check run twice from the same input agrees only
 * with speed put back). Before: speed.y = A's frame fall (noting the step's own); after: when the check left it there,
 * the step's own back (a ground hit's 0 stays). */
static inline void wallSegSpeedBegin(f32& vy) {
    Rec& r = s_rec;
    if (!r.wallSegOn || wallSegYOff())
        return;
    r.wsv = vy;
    vy = r.sy;
}
static inline void wallSegSpeedEnd(f32& vy) {
    Rec& r = s_rec;
    if (!r.wallSegOn || wallSegYOff())
        return;
    r.wallSegOn = false;
    if (vy == r.sy)
        vy = r.wsv;
}
/* after autoGroundHit (y0 / f0: current.pos.y and the Acch flags before it) */
static inline void snapNote(f32 y0, f32 y1, uint32_t f0, uint32_t f1, uint32_t tag) {
    Rec& r = s_rec;
    if (t60::dt() == 1.0f || t60::tick30() || !r.valid || r.tag != tag || snapSegOff())
        return;
    if (!(f0 & (uint32_t)dBgS_Acch::GROUND_HIT) && (f1 & (uint32_t)dBgS_Acch::GROUND_HIT) && y1 < y0)
        r.snapped = true;
}

/* after mAcch.CrrPos (pos: the checked position) */
static inline void end(uint32_t flags, cXyz& oldPos, const cXyz& pos) {
    Rec& r = s_rec;
    if (r.bgNote) {
        g_lkT60BgK[0] += (double)pos.x - (double)r.pre[0];
        g_lkT60BgK[1] += (double)pos.z - (double)r.pre[1];
        /* (d_a_player_posadd_t60.h) the note, for the next posAdd's undone-push check */
        g_lkT60BgLast.pre[0] = r.pre[0];
        g_lkT60BgLast.pre[1] = r.pre[1];
        g_lkT60BgLast.push[0] = (double)pos.x - (double)r.pre[0];
        g_lkT60BgLast.push[1] = (double)pos.z - (double)r.pre[1];
        g_lkT60BgLast.on = g_lkT60BgLast.push[0] != 0.0 || g_lkT60BgLast.push[1] != 0.0;
        r.bgNote = false;
    }
    if (r.on) {
        st3(oldPos, r.old);
        r.on = false;
        if (r.snapOn && !t60::tick30()) { /* an earlier step: the record stays for the frame */
            r.snapOn = false;
            return;
        }
        r.snapOn = false;
        r.valid = false;
        return;
    }
    if (t60::dt() == 1.0f || !r.valid || t60::tick30())
        return;
    if (flags & (uint32_t)dBgS_Acch::GROUND_HIT)
        r.landed = true;
    uint32_t p[3]; /* */
    ld3(p, pos);
    if (p[0] != r.pre3[0] || p[1] != r.pre3[1] || p[2] != r.pre3[2])
        r.moved = true;
}
}  // namespace lkT60Land
