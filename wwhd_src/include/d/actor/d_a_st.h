#pragma once
#include "d/d_cc_d.h"
#include "d/d_particle.h"
#include "f_op/f_op_actor.h"
// HD: actor base grows by 0x11C; line material adds another 0x12C before the
// tail. Field placement follows ctor02495AD0 and create02495C70; signed storage
// is retained where the verified HD implementation performs signed accesses.
enum dRes_INDEX_ST {
    /* BCK  */
    dRes_INDEX_ST_BCK_ATTACKL1_e = 0x4,
    dRes_INDEX_ST_BCK_ATTACKL1_END_e = 0x5,
    dRes_INDEX_ST_BCK_ATTACKL2_e = 0x6,
    dRes_INDEX_ST_BCK_ATTACKLEND_e = 0x7,
    dRes_INDEX_ST_BCK_ATTACKL_END_e = 0x8,
    dRes_INDEX_ST_BCK_ATTACKR1_e = 0x9,
    dRes_INDEX_ST_BCK_ATTACKR1_END_e = 0xA,
    dRes_INDEX_ST_BCK_ATTACKR2_e = 0xB,
    dRes_INDEX_ST_BCK_ATTACKREND_e = 0xC,
    dRes_INDEX_ST_BCK_ATTACKR_END_e = 0xD,
    dRes_INDEX_ST_BCK_FUKKI_WAIT_e = 0xE,
    dRes_INDEX_ST_BCK_HEADB_DAMAGE_e = 0xF,
    dRes_INDEX_ST_BCK_HEADB_DEAD_e = 0x10,
    dRes_INDEX_ST_BCK_HEADB_JUMP1_e = 0x11,
    dRes_INDEX_ST_BCK_HEADB_JUMP2_e = 0x12,
    dRes_INDEX_ST_BCK_HEADB_WAIT_e = 0x13,
    dRes_INDEX_ST_BCK_HONE_ATTACK_e = 0x14,
    dRes_INDEX_ST_BCK_HYUNHYUN_e = 0x15,
    dRes_INDEX_ST_BCK_KAITENL1_e = 0x16,
    dRes_INDEX_ST_BCK_KAITENL1_END_e = 0x17,
    dRes_INDEX_ST_BCK_KAITENL2_e = 0x18,
    dRes_INDEX_ST_BCK_KAITENL2_END_e = 0x19,
    dRes_INDEX_ST_BCK_KAITENR1_e = 0x1A,
    dRes_INDEX_ST_BCK_KAITENR1_END_e = 0x1B,
    dRes_INDEX_ST_BCK_KAITENR2_e = 0x1C,
    dRes_INDEX_ST_BCK_KAITENR2_END_e = 0x1D,
    dRes_INDEX_ST_BCK_KICK1_e = 0x1E,
    dRes_INDEX_ST_BCK_KICK2_e = 0x1F,
    dRes_INDEX_ST_BCK_KYORO_e = 0x20,
    dRes_INDEX_ST_BCK_LWAIT_e = 0x21,
    dRes_INDEX_ST_BCK_LWALK_e = 0x22,
    dRes_INDEX_ST_BCK_MAHI2_e = 0x23,
    dRes_INDEX_ST_BCK_MAHIL_e = 0x24,
    dRes_INDEX_ST_BCK_MAHIR_e = 0x25,
    dRes_INDEX_ST_BCK_NUKU1_e = 0x26,
    dRes_INDEX_ST_BCK_NUKU2_e = 0x27,
    dRes_INDEX_ST_BCK_OTOTOL_e = 0x28,
    dRes_INDEX_ST_BCK_OTOTOR_e = 0x29,
    dRes_INDEX_ST_BCK_SITA_WALK_e = 0x2A,
    dRes_INDEX_ST_BCK_START_e = 0x2B,
    dRes_INDEX_ST_BCK_TATE_START_e = 0x2C,
    dRes_INDEX_ST_BCK_TUKERU1_e = 0x2D,
    dRes_INDEX_ST_BCK_TUKERU2_e = 0x2E,
    dRes_INDEX_ST_BCK_UE_JUMP1_e = 0x2F,
    dRes_INDEX_ST_BCK_UE_JUMP2_e = 0x30,
    dRes_INDEX_ST_BCK_UE_WAIT_e = 0x31,
    dRes_INDEX_ST_BCK_UE_WALK_e = 0x32,
    dRes_INDEX_ST_BCK_WAIT_e = 0x33,
    dRes_INDEX_ST_BCK_WALK_e = 0x34,
    dRes_INDEX_ST_BCK_YOKO_START_e = 0x35,
    /* BDLM */
    dRes_INDEX_ST_BDL_HEADB_e = 0x38,
    dRes_INDEX_ST_BDL_ST_e = 0x39,
    dRes_INDEX_ST_BDL_ST_AGO_e = 0x3A,
    dRes_INDEX_ST_BDL_ST_ASIL_e = 0x3B,
    dRes_INDEX_ST_BDL_ST_ASIR_e = 0x3C,
    dRes_INDEX_ST_BDL_ST_BUKI_e = 0x3D,
    dRes_INDEX_ST_BDL_ST_HANDL_e = 0x3E,
    dRes_INDEX_ST_BDL_ST_HANDR_e = 0x3F,
    dRes_INDEX_ST_BDL_ST_HARA_e = 0x40,
    dRes_INDEX_ST_BDL_ST_HAT_e = 0x41,
    dRes_INDEX_ST_BDL_ST_HEAD_e = 0x42,
    dRes_INDEX_ST_BDL_ST_KATAL_e = 0x43,
    dRes_INDEX_ST_BDL_ST_KATAR_e = 0x44,
    dRes_INDEX_ST_BDL_ST_KOTUBAN_e = 0x45,
    dRes_INDEX_ST_BDL_ST_KUBI_e = 0x46,
    dRes_INDEX_ST_BDL_ST_MOMOL_e = 0x47,
    dRes_INDEX_ST_BDL_ST_MOMOR_e = 0x48,
    dRes_INDEX_ST_BDL_ST_MUNE_e = 0x49,
    dRes_INDEX_ST_BDL_ST_SUNEL_e = 0x4A,
    dRes_INDEX_ST_BDL_ST_SUNER_e = 0x4B,
    dRes_INDEX_ST_BDL_ST_UDEL_e = 0x4C,
    dRes_INDEX_ST_BDL_ST_UDER_e = 0x4D,
    dRes_INDEX_ST_BDL_ST_YUBI1L_e = 0x4E,
    dRes_INDEX_ST_BDL_ST_YUBI1R_e = 0x4F,
    dRes_INDEX_ST_BDL_ST_YUBI2L_e = 0x50,
    dRes_INDEX_ST_BDL_ST_YUBI2R_e = 0x51,
};
enum ST_JNT {
    ST_JNT_KOSI_e = 0x0,
    ST_JNT_HARA_e = 0x1,
    ST_JNT_MUNE_e = 0x2,
    ST_JNT_KATAL_e = 0x3,
    ST_JNT_UDEL_e = 0x4,
    ST_JNT_HANDL_e = 0x5,
    ST_JNT_YUBI1L_e = 0x6,
    ST_JNT_YUBI2L_e = 0x7,
    ST_JNT_KATAR_e = 0x8,
    ST_JNT_UDER_e = 0x9,
    ST_JNT_HANDR_e = 0xA,
    ST_JNT_BUKI_e = 0xB,
    ST_JNT_YUBI1R_e = 0xC,
    ST_JNT_YUBI2R_e = 0xD,
    ST_JNT_KUBI_e = 0xE,
    ST_JNT_HEAD_e = 0xF,
    ST_JNT_AGO_e = 0x10,
    ST_JNT_HAT_e = 0x11,
    ST_JNT_HIMO_e = 0x12,
    ST_JNT_KOTUBAN_e = 0x13,
    ST_JNT_MOMOL_e = 0x14,
    ST_JNT_SUNEL_e = 0x15,
    ST_JNT_ASIL_e = 0x16,
    ST_JNT_MOMOR_e = 0x17,
    ST_JNT_SUNER_e = 0x18,
    ST_JNT_ASIR_e = 0x19,
};

