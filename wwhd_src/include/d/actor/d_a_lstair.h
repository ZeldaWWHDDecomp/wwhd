#pragma once
#include "gc_api/gc_api.h"
struct LStairBckState {
    J3DFrameCtrl mFrameCtrl;
    be<u32> mVtable;
    u8 mObject[0x34];
    be<u32> mObjectVtable;
    u8 mPadding4C[0x0C];
    be<u32> field_0x58;
    u8 mPadding5C[0x20];
    be<u32> field_0x7C, field_0x80, field_0x84, field_0x88;
};
WWHD_SIZE(LStairBckState, 0x8C);
struct LStairColorAnm : mDoExt_baseAnm { u8 _10[0x64]; };
struct LStairTevAnm : mDoExt_baseAnm { u8 _10[0x68]; };
struct daLStair_c : fopAc_ac_c {
    daLStair_c();
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel;
    gptr<dBgW> mpBgW;
    Mtx34 mBgMtx;
    union { mDoExt_bckAnm mBckAnm; LStairBckState mBckState; };
    mDoExt_btkAnm mBtkAnm;
    LStairColorAnm mBpkAnm0, mBpkAnm1;
    LStairTevAnm mBrkAnm;
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

struct LStairStageInfo { u8 mPadding[9]; be<u8> mType; };
void* LStair_getResource(void*, SafeString*, s32);
void* fopAcM_myRoomSearchEnemy(s32);
void LStair_scaleM(f32, f32, f32);
u8 LStair_calcTimer(be<u8>*);
f32 LStair_addCalc(be<f32>*, f32, f32, f32, f32);
BOOL LStair_startCheck(dEvent_manager_c*, s16);
void LStair_frameInit(J3DFrameCtrl*, s32);
void LStair_objectConstruct(void*);
void LStair_objectDestruct(void*, s32);
void LStair_colorConstruct(void*);
void LStair_tevConstruct(void*);
BOOL LStair_colorInit(void*, J3DModelData*, void*, s32, s32, f32, s16, s16, s32, s32);
BOOL LStair_tevInit(void*, J3DModelData*, void*, s32, s32, f32, s16, s16, s32, s32);
void LStair_colorEntry(void*, J3DModelData*, f32);
void LStair_tevEntry(void*, J3DModelData*, f32);
JPABaseEmitter* LStair_particleSet(dPa_control_c*, s32, u16, cXyz*, csXyz*, s32, u8, s32, s8, s32, s32, s32);
BOOL LStair_bgSet(dBgW*, void*, u32, Mtx34*);

BOOL LStair_endCheck(dEvent_manager_c*, s16);
s32 LStair_getEventIdx(dEvent_manager_c*, const char*, u8);
void LStair_btkEntry(void*, J3DModelData*, f32);
