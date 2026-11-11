/* bk_class (Bokoblin), WWHD layout. 
 *
 * GameCube -> WWHD (size 0x1848 -> 0x19A4, constructor 0209F74C):
 * - +0x11C up to mDefeated (fopEn_enemy_c as assumed in f_op_actor.h: mPhase at 0x3C8);
 * - mShadowId/0x2E4 (GameCube 0x2E0..0x2E8) shrank by 4 (HD shadows): +0x118 from mpBlurModel;
 * - HD: a dPa_followEcallBack (0x14) after mSmokeCallBack, at 0x488;
 * - damagereaction (c_damagereaction) grew by 0x34 (its joint arrays have 21 entries, the gap after mJnt12LineBlocked is 8 bytes):
 *   +0x160 from mpBridge;
 * - the padding before mDemoCamFovy shrank by 8: +0x158 for mDemoCamFovy, mEnemyIce, mEnemyFire;
 * - enemyfire grew by 4 (LIGHT_INFLUENCE 0x24): +0x15C for mpJntHit.
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"

/* ---- shared classes, local layouts (SHARED-CANDIDATE) ---- */

/* dPa_smokeEcallBack (0x20) / dPa_followEcallBack (0x14): vtable at +0 (JPACallBackBase has no
 * members); remove() is the virtual at +0x44 */
struct dPa_smokeEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x20 - 0x04];
};
WWHD_SIZE(dPa_smokeEcallBack_l, 0x20);
struct dPa_followEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x14 - 0x04];
};
WWHD_SIZE(dPa_followEcallBack_l, 0x14);

/* damagereaction (c_damagereaction), HD: the joint arrays have 21 entries (GameCube 20) and
 * the gap after mJnt12LineBlocked has 8 bytes (GameCube 4), so mJntGroundState..mJnt12LineBlocked are +0x30 and everything from m420 on is
 * +0x34 (size 0x7F0). */
struct damagereaction_l {
    /* 0x000 */ gptr<fopEn_enemy_c> mpEnemy;
    /* 0x004 */ be<s16> mMode;
    /* 0x006 */ be<s16> mAction;
    /* 0x008 */ be<s16> mEnemyType;
    /* 0x00A */ u8 m00A[2];
    /* 0x00C */ be<s32> mTimer;
    /* 0x010 */ csXyz mJntTargetAngle[21];
    /* 0x08E */ csXyz mJntAngle[21];
    /* 0x10C */ cXyz mJntPos[21];
    /* 0x208 */ cXyz mJntGroundPos[21];
    /* 0x304 */ cXyz m2E0[21];
    /* 0x400 */ be<s32> mJntGroundState[14];
    /* 0x438 */ be<s32> mJnt10LineBlocked;          /* GameCube 0x408 */
    /* 0x43C */ be<s32> mJnt11LineBlocked;
    /* 0x440 */ be<s32> mJnt8LineBlocked;
    /* 0x444 */ be<s32> mJnt9LineBlocked;
    /* 0x448 */ be<s32> mJnt12LineBlocked;
    /* 0x44C */ u8 m41C[8];
    /* 0x454 */ be<s32> mJntBgCheckTimer;
    /* 0x458 */ be<s32> mHitFlags;
    /* 0x45C */ be<f32> mKnockbackPower;
    /* 0x460 */ cXyz mHitVec;
    /* 0x46C */ be<s32> m438;
    /* 0x470 */ u8 m43C[4];
    /* 0x474 */ be<s16> mJnt10Pitch;
    /* 0x476 */ be<s16> mJnt11Pitch;
    /* 0x478 */ be<s16> mJnt8Pitch;
    /* 0x47A */ be<s16> mJnt9Pitch;
    /* 0x47C */ be<s16> mJnt12Pitch;
    /* 0x47E */ u8 m44A[2];
    /* 0x480 */ cXyz mPivot;
    /* 0x48C */ cXyz mBodyOffset;
    /* 0x498 */ u8 m464[4];
    /* 0x49C */ be<f32> mCarryOffsetY;
    /* 0x4A0 */ be<f32> mCarryOffsetZ;
    /* 0x4A4 */ be<f32> mFlinchBend;
    /* 0x4A8 */ be<f32> mShakeAmplitude;
    /* 0x4AC */ be<f32> mKnockbackSpeed;
    /* 0x4B0 */ be<s32> mLaunched;
    /* 0x4B4 */ be<s16> mFallPitch;
    /* 0x4B6 */ be<s16> mKnockbackAngleY;
    /* 0x4B8 */ be<s16> mSpinAngleY;
    /* 0x4BA */ be<s16> mFallPitchTarget;
    /* 0x4BC */ be<s16> mKnockedDown;
    /* 0x4BE */ be<s16> mHitStunTimer;
    /* 0x4C0 */ csXyz mSlopeTilt;
    /* 0x4C6 */ be<s16> mJnt12SlopeY;
    /* 0x4C8 */ be<s16> mJnt12SlopeZ;
    /* 0x4CA */ be<s16> mJnt18AddY;
    /* 0x4CC */ be<s16> mJnt13AddY;
    /* 0x4CE */ be<s16> mJnt18AddYTarget;
    /* 0x4D0 */ be<s16> mJnt13AddYTarget;
    /* 0x4D2 */ be<s16> mParalysisTimer;
    /* 0x4D4 */ be<s16> mJnt0AddZ;
    /* 0x4D6 */ be<s16> mJnt1AddZ;
    /* 0x4D8 */ be<s16> mJnt2AddZ;
    /* 0x4DA */ be<s16> mJnt3AddZ;
    /* 0x4DC */ be<s16> mJnt0AddZTarget;
    /* 0x4DE */ be<s16> mJnt1AddZTarget;
    /* 0x4E0 */ be<s16> mJnt2AddZTarget;
    /* 0x4E2 */ be<s16> mJnt3AddZTarget;
    /* 0x4E4 */ be<s16> mJnt2LandZ;
    /* 0x4E6 */ be<s16> mJnt3LandZ;
    /* 0x4E8 */ be<s16> mJnt6LandZ;
    /* 0x4EA */ be<s16> mJnt7LandZ;
    /* 0x4EC */ be<s16> mJnt6AddY;
    /* 0x4EE */ be<s16> mJnt7AddY;
    /* 0x4F0 */ be<s16> mJnt4AddY;
    /* 0x4F2 */ be<s16> mJnt5AddY;
    /* 0x4F4 */ be<s16> mLandAngleY;
    /* 0x4F6 */ u8 m4C2[6];
    /* 0x4FC */ be<s16> mDownTimers[3];
    /* 0x502 */ be<s16> mInvincibleTimer;
    /* 0x504 */ be<s16> mTurnTargetY;
    /* 0x506 */ u8 m4D2[2];
    /* 0x508 */ be<f32> mPushback;
    /* 0x50C */ u8 m4D8[4];
    /* 0x510 */ dBgS_AcchCir mAcchCir;   /* GameCube 0x4DC */
    /* 0x550 */ dBgS_ObjAcch mAcch;      /* GameCube 0x51C */
    /* 0x714 */ be<s32> mSpinLeft;
    /* 0x718 */ be<f32> mSpawnY;
    /* 0x71C */ cXyz mPrevGroundPos;
    /* 0x728 */ cXyz m6F4[2];
    /* 0x740 */ be<u8> mBounceCount;
    /* 0x741 */ be<u8> mFaceDown;
    /* 0x742 */ be<u8> mShakeJntMask;
    /* 0x743 */ u8 m70F;
    /* 0x744 */ be<u8> mLookMode;
    /* 0x745 */ be<u8> mDefencePose;
    /* 0x746 */ be<u8> m712;
    /* 0x747 */ be<u8> mPlayerInView;
    /* 0x748 */ gptr<fopAc_ac_c> mpLookTarget;
    /* 0x74C */ be<s16> mLookPitch;
    /* 0x74E */ be<s16> mLookYaw;
    /* 0x750 */ u8 m71C[2];
    /* 0x752 */ be<s16> mBodyBgCheckDelay;
    /* 0x754 */ be<f32> mMaxFallDistance;
    /* 0x758 */ dCcD_Stts mStts;         /* GameCube 0x724 */
    /* 0x794 */ cXyz mParticlePos;
    /* 0x7A0 */ csXyz mParticleAngle;
    /* 0x7A6 */ u8 m772[2];
    /* 0x7A8 */ dPa_smokeEcallBack_l mParticleCallBack;
    /* 0x7C8 */ be<s16> mWallSmokeTimer;
    /* 0x7CA */ u8 m796[2];
    /* 0x7CC */ be<f32> mHangMoveSpeed;
    /* 0x7D0 */ gptr<cXyz> mpHangPos;
    /* 0x7D4 */ cXyz mHangPos;
    /* 0x7E0 */ csXyz mHangAngle;
    /* 0x7E6 */ be<s16> mBridgeRideTimer;
    /* 0x7E8 */ be<s16> mBridgeRideRoll;
    /* 0x7EA */ be<u8> mSkipMoveOnce;
    /* 0x7EB */ u8 m7B7;
    /* 0x7EC */ be<u32> mHangBridgeId;
};
WWHD_OFFSET(damagereaction_l, mJntPos, 0x10C);
WWHD_OFFSET(damagereaction_l, mAcchCir, 0x510);
WWHD_OFFSET(damagereaction_l, mSpawnY, 0x718);
WWHD_OFFSET(damagereaction_l, mPlayerInView, 0x747);
WWHD_OFFSET(damagereaction_l, mStts, 0x758);
WWHD_OFFSET(damagereaction_l, mParticleCallBack, 0x7A8);
WWHD_SIZE(damagereaction_l, 0x7F0);
WWHD_OFFSET(damagereaction_l, mJntGroundState, 0x400);
WWHD_OFFSET(damagereaction_l, mJnt10LineBlocked, 0x438);