enum StAction {
    ST_ACTION_PATROL = 0,
    ST_ACTION_FIGHT = 5,
    ST_ACTION_GROUND_WAIT = 15,
    ST_ACTION_COFFIN_WAIT = 16,
    ST_ACTION_FIGHT_UNARMED = 29,
    ST_ACTION_BREAK_WAIT = 30,
    ST_ACTION_HEAD_DAMAGE = 31,
    ST_ACTION_UPPER_BODY = 32,
    ST_ACTION_LOWER_BODY = 33
};

enum BehaviorType {
    BEHAVIOR_NORMAL = 0,
    BEHAVIOR_UNDERGROUND = 1,
    BEHAVIOR_IN_HORIZONTAL_COFFIN = 2,
    BEHAVIOR_IN_VERTICAL_COFFIN = 3,
    BEHAVIOR_UPPER_BODY_ONLY = 14
};

namespace Stalfos {
struct HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 mPad01;
    /* 0x02 */ be<s16> mStepStartFrame1;  /* was m06: jyunkai (inlined in St_move 024972A4): from this walk frame to mStepEndFrame1 the Stalfos moves at mPatrolSpeed and turns, and on it the step sound 0x3832 plays; static_initialize 024960B4 9 (tww the same) */
    /* 0x04 */ be<s16> mStepStartFrame2;  /* was m08: jyunkai (inlined in St_move 024972A4): the second step window, from this walk frame to mStepEndFrame2, the step sound plays on it; static_initialize 024960B4 0x2C (tww the same) */
    /* 0x06 */ be<s16> mStepEndFrame1;  /* was m0A: jyunkai (inlined in St_move 024972A4): the last walk frame of the first step window; static_initialize 024960B4 0x15 (tww the same) */
    /* 0x08 */ be<s16> mStepEndFrame2;  /* was m0C: jyunkai (inlined in St_move 024972A4): the last walk frame of the second step window; static_initialize 024960B4 0x35 (tww the same) */
    /* 0x0A */ u8 mPad0A[2];
    /* 0x0C */ be<f32> mPatrolSpeed;  /* was m10: jyunkai (inlined in St_move 024972A4): mTargetSpeed inside the step windows (0 outside); static_initialize 024960B4 6.0 (tww m02DC = it) */
    /* 0x10 */ be<f32> m14;
    /* 0x14 */ be<s16> mCoffinWaitTime;  /* was m18: kan_wait (inlined in St_move 024972A4): after the ambush trigger mTimers[0] = it (mTimers[2] = it + 20); the coffin animation waits until it runs out; static_initialize 024960B4 0x46 (tww the same) */
    /* 0x16 */ be<s16> mReassembleTime;  /* was m1A: st_break_wait 024963A4: on entering the broken wait mTimers[0] and mReassembleWait = it; when the timer runs out the parts are called back (mPartCommand 2); static_initialize 024960B4 200 (tww the same) */
    /* 0x18 */ be<u32> __vtbl;
};
WWHD_SIZE(HIO_c, 0x1C);
} // namespace Stalfos

// HD: partial views retain the original accessor call and argument order.
struct StSortPacket {
    u8 mStorage[0x9C];
};
struct StGameInfoView {
    u8 mSave[0x12A0];
    u8 mBgS[0x1404];
    u8 mCcS[0x50AC - 0x26A4];
    u8 mNameTable[0x599C - 0x50AC];
    u8 mVibration[0x5AB0 - 0x599C];
    gptr<void> mpParticle;
    u8 mPadding5AB4[0x5B2C - 0x5AB4];
    gptr<void> mpPlayer;
    u8 mPadding5B30[0x5FB4 - 0x5B30];
    StSortPacket mSortPacket[1];
};
WWHD_OFFSET(StGameInfoView, mBgS, 0x12A0);
WWHD_OFFSET(StGameInfoView, mCcS, 0x26A4);
WWHD_OFFSET(StGameInfoView, mNameTable, 0x50AC);
WWHD_OFFSET(StGameInfoView, mVibration, 0x599C);
WWHD_OFFSET(StGameInfoView, mpParticle, 0x5AB0);
WWHD_OFFSET(StGameInfoView, mpPlayer, 0x5B2C);
WWHD_OFFSET(StGameInfoView, mSortPacket, 0x5FB4);
WWHD_SIZE(StGameInfoView, 0x6050);
struct StLinePoints {
    gptr<cXyz> mpPoints;
    u8 mPadding[0xC];
};
struct StLineMaterial {
    u8 mStorage[0x130];
    gptr<be<u32>> mpVtable;
    u8 mPadding134[0x10];
    gptr<StLinePoints> mpLine;
};
WWHD_OFFSET(StLineMaterial, mpVtable, 0x130);
WWHD_OFFSET(StLineMaterial, mpLine, 0x144);
WWHD_SIZE(StLineMaterial, 0x148);

struct StPartModelIndices {
    be<u16> values[26];
};
struct StCollisionJoints {
    be<s32> values[7];
};
WWHD_SIZE(StPartModelIndices, 0x34);
WWHD_SIZE(StCollisionJoints, 0x1C);

// HD: these views follow the verified render accesses; the node matrix owner is
// at model+0x2C.
struct StModelNodeData {
    u8 mPadding0[4];
    be<u16> mFlags;
    u8 mPadding6[10];
    gptr<Mtx34> mpAnmMtx;
};
WWHD_OFFSET(StModelNodeData, mpAnmMtx, 0x10);
WWHD_SIZE(StModelNodeData, 0x14);
struct StModel {
    u8 mPadding0[0x2C];
    gptr<StModelNodeData> mpNodeData;
    u8 mPadding30[0xBC - 0x30];
    cXyz mBaseScale;
    Mtx34 mBaseTransform;
};
WWHD_OFFSET(StModel, mBaseScale, 0xBC);
WWHD_OFFSET(StModel, mBaseTransform, 0xC8);
WWHD_SIZE(StModel, 0xF8);

