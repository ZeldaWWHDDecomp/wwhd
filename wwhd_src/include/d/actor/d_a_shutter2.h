/* Earth Temple shutter door, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
enum { JA_SE_OBJ_WDUN_R04_STR_OP = 0x69CA };
fopAc_ac_c* fopAcM_myRoomSearchEnemy(s8);
f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32);
struct daShutter2_c : dBgS_MoveBgActor {
    daShutter2_c() {
        dBgS_MoveBgActor::ct(this);
        __vtbl = 0x1003CC0C; // HD: actor vtable.
    }
    BOOL Delete();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State _create();
    void set_mtx();
    BOOL Execute(Mtx34** pMtx);
    void shutter_move();
    void demo();
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs; /* GameCube 0x2C8 (+0x118) */
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ Mtx34 mMtx;
    /* 0x41C */ be<s32> mDemoState;
    /* 0x420 */ be<s32> mSwitchNo;
    /* 0x424 */ be<s32> m30C;
    /* 0x428 */ be<s32> mStaffId;
    /* 0x42C */ be<s16> mOpenEventIdx;
    /* 0x42E */ be<s16> mCloseEventIdx;
    /* 0x430 */ be<u8> m318;
    /* 0x431 */ be<u8> mType;
    /* 0x432 */ be<u8> mbIsSwitch;
    /* 0x433 */ be<u8> mbIsNearEnemy;
};
WWHD_OFFSET(daShutter2_c, mpModel, 0x3E8);
WWHD_OFFSET(daShutter2_c, mMtx, 0x3EC);
WWHD_OFFSET(daShutter2_c, mDemoState, 0x41C);
WWHD_OFFSET(daShutter2_c, mbIsNearEnemy, 0x433);
WWHD_SIZE(daShutter2_c, 0x434);

namespace daShutter2_prm {
inline u8 getSwitchNo(daShutter2_c* actor) { return fopAcM_GetParam(actor) & 0xFF; }
}
