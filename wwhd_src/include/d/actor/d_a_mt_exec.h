/**
 * Magtail execution's private WWHD layouts and bindings.
 * HD: tww has placeholder layouts and helper bodies; projections retain the verified HD data.
 */
#pragma once
#include "d/actor/d_a_mt.h"
namespace MtExec {
struct AttackInfo {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz mHitAngle;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ gptr<cXyz> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_OFFSET(AttackInfo, mpObj, 0x00);
WWHD_OFFSET(AttackInfo, mpActor, 0x04);
WWHD_OFFSET(AttackInfo, mDamage, 0x08);
WWHD_OFFSET(AttackInfo, mbDead, 0x09);
WWHD_OFFSET(AttackInfo, mResultingAttackType, 0x0A);
WWHD_OFFSET(AttackInfo, _0B, 0x0B);
WWHD_OFFSET(AttackInfo, mHitAngle, 0x0C);
WWHD_OFFSET(AttackInfo, mPlCutBit, 0x12);
WWHD_OFFSET(AttackInfo, pParticlePos, 0x14);
WWHD_OFFSET(AttackInfo, mHitSoundId, 0x18);
WWHD_SIZE(AttackInfo, 0x1C);
struct LavaGroundCheck {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> mCheckVtable;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> mPolyInfoVtable;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> mPolyPassVtable;
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> mGroupPassVtable;
    /* 0x50 */ be<u32> mGrp;
};
WWHD_OFFSET(LavaGroundCheck, mpPolyPassChk, 0x00);
WWHD_OFFSET(LavaGroundCheck, mpGrpPassChk, 0x04);
WWHD_OFFSET(LavaGroundCheck, _08, 0x08);
WWHD_OFFSET(LavaGroundCheck, mCheckVtable, 0x10);
WWHD_OFFSET(LavaGroundCheck, _14, 0x14);
WWHD_OFFSET(LavaGroundCheck, mPolyInfoVtable, 0x20);
WWHD_OFFSET(LavaGroundCheck, m_pos, 0x24);
WWHD_OFFSET(LavaGroundCheck, _30, 0x30);
WWHD_OFFSET(LavaGroundCheck, mPolyPassVtable, 0x40);
WWHD_OFFSET(LavaGroundCheck, mPass, 0x44);
WWHD_OFFSET(LavaGroundCheck, _4B, 0x4B);
WWHD_OFFSET(LavaGroundCheck, mGroupPassVtable, 0x4C);
WWHD_OFFSET(LavaGroundCheck, mGrp, 0x50);
WWHD_SIZE(LavaGroundCheck, 0x54);
struct SegmentVectors { cXyz v[6]; };
struct FightAngles { be<s16> v[5]; };
WWHD_SIZE(SegmentVectors,0x48); WWHD_SIZE(FightAngles,10);
}
using MtExec::AttackInfo;
using MtExec::LavaGroundCheck;
using MtExec::SegmentVectors;
using MtExec::FightAngles;
void mtExec_atPower(AttackInfo* info);
u32 mtExec_atCheck(fopAc_ac_c* actor, AttackInfo* info);
void mtExec_defenceSound(fopAc_ac_c* actor, u32 obj, u32 material);
BOOL mtExec_enemyIce(enemyice_l* ice);
void mtExec_createMpillar(cXyz* pos, f32 scale);
void mtExec_XrotS(Mtx34* mtx, s16 angle);
void* mtExec_search(u32 callback, void* data);
BOOL mtExec_lockonTruth(dAttention_c* attention);
fopAc_ac_c* mtExec_actionTarget(dAttention_c* attention, s32 index);
void mtExec_onEnemyDamage();
void mtExec_bgmHitSound(s32 id);
void mtExec_swordFlush(cXyz* pos, s32 mode);
void mtExec_addAngles(const csXyz* a, const csXyz* b);
f32 mtExec_sinShort(void* table, s16 angle);
f32 mtExec_cosShort(void* table, s16 angle);
void mtExec_seStart(fopAc_ac_c* actor, u32 id, u32 param);
void mtExec_monsSeStart(fopAc_ac_c* actor, u32 id, u32 param);
void mtExec_groundCtor(LavaGroundCheck* check);
void mtExec_groundDtor(LavaGroundCheck* check, s32 flag);
void mtExec_monsSe(u32 id, cXyz* pos, u32 pid, s32 mode, s32 reverb);

namespace MtExec {
struct Actor : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<u8> mType;
    /* 0x3D1 */ be<u8> mSnapTurn;
    /* 0x3D2 */ be<u8> mUseAngleParam;
    /* 0x3D3 */ be<u8> mPathIndex;
    /* 0x3D4 */ be<u8> mSwitch;
    /* 0x3D5 */ be<u8> mDeathSwitch;
    /* 0x3D6 */ be<u8> mAngleParam;
    /* 0x3D7 */ be<u8> mAppearSwitch;
    /* 0x3D8 */ be<u8> mHasPath;
    /* 0x3D9 */ be<u8> mPathPoint;
    /* 0x3DA */ be<u8> mPathDirection;
    /* 0x3DB */ u8 _3DB;
    /* 0x3DC */ gptr<dPath> ppd;
    /* 0x3E0 */ gptr<mDoExt_McaMorf> mpMorf[MT_PART_NUM];
    /* 0x400 */ be<u8> mMaterialMode;
    /* 0x401 */ u8 _401[3];
    /* 0x404 */ be<s32> mBrkFrame;
    /* 0x408 */ be<s32> mBtkFrame;
    /* 0x40C */ gptr<mDoExt_btkAnm> mpBtk[MT_PART_NUM];
    /* 0x42C */ gptr<mDoExt_brkAnm> mpBrk[MT_PART_NUM];
    /* 0x44C */ be<f32> mBodyWaveAmplitude;
    /* 0x450 */ be<s16> mJointWaveStep;
    /* 0x452 */ u8 _452[2];
    /* 0x454 */ be<f32> m454;
    /* 0x458 */ gptr<J3DAnmTexPattern> mpBtpRes;
    /* 0x45C */ gptr<mDoExt_btpAnm> mpBtp;
    /* 0x460 */ be<u8> mBtpFrame;
    /* 0x461 */ be<u8> mBtpOn;
    /* 0x462 */ be<u8> mBtpMaxFrame;
    /* 0x463 */ u8 _463;
    /* 0x464 */ be<u8> mWaterSoundPlayed;
    /* 0x465 */ u8 _465;
    /* 0x466 */ be<s16> mSteamTimer;
    /* 0x468 */ be<u8> mFollowParticlesStarted;
    /* 0x469 */ u8 _469[3];
    /* 0x46C */ cXyz mParticlePos[MT_PART_NUM];
    /* 0x4CC */ dPa_followEcallBack mFollowCb[MT_PART_NUM];
    /* 0x56C */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x570 */ be<u8> mAction;
    /* 0x571 */ be<u8> mMode;
    /* 0x572 */ be<s16> mActionTimer;
    /* 0x574 */ be<s16> mPauseTimer;
    /* 0x576 */ be<s16> mDeleteTimer;
    /* 0x578 */ be<s16> mVoiceTimer;
    /* 0x57A */ be<s16> mMoveSoundTimer;
    /* 0x57C */ be<s16> mInvulnerableTimer;
    /* 0x57E */ be<s16> mShakeTimer;
    /* 0x580 */ be<s16> mBlinkTimer;
    /* 0x582 */ be<s16> mRollTimer;
    /* 0x584 */ be<s16> mHeadWobbleAngle;
    /* 0x586 */ be<s16> mFrameCounter;
    /* 0x588 */ be<s16> mWallLostTimer;
    /* 0x58A */ be<s16> mJointWavePhase;
    /* 0x58C */ be<f32> mBodyOffset;
    /* 0x590 */ be<f32> mWobbleAmplitude;
    /* 0x594 */ be<f32> mRollBodyOffset;
    /* 0x598 */ cXyz mTargetPos;
    /* 0x5A4 */ be<s16> mTurnSpeed;
    /* 0x5A6 */ be<s16> mRollPitchStep;
    /* 0x5A8 */ be<s16> mWallAngle;
    /* 0x5AA */ be<s16> mWavePauseTimer;
    /* 0x5AC */ be<s16> mWaveExtent;
    /* 0x5AE */ be<s16> mClimbTimer;
    /* 0x5B0 */ be<s16> mPlayerPitch;
    /* 0x5B2 */ be<s16> mPlayerYaw;
    /* 0x5B4 */ u8 _5B4[2];
    /* 0x5B6 */ csXyz mShakeAngle;
    /* 0x5BC */ cXyz mSegmentPos[MT_PART_NUM];
    /* 0x61C */ cXyz mOldSegmentPos[MT_PART_NUM];
    /* 0x67C */ csXyz mSegmentAngle[MT_PART_NUM];
    /* 0x6AC */ cXyz mScatterSpeed[MT_PART_NUM];
    /* 0x70C */ be<s16> mSegmentPhase[MT_PART_NUM];
    /* 0x71C */ be<f32> mSegmentScale[MT_PART_NUM];
    /* 0x73C */ be<f32> mSegmentScaleY[MT_PART_NUM];
    /* 0x75C */ csXyz mJointAnglesLeft[15];
    /* 0x7B6 */ csXyz mJointAnglesRight[15];
    /* 0x810 */ cXyz mTrailPos[MT_JOINT_NUM];
    /* 0xB10 */ csXyz mTrailAngle[MT_JOINT_NUM];
    /* 0xC90 */ be<s16> mTrailHeadAngle[MT_JOINT_NUM];
    /* 0xD10 */ be<s32> mTrailIndex;
    /* 0xD14 */ u8 _D14[0xD1C - 0xD14];
    /* 0xD1C */ be<u8> mFastTrail;
    /* 0xD1D */ be<u8> mStoreTrail;
    /* 0xD1E */ be<s16> mAttackFrame;
    /* 0xD20 */ be<u8> mHeadColliderMode;
    /* 0xD21 */ u8 _D21[3];
    /* 0xD24 */ dBgS_AcchCir mAcchCir;
    /* 0xD64 */ dBgS_ObjAcch mAcch;
    /* 0xF28 */ dCcD_Stts mStts;
    /* 0xF64 */ dCcD_Sph mSph[MT_PART_NUM];
    /* 0x18C4 */ dCcD_Sph mAtSph;
    /* 0x19F0 */ be<u8> mRubbleFrame;
    /* 0x19F1 */ u8 _19F1[3];
    /* 0x19F4 */ gptr<J3DModel> mpBrModelA[3];
    /* 0x1A00 */ gptr<J3DModel> mpBrModelB[3];
    /* 0x1A0C */ be<f32> mHeadScale;
    /* 0x1A10 */ be<f32> mKnockbackRate;
    /* 0x1A14 */ be<s8> mDamageMode;
    /* 0x1A15 */ be<u8> mPlayerOccluded;
    /* 0x1A16 */ be<u8> mInLava;
    /* 0x1A17 */ be<u8> mHeadHealth;
    /* 0x1A18 */ be<s16> mHitEffectTimer;
    /* 0x1A1A */ u8 _1A1A[2];
    /* 0x1A1C */ enemyice_l mEnemyIce;
    /* 0x1DD4 */ be<s8> mDeleteRequested;
    /* 0x1DD5 */ u8 _1DD5[3];
};
WWHD_OFFSET(Actor,mPhs,0x3C8);
WWHD_OFFSET(Actor,mType,0x3D0);
WWHD_OFFSET(Actor,mSnapTurn,0x3D1);
WWHD_OFFSET(Actor,mUseAngleParam,0x3D2);
WWHD_OFFSET(Actor,mPathIndex,0x3D3);
WWHD_OFFSET(Actor,mSwitch,0x3D4);
WWHD_OFFSET(Actor,mDeathSwitch,0x3D5);
WWHD_OFFSET(Actor,mAngleParam,0x3D6);
WWHD_OFFSET(Actor,mAppearSwitch,0x3D7);
WWHD_OFFSET(Actor,mHasPath,0x3D8);
WWHD_OFFSET(Actor,mPathPoint,0x3D9);
WWHD_OFFSET(Actor,mPathDirection,0x3DA);
WWHD_OFFSET(Actor,_3DB,0x3DB);
WWHD_OFFSET(Actor,ppd,0x3DC);
WWHD_OFFSET(Actor,mpMorf,0x3E0);
WWHD_OFFSET(Actor,mMaterialMode,0x400);
WWHD_OFFSET(Actor,_401,0x401);
WWHD_OFFSET(Actor,mBrkFrame,0x404);
WWHD_OFFSET(Actor,mBtkFrame,0x408);
WWHD_OFFSET(Actor,mpBtk,0x40C);
WWHD_OFFSET(Actor,mpBrk,0x42C);
WWHD_OFFSET(Actor,mBodyWaveAmplitude,0x44C);
WWHD_OFFSET(Actor,mJointWaveStep,0x450);
WWHD_OFFSET(Actor,_452,0x452);
WWHD_OFFSET(Actor,m454,0x454);
WWHD_OFFSET(Actor,mpBtpRes,0x458);
WWHD_OFFSET(Actor,mpBtp,0x45C);
WWHD_OFFSET(Actor,mBtpFrame,0x460);
WWHD_OFFSET(Actor,mBtpOn,0x461);
WWHD_OFFSET(Actor,mBtpMaxFrame,0x462);
WWHD_OFFSET(Actor,_463,0x463);
WWHD_OFFSET(Actor,mWaterSoundPlayed,0x464);
WWHD_OFFSET(Actor,_465,0x465);
WWHD_OFFSET(Actor,mSteamTimer,0x466);
WWHD_OFFSET(Actor,mFollowParticlesStarted,0x468);
WWHD_OFFSET(Actor,_469,0x469);
WWHD_OFFSET(Actor,mParticlePos,0x46C);
WWHD_OFFSET(Actor,mFollowCb,0x4CC);
WWHD_OFFSET(Actor,mpEmitter,0x56C);
WWHD_OFFSET(Actor,mAction,0x570);
WWHD_OFFSET(Actor,mMode,0x571);
WWHD_OFFSET(Actor,mActionTimer,0x572);
WWHD_OFFSET(Actor,mPauseTimer,0x574);
WWHD_OFFSET(Actor,mDeleteTimer,0x576);
WWHD_OFFSET(Actor,mVoiceTimer,0x578);
WWHD_OFFSET(Actor,mMoveSoundTimer,0x57A);
WWHD_OFFSET(Actor,mInvulnerableTimer,0x57C);
WWHD_OFFSET(Actor,mShakeTimer,0x57E);
WWHD_OFFSET(Actor,mBlinkTimer,0x580);
WWHD_OFFSET(Actor,mRollTimer,0x582);
WWHD_OFFSET(Actor,mHeadWobbleAngle,0x584);
WWHD_OFFSET(Actor,mFrameCounter,0x586);
WWHD_OFFSET(Actor,mWallLostTimer,0x588);
WWHD_OFFSET(Actor,mJointWavePhase,0x58A);
WWHD_OFFSET(Actor,mBodyOffset,0x58C);
WWHD_OFFSET(Actor,mWobbleAmplitude,0x590);
WWHD_OFFSET(Actor,mRollBodyOffset,0x594);
WWHD_OFFSET(Actor,mTargetPos,0x598);
WWHD_OFFSET(Actor,mTurnSpeed,0x5A4);
WWHD_OFFSET(Actor,mRollPitchStep,0x5A6);
WWHD_OFFSET(Actor,mWallAngle,0x5A8);
WWHD_OFFSET(Actor,mWavePauseTimer,0x5AA);
WWHD_OFFSET(Actor,mWaveExtent,0x5AC);
WWHD_OFFSET(Actor,mClimbTimer,0x5AE);
WWHD_OFFSET(Actor,mPlayerPitch,0x5B0);
WWHD_OFFSET(Actor,mPlayerYaw,0x5B2);
WWHD_OFFSET(Actor,_5B4,0x5B4);
WWHD_OFFSET(Actor,mShakeAngle,0x5B6);
WWHD_OFFSET(Actor,mSegmentPos,0x5BC);
WWHD_OFFSET(Actor,mOldSegmentPos,0x61C);
WWHD_OFFSET(Actor,mSegmentAngle,0x67C);
WWHD_OFFSET(Actor,mScatterSpeed,0x6AC);
WWHD_OFFSET(Actor,mSegmentPhase,0x70C);
WWHD_OFFSET(Actor,mSegmentScale,0x71C);
WWHD_OFFSET(Actor,mSegmentScaleY,0x73C);
WWHD_OFFSET(Actor,mJointAnglesLeft,0x75C);
WWHD_OFFSET(Actor,mJointAnglesRight,0x7B6);
WWHD_OFFSET(Actor,mTrailPos,0x810);
WWHD_OFFSET(Actor,mTrailAngle,0xB10);
WWHD_OFFSET(Actor,mTrailHeadAngle,0xC90);
WWHD_OFFSET(Actor,mTrailIndex,0xD10);
WWHD_OFFSET(Actor,_D14,0xD14);
WWHD_OFFSET(Actor,mFastTrail,0xD1C);
WWHD_OFFSET(Actor,mStoreTrail,0xD1D);
WWHD_OFFSET(Actor,mAttackFrame,0xD1E);
WWHD_OFFSET(Actor,mHeadColliderMode,0xD20);
WWHD_OFFSET(Actor,_D21,0xD21);
WWHD_OFFSET(Actor,mAcchCir,0xD24);
WWHD_OFFSET(Actor,mAcch,0xD64);
WWHD_OFFSET(Actor,mStts,0xF28);
WWHD_OFFSET(Actor,mSph,0xF64);
WWHD_OFFSET(Actor,mAtSph,0x18C4);
WWHD_OFFSET(Actor,mRubbleFrame,0x19F0);
WWHD_OFFSET(Actor,_19F1,0x19F1);
WWHD_OFFSET(Actor,mpBrModelA,0x19F4);
WWHD_OFFSET(Actor,mpBrModelB,0x1A00);
WWHD_OFFSET(Actor,mHeadScale,0x1A0C);
WWHD_OFFSET(Actor,mKnockbackRate,0x1A10);
WWHD_OFFSET(Actor,mDamageMode,0x1A14);
WWHD_OFFSET(Actor,mPlayerOccluded,0x1A15);
WWHD_OFFSET(Actor,mInLava,0x1A16);
WWHD_OFFSET(Actor,mHeadHealth,0x1A17);
WWHD_OFFSET(Actor,mHitEffectTimer,0x1A18);
WWHD_OFFSET(Actor,_1A1A,0x1A1A);
WWHD_OFFSET(Actor,mEnemyIce,0x1A1C);
WWHD_OFFSET(Actor,mDeleteRequested,0x1DD4);
WWHD_OFFSET(Actor,_1DD5,0x1DD5);
WWHD_SIZE(Actor,0x1DD8);
inline mt_class* originalActor(Actor* actor) { return reinterpret_cast<mt_class*>(actor); }
inline void anm_init(Actor* a,int bck,f32 blend,u8 mode,f32 speed,int sound) { ::anm_init(originalActor(a),bck,blend,mode,speed,sound); }
inline void tex_anm_set(Actor* a,u16 index) { ::tex_anm_set(originalActor(a),index); }
inline void mt_bg_check(Actor* a) { ::mt_bg_check(originalActor(a)); }
inline void body_wall_check(Actor* a) { ::body_wall_check(originalActor(a)); }
inline void bakuha(Actor* a) { ::bakuha(originalActor(a)); }
inline void water_damage_se_set(Actor* a) { ::water_damage_se_set(originalActor(a)); }
inline void body_control2(Actor* a) { ::body_control2(originalActor(a)); }
inline void body_control3(Actor* a) { ::body_control3(originalActor(a)); }
inline void body_control4(Actor* a) { ::body_control4(originalActor(a)); }
inline void body_control5(Actor* a) { ::body_control5(originalActor(a)); }
inline void mt_move_maru(Actor* a) { ::mt_move_maru(originalActor(a)); }
}

