/* cc_class (ChuChu), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c +0x11C; HD has no mDoExt_J3DModelPacketS m2DC (0x14), so the
 * members from mBehaviorType (GameCube 0x2F0) on are +0x108. Size 0xF10 (constructor 02110BB4;
 * GameCube 0xE04). */
#pragma once
#include "bindings.h"

#ifndef WWHD_ENEMYICE_FIRE_L
#define WWHD_ENEMYICE_FIRE_L
/* enemyice / enemyfire (c_damagereaction), WWHD layouts (SHARED-CANDIDATE) */
struct enemyice_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> mFreezeType;  /* was m00C: view of enemyice::mFreezeType (c/c_damagereaction.h) */
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 _016[2];
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
    /* 0x1B2 */ u8 _1B2[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(enemyice_l, 0x3B8);

struct enemyfire_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFireDuration;
    /* 0x006 */ be<s8> mMode;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<s16> mFireTimer;
    /* 0x00A */ u8 _00A[2];
    /* 0x00C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x010 */ be<s8> mFlameJntIdxs[10];
    /* 0x01A */ u8 _01A[2];
    /* 0x01C */ be<f32> mParticleScale[10];
    /* 0x044 */ be<s16> mFlameTimers[10];
    /* 0x058 */ be<u32> mpFlameEmitters[10];
    /* 0x080 */ cXyz mPrevPos;
    /* 0x08C */ cXyz mDirection;
    /* 0x098 */ be<f32> mFlameScaleY;
    /* 0x09C */ u8 _09C;
    /* 0x09D */ be<u8> mHitboxFlameIdx;
    /* 0x09E */ u8 _09E[2];
    /* 0x0A0 */ dCcD_Stts mStts;
    /* 0x0DC */ dCcD_Sph mSph;
    /* 0x208 */ u8 mLight[0x20];  /* LIGHT_INFLUENCE */
    /* 0x228 */ be<f32> mHD20;  /* was m228: mLight (0x208, a LIGHT_INFLUENCE of HD size 0x24, declared here as its first 0x20 bytes) + 0x20 = LIGHT_INFLUENCE::mHD20 (bindings.h / d/d_kankyo.h), the light's HD tail; the enemyfire constructors (pt 02446C68, ki 0219D368, ks 021AA2D8, cc 02110B28, oq 023C3C80, am 0204B6A0, rd 0245A984, nz 0230F104) set it to 1.0 as the LIGHT_INFLUENCE constructor does; no reader in WWHD, so it keeps the class name; HD: 1.0f in the constructor */
};
WWHD_SIZE(enemyfire_l, 0x22C);

#endif

