#pragma once
#include "d/actor/d_a_npc_cb1.h"
#include "gc_api/gc_api.h"

namespace cb1_execute {
// HD: private views of the original guest objects; shared actor layout is unchanged.
struct daNpc_Cb1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ dNpc_HIO_l mNpc;
    /* 0x30 */ be<f32> field_0x30;
    /* 0x34 */ be<f32> mPlayerChaseDistance;
    /* 0x38 */ be<f32> mChaseDistScale;
    /* 0x3C */ be<f32> mMaxWalkSpeed;
    /* 0x40 */ be<f32> mMinWalkSpeed;
    /* 0x44 */ be<f32> mForwardAccel;
    /* 0x48 */ be<f32> mDecelScale;
    /* 0x4C */ be<f32> mMaxDecel;
    /* 0x50 */ be<f32> mDecel;
    /* 0x54 */ be<f32> mWalkAnmSpeedScale;
    /* 0x58 */ be<f32> mMaxWalkAnmSpeed;
    /* 0x5C */ be<f32> mNpcFlyLaunchSpeedF;
    /* 0x60 */ be<f32> mNpcFlyLaunchSpeedY;
    /* 0x64 */ be<f32> field_0x64;
    /* 0x68 */ be<f32> mHitSpeedScaleF;
    /* 0x6C */ be<f32> mHitSpeedScaleY;
    /* 0x70 */ be<f32> field_0x70;
    /* 0x74 */ be<f32> field_0x74;
    /* 0x78 */ be<f32> field_0x78;
    /* 0x7C */ be<f32> field_0x7C;
    /* 0x80 */ be<f32> field_0x80;
    /* 0x84 */ be<f32> field_0x84;
    /* 0x88 */ be<f32> mStickWalkSpeedScale;
    /* 0x8C */ be<f32> field_0x8C;
    /* 0x90 */ be<f32> field_0x90;
    /* 0x94 */ u8 field_0x94[4];
    /* 0x98 */ be<f32> field_0x98;
    /* 0x9C */ be<f32> field_0x9C;
    /* 0xA0 */ be<f32> field_0xA0;
    /* 0xA4 */ be<f32> field_0xA4;
    /* 0xA8 */ be<f32> field_0xA8;
    /* 0xAC */ be<f32> mStickFlySpeedScale;
    /* 0xB0 */ be<f32> field_0xB0;
    /* 0xB4 */ be<f32> field_0xB4;
    /* 0xB8 */ be<f32> field_0xB8;
    /* 0xBC */ be<f32> mFlyLaunchSpeedY;
    /* 0xC0 */ be<f32> field_0xC0;
    /* 0xC4 */ be<f32> field_0xC4;
    /* 0xC8 */ be<s16> field_0xC8;
    /* 0xCA */ be<s16> mPlayerFlyTimer;
    /* 0xCC */ be<s16> field_0xCC;
    /* 0xCE */ be<s16> field_0xCE;
    /* 0xD0 */ be<s16> field_0xD0;
    /* 0xD2 */ be<s16> field_0xD2;
    /* 0xD4 */ be<s16> field_0xD4;
    /* 0xD6 */ be<s16> field_0xD6;
    /* 0xD8 */ be<s16> field_0xD8;
    /* 0xDA */ be<s16> field_0xDA;
    /* 0xDC */ be<s16> mNpcFlyTimer;
    /* 0xDE */ be<s16> field_0xDE;
    /* 0xE0 */ be<s16> field_0xE0;
    /* 0xE2 */ be<s16> field_0xE2;
    /* 0xE4 */ be<s16> field_0xE4;
    /* 0xE6 */ be<s16> field_0xE6;
    /* 0xE8 */ be<s16> field_0xE8;
    /* 0xEA */ be<s16> field_0xEA;
    /* 0xEC */ be<s16> field_0xEC;
    /* 0xEE */ be<u8> mDamageTimer;
    /* 0xEF */ be<u8> field_0xEF;
};
WWHD_OFFSET(daNpc_Cb1_HIO_c, __vtbl, 0x00);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mNo, 0x04);
WWHD_OFFSET(daNpc_Cb1_HIO_c, _05, 0x05);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mNpc, 0x08);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x30, 0x30);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mPlayerChaseDistance, 0x34);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mChaseDistScale, 0x38);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mMaxWalkSpeed, 0x3C);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mMinWalkSpeed, 0x40);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mForwardAccel, 0x44);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mDecelScale, 0x48);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mMaxDecel, 0x4C);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mDecel, 0x50);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mWalkAnmSpeedScale, 0x54);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mMaxWalkAnmSpeed, 0x58);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mNpcFlyLaunchSpeedF, 0x5C);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mNpcFlyLaunchSpeedY, 0x60);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x64, 0x64);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mHitSpeedScaleF, 0x68);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mHitSpeedScaleY, 0x6C);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x70, 0x70);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x74, 0x74);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x78, 0x78);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x7C, 0x7C);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x80, 0x80);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x84, 0x84);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mStickWalkSpeedScale, 0x88);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x8C, 0x8C);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x90, 0x90);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x94, 0x94);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x98, 0x98);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x9C, 0x9C);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xA0, 0xA0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xA4, 0xA4);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xA8, 0xA8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mStickFlySpeedScale, 0xAC);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xB0, 0xB0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xB4, 0xB4);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xB8, 0xB8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyLaunchSpeedY, 0xBC);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xC0, 0xC0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xC4, 0xC4);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xC8, 0xC8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mPlayerFlyTimer, 0xCA);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xCC, 0xCC);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xCE, 0xCE);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xD0, 0xD0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xD2, 0xD2);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xD4, 0xD4);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xD6, 0xD6);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xD8, 0xD8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xDA, 0xDA);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mNpcFlyTimer, 0xDC);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xDE, 0xDE);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xE0, 0xE0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xE2, 0xE2);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xE4, 0xE4);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xE6, 0xE6);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xE8, 0xE8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xEA, 0xEA);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xEC, 0xEC);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mDamageTimer, 0xEE);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xEF, 0xEF);
WWHD_SIZE(daNpc_Cb1_HIO_c, 0xF0);
struct AnmData {
    /* 0x00 */ be<s8> mAnmFileIdx;
    /* 0x01 */ be<u8> mLoopMode;
    /* 0x02 */ be<s8> field_0x02;
    /* 0x03 */ be<s8> mSpeed;
    /* 0x04 */ be<s8> field_0x04;
    /* 0x05 */ be<s8> field_0x05;
};
WWHD_SIZE(AnmData, 6);
enum : u32 {
    CB1_waitNpcAction = 0x02223048,
    CB1_talkNpcAction = 0x02223500,
    CB1_carryNpcAction = 0x02223694,
    CB1_flyNpcAction = 0x02223A88,
    CB1_searchNpcAction = 0x022244A0,
    CB1_hitNpcAction = 0x022249C0,
    CB1_jumpNpcAction = 0x02224AC0,
    CB1_rescueNpcAction = 0x02224BC0,
    CB1_musicNpcAction = 0x02224C20,
    CB1_shipNpcAction = 0x02224CFC,
    CB1_waitPlayerAction = 0x02224E38,
    CB1_walkPlayerAction = 0x022250AC,
    CB1_hitPlayerAction = 0x02225388,
    CB1_jumpPlayerAction = 0x02225464,
    CB1_flyPlayerAction = 0x0222555C,
    CB1_carryPlayerAction = 0x0222566C,
};
using ActionFunc = BOOL (daNpc_Cb1_c::*)(void *);
using ActionPtmf = gabi::ptmf<ActionFunc>;
struct AnmTable {
    AnmData mData[28];
};
struct AnmTypeTable {
    be<u8> mData[11];
};
struct AnmParameterTable {
    be<s8> mData[24];
};
WWHD_SIZE(AnmTable, 168);
WWHD_SIZE(AnmTypeTable, 11);
WWHD_SIZE(AnmParameterTable, 24);
} // namespace cb1_execute
void CbExec_monsSeStart(u32, cXyz *, u32, u32, s32);
s32 CbExec_getSpecialCode(dBgS *, void *);
void CbExec_setSimpleLand(u32, void *, cXyz *, csXyz *, f32, f32, f32, void *, s32 *, s32);
void CbExec_setAnm(mDoExt_McaMorf *, s32, f32, f32, s32, s32, u32);
void CbExec_practiceStop();
void CbExec_practicePlay(cXyz *);
void CbExec_bgmStart(u32);
void CbExec_changePlayer(fopAc_ac_c *, fopAc_ac_c *);
u32 CbExec_getLockonList(u32, s32);
u32 CbExec_getActionList(u32, s32);
BOOL CbExec_lockonTruth(u32);
u32 CbExec_getActionBtnB(u32);
fopAc_ac_c *CbExec_getAttentionActor(u32);
void CbExec_lookAtTarget(dNpc_JntCtrl_c *, be<s16> *, cXyz *, cXyz *, s16, s16, BOOL);
u32 CbExec_getMyActName(dEvent_manager_c *, s32);
s32 CbExec_calcTimerS16(be<s16> *);
s32 CbExec_stickAngle(s32);
s32 CbExec_controlledAngleY(u32);
BOOL CbExec_trigA(s32);
f32 CbExec_addCalc(be<f32> *, f32, f32, f32, f32);
void CbExec_orderTalkEvent(fopAc_ac_c *, fopAc_ac_c *);
u32 CbExec_messageStatus(u32);
void CbExec_setMessageStatus(u32, u32);
u32 CbExec_tactMessageSet(u32);
u32 CbExec_messageSet(u32, u32, cXyz *);
void CbExec_setRestart(daNpc_Cb1_c *, s32);
void CbExec_allWindVector(cXyz *, cXyz *, be<f32> *);
BOOL CbExec_chaseF(be<f32> *, f32, f32);
void CbExec_setEmitterMatrix(u32, u32, u32);
void CbExec_chaseS(be<s16> *, s16, s16);
f64 CbExec_stickValue(s32);
void CbExec_forceLockOff(u32, u32);
u32 CbExec_drawPriority(fopAc_ac_c *);
void CbExec_enqueueDraw(u32, u32);
u32 CbExec_convActor(u32, u32);
void CbExec_controlSound(u32);
s32 CbExec_getMyActIdx(dEvent_manager_c *, s32, u32, s32, s32, s32);
BOOL CbExec_checkInitialRoom(daNpc_Cb1_c *);
BOOL CbExec_checkMoveBgNoDaBg(dBgS *, void *);
void CbExec_initialRestartOption(daNpc_Cb1_c *, s32, BOOL);
BOOL CbExec_checkMoveStop(daNpc_Cb1_c *);
BOOL CbExec_checkPolySafe(dBgS *, cBgS_PolyInfo_l *);
BOOL CbExec_checkMoveBg(dBgS *, cBgS_PolyInfo_l *);
void CbExec_moveBgCrrPos(dBgS *, cBgS_PolyInfo_l *, s32, cXyz *, u32, u32);
void CbExec_startRestartRoom(u32, s32, s32, f32, s32);
s32 CbExec_getRoomId(dBgS *, void *);
u32 CbExec_getPolyColor(dBgS *, void *);
BOOL CbExec_checkNowPosMove(daNpc_Cb1_c *, u32);
s32 CbExec_getGroundCode(dBgS *, void *);
void CbExec_calcDamageTimer(be<u8> *);
void CbExec_addCalcPosXZ(cXyz *, cXyz *, f32, f32, f32);
BOOL CbExec_trigROrStart(s32);
s32 CbExec_checkMoveBlock(daNpc_Cb1_c *, cXyz *);

