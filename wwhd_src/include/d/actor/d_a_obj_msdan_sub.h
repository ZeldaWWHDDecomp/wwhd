// Msdan staircase step, WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

namespace daObjMsdanSub {
class Act_c : public dBgS_MoveBgActor {
public:
    Act_c();
    BOOL Delete();
    enum Prm_e {
        PRM_SIZE_W = 0x1,
        PRM_SIZE_H = 0x10,
        PRM_SWSAVE_W = 0x8,
        PRM_SWSAVE_S = 0x0,
        PRM_OBJNO_W = 0x8,
        PRM_OBJNO_S = 0x8,
    };
    u32 prm_get_size();
    u32 prm_get_swSave();
    u32 prm_get_objNo();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(gptr<Mtx34>* pMtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mModel;
    /* 0x3EC */ be<s32> mMinObjNo;  /* was m2D4: Create 0237BA50: the start value of mCurObjNo (0, or 16 for the other size); Execute 0237BCB4 lowers mCurObjNo only while it is above it (tww Execute: mCurObjNo > m2D4) */
    /* 0x3F0 */ be<s32> mCurObjNo;
    /* 0x3F4 */ be<f32> mMoveY;  /* was m2DC: Create 0237BA50 home.pos.y; Execute 0237BCB4 adds mSpeedY to it each frame and copies it into current.pos.y for the step whose number is mCurObjNo */
    /* 0x3F8 */ be<f32> mSpeedY;  /* was m2E0: Create 0237BA50 0; Execute 0237BCB4 steps it by -10 (lowering) / +15 (raising) and adds it to mMoveY; reset to 0 when a step lands */
    /* 0x3FC */ be<u8> mShockDone;  /* was m2E4: Create 0237BA50 false; Execute 0237BCB4 sets it after the one-time vibration StartShock(1, -0x21) (tww), clears it when the step goes back up (JA_SE_OBJ_SW_STAIR_OFF_1) */
    u8 mPadding[3];
};
WWHD_SIZE(Act_c, 0x400);
WWHD_OFFSET(Act_c, mModel, 0x3E8);
WWHD_OFFSET(Act_c, mShockDone, 0x3FC);
}  // namespace daObjMsdanSub
