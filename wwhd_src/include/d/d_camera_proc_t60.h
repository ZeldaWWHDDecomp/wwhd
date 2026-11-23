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
#include <cstdlib>
#include "true60.h"

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
