// Main Moblin TU layout and bindings; sibling legacy interfaces remain unchanged.
/* mo2_class (Moblin), WWHD layout.
 *
 * GameCube -> WWHD (offsets measured from the verified functions and the class constructor
 * 021CC91C, which allocates 0x343C; GameCube 0x3038):
 *  - +0x11C up to mpMorf (fopEn_enemy_c);
 *  - mDoExt_btpAnm is 0x74 (GameCube 0x14);
 *  - mShadowHandle (GameCube 0x2E4) is gone (HD shadows): +0x178 from mLanternId;
 *  - mDoExt_3DlineMat0_c is 0x148 (GameCube 0x1C);
 *  - a new float after mTargetDist (HD: |dy| to the target), so +0x2A8 from mPathTargetPos;
 *  - damagereaction is 0x7F0 (GameCube 0x7BC): its joint arrays have 21 entries, the gap after mJnt12LineBlocked 8 bytes;
 *  - +0x2DC from m0DD0, +0x408 after the second 3DlineMat0, +0x404 from the ga_s array;
 *  - m2A50 (J3DMaterialTable*) is gone; enemyice at 0x2E54; enemyfire is 0x22C. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/tu.h"

void Mo2GroundCheck_ctor(void*);
void Mo2BackgroundCheck_dtor(void*, s32);
void Mo2MonstersSound_start(u32, cXyz*, u32, u32, s32);
void Mo2LinePacket_add(u32, void*);
s32 Mo2CharacterTable_getNameIndex2(void*, const char*, s32);
s32 Mo2CharacterTable_getNameIndex(void*, const char*, s32);
void Mo2ActorCheck_ctor(void*);

// HD: exact guest views for the player state used by this TU.
struct Mo2PlayerView {
    u8 _00[0x3CC];
    be<f32> mGrabWear;
    u8 _3D0[0x420 - 0x3D0];
    be<s16> mDemoType;
    u8 _422[6];
    be<u32> mDemoParam0;
    u8 _42C[4];
    be<u32> mDemoMode;
};
WWHD_OFFSET(Mo2PlayerView, mDemoType, 0x420);
WWHD_OFFSET(Mo2PlayerView, mDemoMode, 0x430);
struct Mo2EmitterView {
    u8 _00[0x34];
    be<f32> mRate;
    u8 _38[0x58 - 0x38];
    be<f32> mSpread;
    be<s32> mMaxFrame;
    u8 _60[0x220 - 0x60];
    cXyz mGlobalDynamicsScale;
    u8 _22C[0xC];
    cXyz mGlobalParticleScale;
};
WWHD_SIZE(Mo2EmitterView, 0x244);
WWHD_OFFSET(Mo2EmitterView, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(Mo2EmitterView, mGlobalParticleScale, 0x238);

struct Mo2AcchCtorView {
    u8 _00[0x10];
    be<u32> mPolyVtable;
    be<u32> mAcchVtable;
    be<u8> mAcchFlags;
    u8 _19[7];
    be<u32> mCheckVtable;
};
WWHD_OFFSET(Mo2AcchCtorView, mAcchFlags, 0x18);
WWHD_OFFSET(Mo2AcchCtorView, mCheckVtable, 0x20);

struct Mo2VirtualTable {
    be<u32> mFunctions[18];
};
struct Mo2VirtualObject {
    gptr<Mo2VirtualTable> mpVtable;
};

WWHD_OPAQUE(JntHit_c);
/* J3DMaterial: JSystem/J3DGraph.h */
struct mDoExt_btpAnm_l {
    u8 _[0x74];
}; /* HD size (constructor 025E7820) */
struct Mo2HairLine;
struct mDoExt_3DlineMat0_l {
    u8 _00[0x130];
    gptr<Mo2VirtualTable> mpVtable;
    u8 _134[0x10];
    gptr<Mo2HairLine> mpLines;
}; /* HD size (constructor 025E9960) */
struct dPa_smokeEcallBack_l {
    u8 _[0x20];
};
struct LIGHT_INFLUENCE_l {
    u8 _[0x20];
    be<f32> mFluctuation;
}; /* HD: 0x24 */
struct JntHit_HIO_l {
    u8 _[0x2C];
};

struct ke_s {
    /* 0x000 */ cXyz mPointPos[10];  /* was m000: ke_control 021C6F14: the 10 spear-hair points, each placed 21.875 from the previous one (floor-clamped); nodeCallBack_P 021C6BE0 sets [0] on joint MO_JNT_MO_YARI_e; ke_disp (in daMo2_Draw 021C7344) copies them into the 3D line (tww the same; after our decomp's tt_ke_s mPos) */
    /* 0x078 */ cXyz mPointVelocity[10];  /* was m078: ke_control 021C6F14: holds the old x / z, then (new - old) * damping, added to the next step's point (tww the same; after our decomp's tt_ke_s mVelocity) */
    /* 0x0F0 */ u8 m0F0[0x124 - 0x0F0];
};
WWHD_SIZE(ke_s, 0x124);

struct ga_s {
    /* 0x00 */ gptr<J3DModel> mpModel;
    /* 0x04 */ cXyz mPos;
    /* 0x10 */ cXyz m10;
    /* 0x1C */ be<s16> m1C;
    /* 0x1E */ be<s16> m1E;
    /* 0x20 */ u8 m20[4];
    /* 0x24 */ be<f32> m24;
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<s16> m2C;
    /* 0x2E */ be<u8> m2E;
    /* 0x2F */ be<u8> m2F;
};
WWHD_SIZE(ga_s, 0x30);

struct damagereaction_l {
    /* 0x0000 */ gptr<fopEn_enemy_c> mpEnemy;
    /* 0x0004 */ be<s16> mMode;
    /* 0x0006 */ be<s16> mAction;
    /* 0x0008 */ be<s16> mEnemyType;
    /* 0x000A */ u8 _000A[0x2];
    /* 0x000C */ be<s32> mTimer;
    /* 0x0010 */ csXyz mJntTargetAngle[21];
    /* 0x008E */ csXyz mJntAngle[21];
    /* 0x010C */ cXyz mJntPos[21];
    /* 0x0208 */ cXyz mJntGroundPos[21];
    /* 0x0304 */ cXyz m2E0[21];
    /* 0x0400 */ be<s32> mJntGroundState[14];
    /* 0x0438 */ be<s32> mJnt10LineBlocked;
    /* 0x043C */ be<s32> mJnt11LineBlocked;
    /* 0x0440 */ be<s32> mJnt8LineBlocked;
    /* 0x0444 */ be<s32> mJnt9LineBlocked;
    /* 0x0448 */ be<s32> mJnt12LineBlocked;
    /* 0x044C */ u8 _044C[0x8];
    /* 0x0454 */ be<s32> mJntBgCheckTimer;
    /* 0x0458 */ be<s32> mHitFlags;
    /* 0x045C */ be<f32> mKnockbackPower;
    /* 0x0460 */ cXyz mHitVec;
    /* 0x046C */ be<s32> m438;
    /* 0x0470 */ u8 _0470[0x4];
    /* 0x0474 */ be<s16> mJnt10Pitch;
    /* 0x0476 */ be<s16> mJnt11Pitch;
    /* 0x0478 */ be<s16> mJnt8Pitch;
    /* 0x047A */ be<s16> mJnt9Pitch;
    /* 0x047C */ be<s16> mJnt12Pitch;
    /* 0x047E */ u8 _047E[0x2];
    /* 0x0480 */ cXyz mPivot;
    /* 0x048C */ cXyz mBodyOffset;
    /* 0x0498 */ u8 _0498[0x4];
    /* 0x049C */ be<f32> mCarryOffsetY;
    /* 0x04A0 */ be<f32> mCarryOffsetZ;
    /* 0x04A4 */ be<f32> mFlinchBend;
    /* 0x04A8 */ be<f32> mShakeAmplitude;
    /* 0x04AC */ be<f32> mKnockbackSpeed;
    /* 0x04B0 */ be<s32> mLaunched;
    /* 0x04B4 */ be<s16> mFallPitch;
    /* 0x04B6 */ be<s16> mKnockbackAngleY;
    /* 0x04B8 */ be<s16> mSpinAngleY;
    /* 0x04BA */ be<s16> mFallPitchTarget;
    /* 0x04BC */ be<s16> mKnockedDown;
    /* 0x04BE */ be<s16> mHitStunTimer;
    /* 0x04C0 */ csXyz mSlopeTilt;
    /* 0x04C6 */ be<s16> mJnt12SlopeY;
    /* 0x04C8 */ be<s16> mJnt12SlopeZ;
    /* 0x04CA */ be<s16> mJnt18AddY;
    /* 0x04CC */ be<s16> mJnt13AddY;
    /* 0x04CE */ be<s16> mJnt18AddYTarget;
    /* 0x04D0 */ be<s16> mJnt13AddYTarget;
    /* 0x04D2 */ be<s16> mParalysisTimer;
    /* 0x04D4 */ be<s16> mJnt0AddZ;
    /* 0x04D6 */ be<s16> mJnt1AddZ;
    /* 0x04D8 */ be<s16> mJnt2AddZ;
    /* 0x04DA */ be<s16> mJnt3AddZ;
    /* 0x04DC */ be<s16> mJnt0AddZTarget;
    /* 0x04DE */ be<s16> mJnt1AddZTarget;
    /* 0x04E0 */ be<s16> mJnt2AddZTarget;
    /* 0x04E2 */ be<s16> mJnt3AddZTarget;
    /* 0x04E4 */ be<s16> mJnt2LandZ;
    /* 0x04E6 */ be<s16> mJnt3LandZ;
    /* 0x04E8 */ be<s16> mJnt6LandZ;
    /* 0x04EA */ be<s16> mJnt7LandZ;
    /* 0x04EC */ be<s16> mJnt6AddY;
    /* 0x04EE */ be<s16> mJnt7AddY;
    /* 0x04F0 */ be<s16> mJnt4AddY;
    /* 0x04F2 */ be<s16> mJnt5AddY;
    /* 0x04F4 */ be<s16> mLandAngleY;
    /* 0x04F6 */ u8 _04F6[0x6];
    /* 0x04FC */ be<s16> mDownTimers[3];
    /* 0x0502 */ be<s16> mInvincibleTimer;
    /* 0x0504 */ be<s16> mTurnTargetY;
    /* 0x0506 */ u8 _0506[0x2];
    /* 0x0508 */ be<f32> mPushback;
    /* 0x050C */ u8 _050C[0x4];
    /* 0x0510 */ dBgS_AcchCir mAcchCir;
    /* 0x0550 */ dBgS_ObjAcch mAcch;
    /* 0x0714 */ be<s32> mSpinLeft;
    /* 0x0718 */ be<f32> mSpawnY;
    /* 0x071C */ cXyz mPrevGroundPos;
    /* 0x0728 */ cXyz m6F4[2];
    /* 0x0740 */ be<u8> mBounceCount;
    /* 0x0741 */ be<u8> mFaceDown;
    /* 0x0742 */ be<u8> mShakeJntMask;
    /* 0x0743 */ u8 _0743[0x1];
    /* 0x0744 */ be<u8> mLookMode;
    /* 0x0745 */ be<u8> mDefencePose;
    /* 0x0746 */ be<u8> m712;
    /* 0x0747 */ be<u8> mPlayerInView;
    /* 0x0748 */ gptr<fopAc_ac_c> mpLookTarget;
    /* 0x074C */ be<s16> mLookPitch;
    /* 0x074E */ be<s16> mLookYaw;
    /* 0x0750 */ u8 _0750[0x2];
    /* 0x0752 */ be<s16> mBodyBgCheckDelay;
    /* 0x0754 */ be<f32> mMaxFallDistance;
    /* 0x0758 */ dCcD_Stts mStts;
    /* 0x0794 */ cXyz mParticlePos;
    /* 0x07A0 */ csXyz mParticleAngle;
    /* 0x07A6 */ u8 _07A6[0x2];
    /* 0x07A8 */ dPa_smokeEcallBack_l mParticleCallBack;
    /* 0x07C8 */ be<s16> mWallSmokeTimer;
    /* 0x07CA */ u8 _07CA[0x2];
    /* 0x07CC */ be<f32> mHangMoveSpeed;
    /* 0x07D0 */ gptr<cXyz> mpHangPos;
    /* 0x07D4 */ cXyz mHangPos;
    /* 0x07E0 */ csXyz mHangAngle;
    /* 0x07E6 */ be<s16> mBridgeRideTimer;
    /* 0x07E8 */ be<s16> mBridgeRideRoll;
    /* 0x07EA */ be<u8> mSkipMoveOnce;
    /* 0x07EB */ u8 _07EB[0x1];
    /* 0x07EC */ be<u32> mHangBridgeId;
};
WWHD_SIZE(damagereaction_l, 0x7F0);
WWHD_OFFSET(damagereaction_l, mJntGroundState, 0x400);
WWHD_OFFSET(damagereaction_l, mJnt10LineBlocked, 0x438);
struct enemyice_l {
    /* 0x0000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x0004 */ be<s16> mFreezeDuration;
    /* 0x0006 */ be<s8> mLightShrinkTimer;
    /* 0x0007 */ u8 _0007[0x1];
    /* 0x0008 */ be<f32> mYOffset;
    /* 0x000C */ be<s8> mFreezeType;  /* was m00C: view of enemyice::mFreezeType (c/c_damagereaction.h) */
    /* 0x000D */ be<s8> mMode;
    /* 0x000E */ be<s16> mFreezeTimer;
    /* 0x0010 */ be<s16> mMoveDelayTimer;
    /* 0x0012 */ be<s16> mAngleY;
    /* 0x0014 */ be<s16> mAngularVelY;
    /* 0x0016 */ u8 _0016[0x2];
    /* 0x0018 */ cXyz mSpeed;
    /* 0x0024 */ be<f32> mSpeedF;
    /* 0x0028 */ be<f32> mBodyLiftY;  /* was m028: view of enemyice::mBodyLiftY (c/c_damagereaction.h) */
    /* 0x002C */ be<f32> mIceOffsetY;  /* was m02C: view of enemyice::mIceOffsetY (c/c_damagereaction.h) */
    /* 0x0030 */ dCcD_Stts mStts;
    /* 0x006C */ dCcD_Cyl mCyl;
    /* 0x019C */ be<f32> mCylHeight;
    /* 0x01A0 */ be<f32> mWallRadius;
    /* 0x01A4 */ be<f32> mScaleXZ;
    /* 0x01A8 */ be<f32> mScaleY;
    /* 0x01AC */ be<f32> mParticleScale;
    /* 0x01B0 */ be<u8> m1B0;
    /* 0x01B1 */ be<u8> mDeathSwitch;
    /* 0x01B2 */ u8 _01B2[0x2];
    /* 0x01B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x01F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(enemyice_l, 0x3B8);
struct enemyfire_l {
    /* 0x0000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x0004 */ be<s16> mFireDuration;
    /* 0x0006 */ be<s8> mMode;
    /* 0x0007 */ u8 _0007[0x1];
    /* 0x0008 */ be<s16> mFireTimer;
    /* 0x000A */ u8 _000A[0x2];
    /* 0x000C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x0010 */ be<s8> mFlameJntIdxs[10];
    /* 0x001A */ u8 _001A[0x2];
    /* 0x001C */ be<f32> mParticleScale[10];
    /* 0x0044 */ be<s16> mFlameTimers[10];
    /* 0x0058 */ gptr<JPABaseEmitter> mpFlameEmitters[10];
    /* 0x0080 */ cXyz mPrevPos;
    /* 0x008C */ cXyz mDirection;
    /* 0x0098 */ be<f32> mFlameScaleY;
    /* 0x009C */ u8 _009C[0x1];
    /* 0x009D */ be<u8> mHitboxFlameIdx;
    /* 0x009E */ u8 _009E[0x2];
    /* 0x00A0 */ dCcD_Stts mStts;
    /* 0x00DC */ dCcD_Sph mSph;
    /* 0x0208 */ LIGHT_INFLUENCE_l mLight;
};
WWHD_SIZE(enemyfire_l, 0x22C);

