/** Door smoke, key construction and stop helpers. Local only. */
#pragma once
#include "bindings.h"
#include "gc_api/JSystem/JPAEmitter.h"
struct dDoor_info_c : fopAc_ac_c {
    be<s8> mRoomNo2; be<u8> mFromRoomNo, mToRoomNo, _pad;
    cXyz mAngleVec;
};
WWHD_OFFSET(dDoor_info_c, mAngleVec, 0x3B0);
// HD: callback storage places the rotation at 0x20, rather than GameCube's 0x22.
struct dDoor_smoke_c {
    dPa_smokeEcallBack mSmokeCb;
    csXyz mRot; u8 _26[2]; cXyz mPos;
    be<u8> m34, m35; u8 _36[2];
    void smokeInit(dDoor_info_c*); void smokeProc(dDoor_info_c*); void smokeEnd();
};
WWHD_OFFSET(dDoor_smoke_c, mRot, 0x20);
WWHD_OFFSET(dDoor_smoke_c, mPos, 0x28);
WWHD_OFFSET(dDoor_smoke_c, m34, 0x34);
WWHD_SIZE(dDoor_smoke_c, 0x38);
struct DoorSmokeEmitter : JPABaseEmitter {
    // HD: global scale vectors follow the expanded particle/render state.
    u8 _70[0x220 - 0x70]; cXyz mGlobalDynamicsScale;
    u8 _22C[0xC]; cXyz mGlobalParticleScale;
};
WWHD_OFFSET(DoorSmokeEmitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(DoorSmokeEmitter, mGlobalParticleScale, 0x238);
// HD: animation construction also initializes its embedded resource binding.
struct DoorKeyAnimationBinding {
    u8 _00[0x34]; be<u32> __vtbl;
    u8 _38[0xC]; be<u32> mpResource; u8 _48[0x20];
};
struct DoorKeyMatrixCalc {
    be<u32> __vtbl; DoorKeyAnimationBinding mBinding;
    // HD: three cleared binding slots have no GameCube counterpart.
    be<u32> mBindingSlots[3];
};
struct DoorKeyBckAnimation {
    J3DFrameCtrl mFrameCtrl; DoorKeyMatrixCalc mAnm; be<u32> mpAnm;
};
WWHD_OFFSET(DoorKeyMatrixCalc, mBinding, 4);
WWHD_OFFSET(DoorKeyMatrixCalc, mBindingSlots, 0x6C);
WWHD_SIZE(DoorKeyMatrixCalc, 0x78);
WWHD_OFFSET(DoorKeyBckAnimation, mAnm, 0x10);
WWHD_OFFSET(DoorKeyBckAnimation, mpAnm, 0x88);
WWHD_SIZE(DoorKeyBckAnimation, 0x8C);
struct dDoor_key2_c {
    be<u8> mbEnabled; u8 _01[3]; gptr<J3DModel> mpModel;
    DoorKeyBckAnimation mBckAnim;
    request_of_phase_process_class mPhs;
    be<u8> m20, mbIsBossDoor; u8 _9E[2];
};
WWHD_OFFSET(dDoor_key2_c, mPhs, 0x94);
WWHD_OFFSET(dDoor_key2_c, m20, 0x9C);
WWHD_SIZE(dDoor_key2_c, 0xA0);
struct dDoor_stop_c {
    gptr<J3DModel> mpModel; be<f32> mOffsY;
    be<u8> m8, mFrontCheck, mA, mB;
    void calcMtx(dDoor_info_c*); void closeInit(dDoor_info_c*); s32 closeProc(dDoor_info_c*);
    void openInit(dDoor_info_c*); s32 openProc(dDoor_info_c*); BOOL create();
};
WWHD_OFFSET(dDoor_stop_c, mOffsY, 4);
WWHD_OFFSET(dDoor_stop_c, mB, 0xB);
WWHD_SIZE(dDoor_stop_c, 0xC);
void* dDoor_alloc(u32);
void dDoor_smokeCallback_ct(dPa_smokeEcallBack*, u32);
void dDoor_frameCtrl_init(J3DFrameCtrl*, u32);
void dDoor_animationBinding_ct(DoorKeyAnimationBinding*);
void dDoor_phase_init(request_of_phase_process_class*, u32);
u32 dDoor_getArg1(dDoor_info_c*);
u32 dDoor_getReverb(s32);
void dDoor_startSound(u32, cXyz*, u32, u32);
J3DModelData* dDoor_getStageRes(const char*, const char*);
enum { JA_SE_OBJ_CAGE_CLOSE = 0x697C, JA_SE_OBJ_CAGE_OPEN = 0x697D, JA_SE_OBJ_STN_DOOR_STL_BAR = 0x6906 };

WWHD_SIZE(dDoor_info_c, 0x3BC);
WWHD_SIZE(DoorSmokeEmitter, 0x244);
WWHD_OFFSET(DoorKeyAnimationBinding, __vtbl, 0x34);
WWHD_OFFSET(DoorKeyAnimationBinding, mpResource, 0x44);
WWHD_SIZE(DoorKeyAnimationBinding, 0x68);
