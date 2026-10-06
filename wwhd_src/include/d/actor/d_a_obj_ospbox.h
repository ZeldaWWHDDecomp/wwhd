/* WWHD surprise box. */
#pragma once
#include "bindings.h"
namespace daObjOspbox {
struct Act_c : dBgS_MoveBgActor {
 request_of_phase_process_class mPhase; gptr<J3DModel> mpModel;
 dCcD_Stts mStts; dCcD_Cyl mCyl; u8 mGroundCheck[0x54];
 be<f32> mGroundY; be<s16> mGroundRetries; be<u8> mLockTimer; u8 _pad;
 s32 Mthd_Create(); BOOL Mthd_Delete(); BOOL CreateHeap(); BOOL Create(); BOOL Delete();
 void set_mtx(); void init_mtx(); void set_ground(); void init_ground();
 void make_item(); void eff_break(); void sound_break(); BOOL Execute(gptr<Mtx34>*); BOOL Draw();
};
WWHD_OFFSET(Act_c,mpModel,0x3E8); WWHD_OFFSET(Act_c,mCyl,0x428);
WWHD_OFFSET(Act_c,mGroundCheck,0x558); WWHD_OFFSET(Act_c,mGroundY,0x5AC);
WWHD_SIZE(Act_c,0x5B4);
}
