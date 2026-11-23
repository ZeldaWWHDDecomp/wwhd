/* d_camera_run_t60.h (WWHD, true 60 build): the event engine's whole form inside the converted camera's Run
 *(crane pair; final form for the event cameras:;
 * dt = 1/N).
 *
 * A camera style whose engine is dCamera_c::eventCamera (engine 11, 024FF164; only style 4 EN00, e.g. the ship's crane
 * camera, type 3 mode 0) runs a d_ev_camera* action (callEventEngine). Outside an event-manager event (a camera-set
 * event, chkFlag(0x20000000)) the game does not fall back to N = 1, so Run's engine dispatch called that body on every
 * step of the converted camera with its per-frame ratios (pcr11 /: the view moved 2-4x as fast). The event
 * cameras keep the whole form (no per-step conversion of the d_ev_camera* smoothing): the converted camera runs the
 * event engine on the tick30 step only, with dt = 1 (the step an unconverted process runs on, so the body and every
 * central primitive it reaches keep the original per-frame arithmetic, on the frame-end world as in A); on the frame's
 * other steps the engine is not called (the event view holds, as NotRun's event path does) and Run's result is the
 * last event-engine result. The picture between frame ends is the port's camera blend (frame interpolation, t60_draw camera).
 * Run's other per-step work (the bank chase, the floor clamp, bumpCheck) is unchanged. Outside the engine call the
 * d_ev_camera* code is reached only by StartEventCamera / EndEventCamera (Link's procedure inits and ends: event
 * parameters and flags, read by the next engine call) and by the talk camera's getEv*Data reads (no state); its
 * player-status reads (SHIP_RIDE 0x10000, SWIM 0x100000) are states, not pad input (the first-step input rule not concerned).
 * dt = 1 (N = 1, the normal build's semantics): every call, the original. */
#pragma once
#include "d/d_camera_key_t60.h"
#include "true60.h"
#include <cstring>