struct mo2_class : fopEn_enemy_c {
    /* 0x03C8 */ request_of_phase_process_class mPhsMo2;
    /* 0x03D0 */ request_of_phase_process_class mPhsSpear;
    /* 0x03D8 */ be<u8> mMode;
    /* 0x03D9 */ be<u8> mFrozenInTimePose;
    /* 0x03DA */ be<u8> mPathIndex;
    /* 0x03DB */ be<u8> mEnableSpawnSwitch;
    /* 0x03DC */ be<u8> mDeathSwitch;
    /* 0x03DD */ be<u8> mAppearWait;  /* was m02C1: daMo2_Create 021CCB00: mEnableSpawnSwitch + 1 (0 = none; cleared for the statue mode 0xF); daMo2_Execute 021C9B1C waits until that switch is on, then clears it and sets the lock-on flag; daMo2_Draw 021C7344 draws nothing while it is set */
    /* 0x03DE */ u8 _03DE[0x2];
    /* 0x03E0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x03E4 */ mDoExt_btpAnm_l mEyeBtpAnm;  /* was m02C8: tex_anm_set 021C6368: mDoExt_btpAnm_init with the BTP mo2_tex_anm_idx[idx]; createHeap 021CC524 inits it with BTP 3; daMo2_Draw 021C7344 enters it at mEyeBtpFrame; stepped by mo2_eye_tex_anm (in daMo2_Execute 021C9B1C) */
    /* 0x0458 */ be<u8> mEyeBtpFrame;  /* was m02DC: tex_anm_set 021C6368 clears it; mo2_eye_tex_anm (in daMo2_Execute 021C9B1C) adds 1 up to mEyeBtpMaxFrame; daMo2_Draw 021C7344 enters mEyeBtpAnm at this frame */
    /* 0x0459 */ be<u8> mEyeBtpPlaying;  /* was m02DD: tex_anm_set 021C6368 sets 1; mo2_eye_tex_anm (in daMo2_Execute 021C9B1C) clears it at mEyeBtpMaxFrame and starts BTP 3 only while it is 0 */
    /* 0x045A */ be<u8> mEyeBtpMaxFrame;  /* was m02DE: tex_anm_set 021C6368: mo2_tex_max_frame[idx]; mo2_eye_tex_anm (in daMo2_Execute 021C9B1C) stops the eye BTP there */
    /* 0x045B */ u8 _045B[0x1];
    /* 0x045C */ be<s16> mEyeAnmWait;  /* was m02E0: mo2_eye_tex_anm (in daMo2_Execute 021C9B1C) counts it down, then reloads rnd(50)+10 and starts BTP 3 unless one is playing; nage 021CD958 / p_lost 021CDE6C / defence 021CE46C / Mo2_move 021D0A14 hold it at 2 */
    /* 0x045E */ be<s16> mWeaponViewWait;  /* was m02E2: wepon_search 021CF0F4 sets 70 when it gives up the run to the weapon; daMo2_Execute 021C9B1C counts it down; daMo2_wepon_view_check 021C872C finds no weapon while it is not 0 */
    /* 0x0460 */ be<u32> mLanternId;  /* was m02E8: kantera_get_init (in daMo2_Create 021CCB00): fpcSCtRq_Request(fpcNm_KANTERA_e) result, then fopAcM_SearchByID(it) to give the lantern this actor's id */
    /* 0x0464 */ u8 _0464[0x284];
    /* 0x06E8 */ mDoExt_3DlineMat0_l m0570;
    /* 0x0830 */ gptr<J3DModel> mpBlurModel;  /* was m058C: createHeap 021CC524: mDoExt_J3DModel__create of the spear-trail BMD (HD index 0x7E); br_draw (in daMo2_Draw 021C7344) places it on joint MO_JNT_MO_YARI_e and draws it */
    /* 0x0834 */ be<f32> mBlurScaleX;  /* was m0590: fight (in Mo2_move 021D0A14): moveHIO mAttack1BlurScaleX[] / mAttack2BlurScaleX[] by the attack frame; br_draw (in daMo2_Draw 021C7344) scales the blur model x by l_mo2HIO.mBlurScale * it */
    /* 0x0838 */ be<s32> mBlurOn;  /* was m0594: daMo2_Execute 021C9B1C clears it each frame; fight (in Mo2_move 021D0A14) sets 1 inside the swing frames of attack kinds 1 / 2; br_draw (in daMo2_Draw 021C7344) draws only while it is set */
    /* 0x083C */ be<s32> mBlurUseB;  /* was m0598: daMo2_Execute 021C9B1C clears it each frame; fight (in Mo2_move 021D0A14) sets 1 for the middle swing frames; br_draw (in daMo2_Draw 021C7344) shows the blur model's shape 2 instead of 1 when set */
    /* 0x0840 */ be<s32> mFrameCounter;  /* was m059C: daMo2_Execute 021C9B1C adds 1 per frame; p_lost 021CDE6C / yogan_fail 021CEEB0 / hip_damage 021CF900 / ground_smoke_set (in daMo2_Execute) / fight_run (in Mo2_move 021D0A14) test its low bits */
    /* 0x0844 */ be<s32> mOshiCount;  /* was m05A0: fight_run (in Mo2_move 021D0A14) adds 1 per frame while the armed Moblin is close and sees the target, at 15 starts ACTION_OSHI (push) with 50 %, else clears it; damage_check (in daMo2_Execute 021C9B1C) clears it on a hit */
    /* 0x0848 */ be<s16> mTimers[5];  /* was m05A4: daMo2_Execute 021C9B1C counts all 5 down each frame; the actions load [0]..[4] (e.g. wait_set 021C8E30 [1], nage 021CD958 [0]/[1]/[2], attack_set 021C9310 [4]) */
    /* 0x0852 */ be<s16> mWeaponRunTimer;  /* was m05AE: wepon_search 021CF0F4 sets l_mo2HIO.mWeaponRunTime when the run to a weapon starts and gives up at 0; yari_off_check (in daMo2_Execute 021C9B1C) sets it when it spawns the dropped weapon actor; daMo2_Execute counts it down; jyunkai (in Mo2_move 021D0A14) sets mWeaponSearchGaveUp at 0 */
    /* 0x0854 */ be<s16> mDashStopTimer;  /* was m05B0: fight_run (in Mo2_move 021D0A14) sets rnd(10)+15 when a dash longer than 20 frames stops (BCK GAKEDEMO) and waits for it; anm_init 021C6440 sets no new animation while it is not 0; daMo2_Execute 021C9B1C counts it down and keeps smoke type 4 going */
    /* 0x0856 */ be<s16> mAnmStopTimer;  /* was m05B2: wepon_search 021CF0F4 sets 5 when it sees a weapon and turns only once it is 0; daMo2_Execute 021C9B1C counts it down and plays mpMorf only while it is 0 */
    /* 0x0858 */ be<s16> mNoHitTimer;  /* was m05B4: damage_check (in daMo2_Execute 021C9B1C) takes a Tg hit only while it is 0 and reloads it (4, 10, 0x32, 200); fail 021CED78 / yogan_fail 021CEEB0 / d_sit 021CFFA4 / d_dozou 021D0064 / e3_demo 021D0570 / Mo2_move 021D0A14 hold it at 5; daMo2_Execute moves the head sphere 20000 down while it is not 0 */
    /* 0x085A */ be<s16> mNoGuardTimer;  /* was m05B6: damage_check (in daMo2_Execute 021C9B1C) sets 25 after a hit; daMo2_Execute counts it down; fight_run (in Mo2_move 021D0A14) guards (ACTION_DEFENCE) only while it is 0 */
    /* 0x085C */ u8 _085C[0x2];
    /* 0x085E */ be<s16> mSideStepTimer;  /* was m05BA: fight_run (in Mo2_move 021D0A14) sets 0x10 / 0x20 when a side step starts (and fight on a bounce); side-step reaction (dr.mLookMode 3/4) while it is not 0; daMo2_Execute 021C9B1C counts it down */
    /* 0x0860 */ be<s16> mSideStepWait;  /* was m05BC: fight_run (in Mo2_move 021D0A14) reloads rnd(100)+30 when it is 0 and then picks the next side step; daMo2_Execute 021C9B1C counts it down */
    /* 0x0862 */ be<u8> mSideStepDir;  /* was m05BE: fight_run (in Mo2_move 021D0A14): 0 / 1 / 2 by mGroundCheck 4 / 8 / 2 when a side step starts; selects dr.mLookMode 3 / 4 while mSideStepTimer runs */
    /* 0x0863 */ be<u8> mGroundCheck;  /* was m05BF: fight_run (in Mo2_move 021D0A14): ground_4_check 021C7EDC result (4 directions); 2 / 4 / 8 select the side step and the move-angle offset */
    /* 0x0864 */ be<f32> mTargetDist;  /* was m05C0: Mo2_move 021D0A14: xz distance to the target (dr.mpLookTarget or mSoundTargetPos); nage 021CD958 / hukki 021CE6E8 / fight_run (in Mo2_move) compare it with the l_mo2HIO attack ranges */
    /* 0x0868 */ be<f32> mTargetDistY;  /* was m05C0_dy_hd: Mo2_move 021D0A14: |dy| to the target; fight_run (in Mo2_move) also needs it below REG_F(10,3) + 250 before an attack; HD: |dy| to the target (Mo2_move) */
    /* 0x086C */ cXyz mPathTargetPos;  /* was m05C4: jyunkai (in Mo2_move 021D0A14) copies the current path point there (way_pos_check 021C7C30 adjusts it) and walks toward it; path_check2 (in Mo2_move) line-checks to it */
    /* 0x0878 */ u8 m05D0[4];
    /* 0x087C */ be<s16> mHeadAngleY;  /* was m05D4: nodeCallBack 021C6854: atan2 of the face joint (MO_JNT_KAO_J_e) direction (or angle.y); search_bomb 021C8378 / daMo2_player_view_check 021C8C14 measure the sight cone from it */
    /* 0x087E */ be<s16> mTargetAngleY;  /* was m05D6: Mo2_move 021D0A14: cM_atan2s to the target; nage 021CD958 / fight_run (in Mo2_move) / carry_drop turn to it */
    /* 0x0880 */ be<s16> mMoveAngleOffset;  /* was m05D8: fight_run (in Mo2_move 021D0A14) sets +-0x4000 for side steps; Mo2_move 021D0A14 clears it and moves along angle.y + it */
    /* 0x0882 */ be<s16> mSpitTimer;  /* was m05DA: damage_check / daMo2_Execute 021C9B1C damage cases set 1 / 3 / 5 / 10; daMo2_Execute counts it down and at 0 sets particle ID_AK_JN_TUBA00 at mSpitPos */
    /* 0x0884 */ cXyz mSmokePos;  /* was m05DC: ground_smoke_set (in daMo2_Execute 021C9B1C) computes it (MtxPosition); smoke_set_s 021C6578 drops it to the ground and sets the ELEMENTSMOKE particle there */
    /* 0x0890 */ csXyz mEffectAngle;  /* was m05E8: smoke_set_s 021C6578 / ground_smoke_set (in daMo2_Execute 021C9B1C): smoke particle angle; daMo2_Execute / yogan_fail 021CEEB0: TUBA00 particle angle */
    /* 0x0896 */ be<s16> mSmokeAngleY;  /* was m05EE: ground_smoke_set (in daMo2_Execute 021C9B1C) rotates the smoke offset by it and adds 2000 / 0x1FA0 / 0x2000 per call; fight (in Mo2_move 021D0A14) seeds it from angle.y */
    /* 0x0898 */ be<s16> mSmokeTimer;  /* was m05F0: ground_smoke_set (in daMo2_Execute 021C9B1C) counts it down and sets smoke while it is >= l_mo2HIO.mSmokeTimerBase; b_nige 021CE138 / hukki 021CE6E8 / wepon_search 021CF0F4 / fight load mSmokeTimerBase + n */
    /* 0x089A */ be<u8> mSmokeType;  /* was m05F2: ground_smoke_set (in daMo2_Execute 021C9B1C): switch 0..4 selects where the smoke comes from; b_nige 021CE138 sets 4, hukki 021CE6E8 3, daMo2_Execute 1 */
    /* 0x089B */ be<u8> mSmokeCount;  /* was m05F3: ground_smoke_set (in daMo2_Execute 021C9B1C) clears it; smoke_set_s 021C6578 adds 1 per smoke set and tests it with the ground attribute */
    /* 0x089C */ dPa_smokeEcallBack_l mSmokeCallBack;  /* was m05F4: mo2_class_ct 021CC91C: dPa_smokeEcallBack_ct; smoke_set_s 021C6578 passes it to the smoke emitter; daMo2_Delete 021CC44C removes it */
    /* 0x08BC */ damagereaction_l mDamageReaction;
    /* 0x10AC */ u8 _10AC[0x4];
    /* 0x10B0 */ be<s8> mbHasInnateWeapon;
    /* 0x10B1 */ be<s8> mSpawnWeaponActor;
    /* 0x10B2 */ be<s8> mSpawnWeaponActorMode;
    /* 0x10B3 */ be<s8> mbThrowWeapon;
    /* 0x10B4 */ ke_s mSpearHair[16];  /* was m0DD8: nodeCallBack_P 021C6BE0 places the 16 strand roots on joint MO_JNT_MO_YARI_e (spear); ke_control 021C6F14 / ke_disp (in daMo2_Draw 021C7344) move and draw the strands (ke_s, tww d217e2b ke_control / ke_disp) */
    /* 0x22F4 */ mDoExt_3DlineMat0_l m3Dline;
    /* 0x243C */ cXyz mSpearJointPos;  /* was m2034: nodeCallBack_P 021C6BE0: origin of joint MO_JNT_MO_YARI_e (joint_check 0x14, the spear joint, as for mSpearShaftPos); never read */
    /* 0x2448 */ cXyz mSpearEndPos[2];  /* was m2040: nodeCallBack_P 021C6BE0: [0] point x 165, [1] point x -130 on joint MO_JNT_MO_YARI_e (the spear's two ends, beside mSpearShaftPos at x -50); never read */
    /* 0x2460 */ u8 _2460[0x8];
    /* 0x2468 */ be<s32> mAttackKind;  /* was m2060: attack_set 021C9310 sets 0..5; indexes attack_info / mo2_attack_ready_SE and the move attack tables (fight, yari_hit_check in Mo2_move 021D0A14) */
    /* 0x246C */ be<s32> mAttackStep;  /* was m2064: attack_set 021C9310 clears it; fight (in Mo2_move 021D0A14) steps +1 / -1 through attack_info[mAttackKind][it] animations; hukki 021CE6E8 sets 1 */
    /* 0x2470 */ be<f32> mAttackFrame;  /* was m2068: attack_set 021C9310 clears it; fight (in Mo2_move 021D0A14) adds the step speed; compared with mAtStartFrame / mAtEndFrame, the blur frames and mTurnEndFrame */
    /* 0x2474 */ be<f32> mAtStartFrame;  /* was m206C: attack_set 021C9310 sets 3..36 per kind; yari_hit_check (in Mo2_move 021D0A14): no At before it, attack SE at it */
    /* 0x2478 */ be<f32> mAtEndFrame;  /* was m2070: attack_set 021C9310 sets 8..52 per kind; yari_hit_check (in Mo2_move 021D0A14): no At after it */
    /* 0x247C */ be<f32> mLookStartFrame;  /* was m2074: attack_set 021C9310 sets 0 / 10 / 59 / 75 per attack kind (hukki 021CE6E8 0, wepon_search 021CF0F4 75); fight (in Mo2_move 021D0A14) sets mDamageReaction.mLookMode = 1 (head toward mpLookTarget) once mAttackFrame is past it and the target is in view */
    /* 0x2480 */ be<f32> mTurnEndFrame;  /* was m2078: attack_set 021C9310: l_mo2HIO mAttack0TurnEndFrame..mAttack5TurnEndFrame per kind; fight (in Mo2_move 021D0A14) keeps turning to mTargetAngleY while mAttackFrame is below it */
    /* 0x2484 */ be<s16> mAtWaitTimer;  /* was m207C: yari_hit_check (in Mo2_move 021D0A14) counts it down and does no spear hit check while it runs; nothing sets it (same in tww d217e2b) */
    /* 0x2486 */ be<s8> mAttackDir;  /* was m207E: attack_set 021C9310 sets 1; fight (in Mo2_move 021D0A14) sets -1 when the player guards (plays back) and steps by it; yari_hit_check has no At while < 0 */
    /* 0x2487 */ u8 _2487[0xD];
    /* 0x2494 */ dCcD_Cyl mCoCyl;
    /* 0x25C4 */ dCcD_Cyl mTgCyl;
    /* 0x26F4 */ dCcD_Sph mHeadSph;
    /* 0x2820 */ dCcD_Sph mDefenseSph;
    /* 0x294C */ dCcD_Sph mWeaponSph;
    /* 0x2A78 */ dCcD_Sph mWeapon2Sph;
    /* 0x2BA4 */ dCcD_Sph m279C;
    /* 0x2CD0 */ cXyz mSpitPos;  /* was m28C8: nodeCallBack 021C6854: point (20.75, 18.5) on the face joint MO_JNT_KAO_J_e; daMo2_Execute 021C9B1C / yogan_fail 021CEEB0 set particle ID_AK_JN_TUBA00 there */
    /* 0x2CDC */ cXyz mSpearHeadPos;  /* was m28D4: nodeCallBack_P 021C6BE0 copies the first mSpearHair root (x 98 on MO_JNT_MO_YARI_e); yari_hit_check (in Mo2_move 021D0A14) uses it as mAtPos for spear kinds 0..2; fight plays LANCE_HIT_FLOOR there */
    /* 0x2CE8 */ cXyz mSpearShaftPos;  /* was m28E0: nodeCallBack_P 021C6BE0: point x -50 on joint MO_JNT_MO_YARI_e; yari_hit_check (in Mo2_move 021D0A14) copies it to mAt2Pos, defence 021CE46C to mDefenseSphPos */
    /* 0x2CF4 */ cXyz mLeftHandPos;  /* was m28EC: nodeCallBack_P 021C6BE0: point (17.5, -8.75) on joint MO_JNT_TEL_J_e (left hand); yari_hit_check (in Mo2_move 021D0A14) uses it as mAtPos for kinds 3 / 4 */
    /* 0x2D00 */ cXyz mRightHandPos;  /* was m28F8: nodeCallBack_P 021C6BE0: origin of joint MO_JNT_TER_J_e (right hand); yari_hit_check (in Mo2_move 021D0A14) uses it as mAtPos for kind 5 */
    /* 0x2D0C */ cXyz mAtPos;  /* was m2904: yari_hit_check (in Mo2_move 021D0A14): mRightHandPos / mLeftHandPos / mSpearHeadPos by mAttackKind; mWeaponSph starts / moves there; ground_smoke_set (in daMo2_Execute 021C9B1C) */
    /* 0x2D18 */ cXyz mAt2Pos;  /* was m2910: yari_hit_check (in Mo2_move 021D0A14): mSpearShaftPos for the spear kinds; mWeapon2Sph starts / moves there */
    /* 0x2D24 */ cXyz mPrevAtPos;  /* was m291C: yari_hit_check (in Mo2_move 021D0A14) copies mAtPos here before it picks the new one; never read */
    /* 0x2D30 */ cXyz mDefenseSphPos;  /* was m2928: defence 021CE46C copies mSpearShaftPos; damage_check (in daMo2_Execute 021C9B1C) centres mDefenseSph on it, sets the KIKUZU particle there and moves it to y -10000 */
    /* 0x2D3C */ cXyz mClangEffectPos;  /* was m2934: fight (in Mo2_move 021D0A14) sets particle ID_AK_JN_NG and starts JA_SE_LK_ROPE_HOOK_METAL at it on the spear-bounce branch (m2940 != 0); daMo2_Delete 021CC44C deletes its sound object; never written (tww the same) */
    /* 0x2D48 */ be<u8> m2940;
    /* 0x2D49 */ be<u8> mAtStarted;  /* was m2941: attack_set 021C9310 clears it; yari_hit_check (in Mo2_move 021D0A14) starts the At spheres while it is 0 and sets it, then only moves them */
    /* 0x2D4A */ be<u8> mTurnAlways;  /* was m2942: hukki 021CE6E8 sets 1 when it attacks out of getting up, attack_set 021C9310 clears it; fight (in Mo2_move 021D0A14) keeps turning to the target while it is set */
    /* 0x2D4B */ be<u8> mWeaponSearchGaveUp;  /* was m2943: wepon_search 021CF0F4 sets 1 when the run to a weapon ends at a cliff / wall / timeout and clears it at the start; jyunkai (in Mo2_move 021D0A14) sets it when mWeaponRunTimer ran out; has_weapon / wait_set 021C8E30 / path_check 021C9004 treat it as armed */
    /* 0x2D4C */ be<s16> mBtCounter;  /* was m2944: attack_set 021C9310 clears it for attack kinds 1 / 2 / 5; daMo2_Execute 021C9B1C adds 1 per frame up to 200 (then -1 = off) and opens the parry window at mBtWindow1Start / mBtWindow2Start; damage_check sets -1 */
    /* 0x2D4E */ be<s16> mBtWindow1Start;  /* was m2946: attack_set 021C9310: l_mo2HIO mAttack1BtWindow1Start / mAttack2BtWindow1Start / mAttack5BtWindow1Start; daMo2_Execute 021C9B1C loads mBtWindowTimer with mBtWindow1Len when mBtCounter reaches it */
    /* 0x2D50 */ be<s16> mBtWindow1Len;  /* was m2948: attack_set 021C9310: l_mo2HIO mAttack1BtWindow1Len / mAttack2BtWindow1Len / mAttack5BtWindow1Len; daMo2_Execute 021C9B1C copies it to mBtWindowTimer at mBtWindow1Start */
    /* 0x2D52 */ be<s16> mBtWindow2Start;  /* was m294A: attack_set 021C9310: l_mo2HIO mAttack1BtWindow2Start / mAttack2BtWindow2Start / mAttack5BtWindow2Start; daMo2_Execute 021C9B1C loads mBtWindowTimer with mBtWindow2Len when mBtCounter reaches it */
    /* 0x2D54 */ be<s16> mBtWindow2Len;  /* was m294C: attack_set 021C9310: l_mo2HIO mAttack1BtWindow2Len / mAttack2BtWindow2Len / mAttack5BtWindow2Len; daMo2_Execute 021C9B1C copies it to mBtWindowTimer at mBtWindow2Start */
    /* 0x2D56 */ be<s16> mBtWindowTimer;  /* was m294E: daMo2_Execute 021C9B1C counts it down and sets mBtNowFrame 5 (inside the parry window) while it is not 0, else 100; damage_check clears it */
    /* 0x2D58 */ be<u8> mParryOpeningType;
    /* 0x2D59 */ be<s8> mItemStolen;  /* was m2951: damage_check (in daMo2_Execute 021C9B1C) sets 1 at the first steal hit (type 0xE) and starts ACTION_P_LOST; daMo2_Draw 021C7344 hides the mo_e1 shape while it is set */
    /* 0x2D5A */ be<s16> mJawAngleZ;  /* was m2952: nodeCallBack 021C6854: ZrotM of joint MO_JNT_JAWA_J_e by it; aite_miru 021CEC44 / fight_run (in Mo2_move 021D0A14) ease it to 12000, Mo2_move 021D0A14 back to 0 */
    /* 0x2D5C */ be<u32> mHitActorId;  /* was m2954: attack_set 021C9310 clears it; fight (in Mo2_move 021D0A14) stores the id of a Moblin its attack hit; aite_miru 021CEC44 looks at that actor and clears it */
    /* 0x2D60 */ be<u32> mWeaponPcId;
    /* 0x2D64 */ gptr<fopAc_ac_c> mpBomb;
    /* 0x2D68 */ be<u16> mCutBits;  /* was m2960: damage_check (in daMo2_Execute 021C9B1C) ORs atInfo->mPlCutBit in; fight_run (in Mo2_move 021D0A14) guards against those cuts; search_bomb 021C8378 looks for bombs only once bit 0x200 is set */
    /* 0x2D6A */ u8 _2D6A[0x2];
    /* 0x2D6C */ be<s32> mDashFrames;  /* was m2964: fight_run (in Mo2_move 021D0A14) sets 1 when the armed Moblin starts the DASH animation, adds 1 per frame and stops the dash after 20 frames; 0 = not dashing */
    /* 0x2D70 */ be<s8> mPathOn;  /* was m2968: path_check 021C9004: 0 or mPathIndex + 1; jyunkai (in Mo2_move 021D0A14) follows the path while it is not 0 */
    /* 0x2D71 */ be<s8> mPathPoint;  /* was m2969: path_check 021C9004 picks the start point; jyunkai (in Mo2_move 021D0A14) steps it by mPathDir and indexes ppd->m_points with it; nage 021CD958 steps it back */
    /* 0x2D72 */ be<s8> mPathDir;  /* was mHasPath (misnamed; TWW calls it mHasPath): daMo2_Create 021CCB00 sets 1 with a path; jyunkai (in Mo2_move 021D0A14) adds it to mPathPoint, sets -1 / 1 at the path ends and negates it; path_check 021C9004 / nage 021CD958 step mPathPoint back by it. tww d217e2b src/d/actor/d_a_mo2.cpp (same field, same class) does the same: added to the path index (line 1325), set to 0xff / 1 (lines 1330 / 1340), negated at the path ends (line 1386) */
    /* 0x2D73 */ u8 _2D73[0x1];
    /* 0x2D74 */ gptr<dPath> ppd;
    /* 0x2D78 */ be<s8> m2970;
    /* 0x2D79 */ u8 _2D79[0x3];
    /* 0x2D7C */ ga_s m2978[3];
    /* 0x2E0C */ be<s8> mLanternStep;  /* was m2A08: kantera_get_init (in daMo2_Create 021CCB00): 0 creates the lantern, then counts the frames until it exists (deletes the Moblin after 20), 100 = lantern linked; daMo2_Draw 021C7344 draws nothing while it is 1..99 */
    /* 0x2E0D */ be<u8> m2A09;
    /* 0x2E0E */ be<u8> mHioOwner;  /* was m2A0A: daMo2_Create 021CCB00 sets 1 when it created the l_mo2HIO child; daMo2_Delete 021CC44C deletes the HIO child only then */
    /* 0x2E0F */ be<s8> mParalyzeDelay;  /* was m2A0B: damage_check (in daMo2_Execute 021C9B1C) sets REG_S(13,3) + 8 for attack type 10; Mo2_move 021D0A14 counts it down and starts ACTION_D_MAHI at 0 */
    /* 0x2E10 */ be<s16> mSoundTargetTimer;  /* was m2A0C: jyunkai (in Mo2_move 021D0A14) sets 30 when it hears a sound (dKy_Sound_get); Mo2_move 021D0A14 counts it down and targets mSoundTargetPos instead of the player while it runs */
    /* 0x2E12 */ u8 _2E12[0x2];
    /* 0x2E14 */ cXyz mSoundTargetPos;  /* was m2A10: jyunkai (in Mo2_move 021D0A14) copies the heard sound's position (dKy_Sound_get); Mo2_move 021D0A14 measures mTargetDist / mTargetAngleY to it while mSoundTargetTimer runs */
    /* 0x2E20 */ be<s8> mDefeated;  /* was m2A1C: fail 021CED78 sets 1 for a boss-status Moblin instead of deleting it; daMo2_Draw 021C7344 draws nothing, Mo2_move 021D0A14 turns Co off and mo2_demo_camera (in daMo2_Execute 021C9B1C) stops following while it is set */
    /* 0x2E21 */ be<s8> mDemoCamMode;  /* was m2A1D: mo2_demo_camera (in daMo2_Execute 021C9B1C): switch of the demo camera steps; nage 021CD958 sets 2 / 10, damage_check 0x32, daMo2_Execute 1 */
    /* 0x2E22 */ be<s16> mDemoCamTimer;  /* was m2A1E: mo2_demo_camera (in daMo2_Execute 021C9B1C): cleared per step, +1 per frame, compared with 150 */
    /* 0x2E24 */ cXyz mDemoCamEye;  /* was m2A20: mo2_demo_camera (in daMo2_Execute 021C9B1C): copies the camera's eye and passes it to dCamera_Set as the eye */
    /* 0x2E30 */ cXyz mDemoCamCenter;  /* was m2A2C: mo2_demo_camera (in daMo2_Execute 021C9B1C): actor position / camera centre eased toward the actor (+230); passed to dCamera_Set as the centre */
    /* 0x2E3C */ be<f32> m2A38;
    /* 0x2E40 */ be<f32> m2A3C;
    /* 0x2E44 */ be<f32> m2A40;
    /* 0x2E48 */ be<f32> mDemoCamFovy;  /* was m2A44: mo2_demo_camera (in daMo2_Execute 021C9B1C): 55 eased to 30; passed to dCamera_Set as the fovy */
    /* 0x2E4C */ be<s16> mCatchDemoDelay;  /* was m2A48: AtHitCallback 021C9AA0 sets 1 when the spear hits a hooded player (prison mode); daMo2_Execute 021C9B1C counts it down and at 0 starts ACTION_E3_DEMO */
    /* 0x2E4E */ be<u8> mCatchEventPending;  /* was m2A4A: AtHitCallback 021C9AA0 sets 1 with mCatchDemoDelay; daMo2_Execute 021C9B1C orders a potential event each frame until it is accepted, then clears it */
    /* 0x2E4F */ be<u8> mSendToPrison;  /* was m2A4B: e3_demo 021D0570 sets 1 at the end of the catch demo; daMo2_Execute 021C9B1C then sets the next stage "majroom" */
    /* 0x2E50 */ be<s16> mCatchTimer;  /* was m2A4C: AtHitCallback 021C9AA0 sets 200; daMo2_Execute 021C9B1C counts it down, stops JA_SE_MAJUTOU_ALERM at 30 and sets the next stage "majroom" at 1 */
    /* 0x2E52 */ u8 _2E52[0x2];
    /* 0x2E54 */ enemyice_l mEnemyIce;
    /* 0x320C */ enemyfire_l mEnemyFire;
    /* 0x3438 */ gptr<JntHit_c> mpJntHit;
};
WWHD_SIZE(mo2_class, 0x343C);
WWHD_OFFSET(mo2_class, mEyeBtpFrame, 0x458);
WWHD_OFFSET(mo2_class, mDashStopTimer, 0x854);
WWHD_OFFSET(mo2_class, mHeadAngleY, 0x87C);
WWHD_OFFSET(mo2_class, mPathTargetPos, 0x86C);
WWHD_OFFSET(mo2_class, mDamageReaction, 0x8BC);
WWHD_OFFSET(mo2_class, mbHasInnateWeapon, 0x10B0);
WWHD_OFFSET(mo2_class, mCoCyl, 0x2494);
WWHD_OFFSET(mo2_class, mJawAngleZ, 0x2D5A);
WWHD_OFFSET(mo2_class, mLanternStep, 0x2E0C);
WWHD_OFFSET(mo2_class, mEnemyIce, 0x2E54);
WWHD_OFFSET(mo2_class, mpJntHit, 0x3438);

