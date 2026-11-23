/* d_camera_force_t60.h (WWHD, true 60 build, 2026-11-23). Decomp build only.
 * Separate file: d_camera_mode.cpp keeps thin calls.
 *
 * The forced camera type (SetTypeForce 02514EB8: mForceType / mpForceTypeActor, a request a process makes on each of
 * its frames; nextType 024FA410 takes it and resets it to 0xFF) as a 30 Hz frame's request. In the original the
 * requester (Kalle Demos, bmd_exec "Boss02"; Gohma btd "Boss01"; Ganondorf fganon; bmd "Field" / "Dungeon") runs once a
 * frame before the camera, so every Run of the camera sees the request. A requester that runs once a 30 Hz frame on
 * the tick30 step (unconverted, or converted in a whole form such as bmd 0xEB, the whole-frame form rule) makes it on the tick30
 * step only; the camera's tick30 nextType takes it and the next frame's earlier steps find 0xFF: their nextType falls
 * through to the map-tool type. Kalle Demos pair: after event 2 every B half pass decided
 * map-tool type 60 / style 86 (onTypeChange / onModeChange reset mCurModeTimer m108 to 0), the tick30 whole run put the
 * frame start back and decided the forced 46 / style 120: the half passes' picture and the return chase lagged (camera
 * 54.13 @1164, cam-tslack eye 3.35 @1560).
 * Form: the request nextType takes on a tick30 step (or a dt 1 run: the follow-whole / event-whole frame's run) is noted
 * (host memory, per camera body); on the next frame's earlier steps a nextType that finds no request (0xFF) uses the
 * noted one (the 30 Hz world's request of the previous frame, the value the requester re-makes on its next tick30
 * step), and resets it to 0xFF after as the original. A dt 1 rerun on the same tick30 step (the mode-change rollback /
 * event switch-in: the frame start put back after the step's nextType took the request) gets the request it took. Only a request without an actor is carried (an actor pointer
 * is not carried into a frame where the actor may be gone; all forced types on the known routes pass 0 except mo2's
 * "Restrict" and the ship's "Tornado", converted per-step requesters that request on every step). A request a
 * per-step requester makes on the earlier step itself wins (mForceType != 0xFF). The tick30 step is unchanged.
 * WWHD_T60_CAMFORCE_NEGCTRL=1: the carry off (the form). dt = 1 (N = 1): never carried (the original). */
#pragma once
#include "d/d_camera_key_t60.h"
#include "true60.h"
#include <cstdlib>

namespace camera_force_t60 {
struct ForceFrame {
    uint32_t step; /* t60_step_counter of the tick30 / dt 1 nextType that noted it */
    s32 type;      /* the request it found (0xFF: none) */
    bool valid;
};
inline camera_key_t60::PerCam<ForceFrame> g_force;
static inline bool carryOn() {
    static int s = -1;
    if (s < 0) {
        const char* e = getenv("WWHD_T60_CAMFORCE_NEGCTRL");
        s = (e && e[0] == '1') ? 0 : 1;
    }
    return s == 1;
}
/* top of nextType: *type / *actorEa are mForceType / mpForceTypeActor (as read); returns the type to use in place of
 * 0xFF on an earlier step (0xFF: no change) */
static inline s32 forceTypeAtEntry(s32 type, uint32_t actorEa) {
    if (t60::steps() <= 1)
        return 0xFF;
    const uint32_t now = t60_step_counter();
    ForceFrame& f = g_force();
    if (t60::dt() == 1.0f || t60::tick30()) {
        /* a dt 1 rerun on the tick30 step after the step's own nextType took the request (the mode-change rollback
         * and the event switch-in put back the frame start, where mForceType is 0xFF): the request it took */
        if (t60::dt() == 1.0f && type == 0xFF && f.valid && f.step == now && f.type != 0xFF && carryOn())
            return f.type;
        f.step = now;
        f.type = actorEa == 0 ? type : 0xFF;
        f.valid = true;
        return 0xFF;
    }
    if (type != 0xFF || !f.valid || f.type == 0xFF || !carryOn())
        return 0xFF;
    /* the previous frame's tick30 step: at most N steps back (its whole run and this frame's earlier steps) */
    if (now - f.step > (uint32_t)t60::steps())
        return 0xFF;
    return f.type;
}
} // namespace camera_force_t60