/* enemyice (unchanged size 0x3B8) */
struct enemyice_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 m007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> mFreezeType;  /* was m00C: view of enemyice::mFreezeType (c/c_damagereaction.h) */
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 m016[2];
    /* 0x018 */ cXyz mSpeed;
    /* 0x024 */ be<f32> mSpeedF;
    /* 0x028 */ be<f32> mBodyLiftY;  /* was m028: view of enemyice::mBodyLiftY (c/c_damagereaction.h) */
    /* 0x02C */ be<f32> mIceOffsetY;  /* was m02C: view of enemyice::mIceOffsetY (c/c_damagereaction.h) */
    /* 0x030 */ dCcD_Stts mStts;
    /* 0x06C */ dCcD_Cyl mCyl;
    /* 0x19C */ be<f32> mCylHeight;
    /* 0x1A0 */ be<f32> mWallRadius;
    /* 0x1A4 */ be<f32> mScaleXZ;
    /* 0x1A8 */ be<f32> mScaleY;
    /* 0x1AC */ be<f32> mParticleScale;
    /* 0x1B0 */ be<u8> m1B0;
    /* 0x1B1 */ be<u8> mDeathSwitch;
    /* 0x1B2 */ u8 m1B2[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(enemyice_l, 0x3B8);

/* enemyfire, HD 0x22C (constructor 0209F6C0; LIGHT_INFLUENCE grew by 4) */
struct enemyfire_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFireDuration;
    /* 0x006 */ be<s8> mMode;
    /* 0x007 */ u8 m007;
    /* 0x008 */ be<s16> mFireTimer;
    /* 0x00A */ u8 m00A[2];
    /* 0x00C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x010 */ be<s8> mFlameJntIdxs[10];
    /* 0x01A */ u8 m01A[2];
    /* 0x01C */ be<f32> mParticleScale[10];
    /* 0x044 */ be<s16> mFlameTimers[10];
    /* 0x058 */ be<u32> mpFlameEmitters[10];
    /* 0x080 */ cXyz mPrevPos;
    /* 0x08C */ cXyz mDirection;
    /* 0x098 */ be<f32> mFlameScaleY;
    /* 0x09C */ u8 m09C;
    /* 0x09D */ be<u8> mHitboxFlameIdx;
    /* 0x09E */ u8 m09E[2];
    /* 0x0A0 */ dCcD_Stts mStts;
    /* 0x0DC */ dCcD_Sph mSph;
    /* 0x208 */ u8 mLight[0x24];         /* LIGHT_INFLUENCE (HD 0x24; +0x20 = 1.0f) */
};
WWHD_SIZE(enemyfire_l, 0x22C);

WWHD_OPAQUE(br_s);
WWHD_OPAQUE(JntHit_c);
WWHD_OPAQUE(daObj_Search_Act_c);