/* mo2HIO_c: GameCube offsets - 4 (GHS: vtable pointer after the members) */
struct mo2HIO_c {
    /* 0x0000 */ be<s8> mNo;
    /* 0x0001 */ be<u8> mDrawSearchArea;  /* was m005: search_check_draw (in daMo2_Draw 021C7344) returns at once while it is 0, else builds the search circles / view box geometry from the ranges below (HD draws nothing); __sinit_d_a_mo2_cpp 021CD5B4 0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0002 */ be<u8> mFrameStep;  /* was m006: daMo2_Execute 021C9B1C runs the timers, damage_check and Mo2_move only while it is 0 or on a B trigger / Y hold (frame stepping); __sinit_d_a_mo2_cpp 021CD5B4 0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0003 */ be<u8> mKeepHealth;  /* was m007: __sinit_d_a_mo2_cpp 021CD5B4 0; no read in WWHD (daMo2_Execute: "HD: health debug reset removed"); tww d217e2b damage_check sets health = 10 after a hit while it is set (d_a_mo2.cpp line 3127); never written in WWHD or tww (tww genMessage registers nothing) */
    /* 0x0004 */ be<u8> mHeadFollowsBody;  /* was m008: nodeCallBack 021C6854: 0 = mHeadAngleY from the face joint direction (atan2), else mHeadAngleY = current.angle.y; __sinit_d_a_mo2_cpp 021CD5B4 0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0005 */ be<u8> mNoAttack;  /* was m009: hukki 021CE6E8 (mode 14) and wepon_search 021CF0F4 start an attack only while it is 0; __sinit_d_a_mo2_cpp 021CD5B4 0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0006 */ be<s16> mAlertCheckTime;  /* was m00A: nage 021CD958: mTimers[1] = it + mAlertCheckDelay (length of the KKEIKAI alert look), so the alert runs this long after the view check opens; __sinit_d_a_mo2_cpp 021CD5B4 20; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0008 */ be<s16> mAlertCheckDelay;  /* was m00C: nage 021CD958: mTimers[2] = it at the alert start; the player view re-check waits until mTimers[2] is 0; __sinit_d_a_mo2_cpp 021CD5B4 40; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x000A */ u8 _000A[0x2];
    /* 0x000C */ be<f32> mModelScale;  /* was m010: daMo2_Execute 021C9B1C: MtxScale(it, it, it) for the model base matrix; __sinit_d_a_mo2_cpp 021CD5B4 1.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0010 */ be<f32> mBlurScale;  /* was m014: br_draw (in daMo2_Draw 021C7344): MtxScale(it * mBlurScaleX, it, it) for the spear-trail blur model; __sinit_d_a_mo2_cpp 021CD5B4 1.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0014 */ be<f32> m018;
    /* 0x0018 */ be<f32> mDisappearOffsetY;  /* was m01C: fail 021CED78: fopAcM_createDisappear at current.pos with y + it + 100; __sinit_d_a_mo2_cpp 021CD5B4 25.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x001C */ be<s16> mHairMode;  /* was m020: ke_control 021C6F14: 0 = ground check under each spear-hair point, else the floor is dr.mSpawnY + 2.5; daMo2_Draw 021C7344 draws the hair (ke_disp) only while it is <= 1; daMo2_Create 021CCB00 and __sinit_d_a_mo2_cpp 021CD5B4 set 0 (tww the same) */
    /* 0x001E */ be<s16> mSmokeAlpha;  /* was m022: smoke_set_s 021C6578: alpha of the ID_AK_JT_ELEMENTSMOKE00 toon particle (dComIfGp_particle_setToon); __sinit_d_a_mo2_cpp 021CD5B4 185; never written in WWHD or tww (tww genMessage registers nothing) */
    /* 0x0020 */ be<s16> mSmokeTimerBase;  /* was m024: ground_smoke_set (in daMo2_Execute 021C9B1C) sets smoke only while mSmokeTimer >= it; b_nige / hukki / wepon_search / fight (in Mo2_move 021D0A14) load mSmokeTimer with it + 3..16; __sinit_d_a_mo2_cpp 021CD5B4 12; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0022 */ u8 _0022[0x2];
    /* 0x0024 */ be<f32> mAttentionOffsetY;  /* was m028: nodeCallBack 021C6854: attention_info.position = eyePos with y + it; __sinit_d_a_mo2_cpp 021CD5B4 25.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0028 */ be<f32> mSearchDist;  /* was m02C: jyunkai (in Mo2_move 021D0A14): an armed Moblin notices the player within it (and in view); daMo2_Execute 021C9B1C sets 800 / 1200 while the player has the grab wear in patrol; search_check_draw circle radius; __sinit_d_a_mo2_cpp 021CD5B4 1250.0 */
    /* 0x002C */ be<f32> mFightDist;  /* was m030: fight_run (in Mo2_move 021D0A14) stops the run and strafes within it, goes back to patrol beyond it + 75; nage 021CD958 / e3_demo 021D0570 walk until within it; hukki / wepon_search attack within it; __sinit_d_a_mo2_cpp 021CD5B4 575.0; never written in WWHD or tww (tww genMessage registers nothing) */
    /* 0x0030 */ be<f32> mAttackDist;  /* was m034: fight_run (in Mo2_move 021D0A14): attacks within it (random chance at it +-62.5), walks back below it, pushes (mOshiCount) below it - 62.5; __sinit_d_a_mo2_cpp 021CD5B4 375.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0034 */ be<s16> mViewAngle;  /* was m038: half angle of the view cone: jyunkai / fight_run / fight (in Mo2_move 021D0A14), nage 021CD958, p_lost 021CDE6C pass it to daMo2_player_view_check 021C8C14; search_bomb 021C8378 the same; __sinit_d_a_mo2_cpp 021CD5B4 12740; daMo2_Execute 021C9B1C 0x1932 with the grab wear */
    /* 0x0036 */ u8 _0036[0x2];
    /* 0x0038 */ be<f32> mViewHeight;  /* was m03C: daMo2_player_view_check 021C8C14 / daMo2_wepon_view_check 021C872C / search_bomb 021C8378 reject a target whose y + 50 differs from eyePos.y by more than it; __sinit_d_a_mo2_cpp 021CD5B4 400.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x003C */ cXyz mViewBox;  /* was m040: daMo2_player_view_check 021C8C14 / daMo2_wepon_view_check 021C872C / search_bomb 021C8378: target in the body frame within |x| < it.x, |y| < it.y, z < it.z is seen; __sinit_d_a_mo2_cpp 021CD5B4 (500, 130, 500); daMo2_Execute 021C9B1C x / z 340 with the grab wear */
    /* 0x0048 */ be<f32> mViewBoxBackZ;  /* was m04C: the same view checks (daMo2_player_view_check 021C8C14, daMo2_wepon_view_check 021C872C, search_bomb 021C8378): rear z limit of the view box (z > it); __sinit_d_a_mo2_cpp 021CD5B4 -125.0; daMo2_Execute 021C9B1C -200 with the grab wear */
    /* 0x004C */ be<f32> mPatrolSpeedArmed;  /* was m050: jyunkai (in Mo2_move 021D0A14): speedF target (and path-point reach distance) while the Moblin has its weapon; __sinit_d_a_mo2_cpp 021CD5B4 12.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0050 */ be<f32> mPatrolSpeedUnarmed;  /* was m054: jyunkai (in Mo2_move 021D0A14): speedF target without a weapon (turn speed 0x1000); __sinit_d_a_mo2_cpp 021CD5B4 60.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0054 */ be<f32> mRunSpeedArmed;  /* was m058: fight_run (in Mo2_move 021D0A14): speedF target with the weapon (no dash); nage 021CD958 / e3_demo 021D0570 walk toward the target at it; __sinit_d_a_mo2_cpp 021CD5B4 45.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0058 */ be<f32> mRunSpeedUnarmed;  /* was m05C: fight_run (in Mo2_move 021D0A14): speedF target without a weapon; __sinit_d_a_mo2_cpp 021CD5B4 70.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x005C */ be<f32> mSprintSpeed;  /* was m060: b_nige 021CE138 (run from a bomb) and wepon_search 021CF0F4 (run to a weapon) set speedF to it; __sinit_d_a_mo2_cpp 021CD5B4 90.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0060 */ be<f32> mStrafeSpeed;  /* was m064: fight_run (in Mo2_move 021D0A14) modes 3 / 4: speedF eased to +it (forward) / -it (back); __sinit_d_a_mo2_cpp 021CD5B4 20.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0064 */ be<f32> mSideStepSpeed;  /* was m068: fight_run (in Mo2_move 021D0A14) modes 5 / 6 (move angle +-0x4000): speedF eased to it; __sinit_d_a_mo2_cpp 021CD5B4 20.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0068 */ be<f32> mDashSpeed;  /* was m06C: fight_run (in Mo2_move 021D0A14): speedF target while mDashFrames runs (DASH animation, smoke type 4); __sinit_d_a_mo2_cpp 021CD5B4 70.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x006C */ be<f32> mDashAnmSpeed;  /* was m070: fight_run (in Mo2_move 021D0A14): play speed of the looping BCK DASH animation (anm_init); __sinit_d_a_mo2_cpp 021CD5B4 1.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0070 */ be<s16> mAttackCheckWait;  /* was m074: fight_run (in Mo2_move 021D0A14): mTimers[4] = it each time it rolls the attack chance at mAttackDist +-62.5; __sinit_d_a_mo2_cpp 021CD5B4 15; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0072 */ u8 _0072[0x2];
    /* 0x0074 */ be<f32> mAttackChance;  /* was m078: fight_run (in Mo2_move 021D0A14): attacks when cM_rndF(100) < it at that roll; __sinit_d_a_mo2_cpp 021CD5B4 80.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0078 */ be<f32> mAttack0Chance;  /* was m07C: attack_set 021C9310 (armed, on the player way): rnd(100) < it picks attack kind 0; __sinit_d_a_mo2_cpp 021CD5B4 40.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x007C */ be<f32> mAttack1Chance;  /* was m080: attack_set 021C9310: the next it percent (after mAttack0Chance) pick attack kind 1, the rest kind 2; __sinit_d_a_mo2_cpp 021CD5B4 30.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0080 */ be<f32> mAttack4Chance;  /* was m084: attack_set 021C9310 (unarmed): rnd(100) < it picks attack kind 4, else kind 5; __sinit_d_a_mo2_cpp 021CD5B4 50.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0084 */ be<s16> mDownRecoverDelay;  /* was m088: daMo2_Execute 021C9B1C damage results 0x14 / 0x15 set dr.mDownTimers[2] (recovery delay) to it; __sinit_d_a_mo2_cpp 021CD5B4 30; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0086 */ be<s16> mWeaponRunTime;  /* was m08A: wepon_search 021CF0F4 / yari_off_check (in daMo2_Execute 021C9B1C) set mWeaponRunTimer to it; __sinit_d_a_mo2_cpp 021CD5B4 300; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0088 */ be<f32> m08C;
    /* 0x008C */ be<f32> m090;
    /* 0x0090 */ be<f32> m094;
    /* 0x0094 */ be<f32> m098;
    /* 0x0098 */ be<f32> m09C;
    /* 0x009C */ be<f32> m0A0;
    /* 0x00A0 */ be<f32> m0A4;
    /* 0x00A4 */ be<f32> m0A8;
    /* 0x00A8 */ be<f32> m0AC;
    /* 0x00AC */ be<f32> m0B0;
    /* 0x00B0 */ be<f32> m0B4;
    /* 0x00B4 */ be<f32> m0B8;
    /* 0x00B8 */ be<f32> m0BC;
    /* 0x00BC */ be<f32> m0C0;
    /* 0x00C0 */ be<f32> m0C4;
    /* 0x00C4 */ be<f32> m0C8;
    /* 0x00C8 */ be<s16> mAttack0TurnEndFrame;  /* was m0CC: attack_set 021C9310 sets mTurnEndFrame to it for attack kind 0; __sinit_d_a_mo2_cpp 021CD5B4 14; never written in WWHD or tww (tww genMessage registers nothing) */
    /* 0x00CA */ be<s16> mAttack1TurnEndFrame;  /* was m0CE: attack_set 021C9310 sets mTurnEndFrame to it for attack kind 1; __sinit_d_a_mo2_cpp 021CD5B4 10; never written in WWHD or tww (tww genMessage registers nothing) */
    /* 0x00CC */ be<s16> mAttack2TurnEndFrame;  /* was m0D0: attack_set 021C9310 sets mTurnEndFrame to it for attack kind 2; wepon_search 021CF0F4 uses it for its kind-2 attack; __sinit_d_a_mo2_cpp 021CD5B4 10; never written in WWHD or tww (tww genMessage registers nothing) */
    /* 0x00CE */ be<s16> mAttack3TurnEndFrame;  /* was m0D2: attack_set 021C9310 sets mTurnEndFrame to it for attack kind 3; __sinit_d_a_mo2_cpp 021CD5B4 5; never written in WWHD or tww (tww genMessage registers nothing) */
    /* 0x00D0 */ be<s16> mAttack4TurnEndFrame;  /* was m0D4: attack_set 021C9310 sets mTurnEndFrame to it for attack kind 4; __sinit_d_a_mo2_cpp 021CD5B4 100; never written in WWHD or tww (tww genMessage registers nothing) */
    /* 0x00D2 */ be<s16> mAttack5TurnEndFrame;  /* was m0D6: attack_set 021C9310 sets mTurnEndFrame to it for attack kind 5; hukki 021CE6E8 uses it for its kind-5 attack; __sinit_d_a_mo2_cpp 021CD5B4 10; never written in WWHD or tww (tww genMessage registers nothing) */
    /* 0x00D4 */ be<s16> mAttack1BlurStart;  /* was m0D8: fight (in Mo2_move 021D0A14), attack kind 1: the spear blur (mBlurOn) starts at this attack frame; mAttack1BlurScaleX is indexed from it; __sinit_d_a_mo2_cpp 021CD5B4 29; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x00D6 */ be<s16> mAttack1BlurPreLen;  /* was m0DA: fight (in Mo2_move 021D0A14), attack kind 1: blur frames before the mBlurUseB part (start + it .. start + it + mAttack1BlurBLen); __sinit_d_a_mo2_cpp 021CD5B4 1; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x00D8 */ be<s16> mAttack1BlurBLen;  /* was m0DC: fight (in Mo2_move 021D0A14), attack kind 1: length of the part with mBlurUseB = 1; __sinit_d_a_mo2_cpp 021CD5B4 2; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x00DA */ be<s16> mAttack1BlurPostLen;  /* was m0DE: fight (in Mo2_move 021D0A14), attack kind 1: blur frames after the mBlurUseB part (blur ends at start + pre + B + it); __sinit_d_a_mo2_cpp 021CD5B4 1; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x00DC */ be<f32> mAttack1BlurScaleX[10];  /* was m0E0: fight (in Mo2_move 021D0A14), attack kind 1: mBlurScaleX = it[attack frame - mAttack1BlurStart] (10 entries); __sinit_d_a_mo2_cpp 021CD5B4 1.0 / 1.25 / 1.25 / 1.0 / 0.5 ...; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0104 */ be<s16> mAttack2BlurStart;  /* was m108: fight (in Mo2_move 021D0A14), attack kind 2: the spear blur (mBlurOn) starts at this attack frame; mAttack2BlurScaleX is indexed from it; __sinit_d_a_mo2_cpp 021CD5B4 40; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0106 */ be<s16> mAttack2BlurPreLen;  /* was m10A: fight (in Mo2_move 021D0A14), attack kind 2: blur frames before the mBlurUseB part (start + it .. start + it + mAttack2BlurBLen); __sinit_d_a_mo2_cpp 021CD5B4 1; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0108 */ be<s16> mAttack2BlurBLen;  /* was m10C: fight (in Mo2_move 021D0A14), attack kind 2: length of the part with mBlurUseB = 1; __sinit_d_a_mo2_cpp 021CD5B4 3; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x010A */ be<s16> mAttack2BlurPostLen;  /* was m10E: fight (in Mo2_move 021D0A14), attack kind 2: blur frames after the mBlurUseB part (blur ends at start + pre + B + it); __sinit_d_a_mo2_cpp 021CD5B4 1; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x010C */ be<f32> mAttack2BlurScaleX[10];  /* was m110: fight (in Mo2_move 021D0A14), attack kind 2: mBlurScaleX = it[attack frame - mAttack2BlurStart] (10 entries); __sinit_d_a_mo2_cpp 021CD5B4 1.0 / 1.1 / 1.15 / 1.25 / 1.1 / 1.0 ...; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0134 */ be<f32> mBlurOffsetX;  /* was m138: br_draw (in daMo2_Draw 021C7344): the blur model sits at x = it + 150 on the spear joint MO_JNT_MO_YARI_e; __sinit_d_a_mo2_cpp 021CD5B4 -22.25; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0138 */ be<f32> mHitPushback;  /* was m13C: damage_check (in daMo2_Execute 021C9B1C): dr.mPushback = -it for a hit with mKnockbackPower < 25; __sinit_d_a_mo2_cpp 021CD5B4 30.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x013C */ be<f32> mBtMaxDistance;  /* was m140: daMo2_Execute 021C9B1C: mBtMaxDis (parry max distance) = it each frame; __sinit_d_a_mo2_cpp 021CD5B4 650.0; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0140 */ be<s16> mAttack1BtWindow1Start;  /* was m144: attack_set 021C9310 copies it to mo2_class mBtWindow1Start for attack kind 1 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 8; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0142 */ be<s16> mAttack1BtWindow1Len;  /* was m146: attack_set 021C9310 copies it to mo2_class mBtWindow1Len for attack kind 1 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 12; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0144 */ be<s16> mAttack1BtWindow2Start;  /* was m148: attack_set 021C9310 copies it to mo2_class mBtWindow2Start for attack kind 1 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 34; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0146 */ be<s16> mAttack1BtWindow2Len;  /* was m14A: attack_set 021C9310 copies it to mo2_class mBtWindow2Len for attack kind 1 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 25; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0148 */ be<s16> mAttack2BtWindow1Start;  /* was m14C: attack_set 021C9310 copies it to mo2_class mBtWindow1Start for attack kind 2 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 16; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x014A */ be<s16> mAttack2BtWindow1Len;  /* was m14E: attack_set 021C9310 copies it to mo2_class mBtWindow1Len for attack kind 2 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 12; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x014C */ be<s16> mAttack2BtWindow2Start;  /* was m150: attack_set 021C9310 copies it to mo2_class mBtWindow2Start for attack kind 2 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 45; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x014E */ be<s16> mAttack2BtWindow2Len;  /* was m152: attack_set 021C9310 copies it to mo2_class mBtWindow2Len for attack kind 2 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 30; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0150 */ be<s16> mAttack5BtWindow1Start;  /* was m154: attack_set 021C9310 copies it to mo2_class mBtWindow1Start for attack kind 5 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 8; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0152 */ be<s16> mAttack5BtWindow1Len;  /* was m156: attack_set 021C9310 copies it to mo2_class mBtWindow1Len for attack kind 5 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 12; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0154 */ be<s16> mAttack5BtWindow2Start;  /* was m158: attack_set 021C9310 copies it to mo2_class mBtWindow2Start for attack kind 5 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 40; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0156 */ be<s16> mAttack5BtWindow2Len;  /* was m15A: attack_set 021C9310 copies it to mo2_class mBtWindow2Len for attack kind 5 (parry window, opened by daMo2_Execute 021C9B1C); __sinit_d_a_mo2_cpp 021CD5B4 25; never written in WWHD or tww (tww genMessage registers nothing) (tww the same) */
    /* 0x0158 */ JntHit_HIO_l mJntHitHIO;
    /* 0x0184 */ be<u32> __vtbl;
};
WWHD_SIZE(mo2HIO_c, 0x188);

