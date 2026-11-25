#pragma once
/* true60 (decomp build only): Link's first-step form (the first-step AI-timer rule applies to Link too; the
 * form, first used in). The 30 Hz game counts a procedure's frames, and takes the decisions those counts
 * drive, in its one update per frame; the N-step run maps that update to the frame's FIRST step (the input read, rule
 * first-step input). Motion runs on every step with its dt share.
 *
 *   lkT60_countStep   true on the frame's first step (always at dt 1): counts and count-driven decisions run here.
 *   lkT60_counted     records the value a count left on the first step (step, key, site, value).
 *   lkT60_countHeld   on the frame's later steps: the branch the first step took is held (a count that reached its
 *                       end value on the first step switches on the NEXT frame's first step, as the original's next
 *                       update does; else the frame's second step would take the end branch (N - 1) / N frame early).
 *
 * The form `if (v > 0) { count } else { end }` becomes `if (lkT60_countBranch(...)) { lkT60_countDown(...) } else ...`.
 * A body that first runs after a procedure init later in the frame does not count in that frame (the original runs no
 * body of that procedure in it). Visual-only per-frame work (blink, glints, effects) and sustained sounds that drive no
 * logic may stay on the tick30 step.
 *
 * Host memory, Link only. The latch is a static per translation unit (the split-actor state rule (1): each Link part keeps its own
 * copy; a key = the procedure, or a constant for a counter outside the procedures, plus a per-file site number).
 * Never consulted at dt 1 (the original). */
#include "true60.h"
#include <cmath>
#include <cstdlib>

struct LkT60CountLatch {
    uint32_t stamp;
    int32_t key;
    int32_t value;
    bool valid;
};
static LkT60CountLatch s_lkT60Count[24];

static inline bool lkT60_countStep() { return t60::dt() == 1.0f || t60::first_step(); }
/* procFall's fall-face test on every step again (negative control
 * WWHD_T60_FALLFACE_NEGCTRL=1); kept in this header, a function-local static in a glued unit needs a data glue line */
static inline bool lkT60_fallFaceNegCtrl() {
    static const bool nc = [] { const char* e = getenv("WWHD_T60_FALLFACE_NEGCTRL"); return e && e[0] == '1'; }();
    return nc;
}
/* procFall's fall-face test runs on the frame's first step; every call at dt 1 or under the NC */
static inline bool lkT60_fallFaceStep() { return lkT60_fallFaceNegCtrl() || lkT60_countStep(); }

static inline void lkT60_counted(int32_t key, int site, int32_t value) {
    if (t60::dt() == 1.0f) {
        return;
    }
    LkT60CountLatch& l = s_lkT60Count[site];
    l.stamp = t60_step_counter();
    l.key = key;
    l.value = value;
    l.valid = true;
}

static inline bool lkT60_countHeld(int32_t key, int site, int32_t value) {
    if (t60::dt() == 1.0f || t60::first_step()) {
        return false;
    }
    const LkT60CountLatch& l = s_lkT60Count[site];
    return l.valid && t60_step_counter() - l.stamp < t60::steps() && l.key == key && l.value == value;
}

/* `if (v > 0) { count } else { end }`: the count branch, held over the frame after the first step's count */
static inline bool lkT60_countBranch(int32_t key, int site, int32_t v) {
    return v > 0 || lkT60_countHeld(key, site, v);
}

/* `v = v - 1` on the first step, recorded for the hold */
template <typename T>
static inline void lkT60_countDown(int32_t key, int site, T& v) {
    if (lkT60_countStep()) {
        v = v - 1;
        lkT60_counted(key, site, (int32_t)v);
    }
}

