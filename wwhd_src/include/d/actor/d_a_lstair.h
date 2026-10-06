#pragma once
#include "bindings.h"
struct StairColorAnm_l : mDoExt_baseAnm { u8 _10[0x64]; };
struct StairTevAnm_l : mDoExt_baseAnm { u8 _10[0x68]; };
struct daLStair_c : fopAc_ac_c {
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel;
    gptr<dBgW> mpBgW;
    Mtx34 mBgMtx;
    mDoExt_bckAnm mBckAnm;
    mDoExt_btkAnm mBtkAnm;
    StairColorAnm_l mBpkAnm0, mBpkAnm1;
    StairTevAnm_l mBrkAnm;
    be<f32> mStairYOffset;
    be<s32> mSwitchNo;
    be<u16> mEventState;
    be<s16> mEventIdx;
    be<u8> mTimer;
    be<s8> mAppearTimer;
    be<u8> mSwitchStatus, mEnemyGone;
    BOOL CreateHeap(); void CreateInit(); s32 _create(); bool _delete(); BOOL _draw();
    void setMoveBGMtx(); void set_mtx(); void set_on_se(); void set_off_se();
    void appear_stair(); void disappear_stair(); void checkAppear(); void demoMove();
    void moveBG(); bool _execute();
};
WWHD_OFFSET(daLStair_c,mBckAnm,0x3EC);
WWHD_OFFSET(daLStair_c,mBtkAnm,0x478);
WWHD_OFFSET(daLStair_c,mBpkAnm0,0x4EC);
WWHD_OFFSET(daLStair_c,mBpkAnm1,0x560);
WWHD_OFFSET(daLStair_c,mBrkAnm,0x5D4);
WWHD_OFFSET(daLStair_c,mStairYOffset,0x64C);
WWHD_OFFSET(daLStair_c,mEnemyGone,0x65B);
