/* d_lib_t60.h (WWHD, true 60 build; follow-up): dLib_waveRot (d_lib.cpp
 * 025872F4) on a general step length (dt = 1/N).
 *
 * dLib_waveRot advances the wave's two sway phases by a fixed amount per call (mAnimX + 400, mAnimZ + 430) and smooths
 * the tilt angles with cLib_addCalcAngleS2 (c_lib's integer dt form: each 30 Hz frame ends on the original step).
 * Called from a converted execute (the Ghost Ship's ghostMatrices, every step) the phase adds ran once a step, so the
 * ship rocked N times as fast. Converted (dt < 1): on the frame's k-th step of N the phase is the frame-start phase +
 * amount x k / N (integer, toward zero), and the tick30 step writes the frame start + the whole amount, so each 30 Hz
 * frame ends on the original phase. dt = 1 (every unconverted caller, N = 1): the original adds, no table touched.
 * Host memory only, keyed by the wave's guest address; d_lib.cpp is the only user (one unit). */
#pragma once
#include "true60.h"

static inline u32 dlib60_frame_tag() { return *gabi::at<be<u32>>(0x101FF558); } /* g_Counter.mCounter0 */

struct DLib60Wave {
    u32 wave, frame, stamp;
    s32 startX, startZ;
    bool valid;
};
static DLib60Wave s_dlib60Wave[8];
static u32 s_dlib60WaveNext;

/* the phases this call leaves (curX / curZ: the stored phases before the call) */
static inline void dlib60_wavePhase(u32 wave, s32 curX, s32 curZ, s32* outX, s32* outZ) {
    if (t60::dt() == 1.0f) {
        *outX = curX + 400;
        *outZ = curZ + 430;
        return;
    }
    const u32 frame = dlib60_frame_tag(), now = t60_step_counter(), n = (u32)t60::steps();
    DLib60Wave* e = nullptr;
    for (DLib60Wave& w : s_dlib60Wave) {
        if (w.valid && w.wave == wave) {
            e = &w;
            break;
        }
    }
    if (!e) {
        e = &s_dlib60Wave[s_dlib60WaveNext++ % 8];
        e->valid = false;
        e->wave = wave;
    }
    if (!e->valid || e->frame != frame || now - e->stamp >= n) {
        e->frame = frame;
        e->stamp = now;
        e->startX = curX;
        e->startZ = curZ;
        e->valid = true;
    }
    s32 k = (s32)(now - e->stamp) + 1;
    if (t60::tick30() || k > (s32)n)
        k = (s32)n;
    *outX = e->startX + 400 * k / (s32)n;
    *outZ = e->startZ + 430 * k / (s32)n;
}