/* New animations: play order as in the original (2026-11-16, from the Moblin form; the split-actor state rule (3)).
 * daPy_lk_c::execute plays Link's animations (animeUpdate) BEFORE the procedure dispatch: at 30 Hz an animation set in
 * the dispatch (setFrameCtrl: a new animation's frame control) first plays in the NEXT frame's update. At N steps a set
 * made on a step other than the frame's tick30 step would play on the frame's remaining steps, (N - k) / N frame early
 * (Link's cut, proc 68: 0.6 frame ahead at N 2, 0.9 at N 4, ending a frame early at N 4). Such a set (the new-animation
 * setters setSingleMoveAnime / setActAnimeUpper; NOT setMoveAnime's blend, which re-sets the frame controls on every
 * update at the running phase: a continuation, latching it halved the walk / wait play, psword2d frames 31-74) is latched (the
 * shared set latch, keyed by the frame control, tagged with the 30 Hz frame counter) and animeUpdate skips that frame
 * control's play for the rest of the frame; it first plays on the next frame's first step. A set on the tick30 step is
 * not latched (its next play is the next frame's first step anyway). dt 1: never latched (the original). */
static inline uint32_t lkT60_frameTag() { return *gabi::at<be<uint32_t>>(0x101FF558); /* g_Counter.mCounter0 */ }
static inline void lkT60_markAnmSet(uint32_t frameCtrlEa) {
    if (t60::dt() == 1.0f || t60::tick30()) {
        return;
    }
    wwhd_t60_latch_set(frameCtrlEa, lkT60_frameTag());
}
static inline bool lkT60_anmSetPending(uint32_t frameCtrlEa) {
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(frameCtrlEa, lkT60_frameTag()) != 0;
}

/* The play rate of a running animation, per 30 Hz frame (irol pair: Link's
 * MOVE entry 3612). daPy_lk_c::execute plays the animations (animeUpdate) BEFORE the procedure dispatch, so at 30 Hz the
 * rate a frame's dispatch sets (setMoveAnime's blend re-sets the frame controls on every update: the turn-in-place blend's
 * rate 1.1 .. 2.18 from the frame's turn, the walk blend's rate from mNormalSpeed, the MOVE entry's setBlendMoveAnime)
 * first plays in the NEXT frame's update: frame f advances by the rate frame f - 1 set. At N steps the first step's
 * dispatch re-set the rate and the frame's later steps played with it: (N - 1) / N frame at a rate the original only
 * plays a frame later. ys78: the turn-in-place rate from the first step's partial turn (2.168) instead of the frame's
 * (2.176) lost 0.004 frames per turning frame (WAITS 0.052 behind at 3611), and the entry frame played (N - 1) / N at
 * MOVE's 1.255 instead of WAITS' 1.1 (+0.078): the base toe's chord at the entry (speedF 0.4396 vs 0.4572) and Link's
 * entry position (y 1360.364 / A 1360.437) followed. Form: the frame's first play records each frame control's rate (the
 * rate the previous frame's dispatch left = A's); the frame's later plays use that rate and put the dispatch's rate back
 * after the play; a play that stops or bounces (changes the rate itself) sets the rate the later steps hold. A new animation set
 * mid-frame is latched (lkT60_markAnmSet: no play this frame) and so not concerned. Host memory, animeUpdate's TU only.
 * dt 1 (the original): no record, no change. */
struct LkT60RateHold {
    uint32_t stamp, ea;
    f32 rate;
    bool valid;
};
inline LkT60RateHold s_lkT60RateHold[5]; /* one for Link's TUs (lkT60_rateHeld in main_03) */
/* before the play: returns the rate to put back after it (or NaN: nothing to put back) */
template <class FC> /* J3DFrameCtrl (a template: not every includer has its definition) */
static inline f32 lkT60_rateHoldBegin(FC* fc, int slot) {
    if (t60::dt() == 1.0f || t60::steps() < 2) {
        return __builtin_nanf("");
    }
    LkT60RateHold& h = s_lkT60RateHold[slot];
    const uint32_t now = t60_step_counter();
    const uint32_t ea = gabi::ea(fc);
    if (t60::first_step()) {
        h.stamp = now, h.ea = ea, h.rate = fc->mRate, h.valid = true;
        return __builtin_nanf("");
    }
    if (!h.valid || h.ea != ea || now - h.stamp >= t60::steps()) {
        return __builtin_nanf("");
    }
    const f32 cur = fc->mRate;
    if (cur == h.rate) {
        return __builtin_nanf("");
    }
    fc->mRate = h.rate;
    return cur;
}
/* after the play (every step, the first included): a play that changed the rate itself (a stop's 0, a reverse bounce's
 * -rate) is the original's one update stopping / bouncing within the frame, so the frame's later steps hold the play's
 * rate from then on and the play's value stays; otherwise the dispatch's rate is put back */
