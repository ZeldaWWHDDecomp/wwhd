/* Door lock components: GameCube names, HD layouts.
 * dDoor_info_c grows with fopAc_ac_c; mDoExt_bckAnm expands the key component to 0xA0. */
#pragma once
#include "gc_api/gc_api.h"

struct dDoor_info_c : fopAc_ac_c {
    /* 0x3AC */ be<s8> mRoomNo2;
    /* 0x3AD */ be<u8> mFromRoomNo;
    /* 0x3AE */ be<u8> mToRoomNo;
    u8 _3AF;
    /* 0x3B0 */ cXyz mAngleVec;
    /* 0x3BC */ be<u8> mFrontCheck;
    /* 0x3BD */ be<u8> m2A1;
    /* 0x3BE */ be<s16> mEventIdx[12];
    /* 0x3D6 */ be<u8> mToolId[12];
    /* 0x3E2 */ be<u8> m2C6;
    /* 0x3E3 */ be<u8> m2C7;
    /* 0x3E4 */ be<s32> mStaffId;
    /* 0x3E8 */ be<s8> mRoomNo;
    u8 _3E9[3];
    // HD: this call forwards the full register, rather than narrowing to GC's u8.
    s32 getSwbit();
};
WWHD_OFFSET(dDoor_info_c, mFrontCheck, 0x3BC);
WWHD_SIZE(dDoor_info_c, 0x3EC);

struct dDoor_key2_c {
    /* 0x00 */ be<u8> mbEnabled;
    u8 _01[3];
    /* 0x04 */ gptr<J3DModel> mpModel;
    /* 0x08 */ mDoExt_bckAnm mBckAnim;
    /* 0x94 */ request_of_phase_process_class mPhs;
    /* 0x9C */ be<u8> m20;
    /* 0x9D */ be<u8> mbIsBossDoor;
    u8 _9E[2];
    s32 keyResLoad();
    void keyResDelete();
    void keyInit(dDoor_info_c* door);
    void keyOff();
    BOOL keyProc();
    BOOL keyCreate_Bkey();
    BOOL keyCreate_Nkey();
    BOOL keyCreate(s32 type);
    void keyOn();
    void calcMtx(dDoor_info_c* door);
    void draw(dDoor_info_c* door);
};
WWHD_OFFSET(dDoor_key2_c, mBckAnim, 8);
WWHD_OFFSET(dDoor_key2_c, mPhs, 0x94);
WWHD_OFFSET(dDoor_key2_c, m20, 0x9C);
WWHD_OFFSET(dDoor_key2_c, mbIsBossDoor, 0x9D);
WWHD_SIZE(dDoor_key2_c, 0xA0);

struct dDoor_stop_c {
    /* 0x0 */ gptr<J3DModel> mpModel;
    /* 0x4 */ be<f32> mOffsY;
    /* 0x8 */ be<u8> m8;
    /* 0x9 */ be<u8> mFrontCheck;
    /* 0xA */ be<u8> mA;
    /* 0xB */ be<u8> mB;
};
WWHD_OFFSET(dDoor_stop_c, mOffsY, 4);
WWHD_OFFSET(dDoor_stop_c, mB, 0xB);
WWHD_SIZE(dDoor_stop_c, 0xC);

// HD: private view of the play field not yet exposed by the shared play layout.
struct DoorKeyGameInfo {
    u8 _0[0x5B5C];
    be<s16> mItemKeyNumCount;
};
WWHD_OFFSET(DoorKeyGameInfo, mItemKeyNumCount, 0x5B5C);
WWHD_SIZE(DoorKeyGameInfo, 0x5B5E);

// HD: declaration adapters preserve the raw return registers and both SafeString slots.
DoorKeyGameInfo* dComIfGp_getDoorKeyInfo();
void* dDoor_getObjectRes(dRes_control_c*, SafeString*, s32);
BOOL dDoor_initBck(mDoExt_bckAnm*, J3DModelData*, J3DAnmTransform*, s32, s32, s32, s32, s32, f32);

enum {
    dRes_INDEX_KEY_BDL_VLOCB_e = 8,
    dRes_INDEX_KEY_BDL_VLOCN_e = 9,
    dRes_INDEX_KEY_BCK_VLOCB_e = 4,
    dRes_INDEX_KEY_BCK_VLOCN_e = 5,
    JA_SE_OBJ_BOSS_LOCK_OPEN = 0x69AE,
    JA_SE_OBJ_DOOR_CHAIN_OPEN = 0x6948,
};
