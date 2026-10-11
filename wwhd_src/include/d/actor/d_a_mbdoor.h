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
    /* 0x3C8 */ be<u8> field_0x2ac; // Background registered.
    /* 0x3C9 */ be<u8> field_0x2ad; // Door side.
    /* 0x3CA */ be<u8> mCurActionIdx;
    u8 pad3CB;
    /* 0x3CC */ be<s16> field_0x2b0; // Bar angle.
    /* 0x3CE */ be<s16> field_0x2b2; // Door angle.
    /* 0x3D0 */ be<s16> field_0x2b4; // Angular speed.
    /* 0x3D2 */ be<u8> field_0x2b6; // Bar still opening.
    u8 pad3D3;
    /* 0x3D4 */ be<s16> mAdjustmentTimer;
    u8 pad3D6[2];
    /* 0x3D8 */ be<s32> field_0x2bc; // Enemy absence delay.
    /* 0x3DC */ be<s32> mEvtStaffId;
    /* 0x3E0 */ cXyz field_0x2c4; // Player goal beside the door.
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
static_assert(offsetof(daMbdoor_c, field_0x2ac) == 0x3C8);
static_assert(offsetof(daMbdoor_c, field_0x2ad) == 0x3C9);
static_assert(offsetof(daMbdoor_c, mCurActionIdx) == 0x3CA);
static_assert(offsetof(daMbdoor_c, field_0x2b0) == 0x3CC);
static_assert(offsetof(daMbdoor_c, field_0x2b2) == 0x3CE);
static_assert(offsetof(daMbdoor_c, field_0x2b4) == 0x3D0);
static_assert(offsetof(daMbdoor_c, field_0x2b6) == 0x3D2);
static_assert(offsetof(daMbdoor_c, mAdjustmentTimer) == 0x3D4);
static_assert(offsetof(daMbdoor_c, field_0x2bc) == 0x3D8);
static_assert(offsetof(daMbdoor_c, mEvtStaffId) == 0x3DC);
static_assert(offsetof(daMbdoor_c, field_0x2c4) == 0x3E0);
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