/* resource indices and joints (GameCube res/Object/Mo2.h; the WWHD code uses the same numbers) */
enum dRes_INDEX_MO2 {
    dRes_INDEX_MO2_BAS_ADOTSUKI_e = 0x8,
    dRes_INDEX_MO2_BAS_AHAZUSHI01_e = 0x9,
    dRes_INDEX_MO2_BAS_AHAZUSHI02_e = 0xA,
    dRes_INDEX_MO2_BAS_AHAZUSHI03_e = 0xB,
    dRes_INDEX_MO2_BAS_AKAITEN01_e = 0xC,
    dRes_INDEX_MO2_BAS_AKAITEN02_e = 0xD,
    dRes_INDEX_MO2_BAS_AKAITEN03_e = 0xE,
    dRes_INDEX_MO2_BAS_ATATE01_e = 0xF,
    dRes_INDEX_MO2_BAS_ATATE02_e = 0x10,
    dRes_INDEX_MO2_BAS_ATATE03_e = 0x11,
    dRes_INDEX_MO2_BAS_ATSUKI01_e = 0x12,
    dRes_INDEX_MO2_BAS_ATSUKI02_e = 0x13,
    dRes_INDEX_MO2_BAS_ATSUKI03_e = 0x14,
    dRes_INDEX_MO2_BAS_BB_FLY_e = 0x15,
    dRes_INDEX_MO2_BAS_BKYORO_e = 0x16,
    dRes_INDEX_MO2_BAS_BWAIT_e = 0x17,
    dRes_INDEX_MO2_BAS_BWALKFB_e = 0x18,
    dRes_INDEX_MO2_BAS_BWALKLR_e = 0x19,
    dRes_INDEX_MO2_BAS_DASH_e = 0x1A,
    dRes_INDEX_MO2_BAS_DEFAULT_e = 0x1B,
    dRes_INDEX_MO2_BAS_HIPDMG01_e = 0x1C,
    dRes_INDEX_MO2_BAS_HIPDMG02_e = 0x1D,
    dRes_INDEX_MO2_BAS_JEND_e = 0x1E,
    dRes_INDEX_MO2_BAS_JSTART_e = 0x1F,
    dRes_INDEX_MO2_BAS_KKEIKAI_e = 0x20,
    dRes_INDEX_MO2_BAS_KNAGE_e = 0x21,
    dRes_INDEX_MO2_BAS_KOKERUB_e = 0x22,
    dRes_INDEX_MO2_BAS_KOKERUF_e = 0x23,
    dRes_INDEX_MO2_BAS_KWAIT_e = 0x24,
    dRes_INDEX_MO2_BAS_KWALK_e = 0x25,
    dRes_INDEX_MO2_BAS_NABIGPUNCH01_e = 0x26,
    dRes_INDEX_MO2_BAS_NABIGPUNCH02_e = 0x27,
    dRes_INDEX_MO2_BAS_NABIGPUNCH03_e = 0x28,
    dRes_INDEX_MO2_BAS_NAJAB_e = 0x29,
    dRes_INDEX_MO2_BAS_NBWAIT_e = 0x2A,
    dRes_INDEX_MO2_BAS_NBWALKFB_e = 0x2B,
    dRes_INDEX_MO2_BAS_NBWALKLR_e = 0x2C,
    dRes_INDEX_MO2_BAS_NWALK_e = 0x2D,
    dRes_INDEX_MO2_BAS_NYWALK_e = 0x2E,
    dRes_INDEX_MO2_BAS_OKIRUA_e = 0x2F,
    dRes_INDEX_MO2_BAS_OKIRUU_e = 0x30,
    dRes_INDEX_MO2_BAS_PAOMUKE_e = 0x31,
    dRes_INDEX_MO2_BAS_PUTSUBUSE_e = 0x32,
    dRes_INDEX_MO2_BAS_SCATCH_e = 0x33,
    dRes_INDEX_MO2_BAS_SHAKKEN_e = 0x34,
    dRes_INDEX_MO2_BAS_SKYORO_e = 0x35,
    dRes_INDEX_MO2_BAS_SWALK_e = 0x36,
    dRes_INDEX_MO2_BAS_TALK_e = 0x37,
    dRes_INDEX_MO2_BAS_WAIT_e = 0x38,
    dRes_INDEX_MO2_BAS_WALK_e = 0x39,
    dRes_INDEX_MO2_BAS_YOROKEB_e = 0x3A,
    dRes_INDEX_MO2_BAS_YOROKEF_e = 0x3B,
    dRes_INDEX_MO2_BCK_ADOTSUKI_e = 0x3E,
    dRes_INDEX_MO2_BCK_AHAZUSHI01_e = 0x3F,
    dRes_INDEX_MO2_BCK_AHAZUSHI02_e = 0x40,
    dRes_INDEX_MO2_BCK_AHAZUSHI03_e = 0x41,
    dRes_INDEX_MO2_BCK_AKAITEN01_e = 0x42,
    dRes_INDEX_MO2_BCK_AKAITEN02_e = 0x43,
    dRes_INDEX_MO2_BCK_AKAITEN03_e = 0x44,
    dRes_INDEX_MO2_BCK_ATATE01_e = 0x45,
    dRes_INDEX_MO2_BCK_ATATE02_e = 0x46,
    dRes_INDEX_MO2_BCK_ATATE03_e = 0x47,
    dRes_INDEX_MO2_BCK_ATSUKI01_e = 0x48,
    dRes_INDEX_MO2_BCK_ATSUKI02_e = 0x49,
    dRes_INDEX_MO2_BCK_ATSUKI03_e = 0x4A,
    dRes_INDEX_MO2_BCK_AWATEDEMO_e = 0x4B,
    dRes_INDEX_MO2_BCK_BB_FLY_e = 0x4C,
    dRes_INDEX_MO2_BCK_BKYORO_e = 0x4D,
    dRes_INDEX_MO2_BCK_BWAIT_e = 0x4E,
    dRes_INDEX_MO2_BCK_BWALKFB_e = 0x4F,
    dRes_INDEX_MO2_BCK_BWALKLR_e = 0x50,
    dRes_INDEX_MO2_BCK_DASH_e = 0x51,
    dRes_INDEX_MO2_BCK_DEFAULT_e = 0x52,
    dRes_INDEX_MO2_BCK_GAKEDEMO_e = 0x53,
    dRes_INDEX_MO2_BCK_HIPDMG01_e = 0x54,
    dRes_INDEX_MO2_BCK_HIPDMG02_e = 0x55,
    dRes_INDEX_MO2_BCK_JEND_e = 0x56,
    dRes_INDEX_MO2_BCK_JSTART_e = 0x57,
    dRes_INDEX_MO2_BCK_KKEIKAI_e = 0x58,
    dRes_INDEX_MO2_BCK_KNAGE_e = 0x59,
    dRes_INDEX_MO2_BCK_KOKERUB_e = 0x5A,
    dRes_INDEX_MO2_BCK_KOKERUF_e = 0x5B,
    dRes_INDEX_MO2_BCK_KWAIT_e = 0x5C,
    dRes_INDEX_MO2_BCK_KWALK_e = 0x5D,
    dRes_INDEX_MO2_BCK_NABIGPUNCH01_e = 0x5E,
    dRes_INDEX_MO2_BCK_NABIGPUNCH02_e = 0x5F,
    dRes_INDEX_MO2_BCK_NABIGPUNCH03_e = 0x60,
    dRes_INDEX_MO2_BCK_NAJAB_e = 0x61,
    dRes_INDEX_MO2_BCK_NBWAIT_e = 0x62,
    dRes_INDEX_MO2_BCK_NBWALKFB_e = 0x63,
    dRes_INDEX_MO2_BCK_NBWALKLR_e = 0x64,
    dRes_INDEX_MO2_BCK_NWALK_e = 0x65,
    dRes_INDEX_MO2_BCK_NYWALK_e = 0x66,
    dRes_INDEX_MO2_BCK_OKIRUA_e = 0x67,
    dRes_INDEX_MO2_BCK_OKIRUB_e = 0x68,
    dRes_INDEX_MO2_BCK_OKIRUU_e = 0x69,
    dRes_INDEX_MO2_BCK_PAOMUKE_e = 0x6A,
    dRes_INDEX_MO2_BCK_PUTSUBUSE_e = 0x6B,
    dRes_INDEX_MO2_BCK_SCATCH_e = 0x6C,
    dRes_INDEX_MO2_BCK_SHAKKEN_e = 0x6D,
    dRes_INDEX_MO2_BCK_SKYORO_e = 0x6E,
    dRes_INDEX_MO2_BCK_SLIPDEMO_e = 0x6F,
    dRes_INDEX_MO2_BCK_SWALK_e = 0x70,
    dRes_INDEX_MO2_BCK_TACKLEDEMO_e = 0x71,
    dRes_INDEX_MO2_BCK_UKYADEMO_e = 0x72,
    dRes_INDEX_MO2_BCK_WAIT_e = 0x73,
    dRes_INDEX_MO2_BCK_WAITDEMO_e = 0x74,
    dRes_INDEX_MO2_BCK_WALK_e = 0x75,
    dRes_INDEX_MO2_BDL_MO_e = 0x78,
    dRes_INDEX_MO2_BMD_KB_e = 0x7B,
    dRes_INDEX_MO2_BMT_MO_BLUR_e = 0x7E,
    dRes_INDEX_MO2_BMT_MO_GREEN_e = 0x7F,
    dRes_INDEX_MO2_BTP_TCLOSE_e = 0x82,
    dRes_INDEX_MO2_BTP_TDAMAGE_e = 0x83,
    dRes_INDEX_MO2_BTP_THAKKEN_e = 0x84,
    dRes_INDEX_MO2_BTP_TKOUGEKI_e = 0x85,
    dRes_INDEX_MO2_BTP_TMABATAKI_e = 0x86,
    dRes_INDEX_MO2_BTP_TMIHIRAKI_e = 0x87,
    dRes_INDEX_MO2_BTP_TOPEN_e = 0x88,
};
enum MO_JNT {
    MO_JNT_MO_BODY_e = 0x0,
    MO_JNT_CENTER_e = 0x1,
    MO_JNT_HARA_J_e = 0x2,
    MO_JNT_MUNE_J_e = 0x3,
    MO_JNT_ACCELA_J_e = 0x4,
    MO_JNT_ACCELB_J_e = 0x5,
    MO_JNT_DOKURO_J_e = 0x6,
    MO_JNT_ACCERA_J_e = 0x7,
    MO_JNT_ACCERB_J_e = 0x8,
    MO_JNT_KATAL_J_e = 0x9,
    MO_JNT_UDELA_J_e = 0xA,
    MO_JNT_UDELB_J_e = 0xB,
    MO_JNT_TEL_J_e = 0xC,
    MO_JNT_OYUBIL_J_e = 0xD,
    MO_JNT_YUBILA_J_e = 0xE,
    MO_JNT_YUBILB_J_e = 0xF,
    MO_JNT_KATAR_J_e = 0x10,
    MO_JNT_UDERA_J_e = 0x11,
    MO_JNT_UDERB_J_e = 0x12,
    MO_JNT_TER_J_e = 0x13,
    MO_JNT_OYUBIR_J_e = 0x14,
    MO_JNT_YARI_J_e = 0x15,
    MO_JNT_MO_YARI_e = 0x16,
    MO_JNT_YUBIRA_J_e = 0x17,
    MO_JNT_YUBIRB_J_e = 0x18,
    MO_JNT_KUBI_J_e = 0x19,
    MO_JNT_KAO_J_e = 0x1A,
    MO_JNT_AGOUE_J_e = 0x1B,
    MO_JNT_HANA_J_e = 0x1C,
    MO_JNT_JAWA_J_e = 0x1D,
    MO_JNT_JAWB_J_e = 0x1E,
    MO_JNT_JAWC_J_e = 0x1F,
    MO_JNT_TARAKO_J_e = 0x20,
    MO_JNT_MIMILA_J_e = 0x21,
    MO_JNT_MIMILB_J_e = 0x22,
    MO_JNT_MIMIRA_J_e = 0x23,
    MO_JNT_MIMIRB_J_e = 0x24,
    MO_JNT_KOSHI_J_e = 0x25,
    MO_JNT_KOKAL_J_e = 0x26,
    MO_JNT_ASHILA_J_e = 0x27,
    MO_JNT_ASHILB_J_e = 0x28,
    MO_JNT_ASHILC_J_e = 0x29,
    MO_JNT_HIZL_J_e = 0x2A,
    MO_JNT_KOKAR_J_e = 0x2B,
    MO_JNT_ASHIRA_J_e = 0x2C,
    MO_JNT_ASHIRB_J_e = 0x2D,
    MO_JNT_ASHIRC_J_e = 0x2E,
    MO_JNT_HIZR_J_e = 0x2F,
    MO_JNT_OA_J_e = 0x30,
    MO_JNT_OB_J_e = 0x31,
    MO_JNT_OC_J_e = 0x32,
};

