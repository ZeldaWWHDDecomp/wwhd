#pragma once
#include "f_op/f_op_actor.h"
namespace daObjAshut {
struct Act_c : fopAc_ac_c {
    be<u32> background;
    u8 moveBgStorage[0x3E0-0x3B0];
    request_of_phase_process_class phase;
    be<u32> model;
    be<s32> mode;
    be<f32> height, velocity;
    be<u8> bounceCount;
    u8 padding;
    be<s16> delay;
    be<u8> demoStarted;
    u8 padding2;
    be<s16> eventIndex;
    be<s32> requestedMode;
};
WWHD_OFFSET(Act_c, phase, 0x3E0);
WWHD_OFFSET(Act_c, model, 0x3E8);
WWHD_OFFSET(Act_c, requestedMode, 0x400);
WWHD_SIZE(Act_c, 0x404);
}
