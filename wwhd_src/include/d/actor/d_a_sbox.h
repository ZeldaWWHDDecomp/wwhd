/* Salvaged treasure box, WWHD layout. */
#pragma once
#include "bindings.h"
struct mDoExt_brkAnm_sbox : mDoExt_baseAnm {
    u8 _10[0x78 - 0x10];
};
struct daSbox_c : fopAc_ac_c {
    request_of_phase_process_class mPhase; // 3AC
    gptr<J3DModel> mpModel1;
    mDoExt_bckAnm mBck1;
    gptr<J3DModel> mpModel2;
    mDoExt_bckAnm mBck2;
    mDoExt_btkAnm mBtk;
    mDoExt_brkAnm_sbox mBrk;
    be<u8> mAction;
    u8 _5c1[7];
    be<s16> mTimer;
    u8 _5ca[2];
    be<s32> mStaff;
    gptr<void> mEmitter;
    be<s16> mVolumeTimer;
    u8 _5d6[2];
    be<f32> mDarkRatio;
    be<s16> mDarkTimer;
    u8 _5de[2];
    LIGHT_INFLUENCE mLight, mEffectLight;
    be<s16> mLightTimer;
    be<u16> mFlags;
    bool chkFlag(u16 f) { return (mFlags & f) == f; }
    void setFlag(u16 f) { mFlags = mFlags | f; }
    void clrFlag(u16 f) { mFlags = mFlags & ~f; }
    BOOL CreateHeap();
    void calcMtx();
    void shipMtx();
    BOOL volmProc();
    BOOL darkProc();
    BOOL lightProc();
    void lightInit();
    BOOL CreateInit();
    cPhs_State create();
    void demoInitWait();
    BOOL demoProcWait();
    void demoInitOpen();
    void demoProcOpen();
    void demoInitDelete();
    void demoProcDelete();
    void demoInitCom();
    void demoProcCom();
    s32 getNowEventAction();
    BOOL demoProc();
    BOOL actionWait();
};
WWHD_OFFSET(daSbox_c, mBck1, 0x3B8);
WWHD_OFFSET(daSbox_c, mBck2, 0x448);
WWHD_OFFSET(daSbox_c, mBrk, 0x548);
WWHD_OFFSET(daSbox_c, mStaff, 0x5CC);
WWHD_OFFSET(daSbox_c, mFlags, 0x62A);
WWHD_SIZE(daSbox_c, 0x62C);