enum Action {
    ACTION_JYUNKAI = 0,
    ACTION_FIGHT_RUN = 4,
    ACTION_FIGHT = 5,
    ACTION_NAGE = 6,
    ACTION_OSHI = 7,
    ACTION_P_LOST = 8,
    ACTION_B_NIGE = 9,
    ACTION_DEFENCE = 10,
    ACTION_HUKKI = 11,
    ACTION_WEPON_SEARCH = 12,
    ACTION_HIP_DAMAGE = 13,
    ACTION_AITE_MIRU = 14,
    ACTION_D_SIT = 17,
    ACTION_D_MAHI = 18,
    ACTION_FAIL = 20,
    ACTION_YOGAN_FAIL = 21,
    ACTION_D_DOZOU = 23,
    ACTION_CARRY = 30,
    ACTION_CARRY_DROP = 31,
    ACTION_E3_DEMO = 40,
};
enum { fpcNm_PLAYER_e = 0xA8, fpcNm_BOMB_e = 0x126, fpcNm_BOKO_e = 0x1CF };

/* ---- file statics (.data/.bss) ---- */
#define SAFESTRING_VTBL 0x10014D94 /* this TU's sead::SafeString vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* J3DModel::getModelData: HD +0xAC (bindings' J3DModel_getModelData reads +0x4: wrong) */

