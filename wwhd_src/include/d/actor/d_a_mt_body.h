#pragma once
#include "d/actor/d_a_mt.h"

// HD: the local ground check retains this translation unit's vtables and store order.
/* dBgS_GndChk on the stack (HD layout, as in d_a_mo2; this TU's vtables) */
struct mtBodyGroundCheck {
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
};
WWHD_SIZE(mtBodyGroundCheck, 0x54);
static inline void mt_GndChk_ct(mtBodyGroundCheck* c) {
    gabi::call(0x02008E0C, c); /* cBgS_GndChk::cBgS_GndChk */
    for (int i = 0; i < 7; i++)
        c->mPass[i] = 0;
    c->mpGrpPassChk = gabi::ea(c) + 0x4C;
    c->__vtbl_4C = 0x100154FC;
    c->mpPolyPassChk = gabi::ea(c) + 0x40;
    c->__vtbl_40 = 0x1001550C;
    c->__vtbl_20 = 0x100154EC;
    c->mGrp = 1;
    c->__vtbl_10 = 0x100154DC;
}
static inline void mt_GndChk_dt(mtBodyGroundCheck* c) {
    c->__vtbl_20 = 0x100154EC;
    c->__vtbl_40 = 0x1001550C;
    c->__vtbl_4C = 0x100154CC;
    gabi::call(0x02008DAC, c, 0); /* cBgS_Chk::~cBgS_Chk */
}

// HD: emitter members after the particle lists are shifted relative to GameCube.
struct mtBodyEmitterView {
    u8 _00[0x5C];
    be<s32> mMaxFrame;
    u8 _60[0x1F0 - 0x60];
    Mtx34 mGlobalRotation;
    cXyz mGlobalDynamicsScale;
    cXyz mGlobalTranslation;
    cXyz mGlobalParticleScale;
    GXColor mGlobalPrmColor;
    GXColor mGlobalEnvColor;
    u8 _24C[8];
    be<u32> mStatus;
};
WWHD_OFFSET(mtBodyEmitterView, mGlobalTranslation, 0x22C);
WWHD_OFFSET(mtBodyEmitterView, mGlobalPrmColor, 0x244);
WWHD_OFFSET(mtBodyEmitterView, mStatus, 0x254);
WWHD_SIZE(mtBodyEmitterView, 0x258);