struct bk_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> mType;
    /* 0x3D1 */ be<u8> mSightRangePrm;  /* was m02B5: daBk_Create 0209F918: param bits 8-15; stand_sight (in Bk_move 020A5EF4) and daBk_Execute 0209BD1C use it * 10 as the player sight distance (0xFF: default 500 / 300) */
    /* 0x3D2 */ be<u8> mPathNo;  /* was m02B6: daBk_Create 0209F918: param bits 16-23, dPath_GetRoomPath(it, room) when not 0xFF; path_check 0209B354 stores it + 1 in mPathOn */
    /* 0x3D3 */ be<u8> mSwitchNo;  /* was m02B7: daBk_Create 0209F918: param bits 24-31; mAppearWait = it + 1 (wait for the switch); d_dozou 020A4594 / daBk_Draw 0209971C test dComIfGs_isSwitch(it) */
    /* 0x3D4 */ be<u8> mDeathSwitch;  /* was m02B8: daBk_Create 0209F918: angle.z (0xFF -> 0), no spawn when the switch is set, copied to mEnemyIce.mDeathSwitch; fail 020A3704 / water_fail 020A3B18 dComIfGs_onSwitch(it) */
    /* 0x3D5 */ be<u8> mDeleteOnSwitch;  /* was m02B9: daBk_Create 0209F918: param & 0x10; when set the actor is not created if mSwitchNo is on, and mSwitchNo becomes 0xFF */
    /* 0x3D6 */ be<u8> mAppearWait;  /* was m02BA: daBk_Create 0209F918: 0xFF (types 2/3, wait for the player), mSwitchNo + 1 (wait for that switch); daBk_Execute 0209BD1C clears it when the player comes within range / the switch is on; daBk_Draw 0209971C draws nothing while it is set */
    /* 0x3D7 */ u8 m02BB;
    /* 0x3D8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3DC */ u8 m02C0[4];
    /* 0x3E0 */ gptr<mDoExt_btpAnm> mpEyeBtp;  /* was m02C4: useHeapInit 0209F220: mDoExt_btpAnm with BTP_TMABATAKI (blink); bk_eye_tex_anm (in daBk_Execute 0209BD1C) sets frame 6 / 0 */
    /* 0x3E4 */ be<s16> mBlinkWait;  /* was m02C8: bk_eye_tex_anm (in daBk_Execute 0209BD1C): counts down, then reloads rnd(100)+20 and starts a blink (mBlinkTimer) */
    /* 0x3E6 */ be<s16> mBlinkTimer;  /* was m02CA: bk_eye_tex_anm (in daBk_Execute 0209BD1C): rnd(3)+3 per blink; eye frame 6 (closed) while it counts down */
    /* 0x3E8 */ be<s16> mWeaponViewWait;  /* was m02CC: daBk_Create 0209F918 sets 5, wepon_search 020A3CC8 70, daBk_Execute 0209BD1C counts down; daBk_wepon_view_check 0209AB64 does not look for a weapon while it is not 0 */
    /* 0x3EA */ be<s8> mWeaponJumpCount;  /* was m02CE: wepon_search 020A3CC8: cleared when the run starts, +1 per jump over a wall (JUMP1) up to 2, then gives up */
    /* 0x3EB */ u8 m02CF;
    /* 0x3EC */ gptr<J3DModel> mpShieldModel;  /* was m02D0: useHeapInit 0209F220: model BMD_BK_TATE (shield) when mHasShield; tate_mtx_set 0209BAD8 places it, daBk_Draw 0209971C draws it */
    /* 0x3F0 */ be<u8> mHasShield;  /* was m02D4: daBk_Create 0209F918: param & 0x20 (health 7 instead of 5); useHeapInit 0209F220: green body and the TATE (shield) model; fight_run 020A0A98 guards (action 10) only when set */
    /* 0x3F1 */ be<u8> mHasMachete;  /* was m02D5: daBk_Create 0209F918: param & 0xC0, 0x40 creates boko type 1 (machete), then only 0x40 kept; useHeapInit 0209F220 selects the KB_KEN blur model; attack_set 0209B760 At type 0x800 when set */
    /* 0x3F2 */ u8 m02D6[2];
    /* 0x3F4 */ gptr<J3DModel> mpTelescopeModel;  /* was m02D8: useHeapInit 0209F220: model BDL_BOUEN (telescope) when mHasTelescope; bou_mtx_set 0209BBD4 places it at the BUKI joint, daBk_Draw 0209971C draws it */
    /* 0x3F8 */ be<u8> mHasTelescope;  /* was m02DC: daBk_Create 0209F918: set for type 0xB (no weapon created); useHeapInit 0209F220: pink body and the BOUEN (telescope) model; stand (in Bk_move 020A5EF4) plays NOZOKU (peek) */
    /* 0x3F9 */ be<u8> mGroundCheck;  /* was m02DD: fight_run 020A0A98: ground_4_check(...) result bits; 4/8/2 select the side-step direction (mSideStepDir) */
    /* 0x3FA */ be<s8> mDefeated;  /* was m02DE: fail 020A3704: set to 1 for type 10 after the drop item instead of deleting; daBk_Draw 0209971C draws nothing and Bk_move 020A5EF4 turns Co off while it is set */
    /* 0x3FB */ be<u8> mSpawnTimer;  /* was m02DF: HD: daBk_Create 0209F918 sets 5 with status bit 0x4000 (0 for type 10); daBk_Execute 0209BD1C counts down and clears bit 0x4000 at 0; HD */
    /* 0x3FC */ u8 m02E0[4];             /* GameCube mShadowId + 4: HD 4 bytes, unused */
    /* 0x400 */ gptr<J3DModel> mpBlurModel;  /* was m02E8: useHeapInit 0209F220: swing blur model KB_KEN / KB_BOKO; br_draw (in daBk_Draw 0209971C) shows its BLURA or BLURB shape */
    /* 0x404 */ be<f32> mBlurScaleX;  /* was m02EC: fight_attack (in fight 020A1AAC): l_bkHIO mBlur0ScaleX[] / mBlur1ScaleX[] by attack frame; br_draw (in daBk_Draw 0209971C) scales the blur model x by it */
    /* 0x408 */ be<s32> mBlurOn;  /* was m02F0: daBk_Execute 0209BD1C clears it each frame; fight_attack (in fight 020A1AAC) sets 1 inside the swing frames; br_draw (in daBk_Draw 0209971C) draws the blur only when set */
    /* 0x40C */ be<s32> mBlurUseB;  /* was m02F4: daBk_Execute 0209BD1C clears it; fight_attack (in fight 020A1AAC) sets 1 for the middle swing frames; br_draw shows BLURB instead of BLURA when set */
    /* 0x410 */ be<s32> mFrameCounter;  /* was m02F8: daBk_Execute 0209BD1C adds 1 per frame; ground_smoke_set 020A070C / yogan_fail 020A38D4 / fight_run 020A0A98 test its low bits */
    /* 0x414 */ be<s32> mOshiCount;  /* was m02FC: fight_run 020A0A98 counts it up while the Bokoblin holds its weapon close to the player and sees it; at 25 (+REG) it may start BK_ACTION_OSHI and clears it, else it is cleared; damage_check (in daBk_Execute 0209BD1C) clears it on a hit */
    /* 0x418 */ be<s16> mTimers[5];  /* was m0300: daBk_Execute 0209BD1C counts all 5 down each frame; actions load [0]..[4] (e.g. hukki 020A3048, water_fail 020A3B18) */
    /* 0x422 */ be<s16> mWeaponRunTimer;  /* was m030A: wepon_search 020A3CC8 sets l_bkHIO mWeaponRunTime when it starts running to a dropped weapon and gives up (bare-handed fight run) when it is 0; daBk_Execute 0209BD1C counts it down; jyunkai (in Bk_move 020A5EF4) starts no new weapon search while it runs (as tn_class mWeponRunTimer) */
    /* 0x424 */ be<s16> mAnmStopTimer;  /* was m030C: wepon_search 020A3CC8 sets 5, daBk_Execute 0209BD1C counts down and plays mpMorf only when it is 0 */
    /* 0x426 */ be<s16> mNoHitTimer;  /* was m030E: daBk_Execute 0209BD1C counts down and moves the Tg spheres 20000 down while it is not 0; damage_check (in daBk_Execute) takes a hit only when it is 0 and reloads it */
    /* 0x428 */ be<s16> mRecentHitTimer;  /* was m0310: damage_check (in daBk_Execute 0209BD1C) sets 25 on a hit (weak hits are ignored while it runs in BK_ACTION_TUBO_WAIT), daBk_Execute 0209BD1C 20 when the hiding Bokoblin comes out, then counts it down; fight_run 020A0A98 dodges shots only when it is 0 */
    /* 0x42A */ u8 m0312[2];
    /* 0x42C */ be<s16> mSideStepTimer;  /* was m0314: fight_run 020A0A98: 0x10/0x20 when a side step starts; side steps while it is not 0; daBk_Execute 0209BD1C counts down */
    /* 0x42E */ be<s16> mSideStepWait;  /* was m0316: fight_run 020A0A98: rnd(100)+30 when it reaches 0, then picks the next side step; daBk_Execute 0209BD1C counts down */
    /* 0x430 */ be<u8> mSideStepDir;  /* was m0318: fight_run 020A0A98: 0/1/2 by mGroundCheck 4/8/2 when a side step starts; selects dr.mLookMode 3/4/... */
    /* 0x431 */ u8 m0319[3];
    /* 0x434 */ be<f32> mPlayerDistance;
    /* 0x438 */ cXyz m0320;
    /* 0x444 */ u8 m032C[4];
    /* 0x448 */ be<s16> mHeadAngleY;  /* was m0330: nodeCallBack 020991CC: atan2 of the head direction (or angle.y); search_check_draw (in daBk_Draw) and daBk_player_view_check 0209B05C measure the sight cone from it */
    /* 0x44A */ be<s16> mTargetAngleY;  /* was m0332: Bk_move 020A5EF4: cM_atan2s to dr.mpLookTarget (the target); fight_run 020A0A98 / carry_drop 020A47B4 turn to it */
    /* 0x44C */ be<s16> mMoveAngleOffset;  /* was m0334: fight_run 020A0A98 sets +-0x4000 for side steps; Bk_move 020A5EF4 clears it and moves along angle.y + it */
    /* 0x44E */ be<s16> mSpitTimer;  /* was m0336: damage reaction cases in daBk_Execute 0209BD1C set 1/3/5; daBk_Execute counts down and at 0 sets particle ID_AK_JN_TUBA00 (spit) at mSpitPos */
    /* 0x450 */ cXyz mSmokePos;  /* was m0338: ground_smoke_set 020A070C computes it; smoke_set_s 02098EEC line-checks below it and sets the smoke / grass particles there */
    /* 0x45C */ csXyz mEffectAngle;  /* was m0344: ground_smoke_set 020A070C / smoke_set_s 02098EEC: smoke particle angle; daBk_Execute 0209BD1C / yogan_fail 020A38D4: TUBA00 particle angle */
    /* 0x462 */ be<s16> mSmokeAngleY;  /* was m034A: ground_smoke_set 020A070C: rotates the smoke offset by it and adds 2000 / 0x1FA0 / 0x2000 per call */
    /* 0x464 */ be<s16> mSmokeTimer;  /* was m034C: ground_smoke_set 020A070C: counts down, sets smoke while >= l_bkHIO mSmokeTimerBase; hukki 020A3048 / wepon_search 020A3CC8 / daBk_Execute load l_bkHIO mSmokeTimerBase + n */
    /* 0x466 */ be<u8> mSmokeType;  /* was m034E: ground_smoke_set 020A070C: switch 0..4 selects where the smoke comes from; hukki 020A3048 sets 3, wepon_search 020A3CC8 4, daBk_Execute 0209BD1C 1 */
    /* 0x467 */ be<u8> mSmokeCount;  /* was m034F: ground_smoke_set 020A070C clears it; smoke_set_s 02098EEC adds 1 per smoke set and tests it with the ground attribute */
    /* 0x468 */ dPa_smokeEcallBack_l mSmokeCallBack;  /* was m0350: bk_class_ct 0209F74C: dPa_smokeEcallBack_ct; smoke_set_s 02098EEC passes it to the smoke emitter; daBk_Delete 0209F164 removes it */
    /* 0x488 */ dPa_followEcallBack_l mHdFollowCb;  /* HD */
    /* 0x49C */ damagereaction_l dr;
    /* 0xC8C */ gptr<br_s> mpBridge;  /* was m0B2C: b_hang 020A5674 reads it as the bridge segment (br_s) the actor hangs on */
    /* 0xC90 */ be<s32> mHoldWeapon;  /* was m0B30: daBk_Execute 0209BD1C sets 1 when it takes the created weapon (mWeaponId) and moves that weapon with the BUKI joint while set; yari_off_check (in daBk_Execute) clears it on dropping */
    /* 0xC94 */ be<s32> mDropWeapon;  /* was m0B34: daBk_Execute 0209BD1C damage reactions set 1, damage_check 2; yari_off_check (in daBk_Execute) throws the weapon (1) or drops it (2), then clears it */
    /* 0xC98 */ u8 m0B38[0xC];
    /* 0xCA4 */ cXyz m0B44[2];
    /* 0xCBC */ be<s32> mAttackKind;  /* was m0B5C: attack_set 0209B760 sets 0/1/2; indexes bk_at_kind / bk_attack_AP / attack SE tables (yari_hit_check, fight_attack in fight 020A1AAC) */
    /* 0xCC0 */ be<s32> mAttackStep;  /* was m0B60: attack_set 0209B760 clears it; fight_attack (in fight 020A1AAC) steps +1/-1 through bkFight_attackInfo(mAttackKind)[it] animations */
    /* 0xCC4 */ be<f32> mAttackFrame;  /* was m0B64: attack_set 0209B760 clears it; fight_attack (in fight 020A1AAC) adds the step speed; compared with mAtStartFrame/mAtEndFrame and the l_bkHIO blur frames */
    /* 0xCC8 */ be<f32> mAtStartFrame;  /* was m0B68: attack_set 0209B760 sets 23/3/0 (+REG); yari_hit_check (in fight_run 020A0A98): no At before it, attack SE at it */
    /* 0xCCC */ be<f32> mAtEndFrame;  /* was m0B6C: attack_set 0209B760 sets 26/9/20 (+REG); yari_hit_check (in fight_run 020A0A98): no At after it */
    /* 0xCD0 */ be<f32> mLookStartFrame;  /* was m0B70: attack_set 0209B760 sets 45, wepon_search 020A3CC8 63; fight_attack (in fight 020A1AAC) sets dr.mLookMode = 1 (head toward mpLookTarget) once mAttackFrame is past it and the target is in view (as mo2_class mLookStartFrame) */
    /* 0xCD4 */ be<f32> mTurnEndFrame;  /* was m0B74: attack_set 0209B760 / wepon_search 020A3CC8 set it; fight_attack (in fight 020A1AAC) keeps turning to mTargetAngleY while mAttackFrame is below it */
    /* 0xCD8 */ be<s16> mAtHitWait;  /* was m0B78: yari_hit_check (in fight_run 020A0A98) sets 5 on an At hit and has no At while it is not 0; daBk_Execute 0209BD1C counts down */
    /* 0xCDA */ be<s8> mAttackDir;  /* was m0B7A: attack_set / wepon_search 020A3CC8 set 1; fight_attack (in fight 020A1AAC) sets -1 when the player guards (plays back); yari_hit_check has no At while < 0 */
    /* 0xCDB */ be<s8> mWeaponAtHome;  /* was m0B7B: stand2 (in Bk_move 020A5EF4) sets 1; daBk_Execute 0209BD1C clears it and, while set, places the held weapon at home.pos instead of the BUKI joint */
    /* 0xCDC */ u8 m0B7C[0xC];
    /* 0xCE8 */ dCcD_Cyl mCoCyl;  /* was m0B88: daBk_Create 0209F918: Set(co_cyl_src); daBk_Execute 0209BD1C places it 35 ahead and sets it in the Co check */
    /* 0xE18 */ dCcD_Cyl mTgCyl;  /* was m0CB8: daBk_Create 0209F918: Set(tg_cyl_src); damage_check (in daBk_Execute 0209BD1C) reads its Tg hit */
    /* 0xF48 */ dCcD_Sph mHeadSph;  /* was m0DE8: daBk_Create 0209F918: Set(head_sph_src); daBk_Execute 0209BD1C centres it on mSpitPos (head); damage_check reads its Tg hit */
    /* 0x1074 */ dCcD_Sph mDefenceSph;  /* was m0F14: daBk_Create 0209F918: Set(defence_sph_src); damage_check (in daBk_Execute 0209BD1C) centres it on mDefenceSphPos and reads its Tg hit */
    /* 0x11A0 */ dCcD_Sph mWeaponSph;  /* was m1040: daBk_Create 0209F918: Set(wepon_sph_src); yari_hit_check (in fight_run 020A0A98) moves it along the swing and reads its At hit */
    /* 0x12CC */ cXyz mSpitPos;  /* was m116C: nodeCallBack 020991CC: head joint point (20.75, 18.5); daBk_Execute 0209BD1C / yogan_fail 020A38D4 set the TUBA00 (spit) particle there */
    /* 0x12D8 */ cXyz mWeaponTipPos;  /* was m1178: daBk_Execute 0209BD1C: point on the held weapon's matrix; yari_hit_check (in fight_run 020A0A98) copies it to mAtPos */
    /* 0x12E4 */ u8 m1184[0xC];
    /* 0x12F0 */ cXyz mNeckPos;  /* was m1190: nodeCallBack_P 02099584: point on joint BK_JNT_KUBI (neck) */
    /* 0x12FC */ cXyz mChestPos;  /* was m119C: nodeCallBack_P 02099584: point on joint BK_JNT_MUNE (chest) */
    /* 0x1308 */ cXyz mAtPos;  /* was m11A8: yari_hit_check (in fight_run 020A0A98): mWeaponTipPos (or dr.mJntPos[14] for kind 2), the At sphere moves to it; ground_smoke_set 020A070C */
    /* 0x1314 */ u8 m11B4[0xC];
    /* 0x1320 */ cXyz mAtPosOld;  /* was m11C0: yari_hit_check (in fight_run 020A0A98): mAtPos of the previous frame */
    /* 0x132C */ cXyz mShieldPos;  /* was m11CC: tate_mtx_set 0209BAD8: point on the shield (TATE joint); damage_check (in daBk_Execute 0209BD1C) sets the KIKUZU particle there; defence 020A2E04 copies it to mDefenceSphPos */
    /* 0x1338 */ cXyz mDefenceSphPos;  /* was m11D8: defence 020A2E04 copies mShieldPos; damage_check (in daBk_Execute 0209BD1C) centres mDefenceSph on it and moves it to y -10000 when not guarding */
    /* 0x1344 */ cXyz m11E4;
    /* 0x1350 */ be<u8> m11F0;
    /* 0x1351 */ be<u8> mAtStarted;  /* was m11F1: wepon_search 020A3CC8 / attack_set 0209B760 clear it; yari_hit_check (in fight_run 020A0A98) starts the At sphere when it is 0, then sets it */
    /* 0x1352 */ be<u8> mTurnAlways;  /* was m11F2: wepon_search 020A3CC8 sets 1, attack_set 0209B760 clears it; fight_attack (in fight 020A1AAC) keeps turning to the target while it is set */
    /* 0x1353 */ be<u8> mBareHanded;  /* was m11F3: daBk_Create 0209F918 sets 1 when no weapon is created; wepon_search 020A3CC8 clears it; walk_set/jyunkai (in Bk_move 020A5EF4) branch on it */
    /* 0x1354 */ be<s16> mHeadAngleZ;  /* was m11F4: nodeCallBack 020991CC: ZrotM by it; aite_miru 020A3624 / fight_run 020A0A98 ease it to 0x2EE0/12000, Bk_move 020A5EF4 back to 0 */
    /* 0x1356 */ u8 m11F6[2];
    /* 0x1358 */ gptr<fopAc_ac_c> mpBomb;  /* was m11F8: daBk_bomb_view_check 0209AE40 / b_nige 020A2A54 store search_bomb() */
    /* 0x135C */ be<u32> mHitActorId;  /* was m11FC: fight_attack (in fight 020A1AAC) stores the hit actor's ID; aite_miru 020A3624 looks it up and clears it to fpcM_ERROR_PROCESS_ID_e */
    /* 0x1360 */ be<u32> mWeaponId;  /* was m1200: daBk_Create 0209F918: fopAcM_create(fpcNm_BOKO_e, ...); daBk_wepon_view_check 0209AB64 stores the found weapon; daBk_Execute 0209BD1C moves it with the hand */
    /* 0x1364 */ be<u32> mHideActorId;  /* was m1204: daBk_Execute 0209BD1C: ID of the actor the waiting (mAppearWait 0xFF) Bokoblin hit, followed until the player comes near */
    /* 0x1368 */ be<u16> mCutBits;  /* was m1208: damage_check (in daBk_Execute 0209BD1C) ORs atInfo->mPlCutBit; fight_run 020A0A98 guards against those cuts (learn_check) */
    /* 0x136A */ u8 m120A[2];
    /* 0x136C */ be<s32> mJumpMove;  /* was m120C: fight_run 020A0A98 and path_run (in Bk_move 020A5EF4) set 1 when the Bokoblin moves by jumps (BK_JUMP1 / JUMP2, steps in mJumpStep) instead of running, and clear it with the BK_RUN animation; path_run turns faster (0x2000) while it is set; jyunkai clears it */
    /* 0x1370 */ be<u8> mJumpStep;  /* was m1210: fight_run 020A0A98: switch 0/1/2 of the JUMP2 jump while holding a weapon */
    /* 0x1371 */ u8 m1211;
    /* 0x1372 */ be<s16> m1212;
    /* 0x1374 */ be<s8> mWeaponPending;  /* was m1214: daBk_Create 0209F918 sets 1 after creating the weapon; daBk_Execute 0209BD1C takes it into the hand once it exists and clears it */
    /* 0x1375 */ be<s8> mPathOn;  /* was m1215: path_check 0209B354: 0 or mPathNo + 1; jyunkai (in Bk_move 020A5EF4) follows the path while it is not 0 */
    /* 0x1376 */ be<s8> mPathPoint;  /* was m1216: path_check 0209B354 / jyunkai (in Bk_move 020A5EF4): index of the current path point, stepped by mPathDir and kept in 0..num-1 */
    /* 0x1377 */ be<s8> mPathDir;  /* was m1217: daBk_Create 0209F918 sets 1; path_run / jyunkai (in Bk_move 020A5EF4) add it to mPathPoint and set -1/1 at the path ends */
    /* 0x1378 */ gptr<dPath> ppd;
    /* 0x137C */ be<s8> mPathEnd;  /* was m121C: path_run (in Bk_move 020A5EF4) sets 1 at either path end; daBk_Draw 0209971C draws nothing and daBk_Execute 0209BD1C deletes the weapon while set */
    /* 0x137D */ be<u8> mHioOwner;  /* was m121D: daBk_Create 0209F918 sets 1 when it created the l_bkHIO child; daBk_Delete 0209F164 removes the HIO child only then */
    /* 0x137E */ be<s8> mStandWaitCount;  /* was m121E: stand (in Bk_move 020A5EF4) adds 1 per wait cycle; after the NOBI stretch it goes to mode 2 below 3 cycles, else to mode 3 and clears it */
    /* 0x137F */ be<s8> mWeaponLost;  /* was m121F: yari_off_check (in daBk_Execute 0209BD1C) sets 1 when the weapon is dropped; walk_set (in Bk_move 020A5EF4) reacts once and clears it */
    /* 0x1380 */ gptr<daObj_Search_Act_c> mpSearchLight;
    /* 0x1384 */ be<s16> mRotAngle;  /* was m1224: stand2 (in Bk_move 020A5EF4) plays BK_MIGIROT / BK_HIDARIROT (turn right / left) when its change against mPrevRotAngle changes sign; nothing sets it (same in tww d217e2b) */
    /* 0x1386 */ u8 m1226[2];
    /* 0x1388 */ be<s16> mPrevRotAngle;  /* was m1228: stand2 (in Bk_move 020A5EF4) stores mRotAngle here each frame */
    /* 0x138A */ be<s16> mRotStep;  /* was m122A: stand2 (in Bk_move 020A5EF4) stores mRotAngle - mPrevRotAngle each frame and picks BK_MIGIROT / BK_HIDARIROT when the new step has the other sign */
    /* 0x138C */ be<s8> mParalyzeDelay;  /* was m122C: damage_check (in daBk_Execute 0209BD1C) sets 8 for attack type 10; Bk_move 020A5EF4 counts down and starts BK_ACTION_D_MAHI (paralysis) at 0 */
    /* 0x138D */ be<s8> mDemoCamMode;  /* was m1234: demo_camera (in daBk_Execute 0209BD1C): switch of the demo camera steps (0, 1..5, 11, 50, 51); damage_check sets 50, stand (in Bk_move) 1; HD: packed after mParalyzeDelay (fail, demo_camera) */
    /* 0x138E */ be<s16> mDemoCamTimer;  /* was m1236: demo_camera (in daBk_Execute 0209BD1C): cleared per step, compared with 5..150 frame marks */
    /* HD: no m1230 (J3DMaterialTable*, GameCube 0x1230) */
    /* 0x1390 */ cXyz mDemoCamEye;  /* was m1238: demo_camera (in daBk_Execute 0209BD1C): camera eye (copies the camera's eye), passed to dCam_Set as the eye; demo_camera stores */
    /* 0x139C */ cXyz mDemoCamCenter;  /* was m1244: demo_camera (in daBk_Execute 0209BD1C): camera centre (player / ken / actor position + height), passed to dCam_Set as the centre */
    /* 0x13A8 */ u8 m1250[0x10];
    /* 0x13B8 */ be<f32> mDemoCamFovy;  /* was m1260: demo_camera (in daBk_Execute 0209BD1C): 45/35/55 eased; passed to dCam_Set as the fovy */
    /* 0x13BC */ enemyice_l mEnemyIce;
    /* 0x1774 */ enemyfire_l mEnemyFire;
    /* 0x19A0 */ gptr<JntHit_c> mpJntHit;
};
WWHD_OFFSET(bk_class, mPhase, 0x3C8);
WWHD_OFFSET(bk_class, mpMorf, 0x3D8);
WWHD_OFFSET(bk_class, mpBlurModel, 0x400);
WWHD_OFFSET(bk_class, mSmokePos, 0x450);
WWHD_OFFSET(bk_class, mSmokeCallBack, 0x468);
WWHD_OFFSET(bk_class, dr, 0x49C);
WWHD_OFFSET(bk_class, mHoldWeapon, 0xC90);
WWHD_OFFSET(bk_class, mCoCyl, 0xCE8);
WWHD_OFFSET(bk_class, mWeaponSph, 0x11A0);
WWHD_OFFSET(bk_class, mSpitPos, 0x12CC);
WWHD_OFFSET(bk_class, mpBomb, 0x1358);
WWHD_OFFSET(bk_class, ppd, 0x1378);
WWHD_OFFSET(bk_class, mDemoCamMode, 0x138D);
WWHD_OFFSET(bk_class, mDemoCamEye, 0x1390);
WWHD_OFFSET(bk_class, mDemoCamFovy, 0x13B8);
WWHD_OFFSET(bk_class, mEnemyIce, 0x13BC);
WWHD_OFFSET(bk_class, mEnemyFire, 0x1774);
WWHD_OFFSET(bk_class, mpJntHit, 0x19A0);
WWHD_SIZE(bk_class, 0x19A4);