template <class FC>
static inline void lkT60_rateHoldEnd(FC* fc, int slot, f32 put) {
    if (t60::dt() == 1.0f || t60::steps() < 2) {
        return;
    }
    LkT60RateHold& h = s_lkT60RateHold[slot];
    if (!h.valid || h.ea != gabi::ea(fc) || t60_step_counter() - h.stamp >= t60::steps()) {
        return;
    }
    const f32 now = fc->mRate;
    if (now != h.rate) {
        h.rate = now;
        return;
    }
    if (put == put) {
        fc->mRate = put;
    }
}

/* the rate the frame's later plays of this control hold (the frame-start rate, see above) when
 * the dispatch has set another one since the frame's first play; NaN: none (no hold this frame, the same rate, dt 1) */
template <class FC>
static inline f32 lkT60_rateHeld(FC* fc, int slot) {
    if (t60::dt() == 1.0f || t60::steps() < 2) {
        return __builtin_nanf("");
    }
    const LkT60RateHold& h = s_lkT60RateHold[slot];
    if (!h.valid || h.ea != gabi::ea(fc) || t60_step_counter() - h.stamp >= t60::steps()) {
        return __builtin_nanf("");
    }
    const f32 cur = fc->mRate;
    return cur == h.rate ? __builtin_nanf("") : h.rate;
}

/* keys for counters outside a procedure (no procedure is negative) */
enum { LKT60_KEY_HOVER = -2, LKT60_KEY_FOREST_WATER = -3, LKT60_KEY_DEMO_KEEP = -4, LKT60_KEY_PARRY = -5,
       LKT60_KEY_BOW = -6, LKT60_KEY_ITEM_READY = -7 };

/* Procedure bodies entered on the frame's first step (rule 1, 2026-11-17, pair: the first-step frame-level rule's lock-on turn).
 * daPy_lk_c::execute runs one procedure body per 30 Hz update; a body that changes the procedure (its init via
 * commonProcInit) returns, and the new procedure's body first runs in the NEXT update (procWait -> procAtnActorWait_init
 * on the lock-on frame, the turn to the target in procAtnActorWait from the next frame). At N steps a change made in the
 * frame's first-step update (input, the first-step input rule; the frame's attention / events, the first-step frame-level rule) ran the new body on
 * the frame's later steps, (N - 1) / N frame early (shape_angle.y turned on steps 2-4 of frame 31, A from 32).
 * Form: on the first step the dispatch opens a window (key: Link's mCurProc EA); commonProcInit called in it from then
 * on (the body, or the execute's later work) marks the frame; on the frame's later steps the dispatch is held (no body;
 * the rest of the execute, motion included, runs with its dt share), as long as the procedure entered is still the
 * current one (a later step's own change runs its body as before). The first step's body runs as before. Shared through
 * the port latch (commonProcInit is in d_a_player_main_01, the dispatch in d_a_player_main_04). dt 1: never. */
