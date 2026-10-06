/* Nintendo Gallery hatch, WWHD layout. */
#pragma once
#include "bindings.h"
struct daObjOhatch_c : fopAc_ac_c {
 request_of_phase_process_class mPhase;
 gptr<J3DModel> mModel;
 be<s32> mSwitch;
 gptr<dBgW> mClosedBg,mOpenBg;
 Mtx34 mBgMatrix;
 be<s16> mHingeAngle; u8 _3F6[2];
 be<s16> mActionDelta,mActionIndex; be<u32> mActionTarget;
 be<f32> mTremorHeight; be<s32> mTremorPhase;
 be<s16> mVibrationAmplitude,mVibrationPhase;
 bool create_heap(); void init_mtx(); s32 _create(); bool _delete();
 void set_mtx(); bool _execute(); bool _draw();
 void close_wait_act_proc(); void tremor_act_proc(); void open_act_proc();
 void vibrate_act_proc(); void open_wait_act_proc();
};
WWHD_OFFSET(daObjOhatch_c,mModel,0x3B4);
WWHD_OFFSET(daObjOhatch_c,mBgMatrix,0x3C4);
WWHD_OFFSET(daObjOhatch_c,mActionDelta,0x3F8);
WWHD_OFFSET(daObjOhatch_c,mTremorHeight,0x400);
WWHD_SIZE(daObjOhatch_c,0x40C);