struct cc_class {
    /* 0x000 */ fopEn_enemy_c actor;
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;  /* was m2B4: useHeapInit 02110528: McaMorf of BMD_CC (the body, nodeCallBack on its joints); anm_init 0210C5C8 sets its animation; daCC_Draw 0210BFEC draws it unless split / flattened */
    /* 0x3D4 */ gptr<mDoExt_brkAnm> mpColorBrk;  /* was m2B8: useHeapInit 02110528: BRK_CC on the body model; daCC_Draw 0210BFEC sets its frame to mColorType */
    /* 0x3D8 */ gptr<mDoExt_McaMorf> mpFlatMorf;  /* was m2BC: useHeapInit 02110528: McaMorf of BDL_CC_BETA; daCC_Draw 0210BFEC draws it instead of the body in mode CC_MODE_FLATTENED; action_dead_move (in daCC_Execute 0210DD3C) plays BCK_CC_BETA on it */
    /* 0x3DC */ gptr<mDoExt_brkAnm> mpFlatColorBrk;  /* was m2C0: useHeapInit 02110528: BRK_CC_BETA on the flat model; daCC_Draw 0210BFEC sets its frame to mColorType */
    /* 0x3E0 */ gptr<mDoExt_McaMorf> mpStoneMorf;  /* was m2C4: useHeapInit 02110528: McaMorf of BDL_CC_IWA (rock); draw_SUB 0210BB58 / daCC_Draw 0210BFEC place and draw it while mStoneState != 0 */
    /* 0x3E4 */ gptr<mDoExt_brkAnm> mpStoneBrk;  /* was m2C8: useHeapInit 02110528: BRK_CC_IWA01 on the rock model; daCC_Draw 0210BFEC uses it in mStoneState 1 at frame mStoneAnmFrame; action_up_check 02112414 rewinds it when the ChuChu turns to stone */
    /* 0x3E8 */ gptr<mDoExt_brkAnm> mpStoneRecoverBrk;  /* was m2CC: useHeapInit 02110528: BRK_CC_IWA02 on the rock model; daCC_Draw 0210BFEC uses it in mStoneState 2; action_up_check 02112414 rewinds it when the recovery starts */
    /* 0x3EC */ gptr<mDoExt_btkAnm> mpStoneBtk;  /* was m2D0: useHeapInit 02110528: BTK_CC_IWA01 on the rock model; daCC_Draw 0210BFEC uses it in mStoneState 1; action_up_check 02112414 rewinds it */
    /* 0x3F0 */ gptr<mDoExt_btkAnm> mpStoneRecoverBtk;  /* was m2D4: useHeapInit 02110528: BTK_CC_IWA02 on the rock model; daCC_Draw 0210BFEC uses it in mStoneState 2; action_up_check 02112414 rewinds it */
    /* 0x3F4 */ gptr<mDoExt_McaMorf> mpSplitMorf;  /* was m2D8: useHeapInit 02110528: McaMorf of BDL_CC_PTCL with BCK_CC_PTCL; drawn and played instead of the body while mIsSplit (action_damage_move mode 0x2D, in daCC_Execute 0210DD3C) */
    // HD: no mDoExt_J3DModelPacketS m2DC
    /* 0x3F8 */ be<u8> mBehaviorType;
    /* 0x3F9 */ be<u8> mColorType;
    /* 0x3FA */ be<u8> mNoticeRangeByte;
    /* 0x3FB */ be<u8> mDisableSpawnSwitchNo;
    /* 0x3FC */ be<u8> mCurrAction;
    /* 0x3FD */ be<u8> mMode;  /* was m2F5: step within mCurrAction: every action function switches on it (action_nomal_move .. action_tubo_search, daCC_Execute 0210DD3C); values are the CC_MODE_* constants (daCC_Create 02110D4C, action_tomaru 02112DA0) */
    /* 0x3FE */ be<u8> mDamageType;  /* was m2F6: body_atari_check 0210CE9C: reaction per attack type (0 none, 1 critical, 3 wind, 4 boomerang, 6 hammer, 7 hookshot, 8 fire/ice/light, 9 grappling hook); damage_mode_move 0210C6F4 2; action_damage_move (in daCC_Execute 0210DD3C) switches on it */
    /* 0x3FF */ be<u8> mElectricState;  /* was m2F7: denki_start 0210CBC0 sets 1 (blue / yellow only, At spark on), denki_end 0210CC00 clears it; daCC_Execute 0210DD3C: 1 starts the CCTHUNDER00/01 emitters and goes to 2, 2 keeps them on the BODY03 joint */
    /* 0x400 */ be<u8> mOnWaterSurface;  /* was m2F8: action_oyogu (in daCC_Execute 0210DD3C) sets 1 once the body is at the water height - 20; BG_check 0210DC8C then holds pos.y there; cleared on leaving the water (naraku_check 0210C828) or on a hit while swimming */
    /* 0x401 */ be<u8> mNoBgCheck;  /* was m2F9: BG_check 0210DC8C does nothing while it is set; daCC_Create 02110D4C sets 1, the actions clear it when the ChuChu starts to move (action_nomal_move, action_tubo_search 02112E74) */
    /* 0x402 */ be<u8> mAbyssFrames;  /* was m2FA: naraku_check 0210C828: +1 per frame on ground code 4; deletes the actor after 50 frames (or below y -500) */
    /* 0x403 */ be<u8> mDekuState;  /* was m2FB: behavior DEKU_TREE: draw_SUB 0210BB58 / daCC_Execute 0210DD3C keep the body on mDekuMtx while it is not 3; action_noboru 021116B8 clears it, waits for it to become non-zero (mode 0x3D), sets 3 when the ChuChu drops off and 4 when it is back */
    /* 0x404 */ be<u8> mComeDemoStep;  /* was m2FC: deku_come_demo (in action_noboru 021116B8): step of the CyuCyu_STR event (1..5, 0 when done); action_noboru sets 1 */
    /* 0x405 */ be<u8> mRetDemoStep;  /* was m2FD: deku_ret_demo (in action_noboru 021116B8): step of the CyuCyu_RET event (1..6, 0 when done); action_noboru sets 1, 2 */
    /* 0x406 */ be<u8> mStoneState;  /* was m2FE: action_up_check 02112414: 1 when the ChuChu turns to stone, 2 while it turns back, 0 when done; daCC_Draw 0210BFEC draws the rock model with the IWA01 / IWA02 animations by it; naraku_check 0210C828 sinks deeper and deletes it in water */
    /* 0x407 */ be<u8> mStoneAnmFrame;  /* was m2FF: daCC_Execute 0210DD3C: +1 per frame up to 25 while mStoneState is set; daCC_Draw 0210BFEC uses it as the frame of the rock brk/btk; action_up_check 02112414 resets it and ends the recovery at 25 */
    /* 0x408 */ be<u8> mInWater;  /* was m300: naraku_check 0210C828: set when the actor first enters water (water pillar, FALL_WATER sound, HAMON ripple), cleared with the ripple when it is out of water */
    /* 0x409 */ be<u8> mIsSplit;  /* was m301: action_damage_move (in daCC_Execute 0210DD3C) sets 1 in mode 0x2D (CCSPLIT particle, split model plays BCK_CC_PTCL) and clears it at the end; draw_SUB 0210BB58 / daCC_Draw 0210BFEC draw mpSplitMorf instead of the body while it is set */
    /* 0x40A */ be<u8> mBombShockDone;  /* was m302: shock_damage_check 0210CC58: set when a bomb within 1000 caused the damage (once per bomb), cleared when no bomb is near */
    /* 0x40B */ u8 _40B;
    /* 0x40C */ be<u8> mOtherHitState;  /* was m304: daCC_Create 02110D4C: 1 for colour param 15 (red, Tg also hit by other actors); body_atari_check 0210CE9C: 2 (health 0) when hit by an actor other than the player; dead_drop (in daCC_Execute 0210DD3C): 0 drops the IBALL item, 1 a heart / magic / arrow, 2 nothing */
    /* 0x40D */ u8 _40D;
    /* 0x40E */ be<s16> mAttackFrame;  /* was m306: action_attack_move (in daCC_Execute 0210DD3C): cleared at the attack start, +1 per frame in modes 0x1F..0x22; mBtNowFrame is 5 (parry window) while it is between REG_F(8,7) and 25 */
    /* 0x410 */ be<s16> mEventIdx;  /* was m308: deku_come_demo / action_noboru 021116B8: dComIfGp_evmng_getEventIdx("CyuCyu_STR" / "CyuCyu_RET"); fopAcM_orderOtherEventId and evmng_endCheck on it */
    /* 0x412 */ u8 _412[2];
    /* 0x414 */ be<u32> mPotId;  /* was m30C: tsubo_search 0210DBE0: process id of the TSUBO (pot) at the actor's position; action_tubo_search 02112E74 follows that pot (fopAcM_SearchByID), deletes the actor when there is none */
    /* 0x418 */ csXyz mSwingAngle;  /* was m310: nodeCallBack 0210B874 rotates body joints 0..BODY03 by it * jointNo / 2; damage_swing (in daCC_Execute 0210DD3C) sets x/z from the swing; cleared after a hit */
    /* 0x41E */ csXyz mGroundAngle;  /* was m316: BG_check 0210DC8C: fopAcM_getGroundAngle; draw_SUB 0210BB58 tilts the model by its x/z; daCC_Execute 0210DD3C clears it except on the deku tree */
    /* 0x424 */ be<s16> mWobbleAngle;  /* was m31C: draw_SUB 0210BB58: +1000 per frame; the 1.1/1.0/0.9 squash scale is applied rotated by it (0 and uniform mScale when climbing, flattened, split, stone or stunned) */
    /* 0x426 */ u8 _426[2];
    /* 0x428 */ be<s32> mAnmResIdx;  /* was m320: anm_init 0210C5C8 stores the BCK resource index it plays; action_noboru / action_oyogu / daCC_Execute 0210DD3C test it (TACHI_WALK, TACHI2HUSE, HUSE_WALK: lower Cyl height) */
    /* 0x42C */ be<f32> mWaterRiseSpeed;  /* was m324: action_oyogu (in daCC_Execute 0210DD3C): max step of pos.y towards the water height, eased up to 6; body_atari_check 0210CE9C clears it */
    /* 0x430 */ be<f32> mSwimBobY;  /* was m328: action_oyogu (in daCC_Execute 0210DD3C): eased to sin(mWavePhase)*3 - 15; draw_SUB 0210BB58 lowers the model by it */
    /* 0x434 */ be<f32> mSwingAmplitude;  /* was m32C: action_damage_move (in daCC_Execute 0210DD3C): 7000 / 16000 at a hit; damage_swing scales the sine swing by it and eases it to 0 by mSwingDecay; the damage ends below 1 */
    /* 0x438 */ be<f32> mSwingDecay;  /* was m330: action_damage_move (in daCC_Execute 0210DD3C): 250 / 500; damage_swing passes it as the step of cLib_addCalc0(mSwingAmplitude) */
    /* 0x43C */ be<f32> mScale;  /* was m334: draw_SUB 0210BB58: uniform model scale when not squashing; daCC_Create 02110D4C 1.0; action_noboru 021116B8 shrinks it to 0.1 on the way back into the deku tree */
    /* 0x440 */ be<f32> mCarryStartY;  /* was m338: action_up_check 02112414: pos.y when the stone is picked up; the shape turns towards angle.y only once the actor is 10 above it */
    /* 0x444 */ be<f32> mNoticeRange;
    /* 0x448 */ be<f32> mHomeRange;  /* was m340: daCC_Create 02110D4C: angle.z * 10 (at least 100); search_angle_set 0210DA18 (behavior FIXED) heads back to mHomePos when farther than it */
    /* 0x44C */ be<f32> mVoiceFrame;  /* was m344: action_noboru 021116B8: cM_rndF(43) per walk start; CV_CC_PARASITE plays when the body animation reaches that frame */
    /* 0x450 */ be<s16> mSwingPhase;  /* was m348: damage_swing (in daCC_Execute 0210DD3C): sine phase of the damage swing, +0x3000 per frame (not for wind); action_damage_move sets 0 / 10000 */
    /* 0x452 */ be<s16> mSwingDirY;  /* was m34A: action_damage_move (in daCC_Execute 0210DD3C): player angle + 0x8000 - shape_angle.y; damage_swing rotates the swing by it */
    /* 0x454 */ be<s16> mChaseAngleOffset;  /* was m34C: daCC_Create 02110D4C: random offset from the process id; search_angle_set 0210DA18 adds it to the angle towards the player */
    /* 0x456 */ be<s16> mTimers[7];  /* was m34E: daCC_Execute 0210DD3C counts all 7 down each frame; [0] move/attack waits, [2] stone shake, [3] spark sound, [4] stun / stone time, [5] deku demo, [6] switch delay (action_tomaru 02112DA0) */
    /* 0x464 */ be<s16> mWavePhase[5];  /* was m35C: only [0] is used: action_oyogu (in daCC_Execute 0210DD3C) +0x800 per frame for mSwimBobY, action_up_check 02112414 +20000 for the stone shake; all 5 cleared together */
    /* 0x46E */ u8 _46E[2];
    /* 0x470 */ dPa_followEcallBack mThunderEcallBack0;  /* was m368: cc_class_ct 02110BB4 constructs it; daCC_Execute 0210DD3C attaches ID_AK_JN_CCTHUNDER00 to it and moves its emitter with the BODY03 joint; denki_end 0210CC00 ends it */
    /* 0x484 */ dPa_followEcallBack mThunderEcallBack1;  /* was m37C: cc_class_ct 02110BB4 constructs it; daCC_Execute 0210DD3C attaches ID_AK_JN_CCTHUNDER01 to it and moves its emitter with the BODY03 joint; denki_end 0210CC00 ends it */
    /* 0x498 */ u8 mRippleEcallBack[0x14];  /* was m390: naraku_check 0210C828: the water ripple (HAMON) callback (dPa_rippleEcallBack, rate 0); cc_class_ct 02110BB4 constructs it; action_noboru 021116B8 ends it; dPa_rippleEcallBack */
    /* 0x4AC */ cXyz mThunderPos;  /* was m3A4: daCC_Execute 0210DD3C: translation of the BODY03 joint, the position of the CCTHUNDER00/01 particles */
    /* 0x4B8 */ u8 _4B8[0xA];
    /* 0x4C2 */ be<s16> mCenterRotZ;  /* was m3BA: nodeCallBack 0210B874 rotates the CENTER joint by it about Z; action_noboru 021116B8 -0x4000 on the deku tree, 0 when it drops off */
    /* 0x4C4 */ cXyz mHomePos;  /* was m3BC: daCC_Create 02110D4C copies current.pos; search_angle_set 0210DA18 (behavior FIXED) heads to it; naraku_check 0210C828 deletes 20000 below it; daCC_Execute 0210DD3C sets it from mDekuMtx on the deku tree */
    /* 0x4D0 */ u8 _4D0[0x18];
    /* 0x4E8 */ cXyz m3E0[6];
    /* 0x530 */ cXyz m428[6];
    /* 0x578 */ cXyz mBody03Pos;  /* was m470: nodeCallBack 0210B874: world position of the BODY03 joint; cc_eff_set 0210C364 uses it as the particle position */
    /* 0x584 */ dBgS_AcchCir mAcchCir;
    /* 0x5C4 */ dBgS_ObjAcch mAcch;
    /* 0x788 */ dCcD_Stts mStts;
    /* 0x7C4 */ dCcD_Cyl mCyl;
    /* 0x8F4 */ Mtx34 mDekuMtx;  /* was m7EC: daCC_Create 02110D4C / daCC_Execute 0210DD3C: base matrix of the body (and source of mHomePos) for behavior DEKU_TREE while mDekuState != 3; read only in this actor */
    /* 0x924 */ enemyice_l mEnemyIce;
    /* 0xCDC */ enemyfire_l mEnemyFire;
    /* 0xF08 */ u8 mInvisModel[8];  /* was mDFC: mDoExt_invisibleModel (cc_class_ct 02110BB4, useHeapInit 02110528 create on the body model, cc_class_dt 02113110); daCC_Draw 0210BFEC draws the frozen body through it (Cc_iceUpdateDL); mDoExt_invisibleModel */
};
WWHD_OFFSET(cc_class, mBehaviorType, 0x3F8);
WWHD_OFFSET(cc_class, mPotId, 0x414);
WWHD_OFFSET(cc_class, mAnmResIdx, 0x428);
WWHD_OFFSET(cc_class, mThunderEcallBack0, 0x470);
WWHD_OFFSET(cc_class, mCenterRotZ, 0x4C2);
WWHD_OFFSET(cc_class, mBody03Pos, 0x578);
WWHD_OFFSET(cc_class, mAcchCir, 0x584);
WWHD_OFFSET(cc_class, mStts, 0x788);
WWHD_OFFSET(cc_class, mCyl, 0x7C4);
WWHD_OFFSET(cc_class, mEnemyIce, 0x924);
WWHD_OFFSET(cc_class, mEnemyFire, 0xCDC);
WWHD_SIZE(cc_class, 0xF10);

