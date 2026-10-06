#pragma once
#include "bindings.h"
namespace Kddoor {
struct DoorInfo : fopAc_ac_c {
    be<s8> secondaryRoom; be<u8> fromRoom,toRoom; u8 pad3AF;
    cXyz approachDirection;
    be<u8> front,enemyTimer; be<s16> eventIds[12]; be<u8> toolIds[12];
    be<u8> action,eventStep; be<s32> staff; be<s8> doorRoom; u8 pad3E9[3];
};
struct Arm {
    gptr<mDoExt_McaMorf> morph,cap;
    u8 smoke[0x20]; dCcD_Stts status; dCcD_Cyl cylinder;
    be<u8> state; u8 pad195; be<s16> phase,phaseSpeed,angle;
    cXyz scale,position,previousPosition;
    be<u8> started,soundTimer; u8 pad1C2[2];
};
struct Stop {
    Arm arms[3]; be<u8> enabled,side,otherEnabled,moving;
    dKy_tevstr_c tev;
};
struct Key { be<u8> enabled; u8 storage[0x9F]; };
struct Actor : DoorInfo {
    request_of_phase_process_class phase;
    u8 smoke[0x38]; Key key; Stop stop;
    gptr<J3DModel> model; gptr<dBgW> background;
    be<u8> openState; u8 padBED; be<u16> flags; be<f32> travel;
};
WWHD_SIZE(DoorInfo,0x3EC);
WWHD_OFFSET(DoorInfo,front,0x3BC); WWHD_OFFSET(DoorInfo,action,0x3E2); WWHD_OFFSET(DoorInfo,staff,0x3E4);
WWHD_SIZE(Arm,0x1C4); WWHD_OFFSET(Arm,status,0x28); WWHD_OFFSET(Arm,cylinder,0x64);
WWHD_OFFSET(Arm,phase,0x196); WWHD_OFFSET(Arm,scale,0x19C); WWHD_OFFSET(Arm,position,0x1A8); WWHD_OFFSET(Arm,started,0x1C0);
WWHD_SIZE(Stop,0x718); WWHD_OFFSET(Stop,enabled,0x54C); WWHD_OFFSET(Stop,tev,0x550);
WWHD_SIZE(Key,0xA0); WWHD_SIZE(Actor,0xBF4);
WWHD_OFFSET(Actor,phase,0x3EC); WWHD_OFFSET(Actor,smoke,0x3F4); WWHD_OFFSET(Actor,key,0x42C);
WWHD_OFFSET(Actor,stop,0x4CC); WWHD_OFFSET(Actor,model,0xBE4); WWHD_OFFSET(Actor,background,0xBE8); WWHD_OFFSET(Actor,flags,0xBEE); WWHD_OFFSET(Actor,travel,0xBF0);
}
