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
 * original. */
#pragma once
#include "true60.h"

/* true: this camera draw hands the audio system the frame's camera */
static inline bool camera_t60_audio_pass() { return t60::tick30(); }
