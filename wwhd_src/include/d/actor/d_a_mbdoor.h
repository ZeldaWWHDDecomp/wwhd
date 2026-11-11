#pragma once
#include "JSystem/J3DGraph.h"
#include "bindings.h"
#include "d/actor/d_a_player.h"
#include "gc_api/f_op/f_op_actor.h"
// HD: actor base grows by 0x11C; fields retain GameCube member names.
class daMbdoor_c : public fopAc_ac_c {
  public:
    u8 getShapeType();
    const char* getArcName();
    u32 getFuBdl();
    u32 getDzb();
    u32 getLBdl();
    u32 getRBdl();
    u32 getToBdl();
    f32 getROffset();
    f32 getLOffset();
    f32 getToOffset();
    void calcMtx();
    BOOL CreateHeap();
    u8 getSwbit();
    u8 getType();
    BOOL checkUnlock();
    s32 getDemoAction();
    void demoProc();
    BOOL checkArea();
    BOOL draw();
    BOOL execute();
    BOOL CreateInit();
    s32 create();
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpFuModel;
    /* 0x3B8 */ gptr<J3DModel> mpLModel;
    /* 0x3BC */ gptr<J3DModel> mpRModel;
    /* 0x3C0 */ gptr<J3DModel> mpToModel;
    /* 0x3C4 */ gptr<dBgW> mpBgW;
    /* 0x3C8 */ be<u8> mBgRegistered;  /* was field_0x2ac: CreateInit 021C0AFC sets 1 once mpBgW is registered; daMbdoor_actionCloseWait 021C07FC sets 0 when it releases mpBgW for the door event (tww the same); not read in WWHD; tww's demo-version daMbdoor_Delete releases mpBgW only while it is set (the retail one, like WWHD 021C0AA0, tests the heap); Background registered. */
    /* 0x3C9 */ be<u8> mOpenSide;  /* was field_0x2ad: checkArea 021C06D8: 1 when the player stands on the +lateral side, else 0; calcMtx 021BF854 swings the door to that side (mDoorAngle or its negative) and places mPlayerGoalPos there; demoProc 021C00C4 uses it for the goal; execute 021C0984 sets 0 for a demo actor; Door side. */
    /* 0x3CA */ be<u8> mCurActionIdx;
    u8 pad3CB;
    /* 0x3CC */ be<s16> mBarAngle;  /* was field_0x2b0: calcMtx 021BF854: Z rotation of the bar (lock) model; CreateInit 021C0AFC: 0 when locked, -0x3F65 when open; demoProc 021C00C4 ACT_STOP_OPEN raises it to -0x3F65 by mAngleSpeed; execute 021C0984 takes a demo actor's mRotate.z; Bar angle. */
    /* 0x3CE */ be<s16> mDoorAngle;  /* was field_0x2b2: calcMtx 021BF854: Y rotation of the door wings; demoProc 021C00C4 ACT_OPEN opens it to -0x1C71 by mAngleSpeed; CreateInit 021C0AFC sets 0; execute 021C0984 takes a demo actor's mRotate.y; Door angle. */
    /* 0x3D0 */ be<s16> mAngleSpeed;  /* was field_0x2b4: demoProc 021C00C4: cleared by ACT_OPEN / ACT_STOP_OPEN, then grows each frame as the step of mDoorAngle (ACT_OPEN) or mBarAngle (ACT_STOP_OPEN); Angular speed. */
    /* 0x3D2 */ be<u8> mBarOpening;  /* was field_0x2b6: demoProc 021C00C4: ACT_STOP_OPEN sets 1 and moves mBarAngle only while it is set; cleared when the bar reaches -0x3F65; Bar still opening. */
    u8 pad3D3;
    /* 0x3D4 */ be<s16> mAdjustmentTimer;
    u8 pad3D6[2];
    /* 0x3D8 */ be<s32> mUnlockDelay;  /* was field_0x2bc: CreateInit 021C0AFC sets 30; checkUnlock 021BFF14 (type 1): counts it down after the room's enemies are gone and turns the switch on only at 0; Enemy absence delay. */
    /* 0x3DC */ be<s32> mEvtStaffId;
    /* 0x3E0 */ cXyz mPlayerGoalPos;  /* was field_0x2c4: calcMtx 021BF854: the point beside the opening door wing (on mOpenSide); demoProc 021C00C4 makes it the player's goal (setPlayerGoal / ACT_SET_GOAL) and checks the player's distance to it; Player goal beside the door. */
    /* 0x3EC */ cXyz mGoalPos;
    /* 0x3F8 */ cXyz mGoal2Pos;
};
struct MbdoorSaveData {
    u8 prefix[0x20];
    u8 mSwitch;
};
struct MbdoorDemoActor {
    u8 prefix[4];
    be<u16> mEnabled;
    u8 gap[0x20 - 6];
    csXyz mRotate;
};
struct MbdoorDispatch {
    gfn mActions[6];
};
struct MbdoorDemoActions {
    gptr<char> mNames[9];
};
struct MbdoorSinCos {
    be<f32> mSin, mCos;
};
struct MbdoorSinCosTable {
    MbdoorSinCos mEntries[8192];
};
void* mbdoorGetResource(void*, SafeString*, s32);
J3DModel* mbdoorCreateModel(void*, u32, u32);
dBgW* mbdoorCreateBgW(u32);
BOOL mbdoorSetBackground(dBgW*, void*, u32, Mtx34*);
BOOL mbdoorIsSwitch(void*, u32, s8);
BOOL mbdoorSearchEnemy(s8);
void mbdoorOnSwitch(void*, u32, s8);
void mbdoorOrderEvent(daMbdoor_c*, const char*, u32, u32);
s32 mbdoorGetMyActIdx(void*, s32, void*, u32, u32, u32);
void mbdoorCutEnd(void*, s32);
void mbdoorSetGoal(void*, cXyz*);
BOOL mbdoorGetIsAdvance(void*, s32);
be<s32>* mbdoorGetSubstance(void*, s32, const char*, u32);
void mbdoorAddCalcAngle(be<s16>*, s16, s32, s16);
s32 mbdoorGetStaffId(void*, const char*, u32, u32);
BOOL mbdoorEndCheck(void*, const char*);
void mbdoorSubtract(const cXyz*, cXyz*, const cXyz*);
s32 mbdoorSeenAngle(daPy_py_c*, daMbdoor_c*);
void mbdoorRelease(void*, dBgW*);
void mbdoorSetEventName(dEvt_info_c*, const char*);
void mbdoorSetLight(void*, s32, cXyz*, dKy_tevstr_c*);
void mbdoorApplyLight(void*, J3DModel*, dKy_tevstr_c*);
void mbdoorUpdateModel(J3DModel*, u32);
void mbdoorCopyDispatch(void*, void*, u32);
MbdoorDemoActor* mbdoorGetDemoActor(void*, u32);
BOOL mbdoorRegister(void*, dBgW*, daMbdoor_c*);
void mbdoorMoveBackground(dBgW*);
void mbdoorConstructActor(daMbdoor_c*);
BOOL mbdoorEntryHeap(daMbdoor_c*, void*, u32);
void mbdoorDeleteObject(void*);
void mbdoorDestroyActor(daMbdoor_c*, s32);
static_assert(sizeof(daMbdoor_c) == 0x404);
static_assert(offsetof(daMbdoor_c, mPhs) == 0x3AC);
static_assert(offsetof(daMbdoor_c, mpFuModel) == 0x3B4);
static_assert(offsetof(daMbdoor_c, mpLModel) == 0x3B8);
static_assert(offsetof(daMbdoor_c, mpRModel) == 0x3BC);
static_assert(offsetof(daMbdoor_c, mpToModel) == 0x3C0);
static_assert(offsetof(daMbdoor_c, mpBgW) == 0x3C4);
static_assert(offsetof(daMbdoor_c, mBgRegistered) == 0x3C8);
static_assert(offsetof(daMbdoor_c, mOpenSide) == 0x3C9);
static_assert(offsetof(daMbdoor_c, mCurActionIdx) == 0x3CA);
static_assert(offsetof(daMbdoor_c, mBarAngle) == 0x3CC);
static_assert(offsetof(daMbdoor_c, mDoorAngle) == 0x3CE);
static_assert(offsetof(daMbdoor_c, mAngleSpeed) == 0x3D0);
static_assert(offsetof(daMbdoor_c, mBarOpening) == 0x3D2);
static_assert(offsetof(daMbdoor_c, mAdjustmentTimer) == 0x3D4);
static_assert(offsetof(daMbdoor_c, mUnlockDelay) == 0x3D8);
static_assert(offsetof(daMbdoor_c, mEvtStaffId) == 0x3DC);
static_assert(offsetof(daMbdoor_c, mPlayerGoalPos) == 0x3E0);
static_assert(offsetof(daMbdoor_c, mGoalPos) == 0x3EC);
static_assert(offsetof(daMbdoor_c, mGoal2Pos) == 0x3F8);
static_assert(sizeof(MbdoorSaveData) == 33);
static_assert(offsetof(MbdoorSaveData, mSwitch) == 32);
static_assert(sizeof(MbdoorDemoActor) == 38);
static_assert(offsetof(MbdoorDemoActor, mEnabled) == 4);
static_assert(offsetof(MbdoorDemoActor, mRotate) == 32);
static_assert(sizeof(MbdoorDispatch) == 24);
static_assert(offsetof(MbdoorDispatch, mActions) == 0);
static_assert(sizeof(MbdoorDemoActions) == 36);
static_assert(offsetof(MbdoorDemoActions, mNames) == 0);
static_assert(sizeof(MbdoorSinCos) == 8);
static_assert(offsetof(MbdoorSinCos, mSin) == 0);
static_assert(offsetof(MbdoorSinCos, mCos) == 4);
static_assert(sizeof(MbdoorSinCosTable) == 65536);
static_assert(offsetof(MbdoorSinCosTable, mEntries) == 0);

// HD: the two door archives use the same resource indices.
enum dRes_INDEX_MBDOOR {
    dRes_INDEX_MBDOOR_BDL_S_MBD_L_e = 4,
    dRes_INDEX_MBDOOR_BDL_S_MBD_R_e = 5,
    dRes_INDEX_MBDOOR_BDL_S_MBDFU_e = 6,
    dRes_INDEX_MBDOOR_BDL_S_MBDTO_e = 7,
    dRes_INDEX_MBDOOR_DZB_S_MBDFU_e = 10
};
