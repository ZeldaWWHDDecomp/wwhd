#pragma once
#include "bindings.h"
struct daWarpls_c : fopAc_ac_c {
 request_of_phase_process_class mPhs;
 gptr<J3DModel> mpModel;
 be<u32> mpBrkAnm, mpBckAnm, mpEmitter;
 be<s32> mSwitchNo,mSceneNo,mEvtState;
 be<u8> mWarpActive,mSkipActivationEvt,mPrevSwitchState;
 be<s8> mStartupDelayTimer;
 be<s16> mActivationEvtIdx,mWarpingEvtIdx;
 be<u8> mWarpType,mEvtType,mPlayerStartedInWarp;
 u8 padding;
 s32 CreateHeap(); void CreateInit(); s32 create(); bool remove();
 s32 distance(); s32 link(); void effect(); bool demo();
 void checkOrder(); void eventOrder(); bool status(); bool execute();
};
WWHD_OFFSET(daWarpls_c,mPhs,0x3AC);
WWHD_OFFSET(daWarpls_c,mpModel,0x3B4);
WWHD_OFFSET(daWarpls_c,mpEmitter,0x3C0);
WWHD_OFFSET(daWarpls_c,mEvtState,0x3CC);
WWHD_OFFSET(daWarpls_c,mWarpActive,0x3D0);
WWHD_OFFSET(daWarpls_c,mActivationEvtIdx,0x3D4);
WWHD_OFFSET(daWarpls_c,mWarpType,0x3D8);
WWHD_SIZE(daWarpls_c,0x3DC);
