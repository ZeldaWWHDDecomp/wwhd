/* d_a_npc_kamome_t60.h (WWHD, true 60 build, 2026-11-21): the Hyoi seagull's two forms.
 * Decomp build only. Separate file: the actor sources
 * keep thin calls.
 *
 * while the player flies the gull (dComIfGp_getPlayer(0) == this) the gull IS the player: it runs
 * per step like Link (Link's conversion rules), so the per-step camera follows it; while it is NOT the player it takes
 * the NPC whole form (the whole-frame form rule, d/t60_npc.h). The form is chosen on the frame's FIRST step from that test and held
 * over the frame (never switched mid-frame; a later step of a frame whose first step this actor missed tests live).
 *
 * The player (per-step) form, daNpc_kam_c::execute at dt 1/N:
 *   - input per step (the mixed-pair hit-flag rule (2)): the A / R / START triggers through c_API_controller's cpadTrig (a converted
 *     context takes the step's own presses), the stick per step;
 *   - speeds hold (rule (a), 2026-11-17, Link's LKT60_CHASE form): the speed chase (speedF toward mTargetSpeedF by
 *     mAccelF) and the turn / pitch rate chases (mAngVelY / mAngVelX, cLib_addCalcAngleS) change once a frame: they run
 *     whole on the frame's first step at dt 1 and the later steps keep them; every step moves dt of that speed
 *     (fopAcM_posMove at dt), so the frame's move is the original's on the frame-end speed;
 *   - the steering (area-out test, stick / camera target, yaw / pitch / roll chases, pitch lock) once a frame on the
 *     tick30 step at dt 1, the frame's velocity (and the ground / water / floor checks with their pitch lock) on the
 *     first step from the frame-start place, held, and the frame's x / y / z move exact (kamT60::steerStep / moveExact;
 * the frame form of the flight), so every step flies the frame's velocity;
 *   - counts on the first step (the first-step AI-timer rule; the keyProc flap / energy / exhaustion timers, the pitch lock, the sing
 *     timer, the damage timer, the damage fog), the exhaustion branch held over the frame (t60c::held); the damage spin
 *     (0x2000 a frame) in step shares, exact per frame (t60c::share);
 *   - the action dispatch under the 30 Hz visual-motion rule (2) (t60c::procBegin / procEnd): an action set on a step first runs its body
 *     on the next frame's first step;
 *   - a new animation set on a step before the tick30 step is a request (Moblin request form): the old animation
 *     plays to the frame end, the morf set is made on the tick30 step right after its play (execute plays the morf
 *     BEFORE the action dispatch: the original's set blends from the old animation's frame-end pose);
 *   - the colliders (body sphere, beak capsule, mass) registered once per frame on the tick30 step (the one-sweep-per-frame attack rule), the
 *     event order on the tick30 step (the event manager's frame); the collision push (mStts.m_cc_move from the tick30
 *     Move) is moved dt a step and cleared on the tick30 step, so the frame takes it whole once;
 *   - ground / roof / water / line checks and the keep-out circle per step (the per-step ground-check rule);
 *   - the event branch (eventProc: demo accept, cut init / action / cutEnd, endCheck) once a frame on the tick30 step at
 *     dt 1 (like the other event procs: the event manager's frame; pairs: per step it
 *     halved the area-out event's wait timer and fly-back), the earlier steps of an event frame skip the dispatch as the
 *     original's event frame does (kamT60::eventStep).
 * dt 1 (N 1, the 30 Hz fallback in events, the verification harness's dt-1 check pin): every helper is the original and makes
 * no extra guest read. Negative control (T60C_NEGCTRL): every t60c helper takes the per-step form and the whole form is
 * off (the execute on every step at dt). */
#pragma once
#include "d/t60conv.h"
#include "d/t60_npc.h"
#include <new>

