/* d_a_player_swimrise_t60.h (WWHD, true 60 build, 2026-11-20,
 * tbko pair:  high fall into the water). Decomp build only. Separate
 * file.
 *
 * In the water Link's gravity is 0 (procSwimWait_init / procSwimUp_init); the rise is changeSwimUpProc's own push
 * (02421FBC: speed.y += 6 a frame up to 9.5, x dt per step since), run from the swim / demo-dead procedures
 * BEFORE posMoveFromFootPos. A then adds the new speed once (semi-implicit: v += 6; p += v). posMoveFromFootPos's
 * exact-arc term dv (1 - dt) / 2 (the per-step movement rule) only saw its own gravity change (0 in the water), so N steps of
 * (v += 6 dt; p += v dt) sank 6 (1 - dt) / 2 more per frame than A (tbko f122 SWIM_WAIT y A 4084.104 / B 4082.604, 1.5
 * a frame at N 2; B touched the floor a frame early with speed.y -11 and SWIM_UP started 12 frames early).
 * Form: changeSwimUpProc records the step's speed.y change (after the 9.5 clamp, as posMove's gravity term takes the
 * fall-speed limit), posMoveFromFootPos adds it to the step's arc dv. A speed.y SET (the water entry's -50 in
 * changeSwimProc, procSwimUp_init's 0) is not a change of this record: only the push is; the surface branch (the same
 * call starts SWIM_UP / sets pos.y = mWaterY) drops the step's record.
 * The 9.5 clamp (s84 bko84s2: exact through f130, then +0.125 a frame from the clamp frame, SWIM_UP one frame early):
 * A's frame adds min(v0 + 6, 9.5) once; the steps' arc form is exact only while no step clamps. On a frame whose push
 * clamped, the tick30 step's add is A's frame add minus what the frame's earlier steps added (v0 = speed.y at the
 * frame's first-step push; the compensated add, then rounds the frame's sum once as A does). A frame
 * whose first step did not push (speed.y >= 9.5 already: no push in A either) or whose surface branch ran keeps the
 * per-step form.
 * The surface test (s85 bko84s3: SWIM_UP B f147 / A f148, y exact before): A tests mWaterY - pos.y < 55.1 once, before
 * the frame's move; only the frame's first step tests, from the frame-start pos.y and A's pushed speed.y
 * (lkT60_swimSurfaceSpeed); a later step's half-frame pos.y crossed the depth a frame early.
 * Lifetime: host memory only (never saved), one record for Link (one player), valid on the step it was recorded in
 * (step counter stamp) and taken (cleared) by that step's posMove; a record of a step whose posMove does not run (a
 * procedure change that skips it) is dropped by the stamp. dt 1: never recorded, never read (the original).
 * Shared by d_a_player_main_05.cpp (writer) and d_a_player_main_03.cpp (reader): both are TUs of the one game unit
 * d_a_player_main (units.cmake), so the inline variable is one object there (true60.h's rule, a port global for state
 * shared BETWEEN units, does not apply inside a unit); a verify harness that compiles one of them alone gets its own. */
#pragma once
#include "true60.h"
#include <cstdlib>

struct LkT60SwimRise {
    float dv;        /* this step's push */
    uint32_t stamp;  /* step counter of the step that recorded dv */
    bool valid;
    /* the frame (clamp): */
    uint32_t frame;  /* step counter of the frame's first-step push */
    float v0;        /* speed.y before that push */
    double added;    /* the frame's earlier steps' y adds (posMoveFromFootPos) */
    bool clamped;    /* a push of this frame hit 9.5 */
    bool active;
};
inline LkT60SwimRise g_lkT60SwimRise;

static inline bool lkT60_swimRiseFrame(const LkT60SwimRise &s) {
    return s.active && t60_step_counter() - s.frame < t60::steps();
}

/* changeSwimUpProc, dt != 1: the push of this step from speed.y sy0 to the new speed.y (clamped: hit 9.5) */
static inline void lkT60_swimRiseRecord(float sy0, float vNew, bool clamped) {
    if (t60::dt() == 1.0f) {
        return;
    }
    LkT60SwimRise &s = g_lkT60SwimRise;
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        s.frame = now;
        s.v0 = sy0;
        s.added = 0.0;
        s.clamped = false;
        s.active = true;
    }
    if (clamped && lkT60_swimRiseFrame(s)) {
        s.clamped = true;
    }
    s.dv = vNew - sy0;
    s.stamp = now;
    s.valid = true;
}

