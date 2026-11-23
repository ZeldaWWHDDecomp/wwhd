/* JAIZelBasic_t60.h (WWHD, true 60 build, 2026-11-22): the camera timer on a general step
 * length (port runtime/src/ camera_timer_counts).
 *
 * JAIZelBasic::getCameraInfo 02027558 counts mNoCameraTimer down once per call (10 after a camera-less frame, e.g. a
 * stage load; seaEnvSePlay, riverSePlay and changeSeaBgm wait for 0). In the original it is called once per 30 Hz frame
 * by camera_draw. The port's listener takes the camera twice per logic step while interpolating (the exact pre-run and
 * the record pass) and on true 60's tick30 draw twice as well, so the timer ran out early: in the Forest Haven pair
 * (N 2, origin 2 frames after the load) the river ambience 0x3034 started 4 frames early (52 B-only starts).
 * Form: the count-down runs when the port says this hand-over is the frame's (once per 30 Hz frame with true 60, once per
 * logic step with interpolation, always at N = 1 without it); the pointers / the camera-less reset are unchanged. Game
 * build only; outside it (verify) it always counts.
 * setCameraGroupInfo 02027670 (camera_draw's audio pass: the camera's ground group, it can switch the audio scene through
 * setScene) gets the same double hand-over; it runs on the frame's first one only (wwhd_t60_audio_camgroup, its own
 * once-latch, preventive: the two hand-overs see the same camera on the routes traced).
 * getCameraMapInfo 02027668 (the material sound id of the ground under the eye) likewise (wwhd_t60_audio_cammap): on a
 * fallback frame with interpolation the record pass hands over a BLENDED camera after the exact pre-run; at a ground
 * boundary its ground flipped mCameraMapInfo back for a frame (mdT2 trace, Outset jetty frame 369: pre-run eye = A's
 * frame camera, sound 3; record pass halfway, 0xB; zb +0x288 one frame late in B). */
#pragma once
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) int wwhd_t60_audio_camtimer();
__attribute__((visibility("default"))) int wwhd_t60_audio_camgroup();
__attribute__((visibility("default"))) int wwhd_t60_audio_cammap();
}
inline bool t60_audio_camtimer_counts() { return wwhd_t60_audio_camtimer() != 0; }
inline bool t60_audio_camgroup_counts() { return wwhd_t60_audio_camgroup() != 0; }
inline bool t60_audio_cammap_counts() { return wwhd_t60_audio_cammap() != 0; }
#else
inline bool t60_audio_camtimer_counts() { return true; }
inline bool t60_audio_camgroup_counts() { return true; }
inline bool t60_audio_cammap_counts() { return true; }
#endif