// HD: named body-control view; the shared actor layout remains available to companion units.
struct mtBodyActorView : fopEn_enemy_c {
    u8 _pad3C8[0x18];
    /* 0x3E0 */ gptr<mDoExt_McaMorf> mpMorf[MT_PART_NUM];
    /* 0x400 */ be<u8> mTextureFrameMode;
    u8 _pad401[0x4B];
    /* 0x44C */ be<f32> mBodyWaveAmplitude;
    u8 _pad450[0x4];
    /* 0x454 */ be<f32> m454;
    u8 _pad458[0xC];
    /* 0x464 */ be<u8> mWaterSteamState;
    u8 _pad465[0x1];
    /* 0x466 */ be<s16> mWaterSteamTimer;
    u8 _pad468[0x104];
    /* 0x56C */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x570 */ be<u8> mAction;
    /* 0x571 */ be<u8> mRollMode;
    /* 0x572 */ be<s16> mRollCollisionTimer;
    /* 0x574 */ be<s16> mActionTimer;
    u8 _pad576[0x6];
    /* 0x57C */ be<s16> mShrinkColliderTimer;
    u8 _pad57E[0x2];
    /* 0x580 */ be<s16> mRollingStateTimer;
    /* 0x582 */ be<s16> mCrawlPhase;
    /* 0x584 */ be<s16> mHeadWobbleAngle;
    /* 0x586 */ be<s16> mBodyWavePhase;
    u8 _pad588[0x4];
    /* 0x58C */ be<f32> mModelOffsetZ;
    /* 0x590 */ be<f32> mBodyWaveScale;
    /* 0x594 */ be<f32> mCrawlSegmentOffset;
    u8 _pad598[0xE];
    /* 0x5A6 */ be<s16> mSegmentPitchStep;
    u8 _pad5A8[0x2];
    /* 0x5AA */ be<s16> mSuppressBodyWave;
    u8 _pad5AC[0x10];
    /* 0x5BC */ cXyz mSegmentPositions[MT_PART_NUM];
    /* 0x61C */ cXyz mPreviousSegmentPositions[MT_PART_NUM];
    /* 0x67C */ csXyz mSegmentAngles[MT_PART_NUM];
    /* 0x6AC */ cXyz mSegmentSpeeds[MT_PART_NUM];
    /* 0x70C */ be<s16> mScatterDelay[MT_PART_NUM];
    /* 0x71C */ be<f32> mSegmentScales[MT_PART_NUM];
    /* 0x73C */ be<f32> mSegmentScaleY[MT_PART_NUM];
    u8 _pad75C[0xB4];
    /* 0x810 */ cXyz mTrailPositions[MT_JOINT_NUM];
    /* 0xB10 */ csXyz mTrailAngles[MT_JOINT_NUM];
    u8 _padC90[0x80];
    /* 0xD10 */ be<s32> mTrailState;
    u8 _padD14[0x8];
    /* 0xD1C */ be<u8> mTrailTableSelect;
    /* 0xD1D */ be<u8> mWriteTrail;
    u8 _padD1E[0x2];
    /* 0xD20 */ be<u8> mHeadColliderMode;
    u8 _padD21[0x43];
    /* 0xD64 */ dBgS_ObjAcch mAcch;
    u8 _padF28[0x3C];
    /* 0xF64 */ dCcD_Sph mSph[MT_PART_NUM];
    /* 0x18C4 */ dCcD_Sph mAtSph;
    u8 _pad19F0[0x1C];
    /* 0x1A0C */ be<f32> mHeadScale;
    /* 0x1A10 */ be<f32> mSegmentSpeedScale;
    /* 0x1A14 */ be<s8> mImpactState;
    u8 _pad1A15[0x1];
    /* 0x1A16 */ be<u8> mSuppressUnrollHop;
    u8 _pad1A17[0x1];
    /* 0x1A18 */ be<s16> mCrawlPitchFlags;
    u8 _pad1A1A[0x3BE];
};
WWHD_SIZE(mtBodyActorView, 0x1DD8);
WWHD_OFFSET(mtBodyActorView, mSegmentSpeedScale, 0x1A10);
WWHD_OFFSET(mtBodyActorView, mImpactState, 0x1A14);
WWHD_OFFSET(mtBodyActorView, mSuppressUnrollHop, 0x1A16);
WWHD_OFFSET(mtBodyActorView, mCrawlPitchFlags, 0x1A18);
WWHD_OFFSET(mtBodyActorView, mTextureFrameMode, 0x400);
WWHD_OFFSET(mtBodyActorView, mBodyWaveAmplitude, 0x44C);
WWHD_OFFSET(mtBodyActorView, mWaterSteamState, 0x464);
WWHD_OFFSET(mtBodyActorView, mWaterSteamTimer, 0x466);
WWHD_OFFSET(mtBodyActorView, mAction, 0x570);
WWHD_OFFSET(mtBodyActorView, mRollMode, 0x571);
WWHD_OFFSET(mtBodyActorView, mRollCollisionTimer, 0x572);
WWHD_OFFSET(mtBodyActorView, mActionTimer, 0x574);
WWHD_OFFSET(mtBodyActorView, mShrinkColliderTimer, 0x57C);
WWHD_OFFSET(mtBodyActorView, mRollingStateTimer, 0x580);
WWHD_OFFSET(mtBodyActorView, mCrawlPhase, 0x582);
WWHD_OFFSET(mtBodyActorView, mHeadWobbleAngle, 0x584);
WWHD_OFFSET(mtBodyActorView, mBodyWavePhase, 0x586);
WWHD_OFFSET(mtBodyActorView, mModelOffsetZ, 0x58C);
WWHD_OFFSET(mtBodyActorView, mBodyWaveScale, 0x590);
WWHD_OFFSET(mtBodyActorView, mCrawlSegmentOffset, 0x594);
WWHD_OFFSET(mtBodyActorView, mSegmentPitchStep, 0x5A6);
WWHD_OFFSET(mtBodyActorView, mSuppressBodyWave, 0x5AA);
WWHD_OFFSET(mtBodyActorView, mSegmentPositions, 0x5BC);
WWHD_OFFSET(mtBodyActorView, mPreviousSegmentPositions, 0x61C);
WWHD_OFFSET(mtBodyActorView, mSegmentAngles, 0x67C);
WWHD_OFFSET(mtBodyActorView, mSegmentSpeeds, 0x6AC);
WWHD_OFFSET(mtBodyActorView, mScatterDelay, 0x70C);
WWHD_OFFSET(mtBodyActorView, mSegmentScales, 0x71C);
WWHD_OFFSET(mtBodyActorView, mSegmentScaleY, 0x73C);
WWHD_OFFSET(mtBodyActorView, mTrailPositions, 0x810);
WWHD_OFFSET(mtBodyActorView, mTrailAngles, 0xB10);
WWHD_OFFSET(mtBodyActorView, mTrailState, 0xD10);
WWHD_OFFSET(mtBodyActorView, mTrailTableSelect, 0xD1C);
WWHD_OFFSET(mtBodyActorView, mWriteTrail, 0xD1D);
WWHD_OFFSET(mtBodyActorView, mHeadColliderMode, 0xD20);

struct mtBodyHIOView {
    u8 _pad0[0x18];
    /* 0x18 */ be<f32> mGroundClearance;
    /* 0x1C */ be<f32> mHeadScale;
    /* 0x20 */ be<f32> mCrawlHeadScale;
    u8 _pad24[0x1C];
    /* 0x40 */ be<f32> mHeadColliderRadius;
    /* 0x44 */ be<f32> mAttackColliderRadius;
    /* 0x48 */ be<f32> mSegmentColliderRadius;
    u8 _pad4C[0x6];
    /* 0x52 */ be<s16> mUnrollPitchStep;
    u8 _pad54[0x4];
    /* 0x58 */ be<f32> mThrowSpeedScale;
    /* 0x5C */ be<f32> mRollingFriction;
};
WWHD_SIZE(mtBodyHIOView, 0x60);
inline mtBodyHIOView& bodyHIO() { return *gabi::at<mtBodyHIOView>(gabi::ea(&l_HIO())); }

inline void mtBodyScaleMatrix(Mtx34* m, f32 x, f32 y, f32 z) { gabi::call(0x028E945C, m, x, y, z); }

struct mtBodyLineCrossView {
    u8 _00[0x30];
    cXyz mCross;
};
WWHD_OFFSET(mtBodyLineCrossView, mCross, 0x30);
WWHD_SIZE(mtBodyLineCrossView, 0x3C);
inline const cXyz& bodyLineCross(dBgS_LinChk_l* check) {
    return gabi::at<mtBodyLineCrossView>(gabi::ea(check))->mCross;
}
struct mtBodyPlayPauseView {
    u8 _00[0x5292];
    be<u8> mPauseState;
};
WWHD_OFFSET(mtBodyPlayPauseView, mPauseState, 0x5292);
WWHD_SIZE(mtBodyPlayPauseView, 0x5293);
inline u8 bodyPauseState() { return gabi::at<mtBodyPlayPauseView>(dComIfGp_ea())->mPauseState; }