/* 025E789C mDoExt_btpAnm::init(modelData, btp, anmPlay, attr, speed, start, end, modify, entry) */
BOOL mDoExt_btpAnm_init(mDoExt_btpAnm_l* a, J3DModelData* d, J3DAnmTexPattern* btp, s32 anmPlay,
                        s32 attr, f32 speed, s16 start, s16 end, s32 modify, s32 entry);
/* fopAcM_checkCarryNow: actor_status bit 0x2000 */
inline bool fopAcM_checkCarryNow(fopAc_ac_c* a) {
    return (a->actor_status & 0x2000) != 0;
}
/* 020CB92C daBomb_c::chk_state(state) */
bool daBomb_chk_state(void* bomb, u32 state);
/* daPy_py_c::checkGrabWear(): mGrabItemOffsetY (HD +0x3CC) < 0 */
inline bool daPy_checkGrabWear(fopAc_ac_c* p) {
    return reinterpret_cast<Mo2PlayerView*>(p)->mGrabWear < 0.0f;
}
/* dPa_smokeEcallBack::remove(): HD virtual (vtable slot 0x44) */
inline void dPa_smokeEcallBack_remove(dPa_smokeEcallBack_l* cb) {
    gabi::call_ptr(reinterpret_cast<Mo2VirtualObject*>(cb)->mpVtable->mFunctions[17], cb);
}
/* 02041C30 enemy_fire_remove(enemyfire*) */
void enemy_fire_remove(enemyfire_l* f);

