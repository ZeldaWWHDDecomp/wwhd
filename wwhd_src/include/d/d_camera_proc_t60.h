/* d_camera_proc_t60.h (WWHD, true 60 build; 2026-11-17): camera_draw's audio calls on a general step
 * length (dt = 1/N, the first-step AI-timer rule). Decomp build only.
 *
 * camera_draw hands the audio system its camera every draw: mDoAud_getCameraInfo (JAIZelBasic::getCameraInfo counts
 * mNoCameraTimer down by one per call; seaEnvSePlay, riverSePlay and changeSeaBgm wait for it), the camera's ground
 * material / group (setCameraGroupInfo can switch the island's audio scene) and the ground polygon position. In the
 * original that is once per 30 Hz frame, after the frame's logic.
 *
 * At N > 1 the game draws after every step, so the timer ran N times as fast: after a state load at Forest Haven the
 * river ambience (JA_SE_ATM_RIVER_*, 13 positions) started 5 frames early in the om pairs (vom7n2: frame 3 instead of
 * 8 after the origin, 65 unmatched starts). The audio itself runs on the tick30 step.
 *
 * Form: the calls run on the draw after the frame's tick30 step only. At N = 1 every step is a tick30 step: the
 * original.
 *
 * The audio hand-over: the tick30 pass calls camera_draw twice for one camera: the
 * port's exact pre-run (the step's exact camera, interp.cpp exact_prerun) and then the shown draw, which for a converted
 * camera is the picture's camera at the drawn world time (t60::draw::camera). The listener (port hook 025E1B44) took
 * both, so the shown draw's camera was what the next frame's sounds heard. On a camera cut the picture still shows the
 * pre-cut camera on that pass: the listener kept the old eye for one more frame (Edaichi stage-entry demo
 * cut at f90 under EVENTS60: the torches' level-object mix JA_SE_OBJ_TORCH_BURNING 0x6103, JAIZelBasic::processLevObjSE
 * 02021440, got vol 0 on f91 and started on f92, A f91). The 30 Hz game hands over the frame's exact camera once.
 * Form at N > 1: the first hand-over of a step per camera (the exact pre-run where there is one) only; a later draw of
 * the same camera on the same step skips the audio calls. N = 1: unchanged. NC: WWHD_T60_CAMAUD_NEGCTRL=1. */
#pragma once
#include <cstdint>
#include <cstdlib>
#include "true60.h"
#include "d/d_camera_key_t60.h"

/* true: this camera draw hands the audio system the frame's camera (camId: fopCamM_GetParam, the audio camera index) */
static inline bool camera_t60_audio_pass(s32 camId) {
    if (!t60::tick30())
        return false;
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool negctrl = [] { const char* e = getenv("WWHD_T60_CAMAUD_NEGCTRL"); return e && e[0] == '1'; }();
    if (negctrl || t60::steps() <= 1)
        return true;
    /* this TU's own latch (camera_draw is the only user): the step counter of each camera index's last hand-over */
    static uint32_t s_handed[4] = {~0u, ~0u, ~0u, ~0u};
    const uint32_t k = (uint32_t)camId & 3u;
    if (s_handed[k] == t60_step_counter())
        return false;
    s_handed[k] = t60_step_counter();
#else
    (void)camId;
#endif
    return true;
}

/* (bomb per-step helper 2026-11-24): the letterbox bars. camera_execute's CalcTrimSize eases mTrimHeight by
 * 0.25 toward the bar height (Vista / cinema scope) or back toward 0, once a 30 Hz frame in the original; camera_draw sets
 * the window's scissor from it (720 - 2 x trim high), and the next frame's Run reads that scissor: bumpCheck's lock-on
 * sight fit (radiusActorInSight's vertical half-angle from the scissor height). At N > 1 the ease ran on every step (N 2
 * and N 4 alike): the bars grew N times as fast, and the frame's Run saw a smaller scissor than A's (throw route N 4,
 * lock-on on the thrown bomb: scissor height 552 vs A's 566 in frame 93, the sight test answered 1.797 vs 0, the fit
 * radius started a frame early, the eye 3.70 off at frame 96).
 * Form. Logic: CalcTrimSize on the tick30 step only (the frame's whole ease from the frame start, with the frame-end trim
 * size and event state). Picture (2026-11-24, no 30 Hz staircase): on an earlier step k of N, camera_execute runs
 * CalcTrimSize on the frame start's height and puts it back (the frame end it predicts from this step's state; the port's
 * letterbox points run as they did per step before), and that step's draw sets the scissor at k/N between the frame
 * start's and the predicted frame end's height. Every Run at N > 1 first sets its window's scissor from the logic height
 * (the frame start's until the tick30 step), so the reads in the frame's Runs see A's scissor, not a picture's.
 * N = 1: never consulted. NC: WWHD_T60_CAMTRIM_NEGCTRL=1 (the ease on every step, no picture form, no scissor reset). */
namespace camera_trim_t60 {
static inline bool nc() {
#if defined(GABI_GAME_BUILD) && GABI_T60
    static const bool v = [] { const char* e = getenv("WWHD_T60_CAMTRIM_NEGCTRL"); return e && e[0] == '1'; }();
    return v;
#else
    return false;
#endif
}
struct TrimPic {
    uint32_t firstStep; /* the frame's first step (camera_execute) */
    uint32_t picStep;   /* the step the picture height was noted on */
    f32 pic;            /* the picture's bar height for that step's draw */
    bool valid;
};
inline camera_key_t60::PerCam<TrimPic> g_trim;
/* the converted camera at N > 1 with the form on */
static inline bool on() { return t60::dt() != 1.0f && !nc(); }
/* camera_execute: the frame's first step */
static inline void noteStep() {
    if (on() && t60::first_step())
        g_trim().firstStep = t60_step_counter();
}
/* camera_execute after Run: the logic ease on this step (N 1, tick30, NC) */
static inline bool easeStep() { return !on() || t60::tick30(); }
/* camera_execute on an earlier step: the frame start's height h0 and the frame end h1 predicted from it */
static inline void notePic(f32 h0, f32 h1) {
    TrimPic& t = g_trim();
    const uint32_t now = t60_step_counter();
    const uint32_t n = (uint32_t)t60::steps();
    const uint32_t k = now - t.firstStep + 1;
    t.valid = k >= 1 && k < n;
    if (!t.valid)
        return;
    t.pic = h0 + ((f32)k / (f32)n) * (h1 - h0);
    t.picStep = now;
}
/* camera_draw: the picture's bar height for this pass (false: the logic height) */
static inline bool picTrim(f32* out) {
    if (!on() || t60::tick30())
        return false;
    const TrimPic& t = g_trim();
    if (!t.valid || t.picStep != t60_step_counter())
        return false;
    *out = t.pic;
    return true;
}
} // namespace camera_trim_t60