namespace kamT60 {

/* the frame's form: true = the player form (per step), false = the NPC whole form; dt 1: never asked. Player 0 is read
 * from its guest word (g_dComIfG_gameInfo 0x1046F0B0 + play 0x12A0 + mPlayerInfo[0] 0x488C), not through dComIfGp_get():
 * a call the original does not make here would shift the N-step spec harness's per-step mock answers */
static inline bool playerFrame(daNpc_kam_c* a) {
    return t60c::held((u32)*gabi::at<be<u32>>(0x10474BDCu) == gabi::ea(a), 0x7A, a);
}

/* the execute's form object: the NPC frames run in t60w::Guard (whole on the tick30 step), the player frames per step.
 * Host objects: templates (autoglue turns a non-template class-typed local into a guest stack slot) */
template <class A> struct Form {
    using G = t60w::Guard<A>;
    alignas(G) unsigned char buf[sizeof(G)];
    G* g;
    explicit Form(A* a) : g(nullptr) {
#ifndef T60C_NEGCTRL
        if (t60c::on() && !playerFrame(a))
            g = new (buf) G(a, t60npc::Logic<A>::kOps);
#else
        (void)a;
#endif
    }
    ~Form() {
        if (g)
            g->~G();
    }
    bool skip() const { return g && g->skip; }
    bool player() const { return t60c::on() && g == nullptr; }
    void model() {
        if (g)
            g->model = true;
    }
};

/* dt 1 for a block that changes once a frame (the hold form), back after it */
template <int = 0> struct Dt1 {
    f32 prev;
    bool set;
    Dt1() : prev(1.0f), set(t60c::on()) {
        if (set)
            prev = t60_set_dt(1.0f);
    }
    ~Dt1() {
        if (set)
            t60_set_dt(prev);
    }
};

/* The flight's frame form (the pair kfP flew its own path: the turn half a
 * frame late, the pitch lock one frame early off a mid-frame water / ground hit, x ~ -117000 losing an f32 step per frame
 * on the two half moves; its area-out event came 2 frames late where the unconverted control's came 2 early). The steering
 * (the area-out test, the stick / camera target, the yaw / pitch / roll chases, the pitch lock) runs once a frame on the
 * tick30 step at dt 1, from the frame-end position and hit words as the original (it steers after the frame's move); the
 * earlier steps fly on the frame's held angles, so every step moves along the original frame velocity (the hold form of
 * rule (a) for the angles too). dt 1 / NPC form (whole, dt 1): every step (the original). NEGCTRL: every step. */
static inline bool steerStep() {
#ifdef T60C_NEGCTRL
    return true;
#else
    return !t60c::on() || t60::tick30();
#endif
}
/* the frame's move exact (t60c::moveBegin / moveEnd, the tornado rule): the tick30 step lands x / y / z on the
 * frame-start place + the frame's speed (held from the first step), the original's single add. Only without a collision push (m_cc_move 0: the original adds it
 * after the speed; constant over the frame, from the tick30 Move); with one the per-step move stays */
template <class A> static inline bool moveExact(A* a) {
    return t60c::on() && (f32)a->mStts.m_cc_move.x == 0.0f && (f32)a->mStts.m_cc_move.z == 0.0f;
}

/* the action word (a guest member pointer copy: read through its be<> fields) */
static inline u32 actWord(const KamomeAction* p) {
    const u32 w[3] = {(u32)(s32)(s16)p->d, (u32)(s32)(s16)p->i, (u32)p->f};
    return t60c::procWord(w);
}
static inline bool actBegin(daNpc_kam_c* a, const KamomeAction* p) {
    return !t60c::on() || t60c::procBegin(0x7B, a, actWord(p));
}
static inline void actEnd(daNpc_kam_c* a, const KamomeAction* p) {
    if (t60c::on())
        t60c::procEnd(0x7B, a, actWord(p));
}

/* a timer counted on the frame's first step (the first-step AI-timer rule); the later steps read it */
static inline s32 timer(be<s16>* t) { return t60c::first() ? kam_calcTimer(t) : (s32)(s16)*t; }

/* New animation (2026-11-21, Moblin request form): execute plays the morf BEFORE the
 * dispatch, so the original's setAnm always follows the frame's whole play and the morf blends from the old animation's
 * frame-end pose. A set on a step before the tick30 step is a request: setAnm keeps its logic words at once (mAnmIdx,
 * mAnmTblIdx, mPrevMorfFrame / mReachedAnimEnd 0) and marks the latch (the morf's frame control, port-owned host latch,
 * this 30 Hz frame); the later steps play the OLD animation on for the pose only (no logic word: playOld); the tick30
 * step, right after that play, makes the requested morf set (setAnm(mAnmIdx) again: the request is mAnmIdx itself,
 * shared by both units without host state); a set on the tick30 step itself is the original's. dt 1: never. */
static inline u32 anmKey(daNpc_kam_c* a) { return gabi::ea(&a->mpMorf->mFrameCtrl); /* the morf's frame control */ }
static inline bool anmPending(daNpc_kam_c* a) {
    return t60c::on() && wwhd_t60_latch_pending(anmKey(a), t60c::frameTag()) != 0;
}
/* setAnm, after its logic words: true = a request (no morf set now) */
static inline bool anmRequest(daNpc_kam_c* a) {
#ifdef T60C_NEGCTRL
    (void)a;
    return false;
#else
    if (!t60c::on() || t60::tick30())
        return false;
    wwhd_t60_latch_set(anmKey(a), t60c::frameTag());
    return true;
#endif
}
/* the old animation's play on a later step of a request's frame: the pose (and its sounds), no logic word */
static inline void playOld(daNpc_kam_c* a) {
    u32 mtrlSndId = 0;
    u32 gnd = gabi::ea(static_cast<cBgS_PolyInfo*>(&a->mAcch.m_gnd));
    if (a->mAcch.ChkGroundHit() && kam_checkPolySafe(dComIfG_Bgsp(), gnd))
        mtrlSndId = kam_getMaterialSound(dComIfG_Bgsp(), gnd);
    kam_morfPlay(a->mpMorf.get(), &a->eyePos, mtrlSndId, dComIfGp_getReverb(a->current.roomNo));
}
/* the tick30 step after its play: the pending request's morf set (once: the latch is this frame's) */
static inline void anmApply(daNpc_kam_c* a) {
    if (!t60c::on() || !t60::tick30() || !anmPending(a))
        return;
    a->mAnmTblIdx = (s8)-1; /* setAnm sets it again (its same-animation check must not skip the set) */
    a->setAnm((s32)a->mAnmIdx);
}

/* execute's eventProc call. The player form: the event branch once a frame, whole on the
 * tick30 step at dt 1; an earlier step only answers eventProc's "in an event" (event_runCheck && !checkCommandTalk, the
 * latter without its mEventState write: the tick30 step's call makes it), so the dispatch is skipped on the event's
 * frames as in the original. NPC form / dt 1: the original call. NEGCTRL: the call on every step (the kam2 form). */
template <class A, class F> static inline BOOL eventStep(A* a, const F& f) {
#ifndef T60C_NEGCTRL
    if (f.player()) {
        if (!t60::tick30()) {
            /* dEvt_control_c mMode / mTalkButton from their guest words (g_dComIfG_gameInfo 0x1046F0B0 + 0x5292 / 0x52B0 = save
             * info 0x12A0 + play mEvtCtrl 0x3F30 + 0xC2 / 0xE0, as PLAY_PLAYER 0x5B2C; added 0x12A0 twice),
             * not through dComIfGp_get: a call the original does not make on these steps would shift the N-step spec
             * harness's per-step mock answers (as playerFrame) */
            if (gabi::load<u8>(0x10474342u) == 0)
                return FALSE;
            if (a->eventInfo.checkCommandTalk() && !((u32)(gabi::load<u8>(0x10474360u) - 1) <= 3))
                return FALSE;
            return TRUE;
        }
        Dt1<> d;
        return a->eventProc();
    }
#else
    (void)f;
#endif
    return a->eventProc();
}

}  // namespace kamT60
