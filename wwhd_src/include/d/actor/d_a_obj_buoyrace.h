/* WWHD race buoy. */
#pragma once
#include "bindings.h"
namespace daObjBuoyrace {
struct Act_c : fopAc_ac_c {
 be<f32> mMeanSeaHeight; cXyz mNormal; be<s16> mPhaseAngle; u8 _3BE[10];
 be<f32> mSwayX,mSwayZ,mSwayVelocityX,mSwayVelocityZ,mBob,mBobVelocity;
 request_of_phase_process_class mPhsKiba,mPhsHasi; gptr<J3DModel> mpModelKiba,mpModelHasi;
 s32 create_load(); bool create_heap(); void set_water_pos(); void set_rope_pos();
 void set_mtx(); void init_mtx(); s32 _create(); bool _delete(); void afl_calc_sway();
 void afl_calc(); bool _execute(); bool _draw();
};
WWHD_OFFSET(Act_c,mMeanSeaHeight,0x3AC); WWHD_OFFSET(Act_c,mSwayX,0x3C8);
WWHD_OFFSET(Act_c,mPhsKiba,0x3E0); WWHD_OFFSET(Act_c,mpModelKiba,0x3F0); WWHD_SIZE(Act_c,0x3F8);
struct Quaternion_l { be<f32> x,y,z,w; };
}