// HD: private views used by the main unit; the shared actor layout above is unchanged.
template<class T> inline T* Cc_view(u32 address) { return gabi::at<T>(address); }
struct CcModel {
    u8 mPadding0[0x2C];
    /* 0x2C */ be<u32> mMatrixBlock;
    u8 mPadding30[0x7C];
    /* 0xAC */ be<u32> mModelData;
    u8 mPaddingB0[0x8];
    /* 0xB8 */ be<u32> mUserArea;
};
WWHD_OFFSET(CcModel, mMatrixBlock, 0x2C);
WWHD_OFFSET(CcModel, mModelData, 0xAC);
WWHD_OFFSET(CcModel, mUserArea, 0xB8);
WWHD_SIZE(CcModel, 0xBC);
struct CcMatrixBlock {
    u8 mPadding0[0x4];
    /* 0x4 */ be<u16> mFlags;
    u8 mPadding6[0xA];
    /* 0x10 */ be<u32> mMatrices;
};
WWHD_OFFSET(CcMatrixBlock, mFlags, 0x4);
WWHD_OFFSET(CcMatrixBlock, mMatrices, 0x10);
WWHD_SIZE(CcMatrixBlock, 0x14);
struct CcModelData {
    u8 mPadding0[0x4];
    /* 0x4 */ be<u32> mJointCount;
    /* 0x8 */ be<u32> mJoints;
    u8 mPaddingC[0x38];
    /* 0x44 */ be<u32> mTexMtxAnimation;
    /* 0x48 */ be<u32> mTevRegAnimation;
};
WWHD_OFFSET(CcModelData, mJointCount, 0x4);
WWHD_OFFSET(CcModelData, mJoints, 0x8);
WWHD_OFFSET(CcModelData, mTexMtxAnimation, 0x44);
WWHD_OFFSET(CcModelData, mTevRegAnimation, 0x48);
WWHD_SIZE(CcModelData, 0x4C);
struct CcJoint {
    u8 mPadding0[0x4];
    /* 0x4 */ be<u16> mJointNo;
    u8 mPadding6[0x2];
    /* 0x8 */ be<u32> mCallback;
    u8 mPaddingC[0x10];
};
WWHD_OFFSET(CcJoint, mJointNo, 0x4);
WWHD_OFFSET(CcJoint, mCallback, 0x8);
WWHD_SIZE(CcJoint, 0x1C);
struct CcJointTree {
    u8 mPadding0[0x8];
    /* 0x8 */ be<u16> mJointCount;
};
WWHD_OFFSET(CcJointTree, mJointCount, 0x8);
WWHD_SIZE(CcJointTree, 0xA);
struct CcAnimation {
    u8 mPadding0[0x4];
    /* 0x4 */ be<f32> mFrame;
};
WWHD_OFFSET(CcAnimation, mFrame, 0x4);
WWHD_SIZE(CcAnimation, 0x8);
struct CcParticle {
    u8 mPadding0[0xF4];
    /* 0xF4 */ GXColor mToonColor;
};
WWHD_OFFSET(CcParticle, mToonColor, 0xF4);
WWHD_SIZE(CcParticle, 0xF8);
struct CcEmitter {
    u8 mPadding0[0x244];
    /* 0x244 */ GXColor mPrmColor;
    /* 0x248 */ GXColor mEnvColor;
};
WWHD_OFFSET(CcEmitter, mPrmColor, 0x244);
WWHD_OFFSET(CcEmitter, mEnvColor, 0x248);
WWHD_SIZE(CcEmitter, 0x24C);
struct CcRipple {
    u8 mPadding0[0x10];
    /* 0x10 */ be<f32> mRate;
};
WWHD_OFFSET(CcRipple, mRate, 0x10);
WWHD_SIZE(CcRipple, 0x14);
struct CcPlayer {
    u8 mPadding0[0x3C0];
    /* 0x3C0 */ be<u32> mNoResetFlg2;
    u8 mPadding3C4[0x20];
    /* 0x3E4 */ cXyz mSwordTopPos;
};
WWHD_OFFSET(CcPlayer, mNoResetFlg2, 0x3C0);
WWHD_OFFSET(CcPlayer, mSwordTopPos, 0x3E4);
WWHD_SIZE(CcPlayer, 0x3F0);
struct CcAcchVtables {
    u8 mPadding0[0x10];
    /* 0x10 */ be<u32> mCheckVtable;
    /* 0x14 */ be<u32> mPassVtable;
    /* 0x18 */ be<u8> mObjectPass;
    u8 mPadding19[0x7];
    /* 0x20 */ be<u32> mObjectVtable;
};
WWHD_OFFSET(CcAcchVtables, mCheckVtable, 0x10);
WWHD_OFFSET(CcAcchVtables, mPassVtable, 0x14);
WWHD_OFFSET(CcAcchVtables, mObjectPass, 0x18);
WWHD_OFFSET(CcAcchVtables, mObjectVtable, 0x20);
WWHD_SIZE(CcAcchVtables, 0x24);
void Cc_enemyFire(enemyfire_l*);
void Cc_enemyFireRemove(enemyfire_l*);
void Cc_iceUpdateDL(mDoExt_McaMorf*, s32, void*);
BOOL Cc_bombCheckState(void*, s32);
BOOL Cc_bomb2CheckExplode(void*);
BOOL Cc_checkPolySafe(dBgS*, void*);
s32 Cc_getGroundCode(dBgS*, void*);
void Cc_rippleRemove(void*);
BOOL Cc_detectLight(u32, cXyz*);
void* Cc_brkCtor(void*);
void* Cc_btkCtor(void*);
BOOL Cc_brkInit(void*, J3DModelData*, void*, s32, s32, s16, s16, bool, s32, f32);
BOOL Cc_btkInit(void*, J3DModelData*, void*, s32, s32, s16, s16, bool, s32, f32);
void* Cc_jointTree(J3DModelData*);
bool Cc_invisibleCreate(void*, J3DModel*);
void Cc_sphereCtor(dCcD_Sph*);
void Cc_acchCtor(dBgS_ObjAcch*);
void Cc_rippleCtor(void*);
void Cc_acchCirCtor(dBgS_AcchCir*);
void Cc_invisibleCtor(void*);
s32 Cc_getNameIndex(u32, const char*, s32);
void Cc_acchDtor(dBgS_ObjAcch*, s32);
void Cc_invisibleDtor(void*, s32);
void Cc_sphereDtor(dCcD_Sph*, s32);
void Cc_circleDtor(void*, s32);
void Cc_actorDtor(void*, s32);
enum CcAnimationKind { CC_ANM_BRK, CC_ANM_BTK };

