#pragma once
#include "bindings.h"

struct GhostshipCircle_l {
    cXyz translation, position;
    be<f32> radius, wobbleAmplitude;
    be<s16> angle, angleSpeed;
};
WWHD_SIZE(GhostshipCircle_l, 0x24);
struct GhostshipWave_l {
    u8 history[8];
    be<s16> rotX, rotZ;
};
struct daGhostship_c : fopAc_ac_c {
    be<u32> mode;
    be<s32> pathNo;
    be<u8> moonPhase;
    u8 pad3B5[3];
    cXyz currentPoint;
    be<s8> pointNo;
    u8 pad3C5[3];
    gptr<u8> path;
    be<u32> pathState;
    cXyz nextPoint;
    be<u32> unusedPathState[3];
    request_of_phase_process_class phase, clothPhase;
    gptr<u8> cloth, cloth2;
    gptr<J3DModel> model;
    mDoExt_btkAnm btk;
    be<f32> alpha;
    dBgS_ObjAcch acch;
    dBgS_AcchCir cir;
    GhostshipWave_l wave;
    be<u8> enteredShip;
    u8 pad68D[3];
    GhostshipCircle_l circles[12];
    be<f32> radiusTargets[12];
    cXyz pathPos;
    be<u32> unused87C;
    be<f32> pathSpeed;
    be<u8> canEnterShip;
    u8 pad885[3];
};
WWHD_OFFSET(daGhostship_c, mode, 0x3AC);
WWHD_OFFSET(daGhostship_c, currentPoint, 0x3B8);
WWHD_OFFSET(daGhostship_c, nextPoint, 0x3D0);
WWHD_OFFSET(daGhostship_c, phase, 0x3E8);
WWHD_OFFSET(daGhostship_c, cloth, 0x3F8);
WWHD_OFFSET(daGhostship_c, model, 0x400);
WWHD_OFFSET(daGhostship_c, btk, 0x404);
WWHD_OFFSET(daGhostship_c, alpha, 0x478);
WWHD_OFFSET(daGhostship_c, acch, 0x47C);
WWHD_OFFSET(daGhostship_c, cir, 0x640);
WWHD_OFFSET(daGhostship_c, wave, 0x680);
WWHD_OFFSET(daGhostship_c, circles, 0x690);
WWHD_OFFSET(daGhostship_c, radiusTargets, 0x840);
WWHD_OFFSET(daGhostship_c, pathPos, 0x870);
WWHD_OFFSET(daGhostship_c, canEnterShip, 0x884);
WWHD_SIZE(daGhostship_c, 0x888);
