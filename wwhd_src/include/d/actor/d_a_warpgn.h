#pragma once
#include "f_op/f_op_actor.h"
namespace Warpgn {
struct Actor : fopAc_ac_c {
 request_of_phase_process_class phase;
 be<u32> model,btk1,btk2,brk,bck,emitter1,emitter2,emitter3,switchNo;
 be<u8> isSwitch;u8 pad[3];
 be<u32> scene;be<s32> eventState;be<s16> warpEvent,appearEvent;
 be<s32> staffId;be<u32> demoShape;be<s32> timer;
 dKy_tevstr_c extraTev;be<u8> appearing;u8 tail[3];
};
WWHD_OFFSET(Actor,phase,0x3AC);WWHD_OFFSET(Actor,model,0x3B4);
WWHD_OFFSET(Actor,emitter1,0x3C8);WWHD_OFFSET(Actor,switchNo,0x3D4);
WWHD_OFFSET(Actor,eventState,0x3E0);WWHD_OFFSET(Actor,staffId,0x3E8);
WWHD_OFFSET(Actor,timer,0x3F0);WWHD_OFFSET(Actor,extraTev,0x3F4);
WWHD_OFFSET(Actor,appearing,0x5BC);WWHD_SIZE(Actor,0x5C0);
}