namespace camera_run_t60 {
/* host memory, this unit, one camera: Run's result from the last event-engine call (the held steps return it) */
static camera_key_t60::PerCam<u32> s_eventEngineRes;

/* the step runs the style's engine: not an event-engine style, dt = 1, or the tick30 step */
static inline bool eventEngineRuns(u32 algorithm, u32 eventAlgorithm) {
    return algorithm != eventAlgorithm || t60::dt() == 1.0f || t60::tick30();
}
/* the event engine's call context: dt 1 for the call (the previous dt is returned; put it back with endEventEngine) */
static inline float beginEventEngine(u32 algorithm, u32 eventAlgorithm) {
    return algorithm == eventAlgorithm ? t60_set_dt(1.0f) : t60::dt();
}
static inline u32 endEventEngine(u32 algorithm, u32 eventAlgorithm, float previousDt, u32 res) {
    if (algorithm == eventAlgorithm) {
        t60_set_dt(previousDt);
        s_eventEngineRes() = res;
    }
    return res;
}
static inline u32 heldEventEngineRes() { return s_eventEngineRes(); }

/* WaitAnyKey hold release (pair: type 25 / style 84 pull-back a 0.75-frame lead;
 * dt = 1/N the first-step AI-timer rule's cross-party order, the first-step input rule). An event camera with "WaitAnyKey" sets mEventFlags 0x200000;
 * Run's hold path then calls no engine and counts no timer until a stick / C-stick / trigger input, the monitor speed
 * (> 10), leaving the floor or a moving background clears the flag. In the original the clearing call is the frame's one
 * Run: the engine resumes on the NEXT frame. At N steps the input is read on the frame's first step (first-step input), so the
 * release fell on step 1 and the engine (followCamera's pull-back, its mode timer) started on step 2: (N - 1) / N frame
 * early. The release frame's remaining steps keep the hold path (and the ship-rock offset it skips); the engine resumes
 * on the next frame's first step, the original order. A release on the tick30 step needs no latch. The shared set latch (true60.h,
 * port-owned) keyed by the camera's mEventFlags and tagged with the 30 Hz frame counter g_Counter.mCounter0 (counts on
 * tick30 only), so a frame whose tick30 step the camera misses leaves nothing behind. dt = 1 (N = 1, the normal build's semantics): no latch, the original. */
static inline uint32_t holdFrameTag() { return *gabi::at<be<uint32_t>>(0x101FF558); /* g_Counter.mCounter0 */ }
/* true while the step lies after a hold release in the same 30 Hz frame */
static inline bool holdReleaseStep(uint32_t flagsEa) {
    return t60::dt() != 1.0f && wwhd_t60_latch_pending(flagsEa, holdFrameTag()) != 0;
}
/* the hold path cleared 0x200000 on this step */
static inline void noteHoldRelease(uint32_t flagsEa) {
    if (t60::dt() != 1.0f && !t60::tick30())
        wwhd_t60_latch_set(flagsEa, holdFrameTag());
}

/* The forward tilt (mForwardTiltOffset) per 30 Hz frame
 *. Per original frame Run chases it toward forwardCheckAngle by FwdCushion (x + (a - x) c) and
 * followCamera's flying branch (Cb1 / Medli flying) then multiplies it by 0.33. At N steps both ran per step: the
 * 0.33 on every call decayed the tilt 0.33^N per frame (and the per-step chase toward a target it then lost 2/3 of).
 * The per-frame form: on the frame's first step Run records the ORIGINAL frame's chase (the original routines with the
 * unconverted FwdCushion, from the frame-start tilt and target); the flying branch takes that value, scales it by 0.33
 * once and holds the result on the frame's later steps (so every read in the frame sees the original frame's value, and
 * the frame ends on it). Host memory, one camera. dt = 1: not used (the original per call). */
struct FwdFrame {
    uint32_t step; /* t60_step_counter of the step that recorded val */
    s16 val;
    bool valid;
};
inline camera_key_t60::PerCam<FwdFrame> g_fwdFrame;
static inline void noteFwdFrame(s16 v) {
    g_fwdFrame().step = t60_step_counter();
    g_fwdFrame().val = v;
    g_fwdFrame().valid = true;
}
/* the frame-start tilt (first step) for the tick30 step's one original chase (d_camera_run.cpp) */
struct FwdStart {
    uint32_t step; /* t60_step_counter of the frame's first step */
    s16 val;
    bool valid;
};
inline camera_key_t60::PerCam<FwdStart> g_fwdStart;
static inline void noteFwdStart(s16 v) {
    g_fwdStart().step = t60_step_counter();
    g_fwdStart().val = v;
    g_fwdStart().valid = true;
}
/* the tick30 step of a frame whose first step noted its start tilt: *v = that tilt */
static inline bool fwdStartTick30(s16* v) {
    if (t60::dt() == 1.0f || !t60::tick30() || t60::first_step())
        return false;
    FwdStart& f = g_fwdStart();
    if (!f.valid || t60_step_counter() - f.step >= (uint32_t)t60::steps())
        return false;
    f.valid = false;
    *v = f.val;
    return true;
}
/* the original frame's chased tilt, when Run recorded it on the current step */
static inline bool fwdFrameNow(s16* v) {
    if (!(g_fwdFrame().valid && g_fwdFrame().step == t60_step_counter()))
        return false;
    *v = g_fwdFrame().val;
    return true;
}

/* Mode / style change on the tick30 step: the rollback form (boomerang pair frame 118,
 * the speed-ramp position-gap rule (2) / camera notes). In the original the frame's one Run decides the camera type / mode / style from the
 * frame-end world (the attention's lock release lands on the tick30 step) and runs the new style's engine
 * for the whole frame. At N steps the earlier steps still ran the old style (bm1: the lock-on camera turned +1.7 deg more
 * on step 0, a yaw the follow camera then keeps). When the mode or style decided on the tick30 step differs from the one
 * the frame's earlier steps ran, Run restores the camera's frame-start state (the dCamera_c object, 0x8E0 bytes, and its
 * dComIfGp com status block, 0x34 bytes; taken at the top of Run on the frame's first step) and runs the whole frame once
 * in the new mode at dt 1, exact as in A. Host memory, one camera. dt = 1: never (the original). */
constexpr uint32_t kCamObjSize = 0x8E0;
constexpr uint32_t kCamComSize = 0x34;
struct RunFrameStart {
    uint32_t stamp;    /* t60_step_counter of the frame's first step */
    uint32_t prevStep; /* the step that recorded prevMode / prevStyle */
    s32 prevMode, prevStyle;
    bool valid;
    uint8_t obj[kCamObjSize];
    uint8_t com[kCamComSize];
    /* (on the YAW whole form) the one store Run's own body makes outside the
     * camera object and its com block: the environment light's HD zoom (dKy_getEnvlight + 0x104C, f32: 0.7 / 1.0, or
     * cLib_addCalc toward 0.8 / 1.0, a read-modify-write). Noted at its first write of the frame's first step, put back
     * with obj / com, so a rerun from the frame start eases it once per frame as A does. */
    uint32_t zoomEa, zoomBits;
    bool zoomNoted;
    /* (on cameraRunSubjectStart, run.cpp style change into SUBJECT) the subject
     * start 026185BC writes the pad object's subject base (pad + 0x224, 3x3 f32 copy of *(pad + 0x220)) and its inverse
     * (pad + 0x248): 0x48 bytes outside obj / com. A step-0 call is not undone by the rollback (the rerun from the frame
     * start may not switch into SUBJECT again) and its value would reach the tick30 step's readers before the camera's
     * rerun. Noted before the frame's first call, put back with obj / com. */
    uint32_t padEa;
    bool padNoted;
    uint8_t pad[0x48];
};
inline camera_key_t60::PerCam<RunFrameStart> g_runFrameStart;
inline camera_key_t60::PerCam<uint32_t> g_eventWholeFrame{0xFFFFFFFFu}; /* the frame (first-step stamp) held for the whole form */
/* guest memory copies in 32-bit words through gabi load / store (the sizes are multiples of 4; the bytes keep the guest
 * order): no raw host access to guest memory, which the verify harness does not map (N-step spec of Run, 2026-11-18) */
static inline void copyFromGuest(uint8_t* dst, uint32_t ea, uint32_t n) {
    for (uint32_t i = 0; i < n; i += 4) {
        const uint32_t w = gabi::load<uint32_t>(ea + i);
        dst[i] = (uint8_t)(w >> 24), dst[i + 1] = (uint8_t)(w >> 16), dst[i + 2] = (uint8_t)(w >> 8), dst[i + 3] = (uint8_t)w;
    }
}
static inline void copyToGuest(uint32_t ea, const uint8_t* src, uint32_t n) {
    for (uint32_t i = 0; i < n; i += 4)
        gabi::store<uint32_t>(ea + i, (uint32_t)src[i] << 24 | (uint32_t)src[i + 1] << 16 | (uint32_t)src[i + 2] << 8 | src[i + 3]);
}
/* top of Run: the frame's first step records the frame-start state */
static inline void noteFrameStart(void* self, uint32_t comEa) {
    if (t60::dt() == 1.0f || !t60::first_step())
        return;
    copyFromGuest(g_runFrameStart().obj, gabi::ea(self), kCamObjSize);
    copyFromGuest(g_runFrameStart().com, comEa, kCamComSize);
    g_runFrameStart().stamp = t60_step_counter();
    g_runFrameStart().valid = true;
    g_runFrameStart().zoomNoted = false;
    g_runFrameStart().padNoted = false;
    g_eventWholeFrame() = 0xFFFFFFFFu; /* a new frame: no whole-form hold yet (eventWholeEntry) */
}
/* after the type / mode / style decision: true when the tick30 step changed the mode or style the frame's previous
 * step ran (consecutive steps of the frame whose first step recorded the state); records this step's mode / style */
static inline bool modeChangedOnTick30(s32 mode, s32 style) {
    if (t60::dt() == 1.0f)
        return false;
    const uint32_t now = t60_step_counter();
    const bool prevOk = g_runFrameStart().valid && g_runFrameStart().prevStep + 1 == now;
    const bool changed = prevOk && t60::tick30() && !t60::first_step() &&
                         now - g_runFrameStart().stamp == (uint32_t)(t60::steps() - 1) &&
                         (mode != g_runFrameStart().prevMode || style != g_runFrameStart().prevStyle);
    g_runFrameStart().prevStep = now;
    g_runFrameStart().prevMode = mode;
    g_runFrameStart().prevStyle = style;
    return changed;
}
/* Run's env-light zoom block on the frame's first step (zoomEa: the field's guest address, from Run's own env pointer):
 * the frame-start value, once per frame */
static inline void noteZoom(uint32_t zoomEa) {
    RunFrameStart& f = g_runFrameStart();
    if (t60::dt() == 1.0f || !t60::first_step() || !f.valid || f.zoomNoted || f.stamp != t60_step_counter())
        return;
    f.zoomEa = zoomEa;
    f.zoomBits = gabi::load<uint32_t>(zoomEa);
    f.zoomNoted = true;
}
/* before cameraRunSubjectStart (padEa: the pad object's guest address, Run's padObject): the subject base / inverse
 * (pad + 0x224, 0x48 bytes) before the frame's first call, on any step of the frame (a tick30-step call before a rollback
 * is undone too) */
constexpr uint32_t kSubjectBase = 0x224, kSubjectSize = 0x48;
static inline void noteSubjectStart(uint32_t padEa) {
    RunFrameStart& f = g_runFrameStart();
    if (t60::dt() == 1.0f || padEa == 0 || !f.valid || f.padNoted || t60_step_counter() - f.stamp >= (uint32_t)t60::steps())
        return;
    f.padEa = padEa;
    copyFromGuest(f.pad, padEa + kSubjectBase, kSubjectSize);
    f.padNoted = true;
}
/* Run's stores outside obj / com: the env light's HD zoom and the pad's subject base */
static inline void restoreZoom() {
    if (g_runFrameStart().zoomNoted)
        gabi::store<uint32_t>(g_runFrameStart().zoomEa, g_runFrameStart().zoomBits);
    if (g_runFrameStart().padNoted)
        copyToGuest(g_runFrameStart().padEa + kSubjectBase, g_runFrameStart().pad, kSubjectSize);
}
/* the rollback: the frame-start camera state back (the caller then runs Run once at dt 1) */
static inline void restoreFrameStart(void* self, uint32_t comEa) {
    copyToGuest(gabi::ea(self), g_runFrameStart().obj, kCamObjSize);
    copyToGuest(comEa, g_runFrameStart().com, kCamComSize);
    restoreZoom();
}
/* The event cameras' whole form, Run level (ET pair evcc_et: the door event's
 * fixedFrameEvCamera cut started 94 units off A because the eye's chase toward the event view (bumpCheck cushions and the
 * other per-step post-engine work) ran per step while the event view only changed on the tick30 step). While an
 * event-engine style runs, the whole of Run runs once per 30 Hz frame on the tick30 step at dt 1 (engine and the
 * post-engine per-frame work: bumpCheck, the eye / bank / centre chases, the shake, the timers); the frame's earlier
 * steps change nothing in the camera (the picture is the port's frame interpolation camera blend). Normal styles keep the per-step
 * form. Switch-in: a style decision on an earlier step that enters an event-engine style puts the frame-start state back
 * and holds; the tick30 step then runs the whole frame at dt 1 from the frame start (the decision taken again on the
 * frame-end world, as in A). Switch-out: a frame that starts in an event-engine style is held until the tick30 step,
 * whose whole run at dt 1 takes the switch and runs the new style for the frame (as A). dt = 1: never. */
/* top of Run (after noteFrameStart): 1 = hold this step (return the held result), 2 = run the whole frame now at dt 1 */
static inline int eventWholeEntry(bool eventStyle) {
    if (t60::dt() == 1.0f || !g_runFrameStart().valid)
        return 0;
    const bool latched = g_eventWholeFrame() == g_runFrameStart().stamp;
    if (!eventStyle && !latched)
        return 0;
    if (!t60::tick30()) {
        g_eventWholeFrame() = g_runFrameStart().stamp;
        return 1;
    }
    g_eventWholeFrame() = 0xFFFFFFFFu;
    return 2;
}
/* after the style decision: an earlier step entered an event-engine style (the caller puts the frame start back) */
static inline bool eventWholeSwitchIn(bool eventStyle) {
    if (t60::dt() == 1.0f || t60::tick30() || !eventStyle || !g_runFrameStart().valid)
        return false;
    g_eventWholeFrame() = g_runFrameStart().stamp;
    return true;
}
/* The ship rock and the camera's sound requests per 30 Hz frame (2026-11-19, tool owner's long-idle pairs /
 * otas2 on: the camera sound 0x381B JA_SE_ATM_PRT_SHIP_CREAK, Run's mSeRequest bit 2, every 132 frames at
 * 30 Hz, every 65 at N 2). Run's ship-rock block calls daObjPirateship::getShipOffsetY (025269A8), which eases
 * mShipRockSpeed and adds it to mShipRockAngle on every call: once per original frame, once per step in the converted
 * camera, so the rock (the view's centre offset and its creak edge, mShipOffsetYDelta < 0 -> dy > 0) ran N times as
 * fast. Form: (1) the block runs on the frame's first step only (the frame's one rock step, its whole delta applied
 * there; the later steps keep the offset); (2) Run's sound requests (mSeRequest bits 0-2: mode / style changes and the
 * creak) are gathered over the frame's steps and their edges (against the previous frame's set, mSeRequestLast) are
 * taken once, on the tick30 step (the end of the original frame's Run): a request made on any step of the frame is the
 * frame's request, as in A's one call. Host memory, one per camera body (d_camera_key_t60.h). dt = 1: the original. */
static inline bool shipRockStep() { return t60::dt() == 1.0f || t60::first_step(); }
/* (rd72: the block on B's first step logged dt 0.5 inside getShipOffsetY) the frame's one rock step is
 * the whole original frame's rock: daObjPirateship::getShipOffsetY's cLib_addCalcAngleS2 (speed eased toward 0x180 /
 * 0x280 by 1/16 of the gap) took the converted path at the step's dt 0.5 (256 x (1 - (15/16)^0.5) = 8.1 -> 9 instead
 * of the original 16: the rock's speed eased at about half the rate, ~1% fast drift). The step runs the frame's whole
 * rock, so it runs at dt 1 (the original), the step's dt put back after it. dt = 1: no change. Plain calls (glue). */
static inline float shipRockDtBegin() { return t60_set_dt(1.0f); }
static inline void shipRockDtEnd(float previous) { t60_set_dt(previous); }
inline camera_key_t60::PerCam<u32> g_seFrameReq;
/* before the request edges: true when this step takes them (req then holds the frame's requests) */
static inline bool seFrameEdges(be<s32>* req) {
    if (t60::dt() == 1.0f)
        return true;
    g_seFrameReq() |= (u32)*req;
    if (!t60::tick30())
        return false;
    *req = (s32)g_seFrameReq();
    g_seFrameReq() = 0;
    return true;
}
} // namespace camera_run_t60