/* dBgS_LinChk on the stack (HD layout, as in d_a_kamome; per-TU vtables) */
struct dBgS_LinChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x30 - 0x24];
    /* 0x30 */ cXyz mCross;
    /* 0x3C */ u8 _3C[0x58 - 0x3C];
    /* 0x58 */ be<u32> __vtbl_58; /* dBgS_PolyPassChk */
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64; /* dBgS_GrpPassChk */
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_l, 0x6C);
struct LinChkVtbls {
    u32 v10, v20, v58, v64, dt64, dt20;
};
#define LINCHK_VTBLS                                                                               \
    (LinChkVtbls{0x10014E4C, 0x10014E5C, 0x10014E7C, 0x10014E6C, 0x10014DCC, 0x10014DBC})
inline void dBgS_LinChk_ct(dBgS_LinChk_l* c, LinChkVtbls v) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++)
        c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = v.v10;
    c->__vtbl_64 = v.v64;
    c->__vtbl_58 = v.v58;
    c->__vtbl_20 = v.v20;
}
inline void dBgS_LinChk_dt(dBgS_LinChk_l* c, LinChkVtbls v) {
    c->__vtbl_58 = v.v58;
    c->__vtbl_64 = v.dt64;
    c->__vtbl_20 = v.dt20;
    cBgS_LinChk_dt(c, 0);
}
/* a float moved by lfs/stfs without arithmetic: the recompiled code copies the bits */
inline void fcopy(be<f32>& dst, const be<f32>& src) {
    *reinterpret_cast<be<u32>*>(&dst) = *reinterpret_cast<const be<u32>*>(&src);
}

/* dBgS_GndChk on the stack (HD layout, as in d_a_kamome_exec; this TU's vtables) */
struct dBgS_GndChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;
    /* 0x50 */ be<u32> mGrp;
    void SetPos(cXyz* p) {
        m_pos.copy(*p);
    }
};
WWHD_SIZE(dBgS_GndChk_l, 0x54);
inline void dBgS_GndChk_ct(dBgS_GndChk_l* c) {
    Mo2GroundCheck_ctor(c); /* cBgS_GndChk::cBgS_GndChk */
    for (int i = 0; i < 7; i++)
        c->mPass[i] = 0;
    c->__vtbl_10 = 0x10014DDC;
    c->__vtbl_4C = 0x10014DFC;
    c->__vtbl_20 = 0x10014DEC;
    c->__vtbl_40 = 0x10014E0C;
    c->mpGrpPassChk = gabi::ea(c) + 0x4C;
    c->mpPolyPassChk = gabi::ea(c) + 0x40;
    c->mGrp = 1;
}
inline void dBgS_GndChk_dt(dBgS_GndChk_l* c) {
    c->__vtbl_20 = 0x10014DEC;
    c->__vtbl_40 = 0x10014E0C;
    c->__vtbl_4C = 0x10014DCC;
    Mo2BackgroundCheck_dtor(c, 0); /* cBgS_Chk::~cBgS_Chk */
}
void MtxRotY(f32 rad, u8 concat);

/* dPath / dPnt (unchanged from GameCube) */
struct dPnt_l {
    /* 0x0 */ u8 _0[3];
    /* 0x3 */ be<u8> mArg3;
    /* 0x4 */ cXyz m_position;
};
struct dPath_l {
    /* 0x0 */ be<u16> m_num;
    /* 0x2 */ be<u16> m_nextID;
    /* 0x4 */ u8 _4;
    /* 0x5 */ be<u8> m_closed;
    /* 0x6 */ u8 _6[2];
    /* 0x8 */ gptr<dPnt_l> m_points;
};
inline dPath_l* ppd(mo2_class* i_this) {
    return (dPath_l*)(dPath*)i_this->ppd;
}

/* fopAcM_monsSeStart (HD inline): 025E1AA4 mDoAud_monsSeStart(id, pos, actorId, param, reverb) */
inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        Mo2MonstersSound_start(id, &a->eyePos, pid, param, reverb);
    }
}
inline bool fopAcM_CheckStatus(fopAc_ac_c* a, u32 m) {
    return (a->actor_status & m) != 0;
}
inline void fopAcM_onActor(fopAc_ac_c* a) {
    dComIfGs_onActor(a->setID, a->home.roomNo);
}
inline be<u32>& attention_flags(fopAc_ac_c* a) {
    return a->attention_info.flags;
}
enum { JA_SE_CV_MO_SEARCH = 0x4812, JA_SE_CV_MO_ALERT = 0x4832, JA_SE_CV_MO_STOLEN = 0x4900 };
enum { JA_SE_CM_MD_PIYO = 0x50BC };
/* 02041CAC enemy_piyo_set(fopAc_ac_c*) */
void enemy_piyo_set(fopAc_ac_c* a);
/* 027EC9E8 HD debug print (varargs; (100, line?, fmt) from d_mahi) -- identity unknown */
void hd_debug_print(s32 a, s32 b, const char* fmt);
/* sead::SafeString operator== (HD inline): cstr() of both operands through the vtable (slot 0x14;
 * the left one twice), pointer compare, then a bounded strcmp (0x40001 characters) */
inline bool SafeString_eq(const char* a, const char* b, u32 vtbl) {
    gabi::Local<SafeString> sa;
    sa->mTextPtr = gabi::guest_addr(a);
    sa->__vtbl = vtbl;
    gabi::Local<SafeString> sb;
    sb->mTextPtr = gabi::guest_addr(b);
    sb->__vtbl = vtbl;
    gabi::call_ptr(gabi::at<Mo2VirtualTable>(sa->__vtbl)->mFunctions[5], sa.get());
    gabi::call_ptr(gabi::at<Mo2VirtualTable>(sa->__vtbl)->mFunctions[5], sa.get());
    gabi::call_ptr(gabi::at<Mo2VirtualTable>(sb->__vtbl)->mFunctions[5], sb.get());
    u32 pa = sa->mTextPtr;
    u32 pb = sb->mTextPtr;
    if (pa == pb)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::at<const be<u8>>(pa)[i];
        if (ca != gabi::at<const be<u8>>(pb)[i])
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
/* dComIfGp_getStartStageName(): the char array at play+0x5134 */
enum {
    JA_SE_CV_MO_JAB = 0x480D,
    JA_SE_CM_MO_BB_LANDING = 0x58F4,
    JA_SE_CV_MO_FIND_LANCE = 0x4811,
    JA_SE_CM_LANCE_PICKUP = 0x5806
};
/* save events: dSv_event_c at *(0x101F84DC) + 0x644 */
enum { COLORS_IN_HYRULE = 0x3802 };
struct attack_info_s {
    /* 0x00 */ be<s32> bckFileIdx;
    /* 0x04 */ be<f32> speed;
    /* 0x08 */ be<s32> soundFileIdx;
};
/* attack_info[] (.data): pointers to the per-attack tables */
enum { JA_SE_MAJUTOU_ALERM = 0x834 };
/* camera: play+0x5B30 player camera id (s8); camera_process_class* at play+0x5AF8+id*0x34;
 * its dCamera_c at +0x248 */
inline u8* dComIfGp_getPlayerCamera0() {
    s8 id = dComIfGp_getPlayerCameraID(0);
    return reinterpret_cast<u8*>(dComIfGp_getCamera(id));
}
void dCamera_SetTrimSize(void* cam, s32 size);
void dMeter_mtrHide();
void mDoAud_seStart_1(u32 id); /* 025E1988 HD: seStart(id) */
void mDoAud_seStop(u32 id, s32 time);
/* daPy_py_c demo (HD offsets): mDemo type at +0x420, param0 at +0x428, mode at +0x430 */
inline void daPy_changeOriginalDemo(fopAc_ac_c* p) {
    reinterpret_cast<Mo2PlayerView*>(p)->mDemoType = 3;
    reinterpret_cast<Mo2PlayerView*>(p)->mDemoParam0 = 0;
}
inline void daPy_changeDemoMode(fopAc_ac_c* p, u32 mode) {
    reinterpret_cast<Mo2PlayerView*>(p)->mDemoMode = mode;
}
enum { DEMO_HOLDUP_e = 0xD };
/* daPy_py_c::checkPlayerGuard(): virtual, vtable (+0xB4) slot 0x3C */
inline BOOL daPy_checkPlayerGuard(fopAc_ac_c* p) {
    return gabi::call_ptr<BOOL>(gabi::at<Mo2VirtualTable>(p->__vtbl)->mFunctions[15], p);
}
enum { OPENING_JUMP_PARRY = 1, OPENING_ROLL_PARRY = 2 };
/* 024EF0F4 dBgS::GetAttributeCode(cBgS_PolyInfo&): the line check's poly info at +0x14 */
s32 dBgS_GetAttributeCode(dBgS* bgs, void* polyInfo);
void dPa_smokeEcallBack_end_l(dPa_smokeEcallBack_l* cb);
/* JPABaseEmitter (HD offsets) */
inline void JPA_setRate(JPABaseEmitter* e, f32 r) {
    reinterpret_cast<Mo2EmitterView*>(e)->mRate = r;
}
inline void JPA_setSpread(JPABaseEmitter* e, f32 s) {
    reinterpret_cast<Mo2EmitterView*>(e)->mSpread = s;
}
inline void JPA_setMaxFrame(JPABaseEmitter* e, s32 f) {
    reinterpret_cast<Mo2EmitterView*>(e)->mMaxFrame = f;
}
inline void JPA_setGlobalDynamicsScale(JPABaseEmitter* e, f32 s) {
    reinterpret_cast<Mo2EmitterView*>(e)->mGlobalDynamicsScale.x = s;
    reinterpret_cast<Mo2EmitterView*>(e)->mGlobalDynamicsScale.y = s;
    reinterpret_cast<Mo2EmitterView*>(e)->mGlobalDynamicsScale.z = s;
}
inline void JPA_setGlobalParticleScale(JPABaseEmitter* e, f32 s) {
    reinterpret_cast<Mo2EmitterView*>(e)->mGlobalParticleScale.x = s;
    reinterpret_cast<Mo2EmitterView*>(e)->mGlobalParticleScale.y = s;
    reinterpret_cast<Mo2EmitterView*>(e)->mGlobalParticleScale.z = s;
}
/* dComIfGp_particle_setToon (HD): dPa_control_c::set with group 2, setupInfo = roomNo */
inline JPABaseEmitter* dComIfGp_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle,
                                                 const cXyz* scale, u8 alpha, void* cb, s8 roomNo) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, roomNo,
                           nullptr, nullptr, nullptr);
}
void mDoMtx_XrotS(Mtx34* m, s16 x);