namespace cb1_execute {
struct Attention {
    u8 mPrefix[0x20];
    be<u32> mFlags;
};
struct AttentionList {
    u8 mPrefix[8];
    be<u32> mType;
};
struct MessageManager {
    u8 mPrefix[0x921];
    be<u8> mMessageSendOn;
};
struct Ship : fopAc_ac_c {
    u8 mPrefix[0x644 - 0x3AC];
    be<u32> mFlags;
    u8 mPadding[0x1350 - 0x648];
    cXyz mAtnPos;
};
struct PlayerVtable {
    u8 mPrefix[0xA4];
    be<u32> mBaseAnimeFrame;
};
struct HitObject {
    u8 mPrefix[0x10];
    be<u32> mAtType;
};
WWHD_SIZE(Attention, 0x24);
WWHD_SIZE(AttentionList, 0xC);
WWHD_SIZE(MessageManager, 0x922);
WWHD_OFFSET(Ship, mFlags, 0x644);
WWHD_OFFSET(Ship, mAtnPos, 0x1350);
WWHD_SIZE(Ship, 0x135C);
WWHD_SIZE(PlayerVtable, 0xA8);
WWHD_SIZE(HitObject, 0x14);
} // namespace cb1_execute

namespace cb1_execute {
// HD: word views preserve the verified interleaved copy, including room/padding bytes.
struct WordPosition {
    be<u32> x, y, z, angleXY, angleZRoom;
};
WWHD_SIZE(WordPosition, 0x14);
struct RoomStatus {
    be<u8> mStatus;
    u8 mPadding[0x22B];
};
struct RoomStatusTable {
    RoomStatus mRoom[64];
};
WWHD_SIZE(RoomStatus, 0x22C);
struct EventProc {
    ProcFunc_l mInit;
    ProcFunc_l mRun;
};
struct EventProcTable {
    EventProc mData[15];
};
struct CutNameTable {
    gptr<const char> mData[15];
};
WWHD_SIZE(EventProc, 16);
WWHD_SIZE(EventProcTable, 240);
WWHD_SIZE(CutNameTable, 60);
struct Emitter {
    u8 mPrefix[0x1F0];
    u8 mGlobalRotation[0x24];
    u8 mPadding[0x18];
    cXyz mGlobalTranslation;
};
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22C);
WWHD_SIZE(Emitter, 0x238);
} // namespace cb1_execute

namespace cb1_execute {
enum : u32 { HD_fallResetNpcAction = 0x0222487C };
}

namespace cb1_execute {
enum : u32 {
    JA_SE_CV_CB_LANDING = 0x48C1,
    JA_BGM_MAKORE_TAKT_8 = 0x8000005F,
    JA_SE_CV_CB_LEFT_ALONE = 0x491A,
    JA_SE_CV_CB_TROUBLE = 0x491C,
    JA_SE_CV_CB_DAMAGE = 0x48BF,
    JA_SE_CV_CB_LIFT_UP = 0x48BC,
    JA_SE_CV_CB_THROW = 0x48BD,
    JA_SE_CV_CB_FLY_END = 0x4922,
    JA_SE_CM_PRAPELLO_OPEN = 0x58C0,
    ID_AK_SN_PERAPROOPEN00 = 0x830B,
    JA_SE_CM_PRAPELLO_ROLLING = 0x50BF,
};
}
