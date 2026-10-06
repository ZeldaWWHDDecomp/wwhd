#pragma once
#include "f_op/f_op_actor.h"
struct daObjHcbh_c : fopAc_ac_c {
    request_of_phase_process_class phase;
    be<u32> model;
    be<u32> fragmentModels[4];
    be<u32> background[2];
    u8 collisionStorage[0xE48-0x3D0];
    be<f32> pillarHeight,fallVelocity;
    be<s16> pillarAngle,pillarAngularVelocity;
    cXyz fragmentPositions[4];
    be<f32> fragmentVelocities[4];
    be<s16> fragmentAngles[4],fragmentYaws[4],fragmentAngularVelocities[4];
    be<s32> fragmentDelays[4];
    be<s16> breakYaw;
    u8 padding[2];
    be<s32> breakCondition;
    u8 smokeCallback[0x20];
    be<u8> appears;
    u8 padding2[3];
    be<s16> actionAdjustment,actionIndex;
    be<u32> actionTarget;
};
WWHD_OFFSET(daObjHcbh_c,model,0x3B4);
WWHD_OFFSET(daObjHcbh_c,collisionStorage,0x3D0);
WWHD_OFFSET(daObjHcbh_c,pillarHeight,0xE48);
WWHD_OFFSET(daObjHcbh_c,fragmentDelays,0xEAC);
WWHD_OFFSET(daObjHcbh_c,breakCondition,0xEC0);
WWHD_OFFSET(daObjHcbh_c,actionTarget,0xEEC);
WWHD_SIZE(daObjHcbh_c,0xEF0);
