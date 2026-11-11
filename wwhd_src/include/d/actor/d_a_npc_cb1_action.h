// Makar action and event helpers; shared actor declaration remains unchanged.
// HD: private HIO view preserves the GameCube field offsets inside the HD allocation.
#pragma once
#include "d/actor/d_a_npc_cb1.h"
#include "gc_api/gc_api.h"
namespace cb1_action {
using ActionFunc = BOOL (daNpc_Cb1_c::*)(void *);
using ActionPtmf = gabi::ptmf<ActionFunc>;
struct daNpc_Cb1_HIO_c {
    be<u32> vtable;
    /* 0x04 */ be<s8> mNo;
    u8 mPadding[3];
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
    /* 0x94 */ be<u8> field_0x94[4];
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
WWHD_SIZE(daNpc_Cb1_HIO_c, 0xF0);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mNpc, 8);
WWHD_OFFSET(daNpc_Cb1_HIO_c, mDamageTimer, 0xEE);
} // namespace cb1_action

// HD: bindings preserve the original register ABI, including unrounded f1 values.
cXyz *CbAction_getGoal(u32 manager);
void CbAction_offsetPos(cXyz *out, cXyz *pos, s16 ang, const cXyz *ofs);
BOOL CbAction_chaseS(be<s16> *v, s16 target, s16 step);
s32 CbAction_calcTimerS16(be<s16> *t);
f64 CbAction_stickValue(s32 pad);
s32 CbAction_drawPriority(fopAc_ac_c *actor);
void CbAction_enqueueDraw(u32 tag, s32 priority);
fopAc_ac_c *CbAction_convPId(u32 control, u32 id);
f64 CbAction_squareMag(const cXyz *vector);
f64 CbAction_sqrt(f64 value);
f64 CbAction_searchDistanceXZ(fopAc_ac_c *actor, fopAc_ac_c *player);
f64 CbAction_searchDistance2(fopAc_ac_c *actor, fopAc_ac_c *player);
f64 CbAction_searchDistanceXZ2(fopAc_ac_c *actor, fopAc_ac_c *player);
s32 CbAction_checkEndTactMusic(fopAc_ac_c *player);
void CbAction_initPlay(mDoExt_bckAnm *animation, u32 end, s32 mode, f32 speed, s32 start, s32 last,
                       s32 modify);
void CbAction_setNextStage(const char *stage, s32 start, s32 room, s32 layer, f32 speed, s32 wipe,
                           s32 mode, s32 param);
void CbAction_destroyBgCheck(void *check, s32 flags);
f64 CbAction_checkForwardGroundY(daNpc_Cb1_c *actor, s16 angle);
f64 CbAction_checkWallJump(daNpc_Cb1_c *actor, s16 angle);
void CbAction_outprod(cXyz *vector, cXyz *out, cXyz *other);
BOOL CbAction_routeCheck(daNpc_Cb1_c *actor, f64 distance, be<s16> *angle);
s32 CbAction_calcTimerU8(be<u8> *timer);
s32 CbAction_getRndValue(s16 minimum, s16 range);
BOOL CbAction_checkCoHit(dCcD_Cyl *cylinder);
void CbAction_doorOffsetPos(cXyz *out, cXyz *base, s16 angle, cXyz *offset);
void CbAction_monsSeStart(u32 sound, cXyz *pos, u32 actorId, s32 param, s32 reverb);
void CbAction_lookAtTarget(dNpc_JntCtrl_c *joint, be<s16> *angle, cXyz *target, cXyz *eye, s16 yaw,
                           s16 velocity, s32 headOnly);
bool CbAction_lockonTruth(u32 attention);
BOOL CbAction_checkTrigA(s32 pad);
BOOL CbAction_checkTrigB(s32 pad);

namespace cb1_action {
struct LineCheck : cBgS_LinChk, dBgS_Chk {};
WWHD_SIZE(LineCheck, 0x6C);
struct DebugRegisters {
    u8 mPrefix[0xA28];
    be<f32> mFloat[20];
    u8 mPadding[0x28];
    be<s16> mShort[8];
};
WWHD_OFFSET(DebugRegisters, mFloat, 0xA28);
WWHD_OFFSET(DebugRegisters, mShort, 0xAA0);
WWHD_SIZE(DebugRegisters, 0xAB0);
} // namespace cb1_action

namespace cb1_action {
// HD: virtual entries are loaded at the original points relative to other calls.
struct PlayerVtable {
    u8 mPrefix[0x2C];
    be<u32> mGetTactMusic;
    u8 mPadding[0x114 - 0x30];
    be<u32> mSetPlayerPosAndAngle;
};
WWHD_OFFSET(PlayerVtable, mGetTactMusic, 0x2C);
WWHD_OFFSET(PlayerVtable, mSetPlayerPosAndAngle, 0x114);
WWHD_SIZE(PlayerVtable, 0x118);
struct AnimationVtable {
    u8 mPrefix[0x14];
    be<u32> mGetFrameMax;
};
WWHD_SIZE(AnimationVtable, 0x18);
struct Animation {
    u8 mPrefix[4];
    gptr<AnimationVtable> mVtable;
};
WWHD_SIZE(Animation, 8);
struct BckAnimation {
    u8 mPrefix[0x88];
    gptr<Animation> mpAnm;
};
WWHD_OFFSET(BckAnimation, mpAnm, 0x88);
WWHD_SIZE(BckAnimation, 0x8C);
struct VmcView : fopAc_ac_c {
    u8 mPrefix[0x6D6 - 0x3AC];
    be<u8> mHasTree;
};
WWHD_OFFSET(VmcView, mHasTree, 0x6D6);
WWHD_SIZE(VmcView, 0x6D7);
struct WordVector {
    be<u32> x, y, z;
};
WWHD_SIZE(WordVector, 12);
struct Plane {
    cXyz mNormal;
    be<f32> mD;
    be<u32> mVtable;
};
WWHD_SIZE(Plane, 0x14);
struct Attention {
    u8 mPrefix[0x20];
    be<u32> mFlags;
};
WWHD_SIZE(Attention, 0x24);
} // namespace cb1_action

namespace cb1_action {
// GameCube animation/sound names and HD inline-constructor identities.
enum : u32 {
    ANM_08 = 0x8,
    ANM_00 = 0x0,
    ANM_01 = 0x1,
    ANM_17 = 0x17,
    ANM_0C = 0xC,
    ANM_02 = 0x2,
    ANM_16 = 0x16,
    ANM_09 = 0x9,
    ANM_0D = 0xD,
    ANM_04 = 0x4,
    ANM_0B = 0xB,
    JA_SE_CM_CB_BOUND = 0x5945,
    JA_SE_CV_CB_LEAF_OUT = 0x48C0,
    JA_SE_CV_CB_DAMAGE_S = 0x48BE,
    LineCheckVtable = 0x10018D9C,
    LinePolyVtable = 0x10018DAC,
    LinePassVtable = 0x10018DCC,
    LineGroupVtable = 0x10018DBC,
    BaseGroupVtable = 0x10018D4C,
    BaseLinePolyVtable = 0x10018D14,
};
} // namespace cb1_action