// HD: the checked cylinder source descriptor is 0x44 bytes, as in GC.
struct CcCylinderSource {
    /* 0x0 */ be<u32> mFlags;
    /* 0x4 */ be<u32> mAtType;
    /* 0x8 */ be<u32> mAtPowerPacked;
    /* 0xC */ be<u32> mAtSPrm;
    /* 0x10 */ be<u32> mTgType;
    /* 0x14 */ be<u32> mTgSPrm;
    /* 0x18 */ be<u32> mCoSPrm;
    /* 0x1C */ be<u32> mGAtAttributes;
    /* 0x20 */ be<u32> mGAtSPrm;
    /* 0x24 */ be<u32> mGTgAttributes;
    /* 0x28 */ be<u32> mGTgSPrm;
    /* 0x2C */ be<u32> mGCoSPrm;
    /* 0x30 */ be<f32> mCenterX;
    /* 0x34 */ be<f32> mCenterY;
    /* 0x38 */ be<f32> mCenterZ;
    /* 0x3C */ be<f32> mRadius;
    /* 0x40 */ be<f32> mHeight;
};
WWHD_OFFSET(CcCylinderSource, mFlags, 0x0);
WWHD_OFFSET(CcCylinderSource, mAtType, 0x4);
WWHD_OFFSET(CcCylinderSource, mAtPowerPacked, 0x8);
WWHD_OFFSET(CcCylinderSource, mAtSPrm, 0xC);
WWHD_OFFSET(CcCylinderSource, mTgType, 0x10);
WWHD_OFFSET(CcCylinderSource, mTgSPrm, 0x14);
WWHD_OFFSET(CcCylinderSource, mCoSPrm, 0x18);
WWHD_OFFSET(CcCylinderSource, mGAtAttributes, 0x1C);
WWHD_OFFSET(CcCylinderSource, mGAtSPrm, 0x20);
WWHD_OFFSET(CcCylinderSource, mGTgAttributes, 0x24);
WWHD_OFFSET(CcCylinderSource, mGTgSPrm, 0x28);
WWHD_OFFSET(CcCylinderSource, mGCoSPrm, 0x2C);
WWHD_OFFSET(CcCylinderSource, mCenterX, 0x30);
WWHD_OFFSET(CcCylinderSource, mCenterY, 0x34);
WWHD_OFFSET(CcCylinderSource, mCenterZ, 0x38);
WWHD_OFFSET(CcCylinderSource, mRadius, 0x3C);
WWHD_OFFSET(CcCylinderSource, mHeight, 0x40);
WWHD_SIZE(CcCylinderSource, 0x44);
namespace CcResources {
enum CcResource_e {
    dRes_INDEX_CC_BMD_CC_e = 0x2A,
    dRes_INDEX_CC_BRK_CC_e = 0x2D,
    dRes_INDEX_CC_BDL_CC_IWA_e = 0x26,
    dRes_INDEX_CC_BRK_CC_IWA01_e = 0x2F,
    dRes_INDEX_CC_BRK_CC_IWA02_e = 0x30,
    dRes_INDEX_CC_BTK_CC_IWA01_e = 0x33,
    dRes_INDEX_CC_BTK_CC_IWA02_e = 0x34,
    dRes_INDEX_CC_BDL_CC_BETA_e = 0x25,
    dRes_INDEX_CC_BRK_CC_BETA_e = 0x2E,
    dRes_INDEX_CC_BDL_CC_PTCL_e = 0x27,
    dRes_INDEX_CC_BCK_CC_PTCL_e = 0x18,
    dRes_INDEX_CC_BCK_HUSE2TACHI_e = 0x1B,
    dRes_INDEX_CC_BCK_START_e = 0x1E,
};
}

