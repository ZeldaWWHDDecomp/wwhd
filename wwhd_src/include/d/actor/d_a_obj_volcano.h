/* Fire Mountain exterior, WWHD. */
#pragma once
#include "bindings.h"
namespace daObjVolcano {
struct Brk_l : mDoExt_baseAnm { u8 _10[0x78-0x10]; };
struct FloatColor { be<f32> r,g,b,a; };
struct Act_c : dBgS_MoveBgActor {
 gptr<u8> mEmitters[10]; request_of_phase_process_class mPhase;
 gptr<J3DModel> mpModel; mDoExt_btkAnm mBtk;
 gptr<J3DModel> mpFireModel; mDoExt_btkAnm mFireBtk; Brk_l mFireBrk;
 dCcD_Stts mStts; dCcD_Cyl mCyl;
 cXyz mParticleScale,mFirePos,mHeadPos,mMiddlePos,mFootPos;
 be<s32> mTimer; be<f32> mHeight,mRadius; be<u8> mFireVisible; u8 _72D[3];
 be<f32> mOpacity; be<s16> mFreezeEvent,mFireEvent; be<s32> mState;
 s32 Mthd_Create(); BOOL Mthd_Delete(); BOOL CreateHeap(); BOOL Create(); BOOL Delete();
 void set_mtx(); void init_mtx(); void StopFire(); void StartFire();
 void fire_main(); void freeze_demo_wait(); void freeze_demo_main(); void freeze_main();
 void fire_demo_wait(); void fire_demo_main(); void fail_demo_wait(); void fail_demo_main();
 BOOL Execute(gptr<Mtx34>*); BOOL Draw();
};
WWHD_OFFSET(Act_c,mBtk,0x414); WWHD_OFFSET(Act_c,mFireBrk,0x500);
WWHD_OFFSET(Act_c,mStts,0x578); WWHD_OFFSET(Act_c,mCyl,0x5B4);
WWHD_OFFSET(Act_c,mFirePos,0x6F0); WWHD_OFFSET(Act_c,mState,0x738); WWHD_SIZE(Act_c,0x73C);
}