static inline bool lkT60_procStep() { return t60::dt() != 1.0f && t60::steps() > 1 && t60::first_step(); }
static inline uint32_t lkT60_procOpenKey(uint32_t curProcEa) { return curProcEa; }
static inline uint32_t lkT60_procEnterKey(uint32_t curProcEa) { return curProcEa + 1; }
/* d_a_player_main_04: first step, before the dispatch */
static inline void lkT60_procOpen(uint32_t curProcEa) {
    if (!lkT60_procStep()) {
        return;
    }
    wwhd_t60_latch_set(lkT60_procEnterKey(curProcEa), ~lkT60_frameTag()); /* (a replayed first step starts unmarked) */
    wwhd_t60_latch_set(lkT60_procOpenKey(curProcEa), lkT60_frameTag());
}
/* d_a_player_main_01 commonProcInit */
static inline void lkT60_procEntered(uint32_t curProcEa) {
    if (!lkT60_procStep() || !wwhd_t60_latch_pending(lkT60_procOpenKey(curProcEa), lkT60_frameTag())) {
        return;
    }
    wwhd_t60_latch_set(lkT60_procEnterKey(curProcEa), lkT60_frameTag());
}
/* d_a_player_main_04: true when the first step entered a procedure in its dispatch (or after it) */
static inline bool lkT60_procEnteredThisFrame(uint32_t curProcEa) {
    return lkT60_procStep() && wwhd_t60_latch_pending(lkT60_procEnterKey(curProcEa), lkT60_frameTag()) != 0;
}

/* Speed chases that drive the position: frame-exact (rule (a), 2026-11-17, pairs: procDamage's
 * knock-back feeding the Bokoblin's AI). The original chases a speed (mNormalSpeed) once per frame and the frame's move
 * (posMove) then adds the frame's END speed. Chased per step with dt, the speed at the frame end is the same, but the
 * steps move with the in-frame speeds: (N - 1) / (2N) of the frame's change further per frame (knock-back 0.30 / frame
 * at N 2, 0.45 at N 4: 5 units after 20 frames). Hold form (the first-step input rule): the frame's FIRST step runs the chase for the
 * whole frame (at dt 1: the original's one chase), the frame's later steps keep the speed and give the original's
 * result (|target - speed|, what cLib_addCalc returns) without chasing; every step then moves dt of the frame's end
 * speed, so the frame's move is the original's. A chase first reached on a later step of the frame (a procedure entered
 * there) keeps the per-step form. Latch per translation unit: call site (0 .. 15), field, procedure; host memory, never
 * consulted at dt 1 (the original call, no extra guest reads). */
struct LkT60ChaseLatch {
    uint32_t stamp;
    uint32_t ea;
    int32_t proc;
    bool valid;
};
static LkT60ChaseLatch s_lkT60Chase[16];
static float s_lkT60ChaseDt = 1.0f;
/* a later step of a frame whose first step ran this site's chase (same field, same procedure): hold */
template <class P>
static inline bool lkT60_chaseHeld(int site, const void* field, const P* curProc) {
    if (t60::dt() == 1.0f || t60::first_step()) {
        return false;
    }
    const LkT60ChaseLatch& l = s_lkT60Chase[site];
    return l.valid && t60_step_counter() - l.stamp < t60::steps() && l.ea == gabi::ea(field) &&
           l.proc == (int32_t)*curProc;
}
/* before the chase: on the frame's first step the chase runs at dt 1 (the whole frame) */
static inline void lkT60_chaseOpen() {
    if (t60::dt() != 1.0f && t60::first_step()) {
        s_lkT60ChaseDt = t60_set_dt(1.0f);
    }
}
/* after the chase: the step's dt back, the site latched for the frame's later steps */
template <class P>
static inline void lkT60_chaseClose(int site, const void* field, const P* curProc) {
    if (t60::dt() == 1.0f && s_lkT60ChaseDt == 1.0f) {
        return; /* the original (dt 1 throughout) */
    }
    if (s_lkT60ChaseDt != 1.0f) {
        t60_set_dt(s_lkT60ChaseDt);
        s_lkT60ChaseDt = 1.0f;
        LkT60ChaseLatch& l = s_lkT60Chase[site];
        l.stamp = t60_step_counter();
        l.ea = gabi::ea(field);
        l.proc = (int32_t)*curProc;
        l.valid = true;
    }
}
template <class P, class R>
static inline R lkT60_chaseDone(int site, const void* field, const P* curProc, R r) {
    lkT60_chaseClose(site, field, curProc);
    return r;
}
static inline f32 lkT60_chaseRest(const be<f32>* field, f32 target) { return std::fabs(target - (f32)*field); }
/* a chase whose result is used: LKT60_CHASE(site, &mNormalSpeed, target, &mCurProc, cLib_addCalc(...)) */
#define LKT60_CHASE(site, field, target, curProc, call)                                                              \
    (lkT60_chaseHeld(site, field, curProc) ? (f64)lkT60_chaseRest(field, target)                                     \
                                           : (f64)(lkT60_chaseOpen(), lkT60_chaseDone(site, field, curProc, (call))))