namespace MtExec {
struct HIO {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ be<u8> mDisablePauses;
    /* 0x06 */ be<u8> mDebugMode;
    /* 0x07 */ be<u8> mHideParticles;
    /* 0x08 */ be<s16> mJointWaveSpeed;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ be<f32> mJointBendBase;
    /* 0x10 */ be<s16> mActionDuration;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> mAttackChance;
    /* 0x18 */ be<f32> mHoverHeight;
    /* 0x1C */ be<f32> mHeadScale;            /* Draw: rubble model scale */
    /* 0x20 */ be<f32> mUnusedScale;
    /* 0x24 */ be<f32> mAttackDistance;
    /* 0x28 */ be<f32> mCircleDistance;
    /* 0x2C */ be<f32> mLungeSpeed;
    /* 0x30 */ be<s16> mAttackStartFrame;
    /* 0x32 */ be<s16> mAttackEndFrame;
    /* 0x34 */ be<f32> mAttackDistanceLimit;
    /* 0x38 */ be<s16> mLungeTargetFrame;
    /* 0x3A */ be<s16> mCircleReturnFrame;
    /* 0x3C */ be<s16> mBiteAnimationFrame;
    /* 0x3E */ be<s16> mCloseAnimationFrame;
    /* 0x40 */ be<f32> mUnusedDistance;
    /* 0x44 */ be<f32> mHeadRadius;
    /* 0x48 */ be<f32> mBodyRadius;
    /* 0x4C */ be<f32> mKnockbackScale;
    /* 0x50 */ be<s16> mMaterialFrameStep;            /* Draw: per-segment btk/brk frame step */
    /* 0x52 */ be<s16> mUnusedTimer;
    /* 0x54 */ be<s16> mRollDuration;
    /* 0x56 */ u8 _56[2];
    /* 0x58 */ be<f32> mUnusedRate;
    /* 0x5C */ be<f32> mUnusedSpeed;
};
WWHD_OFFSET(HIO, __vtbl, 0x00);
WWHD_OFFSET(HIO, mNo, 0x04);
WWHD_OFFSET(HIO, mDisablePauses, 0x05);
WWHD_OFFSET(HIO, mDebugMode, 0x06);
WWHD_OFFSET(HIO, mHideParticles, 0x07);
WWHD_OFFSET(HIO, mJointWaveSpeed, 0x08);
WWHD_OFFSET(HIO, _0A, 0x0A);
WWHD_OFFSET(HIO, mJointBendBase, 0x0C);
WWHD_OFFSET(HIO, mActionDuration, 0x10);
WWHD_OFFSET(HIO, _12, 0x12);
WWHD_OFFSET(HIO, mAttackChance, 0x14);
WWHD_OFFSET(HIO, mHoverHeight, 0x18);
WWHD_OFFSET(HIO, mHeadScale, 0x1C);
WWHD_OFFSET(HIO, mUnusedScale, 0x20);
WWHD_OFFSET(HIO, mAttackDistance, 0x24);
WWHD_OFFSET(HIO, mCircleDistance, 0x28);
WWHD_OFFSET(HIO, mLungeSpeed, 0x2C);
WWHD_OFFSET(HIO, mAttackStartFrame, 0x30);
WWHD_OFFSET(HIO, mAttackEndFrame, 0x32);
WWHD_OFFSET(HIO, mAttackDistanceLimit, 0x34);
WWHD_OFFSET(HIO, mLungeTargetFrame, 0x38);
WWHD_OFFSET(HIO, mCircleReturnFrame, 0x3A);
WWHD_OFFSET(HIO, mBiteAnimationFrame, 0x3C);
WWHD_OFFSET(HIO, mCloseAnimationFrame, 0x3E);
WWHD_OFFSET(HIO, mUnusedDistance, 0x40);
WWHD_OFFSET(HIO, mHeadRadius, 0x44);
WWHD_OFFSET(HIO, mBodyRadius, 0x48);
WWHD_OFFSET(HIO, mKnockbackScale, 0x4C);
WWHD_OFFSET(HIO, mMaterialFrameStep, 0x50);
WWHD_OFFSET(HIO, mUnusedTimer, 0x52);
WWHD_OFFSET(HIO, mRollDuration, 0x54);
WWHD_OFFSET(HIO, _56, 0x56);
WWHD_OFFSET(HIO, mUnusedRate, 0x58);
WWHD_OFFSET(HIO, mUnusedSpeed, 0x5C);
WWHD_SIZE(HIO,0x60);
inline HIO& hostIO() { return *reinterpret_cast<HIO*>(&l_HIO()); }
}

