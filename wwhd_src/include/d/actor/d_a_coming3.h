#pragma once
#include "bindings.h"
namespace daComing3 {
struct Act_c : fopAc_ac_c {
    u8 collision[0x518-0x3ac];
    be<u32> barrelID;
    be<s32> gameState;
    request_of_phase_process_class phase;
    Mtx34 barrelMatrix;
    gptr<J3DModel> model;
    be<s16> process;
    u8 reserved[0x590-0x55e];
    be<f32> distance;
    be<s16> delay;
    be<s16> challenge;
};
WWHD_OFFSET(Act_c, barrelID, 0x518);
WWHD_OFFSET(Act_c, model, 0x558);
WWHD_OFFSET(Act_c, distance, 0x590);
WWHD_SIZE(Act_c, 0x598);
}
