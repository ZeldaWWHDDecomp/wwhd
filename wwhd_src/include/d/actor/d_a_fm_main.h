/* Floormaster main: private layouts for the verified HD fields.
 * HIO is 0x15C (GC 0x158); its JointHit HIO subobject grows by four bytes.
 * J3D model/node, player virtuals and particles use the HD layouts asserted below.
 * The actor layout shared by all four Floormaster parts stays in d_a_fm.h. */
#pragma once
#include "d/actor/d_a_fm.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
namespace fm_main {
struct HIO {
    be<u32> __vtbl;
    /* 0x004 */ be<s8> mDebugTargets;  /* was field_0x004: debugDraw 021439A0: while set (and the body is out) marks mpActorTarget / mBaseTarget; tww debugDraw the same; ctor 0214996C clears it */
    /* 0x005 */ be<u8> mDebugHeadTop;  /* was field_0x005: debugDraw 021439A0 tests it; tww debugDraw reads Link's getHeadTopPos() under it; ctor 0214996C clears it */
    /* 0x006 */ be<u8> mDebugGrabAxis;  /* was field_0x006: debugDraw 021439A0 tests it; tww debugDraw draws the axis of mGrabMtx (dLib_debugDrawAxis) under it; ctor 0214996C clears it */
    /* 0x007 */ be<u8> mDebugHandAxis;  /* was field_0x007: debugDraw 021439A0 tests it; tww debugDraw draws the axis of the FM_JNT_TE_e joint matrix under it; ctor 0214996C clears it */
    /* 0x008 */ be<u8> field_0x008;
    /* 0x009 */ be<u8> mDebugAngles;  /* was field_0x009: debugDraw 021439A0 tests it; tww debugDraw draws the mGrabAngleMax / mTurnAngleMin fans (radius mReachDist) and the mHeightRange mark under it; ctor 0214996C clears it */
    /* 0x00A */ be<u8> mDebugGoalLine;  /* was field_0x00A: debugDraw 021439A0 tests it; tww debugDraw draws current.pos to mGoalPos under it; ctor 0214996C clears it */
    /* 0x00B */ be<u8> mDebugJoints;  /* was field_0x00B: debugDraw 021439A0 tests it (loop over the six joints, TE skipped); ctor 0214996C clears it */
    /* 0x00C */ be<u8> mDebugArea;  /* was field_0x00C: debugDraw 021439A0 tests it; tww debugDraw draws the 12 mAreaPoints, the line to mBaseTarget and mHomePos under it; ctor 0214996C clears it */
    /* 0x00D */ be<u8> mNoGrabDemo;  /* was field_0x00D: modeGrab 02147BE0 / modeGrabFootDemo 021487F8: while set the grab demo is not started (sink timer and item-throw timer end without MODE_GRAB_DEMO / MODE_THROW); ctor 0214996C clears it */
    /* 0x00E */ be<u8> field_0x00E;
    /* 0x00F */ be<u8> mIkJointOn[6];  /* was field_0x00F: _nodeControl 021404AC applies the IK quaternion of joint i only while [i] == 1; calcInvKine 021468E8 solves only those joints; ctor 0214996C sets 1 / 0 per joint */
    /* 0x015 */ u8 field_0x015[0x01F - 0x015];
    /* 0x01F */ be<u8> field_0x01F;
    /* 0x020 */ cXyz mHoleOffset;  /* was field_0x020: holeSetMtx 0214162C translates the hole model by it after the Y rotation; ctor 0214996C (-10, 5, -10) */
    /* 0x02C */ cXyz mBodyOffsetInit;  /* was field_0x02C: createInit 02141930 copies it into daFm_c::mBodyOffset (the body model translation in bodySetMtx); ctor 0214996C (0, 0, -30) */
    /* 0x038 */ cXyz mHandOffset;  /* was field_0x038: _nodeControl 021404AC transforms it by the TE joint matrix into mHandPos; grabPlayer 021429BC / grabBomb 02142E0C / grabTsubo 02142EF4 add it to the carried position; ctor 0214996C (40, -20, 0) */
    /* 0x044 */ cXyz field_0x044;
    /* 0x050 */ cXyz field_0x050;
    /* 0x05C */ csXyz field_0x05C;
    /* 0x062 */ be<s16> mFreezeDuration;
    /* 0x064 */ be<s16> mMaxHealth;
    /* 0x066 */ be<s16> mUnderFootTime;  /* was field_0x066: modeUnderFootInit 0214468C loads mModeTimer with it (the wait before appear / hide / foot grab); ctor 0214996C 0x28 */
    /* 0x068 */ be<s16> mBikubikuTime;  /* was field_0x068: modeBikubikuInit 021497C0 loads mModeTimer with it; ctor 0214996C 0x1E */
    /* 0x06A */ be<s16> field_0x06A;
    /* 0x06C */ be<f32> mSpAttackVJumpHeight;
    /* 0x070 */ be<f32> mSpAttackVJumpRadius;
    /* 0x074 */ be<s16> mSpAttackVJumpStartFrame;
    /* 0x076 */ be<s16> mSpAttackVJumpEndFrame;
    /* 0x078 */ be<f32> mSpAttackJumpHeight;
    /* 0x07C */ be<f32> mSpAttackJumpRadius;
    /* 0x080 */ be<s16> mSpAttackJumpStartFrame;
    /* 0x082 */ be<s16> mSpAttackJumpEndFrame;
    /* 0x084 */ be<s16> mHideTimeMin;  /* was field_0x084: modeHideInit 02144568: mModeTimer = setRnd(it, mHideTimeMax); ctor 0214996C 0x5A */
    /* 0x086 */ be<s16> mHideTimeMax;  /* was field_0x086: modeHideInit 02144568: mModeTimer = setRnd(mHideTimeMin, it); ctor 0214996C 0xF0 */
    /* 0x088 */ be<s16> mFarTime;  /* was field_0x088: modeWaitInit 02146434 / modeWait 021464B4 load mFarTimer with it; modeWait disappears when the base target stays beyond mNoticeRange until it runs out; ctor 0214996C 0x3C */
    /* 0x08A */ be<s16> field_0x08A;
    /* 0x08C */ be<s16> mWaitTimeMin;  /* was field_0x08C: modeWaitInit 02146434: mModeTimer = setRnd(it, mWaitTimeMax); ctor 0214996C 0x3C */
    /* 0x08E */ be<s16> mWaitTimeMax;  /* was field_0x08E: modeWaitInit 02146434: mModeTimer = setRnd(mWaitTimeMin, it); ctor 0214996C 0x78 */
    /* 0x090 */ be<s16> mGrabLinkSinkTime;  /* was field_0x090: modeGrabInit 02147810 loads mSinkTimer with it when the target is Link (mGrabNpcSinkTime otherwise); modeGrab 02147BE0 starts the grab demo when it runs out; ctor 0214996C 10 */
    /* 0x092 */ be<s16> mItemThrowTime;  /* was field_0x092: modeGrabInit 02147810 loads mModeTimer with it; modeGrab 02147BE0 reloads it while Link guards and throws the held bomb / pot (MODE_THROW) when it runs out; ctor 0214996C 0x11 */
    /* 0x094 */ be<s16> mAttackTime;  /* was field_0x094: modeAttackInit 02146764 loads mModeTimer with it; modeAttack 02146A34 ends the attack when the animation stops and it ran out; ctor 0214996C 0 */
    /* 0x096 */ be<s16> mTurnStep;  /* was field_0x096: modeAttack 02146A34 / modeGrab 02147BE0: the max step of cLib_addCalcAngleS2 turning shape_angle.y to the target; ctor 0214996C 500 */
    /* 0x098 */ be<s16> mGrabAngleMax;  /* was field_0x098: isGrabPos 02145F90 refuses while the angle to the grab position differs from shape_angle.y by more; ctor 0214996C 18000 */
    /* 0x09A */ be<s16> mTurnAngleMin;  /* was field_0x09A: modeWait 021464B4 / modeAttack 02146A34 / modeGrab 02147BE0 turn only while the angle difference is above it; ctor 0214996C 7000 */
    /* 0x09C */ be<s16> mTgHitTime;  /* was field_0x09C: checkTgHit 02145070 loads mTgHitTimer with it after a hit; ctor 0214996C 5 */
    /* 0x09E */ be<s16> mGrabNpcSinkTime;  /* was field_0x09E: modeGrabInit 02147810 loads mSinkTimer with it when the target is not Link (mGrabLinkSinkTime otherwise); ctor 0214996C 0x96 */
    /* 0x0A0 */ be<s16> mRndMoveTime;  /* was field_0x0A0: moveRndBack 02144DE4 / moveRndEscape 02147914 reload mMoveTimer with it when they pick a new random point; ctor 0214996C 0x14 */
    /* 0x0A2 */ be<s16> field_0x0A2;
    /* 0x0A4 */ be<s16> field_0x0A4;
    /* 0x0A6 */ be<s16> mDamageTime;  /* was field_0x0A6: modeDamageInit 02147724 loads mModeTimer with it; ctor 0214996C 10 */
    /* 0x0A8 */ be<f32> mHeightRange;  /* was field_0x0A8: the vertical range: checkHeight 02140CC0 (half of it), modeHide 021445B8 / modeUnderFoot 02144838 (base target against mHomePos.y), modePathMove 02144A18 / modeGoalKeeper 02144B94 / isGrabPos 02145F90 / isGrab 021467D8 / modeAttack 02146A34 compare a height difference with it; ctor 0214996C 600 */
    /* 0x0AC */ be<f32> mRndBackDist;  /* was field_0x0AC: moveRndBack 02144DE4: the new mMoveTargetPos is it along mMoveAngle; ctor 0214996C 300 */
    /* 0x0B0 */ be<f32> mRndMoveStep;  /* was field_0x0B0: moveRndBack 02144DE4 / moveRndEscape 02147914: the max step of fm_addCalcPosXZ2 toward mMoveTargetPos; ctor 0214996C 30 */
    /* 0x0B4 */ be<f32> mEscapeRange;  /* was field_0x0B4: moveRndEscape 02147914 / modeGrab 02147BE0 compare the player distance with it (escape with an NPC, goal-keeper throw check); ctor 0214996C 1000 */
    /* 0x0B8 */ be<f32> mRndEscapeDist;  /* was field_0x0B8: moveRndEscape 02147914: the new mMoveTargetPos is it along mMoveAngle from mGoalPos; ctor 0214996C 1000 */
    /* 0x0BC */ be<f32> mNpcDemoPlayerDist;  /* was field_0x0BC: modeAttack 02146A34: after grabbing an NPC, MODE_GRAB_NPC_DEMO starts while the player is farther than it (or out of mHeightRange); ctor 0214996C 750 */
    /* 0x0C0 */ be<f32> mUnderFootStep;  /* was field_0x0C0: modeUnderFoot 02144838: the max step of fm_addCalcPosXZ2 toward the base target; ctor 0214996C 15 */
    /* 0x0C4 */ be<f32> mReachDist;  /* was field_0x0C4: modeAttack 02146A34 does not chase a grab position between mNearRange and it; modeGrab 02147BE0 throws the held NPC at the player inside it; tww debugDraw uses it as the radius of the angle fans; ctor 0214996C 290 */
    /* 0x0C8 */ be<f32> mSphRadius;  /* was field_0x0C8: setCollision 021432B8: mSph.SetR(it); ctor 0214996C 50 */
    /* 0x0CC */ be<f32> mChaseSpeed;  /* was field_0x0CC: modeGoalKeeper 02144B94 / modeAttack 02146A34 / moveRndEscape 02147914: step rate (it + 1) * 0.005 * mSpeedRate of fm_addCalcPosXZ2; ctor 0214996C 7 */
    /* 0x0D0 */ be<f32> mPathSpeed;  /* was field_0x0D0: modePathMove 02144A18: step rate (it + 1) * 0.005 * mSpeedRate toward mPathPos; ctor 0214996C 2 */
    /* 0x0D4 */ be<f32> mPathReachDist;  /* was field_0x0D4: _pathMove 02140AC8: a path point counts as reached within it + 1; ctor 0214996C 5 */
    /* 0x0D8 */ be<f32> mPathRndOffset;  /* was field_0x0D8: _pathMove 02140AC8: the reached point moves along the segment by it times a random 1-10; ctor 0214996C 80 */
    /* 0x0DC */ be<f32> mFootGrabDist;  /* was field_0x0DC: isGrabFoot 0214472C: Link must be within it (XZ); calcInvKine 021468E8 solves the IK only for a target within it; ctor 0214996C 100 */
    /* 0x0E0 */ be<f32> mNearRange;  /* was field_0x0E0: areaCheck 0214264C: the radius of the 12 area points; isGrabPos 02145F90 / modeWait 021464B4 / modeAttack 02146A34: the inner distance limit; moveRndBack 02144DE4; ctor 0214996C 150 */
    /* 0x0E4 */ be<f32> mAttackRange;  /* was field_0x0E4: searchNearOtherActor 02140D04 / searchTarget 02145A60 take targets within it; modeUnderFoot 02144838 / modePathMove 02144A18 appear when the base target is within it; isGrabPos 02145F90 outer limit for non-Link targets; ctor 0214996C 500 */
    /* 0x0E8 */ be<f32> mNoticeRange;  /* was field_0x0E8: setBaseTarget 02141714 picks Medli / Link within it; _execute 02143418 line-checks the base target within it; modeGoalKeeper 02144B94 / modeWait 021464B4 (turn between mAttackRange and it, mFarTimer beyond it); ctor 0214996C 800 */
    /* 0x0EC */ be<f32> mGrabReach;  /* was field_0x0EC: isGrab 021467D8: the hand (mHandPos) must be within it of mGrabPos (doubled with mSwAppearDemo); setAttention 021422A4 moves eyePos while Link's head top is within it of the hand; ctor 0214996C 30 */
    /* 0x0F0 */ be<f32> mThrowSpeedF;  /* was field_0x0F0: modeThrow 02147408: the thrown target's speedF; ctor 0214996C 25 */
    /* 0x0F4 */ be<f32> mThrowSpeedY;  /* was field_0x0F4: modeThrow 02147408: the thrown target's speed.y; ctor 0214996C 9 */
    /* 0x0F8 */ be<f32> mThrowGravity;  /* was field_0x0F8: modeThrow 02147408: the thrown target's gravity; ctor 0214996C -2.5 */
    /* 0x0FC */ be<f32> mThrowFrame;  /* was field_0x0FC: modeThrow 02147408 releases the target at this animation frame; ctor 0214996C 16 */
    /* 0x100 */ be<s16> mGrabDelayTime;  /* was field_0x100: modeWaitInit 02146434 loads mGrabDelayTimer with it (modeWait attacks a held target only after it); ctor 0214996C 0x1E */
    u8 mPadding102[2];
    /* 0x104 */ cXyz mPullOffset;  /* was field_0x104: _nodeControl 021404AC transforms it by the TE joint matrix into mPullPos; ctor 0214996C (0, -20, 0) */
    /* 0x110 */ be<f32> mPullDistMin;  /* was field_0x110: modeAttack 02146A34 (fm_pushLink) / modeGrab 02147BE0 pull Link only while mGrabPos is farther than it from mPullPos; ctor 0214996C 20 */
    /* 0x114 */ be<f32> mPullDistMax;  /* was field_0x114: modeAttack 02146A34 (fm_pushLink) / modeGrab 02147BE0 pull Link only while mGrabPos is nearer than it to mPullPos; ctor 0214996C 50 */
    /* 0x118 */ be<f32> mPullPower;  /* was field_0x118: fm_pushLink (in modeAttack 02146A34) / modeGrab 02147BE0: daPy_setOutPower power = it * the distance ratio; ctor 0214996C -20 */
    /* 0x11C */ be<s16> mParalysisTime;  /* was field_0x11C: modeParalysisInit 02149688 loads mModeTimer with it; modeParalysis 021496C8 shakes between it - 0x14 and it - 0x3C; ctor 0214996C 100 */
    /* 0x11E */ be<s16> field_0x11E;
    /* 0x120 */ be<f32> mHoleScaleClosed;  /* was field_0x120: the closed hole scale: modeSwWaitInit 02144190 / modePlayerStartDemoInit 021491B4 set mHoleScale to it; iceProc 021424D0 / modeHide 021445B8 / modeDeath 02148CC0 / modeDelete 021495F8 shrink toward it; ctor 0214996C 0.01 */
    /* 0x124 */ be<f32> mHoleScaleOpen;  /* was field_0x124: the open hole scale: modePathMoveInit 021449C4 sets mHoleScale to it; modeSwWait 021442A8 / modeUnderFoot 02144838 / modePlayerStartDemo 02149208 grow toward it; ctor 0214996C 1 */
    /* 0x128 */ be<f32> mHoleScaleStep;  /* was field_0x128: the max step of setHoleScale toward mHoleScaleClosed / mHoleScaleOpen (iceProc 021424D0, modeHide 021445B8, modeSwWait 021442A8, modeDeath 02148CC0, modePlayerStartDemo 02149208, modeDelete 021495F8); ctor 0214996C 0.1 */
    /* 0x12C */ be<f32> mHoleScaleStepUnderFoot;  /* was field_0x12C: modeUnderFoot 02144838: the max step of setHoleScale toward mHoleScaleOpen; ctor 0214996C 0.02 */
    /* 0x130 */ u8 mJntHitHIO[0x2C];  /* was field_0x130: ctor 0214996C constructs a JntHit_HIO_c in it (tww: JntHit_HIO_c mJntHitHIO) */};
WWHD_OFFSET(HIO, mDebugTargets, 0x4);
WWHD_OFFSET(HIO, mDebugHeadTop, 0x5);
WWHD_OFFSET(HIO, mDebugGrabAxis, 0x6);
WWHD_OFFSET(HIO, mDebugHandAxis, 0x7);
WWHD_OFFSET(HIO, field_0x008, 0x8);
WWHD_OFFSET(HIO, mDebugAngles, 0x9);
WWHD_OFFSET(HIO, mDebugGoalLine, 0xA);
WWHD_OFFSET(HIO, mDebugJoints, 0xB);
WWHD_OFFSET(HIO, mDebugArea, 0xC);
WWHD_OFFSET(HIO, mNoGrabDemo, 0xD);
WWHD_OFFSET(HIO, field_0x00E, 0xE);
WWHD_OFFSET(HIO, mIkJointOn, 0xF);
WWHD_OFFSET(HIO, field_0x015, 0x15);
WWHD_OFFSET(HIO, field_0x01F, 0x1F);
WWHD_OFFSET(HIO, mHoleOffset, 0x20);
WWHD_OFFSET(HIO, mBodyOffsetInit, 0x2C);
WWHD_OFFSET(HIO, mHandOffset, 0x38);
WWHD_OFFSET(HIO, field_0x044, 0x44);
WWHD_OFFSET(HIO, field_0x050, 0x50);
WWHD_OFFSET(HIO, field_0x05C, 0x5C);
WWHD_OFFSET(HIO, mFreezeDuration, 0x62);
WWHD_OFFSET(HIO, mMaxHealth, 0x64);
WWHD_OFFSET(HIO, mUnderFootTime, 0x66);
WWHD_OFFSET(HIO, mBikubikuTime, 0x68);
WWHD_OFFSET(HIO, field_0x06A, 0x6A);
WWHD_OFFSET(HIO, mSpAttackVJumpHeight, 0x6C);
WWHD_OFFSET(HIO, mSpAttackVJumpRadius, 0x70);
WWHD_OFFSET(HIO, mSpAttackVJumpStartFrame, 0x74);
WWHD_OFFSET(HIO, mSpAttackVJumpEndFrame, 0x76);
WWHD_OFFSET(HIO, mSpAttackJumpHeight, 0x78);
WWHD_OFFSET(HIO, mSpAttackJumpRadius, 0x7C);
WWHD_OFFSET(HIO, mSpAttackJumpStartFrame, 0x80);
WWHD_OFFSET(HIO, mSpAttackJumpEndFrame, 0x82);
WWHD_OFFSET(HIO, mHideTimeMin, 0x84);
WWHD_OFFSET(HIO, mHideTimeMax, 0x86);
WWHD_OFFSET(HIO, mFarTime, 0x88);
WWHD_OFFSET(HIO, field_0x08A, 0x8A);
WWHD_OFFSET(HIO, mWaitTimeMin, 0x8C);
WWHD_OFFSET(HIO, mWaitTimeMax, 0x8E);
WWHD_OFFSET(HIO, mGrabLinkSinkTime, 0x90);
WWHD_OFFSET(HIO, mItemThrowTime, 0x92);
WWHD_OFFSET(HIO, mAttackTime, 0x94);
WWHD_OFFSET(HIO, mTurnStep, 0x96);
WWHD_OFFSET(HIO, mGrabAngleMax, 0x98);
WWHD_OFFSET(HIO, mTurnAngleMin, 0x9A);
WWHD_OFFSET(HIO, mTgHitTime, 0x9C);
WWHD_OFFSET(HIO, mGrabNpcSinkTime, 0x9E);
WWHD_OFFSET(HIO, mRndMoveTime, 0xA0);
WWHD_OFFSET(HIO, field_0x0A2, 0xA2);
WWHD_OFFSET(HIO, field_0x0A4, 0xA4);
WWHD_OFFSET(HIO, mDamageTime, 0xA6);
WWHD_OFFSET(HIO, mHeightRange, 0xA8);
WWHD_OFFSET(HIO, mRndBackDist, 0xAC);
WWHD_OFFSET(HIO, mRndMoveStep, 0xB0);
WWHD_OFFSET(HIO, mEscapeRange, 0xB4);
WWHD_OFFSET(HIO, mRndEscapeDist, 0xB8);
WWHD_OFFSET(HIO, mNpcDemoPlayerDist, 0xBC);
WWHD_OFFSET(HIO, mUnderFootStep, 0xC0);
WWHD_OFFSET(HIO, mReachDist, 0xC4);
WWHD_OFFSET(HIO, mSphRadius, 0xC8);
WWHD_OFFSET(HIO, mChaseSpeed, 0xCC);
WWHD_OFFSET(HIO, mPathSpeed, 0xD0);
WWHD_OFFSET(HIO, mPathReachDist, 0xD4);
WWHD_OFFSET(HIO, mPathRndOffset, 0xD8);
WWHD_OFFSET(HIO, mFootGrabDist, 0xDC);
WWHD_OFFSET(HIO, mNearRange, 0xE0);
WWHD_OFFSET(HIO, mAttackRange, 0xE4);
WWHD_OFFSET(HIO, mNoticeRange, 0xE8);
WWHD_OFFSET(HIO, mGrabReach, 0xEC);
WWHD_OFFSET(HIO, mThrowSpeedF, 0xF0);
WWHD_OFFSET(HIO, mThrowSpeedY, 0xF4);
WWHD_OFFSET(HIO, mThrowGravity, 0xF8);
WWHD_OFFSET(HIO, mThrowFrame, 0xFC);
WWHD_OFFSET(HIO, mGrabDelayTime, 0x100);
WWHD_OFFSET(HIO, mPullOffset, 0x104);
WWHD_OFFSET(HIO, mPullDistMin, 0x110);
WWHD_OFFSET(HIO, mPullDistMax, 0x114);
WWHD_OFFSET(HIO, mPullPower, 0x118);
WWHD_OFFSET(HIO, mParalysisTime, 0x11C);
WWHD_OFFSET(HIO, field_0x11E, 0x11E);
WWHD_OFFSET(HIO, mHoleScaleClosed, 0x120);
WWHD_OFFSET(HIO, mHoleScaleOpen, 0x124);
WWHD_OFFSET(HIO, mHoleScaleStep, 0x128);
WWHD_OFFSET(HIO, mHoleScaleStepUnderFoot, 0x12C);
WWHD_OFFSET(HIO, mJntHitHIO, 0x130);
WWHD_SIZE(HIO, 0x15C);
inline HIO& hio() { return *gabi::at<HIO>(0x10463E0C); }
struct ModelMatrixBlock {
    u8 pad0[4];
    /* 0x004 */ be<u16> mFlags;
    u8 pad6[10];
    /* 0x010 */ gptr<Mtx34> mAnmMtx;};
WWHD_OFFSET(ModelMatrixBlock, mAnmMtx, 0x10);
struct JointNode {
    u8 pad0[8];
    be<u32> mCallBack;
    u8 padC[0x10];};
WWHD_SIZE(JointNode, 0x1C);
struct ModelData {
    u8 pad0[4];
    /* 0x004 */ be<u32> mJointCount;
    /* 0x008 */ gptr<JointNode> mJointNodes;};
struct Model {
    u8 pad0[0x2C];
    /* 0x02C */ gptr<ModelMatrixBlock> mMatrixBlock;
    u8 pad30[0x7C];
    /* 0x0AC */ gptr<J3DModelData> mModelData;
    u8 padB0[8];
    /* 0x0B8 */ gptr<daFm_c> mUserArea;
    /* 0x0BC */ cXyz mBaseScale;};
WWHD_OFFSET(Model, mModelData, 0xAC);
WWHD_OFFSET(Model, mUserArea, 0xB8);
WWHD_OFFSET(Model, mBaseScale, 0xBC);
struct Joint {
    u8 pad0[4];
    /* 0x004 */ be<u16> mJointNo;};
struct JointTree {
    u8 pad0[8];
    /* 0x008 */ be<u16> mJointCount;};
struct Player : fopAc_ac_c {
    u8 pad3AC[0x2C];
    /* 0x3D8 */ cXyz mHeadTopPos;};
WWHD_OFFSET(Player, mHeadTopPos, 0x3D8);
struct PlayerVtable {
    u8 pad0[0xBC];
    /* 0x0BC */ be<u32> getGrabActorID;};
struct FollowVtable {
    u8 pad0[0x44];
    /* 0x044 */ be<u32> remove;};
struct Follow {
    /* 0x000 */ gptr<FollowVtable> mVtable;};
struct Emitter {
    u8 pad0[0x220];
    /* 0x220 */ cXyz mGlobalScale;
    u8 pad22C[12];
    /* 0x238 */ cXyz mGlobalParticleScale;};
WWHD_OFFSET(Emitter, mGlobalParticleScale, 0x238);
struct QuaternionWords {
    /* 0x00C */ be<u32> x,y,z,w;};
struct LineCheck {
    u8 pad0[0x20];
    /* 0x020 */ be<u32> mVtable20;
    u8 pad24[0xC];
    /* 0x030 */ cXyz mCross;
    u8 pad3C[0x1C];
    /* 0x058 */ be<u32> mVtable58;
    u8 pad5C[8];
    /* 0x064 */ be<u32> mVtable64;};
WWHD_OFFSET(LineCheck, mCross, 0x30);
WWHD_OFFSET(LineCheck, mVtable58, 0x58);
WWHD_OFFSET(LineCheck, mVtable64, 0x64);
struct Acch {
    u8 pad0[0x14];
    /* 0x014 */ be<u32> mVtable14;
    u8 pad18[8];
    /* 0x020 */ be<u32> mVtable20;};
inline Model* model(J3DModel* m) { return reinterpret_cast<Model*>(m); }
inline J3DModel* currentModel() { return *gabi::at<gptr<J3DModel>>(0x104B462C); }
inline const QuaternionWords& zeroQuat() { return *gabi::at<const QuaternionWords>(0x101E9C38); }
inline Mtx34* stackMtx() { return gabi::at<Mtx34>(0x1048D0CC); }
inline Mtx34* currentMtx() { return gabi::at<Mtx34>(0x104B4868); }
inline Mtx34* anmMtx(J3DModel* m, u32 i) { auto* block=model(m)->mMatrixBlock.get(); Mtx34* matrices=block->mAnmMtx; block->mFlags |= 0x10; return matrices+i; }
inline JointNode* jointNode(J3DModelData* d,u32 i) { auto* data=reinterpret_cast<ModelData*>(d); u32 count=data->mJointCount; JointNode* nodes=data->mJointNodes; return i<count?nodes+i:nodes; }
inline u32 headerGuards() { return 0x10463DF0; }
inline u32 headerDestructors() { return 0x101B5228; }
constexpr u32 SafeStringVtable=0x1000F258, ActorVtable=0x1000F350, HIOVtable=0x1000F360, AabVtable=0x1000F270;
}
inline u32 daPy_getGrabActorID(fopAc_ac_c* p) { return gabi::call_ptr<u32>(gabi::at<fm_main::PlayerVtable>(p->__vtbl)->getGrabActorID,p); }
inline cXyz* daPy_getHeadTopPos(fopAc_ac_c* p) { return &reinterpret_cast<fm_main::Player*>(p)->mHeadTopPos; }
inline void fm_seStart(fopAc_ac_c* a,u32 id,u32 param) { s32 reverb=dComIfGp_getReverb(a->current.roomNo); mDoAud_seStart(id,&a->eyePos,param,reverb); }
void mDoMtx_stack_quatM(Quaternion_fm* q);
void PSMTXConcat_g(Mtx34* a, Mtx34* b, Mtx34* ab);
void cXyz_normZP(const cXyz* a, cXyz* res);
f32 PSVECSquareDistance(const cXyz* a, const cXyz* b);
bool daBomb_chk_state(fopAc_ac_c* b, s32 st);
u32 daObj_PrmAbstract(fopAc_ac_c* a, s32 w, s32 s);
fm_main::JointTree* fm_jointTree(J3DModelData* d);
void fm_jointHitHIO_ct(void* h);
void fm_invisible_ct(void* p);
void fm_sphere_ct(dCcD_Sph* p);
void fm_setAnmTables(const char* arc, mDoExt_McaMorf* morf, be<s8>* bck, be<s8>* prm, be<s8>* old, u32 bcks, u32 prms, bool force, s32 hd);
namespace fm_main {
struct AcchCir {
    u8 pad0[0x14];
    u8 mCircle[0xC];};
WWHD_SIZE(AcchCir, 0x20);
constexpr u32 AcchVtable20=0x1000F2B0, AcchVtable14=0x1000F2C0;
constexpr u32 LineVtable58=0x1000F300, LineVtable64=0x1000F290, LineVtable20=0x1000F280;
}
void fm_acch_dt(dBgS_ObjAcch* p, s32 flags);
void fm_circle_dt(void* p, s32 flags);
void fm_line_dt(void* p, s32 flags);
void fm_sphere_dt(dCcD_Sph* p, s32 flags);
void fm_invisible_dt(void* p, s32 flags);
void fm_actor_dt(fopAc_ac_c* p, s32 flags);
s32 fm_calcTimer(be<s32>* timer);
s32 fm_nameIndex(void* table, const char* name, s32 type);
inline s32 fm_charNameIndex(const char* name,s32 type) { return fm_nameIndex(gabi::at<void>(dComIfGp_ea() + PLAY_NAMETBL),name,type); }