namespace MtExec {
struct CheckOffsets { be<f32> values[6]; }; WWHD_SIZE(CheckOffsets,24);
struct CheckBits { be<u8> values[6]; }; WWHD_SIZE(CheckBits,6);
struct TrailOffsets { be<s32> values[8]; }; WWHD_SIZE(TrailOffsets,32);
struct PathPoint { be<u8> mArg0,mArg1,mArg2,mArg3; cXyz m_position; }; WWHD_OFFSET(PathPoint,m_position,4); WWHD_SIZE(PathPoint,0x10);
struct Path { union { be<u16> m_num; be<u8> mCountBytes[2]; }; be<u16> m_nextID; be<u8> mArg0,m_closed,mArg2,mArg3; gptr<PathPoint> m_points; }; WWHD_OFFSET(Path,m_points,8); WWHD_SIZE(Path,12);
struct Attention { u8 padding0[0x20]; be<u32> mFlags; }; WWHD_OFFSET(Attention,mFlags,0x20); WWHD_SIZE(Attention,0x24);
struct Player { u8 padding0[0x3AC]; be<u8> mCutType; }; WWHD_OFFSET(Player,mCutType,0x3AC); WWHD_SIZE(Player,0x3AD);
struct PlayerVtable { u8 padding0[0x3C]; be<u32> checkPlayerGuard; }; WWHD_OFFSET(PlayerVtable,checkPlayerGuard,0x3C); WWHD_SIZE(PlayerVtable,0x40);
inline BOOL playerGuard(fopAc_ac_c* p) { return gabi::call_ptr<BOOL>(gabi::at<PlayerVtable>(p->__vtbl)->checkPlayerGuard,p); }
inline Player* player(fopAc_ac_c* p) { return reinterpret_cast<Player*>(p); }
inline Attention* attention(dAttention_c* p) { return reinterpret_cast<Attention*>(p); }
inline dCcD_GObjInf* hitObject(u32 address) { return gabi::at<dCcD_GObjInf>(address); }
inline cXyz* lineCross(dBgS_LinChk_l* p) { return &reinterpret_cast<dBgS_LinChk*>(p)->mLin.mEnd; }
inline cBgS_PolyInfo* linePoly(dBgS_LinChk_l* p) { return static_cast<cBgS_PolyInfo*>(reinterpret_cast<dBgS_LinChk*>(p)); }
inline Path* path(dPath* p) { return reinterpret_cast<Path*>(p); }
inline void* trigTable() { return gabi::at<void>(0x104A44F8); }
}