enum { CC_GREEN, CC_RED, CC_BLUE, CC_BLACK, CC_YELLOW };
enum { CC_ACTION_NORMAL, CC_ACTION_SWIM, CC_ACTION_ATTACK, CC_ACTION_DAMAGE, CC_ACTION_DEAD, CC_ACTION_CLIMB, CC_ACTION_STONE, CC_ACTION_WAIT_SWITCH, CC_ACTION_POT };
enum { CC_BEHAVIOR_GROUND, CC_BEHAVIOR_CEILING, CC_BEHAVIOR_DEKU_TREE, CC_BEHAVIOR_FIXED, CC_BEHAVIOR_POT };
enum { CC_MODE_WAIT = 0, CC_MODE_STAND_END = 11, CC_MODE_SWIM_START = 20, CC_MODE_DAMAGE_START = 40, CC_MODE_SPLIT_START = 45, CC_MODE_SPLIT_WAIT = 46, CC_MODE_DEAD_START = 50, CC_MODE_FLATTENED = 53, CC_MODE_CLIMB_START = 60, CC_MODE_STONE_START = 80, CC_MODE_WAIT_SWITCH = 100, CC_MODE_WAIT_SWITCH_DELAY = 101, CC_MODE_POT_SEARCH = 110, CC_MODE_POT_WAIT = 111, CC_MODE_POT_EMERGE = 112 };
enum { CC_PARTICLE_HEAD = 0x3F0, CC_PARTICLE_FOOT = 0x3EF };