/* HD J3D (as in d_a_kamome): j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a
 * model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices); user area +0xB8 */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xB8 - 0x30];
    /* 0xB8 */ gptr<mo2_class> mUserArea;
};
inline J3DModel_l* j3dSys_getModel() {
    return reinterpret_cast<J3DModel_l*>(j3dSys.getModel());
}
inline Mtx34* J3DSys_mCurrentMtx() {
    return J3DSys::mCurrentMtx;
}
/* getAnmMtx: HD marks the joint matrices dirty */
inline Mtx34* model_getAnmMtx(J3DModel_l* model, s32 jntNo) {
    J3DMtxBlock_l* blk = model->mpMtxBlock;
    blk->mFlags |= 0x10;
    return &blk->mpMtx[jntNo];
}

/* ---- Draw (HD) ---- */
/* 021D3DEC: this TU's sead::SafeString::assureTerminationImpl_ (empty); GHS calls it directly */
void SafeString_assureTermination(SafeString* s);
/* 027DF9B0 JUTNameTab::getIndex(name) (HD) */
s32 JUTNameTab_getIndex(u32 tab, u32 name);
/* J3DModelData::getMaterialNodePointer(getMaterialName()->getIndex(name)) (HD inline): the material
 * name table hangs off *(modelData+0); materials (0x39C each) at +0x10, count at +0xC */

BOOL dScnPly_isPause();
void dMat_ice_entryDL(mDoExt_McaMorf* m, s32 a, void* b);
/* 027F3F94: J3DModelData joint-tree header (HD; +8 joint count) */
u32 modelData_getJointTree(u32 md);
void mDoExt_3DlineMat0_update(mDoExt_3DlineMat0_l* l, u16 segs, f32 size, const GXColor* color,
                              u16 space, dKy_tevstr_c* tev);
inline void dComIfGd_set3DlineMat(mDoExt_3DlineMat0_l* l) {
    u32 pkt = dComIfGp_ea() + 0x5FB4;
    s32 id = gabi::call_ptr<s32>(l->mpVtable->mFunctions[5], l); /* getMaterialID() */
    Mo2LinePacket_add(pkt + id * 0x9C, l);
}
BOOL mDoExt_3DlineMat0_init(mDoExt_3DlineMat0_l* l, u16 numLines, u16 numSegs, BOOL hasSize);
JntHit_c* JntHit_create(J3DModel* m, u32 data, s16 num);

/* ---- Create / constructors ---- */
void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z);
void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z);
u8* fopAcM_CreateAppend();
u32 fpcLy_CurrentLayer();
u32 fpcSCtRq_Request(u32 layer, s16 name, u32 a, u32 b, void* params);
enum { fpcNm_KANTERA_e = 0xC1 };
inline s32 dComIfGp_CharTbl_GetNameIndex2(const char* name, s32 n) {
    return Mo2CharacterTable_getNameIndex2(gabi::at<u8>(dComIfGp_ea() + 0x50A0), name, n);
}
inline s32 dComIfGp_CharTbl_GetNameIndex(const char* name, s32 n) {
    return Mo2CharacterTable_getNameIndex(gabi::at<u8>(dComIfGp_ea() + 0x50AC), name, n);
}
void dPa_smokeEcallBack_ct(void* p, u8 a);
void dBgS_AcchCir_ct(void* p);
/* dBgS_ObjAcch: dBgS_Acch::dBgS_Acch + this TU's inline vtables */
inline void dBgS_ObjAcch_ct(dBgS_ObjAcch* p) {
    Mo2ActorCheck_ctor(p);
    Mo2AcchCtorView* view = reinterpret_cast<Mo2AcchCtorView*>(p);
    view->mPolyVtable = 0x10014E1C;
    view->mCheckVtable = 0x10014E2C;
    view->mAcchVtable = 0x10014E3C;
    view->mAcchFlags = 1;
}
void dCcD_Sph_ct(void* p);
#define MO2_AAB_VTBL 0x10014DAC /* this TU's cM3dGAab vtable */
#define MO2_VTBL 0x10014E9C     /* mo2_class vtable */

// HD: model data uses contiguous joint and material arrays.
struct Mo2Shape {
    u8 _00[4];
    be<u8> mVisible;
};
WWHD_OFFSET(Mo2Shape, mVisible, 0x4);
struct Mo2Material {
    u8 _00[4];
    gptr<Mo2Material> mpNext;
    gptr<Mo2Shape> mpShape;
    u8 _0C[0x39C - 0xC];
};
WWHD_SIZE(Mo2Material, 0x39C);
WWHD_OFFSET(Mo2Material, mpNext, 0x4);
WWHD_OFFSET(Mo2Material, mpShape, 0x8);
struct Mo2Joint {
    u8 _00[4];
    be<u16> mJntNo;
    u8 _06[2];
    be<u32> mpCallBack;
    u8 _0C[4];
    gptr<Mo2Material> mpMesh;
    u8 _14[8];
    void setCallBack(BOOL (*callback)(J3DNode*, int)) {
        mpCallBack = gabi::guest_fn(reinterpret_cast<const void*>(callback));
    }
};
WWHD_SIZE(Mo2Joint, 0x1C);
WWHD_OFFSET(Mo2Joint, mJntNo, 0x4);
WWHD_OFFSET(Mo2Joint, mpCallBack, 0x8);
WWHD_OFFSET(Mo2Joint, mpMesh, 0x10);
struct Mo2JointTree {
    u8 _00[8];
    be<u16> mJointNum;
};
struct Mo2ModelHeader {
    u8 _00[0x18];
    be<s32> mMaterialNamesOffset;
};
WWHD_OFFSET(Mo2ModelHeader, mMaterialNamesOffset, 0x18);
struct Mo2ModelData {
    gptr<Mo2ModelHeader> mpHeader;
    be<u32> mJointNum;
    gptr<Mo2Joint> mpJointArray;
    be<u32> mMaterialNum;
    gptr<Mo2Material> mpMaterialArray;
    u8 _14[0x38 - 0x14];
    be<u32> mpTexPattern;
    Mo2Joint* getJointNodePointer(u16 index) {
        Mo2Joint* joints = mpJointArray;
        return index < mJointNum ? &joints[index] : joints;
    }
};
WWHD_OFFSET(Mo2ModelData, mpTexPattern, 0x38);
struct Mo2Model {
    u8 _00[0xAC];
    gptr<Mo2ModelData> mpModelData;
    u8 _B0[8];
    be<u32> mUserArea;
};
WWHD_OFFSET(Mo2Model, mpModelData, 0xAC);
WWHD_OFFSET(Mo2Model, mUserArea, 0xB8);
inline Mo2ModelData* mo2ModelData(J3DModel* model) {
    return reinterpret_cast<Mo2Model*>(model)->mpModelData;
}
inline u16 mo2JointNum(J3DModel* model) {
    return gabi::at<Mo2JointTree>(modelData_getJointTree(gabi::ea(mo2ModelData(model))))->mJointNum;
}

void mDoExt_btpAnm_ct(mDoExt_btpAnm_l*);
void mDoExt_3DlineMat0_ct(mDoExt_3DlineMat0_l*);
void Mo2_headerByteA_ctor(u32);
void Mo2_headerByteB_ctor(u32);
void JntHit_HIO_ct(JntHit_HIO_l*);
void dBgS_Acch_dt(dBgS_ObjAcch*, s32);
void dCcD_Sph_dt(void*, s32);
void cM3dGCir_dt(void*, s32);
void mDoExt_3DlineMat0_dt(mDoExt_3DlineMat0_l*, s32);
void fopAc_ac_c_dt(fopAc_ac_c*, s32);

struct Mo2CreateAppend {
    be<u32> mParameters;
    cXyz mPosition;
    csXyz mAngle;
    u8 _16[0x21 - 0x16];
    be<s8> mRoomNo;
};
WWHD_OFFSET(Mo2CreateAppend, mAngle, 0x10);
WWHD_OFFSET(Mo2CreateAppend, mRoomNo, 0x21);
struct Mo2Lantern {
    u8 _00[0x3C0];
    be<u32> mTargetActorID;
};
WWHD_OFFSET(Mo2Lantern, mTargetActorID, 0x3C0);
struct Mo2SaveView {
    u8 _00[0x2E];
    be<u8> mDungeonFlags;
};
struct Mo2HairLine {
    gptr<cXyz> mpPositions;
    u8 _04[0xC];
};
WWHD_SIZE(Mo2HairLine, 0x10);
struct Mo2HairMaterial {
    u8 _00[0x144];
    gptr<Mo2HairLine> mpLines;
};
WWHD_OFFSET(Mo2HairMaterial, mpLines, 0x144);
struct Mo2AcchVtables {
    u8 _00[0x14];
    be<u32> mAcchVtable;
    u8 _18[8];
    be<u32> mChkVtable;
};
WWHD_OFFSET(Mo2AcchVtables, mChkVtable, 0x20);

struct Mo2TargetInfo {
    gptr<fopAc_ac_c> mActors[10];
};
struct Mo2PathChecks {
    be<u8> mIndices[255];
};
struct Mo2AttackInfo {
    gptr<attack_info_s> mAttacks[6];
};
WWHD_SIZE(Mo2TargetInfo, 0x28);
WWHD_SIZE(Mo2PathChecks, 0xFF);
WWHD_SIZE(Mo2AttackInfo, 0x18);

inline Mo2Material* mo2MaterialByName(Mo2ModelData* modelData, const char* name) {
    gabi::Local<SafeString> string;
    string->mTextPtr = gabi::guest_addr(name);
    string->__vtbl = SAFESTRING_VTBL;
    Mo2ModelHeader* header = modelData->mpHeader;
    SafeString_assureTermination(string);
    s32 offset = header->mMaterialNamesOffset;
    u32 names = offset != 0 ? gabi::ea(&header->mMaterialNamesOffset) + offset : 0;
    s32 index = JUTNameTab_getIndex(names, string->mTextPtr);
    if (index < 0)
        return nullptr;
    Mo2Material* materials = modelData->mpMaterialArray;
    if ((u32)index < modelData->mMaterialNum)
        return &materials[index];
    return materials;
}
inline Mo2SaveView* mo2SaveView() {
    return gabi::at<Mo2SaveView>(dComIfGs_ea());
}

struct Mo2HeaderWords {
    be<u32> mWords[4];
};
WWHD_SIZE(Mo2HeaderWords, 0x10);

WWHD_SIZE(LIGHT_INFLUENCE_l, 0x24);
WWHD_SIZE(Mo2Shape, 0x5);
WWHD_SIZE(Mo2JointTree, 0xA);
WWHD_SIZE(Mo2ModelData, 0x3C);
WWHD_SIZE(Mo2Model, 0xBC);
WWHD_SIZE(Mo2CreateAppend, 0x22);
WWHD_SIZE(Mo2SaveView, 0x2F);
WWHD_SIZE(Mo2AcchVtables, 0x24);
WWHD_SIZE(mDoExt_3DlineMat0_l, 0x148);
