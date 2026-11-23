/* d_a_player_camyaw_t60.h (WWHD, true 60 build, 2026-11-18, rule on pair):
 *Link's camera-relative stick target reads the camera's frame-start yaw (the process-order read rule). Decomp build only,
 *.
 *
 * setStickData sets mMoveTargetAngleY = dCam_getControledAngleY(camera) + the stick angle; Link's facing chases it. The
 * camera (0x1DC, converted with Link) runs AFTER Link in the process line, so at 30 Hz Link's frame f reads the
 * camera's frame f - 1 end. At N steps his step k > 1 read the camera after ITS step k - 1 of the same frame: while the
 * camera yaw drifted (143-150: the Darknut's Co pushes Link sideways, the centre and the yaw follow on the
 * tick30 step) his target moved on one step of two and his per-frame turn followed a mid-frame yaw A never reads
 * (A 20 / 11 / 20 / 10 / 31, B 10 / 10 / 21 / 10 / 20; 30 units short at 150). the process-order read rule: a converted reader reads a
 * converted actor later in the line at its frame-start state.
 * Form: the frame's first step's read (the camera has not run in this frame yet: the frame-start yaw) is kept for the
 * frame's later steps. dt 1 (N = 1, Link or the camera held at 30 Hz): the live read, the original. Host memory, one
 * player. */
#pragma once
#include "true60.h"

struct LkT60CamYaw {
    uint32_t stamp; /* step counter of the frame's first step that read it */
    s16 yaw;
    bool valid;
};
static LkT60CamYaw s_lkT60CamYaw;

/* live: dCam_getControledAngleY's value on this step */
static inline s16 lkT60_camYawFrameStart(s16 live) {
    if (t60::dt() == 1.0f || t60::steps() <= 1)
        return live;
    const uint32_t now = t60_step_counter();
    if (t60::first_step()) {
        s_lkT60CamYaw.stamp = now;
        s_lkT60CamYaw.yaw = live;
        s_lkT60CamYaw.valid = true;
        return live;
    }
    if (s_lkT60CamYaw.valid && now - s_lkT60CamYaw.stamp < (uint32_t)t60::steps())
        return s_lkT60CamYaw.yaw;
    return live; /* no first-step read this frame (Link started reading on a later step) */
}
