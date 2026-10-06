#pragma once
#include "f_op/f_op_actor.h"
struct daObjTry_c : fopAc_ac_c {
    request_of_phase_process_class phase;
    be<u32> model;
    u8 animationStorage[0x430-0x3B8];
    u8 collisionStorage[0x7A0-0x430];
    be<u32> type,mode;
    u8 movementStorage[0x7B8-0x7A8];
    cXyz correctPosition;
    be<s16> correctYaw,restartDelay;
    be<u8> correctEnabled,correctImmediate;
    u8 padding[2];
    be<u8> appears,bingoActive;
    u8 padding2[2];
    u8 particleCallback[0x14];
    be<u32> emitter;
};
WWHD_OFFSET(daObjTry_c,model,0x3B4);
WWHD_OFFSET(daObjTry_c,type,0x7A0);
WWHD_OFFSET(daObjTry_c,correctPosition,0x7B8);
WWHD_OFFSET(daObjTry_c,appears,0x7CC);
WWHD_OFFSET(daObjTry_c,emitter,0x7E4);
WWHD_SIZE(daObjTry_c,0x7E8);