/* changeSwimUpProc's surface test, dt != 1 (first step only): A's speed.y at its one test = the frame's push from the
 * frame-start speed vStart (02421FBC: v + 6 clamped at 9.5 when v < 9.5; unchanged otherwise) */
static inline float lkT60_swimSurfaceSpeed(float vStart) {
    if (!(vStart < 9.5f)) {
        return vStart;
    }
    const float v = vStart + 6.0f;
    return v > 9.5f ? 9.5f : v;
}

/* changeSwimUpProc's surface branch (pos.y = mWaterY; SWIM_UP init sets speed.y 0): the step's add carries no push */
static inline void lkT60_swimRiseDrop() {
    g_lkT60SwimRise.valid = false;
    g_lkT60SwimRise.active = false;
}

/* posMoveFromFootPos, dt != 1: the push of this step (0 when none was recorded on it); the record is cleared */
static inline float lkT60_swimRiseTake() {
    LkT60SwimRise &s = g_lkT60SwimRise;
    const float dv = (s.valid && s.stamp == t60_step_counter()) ? s.dv : 0.0f;
    s.valid = false;
    return dv;
}

/* posMoveFromFootPos, dt != 1, after the step's y add vy is formed (own = the add came from Link's own speed, no port
 * movement mod): on the tick30 step of a clamped frame A's frame add minus the earlier steps' adds; else vy */
static inline double lkT60_swimRiseAdd(double vy, bool own) {
    LkT60SwimRise &s = g_lkT60SwimRise;
    if (!lkT60_swimRiseFrame(s)) {
        s.active = false;
        return vy;
    }
    if (!t60::tick30()) {
        s.added += vy;
        return vy;
    }
    s.active = false;
    if (!s.clamped || !own) {
        return vy;
    }
    float v1 = s.v0 + 6.0f; /* A: speed.y + 6, clamped at 9.5 (02421FBC) */
    if (v1 > 9.5f) {
        v1 = 9.5f;
    }
    return (double)v1 - s.added;
}

/* follow-up (2026-11-22;, probe wgL1): the water entry's frame. The
 * original's entry update runs changeSwimProc (speed.y = the fall speed) and the new SWIM_WAIT body (the push +6) once;
 * the body then changes to SWIM_MOVE, whose body first runs in the next update. At N steps the entry (now on the frame's
 * first step) runs SWIM_WAIT's body with dt (3 at N 2) and the procedure change holds the frame's later
 * bodies (rule 1, 2026-11-17, d_a_player_t60.h lkT60_procEntered): no push on them, the frame ended 3 lower in
 * speed.y (-30.84 vs A's -27.84) and the dive ran 2 frames long (SWIM_UP A 217 / B 219). Form: on a later step of a frame
 * whose first step pushed (this frame's record) and whose body is held (a procedure entered on the first step, the
 * gravity still the water's 0), posMoveFromFootPos pushes the step's dt share itself, as the held body would (the same
 * record and clamp), so the frame's push is A's one push. dt 1: never. Negative control WWHD_T60_SWIMHELD_NEGCTRL=1
 * (game build). */
static inline bool lkT60_swimRiseHeldPush(float vy, bool held, float* out) {
    if (t60::dt() == 1.0f || t60::first_step() || !held) {
        return false;
    }
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool negctrl = [] { const char* e = getenv("WWHD_T60_SWIMHELD_NEGCTRL"); return e && e[0] == '1'; }();
    if (negctrl) {
        return false;
    }
#endif
    const LkT60SwimRise &s = g_lkT60SwimRise;
    if (!lkT60_swimRiseFrame(s) || (s.valid && s.stamp == t60_step_counter()) || !(vy < 9.5f)) {
        return false;
    }
    float n = vy + 6.0f * (float)t60::dt();
    const bool clamped = n > 9.5f;
    if (clamped) {
        n = 9.5f;
    }
    lkT60_swimRiseRecord(vy, n, clamped);
    *out = n;
    return true;
}