/* bkHIO_c, HD: the vtable pointer follows the members (0x108), so every GameCube offset is -4.
 * l_bkHIO is at 0x10462368. */
struct bkHIO_c {
    /* 0x000 */ be<s8> mNo;
    /* 0x001 */ be<u8> mDrawSearchRange;  /* was m005: search_check_draw (in daBk_Draw 0209971C) draws the sight circles and cone only while it is set; __sinit_d_a_bk_cpp 020A0364 sets the default 0 */
    /* 0x002 */ be<u8> mDebugFrameStep;  /* was m006: daBk_Execute 0209BD1C runs the per-frame update (mFrameCounter, mTimers, actions) only while it is 0 or on a B trigger / Y hold, so a set value steps frame by frame; __sinit_d_a_bk_cpp 020A0364 sets the default 0 */
    /* 0x003 */ be<u8> mResetHealthOnHit;  /* was m007: damage_check (in daBk_Execute 0209BD1C) sets health back to 10 on every hit while it is set; __sinit_d_a_bk_cpp 020A0364 sets the default 0 */
    /* 0x004 */ be<u8> mAlwaysGuard;  /* was m008: fight_run 020A0A98: a shield Bokoblin (mHasShield) guards a cut every time while it is set, else with a 50 % chance; __sinit_d_a_bk_cpp 020A0364 sets the default 1 */
    /* 0x005 */ be<u8> mHeadAngleIsBody;  /* was m009: nodeCallBack 020991CC: while set mHeadAngleY takes angle.y instead of the head joint direction; __sinit_d_a_bk_cpp 020A0364 sets the default 0 */
    /* 0x006 */ be<u8> mNoPickupAttack;  /* was m00A: wepon_search 020A3CC8: once the weapon is picked up, attacks (action 5) a player inside mAttackDist only while it is 0; __sinit_d_a_bk_cpp 020A0364 sets the default 0 */
    /* 0x007 */ u8 m00B;
    /* 0x008 */ be<s16> mSmokeTimerBase;  /* was m00C: ground_smoke_set 020A070C sets smoke while mSmokeTimer >= it; hukki 020A3048, wepon_search 020A3CC8, carry_drop 020A47B4, daBk_Execute 0209BD1C and fight_run 020A0A98 load mSmokeTimer with it + 2..16; __sinit_d_a_bk_cpp 020A0364 sets the default 4 */
    /* 0x00A */ u8 m00E[2];
    /* 0x00C */ be<f32> mWeakHitPushback;  /* was m010: damage_check (in daBk_Execute 0209BD1C): dr.mPushback = -it for a hit with knockback power below 25; __sinit_d_a_bk_cpp 020A0364 sets the default 0.0 */
    /* 0x010 */ be<f32> mAttackBtMaxDis;  /* was m014: fight_attack (in fight 020A1AAC) copies it to the fopEn_enemy_c mBtMaxDis of the swing; __sinit_d_a_bk_cpp 020A0364 sets the default 400 */
    /* 0x014 */ be<f32> mBlurScale;  /* was m018: br_draw (in daBk_Draw 0209971C): MtxScale(it * mBlurScaleX, it, it) of the swing blur model; __sinit_d_a_bk_cpp 020A0364 sets the default 0.7 */
    /* 0x018 */ be<f32> m01C;
    /* 0x01C */ be<f32> mDropItemOffsetY;  /* was m020: fail 020A3704: the drop item (fopAcM_createDisappear) appears at current.pos.y + 100 + it; __sinit_d_a_bk_cpp 020A0364 sets the default 25 */
    /* 0x020 */ be<f32> mAttnOffsetY;  /* was m024: nodeCallBack 020991CC: attention_info.position = eyePos + it in y; __sinit_d_a_bk_cpp 020A0364 sets the default 25 */
    /* 0x024 */ be<f32> mNoticeDist;  /* was m028: jyunkai / path_run (in Bk_move 020A5EF4) notice the player closer than it (with the sight check); search_check_draw (in daBk_Draw 0209971C) draws a circle of this radius; __sinit_d_a_bk_cpp 020A0364 sets the default 1000 */
    /* 0x028 */ be<f32> mAttackDist;  /* was m02C: fight_attack (in fight 020A1AAC), hukki 020A3048 and wepon_search 020A3CC8 attack a player closer than it; fight_run 020A0A98 gives up the fight beyond it + 75; search_check_draw draws a circle of this radius; __sinit_d_a_bk_cpp 020A0364 sets the default 400 */
    /* 0x02C */ be<f32> mFightDist;  /* was m030: fight_run 020A0A98: walks in (mode 3) farther than it, backs off (mode 4) closer, attack chances inside it +- 62.5, the push (oshi) check below it - 62.5; __sinit_d_a_bk_cpp 020A0364 sets the default 240 */
    /* 0x030 */ be<s16> mViewAngle;  /* was m034: daBk_player_view_check 0209B05C (called by fight_run 020A0A98, p_lost 020A27F8, jyunkai in Bk_move 020A5EF4, fight_attack) and search_bomb 0209A728: half angle of the sight cone around mHeadAngleY; search_check_draw draws the cone with it; __sinit_d_a_bk_cpp 020A0364 sets the default 0x6590 */
    /* 0x032 */ u8 m036[2];
    /* 0x034 */ be<f32> mViewHeight;  /* was m038: daBk_player_view_check 0209B05C, search_bomb 0209A728 and search_wepon_inline (in daBk_wepon_view_check 0209AB64): no sight when the height difference to eyePos is above it; search_check_draw draws the cone at +- it; __sinit_d_a_bk_cpp 020A0364 sets the default 300 */
    /* 0x038 */ be<f32> mViewBoxHalfX;  /* was m03C: daBk_player_view_check 0209B05C, search_bomb 0209A728, search_wepon_inline (in daBk_wepon_view_check 0209AB64): the near sight box is |x| < it in the actor's frame; __sinit_d_a_bk_cpp 020A0364 sets the default 500 */
    /* 0x03C */ be<f32> mViewBoxHalfY;  /* was m040: daBk_player_view_check 0209B05C, search_bomb 0209A728, search_wepon_inline (in daBk_wepon_view_check 0209AB64): the near sight box is |y| < it; __sinit_d_a_bk_cpp 020A0364 sets the default 300 */
    /* 0x040 */ be<f32> mViewBoxFront;  /* was m044: daBk_player_view_check 0209B05C, search_bomb 0209A728, search_wepon_inline (in daBk_wepon_view_check 0209AB64): the near sight box ends at z < it (in front); __sinit_d_a_bk_cpp 020A0364 sets the default 500 */
    /* 0x044 */ be<f32> mViewBoxBack;  /* was m048: daBk_player_view_check 0209B05C, search_bomb 0209A728, search_wepon_inline (in daBk_wepon_view_check 0209AB64): the near sight box starts at z > it (behind); __sinit_d_a_bk_cpp 020A0364 sets the default -125 */
    /* 0x048 */ be<f32> mPatrolSpeed;  /* was m04C: jyunkai (in Bk_move 020A5EF4): patrol speedF target with a weapon (or bare-handed), arrival radius half of it; __sinit_d_a_bk_cpp 020A0364 sets the default 12 */
    /* 0x04C */ be<f32> mPatrolSpeedNoWeapon;  /* was m050: jyunkai (in Bk_move 020A5EF4): patrol speedF target without a weapon, arrival radius half of it; __sinit_d_a_bk_cpp 020A0364 sets the default 60 */
    /* 0x050 */ be<f32> mRunSpeed;  /* was m054: stand / path_run (in Bk_move 020A5EF4) speedF (x 0.5 / x 1.2 in some modes, home arrival radius it x 1.25); fight_run 020A0A98 run speed with a weapon; __sinit_d_a_bk_cpp 020A0364 sets the default 45 */
    /* 0x054 */ be<f32> mRunSpeedNoWeapon;  /* was m058: fight_run 020A0A98: run speed (mode 1) while it holds no weapon and is not bare-handed; __sinit_d_a_bk_cpp 020A0364 sets the default 70 */
    /* 0x058 */ be<f32> mDashSpeed;  /* was m05C: wepon_search 020A3CC8 (run to the weapon, x 0.5 while turning) and b_nige 020A2A54 (flee from a bomb): speedF; __sinit_d_a_bk_cpp 020A0364 sets the default 90 */
    /* 0x05C */ be<f32> mFightWalkSpeed;  /* was m060: fight_run 020A0A98: speedF target walking in (mode 3), minus it backing off (mode 4); __sinit_d_a_bk_cpp 020A0364 sets the default 20 */
    /* 0x060 */ be<f32> mSideStepSpeed;  /* was m064: fight_run 020A0A98: speedF target of the side step (mMoveAngleOffset +-0x4000); __sinit_d_a_bk_cpp 020A0364 sets the default 20 */
    /* 0x064 */ be<f32> mJumpRunSpeed;  /* was m068: fight_run 020A0A98: speedF during the JUMP2 jumps while holding a weapon (mJumpStep); __sinit_d_a_bk_cpp 020A0364 sets the default 90 */
    /* 0x068 */ be<f32> m06C;
    /* 0x06C */ be<f32> mRunAnmSpeed;  /* was m070: fight_run 020A0A98 (fight_run_set) and path_run (in Bk_move 020A5EF4): speed of the BK_RUN animation; __sinit_d_a_bk_cpp 020A0364 sets the default 1.0 */
    /* 0x070 */ be<f32> mDashAnmSpeed;  /* was m074: wepon_search 020A3CC8: speed of the BK_RUN animation while running to the weapon; __sinit_d_a_bk_cpp 020A0364 sets the default 1.5 */
    /* 0x074 */ be<s16> mAttackChanceWait;  /* was m078: fight_run 020A0A98: reloads mTimers[4] with it; the attack chance (mAttackChance) is rolled only when that timer is 0; __sinit_d_a_bk_cpp 020A0364 sets the default 0x23 */
    /* 0x076 */ u8 m07A[2];
    /* 0x078 */ be<f32> mAttackChance;  /* was m07C: fight_run 020A0A98: attacks (action 5) when cM_rndF(100) < it inside mFightDist +- 62.5; __sinit_d_a_bk_cpp 020A0364 sets the default 50 */
    /* 0x07C */ be<f32> m080;
    /* 0x080 */ be<f32> m084;
    /* 0x084 */ be<f32> m088;
    /* 0x088 */ be<s16> mDownTime;  /* was m08C: daBk_Execute 0209BD1C: damage reaction modes 20 / 21 load dr.mDownTimers[2] with it; __sinit_d_a_bk_cpp 020A0364 sets the default 0x1E */
    /* 0x08A */ be<s16> mWeaponRunTime;  /* was m08E: wepon_search 020A3CC8 loads mWeaponRunTimer with it when the run to the weapon starts; __sinit_d_a_bk_cpp 020A0364 sets the default 0x12C */
    /* 0x08C */ be<f32> m090;
    /* 0x090 */ be<f32> m094;
    /* 0x094 */ be<f32> m098;
    /* 0x098 */ be<f32> mAttackTurnEndFrame;  /* was m09C: attack_set 0209B760 (kind 0) and wepon_search 020A3CC8 copy it to mTurnEndFrame; __sinit_d_a_bk_cpp 020A0364 sets the default 10 */
    /* 0x09C */ be<s16> mBlur0Start;  /* was m0A0: fight_attack (in fight 020A1AAC), attack kind 0: the swing blur is on from this mAttackFrame; mBlur0ScaleX is indexed from it; __sinit_d_a_bk_cpp 020A0364 sets the default 0x17 */
    /* 0x09E */ be<s16> mBlur0ALen;  /* was m0A2: fight_attack (in fight 020A1AAC), attack kind 0: frames of the BLURA shape after mBlur0Start before mBlurUseB; __sinit_d_a_bk_cpp 020A0364 sets the default 1 */
    /* 0x0A0 */ be<s16> mBlur0BLen;  /* was m0A4: fight_attack (in fight 020A1AAC), attack kind 0: frames with mBlurUseB (BLURB shape); __sinit_d_a_bk_cpp 020A0364 sets the default 1 */
    /* 0x0A2 */ be<s16> mBlur0TailLen;  /* was m0A6: fight_attack (in fight 020A1AAC), attack kind 0: the last part of the blur window (start + A + B + it); __sinit_d_a_bk_cpp 020A0364 sets the default 0 */
    /* 0x0A4 */ be<f32> mBlur0ScaleX[10];  /* was m0A8: fight_attack (in fight 020A1AAC), attack kind 0: mBlurScaleX = it[mAttackFrame - mBlur0Start] (10 entries); __sinit_d_a_bk_cpp 020A0364 sets the default table bkHioDefaultsA8 */
    /* 0x0CC */ be<s16> mBlur1Start;  /* was m0D0: fight_attack (in fight 020A1AAC), attack kind 1: the swing blur is on from this mAttackFrame; mBlur1ScaleX is indexed from it; __sinit_d_a_bk_cpp 020A0364 sets the default 5 */
    /* 0x0CE */ be<s16> mBlur1ALen;  /* was m0D2: fight_attack (in fight 020A1AAC), attack kind 1: frames of the BLURA shape after mBlur1Start before mBlurUseB; __sinit_d_a_bk_cpp 020A0364 sets the default 1 */
    /* 0x0D0 */ be<s16> mBlur1BLen;  /* was m0D4: fight_attack (in fight 020A1AAC), attack kind 1: frames with mBlurUseB (BLURB shape); __sinit_d_a_bk_cpp 020A0364 sets the default 1 */
    /* 0x0D2 */ be<s16> mBlur1TailLen;  /* was m0D6: fight_attack (in fight 020A1AAC), attack kind 1: the last part of the blur window (start + A + B + it); __sinit_d_a_bk_cpp 020A0364 sets the default 1 */
    /* 0x0D4 */ be<f32> mBlur1ScaleX[10];  /* was m0D8: fight_attack (in fight 020A1AAC), attack kind 1: mBlurScaleX = it[mAttackFrame - mBlur1Start] (10 entries); __sinit_d_a_bk_cpp 020A0364 sets the default table bkHioDefaultsD8 */
    /* 0x0FC */ be<f32> mBlurOffsetX;  /* was m100: br_draw (in daBk_Draw 0209971C): the blur model sits 150 + it along the BUKI joint's x; __sinit_d_a_bk_cpp 020A0364 sets the default -60 */
    /* 0x100 */ be<s16> mWaitTimeMin;  /* was m104: stand (in Bk_move 020A5EF4): mTimers[1] = random in [it, mWaitTimeMax) for the wait; __sinit_d_a_bk_cpp 020A0364 sets the default 0xC8 */
    /* 0x102 */ be<s16> mWaitTimeMax;  /* was m106: stand (in Bk_move 020A5EF4): upper end of the random wait time (mWaitTimeMin); __sinit_d_a_bk_cpp 020A0364 sets the default 0x1F4 */
    /* 0x104 */ be<s16> mSleepTimeMin;  /* was m108: stand (in Bk_move 020A5EF4): mTimers[1] = random in [it, mSleepTimeMax) once the BK_SLEEP animation starts; __sinit_d_a_bk_cpp 020A0364 sets the default 0x12C */
    /* 0x106 */ be<s16> mSleepTimeMax;  /* was m10A: stand (in Bk_move 020A5EF4): upper end of the random sleep time (mSleepTimeMin); __sinit_d_a_bk_cpp 020A0364 sets the default 0x258 */
    /* 0x108 */ be<u32> __vtbl;
};
WWHD_SIZE(bkHIO_c, 0x10C);