// HD: the particle tail is expanded; use the offsets exercised by Stalfos's
// effects.
struct StEmitter {
    u8 mPadding0[8];
    cXyz mEmitterScale;
    u8 mPadding14[0x34 - 0x14];
    be<f32> mRate;
    u8 mPadding38[0x58 - 0x38];
    be<f32> mSpread;
    be<u32> mMaxFrame;
    u8 mPadding60[8];
    be<f32> mAwayFromCenterSpeed;
    u8 mPadding6C[0x1F0 - 0x6C];
    Mtx34 mGlobalRotation;
    cXyz mGlobalDynamicsScale, mGlobalTranslation, mGlobalParticleScale;
    u8 mPadding244[0x10];
    be<u32> mFlags;
};
WWHD_OFFSET(StEmitter, mRate, 0x34);
WWHD_OFFSET(StEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(StEmitter, mFlags, 0x254);
WWHD_SIZE(StEmitter, 0x258);
struct StPlayerView : fopAc_ac_c {
    be<u8> mCutType, mCutCount;
    u8 mPadding3AE[0x69E8 - 0x3AE];
    be<u8> mStrongJumpCut;  /* was mJumpCutExtraDamage (misnamed): view of daPy_lk_c::mStrongJumpCut (d_a_player_main.h), set by the player's spun-up jump cut; the extra spin damage / hit timer here is what it does in this reader */
};
WWHD_OFFSET(StPlayerView, mCutType, 0x3AC);
WWHD_OFFSET(StPlayerView, mStrongJumpCut, 0x69E8);
WWHD_SIZE(StPlayerView, 0x69E9);

struct StHeldWeapon : fopAc_ac_c {
    u8 mPadding3AC[8];
    gptr<StModel> mpModel;
};
WWHD_OFFSET(StHeldWeapon, mpModel, 0x3B4);
WWHD_SIZE(StHeldWeapon, 0x3B8);

struct StMorf {
    u8 mPadding[0x90];
    gptr<StModel> mpModel;
    be<u32> mpAnm;
    be<f32> mRate, mFrame;
    be<s16> mStart, mEnd, mLoop;
    be<u8> mAttribute, mState;
};
WWHD_OFFSET(StMorf, mFrame, 0x9C);
WWHD_OFFSET(StMorf, mState, 0xA7);
WWHD_SIZE(StMorf, 0xA8);

struct StSphereSource {
    be<u32> mFlags, mAtType;
    be<u8> mAtp;
    u8 mAtPadding[3];
    be<u32> mAtSPrm, mTgType, mTgSPrm, mCoSPrm;
    u8 mAtSe, mAtHitMark, mAtSpl, mAtMtrl;
    be<u32> mAtFlags;
    u8 mTgSe, mTgHitMark, mTgSpl, mTgMtrl;
    be<u32> mTgFlags, mCoFlags;
    be<f32> mCenter[3];
    be<f32> mRadius;
};
WWHD_SIZE(StSphereSource, 0x40);
struct StStaticState {
    be<f32> mMinusPi, mPi;
    u8 mAnotherHit, mByteGuard;
    be<u16> mWordGuard;
    be<u32> mRuntimeWords[4];
};
WWHD_SIZE(StStaticState, 0x1C);
struct StRuntimeRegistration {
    be<u32> mWords[3];
};
WWHD_SIZE(StRuntimeRegistration, 0xC);
struct StSinCosEntry {
    be<f32> mSin, mCos;
};
WWHD_SIZE(StSinCosEntry, 8);

struct st_p {
    /* 0x00 */ gptr<void> mpPartModel;
    /* 0x04 */ be<s8> mPartState;
    /* 0x05 */ be<s8> mDetached;  /* was m05: part_move 024925B8: 0 while the part follows its joint matrix, 1 while it moves on its own (switch at the end); scatter_part (inlined) and the break code set 1, the return to the joint sets 0 (tww the same) */
    /* 0x06 */ u8 pad06[2];
    /* 0x08 */ cXyz mPartPos;
    /* 0x14 */ cXyz mPartPrevPos;
    /* 0x20 */ cXyz mPartVelocity;
    /* 0x2C */ csXyz mPartRot;
    /* 0x32 */ csXyz mPartRotAdd;
    /* 0x38 */ be<s16> mFlyAngleX;  /* was m38: part_move 024925B8 state 1: the part's pitch toward its joint, eased by cLib_addCalcAngleS2 (step 0x2000) and used for the flight matrix; the head (joint 15) takes it as mPartRot.x (tww the same) */
    /* 0x3A */ be<s16> mFlyAngleY;  /* was m3A: part_move 024925B8 state 1: the part's yaw toward its joint, eased by cLib_addCalcAngleS2 and used for the flight matrix; the head takes it as mPartRot.y (tww the same) */
    /* 0x3C */ be<s16> mFlyAngleZ;  /* was m3C: part_move 024925B8 state 1: the head (joint 15) takes it as mPartRot.z with mFlyAngleX / mFlyAngleY; never written in WWHD or tww (tww the same read) */
    /* 0x3E */ be<s16> mWobbleCount;  /* was m3E: part_move 024925B8: counts up every frame (random start when a part breaks off); in state 1 the flight back wobbles by sin(it * 1000) / cos(it * 1100) / cos(it * 1200) (tww the same) */
    /* 0x40 */ be<s8> mWaitTimer;
    /* 0x41 */ u8 pad41[3];
    /* 0x44 */ be<f32> mFlySpeed;  /* was m44: part_move 024925B8 state 1: the flight step is it * 20 along the flight direction, eased toward 1 by 0.01; 0 or 0.5 when the part breaks off (tww the same) */
    /* 0x48 */ dPa_smokeEcallBack m48;
    /* 0x68 */ u8 pad68[4];
};
WWHD_SIZE(st_p, 0x6C);
struct st_ke_s {
    cXyz mPos[10];  /* was m000: ke_control 02491458: the positions of the 10 points of the ke chain; part_move 024925B8 puts point 0 at the part joint; ke_control moves points 1-9 after the previous one with mSpeed and keeps them above the ground (tww ke_control the same) */
    cXyz mSpeed[10];  /* was m078: ke_control 02491458: the per-point velocity of the ke chain, the damped difference of the new and old positions (tww ke_control the same) */
    csXyz mRootAngle;  /* was m0F0: part_move 024925B8 copies the part rotation into it and spreads the three chains by 0x800 in y (chain 1 + 0x500 in x); ke_control 02491458 turns the chain direction by it (tww the same) */
    u8 padF6[0x36];
};
WWHD_SIZE(st_ke_s, 0x12C);

struct st_nun_s {
    cXyz mPos[3];  /* was m00: nun_control 0249196C: the positions of the 3 nunchaku chain points; point 1 is placed at the hand joint, point 2 moves from it by the swung offset and is drawn there (tww nun_control the same) */
    csXyz mAngle[3];  /* was m24: nun_control 0249196C stores the pitch / yaw of point 2 as seen from point 1 in [1] and turns the nunchaku model by them (tww the same) */
    u8 pad36[2];
    cXyz mSpeed[3];  /* was m38: nun_control 0249196C: per-point velocity, the damped difference of the new and old position of point 2, added to the next step (tww the same; as st_ke_s::mSpeed) */
};
WWHD_SIZE(st_nun_s, 0x5C);

// HD: the ice effect retains the GameCube member layout.
struct StEnemyIce {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 mPadding;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ u8 mMotionState[0x30 - 0xC];
    /* 0x030 */ dCcD_Stts mStts;
    /* 0x06C */ dCcD_Cyl mCyl;
    /* 0x19C */ be<f32> mCylHeight;
    /* 0x1A0 */ be<f32> mWallRadius;
    /* 0x1A4 */ be<f32> mScaleXZ;
    /* 0x1A8 */ be<f32> mScaleY;
    /* 0x1AC */ be<f32> mParticleScale;
    /* 0x1B0 */ be<u8> mDeathState, mDeathSwitch;
    u8 mDeathPadding[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_OFFSET(StEnemyIce, mCyl, 0x6C);
WWHD_OFFSET(StEnemyIce, mParticleScale, 0x1AC);
WWHD_OFFSET(StEnemyIce, mBgAcch, 0x1F4);
WWHD_SIZE(StEnemyIce, 0x3B8);

struct st_class : fopEn_enemy_c {
    /* 0x03C8 */ request_of_phase_process_class mPhase;
    /* 0x03D0 */ be<u8> mBehaviorType;
    /* 0x03D1 */ be<u8> mAmbushSightRange;
    /* 0x03D2 */ be<u8> mUnusedParam;
    /* 0x03D3 */ be<u8> mAmbushSwitch;
    /* 0x03D4 */ be<u8> mDeathSwitch;
    /* 0x03D5 */ be<s8> mAmbushTrigger;  /* was m02B9: daSt_Create 02495C70 sets mAmbushSwitch + 1 (0xFF: by sight range) for the ambush behaviours 1-3; ambush_trigger (in ground_wait / kan_wait, St_move 024972A4) checks that switch (value - 1) or mAmbushSightRange; cleared when the Stalfos appears; daSt_Draw 02491F34 draws nothing while it is set and part_move 024925B8 moves the held weapon away */
    /* 0x03D6 */ u8 pad3D6[2];
    /* 0x03D8 */ gptr<void> mpMorf;
    /* 0x03DC */ gptr<void> mpMorf2;
    /* 0x03E0 */ be<s16> mMode;  /* was m02C4: step within mActionState: every action function of St_move 024972A4 (jyunkai, fight, fight2, ground_wait, kan_wait) and st_break_wait 024963A4, head_damage 02496718, ue_move 02496AA0, sita_move 02496F34 switch on it; damage_check (in daSt_Execute 02493FB0) sets 0 / 20 / 35 by bodyForm */
    /* 0x03E2 */ be<s16> mActionState;
    /* 0x03E4 */ be<s8> mFightBehavior;
    /* 0x03E5 */ u8 pad3E5;
    /* 0x03E6 */ be<s16> mSpinSeCounter;  /* was m02CA: fight (in St_move 024972A4): adds (5000 * abs(mSpinSpeed)) while spinning, plays SE 0x5895 each time it passes 100 (s16 wrap), 0 when not spinning */
    /* 0x03E8 */ be<s8> mLeftStance;  /* was m02CC: fight (in St_move 024972A4) sets 1 / 0 after the right / left attack ends; wait_set 0249210C and jyunkai pick dRes_INDEX_ST_BCK_LWAIT_e / LWALK_e (0x21 / 0x22) instead of WAIT / WALK and the left attack chains (mFightBehavior 5 / 15) while it is set */
    /* 0x03E9 */ u8 pad3E9[3];
    /* 0x03EC */ cXyz mPlayerPos;
    /* 0x03F8 */ be<f32> mTargetSpeed;  /* was m02DC: speed_pos_calc 024961B8 eases speedF to it (step mSpeedStep); the actions of St_move 024972A4, ue_move 02496AA0 and sita_move 02496F34 set it (0, l_HIO walk speed, 10, 20, -2) */
    /* 0x03FC */ be<f32> mSpeedStep;  /* was m02E0: speed_pos_calc 024961B8: step of cLib_addCalc2(speedF, mTargetSpeed, 1, it), reset to 1 every frame; jyunkai (in St_move 024972A4) sets 5 */
    /* 0x0400 */ be<s16> mTurnStep;  /* was m02E4: speed_pos_calc 024961B8: while on the ground (mAcch flag 0x20) and not 0, turns current.angle.y toward mPlayerPos by at most it, then clears it; jyunkai / fight (in St_move 024972A4), ue_move 02496AA0, sita_move 02496F34 set 0x200..0x2000 */
    /* 0x0402 */ u8 pad402[2];
    /* 0x0404 */ be<u32> mFrameCounter;  /* was m02E8: daSt_Execute 02493FB0 increments it every frame; phase of the shake / wobble sines (part_move 024925B8, fight in St_move 024972A4, nun_control 0249196C), cc_set's (in daSt_Execute) odd / even weapon sphere and nun_control's every-8th-frame check */
    /* 0x0408 */ be<s16> mTimers[5];
    /* 0x0412 */ be<s16> mNoHitTimer;  /* was m02F6: damage_check (in daSt_Execute 02493FB0) checks hits only while it is 0 and sets 5 / 2 after a hit; daSt_Execute counts it down; daSt_Create 02495C70 sets 15, the wait / unarmed actions keep it at 10 */
    /* 0x0414 */ be<s16> mDisappearTimer;  /* was m02F8: st_break_wait 024963A4 sets 499 when the scattered parts start to reassemble, counts it down and at 0 removes the Stalfos (fopAcM_createDisappear, fopAcM_delete, death switch); part_move 024925B8 sets 500 when it picks the dropped weapon back up, damage_check (in daSt_Execute 02493FB0) / sita_move 02496F34 clear it */
    /* 0x0416 */ be<s8> mDeleteRequest;  /* was m02FA: daSt_Execute 02493FB0 deletes the actor while it is set (with the 4000-unit fall check); daSt_Draw 02491F34 draws nothing; never set in d_a_st */
    /* 0x0417 */ be<u8> mHioOwner;  /* was m02FB: daSt_Create 02495C70 sets 1 when this actor created the l_HIO child (hio_set); daSt_Delete 024957B4 deletes the HIO child only then */
    /* 0x0418 */ csXyz mHeadAngleOffset;  /* was m02FC: part_move 024925B8: added to shape_angle (csXyz::operator+) for the attached head part 15's rotation; never written in d_a_st */
    /* 0x041E */ be<s16> mMoveAngleOffset;  /* was m0302: speed_pos_calc 024961B8 moves along current.angle.y + it; daSt_Execute 02493FB0 turns shape_angle.y toward the same and clears it; fight (in St_move 024972A4) sets a sine wobble of mFrameCounter */
    /* 0x0420 */ be<s16> mSpinAngleY;  /* was m0304: fight (in St_move 024972A4) adds 5000 * mSpinSpeed each frame; daSt_Execute 02493FB0 rotates the body model by shape_angle.y + it and eases it back to 0; the spin-end checks wait for it to be within +-0x400 */
    /* 0x0422 */ u8 pad422[2];
    /* 0x0424 */ be<f32> mSpinSpeed;  /* was m0308: fight (in St_move 024972A4) eases it to +-1 during the spin attacks (KAITEN*), 0 otherwise; drives mSpinAngleY, mSpinSeCounter, the spin smoke (spin_smoke_move in daSt_Execute 02493FB0) and cc_set's weapon sphere size / spl 7 above 0.5 */
    /* 0x0428 */ dBgS_AcchCir mAcchCir;
    /* 0x0468 */ dBgS_ObjAcch mAcch;
    /* 0x062C */ dCcD_Stts mStts;
    /* 0x0668 */ dCcD_Sph mBodySph[7];  /* was m054C: daSt_Create 02495C70 sets [0] from head_sph_src and [1..6] from body_sph_src; cc_set (in daSt_Execute 02493FB0) places them on the cc_j_no joints (radius 45 head / 25); damage_check checks their TG hits ([0] = head) */
    /* 0x0E9C */ dCcD_Sph mWeaponSph;  /* was m0D80: daSt_Create 02495C70 sets it from wepon_sph_src; cc_set (in daSt_Execute 02493FB0) places it on the weapon (ST_JNT_BUKI_e), the nunchaku arm or the foot (ST_JNT_ASIR_e) by mAtMode */
    /* 0x0FC8 */ cXyz mWeaponTipPos;  /* was m0EAC: St_move 024972A4: ST_JNT_BUKI_e joint + (160, 0, 0) every frame; the weapon drag smoke 0xA17D (mDragSmokeCallBack) is emitted there */
    /* 0x0FD4 */ cXyz mWeaponLinePos;  /* was m0EB8: fight (in St_move 024972A4): ST_JNT_BUKI_e joint + (80, 0, 0); a wall line check from mWeaponLinePosOld to it ends the fast spin attack */
    /* 0x0FE0 */ cXyz mWeaponLinePosOld;  /* was m0EC4: fight (in St_move 024972A4): the previous frame's mWeaponLinePos (copied before the update), start of the spin attack's wall line check */
    /* 0x0FEC */ be<s8> mTgMode;  /* was m0ED0: cc_set (in daSt_Execute 02493FB0): 0 all body spheres (mBodySph), 1 the head sphere only, 2 none; also keeps mWeaponSph off while not 0; reset to 0 every frame; fight2 / ground_wait / kan_wait (in St_move 024972A4) and st_break_wait 024963A4 set it */
    /* 0x0FED */ be<s8> mAtMode;  /* was m0ED1: cc_set (in daSt_Execute 02493FB0): 0 no weapon attack, 1 weapon, 2 spin (sphere scaled by mSpinSpeed), 3 kick (ST_JNT_ASIR_e); reset every frame; fight / fight2 (in St_move 024972A4) and sita_move 02496F34 set it */
    /* 0x0FEE */ be<s8> mPartsAttached;  /* was m0ED2: part_move 024925B8 counts down for each part that falls off and up for each part that gets back in place (not the weapon part 11); daSt_Create 02495C70 sets 25; st_break_wait 024963A4 ends the break once it is >= 25 */
    /* 0x0FEF */ be<s8> mPartCommand;  /* was m0ED3: part_move 024925B8: 1 scatters all parts, 10 drops the upper body parts (< 19), 20 the lower body parts, 2 / 11 sends the parts back; set by damage_check (in daSt_Execute 02493FB0), st_break_wait 024963A4, sita_move 02496F34, daSt_Create 02495C70; daSt_Execute clears it unless 2 */
    /* 0x0FF0 */ st_p mParts[26];
    /* 0x1AE8 */ st_ke_s mKe[3];
    /* 0x1E6C */ StLineMaterial mLineMat;
    /* 0x1FB4 */ st_nun_s mNun;
    /* 0x2010 */ be<s8> mNunMode;  /* was m1DC8: the pulled-off arm used as a nunchaku (mNun): fight2 (in St_move 024972A4) sets 1 after NUKU (pull off), 3 for HONE_ATTACK, 0 after TUKERU (put back); wait_set 0249210C sets 2 (HYUNHYUN); daSt_Execute 02493FB0 runs nun_control 0249196C and skips parts 3 / 4 while it is set; daSt_Draw 02491F34 hides parts 5-7 */
    /* 0x2011 */ u8 pad2011[3];
    /* 0x2014 */ be<u32> mHeldWeaponEntityId;
    /* 0x2018 */ be<s8> mWeaponState;  /* was m1DD0: part_move 024925B8: 1 (daSt_Create 02495C70) picks the weapon up (fopAcM_setCarryNow) and becomes 10 (held); 2 while the weapon part 11 is carried back with the reassembling parts, 0 when it lies on the ground (or the Stalfos is hit by light); jyunkai (in St_move 024972A4) fights armed at 10, unarmed (fight2) at 0 */
    /* 0x2019 */ u8 pad2019[3];
    /* 0x201C */ be<u32> mUpperBodyEntityId;
    /* 0x2020 */ be<s8> mHeadState;  /* was m1DD8: part_move 024925B8: 1 when the head part 15 is knocked off, 2 while it is drawn / moved with mpMorf2, 0 when it is back; daSt_Draw 02491F34, ke_control 02491458, damage_check / cc_set (in daSt_Execute 02493FB0) treat the loose head separately */
    /* 0x2021 */ u8 pad2021;
    /* 0x2022 */ be<s16> mHeadTargetAngleY;  /* was m1DDA: part_move 024925B8: target of the loose head part's rotation y (cLib_addCalcAngleS2), set to its angle when it falls off and when it turns */
    /* 0x2024 */ be<s16> mReassembleWait;  /* was m1DDC: st_break_wait 024963A4 sets l_HIO.mReassembleTime / 100; daSt_Execute 02493FB0 counts it down; part_move 024925B8 moves the loose parts back only when it is 0 */
    /* 0x2026 */ be<s16> mKnockbackAngleY;  /* was m1DDE: speed_pos_calc 024961B8: direction of the extra speed mKnockbackSpeed added to speed; never written in WWHD's d_a_st (zeldaret/tww d_a_st.cpp: only the VERSION_DEMO damage_check sets it to the hit angle atInfo.m0C.y) */
    /* 0x2028 */ be<f32> mKnockbackSpeed;  /* was m1DE0: speed_pos_calc 024961B8: while > 0.1 adds this speed in direction mKnockbackAngleY to speed, then eases it to 0 (cLib_addCalc0, step 2.5); never set in WWHD (zeldaret/tww d_a_st.cpp: the VERSION_DEMO damage_check sets 40 on a hit) */
    /* 0x202C */ be<s8> mShakeTimer;  /* was m1DE4: damage_check (in daSt_Execute 02493FB0) sets 5 / 8 / 10 / 20 on a hit and counts it down; part_move 024925B8 shakes the attached parts by it * 2 */
    /* 0x202D */ be<s8> m1DE5;
    /* 0x202E */ csXyz m1DE6[26];
    /* 0x20CA */ be<s16> mHeadShakeTimer;  /* was m1E82: damage_check (in daSt_Execute 02493FB0) sets 20 when the loose head is hit; part_move 024925B8 counts it down and shakes the head part 15 by it */
    /* 0x20CC */ be<s8> mBreakHealth;  /* was m1E84: daSt_Create 02495C70 sets 16 (5 for the upper body); damage_check (in daSt_Execute 02493FB0) subtracts the damage only for hammer (0xA8) / type-2 hits and breaks the Stalfos apart at <= 0; st_break_wait 024963A4 resets 16 after it reassembles */
    /* 0x20CD */ be<s8> mCountdownToDeath;
    /* 0x20CE */ be<s8> mUpperSeparate;  /* was m1E86: sita_move 02496F34 sets 1 while the separate upper-body actor (mUpperBodyEntityId) exists and 0 when it deletes it; part_move 024925B8 moves eyePos / attention to the head part only while it is 0 */
    /* 0x20CF */ be<s8> mAtStarted;  /* was m1E87: cc_set (in daSt_Execute 02493FB0): 0 -> dCcD_Sph::StartCAt and counts up, then MoveCAt; cleared when the weapon attack is off */
    /* 0x20D0 */ dPa_smokeEcallBack mSmokeCallBack[3];  /* was m1E88: smoke callbacks: [0] the appear smoke 0xA17C (ground_wait in St_move 024972A4), [1] the weapon ground smoke 0x2027 (buki_smoke_set in fight), [2] the spin smoke 0x2022 (smoke_set_s in daSt_Execute 02493FB0); actor_construct 02495AD0 / actor_destroy 0249A124 build / destroy the 3 */
    /* 0x2130 */ gptr<void> mBreakEmitter[20];  /* was m1EE8: eff_move (in daSt_Execute 02493FB0): up to 20 emitters 0x817B made every 15 frames while the Stalfos lies broken (mBreakEffMode 1), each stopped after mBreakEmitterTimer; daSt_Delete 024957B4 stops the rest */
    /* 0x2180 */ be<s16> mBreakEmitterTimer[20];  /* was m1F38: eff_move (in daSt_Execute 02493FB0): 200 per mBreakEmitter slot, counted down; at 0 the emitter is stopped and the slot freed */
    /* 0x21A8 */ be<s16> mBreakEffWait;  /* was m1F60: eff_move (in daSt_Execute 02493FB0): counted down; a new mBreakEmitter is made when it is 0, then 15 again */
    /* 0x21AA */ u8 pad21AA[2];
    /* 0x21AC */ be<f32> mBreakEffScale;  /* was m1F64: eff_move (in daSt_Execute 02493FB0): global particle scale of every mBreakEmitter; 1 while mBreakEffMode is 1, eased to 0 at 2 */
    /* 0x21B0 */ be<s8> mBreakEffMode;  /* was m1F68: st_break_wait 024963A4 sets 1 while the parts lie scattered and 2 when they are back; eff_move (in daSt_Execute 02493FB0) makes the 0x817B emitters at 1 and fades them at 2 */
    /* 0x21B1 */ u8 pad21B1[3];
    /* 0x21B4 */ cXyz mSpinSmokePos;  /* was m1F6C: spin_smoke_move (in daSt_Execute 02493FB0): body position + 175 along mSpinSmokeAngleY; smoke_set_s puts it on the ground (line check) and emits the smoke 0x2022 / 0x24 there */
    /* 0x21C0 */ csXyz mSpinSmokeAngle;  /* was m1F78: spin_smoke_move (in daSt_Execute 02493FB0): (0, mSpinSmokeAngleY, 0); angle of the spin smoke (smoke_set_s) */
    /* 0x21C6 */ be<s16> mSpinSmokeAngleY;  /* was m1F7E: fight (in St_move 024972A4) sets it from current.angle.y when a swing / spin starts; spin_smoke_move (in daSt_Execute 02493FB0) turns it by +-2000 or 5000 * mSpinSpeed each frame */
    /* 0x21C8 */ be<s16> mSpinSmokeTimer;  /* was m1F80: fight (in St_move 024972A4) sets 23 / 15; spin_smoke_move (in daSt_Execute 02493FB0) counts it down, makes smoke while it is >= 10 and ends mSmokeCallBack[2] at 0 */
    /* 0x21CA */ be<s8> mSpinSmokeDir;  /* was m1F82: fight (in St_move 024972A4): 0 right swing, 1 left swing, 2 spin; spin_smoke_move (in daSt_Execute 02493FB0) turns mSpinSmokeAngleY by +2000 / -2000 / 5000 * mSpinSpeed */
    /* 0x21CB */ be<s8> mSpinSmokeCount;  /* was m1F83: smoke_set_s (in daSt_Execute 02493FB0) emits the ground smoke only while it is 0 (or on attribute 4) and counts it up; spin_smoke_move clears it when the smoke ends */
    /* 0x21CC */ be<s8> mDragSmokeMode;  /* was m1F84: jyunkai (in St_move 024972A4) sets 1 when the walk starts and 2 while walking; St_move clears it every frame and, while the weapon is held (mWeaponState 10), emits the drag smoke 0xA17D at mWeaponTipPos at 1 / keeps it, else ends it */
    /* 0x21CD */ u8 pad21CD[3];
    /* 0x21D0 */ dPa_smokeEcallBack mDragSmokeCallBack;  /* was m1F88: St_move 024972A4: callback of the weapon drag smoke 0xA17D (emitted at mWeaponTipPos, ended when mDragSmokeMode is 0); actor_construct 02495AD0 builds it */
    /* 0x21F0 */ StEnemyIce mEnemyIce;
    /* 0x25A8 */ be<u32> mShadowId;
    /* 0x25AC */ be<s8> m2364;
    /* 0x25AD */ be<u8> bodyForm; // HD: distinguishes intact, upper and lower body updates.
    /* 0x25AE */ u8 pad25AE[2];
};
WWHD_OFFSET(st_class, mPhase, 0x3C8);
WWHD_OFFSET(st_class, mpMorf, 0x3D8);
WWHD_OFFSET(st_class, mAcchCir, 0x428);
WWHD_OFFSET(st_class, mParts, 0xFF0);
WWHD_OFFSET(st_class, mSmokeCallBack, 0x20D0);
WWHD_OFFSET(st_class, mShadowId, 0x25A8);
WWHD_SIZE(st_class, 0x25B0);

WWHD_OFFSET(st_class, mKe, 0x1AE8);
WWHD_OFFSET(st_class, mLineMat, 0x1E6C);
WWHD_OFFSET(st_class, mNun, 0x1FB4);

// HD: explicit outgoing linkage words preserve the original guest stack layout.
namespace Stalfos {
struct ResourceLocal {
    be<u32> linkage[2], text, vtable;
};

struct VectorLocal {
    be<u32> linkage[2];
    cXyz value;
};

struct LineCheckLocal {
    be<u32> linkage[2];
    dBgS_LinChk check;
};

struct IDLocal {
    be<u32> linkage[2], id;
};

struct AngleLocal {
    be<u32> linkage[2];
    csXyz value;
};

struct GroundCheckLocal {
    be<u32> linkage[2];
    dBgS_GndChk check;
};

struct AttackInfo {
    gptr<void> mpObj, mpActor;
    be<u8> mDamage, mbDead, mResultingAttackType, pad;
    csXyz mHitAngle;  /* was m0C: the CcAtInfo copy of this unit; see d/d_cc_uty.h */
    be<u16> mPlCutBit;
    gptr<cXyz> pParticlePos;
    be<s32> mHitSoundId;
};

struct AttackInfoLocal {
    be<u32> linkage[2];
    AttackInfo value;
};

} // namespace Stalfos

namespace StalfosHD {
inline be<u32> &stShapeVtable(dCcD_Cyl &cyl) {
    return *reinterpret_cast<be<u32> *>(&cyl.mShapeAttr[0x1C]);
}
inline be<u32> &stShapeAuxVtable(dCcD_Cyl &cyl) {
    return *reinterpret_cast<be<u32> *>(&cyl.mShapeAttr[0x18]);
}

template <class T> T *at(u32 p, u32 off = 0) { return gabi::at<T>(p + off); }
template <class T> T *member(st_class *i_this, u32 off) { return at<T>(gabi::ea(i_this), off); }

inline StSinCosEntry *stSinCosTable() { return gabi::at<StSinCosEntry>(0x104A44F8); }

inline StPlayerView &stPlayer(u32 address) { return *gabi::at<StPlayerView>(address); }

inline StEmitter &stEmitter(u32 address) { return *gabi::at<StEmitter>(address); }
inline fopAc_ac_c &stActor(u32 address) { return *gabi::at<fopAc_ac_c>(address); }
inline StHeldWeapon &stHeldWeapon(u32 address) { return *gabi::at<StHeldWeapon>(address); }
inline dCcD_Sph &stSphere(u32 address) { return *gabi::at<dCcD_Sph>(address); }
inline be<u32> &stFloatWord(be<f32> &value) { return *reinterpret_cast<be<u32> *>(&value); }

inline StMorf &stMorf(u32 address) { return *gabi::at<StMorf>(address); }
inline StModel &stModel(u32 address) { return *gabi::at<StModel>(address); }
inline StModelNodeData &stNodeData(u32 address) { return *gabi::at<StModelNodeData>(address); }
inline gptr<Mtx34> &stCalcMatrix() { return *gabi::at<gptr<Mtx34>>(0x1018C7B0); }
inline be<u32> &stSaveContext() { return *gabi::at<be<u32>>(0x101F84DC); }
inline be<u32> &stResourceControl() { return *gabi::at<be<u32>>(0x101F4F28); }

// HD: GHS nonleaf callees save LR at callerSP+4, even without stack arguments.
// Preserve this outgoing frame for every call, including its allocation order.
struct OutgoingLinkage {
    be<u32> backchain, savedLR;
};
template <class R = void, class... A> R callWithLinkage(u32 target, A... args) {
    gabi::Local<OutgoingLinkage> frame;
    frame->backchain = frame.a + frame.kSize;
    return gabi::call<R>(target, args...);
}
template <class R = void, class... A> R callVirtualWithLinkage(u32 target, A... args) {
    gabi::Local<OutgoingLinkage> frame;
    frame->backchain = frame.a + frame.kSize;
    return gabi::call_ptr<R>(target, args...);
}

// 02008860 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cBgS_LineCross(A... args) {
    return callWithLinkage<R>(0x02008860, args...);
}
// 02008974 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cBgS_GroundCross(A... args) {
    return callWithLinkage<R>(0x02008974, args...);
}
// 02008B4C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cBgS_LinChk_destroy(A... args) {
    return callWithLinkage<R>(0x02008B4C, args...);
}
// 02008DAC [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cBgS_Chk_destroy_cBgS_Chk(A... args) {
    return callWithLinkage<R>(0x02008DAC, args...);
}
// 02008E0C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cBgS_GndChk_cBgS_GndChk(A... args) {
    return callWithLinkage<R>(0x02008E0C, args...);
}
// 02008FEC [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cBgS_LinChk_ct(A... args) {
    return callWithLinkage<R>(0x02008FEC, args...);
}
// 0200BD2C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cCcD_Stts_construct(A... args) {
    return callWithLinkage<R>(0x0200BD2C, args...);
}
// 0200E240 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cCcS_Set(A... args) {
    return callWithLinkage<R>(0x0200E240, args...);
}
// 0200E814 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cDT_NamePTbl_GetIndex(A... args) {
    return callWithLinkage<R>(0x0200E814, args...);
}
// 0200ED84 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cLib_addCalc2(A... args) {
    return callWithLinkage<R>(0x0200ED84, args...);
}
// 0200EDC8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cLib_addCalc0(A... args) {
    return callWithLinkage<R>(0x0200EDC8, args...);
}
// 0200F428 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cLib_addCalcAngleS2(A... args) {
    return callWithLinkage<R>(0x0200F428, args...);
}
// 0200FAD8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R MtxTrans(A... args) {
    return callWithLinkage<R>(0x0200FAD8, args...);
}
// 0200FC74 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R MtxScale(A... args) {
    return callWithLinkage<R>(0x0200FC74, args...);
}
// 0200FCD8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R MtxPosition(A... args) {
    return callWithLinkage<R>(0x0200FCD8, args...);
}
// 02018034 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cM3dGCir_destroy(A... args) {
    return callWithLinkage<R>(0x02018034, args...);
}
// 02018590 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cM3dGCyl_construct(A... args) {
    return callWithLinkage<R>(0x02018590, args...);
}
// 02018C8C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cM3dGSph_SetR(A... args) {
    return callWithLinkage<R>(0x02018C8C, args...);
}
// 02018D40 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cM3dGSph_SetC(A... args) {
    return callWithLinkage<R>(0x02018D40, args...);
}
// 020195B0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cM_atan2s(A... args) {
    return callWithLinkage<R>(0x020195B0, args...);
}
// 020198D8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cM_rndF(A... args) {
    return callWithLinkage<R>(0x020198D8, args...);
}
// 02019918 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cM_rndFX(A... args) {
    return callWithLinkage<R>(0x02019918, args...);
}
// 0201A478 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R csXyz_csXyz(A... args) {
    return callWithLinkage<R>(0x0201A478, args...);
}
// 0201A4DC [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R csXyz___pl(A... args) {
    return callWithLinkage<R>(0x0201A4DC, args...);
}
// 0201A554 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R csXyz___apl(A... args) {
    return callWithLinkage<R>(0x0201A554, args...);
}
// 0201AD78 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cXyz___pl(A... args) {
    return callWithLinkage<R>(0x0201AD78, args...);
}
// 0201ADE0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cXyz___mi(A... args) {
    return callWithLinkage<R>(0x0201ADE0, args...);
}
// 020402C8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R enemy_ice(A... args) {
    return callWithLinkage<R>(0x020402C8, args...);
}
// 02493FB0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R native_daSt_Execute(A... args) {
    return callWithLinkage<R>(0x02493FB0, args...);
}
// 024EECAC [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dBgS_GetMtrlSndId(A... args) {
    return callWithLinkage<R>(0x024EECAC, args...);
}
// 024EF0F4 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dBgS_GetAttributeCode(A... args) {
    return callWithLinkage<R>(0x024EF0F4, args...);
}
// 024EFD9C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dBgS_Acch_destroy(A... args) {
    return callWithLinkage<R>(0x024EFD9C, args...);
}
// 024EFE94 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dBgS_AcchCir_dBgS_AcchCir(A... args) {
    return callWithLinkage<R>(0x024EFE94, args...);
}
// 024EFF44 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dBgS_AcchCir_SetWall(A... args) {
    return callWithLinkage<R>(0x024EFF44, args...);
}
// 024F0474 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dBgS_Acch_dBgS_Acch(A... args) {
    return callWithLinkage<R>(0x024F0474, args...);
}
// 024F06B4 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dBgS_Acch_Set(A... args) {
    return callWithLinkage<R>(0x024F06B4, args...);
}
// 024F08A8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dBgS_Acch_CrrPos(A... args) {
    return callWithLinkage<R>(0x024F08A8, args...);
}
// 024F1AFC [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dBgS_LinChk_Set(A... args) {
    return callWithLinkage<R>(0x024F1AFC, args...);
}
// 02515860 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_Stts_destroy(A... args) {
    return callWithLinkage<R>(0x02515860, args...);
}
// 02515A70 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_Cyl_destroy(A... args) {
    return callWithLinkage<R>(0x02515A70, args...);
}
// 02515AE8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_Sph_destroy(A... args) {
    return callWithLinkage<R>(0x02515AE8, args...);
}
// 02515DA0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_GStts_dCcD_GStts(A... args) {
    return callWithLinkage<R>(0x02515DA0, args...);
}
// 02515E50 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_GStts_Move(A... args) {
    return callWithLinkage<R>(0x02515E50, args...);
}
// 02515F14 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_Stts_Init(A... args) {
    return callWithLinkage<R>(0x02515F14, args...);
}
// 02515FB8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_GObjInf_dCcD_GObjInf(A... args) {
    return callWithLinkage<R>(0x02515FB8, args...);
}
// 025162A4 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_GObjInf_ChkTgHit(A... args) {
    return callWithLinkage<R>(0x025162A4, args...);
}
// 02516300 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_GObjInf_GetTgHitObj(A... args) {
    return callWithLinkage<R>(0x02516300, args...);
}
// 025166F0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_Sph_construct(A... args) {
    return callWithLinkage<R>(0x025166F0, args...);
}
// 0251677C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_Sph_Set(A... args) {
    return callWithLinkage<R>(0x0251677C, args...);
}
// 025167C0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_Sph_StartCAt(A... args) {
    return callWithLinkage<R>(0x025167C0, args...);
}
// 025167E4 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dCcD_Sph_MoveCAt(A... args) {
    return callWithLinkage<R>(0x025167E4, args...);
}
// 02518CC8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R def_se_set(A... args) {
    return callWithLinkage<R>(0x02518CC8, args...);
}
// 02518DB0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R at_power_check(A... args) {
    return callWithLinkage<R>(0x02518DB0, args...);
}
// 025192A8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R cc_at_check(A... args) {
    return callWithLinkage<R>(0x025192A8, args...);
}
// 025200D4 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R stGetGameInfoWithLinkage(A... args) {
    return callWithLinkage<R>(0x025200D4, args...);
}
// 02520460 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dComIfG_resLoad(A... args) {
    return callWithLinkage<R>(0x02520460, args...);
}
// 025204C8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dComIfG_resDelete(A... args) {
    return callWithLinkage<R>(0x025204C8, args...);
}
// 02520540 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R stGetReverbWithLinkage(A... args) {
    return callWithLinkage<R>(0x02520540, args...);
}
// 02555D0C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dKy_getEnvlight(A... args) {
    return callWithLinkage<R>(0x02555D0C, args...);
}
// 025626A4 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dScnKy_env_light_c_settingTevStruct(A... args) {
    return callWithLinkage<R>(0x025626A4, args...);
}
// 02562F5C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dScnKy_env_light_c_setLightTevColorType(A... args) {
    return callWithLinkage<R>(0x02562F5C, args...);
}
// 025A5B18 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dPa_smokeEcallBack_dPa_smokeEcallBack(A... args) {
    return callWithLinkage<R>(0x025A5B18, args...);
}
// 025A5F88 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dPa_smokeEcallBack_end(A... args) {
    return callWithLinkage<R>(0x025A5F88, args...);
}
// 025A847C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dPa_control_c_set(A... args) {
    return callWithLinkage<R>(0x025A847C, args...);
}
// 025B9E38 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dSv_info_c_onSwitch(A... args) {
    return callWithLinkage<R>(0x025B9E38, args...);
}
// 025BA0C0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dSv_info_c_isSwitch(A... args) {
    return callWithLinkage<R>(0x025BA0C0, args...);
}
// 025BA5D4 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dSv_info_c_onActor(A... args) {
    return callWithLinkage<R>(0x025BA5D4, args...);
}
// 025BED80 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dSnap_RegistFig(A... args) {
    return callWithLinkage<R>(0x025BED80, args...);
}
// 025CB374 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dVibration_c_StartShock(A... args) {
    return callWithLinkage<R>(0x025CB374, args...);
}
// 025D4ED0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAc_ac_c_fopAc_ac_c(A... args) {
    return callWithLinkage<R>(0x025D4ED0, args...);
}
// 025D50BC [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAc_ac_c_destroy_fopAc_ac_c(A... args) {
    return callWithLinkage<R>(0x025D50BC, args...);
}
// 025D5218 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAcIt_Judge(A... args) {
    return callWithLinkage<R>(0x025D5218, args...);
}
// 025D57E0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAcM_delete(A... args) {
    return callWithLinkage<R>(0x025D57E0, args...);
}
// 025D5834 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAcM_create(A... args) {
    return callWithLinkage<R>(0x025D5834, args...);
}
// 025D63E8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAcM_entrySolidHeap(A... args) {
    return callWithLinkage<R>(0x025D63E8, args...);
}
// 025D6894 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAcM_searchActorAngleY(A... args) {
    return callWithLinkage<R>(0x025D6894, args...);
}
// 025D68EC [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAcM_searchActorDistance(A... args) {
    return callWithLinkage<R>(0x025D68EC, args...);
}
// 025D9000 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAcM_createStealItem(A... args) {
    return callWithLinkage<R>(0x025D9000, args...);
}
// 025D99E8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAcM_createDisappear(A... args) {
    return callWithLinkage<R>(0x025D99E8, args...);
}
// 025D9D0C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAcM_setCarryNow(A... args) {
    return callWithLinkage<R>(0x025D9D0C, args...);
}
// 025D9D24 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R fopAcM_cancelCarryNow(A... args) {
    return callWithLinkage<R>(0x025D9D24, args...);
}
// 025E1A40 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoAud_seStart(A... args) {
    return callWithLinkage<R>(0x025E1A40, args...);
}
// 025E1AA4 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoAud_monsSeStart(A... args) {
    return callWithLinkage<R>(0x025E1AA4, args...);
}
// 025E1D30 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoAud_bgmHitSound(A... args) {
    return callWithLinkage<R>(0x025E1D30, args...);
}
// 025E1FD8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoAud_onEnemyDamage(A... args) {
    return callWithLinkage<R>(0x025E1FD8, args...);
}
// 025E2DE0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_modelUpdateDL(A... args) {
    return callWithLinkage<R>(0x025E2DE0, args...);
}
// 025E38E0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_J3DModel__create(A... args) {
    return callWithLinkage<R>(0x025E38E0, args...);
}
// 025E4A98 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_McaMorf_setAnm(A... args) {
    return callWithLinkage<R>(0x025E4A98, args...);
}
// 025E4F64 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_McaMorf_mDoExt_McaMorf(A... args) {
    return callWithLinkage<R>(0x025E4F64, args...);
}
// 025E535C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_McaMorf_play(A... args) {
    return callWithLinkage<R>(0x025E535C, args...);
}
// 025E5590 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_McaMorf_entryDL(A... args) {
    return callWithLinkage<R>(0x025E5590, args...);
}
// 025E55A0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_McaMorf_calc(A... args) {
    return callWithLinkage<R>(0x025E55A0, args...);
}
// 025E9960 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_3DlineMat0_c_construct(A... args) {
    return callWithLinkage<R>(0x025E9960, args...);
}
// 025E99E0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_3DlineMat0_c_destroy(A... args) {
    return callWithLinkage<R>(0x025E99E0, args...);
}
// 025E9B80 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_3DlineMat0_c_init(A... args) {
    return callWithLinkage<R>(0x025E9B80, args...);
}
// 025EA548 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_3DlineMat0_c_update(A... args) {
    return callWithLinkage<R>(0x025EA548, args...);
}
// 025EDD04 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoExt_3DlineMatSortPacket_setMat(A... args) {
    return callWithLinkage<R>(0x025EDD04, args...);
}
// 025F0A10 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoHIO_subRoot_c_createChild(A... args) {
    return callWithLinkage<R>(0x025F0A10, args...);
}
// 025F0A18 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoHIO_subRoot_c_deleteChild(A... args) {
    return callWithLinkage<R>(0x025F0A18, args...);
}
// 025F1884 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoMtx_YrotS(A... args) {
    return callWithLinkage<R>(0x025F1884, args...);
}
// 025F18EC [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoMtx_XrotS(A... args) {
    return callWithLinkage<R>(0x025F18EC, args...);
}
// 025F1BF4 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoMtx_XrotM(A... args) {
    return callWithLinkage<R>(0x025F1BF4, args...);
}
// 025F1C28 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoMtx_YrotM(A... args) {
    return callWithLinkage<R>(0x025F1C28, args...);
}
// 025F1C5C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R mDoMtx_ZrotM(A... args) {
    return callWithLinkage<R>(0x025F1C5C, args...);
}
// 026066C4 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R dRes_control_c_getRes(A... args) {
    return callWithLinkage<R>(0x026066C4, args...);
}
// 0273AD10 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R operator_new(A... args) {
    return callWithLinkage<R>(0x0273AD10, args...);
}
// 0273AF40 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R __dl(A... args) {
    return callWithLinkage<R>(0x0273AF40, args...);
}
// 028249B0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R JPASetRMtxTVecfromMtx(A... args) {
    return callWithLinkage<R>(0x028249B0, args...);
}
// 028E8D88 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R PSVECAdd(A... args) {
    return callWithLinkage<R>(0x028E8D88, args...);
}
// 028E8DD0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R PSVECSquareMag(A... args) {
    return callWithLinkage<R>(0x028E8DD0, args...);
}
// 028E90D4 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R PSMTXCopy(A... args) {
    return callWithLinkage<R>(0x028E90D4, args...);
}
// 028EAB2C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R runtime_initializeGuardWord(A... args) {
    return callWithLinkage<R>(0x028EAB2C, args...);
}
// 028ED6F8 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R runtime_initializeGuardByte(A... args) {
    return callWithLinkage<R>(0x028ED6F8, args...);
}
// 028EFFD0 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R __construct_array(A... args) {
    return callWithLinkage<R>(0x028EFFD0, args...);
}
// 028F0164 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R __destroy_arr(A... args) {
    return callWithLinkage<R>(0x028F0164, args...);
}
// 028F026C [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R __register_global_object(A... args) {
    return callWithLinkage<R>(0x028F026C, args...);
}
// 028F4384 [v d_a_st]; argument and result widths retain the verified call
// site.
template <class R = void, class... A> R sqrtHD(A... args) {
    return callWithLinkage<R>(0x028F4384, args...);
}

