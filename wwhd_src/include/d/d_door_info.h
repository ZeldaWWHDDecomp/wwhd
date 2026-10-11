/* dDoor_info_c: tww door state after the larger HD actor base (GC 0x290 -> HD 0x3AC). */
#pragma once
#include "f_op/f_op_actor.h"
#include "d/d_save.h"

class dDoor_info_c : public fopAc_ac_c {
  public:
    // HD: preserve the full register arguments/results of the out-of-line accessors.
    void initOpenDemo(s32);
    u32 getFRoomNo();
    u32 getBRoomNo();
    u32 getShipId();
    void openInitCom(s32);
    void openProcCom();
    void closeEndCom();
    s32 getDemoAction();
    void setGoal();
    void setPlayerAngle(s32);
    void setPosAndAngle(cXyz*, s32);
    u32 getEventNo();
    void makeEventId(u32);
    void initProc(u32);
    u32 getSwbit();
    u32 getSwbit2();
    u32 getType();
    void setType(u32);
    u32 getArg1();

    /* 0x3AC */ be<s8> mRoomNo2;
    /* 0x3AD */ be<u8> mFromRoomNo;
    /* 0x3AE */ be<u8> mToRoomNo;
    /* 0x3AF */ u8 pad3AF;
    /* 0x3B0 */ cXyz mAngleVec;
    /* 0x3BC */ be<u8> mFrontCheck;
    /* 0x3BD */ be<u8> m2A1;
    /* 0x3BE */ be<s16> mEventIdx[12];
    /* 0x3D6 */ be<u8> mToolId[12];
    /* 0x3E2 */ be<u8> m2C6;
    /* 0x3E3 */ be<u8> m2C7;
    /* 0x3E4 */ be<s32> mStaffId;
    /* 0x3E8 */ be<s8> mRoomNo;
    /* 0x3E9 */ u8 pad3E9[3];
};
WWHD_OFFSET(dDoor_info_c, mRoomNo2, 0x3AC);
WWHD_OFFSET(dDoor_info_c, mAngleVec, 0x3B0);
WWHD_OFFSET(dDoor_info_c, mFrontCheck, 0x3BC);
WWHD_OFFSET(dDoor_info_c, mEventIdx, 0x3BE);
WWHD_OFFSET(dDoor_info_c, mToolId, 0x3D6);
WWHD_OFFSET(dDoor_info_c, mStaffId, 0x3E4);
WWHD_OFFSET(dDoor_info_c, mRoomNo, 0x3E8);
WWHD_SIZE(dDoor_info_c, 0x3EC);

// HD: private views of only the extra fields this unit accesses. Shared player padding stays
// intact.
struct dDoor_roomStatusHD {
    u8 pad0[0x21C];
    be<u8> mFlags;
    u8 pad21D[0x22C - 0x21D];
};
WWHD_OFFSET(dDoor_roomStatusHD, mFlags, 0x21C);
WWHD_SIZE(dDoor_roomStatusHD, 0x22C);
struct dDoor_environmentHD {
    u8 pad0[0x10A3];
    be<u8> mDoorDestinationRoom;
};
WWHD_OFFSET(dDoor_environmentHD, mDoorDestinationRoom, 0x10A3);
struct dDoor_playerDemoHD : fopAc_ac_c {
    u8 pad3AC[0x420 - 0x3AC];
    be<u16> mDemoType;
    be<s16> mDemoMoveAngle;
};
WWHD_OFFSET(dDoor_playerDemoHD, mDemoMoveAngle, 0x422);
WWHD_SIZE(dDoor_playerDemoHD, 0x424);

struct dEvent_manager_c;
void dDoor_bzeroHD(void*, u32);
dDoor_environmentHD* dDoor_getEnvlightHD();
void dDoor_setShipRoomHD(u32, u32);
s32 dDoor_getMyActIdxHD(dEvent_manager_c*, u32, const gptr<char>*, s32, s32, s32);

// HD: full-width callee adapters keep tool/event IDs intact until their field stores.
u32 dDoor_getMyStaffIdHD(dEvent_manager_c*, const char*, u32, u32);
u32 dDoor_getToolIdHD(dEvent_manager_c*, u32, u32);
u32 dDoor_getEventIdxHD(dEvent_manager_c*, const char*, u32);
void dDoor_setGoalHD(dEvent_manager_c*, cXyz*);
void dDoor_setRestartRoomHD(dSv_restart_c*, cXyz*, s32, s32);
struct dDoor_saveInfoHD {
    u8 pad0[0x20];
    dSv_info_c mSave;
};
WWHD_OFFSET(dDoor_saveInfoHD, mSave, 0x20);
struct dDoor_playerVtableHD {
    u8 pad0[0x114];
    be<u32> mSetPlayerPosAndAngle;
};
WWHD_OFFSET(dDoor_playerVtableHD, mSetPlayerPosAndAngle, 0x114);
WWHD_OFFSET(dDoor_info_c, mFromRoomNo, 0x3AD);
WWHD_OFFSET(dDoor_info_c, mToRoomNo, 0x3AE);
WWHD_OFFSET(dDoor_info_c, m2A1, 0x3BD);
WWHD_OFFSET(dDoor_info_c, m2C6, 0x3E2);
WWHD_OFFSET(dDoor_info_c, m2C7, 0x3E3);
WWHD_SIZE(dDoor_environmentHD, 0x10A4);
WWHD_SIZE(dDoor_saveInfoHD, 0x20 + sizeof(dSv_info_c));
WWHD_SIZE(dDoor_playerVtableHD, 0x118);