/* a chase as a statement */
#define LKT60_CHASE_S(site, field, curProc, call)                                                                    \
    do {                                                                                                             \
        if (!lkT60_chaseHeld(site, field, curProc)) {                                                                \
            lkT60_chaseOpen();                                                                                       \
            call;                                                                                                    \
            lkT60_chaseClose(site, field, curProc);                                                                  \
        }                                                                                                            \
    } while (0)

/* the process-order read rule (f_pc/f_pc_t60_frame_start.h): Link's lock-on reads of a
 * converted actor LATER in the process line see its frame-start state (the eye / position A's frame reads); any other
 * target (unconverted, held at 30 Hz, earlier in the line) and N = 1 read the live state through the original call.
 * true and the angle from `from` to the target's frame-start eye (what 0) or position (what 1) when it applies. */
#include "f_pc/f_pc_t60_frame_start.h"
#include <cstring>
static inline bool lkT60_fsTargetAngleY(const void* from, const void* target, int what, s16* out) {
    uint8_t fs[12];
    if (!t60_fs_read(gabi::ea(target), what, fs))
        return false;
    gabi::Local<cXyz> p;
    std::memcpy(p.get(), fs, 12);
    *out = gabi::call<s16>(0x0200F93C /* cLib_targetAngleY */, from, p.get());
    return true;
}

/* the process-order read rule, Link's own position (PARRY pair Bokoblin unconverted: lock-on procCutA frame 50,
 * A -28813 / B step 0 -28813, tick30 step -28778): setShapeAngleToAtnActor turns the shape toward the target from Link's
 * current.pos. In A that is the frame-start position (the procedure runs before the frame's move); at N steps the later
 * steps start from the position after the earlier steps' moves, so the tick30 step's angle (cLib_addCalcAngleS with min
 * step 0x800 snaps to it) is the angle from the mid-frame position. Form: at N > 1, for a converted Link (a frame-start
 * record of this frame, f_pc_t60_frame_start.h what 3: Link has executed on this step), the angle is taken from Link's
 * frame-start position to the target's frame-start eye (process-order read, as lkT60_fsTargetAngleY) or its live eye; false (no
 * record: unconverted, held, N = 1): the original call. */
static inline bool lkT60_fsSelfTargetAngleY(const void* self, const void* target, const void* liveEye, s16* out) {
    uint8_t me[12], te[12];
    if (!t60_fs_read(gabi::ea(self), 3, me))
        return false;
    gabi::Local<cXyz> from;
    std::memcpy(from.get(), me, 12);
    if (t60_fs_read(gabi::ea(target), 0, te)) {
        gabi::Local<cXyz> eye;
        std::memcpy(eye.get(), te, 12);
        *out = gabi::call<s16>(0x0200F93C /* cLib_targetAngleY */, from.get(), eye.get());
    } else {
        *out = gabi::call<s16>(0x0200F93C /* cLib_targetAngleY */, from.get(), liveEye);
    }
    return true;
}

/* the process-order read rule in setNeckAngle (pair): checkAttentionPosAngle's look cone test
 * (the angle to the actor's eye against m34DE, 0x6000) and the look-at point it hands back read the frame-start eye of a
 * converted actor later in the line (A's frame reads its previous frame end); otherwise (unconverted, held, earlier in
 * the line, N = 1) the live eye through the original code.: on Link's tick30 step the ReDead's current-frame eye
 * put it inside the cone one frame early (daPyFlg1_UNK400, the head look-at). */