bool invisibleModel_create(void* inv, J3DModel* model);
JntHit_c* JntHit_create(J3DModel* model, u32 data, s16 count);
namespace fm_main {
struct AnimationParameters {
    be<s8> mAnmIdx, mNextPrmIdx;
    be<s16> field_0x02;
    be<f32> mMorf,mPlaySpeed;
    be<s32> mLoopMode;};
WWHD_SIZE(AnimationParameters,0x10);
struct ModeRecord {
    be<s16> mInitDelta,mInitIndex;
    be<u32> mInitFunction;
    be<s16> mRunDelta,mRunIndex;
    be<u32> mRunFunction;
    gptr<const char> mName;};
WWHD_SIZE(ModeRecord,0x14);
using ModeFunction=void (daFm_c::*)();
inline const gabi::ptmf<ModeFunction>& initFunction(ModeRecord& record) { return *reinterpret_cast<gabi::ptmf<ModeFunction>*>(&record.mInitDelta); }
inline const gabi::ptmf<ModeFunction>& runFunction(ModeRecord& record) { return *reinterpret_cast<gabi::ptmf<ModeFunction>*>(&record.mRunDelta); }
struct JointHitRecord {
    be<s16> mShapeType,mJointIndex;
    be<f32> mRadius;
    gptr<cXyz> mpOffsets;};
WWHD_SIZE(JointHitRecord,0xC);
// dCcD source geometry is opaque in the shared header; these mirror its flat data records.
struct SphereSource {
    be<u32> mFlags,mAttackType;
    be<u8> mAttackPower;
    u8 padding[3];
    be<u32> mAttackParameters,mTargetType,mTargetParameters,mCollisionParameters;
    be<u8> mAttackSe,mAttackHitMark,mAttackSpecial,mAttackMaterial;
    be<u32> mAttackFlags;
    be<u8> mTargetSe,mTargetHitMark,mTargetSpecial,mTargetMaterial;
    be<u32> mTargetFlags,mCollisionFlags;
    be<f32> mCenterX,mCenterY,mCenterZ,mRadius;};
WWHD_SIZE(SphereSource,0x40);
struct CylinderSource {
    be<u32> mFlags,mAttackType;
    be<u8> mAttackPower;
    u8 padding[3];
    be<u32> mAttackParameters,mTargetType,mTargetParameters,mCollisionParameters;
    be<u8> mAttackSe,mAttackHitMark,mAttackSpecial,mAttackMaterial;
    be<u32> mAttackFlags;
    be<u8> mTargetSe,mTargetHitMark,mTargetSpecial,mTargetMaterial;
    be<u32> mTargetFlags,mCollisionFlags;
    be<f32> mCenterX,mCenterY,mCenterZ,mRadius,mHeight;};
WWHD_SIZE(CylinderSource,0x44);
}

