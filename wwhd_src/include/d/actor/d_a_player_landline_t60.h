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
    r.on = false;
    r.set = false;
    r.setHold = false;
}

/* every step, after the body, before posMove (x, y, z: the pre-posMove position's bits): the
 * tick30 step notes whether its body left Link at the first step's set position */
static inline void bodyPos(uint32_t x, uint32_t y, uint32_t z, uint32_t tag) {
    Rec& r = s_rec;
    if (t60::dt() == 1.0f || !r.valid || r.tag != tag || !t60::tick30() || t60::first_step())
        return;
    r.setHold = r.set && x == r.p0[0] && y == r.p0[1] && z == r.p0[2];
}

/* before mAcch.CrrPos (acf: the flags before ClrGroundHit, the previous check's result) */
static inline bool begin(uint32_t acf, cXyz& oldPos, cXyz& pos, uint32_t tag) {
    Rec& r = s_rec;
    r.on = false;
    if (t60::dt() == 1.0f || negctrl() || !r.valid || r.tag != tag)
        return false;
    r.bgNote = false;
    if (!t60::tick30()) { /* an earlier step: its BG check's xz move is noted in end */
        if (t60::first_step())
            g_lkT60BgK[0] = g_lkT60BgK[1] = 0.0;
        r.pre[0] = pos.x;
        r.pre[1] = pos.z;
        r.bgNote = true;
    }
    if (t60::first_step()) {
        g_lkT60WallSeg = false;
        r.air = !(acf & (uint32_t)dBgS_Acch::GROUND_HIT);
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
    if (!t60::tick30())
        return false;
    if (g_lkT60WallSeg) {
        /* (speed-ramp position-gap (3) K=0 probe) the wall frame: the tick30 place is A's (frame start + A's move); the check gets A's
         * segment start, old.pos = the frame-start position */
        g_lkT60WallSeg = false;
        if (!(r.air && r.landed)) {
            ld3(r.old, oldPos);
            st3(oldPos, r.p0);
            r.on = true;
            return true;
        }
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
    return true;
}
/* after mAcch.CrrPos (pos: the checked position) */
static inline void end(uint32_t flags, cXyz& oldPos, const cXyz& pos) {
    Rec& r = s_rec;
    if (r.bgNote) {
        g_lkT60BgK[0] += (double)pos.x - (double)r.pre[0];
        g_lkT60BgK[1] += (double)pos.z - (double)r.pre[1];
        r.bgNote = false;
    }
    if (r.on) {
        st3(oldPos, r.old);
        r.on = false;
        r.valid = false;
        return;
    }
    if (t60::dt() == 1.0f || !r.valid || t60::tick30())
        return;
    if (flags & (uint32_t)dBgS_Acch::GROUND_HIT)
        r.landed = true;
}
}  // namespace lkT60Land