static inline bool lkT60_attnAngleY(const void* from, const void* actor, s16* out) {
    return lkT60_fsTargetAngleY(from, actor, 0, out);
}
/* the look-at point: `slot` holds the actor's &eyePos as checkAttentionPosAngle set it; when process-order read gives the frame-start
 * eye, it is copied into `local` (setNeckAngle's own stack cXyz, alive through the pose) and the slot points there */
static inline void lkT60_attnLookPoint(be<u32>* slot, cXyz* local) {
    const u32 eye = *slot;
    uint8_t fs[12];
    if (eye == 0 || !t60_fs_read(eye - 0x37C /* fopAc_ac_c::eyePos */, 0, fs))
        return;
    std::memcpy(local, fs, 12);
    *slot = gabi::ea(local);
}

/* an outside pos / angle set (daPy_lk_c::setPlayerPosAndAngle, the three forms: an event or an
 * actor placing Link) re-bases the frame-start facing of the turn-in-place tests (setBlendMoveAnime's
 * lk_frameStartAngle, procGrabWait's lk07_frameStartAngle). The set writes shape_angle.y and m34DE together: at 30 Hz the
 * next turn test reads m34DE = the set angle, so the set is no turn. At N steps the test held the facing of the frame's
 * first step; a set on a later step (the cannon minigame's start, daCanon_c::wait_proc on its tick30 step after Link's
 * first step left DEMO_TALK for WAIT) read as a turn of the whole set: the turn blend (WAITS at rate 3.211 instead of
 * 1.1) played 2.111 frames ahead from frame 631 on. Form: the set marks the port latch (one key per test, this 30 Hz
 * frame's tag); the test's next call in the frame takes m34DE (then the set angle) as the frame start and clears its
 * mark. N = 1: never set (the original). */
static inline uint32_t lkT60_angleSetKey(uint32_t m34DEEa, int test) { return m34DEEa ^ (test ? 0xC0000000u : 0x80000000u); }
static inline void lkT60_angleSet(uint32_t m34DEEa) {
    /* (the setter usually runs inside the caller's execute: an unconverted / whole-form actor at dt 1, so the gate is the
     * step count, not dt) */
    if (t60::steps() <= 1) {
        return;
    }
    wwhd_t60_latch_set(lkT60_angleSetKey(m34DEEa, 0), lkT60_frameTag());
    wwhd_t60_latch_set(lkT60_angleSetKey(m34DEEa, 1), lkT60_frameTag());
}
/* the test `test` (0 setBlendMoveAnime, 1 procGrabWait): true once after a set in this frame (the mark is cleared) */
static inline bool lkT60_angleSetTaken(uint32_t m34DEEa, int test) {
    if (t60::steps() <= 1 || !wwhd_t60_latch_pending(lkT60_angleSetKey(m34DEEa, test), lkT60_frameTag())) {
        return false;
    }
    wwhd_t60_latch_set(lkT60_angleSetKey(m34DEEa, test), ~lkT60_frameTag());
    return true;
}

/* (pot potP2 frame 295): the carried actor's frame-start placement. Link places the carried
 * actor (setGrabItemPos, end of his execute) on every step (the carryables' whole form expects it: their carried
 * picture). At 30 Hz an update that leaves the carry (freeGrabItem in a procedure init, before the placement) leaves the
 * actor at the previous frame's placement. At N steps a frame whose earlier step still carried and whose later step left
 * the carry left the actor at that earlier step's half-frame placement (potP2 frame 293: step 1 FALL placed the pot,
 * step 2 DEMO_LAVA_DAMAGE dropped it 38 units lower than A; Link's co cylinder met it a frame early, z push 50.06 @295).
 * Form: the frame's first placement notes the actor's position before it (the previous frame's placement); a release on
 * a later step of the same frame puts that position back. Game build (exported once, d_a_player_main_04.cpp); N = 1
 * never. Negative control WWHD_T60_GRABFS_NEGCTRL=1. */
