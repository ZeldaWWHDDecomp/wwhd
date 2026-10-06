#pragma once
#include "f_op/f_op_actor.h"
namespace daTagLight {
struct Act_c : fopAc_ac_c {
 request_of_phase_process_class phase;
 be<u32> model, btk;
 be<s32> type;
 be<f32> transform[12], inverse[12];
 be<u8> enabled, projectionEnabled;
 be<s16> lightCounter;
 be<f32> alpha;
 u8 collisionStatus[0x3C], sphere[0x12C];
 be<f32> projection[12];
 cXyz volumeScale;
};
WWHD_OFFSET(Act_c, model,0x3B4);
WWHD_OFFSET(Act_c, type,0x3BC);
WWHD_OFFSET(Act_c, alpha,0x424);
WWHD_OFFSET(Act_c, projection,0x590);
WWHD_OFFSET(Act_c, volumeScale,0x5C0);
}
