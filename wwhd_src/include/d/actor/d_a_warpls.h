/** Warp light shafts; GameCube actor tail 0x290 begins at WWHD 0x3AC. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/gc_api.h"
struct WarplsEmitterHD {
    u8 _0[0x5C];
    be<s32> mMaxFrame;
    u8 _60[0x220 - 0x60];
    cXyz mGlobalDynamicsScale;
    u8 _22C[0x254 - 0x22C];
    be<u32> mFlags;
};
WWHD_OFFSET(WarplsEmitterHD, mMaxFrame, 0x5C);
WWHD_OFFSET(WarplsEmitterHD, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(WarplsEmitterHD, mFlags, 0x254);
WWHD_SIZE(WarplsEmitterHD, 0x258);
// HD: the SafeString comparison invokes assureTermination twice on the literal and once on the stage.
struct WarplsSafeNameVtableHD {
    u8 _0[0x14];
    be<u32> mAssureTermination;
};
struct SafeName {
    gptr<const char> mText;
    gptr<WarplsSafeNameVtableHD> mVtable;
};
WWHD_SIZE(SafeName, 8);
inline void Warpls_assureName(SafeName *name) {
    gabi::call_ptr(name->mVtable->mAssureTermination, name);
}
// HD: inline BCK construction retains the native frame/transform constructors and every vtable store.
struct WarplsBckLayoutHD {
    J3DFrameCtrl mFrameCtrl;
    be<u32> mVtable;
    u8 mTransformMtx[0x34];
    be<u32> mMtxCalcVtable;
    u8 _4C[0x58 - 0x4C];
    be<u32> mJoint;
    u8 _5C[0x7C - 0x5C];
    be<u32> mParent, mAnm, mFlags, mExtra;
};
WWHD_SIZE(WarplsBckLayoutHD, 0x8C);
struct WarplsHeaderHD {
    be<f32> mPiMin, mPiMax;
    be<u8> mAngleByte, mAngleFlag;
    u8 _A[2];
    be<u32> mZero[4];
};
WWHD_SIZE(WarplsHeaderHD, 0x1C);
struct WarplsRegistrationHD {
    u8 mBytes[12];
};
struct WarplsRegistrationTableHD {
    WarplsRegistrationHD mRecords[3];
};
WWHD_SIZE(WarplsRegistrationTableHD, 36);
struct daWarpls_c : fopAc_ac_c {
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    gptr<mDoExt_brkAnm> mpBrkAnm;
    gptr<mDoExt_bckAnm> mpBckAnm;
    gptr<WarplsEmitterHD> mpEmitter;
    be<s32> mSwitchNo, mSceneNo, mEvtState;
    be<u8> mWarpActive, mSkipActivationEvt, mPrevSwitchState;
    be<s8> mStartupDelayTimer;
    be<s16> mActivationEvtIdx, mWarpingEvtIdx;
    be<u8> mWarpType, mEvtType, mPlayerStartedInWarp;
    u8 padding;
    s32 CreateHeap();
    void CreateInit();
    s32 _create();
    bool _delete();
    s32 check_warp_distance();
    s32 check_warp_link();
    void warp_eff_start();
    bool demo();
    void checkOrder();
    void eventOrder();
    bool setStatus();
    bool _execute();
};
WWHD_OFFSET(daWarpls_c, mPhs, 0x3AC);
WWHD_OFFSET(daWarpls_c, mpModel, 0x3B4);
WWHD_OFFSET(daWarpls_c, mpEmitter, 0x3C0);
WWHD_OFFSET(daWarpls_c, mEvtState, 0x3CC);
WWHD_OFFSET(daWarpls_c, mWarpActive, 0x3D0);
WWHD_OFFSET(daWarpls_c, mActivationEvtIdx, 0x3D4);
WWHD_OFFSET(daWarpls_c, mWarpType, 0x3D8);
WWHD_SIZE(daWarpls_c, 0x3DC);

inline const char *Warpls_guestString(const char *text) {
    return gabi::at<const char>(gabi::guest_addr(text));
}