namespace MtExec {
// HD: constructor metadata and the private actor-search callback owned by the main TU.
enum : u32 {
    LAVA_CHECK_VTABLE = 0x1001555C,
    LAVA_GROUP_VTABLE = 0x1001557C,
    LAVA_PASS_VTABLE = 0x1001558C,
    LAVA_POLY_VTABLE = 0x1001556C,
    GROUND_POLY_VTABLE = 0x100154EC,
    OBJECT_PASS_VTABLE = 0x1001550C,
    OBJECT_GROUP_VTABLE = 0x100154CC,
    OBJECT_LINE_VTABLE = 0x1001560C,
    OBJECT_LINE_GROUP_VTABLE = 0x1001562C,
    OBJECT_LINE_PASS_VTABLE = 0x1001563C,
    OBJECT_LINE_POLY_VTABLE = 0x1001561C,
    MAGTAIL_SEARCH_CALLBACK = 0x021D7480,
};
/* csXyz::operator+ (0201A4DC): the 6-byte result comes back in r3:r4 */
static inline void csXyz_pl(csXyz* res, const csXyz* a, const csXyz* b) {
    mtExec_addAngles(a, b);
    u32 r3 = gabi::cpu->r[3], r4 = gabi::cpu->r[4];
    res->x = (s16)(r3 >> 16);
    res->y = (s16)r3;
    res->z = (s16)(r4 >> 16);
}
}

namespace MtExec {
enum Action { ACTION_MOVE=0, ACTION_FIGHT=1, ACTION_ROLL=2, ACTION_DESTROYED=3, ACTION_CARRIED=6 };
enum : u32 { ATTACK_LIGHT_ARROW=0x100000, ATTACK_ICE_ARROW=0x80000, ATTENTION_ACTION_TARGET=0x20000000 };
}
