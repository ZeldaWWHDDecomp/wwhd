#pragma once
#include "bindings.h"
namespace daObjSwhammer {
struct SmokeCb_l { u8 data[0x20]; };
struct Act_c : dBgS_MoveBgActor {
 request_of_phase_process_class mPhs; gptr<J3DModel> mpModel;
 dCcD_Cyl mCylCo; dCcD_Stts mSttsCo; dCcD_Cyl mCylTg; dCcD_Stts mSttsTg;
 be<s32> mMode; be<s16> mCrushTimer; be<u8> mCrushState; u8 pad;
 be<f32> mScaleYSpeed,mScaleY,mAngleZ,mAngleX,mAngleSpeedZ,mAngleSpeedX,mTargetHFrac,mCurHFrac,mVSpeed,mTopPos;
 SmokeCb_l mSmokeCb; be<u8> mChanging; u8 pad2[3];
 static Act_c* ct(Act_c*);
 s32 _create(); bool _delete(); s32 CreateHeap(); s32 Create(); s32 Delete(); s32 Draw(); s32 Execute(Mtx34**);
 void set_mtx(); void init_mtx(); void set_damage(); void vib_start(s16,f32); void vib_proc(); void crush_start(); void crush_proc(); void eff_crush(); void calc_top_pos();
 void mode_upper_init(); void mode_lower_init(); void mode_u_l_init(); void mode_l_u_init(); void mode_upper(); void mode_lower(); void mode_u_l(); void mode_l_u();
};
WWHD_OFFSET(Act_c,mpModel,0x3E8); WWHD_OFFSET(Act_c,mMode,0x6C4); WWHD_OFFSET(Act_c,mSmokeCb,0x6F4); WWHD_SIZE(Act_c,0x718);
}