namespace fm_main {
WWHD_SIZE(ModelMatrixBlock, 0x14);
WWHD_OFFSET(ModelMatrixBlock, mFlags, 0x4);
WWHD_SIZE(ModelData, 0xC);
WWHD_OFFSET(ModelData, mJointCount, 0x4);
WWHD_OFFSET(ModelData, mJointNodes, 0x8);
WWHD_SIZE(Model, 0xC8);
WWHD_OFFSET(Model, mMatrixBlock, 0x2C);
WWHD_SIZE(Joint, 0x6);
WWHD_OFFSET(Joint, mJointNo, 0x4);
WWHD_SIZE(JointTree, 0xA);
WWHD_OFFSET(JointTree, mJointCount, 0x8);
WWHD_SIZE(Player, 0x3E4);
WWHD_SIZE(PlayerVtable, 0xC0);
WWHD_OFFSET(PlayerVtable, getGrabActorID, 0xBC);
WWHD_SIZE(FollowVtable, 0x48);
WWHD_OFFSET(FollowVtable, remove, 0x44);
WWHD_SIZE(Follow, 0x4);
WWHD_OFFSET(Follow, mVtable, 0x0);
WWHD_SIZE(Emitter, 0x244);
WWHD_OFFSET(Emitter, mGlobalScale, 0x220);
WWHD_SIZE(QuaternionWords, 0x10);
WWHD_OFFSET(QuaternionWords, x, 0x0);
WWHD_OFFSET(QuaternionWords, y, 0x4);
WWHD_OFFSET(QuaternionWords, z, 0x8);
WWHD_OFFSET(QuaternionWords, w, 0xC);
WWHD_SIZE(LineCheck, 0x68);
WWHD_OFFSET(LineCheck, mVtable20, 0x20);
WWHD_SIZE(Acch, 0x24);
WWHD_OFFSET(Acch, mVtable14, 0x14);
WWHD_OFFSET(Acch, mVtable20, 0x20);
}
enum FmMode { MODE_SW_WAIT,MODE_HIDE,MODE_UNDERFOOT,MODE_PATH_MOVE,MODE_GOAL_KEEPER,MODE_APPEAR,MODE_DISAPPEAR,MODE_WAIT,MODE_ATTACK,MODE_THROW,MODE_DAMAGE,MODE_PREPARE_ITEM,MODE_DEATH,MODE_GRAB,MODE_GRAB_DEMO,MODE_GRAB_FOOT_DEMO,MODE_GRAB_NPC_DEMO,MODE_PLAYER_START_DEMO,MODE_DELETE,MODE_PARALYSIS,MODE_BIKUBIKU };
enum { FM_JNT_TE_e=5, FM_JNT_CYUBIA_e=6 };
enum { dRes_INDEX_FM_BCK_MODORU_e=0xC,dRes_INDEX_FM_BDL_FM_e=0x15,dRes_INDEX_FM_BDL_YPIT00_e=0x18,dRes_INDEX_FM_BTK_YPIT00_e=0x1B };
enum { fpcNm_FM_e=0x77,fpcNm_NPC_CB1_e=0x14E,fpcNm_NPC_MD_e=0x16F };