// Debug HIO banks keep the GameCube REGn_F/REGn_S spelling.
inline f32 REG0_F(int index) { return *gabi::at<be<f32>>(0x1047B610 + 4 * index); }
inline s16 REG0_S(int index) { return *gabi::at<be<s16>>(0x1047B688 + 2 * index); }
inline f32 REG6_F(int index) { return *gabi::at<be<f32>>(0x1047B970 + 4 * index); }
inline s16 REG6_S(int index) { return *gabi::at<be<s16>>(0x1047B9E8 + 2 * index); }
inline f32 REG8_F(int index) { return *gabi::at<be<f32>>(0x1047BA90 + 4 * index); }
inline s16 REG8_S(int index) { return *gabi::at<be<s16>>(0x1047BB08 + 2 * index); }
inline f32 REG13_F(int index) { return *gabi::at<be<f32>>(0x1047BD60 + 4 * index); }
} // namespace StalfosHD

namespace Stalfos {
u32 word(u32 p);
u32 resource(gabi::Local<ResourceLocal> &local, const void *name, u32 id);
void anm_init(st_class *i_this, s32 animation, f32 morph, u32 loop, f32 speed, s32 sound);
void head_anm_init(st_class *i_this, s32 animation, f32 morph, u32 loop, f32 speed, s32 sound);
void wait_set(st_class *i_this);
void attack_set(st_class *i_this, s32 animation, f32 morph, u32 loop);
BOOL daSt_IsDelete(st_class *i_this);
BOOL daSt_Delete(st_class *i_this);
u32 smoke_construct(void *callback);
void hio_destroy(void *self, u32 flags);
void smoke_destroy(void *self, u32 flags);
void part_destroy(st_p *self, u32 flags);
st_p *part_construct(st_p *part);
st_class *actor_construct(st_class *i_this);
void actor_destroy(st_class *i_this, u32 flags);
void hio_message(void *self, void *context);
void static_initialize();
BOOL createHeap(st_class *i_this);
BOOL daSt_Draw(st_class *i_this);
void speed_pos_calc(st_class *i_this);
cPhs_State daSt_Create(st_class *i_this);
void initialize_line(gabi::Local<LineCheckLocal> &local);
void finish_line(gabi::Local<LineCheckLocal> &local);
bool line_cross(gabi::Local<LineCheckLocal> &local, cXyz *start, cXyz *end, st_class *i_this);
f32 part_posmove(st_class *i_this, st_p *part);
void sound_at(st_class *i_this, u32 sound, cXyz *position);
void st_break_wait(st_class *i_this);
void head_effect(st_class *i_this, gabi::Local<VectorLocal> &zero, gabi::Local<VectorLocal> &effect,
                 gabi::Local<AngleLocal> &angle);
void head_damage(st_class *i_this);
void copyVectorWords(u32 destination, u32 source);
u32 player_pointer();
s16 player_angle(st_class *i_this);
f64 player_distance(st_class *i_this);
bool animation_stopped(st_class *i_this);
void ue_move(st_class *i_this);
void sita_move(st_class *i_this);
void ke_control(st_class *i_this);
void copy_model_matrix(u32 model, u32 matrix);
void nun_control(st_class *i_this);
u32 find_actor(u32 id);
f64 random_positive(f32 range);
f64 random_signed(f32 range);
f32 angle_sine(s32 angle);
f32 angle_cosine(s32 angle);
f64 magnitude(cXyz *vector);
u32 animation_matrix(u32 animation, s32 joint);
void joint_matrix(st_class *i_this, s32 joint);
u32 particle(u32 id, cXyz *position);
void scatter_part(st_p *part);
void part_move(st_class *i_this, s32 joint);
void sound_at_level(st_class *i_this, u32 sound, u32 level);
void monster_sound(st_class *i_this, u32 sound, u32 level);
u32 particle_with_angle(u32 id, cXyz *position, csXyz *angle);
u32 toon_particle(st_class *i_this, u32 id, cXyz *position, csXyz *angle, void *callback);
void buki_smoke_set(st_class *i_this);
void jyunkai(st_class *i_this);
bool ambush_trigger(st_class *i_this);
void ground_wait(st_class *i_this);
void kan_wait(st_class *i_this);
void fight(st_class *i_this);
void fight2(st_class *i_this);
void St_move(st_class *i_this);
void damage_check(st_class *i_this);
void collision_register(u32 sphere);
void cc_set(st_class *i_this);
void eff_move(st_class *i_this);
void smoke_set_s(st_class *i_this, f32 rate);
void spin_smoke_move(st_class *i_this);
void move_visible_parts(st_class *i_this);
s32 daSt_Execute(st_class *i_this);
} // namespace Stalfos