#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" __attribute__((weak)) void wwhd_t60_grab_fs_note(uint32_t actor, uint32_t posEa);
extern "C" __attribute__((weak)) void wwhd_t60_grab_fs_release(uint32_t actor, uint32_t posEa);
static inline void lkT60_grabNote(uint32_t actor, uint32_t posEa) {
    if (t60::steps() > 1 && wwhd_t60_grab_fs_note)
        wwhd_t60_grab_fs_note(actor, posEa);
}
static inline void lkT60_grabRelease(uint32_t actor, uint32_t posEa) {
    if (t60::steps() > 1 && wwhd_t60_grab_fs_release)
        wwhd_t60_grab_fs_release(actor, posEa);
}
#else
static inline void lkT60_grabNote(uint32_t, uint32_t) {}
static inline void lkT60_grabRelease(uint32_t, uint32_t) {}
#endif

/* (2026-11-22; grandma pair): setDemoData's DEMO_N_WALK /
 * DEMO_N_DASH goal tests (stop within 10 of the event goal, or within 50 at rest: DEMO_N_WAIT + end the cut; the stick
 * release within 20 / 50). The original tests once per 30 Hz frame, before the frame's move, from the previous frame's
 * end. Per step, B's step 2 of frame 167 tested the half-frame position (z 117.908, inside 10 of Grandma's goal) where A's
 * frame 167 test read 119.938 and its frame 168 test 115.878: B stopped a step early, half a frame short (pos 2.03 @170,
 * anim 1.118 @171 from WAIT one step early). Form (as the placement / procChangeHeld): the tests decide on the frame's
 * first step only; the frame's later steps keep walking (the walk angle is still set every step). Negative control
 * WWHD_T60_DEMOWALK_NEGCTRL=1 (game build); dt 1: the original. */
static inline bool lkT60_demoWalkDecides() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool negctrl = [] { const char* e = getenv("WWHD_T60_DEMOWALK_NEGCTRL"); return e && e[0] == '1'; }();
    if (negctrl)
        return true;
#endif
    return t60::dt() == 1.0f || t60::first_step();
}
/* (an event end: BS_SHUTTER_START cut 454, 003n_dash): the first-step-only
 * tests above left the frame's later steps without the slow-down's stick release: setDemoData sets mDemo.mStick = 1 for a
 * system demo on every step before the tests, so the tick30 step ran with stick 1 where A's whole frame has 0 (the frame-end
 * speedF 7.13 / 4.12 / 2.37 against A's 9.98 / 7.10 / 0.00 from mNormalSpeed 12 / 9.5 / 7): Link reached the goal (cut end
 * flag 1115) a frame late, the shutter-start demo ended a frame late and the day clock resumed a frame late (KANKYO). Form:
 * the first step's outcome (stop / stick release) is latched and the frame's later steps apply it again, so every step of
 * the frame sees A's one decision. Host state only (the split-actor state rule (1)); NC WWHD_T60_DEMOSTICK_NEGCTRL=1 (game build: the
 * later steps decide nothing, as before); dt 1: the original. */
struct LkT60DemoWalk {
    uint32_t stamp;
    bool valid, stop, release;
};
static LkT60DemoWalk s_lkT60DemoWalk;
static inline void lkT60_demoWalkNote(bool stop, bool release) {
    if (t60::dt() == 1.0f || !t60::first_step())
        return;
    s_lkT60DemoWalk = {t60_step_counter(), true, stop, release};
}
/* a later step of a frame whose first step decided: true and that decision */
static inline bool lkT60_demoWalkHeld(bool* stop, bool* release) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool negctrl = [] { const char* e = getenv("WWHD_T60_DEMOSTICK_NEGCTRL"); return e && e[0] == '1'; }();
    if (negctrl)
        return false;
#endif
    if (t60::dt() == 1.0f || t60::first_step())
        return false;
    const LkT60DemoWalk& h = s_lkT60DemoWalk;
    if (!h.valid || t60_step_counter() - h.stamp >= t60::steps())
        return false;
    *stop = h.stop;
    *release = h.release;
    return true;
}
