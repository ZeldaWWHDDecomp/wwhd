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
    /* 0x30 */ be<f32> mEyePosOffsetY;  /* was field_0x30: setBaseMtx 0221ED08 puts eyePos this far above current.pos (cLib_offsetPos), next to attention_info.position at mNpc.mAttnYOffset; daNpc_Cb1_HIO_c ctor 02226B9C 50.0 (tww execute the same) */
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
    /* 0x64 */ be<f32> mFlyLandSpeedY;  /* was field_0x64: flyAction 022222D0: speed.y on touching down from the flight (not on ground special code 1), before checkLanding (tww the same); ctor 8.0 */
    /* 0x68 */ be<f32> mHitSpeedScaleF;
    /* 0x6C */ be<f32> mHitSpeedScaleY;
    /* 0x70 */ be<f32> mGravity;  /* was field_0x70: init 0221F3C0 and execute 0222091C (cb1_execRoom, the walking position move) gravity (tww the same); ctor -4.0 */
    /* 0x74 */ be<f32> mCarryOffsetX;  /* was field_0x74: carryNpcAction 02223694 chases mCarryOffset.x to it (tww the same); ctor -1.5 */
    /* 0x78 */ be<f32> mCarryOffsetY;  /* was field_0x78: carryNpcAction 02223694 chases mCarryOffset.y to it (tww the same); ctor -6.0 */
    /* 0x7C */ be<f32> mCarryOffsetZ;  /* was field_0x7C: carryNpcAction 02223694 chases mCarryOffset.z to it (tww the same); ctor -7.6 */
    /* 0x80 */ be<f32> mStickTurnMin;  /* was field_0x80: waitPlayerAction 02224E38 turns Makar only with the stick at or above it (or a lock-on); flyAction 022222D0 ends a grounded flight at it (tww CPad_GET_STICK_VALUE(0) >= it); ctor 0.1 */
    /* 0x84 */ be<f32> mStickWalkMin;  /* was field_0x84: waitPlayerAction 02224E38 switches to walkPlayerAction with the stick at or above it (tww the same); ctor 0.2 */
    /* 0x88 */ be<f32> mStickWalkSpeedScale;
    /* 0x8C */ be<f32> mPropellerLiftScale;  /* was field_0x8C: flyAction 022222D0: the lift is mPropellerSpeed times it (tww temp3 = m8F8 * it); ctor 0.0002 */
    /* 0x90 */ be<f32> mFlyMaxFallSpeed;  /* was field_0x90: flyAction 022222D0 maxFallSpeed while the propeller turns (tww the same); ctor -15.0 */
    /* 0x94 */ u8 field_0x94[4];
    /* 0x98 */ be<f32> mFlyMaxSpeedY;  /* was field_0x98: flyAction 022222D0 caps speed.y at it in the air (tww ySpeedLimit = it, raised by the wind); ctor 10.0 */
    /* 0x9C */ be<f32> mFlyMaxSpeed;  /* was field_0x9C: flyAction 022222D0 (cb1_flyAir): the airborne speed limit the speed is chased down to (tww temp5 = it, raised by the wind); ctor 10.0 */
    /* 0xA0 */ be<f32> mFlyGravity;  /* was field_0xA0: flyAction 022222D0: added to the propeller lift as the vertical acceleration (gravity on the ground, speed.y in the air) (tww the same); ctor -2.5 */
    /* 0xA4 */ be<f32> mFlyDriftHeight;  /* was field_0xA4: flyAction 022222D0 sets m8F2 (the sideways drift) once mFlyHeight passes it (tww the same); ctor 100.0 */
    /* 0xA8 */ be<f32> mFlyTiltScale;  /* was field_0xA8: flyAction 022222D0 (cb1_flyAir): the body tilt is the horizontal push times it, at most 14000 (tww the same); ctor 6000.0 */
    /* 0xAC */ be<f32> mStickFlySpeedScale;
    /* 0xB0 */ be<f32> mFlyOpenSpeedY;  /* was field_0xB0: flyAction 022222D0: added to speed.y when the propeller opens in the air (tww the same); ctor 34.0 */
    /* 0xB4 */ be<f32> field_0xB4;
    /* 0xB8 */ be<f32> mFlyWindScale;  /* was field_0xB8: flyAction 022222D0 (cb1_flyAir) scales the wind power by it (tww power *= it); ctor 2.0 */
    /* 0xBC */ be<f32> mFlyLaunchSpeedY;
    /* 0xC0 */ be<f32> mNpcCallDist;  /* was field_0xC0: waitNpcAction 02223048: with event bit 0x1610 set and Link nearer than it, Link's NPC call turns on (tww daPy onNpcCall()); ctor 400.0 */
    /* 0xC4 */ be<f32> mDamageSpeedF;  /* was field_0xC4: execute 0222091C (cb1_execDamage): speedF away from the hit before the hit action (tww the same); ctor 20.0 */
    /* 0xC8 */ be<s16> field_0xC8;
    /* 0xCA */ be<s16> mPlayerFlyTimer;
    /* 0xCC */ be<s16> mLandIdleMax;  /* was field_0xCC: flyAction 022222D0 ends a grounded flight once mLandIdleCount passes it (tww ++m8F6 > it); ctor 100 */
    /* 0xCE */ be<s16> mFlyEndWarnTime;  /* was field_0xCE: flyAction 022222D0: Makar as the player plays the fly-end animation (ANM_15) while the flying timer is below it (tww getFlyingTimer() < it); ctor 15 */
    /* 0xD0 */ be<s16> mPropellerSpeedMax;  /* was field_0xD0: flyAction 022222D0 chases mPropellerSpeed up to it while lifting and scales the rolling sound by 100 / it (tww the same); ctor 20000 */
    /* 0xD2 */ be<s16> mPropellerIdleSpeed;  /* was field_0xD2: flyAction 022222D0 chases mPropellerSpeed to it on the ground (0 in the air) when not lifting, and a grounded flight may end at or below it (tww the same); ctor 400 */
    /* 0xD4 */ be<s16> mPropellerAccel;  /* was field_0xD4: flyAction 022222D0: the chase step toward mPropellerSpeedMax (tww cLib_chaseS(&m8F8, field_0xD0, it)); ctor 2500 */
    /* 0xD6 */ be<s16> mPropellerDecel;  /* was field_0xD6: flyAction 022222D0: the chase step toward mPropellerIdleSpeed / 0 (tww the same); ctor 200 */
    /* 0xD8 */ be<s16> mFlyAnmHeight;  /* was field_0xD8: flyAction 022222D0 plays the flying animation (ANM_07) once mFlyHeight passes it; flyNpcAction 02223A88 starts mFlyHeight at it + 1 (tww the same); ctor 1 */
    /* 0xDA */ be<s16> mPropellerOpenSpeed;  /* was field_0xDA: flyAction 022222D0: mPropellerSpeed when the propeller opens in the air (tww the same); ctor 0x2968 */
    /* 0xDC */ be<s16> mNpcFlyTimer;
    /* 0xDE */ be<s16> mFlyLiftPeriod;  /* was field_0xDE: flyNpcAction 02223A88: mFlyLiftCycle = (mFlyLiftCycle + 1) % it, lift only at 0 (tww the same); ctor 13 */
    /* 0xE0 */ be<s16> mFlyTiltStep;  /* was field_0xE0: flyAction 022222D0: cLib_chaseAngleS step of shape_angle.x / z toward the tilt (or 0 on the ground) (tww the same); ctor 200 */
    /* 0xE2 */ be<s16> mSearchTimeBase;  /* was field_0xE2: searchNpcAction 022244A0 reloads mSearchTimer with cLib_getRndValue(it, mSearchTimeRange) (tww the same); ctor 5 */
    /* 0xE4 */ be<s16> mSearchTimeRange;  /* was field_0xE4: searchNpcAction 022244A0: the cLib_getRndValue range for mSearchTimer (tww the same); ctor 20 */
    /* 0xE6 */ be<s16> mSearchTurnAngle;  /* was field_0xE6: searchNpcAction 022244A0 turns by -/+ it by mSearchSide (tww the same); ctor 3000 */
    /* 0xE8 */ be<s16> mWalkTurnMaxStep;  /* was field_0xE8: walkPlayerAction 022250AC: cLib_addCalcAngleS maxStep toward the stick angle (tww the same); ctor 0x8FC */
    /* 0xEA */ be<s16> mWalkTurnMinStep;  /* was field_0xEA: walkPlayerAction 022250AC: cLib_addCalcAngleS minStep (tww the same); ctor 800 */
    /* 0xEC */ be<s16> mWalkTurnScale;  /* was field_0xEC: walkPlayerAction 022250AC: cLib_addCalcAngleS scale (tww the same); ctor 3 */
    /* 0xEE */ be<u8> mDamageTimer;
    /* 0xEF */ be<u8> field_0xEF;
};
WWHD_OFFSET(daNpc_Cb1_HIO_c, __vtbl, 0x00);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mNo, 0x04);
WWHD_OFFSET(daNpc_Cb1_HIO_c, _05, 0x05);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mNpc, 0x08);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mEyePosOffsetY, 0x30);
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
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyLandSpeedY, 0x64);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mHitSpeedScaleF, 0x68);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mHitSpeedScaleY, 0x6C);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mGravity, 0x70);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mCarryOffsetX, 0x74);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mCarryOffsetY, 0x78);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mCarryOffsetZ, 0x7C);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mStickTurnMin, 0x80);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mStickWalkMin, 0x84);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mStickWalkSpeedScale, 0x88);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mPropellerLiftScale, 0x8C);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyMaxFallSpeed, 0x90);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0x94, 0x94);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyMaxSpeedY, 0x98);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyMaxSpeed, 0x9C);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyGravity, 0xA0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyDriftHeight, 0xA4);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyTiltScale, 0xA8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mStickFlySpeedScale, 0xAC);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyOpenSpeedY, 0xB0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xB4, 0xB4);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyWindScale, 0xB8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyLaunchSpeedY, 0xBC);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mNpcCallDist, 0xC0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mDamageSpeedF, 0xC4);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xC8, 0xC8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mPlayerFlyTimer, 0xCA);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mLandIdleMax, 0xCC);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyEndWarnTime, 0xCE);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mPropellerSpeedMax, 0xD0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mPropellerIdleSpeed, 0xD2);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mPropellerAccel, 0xD4);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mPropellerDecel, 0xD6);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyAnmHeight, 0xD8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mPropellerOpenSpeed, 0xDA);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mNpcFlyTimer, 0xDC);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyLiftPeriod, 0xDE);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mFlyTiltStep, 0xE0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mSearchTimeBase, 0xE2);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mSearchTimeRange, 0xE4);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mSearchTurnAngle, 0xE6);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mWalkTurnMaxStep, 0xE8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mWalkTurnMinStep, 0xEA);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mWalkTurnScale, 0xEC);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mDamageTimer, 0xEE);
WWHD_OFFSET(daNpc_Cb1_HIO_c, field_0xEF, 0xEF);
WWHD_SIZE(daNpc_Cb1_HIO_c, 0xF0);
struct AnmData {
    /* 0x00 */ be<s8> mAnmFileIdx;
    /* 0x01 */ be<u8> mLoopMode;
    /* 0x02 */ be<s8> mMorf;  /* was field_0x02: initAnm 0221FEFC passes it to dNpc_setAnm as the morf frames when morphing (tww data.field_0x02 the same) */
    /* 0x03 */ be<s8> mSpeed;
    /* 0x04 */ be<s8> mLoopCount;  /* was field_0x04: playAnm 02220024 switches the animation once mAnmLoopCount reaches it (tww the same) */
    /* 0x05 */ be<s8> mNextAnmTblIdx;  /* was field_0x05: playAnm 02220024 then starts table entry it (initAnm(it, 1)); 0x13 / 0x11 also call musicStop / musicPlay (tww the same) */
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
