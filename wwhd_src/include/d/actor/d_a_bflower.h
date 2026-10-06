#pragma once
#include "f_op/f_op_actor.h"
struct daBFlower_c : fopAc_ac_c {
 request_of_phase_process_class mPhs;
 gptr<u8> mpModel;
 u8 mCollision[0x650-0x3B8];
 u8 mBck1[0x6DC-0x650];
 u8 mBrk1[0x754-0x6DC];
 be<s16> mAnimation;
 u8 pad756[2];
 gptr<u8> mpModel2;
 u8 mBck2[0x7E8-0x75C];
 u8 mBrk2[0x860-0x7E8];
 be<u8> mAvailable, mCarryReady, mUnused, mState;
 be<s32> mSwitchNo, mRecoveryTimer, mGrabbable;
 gptr<u8> mpBombActor;
 be<s32> mAnimTimer;
 be<f32> mPrevPlayerDist;
 be<u32> mPrevGrabActorID;
 cXyz mBombScale;
 be<u8> mGrowing;
 BOOL CreateHeap(); void CreateInit(); s32 init_bck_anm(s16); s32 _create();
 void set_mtx(); bool _execute(); BOOL actLive(); BOOL actDead();
 void animPlay(); void setCollision(); bool _draw();
};
WWHD_OFFSET(daBFlower_c,mpModel,0x3B4);
WWHD_OFFSET(daBFlower_c,mAnimation,0x754);
WWHD_OFFSET(daBFlower_c,mState,0x863);
WWHD_OFFSET(daBFlower_c,mBombScale,0x880);
